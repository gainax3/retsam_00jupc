#include <nitro.h>
#include <base/dwc_base_gamespy.h>

#include <auth/dwc_auth.h>
#include <base/dwc_report.h>
#include <base/dwc_account.h>
#include <base/dwc_error.h>
#include <base/dwc_memfunc.h>
#include <base/dwc_login.h>
#include <base/dwc_friend.h>
#include <base/dwc_match.h>
#include <base/dwc_transport.h>
#include <base/dwc_main.h>
#include <base/dwc_common.h>


//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// typedef
//----------------------------------------------------------------------------
typedef void (*DWCRemoteAuthCallback)(const char* authToken, const char* partnerChallenge, void* param);

//----------------------------------------------------------------------------
// static variable
//----------------------------------------------------------------------------
// ログイン制御オブジェクトへのポインタ
static DWCLoginControl* stpLoginCnt = NULL;

static DWCRemoteAuthCallback stRemoteAuthCallback;
static void* pStRemoteAuthParam;

//----------------------------------------------------------------------------
// prototype
//----------------------------------------------------------------------------
static void DWCi_CloseLogin(void);

static GPResult DWCi_HandleGPError     (GPResult result);
static void     DWCi_GPConnectCallback (GPConnection* pconnection, GPConnectResponseArg* arg, void* param);
static void     DWCi_RemoteAuthCallback(const char* authToken, const char* partnerChallenge, void* param);
static void     DWCi_GPConnect( const char* authToken,  const char* partnerChallenge, GPCallback callback, DWCLoginState nextstate );

static void DWCi_GPGetInfo( void );
static void DWCi_GPGetInfoCallback( GPConnection* pconnection, GPGetInfoResponseArg* arg, void* param );

static void DWCi_RemoteLogin       ( DWCRemoteAuthCallback callback, void* param );
static void DWCi_RemoteLoginProcess( void );


//----------------------------------------------------------------------------
// function - internal
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  ログイン制御構造体初期化関数
  引数　：logcnt     ログイン制御オブジェクトへのポインタ
          userdata   ユーザデータオブジェクトへのポインタ
          pGpObj     gpコネクション構造体ポインタへのポインタ
          productID  GameSpyから与えられるプロダクトID
          gamecode   ゲーム特定用のコード（通常イニシャルコード）
          playerName ゲーム内スクリーンネーム
          callback   ログイン完了通知用コールバック関数
          param      コールバック用パラメータ
  戻り値：なし
  用途　：ログイン制御構造体を初期化する
 *---------------------------------------------------------------------------*/
void DWCi_LoginInit(DWCLoginControl* logcnt, DWCUserData* userdata, GPConnection* pGpObj, int productID, u32 gamecode, const u16* playerName, DWCLoginCallback callback, void* param)
{
    SDK_ASSERT(logcnt);

    DWC_Printf( DWC_REPORTFLAG_LOGIN, "Login Init\n" );

    stpLoginCnt = logcnt;

    MI_CpuClear8( stpLoginCnt, sizeof( DWCLoginControl ) );

    // ログイン制御構造体初期化
    stpLoginCnt->pGpObj     = pGpObj;
    stpLoginCnt->state      = DWC_LOGIN_STATE_INIT;
    stpLoginCnt->productID  = productID;
    stpLoginCnt->gamecode   = gamecode;
    stpLoginCnt->playerName = playerName;
    stpLoginCnt->callback   = callback;
    stpLoginCnt->param      = param;

    stpLoginCnt->userdata   = userdata;

    DWC_Printf( DWC_REPORTFLAG_LOGIN, "******************************************\n" );
    DWC_Printf( DWC_REPORTFLAG_LOGIN, "  pseudo    UserID   : %016llx\n", DWCi_Acc_GetUserId( &userdata->pseudo ) );
    DWC_Printf( DWC_REPORTFLAG_LOGIN, "  pseudo    PlayerID : %08x\n"   , DWCi_Acc_GetPlayerId( &userdata->pseudo ) );
    DWC_Printf( DWC_REPORTFLAG_LOGIN, "  authentic UserID   : %016llx\n", DWCi_Acc_GetUserId( &userdata->authentic ) );
    DWC_Printf( DWC_REPORTFLAG_LOGIN, "  authentic PlayerID : %08x\n"   , DWCi_Acc_GetPlayerId( &userdata->authentic ) );
    DWC_Printf( DWC_REPORTFLAG_LOGIN, "******************************************\n" );

}


