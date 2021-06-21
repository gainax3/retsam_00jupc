#include <nitro.h>
#include <base/dwc_base_gamespy.h>
#include <available.h>
#include <natneg/natneg.h>
#include <gstats/gpersist.h>

#include <base/dwc_report.h>
#include <base/dwc_account.h>
#include <base/dwc_error.h>
#include <base/dwc_connectinet.h>
#include <base/dwc_login.h>
#include <base/dwc_friend.h>
#include <base/dwc_match.h>
#include <base/dwc_transport.h>
#include <base/dwc_common.h>
#include <base/dwc_main.h>


//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// variable
//----------------------------------------------------------------------------
// FriendsMatch制御オブジェクトへのポインタ
static DWCFriendsMatchControl* stpDwcCnt = NULL;

static int stLastSocketError = 0;  // 最後に発生したソケットエラー番号

// gt2コネクションリスト
// [todo]
// 下のInfoListと共に、ゲームから指定されたサイズだけリストを持つように変更する
static GT2Connection stGt2ConnectionList[DWC_MAX_CONNECTIONS];
// gt2コネクション情報リスト
static DWCConnectionInfo stConnectionInfoList[DWC_MAX_CONNECTIONS];


//----------------------------------------------------------------------------
// prototype
//----------------------------------------------------------------------------
static BOOL DWCs_UpdateConnection( void );
static void DWCs_ForceShutdown( void );

static void DWCi_SetState(DWCState state);

static int  DWCi_DeleteAID(u8 aid);
static u32  DWCi_GetAIDBitmapFromList(u8* aidList, int aidListLen);

static GPResult DWCi_HandleGPError(GPResult result);
static GT2Result DWCi_HandleGT2Error(GT2Result result);

static void DWCi_LoginCallback(DWCError error, int profileID, void *param);
static void DWCi_UpdateServersCallback(DWCError error, BOOL isChanged, void* param);
static void DWCi_MatchedCallback(DWCError error, BOOL cancel, BOOL self, BOOL isServer, int profileID, void* param);

static void DWCi_GPErrorCallback(GPConnection* pconnection, GPErrorArg* arg, void* param);
static void DWCi_GPRecvBuddyMessageCallback(GPConnection* pconnection, GPRecvBuddyMessageArg* arg, void* param);

static void DWCi_GT2ReceivedCallback(GT2Connection connection, GT2Byte* message, int len, GT2Bool reliable);
static void DWCi_GT2ClosedCallback(GT2Connection connection, GT2CloseReason reason);
static void DWCi_GT2PingCallback(GT2Connection connection, int latency);
static void DWCi_GT2SocketErrorCallback(GT2Socket socket);


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
void DWC_InitFriendsMatch(DWCFriendsMatchControl* dwccnt,
              DWCUserData* userdata,
              int productID,
              const char* gameName,
              const char* secretKey,
              int sendBufSize,
              int recvBufSize,
              DWCFriendData friendList[],
              int friendListLen)
{
    u32 cpySize;

    SDK_ASSERT(dwccnt);
    SDK_ASSERT(gameName);
    SDK_ASSERT(secretKey);
    SDK_ASSERT(friendListLen <= DWC_MAX_MATCH_IDX_LIST);

    // [arakit] main 051013
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "!!DWC_InitFriendsMatch() was called!!\n");

    stpDwcCnt = dwccnt;  // 制御構造体へのポインタをセット

    DWC_ClearError();  // ここまでのエラーをクリア

    stpDwcCnt->gt2Socket              = NULL;
    stpDwcCnt->gt2Callbacks.connected = DWCi_GT2ConnectedCallback;  // dwc_match.cで定義
    stpDwcCnt->gt2Callbacks.received  = DWCi_GT2ReceivedCallback;
    stpDwcCnt->gt2Callbacks.closed    = DWCi_GT2ClosedCallback;
    stpDwcCnt->gt2Callbacks.ping      = DWCi_GT2PingCallback;
    stpDwcCnt->gt2SendBufSize         = sendBufSize ? sendBufSize : 8*1024;
    stpDwcCnt->gt2RecvBufSize         = recvBufSize ? recvBufSize : 8*1024;

    stpDwcCnt->gpObj            = NULL;
    stpDwcCnt->userdata         = userdata;

    stpDwcCnt->state            = DWC_STATE_INIT;
    stpDwcCnt->lastState        = DWC_STATE_INIT;
    stpDwcCnt->aid              = 0;
    stpDwcCnt->ownCloseFlag     = FALSE;
    stpDwcCnt->profileID        = 0;
    stpDwcCnt->gameName         = gcd_gamename;    // stats変数へのポインタを格納
    stpDwcCnt->secretKey        = gcd_secret_key;  // stats変数へのポインタを格納

    stpDwcCnt->loginCallback         = NULL;
    stpDwcCnt->loginParam            = NULL;
    stpDwcCnt->updateServersCallback = NULL;
    stpDwcCnt->updateServersParam    = NULL;
    stpDwcCnt->matchedCallback       = NULL;
    stpDwcCnt->matchedParam          = NULL;
    stpDwcCnt->matchedSCCallback     = NULL;
    stpDwcCnt->matchedSCParam        = NULL;
    stpDwcCnt->closedCallback        = NULL;
    stpDwcCnt->closedParam           = NULL;

    // gt2コネクションリストとgt2コネクション情報リストを初期化
    DWCi_ClearGT2ConnectionList();

    // ログイン制御構造体初期化
    DWCi_LoginInit(&stpDwcCnt->logcnt, userdata, &stpDwcCnt->gpObj, productID, userdata->gamecode, stpDwcCnt->playerName, DWCi_LoginCallback, NULL);

    // 友達管理構造体初期化
    DWCi_FriendInit(&stpDwcCnt->friendcnt, &stpDwcCnt->gpObj, stpDwcCnt->playerName, friendList, friendListLen);

    // マッチメイク制御構造体初期化
    DWCi_MatchInit(&stpDwcCnt->matchcnt, &stpDwcCnt->gpObj,
                   &stpDwcCnt->gt2Socket, &stpDwcCnt->gt2Callbacks,
                   gcd_gamename, gcd_secret_key,
                   friendList, friendListLen);

    // トランスポート制御構造体初期化
    DWCi_InitTransport(&stpDwcCnt->transinfo);

    // GameSpy persistent SDK のためにグローバル変数を設定（gcd_gamename, gcd_secret_key）
    cpySize = strlen(gameName) < sizeof(gcd_gamename) ? strlen(gameName) : sizeof(gcd_gamename)-1;
    MI_CpuCopy8(gameName, gcd_gamename, cpySize);
    gcd_gamename[cpySize] = '\0';
    
    cpySize = strlen(secretKey) < sizeof(gcd_secret_key) ? strlen(secretKey) : sizeof(gcd_secret_key)-1;
    MI_CpuCopy8(secretKey, gcd_secret_key, cpySize);
    gcd_secret_key[cpySize] = '\0';
}


/*---------------------------------------------------------------------------*
  FriendsMatchライブラリ終了関数
  引数　：なし
  戻り値：なし
  用途　：FriendsMatchライブラリを終了し、GameSpySDKのヒープ領域を開放する
 *---------------------------------------------------------------------------*/
void DWC_ShutdownFriendsMatch(void)
{

    // [arakit] main 051013
    DWC_Printf(DWC_REPORTFLAG_DEBUG,
               "!!DWC_ShutdownFriendsMatch() was called!! stpDwcCnt = 0x%x\n",
               stpDwcCnt);

    if (!stpDwcCnt) return;

    // GPステータスをオフラインにする
    // →gpDestroy()内のクローズ送信（これは即送信される）が同じ効果
    //if (stpDwcCnt->gpObj){
    //    (void)DWCi_SetGPStatus(DWC_STATUS_OFFLINE, "", "");
    //    (void)gpProcess(&stpDwcCnt->gpObj);  // GPサーバにアップ
    //}

    // QR2オブジェクト開放
    // qr2_shutdown()は通信するのでgt2CloseSocket()より前に呼ぶ
    if (stpDwcCnt->matchcnt.qr2Obj){
        qr2_shutdown(stpDwcCnt->matchcnt.qr2Obj);
        stpDwcCnt->matchcnt.qr2Obj = NULL;  // 自分でNULLクリアしなければならない！
    }
#ifdef DWC_QR2_ALIVE_DURING_MATCHING
    stpDwcCnt->matchcnt.qr2ShutdownFlag = 0;  // QR2シャットダウンフラグクリア
#endif

    // SBオブジェクト開放
    if (stpDwcCnt->matchcnt.sbObj){
        ServerBrowserFree(stpDwcCnt->matchcnt.sbObj);
        stpDwcCnt->matchcnt.sbObj = NULL;  // 自分でNULLクリアしなければならない！
    }

    // ネゴシエーションリスト解放
    DWCi_NNFreeNegotiateList();

    CloseStatsConnection();  // Persistentオブジェクトの開放

    // GPオブジェクトの開放
    // [todo]
    // これもコールバック内で呼ばれた場合に問題があるが、開放する必要がある
    if (stpDwcCnt->gpObj){
         // WCMがdisconnectしているときに、最後の送信エラーが出る場合があり、そのときにメモリリークする。
        (void)gpSetCallback(&stpDwcCnt->gpObj, GP_ERROR, (GPCallback)NULL, NULL);
        (void)gpSetCallback(&stpDwcCnt->gpObj, GP_RECV_BUDDY_MESSAGE, (GPCallback)NULL, NULL);
        (void)gpSetCallback(&stpDwcCnt->gpObj, GP_RECV_BUDDY_REQUEST, (GPCallback)NULL, NULL);
        (void)gpSetCallback(&stpDwcCnt->gpObj, GP_RECV_BUDDY_STATUS, (GPCallback)NULL, NULL);
        (void)gpProcess(&stpDwcCnt->gpObj); // たまっているメモリを吐き出す。
        gpDestroy(&stpDwcCnt->gpObj);
        stpDwcCnt->gpObj = NULL;
    }

    // 各モジュールのシャットダウン（主に制御オブジェクトのクリア）
    DWCi_ShutdownLogin();
    DWCi_ShutdownFriend();
    DWCi_ShutdownMatch();
    DWCi_ShutdownTransport();

    // GT2ソケットの開放
    // [todo]
    // コールバック内で関数が呼ばれた場合にオブジェクトがなくて止まる
    // ことがないか確認
    if (stpDwcCnt->gt2Socket){
        gt2CloseSocket(stpDwcCnt->gt2Socket);
        stpDwcCnt->gt2Socket = NULL;  // 自分でNULLクリアしなければならない！
    }

#ifdef GSI_MEM_MANAGED
    gsMemMgrDestroy();  // GameSpyのヒープ領域とヒープマネージャ開放
#endif

    stpDwcCnt = NULL;  // FriendsMatch制御オブジェクトクリア
    
    // パラメータ初期化
    //DWC_InitFriendsMatch(dwccnt, dwccnt->userdata, dwccnt->logcnt.productID, dwccnt->gameName,
    //                     dwccnt->secretKey, dwccnt->gt2SendBufSize,
    //                     dwccnt->gt2RecvBufSize, NULL, NULL);
}


