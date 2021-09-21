/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - include
  File:     wm_api.h

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wm_api.h,v $
  Revision 1.76  2007/11/09 13:08:09  seiki_masashi
  電波の受信のみ可能な状態に無線を初期化する関数を追加

  Revision 1.75  2007/02/20 00:28:08  kitase_hirotake
  indent source

  Revision 1.74  2006/07/06 11:45:31  okubata_ryoma
  undo

  Revision 1.72  2006/02/20 07:42:43  seiki_masashi
  WM_SetGameInfo 関数の引数名を変更

  Revision 1.71  2006/02/20 06:48:19  seiki_masashi
  WM_IsValidGameInfo 関数と WM_IsValidGameBeacon 関数の追加

  Revision 1.70  2006/01/18 02:12:39  kitase_hirotake
  do-indent

  Revision 1.69  2005/11/25 00:46:09  seiki_masashi
  mask を WMMPParam 構造体内に移動

  Revision 1.68  2005/11/24 03:02:42  seiki_masashi
  WM_SetMPParameter 関数と関連する定義を追加

  Revision 1.67  2005/11/02 01:32:23  ooe
  WM_StartTestRxMode, WM_StopTestRxModeを追加。

  Revision 1.66  2005/11/01 08:59:44  seiki_masashi
  WM_IsMP, WM_GetMPReadyAIDs の各関数を非公開関数に変更

  Revision 1.65  2005/10/28 11:18:55  seiki_masashi
  WMStatus 構造体へのアクセサの実装

  Revision 1.64  2005/07/27 06:52:42  ooe
  WM_SetPowerSaveModeを追加。

  Revision 1.63  2005/06/07 05:44:48  seiki_masashi
  Key Sharing に関する特別処理を低減するための変更

  Revision 1.62  2005/04/11 08:51:18  yosizaki
  add WM_GetNextTgid().

  Revision 1.61  2005/04/11 01:16:32  seiki_masashi
  WMi_StartParentEx 関数を追加

  Revision 1.60  2005/03/17 05:09:13  ooe
  WM_GetOtherElementsを追加。

  Revision 1.59  2005/03/03 14:44:57  seiki_masashi
  WM_DISABLE_KEYSHARING スイッチの追加

  Revision 1.58  2005/02/28 05:26:35  yosizaki
  do-indent.

  Revision 1.57  2005/02/14 11:48:18  terui
  WM_StartScan[Ex]に関するコメントFIX。

  Revision 1.56  2005/02/14 06:35:08  terui
  WMi_CheckMpPacketTimeRequired関数を追加。

  Revision 1.55  2005/02/08 06:43:42  ooe
  WM_StartScanExの引数の型を変更(WMScanExParam)

  Revision 1.54  2005/02/03 11:42:51  ooe
  WM_SetWEPKeyExを追加。

  Revision 1.53  2005/02/02 07:46:20  takano_makoto
  WM_CopyBssid16, WM_IsBssidEqual16を追加

  Revision 1.52  2005/01/27 12:03:27  takano_makoto
  typo修正

  Revision 1.51  2005/01/27 09:11:49  takano_makoto
  WM_CopyBssId, WM_IsBssidEqualをu8単位で処理するように修正。

  Revision 1.50  2005/01/27 08:06:21  takano_makoto
  ASSERTを追加

  Revision 1.49  2005/01/17 01:04:09  ooe
  WM_StartConnectExを追加し、WM_StartConnectをインライン化。
  DCF送信バッファサイズ計算用に、WM_SIZE_DCF_CHILD_SEND_BUFFERを追加。

  Revision 1.48  2005/01/11 07:54:48  takano_makoto
  fix copyright header.

  Revision 1.47  2005/01/06 02:05:10  seiki_masashi
  WM_StartMP, WM_SetMPData, WM_SetMPDataToPort の inline 化

  Revision 1.46  2004/12/22 02:29:53  terui
  WM_IsExistAllowedChannelを追加。

  Revision 1.45  2004/11/17 07:31:04  terui
  Only fix comment.

  Revision 1.44  2004/11/10 05:41:49  takano_makoto
  WM_CopyBssid, WM_IsBssidEqualの引数をconstポインタに修正

  Revision 1.43  2004/11/10 00:59:12  takano_makoto
  WM_IsBssidEqual, WM_CopyBssidを追加

  Revision 1.42  2004/11/09 00:03:59  seiki_masashi
  Null 応答によるタイムアウトを実装

  Revision 1.41  2004/11/02 07:27:38  terui
  コメント修正。

  Revision 1.40  2004/10/29 02:53:37  terui
  WM_StartScanExを一時的に非公開関数に変更。

  Revision 1.39  2004/10/26 08:22:09  ooe
  WM_StartScanExを追加

  Revision 1.38  2004/10/22 04:36:47  terui
  WMErrCodeとしてint型を返していた関数について、WMErrCode列挙型を返すように変更。

  Revision 1.37  2004/10/18 11:08:54  terui
  WM_SetBeaconPeriod関数を一旦非公開に変更。

  Revision 1.36  2004/10/18 04:21:00  terui
  Fix comment only.

  Revision 1.35  2004/10/18 04:17:46  terui
  ビーコン間隔、スキャン間隔をばらつかせる仕様を追加。

  Revision 1.34  2004/10/15 14:09:16  terui
  Fix comment.

  Revision 1.33  2004/10/05 09:45:28  terui
  MACアドレスをu8型の配列に統一。

  Revision 1.32  2004/10/04 13:40:25  terui
  ゲームグループIDをu32型に統一。

  Revision 1.31  2004/10/03 07:51:10  ooe
  ipl_branchとマージ。 ignoreFatalErrorフラグに対応。

  Revision 1.30  2004/10/01 04:09:26  terui
  WM_ENABLE_TESTMODEスイッチによりテストモード操作についての機能を制限。

  Revision 1.29  2004/09/20 10:30:21  ooe
  WM_SetBeaconPeriodを追加。

  Revision 1.28  2004/09/16 02:59:28  seiki_masashi
  WM_GetMPReceiveBufferSize の不具合を修正

  Revision 1.27  2004/09/13 04:09:12  seiki_masashi
  WM_SetMPFrequency の追加

  Revision 1.26  2004/09/11 04:09:38  terui
  Only fix comment.

  Revision 1.25  2004/09/10 12:04:37  seiki_masashi
  キーシェアリングの送信バイト数をライブラリ内で隠蔽するように変更

  Revision 1.24  2004/09/10 04:56:16  seiki_masashi
  WM_SIZE_MP_*_BUFFER を正確な値に変更

  Revision 1.23  2004/09/09 07:24:11  seiki_masashi
  WM_SetMPDataToPortEx の追加

  Revision 1.22  2004/09/09 01:58:20  terui
  初期化・終了処理の細分化に伴い、プロトタイプ宣言を追加。

  Revision 1.21  2004/09/02 09:26:49  seiki_masashi
  fix typo.

  Revision 1.20  2004/09/02 09:15:33  terui
  WMi_GetStatusAddress関数を追加。

  Revision 1.19  2004/08/30 00:53:09  terui
  Move definition about LinkLevel to wm.h

  Revision 1.18  2004/08/27 06:44:20  seiki_masashi
  WM_StartMPEx の引数に defaultRetryCount, fixFreqMode を追加

  Revision 1.17  2004/08/27 02:16:00  terui
  Only fix comment.

  Revision 1.16  2004/08/25 08:20:38  seiki_masashi
  WM_StatMPEx の追加

  Revision 1.15  2004/08/25 07:59:06  terui
  Add WM_ConvGgid*.

  Revision 1.14  2004/08/24 13:20:04  terui
  リンク強度取得関数及び強度を示す列挙型を追加。

  Revision 1.13  2004/08/23 04:39:10  ooe
  WM_StartConnect()の仕様変更。引数にssidを追加。

  Revision 1.12  2004/08/20 11:54:21  terui
  Childs -> Children

  Revision 1.11  2004/08/20 06:47:39  terui
  WM_DisconnectChildsを追加。

  Revision 1.10  2004/08/19 15:25:25  miya
  modified SetGameInfo function.

  Revision 1.9  2004/08/18 12:54:30  seiki_masashi
  WM_SIZE_MP_*_*_BUFFER へ名称変更

  Revision 1.8  2004/08/18 06:47:28  terui
  WM_GetAllowedChannel関数を追加。
  WM_SetEntry関数を追加。

  Revision 1.7  2004/08/18 00:24:47  seiki_masashi
  WM_StartMP の引数の微調整

  Revision 1.6  2004/08/17 12:49:45  seiki_masashi
  WM_StartMP の引数に送信バッファを指定するように変更

  Revision 1.5  2004/08/11 04:27:09  seiki_masashi
  StartDataSharing の引数に doubleMode を追加

  Revision 1.4  2004/08/10 13:17:04  terui
  Fix comment.

  Revision 1.1  2004/08/10 09:10:17  terui
  Initial upload

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_WM_ARM9_WM_API_H_
#define NITRO_WM_ARM9_WM_API_H_

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================*/

