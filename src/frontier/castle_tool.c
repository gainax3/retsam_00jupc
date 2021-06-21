//==============================================================================
/**
 * @file	castle_tool.c
 * @brief	バトルキャッスル関連ツール類
 * @author	nohara
 * @date	2007.07.02
 */
//==============================================================================
#include "common.h"
#include "system/procsys.h"
#include "system/pm_str.h"
#include "poketool/poke_tool.h"
#include "poketool/status_rcv.h"
#include "itemtool/itemsym.h"

#include "frontier_tool.h"
#include "frontier_def.h"
#include "castle_def.h"
#include "castle_tool.h"
#include "../field/scr_tool.h"

#include "msgdata/msg.naix"			//暫定


//==============================================================================
//	データ
//==============================================================================
//★トレーナーデータは0オリジンなので、データ作成時のナンバーから-1して参照する
///週回数、何人目の相手か、によるトレーナーの選出データ構造体
static const struct{
	u16 start_no;		///<１～６人目の開始No
	u16 end_no;			///<１～６人目の終了No
	u16 boss_start;		///<7人目の開始No
	u16 boss_end;		///<7人目の終了No
}TrainerSelectRange[] = {
	{1-1,	100-1,	101-1,	120-1},
	{81-1,	120-1,	121-1,	140-1},
	{101-1,	140-1,	141-1,	160-1},
	{121-1,	160-1,	161-1,	180-1},
	{141-1,	180-1,	181-1,	200-1},
	{161-1,	200-1,	201-1,	220-1},
	{181-1,	220-1,	221-1,	240-1},
	{201-1,	300-1,	201-1,	300-1},
};


//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static int Castle_EnemyTrainerIndexGet( u8 type, int lap_num, int enemy_number );
void Castle_EnemyLapAllTrainerIndexGet( u8 type, int lap, u16 trainer_index[], u8 num );


//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   出現する敵トレーナーのトレーナーデータNoを取得する
 *
 * @param   lap					週回数(0～)
 * @param   enemy_number		何番目に登場する敵か(０～CASTLE_LAP_ENEMY_MAX-1)
 *
 * @retval  トレーナーデータNo
 *
 * Lv50、オープン共通です
 */
//--------------------------------------------------------------
#define CASTLE_LEADER_TR_INDEX_1ST	(313)	//21戦目(0オリジン)
#define CASTLE_LEADER_TR_INDEX_2ND	(314)	//42戦目

static int Castle_EnemyTrainerIndexGet( u8 type, int lap_num, int enemy_number )
{
	int trainer_index, offset, start,check_num,lap;
	
	//ブレーン(シングルのみ)
	if( type == CASTLE_TYPE_SINGLE ){
		check_num = (lap_num * CASTLE_LAP_ENEMY_MAX) + (enemy_number + 1);
		if( check_num == CASTLE_LEADER_SET_1ST ){
			return CASTLE_LEADER_TR_INDEX_1ST;
		}else if( check_num == CASTLE_LEADER_SET_2ND ){
			return CASTLE_LEADER_TR_INDEX_2ND;
		}
	}

	lap = lap_num;
	if(lap >= NELEMS(TrainerSelectRange)){
		lap = NELEMS(TrainerSelectRange) - 1;
	}
	
	if(enemy_number == CASTLE_LAP_ENEMY_MAX-1){
		offset = TrainerSelectRange[lap].boss_end - TrainerSelectRange[lap].boss_start;
		start = TrainerSelectRange[lap].boss_start;
	}
	else{
		offset = TrainerSelectRange[lap].end_no - TrainerSelectRange[lap].start_no;
		start = TrainerSelectRange[lap].start_no;
	}
	
	trainer_index = start + (gf_rand() % offset);
	return trainer_index;
}

//--------------------------------------------------------------
/**
 * @brief   その周に登場する敵トレーナーIndexを全員分被らないように取得する
 *
 * @param   lap					周回数(0～)
 * @param   trainer_index		トレーナーIndex代入先(CASTLE_LAP_ENEMY_MAX分の要素数が必要)
 * @param   num					取得する数
 */