/*---------------------------------------------------------------------------*
  FriendsMatchライブラリ通信処理更新関数
  引数　：なし
  戻り値：なし
  用途　：毎ゲームフレーム呼び出し、 FriendsMatchライブラリの通信処理を更新する
 *---------------------------------------------------------------------------*/
void DWC_ProcessFriendsMatch(void)
{
    GSIACResult acResult;
    GPResult gpResult;

    if ( DWC_UpdateConnection() )
    {
        // DCFが切断された。
        DWCs_ForceShutdown();
    }

    if (!stpDwcCnt || (stpDwcCnt->state == DWC_STATE_INIT) || DWCi_IsError())
        return;

    switch (stpDwcCnt->state){
    case DWC_STATE_AVAILABLE_CHECK:  // ゲーム利用可能状況チェック中
        acResult = GSIAvailableCheckThink();  // AvailableCheck更新
        switch (acResult){
        case GSIACAvailable:    // 利用可能。
            DWC_Printf(DWC_REPORTFLAG_ACHECK, "Confirmed the backend of GameSpy server.\n");
            // GP初期化
            //
            // Nintendo DS
            //
            // namespaceID - 16
            // partnerID -   11(not used for this GameSpy SDK version)
            // partnerCode - NDS(used by NAS)
            //
            gpResult = gpInitialize(&stpDwcCnt->gpObj, stpDwcCnt->logcnt.productID, 0);
            if (DWCi_HandleGPError(gpResult)) return;

#if 0       // 未検証のため保留
            // GPのキャッシュはBUDDYのみに限定
            gpResult = gpDisable(&stpDwcCnt->gpObj, GP_INFO_CACHING_BUDDY_ONLY); 
            if (DWCi_HandleGPError(gpResult)) return;
#endif
            
            // GPのコールバック関数を設定
            gpResult = gpSetCallback(&stpDwcCnt->gpObj, GP_ERROR,
                                     (GPCallback)DWCi_GPErrorCallback,
                                     NULL);
            if (DWCi_HandleGPError(gpResult)) return;

            gpResult = gpSetCallback(&stpDwcCnt->gpObj, GP_RECV_BUDDY_MESSAGE,
                                     (GPCallback)DWCi_GPRecvBuddyMessageCallback,
                                     NULL);
            if (DWCi_HandleGPError(gpResult)) return;

            // DWCi_GPRecvBuddyRequestCallback()はdwc_friend.cで定義されている
            gpResult = gpSetCallback(&stpDwcCnt->gpObj, GP_RECV_BUDDY_REQUEST,
                                     (GPCallback)DWCi_GPRecvBuddyRequestCallback,
                                     NULL);
            if (DWCi_HandleGPError(gpResult)) return;

            // DWCi_GPRecvBuddyStatusCallback()はdwc_friend.cで定義されている
            gpResult = gpSetCallback(&stpDwcCnt->gpObj, GP_RECV_BUDDY_STATUS,
                                     (GPCallback)DWCi_GPRecvBuddyStatusCallback,
                                     NULL);
            if (DWCi_HandleGPError(gpResult)) return;

            DWCi_SetState(DWC_STATE_LOGIN);

            // ログイン内部関数を呼び出す
            DWCi_LoginAsync();
            break;
        case GSIACUnavailable:  // サービス終了
            // ログイン処理終了
            DWCi_StopLogin(DWC_ERROR_AUTH_OUT_OF_SERVICE, -20110); // サービス終了のエラーコード：-20110
            return;
            break;
        case GSIACTemporarilyUnavailable:  // 一時的に利用不可
            // ログイン処理終了
            DWCi_StopLogin(DWC_ERROR_AUTH_STOP_SERVICE, -20101); // サービス一時利用不可のエラーコード：-20101
            return;
            break;
        default:
            break;
        }
        break;
        
    case DWC_STATE_LOGIN:  // ログイン処理中
        DWCi_LoginProcess();
        break;

    case DWC_STATE_UPDATE_SERVERS:  // 友達リスト同期処理中
    case DWC_STATE_ONLINE:          // オンライン中
        DWCi_FriendProcess();
        DWCi_MatchProcess(FALSE);  // ここでqr2_think(), gt2Think()を呼び出す
        break;

    case DWC_STATE_MATCHING:  // マッチメイク処理中
        DWCi_MatchProcess(TRUE);
        DWCi_FriendProcess();
        break;

    case DWC_STATE_CONNECTED:  // 接続完了状態
		DWCi_TransportProcess();
        DWCi_FriendProcess();

        if ((stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_SV) ||
            (stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_CL)){
            // 途中参加ありの場合はマッチメイク処理を行う
            DWCi_MatchProcess(TRUE);
        }
        else if (stpDwcCnt->gt2Socket){
            // 上記以外の場合はここでqr2_think(), gt2Think()を呼び出す
            DWCi_MatchProcess(FALSE);
        }
        break;

    default:
        break;
    }

#ifdef DWC_QR2_ALIVE_DURING_MATCHING
    if (stpDwcCnt->matchcnt.qr2ShutdownFlag == 1){
        // QR2シャットダウンフラグが立っていたらQR2を終了する
        if (stpDwcCnt->matchcnt.qr2Obj != NULL){
            qr2_shutdown(stpDwcCnt->matchcnt.qr2Obj);
            stpDwcCnt->matchcnt.qr2Obj = NULL;  // 自分でNULLクリアしなければならない！
        }
        stpDwcCnt->matchcnt.qr2ShutdownFlag = 0;  // QR2シャットダウンフラグクリア
    }
#endif
}


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
BOOL DWC_LoginAsync(const u16*  ingamesn,
                    const char* reserved,
                    DWCLoginCallback callback,
                    void* param)
{
#pragma unused(reserved)
    u32 len;

    SDK_ASSERT(stpDwcCnt);
//    SDK_ASSERT(userID);
//    SDK_ASSERT(password);
    SDK_ASSERT(callback);

    // [arakit] main 051013
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "!!DWC_LoginAsync() was called!!\n");

    if (ingamesn == NULL){
        // ingamesnは必ず指定してもらう必要がある
        DWC_Printf(DWC_REPORTFLAG_WARNING, "ingamesn is NULL!!\n");
        return FALSE;
    }

    // [arakit] main 051025
    if (DWCi_IsError() || (stpDwcCnt->state != DWC_STATE_INIT)){
        // 既にログイン中かログインしているのに呼ばれたら何もしない
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "But ignored.\n");
        return FALSE;
    }
    // [arakit] main 051025

    stpDwcCnt->loginCallback   = callback;
    stpDwcCnt->loginParam      = param;
    //stpDwcCnt->logcnt.userID   = userID;
    //stpDwcCnt->logcnt.password = password;

    // ゲーム内スクリーンネームを登録する
    if (!ingamesn || (ingamesn[0] == '\0')){
        len = 0;
    }
    else {
        // 最大文字数に収まる分だけコピーする
        MI_CpuClear16( stpDwcCnt->playerName, DWC_MAX_PLAYER_NAME*2 );
        len = DWCi_WStrLen(ingamesn) <= DWC_MAX_PLAYER_NAME-1 ? DWCi_WStrLen(ingamesn) : DWC_MAX_PLAYER_NAME-1;
        MI_CpuCopy16(ingamesn, stpDwcCnt->playerName, len*2);
    }
    stpDwcCnt->playerName[len] = 0;

    // まだインターネットに接続してないのに、Loginしようとした。
    if (DWC_GetInetStatus() != DWC_CONNECTINET_STATE_CONNECTED )
    {
        DWCi_StopLogin( DWC_ERROR_AUTH_ANY, DWC_ECODE_SEQ_LOGIN + DWC_ECODE_TYPE_NETWORK );
        // [arakit] main 051025
        return TRUE;
    }

    DWCi_SetState(DWC_STATE_AVAILABLE_CHECK);

    // GameSpy APIを使用する前に、GAME_NAMEで指定したゲームが
    // GameSpyサーバで処理できるか確認する。
	// Availableチェック開始
	GSIStartAvailableCheck(stpDwcCnt->gameName);

    // [arakit] main 051025
    return TRUE;
}


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
BOOL DWC_UpdateServersAsync(const char* playerName,
                            DWCUpdateServersCallback updateCallback,
                            void* updateParam,
                            DWCFriendStatusCallback statusCallback,
                            void* statusParam,
                            DWCDeleteFriendListCallback deleteCallback,
                            void* deleteParam)
{
#pragma unused(playerName)
    //u32 len;

    // [arakit] main 051025
    SDK_ASSERT(stpDwcCnt);
    SDK_ASSERT(updateCallback);

    // [arakit] main 051013
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "!!DWC_UpdateServersAsync() was called!!\n");

    // [arakit] main 051025
    if (DWCi_IsError() ||
        (stpDwcCnt->state < DWC_STATE_ONLINE) || (stpDwcCnt->state == DWC_STATE_UPDATE_SERVERS)){
        // ログインコールバック中に呼ばれるとSTATE_ONLINE
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "But ignored.\n");
        return FALSE;
    }

#if 0
    // プレイヤー名を取得→プレイヤー名はここではセットしない
    if (!playerName || (playerName[0] == '\0')){
        len = 0;
    }
    else {
        len = strlen(playerName) < DWC_MAX_PLAYER_NAME ? strlen(playerName) : DWC_MAX_PLAYER_NAME-1;
        MI_CpuCopy8(playerName, stpDwcCnt->playerName, len);
    }
    stpDwcCnt->playerName[len] = '\0';  // NULL終端を保証
