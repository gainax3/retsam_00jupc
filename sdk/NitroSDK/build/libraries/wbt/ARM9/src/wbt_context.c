/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WBT - libraries
  File:     wbt_context.c

  Copyright 2006,2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/

#include	<nitro.h>
#include	<nitro/wbt.h>
#include	<nitro/wbt/context.h>


/*---------------------------------------------------------------------------*/
/* macros */

// #define PRINTF_DEBUG 1
// #define PRINTF_DEBUG_L1 1
// #define PRINTF_DEBUG_L2 1

#if defined(PRINTF_DEBUG)
#define WBT_DEBUG_OUTPUT0       OS_TPrintf
#else
#define WBT_DEBUG_OUTPUT0(...)  (void)0
#endif

#if defined(PRINTF_DEBUG_L1)
#define WBT_DEBUG_OUTPUT1       OS_TPrintf
#else
#define WBT_DEBUG_OUTPUT1(...)  (void)0
#endif

#if defined(PRINTF_DEBUG_L2)
#define WBT_DEBUG_OUTPUT2       OS_TPrintf
#else
#define WBT_DEBUG_OUTPUT2(...)  (void)0
#endif


/*---------------------------------------------------------------------------*/
/* constants */

/* *INDENT-OFF* */
#define WBT_CMD_0   WBT_CMD_REQ_NONE
#define WBT_COMMAND_REQ(type, pair_type, argument)                  \
    {                                                               \
        (u16) WBT_CMD_REQ_ ## type, (u16) WBT_CMD_ ## pair_type,    \
        sizeof(WBTPacketHeaderFormat) + argument,                   \
        TRUE, FALSE                                                 \
    }
#define WBT_COMMAND_RES(type, pair_type, argument)                  \
    {                                                               \
        (u16) WBT_CMD_RES_ ## type, (u16) WBT_CMD_ ## pair_type,    \
        sizeof(WBTPacketHeaderFormat) + argument,                   \
        FALSE, TRUE                                                 \
    }
#define WBT_COMMAND_MSG(type)                                       \
    {                                                               \
        (u16) WBT_CMD_ ## type, 0, 0, FALSE, FALSE                  \
    }
static const struct
{
    u32     type:8;         /* 自身のコマンド種別 */
    u32     pair_type:8;    /* 対になるコマンド種別 */
    u32     packet;         /* コマンドパケットサイズ (最低長) */
    u32     is_req:1;       /* WBT_CMD_REQ_* なら TRUE */
    u32     is_res:1;       /* WBT_CMD_RES_* なら TRUE */
}
WBTi_CommandTable[] =
{
    WBT_COMMAND_MSG(REQ_NONE),
    WBT_COMMAND_REQ(WAIT,           0,                  0),
    WBT_COMMAND_REQ(SYNC,           RES_SYNC,           sizeof(WBTPacketRequestSyncFormat)),
    WBT_COMMAND_RES(SYNC,           REQ_SYNC,           sizeof(WBTPacketResponseSyncFormat)),
    WBT_COMMAND_REQ(GET_BLOCK,      RES_GET_BLOCK,      sizeof(WBTPacketRequestGetBlockFormat)),
    WBT_COMMAND_RES(GET_BLOCK,      REQ_GET_BLOCK,      sizeof(WBTPacketResponseGetBlockFormat)),
    WBT_COMMAND_REQ(GET_BLOCKINFO,  RES_GET_BLOCKINFO,  sizeof(WBTPacketRequestGetBlockFormat)),
    WBT_COMMAND_RES(GET_BLOCKINFO,  REQ_GET_BLOCKINFO,  sizeof(WBTPacketResponseGetBlockFormat)),
    WBT_COMMAND_REQ(GET_BLOCK_DONE, RES_GET_BLOCK_DONE, sizeof(WBTPacketRequestGetBlockDoneFormat)),
    WBT_COMMAND_RES(GET_BLOCK_DONE, REQ_GET_BLOCK_DONE, sizeof(WBTPacketResponseGetBlockDoneFormat)),
    WBT_COMMAND_REQ(USER_DATA,      RES_USER_DATA,      sizeof(WBTPacketRequestUserDataFormat)),
    WBT_COMMAND_RES(USER_DATA,      REQ_USER_DATA,      0),
    WBT_COMMAND_MSG(SYSTEM_CALLBACK),
    WBT_COMMAND_MSG(PREPARE_SEND_DATA),
    WBT_COMMAND_MSG(REQ_ERROR),
    WBT_COMMAND_MSG(RES_ERROR),
    WBT_COMMAND_MSG(CANCEL),
};
enum { WBT_COMMAND_MAX = sizeof(WBTi_CommandTable) / sizeof(*WBTi_CommandTable) };
#undef WBT_CMD_0
#undef WBT_COMMAND_REQ
#undef WBT_COMMAND_RES
#undef WBT_COMMAND_MSG
/* *INDENT-ON* */


/*---------------------------------------------------------------------------*/
/* functions */

SDK_INLINE int div32(int a)
{
    return (a >> 5);
}

SDK_INLINE int mod32(int a)
{
    return (a & 0x1f);
}

/*---------------------------------------------------------------------------*
  Name:         WBTi_CopySafeMemory

  Description:  メモリ転送または 0 クリア.

  Arguments:    src               転送元バッファまたは NULL.
                dst               転送先バッファ.
                                  src が NULL なら 0 クリアされる.
                len               転送サイズ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE void WBTi_CopySafeMemory(const void *src, void *dst, u32 len)
{
    if (!src)
    {
        MI_CpuFill8(dst, 0x00, len);
    }
    else
    {
        MI_CpuCopy8(src, dst, len);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WBTi_GetFirstIterationAID

  Description:  前回の通信対象を基点にしてループ検索するための準備.

  Arguments:    context           WBTContext構造体.

  Returns:      検索開始位置のAID.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
int WBTi_GetFirstIterationAID(WBTContext *context)
{
    const int   mask = context->req_bitmap;
    /* 無限ループの回避. (前回の通信対象が存在しなければ先頭から検索) */
    if (((1 << context->last_target_aid) & mask) == 0)
    {
        context->last_target_aid = 31 - (int)MATH_CLZ((u32)mask);
    }
    return context->last_target_aid;
}

/*---------------------------------------------------------------------------*
  Name:         WBTi_GetNextIterationAID

  Description:  ループ検索で次の位置にあたるAIDを取得.

  Arguments:    aid               現在のAID.
                mask              検索対象全体のビットマップ.

  Returns:      次のAID.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
int WBTi_GetNextIterationAID(int aid, int mask)
{
    ++aid;
    if ((1 << aid) > mask)
    {
        aid = (int)MATH_CTZ((u32)mask);
    }
    return aid;
}

/*---------------------------------------------------------------------------*
  Name:         WBTi_InitBitmap

  Description:  ビットマップ構造体を初期化.

  Arguments:    pkt_bmp           ビットマップ構造体.
                length            データ長のバイトサイズ.
                bits              ビットマップ用バッファ.
                buffer            データバッファ.

  Returns:      none.
 *---------------------------------------------------------------------------*/
static void WBTi_InitBitmap(WBTContext * work, WBTPacketBitmap * pkt_bmp, s32 length, u32 *bits, u8 *buffer,
                            int packet)
{
    SDK_ASSERT(packet > 0);
    pkt_bmp->length = length;
    pkt_bmp->buffer = buffer;
    pkt_bmp->total = (length + packet - 1) / packet;
    pkt_bmp->count = 0;
    pkt_bmp->bitmap = bits;
    MI_CpuFill8(bits, 0x00, (u32)WBT_GetBitmapLength(work, length));
}

