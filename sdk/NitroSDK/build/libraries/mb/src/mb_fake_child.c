/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - libraries
  File:     mb_fake_child.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_fake_child.c,v $
  Revision 1.19  2007/04/18 04:02:18  yosizaki
  add MB_FakeSetCompareGGIDCallback.

  Revision 1.18  2007/02/20 00:28:10  kitase_hirotake
  indent source

  Revision 1.17  2006/11/16 10:30:18  takano_makoto
  MB_FakeSetVerboseScanCallback()関数を追加

  Revision 1.16  2006/03/13 06:33:33  yosizaki
  add MB_FAKESCAN_PARENT_BEACON.

  Revision 1.15  2006/02/20 02:31:03  seiki_masashi
  WMGameInfo.gameNameCount_attribute を attribute に名前変更

  Revision 1.14  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.13  2005/12/01 09:15:13  takano_makoto
  MB_FAKE_WORK_SIZEのサイズチェックを追加

  Revision 1.12  2005/02/28 05:26:24  yosizaki
  do-indent.

  Revision 1.11  2005/02/21 00:39:34  yosizaki
  replace prefix MBw to MBi.

  Revision 1.10  2005/01/11 07:41:13  takano_makoto
  fix copyright header.

  Revision 1.9  2005/01/11 02:24:33  takano_makoto
  Lock中の親のデータ受信時に、Timeoutカウンタをリセットするように修正

  Revision 1.8  2005/01/07 02:55:30  takano_makoto
  デバッグ出力を追加

  Revision 1.7  2004/11/29 05:46:01  takano_makoto
  送受信バッファサイズを変更

  Revision 1.6  2004/11/29 04:53:08  takano_makoto
  WM_StartMPのsendBufferと WM_SetMPDataのSendBufferに同じ領域が使用されていたのを修正。

  Revision 1.5  2004/11/29 02:55:30  takano_makoto
  32byte align不具合修正

  Revision 1.4  2004/11/25 12:30:28  takano_makoto
  print debugをOFFに変更

  Revision 1.3  2004/11/24 13:00:35  takano_makoto
  エラー処理を追加

  Revision 1.2  2004/11/23 23:51:20  takano_makoto
  FINALROMビルド時のWarningを除去

  Revision 1.1  2004/11/22 12:38:30  takano_makoto
  Initial update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include "mb_common.h"
#include "mb_block.h"
#include "mb_child.h"
#include "mb_private.h"
#include "mb_wm_base.h"
#include "mb_fake_child.h"
#include "mb_wm.h"

#define MB_FAKE_PRINT   (0)

#if (MB_FAKE_PRINT == 1)
#define MB_FAKE_OUTPUT    OS_TPrintf
#else
#define MB_FAKE_OUTPUT( ... ) ((void)0)
#endif

#define MY_ROUND(n, a)      (((u32) (n) + (a) - 1) & ~((a) - 1))

typedef struct
{
    u32     sendBuf[WM_SIZE_CHILD_SEND_BUFFER(MB_COMM_PARENT_RECV_MAX, FALSE) / sizeof(u32)] ATTRIBUTE_ALIGN(32);       // 送信バッファ 0x400 byte
    u32     recvBuf[WM_SIZE_CHILD_RECEIVE_BUFFER(MB_COMM_PARENT_SEND_MAX, FALSE) / sizeof(u32)] ATTRIBUTE_ALIGN(32);    // 受信バッファ 0xF00 byte
    u32     sendDataBuf[WM_SIZE_CHILD_SEND_BUFFER(MB_COMM_PARENT_RECV_MAX, FALSE) / sizeof(u32)] ATTRIBUTE_ALIGN(32);   // 送信データ作成用バッファ 0x400 byte
    WMBssDesc bssDescBuf ATTRIBUTE_ALIGN(32);   // 親機スキャン用バッファ 要32バイトアライン  0xC0 byte
    WMScanParam scanParam ATTRIBUTE_ALIGN(32);  // スキャンパラメータ     要32バイトアライン  0x20 byte
    MBWMWork wmWork ATTRIBUTE_ALIGN(32);        // MP通信用ワークバッファ意 要32バイトアライン
    MBUserInfo userInfo;               // ユーザー情報
    u8      _padding1[2];
    MBFakeScanCallbackFunc scanCallback;        // 親機スキャン通知用コールバック
    MBCommCStateCallbackFunc stateCallback;     // 状態通知用コールバック
    u32     ggid;                      // 検索するGGID
    MbBeaconRecvStatus beaconRecvStatus;        // ビーコン受信ステータス
    BOOL    scanning;                  // スキャン中フラグ
    BOOL    endScanBusy;               // スキャン終了処理中フラグ
    BOOL    locking;                   // ロック中フラグ
    BOOL    endFlag;                   // MBの終了処理要求
    u32     targetGgid;                // 要求するバイナリのGGID
    u16     targetFileNo;              // 要求するバイナリのFileNo
    u16     c_comm_state;              // 子機状態
    WMCallbackFunc verboseScanCallback; // 生のWMScanの通知をユーザに知らせるコールバック
    MBFakeCompareGGIDCallbackFunc compareGGIDCallback; // ユーザ定義のGGID比較コールバック
    u8      _padding2[8];
}
MBFakeWork;

SDK_COMPILER_ASSERT(sizeof(MBFakeWork) <= MB_FAKE_WORK_SIZE);


//============================================================================
// 関数プロトタイプ宣言
//============================================================================

static void MBFi_EndComplete(void);

static void MBFi_CommChangeChildState(u16 state, void *arg);
static void MBFi_SendCallback(u16 state, void *arg);
static void MBFi_ErrorCallback(u16 apiid, u16 errcode, BOOL isApiError);

