//============================================================================================
/**
 * @file	fss_roulette_sub.c
 * @bfief	フロンティアシステムスクリプトコマンドサブ：ルーレット
 * @author	Satoshi Nohara
 * @date	07.09.06
 */
//============================================================================================
#include "common.h"
#include "savedata/savedata_def.h"
#include "savedata/record.h"
#include "system/buflen.h"
#include "poketool/poke_tool.h"
#include "poketool/pokeparty.h"
#include "system/gamedata.h"
#include "gflib/strbuf_family.h"
#include "../field/fieldsys.h"
#include "savedata/frontier_savedata.h"
#include "savedata/b_tower.h"
#include "field/eventflag.h"
#include "field/evwkdef.h"
#include "../field/sysflag.h"
#include "../field/syswork.h"

#include "graphic/frontier_bg_def.h"
#include "application/roulette.h"
#include "frontier_types.h"
#include "frontier_main.h"
#include "frontier_map.h"
#include "frontier_scr.h"
#include "frontier_tool.h"
#include "frontier_def.h"
#include "frontier_scrcmd.h"
#include "frontier_scrcmd_sub.h"
#include "roulette_tool.h"
#include "fssc_roulette_sub.h"
#include "../field/scr_tool.h"		//RouletteScr_GetWinRecordID
#include "itemtool/itemsym.h"
#include "comm_command_frontier.h"

#include "communication/comm_system.h"
#include "communication/comm_tool.h"
#include "communication/comm_def.h"

#include "battle/tokusyu_def.h"
#include "field/weather_no.h"


//============================================================================================
//
//	プロトタイプ宣言
//
//============================================================================================
ROULETTE_SCRWORK* RouletteScr_WorkAlloc( SAVEDATA* savedata, u16 init, u8 type, u16 pos1, u16 pos2, u16 pos3, u16* work );
void RouletteScr_WorkInit( ROULETTE_SCRWORK* wk, u16 init );
static void RouletteScr_InitDataSet( ROULETTE_SCRWORK* wk );
static void RouletteScr_ContinueDataSet( ROULETTE_SCRWORK* wk );
static void RouletteScr_ContinuePokePara( ROULETTE_SCRWORK* wk );
void RouletteScr_WorkRelease( ROULETTE_SCRWORK* wk );
void RouletteScr_GetResult( ROULETTE_SCRWORK* wk, void* roulette_call );
u16 RouletteCall_GetRetWork( void* param, u8 pos );
void RouletteScr_BtlBeforePartySet( ROULETTE_SCRWORK* wk );
void RouletteScr_BtlAfterPartySet( ROULETTE_SCRWORK* wk );
int RouletteScr_GetBtlWinPoint( ROULETTE_SCRWORK* wk, POKEPARTY* party, POKEPARTY* pair_party, int turn );
static int RouletteScr_GetBtlWinPointSub( ROULETTE_SCRWORK* wk, POKEPARTY* party, POKEPARTY* pair_party, u8 m_max );
void RouletteScr_BGPanelChange( void* p_work, FMAP_PTR fmap );
void RouletteScr_MainBGChange( void* p_work, FMAP_PTR fmap );
void RouletteScr_AddPokeIcon( ROULETTE_SCRWORK* wk, FMAP_PTR fmap, u16 param1 );
void RouletteScr_DelPokeIcon( ROULETTE_SCRWORK* wk, FMAP_PTR fmap, u16 param1 );
void RouletteScr_MineChgPokeIconVanish( ROULETTE_SCRWORK* wk, FMAP_PTR fmap, u16 param1, u16 param2 );
void RouletteScr_EnemyChgPokeIconVanish( ROULETTE_SCRWORK* wk, FMAP_PTR fmap, u16 param1, u16 param2 );
void RouletteScr_AddItemKeepIcon( ROULETTE_SCRWORK* wk, FMAP_PTR fmap, u16 param1 );
void RouletteScr_DelItemKeepIcon( ROULETTE_SCRWORK* wk, FMAP_PTR fmap, u16 param1 );
void RouletteScr_ChgItemKeepIconVanish( ROULETTE_SCRWORK* wk, FMAP_PTR fmap, u16 param1, u16 param2, u16 param3 );
void RouletteScr_ResColorChange( u8 evy, u8 flag );

//通信
BOOL RouletteScr_CommSetSendBuf( ROULETTE_SCRWORK* wk, u16 type, u16 param );


//============================================================================================
//
//	パネルのキャラクタ、スクリーンテーブル
//
//============================================================================================
static const u16 roulette_panel_data_tbl[ROULETTE_EV_MAX][3] = {
	{ BR_KEKKA01_HP_NCGR_BIN,		BR_KEKKA01_HP_B_NSCR_BIN, BR_ROOM_KEKKA01_HP_NCLR },		//0
	{ BR_KEKKA02_DOKU_NCGR_BIN,		BR_KEKKA02_DOKU_B_NSCR_BIN, BR_ROOM_KEKKA02_DOKU_NCLR },
	{ BR_KEKKA03_MAHI_NCGR_BIN,		BR_KEKKA03_MAHI_B_NSCR_BIN, BR_ROOM_KEKKA03_MAHI_NCLR },
	{ BR_KEKKA04_YAKEDO_NCGR_BIN,	BR_KEKKA04_YAKEDOI_B_NSCR_BIN, BR_ROOM_KEKKA04_YAKEDOI_NCLR },
	{ BR_KEKKA05_NEMURI_NCGR_BIN,	BR_KEKKA05_NEMURI_B_NSCR_BIN, BR_ROOM_KEKKA05_NEMURI_NCLR },
	{ BR_KEKKA06_KOORI_NCGR_BIN,	BR_KEKKA06_KOORI_B_NSCR_BIN, BR_ROOM_KEKKA06_KOORI_NCLR },
	{ BR_KEKKA07_KINOMI_NCGR_BIN,	BR_KEKKA07_KINOMI_B_NSCR_BIN, BR_ROOM_KEKKA07_KINOMI_NCLR },
	{ BR_KEKKA08_DOUGU_NCGR_BIN,	BR_KEKKA08_DOUGU_B_NSCR_BIN, BR_ROOM_KEKKA08_DOUGU_NCLR },
	{ BR_KEKKA09_LEVEL_NCGR_BIN,	BR_KEKKA09_LEVEL_B_NSCR_BIN, BR_ROOM_KEKKA09_LEVEL_NCLR },
	////////////////////////////
	{ BR_KEKKA01_HP_NCGR_BIN,		BR_KEKKA01_HP_A_NSCR_BIN, BR_ROOM_KEKKA01_HP_NCLR },
	{ BR_KEKKA02_DOKU_NCGR_BIN,		BR_KEKKA02_DOKU_A_NSCR_BIN, BR_ROOM_KEKKA02_DOKU_NCLR },	//10
	{ BR_KEKKA03_MAHI_NCGR_BIN,		BR_KEKKA03_MAHI_A_NSCR_BIN, BR_ROOM_KEKKA03_MAHI_NCLR },
	{ BR_KEKKA04_YAKEDO_NCGR_BIN,	BR_KEKKA04_YAKEDOI_A_NSCR_BIN, BR_ROOM_KEKKA04_YAKEDOI_NCLR },
	{ BR_KEKKA05_NEMURI_NCGR_BIN,	BR_KEKKA05_NEMURI_A_NSCR_BIN, BR_ROOM_KEKKA05_NEMURI_NCLR },
	{ BR_KEKKA06_KOORI_NCGR_BIN,	BR_KEKKA06_KOORI_A_NSCR_BIN, BR_ROOM_KEKKA06_KOORI_NCLR },
	{ BR_KEKKA07_KINOMI_NCGR_BIN,	BR_KEKKA07_KINOMI_A_NSCR_BIN, BR_ROOM_KEKKA07_KINOMI_NCLR },
	{ BR_KEKKA08_DOUGU_NCGR_BIN,	BR_KEKKA08_DOUGU_A_NSCR_BIN, BR_ROOM_KEKKA08_DOUGU_NCLR },
	{ BR_KEKKA09_LEVEL_NCGR_BIN,	BR_KEKKA09_LEVEL_A_NSCR_BIN, BR_ROOM_KEKKA09_LEVEL_NCLR },
	////////////////////////////
	{ BR_KEKKA10_HARE_NCGR_BIN,		BR_KEKKA10_HARE_NSCR_BIN, BR_ROOM_KEKKA10_HARE_NCLR },
	{ BR_KEKKA11_AME_NCGR_BIN,		BR_KEKKA11_AME_NSCR_BIN, BR_ROOM_KEKKA11_AME_NCLR },
	{ BR_KEKKA12_SUNA_NCGR_BIN,		BR_KEKKA12_SUNA_NSCR_BIN, BR_ROOM_KEKKA12_SUNA_NCLR },		//20
	{ BR_KEKKA13_ARARE_NCGR_BIN,	BR_KEKKA13_ARARE_NSCR_BIN, BR_ROOM_KEKKA13_ARARE_NCLR },
	{ BR_KEKKA14_KIRI_NCGR_BIN,		BR_KEKKA14_KIRI_NSCR_BIN, BR_ROOM_KEKKA14_KIRI_NCLR },
	{ BR_KEKKA15_TRICK_NCGR_BIN,	BR_KEKKA15_TRICK_NSCR_BIN, BR_ROOM_KEKKA15_TRICK_NCLR },
	{ BR_KEKKA17_SPEEDUP_NCGR_BIN,	BR_KEKKA17_SPEEDUP_NSCR_BIN, BR_ROOM_KEKKA17_SPEEDUP_NCLR },
	{ BR_KEKKA18_SPEEDDN_NCGR_BIN,	BR_KEKKA18_SPEEDDN_NSCR_BIN, BR_ROOM_KEKKA18_SPEEDDN_NCLR },//25
	{ BR_KEKKA19_RANDOM_NCGR_BIN,	BR_KEKKA19_RANDOM_NSCR_BIN, BR_ROOM_KEKKA19_RANDOM_NCLR },
	////////////////////////////
	{ BR_KEKKA16_CHANGE_NCGR_BIN,	BR_KEKKA16_CHANGE_NSCR_BIN, BR_ROOM_KEKKA16_CHANGE_NCLR },	//27
	{ BR_KEKKA21_BP_NCGR_BIN,		BR_KEKKA21_BP_NSCR_BIN, BR_ROOM_KEKKA21_BP_NCLR },			//28
	{ BR_KEKKA20_THROUGH_NCGR_BIN,	BR_KEKKA20_THROUGH_NSCR_BIN, BR_ROOM_KEKKA20_THROUGH_NCLR },//29
	{ BR_KEKKA22_SUKA_NCGR_BIN,		BR_KEKKA22_SUKA_NSCR_BIN, BR_ROOM_KEKKA22_SUKA_NCLR },		//30
	{ BR_KEKKA23_BP2_NCGR_BIN,		BR_KEKKA23_BP2_NSCR_BIN, BR_ROOM_KEKKA23_BP2_NCLR },		//31
};


//============================================================================================
//
//	周回数によって、挑戦時の基本のルーレット速度を決める
//
//============================================================================================
static const u8 roulette_basic_spd_tbl[][2] = {
	//周回数	基本速度
	{ 7,		5 },		//7-
	{ 4,		4 },		//4-6
	{ 0,		3 },		//1-3
};
#define ROULETTE_BASIC_SPD_TBL_MAX	( NELEMS(roulette_basic_spd_tbl) )


//============================================================================================
//
//	関数
//
//============================================================================================