/*---------------------------------------------------------------------------*
    定数定義
 *---------------------------------------------------------------------------*/
#define     WM_DEFAULT_BEACON_PERIOD    200     // ms
#define     WM_DEFAULT_SCAN_PERIOD      30      // ms
#define     WM_MAX_MP_PACKET_TIME       5600    // us


/*---------------------------------------------------------------------------*
    関数定義
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         WM_Init

  Description:  WMライブラリの初期化処理を行う。
                ARM9側の初期化のみを行う同期関数。

  Arguments:    wmSysBuf    -   呼び出し元によって確保されたバッファへのポインタ。
                                バッファのサイズはWM_SYSTEM_BUF_SIZEだけ必要。
                dmaNo       -   WMが使用するDMA番号。

  Returns:      WMErrCode   -   処理結果を返す。
 *---------------------------------------------------------------------------*/
WMErrCode WM_Init(void *wmSysBuf, u16 dmaNo);

/*---------------------------------------------------------------------------*
  Name:         WM_Finish

  Description:  WMライブラリの終了処理を行う。同期関数。
                WM_Init関数を呼ぶ前の状態に戻る。

  Arguments:    None.

  Returns:      WMErrCode   -   処理結果を返す。
 *---------------------------------------------------------------------------*/
WMErrCode WM_Finish(void);

/*---------------------------------------------------------------------------*
  Name:         WM_Enable

  Description:  無線ハードウェアを使用可能な状態にする。
                内部状態はREADY状態からSTOP状態に遷移される。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_Enable(WMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         WM_EnableForListening

  Description:  無線ハードウェアを使用可能な状態にする。
                内部状態はREADY状態からSTOP状態に遷移される。
                電波を送出するオペレーションはできない。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                blink       -   LED を点滅させるか

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_EnableForListening(WMCallbackFunc callback, BOOL blink);

/*---------------------------------------------------------------------------*
  Name:         WMi_EnableEx

  Description:  無線ハードウェアを使用可能な状態にする。
                内部状態はREADY状態からSTOP状態に遷移される。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                miscFlags   -   初期化時のフラグ

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WMi_EnableEx(WMCallbackFunc callback, u32 miscFlags);

/*---------------------------------------------------------------------------*
  Name:         WM_Disable

  Description:  無線ハードウェアを使用禁止状態にする。
                内部状態はSTOP状態からREADY状態に遷移される。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_Disable(WMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         WM_PowerOn

  Description:  無線ハードウェアを起動する。
                内部状態はSTOP状態からIDLE状態に遷移される。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_PowerOn(WMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         WM_PowerOff

  Description:  無線ハードウェアをシャットダウンする。
                内部状態はIDLE状態からSTOP状態に遷移される。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_PowerOff(WMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         WM_Initialize

  Description:  WM初期化処理を行う。

  Arguments:    wmSysBuf    -   呼び出し元によって確保されたバッファへのポインタ。
                                バッファのサイズはWM_SYSTEM_BUF_SIZEだけ必要。
                callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                dmaNo       -   WMが使用するDMA番号。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_Initialize(void *wmSysBuf, WMCallbackFunc callback, u16 dmaNo);

/*---------------------------------------------------------------------------*
  Name:         WM_InitializeForListening

  Description:  WM初期化処理を行う。
                スキャン以外の通信はできない。

  Arguments:    wmSysBuf    -   呼び出し元によって確保されたバッファへのポインタ。
                                バッファのサイズはWM_SYSTEM_BUF_SIZEだけ必要。
                callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                dmaNo       -   WMが使用するDMA番号。
                blink       -   LED を点滅させるかどうか

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_InitializeForListening(void *wmSysBuf, WMCallbackFunc callback, u16 dmaNo, BOOL blink);

/*---------------------------------------------------------------------------*
  Name:         WMi_InitializeEx

  Description:  WM初期化処理を行う。

  Arguments:    wmSysBuf    -   呼び出し元によって確保されたバッファへのポインタ。
                                バッファのサイズはWM_SYSTEM_BUF_SIZEだけ必要。
                callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                dmaNo       -   WMが使用するDMA番号。
                miscFlags   -   初期化時のフラグ

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WMi_InitializeEx(void *wmSysBuf, WMCallbackFunc callback, u16 dmaNo, u32 miscFlags);

/*---------------------------------------------------------------------------*
  Name:         WM_SetIndCallback

  Description:  WM7からの状況通知に対して呼びだされる関数を設定する。
                この関数自体は同期関数。

  Arguments:    callback    -   無線ファームウェアからの状況通知時に呼び出される
                                コールバック関数を指定。

  Returns:      WMErrCode   -   処理結果を返す。
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetIndCallback(WMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         WM_SetPortCallback

  Description:  WM7からの通信フレーム受信通知に対して呼びだされる関数を設定する。

  Arguments:    port        -   ポート番号。
                callback    -   受信通知時に呼び出されるコールバック関数。
                arg         -   コールバック関数に WMPortRecvCallback.arg として
                                渡される引数。

  Returns:      WMErrCode   -   処理結果を返す。
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetPortCallback(u16 port, WMCallbackFunc callback, void *arg);

/*---------------------------------------------------------------------------*
  Name:         WM_Reset

  Description:  無線ライブラリをリセットし、初期化直後の状態に戻す。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_Reset(WMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         WM_End

  Description:  無線ライブラリを終了する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_End(WMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         WM_ReadStatus

  Description:  無線ライブラリ状態を示す構造体を取得する。同期関数。

  Arguments:    statusBuf   -   状態を取得する変数へのポインタ。

  Returns:      WMErrCode   -   処理結果を返す。
 *---------------------------------------------------------------------------*/
