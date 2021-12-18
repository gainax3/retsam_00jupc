/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - libraries
  File:     wm.c

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wm.c,v $
  Revision 1.92  2005/02/28 05:26:35  yosizaki
  do-indent.

  Revision 1.91  2004/09/13 00:20:35  terui
  別ファイル群への分割完了に伴い、更新履歴のみを残してコードを削除。

  Revision 1.90  2004/09/10 12:05:40  seiki_masashi
  キーシェアリングの送信バイト数をライブラリ内で隠蔽するように変更

  Revision 1.89  2004/09/10 04:55:36  seiki_masashi
  WM_GetReceiveBufferSize を正確な値に変更

  Revision 1.88  2004/09/09 05:54:00  seiki_masashi
  SetMPData の callback に引数を渡せるように変更

  Revision 1.87  2004/09/09 01:56:11  terui
  wm.cファイルの分割着手に伴い一部関数を別ファイルへ移動。
  別ファイルから参照する共通関数をいくつか追加。
  static変数、static関数について、命名規約に基づいた名称にRename。
  WM_Init、WM_Finish関数の追加。

  Revision 1.86  2004/09/03 07:15:24  terui
  デバッグ用にCheckParentParameter関数を追加。

  Revision 1.85  2004/09/03 05:05:30  seiki_masashi
  SetMPDataToPort の NO_CHILD 判定条件を child_bitmap==0 に変更

  Revision 1.84  2004/09/03 04:46:49  seiki_masashi
  WMStatus.mp_readyBitmap を利用するように変更

  Revision 1.83  2004/09/02 09:27:00  seiki_masashi
  fix typo.

  Revision 1.82  2004/09/02 09:24:42  seiki_masashi
  MPEND_IND より port 受信コールバックが早くなったことに伴い、キャッシュ処理の追加(再)

  Revision 1.81  2004/09/02 09:15:13  terui
  WMi_GetStatusAddress関数を追加。

  Revision 1.78  2004/08/30 01:59:46  seiki_masashi
  WM_STATECODE_CHILD_CONNECTED を WM_STATECODE_CONNECTED に統一

  Revision 1.77  2004/08/30 01:58:23  ooe
  small fix

  Revision 1.76  2004/08/30 00:58:51  terui
  リンク強度をARM7側で一元管理するように修正。

  Revision 1.75  2004/08/27 08:14:14  seiki_masashi
  small fix.

  Revision 1.74  2004/08/27 06:43:50  seiki_masashi
  WM_StartMPEx の引数に defaultRetryCount, fixFreqMode を追加

  Revision 1.73  2004/08/27 04:42:20  terui
  WM_Disconnectのパラメータチェックを修正。

  Revision 1.72  2004/08/27 04:25:37  terui
  WM_Disconnectにおいて、子機である場合のパラメータチェックを修正。

  Revision 1.71  2004/08/27 02:16:27  terui
  Only fix comment.

  Revision 1.70  2004/08/25 08:17:32  seiki_masashi
  WM_StatMPEx の追加

  Revision 1.69  2004/08/25 05:52:44  terui
  Rename parentRssi to lastRssi.

  Revision 1.68  2004/08/24 13:21:32  terui
  リンク強度取得関数を追加。

  Revision 1.67  2004/08/23 04:42:45  ooe
  WM_StartConnect()の仕様変更に対応。

  Revision 1.66  2004/08/21 06:56:50  ooe
  WMStartConnectReq型を使用するように変更

  Revision 1.65  2004/08/20 11:55:28  terui
  Childs -> Children

  Revision 1.64  2004/08/20 06:48:17  terui
  WM_DisconnectChildsを追加。

  Revision 1.63  2004/08/19 15:25:07  miya
  modified SetGameInfo function.

  Revision 1.62  2004/08/19 05:55:21  miya
  WM_SetGameInfo

  Revision 1.61  2004/08/19 02:53:48  terui
  WM_SetEntry関数を追加。

  Revision 1.60  2004/08/18 12:39:49  yosiokat
  WM_SetGameInfoのgameInfoサイズチェックを64からWM_SIZE_USER_GAMEINFOに修正。

  Revision 1.59  2004/08/18 06:45:09  terui
  WM_GetAllowedChannel関数を追加。

  Revision 1.58  2004/08/18 00:22:30  seiki_masashi
  WM_StartMP の引数の微調整

  Revision 1.57  2004/08/17 12:43:13  seiki_masashi
  WM_StartMP の引数に送信バッファを指定するように変更
  WM_ERRCODE_FRAME_GAP の廃止に伴う修正

  Revision 1.56  2004/08/16 06:00:21  seiki_masashi
  DataSharing の動的接続・切断対応の修正

  Revision 1.55  2004/08/12 14:22:37  seiki_masashi
  DataSharing の同期ずれへの対応

  Revision 1.54  2004/08/11 10:26:55  seiki_masashi
  受信バッファのキャッシュをARM7 に渡す前に書き出すように修正

  Revision 1.53  2004/08/11 10:03:03  seiki_masashi
  DataSharing 開始時の処理を修正

  Revision 1.52  2004/08/11 07:26:00  seiki_masashi
  DataSharing 開始時の処理を修正

  Revision 1.51  2004/08/11 04:26:13  seiki_masashi
  StartDataSharing の引数に doubleMode を追加
  DataSharing 開始時の処理の安定化

  Revision 1.50  2004/08/10 14:16:52  seiki_masashi
  子機からの Data は 2 つまでバッファリングするようにするのを一時取りやめ

  Revision 1.49  2004/08/10 12:08:12  seiki_masashi
  WmDataSharingReceiveData から WmDataSharingSendDataSet の分離
  子機からの Data は 2 つまでバッファリングするように

  Revision 1.48  2004/08/10 10:12:33  terui
  コメント追加、ソース整形のみ。

  Revision 1.47  2004/08/10 04:14:35  seiki_masashi
  port 受信通知の callback に CONNECT, DISCONNECT も通知するように変更
  DataSharing 時に動的に子機が落ちても動作を継続するように変更

  Revision 1.46  2004/08/10 01:40:34  seiki_masashi
  子機がいない状態で StepDataSharing を呼んでも正常動作するように修正

  Revision 1.45  2004/08/09 10:29:33  seiki_masashi
  DataSharing 時に aidBitmap が自分宛でない DataSet は受け取らないように変更

  Revision 1.44  2004/08/07 13:39:39  terui
  DmaCopy32->CpuCopyFast in WM_ReadStatus.

  Revision 1.43  2004/08/07 13:37:25  terui
  Undefine WM_USE_TEMP_STATUS_BUF switch

  Revision 1.42  2004/08/07 10:23:19  seiki_masashi
  受信バッファのキャッシュ無効化を MPEND.ind, MP.ind で自動的に行うように変更
  WMDataSharingInfo に DataSet の seq 番号を保存

  Revision 1.41  2004/08/07 08:43:16  seiki_masashi
  DataSharing の送信データを 32-byte align に

  Revision 1.40  2004/08/07 07:04:25  seiki_masashi
  WM_StepDataSharing の引数を変更

  Revision 1.39  2004/08/07 06:51:18  seiki_masashi
  7 からの受信データのキャッシュ処理を行う

  Revision 1.38  2004/08/07 05:16:20  seiki_masashi
  small fix

  Revision 1.37  2004/08/07 04:09:02  seiki_masashi
  DataSharing 時に receivedBitmap, aidBitmap も送信するように変更
  DataSharing の送信バッファの初期化忘れを修正

  Revision 1.36  2004/08/07 02:58:28  terui
  コールバック排他制御フラグををshared領域に移動
  WM_USE_BUF_REMAINスイッチを削除

  Revision 1.35  2004/08/07 00:19:40  seiki_masashi
  dataSharing の実装

  Revision 1.34  2004/08/05 10:01:59  seiki_masashi
  SetMPDataToPort() が tmptt を受けとらないように変更

  Revision 1.33  2004/08/05 08:25:54  seiki_masashi
  sendQueue の実装に伴う変更

  Revision 1.32  2004/08/04 07:12:39  seiki_masashi
  WMi_DebugPrintAllSendQueue の追加

  Revision 1.31  2004/08/04 06:20:50  ooe
  InitWirelessCounter及びGetWirelessCounterを追加

  Revision 1.30  2004/08/04 05:16:29  seiki_masashi
  small fix

  Revision 1.29  2004/08/04 01:45:09  seiki_masashi
  port 導入の前準備
  WM_SetMPDataToPort, WM_SetPortCallback の追加

  Revision 1.28  2004/08/03 12:25:08  ooe
  WLのTestModeコマンド変更に対応

  Revision 1.27  2004/08/02 06:29:47  terui
  WMbssDesc -> WMBssDescに統一
  WMstatus -> WMStatusに統一

  Revision 1.26  2004/08/02 02:52:51  terui
  構造体名を命名規約に基づき統一

  Revision 1.25  2004/07/30 05:20:17  ooe
  MeasureChannelコマンドを発行できるようにした

  Revision 1.24  2004/07/29 12:09:59  ooe
  WM_MeasureChannelを追加

  Revision 1.23  2004/07/29 07:37:59  miya
  add error message

  Revision 1.22  2004/07/29 06:10:44  miya
  bug fix

  Revision 1.21  2004/07/29 05:13:46  miya
  add comment

  Revision 1.20  2004/07/29 04:20:50  miya
  add SetLifeTime function

  Revision 1.19  2004/07/28 04:42:27  miya
  add comment

  Revision 1.18  2004/07/28 04:21:18  miya
  add functions about ggid & tgid

  Revision 1.17  2004/07/27 00:06:26  terui
  API呼び出し時のステートチェックを強化

  Revision 1.16  2004/07/23 14:43:48  terui
  WM_ReadStatus関数におけるDMAコピーサイズを変更。

  Revision 1.15  2004/07/20 01:52:52  terui
  small fix

  Revision 1.14  2004/07/20 01:16:20  terui
  関数説明コメントを大幅に追加。
  srcとしてのポインタ引数をconstに統一。
  メインメモリを介してARM7とデータやり取りする部分でのキャッシュ操作の徹底。
  不要な関数削除。
  WM_SetGameInfoの仕様をユーザーGameInfoのみ更新可能な仕様に変更。
  その他各所修正。

  Revision 1.13  2004/07/16 11:26:09  terui
  Add CheckStateEx.
  Update comments.

  Revision 1.12  2004/07/15 00:32:57  yasu
  fix a small bug

  Revision 1.11  2004/07/14 09:41:24  ooe
  StartScanの引数を変更

  Revision 1.10  2004/07/14 05:47:23  seiki_masashi
  fix a bug.

  Revision 1.9  2004/07/13 08:34:24  yada
  sorry, fix a little

  Revision 1.8  2004/07/13 08:32:06  yada
  let WM_sp_init() return WM_ERRCODE_WM_DISABLE when WMsp is not ready

  Revision 1.7  2004/07/12 10:37:26  terui
  Scan開始時のステートチェックを変更

  Revision 1.6  2004/07/12 01:31:25  ooe
  small fix

  Revision 1.5  2004/07/11 03:03:33  ooe
  WM_SetBeaconIndication関係の追加など

  Revision 1.4  2004/07/08 10:07:50  terui
  WM7改造開始に伴う変更

  Revision 1.15  2004/07/06 08:37:46  Ooe
  使用していない変数を削除

  Revision 1.14  2004/07/06 01:53:33  Ooe
  fifoFlagの初期化部分を修正

  Revision 1.13  2004/07/05 13:01:22  Ooe
  キャッシュ制御を修正

  Revision 1.12  2004/07/02 04:53:01  Miya
  キャッシュ制御の修正

  Revision 1.11  2004/06/30 03:08:53  ikedae
  fifoBuf　のアラインメント修正

  Revision 1.10  2004/06/25 09:18:27  Ooe
  Marionea-1.52.00に対応

  Revision 1.9  2004/06/25 06:18:06  Ooe
  WM_SetMPData()の引数追加 (pollbmp)

  Revision 1.8  2004/06/24 11:22:03  Ooe
  WM_SetGameInfo()のエラー処理を追加。

  Revision 1.7  2004/06/24 10:04:00  Ooe
  WM_SetGameInfo()を追加

  Revision 1.6  2004/06/22 09:28:06  Ikedae
  (none)

  Revision 1.5  2004/06/22 05:19:16  Ikedae
  WM_lib_Main削除関連の変更

  Revision 1.4  2004/06/19 05:56:29  Miya
  modified header file reference

  Revision 1.3  2004/06/19 04:05:10  miya
  ver. up Marionea-1.48.00

  Revision 1.2  2004/06/02 02:29:40  miya
  ver. up

  Revision 1.1  2004/05/26 08:55:07  terui
  無線組み込みに伴う追加

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
