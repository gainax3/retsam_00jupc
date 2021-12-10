/*---------------------------------------------------------------------------*
  Project:  NitroDWC -  - 
  File:     dwc_friend.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_friend.h,v $
  Revision 1.32  2006/02/27 02:19:17  nakata
  Base64関連のラッパ関数の引数型を修正。

  Revision 1.31  2006/01/25 07:50:50  arakit
  データストレージサーバ・ロード関数DWC_LoadOwnPublicDataAsync(),
  DWC_LoadOwnPrivateDataAsync()を追加した。

  Revision 1.30  2006/01/23 04:50:43  arakit
  統一用語に合わせてコメントを修正

  Revision 1.29  2006/01/17 07:17:25  arakit
  ・DWCFriendStatusCallbackの設定のみを行う関数 DWC_SetFriendStatusCallback()
  　を作成した。
  ・DWC_SetBuddyFriendCallback(), DWC_SetStorageServerCallback() の戻り値を
  　voidからBOOLに変更し、制御変数確保前は何もせずにFALSEを返すようにした。
  ・Base64エンコード・デコード関数（Authのラップ関数）DWC_Base64Encode(),
  　DWC_Base64Decode()を作成した。

  Revision 1.28  2005/12/21 03:08:32  arakit
  制御構造体のメンバplayerNameのフォーマットの変更に対応した。

  Revision 1.27  2005/11/07 02:54:59  sasakit
  GsStatus->OwnStatusに変更。

  Revision 1.26  2005/11/04 11:36:44  arakit
  DWCLoadFromServerCallbackの引数profileIDを、友達リストインデックス（index）に変更した。

  Revision 1.25  2005/11/04 05:44:59  arakit
  スペルミスを修正した。

  Revision 1.24  2005/11/04 05:07:18  arakit
  Persistentサーバへのログイン、ログアウト関数を追加し、Persistentサーバへの
  アクセスを可能にした。

  Revision 1.23  2005/11/02 02:49:33  arakit
  ファイルのインクルード関係を調整した。

  Revision 1.22  2005/10/28 09:11:05  arakit
  関数説明の「～通信中に突然電源を切った場合などは10秒近く前の状態が残ります」
  というコメントは、実情を表していないので修正した。

  Revision 1.21  2005/10/06 07:00:54  arakit
  関数の説明文を若干修正

  Revision 1.20  2005/10/03 04:05:04  arakit
  gpのTCP接続が切られないようにキープアライブ代わりの関数呼び出しを追加した。

  Revision 1.19  2005/10/01 07:14:07  arakit
  友達リスト編集可能状態取得関数 BOOL DWC_CanChangeFriendList(void) を追加した。

  Revision 1.18  2005/09/28 05:17:28  arakit
  友達データを消去し、バディ関係も解消する関数DWC_DeleteBuddyFriendData()を作成した。

  Revision 1.17  2005/09/27 13:01:42  arakit
  使わなくなった定数を削除した。

  Revision 1.16  2005/09/20 10:03:40  sasakit
  gpSendBuddyRequest()の返答処理をdwc_friend.cに移した。

  Revision 1.15  2005/09/16 07:26:40  arakit
  コールバックの引数名を変更

  Revision 1.14  2005/09/13 11:12:22  arakit
  ・MP友達データからProfileSearchを行った時に、相手が見つからなかった場合の
  　不具合を修正した。
  ・DWC_UpdateServersAsync()処理中以外にバディ成立したら返すコールバックを
  　設定できるようにした。

  Revision 1.13  2005/09/12 07:50:25  arakit
  ・gpのAlloc対策として、ProfileSearchを１人ずつ行うようにした。
  ・オンライン中にバディ関係の成立が完了するよう、修正した。
  ・友達リスト消去コールバックの引数に、消されたものと同じと判定された友達データの、
  　友達リストインデックスも渡すようにした。

  Revision 1.12  2005/09/09 09:54:09  arakit
  バージョン違いのライブラリ同士でマッチングしないように変更した。

  Revision 1.11  2005/09/05 11:43:31  arakit
  ・Persistentサーバロード関数DWC_LoadOthersDataAsync()の引数profaileIDを
  　友達リストインデックスに変更した。
  ・gpStatus文字列のうち、ライブラリがlocationString、ゲームがstatusStringを
  　使用するようにしていたが、statusStringはデフォルトで"Offline"の文字が
  　入ってしまうので、ライブラリがstatusString、ゲームがlocationStringを
  　使用するように入れ替えた。
  ・バイナリデータをテキストエンコードしてgpStatusのlocationStringに
  　セットする関数DWC_SetGsStatusData()、そのデータをエンコードして読む関数
  　DWC_GetGsStatusData()と、友達のStatusバイナリデータを読む関数
  　DWC_GetFriendStatusData(), DWC_GetFriendStatusDataSC()を追加した。

  Revision 1.10  2005/09/03 13:03:35  arakit
  ・friendList操作系外部関数 DWC_SetFriendDataToList(), DWC_SetGsProfileIDToList(),
  　DWC_DeleteFriendFromList()を消去した。
  ・GameSpyプロファイルIDを直接扱う関数 DWC_GetProfileIDFromList(),
  　DWC_GetFriendListIndex() を内部関数に変更した。
  ・DWC_GetFriendStatus(), DWC_GetFriendStatusSC() の引数を、「友達リストへの
  　ポインタ＋友達リスト長」から、「友達データへのポインタ」に変更した。
  ・友達リスト同期更新時に友達データが消去されても、リストを前詰めしないように
  　変更した。それに伴い、前詰めを前提としていた処理を修正した。
  ・MP交換友達データから友達のGameSpyプロファイルIDを取得する際に、
  　友達がまだ一度もログインしていなかった時に不具合が発生していたのを修正した。
  ・バディリクエスト受信時の承認処理を、最終仕様に合わせた。

  Revision 1.9  2005/08/29 06:35:56  arakit
  ・gpStatusのstatusStringをゲーム側で、locationStringをライブラリ側で
  読み書きできるようにした。
  ・サーバクライアント型マッチングで、最大接続人数、現在接続人数をgpStatusの
  locationStringにセットするようにし、その値を参照するための関数を用意した。

  Revision 1.8  2005/08/26 08:15:22  arakit
  ・ASSERTとモジュール制御変数のチェックを整理した。
  ・外部関数DWC_ShutdownFriendsMatch()の引数をとらないように変更した。
  また、同関数から各モジュールの制御変数をNULLクリアするようにした。
  ・エラー処理関数を変更し、エラーセット、エラーチェックを必要な箇所に入れた。

  Revision 1.7  2005/08/23 13:39:20  arakit
  ・外部関数DWC_HasProflieIDInList(), DWC_HasOnlyMPFriendKeyInList()を消去した。
  ・外部関数DWC_GetProfileIDFromfList()と一部内部関数を、ユーザデータを引数に
  取るように変更した。

  Revision 1.6  2005/08/20 07:01:19  sasakit
  ヘッダインクルードガードの書式を統一した。
  bm/dwc_init.h -> bm/dwc_bm_init.hに変更
  ヘッダにCopyrightをつけた。
  構造体の名前空間をできるだけルールにちかづけた。
  util_wifiidtool.hとdwc_backup.hの関係を若干修正。


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_FRIEND_H_
#define DWC_FRIEND_H_


#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------
// GameSpy Status文字列最大長（NULL終端含む）。256文字。
#define DWC_GS_STATUS_STRING_LEN GP_LOCATION_STRING_LEN

// DWC制御下のPersistetデータのセーブ・ロードであることを表すフラグ
#define DWC_PERS_KEY_ID_MASK         0x80000000
#define DWC_PERS_KEY_ID_PLAYER_NAME  (DWC_PERS_KEY_ID_MASK | 0x01)

// gpProcess()呼び出し間隔（単位：msec）
#define DWC_GP_PROCESS_INTERVAL      300

// 友達リスト同期処理を開始するまでのgpProcess()呼び出し回数
#define DWC_FRIEND_UPDATE_WAIT_COUNT 7    // 300×7 = 2.1sec

// GPキープアライブ送信gpProcess()呼び出し回数
#define DWC_GP_KEEP_ALIVE_COUNT      256  // 300×256 = 76.8sec

// gpStatusのstatusStringに設定する、key文字列（必ず３文字）
#define DWC_GP_SSTR_KEY_MATCH_VERSION    "VER"  // マッチメイクのプロトコルバージョン
#define DWC_GP_SSTR_KEY_MATCH_FRIEND_NUM "FME"  // 友達指定ピアマッチメイクの希望接続人数
#define DWC_GP_SSTR_KEY_DISTANT_FRIEND   "MDF"  // 友達指定ピアマッチメイクで友達の友達を許すかどうか
#define DWC_GP_SSTR_KEY_MATCH_SC_MAX     "SCM"  // サーバクライアントマッチメイクの最大接続人数
#define DWC_GP_SSTR_KEY_MATCH_SC_NUM     "SCN"  // サーバクライアントマッチメイクの現在接続人数

// gpStatusのstatusを変更しないことを表すstatus値
#define DWC_GP_STATUS_NO_CHANGE -1

    
//----------------------------------------------------------------------------
// enum
//----------------------------------------------------------------------------
// 友達の通信状態（GPEnumのStatus＋αを再定義して使用）
enum {
    DWC_STATUS_OFFLINE = 0,    // オフライン
    DWC_STATUS_ONLINE,         // オンライン（GPサーバにログイン済み）
    DWC_STATUS_PLAYING,        // ゲームプレイ中
    DWC_STATUS_MATCH_ANYBODY,  // 友達無指定ピアマッチメイク中
    DWC_STATUS_MATCH_FRIEND,   // 友達指定ピアマッチメイク中
    DWC_STATUS_MATCH_SC_CL,    // サーバクライアントマッチメイク中のクライアント
    DWC_STATUS_MATCH_SC_SV,    // サーバクライアントマッチメイク中のサーバ
    DWC_STATUS_NUM
};
    
// 友達管理処理状態
typedef enum {
    DWC_FRIEND_STATE_INIT = 0,      // 初期状態
    DWC_FRIEND_STATE_PERS_LOGIN,    // Persistentサーバログイン＆友達リスト同期処理中
    DWC_FRIEND_STATE_LOGON,         // Persistentサーバログイン後
    DWC_FRIEND_STATE_NUM
} DWCFriendState;

// ローカル友達リストとGPバディリストとの同期処理状態
enum {
    DWC_BUDDY_UPDATE_STATE_WAIT = 0,  // バディリストダウンロード完了待ち
    DWC_BUDDY_UPDATE_STATE_CHECK,     // バディリストチェック中
    DWC_BUDDY_UPDATE_STATE_PSEARCH,   // プロファイルサーチ中
    DWC_BUDDY_UPDATE_STATE_COMPLETE,  // バディリスト同期処理完了
    DWC_BUDDY_UPDATE_STATE_NUM
};

// Persistentサーバログイン状態
enum {
    DWC_PERS_STATE_INIT = 0,   // 初期状態
    DWC_PERS_STATE_LOGIN,      // ログイン中
    DWC_PERS_STATE_CONNECTED,  // 接続状態
    DWC_PERS_STATE_NUM
};


//----------------------------------------------------------------------------
// typedef - function
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  友達リスト同期処理コールバック型
  引数　：error     DWCエラー種別
          isChanged TRUE :同期処理中に友達リストが書き換えられた、
                    FALSE:友達リストの書き換えなし
          param     コールバック用パラメータ
  戻り値：なし
 *---------------------------------------------------------------------------*/
