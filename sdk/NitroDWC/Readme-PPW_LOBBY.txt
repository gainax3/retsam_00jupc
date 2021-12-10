■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■                                                                          ■
■  Readme.txt                                                              ■
■                                                                          ■
■  ポケモンプラチナ Wi-Fi ロビーライブラリ                                 ■
■                                                                          ■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

1. はじめに
==============================================================================

　このライブラリは、GameSpyのIRCサーバを介して他のプレイヤーと通信するものです。
ライブラリの詳細はmanフォルダ内の関数リファレンスを参照してください。


2. ディレクトリ構成
==============================================================================

+build----+-demos      デモ
          +-libraries  ライブラリソースファイル
+include               外部インクルードファイル
+lib                   ライブラリファイル
+man                   関数リファレンス
+tools                 管理ツール


3. テスト環境
==============================================================================

　本パッケージは、以下の環境で構築、動作確認されました。

 NitroSDK-4_2-patch-plus-080118
 NITRO-System-Library-071126-patch1
 NitroDWC-2_2plus5-080612
 NitroWiFi-2_1-patch-plus3-070920
 CodeWarrior-2.0-sp2-patch2


4. インストール
==============================================================================

　パッケージに含まれるすべてのファイルを既存のNitroDWCのパッケージに上書きして
ください。
ビルド済みのライブラリファイルは付属していますが、ソースコードが同梱されていま
すのでビルドすることも可能です。
ライブラリをビルドする場合は前回上書きしたものをクリーン後、ビルドを行ってくだ
さい。
$(NITRODWC_ROOT)/include/ppwlobby/ppw_lobby.hをインクルードし、
$(NITRODWC_ROOT)/lib以下のlibdwcppwlobby.aとlibdwcilobby.aとlibdwcenc.aをリンク
してください。

5. デモ
==============================================================================

　ロビーライブラリを初期化し、メッセージ送信などを行うデモです。
初期化完了後にXボタンを押すとロビーライブラリの関数を順に呼び出します。
Aボタンを押すとミニゲームの募集からP2P通信までを行います。

6. 管理用ツール
==============================================================================

　tools以下にWi-Fiひろばの管理ツールが収録されています。詳しくは
"tools/PpwAdminTool/Readme.txt"を参照してください。


【更新履歴】

■ 2008/07/07 1_0_0

・20人集まった瞬間に人が抜けるとロックされない人が出てくる不具合を修正しました。

■ 2008/07/02 1_0_0 beta 20

・接続するドメインがpeerchat.gamespy.comだったのを
  <gamename>.gs.peerchat.nintendowifi.netに修正しました。

■ 2008/06/23 1_0_0 beta 19

・初期化時にライブラリをシャットダウンするとライブラリが不正な状態となり正しく
シャットダウンされなくなるバグを修正しました。
・PPW_LobbyShutdownAsync関数の仕様を変更しました。DWC_InitFriendsMatch関数と共
にロビーライブラリを使用しているときは、ロビーライブラリの終了を確認した後に
DWC_ShutdownFriendsMatch関数を呼び出してください。  

■ 2008/06/20 1_0_0 beta 18

・PPW_LobbySystemProfileUpdatedCallbackコールバックが
  PPW_LobbyPlayerConnectedCallbackコールバックの前及び
  PPW_LobbyPlayerDisconnectedCallbackコールバックの後に呼ばれる可能性がありまし
  たので呼ばれないように修正しました。

■ 2008/06/18 1_0_0 beta 17

・メインチャンネルに接続する際に3回まで再試行するようにしました。
・管理ツールをver1_0_0 beta7に更新しました。

■ 2008/06/16 1_0_0 beta 16

・アンケート情報にスペシャルウィークかどうかのフラグを追加しました。
・管理ツールをver1_0_0 beta6に更新しました。

■ 2008/06/13 1_0_0 beta 15

・アンケート機能に対応しました。
・アンケートを提出するPPW_LobbySubmitQuestionnaire関数を追加しました。
・取得したアンケート情報を通知するPPW_LobbyGetQuestionnaireCallbackコールバッ
　クを追加しました。
・アンケートの提出が完了したときに呼び出される
　PPW_LobbySubmitQuestionnaireCallbackコールバックを追加しました。
・エラーの状態からエラーコードを取得するPPW_LobbyToErrorCode関数を追加しました。
・管理ツールをver1_0_0 beta5に更新しました。

■ 2008/05/27 1_0_0 beta 14

・タイプ表示に対応しました。
・チャットサーバへの接続とスケジュールなどのデータ取得を同時に行うことにより、
  初期化時間を若干短縮しました。
・管理ツールをver1_0_0 beta4に更新しました。

■ 2008/05/08 1_0_0 beta 13

・PPW_LobbyCheckProfileCallbackコールバックの返り値をvoidに修正しました。

■ 2008/05/07 1_0_0 beta 12