//--------------------------------------------------------------
/**
 * @brief	ワークエリア確保
 *
 * @param	none
 *	
 * ＊かならず RouletteScr_WorkRelease()で領域を開放すること
 */
//--------------------------------------------------------------
ROULETTE_SCRWORK* RouletteScr_WorkAlloc( SAVEDATA* savedata, u16 init, u8 type, u16 pos1, u16 pos2, u16 pos3, u16* work )
{
	u32 buf,exp;
	POKEPARTY* temoti_party;
	POKEMON_PARAM* temp_poke;
	ROULETTEDATA* ca_sv;
	u8 clear_flag,m_max;
	u16	i,chg_flg;
	static ROULETTE_SCRWORK* wk;					//あとで確認]]]]]]]]]]]]]]]]]]]]]]]]]
	ROULETTESCORE* score_sv;

	wk = sys_AllocMemory( HEAPID_WORLD, sizeof(ROULETTE_SCRWORK) );
	MI_CpuClear8( wk, sizeof(ROULETTE_SCRWORK) );

	//セーブデータ取得
	wk->roulette_savedata	= SaveData_GetRouletteData( savedata );
	wk->sv				= savedata;
	wk->heapID			= HEAPID_WORLD;
	wk->p_m_party		= PokeParty_AllocPartyWork( HEAPID_WORLD );
	wk->p_e_party		= PokeParty_AllocPartyWork( HEAPID_WORLD );
	wk->list_del_work	= work;
	wk->decide_ev_no	= ROULETTE_EV_NONE;
	
	ca_sv = wk->roulette_savedata;
	score_sv = SaveData_GetRouletteScore( savedata );

	//新規か、継続か
	if( init == 0 ){
		wk->type	= type;
		wk->round	= 0;
		//RouletteScr_InitDataSet(...)の中で、周回数によって基本速度を設定している
		wk->csr_speed_level = CSR_SPEED_LEVEL_DEFAULT;
		wk->random_flag = 0;

		ROULETTEDATA_Init( ca_sv );	//中断データ初期化

		//WIFIのみ特殊
		if( wk->type == ROULETTE_TYPE_WIFI_MULTI ){
#if 0
			clear_flag = FrontierRecord_Get(SaveData_GetFrontier(wk->sv), 
									FRID_ROULETTE_MULTI_WIFI_CLEAR_BIT,
									Frontier_GetFriendIndex(FRID_ROULETTE_MULTI_WIFI_CLEAR_BIT) );
#else
			clear_flag = SysWork_WifiFrClearFlagGet( SaveData_GetEventWork(wk->sv) );
#endif

		}else{
			//クリアしたかフラグを取得
			clear_flag = (u8)ROULETTESCORE_GetScoreData(score_sv, ROULETTESCORE_ID_CLEAR_FLAG, 
														wk->type, 0, NULL );
		}

		if( clear_flag == 1 ){
			wk->rensyou = FrontierRecord_Get(	SaveData_GetFrontier(wk->sv), 
									RouletteScr_GetWinRecordID(wk->type),
									Frontier_GetFriendIndex(RouletteScr_GetWinRecordID(wk->type)) );
		}else{
			wk->rensyou = 0;
		}

		wk->lap		= (u16)(wk->rensyou / ROULETTE_LAP_ENEMY_MAX);
		wk->win_cnt	= 0;

		//選択した手持ちの位置
		wk->mine_poke_pos[0] = pos1;
		wk->mine_poke_pos[1] = pos2;
		wk->mine_poke_pos[2] = pos3;
	}else{
		//現在のプレイ進行データ取得
		wk->type = (u8)ROULETTEDATA_GetPlayData( ca_sv, ROULETTEDATA_ID_TYPE, 0, 0, NULL);
		wk->round = (u8)ROULETTEDATA_GetPlayData( ca_sv, ROULETTEDATA_ID_ROUND, 0, 0, NULL);
		wk->csr_speed_level = (u8)ROULETTEDATA_GetPlayData( ca_sv, 
													ROULETTEDATA_ID_CSR_SPEED_LEVEL, 0, 0, NULL);
		wk->random_flag = (u8)ROULETTEDATA_GetPlayData( ca_sv, 
													ROULETTEDATA_ID_RANDOM_FLAG, 0, 0, NULL);
		wk->rensyou = FrontierRecord_Get(	SaveData_GetFrontier(wk->sv), 
									RouletteScr_GetWinRecordID(wk->type),
									Frontier_GetFriendIndex(RouletteScr_GetWinRecordID(wk->type)) );
		wk->lap		= (u16)(wk->rensyou / ROULETTE_LAP_ENEMY_MAX);

		for( i=0; i < ROULETTE_ENTRY_POKE_MAX ;i++ ){
			wk->mine_poke_pos[i] = (u8)ROULETTEDATA_GetPlayData(ca_sv, 
																ROULETTEDATA_ID_MINE_POKE_POS, 
																i, 0, NULL);
		}
	}

	//所持アイテムを保存しておく
	for( i=0; i < ROULETTE_ENTRY_POKE_MAX ;i++ ){
		temp_poke = PokeParty_GetMemberPointer( SaveData_GetTemotiPokemon(wk->sv), 
												wk->mine_poke_pos[i] );
		wk->itemno[i] = PokeParaGet( temp_poke, ID_PARA_item, NULL );
	}

	//出場ポケモンをセット
	temoti_party = SaveData_GetTemotiPokemon( wk->sv );
	m_max = Roulette_GetMinePokeNum( wk->type, ROULETTE_FLAG_SOLO );		//味方
	for( i=0; i < m_max; i++ ){

		PokeParty_Add(	wk->p_m_party, 
						PokeParty_GetMemberPointer(temoti_party,wk->mine_poke_pos[i]) );

		////////////////////////////////////////////////////////////
		//保存しておいた、HP、PP、アイテムなどを元に戻さないとダメ！
		////////////////////////////////////////////////////////////
		
		//アイテムを外す
		temp_poke = PokeParty_GetMemberPointer( wk->p_m_party, i );
		buf = 0;
		PokeParaPut( temp_poke, ID_PARA_item, &buf );

		//レベル調整
		if( PokeParaGet(temp_poke,ID_PARA_level,NULL) > 50 ){
			exp = PokeLevelExpGet( PokeParaGet(temp_poke,ID_PARA_monsno,NULL), 50 );
			PokeParaPut( temp_poke, ID_PARA_exp, &exp );
			PokeParaCalc( temp_poke );
		}
	}

	//地球儀登録
	if( Roulette_CommCheck(wk->type) == TRUE ){
		FrontierTool_WifiHistory( wk->sv );
	}

	return wk;
}

//--------------------------------------------------------------
/**
 * @brief	ワークエリアを取得して初期化する
 *
 * @param	savedata	セーブデータへのポインタ
 * @param	init		初期化モード BTWR_PLAY_NEW:初めから、BTWR_PLAY_CONTINE:続きから
 *	
 * ＊かならず RouletteScr_WorkRelease()で領域を開放すること
 */
