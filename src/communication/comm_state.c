//=============================================================================
/**
 * @file	comm_state.c
 * @brief	通信状態を管理するサービス  通信の上位にある
 *          スレッドのひとつとして働き、自分の通信状態や他の機器の
 *          開始や終了を管理する
 *          フィールド管理をしない部分をもう一回ここに引越し...2006.01.12
 * @author	k.ohno
 * @date    2006.01.12
 * @data	2007.12.06	Wi-Fiロビー専用ログイン処理の追加	tomoya
 */
//=============================================================================

#include "common.h"
#include "communication/communication.h"
#include "wh.h"
#include "comm_local.h"
#include "system/gamedata.h"
#include "system/pm_str.h"

#include "wifi/dwc_rap.h"   //WIFI
#include "wifi/dwc_lobbylib.h"   //WIFIlobby
#include "system/snd_tool.h"  //sndTOOL

#include  "communication/wm_icon.h"

#include "communication/comm_state.h"

//==============================================================================
//	型宣言
//==============================================================================
// コールバック関数の書式
typedef void (*PTRStateFunc)(void);

//==============================================================================
// ワーク
//==============================================================================

typedef struct{
    void* pWifiFriendStatus;
    u8 select[6];
    MATHRandContext32 sRand; ///< 親子機ネゴシエーション用乱数キー
    PTRStateFunc state;
    SAVEDATA* pSaveData;
    MYSTATUS* pMyStatus;
    const REGULATION* pReg;
    COMMSTATE_DWCERROR aError;
    int timer;
    u16 reConnectTime;  // 再接続時に使用するタイマー
#ifdef PM_DEBUG		// Debug ROM
    u16 debugTimer;
#endif
    u8 limitNum;      // 受付制限したい場合のLIMIT数
    u8 nowConnectNum;      // 現在の接続人数
    u8 negotiation;   // 接続認定
    u8 connectIndex;   // 子機が接続する親機のindex番号
    u8 serviceNo;      // 通信サービス番号
    u8 regulationNo;   // コンテスト種別を決める番号
#ifdef PM_DEBUG		// Debug ROM
    u8 soloDebugNo;
    u8 bDebugStart;
#endif
    u8 disconnectIndex;  //wifi切断方向 親機子機
    s8 wifiTargetNo;   // WIFI接続を行う人の番号
    u8 bFirstParent;   // 繰り返し親子切り替えを行う場合の最初の親状態
    u8 bDisconnectError; // 切断エラーをエラーにするときTRUE
    u8 bErrorAuto;     // 自動エラー検索起動TRUE
    u8 bWorldWifi;     // DPWが動いている場合
    u8 ResetStateType;    // エラー状態に入りソフトウエアリセットを待つ状態
    u8 bUnionPause;    // ユニオンルームで接続を防ぎたい場合
    u8 partyGameBit;   // 他社のGGIDを拾う
    u8 bParentOnly;   // 親機状態にしかならない
    u8 bChildOnly;   // 子機状態にしかならない
    u8 bNotConnect;   // 通信状態に遷移しない
    u8 bWifiDisconnect; //WIFI切断用 コマンド受け取った場合に1
    u8 stateError;         //エラー扱いにする場合1以上
    u8 bPauseFlg;

	// プラチナで追加
    const void* cp_lobby_init_profile;	// ロビー用初期化プロフィール
	u8 lobby_dwc_login;					// DWC_Login_Async完了チェック用
	u8 lobby_dummy[3];
} _COMM_STATE_WORK;

static _COMM_STATE_WORK* _pCommState = NULL;  ///<　ワーク構造体のポインタ




// WiFiロビーデバック開始用
#ifdef COMMST_DEBUG_WFLBY_START
static u32 COMMST_DEBUG_WFLBY_START_season;
static u32 COMMST_DEBUG_WFLBY_START_room;
#endif
#ifdef PM_DEBUG
extern u32 D_Tomoya_WiFiLobby_ChannelPrefix;
extern BOOL D_Tomoya_WiFiLobby_ChannelPrefixFlag;
#endif

//==============================================================================
// 定義
//==============================================================================

//----#if TESTOHNO
#define _HEAPSIZE_BATTLE           (0x7080)  // バトル機領域
#define _HEAPSIZE_WIFILOBBY        (0x7080)  // Wi-Fiロビー領域
#define _HEAPSIZE_UNDERGROUND      (0xF000)  // 地下領域
#define _HEAPSIZE_UNION            (0x7080)  // ユニオンルーム領域
#define _HEAPSIZE_POKETCH          (0x7000)  // ぽけっち機領域
#define _HEAPSIZE_PARTY            (0x7000)  // パーティーサーチ
//#define _HEAPSIZE_WIFI            (0x2A000+0x7000)  //DWCRAP が使用する領域(DP)
#define _HEAPSIZE_WIFI            (0x2A000+0xA000 + 0x1400)  //DWCRAP が使用する領域(VCTなし)
#define _HEAPSIZE_WIFI_LOBBY      (0x60000)  //LOBBYを含めDWCRAPが使う領域VCTなし

//#define _HEAPSIZE_DWCLOBBY        (0x50000)  //DWCに許可するHEAP量
#define _HEAPSIZE_DWCLOBBY        (0x58000)  //DWCに許可するHEAP量
#define _HEAPSIZE_DWCP2P          (0x2B000+ 0x1400)

//#define _HEAPSIZE_WIFI            (0x2A000+0xB000)  //DWCRAP が使用する領域(VCTあり)
//#define _HEAPSIZE_WIFI            (0x2A000+0xB800)  //DWCRAP が使用する領域(VCTあり)
#define _HEAPSIZE_DPW              (0x100)   // 世界交換　タワー用

//----#endif //TESTOHNO

#define _PACKETSIZE_BATTLE         (512)//(1376)  // バトル機領域
#define _PACKETSIZE_UNDERGROUND     (500)  // 地下領域
#define _PACKETSIZE_UNION          (512)//(1280)  // ユニオンルーム領域
#define _PACKETSIZE_POKETCH          (32)  // ぽけっち領域
#define _PACKETSIZE_PARTY         (32)  // ぽけっち領域


#define _START_TIME (50)     // 開始時間
#define _CHILD_P_SEARCH_TIME (32) ///子機として親を探す時間
#define _PARENT_WAIT_TIME (40) ///親としてのんびり待つ時間
#define _FINALIZE_TIME (2)
#define _EXIT_SENDING_TIME (5)
#define _EXIT_SENDING_TIME2 (15)
#define _PARENT_END_TIME (2)
#define _SEND_NAME_TIME (10)
#define _PARENTSCAN_PA (3)  // 親機として検索する確立は1/3

#define _RETRY_COUNT_UNION  (3)  // ユニオンルームで子機が接続に要する回数

#define _TCB_COMMCHECK_PRT   (10)    ///< フィールドを歩く通信の監視ルーチンのPRI


#define _NEGOTIATION_OK (1)      // 接続確認完了
#define _NEGOTIATION_CHECK (0)   // 接続確認中
#define _NEGOTIATION_NG (2)     // 接続失敗

// ログイン時のタイムアウト
#define MYDWC_TIMEOUTLOGIN (30*60*2)



//==============================================================================
// static宣言
//==============================================================================

// ステートの初期化

static void _changeStateDebug(PTRStateFunc state, int time, int line);  // ステートを変更する
static void _changeState(PTRStateFunc state, int time);  // ステートを変更する

#ifdef PM_DEBUG
#if 1
#define   _CHANGE_STATE(state, time)  _changeStateDebug(state, time, __LINE__)
#else
#define   _CHANGE_STATE(state, time)  _changeState(state, time)
#endif
#else  //PM_DEBUG
#define   _CHANGE_STATE(state, time)  _changeState(state, time)
#endif //PM_DEBUG


// 地下関連ステート
static void _underStart(void);           // 子機の初期化 + システムの初期化
static void _underChildPInit(void);      // 子機の初期化
static void _underChildFInit(void);
static void _underChildPSearching(void); // 子機が親機を探している
static void _underChildForceConnect(void); // 子機が親機に強引に接続中
static void _underChildConnecting(void);   // 子機が親機に接続中
static void _underChildConnect(void);    // 子機として接続中
static void _underChildReset(void);
static void _underChildFinalize(void);   // 子機になれなかったので終了処理中
static void _underParentFinalize(void);  // 親機になれなかったので終了処理中
static void _underParentInit(void);      // 親機として初期化
static void _underParentWait(void);      // 親機として待機状態
static void _underParentConnectInit(void); // 接続したので初期化中
static void _underParentConnect(void);   // 親機として接続中
static void _stateUnderGroundConnectEnd(void);  // 地下切断
static void _underSBReset(void);


// バトル関連ステート
static void _battleParentInit(void);     // 戦闘用親機として初期化
static void _battleParentWaiting(void);  // 戦闘用親機として待機中
static void _battleParentMoveRoomEnter(void);  // 戦闘前の部屋に入って待機中
static void _battleParentMoveRoom(void);  // 戦闘前の部屋に入って待機中
static void _battleParentSendName(void);  // 自分の名前をみんなに送信
static void _battleParentReTry(void);   // 戦闘用親機を中断

static void _battleChildInit(void);     // 戦闘用子機として初期化
static void _battleChildBconScanning(void);  // 戦闘用子機として親機選択中
static void _battleChildConnecting(void);  // 接続許可もらい中
static void _battleChildSendName(void);  // 自分の名前をみんなに送信
static void _battleChildWaiting(void);  // ダイレクト用子機待機中
static void _battleChildMoveRoomEnter(void); // 戦闘前の部屋に入って待機中
static void _battleChildMoveRoom(void); // 戦闘前の部屋に入って待機中
static void _battleChildReTry(void);   // 子機を中断
static void _battleChildReInit(void);   // 子機を再起動
static void _battleChildReset(void);
static void _battleChildReConnect(void);
static void _underChildOnline(void);


// UNIONルーム関連ステート
static void _unionStart(void);
static void _unionChildSearching(void);
static void _unionChildFinalize(void);
static void _unionParentInit(void);
static void _unionParentWait(void);
static void _unionChildRestart(void);
static void _unionChildNegotiation(void);

static void _unionForceConnectStart(void);
static void _unionForceConnectStart2(void);
static void _unionForceConnect(void);
static void _unionChildConnecting(void);
static void _unionChildConnectSuccess(void);
static void _unionChildConnectFailed(void);
static void _unionChildReset(void);
static void _unionParentConnect(void);
static void _unionParentPause(void);

static void _mysteryParentInit(void);
static void _mysteryChildInit(void);

// その他一般的なステート
static void _stateNone(void);            // 何もしない
static void _stateConnectError(void);    // 接続エラー状態
static void _stateEnd(void);             // 終了処理
static void _stateConnectChildEndWait(void);   // 子機の終了を待って終わる
static void _stateConnectChildEnd(void);
static void _stateConnectEnd(void);      // 切断処理開始
static void _stateConnectAutoEnd(void);  // 自動切断処理開始


// WIFI用
static void _wifiBattleLogin(void);
static void _stateWifiLogout(void);

// Wi-Fiロビー専用
static void _wifiLobbyCommInit( void );		// 通信初期化
static void _wifiLobbyLogin( void );		// Lobbyサーバにログイン
static void _wifiLobbyConnect( void );		// ログイン完了
static void _wifiLobbyLogout( void );		// Lobbyサーバからログアウト
static void _wifiLobbyLogoutWait( void );	// ログアウト待ち
static void _wifiLobbyError( void );		// Lobby特有のエラー
static void _wifiLobbyP2PMatch( void );		// LobbyP2P接続処理
static void _wifiLobbyP2PMatchWait( void );	// LobbyP2P接続待ち処理
static void _wifiLobbyP2PDisconnect( void );
static void _wifiLobbyTimeout( void );

static BOOL _wifiLobbyDwcStepRetCheck( int dwc_err );
static BOOL _wifiLobbyDwcStepRetCheckEasy( int dwc_err );
static BOOL _wifiLobbyUpdate( void );
static BOOL _wifiLobbyUpdateCommon( void );	// Lobby状態更新処理
static BOOL _wifiLobbyUpdateMatchCommon( u32* p_matchret );	// Lobby状態更新	マッチング中用



// ネゴシエーション用確認KEY
static u8 _negotiationMsg[]={"FREAK"};
static u8 _negotiationMsgReturnOK[]={" GAME"};
static u8 _negotiationMsgReturnNG[]={" FULL"};


#ifdef PM_DEBUG
static u8 _debugConnectNo = 0;
static u8 _debugParentOnly = 0;
static u8 _debugChildOnly = 0;

void DebugOhnoCommDebugUnderNo(int no)
{
    _debugConnectNo = no;
}

void DebugOhnoCommDebugUnderParentOnly(int no)
{
    _debugParentOnly = no;
}

void DebugOhnoCommDebugUnderChildOnly(int no)
{
    _debugChildOnly = no;
}

#endif //PM_DEBUG

//==============================================================================
/**
 * 通信管理ステートの初期化処理
 * @param   MYSTATUS* pMyStatus
 * @retval  none
 */
//==============================================================================

static void _commStateInitialize(SAVEDATA* pSaveData,int serviceNo)
{
    void* pWork;

    if(_pCommState!=NULL){   // すでに動作中の場合必要ない
        return;
    }
    GF_ASSERT(pSaveData);
    CommVRAMDInitialize();
    // 初期化
    _pCommState = (_COMM_STATE_WORK*)sys_AllocMemory(HEAPID_COMMUNICATION, sizeof(_COMM_STATE_WORK));
    MI_CpuFill8(_pCommState, 0, sizeof(_COMM_STATE_WORK));
    _pCommState->timer = _START_TIME;
    _pCommState->bFirstParent = TRUE;  // 親の初めての起動の場合TRUE
    _pCommState->pSaveData = pSaveData;
    _pCommState->pMyStatus = SaveData_GetMyStatus(pSaveData);
    _pCommState->limitNum = COMM_MODE_UNION_NUM_MIN + 1;   // 一人は最低でも接続可能
    _pCommState->negotiation = _NEGOTIATION_CHECK;
    _pCommState->bUnionPause = FALSE;
    _pCommState->serviceNo = serviceNo;
    //_pCommState->bWifi = FALSE;
    CommRandSeedInitialize(&_pCommState->sRand);
    CommCommandInitialize(NULL, 0, NULL);

    if((serviceNo != COMM_MODE_UNION) && (serviceNo != COMM_MODE_PARTY) &&
       (serviceNo != COMM_MODE_MYSTERY)){
        WirelessIconEasy();
    }
}


//==============================================================================
/**
 * 通信管理ステートの終了処理
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _stateFinalize(void)
{
    if(_pCommState==NULL){  // すでに終了している
        return;
    }
//    TCB_Delete(_pCommState->pTcb);
    CommCommandFinalize();
    if(_pCommState->pWifiFriendStatus){
        sys_FreeMemoryEz(_pCommState->pWifiFriendStatus);
    }
    //  バトルファクトリーでの動作が変わってしまう不具合の修正の時に見つけたので修正
    if(CommStateIsWifiConnect()){
//    if(_pCommState->serviceNo >= COMM_MODE_BATTLE_SINGLE_WIFI){
        sys_DeleteHeap(HEAPID_WIFIMENU);
    }
    WirelessIconEasyEnd();
    CommVRAMDFinalize();
    sys_FreeMemoryEz(_pCommState);
    sys_DeleteHeap(HEAPID_COMMUNICATION);
    _pCommState = NULL;
}

//==============================================================================
/**
 * 初期化がすんでいるかどうか
 * @param   none
 * @retval  すんでいる場合TRUE
 */
//==============================================================================

BOOL CommStateIsInitialize(void)
{
    if(_pCommState){
        return TRUE;
    }
    return FALSE;
}

//==============================================================================
/**
 * 地下にはいった時の通信処理
 * @param   MYSTATUS* pMyStatus
 * @retval  none
 */
//==============================================================================

void CommStateEnterUnderGround(SAVEDATA* pSaveData)
{
    if(_pCommState!=NULL){ // つながっている場合今は除外する
        return;
    }
    // 通信ヒープ作成
    sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_COMMUNICATION, _HEAPSIZE_UNDERGROUND );
    _commStateInitialize(pSaveData,COMM_MODE_UNDERGROUND);
//    _pCommState->serviceNo = COMM_MODE_UNDERGROUND;
    _pCommState->regulationNo = 0;
#ifdef PM_DEBUG
    _pCommState->soloDebugNo = _debugConnectNo + SOLO_DEBUG_NO;
    _pCommState->bParentOnly = _debugParentOnly;
    _pCommState->bChildOnly = _debugChildOnly;
#endif
    // 地下ステートの遷移のため初期化
    _CHANGE_STATE(_underStart, _START_TIME);
}