typedef void (*DWCUpdateServersCallback)(DWCError error, BOOL isChanged, void* param);

/*---------------------------------------------------------------------------*
  友達状態変化通知コールバック型
  引数　：index        状態が変化した友達の友達リストインデックス
          status       友達の状態。DWC_STATUS_* の列挙子。
          statusString ゲームで定義したGameSpy status文字列。
                       文字列は最大255文字＋NULL終端。
          param  コールバック用パラメータ
  戻り値：なし
 *---------------------------------------------------------------------------*/
typedef void (*DWCFriendStatusCallback)(int index, u8 status, const char* statusString, void* param);

/*---------------------------------------------------------------------------*
  友達リスト削除コールバック型
  引数　：deletedIndex 削除された友達の友達リストインデックス
          srcIndex     同じ友達情報と判定された友達リストインデックス
          param        コールバック用パラメータ
  戻り値：なし
  DWC_UpdateServersAsync()の処理中に、友達リスト中に同じ友達情報を見つけ、
  ライブラリ側で消去した時に呼ばれる
 *---------------------------------------------------------------------------*/
typedef void (*DWCDeleteFriendListCallback)(int deletedIndex, int srcIndex, void* param);

/*---------------------------------------------------------------------------*
  バディ成立コールバック型
  引数　：index GPサーバ上でバディ関係が成立した友達の、友達リストインデックス
          param コールバック用パラメータ
  戻り値：なし
 *---------------------------------------------------------------------------*/
