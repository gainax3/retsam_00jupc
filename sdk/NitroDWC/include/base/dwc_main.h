/*---------------------------------------------------------------------------*
  Project:  NitroDWC -  - 
  File:     dwc_main.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_main.h,v $
  Revision 1.31  2006/04/27 07:06:09  takayama
  ingamesnを10文字から25文字に拡張。

  Revision 1.30  2006/01/23 04:50:43  arakit
  統一用語に合わせてコメントを修正

  Revision 1.29  2006/01/17 07:29:17  arakit
  ・DWC_SetConnectionClosedCallback()の戻り値をvoidからBOOLに変更し、
  　制御変数確保前は何もせずにFALSEを返すようにした。

  Revision 1.28  2005/12/21 03:08:54  arakit
  ログインの際にサーバにゲーム内スクリーンネームを渡せるようにした。

  Revision 1.27  2005/11/04 05:57:17  arakit
  関数説明のコメントを追加した。

  Revision 1.26  2005/11/02 02:49:33  arakit
  ファイルのインクルード関係を調整した。

  Revision 1.25  2005/10/28 09:10:20  arakit
  コメントを修正した。

  Revision 1.24  2005/10/26 09:06:35  arakit
  DWC_LoginAsync(), DWC_ConnectToAnybodyAsync(), DWC_ConnectToFriendsAsync(),
  DWC_SetupGameServerAsync(), DWC_ConnectToGameServerAsync() に戻り値を与えた。

  Revision 1.23  2005/10/05 05:19:27  arakit
  ・DWC_UpdateServersAsync()のコールバック内で、必ずStateをLOGINに上書きしてしまう
  　不具合を修正した。

  Revision 1.22  2005/09/26 05:06:08  arakit
  ・SBアップデート用のaddFilter文字列を、ライブラリ内部にコピーして持つように変更した。
  ・DWC_AddMatchKey*()のkey名の文字列も、ライブラリ内部にコピーして持つように変更した。

  Revision 1.21  2005/09/24 12:56:30  arakit
  サーバクライアント型マッチング時に、マッチング完了したホストのみを有効なAIDとして
  扱うよう、AIDやコネクション数へのアクセス関数を修正した。

  Revision 1.20  2005/09/21 05:21:26  arakit
  ・キャンセルを含めたクローズ処理のシステムを大きく変更した。
  ・コマンド送信のリトライ、タイムアウト処理を作成した。
  ・NATネゴシエーション失敗時のマッチングやり直し処理を追加した。
  ・gt2のソフトクローズを全てハードクローズに置き換えた。
  　それに伴いDWCi_CloseConnectionsAsync()を使わないようにした。
  （注）サーバクライアント型マッチングは正常動作しない

  Revision 1.19  2005/09/15 00:22:06  arakit
  ・DWC_CloseConnectionsAsync()に戻り値を追加した。
  ・コネクション強制切断関数DWC_CloseConnectionHard(u8 aid)と
  　int  DWC_CloseConnectionHardBitmap(u8 aid) を追加した。

  Revision 1.18  2005/09/13 11:10:56  arakit
  ・既にクローズされたaidに対して送信しようとしたらデータアクセス例外で
  　止まってしまう不具合を修正した。
  ・AIDビットマップ取得関数   u32 DWC_GetAIDBitmap( void )、
  　AID指定Reliable送信関数   u32 DWC_SendReliableBitmap( u32 bitmap, const void* buffer, int size )、
  　AID指定Unreliable送信関数 u32 DWC_SendUnreliableBitmap( u32 bitmap, const void* buffer, int size )
  　を用意した。

  Revision 1.17  2005/09/09 10:43:59  arakit
  SBとPersistentでDNSエラーが起こった際にリトライするようにした。

  Revision 1.16  2005/09/09 09:54:30  arakit
  バージョン違いのライブラリ同士でマッチングしないように変更した。

  Revision 1.15  2005/09/08 08:50:24  arakit
  ・ログイン時にgt2SocketとQR2を起動するように変更
  ・aidが重複して発行される不具合を修正

  Revision 1.14  2005/09/05 11:44:16  arakit
  ・DWCConnectionClosedCallbackの引数isServerは、自分がサーバクライアント型
  　マッチングのクライアントで、サーバがクローズした時のみTRUEを返すように
  　していたが、紛らわしいので、自分がサーバで自分がクローズしてもTRUEを
  　返すように変更した。
  ・サーバクライアント型マッチングで、クライアントがいなくなった時の
  　サーバの処理に不具合があったので、修正した。
  ・サーバクライアント型で、クローズ、キャンセル後にqr2_shutdown()が呼ばれて
  　いなかったのを修正した。

  Revision 1.13  2005/09/03 13:03:53  arakit
  GameSpyプロファイルIDを引数にとるようになっていたコールバック、
  DWCMatchedSCCallback, DWCNewClientCallback, DWCConnectionClosedCallback
  について、友達リストインデックスを返すように変更した。

  Revision 1.12  2005/08/31 10:29:46  arakit
  DWC制御構造体の型名をDWC_FriendsMatchControlに変更した。

  Revision 1.11  2005/08/31 06:04:52  arakit
  ソケットエラーを取得する関数DWC_GetLastSocketError()を追加した。

  Revision 1.10  2005/08/31 02:10:35  arakit
  友達指定マッチングで、友達の友達を許可するかどうかを選択できるようにした。

  Revision 1.9  2005/08/26 08:15:23  arakit
  ・ASSERTとモジュール制御変数のチェックを整理した。
  ・外部関数DWC_ShutdownFriendsMatch()の引数をとらないように変更した。
  また、同関数から各モジュールの制御変数をNULLクリアするようにした。
  ・エラー処理関数を変更し、エラーセット、エラーチェックを必要な箇所に入れた。

  Revision 1.8  2005/08/23 13:53:46  arakit
  ランダム関数にMATH_Rand*を使うようにした。

  Revision 1.7  2005/08/20 07:01:20  sasakit
  ヘッダインクルードガードの書式を統一した。
  bm/dwc_init.h -> bm/dwc_bm_init.hに変更
  ヘッダにCopyrightをつけた。
  構造体の名前空間をできるだけルールにちかづけた。
  util_wifiidtool.hとdwc_backup.hの関係を若干修正。


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_MAIN_H_
#define DWC_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------
// 登録済みバディへのバディ登録要求エラーを無視するためのスイッチ。
#define DWC_IGNORE_GP_ERROR_ALREADY_BUDDY
    
// GPバディメッセージのコマンド定数
//#define DWC_GP_COMMAND_STRING       "DWC_GPCM"  // GPメッセージコマンド判別文字列
#define DWC_GP_COMMAND_STRING       "GPCM"  // GPメッセージコマンド判別文字列
#define DWC_GP_COMMAND_MATCH_STRING "MAT"   // マッチメイク用メッセージ判別文字列

#define DWC_DNS_ERROR_RETRY_MAX 5  // DNSエラーのリトライ上限回数

// ダミーのリモート認証処理を行うためのスイッチ
//#define DWC_USE_DUMMY_REMOTE_AUTH

// プレイヤー名（ゲーム内スクリーンネーム）の最大長（ワイド文字25文字＋"\0\0"）
#define DWC_MAX_PLAYER_NAME 26


//----------------------------------------------------------------------------
// enum
//----------------------------------------------------------------------------
// DWCネットワーク状態列挙子
typedef enum {
    DWC_STATE_INIT = 0,         // 初期状態
    DWC_STATE_AVAILABLE_CHECK,  // ゲームの利用可能状況をチェック中
    DWC_STATE_LOGIN,            // ログイン処理中
    DWC_STATE_ONLINE,           // ログイン完了後オンライン状態
    DWC_STATE_UPDATE_SERVERS,   // 友達リスト同期処理中
    DWC_STATE_MATCHING,         // マッチメイク処理中
    DWC_STATE_CONNECTED,        // マッチメイク完了後、メッシュ型ネットワーク構築済み状態
    DWC_STATE_NUM
} DWCState;


//----------------------------------------------------------------------------
// typedef - function
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  コネクションクローズコールバック型
  引数　：error    DWCエラー種別
          isLocal  TRUE:自分がクローズした、FALSE:他の誰かがクローズした。
                   マッチメイク中に応答のないホストを切断する時もTRUEになる。
          isServer TRUE: サーバクライアントマッチメイクのサーバがクローズした。
                         自分がサーバでisLoacal = TRUE の時もTRUEになる。
                   FALSE:それ以外。サーバクライアントマッチメイク以外では
                         必ずFALSEとなる。
          aid      クローズしたプレイヤーのaid
          index    クローズしたプレイヤーの友達リストインデックス。
                   クローズしたプレイヤーが友達でなければ-1となる。
          param    コールバック用パラメータ
  戻り値：なし
 *---------------------------------------------------------------------------*/