//==============================================================================
/**
 * 地下を出る場合の通信処理
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommStateExitUnderGround(void)
{
    if(_pCommState==NULL){  // すでに終了している
        return;
    }
    // 切断ステートに移行する  すぐに切れない
    CommSystemShutdown();
    _CHANGE_STATE(_stateUnderGroundConnectEnd, 0);
}

//==============================================================================
/**
 * ビーコン収集を再開する
 * @param   MYSTATUS* pMyStatus
 * @retval  none
 */
//==============================================================================

void CommStateUnderRestart(void)
{
    CommSystemReset();   // 今までの通信バッファをクリーンにする
    // ぐるぐるまわす
    _pCommState->reConnectTime = CommGetCurrentID();
    _CHANGE_STATE(_underChildReset, 0);  // エラー終了の場合RESETする
}

//==============================================================================
/**
 * ビーコン収集を停止する
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommStaetUnderPause(BOOL bFlg)
{
    _pCommState->bPauseFlg = bFlg;
}


//==============================================================================
/**
 * ビーコン収集を再開する 親機側のリスターと
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underQueueReset(void)
{

    if(!CommMPSwitchParentChild()){
        return;
    }
    CommSystemReset();         // 今までの通信バッファをクリーンにする
    _CHANGE_STATE(_underChildFinalize, 0);

    /*
    if(_pCommState->timer!=0){
        _pCommState->timer--;
        return;
    }
    if(CommIsEmptyQueue_ServerSize()){
        CommSystemResetQueue_Server();
    }
    OHNO_PRINT("送信キューけし\n");
       */
//    _CHANGE_STATE(_underChildFinalize, _FINALIZE_TIME);
}

void CommStateUnderParentRestart(void)
{
   // _CHANGE_STATE(_underQueueReset, 0);
    _CHANGE_STATE(_underChildFinalize, 0);

}


//==============================================================================
/**
 * 地下においてリセット中なのかどうか返す
 * @param   none
 * @retval  RESET中ならTRUE
 */
//==============================================================================

BOOL CommIsUnderResetState(void)
{
    int i;
    u32 stateAddr = (u32)_pCommState->state;

    if(_pCommState==NULL){  // すでに終了している
        return FALSE;
    }
    if(stateAddr == (u32)_underQueueReset){
        return TRUE;
    }
    return FALSE;
}

//==============================================================================
/**
 * 地下において接続中STATEなのかどうか返す
 * @param   none
 * @retval  接続中ならTRUE
 */
//==============================================================================

BOOL CommIsUnderGroundConnectingState(void)
{
    int i;
    u32 funcTbl[]={
        (u32)_underChildConnect,
        (u32)_underParentConnect,
        0,
    };
    u32 stateAddr = (u32)_pCommState->state;

    if(_pCommState==NULL){  // すでに終了している
        return FALSE;
    }
    for(i = 0; funcTbl[i] != 0; i++ ){
        if(stateAddr == funcTbl[i]){
            return TRUE;
        }
    }
    return FALSE;
}

//==============================================================================
/**
 * 通信を切り秘密基地にはいる
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommStateUnderGroundOfflineSecretBase(void)
{
    // 切断ステートに移行する  すぐに切れない
    _CHANGE_STATE(_underSBReset, 0);  // エラー終了の場合RESETする
}

//==============================================================================
/**
 * はじめのイベント
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommStateSetFirstEvent(void)
{
    _pCommState->bNotConnect = TRUE;
}





//==============================================================================
/**
 * 通信を繋ぎ秘密基地から戻る
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommStateUnderGroundOnlineSecretBase(void)
{
    CommVRAMDInitialize();
    _CHANGE_STATE(_underChildOnline, 0);  // エラー終了の場合RESETする
}

//==============================================================================
/**
 * バトル時の親としての通信処理開始
 * @param   pMyStatus  mystatus
 * @param   serviceNo  通信サービス番号
 * @param   regulationNo  通信サービス番号
 * @retval  none
 */
//==============================================================================

#ifdef PM_DEBUG
void CommStateEnterBattleParent(SAVEDATA* pSaveData, int serviceNo, int regulationNo, const REGULATION* pReg, BOOL bWifi, int soloDebugNo)
#else
void CommStateEnterBattleParent(SAVEDATA* pSaveData, int serviceNo, int regulationNo, const REGULATION* pReg, BOOL bWifi)
#endif
{
    if(CommIsInitialize()){
        return;      // つながっている場合今は除外する
    }
    // 通信ヒープ作成
    sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_COMMUNICATION, _HEAPSIZE_BATTLE );
    _commStateInitialize(pSaveData,serviceNo);
//    _pCommState->serviceNo = serviceNo;
    _pCommState->regulationNo = regulationNo;
    _pCommState->pReg = pReg;
#ifdef PM_DEBUG
    _pCommState->soloDebugNo = soloDebugNo;
#endif
    _CHANGE_STATE(_battleParentInit, 0);
}

//==============================================================================
/**
 * バトル時の子としての通信処理開始
 * @param   serviceNo  通信サービス番号
 * @retval  none
 */
//==============================================================================

#ifdef PM_DEBUG
void CommStateEnterBattleChild(SAVEDATA* pSaveData, int serviceNo, int regulationNo, const REGULATION* pReg,BOOL bWifi, int soloDebugNo)
#else
void CommStateEnterBattleChild(SAVEDATA* pSaveData, int serviceNo, int regulationNo, const REGULATION* pReg,BOOL bWifi)
#endif
{
    if(CommIsInitialize()){
        return;      // つながっている場合今は除外する
    }
    // 通信ヒープ作成
    sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_COMMUNICATION, _HEAPSIZE_BATTLE );
    _commStateInitialize(pSaveData,serviceNo);
//    _pCommState->serviceNo = serviceNo;
    _pCommState->regulationNo = regulationNo;
    _pCommState->pReg = pReg;
#ifdef PM_DEBUG
    _pCommState->soloDebugNo = soloDebugNo;
#endif
    _CHANGE_STATE(_battleChildInit, 0);
}

//==============================================================================
/**
 * バトル時の子としての通信処理開始
 * @param   connectIndex 接続する親機のIndex
 * @retval  none
 */
//==============================================================================

void CommStateConnectBattleChild(int connectIndex)
{
    _pCommState->connectIndex = connectIndex;
    _CHANGE_STATE(_battleChildConnecting, 0);
}

//==============================================================================
/**
 * バトル時の子としての通信再起動処理
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommStateRebootBattleChild(void)
{
    CommSystemResetBattleChild();
    _CHANGE_STATE(_battleChildReTry, 0);
}

//==============================================================================
/**
 * バトル時の通信処理終了手続き
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommStateExitBattle(void)
{
    if(_pCommState==NULL){
        return;      // すでに終了している場合は除外
    }
    _CHANGE_STATE(_stateConnectEnd, _EXIT_SENDING_TIME);
    //_CHANGE_STATE(_stateConnectAutoEnd, _EXIT_SENDING_TIME);
}

//==============================================================================
/**
 * 子機受付できるかどうか
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommStateSetEntryChildEnable(BOOL bEntry)
{
    WHSetEntry(bEntry);
}


//==============================================================================
/**
 * バトル接続中STATEなのかどうか返す
 * @param   none
 * @retval  接続中TRUE
 */
//==============================================================================

BOOL CommIsBattleConnectingState(void)
{
    int i;
    u32 funcTbl[]={
        (u32)_battleParentWaiting,
        (u32)_battleChildWaiting,
        0,
    };
    u32 stateAddr = (u32)_pCommState->state;

    if(_pCommState==NULL){  // すでに終了している
        return FALSE;
    }
    for(i = 0; funcTbl[i] != 0; i++ ){
        if(stateAddr == funcTbl[i]){
            return TRUE;
        }
    }
    return FALSE;
}

//==============================================================================
/**
 * ビーコン収集を開始する
 * @param   MYSTATUS* pMyStatus
 * @retval  none
 */
//==============================================================================

void CommStateUnionBconCollection(SAVEDATA* pSaveData)
{
    if(_pCommState!=NULL){ // つながっている場合今は除外する
        return;
    }
    // 通信ヒープ作成
    if(sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_COMMUNICATION, _HEAPSIZE_UNION )==0){
		// 通信ヒープが作成できなかった。
		// 続行不可能(電源切りエラーへ）
		CommFatalErrorFunc_NoNumber();

	}
    _commStateInitialize(pSaveData,COMM_MODE_UNION);
    _pCommState->serviceNo = COMM_MODE_UNION;
    _pCommState->regulationNo = 0;
#ifdef PM_DEBUG
    _pCommState->soloDebugNo = SOLO_DEBUG_NO;
#endif
    // ステートの遷移のため初期化
    _CHANGE_STATE(_unionStart, 0);
}


//==============================================================================
/**
 * 会話を開始したので子機接続
 * @param   接続する親機index
 * @retval  none
 */
//==============================================================================

void CommStateUnionConnectStart(int index)
{
    _pCommState->connectIndex = index;
    _pCommState->reConnectTime = _RETRY_COUNT_UNION;
    WHParentConnectPause(TRUE);  // 親機にはならない
    WirelessIconEasy();
    _CHANGE_STATE(_unionForceConnectStart, 0);
}

//==============================================================================
/**
 * 子機接続に成功したかどうか
 * @param   none
 * @retval  接続中＝０　成功＝１　失敗＝－１
 */
//==============================================================================

int CommStateIsUnionConnectSuccess(void)
{
    u32 stateAddr;

    if(_pCommState==NULL){  // すでに終了している
        return -1;
    }
    stateAddr = (u32)_pCommState->state;
    if(stateAddr == (u32)_unionChildConnectSuccess){
        return 1;
    }
    if(stateAddr == (u32)_unionChildConnectFailed){
        return -1;
    }
    return 0;
}

//==============================================================================
/**
 * 親機接続に成功したかどうか
 * @param   none
 * @retval  接続中ならTRUE
 */
//==============================================================================

BOOL CommStateIsUnionParentConnectSuccess(void)
{
    u32 stateAddr;

    if(_pCommState==NULL){  // すでに終了している もしくは初期化がまだ
        return FALSE;
    }
    stateAddr = (u32)_pCommState->state;
    if(stateAddr == (u32)_unionParentConnect){
        return TRUE;
    }
    return FALSE;
}

//==============================================================================
/**
 * ビーコン収集を再開する
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommStateUnionBconCollectionRestart(void)
{
	OS_TPrintf( "リスタートしました！\n" );

    WHParentConnectPause(FALSE);

	if( !(_pCommState->bDisconnectError == TRUE && _pCommState->bErrorAuto == TRUE) ||
		_pCommState->serviceNo == COMM_MODE_UNION ){
	   CommStateSetErrorCheck(FALSE,FALSE);
	}

    WirelessIconEasyEnd();
    _pCommState->serviceNo = COMM_MODE_UNION;
    _pCommState->bUnionPause = FALSE;
    WH_SetMaxEntry(_pCommState->limitNum);

    if(CommGetCurrentID() == COMM_PARENT_ID){
        WHParentConnectPauseSystem(TRUE);
        _CHANGE_STATE(_stateConnectChildEndWait, _EXIT_SENDING_TIME2);
    }
    else{
        _CHANGE_STATE(_stateConnectChildEnd, _EXIT_SENDING_TIME);
    }
}

//==============================================================================
/**
 * ビーコン収集を再開を確認する
 * @param   none
 * @retval  none
 */
//==============================================================================

BOOL CommStateUnionIsRestartSuccess(void)
{
    u32 stateAddr;

    if(_pCommState==NULL){
        return TRUE;  // 
    }
    stateAddr = (u32)_pCommState->state;
    if(stateAddr == (u32)_unionChildFinalize){
        return FALSE;
    }
    return TRUE;
}

//==============================================================================
/**
 * UNIONROOMを出る場合の通信処理
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommStateExitUnion(void)
{
    if(_pCommState==NULL){  // すでに終了している
        return;
    }
    OHNO_SP_PRINT(">> UNIONいったん切断\n");
    // 切断ステートに移行する  すぐに切れない
    
    _CHANGE_STATE(_stateConnectEnd, _EXIT_SENDING_TIME);
}

//==============================================================================
/**
 * 子機としてつながる予定なので、親機にはならない
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommStateChildReserve(void)
{
    if(_pCommState==NULL){  // すでに終了している
        return;
    }
    _pCommState->bUnionPause = TRUE;
    WHParentConnectPause(TRUE);
}

//==============================================================================
/**
 * UNIONROOM一時停止  再開はReStart
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommStateUnionPause(void)
{
    if(_pCommState==NULL){  // すでに終了している
        return;
    }
    _pCommState->bUnionPause = TRUE;
    WHParentConnectPause(TRUE);
    _CHANGE_STATE(_unionParentPause, 0);
}

//==============================================================================
/**
 * UNIONROOMアプリケーションスタート
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommStateUnionAppStart(void)
{
    _pCommState->serviceNo = COMM_MODE_UNION_APP;
}

//==============================================================================
/**
 * UNIONROOMアプリケーションエンド
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommStateUnionAppEnd(void)
{
    _pCommState->serviceNo = COMM_MODE_UNION;
}

//==============================================================================
/**
 * 不思議通信としての通信処理開始
 * @param   pMyStatus  mystatus
 * @param   serviceNo  通信サービス番号
 * @param   regulationNo  通信サービス番号
 * @retval  none
 */
//==============================================================================

void CommStateEnterMysteryParent(SAVEDATA* pSaveData, int serviceNo)
{
    if(CommIsInitialize()){
        return;      // つながっている場合今は除外する
    }
    // 通信ヒープ作成
    sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_COMMUNICATION, _HEAPSIZE_BATTLE );
    _commStateInitialize(pSaveData,serviceNo);
//    _pCommState->serviceNo = serviceNo;
#ifdef PM_DEBUG
    _pCommState->soloDebugNo = 0;
#endif
    _CHANGE_STATE(_mysteryParentInit, 0);
}

//==============================================================================
/**
 * 不思議通信の子としての通信処理開始
 * @param   serviceNo  通信サービス番号
 * @retval  none
 */
//==============================================================================

void CommStateEnterMysteryChild(SAVEDATA* pSaveData, int serviceNo)
{
    if(CommIsInitialize()){
        return;      // つながっている場合今は除外する
    }
    // 通信ヒープ作成
    sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_COMMUNICATION, _HEAPSIZE_BATTLE );
    _commStateInitialize(pSaveData,serviceNo);
//    _pCommState->serviceNo = serviceNo;
#ifdef PM_DEBUG
    _pCommState->soloDebugNo = 0;
#endif
    _CHANGE_STATE(_mysteryChildInit, 0);
}


//==============================================================================
/**
 * 通信管理ステートの処理
 * @param
 * @retval  none
 */
//==============================================================================

void CommStateCheckFunc(void)
{

    if(_pCommState){
        if(_pCommState->state != NULL){
            PTRStateFunc state = _pCommState->state;
            state();
        }
    }
    if(CommStateIsWifiConnect()){
        WirelessIconEasy_SetLevel(WM_LINK_LEVEL_3 - DWC_GetLinkLevel());
    }
    else if(CommMPIsInitialize()){
        WirelessIconEasy_SetLevel(WM_LINK_LEVEL_3 - WM_GetLinkLevel());
    }
}

//==============================================================================
/**
 * 通信管理ステートの変更
 * @param   state  変えるステートの関数
 * @param   time   ステート保持時間
 * @retval  none
 */
//==============================================================================

static void _changeState(PTRStateFunc state, int time)
{
    _pCommState->state = state;
    _pCommState->timer = time;
}

//==============================================================================
/**
 * 通信管理ステートの変更
 * @param   state  変えるステートの関数
 * @param   time   ステート保持時間
 * @retval  none
 */
//==============================================================================
#ifdef PM_DEBUG
static void _changeStateDebug(PTRStateFunc state, int time, int line)
{
    OHNO_PRINT("comm_state: %d\n",line);
    _changeState(state, time);
}
#endif

