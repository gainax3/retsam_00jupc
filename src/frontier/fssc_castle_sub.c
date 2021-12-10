//============================================================================================
/**
 * @file	fss_castle_sub.c
 * @bfief	フロンティアシステムスクリプトコマンドサブ：キャッスル
 * @author	Satoshi Nohara
 * @date	07.07.04
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

#include "application/castle.h"
#include "frontier_tool.h"
#include "frontier_def.h"
#include "castle_tool.h"
#include "fssc_castle_sub.h"
#include "../field/scr_tool.h"		//CastleScr_GetWinRecordID
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
CASTLE_SCRWORK* CastleScr_WorkAlloc( SAVEDATA* savedata, u16 init, u8 type, u16 pos1, u16 pos2, u16 pos3, u16* work );
void CastleScr_WorkInit( CASTLE_SCRWORK* wk, u16 init );
static void CastleScr_InitDataSet( CASTLE_SCRWORK* wk );
static void CastleScr_ContinueDataSet( CASTLE_SCRWORK* wk );
static void CastleScr_ContinuePokePara( CASTLE_SCRWORK* wk );
void CastleScr_WorkRelease( CASTLE_SCRWORK* wk );
void CastleScr_GetResult( CASTLE_SCRWORK* wk, void* castle_call );
u16 CastleCall_GetRetWork( void* param, u8 pos );
void CastleScr_BtlBeforePartySet( CASTLE_SCRWORK* wk );
void CastleScr_BtlAfterPartySet( CASTLE_SCRWORK* wk );
void CastleScr_BtlWinPokeData( CASTLE_SCRWORK* wk );
int CastleScr_GetBtlWinCpPoint( CASTLE_SCRWORK* wk );

//通信
BOOL CastleScr_CommSetSendBuf( CASTLE_SCRWORK* wk, u16 type, u16 param );


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
 * ＊かならず CastleScr_WorkRelease()で領域を開放すること
 */
