//=============================================================================
/**
 * @file	comm_field_state.c
 * @brief	通信状態を管理するサービス  通信の上位にある
 *          スレッドのひとつとして働き、自分の通信状態や他の機器の
 *          開始や終了を管理する
 *          フィールド上で直接管理したいのでcommunicationからfieldに移動 2005.09.28
 * @author	Katsumi Ohno
 * @date    2005.08.02
 */
//=============================================================================

#include "common.h"
#include "communication/communication.h"
#include "communication/comm_state.h"
#include "system/gamedata.h"
#include "system/pm_str.h"

#include "comm_field_state.h"
#include "underground/ug_manager.h"
#include "field/field.h"
#include "fieldsys.h"
#include "comm_command_field.h"
//#include "comm_direct_counter.h"
//#include "comm_vsroom.h"
#include "sysflag.h"

#include "script.h"							//StartFieldScript
#include "..\fielddata\script\common_scr_def.h"		//SCRID_TRAINER_MOVE_SCRIPT
#include "..\fielddata\script\connect_def.h"		//SCRID_CONNECT_COLOSSEUM
#include "../fielddata/script/battle_room_def.h"
#include "savedata/trainercard_data.h"
#include "application/trainer_card.h"
#include "tr_card_setup.h"
#include "system/wipe.h"
#include "system/pm_overlay.h"
#include "system/brightness.h"

#include "field/union_beacon_tool.h"
#include "field_encount.h"

#ifdef PM_DEBUG
#include "d_ohno.h"
#include "fld_debug.h"
#endif

#include "comm_field_state_local.h"
//==============================================================================
// 定義
//==============================================================================
#define _START_TIME (50)     // 開始時間
#define _CHILD_P_SEARCH_TIME (12) ///子機として親を探す時間
#define _PARENT_WAIT_TIME (40) ///親としてのんびり待つ時間
#define _FINALIZE_TIME (2)
#define _EXIT_SENDING_TIME (5)
#define _PARENT_END_TIME (2)
#define _SEND_NAME_TIME (10)
#define _PARENTSCAN_PA (3)  // 親機として検索する確立は1/3

#define _TCB_COMMCHECK_PRT   (10)    ///< フィールドを歩く通信の監視ルーチンのPRI


//==============================================================================
// static宣言
//==============================================================================

// ステートの初期化

static void _commCheckFunc(TCB_PTR tcb, void* work);  // ステートを実行しているタスク
static void _changeState(PTRStateFunc state, int time);  // ステートを変更する
static void _changeStateDebug(PTRStateFunc state, int time, int line);  // ステートを変更する

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
static void _underStart(void);           // 初期化 + システムの初期化
static void _underOpenning(void);       //イベント中
static void _underConnecttingAloneSend(void);  // 初期化転送完了
static void _underConnectting(void);     // 接続中
static void _underChildConnecting(void);   // 子機が親機に接続中
static void _underChildConnectInfoWait(void);  // 子機として接続中 info収集
static void _underChildConnectFlagWait(void);
static void _underChildConnectSecret(void);    // 子機として接続中
static void _underChildConnectDig(void);    // 子機として接続中
static void _underChildConnect(void);    // 子機として接続中
static void _underChildConnect_JumpFieldStart(void);    // 子機として接続中 Jump開始
static void _underChildConnect_JumpField(void);    // 子機として接続中 Jump中
static void _underChildConnect_JumpFieldEnd(void);    // 子機として接続中 Jumpおわり
static void _underParentRestart(void);
#if (T1645_060815_FIX|T1644_060816_FIX)
static void _underParentRestartAloneWait(void);
#endif //(T1645_060815_FIX|T1644_060816_FIX)
static void _underChildRestart(void);
static void _underChildRestart_Base(void);
static void _underChildStalth(void);

static void _underParentWait(void);      // 親機として待機状態
static void _underParentConnect(void);   // 親機として接続中
static void _underParentConnect_JumpFieldStart(void);  // 親機として接続中 Jump開始
static void _underParentConnect_JumpField(void);  // 親機として接続中 Jump中
static void _underParentConnect_JumpFieldEnd(void);  // 親機として接続中 Jump終わり
static void _stateUnderGroundConnectEnd(void);  // 地下切断