//==============================================================================
/**
 * 地下スタート
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underStart(void)
{
    void* pWork;

    if(_pCommState->timer!=0){
        _pCommState->timer--;
        return;
    }
    if(!CommIsVRAMDInitialize()){
        return;  //
    }
    CommMPInitialize(_pCommState->pMyStatus, TRUE);
    CommInfoInitialize(_pCommState->pSaveData, NULL);
    CommSetAloneMode(TRUE);
    CommEnableSendMoveData(); //commsystemにおいてあるキーデータの送信を許可

    if(_pCommState->bParentOnly){
        if(CommParentModeInit(TRUE, _pCommState->bFirstParent, _PACKETSIZE_UNDERGROUND, TRUE))  {
            u32 rand = MATH_Rand32(&_pCommState->sRand, _PARENT_WAIT_TIME/2);
            _pCommState->bFirstParent = FALSE;
            _CHANGE_STATE(_underParentWait, _PARENT_WAIT_TIME/2+rand);
        }
    }
    else{
        // まず子機になってみて、親機を探す  理由：親機がいたらすばやくつながるから
        if(CommChildModeInit(TRUE, TRUE, _PACKETSIZE_UNDERGROUND)){
            if(_pCommState->bNotConnect){
                _CHANGE_STATE(_stateNone,0);
            }
            else{
                _CHANGE_STATE(_underChildPSearching, _CHILD_P_SEARCH_TIME*2);
            }
        }
    }
}

//==============================================================================
/**
 * リセット後の再スタート
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underChildFInit(void)
{
    BOOL bSuccess;

    if(!CommMPIsStateIdle()){  // 終了処理がきちんと終わっていることを確認
        return;
    }
    CommSetAloneMode(TRUE);  // 非接続状態
    CommEnableSendMoveData(); //commsystemにおいてあるキーデータの送信を許可

    if(_pCommState->bParentOnly){
        _CHANGE_STATE(_underParentInit, 0);
    }
    else{
        bSuccess = CommChildModeInit(FALSE, TRUE, _PACKETSIZE_UNDERGROUND);
        if(bSuccess){
            u32 rand = MATH_Rand32(&_pCommState->sRand, _CHILD_P_SEARCH_TIME*2);
            if(_pCommState->reConnectTime > 0){
                //OHNO_PRINT("再接続時間考慮 %d \n", _pCommState->reConnectTime);
                // 再接続時間考慮
                rand = 2 * _CHILD_P_SEARCH_TIME * _pCommState->reConnectTime;
                _pCommState->reConnectTime = 0;
            }
            _CHANGE_STATE(_underChildPSearching, rand);
        }
    }
}

//==============================================================================
/**
 * 子機再スタート
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underChildPInit(void)
{
    BOOL bSuccess;
    
    if(!CommMPIsStateIdle()){  // 終了処理がきちんと終わっていることを確認
        return;
    }
    bSuccess = CommChildModeInit(FALSE, FALSE, _PACKETSIZE_UNDERGROUND);
    if(bSuccess){
        u32 rand = MATH_Rand32(&_pCommState->sRand, _CHILD_P_SEARCH_TIME);
        _CHANGE_STATE(_underChildPSearching, _CHILD_P_SEARCH_TIME/2 + rand);
    }
}

//==============================================================================
/**
 * 子機となって親機を探し中
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underChildPSearching(void)
{
    int realParent;
    
    CommMPParentBconCheck();  // bconの検査

    if(_pCommState->bPauseFlg || _pCommState->bChildOnly){
        return;
    }
    
    realParent = CommMPGetFastConnectIndex();  //つながったことがある人がいたら
    if(realParent != -1){
        _pCommState->connectIndex = realParent;  // 記憶
        _CHANGE_STATE(_underChildForceConnect, _CHILD_P_SEARCH_TIME);
        return;
    }
    if(_pCommState->timer != 0){
        _pCommState->timer--;
        return;
    }
    realParent = CommMPGetNextConnectIndex();  //つながったことがある人がいたら
    if(realParent != -1){
        _pCommState->connectIndex = realParent;  // 記憶
        _CHANGE_STATE(_underChildForceConnect, _CHILD_P_SEARCH_TIME);
        return;
    }


    // 親がいない場合 自分が仮親になる
    _CHANGE_STATE(_underChildFinalize, 0);
}


//==============================================================================
/**
 * 目標となる親機に対して接続中
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underChildForceConnect(void)
{
    CommMPParentBconCheck();  // bconの検査
    if(CommMPGetParentConnectionNum(_pCommState->connectIndex)!=0){ // 親が見つかっている場合
        if(CommChildIndexConnect(_pCommState->connectIndex)){  // 接続するまで呼び続ける
            _CHANGE_STATE(_underChildConnecting, 100);
            return;
        }
    }
    if(CommIsError()){
        _CHANGE_STATE(_underChildFinalize, 0);
    }
    else if(_pCommState->timer!=0){
        _pCommState->timer--;
    }
    else{
        _CHANGE_STATE(_underChildFinalize, 0);
    }
}



//==============================================================================
/**
 * 子機となって接続中
 * @param   none
 * @retval  none
 */
//==============================================================================
static void _underChildConnecting(void)
{
    if(CommIsConnect(CommGetCurrentID())){   // 自分自身が接続していることが確認できたら
        CommSystemReset();   // 今までの通信バッファをクリーンにする
        CommSetAloneMode(FALSE);
        CommEnableSendMoveData();
        _CHANGE_STATE(_underChildConnect, 0);
        return;
    }
    
    if(CommIsError()){
        _CHANGE_STATE(_underChildFinalize, 0);
        return;
    }
    if(_pCommState->timer!=0){
        _pCommState->timer--;
        return;
    }
    // 時間切れ
    _CHANGE_STATE(_underChildFinalize, 0);
}

//==============================================================================
/**
 * 子機終了 仮親に変わる
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underChildFinalize(void)
{
    if(!CommMPSwitchParentChild()){
        return;
    }
    _CHANGE_STATE(_underParentInit, 0);
}

//==============================================================================
/**
 * 親機終了　確立で親機になるか子機になるか決める
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underParentFinalize(void)
{
    if(!CommMPSwitchParentChild()){
        return;
    }
    _CHANGE_STATE(_underChildPInit, 0);
}

//==============================================================================
/**
 * 親機開始
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underParentInit(void)
{
    MYSTATUS* pMyStatus;
    
    if(!CommMPIsStateIdle()){  // 終了処理がきちんと終わっていることを確認
        return;
    }
    // 親機になってみる
    CommSetAloneMode(TRUE); // 
    if(CommParentModeInit(FALSE, _pCommState->bFirstParent, _PACKETSIZE_UNDERGROUND, TRUE))  {
        u32 rand = MATH_Rand32(&_pCommState->sRand, _PARENT_WAIT_TIME/2);
        _pCommState->bFirstParent = FALSE;
        _CHANGE_STATE(_underParentWait, _PARENT_WAIT_TIME/2+rand);
    }
}

//==============================================================================
/**
 * 親機になり、子機が接続してくるのを待つ
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underParentWait(void)
{
   if(CommIsChildsConnecting()){   // 自分以外がつながったら親機固定
        _pCommState->bFirstParent = TRUE;  // 親機として繋がったのでフラグを戻しておく
        _CHANGE_STATE(_underParentConnectInit, 0);
        return;
    }
    if(_pCommState->timer!=0){
        _pCommState->timer--;
        return;
    }
    if(_pCommState->bParentOnly){
        return;
    }
    if( CommMPSwitchParentChild() ){
        _CHANGE_STATE(_underParentFinalize, _FINALIZE_TIME);
    }
}

//==============================================================================
/**
 * 親機として接続完了したので初期化を行う
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underParentConnectInit(void)
{
    CommSetAloneMode(FALSE);
    CommMPSetNoChildError(TRUE);  // 子機がいなくなったら再検索するためにERR扱いにする
    CommEnableSendMoveData();

    _CHANGE_STATE(_underParentConnect, 0);
}

//==============================================================================
/**
 * 親機として接続中
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underParentConnect(void)
{
}

//==============================================================================
/**
 * 子機として接続完了
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underChildConnect(void)
{
}


static void _underSBBoot(void)
{
    //  バトルファクトリーでの動作が変わってしまう不具合の修正
    //    if(_pCommState->serviceNo < COMM_MODE_BATTLE_SINGLE_WIFI){
    if(!CommStateIsWifiConnect()){
        if(!CommMPIsStateIdle()){  // 終了処理がきちんと終わっていることを確認
            return;
        }
        CommSetAloneMode(TRUE);  // 非接続状態
        CommEnableSendMoveData(); //commsystemにおいてあるキーデータの送信を許可
        CommMPStealth(TRUE);  // 秘密裏に切断
    }
    else{
    }
    _CHANGE_STATE(_stateNone, 0);
}

//==============================================================================
/**
 * 秘密基地OFFLINE用にリセット処理を行う
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underSBReset(void)
{
    if(!CommMPSwitchParentChild()){
        return;
    }
    CommSystemReset();         // 今までの通信バッファをクリーンにする
    _CHANGE_STATE(_underSBBoot, 0);
}


//==============================================================================
/**
 * イクニューモン初期化後通信起動
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underChildOnline(void)
{
    if(!CommIsVRAMDInitialize()){
        return;  //
    }
    CommMPStealth(FALSE);

    CommSystemReset();         // 今までの通信バッファをクリーンにする

    _CHANGE_STATE(_underChildFInit, 0);
}

//==============================================================================
/**
 * リセット処理を行い、子機として再起動
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underChildReset(void)
{
    if(!CommMPSwitchParentChild()){
        return;
    }
    _CHANGE_STATE(_underChildFInit, 0);
}

//==============================================================================
/**
 * 親機として初期化を行う
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _battleParentInit(void)
{
    MYSTATUS* pMyStatus;
    
    if(!CommIsVRAMDInitialize()){
        return;
    }
    CommMPInitialize(_pCommState->pMyStatus, TRUE);
    CommInfoInitialize(_pCommState->pSaveData, _pCommState->pReg);

    if(CommParentModeInit(TRUE, TRUE, _PACKETSIZE_BATTLE,TRUE)){
        CommSetTransmissonTypeDS();
        _CHANGE_STATE(_battleParentSendName, 0);
    }
}


//==============================================================================
/**
 * 子機待機状態  親機を選択中
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _battleParentSendName(void)
{
    if(!CommIsConnect(CommGetCurrentID())){
        return;
    }
    _CHANGE_STATE(_battleParentWaiting, 0);
}

//==============================================================================
/**
 * 親機として待機中
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _battleParentWaiting(void)
{
    if(!CommIsInitialize()){
        _CHANGE_STATE(_stateEnd,0);
    }
}

//==============================================================================
/**
 * 子機の初期化
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _battleChildInit(void)
{
    if(!CommIsVRAMDInitialize()){
        return;
    }
    CommMPInitialize(_pCommState->pMyStatus, TRUE);
    CommInfoInitialize(_pCommState->pSaveData, _pCommState->pReg);
    
    if(CommChildModeInit(TRUE, TRUE, _PACKETSIZE_BATTLE)){
        CommSetTransmissonTypeDS();
        _CHANGE_STATE(_battleChildBconScanning, 0);
    }
}

//==============================================================================
/**
 * 子機待機状態  親機ビーコン収集中
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _battleChildBconScanning(void)
{
    CommMPParentBconCheck();
}

//==============================================================================
/**
 * 子機待機状態  親機に許可もらい中
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _battleChildConnecting(void)
{
    CommMPParentBconCheck();
    if(CommChildIndexConnect(_pCommState->connectIndex)){  // 接続完了
        _CHANGE_STATE(_battleChildSendName, _SEND_NAME_TIME);
    }

}

//==============================================================================
/**
 * 子機待機状態  親機に情報を送信
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _battleChildSendName(void)
{

    if(CommIsError()){
        //OHNO_PRINT("エラーの場合戻る\n");
        _CHANGE_STATE(_battleChildReset, 0);
    }

    
    if(CommIsConnect(CommGetCurrentID()) && ( COMM_PARENT_ID != CommGetCurrentID())){
        _CHANGE_STATE(_battleChildWaiting, 0);
    }
}

//==============================================================================
/**
 * 子機リセット   
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _battleChildReset(void)
{
    CommMPSwitchParentChild();
    _CHANGE_STATE(_battleChildReConnect, _FINALIZE_TIME);
}

//==============================================================================
/**
 * 子機をいったん終了し再接続
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _battleChildReConnect(void)
{
    MYSTATUS* pMyStatus;

    if(_pCommState->timer!=0){
        _pCommState->timer--;
        return;
    }
    if(!CommMPIsStateIdle()){  /// 終了処理がきちんと終わっていることを確認
        return;
    }
    if(CommChildModeInit(FALSE, TRUE, _PACKETSIZE_BATTLE)){
        CommSetTransmissonTypeDS();
        _CHANGE_STATE(_battleChildConnecting, _SEND_NAME_TIME);
    }
}


//==============================================================================
/**
 * 子機待機状態
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _battleChildWaiting(void)
{
    if(!CommIsInitialize()){
        _CHANGE_STATE(_stateEnd,0);
    }
}

//==============================================================================
/**
 * 子機をいったん終了
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _battleChildReTry(void)
{
    CommMPSwitchParentChild();
    _CHANGE_STATE(_battleChildReInit, _FINALIZE_TIME);
}

//==============================================================================
/**
 * 子機をいったん終了し再起動をかける
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _battleChildReInit(void)
{
    MYSTATUS* pMyStatus;

    if(_pCommState->timer!=0){
        _pCommState->timer--;
        return;
    }
    if(!CommMPIsStateIdle()){  /// 終了処理がきちんと終わっていることを確認
        return;
    }
    if(CommChildModeInit(FALSE, TRUE, _PACKETSIZE_BATTLE)){
        CommSetTransmissonTypeDS();
        _CHANGE_STATE(_battleChildBconScanning, _SEND_NAME_TIME);
    }
}

//==============================================================================
/**
 * 何もしないステート
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _stateNone(void)
{
    // なにもしていない
}

//==============================================================================
/**
 * @brief エラー処理
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _stateConnectError(void)
{
}

//==============================================================================
/**
 * @brief  終了処理中
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _stateEnd(void)
{
    if(CommIsInitialize()){
        return;
    }
    _stateFinalize();
}

//==============================================================================
/**
 * @brief  地下離脱処理開始
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _stateUnderGroundConnectEnd(void)
{
    _CHANGE_STATE(_stateConnectEnd, _EXIT_SENDING_TIME);
}

//==============================================================================
/**
 * @brief  自動終了処理開始
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _stateConnectAutoEnd(void)
{
    if(CommSendFixData(CS_AUTO_EXIT)){
        _CHANGE_STATE(_stateEnd, 0);
    }
}

//==============================================================================
/**
 * @brief   接続切り替えの前に親機ならば相手の切断を確認する
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _stateConnectChildEndWait(void)
{
    if(CommGetConnectNum() <= 1){  // 自分だけの接続になったら
        WHParentConnectPauseSystem(FALSE);
        CommSystemResetDS();   // 今までの通信バッファをクリーンにする
        _CHANGE_STATE(_unionChildFinalize, 0);
    }
    if(_pCommState->timer != 0){
        _pCommState->timer--;
        return;
    }
    // 強制
    WHParentConnectPauseSystem(FALSE);
    CommSystemResetDS();   // 今までの通信バッファをクリーンにする
    _CHANGE_STATE(_unionChildFinalize, 0);
}

//==============================================================================
/**
 * @brief   接続切り替えの前に子機ならば切れた時に初期化
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _stateConnectChildEnd(void)
{
    if(!CommMPSwitchParentChild()){
        return;
    }
    CommSystemReset();   // 今までの通信バッファをクリーンにする
    _CHANGE_STATE(_unionChildFinalize, 0);
}

//==============================================================================
/**
 * @brief  終了処理開始
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _stateConnectEnd(void)
{
    if(_pCommState->timer != 0){
        _pCommState->timer--;
    }
    if(!CommMPSwitchParentChild()){
        return;
    }
    if(_pCommState->timer != 0){
        return;
    }
    CommFinalize();
    _CHANGE_STATE(_stateEnd, 0);
}



//==============================================================================
/**
 * UNIONスタート
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _unionStart(void)
{
    void* pWork;

    if(!CommIsVRAMDInitialize()){
        return;  //
    }
    CommMPInitialize(_pCommState->pMyStatus, TRUE);
    WH_SetMaxEntry(_pCommState->limitNum);
    CommInfoInitialize(_pCommState->pSaveData, NULL);
    
    // まず子機になってみて、親機を探す   ぐるぐる回してbconをためる
    if(CommChildModeInit(TRUE, TRUE, _PACKETSIZE_UNION)){
        CommSetTransmissonTypeDS();
        _CHANGE_STATE(_unionChildSearching, _CHILD_P_SEARCH_TIME*2);
    }
}

//==============================================================================
/**
 * 子機となって親機を探し中
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _unionChildSearching(void)
{
    int realParent;

    CommMPParentBconCheck();  // bconの検査
    if(_pCommState->timer != 0){
        _pCommState->timer--;
        return;
    }

    if(!CommMPSwitchParentChild()){
        return;
    }
    _CHANGE_STATE(_unionParentInit, 0);
}

//==============================================================================
/**
 * 子機終了 仮親に変わる
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _unionChildFinalize(void)
{
    if(!CommMPSwitchParentChild()){
        return;
    }
    _CHANGE_STATE(_unionParentInit, 0);
}

//==============================================================================
/**
 * 親機開始
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _unionParentInit(void)
{
    if(!CommMPIsStateIdle()){  // 終了処理がきちんと終わっていることを確認
        return;
    }
    // 親機になってみる
    if(CommParentModeInit(FALSE, _pCommState->bFirstParent, _PACKETSIZE_UNION,TRUE))  {
        u32 rand = MATH_Rand32(&_pCommState->sRand, _PARENT_WAIT_TIME*2);
        CommSetTransmissonTypeDS();
        _pCommState->bFirstParent = FALSE;
        _CHANGE_STATE(_unionParentWait, 10000);
    }
}

//==============================================================================
/**
 * 親機になりbcon放出
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _unionParentWait(void)
{
    if(CommMPIsParentBeaconSent()){  // ビーコンを送り終わったら子機に切り替わる
    }
    else{
        if(CommIsChildsConnecting()){   // 自分以外がつながったら親機固定
            _pCommState->bFirstParent = TRUE;  // 親機として繋がったのでフラグを戻しておく
            WirelessIconEasy();
            _CHANGE_STATE(_unionParentConnect, 0);
            return;
        }
        if(_pCommState->timer!=0){
            _pCommState->timer--;
            return;
        }
    }
    if( CommMPSwitchParentChild() ){
        _CHANGE_STATE(_unionChildRestart, 0);
    }
}

//==============================================================================
/**
 * 子機再スタート
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _unionChildRestart(void)
{
    u32 rand;
    
    if(!CommMPIsStateIdle()){  // 終了処理がきちんと終わっていることを確認
        return;
    }
    // 今度はビーコンを残したまま
    if(CommChildModeInit(FALSE, FALSE, _PACKETSIZE_UNION)){
        CommSetTransmissonTypeDS();
        rand = MATH_Rand32(&_pCommState->sRand, _CHILD_P_SEARCH_TIME);
        _CHANGE_STATE(_unionChildSearching, rand);
    }
}

//==============================================================================
/**
 * 話しかけ開始で、子機待機状態になる
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _unionForceConnectStart(void)
{
    if(!CommMPSwitchParentChild()){  // 今の状態を終了
        return;
    }
    _CHANGE_STATE(_unionForceConnectStart2, 0);
}


static void _unionForceConnectStart2(void)
{
    if(!CommMPIsStateIdle()){  // 終了処理がきちんと終わっていることを確認
        return;
    }
    // 子機になる
    if(CommChildModeInit(FALSE, FALSE, _PACKETSIZE_UNION)){
        CommSetTransmissonTypeDS();
        _CHANGE_STATE(_unionForceConnect, 100);
    }
}

//==============================================================================
/**
 * 子機となって強制接続
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _unionForceConnect(void)
{
    if(CommMPGetParentConnectionNum(_pCommState->connectIndex)!=0){ // 親が見つかっている場合
        if(CommChildIndexConnect(_pCommState->connectIndex)){  // 接続するまで呼び続ける
            _CHANGE_STATE(_unionChildConnecting, 100);
            return;
        }
    }
    if(CommIsError()){
        _CHANGE_STATE(_unionChildReset, 0);
    }
    else if(_pCommState->timer!=0){
        _pCommState->timer--;
    }
    else{
        _CHANGE_STATE(_unionChildReset, 0);
    }
}

//==============================================================================
/**
 * 子機となって接続中
 * @param   none
 * @retval  none
 */