WMErrCode WM_ReadStatus(WMStatus *statusBuf);

/*---------------------------------------------------------------------------*
  Name:         WM_SetParentParameter

  Description:  親機情報を設定する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                pparaBuf    -   親機情報を示す構造体へのポインタ。
                                pparaBuf及びpparaBuf->userGameInfoの実体は強制的に
                                キャッシュストアされる点に注意。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetParentParameter(WMCallbackFunc callback, const WMParentParam *pparaBuf);

/*---------------------------------------------------------------------------*
  Name:         WMi_StartParent

  Description:  親機として通信を開始する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                powerSave   -   省電力モードを使用する場合はTRUE、しない場合はFALSE。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WMi_StartParentEx(WMCallbackFunc callback, BOOL powerSave);

/*---------------------------------------------------------------------------*
  Name:         WM_StartParent

  Description:  親機として通信を開始する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartParent(WMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         WM_EndParent

  Description:  親機としての通信を停止する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_EndParent(WMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         WM_StartScan

  Description:  子機として親機のスキャンを開始する。
                一度の呼び出しで一台の親機情報を取得する。
                WM_EndScanを呼び出さずに連続して呼び出しても良い。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                param       -   スキャン情報を示す構造体へのポインタ。
                                スキャン結果の情報はparam->scanBufにARM7が直接書き出す
                                ので、キャッシュラインに合わせておく必要がある。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartScan(WMCallbackFunc callback, const WMScanParam *param);

/*---------------------------------------------------------------------------*
  Name:         WM_StartScanEx

  Description:  子機として親機のスキャンを開始する。
                一度の呼び出しで複数台の親機情報を取得する。
                WM_EndScanを呼び出さずに連続して呼び出しても良い。

  Arguments:    callback - 非同期処理が完了した時に呼び出されるコールバック関数。
                param    - スキャン情報を示す構造体へのポインタ。
                           スキャン結果の情報はparam->scanBufにARM7が直接書き出す
                           ので、キャッシュラインに合わせておく必要がある。

  Returns:      int      - WM_ERRCODE_*型の処理結果を返す。
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartScanEx(WMCallbackFunc callback, const WMScanExParam *param);

/*---------------------------------------------------------------------------*
  Name:         WM_EndScan

  Description:  子機としてのスキャン処理を停止する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_EndScan(WMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         WM_StartConnectEx

  Description:  子機として親機への接続を開始する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                pInfo       -   接続する親機の情報。
                                WM_StartScanにて取得した構造体を指定する。
                                この構造体の実体は強制的にキャッシュストア
                                される点に注意。
                ssid        -   親機に通知する子機情報(24Byte(WM_SIZE_CHILD_SSID)固定サイズ)
                powerSave   -   省電力モードを使用する場合はTRUE、しない場合はFALSE。
                authMode    -   Authenticationのモード選択。
                                  WM_AUTHMODE_OPEN_SYSTEM : OPEN SYSTEMモード
                                  WM_AUTHMODE_SHARED_KEY  : SHARED KEY モード

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartConnectEx(WMCallbackFunc callback, const WMBssDesc *pInfo, const u8 *ssid,
                            BOOL powerSave, u16 authMode);

/*---------------------------------------------------------------------------*
  Name:         WM_StartConnect

  Description:  子機として親機への接続を開始する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                pInfo       -   接続する親機の情報。WM_StartScanにて取得した構造体を
                                指定する。この構造体の実体は強制的にキャッシュストア
                                される点に注意。
                ssid        -   親機に通知する子機情報(24Byte(WM_SIZE_CHILD_SSID)固定サイズ)

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
static inline WMErrCode
WM_StartConnect(WMCallbackFunc callback, const WMBssDesc *pInfo, const u8 *ssid)
{
    return WM_StartConnectEx(callback, pInfo, ssid, TRUE, WM_AUTHMODE_OPEN_SYSTEM);
}

/*---------------------------------------------------------------------------*
  Name:         WM_Disconnect

  Description:  確立されている接続を切断する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                aid         -   切断する通信相手のAID。
                                親機の場合、IDが1～15の子機を個別に切断。
                                子機の場合、IDが0の親機との通信を終了。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_Disconnect(WMCallbackFunc callback, u16 aid);

/*---------------------------------------------------------------------------*
  Name:         WM_DisconnectChildren

  Description:  接続確立されている子機をそれぞれ切断する。親機専用関数。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                aidBitmap   -   切断する子機のAIDビットフィールド。
                                最下位ビットは無視され、bit 1～15 がAID 1～15 の子機
                                をそれぞれ示す。
                                接続されていない子機を示すビットは無視されるので、
                                接続状況に関わらず全子機を切断する場合は0xFFFFを指定。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_DisconnectChildren(WMCallbackFunc callback, u16 aidBitmap);

/*---------------------------------------------------------------------------*
  Name:         WM_GetMPSendBufferSize

  Description:  MP 通信の送信バッファのサイズを現在の WM の状態から計算する。

  Arguments:    None.

  Returns:      int     -   WM_StartMP に渡す必要がある送信バッファのサイズ。
 *---------------------------------------------------------------------------*/
int     WM_GetMPSendBufferSize(void);

/*---------------------------------------------------------------------------*
  Name:         WM_GetMPReceiveBufferSize

  Description:  MP 通信の受信バッファのサイズを現在の WM の状態から計算する。

  Arguments:    None.

  Returns:      int     -   WM_StartMP に渡す必要がある受信バッファのサイズ。
 *---------------------------------------------------------------------------*/
int     WM_GetMPReceiveBufferSize(void);