//--------------------------------------------------------------
void RouletteScr_WorkInit( ROULETTE_SCRWORK* wk, u16 init )
{
	//新規か、継続か
	if( init == 0 ){
		//OS_Printf( "前 新規 問題のwk = %d\n", wk );
		RouletteScr_InitDataSet( wk );
		//OS_Printf( "後 新規 問題のwk = %d\n", wk );
	}else{
		//OS_Printf( "前 継続 問題のwk = %d\n", wk );
		RouletteScr_ContinueDataSet( wk );
		//OS_Printf( "後 継続 問題のwk = %d\n", wk );
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	新規：トレーナー、ポケモンデータを生成
 *
 * @param	wk		ROULETTE_SCRWORKへのポインタ
 */
//--------------------------------------------------------------
static void RouletteScr_InitDataSet( ROULETTE_SCRWORK* wk )
{
	u16 lap;
	int i;
	B_TOWER_POKEMON poke;
	POKEMON_PARAM* temp_poke;
	POKEPARTY* temoti_party;
	u16 poke_check_tbl[6];	//rental
	u16 item_check_tbl[6];	//rental

	OS_Printf( "新規：データを生成\n" );

	//07.09.25 常に周回数*2のトレーナーを取得
	//シングル(0-6)、マルチのパートナー(7-13)で取得
	//その周に登場するトレーナーのindexを全て取得
	Roulette_EnemyLapAllTrainerIndexGet(	wk->type, RouletteScr_CommGetLap(wk), wk->tr_index, 
											ROULETTE_LAP_MULTI_ENEMY_MAX );
	OS_Printf( "lap = %d\n", RouletteScr_CommGetLap(wk) );

	//デバック情報
	for( i=0; i < ROULETTE_LAP_MULTI_ENEMY_MAX ;i++ ){
		OS_Printf( "tr_index[%d] = %d\n", i, wk->tr_index[i] );
	}

	//敵トレーナーが出すポケモン生成
	Frontier_EnemyPokeMake(	Roulette_GetEnemyPokeNum(wk->type,ROULETTE_FLAG_TOTAL),
							wk->tr_index[wk->round], 
							wk->tr_index[wk->round+ROULETTE_LAP_ENEMY_MAX], 
							wk->enemy_poke_index, 
							wk->enemy_poke,						//B_TOWER_POKEMON
							wk->enemy_pow_rnd,
							wk->enemy_personal_rnd,
							Roulette_CommCheck(wk->type) );

	for( i=0; i < 4 ;i++ ){
		OS_Printf( "enemy_poke[%d] = %d\n", i, wk->enemy_poke[i].mons_no );
		OS_Printf( "enemy_index[%d] = %d\n", i, wk->enemy_poke_index[i] );
		OS_Printf( "enemy_pow[%d] = %d\n", i, wk->enemy_pow_rnd[i] );
		OS_Printf( "enemy_personal[%d] = %d\n", i, wk->enemy_personal_rnd[i] );
	}

	//通信で相手の周回数を取得した後に処理する
	lap = RouletteScr_CommGetLap( wk );

	//周回数によって、挑戦時の基本のルーレット速度を決める
	for( i=0; i < ROULETTE_BASIC_SPD_TBL_MAX ;i++ ){
		if( lap >= roulette_basic_spd_tbl[i][0] ){
			wk->csr_speed_level = roulette_basic_spd_tbl[i][1];
			OS_Printf( "basic spd = %d\n", wk->csr_speed_level );
			break;
		}
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	継続：保存しておいたデータを使ってB_TOWER_POKEMONなどを生成
 *
 * @param	wk		ROULETTE_SCRWORKへのポインタ
 */
//--------------------------------------------------------------
static void RouletteScr_ContinueDataSet( ROULETTE_SCRWORK* wk )
{
	u16 buf;
	int i;
	POKEMON_PARAM* pp;
	u8 count;
	B_TOWER_POKEMON bt_poke[ROULETTE_PARTY_POKE_MAX];
	u8 pow_rnd[ROULETTE_PARTY_POKE_MAX];
	u16 poke_index[ROULETTE_PARTY_POKE_MAX];
	u32 personal_rnd[ROULETTE_PARTY_POKE_MAX];
	OS_Printf( "継続：保存しておいたデータをロード\n" );

	//"手持ちポケモンの状態"ロード
	RouletteScr_ContinuePokePara( wk );

	//-----------------------------------------------------------------------------------
	
	//保存しておいたトレーナーのindex取得
	for( i=0; i < ROULETTE_LAP_MULTI_ENEMY_MAX ;i++ ){
		wk->tr_index[i] = (u16)ROULETTEDATA_GetPlayData(	wk->roulette_savedata, 
														ROULETTEDATA_ID_TR_INDEX, i, 0, NULL );
		//OS_Printf( "tr_index[%d] = %d\n", i, wk->tr_index[i] );
	}

	//---------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------
	//保存しておいた敵ポケモン取得
	for( i=0; i < ROULETTE_ENEMY_POKE_MAX ;i++ ){

		poke_index[i] = (u16)ROULETTEDATA_GetPlayData(wk->roulette_savedata, 
												ROULETTEDATA_ID_ENEMY_POKE_INDEX, i, 0, NULL );
		//OS_Printf( "poke_index[%d] = %d\n", i, poke_index[i] );
		
		//敵ポケモンのインデックス格納
		wk->enemy_poke_index[i] = poke_index[i];
	}

	//手持ちポケモンを一度に生成(personalなし)
	Frontier_PokemonParamCreateAll(	bt_poke, poke_index, pow_rnd,
									NULL, personal_rnd, ROULETTE_ENEMY_POKE_MAX, HEAPID_WORLD,
									ARC_PL_BTD_PM );

	pp = PokemonParam_AllocWork( HEAPID_WORLD );
	for( i=0; i < ROULETTE_ENEMY_POKE_MAX ;i++ ){
		//バトルタワー用ポケモンデータからPOKEMON_PARAMを生成
		Frontier_PokeParaMake( &bt_poke[i], pp, Roulette_GetLevel(wk) );
		Roulette_PokePartyAdd( wk, wk->p_e_party, pp );
		OS_Printf( "継続e_party[%d] monsno = %d\n", i, PokeParaGet(pp,ID_PARA_monsno,NULL) );

	}
	sys_FreeMemoryEz( pp );

	return;
}

//--------------------------------------------------------------
/**
 * @brief	継続：HP、PP、CONDITION、ITEMをロード
 *
 * @param	wk		ROULETTE_SCRWORKへのポインタ
 */
//--------------------------------------------------------------
static void RouletteScr_ContinuePokePara( ROULETTE_SCRWORK* wk )
{
	u8 count;
	u16 buf;
	int i;
	POKEMON_PARAM* pp;

	count = PokeParty_GetPokeCount( wk->p_m_party );

	//"手持ちポケモンの状態"ロード
	for( i=0; i < count ;i++ ){

		pp = PokeParty_GetMemberPointer( wk->p_m_party, i );

		buf = (u16)ROULETTEDATA_GetPlayData(wk->roulette_savedata, 
											ROULETTEDATA_ID_TEMOTI_ITEM, i, 0, NULL );
		PokeParaPut( pp, ID_PARA_item, &buf );			//ITEM
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	ワークエリアを開放する
 */
//--------------------------------------------------------------
void RouletteScr_WorkRelease( ROULETTE_SCRWORK* wk )
{
	int	heap;

	if( wk == NULL ){
		return;
	}

	if( wk->p_m_party != NULL ){
		sys_FreeMemoryEz( wk->p_m_party );
	}

	if( wk->p_e_party != NULL ){
		sys_FreeMemoryEz( wk->p_e_party );
	}

	MI_CpuClear8( wk, sizeof(ROULETTE_SCRWORK) );
	sys_FreeMemoryEz( wk );
	wk = NULL;

	return;
}

//--------------------------------------------------------------
/**
 * @brief	ルーレット画面呼び出し後の結果取得
 *
 * @param	wk		ROULETTE_SCRWORKのポインタ
 */
//--------------------------------------------------------------
void RouletteScr_GetResult( ROULETTE_SCRWORK* wk, void* roulette_call )
{
	int i;
	ROULETTE_CALL_WORK* call_wk = roulette_call;

	//結果を取得する
	for( i=0; i < ROULETTE_RET_WORK_MAX ;i++ ){
		wk->ret_work[i] = RouletteCall_GetRetWork( roulette_call, i );
		OS_Printf( "wk->ret_work[%d] = %d\n", i, wk->ret_work[i] );
	}

	return;
}

//----------------------------------------------------------------------------
/**
 * @brief	ポケモン選択画面での結果を取得
 *
 * @param	param	ROULETTE_CALL_WORKのポインタ
 * @param	pos		ret_work[pos](0-5)
 *
 * @return	"結果"
 */
//----------------------------------------------------------------------------
u16 RouletteCall_GetRetWork( void* param, u8 pos )
{
	ROULETTE_CALL_WORK* roulette_call = param;

	if( pos >= ROULETTE_RET_WORK_MAX ){
		GF_ASSERT(0);
		return 0;
	}

	return roulette_call->ret_work;
}


/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
/*****************************************************/
void RouletteScr_SaveRestPlayData( ROULETTE_SCRWORK* wk, u8 mode );
u16	RouletteScr_IncRound( ROULETTE_SCRWORK* wk );
u16	RouletteScr_GetRound( ROULETTE_SCRWORK* wk );
u16 RouletteScr_GetEnemyObjCode( ROULETTE_SCRWORK* wk, u8 param );
u16 RouletteScr_GetTrIndex( ROULETTE_SCRWORK* wk, u8 param );
void RouletteScr_SetLose( ROULETTE_SCRWORK* wk );
void RouletteScr_SetClear( ROULETTE_SCRWORK* wk );

//--------------------------------------------------------------
/**
 * @brief	休むときに現在のプレイ状況をセーブに書き出す
 *
 * @param	wk		ROULETTE_SCRWORKのポインタ
 * @param	mode	FR_MODE_CLEAR="クリア",FR_MODE_LOSE="負け",FR_MODE_REST="休む"
 */
//--------------------------------------------------------------
void RouletteScr_SaveRestPlayData( ROULETTE_SCRWORK* wk, u8 mode )
{
	u16	i;
	u8 e_max;
	u8	buf8[4];
	u16	buf16[4];
	u32	buf32[4];
	u32 after,count;
	POKEMON_PARAM* pp;
	ROULETTEDATA* f_sv = wk->roulette_savedata;
	ROULETTESCORE* score_sv = SaveData_GetRouletteScore( wk->sv );

	//タイプによってポケモンの数を取得
	e_max = Roulette_GetEnemyPokeNum( wk->type, ROULETTE_FLAG_TOTAL );	//敵

	//"シングル、ダブル、マルチ、wifiマルチ"書き出し
	buf8[0] = wk->type;
	ROULETTEDATA_PutPlayData( wk->roulette_savedata, ROULETTEDATA_ID_TYPE, 0, 0, buf8 );
	
	//セーブフラグを有効状態にリセット
	ROULETTEDATA_SetSaveFlag( wk->roulette_savedata, TRUE );

	//"ラウンド数"書き出し(0-6人目の何人目かをあわらす)
	buf8[0] = wk->round;
	//OS_Printf( "wk->round = %d\n", wk->round );
	ROULETTEDATA_PutPlayData( wk->roulette_savedata, ROULETTEDATA_ID_ROUND, 0, 0, buf8 );

	//"カーソルスピードレベル"書き出し
	buf8[0] = wk->csr_speed_level;
	//OS_Printf( "wk->csr_speed_level = %d\n", wk->csr_speed_level );
	ROULETTEDATA_PutPlayData( wk->roulette_savedata, ROULETTEDATA_ID_CSR_SPEED_LEVEL, 0, 0, buf8 );

	//"カーソルランダム移動フラグ"書き出し
	buf8[0] = wk->random_flag;
	//OS_Printf( "wk->random_flag = %d\n", wk->random_flag );
	ROULETTEDATA_PutPlayData( wk->roulette_savedata, ROULETTEDATA_ID_RANDOM_FLAG, 0, 0, buf8 );

#if 0
	//連勝 / 7 = 周回数
	//連勝 % 7 = 何人目か
#endif
	//"連勝数"書き出し(「次は27人目です」というように使う)
	OS_Printf( "rensyou = %d\n", wk->rensyou );
	FrontierRecord_Set(	SaveData_GetFrontier(wk->sv), 
						RouletteScr_GetWinRecordID(wk->type),
						Frontier_GetFriendIndex(RouletteScr_GetWinRecordID(wk->type)), wk->rensyou);
	
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//"休む"以外
	if( mode != FR_MODE_REST ){

		//"最大連勝数"書き出し
		after = FrontierRecord_SetIfLarge(	SaveData_GetFrontier(wk->sv),
								RouletteScr_GetMaxWinRecordID(wk->type),
								Frontier_GetFriendIndex(RouletteScr_GetMaxWinRecordID(wk->type)), 
								wk->rensyou );
		OS_Printf( "ROULETTE after = %d\n", after  );

		//"7連勝(クリア)したかフラグ"書き出し
		buf8[0] = wk->clear_flag;
		ROULETTESCORE_PutScoreData( score_sv, ROULETTESCORE_ID_CLEAR_FLAG, wk->type, 0, buf8 );

		//WIFIのみ特殊
		if( wk->type == ROULETTE_TYPE_WIFI_MULTI ){
			FrontierRecord_Set(	SaveData_GetFrontier(wk->sv), 
					FRID_ROULETTE_MULTI_WIFI_CLEAR_BIT,
					Frontier_GetFriendIndex(FRID_ROULETTE_MULTI_WIFI_CLEAR_BIT), wk->clear_flag );
		}
	}
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

	//"トレーナーインデックス"書き出し
	for( i=0; i < ROULETTE_LAP_MULTI_ENEMY_MAX ;i++ ){
		buf16[0] = wk->tr_index[i];
		ROULETTEDATA_PutPlayData( wk->roulette_savedata, ROULETTEDATA_ID_TR_INDEX, i, 0, buf16 );
	}

	//"手持ち位置"書き出し
	for( i=0; i < ROULETTE_ENTRY_POKE_MAX ;i++ ){
		buf8[0] = wk->mine_poke_pos[i];
		ROULETTEDATA_PutPlayData(wk->roulette_savedata, ROULETTEDATA_ID_MINE_POKE_POS, i, 0, buf8 );
	}

	//"手持ちポケモンの状態"書き出し
	count = PokeParty_GetPokeCount( wk->p_m_party );
	for( i=0; i < count ;i++ ){

		pp = PokeParty_GetMemberPointer( wk->p_m_party, i );

		buf16[0] = PokeParaGet( pp, ID_PARA_item, NULL );		//ITEM
		ROULETTEDATA_PutPlayData( wk->roulette_savedata, ROULETTEDATA_ID_TEMOTI_ITEM, i, 0, buf16 );
	}

	//敵
	count = PokeParty_GetPokeCount( wk->p_e_party );
	for( i=0; i < count ;i++ ){

		pp = PokeParty_GetMemberPointer( wk->p_e_party, i );

		//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
		//ここがモンスターナンバーではない！
		//インデックス！
		//buf16[0] = PokeParaGet( pp, ID_PARA_monsno, NULL );
		buf16[0] = wk->enemy_poke_index[i];
		ROULETTEDATA_PutPlayData(	wk->roulette_savedata, ROULETTEDATA_ID_ENEMY_POKE_INDEX, 
									i, 0, buf16 );
		//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	ラウンド数をインクリメント
 *
 * @param	wk		ROULETTE_SCRWORKのポインタ
 */
//--------------------------------------------------------------
u16	RouletteScr_IncRound( ROULETTE_SCRWORK* wk )
{
	wk->round++;
	return wk->round;
}

//--------------------------------------------------------------
/**
 * @brief	ラウンド数を取得
 *
 * @param	wk		ROULETTE_SCRWORKのポインタ
 */
//--------------------------------------------------------------
u16	RouletteScr_GetRound( ROULETTE_SCRWORK* wk )
{
	return wk->round;
}

//--------------------------------------------------------------
/**
 * @brief	対戦トレーナーOBJコード取得
 *
 * @param	wk		ROULETTE_SCRWORKのポインタ
 * @param	param	0=1人目、1=2人目(敵のパートナ－)
 */
//--------------------------------------------------------------
u16 RouletteScr_GetEnemyObjCode( ROULETTE_SCRWORK* wk, u8 param )
{
	B_TOWER_TRAINER bt_trd;
	B_TOWER_TRAINER_ROM_DATA* p_rom_tr;
	u8 index;

	//取得するトレーナーデータのインデックスを取得
	index = RouletteScr_GetTrIndex( wk, param );

	//ROMからトレーナーデータを確保
	p_rom_tr = Frontier_TrainerDataGet( &bt_trd, wk->tr_index[index], HEAPID_WORLD, ARC_PL_BTD_TR );
	sys_FreeMemoryEz( p_rom_tr );

	OS_Printf( "1人目、2人目？ = %d\n", param );
	OS_Printf( "敵トレーナータイプ = %d\n", bt_trd.tr_type );
	OS_Printf( "敵トレーナーOBJコード = %d\n", Frontier_TrType2ObjCode(bt_trd.tr_type) );

	//トレーナータイプからOBJコードを取得してくる
	return Frontier_TrType2ObjCode( bt_trd.tr_type );
}

//--------------------------------------------------------------
/**
 * @brief	wk->tr_index[トレーナーデータのインデックスを取得]
 *
 * @param	wk		ROULETTE_SCRWORKのポインタ
 * @param	param	0=1人目、1=2人目(敵のパートナ－)
 */
//--------------------------------------------------------------
u16 RouletteScr_GetTrIndex( ROULETTE_SCRWORK* wk, u8 param )
{
	return ( wk->round + (param * ROULETTE_LAP_ENEMY_MAX) );
}

//--------------------------------------------------------------
/**
 * @brief	敗戦した時の処理
 *
 * @param	wk		ROULETTE_SCRWORKのポインタ
 */
//--------------------------------------------------------------
void RouletteScr_SetLose( ROULETTE_SCRWORK* wk )
{
	u16 buf16[4];
	int i;

	OS_Printf( "\nバトルルーレットデータ　敗戦セット\n" );

	//現在の5連勝などを保存する必要がある！
	RouletteScr_SaveRestPlayData( wk, FR_MODE_LOSE );		//セーブデータに代入

	//新規か、継続かは、WK_SCENE_ROULETTE_LOBBYに格納されているので、
	//ここでroundなどをクリアしなくても、
	//WK_SCENE_ROULETTE_LOBBYが継続ではない状態でセーブされるので、
	//受付に話しかけても、新規判定になりワークはクリアされる。
	return;
}

//--------------------------------------------------------------
/**
 * @brief	7連勝(クリア)した時の処理
 *
 * @param	wk		ROULETTE_SCRWORKのポインタ
 */
//--------------------------------------------------------------
void RouletteScr_SetClear( ROULETTE_SCRWORK* wk )
{
	OS_Printf( "\nバトルルーレットデータ　7連勝(クリア)セット\n" );
	
	/***********************************************************/
	//
	//レコードにもデータを保持する必要があるかは保留
	//
	//
	//スコアウィンドウを開く時に参照するデータが、
	//レコードという作り？？それとも、通信で渡すデータ？？
	//
	/***********************************************************/

	wk->clear_flag = 1;						//7連勝(クリア)したかフラグON

	//wk->rensyou		= 0;				//現在の連勝数
	if( wk->lap < ROULETTE_LAP_MAX ){
		wk->lap++;							//周回数のインクリメント
	}

#if 0
	//RouletteScr_CommGetLapでずれが出ないようにペアも更新
	if( wk->pair_lap < ROULETTE_LAP_MAX ){
		wk->pair_lap++;						//周回数のインクリメント
	}
#endif

	wk->round			= 0;				//今何人目？
	RouletteScr_SaveRestPlayData( wk, FR_MODE_CLEAR );		//セーブデータに代入
	return;
}

//--------------------------------------------------------------
/**
 * @brief	戦闘前のPOKEPARTYのセット
 *
 * @param	wk		ROULETTE_SCRWORKのポインタ
 */
//--------------------------------------------------------------
void RouletteScr_BtlBeforePartySet( ROULETTE_SCRWORK* wk )
{
	Roulette_EnemyPartySet( wk );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	バトル後のPOKEPARTYのセット
 *
 * @param	wk		ROULETTE_SCRWORKのポインタ
 */
//--------------------------------------------------------------
void RouletteScr_BtlAfterPartySet( ROULETTE_SCRWORK* wk )
{
	int i,count,tmp_count;
	u8 e_max;
#if 0
	u16 check_monsno[ ROULETTE_TOTAL_POKE_MAX ];		//mine=4,enemy=4 = 8
	u16 check_itemno[ ROULETTE_TOTAL_POKE_MAX ];
	POKEMON_PARAM* temp_poke;
	B_TOWER_TRAINER bt_trd;
	B_TOWER_TRAINER_ROM_DATA* trd;

	//初期化	
	for( i=0; i < ROULETTE_TOTAL_POKE_MAX ; i++ ){	
		check_monsno[i] = 0;
		check_itemno[i] = 0;
	}

	//タイプによってポケモンの数を取得
	e_max = Roulette_GetEnemyPokeNum( wk->type, ROULETTE_FLAG_TOTAL );//敵

	count = PokeParty_GetPokeCount( wk->p_m_party );
	for( i=0; i < count ; i++ ){									//手持ちポケモン
		temp_poke = PokeParty_GetMemberPointer( wk->p_m_party, i );
		check_monsno[i] = PokeParaGet(temp_poke, ID_PARA_monsno, NULL);
		check_itemno[i] = PokeParaGet(temp_poke, ID_PARA_item, NULL);
	}
	tmp_count = count;

	count = PokeParty_GetPokeCount( wk->p_e_party );
	for( i=0; i < count ; i++ ){									//敵ポケモン
		temp_poke = PokeParty_GetMemberPointer( wk->p_e_party, i );
		check_monsno[i+tmp_count] = PokeParaGet(temp_poke, ID_PARA_monsno, NULL);
		check_itemno[i+tmp_count] = PokeParaGet(temp_poke, ID_PARA_item, NULL);
	}

	//ROMからトレーナーデータを確保
	trd = Frontier_TrainerDataGet( &bt_trd, wk->tr_index[wk->round], HEAPID_WORLD, ARC_PL_BTD_TR );

	SDK_ASSERTMSG( (tmp_count + count) <= ROULETTE_TOTAL_POKE_MAX, "check bufの要素数が足りない！");

	//被りポケ、アイテム、データ数、取得する数、代入先
	Frontier_PokemonIndexCreate(trd, check_monsno, check_itemno, 
								(tmp_count + count),				//手持ち、敵の数
								e_max, wk->enemy_poke_index, HEAPID_WORLD );

	sys_FreeMemoryEz( trd );

	//敵ポケモンを一度に生成(personalなし)
	Frontier_PokemonParamCreateAll(	wk->enemy_poke, wk->enemy_poke_index, 
									wk->enemy_pow_rnd, NULL, wk->enemy_personal_rnd, 
									e_max, HEAPID_WORLD, ARC_PL_BTD_PM );
#else
	//敵トレーナーが出すポケモン生成
	Frontier_EnemyPokeMake(	Roulette_GetEnemyPokeNum(wk->type,ROULETTE_FLAG_TOTAL),
							wk->tr_index[wk->round], 
							wk->tr_index[wk->round+ROULETTE_LAP_ENEMY_MAX], 
							wk->enemy_poke_index, 
							wk->enemy_poke,						//B_TOWER_POKEMON
							wk->enemy_pow_rnd,
							wk->enemy_personal_rnd,
							Roulette_CommCheck(wk->type) );

	for( i=0; i < 4 ;i++ ){
		OS_Printf( "enemy_poke[%d] = %d\n", i, wk->enemy_poke[i].mons_no );
		OS_Printf( "enemy_index[%d] = %d\n", i, wk->enemy_poke_index[i] );
		OS_Printf( "enemy_pow[%d] = %d\n", i, wk->enemy_pow_rnd[i] );
		OS_Printf( "enemy_personal[%d] = %d\n", i, wk->enemy_personal_rnd[i] );
	}
#endif

	//敵パーティのセット
	Roulette_EnemyPartySet( wk );

	return;
}

#define ROULETTE_POINT_TBL_MAX	(5)
static const u8 roulette_bad_condition_point_tbl[ROULETTE_POINT_TBL_MAX] = { 8, 6, 4, 0, 0 };
static const u8 roulette_hinsi_point_tbl[ROULETTE_POINT_TBL_MAX] = { 6, 4, 2, 0, 0 };
static const u8 roulette_turn_point_tbl[ROULETTE_POINT_TBL_MAX][2] = {
	{ 3, 10 }, 
	{ 5,  6 },
	{ 7,  4 },
	{ 9,  2 },
	{ 10, 0 },
};

//--------------------------------------------------------------
/**
 * @brief	バトル勝利後のポイント計算
 *
 * @param	wk			ROULETTE_SCRWORK型のポインタ
 *
 * @return	"ポイント"
 */
//--------------------------------------------------------------
int RouletteScr_GetBtlWinPoint( ROULETTE_SCRWORK* wk, POKEPARTY* party, POKEPARTY* pair_party, int turn )
{
	u8 m_max;
	int i;
	POKEMON_PARAM* pp;
	int ret;

	OS_Printf( "********************\n戦闘後のポイント計算\n" );

	ret = 0;
	m_max = Roulette_GetMinePokeNum( wk->type, ROULETTE_FLAG_SOLO );

	//瀕死、状態のポイント
	ret += RouletteScr_GetBtlWinPointSub( wk, party, pair_party, m_max );

	//ターン数のポイント
	for( i=0; i < ROULETTE_POINT_TBL_MAX ;i++ ){
		if( turn < roulette_turn_point_tbl[i][0] ){
			ret += roulette_turn_point_tbl[i][1];
			OS_Printf( "ポイント = %d\n\n", roulette_turn_point_tbl[i][1] );
			break;
		}
	}
	OS_Printf( "turn数 = %d\n", turn );

	OS_Printf( "ポイント合計 = %d\n", ret );
	return ret;
}

static int RouletteScr_GetBtlWinPointSub( ROULETTE_SCRWORK* wk, POKEPARTY* party, POKEPARTY* pair_party, u8 m_max )
{
	u32 hp;
	u8 tmp_hinsi;
	u8 tmp_bad_condition;
	int i,ret;
	POKEMON_PARAM* pp;

	//クリア
	ret = 0;
	tmp_hinsi = 0;
	tmp_bad_condition = 0;

	for( i=0; i < m_max; i++ ){

		pp = PokeParty_GetMemberPointer( party, i );
		OS_Printf( "monsno = %d\n", PokeParaGet(pp,ID_PARA_monsno,NULL) );

		if( PokeParaGet(pp,ID_PARA_poke_exist,NULL) == FALSE ){
			continue;
		}

		hp = PokeParaGet( pp, ID_PARA_hp, NULL );

		//気絶しているポケモン数
		if( hp == 0 ){
			tmp_hinsi++;
		}

		//状態異常になっているポケモン数
		if( PokeParaGet(pp,ID_PARA_condition,NULL) != 0 ){
			tmp_bad_condition++;
		}
	}

	//通信の時はペアのポケモンの状態もチェックする
	if( Roulette_CommCheck(wk->type) == TRUE ){

		for( i=0; i < m_max; i++ ){

			pp = PokeParty_GetMemberPointer( pair_party, i );
			OS_Printf( "monsno = %d\n", PokeParaGet(pp,ID_PARA_monsno,NULL) );

			if( PokeParaGet(pp,ID_PARA_poke_exist,NULL) == FALSE ){
				continue;
			}

			hp = PokeParaGet( pp, ID_PARA_hp, NULL );

			//気絶しているポケモン数
			if( hp == 0 ){
				tmp_hinsi++;
			}

			//状態異常になっているポケモン数
			if( PokeParaGet(pp,ID_PARA_condition,NULL) != 0 ){
				tmp_bad_condition++;
			}
		}
	}

	//ポイント計算
	ret += roulette_bad_condition_point_tbl[tmp_bad_condition];
	OS_Printf( "ID_BAD_CONDITIONの数 = %d\n", tmp_bad_condition );
	OS_Printf( "ポイント = %d\n\n", roulette_bad_condition_point_tbl[tmp_bad_condition] );

	ret += roulette_hinsi_point_tbl[tmp_hinsi];
	OS_Printf( "ID_HINSIの数 = %d\n", tmp_hinsi );
	OS_Printf( "ポイント = %d\n\n", roulette_hinsi_point_tbl[tmp_hinsi] );

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief	パネルを変更する
 *
 * @param	wk		ROULETTE_SCRWORKのポインタ
 */
//--------------------------------------------------------------
void RouletteScr_BGPanelChange( void* p_work, FMAP_PTR fmap )
{
	ARCHANDLE *hdl;
	ROULETTE_SCRWORK* wk;
	wk = (ROULETTE_SCRWORK*)p_work;

	OS_Printf( "パネル変更関数が呼ばれました！\n" );
	OS_Printf( "wk->decide_ev_no = %d\n", wk->decide_ev_no );

	if( wk->decide_ev_no != ROULETTE_EV_NONE ){

		hdl = ArchiveDataHandleOpen( ARC_FRONTIER_BG, HEAPID_FRONTIERMAP );
		
		ArcUtil_HDL_BgCharSet(	hdl, roulette_panel_data_tbl[wk->decide_ev_no][0],		//キャラ
								fmap->bgl, FRMAP_FRAME_EFF, 0, 0, TRUE, HEAPID_FRONTIERMAP );

		ArcUtil_HDL_ScrnSet(	hdl, roulette_panel_data_tbl[wk->decide_ev_no][1],		//スクリーン
								fmap->bgl, FRMAP_FRAME_EFF, 0, 0, 1, HEAPID_FRONTIERMAP );

		{
			//パレットデータを拡張パレットVRAMに転送
			NNSG2dPaletteData *palData;
			void *pal_work;
				
			pal_work = ArcUtil_HDL_PalDataGet(	hdl, 
												roulette_panel_data_tbl[wk->decide_ev_no][2],
												&palData, HEAPID_FRONTIERMAP );
			DC_FlushRange( palData->pRawData, palData->szByte );
			GX_BeginLoadBGExtPltt();	   //パレットデータの転送準備
			GX_LoadBGExtPltt( palData->pRawData, 0x4000, 0x2000 );
		    GX_EndLoadBGExtPltt();         //パレットデータの転送完了
		    sys_FreeMemoryEz( pal_work );
		}

		//GF_Disp_GX_VisibleControl( FRMAP_FRAME_EFF, VISIBLE_ON );		//BG表示
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );		//BG表示
		GF_BGL_LoadScreenV_Req( fmap->bgl, FRMAP_FRAME_EFF );

		ArchiveDataHandleClose( hdl );
	}else{
		//GF_Disp_GX_VisibleControl( FRMAP_FRAME_EFF, VISIBLE_OFF );	//BG非表示
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );		//BG非表示
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	背景を変更する
 *
 * @param	wk		ROULETTE_SCRWORKのポインタ
 */
//--------------------------------------------------------------
void RouletteScr_MainBGChange( void* p_work, FMAP_PTR fmap )
{
	ARCHANDLE *hdl;
	ROULETTE_SCRWORK* wk;
	wk = (ROULETTE_SCRWORK*)p_work;

	OS_Printf( "背景変更関数が呼ばれました！\n" );

	hdl = ArchiveDataHandleOpen( ARC_FRONTIER_BG, HEAPID_FRONTIERMAP );
		
	ArcUtil_HDL_ScrnSet(	hdl, BR_ROOM_MUL_NSCR_BIN,		//スクリーン
							fmap->bgl, FRMAP_FRAME_MAP, 0, 0, 1, HEAPID_FRONTIERMAP );

	GF_BGL_LoadScreenV_Req( fmap->bgl, FRMAP_FRAME_MAP );

	ArchiveDataHandleClose( hdl );
	return;
}

typedef struct{
	s16  x;
	s16  y;
}POS;

//味方側のポケモンアイコン表示位置
static const POS mine_icon_pos_tbl[] = {
	{ 33, 20 },
	{ 39, 43 },
	{ 43, 66 },
	{ 46, 89 },
};

//敵側のポケモンアイコン表示位置
static const POS enemy_icon_pos_tbl[] = {
	{ 222, 20 },
	{ 216, 43 },
	{ 213, 66 },
	{ 210, 89 },
};

#define ITEM_KEEP_OFFSET_X	(8)			//アイテム持っているアイコンの表示オフセットX
#define ITEM_KEEP_OFFSET_Y	(4)			//アイテム持っているアイコンの表示オフセットY

//--------------------------------------------------------------
/**
 * @brief	ポケモンアイコン追加
 *
 * @param	wk		ROULETTE_SCRWORKのポインタ
 * @param	param1	0=味方、1=相手
 */
//--------------------------------------------------------------
void RouletteScr_AddPokeIcon( ROULETTE_SCRWORK* wk, FMAP_PTR fmap, u16 param1 )
{
	u8 m_max,e_max;
	int i;
	POKEMON_PARAM* pp;

	m_max = Roulette_GetMinePokeNum( wk->type, ROULETTE_FLAG_TOTAL );
	e_max = Roulette_GetEnemyPokeNum( wk->type, ROULETTE_FLAG_TOTAL );

	if( param1 == 0 ){		//味方
		for( i=0; i < m_max ;i++ ){	
			pp = PokeParty_GetMemberPointer( wk->p_m_party, i );
			wk->m_pokeicon[i] = Frontier_PokeIconAdd( fmap, pp, i, 
												mine_icon_pos_tbl[i].x, mine_icon_pos_tbl[i].y );
			CLACT_SetAnmFlag( wk->m_pokeicon[i]->act, 0 );		//アニメ停止
			//CLACT_SetDrawFlag( wk->m_pokeicon[i]->act, 0 );
		}
	}else{					//敵
		for( i=0; i < e_max ;i++ ){	
			pp = PokeParty_GetMemberPointer( wk->p_e_party, i );
			wk->e_pokeicon[i] = Frontier_PokeIconAdd( fmap, pp, i+m_max,
												enemy_icon_pos_tbl[i].x, enemy_icon_pos_tbl[i].y );
			CLACT_SetAnmFlag( wk->e_pokeicon[i]->act, 0 );		//アニメ停止
			//CLACT_SetDrawFlag( wk->e_pokeicon[i]->act, 0 );
		}
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	ポケモンアイコン削除
 *
 * @param	wk		ROULETTE_SCRWORKのポインタ
 * @param	param1	0=味方、1=相手
 */
//--------------------------------------------------------------
void RouletteScr_DelPokeIcon( ROULETTE_SCRWORK* wk, FMAP_PTR fmap, u16 param1 )
{
	u8 m_max,e_max;
	int i;

	m_max = Roulette_GetMinePokeNum( wk->type, ROULETTE_FLAG_TOTAL );
	e_max = Roulette_GetEnemyPokeNum( wk->type, ROULETTE_FLAG_TOTAL );

	if( param1 == 0 ){		//味方
		for( i=0; i < m_max ;i++ ){	
			FSSC_Sub_PokeIconDel(	fmap, wk->m_pokeicon[i], i );
		}
	}else{					//敵
		for( i=0; i < e_max ;i++ ){	
			FSSC_Sub_PokeIconDel(	fmap, wk->e_pokeicon[i], i+m_max );
		}
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	(味方)ポケモンアイコンバニッシュ操作
 *
 * @param	wk		ROULETTE_SCRWORKのポインタ
 * @param	param1	0=非表示、1=表示
 * @param	param2	何番目か
 */
//--------------------------------------------------------------
void RouletteScr_MineChgPokeIconVanish( ROULETTE_SCRWORK* wk, FMAP_PTR fmap, u16 param1, u16 param2 )
{
	u8 m_max;

	m_max = Roulette_GetMinePokeNum( wk->type, ROULETTE_FLAG_TOTAL );

	//不正な値は何もしない
	if( param2 >= m_max ){
		return;
	}

	if( param1 == 1){
		CLACT_SetDrawFlag( wk->m_pokeicon[param2]->act, 1 );		//表示
	}else{
		CLACT_SetDrawFlag( wk->m_pokeicon[param2]->act, 0 );		//非表示
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	(敵)ポケモンアイコンバニッシュ操作
 *
 * @param	wk		ROULETTE_SCRWORKのポインタ
 * @param	param1	0=非表示、1=表示
 * @param	param2	何番目か
 */
//--------------------------------------------------------------
void RouletteScr_EnemyChgPokeIconVanish( ROULETTE_SCRWORK* wk, FMAP_PTR fmap, u16 param1, u16 param2 )
{
	u8 e_max;

	e_max = Roulette_GetEnemyPokeNum( wk->type, ROULETTE_FLAG_TOTAL );

	//不正な値は何もしない
	if( param2 >= e_max ){
		return;
	}

	if( param1 == 1){
		CLACT_SetDrawFlag( wk->e_pokeicon[param2]->act, 1 );		//表示
	}else{
		CLACT_SetDrawFlag( wk->e_pokeicon[param2]->act, 0 );		//非表示
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	アイテム持っているかアイコン追加
 *
 * @param	wk		ROULETTE_SCRWORKのポインタ
 * @param	param1	0=味方、1=相手
 */
//--------------------------------------------------------------
void RouletteScr_AddItemKeepIcon( ROULETTE_SCRWORK* wk, FMAP_PTR fmap, u16 param1 )
{
	u8 m_max,e_max;
	int i;
	POKEMON_PARAM* pp;

	m_max = Roulette_GetMinePokeNum( wk->type, ROULETTE_FLAG_TOTAL );
	e_max = Roulette_GetEnemyPokeNum( wk->type, ROULETTE_FLAG_TOTAL );

	if( param1 == 0 ){		//味方
		for( i=0; i < m_max ;i++ ){	
			wk->m_itemkeep[i] = Frontier_ItemIconAdd( fmap, 
										mine_icon_pos_tbl[i].x + ITEM_KEEP_OFFSET_X, 
										mine_icon_pos_tbl[i].y + ITEM_KEEP_OFFSET_Y );

			//持っていない時は非表示
			pp =  PokeParty_GetMemberPointer( wk->p_m_party, i );
			if( PokeParaGet(pp,ID_PARA_item,NULL) == 0 ){
				CLACT_SetDrawFlag( wk->m_itemkeep[i]->act, 0 );
			}
		}
	}else{					//敵
		for( i=0; i < e_max ;i++ ){	
			wk->e_itemkeep[i] = Frontier_ItemIconAdd( fmap,
										enemy_icon_pos_tbl[i].x + ITEM_KEEP_OFFSET_X, 
										enemy_icon_pos_tbl[i].y + ITEM_KEEP_OFFSET_Y );

			//持っていない時は非表示
			pp =  PokeParty_GetMemberPointer( wk->p_e_party, i );
			if( PokeParaGet(pp,ID_PARA_item,NULL) == 0 ){
				CLACT_SetDrawFlag( wk->e_itemkeep[i]->act, 0 );
			}
		}
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	アイテム持っているかアイコン削除
 *
 * @param	wk		ROULETTE_SCRWORKのポインタ
 * @param	param1	0=味方、1=相手
 */
//--------------------------------------------------------------
void RouletteScr_DelItemKeepIcon( ROULETTE_SCRWORK* wk, FMAP_PTR fmap, u16 param1 )
{
	u8 m_max,e_max;
	int i;

	m_max = Roulette_GetMinePokeNum( wk->type, ROULETTE_FLAG_TOTAL );
	e_max = Roulette_GetEnemyPokeNum( wk->type, ROULETTE_FLAG_TOTAL );

	if( param1 == 0 ){		//味方
		for( i=0; i < m_max ;i++ ){	
			FSSC_Sub_ItemIconDel(	fmap, wk->m_itemkeep[i] );
		}
	}else{					//敵
		for( i=0; i < e_max ;i++ ){	
			FSSC_Sub_ItemIconDel(	fmap, wk->e_itemkeep[i] );
		}
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	アイテム持っているかアイコンのバニッシュ操作
 *
 * @param	wk		ROULETTE_SCRWORKのポインタ
 * @param	param1	0=味方、1=相手
 * @param	param2	何番目か
 * @param	param3	1=表示、0=非表示
 */
//--------------------------------------------------------------
void RouletteScr_ChgItemKeepIconVanish( ROULETTE_SCRWORK* wk, FMAP_PTR fmap, u16 param1, u16 param2, u16 param3 )
{
	u32 item;
	u8 m_max,e_max;
	POKEMON_PARAM* pp;

	m_max = Roulette_GetMinePokeNum( wk->type, ROULETTE_FLAG_TOTAL );
	e_max = Roulette_GetEnemyPokeNum( wk->type, ROULETTE_FLAG_TOTAL );

	if( param1 == 0 ){		//味方

		//不正な値は何もしない
		if( param2 >= m_max ){
			return;
		}

		pp =  PokeParty_GetMemberPointer( wk->p_m_party, param2 );
		item = PokeParaGet( pp, ID_PARA_item, NULL );

		//表示させたい
		if( param3 == 1 ){
			if( item == 0 ){
				CLACT_SetDrawFlag( wk->m_itemkeep[param2]->act, 0 );		//非表示
			}else{
				CLACT_SetDrawFlag( wk->m_itemkeep[param2]->act, 1 );		//表示
			}

		//非表示にさせたい
		}else{
			CLACT_SetDrawFlag( wk->m_itemkeep[param2]->act, 0 );			//非表示
		}

	}else{					//敵

		//不正な値は何もしない
		if( param2 >= e_max ){
			return;
		}

		pp =  PokeParty_GetMemberPointer( wk->p_e_party, param2 );
		item = PokeParaGet( pp, ID_PARA_item, NULL );

		//表示させたい
		if( param3 == 1 ){
			if( item == 0 ){
				CLACT_SetDrawFlag( wk->e_itemkeep[param2]->act, 0 );		//非表示
			}else{
				CLACT_SetDrawFlag( wk->e_itemkeep[param2]->act, 1 );		//表示
			}

		//非表示にしたい
		}else{
			CLACT_SetDrawFlag( wk->e_itemkeep[param2]->act, 0 );			//非表示
		}
	}

	return;
}

//----------------------------------------------------------------------------
/**
 *	@brief	カラーパレット変更(拡張パレット用)
 *
 *	@param	evy		evy
 */
//-----------------------------------------------------------------------------
#define ROULETTE_EX_PAL_SIZE		(0x1000)
#define ROULETTE_WAY_EX_PAL_NUM		(16)				//16本
#define ROULETTE_BTL_EX_PAL_NUM		(9)					//9本
#define ROULETTE_ROUKA_EX_PAL_NUM	(15)				//15本

void RouletteScr_ResColorChange( u8 evy, u8 flag )
{
	u32 index,size;
	NNSG2dPaletteData *palData;
	void *p_work;
	ARCHANDLE *hdl_bg;
	u16* dest;

	//通路、対戦部屋でパレットが違うので、パレットデータと"転送サイズ"を分ける
	if( flag == 0 ){
		index = BR_ROUKA_NCLR;

#if 0
		//(256(16*16)カラー*16本) = 0x1000 = 4096 * 2 = 8196 = 0x2000
		size  = (ROULETTE_EX_PAL_SIZE * 2);
#else
		size  = (ROULETTE_ROUKA_EX_PAL_NUM * 16) * 2;
#endif
	}else{
		index = BR_ROOM_NCLR;

		//256色をフルに使っていないため。
		size  = (ROULETTE_BTL_EX_PAL_NUM * 16) * 2;
	}

	//データ作成先
	dest = sys_AllocMemory( HEAPID_FRONTIERMAP, ROULETTE_EX_PAL_SIZE*2 );
	memset( dest, 0, ROULETTE_EX_PAL_SIZE*2 );

	hdl_bg = ArchiveDataHandleOpen( ARC_FRONTIER_BG, HEAPID_FRONTIERMAP );
	p_work = ArcUtil_HDL_PalDataGet(hdl_bg, index, &palData, HEAPID_FRONTIERMAP );

	//OS_Printf( "pal size = %d\n", palData->szByte );			//8192 = 0x2000
	
	//256色x16パレット 8k = 8192
	SoftFade( palData->pRawData, dest, ROULETTE_EX_PAL_SIZE, evy, 0x0000 );

	//512byte = 16色 x 16
	//512byte = 256色 x 1
	//8kbyte  = 256色 * 16

	//内部でDMA転送を行うので、前もって転送元のデータをDC_FlushRange等を用いて
	//メインメモリに書き出しておく必要があります。
	//DC_FlushRange(palData->pRawData, palData->szByte);
	DC_FlushRange( dest, ROULETTE_EX_PAL_SIZE*2 );

	GX_BeginLoadBGExtPltt();	   // パレットデータの転送準備

	//GX_LoadBGExtPltt(palData->pRawData, 0x6000, ROULETTE_EX_PAL_SIZE*2);
	GX_LoadBGExtPltt( dest, 0x6000, size );

	GX_EndLoadBGExtPltt();         // パレットデータの転送完了

	ArchiveDataHandleClose( hdl_bg );
	sys_FreeMemoryEz( dest );
	sys_FreeMemoryEz(p_work);
	return;
}


//==============================================================================================
//
//	通信(CommStart)
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	送信ウェイト　
 *
 * @param	wk			ROULETTE_SCRWORK型のポインタ
 * @param	type		送信タイプ
 *
 * @retval	none
 */
//--------------------------------------------------------------
BOOL RouletteScr_CommSetSendBuf( ROULETTE_SCRWORK* wk, u16 type, u16 param )
{
	int ret;

	switch( type ){

	case ROULETTE_COMM_BASIC:
		ret = CommRouletteSendBufBasicData( wk );
		break;

	case ROULETTE_COMM_TR:
		ret = CommRouletteSendBufTrData( wk );
		break;

	case ROULETTE_COMM_ENEMY:
		ret = CommRouletteSendBufEnemyPokeData( wk );
		break;

	case ROULETTE_COMM_RETIRE:
		ret = CommRouletteSendBufRetireFlag( wk, param );
		break;

	case ROULETTE_COMM_TRADE_YESNO:
		ret = CommRouletteSendBufTradeYesNoFlag( wk, param );
		break;

	case ROULETTE_COMM_TEMOTI:
		ret = CommRouletteSendBufTemotiPokeData( wk );
		break;
	};

	return ret;
}


//==============================================================================================
//
//	パネルイベント処理
//
//==============================================================================================

//==============================================================================
//
//	道具リスト(周回数によって使用するテーブルをかえる)
//
//	・3(4)匹全てに持たせる
//	・すでに何か持っている時は、強制上書きする
//
//==============================================================================
//周回数1-3
static const u16 roulette_item_1_3[] = {
	ITEM_OUZYANOSIRUSI,
	ITEM_SENSEINOTUME,
	ITEM_HIKARINOKONA,
	ITEM_METORONOOMU,
	ITEM_OOKINANEKKO,
	///////////////////////////////////
	ITEM_KIAINOHATIMAKI,
	ITEM_KIAINOTASUKI,
	ITEM_TABENOKOSI,
	///////////////////////////////////
	ITEM_KUROITEKKYUU,
	ITEM_KOUKOUNOSIPPO,
	ITEM_KUROIHEDORO,
};
#define ROULETTE_ITEM_1_3_MAX	( NELEMS(roulette_item_1_3) )

//周回数4-6
static const u16 roulette_item_4_6[] = {
	ITEM_SIROIHAABU,
	ITEM_PAWAHURUHAABU,
	ITEM_TIKARANOHATIMAKI,
	ITEM_MONOSIRIMEGANE,
	ITEM_KAIGARANOSUZU,
	///////////////////////////////////
	ITEM_PINTORENZU,
	ITEM_KOUKAKURENZU,
	ITEM_FOOKASURENZU,
	ITEM_TATUZINNOOBI,
	ITEM_INOTINOTAMA,
	///////////////////////////////////
	ITEM_KUROITEKKYUU,
	ITEM_KOUKOUNOSIPPO,
	ITEM_KUROIHEDORO,
};
#define ROULETTE_ITEM_4_6_MAX	( NELEMS(roulette_item_4_6) )

//周回数7
static const u16 roulette_item_7[] = {
	ITEM_KIAINOHATIMAKI,
	ITEM_KIAINOTASUKI,
	ITEM_TABENOKOSI,
	///////////////////////////////////
	ITEM_PINTORENZU,
	ITEM_KOUKAKURENZU,
	ITEM_FOOKASURENZU,
	ITEM_TATUZINNOOBI,
	ITEM_INOTINOTAMA,
	///////////////////////////////////
	ITEM_KODAWARIHATIMAKI,
	ITEM_KODAWARIMEGANE,
	ITEM_KODAWARISUKAAHU,
};
#define ROULETTE_ITEM_7_MAX	( NELEMS(roulette_item_7) )

//テーブル
static const u16* roulette_item_tbl[] = {
	roulette_item_1_3,
	roulette_item_4_6,
	roulette_item_7,
};
#define ROULETTE_ITEM_TBL_MAX	( NELEMS(roulette_item_tbl) )


//==============================================================================
//
//	木の実リスト(周回数によって使用するテーブルをかえる)
//
//	・3(4)匹全てに持たせる
//	・すでに何か持っている時は、強制上書きする
//
//==============================================================================
//周回数1-3
static const u16 roulette_seed_1_3[] = {
	ITEM_KURABONOMI,
	ITEM_KAGONOMI,
	ITEM_MOMONNOMI,
	ITEM_TIIGONOMI,
	ITEM_NANASINOMI,
	///////////////////////////////////
	ITEM_KIINOMI,
	ITEM_OBONNOMI,
	ITEM_RAMUNOMI,
};
#define ROULETTE_SEED_1_3_MAX	( NELEMS(roulette_seed_1_3) )

//周回数4-6
static const u16 roulette_seed_4_6[] = {
	///////////////////////////////////
	ITEM_OKKANOMI,
	ITEM_ITOKENOMI,
	ITEM_SOKUNONOMI,
	ITEM_RINDONOMI,
	ITEM_YATHENOMI,
	ITEM_YOPUNOMI,
	ITEM_BIAANOMI,
	ITEM_SYUKANOMI,
	ITEM_BAKOUNOMI,
	ITEM_UTANNOMI,
	ITEM_TANGANOMI,
	ITEM_YOROGINOMI,
	ITEM_KASIBUNOMI,
	ITEM_HABANNOMI,
	ITEM_NAMONOMI,
	ITEM_RIRIBANOMI,
	ITEM_HOZUNOMI,
	///////////////////////////////////
	ITEM_KIINOMI,
	ITEM_OBONNOMI,
	ITEM_RAMUNOMI,
};
#define ROULETTE_SEED_4_6_MAX	( NELEMS(roulette_seed_4_6) )

//周回数7
static const u16 roulette_seed_7[] = {
	ITEM_KIINOMI,
	ITEM_OBONNOMI,
	ITEM_RAMUNOMI,
	///////////////////////////////////
	ITEM_TIIRANOMI,
	ITEM_RYUGANOMI,
	ITEM_KAMURANOMI,
	ITEM_YATAPINOMI,
	ITEM_ZUANOMI,
	ITEM_SANNOMI,
	ITEM_SUTAANOMI,
};
#define ROULETTE_SEED_7_MAX	( NELEMS(roulette_seed_7) )

//テーブル
static const u16* roulette_seed_tbl[] = {
	roulette_seed_1_3,
	roulette_seed_4_6,
	roulette_seed_7,
};
#define ROULETTE_SEED_TBL_MAX	( NELEMS(roulette_seed_tbl) )

typedef void (*rouletteFunc)(ROULETTE_SCRWORK* wk,POKEPARTY* party, u8 num);	//関数ポインタ型

//イベント
static void Roulette_EvHpDown( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num );
static void Roulette_EvDoku( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num );
static void Roulette_EvMahi( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num );
static void Roulette_EvYakedo( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num );
static void Roulette_EvNemuri( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num );
static void Roulette_EvKoori( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num );
static void Roulette_EvSeedGet( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num );
static void Roulette_EvItemGet( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num );
static void Roulette_EvLvUp( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num );
static void Roulette_EvPlaceHare( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num );
static void Roulette_EvPlaceAme( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num );
static void Roulette_EvPlaceSunaarasi( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num );
static void Roulette_EvPlaceArare( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num );
static void Roulette_EvPlaceKiri( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num );
static void Roulette_EvPlaceTrick( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num );
static void Roulette_EvPlaceSpeedUp( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num );
static void Roulette_EvPlaceSpeedDown( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num );
static void Roulette_EvPlaceRandom( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num );
static void Roulette_EvExPokeChange( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num );
static void Roulette_EvExBPGet( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num );
static void Roulette_EvExBtlWin( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num );
static void Roulette_EvExSuka( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num );

//イベントテーブル
static void* ev_func_tbl[] = {
	Roulette_EvHpDown,
	Roulette_EvDoku,
	Roulette_EvMahi,
	Roulette_EvYakedo,
	Roulette_EvNemuri,
	Roulette_EvKoori,
	Roulette_EvSeedGet,
	Roulette_EvItemGet,
	Roulette_EvLvUp,
	/////////////////////////////////
	Roulette_EvHpDown,			//9
	Roulette_EvDoku,
	Roulette_EvMahi,
	Roulette_EvYakedo,
	Roulette_EvNemuri,
	Roulette_EvKoori,
	Roulette_EvSeedGet,
	Roulette_EvItemGet,
	Roulette_EvLvUp,
	/////////////////////////////////
	Roulette_EvPlaceHare,		//18
	Roulette_EvPlaceAme,
	Roulette_EvPlaceSunaarasi,
	Roulette_EvPlaceArare,
	Roulette_EvPlaceKiri,
	Roulette_EvPlaceTrick,
	Roulette_EvPlaceSpeedUp,
	Roulette_EvPlaceSpeedDown,
	Roulette_EvPlaceRandom,
	/////////////////////////////////
	Roulette_EvExPokeChange,	//27
	Roulette_EvExBPGet,
	Roulette_EvExBtlWin,
	Roulette_EvExSuka,
	Roulette_EvExBPGet,			//31
};

//--------------------------------------------------------------
/**
 * @brief	イベント実行
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
void Roulette_EvSet( ROULETTE_SCRWORK* wk, u8 no )
{
	u8 max;
	POKEPARTY* party;
	rouletteFunc func;

	if( no < ROULETTE_EV_MINE_START ){
		party = wk->p_e_party;
		max = Roulette_GetEnemyPokeNum( wk->type, ROULETTE_FLAG_TOTAL );
	}else{
		party = wk->p_m_party;
		max = Roulette_GetMinePokeNum( wk->type, ROULETTE_FLAG_TOTAL );
	}

	func = ev_func_tbl[no];
	func( wk, party, max );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	イベント：敵：ＨＰ減少
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_EvHpDown( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num )
{
	int i;
	u32 hpmax,hp;
	POKEMON_PARAM* poke;

	OS_Printf( "イベント：ＨＰ減少\n" );

	for( i=0; i < num ;i++ ){
		poke =  PokeParty_GetMemberPointer( party, i );
		hpmax = PokeParaGet( poke, ID_PARA_hpmax, NULL );		//最大HP

		hp = (hpmax * 1.2);
		hp -= hpmax;		//20%
		hp = (hpmax - hp);

		PokeParaPut( poke, ID_PARA_hp, &hp );
		//PokeParaCalc( poke );
	}
	return;
}

//--------------------------------------------------------------
/**
 * @brief	イベント：敵：毒
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_EvDoku( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num )
{
	int i,ng_count;
	u32 condition,type1,type2,spe;
	POKEMON_PARAM* poke;

	OS_Printf( "イベント：毒\n" );

	ng_count = 0;

	for( i=0; i < num ;i++ ){
		poke =  PokeParty_GetMemberPointer( party, i );

		type1 = PokeParaGet( poke, ID_PARA_type1, NULL );
		type2 = PokeParaGet( poke, ID_PARA_type2, NULL );
		spe	  = PokeParaGet( poke, ID_PARA_speabino, NULL );

		//毒、鋼、免疫はダメ！
		if(	(type1 == POISON_TYPE) || (type2 == POISON_TYPE) ||
			(type1 == METAL_TYPE) || (type2 == METAL_TYPE) || (spe == TOKUSYU_MENEKI) ){
			ng_count++;
		//
		}else{
			condition = CONDITION_DOKU;
			PokeParaPut( poke, ID_PARA_condition, &condition );
			//PokeParaCalc( poke );
		}
	}	

	//全てチェックしている時は、誰も氷にすることが出来なかった
	if( ng_count >= num ){
		wk->poke_safe_flag = 1;
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	イベント：敵：麻痺
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_EvMahi( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num )
{
	int i,ng_count;
	u32 condition,type1,type2,spe;
	POKEMON_PARAM* poke;

	OS_Printf( "イベント：麻痺\n" );

	ng_count = 0;

	for( i=0; i < num ;i++ ){
		poke =  PokeParty_GetMemberPointer( party, i );

		type1 = PokeParaGet( poke, ID_PARA_type1, NULL );
		type2 = PokeParaGet( poke, ID_PARA_type2, NULL );
		spe	  = PokeParaGet( poke, ID_PARA_speabino, NULL );

		//じめん、じゅうなんはダメ！
		if(	(type1 == JIMEN_TYPE) || (type2 == JIMEN_TYPE) || (spe == TOKUSYU_ZYUUNAN) ){
			ng_count++;
		//
		}else{
			condition = CONDITION_MAHI;
			PokeParaPut( poke, ID_PARA_condition, &condition );
			//PokeParaCalc( poke );
		}
	}	

	//全てチェックしている時は、誰も氷にすることが出来なかった
	if( ng_count >= num ){
		wk->poke_safe_flag = 1;
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	イベント：敵：やけど
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_EvYakedo( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num )
{
	int i,ng_count;
	u32 condition,type1,type2,spe;
	POKEMON_PARAM* poke;

	OS_Printf( "イベント：やけど\n" );

	ng_count = 0;

	for( i=0; i < num ;i++ ){
		poke =  PokeParty_GetMemberPointer( party, i );

		type1 = PokeParaGet( poke, ID_PARA_type1, NULL );
		type2 = PokeParaGet( poke, ID_PARA_type2, NULL );
		spe	  = PokeParaGet( poke, ID_PARA_speabino, NULL );

		//炎、みずのベールはダメ！
		if(	(type1 == FIRE_TYPE) || (type2 == FIRE_TYPE) || (spe == TOKUSYU_MIZUNOBEERU) ){
			ng_count++;
		//
		}else{
			condition = CONDITION_YAKEDO;
			PokeParaPut( poke, ID_PARA_condition, &condition );
			//PokeParaCalc( poke );
		}
	}	

	//全てチェックしている時は、誰も氷にすることが出来なかった
	if( ng_count >= num ){
		wk->poke_safe_flag = 1;
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	イベント：敵：ねむり
 *
 * @param	
 *
 * @return	none
 *
 * 仕様変更で対象が1人固定となりました。
 */
//--------------------------------------------------------------
static void Roulette_EvNemuri( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num )
{
	u8 check_pos;
	int i,ng_count;
	u32 condition,spe;
	POKEMON_PARAM* poke;

	OS_Printf( "イベント：眠り\n" );

	ng_count = 0;

	//ポケパーティのどの位置からチェックしていくかを代入
	check_pos = ( wk->rand_pos % num );

	for( i=0; i < num ;i++ ){
		poke =  PokeParty_GetMemberPointer( party, check_pos );
		//OS_Printf( "ポケモンナンバー = %d\n", PokeParaGet(poke,ID_PARA_monsno,NULL) );

		spe	  = PokeParaGet( poke, ID_PARA_speabino, NULL );

		//ふみん、やるきはダメ！
		if(	(spe == TOKUSYU_HUMIN) || (spe == TOKUSYU_YARUKI) ){
			ng_count++;
			check_pos++;
			if( check_pos >= num ){
				check_pos = 0;
			}
		//
		}else{
			//condition = CONDITION_NEMURI;
			condition = ( gf_rand() % 4 + 2 );		//2-5ターン眠りになる
			PokeParaPut( poke, ID_PARA_condition, &condition );
			//PokeParaCalc( poke );
			break;
		}	
	}	

	//全てチェックしている時は、誰も氷にすることが出来なかった
	if( ng_count >= num ){
		wk->poke_safe_flag = 1;
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	イベント：敵：こおり
 *
 * @param	
 *
 * @return	none
 *
 * 仕様変更で対象が1人固定となりました。
 */
//--------------------------------------------------------------
static void Roulette_EvKoori( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num )
{
	u8 check_pos;
	int i,ng_count;
	u32 condition,type1,type2,spe;
	POKEMON_PARAM* poke;

	OS_Printf( "イベント：氷\n" );

	ng_count = 0;

	//ポケパーティのどの位置からチェックしていくかを代入
	check_pos = ( wk->rand_pos % num );

	for( i=0; i < num ;i++ ){
		poke =  PokeParty_GetMemberPointer( party, check_pos );

		type1 = PokeParaGet( poke, ID_PARA_type1, NULL );
		type2 = PokeParaGet( poke, ID_PARA_type2, NULL );
		spe	  = PokeParaGet( poke, ID_PARA_speabino, NULL );

		//こおり、マグマのよろいはダメ！
		if(	(type1 == KOORI_TYPE) || (type2 == KOORI_TYPE) || (spe == TOKUSYU_MAGUMANOYOROI) ){
			ng_count++;
			check_pos++;
			if( check_pos >= num ){
				check_pos = 0;
			}
		//
		}else{
			condition = CONDITION_KOORI;
			PokeParaPut( poke, ID_PARA_condition, &condition );
			//PokeParaCalc( poke );
			break;
		}	
	}	

	//全てチェックしている時は、誰も氷にすることが出来なかった
	if( ng_count >= num ){
		wk->poke_safe_flag = 1;
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	イベント：敵：木の実ゲット
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_EvSeedGet( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num )
{
	int i,max;
	const u16* seed_tbl;
	u16 item,lap;
	POKEMON_PARAM* poke;

	OS_Printf( "イベント：木の実\n" );

	lap = RouletteScr_CommGetLap( wk );

	if( lap < 3 ){
		seed_tbl = roulette_seed_tbl[0];
		max = ROULETTE_SEED_1_3_MAX;
	}else if( lap < 6 ){
		seed_tbl = roulette_seed_tbl[1];
		max = ROULETTE_SEED_4_6_MAX;
	}else{
		seed_tbl = roulette_seed_tbl[2];
		max = ROULETTE_SEED_7_MAX;
	}

	item = seed_tbl[ (wk->rand_pos % max) ];
	OS_Printf( "セットする木の実ナンバー = %d\n", item );

#if 0	//デバック！
	item = ITEM_ORENNOMI;
#endif

	for( i=0; i < num ;i++ ){
		poke =  PokeParty_GetMemberPointer( party, i );
		PokeParaPut( poke, ID_PARA_item, &item );
		//PokeParaCalc( poke );
	}	

	//アイテム持っているかアイコンのバニッシュセット
	//Roulette_ItemKeepVanishSet( wk );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	イベント：敵：アイテムゲット
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_EvItemGet( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num )
{
	int i,max;
	const u16* item_tbl;
	u16 item,lap;
	POKEMON_PARAM* poke;

	OS_Printf( "イベント：アイテム\n" );

	lap = RouletteScr_CommGetLap( wk );

	if( lap < 3 ){
		item_tbl = roulette_item_tbl[0];
		max = ROULETTE_ITEM_1_3_MAX;
	}else if( lap < 6 ){
		item_tbl = roulette_item_tbl[1];
		max = ROULETTE_ITEM_4_6_MAX;
	}else{
		item_tbl = roulette_item_tbl[2];
		max = ROULETTE_ITEM_7_MAX;
	}

	item = item_tbl[ (wk->rand_pos % max) ];
	OS_Printf( "セットするアイテムナンバー = %d\n", item );

	for( i=0; i < num ;i++ ){
		poke =  PokeParty_GetMemberPointer( party, i );
		PokeParaPut( poke, ID_PARA_item, &item );
		//PokeParaCalc( poke );
	}	

	//アイテム持っているかアイコンのバニッシュセット
	//Roulette_ItemKeepVanishSet( wk );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	イベント：敵：LVアップ
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_EvLvUp( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num )
{
	int i;
	u32 exp,level;
	POKEMON_PARAM* poke;

	OS_Printf( "イベント：ＬＶアップ\n" );

	for( i=0; i < num ;i++ ){
		poke =  PokeParty_GetMemberPointer( party, i );
		level = PokeParaGet( poke, ID_PARA_level, NULL );
		level+=3;

		if( level > 100 ){
			OS_Printf( "レベルが100を超えている！\n" );
			GF_ASSERT(0);
			level = 100;
		}

		exp = PokeLevelExpGet( PokeParaGet(poke,ID_PARA_monsno,NULL), level );
		PokeParaPut( poke, ID_PARA_exp, &exp );
		PokeParaCalc( poke );
	}
	return;
}

//--------------------------------------------------------------
/**
 * @brief	イベント：場に効果：はれ
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_EvPlaceHare( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num )
{
	POKEMON_PARAM* poke;
	OS_Printf( "はれ　状態で戦闘へ！\n" );
	wk->weather = WEATHER_SYS_HIGHSUNNY;			//日本晴れ
	return;
}

//--------------------------------------------------------------
/**
 * @brief	イベント：場に効果：あめ
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_EvPlaceAme( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num )
{
	POKEMON_PARAM* poke;
	OS_Printf( "あめ　状態で戦闘へ！\n" );
	wk->weather = WEATHER_SYS_RAIN;					//雨
	return;
}

//--------------------------------------------------------------
/**
 * @brief	イベント：場に効果：すなあらし
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_EvPlaceSunaarasi( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num )
{
	POKEMON_PARAM* poke;
	OS_Printf( "すなあらし　状態で戦闘へ！\n" );
	wk->weather = WEATHER_SYS_SANDSTORM;			//砂嵐
	return;
}

//--------------------------------------------------------------
/**
 * @brief	イベント：場に効果：あられ
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_EvPlaceArare( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num )
{
	POKEMON_PARAM* poke;
	OS_Printf( "あられ　状態で戦闘へ！\n" );
	wk->weather = WEATHER_SYS_SNOW;					//雪
	return;
}

//--------------------------------------------------------------
/**
 * @brief	イベント：場に効果：きり
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_EvPlaceKiri( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num )
{
	POKEMON_PARAM* poke;
	OS_Printf( "きり　状態で戦闘へ！\n" );
	//wk->weather = WEATHER_SYS_FOGS;					//霧
	wk->weather = WEATHER_SYS_MIST1;					//霧
	return;
}

//--------------------------------------------------------------
/**
 * @brief	イベント：場に効果：トリックルーム
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_EvPlaceTrick( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num )
{
	POKEMON_PARAM* poke;
	OS_Printf( "トリックルーム　状態で戦闘へ！\n" );
	wk->weather = WEATHER_SYS_TRICKROOM;			//トリックルーム
	return;
}

//--------------------------------------------------------------
/**
 * @brief	イベント：場に効果：スピードアップ
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_EvPlaceSpeedUp( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num )
{
	POKEMON_PARAM* poke;
	OS_Printf( "スピードアップ\n" );

	if( wk->csr_speed_level < (CSR_WAIT_TBL_MAX - 1) ){
		wk->csr_speed_level++;						//カーソルが動くスピードのレベル
	}
	return;
}

//--------------------------------------------------------------
/**
 * @brief	イベント：場に効果：スピードダウン
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_EvPlaceSpeedDown( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num )
{
	POKEMON_PARAM* poke;
	OS_Printf( "スピードダウン\n" );

	if( wk->csr_speed_level > 0 ){
		wk->csr_speed_level--;			//カーソルが動くスピードのレベル
	}
	return;
}

//--------------------------------------------------------------
/**
 * @brief	イベント：場に効果：ランダム
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_EvPlaceRandom( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num )
{
	POKEMON_PARAM* poke;
	OS_Printf( "ランダム\n" );
	//wk->csr_speed_level = CSR_RANDOM_SPEED;		//カーソルが動くスピードのレベル
	wk->random_flag = 1;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	イベント：特殊：ポケモンチェンジ
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_EvExPokeChange( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num )
{
	POKEMON_PARAM* poke;
	OS_Printf( "ポケモンチェンジ　状態で戦闘へ！\n" );
	//wk->place_poke_change_flag = 1;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	イベント：特殊：BPゲット
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_EvExBPGet( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num )
{
	u16 add_bp;
	POKEMON_PARAM* poke;
	OS_Printf( "BPゲット\n" );

	//専用画面を抜けてからBPを足す
	//TowerScoreData_SetBattlePoint( SaveData_GetTowerScoreData(wk->sv), add_bp, BTWR_DATA_add );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	イベント：特殊：戦闘スルー
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_EvExBtlWin( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num )
{
	POKEMON_PARAM* poke;
	OS_Printf( "戦闘スルー\n" );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	イベント：特殊：スカ
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_EvExSuka( ROULETTE_SCRWORK* wk, POKEPARTY* party, u8 num )
{
	POKEMON_PARAM* poke;
	OS_Printf( "スカ\n" );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * ルーレットクリア時に貰えるバトルポイントを取得
 *
 * @param	wk
 *
 * @return	"貰えるバトルポイント"
 */
//--------------------------------------------------------------------------------------------
u16	RouletteScr_GetAddBtlPoint( ROULETTE_SCRWORK* wk );
u16	RouletteScr_GetAddBtlPoint( ROULETTE_SCRWORK* wk )
{
	u8 add_bp;
	u16 lap;
	static const u8 bppoint_normal[ROULETTE_LAP_MAX+1] = {0, 
													2, 2, 2, 2, 4, 4, 5, 6 };//0,1周～8周以降

	static const u8 bppoint_multi[ROULETTE_LAP_MAX+1] = {0, 
													7, 8, 9, 10, 13, 14, 16, 17 };//0,1周～8周以降

	//lap = RouletteScr_CommGetLap( wk );
	lap = wk->lap;

	OS_Printf( "周回数 = %d\n", lap );
	OS_Printf( "連勝数 = %d\n", wk->rensyou );

	//シングル、ダブル
	if( (wk->type == ROULETTE_TYPE_SINGLE) || (wk->type == ROULETTE_TYPE_DOUBLE) ){

		//最大周回数チェック
		if( lap >= ROULETTE_LAP_MAX ){
			add_bp = bppoint_normal[ ROULETTE_LAP_MAX ];		//テーブルが+1しているので
		}else{
			add_bp = bppoint_normal[ lap ];
		}

	//マルチ、WIFI
	}else{

		//最大周回数チェック
		if( lap >= ROULETTE_LAP_MAX ){
			add_bp = bppoint_multi[ ROULETTE_LAP_MAX ];		//テーブルが+1しているので
		}else{
			add_bp = bppoint_multi[ lap ];
		}

	}

	//ブレーンチェック
	if( wk->type == ROULETTE_TYPE_SINGLE ){
		if( (wk->rensyou == ROULETTE_LEADER_SET_1ST) || (wk->rensyou == ROULETTE_LEADER_SET_2ND) ){
			add_bp = 20;
		}
	}

	return add_bp;
}

//--------------------------------------------------------------------------------------------
/**
 * 戦闘後のアイテム状態をセットする
 *
 * @param	btl_party	戦闘で使用したパーティ
 * @param	party		セットするパーティ
 * @param	btl_pos		戦闘で使用したパーティの位置
 * @param	pos			セットするパーティの位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void RouletteScr_BattleAfterItemSet( POKEPARTY* btl_party, POKEPARTY* party, int btl_pos, int pos );
void RouletteScr_BattleAfterItemSet( POKEPARTY* btl_party, POKEPARTY* party, int btl_pos, int pos )
{
	u16 item;
	POKEMON_PARAM* pp;
	POKEMON_PARAM* btl_pp;

	//バトルで使用したポケモンデータからアイテムを取得
	btl_pp	= PokeParty_GetMemberPointer( btl_party, btl_pos );
	item	= PokeParaGet( btl_pp, ID_PARA_item, NULL );

	//セットするポケモンデータ
	pp =  PokeParty_GetMemberPointer( party, pos );
	PokeParaPut( pp, ID_PARA_item, &item );
	//PokeParaCalc( pp );
	return;
}


