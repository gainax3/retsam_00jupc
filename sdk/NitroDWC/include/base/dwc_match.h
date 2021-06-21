/*---------------------------------------------------------------------------*
  Project:  NitroDWC - base - dwc_match
  File:     dwc_match.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_match.h,v $
  Revision 1.62  2007/08/24 10:12:42  takayama
  NNThink内から呼び出されているDWCi_NNCompletedCallbackから
  NNFreeNegotiateListを呼び出していたバグを修正。

  Revision 1.61  2006/12/15 07:06:26  nakata
  ServerBrowserLimitUpdateのタイムアウト時間を延ばした
  そのままだと普通にタイムアウトにヒットする場合があるため

  Revision 1.60  2006/09/26 11:22:42  nakata
  ServerBrowserLimitUpdateのタイムアウト検出不可能な問題を修正

  Revision 1.59  2006/02/07 00:16:36  arakit
  サーバクライアントマッチメイクのサーバ締め切り関数DWC_StopGameServer()を作成した。

  Revision 1.58  2006/01/26 05:33:15  arakit
  サーバクライアントマッチメイクのサーバ締め切り関数DWC_StopGameServer()を作成した。

  Revision 1.57  2006/01/25 07:52:14  arakit
  ・マッチメイク完了同期中に親機が切断すると、子機がはまり状態になってしまう不具合を修正
  ・友達指定ピアマッチメイクにおいて、distantFriend = FALSE の時は絶対に指定の友達
  　以外とは接続しないように修正した。

  Revision 1.56  2006/01/23 04:50:43  arakit
  統一用語に合わせてコメントを修正

  Revision 1.55  2005/11/29 09:14:19  arakit
  ServerBrowserLimitUpdate()で取得する最大サーバ数を6に減らした。

  Revision 1.54  2005/11/04 11:37:00  arakit
  関数説明コメントを修正した。

  Revision 1.53  2005/11/02 02:49:32  arakit
  ファイルのインクルード関係を調整した。

  Revision 1.52  2005/10/28 09:09:58  arakit
  ・DWCi_GetCurrentTime()の削除に伴い、時間を全てOSTickで持つように変更した。
  ・[todo]のコメント部分を削除、もしくは訂正した。

  Revision 1.51  2005/10/05 08:28:50  arakit
  サーバクライアントマッチング時に、マッチング完了後、マッチングをブロックし、ブロックされた
  クライアントに、専用のエラーを返すマッチングオプションを追加した。

  Revision 1.50  2005/10/05 06:50:52  arakit
  友達指定マッチングで、バディ成立済みのバディがいなくても、条件次第では
  友達不足のエラーを出さないようにした。

  Revision 1.49  2005/10/04 09:02:14  arakit
  マッチング完了同期調整中にクローズ、電源OFFされても整合性がとれるよう変更した。

  Revision 1.48  2005/10/04 06:42:33  sasakit
  SBUpdateパラメータのチューニング

  Revision 1.47  2005/10/04 06:28:00  sasakit
  SBUpdateするタイミングの調整。

  Revision 1.46  2005/10/03 12:21:19  arakit
  サーバクライアント型マッチングのクライアント予約時に、サーバからの無応答タイムアウトが
  発生しても、一定回数リトライするようにした。

  Revision 1.45  2005/10/03 05:06:11  arakit
  QR2起動関数の名前をDWC_RegisterMatchingStatusに変更した。

  Revision 1.43  2005/10/01 14:25:45  arakit
  タイムアウト処理の不具合を修正した。

  Revision 1.42  2005/10/01 10:32:24  sasakit
  主なTimeoutをmsecオーダーに修正。

  Revision 1.41  2005/10/01 07:16:09  arakit
  サーバクライアント型マッチング時のキャンセル同期調整処理を作成した。

  Revision 1.40  2005/09/30 02:50:07  arakit
  スイッチ定数DWC_CLOSE_CONNECTION_NEWを廃止した。

  Revision 1.39  2005/09/28 12:42:20  arakit
  ・１回のマッチングで５回NATネゴシエーションに失敗したらエラーになるようにした。
  ・NATネゴシエーションのクッキーを、上位16bitをランダム値、下位16bitをプロファイルIDの
  　下位16bitに変更した。

  Revision 1.38  2005/09/27 13:02:15  arakit
  子機同士の接続完了待ち時間は、最低でもNNリトライ完了分は待つように延ばした。

  Revision 1.37  2005/09/27 05:14:25  arakit
  マッチング完了同期通信のデータサイズを定数にした。

  Revision 1.36  2005/09/26 15:16:09  sasakit
  NAT Negotiationのリトライ回数を減らした。

  Revision 1.35  2005/09/26 11:06:26  arakit
  QR2キーのnumplayersが253などの異常な値になってしまう不具合を修正した。

  Revision 1.34  2005/09/26 05:06:08  arakit
  ・SBアップデート用のaddFilter文字列を、ライブラリ内部にコピーして持つように変更した。
  ・DWC_AddMatchKey*()のkey名の文字列も、ライブラリ内部にコピーして持つように変更した。

  Revision 1.33  2005/09/24 12:56:30  arakit
  サーバクライアント型マッチング時に、マッチング完了したホストのみを有効なAIDとして
  扱うよう、AIDやコネクション数へのアクセス関数を修正した。

  Revision 1.32  2005/09/23 04:30:09  arakit
  応答無しホストのタイムアウト処理を修正、追加した。

  Revision 1.31  2005/09/21 07:13:14  arakit
  gt2Connect待ちのタイムアウトを設定した。

  Revision 1.30  2005/09/21 06:06:45  arakit
  gt2Connectのリトライ回数オーバ時にマッチングをやり直すようにした。

  Revision 1.29  2005/09/21 05:21:26  arakit
  ・キャンセルを含めたクローズ処理のシステムを大きく変更した。
  ・コマンド送信のリトライ、タイムアウト処理を作成した。
  ・NATネゴシエーション失敗時のマッチングやり直し処理を追加した。
  ・gt2のソフトクローズを全てハードクローズに置き換えた。
  　それに伴いDWCi_CloseConnectionsAsync()を使わないようにした。
  （注）サーバクライアント型マッチングは正常動作しない

  Revision 1.28  2005/09/19 13:51:24  sasakit
  RESV_WAIT受信時の再送処理を変更した。

  Revision 1.27  2005/09/15 13:02:06  arakit
  友達指定マッチングの古いクライアントQR2データを見つけてメッセージを送ってしまうのを回避した。

  Revision 1.26  2005/09/15 09:44:32  arakit
  ・SBメッセージコマンド再送処理を一部導入した。
  ・サーバに上がった自分のQR2データをより正確に判定するようにした。

  Revision 1.25  2005/09/15 06:07:41  arakit
  指定人数以下マッチング完了オプションの状態を取得できる関数
  DWC_GetMOMinCompState(u64* time)を追加した。

  Revision 1.24  2005/09/14 10:07:49  arakit
  指定人数以下でのマッチング完了オプション設定機能を追加した。

  Revision 1.23  2005/09/13 11:11:30  arakit
  プログラム整理

  Revision 1.22  2005/09/10 09:02:57  arakit
  デバッグ用のマッチング内部状態表示関数DWC_GetMatchingState()を追加した。

  Revision 1.21  2005/09/09 09:54:09  arakit
  バージョン違いのライブラリ同士でマッチングしないように変更した。

  Revision 1.20  2005/09/08 14:41:26  arakit
  ・友達指定マッチングで、友達が1人もマッチング開始していない場合の、予約コマンド
  　再送間隔を短くした。
  ・遅れて受信した予約コマンドに反応してしまう不具合を修正した。

  Revision 1.19  2005/09/08 08:50:24  arakit
  ・ログイン時にgt2SocketとQR2を起動するように変更
  ・aidが重複して発行される不具合を修正

  Revision 1.18  2005/09/08 02:48:34  arakit
  接続済み子機を持つサーバの、他サーバ検索（もしくは予約）開始を遅らせ、
  できるだけ他クライアントからの接続を待つように変更。

  Revision 1.17  2005/09/07 12:43:34  arakit
  友達無指定マッチングの仕様を変更し、マッチングを高速化した。

  Revision 1.16  2005/09/05 11:44:29  arakit
  ・DWCConnectionClosedCallbackの引数isServerは、自分がサーバクライアント型
  　マッチングのクライアントで、サーバがクローズした時のみTRUEを返すように
  　していたが、紛らわしいので、自分がサーバで自分がクローズしてもTRUEを
  　返すように変更した。
  ・サーバクライアント型マッチングで、クライアントがいなくなった時の
  　サーバの処理に不具合があったので、修正した。
  ・サーバクライアント型で、クローズ、キャンセル後にqr2_shutdown()が呼ばれて
  　いなかったのを修正した。
  ・最後のマッチングタイプ取得関数DWC_GetLastMatchingType()を追加した。

  Revision 1.15  2005/09/03 13:03:53  arakit
  GameSpyプロファイルIDを引数にとるようになっていたコールバック、
  DWCMatchedSCCallback, DWCNewClientCallback, DWCConnectionClosedCallback
  について、友達リストインデックスを返すように変更した。

  Revision 1.14  2005/09/01 12:15:00  arakit
  マッチング完了時の同期調整処理を追加した。

  Revision 1.13  2005/08/31 02:10:07  arakit
  ・友達指定マッチングで、友達の友達を許可するかどうかを選択できるようにした。
  ・友達無指定マッチングの場合、クライアント状態もランダム時間維持するようにした。

  Revision 1.12  2005/08/29 12:28:15  arakit
  ・友達無指定マッチングで、サーバになったホストが、タイムアウトでまたクライアントに戻り、
  サーバの検索をやり直すよう変更した。
  ・友達無指定マッチング時に、既に他のサーバに接続したクライアントにNN予約を
  送信してしまう不具合を修正した。

  Revision 1.11  2005/08/29 06:37:17  arakit
  ・友達指定マッチングで、SBのタイムアウトが発生する不具合を修正した。
  ・友達指定マッチングで接続人数指定が反映されていなかったのを修正した。
  ・サーバクライアント型マッチングで、最大接続人数、現在接続人数をgpStatusの
  locationStringにセットするようにし、その値を参照するための関数を用意した。

  Revision 1.10  2005/08/26 08:15:23  arakit
  ・ASSERTとモジュール制御変数のチェックを整理した。
  ・外部関数DWC_ShutdownFriendsMatch()の引数をとらないように変更した。
  また、同関数から各モジュールの制御変数をNULLクリアするようにした。
  ・エラー処理関数を変更し、エラーセット、エラーチェックを必要な箇所に入れた。

  Revision 1.9  2005/08/24 07:12:07  arakit
  qr2コールバック内からqr2_shutdown()を呼ばないよう修正した。

  Revision 1.8  2005/08/23 13:54:52  arakit
  ・GPバディメッセージ、及びSBメッセージによる通信コマンド再送間隔を、３０ゲームフレーム
  から、６０ゲームフレームに変更した。
  ・マッチング完了・キャンセル後、再びマッチングを行う時にQR2エラーが発生するバグを修正した。

  Revision 1.7  2005/08/20 10:28:07  sasakit
  行末に\来ている行の対策。

  Revision 1.6  2005/08/20 07:01:20  sasakit
  ヘッダインクルードガードの書式を統一した。
  bm/dwc_init.h -> bm/dwc_bm_init.hに変更
  ヘッダにCopyrightをつけた。
  構造体の名前空間をできるだけルールにちかづけた。
  util_wifiidtool.hとdwc_backup.hの関係を若干修正。


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_MATCH_H_
#define DWC_MATCH_H_

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------
// サーバクライアントマッチメイク以外ではマッチメイクが終わったらQR2を終了する仕様
#define DWC_QR2_ALIVE_DURING_MATCHING

// 友達リストに友達がいなくても場合によってはマッチメイク開始をOKする
#define DWC_MATCH_ACCEPT_NO_FRIEND

// 友達限定の友達指定ピアマッチメイクを有効にする
#define DWC_LIMIT_FRIENDS_MATCH_VALID

// サーバクライアントマッチメイクの途中締め切りを有効にする
#define DWC_STOP_SC_SERVER
    
// マッチメイクのプロトコルバージョン。バージョンの違うマッチメイクコマンドや
// サーバとは通信を行わないようにするために使う。
#define DWC_MATCHING_VERSION 3U
    
// gt2の標準に準拠（AIDビットマップの関係からも32以上にはできない）
#define DWC_MAX_CONNECTIONS 32

// QR2カスタムキーのインデックス（NUM_RESERVED_KEYS = 50）
// 50～99をDWCの予約キー、100～253をゲームで使用できるキーとする
#define DWC_QR2_PID_KEY            NUM_RESERVED_KEYS
#define DWC_QR2_MATCH_TYPE_KEY     (NUM_RESERVED_KEYS+1)
#define DWC_QR2_MATCH_RESV_KEY     (NUM_RESERVED_KEYS+2)
#define DWC_QR2_MATCH_VER_KEY      (NUM_RESERVED_KEYS+3)
#define DWC_QR2_MATCH_EVAL_KEY     (NUM_RESERVED_KEYS+4)

// 各キーの文字列
#define DWC_QR2_PID_KEY_STR        "dwc_pid"
#define DWC_QR2_MATCH_TYPE_KEY_STR "dwc_mtype"
#define DWC_QR2_MATCH_RESV_KEY_STR "dwc_mresv"
#define DWC_QR2_MATCH_VER_KEY_STR  "dwc_mver"
#define DWC_QR2_MATCH_EVAL_KEY_STR "dwc_eval"

// DWCの予約キー数（50）
#define DWC_QR2_RESERVED_KEYS  (100-NUM_RESERVED_KEYS)
    
// ゲームで最大使用できるキーの数（153）
#define DWC_QR2_GAME_RESERVED_KEYS (MAX_REGISTERED_KEYS-NUM_RESERVED_KEYS-DWC_QR2_RESERVED_KEYS)
    
// ゲーム定義キーの開始値
#define DWC_QR2_GAME_KEY_START 100


// SBメッセージ、及びGPバディメッセージのマッチメイク用コマンド
#define DWC_SB_COMMAND_STRING "SBCM"  // SBメッセージコマンド判別文字列
    
#define DWC_MATCH_COMMAND_RESERVATION    0x01  // NATネゴシエーション予約要求
#define DWC_MATCH_COMMAND_RESV_OK        0x02  // NATネゴシエーション予約完了
#define DWC_MATCH_COMMAND_RESV_DENY      0x03  // NATネゴシエーション予約拒否
#define DWC_MATCH_COMMAND_RESV_WAIT      0x04  // NATネゴシエーション予約待ち要求
#define DWC_MATCH_COMMAND_RESV_CANCEL    0x05  // NATネゴシエーション予約解除要求
#define DWC_MATCH_COMMAND_TELL_ADDR      0x06  // NN開始側アドレス通知（NATネゴシエーション不要）
#define DWC_MATCH_COMMAND_NEW_PID_AID    0x07  // 新規接続クライアントのプロファイルIDとAID通知
#define DWC_MATCH_COMMAND_LINK_CLS_REQ   0x08  // クライアント同士の接続要求
#define DWC_MATCH_COMMAND_LINK_CLS_SUC   0x09  // クライアント同士の接続成功通知
#define DWC_MATCH_COMMAND_CLOSE_LINK     0x0a  // 接続済みのコネクションクローズ通知
#define DWC_MATCH_COMMAND_RESV_PRIOR     0x0b  // NATネゴシエーション優先予約要求
#define DWC_MATCH_COMMAND_CANCEL         0x0c  // マッチメイクキャンセルコマンド
#define DWC_MATCH_COMMAND_CANCEL_SYN     0x0d  // マッチメイクキャンセル同期用SYN
#define DWC_MATCH_COMMAND_CANCEL_SYN_ACK 0x0e  // マッチメイクキャンセル同期用SYN-ACK
#define DWC_MATCH_COMMAND_CANCEL_ACK     0x0f  // マッチメイクキャンセル同期用ACK
#define DWC_MATCH_COMMAND_SC_CLOSE_CL    0x10  // 接続済みクライアントの意図しない切断通知
#define DWC_MATCH_COMMAND_POLL_TIMEOUT   0x11  // OPTION_MIN_COMPLETEのタイムアウトポーリング
#define DWC_MATCH_COMMAND_POLL_TO_ACK    0x12  // OPTION_MIN_COMPLETEのタイムアウトポーリングへのACK
#define DWC_MATCH_COMMAND_SC_CONN_BLOCK  0x13  // OPTION_SC_CONNECT_BLOCKのブロックによる予約拒否
#ifdef DWC_LIMIT_FRIENDS_MATCH_VALID
#define DWC_MATCH_COMMAND_FRIEND_ACCEPT  0x20  // クライアントからの新規接続友達許可
#endif
#define DWC_MATCH_COMMAND_CL_WAIT_POLL   0x40  // クライアント待ち状態タイムアウトポーリング
#define DWC_MATCH_COMMAND_SV_KA_TO_CL    0x41  // 上記ポーリングに対するサーバのキープアライブ
#define DWC_MATCH_COMMAND_DUMMY          0xff  // ダミーコマンド

#if 0
#define DWC_MATCH_CMD_RESEND_INTERVAL     60    // マッチメイクコマンド再送間隔
#define DWC_MATCH_CMD_RESEND_INTERVAL_ADD 150   // マッチメイクコマンド再送間隔追加時間ベース
#define DWC_MATCH_CMD_RESV_TIMEOUT        480   // NN予約コマンド応答タイムアウト
#define DWC_SB_UPDATE_INTERVAL            60    // SBサーバアップデート間隔
#define DWC_SB_UPDATE_INTERVAL_ADD        180   // SBサーバアップデート間隔追加時間ベース
#define DWC_QR2_KEEP_RESV_TIMEOUT         600   // QR2予約保持タイムアウト時間
#else
#define DWC_MATCH_CMD_RESEND_INTERVAL_MSEC     3000   // マッチメイクコマンド再送間隔
#define DWC_MATCH_CMD_RESEND_INTERVAL_ADD_MSEC 3000   // マッチメイクコマンド再送間隔追加時間ベース
#define DWC_MATCH_CMD_RESV_TIMEOUT_MSEC        6000   // NN予約コマンド応答タイムアウト
#define DWC_SB_UPDATE_INTERVAL_MSEC            3000   // SBサーバアップデート間隔
#define DWC_SB_UPDATE_INTERVAL_SHORT_MSEC      1000   // SBサーバアップデート間隔
#define DWC_SB_UPDATE_INTERVAL_ADD_MSEC        3000   // SBサーバアップデート間隔追加時間ベース
#define DWC_SB_UPDATE_INTERVAL_SHORT           1
#define DWC_SB_UPDATE_INTERVAL_LONG            2
#define DWC_SB_UPDATE_TIMEOUT                  30000  // SBサーバアップデート無応答タイムアウト時間
#endif

// コマンドを送ってすぐに処理してまたコマンドが返される処理の場合に
// 汎用的に使う、コマンド往復タイムアウト時間
#define DWC_MATCH_CMD_RTT_TIMEOUT          6000

// SBサーバアップデートを行ってから、目的のサーバデータが見つかるまでの処理に
// 汎用的に使うタイムアウト時間（検索２回分を想定）
#define DWC_MATCH_SB_UPDATE_TIMEOUT        13000

// マッチメイクコマンドの標準的な最大再送回数
#define DWC_MATCH_CMD_RETRY_MAX            5

// マッチメイク完了同期調整用タイムアウト時間
#define DWC_MATCH_SYN_ACK_WAIT_TIME        DWC_MATCH_CMD_RTT_TIMEOUT

// サーバクライアントマッチメイクキャンセル同期調整用タイムアウト時間
#define DWC_MATCH_CANCEL_SYN_ACK_WAIT_TIME DWC_MATCH_CMD_RTT_TIMEOUT

// 友達無指定ピアマッチメイクの予約保持タイムアウト時間
#define DWC_MATCH_RESV_KEEP_TIME_ANYBODY   DWC_MATCH_CMD_RTT_TIMEOUT
// 友達指定、サーバクライアントマッチメイクの予約保持タイムアウト時間
// （予約OKコマンド送信＋SBサーバアップデート＋NN開始時間を考慮）
#define DWC_MATCH_RESV_KEEP_TIME_FRIEND    (DWC_MATCH_CMD_RTT_TIMEOUT+DWC_MATCH_SB_UPDATE_TIMEOUT)

#define DWC_MAX_MATCH_NN_RETRY  1   // NATネゴシエーションリトライ最大回数
#define DWC_MATCH_NN_FAILURE_MAX 5  // NATネゴシエーション失敗許容最大回数（これを越えるとエラー終了）

// NATネゴシエーション失敗後リトライ待ちタイムアウト時間
#define DWC_WAIT_NN_RETRY_TIMEOUT          10000

// gt2Connectのタイムアウト時間
#define DWC_GT2_CONNECT_TIMEOUT            5000

// NATネゴシエーション完了後、gt2Connect受信待ちタイムアウト時間
#define DWC_WAIT_GT2_CONNECT_TIMEOUT       (DWC_GT2_CONNECT_TIMEOUT*DWC_MATCH_CMD_RETRY_MAX)

// クライアント同士の1対1の接続完了タイムアウト時間（サーバが使う）
// →CLS_REQコマンドの再送は行わず、タイムアウト時間になったら即タイムアウト
// 処理を行うようになったので、
// PARTNER_WAIT_TIME（10msec）×DWC_WAIT_NN_RETRY_TIMEOUT＋αの時間は
// 待ってNNのリトライを待つ必要がある。
#define DWC_MATCH_LINK_CLS_TIMEOUT         30000

// クライアントの待ち状態時にサーバからのデータ受信が以下の時間なければ、
// タイムアウトと判断してサーバへのポーリングを行う
#define DWC_MATCH_CL_WAIT_TIMEOUT          30000

// RESV_WAIT受信後予約コマンド最大再送回数（最初の１回は除いた再送のみの回数）
#define DWC_RESV_COMMAND_RETRY_MAX 16

// OPTION_MIN_COMPLETEで、タイムアウトポーリングをリトライする回数
// （最初の１回は除いた再送のみの回数）
#define DWC_MATCH_OPT_MIN_COMP_POLL_RETRY_MAX 5

// ServerBrowserLimitUpdate()に渡す、検索サーバ最大数
#define DWC_SB_UPDATE_MAX_SERVERS 6

// マッチメイク完了同期パケットの、Reliable送信データ本体サイズ
#define DWC_MATCH_SYN_DATA_BODY_SIZE 4

// [todo]
// マッチメイク要求友達インデックスリスト長。
// 仮に定数で設定するが、本来はゲーム側から指定された大きさだけ使用する
#define DWC_MAX_MATCH_IDX_LIST 64

    
//----------------------------------------------------------------------------
// enum
//----------------------------------------------------------------------------
// マッチメイクオプションタイプ
typedef enum {
    DWC_MATCH_OPTION_MIN_COMPLETE = 0,  // 友達指定・無指定ピアマッチメイクで、
                                        // 指定人数以下でもマッチメイクを完了する
    DWC_MATCH_OPTION_SC_CONNECT_BLOCK,  // サーバクライアントマッチメイクで、
                                        // １台マッチメイク完了後に他の接続を受け付けなくする
    DWC_MATCH_OPTION_NUM
} DWCMatchOptType;

// DWC_SetMatchingOption()の結果型
enum {
    DWC_SET_MATCH_OPT_RESULT_SUCCESS = 0,  // エラーなし
    DWC_SET_MATCH_OPT_RESULT_E_BAD_STATE,  // FriendsMatchライブラリ非動作中
    DWC_SET_MATCH_OPT_RESULT_E_INVALID,    // 無効なオプション指定
    DWC_SET_MATCH_OPT_RESULT_E_PARAM,      // パラメータエラー
    DWC_SET_MATCH_OPT_RESULT_E_ALLOC,      // Allocエラー
    DWC_SET_MATCH_OPT_RESULT_NUM
};


// マッチメイク状態列挙子
// [todo]
// ゲームも他ライブラリも参照しないenumはdwc_match.cに移す（上のdefineも）
typedef enum {
    DWC_MATCH_STATE_INIT = 0,           // 初期状態

    // クライアント側の状態
    DWC_MATCH_STATE_CL_WAITING,         // 待ち状態
    DWC_MATCH_STATE_CL_SEARCH_OWN,      // 自ホスト情報検索中
    DWC_MATCH_STATE_CL_SEARCH_HOST,     // 空きホスト検索状態（友達無指定の時のみ）
    DWC_MATCH_STATE_CL_WAIT_RESV,       // 予約に対するサーバからの返答待ち
    DWC_MATCH_STATE_CL_SEARCH_NN_HOST,  // 予約が完了した相手ホストを検索中
    DWC_MATCH_STATE_CL_NN,              // NATネゴシエーション中
    DWC_MATCH_STATE_CL_GT2,             // GT2コネクション確立中
    DWC_MATCH_STATE_CL_CANCEL_SYN,      // サーバクライアントでマッチメイクキャンセル同期調整中
    DWC_MATCH_STATE_CL_SYN,             // マッチメイク完了同期調整中
        
    // サーバ側の状態
    DWC_MATCH_STATE_SV_WAITING,         // 待ち状態
    DWC_MATCH_STATE_SV_OWN_NN,          // クライアントとのNATネゴシエーション中
    DWC_MATCH_STATE_SV_OWN_GT2,         // クライアントとのGT2コネクション確立中
    DWC_MATCH_STATE_SV_WAIT_CL_LINK,    // クライアント同士の接続完了待ち
    DWC_MATCH_STATE_SV_CANCEL_SYN,      // サーバクライアントでマッチメイクキャンセル同期調整SYN-ACK待ち
    DWC_MATCH_STATE_SV_CANCEL_SYN_WAIT, // サーバクライアントでマッチメイクキャンセル同期調整終了待ち
    DWC_MATCH_STATE_SV_SYN,             // マッチメイク終了同期調整SYN-ACK待ち
    DWC_MATCH_STATE_SV_SYN_WAIT,        // マッチメイク終了同期調整終了待ち

    // 共通状態
    DWC_MATCH_STATE_WAIT_CLOSE,         // コネクションクローズ完了待ち

    // マッチメイクオプション使用時のみの状態
    DWC_MATCH_STATE_SV_POLL_TIMEOUT,    // サーバがOPTION_MIN_COMPLETEのタイムアウトをポーリング中

    DWC_MATCH_STATE_NUM
} DWCMatchState;

// マッチメイクタイプ列挙子
enum {
    DWC_MATCH_TYPE_ANYBODY = 0,  // 友達無指定ピアマッチメイク
    DWC_MATCH_TYPE_FRIEND,       // 友達指定ピア間あっちメイク
    DWC_MATCH_TYPE_SC_SV,        // サーバクライアントマッチメイクのサーバ側
    DWC_MATCH_TYPE_SC_CL,        // サーバクライアントマッチメイクのクライアント側
    DWC_MATCH_TYPE_NUM
};


//----------------------------------------------------------------------------
// typedef - function
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  友達指定、友達無指定ピアマッチメイク完了コールバック型
  引数　：error  DWCエラー種別
          cancel TRUE:キャンセルでマッチメイク終了、FALSE:キャンセルされていない
          param  コールバック用パラメータ
  戻り値：なし
  規定人数のコネクションが確立された時に一度だけ呼ばれる
 *---------------------------------------------------------------------------*/
