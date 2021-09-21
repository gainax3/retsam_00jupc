/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - libraries
  File:     mb_child.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_child.c,v $
  Revision 1.51  2007/09/24 23:50:28  yosizaki
  update copyright date.

  Revision 1.50  2007/08/09 08:28:17  takano_makoto
  MBi_CommBeaconRecvCallbackでのNULLポインタアクセスを修正

  Revision 1.49  2006/03/08 10:29:53  kitase_hirotake
  MB_ReadMultiBootParentBssDesc 関数で magicNumber, ver を初期化するように変更

  Revision 1.48  2006/02/20 02:30:49  seiki_masashi
  WMGameInfo.gameNameCount_attribute を attribute に名前変更

  Revision 1.47  2006/01/18 02:11:29  kitase_hirotake
  do-indent

  Revision 1.46  2005/02/28 05:26:24  yosizaki
  do-indent.

  Revision 1.45  2005/02/21 00:39:34  yosizaki
  replace prefix MBw to MBi.

  Revision 1.44  2005/02/18 11:56:03  yosizaki
  fix around hidden warnings.

  Revision 1.43  2004/11/22 12:40:02  takano_makoto
  分割リクエスト送信の処理をmb_blockとして分離

  Revision 1.42  2004/11/10 07:42:00  takano_makoto
  mb_gameinfo.cの中からMBw関数を排除し、WMを駆動する処理と分割ビーコンを操作する処理の切り分けをおこなう

  Revision 1.41  2004/11/02 18:15:36  takano_makoto
  fix comments.

  Revision 1.39  2004/10/28 11:01:42  yada
  just fix comment

  Revision 1.38  2004/10/12 05:39:21  sato_masaki
  WM_APIID_RESET, WM_APIID_END に関するCallbackエラーをMB_ERRCODE_WM_FAILUREに変更。

  Revision 1.37  2004/10/05 10:05:43  sato_masaki
  BOOT_REQを受けた時のMB終了処理を識別する処理の追加。

  Revision 1.36  2004/10/04 13:40:25  terui
  ゲームグループIDをu32型に統一。

  Revision 1.35  2004/09/28 08:53:10  sato_masaki
  - MB_GetChildProgressPercentage()に、割り込み制御を追加。
  - Warningを随所に追加。

  Revision 1.34  2004/09/25 12:12:50  sato_masaki
  IsAbleToRecv()をMBi_IsAbleToRecv()にリネームし、mb_fileinfo.cへ移動。

  Revision 1.33  2004/09/25 05:30:28  sato_masaki
  メインループにおいて呼ばれ得るAPI実行中に、割り込み禁止をする処理を追加。

  Revision 1.32  2004/09/25 04:40:09  sato_masaki
  NitroSDK_IPL_branch 1.24.2.8　とマージ

  Revision 1.31  2004/09/22 09:42:00  sato_masaki
  IPL_branch 1.24.2.6とマージ
  - rename get_minimum_blockno to get_next_blockno
  - optimize for code-size around MBi_CommChildRecvData().
  - 未受信データ検索方法変更

  Revision 1.30  2004/09/21 01:39:15  sato_masaki
  WMに起因するエラーを、MB_ERRCODE_WM_FAILUREに統一。MB_ERRCODE_MP_SENT_FAILUREを廃止。

  Revision 1.29  2004/09/20 13:11:10  sato_masaki
  - MB_COMM_PSTATE_ERROR　に関して、MBErrCode列挙子の値をargに渡すように変更。
  - 送信用関数で、用いていたMB_ERRORを、MB_SENDFUNC_STATE_ERR(ライブラリ内部の定義)に変更。

  Revision 1.28  2004/09/18 12:01:11  sato_masaki
  子機送信ヘッダ定義に、reservedを設ける。

  Revision 1.27  2004/09/17 04:43:31  sato_masaki
  add function MB_ReadMultiBootParentBssDesc()

  Revision 1.26  2004/09/17 04:02:04  miya
  不要な部分の削除

  Revision 1.25  2004/09/16 12:49:27  sato_masaki
  mb.hにて、MBi_***で定義していた関数、型を、MB_***に変更。

  Revision 1.24  2004/09/15 06:51:59  sato_masaki
  WBT定義に纏わる処理を削除。

  Revision 1.23  2004/09/15 06:25:29  sato_masaki
  MB_COMM_P_SENDLEN, MB_COMM_C_SENDLEN の定義に依存していた部分を変数化。

  Revision 1.22  2004/09/15 00:59:04  sato_masaki
  modify Recv Region check.

  Revision 1.21  2004/09/14 13:19:44  sato_masaki
  リクエストデータを断片化して送るように変更。

  Revision 1.20  2004/09/14 06:16:19  sato_masaki
  ワーククリアを#ifdef SDK_FINALROMの時に限定。

  Revision 1.19  2004/09/13 13:14:05  sato_masaki
  - remove member word from MB_CommBlockHeader
  - change cast (WMmpRecvBuf*) -> (WMPortRecvCallback*) in MBi_CommChildRecvData()

  Revision 1.18  2004/09/13 09:26:56  yosiokat
  ・スキャンロック時にロック対象が全く見つからない状態の対策のため、MB_CountGameInfoLifetimeの引数にwmTypeEventを追加。

  Revision 1.17  2004/09/13 08:10:41  sato_masaki
  - MBi_CommCallError() -> MBi_CommCallChildError()にリネーム。
  - MBi_CommRequestFile() の引数にGGIDを追加。

  Revision 1.16  2004/09/13 02:58:40  sato_masaki
  エラーコールバックを呼ぶ関数、MBi_CommCallError()追加。

  Revision 1.15  2004/09/11 11:17:59  sato_masaki
  change mbc to pCwork

  Revision 1.14  2004/09/10 05:26:51  sato_masaki
  内部状態を変えずにコールバックのみを送る関数MBi_CommChangeChildStateCallbackOnly()追加。
  エラーの通知等に使用。

  Revision 1.13  2004/09/10 02:54:44  sato_masaki
  small fix

  Revision 1.12  2004/09/10 02:54:06  sato_masaki
  CALLBACK_WM_STATE定義に関する変更。

  Revision 1.11  2004/09/10 01:58:17  sato_masaki
  mbc->user.playerNoの割り当て、クリア処理を追加。

  Revision 1.10  2004/09/09 14:01:47  sato_masaki
  mb_gameinfo.cにおける、PlayerFlagのフォーマット変更。（作業中）

  Revision 1.9  2004/09/09 10:06:21  sato_masaki
  MBCommCStateにGameInfo関連の項目追加。

  Revision 1.8  2004/09/08 04:36:14  sato_masaki
  MB_CommBlockHeaderから、fileidを削除(mb_common.h)。それに伴う処理変更。

  Revision 1.7  2004/09/07 06:50:12  sato_masaki
  一部のコールバックにarg引数を渡すように変更。
  ブロック受取り時に、IsAbleToRecv()で受取バッファアドレスの正当性評価する機能を追加。

  Revision 1.6  2004/09/07 04:40:16  sato_masaki
  一部のコールバックにて、引数を渡すようにした。
  MB_CommStartDownload()にチェック文を追加。

  Revision 1.5  2004/09/06 09:52:39  sato_masaki
  small fix

  Revision 1.4  2004/09/04 11:02:02  sato_masaki
  キャンセル時に、コードをロードされ得る領域のクリアをする機能追加。

  Revision 1.3  2004/09/04 09:54:13  sato_masaki
  MB_COMM_CSTATE_DLINFO_ACCEPTEDのコールバック引数に
  受信したMBDownloadFileInfoへのポインタを指定するようにした。(主にデバッグ用途)
  
  Revision 1.2  2004/09/04 06:46:15  sato_masaki
  定員超過時のコールバックステート(MB_COMM_*STATE_MEMBER_FULL)を設ける。

  Revision 1.1  2004/09/03 07:04:36  sato_masaki
  ファイルを機能別に分割。


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include "mb_private.h"