#endif

    stpDwcCnt->updateServersCallback = updateCallback;
    stpDwcCnt->updateServersParam    = updateParam;

    // 友達リスト同期処理状態に移行
    DWCi_SetState(DWC_STATE_UPDATE_SERVERS);

    // 友達リスト同期処理開始
    DWCi_UpdateServersAsync(stpDwcCnt->logcnt.authToken,
                            stpDwcCnt->logcnt.partnerChallenge,
                            DWCi_UpdateServersCallback, NULL,
                            statusCallback, statusParam,
                            deleteCallback, deleteParam);

    return TRUE;
}


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
BOOL DWC_ConnectToAnybodyAsync(u8  numEntry,
                               const char* addFilter,
                               DWCMatchedCallback matchedCallback,
                               void* matchedParam,
                               DWCEvalPlayerCallback evalCallback,
                               void* evalParam)
{

    SDK_ASSERT(stpDwcCnt);
    SDK_ASSERT((numEntry > 1) && (numEntry <= DWC_MAX_CONNECTIONS));
    SDK_ASSERT(matchedCallback);

    // [arakit] main 051013
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "!!DWC_ConnectToAnybodyAsync() was called!!\n");

    if (DWCi_IsError() || (stpDwcCnt->state != DWC_STATE_ONLINE)){
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "But ignored.\n");
        // [arakit] main 051025
        return FALSE;
    }

    // gt2コネクションリストとgt2コネクション情報リストを初期化
    DWCi_ClearGT2ConnectionList();

    stpDwcCnt->matchedCallback = matchedCallback;
    stpDwcCnt->matchedParam    = matchedParam;

    DWCi_SetState(DWC_STATE_MATCHING);

    // マッチメイク関数呼び出し
    // numEntryはマッチメイク処理内では自分を含まない接続人数を表すので-1して渡す
    DWCi_ConnectToAnybodyAsync((u8)(numEntry-1),
                               addFilter,
                               DWCi_MatchedCallback, NULL,
                               evalCallback, evalParam);

    // [arakit] main 051025
    return TRUE;
}


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
BOOL DWC_ConnectToFriendsAsync(const u8 friendIdxList[],
                               int friendIdxListLen,
                               u8  numEntry,
                               BOOL distantFriend,
                               DWCMatchedCallback matchedCallback,
                               void* matchedParam,
                               DWCEvalPlayerCallback evalCallback,
                               void* evalParam)
{
    // 一時友達リスト。ゲームから友達最大数をもらうようになったらサイズを変更する
    u8  tmpFriendIdxList[DWC_MAX_MATCH_IDX_LIST];
    u8  idxList[DWC_MAX_MATCH_IDX_LIST];
    u8  i;
    u32 j;

    SDK_ASSERT(stpDwcCnt);
    SDK_ASSERT(friendIdxListLen <= DWC_MAX_MATCH_IDX_LIST);
    SDK_ASSERT((numEntry > 1) && (numEntry <= DWC_MAX_CONNECTIONS));
    SDK_ASSERT(matchedCallback);

    // [arakit] main 051013
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "!!DWC_ConnectToFriendsAsync() was called!!\n");

    if (DWCi_IsError() || (stpDwcCnt->state != DWC_STATE_ONLINE)){
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "But ignored.\n");
        // [arakit] main 051025
        return FALSE;
    }

    // gt2コネクションリストとgt2コネクション情報リストを初期化
    DWCi_ClearGT2ConnectionList();

    stpDwcCnt->matchedCallback = matchedCallback;
    stpDwcCnt->matchedParam    = matchedParam;

    DWCi_SetState(DWC_STATE_MATCHING);

    // numEntryはマッチメイク処理内では自分を含まない接続人数を表すので-1して渡す
    if (friendIdxList){
        // マッチメイク関数呼び出し
        DWCi_ConnectToFriendsAsync(friendIdxList, friendIdxListLen,
                                   (u8)(numEntry-1),
                                   distantFriend,
                                   DWCi_MatchedCallback, NULL,
                                   evalCallback, evalParam);
    }
    else {
        // 友達インデックスリストの指定が無い場合は、全ての友達のインデックスを
        // 一時インデックスリストに登録して、マッチメイク関数に渡す
        friendIdxListLen = 0;
        
        // インデックス抜き出し元となるインデックスリストを作成する
        for (i = 0; i < DWCi_GetFriendListLen(); i++){
            idxList[i] = i;
        }

        // インデックスリストからランダムにインデックス値を抜き出し、
        // 友達インデックスリストを作成する
        for (i = 0; i < DWCi_GetFriendListLen(); i++){
            u32 randIdx = DWCi_GetMathRand32((u32)(DWCi_GetFriendListLen()-i));

#ifdef DWC_MATCH_ACCEPT_NO_FRIEND
            // 成立済みバディでなくても全てセットする
            tmpFriendIdxList[i] = idxList[randIdx];
            friendIdxListLen++;
            
#else
            // 無効な友達情報は選ばれないようにする
            if (DWCi_Acc_IsValidFriendData(&(DWCi_GetFriendList())[idxList[randIdx]])){
                tmpFriendIdxList[i] = idxList[randIdx];
                friendIdxListLen++;
            }
#endif

            // 抜き出されたインデックスから後ろのインデックスリストを前に詰める
            for (j = randIdx; j < DWCi_GetFriendListLen()-i-1; j++){
                idxList[j] = idxList[j+1];
            }
        }
            
        // マッチメイク関数呼び出し
        DWCi_ConnectToFriendsAsync(tmpFriendIdxList, friendIdxListLen,
                                   (u8)(numEntry-1),
                                   distantFriend,
                                   DWCi_MatchedCallback, NULL,
                                   evalCallback, evalParam);
    }

    // [arakit] main 051025
    return TRUE;
}


/*---------------------------------------------------------------------------*
  サーバクライアントマッチメイクのサーバ起動関数
  引数　：maxEntry          最大接続人数（自分を含む）
          matchedCallback   マッチメイク完了コールバック。１人接続するごとに呼ばれる。
          matchedParam      上記コールバック用パラメータ
          newClientCallback 新規接続クライアント通知コールバック
          newClientParam    上記コールバック用パラメータ
  戻り値：TRUE :関数呼び出し完了。マッチメイクの結果がコールバックで返ってくる。
          FALSE:本関数を呼んで良い状態ではない。コールバックは返ってこない。
  用途　：サーバを立ててクライアントからの要求があれば、コネクションの
          接続・切断処理を行う。
 *---------------------------------------------------------------------------*/
// [arakit] main 051025 051027
BOOL DWC_SetupGameServer(u8  maxEntry,
                         DWCMatchedSCCallback matchedCallback,
                         void* matchedParam,
                         DWCNewClientCallback newClientCallback,
                         void* newClientParam)
{

    SDK_ASSERT(stpDwcCnt);
    SDK_ASSERT((maxEntry > 1) && (maxEntry <= DWC_MAX_CONNECTIONS));
    SDK_ASSERT(matchedCallback);

    // [arakit] main 051013
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "!!DWC_SetupGameServer() was called!!\n");

    if (DWCi_IsError() || (stpDwcCnt->state != DWC_STATE_ONLINE)){
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "But ignored.\n");
        // [arakit] main 051025
        return FALSE;
    }

    // gt2コネクションリストとgt2コネクション情報リストを初期化
    DWCi_ClearGT2ConnectionList();

    stpDwcCnt->matchedSCCallback = matchedCallback;
    stpDwcCnt->matchedSCParam    = matchedParam;

    // サーバの場合は必ずAID = 0
    stpDwcCnt->aid = 0;

    DWCi_SetState(DWC_STATE_MATCHING);

    // マッチメイク関数呼び出し
    // maxEntryはマッチメイク処理内では自分を含まない接続人数を表すので-1して渡す
    // [arakit] main 051027
    DWCi_SetupGameServer((u8)(maxEntry-1),
                         DWCi_MatchedCallback, NULL,
                         newClientCallback, newClientParam);

    // [arakit] main 051025
    return TRUE;
}


/*---------------------------------------------------------------------------*
  サーバクライアントマッチメイクのクライアント起動関数
  引数　：serverIndex       接続先サーバの友達リストインデックス
          matchedCallback   マッチメイク完了コールバック。１人接続するごとに呼ばれる。
          matchedParam      上記コールバック用パラメータ
          newClientCallback 新規接続クライアント通知コールバック
          newClientParam    上記コールバック用パラメータ
  戻り値：TRUE :関数呼び出し完了。マッチメイクの結果がコールバックで返ってくる。
          FALSE:本関数を呼んで良い状態ではない。コールバックは返ってこない。
  用途　：友達リストのインデックスで接続するサーバを指定し、そこへ接続する。
          また、サーバへ新たに接続したクライアントがあれば、そのクライアントと
          の接続処理を行う。
 *---------------------------------------------------------------------------*/
// [arakit] main 051025
BOOL DWC_ConnectToGameServerAsync(int serverIndex,
                                  DWCMatchedSCCallback matchedCallback,
                                  void* matchedParam,
                                  DWCNewClientCallback newClientCallback,
                                  void* newClientParam)
{
    int profileID;
    int buddyIdx = -1;
    GPBuddyStatus status;
    // [arakit] main 051025
    DWCError dwcError;

    SDK_ASSERT(stpDwcCnt);
    SDK_ASSERT(matchedCallback);

    // [arakit] main 051013
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "!!DWC_ConnectToGameServerAsync() was called!!\n");

    if (DWCi_IsError() || (stpDwcCnt->state != DWC_STATE_ONLINE)){
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "But ignored.\n");
        // [arakit] main 051025
        return FALSE;
    }

    // gt2コネクションリストとgt2コネクション情報リストを初期化
    DWCi_ClearGT2ConnectionList();

    stpDwcCnt->matchedSCCallback = matchedCallback;
    stpDwcCnt->matchedSCParam    = matchedParam;

    DWCi_SetState(DWC_STATE_MATCHING);

    if (!(profileID = DWCi_GetProfileIDFromList(serverIndex)) ||
        !gpIsBuddy(&stpDwcCnt->gpObj, profileID)){
        // 指定されたインデックスがプロファイルIDを持っていない、もしくは
        // 相手がバディでなかった場合はエラーでコールバックを呼び出す
        // [arakit] main 051025
        DWC_Printf(DWC_REPORTFLAG_ERROR, "pid %d is not buddy.\n", profileID);
        dwcError = DWC_ERROR_NOT_FRIEND_SERVER;
        goto error;
        // [arakit] main 051025
    }

    // エラーは有り得ない
    (void)gpGetBuddyIndex(&stpDwcCnt->gpObj, profileID, &buddyIdx);
    (void)gpGetBuddyStatus(&stpDwcCnt->gpObj, buddyIdx, &status);
    
    if (status.status != DWC_STATUS_MATCH_SC_SV){
        // 指定された相手がサーバクライアントマッチメイクのサーバを立ち上げて
        // いなかった場合はエラーでコールバックを呼び出す
        // [arakit] main 051025
        DWC_Printf(DWC_REPORTFLAG_ERROR, "pid %d is not game server.\n", profileID);
        dwcError = DWC_ERROR_NOT_FRIEND_SERVER;
        goto error;
        // [arakit] main 051025
    }

    // [arakit] main 051024
    {
        char valueStr[4];
        u8   maxEntry, numEntry;

        valueStr[0] = '0';  // 念のため0人で初期化
            
        // サーバの最大接続人数の設定を読み出す
        DWC_GetCommonValueString(DWC_GP_SSTR_KEY_MATCH_SC_MAX, valueStr,
                                 status.statusString, '/');
        maxEntry = (u8)strtoul(valueStr, NULL, 10);

        // サーバの現在接続人数の設定を読み出す
        DWC_GetCommonValueString(DWC_GP_SSTR_KEY_MATCH_SC_NUM, valueStr,
                                 status.statusString, '/');
        numEntry = (u8)strtoul(valueStr, NULL, 10);

        if (numEntry == maxEntry){
            // [arakit] main 051025
            DWC_Printf(DWC_REPORTFLAG_ERROR, "pid %d is fully occupied.\n", profileID);
            dwcError = DWC_ERROR_SERVER_FULL;
            goto error;
            // [arakit] main 051025
        }
    }
    // [arakit] main 051024

    // マッチメイク関数呼び出し
    DWCi_ConnectToGameServerAsync(profileID, DWCi_MatchedCallback, NULL,
                                  newClientCallback, newClientParam);

    // [arakit] main 051025
    return TRUE;

    // [arakit] main 051025