static void _underSoloConnect_JumpFieldStart(void);    // solo機として接続中 Jump開始
static void _underSoloConnect_JumpField(void);    // solo機として接続中 Jump中
static void _underSoloConnect_JumpFieldEnd(void);    // solo機として接続中 Jumpおわり
static void _underStalthConnect_JumpFieldStart(void);    // stalth機として接続中 Jump開始
static void _underStalthConnect_JumpField(void);    // stalth機として接続中 Jump中
static void _underStalthConnect_JumpFieldEnd(void);    // stalth機として接続中 Jumpおわり

static void _underChildStalthDeadLoop(void);



// その他一般的なステート
static void _stateNone(void);            // 何もしない
static void _stateConnectError(void);    // 接続エラー状態
static void _stateEnd(void);             // 終了処理
static void _stateConnectEnd(void);      // 切断処理開始
static void _underChildRebootFunc(void);

#ifdef PM_DEBUG		// Debug ROM
static void _debugTimerStart(void);   // デバッグ用
static void _debugTimerDisp(char* msg);
#else
static void _debugTimerStart(void){}   // デバッグ用
static void _debugTimerDisp(char* msg){}
#endif


//==============================================================================
/**
 * 地下にはいった時の通信処理
 * @param   pFSys FIELDSYS_WORK
 * @retval  none
 */
//==============================================================================

void CommFieldStateEnterUnderGround(FIELDSYS_WORK* pFSys)
{
    _COMM_FSTATE_WORK *pCFS = _getCommFieldStateWork();
    
    if(pCFS != NULL){ // つながっている場合今は除外する
        return;
    }
    // 接続実行
    CommStateEnterUnderGround(GameSystem_GetSaveData(pFSys));
    _commStateInitialize(pFSys);
    pCFS = _getCommFieldStateWork();
    sys_CreateHeap( HEAPID_BASE_APP, HEAPID_UNDERGROUND, 0xe800 );  //グッズメニューでかなり確保
    if(!SysFlag_UgFirstCheck(SaveData_GetEventWork(pCFS->pFSys->savedata))){
        OS_TPrintf("初回地下イベント\n");
        CommStateSetFirstEvent();
    }

}

//==============================================================================
/**
 * 地下にはいって、フィールドが地下に変わった時の処理
 * @param   pFSys FIELDSYS_WORK
 * @retval  none
 */
//==============================================================================

void CommFieldStateArrivalUnderGround(void)
{
    _COMM_FSTATE_WORK *pCFS = _getCommFieldStateWork();

    pCFS->bUGOverlay = TRUE;
    RECORD_Score_Add(SaveData_GetRecord(pCFS->pFSys->savedata), SCORE_ID_MINE_IN);

    CommUnderOptionInitialize(pCFS->pFSys);  // 地下サービスの起動

#ifdef PM_DEBUG
    if(DebugOhnoGetCommDebugDisp()){
        DebugIchiCheckOn(pCFS->pFSys);
    }
#endif
    
    _CHANGE_STATE(_underStart, 0);
}

//==============================================================================
/**
 * 地下を出る場合の通信処理
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommFieldStateExitUnderGround(void)
{
    _COMM_FSTATE_WORK *pCFS = _getCommFieldStateWork();
    if(pCFS==NULL){  // すでに終了している
        return;
    }
    // 切断ステートに移行する  すぐに切れない
    
    _CHANGE_STATE(_stateUnderGroundConnectEnd, 3);
}

//==============================================================================
/**
 * 地下を通信停止にする場合の通信処理
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommFieldStateStalthUnderGround(void)
{
//    OHNO_PRINT("stalth \n");
//    CommUnderOptionFinalize();  // ここをはずす@@OO
    CommStateUnderGroundOfflineSecretBase();
    _CHANGE_STATE(_underChildStalth, 0);
}

//==============================================================================
/**
 * 地下を再接続する場合の処理
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underStalthEndCheck(void)
{
    if(!CommMPIsConnectStalth()){
        CommSecretBaseInfoJumpEnd();
        _CHANGE_STATE(_underStart,0);
    }

}


void CommFieldStateUnderGroundReConnect(void)
{
//    CommSecretBaseInfoChildSendStart();
    //_underChildConnect_JumpFieldEnd();
    CommStateUnderGroundOnlineSecretBase();  // 通信状態をONLINEにもどす

    _CHANGE_STATE(_underStalthEndCheck,0);
//    _CHANGE_STATE(_underStart,1)
    
//    CommTrapInfoChildSendStart();  //Trapを自分自身に送信開始
//    _CHANGE_STATE(_underConnectting, _CHILD_P_SEARCH_TIME*2);
}

//==============================================================================
/**
 * 地下をJUMPする場合の通信処理
 * @param   none
 * @retval  none
 */
