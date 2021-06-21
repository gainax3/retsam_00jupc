/*---------------------------------------------------------------------------*
  Project:  NitroWiFi - WCM - libraries
  File:     system.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: system.c,v $
  Revision 1.17  2006/03/10 09:22:19  kitase_hirotake
  INDENT SOURCE

  Revision 1.16  2005/11/08 01:08:02  seiki_masashi
  NITRO-SDK 3.0RC での変更に対応

  Revision 1.15  2005/11/01 12:08:26  seiki_masashi
  SDK_ASSERT_ON_COMPILE が定義されていた場合に対処

  Revision 1.14  2005/10/05 08:49:50  terui
  DCFデータ受信の都度、電波強度を退避する機能を追加

  Revision 1.13  2005/09/17 10:09:08  terui
  CAMライフタイムでタイムアウトする機能を追加。

  Revision 1.12  2005/09/16 04:50:10  terui
  WCM_ConnectAsync関数にて、WEPモードが暗号化無しの場合にはWEPキーデータを全て0のデータで偽装するよう対策

  Revision 1.11  2005/09/16 04:23:14  terui
  レートセットの不整合により接続に失敗した時のリトライ時に状態が一時IDLEに戻ってしまう不具合を修正

  Revision 1.10  2005/09/12 10:02:13  terui
  レートセットの不整合により接続に失敗した時の対策を追加

  Revision 1.9  2005/09/10 03:08:31  terui
  無線通信を禁止されている本体のチェックを追加。
  ユーティリティ関数を別ファイルへ切り出し。

  Revision 1.8  2005/09/01 13:08:14  terui
  アラームによってタイミングを取り、Keep Alive パケットを送信する機能を追加
  Connect 失敗時、WM から返される wlStatus フィールドを上位に伝える機能を追加

  Revision 1.7  2005/08/09 07:58:30  terui
  WEPキーを退避する内部バッファを32バイトアラインした位置に調整

  Revision 1.6  2005/08/08 06:13:42  terui
  APに接続する際のサポートレートセットに1M/2Mを固定で追加するよう改良

  Revision 1.5  2005/07/18 01:34:49  terui
  DCF 通信中に切断された場合の通知種別の設定位置を変更

  Revision 1.4  2005/07/15 11:32:10  terui
  WCM_SearchAsync 関数を開始関数と停止関数２つに分ける改造。

  Revision 1.3  2005/07/12 06:25:30  terui
  WCM_SetChannelScanTime 関数を追加

  Revision 1.2  2005/07/11 23:44:29  terui
  Fix comment.

  Revision 1.1  2005/07/07 10:45:52  terui
  新規追加

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include "wcm_private.h"
#include "wcm_cpsif.h"

/*---------------------------------------------------------------------------*
    定数定義
 *---------------------------------------------------------------------------*/