/*---------------------------------------------------------------------------*
  Wi-Fiコネクションログイン関数
  引数　：なし
  戻り値：なし
  用途　：Wi-Fiコネクションへのログインを開始する
 *---------------------------------------------------------------------------*/
//void DWCi_LoginAsync(DWCLoginCallback callback, void* param)
void DWCi_LoginAsync( void )
{

    DWC_Printf( DWC_REPORTFLAG_LOGIN, "Login Async\n" );

    // 本来の認証サーバ呼び出し。
    DWCi_RemoteLogin( DWCi_RemoteAuthCallback, NULL );

    stpLoginCnt->state = DWC_LOGIN_STATE_REMOTE_AUTH;
    stpLoginCnt->connectFlag = FALSE;
}


/*---------------------------------------------------------------------------*
  ログイン処理更新関数
  引数　：なし
  戻り値：なし
  用途　：毎ゲームフレーム呼び出し、ログイン通信処理を更新する
 *---------------------------------------------------------------------------*/
void DWCi_LoginProcess(void)
{

    if (!stpLoginCnt || DWCi_IsError()) return;

    switch (stpLoginCnt->state){
    case DWC_LOGIN_STATE_REMOTE_AUTH:  // リモート認証中
        DWCi_RemoteLoginProcess();
        break;

    case DWC_LOGIN_STATE_CONNECTING:  // GPサーバ接続中
    case DWC_LOGIN_STATE_GPGETINFO:
    case DWC_LOGIN_STATE_GPSETINFO:
        if (stpLoginCnt->pGpObj && *stpLoginCnt->pGpObj)
        {
            gpProcess(stpLoginCnt->pGpObj);
        }
        if ( stpLoginCnt->connectFlag &&
             ( OS_TicksToMilliSeconds( OS_GetTick() - stpLoginCnt->connectTick ) > DWC_LOGIN_GPCONNECT_TIMEOUT ) )
        {
            // タイムアウト発生。
            DWCi_StopLogin( DWC_ERROR_NETWORK, DWC_ECODE_SEQ_LOGIN+DWC_ECODE_GS_GP+DWC_ECODE_TYPE_TIMEOUT );
            stpLoginCnt->connectFlag = FALSE;
        }
        break;

    case DWC_LOGIN_STATE_INIT:
    case DWC_LOGIN_STATE_CONNECTED:
    default:
        // 上記以外の場合は何もしないで抜ける
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Ignore invalid login state.\n");
        break;
    }
}


/*---------------------------------------------------------------------------*
  ユーザデータ取得関数
  引数　：なし
  戻り値：ユーザデータへのポインタ
  用途　：ユーザデータへのポインタを取得する
 *---------------------------------------------------------------------------*/
DWCAccUserData* DWCi_GetUserData(void)
{

    if (stpLoginCnt){
        return stpLoginCnt->userdata;
    }
    else {
        return NULL;
    }
}


/*---------------------------------------------------------------------------*
  ログインエラー終了関数
  引数　：error     DWCエラー種別
          errorCode エラーコード
  戻り値：なし
  用途　：ログイン処理中にエラーが発生した場合に、処理を中断する
 *---------------------------------------------------------------------------*/
