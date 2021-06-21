/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WFS - libraries
  File:     wfs_thread.c

  Copyright 2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/


#include <nitro.h>


/*---------------------------------------------------------------------------*/
/* constants */

/*
 * ROMキャッシュの各設定値.
 * [ページサイズ]
 * - ROMページサイズ(512BYTE)の整数倍が望ましい.
 * - 小さすぎるとROMアクセスのオーバーヘッドが増加する.
 * - 大きすぎるとサーバ自身のROMアクセス(FS/SND/...)が不定期に阻害される.
 * [ページ数]
 * - 大きければ転送効率の安定化(劣悪な通信環境での再送処理応答性)に寄与する.
 * - アクセスの局所性が高い場合にはクライアント総数より大きい値が望ましい.
 */
#define	WFS_FILE_CACHE_SIZE	    1024UL
#define WFS_CACHE_PAGE_TOTAL    8


/*---------------------------------------------------------------------------*/
/* declarations */

typedef struct WFSServerThread
{
    WFSEventCallback    hook;
    MIDevice            device[1];
    FSFile              file[1];
    MICache             cache[1];
    u8                  cache_buf[MI_CACHE_BUFFER_WORKSIZE(
                                  WFS_FILE_CACHE_SIZE, WFS_CACHE_PAGE_TOTAL)];
    OSMessageQueue      msg_queue[1];
    OSMessage           msg_array[1];
    OSThread            thread[1];
    u8                  thread_stack[0x400];
}
WFSServerThread;


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         WFSi_ReadRomCallback

  Description:  ROMデバイス読み込みコールバック.

  Arguments:    userdata         WFSServerThread構造体.
                buffer           転送先メモリ.
                offset           転送元オフセット.
                length           転送サイズ.

  Returns:      成功すれば0以上の値, 失敗すれば負の値.
 *---------------------------------------------------------------------------*/
static int WFSi_ReadRomCallback(void *userdata, void *buffer, u32 offset, u32 length)
{
    WFSServerThread * const thread = (WFSServerThread*)userdata;

    /* NITRO-SDK 固有 : 先頭32kBはアクセス不可なのでメモリから読み込み */
    if(offset < sizeof(CARDRomRegion))
    {
        const u8 *header = CARD_GetRomHeader();
        if (length > sizeof(CARDRomHeader) - offset)
        {
            length = sizeof(CARDRomHeader) - offset;
        }
        MI_CpuCopy8(header + offset, buffer, length);
    }
    else
    {
        if (offset < 0x8000)
        {
            u32     memlen = length;
            if (memlen > 0x8000 - offset)
            {
                memlen = 0x8000 - offset;
            }
            MI_CpuFill8(buffer, 0x00, length);
            offset += memlen;
            length -= memlen;
        }
        if (length > 0)
        {
            (void)FS_SeekFile(thread->file, (int)offset, FS_SEEK_SET);
            (void)FS_ReadFile(thread->file, buffer, (int)length);
        }
    }
    return (int)length;
}

/*---------------------------------------------------------------------------*
  Name:         WFSi_WriteRomCallback

  Description:  ROMデバイス書き込みコールバック. (ダミー)

  Arguments:    userdata         WFSServerThread構造体.
                buffer           転送先メモリ.
                offset           転送元オフセット.
                length           転送サイズ.

  Returns:      成功すれば0以上の値, 失敗すれば負の値.
 *---------------------------------------------------------------------------*/
static int WFSi_WriteRomCallback(void *userdata, const void *buffer, u32 offset, u32 length)
{
    (void)userdata;
    (void)buffer;
    (void)offset;
    (void)length;
    return -1;
}