typedef void (*DWCBuddyFriendCallback)(int index, void* param);

/*---------------------------------------------------------------------------*
  データストレージサーバ・ログインコールバック型
  引数　：error DWCエラー種別
          param コールバック用パラメータ
  戻り値：なし
 *---------------------------------------------------------------------------*/
typedef void (*DWCStorageLoginCallback)(DWCError error, void* param);

/*---------------------------------------------------------------------------*
  データストレージサーバ・セーブ完了コールバック型
  引数　：success  TRUE:セーブ成功、FALSE:セーブ失敗
          isPublic TRUE:セーブしたデータがパブリックデータ、FALSE:プライベートデータ
          param  コールバック用パラメータ
  戻り値：なし
 *---------------------------------------------------------------------------*/
typedef void (*DWCSaveToServerCallback)(BOOL success, BOOL isPublic, void* param);

/*---------------------------------------------------------------------------*
  データストレージサーバ・ロード完了コールバック型
  引数　：success   TRUE:ロード成功、FALSE:ロード失敗
          index     ロード元データ保持者の友達リストインデックス。
                    自分の場合と、ロード完了前に友達でなくなった場合は-1になる。
          data      ロードしたデータ（key/value組の文字列）
          len       ロードデータ長
          param     コールバック用パラメータ
  戻り値：なし
  ロード関数で指定したキーが存在しなかった場合や、他人がDWC_SavePrivateDataAsync()
  でセーブしたキーを読もうとした場合も success = FALSE になる。
 *---------------------------------------------------------------------------*/
