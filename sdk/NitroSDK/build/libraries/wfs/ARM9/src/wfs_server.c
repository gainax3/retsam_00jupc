/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WFS - libraries
  File:     wfs_server.c

  Copyright 2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/


#include <nitro/wfs/server.h>


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         WFSi_NotifySegmentEvent

  Description:  WFS_EVENT_SERVER_SEGMENT_REQUESTイベントを通知する.

  Arguments:    context          WFSServerContext構造体.
                argument         イベント引数.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WFSi_NotifySegmentEvent(WFSServerContext *context, void *argument)
{
    if (context->thread_work)
    {
        (*context->thread_hook)(context->thread_work, argument);
    }
    else if (context->callback)
    {
        (*context->callback)(context, WFS_EVENT_SERVER_SEGMENT_REQUEST, argument);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WFSi_WBTCallback

  Description:  サーバ側のWBTイベント通知.

  Arguments:    userdata         WFSServerContext構造体.
                uc               WBTのイベント引数.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WFSi_WBTCallback(void *userdata, WBTCommand *uc)
{
    WFSServerContext *work = (WFSServerContext *)userdata;
    const int aid = (int)MATH_CTZ(uc->peer_bmp);

    switch (uc->command)
    {

    default:
        WFS_DEBUG_OUTPUT(("invalid WBT callback (command = %d)", uc->command));
        break;

    case WBT_CMD_REQ_USER_DATA:
        /* 全員へメッセージ送信完了したか確認 */
        if (!uc->target_bmp)
        {
            work->msg_busy = FALSE;
        }
        break;

    case WBT_CMD_SYSTEM_CALLBACK:
        /* インジケーション */

        switch (uc->event)
        {

        default:
            WFS_DEBUG_OUTPUT(("unknown WBT system callback (event = %d)", uc->event));
            break;

        case WBT_CMD_REQ_SYNC:
        case WBT_CMD_REQ_GET_BLOCK_DONE:
            break;

        case WBT_CMD_REQ_USER_DATA:
            /* メッセージ受信通知 */
            {
                /* 受信したメッセージを保存 */
                const WFSMessageFormat *const msg = (const WFSMessageFormat *)uc->user_data.data;
                work->recv_msg[aid] = *msg;

                switch (msg->type)
                {

                case WFS_MSG_LOCK_REQ:
                    /* 読み込みロック要求 */
                    {
                        const u32 offset = MI_LEToH32(msg->arg1);
                        const u32 length = MI_LEToH32_BITFIELD(24, msg->arg2);
                        const int packet = (int)((msg->packet_hi << 8) | msg->packet_lo);

                        WFS_DEBUG_OUTPUT(("WBT-MSG(LOCK):recv [offset=0x%08X, length=0x%08X] (AID=%d, packet=%d)",
                                         offset, length, aid, packet));
                        /* パケットサイズ変更中の場合は拒否応答 */
                        if (work->is_changing ||
                            (packet != WBT_GetParentPacketLength(work->wbt) + WBT_PACKET_SIZE_MIN))
                        {
                            work->is_changing = TRUE;
                            work->deny_bitmap |= (1 << aid);
                        }
                        /* 既存のロック領域と一致するならそれを参照 */
                        else
                        {
                            WFSLockInfo *file = NULL;
                            int     index;
                            for (index = 0; (1 << index) <= work->use_bitmap; ++index)
                            {
                                if(((1 << index) & work->use_bitmap) != 0)
                                {
                                    if ((work->list[index].offset == offset) &&
                                        (work->list[index].length == length))
                                    {
                                        file = &work->list[index];
                                        ++file->ref;
                                        break;
                                    }

                                }

                            }
                            /* 新規のロック領域であれば空きリストから確保 */
                            if (!file)
                            {
                                index = (int)MATH_CTZ((u32)~work->use_bitmap);
                                if (index < WFS_LOCK_HANDLE_MAX)
                                {
                                    PLATFORM_ENTER_CRITICALSECTION();
                                    work->use_bitmap |= (1 << index);
                                    file = &work->list[index];
                                    file->ref = 1;
                                    file->offset = offset;
                                    file->length = length;
                                    /* 新規ファイルなので登録済みリストへ移行 */
                                    WBT_RegisterBlockInfo(work->wbt, &file->info,
                                                          (u32)(WFS_LOCKED_BLOCK_INDEX + index),
                                                          NULL, NULL, (int)file->length);
                                    file->ack_seq = 0;
                                    PLATFORM_LEAVE_CRITICALSECTION();
                                    /* 先頭だけは先読み要求しておく */
                                    {
                                        WFSSegmentBuffer    segment[1];
                                        segment->offset = file->offset;
                                        segment->length = (u32)WBT_GetParentPacketLength(work->wbt);
                                        segment->buffer = NULL;
                                        WFSi_NotifySegmentEvent(work, segment);
                                    }
                                }
                                else
                                {
                                    OS_TPanic("internal-error (no available lock handles)");
                                }
                            }
                            work->ack_bitmap |= (1 << aid);
                            work->recv_msg[aid].arg1 = MI_HToLE32((u32)(WFS_LOCKED_BLOCK_INDEX + index));
                        }
                        work->busy_bitmap |= (1 << aid);
                    }
                    break;

                case WFS_MSG_UNLOCK_REQ:
                    /* CLOSEFILE 要求 */
                    {
                        PLATFORM_ENTER_CRITICALSECTION();
                        u32     id = MI_LEToH32(msg->arg1);
                        u32     index = id - WFS_LOCKED_BLOCK_INDEX;
                        if (index < WFS_LOCK_HANDLE_MAX)
                        {
                            WFSLockInfo *file = &work->list[index];
                            /* 全ての参照が消えたら解放処理 */
                            if (--file->ref <= 0)
                            {
                                (void)WBT_UnregisterBlockInfo(work->wbt, id);
                                work->use_bitmap &= ~(1 << index);
                            }
                        }
                        work->ack_bitmap |= (1 << aid);
                        PLATFORM_LEAVE_CRITICALSECTION();
                        WFS_DEBUG_OUTPUT(("WBT-MSG(UNLOCK):recv [id=0x%08X] (AID=%d)", id, aid));
                    }
                    break;

                }
            }
            break;

        case WBT_CMD_PREPARE_SEND_DATA:
            /* GETBLOCK バッファ準備要求 */
            {
                WBTPrepareSendDataCallback *const p_prep = &uc->prepare_send_data;
                u32     id = p_prep->block_id;
                p_prep->data_ptr = NULL;
                /* 正当なファイルであれば応答 */
                id -= WFS_LOCKED_BLOCK_INDEX;
                if (id < WFS_LOCK_HANDLE_MAX)
                {
                    WFSLockInfo *file = &work->list[id];
                    /* 前回の要求を今回応答, 今回の要求は次回応答 */
                    WFSSegmentBuffer    segment[1];
                    const u32 length = p_prep->own_packet_size;
                    const u32 current = file->ack_seq;
                    const u32 next = (u32)p_prep->block_seq_no;
                    file->ack_seq = next;
                    /* 次回の準備 */
                    segment->offset = file->offset + length * next;
                    segment->length = length;
                    segment->buffer = NULL;
                    WFSi_NotifySegmentEvent(work, segment);
                    /* 今回の要求 */
                    segment->offset = file->offset + length * current;
                    segment->length = length;
                    segment->buffer = work->cache_hit_buf;
                    WFSi_NotifySegmentEvent(work, segment);
                    if (segment->buffer != NULL)
                    {
                        p_prep->block_seq_no = (s32)current;
                        p_prep->data_ptr = segment->buffer;
                    }
                }
            }
            break;

        }
        break;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WFS_CallServerConnectHook

  Description:  サーバ側の接続通知.

  Arguments:    context          WFSServerContext構造体.
                peer             接続した通信先の情報.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_CallServerConnectHook(WFSServerContext *context, const WFSPeerInfo *peer)
{
    /*
     * 全てのクライアントがWFSを使用するとは限らないので,
     * ここでの接続通知は無視してPacketRecvを接続通知とみなす.
     */
    (void)context;
    (void)peer;
}

/*---------------------------------------------------------------------------*
  Name:         WFS_CallServerDisconnectHook

  Description:  サーバ側の切断通知.

  Arguments:    context          WFSServerContext構造体.
                peer             切断した通信先の情報.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_CallServerDisconnectHook(WFSServerContext *context, const WFSPeerInfo *peer)
{
    const int bit = (1 << peer->aid);
    context->all_bitmap &= ~bit;
    (void)WBT_CancelCommand(context->wbt, bit);
}

/*---------------------------------------------------------------------------*
  Name:         WFS_CallServerPacketSendHook

  Description:  サーバ側のパケット送信可能タイミング通知.

  Arguments:    context          WFSServerContext構造体.
                packet           送信パケット設定.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_CallServerPacketSendHook(WFSServerContext *context, WFSPacketBuffer *packet)
{
    /* メッセージ送信可能であれば未応答の処理を再開 */
    if (!context->msg_busy)
    {
        /* 現在の有効な通信状況を各種ビットマップへ反映する */
        context->ack_bitmap &= context->all_bitmap;
        context->sync_bitmap &= context->all_bitmap;
        context->busy_bitmap &= context->all_bitmap;
        context->deny_bitmap &= context->all_bitmap;
        /* パケットサイズ変更は受理中の処理が全て完了するまで待つ */
        if (context->is_changing && !context->use_bitmap)
        {
            /* パケットサイズ変更 */
            context->is_changing = FALSE;
            (void)WBT_SetPacketLength(context->wbt, context->new_packet, WBT_PACKET_SIZE_MIN);
            /* 旧サイズによる要求へはここでまとめて拒否応答 */
            if (context->deny_bitmap)
            {
                WFS_DEBUG_OUTPUT(("WBT-MSG(LOCK):deny [packet-length renewal] (BITMAP=%d)", context->deny_bitmap));
                (void)WFS_SendMessageLOCK_ACK(context->wbt, WFSi_WBTCallback, context->deny_bitmap, 0);
                context->msg_busy = TRUE;
                context->deny_bitmap = 0;
            }
        }
        /* パケットサイズ変更以前に受け付けた要求はそのまま対応する */
        else if (context->ack_bitmap)
        {
            int     bitmap = context->ack_bitmap;
            WFSMessageFormat *msg = NULL;
            int     i;
            const int sync = context->sync_bitmap;
            const BOOL is_sync = (sync && ((bitmap & sync) == sync));
            /* 同期指定を受けた子機群が揃えば一括応答 */
            if (is_sync)
            {
                bitmap = sync;
            }
            /* そうでなければ通常応答 */
            else
            {
                bitmap &= ~sync;
            }
            /* 該当する応答可能子機を検索する */
            for (i = 0;; ++i)
            {
                const int bit = (1 << i);
                if (bit > bitmap)
                {
                    break;
                }
                if ((bit & bitmap) != 0)
                {
                    /* aid の小さい順に応答可能子機を検索 */
                    if (!msg)
                    {
                        msg = &context->recv_msg[i];
                    }
                    /* 同種の応答であれば一括送信. */
                    else if ((msg->type == context->recv_msg[i].type) &&
                             (msg->arg1 == context->recv_msg[i].arg1))
                    {
                    }
                    /* そうでない子機に対しては今回は保留. */
                    else
                    {
                        bitmap &= ~bit;
                    }
                }
            }
            /*
             * NOTE:
             *   「同期を指定したにも関わらず要求内容が異なる」
             *   というアプリ側不具合の状態においては
             *   ・単にどれかの同期内容がズレているだけなのか
             *   ・ズレているのならどれを先に応答すれば復旧するのか
             *   ・そもそもズレでなく要求内容が致命的に異なるのか
             *   を判断することが不可能なので,
             *   「警告を発しつつ同期指定を自動解除」
             *   という対処で復旧する.
             */
            if (is_sync && (bitmap != sync))
            {
                context->sync_bitmap = 0;
                OS_TWarning("[WFS] specified synchronous-access failed! "
                            "(then synchronous-setting was reset)");
            }
            /* 今回選択された応答を送信 */
            if (msg)
            {
                switch (msg->type)
                {
                case WFS_MSG_LOCK_REQ:
                    (void)WFS_SendMessageLOCK_ACK(context->wbt, WFSi_WBTCallback, bitmap,
                                                  MI_LEToH32(msg->arg1));
                    break;
                case WFS_MSG_UNLOCK_REQ:
                    (void)WFS_SendMessageUNLOCK_ACK(context->wbt, WFSi_WBTCallback, bitmap,
                                                    MI_LEToH32(msg->arg1));
                    context->busy_bitmap &= ~bitmap;
                    break;
                }
                context->msg_busy = TRUE;
                context->ack_bitmap &= ~bitmap;
            }
        }
    }

    /* ひととおりの最新状況を更新してからWBTを呼び出す */
    packet->length = WBT_CallPacketSendHook(context->wbt, packet->buffer, packet->length, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         WFS_CallServerPacketRecvHook

  Description:  サーバ側のパケット受信通知.

  Arguments:    context          WFSServerContext構造体.
                packet           送信元パケット情報.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_CallServerPacketRecvHook(WFSServerContext *context, const WFSPacketBuffer *packet)
{
    int aid = (int)MATH_CTZ((u32)packet->bitmap);
    const void *buffer = packet->buffer;
    int length = packet->length;
    /*
     * 親機からの不要なポート通信で無関係なクライアントに影響を与えないよう
     * クライアントからの実際のパケット受信によって接続を検出する.
     */
    context->all_bitmap |= (1 << aid);
    WBT_CallPacketRecvHook(context->wbt, aid, buffer, length);
}

/*---------------------------------------------------------------------------*
  Name:         WFS_InitServer

  Description:  WFSサーバコンテキストを初期化.

  Arguments:    context          WFSServerContext構造体.
                userdata         コンテキストに関連付ける任意のユーザ定義値.
                callback         システムイベント通知コールバック.
                                 不要ならNULLを指定する.
                allocator        内部で使用するアロケータ.
                packet           親機の初期パケットサイズ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_InitServer(WFSServerContext *context,
                    void *userdata, WFSEventCallback callback,
                    MIAllocator *allocator, int packet)
{
    /* 基本設定の初期化 */
    MI_CpuClear8(context, sizeof(*context));
    context->userdata = userdata;
    context->callback = callback;
    context->allocator = allocator;

    /* 内部状態の初期化 */
    context->new_packet = packet;
    context->table->buffer = NULL;
    context->table->length = 0;
    context->sync_bitmap = 0;
    context->ack_bitmap = 0;
    context->msg_busy = FALSE;
    context->all_bitmap = 1;
    context->busy_bitmap = 0;
    context->is_changing = FALSE;
    context->deny_bitmap = 0;
    context->use_bitmap = 0;
    context->thread_work = NULL;
    context->thread_hook = NULL;

    /* WBT の初期化 */
    WBT_InitContext(context->wbt, context, WFSi_WBTCallback);
    WBT_AddCommandPool(context->wbt, context->wbt_list,
                       sizeof(context->wbt_list) / sizeof(*context->wbt_list));
    WBT_StartParent(context->wbt, packet, WBT_PACKET_SIZE_MIN);
}

/*---------------------------------------------------------------------------*
  Name:         WFS_EndServer

  Description:  WFSサーバコンテキストを解放.

  Arguments:    context          WFSServerContext構造体.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_EndServer(WFSServerContext *context)
{
    if (context->thread_work)
    {
        (*context->thread_hook)(context->thread_work, NULL);
        MI_CallFree(context->allocator, context->thread_work);
        context->thread_work = NULL;
    }
    WBT_ResetContext(context->wbt, NULL);
    if (context->table->buffer)
    {
        MI_CallFree(context->allocator, context->table->buffer);
        context->table->buffer = NULL;
        context->table->length = 0;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WFS_RegisterServerTable

  Description:  デバイスからROMファイルテーブルをロードしてサーバに登録.

  Arguments:    context          WFSServerContext構造体.
                device           NTRバイナリが格納されているデバイス.
                fatbase          NTRバイナリが配置されているデバイス内オフセット.
                overlay          マージしたいオーバーレイを含むNTRバイナリが
                                 配置されているデバイス内オフセット.
                                 (複数のROMファイルテーブルをマージしないなら
                                  この値はfatbaseと同じ)

  Returns:      ROMファイルテーブルを正しくロードし登録できればTRUE.
 *---------------------------------------------------------------------------*/
BOOL WFS_RegisterServerTable(WFSServerContext *context,
                             MIDevice *device, u32 fatbase, u32 overlay)
{
    BOOL    retval = FALSE;
    /* 複数のファイルテーブルを登録することはできない */
    if (context->table->buffer)
    {
        OS_TWarning("table is already registered.\n");
    }
    /* デバイスからROMファイルテーブルをロード */
    else if (WFS_LoadTable(context->table, context->allocator, device, fatbase, overlay))
    {
        /* ROMファイルテーブルを登録 */
        WBT_RegisterBlockInfo(context->wbt, context->table_info,
                              WFS_TABLE_BLOCK_INDEX, NULL,
                              context->table->buffer,
                              (int)context->table->length);
        retval = TRUE;
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         WFS_GetServerPacketLength

  Description:  サーバ送信パケットサイズを取得.

  Arguments:    context          WFSServerContext構造体.

  Returns:      現在設定されているパケットサイズ.
 *---------------------------------------------------------------------------*/
int WFS_GetServerPacketLength(const WFSServerContext *context)
{
    return context->new_packet;
}

/*---------------------------------------------------------------------------*
  Name:         WFS_SetServerPacketLength

  Description:  サーバ送信パケットサイズを設定.

  Arguments:    context          WFSServerContext構造体.
                length           設定するパケットサイズ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_SetServerPacketLength(WFSServerContext *context, int length)
{
    SDK_ASSERT(length >= WBT_PACKET_SIZE_MIN);
    {
        PLATFORM_ENTER_CRITICALSECTION();
        context->new_packet = length;
        context->is_changing = TRUE;
        PLATFORM_LEAVE_CRITICALSECTION();
    }
}

/*---------------------------------------------------------------------------*
  Name:         WFS_SetServerSync

  Description:  サーバ側でアクセス同期を取るクライアント群を設定.
                この関数は, 同一のファイル群を同じ順序でアクセスすることが
                明らかに保証されているクライアント群へ応答を同期するにより
                WBTライブラリの特性を活かした効率的な転送を実現する.
                ただし, 同期開始のタイミングが論理的に安全でない場合
                応答がずれてデッドロックする点に注意する必要がある.

  Arguments:    context          WFSServerContext構造体.
                bitmap           同期させるクライアントのAIDビットマップ.
                                 0を指定すると同期は行われない. (デフォルト)
                                 最下位ビット 1 は常に無視される.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_SetServerSync(WFSServerContext *context, int bitmap)
{
    PLATFORM_ENTER_CRITICALSECTION();
    context->sync_bitmap = (bitmap & ~1);
    PLATFORM_LEAVE_CRITICALSECTION();
}


/*---------------------------------------------------------------------------*
  $Log: wfs_server.c,v $
  Revision 1.6  2007/10/04 05:36:31  yosizaki
  fix about WFS_EndServer

  Revision 1.5  2007/06/14 13:15:27  yosizaki
  add hook to a thread-proc

  Revision 1.4  2007/06/11 10:23:32  yosizaki
  small fix.

  Revision 1.3  2007/06/11 06:40:00  yosizaki
  add WFS_GetServerPacketLength().

  Revision 1.2  2007/04/17 00:01:06  yosizaki
  rename some structures.

  Revision 1.1  2007/04/13 04:12:37  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
