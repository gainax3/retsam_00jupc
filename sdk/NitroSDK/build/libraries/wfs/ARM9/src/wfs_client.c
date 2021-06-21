/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WFS - libraries
  File:     wfs_client.c

  Copyright 2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/


#include <nitro/wfs/client.h>


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         WFSi_NotifyEvent

  Description:  WFSサーバ内部のイベントを通知する.

  Arguments:    context          WFSClientContext構造体.
                event            通知するイベント種別.
                argument         イベント引数.

  Returns:      None.
 *---------------------------------------------------------------------------*/
inline static void WFSi_NotifyEvent(WFSClientContext *context,
                                    WFSEventType event, void *argument)
{
    if (context->callback)
    {
        context->callback(context, event, argument);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WFSi_ReallocBitmap

  Description:  指定されたサイズ受信可能なビットマップを再確保.

  Arguments:    context          WFSClientContext構造体.
                length           受信予定のファイルサイズ.
                                 負の値を指定すると現在の値で再確保.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WFSi_ReallocBitmap(WFSClientContext *context, int length)
{
    if (length < 0)
    {
        length = (int)context->max_file_size;
        context->max_file_size = 0;
    }
    if (context->max_file_size < length)
    {
        const int packet = WBT_GetParentPacketLength(context->wbt);
        const u32 newsize = WBT_PACKET_BITMAP_SIZE((u32)length, packet);
        context->max_file_size = (u32)length;
        MI_CallFree(context->allocator, context->recv_pkt_bmp_buf);
        context->recv_pkt_bmp_buf = (u32 *)MI_CallAlloc(context->allocator,
                                    sizeof(u32) * newsize, sizeof(u32));
        context->recv_buf_packet_bmp_table.packet_bitmap[0] = context->recv_pkt_bmp_buf;
    }
    SDK_ASSERT(context->recv_pkt_bmp_buf);
}

/*---------------------------------------------------------------------------*
  Name:         WFSi_ReceiveTableSequence

  Description:  ROMファイルテーブルを受信するシーケンス.
                サーバとの接続直後に1回だけ実行される.

  Arguments:    userdata         WFSClientContext構造体.
                callback         WBT完了コールバック引数.
                                 シーケンス開始時はNULLを指定して呼び出す.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WFSi_ReceiveTableSequence(void *userdata, WBTCommand *callback)
{
    WFSClientContext * const    context = (WFSClientContext *)userdata;
    WBTContext * const          wbt = context->wbt;

    if ((callback == NULL) || (callback->result == WBT_RESULT_SUCCESS))
    {
        BOOL    post = FALSE;
        const int   bitmap = 0x0001;
        /* シーケンス開始 => WBT-SYNC() を発行してパケットサイズを同期 */
        if (callback == NULL)
        {
            WFS_DEBUG_OUTPUT(("WBT-SYNC():post"));
            post = WBT_PostCommandSYNC(wbt, bitmap, WFSi_ReceiveTableSequence);
        }
        /* WBT-SYNC 完了 => WBT-INFO(0) を発行してROMファイルテーブルのサイズを取得 */
        else if (callback->event == WBT_CMD_RES_SYNC)
        {
            WFS_DEBUG_OUTPUT(("WBT-SYNC():done [server = %d, client = %d]",
                             callback->sync.peer_packet_size + WBT_PACKET_SIZE_MIN,
                             callback->sync.my_packet_size + WBT_PACKET_SIZE_MIN));
            WFS_DEBUG_OUTPUT(("WBT-INFO(0):post"));
            post = WBT_PostCommandINFO(wbt, bitmap, WFSi_ReceiveTableSequence,
                                       0, &context->block_info_table);
        }
        /* WBT-INFO(0) 完了 => WBT-GET(0x20000) を発行してROMファイルテーブルを取得 */
        else if (callback->event == WBT_CMD_RES_GET_BLOCKINFO)
        {
            const int length = context->block_info_table.block_info[0]->block_size;
            WFS_DEBUG_OUTPUT(("WBT-INFO(0):done [table-length = %d]", length));
            context->table->length = (u32)length;
            context->table->buffer = (u8 *)MI_CallAlloc(context->allocator, (u32)length, 1);
            SDK_ASSERT(context->table->buffer);
            WFSi_ReallocBitmap(context, length);
            context->recv_buf_table.recv_buf[0] = context->table->buffer;
            WFS_DEBUG_OUTPUT(("WBT-GET(0x%08X):post", WFS_TABLE_BLOCK_INDEX));
            post = WBT_PostCommandGET(wbt, bitmap, WFSi_ReceiveTableSequence,
                                      WFS_TABLE_BLOCK_INDEX, context->table->length,
                                      &context->recv_buf_table,
                                      &context->recv_buf_packet_bmp_table);
        }
        /* WBT-GET(0x20000) 完了 => マウント準備完了イベント通知 */
        else if (callback->event == WBT_CMD_RES_GET_BLOCK)
        {
            WFS_DEBUG_OUTPUT(("WBT-GET(0x%08X):done [ready for mount]", WFS_TABLE_BLOCK_INDEX));
            WFS_ParseTable(context->table);
            context->fat_ready = TRUE;
            WFSi_NotifyEvent(context, WFS_EVENT_CLIENT_READY, NULL);
            post = TRUE;    /* 便宜上 */
        }
        /* WBTコマンド発行失敗 (WFS内部不具合によるコマンドキュー不足) */
        if (!post)
        {
            WFS_DEBUG_OUTPUT(("internal-error (failed to post WBT command)"));
        }
    }
    /* 何らかの内部エラー */
     else
    {
        /* WFSスタートアップ中のキャンセル (ここでは特に何もしなくてよい) */
        if (callback->event == WBT_CMD_CANCEL)
        {
        }
        /* 予期せぬWBTエラー (WFS内部不具合による状態管理の不整合) */
        else
        {
            WFS_DEBUG_OUTPUT(("internal-error (unexpected WBT result)"));
            WFS_DEBUG_OUTPUT(("  command = %d", callback->command));
            WFS_DEBUG_OUTPUT(("  event   = %d", callback->event));
            WFS_DEBUG_OUTPUT(("  result  = %d", callback->result));
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WFSi_ReadRomSequence

  Description:  サーバ側のROMイメージを受信するシーケンス.
                クライアントからのFS_ReadFile関数呼び出しのたびに実行される.

  Arguments:    userdata         WFSClientContext構造体.
                callback         WBT完了コールバック引数.
                                 シーケンス開始時はNULLを指定して呼び出す.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WFSi_ReadRomSequence(void *userdata, WBTCommand *callback)
{
    WFSClientContext * const    context = (WFSClientContext *)userdata;
    WBTContext * const          wbt = context->wbt;

    if ((callback == NULL) || (callback->result == WBT_RESULT_SUCCESS))
    {
        BOOL    post = FALSE;
        const int   bitmap = 0x0001;
        /* シーケンス開始 => WBT-MSG(LOCK) を発行して転送範囲をロック */
        if (callback == NULL)
        {
            WFS_DEBUG_OUTPUT(("WBT-MSG(LOCK):post"));
            post = WFS_SendMessageLOCK_REQ(wbt, WFSi_ReadRomSequence, bitmap,
                                           context->request_region.offset + context->table->origin,
                                           context->request_region.length);
        }
        /* WBT-MSG() 発行完了 (サーバの応答を待つのでここでは何もしない) */
        else if (callback->event == WBT_CMD_RES_USER_DATA)
        {
            /* waiting for response from server... */
            post = TRUE;    /* 便宜上 */
        }
        else
        {
            const WFSMessageFormat *const msg = (const WFSMessageFormat *)callback->user_data.data;
            /* WBT-MSG(LOCK) 応答受信 */
            if ((callback->event == WBT_CMD_REQ_USER_DATA) &&
                (msg->type == WFS_MSG_LOCK_ACK))
            {
                BOOL    accepted = (BOOL)MI_LEToH32(msg->arg2);
                /* 拒否応答 => WBT-SYNC() を再発行してパケットサイズを同期 */
                if (!accepted)
                {
                    WFS_DEBUG_OUTPUT(("WBT-MSG(LOCK):failed [packet-length renewal]"));
                    WFS_DEBUG_OUTPUT(("WBT-SYNC():post"));
                    post = WBT_PostCommandSYNC(wbt, bitmap, WFSi_ReadRomSequence);
                }
                /* 許可応答 => WBT-GET(id) を再発行してブロックを受信 */
                else
                {
                    u32     id = MI_LEToH32(msg->arg1);
                    WFS_DEBUG_OUTPUT(("WBT-MSG(LOCK):done [lock-id = 0x%08X]", id));
                    context->block_id = id;
                    context->recv_buf_table.recv_buf[0] = context->request_buffer;
                    WFS_DEBUG_OUTPUT(("WBT-GET(0x%08X):post", id));
                    post = WBT_PostCommandGET(wbt, bitmap, WFSi_ReadRomSequence,
                                              context->block_id, context->request_region.length,
                                              &context->recv_buf_table,
                                              &context->recv_buf_packet_bmp_table);
                }
            }
            /* WBT-SYNC() 完了 => シーケンスを再試行 (一段だけの再帰呼び出しとなる) */
            else if (callback->event == WBT_CMD_RES_SYNC)
            {
                WFS_DEBUG_OUTPUT(("WBT-SYNC():done [server = %d, client = %d]",
                                 callback->sync.peer_packet_size + WBT_PACKET_SIZE_MIN,
                                 callback->sync.my_packet_size + WBT_PACKET_SIZE_MIN));
                WFSi_ReallocBitmap(context, -1);
                WFSi_ReadRomSequence(context, NULL);
                post = TRUE;    /* 便宜上 */
            }
            /* WBT-GET(id) 完了 => WBT-MSG(UNLOCK, id) を発行して転送範囲を解放 */
            else if (callback->event == WBT_CMD_RES_GET_BLOCK)
            {
                u32     id = context->block_id;
                WFS_DEBUG_OUTPUT(("WBT-GET(0x%08X):done", id));
                WFS_DEBUG_OUTPUT(("WBT-MSG(UNLOCK,0x%08X):post", id));
                post = WFS_SendMessageUNLOCK_REQ(wbt, WFSi_ReadRomSequence, bitmap, id);
            }
            /* WBT-MSG(UNLOCK, id) 応答受信 => リード完了イベント通知 */
            else if ((callback->event == WBT_CMD_REQ_USER_DATA) &&
                (msg->type == WFS_MSG_UNLOCK_ACK))
            {
                u32     id = context->block_id;
                WFS_DEBUG_OUTPUT(("WBT-MSG(UNLOCK,0x%08X):done [read-operation completed]", id));
                context->request_buffer = NULL;
                {
                    WFSRequestClientReadDoneCallback callback = context->request_callback;
                    void   *argument = context->request_argument;
                    context->request_callback = NULL;
                    context->request_argument = NULL;
                    if (callback)
                    {
                        (*callback)(context, TRUE, argument);
                    }
                }
                post = TRUE;    /* 便宜上 */
            }
        }
        /* WBTコマンド発行失敗 (WFS内部不具合によるコマンドキュー不足) */
        if (!post)
        {
            WFS_DEBUG_OUTPUT(("internal-error (failed to post WBT command)"));
        }
    }
    /* 何らかの内部エラー */
     else
    {
        /* WFSリード処理中のキャンセル (ここでは特に何もしなくてよい) */
        if (callback->event == WBT_CMD_CANCEL)
        {
        }
        /* 予期せぬWBTエラー (WFS内部不具合による状態管理の不整合) */
        else
        {
            WFS_DEBUG_OUTPUT(("internal-error (unexpected WBT result)"));
            WFS_DEBUG_OUTPUT(("  command = %d", callback->command));
            WFS_DEBUG_OUTPUT(("  event   = %d", callback->event));
            WFS_DEBUG_OUTPUT(("  result  = %d", callback->result));
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WFSi_WBTSystemCallback

  Description:  クライアント側のWBTシステムコールバック.

  Arguments:    userdata         WFSServerContext構造体.
                callback         WBTのイベント引数.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WFSi_WBTSystemCallback(void *userdata, WBTCommand *callback)
{
    WFSClientContext * const context = (WFSClientContext *)userdata;
    /* サーバからの応答メッセージは WFSi_ReadRomSequence へリダイレクト */
    if ((callback->event == WBT_CMD_REQ_USER_DATA) &&
        (context->request_buffer))
    {
        WFSi_ReadRomSequence(context, callback);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WFS_CallClientConnectHook

  Description:  クライアント側の接続通知.

  Arguments:    context          WFSClientContext構造体.
                peer             接続した通信先の情報.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_CallClientConnectHook(WFSClientContext *context, const WFSPeerInfo *peer)
{
    (void)context;
    (void)peer;
}

/*---------------------------------------------------------------------------*
  Name:         WFS_CallClientDisconnectHook

  Description:  クライアント側の切断通知.

  Arguments:    context          WFSClientContext構造体.
                peer             切断した通信先の情報.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_CallClientDisconnectHook(WFSClientContext *context, const WFSPeerInfo *peer)
{
    (void)context;
    (void)peer;
}

/*---------------------------------------------------------------------------*
  Name:         WFS_CallClientPacketSendHook

  Description:  クライアント側のパケット送信可能タイミング通知.

  Arguments:    context          WFSClientContext構造体.
                packet           送信パケット設定.

  Returns:      実際のパケットサイズ.
 *---------------------------------------------------------------------------*/
void WFS_CallClientPacketSendHook(WFSClientContext *context, WFSPacketBuffer *packet)
{
    packet->length = WBT_CallPacketSendHook(context->wbt, packet->buffer, packet->length, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         WFS_CallClientPacketRecvHook

  Description:  クライアント側のパケット受信タイミング通知.

  Arguments:    context          WFSClientContext構造体.
                packet           送信元パケット情報.

  Returns:      実際のパケットサイズ.
 *---------------------------------------------------------------------------*/
void WFS_CallClientPacketRecvHook(WFSClientContext *context, const WFSPacketBuffer *packet)
{
    int aid = (int)MATH_CTZ((u32)packet->bitmap);
    WBT_CallPacketRecvHook(context->wbt, aid, packet->buffer, packet->length);
}

/*---------------------------------------------------------------------------*
  Name:         WFS_InitClient

  Description:  WFSクライアントコンテキストを初期化.

  Arguments:    context          WFSClientContext構造体.
                userdata         コンテキストに関連付ける任意のユーザ定義値.
                callback         システムイベント通知コールバック.
                                 不要ならNULLを指定する.
                allocator        内部で使用するアロケータ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_InitClient(WFSClientContext *context,
                    void *userdata, WFSEventCallback callback,
                    MIAllocator *allocator)
{
    int     i;
    context->userdata = userdata;
    context->callback = callback;
    context->allocator = allocator;
    context->fat_ready = FALSE;
    /* WBT用変数の初期化 */
    for (i = 0; i < WBT_NUM_OF_AID; ++i)
    {
        context->block_info_table.block_info[i] = &context->block_info[i];
        context->recv_buf_table.recv_buf[i] = NULL;
        context->recv_buf_packet_bmp_table.packet_bitmap[i] = NULL;
    }
    context->recv_pkt_bmp_buf = NULL;
    context->max_file_size = 0;
    context->block_id = 0;
    context->request_buffer = NULL;
    context->table->length = 0;
    context->table->buffer = NULL;
    /* WBT の初期化 */
    WBT_InitContext(context->wbt, context, WFSi_WBTSystemCallback);
    WBT_AddCommandPool(context->wbt, context->wbt_list,
                       sizeof(context->wbt_list) / sizeof(*context->wbt_list));
}

/*---------------------------------------------------------------------------*
  Name:         WFS_EndClient

  Description:  WFSクライアントコンテキストを解放.

  Arguments:    context          WFSClientContext構造体.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_EndClient(WFSClientContext *context)
{
    MI_CallFree(context->allocator, context->recv_pkt_bmp_buf);
    WBT_ResetContext(context->wbt, NULL);
    if (context->table->buffer)
    {
        MI_CallFree(context->allocator, context->table->buffer);
        context->table->buffer = NULL;
    }
    if (context->request_callback)
    {
        (*context->request_callback)(context->request_argument, FALSE, context->request_argument);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WFS_StartClient

  Description:  WFSクライアントコンテキストの通信を開始.

  Arguments:    context          WFSClientContext構造体.
                peer             自身の接続情報.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_StartClient(WFSClientContext *context, const WFSPeerInfo *peer)
{
    WBT_StartChild(context->wbt, peer->aid);
    WFSi_ReceiveTableSequence(context, NULL);
}

/*---------------------------------------------------------------------------*
  Name:         WFS_RequestClientRead

  Description:  サーバへROMイメージ読み込み要求を開始.
                完了時には WFS_EVENT_CLIENT_READ 通知が発生する.

  Arguments:    context          WFSClientContext構造体.
                buffer           読み込みデータの格納先メモリ.
                offset           デバイスの読み込み開始位置.
                length           読み込みサイズ.
                callback         読み込み完了コールバック.
                                 不要ならNULL.
                arg              読み込み完了コールバックに与える引数.
  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_RequestClientRead(WFSClientContext *context, void *buffer, u32 offset,
                           u32 length, WFSRequestClientReadDoneCallback callback,
                           void *arg)
{
    if (context->fat_ready)
    {
        context->request_buffer = buffer;
        context->request_region.offset = offset;
        context->request_region.length = length;
        context->request_callback = callback;
        context->request_argument = arg;
        WFSi_ReallocBitmap(context, (int)length);
        WFSi_ReadRomSequence(context, NULL);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WFS_GetClientReadProgress

  Description:  ROMイメージ読み込み要求の進捗状況を取得.

  Arguments:    context          WFSClientContext構造体.
                current          受信済みパケット数を取得する変数.
                total            受信予定のパケット総数を取得する変数.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_GetClientReadProgress(WFSClientContext *context,int *current, int *total)
{
    WBT_GetDownloadProgress(context->wbt, context->block_id, 0, current, total);
}


/*---------------------------------------------------------------------------*
  $Log: wfs_client.c,v $
  Revision 1.2  2007/06/11 06:39:24  yosizaki
  change WFS_RequestClientRead().

  Revision 1.1  2007/04/13 04:12:37  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