//--------------------------------------------------------------
void Castle_EnemyLapAllTrainerIndexGet( u8 type, int lap, u16 trainer_index[], u8 num )
{
	int set_count = 0;
	int i;
	
	do{
		trainer_index[set_count] = Castle_EnemyTrainerIndexGet( type, lap, set_count );
		//被りチェック
		for(i = 0; i < set_count; i++){
			if(trainer_index[i] == trainer_index[set_count]){
				break;
			}
		}
		if(i != set_count){
			continue;
		}
		set_count++;
	}while(set_count < num);
}


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
//==============================================================================
//
//	07.04.10追加
//
//==============================================================================
#include "gflib/strbuf_family.h"
#include "savedata/savedata_def.h"
#include "savedata/savedata.h"
#include "savedata/zukanwork.h"
#include "savedata/record.h"
#include "savedata/b_tower_local.h"
#include "field/encount_set.h"
#include "field/ev_time.h"
#include "field/fieldobj_code.h"
#include "field/location.h"
#include "communication/comm_tool.h"
#include "communication/comm_system.h"
#include "communication/comm_info.h"
#include "communication/comm_def.h"
#include "battle/attr_def.h"
#include "battle/battle_common.h"
#include "battle/b_tower_data.h"


//==============================================================================
//
//	プロトタイプ宣言
//
//==============================================================================
u8 Castle_GetMinePokeNum( u8 type, BOOL flag );
u8 Castle_GetEnemyPokeNum( u8 type, BOOL flag );
BATTLE_PARAM* BtlCastle_CreateBattleParam( CASTLE_SCRWORK* wk, FRONTIER_EX_PARAM* ex_param );
static u32 Castle_GetFightType( u8 type );
u8 Castle_GetLevel( CASTLE_SCRWORK* wk );
BOOL Castle_CommCheck( u8 type );
void Castle_PokePartyRecoverAll( POKEPARTY* party );
void Castle_EnemyPartySet( CASTLE_SCRWORK* wk );
void Castle_BirthInfoSet( CASTLE_SCRWORK* wk, POKEMON_PARAM* poke );
void Castle_PokePartyAdd( CASTLE_SCRWORK* wk, POKEPARTY* party, POKEMON_PARAM* poke );
static u16 Castle_GetTrAI( CASTLE_SCRWORK* wk );
u16 CastleScr_CommGetLap( CASTLE_SCRWORK* wk );
u16 Castle_GetLap( u16 lap );


//==============================================================================
//
//	extern宣言
//
//==============================================================================
//トレーナーデータ生成（自分側）
extern void BattleParam_TrainerDataMake( BATTLE_PARAM* bp );
//トレーナーデータ生成（敵側）
extern void BattleParam_EnemyTrainerDataMake( BATTLE_PARAM* bp );


//==============================================================================
//
//	関数
//
//==============================================================================

//--------------------------------------------------------------
/**
 * @brief   タイプによって自分のポケモンの数を取得
 *
 * @param   type
 *
 * @param   "ポケモンの数"
 */
//--------------------------------------------------------------
u8 Castle_GetMinePokeNum( u8 type, BOOL flag )
{
	switch( type ){

	case CASTLE_TYPE_SINGLE:
	case CASTLE_TYPE_DOUBLE:
		return 3;

	case CASTLE_TYPE_MULTI:
	case CASTLE_TYPE_WIFI_MULTI:
		if( flag == CASTLE_FLAG_SOLO ){
			return CASTLE_COMM_POKE_NUM;				//2
		}else{
			return CASTLE_COMM_POKE_TOTAL_NUM;			//2+2=4
		}
	};

	//エラー
	GF_ASSERT(0);
	return 3;
}

//--------------------------------------------------------------
/**
 * @brief   タイプによって敵ポケモンの数を取得
 *
 * @param   type
 * @param   flag
 *
 * @param   "ポケモンの数"
 */