typedef void (*DWCMatchedCallback)(DWCError error, BOOL cancel, void* param);

/*---------------------------------------------------------------------------*
  サーバクライアントマッチメイク完了コールバック型
  引数　：error    DWCエラー種別
          cancel   TRUE:キャンセルでマッチメイク終了、FALSE:キャンセルではない
          self     TRUE: 自分がサーバへの接続に成功、もしくは
                         接続をキャンセルした。
                   FALSE:他のホストがサーバへの接続に成功、
                         もしくは接続をキャンセルした。
          isServer TRUE: self = FALSE の時に上記の動作を完了したホストが
                         サーバである、
                   FALSE:self = FALSE の時に上記の動作を完了したホストが
                         クライアントであるか、self = TRUE
          index    self = FALSE の時に、上記の動作を完了したホストの
                   友達リストインデックス。
                   ホストが友達でないか、self = TRUE の時は-1となる。
          param    コールバック用パラメータ
  戻り値：なし
  １つのコネクションが確立される（失敗、キャンセルも）度に呼ばれる。
  エラー発生時は、param以外の各引数の値は不定。
 *---------------------------------------------------------------------------*/
typedef void (*DWCMatchedSCCallback)(DWCError error, BOOL cancel, BOOL self, BOOL isServer, int index, void* param);