typedef void (*DWCLoadFromServerCallback)(BOOL success, int index, char* data, int len, void* param);


//----------------------------------------------------------------------------
// struct
//----------------------------------------------------------------------------
// 友達リスト用共用体
#define DWC_FRIEND_DATA_SIZE 16  // 友達リストサイズ（単位：バイト）（今は使っていない）
typedef union DWCunFriendDataOld
{
    u8  mpFriendKey[DWC_FRIEND_DATA_SIZE];  // MP友達交換鍵
    struct {
        int id;                             // プロファイルID（非暗号化状態）
        u8  pad[DWC_FRIEND_DATA_SIZE-sizeof(int)];  // パディング
    } profile;
    u8  flags[DWC_FRIEND_DATA_SIZE];  // フラグ参照用（[15]の上位2bitがフラグ）
} DWCFriendDataOld;

typedef struct DWCstFriendControl
{
    DWCFriendState state;    // 友達管理処理状態
        
    GPConnection* pGpObj;    // gpコネクション構造体ポインタへのポインタ
    u32 gpProcessCount;      // gpProcess呼び出し回数
    OSTick lastGpProcess;    // gpProcessを最後に呼び出した時間
        
    int friendListLen;       // 友達リスト最大長
    DWCFriendData* friendList;  // 友達リスト

    u8  buddyUpdateIdx;      // 友達リスト同期処理中インデックス
    u8  friendListChanged;   // 1:フレンドリストに変更があった、0:なかった
    u8  buddyUpdateState;    // 友達リスト同期処理状態。DWC_BUDDY_UPDATE_STATE_*列挙子で定義。
    vu8 svUpdateComplete;    // 友達リスト同期処理完了フラグ。2で同期処理完了
    u32 persCallbackLevel;   // Persistentデータセーブ・ロードコールバック待ち件数
    int profileID;           // 自分のプロファイルID
    const u16* playerName;   // 他のユーザも参照する自分のプレイヤー名へのポインタ

    DWCUpdateServersCallback updateCallback;     // 友達リスト同期処理完了コールバック
    void* updateParam;       // 上記コールバック用パラメータ
    DWCFriendStatusCallback statusCallback;      // 友達状態変化通知コールバック
    void* statusParam;       // 上記コールバック用パラメータ
    DWCDeleteFriendListCallback deleteCallback;  // 友達リスト削除コールバック
    void* deleteParam;       // 上記コールバック用パラメータ
    DWCBuddyFriendCallback buddyCallback;        // バディ関係成立コールバック
    void* buddyParam;        // 上記コールバック用パラメータ
    DWCStorageLoginCallback persLoginCallback;   // Persistentサーバログイン完了通知コールバック
    void* persLoginParam;    // 上記コールバック用パラメータ
    DWCSaveToServerCallback saveCallback;        // Persistentサーバセーブコールバック
    DWCLoadFromServerCallback loadCallback;      // Persistentサーバロードコールバック
} DWCFriendControl;


//----------------------------------------------------------------------------
// function - external
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  友達通信状態取得関数
  引数　：friendData   友達情報へのポインタ
          statusString ゲームで定義したGameSpy status文字列格納先ポインタ。
                       NULLを渡せば文字列のコピーは行わない。
                       文字列は最大255文字＋NULL終端。
  戻り値：通信状態enum値（dwc_friend.h のDWC_STATUS_*）。
          自分がオフラインの時などに呼び出すとDWC_STATUS_OFFLINEしか返さない。
          また相手がまだGPのバディリストに登録されていない時も
          DWC_STATUS_OFFLINE返す。
  用途　：ローカルにキャッシュされた、指定の友達の通信状態を取得する。
  　　　　通信は行わない。
          友達の通信状態は、変化がある度にGPサーバから通知される。
          ただし、通信中に突然電源を切った場合などは数分間は前の状態が残る。
 *---------------------------------------------------------------------------*/
extern u8   DWC_GetFriendStatus(const DWCFriendData* friendData, char* statusString);


/*---------------------------------------------------------------------------*
  友達通信状態取得関数（サーバクライアントマッチメイクゲーム用）
  引数　：friendData   友達情報へのポインタ
          maxEntry     友達がサーバクライアントマッチメイクのサーバを立てて
                       いる場合の最大接続人数。
                       格納先ポインタ。NULLを渡せば取得しない。
          numEntry     友達がサーバクライアントマッチメイクのサーバを立てて
                       いる場合の現在接続人数。
                       格納先ポインタ。NULLを渡せば取得しない。
          statusString ゲームで定義したGameSpy status文字列格納先ポインタ。
                       NULLを渡せば文字列のコピーは行わない。
                       文字列は最大255文字＋NULL終端。
  戻り値：通信状態enum値（dwc_friend.h のDWC_STATUS_*）。
          自分がオフラインの時などに呼び出すとDWC_STATUS_OFFLINEしか返さない。
          また相手がまだGPのバディリストに登録されていない時も
          DWC_STATUS_OFFLINE返す。
  用途　：ローカルにキャッシュされた、指定の友達の通信状態を取得する。
  　　　　通信は行わない。
          友達の通信状態は、変化がある度にGPサーバから通知される。
          ただし、通信中に突然電源を切った場合などは数分間は前の状態が残る。
 *---------------------------------------------------------------------------*/