//--------------------------------------------------------------
u8 Castle_GetEnemyPokeNum( u8 type, BOOL flag )
{
	switch( type ){

	case CASTLE_TYPE_SINGLE:
	case CASTLE_TYPE_DOUBLE:
		return 3;

	case CASTLE_TYPE_MULTI:
	case CASTLE_TYPE_WIFI_MULTI:
		if( flag == CASTLE_FLAG_SOLO ){
			return CASTLE_COMM_POKE_NUM;				//2
		}else{
			return CASTLE_COMM_POKE_TOTAL_NUM;			//2+2=4
		}
	};

	//エラー
	GF_ASSERT(0);
	return 3;
}

//--------------------------------------------------------------
/**
 * @brief	バトルキャッスル用　バトルパラメータ生成
 */
//--------------------------------------------------------------
BATTLE_PARAM* BtlCastle_CreateBattleParam( CASTLE_SCRWORK* wk, FRONTIER_EX_PARAM* ex_param )
{
	int i;
	u32	val32;
	u8 m_max,e_max,mine_offset;
	BATTLE_PARAM* bp;
	POKEMON_PARAM* pp;
	B_TOWER_TRAINER bt_trd;
	B_TOWER_TRAINER_ROM_DATA* p_rom_tr;
	MSGDATA_MANAGER* man;
	STRBUF* msg;
	MYSTATUS* my;

	//参加ポケモン数を取得
	m_max = Castle_GetMinePokeNum( wk->type, CASTLE_FLAG_SOLO );
	e_max = Castle_GetEnemyPokeNum( wk->type, CASTLE_FLAG_SOLO );

	//回復処理
#if 0
	PokeParty_RecoverAll( wk->p_m_party );
	PokeParty_RecoverAll( wk->p_e_party );
#else
	//Castle_PokePartyRecoverAll( wk->p_m_party );	//CastleScr_BtlAfterPartySetに移動
	PokeParty_RecoverAll( wk->p_e_party );
#endif

	//戦闘パラメータの生成処理(field_battle.c)(fssc_castle.c FSSC_CastleCallGetResultでDelete)
	bp = BattleParam_Create( HEAPID_WORLD, Castle_GetFightType(wk->type) );
	
	//戦闘パラメータの収集コア(フィールド)
	BattleParam_SetParamByGameDataCore( bp, NULL, 
										ex_param->savedata,
										ex_param->zone_id,
										ex_param->fnote_data,
										ex_param->bag_cursor,
										ex_param->battle_cursor);

	//現状は固定！
	bp->bg_id		= BG_ID_CASTLE;			//基本背景指定
	bp->ground_id	= GROUND_ID_CASTLE;		//基本地面指定

	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//MINE：選んだ手持ちポケモンをセット
	PokeParty_Init( bp->poke_party[POKEPARTY_MINE], m_max );

	//親は手前の2匹、子は後ろの2匹を、p_m_partyから取得する
	if( CommGetCurrentID() == COMM_PARENT_ID ){
		mine_offset = 0;
	}else{
		mine_offset = CASTLE_COMM_POKE_NUM;
	}

	//MINE：自分のポケモンデータセット
	pp = PokemonParam_AllocWork( HEAPID_WORLD );
	for( i=0; i < m_max ;i++ ){

		//POKEMON_PARAM構造体からPOKEMON_PARAM構造体へコピー
		PokeCopyPPtoPP( PokeParty_GetMemberPointer(wk->p_m_party,(mine_offset+i)), pp );

		//戦闘パラメータセット:ポケモンを加える
		BattleParam_AddPokemon( bp, pp, POKEPARTY_MINE );
	}
	sys_FreeMemoryEz( pp );

	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//MINE：トレーナーデータ生成
	BattleParam_TrainerDataMake( bp );

	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//ENEMY1：ROMからトレーナーデータを確保
	p_rom_tr = Frontier_TrainerDataGet( &bt_trd, wk->tr_index[wk->round], HEAPID_WORLD, 
										ARC_PL_BTD_TR );
	sys_FreeMemoryEz( p_rom_tr );

	//ENEMY1：トレーナーデータをセット
	BattleParamTrainerDataSet( bp, &bt_trd, e_max, CLIENT_NO_ENEMY, HEAPID_WORLD );

	//ENEMY1：選んだ手持ちポケモンをセット
	PokeParty_Init( bp->poke_party[POKEPARTY_ENEMY], 
					Castle_GetEnemyPokeNum(wk->type,CASTLE_FLAG_SOLO) );

	//AIセット
	for( i=0; i < CLIENT_MAX ;i++ ){
		bp->trainer_data[i].aibit = Castle_GetTrAI( wk );
	}
	OS_Printf( "トレーナーのAI = %d\n", Castle_GetTrAI(wk) );

	//ENEMY1：戦闘パラメータセット:ポケモンを加える
	pp = PokemonParam_AllocWork( HEAPID_WORLD );
	for( i=0; i < e_max ;i++ ){

		//POKEMON_PARAM構造体からPOKEMON_PARAM構造体へコピー
		PokeCopyPPtoPP( PokeParty_GetMemberPointer(wk->p_e_party,i), pp );
		BattleParam_AddPokemon( bp, pp, POKEPARTY_ENEMY );
	}
	sys_FreeMemoryEz( pp );

	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//デバック情報表示
//OS_Printf("p_party count = %d\n", PokeParty_GetPokeCount(wk->p_party) );
//OS_Printf("bp_party[mine]count= %d\n",PokeParty_GetPokeCount(bp->poke_party[POKEPARTY_MINE]));
//OS_Printf("bp_party[enemy]count= %d\n",PokeParty_GetPokeCount(bp->poke_party[POKEPARTY_ENEMY]));

	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	switch( wk->type ){

	case CASTLE_TYPE_MULTI:
	case CASTLE_TYPE_WIFI_MULTI:

		//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
		//パートナーのデータを格納しておかないとダメ！
		//B_TOWER_PARTNER_DATA	tr_data[2];

		//MINE2：トレーナーデータ生成
		BattleParam_TrainerDataMake( bp );

		//MINE2：パートナーのMyStatusを取得してコピー
		my = CommInfoGetMyStatus( 1 - CommGetCurrentID() );
		MyStatus_Copy( my, bp->my_status[CLIENT_NO_MINE2] );

		//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
		//パートナーのデータを格納しておかないとダメ！
		//B_TOWER_PARTNER_DATA	tr_data[2];

		//ENEMY2：ROMからトレーナーデータを確保
		p_rom_tr = Frontier_TrainerDataGet( &bt_trd, wk->tr_index[wk->round+CASTLE_LAP_ENEMY_MAX],
											HEAPID_WORLD, ARC_PL_BTD_TR );
		sys_FreeMemoryEz( p_rom_tr );

		//ENEMY2：トレーナーデータをセット
		BattleParamTrainerDataSet( bp, &bt_trd, e_max, CLIENT_NO_ENEMY2, HEAPID_WORLD );

		//ENEMY2：選んだ手持ちポケモンをセット
		PokeParty_Init( bp->poke_party[POKEPARTY_ENEMY_PAIR], 
						Castle_GetEnemyPokeNum(wk->type,CASTLE_FLAG_SOLO) );

		//ENEMY2：戦闘パラメータセット:ポケモンを加える
		pp = PokemonParam_AllocWork( HEAPID_WORLD );
		for( i=0; i < e_max ;i++ ){

			//POKEMON_PARAM構造体からPOKEMON_PARAM構造体へコピー
			PokeCopyPPtoPP( PokeParty_GetMemberPointer(wk->p_e_party,(e_max+i)), pp );
			BattleParam_AddPokemon( bp, pp, POKEPARTY_ENEMY_PAIR );
		}
		sys_FreeMemoryEz( pp );

		//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

		break;
	};

	return bp;
}