//==============================================================================
#if AFTER_MASTER_070420_GF_COMM_FIX
#define _NEGOTIATION_TIME (120)
#endif
static void _unionChildConnecting(void)
{
    if(CommIsError()){
        _CHANGE_STATE(_unionChildReset, 0);
        return;
    }
    if(CommIsConnect(CommGetCurrentID())){   // 自分自身が接続していることが確認できたら
        _pCommState->negotiation = _NEGOTIATION_CHECK;
#if AFTER_MASTER_070420_GF_COMM_FIX
        _CHANGE_STATE(_unionChildNegotiation, _NEGOTIATION_TIME);
#else
        CommSendFixSizeData(CS_COMM_NEGOTIATION, _negotiationMsg);
        _CHANGE_STATE(_unionChildNegotiation, 120);
#endif
        return;
    }
    
    if(_pCommState->timer!=0){
        _pCommState->timer--;
        return;
    }
    // 時間切れ
    _CHANGE_STATE(_unionChildReset, 0);
}

//==============================================================================
/**
 * 子機となって接続中  親機認証待ち
 * @param   none
 * @retval  none
 */
//==============================================================================
static void _unionChildNegotiation(void)
{
    if(CommIsError()){
        _CHANGE_STATE(_unionChildReset, 0);
        return;
    }
    if(_pCommState->negotiation == _NEGOTIATION_NG){
        _CHANGE_STATE(_unionChildConnectFailed, 0);
        return;
    }
    if(_pCommState->negotiation == _NEGOTIATION_OK){
        CommInfoSendPokeData();
        _CHANGE_STATE(_unionChildConnectSuccess, 0);
        return;
    }
#if AFTER_MASTER_070420_GF_COMM_FIX
    if(_pCommState->timer > (_NEGOTIATION_TIME-10)){
        CommSendFixSizeData(CS_COMM_NEGOTIATION, _negotiationMsg);
    }
#endif
    
    if(_pCommState->timer!=0){
        _pCommState->timer--;
        return;
    }
    _CHANGE_STATE(_unionChildReset, 0);
}



static void _unionChildConnectSuccess(void)
{
    if(CommIsError()){
        _CHANGE_STATE(_unionChildReset, 0);
        return;
    }
}

static void _unionChildConnectFailed(void)
{
}

static void _unionChildReset(void)
{
    _pCommState->negotiation = _NEGOTIATION_CHECK;
    if(!CommMPSwitchParentChild()){
        return;
    }

    if(_pCommState->reConnectTime != 0){
        _pCommState->reConnectTime--;
        _CHANGE_STATE(_unionForceConnectStart2, 0);
    }
    else{  // つながらなかったので失敗ステートへ
        _CHANGE_STATE(_unionChildConnectFailed, 0);
    }
}

//==============================================================================
/**
 * 親として接続中
 * @param   none
 * @retval  none
 */
//==============================================================================
static void _unionParentConnect(void)
{
    if(!CommIsChildsConnecting()){   // 自分以外がつながってないばあいもう一回
        if(!CommStateGetErrorCheck()){
            if( CommMPSwitchParentChild() ){
                _CHANGE_STATE(_unionChildRestart, 0);
            }
        }
    }
    else{
        if(CommGetCurrentID() == COMM_PARENT_ID){
            CommInfoSendArray_ServerSide();  // 子機から問い合わせがあったらinfoを送信
        }
    }
    if(CommIsError()){
        if(!CommStateGetErrorCheck()){
            _CHANGE_STATE(_unionChildReset, 0);
            return;
        }
    }
}

//==============================================================================
/**
 * 親機のまま一時停止
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _unionParentPause(void)
{
    u32 rand;
    
    if(!CommMPIsStateIdle()){  // 終了処理がきちんと終わっていることを確認
        return;
    }
    // つながらない親機になる
    if(CommParentModeInit(FALSE, _pCommState->bFirstParent, _PACKETSIZE_UNION, FALSE))  {
        CommSetTransmissonTypeDS();
        _CHANGE_STATE(_stateNone,0);
    }
}




static void _pictureBoardChangeParent(void);
static void _pictureBoardInitParent(void);
static void _pictureBoardParentWait(void);

static void _pictureBoardChangeChild(void);
static void _pictureBoardInitChild(void);
static void _pictureBoardForceConnect(void);
static void _pictureBoardChildConnecting(void);

//==============================================================================
/**
 * お絵かきモードに繋ぎなおす
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommStateUnionPictureBoardChange(void)
{
    _pCommState->serviceNo = COMM_MODE_PICTURE;
    if(CommGetCurrentID() == COMM_PARENT_ID){
        u8 bDSMode = FALSE;
        CommSendFixSizeData(CS_DSMP_CHANGE, &bDSMode);  //移動モードに変更する
//        _pCommState->limitNum = COMM_MODE_PICTURE_NUM_MAX+1;
    }
    else{
        u8 bDSMode = FALSE;
        CommSendFixSizeData(CS_DSMP_CHANGE, &bDSMode);  //移動モードに変更する
    }
}

//==============================================================================
/**
 * お絵かきモード子機として繋ぐ
 * @param   接続する親機index
 * @retval  none
 */
//==============================================================================

void CommStateUnionPictureBoardChild(int index)
{
    _pCommState->connectIndex = index;
    _pCommState->reConnectTime = _RETRY_COUNT_UNION;
    _CHANGE_STATE(_pictureBoardChangeChild, 0);
}

//==============================================================================
/**
 * レコードコーナーの状態に切り替える
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommStateUnionRecordCornerChange(void)
{
    _pCommState->serviceNo = COMM_MODE_RECORD;
}

//==============================================================================
/**
 * レコードコーナー子機として繋ぐ
 * @param   接続する親機index
 * @retval  none
 */
//==============================================================================

void CommStateUnionRecordCornerChild(int index)
{
    _pCommState->connectIndex = index;
    _pCommState->serviceNo = COMM_MODE_RECORD;
    _pCommState->reConnectTime = _RETRY_COUNT_UNION;
    WirelessIconEasy();
    _CHANGE_STATE(_unionForceConnectStart, 0);
}

//==============================================================================
/**
 * ぐるぐる交換の状態に切り替える
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommStateUnionGuru2Change(void)
{
    _pCommState->serviceNo = COMM_MODE_GURU2;
}

//==============================================================================
/**
 * ぐるぐる交換子機として繋ぐ
 * @param   接続する親機index
 * @retval  none
 */
//==============================================================================

void CommStateUnionGuru2Child(int index)
{
    _pCommState->connectIndex = index;
    _pCommState->serviceNo = COMM_MODE_GURU2;
    _pCommState->reConnectTime = _RETRY_COUNT_UNION;
    WirelessIconEasy();
    _CHANGE_STATE(_unionForceConnectStart, 0);
}

//==============================================================================
/**
 * お絵かきstateに移行したかどうかの確認
 * @param   none
 * @retval  お絵かきstateに移行した場合TRUE
 */
//==============================================================================

BOOL CommStateIsUnionPictureBoardState(void)
{
    u32 stateAddr = (u32)_pCommState->state;

    if(CommIsTransmissonDSType()){
        return FALSE;
    }
    if(stateAddr == (u32)_unionParentConnect){
        return TRUE;
    }
    if(stateAddr == (u32)_unionChildConnectSuccess){
        return TRUE;
    }
    return FALSE;
}


static void _pictureBoardChangeParent(void)
{
    if( CommMPSwitchParentChild() ){
        _CHANGE_STATE(_pictureBoardInitParent, 0);
    }
}

static void _pictureBoardInitParent(void)
{
    if(!CommMPIsStateIdle()){  // 終了処理がきちんと終わっていることを確認
        return;
    }
    _pCommState->serviceNo = COMM_MODE_PICTURE;
    if(CommParentModeInit(FALSE, FALSE, _PACKETSIZE_UNION,TRUE)) {
        CommSetTransmissonTypeMP();
        _CHANGE_STATE(_pictureBoardParentWait, 0);
    }
}


static void _pictureBoardParentWait(void)
{
    if(CommIsChildsConnecting()){   // 自分以外がつながったら親機固定
        _pCommState->bFirstParent = TRUE;  // 親機として繋がったのでフラグを戻しておく
        CommInfoSendPokeData();
        _CHANGE_STATE(_unionParentConnect, 0);
    }
}


static void _pictureBoardChangeChild(void)
{
    if( CommMPSwitchParentChild() ){
        _CHANGE_STATE(_pictureBoardInitChild, 0);
    }
}

static void _pictureBoardInitChild(void)
{
    if(!CommMPIsStateIdle()){  // 終了処理がきちんと終わっていることを確認
        return;
    }
    _pCommState->serviceNo = COMM_MODE_PICTURE;
    if(CommChildModeInit(FALSE, FALSE, _PACKETSIZE_UNION)){
        CommSetTransmissonTypeMP();
        _CHANGE_STATE(_pictureBoardForceConnect, 100);
    }
}


//==============================================================================
/**
 * 子機となって強制接続
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _pictureBoardForceConnect(void)
{
    if(CommMPGetParentConnectionNum(_pCommState->connectIndex)!=0){ // 親が見つかっている場合
        if(CommChildIndexConnect(_pCommState->connectIndex)){  // 接続するまで呼び続ける
            _CHANGE_STATE(_pictureBoardChildConnecting, 100);
            return;
        }
    }
    if(CommIsError() || (_pCommState->timer == 0)){
        _pCommState->reConnectTime--;
        if(_pCommState->reConnectTime==0){
            _CHANGE_STATE(_unionChildConnectFailed, 0);
        }
        else{
            _CHANGE_STATE(_pictureBoardChangeChild, 0);
        }
    }
    else if(_pCommState->timer!=0){
        _pCommState->timer--;
    }
}

//==============================================================================
/**
 * 子機となって接続中
 * @param   none
 * @retval  none
 */
//==============================================================================
static void _pictureBoardChildConnecting(void)
{
    if(_pCommState->timer > 90){
        _pCommState->timer--;
        return;
    }
    
    if(CommIsError()){
        _pCommState->reConnectTime--;
        if(_pCommState->reConnectTime==0){
            _CHANGE_STATE(_unionChildConnectFailed, 0);
        }
        else{
            _CHANGE_STATE(_pictureBoardChangeChild, 0);
        }
        return;
    }
    
    if(CommIsConnect(CommGetCurrentID())){   // 自分自身が接続していることが確認できたら
        _CHANGE_STATE(_unionChildConnectSuccess, 0);
        return;
    }
    if(_pCommState->timer!=0){
        _pCommState->timer--;
        return;
    }

    _pCommState->reConnectTime--;
    if(_pCommState->reConnectTime==0){
        _CHANGE_STATE(_unionChildConnectFailed, 0);
    }
    else{
        // 時間切れ
        _CHANGE_STATE(_pictureBoardChangeChild, 0);
    }
}

//==============================================================================
/**
 * ネゴシエーション用コールバック CS_COMM_NEGOTIATION
 * @param   callback用引数
 * @retval  none
 */
//==============================================================================

void CommRecvNegotiation(int netID, int size, void* pData, void* pWork)
{
    int i;
    u8* pMsg = pData;
    BOOL bMatch = TRUE;

    if(CommGetCurrentID() != COMM_PARENT_ID){  // 親機のみ判断可能
        return;
    }
    bMatch = TRUE;
    for(i = 0; i < sizeof(_negotiationMsg); i++){
        if(pMsg[i] != _negotiationMsg[i]){
            bMatch = FALSE;
            break;
        }
    }
    if(bMatch  && (!_pCommState->bUnionPause)){   // 子機から接続確認が来た
//        if(CommGetConnectNum() <= _pCommState->limitNum){  // 指定接続人数より下回ること
            _negotiationMsgReturnOK[0] = netID;
            CommSendFixSizeData_ServerSide(CS_COMM_NEGOTIATION_RETURN, _negotiationMsgReturnOK);
            return;
//        }
    }
    _negotiationMsgReturnNG[0] = netID;
    CommSendFixSizeData_ServerSide(CS_COMM_NEGOTIATION_RETURN, _negotiationMsgReturnNG);
}

//==============================================================================
/**
 * ネゴシエーション用コールバック CS_COMM_NEGOTIATION_RETURN
 * @param   callback用引数
 * @retval  none
 */
//==============================================================================

void CommRecvNegotiationReturn(int netID, int size, void* pData, void* pWork)
{
    u8 id;
    int i;
    u8* pMsg = pData;
    BOOL bMatch = TRUE;

    for(i = 1; i < sizeof(_negotiationMsgReturnOK); i++){
        if(pMsg[i] != _negotiationMsgReturnOK[i]){
            bMatch = FALSE;
            break;
        }
    }
    if(bMatch){   // 親機から接続認証が来た
        id = pMsg[0];
        if(id == CommGetCurrentID()){
            _pCommState->negotiation = _NEGOTIATION_OK;
        }
        return;
    }
    bMatch = TRUE;
    for(i = 1; i < sizeof(_negotiationMsgReturnNG); i++){
        if(pMsg[i] != _negotiationMsgReturnNG[i]){
            bMatch = FALSE;
            break;
        }
    }
    if(bMatch){   // 親機から接続否定が来た
        id = pMsg[0];
        if(id == (u8)CommGetCurrentID()){
            _pCommState->negotiation = _NEGOTIATION_NG;
        }
        return;
    }
}

