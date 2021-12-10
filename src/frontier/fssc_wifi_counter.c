//============================================================================================
/**
 * @file	fssc_wifi_counter.c
 * @bfief	フロンティアシステムスクリプトコマンド：WiFi受付
 * @author	Satoshi Nohara
 * @date	07.07.31
 */
//============================================================================================
#include "common.h"
#include <nitro/code16.h> 
#include "system/pm_str.h"
#include "system/msgdata.h"						//MSGMAN_GetString
#include "system/wordset.h"						//WordSet_RegistPlayerName
#include "system/lib_pack.h"
#include "system/snd_tool.h"
#include "savedata/frontier_savedata.h"

#include "field/eventflag.h"
#include "field/evwkdef.h"
#include "gflib/strbuf_family.h"				//許可制

#include "../field/field_subproc.h"				//TestBattleProcData

#include "frontier_types.h"
#include "frontier_main.h"
#include "frontier_scr.h"
#include "frontier_scrcmd.h"
#include "fss_task.h"
#include "comm_command_frontier.h"
#include "frontier_wifi.h"
#include "frontier_def.h"
#include "frontier_tool.h"
#include "wifi_counter_tool.h"
#include "fssc_wifi_counter_sub.h"

#include "field/eventflag.h"
#include "../field/scr_stage_sub.h"
#include "../field/scr_tool.h"

#include "poketool/status_rcv.h"
#include "communication/comm_system.h"
#include "communication/comm_def.h"
#include "communication/comm_tool.h"
#include "field/location.h"
#include "../field/b_tower_scr_def.h"
#include "../field/tower_scr_common.h"			//BTOWER_SCRWORK
#include "../field/b_tower_scr.h"				//TowerScr_WorkInit


//============================================================================================
//
//	プロトタイプ宣言
//
//============================================================================================
BOOL FSSC_WiFiCounterWorkAlloc( FSS_TASK * core );
BOOL FSSC_WiFiCounterWorkFree( FSS_TASK * core );
BOOL FSSC_WiFiCounterSendBuf( FSS_TASK * core );
BOOL FSSC_WiFiCounterRecvBuf( FSS_TASK * core );
static BOOL WaitWiFiCounterRecvBuf( FSS_TASK * core );
BOOL FSSC_WiFiCounterBFNoCheck( FSS_TASK * core );
BOOL FSSC_WiFiCounterPokeList( FSS_TASK * core );
static BOOL EvWaitFrWifiCounterPokeList(FSS_TASK * core);
BOOL FSSC_WiFiCounterPokeListGetResult(FSS_TASK * core);
BOOL FSSC_WiFiCounterPairYameruCheck( FSS_TASK * core );
BOOL FSSC_WiFiCounterScrWork( FSS_TASK * core );
BOOL FSSC_WiFiCounterTowerCallBefore( FSS_TASK * core );
BOOL FSSC_WiFiCounterTowerSendTrainerData( FSS_TASK * core );
BOOL FSSC_WiFiCounterTowerRecvTrainerData( FSS_TASK * core );
static BOOL EvWaitWiFiCounterTowerRecvTrainerData(FSS_TASK * core);
BOOL FSSC_WiFiCounterTowerCallAfter( FSS_TASK * core );


//============================================================================================
//
//	コマンド
//
//============================================================================================

//--------------------------------------------------------------
/**
 * @brief	フロンティアWiFi受付ワーク確保
 *
 * @param	core	仮想マシン制御構造体へのポインタ
 *
 * @return	"0"
 */