//--------------------------------------------------------------
/**
 * @brief	プレイモードからFIGHT_TYPEを返す
 */
//--------------------------------------------------------------
static u32 Castle_GetFightType( u8 type )
{
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//
	//キャッスル用のFIGHT_TYPEを作成するか確認する！
	//
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

	switch( type ){

	case CASTLE_TYPE_SINGLE:
		return FIGHT_TYPE_BATTLE_TOWER_1vs1;

	case CASTLE_TYPE_DOUBLE:
		return FIGHT_TYPE_BATTLE_TOWER_2vs2;

	case CASTLE_TYPE_MULTI:
		return FIGHT_TYPE_BATTLE_TOWER_SIO_MULTI;	
		//return FIGHT_TYPE_MULTI_SIO;	

	case CASTLE_TYPE_WIFI_MULTI:
		return FIGHT_TYPE_BATTLE_TOWER_SIO_MULTI;
		//return FIGHT_TYPE_MULTI_SIO;	
	}

	return FIGHT_TYPE_BATTLE_TOWER_1vs1;
}

//--------------------------------------------------------------
/**
 * @brief	レベルの定義から実際のレベルを取得
 *
 * @param	wk		
 *
 * @retval	"実際のレベル"
 */
//--------------------------------------------------------------
u8 Castle_GetLevel( CASTLE_SCRWORK* wk )
{
	return 50;
}