・不正チェックを行うようにしました。
・不正チェックの結果を通知する、PPW_LobbyCheckProfileCallbackコールバックを追加
  しました。 
・スケジュールやVIP設定の取得先(リリース/デバッグ)がPPW_LobbyInitializeAsync関
  数のprefixにより決定されるようにしました。DWC_LOBBY_CHANNEL_PREFIX_RELEASEは
  リリースサーバから、それ以外ではデバッグサーバからデータを取得します。ただし、
  現在リリースサーバの用意はできていません。
・管理ツールをver1_0_0 beta3に更新しました。

■ 2008/04/16 1_0_0 beta 11

・管理ツールをver1_0_0 beta2に更新しました。

■ 2008/04/14 1_0_0 beta 10

・サーバからスケジュールとVIP情報を取得するようにしました。
・取得したスケジュールを通知する、PPW_LobbyNotifyScheduleCallbackコールバックを追
  加しました。
・取得したVIP情報を通知する、 PPW_LobbyGetVipCallbackコールバックを追加しました。
・サーバとの通信が多すぎる状態を通知する、PPW_LobbyExcessFloodCallbackコールバッ
  クを追加しました。
・VIP情報を参照する、PPW_LobbyGetVip関数を追加しました。
・スケジュールとVIPの設定を行える管理ツールを追加しました。

■ 2008/02/07 1_0_0 beta 9

・デモのミニゲーム募集方法を修正しました。
・サーバから切断されたときにライブラリを終了できなくなるバグを修正しました。
・チャンネルの人数制限が効かないバグを修正しました。
・システム定義プロフィールが変化していないのに
  PPW_LobbySystemProfileUpdatedCallbackコールバックが呼ばれるバグを修正しました。
・PPW_LobbySchedule構造体にroomFlagメンバを追加し、アルセウスフラグがセットされ
  るようにしました。

■ 2008/01/22 1_0_0 beta 8

・イベント(PPW_LOBBY_TIME_EVENT)の種類を資料PlaWifi080117に合わせました。
・同時刻にイベントが発生した際、PPW_LobbyScheduleProgressCallbackコールバックが
  同一フレーム内に発生するようにしました。
・対応ライブラリをアップデートしました。

■ 2008/01/10 1_0_0 beta 7

・内部のライブラリをlibdwcilobby.aへ分離しました。
・PPW_IsConnectedUserId関数のdwcFriendMatchCtrlパラメータをconstにしました。
・PPW_LobbyBinaryMassageReceivedCallbackコールバックのdataパラメータをconstに
  しました。
・二人以上チャンネルに参加したときに正しくREADY状態にならなくなるバグを修正し
  ました。(beta5から)
・ユーザが再接続したときにそのユーザに対して
  PPW_LobbyPlayerDisconnectedCallbackコールバックを呼んだ後、
  PPW_LobbyPlayerConnectedCallbackコールバックを呼ぶように修正しました。
・複数人での開発時に同一のチャンネルに入らないようにするために
  DWC_LOBBY_CHANNEL_PREFIX_DEBUGを10個に増やしました。
・PPW_LobbyEnumUserId関数が正しい結果を返さないバグを修正しました。
・エラーメッセージの表示について関数リファレンスに追記しました。

■ 2007/12/20 1_0_0 beta 6

・チャンネル自体にセットしたチャンネルデータを受信できないバグを修正しました。
・beta5でPPW_LobbyGetChannelDataCallbackのparamパラメータが常にNULLとなるバグ
  がありましたので修正しました。
・関数リファレンスで、PPW_LobbySystemProfile構造体、PPW_LobbyRecruitInfo構造体、
  PPW_LobbyTimeInfo構造体のメンバに関する説明がずれていたり抜けていたりしてい
  たのを修正しました。

■ 2007/12/19 1_0_0 beta 5

・PPW_LobbySendPlayer*Message関数で送信されたメッセージを受信すると止まるバグ
  を修正しました。
・自分自身がメインチャンネルに入ったときにもPPW_LobbyPlayerConnectedCallbackが
  呼ばれるようにしました。

■ 2007/12/14 1_0_0 beta 4

・PPW_LobbyStartRecruit関数が正しくmatchMakingStringをセットしないバグを修正し
  ました。
・指定したユーザIDの人とP2P接続しているかを調べる、PPW_IsConnectedUserId関数を
  追加しました。
・ミニゲームの募集を終了したときに呼び出されるコールバック関数型、
  PPW_LobbyRecruitStoppedCallbackを追加しました。
・メモリ確保に失敗したときにPanicで仮処理していたのをPanicしないよう修正しまし
  た。
・デモでミニゲームの募集からP2P通信までを行うようにしました。

■ 2007/12/05 1_0_0 beta 3

・メモリ使用量削減のためRTTIをオフにしました。

■ 2007/12/04 1_0_0 beta 2

・メモリ使用量削減のため例外機構をオフにしました。

■ 2007/11/30 1_0_0 beta 1

・初回リリース。