/*---------------------------------------------------------------------------*
  Name:         WM_StartMPEx

  Description:  MP通信を開始する。詳細な動作モードを指定可能。親機・子機共通関数。

  Arguments:    callback        -   非同期処理が完了した時に呼び出されるコールバック関数。
                recvBuf         -   受信データ格納バッファ。
                                    ARM7が直接データを書き出すので、キャッシュに注意。
                recvBufSize     -   受信データ格納バッファのサイズ。
                                    WM_GetReceiveBufferSize() の返り値よりも大きい必要がある。
                sendBuf         -   送信データ格納バッファ。
                                    ARM7が直接データを書き出すので、キャッシュに注意。
                sendBufSize     -   送信データ格納バッファのサイズ。
                                    WM_GetSendBufferSize() の返り値よりも大きい必要がある。
                mpFreq          -   1フレームに何回 MP 通信をするか。
                                    0 で連続送信モード。親機のみ意味を持つ。
                defaultRetryCount - 0～7番ポートの通信の際に、送信に失敗した場合に
                                    リトライを行う標準の回数。0 でリトライを行わない。
                minPollBmpMode  -   MP 通信時の pollBitmap を送信する packet の宛先に応じて
                                    最低限の集合に抑える動作モード。
                singlePacketMode -  1 回の MP について一つの packet しか送らなくなる
                                    特殊な動作モード。
                fixFreqMode     -   リトライによる MP 通信の増加を禁止する特殊な動作モード。
                                    1フレーム中の MP 通信の回数をちょうど mpFreq 回に固定。

                ignoreFatalError -  FatalError発生時にAutoDisconnectを行なわなくなる。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartMPEx(WMCallbackFunc callback,
                       u16 *recvBuf,
                       u16 recvBufSize,
                       u16 *sendBuf,
                       u16 sendBufSize,
                       u16 mpFreq,
                       u16 defaultRetryCount,
                       BOOL minPollBmpMode,
                       BOOL singlePacketMode, BOOL fixFreqMode, BOOL ignoreFatalError);

/*---------------------------------------------------------------------------*
  Name:         WM_StartMP

  Description:  MP通信を開始する。親機・子機共通関数。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                recvBuf     -   受信データ格納バッファ。
                                ARM7が直接データを書き出すので、キャッシュに注意。
                recvBufSize -   受信データ格納バッファのサイズ。
                                WM_GetReceiveBufferSize() の返り値よりも大きい必要がある。
                sendBuf     -   送信データ格納バッファ。
                                ARM7が直接データを書き出すので、キャッシュに注意。
                sendBufSize -   送信データ格納バッファのサイズ。
                                WM_GetSendBufferSize() の返り値よりも大きい必要がある。
                mpFreq      -   1フレームに何回 MP 通信をするか。
                                0 で連続送信モード。親機のみ意味を持つ。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartMP(WMCallbackFunc callback,
                     u16 *recvBuf, u16 recvBufSize, u16 *sendBuf, u16 sendBufSize, u16 mpFreq);

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPParameter

  Description:  MP 通信用の各種パラメータを設定する

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                param       -   MP 通信用のパラメータを格納した構造体へのポインタ

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetMPParameter(WMCallbackFunc callback, const WMMPParam * param);

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPChildSize

  Description:  1回のMP通信での子機の送信バイト数を設定する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                childSize   -   子機の送信バイト数。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetMPChildSize(WMCallbackFunc callback, u16 childSize);

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPParentSize

  Description:  1回のMP通信での親機の送信バイト数を設定する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                childSize   -   親機の送信バイト数。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetMPParentSize(WMCallbackFunc callback, u16 parentSize);

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPFrequency

  Description:  1フレームに何回 MP 通信するかを切り替える。親機用関数。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                mpFreq      -   1フレームに何回 MP 通信をするか。
                                0 で連続送信モード。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetMPFrequency(WMCallbackFunc callback, u16 mpFreq);

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPInterval

  Description:  1フレーム中に連続して MP 通信を行う場合の間隔を設定する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                parentInterval - 1フレーム中に連続して行う場合の親機の MP 通信間隔 (ms)
                childInterval  - 1フレーム中に連続して行う場合の子機が MP 通信を準備する間隔 (ms)

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetMPInterval(WMCallbackFunc callback, u16 parentInterval, u16 childInterval);

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPTiming

  Description:  フレーム同期通信モード時に MP 通信の準備を開始するタイミングを設定する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                parentVCount -  フレーム同期通信時の親機処理の開始 V Count
                childVCount  -  フレーム同期通信時の子機処理の開始 V Count

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetMPTiming(WMCallbackFunc callback, u16 parentVCount, u16 childVCount);

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPDataToPortEx

  Description:  MP通信にて送信するデータを予約する。親機・子機共通関数。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                arg         -   callback に渡される引数
                sendData    -   送信予約するデータへのポインタ。
                                このデータの実体は強制的にキャッシュストアされる点に注意。
                sendDataSize -  送信予約するデータのサイズ。
                destBitmap  -   送信先子機を示す aid の bitmap を指定。
                port        -   送信するポート番号。
                prio        -   優先度 (0:最高～3:最低)

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetMPDataToPortEx(WMCallbackFunc callback,
                               void *arg,
                               const u16 *sendData,
                               u16 sendDataSize, u16 destBitmap, u16 port, u16 prio);

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPDataToPort

  Description:  MP通信にて送信するデータを予約する。親機・子機共通関数。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                sendData    -   送信予約するデータへのポインタ。
                                このデータの実体は強制的にキャッシュストアされる点に注意。
                sendDataSize -  送信予約するデータのサイズ。
                destBitmap  -   送信先子機を示す aid の bitmap を指定。
                port        -   送信するポート番号。
                prio        -   優先度 (0:最高～3:最低)

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
static inline WMErrCode
WM_SetMPDataToPort(WMCallbackFunc callback, const u16 *sendData, u16 sendDataSize, u16 destBitmap,
                   u16 port, u16 prio)
{
    return WM_SetMPDataToPortEx(callback, NULL, sendData, sendDataSize, destBitmap, port, prio);
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPData

  Description:  MP通信にて送信するデータを予約する。親機・子機共通関数。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                sendData    -   送信予約するデータへのポインタ。
                                このデータの実体は強制的にキャッシュストアされる点に注意。
                sendDataSize -  送信予約するデータのサイズ。
                tmptt       -   連続送信モードの場合、MPパケット送信時間を設定。
                                (親機のみ有効。) (2004/07/26 現在・無視)
                pollbmp     -   連続送信モードの場合、送信先子機を示すPollマップを指定。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
static inline WMErrCode
WM_SetMPData(WMCallbackFunc callback, const u16 *sendData, u16 sendDataSize, u16 tmptt, u16 pollbmp)
{
#pragma unused( tmptt )
    return WM_SetMPDataToPortEx(callback, NULL, sendData, sendDataSize, pollbmp, WM_PORT_RAWDATA,
                                WM_PRIORITY_NORMAL);
}

/*---------------------------------------------------------------------------*
  Name:         WM_EndMP

  Description:  MP通信を停止する。親機・子機共通関数。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_EndMP(WMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         WM_StartDCF

  Description:  インフラストラクチャモードでの通信を開始する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                recvBuf     -   データ受信バッファへのポインタ。
                                ARM7が直接データを書き出すので、キャッシュに注意。
                recvBufSize -   データ受信バッファのサイズ。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartDCF(WMCallbackFunc callback, WMDcfRecvBuf *recvBuf, u16 recvBufSize);

/*---------------------------------------------------------------------------*
  Name:         WM_SetDCFData

  Description:  インフラストラクチャモード通信にて送信するデータを予約する

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                destAdr     -   通信相手のMACアドレスを示すバッファへのポインタ。
                sendData    -   送信予約するデータへのポインタ。
                                送信予約するデータの実体は強制的にキャッシュストア
                                される点に注意。
                sendDataSize -  送信予約するデータのサイズ。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetDCFData(WMCallbackFunc callback,
                        const u8 *destAdr, const u16 *sendData, u16 sendDataSize);

/*---------------------------------------------------------------------------*
  Name:         WM_EndDCF

  Description:  インフラストラクチャモードでの通信を停止する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_EndDCF(WMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         WM_SetWEPKey

  Description:  暗号機能、暗号キーを設定する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                wepmode     -   0: 暗号機能なし
                                1: RC4(40bit)暗号モード
                                2: RC4(104bit)暗号モード
                                3: RC4(128bit)暗号モード
                wepkey      -   暗号キーデータ( 80バイト )へのポインタ。
                                キーデータは4つのデータで構成され、それぞれ20バイト。
                                各20バイトのうち、
                                 40ビットモードでは  5 バイト
                                104ビットモードでは 13 バイト
                                128ビットモードでは 16 バイト
                                のデータが使用される。
                                また、このデータの実体は強制的にキャッシュストアされる。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetWEPKey(WMCallbackFunc callback, u16 wepmode, const u16 *wepkey);

/*---------------------------------------------------------------------------*
  Name:         WM_SetWEPKeyEx

  Description:  暗号機能、暗号キー、暗号キーIDを設定する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                wepmode     -   0: 暗号機能なし
                                1: RC4(40bit)暗号モード
                                2: RC4(104bit)暗号モード
                                3: RC4(128bit)暗号モード
                wepkeyid    -   4つ指定したwepkeyのどれを使用するかを選択します。
                                0～3で指定します。
                wepkey      -   暗号キーデータ( 80バイト )へのポインタ。
                                キーデータは4つのデータで構成され、それぞれ20バイト。
                                各20バイトのうち、
                                 40ビットモードでは  5 バイト
                                104ビットモードでは 13 バイト
                                128ビットモードでは 16 バイト
                                のデータが使用される。
                                また、このデータの実体は強制的にキャッシュストアされる。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetWEPKeyEx(WMCallbackFunc callback, u16 wepmode, u16 wepkeyid, const u8 *wepkey);

/*---------------------------------------------------------------------------*
  Name:         WM_ReadMPData

  Description:  受信したデータ全体から指定子機のデータだけを抽出する。同期関数。

  Arguments:    header      -   受信データ全体を示すポインタ。
                aid         -   データを抽出する子機のAID。

  Returns:      WMMpRecvData* - 該当子機からの受信データへのポインタを返す。
                                抽出に失敗した場合、NULLを返す。
 *---------------------------------------------------------------------------*/
