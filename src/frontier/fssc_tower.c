//============================================================================================
/**
 * @file	fssc_tower.c
 * @bfief	フロンティアシステムスクリプトコマンド：タワー
 * @author	Satoshi Nohara
 * @date	07.05.25
 *
 * scr_btower.cの処理を移動
 *
 * 関連ソース	field/scr_btower.c		常駐
 *				field/b_tower_scr.c		フィールドサブ→常駐
 *				field/b_tower_ev.c		常駐
 *				field/b_tower_wifi.c	常駐
 *				field/b_tower_fld.c		フィールドサブ→常駐
 */
//============================================================================================
#include "common.h"
#include <nitro/code16.h> 
#include "system/pm_str.h"
#include "system\msgdata.h"					//MSGMAN_GetString
#include "system/brightness.h"				//ChangeBrightnessRequest
#include "system\wordset.h"					//WordSet_RegistPlayerName
#include "system/bmp_menu.h"
#include "system/bmp_list.h"
#include "system/get_text.h"
#include "system/window.h"
#include "system/fontproc.h"
#include "system/wipe.h"
#include "system/lib_pack.h"
#include "system/procsys.h"
#include "system/snd_tool.h"
#include "savedata/misc.h"
#include "savedata/b_tower.h"
#include "msgdata\msg.naix"					//NARC_msg_??_dat
//フィールド								//field_encount.h	LOCATION必要、BTLTOWER_PLAYWORK必要
#include "field/location.h"
#include "../field/field_encount.h"
#include "../field/field_battle.h"			//BattleParam_IsWinResult
#include "../field/field_subproc.h"			//TestBattleProcData
#include "field/evwkdef.h"
#include "field/eventflag.h"
//フロンティアシステム
#include "frontier_types.h"
#include "frontier_main.h"
#include "frontier_scr.h"
#include "frontier_tool.h"
#include "frontier_scrcmd.h"
#include "frontier_scrcmd_sub.h"
#include "fss_task.h"
//バトルタワー
#include "../field/tower_scr_common.h"		//BTOWER_SCRWORK(新規)
#include "../field/b_tower_scr_def.h"		//バトルタワーで使用する定義

#include "fssc_tower_sub.h"
#include "tower_tool.h"
//通信
#include "communication/comm_system.h"
#include "communication/comm_tool.h"
#include "communication/comm_def.h"
//TV
#include "savedata/tv_work.h"
#include "field/tvtopic_extern.h"


//============================================================================================
//
//	プロトタイプ宣言
//
//============================================================================================
BOOL FSSC_TowerScrWork( FSS_TASK * core );
BOOL FSSC_TowerTalkMsgAppear(FSS_TASK* core);
BOOL FSSC_TowerWorkRelease(FSS_TASK* core);
BOOL FSSC_TowerBattleCall( FSS_TASK * core);
BOOL FSSC_TowerCallGetResult( FSS_TASK * core);
BOOL FSSC_TowerSendBuf(FSS_TASK* core);
BOOL FSSC_TowerRecvBuf(FSS_TASK* core);
static BOOL WaitTowerRecvBuf( FSS_TASK * core );
static u16 btltower_GetMineObj(BTOWER_SCRWORK* wk,u8 mode);
static BOOL BtlTowerEv_SioRecvBuf( FSS_TASK* core, BTOWER_SCRWORK* bt_scr_wk, SAVEDATA* sv, u16 mode, u16 ret_wkno );

#if	PL_T0855_080710_FIX
void BtlTower_BattleRecordUpdateEx(BTOWER_SCRWORK* wk,const BATTLE_TOWER_RECORD* rec);
#endif

//============================================================================================
//
//	コマンド
//
//============================================================================================

//--------------------------------------------------------------
/**
 * @brief	スクリプトコマンド：BTOWER_SCRWORKのセット、ゲット
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	"0"
 *
 * 戻り値ワークの引数のところをBTWR_NULL_PARAMにするとダメ！
 */