/*---------------------------------------------------------------------------*
  新規接続クライアント接続開始通知コールバック型
  引数　：index 新規接続クライアントの友達リストインデックス。
                新規接続クライアントが友達でなければ-1となる。
          param コールバック用パラメータ
  戻り値：なし
  サーバクライアントマッチメイクで、今できているネットワークに新たに
  別のクライアントが接続を開始した時に呼ばれる
 *---------------------------------------------------------------------------*/
typedef void (*DWCNewClientCallback)(int index, void* param);

/*---------------------------------------------------------------------------*
  プレイヤー評価コールバック型
  引数　：index 評価対象プレイヤーのインデックス。マッチメイクキー値を
                取得する関数に渡す必要がある。
          param コールバック用パラメータ
  戻り値：プレイヤーの評価値。0以下ならマッチメイクの対象から外される。
  友達指定・友達無指定ピアマッチメイク時に、マッチメイク対象プレイヤーを
  見つける度に呼ばれる。
  ここで設定した戻り値の大きいプレイヤーほど接続先として選ばれる可能性が高くなる。
 *---------------------------------------------------------------------------*/
typedef int (*DWCEvalPlayerCallback)(int index, void* param);

/*---------------------------------------------------------------------------*
  サーバクライアントマッチメイク締め切りコールバック型
  引数　：param コールバック用パラメータ
  戻り値：なし
  サーバクライアントマッチメイクのサーバにおいて、マッチメイクの締め切りを
  行った場合に、締め切り処理が完了した時に呼ばれる。
 *---------------------------------------------------------------------------*/