//--------------------------------------------------------------
/**
 * @brief	通信タイプかチェック
 *
 * @param	type		タイプ
 *
 * @return	"FALSE = 通信ではない"
 * @return	"TRUE  = 通信である"
 */
//--------------------------------------------------------------
BOOL Castle_CommCheck( u8 type )
{
	switch( type ){
	case CASTLE_TYPE_MULTI:
	case CASTLE_TYPE_WIFI_MULTI:
		return TRUE;
	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	きぜつはＨＰ１で回復、状態異常回復
 *
 * @param	party		回復するPOKEPARTY型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void Castle_PokePartyRecoverAll( POKEPARTY* party )
{
	int i, j,total;
	u32 buf;
	POKEMON_PARAM* pp;

	total = PokeParty_GetPokeCount( party );

	for( i=0; i < total; i++ ){

		pp = PokeParty_GetMemberPointer( party, i );

		if( PokeParaGet(pp,ID_PARA_poke_exist,NULL) == FALSE ){
			continue;
		}

		//HP=0だったら、HP=1で回復
		if( PokeParaGet(pp,ID_PARA_hp,NULL) == 0 ){
			buf = 1;
			PokeParaPut( pp, ID_PARA_hp, &buf );
		}

		//状態異常回復
		buf = 0;
		PokeParaPut( pp, ID_PARA_condition, &buf );
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	ポケモン孵化情報を格納
 *
 * @param	wk			CASTLE_SCRWORK型のポインタ
 * @param	poke		POKEMON_PARAM型のポインタ
 *
 * @retval	none
 *
 * 親名とかセットしないと、ステータス画面の表示がうまくいかないのでセット
 */
//--------------------------------------------------------------
void Castle_BirthInfoSet( CASTLE_SCRWORK* wk, POKEMON_PARAM* poke )
{
	/*******************************************/
	//ポケモンの生まれた情報をどうするか要確認！
	/*******************************************/

	PokeParaBirthInfoSet(	poke, SaveData_GetMyStatus(wk->sv),
							//int ball,int place,int ground_id,int heapID)
							ITEM_MONSUTAABOORU, 0, 0, HEAPID_WORLD );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ポケモンパーティにポケモンを追加
 *
 * @param	wk			CASTLE_SCRWORK型のポインタ
 * @param	party		POKEPARTY型のポインタ
 * @param	poke		POKEMON_PARAM型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
void Castle_PokePartyAdd( CASTLE_SCRWORK* wk, POKEPARTY* party, POKEMON_PARAM* poke )
{
	Castle_BirthInfoSet( wk, poke );	//親名とかセット
	PokeParty_Add( party, poke );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	敵POKEPARTYのセット
 *
 * @param	wk		CASTLE_SCRWORKのポインタ
 */
//--------------------------------------------------------------
void Castle_EnemyPartySet( CASTLE_SCRWORK* wk )
{
	int i,j;
	u8 e_max;
	POKEMON_PARAM* temp_poke;

	PokeParty_InitWork( wk->p_e_party );								//POKEPARTYを初期化

	//タイプによってポケモンの数を取得
	e_max = Castle_GetEnemyPokeNum( wk->type, CASTLE_FLAG_TOTAL );		//敵

	//敵ポケモンをセット
	temp_poke = PokemonParam_AllocWork( HEAPID_WORLD );
	for( i=0; i < e_max ;i++ ){

		//敵ポケモンデータの必要なデータをPOKEPARTYにセット(fieldからは呼べない！)
		//バトルタワー用ポケモンデータからPOKEMON_PARAMを生成
		Frontier_PokeParaMake( &wk->enemy_poke[i], temp_poke, Castle_GetLevel(wk) );
		Castle_PokePartyAdd( wk, wk->p_e_party, temp_poke );

		OS_Printf("次の敵e_party[%d] monsno = %d\n", i, PokeParaGet(temp_poke,ID_PARA_monsno,NULL));
	}
	sys_FreeMemoryEz( temp_poke );

	return;
}

//--------------------------------------------------------------
/**
 * @brief	周回数によってトレーナーAIを決定して取得
 *
 * @param	
 *
 * @return	"トレーナーAI"
 */
//--------------------------------------------------------------
static u16 Castle_GetTrAI( CASTLE_SCRWORK* wk )
{
	u16 ai,lap;

#if 1
	//ブレーン
	if( wk->type == CASTLE_TYPE_SINGLE ){
		if( (wk->tr_index[wk->round] == CASTLE_LEADER_TR_INDEX_1ST) ||
			(wk->tr_index[wk->round] == CASTLE_LEADER_TR_INDEX_2ND) ){
			return FR_AI_EXPERT;
		}
	}
#endif

	//周回数を取得
	lap = CastleScr_CommGetLap( wk );

	//5周目以降は全てエキスパート
	ai = FR_AI_EXPERT;

	//0オリジンなので+1して計算
	switch( (lap+1) ){

	case 1:
	case 2:
		ai = FR_AI_YASEI;
		break;

	case 3:
	case 4:
		ai = FR_AI_BASIC;
		break;
	};

	return ai;
}

//--------------------------------------------------------------
/**
 * @brief	"親子"の周回数を比較して、使用する周回数を取得
 *
 * @param	wk		CASTLE_SCRWORK型のポインタ
 *
 * @return	"周回数"
 *
 * 通信して相手の周回数を取得したあとに使用可能
 */
//--------------------------------------------------------------
u16 CastleScr_CommGetLap( CASTLE_SCRWORK* wk )
{
	u16	lap;
	
	lap = wk->lap;

	if( Castle_CommCheck(wk->type) == TRUE ){

		//通信時には周回数の多いほうで抽選
		if( wk->pair_lap > wk->lap ){
			lap = wk->pair_lap;
		}
	}

	return lap;
}

//--------------------------------------------------------------
/**
 * @brief	キャッスル周回数を示すスクリーンを更新
 *
 * @param	wk		BATON_WORK型のポインタ
 * @param	frm		BGフレーム
 *
 * @return	none
 */
//--------------------------------------------------------------
#define CASTLE_LAP_CHAR_XSIZE	(10)							//
#define CASTLE_LAP_CHAR_HALF	(CASTLE_LAP_CHAR_XSIZE / 2)		//折り返し地点
//#define CASTLE_LAP_CHAR_YSIZE	(2)								//
#define CASTLE_LAP_CHAR_YSIZE	(3)								//
#define CASTLE_LAP_SCRN_SIZE	(CASTLE_LAP_CHAR_XSIZE * CASTLE_LAP_CHAR_YSIZE)
#define CASTLE_CHAR_START		(0x10)
#define CASTLE_CHAR_ADD			(0x60)
#define NUM_WRITE_X				(11)							//書き込みX
#define NUM_WRITE_Y				(6)								//書き込みY
#define H_FLIP_BIT				(10)							//Hフリップのビット

void Castle_LapNumWrite( GF_BGL_INI* ini, CASTLE_SCRWORK* wk, u32 frm );
static void Castle_LapNumWriteSub( u16* src, u16 lap_num );

void Castle_LapNumWrite( GF_BGL_INI* ini, CASTLE_SCRWORK* wk, u32 frm )
{
	int i;
	u16 src[CASTLE_LAP_SCRN_SIZE];

	Castle_LapNumWriteSub( src, CastleScr_CommGetLap(wk) );

	GF_BGL_ScrWrite(ini, frm, src, NUM_WRITE_X, NUM_WRITE_Y, 
					CASTLE_LAP_CHAR_XSIZE, CASTLE_LAP_CHAR_YSIZE );	//スクリーンデータ書き込み

	GF_BGL_LoadScreenV_Req( ini, frm );								//スクリーンデータ転送
	return;
}

//--------------------------------------------------------------
/**
 * @brief	バッファに書き込み
 *
 * @param	src		書き込みバッファ
 * @param	lap_num	周回数(wk->lapは0オリジン)
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Castle_LapNumWriteSub( u16* src, u16 lap_num )
{
	u8 char_tbl[CASTLE_LAP_CHAR_XSIZE];
	u16 lap;
	u32 char_offset,i,j,no;

	lap = Castle_GetLap( lap_num );
	OS_Printf( "lap = %d\n", lap );

	//フリップするので参照位置を作成
	for( i=0; i < CASTLE_LAP_CHAR_HALF ;i++ ){
		char_tbl[i] = i;													//0,1,2,3,4
		char_tbl[CASTLE_LAP_CHAR_HALF+i] = ( (CASTLE_LAP_CHAR_HALF-1) - i);	//4,3,2,1,0
	}

	//周回数キャラクタデータのオフセット取得
	char_offset = (CASTLE_CHAR_ADD * lap) + CASTLE_CHAR_START;
	OS_Printf( "char_offset = %d\n", char_offset );

	for( i=0; i < CASTLE_LAP_CHAR_YSIZE ;i++ ){
		for( j=0; j < CASTLE_LAP_CHAR_XSIZE ;j++ ){

			//0x20=CHARファイルのYオフセット
			no = char_offset + (i*0x20) + char_tbl[j];
			OS_Printf( "no = %d\n", no );
			src[ j + (i*CASTLE_LAP_CHAR_XSIZE) ] = no;

			//フリップ
			if( j >= CASTLE_LAP_CHAR_HALF ){
				src[ j + (i*CASTLE_LAP_CHAR_XSIZE) ] |= (1 << H_FLIP_BIT);	//Hフリップ
			}
		}
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	上限を考慮して周回数を取得
 *
 * @param	lap		周回数
 *
 * @return	"周回数"
 */
//--------------------------------------------------------------
u16 Castle_GetLap( u16 lap )
{
	if( lap >= CASTLE_LAP_MAX ){
		return (CASTLE_LAP_MAX - 1);
	}

	return lap;
}

//--------------------------------------------------------------
/**
 * @brief	CPレコードを減らす(USE_CPレコードも計算)
 *
 * @param	fro_sv		FRONTIER_SAVEWORK型のポインタ
 * @param	type		バトルタイプ
 * @param	value		減らすCPの値
 *
 * @return	none
 */
//--------------------------------------------------------------
void Castle_CPRecord_Sub( FRONTIER_SAVEWORK* fro_sv, u8 type, int value )
{
	u16 use_cp;

	//CPレコードを減らす
	FrontierRecord_Sub(	fro_sv, CastleScr_GetCPRecordID(type),
						Frontier_GetFriendIndex(CastleScr_GetCPRecordID(type)), value );

	//現在の使用したCPレコードを取得
	use_cp = FrontierRecord_Get(fro_sv, CastleScr_GetUsedCPRecordID(type),
								Frontier_GetFriendIndex(CastleScr_GetUsedCPRecordID(type)) );

	//使用したCPレコードを増やす
	//9999リミットチェック
	if( use_cp + value > CASTLE_USE_CP_MAX ){
		FrontierRecord_Set(	fro_sv, CastleScr_GetUsedCPRecordID(type),
							Frontier_GetFriendIndex(CastleScr_GetUsedCPRecordID(type)), 
							CASTLE_USE_CP_MAX );
	}else{
		FrontierRecord_Add(	fro_sv, CastleScr_GetUsedCPRecordID(type),
							Frontier_GetFriendIndex(CastleScr_GetUsedCPRecordID(type)), value );
	}

	return;
}