//--------------------------------------------------------------
BOOL FSSC_TowerScrWork( FSS_TASK * core )
{
	TV_WORK * tvwk;
	POKEMON_PARAM* pp;
	BTOWER_SCRWORK*	bt_scr_wk;		///<バトルタワー制御用ワークポインタ
	int i,j;
	u16 code,param;
	u16* ret_wk;
	FRONTIER_EX_PARAM* ex_param = Frontier_ExParamGet( core->fss->fmain );

	code	= FSSTGetU16( core );
	OS_Printf( "FSSC_TowerScrWork 渡されたコード = %d\n", code );
	param	= FSSTGetWorkValue( core );
	ret_wk	= FSSTGetWork( core );
	//retwk_id	= FSSTGetU16(core);
	//ret_wk	= FSS_GetEventWorkAdrs( core->fsys, retwk_id );

	bt_scr_wk =  Frontier_SysWorkGet( core->fss->fmain );

	switch( code ){

#if 0
	//0:プレイモード別のメンバー数を取得する
	case BTWR_TOOL_GET_MEMBER_NUM:
		*ret_wk = TowerScrTools_GetMemberNum(param);
		break;
#endif

	//2:リセットしてタイトルに戻る
	case BTWR_TOOL_SYSTEM_RESET:
		TowerScrTools_SystemReset();
		break;

#if 0
	//3:new game時にプレイデータをクリアする
	case BTWR_TOOL_CLEAR_PLAY_DATA:
		TowerScrTools_ClearPlaySaveData(SaveData_GetTowerPlayData(ex_param->savedata));
		break;

	//4:セーブデータが有効かどうか返す
	case BTWR_TOOL_IS_SAVE_DATA_ENABLE:
		*ret_wk = TowerScrTools_IsSaveDataEnable(SaveData_GetTowerPlayData(ex_param->savedata));
		break;

	//8:プレイモード別レコードセーブデータ連勝数を返す
	case BTWR_TOOL_GET_RENSHOU_RECORD:
		*ret_wk = TowerScrTools_GetRenshouRecord(ex_param->savedata,param);
		break;

	//9:現在のWIFIランクを操作して返す
	case BTWR_TOOL_WIFI_RANK_DOWN:
		//"NULL"を渡しているが大丈夫なのか？
		*ret_wk = TowerScr_SetWifiRank(NULL,ex_param->savedata,2);
		break;

	//10:現在のWIFIランクを操作して返す
	case BTWR_TOOL_GET_WIFI_RANK:
		//"NULL"を渡しているが大丈夫なのか？
		*ret_wk = TowerScr_SetWifiRank(NULL,ex_param->savedata,0);
		break;

	//11:Wifi成績の未アップロードフラグを制御する
	case BTWR_TOOL_SET_WIFI_UPLOAD_FLAG:
		TowerScrTools_SetWifiUploadFlag(ex_param->savedata,param);
		break;

	//12:WiFiプレイ成績の未アップロードフラグを取得する
	case BTWR_TOOL_GET_WIFI_UPLOAD_FLAG:
		*ret_wk = TowerScrTools_GetWifiUploadFlag(ex_param->savedata);
		break;

	//14:セーブせずに止めたときのエラー処理
	case BTWR_TOOL_SET_NG_SCORE:
		*ret_wk = TowerScrTools_SetNGScore(ex_param->savedata);
		break;

	//15:WIFIのプレイヤーDLデータがあるかどうかチェック
	case BTWR_TOOL_IS_PLAYER_DATA_ENABLE:
		*ret_wk = TowerScrTools_IsPlayerDataEnable(ex_param->savedata);
		break;
#endif

	//46:バトルポイント追加処理(受付外で呼ばれていたものを受付内に移動)
	case BTWR_SUB_ADD_BATTLE_POINT:
		*ret_wk = TowerScr_AddBtlPoint( bt_scr_wk );

		//TV:バトルポイント名人
		tvwk = SaveData_GetTvWork( ex_param->savedata );
		TVTOPIC_BPTemp_Set( tvwk, *ret_wk );
		//TVTOPIC_Entry_Record_BP( ex_param->savedata );

		//レコード更新
		RECORD_Add( SaveData_GetRecord(ex_param->savedata), RECID_WIN_BP, *ret_wk );

		//★frontier_scrcmd.c FSSC_AddBtlPointもチェックする

		break;

	//55:必要な人物OBJコードを返す
	case BTWR_SUB_GET_MINE_OBJ:
		*ret_wk = btltower_GetMineObj(bt_scr_wk,param);
		break;

	//33:現在のラウンド数をスクリプトワークに取得する
	case BTWR_SUB_GET_NOW_ROUND:
		*ret_wk = TowerScr_GetNowRound(bt_scr_wk);
		break;

	//4:ラウンド数をインクリメント
	case BTWR_SUB_INC_ROUND:
		*ret_wk = FSTowerScr_IncRound(bt_scr_wk);
		break;

	//35:7連勝しているかどうかチェック
	case BTWR_SUB_IS_CLEAR:
		*ret_wk = TowerScr_IsClear(bt_scr_wk);
		break;

	//36:プレイ中の連勝数を取得
	case BTWR_SUB_GET_RENSHOU_CNT:
		*ret_wk = FSTowerScr_GetRenshouCount(bt_scr_wk);
		break;

	//43:現在のプレイモードを取得
	case BTWR_SUB_GET_PLAY_MODE:
		*ret_wk = (u16)TowerScr_GetPlayMode(bt_scr_wk);
		break;

	//40:対戦トレーナー抽選
	case BTWR_SUB_CHOICE_BTL_PARTNER:
		FSTowerScr_ChoiceBtlPartner(bt_scr_wk,ex_param->savedata);
		break;

	//41:対戦トレーナーOBJコード取得
	case BTWR_SUB_GET_ENEMY_OBJ:
		*ret_wk = FSTowerScr_GetEnemyObj(bt_scr_wk,param);
		break;

	//44:リーダークリアフラグを立てる
	case BTWR_SUB_SET_LEADER_CLEAR_FLAG:
		FSTowerScr_SetLeaderClearFlag(bt_scr_wk,param);
		break;

	//37:敗戦処理
	case BTWR_SUB_SET_LOSE_SCORE:
		TowerScr_SetLoseScore(bt_scr_wk,ex_param->savedata);
		break;

	//38:クリア処理
	case BTWR_SUB_SET_CLEAR_SCORE:
		TowerScr_SetClearScore(bt_scr_wk, ex_param->savedata, ex_param->fnote_data );
		break;

	//39:休むときに現在のプレイ状況をセーブに書き出す
	case BTWR_SUB_SAVE_REST_PLAY_DATA:
		TowerScr_SaveRestPlayData(bt_scr_wk);
		break;

	//58:受信バッファクリア
	case BTWR_SUB_RECV_BUF_CLEAR:
		MI_CpuClear8(bt_scr_wk->recv_buf,BTWR_SIO_BUF_LEN);
		break;

	//59:初戦をおこなったかセット
	case BTWR_SUB_SET_FIRST_BTL_FLAG:
		bt_scr_wk->first_btl_flag = 1;
		break;

	//100:ワークが不正かチェック
	case BTWR_DEB_IS_WORK_NULL:
		if(bt_scr_wk == NULL){
			*ret_wk = 1;
		}else{
			*ret_wk = 0;
		}
		break;

	//-----------------------------------------------------------------------------------	
	//エラーチェック
	default:
		OS_Printf( "未対応 渡されたコード = %d\n", code );
		GF_ASSERT(0);
		break;
	};

	return 0;
}