/* scan用 */
static void MBFi_StateInStartScan(void);
static void MBFi_StateOutStartScanParent(void *arg);
static void MBFi_CommBeaconRecvCallback(MbBeaconMsg msg, MBGameInfoRecvList * gInfop, int index);

static void MBFi_ScanCallback(u16 state, MBGameInfoRecvList * gInfop, int index);
static void MBFi_ScanErrorCallback(u16 apiid, u16 errcode);
static void MBFi_ScanLock(u8 *macAddr);
static void MBFi_ScanUnlock(void);
static BOOL RotateChannel(void);

static void MBFi_StateInEndScan(void);
static void MBFi_StateOutEndScan(void *arg);

/* エントリー処理用 */
static void MBFi_WMCallback(u16 type, void *arg);
static void MBFi_CommChildRecvData(WMPortRecvCallback *cb);
static void MBFi_CommChildSendData(void);

/* デバッグ出力用 */
#if (MB_FAKE_PRINT == 1)
static void MBFi_PrintMBCallbackType(u16 type);
static void MBFi_PrintMBCommCallbacyType(u16 type);
#else
#define MBFi_PrintMBCallbackType(a)         ((void)0)
#define MBFi_PrintMBCommCallbackType(a)     ((void)0)
#endif

//============================================================================
// ローカル変数宣言
//============================================================================

static vu16 mbf_initialize;
static MBFakeWork *mbf_work;



//============================================================================
// 関数定義
//============================================================================


/*---------------------------------------------------------------------------*
  Name:         MB_FakeInit

  Description:  フェイクマルチブート子機の初期化関数。

  Arguments:    buf     フェイク子機に必要なワーク領域へのポインタです。
                        MB_FakeGetWorkSize()で必要なサイズを取得できます。
                user    子機のユーザ情報へのポインタです。
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MB_FakeInit(void *buf, const MBUserInfo *user)
{
    SDK_NULL_ASSERT(buf);
    SDK_ASSERT(((u32)buf & 0x1F) == 0); // 32バイトアラインをチェック

    if (mbf_initialize)
    {
        OS_TWarning("MB_FakeInit multiply called\n");
        return;
    }
    mbf_initialize = 1;

    MB_FAKE_OUTPUT("MB_Fake Initialized\n");

    // バッファのクリア
    MI_CpuClear8(buf, sizeof(MBFakeWork));

    mbf_work = (MBFakeWork *) buf;
    mbf_work->c_comm_state = MB_COMM_CSTATE_NONE;
    mbf_work->verboseScanCallback = NULL;
    mbf_work->compareGGIDCallback = NULL;

    MI_CpuCopy8(user, &mbf_work->userInfo, sizeof(MBUserInfo));
    // ビーコン受信用ワークバッファを設定
    MBi_SetBeaconRecvStatusBuffer(&mbf_work->beaconRecvStatus);
    // ゲーム情報受信ステータスを初期化
    MB_InitRecvGameInfoStatus();
    // スキャンロック用関数を設定
    MBi_SetScanLockFunc(MBFi_ScanLock, MBFi_ScanUnlock);
}


/*---------------------------------------------------------------------------*
  Name:         MB_FakeEnd

  Description:  フェイクマルチブート子機の終了関数。

  Arguments:    None.
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MB_FakeEnd(void)
{
    if (mbf_work->endFlag)
    {
        // 既に終了処理に入っている場合は何もしない
        return;
    }

    mbf_work->endFlag = TRUE;

    switch (mbf_work->c_comm_state)
    {
    case MB_COMM_CSTATE_NONE:
        // スキャン中であればスキャンを終了
        if (mbf_work->scanning)
        {
            mbf_work->scanning = FALSE;
        }
        // スキャン開始前もしくはEndScan完了後であれば何もせずに完了
        else if (!mbf_work->endScanBusy)
        {
            MBFi_EndComplete();
        }
        else
        {
            // スキャン終了処理中であれば完了を待つ
        }
        break;

    case MB_COMM_CSTATE_INIT_COMPLETE:
    case MB_COMM_CSTATE_CONNECT:
    case MB_COMM_CSTATE_CONNECT_FAILED:
    case MB_COMM_CSTATE_DISCONNECTED_BY_PARENT:
    case MB_COMM_CSTATE_REQ_REFUSED:
    case MB_COMM_CSTATE_ERROR:
    case MB_COMM_CSTATE_MEMBER_FULL:
        // リセットして終了
        MBi_WMReset();
        break;

    case MB_COMM_CSTATE_REQ_ENABLE:
    case MB_COMM_CSTATE_DLINFO_ACCEPTED:
    case MB_COMM_CSTATE_RECV_PROCEED:
    case MB_COMM_CSTATE_RECV_COMPLETE:
        // MPから順に安全に終了
        MBi_WMDisconnect();
        break;

    case MB_COMM_CSTATE_BOOTREQ_ACCEPTED:
        // 切断完了を待つ
        break;

    case MB_COMM_CSTATE_BOOT_READY:
    case MB_COMM_CSTATE_CANCELLED:
        // 既にIDLEステートに落ちているので終了
        MBFi_EndComplete();
        break;

    default:
        OS_TPanic("MB fake child is in Illegal State\n");
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBFi_EndComplete

  Description:  フェイク子機ライブラリの完了処理

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_EndComplete(void)
{
    mbf_initialize = 0;
    mbf_work->endFlag = 0;
    // ポートコールバックをクリア
    MBi_WMClearCallback();

    // ユーザに完了通知
    MBFi_CommChangeChildState(MB_COMM_CSTATE_FAKE_END, NULL);
}


/*---------------------------------------------------------------------------*
  Name:         MB_FakeGetWorkSize

  Description:  フェイク子機ライブラリに必要なワークサイズを取得します。

  Arguments:    None.

  Returns:      必要なワークバッファのサイズ
 *---------------------------------------------------------------------------*/
u32 MB_FakeGetWorkSize(void)
{
    return sizeof(MBFakeWork);
}