// ----------------------------------------------------------------------------
// definition

#define MY_ROUND(n, a)      (((u32) (n) + (a) - 1) & ~((a) - 1))

// --- for child
static int MBi_CommRequestFile(u8 file_no, u32 ggid);
static void MBi_CommChangeChildState(int state, void *arg);
static void MBi_CommChangeChildStateCallbackOnly(int state, void *arg);
static void MBi_CommChildRecvData(void *arg);
static int MBi_CommChildSendData(void);
static void MBi_CommBeaconRecvCallback(MbBeaconMsg msg, MBGameInfoRecvList * gInfop, int index);

// --- miscellany
static BOOL checkRecvFlag(int x);
static void setRecvFlag(int x);
static u16 countRemainingblocks(void);
static u16 get_next_blockno(void);
static BOOL checkPollbmp(u16 pollbmp, u16 child_aid);
static void clearRecvRegion(void);
static void MBi_CommCallChildError(u16 errcode);


/*  ============================================================================

    子機用関数

    ============================================================================*/

/*---------------------------------------------------------------------------*
  Name:         MB_CommDownloadRequest

  Description:  ダウンロード要求を出す（親機接続→File要求→ダウンロードといった、
                一連の動作のトリガとなる関数）
                下のMB_CommDownloadRequestをパックして、mbrsリストのインデックス
                番号で接続要求をするもの。
                インデックス番号を覚えておくことで、接続に失敗した時に内部でその
                親機情報をクリアできる。

  Arguments:    index - 要求するMbBeaconRecvStatusのリストインデックス番号

  Returns:      
 *---------------------------------------------------------------------------*/