//==============================================================================
/**
 * ネゴシエーション用コールバック
 * @param   callback用引数
 * @retval  none
 */
//==============================================================================

int CommRecvGetNegotiationSize(void)
{
    return sizeof(_negotiationMsg);
}


//==============================================================================
/**
 * 入ってくる人数を制限する
 * @param   人数
 * @retval  none
 */
//==============================================================================

void CommStateSetLimitNum(int num)
{
    if(_pCommState){
        _pCommState->limitNum = num;
        WH_SetMaxEntry(num);
    }
}


//==============================================================================
/**
 * 子機待機状態  親機ビーコン収集中
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _poketchBconScanning(void)
{
    CommMPParentBconCheck();
}

//==============================================================================
/**
 * ポケッチ子機の初期化
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _poketchChildInit(void)
{
    if(!CommIsVRAMDInitialize()){
        return;
    }
    CommMPInitialize(_pCommState->pMyStatus, TRUE);
    CommInfoInitialize(_pCommState->pSaveData, NULL);
    
    if(CommChildModeInit(TRUE, TRUE, _PACKETSIZE_POKETCH)){
        CommSetTransmissonTypeDS();
        _CHANGE_STATE(_poketchBconScanning, 0);
    }
}

//==============================================================================
/**
 * ぽけっちとしての通信処理開始（子機状態のみ）
 * @param   serviceNo  通信サービス番号
 * @retval  none
 */
//==============================================================================

void CommStateEnterPockchChild(SAVEDATA* pSaveData)
{
    if(CommIsInitialize()){
        return;      // つながっている場合今は除外する
    }
    // 通信ヒープ作成
    sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_COMMUNICATION, _HEAPSIZE_POKETCH );
    _commStateInitialize(pSaveData,COMM_MODE_POKETCH);
//    _pCommState->serviceNo = COMM_MODE_POKETCH;
    _pCommState->regulationNo = 0;
#ifdef PM_DEBUG
    _pCommState->soloDebugNo = 0;
#endif
    _CHANGE_STATE(_poketchChildInit, 0);
}

//==============================================================================
/**
 * ぽけっちとしての終了処理
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommStateExitPoketch(void)
{
    if(_pCommState==NULL){  // すでに終了している
        return;
    }
    // 切断ステートに移行する  すぐに切れない
    _CHANGE_STATE(_stateConnectEnd, _EXIT_SENDING_TIME);
}

//==============================================================================
/**
 * 通信サーチャーにおいて情報収集STATEなのかどうか返す
 * @param   none
 * @retval  収集中ならTRUE
 */
//==============================================================================

BOOL CommIsPoketchSearchingState(void)
{
    int i;
    u32 funcTbl[]={
        (u32)_poketchBconScanning,
        0,
    };
    u32 stateAddr = (u32)_pCommState->state;

    if(_pCommState==NULL){  // すでに終了している
        return FALSE;
    }
    for(i = 0; funcTbl[i] != 0; i++ ){
        if(stateAddr == funcTbl[i]){
            return TRUE;
        }
    }
    return FALSE;
}


//==============================================================================
/**
 * PARTYサーチcallback
 * @param   ggid
 * @retval  none
 */
//==============================================================================

static void _callbackGGIDScan(u32 ggid,int serviceNo)
{
    switch(ggid){
      case _MYSTERY_GGID:
        if(serviceNo == COMM_MODE_MYSTERY){
            _pCommState->partyGameBit |= PARTYGAME_MYSTERY_BCON;
        }
        break;
      case _BCON_DOWNLOAD_GGID:
        _pCommState->partyGameBit |= PARTYGAME_MYSTERY_BCON;
        break;
      case _RANGER_GGID:
        _pCommState->partyGameBit |= PARTYGAME_RANGER_BCON;
        break;
      case _WII_GGID:
        _pCommState->partyGameBit |= PARTYGAME_WII_BCON;
        break;
    }
}

//==============================================================================
/**
 * PARTYサーチ子機の初期化
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _partySearchChildMain(void)
{
    if(CommChildModeInit(TRUE, TRUE, _PACKETSIZE_PARTY)){
        CommSetTransmissonTypeDS();
        _CHANGE_STATE(_poketchBconScanning, 0);
    }
}

//==============================================================================
/**
 * PARTYサーチ子機の初期化
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _partySearchChildInit(void)
{
    if(!CommIsVRAMDInitialize()){
        return;
    }
    CommMPInitialize(_pCommState->pMyStatus, FALSE);
    CommInfoInitialize(_pCommState->pSaveData, NULL);
    WHSetGGIDScanCallback(_callbackGGIDScan);
    
    _CHANGE_STATE(_partySearchChildMain, 0);
}

//==============================================================================
/**
 * パーティーゲーム検索の通信処理開始（子機状態のみ）
 * @param   SAVEDATA  savedata
 * @retval  none
 */
//==============================================================================

void CommStateEnterPartyGameScanChild(SAVEDATA* pSaveData)
{
    if(CommIsInitialize()){
        return;      // つながっている場合今は除外する
    }
    // 通信ヒープ作成
    sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_COMMUNICATION, _HEAPSIZE_PARTY );
    _commStateInitialize(pSaveData,COMM_MODE_PARTY);
    _pCommState->regulationNo = 0;
#ifdef PM_DEBUG
    _pCommState->soloDebugNo = 0;
#endif
    _CHANGE_STATE(_partySearchChildInit, 0);
}

//==============================================================================
/**
 * パーティーゲームサーチの終了処理
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommStateExitPartyGameScan(void)
{
    if(_pCommState==NULL){  // すでに終了している
        return;
    }
    // 切断ステートに移行する  すぐに切れない
    _CHANGE_STATE(_stateConnectEnd, _EXIT_SENDING_TIME);
}

//==============================================================================
/**
 * 拾ったビーコンのBITを返す
 * @param   none
 * @retval  none
 */
//==============================================================================

u8 CommStateGetPartyGameBit(void)
{
  return _pCommState->partyGameBit;
}

//==============================================================================
/**
 * リセット終了処理
 * @param   none
 * @retval  none
 */
//==============================================================================

BOOL CommStateExitReset(void)
{
    CommSystemShutdown();
    if(_pCommState==NULL){  // すでに終了している
        return TRUE;
    }
    if( (_pCommState->serviceNo == COMM_MODE_DPW_WIFI) || 
		(_pCommState->serviceNo == COMM_MODE_FUSIGI_WIFI) ||
		(_pCommState->serviceNo == COMM_MODE_EMAIL_WIFI) ){
        mydwc_Logout();
        return TRUE;
    }
    else if(CommStateIsWifiConnect()){
        OHNO_SP_PRINT(" ログアウト処理\n");
		if( _pCommState->serviceNo == COMM_MODE_LOBBY_WIFI ){
			_CHANGE_STATE( _wifiLobbyLogout, 0 );
		}else{
	        _CHANGE_STATE(_stateWifiLogout,0);  // ログアウト処理
		}
    }
    else{
        _CHANGE_STATE(_underSBReset, 0);  // エラー終了の場合RESETする
    }
    return FALSE;
}

//==============================================================================
/**
 * 不思議通信親機として初期化を行う
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _mysteryParentInit(void)
{
    MYSTATUS* pMyStatus;
    
    if(!CommIsVRAMDInitialize()){
        return;
    }
    CommMPInitialize(_pCommState->pMyStatus, TRUE);
    CommInfoInitialize(_pCommState->pSaveData, NULL);

    if(CommParentModeInit(TRUE, TRUE, _PACKETSIZE_BATTLE,TRUE)){
        CommSetTransmissonTypeDS();
        _CHANGE_STATE(_battleParentSendName, 0);
    }
}

//==============================================================================
/**
 * 不思議通信子機の初期化
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _mysteryChildInit(void)
{
    if(!CommIsVRAMDInitialize()){
        return;
    }
    CommMPInitialize(_pCommState->pMyStatus, TRUE);
    CommInfoInitialize(_pCommState->pSaveData, NULL);
    
    if(CommChildModeInit(TRUE, TRUE, _PACKETSIZE_BATTLE)){
        CommSetTransmissonTypeDS();
        _CHANGE_STATE(_battleChildBconScanning, 0);
    }
}




//---------------------wifi


//==============================================================================
/**
 * エラーが発生した状態
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _wifiBattleError(void){}

static void _wifiBattleTimeout(void)
{
   int ret;
    ret = mydwc_stepmatch(FALSE);

	if( ret < 0 ){
        // エラー発生。
        _CHANGE_STATE(_wifiBattleError, 0);

	// タイムアウトエラーが帰ってきてるときは
	// 自分でWiFiエラーをチェックする
    }else if( ret == STEPMATCH_TIMEOUT ){ 
#if 0   // この処理はdwc_rap内に反映させました   k.ohno 2008.05.23
		// タイムアウトに遷移している場合、
		// 通常のWiFiエラーチェックを行っていない可能性のほうが高い、
		// その為、ここでチェックする
		ret = mydwc_HandleError();
		if( ret < 0 ){
			// エラー発生。
			_CHANGE_STATE(_wifiBattleError, 0);
		}
#endif
	}
}

static void _wifiBattleDisconnect(void){}

static void _wifiBattleFailed(void){}

static int _wifiLinkLevel(void)
{
    return WM_LINK_LEVEL_3 - DWC_GetLinkLevel();
}

//==============================================================================
/**
 * 接続中
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _wifiBattleConnect(void)
{
    int ret,errCode;

    CommSetWifiConnect(TRUE);

    ret = mydwc_stepmatch(FALSE);

    if((ret >= DWC_ERROR_FRIENDS_SHORTAGE) && (STEPMATCH_SUCCESS > ret)){
        _CHANGE_STATE(_wifiBattleFailed, 0);
    }
    else if( ret < 0 ){
        // エラー発生。
        // バトル通信不具合のテストのためASSERT追加    81010 91010はそのまま処理しますので ASSERTをはずします 08/06/10
//        GF_ASSERT_MSG(0,"err%d %d %d",_pCommState->aError.errorCode,_pCommState->aError.errorType,_pCommState->aError.errorRet);
        _CHANGE_STATE(_wifiBattleError, 0);
    }		
    else if( ret == STEPMATCH_TIMEOUT ) {     // タイムアウト（相手から１０秒以上メッセージが届かない）
//        GF_ASSERT_MSG(0,"timeout");
        _CHANGE_STATE(_wifiBattleTimeout, 0);
    }
    else if(ret == STEPMATCH_DISCONNECT){
        if(_pCommState->bDisconnectError){
            _CHANGE_STATE(_wifiBattleError, 0);
        }
        else{
            _CHANGE_STATE(_wifiBattleDisconnect, 0);
        }
    }
    else if(ret == STEPMATCH_CANCEL){
        if(_pCommState->bDisconnectError){
            _CHANGE_STATE(_wifiBattleError, 0);
        }
        else{
            _CHANGE_STATE(_wifiBattleDisconnect, 0);
        }
	}


    if(_pCommState->bDisconnectError){
        if(_pCommState->nowConnectNum != CommGetConnectNum()){
            _CHANGE_STATE(_wifiBattleError, 0);
        }
    }
}

//==============================================================================
/**
 * エラーコードを保持する
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommStateSetWifiError(int code, int type, int ret)
{
    // エラー発生。	
    int errorcode;
    if(_pCommState){
        if( (code == ERRORCODE_HEAP) || (code == ERRORCODE_0)){
            errorcode = code;
        }
        else {
            errorcode = -code;
        }
        _pCommState->aError.errorCode = errorcode;
        _pCommState->aError.errorType = type;
        _pCommState->aError.errorRet = ret;
    }
}


//==============================================================================
/**
 * 対戦相手を探し中
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _wifiBattleMaching(void)
{
    int ret = mydwc_stepmatch( 0 );

    if((ret >= DWC_ERROR_FRIENDS_SHORTAGE) && (STEPMATCH_SUCCESS > ret)){
        _CHANGE_STATE(_wifiBattleFailed, 0);
    }
    else if( ret < 0 ){
        //_errcodeConvert(ret);
        _CHANGE_STATE(_wifiBattleError, 0);
    }
    else if ( ret == STEPMATCH_SUCCESS )  {
        OS_TPrintf("対戦相手が見つかりました。\n");
        _CHANGE_STATE(_wifiBattleConnect, 0);
    }
    else if ( ret == STEPMATCH_CANCEL ){
        OS_TPrintf("キャンセルしました。\n");	
        _CHANGE_STATE(_wifiBattleDisconnect, 0);
    }
    else if( ret == STEPMATCH_FAIL){
        _CHANGE_STATE(_wifiBattleFailed, 0);
    }
    else if( ret == STEPMATCH_DISCONNECT){
//        _CHANGE_STATE(_wifiBattleError, 0);
        _CHANGE_STATE(_wifiBattleDisconnect, 0);
    }
}



//==============================================================================
/**
 * キャンセル中  親をやめる時や切り替える時に使用
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _wifiBattleCanceling(void)
{

    int ret = mydwc_stepmatch( 1 );  // キャンセル中
				
    if( ret < 0 ){
        // エラー発生。
        //_errcodeConvert(ret);
        _CHANGE_STATE(_wifiBattleError, 0);
    }
    else{
        int ret = mydwc_startgame( _pCommState->wifiTargetNo,CommLocalGetServiceMaxEntry( _pCommState->serviceNo ) + 1 ,FALSE);
        switch(ret){
          case DWCRAP_STARTGAME_OK:    //最初はVCT待ち状態になる
            CommSystemReset();   // 今までの通信バッファをクリーンにする
            if( _pCommState->wifiTargetNo < 0 ){
                OS_TPrintf("ゲーム参加者を募集します。\n");
            } else {
                OS_TPrintf(" %d番目の友達に接続します。\n", _pCommState->wifiTargetNo);	        
            }
            _CHANGE_STATE(_wifiBattleMaching, 0);
            break;
          case DWCRAP_STARTGAME_NOTSTATE:
          case DWCRAP_STARTGAME_RETRY:
            break;
          case DWCRAP_STARTGAME_FAILED:
            _CHANGE_STATE(_wifiBattleError, 0);
            break;
        }
    }
}


//==============================================================================
/**
 * ゲームを開始する。// 2006.4.13 吉原追加
 * @param   target: 負なら親、０以上ならつなぎにいく親機の番号
 * @retval  1. 成功　　0. 失敗
 */
//==============================================================================
int CommWifiBattleStart( int target )
{
    if( _pCommState->state != _wifiBattleMaching ) return 0;

    mydwc_setReceiver( CommRecvParentCallback, CommRecvCallback );

    _pCommState->wifiTargetNo = target;
    _CHANGE_STATE(_wifiBattleCanceling,0);  // 今の状態を破棄
    return 1;
}

//==============================================================================
/**
 * ４人用ゲームを開始する
 * @param   target: 負なら親、０以上ならつなぎにいく親機の番号
 * @retval  1. 成功　　0. 失敗
 */
//==============================================================================
int CommWifiPofinStart( int target )
{
    if( _pCommState->state != _wifiBattleMaching ) return 0;

	_pCommState->serviceNo = COMM_MODE_WIFI_POFIN;
    mydwc_setReceiver( CommRecvOtherCallback, CommRecvOtherCallback );
    _pCommState->wifiTargetNo = target;
    _CHANGE_STATE(_wifiBattleCanceling,0);  // 今の状態を破棄
    return 1;
}


//==============================================================================
/**
 * マッチングが完了したかどうかを判定
 * @param   none
 * @retval  1. 完了　　0. 接続中   2. エラーやキャンセルで中断
 */
//==============================================================================
int CommWifiIsMatched(void)
{
	if( _pCommState->state == _wifiBattleMaching ) return 0;
	if( _pCommState->state == _wifiBattleConnect ) return 1;
    if(  _pCommState->state == _wifiBattleTimeout ) return 3;
    if(  _pCommState->state == _wifiBattleDisconnect ) return 4;
    if(  _pCommState->state == _wifiBattleFailed ) return 5;

	// ロビーのとき
	if( _pCommState->state == _wifiLobbyP2PMatchWait ) return 0;
	if( _pCommState->state == _wifiLobbyP2PMatch ) return 1;
    if(  _pCommState->state == _wifiLobbyTimeout ) return 3;
	
//	if( _pCommState->state == _wifiBattleError ) return 5;
    return 2;
}