//--------------------------------------------------------------
/**
 *	@biref	必要な人物OBJコードを返す
 */
//--------------------------------------------------------------
static u16 btltower_GetMineObj(BTOWER_SCRWORK* wk,u8 mode)
{
	static const u16 five_cord[] = {
	 SEVEN1,SEVEN5,SEVEN2,SEVEN3,SEVEN4
	};
	if(mode == BTWR_PTCODE_OBJ){
		return wk->partner;
	}
	if(mode == BTWR_PTCODE_MINE2){
		if(wk->play_mode == BTWR_MODE_MULTI){
			return five_cord[wk->partner];
		}else{
			if(wk->pare_sex){
				return HEROINE;
			}else{
				return HERO;
			}
		}
	}
	if(wk->my_sex){
		return HEROINE;
	}else{
		return HERO;
	}
}

//--------------------------------------------------------------
/**
 *	@brief	バトルタワー対戦前メッセージ専用表示
 */
//--------------------------------------------------------------
BOOL FSSC_TowerTalkMsgAppear(FSS_TASK* core)
{
	u8 spd;
	u16	*msg;
	BTOWER_SCRWORK*	bt_scr_wk;		///<バトルタワー制御用ワークポインタ
	FRONTIER_EX_PARAM* ex_param = Frontier_ExParamGet( core->fss->fmain );
	u16	tr_idx = FSSTGetU8(core);	//一人目か二人目か？

	bt_scr_wk =  Frontier_SysWorkGet( core->fss->fmain );
	//wk = core->fsys->btower_wk;
	if(bt_scr_wk == NULL){
		return 0;
	}

	msg = bt_scr_wk->tr_data[tr_idx].bt_trd.appear_word;

	FrontierTalkMsgSub2( core, msg, BtlTower_GetTrMsgArc(bt_scr_wk->play_mode) );
	return 1;
}

