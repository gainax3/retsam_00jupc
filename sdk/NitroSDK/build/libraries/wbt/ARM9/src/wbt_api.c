/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WBT - libraries
  File:     wbt_api.c

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

/*---------------------------------------------------------------------------*
	変数定義
 *---------------------------------------------------------------------------*/

static BOOL wbt_initialize_flag = FALSE;

static WBTContext wbti_command_work[1];

/* 2 段のコマンドキュー */
static WBTCommandList cmd_q[2];


/*---------------------------------------------------------------------------*
	関数定義
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         WBT_PrintBTList

  Description:  ブロック情報リストをOS_Printfで表示する

  Arguments:    none

  Returns:      none
 *---------------------------------------------------------------------------*/
void WBT_PrintBTList(void)
{
    WBTBlockInfoList *list = wbti_command_work->list;
    for (; list != NULL; list = list->next)
    {
        OS_Printf("BTList id = %d\n", list->data_info.id);
        OS_Printf("  data size %d\n", list->data_info.block_size);
        OS_Printf("  uid %s\n", list->data_info.user_id);
        OS_Printf("  info ptr = %p\n", &(list->data_info));
    }
}

/*---------------------------------------------------------------------------*
  Name:         WBT_AidbitmapToAid

  Description:  AIDビットマップをAIDに変換する（最下位ビットのみ見る）

  Arguments:    abmp - AIDビットマップ

  Returns:      int  - AID
 *---------------------------------------------------------------------------*/
int WBT_AidbitmapToAid(WBTAidBitmap abmp)
{
    return abmp ? (int)MATH_CTZ(abmp) : -1;
}

/*---------------------------------------------------------------------------*
  Name:         WBT_InitParent

  Description:  親機用ブロック転送初期化関数

  Arguments:    send_packet_size - 送信パケットのサイズ
                recv_packet_size - 受信パケットのサイズ
                callback  - 相手局からのリクエスト発生時にかかるコールバック関数

  Returns:      none.
 *---------------------------------------------------------------------------*/
void WBT_InitParent(int send_packet_size, int recv_packet_size, WBTCallback callback)
{
    PLATFORM_ENTER_CRITICALSECTION();
    if (!wbt_initialize_flag)
    {
        wbt_initialize_flag = TRUE;
        WBT_InitContext(wbti_command_work, NULL, NULL);
        wbti_command_work->system_cmd.callback = callback;
        /* コマンドプールを初期化 */
        MI_CpuFill8(cmd_q, 0x00, sizeof(cmd_q));
        WBT_AddCommandPool(wbti_command_work, cmd_q, sizeof(cmd_q) / sizeof(*cmd_q));
        WBT_StartParent(wbti_command_work, send_packet_size, recv_packet_size);
    }
    PLATFORM_LEAVE_CRITICALSECTION();
}

/*---------------------------------------------------------------------------*
  Name:         WBT_InitChild

  Description:  子機用ブロック転送初期化関数

  Arguments:    callback - 相手局からのリクエスト発生時にかかるコールバック関数

  Returns:      none.
 *---------------------------------------------------------------------------*/
void WBT_InitChild(WBTCallback callback)
{
    PLATFORM_ENTER_CRITICALSECTION();
    if (!wbt_initialize_flag)
    {
        wbt_initialize_flag = TRUE;
        WBT_InitContext(wbti_command_work, NULL, NULL);
        wbti_command_work->system_cmd.callback = callback;
        /* コマンドプールを初期化 */
        MI_CpuFill8(cmd_q, 0x00, sizeof(cmd_q));
        WBT_AddCommandPool(wbti_command_work, cmd_q, sizeof(cmd_q) / sizeof(*cmd_q));
    }
    PLATFORM_LEAVE_CRITICALSECTION();
}