typedef void (*DWCStopSCCallback)(void* param);


//----------------------------------------------------------------------------
// struct
//----------------------------------------------------------------------------
// マッチメイクオプション DWC_MATCH_OPTION_MIN_COMPLETE の時に使用する
// オプション値構造体
typedef struct DWCstMatchOptMinComplete {
    u8  valid;       // 1:設定を有効にする、0:設定を無効にする
    u8  minEntry;    // 最低マッチメイク完了人数（自分を含む）
    u8  pad[2];      // パディング
    u32 timeout;     // マッチメイク完了とするタイムアウト時間（単位：msec）
} DWCMatchOptMinComplete;


// NATネゴシエーションコールバックパラメータ用情報構造体
typedef struct DWCstNNInfo
{
    u8  isQR2;       // 1:QR2側なのでリトライ必要なし、0:SB側なのでリトライ必要
    u8  retryCount;  // リトライ回数計測カウンタ
    u16 port;        // NATネゴシエーション先のポート番号
    u32 ip;          // NATネゴシエーション先のIP
    int cookie;      // NATネゴシエーションのクッキー値。0でNN中でないことを表す
} DWCNNInfo;

// マッチメイク用コマンド制御構造体
// [todo]
// dataのサイズはDWCSBMessageと共に見直す必要あり
typedef struct DWCstMatchCommandControl {
    u8  command;      // 送信コマンド
    u8  count;        // リトライ回数
    u16 port;         // 送信先QR2用パブリックポート番号
    u32 ip;           // 送信先QR2用IP
    u32 data[32];     // 追加送信データ
    int profileID;    // 送信先プロファイルID
    int len;          // 追加送信データ要素数
    OSTick sendTime;  // 送信時間
} DWCMatchCommandControl;

