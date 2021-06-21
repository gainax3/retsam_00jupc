/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - libraries
  File:     mb_fileinfo.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_fileinfo.c,v $
  Revision 1.36  2007/07/31 00:58:42  yosizaki
  update copyright

  Revision 1.35  2007/07/30 03:32:38  yosizaki
  fix about MB_RegisterFile

  Revision 1.34  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.33  2005/11/07 01:21:50  okubata_ryoma
  SDK_STATIC_ASSERTからSDK_COMPILER_ASSERTに変更

  Revision 1.32  2005/05/20 03:10:01  yosizaki
  support MBGameRegistry::userParam.

  Revision 1.31  2005/02/28 05:26:24  yosizaki
  do-indent.

  Revision 1.30  2005/02/21 00:39:34  yosizaki
  replace prefix MBw to MBi.

  Revision 1.29  2005/02/18 11:56:03  yosizaki
  fix around hidden warnings.

  Revision 1.28  2004/12/07 10:20:37  yosizaki
  fix to reload cache from correct archive. (not only "rom")

  Revision 1.27  2004/11/26 09:16:42  yosizaki
  fix MB_ReadSegment (around Debugger's patch-code)

  Revision 1.26  2004/11/22 07:39:07  yosizaki
  fix MB_ReadSegment (around auth-code)

  Revision 1.25  2004/11/11 11:49:06  yosizaki
  fix MB_ReadSegment around cache-mode and auth-code.
  add task-thread system.

  Revision 1.24  2004/11/10 13:16:27  yosizaki
  change MB_ReadSegment to use cache-system.

  Revision 1.23  2004/10/06 05:47:58  sato_masaki
  small fix

  Revision 1.22  2004/09/29 07:15:56  yosizaki
  fix MB_ReadSegment. (add size of auth-code)

  Revision 1.21  2004/09/28 00:13:18  sato_masaki
  bug fix

  Revision 1.20  2004/09/27 11:24:02  sato_masaki
  - IsAbleToLoad() 関数追加。
  - MB_RegisterFileにて、Block情報テーブル作成に失敗しても、GameInfoがBeaconに登録されていたのを修正。

  Revision 1.19  2004/09/25 12:11:53  sato_masaki
  MBi_IsAbleToRecv()を追加。(IsAbleToRecv()をリネームし、mb_child.cから移動。)

  Revision 1.18  2004/09/25 05:30:32  sato_masaki
  メインループにおいて呼ばれ得るAPI実行中に、割り込み禁止をする処理を追加。

  Revision 1.17  2004/09/22 09:41:50  sato_masaki
  MBi_get_blockinfo()に、条件判定追加。

  Revision 1.16  2004/09/20 13:12:20  sato_masaki
  MBi_get_blockinfo()の返り値をBOOL値に変更。

  Revision 1.15  2004/09/17 11:37:17  sato_masaki
  MB_RegisterFile() において、MBが開始されていなかったら、FALSEを返すようにした。

  Revision 1.14  2004/09/16 12:50:37  sato_masaki
  - mb.hにて、MBi_***で定義していた関数、型を、MB_***に変更。
  - MB_UnregisterFileの返り値を、登録したセグメントバッファへのポインタに変更。

  Revision 1.13  2004/09/16 08:44:21  miya
  ファイル更新用メンバ追加

  Revision 1.12  2004/09/15 06:51:59  sato_masaki
  WBT定義に纏わる処理を削除。

  Revision 1.11  2004/09/15 06:25:29  sato_masaki
  MB_COMM_P_SENDLEN, MB_COMM_C_SENDLEN の定義に依存していた部分を変数化。

  Revision 1.10  2004/09/14 15:38:45  yosizaki
  fix MB_ReadSegment

  Revision 1.9  2004/09/11 11:20:48  sato_masaki
  change mbc to pPwork

  Revision 1.8  2004/09/10 03:24:14  yosizaki
  unify file-access to register file.

  Revision 1.7  2004/09/08 11:09:46  miya
  small fix.

  Revision 1.6  2004/09/07 04:38:17  sato_masaki
  - const値、defaultLoadSegListをMBi_defaultLoadSegListにリネームし、非static化。
  - MB_BlockInfoの要素追加に対応。

  Revision 1.5  2004/09/06 11:51:52  sato_masaki
  GameInfoへのfileNoの設定を、MB_RegisterFile()内で行うように変更。

  Revision 1.4  2004/09/06 06:23:03  yosizaki
  fix around MBi_ReadSegmentHeader().

  Revision 1.3  2004/09/06 03:29:21  yosizaki
  -add routine about reading segment header.
  -change MB_RegisterFile() in order to send own program too.

  Revision 1.2  2004/09/04 11:03:18  sato_masaki
  ROM_HEADER_SIZE_FULLの定義をmb_fileinfo.ｈへ移動。

  Revision 1.1  2004/09/03 07:04:36  sato_masaki
  ファイルを機能別に分割。


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include "mb_private.h"

// definition

/*
 * キャッシュページ単位.
 * 最初は親機ヘッダ部分の 16kB にしていたが,
 * 境界をまたいだ場合に問題があるので 17kB に増加.
 */
#define	MB_CACHE_PAGE_SIZE	(17 * 1024)

/* MB_ReadSegment() 所要バッファ定義値の正当性判定 */
SDK_COMPILER_ASSERT(ROM_HEADER_SIZE_FULL +
                    MB_AUTHCODE_SIZE +
                    sizeof(MBiCacheList) +
                    sizeof(u32) * 4 + MB_CACHE_PAGE_SIZE * 3 <= MB_SEGMENT_BUFFER_MIN);

// constant variables

/* 
    デフォルトのセグメントリスト
    セグメントのタイプおよび格納順は、この形で固定とする。
 */
const MbSegmentType MBi_defaultLoadSegList[MB_DL_SEGMENT_NUM] = {
    MB_SEG_ROMHEADER,                  /* ROM HEADER  */
    MB_SEG_ARM9STATIC,                 /* ARM9 static */
    MB_SEG_ARM7STATIC,                 /* ARM7 static */
};


/* セグメントのヘッダ領域 */
#define	MB_SEGMENT_HEADER_MIN	0x4000
#define	MB_SEGMENT_HEADER_MAX	0x8000


/* 有効なセグメントヘッダ領域. (将来可変にする場合は設定関数を用意) */
static const CARDRomRegion mbi_seg_header_default[] = {
    {0x4000, 0x1000,},
    {0x7000, 0x1000,},
    {0, 0},
};
static const CARDRomRegion *mbi_seg_header = mbi_seg_header_default;

/* セグメントのヘッダ部をリードする際に必要な情報 */
typedef struct
{
    u32     rom_src;
    u32     mem_src;
    u32     mem_dst;
    u32     len;
}
MBiSegmentHeaderInfo;


// ----------------------------------------------------------------------------
// static funciton's prototypes

static void MBi_MakeDownloadFileInfo(MbDownloadFileInfoHeader * mbdlFileInfop, const void *buf);
static void MBi_SetSegmentInfo(const RomHeader * mbRomHeaderp, const MbSegmentType * loadSegListp,
                               MbSegmentInfo * seg_infop, u32 *child_recv_buff_addr);
static void MBi_ReadSegmentHeader(const MBiSegmentHeaderInfo * p, u32 top, u32 bottom, BOOL clean);
static BOOL IsAbleToLoad(u8 segment_no, u32 address, u32 size);

/*---------------------------------------------------------------------------*
  Name:         MB_GetSegmentLength

  Description:  指定されたバイナリファイルに必要なセグメントバッファ長を取得

  Arguments:    file : バイナリファイルを指す FSFile 構造体
                       NULL の場合, 自身を使用する. (親子共用バイナリ)
  
  Returns:      正常にサイズを取得できれば正の値を, そうでなければ 0
 *---------------------------------------------------------------------------*/

u32 MB_GetSegmentLength(FSFile *file)
{
    enum
    { ROM_HEADER_SIZE_SMALL = 0x60 };
    u8      rom[ROM_HEADER_SIZE_SMALL];
    const RomHeader *p = NULL;

    u32     ret = 0;

    SDK_ASSERT(!file || FS_IsFile(file));

    /* 単体のバイナリファイルを指定 */
    if (file)
    {
        /* 現在の位置を記憶 */
        const u32 bak_pos = FS_GetPosition(file);
        /* とりあえず ROM ヘッダ分読み込み */
        if (FS_ReadFile(file, &rom, sizeof(rom)) >= sizeof(rom))
        {
            p = (const RomHeader *)rom;
        }
        /* 最初の位置を復帰 */
        (void)FS_SeekFile(file, (int)bak_pos, FS_SEEK_SET);
    }
    /* 親子共用バイナリ */
    else
    {
        p = (const RomHeader *)HW_ROM_HEADER_BUF;
    }
    if (p)
    {
        ret = (u32)(ROM_HEADER_SIZE_FULL +      /* ROM ヘッダ */
                    MB_AUTHCODE_SIZE + /* auth コード */
                    sizeof(MBiCacheList) +      /* キャッシュ構造体 */
                    sizeof(u32) * 4 +  /* segment - CARD マッピング */
                    p->main_size +     /* ARM9 static セグメント */
                    p->sub_size        /* ARM7 static セグメント */
            );
        if (ret < MB_SEGMENT_BUFFER_MIN)
            ret = MB_SEGMENT_BUFFER_MIN;
    }
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         MB_ReadSegment

  Description:  指定されたバイナリファイルから必要なセグメントデータを読み出し.

  Arguments:    file : バイナリファイルを指す FSFile 構造体
                       NULL の場合, 自身を使用する. (親子共用バイナリ)
                buf : セグメントデータを読み出すバッファ
				len : buf のサイズ
  
  Returns:      正常にセグメントデータを読み出せれば TRUE, そうでなければ FALSE
 *---------------------------------------------------------------------------*/

BOOL MB_ReadSegment(FSFile *file, void *buf, u32 len)
{
    BOOL    ret = FALSE;
    SDK_ASSERT(!file || FS_IsFile(file));

    /*
     * 指定されたバッファを以下のフォーマットで切り出す.
     *
     * [RomHeader]
     * [AuthCode]
     * [CacheList]
     * [CardMapping]
     * [ARM9 static]
     * [ARM7 static]
     *
     * 最後の 2 つのセグメントは, サイズが不足していれば
     * キャッシュ方式で仮想メモリを実現して対応.
     */

    if (len >= ROM_HEADER_SIZE_FULL + 4)
    {

        BOOL    is_own_binary = FALSE;

        u32     rest = len;
        u8     *dst = (u8 *)buf;
        u32     bak_pos;
        FSFile  own_bin[1];
        u32     own_size;
        RomHeader *p_rom;

        MBiCacheList *p_cache = NULL;
        u32    *p_map = NULL;

        p_rom = (RomHeader *) dst;
        dst += ROM_HEADER_SIZE_FULL, rest -= ROM_HEADER_SIZE_FULL;

        /* 通常のマルチブート */
        if (file)
        {
            bak_pos = FS_GetPosition(file);
            if (FS_ReadFile(file, p_rom, ROM_HEADER_SIZE_FULL) < ROM_HEADER_SIZE_FULL)
                rest = 0;
            own_size = p_rom->own_size;
            if (!own_size)
                own_size = 16 * 1024 * 1024;
        }
        /* クローンブート */
        else
        {
            const RomHeader *mem_rom = (RomHeader *) HW_ROM_HEADER_BUF;
            own_size = mem_rom->own_size;
            if (!own_size)
                own_size = 16 * 1024 * 1024;
            is_own_binary = TRUE;
            /* 自身を表すファイルを直接開く */
            FS_InitFile(own_bin);
            (void)FS_OpenFileDirect(own_bin,
                                    FS_FindArchive("rom", 3), 0, (u32)(own_size + MB_AUTHCODE_SIZE),
                                    (u32)~0);
            file = own_bin;
            bak_pos = FS_GetPosition(file);
            /* とりあえず ROM ヘッダ分コピー */
            MI_CpuCopy8(mem_rom, p_rom, ROM_HEADER_SIZE_FULL);
            /* 変換された個所を訂正 */
            *(u32 *)((u32)p_rom + 0x60) |= 0x00406000;
        }

        /* MB_AUTHCODE_SIZE分 のバイナリ値を読み込む */
        if (rest >= MB_AUTHCODE_SIZE)
        {
            (void)FS_SeekFile(file, (int)(bak_pos + own_size), FS_SEEK_SET);
            (void)FS_ReadFile(file, dst, MB_AUTHCODE_SIZE);
            dst += MB_AUTHCODE_SIZE, rest -= MB_AUTHCODE_SIZE;
        }
        else
        {
            rest = 0;
        }

        /* キャッシュリスト構造体を確保 */
        if (rest >= sizeof(MBiCacheList))
        {
            p_cache = (MBiCacheList *) dst;
            MBi_InitCache(p_cache);
            dst += sizeof(MBiCacheList), rest -= sizeof(MBiCacheList);
            /* ROM ヘッダを最初のキャッシュに登録 */
            MBi_AttachCacheBuffer(p_cache, 0, ROM_HEADER_SIZE_FULL, p_rom, MB_CACHE_STATE_LOCKED);
            /* 対象アーカイブ名を記憶. (遅延ロード時に指定) */
            {
                FSArchive *p_arc = FS_GetAttachedArchive(file);
                int     i = 0;
                while ((i < FS_ARCHIVE_NAME_LEN_MAX) && p_arc->name.ptr[i])
                    ++i;
                MI_CpuCopy8(p_arc->name.ptr, p_cache->arc_name, (u32)i);
                p_cache->arc_name_len = (u32)i;
                p_cache->arc_pointer = p_arc;
            }
        }
        else
        {
            rest = 0;
        }

        /* segment-CARD マッピング領域を確保 */
        if (rest >= sizeof(u32) * 4)
        {
            p_map = (u32 *)dst;
            p_map[0] = 0;              /* ROM ヘッダ */
            p_map[1] = FS_GetFileImageTop(file) + bak_pos + p_rom->main_rom_offset;     /* ARM9 static */
            p_map[2] = FS_GetFileImageTop(file) + bak_pos + p_rom->sub_rom_offset;      /* ARM7 static */
            dst += sizeof(u32) * 4, rest -= sizeof(u32) * 4;
        }
        else
        {
            rest = 0;
        }

        /* 各セグメント読み込み */
        if (rest >= p_rom->main_size + p_rom->sub_size)
        {
            /* 充分に全て一括リードできる余裕がある場合 */
            const u32 base = FS_GetFileImageTop(file);
            (void)FS_SeekFile(file, (int)(p_map[1] - base), FS_SEEK_SET);
            (void)FS_ReadFile(file, dst, (int)p_rom->main_size);
            MBi_AttachCacheBuffer(p_cache, p_map[1], p_rom->main_size, dst, MB_CACHE_STATE_LOCKED);
            dst += p_rom->main_size, rest -= p_rom->main_size;
            (void)FS_SeekFile(file, (int)(p_map[2] - base), FS_SEEK_SET);
            (void)FS_ReadFile(file, dst, (int)p_rom->sub_size);
            MBi_AttachCacheBuffer(p_cache, p_map[2], p_rom->sub_size, dst, MB_CACHE_STATE_LOCKED);
            dst += p_rom->sub_size, rest -= p_rom->sub_size;
            /* 準備完了 */
            ret = TRUE;
        }
        else if (rest >= MB_CACHE_PAGE_SIZE * 3)
        {
            /* 3 枚のキャッシュ方式で対応できる分の余裕がある場合 */
            const u32 base = FS_GetFileImageTop(file);
            u32     offset = p_map[1];
            /* 1 枚目 (固定) */
            (void)FS_SeekFile(file, (int)(offset - base), FS_SEEK_SET);
            (void)FS_ReadFile(file, dst, MB_CACHE_PAGE_SIZE);
            MBi_AttachCacheBuffer(p_cache, offset, MB_CACHE_PAGE_SIZE, dst, MB_CACHE_STATE_LOCKED);
            dst += MB_CACHE_PAGE_SIZE, rest -= MB_CACHE_PAGE_SIZE;
            offset += MB_CACHE_PAGE_SIZE;
            /* 2 枚目 */
            (void)FS_SeekFile(file, (int)(offset - base), FS_SEEK_SET);
            (void)FS_ReadFile(file, dst, MB_CACHE_PAGE_SIZE);
            MBi_AttachCacheBuffer(p_cache, offset, MB_CACHE_PAGE_SIZE, dst, MB_CACHE_STATE_READY);
            dst += MB_CACHE_PAGE_SIZE, rest -= MB_CACHE_PAGE_SIZE;
            offset += MB_CACHE_PAGE_SIZE;
            /* 3 枚目 */
            (void)FS_SeekFile(file, (int)(offset - base), FS_SEEK_SET);
            (void)FS_ReadFile(file, dst, MB_CACHE_PAGE_SIZE);
            MBi_AttachCacheBuffer(p_cache, offset, MB_CACHE_PAGE_SIZE, dst, MB_CACHE_STATE_READY);
            dst += MB_CACHE_PAGE_SIZE, rest -= MB_CACHE_PAGE_SIZE;
            /* 準備完了 */
            ret = TRUE;
        }

        /* 最初の位置を復帰 */
        (void)FS_SeekFile(file, (int)bak_pos, FS_SEEK_SET);

        /* クローンブート */
        if (is_own_binary)
        {
            (void)FS_CloseFile(own_bin);
            if (ret)
            {
                const CARDRomRegion *p_header = mbi_seg_header;
                MBiSegmentHeaderInfo info;
                info.rom_src = p_rom->main_rom_offset;
                info.mem_src = (u32)((u32)p_rom->main_ram_address - p_rom->main_rom_offset);
                info.mem_dst = (u32)((u32)p_cache->list[1].ptr - p_rom->main_rom_offset);
                info.len = len;
                /* 最初に該当領域を初期化 */
                MBi_ReadSegmentHeader(&info, MB_SEGMENT_HEADER_MIN, MB_SEGMENT_HEADER_MAX, TRUE);
                /* ヘッダを順次転送 */
                for (; p_header->length != 0; ++p_header)
                {
                    MBi_ReadSegmentHeader(&info,
                                          p_header->offset,
                                          (u32)(p_header->offset + p_header->length), FALSE);
                }
                {
                    /* デバッガの autoload パッチ処理を復元 */
                    extern u32 _start_AutoloadDoneCallback[1];
                    u8     *dst = (u8 *)p_cache->list[1].ptr;
                    dst += ((u32)&_start_AutoloadDoneCallback - (u32)p_rom->main_ram_address);
                    *(u32 *)dst = 0xE12FFF1E;   /* asm { bx, lr } (always code32) */
                }
            }
        }

        if (ret)
        {
            DC_FlushRange(buf, len);
        }
    }

    return ret;
}

/*
 * セグメントのヘッダ部を読み込む.
 */
static void MBi_ReadSegmentHeader(const MBiSegmentHeaderInfo * p, u32 top, u32 bottom, BOOL clean)
{
    /* ヘッダ範囲の補正 */
    if (top < MB_SEGMENT_HEADER_MIN)
        top = MB_SEGMENT_HEADER_MIN;
    if (bottom > MB_SEGMENT_HEADER_MAX)
        bottom = MB_SEGMENT_HEADER_MAX;
    /* 実転送領域の補正 */
    if (top < p->rom_src)
        top = p->rom_src;
    if (bottom > p->rom_src + p->len)
        bottom = p->rom_src + p->len;
    /* 領域が有効なら転送 */
    if (top < bottom)
    {
        if (clean)
        {                              /* 0 初期化 */
            MI_CpuClear8((void *)(p->mem_dst + top), (u32)(bottom - top));
        }
        else
        {                              /* 転送 */
            MI_CpuCopy8((const void *)(p->mem_src + top),
                        (void *)(p->mem_dst + top), (u32)(bottom - top));
        }
    }
}


/*---------------------------------------------------------------------------*
  Name:         MB_RegisterFile

  Description:  親機にてDownload元のファイルを登録する

  Arguments:    game_reg : ゲーム登録情報
                buf : すでに MB_ReadSegment でロード済みのセグメントバッファ
  
  Returns:      
 *---------------------------------------------------------------------------*/

BOOL MB_RegisterFile(const MBGameRegistry *game_reg, const void *buf)
{
    MbDownloadFileInfoHeader *mdfi;
    static u8 update = 0;
    u8     *appname = (u8 *)game_reg->gameNamep;
    u8      fileID = 0xff, i;

    OSIntrMode enabled = OS_DisableInterrupts();        /* 割り込み禁止 */

    /* MBが開始されていなかったら、FALSEを返す． */
    if (!MBi_IsStarted())
    {
        OS_TWarning("MB has not started yet. Cannot Register File\n");
        (void)OS_RestoreInterrupts(enabled);    /* 割り込み禁止解除 */
        return FALSE;
    }

    if (pPwork->file_num + 1 > MB_MAX_FILE)
    {
        OS_TWarning("The number of registered files has exceeded.\n");
        (void)OS_RestoreInterrupts(enabled);    /* 割り込み禁止解除 */
        return FALSE;
    }

    /* 空きfileinfoを探す */
    for (i = 0; i < MB_MAX_FILE; i++)
    {
        /* そのGameRegistryが既に登録されていたら、FALSEを返す */
        if (pPwork->fileinfo[i].game_reg == (MBGameRegistry *)game_reg)
        {
            MB_DEBUG_OUTPUT("Already Registered \"%s\"\n", game_reg->gameNamep);
            (void)OS_RestoreInterrupts(enabled);        /* 割り込み禁止解除 */
            return FALSE;
        }

        if (pPwork->fileinfo[i].active == 0)
        {
            fileID = i;
            break;
        }
    }

    /* 空きfileinfoが見つからなかったら、異常終了 */
    if (i == MB_MAX_FILE)
    {
        OS_TWarning("Too many Files! \"%s\"\n", game_reg->gameNamep);
        (void)OS_RestoreInterrupts(enabled);    /* 割り込み禁止解除 */
        return FALSE;
    }


    // GameRegistryのポインタを登録
    pPwork->fileinfo[fileID].game_reg = (MBGameRegistry *)game_reg;

    // DownloadFileInfoへのポインタセット
    mdfi = &pPwork->fileinfo[fileID].dl_fileinfo.header;

    // MBDownloadFileInfo の生成
    MBi_MakeDownloadFileInfo(mdfi, buf);
    /* ユーザ定義拡張パラメータのコピー */
    MI_CpuCopy8(game_reg->userParam,
                ((MBDownloadFileInfo *) mdfi)->reserved, HW_DOWNLOAD_PARAMETER_SIZE);

    // Block情報テーブルの生成
    if (FALSE == MBi_MakeBlockInfoTable(&pPwork->fileinfo[fileID].blockinfo_table, mdfi))
    {
        OS_TWarning("Cannot make block information!\n");
        (void)OS_RestoreInterrupts(enabled);    /* 割り込み禁止解除 */
        return FALSE;
    }

    // MbGameInfoの生成
    MBi_MakeGameInfo(&pPwork->fileinfo[fileID].game_info, game_reg, &pPwork->common.user);

    /* ファイル番号をGameInfoにセット */
    pPwork->fileinfo[fileID].game_info.fileNo = fileID;

    /* GameInfo をBeaconリストに登録する。 */
    MB_AddGameInfo(&pPwork->fileinfo[fileID].game_info);

    /* ファイル更新番号をGameInfoにセット */
    pPwork->fileinfo[fileID].game_info.seqNoFixed = update++;

    pPwork->fileinfo[fileID].gameinfo_child_bmp = MB_GAMEINFO_PARENT_FLAG;

    pPwork->fileinfo[fileID].src_adr = (u32 *)buf;

    /* キャッシュ用の各種ポインタを保存 */
    pPwork->fileinfo[fileID].cache_list = (MBiCacheList *)
        ((u32)buf + ROM_HEADER_SIZE_FULL + MB_AUTHCODE_SIZE);
    pPwork->fileinfo[fileID].card_mapping = (u32 *)
        ((u32)buf + ROM_HEADER_SIZE_FULL + MB_AUTHCODE_SIZE + sizeof(MBiCacheList));

    /* タスクスレッドが必要になったら起動 */
    if (pPwork->fileinfo[fileID].cache_list->list[3].state != MB_CACHE_STATE_EMPTY)
    {
        if (!MBi_IsTaskAvailable())
        {
            MBi_InitTaskInfo(&pPwork->cur_task);
            MBi_InitTaskThread(pPwork->task_work, sizeof(pPwork->task_work));
        }
    }

    /* 登録ファイルをアクティブにする。 */
    pPwork->fileinfo[fileID].active = 1;

    // file数のインクリメント
    pPwork->file_num++;

    MB_DEBUG_OUTPUT("Register Game \"%s\"\n", game_reg->gameNamep);

    (void)OS_RestoreInterrupts(enabled);        /* 割り込み禁止解除 */

    return TRUE;

}

/*---------------------------------------------------------------------------*
  Name:         MB_UnregisterFile

  Description:  指定したファイルをダウンロードリストから削除

  Arguments:    game_reg : ゲーム登録情報
  
  Returns:      
 *---------------------------------------------------------------------------*/

/* 指定したファイルをダウンロードリストから削除 */
void   *MB_UnregisterFile(const MBGameRegistry *game_reg)
{
    u8      fileID, i;
    void   *ret_bufp;
    OSIntrMode enabled = OS_DisableInterrupts();        /* 割り込み禁止 */

    /* GameRegistryと一致するfileinfoを探す */
    for (i = 0; i < MB_MAX_FILE; i++)
    {
        if (pPwork->fileinfo[i].game_reg == (MBGameRegistry *)game_reg)
        {
            fileID = i;
            break;
        }
    }

    if (fileID != pPwork->fileinfo[fileID].game_info.fileNo)
    {
        OS_TWarning("Registerd File ID does not correspond with File ID in Registry List.\n");
        (void)OS_RestoreInterrupts(enabled);    /* 割り込み禁止解除 */
        return NULL;
    }

    /* 一致するfileinfoが見つからなかったら、異常終了 */
    if (i == MB_MAX_FILE)
    {
        OS_TWarning("Cannot find corresponding GameRegistry\n");
        (void)OS_RestoreInterrupts(enabled);    /* 割り込み禁止解除 */
        return NULL;
    }

    /* GameInfoから削除 */
    if (FALSE == MB_DeleteGameInfo(&pPwork->fileinfo[fileID].game_info))
    {
        OS_TWarning("Cannot delete GameInfo %s\n",
                    pPwork->fileinfo[fileID].game_info.fixed.gameName);
        (void)OS_RestoreInterrupts(enabled);    /* 割り込み禁止解除 */
        return NULL;
    }

    ret_bufp = pPwork->fileinfo[fileID].src_adr;

    pPwork->fileinfo[fileID].active = 0;

    /* 対象のfileinfoを削除 */
    MI_CpuClear16(&pPwork->fileinfo[fileID], sizeof(pPwork->fileinfo[0]));

    pPwork->file_num--;

    MB_DEBUG_OUTPUT("Delete Game \"%s\"\n", game_reg->gameNamep);

    (void)OS_RestoreInterrupts(enabled);        /* 割り込み禁止解除 */

    return ret_bufp;
}

/*---------------------------------------------------------------------------*
  Name:         MBi_MakeDownloadFileInfo

  Description:  指定されたマルチブートゲーム登記から、ダウンロードファイル情報を構築する。

  Arguments:    mbdlFileInfop - 構築されるMbDownloadFileInfoへのポインタ
                buf           - Fileイメージバッファへのポインタ
  
  Returns:      None.
 *---------------------------------------------------------------------------*/

static void MBi_MakeDownloadFileInfo(MbDownloadFileInfoHeader * mbdlFileInfop, const void *buf)
{
    const RomHeader *mbRomHeaderp;
    const MbSegmentType *loadSegListp;
    MbSegmentInfo *seg_infop;
    int     seg_num;
    const u16 *auth_code;
    u32     child_recv_buff_addr = MB_ARM7_STATIC_RECV_BUFFER;

    mbRomHeaderp = (const RomHeader *)buf;
    auth_code = (const u16 *)((u32)buf + ROM_HEADER_SIZE_FULL);

    /*
       ※ROMヘッダのチェックをして、ROMバイナリかどうかを判定した方がいいかも？
     */

    // マルチブートゲーム情報をもとに、初期ダウンロードファイル情報を構築する。
    mbdlFileInfop->arm9EntryAddr = (u32)mbRomHeaderp->main_entry_address;
    mbdlFileInfop->arm7EntryAddr = (u32)mbRomHeaderp->sub_entry_address;
    seg_infop = (MbSegmentInfo *) (mbdlFileInfop + 1);
    loadSegListp = MBi_defaultLoadSegList;
    MB_DEBUG_OUTPUT("\t<segment list>         recv_adr load_adr     size  rom_adr (base)\n");

    /*
     * セグメントは static にする方向でまとまったので全て破棄.
     * overlay : Load/Unload 時の管理ができないので不可能.
     * file    : 容量などの仕様を策定してリファレンスにする必要があり日程的に不可能.
     */
    for (seg_num = 0; seg_num < MB_DL_SEGMENT_NUM; seg_num++)
    {
        /* 各セグメントのロードアドレスおよびサイズ情報取得処理 */
        MB_DEBUG_OUTPUT("\t SEG%2d : ", seg_num);
        MBi_SetSegmentInfo(mbRomHeaderp, loadSegListp, seg_infop, &child_recv_buff_addr);
        seg_infop++;
        loadSegListp++;
    }

    /* 認証用コードをMBDownloadFileInfo内にセットする */
    {
        MBDownloadFileInfo *pMdfi = (MBDownloadFileInfo *) mbdlFileInfop;
        MI_CpuCopy8(auth_code, &pMdfi->auth_code[0], MB_AUTHCODE_SIZE);
    }
}

/*---------------------------------------------------------------------------*
  Name:         MBi_SetSegmentInfo

  Description:  セグメント情報のセット

  Arguments:    mbRomHeaderp
                loadSegListp
				seg_infop
				child_recv_buffer

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBi_SetSegmentInfo(const RomHeader * mbRomHeaderp,
                               const MbSegmentType * loadSegListp,
                               MbSegmentInfo * seg_infop, u32 *child_recv_buff_addr)
{
    CARDRomRegion *romRegp;

    /* 各セグメントのロードアドレスおよびサイズ情報取得処理 */

    switch (*loadSegListp)
    {
    case MB_SEG_ARM9STATIC:
        romRegp = (CARDRomRegion *)(&mbRomHeaderp->main_ram_address);

        if (((u32)romRegp->offset >= MB_LOAD_AREA_LO)
            && ((u32)romRegp->offset < MB_LOAD_AREA_HI)
            && (((u32)romRegp->offset + romRegp->length) <= MB_LOAD_AREA_HI))
        {

            seg_infop->size = romRegp->length;
            seg_infop->load_addr = (u32)romRegp->offset;
            /* ※圧縮展開を考慮すると、将来的には、load_addr == recv_addrではダメ */
            seg_infop->recv_addr = seg_infop->load_addr;
            seg_infop->target = MI_PROCESSOR_ARM9;
            MB_DEBUG_OUTPUT("arm9 static :");
        }
        else
        {
            // ロードプログラムのアドレスもしくはアドレス＋サイズがロード可能エリアを超えている
            OS_Panic("ARM9 boot code out of the load area. : addr = %x  size = %x\n",
                     (u32)romRegp->offset, romRegp->length);
        }
        break;

    case MB_SEG_ARM7STATIC:
        {
            BOOL    error_flag = FALSE;
            BOOL    reload_flag = FALSE;
            u32     load_last_addr;

            romRegp = (CARDRomRegion *)(&mbRomHeaderp->sub_ram_address);
            load_last_addr = (u32)((u32)romRegp->offset + romRegp->length);

            // 配置アドレス、サイズの判定
            /* 配置先頭アドレスについて(メインメモリに配置の場合) */
            if (((u32)romRegp->offset >= MB_LOAD_AREA_LO)
                && ((u32)romRegp->offset < MB_BSSDESC_ADDRESS))
            {
                /* 配置アドレス最後尾について */
                if (load_last_addr <= MB_ARM7_STATIC_LOAD_AREA_HI)      // 再配置の必要のない場合はそのままセット
                {
                }
                else if ((load_last_addr < MB_BSSDESC_ADDRESS)
                         && (romRegp->length <= MB_ARM7_STATIC_RECV_BUFFER_SIZE))
                {                      // 再配置が必要な場合は、そのように。
                    reload_flag = TRUE;
                }
                else
                {
                    error_flag = TRUE;
                }
            }
            else
                /* 配置先頭アドレスについて(WRAMに配置の場合) */
            if (((u32)romRegp->offset >= HW_WRAM)
                    && ((u32)romRegp->offset < HW_WRAM + MB_ARM7_STATIC_LOAD_WRAM_MAX_SIZE))
            {

                if (load_last_addr <= (HW_WRAM + MB_ARM7_STATIC_LOAD_WRAM_MAX_SIZE))
                {                      // 再配置とする。
                    reload_flag = TRUE;
                }
                else
                {
                    error_flag = TRUE;
                }
            }
            else
            {                          // それ以外の領域の時はエラー
                error_flag = TRUE;
            }

            // エラー判定
            if (error_flag == TRUE)
            {
                OS_Panic("ARM7 boot code out of the load area. : addr = %x  size = %x\n",
                         (u32)romRegp->offset, romRegp->length);
            }

            // セグメント情報セット
            {
                seg_infop->size = romRegp->length;
                seg_infop->load_addr = (u32)romRegp->offset;
                if (!reload_flag)
                {
                    seg_infop->recv_addr = seg_infop->load_addr;
                }
                else
                {
                    /*
                     * ARM7で実行アドレスが再配置が必要な領域なら、
                     * 受信アドレスをメインメモリ受信バッファに設定する。
                     */
                    seg_infop->recv_addr = *child_recv_buff_addr;
                    *child_recv_buff_addr += seg_infop->size;
                }
            }

            seg_infop->target = MI_PROCESSOR_ARM7;
            MB_DEBUG_OUTPUT("arm7 static :");
        }
        break;

    case MB_SEG_ROMHEADER:
        seg_infop->size = ROM_HEADER_SIZE_FULL;
        seg_infop->load_addr = (u32)MB_ROM_HEADER_ADDRESS;
        seg_infop->recv_addr = seg_infop->load_addr;
        // ROMHEADERのtargetはMI_PROCESSOR_ARM9とする
        seg_infop->target = MI_PROCESSOR_ARM9;
        MB_DEBUG_OUTPUT("rom header  :");
        break;
    }

    MB_DEBUG_OUTPUT(" %8x %8x %8x %8x\n",
                    seg_infop->recv_addr, seg_infop->load_addr,
                    seg_infop->size,
                    (*loadSegListp == MB_SEG_ROMHEADER) ? 0 : *((u32 *)romRegp - 2));

}

/*	----------------------------------------------------------------------------

	ブロック情報制御関数(親機用)

	----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         MBi_MakeBlockInfoTable

  Description:  Block情報取得のためのテーブルを作成

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/


BOOL MBi_MakeBlockInfoTable(MB_BlockInfoTable * table, MbDownloadFileInfoHeader * mdfi)
{
    u16    *seg_headB = table->seg_head_blockno;
    u32    *seg_src = table->seg_src_offset;
    u8      i;
    u32     src_ofs = 0;

    if (!mdfi)
        return FALSE;

    for (i = 0; i < MB_DL_SEGMENT_NUM; ++i)
    {
        MbSegmentInfo *si = MBi_GetSegmentInfo(mdfi, i);
        seg_src[i] = src_ofs;
        src_ofs += si->size;
    }

    seg_headB[0] = 0;                  // Segment 0

    for (i = 0; i < MB_DL_SEGMENT_NUM; ++i)
    {
        MbSegmentInfo *si = MBi_GetSegmentInfo(mdfi, i);
        u16     next_block_head =
            (u16)(seg_headB[i] + (u16)((si->size + mbc->block_size_max - 1) / mbc->block_size_max));

        if (FALSE == IsAbleToLoad(i, si->load_addr, si->size))
        {
            return FALSE;
        }

        if (i < MB_DL_SEGMENT_NUM - 1)
        {
            seg_headB[i + 1] = next_block_head;
        }
        else
        {
            table->block_num = next_block_head;
        }

    }

    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         MBi_get_blockinfo

  Description:  Block情報の取得

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/

BOOL MBi_get_blockinfo(MB_BlockInfo * bi, MB_BlockInfoTable * table,
                       u32 block, MbDownloadFileInfoHeader * mdfi)
{
    s8      i;
    u32     seg_block, block_adr_offset;

    if (block >= table->block_num)
    {
        return FALSE;
    }

    for (i = MB_DL_SEGMENT_NUM - 1; i >= 0; i--)
    {
        if (block >= table->seg_head_blockno[i])
        {
            break;
        }
    }

    if (i < 0)
        return FALSE;

    seg_block = block - table->seg_head_blockno[i];
    block_adr_offset = seg_block * mbc->block_size_max;

    {
        MbSegmentInfo *si = MBi_GetSegmentInfo(mdfi, i);
        bi->size = si->size - block_adr_offset;
        if (bi->size > mbc->block_size_max)
            bi->size = (u32)mbc->block_size_max;
        bi->offset = (u32)(block_adr_offset + table->seg_src_offset[i]);
        bi->child_address = (u32)(block_adr_offset + (u32)(si->recv_addr));
        bi->segment_no = (u8)i;
    }

    MB_DEBUG_OUTPUT("blockNo:%d \n", block);
    MB_DEBUG_OUTPUT("Segment %d [block:%d offset(%08x) destination(%08x) size(%04x)]\n",
                    i, seg_block, bi->offset, bi->child_address, bi->size);

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         MBi_get_blocknum

  Description:  Block総数の取得

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/

u16 MBi_get_blocknum(MB_BlockInfoTable * table)
{
    SDK_ASSERT(table != 0);
    return table->block_num;
}


/*---------------------------------------------------------------------------*
  Name:         MBi_IsAbleToRecv

  Description:  受信アドレスの正当性チェック

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/

BOOL MBi_IsAbleToRecv(u8 segment_no, u32 address, u32 size)
{
    MbSegmentType seg_type;

    SDK_ASSERT(segment_no < MB_DL_SEGMENT_NUM);

    seg_type = MBi_defaultLoadSegList[segment_no];

    switch (seg_type)
    {
    case MB_SEG_ROMHEADER:
        if (address >= MB_ROM_HEADER_ADDRESS
            && address + size <= MB_ROM_HEADER_ADDRESS + ROM_HEADER_SIZE_FULL)
        {
            return TRUE;
        }
        break;

    case MB_SEG_ARM9STATIC:
        if (address >= MB_LOAD_AREA_LO && address + size <= MB_LOAD_AREA_HI)
        {
            return TRUE;
        }
        break;

    case MB_SEG_ARM7STATIC:
        /* wram mapping */
        if (address >= MB_ARM7_STATIC_RECV_BUFFER
            && address + size <= MB_ARM7_STATIC_RECV_BUFFER_END)
        {
            return TRUE;
        }
        else
            /* main memory mapping */
        if (address >= MB_LOAD_AREA_LO && address + size <= MB_ARM7_STATIC_RECV_BUFFER_END)
        {
            return TRUE;
        }
        break;

    default:
        return FALSE;
    }

    return FALSE;
}