/*---------------------------------------------------------------------------*
  Name:         MB_FakeSetCStateCallback

  Description:  フェイク子機の状態遷移を通知するコールバック関数を設定します。

  Arguments:    callback    設定するコールバック関数

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MB_FakeSetCStateCallback(MBCommCStateCallbackFunc callback)
{
    OSIntrMode enabled = OS_DisableInterrupts();

    mbf_work->stateCallback = callback;

    (void)OS_RestoreInterrupts(enabled);
}


/*---------------------------------------------------------------------------*
  Name:         MBFi_CommChangeChildState

  Description:  子機状態の変更する関数です。
                状態の変更と同時にユーザプログラムへコールバックを返します。

  Arguments:    state   設定する状態
                arg     コールバックへの引数

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_CommChangeChildState(u16 state, void *arg)
{
#pragma unused( arg )
    OSIntrMode enabled;

    static const char *const CSTATE_NAME_STRING[] = {
        "MB_COMM_CSTATE_NONE",
        "MB_COMM_CSTATE_INIT_COMPLETE",
        "MB_COMM_CSTATE_CONNECT",
        "MB_COMM_CSTATE_CONNECT_FAILED",
        "MB_COMM_CSTATE_DISCONNECTED_BY_PARENT",
        "MB_COMM_CSTATE_REQ_ENABLE",
        "MB_COMM_CSTATE_REQ_REFUSED",
        "MB_COMM_CSTATE_DLINFO_ACCEPTED",
        "MB_COMM_CSTATE_RECV_PROCEED",
        "MB_COMM_CSTATE_RECV_COMPLETE",
        "MB_COMM_CSTATE_BOOTREQ_ACCEPTED",
        "MB_COMM_CSTATE_BOOT_READY",
        "MB_COMM_CSTATE_CANCELLED",
        "MB_COMM_CSTATE_AUTHENTICATION_FAILED",
        "MB_COMM_CSTATE_MEMBER_FULL",
        "MB_COMM_CSTATE_GAMEINFO_VALIDATED",
        "MB_COMM_CSTATE_GAMEINFO_INVALIDATED",
        "MB_COMM_CSTATE_GAMEINFO_LOST",
        "MB_COMM_CSTATE_GAMEINFO_LIST_FULL",
        "MB_COMM_CSTATE_ERROR",
        "MB_COMM_CSTATE_FAKE_END",
    };

    MB_FAKE_OUTPUT("state %s -> %s\n", CSTATE_NAME_STRING[mbf_work->c_comm_state],
                   CSTATE_NAME_STRING[state]);

    enabled = OS_DisableInterrupts();
    mbf_work->c_comm_state = state;
    (void)OS_RestoreInterrupts(enabled);
    MBFi_SendCallback(state, arg);
}


/*---------------------------------------------------------------------------*
  Name:         MBFi_SendCallback

  Description:  コールバックを送信します。

  Arguments:    state   フェイク子機の状態
                arg     コールバック引数

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void MBFi_SendCallback(u16 state, void *arg)
{
    if (mbf_work->stateCallback == NULL)
    {
        return;
    }
    mbf_work->stateCallback(state, arg);
}

/*---------------------------------------------------------------------------*
  Name:         MBFi_ErrorCallback

  Description:  エラー発生をコールバックで通知します。

  Arguments:    apiid   エラーの原因となったAPIをコールバックで通知します。
                errcode  エラーの原因コードをコールバックで通知します。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void MBFi_ErrorCallback(u16 apiid, u16 errcode, BOOL isApiError)
{
    MBErrorStatus cb;
    u16     error_type;

    if (mbf_work->stateCallback == NULL)
    {
        return;
    }

    if (isApiError)
    {
        // API関数呼び出し時のエラー
        switch (errcode)
        {
        case WM_ERRCODE_INVALID_PARAM:
        case WM_ERRCODE_FAILED:
        case WM_ERRCODE_WM_DISABLE:
        case WM_ERRCODE_NO_DATASET:
        case WM_ERRCODE_FIFO_ERROR:
        case WM_ERRCODE_TIMEOUT:
            error_type = MB_ERRCODE_FATAL;
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
            error_type = MB_ERRCODE_WM_FAILURE;
            break;
        }
    }
    else
    {
        // コールバック時のエラー
        switch (apiid)
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
            error_type = MB_ERRCODE_FATAL;
            break;
        case WM_APIID_RESET:
        case WM_APIID_END:
        default:
            /* その他のエラーについては、コールバックエラーとして返す */
            error_type = MB_ERRCODE_WM_FAILURE;
            break;
        }
    }

    MB_FAKE_OUTPUT("MBFi_ErrorCallback apiid = 0x%x, errcode = 0x%x\n", apiid, errcode);
    cb.errcode = error_type;
    mbf_work->stateCallback(MB_COMM_CSTATE_ERROR, &cb);
}


//============================================================================
// スキャン用関数
//============================================================================

void MB_FakeSetVerboseScanCallback(WMCallbackFunc callback)
{
    OSIntrMode enabled = OS_DisableInterrupts();

    mbf_work->verboseScanCallback = callback;

    (void)OS_RestoreInterrupts(enabled);
}