error:
    // エラー処理
    DWCi_SetError(dwcError, 0);
            
    stpDwcCnt->matchedSCCallback(dwcError,
                                 FALSE, TRUE, FALSE,
                                 0, stpDwcCnt->matchedSCParam);

    if ((stpDwcCnt != NULL) && (stpDwcCnt->state == DWC_STATE_MATCHING)){
        // マッチメイク状態のままならオンライン状態に戻す
        DWCi_SetState(DWC_STATE_ONLINE);
        // GPステータスをオンラインにする。
        (void)DWCi_SetGPStatus(DWC_STATUS_ONLINE, "", NULL);
    }

    return TRUE;
    // [arakit] main 051025
}


/*---------------------------------------------------------------------------*
  コネクションクローズコールバック設定関数
  引数　：callback コネクション１つをクローズされる度に呼び出されるコールバック
          param    上記コールバック用パラメータ
  戻り値：TRUE :登録成功
          FALSE:FriendsMatchライブラリ非動作中で登録失敗
  用途　：コネクションクローズコールバックを設定する
 *---------------------------------------------------------------------------*/
BOOL DWC_SetConnectionClosedCallback(DWCConnectionClosedCallback callback, void* param)
{

    if (!stpDwcCnt) return FALSE;

    stpDwcCnt->closedCallback = callback;
    stpDwcCnt->closedParam    = param;

    return TRUE;
}


/*---------------------------------------------------------------------------*
  全コネクションクローズ関数 (obsolete function)
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
int  DWC_CloseConnectionsAsync(void)
{

    // [arakit] main 051013
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "!!DWC_CloseConnectionsAsync() was called!!\n");

    if (!stpDwcCnt || DWCi_IsError() ||
        ((stpDwcCnt->state != DWC_STATE_MATCHING) && (stpDwcCnt->state != DWC_STATE_CONNECTED))){
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "But ignored.\n");
        return -1;
    }

    if (!stpDwcCnt->matchcnt.gt2NumConnection){
        // サーバクライアントマッチメイクのサーバ用にここで終了処理を行う
        // （接続ホスト数0でクローズを呼べるのはサーバクライアントマッチメイク
        // のサーバだけ）
        DWC_Printf(DWC_REPORTFLAG_TRANSPORT, "Closed 0 connection.\n");

        // GPステータスをオンラインにする。
        (void)DWCi_SetGPStatus(DWC_STATUS_ONLINE, "", NULL);

        // NN使用領域も開放する
        DWCi_NNFreeNegotiateList();

        DWCi_SetState(DWC_STATE_ONLINE);  // ログイン状態に戻す
            
        return 1;
    }

    // 接続中の全てのホストに対してコネクションをクローズする
    gt2CloseAllConnections(stpDwcCnt->gt2Socket);

    return 0;
}


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
int  DWC_CloseAllConnectionsHard(void)
{

    // [arakit] main 051013
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "!!DWC_CloseAllConnectionsHard() was called!!\n");

    if (!stpDwcCnt || DWCi_IsError() ||
        ((stpDwcCnt->state != DWC_STATE_MATCHING) && (stpDwcCnt->state != DWC_STATE_CONNECTED))){
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "But ignored.\n");
        return -1;
    }

    if (!stpDwcCnt->matchcnt.gt2NumConnection){
        // サーバクライアントマッチメイクのサーバ用にここで終了処理を行う
        // （接続ホスト数0でクローズを呼べるのはサーバクライアントマッチメイク
        // のサーバだけ）
        DWC_Printf(DWC_REPORTFLAG_TRANSPORT, "Closed 0 connection.\n");

        // GPステータスをオンラインにする。
        (void)DWCi_SetGPStatus(DWC_STATUS_ONLINE, "", NULL);

        // NN使用領域も開放する
        DWCi_NNFreeNegotiateList();

        DWCi_SetState(DWC_STATE_ONLINE);  // ログイン状態に戻す
            
        return 1;
    }

    stpDwcCnt->ownCloseFlag = TRUE;   // ゲーム側からクローズされたことを記録

    // 接続中の全てのホストに対してコネクションをクローズする
    gt2CloseAllConnectionsHard(stpDwcCnt->gt2Socket);

    stpDwcCnt->ownCloseFlag = FALSE;  // フラグを下ろす

    return 0;
}


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
int  DWC_CloseConnectionHard(u8 aid)
{
    GT2Connection connection;

    // [arakit] main 051013
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "!!DWC_CloseConnectionHard() was called!! aid = %d.\n", aid);

    if (!stpDwcCnt || DWCi_IsError() ||
        ((stpDwcCnt->state != DWC_STATE_MATCHING) && (stpDwcCnt->state != DWC_STATE_CONNECTED))){
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "But ignored.\n");
        return -1;
    }
    
    if (!(connection = DWCi_GetGT2Connection(aid))){
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "No connection!\n");
        return -2;
    }
    
    gt2CloseConnectionHard(connection);

    return 0;
}


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
int  DWC_CloseConnectionHardBitmap(u32* bitmap)
{
    u8  aid;
    u32 bitmask;

    // [arakit] main 051013
    DWC_Printf(DWC_REPORTFLAG_DEBUG,
               "!!DWC_CloseConnectionHardBitmap() was called!! AID bitmap = 0x%x\n",
               (bitmap == NULL) ? 0 : *bitmap);

    if (!stpDwcCnt || !bitmap || DWCi_IsError() ||
        ((stpDwcCnt->state != DWC_STATE_MATCHING) && (stpDwcCnt->state != DWC_STATE_CONNECTED))){
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "But ignored.\n");
        return -1;
    }

    for (aid = 0; aid < DWC_MAX_CONNECTIONS; aid++){
        bitmask = 1U << aid;

        if (*bitmap & bitmask){
            if (aid == DWC_GetMyAID()){
                *bitmap &= ~bitmask;       // 自分のAIDのビットは下ろす
            }
            else if (DWC_CloseConnectionHard(aid)){
                *bitmap &= ~bitmask;  // クローズ失敗
            }
        }
    }

    if (!*bitmap) return -2;  // 全て存在しないコネクションだった
    return 0;  // クローズ成功
}


/*---------------------------------------------------------------------------*
  接続ホスト数取得関数
  引数　：なし
  戻り値：ネットワークを構成しているホスト数（自分を含む）。
          オフライン時は0を返す。
  用途　：メッシュ型ネットワーク構成ホスト数を返す
 *---------------------------------------------------------------------------*/
int DWC_GetNumConnectionHost(void)
{

    if (!stpDwcCnt) return 0;

    if ((stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_SV) ||
        (stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_CL)){
        // 途中参加ありの場合は有効なaidの数のみ返す
        return DWCi_GetNumValidConnection()+1;
    }
    else {
        // マッチメイク中でも現在の実際接続数を返す
        return DWCi_GetNumAllConnection()+1;
    }
}


/*---------------------------------------------------------------------------*
  自分のAID取得関数
  引数　：なし
  戻り値：自分のAID
  用途　：自分のAID（メッシュ型ネットワーク内で固有のホストID）を返す
 *---------------------------------------------------------------------------*/
u8  DWC_GetMyAID(void)
{

    if (!stpDwcCnt) return 0;

    return stpDwcCnt->aid;
}


/*---------------------------------------------------------------------------*
  接続中ホストのAIDリスト取得関数
  引数　：aidList AIDリストへのポインタ
  戻り値：AIDリスト長（自分を含む）
  用途　：AIDリストへのポインタを取得する（コピーはしない）。
          AIDリストは、現在有効なAIDが前から順に詰められたリストで、
          その要素数は自分も含めた接続中ホスト数になる。
 *---------------------------------------------------------------------------*/
int DWC_GetAIDList(u8** aidList)
{

    if (!stpDwcCnt) return 0;

    *aidList = stpDwcCnt->matchcnt.aidList;

    if ((stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_SV) ||
        (stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_CL)){
        // 途中参加ありの場合は有効なaidのみ返す
        return DWCi_GetValidAIDList(aidList);
    }
    else {
        return DWCi_GetAllAIDList(aidList);
    }
}


/*---------------------------------------------------------------------------*
  接続中ホストのAIDビットマップ取得関数
  引数　：なし
  戻り値：AIDビットマップ。オフライン時は0を返す。
  用途　：接続中ホストのAIDビットマップを取得する。
 *---------------------------------------------------------------------------*/
u32 DWC_GetAIDBitmap(void)
{
    u8* pAidList;
    u32 bitmap = 0;
    int numHost;

    if (!stpDwcCnt) return 0;

    numHost = DWC_GetAIDList(&pAidList);

    // AIDビットマップ取得
    bitmap = DWCi_GetAIDBitmapFromList(pAidList, numHost);

    return bitmap;
}


/*---------------------------------------------------------------------------*
  AID有効判定関数
  引数　：aid AID
  戻り値：TRUE:AIDが有効、FALSE:AIDが無効
  用途　：指定されたAIDが有効かどうか（自分に接続されているかどうか）を調べる。
          自分のAIDを指定した場合はFALSEを返す。
          マッチメイク中の新規接続クライアントのAIDは無効と判定する。
 *---------------------------------------------------------------------------*/