typedef void (*DWCConnectionClosedCallback)(DWCError error, BOOL isLocal, BOOL isServer, u8 aid, int index, void* param);


//----------------------------------------------------------------------------
// struct
//----------------------------------------------------------------------------
// FriendsMatch制御構造体
typedef struct DWCstControl
{
    GT2Socket gt2Socket;   // 自ホストのGT2ソケット構造体へのポインタ
    GT2ConnectionCallbacks gt2Callbacks;  // gt2コネクションコールバック集
    int  gt2SendBufSize;   // GT2ソケットの送信バッファサイズ
    int  gt2RecvBufSize;   // GT2ソケットの受信バッファサイズ

    GPConnection gpObj;    // gpコネクション構造体へのポインタ
    DWCUserData* userdata; // DWCUserData構造体へのポインタ

    DWCState state;        // ネットワーク状態
    DWCState lastState;    // 前回のネットワーク状態
    u8   aid;              // 自分のAID（メッシュ型ネットワーク内での固有ID）
    u8   ownCloseFlag;     // TRUE:ゲーム側が自分でクローズした、FALSE:それ以外のクローズ
    u16  playerName[DWC_MAX_PLAYER_NAME];  // 他のユーザも参照する自分のプレイヤー名
    int  profileID;        // ユーザプロファイルID
    const char* gameName;  // GameSpyから与えられるゲーム名へのポインタ
    const char* secretKey; // GameSpyから与えられるシークレットキーへのポインタ

    DWCLoginCallback         loginCallback;          // ログイン完了コールバック
    void *loginParam;          // 上記コールバック用パラメータ
    DWCUpdateServersCallback updateServersCallback;  // 友達リスト同期処理完了コールバック
    void* updateServersParam;  // 上記コールバック用パラメータ
    DWCMatchedCallback       matchedCallback;        // ピアマッチメイク完了コールバック
    void* matchedParam;        // 上記コールバック用パラメータ
    DWCMatchedSCCallback     matchedSCCallback;      // サーバクライアントマッチメイク完了コールバック
    void* matchedSCParam;      // 上記コールバック用パラメータ
    DWCConnectionClosedCallback closedCallback;      // コネクションクローズコールバック
    void* closedParam;         // 上記コールバック用パラメータ

    DWCLoginControl logcnt;       // ログイン制御オブジェクト
    DWCFriendControl friendcnt;   // 友達管理オブジェクト
    DWCMatchControl matchcnt;     // マッチメイク制御オブジェクト
    DWCTransportInfo transinfo;   // トランスポート制御オブジェクト
} DWCFriendsMatchControl;