void DWCi_StopLogin(DWCError error, int errorCode)
{

    if (!stpLoginCnt || (error == DWC_ERROR_NONE)) return;

    DWCi_SetError(error, errorCode);
    
    // ログイン完了コールバック関数呼び出し
    if ( stpLoginCnt->callback != NULL )
    {
        stpLoginCnt->callback(error, 0, stpLoginCnt->param);
    }

    DWCi_CloseLogin();

    // この後、DWC_ShutdownFriendsMatch()でGPオブジェクトを解放する必要がある
}


/*---------------------------------------------------------------------------*
  ログイン制御オブジェクト解放関数
  引数　：なし
  戻り値：なし
  用途　：ログイン制御オブジェクトへのポインタをNULLクリアする
 *---------------------------------------------------------------------------*/
void DWCi_ShutdownLogin(void)
{
    // 認証中かどうかを確認。
    if ( stpLoginCnt->http != NULL )
    {
        DWC_Auth_Abort();
        DWC_Auth_Destroy();

        DWC_Free( DWC_ALLOCTYPE_AUTH, stpLoginCnt->http, 0 );
        stpLoginCnt->http = NULL;
    }

    stpLoginCnt = NULL;
}


//----------------------------------------------------------------------------
// function - static
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  ログイン終了関数
  引数　：なし
  戻り値：なし
  用途　：ログイン制御オブジェクトを初期化してログイン処理を終了する
 *---------------------------------------------------------------------------*/
static void DWCi_CloseLogin(void)
{

    if (!stpLoginCnt) return;
    
    // ログイン制御オブジェクト初期化
    stpLoginCnt->state = DWC_LOGIN_STATE_INIT;
    stpLoginCnt->connectFlag = FALSE;

    // passwordもauthTokenもpartnerChallengeも、Persistent Storageへの
    // ログインに使う
}


/*---------------------------------------------------------------------------*
  GPResult対応エラー処理関数
  引数　：result GPの処理結果型
  戻り値：GPの処理結果型（引数をそのまま返す）
  用途　：GPResultの値に対応するエラー表示（もしくは停止）を行いつつ、
          対応するDWCエラーを渡してログイン手続きを終了する
 *---------------------------------------------------------------------------*/
static GPResult DWCi_HandleGPError(GPResult result)
{
    int errorCode;
    DWCError dwcError;

    if (result == GP_NO_ERROR) return GP_NO_ERROR;

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Login, GP error %d\n", result);

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

    // ベースエラーコードを追加
    errorCode += DWC_ECODE_SEQ_LOGIN+DWC_ECODE_GS_GP;

    DWCi_StopLogin(dwcError, errorCode);  // ログイン手続きをエラー終了

    return result;
}


/*---------------------------------------------------------------------------*
  GPコネクトコールバック関数
  引数　：pconnection GPコネクションオブジェクトへのポインタ
          arg         GPコネクション応答構造体へのポインタ
          param       コールバック用パラメータ
  戻り値：なし
  用途　：GPコネクトコールバック
 *---------------------------------------------------------------------------*/