WMMpRecvData *WM_ReadMPData(const WMMpRecvHeader *header, u16 aid);

/*---------------------------------------------------------------------------*
  Name:         WM_StartDataSharing

  Description:  データシェアリング機能を有効にする。同期関数。
                機能を有効にした後MP通信を行うことで、MP通信に付随して
                データシェアリング通信が行われる。

  Arguments:    dsInfo      -   WMDataSharingInfo 構造体。
                port        -   使用する port 番号。
                aidBitmap   -   データシェアを行う相手の AID。
                dataLength  -   共有メモリのデータ長。 (偶数バイトを指定すること)
                doubleMode  -   毎フレーム WM_StepDataSharing する場合は TRUE を指定

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartDataSharing(WMDataSharingInfo *dsInfo,
                              u16 port, u16 aidBitmap, u16 dataLength, BOOL doubleMode);

/*---------------------------------------------------------------------------*
  Name:         WM_EndDataSharing

  Description:  データシェアリング機能を無効にする。同期関数。

  Arguments:    dsInfo      -   WMDataSharingInfo 構造体。

  Returns:      WMErrCode   -   処理結果を返す。
 *---------------------------------------------------------------------------*/
WMErrCode WM_EndDataSharing(WMDataSharingInfo *dsInfo);

/*---------------------------------------------------------------------------*
  Name:         WM_StepDataSharing

  Description:  データシェアリングの同期を一つ進める。同期関数。

  Arguments:    dsInfo      -   WMDataSharingInfo 構造体。
                sendData    -   共有したい送信データ。
                receiveData -   受信した共有データ。

  Returns:      WMErrCode   -   処理結果を返す。
                         *_SUCCESS      : 共有データ送受信成功
                         *_NO_DATASET   : まだ共有データが来ていない
 *---------------------------------------------------------------------------*/
WMErrCode WM_StepDataSharing(WMDataSharingInfo *dsInfo,
                             const u16 *sendData, WMDataSet *receiveData);

/*---------------------------------------------------------------------------*
  Name:         WM_GetSharedDataAddress

  Description:  データシェアリングの受信データ中の特定の AID のアドレスを得る。
                同期関数。

  Arguments:    dsInfo      -   WMDataSharingInfo 構造体。
                receiveData -   受信した共有データ。
                aid         -   AID。

  Returns:      u16*        -   受信データのアドレス。存在しなければ NULL を返す。
 *---------------------------------------------------------------------------*/
u16    *WM_GetSharedDataAddress(WMDataSharingInfo *dsInfo, WMDataSet *receiveData, u16 aid);

/*---------------------------------------------------------------------------*
  Name:         WM_StartKeySharing

  Description:  キーシェアリング機能を有効にする。
                機能を有効にした後MP通信を行うことで、MP通信に付随して
                キーシェアリング通信が行われる。

  Arguments:    buf         -   キー情報を格納するバッファへのポインタ。
                                実体は WMDataSharingInfo 構造体へのポインタ。
                port        -   使用する port 番号

  Returns:      WMErrCode   -   処理結果を返す。
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartKeySharing(WMKeySetBuf *buf, u16 port);

/*---------------------------------------------------------------------------*
  Name:         WM_EndKeySharing

  Description:  キーシェアリング機能を無効にする。

  Arguments:    buf         -   キー情報を格納するバッファへのポインタ。
                                実体は WMDataSharingInfo 構造体へのポインタ。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_EndKeySharing(WMKeySetBuf *buf);

/*---------------------------------------------------------------------------*
  Name:         WM_GetKeySet

  Description:  キーシェアリングされたキーセットデータをひとつ読み出す。

  Arguments:    buf         -   キー情報を格納するバッファへのポインタ。
                                実体は WMDataSharingInfo 構造体へのポインタ。
                keySet      -   キーセットを読み出すバッファへのポインタ。
                                WM_StartKeySharingにて与えたバッファとは
                                別のバッファを指定する。

  Returns:      MWErrCode   -   処理結果を返す。
 *---------------------------------------------------------------------------*/
WMErrCode WM_GetKeySet(WMKeySetBuf *buf, WMKeySet *keySet);