//==============================================================================

BOOL CommStateJumpUnderGround(void)
{
    _COMM_FSTATE_WORK *pCFS = _getCommFieldStateWork();
    int i;
    u32 stateAddr = (u32)pCFS->state;
    u32 parentAddr = (u32)_underParentConnect;
    u32 childAddr = (u32)_underChildConnect;
    u32 soloAddr = (u32)_underConnectting;
    u32 stalthAddr = (u32)_underChildStalthDeadLoop;
    u32 jumpState[] = {
        (u32)_underParentConnect_JumpFieldStart,
        (u32)_underChildConnect_JumpFieldStart,
        (u32)_underSoloConnect_JumpFieldStart,
        (u32)_underStalthConnect_JumpFieldStart,
    };


    
    if(stateAddr == parentAddr){
        _CHANGE_STATE(_underParentConnect_JumpFieldStart, 0);
        return TRUE;
    }
    else if(childAddr == stateAddr){
        _CHANGE_STATE(_underChildConnect_JumpFieldStart, 0);
        return TRUE;
    }
    else if(soloAddr == stateAddr){
        _CHANGE_STATE(_underSoloConnect_JumpFieldStart, 0);
        return TRUE;
    }
    else if(stalthAddr == stateAddr){
        _CHANGE_STATE(_underStalthConnect_JumpFieldStart, 0);
        return TRUE;
    }
    for(i = 0 ; i < NELEMS(jumpState); i++){
        if(jumpState[i] == stateAddr){
            return TRUE;
        }
    }
    return FALSE;
}

//==============================================================================
/**
 * 地下をJUMPし終わった場合の処理 ステートがActive状態になる
 * @param   none
 * @retval  none
 */
//==============================================================================

BOOL CommStateJumpEndUnderGround(void)
{
    _COMM_FSTATE_WORK *pCFS = _getCommFieldStateWork();
    u32 stateAddr = (u32)pCFS->state;
    u32 parentAddr = (u32)_underParentConnect_JumpField;
    u32 childAddr = (u32)_underChildConnect_JumpField;
    u32 soloAddr = (u32)_underSoloConnect_JumpField;
    u32 stalthAddr = (u32)_underStalthConnect_JumpField;

    if(stateAddr == parentAddr){
        _CHANGE_STATE(_underParentConnect_JumpFieldEnd, 0);
        return TRUE;
    }
    else if(childAddr == stateAddr){
        _CHANGE_STATE(_underChildConnect_JumpFieldEnd, 0);
        return TRUE;
    }
    else if(soloAddr == stateAddr){
        _CHANGE_STATE(_underSoloConnect_JumpFieldEnd, 0);
        return TRUE;
    }
    else if(stalthAddr == stateAddr){
        _CHANGE_STATE(_underStalthConnect_JumpFieldEnd, 0);
        return TRUE;
    }
    {
//        GF_ASSERT(0 && "想定外のstate:要修正");
    }
    return FALSE;
}

//==============================================================================
/**
 * 移動可能ステートなのかどうか返す
 * @param   none
 * @retval  移動可能ならTRUE
 */
//==============================================================================