static void DWCi_GPConnectCallback(GPConnection* pconnection, GPConnectResponseArg* arg, void* param)
{
#pragma unused( param )
    GPResult gpResult;

    DWC_Printf(DWC_REPORTFLAG_LOGIN, "Finished connecting to GP server, result = %d\n", arg->result);
    // コールバックが呼び出されたら、終了。
    stpLoginCnt->connectFlag = FALSE;

    if (arg->result == GP_NO_ERROR)
    {
        // [nakata] GPConnectResponseArgを退避(gpGetInfo後に使用する)
        stpLoginCnt->gpconnectresponsearg = *arg;
        if ( stpLoginCnt->state == DWC_LOGIN_STATE_CONNECTING )
        {
            // GPステータスをオンライン状態にセットする。
            //gpResult = DWCi_SetGPStatus(DWC_STATUS_ONLINE, "", NULL);

            //if (DWCi_HandleGPError(gpResult)) return;

            if ( stpLoginCnt->userdata->gs_profile_id == arg->profile )
            {
                DWC_Printf( DWC_REPORTFLAG_LOGIN, "  gs profile id is valid.\n" );

                stpLoginCnt->state = DWC_LOGIN_STATE_CONNECTED;

                // GPステータスをオンライン状態にセットする。
                gpResult = DWCi_SetGPStatus(DWC_STATUS_ONLINE, "", NULL);
                if (DWCi_HandleGPError(gpResult)) return;

                // 接続成功。ログイン完了コールバック関数呼び出し
                stpLoginCnt->callback(DWC_ERROR_NONE, arg->profile, stpLoginCnt->param);

                // gt2ソケット作成
                if (!DWCi_GT2Startup()){
                    // 成功したらQR2起動
                    if (DWCi_QR2Startup(arg->profile)) return;
                }
                else return;
                // 各関数内でエラーが発生した場合は、内部でログインエラー終了
                // 処理を行っている
            }
            else
            {
                DWC_Printf( DWC_REPORTFLAG_LOGIN, "  gs profile id is invalid.\n" );

                DWCi_StopLogin( DWC_ERROR_NETWORK, DWC_ECODE_SEQ_LOGIN );
            }
        }
        else if ( stpLoginCnt->state == DWC_LOGIN_STATE_GPGETINFO )
        {
            // GPGetInfoでlastnameチェックをするためのシーケンス
            gpResult = gpGetInfo( pconnection, arg->profile, GP_DONT_CHECK_CACHE, GP_NON_BLOCKING, (GPCallback)DWCi_GPGetInfoCallback, NULL );
            if (DWCi_HandleGPError(gpResult)) return;
        }
    }
    else
    {
        // 接続に失敗した場合
        (void)DWCi_HandleGPError(arg->result);
    }
}

/*---------------------------------------------------------------------------*
  リモート認証完了コールバック関数
  引数　：authToken        取得した認証用トークンへのポインタ
          partnerchallenge 取得したチャレンジへのポインタ
          param            コールバック用パラメータ
  戻り値：なし
  用途　：リモート認証完了コールバック
 *---------------------------------------------------------------------------*/
static void DWCi_RemoteAuthCallback(const char* authToken, const char* partnerChallenge, void* param)
{
#pragma unused( param )

    DWCi_GPConnect( authToken, partnerChallenge, (GPCallback)DWCi_GPConnectCallback, DWC_LOGIN_STATE_CONNECTING );
}