/*---------------------------------------------------------------------------*
  Name:         WBTi_MergeBitmapIndex

  Description:  ビットマップ内の指定インデックスが未受信なら格納.

  Arguments:    pkt_bmp           ビットマップ構造体.
                index             パケットのインデックス.
                packet            パケット単位サイズ.
                src               パケットデータ.

  Returns:      未受信なら格納して TRUE.
 *---------------------------------------------------------------------------*/
static BOOL WBTi_MergeBitmapIndex(WBTPacketBitmap * pkt_bmp, int index, u32 packet, const void *src)
{
    BOOL    retval = FALSE;
    u32     pos = (u32)div32(index);
    u32     bit = (u32)mod32(index);
    if ((pkt_bmp->bitmap[pos] & (1 << bit)) == 0)
    {
        u8     *dst = pkt_bmp->buffer;
        const u32 offset = index * packet;
        const u32 total = (u32)pkt_bmp->length;
        u32     length = (u32)MATH_MIN(packet, total - offset);
        MI_CpuCopy8(src, dst + offset, length);
        pkt_bmp->bitmap[pos] |= (1 << bit);
        pkt_bmp->count += 1;
        retval = TRUE;
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         WBTi_FindBitmapIndex

  Description:  ビットマップ内の未受信インデックスを検索.

  Arguments:    pkt_bmp           ビットマップ構造体.

  Returns:      未受信インデックスまたは -1.
 *---------------------------------------------------------------------------*/
static s32 WBTi_FindBitmapIndex(WBTPacketBitmap * pkt_bmp)
{
    int     last_num;
    int     bit_num;
    u32    *bmp;
    int     num;

    num = pkt_bmp->current + 1;
    if (num >= pkt_bmp->total)
    {
        num = 0;
    }
    last_num = num;
    bmp = pkt_bmp->bitmap + div32(num);
    bit_num = mod32(num);

    for (;;)
    {
        /* 指定インデックスが未受信なら検索完了 */
        if ((*bmp & (u32)((u32)1 << bit_num)) == 0)
        {
            break;
        }
        /* そうでなければ次のインデックスへ */
        else
        {
            /* インデックスの終端に達したら先頭へループ */
            if (++num >= pkt_bmp->total)
            {
                num = 0;
                bit_num = 0;
                bmp = pkt_bmp->bitmap;
            }
            /* 受信管理ビットマップは 32bit ごとに管理 */
            else if (++bit_num >= 32)
            {
                bit_num = 0;
                ++bmp;
            }
            /* 全パケット受信済み */
            if (num == last_num)
            {
                num = -1;
                break;
            }
        }
    }
    return num;
}

/*---------------------------------------------------------------------------*
  Name:         WBTi_GetPacketBuffer

  Description:  指定ブロック内インデックスで表される
                パケットデータのバッファを取得.

  Arguments:    work              WBT 構造体.
                id                要求されたブロックID.
                index             要求されたインデックス.

  Returns:      ブロックデータへのポインタ.
 *---------------------------------------------------------------------------*/
static u8 *WBTi_GetPacketBuffer(WBTContext * work, u32 id, s32 index)
{
    u8     *ptr = NULL;
    WBTBlockInfoList *list = work->list;
    int     count = 0;
    for (; list != NULL; list = list->next)
    {
        if (id < WBT_NUM_MAX_BLOCK_INFO_ID)
        {
            if (id == count)
            {
                /*
                 * CAUTION!:
                 * 内部管理用の構造体をそのまま渡さずここでエンディアンを解決する.
                 * (ただしこれは恒久対策ではなく一時的な回避)
                 */
                static WBTBlockInfo tmp;
                tmp = list->data_info;
                tmp.id = MI_HToLE32(tmp.id);
                tmp.block_size = (s32)MI_HToLE32(tmp.block_size);
                ptr = (u8 *)&tmp;
//                ptr = (u8 *)&list->data_info;
                break;
            }
        }
        else
        {
            if (id == list->data_info.id)
            {
                ptr = (u8 *)list->data_ptr;
                break;
            }
        }
        ++count;
    }
    if (ptr)
    {
        ptr += index * work->my_data_packet_size;
    }
    return ptr;
}

/*---------------------------------------------------------------------------*
  Name:         WBTi_SwitchNextCommand

  Description:  コマンドリストに更新があった場合に呼び出す.
                  - 新規コマンド発行時.
                  - 現在のコマンド完了時.

  Arguments:    work              WBT 構造体.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WBTi_SwitchNextCommand(WBTContext * work)
{
    BOOL    retval = FALSE;

    /* 更新されたコマンドが有効であれば処理 */
    WBTCommand *current = WBT_GetCurrentCommand(work);
    if (current)
    {
        /* コマンドカウンタを増加 (0は避ける) */
        if (++work->my_command_counter == 0)
        {
            ++work->my_command_counter;
        }
        current->my_cmd_counter = work->my_command_counter;

        /* コマンドごとの初期化処理 */
        switch (current->command)
        {

        case WBT_CMD_REQ_GET_BLOCK:
        case WBT_CMD_REQ_GET_BLOCKINFO:
            /* ビットマップを初期化 */
            {
                int     aid;
                for (aid = 0; aid < 16; ++aid)
                {
                    if ((current->target_bmp & (1 << aid)) != 0)
                    {
                        WBTPacketBitmap *pkt_bmp = &work->peer_param[aid].pkt_bmp;
                        WBTi_InitBitmap(work, pkt_bmp, (int)current->get.recv_data_size,
                                        current->get.pkt_bmp_table.packet_bitmap[aid],
                                        current->get.recv_buf_table.recv_buf[aid],
                                        work->peer_data_packet_size);
                        pkt_bmp->current = 0;
                    }
                }
            }
            break;

        case WBT_CMD_REQ_SYNC:
        case WBT_CMD_REQ_USER_DATA:
            /* 初期化の不要なコマンド */
            break;

        default:
            /* ありえないコマンド */
            current->command = WBT_CMD_REQ_NONE;
            break;

        }
        retval = (current->command != WBT_CMD_REQ_NONE);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WBTi_NotifySystemCallback

  Description:  システムコールバック通知.

  Arguments:    work              WBT 構造体.
                event             イベント種別.
                aid               要求受信イベントの場合, 相手局のAID.
                result            処理結果.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WBTi_NotifySystemCallback(WBTContext * work, WBTCommandType event, int aid,
                                      WBTResult result)
{
    BOOL    retval = TRUE;
    WBTRecvToken *peer = &work->peer_param[aid].recv_token;

    /* WBT_CMD_PREPARE_SEND_DATA 以外は 1 回だけ通知 */
    if ((event != WBT_CMD_PREPARE_SEND_DATA) &&
        (peer->token_peer_cmd_counter == peer->last_peer_cmd_counter))
    {
        retval = FALSE;
    }
    /*
     * GetBlockInfo() の処理はライブラリ内部で GetBlock() を使用しているため,
     * この内部処理完了はアプリケーションに通知する必要はない(してはならない).
     */
    else if ((event == WBT_CMD_REQ_GET_BLOCK_DONE) &&
             (peer->token_block_id < WBT_NUM_MAX_BLOCK_INFO_ID))
    {
        retval = FALSE;
    }
    /* 必要ならシステムコールバック発生 */
    if (retval)
    {
        WBTCommand *cmd = &work->system_cmd;
        peer->last_peer_cmd_counter = peer->token_peer_cmd_counter;
        cmd->peer_cmd_counter = peer->token_peer_cmd_counter;   /* デバッグ用 */
        cmd->result = result;
        cmd->event = event;
        cmd->command = WBT_CMD_SYSTEM_CALLBACK;
        cmd->peer_bmp = (u16)(1 << aid);
        /* 新仕様 */
        if (work->callback)
        {
            work->callback(work->userdata, cmd);
        }
        /* 旧仕様 */
        else if (cmd->callback)
        {
            (*cmd->callback) (cmd);
        }
    }
}


/*****************************************************************************
 * パケット送信処理.
 *****************************************************************************/

PLATFORM_ATTRIBUTE_INLINE
    WBTPacketFormat * WBTi_MakeCommandHeader(void *dst, u8 cmd, int target, u8 counter)
{
    WBTPacketFormat *format = (WBTPacketFormat *) dst;
    MI_StoreLE8(&format->header.command, cmd);
    MI_StoreLE16(&format->header.bitmap, (u16)target);
    MI_StoreLE8(&format->header.counter, counter);
    return format;
}

/*---------------------------------------------------------------------------*
  Name:         WBTi_TryCreateResponse

  Description:  要求を受信中であれば応答を生成.

  Arguments:    work              WBT 構造体.
                aid               相手局のAID.
                buf               送信コマンド作成用バッファ.
                size              バッファサイズ.
                command           応答を確認するコマンド.
                foroce_blockinfo  指定 ID が存在しなくても応答する場合は TRUE.

  Returns:      応答が生成されればそのパケット長, そうでなければ 0.
 *---------------------------------------------------------------------------*/
static
    int WBTi_TryCreateResponse(WBTContext * work, int aid, void *buf, int size, int command,
                               BOOL foroce_blockinfo)
{
    int     ret_size = 0;
    WBTRecvToken *token = &work->peer_param[aid].recv_token;
    BOOL    done = FALSE;
    (void)size;
    if (command == WBT_CMD_REQ_SYNC)
    {
        WBTPacketFormat *format = WBTi_MakeCommandHeader(buf, WBT_CMD_RES_SYNC, (1 << aid),
                                                         token->token_peer_cmd_counter);
        /* 自他の区別は送信先が基準である点に注意 */
        MI_StoreLE16(&format->res_sync.block_total, (u16)WBT_GetRegisteredCount(work));
        MI_StoreLE16(&format->res_sync.peer_packet, (u16)work->my_data_packet_size);
        MI_StoreLE16(&format->res_sync.own_packet, (u16)work->peer_data_packet_size);
        ret_size = (int /*temporary */ )sizeof(format->header) + sizeof(format->res_sync);
        done = TRUE;
    }
    else if (command == WBT_CMD_REQ_GET_BLOCK_DONE)
    {
        u32     id = token->token_block_id;
        WBTPacketFormat *format =
            WBTi_MakeCommandHeader(buf, WBT_CMD_RES_GET_BLOCK_DONE, (1 << aid),
                                   token->token_peer_cmd_counter);
        MI_StoreLE32(&format->res_getblock_done.id, id);
        ret_size = (int /*temporary */ )sizeof(format->header) + sizeof(format->res_getblock_done);
        done = TRUE;
        WBT_DEBUG_OUTPUT1("send BlockDone to %d id = %d\n", aid, id);
    }
    else if (command == WBT_CMD_REQ_USER_DATA)
    {
        WBTPacketFormat *format = WBTi_MakeCommandHeader(buf, WBT_CMD_RES_USER_DATA, (1 << aid),
                                                         token->token_peer_cmd_counter);
        ret_size = (int /*temporary */ )sizeof(format->header);
        done = TRUE;
    }
    else if (command == WBT_CMD_REQ_GET_BLOCK)
    {
        /*
         * 要求されたブロックが存在しない場合, 現状では何も行わない.
         * エラー等を新設して通知してもよい.
         */
        u32     id = token->token_block_id;
        /* ここでは id < 1000 でもブロック情報を返したりはしない */
        WBTBlockInfoList *list = work->list;
        for (; list != NULL; list = list->next)
        {
            if (list->data_info.id == id)
            {
                break;
            }
        }
        if (list)
        {
            /*
             * ブロック要求に対する適切な応答インデックスを計算.
             * MP 通信の情報遅延により要求内容が重複しやすいため,
             * 応答側では履歴を管理して過去2回までの重複を避ける.
             */
            s32     index = token->token_block_seq_no;
            s32     block_seq_no;
            /* GetBlockInfo 要求に対しては指定されたインデックスをそのまま応答 */
            if (id >= WBT_NUM_MAX_BLOCK_INFO_ID)
            {
                /*
                 * 指定されたブロックを検索.
                 * CAUTION!:
                 *     唯一この関数を呼び出す箇所では, すでにこの判定を行っている.
                 *     よってこの関数が -1 を返すことはありえない!
                 */
                /* 最近応答したブロックの場合は応答履歴を考慮 */
                if (id == work->last_block_id)
                {
                    const int index_total =
                        (list->data_info.block_size + work->my_data_packet_size -
                         1) / work->my_data_packet_size;
                    int     i;
                    /* 応答履歴に存在しない直近のインデックスを検索 */
                    for (i = 0; (i < 3) && ((index == work->last_seq_no_1) ||
                                            (index == work->last_seq_no_2)); ++i)
                    {
                        if (++index >= index_total)
                        {
                            index = 0;
                        }
                    }
                }
                /*
                 * 今回の応答内容を応答履歴に記録.
                 * CAUTION!:
                 *     この実装だと前回の無関係なブロックのインデックスも
                 *     last_seq_no_2 に残されてしまっている.
                 */
                work->last_block_id = id;
                work->last_seq_no_2 = work->last_seq_no_1;
                work->last_seq_no_1 = index;
            }
            block_seq_no = index;

            {
                u32     packet = work->my_data_packet_size;
                u8     *data_ptr = NULL;
                BOOL    sendable = FALSE;
                if (list->data_ptr)
                {
                    data_ptr = WBTi_GetPacketBuffer(work, id, block_seq_no);
                    WBT_DEBUG_OUTPUT1("send BlockData to %d id = %d seq no = %d pktsize %d\n", aid,
                                      id, index, packet);
                    sendable = TRUE;
                }
                else if (list->block_type == WBT_BLOCK_LIST_TYPE_USER)
                {
                    /* データパケット準備をユーザへ促す通知 */
                    WBTCommand *system_cmd = &work->system_cmd;
                    system_cmd->prepare_send_data.block_id = id;
                    system_cmd->prepare_send_data.block_seq_no = block_seq_no;
                    system_cmd->prepare_send_data.own_packet_size = (s16)packet;
                    system_cmd->prepare_send_data.data_ptr = NULL;
                    WBTi_NotifySystemCallback(work, WBT_CMD_PREPARE_SEND_DATA, aid,
                                              WBT_RESULT_SUCCESS);
                    WBT_DEBUG_OUTPUT1("peer req seq no  = %d seq no = %d dataptr = %p\n", index,
                                      block_seq_no, system_cmd->prepare_send_data.data_ptr);
                    if (system_cmd->prepare_send_data.data_ptr != NULL)
                    {
                        id = system_cmd->prepare_send_data.block_id;
                        block_seq_no = system_cmd->prepare_send_data.block_seq_no;
                        data_ptr = system_cmd->prepare_send_data.data_ptr;
                        packet = system_cmd->prepare_send_data.own_packet_size;
                        sendable = TRUE;
                    }
                }
                /* データが準備がされれば送信 */
                if (sendable)
                {
                    /* CAUTION!: この送信先ビットマップ計算は暫定処理らしい */
                    u16     target = (u16)((WBT_GetAid(work) == WBT_AID_PARENT) ? 0xFFFE : 0x0001);
                    WBTPacketFormat *format =
                        WBTi_MakeCommandHeader(buf, WBT_CMD_RES_GET_BLOCK, target,
                                               token->token_peer_cmd_counter);
                    MI_StoreLE32(&format->res_getblock.id, id);
                    MI_StoreLE32(&format->res_getblock.index, (u32)block_seq_no);
                    WBTi_CopySafeMemory(data_ptr, &format->res_getblock + 1, (u32)packet);
                    ret_size = (int /*temporary */ )(sizeof(format->header) +
                                                     sizeof(format->res_getblock) + packet);
					done = TRUE;
                }
            }
        }
    }
    else if (command == WBT_CMD_REQ_GET_BLOCKINFO)
    {
        u32     id = token->token_block_id;
        s32     index = token->token_block_seq_no;
        int     packet = work->my_data_packet_size;
        u8     *data_ptr = WBTi_GetPacketBuffer(work, id, index);
        WBT_DEBUG_OUTPUT1("send BlockData to %d id = %d seq no = %d pktsize %d\n", aid, id, index,
                          packet);
        /*
         * CAUTION!:
         *     指定されたIDのブロックが無い場合に限り data_ptr が NULL になりうる.
         *     その場合はコールバックを返すか常に 0 データを送る方が良いのではないか?
         *     現在 HEAD で改造中!
         */
        if (foroce_blockinfo || data_ptr)
        {
            /* CAUTION!: この送信先ビットマップ計算は暫定処理らしい */
            u16     target = (u16)((WBT_GetAid(work) == 0) ? 0xFFFF : 0x0001);
            WBTPacketFormat *format = WBTi_MakeCommandHeader(buf, WBT_CMD_RES_GET_BLOCKINFO, target,
                                                             token->token_peer_cmd_counter);
            MI_StoreLE32(&format->res_getblock.id, id);
            MI_StoreLE32(&format->res_getblock.index, (u32)index);
            WBTi_CopySafeMemory(data_ptr, &format->res_getblock + 1, (u32)packet);
            ret_size = (int /*temporary */ )(sizeof(format->header) + sizeof(format->res_getblock) +
                                             packet);
            done = TRUE;
        }
    }

    /* 応答した後は要求内容を破棄する */
    if (done)
    {
        work->req_bitmap &= ~(1 << aid);
    }
    return ret_size;
}

/*---------------------------------------------------------------------------*
  Name:         WBTi_CheckRequest

  Description:  自身のコマンド要求を生成する.

  Arguments:    work              WBT 構造体.
                buffer            データ格納バッファ.
                length            バッファサイズ.

  Returns:      要求を生成したらそのサイズ, そうでなければ 0.
 *---------------------------------------------------------------------------*/
static int WBTi_CheckRequest(WBTContext * work, void *send_buf, int size)
{
    int     send_size = 0;
    WBTCommand *current = WBT_GetCurrentCommand(work);
    (void)size;
    if (current)
    {
        switch (current->command)
        {
        case WBT_CMD_REQ_SYNC:
            {
                WBTPacketFormat *format =
                    WBTi_MakeCommandHeader(send_buf, WBT_CMD_REQ_SYNC, current->target_bmp,
                                           current->my_cmd_counter);
                /* 自他の区別は送信先が基準である点に注意 */
                MI_StoreLE16(&format->req_sync.peer_packet, (u16)work->my_data_packet_size);
                MI_StoreLE16(&format->req_sync.own_packet, (u16)work->peer_data_packet_size);
                send_size = (int /*temporary */ )sizeof(format->header) + sizeof(format->req_sync);
                WBT_DEBUG_OUTPUT0("send ReqSync to 0x%04x cmd counter %d\n",
                                  current->target_bmp, current->my_cmd_counter);
            }
            break;
        case WBT_CMD_REQ_GET_BLOCK:
        case WBT_CMD_REQ_GET_BLOCKINFO:
            {
                int     aid;
                for (aid = 0; aid < 16; ++aid)
                {
                    /* 自分が要求すべき相手かどうか確認 */
                    if ((current->target_bmp & (1 << aid)) != 0)
                    {
                        WBTPacketBitmap *pkt_bmp = &work->peer_param[aid].pkt_bmp;
                        s32     next_seq_no = WBTi_FindBitmapIndex(pkt_bmp);
                        /* 受信完了していれば BlockDone */
                        if (next_seq_no == -1)
                        {
                            WBTPacketFormat *format =
                                WBTi_MakeCommandHeader(send_buf, WBT_CMD_REQ_GET_BLOCK_DONE,
                                                       (1 << aid), current->my_cmd_counter);
                            MI_StoreLE32(&format->req_getblock_done.id, current->get.block_id);
                            send_size = (int /*temporary */ )sizeof(format->header) +
                                sizeof(format->req_getblock_done);
                            WBT_DEBUG_OUTPUT0("send ReqBlockDone to %d 0x%04x\n", aid, (1 << aid));
                        }
                        /* 受信完了していなければ次のブロックを要求 */
                        else
                        {
                            WBTPacketFormat *format =
                                WBTi_MakeCommandHeader(send_buf, current->command,
                                                       current->target_bmp,
                                                       current->my_cmd_counter);
                            MI_StoreLE32(&format->req_getblock.id, current->get.block_id);
                            MI_StoreLE32(&format->req_getblock.index, (u32)next_seq_no);
                            send_size = (int /*temporary */ )sizeof(format->header) +
                                sizeof(format->req_getblock);
                            WBT_DEBUG_OUTPUT1("send ReqBlock id=%d seq=%d\n", current->get.block_id,
                                              next_seq_no);
                        }
                        if (send_size)
                        {
                            break;
                        }
                    }
                }
            }
            break;

        case WBT_CMD_REQ_USER_DATA:
            {
                WBTPacketFormat *format =
                    WBTi_MakeCommandHeader(send_buf, WBT_CMD_REQ_USER_DATA, current->target_bmp,
                                           current->my_cmd_counter);
                MI_StoreLE8(&format->req_userdata.length, current->user_data.size);
                MI_CpuCopy8(current->user_data.data, &format->req_userdata.buffer,
                            WBT_SIZE_USER_DATA);
                send_size =
                    (int /*temporary */ )sizeof(format->header) + sizeof(format->req_userdata);
            }
            break;

        default:
            WBT_DEBUG_OUTPUT0("Unknown User Command:Error %s %s %d\n", __FILE__, __FUNCTION__,
                              __LINE__);
            break;
        }
    }
    return send_size;
}

/*---------------------------------------------------------------------------*
  Name:         WBTi_CheckBlockResponse

  Description:  各AIDからの要求を順番に確認し, 適切なブロック応答を生成する.

  Arguments:    work              WBT 構造体.
                buffer            データ格納バッファ.
                length            バッファサイズ.

  Returns:      ブロック応答を生成したらそのサイズ, そうでなければ 0.
 *---------------------------------------------------------------------------*/
static int WBTi_CheckBlockResponse(WBTContext * work, void *buffer, int length)
{
    int     retval = 0;

    /* 要求中の対象を順に判定する */
    int     mask = work->req_bitmap;
    if (!retval && (mask != 0))
    {
        int     aid = WBTi_GetFirstIterationAID(work);
        do
        {
            aid = WBTi_GetNextIterationAID(aid, mask);
            if ((work->req_bitmap & (1 << aid)) != 0)
            {
                WBTRecvToken *token = &work->peer_param[aid].recv_token;
                if (WBT_CMD_REQ_GET_BLOCK == token->token_command)
                {
                    /*
                     * 応答コマンドを作成試行.
                     * ここまで来て 0 が返るのは,
                     * ・WBT_CMD_REQ_GET_BLOCK
                     *   - 指定 ID のブロックがない.
                     *   - PREPARE コールバックで準備されなかった.
                     * のみ.
                     * しかし WBT_CMD_REQ_GET_BLOCK は
                     * 応答しなくても done にだけはなる.
                     */
                    retval =
                        WBTi_TryCreateResponse(work, aid, buffer, length, WBT_CMD_REQ_GET_BLOCK,
                                               FALSE);
                }
                if (retval)
                {
                    work->last_target_aid = aid;
                }
            }
        }
        while (aid != work->last_target_aid);
    }

    if (!retval && (mask != 0))
    {
        int     aid = WBTi_GetFirstIterationAID(work);
        do
        {
            aid = WBTi_GetNextIterationAID(aid, mask);
            if ((work->req_bitmap & (1 << aid)) != 0)
            {
                WBTRecvToken *token = &work->peer_param[aid].recv_token;
                if (WBT_CMD_REQ_GET_BLOCKINFO == token->token_command)
                {
                    retval =
                        WBTi_TryCreateResponse(work, aid, buffer, length, WBT_CMD_REQ_GET_BLOCKINFO,
                                               TRUE);
                }
                if (retval)
                {
                    work->last_target_aid = aid;
                }
            }
        }
        while (aid != work->last_target_aid);
    }

    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         WBT_CallPacketSendHook

  Description:  送信パケットデータを生成するためのフック関数.

  Arguments:    work              WBT 構造体.
                buffer            データ格納バッファ.
                length            バッファサイズ.

  Returns:      生成されたパケットサイズ.
 *---------------------------------------------------------------------------*/
int WBT_CallPacketSendHook(WBTContext * work, void *buffer, int length, BOOL is_parent)
{
    int     retval = 0;

    if (work->last_target_aid == -1)
    {
        work->last_target_aid = (is_parent ? 1 : 0);
    }

    /* 引数の正当性判定 */
    if (!buffer || (length <= 0))
    {
        return 0;
    }

    /* 各AIDからの要求を順番に確認し, 適切な応答を生成する */
    {
        /*
         * CAUTION!:
         *     応答生成の優先度が (AID順序 > コマンド) なら,
         *     同一AIDから複数のコマンドを受理しないのだから
         *     判定は1回でよいのではないか?
         */
        static const WBTCommandType tbl[] = {
            WBT_CMD_REQ_USER_DATA,
            WBT_CMD_REQ_SYNC,
            WBT_CMD_REQ_GET_BLOCKINFO,
            WBT_CMD_REQ_GET_BLOCK_DONE,
            WBT_CMD_REQ_NONE,
        };
        /* 要求中の対象を順に判定する */
        int     mask = work->req_bitmap;
        if (mask != 0)
        {
            int     aid = WBTi_GetFirstIterationAID(work);
            do
            {
                aid = WBTi_GetNextIterationAID(aid, mask);
                if ((mask & (1 << aid)) != 0)
                {
                    /*
                     * 応答コマンドを作成試行.
                     * ここまで来て 0 が返るのは,
                     * ・WBT_CMD_REQ_GET_BLOCKINFO
                     *   - 指定 ID のブロックがなく, かつ force でない.
                     * のみ.
                     */
                    WBTRecvToken *token = &work->peer_param[aid].recv_token;
                    int     i;
                    for (i = 0; !retval && (tbl[i] != WBT_CMD_REQ_NONE); ++i)
                    {
                        /* WBT_CMD_REQ_*** */
                        if (tbl[i] == token->token_command)
                        {
                            retval =
                                WBTi_TryCreateResponse(work, aid, buffer, length, tbl[i], FALSE);
                        }
                    }
                    if (retval)
                    {
                        work->last_target_aid = aid;
                    }
                }
            }
            while (aid != work->last_target_aid);
        }
    }


    if (!retval)
    {
        /*
         * 親機のコマンド優先度は
         *   (1) 一般応答. (非登録のGetBlockInfoは無視)
         *   (2) 自身の要求.
         *   (3) ブロック応答. (非登録のGetBlockInfoも強制的に応答)
         *   (4) (wait)
         */
        if (is_parent)
        {
            /*
             * ここで 0 が返るのは,
             * - コマンドを発行していない時.
             */
            retval = WBTi_CheckRequest(work, buffer, length);
            if (!retval)
            {
                retval = WBTi_CheckBlockResponse(work, buffer, length);
            }
        }
        /*
         * 子機のコマンド優先度は
         *   (1) 一般応答. (非登録のGetBlockInfoは無視)
         *   (3) ブロック応答. (非登録のGetBlockInfoも強制的に応答)
         *   (2) 自身の要求.
         *   (4) (wait)
         */
        else
        {
            retval = WBTi_CheckBlockResponse(work, buffer, length);
            if (!retval)
            {
                /*
                 * ここで 0 が返るのは,
                 * - コマンドを発行していない時.
                 */
                retval = WBTi_CheckRequest(work, buffer, length);
            }
        }
        if (!retval)
        {
            /* WAIT コマンドは常にカウンタ 0 で送信する */
            int     mask = (is_parent ? 0xFFFE : 0x0001);
            WBTPacketFormat *format =
                WBTi_MakeCommandHeader(buffer, WBT_CMD_REQ_WAIT, (u16)mask, (WBTCommandCounter)0);
            retval = (int /*temporary */ )sizeof(format->header);
        }
    }

    return retval;
}


/*****************************************************************************
 * パケット受信処理.
 *****************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         WBTi_NotifyCompletionCallback

  Description:  ユーザーコマンド完了通知.

  Arguments:    work              WBT 構造体.
                event             イベント種別.
                aid               コマンド完了した相手局のAID.

  Returns:      none.
 *---------------------------------------------------------------------------*/
static void WBTi_NotifyCompletionCallback(WBTContext * work, WBTCommandType event, int aid)
{
    WBTCommandList *list = WBT_GetCurrentCommandList(work);
    WBTCommand *current = WBT_GetCurrentCommand(work);
    WBTRecvToken *token = &work->peer_param[aid].recv_token;
    /* 通知は 1 回だけ発生 */
    if (current->my_cmd_counter == token->token_peer_cmd_counter)
    {
        int     peer_bmp = (1 << aid);
        current->peer_cmd_counter = token->token_peer_cmd_counter;
        current->peer_bmp = (u16)peer_bmp;      /* デバッグ用 */
        if ((current->target_bmp & peer_bmp) != 0)
        {
            /* 完了通知なので結果は常に SUCCESS. (エラーはシステムコールバックへ通知される) */
            current->target_bmp &= ~peer_bmp;
            current->event = event;
            current->result = WBT_RESULT_SUCCESS;
            /* 新仕様 */
            if (list->callback)
            {
                list->callback(work->userdata, current);
            }
            /* 旧仕様 */
            else if (current->callback)
            {
                current->callback(current);
            }
        }
        /* 全対象が応答完了したらコマンド破棄 */
        if (current->target_bmp == 0)
        {
            WBTCommandList *list = work->command;
            work->command = list->next;
            WBT_AddCommandPool(work, list, 1);
            WBTi_SwitchNextCommand(work);
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WBT_CallPacketRecvHook

  Description:  受信パケットデータを解析.

  Arguments:    work              WBT 構造体.
                aid               データ送信元のAID
                buffer            受信データバッファ.
                length            受信データ長.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WBT_CallPacketRecvHook(WBTContext * work, int aid, const void *buffer, int length)
{
    WBTRecvToken *token = &work->peer_param[aid].recv_token;

    /* ここで常に前回の受信状態がクリアされる */
    work->req_bitmap &= ~(1 << aid);

    /* 少なくともコマンドヘッダは必ず存在すべき */
    if (buffer && (length >= sizeof(WBTPacketHeaderFormat)))
    {
        const WBTPacketFormat *format = (const WBTPacketFormat *)buffer;

        u8      command;
        u16     bitmap;

        command = MI_LoadLE8(&format->header.command);
        bitmap = MI_LoadLE16(&format->header.bitmap);
        token->token_peer_cmd_counter = MI_LoadLE8(&format->header.counter);
        token->token_command = command;


        /*
         * 自分あてのパケットならコマンド処理.
         * NOTE: if-else にしているが最終的には関数テーブルにしたい.
         */
        if ((WBT_GetAid(work) != -1) && ((bitmap & (1 << WBT_GetAid(work))) != 0))
        {
            /* 範囲外の未知のコマンドは無視 */
            if (command >= WBT_COMMAND_MAX)
            {
            }
            /* コマンドパケットサイズが最低長に満たない場合は無視 */
            else if (length < WBTi_CommandTable[command].packet)
            {
            }
            /** 要求コマンド */
            else if (WBTi_CommandTable[command].is_req)
            {
                if (command == WBT_CMD_REQ_WAIT)
                {
                }

                else if (command == WBT_CMD_REQ_SYNC)
                {
                    WBTRequestSyncCallback *cb = &work->system_cmd.sync;
                    cb->peer_packet_size = (s16)MI_LoadLE16(&format->req_sync.peer_packet);
                    cb->my_packet_size = (s16)MI_LoadLE16(&format->req_sync.own_packet);
                    cb->num_of_list = 0;        /* 旧仕様上, このメンバのみ要求に含まれない */
                    /* 子機側は常に親機の送受信設定に従う */
                    if (WBT_GetAid(work) != 0)
                    {
                        work->my_data_packet_size = cb->my_packet_size;
                        work->peer_data_packet_size = cb->peer_packet_size;
                    }
                    work->req_bitmap |= (1 << aid);
                }
                /* ユーザ定義データ要求 */
                else if (command == WBT_CMD_REQ_USER_DATA)
                {
                    WBTRecvUserDataCallback *cb = &work->system_cmd.user_data;
                    cb->size = MI_LoadLE8(&format->req_userdata.length);
                    if (cb->size > WBT_SIZE_USER_DATA)
                    {
                        cb->size = 0;
                    }
                    MI_CpuCopy8(format->req_userdata.buffer, cb->data, cb->size);
                    work->req_bitmap |= (1 << aid);
                }
                /* ブロック情報要求またはブロック要求 (同一フォーマット) */
                else if ((command == WBT_CMD_REQ_GET_BLOCK) ||
                         (command == WBT_CMD_REQ_GET_BLOCKINFO))
                {
                    token->token_block_id = MI_LoadLE32(&format->req_getblock.id);
                    token->token_block_seq_no = (s32)MI_LoadLE32(&format->req_getblock.index);
                    work->req_bitmap |= (1 << aid);
                    WBT_DEBUG_OUTPUT1("get req Block from %d id = %d seq no = %d\n", aid, token->token_block_id,
                                      token->token_block_seq_no);
                }
                /* ブロック受信完了通知 */
                else if (command == WBT_CMD_REQ_GET_BLOCK_DONE)
                {
                    WBTGetBlockDoneCallback *cb = &work->system_cmd.blockdone;
                    cb->block_id = MI_LoadLE32(&format->req_getblock_done.id);
                    // 他の子機が実行中の同一IDへの要求に相乗りして
                    // 1回も要求を出すことなく完了を通知されることもある
                    token->token_block_id = MI_LoadLE32(&format->req_getblock_done.id);
                    work->req_bitmap |= (1 << aid);
                }
            }

            /* 応答コマンド */
            else if (WBTi_CommandTable[command].is_res)
            {
                WBTCommand *current = WBT_GetCurrentCommand(work);

                /* 現在コマンド要求中でなければ無視 */
                if (!current)
                {
                }
                /*
                 * CAUTION!:
                 *     REQ_ と RES_ の対応が取れているかどうかの判定処理を統一化したいが,
                 *     GetBlock 系が1対1対応でないので未着手.
                 *     (1) WBT_CMD_RES_GET_BLOCK / WBT_CMD_REQ_GET_BLOCK
                 *     (2) WBT_CMD_RES_GET_BLOCKINFO / WBT_CMD_REQ_GET_BLOCKINFO
                 *     (3) WBT_CMD_RES_GET_BLOCK_DONE / WBT_CMD_REQ_GET_BLOCK, WBT_CMD_REQ_GET_BLOCKINFO
                 *     WBT_CMD_REQ_GET_BLOCK_DONE を送っている時に何かすべき?
                 */
                else
                {
                    /* 同期応答 */
                    if (command == WBT_CMD_RES_SYNC)
                    {
                        if (current->command == WBT_CMD_REQ_SYNC)
                        {
                            current->sync.num_of_list =
                                (s16)MI_LoadLE16(&format->res_sync.block_total);
                            current->sync.peer_packet_size =
                                (s16)MI_LoadLE16(&format->res_sync.peer_packet);
                            current->sync.my_packet_size =
                                (s16)MI_LoadLE16(&format->res_sync.own_packet);

                            /* 子機側は常に親機の送受信設定に従う */
                            if (WBT_GetAid(work) != 0)
                            {
                                work->my_data_packet_size = current->sync.my_packet_size;
                                work->peer_data_packet_size = current->sync.peer_packet_size;
                            }
                            WBT_DEBUG_OUTPUT0("Get res Sync from %d my %d peer %d\n", aid,
                                              current->sync.my_packet_size,
                                              current->sync.peer_packet_size);
                            WBTi_NotifyCompletionCallback(work, (WBTCommandType)command, aid);
                        }
                    }
                    /* 完了通知 */
                    else if (command == WBT_CMD_RES_USER_DATA)
                    {
                        if (current->command == WBT_CMD_REQ_USER_DATA)
                        {
                            WBTi_NotifyCompletionCallback(work, (WBTCommandType)command, aid);
                        }
                    }
                    /* ブロック応答 */
                    else if ((command == WBT_CMD_RES_GET_BLOCK) ||
                             (command == WBT_CMD_RES_GET_BLOCKINFO))
                    {
                        if ((current->command == WBT_CMD_REQ_GET_BLOCK) ||
                            (current->command == WBT_CMD_REQ_GET_BLOCKINFO))
                        {
                            u32     id = MI_LoadLE32(&format->res_getblock.id);
                            s32     index = (s32)MI_LoadLE32(&format->res_getblock.index);

                            /* 要求する ID と一致するか判定 */
                            if (id == current->get.block_id)
                            {
                                /* 要求先からの応答か判定 */
                                if ((current->target_bmp & (1 << aid)) != 0)
                                {
                                    /* 範囲内のインデックスか判定 */
                                    WBTPacketBitmap *pkt_bmp = &work->peer_param[aid].pkt_bmp;
                                    if (index >= pkt_bmp->total)
                                    {
                                        WBT_DEBUG_OUTPUT1
                                            ("%s num of seq over seq no = %d total = %d\n",
                                             __FUNCTION__, index, pkt_bmp->total);
                                        /* セーブに失敗 */
                                        WBTi_NotifySystemCallback(work, WBT_CMD_RES_ERROR, aid,
                                                                  WBT_RESULT_ERROR_SAVE_FAILURE);
                                    }
                                    else
                                    {
                                        /* 未受信のデータなら格納 */
                                        const void *src = (const u8 *)format +
                                            sizeof(format->header) + sizeof(format->res_getblock);
                                        u32     packet = (u32)work->peer_data_packet_size;
                                        if (WBTi_MergeBitmapIndex(pkt_bmp, index, packet, src))
                                        {
                                            pkt_bmp->current = index;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    /* ブロック受信完了通知 */
                    else if (command == WBT_CMD_RES_GET_BLOCK_DONE)
                    {
                        /* 現在の要求に対応した正しい応答であれば完了通知 */
                        if ((current->command == WBT_CMD_REQ_GET_BLOCK) ||
                            (current->command == WBT_CMD_REQ_GET_BLOCKINFO))
                        {
                            u32     id = MI_LoadLE32(&format->res_getblock_done.id);
                            if (current->get.block_id == id)
                            {
                                WBT_DEBUG_OUTPUT1
                                    ("get block my cmd counter = %d peer cmd counter = %d\n",
                                     current->my_cmd_counter, token->token_peer_cmd_counter);
                                WBTi_NotifyCompletionCallback(work,
                                                              (current->command ==
                                                               WBT_CMD_REQ_GET_BLOCK) ?
                                                              WBT_CMD_RES_GET_BLOCK :
                                                              WBT_CMD_RES_GET_BLOCKINFO, aid);
                            }
                            WBT_DEBUG_OUTPUT0("c usr cmd tbmp 0x%x\n", current->target_bmp);
                        }
                    }
                }
            }

            /* 非対応または不正なコマンド */
            else
            {
                WBTi_NotifySystemCallback(work, WBT_CMD_RES_ERROR, aid,
                                          WBT_RESULT_ERROR_UNKNOWN_PACKET_COMMAND);
            }

        }

        /* 要求を受信したらシステムコールバック */
        if ((work->req_bitmap & (1 << aid)) != 0)
        {
            /* ただし GetBlock* コマンドは何度も受信するので通知しない */
            if ((command != WBT_CMD_REQ_GET_BLOCK) && (command != WBT_CMD_REQ_GET_BLOCKINFO))
            {
                WBTi_NotifySystemCallback(work, (WBTCommandType)command, aid, WBT_RESULT_SUCCESS);
            }
        }

    }
}

/*---------------------------------------------------------------------------*
  Name:         WBT_InitContext

  Description:  WBT 構造体を初期化.

  Arguments:    work              WBT 構造体.
                userdata          任意のユーザ定義値.
                callback          システムコールバック.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WBT_InitContext(WBTContext * work, void *userdata, WBTEventCallback callback)
{
    work->userdata = userdata;
    work->callback = callback;

    /* ブロック転送送信履歴の初期化 */
    work->last_block_id = (u32)-1;
    work->last_seq_no_1 = -1;
    work->last_seq_no_2 = -1;

    /* コマンド管理を初期化 */
    work->command = NULL;
    work->command_pool = NULL;
    work->my_command_counter = 0;
    work->last_target_aid = -1;
    work->req_bitmap = 0;
    MI_CpuFill8(&work->system_cmd, 0x00, sizeof(work->system_cmd));
    MI_CpuFill8(work->peer_param, 0x00, sizeof(work->peer_param));

    /* コマンドリストをクリア */
    WBT_ResetContext(work, callback);
}

/*---------------------------------------------------------------------------*
  Name:         WBT_ResetContext

  Description:  WBT 構造体を再初期化.

  Arguments:    work              WBT 構造体.
                callback          システムコールバック.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WBT_ResetContext(WBTContext * work, WBTEventCallback callback)
{
    int     i;

    work->my_aid = -1;
    work->peer_data_packet_size = 0;
    work->my_data_packet_size = 0;

    work->list = NULL;
    work->callback = callback;

    /* 全てのコマンドを破棄 */
    while (work->command)
    {
        WBTCommandList *list = work->command;
        work->command = list->next;
        list->command.command = WBT_CMD_REQ_NONE;
    }

    work->system_cmd.command = WBT_CMD_REQ_NONE;
    work->system_cmd.target_bmp = 0;
    work->system_cmd.peer_bmp = 0;

    for (i = 0; i < 16; ++i)
    {
        work->peer_param[i].recv_token.last_peer_cmd_counter = 0;
    }

}

/*---------------------------------------------------------------------------*
  Name:         WBT_PostCommand

  Description:  コマンドを発行してコマンドキューに追加.

  Arguments:    work              WBT 構造体.
                cmd               コマンド情報が格納された構造体.
                                  コマンド完了までライブラリ内部で管理される.
                bitmap            コマンド発行対象のAIDビットマップ.
                callback          コマンド完了コールバック. 不要なら NULL.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WBT_PostCommand(WBTContext *work, WBTCommandList *list, u16 bitmap,
                     WBTEventCallback callback)
{
    PLATFORM_ENTER_CRITICALSECTION();
    {
        if (list)
        {
            /* コマンドリストの終端に追加 */
            WBTCommandList **pp;
            for (pp = &work->command; *pp; pp = &(*pp)->next)
            {
            }
            *pp = list;
            list->next = NULL;
            list->command.target_bmp = bitmap;
            list->callback = callback;
            /* アイドル時の新規コマンドならここで処理開始 */
            if (work->command == list)
            {
                WBTi_SwitchNextCommand(work);
            }
        }
    }
    PLATFORM_LEAVE_CRITICALSECTION();
}

/*---------------------------------------------------------------------------*
  Name:         WBT_CancelCommand

  Description:  現在処理中のコマンドを中止.

  Arguments:    work              WBT 構造体.
                bitmap            コマンドを中止する相手.

  Returns:      実際に中止された相手を示すビットマップ.
 *---------------------------------------------------------------------------*/
int WBT_CancelCommand(WBTContext * work, int bitmap)
{
    PLATFORM_ENTER_CRITICALSECTION();
    {
        WBTCommandList *list = WBT_GetCurrentCommandList(work);
        WBTCommand *current = WBT_GetCurrentCommand(work);
        if (current)
        {
            int     aid;
            /* 処理中の全AIDについて中止通知 */
            bitmap &= current->target_bmp;
            for (aid = 0;; ++aid)
            {
                int     bit = (1 << aid);
                if (bit > bitmap)
                {
                    break;
                }
                if ((bit & bitmap) == 0)
                {
                    bitmap &= ~bit;
                }
                /* 新仕様 */
                else if (list->callback)
                {
                    current->event = WBT_CMD_CANCEL;
                    current->target_bmp &= ~bit;
                    current->peer_bmp = (u16)bit;
                    list->callback(work->userdata, current);
                }
                /* 旧仕様 */
                else if (current->callback)
                {
                    current->event = WBT_CMD_CANCEL;
                    current->target_bmp &= ~bit;
                    current->peer_bmp = (u16)bit;
                    (*current->callback) (current);
                }
            }
            /* キャンセルしたコマンドを破棄 (WBTi_NotifyCompletionCallbackと同様) */
            if (current->target_bmp == 0)
            {
                WBTCommandList *list = work->command;
                work->command = list->next;
                WBT_AddCommandPool(work, list, 1);
                WBTi_SwitchNextCommand(work);
            }
        }
    }
    PLATFORM_LEAVE_CRITICALSECTION();
    return bitmap;
}

/*---------------------------------------------------------------------------*
  Name:         WBT_GetBitmapLength

  Description:  ブロック転送の制御に必要なビットマップバッファサイズを取得.

  Arguments:    work              WBT 構造体.
                length            転送するブロックの最大サイズ.

  Returns:      必要なビットマップバッファのサイズ.
 *---------------------------------------------------------------------------*/
int WBT_GetBitmapLength(const WBTContext *work, int length)
{
    int     packet = work->peer_data_packet_size;
    SDK_ASSERT(packet > 0);
    return (int)(sizeof(u32) * MATH_ROUNDUP(((length + packet - 1) / packet), 32));
}

/*---------------------------------------------------------------------------*
  Name:         WBT_GetDownloadProgress

  Description:  ブロック転送の進捗状況を取得.

  Arguments:    work              WBT 構造体.
                id                受信ブロックID.
                aid               受信先AID.
                count             受信済みパケット数の格納先.
                total             パケット総数の格納先.

  Returns:      None.
                ブロック転送状態になければ current, total とも 0 を返す.
 *---------------------------------------------------------------------------*/
void WBT_GetDownloadProgress(const WBTContext *work, u32 id, int aid, int *count, int *total)
{
    const WBTCommand *current = WBT_GetCurrentCommand(work);
    if ((current != NULL) &&
        (current->command == WBT_CMD_REQ_GET_BLOCK) && (current->get.block_id == id))
    {
        const WBTPacketBitmap *pkt_bmp = &work->peer_param[aid].pkt_bmp;
        *count = pkt_bmp->count;
        *total = pkt_bmp->total;
    }
    else
    {
        *count = 0;
        *total = 0;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WBT_SetPacketLength

  Description:  パケットサイズを変更.
                親機のみ使用可能.

  Arguments:    work              WBT 構造体.
                own               自身のMP送信パケットサイズ.
                peer              相手のMP送信パケットサイズ.

  Returns:      設定に成功すれば TRUE.
 *---------------------------------------------------------------------------*/
BOOL WBT_SetPacketLength(WBTContext * work, int own, int peer)
{
    BOOL    retval = FALSE;
    WBTCommand *current;

    SDK_ASSERT(own >= WBT_PACKET_SIZE_MIN);
    SDK_ASSERT(peer >= WBT_PACKET_SIZE_MIN);

    current = WBT_GetCurrentCommand(work);
    /* ブロック転送要求中はパケットサイズを変更できない */
    if ((current == NULL) ||
        ((current->command != WBT_CMD_REQ_GET_BLOCK) &&
        (current->command != WBT_CMD_REQ_GET_BLOCKINFO)))
    {
        work->my_data_packet_size = (s16)(own - WBT_PACKET_SIZE_MIN);
        work->peer_data_packet_size = (s16)(peer - WBT_PACKET_SIZE_MIN);
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         WBT_RegisterBlockInfo

  Description:  新規にデータブロックを登録する.

  Arguments:    work              WBT 構造体.
                list              登録に使用するリスト構造体.
                                  Unregister で解放するまでライブラリが使用する.
                id                データブロックに関連付ける一意なID.
                userinfo          データブロックに関連付けるユーザ定義情報.
                                  このポインタが指す先はこの関数内でのみ参照する.
                                  不要な場合は NULL を指定することもできる.
                buffer            ブロックデータを格納したバッファ.
                                  NULL を指定すると, 必要に応じてライブラリから
                                  WBT_CMD_PREPARE_SEND_DATA コールバックが通知される.
                length            ブロックデータのサイズ.
                                  buffer に NULL を指定する場合であっても
                                  この値は正しく指定しておく必要がある.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
WBT_RegisterBlockInfo(WBTContext * work, WBTBlockInfoList *list, u32 id,
                      const void *userinfo, const void *buffer, int length)
{
    PLATFORM_ENTER_CRITICALSECTION();
    {
        WBTBlockInfoList **pp;
        for (pp = &work->list; *pp; pp = &((*pp)->next))
        {
        }
        *pp = list;
        list->next = NULL;
        list->data_info.id = id;
        list->data_info.block_size = length;
        WBTi_CopySafeMemory(userinfo, list->data_info.user_id, WBT_USER_ID_LEN);
        list->data_ptr = (void *)buffer;
        /* 現状不使用なメンバ */
        list->permission_bmp = 0;
        list->block_type = (u16)(buffer ? WBT_BLOCK_LIST_TYPE_COMMON : WBT_BLOCK_LIST_TYPE_USER);
    }
    PLATFORM_LEAVE_CRITICALSECTION();
}


/*---------------------------------------------------------------------------*
  Name:         WBT_UnregisterBlockInfo

  Description:  登録済みのデータブロックを解放する.

  Arguments:    work              WBT 構造体.
                id                解放するデータブロックに関連付けられた一意なID.

  Returns:      解放したリスト構造体または NULL.
 *---------------------------------------------------------------------------*/
WBTBlockInfoList *WBT_UnregisterBlockInfo(WBTContext * work, u32 id)
{
    WBTBlockInfoList *retval = NULL;
    {
        PLATFORM_ENTER_CRITICALSECTION();
        WBTBlockInfoList **pp;
        for (pp = &work->list; *pp; pp = &(*pp)->next)
        {
            if ((*pp)->data_info.id == id)
            {
                retval = *pp;
                *pp = (*pp)->next;
                break;
            }
        }
        PLATFORM_LEAVE_CRITICALSECTION();
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         WBT_GetRegisteredCount

  Description:  登録済みのデータブロック総数を取得.

  Arguments:    work              WBT 構造体.

  Returns:      登録済みのデータブロック総数.
 *---------------------------------------------------------------------------*/
int WBT_GetRegisteredCount(const WBTContext * work)
{
    int     n = 0;
    {
        PLATFORM_ENTER_CRITICALSECTION();
        WBTBlockInfoList *list = work->list;
        for (list = work->list; list; list = list->next)
        {
            ++n;
        }
        PLATFORM_LEAVE_CRITICALSECTION();
    }
    return n;
}


/*---------------------------------------------------------------------------*
  $Log: wbt_context.c,v $
  Revision 1.5  2007/12/06 01:39:01  yosizaki
  fix WBT_CancelCommand

  Revision 1.4  2007/11/22 02:04:46  yosizaki
  fix about GETBLOCK_DONE sequence.

  Revision 1.3  2007/07/30 08:50:29  yosizaki
  fix about GetBlock response.

  Revision 1.2  2007/05/15 03:17:02  yosizaki
  fix about response iterations.

  Revision 1.1  2007/04/10 08:19:45  yosizaki
  initial upload.
s
  $NoKeywords: $
 *---------------------------------------------------------------------------*/