// GT2コネクション情報構造体
typedef struct DWCstConnectionInfo
{
    u8  index;    // 格納されているコネクションリストのインデックス
    u8  aid;      // ホスト固有ID
    u16 reserve;  // 予約
    void* param;  // ユーザデータ
} DWCConnectionInfo;


//----------------------------------------------------------------------------
// function - external
//----------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  FriendsMatchライブラリ初期化関数
  引数　：dwccnt        FriendsMatch制御オブジェクトへのポインタ
          userdata      ユーザデータオブジェクトへのポインタ
          productID     GameSpyから与えられるプロダクトID
          gameName      GameSpyから与えられたゲーム名（NULL終端必要）
          secretKey     GameSpyから与えられたシークレットキー（NULL終端必要）
          sendBufSize   DWC_Transportが使う送信バッファサイズ。0ならデフォルト8KByteを使用する。
          recvBufSize   DWC_Transportが使う受信バッファサイズ。0ならデフォルト8KByteを使用する。
          friendList    友達リスト（ゲームで使用しない場合はNULLでも良い）
          friendListLen 友達リストの最大長（要素数）
  戻り値：なし
  用途　：FriendsMatchライブラリを初期化する
 *---------------------------------------------------------------------------*/
extern void DWC_InitFriendsMatch(DWCFriendsMatchControl* dwccnt,
                                 DWCUserData* userdata,
                                 int productID,
                                 const char* gameName,
                                 const char* secretKey,
                                 int sendBufSize,
                                 int recvBufSize,
                                 DWCFriendData friendList[],
                                 int friendListLen);