int MB_CommDownloadRequest(int index)
{
    const MbBeaconRecvStatus *mbrsp;   // 親機情報受信ステータス
    const WMBssDesc *bssDescp;         // 接続先の親機情報のポインタ
    u8      fileNo;                    // 　〃        に要求するファイルNo.
    int     ret;
    OSIntrMode enabled = OS_DisableInterrupts();        /* 割り込み禁止 */

    mbrsp = MB_GetBeaconRecvStatus();  // 親機情報受信ステータス
    bssDescp = &(mbrsp->list[index].bssDesc);
    fileNo = mbrsp->list[index].gameInfo.fileNo;

    pCwork->connectTargetNo = index;   // 接続を試みる親機のリストNo.を保存。
    pCwork->fileid = fileNo;           // リクエストファイルIDを予め登録
    pCwork->last_recv_seq_no = -1;     //最終受信ブロックシーケンス番号初期化

    // bssDescriptionをワーク内にコピー
    MI_CpuCopy16(bssDescp, &pCwork->bssDescbuf, WM_BSS_DESC_SIZE);

    ret = MBi_CommConnectToParent((const WMBssDesc *)&pCwork->bssDescbuf);      // まずは接続に入る
    (void)OS_RestoreInterrupts(enabled);        /* 割り込み禁止解除 */

    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         MBi_CommRequestFile

  Description:  ファイルナンバーを指定して、要求を出す

  Arguments:    file_no

  Returns:      
 *---------------------------------------------------------------------------*/

static int MBi_CommRequestFile(u8 file_no, u32 ggid)
{
    int     errcode;
    MBCommChildBlockHeader hd;
    u8     *databuf;
    MBCommRequestData req_data;

    /* リクエストするゲームのGGID、UserInfo、Versionを登録 */
    req_data.ggid = ggid;
    MI_CpuCopy8(&pCwork->common.user, &req_data.userinfo, sizeof(MBUserInfo));
    req_data.version = MB_IPL_VERSION;
    req_data.fileid = file_no;

    hd.type = MB_COMM_TYPE_CHILD_FILEREQ;
    // 送信バッファにデータをコピー
    hd.req_data.piece = MBi_SendRequestDataPiece(hd.req_data.data, &req_data);

    databuf = MBi_MakeChildSendBuffer(&hd, (u8 *)pCwork->common.sendbuf);

    if (!databuf)
    {
        return MB_SENDFUNC_STATE_ERR;
    }
    errcode = MBi_BlockHeaderEnd(MB_COMM_CHILD_HEADER_SIZE, 0xFFFF, pCwork->common.sendbuf);
    return errcode;
}

/*---------------------------------------------------------------------------*
  Name:         MB_CommSetChildStateCallback

  Description:  子機イベントコールバックの設定

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/

void MB_CommSetChildStateCallback(MBCommCStateCallbackFunc callback)
{
    OSIntrMode enabled;

    SDK_ASSERT(pCwork != 0);

    enabled = OS_DisableInterrupts();  /* 割り込み禁止 */

    pCwork->child_callback = callback;

    (void)OS_RestoreInterrupts(enabled);        /* 割り込み禁止解除 */
}

/*---------------------------------------------------------------------------*
  Name:         MB_CommGetChildState

  Description:  Download状態の取得

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/

int MB_CommGetChildState(void)
{
    if (pCwork)
    {
        return pCwork->c_comm_state;
    }
    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         MB_GetChildProgressPercentage

  Description:  Download進捗パーセンテージの取得

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/

u16 MB_GetChildProgressPercentage(void)
{
    OSIntrMode enabled;
    u16     ret = 0;

    enabled = OS_DisableInterrupts();  /* 割り込み禁止 */

    if (pCwork->total_block > 0)
    {
        ret = (u16)((u32)(pCwork->got_block * 100) / pCwork->total_block);
    }

    (void)OS_RestoreInterrupts(enabled);        /* 割り込み禁止解除 */

    return ret;                        // TotalBlock数が0のときは、0%を返しておく
}


/*---------------------------------------------------------------------------*
  Name:         MB_CommStartDownload

  Description:  ダウンロードを開始する

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/

BOOL MB_CommStartDownload(void)
{
    OSIntrMode enabled;
    BOOL    ret = FALSE;

    enabled = OS_DisableInterrupts();  /* 割り込み禁止 */

    if (pCwork
        && pCwork->c_comm_state == MB_COMM_CSTATE_DLINFO_ACCEPTED
        && pCwork->user_req == MB_COMM_USER_REQ_NONE)
    {

        pCwork->user_req = MB_COMM_USER_REQ_DL_START;

        ret = TRUE;
    }

    (void)OS_RestoreInterrupts(enabled);        /* 割り込み禁止解除 */
    return ret;
}


/*---------------------------------------------------------------------------*
  Name:         MBi_CommChangeChildState

  Description:  子機状態の変更＆コールバック呼び出し

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/

static void MBi_CommChangeChildState(int state, void *arg)
{
    pCwork->c_comm_state = state;

    MBi_CommChangeChildStateCallbackOnly(state, arg);

}

/*---------------------------------------------------------------------------*
  Name:         MBi_CommChangeChildStateCallbackOnly

  Description:  子機状態通知をコールバック呼び出しのみで行う
                内部の状態は変更せず

  Arguments:    

  Returns:      
 *---------------------------------------------------------------------------*/

