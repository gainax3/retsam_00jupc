//============================================================================================
/**
 * @file	fss_factory_sub.c
 * @bfief	フロンティアシステムスクリプトコマンドサブ：ファクトリー
 * @author	Satoshi Nohara
 * @date	07.04.10
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

#include "application/factory.h"
#include "frontier_tool.h"
#include "frontier_def.h"
#include "factory_tool.h"
#include "fssc_factory_sub.h"
#include "../field/scr_tool.h"		//FactoryScr_GetWinRecordID
#include "itemtool/itemsym.h"
#include "comm_command_frontier.h"

#include "communication/comm_system.h"
#include "communication/comm_tool.h"
#include "communication/comm_def.h"


//============================================================================================
//
//	プロトタイプ宣言
//
//============================================================================================
FACTORY_SCRWORK* FactoryScr_WorkAlloc( SAVEDATA* savedata, u16 init, u8 type, u8 level );
void FactoryScr_WorkInit( FACTORY_SCRWORK* wk, u16 init );
static void FactoryScr_InitDataSet( FACTORY_SCRWORK* wk );
static void FactoryScr_ContinueDataSet( FACTORY_SCRWORK* wk );
void FactoryScr_WorkRelease( FACTORY_SCRWORK* wk );
void FactoryScr_GetResult( FACTORY_SCRWORK* wk, void* factory_call );
u16 FactoryCall_GetRetWork( void* param, u8 pos );
void FactoryScr_RentalPartySet( FACTORY_SCRWORK* wk );
void FactoryScr_BtlAfterPartySet( FACTORY_SCRWORK* wk );
void FactoryScr_TradePokeChange( FACTORY_SCRWORK* wk );
void FactoryScr_TradeAfterPartySet( FACTORY_SCRWORK* wk );
void FactoryScr_TradeCountInc( FACTORY_SCRWORK* wk );
static void FactoryScr_RentalPokePosChg( FACTORY_SCRWORK* wk, u8 pos, u8 flag );

//通信
BOOL FactoryScr_CommSetSendBuf( FACTORY_SCRWORK* wk, u16 type, u16 param );


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
 * ＊かならず FactoryScr_WorkRelease()で領域を開放すること
 */