static void DWCi_GPConnect( const char* authToken,  const char* partnerChallenge,
                            GPCallback callback, DWCLoginState next_state )
{
    GPResult gpResult;

    DWC_Printf(DWC_REPORTFLAG_LOGIN, "Succeeded to remote authentication.\n");

    strcpy( stpLoginCnt->authToken, authToken );
    strcpy( stpLoginCnt->partnerChallenge, partnerChallenge );

    stpLoginCnt->connectTick = OS_GetTick();
    stpLoginCnt->connectFlag = TRUE;

    gpResult = gpConnectPreAuthenticated(stpLoginCnt->pGpObj, stpLoginCnt->authToken,
                                         stpLoginCnt->partnerChallenge, GP_FIREWALL,
                                         GP_NON_BLOCKING,
                                         callback,
                                         NULL);
    if (DWCi_HandleGPError(gpResult))
    {
        return;
    }

    stpLoginCnt->state = next_state;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static void DWCi_RemoteLogin( DWCRemoteAuthCallback callback,
                              void*                 param )
{
    DWCAuthParam auth;
    void* http;

    DWC_Printf( DWC_REPORTFLAG_LOGIN, "Start Remote Auth\n" );

    MI_CpuClear8( &auth, sizeof(auth) );
    
    stRemoteAuthCallback = callback;
    pStRemoteAuthParam   = param;

    // 認証用のデータ作成
    if ( DWCi_Acc_IsAuthentic( stpLoginCnt->userdata ) )
    {
        DWC_Printf( DWC_REPORTFLAG_LOGIN, "  Hmm.. you already have authentic login id.\n" );

        // 認証済みなので、認証済みのuseridを使用する。
        DWCi_Acc_LoginIdToUserName( &stpLoginCnt->userdata->authentic, stpLoginCnt->userdata->gamecode, stpLoginCnt->username );
    }
    else
    {
        DWC_Printf( DWC_REPORTFLAG_LOGIN, "  Hmm.. you need to create authentic login id.\n" );

        // 何度目の実行かを確認する。
        if ( !DWCi_Acc_IsValidLoginId( &stpLoginCnt->tempLoginId ) )
        {
            DWC_Printf( DWC_REPORTFLAG_LOGIN, "    Hmm.. you are the first time to get authentic login id." );
            // 最初の実行の場合は、仮IDをコピーして使う。
            if ( DWCi_Acc_CheckConsoleUserId( &stpLoginCnt->userdata->pseudo ) )
            {
                DWC_Printf( DWC_REPORTFLAG_LOGIN, "- copy temp loginid from pseudo login id\n" );
                stpLoginCnt->tempLoginId = stpLoginCnt->userdata->pseudo;
            }
            else
            {
                // 仮IDを作ったのと違うホストなので。
                DWC_Printf( DWC_REPORTFLAG_LOGIN, "- create temp loginid from console user id\n" );
                DWCi_Acc_CreateTempLoginId( &stpLoginCnt->tempLoginId );
            }
        }
        else
        {
            MATHRandContext32   randcontext;

            DWC_Printf( DWC_REPORTFLAG_LOGIN, "    Hmm.. you are NOT the first times to get authentic login id.\n" );
            // 二度目以降は、プレイヤーIDのみを置き換える。
            // [todo] 乱数化. -> ok
            //DWCi_Acc_SetPlayerId( &stpLoginCnt->tempLoginId, DWCi_Acc_GetPlayerId( &stpLoginCnt->tempLoginId ) + 1 );

            MATH_InitRand32( &randcontext, OS_GetTick() );
            DWCi_Acc_SetPlayerId( &stpLoginCnt->tempLoginId, MATH_Rand32( &randcontext, 0 ) );
        }

        DWCi_Acc_LoginIdToUserName( &stpLoginCnt->tempLoginId, stpLoginCnt->gamecode, stpLoginCnt->username );
    }

    // ゲーム内スクリーンネームを認証用データにコピーする
	MI_CpuCopy8(stpLoginCnt->playerName, auth.ingamesn, DWCi_WStrLen(stpLoginCnt->playerName)*2+2);

    DWC_Printf( DWC_REPORTFLAG_LOGIN, "  ingamesn : %s\n", auth.ingamesn );

    strcpy( auth.gsbrcd, &(stpLoginCnt->username[DWC_ACC_USERNAME_GSBRCD_OFFSET]) );
    DWC_Printf( DWC_REPORTFLAG_LOGIN, "  gsbrcd   : %s.\n", auth.gsbrcd );

    auth.alloc = (DWCAuthAlloc)DWC_Alloc;
    auth.free  = (DWCAuthFree)DWC_Free;

    http = DWC_AllocEx( DWC_ALLOCTYPE_AUTH, sizeof( DWCHttp ), 4 );
    SDK_ASSERTMSG( http, "DWC_LOGIN: can't alloc memory %s :%d\n", __FILE__, __LINE__ );

    // 開放用に覚えておく
    stpLoginCnt->http   = http;
    stpLoginCnt->startTick = OS_GetTick();

    // 認証処理の開始。
    DWC_Auth_Create( &auth, http );
}

static void DWCi_RemoteLoginProcess( void )
{
    if ( DWC_Auth_GetError() == DWCAUTH_E_FINISH )
    {
        DWCAuthResult result;
        DWC_Printf(DWC_REPORTFLAG_LOGIN, " *** Auth Done\n");
        DWC_Auth_GetResult( &result );

        strcpy( stpLoginCnt->authToken , result.token );
        strcpy( stpLoginCnt->partnerChallenge , result.challenge );

        DWC_Auth_Destroy();

        DWC_Free( DWC_ALLOCTYPE_AUTH, stpLoginCnt->http, 0 );
        stpLoginCnt->http = NULL;

        // 認証パケット取得（コールバックを呼び出す）
        // 本来なら、プレイヤーIDなどの処理も必要。
        if ( DWCi_Acc_IsAuthentic( stpLoginCnt->userdata ) )
        {
            stRemoteAuthCallback( stpLoginCnt->authToken, stpLoginCnt->partnerChallenge, pStRemoteAuthParam );
        }
        else
        {
            // 初めての接続なので、LoginIDを取得する。
            DWCi_GPConnect( stpLoginCnt->authToken, stpLoginCnt->partnerChallenge, (GPCallback)DWCi_GPConnectCallback, DWC_LOGIN_STATE_GPGETINFO );
        }
    }
    else if ( DWC_Auth_GetError() != DWCAUTH_E_NOERR )
    {
        OSTick nowTick;

        DWC_Printf(DWC_REPORTFLAG_LOGIN, " *** Auth Error\n");
        nowTick = OS_GetTick();
        if ( OS_TicksToMilliSeconds( nowTick - stpLoginCnt->startTick ) > DWC_LOGIN_AUTH_TIMEOUT )
        {
            DWCAuthResult result;
            DWC_Auth_GetResult( &result );

            // エラー発生。
            DWC_Auth_Destroy();
            DWC_Free( DWC_ALLOCTYPE_AUTH, stpLoginCnt->http, 0 );
            stpLoginCnt->http = NULL;

            DWCi_StopLogin( DWC_ERROR_AUTH_ANY, result.returncode );

            return;
            // NOT REACH HERE
        }
        else
        {
            // タイムアウト以下なので、同じパラメータでリトライ。
            DWCAuthParam auth;

            DWC_Printf( DWC_REPORTFLAG_LOGIN, "  **** AUTH FAILED ... retry AUTH_ERROR:%d.\n", DWC_Auth_GetError() );

            // 認証処理のやり直し。
            DWC_Auth_Destroy();
            
            MI_CpuClear8( &auth, sizeof(auth) );

            // ゲーム内スクリーンネームを認証用データにコピーする
            MI_CpuCopy8(stpLoginCnt->playerName, auth.ingamesn, DWCi_WStrLen(stpLoginCnt->playerName)*2+2);

            DWC_Printf( DWC_REPORTFLAG_LOGIN, "  ingamesn : %s\n", auth.ingamesn );
            
            strcpy( auth.gsbrcd, &(stpLoginCnt->username[DWC_ACC_USERNAME_GSBRCD_OFFSET]) );
            auth.alloc = (DWCAuthAlloc)DWC_Alloc;
            auth.free  = (DWCAuthFree)DWC_Free;

            DWC_Printf( DWC_REPORTFLAG_LOGIN, "  gsbrcd   : %s.\n", auth.gsbrcd );

            DWC_Auth_Create( &auth, stpLoginCnt->http );
        }
    }
}

static void DWCi_GPGetInfoCallback( GPConnection* pconnection, GPGetInfoResponseArg* arg, void* param )
{
#pragma unused( param )

    if ( arg->result == GP_NO_ERROR )
    {
        if ( stpLoginCnt->state == DWC_LOGIN_STATE_GPGETINFO )
        {
            if ( arg->lastname[0] == 0 )
            {
                GPResult gpResult;
                char pseudo_name[DWC_ACC_USERNAME_STRING_BUFSIZE];
                // profileIDがはじめて使われるので、lastnameをセットする。
                DWC_Printf( DWC_REPORTFLAG_LOGIN, "    login id is authenticated. set lastname field.\n" );
                DWCi_Acc_LoginIdToUserName( &stpLoginCnt->userdata->pseudo, stpLoginCnt->gamecode, pseudo_name );
                gpResult = gpSetInfos( pconnection, GP_LASTNAME, pseudo_name );
                if (DWCi_HandleGPError(gpResult)) return;

                stpLoginCnt->state = DWC_LOGIN_STATE_GPSETINFO;

                // 本当にセットされているかどうかを取得して確認する。
                gpResult = gpGetInfo( pconnection, arg->profile, GP_DONT_CHECK_CACHE, GP_NON_BLOCKING, (GPCallback)DWCi_GPGetInfoCallback, NULL );
                if (DWCi_HandleGPError(gpResult)) return;

                DWC_Printf( DWC_REPORTFLAG_LOGIN, "    call gpSetInfos\n" );
            }
            else
            {
                DWC_Printf( DWC_REPORTFLAG_LOGIN, "    this login id is used by anybody.... retry.\n" );

                // 一旦切断する。
                gpDisconnect( pconnection );

                // PlayerIDを変えてのリトライ。
                DWCi_RemoteLogin( DWCi_RemoteAuthCallback, NULL );
                stpLoginCnt->state = DWC_LOGIN_STATE_REMOTE_AUTH;
            }
        }
        else if ( stpLoginCnt->state == DWC_LOGIN_STATE_GPSETINFO )
        {
            // セットした後のベリファイ
            char pseudo_name[DWC_ACC_USERNAME_STRING_BUFSIZE];
            DWCi_Acc_LoginIdToUserName( &stpLoginCnt->userdata->pseudo, stpLoginCnt->gamecode, pseudo_name );

            if ( strcmp( arg->lastname, pseudo_name ) == 0 )
            {
                char username[DWC_ACC_USERNAME_STRING_BUFSIZE];
                DWCi_Acc_LoginIdToUserName( &stpLoginCnt->tempLoginId, stpLoginCnt->gamecode, username );
                DWC_Printf( DWC_REPORTFLAG_LOGIN, "    Account is created : %s(%s) - %d.\n", username, pseudo_name, arg->profile );

                DWCi_Acc_SetLoginIdToUserData( stpLoginCnt->userdata, &stpLoginCnt->tempLoginId, arg->profile );

                // [nakata] 切断/再接続は無駄なのでやめてみる
                //gpDisconnect( pconnection ); // 接続状態を解除する。

                // 本番の接続。
                //stRemoteAuthCallback( stpLoginCnt->authToken, stpLoginCnt->partnerChallenge, pStRemoteAuthParam );
                stpLoginCnt->state = DWC_LOGIN_STATE_CONNECTING;
                DWCi_GPConnectCallback(pconnection, &stpLoginCnt->gpconnectresponsearg, NULL);
            }
            else
            {
                // [todo] retry
                GPResult gpResult;
                DWC_Printf( DWC_REPORTFLAG_LOGIN, "    Login but gpSetInfo failed... %s : %d retry gpGetInfo.\n", arg->lastname, arg->profile );

                gpResult = gpGetInfo( pconnection, arg->profile, GP_DONT_CHECK_CACHE, GP_NON_BLOCKING, (GPCallback)DWCi_GPGetInfoCallback, NULL );
                if (DWCi_HandleGPError(gpResult)) return;
            }
        }
    }
    else
    {
        DWC_Printf( DWC_REPORTFLAG_LOGIN, " ERROR: gpGetInfo. why??? : %d\n", arg->result );
    }
}

BOOL
DWCi_CheckLogin( void )
{
    if ( stpLoginCnt != NULL && stpLoginCnt->state == DWC_LOGIN_STATE_CONNECTED )
    {
        return TRUE;
    }

    return FALSE;
}

BOOL
DWCi_GetAuthInfo( char** authToken, char** partnerChallenge )
{
    if ( !DWCi_CheckLogin() ) return FALSE;
    
    *authToken        = stpLoginCnt->authToken;
    *partnerChallenge = stpLoginCnt->partnerChallenge;

    return TRUE;
}
