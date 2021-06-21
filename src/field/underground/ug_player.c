//=============================================================================
/**
 * @file	comm_player.c
 * @bfief	通信で相手側のプレーヤーを表示する
 *            コミュニケーションプレーヤー
 * @author	katsumi ohno
 * @date	05/07/14
 */
//=============================================================================

#include <nitro/os.h>
#include "common.h"
#include "gflib/blact.h"

#include "communication/communication.h"
#include "communication/comm_state.h"
#include "field/comm_field_state.h"
#include "field/underground/ug_manager.h"

#include "system/bmp_menu.h"
#include "field/worldmap.h"
#include "field/fieldobj.h"
#include "field/fieldobj_code.h" //AC_WALK...等 アニメーションコードがまとめてあります。
#include "field/fieldsys.h"
#include "field/fieldmap.h"
#include "field/fld_bmp.h"
#include "system/pm_str.h"
#include "system/fontproc.h"
#include "system/bmp_list.h"
#include "system/window.h"

#include "field/player.h"
#include "field/comm_player_condition.h"
#include "field/comm_direct_counter.h"
#include "system/snd_tool.h"
#include "system/wordset.h"
#include "msgdata/msg_underworld.h"
#include "msgdata/msg_uwflag.h"

#include "field/field_effect.h"
#include "field/syswork.h"

#include "field/talk_msg.h"
#include "field/fldeff_flag.h"
#include "system/clact_tool.h"
#include "field/field_clact.h"
#include "field/fld_comact.h"
#include "field/underground/ug_local.h"
#include "field/eventdata.h"
#include "field/tv_topic.h"
#include "field/sysflag.h"

#include "field/d_ohno.h"  // デバッグ用

#include <nnsys/g3d/glbstate.h>


//==============================================================================
// 定義
//==============================================================================
#include "field/comm_player_local.h"

extern CommPlayerWork* CommPlayerGetInstance(void);

//==============================================================================
/**
 * 秘密基地に穴を閉じていく場合の消去
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommPlayerManagerDeletePlayers(void)
{
    int i;
    CommPlayerWork* pCP = CommPlayerGetInstance();
    if(pCP==NULL){
        return;
    }
    for(i = 0; i < COMM_MACHINE_MAX; i++){
        CommPlayerDestroy( i, FALSE ,FALSE);
        OHNO_SP_PRINT("あなけします %d \n",i);
        CommPlayerResetHole( i );
    }
    pCP->bResetting = TRUE;
}


//--------------------------------------------------------------
/**
 * 旗を保存する
 * @param	netID   playerID
 * @retval	消したらTRUE
 */
//--------------------------------------------------------------
static void _commPlayerDeliveryFlagData(int netID)
{
    int i;
    CommPlayerWork* pCP = CommPlayerGetInstance();

    if(netID == CommGetCurrentID()){  // 記録
        if(pCP->pNowFlag[netID] != NULL){

            RECORD_Score_Add(SaveData_GetRecord(pCP->pFSys->savedata), SCORE_ID_NEW_FLAG_TAKE_OUT);

#if AFTERMASTER_070206_UNDERGROUND_PCFLAGLIST2ND_FIX
#else
            for(i = 0; i < _KEEP_FLAG_MAX; i++){
                if(pCP->pKeepFlag[i]){
                    if(MyStatus_Compare(pCP->pNowFlag[netID], pCP->pKeepFlag[i]) == TRUE){
                        // 完全一致 は今の旗を消すだけ
                        CommPlayerDeleteFlagData(netID);
                        return;
                    }
                }
            }
#endif
            if(pCP->pKeepFlag[_KEEP_FLAG_MAX-1]){
                sys_FreeMemoryEz(pCP->pKeepFlag[_KEEP_FLAG_MAX-1]);
            }
            for(i = _KEEP_FLAG_MAX - 1; i >= 1; i--){
                pCP->pKeepFlag[i] = pCP->pKeepFlag[i-1];
            }
            pCP->pKeepFlag[0] = pCP->pNowFlag[netID];
            UnderGroundAddFlag(SaveData_GetUnderGroundData(GameSystem_GetSaveData(pCP->pFSys)),
                               pCP->pNowFlag[netID]);
            pCP->pNowFlag[netID] = NULL;
            pCP->sendFlagData[netID].netID = INVALID_NETID;
        }
    }
    else{
        CommPlayerDeleteFlagData(netID);
    }
}

//--------------------------------------------------------------
/**
 * @brief   相手の頭から旗を取る
 * @param   myNetID   旗をとる人
 * @param   targetID  旗をとられる人
 * @retval  とったらTRUE
 */
//--------------------------------------------------------------