//==============================================================================
/**
 * 親機として待機中
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _wifiBattleLogin(void)
{
    int ret;
	int err;

    mydwc_setReceiver( CommRecvParentCallback, CommRecvCallback );

    ret = mydwc_startgame( -1,4,TRUE );
    switch(ret){
      case DWCRAP_STARTGAME_OK:    //最初はVCT待ち状態になる
        _pCommState->bWifiDisconnect = FALSE;
        CommSystemReset();   // 今までの通信バッファをクリーンにする
        OS_TPrintf("VCT参加者を募集します。\n");
        _CHANGE_STATE(_wifiBattleMaching, 0);
        break;
      case DWCRAP_STARTGAME_NOTSTATE:
      case DWCRAP_STARTGAME_RETRY:
#ifdef PM_DEBUG
		{
			int dwc_state =DWC_GetState(); 
			OHNO_PRINT( "DWC_GetState == %d\n", dwc_state );
		}
#endif
        break;
      case DWCRAP_STARTGAME_FAILED:
        _CHANGE_STATE(_wifiBattleError, 0);
        break;
      case DWCRAP_STARTGAME_FIRSTSAVE:
        return;
    }


	// エラーチェックを行う
	err = mydwc_HandleError();
	if( err < 0 ){
        _CHANGE_STATE(_wifiBattleError, 0);
	}else if( err == ERRORCODE_HEAP ){
        _CHANGE_STATE(_wifiBattleError, 0);
	}
}

//==============================================================================
/**
 * マッチングを終了する
 * @param   none
 * @retval  none
 */
//==============================================================================


static void _stateWifiMatchEnd(void)
{
    int ret;

    CommSetWifiConnect(FALSE);
    if(mydwc_disconnect( _pCommState->disconnectIndex )){
        if(mydwc_returnLobby()){
            CommInfoFinalize();
//            CommStateSetErrorCheck(FALSE,FALSE);
//            CommStateSetErrorCheck(FALSE,TRUE);   ///335//ここでやってしまうとWiFiクラブのボイスチャットの終了時にもオートエラーTRUEにしてしまうので、コメントアウト
            _CHANGE_STATE(_wifiBattleLogin, 0);
            return;
        }
    }
    ret = mydwc_stepmatch(FALSE);
    if( ret < 0 ){
        _CHANGE_STATE(_wifiBattleError, 0);
    }
}

//==============================================================================
/**
 * WIFI終了コマンド   子機が親機にやめるように送信 ぶっつりきる CS_WIFI_EXIT
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommRecvExit(int netID, int size, void* pData, void* pWork)
{
    if(CommGetCurrentID() == COMM_PARENT_ID){
        _pCommState->disconnectIndex = 0;
        _CHANGE_STATE(_stateWifiMatchEnd, 0);
    }
    else{
        _pCommState->disconnectIndex = 1;
        _CHANGE_STATE(_stateWifiMatchEnd, 0);
    }
    _pCommState->bWifiDisconnect = TRUE;
}

//==============================================================================
/**
 * Wifi切断コマンドを受け取った場合TRUE
 * @param   none
 * @retval  WifiLoginに移行した場合TRUE
 */
//==============================================================================

BOOL CommStateIsWifiDisconnect(void)
{
    return _pCommState->bWifiDisconnect;
}

//==============================================================================
/**
 * WifiLoginに移行したかどうかの確認
 * @param   none
 * @retval  WifiLoginに移行した場合TRUE
 */
//==============================================================================

BOOL CommStateIsWifiLoginState(void)
{
    u32 stateAddr = (u32)_pCommState->state;

    if(stateAddr == (u32)_wifiBattleLogin){
        return TRUE;
    }
    if(stateAddr == (u32)_wifiLobbyConnect){	// ロビーのとき
        return TRUE;
    }
    return FALSE;
}


BOOL CommStateIsWifiLoginMatchState(void)
{
    u32 stateAddr = (u32)_pCommState->state;

    if(stateAddr == (u32)_wifiBattleMaching){
        return TRUE;
    }
    if(stateAddr == (u32)_wifiBattleLogin){
        return TRUE;
    }
    if(stateAddr == (u32)_wifiLobbyConnect){	// ロビーのとき
        return TRUE;
    }
    return FALSE;
}

//==============================================================================
/**
 * WifiErrorの場合その番号を返す  エラーになってるかどうかを確認してから引き出すこと
 * @param   none
 * @retval  Error番号
 */
//==============================================================================

COMMSTATE_DWCERROR* CommStateGetWifiError(void)
{
    GF_ASSERT(_pCommState);
    return &_pCommState->aError;
}

//==============================================================================
/**
 * WIFIでログアウトを行う場合の処理
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _stateWifiLogout(void)
{
    int ret;
    
    CommSetWifiConnect(FALSE);
    if(mydwc_disconnect( 0 )){ mydwc_returnLobby();
        _CHANGE_STATE(_stateConnectEnd, 0);
    }
    ret = mydwc_stepmatch(FALSE);
    if( ret < 0 ){
        // エラー発生。
        _CHANGE_STATE(_wifiBattleError, 0);
    }
}

//==============================================================================
/**
 * WIFIでログアウトを行う場合の処
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommStateWifiLogout(void)
{
    if(_pCommState==NULL){  // すでに終了している
        return;
    }
    sys_SoftResetOK(SOFTRESET_TYPE_WIFI);
    CommInfoFinalize();
    _CHANGE_STATE(_stateConnectEnd, _EXIT_SENDING_TIME);
}

//==============================================================================
/**
 * WIFIでマッチングを切る場合の処理
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommStateWifiMatchEnd(void)
{
    if(_pCommState==NULL){  // すでに終了している
        return;
    }
    _pCommState->disconnectIndex = 0;
    _CHANGE_STATE(_stateWifiMatchEnd, 0);
}

//==============================================================================
/**
 * WIFIでマッチングを切る場合の処理 交換処理
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommStateWifiTradeMatchEnd(void)
{
    if(_pCommState==NULL){  // すでに終了している
        return;
    }
    CommStateSetErrorCheck(FALSE,TRUE);  // 再初期化されるまで エラー監視
    if(CommGetCurrentID() == COMM_PARENT_ID){
        _pCommState->disconnectIndex = 0;
    }
    else{
        _pCommState->disconnectIndex = 1;
    }
    _CHANGE_STATE(_stateWifiMatchEnd, 0);
}





void CommStateWifiBattleMatchEnd(void)
{
    u8 id = CommGetCurrentID();
    
    CommSendFixSizeData(CS_WIFI_EXIT,&id);
}


//==============================================================================
/**
 * WifiErrorかどうか
 * @param   none
 * @retval  Error
 */
//==============================================================================

BOOL CommStateIsWifiError(void)
{
    if(_pCommState){
        u32 stateAddr = (u32)_pCommState->state;
        if(stateAddr == (u32)_wifiBattleError){
            return TRUE;
        }
        if((stateAddr == (u32)_wifiBattleTimeout) && _pCommState->bDisconnectError){
            return TRUE;
        }
        if((stateAddr == (u32)_wifiLobbyTimeout) && _pCommState->bDisconnectError){
            return TRUE;
        }
    }
    return FALSE;
}

//==============================================================================
/**
 * 世界交換、世界バトルタワーの開始  エラー管理用
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommStateWifiDPWStart(SAVEDATA* pSaveData)
{
    if(!_pCommState){
        sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_COMMUNICATION, _HEAPSIZE_DPW );
        _pCommState = (_COMM_STATE_WORK*)sys_AllocMemory(HEAPID_COMMUNICATION, sizeof(_COMM_STATE_WORK));
        MI_CpuFill8(_pCommState, 0, sizeof(_COMM_STATE_WORK));
        _pCommState->serviceNo = COMM_MODE_DPW_WIFI;
        _pCommState->bWorldWifi = TRUE;
        _pCommState->pSaveData = pSaveData;
        CommStateSetErrorCheck(FALSE,TRUE);
//        DWC_SetReportLevel( DWC_REPORTFLAG_ALL);
        sys_SoftResetNG(SOFTRESET_TYPE_WIFI);
    }
}

//==============================================================================
/**
 * 世界交換、世界バトルタワーの終了
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommStateWifiDPWEnd(void)
{
    if(_pCommState){
        sys_SoftResetOK(SOFTRESET_TYPE_WIFI);
        CommStateSetErrorCheck(FALSE,FALSE);
        sys_FreeMemoryEz(_pCommState);
        _pCommState = NULL;
        sys_DeleteHeap(HEAPID_COMMUNICATION);
    }
}

//==============================================================================
/**
 * 世界ふしぎ交換  エラー管理用
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommStateWifiFusigiStart(SAVEDATA* pSaveData)
{
    if(!_pCommState){
        sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_COMMUNICATION, _HEAPSIZE_DPW );
        _pCommState = (_COMM_STATE_WORK*)sys_AllocMemory(HEAPID_COMMUNICATION, sizeof(_COMM_STATE_WORK));
        MI_CpuFill8(_pCommState, 0, sizeof(_COMM_STATE_WORK));
        _pCommState->serviceNo = COMM_MODE_FUSIGI_WIFI;
        _pCommState->bWorldWifi = TRUE;
        _pCommState->pSaveData = pSaveData;
        CommStateSetErrorCheck(FALSE,TRUE);
        sys_SoftResetNG(SOFTRESET_TYPE_WIFI);
    }
}

//==============================================================================
/**
 * 世界交換、世界バトルタワーの終了
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommStateWifiFusigiEnd(void)
{
    if(_pCommState){
        sys_SoftResetOK(SOFTRESET_TYPE_WIFI);
        CommStateSetErrorCheck(FALSE,FALSE);
        sys_FreeMemoryEz(_pCommState);
        _pCommState = NULL;
        sys_DeleteHeap(HEAPID_COMMUNICATION);
    }
}

//----------------------------------------------------------------------------
/**
 *	@brief	Email設定のWi-Fiエラーチェック開始
 *
 *	@param	pSaveData	セーブデータ
 */
//-----------------------------------------------------------------------------
void CommStateWifiEMailStart(SAVEDATA* pSaveData)
{
    if(!_pCommState){
        sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_COMMUNICATION, _HEAPSIZE_DPW );
        _pCommState = (_COMM_STATE_WORK*)sys_AllocMemory(HEAPID_COMMUNICATION, sizeof(_COMM_STATE_WORK));
        MI_CpuFill8(_pCommState, 0, sizeof(_COMM_STATE_WORK));
        _pCommState->serviceNo = COMM_MODE_EMAIL_WIFI;
        _pCommState->bWorldWifi = TRUE;
        _pCommState->pSaveData = pSaveData;
        CommStateSetErrorCheck(FALSE,TRUE);
//        DWC_SetReportLevel( DWC_REPORTFLAG_ALL);
        sys_SoftResetNG(SOFTRESET_TYPE_WIFI);
    }
}

//----------------------------------------------------------------------------
/**
 *	@brief	Email設定のWi-Fiエラーチェック終了
 *
 *	@param	pSaveData	セーブデータ
 */
//-----------------------------------------------------------------------------
void CommStateWifiEMailEnd(void)
{
    if(_pCommState){
        sys_SoftResetOK(SOFTRESET_TYPE_WIFI);
        CommStateSetErrorCheck(FALSE,FALSE);
        sys_FreeMemoryEz(_pCommState);
        _pCommState = NULL;
        sys_DeleteHeap(HEAPID_COMMUNICATION);
    }
}





//==============================================================================
/**
 * 世界交換、世界バトルタワーでエラーかどうか
 * @param   none
 * @retval  none
 */
//==============================================================================

BOOL CommStateGetWifiDPWError(void)
{
	int errorCode,ret;
	DWCErrorType myErrorType;
    
    if(!_pCommState){
        return FALSE;
    }

    if((_pCommState->serviceNo == COMM_MODE_DPW_WIFI) || (_pCommState->serviceNo == COMM_MODE_EMAIL_WIFI)){
        ret = DWC_GetLastErrorEx( &errorCode, &myErrorType );
        if(ret!=0){
            DWC_ClearError();
            return TRUE;
        }
    } else if(_pCommState->serviceNo == COMM_MODE_FUSIGI_WIFI){
        ret = DWC_GetLastErrorEx( &errorCode, &myErrorType );
        if(ret!=0){
			if(myErrorType == DWC_ETYPE_FATAL){
				OS_TPrintf("エラーを捕獲: %d, %d\n", errorCode, myErrorType);
				return TRUE;
			}
		}
    }
    return FALSE;
}


//==============================================================================
/**
 * 子機待機状態  親機を選択中
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _wifiBattleConnecting(void)
{
    // 接続中  
    int ret = mydwc_connect();

    _pCommState->timer--;

    if ( ret == STEPMATCH_CONNECT )  {
        OS_TPrintf("WiFiコネクションに接続しました。\n	");
		if( _pCommState->serviceNo == COMM_MODE_LOBBY_WIFI ){
			BOOL result;
			// Wi-Fiロビーにログイン開始
#ifndef PM_DEBUG
			// 通常LOGIN
			result = DWC_LOBBY_Login( _pCommState->cp_lobby_init_profile );
#else
			// DEBUGLOGIN
			if( D_Tomoya_WiFiLobby_ChannelPrefixFlag == TRUE ){
				OS_TPrintf( "prefix room %d\n", D_Tomoya_WiFiLobby_ChannelPrefix );
				result = DWC_LOBBY_LoginEx( _pCommState->cp_lobby_init_profile, D_Tomoya_WiFiLobby_ChannelPrefix );
			}else{
				result = DWC_LOBBY_Login( _pCommState->cp_lobby_init_profile );
			}
#endif
			// ログインに成功するまでループ
            OHNO_PRINT("LOGIN中  %d\n",result);
			if( result == TRUE ){
#ifdef COMMST_DEBUG_WFLBY_START
                {
                    // 今回の部屋データを設定	
                    DWC_LOBBY_DEBUG_SetRoomData( 20*60, 0, COMMST_DEBUG_WFLBY_START_room, COMMST_DEBUG_WFLBY_START_season );
                }
#endif
				
                // DWC_LoginAsyncに成功
                _pCommState->lobby_dwc_login = TRUE;
                _CHANGE_STATE(_wifiLobbyLogin, _pCommState->timer);
                return;
            }
            else{
                OHNO_PRINT("LOGIN ERROR  %d\n",result);
                _CHANGE_STATE(_wifiLobbyError, 0);
                return;
            }
        }else{
	        _CHANGE_STATE(_wifiBattleLogin, 0);
            return;
		}
    }
    else if(ret != 0){
	    _CHANGE_STATE(_wifiBattleError, 0);
    }
    if(_pCommState->timer <= 0){
	    _CHANGE_STATE(_wifiBattleError, 0);
    }
}


//==============================================================================
/**
 * 親機として初期化を行う
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _wifiBattleParentInit(void)
{
    MYSTATUS* pMyStatus;
    
    if(!CommIsVRAMDInitialize()){
        return;
    }
    {
        sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_WIFIMENU, _HEAPSIZE_WIFI);
    }

    if(CommParentModeInit(TRUE, TRUE, _PACKETSIZE_BATTLE,TRUE)){
        mydwc_startConnect(_pCommState->pSaveData, HEAPID_WIFIMENU,_HEAPSIZE_DWCP2P, CommLocalGetServiceMaxEntry( _pCommState->serviceNo ) + 1); //CommLocalGetServiceMaxEntry(COMM_MODE_LOGIN_WIFI)+1);
        mydwc_setFetalErrorCallback(CommFatalErrorFunc);

        CommSetTransmissonTypeDS();
        _CHANGE_STATE(_wifiBattleConnecting, MYDWC_TIMEOUTLOGIN);
    }
}

//==============================================================================
/**
 * 子機の初期化
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _wifiBattleChildInit(void)
{
    if(!CommIsVRAMDInitialize()){
        return;
    }

    {
        sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_WIFIMENU, _HEAPSIZE_WIFI);
    }
    
    if(CommChildModeInit(TRUE, TRUE, _PACKETSIZE_BATTLE)){
        mydwc_startConnect(_pCommState->pSaveData, HEAPID_WIFIMENU,_HEAPSIZE_DWCP2P, CommLocalGetServiceMaxEntry(_pCommState->serviceNo)+1);
        CommSetTransmissonTypeDS();
        _CHANGE_STATE(_wifiBattleConnecting, MYDWC_TIMEOUTLOGIN);
    }
}

//==============================================================================
/**
 * バトル時の子としての通信処理開始
 * @param   serviceNo  通信サービス番号
 * @retval  none
 */
//==============================================================================

#ifdef PM_DEBUG
void CommStateWifiEnterBattleChild(SAVEDATA* pSaveData, int serviceNo, int regulationNo, int soloDebugNo)
#else
void CommStateWifiEnterBattleChild(SAVEDATA* pSaveData, int serviceNo, int regulationNo)
#endif
{
    MYSTATUS* pMyStatus = SaveData_GetMyStatus(pSaveData);
    if(CommIsInitialize()){
        return;      // つながっている場合今は除外する
    }
    sys_SoftResetNG(SOFTRESET_TYPE_WIFI);
    // 通信ヒープ作成
    sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_COMMUNICATION, _HEAPSIZE_BATTLE );
    _commStateInitialize(pSaveData,serviceNo);