// マッチメイク制御構造体
typedef struct DWCstMatchControl
{
    GPConnection* pGpObj;  // gpコネクション構造体ポインタへのポインタ
        
    GT2Socket* pGt2Socket; // GT2オブジェクトポインタへのポインタ
    GT2ConnectionCallbacks* gt2Callbacks;  // GT2コールバック集へのポインタ
    u8  gt2ConnectCount;   // GT2リトライカウンタ
    u8  gt2NumConnection;  // 成立したGT2コネクションの数
    u8  gt2NumValidConn;   // 全員と接続完了した有効なGT2コネクションの数
    u8  pad1;
        
    qr2_t qr2Obj;          // QR2オブジェクトへのポインタ
    vu8  qr2NNFinishCount; // NATネゴシエーション完了カウンタ
    vu8  qr2MatchType;     // マッチメイクのタイプ。DWC_MATCH_TYPE_*列挙子
    vu8  qr2NumEntry;      // 自分以外に最大何人集めたいか
    vu8  qr2IsReserved;    // 1:自分へのNN予約済み、0:未予約
#ifdef DWC_QR2_ALIVE_DURING_MATCHING
    u8   qr2ShutdownFlag;  // 1:QR2シャットダウンを予約中、0:何も無し
    u8   pad2;
#else
    u16  pad2;
#endif
    u16  qr2Port;          // 自分のQR2用パブリックポート番号
    u32  qr2IP;            // 自分のQR2用パブリックIP
    volatile int qr2Reservation;    // 自分へのNN予約ホストのプロファイルID
    u32 qr2IPList[DWC_MAX_CONNECTIONS];    // 接続ホストのQR2用IPアドレスリスト
    u16 qr2PortList[DWC_MAX_CONNECTIONS];  // 接続ホストのQR2用ポート番号リスト

    ServerBrowser sbObj;   // SBオブジェクトへのポインタ
//    u32 sbUpdateCount;     // SBサーバアップデート呼び出し間隔カウンタ
    BOOL   sbUpdateFlag;   // SBサーバアップデート呼び出しフラグ
    OSTick sbUpdateTick;   // SBサーバアップデート呼び出し間隔時間
    int sbPidList[DWC_MAX_CONNECTIONS];  // 接続ホストのプロファイルIDリスト
	OSTick sbUpdateRequestTick; // SBサーバリミットアップデート関数呼び出し時間

    u8  nnRecvCount;       // 同一NATネゴシエーション受信回数
    u8  nnFailureCount;    // NN失敗カウント
    u16 nnCookieRand;      // NATネゴシエーションに使うクッキー用乱数値
    int nnLastCookie;      // 前回受信したNN用クッキー
    OSTick nnFailedTime;   // NATネゴシエーション失敗時の時間
    OSTick nnFinishTime;   // NATネゴシエーション終了時の時間
    DWCNNInfo nnInfo;      // NATネゴシエーション情報構造体

    DWCMatchState state;   // マッチメイク進行状態
    u8  clLinkProgress;    // クライアント同士の接続確立進度
    u8  friendCount;       // 友達指定ピアマッチメイクの友達リスト調査カウンタ
    u8  distantFriend;     // 1:友達の友達との接続を許す、2:許さない
    u8  resvWaitCount;     // 予約コマンド再送回数 
    u8  closeState;        // コネクションクローズ状態。DWC_MATCH_CLOSE_STATE_*列挙子で指定
    u8  cancelState;       // マッチメイクキャンセル状態。DWC_MATCH_CANCEL_STATE_*列挙子で指定
    u8  scResvRetryCount;  // サーバクライアントマッチメイクのクライアントの予約リトライ回数
    u8  synResendCount;    // マッチメイク完了同期のSYNパケット再送回数
    u8  cancelSynResendCount;  // マッチメイクキャンセル完了同期のSYNパケット再送回数
    u8  clWaitTimeoutCount;    // クライアント待ち状態タイムアウト回数
#ifdef DWC_STOP_SC_SERVER
    u8  stopSCFlag;        // サーバクライアントマッチメイク締め切りフラグ
    u8  pad3;
#endif
    u16 baseLatency;       // クライアントならサーバとのレイテンシ、サーバならクライアントのうちの最大のレイテンシ
    u16 cancelBaseLatency; // サーバクライアントマッチメイクキャンセル同期に使うGPサーバ経由のホスト間最短レイテンシ
    u16 searchPort;        // 検索サーバQR2パブリックポート番号
#ifdef DWC_STOP_SC_SERVER
    u16 pad4;
#endif
    u32 searchIP;          // 検索サーバQR2パブリックIP
#if 0
    u32 cmdResendCount;    // マッチメイクコマンド再送カウンタ
    u32 cmdTimeoutCount;   // マッチメイクコマンドレスポンスタイムアウトカウンタ
#else
    BOOL   cmdResendFlag;  // マッチメイクコマンド再送フラグ
    OSTick cmdResendTick;  // マッチメイクコマンド再送時間
    u32 cmdTimeoutTime;    // マッチメイクコマンドレスポンスタイムアウトカウンタ
    OSTick cmdTimeoutStartTick;  // マッチメイクコマンドレスポンスタイムアウトカウンタ
#endif
    u32 synAckBit;         // マッチメイク完了同期のSYN-ACKの受信AIDビットマップ
    u32 cancelSynAckBit;   // マッチメイクキャンセル同期のSYN-ACKの受信AIDビットマップ
#ifdef DWC_LIMIT_FRIENDS_MATCH_VALID
    u32 friendAcceptBit;   // 新規接続友達許可コマンド受信ビットマップ
#endif
    OSTick lastSynSent;          // 最後にマッチメイク完了同期信号を送信した時間
    OSTick lastCancelSynSent;    // 最後にマッチメイクキャンセル同期信号を送信した時間
    OSTick closedTime;           // 他ホストからクローズされた時間
    OSTick clWaitTime;           // クライアント待ち状態開始時間
    volatile int profileID;      // 自分のプロファイルID
    int reqProfileID;      // 友達指定ピアマッチメイクでNN要求を送った相手のプロファイルID
    int priorProfileID;    // 優先NN要求を送信した相手サーバのプロファイルID
    int cbEventPid;        // 主にサーバクライアントピアマッチメイク完了
                           // コールバックに渡す友達のプロファイルID。
                           // キャンセルしホストのプロファイルIDの保存にも用いる。
                               
    u32 ipList[DWC_MAX_CONNECTIONS];    // 接続ホストのゲームIPアドレスリスト
    u16 portList[DWC_MAX_CONNECTIONS];  // 接続ホストのゲームポート番号リスト
    u8  aidList[DWC_MAX_CONNECTIONS];   // 接続ホスト固有のIDリスト
    u32 validAidBitmap;                 // 全員と接続完了した有効なAIDのビットマップ。マッチメイク中のものを区別する為に使う。
    
    const char* gameName;  // GameSpyから与えられるゲーム名
    const char* secretKey; // GameSpyから与えられるシークレットキー

    const DWCFriendData* friendList;  // 友達リストへのポインタ
    int friendListLen;                // 友達リスト長
    u8  friendIdxList[DWC_MAX_MATCH_IDX_LIST];  // マッチメイク友達インデックスリスト
    int friendIdxListLen;  // マッチメイク友達インデックスリスト長
    u32 svDataBak[DWC_MAX_CONNECTIONS+1];    // サーバ接続ホストデータバックアップ or キャンセル時データバックアップ
                                             // [todo]
                                             // 使い道の少ないデータなので動的に取るようにする

    DWCMatchCommandControl cmdCnt;           // マッチメイク用コマンド制御構造体

    DWCMatchedSCCallback matchedCallback;    // dwc_main.cのマッチメイク完了コールバック
    void* matchedParam;                      // 上記コールバック用パラメータ
    DWCNewClientCallback newClientCallback;  // 新規接続クライアント通知コールバック
    void* newClientParam;                    // 上記コールバック用パラメータ
    DWCEvalPlayerCallback evalCallback;      // プレイヤー評価コールバック
    void* evalParam;                         // 上記コールバック用パラメータ
#ifdef DWC_STOP_SC_SERVER
    DWCStopSCCallback stopSCCallback;        // サーバクライアントマッチメイク締め切り完了コールバック
    void* stopSCParam;                       // 上記コールバック用パラメータ
#endif
} DWCMatchControl;