/*---------------------------------------------------------------------------*
  FriendsMatchライブラリ終了関数
  引数　：なし
  戻り値：なし
  用途　：FriendsMatchライブラリを終了し、GameSpySDKのヒープ領域を開放する
 *---------------------------------------------------------------------------*/
extern void DWC_ShutdownFriendsMatch(void);


/*---------------------------------------------------------------------------*
  FriendsMatchライブラリ通信処理更新関数
  引数　：なし
  戻り値：なし
  用途　：毎ゲームフレーム呼び出し、FriendsMatchライブラリの通信処理を更新する
 *---------------------------------------------------------------------------*/
extern void DWC_ProcessFriendsMatch(void);


/*---------------------------------------------------------------------------*
  Wi-Fiコネクションログイン関数
  引数　：ingamesn ゲーム内スクリーンネーム
          reserved 過去の仕様。NULLを渡す。
          callback ログイン完了通知用コールバック関数
          param    コールバック用パラメータ
  戻り値：TRUE :関数呼び出し完了。成功か失敗でコールバックが返ってくる。
          FALSE:本関数を呼んで良い状態ではない。コールバックは返ってこない。
  用途　：ゲームが利用可能であるか調べ、リモート認証、GPサーバへの接続を行う
 *---------------------------------------------------------------------------*/
// [arakit] main 051025
extern BOOL DWC_LoginAsync(const u16*  ingamesn,
                           const char* reserved,
                           DWCLoginCallback callback,
                           void* param);


/*---------------------------------------------------------------------------*
  友達リスト同期処理関数
  引数　：playerName     他のユーザも参照する自分のプレイヤー名
                         →過去の仕様です。現在はセットされた値を無視しています。
          updateCallback 友達リスト同期処理完了コールバック
          updateParam    上記コールバック用パラメータ
          statusCallback 友達状態変化通知コールバック
          statusParam    上記コールバック用パラメータ
          deleteCallback 友達リスト削除コールバック
          deleteParam    上記コールバック用パラメータ
  戻り値：TRUE :関数呼び出し完了。成功か失敗でコールバックが返ってくる。
          FALSE:本関数を呼んで良い状態ではない。コールバックは返ってこない。
  用途　：ログイン完了後、GPサーバ上の友達リスト（バディリスト）と
          ローカルの友達リストの同期処理を行う
 *---------------------------------------------------------------------------*/
extern BOOL DWC_UpdateServersAsync(const char* playerName,
                                   DWCUpdateServersCallback updateCallback,
                                   void* updateParam,
                                   DWCFriendStatusCallback statusCallback,
                                   void* statusParam,
                                   DWCDeleteFriendListCallback deleteCallback,
                                   void* deleteParam);


/*---------------------------------------------------------------------------*
  友達無指定ピアマッチメイク開始関数
  引数　：numEntry        要求するネットワーク構成人数（自分を含む）
          addFilter       ゲームで追加したいマッチメイク条件文字列。
                          条件を追加しない場合はNULLを渡す。
                          条件はスタンダードなSQLの書式で書くことができます。
                          今のところ設定できる文字列長は最大127文字で、
                          デバッグビルドでは文字数オーバチェックをしています。
                          文字列はコピーしてライブラリ内で保持します。
          matchedCallback マッチメイク完了コールバック
          matehedParam    上記コールバック用パラメータ
          evalCallback    プレイヤー評価コールバック
          evalParam       上記コールバック用パラメータ
  戻り値：TRUE :関数呼び出し完了。マッチメイクの結果がコールバックで返ってくる。
          FALSE:本関数を呼んで良い状態ではない。コールバックは返ってこない。
  用途　：友達を指定せずに、人数指定でメッシュ型ネットワークを作成する
 *---------------------------------------------------------------------------*/