/*---------------------------------------------------------------------------*
  Name:         MB_FakeSetCompareGGIDCallback

  Description:  MB親機のGGID比較コールバックを設定します。

  Arguments:    callback    GGID比較コールバック関数

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MB_FakeSetCompareGGIDCallback(MBFakeCompareGGIDCallbackFunc callback)
{
    OSIntrMode enabled = OS_DisableInterrupts();

    mbf_work->compareGGIDCallback = callback;

    (void)OS_RestoreInterrupts(enabled);
}

/*---------------------------------------------------------------------------*
  Name:         MB_FakeStartScanParent

  Description:  マルチブート親機のスキャンを開始します。

  Arguments:    callback    親機スキャンのコールバックを通知する関数
                ggid        検索するMB親機のGGID。GGIDが異なれば発見されません。

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MB_FakeStartScanParent(MBFakeScanCallbackFunc callback, u32 ggid)
{
    MB_FAKE_OUTPUT("%s\n", __func__);

    mbf_work->scanCallback = callback;
    mbf_work->ggid = ggid;
    mbf_work->scanParam.channel = 0;
    mbf_work->scanParam.scanBuf = &mbf_work->bssDescBuf;
    mbf_work->scanning = TRUE;
    mbf_work->locking = FALSE;
    mbf_work->beaconRecvStatus.nowScanTargetFlag = FALSE;
    MBFi_StateInStartScan();
}

/*---------------------------------------------------------------------------*
  Name:         MBFi_StateInStartScan

  Description:  スキャンコマンドを発行します。

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_StateInStartScan(void)
{
    WMErrCode result;

    // WM_StartScan()発行
    if (mbf_work->locking)
    {
        mbf_work->scanParam.maxChannelTime = MB_SCAN_TIME_LOCKING;
    }
    else
    {
        static const u8 ANY_PARENT[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
        WM_CopyBssid(ANY_PARENT, mbf_work->scanParam.bssid);

        mbf_work->scanParam.maxChannelTime = MB_SCAN_TIME_NORMAL;
        // ロック中でなければチャンネルの設定をする
        if (!RotateChannel())
        {
            // 無線が使えない状態
            MBFi_ScanErrorCallback(WM_APIID_MEASURE_CHANNEL, 0);
            return;
        }
    }

    if (mbf_work->scanParam.channel == 0)
    {
        mbf_work->scanParam.channel = 1;
    }
    result = WM_StartScan(MBFi_StateOutStartScanParent, &mbf_work->scanParam);
    if (result != WM_ERRCODE_OPERATING)
    {
        MBFi_ScanErrorCallback(WM_APIID_START_SCAN, result);
    }
}

/*---------------------------------------------------------------------------*
  Name:         MBFi_StateOutStartScanParent

  Description:  WM_StartScanのコールバック関数

  Arguments:    arg     WM_StartScanのコールバック引数

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_StateOutStartScanParent(void *arg)
{
    WMStartScanCallback *cb = (WMStartScanCallback *)arg;

    if (WM_ERRCODE_SUCCESS != cb->errcode)
    {
        MBFi_ScanErrorCallback(WM_APIID_START_SCAN, cb->errcode);
        return;
    }

    switch (cb->state)
    {
    case WM_STATECODE_SCAN_START:
        break;

    case WM_STATECODE_PARENT_FOUND:
        // Scanで取得した親機情報の格納
        {
            /*
             * 指定されたGGIDと一致するか, または
             * ユーザ定義の比較処理がTRUEを返せば
             * ビーコン解析対象とする.
             */
            BOOL    matched = (mbf_work->compareGGIDCallback == NULL) ?
                (cb->gameInfo.ggid == mbf_work->ggid) :
                (*mbf_work->compareGGIDCallback)(cb, mbf_work->ggid);
            if (mbf_work->verboseScanCallback)
            {
                mbf_work->verboseScanCallback(arg);
            }

            if (matched)
            {
                DC_InvalidateRange(&mbf_work->bssDescBuf, WM_BSS_DESC_SIZE);
                (void)MB_RecvGameInfoBeacon(MBFi_CommBeaconRecvCallback, cb->linkLevel,
                                            &mbf_work->bssDescBuf);
                MB_CountGameInfoLifetime(MBFi_CommBeaconRecvCallback, TRUE);
            }
        }
        // don't break;

    case WM_STATECODE_PARENT_NOT_FOUND:
        /* 親機ゲーム情報の寿命カウント */
        MB_CountGameInfoLifetime(MBFi_CommBeaconRecvCallback, FALSE);

        if (mbf_work->scanning)
        {
            MBFi_StateInStartScan();
        }
        else
        {
            MBFi_StateInEndScan();
        }
        break;

    default:
        MBFi_ScanErrorCallback(WM_APIID_START_SCAN, WM_ERRCODE_FAILED);
        break;
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBFi_ScanCallback

  Description:  ユーザプログラムに対して、親機のスキャン情報をコールバックで
                通知します。

  Arguments:    state   通知する状態
                gInfop  発見した親機情報へのポインタ
                index   内部で管理している親機の管理番号

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_ScanCallback(u16 state, MBGameInfoRecvList * gInfop, int index)
{
    MBFakeScanCallback cb;

    if (mbf_work->scanCallback == NULL)
    {
        return;
    }

    cb.index = (u16)index;
    if (gInfop != NULL)
    {
        cb.gameInfo = &gInfop->gameInfo;
        cb.bssDesc = &gInfop->bssDesc;
    }
    else
    {
        cb.gameInfo = NULL;
        cb.bssDesc = NULL;
    }
    mbf_work->scanCallback(state, &cb);
}