//    _pCommState->serviceNo = serviceNo;
    _pCommState->regulationNo = regulationNo;
    _pCommState->pSaveData = pSaveData;
#ifdef PM_DEBUG
    _pCommState->soloDebugNo = soloDebugNo;
#endif
    
    _CHANGE_STATE(_wifiBattleChildInit, 0);
}


//==============================================================================
/**
 * バトル時の親としての通信処理開始
 * @param   pMyStatus  mystatus
 * @param   serviceNo  通信サービス番号
 * @param   regulationNo  通信サービス番号
 * @retval  none
 */
//==============================================================================

#ifdef PM_DEBUG
void CommStateWifiEnterBattleParent(SAVEDATA* pSaveData, int serviceNo, int regulationNo, int soloDebugNo)
#else
void CommStateWifiEnterBattleParent(SAVEDATA* pSaveData, int serviceNo, int regulationNo)
#endif
{
    MYSTATUS* pMyStatus = SaveData_GetMyStatus(pSaveData);
    if(CommIsInitialize()){
        return;      // つながっている場合今は除外する
    }
    // 通信ヒープ作成
    sys_SoftResetNG(SOFTRESET_TYPE_WIFI);
    sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_COMMUNICATION, _HEAPSIZE_BATTLE );
    _commStateInitialize(pSaveData,serviceNo);
//    _pCommState->serviceNo = serviceNo;
    _pCommState->regulationNo = regulationNo;
    _pCommState->pSaveData = pSaveData;
#ifdef PM_DEBUG
    _pCommState->soloDebugNo = soloDebugNo;
#endif
    _CHANGE_STATE(_wifiBattleParentInit, 0);
}

//==============================================================================
/**
 * WIFI環境にログインする為の通信開始
 * @param   pSaveData
 * @retval  none
 */
//==============================================================================

void* CommStateWifiEnterLogin(SAVEDATA* pSaveData, int wifiFriendStatusSize)
{
    MYSTATUS* pMyStatus = SaveData_GetMyStatus(pSaveData);
    if(CommIsInitialize()){
        return NULL;      // つながっている場合今は除外する
    }
    // 通信ヒープ作成
    sys_SoftResetNG(SOFTRESET_TYPE_WIFI);
    sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_COMMUNICATION, _HEAPSIZE_BATTLE );
    _commStateInitialize(pSaveData,COMM_MODE_LOGIN_WIFI);
    _pCommState->pWifiFriendStatus = sys_AllocMemory( HEAPID_COMMUNICATION, wifiFriendStatusSize );
    MI_CpuFill8( _pCommState->pWifiFriendStatus, 0, wifiFriendStatusSize );
//    _pCommState->serviceNo = COMM_MODE_LOGIN_WIFI;
    _pCommState->regulationNo = 0;
    _pCommState->pSaveData = pSaveData;
#ifdef PM_DEBUG
    _pCommState->soloDebugNo = 0;
#endif

    
    _CHANGE_STATE(_wifiBattleParentInit, 0);
    OHNO_PRINT("pMatchAddr %x\n",(u32)_pCommState->pWifiFriendStatus);
    return _pCommState->pWifiFriendStatus;
}


#ifdef COMMST_DEBUG_WFLBY_START
void CommStateWifiLobbyLogin_Debug( SAVEDATA* p_save, const void* cp_initprofile, u32 season, u32 room )
{
	CommStateWifiLobbyLogin( p_save, cp_initprofile );

	COMMST_DEBUG_WFLBY_START_season = season;
	COMMST_DEBUG_WFLBY_START_room	= room;
}
#endif

//----------------------------------------------------------------------------
/**
 *	@brief	WiFiロビー専用	ログイン関数
 *
 *	@param	p_save				セーブデータ
 *	@param	cp_initprofile		初期化プロフィールデータ
 */
//-----------------------------------------------------------------------------
void CommStateWifiLobbyLogin( SAVEDATA* p_save, const void* cp_initprofile )
{
    MYSTATUS* pMyStatus = SaveData_GetMyStatus( p_save );
    if(CommIsInitialize()){
		TOMOYA_PRINT( "now status  wifiLobby connecting\n" );
        return ;      // つながっている場合今は除外する
    }

#ifdef COMMST_DEBUG_WFLBY_START
	COMMST_DEBUG_WFLBY_START_season = 0;
	COMMST_DEBUG_WFLBY_START_room = 0;
#endif

    // 通信ヒープ作成
    sys_SoftResetNG(SOFTRESET_TYPE_WIFI);
    sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_COMMUNICATION, _HEAPSIZE_WIFILOBBY );
    _commStateInitialize( p_save, COMM_MODE_LOBBY_WIFI );
    _pCommState->pWifiFriendStatus = NULL;

	_pCommState->cp_lobby_init_profile = cp_initprofile;	// 初期化プロフィールを保存
	_pCommState->lobby_dwc_login = FALSE;	// DWC_LoginASync処理前

    _pCommState->regulationNo = 0;
    _pCommState->pSaveData = p_save;
#ifdef PM_DEBUG
    _pCommState->soloDebugNo = 0;
#endif

    
    _CHANGE_STATE(_wifiLobbyCommInit, 0);
}

//----------------------------------------------------------------------------
/**
 *	@brief	WiFiロビーのログアウト
 */
//-----------------------------------------------------------------------------
void CommStateWifiLobbyLogout( void )
{
    if(_pCommState==NULL){  // すでに終了している
        return;
    }
    _CHANGE_STATE(_wifiLobbyLogout, 0);
}

//----------------------------------------------------------------------------
/**
 *	@brief	DWC_LoginAsyncのログインまでが完了したかチェック
 *	@retval	TRUE	した
 *	@retval	FALSE	してない
 */