static BOOL _getHeadFlag(int myNetID, int targetNetID)
{
    CommPlayerWork* pCP = CommPlayerGetInstance();

    
    if((pCP->pNowFlag[myNetID] == NULL) &&
       (pCP->pNowFlag[targetNetID] != NULL)){
        pCP->pNowFlag[myNetID] = pCP->pNowFlag[targetNetID];
        pCP->pNowFlag[targetNetID] = NULL;
        pCP->hedFE[myNetID] = _HFE_FLAG;
        pCP->hedFE[targetNetID] = _HFE_NONE;
//        OHNO_PRINT("はたとり\n");
        pCP->sendFlagData[myNetID].netID = myNetID;
        pCP->sendFlagData[targetNetID].netID = INVALID_NETID;
        MyStatus_Copy(pCP->pNowFlag[myNetID], (MYSTATUS*)&pCP->sendFlagData[myNetID].mystatus);
        return TRUE;
    }
    return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   メッセージを自動表示した場合の終了時コールバック
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------
static void _msgEndCallBack(int num)
{
    CommPlayerHoldEnd();
}

static void _msgEndCallBackDel(int num)
{
    CommPlayerHoldEnd();
}

static void _msgEndCallBackNoTalk(int num)
{
    CommPlayerHoldBitEnd(_HOLD_FLAG_NO_TALK);
}

static void _msgEndCallBackNoDig(int num)
{
    CommPlayerHoldBitEnd(_HOLD_FLAG_NO_DIG);
}

static void _msgEndCallBackRipOff(int num)
{
    CommPlayerHoldBitEnd(_HOLD_FLAG_RIP_OFF);
}


static void _msgEndCallBackStolenMy(int num)
{
    CommPlayerHoldBitEnd(_HOLD_FLAG_STOLEN_MY);
}

//==============================================================================
/**
 * 会話リストが終わった時に呼び出してもらう
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _talkMenuEnd(int selectNo)
{
    CommPlayerWork* pCP = CommPlayerGetInstance();
    if(pCP){
        pCP->bTalkMenuDisp=FALSE;
    }
}

typedef struct{
    u8 bOpen;
    u8 netID;
    u8 targetID;
} _ugMenuOpen;

//--------------------------------------------------------------
/**
 * @brief   炭鉱フィールドメニューを開いてよいかどうか  CF_CHECK_FIELDMENU_OPEN
 * @param   コールバックパラメーター
 * @retval  none
 */
//--------------------------------------------------------------

void CommPlayerRecvCheckFieldMenuOpen(int netID, int size, void* pData, void* pWork)
{
    _ugMenuOpen menuOpen;
    CommPlayerWork* pCP = CommPlayerGetInstance();

    menuOpen.bOpen = _RESULT_NG;
    menuOpen.netID = netID;

    if(pCP->bMoveControl[ netID ] && !CommPlayerIsTrapBind(netID)){
        if(CommPlayerIsFlagData(netID)){
            menuOpen.bOpen = _RESULT_DELFLAG_OPEN;
        }
        else{
            menuOpen.bOpen = _RESULT_OK;
        }
    }
    if(CommSendData_ServerSide(CF_RESULT_FIELDMENU_OPEN, &menuOpen, 2)){
        if(menuOpen.bOpen!=_RESULT_NG){
            CommPlayerSetMoveControl_Server(netID, FALSE);            
//            _pCommPlayerWork->bMoveControl[ netID ] = FALSE;
        }
    }
}

//--------------------------------------------------------------
/**
 * @brief   STARTメニューを開いていいかどうかのコールバック CF_RESULT_FIELDMENU_OPEN
 * @param   
 * @retval  none
 */
//--------------------------------------------------------------

void CommPlayerRecvResultFieldMenuOpen(int netID, int size, void* pData, void* pWork)
{
    CommPlayerWork* pCP = CommPlayerGetInstance();
    _ugMenuOpen* pMenuOpen = pData;

    if(!CommIsSendMoveData()){
        return;
    }
    if((pMenuOpen->bOpen == _RESULT_OK) && (pMenuOpen->netID == CommGetCurrentID())){
        if(!pCP->bTalkMenuDisp){
            pCP->bTalkMenuDisp = TRUE;
            //地下専用メニューを出す
            CommUnderBagInit(_talkMenuEnd, pCP->pFSys);
        }
    }
    else if((pMenuOpen->bOpen == _RESULT_DELFLAG_OPEN) && (pMenuOpen->netID == CommGetCurrentID())){
        if(!pCP->bTalkMenuDisp){
            pCP->bTalkMenuDisp = TRUE;
            CommUnderBagFlagStart(_talkMenuEnd, pCP->pFSys);
        }
    }
}

//--------------------------------------------------------------
/**
 * @brief   問いかけメニューを開いていいかどうかのコールバック
 *          CF_RESULT_UGMENU_OPEN
 * @param   通信コールバック
 * @retval  none
 */
//--------------------------------------------------------------

void CommPlayerRecvResultUGMenuOpen(int netID, int size, void* pData, void* pWork)
{
    CommPlayerWork* pCP = CommPlayerGetInstance();
    _ugMenuOpen* pMenuOpen = pData;
    EVENTWORK* pEV = SaveData_GetEventWork( pCP->pFSys->savedata );
    GF_ASSERT_RETURN(pMenuOpen->netID < COMM_MACHINE_MAX,);

//    OHNO_PRINT("会話開始 クライアント\n");

    if((pMenuOpen->bOpen == _RESULT_FOSSIL) && (pMenuOpen->netID == CommGetCurrentID())){
        CommPlayerHold();
        CommMsgTalkWindowStart(CommUnderGetMsgUnderWorld(),
                               msg_underworld_72,TRUE,_msgEndCallBack);
    }
    if(pMenuOpen->bOpen == _RESULT_OK){    // 向かい合う方向に
        CommPlayerSetLook(pMenuOpen->netID, pMenuOpen->targetID);
    }
    if((pMenuOpen->bOpen == _RESULT_OK) && (pMenuOpen->netID == CommGetCurrentID())){
        CommUnderTalkInit(pMenuOpen->targetID , _talkMenuEnd, pCP->pFSys);
        pCP->talkMenuInfo.netID = pMenuOpen->netID ;
        pCP->talkMenuInfo.targetID = pMenuOpen->targetID;
        pCP->talkMenuInfo.bItemMenu = FALSE;
        pCP->talkMenuInfo.bPersonalInformation = FALSE;
        if(pCP->handCount[pMenuOpen->targetID] == 0){
            SecretBaseRecordSetTalkNum(SaveData_GetSecretBaseRecord(GameSystem_GetSaveData(pCP->pFSys)),pMenuOpen->targetID); // 話しかけ開始
            SysWork_UGTalkCountSet(pEV,SysWork_UGTalkCountGet(pEV)+1);
            SysWork_UGTalkCount2Set(pEV,SysWork_UGTalkCount2Get(pEV)+1);
            pCP->handCount[pMenuOpen->targetID]++;
        }
    }
    if((pMenuOpen->bOpen == _RESULT_OK) && (pMenuOpen->targetID == CommGetCurrentID())){
        CommUnderTalkBind(pMenuOpen->targetID, pMenuOpen->netID, pCP->pFSys);
    }
    if((pMenuOpen->bOpen == _RESULT_NG) && (pMenuOpen->netID == CommGetCurrentID())){
        CommPlayerHold();
        CommMsgTalkWindowStart(CommUnderGetMsgUnderWorld(),
                               msg_underworld_03,TRUE,_msgEndCallBack);
    }
    if(pMenuOpen->bOpen == _RESULT_OK){
        UgMgrSetTalkLog(pMenuOpen->netID, pMenuOpen->targetID);
    }
}

//--------------------------------------------------------------
/**
 * @brief   旗に関する状態をサーバーから返す場合のコマンドサイズ
 * @param   通信コールバック
 * @retval  none
 */
//--------------------------------------------------------------

typedef struct{
    u8 state;
    u8 myNetID;
    u8 targetID;
} _flagStateCallBack;

int CommPlayerRecvFlagStateServerRetGetSize(void)
{
    return sizeof(_flagStateCallBack);
}

//--------------------------------------------------------------
/**
 * @brief   旗に関する状態がサーバーに届いた  CF_FLAG_STATE
 * @param   通信コールバック
 * @retval  none
 */
//--------------------------------------------------------------

void CommPlayerRecvFlagState(int netID, int size, void* pData, void* pWork)
{
    u8* pState = pData;
    _flagStateCallBack ret;

    ret.myNetID = netID;
    switch(pState[0]){
      case UNDER_FLAG_DEL:
        ret.state = UNDER_FLAG_DEL;
        CommSendFixSizeData_ServerSide(CF_FLAG_STATE_RET,&ret);  // 旗を消す為にクライアントに送信
//        CommPlayerSetMoveControl_Server(netID,FALSE);
        break;
      case UNDER_FLAG_DELIVERY:
        ret.state = UNDER_FLAG_DELIVERY;
        CommSendFixSizeData_ServerSide(CF_FLAG_STATE_RET,&ret);  // 旗を消す為にクライアントに送信
        CommPlayerSetMoveControl_Server(netID,FALSE);
        break;
    }
}

//--------------------------------------------------------------
/**
 * @brief   旗に関する状態がサーバーに届いた  CF_PLAYER_FREEZE_END
 * @param   通信コールバック
 * @retval  none
 */
//--------------------------------------------------------------

//void CommPlayerRecvFreezeEnd(int netID, int size, void* pData, void* pWork)

static void _commPlayerFreeze(int num)
{
//    u8* pBuff = pData;

    CommPlayerSetFENoneAdd(CommGetCurrentID()); // ！けし
    CommMsgTalkWindowEnd(CommUnderGetMsgUnderFlag());
    CommPlayerHoldBitEnd(_HOLD_FLAG_STOLEN);
}


//--------------------------------------------------------------
/**
 * @brief   ハタを取られたので一秒以上固まる
 * @param   通信コールバック
 * @retval  none
 */
//--------------------------------------------------------------

static void _freezePlayer(TCB_PTR tcb, void* work)
{
    CommPlayerWork* pCP = CommPlayerGetInstance();
    _FREEZE_PLAYER* pFreeze = work;
    u8 id = pFreeze->targetID;
    u8 bEnd = FALSE;

    if(!CommIsConnect(id)){
        sys_FreeMemoryEz(pFreeze);
        TCB_Delete(tcb);
        return;
    }
    else{
        pFreeze->timer++;
        if(pFreeze->timer > 60){
            bEnd = TRUE;
        }
    }
    if(bEnd){
        pCP->bMoveControlFlag[ id ]=TRUE;
        sys_FreeMemoryEz(pFreeze);
        TCB_Delete(tcb);
    }
}

//--------------------------------------------------------------
/**
 * @brief   ハタを取られたので一秒以上固まる
 * @param   通信コールバック
 * @retval  none
 */
//--------------------------------------------------------------

static void _freezeConditionStart(int targetID)
{
    CommPlayerWork* pCP = CommPlayerGetInstance();
    _FREEZE_PLAYER* pFreeze = sys_AllocMemory(HEAPID_WORLD, sizeof(_FREEZE_PLAYER));
    pFreeze->timer = 0;
    pFreeze->targetID = targetID;
    pCP->bMoveControlFlag[ targetID ] = FALSE;
    TCB_Add(_freezePlayer, pFreeze, TCB_PRIORITY_NORMAL);
    CommPlayerSetMoveControl_Server(targetID,FALSE);
    
}

//--------------------------------------------------------------
/**
 * @brief   旗を持っている人との検査
 * @param   通信コールバック
 * @retval  none
 */
//--------------------------------------------------------------

BOOL CommPlayerFlagTalkCheck(int netID, int targetID, BOOL bNPCTalk)
{
    _flagStateCallBack ret;

    if(bNPCTalk & 0x01){
        ret.state = UNDER_FLAG_NOTALK;
    }
    else if(CommPlayerIsFlagData(netID)){ // 自分が旗を持っている場合はこれ
        ret.state = UNDER_FLAG_NOTALK;
    }
    else{
        if(targetID == COMM_INVALID_ID){
            return FALSE;
        }
        else if(!CommPlayerIsFlagData(targetID)){ // 相手が旗を持ってない
            return FALSE;
        }
        else{
            ret.state = UNDER_FLAG_RIP_OFF;
            _freezeConditionStart(targetID);
        }
    }
    ret.myNetID = netID;
    ret.targetID = targetID;
    CommPlayerSetMoveControl_Server(netID,FALSE);
    CommSendFixSizeData_ServerSide(CF_FLAG_STATE_RET,&ret);  // クライアントに送信
    return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ほれるかどうか  旗持ってたらほれない
 * @param   id     人
 * @retval  通信を返したらTRUE
 */
//--------------------------------------------------------------

BOOL CommPlayerFlagDigCheck(int netID)
{
    _flagStateCallBack ret;

    if(CommPlayerIsFlagData(netID)){ // 自分が旗を持っている場合はこれ
        ret.state = UNDER_FLAG_NODIG;
    }
    else{
        return FALSE;
    }
    ret.myNetID = netID;
    CommSendFixSizeData_ServerSide(CF_FLAG_STATE_RET,&ret);  // クライアントに送信
    return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ハタを盗まれた人の処理  
 * @param   通信コールバック
 * @retval  none
 */
//--------------------------------------------------------------

static void _stolenFlagPlayerFunc(_flagStateCallBack* pRet, BOOL bGet, SB_RECORD_DATA* pSBD)
{
    CommPlayerWork* pCP = CommPlayerGetInstance();

    UgMgrForceExitNowTCB();
#if T1656_060815_FIX
    CommPlayerSetMoveControl(FALSE);  // サーバーに送信
#endif  //T1656_060815_FIX
    CommPlayerHoldBit(_HOLD_FLAG_STOLEN);
    CommMsgRegisterTargetName(CommUnderGetMsgUnderFlag(),CommInfoGetMyStatus(pRet->myNetID));
    if(bGet){
        pCP->hedFE[CommGetCurrentID()] = _HFE_NONE;
        CommPlayerSetFENoneAdd(CommGetCurrentID()); // 旗消し
        CommPlayerSetFEExclamationAdd(CommGetCurrentID()); //!
        CommMsgTalkWindowStart(CommUnderGetMsgUnderFlag(),  // 奪われた
                               mes_uw_flag_15, TRUE, _commPlayerFreeze);
    }
    else{
        CommMsgTalkWindowStart(CommUnderGetMsgUnderFlag(),  // 奪われた
                               mes_uw_flag_16, TRUE, _commPlayerFreeze);
    }
    Snd_FadeOutNextPlaySet(SND_SCENE_FIELD,SEQ_TANKOU,BGM_FADE_TANKOU_TIME,0,0xff,NULL);
    Snd_SePlay(UG_SE_FLAG_STEAL);
}

//--------------------------------------------------------------
/**
 * @brief   旗に関する状態がクライアントに届いた  CF_FLAG_STATE_RET
 * @param   通信コールバック
 * @retval  none
 */
//--------------------------------------------------------------

void CommPlayerRecvFlagStateServerRet(int netID, int size, void* pData, void* pWork)
{
    CommPlayerWork* pCP = CommPlayerGetInstance();
    _flagStateCallBack* pRet = pData;
    BOOL bDeliv;
    int point = 0;
    SB_RECORD_DATA* pSBD = SaveData_GetSecretBaseRecord(GameSystem_GetSaveData(pCP->pFSys));

    switch(pRet->state){
      case UNDER_FLAG_DEL:
        if(pCP->pNowFlag[pRet->myNetID]){ // 自分のハタがある場合
            CommPlayerDeleteFlagData(pRet->myNetID);
            CommPlayerSetFENoneAdd(pRet->myNetID);
            if(pRet->myNetID == CommGetCurrentID()){
                CommPlayerHold();
                CommMsgTalkWindowStart(CommUnderGetMsgUnderFlag(),
                                       mes_uw_flag_07, TRUE, _msgEndCallBackDel);
                Snd_FadeOutNextPlaySet(SND_SCENE_FIELD,SEQ_TANKOU,BGM_FADE_TANKOU_TIME,0,0xff,NULL);
            }
        }
//        OHNO_PRINT("はた捨てた\n");
        break;
      case UNDER_FLAG_RIP_OFF:  // 奪い合い
        // クライアント管理なので自分が持ってたら取れたことにする
        if(CommInfoGetMyStatus(pRet->myNetID)==NULL){
            //奪った人がいない場合  相手のハタだけ消して完了
            CommPlayerDeleteFlagData(pRet->targetID);
//            OHNO_PRINT("奪い合いけしておわり\n");
            return;
        }
        if(pCP->pNowFlag[pRet->targetID]==NULL){ // 相手のハタが無い場合
            // 終了
            if(pRet->myNetID==CommGetCurrentID()){
                CommPlayerHoldEnd();
            }
//            OHNO_PRINT("はたない\n");
            return;
        }
        if(MyStatus_Compare(pCP->pNowFlag[pRet->targetID],
                            CommInfoGetMyStatus(pRet->myNetID)) == TRUE){  // 実は自分の旗だった
            // 相手の旗を消す
            CommPlayerDeleteFlagData(pRet->targetID);
            if(pRet->myNetID == CommGetCurrentID()){
                SecretBaseRecordSetFlagReverseNum(pSBD);   // 奪った旗を取り戻した
                CommPlayerHold();
                CommMsgTalkWindowStart(CommUnderGetMsgUnderFlag(),  // とりもどした
                                              mes_uw_flag_18, TRUE, _msgEndCallBack);
                Snd_SePlay(UG_SE_FLAG_CONQUER);
            }
            else if(pRet->targetID == CommGetCurrentID()){
                _stolenFlagPlayerFunc(pRet, FALSE, pSBD); // 取られた側
            }
            UgSecretBaseSetFlagReverseLog(pRet->myNetID);//とりもどした
        }
        else if(_getHeadFlag(pRet->myNetID,pRet->targetID)){  // ハタを取れた
            if(pRet->myNetID == CommGetCurrentID()){
                SecretBaseRecordSetFlagConquerNum(pSBD);  // 奪った
                CommPlayerHoldBit(_HOLD_FLAG_RIP_OFF);
                CommMsgRegisterTargetName(CommUnderGetMsgUnderFlag(),CommInfoGetMyStatus(pRet->targetID));
                CommMsgTalkWindowStart(CommUnderGetMsgUnderFlag(),  // 奪った
                                              mes_uw_flag_17, TRUE, _msgEndCallBackRipOff);
                Snd_BgmPlay( SEQ_HATANIGE );
                Snd_SePlay(UG_SE_FLAG_CONQUER);
            }
            else if(pRet->targetID == CommGetCurrentID()){
                _stolenFlagPlayerFunc(pRet, TRUE, pSBD); // 取られた側
            }
//            OHNO_PRINT("奪った\n");
            UgSecretBaseSetFlagConquerLog(pRet->myNetID, pRet->targetID);//うばった
        }
        break;
      case UNDER_FLAG_NOTALK:
        if(pRet->myNetID == CommGetCurrentID()){
            CommPlayerHoldBit(_HOLD_FLAG_NO_TALK);
            CommMsgTalkWindowStart(CommUnderGetMsgUnderFlag(),
                                   mes_uw_flag_11, TRUE, _msgEndCallBackNoTalk);
            //UgMgrSetNowTCB();//
        }
        break;
      case UNDER_FLAG_NODIG:
        if(pRet->myNetID == CommGetCurrentID()){
            CommPlayerHoldBit(_HOLD_FLAG_NO_DIG);
            CommMsgTalkWindowStart(CommUnderGetMsgUnderFlag(),
                                   mes_uw_flag_10, TRUE, _msgEndCallBackNoDig);
        }
        break;
      case UNDER_FLAG_DELIVERY:
        pCP->hedFE[pRet->myNetID] = _HFE_NONE;
        CommPlayerSetFENoneAdd(pRet->myNetID); // 旗消し

        if(pRet->myNetID == CommGetCurrentID()){  // 表示
            SB_RECORD_DATA* pSBD = SaveData_GetSecretBaseRecord(GameSystem_GetSaveData(pCP->pFSys));
            u8 type = SecretBaseRecordGetFlagType(pSBD);
            SecretBaseRecordSetFlagDeliveryNum(pSBD);  // 運んだ
            SysFlag_UgHataSet(SaveData_GetEventWork(pCP->pFSys->savedata)); // 運んだ
            if(pCP->pNowFlag[pRet->myNetID]){            // ハタを運ぶたびに呼ばれる
                TVTOPIC_Entry_Watch_GetFlag(pCP->pFSys,pCP->pNowFlag[pRet->myNetID]);
                if(pCP->myDeliveryFlagNum!=0xffff){
                    pCP->myDeliveryFlagNum++;
                }
            }
            CommPlayerHold();
            Snd_SePlay(UG_SE_FLAG_DELIVERY);
            if(type == SecretBaseRecordGetFlagType(pSBD)){  // ハタ同じだった
                CommMsgTalkWindowStart(CommUnderGetMsgUnderFlag(),
                                       mes_uw_flag_12, TRUE, _msgEndCallBack);
            }
            else{
                CommSendFixSizeData(CF_LVUP_FLAG, &type);  // LVUP演出
            }
            Snd_FadeOutNextPlaySet(SND_SCENE_FIELD,SEQ_TANKOU,BGM_FADE_TANKOU_TIME,0,0xff,NULL);
        }
        if(pCP->pNowFlag[pRet->myNetID]){
            if(MyStatus_Compare(pCP->pNowFlag[pRet->myNetID],
                                CommInfoGetMyStatus(CommGetCurrentID())) == TRUE){  // 実は自分の旗だった
                TVTOPIC_Entry_Watch_LoseFlag(pCP->pFSys,CommInfoGetMyStatus(pRet->myNetID));
            }
        }
        _commPlayerDeliveryFlagData(pRet->myNetID);  // 記録
        break;
    }
}

//--------------------------------------------------------------
/**
 * @brief   親機側のハタプールデータを消す
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------

void CommPlayerParentFlagDataReset(void)
{
    int i;
    CommPlayerWork* pCP = CommPlayerGetInstance();
    
    for(i = 0;i < COMM_MACHINE_MAX;i++){
        pCP->sendFlagData[i].netID = INVALID_NETID;
    }
}

//--------------------------------------------------------------
/**
 * @brief   ハタを受け取るバッファ
 * @param   xpos  場所
 * @param   zpos
 * @retval  playerのnetID
 */
//--------------------------------------------------------------

u8* CommPlayerGetFlagRecvBuff( int netID, void* pWork, int size)
{
    CommPlayerWork* pCP = CommPlayerGetInstance();
    return (u8*)&pCP->sendFlagData[COMM_MACHINE_MAX];
}

//--------------------------------------------------------------
/**
 * @brief   現状のハタデータ送信
 * @param   xpos  場所
 * @param   zpos
 * @retval  playerのnetID
 */
//--------------------------------------------------------------

void CommPlayerNowFlagSend(void)
{
    CommPlayerWork* pCP = CommPlayerGetInstance();

    pCP->bFlagDataRecv = FALSE;
    if(pCP->pNowFlag[CommGetCurrentID()]){
        CommSendFixSizeData(CF_FLAG_NOWARRAY_REQ, pCP->pNowFlag[CommGetCurrentID()]); //ハタ情報更新
    }
    else{
        MYSTATUS* pMy = MyStatus_AllocWork(HEAPID_COMMUNICATION);
        STRBUF* pBuf = STRBUF_Create(20,HEAPID_COMMUNICATION);
        GF_ASSERT(pMy);
        GF_ASSERT(pBuf);
        MyStatus_SetMyNameFromString(pMy,pBuf);
        CommSendFixSizeData(CF_FLAG_NOWARRAY_REQ, pMy); //ハタ情報更新
        STRBUF_Delete(pBuf);
        sys_FreeMemoryEz(pMy);
    }
}

//--------------------------------------------------------------
/**
 * @brief   現状のハタデータサイズをえる
 * @param   xpos  場所
 * @param   zpos
 * @retval  playerのnetID
 */
//--------------------------------------------------------------

int CommPlayerNowFlagSize(void)
{
    return COMM_SEND_MYSTATUS_SIZE;
}

//--------------------------------------------------------------
/**
 * @brief   現状のハタデータを全員に送る
 * @param   xpos  場所
 * @param   zpos
 * @retval  playerのnetID
 */
//--------------------------------------------------------------

void CommPlayerRecvNowFlagDataReq(int netID, int size, void* pData, void* pWork)
{
    CommPlayerWork* pCP = CommPlayerGetInstance();
    int i;
    const MYSTATUS* pMyStatus = pData;
    STRBUF* pStrBuf;
    u8 id = netID;

//    OHNO_PRINT("size %d\n",size);
    

    if(pCP){
        MyStatus_Copy(pMyStatus, (MYSTATUS*)pCP->sendFlagData[netID].mystatus);

        pStrBuf = MyStatus_CreateNameString(pMyStatus, HEAPID_COMMUNICATION);
        if(STRBUF_GetLen(pStrBuf)!=0){  // 名前があれば正しいデータ
            pCP->sendFlagData[netID].netID = netID;
        }
        for(i = 0; i < COMM_MACHINE_MAX;i++){
//            OHNO_PRINT("旗検査 %d\n", pCP->sendFlagData[i].netID);
            if(pCP->sendFlagData[i].netID != INVALID_NETID){


                pCP->sendFlagData[i].netID = i;
                CommSendHugeData_ServerSide(CF_FLAG_NOWARRAY,
                                            &pCP->sendFlagData[i],
                                            sizeof(_SEND_FLAG));
            }
        }
        CommSendData_ServerSide(CF_FLAG_NOWARRAY_END, &id,1);
        STRBUF_Delete(pStrBuf);
        pCP->bDataSendFlag = TRUE;
    }
    else{
        GF_ASSERT(0);
    }
}


//--------------------------------------------------------------
/**
 * @brief   現状のハタデータが到着
 * @param   xpos  場所
 * @param   zpos
 * @retval  playerのnetID
 */
//--------------------------------------------------------------

void CommPlayerRecvNowFlagDataArray(int netID, int size, void* pData, void* pWork)
{
    CommPlayerWork* pCP = CommPlayerGetInstance();
    _SEND_FLAG* pBuff = pData;
    int i,id;

    if(pCP){
        id = pBuff->netID;
        if(pCP->pNowFlag[id]){
            sys_FreeMemoryEz(pCP->pNowFlag[id]);
        }
        pCP->pNowFlag[id] = MyStatus_AllocWork(HEAPID_COMMUNICATION);
        MyStatus_Copy((MYSTATUS*)pBuff->mystatus, pCP->pNowFlag[id]);

        pCP->hedFE[id] = _HFE_FLAG;
    }
}

//--------------------------------------------------------------
/**
 * @brief   現状のハタデータサイズ
 * @param   xpos  場所
 * @param   zpos
 * @retval  playerのnetID
 */
//--------------------------------------------------------------

int CommPlayerNowFlagDataArraySize(void)
{
    return sizeof(_SEND_FLAG);
}

//--------------------------------------------------------------
/**
 * @brief   旗データ受信コールバック
 */
//--------------------------------------------------------------

void CommPlayerRecvNowFlagDataEnd(int netID, int size, void* pData, void* pWork)
{
    u8* pBuff = pData;
    CommPlayerWork* pCP = CommPlayerGetInstance();

//    OHNO_PRINT("CommPlayerRecvNowFlagDataEndコマンド受信 %d\n",pBuff[0]);
    if(pBuff[0] == CommGetCurrentID()){
        pCP->bFlagDataRecv = TRUE;
    }
    pCP->bDataSendFlag = FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   旗データを受信したかどうか
 * @retval  TRUEなら受信した
 */
//--------------------------------------------------------------

BOOL CommPlayerIsNowFlagDataEnd(void)
{
    CommPlayerWork* pCP = CommPlayerGetInstance();
    return pCP->bFlagDataRecv;
}


//--------------------------------------------------------------
/**
 * @brief   子機として持っていた旗を親機（ソロ）に繋ぐ
 *          自分の以外は抹消
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------

void CommPlayerFlagChange(void)
{
    MYSTATUS* pMy = NULL;
    int i,j;
    int id = CommGetCurrentID();
    CommPlayerWork* pCP = CommPlayerGetInstance();

    // 旗を持ってたら置いておく
    if(pCP->pNowFlag[id]){
//        OHNO_PRINT("自分が旗を持っている\n");
        pMy = pCP->pNowFlag[id];
        pCP->pNowFlag[id] = NULL;
        pCP->hedFE[id] = _HFE_NONE;
        pCP->sendFlagData[id].netID = INVALID_NETID;
    }
    // 全部消す
    for(i = 0; i < COMM_MACHINE_MAX; i++){
        CommPlayerDeleteFlagData(i);
    }
    if(pMy){
//        OHNO_PRINT("切断　ハタ譲渡\n");
        pCP->pNowFlag[COMM_PARENT_ID] = pMy;
        pCP->hedFE[COMM_PARENT_ID] = _HFE_FLAG;
        MyStatus_Copy(pMy, (MYSTATUS*)&pCP->sendFlagData[COMM_PARENT_ID].mystatus);
        pCP->sendFlagData[COMM_PARENT_ID].netID = COMM_PARENT_ID;
    }
}

//==============================================================================
/**
 * 罠にかかってるかどうか
 * @param   targetID
 * @retval  かかっていたらTRUE
 */
//==============================================================================

BOOL CommPlayerIsTrapBind(int targetID)
{
    CommPlayerWork* pCP = CommPlayerGetInstance();

    if(pCP->pCondMgr){
        return !CommPlayerIsNormalCondition_ServerSide(
            CommPlayerCondGetWork(pCP->pCondMgr, targetID));
    }
    return FALSE;
}



//--------------------------------------------------------------
/**
 * @brief   子機からNPCに話しかけた際に飛んできます
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------
void CommPlayerRecvOtherTalk(int netID, int size, void* pData, void* pWork)
{
    u8 id = netID;
    
//    CommPlayerSetMoveControl_Server(netID,TRUE);
    CommSendFixSizeData_ServerSide(CF_OTHER_TALK_SERVER_SIDE,&id);
}

//--------------------------------------------------------------
/**
 * @brief   親機からNPCに話しかけた子機がいた際に飛んできます CF_OTHER_TALK_SERVER_SIDE
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------
void CommPlayerRecvOtherTalk_ServerSide(int netID, int size, void* pData, void* pWork)
{
    u8* pBuff = pData;

    UgMgrSetTalkOtherLog(pBuff[0]);  //LOG
}

//--------------------------------------------------------------
/**
 * @brief   プレーヤーキャラのXの位置を返す  かせき掘り専用
 * @param   netID ネットワークでのID
 * @retval  Xの場所
 */
//--------------------------------------------------------------

int CommPlayerGetPosDigFossilX(int netID)
{
    CommPlayerWork* pCP = CommPlayerGetInstance();

    if(!pCP){
        return COMM_PLAYER_INVALID_GRID;
    }
    else if(CommGetCurrentID() == netID){
        return pCP->sPlayerPlace[netID].xpos;
    }
    else if(!CommPlayerIsControl()){
        return COMM_PLAYER_INVALID_GRID;
    }
    return pCP->sPlayerPlace[netID].xpos;
}

//--------------------------------------------------------------
/**
 * @brief   プレーヤーキャラのZの位置を返す  かせき掘り専用
 * @param   netID ネットワークでのID
 * @retval  Zの場所
 */
//--------------------------------------------------------------

int CommPlayerGetPosDigFossilZ(int netID)
{
    CommPlayerWork* pCP = CommPlayerGetInstance();

    if(!pCP){
        return COMM_PLAYER_INVALID_GRID;
    }
    else if(CommGetCurrentID() == netID){
        return pCP->sPlayerPlace[netID].zpos;
    }
    else if(!CommPlayerIsControl()){
        return COMM_PLAYER_INVALID_GRID;
    }
    return pCP->sPlayerPlace[netID].zpos;
}

//--------------------------------------------------------------
/**
 * @brief   罠にかかった時のプレーヤーの状態をセットすいる
 * @param   victimNetID  罠にかかった人
 * @param   trapType     罠タイプ
 * @retval  none
 */
//--------------------------------------------------------------

void CommPlayerSetCondition(int victimNetID, int condition)
{
    CommPlayerWork* pCP = CommPlayerGetInstance();
//    OHNO_PRINT("setコンディション %d \n", victimNetID);
    CommPlayerCondSetCondition(
        CommPlayerCondGetWork(pCP->pCondMgr, victimNetID),
        condition);
}

//--------------------------------------------------------------
/**
 * @brief   罠にかかった時のプレーヤーの状態を消す
 * @param   victimNetID  罠にかかった人
 * @param   trapType     罠タイプ
 * @retval  none
 */
//--------------------------------------------------------------

void CommPlayerResetCondition(int victimNetID)
{
    CommPlayerWork* pCP = CommPlayerGetInstance();
//    OHNO_PRINT("リセットコンディション %d \n", victimNetID);
    CommPlayerCondSetCondition(
        CommPlayerCondGetWork(pCP->pCondMgr, victimNetID),
        PLAYER_COND_NONE);
}

//--------------------------------------------------------------
/**
 * @brief   場所移動する
 * @param   netID
 * @param   x
 * @param   y
 * @retval  none
 */
//--------------------------------------------------------------

void CommPlayerBaseTeleportServer(int netID, int x, int z, int dir)
{
    CommPlayerWork* pCP = CommPlayerGetInstance();
    _PlayerPlace* pPP = &pCP->sPlayerPlaceServer[netID];
    pPP->xpos = x + FieldOBJ_DirAddValueGX(dir);
    pPP->zpos = z + FieldOBJ_DirAddValueGZ(dir);
    pPP->dir = dir;
 //   _pCommPlayerWork->bMoveChange[netID]=TRUE;  // 煙を秘密基地に入る時に出したいので この瞬間はclientに通知しない 2006.05.16
}

//--------------------------------------------------------------
/**
 * @brief   場所移動する
 * @param   netID
 * @param   x
 * @param   y
 * @retval  none
 */
//--------------------------------------------------------------

void CommPlayerBaseTeleportClient(int netID, int x, int z, int dir)
{
    CommPlayerWork* pCP = CommPlayerGetInstance();
    _PlayerPlace* pPP;
    
    if(!pCP){
        return;
    }
    pPP = &pCP->sPlayerPlace[netID];
    if(!pCP->bResetting){
        FE_FldOBJTrapsmogSet(pCP->pFSys, pPP->xpos, 0, pPP->zpos);
    }
    pPP->xpos = x;
    pPP->zpos = z;
    pPP->dir = dir;
    if(pCP->pPlayer[netID]){
        Player_GPosInit( pCP->pPlayer[netID],
                         x, z, dir );
    }
    if(!pCP->bResetting){
        FE_FldOBJTrapsmogSet(pCP->pFSys, pPP->xpos, 0, pPP->zpos);
    }
#if T1648_060818_FIX
    if(!pCP->bResetting){
        CommPlayerSetFENoneAdd(netID);
    }
#endif
}

//--------------------------------------------------------------
/**
 * 旗を消す
 * @param	netID   playerID
 * @retval	消したらTRUE
 */
//--------------------------------------------------------------
BOOL CommPlayerDeleteFlagData(int netID)
{
    CommPlayerWork* pCP = CommPlayerGetInstance();
    if(pCP->pNowFlag[netID] != NULL){
        sys_FreeMemoryEz(pCP->pNowFlag[netID]);
        pCP->pNowFlag[netID] = NULL;
        pCP->hedFE[netID] = _HFE_NONE;
        pCP->sendFlagData[netID].netID = INVALID_NETID;
        return TRUE;
    }
    return FALSE;
}

//--------------------------------------------------------------
/**
 * 旗を持っているかどうか
 * @param	netID   playerID
 * @retval	持っていたらTRUE
 */
//--------------------------------------------------------------
BOOL CommPlayerIsFlagData(int netID)
{
    CommPlayerWork* pCP = CommPlayerGetInstance();
    if(pCP->pNowFlag[netID] != NULL){
        return TRUE;
    }
    return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   相手のパソコンから旗を取る
 * @param   myNetID   旗をとる人
 * @param   targetID  旗をとられる人
 * @retval  none
 */
//--------------------------------------------------------------

BOOL CommPlayerGetFlag(int myNetID, int targetNetID)
{
    CommPlayerWork* pCP = CommPlayerGetInstance();
    if(pCP->pNowFlag[myNetID] == NULL){
        MYSTATUS* pTargetStatus = CommInfoGetMyStatus(targetNetID);
        if(pTargetStatus){
            pCP->pNowFlag[myNetID] = MyStatus_AllocWork(HEAPID_COMMUNICATION);
            MyStatus_Copy(pTargetStatus, pCP->pNowFlag[myNetID]);
            pCP->hedFE[myNetID] = _HFE_FLAG;
//            OHNO_PRINT("はたtor %d\n",myNetID);
            pCP->sendFlagData[myNetID].netID = myNetID;
            MyStatus_Copy(pTargetStatus, (MYSTATUS*)&pCP->sendFlagData[myNetID].mystatus);
            if(targetNetID == CommGetCurrentID()){
                SB_RECORD_DATA* pSBD = SaveData_GetSecretBaseRecord(pCP->pFSys->savedata);
                SecretBaseRecordSetFlagStealNum(pSBD);  // とられた
            }
            return TRUE;
        }
    }
    return FALSE;
}

//==============================================================================
/**
 * びっくりマークを消す関数
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommPlayerMarkDelete(int netID)
{
    CommPlayerWork* pCP = CommPlayerGetInstance();
    if(pCP->pMark[netID] != NULL){
        if(EOA_StatusBit_UseCheck( pCP->pMark[netID] )){
            EOA_Delete(pCP->pMark[netID]);
        }
        else{
            GF_ASSERT(0);
        }
        pCP->pMark[netID] = NULL;
    }
}

//--------------------------------------------------------------
/**
 * @brief   指定の人に旗を出す
 * @param   netID        旗がつく人
 * @retval  none
 */
//--------------------------------------------------------------

void CommPlayerSetFlagDisp(int netID)
{
    CommPlayerWork* pCP = CommPlayerGetInstance();
    if(!pCP->bResetting){
        if(pCP->pPlayer[netID]){
            switch(pCP->hedFE[netID]){
              case _HFE_OK:
                if(pCP->pMark[netID]==NULL){
                    pCP->pMark[netID] = FE_Ok_Add( Player_FieldOBJGet(pCP->pPlayer[netID]) );
                }
                pCP->hedFE[netID] = _HFE_NONE;
                break;
              case _HFE_EXCLAMATION:
                if(pCP->pMark[netID]==NULL){
                    pCP->pMark[netID] =
                        FE_Exclamation_Add( Player_FieldOBJGet(pCP->pPlayer[netID]) );
                }
                break;
              case _HFE_FLAG:
                if(pCP->pMark[netID]==NULL){
                    pCP->pMark[netID] =
                        FE_Flag_Add( Player_FieldOBJGet(pCP->pPlayer[netID]) );
                }
                break;
              case _HFE_NONE:
                CommPlayerMarkDelete(netID);
                break;
            }
        }
    }
}

//--------------------------------------------------------------
/**
 * @brief   指定の人にびっくりマークを出す
 * @param   netID        びっくりマークがつく人
 * @retval  none
 */
//--------------------------------------------------------------

void CommPlayerSetFEExclamationAdd(int netID)
{
    CommPlayerWork* pCP = CommPlayerGetInstance();
//    OHNO_PRINT("びっくりマークを出す\n");
    if(pCP->hedFE[ netID ] != _HFE_FLAG){
        pCP->hedFE[ netID ] = _HFE_EXCLAMATION;
    }
}

//--------------------------------------------------------------
/**
 * @brief   指定の人にOKマークを出す
 * @param   netID        びっくりマークがつく人
 * @retval  none
 */
//--------------------------------------------------------------

void CommPlayerSetFEOkAdd(int netID)
{
    CommPlayerWork* pCP = CommPlayerGetInstance();
    if(pCP->hedFE[ netID ] != _HFE_FLAG){
        pCP->hedFE[ netID ] = _HFE_OK;
    }
    CommPlayerMarkDelete(netID);
}

//--------------------------------------------------------------
/**
 * @brief   指定の人にマークを消す
 * @param   netID        けす人
 * @retval  none
 */
//--------------------------------------------------------------

void CommPlayerSetFENoneAdd(int netID)
{
    CommPlayerWork* pCP = CommPlayerGetInstance();
    if(pCP->hedFE[ netID ] != _HFE_FLAG){
        pCP->hedFE[ netID ] = _HFE_NONE;
    }
    CommPlayerMarkDelete(netID);
}