static void MBi_CommChangeChildStateCallbackOnly(int state, void *arg)
{
    if (pCwork->child_callback)        // 状態遷移コールバック
    {
        (*pCwork->child_callback) ((u32)state, arg);
    }

}

/*---------------------------------------------------------------------------*
  Name:         MBi_CommChildCallback

  Description:  子機コールバック本体

  Arguments:    type:WM_TYPE event arg:callback argument

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_CommChildCallback(u16 type, void *arg)
{
    MB_COMM_WMEVENT_OUTPUT(type, arg);

    switch (type)
    {
    case MB_CALLBACK_INIT_COMPLETE:
        /* 初期化完了 */
        MBi_CommChangeChildState(MB_COMM_CSTATE_INIT_COMPLETE, arg);

        break;

    case MB_CALLBACK_PARENT_FOUND:
        {
            WMStartScanCallback *pSsc = (WMStartScanCallback *)arg;
            int     parent_no = MBi_GetLastFountParent();
            WMBssDesc *bssDescp = MBi_GetParentBssDesc(parent_no);

            /* 親機発見 */
            /* ビーコン経由で親機ゲーム情報を取得 */
            (void)MB_RecvGameInfoBeacon(MBi_CommBeaconRecvCallback, pSsc->linkLevel, bssDescp);

            MB_CountGameInfoLifetime(MBi_CommBeaconRecvCallback, TRUE);
        }
        break;
    case MB_CALLBACK_PARENT_NOT_FOUND:
        /* 親機ゲーム情報の寿命カウント */
        MB_CountGameInfoLifetime(MBi_CommBeaconRecvCallback, FALSE);
        break;

    case MB_CALLBACK_CONNECTED_TO_PARENT:
        MBi_CommChangeChildState(MB_COMM_CSTATE_CONNECT, arg);
        break;

    case MB_CALLBACK_MP_CHILD_RECV:
        MBi_CommChildRecvData(arg);
        break;

    case MB_CALLBACK_MP_SEND_ENABLE:
        (void)MBi_CommChildSendData();
        break;

    case MB_CALLBACK_END_COMPLETE:
        /* 終了完了 */

        /* BOOTリクエスト受信状態で、MBi_CommEnd()が呼ばれた場合 */
        if (pCwork->c_comm_state == MB_COMM_CSTATE_BOOTREQ_ACCEPTED && pCwork->boot_end_flag == 1)
        {
            // 所定の領域にDownloadFileInfo, bssDescをセット→ローダーで用いる
            MI_CpuCopy16(&pCwork->dl_fileinfo,
                         (void *)MB_DOWNLOAD_FILEINFO_ADDRESS, sizeof(MBDownloadFileInfo));
            MI_CpuCopy16(&pCwork->bssDescbuf, (void *)MB_BSSDESC_ADDRESS, MB_BSSDESC_SIZE);
            MBi_CommChangeChildState(MB_COMM_CSTATE_BOOT_READY, NULL);  // ブート準備完了
        }
        /* それ以外の場合(キャンセル) */
        else
        {
            MBi_CommChangeChildState(MB_COMM_CSTATE_CANCELLED, NULL);   // キャンセル

            /* PlayerNo を0クリア */
            pCwork->common.user.playerNo = 0;
            clearRecvRegion();

            pCwork->c_comm_state = MB_COMM_CSTATE_NONE;
        }

        break;

    case MB_CALLBACK_CONNECT_FAILED:
        /* 接続失敗 */
        /* Callback引数に、WMstartConnectCallback型のargを渡す。 */
        MBi_CommChangeChildState(MB_COMM_CSTATE_CONNECT_FAILED, arg);
        MB_DeleteRecvGameInfo(pCwork->connectTargetNo); // 接続に失敗した親機のゲーム情報を削除。
        pCwork->connectTargetNo = 0;
        (void)MBi_RestartScan();       // コールバック通知後にスキャン再開
        (void)MBi_CommEnd();

        break;

    case MB_CALLBACK_DISCONNECTED_FROM_PARENT:
        /* 切断通知 */
        /* Callback引数に、WMstartConnectCallback型のargを渡す。 */
        MBi_CommChangeChildState(MB_COMM_CSTATE_DISCONNECTED_BY_PARENT, arg);
        (void)MBi_RestartScan();       // コールバック通知後にスキャン再開
        (void)MBi_CommEnd();
        break;

    case MB_CALLBACK_API_ERROR:
        /* ARM9でWM APIをコールした時点での返り値エラー */
        {
            u16     apiid, errcode;

            apiid = ((u16 *)arg)[0];
            errcode = ((u16 *)arg)[1];

            switch (errcode)
            {
            case WM_ERRCODE_INVALID_PARAM:
            case WM_ERRCODE_FAILED:
            case WM_ERRCODE_WM_DISABLE:
            case WM_ERRCODE_NO_DATASET:
            case WM_ERRCODE_FIFO_ERROR:
            case WM_ERRCODE_TIMEOUT:
                MBi_CommCallChildError(MB_ERRCODE_FATAL);
                break;
            case WM_ERRCODE_OPERATING:
            case WM_ERRCODE_ILLEGAL_STATE:
            case WM_ERRCODE_NO_CHILD:
            case WM_ERRCODE_OVER_MAX_ENTRY:
            case WM_ERRCODE_NO_ENTRY:
            case WM_ERRCODE_INVALID_POLLBITMAP:
            case WM_ERRCODE_NO_DATA:
            case WM_ERRCODE_SEND_QUEUE_FULL:
            case WM_ERRCODE_SEND_FAILED:
            default:
                MBi_CommCallChildError(MB_ERRCODE_WM_FAILURE);
                break;
            }
        }
        break;
    case MB_CALLBACK_ERROR:
        {
            /* WM APIコール後に返ってきたコールバックにおけるエラー */
            WMCallback *pWmcb = (WMCallback *)arg;
            switch (pWmcb->apiid)
            {
            case WM_APIID_INITIALIZE:
            case WM_APIID_SET_LIFETIME:
            case WM_APIID_SET_P_PARAM:
            case WM_APIID_SET_BEACON_IND:
            case WM_APIID_START_PARENT:
            case WM_APIID_START_MP:
            case WM_APIID_SET_MP_DATA:
            case WM_APIID_START_DCF:
            case WM_APIID_SET_DCF_DATA:
            case WM_APIID_DISCONNECT:
            case WM_APIID_START_KS:
                /* 以上のエラーは、WM最初期化の必要なエラー */
                MBi_CommCallChildError(MB_ERRCODE_FATAL);
                break;
            case WM_APIID_RESET:
            case WM_APIID_END:
            default:
                /* その他のエラーについては、コールバックエラーとして返す */
                MBi_CommCallChildError(MB_ERRCODE_WM_FAILURE);
                break;
            }
        }
        break;

    default:
        break;
    }