/*---------------------------------------------------------------------------*
  Name:         WM_SetGameInfo

  Description:  ゲーム情報を設定する。WM_SetParentParameterにより初期値はセット
                されるので、これを変更する場合に用いる。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                userGameInfo     - ユーザーゲーム情報へのポインタ。
                userGameInfoSize - ユーザーゲーム情報のサイズ。
                ggid        -   ゲームグループID
                tgid        -   テンポラリグループID
                attr        -   フラグ群。以下のフラグの論理和を設定。
                                    WM_ATTR_FLAG_ENTRY - エントリー許可
                                    WM_ATTR_FLAG_MB    - マルチブート受け付け
                                    WM_ATTR_FLAG_KS    - キーシェアリング
                                    WM_ATTR_FLAG_CS    - 連続転送モード
  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetGameInfo(WMCallbackFunc callback,
                         const u16 *userGameInfo, u16 userGameInfoSize,
                         u32 ggid, u16 tgid, u8 attr);

/*---------------------------------------------------------------------------*
  Name:         WM_SetBeaconIndication

  Description:  ビーコン送受信indicateの有効/無効を切り替える。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                flag        -   0: 無効
                                1: 有効

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetBeaconIndication(WMCallbackFunc callback, u16 flag);

#ifdef  WM_ENABLE_TESTMODE
/*---------------------------------------------------------------------------*
  Name:         WM_StartTestMode

  Description:  テストモードでの通信を開始する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                signal      -   0: 無変調(data=0)
                                1: 無変調(data=1)
                                2: PN15段,
                                3: 01パターン(スクランブルあり)
                                4: 01パターン(スクランブルなし)
                rate        -   1: 1Mbps
                                2: 2Mbps
                channel     -   データを送信するチャンネルを指定(1～14)。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartTestMode(WMCallbackFunc callback, u16 signal, u16 rate, u16 channel);

/*---------------------------------------------------------------------------*
  Name:         WM_StopTestMode

  Description:  テストモードでの通信を停止する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_StopTestMode(WMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         WM_StartTestRxMode

  Description:  テストモードでの受信を開始する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                channel     -   データを受信するチャンネルを指定(1～14)。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartTestRxMode(WMCallbackFunc callback, u16 channel);

/*---------------------------------------------------------------------------*
  Name:         WM_StopTestRxMode

  Description:  テストモードでの受信を停止する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_StopTestRxMode(WMCallbackFunc callback);

#endif

/*---------------------------------------------------------------------------*
  Name:         WM_SetLifeTime

  Description:  ライフタイムを設定する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                tableNumber -   ライフタイムを設定するCAMテーブル番号。
                                ( 0xFFFFですべてのテーブル )
                camLifeTime -   CAMのライフタイム。( 100ms単位：0xFFFFで無効 )
                frameLifeTime - 設定するフレームのライフタイムのビーコン間隔。
                                ( 100ms単位：0xFFFFで無効 )
                camLifeTime -   MP 通信のライフタイム。( 100ms単位：0xFFFFで無効 )

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetLifeTime(WMCallbackFunc callback,
                         u16 tableNumber, u16 camLifeTime, u16 frameLifeTime, u16 mpLifeTime);

/*---------------------------------------------------------------------------*
  Name:         WM_MeasureChannel

  Description:  チャンネルの使用状況を測定する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                ccaMode     -   CCA動作モード。
                                0: キャリアセンスのみ。ED閾値は無視。
                                1: ED閾値のみ有効。
                                2: キャリアセンスとED閾値の論理積。
                                3: キャリアセンスとED閾値の論理和。
                EDThreshold -   ED閾値。(0～61) -60dBm ～ -80dBm
                channel     -   調査するチャンネル。
                                ( 1回のMeasureChannelで1つのチャンネルのみ )
                measureTime -   調査する時間。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_MeasureChannel(WMCallbackFunc callback,
                            u16 ccaMode, u16 edThreshold, u16 channel, u16 measureTime);

/*---------------------------------------------------------------------------*
  Name:         WM_InitWirelessCounter

  Description:  WirelessCounterを初期化する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_InitWirelessCounter(WMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         WM_GetWirelessCounter

  Description:  Wireless NICの送受信フレーム数、送受信エラーフレーム数を取得する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_GetWirelessCounter(WMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         WM_GetAllowedChannel

  Description:  通信時に使用を許可されたチャンネルを取得する。同期関数。

  Arguments:    None.

  Returns:      u16 -   使用許可チャンネルのビットフィールドを返す。最下位ビットが
                        1チャンネル、最上位ビットが16チャンネルを示す。ビットが1の
                        チャンネルが使用許可、ビットが0のチャンネルは使用禁止。
                        通常は 1～13 チャンネルの内いくつかのビットが1になった値が
                        返される。0x0000が返された場合は使用を許可されたチャンネルが
                        存在しないため、無線機能そのものを使用禁止である。
                        また、未初期化時など関数に失敗した場合は0x8000が返される。
 *---------------------------------------------------------------------------*/
u16     WM_GetAllowedChannel(void);

#ifdef  WM_PRECALC_ALLOWEDCHANNEL
/*---------------------------------------------------------------------------*
  Name:         WM_IsExistAllowedChannel

  Description:  通信時に使用を許可されたチャンネルが存在するかどうか確認する。
                WMライブラリを未初期化であっても正常に判定可能。

  Arguments:    None.

  Returns:      BOOL    -   使用許可されたチャンネルが存在する場合にTRUEを、
                            存在しない場合はFALSEを返す。
 *---------------------------------------------------------------------------*/
BOOL    WM_IsExistAllowedChannel(void);
#endif

/*---------------------------------------------------------------------------*
  Name:         WM_SetEntry

  Description:  親機として、子機からの接続受け付け可否を切り替える。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                enabled     -   エントリー許可/不許可フラグ。TRUE:許可、FALSE:不許可。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetEntry(WMCallbackFunc callback, BOOL enabled);

/*---------------------------------------------------------------------------*
  Name:         WM_GetLinkLevel

  Description:  通信時のリンク強度を取得する。同期関数。

  Arguments:    None.

  Returns:      WMLinkLevel -   4段階に評価したリンク強度を返す。
 *---------------------------------------------------------------------------*/
WMLinkLevel WM_GetLinkLevel(void);


/*---------------------------------------------------------------------------*
  Name:         WMi_SetBeaconPeriod

  Description:  Beaconの間隔を変更する

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                beaconPeriod -  Beacon間隔(10～1000 TU(1024μs))

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WMi_SetBeaconPeriod(WMCallbackFunc callback, u16 beaconPeriod);
/* 2004/10/18現在、通信中にビーコン間隔を変更することによる問題が発覚して
   いるため、一旦使用禁止関数とする為に非公開とします。 */


/*---------------------------------------------------------------------------*
  Name:         WM_GetDispersionBeaconPeriod

  Description:  親機として活動する際に設定すべきビーコン間隔値を取得する。
                同期関数。

  Arguments:    None.

  Returns:      u16 -   設定すべきビーコン間隔値(ms)。
 *---------------------------------------------------------------------------*/
u16     WM_GetDispersionBeaconPeriod(void);

/*---------------------------------------------------------------------------*
  Name:         WM_GetDispersionScanPeriod

  Description:  子機として親機を探索する際に設定すべき探索限度時間を取得する。
                同期関数。

  Arguments:    None.

  Returns:      u16 -   設定すべき探索限度時間(ms)。
 *---------------------------------------------------------------------------*/
u16     WM_GetDispersionScanPeriod(void);

/*---------------------------------------------------------------------------*
  Name:         WM_GetOtherElements

  Description:  ビーコン内の拡張エレメントを取得する。
                同期関数。

  Arguments:    bssDesc - 親機情報構造体。
                          WM_StartScan(Ex)にて取得した構造体を指定する。

  Returns:      WMOtherElements - 拡張エレメント構造体。
 *---------------------------------------------------------------------------*/