extern u8   DWC_GetFriendStatusSC(const DWCFriendData* friendData, u8* maxEntry, u8* numEntry, char* statusString);


/*---------------------------------------------------------------------------*
  友達通信状態取得関数（statusData版）
  引数　：friendData 友達情報へのポインタ
          statusData ゲームで定義したGameSpy statusデータ格納先ポインタ。
                     NULLを渡せばデータのコピーは行わない。
          size       ゲームで定義したGameSpy statusデータ長の格納先ポインタ。
                     無効なデータの場合は-1が格納される。
  戻り値：通信状態enum値（dwc_friend.h のDWC_STATUS_*）。
          自分がオフラインの時などに呼び出すとDWC_STATUS_OFFLINEしか返さない。
          また相手がまだGPのバディリストに登録されていない時も
          DWC_STATUS_OFFLINE返す。
  用途　：ローカルにキャッシュされた、指定の友達の通信状態を取得する。
  　　　　通信は行わない。
          友達の通信状態は、変化がある度にGPサーバから通知される。
          ただし、通信中に突然電源を切った場合などは数分間は前の状態が残る。
          戻り値がDWC_STATUS_OFFLINEの場合は、*size = -1 をセットする以外は
          値をセットせずに返します。
 *---------------------------------------------------------------------------*/
extern u8 DWC_GetFriendStatusData(const DWCFriendData* friendData, char* statusData, int* size);


/*---------------------------------------------------------------------------*
  友達通信状態取得関数（サーバクライアントマッチメイクゲーム用statusData版）
  引数　：friendData 友達情報へのポインタ
          maxEntry   友達がサーバクライアントマッチメイクのサーバを立てて
                     いる場合の最大接続人数。
                     格納先ポインタ。NULLを渡せば取得しない。
          numEntry   友達がサーバクライアントマッチメイクのサーバを立てて
                     いる場合の現在接続人数。
                     格納先ポインタ。NULLを渡せば取得しない。
          statusData ゲームで定義したGameSpy status文字列格納先ポインタ。
                     NULLを渡せば文字列のコピーは行わない。
          size       ゲームで定義したGameSpy statusデータ長の格納先ポインタ。
                     無効なデータの場合は-1が格納される。
  戻り値：通信状態enum値（dwc_friend.h のDWC_STATUS_*）。
          自分がオフラインの時などに呼び出すとDWC_STATUS_OFFLINEしか返さない。
          また相手がまだGPのバディリストに登録されていない時も
          DWC_STATUS_OFFLINE返す。
  用途　：ローカルにキャッシュされた、指定の友達の通信状態を取得する。
  　　　　通信は行わない。
          友達の通信状態は、変化がある度にGPサーバから通知される。
          ただし、通信中に突然電源を切った場合などは数分間は前の状態が残る。
          戻り値がDWC_STATUS_OFFLINEの場合は、*size = -1 をセットする以外は
          値をセットせずに返します。
 *---------------------------------------------------------------------------*/
extern u8 DWC_GetFriendStatusDataSC(const DWCFriendData* friendData, u8* maxEntry, u8* numEntry, char* statusData, int* size);


/*---------------------------------------------------------------------------*
  友達数取得関数
  引数　：friendList    友達リスト
          friendListLen 友達リストの最大長（要素数）
  戻り値：友達リストに登録されている友達の数
  用途　：ローカルの友達リストを参照して、友達登録数を返す。通信は行わない。
 *---------------------------------------------------------------------------*/
extern int  DWC_GetNumFriend(const DWCFriendData friendList[], int friendListLen);


/*---------------------------------------------------------------------------*
  GameSpy Status文字列セット関数
  引数　：statusString GameSpyのStatusにセットする文字列。255文字＋NULL終端まで。
  戻り値：TRUE :セット成功
          FALSE:ログイン前などでセット失敗
  用途　：自分のGameSpy Statusに文字列をセットする。
          セットされた文字列は、他のホストからもDWC_GetFriendStatus()で
          取得することができる。
          '/'と'\\'は識別文字としてライブラリが使うので、文字列中に
          使用しないで下さい。
          セットされた文字列は、DWC_ShutdownFriendsMatch()が呼ばれると
          クリアされます。
 *---------------------------------------------------------------------------*/
extern BOOL DWC_SetOwnStatusString(const char* statusString);


/*---------------------------------------------------------------------------*
  GameSpy Status文字列取得関数
  引数　：statusString GameSpyのStatus文字列格納先ポインタ。255文字＋NULL終端まで。
  戻り値：TRUE :取得成功、FALSE:ログイン前などで取得失敗
  用途　：現在の自分のGameSpy Status文字列を取得する。
 *---------------------------------------------------------------------------*/