// [arakit] main 051025
extern BOOL DWC_ConnectToAnybodyAsync(u8  numEntry,
                                      const char* addFilter,
                                      DWCMatchedCallback matchedCallback,
                                      void* matchedParam,
                                      DWCEvalPlayerCallback evalCallback,
                                      void* evalParam);


/*---------------------------------------------------------------------------*
  友達指定ピアマッチメイク開始関数
  引数　：friendIdxList    接続要求友達インデックスリスト。
                           NULLなら友達リスト全てを接続要求対象とする。
          friendIdxListLen 接続要求友達インデックスリスト長
          numEntry         要求するネットワーク構成人数（自分を含む）
          distantFriend    TRUE:友達の友達との接続を許す、FALSE:許さない
          matchedCallback  マッチメイク完了コールバック
          matchedParam     上記コールバック用パラメータ
          evalCallback     プレイヤー評価コールバック
          evalParam        上記コールバック用パラメータ
  戻り値：TRUE :関数呼び出し完了。マッチメイクの結果がコールバックで返ってくる。
          FALSE:本関数を呼んで良い状態ではない。コールバックは返ってこない。
  用途　：友達を指定して接続し、メッシュ型ネットワークを作成する
 *---------------------------------------------------------------------------*/
// [arakit] main 051025
extern BOOL DWC_ConnectToFriendsAsync(const u8 friendIdxList[],
                                      int friendIdxListLen,
                                      u8  numEntry,
                                      BOOL distantFriend,
                                      DWCMatchedCallback matchedCallback,
                                      void* matchedParam,
                                      DWCEvalPlayerCallback evalCallback,
                                      void* evalParam);


/*---------------------------------------------------------------------------*
  サーバクライアントマッチメイクのサーバ起動関数
  引数　：maxEntry          最大接続人数（自分を含む）
          matchedCallback   接続完了コールバック。１人接続するごとに呼ばれる。
          matchedParam      上記コールバック用パラメータ
          newClientCallback 新規接続クライアント通知コールバック
          newClientParam    上記コールバック用パラメータ
  戻り値：TRUE :関数呼び出し完了。マッチメイクの結果がコールバックで返ってくる。
          FALSE:本関数を呼んで良い状態ではない。コールバックは返ってこない。
  用途　：サーバを立ててクライアントからの要求があれば、コネクションの
          接続・切断処理を行う。
 *---------------------------------------------------------------------------*/
// [arakit] main 051025 051027
extern BOOL DWC_SetupGameServer(u8  maxEntry,
                                DWCMatchedSCCallback matchedCallback,
                                void* matchedParam,
                                DWCNewClientCallback newClientCallback,
                                void* newClientParam);

// 内容を正しく表す関数名に変更しましたが、旧名DWC_SetupGameServersAsync()も使えます
#define DWC_SetupGameServerAsync DWC_SetupGameServer
// [arakit] main 051027


/*---------------------------------------------------------------------------*
  サーバクライアントマッチメイクのクライアント起動関数
  引数　：serverIndex       接続先サーバの友達リストインデックス
          matchedCallback   接続完了コールバック。１人接続するごとに呼ばれる。
          matchedParam      上記コールバック用パラメータ
          newClientCallback 新規接続クライアント通知コールバック
          newClientParam    上記コールバック用パラメータ
  戻り値：TRUE :関数呼び出し完了。マッチメイクの結果がコールバックで返ってくる。
          FALSE:本関数を呼んで良い状態ではない。コールバックは返ってこない。
  用途　：友達リストのインデックスで接続するサーバを指定し、そこへ接続する。
          また、サーバへ新たに接続したクライアントが
          あれば、そのクライアントとの接続処理を行う。
 *---------------------------------------------------------------------------*/
// [arakit] main 051025
extern BOOL DWC_ConnectToGameServerAsync(int serverIndex,
                                         DWCMatchedSCCallback matchedCallback,
                                         void* matchedParam,
                                         DWCNewClientCallback newClientCallback,
                                         void* newClientParam);