//--------------------------------------------------------------
BOOL FSSC_WiFiCounterWorkAlloc( FSS_TASK * core ) 
{
	FRWIFI_SCRWORK* wk;
	FRONTIER_EX_PARAM* ex_param;

	ex_param = Frontier_ExParamGet( core->fss->fmain );

	wk = FrWiFiCounterScr_WorkAlloc( ex_param->savedata );
	Frontier_SysWorkSet( core->fss->fmain, wk );

	OS_Printf( "スクリプトフロンティアWiFi受付ワーク確保\n" );
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	フロンティアWiFi受付ワーク開放
 *
 * @param	core	仮想マシン制御構造体へのポインタ
 *
 * @return	"0"
 */
//--------------------------------------------------------------
BOOL FSSC_WiFiCounterWorkFree( FSS_TASK * core ) 
{
	FRWIFI_SCRWORK* wifi_scr_wk;
	wifi_scr_wk =  Frontier_SysWorkGet( core->fss->fmain );
	FrWiFiCounterScr_WorkFree( wifi_scr_wk );
	OS_Printf( "スクリプトフロンティアWiFi受付ワーク開放\n" );
	return 0;
}

//--------------------------------------------------------------
/**
 * 送信
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	"1"
 */
//--------------------------------------------------------------
BOOL FSSC_WiFiCounterSendBuf( FSS_TASK * core )
{
	FRWIFI_SCRWORK* wifi_scr_wk;
	u16 type	= FSSTGetWorkValue( core );
	u16 param	= FSSTGetWorkValue( core );
	u16 param2	= FSSTGetWorkValue( core );
	u16* ret_wk	= FSSTGetWork( core );

	wifi_scr_wk =  Frontier_SysWorkGet( core->fss->fmain );

	*ret_wk = FrWiFiScr_CommSetSendBuf( wifi_scr_wk, type, param, param2 );
	return 1;
}

//--------------------------------------------------------------
/**
 * 受信
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	"0"
 */
//--------------------------------------------------------------
BOOL FSSC_WiFiCounterRecvBuf( FSS_TASK * core )
{
	u16 wk_id = FSSTGetU16( core );

	//仮想マシンの汎用レジスタにワークのIDを格納
	core->reg[0] = wk_id;

	FSST_SetWait( core, WaitWiFiCounterRecvBuf );
	return 1;
}

//return 1 = 終了
static BOOL WaitWiFiCounterRecvBuf( FSS_TASK * core )
{
	FRWIFI_SCRWORK* wifi_scr_wk;
	//u16 type = FSS_GetEventWorkValue( core, core->reg[0] );		//注意！

	wifi_scr_wk =  Frontier_SysWorkGet( core->fss->fmain );

	//OS_Printf( "wkfi_scr_wk->recieve_count = %d\n", wifi_scr_wk->recieve_count );
	if( wifi_scr_wk->recieve_count >= FRONTIER_COMM_PLAYER_NUM ){
		wifi_scr_wk->recieve_count = 0;
		return 1;
	}

	return 0;
}

//--------------------------------------------------------------
/**
 * 施設Noの比較
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	"0"
 */
//--------------------------------------------------------------
BOOL FSSC_WiFiCounterBFNoCheck( FSS_TASK * core )
{
	FRWIFI_SCRWORK* wifi_scr_wk;
	u16 bf_no	= FSSTGetWorkValue( core );
	u16* ret_wk	= FSSTGetWork( core );

	wifi_scr_wk =  Frontier_SysWorkGet( core->fss->fmain );

	if( bf_no == wifi_scr_wk->pair_bf_no ){
		*ret_wk = 1;							//同じ(OK)
	}else{
		*ret_wk = 0;							//違う(NG)
	}

	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ポケモンリスト
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	"1"
 */
//--------------------------------------------------------------------------------------------
BOOL FSSC_WiFiCounterPokeList( FSS_TASK * core )
{
	FRWIFI_SCRWORK* wifi_scr_wk;
	FRONTIER_EX_PARAM* ex_param;
	u16 bf_no	= FSSTGetWorkValue( core );
	u16 type	= FSSTGetWorkValue( core );
	u16* ret_wk	= FSSTGetWork( core );
	
	wifi_scr_wk = Frontier_SysWorkGet( core->fss->fmain );
	ex_param = Frontier_ExParamGet( core->fss->fmain );

#if 0
	//ステージ
	if( bf_no == FRONTIER_NO_STAGE ){
		//wifi_scr_wk->mode = PL_MODE_BATTLE_STAGE;

	//キャッスル
	}else if( bf_no == FRONTIER_NO_CASTLE ){
		//wifi_scr_wk->mode = PL_MODE_BATTLE_CASTLE;

	}else{
		GF_ASSERT(0);
	}
#endif

	//(仮)ポケモンリスト呼び出しにfsysが必要
	wifi_scr_wk->fsys	= ex_param->fsys;
	//wifi_scr_wk->bf_no	= bf_no;
	wifi_scr_wk->ret_wk = ret_wk;

	FrWiFiCounter_PokeSelect( core->fss->fmain, wifi_scr_wk, type );

	//FSST_SetWait( core, EvWaitFrWifiCounterPokeList );
	return 1;
}

//return 1 = 終了
static BOOL EvWaitFrWifiCounterPokeList(FSS_TASK * core)
{
	FRWIFI_SCRWORK* wifi_scr_wk;
	wifi_scr_wk = Frontier_SysWorkGet( core->fss->fmain );

	if( 0 ){
		return 1;
	}
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	ポケモンリストの結果を取得
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	0
 */
//--------------------------------------------------------------------------------------------
BOOL FSSC_WiFiCounterPokeListGetResult(FSS_TASK * core)
{
	int i;
	FRWIFI_SCRWORK* wifi_scr_wk;
	u16 num;
	u16* ret_wk	= FSSTGetWork( core );
	u16* ret2_wk= FSSTGetWork( core );

	wifi_scr_wk = Frontier_SysWorkGet( core->fss->fmain );

	if( wifi_scr_wk->pld->ret_sel == PL_SEL_POS_EXIT ){				//戻る

		*ret_wk = 0xff;

		//参加する施設を再度選ぶことが出来るので、ワークをクリアしないといけない！
		//選択している位置クリア(★)
		for( i=0; i < WIFI_COUNTER_ENTRY_POKE_MAX ;i++ ){
			wifi_scr_wk->sel[i] = 0;
		}

	}else if( wifi_scr_wk->pld->ret_sel == PL_SEL_POS_ENTER ){		//決定

		OS_Printf( "フロンティアWiFi受付　ポケモン選択結果\n" );

		//現在選ばれているポケモンを保存
		*ret_wk = wifi_scr_wk->pld->in_num[0];
		*ret_wk -= 1;												//0-5に変更
		OS_Printf( "*ret_wk = %d\n", *ret_wk );

		*ret2_wk = wifi_scr_wk->pld->in_num[1];
		if( *ret2_wk > 0 ){
			*ret2_wk -= 1;										//0-5に変更
		}
	}

	sys_FreeMemoryEz( wifi_scr_wk->pld );
	wifi_scr_wk->pld = NULL;

	return 0;
}

//--------------------------------------------------------------
/**
 * 相手が選んだメニューを取得
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	"0"
 */
//--------------------------------------------------------------
BOOL FSSC_WiFiCounterPairYameruCheck( FSS_TASK * core )
{
	FRWIFI_SCRWORK* wifi_scr_wk;
	u16* ret_wk	= FSSTGetWork( core );

	wifi_scr_wk	=  Frontier_SysWorkGet( core->fss->fmain );
	*ret_wk = wifi_scr_wk->pair_retire_flag;
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	スクリプトコマンド：FRWIFI_SCRWORKのセット、ゲット
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	"0"
 */
//--------------------------------------------------------------
BOOL FSSC_WiFiCounterScrWork( FSS_TASK * core )
{
	int i;
	u16 id,my_rensyou;
	BTLTOWER_SCOREWORK* scoreSave;
	FRWIFI_SCRWORK* wifi_scr_wk;
	FRONTIER_EX_PARAM* ex_param;

	u8 code		= FSSTGetU8( core );
	u8 param1	= FSSTGetU8( core );
	u8 param2	= FSSTGetU8( core );
	u16* work	= FSSTGetWork( core );

	wifi_scr_wk	=  Frontier_SysWorkGet( core->fss->fmain );
	ex_param	= Frontier_ExParamGet( core->fss->fmain );

	switch( code ){

	//通信設定
	case FWIFI_ID_COMM_COMMAND_INITIALIZE:
		CommCommandFrontierInitialize( wifi_scr_wk );
		break;

	//パートナーがポケモンリストをキャンセルしたか取得
	case FWIFI_ID_GET_PAIR_POKELIST_CANCEL:
		*work = wifi_scr_wk->pair_monsno[0];
		break;

	//施設ごとのお互いのポケモンが条件を満たしているかチェック
	case FWIFI_ID_CHECK_ENTRY:

		//バトルタワー
		if( param1 == FRONTIER_NO_TOWER ){

			*work = 0;				//OK

			//アイテムの被りチェックはどこでしているか？？
	
			//出場ポケモンがかぶっているかチェック
			if( (wifi_scr_wk->monsno[0] == wifi_scr_wk->pair_monsno[0]) ||
				(wifi_scr_wk->monsno[0] == wifi_scr_wk->pair_monsno[1]) ){
				*work+=1;			//NG
			}

			if( (wifi_scr_wk->monsno[1] == wifi_scr_wk->pair_monsno[0]) ||
				(wifi_scr_wk->monsno[1] == wifi_scr_wk->pair_monsno[1]) ){
				*work+=2;			//NG
			}

		//バトルステージ
		}else if( param1 == FRONTIER_NO_STAGE ){

			//同じポケモンを選んでいるかチェック
			if( wifi_scr_wk->monsno[0] == wifi_scr_wk->pair_monsno[0] ){
				*work = 0;			//OK
			}else{
				*work = 1;			//NG
			}

		//バトルキャッスル
		//バトルルーレット
		//}else if( param1 == FRONTIER_NO_CASTLE ){
		}else if( (param1 == FRONTIER_NO_CASTLE) || (param1 == FRONTIER_NO_ROULETTE) ){

			*work = 0;				//OK

			//出場ポケモンがかぶっているかチェック
			if( (wifi_scr_wk->monsno[0] == wifi_scr_wk->pair_monsno[0]) ||
				(wifi_scr_wk->monsno[0] == wifi_scr_wk->pair_monsno[1]) ){
				*work+=1;			//NG
			}

			if( (wifi_scr_wk->monsno[1] == wifi_scr_wk->pair_monsno[0]) ||
				(wifi_scr_wk->monsno[1] == wifi_scr_wk->pair_monsno[1]) ){
				*work+=2;			//NG
			}
		}
		break;
		
	//バトルステージ：連勝中か取得
	case FWIFI_ID_STAGE_GET_CLEAR_FLAG:
#if 0
		*work = (u16)STAGESCORE_GetScoreData(	SaveData_GetStageScore(ex_param->savedata), 
												STAGESCORE_ID_CLEAR_FLAG, 
												STAGE_TYPE_WIFI_MULTI, 0, NULL);
#else
		*work = (u16)FrontierRecord_Get(SaveData_GetFrontier(ex_param->savedata), 
										FRID_STAGE_MULTI_WIFI_CLEAR_BIT,
										Frontier_GetFriendIndex(FRID_STAGE_MULTI_WIFI_CLEAR_BIT) );
#endif
		break;

	//バトルステージ：連勝中のポケモンナンバーを取得
	case FWIFI_ID_STAGE_GET_CLEAR_MONSNO:
		*work = FrontierRecord_Get(	SaveData_GetFrontier(ex_param->savedata), 
					StageScr_GetMonsNoRecordID(STAGE_TYPE_WIFI_MULTI),
					Frontier_GetFriendIndex(StageScr_GetMonsNoRecordID(STAGE_TYPE_WIFI_MULTI)) );
		break;

	//選択したポケモンナンバーを取得
	case FWIFI_ID_GET_ENTRY_MONSNO:
		*work = wifi_scr_wk->monsno[param1]; 
		break;

	//バトルステージ：連勝中フラグクリア、連勝レコードクリア、タイプカウントクリア
	case FWIFI_ID_STAGE_SET_NEW_CHALLENGE:
		BattleStageSetNewChallenge( ex_param->savedata, SaveData_GetStageScore(ex_param->savedata),
									STAGE_TYPE_WIFI_MULTI );
		break;

	//施設Noセット
	case FWIFI_ID_SET_BF_NO:
		OS_Printf( "セットする施設No = %d\n", param1 );
		wifi_scr_wk->bf_no	= param1;
		break;

	//ステージの記録を消していいかのパートナーの選択を取得
	case FWIFI_ID_GET_PAIR_STAGE_RECORD_DEL:
		*work = wifi_scr_wk->pair_stage_del;
		break;

	//施設No取得
	case FWIFI_ID_GET_BF_NO:
		*work = wifi_scr_wk->bf_no;
		break;

	//パートナーが続けて遊ぶか取得
	case FWIFI_ID_GET_PAIR_GAME_CONTINUE:
		*work = wifi_scr_wk->pair_game_continue;
		break;

	//パートナーとの連勝数を比較
	case FWIFI_ID_CHECK_PAIR_RENSYOU:
		*work = 1;
		id = WifiCounterRensyouRecordIDGet( wifi_scr_wk->bf_no );
		my_rensyou = FrontierRecord_Get(SaveData_GetFrontier(wifi_scr_wk->sv), id, 
										Frontier_GetFriendIndex(id) );

		//連勝数が食い違っていたらクリアする
		OS_Printf( "my_rensyou = %d\n", my_rensyou );
		OS_Printf( "pair_rensyou = %d\n", wifi_scr_wk->pair_rensyou );
		if( my_rensyou != wifi_scr_wk->pair_rensyou ){
			FrontierRecord_Set(	SaveData_GetFrontier(wifi_scr_wk->sv), 
								id, Frontier_GetFriendIndex(id), 0 );

			//クリアフラグ落とす
			id = WifiCounterClearRecordIDGet( wifi_scr_wk->bf_no );
			FrontierRecord_Set(	SaveData_GetFrontier(wifi_scr_wk->sv), 
								id, Frontier_GetFriendIndex(id), 0 );

			OS_Printf( "my_rensyou = %d\n", my_rensyou );
			OS_Printf( "pair_rensyou = %d\n", wifi_scr_wk->pair_rensyou );
			*work = 0;	//NG
		}
		break;

	//クリアフラグを取得して、クリア
	case FWIFI_ID_GET_CLEAR_FLAG:
		id = WifiCounterClearRecordIDGet( wifi_scr_wk->bf_no );
		*work = FrontierRecord_Get(	SaveData_GetFrontier(wifi_scr_wk->sv),		//クリアフラグ取得
									id, Frontier_GetFriendIndex(id) );

		//クリアフラグを取得して、0だったら、連勝レコードを0にしておく(友達手帳の記録のため)
		if( *work == 0 ){
			id = WifiCounterRensyouRecordIDGet( wifi_scr_wk->bf_no );
			FrontierRecord_Set(	SaveData_GetFrontier(wifi_scr_wk->sv), 
								id, Frontier_GetFriendIndex(id), 0 );

#if 1
			//タワーの時は周回数をクリア
			if( wifi_scr_wk->bf_no == FRONTIER_NO_TOWER ){

				//現在の周回数リセット
				scoreSave = SaveData_GetTowerScoreData( wifi_scr_wk->sv );
				TowerScoreData_SetStage( scoreSave, BTWR_MODE_WIFI_MULTI, BTWR_DATA_reset );
			}

			//キャッスルの時はCPクリア
			if( wifi_scr_wk->bf_no == FRONTIER_NO_CASTLE ){

				FrontierRecord_Set(	SaveData_GetFrontier(wifi_scr_wk->sv), 
									FRID_CASTLE_MULTI_WIFI_CP,				//現在CP
									Frontier_GetFriendIndex(id), 0 );

				FrontierRecord_Set(	SaveData_GetFrontier(wifi_scr_wk->sv), 
									FRID_CASTLE_MULTI_WIFI_USED_CP,			//使用したCP
									Frontier_GetFriendIndex(id), 0 );
			}

			//ファクトリーLV50の時は交換回数クリア
			if( wifi_scr_wk->bf_no == FRONTIER_NO_FACTORY_LV50 ){
				FrontierRecord_Set(	SaveData_GetFrontier(wifi_scr_wk->sv),	//交換回数
				FactoryScr_GetTradeRecordID(0,FACTORY_TYPE_WIFI_MULTI),
				Frontier_GetFriendIndex(FactoryScr_GetTradeRecordID(0,FACTORY_TYPE_WIFI_MULTI)), 0);
			}

			//ファクトリーLV100の時は交換回数クリア
			if( wifi_scr_wk->bf_no == FRONTIER_NO_FACTORY_LV100 ){
				FrontierRecord_Set(	SaveData_GetFrontier(wifi_scr_wk->sv),	//交換回数
				FactoryScr_GetTradeRecordID(1,FACTORY_TYPE_WIFI_MULTI),
				Frontier_GetFriendIndex(FactoryScr_GetTradeRecordID(1,FACTORY_TYPE_WIFI_MULTI)), 0);
			}
#endif

		}else{
			//クリアフラグを落としてから、セーブに行くようにする(不正な終了の対処)
			FrontierRecord_Set(	SaveData_GetFrontier(wifi_scr_wk->sv),			//クリア(ID注意)
								id, Frontier_GetFriendIndex(id), 0 );
		}

		OS_Printf( "連勝 = %d\n", *work );
		OS_Printf( "クリアフラグ = %d\n", *work );
		break;

	//リスト用ワークをクリア(★)
	case FWIFI_ID_POKELIST_SEL_CLEAR:
		for( i=0; i < WIFI_COUNTER_ENTRY_POKE_MAX ;i++ ){
			wifi_scr_wk->sel[i] = 0;
		}
		wifi_scr_wk->pos = 0;
		break;

	};

	return 0;
}

//--------------------------------------------------------------
/**
 * バトルタワー呼び出し前の準備
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	"0"
 */
//--------------------------------------------------------------
BOOL FSSC_WiFiCounterTowerCallBefore( FSS_TASK * core )
{
	int i;
	BTOWER_SCRWORK* wk;
	FRONTIER_EX_PARAM* ex_param;
	FRWIFI_SCRWORK* wifi_scr_wk;
	FRWIFI_SCRWORK tmp_wifi_scr_wk;

	ex_param = Frontier_ExParamGet( core->fss->fmain );
	wifi_scr_wk =  Frontier_SysWorkGet( core->fss->fmain );

	tmp_wifi_scr_wk = *wifi_scr_wk;				//コピー
	FrWiFiCounterScr_WorkFree( wifi_scr_wk );
	OS_Printf( "スクリプトフロンティアWiFi受付ワーク開放\n" );

	////////////////////////////
	//呼び出しモードを確認する！
	////////////////////////////
	
	//ワークエリアを取得して初期化
	//wk = TowerScr_WorkInit( ex_param->savedata, BTWR_PLAY_NEW, BTWR_MODE_COMM_MULTI );
	wk = TowerScr_WorkInit( ex_param->savedata, BTWR_PLAY_NEW, BTWR_MODE_WIFI_MULTI );
	Frontier_SysWorkSet( core->fss->fmain, wk );

	//地球儀登録
	//(マルチは、まだ通信していないので、スクリプト(d32r0201.ev)でセットしている)
	//if(	(wk->play_mode == BTWR_MODE_COMM_MULTI) || (wk->play_mode == BTWR_MODE_WIFI_MULTI) ){
	if(	wk->play_mode == BTWR_MODE_WIFI_MULTI ){
		FrontierTool_WifiHistory( ex_param->savedata );
	}

	//出場メンバーをセット
	for( i=0; i < wk->member_num ;i++ ){
		wk->member[i] = tmp_wifi_scr_wk.mine_poke_pos[i];
		wk->mem_poke[i] = tmp_wifi_scr_wk.monsno[i];	
		wk->mem_item[i] = tmp_wifi_scr_wk.itemno[i];	
	}

	wk->pare_poke[0]= tmp_wifi_scr_wk.pair_monsno[0];
	wk->pare_poke[1]= tmp_wifi_scr_wk.pair_monsno[1];
	wk->pare_sex	= tmp_wifi_scr_wk.pair_sex;
	wk->partner		= 5+wk->pare_sex;
#if 0
	wk->pare_stage = recv_buf[3];
#endif

	//プラチナで追加されたWIFIマルチは通常のランダムを使用するので更新しない
	//プレイランダムシードを更新する
	//TowerScr_PlayRandUpdate( wk, ex_param->savedata );

	//親機だったら対戦トレーナーNo抽選
	if( CommGetCurrentID() == COMM_PARENT_ID ){
		TowerScr_BtlTrainerNoSet( wk, ex_param->savedata );
	}

	//親は対戦トレーナーNo送信
	//子のみ受信
	
	//手持ち全回復
	PokeParty_RecoverAll( SaveData_GetTemotiPokemon(ex_param->savedata) );

	//BTOWER_SCRWORKに切り替え
	CommCommandFrontierInitialize( wk );
	wk->recieve_count = 0;

	OS_Printf( "*********FSSC_WiFiCounterTowerCallBefore 終了\n" );
	return 0;
}

//--------------------------------------------------------------
/**
 * バトルタワー　通信マルチ　抽選したトレーナーNoを子機に送信
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	"0"
 */
//--------------------------------------------------------------
BOOL FSSC_WiFiCounterTowerSendTrainerData( FSS_TASK * core )
{
	BTOWER_SCRWORK* wk;
	u16* ret_wk	= FSSTGetWork( core );

	OS_Printf( "\n]]]]]]]]]]]]\nフロンティアWiFi受付 バトルタワー　トレーナーNoを送信開始\n" );
	wk =  Frontier_SysWorkGet( core->fss->fmain );

	*ret_wk = CommFrWiFiCounterTowerSendBufTrainerData( wk );
	return 1;
}

//--------------------------------------------------------------
/**
 * バトルタワー　送られてきたトレーナーデータを受信処理待ち
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	"1"
 */
//--------------------------------------------------------------
BOOL FSSC_WiFiCounterTowerRecvTrainerData( FSS_TASK * core )
{
	OS_Printf( "\n***********\nフロンティアWiFi受付 バトルタワー　トレーナーNoを受信開始\n" );
	FSST_SetWait( core, EvWaitWiFiCounterTowerRecvTrainerData );
	return 1;
}

static BOOL EvWaitWiFiCounterTowerRecvTrainerData(FSS_TASK * core)
{
	BTOWER_SCRWORK* wk;
	wk =  Frontier_SysWorkGet( core->fss->fmain );

	//受信人数をチェック
	if( wk->recieve_count < FRONTIER_COMM_PLAYER_NUM ){
		return FALSE;
	}

	wk->recieve_count = 0;
	return TRUE;
}

//--------------------------------------------------------------
/**
 * バトルタワー呼び出し後の後始末
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	"0"
 */
//--------------------------------------------------------------
BOOL FSSC_WiFiCounterTowerCallAfter( FSS_TASK * core )
{
	BTOWER_SCRWORK* btower_wk;
	btower_wk =  Frontier_SysWorkGet( core->fss->fmain );

	//ワーク領域解放
	TowerScr_WorkRelease( btower_wk );
	btower_wk = NULL;
	OS_Printf( "バトルタワーク開放\n" );
	return 0;
}