BOOL CommIsUnderGroundMoveState(void)
{
    _COMM_FSTATE_WORK *pCFS = _getCommFieldStateWork();
    int i;
    u32 funcTbl[]={
//        (u32)_battleMoveRoom,
        (u32)_underChildConnect,
        (u32)_underParentConnect,
        (u32)_underChildConnectInfoWait,
        (u32)_underChildConnectFlagWait,
        (u32)_underChildConnectSecret,
        (u32)_underChildConnectDig,
        (u32)_underChildConnect_JumpFieldStart,
        (u32)_underChildConnect_JumpField,
        (u32)_underChildConnect_JumpFieldEnd,
        (u32)_underParentConnect_JumpFieldStart,
        (u32)_underParentConnect_JumpField,
        (u32)_underParentConnect_JumpFieldEnd,
        0,
    };
    u32 stateAddr = (u32)pCFS->state;

    if(pCFS==NULL){  // すでに終了している
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
 * 通信管理ステートの処理
 * @param
 * @retval  none
 */
//==============================================================================

static void _commCheckFunc(TCB_PTR tcb, void* work)
{
    _COMM_FSTATE_WORK *pCFS = _getCommFieldStateWork();
#ifdef PM_DEBUG
    if(pCFS){
        pCFS->debugTimer++;
    }
#endif

    if(pCFS==NULL){
        TCB_Delete(tcb);
    }
    else{
        if(pCFS->state != NULL){
            PTRStateFunc state = pCFS->state;
            if(!pCFS->bStateNoChange){
                state();
            }
        }
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
    _COMM_FSTATE_WORK *pCFS = _getCommFieldStateWork();
    pCFS->state = state;
    pCFS->timer = time;
}

#ifdef PM_DEBUG
static void _changeStateDebug(PTRStateFunc state, int time, int line)
{
    OHNO_PRINT("CF field_state: %d %d\n",line, CommGetCurrentID());
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
    _COMM_FSTATE_WORK *pCFS = _getCommFieldStateWork();
    if(pCFS->timer!=0){
        pCFS->timer--;
        return;
    }
    if(!CommIsInitialize()){
        return;  //
    }
//    if(!CommGetAloneMode()){   // aloneモードの場合
//        return;
//    }    
    CommUnderOptionReInit(pCFS->pFSys);  // 地下サービス再度初期化

    // 自分自身にいったん送る
    CommInfoSendPokeData();
    CommPlayerSendPos(FALSE);
    CommTrapInfoChildSendStart();  //Trapを自分自身に送信開始
    CommSecretBaseInfoChildSendStart(); // 自分の秘密基地データ自分の親側に送信開始

    if(!SysFlag_UgFirstCheck(SaveData_GetEventWork(pCFS->pFSys->savedata))){
        // ひょうたようステート
        _CHANGE_STATE(_underOpenning, 0);
    }
    else{
        CommPlayerFirstMoveEnable();
        _CHANGE_STATE(_underConnecttingAloneSend, _CHILD_P_SEARCH_TIME*2);
    }
}

//==============================================================================
/**
 * openiingが終わるまでなにもしない  つながらないようにする為
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underOpenning(void)
{
    _COMM_FSTATE_WORK *pCFS = _getCommFieldStateWork();
    if(!SysFlag_UgFirstCheck(SaveData_GetEventWork(pCFS->pFSys->savedata))){
        return;
    }
    CommStateUnderRestart();
    CommPlayerFirstMoveEnable();
    
    _CHANGE_STATE(_underConnectting, _CHILD_P_SEARCH_TIME*2);
}



//==============================================================================
/**
 * 自分で自分にデータを送信し終わるまで待つ
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underConnecttingAloneSend(void)
{
    _COMM_FSTATE_WORK *pCFS = _getCommFieldStateWork();
    if(pCFS->timer!=0){
        pCFS->timer--;
        return;
    }
    _CHANGE_STATE(_underConnectting, _CHILD_P_SEARCH_TIME*2);
}

//==============================================================================
/**
 * ネットワークがつながるまで待機
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underConnectting(void)
{
    CommUnderProcess();

    CommPlayerFirstMoveEnable();
    
    if(CommIsUnderGroundConnectingState()){  // 接続するまで呼び続ける
        if(CommGetCurrentID() == COMM_PARENT_ID){
          //  OHNO_PRINT("親になった\n");
            CommSecretBaseInfoParentDel();

            _CHANGE_STATE(_underParentWait, 60);
        }
        else{
          //  OHNO_PRINT("子になった\n");
            _CHANGE_STATE(_underChildConnecting, 120);
        }
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
    _COMM_FSTATE_WORK *pCFS = _getCommFieldStateWork();
    if(pCFS->timer!=0){
        pCFS->timer--;
    }

    if(CommMPParentDisconnect() || CommIsError() || (pCFS->timer==0) ){
        CommStateUnderRestart();
        _CHANGE_STATE(_underChildRestart, 0);  // 子機になる前にエラーになった
    }
    else if(CommIsConnect(CommGetCurrentID())){   // 自分自身が接続していることが確認できたら
        if(pCFS->timer!=0){
            pCFS->timer--;
            if(CommGetSendRestSize() != COMM_COMMAND_SEND_SIZE_MAX){  // 空になるまで待つ タイマーつき
                return;
            }
        }
        UgMgrForceExitNowTCB();
        CommPlayerMyDataCopy( COMM_PARENT_ID );  // 親データから自分を持ってくる
        CommTrapRecvForceEnd(COMM_PARENT_ID,0,NULL,NULL);  // 罠を止める
        UgTrapForceExit(COMM_PARENT_ID,TRUE);
        CommInfoDeletePlayer(COMM_PARENT_ID);   // 自分が親だったのでそれを消す
        CommPlayerDestroy( COMM_PARENT_ID, FALSE, FALSE );
//        CommPlayerSendPos(FALSE);
        //_debugTimerDisp("子機接続----!!!-----");
  //      OHNO_PRINT("子機になった %d \n",CommGetCurrentID());
        CommPlayerFirstMoveDisable();
        _CHANGE_STATE(_underChildConnectInfoWait, 10);
        return;
    }
//    pCFS->timer--;
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
    _COMM_FSTATE_WORK *pCFS = _getCommFieldStateWork();
    CommUnderProcess();

    if(pCFS->timer!=0){
        pCFS->timer--;
    }

    if(CommIsChildsConnecting() || (pCFS->timer==0)){
        // 自分以外がつながったら親機固定
        // もしくは子機が切断してしまった場合のタイムアウトエラー的に移動
        if(UgMgrForceExitNowTCB()){
            CommPlayerHoldEnd();
        }
        _debugTimerDisp("親機接続");
//        CommInfoSendPokeData();
        CommPlayerSendPos(FALSE);
//        CommTrapInfoChildSendStart();  //Trapを自分自身に送信開始
        CommPlayerNowFlagSend();
        CommSecretBaseInfoChildSendStart(); // 自分の秘密基地データ自分の親側に送信開始
//        UgDigFossilPlayerInit();
        UgInitialDataSend();   // 化石＋エスケープ場所を送信
        _CHANGE_STATE(_underParentConnect, 0);
        return;
    }

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
    _COMM_FSTATE_WORK *pCFS = _getCommFieldStateWork();
    CommUnderProcess();

    // 全員接続が切れたら巡回に戻るがイベント中は禁止
//    if((pCFS->pFSys->event != NULL) || UgSecretBaseIsMoveTickets()){  // 地下移動イベント時にステートを変えるのは禁止
    if(pCFS->pFSys->event != NULL){  // 地下移動イベント時にステートを変えるのは禁止
        return;
    }

    if(CommIsError() || !CommMPIsChildsConnecting() || ((!CommIsConnect(CommGetCurrentID()) && !CommGetAloneMode())) ){

//        OHNO_PRINT("なぜか子機が切断する %d %d\n",CommIsError(),CommMPIsChildsConnecting());
        
        // 秘密基地から戻るウインドウが出ていると親機の移動フラグがたっているのでここでリセット
        UgSecretBaseResetPlayer(COMM_PARENT_ID); // イベントが動いてないならリセット

        CommPlayerParentFlagDataReset();
        UgSecretBaseErrorMoveTickets();
        CommSendFixData(CF_TRAP_END_FORCE);    // サーバー側も強制解除
        UgTrapForceExit(CommGetCurrentID(),TRUE);
        UgMgrForceExitNowTCB();
        CommStateUnderParentRestart();
#if (T1645_060815_FIX | T1644_060816_FIX)
        _CHANGE_STATE(_underParentRestartAloneWait, 0);
#else // (T1645_060815_FIX | T1644_060816_FIX)
        _CHANGE_STATE(_underParentRestart, 1);
#endif // (T1645_060815_FIX | T1644_060816_FIX)
    }
}

//==============================================================================
/**
 * 親機として接続中   秘密基地への移動開始
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underParentConnect_JumpFieldStart(void)
{
    //CommSecretBaseInfoReset();
    //CommTrapInfoReset();
    CommPlayerManagerReset();
    _CHANGE_STATE(_underParentConnect_JumpField, 0);
}

//==============================================================================
/**
 * 親機として接続中   秘密基地への移動中
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underParentConnect_JumpField(void)
{
    CommUnderProcess();
}

//==============================================================================
/**
 * 親機として接続中   秘密基地への移動終わり
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underParentConnect_JumpFieldEnd(void)
{
    _COMM_FSTATE_WORK *pCFS = _getCommFieldStateWork();
    CommSecretBaseInfoJumpEnd();
    //CommSecretBaseInfoReboot();
    //CommTrapInfoReboot();
    CommPlayerManagerReboot();

#ifdef PM_DEBUG
    if(DebugOhnoGetCommDebugDisp()){
        DebugIchiCheckOn(pCFS->pFSys);
    }
#endif
    _CHANGE_STATE(_underParentConnect, 0);
}

//==============================================================================
/**
 * 子機として接続完了していて　infoデータを集めているところ
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underChildConnectInfoWait(void)
{
    _COMM_FSTATE_WORK *pCFS = _getCommFieldStateWork();
    if(pCFS->timer == 9){
        CommInfoSendPokeData();  //infoデータを送る
        UgInitialDataSend();   // 化石＋エスケープ場所を送信
    }
    if(pCFS->timer == 1){
        //UgDigFossilPlayerInit();// 化石掘ってるかどうかを送る
        CommPlayerSendPosServer(FALSE);  //テスト
    }
    if(pCFS->timer!=0){
        pCFS->timer--;
        return;
    }
    if(CommInfoIsInfoCollected()){
        _debugTimerDisp("infoデータ受信完了");
        CommPlayerNowFlagSend();
        _CHANGE_STATE(_underChildConnectFlagWait,200);
        return;
    }
    _underChildConnect();
}


//==============================================================================
/**
 * 子機として接続完了していて　ハタデータを送る所
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underChildConnectFlagWait(void)
{
    _COMM_FSTATE_WORK *pCFS = _getCommFieldStateWork();
    if(CommPlayerIsNowFlagDataEnd()){
        CommTrapInfoChildSendStart();  // 罠情報送信
        _CHANGE_STATE(_underChildConnectSecret,0);
    }
    else{
        _underChildConnect();
        if(pCFS->timer!=0){
            pCFS->timer--;
        }
        else{
            // タイムアウトによる切断
        //    OHNO_PRINT("うけとってない\n");
            _underChildRebootFunc();
        }
    }
}

//==============================================================================
/**
 * 子機として接続  罠送信完了
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underChildConnectSecret(void)
{
    if(CommTrapInfoIsParentRecv()){
        _debugTimerDisp("親機罠データ届いたらしい");
        CommTrapInfoResetParentRecv();
        CommSecretBaseInfoChildSendStart(); // 子機の秘密基地データ送信開始
        _CHANGE_STATE(_underChildConnectDig,0);
        return;
    }
    _underChildConnect();
}

//==============================================================================
/**
 * 子機として接続 秘密基地送信完了
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underChildConnectDig(void)
{
    if(CommSecretBaseInfoIsPosRecv()){
        _debugTimerDisp("秘密基地の位置がきた");
        CommSecretBaseResetPosRecv();
     //   CommPlayerSetMoveControl(TRUE);
        CommPlayerSendPos(TRUE);  // キャラを出す為にもう一回送る　テスト中
        CommSendFixData(CF_PLAYER_INIT_END);  // 初期化完了を送信
        _CHANGE_STATE(_underChildConnect,0);
        return;
    }
    _underChildConnect();
}

#if (T1645_060815_FIX | T1644_060816_FIX)

//==============================================================================
/**
 * 親機として切断をする
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underParentRestartAloneWait(void)
{
    if(!CommGetAloneMode()){
//        OHNO_PRINT("s wait\n");
        return;
    }
#if T1644_060816_FIX
    CommPlayerSetMoveControl_Server(COMM_PARENT_ID,FALSE);
    _CHANGE_STATE(_underParentRestart,30);
#else //T1644_060816_FIX
    _CHANGE_STATE(_underParentRestart,10);
#endif  //T1644_060816_FIX

}

//==============================================================================
/**
 * 親機として切断をするのを待つ
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underParentRestart(void)
{
    _COMM_FSTATE_WORK *pCFS = _getCommFieldStateWork();
#if T1644_060816_FIX
    if(pCFS->timer!=0){
        pCFS->timer--;
        return;
    }
#else //T1644_060816_FIX
    if(pCFS->timer!=0){
        pCFS->timer--;
        if(!CommIsEmptyQueue()){
        //    OHNO_PRINT("not e\n");
            return;
        }
    }
#endif //T1644_060816_FIX
    if(CommGetCurrentID() == COMM_PARENT_ID){  // 親になった
      //  OHNO_PRINT("再スタート\n");
        _CHANGE_STATE(_underStart, 0);
    }
}

#else  //(T1645_060815_FIX | T1644_060816_FIX)

//==============================================================================
/**
 * 親機として切断をする
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underParentRestart(void)
{
//    if(CommIsUnderResetState()){
  //      return;
 //   }
//    if(!CommIsConnect(CommGetCurrentID()) && !CommGetAloneMode()){
    if(!CommGetAloneMode()){
     //   OHNO_PRINT("s wait\n");
        return;
    }
    if(!CommIsEmptyQueue()){
       // OHNO_PRINT("not e\n");
        return;
    }
    
    if(CommGetCurrentID() == COMM_PARENT_ID){  // 親になった
     //   OHNO_PRINT("再スタート\n");
        _CHANGE_STATE(_underStart, 0);
    }
}

#endif //(T1645_060815_FIX | T1644_060816_FIX)

//==============================================================================
/**
 * 子機として切断をする
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underChildRestart(void)
{
    _COMM_FSTATE_WORK *pCFS = _getCommFieldStateWork();
    if(pCFS->timer!=0){
        pCFS->timer--;
        return;
    }
    if(CommGetCurrentID() == COMM_PARENT_ID){  // 親になった
   //     OHNO_PRINT("再スタート\n");
        _CHANGE_STATE(_underStart, 0);
    }
}

static void _underChildRestart_Base(void)
{
    _COMM_FSTATE_WORK *pCFS = _getCommFieldStateWork();
    if(pCFS->pFSys->event == NULL ){  // 地下移動イベント終了
        CommStateUnderRestart();  // 地下の通信を親子切り替えに
        _CHANGE_STATE(_underChildRestart, 0);
    }
}



static void _underChildStalthDeadLoop(void)
{
    CommUnderProcess();
}

static void _underChildStalth(void)
{
    _COMM_FSTATE_WORK *pCFS = _getCommFieldStateWork();
    if(CommGetCurrentID() == COMM_PARENT_ID){
//        CommUnderOptionInitialize(pCFS->pFSys);  // 地下サービスの起動

        CommUnderOptionReInit(pCFS->pFSys);  // 地下サービス再度初期化

        // 自分自身にいったん送る
        CommInfoSendPokeData();
        CommPlayerSendPos(FALSE);
        CommTrapInfoChildSendStart();  //Trapを自分自身に送信開始
        CommSecretBaseInfoChildSendStart(); // 自分の秘密基地データ自分の親側に送信開始
        _CHANGE_STATE(_underChildStalthDeadLoop, 0);
    }
}

//==============================================================================
/**
 * 子機として接続完了
 * @param   none
 * @retval  none
 */
//==============================================================================



//==============================================================================
/**
 * 子機として接続完了
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underChildRebootFunc(void)
{
#if AFTER_MASTER_070202_FOSSILLOG_FIX
    UgTrapForceExit(CommGetCurrentID(),TRUE);
    UgSecretBaseRemovePlayer_Client(CommGetCurrentID(),TRUE);
    UgDigFossilDeleteLog();
    CommPlayerManagerStop();  // 移動停止
    UgMgrForceExitNowTCB();   // 登録イベントを終了させる
    CommStateUnderRestart();  // 地下の通信を親子切り替えに
    CommPlayerFlagChange();   // 自分のハタを挿げ替える
    _CHANGE_STATE(_underChildRestart, 0);//
#else
    UgTrapForceExit(CommGetCurrentID(),TRUE);
    if(UgSecretBaseRemovePlayer_Client(CommGetCurrentID(),TRUE)){
        CommPlayerManagerStop();  // 移動停止
        UgMgrForceExitNowTCB();   // 登録イベントを終了させる
        CommStateUnderRestart();  // 地下の通信を親子切り替えに
        CommPlayerFlagChange();   // 自分のハタを挿げ替える
        _CHANGE_STATE(_underChildRestart, 0);//
    }
    else{
  //      OHNO_PRINT("切断再起動 %d\n",CommGetCurrentID());
        CommPlayerManagerStop();  // 移動停止
        UgMgrForceExitNowTCB();   // 登録イベントを終了させる
        CommStateUnderRestart();  // 地下の通信を親子切り替えに
        CommPlayerFlagChange();   // 自分のハタを挿げ替える
        _CHANGE_STATE(_underChildRestart, 0);
    }
#endif
}


static void _underChildConnect(void)
{
    _COMM_FSTATE_WORK *pCFS = _getCommFieldStateWork();
    CommUnderProcess();

    CommPlayerFirstMoveEnable();
    
    // 親機がいなくなったら終了処理をして別親を探しにいく
    if(pCFS->pFSys->event != NULL ){  // 地下移動イベント時にステートを変えるのは禁止
        return;
    }
    if(CommMPParentDisconnect() || CommIsError()){
        _underChildRebootFunc();  // 再起動
    }
}

//==============================================================================
/**
 * 子機として接続中　　　JUMP処理
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underChildConnect_JumpFieldStart(void)
{
    //CommSecretBaseInfoReset();
    //CommTrapInfoReset();
    CommPlayerManagerReset();
    _CHANGE_STATE(_underChildConnect_JumpField,0);
}

//==============================================================================
/**
 * 子機として接続中　　　JUMP処理
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underChildConnect_JumpField(void)
{
    // 出来るだけ何もしない
    UgSecretBaseErrorDisconnectTickets();
}

//==============================================================================
/**
 * 子機として接続中　　　JUMP処理
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underChildConnect_JumpFieldEnd(void)
{
    _COMM_FSTATE_WORK *pCFS = _getCommFieldStateWork();
    CommSecretBaseInfoJumpEnd();
 //   CommSecretBaseInfoReboot();
//    CommTrapInfoReboot();
    CommPlayerManagerReboot();
#ifdef PM_DEBUG
    if(DebugOhnoGetCommDebugDisp()){
        DebugIchiCheckOn(pCFS->pFSys);
    }
#endif

    if(CommMPParentDisconnect() || CommIsError()){
   //     OHNO_PRINT("切断再起動 %d\n",CommGetCurrentID());
        
        CommSecretBaseInfoReInit();  // 
        CommPlayerManagerStop();  // 移動停止
        UgMgrForceExitNowTCB();   // 登録イベントを終了させる
        CommStateUnderRestart();  // 地下の通信を親子切り替えに
        CommPlayerFlagChange();   // 自分のハタを挿げ替える
        _CHANGE_STATE(_underChildRestart, 0);
    }
    else{
        _CHANGE_STATE(_underChildConnect, 0);
    }
}



//==============================================================================
/**
 * solo機として接続中　　　JUMP処理
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underSoloConnect_JumpFieldStart(void)
{
    CommPlayerManagerReset();
    _CHANGE_STATE(_underSoloConnect_JumpField,0);
}

//==============================================================================
/**
 * solo機として接続中　　　JUMP処理
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underSoloConnect_JumpField(void)
{
}

//==============================================================================
/**
 * solo機として接続中　　　JUMP処理
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underSoloConnect_JumpFieldEnd(void)
{
    _COMM_FSTATE_WORK *pCFS = _getCommFieldStateWork();
    CommSecretBaseInfoJumpEnd();
    CommPlayerManagerReboot();
#ifdef PM_DEBUG
    if(DebugOhnoGetCommDebugDisp()){
        DebugIchiCheckOn(pCFS->pFSys);
    }
#endif
    _CHANGE_STATE(_underConnectting, 0);
}

//==============================================================================
/**
 * stalth機として接続中　　　JUMP処理
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underStalthConnect_JumpFieldStart(void)
{
    CommPlayerManagerDeletePlayers();
    _CHANGE_STATE(_underStalthConnect_JumpField,0);
}

//==============================================================================
/**
 * stalth機として接続中　　　JUMP処理
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underStalthConnect_JumpField(void)
{
}

//==============================================================================
/**
 * solo機として接続中　　　JUMP処理
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underStalthConnect_JumpFieldEnd(void)
{
    _COMM_FSTATE_WORK *pCFS = _getCommFieldStateWork();
    CommSecretBaseInfoJumpEnd();
    CommPlayerManagerReboot();
#ifdef PM_DEBUG
    if(DebugOhnoGetCommDebugDisp()){
        DebugIchiCheckOn(pCFS->pFSys);
    }
#endif
    _CHANGE_STATE(_underChildStalthDeadLoop, 0);
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
    _COMM_FSTATE_WORK *pCFS = _getCommFieldStateWork();
    if(pCFS->timer != 0){
        pCFS->timer--;
        return;
    }

    CommUnderOptionFinalize();
    CommStateExitUnderGround();

    sys_DeleteHeap(HEAPID_UNDERGROUND);

    pCFS->bUGOverlay = FALSE; // 地下を止める

    _stateFinalize();   // stateはここで終了 通信は後から切れる
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
    _COMM_FSTATE_WORK *pCFS = _getCommFieldStateWork();
    if(pCFS->timer != 0){
        pCFS->timer--;
        return;
    }
    // 切断する
    CommFinalize();
    _CHANGE_STATE(_stateEnd, 0);
}

#ifdef PM_DEBUG
static void _debugTimerStart(void)
{
    _COMM_FSTATE_WORK *pCFS = _getCommFieldStateWork();
    pCFS->debugTimer=0;
}

static void _debugTimerDisp(char* msg)
{
    _COMM_FSTATE_WORK *pCFS = _getCommFieldStateWork();
    OHNO_PRINT("%s 開始から %d sync かかりました\n", msg, pCFS->debugTimer * 2);
}

#endif