/*---------------------------------------------------------------------------*
  コネクションクローズコールバック設定関数
  引数　：callback コネクション１つをクローズされる度に呼び出されるコールバック
          param    上記コールバック用パラメータ
  戻り値：TRUE :登録成功
          FALSE:FriendsMatchライブラリ非動作中で登録失敗
  用途　：コネクションクローズコールバックを設定する
 *---------------------------------------------------------------------------*/
extern BOOL DWC_SetConnectionClosedCallback(DWCConnectionClosedCallback callback, void* param);


/*---------------------------------------------------------------------------*
  全コネクション強制クローズ関数
  引数　：なし
  戻り値：0以上なら成功、負の数なら失敗。
          0 :クローズ実行。
          1 :接続ホスト数0なので、クローズ処理は行ったが、コールバックは呼ばれない。
             サーバクライアントマッチメイクのサーバの場合のみ。
          -1:接続完了後でない、もしくはエラー発生時なので何もしなかった。
  用途　：接続中のコネクションを全てクローズする。
          クローズ処理は本関数内で完了し、本関数を抜ける前に、
          １つのコネクションをクローズするごとに、
          DWC_SetConnectionClosedCallback()で設定したコールバック関数が
          呼び出される。Wi-Fiコネクションにはログインしたままとなる。
          このクローズは相手ホストにも通知される。
          サーバクライアントマッチメイクのサーバにおいて、既に接続中のホストが
          ない場合は、終了処理を行うだけで、コールバックは呼ばれない。
 *---------------------------------------------------------------------------*/
extern int  DWC_CloseAllConnectionsHard(void);


/*---------------------------------------------------------------------------*
  コネクション強制クローズ関数
  引数　：aid クローズしたいホストのAID
  戻り値：0 :クローズ実行。
          -1:接続完了後でない、もしくはエラー発生時なので何もしなかった。
          -2:既にクローズ済みのAIDが指定されたので何もしなかった。
  用途　：指定のAIDとのコネクションを強制クローズする。
          クローズ処理は本関数内で完了し、本関数を抜ける前に
          DWC_SetConnectionClosedCallback()で設定したコールバック関数が
          呼び出される。Wi-Fiコネクションにはログインしたままとなる。
          このクローズは相手ホストにも通知される。
          電源を切るなどの理由で通信不能になったホストに対し、コネクションを
          クローズするというような、異常状態処理の用途でのみご使用下さい。
 *---------------------------------------------------------------------------*/
extern int  DWC_CloseConnectionHard(u8 aid);


/*---------------------------------------------------------------------------*
  ビットマップ指定コネクション強制クローズ関数
  引数　：bitmap クローズしたいホストのAIDビットマップへのポインタ。
                 実際にクローズに成功したAIDのビットのみを立てて返す。
                 自分のビットは立っていても必ず下ろす。
  戻り値：0 :クローズ開始。クローズ処理が完了する度にコールバックが呼ばれる。
          -1:接続完了後でない、もしくはエラー発生時なので何もしなかった。
          -2:全てのAIDが既にクローズ済みだったので何もしなかった。
  用途　：指定のAIDとのコネクションを強制クローズする。
          クローズ処理は本関数内で完了し、本関数を抜ける前に
          DWC_SetConnectionClosedCallback()で設定したコールバック関数が
          呼び出される。Wi-Fiコネクションにはログインしたままとなる。
          このクローズは相手ホストにも通知される。
          電源を切るなどの理由で通信不能になったホストに対し、コネクションを
          クローズするというような、異常状態処理の用途でのみご使用下さい。
 *---------------------------------------------------------------------------*/
extern int  DWC_CloseConnectionHardBitmap(u32* bitmap);


/*---------------------------------------------------------------------------*
  接続ホスト数取得関数
  引数　：なし
  戻り値：ネットワークを構成しているホスト数（自分を含む）。
          オフライン時は0を返す。
  用途　：メッシュ型ネットワーク構成ホスト数を返す
 *---------------------------------------------------------------------------*/
extern int DWC_GetNumConnectionHost(void);


/*---------------------------------------------------------------------------*
  自分のAID取得関数
  引数　：なし
  戻り値：自分のAID
  用途　：自分のAID（メッシュ型ネットワーク内で固有のホストID）を返す
 *---------------------------------------------------------------------------*/