BOOL DWC_IsValidAID(u8 aid)
{

    if (!stpDwcCnt) return FALSE;

    // コネクションがあってもなくても、ゲームからはマッチメイクが完了した
    // ホストしか有効と見えないようにする
    if (!(stpDwcCnt->matchcnt.validAidBitmap & (1 << aid))) return FALSE;

    return DWCi_IsValidAID(aid);
}


/*---------------------------------------------------------------------------*
  FriendsMatchライブラリ状態取得関数
  引数　：なし
  戻り値：DWCState型列挙子
  用途　：FriendsMatchライブラリの処理状態を取得する
 *---------------------------------------------------------------------------*/
DWCState DWC_GetState(void)
{

    if (!stpDwcCnt){
        return DWC_STATE_INIT;
    }
    else {
        return stpDwcCnt->state;
    }
}


/*---------------------------------------------------------------------------*
  ソケットエラー取得関数
  引数　：なし
  戻り値：GT2ソケットエラー種別
  用途　：GameSpy gt2 SDK のソケットエラー種別を取得する
 *---------------------------------------------------------------------------*/
int DWC_GetLastSocketError(void)
{

    return stLastSocketError;
}


//----------------------------------------------------------------------------
// function - internal
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  GT2初期化関数
  引数　：なし
  戻り値：GT2の処理結果型
  用途　：GT2の初期化を行う
 *---------------------------------------------------------------------------*/
GT2Result DWCi_GT2Startup(void)
{
    u16 baseport;
    GT2Result gt2Result;

    if (stpDwcCnt->gt2Socket){
        // 万が一gt2ソケットが既にあるのにここに来た場合は何もしない
        DWC_Printf(DWC_REPORTFLAG_WARNING, "gt2Socket is already made.\n");
        return GT2Success;
    }

    // GT2ソケットに使用するポートをランダムに生成する
    baseport = (u16)(0xc000+DWCi_GetMathRand32(0x4000));

    DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "--- Private port = %d ---\n", baseport);

    // ソケット作成
    gt2Result = gt2CreateSocket(&stpDwcCnt->gt2Socket,
                                gt2AddressToString(0, baseport, NULL),
                                stpDwcCnt->gt2SendBufSize,
                                stpDwcCnt->gt2RecvBufSize,
                                DWCi_GT2SocketErrorCallback);
    if (DWCi_HandleGT2Error(gt2Result)) return gt2Result;

    // 接続要求受信コールバック関数セット
    // dwc_match.cで定義
    gt2Listen(stpDwcCnt->gt2Socket, DWCi_GT2ConnectAttemptCallback);

    // GT2認識不能メッセージ受信コールバック関数登録
    // dwc_match.cで定義
    gt2SetUnrecognizedMessageCallback(stpDwcCnt->gt2Socket, DWCi_GT2UnrecognizedMessageCallback);

    return gt2Result;
}


/*---------------------------------------------------------------------------*
  GT2Connection取得関数
  引数　：aid ホストのAID
  戻り値：GT2Connection。AIDに対応するコネクションがない場合はNULLを返す
  用途　：AIDに対応するGT2Connection値を取得する
 *---------------------------------------------------------------------------*/
GT2Connection DWCi_GetGT2Connection(u8 aid)
{
    int i;

    if (!stpDwcCnt) return NULL;

    for (i = 0; i < DWC_MAX_CONNECTIONS; i++){
        if (stGt2ConnectionList[i] &&
            (((DWCConnectionInfo *)gt2GetConnectionData(stGt2ConnectionList[i]))->aid == aid)){
            return stGt2ConnectionList[i];
        }
    }

    return NULL;
}


/*---------------------------------------------------------------------------*
  GT2ConnectionからのAID取得関数
  引数　：connection GT2Connection型
  戻り値：コネクションのAID
  用途　：GT2ConnectionからAIDを取得する
 *---------------------------------------------------------------------------*/
u8  DWCi_GetConnectionAID(GT2Connection connection)
{

    return ((DWCConnectionInfo *)gt2GetConnectionData(connection))->aid;
}


/*---------------------------------------------------------------------------*
  GT2Connectionからのインデックス取得関数
  引数　：connection GT2Connection型
  戻り値：コネクションの、コネクションリスト内のインデックス
  用途　：GT2Connectionからコネクションリスト内でのインデックスを取得する
 *---------------------------------------------------------------------------*/
u8  DWCi_GetConnectionIndex(GT2Connection connection)
{

    return ((DWCConnectionInfo *)gt2GetConnectionData(connection))->index;
}


/*---------------------------------------------------------------------------*
  GT2Connectionからのユーザデータ取得関数
  引数　：connection GT2Connection型
  戻り値：コネクション固有のユーザ設定データへのポインタ
  用途　：GT2Connectionからコネクション固有のユーザ設定データへのポインタを
          取得する
 *---------------------------------------------------------------------------*/
void* DWCi_GetConnectionUserData(GT2Connection connection)
{

    return ((DWCConnectionInfo *)gt2GetConnectionData(connection))->param;
}


/*---------------------------------------------------------------------------*
  GT2コネクションリストの空きインデックス取得関数
  引数　：なし
  戻り値：リストの空きインデックス。空きが無ければ-1を返す。
  用途　：GT2コネクションリストの空きインデックスを取得する。
 *---------------------------------------------------------------------------*/
int  DWCi_GT2GetConnectionListIdx(void)
{
    int i;

    for (i = 0; i < DWC_MAX_CONNECTIONS; i++){
        if (!stGt2ConnectionList[i]){
            return i;
        }
    }

    return -1;
}


/*---------------------------------------------------------------------------*
  GT2コネクションリスト及びGT2コネクション情報リスト初期化関数
  引数　：なし
  戻り値：なし
  用途　：GT2コネクションリスト及びGT2コネクション情報リストを初期化する
 *---------------------------------------------------------------------------*/
void DWCi_ClearGT2ConnectionList(void)
{
    
    MI_CpuClear32(stGt2ConnectionList, sizeof(GT2Connection)*DWC_MAX_CONNECTIONS);
    MI_CpuClear32(stConnectionInfoList, sizeof(DWCConnectionInfo)*DWC_MAX_CONNECTIONS);
}


/*---------------------------------------------------------------------------*
  GT2コネクションリストポインタ取得関数
  引数　：index GT2コネクションリストのインデックス
  戻り値：GT2コネクションリストへのポインタ
  用途　：インデックスからGT2コネクションリストの要素へのポインタを取得する
 *---------------------------------------------------------------------------*/
GT2Connection* DWCi_GetGT2ConnectionByIdx(int index)
{

    return &stGt2ConnectionList[index];
}


/*---------------------------------------------------------------------------*
  GT2コネクションリストポインタ取得関数
  引数　：profileID ホストのプロファイルID
          numHost   調査するホスト数
  戻り値：GT2コネクションリストへのポインタ。
          プロファイルIDに対応するコネクションがない場合はNULLを返す。
  用途　：プロファイルIDからGT2コネクションリストの要素へのポインタを取得する
 *---------------------------------------------------------------------------*/
GT2Connection* DWCi_GetGT2ConnectionByProfileID(int profileID, int numHost)
{
    u8 i;

    for (i = 0; i < numHost; i++){
        if (stpDwcCnt->matchcnt.sbPidList[i] == profileID) break;
    }

    if (i >= numHost) return NULL;  // 指定のホストなし

    return DWCi_GetGT2ConnectionByIdx(DWCi_GetConnectionIndex(DWCi_GetGT2Connection(stpDwcCnt->matchcnt.aidList[i])));
}


/*---------------------------------------------------------------------------*
  GT2コネクション情報リストポインタ取得関数
  引数　：index GT2コネクションリストのインデックス
  戻り値：GT2コネクション情報リストへのポインタ
  用途　：GT2コネクションリストのインデックスに対応する（同じインデックスの）
          GT2コネクション情報リストの要素へのポインタを取得する
 *---------------------------------------------------------------------------*/
DWCConnectionInfo* DWCi_GetConnectionInfoByIdx(int index)
{

    return &stConnectionInfoList[index];
}


/*---------------------------------------------------------------------------*
  AID有効判定関数　内部使用版
  引数　：aid AID
  戻り値：TRUE:AIDが有効、FALSE:AIDが無効
  用途　：指定されたAIDが有効かどうか（接続されているかどうか）を調べる。
          自分のAIDを指定した場合はFALSEを返す。
          外部公開版と異なり、マッチメイク中の新規接続クライアントのAIDも
          有効と判定する。
 *---------------------------------------------------------------------------*/
BOOL DWCi_IsValidAID(u8 aid)
{
    int i;

    for (i = 0; i < DWC_MAX_CONNECTIONS; i++){
        if (stGt2ConnectionList[i] &&
            (((DWCConnectionInfo *)gt2GetConnectionData(stGt2ConnectionList[i]))->aid == aid)){
            return TRUE;
        }
    }

    return FALSE;
}


//----------------------------------------------------------------------------
// function - static
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  DCF通信処理終了関数
  引数　：なし
  戻り値：なし
  用途　：DCFが切断されたときに呼び出され、必要な処理がある場合にここに記述する。
 *---------------------------------------------------------------------------*/
static void DWCs_ForceShutdown( void )
{
    // 強制的に呼んでしまってもかまわない関数をここで呼び出す。
}


/*---------------------------------------------------------------------------*
  FriendsMatchライブラリ状態セット関数
  引数　：DWCState型列挙子
  戻り値：なし
  用途　：FriendsMatchライブラリの処理状態を設定する
 *---------------------------------------------------------------------------*/
static void DWCi_SetState(DWCState state)
{

    stpDwcCnt->lastState = stpDwcCnt->state;
    stpDwcCnt->state     = state;
}


/*---------------------------------------------------------------------------*
  AID消去関数
  引数　：aid リストから消したいAID
  戻り値：消されたAIDに対応するプロファイルID
  用途　：指定されたAIDをAIDリストから消去して前に詰め、同時にインデックスが
          依存関係にあるプロファイルリスト等も前に詰める
 *---------------------------------------------------------------------------*/
static int DWCi_DeleteAID(u8 aid)
{
    u8* pAidList;
    int numHost;
    int i;

    numHost = DWCi_GetAllAIDList(&pAidList);

    for (i = 0; i < numHost; i++){
        if (pAidList[i] == aid) break;
    }

    if (i == numHost) return 0;  // 存在しないAIDが指定された

    return DWCi_DeleteHostByIndex(i, numHost);
}


/*---------------------------------------------------------------------------*
  AIDビットマップ取得関数
  引数　：aidList    AIDビットマップ取得元リスト
          aidListLen AIDリスト長（接続ホスト数＋１）
  戻り値：AIDビットマップ
  用途　：指定されたAIDリストからAIDビットマップを作成する
 *---------------------------------------------------------------------------*/