// SBメッセージヘッダ構造体
typedef struct DWCstSBMessageHeader {
    char identifier[4];  // 識別子
    u32 version;         // マッチメイクプロトコルバージョン
    u8  command;         // コマンド種別（定数DWC_SB_COMMAND_*）
    u8  size;            // コマンドに対するデータサイズ（ヘッダサイズは含まない）
    u16 qr2Port;         // 自分のQR2用パブリックポート番号
    u32 qr2IP;           // 自分のQR2用パブリックIP
    int profileID;       // 自分のプロファイルID 
} DWCSBMessageHeader;

// SBメッセージ構造体
// 複数のメッセージを続けて、１つのデータとして送信することも可能。
// [todo]
// dataのサイズは何らかの上限をGPバディメッセージコマンドと共通で
// 設けなければならない
typedef struct DWCstSBMessage {
    DWCSBMessageHeader header;      // SBメッセージヘッダ
    u32 data[DWC_MAX_CONNECTIONS];  // 送信データ本体
} DWCSBMessage;

// ゲーム定義のマッチメイク用QR2キーデータ構造体
typedef struct DWCstGameMatchKeyData {
    u8  keyID;          // キーID
    u8  isStr;          // 0:バリューがint、1:バリューが文字列
    u16 pad;
    char* keyStr;       // キー文字列へのポインタ
    const void* value;  // バリューへのポインタ
} DWCGameMatchKeyData;

// マッチメイクオプション DWC_MATCH_OPTION_MIN_COMPLETE の時に使用する
// オプション値内部構造体 
typedef struct DWCstMatchOptMinCompleteIn {
    u8  valid;            // 1:設定を有効にする、0:設定を無効にする
    u8  minEntry;         // 最低マッチメイク完了人数（自分を含む）
    u8  retry;            // 1:最初のポーリング以降、0:最初のポーリングまで
    u8  pad;
    u32 timeout;          // マッチメイク完了とするタイムアウト時間（単位：msec）
    u32 recvBit;          // ポーリングに対するクライアントの返信状況ビット
    u32 timeoutBit;       // クライアントのタイムアウト状況ビット
    OSTick startTime;     // マッチメイク開始時間
    OSTick lastPollTime;  // 最後にポーリングを行った時間
} DWCMatchOptMinCompleteIn;

// マッチメイクオプションDWC_MATCH_OPTION_SC_CONNECT_BLOCK の時に使用する
// オプション値内部構造体
typedef struct DWCstMatchOptSCBlock {
    u8  valid;         // 1:設定を有効にする、0:設定を無効にする
    u8  lock;          // 1:接続完了後新規接続拒否中、0:通常時
    u16 pad;
} DWCMatchOptSCBlock;