// 全て 0xff の ESSID
const u8    WCM_Essid_Any[WCM_ESSID_SIZE] =
{
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// 全て 0xff の BSSID
const u8    WCM_Bssid_Any[WCM_BSSID_SIZE] =
{
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

#if WCM_DEBUG

// 警告文用テキスト雛形
static const char   wcmWarningText_AlreadyInit[] = { "WCM library is already initialized.\n" };
static const char   wcmWarningText_IllegalParam[] = { "Illegal parameter ( %s )\n" };
static const char   wcmWarningText_NotInit[] = { "WCM library is not initialized yet.\n" };
static const char   wcmWarningText_IllegalPhase[] = { "Could not accept request now. ( PHASE: %d )\n" };
static const char   wcmWarningText_InvalidWmState[] = { "Invalid state of WM library. Don't use WM or WVR library while using WCM library.\n" };
static const char   wcmWarningText_UnsuitableArm7[] = { "Unsuitable ARM7 component. Could not drive wireless module.\n" };
static const char   wcmWarningText_WirelessForbidden[] = { "Forbidden to use wireless module." };

// 報告文用テキスト雛形
static const char   wcmReportText_WmSyncError[] = { "%s failed syncronously. ( ERRCODE: 0x%02x )\n" };
static const char   wcmReportText_WmAsyncError[] = { "%s failed asyncronously. ( ERRCODE: 0x%02x - 0x%04x - 0x%04x )\n" };
static const char   wcmReportText_WmDisconnected[] = { "%s succeeded asyncronously , but already disconnected from AP.\n" };
static const char   wcmReportText_InvalidAid[] = { "%s succeeded asyncronously , but given Auth-ID is invalid.\n" };
static const char   wcmReportText_SupportRateset[] = { "Failed association to \"%s\" because of illegal support rate set.\n  So retry with camouflage 5.5M and 11M automatically.\n" };
#endif

/*---------------------------------------------------------------------------*
    内部変数定義
 *---------------------------------------------------------------------------*/
static WCMWork*     wcmw = NULL;

/*---------------------------------------------------------------------------*
    内部関数プロトタイプ
 *---------------------------------------------------------------------------*/
static void         WcmConfigure(WCMConfig* config, WCMNotify notify);
static void         WcmEditScanExParam(void* bssid, void* essid, u32 option);
static void         WcmInitOption(void);
static u16          WcmGetNextScanChannel(u16 channel);
static void         WcmNotify(s16 result, void* para0, void* para1, s32 para2);
static void         WcmNotifyEx(s16 notify, s16 result, void* para0, void* para1, s32 para2);
static void         WcmSetPhase(u32 phase);
static void         WcmKeepAliveAlarm(void* arg);
static u32          WcmCountBits(u32 arg);
static u32          WcmCountLeadingZero(u32 arg);

static void         WcmWmReset(void);

static void         WcmWmcbIndication(void* arg);
static void         WcmWmcbCommon(void* arg);
static void         WcmWmcbScanEx(void* arg);
static void         WcmWmcbEndScan(void* arg);
static void         WcmWmcbConnect(void* arg);
static void         WcmWmcbDisconnect(void* arg);
static void         WcmWmcbStartDCF(void* arg);
static void         WcmWmcbEndDCF(void* arg);
static void         WcmWmcbReset(void* arg);

/* コンパイル時アサーション */
#ifndef SDK_ASSERT_ON_COMPILE
#define SDK_ASSERT_ON_COMPILE(expr) extern assert_on_compile ## #__LINE__ (char a[(expr) ? 1 : -1])
#endif

/*---------------------------------------------------------------------------*
  Name:         WCM_Init

  Description:  WCM ライブラリを初期化する。同期関数。

  Arguments:    buf     -   WCM ライブラリが内部的に使用するワークバッファへの
                            ポインタを指定する。初期化完了後は、WCM_Finish 関数
                            によって WCM ライブラリを終了するまで別の用途に用い
                            てはいけない。
                len     -   ワークバッファのサイズを指定する。WCM_WORK_SIZE より
                            小さいサイズを指定するとこの関数は失敗する。

  Returns:      s32     -   以下の内のいずれかの処理結果が返される。
                            WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE ,
                            WCM_RESULT_NOT_ENOUGH_MEM
 *---------------------------------------------------------------------------*/
s32 WCM_Init(void* buf, s32 len)
{
    OSIntrMode  e = OS_DisableInterrupts();

    // WCM_WORK_SIZE 定数の妥当性をコンパイル時に確認
    SDK_ASSERT_ON_COMPILE(sizeof(WCMWork) <= WCM_WORK_SIZE);

    // 未初期化を確認
    if (wcmw != NULL)
    {
        WCMi_Warning(wcmWarningText_AlreadyInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;          // 既に初期化済み
    }

    // パラメータを確認
    if (buf == NULL)
    {
        WCMi_Warning(wcmWarningText_IllegalParam, "buf");
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;          // 指定バッファが NULL
    }

    if ((u32) buf & 0x01f)
    {
        WCMi_Warning(wcmWarningText_IllegalParam, "buf");
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;          // 指定バッファが 32 バイトアラインでない
    }

    if (len < sizeof(WCMWork))
    {
        WCMi_Warning(wcmWarningText_IllegalParam, "len");
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_NOT_ENOUGH_MEM;   // 指定バッファのサイズが足りない
    }

    // 各ワーク変数を初期化
    wcmw = (WCMWork*)buf;
    wcmw->phase = WCM_PHASE_WAIT;           // 内部フェーズを初期化
    wcmw->notifyId = WCM_NOTIFY_COMMON;     // 非同期 API 通知 ID を初期化
    wcmw->maxScanTime = 0;                  // 固定スキャン時間を初期化
    wcmw->apListLock = WCM_APLIST_UNLOCK;   // AP 情報保持リストのロック状態を初期化
    wcmw->resetting = WCM_RESETTING_OFF;    // リセット重複呼び出し管理フラグを初期化
    wcmw->authId = 0;                       // Auth-ID を初期化
    wcmw->wlStatusOnConnectFail = 0x0000;
    WcmInitOption();                        // オプション設定を初期化
    WCMi_InitCpsif();                       // CPS インターフェースを初期化

    // アラームを初期化
    if (!OS_IsTickAvailable())
    {
        OS_InitTick();
    }

    if (!OS_IsAlarmAvailable())
    {
        OS_InitAlarm();
    }

    OS_CreateAlarm(&(wcmw->keepAliveAlarm));

    // 正常終了
    (void)OS_RestoreInterrupts(e);
    return WCM_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_Finish

  Description:  WCMライブラリを終了する。同期関数。WCM_Init関数によって指定した
                ワーク用バッファは解放される。

  Arguments:    None.

  Returns:      s32     -   以下の内のいずれかの処理結果が返される。
                            WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE
 *---------------------------------------------------------------------------*/
s32 WCM_Finish(void)
{
    OSIntrMode  e = OS_DisableInterrupts();

    // 初期化済みを確認
    if (wcmw == NULL)
    {
        WCMi_Warning(wcmWarningText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;  // 初期化されていない
    }

    // 内部フェーズ確認
    if (wcmw->phase != WCM_PHASE_WAIT)
    {
        WCMi_Warning(wcmWarningText_IllegalPhase, wcmw->phase);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;  // WAIT フェーズでない
    }

    // ワークバッファを解放
    wcmw = NULL;

    // 正常終了
    (void)OS_RestoreInterrupts(e);
    return WCM_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_StartupAsync

  Description:  無線機能の起動シーケンスを開始する。
                内部的には WAIT から IDLE へフェーズの移行処理が行われる。
                非同期関数であり、同期的な返り値が WCM_RESULT_ACCEPT であった
                場合には notify で指定したコールバック関数により非同期的な
                処理結果が通知される。
                非同期処理結果の通知種別は WCM_NOTIFY_STARTUP となる。

  Arguments:    config  -   WCM の動作設定となる構造体へのポインタを指定。
                notify  -   非同期的な処理結果を通知するコールバック関数を指定。
                            このコールバック関数は以後の非同期的な関数の結果
                            通知に共通して使用される。

  Returns:      s32     -   以下の内のいずれかの同期的な処理結果が返される。
                            WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE ,
                            WCM_RESULT_PROGRESS , WCM_RESULT_WMDISABLE ,
                            WCM_RESULT_FATAL_ERROR , WCM_RESULT_ACCEPT
 *---------------------------------------------------------------------------*/
s32 WCM_StartupAsync(WCMConfig* config, WCMNotify notify)
{
    OSIntrMode  e = OS_DisableInterrupts();

    // 初期化済みを確認
    if (wcmw == NULL)
    {
        WCMi_Warning(wcmWarningText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;                  // 初期化されていない
    }

    // 内部フェーズ確認
    switch (wcmw->phase)
    {
    case WCM_PHASE_WAIT:
        WcmConfigure(config, notify);
        break;

    case WCM_PHASE_WAIT_TO_IDLE:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_PROGRESS;                 // 既に同じリクエストの非同期処理中

    case WCM_PHASE_IDLE:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_SUCCESS;                  // 状態遷移目標フェーズに移行済み

    default:
        WCMi_Warning(wcmWarningText_IllegalPhase, wcmw->phase);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;                  // 要求を受け付けられないフェーズ
    }

    // 無線機能の起動シーケンスを開始
    {
        WMErrCode   wmResult;

        // WM ライブラリ初期化
        wmResult = WM_Init(wcmw->wmWork, (u16) wcmw->config.dmano);
        switch (wmResult)
        {
        case WM_ERRCODE_SUCCESS:
            break;

        case WM_ERRCODE_ILLEGAL_STATE:
            WCMi_Warning(wcmWarningText_InvalidWmState);
            WcmSetPhase(WCM_PHASE_FATAL_ERROR);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_FATAL_ERROR;          // WM ライブラリが既に初期化されている

        case WM_ERRCODE_WM_DISABLE:
            WCMi_Warning(wcmWarningText_UnsuitableArm7);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_WMDISABLE;            // ARM7 側で WM ライブラリが動作していない

        case WM_ERRCODE_INVALID_PARAM:
        default:
            WcmSetPhase(WCM_PHASE_FATAL_ERROR);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_FATAL_ERROR;          // 想定範囲外のエラー
        }

        // 無線通信を禁止されている本体でないことを確認
        if (0 == WM_GetAllowedChannel())
        {
            if (WM_ERRCODE_SUCCESS != WM_Finish())
            {
                WcmSetPhase(WCM_PHASE_FATAL_ERROR);
                (void)OS_RestoreInterrupts(e);
                return WCM_RESULT_FATAL_ERROR;      // 想定範囲外のエラー
            }

            WCMi_Warning(wcmWarningText_WirelessForbidden);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_WMDISABLE;            // 無線通信が許可されていない
        }

        // 不定期な ARM7 からの通知を受け取るベクトルを設定
        wmResult = WM_SetIndCallback(WcmWmcbIndication);
        if (wmResult != WM_ERRCODE_SUCCESS)
        {
            WcmSetPhase(WCM_PHASE_FATAL_ERROR);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_FATAL_ERROR;          // 想定範囲外のエラー
        }

        // 無線ハードウェア使用許可要求を発行
        wmResult = WM_Enable(WcmWmcbCommon);
        switch (wmResult)
        {
        case WM_ERRCODE_OPERATING:
            WcmSetPhase(WCM_PHASE_WAIT_TO_IDLE);    // 非同期的なシーケンスの開始に成功
            wcmw->notifyId = WCM_NOTIFY_STARTUP;
            break;

        case WM_ERRCODE_FIFO_ERROR:
            WCMi_Printf(wcmReportText_WmSyncError, "WM_Enable", wmResult);
            WcmSetPhase(WCM_PHASE_IRREGULAR);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_FAILURE;              // ARM7 への要求発行に失敗

        case WM_ERRCODE_ILLEGAL_STATE:
            WCMi_Warning(wcmWarningText_InvalidWmState);

        /* Don't break here */
        default:
            WcmSetPhase(WCM_PHASE_FATAL_ERROR);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_FATAL_ERROR;          // 想定範囲外のエラー
        }
    }

    // 正常終了
    (void)OS_RestoreInterrupts(e);
    return WCM_RESULT_ACCEPT;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_CleanupAsync

  Description:  無線機能の停止シーケンスを開始する。
                内部的には IDLE から WAIT へフェーズの移行処理が行われる。
                非同期関数であり、同期的な返り値が WCM_RESULT_ACCEPT であった
                場合には WCM_StartupAsync 関数で指定したコールバック関数により
                非同期的な処理結果が通知される。
                非同期処理結果の通知種別は WCM_NOTIFY_CLEANUP となる。

  Arguments:    None.

  Returns:      s32     -   以下の内のいずれかの同期的な処理結果が返される。
                            WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE ,
                            WCM_RESULT_PROGRESS , WCM_RESULT_REJECT ,
                            WCM_RESULT_FATAL_ERROR , WCM_RESULT_ACCEPT
 *---------------------------------------------------------------------------*/
s32 WCM_CleanupAsync(void)
{
    OSIntrMode  e = OS_DisableInterrupts();

    // 初期化済みを確認
    if (wcmw == NULL)
    {
        WCMi_Warning(wcmWarningText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;                  // 初期化されていない
    }

    // 内部フェーズ確認
    switch (wcmw->phase)
    {
    case WCM_PHASE_IDLE:
        break;

    case WCM_PHASE_IDLE_TO_WAIT:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_PROGRESS;                 // 既に同じリクエストの非同期処理中

    case WCM_PHASE_WAIT:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_SUCCESS;                  // 状態遷移目標フェーズに移行済み

    default:
        WCMi_Warning(wcmWarningText_IllegalPhase, wcmw->phase);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;                  // 要求を受け付けられないフェーズ
    }

    // 無線機能の停止シーケンスを開始
    {
        WMErrCode   wmResult;

        // 無線ハードウェアのシャットダウン要求を発行
        wmResult = WM_PowerOff(WcmWmcbCommon);
        switch (wmResult)
        {
        case WM_ERRCODE_OPERATING:
            WcmSetPhase(WCM_PHASE_IDLE_TO_WAIT);    // 非同期的なシーケンスの開始に成功
            wcmw->notifyId = WCM_NOTIFY_CLEANUP;
            break;

        case WM_ERRCODE_FIFO_ERROR:
            WCMi_Printf(wcmReportText_WmSyncError, "WM_PowerOff", wmResult);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_REJECT;               // ARM7 への要求発行に失敗( リトライ可能 )

        case WM_ERRCODE_ILLEGAL_STATE:
            WCMi_Warning(wcmWarningText_InvalidWmState);

        /* Don't break here */
        default:
            WcmSetPhase(WCM_PHASE_FATAL_ERROR);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_FATAL_ERROR;          // 想定範囲外のエラー
        }
    }

    // 正常終了
    (void)OS_RestoreInterrupts(e);
    return WCM_RESULT_ACCEPT;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_SearchAsync

  Description:  AP 自動探索開始シーケンス、もしくは停止シーケンスを開始する。
                パラメータによって WCM_BeginSearchAsync 関数を呼び出すか、
                WCM_EndSearchAsync 関数を呼び出すか振り分けるだけの関数。

  Arguments:    bssid   -   探索する AP の BSSID を指定する。NULL を指定した
                            場合には、探索の停止を指示したものとみなされる。
                essid   -   探索する AP の ESSID を指定する。NULL を指定した
                            場合には、探索の停止を指示したものとみなされる。
                option  -   オプション設定フォーマットに従った 32 ビットの
                            オプション変更用変数を指定する。0 を指定した場合は
                            変更は行われない。

  Returns:      s32     -   以下の内のいずれかの同期的な処理結果が返される。
                            WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE ,
                            WCM_RESULT_PROGRESS , WCM_RESULT_REJECT ,
                            WCM_RESULT_FATAL_ERROR , WCM_RESULT_ACCEPT
 *---------------------------------------------------------------------------*/
s32 WCM_SearchAsync(void* bssid, void* essid, u32 option)
{
    // パラメータを確認
    if ((bssid == NULL) || (essid == NULL))
    {
        return WCM_EndSearchAsync();
    }

    return WCM_BeginSearchAsync(bssid, essid, option);
}

/*---------------------------------------------------------------------------*
  Name:         WCM_BeginSearchAsync

  Description:  AP 自動探索開始シーケンスを開始する。既に自動探索状態であった
                場合に、探索条件の変更のみ行うことも可能。
                内部的には IDLE から SEARCH へフェーズの移行処理が行われる。
                非同期関数であり、同期的な返り値が WCM_RESULT_ACCEPT であった
                場合には WCM_StartupAsync 関数で指定したコールバック関数により
                非同期的な処理結果が通知される。
                非同期処理結果の通知種別は WCM_NOTIFY_BEGIN_SEARCH となる。

  Arguments:    bssid   -   探索する AP の BSSID を指定する。NULL もしくは
                            WCM_BSSID_ANY を指定した場合は任意の BSSID を持つ
                            AP を探索する。
                essid   -   探索する AP の ESSID を指定する。NULL もしくは
                            WCM_ESSID_ANY を指定した場合は任意の ESSID を持つ
                            AP を探索する。
                option  -   オプション設定フォーマットに従った 32 ビットの
                            オプション変更用変数を指定する。0 を指定した場合は
                            変更は行わない。

  Returns:      s32     -   以下の内のいずれかの同期的な処理結果が返される。
                            WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE ,
                            WCM_RESULT_PROGRESS , WCM_RESULT_REJECT ,
                            WCM_RESULT_FATAL_ERROR , WCM_RESULT_ACCEPT
 *---------------------------------------------------------------------------*/
s32 WCM_BeginSearchAsync(void* bssid, void* essid, u32 option)
{
    OSIntrMode  e = OS_DisableInterrupts();

    // 初期化済みを確認
    if (wcmw == NULL)
    {
        WCMi_Warning(wcmWarningText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;                  // 初期化されていない
    }

    // AP 自動探索開始シーケンスを開始、もしくは探索条件変更
    {
        WMErrCode   wmResult;

        switch (wcmw->phase)
        {
        case WCM_PHASE_IDLE:
            break;

        case WCM_PHASE_IDLE_TO_SEARCH:
            WcmEditScanExParam(bssid, essid, option);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_PROGRESS;             // 既に同じリクエストの非同期処理中

        case WCM_PHASE_SEARCH:
            WcmEditScanExParam(bssid, essid, option);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_SUCCESS;              // 状態遷移目標フェーズに移行済み

        default:
            WCMi_Warning(wcmWarningText_IllegalPhase, wcmw->phase);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_FAILURE;              // 要求を受け付けられないフェーズ
        }

        // スキャン開始要求を発行
        WcmEditScanExParam(bssid, essid, option);
        DC_InvalidateRange(wcmw->scanExParam.scanBuf, wcmw->scanExParam.scanBufSize);
        wcmw->scanCount++;
        wmResult = WM_StartScanEx(WcmWmcbScanEx, &(wcmw->scanExParam));
        switch (wmResult)
        {
        case WM_ERRCODE_OPERATING:
            WcmSetPhase(WCM_PHASE_IDLE_TO_SEARCH);  // 非同期的なシーケンスの開始に成功
            wcmw->notifyId = WCM_NOTIFY_BEGIN_SEARCH;
            break;

        case WM_ERRCODE_FIFO_ERROR:
            WCMi_Printf(wcmReportText_WmSyncError, "WM_StartScanEx", wmResult);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_REJECT;               // ARM7 への要求発行に失敗( リトライ可能 )

        case WM_ERRCODE_ILLEGAL_STATE:
            WCMi_Warning(wcmWarningText_InvalidWmState);

        /* Don't break here */
        default:
            WcmSetPhase(WCM_PHASE_FATAL_ERROR);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_FATAL_ERROR;          // 想定範囲外のエラー
        }
    }

    // 正常終了
    (void)OS_RestoreInterrupts(e);
    return WCM_RESULT_ACCEPT;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_EndSearchAsync

  Description:  AP 自動探索停止シーケンスを開始する。
                内部的には SEARCH から IDLE へフェーズの移行処理が行われる。
                非同期関数であり、動機的な返り値が WCM_RESULT_ACCEPT であった
                場合には WCM_StartupAsync 関数で指定したコールバック関数により
                非同期的な処理結果が通知される。
                非同期処理結果の通知種別は WCM_NOTIFY_END_SEARCH となる。

  Arguments:    None.

  Returns:      s32     -   以下の内のいずれかの同期的な処理結果が返される。
                            WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE ,
                            WCM_RESULT_PROGRESS , WCM_RESULT_ACCEPT
 *---------------------------------------------------------------------------*/
s32 WCM_EndSearchAsync(void)
{
    OSIntrMode  e = OS_DisableInterrupts();

    // 初期化済みを確認
    if (wcmw == NULL)
    {
        WCMi_Warning(wcmWarningText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;  // 初期化されていない
    }

    // 内部フェーズ確認
    switch (wcmw->phase)
    {
    case WCM_PHASE_SEARCH:
        WcmSetPhase(WCM_PHASE_SEARCH_TO_IDLE);
        wcmw->notifyId = WCM_NOTIFY_END_SEARCH;
        break;

    case WCM_PHASE_SEARCH_TO_IDLE:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_PROGRESS; // 既に同じリクエストの非同期処理中

    case WCM_PHASE_IDLE:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_SUCCESS;  // 状態遷移目標フェーズに移行済み

    default:
        WCMi_Warning(wcmWarningText_IllegalPhase, wcmw->phase);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;  // 要求を受け付けられないフェーズ
    }

    /* スキャン停止要求の発行は StartScanEx のコールバック内で行うので、ここでは行わない */

    // 正常終了
    (void)OS_RestoreInterrupts(e);
    return WCM_RESULT_ACCEPT;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_ConnectAsync

  Description:  AP との無線接続シーケンスを開始する。
                内部的には IDLE から DCF へフェーズの移行処理が行われる。
                非同期関数であり、同期的な返り値が WCM_RESULT_ACCEPT であった
                場合には WCM_StartupAsync 関数で指定したコールバック関数により
                非同期的な処理結果が通知される。
                非同期処理結果の通知種別は WCM_NOTIFY_CONNECT となる。

  Arguments:    bssDesc -   接続する AP の無線ネットワーク情報を指定する。
                            WCM_SearchAsync によって得られた情報がそのまま指定
                            されることを想定。
                wepDesc -   WCMWepDesc 型の WEP キーによる暗号化設定情報を指定
                            する。NULL の場合は、WEP 暗号化なしという設定になる。
                option  -   オプション設定フォーマットに従った 32 ビットの
                            オプション変更用変数を指定する。0 を指定した場合は
                            変更は行われない。

  Returns:      s32     -   以下の内のいずれかの同期的な処理結果が返される。
                            WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE ,
                            WCM_RESULT_PROGRESS , WCM_RESULT_REJECT ,
                            WCM_RESULT_FATAL_ERROR , WCM_RESULT_ACCEPT
 *---------------------------------------------------------------------------*/
s32 WCM_ConnectAsync(void* bssDesc, void* wepDesc, u32 option)
{
    OSIntrMode  e = OS_DisableInterrupts();

    // 初期化済みを確認
    if (wcmw == NULL)
    {
        WCMi_Warning(wcmWarningText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;              // 初期化されていない
    }

    // 内部フェーズ確認
    switch (wcmw->phase)
    {
    case WCM_PHASE_IDLE:
        // パラメータ確認
        if (bssDesc == NULL)
        {
            WCMi_Warning(wcmWarningText_IllegalParam, "bssDesc");
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_FAILURE;          // bssDesc 引数が NULL
        }

        if (((WMBssDesc*)bssDesc)->gameInfoLength > 0)
        {
            WCMi_Warning(wcmWarningText_IllegalParam, "bssDesc");
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_FAILURE;          // bssDesc 引数が AP 情報ではなく DS 親機
        }

        if (wepDesc != NULL)
        {
            if ((((WCMWepDesc*)wepDesc)->mode >= 4) || (((WCMWepDesc*)wepDesc)->keyId >= 4))
            {
                WCMi_Warning(wcmWarningText_IllegalParam, "wepDesc");
                (void)OS_RestoreInterrupts(e);
                return WCM_RESULT_FAILURE;      // wepDesc 引数が想定外の内容
            }

            wcmw->wepDesc.mode = ((WCMWepDesc*)wepDesc)->mode;
            wcmw->wepDesc.keyId = ((WCMWepDesc*)wepDesc)->keyId;
            if (wcmw->wepDesc.mode == WCM_WEPMODE_NONE)
            {
                MI_CpuClear8(wcmw->wepDesc.key, WM_SIZE_WEPKEY);
            }
            else
            {
                MI_CpuCopy8(((WCMWepDesc*)wepDesc)->key, wcmw->wepDesc.key, WM_SIZE_WEPKEY);
            }
        }
        else
        {
            MI_CpuClear8(&(wcmw->wepDesc), sizeof(WCMWepDesc));
        }

        MI_CpuCopy8(bssDesc, &(wcmw->bssDesc), sizeof(WMBssDesc));
#if 0
        /* debug */
        {
            int     i;
            u8*     pDump = (u8 *) (&(wcmw->bssDesc));

            for (i = 0; i < 192; i++)
            {
                if (i % 16)
                {
                    OS_TPrintf(" ");
                }

                OS_TPrintf("%02X", pDump[i]);
                if ((i % 16) == 15)
                {
                    OS_TPrintf("\n");
                }
            }
        }
#endif

        // サポートするデータ転送レートを NITRO 用に改変
        wcmw->bssDesc.rateSet.support = (u16) (WCM_ADDITIONAL_RATESET | wcmw->bssDesc.rateSet.basic);
        (void)WCM_UpdateOption(option);
        break;

    case WCM_PHASE_IDLE_TO_DCF:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_PROGRESS;             // 既に同じリクエストの非同期処理中

    case WCM_PHASE_DCF:
        (void)OS_RestoreInterrupts(e);          // 状態遷移目標フェーズに移行済み
        return WCM_RESULT_SUCCESS;

    default:
        WCMi_Warning(wcmWarningText_IllegalPhase, wcmw->phase);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;              // 要求を受け付けられないフェーズ
    }

    // AP との無線接続シーケンスを開始
    {
        WMErrCode   wmResult;

        // 無線ファームウェアのタイムアウト設定要求を発行
        wmResult = WM_SetLifeTime(WcmWmcbCommon, 0xffff, WCM_CAM_LIFETIME, 0xffff, 0xffff);
        switch (wmResult)
        {
        case WM_ERRCODE_OPERATING:
            WcmSetPhase(WCM_PHASE_IDLE_TO_DCF); // 非同期的なシーケンスの開始に成功
            wcmw->notifyId = WCM_NOTIFY_CONNECT;
            break;

        case WM_ERRCODE_FIFO_ERROR:
            WCMi_Printf(wcmReportText_WmSyncError, "WM_SetLifeTime", wmResult);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_REJECT;           // ARM7 への要求発行に失敗( リトライ可能 )

        case WM_ERRCODE_ILLEGAL_STATE:
            WCMi_Warning(wcmWarningText_InvalidWmState);

        /* Don't break here */
        default:
            WcmSetPhase(WCM_PHASE_FATAL_ERROR);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_FATAL_ERROR;      // 想定範囲外のエラー
        }
    }

    // 正常終了
    (void)OS_RestoreInterrupts(e);
    return WCM_RESULT_ACCEPT;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_DisconnectAsync

  Description:  AP との無線接続を切断するシーケンスを開始する。
                内部的には DCF から IDLE へフェーズの移行処理が行われる。
                非同期関数であり、同期的な返り値が WCM_RESULT_ACCEPT であった
                場合には WCM_StartupAsync 関数で指定したコールバック関数により
                非同期的な処理結果が通知される。
                非同期処理結果の通知種別は WCM_NOTIFY_DISCONNECT となる。

  Arguments:    None.

  Returns:      s32     -   以下の内のいずれかの同期的な処理結果が返される。
                            WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE ,
                            WCM_RESULT_PROGRESS , WCM_RESULT_REJECT ,
                            WCM_RESULT_FATAL_ERROR , WCM_RESULT_ACCEPT
 *---------------------------------------------------------------------------*/
s32 WCM_DisconnectAsync(void)
{
    OSIntrMode  e = OS_DisableInterrupts();

    // 初期化済みを確認
    if (wcmw == NULL)
    {
        WCMi_Warning(wcmWarningText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;              // 初期化されていない
    }

    // 内部フェーズ確認
    switch (wcmw->phase)
    {
    case WCM_PHASE_DCF:
        break;

    case WCM_PHASE_DCF_TO_IDLE:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_PROGRESS;             // 既に同じリクエストの非同期処理中

    case WCM_PHASE_IDLE:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_SUCCESS;              // 状態遷移目標フェーズに移行済み

    default:
        WCMi_Warning(wcmWarningText_IllegalPhase, wcmw->phase);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;              // 要求を受け付けられないフェーズ
    }

    // AP との無線接続の切断シーケンスを開始
    if (wcmw->resetting == WCM_RESETTING_ON)
    {
        /* 切断通知を受けてリセット中なので、このリセットをもって切断要求が実行されたことにする */
        WcmSetPhase(WCM_PHASE_DCF_TO_IDLE);
        wcmw->notifyId = WCM_NOTIFY_DISCONNECT;
    }
    else
    {
        WMErrCode   wmResult;

        // DCF 通信モード終了要求を発行
        wmResult = WM_EndDCF(WcmWmcbEndDCF);
        switch (wmResult)
        {
        case WM_ERRCODE_OPERATING:
            WcmSetPhase(WCM_PHASE_DCF_TO_IDLE); // 非同期的なシーケンスの開始に成功
            wcmw->notifyId = WCM_NOTIFY_DISCONNECT;
            break;

        case WM_ERRCODE_FIFO_ERROR:
            WCMi_Printf(wcmReportText_WmSyncError, "WM_EndDCF", wmResult);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_REJECT;           // ARM7 への要求発行に失敗( リトライ可能 )

        case WM_ERRCODE_ILLEGAL_STATE:
            WCMi_Warning(wcmWarningText_InvalidWmState);

        /* Don't break here */
        default:
            WcmSetPhase(WCM_PHASE_FATAL_ERROR);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_FATAL_ERROR;      // 想定範囲外のエラー
        }
    }

    // 正常終了
    (void)OS_RestoreInterrupts(e);
    return WCM_RESULT_ACCEPT;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_TerminateAsync

  Description:  無線機能の強制終了シーケンスを開始する。
                別の非同期処理が実行されていない限りはどのフェーズからでも実行
                可能であり、内部的には WAIT へとフェーズの移行処理が行われる。
                非同期関数であり、同期的な返り値が WCM_RESULT_ACCEPT であった
                場合には WCM_SartupAsync 関数で指定したコールバック関数により
                非同期的な処理結果が通知される。
                非同期処理結果の通知種別は WCM_NOTIFY_TERMINATE となる。

  Arguments:    None.

  Returns:      s32     -   以下の内のいずれかの同期的な処理結果が返される。
                            WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE ,
                            WCM_RESULT_PROGRESS , WCM_RESULT_REJECT ,
                            WCM_RESULT_FATAL_ERROR , WCM_RESULT_ACCEPT
 *---------------------------------------------------------------------------*/
s32 WCM_TerminateAsync(void)
{
    OSIntrMode  e = OS_DisableInterrupts();

    // 初期化済みを確認
    if (wcmw == NULL)
    {
        WCMi_Warning(wcmWarningText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;              // 初期化されていない
    }

    // 内部フェーズ確認
    switch (wcmw->phase)
    {
    case WCM_PHASE_IDLE:
    case WCM_PHASE_DCF:
    case WCM_PHASE_IRREGULAR:
        break;

    case WCM_PHASE_TERMINATING:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_PROGRESS;             // 既に同じリクエストの非同期処理中

    case WCM_PHASE_WAIT:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_SUCCESS;              // 状態遷移目標フェーズに移行済み

    case WCM_PHASE_SEARCH:
        WcmSetPhase(WCM_PHASE_TERMINATING);
        wcmw->notifyId = WCM_NOTIFY_TERMINATE;
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_ACCEPT;               // 非同期処理要求を受諾

    /* リセット要求の発行は StartScanEx のコールバック内で行うので、ここでは行わない */
    case WCM_PHASE_WAIT_TO_IDLE:
    case WCM_PHASE_IDLE_TO_WAIT:
    case WCM_PHASE_IDLE_TO_SEARCH:
    case WCM_PHASE_SEARCH_TO_IDLE:
    case WCM_PHASE_IDLE_TO_DCF:
    case WCM_PHASE_DCF_TO_IDLE:
    case WCM_PHASE_FATAL_ERROR:
    default:
        WCMi_Warning(wcmWarningText_IllegalPhase, wcmw->phase);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;              // 要求を受け付けられないフェーズ
    }

    // 無線機能の強制終了シーケンスを開始する
    if (wcmw->resetting == WCM_RESETTING_ON)
    {
        /* DCF 通信中に切断されてリセット中なので、このリセットをもって強制終了要求が実行されたことにする */
        WcmSetPhase(WCM_PHASE_TERMINATING);
        wcmw->notifyId = WCM_NOTIFY_TERMINATE;
    }
    else
    {
        WMStatus*   ws;
        u16         wmState;
        WMErrCode   wmResult;

        // WM ライブラリの内部状態を調査
        ws = (WMStatus*)WMi_GetStatusAddress();
        DC_InvalidateRange(ws, 2);
        wmState = ws->state;

        // WM 内部状態により停止状態に遷移するよう連鎖を開始
        switch (wmState)
        {
        case WM_STATE_READY:
            // WM ライブラリ終了
            wmResult = WM_Finish();
            if (wmResult == WM_ERRCODE_SUCCESS)
            {
                WcmSetPhase(WCM_PHASE_WAIT);
                wcmw->notifyId = WCM_NOTIFY_COMMON;
                (void)OS_RestoreInterrupts(e);
                return WCM_RESULT_SUCCESS;      // 同期的に WAIT フェーズへの移行が完了
            }
            break;

        case WM_STATE_STOP:
            // 無線ハードウェアの使用禁止要求を発行
            wmResult = WM_Disable(WcmWmcbCommon);
            break;

        case WM_STATE_IDLE:
            // 無線ハードウェアのシャットダウン要求を発行
            wmResult = WM_PowerOff(WcmWmcbCommon);
            break;

        default:
            // リセット重複呼び出し管理フラグをセット
            wcmw->resetting = WCM_RESETTING_ON;

            // 無線接続状態のリセット要求を発行
            wmResult = WM_Reset(WcmWmcbReset);
        }

        // 要求発行に対する同期的な処理結果を確認
        switch (wmResult)
        {
        case WM_ERRCODE_OPERATING:
            WcmSetPhase(WCM_PHASE_TERMINATING); // 非同期的なシーケンスの開始に成功
            wcmw->notifyId = WCM_NOTIFY_TERMINATE;
            break;

        case WM_ERRCODE_FIFO_ERROR:
            WCMi_Printf(wcmReportText_WmSyncError, "WM_***", wmResult);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_REJECT;           // ARM7 への要求発行に失敗( リトライ可能 )

        case WM_ERRCODE_ILLEGAL_STATE:
            WCMi_Warning(wcmWarningText_InvalidWmState);

        /* Don't break here */
        default:
            WcmSetPhase(WCM_PHASE_FATAL_ERROR);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_FATAL_ERROR;      // 想定範囲外のエラー
        }
    }

    // 正常終了
    (void)OS_RestoreInterrupts(e);
    return WCM_RESULT_ACCEPT;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_GetPhase

  Description:  WCM ライブラリの内部状態 ( フェーズ ) を取得する。同期関数。

  Arguments:    None.

  Returns:      s32     -   現在の WCM ライブラリのフェーズを返す。
 *---------------------------------------------------------------------------*/
s32 WCM_GetPhase(void)
{
    OSIntrMode  e = OS_DisableInterrupts();
    s32         phase = WCM_PHASE_NULL;

    // 初期化済みを確認
    if (wcmw != NULL)
    {
        phase = (s32) (wcmw->phase);
    }

    (void)OS_RestoreInterrupts(e);
    return phase;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_UpdateOption

  Description:  WCM ライブラリのオプション設定を更新する。

  Arguments:    option  -   オプション設定フォーマットに従った 32 ビットの
                            オプション変更用変数を指定する。
                            0 を指定した場合は何も更新を行わない動作になる。

  Returns:      u32     -   変更を行う前のオプション変数を返す。
 *---------------------------------------------------------------------------*/
u32 WCM_UpdateOption(u32 option)
{
    OSIntrMode  e = OS_DisableInterrupts();
    u32         filter = 0;
    u32         old_option = wcmw->option;

    // 初期化確認
    if (wcmw == NULL)
    {
        (void)OS_RestoreInterrupts(e);
        return 0;
    }

    // 更新すべきオプションカテゴリを抽出し、一旦クリアすべきビットを編集する
    if (option & WCM_OPTION_TEST_CHANNEL)
    {
        filter |= WCM_OPTION_FILTER_CHANNEL;
        if (!(option & WCM_OPTION_FILTER_CHANNEL))
        {
            option |= WCM_OPTION_CHANNEL_RDC;
        }
    }

    if (option & WCM_OPTION_TEST_POWER)
    {
        filter |= WCM_OPTION_FILTER_POWER;
    }

    if (option & WCM_OPTION_TEST_AUTH)
    {
        filter |= WCM_OPTION_FILTER_AUTH;
    }

    if (option & WCM_OPTION_TEST_SCANTYPE)
    {
        filter |= WCM_OPTION_FILTER_SCANTYPE;
    }

    if (option & WCM_OPTION_TEST_ROUNDSCAN)
    {
        filter |= WCM_OPTION_FILTER_ROUNDSCAN;
    }

    // オプション変数の各ビットを更新
    wcmw->option = (u32) ((old_option &~filter) | option);

    (void)OS_RestoreInterrupts(e);
    return old_option;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_SetChannelScanTime

  Description:  AP の自動探索時に１つのチャンネルをスキャンする時間を設定する。

  Arguments:    msec    -   １チャンネルをスキャンする時間を ms 単位で指定。
                            10 ～ 1000 までの間で設定可能だが、この範囲外の値を
                            指定すると自動探索はデフォルトの設定時間で探索を行う
                            ようになる。

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WCM_SetChannelScanTime(u16 msec)
{
    // 初期化確認
    if (wcmw != NULL)
    {
        if ((msec >= 10) && (msec <= 1000))
        {
            wcmw->maxScanTime = msec;
        }
        else
        {
            wcmw->maxScanTime = 0;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_GetSystemWork

  Description:  WCM ライブラリが内部で使用しているワークバッファへのポインタを
                取得するための内部関数。

  Arguments:    None.

  Returns:      WCMWork*    -   ワークバッファへのポインタを返す。初期化前など
                                バッファが存在しない場合は NULL になる。
 *---------------------------------------------------------------------------*/
WCMWork* WCMi_GetSystemWork(void)
{
    return wcmw;
}

/*---------------------------------------------------------------------------*
  Name:         WcmConfigure

  Description:  WCM の内部動作設定を編集する。

  Arguments:    config  -   WCM の動作設定構造体へのポインタを指定。
                notify  -   非同期的な処理結果を通知するコールバック関数を指定。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmConfigure(WCMConfig* config, WCMNotify notify)
{
    if (config == NULL)
    {
        // 設定が示されない場合、デフォルト値を使用する
        wcmw->config.dmano = 3;
        wcmw->config.pbdbuffer = NULL;
        wcmw->config.nbdbuffer = 0;
        wcmw->config.nbdmode = 0;
    }
    else
    {
        // DMA 番号を退避
        if (config->dmano &~(0x03))
        {
            // DMA 番号は 0 ～ 3 でないといけない
            WCMi_Warning(wcmWarningText_IllegalParam, "config->dmano");
        }

        wcmw->config.dmano = (config->dmano & 0x03);

        // AP 情報保持リスト領域の設定を退避
        if ((((4 - ((u32) (config->pbdbuffer) & 0x03)) % 4) + sizeof(WCMApListHeader)) > config->nbdbuffer)
        {
            // リスト管理用ヘッダ領域すら確保できないサイズなので、バッファなしの場合と同等
            wcmw->config.pbdbuffer = NULL;
            wcmw->config.nbdbuffer = 0;
        }
        else
        {
            /*
             * 与えられたバッファは 4 バイトアラインされていない可能性もあるため、
             * 4 バイトアラインされた位置にずらし、サイズをその分目減りさせる。
             */
            wcmw->config.pbdbuffer = (void*)WCM_ROUNDUP4((u32) (config->pbdbuffer));
            wcmw->config.nbdbuffer = config->nbdbuffer - (s32) ((4 - ((u32) (config->pbdbuffer) & 0x03)) % 4);
            MI_CpuClear8(wcmw->config.pbdbuffer, (u32) (wcmw->config.nbdbuffer));
        }

        wcmw->config.nbdmode = config->nbdmode;
    }

    // 通知ベクトル退避
    wcmw->notify = notify;
}

/*---------------------------------------------------------------------------*
  Name:         WcmEditScanExParam

  Description:  WM ライブラリにスキャンを指示する際のスキャン設定構造体を編集する。

  Arguments:    bssid   -   探索時の BSSID フィルター。WCM_BSSID_ANY は全て 0xff
                            である BSSID なので、フィルターしない設定となる。
                essid   -   探索時の ESSID フィルター。WCM_ESSID_ANY を指定すると
                            長さ 0 の ESSID とみなされ、フィルターしない設定となる。
                option  -   オプション変更用変数。変更を行うと WCM ライブラリ内で
                            共通使用される option が変わって、元には戻らない。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmEditScanExParam(void* bssid, void* essid, u32 option)
{
    (void)WCM_UpdateOption(option);
    wcmw->scanExParam.scanBuf = (WMBssDesc*)wcmw->recvBuf;
    wcmw->scanExParam.scanBufSize = WM_SIZE_SCAN_EX_BUF;
    wcmw->scanExParam.channelList = (u16) ((0x0001 << WcmGetNextScanChannel(0)) >> 1);
    wcmw->scanExParam.maxChannelTime = ((wcmw->maxScanTime != 0) ? wcmw->maxScanTime : WM_GetDispersionScanPeriod());
    wcmw->scanExParam.scanType = (u16) (((wcmw->option & WCM_OPTION_MASK_SCANTYPE) == WCM_OPTION_SCANTYPE_ACTIVE) ? WM_SCANTYPE_ACTIVE : WM_SCANTYPE_PASSIVE);
    if (bssid == NULL)
    {
        MI_CpuCopy8(WCM_Bssid_Any, wcmw->scanExParam.bssid, WCM_BSSID_SIZE);
    }
    else
    {
        MI_CpuCopy8(bssid, wcmw->scanExParam.bssid, WCM_BSSID_SIZE);
    }

    //MI_CpuCopy8( bssid , wcmw->scanExParam.bssid , WCM_BSSID_SIZE );
    if ((essid == NULL) || (essid == (void*)WCM_Essid_Any))
    {
        MI_CpuCopy8(WCM_Essid_Any, wcmw->scanExParam.ssid, WCM_ESSID_SIZE);
        wcmw->scanExParam.ssidLength = 0;
    }
    else
    {
        s32 len;

        MI_CpuCopy8(essid, wcmw->scanExParam.ssid, WCM_ESSID_SIZE);
        for (len = 0; len < WCM_ESSID_SIZE; len++)
        {
            if (((u8*)essid)[len] == '\0')
            {
                break;
            }
        }

        wcmw->scanExParam.ssidLength = (u16) len;
    }

    wcmw->scanCount = 0;
}

/*---------------------------------------------------------------------------*
  Name:         WcmInitOption

  Description:  WCM ライブラリのオプション設定値を初期化する。

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmInitOption(void)
{
    /*
     * IW と I/O を合わせる為に下記のようなデフォルト設定になっているが、
     * CHANNEL は ALL 、SCANTYPE は ACTIVE であったほうが妥当と思われる。
     */
    wcmw->option = WCM_OPTION_CHANNEL_RDC |
        WCM_OPTION_POWER_SAVE |
        WCM_OPTION_AUTH_OPENSYSTEM |
        WCM_OPTION_SCANTYPE_PASSIVE |
        WCM_OPTION_ROUNDSCAN_IGNORE;
}

/*---------------------------------------------------------------------------*
  Name:         WcmGetNextScanChannel

  Description:  オプションに設定されている探索時のチャンネルリストから、次に
                スキャンすべきチャンネルを決定する。

  Arguments:    channel -   今回スキャンしたチャンネルを渡す。

  Returns:      u16     -   次回スキャンすべきチャンネルを返す。
 *---------------------------------------------------------------------------*/
static u16 WcmGetNextScanChannel(u16 channel)
{
    s32 i;

    for (i = 0; i < 13; i++)
    {
        if (wcmw->option & (0x0001 << (((channel + i) % 13) + 1)))
        {
            break;
        }
    }

    return(u16) (((channel + i) % 13) + 1);
}

/*---------------------------------------------------------------------------*
  Name:         WcmNotify

  Description:  非同期的な処理の処理結果をコールバックする。
                不定期に発生する通知もこの関数を通ってコールバックされる。
                通知種別については WCM 内部共通変数から自動的に設定され、かつ
                重複した通知を避けるために、通知する度にクリアされる。

  Arguments:    result  -   処理結果を指定する。
                para0   -   通知関数に渡すパラメータ[ 0 ]
                para1   -   通知関数に渡すパラメータ[ 1 ]
                para2   -   通知関数に渡すパラメータ[ 2 ]

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmNotify(s16 result, void* para0, void* para1, s32 para2)
{
    s16 notifyId = wcmw->notifyId;

    wcmw->notifyId = WCM_NOTIFY_COMMON;
    WcmNotifyEx(notifyId, result, para0, para1, para2);
}

/*---------------------------------------------------------------------------*
  Name:         WcmNotifyEx

  Description:  アプリケーションへのコールバックによる通知を行う。

  Arguments:    result  -   処理結果を指定する。
                para0   -   通知関数に渡すパラメータ[ 0 ]
                para1   -   通知関数に渡すパラメータ[ 1 ]
                para2   -   通知関数に渡すパラメータ[ 2 ]

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmNotifyEx(s16 notify, s16 result, void* para0, void* para1, s32 para2)
{
    if (wcmw->notify)
    {
        WCMNotice   notice;

        notice.notify = notify;
        notice.result = result;
        notice.parameter[0].p = para0;
        notice.parameter[1].p = para1;
        notice.parameter[2].n = para2;
        wcmw->notify(&notice);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmSetPhase

  Description:  WCM ライブラリの内部状態 ( フェーズ ) を変更する。
                FATAL_ERROR 状態から別のフェーズに変更はできない。
                また、DCF 通信状態へのフェーズ変更及び DCF 通信状態からの
                フェーズ変更を監視し、キープアライブパケット送信用アラームの
                動作を制御する。

  Arguments:    phase   -   変更するフェーズを指定。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmSetPhase(u32 phase)
{
    OSIntrMode  e = OS_DisableInterrupts();

    // DCF 通信状態から別の状態に移行する際にアラームを停止
    if ((wcmw->phase == WCM_PHASE_DCF) && (phase != WCM_PHASE_DCF))
    {
        // Keep Alive 用アラーム停止
        OS_CancelAlarm(&(wcmw->keepAliveAlarm));
    }

    // FATAL ERROR 状態でない場合は指定状態に変更
    if (wcmw->phase != WCM_PHASE_FATAL_ERROR)
    {
        wcmw->phase = phase;
    }

    // DCF 通信状態に移行する際にアラームを開始
    if (phase == WCM_PHASE_DCF)
    {
        // Keep Alive 用アラーム開始
        OS_SetAlarm(&(wcmw->keepAliveAlarm), OS_SecondsToTicks(WCM_KEEP_ALIVE_SPAN), WcmKeepAliveAlarm, NULL);
    }
    (void)OS_RestoreInterrupts(e);
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_ResetKeepAliveAlarm

  Description:  キープアライブ NULL パケット送信用アラームをリセットし、規定
                時間のアラームを設定し直す。

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WCMi_ResetKeepAliveAlarm(void)
{
    OSIntrMode  e = OS_DisableInterrupts();

    // 改めてアラームをセット
    OS_CancelAlarm(&(wcmw->keepAliveAlarm));
    if (wcmw->phase == WCM_PHASE_DCF)
    {
        OS_SetAlarm(&(wcmw->keepAliveAlarm), OS_SecondsToTicks(WCM_KEEP_ALIVE_SPAN), WcmKeepAliveAlarm, NULL);
    }
    (void)OS_RestoreInterrupts(e);
}

/*---------------------------------------------------------------------------*
  Name:         WcmKeepAliveAlarm

  Description:  キープアライブ NULL パケット送信用アラームのアラームハンドラ。

  Arguments:    arg     -   未使用。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmKeepAliveAlarm(void* arg)
{
#pragma unused(arg)

    WCMi_CpsifSendNullPacket();
    WCMi_ResetKeepAliveAlarm();
}

#include <nitro/code32.h>

/*---------------------------------------------------------------------------*
  Name:         WcmCountBits

  Description:  u32 型の値に含まれる 1 であるビットの数を取得する。

  Arguments:    arg     -   調査する u32 型の値。

  Returns:      u32     -   引数に含まれる 1 ビットの数を返す。0 ～ 32 の値。
 *---------------------------------------------------------------------------*/

static asm u32
WcmCountBits( u32 arg )
{
    mov     r1, r0
    mov     r0, #0
    mov     r3, #1
@loop:
    clz     r2, r1
    rsbs    r2, r2, #31
    bxcc    lr
    bic     r1, r1, r3, LSL r2
    add     r0, r0, #1
    b       @loop
}

/*---------------------------------------------------------------------------*
  Name:         WcmCountLeadingZero

  Description:  u32 型の値の先行ゼロカウント(最上位ビットから 0 であるビットが
                いくつ連続しているか)を取得する。

  Arguments:    arg     -   調査する u32 型の値。

  Returns:      u32     -   先行ゼロカウントを返す。0 ～ 32 の値。
 *---------------------------------------------------------------------------*/

static asm u32
WcmCountLeadingZero( u32 arg )
{
    clz     r0, r0
    bx      lr
}
#include <nitro/codereset.h>

/*---------------------------------------------------------------------------*
  Name:         WcmWmReset

  Description:  WM_Reset 関数により WM ライブラリのリセット処理を開始する。

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmWmReset(void)
{
    WMErrCode   wmResult;

    if (wcmw->resetting == WCM_RESETTING_OFF)
    {
        // リセット重複呼び出し管理フラグをセット
        wcmw->resetting = WCM_RESETTING_ON;

        // 無線接続状態のリセット要求を発行
        wmResult = WM_Reset(WcmWmcbReset);
        if (wmResult != WM_ERRCODE_OPERATING)
        {
            /* リセットに失敗した場合は復旧不可能 */
            WCMi_Printf(wcmReportText_WmSyncError, "WM_Reset", wmResult);
            WcmSetPhase(WCM_PHASE_FATAL_ERROR);
            WcmNotify(WCM_RESULT_FATAL_ERROR, 0, 0, __LINE__);
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmWmcbIndication

  Description:  WM ライブラリからの不定期通知を受け取るハンドラ。

  Arguments:    arg     -   WM ライブラリから渡される通知パラメータへのポインタ。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmWmcbIndication(void* arg)
{
    WMIndCallback*  cb = (WMIndCallback*)arg;

    /* WCM 内部処理に関係しない通知は無視する */
    if (cb->errcode == WM_ERRCODE_FIFO_ERROR)
    {
        if ((cb->state == WM_STATECODE_FIFO_ERROR) && (cb->reason == WM_APIID_AUTO_DISCONNECT))
        {
            /*
             * 通信の整合性が保てなくなる致命的なハードウェアエラーが ARM7 側で検知され、
             * 自動的に切断しようとしたがリクエストキューが詰まっていて切断要求を予約できなかった場合
             */
            switch (wcmw->phase)
            {
            case WCM_PHASE_IDLE_TO_DCF:
                WcmSetPhase(WCM_PHASE_IRREGULAR);
                break;

            case WCM_PHASE_DCF:
            case WCM_PHASE_IRREGULAR:
                WcmWmReset();
                break;

            case WCM_PHASE_DCF_TO_IDLE:
                WcmSetPhase(WCM_PHASE_IRREGULAR);
                break;

                /* 想定外のフェーズでの通知は無視する */
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmWmcbCommon

  Description:  WM ライブラリからの通知を受け取るハンドラ。
                WM_Enable , WM_Disable , WM_PowerOn , WM_PowerOff ,
                WM_SetLifeTime , WM_SetBeaconIndication , WM_SetWEPKeyEx
                に対する結果応答を受け取る。

  Arguments:    arg     -   WM ライブラリから渡される通知パラメータへのポインタ。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmWmcbCommon(void* arg)
{
    WMCallback*     cb = (WMCallback*)arg;
    WMErrCode       wmResult = WM_ERRCODE_MAX;

    switch (cb->errcode)
    {
    case WM_ERRCODE_SUCCESS:
        // 次のステップとして行うべき処理を決定
        switch (cb->apiid)
        {
        case WM_APIID_ENABLE:
            // 無線ハードウェアへの電力供給要求を発行
            wmResult = WM_PowerOn(WcmWmcbCommon);
            break;

        case WM_APIID_DISABLE:
            // WM ライブラリ終了
            wmResult = WM_Finish();
            switch (wmResult)
            {
            case WM_ERRCODE_SUCCESS:
                WcmSetPhase(WCM_PHASE_WAIT);        // 非同期シーケンス正常終了
                WcmNotify(WCM_RESULT_SUCCESS, 0, 0, __LINE__);
                break;

            case WM_ERRCODE_WM_DISABLE:
                WCMi_Warning(wcmWarningText_InvalidWmState);

            /* Don't break here */
            default:
                WcmSetPhase(WCM_PHASE_FATAL_ERROR); // 想定範囲外のエラー
                WcmNotify(WCM_RESULT_FATAL_ERROR, 0, 0, __LINE__);
            }

            return; // 次のステップはないのでここで終了

        case WM_APIID_POWER_ON:
            WcmSetPhase(WCM_PHASE_IDLE);    // 非同期シーケンス正常終了
            WcmNotify(WCM_RESULT_SUCCESS, 0, 0, __LINE__);
            return; // 次のステップはないのでここで終了

        case WM_APIID_POWER_OFF:
            // 無線ハードウェアの使用禁止要求を発行
            wmResult = WM_Disable(WcmWmcbCommon);
            break;

        case WM_APIID_SET_LIFETIME:
            // ビーコン送受信通知 OFF 要求を発行
            wmResult = WM_SetBeaconIndication(WcmWmcbCommon, 0);
            break;

        case WM_APIID_SET_BEACON_IND:
            // WEP 暗号化設定要求を発行
            wmResult = WM_SetWEPKeyEx(WcmWmcbCommon, (u16) (wcmw->wepDesc.mode), (u16) (wcmw->wepDesc.keyId),
                                      (const u8*)(wcmw->wepDesc.key));
            break;

        case WM_APIID_SET_WEPKEY_EX:
            // AP への無線接続要求を発行
            wmResult = WM_StartConnectEx(WcmWmcbConnect, &(wcmw->bssDesc), NULL,
                                         ((wcmw->option & WCM_OPTION_MASK_POWER) == WCM_OPTION_POWER_ACTIVE ? FALSE : TRUE),
                                         ((wcmw->option & WCM_OPTION_MASK_AUTH) == WCM_OPTION_AUTH_SHAREDKEY ? (u16) WM_AUTHMODE_SHARED_KEY :
                                                     (u16) WM_AUTHMODE_OPEN_SYSTEM));
            break;
        }

        // 同期的な処理結果を確認
        switch (wmResult)
        {
        case WM_ERRCODE_OPERATING:
            break;

        case WM_ERRCODE_FIFO_ERROR:
#if WCM_DEBUG
            switch (cb->apiid)
            {
            case WM_APIID_ENABLE:
                WCMi_Printf(wcmReportText_WmSyncError, "WM_PowerOn", wmResult);
                break;

            case WM_APIID_POWER_OFF:
                WCMi_Printf(wcmReportText_WmSyncError, "WM_Disable", wmResult);
                break;

            case WM_APIID_SET_LIFETIME:
                WCMi_Printf(wcmReportText_WmSyncError, "WM_SetBeaconIndication", wmResult);
                break;

            case WM_APIID_SET_BEACON_IND:
                WCMi_Printf(wcmReportText_WmSyncError, "WM_SetWEPKeyEx", wmResult);
                break;

            case WM_APIID_SET_WEPKEY_EX:
                WCMi_Printf(wcmReportText_WmSyncError, "WM_StartConnectEx", wmResult);
                break;
            }
#endif
            WcmSetPhase(WCM_PHASE_IRREGULAR);   // ARM7 への要求発行に失敗( シーケンス途上でリトライ不能 )
            WcmNotify(WCM_RESULT_FAILURE, (wcmw->notifyId == WCM_NOTIFY_CONNECT ? &(wcmw->bssDesc) : 0), 0, __LINE__);
            break;

        case WM_ERRCODE_ILLEGAL_STATE:
            WCMi_Warning(wcmWarningText_InvalidWmState);

        /* Don't break here */
        default:
            WcmSetPhase(WCM_PHASE_FATAL_ERROR); // 想定範囲外のエラー
            WcmNotify(WCM_RESULT_FATAL_ERROR, (wcmw->notifyId == WCM_NOTIFY_CONNECT ? &(wcmw->bssDesc) : 0), 0, __LINE__);
        }
        break;

    case WM_ERRCODE_FAILED:
#if WCM_DEBUG
        switch (cb->apiid)
        {
        case WM_APIID_ENABLE:
            WCMi_Printf(wcmReportText_WmAsyncError, "WM_Enable", cb->errcode, cb->wlCmdID, cb->wlResult);
            break;

        case WM_APIID_DISABLE:
            WCMi_Printf(wcmReportText_WmAsyncError, "WM_Disable", cb->errcode, cb->wlCmdID, cb->wlResult);
            break;

        case WM_APIID_POWER_ON:
            WCMi_Printf(wcmReportText_WmAsyncError, "WM_PowerOn", cb->errcode, cb->wlCmdID, cb->wlResult);
            break;

        case WM_APIID_POWER_OFF:
            WCMi_Printf(wcmReportText_WmAsyncError, "WM_PowerOff", cb->errcode, cb->wlCmdID, cb->wlResult);
            break;

        case WM_APIID_SET_LIFETIME:
            WCMi_Printf(wcmReportText_WmAsyncError, "WM_SetLifeTime", cb->errcode, cb->wlCmdID, cb->wlResult);
            break;

        case WM_APIID_SET_BEACON_IND:
            WCMi_Printf(wcmReportText_WmAsyncError, "WM_SetBeaconIndication", cb->errcode, cb->wlCmdID, cb->wlResult);
            break;

        case WM_APIID_SET_WEPKEY_EX:
            WCMi_Printf(wcmReportText_WmAsyncError, "WM_SetWEPKeyEx", cb->errcode, cb->wlCmdID, cb->wlResult);
            break;
        }
#endif
        WcmSetPhase(WCM_PHASE_IRREGULAR);       // ARM7 でなんらかの理由でエラー
        WcmNotify(WCM_RESULT_FAILURE, (wcmw->notifyId == WCM_NOTIFY_CONNECT ? &(wcmw->bssDesc) : 0), 0, __LINE__);
        break;

    case WM_ERRCODE_ILLEGAL_STATE:
    case WM_ERRCODE_WM_DISABLE:
        WCMi_Warning(wcmWarningText_InvalidWmState);

    /* Don't break here */
    default:
        WcmSetPhase(WCM_PHASE_FATAL_ERROR);     // 想定範囲外のエラー
        WcmNotify(WCM_RESULT_FATAL_ERROR, (wcmw->notifyId == WCM_NOTIFY_CONNECT ? &(wcmw->bssDesc) : 0), 0, __LINE__);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmWmcbScanEx

  Description:  WM ライブラリからの通知を受け取るハンドラ。
                WM_StartScanEx に対する結果応答を受け取る。

  Arguments:    arg     -   WM ライブラリから渡される通知パラメータへのポインタ。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmWmcbScanEx(void* arg)
{
    WMStartScanExCallback*  cb = (WMStartScanExCallback*)arg;
    WMErrCode   wmResult = WM_ERRCODE_MAX;

    switch (cb->errcode)
    {
    case WM_ERRCODE_SUCCESS:
        // 自動探索開始中の場合は、正常に自動探索モードに入ったことを通知
        if (wcmw->phase == WCM_PHASE_IDLE_TO_SEARCH)
        {
            WcmSetPhase(WCM_PHASE_SEARCH);

            /* この通知内にて自動探索停止を要求される場合も想定される */
            WcmNotify(WCM_RESULT_SUCCESS, 0, 0, __LINE__);
        }

        // 次のステップとして行うべき処理を決定
        switch (wcmw->phase)
        {
        case WCM_PHASE_SEARCH:
            wcmw->notifyId = WCM_NOTIFY_FOUND_AP;
            if (cb->state == WM_STATECODE_PARENT_FOUND)
            {
                // AP 情報を発見した場合、内部のリストを編集しつつアプリケーションに通知
                s32 i;

                DC_InvalidateRange(wcmw->scanExParam.scanBuf, wcmw->scanExParam.scanBufSize);
                for (i = 0; i < cb->bssDescCount; i++)
                {
                    WCMi_EntryApList(cb->bssDesc[i], cb->linkLevel[i]);

                    /*
                     * IW とI/O を合わせる為に下記のようなパラメータ構成にしてあるが、
                     * cb と i をパラメータに持ったほうが妥当と思われる。
                     */
                    WcmNotifyEx(WCM_NOTIFY_FOUND_AP, WCM_RESULT_SUCCESS, cb->bssDesc[i], (void*)cb, __LINE__);
                }
            }

            // 指定されたチャンネルを一巡したかを判定
            if ((wcmw->option & WCM_OPTION_MASK_ROUNDSCAN) == WCM_OPTION_ROUNDSCAN_NOTIFY)
            {
                u32 channels = WcmCountBits(wcmw->option & WCM_OPTION_FILTER_CHANNEL);

                if (channels > 0)
                {
                    if ((wcmw->scanCount % channels) == 0)
                    {
                        // 各チャンネルのスキャンが一巡したことを通知
                        WcmNotifyEx(WCM_NOTIFY_SEARCH_AROUND, WCM_RESULT_SUCCESS, (void*)(wcmw->scanCount), 0, __LINE__);
                    }
                }
            }

            // 次のチャンネルのスキャン開始要求を発行
            wcmw->scanExParam.channelList = (u16) ((0x0001 << WcmGetNextScanChannel((u16) (32 - WcmCountLeadingZero(cb->channelList)))) >> 1);
            DC_InvalidateRange(wcmw->scanExParam.scanBuf, wcmw->scanExParam.scanBufSize);
            wcmw->scanCount++;
            wmResult = WM_StartScanEx(WcmWmcbScanEx, &(wcmw->scanExParam));
            break;

        case WCM_PHASE_SEARCH_TO_IDLE:
            // スキャン停止要求を発行
            wmResult = WM_EndScan(WcmWmcbEndScan);
            break;

        case WCM_PHASE_TERMINATING:
            // 強制終了シーケンス中の場合はここでリセット
            WcmWmReset();
            return;
        }

        // 同期的な処理結果を確認
        switch (wmResult)
        {
        case WM_ERRCODE_OPERATING:
            break;

        case WM_ERRCODE_FIFO_ERROR:
#if WCM_DEBUG
            switch (wcmw->phase)
            {
            case WCM_PHASE_SEARCH:
                WCMi_Printf(wcmReportText_WmSyncError, "WM_StartScanEx", wmResult);
                break;

            case WCM_PHASE_SEARCH_TO_IDLE:
                WCMi_Printf(wcmReportText_WmSyncError, "WM_EndScan", wmResult);
                break;
            }
#endif
            WcmSetPhase(WCM_PHASE_IRREGULAR);   // ARM7 への要求発行に失敗( シーケンス途上でリトライ不能 )
            WcmNotify(WCM_RESULT_FAILURE, 0, 0, __LINE__);
            break;

        case WM_ERRCODE_ILLEGAL_STATE:
            WCMi_Warning(wcmWarningText_InvalidWmState);

        /* Don't break here */
        default:
            WcmSetPhase(WCM_PHASE_FATAL_ERROR); // 想定範囲外のエラー
            WcmNotify(WCM_RESULT_FATAL_ERROR, 0, 0, __LINE__);
        }
        break;

    case WM_ERRCODE_FAILED:
        // スキャン要求に失敗した場合はリセット
        WCMi_Printf(wcmReportText_WmAsyncError, "WM_StartScanEx", cb->errcode, cb->wlCmdID, cb->wlResult);
        WcmWmReset();
        break;

    case WM_ERRCODE_ILLEGAL_STATE:
    case WM_ERRCODE_WM_DISABLE:
        WCMi_Warning(wcmWarningText_InvalidWmState);

    /* Don't break here */
    default:
        WcmSetPhase(WCM_PHASE_FATAL_ERROR);     // 想定範囲外のエラー
        WcmNotify(WCM_RESULT_FATAL_ERROR, 0, 0, __LINE__);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmWmcbEndScan

  Description:  WM ライブラリからの通知を受け取るハンドラ。
                WM_EndScan に対する結果応答を受け取る。

  Arguments:    arg     -   WM ライブラリから渡される通知パラメータへのポインタ。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmWmcbEndScan(void* arg)
{
    WMCallback*     cb = (WMCallback*)arg;

    switch (cb->errcode)
    {
    case WM_ERRCODE_SUCCESS:
        WcmSetPhase(WCM_PHASE_IDLE);        // 非同期シーケンス正常終了
        WcmNotify(WCM_RESULT_SUCCESS, 0, 0, __LINE__);
        break;

    case WM_ERRCODE_FAILED:
        // スキャン停止要求に失敗した場合はリセット
        WCMi_Printf(wcmReportText_WmAsyncError, "WM_EndScan", cb->errcode, cb->wlCmdID, cb->wlResult);
        WcmWmReset();
        break;

    case WM_ERRCODE_ILLEGAL_STATE:
    case WM_ERRCODE_WM_DISABLE:
        WCMi_Warning(wcmWarningText_InvalidWmState);

    /* Don't break here */
    default:
        WcmSetPhase(WCM_PHASE_FATAL_ERROR); // 想定範囲外のエラー
        WcmNotify(WCM_RESULT_FATAL_ERROR, 0, 0, __LINE__);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmWmcbConnect

  Description:  WM ライブラリからの通知を受け取るハンドラ。
                WM_StartConnectEx に対する結果応答、及び無線接続後の被切断通知
                を受け取る。

  Arguments:    arg     -   WM ライブラリから渡される通知パラメータへのポインタ。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmWmcbConnect(void* arg)
{
    WMStartConnectCallback*     cb = (WMStartConnectCallback*)arg;
    WMErrCode   wmResult = WM_ERRCODE_MAX;

    switch (cb->errcode)
    {
    case WM_ERRCODE_SUCCESS:
        switch (cb->state)
        {
        case WM_STATECODE_DISCONNECTED:                     // 被切断通知
        case WM_STATECODE_BEACON_LOST:                      // 切断された場合と同等の処理を行う
            switch (wcmw->phase)
            {
            case WCM_PHASE_DCF_TO_IDLE:
                // Auth-ID をクリア
                wcmw->authId = 0;

            /* Don't break here */
            case WCM_PHASE_IDLE_TO_DCF:
                WcmSetPhase(WCM_PHASE_IRREGULAR);
                break;

            case WCM_PHASE_DCF:
                // Auth-ID をクリア
                wcmw->authId = 0;

                // 通知種別を設定
                wcmw->notifyId = WCM_NOTIFY_DISCONNECT;

            /* Don't break here */
            case WCM_PHASE_IRREGULAR:
                WcmWmReset();
                break;

                /* 想定外のフェーズでの切断通知は無視する */
            }
            break;

#if SDK_VERSION_MAJOR > 3 || (SDK_VERSION_MAJOR == 3 && SDK_VERSION_MINOR > 0) || \
        (SDK_VERSION_MAJOR == 3 && SDK_VERSION_MINOR == 0 && SDK_VERSION_RELSTEP >= 20100)

        case WM_STATECODE_DISCONNECTED_FROM_MYSELF:         // 自分からの切断では切断関数内で処理しているので、処理なし
            break;
#endif

        case WM_STATECODE_CONNECT_START:                    // 接続の途中経過なので、処理なし
            break;

        case WM_STATECODE_CONNECTED:                        // 接続完了通知
            if (wcmw->phase == WCM_PHASE_IRREGULAR)
            {
                // 切断された後に接続完了通知が来た場合は、失敗とみなしリセット
                WCMi_Printf(wcmReportText_WmDisconnected, "WM_StartConnectEx");
                WcmSetPhase(WCM_PHASE_IDLE_TO_DCF);         // フェーズを上書き
                WcmWmReset();
            }
            else
            {
                // AID は 1 ～ 2007 までの範囲で与えられるはず
                if ((WCM_AID_MIN <= cb->aid) && (cb->aid <= WCM_AID_MAX))
                {
                    // Auth-ID を退避
                    wcmw->authId = cb->aid;

                    // DCM 通信モード開始要求を発行
                    wmResult = WM_StartDCF(WcmWmcbStartDCF, (WMDcfRecvBuf *) (wcmw->recvBuf), WCM_DCF_RECV_BUF_SIZE);
                    switch (wmResult)
                    {
                    case WM_ERRCODE_OPERATING:
                        break;

                    case WM_ERRCODE_FIFO_ERROR:
                        WCMi_Printf(wcmReportText_WmSyncError, "WM_StartDCF", wmResult);
                        WcmSetPhase(WCM_PHASE_IRREGULAR);   // ARM7 への要求発行に失敗( シーケンス途上でリトライ不能 )
                        WcmNotify(WCM_RESULT_FAILURE, &(wcmw->bssDesc), 0, __LINE__);
                        break;

                    case WM_ERRCODE_ILLEGAL_STATE:
                        WCMi_Warning(wcmWarningText_InvalidWmState);

                    /* Don't break here */
                    default:
                        WcmSetPhase(WCM_PHASE_FATAL_ERROR); // 想定範囲外のエラー
                        WcmNotify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), 0, __LINE__);
                    }
                }
                else
                {
                    // 想定外の AID が与えられた場合は、失敗とみなしリセット
                    WCMi_Printf(wcmReportText_InvalidAid, "WM_StartConnectEx");
                    WcmWmReset();
                }
            }
            break;

        default:    // 想定外のステートコード
            WcmSetPhase(WCM_PHASE_FATAL_ERROR); // 想定範囲外のエラー
            WcmNotify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), (void*)(cb->state), __LINE__);
        }
        break;

    case WM_ERRCODE_FAILED:
        wcmw->wlStatusOnConnectFail = cb->wlStatus;

    case WM_ERRCODE_NO_ENTRY:
    case WM_ERRCODE_INVALID_PARAM:
    case WM_ERRCODE_OVER_MAX_ENTRY:
        // 無線接続に失敗した場合はリセット
        WCMi_Printf(wcmReportText_WmAsyncError, "WM_StartConnectEx", cb->errcode, cb->wlCmdID, cb->wlResult);
        WcmSetPhase(WCM_PHASE_IDLE_TO_DCF);     // IRREGULAR になっている可能性があるのでフェーズを上書き
        WcmWmReset();
        break;

    case WM_ERRCODE_ILLEGAL_STATE:
    case WM_ERRCODE_WM_DISABLE:
        WCMi_Warning(wcmWarningText_InvalidWmState);

    /* Don't break here */
    default:
        WcmSetPhase(WCM_PHASE_FATAL_ERROR);     // 想定範囲外のエラー
        WcmNotify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), 0, __LINE__);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmWmcbDisconnect

  Description:  WM ライブラリからの通知を受け取るハンドラ。
                WM_Disconnect に対する結果応答を受け取る。

  Arguments:    arg     -   WM ライブラリから渡される通知パラメータへのポインタ。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmWmcbDisconnect(void* arg)
{
    WMCallback*     cb = (WMCallback*)arg;

    switch (cb->errcode)
    {
    case WM_ERRCODE_SUCCESS:
        if (wcmw->phase == WCM_PHASE_IRREGULAR)
        {
            // 切断された後に明示的な切断要求の完了通知が来た場合、念のためリセット
            WCMi_Printf(wcmReportText_WmDisconnected, "WM_Disconnect");
            WcmSetPhase(WCM_PHASE_DCF_TO_IDLE);
            WcmWmReset();
        }
        else
        {
            // Auth-ID をクリア
            wcmw->authId = 0;

            WcmSetPhase(WCM_PHASE_IDLE);    // 非同期シーケンス正常終了
            WcmNotify(WCM_RESULT_SUCCESS, &(wcmw->bssDesc), 0, __LINE__);
        }
        break;

    case WM_ERRCODE_FAILED:
    case WM_ERRCODE_ILLEGAL_STATE:          // クリティカルなタイミングで通信が切れた場合に返ってきうる
        // 無線接続の切断要求が失敗した場合はリセット
        WCMi_Printf(wcmReportText_WmAsyncError, "WM_Disconnect", cb->errcode, cb->wlCmdID, cb->wlResult);
        WcmSetPhase(WCM_PHASE_DCF_TO_IDLE);
        WcmWmReset();
        break;

    case WM_ERRCODE_WM_DISABLE:
        WCMi_Warning(wcmWarningText_InvalidWmState);

    /* Don't break here */
    default:
        WcmSetPhase(WCM_PHASE_FATAL_ERROR); // 想定範囲外のエラー
        WcmNotify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), 0, __LINE__);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmWmcbStartDCF

  Description:  WM ライブラリからの通知を受け取るハンドラ。
                WM_StartDCF に対する結果応答、及び DCF モードでのデータ受信通知
                を受け取る。

  Arguments:    arg     -   WM ライブラリから渡される通知パラメータへのポインタ。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmWmcbStartDCF(void* arg)
{
    WMStartDCFCallback*     cb = (WMStartDCFCallback*)arg;

    switch (cb->errcode)
    {
    case WM_ERRCODE_SUCCESS:
        switch (cb->state)
        {
        case WM_STATECODE_DCF_START:
            if (wcmw->phase == WCM_PHASE_IRREGULAR)
            {
                // 切断された後に DCF 開始完了通知が来た場合は、失敗とみなしリセット
                WCMi_Printf(wcmReportText_WmDisconnected, "WM_StartDCF");
                WcmSetPhase(WCM_PHASE_IDLE_TO_DCF); // フェーズを上書き
                WcmWmReset();
            }
            else
            {
                WcmSetPhase(WCM_PHASE_DCF);         // 非同期シーケンス正常終了
                WcmNotify(WCM_RESULT_SUCCESS, &(wcmw->bssDesc), 0, __LINE__);
            }
            break;

        case WM_STATECODE_DCF_IND:
            // 電波強度を退避
            WCMi_ShelterRssi((u8) (cb->recvBuf->rate_rssi >> 8));

            // CPS とのインターフェースに DCF 受信を通知
            DC_InvalidateRange(cb->recvBuf, WCM_DCF_RECV_BUF_SIZE);
            WCMi_CpsifRecvCallback(cb->recvBuf);
            break;

        default:    // 想定外のステートコード
            WcmSetPhase(WCM_PHASE_FATAL_ERROR); // 想定範囲外のエラー
            WcmNotify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), (void*)(cb->state), __LINE__);
        }
        break;

    /* StartDCF には WM_ERRCODE_FAILED は返ってこないので省略 */
    case WM_ERRCODE_WM_DISABLE:
        WCMi_Warning(wcmWarningText_InvalidWmState);

    /* Don't break here */
    default:
        WcmSetPhase(WCM_PHASE_FATAL_ERROR);     // 想定範囲外のエラー
        WcmNotify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), 0, __LINE__);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmWmcbEndDCF

  Description:  WM ライブラリからの通知を受け取るハンドラ。
                WM_EndDCF に対する結果応答を受け取る。

  Arguments:    arg     -   WM ライブラリから渡される通知パラメータへのポインタ。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmWmcbEndDCF(void* arg)
{
    WMCallback*     cb = (WMCallback*)arg;
    WMErrCode       wmResult = WM_ERRCODE_MAX;

    switch (cb->errcode)
    {
    case WM_ERRCODE_SUCCESS:
        if (wcmw->phase == WCM_PHASE_IRREGULAR)
        {
            // 切断された後に DCF 終了完了通知が来た場合は、失敗とみなしリセット
            WcmSetPhase(WCM_PHASE_DCF_TO_IDLE);
            WcmWmReset();
        }
        else
        {
            // AP との無線接続切断要求を発行
            wmResult = WM_Disconnect(WcmWmcbDisconnect, 0);
            switch (wmResult)
            {
            case WM_ERRCODE_OPERATING:
                break;

            case WM_ERRCODE_FIFO_ERROR:
                WCMi_Printf(wcmReportText_WmSyncError, "WM_Disconnect", wmResult);
                WcmSetPhase(WCM_PHASE_IRREGULAR);   // ARM7 への要求発行に失敗( シーケンス途上でリトライ不能 )
                WcmNotify(WCM_RESULT_FAILURE, &(wcmw->bssDesc), 0, __LINE__);
                break;

            case WM_ERRCODE_ILLEGAL_STATE:          // クリティカルなタイミングで通信が切れた場合
                // 切断する寸前に AP 側から切断された場合は、失敗とみなしリセット
                WCMi_Printf(wcmReportText_WmSyncError, "WM_Disconnect", wmResult);
                WcmSetPhase(WCM_PHASE_DCF_TO_IDLE);
                WcmWmReset();
                break;

            default:
                WcmSetPhase(WCM_PHASE_FATAL_ERROR); // 想定範囲外のエラー
                WcmNotify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), 0, __LINE__);
            }
        }
        break;

    case WM_ERRCODE_FAILED:
    case WM_ERRCODE_ILLEGAL_STATE:                  // クリティカルなタイミングで通信が切れた場合
        // DCF 通信モード終了に失敗した場合はリセット
        WCMi_Printf(wcmReportText_WmAsyncError, "WM_EndDCF", cb->errcode, cb->wlCmdID, cb->wlResult);
        WcmSetPhase(WCM_PHASE_DCF_TO_IDLE);         // IRREGULAR になっている可能性があるのでフェーズを上書き
        WcmWmReset();
        break;

    case WM_ERRCODE_WM_DISABLE:
        WCMi_Warning(wcmWarningText_InvalidWmState);

    /* Don't break here */
    default:
        WcmSetPhase(WCM_PHASE_FATAL_ERROR);         // 想定範囲外のエラー
        WcmNotify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), 0, __LINE__);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmWmcbReset

  Description:  WM ライブラリからの通知を受け取るハンドラ。
                WM_Reset に対する結果応答を受け取る。

  Arguments:    arg     -   WM ライブラリから渡される通知パラメータへのポインタ。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmWmcbReset(void* arg)
{
    WMCallback*     cb = (WMCallback*)arg;
    WMErrCode       wmResult = WM_ERRCODE_MAX;

    switch (cb->errcode)
    {
    case WM_ERRCODE_SUCCESS:
        // リセット重複呼び出し管理フラグをクリア
        wcmw->resetting = WCM_RESETTING_OFF;

        // Auth-ID をクリア
        wcmw->authId = 0;

        switch (wcmw->phase)
        {
        case WCM_PHASE_IDLE_TO_SEARCH:  // AP 自動探索開始中
        case WCM_PHASE_SEARCH:          // AP 自動探索中
            // スキャン処理に失敗してリセットすることになった旨を通知
            WcmSetPhase(WCM_PHASE_IDLE);
            WcmNotify(WCM_RESULT_FAILURE, 0, 0, __LINE__);
            break;

        case WCM_PHASE_SEARCH_TO_IDLE:  // AP 自動探索停止中
            // 要求した AP 自動探索停止処理が完了したことを通知
            WcmSetPhase(WCM_PHASE_IDLE);
            WcmNotify(WCM_RESULT_SUCCESS, 0, 0, __LINE__);
            break;

        case WCM_PHASE_IDLE_TO_DCF:     // 接続中
            // 接続処理に失敗してリセットすることになった旨を通知
            {
                u16 wlStatus = wcmw->wlStatusOnConnectFail;

                wcmw->wlStatusOnConnectFail = 0x0000;
#ifdef WCM_CAMOUFLAGE_RATESET
                /* レートセットの不整合により接続に失敗した時の対策 */
                if (wlStatus == WCM_CONNECT_STATUSCODE_ILLEGAL_RATESET)
                {
                    if ((wcmw->bssDesc.rateSet.support & WCM_CAMOUFLAGE_RATESET) != WCM_CAMOUFLAGE_RATESET)
                    {
                        WCMi_Printf(wcmReportText_SupportRateset, wcmw->bssDesc.ssid);
                        wcmw->bssDesc.rateSet.support |= WCM_CAMOUFLAGE_RATESET;

                        // 自動的に接続をリトライ
                        wmResult = WM_StartConnectEx(WcmWmcbConnect, &(wcmw->bssDesc), NULL,
                                                     ((wcmw->option & WCM_OPTION_MASK_POWER) == WCM_OPTION_POWER_ACTIVE ? FALSE : TRUE),
                                                             ((wcmw->option & WCM_OPTION_MASK_AUTH) == WCM_OPTION_AUTH_SHAREDKEY ?
                                                                     (u16) WM_AUTHMODE_SHARED_KEY : (u16) WM_AUTHMODE_OPEN_SYSTEM
                                                                     ));

                        // 同期的な処理結果を確認
                        switch (wmResult)
                        {
                        case WM_ERRCODE_OPERATING:
                            break;

                        case WM_ERRCODE_FIFO_ERROR:
                            WcmSetPhase(WCM_PHASE_IRREGULAR);   // ARM7 への要求発行に失敗( シーケンス途上でリトライ不能 )
                            WcmNotify(WCM_RESULT_FAILURE, &(wcmw->bssDesc), (void*)wlStatus, __LINE__);
                            break;

                        case WM_ERRCODE_ILLEGAL_STATE:
                            WCMi_Warning(wcmWarningText_InvalidWmState);

                        /* Don't break here */
                        default:
                            WcmSetPhase(WCM_PHASE_FATAL_ERROR); // 想定範囲外のエラー
                            WcmNotify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), (void*)wlStatus, __LINE__);
                        }

                        return;
                    }
                }
#endif
                WcmSetPhase(WCM_PHASE_IDLE);
                WcmNotify(WCM_RESULT_FAILURE, &(wcmw->bssDesc), (void*)wlStatus, __LINE__);
            }
            break;

        case WCM_PHASE_DCF:         // DCF 通信中
        case WCM_PHASE_IRREGULAR:
            // AP から切断されたことを通知
            WcmSetPhase(WCM_PHASE_IDLE);
            WcmNotify(WCM_RESULT_SUCCESS, &(wcmw->bssDesc), (void*)1, __LINE__);

            break;

        case WCM_PHASE_DCF_TO_IDLE: // 切断中
            // 要求した切断処理が完了したことを通知
            WcmSetPhase(WCM_PHASE_IDLE);
            WcmNotify(WCM_RESULT_SUCCESS, &(wcmw->bssDesc), 0, __LINE__);
            break;

        case WCM_PHASE_TERMINATING: // 強制終了中
            // リセットの完了に続いて、PHASE_WAIT に戻すように連鎖的に処理を継続
            wmResult = WM_PowerOff(WcmWmcbCommon);
            switch (wmResult)
            {
            case WM_ERRCODE_OPERATING:
                break;

            case WM_ERRCODE_FIFO_ERROR:
                WCMi_Printf(wcmReportText_WmSyncError, "WM_Reset", wmResult);
                WcmSetPhase(WCM_PHASE_IRREGULAR);   // ARM7 への要求発行に失敗( シーケンス途上でリトライ不能 )
                WcmNotify(WCM_RESULT_FAILURE, 0, 0, __LINE__);
                break;

            case WM_ERRCODE_ILLEGAL_STATE:
                WCMi_Warning(wcmWarningText_InvalidWmState);

            /* Don't break here */
            default:
                WcmSetPhase(WCM_PHASE_FATAL_ERROR); // 想定範囲外のエラー
                WcmNotify(WCM_RESULT_FATAL_ERROR, 0, 0, __LINE__);
            }
            break;

        default:
            WcmSetPhase(WCM_PHASE_FATAL_ERROR);     // 想定範囲外のエラー
            WcmNotify(WCM_RESULT_FATAL_ERROR, 0, (void*)(wcmw->phase), __LINE__);
        }
        break;

    default:
        /* リセットに失敗した場合は復旧不可能 */
        WcmSetPhase(WCM_PHASE_FATAL_ERROR);         // 想定範囲外のエラー
        WcmNotify(WCM_RESULT_FATAL_ERROR, 0, 0, __LINE__);
    }
}

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
