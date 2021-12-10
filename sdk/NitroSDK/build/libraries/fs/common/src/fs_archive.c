/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FS - libraries
  File:     fs_archive.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/


#include <nitro/mi.h>
#include <nitro/os.h>

#include <nitro/fs.h>

#include "../include/util.h"
#include "../include/command.h"


/* アーカイブは基本的に ARM7 に含まない */
#if defined(FS_IMPLEMENT)


/*****************************************************************************/
/* variable */

/* アーカイブリスト */
static FSArchive *arc_list = NULL;

/* カレントディレクトリの位置 */
FSDirPos current_dir_pos;


/*****************************************************************************/
/* function */

/*---------------------------------------------------------------------------*
  Name:         FSi_GetPackedName

  Description:  内部関数.
                指定文字数を小文字にして u32 にパックする.
                長さが FS_ARCHIVE_NAME_LEN_MAX を上回る場合は 0 を返す.

  Arguments:    name             パックする名前の文字列.
                name_len         name の文字列長.

  Returns:      パックされた名前.
 *---------------------------------------------------------------------------*/
static u32 FSi_GetPackedName(const char *name, int name_len)
{
    u32     ret = 0;
    if (name_len <= FS_ARCHIVE_NAME_LEN_MAX)
    {
        int     i = 0;
        for (; i < name_len; ++i)
        {
            u32     c = (u32)MI_ReadByte(name + i);
            if (!c)
                break;
            c = (u32)(c - 'A');
            if (c <= (u32)('Z' - 'A'))
                c = (u32)(c + 'a');
            else
                c = (u32)(c + 'A');
            ret |= (u32)(c << (i * 8));
        }
    }
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ReadMemCallback

  Description:  内部関数.
                デフォルトのメモリリードコールバック.

  Arguments:    p_arc            操作するアーカイブ.
                dst              読み込むメモリの格納先.
                pos              読み込み位置.
                size             読み込みサイズ.

  Returns:      常に FS_RESULT_SUCCESS.
 *---------------------------------------------------------------------------*/
static FSResult FSi_ReadMemCallback(FSArchive *p_arc, void *dst, u32 pos, u32 size)
{
    MI_CpuCopy8((const void *)FS_GetArchiveOffset(p_arc, pos), dst, size);
    return FS_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_WriteMemCallback

  Description:  内部関数.
                デフォルトのメモリライトコールバック.

  Arguments:    p_arc            操作するアーカイブ.
                dst              書き込むメモリの参照先.
                pos              書き込み位置.
                size             書き込みサイズ.

  Returns:      常に FS_RESULT_SUCCESS.
 *---------------------------------------------------------------------------*/
static FSResult FSi_WriteMemCallback(FSArchive *p_arc, const void *src, u32 pos, u32 size)
{
    MI_CpuCopy8(src, (void *)FS_GetArchiveOffset(p_arc, pos), size);
    return FS_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ReadMemoryCore

  Description:  内部関数.
                プリロードテーブルから読み込み.

  Arguments:    p_arc            操作するアーカイブ.
                dst              読み込むメモリの格納先.
                pos              読み込み位置.
                size             読み込みサイズ.

  Returns:      常に FS_RESULT_SUCCESS.
 *---------------------------------------------------------------------------*/
static FSResult FSi_ReadMemoryCore(FSArchive *p_arc, void *dst, u32 pos, u32 size)
{
    (void)p_arc;
    MI_CpuCopy8((const void *)pos, dst, size);
    return FS_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_NextCommand

  Description:  内部関数.
                次に処理すべきコマンドを選択する.
                非同期コマンドが選択されたらそのポインタを返す.
                NULL 以外が返された場合はその場で処理が必要.

  Arguments:    p_arc            次のコマンドを取得するアーカイブ.

  Returns:      この場で処理を必要とする次のコマンド.
 *---------------------------------------------------------------------------*/
FSFile *FSi_NextCommand(FSArchive *p_arc)
{
    /* キャンセル, コマンド選択のために割り込みを禁止 */
    OSIntrMode bak_psr = OS_DisableInterrupts();

    /* まず, 全てのキャンセル要求をチェック */
    if (FSi_IsArchiveCanceling(p_arc))
    {
        FSFile *p, *q;
        p_arc->flag &= ~FS_ARCHIVE_FLAG_CANCELING;
        for (p = p_arc->list.next; p; p = q)
        {
            q = p->link.next;
            if (FS_IsCanceling(p))
            {
                if (p_arc->list.next == p)
                    p_arc->list.next = q;
                FSi_ReleaseCommand(p, FS_RESULT_CANCELED);
                if (!q)
                    q = p_arc->list.next;
            }
        }
    }

    /* サスペンド中でなければ次のコマンドを選択 */
    if (!FSi_IsArchiveSuspending(p_arc) && !FS_IsArchiveSuspended(p_arc) && p_arc->list.next)
    {
        FSFile *p_file = p_arc->list.next;
        const BOOL is_start = !FSi_IsArchiveRunning(p_arc);
        if (is_start)
            p_arc->flag |= FS_ARCHIVE_FLAG_RUNNING;
        (void)OS_RestoreInterrupts(bak_psr);
        if (is_start)
        {
            /*
             * ACTIVATE メッセージ通知.
             * (返り値は意味を持たない)
             */
            if ((p_arc->proc_flag & FS_ARCHIVE_PROC_ACTIVATE) != 0)
                (void)(*p_arc->proc) (p_file, FS_COMMAND_ACTIVATE);
        }
        bak_psr = OS_DisableInterrupts();
        p_file->stat |= FS_FILE_STATUS_OPERATING;
        /* 同期コマンドなら待機スレッドに移譲 */
        if (FS_IsFileSyncMode(p_file))
        {
            OS_WakeupThread(p_file->queue);
            (void)OS_RestoreInterrupts(bak_psr);
            return NULL;
        }
        /* 非同期コマンドなら呼び出し元に処理させる */
        else
        {
            (void)OS_RestoreInterrupts(bak_psr);
            return p_file;
        }
    }

    /* 結局コマンドを実行しない場合はアイドル状態へ */
    if (FSi_IsArchiveRunning(p_arc))
    {
        p_arc->flag &= ~FS_ARCHIVE_FLAG_RUNNING;
        if ((p_arc->proc_flag & FS_ARCHIVE_PROC_IDLE) != 0)
        {
            FSFile  tmp;
            FS_InitFile(&tmp);
            tmp.arc = p_arc;
            /*
             * IDLE メッセージ通知.
             * (返り値は意味を持たない)
             */
            (void)(*p_arc->proc) (&tmp, FS_COMMAND_IDLE);
        }
    }

    /* サスペンド移行に伴うアイドル状態であれば実行者を起こす */
    if (FSi_IsArchiveSuspending(p_arc))
    {
        p_arc->flag &= ~FS_ARCHIVE_FLAG_SUSPENDING;
        p_arc->flag |= FS_ARCHIVE_FLAG_SUSPEND;
        OS_WakeupThread(&p_arc->stat_q);
    }
    (void)OS_RestoreInterrupts(bak_psr);

    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ExecuteAsyncCommand

  Description:  内部関数.
                非同期系コマンドの実行.
                最初の 1 回はユーザスレッドから割り込み許可で呼ばれる.
                アーカイブが同期的に動作する限りここでコマンド処理を繰り返し,
                1 回でも非同期処理になれば続きは NotifyAsyncEnd() で行う.

                よって, アーカイブ処理が同期 / 非同期で切り替わる場合は
                NotifyAsyncEnd() の呼び出し環境に注意する必要がある.

  Arguments:    p_file           実行する非同期コマンドを格納した FSFile 構造体.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FSi_ExecuteAsyncCommand(FSFile *p_file)
{
    FSArchive *const p_arc = p_file->arc;
    while (p_file)
    {
        OSIntrMode bak_psr = OS_DisableInterrupts();
        /* 呼び出し元で同期待ち中ならそれを起こして処理を渡す */
        p_file->stat |= FS_FILE_STATUS_OPERATING;
        if (FS_IsFileSyncMode(p_file))
        {
            OS_WakeupThread(p_file->queue);
            (void)OS_RestoreInterrupts(bak_psr);
            break;
        }
        /* そうでなければここで非同期モード確定 */
        else
        {
            p_file->stat |= FS_FILE_STATUS_ASYNC;
        }
        (void)OS_RestoreInterrupts(bak_psr);
        /* 処理が非同期なら一時終了 */
        if (FSi_TranslateCommand(p_file, p_file->command) == FS_RESULT_PROC_ASYNC)
            break;
        /* 結果が同期完了ならここで続きを選択 */
        p_file = FSi_NextCommand(p_arc);
    }
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ExecuteSyncCommand

  Description:  内部関数.
                同期モードにおけるコマンド実行.

  Arguments:    p_file           実行する同期コマンドを格納した FSFile 構造体.

  Returns:      コマンドが成功すれば TRUE.
 *---------------------------------------------------------------------------*/
BOOL FSi_ExecuteSyncCommand(FSFile *p_file)
{
    FSFile *p_target;
    FSResult ret = FSi_TranslateCommand(p_file, p_file->command);
    FSi_ReleaseCommand(p_file, ret);
    p_target = FSi_NextCommand(p_file->arc);
    if (p_target)
        FSi_ExecuteAsyncCommand(p_target);
    return FS_IsSucceeded(p_file);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_SendCommand

  Description:  内部関数.
                アーカイブへコマンドを発行する.
                起動タイミングの調整とともに, 同期系ならここでブロッキング.

  Arguments:    p_file           コマンド引数を指定された FSFile 構造体.
                command          コマンド ID.

  Returns:      コマンドが成功すれば TRUE.
 *---------------------------------------------------------------------------*/
BOOL FSi_SendCommand(FSFile *p_file, FSCommandType command)
{
    FSArchive *const p_arc = p_file->arc;
    const int bit = (1 << command);

    /*
     * ファイルそのもののスレッドセーフまでは SDK で保証しない.
     * また, 状態判定は呼び出し元のインタフェースで完了済み.
     */
    p_file->command = command;
    p_file->error = FS_RESULT_BUSY;
    p_file->stat |= FS_FILE_STATUS_BUSY;

    {
        /* リスト追加と起動判定のために割り込みを禁止 */
        OSIntrMode bak_psr = OS_DisableInterrupts();

        /* アーカイブがアンロード中ならキャンセル */
        if (p_arc->flag & FS_ARCHIVE_FLAG_UNLOADING)
        {
            FSi_ReleaseCommand(p_file, FS_RESULT_CANCELED);
            (void)OS_RestoreInterrupts(bak_psr);
            return FALSE;
        }

        /* リストに追加 */
        if ((bit & FS_ARCHIVE_PROC_SYNC) != 0)
            p_file->stat |= FS_FILE_STATUS_SYNC;
        FSi_AppendToList(p_file, (FSFile *)&p_arc->list);

        /* アイドル中ならアーカイブを起動する */
        if (!FS_IsArchiveSuspended(p_arc) && !FSi_IsArchiveRunning(p_arc))
        {
            p_arc->flag |= FS_ARCHIVE_FLAG_RUNNING;
            (void)OS_RestoreInterrupts(bak_psr);
            /*
             * ACTIVATE メッセージ通知.
             * (返り値は意味を持たない)
             */
            if ((p_arc->proc_flag & FS_ARCHIVE_PROC_ACTIVATE) != 0)
                (void)(*p_arc->proc) (p_file, FS_COMMAND_ACTIVATE);
            /* 非同期モードならコマンドを実行してすぐ戻る */
            bak_psr = OS_DisableInterrupts();
            p_file->stat |= FS_FILE_STATUS_OPERATING;
            if (!FS_IsFileSyncMode(p_file))
            {
                (void)OS_RestoreInterrupts(bak_psr);
                FSi_ExecuteAsyncCommand(p_file);
                return TRUE;
            }
            (void)OS_RestoreInterrupts(bak_psr);
        }

        /* 起動中かつ非同期モードなら後を流れに任せる */
        else if (!FS_IsFileSyncMode(p_file))
        {
            (void)OS_RestoreInterrupts(bak_psr);
            return TRUE;
        }

        /* 起動中かつ同期モードであればここで順番待ち */
        else
        {
            do
            {
                OS_SleepThread(p_file->queue);
            }
            while (!(p_file->stat & FS_FILE_STATUS_OPERATING));
            (void)OS_RestoreInterrupts(bak_psr);
        }

    }

    /*
     * 同期モードは全てここに来て, この場で完了する.
     * ReadFileAsync + WaitAsync などもここへ来る.
     */
    return FSi_ExecuteSyncCommand(p_file);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_EndArchive

  Description:  全てのアーカイブを終了させて解放する.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FSi_EndArchive(void)
{
    OSIntrMode bak_psr = OS_DisableInterrupts();
    FSArchive *volatile *p_list = &arc_list;
    for (;;)
    {
        FSArchive *p_arc = arc_list;
        if (!p_arc)
            break;
        arc_list = arc_list->next;
        (void)FS_UnloadArchive(p_arc);
        FS_ReleaseArchiveName(p_arc);
    }
    (void)OS_RestoreInterrupts(bak_psr);
}

/*---------------------------------------------------------------------------*
  Name:         FS_InitArchive

  Description:  アーカイブ構造体を初期化.

  Arguments:    p_arc            初期化するアーカイブ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FS_InitArchive(FSArchive *p_arc)
{
    FS_ASSERT_ARG(p_arc, FALSE);
    MI_CpuClear8(p_arc, sizeof(FSArchive));
#if !defined(SDK_NO_THREAD)
    OS_InitThreadQueue(&p_arc->sync_q);
    OS_InitThreadQueue(&p_arc->stat_q);
#endif
}

/*---------------------------------------------------------------------------*
  Name:         FS_FindArchive

  Description:  アーカイブ名を検索する.
                一致する名前が無ければ NULL を返す.

  Arguments:    name             検索するアーカイブ名の文字列.
                name_len         name の文字列長.

  Returns:      検索して見つかったアーカイブのポインタか NULL.
 *---------------------------------------------------------------------------*/
FSArchive *FS_FindArchive(const char *name, int name_len)
{
    u32     pack = FSi_GetPackedName(name, name_len);
    OSIntrMode bak_psr = OS_DisableInterrupts();
    FSArchive *p_arc = arc_list;
    while (p_arc && (p_arc->name.pack != pack))
        p_arc = p_arc->next;
    (void)OS_RestoreInterrupts(bak_psr);

    return p_arc;
}

/*---------------------------------------------------------------------------*
  Name:         FS_RegisterArchiveName

  Description:  アーカイブ名をファイルシステムへ登録し, 関連付ける.
                アーカイブ自体はまだファイルシステムにロードされない.
                アーカイブ名 "rom" はファイルシステムに予約済み.

  Arguments:    p_arc            名前を関連付けるアーカイブ.
                name             登録する名前の文字列.
                name_len         name の文字列長.

  Returns:      None.
 *---------------------------------------------------------------------------*/
BOOL FS_RegisterArchiveName(FSArchive *p_arc, const char *name, u32 name_len)
{
    FS_ASSERT_INIT(FALSE);
    FS_ASSERT_ARG(p_arc && name, FALSE);
    FS_ASSERT_ARC_FREE(p_arc, FALSE);

    {
        BOOL    ret = FALSE;
        OSIntrMode bak_intr = OS_DisableInterrupts();
        if (!FS_FindArchive(name, (s32)name_len))
        {
            FSArchive *p_tail = arc_list;
            if (!p_tail)
            {
                arc_list = p_arc;
                current_dir_pos.arc = p_arc;
                current_dir_pos.pos = 0;
                current_dir_pos.index = 0;
                current_dir_pos.own_id = 0;
            }
            else
            {
                while (p_tail->next)
                    p_tail = p_tail->next;
                p_tail->next = p_arc;
                p_arc->prev = p_tail;
            }
            p_arc->name.pack = FSi_GetPackedName(name, (s32)name_len);
            p_arc->flag |= FS_ARCHIVE_FLAG_REGISTER;
            ret = TRUE;
        }
        (void)OS_RestoreInterrupts(bak_intr);
        return ret;
    }
}

/*---------------------------------------------------------------------------*
  Name:         FS_ReleaseArchiveName

  Description:  登録済みのアーカイブ名を解放する.
                ファイルシステムからアンロードされている必要がある.

  Arguments:    p_arc            名前を解放するアーカイブ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FS_ReleaseArchiveName(FSArchive *p_arc)
{
    FS_ASSERT_INIT(void);
    FS_ASSERT_ARG(p_arc, void);
    FS_ASSERT_ARC_NOT_ROM(p_arc, void);
    if (p_arc->name.pack)
    {
        OSIntrMode bak_psr = OS_DisableInterrupts();
        // リストから切断.
        if (p_arc->next)
            p_arc->next->prev = p_arc->prev;
        if (p_arc->prev)
            p_arc->prev->next = p_arc->next;
        p_arc->name.pack = 0;
        p_arc->next = p_arc->prev = NULL;
        p_arc->flag &= ~FS_ARCHIVE_FLAG_REGISTER;
        // 解放したアーカイブ名がカレントディレクトリなら,
        //  自動的に "rom" のルートへ移動する.
        if (current_dir_pos.arc == p_arc)
        {
            current_dir_pos.arc = arc_list;
            current_dir_pos.pos = 0;
            current_dir_pos.index = 0;
            current_dir_pos.own_id = 0;
        }
        (void)OS_RestoreInterrupts(bak_psr);
    }
}

/*---------------------------------------------------------------------------*
  Name:         FS_LoadArchive

  Description:  アーカイブをファイルシステムにロードする.
                すでにアーカイブリストに名前が登録されている必要がある.

  Arguments:    p_arc            ロードするアーカイブ.
                base             独自に使用可能な任意の u32 値.
                fat              FAT テーブルの先頭オフセット.
                fat_size         FAT テーブルのサイズ.
                fnt              FNT テーブルの先頭オフセット.
                fnt_size         FNT テーブルのサイズ.
                read_func        リードアクセスコールバック.
                write_func       ライトアクセスコールバック.

  Returns:      アーカイブが正しくロードされれば TRUE.
 *---------------------------------------------------------------------------*/
BOOL FS_LoadArchive(FSArchive *p_arc, u32 base,
                    u32 fat, u32 fat_size,
                    u32 fnt, u32 fnt_size,
                    FS_ARCHIVE_READ_FUNC read_func, FS_ARCHIVE_WRITE_FUNC write_func)
{
    FS_ASSERT_INIT(FALSE);
    FS_ASSERT_ARG(p_arc, FALSE);
    FS_ASSERT_ARC_UNLOADED(p_arc, FALSE);

    // ベースオフセットを設定.
    p_arc->base = base;
    p_arc->fat_size = fat_size;
    p_arc->fat = p_arc->fat_bak = fat;
    p_arc->fnt_size = fnt_size;
    p_arc->fnt = p_arc->fnt_bak = fnt;
    // アクセスコールバック. (NULL ならメモリ扱い)
    p_arc->read_func = read_func ? read_func : FSi_ReadMemCallback;
    p_arc->write_func = write_func ? write_func : FSi_WriteMemCallback;
    // 最初はプリロード無効状態.
    p_arc->table_func = p_arc->read_func;
    p_arc->load_mem = NULL;
    // 
    p_arc->flag |= FS_ARCHIVE_FLAG_LOADED;
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         FS_UnloadArchive

  Description:  アーカイブをファイルシステムからアンロードする.
                現在処理中のタスクが全て完了するまでブロッキングする.

  Arguments:    p_arc            アンロードするアーカイブ.

  Returns:      アーカイブが正しくアンロードされれば TRUE.
 *---------------------------------------------------------------------------*/
BOOL FS_UnloadArchive(FSArchive *p_arc)
{
    FS_ASSERT_INIT(FALSE);
    FS_ASSERT_ARG(p_arc, FALSE);

    {
        OSIntrMode bak_psr = OS_DisableInterrupts();
        /* ロードされていないなら無視 */
        if (FS_IsArchiveLoaded(p_arc))
        {
            /*
             * テーブルをアンロードしないままだと
             * メモリリークの可能性があることを警告.
             */
            if (FS_IsArchiveTableLoaded(p_arc))
            {
                OS_TWarning("memory may leak. preloaded-table of archive \"%s\" (0x%08X)",
                            p_arc->name.ptr, p_arc->load_mem);
            }
            {
                FSFile *p, *q;
                /* まずサスペンド */
                BOOL    bak_state = FS_SuspendArchive(p_arc);
                /* アンロード中フラグを立てて全コマンドをキャンセル */
                p_arc->flag |= FS_ARCHIVE_FLAG_UNLOADING;
                for (p = p_arc->list.next; p; p = q)
                {
                    q = p->link.next;
                    FSi_ReleaseCommand(p, FS_RESULT_CANCELED);
                }
                p_arc->list.next = NULL;
                /* 前状態を保持 */
                if (bak_state)
                    (void)FS_ResumeArchive(p_arc);
            }
            /* 実際のアンロード処理 */
            p_arc->base = 0;
            p_arc->fat = 0;
            p_arc->fat_size = 0;
            p_arc->fnt = 0;
            p_arc->fnt_size = 0;
            p_arc->fat_bak = p_arc->fnt_bak = 0;
            p_arc->flag &= ~(FS_ARCHIVE_FLAG_CANCELING |
                             FS_ARCHIVE_FLAG_LOADED | FS_ARCHIVE_FLAG_UNLOADING);
        }
        (void)OS_RestoreInterrupts(bak_psr);
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         FS_LoadArchiveTables

  Description:  アーカイブの FAT + FNT をメモリ上にプリロードする.
                指定サイズ以内の場合のみ読み込みを実行し, 必要サイズを返す.

  Arguments:    p_arc            テーブルをプリロードするアーカイブ.
                p_mem            テーブルデータの格納先バッファ.
                max_size         p_mem のサイズ.

  Returns:      常に, 合計のテーブルサイズを返す.
 *---------------------------------------------------------------------------*/
u32 FS_LoadArchiveTables(FSArchive *p_arc, void *p_mem, u32 max_size)
{
    FS_ASSERT_INIT(0);
    FS_ASSERT_ARG(p_arc, 0);

    {
        // プリロードサイズは 32 BYTE アライン.
        u32     total_size = ALIGN_BYTE(p_arc->fat_size + p_arc->fnt_size + 32, 32);
        if (total_size <= max_size)
        {
            // サイズが充分ならメモリへロード.
            u8     *p_cache = (u8 *)ALIGN_BYTE((u32)p_mem, 32);
            FSFile  tmp;
            FS_InitFile(&tmp);
            /*
             * テーブルはリードできないこともある.
             * その場合は, 元々テーブルにアクセスできないので何もしない.
             */
            // FAT ロード.
            if (FS_OpenFileDirect(&tmp, p_arc, p_arc->fat, p_arc->fat + p_arc->fat_size, (u32)~0))
            {
                if (FS_ReadFile(&tmp, p_cache, (s32)p_arc->fat_size) < 0)
                {
                    MI_CpuFill8(p_cache, 0x00, p_arc->fat_size);
                }
                (void)FS_CloseFile(&tmp);
            }
            p_arc->fat = (u32)p_cache;
            p_cache += p_arc->fat_size;
            // FNT ロード.
            if (FS_OpenFileDirect(&tmp, p_arc, p_arc->fnt, p_arc->fnt + p_arc->fnt_size, (u32)~0))
            {
                if (FS_ReadFile(&tmp, p_cache, (s32)p_arc->fnt_size) < 0)
                {
                    MI_CpuFill8(p_cache, 0x00, p_arc->fnt_size);
                }
                (void)FS_CloseFile(&tmp);
            }
            p_arc->fnt = (u32)p_cache;
            // 割り当てられたメモリを保存.
            p_arc->load_mem = p_mem;
            // 以降はテーブルリード系でプリロードメモリが働く.
            p_arc->table_func = FSi_ReadMemoryCore;
            p_arc->flag |= FS_ARCHIVE_FLAG_TABLE_LOAD;
        }
        return total_size;
    }
}

/*---------------------------------------------------------------------------*
  Name:         FS_UnloadArchiveTables

  Description:  アーカイブのプリロードメモリを解放する.

  Arguments:    p_arc            プリロードメモリを解放するアーカイブ.

  Returns:      プリロードメモリとしてユーザから与えられていたバッファ.
 *---------------------------------------------------------------------------*/
void   *FS_UnloadArchiveTables(FSArchive *p_arc)
{
    FS_ASSERT_INIT(0);
    FS_ASSERT_ARG(p_arc, 0);

    {
        void   *ret = NULL;
        if (FS_IsArchiveLoaded(p_arc))
        {
            BOOL    bak_stat = FS_SuspendArchive(p_arc);
            if (FS_IsArchiveTableLoaded(p_arc))
            {
                p_arc->flag &= ~FS_ARCHIVE_FLAG_TABLE_LOAD;
                ret = p_arc->load_mem;
                p_arc->load_mem = NULL;
                p_arc->fat = p_arc->fat_bak;
                p_arc->fnt = p_arc->fnt_bak;
                p_arc->table_func = p_arc->read_func;
            }
            if (bak_stat)
                (void)FS_ResumeArchive(p_arc);
        }
        return ret;
    }
}

/*---------------------------------------------------------------------------*
  Name:         FS_SuspendArchive

  Description:  アーカイブの処理機構自体を停止する.
                現在実行中の処理があれば, その完了を待機.

  Arguments:    p_arc            停止するアーカイブ.

  Returns:      呼び出し以前にサスペンド状態でなければ TRUE.
 *---------------------------------------------------------------------------*/
BOOL FS_SuspendArchive(FSArchive *p_arc)
{
    FS_ASSERT_INIT(0);
    FS_ASSERT_ARG(p_arc, 0);

    {
        /* すでにサスペンド中なら無視 */
        OSIntrMode bak_psr = OS_DisableInterrupts();
        const BOOL bak_stat = !FS_IsArchiveSuspended(p_arc);
        if (bak_stat)
        {
            /* 実行中ならフラグを立てて停止を待つ */
            if (FSi_IsArchiveRunning(p_arc))
            {
                p_arc->flag |= FS_ARCHIVE_FLAG_SUSPENDING;
                do
                    OS_SleepThread(&p_arc->stat_q);
                while (FSi_IsArchiveSuspending(p_arc));
            }
            /* アイドル中なら直接停止 */
            else
            {
                p_arc->flag |= FS_ARCHIVE_FLAG_SUSPEND;
            }
        }
        (void)OS_RestoreInterrupts(bak_psr);
        return bak_stat;
    }
}

/*---------------------------------------------------------------------------*
  Name:         FS_ResumeArchive

  Description:  停止していたアーカイブの処理を再開する.

  Arguments:    p_arc            再開するアーカイブ.

  Returns:      呼び出し以前にサスペンド状態でなければ TRUE.
 *---------------------------------------------------------------------------*/
BOOL FS_ResumeArchive(FSArchive *p_arc)
{
    FS_ASSERT_INIT(0);
    FS_ASSERT_ARG(p_arc, 0);

    {
        FSFile *p_target = NULL;
        /* サスペンドフラグを落とす */
        OSIntrMode bak_irq = OS_DisableInterrupts();
        const BOOL bak_stat = !FS_IsArchiveSuspended(p_arc);
        if (!bak_stat)
        {
            p_arc->flag &= ~FS_ARCHIVE_FLAG_SUSPEND;
            /* 起動タイミングならコマンド開始 */
            p_target = FSi_NextCommand(p_arc);
        }
        (void)OS_RestoreInterrupts(bak_irq);
        if (p_target)
            FSi_ExecuteAsyncCommand(p_target);
        return bak_stat;
    }
}

/*---------------------------------------------------------------------------*
  Name:         FS_SetArchiveProc

  Description:  アーカイブのユーザプロシージャを設定する.
                proc == NULL または flags = 0 なら
                単にユーザプロシージャを無効にする.

  Arguments:    p_arc            ユーザプロシージャを設定するアーカイブ.
                proc             ユーザプロシージャ.
                flags            プロシージャへフックするコマンドのビット集合.

  Returns:      常に, 合計のテーブルサイズを返す.
 *---------------------------------------------------------------------------*/
void FS_SetArchiveProc(struct FSArchive *p_arc, FS_ARCHIVE_PROC_FUNC proc, u32 flags)
{
    if (!flags)
        proc = NULL;
    else if (!proc)
        flags = 0;
    p_arc->proc = proc;
    p_arc->proc_flag = flags;
}

/*---------------------------------------------------------------------------*
  Name:         FS_NotifyArchiveAsyncEnd

  Description:  非同期で実行していたアーカイブ処理の完了を通知するために
                アーカイブ実装側から呼び出す.

  Arguments:    p_arc            完了を通知するアーカイブ.
                ret              処理結果.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FS_NotifyArchiveAsyncEnd(FSArchive *p_arc, FSResult ret)
{
    /* 非同期コマンド中なら完了を意味する */
    if (FSi_IsArchiveAsync(p_arc))
    {
        FSFile *p_file = p_arc->list.next;
        p_arc->flag &= ~FS_ARCHIVE_FLAG_IS_ASYNC;
        FSi_ReleaseCommand(p_file, ret);
        /* 次のが非同期コマンドならここで実行 */
        p_file = FSi_NextCommand(p_arc);
        if (p_file)
            FSi_ExecuteAsyncCommand(p_file);
    }
    /* 同期コマンドならブロッキング中のスレッドを復帰 */
    else
    {
        FSFile *p_file = p_arc->list.next;
        OSIntrMode bak_psr = OS_DisableInterrupts();
        p_file->error = ret;
        p_arc->flag &= ~FS_ARCHIVE_FLAG_IS_SYNC;
#if !defined(SDK_NO_THREAD)
        OS_WakeupThread(&p_arc->sync_q);
#endif
        (void)OS_RestoreInterrupts(bak_psr);
    }
}


#endif /* FS_IMPLEMENT */

/*---------------------------------------------------------------------------*
  $Log: fs_archive.c,v $
  Revision 1.34  2006/01/18 02:11:20  kitase_hirotake
  do-indent

  Revision 1.33  2005/06/28 02:11:39  yosizaki
  remove some assertions.

  Revision 1.32  2005/06/17 13:07:41  yosizaki
  fix FS_WaitAsync. (add FS_FILE_STATUS_OPERATING)

  Revision 1.31  2005/05/30 04:17:17  yosizaki
  add FSi_EndArchive().
  add comments.

  Revision 1.30  2005/04/15 08:42:58  yosizaki
  fix assertion macros format. (remove ';' )

  Revision 1.29  2005/02/28 05:26:13  yosizaki
  do-indent.

  Revision 1.28  2005/02/18 07:26:49  yasu
  Signed/Unsigned 変換警告抑制

  Revision 1.27  2005/01/26 02:58:51  yasu
  Copyright 表記の修正

  Revision 1.26  2005/01/25 11:22:23  yosizaki
  fix around Suspend-state.

  Revision 1.25  2004/11/12 13:36:42  yosizaki
  fix around LoadTable (ignore if empty archive)

  Revision 1.24  2004/11/02 10:06:15  yosizaki
  fix typo.

  Revision 1.23  2004/09/02 11:03:00  yosizaki
  fix FS include directives.

  Revision 1.22  2004/07/23 08:29:52  yosizaki
  add cast to OS_LockCard.

  Revision 1.21  2004/07/19 13:17:36  yosizaki
  add all commands.

  Revision 1.20  2004/07/13 08:00:14  yosizaki
  fix FS_ResumeArchive

  Revision 1.19  2004/07/13 02:54:05  yosizaki
  add argument of FS_OpenFileDirect.

  Revision 1.18  2004/07/12 13:04:45  yosizaki
  change FS_COMMAND_READFILE & FS_COMMAND_WRITEFILE (add argument)

  Revision 1.17  2004/07/09 00:56:32  yosizaki
  fix argument of FS_COMMAND_IDLE procedure-message.

  Revision 1.16  2004/07/08 13:42:00  yosizaki
  change archive commands.

  Revision 1.15  2004/06/30 04:32:30  yosizaki
  implement user-procedure system.

  Revision 1.14  2004/06/22 01:48:48  yosizaki
  fix archive command operation.

  Revision 1.13  2004/06/07 10:26:33  yosizaki
  fix around archive asynchronous operation.

  Revision 1.12  2004/06/03 13:33:07  yosizaki
  fix around archive asynchronous operation.

  Revision 1.11  2004/06/01 10:17:56  yosizaki
  change around assertion code.

  Revision 1.10  2004/05/31 02:56:13  yosizaki
  change FSi_ASSERT_ARG() to FS_ASSERT_ARG().

  Revision 1.9  2004/05/21 10:55:27  yosizaki
  change OS_IsThreadAvailable() to SDK_NO_THREAD.

  Revision 1.8  2004/05/20 09:59:22  yosizaki
  fix asynchronous operation.

  Revision 1.7  2004/05/20 06:31:16  yosizaki
  add FS_ConvertPathToFileID.

  Revision 1.6  2004/05/19 02:30:14  yosizaki
  add FS_GetPathName().

  Revision 1.5  2004/05/17 08:32:26  yosizaki
  add comment.
  add assertion of each interface function.

  Revision 1.4  2004/05/12 05:22:55  yosizaki
  prepare a option "SDK_ARM7FS".

  Revision 1.3  2004/05/11 09:36:36  yosizaki
  fix FS_SeekFile().

  Revision 1.2  2004/05/11 04:30:57  yosizaki
  change archive system.

  Revision 1.1  2004/05/10 06:27:56  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