static u32 DWCi_GetAIDBitmapFromList(u8* aidList, int aidListLen)
{
    u32 bitmap = 0;
    int i;

    for (i = 0; i < aidListLen; i++){
        bitmap |= 1 << aidList[i];
    }

    return bitmap;
}


/*---------------------------------------------------------------------------*
  GPResult対応エラー処理関数
  引数　：result GPの処理結果型
  戻り値：GPの処理結果型（引数をそのまま返す）
  用途　：GPResultの値に対応するエラー表示（もしくは停止）を行いつつ、
          対応するDWCエラーを渡して、対応するコールバックを呼び出す
 *---------------------------------------------------------------------------*/
static GPResult DWCi_HandleGPError(GPResult result)
{
    int errorCode;
    DWCError dwcError;

    if (result == GP_NO_ERROR) return GP_NO_ERROR;

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Main, GP error %d\n", result);

    switch (result){
    case GP_MEMORY_ERROR:
        dwcError  = DWC_ERROR_FATAL;
        errorCode = DWC_ECODE_TYPE_ALLOC;
        break;
    case GP_PARAMETER_ERROR:
        dwcError  = DWC_ERROR_FATAL;
        errorCode = DWC_ECODE_TYPE_PARAM;
        break;
    case GP_NETWORK_ERROR:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_NETWORK;
        break;
    case GP_SERVER_ERROR:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_SERVER;
        break;
    }

    // ベースエラーコードを追加して各種処理中の固有エラー処理を呼び出す
    switch (stpDwcCnt->state){
    case DWC_STATE_AVAILABLE_CHECK:
        errorCode += DWC_ECODE_SEQ_LOGIN+DWC_ECODE_GS_GP;
        DWCi_StopLogin(dwcError, errorCode);
        break;
    case DWC_STATE_LOGIN:
        errorCode += DWC_ECODE_SEQ_LOGIN+DWC_ECODE_GS_GP;
        // [arakit] main 051027
        if (stpDwcCnt->logcnt.state < DWC_LOGIN_STATE_REMOTE_AUTH){
            // ログイン中でgpConnect開始前なら、ログインを停止して
            // ゲーム側のコールバックを呼び出す
            DWCi_StopLogin(dwcError, errorCode);
        }
        else {
            // ログイン中でgpConnect開始後なら、ログインの停止とゲーム側の
            // コールバック呼び出しは、GPConnectCallback内で行なわれるので
            // ここでは何もしない（エラーをセットすると処理が進まないのでそれもしない）
            DWC_Printf(DWC_REPORTFLAG_ERROR, "Not handle an error here.\n");
            // [arakit] main 051027
        }
        break;
    case DWC_STATE_MATCHING:
        // サーバクライアントマッチメイクで既にSTATE_CONNECTEDになっている
        // 状態でのマッチメイクエラー時は、別にマッチメイク完了コールバックを
        // 返さなくても良い
        errorCode += DWC_ECODE_SEQ_MATCH+DWC_ECODE_GS_GP;
        DWCi_StopMatching(dwcError, errorCode);
        break;
    case DWC_STATE_UPDATE_SERVERS:
        errorCode += DWC_ECODE_SEQ_FRIEND+DWC_ECODE_GS_GP;
        break;
    default:
        errorCode += DWC_ECODE_SEQ_ETC+DWC_ECODE_GS_GP;
        break;
    }

    // 友達管理処理は必ず終了
    DWCi_StopFriendProcess(dwcError, errorCode);

    return result;
}


/*---------------------------------------------------------------------------*
  GT2Result対応エラー処理関数
  引数　：result GT2の処理結果型
  戻り値：GT2の処理結果型（引数をそのまま返す）
  用途　：GT2Resultの値に対応するエラー表示（もしくは停止）を行いつつ、
          対応するDWCエラーを渡して、対応するコールバックを呼び出す
 *---------------------------------------------------------------------------*/
static GT2Result DWCi_HandleGT2Error(GT2Result result)
{
    int errorCode;
    DWCError dwcError;

    if (result == GT2Success) return GT2Success;

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Main, GT2 error %d\n", result);

    switch (result){
    case GT2OutOfMemory:
        dwcError  = DWC_ERROR_FATAL;
        errorCode = DWC_ECODE_TYPE_ALLOC;
        break;
    case GT2Rejected:
    case GT2DuplicateAddress:
        dwcError  = DWC_ERROR_NONE;
        errorCode = 0;
        result    = GT2Success;
        break;
    case GT2NetworkError:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_NETWORK;
        break;
    case GT2AddressError:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_DNS;
        break;
    case GT2TimedOut:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_TIMEOUT;
        break;
    case GT2NegotiationError:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_PEER;
        break;
    }

    // 今はログイン時のgt2CreateSocketの時しか呼ばれない
    if (dwcError){
        // ベースエラーコードを追加
        errorCode += DWC_ECODE_SEQ_LOGIN+DWC_ECODE_GS_GT2;
        DWCi_StopLogin(dwcError, errorCode);
    }

    return result;
}


/*---------------------------------------------------------------------------*
  ログイン完了コールバック関数
  引数　：error     DWCエラー種別
          profileID ログインの結果得られたプロファイルID
          param     コールバック用パラメータ
  戻り値：なし
  用途　：ログイン完了コールバック。
          ログイン完了時にdwc_login.c内の関数から呼ばれる。
 *---------------------------------------------------------------------------*/
static void DWCi_LoginCallback(DWCError error, int profileID, void *param)
{
#pragma unused(param)

    if (error == DWC_ERROR_NONE){
        // ログイン成功なら状態をオンラインにする
        stpDwcCnt->profileID = profileID;
        DWCi_SetState(DWC_STATE_ONLINE);

        // gpProcess()呼び出し回数カウンタを初期化する
        DWCi_InitGPProcessCount();
    }
    else {
        // ログイン失敗の場合
        DWCi_SetState(DWC_STATE_INIT);
    }

    // ゲームから与えられたコールバック関数を呼び出す
    if ( stpDwcCnt->loginCallback != NULL )
    {
        stpDwcCnt->loginCallback(error, profileID, stpDwcCnt->loginParam);
    }
}


/*---------------------------------------------------------------------------*
  友達リスト同期処理完了コールバック関数
  引数　：error     DWCエラー種別
          isChanged TRUE:友達リストが変更された、FALSE:友達リストに変更なし
          param     コールバック用パラメータ
  戻り値：なし
  用途　：友達リスト同期処理完了コールバック。
          友達リスト同期処理完了時にdwc_friend.c内の関数から呼ばれる。
 *---------------------------------------------------------------------------*/
static void DWCi_UpdateServersCallback(DWCError error, BOOL isChanged, void* param)
{
#pragma unused(param)

    // [arakit] main 051008
    if (stpDwcCnt->lastState != DWC_STATE_UPDATE_SERVERS){
        // DWC_UpdateServersAsyn()呼出し後、別のstateに移っていなければ、
        // 呼び出し前の状態に戻す
        // [todo]
        // ２つ以上stateが変わっていたら対応できないので、
        // UPDATE_SERVERSはメインのstateからは外す必要がある
        DWCi_SetState(stpDwcCnt->lastState);
    }
    // [arakit] main 051008

    // ゲームから与えられたコールバックを呼び出す
    stpDwcCnt->updateServersCallback(error, isChanged, stpDwcCnt->updateServersParam);
}


/*---------------------------------------------------------------------------*
  マッチメイク完了コールバック関数
  引数　：error  DWCエラー種別
          cancel TRUE :キャンセルによりマッチメイクが終了した、
                 FALSE:キャンセルではない
          self   TRUE:自分の接続処理完了、FALSE:他人の接続処理完了
          index  サーバクライアントマッチメイクの場合の接続ホストの
                 友達リストインデックス
          param  コールバック用パラメータ
  戻り値：なし
  用途　：マッチメイク完了コールバック。
          マッチメイク完了時にdwc_match.c内の関数から呼ばれる。
 *---------------------------------------------------------------------------*/
static void DWCi_MatchedCallback(DWCError error, BOOL cancel, BOOL self, BOOL isServer, int index, void* param)
{
#pragma unused(param)
    int i;

    if ((error == DWC_ERROR_NONE) && cancel){
        // マッチメイクがキャンセルされた場合
        if (stpDwcCnt->matchcnt.state == DWC_MATCH_STATE_INIT){
            // 自分がキャンセルでマッチメイクを完了した場合
            // 残ったQR2キーデータを他クライアントが参照して予約コマンドを
            // 送ってこないように、QR2キーデータをクリアする
            DWCi_ClearQR2Key();

            // オンライン状態に戻す
            DWCi_SetState(DWC_STATE_ONLINE);
        }
        // サーバクライアントマッチメイクの場合は他ホストがキャンセルしても続ける
    }
    else if (error == DWC_ERROR_NONE){
        // 状態を接続状態にする
        DWCi_SetState(DWC_STATE_CONNECTED);

        // 自分のAIDを取得する
        // （サーバクライアントマッチメイクのサーバはマッチメイク開始時に
        // 取得しているが同じ処理を通しておく）
        for (i = 0; i <= stpDwcCnt->matchcnt.gt2NumConnection; i++){
            if (stpDwcCnt->matchcnt.sbPidList[i] == stpDwcCnt->profileID){
                stpDwcCnt->aid = stpDwcCnt->matchcnt.aidList[i];
                break;
            }
        }
    }

#ifndef	SDK_FINALROM
    if (!error){
        int i;
        // [test]
        // デバッグ表示
        for (i = 0; i < stpDwcCnt->matchcnt.gt2NumConnection+1; i++){
            DWC_Printf(DWC_REPORTFLAG_DEBUG,
                       "aid[%d] = %d, pid[%d] = %u\n",
                       i, stpDwcCnt->matchcnt.aidList[i],
                       i, stpDwcCnt->matchcnt.sbPidList[i]);
        }
    }
#endif

    // 有効なAIDビットマップを更新する
    stpDwcCnt->matchcnt.validAidBitmap =
        DWCi_GetAIDBitmapFromList(stpDwcCnt->matchcnt.aidList, stpDwcCnt->matchcnt.gt2NumConnection+1);
    // 有効コネクション数もセットで更新
    DWCi_SetNumValidConnection();

    // ゲームから与えられたコールバックを呼び出す
    if ((stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_SV) ||
        (stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_CL)){
        stpDwcCnt->matchedSCCallback(error, cancel, self, isServer, index,
                                     stpDwcCnt->matchedSCParam);
    }
    else {
        stpDwcCnt->matchedCallback(error, cancel, stpDwcCnt->matchedParam);
    }

    // [arakit] main 051007
    if ((error != DWC_ERROR_NONE) &&
        (stpDwcCnt != NULL) && (stpDwcCnt->state == DWC_STATE_MATCHING)){
        // エラー発生時にマッチメイク状態のままならオンライン状態に戻す
        DWCi_SetState(DWC_STATE_ONLINE);
        // GPステータスは必ずDWCi_StopMatching()内でセットされる
    }
    // [arakit] main 051007
}