//--------------------------------------------------------------
CASTLE_SCRWORK* CastleScr_WorkAlloc( SAVEDATA* savedata, u16 init, u8 type, u16 pos1, u16 pos2, u16 pos3, u16* work )
{
	u32 buf,exp;
	POKEPARTY* temoti_party;
	POKEMON_PARAM* temp_poke;
	CASTLEDATA* ca_sv;
	u8 clear_flag,m_max;
	u16	i,chg_flg;
	static CASTLE_SCRWORK* wk;					//あとで確認]]]]]]]]]]]]]]]]]]]]]]]]]
	CASTLESCORE* score_sv;

	wk = sys_AllocMemory( HEAPID_WORLD, sizeof(CASTLE_SCRWORK) );
	MI_CpuClear8( wk, sizeof(CASTLE_SCRWORK) );

	//セーブデータ取得
	wk->castle_savedata	= SaveData_GetCastleData( savedata );
	wk->sv				= savedata;
	wk->heapID			= HEAPID_WORLD;
	wk->p_m_party		= PokeParty_AllocPartyWork( HEAPID_WORLD );
	wk->p_e_party		= PokeParty_AllocPartyWork( HEAPID_WORLD );
	wk->list_del_work	= work;

	ca_sv = wk->castle_savedata;
	score_sv = SaveData_GetCastleScore( savedata );

	//新規か、継続か
	if( init == 0 ){
		wk->type	= type;
		wk->round	= 0;

		CASTLEDATA_Init( ca_sv );	//中断データ初期化

		//WIFIのみ特殊
		if( wk->type == CASTLE_TYPE_WIFI_MULTI ){
#if 0
			clear_flag = FrontierRecord_Get(SaveData_GetFrontier(wk->sv), 
										FRID_CASTLE_MULTI_WIFI_CLEAR_BIT,
										Frontier_GetFriendIndex(FRID_CASTLE_MULTI_WIFI_CLEAR_BIT) );
#else
			clear_flag = SysWork_WifiFrClearFlagGet( SaveData_GetEventWork(wk->sv) );
#endif

		}else{
			//クリアしたかフラグを取得
			clear_flag = (u8)CASTLESCORE_GetScoreData(	score_sv, CASTLESCORE_ID_CLEAR_FLAG, 
														wk->type, 0, NULL );
		}

		if( clear_flag == 1 ){
			wk->rensyou = FrontierRecord_Get(	SaveData_GetFrontier(wk->sv), 
									CastleScr_GetWinRecordID(wk->type),
									Frontier_GetFriendIndex(CastleScr_GetWinRecordID(wk->type)) );
		}else{
			wk->rensyou = 0;

			//"CP"を0にする
			FrontierRecord_Set(	SaveData_GetFrontier(wk->sv), 
								CastleScr_GetCPRecordID(wk->type),
								Frontier_GetFriendIndex(CastleScr_GetCPRecordID(wk->type)), 0 );

			//"使用したCP"を0にする
			FrontierRecord_Set(	SaveData_GetFrontier(wk->sv), 
								CastleScr_GetUsedCPRecordID(type),
								Frontier_GetFriendIndex(CastleScr_GetUsedCPRecordID(type)), 0 );

#if 1
			//新規で初挑戦の時は、1オリジンセット
			for( i=0; i < CASTLE_RANK_TYPE_MAX ;i++ ){
				FrontierRecord_Set(	SaveData_GetFrontier(wk->sv), 
						CastleScr_GetRankRecordID(wk->type,i),
						Frontier_GetFriendIndex(CastleScr_GetRankRecordID(wk->type,i)), 1 );
			}
#endif

		}

		wk->lap		= (u16)(wk->rensyou / CASTLE_LAP_ENEMY_MAX);
		wk->win_cnt	= 0;

		//選択した手持ちの位置
		wk->mine_poke_pos[0] = pos1;
		wk->mine_poke_pos[1] = pos2;
		wk->mine_poke_pos[2] = pos3;

		//"+10する前に処理する"挑戦開始時のCPを退避
		wk->temp_start_cp = FrontierRecord_Get(	SaveData_GetFrontier(wk->sv), 
								CastleScr_GetCPRecordID(wk->type),
								Frontier_GetFriendIndex(CastleScr_GetCPRecordID(wk->type)) );

		//挑戦開始の時にCP+10する	
		FrontierRecord_Add(	SaveData_GetFrontier(wk->sv), 
							CastleScr_GetCPRecordID(wk->type),
							Frontier_GetFriendIndex(CastleScr_GetCPRecordID(wk->type)), 10 );

	}else{
		//現在のプレイ進行データ取得
		wk->type = (u8)CASTLEDATA_GetPlayData( ca_sv, CASTLEDATA_ID_TYPE, 0, 0, NULL);
		wk->round = (u8)CASTLEDATA_GetPlayData( ca_sv, CASTLEDATA_ID_ROUND, 0, 0, NULL);
		wk->rensyou = FrontierRecord_Get(	SaveData_GetFrontier(wk->sv), 
									CastleScr_GetWinRecordID(wk->type),
									Frontier_GetFriendIndex(CastleScr_GetWinRecordID(wk->type)) );
		wk->lap		= (u16)(wk->rensyou / CASTLE_LAP_ENEMY_MAX);

		for( i=0; i < CASTLE_ENTRY_POKE_MAX ;i++ ){
			wk->mine_poke_pos[i] = (u8)CASTLEDATA_GetPlayData(	ca_sv, CASTLEDATA_ID_MINE_POKE_POS, 
																i, 0, NULL);
		}
	}

	//所持アイテムを保存しておく
	for( i=0; i < CASTLE_ENTRY_POKE_MAX ;i++ ){
		temp_poke = PokeParty_GetMemberPointer( SaveData_GetTemotiPokemon(wk->sv), 
												wk->mine_poke_pos[i] );
		wk->itemno[i] = PokeParaGet( temp_poke, ID_PARA_item, NULL );
	}

	//出場ポケモンをセット
	temoti_party = SaveData_GetTemotiPokemon( wk->sv );
	m_max = Castle_GetMinePokeNum( wk->type, CASTLE_FLAG_SOLO );		//味方
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

#ifdef PM_DEBUG
	//CPを9999セット
	if( sys.cont & PAD_BUTTON_L ){
		FrontierRecord_Set(	SaveData_GetFrontier(wk->sv), 
							CastleScr_GetCPRecordID(wk->type),
							Frontier_GetFriendIndex(CastleScr_GetCPRecordID(wk->type)), 9999 );
	}
#endif

	//地球儀登録
	if( Castle_CommCheck(wk->type) == TRUE ){
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
 * ＊かならず CastleScr_WorkRelease()で領域を開放すること
 */
//--------------------------------------------------------------
void CastleScr_WorkInit( CASTLE_SCRWORK* wk, u16 init )
{
	//新規か、継続か
	if( init == 0 ){
		//OS_Printf( "前 新規 問題のwk = %d\n", wk );
		CastleScr_InitDataSet( wk );
		//OS_Printf( "後 新規 問題のwk = %d\n", wk );
	}else{
		//OS_Printf( "前 継続 問題のwk = %d\n", wk );
		CastleScr_ContinueDataSet( wk );
		//OS_Printf( "後 継続 問題のwk = %d\n", wk );
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	新規：トレーナー、ポケモンデータを生成
 *
 * @param	wk		CASTLE_SCRWORKへのポインタ
 */
//--------------------------------------------------------------
static void CastleScr_InitDataSet( CASTLE_SCRWORK* wk )
{
	u8 count;
	int i;
	B_TOWER_POKEMON poke;
	POKEMON_PARAM* temp_poke;
	POKEPARTY* temoti_party;
	u16 poke_check_tbl[6];	//rental
	u16 item_check_tbl[6];	//rental

	OS_Printf( "新規：データを生成\n" );

	count = PokeParty_GetPokeCount( wk->p_m_party );
	for( i=0; i < count ;i++ ){
		temp_poke = PokeParty_GetMemberPointer( wk->p_m_party, i );
		wk->pp[i][0] = PokeParaGet( temp_poke, ID_PARA_pp1, NULL );					//PP保存
		wk->pp[i][1] = PokeParaGet( temp_poke, ID_PARA_pp2, NULL );
		wk->pp[i][2] = PokeParaGet( temp_poke, ID_PARA_pp3, NULL );
		wk->pp[i][3] = PokeParaGet( temp_poke, ID_PARA_pp4, NULL );
	}

	//07.09.25 常に周回数*2のトレーナーを取得
	//シングル(0-6)、マルチのパートナー(7-13)で取得
	//その周に登場するトレーナーのindexを全て取得
	Castle_EnemyLapAllTrainerIndexGet(	wk->type, CastleScr_CommGetLap(wk), wk->tr_index, 
										CASTLE_LAP_MULTI_ENEMY_MAX );
	OS_Printf( "lap = %d\n", CastleScr_CommGetLap(wk) );

	//デバック情報
	for( i=0; i < CASTLE_LAP_MULTI_ENEMY_MAX ;i++ ){
		OS_Printf( "tr_index[%d] = %d\n", i, wk->tr_index[i] );
	}

	//敵トレーナーが出すポケモン生成
	Frontier_EnemyPokeMake(	Castle_GetEnemyPokeNum(wk->type,CASTLE_FLAG_TOTAL),
							wk->tr_index[wk->round],
							wk->tr_index[wk->round+CASTLE_LAP_ENEMY_MAX], 
							wk->enemy_poke_index, 
							wk->enemy_poke,						//B_TOWER_POKEMON
							wk->enemy_pow_rnd,
							wk->enemy_personal_rnd,
							Castle_CommCheck(wk->type) );

	for( i=0; i < 4 ;i++ ){
		OS_Printf( "enemy_poke[%d] = %d\n", i, wk->enemy_poke[i].mons_no );
		OS_Printf( "enemy_index[%d] = %d\n", i, wk->enemy_poke_index[i] );
		OS_Printf( "enemy_pow[%d] = %d\n", i, wk->enemy_pow_rnd[i] );
		OS_Printf( "enemy_personal[%d] = %d\n", i, wk->enemy_personal_rnd[i] );
	}

	//OS_Printf( "333wk = %d\n", wk );
	//OS_Printf( "333wk->p_party = %d\n", wk->p_party );

	return;
}

//--------------------------------------------------------------
/**
 * @brief	継続：保存しておいたデータを使ってB_TOWER_POKEMONなどを生成
 *
 * @param	wk		CASTLE_SCRWORKへのポインタ
 */
//--------------------------------------------------------------
static void CastleScr_ContinueDataSet( CASTLE_SCRWORK* wk )
{
	u16 buf;
	int i;
	POKEMON_PARAM* pp;
	u8 count;
	B_TOWER_POKEMON bt_poke[CASTLE_PARTY_POKE_MAX];
	u8 pow_rnd[CASTLE_PARTY_POKE_MAX];
	u16 poke_index[CASTLE_PARTY_POKE_MAX];
	u32 personal_rnd[CASTLE_PARTY_POKE_MAX];
	OS_Printf( "継続：保存しておいたデータをロード\n" );

	//"手持ちポケモンの状態"ロード
	CastleScr_ContinuePokePara( wk );

	//-----------------------------------------------------------------------------------
	
	//保存しておいたトレーナーのindex取得
	for( i=0; i < CASTLE_LAP_MULTI_ENEMY_MAX ;i++ ){
		wk->tr_index[i] = (u16)CASTLEDATA_GetPlayData(	wk->castle_savedata, 
														CASTLEDATA_ID_TR_INDEX, i, 0, NULL );
		//OS_Printf( "tr_index[%d] = %d\n", i, wk->tr_index[i] );
	}

	//---------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------
	//保存しておいた敵ポケモン取得
	for( i=0; i < CASTLE_ENEMY_POKE_MAX ;i++ ){

		poke_index[i] = (u16)CASTLEDATA_GetPlayData(wk->castle_savedata, 
												CASTLEDATA_ID_ENEMY_POKE_INDEX, i, 0, NULL );
		//OS_Printf( "poke_index[%d] = %d\n", i, poke_index[i] );
		
		//敵ポケモンのインデックス格納
		wk->enemy_poke_index[i] = poke_index[i];
	}

	//手持ちポケモンを一度に生成(personalなし)
	Frontier_PokemonParamCreateAll(	bt_poke, poke_index, pow_rnd,
									NULL, personal_rnd, CASTLE_ENEMY_POKE_MAX, HEAPID_WORLD,
									ARC_PL_BTD_PM );

	pp = PokemonParam_AllocWork( HEAPID_WORLD );
	for( i=0; i < CASTLE_ENEMY_POKE_MAX ;i++ ){
		//バトルタワー用ポケモンデータからPOKEMON_PARAMを生成
		Frontier_PokeParaMake( &bt_poke[i], pp, Castle_GetLevel(wk) );
		Castle_PokePartyAdd( wk, wk->p_e_party, pp );
		OS_Printf( "継続e_party[%d] monsno = %d\n", i, PokeParaGet(pp,ID_PARA_monsno,NULL) );

	}
	sys_FreeMemoryEz( pp );

	return;
}

//--------------------------------------------------------------
/**
 * @brief	継続：HP、PP、CONDITION、ITEMをロード
 *
 * @param	wk		CASTLE_SCRWORKへのポインタ
 */
//--------------------------------------------------------------
static void CastleScr_ContinuePokePara( CASTLE_SCRWORK* wk )
{
	u8 count;
	u32 buf32;
	u16 buf16;
	u8	buf8;
	int i;
	POKEMON_PARAM* pp;

	count = PokeParty_GetPokeCount( wk->p_m_party );

	//"手持ちポケモンの状態"ロード
	for( i=0; i < count ;i++ ){

		pp = PokeParty_GetMemberPointer( wk->p_m_party, i );

		buf16 = (u16)CASTLEDATA_GetPlayData(	wk->castle_savedata, 
											CASTLEDATA_ID_TEMOTI_HP, i, 0, NULL );
		OS_Printf( "ロード時のHP = %d\n", buf16 );
		PokeParaPut( pp, ID_PARA_hp, &buf16 );			//HP

		buf8 = (u8)CASTLEDATA_GetPlayData(	wk->castle_savedata, 
											CASTLEDATA_ID_TEMOTI_PP, i, 0, NULL );
		PokeParaPut( pp, ID_PARA_pp1, &buf8 );			//PP1

		buf8 = (u8)CASTLEDATA_GetPlayData(	wk->castle_savedata, 
											CASTLEDATA_ID_TEMOTI_PP, i, 1, NULL );
		PokeParaPut( pp, ID_PARA_pp2, &buf8 );			//PP2

		buf8 = (u8)CASTLEDATA_GetPlayData(	wk->castle_savedata, 
											CASTLEDATA_ID_TEMOTI_PP, i, 2, NULL );
		PokeParaPut( pp, ID_PARA_pp3, &buf8 );			//PP3

		buf8 = (u8)CASTLEDATA_GetPlayData(	wk->castle_savedata, 
											CASTLEDATA_ID_TEMOTI_PP, i, 3, NULL );
		PokeParaPut( pp, ID_PARA_pp4, &buf8 );			//PP4

		buf32 = (u32)CASTLEDATA_GetPlayData(wk->castle_savedata, 
											CASTLEDATA_ID_TEMOTI_CONDITION, i, 0, NULL );
		PokeParaPut( pp, ID_PARA_condition, &buf32 );	//CONDITION

		buf16 = (u16)CASTLEDATA_GetPlayData(	wk->castle_savedata, 
											CASTLEDATA_ID_TEMOTI_ITEM, i, 0, NULL );
		PokeParaPut( pp, ID_PARA_item, &buf16 );		//ITEM
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	ワークエリアを開放する
 */
//--------------------------------------------------------------
void CastleScr_WorkRelease( CASTLE_SCRWORK* wk )
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

	MI_CpuClear8( wk, sizeof(CASTLE_SCRWORK) );
	sys_FreeMemoryEz( wk );
	wk = NULL;

	return;
}

//--------------------------------------------------------------
/**
 * @brief	キャッスル画面呼び出し後の結果取得
 *
 * @param	wk		CASTLE_SCRWORKのポインタ
 */
//--------------------------------------------------------------
void CastleScr_GetResult( CASTLE_SCRWORK* wk, void* castle_call )
{
	int i;
	CASTLE_CALL_WORK* call_wk = castle_call;

	//結果を取得する
	for( i=0; i < CASTLE_RET_WORK_MAX ;i++ ){
		wk->ret_work[i] = CastleCall_GetRetWork( castle_call, i );
		OS_Printf( "wk->ret_work[%d] = %d\n", i, wk->ret_work[i] );
	}

	//情報が公開されたかフラグ
	for( i=0; i < CASTLE_ENEMY_POKE_MAX ;i++ ){
		wk->enemy_temoti_flag[i]	= call_wk->enemy_temoti_flag[i];
		wk->enemy_level_flag[i]		= call_wk->enemy_level_flag[i];
		wk->enemy_tuyosa_flag[i]	= call_wk->enemy_tuyosa_flag[i];
		wk->enemy_waza_flag[i]		= call_wk->enemy_waza_flag[i];

		OS_Printf( "enemy_temoti_flag[%d] = %d\n", i, wk->enemy_temoti_flag[i] );
		OS_Printf( "enemy_level_flag[%d] = %d\n", i, wk->enemy_level_flag[i] );
		OS_Printf( "enemy_tuyosa_flag[%d] = %d\n", i, wk->enemy_tuyosa_flag[i] );
		OS_Printf( "enemy_waza_flag[%d] = %d\n", i, wk->enemy_waza_flag[i] );
	}

	//パートナーのCP
	wk->pair_cp	= call_wk->pair_cp;

	return;
}

//----------------------------------------------------------------------------
/**
 * @brief	バトルキャッスルのポケモン選択画面での結果を取得
 *
 * @param	param	CASTLE_CALL_WORKのポインタ
 * @param	pos		ret_work[pos](0-5)
 *
 * @return	"結果"
 */
//----------------------------------------------------------------------------
u16 CastleCall_GetRetWork( void* param, u8 pos )
{
	CASTLE_CALL_WORK* castle_call = param;

	if( pos >= CASTLE_RET_WORK_MAX ){
		GF_ASSERT(0);
		return 0;
	}

	return castle_call->ret_work;
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
void CastleScr_SaveRestPlayData( CASTLE_SCRWORK* wk, u8 mode );
u16	CastleScr_IncRound( CASTLE_SCRWORK* wk );
u16	CastleScr_GetRound( CASTLE_SCRWORK* wk );
u16 CastleScr_GetEnemyObjCode( CASTLE_SCRWORK* wk, u8 param );
void CastleScr_SetLose( CASTLE_SCRWORK* wk );
void CastleScr_SetClear( CASTLE_SCRWORK* wk );

//--------------------------------------------------------------
/**
 * @brief	休むときに現在のプレイ状況をセーブに書き出す
 *
 * @param	wk		CASTLE_SCRWORKのポインタ
 * @param	mode	FR_MODE_CLEAR="クリア",FR_MODE_LOSE="負け",FR_MODE_REST="休む"
 */
//--------------------------------------------------------------
void CastleScr_SaveRestPlayData( CASTLE_SCRWORK* wk, u8 mode )
{
	u16	i,before_max_rensyou,after_max_rensyou;
	u8 e_max;
	u8	buf8[4];
	u16	buf16[4];
	u32	buf32[4];
	u32 after,count,cp;
	FRONTIER_SAVEWORK* fr_sv;
	POKEMON_PARAM* pp;
	CASTLEDATA* f_sv = wk->castle_savedata;
	CASTLESCORE* score_sv = SaveData_GetCastleScore( wk->sv );

	fr_sv = SaveData_GetFrontier( wk->sv );

	//タイプによってポケモンの数を取得
	e_max = Castle_GetEnemyPokeNum( wk->type, CASTLE_FLAG_TOTAL );	//敵

	//"シングル、ダブル、マルチ、wifiマルチ"書き出し
	buf8[0] = wk->type;
	CASTLEDATA_PutPlayData( wk->castle_savedata, CASTLEDATA_ID_TYPE, 0, 0, buf8 );
	
	//セーブフラグを有効状態にリセット
	CASTLEDATA_SetSaveFlag( wk->castle_savedata, TRUE );

	//"ラウンド数"書き出し(0-6人目の何人目かをあわらす)
	buf8[0] = wk->round;
	//OS_Printf( "wk->round = %d\n", wk->round );
	CASTLEDATA_PutPlayData( wk->castle_savedata, CASTLEDATA_ID_ROUND, 0, 0, buf8 );

#if 0
	//連勝 / 7 = 周回数
	//連勝 % 7 = 何人目か
#endif
	//"連勝数"書き出し(「次は27人目です」というように使う)
	//OS_Printf( "rensyou = %d\n", wk->rensyou );
	FrontierRecord_Set(	fr_sv, 
						CastleScr_GetWinRecordID(wk->type),
						Frontier_GetFriendIndex(CastleScr_GetWinRecordID(wk->type)), wk->rensyou );
	
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//"休む"以外
	if( mode != FR_MODE_REST ){

		//変更前"最大連勝数"を取得しておく
		before_max_rensyou = FrontierRecord_Get(	fr_sv, 
							CastleScr_GetMaxWinRecordID(wk->type),
							Frontier_GetFriendIndex(CastleScr_GetMaxWinRecordID(wk->type)) );

		//"最大連勝数"書き出し
		after = FrontierRecord_SetIfLarge(	fr_sv,
								CastleScr_GetMaxWinRecordID(wk->type),
								Frontier_GetFriendIndex(CastleScr_GetMaxWinRecordID(wk->type)), 
								wk->rensyou );

		//変更後"最大連勝数"を取得しておく
		after_max_rensyou = FrontierRecord_Get(	fr_sv, 
							CastleScr_GetMaxWinRecordID(wk->type),
							Frontier_GetFriendIndex(CastleScr_GetMaxWinRecordID(wk->type)) );

		//現在のCPを取得しておく
		cp = FrontierRecord_Get(fr_sv, 
								CastleScr_GetCPRecordID(wk->type),
								Frontier_GetFriendIndex(CastleScr_GetCPRecordID(wk->type)) );
		OS_Printf( "現在のcp = %d\n", cp );

		//現在の連勝数が、前の最大連勝数と同じ時
		if( wk->rensyou == before_max_rensyou ){

			//比較して"最高連勝記録時"の"残りCP"も更新する
			FrontierRecord_SetIfLarge(	fr_sv, 
					CastleScr_GetRemainderCPRecordID(wk->type),
					Frontier_GetFriendIndex(CastleScr_GetRemainderCPRecordID(wk->type)), 
					cp );

		//最大連勝数を更新した時
		}else if( before_max_rensyou < after_max_rensyou ){

			//"最高連勝記録時"の"残りCP"も更新する
			FrontierRecord_Set(	fr_sv, 
					CastleScr_GetRemainderCPRecordID(wk->type),
					Frontier_GetFriendIndex(CastleScr_GetRemainderCPRecordID(wk->type)), cp );
		}

		//"7連勝(クリア)したかフラグ"書き出し
		buf8[0] = wk->clear_flag;
		CASTLESCORE_PutScoreData( score_sv, CASTLESCORE_ID_CLEAR_FLAG, wk->type, 0, buf8 );

		//WIFIのみ特殊
		if( wk->type == CASTLE_TYPE_WIFI_MULTI ){
			FrontierRecord_Set(	fr_sv, 
					FRID_CASTLE_MULTI_WIFI_CLEAR_BIT,
					Frontier_GetFriendIndex(FRID_CASTLE_MULTI_WIFI_CLEAR_BIT), wk->clear_flag );
		}
	}
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

	//"トレーナーインデックス"書き出し
	for( i=0; i < CASTLE_LAP_MULTI_ENEMY_MAX ;i++ ){
		buf16[0] = wk->tr_index[i];
		CASTLEDATA_PutPlayData( wk->castle_savedata, CASTLEDATA_ID_TR_INDEX, i, 0, buf16 );
	}

	//"手持ち位置"書き出し
	for( i=0; i < CASTLE_ENTRY_POKE_MAX ;i++ ){
		buf8[0] = wk->mine_poke_pos[i];
		CASTLEDATA_PutPlayData(	wk->castle_savedata, CASTLEDATA_ID_MINE_POKE_POS, i, 0, buf8 );
	}

	//"手持ちポケモンの状態"書き出し
	count = PokeParty_GetPokeCount( wk->p_m_party );
	for( i=0; i < count ;i++ ){

		pp = PokeParty_GetMemberPointer( wk->p_m_party, i );

		buf16[0] = PokeParaGet( pp, ID_PARA_hp, NULL );			//HP
		OS_Printf( "やすむ時のHP = %d\n", buf16[0] );
		CASTLEDATA_PutPlayData( wk->castle_savedata, CASTLEDATA_ID_TEMOTI_HP, i, 0, buf16 );

		buf8[0] = PokeParaGet( pp, ID_PARA_pp1, NULL );			//PP1
		CASTLEDATA_PutPlayData( wk->castle_savedata, CASTLEDATA_ID_TEMOTI_PP, i, 0, buf8 );

		buf8[0] = PokeParaGet( pp, ID_PARA_pp2, NULL );			//PP2
		CASTLEDATA_PutPlayData( wk->castle_savedata, CASTLEDATA_ID_TEMOTI_PP, i, 1, buf8 );

		buf8[0] = PokeParaGet( pp, ID_PARA_pp3, NULL );			//PP3
		CASTLEDATA_PutPlayData( wk->castle_savedata, CASTLEDATA_ID_TEMOTI_PP, i, 2, buf8 );

		buf8[0] = PokeParaGet( pp, ID_PARA_pp4, NULL );			//PP4
		CASTLEDATA_PutPlayData( wk->castle_savedata, CASTLEDATA_ID_TEMOTI_PP, i, 3, buf8 );

		buf32[0] = PokeParaGet( pp, ID_PARA_condition, NULL );	//CONDITION
		CASTLEDATA_PutPlayData( wk->castle_savedata, CASTLEDATA_ID_TEMOTI_CONDITION, i, 0, buf32 );

		buf16[0] = PokeParaGet( pp, ID_PARA_item, NULL );		//ITEM
		CASTLEDATA_PutPlayData( wk->castle_savedata, CASTLEDATA_ID_TEMOTI_ITEM, i, 0, buf16 );
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
		CASTLEDATA_PutPlayData( wk->castle_savedata, CASTLEDATA_ID_ENEMY_POKE_INDEX, i, 0, buf16 );
		//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	ラウンド数をインクリメント
 *
 * @param	wk		CASTLE_SCRWORKのポインタ
 */
//--------------------------------------------------------------
u16	CastleScr_IncRound( CASTLE_SCRWORK* wk )
{
	wk->round++;
	return wk->round;
}

//--------------------------------------------------------------
/**
 * @brief	ラウンド数を取得
 *
 * @param	wk		CASTLE_SCRWORKのポインタ
 */
//--------------------------------------------------------------
u16	CastleScr_GetRound( CASTLE_SCRWORK* wk )
{
	return wk->round;
}

//--------------------------------------------------------------
/**
 * @brief	対戦トレーナーOBJコード取得
 *
 * @param	wk		CASTLE_SCRWORKのポインタ
 * @param	param	0=1人目、1=2人目(敵のパートナ－)
 */
//--------------------------------------------------------------
u16 CastleScr_GetEnemyObjCode( CASTLE_SCRWORK* wk, u8 param )
{
	B_TOWER_TRAINER bt_trd;
	B_TOWER_TRAINER_ROM_DATA* p_rom_tr;
	u8 index;

	//取得するトレーナーデータのインデックスを取得
	index = wk->round + (param * CASTLE_LAP_ENEMY_MAX);

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
 * @param	wk		CASTLE_SCRWORKのポインタ
 */
//--------------------------------------------------------------
void CastleScr_SetLose( CASTLE_SCRWORK* wk )
{
	u16 buf16[4];
	int i;

	OS_Printf( "\nバトルキャッスルデータ　敗戦セット\n" );

	//負けた時のみランクを元に戻す(7連勝の時はランクそのまま引き継ぐ)
	buf16[0] = 1;
	for( i=0; i < CASTLE_RANK_TYPE_MAX ;i++ ){
		FrontierRecord_Set(	SaveData_GetFrontier(wk->sv), 
					CastleScr_GetRankRecordID(wk->type,i),
					Frontier_GetFriendIndex(CastleScr_GetRankRecordID(wk->type,i)), 1 );
	}

	//現在の5連勝などを保存する必要がある！
	CastleScr_SaveRestPlayData( wk, FR_MODE_LOSE );		//セーブデータに代入

#if 0
	//現在の状態は保持するのでクリアしなくてよい　08.03.17
	//再挑戦する時、不正に終了した時にクリアされる
	
	//"CP"を0にする
	FrontierRecord_Set(	SaveData_GetFrontier(wk->sv), 
						CastleScr_GetCPRecordID(wk->type),
						Frontier_GetFriendIndex(CastleScr_GetCPRecordID(wk->type)), 0 );
#endif

	//新規か、継続かは、WK_SCENE_CASTLE_LOBBYに格納されているので、
	//ここでroundなどをクリアしなくても、
	//WK_SCENE_CASTLE_LOBBYが継続ではない状態でセーブされるので、
	//受付に話しかけても、新規判定になりワークはクリアされる。
	return;
}

//--------------------------------------------------------------
/**
 * @brief	7連勝(クリア)した時の処理
 *
 * @param	wk		CASTLE_SCRWORKのポインタ
 */
//--------------------------------------------------------------
void CastleScr_SetClear( CASTLE_SCRWORK* wk )
{
	OS_Printf( "\nバトルキャッスルデータ　7連勝(クリア)セット\n" );
	
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
	if( wk->lap < CASTLE_LAP_MAX ){
		wk->lap++;							//周回数のインクリメント
	}

#if 0
	//CastleScr_CommGetLapでずれが出ないようにペアも更新
	if( wk->pair_lap < CASTLE_LAP_MAX ){
		wk->pair_lap++;						//周回数のインクリメント
	}
#endif

	wk->round			= 0;				//今何人目？
	CastleScr_SaveRestPlayData( wk, FR_MODE_CLEAR );		//セーブデータに代入
	return;
}

//--------------------------------------------------------------
/**
 * @brief	戦闘前のPOKEPARTYのセット
 *
 * @param	wk		CASTLE_SCRWORKのポインタ
 */
//--------------------------------------------------------------
void CastleScr_BtlBeforePartySet( CASTLE_SCRWORK* wk )
{
	Castle_EnemyPartySet( wk );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	バトル後のPOKEPARTYのセット
 *
 * @param	wk		CASTLE_SCRWORKのポインタ
 */
//--------------------------------------------------------------
void CastleScr_BtlAfterPartySet( CASTLE_SCRWORK* wk )
{
	int i,count,tmp_count;
#if 0
	u8 e_max;
	u16 check_monsno[ CASTLE_TOTAL_POKE_MAX ];		//mine=4,enemy=4 = 8
	u16 check_itemno[ CASTLE_TOTAL_POKE_MAX ];
	POKEMON_PARAM* temp_poke;
	B_TOWER_TRAINER bt_trd;
	B_TOWER_TRAINER_ROM_DATA* trd;

	//初期化	
	for( i=0; i < CASTLE_TOTAL_POKE_MAX ; i++ ){	
		check_monsno[i] = 0;
		check_itemno[i] = 0;
	}

	//タイプによってポケモンの数を取得
	e_max = Castle_GetEnemyPokeNum( wk->type, CASTLE_FLAG_TOTAL );//敵

	count = PokeParty_GetPokeCount( wk->p_m_party );
	for( i=0; i < count ; i++ ){									//手持ちポケモン
		temp_poke = PokeParty_GetMemberPointer( wk->p_m_party, i );
		check_monsno[i] = PokeParaGet(temp_poke, ID_PARA_monsno, NULL);
		check_itemno[i] = PokeParaGet(temp_poke, ID_PARA_item, NULL);

#if 0
		//PP更新
		wk->pp[i][0] = PokeParaGet( temp_poke, ID_PARA_pp1, NULL );
		wk->pp[i][1] = PokeParaGet( temp_poke, ID_PARA_pp2, NULL );
		wk->pp[i][2] = PokeParaGet( temp_poke, ID_PARA_pp3, NULL );
		wk->pp[i][3] = PokeParaGet( temp_poke, ID_PARA_pp4, NULL );
#endif
	}
	tmp_count = count;

	count = PokeParty_GetPokeCount( wk->p_e_party );
	for( i=0; i < count ; i++ ){									//敵ポケモン
		temp_poke = PokeParty_GetMemberPointer( wk->p_e_party, i );
		check_monsno[i+tmp_count] = PokeParaGet(temp_poke, ID_PARA_monsno, NULL);
		check_itemno[i+tmp_count] = PokeParaGet(temp_poke, ID_PARA_item, NULL);
	}

	SDK_ASSERTMSG( (tmp_count + count) <= CASTLE_TOTAL_POKE_MAX, "check bufの要素数が足りない！" );

	//ROMからトレーナーデータを確保
	trd = Frontier_TrainerDataGet( &bt_trd, wk->tr_index[wk->round], HEAPID_WORLD, ARC_PL_BTD_TR );

	//被りポケ、アイテム、データ数、取得する数、代入先
	Frontier_PokemonIndexCreate(	trd, check_monsno, check_itemno, 
									(tmp_count + count),				//手持ち、敵の数
									e_max, wk->enemy_poke_index, HEAPID_WORLD );

	sys_FreeMemoryEz( trd );

	//敵ポケモンを一度に生成(personalなし)
	Frontier_PokemonParamCreateAll(	wk->enemy_poke, wk->enemy_poke_index, 
									wk->enemy_pow_rnd, NULL, wk->enemy_personal_rnd, 
									e_max, HEAPID_WORLD, ARC_PL_BTD_PM );



#else
	//敵トレーナーが出すポケモン生成
	Frontier_EnemyPokeMake(	Castle_GetEnemyPokeNum(wk->type,CASTLE_FLAG_TOTAL),
							wk->tr_index[wk->round],
							wk->tr_index[wk->round+CASTLE_LAP_ENEMY_MAX], 
							wk->enemy_poke_index, 
							wk->enemy_poke,						//B_TOWER_POKEMON
							wk->enemy_pow_rnd,
							wk->enemy_personal_rnd,
							Castle_CommCheck(wk->type) );

	for( i=0; i < 4 ;i++ ){
		OS_Printf( "enemy_poke[%d] = %d\n", i, wk->enemy_poke[i].mons_no );
		OS_Printf( "enemy_index[%d] = %d\n", i, wk->enemy_poke_index[i] );
		OS_Printf( "enemy_pow[%d] = %d\n", i, wk->enemy_pow_rnd[i] );
		OS_Printf( "enemy_personal[%d] = %d\n", i, wk->enemy_personal_rnd[i] );
	}
#endif

	//敵パーティのセット
	Castle_EnemyPartySet( wk );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	バトル後のポケモンデータ更新
 *
 * @param	wk		CASTLE_SCRWORKのポインタ
 */
//--------------------------------------------------------------
void CastleScr_BtlWinPokeData( CASTLE_SCRWORK* wk )
{
	u8 offset,m_max;
	int i,count;
	POKEMON_PARAM* temp_poke;

	//p_m_partyのオフセット
	if( CommGetCurrentID() == COMM_PARENT_ID ){
		offset = 0;
	}else{
		offset = CASTLE_COMM_POKE_NUM;
	}

	m_max = Castle_GetMinePokeNum( wk->type, CASTLE_FLAG_SOLO );
	count = PokeParty_GetPokeCount( wk->p_m_party );

	for( i=offset; i < (m_max+offset) ; i++ ){							//手持ちポケモン
		temp_poke = PokeParty_GetMemberPointer( wk->p_m_party, i );

		//PP更新
		wk->pp[i-offset][0] = PokeParaGet( temp_poke, ID_PARA_pp1, NULL );
		wk->pp[i-offset][1] = PokeParaGet( temp_poke, ID_PARA_pp2, NULL );
		wk->pp[i-offset][2] = PokeParaGet( temp_poke, ID_PARA_pp3, NULL );
		wk->pp[i-offset][3] = PokeParaGet( temp_poke, ID_PARA_pp4, NULL );
	}

	//手持ちポケモンの状態をキャッスル仕様回復
	Castle_PokePartyRecoverAll( wk->p_m_party );

	//情報が公開されたかフラグをクリア
	for( i=0; i < CASTLE_ENEMY_POKE_MAX ;i++ ){
		wk->enemy_temoti_flag[i]	= 0;
		wk->enemy_level_flag[i]		= 0;
		wk->enemy_tuyosa_flag[i]	= 0;
		wk->enemy_waza_flag[i]		= 0;
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	バトル勝利後のもらえるＣＰポイント計算、増やす
 *
 * @param	wk			CASTLE_SCRWORK型のポインタ
 *
 * @return	"CPポイント"
 */
//--------------------------------------------------------------
int CastleScr_GetBtlWinCpPoint( CASTLE_SCRWORK* wk )
{
	u8 m_max,e_max,offset;
	u32 hp,hpmax;
	int i,j,before_pp_max,after_pp_max;
	POKEMON_PARAM* pp;
	u8 tmp[20];
	int ret = 0;

	enum{
		ID_NO_HINSI,
		ID_HP_MAX,
		ID_HP_HALF_UP,
		ID_HP_HALF_DOWN,
		ID_CONDITION,
		ID_PP_5,
		ID_PP_10,
		ID_PP_15,
		ID_LV_PLUS,
		ID_LV_MINUS,			//10
	};

	OS_Printf( "********************\n戦闘後のもらえるCP計算\n" );

	//クリア
	for( i=0; i < 20 ;i++ ){
		tmp[i] = 0;
	}

	wk->hinsi_flag = 0;
	after_pp_max = 0;

	//p_m_partyのオフセット
	if( CommGetCurrentID() == COMM_PARENT_ID ){
		offset = 0;
	}else{
		offset = CASTLE_COMM_POKE_NUM;
	}

	m_max = Castle_GetMinePokeNum( wk->type, CASTLE_FLAG_SOLO );
	e_max = Castle_GetEnemyPokeNum( wk->type, CASTLE_FLAG_TOTAL );

	for( i=offset; i < (m_max+offset); i++ ){

		pp = PokeParty_GetMemberPointer( wk->p_m_party, i );
		OS_Printf( "monsno = %d\n", PokeParaGet(pp,ID_PARA_monsno,NULL) );

		if( PokeParaGet(pp,ID_PARA_poke_exist,NULL) == FALSE ){
			continue;
		}

		hp		= PokeParaGet( pp, ID_PARA_hp, NULL );
		hpmax	= PokeParaGet( pp, ID_PARA_hpmax, NULL );

		//気絶していないポケモン数
		if( hp > 0 ){

			tmp[ID_NO_HINSI]++;

			//HP満タンのポケモン数
			if( hp == hpmax ){

				tmp[ID_HP_MAX]++;

			}else{

				//HP半分以上のポケモン数(満タン含まず)
				if( hp >= (hpmax / 2) ){

					tmp[ID_HP_HALF_UP]++;

				}else{

					//HP半分以下(瀕死含まず)のポケモン数
					tmp[ID_HP_HALF_DOWN]++;

				}
			}

			//状態異常になっていないポケモン数
			if( PokeParaGet(pp,ID_PARA_condition,NULL) == 0 ){
				tmp[ID_CONDITION]++;
			}

		}else{
			wk->hinsi_flag = 1;		//瀕死ポケモンがいる
		}

		//戦闘後のPP合計を取得
		after_pp_max += PokeParaGet( pp, ID_PARA_pp1, NULL );
		after_pp_max += PokeParaGet( pp, ID_PARA_pp2, NULL );
		after_pp_max += PokeParaGet( pp, ID_PARA_pp3, NULL );
		after_pp_max += PokeParaGet( pp, ID_PARA_pp4, NULL );
	}

	//対戦ポケモン
	for( i=0; i < e_max; i++ ){

		//対戦相手のレベルを+5したポケモンの数
		if( wk->enemy_level_flag[i] == 1 ){

			tmp[ID_LV_PLUS]++;

		}else if( wk->enemy_level_flag[i] == 2 ){

			//対戦相手のレベルを-5したポケモンの数
			//tmp[ID_LV_MINUS]++;
		}
	}

	//戦闘前のPP合計を取得
	before_pp_max = 0;
	for( i=0; i < m_max; i++ ){
		for( j=0; j < 4; j++ ){
			before_pp_max += wk->pp[i][j];
		}
	}

	//合計PPの減った値が5以下の場合
	if( (before_pp_max - after_pp_max) <= 5 ){
		tmp[ID_PP_5]++;

	//合計PPの減った値が10以下の場合
	}else if( (before_pp_max - after_pp_max) <= 10 ){
		tmp[ID_PP_10]++;

	//合計PPの減った値が15以下の場合
	}else if( (before_pp_max - after_pp_max) <= 15 ){
		tmp[ID_PP_15]++;
	}

	//得点計算
	ret += tmp[ID_NO_HINSI]		*3;
	OS_Printf( "ID_NO_HINSI = %d\n", tmp[ID_NO_HINSI] );
	ret += tmp[ID_HP_MAX]		*3;
	OS_Printf( "ID_HP_MAX = %d\n", tmp[ID_HP_MAX] );
	ret += tmp[ID_HP_HALF_UP]	*2;
	OS_Printf( "ID_HP_HALF_UP = %d\n", tmp[ID_HP_HALF_UP] );
	ret += tmp[ID_HP_HALF_DOWN]	*1;
	OS_Printf( "ID_HP_HALF_DOWN = %d\n", tmp[ID_HP_HALF_DOWN] );
	ret += tmp[ID_CONDITION]	*1;
	OS_Printf( "ID_CONDITION = %d\n", tmp[ID_CONDITION] );
	ret += tmp[ID_PP_5]			*8;
	OS_Printf( "ID_PP_5 = %d\n", tmp[ID_PP_5] );
	ret += tmp[ID_PP_10]		*6;
	OS_Printf( "ID_PP_10 = %d\n", tmp[ID_PP_10] );
	ret += tmp[ID_PP_15]		*4;
	OS_Printf( "ID_PP_15 = %d\n", tmp[ID_PP_15] );
	ret += tmp[ID_LV_PLUS]		*7;
	OS_Printf( "ID_LV_PLUS = %d\n", tmp[ID_LV_PLUS] );
	//ret -= tmp[ID_LV_MINUS]		*12;
	//OS_Printf( "ID_LV_MINUS = %d\n", tmp[ID_LV_MINUS] );

	if( ret <= 0 ){
		ret = 1;
	}

	OS_Printf( "合計 = %d\n", ret );
	return ret;
}

//--------------------------------------------------------------
/**
 * @brief	CP増やす
 *
 * @param	wk			CASTLE_SCRWORK型のポインタ
 * @param	num			増やす値
 *
 * @return	none
 */
//--------------------------------------------------------------
void CastleScr_AddCP( SAVEDATA* sv, u8 type, int num )
{
	u16 now_cp;

	//現在のCPを取得
	now_cp = FrontierRecord_Get(SaveData_GetFrontier(sv), 
								CastleScr_GetCPRecordID(type),
								Frontier_GetFriendIndex(CastleScr_GetCPRecordID(type)) );

	//9999リミットチェック
	if( now_cp + num > CASTLE_CP_MAX ){
		FrontierRecord_Set(	SaveData_GetFrontier(sv), 
							CastleScr_GetCPRecordID(type),
							Frontier_GetFriendIndex(CastleScr_GetCPRecordID(type)), 
							CASTLE_CP_MAX );
	}else{
		FrontierRecord_Add(	SaveData_GetFrontier(sv), 
							CastleScr_GetCPRecordID(type),
							Frontier_GetFriendIndex(CastleScr_GetCPRecordID(type)), num );
	}

	//レコード更新(これはゲーム内レコードなので制限は外でかけてくれる)
	RECORD_Add( SaveData_GetRecord(sv), RECID_CASTLE_POINT, num );
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
 * @param	wk			CASTLE_SCRWORK型のポインタ
 * @param	type		送信タイプ
 *
 * @retval	none
 */
//--------------------------------------------------------------
BOOL CastleScr_CommSetSendBuf( CASTLE_SCRWORK* wk, u16 type, u16 param )
{
	int ret;

	switch( type ){

	case CASTLE_COMM_BASIC:
		ret = CommCastleSendBufBasicData( wk );
		break;

	case CASTLE_COMM_TR:
		ret = CommCastleSendBufTrData( wk );
		break;

	case CASTLE_COMM_SEL:
		ret = CommCastleSendBufSelData( wk );
		break;

	case CASTLE_COMM_ENEMY:
		ret = CommCastleSendBufEnemyPokeData( wk );
		break;

	case CASTLE_COMM_RETIRE:
		ret = CommCastleSendBufRetireFlag( wk, param );
		break;

	case CASTLE_COMM_TRADE_YESNO:
		ret = CommCastleSendBufTradeYesNoFlag( wk, param );
		break;

	case CASTLE_COMM_TEMOTI:
		ret = CommCastleSendBufTemotiPokeData( wk );
		break;
	};

	return ret;
}


//==============================================================================================
//
//	名前＋CPウィンドウ
//
//==============================================================================================
#include "savedata/castle_savedata.h"
#include "frontier_scr.h"
#include "frontier_map.h"						//fmap->bgl
#include "system/window.h"						//BmpMenuWinWrite
#include "system/arc_tool.h"					//NARC_
#include "system/fontproc.h"					//FONT_SYSTEM
#include "communication/comm_info.h"			//CommInfoGetMyStatus
#include "msgdata/msg.naix"						//NARC_msg_??_dat
#include "msgdata/msg_castle_poke.h"

void FSSC_Sub_CPWinWrite( FSS_PTR fss, CASTLE_SCRWORK* bc_scr_wk );
void FSSC_Sub_CPWinDel( FSS_PTR fss, CASTLE_SCRWORK* bc_scr_wk );
void FSSC_Sub_CPWrite( FSS_PTR fss, CASTLE_SCRWORK* bc_scr_wk );
static void FSSC_Sub_CPWinWrite2( GF_BGL_INI* bgl, GF_BGL_BMPWIN* win );
static void FSSC_Sub_CPWrite2( FSS_PTR fss, GF_BGL_BMPWIN* win, MYSTATUS* my, u16 cp );

#define FSEVWIN_CP_BUF_SIZE		(10*2)			//CPメッセージバッファサイズ

//--------------------------------------------------------------------------------------------
/**
 * CPウィンドウ表示
 *
 * @param	fsys
 * @param	wk
 * @param	x		表示X座標
 * @param	y		表示Y座標
 *
 * @return	確保したBMPウィンドウ
 *
 * EvWin_GoldWinDelで削除すること！
 */
//--------------------------------------------------------------------------------------------
void FSSC_Sub_CPWinWrite( FSS_PTR fss, CASTLE_SCRWORK* bc_scr_wk )
{
	FMAP_PTR fmap = FSS_GetFMapAdrs( fss );

	GF_ASSERT( fss->pParentNameCPWin == NULL );
	GF_ASSERT( fss->pChildNameCPWin == NULL );

	//シングル、ダブル
	if( Castle_CommCheck(bc_scr_wk->type) == FALSE ){
		fss->pParentNameCPWin	= GF_BGL_BmpWinAllocGet( HEAPID_WORLD, 1 );
		GF_BGL_BmpWinAdd(	fmap->bgl, fss->pParentNameCPWin, FRMAP_FRAME_WIN, 
							FFD_CP_WIN_PX, FFD_CP_WIN_PY,
							FFD_CP_WIN_SX, FFD_CP_WIN_SY, FFD_CP_WIN_PAL, FFD_CP_WIN_CGX );
		FSSC_Sub_CPWinWrite2( fmap->bgl, fss->pParentNameCPWin );
	//////////////////////////////////////////////////////////////////////////
	//通信
	}else{
		fss->pParentNameCPWin	= GF_BGL_BmpWinAllocGet( HEAPID_WORLD, 1 );
		fss->pChildNameCPWin	= GF_BGL_BmpWinAllocGet( HEAPID_WORLD, 1 );

		GF_BGL_BmpWinAdd(	fmap->bgl, fss->pParentNameCPWin, FRMAP_FRAME_WIN, 
							FFD_CP_WIN_PX, FFD_CP_WIN_PY,
							FFD_CP_WIN_SX, FFD_CP_WIN_SY, FFD_CP_WIN_PAL, FFD_CP_WIN_CGX );

		GF_BGL_BmpWinAdd(	fmap->bgl, fss->pChildNameCPWin, FRMAP_FRAME_WIN, 
							FFD_CP2_WIN_PX, FFD_CP2_WIN_PY,
							FFD_CP2_WIN_SX, FFD_CP2_WIN_SY, FFD_CP2_WIN_PAL, FFD_CP2_WIN_CGX );

		FSSC_Sub_CPWinWrite2( fmap->bgl, fss->pParentNameCPWin );
		FSSC_Sub_CPWinWrite2( fmap->bgl, fss->pChildNameCPWin );
	}

	FSSC_Sub_CPWrite( fss, bc_scr_wk );
	return;
}

static void FSSC_Sub_CPWinWrite2( GF_BGL_INI* bgl, GF_BGL_BMPWIN* win )
{
	//メニューウィンドウを描画
	BmpMenuWinWrite( win, WINDOW_TRANS_OFF, FR_MENU_WIN_CGX_NUM, FR_MENU_WIN_PAL );

	//指定範囲を塗りつぶし
	GF_BGL_BmpWinDataFill( win, FBMP_COL_WHITE );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * 所持金ウィンドウ削除
 *
 * @param	wk
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FSSC_Sub_CPWinDel( FSS_PTR fss, CASTLE_SCRWORK* bc_scr_wk )
{
	GF_BGL_BMPWIN* l_win;
	GF_BGL_BMPWIN* r_win;

	//シングル、ダブル
	if( Castle_CommCheck(bc_scr_wk->type) == FALSE ){
		GF_ASSERT( fss->pParentNameCPWin != NULL );
		l_win = fss->pParentNameCPWin;
		BmpMenuWinClear( l_win, WINDOW_TRANS_ON );
		GF_BGL_BmpWinFree( l_win, 1 );
	//////////////////////////////////////////////////////////////////////////
	//通信
	}else{
		GF_ASSERT( fss->pParentNameCPWin != NULL );
		GF_ASSERT( fss->pChildNameCPWin != NULL );

		//親
		if( CommGetCurrentID() == COMM_PARENT_ID ){
			l_win = fss->pParentNameCPWin;
			r_win = fss->pChildNameCPWin;
		//子
		}else{
			l_win = fss->pChildNameCPWin;
			r_win = fss->pParentNameCPWin;
		}

		BmpMenuWinClear( l_win, WINDOW_TRANS_ON );
		GF_BGL_BmpWinFree( l_win, 1 );
		BmpMenuWinClear( r_win, WINDOW_TRANS_ON );
		GF_BGL_BmpWinFree( r_win, 1 );
	}

	fss->pParentNameCPWin = NULL;
	fss->pChildNameCPWin = NULL;
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * プレイヤー名＋CP描画
 *
 * @param	fsys
 * @param	wk
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FSSC_Sub_CPWrite( FSS_PTR fss, CASTLE_SCRWORK* bc_scr_wk )
{
	u16 l_cp,r_cp;

	//シングル、ダブル
	if( Castle_CommCheck(bc_scr_wk->type) == FALSE ){
		GF_ASSERT( fss->pParentNameCPWin != NULL );
		l_cp = FrontierRecord_Get(	SaveData_GetFrontier(bc_scr_wk->sv), 
							CastleScr_GetCPRecordID(bc_scr_wk->type),
							Frontier_GetFriendIndex(CastleScr_GetCPRecordID(bc_scr_wk->type)) );
		FSSC_Sub_CPWrite2( fss, fss->pParentNameCPWin, SaveData_GetMyStatus(bc_scr_wk->sv), l_cp );
	//////////////////////////////////////////////////////////////////////////
	//通信
	}else{
		GF_ASSERT( fss->pParentNameCPWin != NULL );
		GF_ASSERT( fss->pChildNameCPWin != NULL );

		//親
		if( CommGetCurrentID() == COMM_PARENT_ID ){
			l_cp = FrontierRecord_Get(	SaveData_GetFrontier(bc_scr_wk->sv), 
							CastleScr_GetCPRecordID(bc_scr_wk->type),
							Frontier_GetFriendIndex(CastleScr_GetCPRecordID(bc_scr_wk->type)) );
			r_cp = bc_scr_wk->pair_cp;
			OS_Printf( "親：名前＋CPウィンドウ\n" );
		//子
		}else{
			l_cp = bc_scr_wk->pair_cp;
			r_cp = FrontierRecord_Get(	SaveData_GetFrontier(bc_scr_wk->sv), 
								CastleScr_GetCPRecordID(bc_scr_wk->type),
								Frontier_GetFriendIndex(CastleScr_GetCPRecordID(bc_scr_wk->type)) );
			OS_Printf( "子：名前＋CPウィンドウ\n" );
		}

		FSSC_Sub_CPWrite2( fss, fss->pParentNameCPWin, CommInfoGetMyStatus(0), l_cp );
		FSSC_Sub_CPWrite2( fss, fss->pChildNameCPWin, CommInfoGetMyStatus(1), r_cp );
	}

	return;
}

static void FSSC_Sub_CPWrite2( FSS_PTR fss, GF_BGL_BMPWIN* win, MYSTATUS* my, u16 cp )
{
	MSGDATA_MANAGER* man;
	STRBUF* tmp_buf = STRBUF_Create( FSEVWIN_CP_BUF_SIZE, fss->heap_id );
	STRBUF* tmp_buf2= STRBUF_Create( FSEVWIN_CP_BUF_SIZE, fss->heap_id );

	man = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_castle_poke_dat, HEAPID_WORLD );

	//////////////////////////////////////////////////////////////////////////////
	//CP
	WORDSET_RegisterNumber( fss->wordset, 0, cp,
							CASTLE_KETA_CP, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT );

	MSGMAN_GetString( man, msg_castle_poke_cp_01, tmp_buf );

	//登録された単語を使って文字列展開する
	WORDSET_ExpandStr( fss->wordset, tmp_buf2, tmp_buf );

	GF_STR_PrintSimple( win, FONT_SYSTEM, tmp_buf2, CASTLE_NAME_CP_START_X, 2*8, MSG_NO_PUT, NULL );

	//////////////////////////////////////////////////////////////////////////////
	//名前
	WORDSET_RegisterPlayerName( fss->wordset, 0, my );

	MSGMAN_GetString( man, msg_castle_poke_name_01, tmp_buf );

	//登録された単語を使って文字列展開する
	WORDSET_ExpandStr( fss->wordset, tmp_buf2, tmp_buf );

	GF_STR_PrintSimple( win, FONT_SYSTEM, tmp_buf2, 0, 0, MSG_ALLPUT, NULL );

	//////////////////////////////////////////////////////////////////////////////
	STRBUF_Delete( tmp_buf );
	STRBUF_Delete( tmp_buf2 );

	MSGMAN_Delete( man );

	GF_BGL_BmpWinOn( win );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * キャッスルクリア時に貰えるバトルポイントを取得
 *
 * @param	wk
 *
 * @return	"貰えるバトルポイント"
 */
//--------------------------------------------------------------------------------------------
u16	CastleScr_GetAddBtlPoint( CASTLE_SCRWORK* wk );
u16	CastleScr_GetAddBtlPoint( CASTLE_SCRWORK* wk )
{
	u8 add_bp;
	u16 lap;
	static const u8 bppoint_normal[CASTLE_LAP_MAX+1] = {0, 
													3, 3, 4, 4, 5, 5, 7, 7 };//0,1周～8周以降

	static const u8 bppoint_multi[CASTLE_LAP_MAX+1]  = {0, 
													8, 9, 11, 12, 14, 15, 18, 18 };//0,1周～8周以降

	//lap = CastleScr_CommGetLap( wk );
	lap = wk->lap;

	OS_Printf( "周回数 = %d\n", lap );
	OS_Printf( "連勝数 = %d\n", wk->rensyou );

	//シングル、ダブル
	if( (wk->type == CASTLE_TYPE_SINGLE) || (wk->type == CASTLE_TYPE_DOUBLE) ){

		//最大周回数チェック
		if( lap >= CASTLE_LAP_MAX ){
			add_bp = bppoint_normal[ CASTLE_LAP_MAX ];		//テーブルが+1しているので
		}else{
			add_bp = bppoint_normal[ lap ];
		}

	////////////////////////////////////////////////////////////////////////////////////
	//マルチ、WIFI
	}else{

		//最大周回数チェック
		if( lap >= CASTLE_LAP_MAX ){
			add_bp = bppoint_multi[ CASTLE_LAP_MAX ];		//テーブルが+1しているので
		}else{
			add_bp = bppoint_multi[ lap ];
		}
	}

	//ブレーンチェック
	if( wk->type == CASTLE_TYPE_SINGLE ){
		if( (wk->rensyou == CASTLE_LEADER_SET_1ST) || (wk->rensyou == CASTLE_LEADER_SET_2ND) ){
			add_bp = 20;
		}
	}

	return add_bp;
}