extern BOOL DWC_GetOwnStatusString(char* statusString);


/*---------------------------------------------------------------------------*
  GameSpy Statusデータセット関数
  引数　：statusData GameSpyのStatusにセットするバイナリデータへのポインタ。
          size       バイナリデータのサイズ。189Byteまで。
  戻り値：TRUE :セット成功
          FALSE:ログイン前、容量オーバーなどでセット失敗
  用途　：自分のGameSpy StatusにバイナリデータをBase64でテキスト化して
          セットする。
          セットされたデータは、他のホストからDWC_GetFriendStatusData()で
          取得することができる。
          セットされたデータは、DWC_ShutdownFriendsMatch()が呼ばれると
          クリアされます。
 *---------------------------------------------------------------------------*/
extern BOOL DWC_SetOwnStatusData(const char* statusData, u32 size);


/*---------------------------------------------------------------------------*
  GameSpy Statusデータ取得関数
  引数　：statusData GameSpyのStatusデータ格納先ポインタ
  戻り値：-1:ログイン前などで取得失敗、
          非負整数:取得したバイナリデータのサイズ
  用途　：現在の自分のGameSpy Statusデータを取得する。
          引数statusDataにNULLを渡すと、バイナリデータのサイズのみ取得する。
 *---------------------------------------------------------------------------*/
extern int DWC_GetOwnStatusData(char* statusData);


/*---------------------------------------------------------------------------*
  [obsolute functions]
  互換性のために、古い関数をインラインで残しておく。
 *---------------------------------------------------------------------------*/
inline BOOL DWC_SetGsStatusString(const char* statusString)
{
    return DWC_SetOwnStatusString( statusString );
}
inline BOOL DWC_GetGsStatusString(char* statusString)
{
    return DWC_GetOwnStatusString( statusString );
}
inline BOOL DWC_SetGsStatusData(const char* statusData, u32 size)
{
    return DWC_SetOwnStatusData( statusData, size );
}
inline int DWC_GetGsStatusData(char* statusData)
{
    return DWC_GetOwnStatusData( statusData );
}

/*---------------------------------------------------------------------------*
  友達リスト編集可能状態取得関数
  引数　：なし
  戻り値：TRUE:友達リストを編集して良い、FALSE:友達リストを編集してはいけない
  用途　：友達リストを編集しても良いタイミングかどうかを調べる。
          ここでの戻り値は、次にDWC_ProcessFriendsMatch()が呼ばれるまで有効。
 *---------------------------------------------------------------------------*/
extern BOOL DWC_CanChangeFriendList(void);


/*---------------------------------------------------------------------------*
  友達情報完全消去関数
  引数　：friendData 友達情報へのポインタ
  戻り値：なし
  用途　：友達情報をゼロクリアし、GPサーバ上のバディ関係も消去する。
 *---------------------------------------------------------------------------*/
extern void DWC_DeleteBuddyFriendData(DWCFriendData* friendData);


/*---------------------------------------------------------------------------*
  バディ成立コールバック登録関数
  引数　：callback バディ成立コールバック
          param    コールバック用パラメータ
  戻り値：TRUE :登録成功
          FALSE:FriendsMatchライブラリ非動作中で登録失敗
  用途　：ローカルの友達リストに登録された友達が、DWC_UpdateServersAsync()
          呼出し中以外のタイミングで、GPサーバ上でもバディとして
          認められた時に呼び出されるコールバックを登録する
 *---------------------------------------------------------------------------*/
extern BOOL DWC_SetBuddyFriendCallback(DWCBuddyFriendCallback callback, void* param);


/*---------------------------------------------------------------------------*
  友達状態変化通知コールバック登録関数
  引数　：callback 友達状態変化通知コールバック
          param    コールバック用パラメータ。NULLを渡せば何もセットしない。
                   コールバック用パラメータのデフォルト値はNULL。
  戻り値：TRUE :登録成功
          FALSE:FriendsMatchライブラリ非動作中で登録失敗
  用途　：友達の通信状態が変化した時に通知されるコールバックを登録する。
          通常はDWC_UpdateServersAsync()で登録できるが、
          DWC_UpdateServersAsync()を使わないが友達の状態は取得したいという
          場合に本関数を用いる。
 *---------------------------------------------------------------------------*/
extern BOOL DWC_SetFriendStatusCallback(DWCFriendStatusCallback callback, void* param);


/*---------------------------------------------------------------------------*
  Base64エンコード関数
  引数　：src    変換元データへのポインタ
          srclen 変換元データ長
          dst    変換後データ格納用バッファへのポインタ。
                 NULLを指定すれば変換後に必要な最大バッファサイズを返すだけ。
          dstlen 変換後データ格納用バッファのサイズ（文字列長）
  戻り値：-1      :変換失敗。必要な変換後バッファサイズがdstlenを越えている。
          それ以外:変換結果の文字列長
  用途　：与えられたデータをBase64でエンコードし、char型の文字列として出力する
 *---------------------------------------------------------------------------*/