/*---------------------------------------------------------------------------*
  GPエラーコールバック関数
  引数　：pconnection GPコネクションオブジェクトへのポインタ
          arg         GPエラー構造体へのポインタ
          param       コールバック用パラメータ
  戻り値：なし
  用途　：GPエラーコールバック
 *---------------------------------------------------------------------------*/
static void DWCi_GPErrorCallback(GPConnection* pconnection, GPErrorArg* arg, void* param)
{
#pragma unused(pconnection)
#pragma unused(param)
	gsi_char* errorCodeString;
	gsi_char* resultString;
    GPResult gpResult;

#ifdef DWC_IGNORE_GP_ERROR_ALREADY_BUDDY
    if (arg->errorCode == GP_ADDBUDDY_ALREADY_BUDDY ||
        arg->errorCode == GP_BM_NOT_BUDDY ||
        arg->errorCode == GP_DELBUDDY_NOT_BUDDY
	){
        // 登録済みバディへのバディ登録要求エラーは無視する
        DWC_Printf(DWC_REPORTFLAG_DEBUG,
                   "Ignore GP ALLREADY_BUDDY or NOT_BUDDY %d.\n",
                   arg->errorCode);
        return;
    }
#endif

#define RESULT(x) case x: resultString = #x; break;
	switch (arg->result){
	RESULT(GP_NO_ERROR)
	RESULT(GP_MEMORY_ERROR)
	RESULT(GP_PARAMETER_ERROR)
	RESULT(GP_NETWORK_ERROR)
	RESULT(GP_SERVER_ERROR)
	default:
		resultString = "Unknown result!\n";
	}

#define ERRORCODE(x) case x: errorCodeString = #x; break;
	switch(arg->errorCode){
	ERRORCODE(GP_GENERAL)
	ERRORCODE(GP_PARSE)
	ERRORCODE(GP_NOT_LOGGED_IN)
	ERRORCODE(GP_BAD_SESSKEY)
	ERRORCODE(GP_DATABASE)
	ERRORCODE(GP_NETWORK)
	ERRORCODE(GP_FORCED_DISCONNECT)
	ERRORCODE(GP_CONNECTION_CLOSED)
	ERRORCODE(GP_LOGIN)
	ERRORCODE(GP_LOGIN_TIMEOUT)
	ERRORCODE(GP_LOGIN_BAD_NICK)
	ERRORCODE(GP_LOGIN_BAD_EMAIL)
	ERRORCODE(GP_LOGIN_BAD_PASSWORD)
	ERRORCODE(GP_LOGIN_BAD_PROFILE)
	ERRORCODE(GP_LOGIN_PROFILE_DELETED)
	ERRORCODE(GP_LOGIN_CONNECTION_FAILED)
	ERRORCODE(GP_LOGIN_SERVER_AUTH_FAILED)
	ERRORCODE(GP_NEWUSER)
	ERRORCODE(GP_NEWUSER_BAD_NICK)
	ERRORCODE(GP_NEWUSER_BAD_PASSWORD)
	ERRORCODE(GP_UPDATEUI)
	ERRORCODE(GP_UPDATEUI_BAD_EMAIL)
	ERRORCODE(GP_NEWPROFILE)
	ERRORCODE(GP_NEWPROFILE_BAD_NICK)
	ERRORCODE(GP_NEWPROFILE_BAD_OLD_NICK)
	ERRORCODE(GP_UPDATEPRO)
	ERRORCODE(GP_UPDATEPRO_BAD_NICK)
	ERRORCODE(GP_ADDBUDDY)
	ERRORCODE(GP_ADDBUDDY_BAD_FROM)
	ERRORCODE(GP_ADDBUDDY_BAD_NEW)
	ERRORCODE(GP_ADDBUDDY_ALREADY_BUDDY)
	ERRORCODE(GP_AUTHADD)
	ERRORCODE(GP_AUTHADD_BAD_FROM)
	ERRORCODE(GP_AUTHADD_BAD_SIG)
	ERRORCODE(GP_STATUS)
	ERRORCODE(GP_BM)
	ERRORCODE(GP_BM_NOT_BUDDY)
	ERRORCODE(GP_GETPROFILE)
	ERRORCODE(GP_GETPROFILE_BAD_PROFILE)
	ERRORCODE(GP_DELBUDDY)
	ERRORCODE(GP_DELBUDDY_NOT_BUDDY)
	ERRORCODE(GP_DELPROFILE)
	ERRORCODE(GP_DELPROFILE_LAST_PROFILE)
	ERRORCODE(GP_SEARCH)
	ERRORCODE(GP_SEARCH_CONNECTION_FAILED)
	default:
		errorCodeString = "Unknown error code!\n";
	}

	if (arg->fatal){
		DWC_Printf(DWC_REPORTFLAG_ERROR, "FATAL ERROR\n");
	}
	else {
		DWC_Printf(DWC_REPORTFLAG_ERROR, "ERROR\n");
	}
    
	DWC_Printf(DWC_REPORTFLAG_ERROR, "RESULT: %s (%d)\n", resultString, arg->result);
	DWC_Printf(DWC_REPORTFLAG_ERROR, "ERROR CODE: %s (0x%X)\n", errorCodeString, arg->errorCode);
	DWC_Printf(DWC_REPORTFLAG_ERROR, "ERROR STRING: %s\n", arg->errorString);

    // 全てネットワークエラーとする
    gpResult = GP_NETWORK_ERROR;

    // GPエラー処理関数呼び出し
    (void)DWCi_HandleGPError(gpResult);
}


/*---------------------------------------------------------------------------*
  GPバディメッセージ受信コールバック関数
  引数　：pconnection GPコネクションオブジェクトへのポインタ
          arg         GPRecvBuddyMessageArg型オブジェクトへのポインタ
          param       コールバック用パラメータ
  戻り値：なし
  用途　：GPバディメッセージ受信コールバック
 *---------------------------------------------------------------------------*/
static void DWCi_GPRecvBuddyMessageCallback(GPConnection* pconnection, GPRecvBuddyMessageArg* arg, void* param)
{
#pragma unused(param)
    char version[12] = { 0, };
    char* message = arg->message;
    u32 len;

    // dwc_friend.cのバディ登録レスポンスコールバックへ飛ぶ。
    if ( DWCi_GPRecvBuddyAuthCallback( pconnection, arg, param ) )
    {
        return;
    }

    if (memcmp(message, DWC_GP_COMMAND_STRING, strlen(DWC_GP_COMMAND_STRING))){
        // DWCのGPコマンドではないバディメッセージを受信しても何もしない
        DWC_Printf(DWC_REPORTFLAG_WARNING,
                   "Received undefined buddy message. '%s'\n",
                   message);
        return;
    }

    message += strlen(DWC_GP_COMMAND_STRING);
    len = (u32)(strchr(message, 'v')-message);
    strncpy(version, message, len);
    if ((len > 10) || (strtoul(version, NULL, 10) != DWC_MATCHING_VERSION)){
        // バージョン違いコマンドを受信した
        DWC_Printf(DWC_REPORTFLAG_WARNING,
                   "Received different version buddy message command. '%s'\n",
                   message);
        return;
    }

    message += len+1;
    if (memcmp(message, DWC_GP_COMMAND_MATCH_STRING, strlen(DWC_GP_COMMAND_MATCH_STRING)) == 0){
        // [arakit] main 051010
        // マッチメイク用コマンドで、マッチメイク中ならマッチメイク用の
        // コールバックを呼び出す（サーバクライアントマッチメイクの場合は
        // CONNECTED状態でも呼び出す）
        if ((stpDwcCnt->state == DWC_STATE_MATCHING) ||
            ((stpDwcCnt->state == DWC_STATE_CONNECTED) &&
             ((stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_SV) ||
              (stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_CL)))){
            message += strlen(DWC_GP_COMMAND_MATCH_STRING);
            DWCi_MatchGPRecvBuddyMsgCallback(pconnection, arg->profile, message);
        }
        else {
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "Ignore delayed GP matching command.\n");
        }
        // [arakit] main 051010
    }
    //else {
    //    DWC_Printf(DWC_REPORTFLAG_WARNING,
    //               "Received undefined buddy message command. '%s'\n",
    //               arg->message);
    //}
}


/*---------------------------------------------------------------------------*
  GT2データ受信コールバック関数
  引数　：connection 受信したコネクションの構造体ポインタ
          message    受信データ
          len        受信データサイズ
          reliable   信頼性のあるデータかどうか
  戻り値：なし
  用途　：GT2データ受信コールバック
 *---------------------------------------------------------------------------*/
static void DWCi_GT2ReceivedCallback(GT2Connection connection, GT2Byte* message, int len, GT2Bool reliable)
{

	// DWC Transportの受信処理
    DWCi_RecvCallback( connection, message, len, reliable );
}


/*---------------------------------------------------------------------------*
  GT2コネクション切断コールバック関数
  引数　：connection 切断されたコネクションの構造体ポインタ
          reason     切断理由
  戻り値：なし
  用途　：GT2コネクション切断コールバック
 *---------------------------------------------------------------------------*/