//--------------------------------------------------------------
FACTORY_SCRWORK* FactoryScr_WorkAlloc( SAVEDATA* savedata, u16 init, u8 type, u8 level )
{
	FACTORYDATA* fa_sv;
	FACTORYSCORE* score_sv;
	const FACTORY_POKE_RANGE* poke_range;
	u8 clear_flag;
	u16	i,chg_flg;
	u32 clear_id;
	static FACTORY_SCRWORK* wk;					//あとで確認]]]]]]]]]]]]]]]]]]]]]]]]]

	wk = sys_AllocMemory( HEAPID_WORLD, sizeof(FACTORY_SCRWORK) );
	MI_CpuClear8( wk, sizeof(FACTORY_SCRWORK) );

	//セーブデータ取得
	wk->factory_savedata= SaveData_GetFactoryData( savedata );
	wk->sv				= savedata;
	wk->heapID			= HEAPID_WORLD;
	wk->p_m_party		= PokeParty_AllocPartyWork( HEAPID_WORLD );
	wk->p_e_party		= PokeParty_AllocPartyWork( HEAPID_WORLD );
	
	fa_sv = wk->factory_savedata;
	score_sv= SaveData_GetFactoryScore( savedata );

	//新規か、継続か
	if( init == 0 ){
		wk->type	= type;
		wk->level	= level;
		wk->round	= 0;

		FACTORYDATA_Init( fa_sv );	//中断データ初期化

		//WIFIのみ特殊
		if( wk->type == FACTORY_TYPE_WIFI_MULTI ){

			if( wk->level == FACTORY_LEVEL_50 ){
				clear_id = FRID_FACTORY_MULTI_WIFI_CLEAR_BIT;
			}else{
				clear_id = FRID_FACTORY_MULTI_WIFI_CLEAR100_BIT;
			}

#if 0
			clear_flag = FrontierRecord_Get(SaveData_GetFrontier(wk->sv), 
											clear_id,
											Frontier_GetFriendIndex(clear_id) );
#else
			clear_flag = SysWork_WifiFrClearFlagGet( SaveData_GetEventWork(wk->sv) );
#endif

		}else{
			//クリアしたかフラグを取得
			clear_flag = (u8)FACTORYSCORE_GetScoreData(	score_sv, FACTORYSCORE_ID_CLEAR_FLAG, 
													(wk->level*FACTORY_TYPE_MAX)+wk->type, NULL );
		}

		if( clear_flag == 1 ){
			wk->rensyou = FrontierRecord_Get(	SaveData_GetFrontier(wk->sv), 
						FactoryScr_GetWinRecordID(wk->level,wk->type),
						Frontier_GetFriendIndex(FactoryScr_GetWinRecordID(wk->level,wk->type)) );
			wk->trade_count = FrontierRecord_Get(	SaveData_GetFrontier(wk->sv), 
						FactoryScr_GetTradeRecordID(wk->level,wk->type),
						Frontier_GetFriendIndex(FactoryScr_GetTradeRecordID(wk->level,wk->type)) );
		}else{
			wk->rensyou = 0;
			wk->trade_count = 0;
		}

		//レンタルするだけでも交換回数が1回増える
		//FactoryScr_TradeCountInc( wk );				//交換回数+1

		wk->win_cnt	= 0;
	}else{
		//現在のプレイ進行データ取得
		wk->type = (u8)FACTORYDATA_GetPlayData( fa_sv, FACTORYDATA_ID_TYPE, 0, NULL);
		wk->level = (u8)FACTORYDATA_GetPlayData( fa_sv, FACTORYDATA_ID_LEVEL,0,NULL);
		wk->round = (u8)FACTORYDATA_GetPlayData( fa_sv, FACTORYDATA_ID_ROUND,0,NULL);
		wk->rensyou = FrontierRecord_Get(	SaveData_GetFrontier(wk->sv), 
						FactoryScr_GetWinRecordID(wk->level,wk->type),
						Frontier_GetFriendIndex(FactoryScr_GetWinRecordID(wk->level,wk->type)) );
		wk->trade_count = FrontierRecord_Get(	SaveData_GetFrontier(wk->sv), 
						FactoryScr_GetTradeRecordID(wk->level,wk->type),
						Frontier_GetFriendIndex(FactoryScr_GetTradeRecordID(wk->level,wk->type)) );
	}

	wk->lap		= (u16)(wk->rensyou / FACTORY_LAP_ENEMY_MAX);

	OS_Printf( "____alloc trade_count = %d\n", wk->trade_count );
	OS_Printf( "____alloc ensyou = %d\n", wk->rensyou );
	OS_Printf( "____alloc lap = %d\n", wk->lap );

	//地球儀登録
	if( Factory_CommCheck(wk->type) == TRUE ){
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
 * ＊かならず FactoryScr_WorkRelease()で領域を開放すること
 */
//--------------------------------------------------------------
void FactoryScr_WorkInit( FACTORY_SCRWORK* wk, u16 init )
{
	//新規か、継続か
	if( init == 0 ){
		//OS_Printf( "前 新規 問題のwk = %d\n", wk );
		FactoryScr_InitDataSet( wk );
		//OS_Printf( "後 新規 問題のwk = %d\n", wk );
	}else{
		//OS_Printf( "前 継続 問題のwk = %d\n", wk );
		FactoryScr_ContinueDataSet( wk );
		//OS_Printf( "後 継続 問題のwk = %d\n", wk );
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	新規：トレーナー、ポケモンデータを生成
 *
 * @param	wk		FACTORY_SCRWORKへのポインタ
 */
//--------------------------------------------------------------
static void FactoryScr_InitDataSet( FACTORY_SCRWORK* wk )
{
	int i,check_count;
	B_TOWER_POKEMON check_poke[FACTORY_RENTAL_POKE_MAX*2];	//通信用に2倍用意
	B_TOWER_POKEMON poke;
	POKEMON_PARAM* temp_poke;
	u16 poke_check_tbl[FACTORY_RENTAL_POKE_MAX];
	u16 item_check_tbl[FACTORY_RENTAL_POKE_MAX];

	OS_Printf( "新規：データを生成\n" );

	//07.09.25 常に周回数*2のトレーナーを取得
	//シングル(0-6)、マルチのパートナー(7-13)で取得
	//その周に登場するトレーナーのindexを全て取得
	Factory_EnemyLapAllTrainerIndexGet( wk->type, FactoryScr_CommGetLap(wk), wk->tr_index,
										FACTORY_LAP_MULTI_ENEMY_MAX );
	OS_Printf( "lap = %d\n", FactoryScr_CommGetLap(wk) );

	//デバック情報
	for( i=0; i < FACTORY_LAP_MULTI_ENEMY_MAX ;i++ ){
		//OS_Printf( "tr_index[%d] = %d\n", i, wk->tr_index[i] );
	}

	//レンタルポケモン生成
	Factory_RentalPokeMake( FactoryScr_CommGetLap(wk), wk->level, 
							wk->rental_poke_index, 
							wk->rental_poke,					//B_TOWER_POKEMON
							wk->rental_pow_rnd,
							wk->rental_personal_rnd,
							wk->trade_count,
							NULL, NULL );

#if 1	//rental_poke_pos_chg
	//6匹のうち、後ろから2匹の位置をランダム移動される
	FactoryScr_RentalPokePosChg( wk, 4, 0 );
	FactoryScr_RentalPokePosChg( wk, 5, 0 );
#endif

	//コピー
	check_count = FACTORY_RENTAL_POKE_MAX;
	for( i=0; i < FACTORY_RENTAL_POKE_MAX ;i++ ){
		check_poke[i] = wk->rental_poke[i];
	}

	for( i=0; i < FACTORY_RENTAL_POKE_MAX ;i++ ){
		OS_Printf( "rental_poke[%d] = %d\n", i, wk->rental_poke[i].mons_no );
		OS_Printf( "rental_index[%d] = %d\n", i, wk->rental_poke_index[i] );
		OS_Printf( "rental_pow[%d] = %d\n", i, wk->rental_pow_rnd[i] );
		OS_Printf( "rental_personal[%d] = %d\n", i, wk->rental_personal_rnd[i] );
	}

	//通信の時
	if( Factory_CommCheck(wk->type) == TRUE ){

		//チェックテーブル作成
		for( i=0; i < FACTORY_RENTAL_POKE_MAX ;i++ ){
#if 0
			poke = *(B_TOWER_POKEMON*)(&wk->rental_poke[i]);
			poke_check_tbl[i] = poke.mons_no;
			item_check_tbl[i] = poke.item_no;
#else
			poke_check_tbl[i] = wk->rental_poke[i].mons_no;
			item_check_tbl[i] = wk->rental_poke[i].item_no;
#endif
		}

		Factory_RentalPokeMake( FactoryScr_CommGetLap(wk), wk->level, 
								wk->pair_rental_poke_index, 
								wk->pair_rental_poke,			//B_TOWER_POKEMON
								wk->pair_rental_pow_rnd,
								wk->pair_rental_personal_rnd,
								wk->pair_trade_count,
								poke_check_tbl, item_check_tbl );

#if 1	//rental_poke_pos_chg
		//6匹のうち、後ろから2匹の位置をランダム移動される
		FactoryScr_RentalPokePosChg( wk, 4, 1 );
		FactoryScr_RentalPokePosChg( wk, 5, 1 );
#endif

		//コピー
		check_count = ( FACTORY_RENTAL_POKE_MAX * 2 );
		for( i=0; i < FACTORY_RENTAL_POKE_MAX ;i++ ){
			check_poke[i+FACTORY_RENTAL_POKE_MAX] = wk->pair_rental_poke[i];
		}

		for( i=0; i < FACTORY_RENTAL_POKE_MAX ;i++ ){
			OS_Printf( "pair_rental_poke[%d] = %d\n", i, wk->pair_rental_poke[i].mons_no );
			OS_Printf( "pair_rental_index[%d] = %d\n", i, wk->pair_rental_poke_index[i] );
			OS_Printf( "pair_rental_pow[%d] = %d\n", i, wk->pair_rental_pow_rnd[i] );
			OS_Printf( "pair_rental_personal[%d] = %d\n", i, wk->pair_rental_personal_rnd[i] );
		}
	}

	//OS_Printf( "222wk = %d\n", wk );
	//OS_Printf( "222wk->p_party = %d\n", wk->p_party );

	//敵トレーナーが出すポケモン生成(通信でペアのレンタルポケモンの被りも考慮)
#if 0	
	Factory_EnemyPokeMake(	Factory_GetEnemyPokeNum(wk->type,FACTORY_FLAG_TOTAL),
							wk->tr_index[wk->round], wk->level, 
							wk->rental_poke,					//B_TOWER_POKEMON
							wk->enemy_poke_index, 
							wk->enemy_poke,						//B_TOWER_POKEMON
							wk->enemy_pow_rnd,
							wk->enemy_personal_rnd );
#else
	Factory_EnemyPokeMake(	Factory_GetEnemyPokeNum(wk->type,FACTORY_FLAG_TOTAL),
							wk->tr_index[wk->round], wk->level, 
							check_poke,							//B_TOWER_POKEMON
							wk->enemy_poke_index, 
							wk->enemy_poke,						//B_TOWER_POKEMON
							wk->enemy_pow_rnd,
							wk->enemy_personal_rnd,
							check_count );
#endif

	for( i=0; i < 4 ;i++ ){
		OS_Printf( "enemy_poke[%d] = %d\n", i, wk->enemy_poke[i].mons_no );
		OS_Printf( "enemy_index[%d] = %d\n", i, wk->enemy_poke_index[i] );
		OS_Printf( "enemy_pow[%d] = %d\n", i, wk->enemy_pow_rnd[i] );
		OS_Printf( "enemy_personal[%d] = %d\n", i, wk->enemy_personal_rnd[i] );
	}

	//OS_Printf( "333wk = %d\n", wk );
	//OS_Printf( "333wk->p_party = %d\n", wk->p_party );

	for( i=0; i < FACTORY_RENTAL_POKE_MAX ;i++ ){			//レンタル6匹
		temp_poke = PokemonParam_AllocWork( HEAPID_WORLD );
		Frontier_PokeParaMake( &wk->rental_poke[i], temp_poke, Factory_GetLevel(wk) );
		Frontier_PokePartyAdd( wk->sv, wk->p_m_party, temp_poke );
		sys_FreeMemoryEz( temp_poke );
	}

	//情報表示
	OS_Printf( "ポケモンパーティ数 = %d\n",	PokeParty_GetPokeCount(wk->p_m_party) );
	for( i=0; i < FACTORY_PARTY_POKE_MAX; i++ ){
		temp_poke = PokeParty_GetMemberPointer( wk->p_m_party, i );
		OS_Printf( "新規party[%d] monsno = %d\n", i, PokeParaGet(temp_poke,ID_PARA_monsno,NULL) );
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	レンタルポケモンの位置をランダムで入れ替え
 *
 * @param	wk		FACTORY_SCRWORKへのポインタ
 * @param	pos		入れ替える位置
 * @param	flag	0=自分、1=ペア
 */
//--------------------------------------------------------------
static void FactoryScr_RentalPokePosChg( FACTORY_SCRWORK* wk, u8 pos, u8 flag )
{
	u16 rand;
	u16 temp_rental_poke_index;											//ポケモンindexテーブル
	u8	temp_rental_pow_rnd;											//ポケモンのパワー乱数
	u32 temp_rental_personal_rnd;										//ポケモンの個性乱数
	B_TOWER_POKEMON temp_rental_poke;

	rand = ( gf_rand() % FACTORY_RENTAL_POKE_MAX );
	OS_Printf( "入れ替える場所 = %d → %d\n", pos, rand );

	//自分用
	if( flag == 0 ){

		//[pos]をtempへ退避
		temp_rental_poke_index			= wk->rental_poke_index[pos];		//ポケモンindexテーブル
		temp_rental_pow_rnd				= wk->rental_pow_rnd[pos];			//ポケモンのパワー乱数
		temp_rental_personal_rnd		= wk->rental_personal_rnd[pos];		//ポケモンの個性乱数
		temp_rental_poke				= wk->rental_poke[pos];

		//[ランダム]を[pos]へ
		wk->rental_poke_index[pos]		= wk->rental_poke_index[rand];		//ポケモンindexテーブル
		wk->rental_pow_rnd[pos]			= wk->rental_pow_rnd[rand];			//ポケモンのパワー乱数
		wk->rental_personal_rnd[pos]	= wk->rental_personal_rnd[rand];	//ポケモンの個性乱数
		wk->rental_poke[pos]			= wk->rental_poke[rand];

		//退避したtempをランダムな位置へ
		wk->rental_poke_index[rand]		= temp_rental_poke_index;			//ポケモンindexテーブル
		wk->rental_pow_rnd[rand]		= temp_rental_pow_rnd;				//ポケモンのパワー乱数
		wk->rental_personal_rnd[rand]	= temp_rental_personal_rnd;			//ポケモンの個性乱数
		wk->rental_poke[rand]			= temp_rental_poke;

	//////////////////////////////////////////////////////////////////////////////////////////////
	//ペア用
	}else{

		//[pos]をtempへ退避
		temp_rental_poke_index			= wk->pair_rental_poke_index[pos];	//ポケモンindexテーブル
		temp_rental_pow_rnd				= wk->pair_rental_pow_rnd[pos];		//ポケモンのパワー乱数
		temp_rental_personal_rnd		= wk->pair_rental_personal_rnd[pos];//ポケモンの個性乱数
		temp_rental_poke				= wk->pair_rental_poke[pos];

		//[ランダム]を[pos]へ
		wk->pair_rental_poke_index[pos]	= wk->pair_rental_poke_index[rand];	//ポケモンindexテーブル
		wk->pair_rental_pow_rnd[pos]	= wk->pair_rental_pow_rnd[rand];	//ポケモンのパワー乱数
		wk->pair_rental_personal_rnd[pos]= wk->pair_rental_personal_rnd[rand];//ポケモンの個性乱数
		wk->pair_rental_poke[pos]		= wk->pair_rental_poke[rand];

		//退避したtempをランダムな位置へ
		wk->pair_rental_poke_index[rand]= temp_rental_poke_index;			//ポケモンindexテーブル
		wk->pair_rental_pow_rnd[rand]	= temp_rental_pow_rnd;				//ポケモンのパワー乱数
		wk->pair_rental_personal_rnd[rand]= temp_rental_personal_rnd;		//ポケモンの個性乱数
		wk->pair_rental_poke[rand]		= temp_rental_poke;
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	継続：保存しておいたデータを使ってB_TOWER_POKEMONなどを生成
 *
 * @param	wk		FACTORY_SCRWORKへのポインタ
 */
//--------------------------------------------------------------
static void FactoryScr_ContinueDataSet( FACTORY_SCRWORK* wk )
{
	int i;
	POKEMON_PARAM* temp_poke;
	u8 count,m_max;
	B_TOWER_POKEMON bt_poke[FACTORY_PARTY_POKE_MAX];
	u8 pow_rnd[FACTORY_PARTY_POKE_MAX];
	u16 poke_index[FACTORY_PARTY_POKE_MAX];
	u32 personal_rnd[FACTORY_PARTY_POKE_MAX];
	OS_Printf( "継続：保存しておいたデータをロード\n" );

	//タイプによってポケモンの数を取得
	m_max = Factory_GetMinePokeNum( wk->type );							//自分

	//-----------------------------------------------------------------------------------
	
	//保存しておいたトレーナーのindex取得
	for( i=0; i < FACTORY_LAP_MULTI_ENEMY_MAX ;i++ ){
		wk->tr_index[i] = (u16)FACTORYDATA_GetPlayData(	wk->factory_savedata, 
														FACTORYDATA_ID_TR_INDEX, i, NULL );
		//OS_Printf( "tr_index[%d] = %d\n", i, wk->tr_index[i] );
	}

	//---------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------
	//保存しておいた手持ちポケモン取得
	for( i=0; i < FACTORY_MINE_POKE_MAX ;i++ ){

		poke_index[i] = (u16)FACTORYDATA_GetPlayData(wk->factory_savedata, 
												FACTORYDATA_ID_TEMOTI_POKE_INDEX, i, NULL );
		//OS_Printf( "poke_index[%d] = %d\n", i, poke_index[i] );
		
		personal_rnd[i] = (u32)FACTORYDATA_GetPlayData(wk->factory_savedata, 
												FACTORYDATA_ID_TEMOTI_PERSONAL_RND, i, NULL );
		OS_Printf( "personal_rnd[%d] = %d\n", i, personal_rnd[i] );
		
		pow_rnd[i] = (u8)FACTORYDATA_GetPlayData(wk->factory_savedata, 
												FACTORYDATA_ID_TEMOTI_POW_RND, i, NULL );
		//OS_Printf( "pow_rnd[%d] = %d\n", i, pow_rnd[i] );
		
		//手持ちにポケモンのインデックス格納
		wk->temoti_poke_index[i] = poke_index[i];
	}

	//手持ちポケモンを一度に生成(personalあり)
	Frontier_PokemonParamCreateAll(	bt_poke, poke_index, pow_rnd,
									//NULL, personal_rnd, FACTORY_MINE_POKE_MAX, HEAPID_WORLD );
									personal_rnd, NULL, FACTORY_MINE_POKE_MAX, HEAPID_WORLD,
									ARC_PL_BTD_PM );

	temp_poke = PokemonParam_AllocWork( HEAPID_WORLD );
	for( i=0; i < FACTORY_MINE_POKE_MAX ;i++ ){
		//バトルタワー用ポケモンデータからPOKEMON_PARAMを生成
		Frontier_PokeParaMake( &bt_poke[i], temp_poke, Factory_GetLevel(wk) );
		Frontier_PokePartyAdd( wk->sv, wk->p_m_party, temp_poke );
		OS_Printf( "継続m_party[%d] monsno = %d\n", i, PokeParaGet(temp_poke,ID_PARA_monsno,NULL) );
	}
	sys_FreeMemoryEz( temp_poke );

	//---------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------
	//保存しておいた敵ポケモン取得
	for( i=0; i < FACTORY_ENEMY_POKE_MAX ;i++ ){

		poke_index[i] = (u16)FACTORYDATA_GetPlayData(wk->factory_savedata, 
												FACTORYDATA_ID_ENEMY_POKE_INDEX, i, NULL );
		//OS_Printf( "poke_index[%d] = %d\n", i, poke_index[i] );
		
		personal_rnd[i] = (u32)FACTORYDATA_GetPlayData(wk->factory_savedata, 
												FACTORYDATA_ID_ENEMY_PERSONAL_RND, i, NULL );
		//OS_Printf( "personal_rnd[%d] = %d\n", i, personal_rnd[i] );
		
		pow_rnd[i] = (u8)FACTORYDATA_GetPlayData(wk->factory_savedata, 
												FACTORYDATA_ID_ENEMY_POW_RND, i, NULL );
		//OS_Printf( "pow_rnd[%d] = %d\n", i, pow_rnd[i] );
		
		//敵ポケモンのインデックス格納
		wk->enemy_poke_index[i] = poke_index[i];
	}

	//手持ちポケモンを一度に生成(personalあり)
	Frontier_PokemonParamCreateAll(	bt_poke, poke_index, pow_rnd,
									//NULL, personal_rnd, FACTORY_ENEMY_POKE_MAX, HEAPID_WORLD );
									personal_rnd, NULL, FACTORY_ENEMY_POKE_MAX, HEAPID_WORLD,
									ARC_PL_BTD_PM );

	temp_poke = PokemonParam_AllocWork( HEAPID_WORLD );
	for( i=0; i < FACTORY_ENEMY_POKE_MAX ;i++ ){
		//バトルタワー用ポケモンデータからPOKEMON_PARAMを生成
		Frontier_PokeParaMake( &bt_poke[i], temp_poke, Factory_GetLevel(wk) );
		Frontier_PokePartyAdd( wk->sv, wk->p_e_party, temp_poke );
		OS_Printf( "継続e_party[%d] monsno = %d\n", i, PokeParaGet(temp_poke,ID_PARA_monsno,NULL) );

	}
	sys_FreeMemoryEz( temp_poke );

	return;
}

//--------------------------------------------------------------
/**
 * @brief	ワークエリアを開放する
 */
//--------------------------------------------------------------
void FactoryScr_WorkRelease( FACTORY_SCRWORK* wk )
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

	MI_CpuClear8( wk, sizeof(FACTORY_SCRWORK) );
	sys_FreeMemoryEz( wk );
	wk = NULL;

	return;
}

//--------------------------------------------------------------
/**
 * @brief	ファクトリー画面呼び出し後の結果取得
 *
 * @param	wk		FACTORY_SCRWORKのポインタ
 */
//--------------------------------------------------------------
void FactoryScr_GetResult( FACTORY_SCRWORK* wk, void* factory_call )
{
	int i;

	//結果を取得する
	for( i=0; i < FACTORY_RET_WORK_MAX ;i++ ){
		wk->ret_work[i] = FactoryCall_GetRetWork( factory_call, i );
		OS_Printf( "wk->ret_work[%d] = %d\n", i, wk->ret_work[i] );
	}

	return;
}

//----------------------------------------------------------------------------
/**
 * @brief	バトルファクトリーのポケモン選択画面での結果を取得
 *
 * @param	param	FACTORY_CALL_WORKのポインタ
 * @param	pos		ret_work[pos](0-5)
 *
 * @return	"結果"
 */
//----------------------------------------------------------------------------
u16 FactoryCall_GetRetWork( void* param, u8 pos )
{
	FACTORY_CALL_WORK* factory_call = param;

	if( pos >= FACTORY_RET_WORK_MAX ){
		GF_ASSERT(0);
		return 0;
	}

	return factory_call->ret_work[pos];
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
void FactoryScr_SaveRestPlayData( FACTORY_SCRWORK* wk, u8 mode );
u16	FactoryScr_IncRound( FACTORY_SCRWORK* wk );
u16	FactoryScr_GetRound( FACTORY_SCRWORK* wk );
u16 FactoryScr_GetEnemyObjCode( FACTORY_SCRWORK* wk, u8 param );
void FactoryScr_SetLose( FACTORY_SCRWORK* wk );
void FactoryScr_SetClear( FACTORY_SCRWORK* wk );

//--------------------------------------------------------------
/**
 * @brief	休むときに現在のプレイ状況をセーブに書き出す
 *
 * @param	wk		FACTORY_SCRWORKのポインタ
 * @param	mode	FR_MODE_CLEAR="クリア",FR_MODE_LOSE="負け",FR_MODE_REST="休む"
 */
//--------------------------------------------------------------
void FactoryScr_SaveRestPlayData( FACTORY_SCRWORK* wk, u8 mode )
{
	u16	i,before_max_rensyou,after_max_rensyou;
	u8 m_max,e_max;
	u8	buf8[4];
	u16	buf16[4];
	u32	buf32[4];
	u32 after,count,clear_id;
	FRONTIER_SAVEWORK* fr_sv;
	POKEMON_PARAM* pp;
	FACTORYDATA* f_sv = wk->factory_savedata;
	FACTORYSCORE* score_sv= SaveData_GetFactoryScore( wk->sv );

	fr_sv = SaveData_GetFrontier( wk->sv );

	//タイプによってポケモンの数を取得
	m_max = Factory_GetMinePokeNum( wk->type );							//自分
	e_max = Factory_GetEnemyPokeNum( wk->type, FACTORY_FLAG_TOTAL );	//敵

	//"レベル50、オープン"書き出し
	buf8[0] = wk->level;
	FACTORYDATA_PutPlayData( wk->factory_savedata, FACTORYDATA_ID_LEVEL, 0, buf8 );

	//"シングル、ダブル、マルチ、wifiマルチ"書き出し
	buf8[0] = wk->type;
	FACTORYDATA_PutPlayData( wk->factory_savedata, FACTORYDATA_ID_TYPE, 0, buf8 );
	
	//セーブフラグを有効状態にリセット
	FACTORYDATA_SetSaveFlag( wk->factory_savedata, TRUE );

	//"ラウンド数"書き出し(0-6人目の何人目かをあわらす)
	buf8[0] = wk->round;
	//OS_Printf( "wk->round = %d\n", wk->round );
	//OS_Printf( "buf8[0] = %d\n", buf8[0] );
	FACTORYDATA_PutPlayData( wk->factory_savedata, FACTORYDATA_ID_ROUND, 0, buf8 );

	//"交換回数"書き出し
	FrontierRecord_Set(	fr_sv, 
						FactoryScr_GetTradeRecordID(wk->level,wk->type), 
						Frontier_GetFriendIndex(FactoryScr_GetTradeRecordID(wk->level,wk->type)), 
						wk->trade_count );

#if 0
	//連勝 / 7 = 周回数
	//連勝 % 7 = 何人目か
#endif
	//"連勝数"書き出し(「次は27人目です」というように使う)
	//OS_Printf( "rensyou = %d\n", wk->rensyou );
	FrontierRecord_Set(	fr_sv, 
						FactoryScr_GetWinRecordID(wk->level,wk->type), 
						Frontier_GetFriendIndex(FactoryScr_GetWinRecordID(wk->level,wk->type)), 
						wk->rensyou );
	
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//"休む"以外
	if( mode != FR_MODE_REST ){

		//変更前"最大連勝数"を取得しておく
		before_max_rensyou = FrontierRecord_Get(	fr_sv, 
						FactoryScr_GetMaxWinRecordID(wk->level,wk->type),
						Frontier_GetFriendIndex(FactoryScr_GetMaxWinRecordID(wk->level,wk->type)) );

		//"最大連勝数"書き出し
		after = FrontierRecord_SetIfLarge(	fr_sv,
						FactoryScr_GetMaxWinRecordID(wk->level,wk->type),
						Frontier_GetFriendIndex(FactoryScr_GetMaxWinRecordID(wk->level,wk->type)),
						wk->rensyou );

		//変更後"最大連勝数"を取得しておく
		after_max_rensyou = FrontierRecord_Get(	fr_sv, 
						FactoryScr_GetMaxWinRecordID(wk->level,wk->type),
						Frontier_GetFriendIndex(FactoryScr_GetMaxWinRecordID(wk->level,wk->type)) );

		//現在の連勝数が、前の最大連勝数と同じ時
		if( wk->rensyou == before_max_rensyou ){

			//比較して"最高連勝記録時"の交換回数も更新する
			FrontierRecord_SetIfLarge(	fr_sv,
						FactoryScr_GetMaxTradeRecordID(wk->level,wk->type),
						Frontier_GetFriendIndex(FactoryScr_GetMaxTradeRecordID(wk->level,wk->type)),
						wk->trade_count );

		//最大連勝数を更新した時
		}else if( before_max_rensyou < after_max_rensyou ){

			//"最高連勝記録時"の交換回数も更新する
			FrontierRecord_Set(	fr_sv,
						FactoryScr_GetMaxTradeRecordID(wk->level,wk->type),
						Frontier_GetFriendIndex(FactoryScr_GetMaxTradeRecordID(wk->level,wk->type)),
						wk->trade_count );
		}

		//"7連勝(クリア)したかフラグ"書き出し
		buf8[0] = wk->clear_flag;
		//FACTORYDATA_PutPlayData( wk->factory_savedata, FACTORYDATA_ID_CLEAR_FLAG, 0, buf8 );
		FACTORYSCORE_PutScoreData( score_sv, FACTORYSCORE_ID_CLEAR_FLAG, 
								(wk->level*FACTORY_TYPE_MAX)+wk->type, buf8 );

		//WIFIのみ特殊
		if( wk->type == FACTORY_TYPE_WIFI_MULTI ){

			if( wk->level == FACTORY_LEVEL_50 ){
				clear_id = FRID_FACTORY_MULTI_WIFI_CLEAR_BIT;
			}else{
				clear_id = FRID_FACTORY_MULTI_WIFI_CLEAR100_BIT;
			}

			FrontierRecord_Set(	fr_sv, 
								clear_id,
								Frontier_GetFriendIndex(clear_id), wk->clear_flag );
		}
	}
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

	//トレーナー
	for( i=0; i < FACTORY_LAP_MULTI_ENEMY_MAX ;i++ ){
		buf16[0] = wk->tr_index[i];
		FACTORYDATA_PutPlayData( wk->factory_savedata, FACTORYDATA_ID_TR_INDEX, i, buf16 );
	}

	//手持ち
	count = PokeParty_GetPokeCount( wk->p_m_party );
	for( i=0; i < count ;i++ ){

		pp = PokeParty_GetMemberPointer( wk->p_m_party, i );

		//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
		//ここがモンスターナンバーではない！
		//インデックス！
		//buf16[0] = PokeParaGet( pp, ID_PARA_monsno, NULL );
		buf16[0] = wk->temoti_poke_index[i];
		FACTORYDATA_PutPlayData( wk->factory_savedata, FACTORYDATA_ID_TEMOTI_POKE_INDEX, i, buf16 );
		//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

		buf8[0] = PokeParaGet( pp, ID_PARA_pow_rnd, NULL );
		FACTORYDATA_PutPlayData( wk->factory_savedata, FACTORYDATA_ID_TEMOTI_POW_RND, i, buf8 );

		buf32[0] = PokeParaGet( pp, ID_PARA_personal_rnd, NULL );
		FACTORYDATA_PutPlayData( wk->factory_savedata, FACTORYDATA_ID_TEMOTI_PERSONAL_RND, i,buf32);
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
		FACTORYDATA_PutPlayData( wk->factory_savedata, FACTORYDATA_ID_ENEMY_POKE_INDEX, i, buf16 );
		//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

		buf8[0] = PokeParaGet( pp, ID_PARA_pow_rnd, NULL );
		FACTORYDATA_PutPlayData( wk->factory_savedata, FACTORYDATA_ID_ENEMY_POW_RND, i, buf8 );

		buf32[0] = PokeParaGet( pp, ID_PARA_personal_rnd, NULL );
		FACTORYDATA_PutPlayData( wk->factory_savedata, FACTORYDATA_ID_ENEMY_PERSONAL_RND, i,buf32);
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	ラウンド数をインクリメント
 *
 * @param	wk		FACTORY_SCRWORKのポインタ
 */
//--------------------------------------------------------------
u16	FactoryScr_IncRound( FACTORY_SCRWORK* wk )
{
	wk->round++;
	return wk->round;
}

//--------------------------------------------------------------
/**
 * @brief	ラウンド数を取得
 *
 * @param	wk		FACTORY_SCRWORKのポインタ
 */
//--------------------------------------------------------------
u16	FactoryScr_GetRound( FACTORY_SCRWORK* wk )
{
	return wk->round;
}

//--------------------------------------------------------------
/**
 * @brief	対戦トレーナーOBJコード取得
 *
 * @param	wk		FACTORY_SCRWORKのポインタ
 * @param	param	0=1人目、1=2人目(敵のパートナ－)
 */
//--------------------------------------------------------------
u16 FactoryScr_GetEnemyObjCode( FACTORY_SCRWORK* wk, u8 param )
{
	B_TOWER_TRAINER bt_trd;
	B_TOWER_TRAINER_ROM_DATA* p_rom_tr;
	u8 index;

	//取得するトレーナーデータのインデックスを取得
	index = wk->round + (param * FACTORY_LAP_ENEMY_MAX);

	//ROMからトレーナーデータを確保
	p_rom_tr = Frontier_TrainerDataGet( &bt_trd, wk->tr_index[index], HEAPID_WORLD, ARC_PL_BTD_TR );
	sys_FreeMemoryEz( p_rom_tr );

	//トレーナータイプからOBJコードを取得してくる
	return Frontier_TrType2ObjCode( bt_trd.tr_type );
}

//--------------------------------------------------------------
/**
 * @brief	敗戦した時の処理
 *
 * @param	wk		FACTORY_SCRWORKのポインタ
 */
//--------------------------------------------------------------
void FactoryScr_SetLose( FACTORY_SCRWORK* wk )
{
	OS_Printf( "\nバトルファクトリーデータ　敗戦セット\n" );

	//現在の5連勝などを保存する必要がある！
	FactoryScr_SaveRestPlayData( wk, FR_MODE_LOSE );		//セーブデータに代入

	//新規か、継続かは、WK_SCENE_FACTORY_LOBBYに格納されているので、
	//ここでroundなどをクリアしなくても、
	//WK_SCENE_FACTORY_LOBBYが継続ではない状態でセーブされるので、
	//受付に話しかけても、新規判定になりワークはクリアされる。
	return;
}

//--------------------------------------------------------------
/**
 * @brief	7連勝(クリア)した時の処理
 *
 * @param	wk		FACTORY_SCRWORKのポインタ
 */
//--------------------------------------------------------------
void FactoryScr_SetClear( FACTORY_SCRWORK* wk )
{
	OS_Printf( "\nバトルファクトリーデータ　7連勝(クリア)セット\n" );
	
	wk->clear_flag = 1;						//7連勝(クリア)したかフラグON

	//wk->rensyou		= 0;				//現在の連勝数
	if( wk->lap < FACTORY_LAP_MAX ){
		wk->lap++;							//周回数のインクリメント
	}

#if 0
	//FactoryScr_CommGetLapでずれが出ないようにペアも更新
	if( wk->pair_lap < FACTORY_LAP_MAX ){
		wk->pair_lap++;						//周回数のインクリメント
	}
#endif

	wk->round			= 0;				//今何人目？
	//wk->trade_count	= 0;				//交換回数
	FactoryScr_SaveRestPlayData( wk, FR_MODE_CLEAR );		//セーブデータに代入
	return;
}

/************************************************/
/************************************************/
/************************************************/
/************************************************/
/************************************************/

//--------------------------------------------------------------
/**
 * @brief	レンタル画面後のPOKEPARTYのセット
 *
 * @param	wk		FACTORY_SCRWORKのポインタ
 */
//--------------------------------------------------------------
void FactoryScr_RentalPartySet( FACTORY_SCRWORK* wk )
{
	int i;
	u8 m_max,e_max;
	POKEMON_PARAM* temp_poke;
	const FACTORY_POKE_RANGE* poke_range;

	//タイプによってポケモンの数を取得
	m_max = Factory_GetMinePokeNum( wk->type );							//自分
	e_max = Factory_GetEnemyPokeNum( wk->type, FACTORY_FLAG_TOTAL );	//敵

	//味方用のpartyを初期化
	PokeParty_InitWork( wk->p_m_party );		//POKEPARTYを初期化
	
	//[0-2]	手持ち(レンタルで選んだ3つ)
	temp_poke = PokemonParam_AllocWork( HEAPID_WORLD );
	for( i=0; i < m_max ;i++ ){

		//レンタルデータの必要なデータをPOKEPARTYにセット(fieldからは呼べない！)
		//バトルタワー用ポケモンデータからPOKEMON_PARAMを生成
		Frontier_PokeParaMake(&wk->rental_poke[ wk->ret_work[i] ], temp_poke, Factory_GetLevel(wk));
		Frontier_PokePartyAdd( wk->sv, wk->p_m_party, temp_poke );

		//手持ちにポケモンのインデックス格納
		wk->temoti_poke_index[i] = wk->rental_poke_index[ wk->ret_work[i] ];

		OS_Printf("戦闘前m_party[%d] monsno = %d\n", i, PokeParaGet(temp_poke,ID_PARA_monsno,NULL));
	}

	for( i=0; i < e_max ;i++ ){

		//敵ポケモンデータの必要なデータをPOKEPARTYにセット(fieldからは呼べない！)
		//バトルタワー用ポケモンデータからPOKEMON_PARAMを生成
		Frontier_PokeParaMake( &wk->enemy_poke[i], temp_poke, Factory_GetLevel(wk) );
		Frontier_PokePartyAdd( wk->sv, wk->p_e_party, temp_poke );

		//手持ちにポケモンのインデックス格納
		//wk->temoti_poke_index[i+m_max] = wk->enemy_poke_index[i];
		
		OS_Printf("戦闘前e_party[%d] monsno = %d\n", i, PokeParaGet(temp_poke,ID_PARA_monsno,NULL));
	}
	sys_FreeMemoryEz( temp_poke );

	return;
}

//--------------------------------------------------------------
/**
 * @brief	バトル後のPOKEPARTYのセット
 *
 * @param	wk		FACTORY_SCRWORKのポインタ
 */
//--------------------------------------------------------------
void FactoryScr_BtlAfterPartySet( FACTORY_SCRWORK* wk )
{
	int i,count,tmp_count;
	u8 e_max;
	u16 check_monsno[ FACTORY_TOTAL_POKE_MAX ];		//mine=4,enemy=4 = 8
	u16 check_itemno[ FACTORY_TOTAL_POKE_MAX ];
	POKEMON_PARAM* temp_poke;
	const FACTORY_POKE_RANGE* poke_range;

	//初期化	
	for( i=0; i < FACTORY_TOTAL_POKE_MAX ; i++ ){	
		check_monsno[i] = 0;
		check_itemno[i] = 0;
	}

	//タイプによってポケモンの数を取得
	e_max = Factory_GetEnemyPokeNum( wk->type, FACTORY_FLAG_TOTAL );//敵

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

		//交換成立した時に、前のenemy_poke_indexが必要なのだが、
		//交換呼び出し前に、次のenemy_poke_indexに書き換えてしまうので、
		//使用していないrental_poke_indexに退避させておく
		wk->rental_poke_index[i] = wk->enemy_poke_index[i];
	}

	poke_range = Factory_EnemyPokemonRangeGet( wk->tr_index[wk->round], wk->level );

	SDK_ASSERTMSG( (tmp_count + count) <= FACTORY_TOTAL_POKE_MAX, "check bufの要素数が足りない！" );

	//被りポケ、アイテム、データ数、取得する数、代入先
	Factory_PokemonIndexCreate( check_monsno, check_itemno, 
								(tmp_count + count),				//手持ち、敵の数
								e_max, wk->enemy_poke_index, 
								HEAPID_WORLD, poke_range, 0, wk->enemy_pow_rnd );

	//敵ポケモンを一度に生成(personalなし)
	Frontier_PokemonParamCreateAll(	wk->enemy_poke, wk->enemy_poke_index, 
									wk->enemy_pow_rnd, NULL, wk->enemy_personal_rnd, 
									e_max, HEAPID_WORLD, ARC_PL_BTD_PM );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	トレードした後のポケモンの変更
 *
 * @param	wk		FACTORY_SCRWORKのポインタ
 */
//--------------------------------------------------------------
void FactoryScr_TradePokeChange( FACTORY_SCRWORK* wk )
{
	POKEMON_PARAM* temp_poke;

	//交換画面で入れ替えるナンバーを取得
	//ret_work[0]手持ちの何番目を変えるか
	//ret_work[1]敵の何番目と変えるか
	//になる

	//交換なしかチェック
	if( wk->ret_work[0] == FACTORY_RET_CANCEL ){
		OS_Printf( "交換画面後:ポケモン入れ替え無し！\n" );
	}else{
		OS_Printf( "交換画面後:ポケモン入れ替え有り！%dと%d\n",wk->ret_work[0],wk->ret_work[1]);
		//PokeParty_ExchangePosition(	wk->p_party, wk->ret_work[0], wk->ret_work[1] );

		//POKEPARTYへポケモンデータを強制セット
		temp_poke = PokeParty_GetMemberPointer( wk->p_e_party, wk->ret_work[1] );
		PokeParty_SetMemberData( wk->p_m_party, wk->ret_work[0], temp_poke );

		//手持ちにポケモンのインデックス格納
		//wk->temoti_poke_index[ wk->ret_work[0] ] = wk->temoti_poke_index[ wk->ret_work[1] ];
		//wk->temoti_poke_index[ wk->ret_work[0] ] = wk->enemy_poke_index[ wk->ret_work[1] ];

		//前のenemy_poke_indexをrental_poke_indexに退避させているので取得
		wk->temoti_poke_index[ wk->ret_work[0] ] = wk->rental_poke_index[ wk->ret_work[1] ];

		FactoryScr_TradeCountInc( wk );				//交換回数+1

		//レコード更新
		RECORD_Inc( SaveData_GetRecord(wk->sv), RECID_FACTORY_POKE_CHANGE );
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	トレードした後のポケモンのPOKEPARTYのセット
 *
 * @param	wk		FACTORY_SCRWORKのポインタ
 *
 * 次の敵パーティをセット
 */
//--------------------------------------------------------------
void FactoryScr_TradeAfterPartySet( FACTORY_SCRWORK* wk )
{
	int i;
	u8 m_max,e_max;
	POKEMON_PARAM* temp_poke;
	int party_num;

	//タイプによってポケモンの数を取得
	m_max = Factory_GetMinePokeNum( wk->type );							//自分
	e_max = Factory_GetEnemyPokeNum( wk->type, FACTORY_FLAG_TOTAL );	//敵

	PokeParty_InitWork( wk->p_e_party );		//POKEPARTYを初期化

	temp_poke = PokemonParam_AllocWork( HEAPID_WORLD );
	for( i=0; i < e_max ;i++ ){

		//バトルタワー用ポケモンデータからPOKEMON_PARAMを生成
		Frontier_PokeParaMake( &wk->enemy_poke[i], temp_poke, Factory_GetLevel(wk) );
		Frontier_PokePartyAdd( wk->sv, wk->p_e_party, temp_poke );

		//手持ちにポケモンのインデックス格納
		//wk->temoti_poke_index[i+m_max] = wk->enemy_poke_index[i];
	}
	sys_FreeMemoryEz( temp_poke );

	//情報表示
	//for( i=0; i < FACTORY_PARTY_POKE_MAX; i++ ){
	for( i=0; i < e_max; i++ ){
		temp_poke = PokeParty_GetMemberPointer( wk->p_e_party, i );
		OS_Printf( "次の敵e_party[%d] monsno = %d\n", 
									i, PokeParaGet(temp_poke,ID_PARA_monsno,NULL) );
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	制限をかけて交換回数をインクリメントする
 *
 * @param	wk		FACTORY_SCRWORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void FactoryScr_TradeCountInc( FACTORY_SCRWORK* wk )
{
	if( wk->trade_count < FACTORY_TRADE_MAX ){
		wk->trade_count++;
	}
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
 * @param	wk			FACTORY_SCRWORK型のポインタ
 * @param	type		送信タイプ
 *
 * @retval	none
 */
//--------------------------------------------------------------
BOOL FactoryScr_CommSetSendBuf( FACTORY_SCRWORK* wk, u16 type, u16 param )
{
	int ret;

	switch( type ){

	//
	case FACTORY_COMM_BASIC:
		ret = CommFactorySendBasicData( wk );
		break;

	case FACTORY_COMM_TR:
		ret = CommFactorySendTrData( wk );
		break;

	case FACTORY_COMM_RENTAL:
		ret = CommFactorySendRentalData( wk );
		break;

	case FACTORY_COMM_ENEMY:
		ret = CommFactorySendEnemyPokeData( wk );
		break;

	case FACTORY_COMM_RETIRE:
		ret = CommFactorySendRetireFlag( wk, param );
		break;

	case FACTORY_COMM_TRADE_YESNO:
		ret = CommFactorySendTradeYesNoFlag( wk, param );
		break;

	case FACTORY_COMM_TEMOTI:
		ret = CommFactorySendTemotiPokeData( wk );
		break;
	};

	return ret;
}


//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//
//factory_tool.c	Factory_RentalPokeMake
//
//先に親機のレンタルを決定、
//親機のレンタルを渡して被りチェックして、子機のレンタルを決定
//
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

//--------------------------------------------------------------------------------------------
/**
 * ファクトリークリア時に貰えるバトルポイントを取得
 *
 * @param	wk
 *
 * @return	"貰えるバトルポイント"
 */
//--------------------------------------------------------------------------------------------
u16	FactoryScr_GetAddBtlPoint( FACTORY_SCRWORK* wk );
u16	FactoryScr_GetAddBtlPoint( FACTORY_SCRWORK* wk )
{
	u8 add_bp;
	u16 lap;
	static const u8 bppoint_normal[FACTORY_LAP_MAX+1] = { 0, 
												5, 5, 5, 5, 7, 7, 8, 9 };//0,1周～8周以降

	static const u8 bppoint_multi[FACTORY_LAP_MAX+1] = { 0, 
												10, 11, 12, 13, 16, 17, 19, 21 };//0,1周～8周以降

	//lap = FactoryScr_CommGetLap( wk );
	lap = wk->lap;

	OS_Printf( "周回数 = %d\n", lap );
	OS_Printf( "連勝数 = %d\n", wk->rensyou );

	//シングル、ダブル
	if( (wk->type == FACTORY_TYPE_SINGLE) || (wk->type == FACTORY_TYPE_DOUBLE) ){

		//最大周回数チェック
		if( lap >= FACTORY_LAP_MAX ){
			add_bp = bppoint_normal[ FACTORY_LAP_MAX ];		//テーブルが+1しているので
		}else{
			add_bp = bppoint_normal[ lap ];
		}
	
	//マルチ、WIFI
	}else{

		//最大周回数チェック
		if( lap >= FACTORY_LAP_MAX ){
			add_bp = bppoint_multi[ FACTORY_LAP_MAX ];		//テーブルが+1しているので
		}else{
			add_bp = bppoint_multi[ lap ];
		}
	}

	//ブレーンチェック
	if( wk->type == FACTORY_TYPE_SINGLE ){
		if( (wk->rensyou == FACTORY_LEADER_SET_1ST) || (wk->rensyou == FACTORY_LEADER_SET_2ND) ){
			add_bp = 20;
		}
	}

	return add_bp;
}