extern int DWC_Base64Encode(const char *src, const u32 srclen, char *dst, const u32 dstlen);


/*---------------------------------------------------------------------------*
  Base64デコード関数
  引数　：src    Base64データへのポインタ
          srclen Base64データ長（文字列長）
          dst    変換後データ格納用バッファへのポインタ。
                 NULLを指定すれば変換後に必要な最大バッファサイズを返すだけ。
          dstlen 変換後データ格納用バッファのサイズ
  戻り値：-1      :変換失敗。必要な変換後バッファサイズがdstlenを越えている。
          それ以外:変換結果のデータサイズ
  用途　：与えられたBase64データをデコードし、char型のデータとして出力する
 *---------------------------------------------------------------------------*/
extern int DWC_Base64Decode(const char *src, const u32 srclen, char *dst, const u32 dstlen);


/*---------------------------------------------------------------------------*
  データストレージサーバへのログイン関数
  引数　：callback ログイン完了通知コールバック
          param    コールバック用パラメータ
  戻り値：TRUE :ログイン開始。成功か失敗でコールバックが返ってくる。
          FALSE:DWC_LoginAsync()完了前、既にログイン済みなどでログインを
                開始しなかった。コールバックは返ってこない。
  用途　：GameSpyデータストレージサーバへのログインを行なう。
          ログインしないとデータストレージサーバへのセーブ・ロードはできない。
 *---------------------------------------------------------------------------*/
extern BOOL DWC_LoginToStorageServerAsync(DWCStorageLoginCallback callback, void* param);


/*---------------------------------------------------------------------------*
  データストレージサーバからのログアウト関数
  引数　：なし
  戻り値：なし
  用途　：GameSpyデータストレージサーバからログアウトし、
          制御オブジェクトを解放する。
          この関数を呼ばなくても、DWC_ShutdownFriendsMatch()を呼べば、
          ログアウト処理が行なわれる。
 *---------------------------------------------------------------------------*/
extern void DWC_LogoutFromStorageServer(void);


/*---------------------------------------------------------------------------*
  データストレージサーバ データセーブ・ロード完了通知コールバック登録関数
  引数　：saveCallback セーブ完了通知コールバック
          loadCallback ロード完了通知コールバック
  戻り値：TRUE :登録成功
          FALSE:FriendsMatchライブラリ非動作中で登録失敗
  用途　：データストレージへのセーブ・ロードが完了した時に呼び出される
          コールバックを登録する
 *---------------------------------------------------------------------------*/
extern BOOL DWC_SetStorageServerCallback(DWCSaveToServerCallback saveCallback,
                                         DWCLoadFromServerCallback loadCallback);


/*---------------------------------------------------------------------------*
  データストレージサーバPublicデータセーブ関数
  引数　：keyvalues データベース文字列（key/value値）へのポインタ
          param     コールバック用パラメータ
  戻り値：TRUE:セーブ開始、FALSE：オフライン状態か通信エラー中でセーブできない
  用途　：誰でも参照可能なデータをデータストレージサーバにセーブする。
          セーブするデータはNULL終端の文字列で、key/valueの組である必要がある。
          （"\\name\\mario\\stage\\3"のように'\\'で区切って記述する。
          この例のように複数のkey/value組を一度にセーブすることができる）
          データのセットが完了したらコールバックが呼ばれる。
 *---------------------------------------------------------------------------*/
extern BOOL DWC_SavePublicDataAsync(char* keyvalues, void* param);


/*---------------------------------------------------------------------------*
  データストレージサーバPrivateデータセーブ関数
  引数　：keyvalues セーブしたいデータベース文字列（key/value値）へのポインタ
          param     コールバック用パラメータ
  戻り値：TRUE:セーブ開始、FALSE：オフライン状態か通信エラー中でセーブできない
  用途　：本人のみ参照可能なデータをデータストレージサーバにセーブする。
          セーブするデータはNULL終端の文字列で、key/valueの組である必要がある。
          （"\\name\\mario\\stage\\3"のように'\\'で区切って記述する。
          この例のように複数のkey/value組を一度にセーブすることができる）
          データのセットが完了したらコールバックが呼ばれる。
 *---------------------------------------------------------------------------*/
extern BOOL DWC_SavePrivateDataAsync(char* keyvalues, void* param);


/*---------------------------------------------------------------------------*
  データストレージサーバ 自分のPublicデータロード関数
  引数　：keys  ロードしたいデータのkey文字列へのポインタ
          param コールバック用パラメータ
  戻り値：TRUE:ロード開始、FALSE：オフライン状態か通信エラー中でロードできない
  用途　：データストレージサーバからPublicデータをロードする。
          ロードするデータはkeyで指定し、NULL終端の文字列である必要がある。
          （"\\name\\stage"のように'\\'で区切って記述する。
          この例のように複数のkeyを一度にロードすることができる）
          データのロードが完了したらコールバックが呼ばれる。
 *---------------------------------------------------------------------------*/