/*---------------------------------------------------------------------------*
  Name:         MBFi_ScanErrorCallback

  Description:  スキャン時のエラー発生をコールバックで通知します。

  Arguments:    apiid    エラーの原因となったAPIID
                errcode  エラーの原因コード

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_ScanErrorCallback(u16 apiid, u16 errcode)
{
    MBFakeScanErrorCallback cb;

    if (mbf_work->scanCallback == NULL)
    {
        return;
    }

    cb.apiid = apiid;
    cb.errcode = errcode;
    mbf_work->scanCallback(MB_FAKESCAN_API_ERROR, &cb);
}

/*---------------------------------------------------------------------------*
  Name:         MBFi_ScanLock

  Description:  スキャンする親機をロック

  Arguments:    macAddr  : ロックする親機のMACアドレス

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_ScanLock(u8 *macAddr)
{
    MB_FAKE_OUTPUT("Scan Locked\n");
    mbf_work->locking = TRUE;
    WM_CopyBssid(macAddr, mbf_work->scanParam.bssid);
}

/*---------------------------------------------------------------------------*
  Name:         MBFi_ScanUnlock

  Description:  スキャンする親機のロックを解除

  Arguments:    None.
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_ScanUnlock(void)
{
    MB_FAKE_OUTPUT("Scan Unlocked\n");
    mbf_work->locking = FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         MBi_CommBeaconRecvCallback

  Description:  子機のビーコン受信コールバック

  Arguments:    msg    : ビーコン受信メッセージ
                gInfop : 親機ゲーム情報
                index  : ビーコンインデクス

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_CommBeaconRecvCallback(MbBeaconMsg msg, MBGameInfoRecvList * gInfop, int index)
{
    switch (msg)
    {
    case MB_BC_MSG_GINFO_VALIDATED:
        MBFi_ScanCallback(MB_FAKESCAN_PARENT_FOUND, (void *)gInfop, index);
        MB_FAKE_OUTPUT("Parent Info validated\n");
        break;
    case MB_BC_MSG_GINFO_INVALIDATED:
        MB_FAKE_OUTPUT("Parent Info invalidate\n");
        break;
    case MB_BC_MSG_GINFO_LOST:
        MBFi_ScanCallback(MB_FAKESCAN_PARENT_LOST, (void *)gInfop, index);
        MB_FAKE_OUTPUT("Parent Info Lost\n");
        break;
    case MB_BC_MSG_GINFO_LIST_FULL:
        MB_FAKE_OUTPUT("Parent List Full\n");
        break;
    case MB_BC_MSG_GINFO_BEACON:
        /* 既知の親機のビーコンを検出するたびに通知 */
        MBFi_ScanCallback(MB_FAKESCAN_PARENT_BEACON, (void *)gInfop, index);
        break;
    }
}

/*---------------------------------------------------------------------------*
  Name:         RotateChannel

  Description:  チャンネルのローテーション

  Arguments:    None.

  Returns:      有効なチャンネルが見つからなければFALSEを返します。
 *---------------------------------------------------------------------------*/