static void DWCi_GT2ClosedCallback(GT2Connection connection, GT2CloseReason reason)
{
    u8  aid;
    int profileID = 0;
    int errorCode;
    int i;
    BOOL isServer = FALSE;
    BOOL aidValid;
    DWCConnectionInfo* conninfo;
    DWCError dwcError;

#if 0
    // [todo]
    // GOAGetLastError()の戻り値はいつも負で、条件として使えなかった。
    // ソケットエラーの時にこのまま切断コールバック呼び出しに進むと
    // ゲームの実装によっては変な動作になってしまうと思われる。
    if (GOAGetLastError(gt2GetSocketSOCKET(stpDwcCnt->gt2Socket)) < 0){
        // ソケットエラーの際は、reasonがLocalCloseでここに来るが、
        // SocketErrorCallbackよりもこちらの方が先に呼ばれるので、
        // GameSpyのセットしたエラーを見て、この先に進むのを止める
        DWC_Printf(DWC_REPORTFLAG_ERROR,
                   "Connection was closed by socket error.\n");
        return;
    }
#endif

    if (DWCi_IsShutdownMatch()){
        // 接続完了後や、マッチメイク中にDWC_ShutdownFriendsMatch()が
        // 呼ばれてしまった場合（通常呼んではいけない）、データアクセス例外に
        // ならないようにするため、ここでは何もしない
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_WARNING,
                   "Called DWC_ShutdownFriendsMatch() with unexpected status.\n");
        return;
    }
        

    switch (reason){
	case GT2LocalClose:
	case GT2RemoteClose:
        dwcError = DWC_ERROR_NONE;
        break;
	case GT2CommunicationError:
	case GT2SocketError:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_GS_GT2+DWC_ECODE_TYPE_CLOSE;
        break;
	case GT2NotEnoughMemory:
		dwcError  = DWC_ERROR_FATAL;
        errorCode = DWC_ECODE_GS_GT2+DWC_ECODE_TYPE_CLOSE+DWC_ECODE_TYPE_ALLOC;
        break;
    }

    DWC_Printf(DWC_REPORTFLAG_DEBUG, "Connection was closed (reason %d).\n", reason);

    if (!dwcError){
        // コネクション情報を取得する
	    conninfo = (DWCConnectionInfo *)gt2GetConnectionData(connection);

        if (!conninfo){
            // 万が一このコールバック内からgt2CloseSocket()が呼ばれ、
            // 再びここに来た場合。何もしない。
            // ライブラリが呼び出すことはないが、ユーザがコールバック内で
            // DWC_ShutdownFriendsMatch()を呼び出したらここに来る。
            return;
        }
        
        aid = conninfo->aid;  // AIDを退避

        // AIDが既にマッチメイク完了したホストのものであるかどうかを記録する
        if (stpDwcCnt->matchcnt.validAidBitmap & (1 << aid))
             aidValid = TRUE;
        else aidValid = FALSE;

        // トランスポートのコネクション構造体をクリアする
        DWCi_ClearTransConnection(aid);

        DWC_Printf(DWC_REPORTFLAG_DEBUG, "aid = %d (validity %d).\n",
                   aid, aidValid);

        // サーバクライアントマッチメイクの場合は、サーバが
        // クローズしたことを記録する
        // クライアントの場合、aid = 0 は必ずサーバなので、それだけで判定できる
        if (((stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_SV) &&
             (reason == GT2LocalClose)) ||
            ((stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_CL) &&
             (aid == 0))){
            isServer = TRUE;
        }

        // AIDリストからAIDを消去して前に詰める。
        // プロファイルIDリスト等も同様の消去処理を行う。
        profileID = DWCi_DeleteAID(aid);

        // [todo]
        // 本来はここでメモリ開放
        stGt2ConnectionList[conninfo->index] = NULL;

        if ( stpDwcCnt->matchcnt.gt2NumConnection > 0 )
        {
            stpDwcCnt->matchcnt.gt2NumConnection--;
        }
        if ( stpDwcCnt->matchcnt.qr2NNFinishCount > 0 )
        {
            stpDwcCnt->matchcnt.qr2NNFinishCount--;
        }
    }

    if (!stpDwcCnt->ownCloseFlag &&
        (stpDwcCnt->state == DWC_STATE_CONNECTED) && !aidValid){
        // サーバクライアントマッチメイクで新規接続クライアントが
        // マッチメイクをキャンセルした場合。
        // もしくは、マッチメイク完了同期調整処理中に新規接続クライアントも
        // 応答がなくなった場合
        if ((stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_SV) &&
            (dwcError == DWC_ERROR_NONE)){
            // 同期調整中の場合に、接続済みクライアントをクローズした時に
            // 新規接続クライアントも含めた人数をgpStatusにセットして
            // しまっている場合があるので、ここで接続人数を改めて
            // GPステータスにセットする
            (void)DWCi_GPSetServerStatus();
            
            // サーバクライアントマッチメイクのサーバは接続済みクライアントへの
            // キャンセル通知を行う
            DWCi_ProcessMatchSCClosing(profileID);
        }

        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Closing process by matching SC.\n");
        return;
    }
    else if (DWCi_ProcessMatchClosing(dwcError, errorCode, profileID)){
        // マッチメイク中にコネクションクローズされた場合は、マッチメイク用の
        // 処理を行い、この先の処理は飛ばす。
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Closing process by matching.\n");
        return;
    }

    if (dwcError){
        // エラーの場合はここで処理する
        DWCi_SetError(dwcError,  errorCode+DWC_ECODE_SEQ_MATCH);
        return;
    }


    // ここ以下は、友達指定、友達無指定の場合は、マッチメイク完了後にしか来ない。
    // サーバクライアントの場合は、自分が接続完了後、接続済みホストが（を）
    // クローズした場合のみ来る。

    // [arakit] main 051010
    if ((!stpDwcCnt->ownCloseFlag) &&
        ((stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_SV) ||
         (stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_CL)) &&
        (stpDwcCnt->matchcnt.sbPidList[stpDwcCnt->matchcnt.gt2NumConnection+2] != 0)){
        // サーバクライアントマッチメイクで、新規接続クライアントが接続完了
        // する前に、接続済みクライアントがコネクションをクローズした場合、
        // リストがずれないように調整を行なう。
        // 次の関数で有効なAIDビットマップが変更されるので、AID = 0 のビットを
        // 下ろしてしまわないように一時的にAIDをセットしておく。
        stpDwcCnt->matchcnt.aidList[stpDwcCnt->matchcnt.gt2NumConnection+1] =
            stpDwcCnt->matchcnt.aidList[stpDwcCnt->matchcnt.gt2NumConnection+2];

        // 新規接続クライアントのデータをリスト上で前にずらす
        (void)DWCi_DeleteHostByIndex(stpDwcCnt->matchcnt.gt2NumConnection+1,
                                     stpDwcCnt->matchcnt.gt2NumConnection+3);
    }
    // [arakit] main 051010
        
    if (stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_SV){
        // サーバクライアントマッチメイクのサーバの場合
        if (!stpDwcCnt->ownCloseFlag){
            // DWC_CloseAllConnectionsHard()でクローズしたのでない場合は、
            // 接続人数の減少をGPステータスにセットする
            (void)DWCi_GPSetServerStatus();
        }
        else if (stpDwcCnt->matchcnt.gt2NumConnection == 0){
            // DWC_CloseAllConnectionsHard()でクローズが完了した場合は
            // GPステータスをオンラインにする
            (void)DWCi_SetGPStatus(DWC_STATUS_ONLINE, "", NULL);
        }
    }
    else if (stpDwcCnt->matchcnt.gt2NumConnection == 0){
        // サーバクライアントマッチメイクのサーバ以外では、
        // 全てのコネクションがクローズされたらGPステータスをオンラインにする
        (void)DWCi_SetGPStatus(DWC_STATUS_ONLINE, "", NULL);
    }

    for (i = 0; i < stpDwcCnt->matchcnt.gt2NumConnection+1; i++){
         DWC_Printf(DWC_REPORTFLAG_DEBUG,
                    "aid[%d] = %d, pid[%d] = %u\n",
                    i, stpDwcCnt->matchcnt.aidList[i],
                    i, stpDwcCnt->matchcnt.sbPidList[i]);
    }

    if ((stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_ANYBODY) ||
        (stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_FRIEND)){
        // 友達指定、無指定マッチメイクの場合は、接続希望人数を現在の接続数に
        // 合わせて減少させ、クライアントからの予約が来ないようにする
        stpDwcCnt->matchcnt.qr2NumEntry = stpDwcCnt->matchcnt.qr2NNFinishCount;

        // 状態の変化をマスターサーバに通知する
        qr2_send_statechanged(stpDwcCnt->matchcnt.qr2Obj);
    }

    if (stpDwcCnt->closedCallback && aidValid){
        // ゲームから与えられたコールバックを呼び出す。
        // サーバクライアントマッチメイクで接続完了していないクライアントの
        // 場合はコールバックを呼び出さない。
        stpDwcCnt->closedCallback(dwcError,
                                  reason == GT2LocalClose ? TRUE : FALSE,
                                  isServer,
                                  aid,
                                  DWCi_GetFriendListIndex(profileID),
                                  stpDwcCnt->closedParam);
    }

    // サーバクライアントマッチメイクのサーバの場合は、クローズされて
    // 接続ホスト数が0になっても関係ないので、ここで抜ける
    if (!stpDwcCnt->ownCloseFlag &&
        (stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_SV))
        return;

    if (!stpDwcCnt->matchcnt.gt2NumConnection){
        // 全てのコネクションがクローズされた時
        // サーバクライアントマッチメイク用にNN使用領域も開放する
        DWCi_NNFreeNegotiateList();

        // 残ったQR2キーデータを他クライアントが参照して予約コマンドを
        // 送ってこないように、QR2キーデータをクリアする
        DWCi_ClearQR2Key();

        DWCi_SetState(DWC_STATE_ONLINE);  // ログイン状態に戻す
    }
}


/*---------------------------------------------------------------------------*
  GT2 PINGコールバック関数
  引数　：connection PINGが完了したコネクションの構造体ポインタ
          latency    相手ホストへのレイテンシ
  戻り値：なし
  用途　：GT2 PINGコールバック
 *---------------------------------------------------------------------------*/
static void DWCi_GT2PingCallback(GT2Connection connection, int latency)
{
#pragma unused(connection)
    
	DWC_Printf(DWC_REPORTFLAG_TRANSPORT, "Ping: %dms\n", latency);

	// DWC Transportで設定するPingコールバック
    DWCi_PingCallback( connection, latency );
}


/*---------------------------------------------------------------------------*
  GT2ソケットエラーコールバック関数
  引数　：socket GT2Socketオブジェクト
  戻り値：なし
  用途　：GT2ソケットエラーコールバック
 *---------------------------------------------------------------------------*/
static void DWCi_GT2SocketErrorCallback(GT2Socket socket)
{

    stLastSocketError = GOAGetLastError(gt2GetSocketSOCKET(socket));

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Socket fatal error! (%d)\n",
               stLastSocketError);

    // マッチメイク中にも起こり得るが、FATALエラーとして処理するので
    // マッチメイク終了処理は呼ばない
    DWCi_SetError(DWC_ERROR_FATAL, DWC_ECODE_SEQ_ETC+DWC_ECODE_GS_GT2+DWC_ECODE_TYPE_SO_SOCKET);

    // ソケットエラーの場合gti2SocketError()内でソケットがクローズされるので
    // NULLクリアしなければならない！
    stpDwcCnt->gt2Socket = NULL;
}