WMOtherElements WM_GetOtherElements(WMBssDesc *bssDesc);

/*---------------------------------------------------------------------------*
  Name:         WM_GetNextTgid

  Description:  自動生成された一意な TGID 値を取得する。
                同期関数。
                この関数は内部で RTC_Init() を呼び出す。

  Arguments:    None.

  Returns:      初回呼び出し時は RTC をもとに生成された TGIDを、
                次回以降は前回の返り値に 1 だけ加算した値を返す。
 *---------------------------------------------------------------------------*/
u16     WM_GetNextTgid(void);

/*---------------------------------------------------------------------------*
  Name:         WM_SetPowerSaveMode

  Description:  PowerSaveModeを変更する

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                powerSave   -   省電力モードを使用する場合はTRUE、しない場合はFALSE。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetPowerSaveMode(WMCallbackFunc callback, BOOL powerSave);

/*---------------------------------------------------------------------------*
  Name:         WMi_IsMP

  Description:  現在の MP 通信状態を取得する。

  Arguments:    None.

  Returns:      MP 通信状態であれば TRUE
 *---------------------------------------------------------------------------*/
BOOL    WMi_IsMP(void);

/*---------------------------------------------------------------------------*
  Name:         WM_GetAID

  Description:  現在の AID を取得する。
                状態が PARENT, MP_PARENT, CHILD, MP_CHILD のいずれかの時のみ
                有効な値を返す。

  Arguments:    None.

  Returns:      AID
 *---------------------------------------------------------------------------*/
u16     WM_GetAID(void);

/*---------------------------------------------------------------------------*
  Name:         WM_GetConnectedAIDs

  Description:  現在の接続相手をビットマップの形で取得する。
                状態が PARENT, MP_PARENT, CHILD, MP_CHILD のいずれかの時のみ
                有効な値を返す。
                子機の場合は親機と接続中は 0x0001 を返す。

  Arguments:    None.

  Returns:      接続している相手の AID のビットマップ
 *---------------------------------------------------------------------------*/
u16     WM_GetConnectedAIDs(void);

/*---------------------------------------------------------------------------*
  Name:         WMi_GetMPReadyAIDs

  Description:  現在の接続相手のうち、MP を受信できる相手の AID の一覧を
                ビットマップの形で取得する。
                状態が PARENT, MP_PARENT, CHILD, MP_CHILD のいずれかの時のみ
                有効な値を返す。
                子機の場合は親機と接続中は 0x0001 を返す。

  Arguments:    None.

  Returns:      MP を開始している相手の AID のビットマップ
 *---------------------------------------------------------------------------*/
u16     WMi_GetMPReadyAIDs(void);


// for debug
void    WMi_DebugPrintSendQueue(WMPortSendQueue *queue);
void    WMi_DebugPrintAllSendQueue(void);
const WMStatus *WMi_GetStatusAddress(void);
BOOL    WMi_CheckMpPacketTimeRequired(u16 parentSize, u16 childSize, u8 childs);


/*===========================================================================*/

/*---------------------------------------------------------------------------*
  Name:         WM_SIZE_MP_PARENT_RECEIVE_BUFFER

  Description:  親機の受信バッファのサイズを計算する。

  Arguments:    childMaxSize - 子機から受信したいデータの最大バイト数
                maxEntry     - 接続したい子機数
                ksFlag       - Key Sharing を行うかどうかのブール値

  Returns:      int      - WM_StartMP に渡さないといけない受信バッファのサイズ
 *---------------------------------------------------------------------------*/
#define WM_SIZE_MP_PARENT_RECEIVE_BUFFER( childMaxSize, maxEntry, ksFlag ) \
            (((sizeof( WMMpRecvHeader ) - sizeof( WMMpRecvData ) + \
            ( ( sizeof( WMMpRecvData ) + (childMaxSize) + WM_HEADER_CHILD_MAX_SIZE - 2 + 2/*MACBUG*/ + ((ksFlag) ? WM_SIZE_KS_CHILD_DATA + WM_SIZE_MP_CHILD_PADDING : 0) ) * (maxEntry) ) \
             + 31) & ~0x1f) * 2)

/*---------------------------------------------------------------------------*
  Name:         WM_SIZE_MP_CHILD_RECEIVE_BUFFER

  Description:  子機の受信バッファのサイズを計算する。

  Arguments:    parentMaxSize - 親機から受信したいデータの最大バイト数
                ksFlag       - Key Sharing を行うかどうかのブール値

  Returns:      int      - WM_StartMP に渡さないといけない受信バッファのサイズ
 *---------------------------------------------------------------------------*/
#define WM_SIZE_MP_CHILD_RECEIVE_BUFFER( parentMaxSize, ksFlag ) \
            (((sizeof( WMMpRecvBuf ) + (parentMaxSize) + WM_HEADER_PARENT_MAX_SIZE - 4 + ((ksFlag) ? WM_SIZE_KS_PARENT_DATA + WM_SIZE_MP_PARENT_PADDING : 0) + 31) & ~0x1f) * 2)

/*---------------------------------------------------------------------------*
  Name:         WM_SIZE_MP_PARENT_SEND_BUFFER

  Description:  親機の送信バッファのサイズを計算する。

  Arguments:    parentMaxSize - 送信したいデータの最大バイト数
                ksFlag       - Key Sharing を行うかどうかのブール値

  Returns:      int      - WM_StartMP に渡さないといけない送信バッファのサイズ
 *---------------------------------------------------------------------------*/
#define WM_SIZE_MP_PARENT_SEND_BUFFER( parentMaxSize, ksFlag ) \
            (((parentMaxSize) + WM_HEADER_PARENT_MAX_SIZE + ((ksFlag) ? WM_SIZE_KS_PARENT_DATA + WM_SIZE_MP_PARENT_PADDING : 0) + 31) & ~0x1f)

/*---------------------------------------------------------------------------*
  Name:         WM_SIZE_MP_CHILD_SEND_BUFFER

  Description:  子機の送信バッファのサイズを計算する。

  Arguments:    childMaxSize - 送信したいデータの最大バイト数
                ksFlag       - Key Sharing を行うかどうかのブール値

  Returns:      int      - WM_StartMP に渡さないといけない送信バッファのサイズ
 *---------------------------------------------------------------------------*/
#define WM_SIZE_MP_CHILD_SEND_BUFFER( childMaxSize, ksFlag ) \
            (((childMaxSize) + WM_HEADER_CHILD_MAX_SIZE + ((ksFlag) ? WM_SIZE_KS_CHILD_DATA + WM_SIZE_MP_CHILD_PADDING : 0) + 31) & ~0x1f)

/*---------------------------------------------------------------------------*
  Name:         WM_SIZE_DCF_CHILD_SEND_BUFFER

  Description:  DCF子機の送信バッファのサイズを計算する。

  Arguments:    childMaxSize - 送信したいデータの最大バイト数

  Returns:      int      - WM_StartDCF に渡さないといけない送信バッファのサイズ
 *---------------------------------------------------------------------------*/