static BOOL RotateChannel(void)
{
    u16     allowedChannel = WM_GetAllowedChannel();
    if (allowedChannel == 0x8000 || allowedChannel == 0)
    {
        return FALSE;
    }

    // チャンネルをローテーション
    mbf_work->scanParam.channel++;
    while (TRUE)
    {
        mbf_work->scanParam.channel++;
        if (mbf_work->scanParam.channel > 16)
        {
            mbf_work->scanParam.channel = 1;
        }
        if (allowedChannel & (0x0001 << (mbf_work->scanParam.channel - 1)))
        {
            break;
        }
    }
    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         MB_FakeEndScan

  Description:  親機のスキャン終了

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MB_FakeEndScan(void)
{
    mbf_work->scanning = FALSE;
    mbf_work->endScanBusy = TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         MBFi_StateInEndScan

  Description:  親機のスキャン終了コマンドを発行

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_StateInEndScan(void)
{
    WMErrCode result;

    result = WM_EndScan(MBFi_StateOutEndScan);
    if (result != WM_ERRCODE_OPERATING)
    {
        MBFi_ScanErrorCallback(WM_APIID_END_SCAN, result);
    }
}

/*---------------------------------------------------------------------------*
  Name:         MBFi_StateOutEndScan

  Description:  親機のスキャン終了コールバック

  Arguments:    arg     WM_EndScanのコールバック引数

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_StateOutEndScan(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        MBFi_ScanErrorCallback(WM_APIID_END_SCAN, cb->errcode);
        return;
    }
    mbf_work->endScanBusy = FALSE;

    if (mbf_work->endFlag)
    {
        MBFi_EndComplete();
        return;
    }

    MBFi_ScanCallback(MB_FAKESCAN_END_SCAN, NULL, 0);

}



//============================================================================
// 親機へのエントリー処理
//============================================================================

/*---------------------------------------------------------------------------*
  Name:         MB_FakeEntryToParent

  Description:  親機へのエントリーを開始

  Arguments:    index   エントリーする親機の内部管理番号を引数として与えます。
                        MB_FakeStartScanParentのコールバックで取得するindex
                        を与えてください。

  Returns:      引数のindexが有効なものであれば TRUE.
                有効でなければ FALSE.
 *---------------------------------------------------------------------------*/
BOOL MB_FakeEntryToParent(u16 index)
{
    MBGameInfoRecvList *info;

    info = MB_GetGameInfoRecvList(index);

    if (info == NULL)
    {
        return FALSE;
    }
    mbf_work->targetGgid = info->gameInfo.ggid;
    mbf_work->targetFileNo = info->gameInfo.fileNo;

    MBi_WMSetBuffer(&mbf_work->wmWork);
    MBi_WMSetCallback(MBFi_WMCallback);
    MBFi_CommChangeChildState(MB_COMM_CSTATE_INIT_COMPLETE, NULL);
    MI_CpuCopy8(&info->bssDesc, &mbf_work->bssDescBuf, sizeof(WMBssDesc));
    MBi_WMStartConnect(&mbf_work->bssDescBuf);

    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         MBFi_WMCallback

  Description:  MP通信のコールバック関数

  Arguments:    type    コールバックタイプ
                arg     コールバック引数

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_WMCallback(u16 type, void *arg)
{
#pragma unused( arg )

    switch (type)
    {
        //--------------------------------------------
        // 親機への接続失敗通知
    case MB_CALLBACK_CONNECT_FAILED:
        MBFi_CommChangeChildState(MB_COMM_CSTATE_CONNECT_FAILED, NULL);
        break;
        //--------------------------------------------
        // 親機への接続完了通知
    case MB_CALLBACK_CONNECTED_TO_PARENT:
        MB_FAKE_OUTPUT("connect to parent\n");
        MBFi_CommChangeChildState(MB_COMM_CSTATE_CONNECT, arg);
        MBi_ChildStartMP((u16 *)mbf_work->sendBuf, (u16 *)mbf_work->recvBuf);
        break;
        //--------------------------------------------
        // 親機からの切断通知
    case MB_CALLBACK_DISCONNECTED_FROM_PARENT:
        MBFi_CommChangeChildState(MB_COMM_CSTATE_DISCONNECTED_BY_PARENT, arg);
        break;
        //--------------------------------------------
        // MP通信開始通知
    case MB_CALLBACK_MP_STARTED:
        break;
        //-----------------------------
        // データ送信可能通知
    case MB_CALLBACK_MP_SEND_ENABLE:
        {
            // 送信データのセット処理
            MBFi_CommChildSendData();
        }
        break;
        //-----------------------------
        // データ受信可能通知
    case MB_CALLBACK_MP_CHILD_RECV:
        {
            MBFi_CommChildRecvData((WMPortRecvCallback *)arg);
        }
        break;
        //-----------------------------
        // 子機側からの切断完了通知
    case MB_CALLBACK_DISCONNECT_COMPLETE:
        if (mbf_work->c_comm_state == MB_COMM_CSTATE_BOOTREQ_ACCEPTED)
        {
            // ブートリクエスト受信後ならブート完了状態へ遷移
            MBFi_CommChangeChildState(MB_COMM_CSTATE_BOOT_READY, NULL);
        }
        else
        {
            // ブートリクエスト受信前ならキャンセル状態へ遷移
            MBFi_CommChangeChildState(MB_COMM_CSTATE_CANCELLED, NULL);
        }
        if (mbf_work->endFlag)
        {
            MBFi_EndComplete();
        }
        break;
        //------------------------------
        // MP通信通知
    case MB_CALLBACK_MP_CHILD_SENT:
    case MB_CALLBACK_MP_CHILD_SENT_TIMEOUT:
    case MB_CALLBACK_MP_CHILD_SENT_ERR:
        // 単なるMPレベルの通信状況の通知なので特に処理は必要ない
        break;
        //------------------------------
        // エラーへの対処
    case MB_CALLBACK_API_ERROR:
        {
            MBErrorCallback *cb = (MBErrorCallback *) arg;

            MBFi_ErrorCallback(cb->apiid, cb->errcode, TRUE);
        }
        break;
    case MB_CALLBACK_ERROR:
        {
            MBErrorCallback *cb = (MBErrorCallback *) arg;

            MBFi_ErrorCallback(cb->apiid, cb->errcode, FALSE);
        }
        break;
    default:
        break;
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBFi_CommChildRecvData

  Description:  親機からのデータ受信処理

  Arguments:    cb  ポートコールバックのコールバック引数

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_CommChildRecvData(WMPortRecvCallback *cb)
{
    MBCommParentBlockHeader hd;

    // ヘッダ解析
    (void)MBi_SetRecvBufferFromParent(&hd, (u8 *)cb->data);

//    MBFi_PrintMBCommCallbacyType( hd.type );

    switch (hd.type)                   // 受信Blockタイプによる状態遷移
    {
    case MB_COMM_TYPE_PARENT_SENDSTART:
        // 親機からの送信開始メッセージ
        if (mbf_work->c_comm_state == MB_COMM_CSTATE_CONNECT)
        {
            MB_FAKE_OUTPUT("Allowd to request file from parent!\n");
            MBFi_CommChangeChildState(MB_COMM_CSTATE_REQ_ENABLE, NULL);
        }
        break;

    case MB_COMM_TYPE_PARENT_KICKREQ: // 親機からのKickメッセージ
        if (mbf_work->c_comm_state == MB_COMM_CSTATE_REQ_ENABLE)
        {
            MBFi_CommChangeChildState(MB_COMM_CSTATE_REQ_REFUSED, NULL);
        }
        break;

    case MB_COMM_TYPE_PARENT_MEMBER_FULL:      // 親機からのメンバー超過メッセージ
        if (mbf_work->c_comm_state == MB_COMM_CSTATE_REQ_ENABLE)
        {
            MBFi_CommChangeChildState(MB_COMM_CSTATE_MEMBER_FULL, NULL);
        }
        break;

    case MB_COMM_TYPE_PARENT_DL_FILEINFO:
        // MbDownloadFileInfoHeaderの受信
        if (mbf_work->c_comm_state == MB_COMM_CSTATE_REQ_ENABLE)
        {
            /* 受信したMbDownloadFileInfoを引数として渡す。 */
            MBFi_CommChangeChildState(MB_COMM_CSTATE_DLINFO_ACCEPTED, NULL);
        }

        break;

    case MB_COMM_TYPE_PARENT_DATA:
        /* ブロックデータの受信 */
        if (mbf_work->c_comm_state == MB_COMM_CSTATE_DLINFO_ACCEPTED)
        {
            MBFi_CommChangeChildState(MB_COMM_CSTATE_RECV_PROCEED, NULL);
        }

        if (mbf_work->c_comm_state == MB_COMM_CSTATE_RECV_PROCEED)
        {
            MBFi_CommChangeChildState(MB_COMM_CSTATE_RECV_COMPLETE, NULL);      // 受信完了
        }
        break;

    case MB_COMM_TYPE_PARENT_BOOTREQ:
        if (mbf_work->c_comm_state == MB_COMM_CSTATE_RECV_COMPLETE)
        {
            MBFi_CommChangeChildState(MB_COMM_CSTATE_BOOTREQ_ACCEPTED, NULL);
        }
        else if (mbf_work->c_comm_state == MB_COMM_CSTATE_BOOTREQ_ACCEPTED)
        {
            MBi_WMDisconnect();        // 通信を終了させる
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

static void MBFi_CommChildSendData(void)
{
    MBCommChildBlockHeader hd;
    WMErrCode errcode = WM_ERRCODE_SUCCESS;
    u16     pollbmp = 0xffff;

    switch (mbf_work->c_comm_state)
    {
    case MB_COMM_CSTATE_REQ_ENABLE:
        {
            MBCommRequestData req_data;
            u8     *databuf;

            req_data.ggid = mbf_work->targetGgid;
            req_data.version = MB_IPL_VERSION;
            req_data.fileid = (u8)mbf_work->targetFileNo;
            MI_CpuCopy8(&mbf_work->userInfo, &req_data.userinfo, sizeof(MBUserInfo));

            hd.type = MB_COMM_TYPE_CHILD_FILEREQ;
            hd.req_data.piece = MBi_SendRequestDataPiece(hd.req_data.data, &req_data);
            databuf = MBi_MakeChildSendBuffer(&hd, (u8 *)mbf_work->sendDataBuf);
            if (databuf == NULL)
            {
                return;
            }
            errcode = MBi_MPSendToParent(MB_COMM_CHILD_HEADER_SIZE, pollbmp, mbf_work->sendDataBuf);
        }
        break;

    case MB_COMM_CSTATE_DLINFO_ACCEPTED:
        // DownloadInfo受領メッセージ
        hd.type = MB_COMM_TYPE_CHILD_ACCEPT_FILEINFO;
        (void)MBi_MakeChildSendBuffer(&hd, (u8 *)mbf_work->sendDataBuf);
        errcode = MBi_MPSendToParent(MB_COMM_CHILD_HEADER_SIZE, pollbmp, mbf_work->sendDataBuf);
        break;

    case MB_COMM_CSTATE_RECV_PROCEED:
        break;

    case MB_COMM_CSTATE_RECV_COMPLETE:
        // ブロック転送停止メッセージ(親からBOOTREQが来るまで送りつづける)
        hd.type = MB_COMM_TYPE_CHILD_STOPREQ;
        (void)MBi_MakeChildSendBuffer(&hd, (u8 *)mbf_work->sendDataBuf);
        errcode = MBi_MPSendToParent(MB_COMM_CHILD_HEADER_SIZE, pollbmp, mbf_work->sendDataBuf);
        break;

    case MB_COMM_CSTATE_BOOTREQ_ACCEPTED:
        hd.type = MB_COMM_TYPE_CHILD_BOOTREQ_ACCEPTED;
        (void)MBi_MakeChildSendBuffer(&hd, (u8 *)mbf_work->sendDataBuf);
        errcode = MBi_MPSendToParent(MB_COMM_CHILD_HEADER_SIZE, pollbmp, mbf_work->sendDataBuf);
        break;
    default:
        // MP通信確立のため、DUMMY MPを送信
        hd.type = MB_COMM_TYPE_DUMMY;
        (void)MBi_MakeChildSendBuffer(&hd, (u8 *)mbf_work->sendDataBuf);
        errcode = MBi_MPSendToParent(MB_COMM_CHILD_HEADER_SIZE, pollbmp, mbf_work->sendDataBuf);
        break;

    }
}


/*---------------------------------------------------------------------------*
  Name:         MB_FakeGetParentGameInfo

  Description:  親機リストから親機のゲーム情報を取得する。

  Arguments:    index     親機インデックス
                pGameInfo 親機情報を取得する領域へのポインタ

  Returns:      有効な親機のインデックスならば TRUE
                無効な親機のインデックスならば FALSE;
 *---------------------------------------------------------------------------*/
BOOL MB_FakeGetParentGameInfo(u16 index, MBGameInfo *pGameInfo)
{
    MBGameInfoRecvList *parentInfo;
    OSIntrMode enabled;

    if (index >= MB_GAME_INFO_RECV_LIST_NUM)
    {
        return FALSE;
    }

    enabled = OS_DisableInterrupts();

    parentInfo = MB_GetGameInfoRecvList(index);
    if (parentInfo == NULL)
    {
        (void)OS_RestoreInterrupts(enabled);
        return FALSE;
    }
    MI_CpuCopy8(&parentInfo->gameInfo, pGameInfo, sizeof(MBGameInfo));
    (void)OS_RestoreInterrupts(enabled);
    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         MB_FakeGetParentBssDesc

  Description:  親機リストから親機のBssDescを取得する。

  Arguments:    index 親機インデックス
                pBssDesc 親機情報を取得する領域へのポインタ

  Returns:      有効な親機のインデックスならば TRUE
                無効な親機のインデックスならば FALSE;
 *---------------------------------------------------------------------------*/
BOOL MB_FakeGetParentBssDesc(u16 index, WMBssDesc *pBssDesc)
{
    MBGameInfoRecvList *parentInfo;
    OSIntrMode enabled;

    if (index >= MB_GAME_INFO_RECV_LIST_NUM)
    {
        return FALSE;
    }

    enabled = OS_DisableInterrupts();

    parentInfo = MB_GetGameInfoRecvList(index);
    if (parentInfo == NULL)
    {
        (void)OS_RestoreInterrupts(enabled);
        return FALSE;
    }
    MI_CpuCopy8(&parentInfo->bssDesc, pBssDesc, sizeof(WMBssDesc));
    (void)OS_RestoreInterrupts(enabled);
    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         MB_FakeReadParentBssDesc

  Description:  親機リストの親機BssDescをもとにWM_StartConnect関数のための情報を取得します。

  Arguments:    index           親機インデックス
                pBssDesc        親機情報を取得する領域へのポインタ
                parent_max_size MP親機の最大送信サイズ
                child_max_size  MP子機の最大送信サイズ
                ks_flag         キーシェアリング設定フラグ
                cs_flag         連続転送設定フラグ

  Returns:      有効な親機のインデックスならば TRUE
                無効な親機のインデックスならば FALSE;
 *---------------------------------------------------------------------------*/
BOOL MB_FakeReadParentBssDesc(u16 index, WMBssDesc *pBssDesc, u16 parent_max_size,
                              u16 child_max_size, BOOL ks_flag, BOOL cs_flag)
{
    BOOL    result;

    SDK_NULL_ASSERT(pBssDesc);

    result = MB_FakeGetParentBssDesc(index, pBssDesc);

    if (!result)
    {
        return FALSE;
    }

    pBssDesc->gameInfoLength = 0x10;
    pBssDesc->gameInfo.userGameInfoLength = 0;
    pBssDesc->gameInfo.parentMaxSize = parent_max_size;
    pBssDesc->gameInfo.childMaxSize = child_max_size;
    pBssDesc->gameInfo.attribute = (u8)((ks_flag ? WM_ATTR_FLAG_KS : 0) |
                                        (cs_flag ? WM_ATTR_FLAG_CS : 0) | WM_ATTR_FLAG_ENTRY);
    return TRUE;
}



#if (MB_FAKE_PRINT == 1)

/*---------------------------------------------------------------------------*
  Name:         MBFi_PrintMBCallbackType

  Description:  MB_WMからのコールバックタイプをデバッグ出力します。

  Arguments:    type  コールバックタイプ

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_PrintMBCallbackType(u16 type)
{
#pragma unused( type )
    static const char *const CALLBACK_NAME[] = {
        "MB_CALLBACK_CHILD_CONNECTED",
        "MB_CALLBACK_CHILD_DISCONNECTED",
        "MB_CALLBACK_MP_PARENT_SENT",
        "MB_CALLBACK_MP_PARENT_RECV",
        "MB_CALLBACK_PARENT_FOUND",
        "MB_CALLBACK_PARENT_NOT_FOUND",
        "MB_CALLBACK_CONNECTED_TO_PARENT",
        "MB_CALLBACK_DISCONNECTED",
        "MB_CALLBACK_MP_CHILD_SENT",
        "MB_CALLBACK_MP_CHILD_RECV",
        "MB_CALLBACK_DISCONNECTED_FROM_PARENT",
        "MB_CALLBACK_CONNECT_FAILED",
        "MB_CALLBACK_DCF_CHILD_SENT",
        "MB_CALLBACK_DCF_CHILD_SENT_ERR",
        "MB_CALLBACK_DCF_CHILD_RECV",
        "MB_CALLBACK_DISCONNECT_COMPLETE",
        "MB_CALLBACK_DISCONNECT_FAILED",
        "MB_CALLBACK_END_COMPLETE",
        "MB_CALLBACK_MP_CHILD_SENT_ERR",
        "MB_CALLBACK_MP_PARENT_SENT_ERR",
        "MB_CALLBACK_MP_STARTED",
        "MB_CALLBACK_INIT_COMPLETE",
        "MB_CALLBACK_END_MP_COMPLETE",
        "MB_CALLBACK_SET_GAMEINFO_COMPLETE",
        "MB_CALLBACK_SET_GAMEINFO_FAILED",
        "MB_CALLBACK_MP_SEND_ENABLE",
        "MB_CALLBACK_PARENT_STARTED",
        "MB_CALLBACK_BEACON_LOST",
        "MB_CALLBACK_BEACON_SENT",
        "MB_CALLBACK_BEACON_RECV",
        "MB_CALLBACK_MP_SEND_DISABLE",
        "MB_CALLBACK_DISASSOCIATE",
        "MB_CALLBACK_REASSOCIATE",
        "MB_CALLBACK_AUTHENTICATE",
        "MB_CALLBACK_SET_LIFETIME",
        "MB_CALLBACK_DCF_STARTED",
        "MB_CALLBACK_DCF_SENT",
        "MB_CALLBACK_DCF_SENT_ERR",
        "MB_CALLBACK_DCF_RECV",
        "MB_CALLBACK_DCF_END",
        "MB_CALLBACK_MPACK_IND",
        "MB_CALLBACK_MP_CHILD_SENT_TIMEOUT",
        "MB_CALLBACK_SEND_QUEUE_FULL_ERR",
        "MB_CALLBACK_API_ERROR",
        "MB_CALLBACK_ERROR",
    };

    MB_FAKE_OUTPUT("RecvCallback %s\n", CALLBACK_NAME[type]);
}

/*---------------------------------------------------------------------------*
  Name:         MBFi_PrintMBCommCallbacyType

  Description:  親機からのパケットのメッセージタイプをデバッグ出力します。

  Arguments:    type メッセージタイプ

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_PrintMBCommCallbacyType(u16 type)
{
#pragma unused( type )
    static const char *const MB_TYPE_STRING[] = {
        "MB_COMM_TYPE_DUMMY",          //      0
        "MB_COMM_TYPE_PARENT_SENDSTART",        //      1
        "MB_COMM_TYPE_PARENT_KICKREQ", //      2
        "MB_COMM_TYPE_PARENT_DL_FILEINFO",      //      3
        "MB_COMM_TYPE_PARENT_DATA",    //      4
        "MB_COMM_TYPE_PARENT_BOOTREQ", //      5
        "MB_COMM_TYPE_PARENT_MEMBER_FULL",      //      6
        "MB_COMM_TYPE_CHILD_FILEREQ",  //      7
        "MB_COMM_TYPE_CHILD_ACCEPT_FILEINFO",   //      8
        "MB_COMM_TYPE_CHILD_CONTINUE", //      9
        "MB_COMM_TYPE_CHILD_STOPREQ",  //      10
        "MB_COMM_TYPE_CHILD_BOOTREQ_ACCEPTED",  //      11
    };

    MB_FAKE_OUTPUT("RECV %s\n", MB_TYPE_STRING[type]);
}

#endif