//----------------------------------------------------------------------------
// function - external
//----------------------------------------------------------------------------
#ifdef DWC_QR2_ALIVE_DURING_MATCHING
/*---------------------------------------------------------------------------*
  マッチメイクステータスのマスターサーバ登録関数（友達指定、無指定ピアマッチメイク用）
  引数　：なし
  戻り値：TRUE :マスターサーバにマッチメイクステータスが登録された、
          FALSE:ログイン前、もしくはエラーのため登録されなかった
  用途　：自ホストのマッチメイクステータスをマスターサーバに登録する。
          この関数が呼ばれてから最短15秒程で、他ホストから自ホストの状態が
          見えるようになり、マッチメイクを進められるようになる。
          サーバクライアントマッチメイク以外では、マッチメイクが終わった時点で
          マスターサーバからこのマッチメイクステータスが消去される。
          マッチメイク開始関数を呼べば、ライブラリ内でこの関数が呼ばれるが、
          マッチメイクの終了を早める為に、予めマッチメイクステータスを
          マスターサーバに登録しておきたい場合は、この関数を呼び出せばよい。
          また、マッチメイクステータスはDWC_ShutdownFriendsMatch()を呼んだ時にも
          消去される。
 *---------------------------------------------------------------------------*/
extern BOOL DWC_RegisterMatchingStatus(void);
#endif


/*---------------------------------------------------------------------------*
  マッチメイク処理キャンセル関数
  引数　：なし
  戻り値：TRUE:キャンセル処理を実行した、FALSE:マッチメイク中でない
  用途　：進行中のマッチメイク処理をキャンセルする。
          キャンセル処理は本関数内で完了し、マッチメイク完了コールバックが
          呼び出される。
 *---------------------------------------------------------------------------*/
BOOL DWC_CancelMatching(void);


/*---------------------------------------------------------------------------*
  マッチメイク処理キャンセル関数 非同期版 (obsolete function)
  引数　：なし
  戻り値：TRUE:キャンセル処理の実行を開始した、FALSE:マッチメイク中でない
  用途　：進行中のマッチメイク処理をキャンセルする。
          必要なキャンセル処理を行った後、マッチメイク完了コールバックが
          呼び出される。
          →DWC_CancelMatching()を使って下さい
 *---------------------------------------------------------------------------*/
extern BOOL DWC_CancelMatchingAsync(void);


/*---------------------------------------------------------------------------*
  マッチメイクキャンセル有効確認関数
  引数　：なし
  戻り値：TRUE:キャンセル可能、FALSE:マッチメイク中でなかったり、中断できない
          状況でキャンセル不可能。
  用途　：進行中のマッチメイク処理をキャンセルできるかどうかを調べる
 *---------------------------------------------------------------------------*/
extern BOOL DWC_IsValidCancelMatching(void);


#ifdef DWC_STOP_SC_SERVER
/*---------------------------------------------------------------------------*
  サーバクライアントマッチメイク サーバ締め切り関数
  引数　：callback 締め切り完了コールバック
          param    コールバック用パラメータ
  戻り値：TRUE :マッチメイクの締め切り開始。完了したらコールバックが返ってくる。
          FALSE:エラー発生、FriendsMatchライブラリ非動作中などの理由で締め切り失敗。
  用途　：サーバクライアントマッチメイクのマッチメイクを締め切り、以後新規の
          接続を受け付けないようにする。
 *---------------------------------------------------------------------------*/
extern BOOL DWC_StopSCMatchingAsync(DWCStopSCCallback callback, void* param);
#endif


/*---------------------------------------------------------------------------*
  マッチメイク指標キー追加関数 int版
  引数　：keyID        キーIDを指定してセットしたい場合はここにキーIDを渡す。
                       新規にkeyIDを取得したければ DWC_QR2_GAME_KEY_START
                       以下の値（0など）をセットすれば良い。
                       既に使用中の追加キーIDを指定した場合は、使用中のものと
                       同じkeyStringを渡せばvalueSrcをセットし直すが、
                       違うkeyStringを渡した場合は何もしない。
          keyString    キー識別のための文字列へのポインタ。
                       文字列はコピーしてライブラリ内で保持します。
          valueSrc     キーに対応する値へのポインタで、マスターサーバからの
                       ポーリングがあると、このポインタから値を参照して
                       マスターサーバに値をアップする。
  戻り値：追加したキーのキーID。パラメータエラー、もしくはキーの設定上限を
          越えたため追加できなかった場合は0を返す。
          DWC_InitFriendsMatch()が呼ばれるより前に呼び出しても0を返す。
  用途　：マッチメイクの指標となるキー（int型）を追加する。
 *---------------------------------------------------------------------------*/
extern u8  DWC_AddMatchKeyInt(u8 keyID,
                              const char* keyString,
                              const int* valueSrc);


/*---------------------------------------------------------------------------*
  マッチメイク指標キー追加関数 string版
  引数　：keyID        キーIDを指定してセットしたい場合はここにキーIDを渡す。
                       新規にkeyIDを取得したければ DWC_QR2_GAME_KEY_START
                       以下の値（0など）をセットすれば良い。
                       既に使用中の追加キーIDを指定した場合は、使用中のものと
                       同じkeyStringを渡せばvalueSrcをセットし直すが、
                       違うkeyStringを渡した場合は何もしない。
          keyString    キー識別のための文字列へのポインタ。
                       文字列はコピーしてライブラリ内で保持します。
          valueSrc     キーに対応する文字列へのポインタで、マスターサーバからの
                       ポーリングがあると、このポインタから文字列を参照して
                       マスターサーバに文字列をアップする。
  戻り値：追加したキーのキーID。パラメータエラー、もしくはキーの設定上限を
          越えたため追加できなかった場合は0を返す。
          DWC_InitFriendsMatch()が呼ばれるより前に呼び出しても0を返す。
  用途　：マッチメイクの指標となるキー（char*型）を追加する。
 *---------------------------------------------------------------------------*/
extern u8  DWC_AddMatchKeyString(u8 keyID,
                                 const char* keyString,
                                 const char* valueSrc);


/*---------------------------------------------------------------------------*
  マッチメイク指標キー取得関数 int版
  引数　：index     マッチメイク候補プレイヤーのインデックス。評価コールバックの
                    引数 index をそのまま渡す。
          keyString キー識別のための文字列へのポインタ
          idefault  指定したキーを持っていないプレイヤーだった場合の
                    デフォルト値
  戻り値：マッチメイク候補プレイヤーが持つ、指定したキーに対応する値。
          キーがない場合は引数 idefault に渡した値が返ってくる。
  用途　：マッチメイク指標キーに対応する値を取得する。
          プレイヤー評価コールバック内での使用のみ保証しており、
          それ以外の場所で呼び出した場合の戻り値は不定。
 *---------------------------------------------------------------------------*/
extern int DWC_GetMatchIntValue(int index, const char* keyString, int idefault);


/*---------------------------------------------------------------------------*
  マッチメイク指標キー取得関数 string版
  引数　：index     マッチメイク候補プレイヤーのインデックス。評価コールバックの
                    引数 index をそのまま渡す。
          keyString キー識別のための文字列へのポインタ
          sdefault  指定したキーを持っていないプレイヤーだった場合の
                    デフォルト文字列
  戻り値：マッチメイク候補プレイヤーが持つ、指定したキーに対応する文字列。
          キーがない場合は引数 sdefault に渡した文字列が返ってくる。
  用途　：マッチメイク指標キーに対応する文字列を取得する。
          プレイヤー評価コールバック内での使用のみ保証しており、
          それ以外の場所で呼び出した場合の戻り値は不定。
 *---------------------------------------------------------------------------*/