static BOOL IsAbleToLoad(u8 segment_no, u32 address, u32 size)
{
    MbSegmentType seg_type;

    SDK_ASSERT(segment_no < MB_DL_SEGMENT_NUM);

    seg_type = MBi_defaultLoadSegList[segment_no];

    switch (seg_type)
    {
    case MB_SEG_ROMHEADER:
    case MB_SEG_ARM9STATIC:
        return MBi_IsAbleToRecv(segment_no, address, size);

    case MB_SEG_ARM7STATIC:
        /* main memory */
        if (address >= MB_LOAD_AREA_LO && address < MB_BSSDESC_ADDRESS)
        {
            u32     end_adr = address + size;

            if (MB_ARM7_STATIC_RECV_BUFFER_END > address
                && MB_ARM7_STATIC_RECV_BUFFER_END < end_adr)

            {
                return FALSE;          // NG
            }
            else if (end_adr <= MB_ARM7_STATIC_LOAD_AREA_HI)
            {
                return TRUE;           // OK
            }
            else if (end_adr < MB_BSSDESC_ADDRESS && size <= MB_ARM7_STATIC_RECV_BUFFER_SIZE)
            {
                return TRUE;           // OK (needs relocate)
            }
            else
            {
                return FALSE;          // NG
            }
        }
        else
            /* wram */
        if (address >= HW_WRAM && address < HW_WRAM + MB_ARM7_STATIC_LOAD_WRAM_MAX_SIZE)
        {
            u32     end_adr = address + size;
            if (end_adr <= (HW_WRAM + MB_ARM7_STATIC_LOAD_WRAM_MAX_SIZE))
            {
                return TRUE;           // OK
            }
            else
            {
                return FALSE;          // NG
            }
        }
        break;

    default:
        return FALSE;
    }

    return FALSE;
}