extern BOOL DWC_LoadOwnPublicDataAsync(char* keys, void* param);


/*---------------------------------------------------------------------------*
  データストレージサーバ 自分のPrivateデータロード関数
  引数　：keys  ロードしたいデータのkey文字列へのポインタ
          param コールバック用パラメータ
  戻り値：TRUE:ロード開始、FALSE：オフライン状態か通信エラー中でロードできない
  用途　：データストレージサーバからPrivateデータをロードする。
          ロードするデータはkeyで指定し、NULL終端の文字列である必要がある。
          （"\\name\\stage"のように'\\'で区切って記述する。
          この例のように複数のkeyを一度にロードすることができる）
          データのロードが完了したらコールバックが呼ばれる。
 *---------------------------------------------------------------------------*/
extern BOOL DWC_LoadOwnPrivateDataAsync(char* keys, void* param);


/*---------------------------------------------------------------------------*
  データストレージサーバ 自分のデータロード関数  (obsolete function)
  引数　：keys  ロードしたいデータのkey文字列へのポインタ
          param コールバック用パラメータ
  戻り値：TRUE:ロード開始、FALSE：オフライン状態か通信エラー中でロードできない
  用途　：データストレージサーバからデータをロードする。
          ロードするデータはkeyで指定し、NULL終端の文字列である必要がある。
          （"\\name\\stage"のように'\\'で区切って記述する。
          この例のように複数のkeyを一度にロードすることができる）
          データのロードが完了したらコールバックが呼ばれる。
          →同じ内容の関数はDWC_LoadOwnPrivateDataAsync()に改名しました。
 *---------------------------------------------------------------------------*/
#define DWC_LoadOwnDataAsync DWC_LoadOwnPrivateDataAsync


/*---------------------------------------------------------------------------*
  データストレージサーバ 他人のPublicデータロード関数
  引数　：keys  ロードしたいデータのkey文字列へのポインタ
          index ロードしたいユーザの友達リストインデックス
          param コールバック用パラメータ
  戻り値：TRUE:ロード開始、
          FALSE：自分がオフライン状態か相手が友達でない、通信エラー中でロードできない
  用途　：データストレージサーバからPublicデータをロードする。
          ロードするデータはkeyで指定し、NULL終端の文字列である必要がある。
          （"\\name\\stage"のように'\\'で区切って記述する。
          この例のように複数のkeyを一度にロードすることができる）
          データのロードが完了したらコールバックが呼ばれる。
 *---------------------------------------------------------------------------*/
extern BOOL DWC_LoadOthersDataAsync(char* keys, int index, void* param);


//----------------------------------------------------------------------------
// function - internal
//---------------------------------------------------------------------------
extern void DWCi_FriendInit(DWCFriendControl* friendcnt,
                            GPConnection* pGpObj,
                            const u16* playerName,
                            DWCFriendData* friendList,
                            int friendListLen);


extern void DWCi_FriendProcess(void);


extern void DWCi_UpdateServersAsync(const char* authToken,
                                    const char* partnerChallenge,
                                    DWCUpdateServersCallback updateCallback,
                                    void* updateParam,
                                    DWCFriendStatusCallback statusCallback,
                                    void* statusParam,
                                    DWCDeleteFriendListCallback deleteCallback,
                                    void* deleteParam);


extern void DWCi_StopFriendProcess(DWCError error, int errorCode);


extern void DWCi_GPGetInfoCallback(GPConnection* connection,
                                   GPGetInfoResponseArg* arg,
                                   void* param);

extern void DWCi_GPRecvBuddyRequestCallback(GPConnection* connection,
                                            GPRecvBuddyRequestArg* arg,
                                            void* param);

extern BOOL DWCi_GPRecvBuddyAuthCallback(GPConnection* connection,
                                         GPRecvBuddyMessageArg* arg,
                                         void* param );


extern void DWCi_GPRecvBuddyStatusCallback(GPConnection* connection,
                                           GPRecvBuddyStatusArg* arg,
                                           void* param);


extern DWCFriendData* DWCi_GetFriendList(void);


extern int  DWCi_GetFriendListLen(void);


extern int  DWCi_GetProfileIDFromList(int index);


extern int DWCi_GetFriendListIndex(int profileID);


extern void DWCi_InitGPProcessCount(void);


extern GPResult DWCi_SetGPStatus(int status, const char* statusString, const char* locationString);


extern BOOL DWCi_GetGPStatus(int* status, char* statusString, char* locationString);


extern void DWCi_CallBuddyFriendCallback(int index);


extern void DWCi_ShutdownFriend(void);


#ifdef __cplusplus
}
#endif


#endif // DWC_FRIEND_H_