#if ( CALLBACK_WM_STATE == 1 )
    if (pCwork->child_callback)
    {
        (*pCwork->child_callback) ((u32)(MB_COMM_CSTATE_WM_EVENT | type), arg);
    }
#endif

    if (type == MB_CALLBACK_END_COMPLETE)
    {
        // ワークの開放
        MI_CpuClear16(pCwork, sizeof(MB_CommCWork));
        pCwork = NULL;
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBi_CommChildRecvData

  Description:  子機データ受信

  Arguments:    arg : pointer to callback argument

  Returns:      None.
 *---------------------------------------------------------------------------*/

static void MBi_CommChildRecvData(void *arg)
{
    WMPortRecvCallback *bufp = (WMPortRecvCallback *)arg;
    MB_CommCWork *const p_child = pCwork;
    MBCommParentBlockHeader hd;
    u8     *databuf;
    u16     aid = MBi_GetAid();

    // 受信バッファのキャッシュを破棄
//  DC_InvalidateRange( bufp->data, MY_ROUND(bufp->length, 32) );

    databuf = MBi_SetRecvBufferFromParent(&hd, (u8 *)bufp->data);

    MB_DEBUG_OUTPUT("RECV ");
    MB_COMM_TYPE_OUTPUT(hd.type);

    switch (hd.type)                   // 受信Blockタイプによる状態遷移
    {
    case MB_COMM_TYPE_PARENT_SENDSTART:
        // 親機からの送信開始メッセージ
        if (p_child->c_comm_state == MB_COMM_CSTATE_CONNECT)
        {
            MB_DEBUG_OUTPUT("Allowed to request file from parent!\n");
            MBi_CommChangeChildState(MB_COMM_CSTATE_REQ_ENABLE, NULL);
        }
        break;

    case MB_COMM_TYPE_PARENT_KICKREQ: // 親機からのKickメッセージ
        if (p_child->c_comm_state == MB_COMM_CSTATE_REQ_ENABLE)
        {
            MB_DEBUG_OUTPUT("Kicked from parent!\n");
            MBi_CommChangeChildState(MB_COMM_CSTATE_REQ_REFUSED, NULL);
        }
        break;

    case MB_COMM_TYPE_PARENT_MEMBER_FULL:      // 親機からのメンバー超過メッセージ
        if (p_child->c_comm_state == MB_COMM_CSTATE_REQ_ENABLE)
        {
            MB_DEBUG_OUTPUT("Member full!\n");
            MBi_CommChangeChildState(MB_COMM_CSTATE_MEMBER_FULL, NULL);
        }
        break;

    case MB_COMM_TYPE_PARENT_DL_FILEINFO:
        // MbDownloadFileInfoHeaderの受信
        if (p_child->c_comm_state == MB_COMM_CSTATE_REQ_ENABLE)
        {

            // MbDownloadFileInfoHeaderを専用バッファに格納
            MI_CpuCopy8(databuf, &p_child->dl_fileinfo, sizeof(MBDownloadFileInfo));

            MB_DEBUG_OUTPUT("Download File Info has received (Total block num = %d)\n",
                            p_child->total_block);
            if (!MBi_MakeBlockInfoTable(&p_child->blockinfo_table,
                                        (MbDownloadFileInfoHeader *) & p_child->dl_fileinfo))
            {
                /* 受け取ったDownloadFileInfoが不正なものであった */
                MBi_CommCallChildError(MB_ERRCODE_INVALID_DLFILEINFO);
                OS_TWarning("The received DownloadFileInfo is illegal.\n");
                return;
            }

            /* PlayerNo を割り当てる */
            p_child->common.user.playerNo = aid;

            // 総ブロック数を格納
            p_child->total_block = MBi_get_blocknum(&p_child->blockinfo_table);

            /* 受信したMbDownloadFileInfoを引数として渡す。 */
            MBi_CommChangeChildState(MB_COMM_CSTATE_DLINFO_ACCEPTED, (void *)&p_child->dl_fileinfo);
        }

        break;

    case MB_COMM_TYPE_PARENT_DATA:
        /* ブロックデータの受信 */
        if (p_child->c_comm_state == MB_COMM_CSTATE_DLINFO_ACCEPTED
            && p_child->user_req == MB_COMM_USER_REQ_DL_START)
        {
            MBi_CommChangeChildState(MB_COMM_CSTATE_RECV_PROCEED, NULL);
            p_child->user_req = MB_COMM_USER_REQ_NONE;
            /* Stateを変更したら即受信データ取得可 */
        }

        if (p_child->c_comm_state == MB_COMM_CSTATE_RECV_PROCEED)
        {
            u16     block_num;
            MB_BlockInfo bi;

            // MB_COMM_TYPE_DATAの時のみの処理
            block_num = p_child->total_block;

            if (block_num == 0 || block_num >= MB_MAX_BLOCK)
            {

                MBi_CommCallChildError(MB_ERRCODE_INVALID_BLOCK_NUM);

                OS_TWarning("Illegal Number of Block! [%d]\n", block_num);
                return;
            }

            /* ブロックNo.に関する評価 */
            if (hd.seqno < 0 ||
                hd.seqno >= block_num ||
                MBi_get_blockinfo(&bi, &p_child->blockinfo_table, hd.seqno,
                                  &p_child->dl_fileinfo.header) == FALSE)
            {
                /* ブロック番号が不正 */
                MBi_CommCallChildError(MB_ERRCODE_INVALID_BLOCK_NO);
                OS_TWarning("The illegal block No.[%d] has been received! (maxnum %d)\n",
                            hd.seqno, block_num);
                goto CheckRemainBlock;
            }

            if (hd.fid != p_child->fileid)
            {
                /* FileIDが要求しているものと異なる */
                MBi_CommCallChildError(MB_ERRCODE_INVALID_FILE);
                OS_TWarning("Received File ID [%d] differs from what was requested!\n", hd.fid);
                goto CheckRemainBlock;
            }

            if (!MBi_IsAbleToRecv(bi.segment_no, bi.child_address, bi.size))
            {
                /* 受信アドレスが不正 */
                MBi_CommCallChildError(MB_ERRCODE_INVALID_RECV_ADDR);
                OS_TWarning("The receive address of Block No.%d is illegal. [%08x - %08x]\n",
                            hd.seqno, bi.child_address, bi.child_address + bi.size);
                goto CheckRemainBlock;
            }

            /* 受信アドレスチェック後、指定アドレスへコピー */
            if (checkRecvFlag(hd.seqno) == FALSE)
            {
                MB_DEBUG_OUTPUT("DATA : BLOCK(%d)/REMAIN(%d), Recv address[%x] size[%x]\n",
                                hd.seqno, countRemainingblocks(), bi.child_address, bi.size);
                MI_CpuCopy8(databuf, (void *)bi.child_address, bi.size);
                p_child->got_block++;
                setRecvFlag(hd.seqno);
            }

          CheckRemainBlock:
            /* 全てのブロックが受け取れたか？ */
            if (0 == countRemainingblocks())
            {
                MBi_CommChangeChildState(MB_COMM_CSTATE_RECV_COMPLETE, NULL);   // 受信完了
            }
        }
        break;

    case MB_COMM_TYPE_PARENT_BOOTREQ:
        if (p_child->c_comm_state == MB_COMM_CSTATE_RECV_COMPLETE)
        {
            MBi_CommChangeChildState(MB_COMM_CSTATE_BOOTREQ_ACCEPTED, NULL);
        }
        else if (p_child->c_comm_state == MB_COMM_CSTATE_BOOTREQ_ACCEPTED)
        {
            p_child->boot_end_flag = 1;
            (void)MBi_CommEnd();       // 通信を終了させる
        }
        break;
    default:
        break;
    }
    return;
}

/*---------------------------------------------------------------------------*
  Name:         MBi_CommChildSendData

  Description:  子機データ送信

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/

static int MBi_CommChildSendData(void)
{
    u16     block_num = pCwork->total_block;
    MBCommChildBlockHeader hd;
    int     errcode = 0;
    u16     pollbmp = 0xffff;

    switch (pCwork->c_comm_state)
    {
    default:
        // MP通信確立のため、DUMMY MPを送信
        hd.type = MB_COMM_TYPE_DUMMY;
        (void)MBi_MakeChildSendBuffer(&hd, (u8 *)pCwork->common.sendbuf);
        errcode = MBi_BlockHeaderEnd(MB_COMM_CHILD_HEADER_SIZE, pollbmp, pCwork->common.sendbuf);
        break;

    case MB_COMM_CSTATE_REQ_ENABLE:
        {
            const MbBeaconRecvStatus *mbrsp = MB_GetBeaconRecvStatus();
            // FileRequestの送信
            errcode = MBi_CommRequestFile(pCwork->fileid,
                                          mbrsp->list[pCwork->connectTargetNo].gameInfo.ggid);
            MB_DEBUG_OUTPUT("Requested File (errcode:%d)\n", errcode);
            // RequestFileでMPをセットしている
        }
        break;

    case MB_COMM_CSTATE_DLINFO_ACCEPTED:
        // DownloadInfo受領メッセージ
        hd.type = MB_COMM_TYPE_CHILD_ACCEPT_FILEINFO;
        (void)MBi_MakeChildSendBuffer(&hd, (u8 *)pCwork->common.sendbuf);
        errcode = MBi_BlockHeaderEnd(MB_COMM_CHILD_HEADER_SIZE, pollbmp, pCwork->common.sendbuf);
        break;

    case MB_COMM_CSTATE_RECV_PROCEED:
        // ブロック転送継続メッセージ
        MI_CpuClear8(&hd, sizeof(MBCommChildBlockHeader));
        hd.type = MB_COMM_TYPE_CHILD_CONTINUE;
        hd.data.req = get_next_blockno();
        hd.data.reserved[0] = (u8)(0x00ff & pCwork->got_block); // Lo
        hd.data.reserved[1] = (u8)((0xff00 & pCwork->got_block) >> 8);  // Hi
        (void)MBi_MakeChildSendBuffer(&hd, (u8 *)pCwork->common.sendbuf);
        errcode = MBi_BlockHeaderEnd(MB_COMM_CHILD_HEADER_SIZE, pollbmp, pCwork->common.sendbuf);
        break;

    case MB_COMM_CSTATE_RECV_COMPLETE:
        // ブロック転送停止メッセージ(親からBOOTREQが来るまで送りつづける)
        hd.type = MB_COMM_TYPE_CHILD_STOPREQ;
        (void)MBi_MakeChildSendBuffer(&hd, (u8 *)pCwork->common.sendbuf);
        errcode = MBi_BlockHeaderEnd(MB_COMM_CHILD_HEADER_SIZE, pollbmp, pCwork->common.sendbuf);
        break;

    case MB_COMM_CSTATE_BOOTREQ_ACCEPTED:
        hd.type = MB_COMM_TYPE_CHILD_BOOTREQ_ACCEPTED;
        (void)MBi_MakeChildSendBuffer(&hd, (u8 *)pCwork->common.sendbuf);
        errcode = MBi_BlockHeaderEnd(MB_COMM_CHILD_HEADER_SIZE, pollbmp, pCwork->common.sendbuf);
        break;
    }

    return errcode;
}

/*---------------------------------------------------------------------------*
  Name:         MBi_CommBeaconRecvCallback

  Description:  子機のビーコン受信コールバック

  Arguments:    msg    : ビーコン受信メッセージ
                gInfop : 親機ゲーム情報
                index  : ビーコンインデクス

  Returns:      None.
 *---------------------------------------------------------------------------*/

static void MBi_CommBeaconRecvCallback(MbBeaconMsg msg, MBGameInfoRecvList * gInfop, int index)
{
#pragma unused(index)
    void* arg = (gInfop == NULL)? NULL : (void *)&gInfop->gameInfo;
    
    switch (msg)
    {
    case MB_BC_MSG_GINFO_VALIDATED:
        MBi_CommChangeChildStateCallbackOnly(MB_COMM_CSTATE_GAMEINFO_VALIDATED,
                                             arg);
        MB_DEBUG_OUTPUT("Parent Info Enable\n");
        break;
    case MB_BC_MSG_GINFO_INVALIDATED:
        MBi_CommChangeChildStateCallbackOnly(MB_COMM_CSTATE_GAMEINFO_INVALIDATED,
                                             arg);
        break;
    case MB_BC_MSG_GINFO_LOST:
        MBi_CommChangeChildStateCallbackOnly(MB_COMM_CSTATE_GAMEINFO_LOST,
                                             arg);
        break;
    case MB_BC_MSG_GINFO_LIST_FULL:
        MBi_CommChangeChildStateCallbackOnly(MB_COMM_CSTATE_GAMEINFO_LIST_FULL,
                                             arg);
        break;
    }
}

/*---------------------------------------------------------------------------*
  Name:         MB_ReadMultiBootParentBssDesc

  Description:  setup WMBssDesc structure information
                with MB_GetMultiBootParentBssDesc() data,
                in order to use by WM_StartConnect().

  Arguments:    p_desc            pointer to destination WMBssDesc
                parent_max_size   max packet length of parent in MP-protocol.
                                    (must be equal to parent's WMParentParam!)
                child_max_size    max packet length of child in MP-protocol.
                                    (must be equal to parent's WMParentParam!)
                ks_flag           if use key-sharing mode, TRUE.
                                    (must be equal to parent's WMParentParam!)
                cs_flag           if use continuous mode, TRUE.
                                    (must be equal to parent's WMParentParam!)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MB_ReadMultiBootParentBssDesc(WMBssDesc *p_desc,
                                   u16 parent_max_size, u16 child_max_size, BOOL ks_flag,
                                   BOOL cs_flag)
{
    const MBParentBssDesc *parentInfo = MB_GetMultiBootParentBssDesc();
    SDK_NULL_ASSERT(parentInfo);
    MI_CpuCopy8(parentInfo, p_desc, sizeof(MBParentBssDesc));

    p_desc->gameInfoLength = 0x10;
    p_desc->gameInfo.magicNumber = 0x0001;
    p_desc->gameInfo.ver = 0;
    p_desc->gameInfo.ggid =
        (u32)(*(const u16 *)(&p_desc->ssid[0]) | (*(const u16 *)(&p_desc->ssid[2]) << 16));
    p_desc->gameInfo.tgid = *(const u16 *)(&p_desc->ssid[4]);
    p_desc->gameInfo.userGameInfoLength = 0;
    p_desc->gameInfo.parentMaxSize = parent_max_size;
    p_desc->gameInfo.childMaxSize = child_max_size;
    MI_WriteByte(&p_desc->gameInfo.attribute,
                 (u8)((ks_flag ? WM_ATTR_FLAG_KS : 0) |
                      (cs_flag ? WM_ATTR_FLAG_CS : 0) | WM_ATTR_FLAG_ENTRY));
}


/*  ============================================================================

    miscellany functions

    ============================================================================*/

static BOOL checkRecvFlag(int x)
{
    int     pos = (x >> 3);
    int     bit = (x & 0x7);

    SDK_ASSERT(x < MB_MAX_BLOCK);

    if (pCwork->recvflag[pos] & (1 << bit))
    {
        return TRUE;
    }
    return FALSE;
}

static void setRecvFlag(int x)
{
    int     pos = (x >> 3);
    int     bit = (x & 0x7);

    SDK_ASSERT(x < MB_MAX_BLOCK);

    pCwork->recvflag[pos] |= (1 << bit);

    pCwork->last_recv_seq_no = x;
}


static u16 countRemainingblocks(void)
{
    return (u16)(pCwork->total_block - pCwork->got_block);
}

static u16 get_next_blockno(void)
{
    int     req;
    int     search_count = 0;
    req = pCwork->last_recv_seq_no;
    req++;

    while (1)
    {
        if (req < 0 || req >= pCwork->total_block)
        {
            req = 0;
        }
        else if (checkRecvFlag(req))
        {
            req++;
        }
        else
        {
            return (u16)req;
        }

        if (pCwork->last_recv_seq_no == req)
        {
            return (u16)(pCwork->total_block);
        }
        search_count++;

        if (search_count > 1000)
        {
            pCwork->last_recv_seq_no = req;
            return (u16)req;
        }
    }
}

// pollbmpに指定した子機AIDのビットが立っているかをチェック
static BOOL checkPollbmp(u16 pollbmp, u16 child_aid)
{
    if (pollbmp & (u16)(1 << child_aid))
    {
        return TRUE;
    }
    return FALSE;
}

/* 受信領域をクリア */
static void clearRecvRegion(void)
{
    /* FINALROM以外の時は、ダウンロード可能領域をクリアしないようにする． */
#ifdef SDK_FINALROM
    /* ARM9ロード可能領域をクリア */
    MI_CpuClearFast((void *)MB_LOAD_AREA_LO, MB_LOAD_MAX_SIZE);
    /* ARM7ロードバッファアドレス領域をクリア */
    MI_CpuClearFast((void *)MB_ARM7_STATIC_RECV_BUFFER, MB_ARM7_STATIC_RECV_BUFFER_SIZE);
    /* MBbssDesc, MBDownloadFileInfo 領域をクリア */
    MI_CpuClear16((void *)MB_BSSDESC_ADDRESS,
                  MB_DOWNLOAD_FILEINFO_ADDRESS + MB_DOWNLOAD_FILEINFO_SIZE - MB_BSSDESC_ADDRESS);
    /* ROM Header 領域をクリア */
    MI_CpuClear16((void *)MB_ROM_HEADER_ADDRESS, ROM_HEADER_SIZE_FULL);
#endif

}


static void MBi_CommCallChildError(u16 errcode)
{
    MBErrorStatus e_stat;
    e_stat.errcode = errcode;

    MBi_CommChangeChildStateCallbackOnly(MB_COMM_CSTATE_ERROR, &e_stat);

}