/*---------------------------------------------------------------------------*
  Name:         WFSi_ServerThreadProc

  Description:  WFSサーバ用スレッドプロシージャ.

  Arguments:    arg             WFSServerThread構造体.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WFSi_ServerThreadProc(void *arg)
{
    WFSServerThread *const thread = (WFSServerThread*)arg;
    for (;;)
    {
        BOOL    busy = FALSE;
        (void)OS_ReceiveMessage(thread->msg_queue, (OSMessage*)&busy, OS_MESSAGE_BLOCK);
        if (!busy)
        {
            break;
        }
        MI_LoadCache(thread->cache, thread->device);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WFSi_ThreadHook

  Description:  WFSサーバのセグメントコールバックに仕掛けるフック関数.

  Arguments:    work            WFSServerThread構造体.
                argument        WFSSegmentBuffer構造体.
                                NULLなら終了通知.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WFSi_ThreadHook(void *work, void *argument)
{
    WFSServerThread * const thread = (WFSServerThread *)work;
    WFSSegmentBuffer * const segment = (WFSSegmentBuffer*)argument;
    /* NULLなら終了通知 */
    if (!segment)
    {
        (void)OS_SendMessage(thread->msg_queue, (OSMessage)FALSE, OS_MESSAGE_BLOCK);
        OS_JoinThread(thread->thread);
        (void)FS_CloseFile(thread->file);
    }
    /* NULLでなければセグメント要求 (またはその準備通知) */
    else if (!MI_ReadCache(thread->cache, segment->buffer, segment->offset, segment->length))
    {
        /* キャッシュミスした場合はスレッドへ読み込み指示 */
        segment->buffer = NULL; /* == "could not prepare immediately" */
        (void)OS_SendMessage(thread->msg_queue, (OSMessage)TRUE, OS_MESSAGE_NOBLOCK);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WFS_ExecuteRomServerThread

  Description:  指定のROMファイルを配信するようWFSライブラリへ登録し,
                内部でROMアクセス用のスレッドを自動的に起動する.
                このスレッドはWFS_EndServer関数の内部で自動的に破棄される.

  Arguments:    context          WFSServerContext構造体.
                file             配信するファイルシステムを持つSRLファイル.
                                 クローンブートならNULLを指定する.
                sharedFS         ファイルシステムを親子で共有させるならTRUE.
                                 その場合, 親機自体の持つファイルシステムに
                                 fileの持つオーバーレイだけを抽出して追加した
                                 混合ファイルシステムを配信する.
                                 fileにNULLを指定した場合はクローンブートなので
                                 この引数は無視される. (常にTRUEと解釈される)


  Returns:      ROMファイルの登録とスレッドの生成に成功すればTRUE.
 *---------------------------------------------------------------------------*/
BOOL WFS_ExecuteRomServerThread(WFSServerContext *context, FSFile *file, BOOL sharedFS)
{
    BOOL    retval = FALSE;

    WFSServerThread *thread = MI_CallAlloc(context->allocator, sizeof(WFSServerThread), 32);
    if (!thread)
    {
        OS_TWarning("failed to allocate memory enough to create internal thread.");
    }
    else
    {
        /* 単体ROM型, クローンブート型, FS共有型のいずれかを判定する */
        u32     pos = file ? (FS_GetFileImageTop(file) + FS_GetPosition(file)) : 0;
        u32     fatbase = (u32)((file && !sharedFS) ? pos : 0);
        u32     overlay = (u32)(file ? pos : 0);
        /* ROMアクセス用のデバイスとキャッシュを初期化 */
        FS_InitFile(thread->file);
        if (!FS_CreateFileFromRom(thread->file, 0, 0x7FFFFFFF))
        {
            OS_TPanic("failed to create ROM-file!");
        }
        MI_InitDevice(thread->device, thread,
                      WFSi_ReadRomCallback, WFSi_WriteRomCallback);
        MI_InitCache(thread->cache, WFS_FILE_CACHE_SIZE,
                     thread->cache_buf, sizeof(thread->cache_buf));
        /* ファイルテーブルを登録 */
        if (WFS_RegisterServerTable(context, thread->device, fatbase, overlay))
        {
            /* 処理が成功すればフックを設定してスレッド起動 */
            context->thread_work = thread;
            context->thread_hook = WFSi_ThreadHook;
            OS_InitMessageQueue(thread->msg_queue, thread->msg_array, 1);
            OS_CreateThread(thread->thread, WFSi_ServerThreadProc, thread,
                            thread->thread_stack + sizeof(thread->thread_stack),
                            sizeof(thread->thread_stack), 15);
            OS_WakeupThreadDirect(thread->thread);
            retval = TRUE;
        }
        else
        {
            MI_CallFree(context->allocator, thread);
        }
    }
    return retval;
}


/*---------------------------------------------------------------------------*
  $Log: wfs_thread.c,v $
  Revision 1.1  2007/06/14 13:14:46  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