extern const char* DWC_GetMatchStringValue(int index, const char* keyString, const char* sdefault);


/*---------------------------------------------------------------------------*
  マッチメイクタイプ取得関数
  引数　：なし
  戻り値：最後に行ったマッチメイク種別。DWC_MATCH_TYPE_*で定義される。
          オフライン状態なら-1を返す。
  用途　：最後に行ったマッチメイク種別を取得する。
 *---------------------------------------------------------------------------*/
extern int DWC_GetLastMatchingType(void);


/*---------------------------------------------------------------------------*
  マッチメイクオプションセット関数
  引数　：opttype マッチメイクオプションの型
          optval  マッチメイクオプションの値へのポインタ。
                  opttypeに対応した型のポインタを渡す。
          optlen  型が配列の場合のみ、その配列のサイズを渡す必要がある。
  戻り値：DWC_SET_MATCH_OPT_RESULT_*型の列挙子
  用途　：マッチメイクについての様々なオプションを設定する。
 *---------------------------------------------------------------------------*/
extern int DWC_SetMatchingOption(DWCMatchOptType opttype, const void* optval, int optlen);


/*---------------------------------------------------------------------------*
  マッチメイクオプション取得関数
  引数　：opttype マッチメイクオプションの型
          optval  マッチメイクオプションの値格納先ポインタ。
                  opttypeに対応した型のポインタを渡す。
          optlen  取得したオプションの値のサイズ格納先
  戻り値：DWC_SET_MATCH_OPT_RESULT_*型の列挙子
  用途　：マッチメイクについての設定中のオプションを取得する。
          指定したオプションが設定されていない場合は、何もせずに、
          引数optlenに0を格納して返す。
 *---------------------------------------------------------------------------*/
extern int DWC_GetMatchingOption(DWCMatchOptType opttype, void* optval, int* optlen);


/*---------------------------------------------------------------------------*
  指定人数以下マッチメイク完了オプション・状態取得関数
  引数　：time マッチメイク開始からの経過時間（単位：msec）格納先ポインタ。
               時間が必要なければNULLを渡せば良い。
  戻り値：0 :タイムアウト時間内
          1 :タイムアウト時間を越えており、指定人数以下でマッチメイク完了する可能性あり
          -1:このマッチメイクオプションが設定されていない、もしくはOFFになっている
  用途　：指定人数以下マッチメイク完了オプションの時間経過状況を取得する。
 *---------------------------------------------------------------------------*/
extern int DWC_GetMOMinCompState(u64* time);


/*---------------------------------------------------------------------------*
  マッチメイクブロックオプション状態取得関数
  引数　：なし
  戻り値：TRUE :新規接続クライアントをブロック中、
          FALSE:ブロック中でないか、オプションが設定されていない
  用途　：マッチメイクブロックオプションのブロック状態を取得する
 *---------------------------------------------------------------------------*/
extern BOOL DWC_GetMOSCConnectBlockState(void);


/*---------------------------------------------------------------------------*
  マッチメイクブロック解除関数
  引数　：なし
  戻り値：なし
  用途　：マッチメイクブロックオプションのブロック状態を解除する
 *---------------------------------------------------------------------------*/
extern void DWC_ClearMOSCConnectBlock(void);


/*---------------------------------------------------------------------------*
  マッチメイク内部状態取得関数
  引数　：なし
  戻り値：現在のマッチメイク内部状態
  用途　：マッチメイク中の内部状態を取得する。デバッグ中にマッチメイクの内部状態を
          知りたい場合にのみ使用する。
 *---------------------------------------------------------------------------*/
extern DWCMatchState DWC_GetMatchingState(void);


//----------------------------------------------------------------------------
// function - internal
//----------------------------------------------------------------------------
extern void DWCi_MatchInit(DWCMatchControl* matchcnt,
                           GPConnection pGpObj,
                           GT2Socket* pGt2Socket,
                           GT2ConnectionCallbacks* gt2Callbacks,
                           const char* gameName,
                           const char* secretKey,
                           const DWCFriendData friendList[],
                           int  friendListLen);


extern qr2_error_t DWCi_QR2Startup(int profileID);


extern void DWCi_ConnectToAnybodyAsync(u8  numEntry,
                                       const char* addFilter,
                                       DWCMatchedSCCallback matchedCallback,
                                       void* matchedParam,
                                       DWCEvalPlayerCallback evalCallback,
                                       void* evalParam);


extern void DWCi_ConnectToFriendsAsync(const u8 friendIdxList[],
                                       int friendIdxListLen,
                                       u8  numEntry,
                                       BOOL distantFriend,
                                       DWCMatchedSCCallback matchedCallback,
                                       void* matchedParam,
                                       DWCEvalPlayerCallback evalCallback,
                                       void* evalParam);


// [arakit] main 051027
extern void DWCi_SetupGameServer(u8  maxEntry,
                                 DWCMatchedSCCallback matchedCallback,
                                 void* matchedParam,
                                 DWCNewClientCallback newClientCallback,
                                 void* newClientParam);


extern void DWCi_ConnectToGameServerAsync(int serverPid,
                                          DWCMatchedSCCallback matchedCallback,
                                          void* matchedParam,
                                          DWCNewClientCallback newClientCallback,
                                          void* newClientParam);


extern void DWCi_MatchProcess(BOOL fullSpec);


extern void DWCi_GT2ConnectAttemptCallback(GT2Socket socket,
                                           GT2Connection connection,
                                           unsigned int ip, unsigned
                                           short port, int latency,
                                           GT2Byte* message, int len);


extern GT2Bool DWCi_GT2UnrecognizedMessageCallback(GT2Socket socket,
                                                   unsigned int ip,
                                                   unsigned short port,
                                                   GT2Byte* message,
                                                   int len);


extern void DWCi_GT2ConnectedCallback(GT2Connection connection,
                                      GT2Result result,
                                      GT2Byte* message,
                                      int len);


extern void DWCi_MatchGPRecvBuddyMsgCallback(GPConnection* pconnection,
                                             int profileID,
                                             char* message);


extern void DWCi_StopMatching(DWCError error, int errorCode);


extern void DWCi_ClearQR2Key(void);


extern void DWCi_ProcessMatchSynPacket(u8 aid, u16 type, u8* data);


extern BOOL DWCi_ProcessMatchClosing(DWCError error, int errorCode, int profileID);


extern void DWCi_ProcessMatchSCClosing(int clientPid);


extern BOOL DWCi_DeleteHostByProfileID(int profileID, int numHost);


extern int  DWCi_DeleteHostByIndex(int index, int numHost);


extern int  DWCi_GetNumAllConnection(void);


extern int  DWCi_GetNumValidConnection(void);


extern void DWCi_SetNumValidConnection(void);


extern int  DWCi_GetAllAIDList(u8** aidList);


extern int  DWCi_GetValidAIDList(u8** aidList);


extern GPResult DWCi_GPSetServerStatus(void);


extern void DWCi_ShutdownMatch(void);


extern BOOL DWCi_IsShutdownMatch(void);


extern int  DWCi_GetNumValidConnection(void);


extern void DWCi_NNFreeNegotiateList(void);


#ifdef __cplusplus
}
#endif

#endif // DWC_MATCH_H_