//--------------------------------------------------------------
/**
 *	@brief	バトルタワー用ワーク解放
 *
 *	@return 0
 */
//--------------------------------------------------------------
BOOL FSSC_TowerWorkRelease(FSS_TASK* core)
{
	BTOWER_SCRWORK*	bt_scr_wk;		///<バトルタワー制御用ワークポインタ
	bt_scr_wk =  Frontier_SysWorkGet( core->fss->fmain );
	
	//ワーク領域解放
	TowerScr_WorkRelease( bt_scr_wk );
	//TowerScr_WorkRelease(core->fsys->btower_wk);
	//core->fsys->btower_wk = NULL;
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	スクリプトコマンド：戦闘呼び出し
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	"1"
 */
//--------------------------------------------------------------
BOOL FSSC_TowerBattleCall( FSS_TASK * core)
{
	BATTLE_PARAM* param;
	BTOWER_SCRWORK* bt_scr_wk;
	FRONTIER_EX_PARAM* ex_param = Frontier_ExParamGet( core->fss->fmain );

	bt_scr_wk = Frontier_SysWorkGet( core->fss->fmain );

	//バトルデータ初期化
	param = BtlTower_CreateBattleParam( bt_scr_wk, ex_param );
	bt_scr_wk->p_work = param;

	//戦闘切り替え
	Snd_DataSetByScene( SND_SCENE_BATTLE, SEQ_BA_TRAIN, 1 );		//バトル曲再生
    Frontier_SubProcSet( core->fss->fmain, &TestBattleProcData, param, FALSE, NULL );
	OS_Printf( "スクリプトタワー戦闘呼び出し\n" );					//field_encount.c
	return 1;
}

//--------------------------------------------------------------
/**
 * @brief	スクリプトコマンド：タワー戦闘結果取得して開放
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	"0"
 */
//--------------------------------------------------------------
BOOL FSSC_TowerCallGetResult( FSS_TASK * core)
{
	BTOWER_SCRWORK* bt_scr_wk;
	BATTLE_PARAM* param;
	u16* ret_wk;

	bt_scr_wk	= Frontier_SysWorkGet( core->fss->fmain );
	ret_wk		= FSSTGetWork( core );
	param		= bt_scr_wk->p_work;

	bt_scr_wk->winlose_flag = BattleParam_IsWinResult( param->win_lose_flag );
	OS_Printf( "bt_scr_wk->winlose_flag = %d\n", bt_scr_wk->winlose_flag );
	*ret_wk	= bt_scr_wk->winlose_flag;

#ifdef PM_DEBUG
	if( sys.cont & PAD_BUTTON_L ){
		bt_scr_wk->winlose_flag = TRUE;
		*ret_wk = TRUE;
		OS_Printf( "デバック操作で勝利判定になりました！\n" );
	}
#endif

#if	PL_T0855_080710_FIX
	//field_encount.cから移動(08.07.10)
	//タワー用対戦結果アップデート処理
	{
		FRONTIER_EX_PARAM* ex_param = Frontier_ExParamGet( core->fss->fmain );
		BtlTower_BattleRecordUpdateEx( bt_scr_wk, &(param->btr) );
	}
#endif

	//BATTLE_PARAMの開放
	BattleParam_Delete( param );
	return 0;
}





#if	PL_T0855_080710_FIX
void BtlTower_BattleRecordUpdateEx(BTOWER_SCRWORK* wk,const BATTLE_TOWER_RECORD* rec)
{
	wk->rec_turn += rec->total_turn;
	wk->rec_down += rec->total_hinshi;
	wk->rec_damage += rec->total_damage;

	OS_Printf( "********************\n" );
	OS_Printf( "wk->rec_turn = %d\n", wk->rec_turn );
	OS_Printf( "wk->rec_down = %d\n", wk->rec_down );
	OS_Printf( "wk->rec_damage = %d\n", wk->rec_damage );
}
#endif



//--------------------------------------------------------------
/**
 *	@brief	通信マルチデータ送信
 */
//--------------------------------------------------------------
BOOL FSSC_TowerSendBuf(FSS_TASK* core)
{
	BTOWER_SCRWORK* bt_scr_wk;
	FRONTIER_EX_PARAM* ex_param = Frontier_ExParamGet( core->fss->fmain );
	u16 mode = FSSTGetWorkValue(core);
	u16 param = FSSTGetWorkValue(core);
	u16* ret_wk	= FSSTGetWork( core );

	bt_scr_wk	= Frontier_SysWorkGet( core->fss->fmain );

	//MI_CpuClear8(bt_scr_wk->recv_buf,BTWR_SIO_BUF_LEN);

	switch(mode){
#if 0
	case 0:	//ポケモン選択
		BTowerComm_SendPlayerData(bt_scr_wk,ex_param->savedata);
		break;
	case 1:	//抽選トレーナー
		BTowerComm_SendTrainerData(bt_scr_wk);
		break;
#endif
	case 2:	//リタイアを選ぶか？
		FSBTowerComm_SendRetireSelect(bt_scr_wk,param);
		break;
	default:
		OS_Printf( "未対応 %d\n", mode );
		GF_ASSERT(0);
		break;
	}

	OS_Printf(">>btwr send = %d,%d,%d\n",
					bt_scr_wk->send_buf[0],bt_scr_wk->send_buf[1],bt_scr_wk->send_buf[2]);

	if( CommToolSetTempData(CommGetCurrentID(),bt_scr_wk->send_buf) == TRUE ){
		*ret_wk = 1;
	}else{
		*ret_wk = 0;
		OS_Printf( "**** tower send fail(→retry)\n" );
		return 1;		//注意！
	}

	//再送信しても、同期待ち、受信人数待ちがないので、
	//この後がずれるかもしれないが、
	//WIFIの時があるので、再送信出来るようにしておく

	return 0;
}

#if 1
//--------------------------------------------------------------
/**
 *	@brief	通信マルチデータ受信
 */
//--------------------------------------------------------------
BOOL FSSC_TowerRecvBuf(FSS_TASK* core)
{
	u16 mode		= FSSTGetU16( core );		//通信type
	u16 ret_wk_id	= FSSTGetU16( core );		//戻り値を格納するワークID

	//仮想マシンの汎用レジスタにワークのIDを格納
	core->reg[0] = mode;
	core->reg[1] = ret_wk_id;
	OS_Printf( "================\nバトルタワー通信データ受信\n" );
	OS_Printf( "mode = %d\n", mode );
	OS_Printf( "ret_wk_id = %d\n", ret_wk_id );

	FSST_SetWait( core, WaitTowerRecvBuf );
	return 1;
}

//return 1 = 終了
static BOOL WaitTowerRecvBuf( FSS_TASK * core )
{
	BTOWER_SCRWORK* bt_scr_wk;
	FRONTIER_EX_PARAM* ex_param = Frontier_ExParamGet( core->fss->fmain );

	bt_scr_wk = Frontier_SysWorkGet( core->fss->fmain );

	//データ受信待ち(注意！reg[0]、reg[1])
	if(BtlTowerEv_SioRecvBuf(core,bt_scr_wk,ex_param->savedata,core->reg[0],core->reg[1]) == TRUE){
		return 1;
	}

	return 0;
}

static BOOL BtlTowerEv_SioRecvBuf( FSS_TASK* core, BTOWER_SCRWORK* bt_scr_wk, SAVEDATA* sv, u16 mode, u16 ret_wkno )
{
	u16	*buf16;
	const void* recv;

	//データ受信待ち
	recv = CommToolGetTempData(1-CommGetCurrentID());
	if(recv == NULL){
		return FALSE;
	}
	//戻り値指定バッファアドレス取得
	//buf16 = GetEventWorkAdrs(fsys,ret_wkno);
	//buf16 = EventWork_GetEventWorkAdrs( SaveData_GetEventWork(sv), ret_wkno );
	buf16 = FSS_GetEventWorkAdrs( core, ret_wkno );

	//受信データモード別解析
	switch(mode){
#if 0
	case 0:
		*buf16 = BTowerComm_RecvPlayerData(bt_scr_wk,recv);
		break;
	case 1:
		*buf16 = BTowerComm_RecvTrainerData(bt_scr_wk,recv);
		break;
#endif
	case 2:
		*buf16 = FSBTowerComm_RecvRetireSelect(bt_scr_wk,recv);
		break;
	default:
		OS_Printf( "未対応 %d\n", mode );
		GF_ASSERT(0);
		break;
	}

	return TRUE;
}
#else
//--------------------------------------------------------------
/**
 * 受信
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	"0"
 */
//--------------------------------------------------------------
BOOL FSSC_TowerRecvBuf( FSS_TASK * core )
{
	u16 wk_id		= FSSTGetU16( core );	//通信typeを格納しているワークID
	u16 ret_wk_id	= FSSTGetU16( core );	//戻り値を格納するワークID

	//仮想マシンの汎用レジスタにワークのIDを格納
	core->reg[0] = wk_id;
	core->reg[1] = ret_wk_id;

	FSST_SetWait( core, WaitTowerRecvBuf );
	return 1;
}

//return 1 = 終了
static BOOL WaitTowerRecvBuf( FSS_TASK * core )
{
	BTOWER_SCRWORK* bt_scr_wk;
	u16 type	= FSS_GetEventWorkValue( core, core->reg[0] );		//注意！
	u16* ret_wk = FSS_GetEventWorkAdrs( core, core->reg[1] );		//注意！

	bt_scr_wk =  Frontier_SysWorkGet( core->fss->fmain );

	if( FactoryScr_CommWaitRecvBuf(bf_scr_wk,type) ){ 
		return 1;
	}

	return 0;
}
#endif


//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