/*---------------------------------------------------------------------------*
  Name:         WBT_End

  Description:  親機子機共用ブロック転送終了関数

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WBT_End(void)
{
    PLATFORM_ENTER_CRITICALSECTION();
    if (wbt_initialize_flag)
    {
        wbt_initialize_flag = FALSE;
        wbti_command_work->system_cmd.callback = NULL;
        WBT_ResetContext(wbti_command_work, NULL);
    }
    PLATFORM_LEAVE_CRITICALSECTION();
}

/*---------------------------------------------------------------------------*
  Name:         WBT_SetOwnAid

  Description:  自局のAIDを設定する

  Arguments:    aid - 自局のAID

  Returns:      none.
 *---------------------------------------------------------------------------*/
void WBT_SetOwnAid(int aid)
{
    WBTContext *const work = wbti_command_work;
    if (WBT_GetAid(work) == -1)
    {
        WBT_StartChild(work, aid);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WBT_GetOwnAid

  Description:  自局のAIDを得る

  Arguments:    none.

  Returns:      int - 自局のAID
 *---------------------------------------------------------------------------*/
int WBT_GetOwnAid(void)
{
    const WBTContext *const work = wbti_command_work;
    return WBT_GetAid(work);
}

/*---------------------------------------------------------------------------*
  Name:         WBT_CalcPacketbitmapSize

  Description:  パケット受信番号記録用バッファのサイズを計算する
                （子機の場合は親機と同期を取った後にコールしなければならない）

  Arguments:    block_size - 受信するブロックのサイズ

  Returns:      int - パケット受信番号記録用バッファサイズ（バイト）
 *---------------------------------------------------------------------------*/
int WBT_CalcPacketbitmapSize(int block_size)
{
    return WBT_GetBitmapLength(wbti_command_work, block_size);
}

/*---------------------------------------------------------------------------*
  Name:         WBT_GetCurrentDownloadProgress

  Description:  現在ブロック受信の進行度合いを見る

  Arguments:    block_id       - 受信中のブロックID
                aid            - 送信相手のAID
                *current_count - 受信完了したパケットカウント
                *total_count   - 全パケットカウント

  Returns:      BOOL - 成功／失敗
 *---------------------------------------------------------------------------*/
BOOL WBT_GetCurrentDownloadProgress(u32 block_id, int aid, int *current_count, int *total_count)
{
    WBT_GetDownloadProgress(wbti_command_work, block_id, aid, current_count, total_count);
    return (*total_count != 0);
}

/*---------------------------------------------------------------------------*
  Name:         WBT_SetPacketSize

  Description:  親機用送受信パケットサイズ変更関数（WBT_InitParent後に変更する場合に使用）

  Arguments:    send_packet_size - 送信パケットサイズ
                recv_packet_size - 受信パケットサイズ

  Returns:      BOOL   - FALSE サイズ変更に失敗
                         TRUE  サイズ変更に成功
 *---------------------------------------------------------------------------*/
BOOL WBT_SetPacketSize(int send_packet_size, int recv_packet_size)
{
    return WBT_SetPacketLength(wbti_command_work, send_packet_size, recv_packet_size);
}

/*---------------------------------------------------------------------------*
  Name:         WBT_NumOfRegisteredBlock

  Description:  登録済みのブロック数を返す

  Arguments:    none.

  Returns:      WBTBlockNumEntry - ブロック数
 *---------------------------------------------------------------------------*/
int WBT_NumOfRegisteredBlock(void)
{
    return WBT_GetRegisteredCount(wbti_command_work);
}

/*---------------------------------------------------------------------------*
  Name:         WBT_MpParentSendHook

  Description:  親機用の送信データを作る関数

  Arguments:    sendbuf   - 送信バッファ
                send_size - 送信バッファサイズ

  Returns:      int - ブロック転送送信サイズ
 *---------------------------------------------------------------------------*/
int WBT_MpParentSendHook(void *sendbuf, int send_size)
{
    SDK_ASSERT(wbt_initialize_flag);
    return WBT_CallPacketSendHook(wbti_command_work, sendbuf, send_size, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         WBT_MpChildSendHook

  Description:  子機の送信データを作る関数

  Arguments:    sendbuf   - 送信バッファ
                send_size - 送信バッファサイズ

  Returns:      int - ブロック転送送信サイズ
 *---------------------------------------------------------------------------*/
int WBT_MpChildSendHook(void *sendbuf, int send_size)
{
    SDK_ASSERT(wbt_initialize_flag);
    return WBT_CallPacketSendHook(wbti_command_work, sendbuf, send_size, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         WBT_MpParentRecvHook

  Description:  親機用の受信データを解析する関数

  Arguments:    recv_buf  - 受信バッファ
                recv_size - 受信バッファサイズ
                aid       - 受信相手局のAID

  Returns:      none.
 *---------------------------------------------------------------------------*/
void WBT_MpParentRecvHook(const void *buf, int size, int aid)
{
    SDK_ASSERT(wbt_initialize_flag);
    WBT_CallPacketRecvHook(wbti_command_work, aid, buf, size);
}

/*---------------------------------------------------------------------------*
  Name:         WBT_MpChildRecvHook

  Description:  子機用の受信データを解析する関数

  Arguments:    recv_buf  - 受信バッファ
                recv_size - 受信バッファサイズ

  Returns:      none.
 *---------------------------------------------------------------------------*/
void WBT_MpChildRecvHook(const void *buf, int size)
{
    SDK_ASSERT(wbt_initialize_flag);
    WBT_CallPacketRecvHook(wbti_command_work, 0, buf, size);
}

/*---------------------------------------------------------------------------*
  Name:         WBT_RegisterBlock

  Description:  送信（配信）可能なブロックを登録する

  Arguments:    block_info_list - ブロック情報登録用構造体
                block_id        - ブロックID
                user_id         - ユーザー定義情報
                data_ptr        - データへのポインタ（ＮＵＬＬの場合、相手局から要求がくるたびに
                                  ユーザーコールバックがかかり、ユーザーはそのコールバック関数内で
                                  データポインタを設定できる）
                data_size       - データサイズ
                permission_bmp  - 配信許可ビットマップ（０で許可「予定」）

  Returns:      BOOL   - TRUE  登録成功
                         FALSE block_idは既に登録済み
 *---------------------------------------------------------------------------*/
BOOL
WBT_RegisterBlock(WBTBlockInfoList *block_info_list, WBTBlockId block_id, const void *user_id,
                  const void *data_ptr, int data_size, WBTAidBitmap permission_bmp)
{
    (void)permission_bmp;
    SDK_ASSERT(wbt_initialize_flag);
    WBT_RegisterBlockInfo(wbti_command_work, block_info_list, block_id, user_id,
                          data_ptr, data_size);
    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         WBT_UnregisterBlock

  Description:  ブロックの配信登録をはずす

  Arguments:    block_id - 配信停止するブロックID

  Returns:      WBTBlockInfoList - ブロック情報登録用構造体
 *---------------------------------------------------------------------------*/
WBTBlockInfoList *WBT_UnregisterBlock(WBTBlockId block_id)
{
    SDK_ASSERT(wbt_initialize_flag);
    return WBT_UnregisterBlockInfo(wbti_command_work, block_id);
}

/*---------------------------------------------------------------------------*
  Name:         WBT_RequstSync

  Description:  相手局と同期を取る（ブロック転送の開始時に必ず呼ばなければならない）

  Arguments:    target   - 同期を取る相手局（AIDビットマップで指定する）
                callback - 同期を取った後にかかるコールバック関数

  Returns:      BOOL   - FALSE 以前のコマンドが実行中
                         TRUE  コマンド発行成功
 *---------------------------------------------------------------------------*/
BOOL WBT_RequestSync(WBTAidBitmap target, WBTCallback callback)
{
    WBTCommandList *list = WBT_AllocCommandList(wbti_command_work);
    if (list)
    {
        list->command.callback = callback;
        WBT_CreateCommandSYNC(wbti_command_work, list);
        WBT_PostCommand(wbti_command_work, list, target, NULL);
    }
    return (list != NULL);
}

/*---------------------------------------------------------------------------*
  Name:         WBT_GetBlockInfo

  Description:  ブロック情報を取得する

  Arguments:    target           - ブロック情報を要求する相手局（AIDビットマップで指定する）
                block_info_no    - ブロック情報番号
                block_info_table - 取得したブロック情報を格納するバッファへのポインタテーブル
                callback         - ブロック情報取得後にかかるコールバック関数

  Returns:      BOOL   - FALSE 以前のコマンドが実行中
                         TRUE  コマンド発行成功
 *---------------------------------------------------------------------------*/
BOOL
WBT_GetBlockInfo(WBTAidBitmap target, int block_info_no, WBTBlockInfoTable *block_info_table,
                 WBTCallback callback)
{
    WBTCommandList *list = WBT_AllocCommandList(wbti_command_work);
    if (list)
    {
        list->command.callback = callback;
        WBT_CreateCommandINFO(wbti_command_work, list, block_info_no, block_info_table);
        WBT_PostCommand(wbti_command_work, list, target, NULL);
    }
    return (list != NULL);
}

/*---------------------------------------------------------------------------*
  Name:         WBT_GetBlock

  Description:  ブロックを取得する
                
  Arguments:    target         - ブロックを要求する相手局（AIDビットマップで指定する）
                block_id       - ブロックID
                recv_buf_table - 受信したブロックを格納するバッファへのポインタテーブル
                recv_size      - 受信ブロックサイズ
                p_bmp_table    - パケット受信番号記録用バッファへのポインタテーブル
                callback       - ブロック取得後にかかるコールバック関数

  Returns:      BOOL   - FALSE 以前のコマンドが実行中
                         TRUE  コマンド発行成功
 *---------------------------------------------------------------------------*/
BOOL
WBT_GetBlock(WBTAidBitmap target, WBTBlockId block_id, WBTRecvBufTable *recv_buf_table,
             u32 recv_size, WBTPacketBitmapTable *p_bmp_table, WBTCallback callback)
{
    WBTCommandList *list = WBT_AllocCommandList(wbti_command_work);
    if (list)
    {
        list->command.callback = callback;
        WBT_CreateCommandGET(wbti_command_work, list, block_id, recv_size, recv_buf_table, p_bmp_table);
        WBT_PostCommand(wbti_command_work, list, target, NULL);
    }
    return (list != NULL);
}

/*---------------------------------------------------------------------------*
  Name:         WBT_PutUserData

  Description:  相手局に９バイト以下のデータを送る
                
  Arguments:    target    - 相手局（AIDビットマップで指定する）
                user_data - 送りたいデータのポインタ
                size      - データサイズ
                callback  - コールバック関数

  Returns:      BOOL   - FALSE 以前のコマンドが実行中
                         TRUE  コマンド発行成功
 *---------------------------------------------------------------------------*/
BOOL WBT_PutUserData(WBTAidBitmap target, const void *user_data, int size, WBTCallback callback)
{
    WBTCommandList *list = WBT_AllocCommandList(wbti_command_work);
    if (list)
    {
        list->command.callback = callback;
        WBT_CreateCommandMSG(wbti_command_work, list, user_data, (u32)size);
        WBT_PostCommand(wbti_command_work, list, target, NULL);
    }
    return (list != NULL);
}

/*---------------------------------------------------------------------------*
  Name:         WBT_CancelCurrentCommand

  Description:  現在発行中のＷＢＴコマンドを中断する
                
  Arguments:    target    - 相手局（AIDビットマップで指定する）

  Returns:      BOOL   - FALSE キャンセルするコマンドがない
                         TRUE  キャンセル成功
 *---------------------------------------------------------------------------*/
BOOL WBT_CancelCurrentCommand(WBTAidBitmap cancel_target)
{
    SDK_ASSERT(wbt_initialize_flag);
    return (WBT_CancelCommand(wbti_command_work, cancel_target) != 0);
}


/*---------------------------------------------------------------------------*
  $Log: wbt_api.c,v $
  Revision 1.2  2007/04/10 23:50:11  yosizaki
  enable WBT_AidbitmapToAid().

  Revision 1.1  2007/04/10 08:19:45  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