extern u8  DWC_GetMyAID(void);


/*---------------------------------------------------------------------------*
  接続中ホストのAIDリスト取得関数
  引数　：aidList AIDリストへのポインタ
  戻り値：AIDリスト長（自分を含む）
  用途　：AIDリストへのポインタを取得する（コピーはしない）。
          AIDリストは、現在有効なAIDが前から順に詰められたリストで、
          その要素数は自分も含めた接続中ホスト数になる。
 *---------------------------------------------------------------------------*/
extern int DWC_GetAIDList(u8** aidList);


/*---------------------------------------------------------------------------*
  接続中ホストのAIDビットマップ取得関数
  引数　：なし
  戻り値：AIDビットマップ。オフライン時は0を返す。
  用途　：接続中ホストのAIDビットマップを取得する。
 *---------------------------------------------------------------------------*/
extern u32 DWC_GetAIDBitmap(void);


/*---------------------------------------------------------------------------*
  AID有効判定関数
  引数　：aid AID
  戻り値：TRUE:AIDが有効、FALSE:AIDが無効
  用途　：指定されたAIDが有効かどうか（自分に接続されているかどうか）を調べる。
          自分のAIDを指定した場合はFALSEを返す。
          マッチメイク中の新規接続クライアントのAIDは無効と判定する。
 *---------------------------------------------------------------------------*/
extern BOOL DWC_IsValidAID(u8 aid);


/*---------------------------------------------------------------------------*
  FriendsMatchライブラリ状態取得関数
  引数　：なし
  戻り値：DWCState型列挙子
  用途　：FriendsMatchライブラリの処理状態を取得する
 *---------------------------------------------------------------------------*/
extern DWCState DWC_GetState(void);


/*---------------------------------------------------------------------------*
  ソケットエラー取得関数
  引数　：なし
  戻り値：GT2ソケットエラー種別
  用途　：GameSpy gt2 SDK のソケットエラー種別を取得する
 *---------------------------------------------------------------------------*/
extern int DWC_GetLastSocketError(void);


/*---------------------------------------------------------------------------*
  全コネクションクローズ関数  (obsolete function)
  引数　：なし
  戻り値：0以上なら成功、負の数なら失敗。
          0 :クローズ開始。クローズ処理が完了したらコールバックが呼ばれる。
          1 :接続ホスト数0なので、クローズ処理は行ったが、コールバックは呼ばれない。
             サーバクライアントマッチメイクのサーバの場合のみ。
          -1:接続完了後でない、もしくはエラー発生時なので何もしなかった。
  用途　：接続中のコネクションを全てクローズする。
          １つのコネクションをクローズするごとに、
          DWC_SetConnectionClosedCallback()で設定したコールバック関数が
          呼び出される。Wi-Fiコネクションにはログインしたままとなる。
          このクローズは相手ホストにも通知される。
          サーバクライアントマッチメイクのサーバにおいて、既に接続中のホストが
          ない場合は、終了処理を行うだけで、コールバックは呼ばれない。
          →DWC_CloseAllConnectionsHard()を使って下さい
 *---------------------------------------------------------------------------*/
extern int DWC_CloseConnectionsAsync(void);


//----------------------------------------------------------------------------
// function - internal
//----------------------------------------------------------------------------
extern GT2Result DWCi_GT2Startup(void);


extern GT2Connection DWCi_GetGT2Connection(u8 aid);


extern u8  DWCi_GetConnectionAID(GT2Connection connection);


extern u8  DWCi_GetConnectionIndex(GT2Connection connection);


extern void* DWCi_GetConnectionUserData(GT2Connection connection);


extern int  DWCi_GT2GetConnectionListIdx(void);


extern void DWCi_ClearGT2ConnectionList(void);


extern GT2Connection* DWCi_GetGT2ConnectionByIdx(int index);


extern GT2Connection* DWCi_GetGT2ConnectionByProfileID(int profileID, int numHost);


extern DWCConnectionInfo* DWCi_GetConnectionInfoByIdx(int index);


extern BOOL DWCi_IsValidAID(u8 aid);


#ifdef __cplusplus
}
#endif


#endif // DWC_MAIN_H_