#define WM_SIZE_DCF_CHILD_SEND_BUFFER( childMaxSize ) \
            (((childMaxSize) + WM_SIZE_MADATA_HEADER + 31) & ~0x1f)

#define WM_SIZE_CHILD_SEND_BUFFER WM_SIZE_MP_CHILD_SEND_BUFFER
#define WM_SIZE_CHILD_RECEIVE_BUFFER WM_SIZE_MP_CHILD_RECEIVE_BUFFER
#define WM_SIZE_PARENT_SEND_BUFFER WM_SIZE_MP_PARENT_SEND_BUFFER
#define WM_SIZE_PARENT_RECEIVE_BUFFER WM_SIZE_MP_PARENT_RECEIVE_BUFFER


/*===========================================================================*/

/*---------------------------------------------------------------------------*
  Name:         WM_ConvGgid32to16

  Description:  ゲームグループIDを"32ビットの形"から"16ビット×2の形"に変換する。

  Arguments:    src     -   32ビットで表された変換元GGIDへのポインタ。
                dest    -   変換後のGGIDを格納するバッファへのポインタ。
                            このアドレスを先頭に4バイトデータが書き込まれます。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void WM_ConvGgid32to16(u32 *src, u16 *dst)
{
    dst[0] = (u16)(*src & 0x0000ffff);
    dst[1] = (u16)(*src >> 16);
}

/*---------------------------------------------------------------------------*
  Name:         WM_ConvGgid16to32

  Description:  ゲームグループIDを"16ビット×2の形"から"32ビットの形"に変換する。

  Arguments:    src     -   16ビット配列で表された変換元GGIDへのポインタ。
                dest    -   変換後のGGIDを格納するバッファへのポインタ。
                            このアドレスを先頭に4バイトデータが書き込まれます。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void WM_ConvGgid16to32(u16 *src, u32 *dst)
{
    *dst = ((u32)src[1] << 16) | src[0];
}

/*---------------------------------------------------------------------------*
  Name:         WM_IsBssidEqual

  Description:  二つのBSSIDが等しいかどうかをチェックする。

  Arguments:    idp1     -   比較するBSSIDへのポインタ。
                idp2     -   比較するBSSIDへのポインタ。

  Returns:      等しければ TRUE、
                異なっていれば FALSE。
 *---------------------------------------------------------------------------*/
static inline BOOL WM_IsBssidEqual(const u8 *idp1, const u8 *idp2)
{
    return ((*idp1 == *idp2) &&
            (*(idp1 + 1) == *(idp2 + 1)) &&
            (*(idp1 + 2) == *(idp2 + 2)) &&
            (*(idp1 + 3) == *(idp2 + 3)) &&
            (*(idp1 + 4) == *(idp2 + 4)) && (*(idp1 + 5) == *(idp2 + 5)));
}


/*---------------------------------------------------------------------------*
  Name:         WM_IsBssidEqual16

  Description:  二つのBSSIDが等しいかどうかを2バイト単位でチェックする。

  Arguments:    idp1     -   比較するBSSIDへのポインタ。2バイトアラインが必要。
                idp2     -   比較するBSSIDへのポインタ。2バイトアラインが必要。

  Returns:      等しければ TRUE、
                異なっていれば FALSE。
 *---------------------------------------------------------------------------*/
static inline BOOL WM_IsBssidEqual16(const u8 *idp1, const u8 *idp2)
{
    SDK_ALIGN2_ASSERT(idp1);
    SDK_ALIGN2_ASSERT(idp2);

    return ((*(u16 *)idp1 == *(u16 *)idp2) &&
            (*(u16 *)(idp1 + 2) == *(u16 *)(idp2 + 2)) &&
            (*(u16 *)(idp1 + 4) == *(u16 *)(idp2 + 4)));
}

/*---------------------------------------------------------------------------*
  Name:         WM_CopyBssid

  Description:  BSSIDをコピーする。

  Arguments:    src     -   コピー元BSSIDへのポインタ。
                dest    -   コピー先BSSIDへのポインタ。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void WM_CopyBssid(const u8 *src, u8 *dst)
{
    *dst = *src;
    *(dst + 1) = *(src + 1);
    *(dst + 2) = *(src + 2);
    *(dst + 3) = *(src + 3);
    *(dst + 4) = *(src + 4);
    *(dst + 5) = *(src + 5);
}

/*---------------------------------------------------------------------------*
  Name:         WM_CopyBssid16

  Description:  BSSIDを2バイト単位でコピーする。

  Arguments:    src     -   コピー元BSSIDへのポインタ。2バイトアラインが必要。
                dest    -   コピー先BSSIDへのポインタ。2バイトアラインが必要。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void WM_CopyBssid16(const u8 *src, u8 *dst)
{
    SDK_ALIGN2_ASSERT(src);
    SDK_ALIGN2_ASSERT(dst);

    *(u16 *)dst = *(u16 *)src;
    *(u16 *)(dst + 2) = *(u16 *)(src + 2);
    *(u16 *)(dst + 4) = *(u16 *)(src + 4);
}

/*---------------------------------------------------------------------------*
  Name:         WM_IsValidGameInfo

  Description:  GameInfo が対応しているものであるかを調べる

  Arguments:    gameInfo - WMGameInfo 構造体へのポインタ
                gameInfoLength - gameInfo の指す先のデータの長さ

  Returns:      対応している GameInfo であった場合は TRUE.
 *---------------------------------------------------------------------------*/
static inline BOOL WM_IsValidGameInfo(const WMGameInfo *gameInfo, u16 gameInfoLength)
{
    return (gameInfoLength >= WM_GAMEINFO_LENGTH_MIN
            && gameInfo->magicNumber == WM_GAMEINFO_MAGIC_NUMBER);
}

/*---------------------------------------------------------------------------*
  Name:         WM_IsValidGameBeacon

  Description:  ビーコンがDSワイヤレスプレイ用の親機からのものであるかを調べる

  Arguments:    bssDesc - ビーコン情報が入っている WMBssDesc 構造体へのポインタ

  Returns:      DSワイヤレスプレイ用親機からのビーコンであった場合は TRUE.
 *---------------------------------------------------------------------------*/
static inline BOOL WM_IsValidGameBeacon(const WMBssDesc *bssDesc)
{
    return WM_IsValidGameInfo(&bssDesc->gameInfo, bssDesc->gameInfoLength);
}

/*---------------------------------------------------------------------------*
  Name:         WMi_IsDisconnectReasonFromMyself

  Description:  WM_STATECODE_DISCONNECTED の reason 値が自分からの切断かを
                判定する

  Arguments:    reason - WMStartParentCallback か WMPortRecvCallback の reason メンバ

  Returns:      自分で WM の関数を呼び出して切断した場合は TRUE,
                相手から切られた、または通信エラーで切れた場合は FALSE
 *---------------------------------------------------------------------------*/
static inline BOOL WMi_IsDisconnectReasonFromMyself(u16 reason)
{
    return (reason >= WM_DISCONNECT_REASON_FROM_MYSELF);
}


/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C" */
#endif

#endif /* NITRO_WM_ARM9_WM_API_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
