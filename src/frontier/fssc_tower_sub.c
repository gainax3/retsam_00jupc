//============================================================================================
/**
 * @file	fss_tower_sub.c
 * @bfief	フロンティアシステムスクリプトコマンドサブ：タワー
 * @author	Satoshi Nohara
 * @date	07.05.25
 *
 * b_tower_scr.cの処理を移動
 *
 * 関連ソース	field/scr_btower.c		常駐
 *				field/b_tower_scr.c		フィールドサブ→常駐
 *				field/b_tower_ev.c		常駐
 *				field/b_tower_wifi.c	常駐
 *				field/b_tower_fld.c		フィールドサブ→常駐
 */
//============================================================================================
#include "common.h"
#include "savedata/savedata_def.h"
#include "savedata/record.h"
#include "savedata/frontier_savedata.h"
#include "savedata/fnote_mem.h"
#include "system/buflen.h"
#include "savedata/tv_work.h"
#include "poketool/poke_tool.h"
#include "poketool/pokeparty.h"
#include "system/procsys.h"
#include "system/gamedata.h"
#include "gflib/strbuf_family.h"
#include "application/pokelist.h"
#include "../field/fieldsys.h"
#include "field/situation.h"
#include "field/location.h"
#include "../field/tv_topic.h"
#include "field/tvtopic_extern.h"
#include "field/field_encount.h"
#include "savedata/undergrounddata.h"
#include "savedata/b_tower.h"
#include "field/eventflag.h"
#include "field/evwkdef.h"
#include "../field/sysflag.h"
#include "../field/syswork.h"
//フロンティアシステム
#include "frontier_tool.h"
#include "../field/scr_tool.h"				//FactoryScr_GetWinRecordID
#include "itemtool/itemsym.h"
//通信
//#include "communication/comm_system.h"
//#include "communication/comm_tool.h"
//#include "communication/comm_def.h"
//バトルタワー
#include "../field/b_tower_scr.h"			//BTOWER_SCRWORKの宣言がある
#include "../field/b_tower_ev.h"
#include "tower_tool.h"


//============================================================================================
//
//	プロトタイプ宣言
//
//	関数の頭にFSをつけた(フロンティアシステムの略)
//
//	(b_tower_scr.cにToolsが頭についているのとついていないのがあるのは？)
//
//============================================================================================
//頭にToolsがついていない
void FSTowerScr_ChoiceBtlPartner(BTOWER_SCRWORK* wk,SAVEDATA* sv);
u16 FSTowerScr_GetEnemyObj(BTOWER_SCRWORK* wk,u16 idx);
void FSTowerScr_SetLeaderClearFlag(BTOWER_SCRWORK* wk,u16 mode);
u16	FSTowerScr_IncRound(BTOWER_SCRWORK* wk);
u16 FSTowerScr_GetRenshouCount(BTOWER_SCRWORK* wk);


//============================================================================================
//
//	関数
//
//============================================================================================

//--------------------------------------------------------------
/**
 *	@brief	対戦トレーナー抽選
 */
//--------------------------------------------------------------
void FSTowerScr_ChoiceBtlPartner(BTOWER_SCRWORK* wk,SAVEDATA* sv)
{
	int i;
	u16	monsno[2];
	u16	itemno[2];
	
	switch(wk->play_mode){
	case BTWR_MODE_WIFI:
	case BTWR_MODE_RETRY:
		btltower_BtlPartnerSelectWifi(sv,wk->tr_data,wk->now_round-1);
		break;
	case BTWR_MODE_MULTI:
	case BTWR_MODE_COMM_MULTI:
	case BTWR_MODE_WIFI_MULTI:
		FSRomBattleTowerTrainerDataMake(wk,&(wk->tr_data[0]),
			wk->trainer[(wk->now_round-1)*2+0],wk->member_num,NULL,NULL,NULL,wk->heapID);
		
		//モンスターNoの重複チェック
		for(i = 0;i < wk->member_num;i++){
			monsno[i] = wk->tr_data[0].btpwd[i].mons_no;
			itemno[i] = wk->tr_data[0].btpwd[i].item_no;
		}
		FSRomBattleTowerTrainerDataMake(wk,&(wk->tr_data[1]),
			wk->trainer[(wk->now_round-1)*2+1],wk->member_num,monsno,itemno,NULL,wk->heapID);
		break;
	case BTWR_MODE_DOUBLE:
	case BTWR_MODE_SINGLE:
	default:
		FSRomBattleTowerTrainerDataMake(wk,&(wk->tr_data[0]),
			wk->trainer[wk->now_round-1],wk->member_num,NULL,NULL,NULL,wk->heapID);
		break;
	}
}

//--------------------------------------------------------------
/**
 *	@brief	対戦トレーナーOBJコード取得
 */
//--------------------------------------------------------------
u16 FSTowerScr_GetEnemyObj(BTOWER_SCRWORK* wk,u16 idx)
{
	//トレーナータイプからOBJコードを取得してくる
	return Frontier_TrType2ObjCode(wk->tr_data[idx].bt_trd.tr_type);
}

//--------------------------------------------------------------
/**
 *	@brief	リーダークリアフラグを立てる
 */
//--------------------------------------------------------------
void FSTowerScr_SetLeaderClearFlag(BTOWER_SCRWORK* wk,u16 mode)
{
	wk->leader_f = mode;
}

//--------------------------------------------------------------
/**
 *	@brief	ラウンド数をインクリメント
 */
//--------------------------------------------------------------
u16	FSTowerScr_IncRound(BTOWER_SCRWORK* wk)
{
	++wk->now_win;	//勝利数もインクリメント

	//プラチナで追加されたWIFIマルチは除く
	if( wk->play_mode != BTWR_MODE_WIFI_MULTI ){
		//ランダムの種更新
		wk->play_rnd_seed = BtlTower_PlayFixRand(wk->play_rnd_seed);
	}

	OS_Printf("TowerRoundInc = %d\n",wk->play_rnd_seed);

	return ++wk->now_round;
}

//--------------------------------------------------------------
/**
 *	@brief	プレイ中の連勝数を取得する
 */
//--------------------------------------------------------------
u16 FSTowerScr_GetRenshouCount(BTOWER_SCRWORK* wk)
{
	OS_Printf( "wk->renshou = %d\n", wk->renshou );
	OS_Printf( "wk->now_win = %d\n", wk->now_win );

	if((u32)wk->renshou+wk->now_win > 0xFFFF){
		return 0xFFFF;
	}else{
		return wk->renshou+wk->now_win;
	}
}