//-----------------------------------------------------------------------------
BOOL CommStateWifiLobbyDwcLoginCheck( void )
{
    if(_pCommState==NULL){  // すでに終了している
        return FALSE;
    }

	return _pCommState->lobby_dwc_login;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WiFiロビー特有のエラー状態になっているかチェック
 *
 *	@retval	TRUE	WiFiロビー特有のエラー状態
 *	@retval	FALSE	エラー状態ではない
 */
//-----------------------------------------------------------------------------
BOOL CommStateWifiLobbyError( void )
{
    if(_pCommState){
        u32 stateAddr = (u32)_pCommState->state;
        if(stateAddr == (u32)_wifiLobbyError){
            return TRUE;
        }
    }
    return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WiFiロビーミニゲーム開始
 *
 *	@param	type		ゲームタイプ
 */
//-----------------------------------------------------------------------------
void CommStateWifiP2PStart( DWC_LOBBY_MG_TYPE type )
{
	GF_ASSERT( _pCommState );
	GF_ASSERT( CommStateIsWifiLoginMatchState() == TRUE );

	// 接続開始
	if( DWC_LOBBY_MG_CheckRecruit( type ) == FALSE ){
		// 誰も募集していないので、自分が募集する
	    DWC_LOBBY_MG_StartRecruit( type, 4 );
		_CHANGE_STATE(_wifiLobbyP2PMatchWait, 0);
	}else{
		// 募集に参加する
		DWC_LOBBY_MG_Entry( type );
		_CHANGE_STATE(_wifiLobbyP2PMatchWait, 0);
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	WiFiロビーミニゲーム終了
 *			P2P切断チェックは、CommStateWifiP2PGetConnectStateで行ってください
 */
//-----------------------------------------------------------------------------
void CommStateWifiP2PEnd( void )
{
	u32 status;

	if( DWC_LOBBY_MG_MyParent() == TRUE ){
		// 親なら募集情報を破棄
		DWC_LOBBY_MG_EndRecruit();
	}
	// P2P接続終了
	DWC_LOBBY_MG_EndConnect();

	// 切断処理へ
	_CHANGE_STATE(_wifiLobbyP2PDisconnect, 0);
}

//----------------------------------------------------------------------------
/**
 *	@brief	P2Pに接続しているかチェック
 *
 *	@retval	COMMSTATE_WIFIP2P_CONNECT_NONE,			// 接続してない
 *	@retval	COMMSTATE_WIFIP2P_CONNECT_MATCHWAIT,	// 接続待ち
 *	@retval	COMMSTATE_WIFIP2P_CONNECT_MATCH,		// 接続完了
 *	@retval	COMMSTATE_WIFIP2P_CONNECT_DISCONNECT,	// 切断中
 */
//-----------------------------------------------------------------------------
u32 CommStateWifiP2PGetConnectState( void )
{
    if(_pCommState){
        u32 stateAddr = (u32)_pCommState->state;
        if(stateAddr == (u32)_wifiLobbyP2PMatch){
            return COMMSTATE_WIFIP2P_CONNECT_MATCH;
        }else if(stateAddr == (u32)_wifiLobbyP2PMatchWait){
            return COMMSTATE_WIFIP2P_CONNECT_MATCHWAIT;
		}else if( stateAddr == (u32)_wifiLobbyP2PDisconnect ){
			return COMMSTATE_WIFIP2P_CONNECT_DISCONNECT;
		}
    }
    return COMMSTATE_WIFIP2P_CONNECT_NONE;
}


//==============================================================================
/**
 * WIFIフレンド用WORKを渡す
 * @param   pSaveData
 * @retval  none
 */
//==============================================================================

void* CommStateGetMatchWork(void)
{
    OHNO_PRINT("pMatchAddr %x\n",(u32)_pCommState->pWifiFriendStatus);
    return _pCommState->pWifiFriendStatus;
}

//==============================================================================
/**
 * ここから先エラーの検査を通信が処理するかどうかを設定
 * @param   bFlg    切断=エラーにする
 * @param   bAuto  TRUEで検査開始
 * @retval  none
 */
//==============================================================================

void CommStateSetErrorCheck(BOOL bFlg,BOOL bAuto)
{
    OHNO_PRINT("CommStateSetErrorCheckChange %d %d\n",bFlg, bAuto);
    if(_pCommState){
        _pCommState->bDisconnectError = bFlg;
        _pCommState->bErrorAuto = bAuto;
        if(bFlg){
            _pCommState->nowConnectNum = CommGetConnectNum();
        }
        else{
            _pCommState->nowConnectNum = 0;
        }
    }
    CommMPSetNoChildError(bFlg);  // 子機がいなくなったら再検索するためにERR扱いにする
    CommMPSetDisconnectOtherError(bFlg);
}


//==============================================================================
/**
 * ここから先エラーの検査を通信が処理するかどうかを設定
 * @param   bFlg  TRUEで検査開始
 * @retval  none
 */
//==============================================================================

BOOL CommStateGetErrorCheck(void)
{
    if(_pCommState){
        if(_pCommState->stateError!=0){
            return TRUE;
        }
        return _pCommState->bErrorAuto;
    }
    return FALSE;
//    CommMPSetDisconnectOtherError(bFlg);
}


#ifdef PM_DEBUG
//==============================================================================
/**
 * デバッグ用通信接続開始
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommStateRecvDebugStart(int netID, int size, void* pData, void* pWork)
{
    if(_pCommState){
        _pCommState->bDebugStart = TRUE;
    }
}

BOOL CommStateDBattleIsReady(void)
{
    if(_pCommState){
        return _pCommState->bDebugStart;
    }
    return FALSE;
}

static void _commConnectChildDebug(TCB_PTR tcb, void* work)
{
    if(!CommIsVRAMDInitialize()){
        return;
    }
    if(CommMPGetGFBss(_pCommState->connectIndex)!=NULL){
        CommStateConnectBattleChild(_pCommState->connectIndex);  // 接続
        TCB_Delete(tcb);
    }
}

static void _commConnectParentDebug(TCB_PTR tcb, void* work)
{
    if(!CommIsVRAMDInitialize()){
        return;
    }
    if(CommGetConnectNum() == (CommLocalGetServiceMaxEntry(CommStateGetServiceNo())+1) ){
        CommSendFixData(CS_DEBUG_START);  // 全員にコマンド送信
        TCB_Delete(tcb);
    }
}

void CommStateDBattleConnect(BOOL bParent, int gameMode, SAVEDATA* pSaveData)
{
    MYSTATUS* pMyStatus = SaveData_GetMyStatus(pSaveData);
    if(COMM_MODE_BATTLE_SINGLE_WIFI <= gameMode){
/*  今は封印
        DwcOverlayStart();
        if(!bParent){
            CommStateWifiEnterBattleChild(pSaveData, gameMode, 0, SOLO_DEBUG_NO + COMMDIRECT_DEBUG_NO);
            _pCommState->connectIndex = 0;
            TCB_Add(_commConnectChildDebug, NULL, _TCB_COMMCHECK_PRT-2);
        }
        else{
            CommStateWifiEnterBattleParent(pSaveData, gameMode, 0, SOLO_DEBUG_NO + COMMDIRECT_DEBUG_NO);
            TCB_Add(_commConnectParentDebug, NULL, _TCB_COMMCHECK_PRT-2);
        }
   */
    }
    else{
        if(!bParent){
            CommStateEnterBattleChild(pSaveData, gameMode, 0, NULL, FALSE, SOLO_DEBUG_NO + COMMDIRECT_DEBUG_NO);
            _pCommState->connectIndex = 0;
            TCB_Add(_commConnectChildDebug, NULL, _TCB_COMMCHECK_PRT-1);
        }
        else{
            CommStateEnterBattleParent(pSaveData, gameMode, 0, NULL, FALSE, SOLO_DEBUG_NO + COMMDIRECT_DEBUG_NO);
            TCB_Add(_commConnectParentDebug, NULL, _TCB_COMMCHECK_PRT-1);
        }
    }
    _pCommState->bDebugStart = FALSE;
}

#endif //PM_DEBUG



int CommStateGetServiceNo(void)
{
    if(_pCommState){
        return _pCommState->serviceNo;
    }
    return COMM_MODE_BATTLE_SINGLE;
}

int CommStateGetRegulationNo(void)
{
    if(_pCommState){
        return _pCommState->regulationNo;
    }
    return COMM_REG_NORMAL;
}

void CommStateSetPokemon(u8* sel)
{
    MI_CpuCopy8(sel,_pCommState->select,NELEMS(_pCommState->select));

    OHNO_SP_PRINT("%d %d %d %d %d %d\n",sel[0],sel[1],sel[2],sel[3],sel[4],sel[5]);
}

void CommStateGetPokemon(u8* sel)
{
    MI_CpuCopy8(_pCommState->select,sel,NELEMS(_pCommState->select));
}


#ifdef PM_DEBUG
int CommStateGetSoloDebugNo(void)
{
    if(_pCommState){
        return _pCommState->soloDebugNo;
    }
    return 0;
}
#endif


//==============================================================================
/**
 * 通信がWIFI接続しているかどうかを返す
 * @param   none
 * @retval  接続していたらTRUE
 */
//==============================================================================

BOOL CommStateIsWifiConnect(void)
{
    return CommLocalIsWiFiGroup(CommStateGetServiceNo());
}


//==============================================================================
/**
 * エラー状態に入る
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommSetErrorReset(u8 type)
{
    if(_pCommState){
        _pCommState->ResetStateType = type;
    }
}

//==============================================================================
/**
 * リセットエラー状態になったかどうか確認する
 * @param   none
 * @retval  リセットエラー状態なら種類を返す エラーでなければFALSE=0
 */
//==============================================================================

u8 CommIsResetError(void)
{
    if(sys_SioErrorNG_Get()){
        return FALSE;
    }
    if(_pCommState){
        return _pCommState->ResetStateType;
    }
    return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   fatalエラー用関数  この関数内から抜けられない
 * @param   no   エラー番号
 * @retval  none
 */
//--------------------------------------------------------------

void CommFatalErrorFunc(int no)
{
    int i=0;
    
    ComErrorWarningResetCall(HEAPID_BASE_SYSTEM,COMM_ERRORTYPE_POWEROFF, no);
    while(1){
        i++;
    }
}

void CommFatalErrorFunc_NoNumber( void )
{
    int i=0;
    
    ComErrorWarningResetCall( HEAPID_BASE_SYSTEM, 4, 0 );
    while(1){
        i++;
    }
}

//--------------------------------------------------------------
/**
 * @brief   エラーを感知して 通信エラー用ウインドウを出す
 * @param   heapID    メモリー確保するHEAPID
 * @param   bgl       GF_BGL_INI
 * @retval  none
 */
//--------------------------------------------------------------

void CommErrorCheck(int heapID, GF_BGL_INI* bgl)
{
// この関数では処理しないことになりました
}


//--------------------------------------------------------------
/**
 * @brief   エラーを感知して 通信エラー用ウインドウを出す
 * @param   heapID    メモリー確保するHEAPID
 * @param   bgl       GF_BGL_INI
 * @retval  none
 */
//--------------------------------------------------------------

void CommErrorDispCheck(int heapID)
{
    if(CommStateGetErrorCheck()){

        if(CommIsError() || CommStateIsWifiError() || CommStateGetWifiDPWError()
           || (_pCommState->stateError!=0) || CommStateWifiLobbyError() ){
            if( !sys_SioErrorNG_Get() ){
                if(!CommIsResetError() ){   // リセットエラー状態で無い場合
                    Snd_Stop();
                    SaveData_DivSave_Cancel(_pCommState->pSaveData); // セーブしてたら止める
                    sys.tp_auto_samp = 1;  // サンプリングも止める
                    
                    if(_pCommState->stateError == COMM_ERROR_RESET_GTS){
                        CommSetErrorReset(COMM_ERROR_RESET_GTS);  // エラーリセット状態になる
                    }
                    else if((_pCommState->serviceNo == COMM_MODE_FUSIGI_WIFI) ||
                            (_pCommState->serviceNo == COMM_MODE_MYSTERY) ||
                            (_pCommState->serviceNo == COMM_MODE_EMAIL_WIFI) ){
                        CommSetErrorReset(COMM_ERROR_RESET_TITLE);  // エラーリセット状態になる
                    }
                    else{
                        CommSetErrorReset(COMM_ERROR_RESET_SAVEPOINT);  // エラーリセット状態になる
                    }
                }
            }
        }
    }
}

//--------------------------------------------------------------
/**
 * @brief   リセットできる状態かどうか
 * @param   none
 * @retval  TRUE ならリセット
 */
//--------------------------------------------------------------

BOOL CommStateIsResetEnd(void)
{
    if(CommMPIsConnectStalth() || !CommStateIsInitialize()){ // 通信終了
        return TRUE;
    }
    if(!CommMPIsConnect()){
        return TRUE;
    }
    return FALSE;
}

//==============================================================================
/**
 * エラーにする場合この関数を呼ぶとエラーになります
 * @param   エラー種類
 * @retval  受け付けた場合TRUE
 */
//==============================================================================

BOOL CommStateSetError(int no)
{

    if(_pCommState){
        _pCommState->stateError = no;
        CommSystemShutdown();
        return TRUE;
    }
    return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	通信状態をWiFiポフィン４人接続用にする
 */
//-----------------------------------------------------------------------------
void CommStateChangeWiFiPofin( void )
{
	_pCommState->serviceNo = COMM_MODE_WIFI_POFIN;


    mydwc_setReceiver( CommRecvOtherCallback, CommRecvOtherCallback );
}

//----------------------------------------------------------------------------
/**
 *	@brief	通信状態をWiFiクラブ用にする
 */
//-----------------------------------------------------------------------------
void CommStateChangeWiFiClub( void )
{
	_pCommState->serviceNo = COMM_MODE_CLUB_WIFI;
    mydwc_setReceiver( CommRecvOtherCallback, CommRecvOtherCallback );
}

//----------------------------------------------------------------------------
/**
 *	@brief	通信状態をWiFiミニゲーム人接続用にする
 */
//-----------------------------------------------------------------------------
void CommStateChangeWiFiLobbyMinigame( void )
{
	_pCommState->serviceNo = COMM_MODE_LOBBY_WIFI;

    mydwc_setReceiver( CommRecvOtherCallback, CommRecvOtherCallback );
}

//----------------------------------------------------------------------------
/**
 *	@brief	通信状態をWiFi一人用状態に戻す
 */
//-----------------------------------------------------------------------------
void CommStateChangeWiFiLogin( void )
{
	_pCommState->serviceNo = COMM_MODE_LOGIN_WIFI;
    mydwc_setReceiver( CommRecvParentCallback, CommRecvCallback );
}

//----------------------------------------------------------------------------
/**
 *	@brief	通信状態をバトル用に変更
 */
//-----------------------------------------------------------------------------
void CommStateChangeWiFiBattle( void )
{
	_pCommState->serviceNo = COMM_MODE_BATTLE_SINGLE_WIFI;
    mydwc_setReceiver( CommRecvParentCallback, CommRecvCallback );
}


//-------------------------------------
///	WiFiロビーで追加
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	通信初期化
 */
//-----------------------------------------------------------------------------
static void _wifiLobbyCommInit( void )
{
    if(!CommIsVRAMDInitialize()){
        return;
    }
    {
        sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_WIFIMENU, _HEAPSIZE_WIFI_LOBBY);
    }

    if(CommParentModeInit(TRUE, TRUE, _PACKETSIZE_BATTLE,TRUE)){
		mydwc_startConnect(_pCommState->pSaveData, HEAPID_WIFIMENU, _HEAPSIZE_DWCLOBBY, CommLocalGetServiceMaxEntry(_pCommState->serviceNo)+1); 
		mydwc_setFetalErrorCallback(CommFatalErrorFunc);
		CommSetTransmissonTypeDS();

		// ボイスチャットはなし
		mydwc_setVChat( FALSE );
			
		// BothNet
		CommSetWifiBothNet( FALSE );
	
		_CHANGE_STATE(_wifiBattleConnecting, MYDWC_TIMEOUTLOGIN);
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	ロビーサーバにログイン
 */
//-----------------------------------------------------------------------------
static void _wifiLobbyLogin( void )
{
	BOOL result;

	// タイムアウト処理を追加
	_pCommState->timer--;
	if( _pCommState->timer <= 0 ){
		// 通信エラーへ
	    _CHANGE_STATE(_wifiBattleError, 0);
		return ;
	}


    CommSetWifiConnect(TRUE);

	result = _wifiLobbyUpdateCommon();
	if( result == FALSE ){ return ; }	// エラー起きた

	// ログイン完了待ち
	result = DWC_LOBBY_LoginWait();
	if( result ){

		//接続完了
		_CHANGE_STATE(_wifiLobbyConnect, 0);
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	サーバ接続中
 */
//-----------------------------------------------------------------------------
static void _wifiLobbyConnect( void )
{
	BOOL result;

	result = _wifiLobbyUpdateCommon();
	if( result == FALSE ){ return ; }	// エラー起きた
}

//----------------------------------------------------------------------------
/**
 *	@brief	dwc_stepの戻り値をチェックする
 *
 *	@param	dwc_err		戻り値
 *
 *	@retval	TRUE	正常
 *	@retval	FALSE	エラー
 */
//-----------------------------------------------------------------------------
static BOOL _wifiLobbyDwcStepRetCheck( int dwc_err )
{
	BOOL ret = TRUE;
    if((dwc_err >= DWC_ERROR_FRIENDS_SHORTAGE) && (STEPMATCH_SUCCESS > dwc_err)){
        _CHANGE_STATE(_wifiBattleFailed, 0);	// 軽度なエラー
		ret = FALSE;
    }
    else if( dwc_err < 0 ){
        // エラー発生。
        //_errcodeConvert(dwc_err);
        _CHANGE_STATE(_wifiBattleError, 0);
		ret = FALSE;
    }		
    else if( dwc_err == STEPMATCH_TIMEOUT ) {     // タイムアウト（相手から１０秒以上メッセージが届かない）
        _CHANGE_STATE(_wifiLobbyTimeout, 0);
		ret = FALSE;
    }
    else if(dwc_err == STEPMATCH_DISCONNECT){
		ret = FALSE;
        if(_pCommState->bDisconnectError){
            _CHANGE_STATE(_wifiBattleError, 0);	// 切断をエラー扱い
        }
        else{	
            _CHANGE_STATE(_wifiBattleDisconnect, 0);	// 切断は切断エラー
        }
    }

    if(_pCommState->bDisconnectError){
        if(_pCommState->nowConnectNum != CommGetConnectNum()){
            _CHANGE_STATE(_wifiBattleError, 0);
			ret = FALSE;
        }
    }
	return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief		ロビーサーバアップデート処理
 */
//-----------------------------------------------------------------------------
static BOOL _wifiLobbyUpdate( void )
{
	DWC_LOBBY_CHANNEL_STATE lobby_err;
	BOOL ret = TRUE;

	// Wi-Fiロビー毎フレーム処理
	lobby_err = DWC_LOBBY_Update();
	switch( lobby_err ){
	//  正常
    case DWC_LOBBY_CHANNEL_STATE_NONE:           // チャンネルに入っていない。
    case DWC_LOBBY_CHANNEL_STATE_LOGINWAIT:		// チャンネルに入室中。
    case DWC_LOBBY_CHANNEL_STATE_CONNECT:		// チャンネルに入室済み。
    case DWC_LOBBY_CHANNEL_STATE_LOGOUTWAIT:     // チャンネルに退室中。
		break;
	
	// エラー処理
	case DWC_LOBBY_CHANNEL_STATE_ERROR:           // チャンネル状態を取得できませんでした。
		_CHANGE_STATE(_wifiLobbyError, 0);
		ret = FALSE;
		break;
	}

	return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WiFiロビー状態更新処理	
 *
 *	@retval	TRUE	正常
 *	@retval	FALSE	エラー発生
 */
//-----------------------------------------------------------------------------
static BOOL _wifiLobbyUpdateCommon( void )
{
	int dwc_err;
	BOOL ret;

	dwc_err = mydwc_stepmatch(FALSE);

	// エラーチェック
	ret = _wifiLobbyDwcStepRetCheck( dwc_err );
	if( ret == FALSE ){
		return ret;
	}

	// ロビーのアップデート
	ret = _wifiLobbyUpdate();
	return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WiFiロビー状態更新処理	
 *
 *	@param	p_matchret		マッチングに対する応答
 * 
 *	@retval	STEPMATCH_CONTINUE…マッチング中
 *	@retval	STEPMATCH_SUCCESS…成功
 *	@retval	STEPMATCH_CANCEL…キャンセル
 *	@retval	STEPMATCH_FAIL  …相手が親をやめたため、接続を中断
 */
//-----------------------------------------------------------------------------
static BOOL _wifiLobbyUpdateMatchCommon( u32* p_matchret )
{
	int dwc_err;
	BOOL ret;

	dwc_err = mydwc_stepmatch( FALSE );

	// マッチング結果を設定
	if( dwc_err >= STEPMATCH_SUCCESS ){
		*p_matchret = dwc_err;
	}else{
		*p_matchret = STEPMATCH_CONTINUE;

		// エラーチェック
		ret = _wifiLobbyDwcStepRetCheck( dwc_err );
		if( ret == FALSE ){
			return ret;
		}
	}

	// ロビーのアップデート
	ret = _wifiLobbyUpdate();
	return ret;
}


//----------------------------------------------------------------------------
/**
 *	@brief	ログアウト開始
 */
//-----------------------------------------------------------------------------
static void _wifiLobbyLogout( void )
{
	// Wi-Fiロビーからログアウト
	DWC_LOBBY_Logout();

	_CHANGE_STATE(_wifiLobbyLogoutWait, 0);
}

//----------------------------------------------------------------------------
/**
 *	@brief	Wi-Fiロビーログアウト
 */
//-----------------------------------------------------------------------------
static void _wifiLobbyLogoutWait( void )
{
	BOOL result;
	
	// ロビーとDWCを動かし続ける
	mydwc_stepmatch(FALSE);
	_wifiLobbyUpdate();

	// ログアウト終了待ち
	result  = DWC_LOBBY_LogoutWait();
	if( result ){
		sys_SoftResetOK(SOFTRESET_TYPE_WIFI);
		CommInfoFinalize();
		_CHANGE_STATE(_stateConnectEnd, _EXIT_SENDING_TIME);

		CommSetWifiConnect(FALSE);
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ロビー特有のエラー
 */
//-----------------------------------------------------------------------------
static void _wifiLobbyError( void ){}



//----------------------------------------------------------------------------
/**
 *	@brief	LobbyP2P接続処理
 */
//-----------------------------------------------------------------------------
static void _wifiLobbyP2PMatch( void )
{
	BOOL result;
	//  ロビーサーバとの接続処理
	_wifiLobbyUpdateCommon();

	// ForceEndで終了
	result = DWC_LOBBY_MG_ForceEnd();
	if( result == TRUE ){
		// P2P接続終了
		DWC_LOBBY_MG_EndConnect();

		// 切断処理へ
		_CHANGE_STATE(_wifiLobbyP2PDisconnect, 0);
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	LobbyP2P親接続待ち処理
 */
//-----------------------------------------------------------------------------
static void _wifiLobbyP2PMatchWaitParent( void )
{
	BOOL result;
	DWC_LOBBY_MG_ENTRYRET entrywait;

	// ForceEndで終了
	result = DWC_LOBBY_MG_ForceEnd();
	if( result == TRUE ){
		// P2P接続終了
		DWC_LOBBY_MG_EndConnect();

		// 切断処理へ
		_CHANGE_STATE(_wifiLobbyP2PDisconnect, 0);
		return ;
	}

	//  ロビーサーバとの接続処理
	_wifiLobbyUpdateCommon();

	// 接続完了チェック
	entrywait = DWC_LOBBY_MG_EntryWait();
	switch( entrywait ){
	case DWC_LOBBY_MG_ENTRYWAIT:		// エントリー中
		break;
	
	case DWC_LOBBY_MG_ENTRYOK:			// エントリー完了
		_CHANGE_STATE(_wifiLobbyP2PMatch, 0);
		break;
	
	case DWC_LOBBY_MG_ENTRYNG:			// エントリー失敗
	case DWC_LOBBY_MG_ENTRYNONE:		// 何もしていない
	default:							// その他
		// P2P接続終了
		DWC_LOBBY_MG_EndConnect();
		// 状態を戻す
		_CHANGE_STATE(_wifiLobbyP2PDisconnect, 0);
		break;
	}

}

//----------------------------------------------------------------------------
/**
 *	@brief	LobbyP2P子接続待ち処理
 */
//-----------------------------------------------------------------------------
static void _wifiLobbyP2PMatchWait( void )
{
	BOOL result;
	u32 matchret;

	// ForceEndで終了
	result = DWC_LOBBY_MG_ForceEnd();
	if( result == TRUE ){
		// P2P接続終了
		DWC_LOBBY_MG_EndConnect();

		// 切断処理へ
		_CHANGE_STATE(_wifiLobbyP2PDisconnect, 0);
		return ;
	}

	//  ロビーサーバとの接続処理
	_wifiLobbyUpdateMatchCommon( &matchret );

	// 接続完了したら子接続処理へ
	switch( matchret ){
	case STEPMATCH_CONTINUE:		// エントリー中
		break;
	
	case STEPMATCH_SUCCESS:			// エントリー完了
		_CHANGE_STATE(_wifiLobbyP2PMatch, 0);
		break;
	
	default:							// その他
		// P2P接続終了
		DWC_LOBBY_MG_EndConnect();
		// 状態を戻す
		_CHANGE_STATE(_wifiLobbyConnect, 0);
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	LobbyP2P切断処理
 */
//-----------------------------------------------------------------------------
static void _wifiLobbyP2PDisconnect( void )
{
	BOOL result;
	int ret;

    OS_TPrintf("_wifiLobbyP2PDisconnect\n");
	ret = mydwc_stepmatch( TRUE );	// 切断処理
    if( ret < 0 ){
        // エラー発生。
        _CHANGE_STATE(_wifiBattleError, 0);
		return ;
    }
	 
	// ロビーのアップデート
	result = _wifiLobbyUpdate();
	if( result == FALSE ){ return ; }
	

    if(mydwc_disconnect( 0 )){
		ret = mydwc_returnLobby();
        if( ret ){
			CommSystemReset();
			CommSetWifiBothNet( FALSE );
			_CHANGE_STATE(_wifiLobbyConnect, 0);
		}
    }
}


//----------------------------------------------------------------------------
/**
 *	@brieof	WiFi広場専用タイムアウト処理
 */
//-----------------------------------------------------------------------------
static void _wifiLobbyTimeout( void )
{
   int ret;
    ret = mydwc_stepmatch(FALSE);

	if( ret < 0 ){
        // エラー発生。
        _CHANGE_STATE(_wifiBattleError, 0);
    }		

	// ロビーのアップデート
	_wifiLobbyUpdate();
}


//----------------------------------------------------------------------------
/**
 *	@brief	通信状態をファクトリーバトル用に変更
 */
//-----------------------------------------------------------------------------
void CommStateChangeWiFiFactory( void )
{
	_pCommState->serviceNo = COMM_MODE_WIFI_FRONTIER_COMMON;
}



//==============================================================================
/**
 * ポケッチの時以外で通信しているかどうかを返す
 * @retval  通信状態中はTRUE
 */
//==============================================================================
BOOL CommStateIsInitializeOtherPoketch(void)
{
    if(_pCommState && (_pCommState->serviceNo == COMM_MODE_POKETCH)){
        return FALSE;
    }
    return CommIsInitialize();
}

