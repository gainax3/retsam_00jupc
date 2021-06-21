//==============================================================================
/**
 * @file	factory_tool.c
 * @brief	バトルファクトリー関連ツール類
 * @author	Satoshi Nohara
 * @date	2007.08.31
 *
 * 松田さんから引継ぎ
 */
//==============================================================================
#include "common.h"
#include "system/procsys.h"
#include "system/pm_str.h"
#include "poketool/poke_tool.h"
#include "poketool/status_rcv.h"

#include "frontier_tool.h"
#include "frontier_def.h"
#include "factory_def.h"
#include "factory_tool.h"

#include "msgdata/msg.naix"			//暫定


#define FACTORY_LEADER_TR_INDEX_1ST	(309)	//21戦目(0オリジン)
#define FACTORY_LEADER_TR_INDEX_2ND	(310)	//42戦目

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

//★ポケモンデータは1オリジンで始まる(0は無効なデータなので参照してはダメ！)
///レベル50の敵が使用してくるポケモンデータのIndex取得範囲(ポケモンは1オリジン)
static const FACTORY_POKE_RANGE Level50EnemyPokemonRange[] = {
	{100,	1,		150,	0,		1},
	{120,	151,	250,	4,		1},
	{140,	251,	350,	8,		1},
	{160,	351,	486,	12,		1},
	{180,	487,	622,	16,		1},
	{200,	623,	758,	20,		1},
	{220,	759,	894,	24,		1},
	{300,	351,	950,	31,		0},		//ノーマル敵の終端
//	{312,	351,	486,	31,		0},		//ブレーン用
	{(FACTORY_LEADER_TR_INDEX_1ST+1),	351,	486,	12,		0},		//ブレーン用
	{(FACTORY_LEADER_TR_INDEX_2ND+1),	351,	486,	31,		0},		//ブレーン用
};

///オープンレベルの敵が使用してくるポケモンデータのIndex取得範囲(ポケモンは1オリジン)
static const FACTORY_POKE_RANGE OpenLevelEnemyPokemonRange[] = {
	{100,	351,	486,	0,		1},
	{120,	487,	622,	4,		1},
	{140,	623,	758,	8,		1},
	{160,	759,	894,	12,		1},
	{180,	351,	950,	16,		1},
	{200,	351,	950,	20,		1},
	{220,	351,	950,	24,		1},
	{300,	351,	950,	31,		0},		//ノーマル敵の終端
//	{312,	759,	950,	31,		0},		//ブレーン用
	{(FACTORY_LEADER_TR_INDEX_1ST+1),	759,	950,	12,		0},		//ブレーン用
	{(FACTORY_LEADER_TR_INDEX_2ND+1),	759,	950,	31,		0},		//ブレーン用
};

///オープンレベルの味方レンタルポケモンデータのIndex取得範囲(ポケモンは1オリジン)
static const FACTORY_POKE_RANGE OpenLevelMinePokemonRange[] = {
	{100,	351,	486,	0,		1},
	{120,	487,	622,	4,		1},
	{140,	623,	758,	8,		1},
	{160,	759,	894,	12,		1},
	{180,	623,	950,	16,		1},
	{200,	623,	950,	20,		1},
	{220,	623,	950,	24,		1},
	{300,	351,	950,	31,		0},		//ノーマル敵の終端
//	{312,	759,	950,	31,		0},		//ブレーン用
	{(FACTORY_LEADER_TR_INDEX_1ST+1),	759,	950,	12,		0},		//ブレーン用
	{(FACTORY_LEADER_TR_INDEX_2ND+1),	759,	950,	31,		0},		//ブレーン用
};

///レベル50で自分が使用出来るポケモンデータのIndex取得範囲(現状敵と範囲は一緒)
#define Level50MinePokemonRange		Level50EnemyPokemonRange

/******************************************************************************/
//31がテーブルに複数あるとダメなので注意！(FACTORY_POKE_RANGEで比較してもよさそう)
/******************************************************************************/
//交換回数によって1つ上のレベルを抽出する時に使用している
#define LV50_ENEMY_POKE_RANGE_MAX	( NELEMS(Level50MinePokemonRange) )	//最大数
//#define MINE_POKE_RANGE_END		( Level50MinePokemonRange[LV50_ENEMY_POKE_RANGE_MAX-1].end )

//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static int Factory_EnemyTrainerIndexGet( u8 type, int lap, int enemy_number);
const FACTORY_POKE_RANGE *Factory_EnemyPokemonRangeGet(int trainer_index, 
	int factory_level);
const FACTORY_POKE_RANGE *Factory_MinePokemonRangeGet(int lap, int factory_level);
static int Factory_EnemyPokemonIndexGet(B_TOWER_TRAINER_ROM_DATA *trd, int trainer_index, 
	int factory_level);
void Factory_EnemyLapAllTrainerIndexGet( u8 type, int lap, u16 trainer_index[], u8 num );


//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   出現する敵トレーナーのトレーナーデータNoを取得する
 *
 * @param   lap					週回数(0～)
 * @param   enemy_number		何番目に登場する敵か(０～FACTORY_LAP_ENEMY_MAX-1)
 *
 * @retval  トレーナーデータNo
 *
 * Lv50、オープン共通です
 */
//--------------------------------------------------------------
static int Factory_EnemyTrainerIndexGet( u8 type, int lap, int enemy_number)
{
	int trainer_index, offset, start, check_num;
	
	if(lap >= NELEMS(TrainerSelectRange)){
		lap = NELEMS(TrainerSelectRange) - 1;
	}
	
	//ブレーン(シングルのみ)
	if( type == FACTORY_TYPE_SINGLE ){
		check_num = (lap * FACTORY_LAP_ENEMY_MAX) + (enemy_number + 1);
		if( check_num == FACTORY_LEADER_SET_1ST ){
			return FACTORY_LEADER_TR_INDEX_1ST;
		}else if( check_num == FACTORY_LEADER_SET_2ND ){
			return FACTORY_LEADER_TR_INDEX_2ND;
		}
	}

	if(enemy_number == FACTORY_LAP_ENEMY_MAX-1){
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
 * @param   trainer_index		トレーナーIndex代入先(FACTORY_LAP_ENEMY_MAX分の要素数が必要)
 */
//--------------------------------------------------------------
void Factory_EnemyLapAllTrainerIndexGet( u8 type, int lap, u16 trainer_index[], u8 num )
{
	int set_count = 0;
	int i;
	
	do{
		trainer_index[set_count] = Factory_EnemyTrainerIndexGet( type, lap, set_count);
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

//--------------------------------------------------------------
/**
 * @brief   敵が使用してくるポケモンデータの抽出範囲を取得する
 *
 * @param   trainer_index		トレーナーIndex
 * @param   factory_level		FACTORY_LEVEL_???
 *
 * @retval  抽出範囲データへのポインタ
 */
//--------------------------------------------------------------
const FACTORY_POKE_RANGE *Factory_EnemyPokemonRangeGet(int trainer_index, int factory_level)
{
	int tbl_no;
	static const FACTORY_POKE_RANGE *range_tbl;
	int range_num;
	
	if(factory_level == FACTORY_LEVEL_50){
		range_tbl = Level50EnemyPokemonRange;
		range_num = NELEMS(Level50EnemyPokemonRange);
	}
	else{
		range_tbl = OpenLevelEnemyPokemonRange;
		range_num = NELEMS(OpenLevelEnemyPokemonRange);
	}
	
	for(tbl_no = 0; tbl_no < range_num; tbl_no++){
		if(trainer_index < range_tbl[tbl_no].trainer_index){
			break;
		}
	}
	if(tbl_no >= range_num){
		GF_ASSERT(0);
		tbl_no = range_num - 1;
	}
	
	return &range_tbl[tbl_no];
}

//--------------------------------------------------------------
/**
 * @brief   最初にレンタルする6体のポケモンデータの抽出範囲を取得する
 *
 * @param   lap					周回数
 * @param   factory_level		FACTORY_LEVEL_???
 *
 * @retval  抽出範囲データへのポインタ
 */
//--------------------------------------------------------------
const FACTORY_POKE_RANGE *Factory_MinePokemonRangeGet(int lap, int factory_level)
{
	//↓でレンタルポケモンの抽出がブレーンのデータに引っかからない
	//データが最大周回数(8)までしか用意されていないので制限する
	if(lap >= FACTORY_LAP_MAX){
		lap = FACTORY_LAP_MAX - 1;
	}
	
	if(factory_level == FACTORY_LEVEL_50){
		return &Level50MinePokemonRange[lap];
	}
	else{
		return &OpenLevelMinePokemonRange[lap];
	}
}

//--------------------------------------------------------------
/**
 * @brief   敵トレーナーが使ってくるポケモンIndexを取得
 *
 * @param   trd					トレーナーデータ
 * @param   trainer_index		トレーナーIndex
 * @param   factory_level		FACTORY_LEVEL_???
 *
 * @retval  ポケモンデータIndex
 */
//--------------------------------------------------------------
static int Factory_EnemyPokemonIndexGet(B_TOWER_TRAINER_ROM_DATA *trd, int trainer_index, 
	int factory_level)
{
	int poke_index;
	
	if(factory_level == FACTORY_LEVEL_50){
		poke_index = trd->use_poke_table[gf_rand() % trd->use_poke_cnt];
	}
	else{
		//オープンレベルはトレーナーデータに入っている手持ちポケモン一覧からではなく
		//トレーナー番号をキーに一定範囲のポケモンからランダムで抽出
		int tbl_no, offset;
		
		for(tbl_no = 0; tbl_no < NELEMS(OpenLevelEnemyPokemonRange); tbl_no++){
			if(trainer_index < OpenLevelEnemyPokemonRange[tbl_no].trainer_index){
				break;
			}
		}
		if(tbl_no == NELEMS(OpenLevelEnemyPokemonRange)){
			GF_ASSERT(0);
			tbl_no = NELEMS(OpenLevelEnemyPokemonRange) - 1;
		}
		
		offset = OpenLevelEnemyPokemonRange[tbl_no].end - OpenLevelEnemyPokemonRange[tbl_no].start;
		poke_index = OpenLevelEnemyPokemonRange[tbl_no].start + (gf_rand() % offset);
	}
	
	return poke_index;
}

//交換回数テーブル
static const trade_count_tbl[] = {
	//回数  //↓お得になるポケモン数(周回数ではない！)
	6,		//0
	13,		//1
	20,		//2
	27,		//3
	34,		//4
	//35以上//5
};
#define TRADE_COUNT_TBL_MAX		( NELEMS(trade_count_tbl) )


//--------------------------------------------------------------
/**
 * @brief   ファクトリーで使用するポケモンのIndexを決める
 *			同じポケモン、装備道具にならないようにIndexを取得します
 *
 * @param   check_pokeno[]		被り防止でチェックするポケモン番号(Indexではない事に注意)
 * @param   check_itemno[]		被り防止でチェックするアイテム番号
 * @param   check_num			check_pokenoとcheck_itemnoのデータ数
 * @param   get_count			何体のポケモンIndexを取得するか
 * @param   ret_pokeindex[]		決まったポケモンIndexの結果代入先(get_count分の配列数がある事!)
 * @param   heap_id				ヒープID
 * @param   range				ポケモンIndex抽出範囲
 * @param   trade_count			交換回数(0以外を渡すとレンタルポケモン抽出になります)
 * @param   pow_rnd				パワー乱数(0-31)
 *
 * @retval	FALSE	抽選ループが50回以内で終わった
 * @retval	TRUE	抽選ループが50回以内で終わらなかった
 *
 * 被り防止を使用しない場合はcheck_numを0にしてください。(check_pokeno, check_itemnoはNULL)
 */
//--------------------------------------------------------------
BOOL Factory_PokemonIndexCreate(const u16 check_pokeno[], const u16 check_itemno[], 
	int check_num, int get_count, u16 get_pokeindex[], int heap_id, const FACTORY_POKE_RANGE *poke_range, u16 trade_count, u8 get_pow_rnd[] )
{
	u8 set_pow_rnd;
	int set_count, range_offset, set_index;
	B_TOWER_POKEMON_ROM_DATA prd_d[6];
	int i,range_up;
	const FACTORY_POKE_RANGE* range;
	
	//代入
	range = poke_range;

	GF_ASSERT(get_count <= 6);	//prd_dの配列数を超えていればASSERT
	
	range_offset = range->end - range->start;
	set_count = 0;
	range_up = 0;
	
	//交換回数が0以外が渡された時は、レンタルポケモン抽出と判断する！
	if( trade_count != 0 ){

		//1つ上のレンジから選出するポケモン数
		range_up = TRADE_COUNT_TBL_MAX;

		//交換回数と比較して、1つ上のレンジから選出するポケモン数を決定
		for( i=0; i < TRADE_COUNT_TBL_MAX; i++ ){
			if( trade_count <= trade_count_tbl[i] ){
				range_up = i;
				break;
			}
		}
	}

	while(set_count != get_count){
		//レンタル回数を考慮
		//
		//例  get_count = セットする数
		//    set_count = セットした数
		//    range_up  = 1つ上をセットする数
		//    range_upする数までは通常選出、その後は1つ上のランクから選出
		if( (set_count >= get_count - range_up) && (range->rankup_flag == 1) ){

			range_offset = (range+1)->end - (range+1)->start;		//抽出範囲1つ上
			set_index = (range+1)->end - (gf_rand() % (range_offset+1));
			set_pow_rnd = (range+1)->pow_rnd;

			//例  start       = 251
			//  　end         = 350 
			//    range_offset= 350 - 251 + 1 = 100
			//    rand        = 0_99
			//    set_index   = 350 - 99 = 251_350
			//    rankup_flag = 1 //ブレーンと通常TRの最強の時は、1つ上のデータを見ない！
			//                               //テーブルオーバーしないようにする
		}else{
			set_index = range->end - (gf_rand() % (range_offset+1));
			set_pow_rnd = range->pow_rnd;

			//例  start       = 151
			//  　end         = 250 
			//    range_offset= 250 - 151 + 1 = 100
			//    rand        = 0_99
			//    set_index   = 250 - 99 = 151_250
		}

		//バトルタワーポケモンロムデータの読み出し
		Frontier_PokemonRomDataGet(&prd_d[set_count], set_index, ARC_PL_BTD_PM);
		
		//今まで生成してきたポケモンと被っていないかチェック
		for(i = 0; i < set_count; i++){
			if(prd_d[i].mons_no == prd_d[set_count].mons_no 
					|| prd_d[i].item_no == prd_d[set_count].item_no){
				break;	//被った
			}
		}
		if(i != set_count){
			continue;
		}
		
		//チェック用に渡されているモンスターナンバー、装備アイテムの被りチェック
		for(i = 0; i < check_num; i++){
			if(prd_d[set_count].mons_no == check_pokeno[i] 
					|| prd_d[set_count].item_no ==  check_itemno[i]){
				break;	//被った
			}
		}
		if(i != check_num){
			continue;
		}

		//OS_Printf( "セットしたpoke index = %d\n", set_index );
		OS_Printf( "セットしたpow rnd = %d\n", set_pow_rnd );
		get_pokeindex[set_count] = set_index;
		get_pow_rnd[set_count] = set_pow_rnd;
		set_count++;
	}
	
	return FALSE;
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
u8 Factory_GetMinePokeNum( u8 type );
u8 Factory_GetEnemyPokeNum( u8 type, BOOL flag );
void Factory_RentalPokeMake( u16 lap, u8 level, u16* rental_poke_index, B_TOWER_POKEMON* rental_poke, u8* pow_rnd, u32* personal_rnd, u16 trade_count, u16* poke_check_tbl, u16* item_check_tbl );
void Factory_EnemyPokeMake( u8 set_num, u16 tr_index, u8 level, B_TOWER_POKEMON* rental_poke, u16* enemy_poke_index, B_TOWER_POKEMON* enemy_poke, u8* pow_rnd, u32* personal_rnd, int check_count );
BATTLE_PARAM* BtlFactory_CreateBattleParam( FACTORY_SCRWORK* wk, FRONTIER_EX_PARAM* ex_param );
static u32 Factory_GetFightType( u8 type );
u8 Factory_GetLevel( FACTORY_SCRWORK* wk );
void Factory_RentalPokeMake2( FACTORY_SCRWORK* wk );
void Factory_PairPokeMake( FACTORY_SCRWORK* wk );
BOOL Factory_CommCheck( u8 type );
static u16 Factory_GetTrAI( FACTORY_SCRWORK* wk );
u16 FactoryScr_CommGetLap( FACTORY_SCRWORK* wk );


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
u8 Factory_GetMinePokeNum( u8 type )
{
	switch( type ){

	case FACTORY_TYPE_SINGLE:
	case FACTORY_TYPE_DOUBLE:
		return 3;

	case FACTORY_TYPE_MULTI:
	case FACTORY_TYPE_WIFI_MULTI:
		return 2;
	};

	//エラー
	return 0;
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
u8 Factory_GetEnemyPokeNum( u8 type, BOOL flag )
{
	switch( type ){

	case FACTORY_TYPE_SINGLE:
	case FACTORY_TYPE_DOUBLE:
		return 3;

	case FACTORY_TYPE_MULTI:
	case FACTORY_TYPE_WIFI_MULTI:
		if( flag == FACTORY_FLAG_SOLO ){
			return 2;								//2
		}else{
			return 4;								//2+2=4
		}
	};

	//エラー
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	自分がレンタル出来るポケモン6匹を決める
 *
 * @param	lap
 * @param	level
 * @param	rental_poke_index
 * @param	rental_poke
 * @param   pow_rnd
 * @param	personal_rnd
 * @param	trade_count
 *
 * @return	
 *
 * 以下の順で処理していく
 * レンタル6
 * (被りチェック:レンタル6) 敵3
 * (被りチェック:手持ち3)(被りチェック:前の敵3) 敵3
 * ...
 */
//--------------------------------------------------------------
void Factory_RentalPokeMake( u16 lap, u8 level, u16* rental_poke_index, B_TOWER_POKEMON* rental_poke, u8 *pow_rnd, u32* personal_rnd, u16 trade_count, u16* poke_check_tbl, u16* item_check_tbl )
{
	const FACTORY_POKE_RANGE* poke_range;
	int i;
	
	//OS_Printf( "lap = %d\n", lap );
	poke_range = Factory_MinePokemonRangeGet( lap, level );

#if 0
	//パワー乱数代入
	for( i=0; i < FACTORY_RENTAL_POKE_MAX; i++ ){
		pow_rnd[i] = poke_range->pow_rnd;
	}
#endif
	
	//通信なし、被りチェックの必要はないのでNULLを渡す
	//通信あり、親機のレンタルで抽出されたポケモンを除外した6匹を子機用に選ぶ

	//被りポケ、アイテム、データ数、取得する数、代入先
	if( poke_check_tbl == NULL ){
		Factory_PokemonIndexCreate( NULL, NULL, 0,
									FACTORY_RENTAL_POKE_MAX, rental_poke_index, 
									HEAPID_WORLD, poke_range, trade_count, pow_rnd );//被りchk無し
	}else{

		OS_Printf( "poke_check_tbl[0] = %d\n", poke_check_tbl[0] );
		OS_Printf( "poke_check_tbl[1] = %d\n", poke_check_tbl[1] );
		OS_Printf( "item_check_tbl[0] = %d\n", item_check_tbl[0] );
		OS_Printf( "item_check_tbl[1] = %d\n", item_check_tbl[1] );

		Factory_PokemonIndexCreate( poke_check_tbl, poke_check_tbl, FACTORY_RENTAL_POKE_MAX,
									FACTORY_RENTAL_POKE_MAX, rental_poke_index, 
									HEAPID_WORLD, poke_range, trade_count, pow_rnd );//被りchk有り
	}

	//0が入るとおかしくなるかのチェック
	//for( i=0; i < 6 ;i++ ){
	//	rental_poke_index[i] = 0;
	//}

	//レンタルポケモンを一度に生成(personalなし)
	Frontier_PokemonParamCreateAll(	rental_poke, rental_poke_index, pow_rnd,
									NULL, personal_rnd, FACTORY_RENTAL_POKE_MAX, HEAPID_WORLD,
									ARC_PL_BTD_PM );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	敵が出す最初のポケモン3(4)匹を決める
 *
 * @param	set_num
 * @param	tr_index
 * @param	level
 * @param	enemy_poke_index
 * @param	rental_poke
 * @param	enemy_poke
 * @param	personal_rnd
 * @param	check_count			被りチェックする数
 *
 * @return	
 *
 * 以下の順で処理していく
 * レンタル6
 * (被りチェック:レンタル6) 敵3
 * (被りチェック:手持ち3)(被りチェック:前の敵3) 敵3
 * ...
 */
//--------------------------------------------------------------
void Factory_EnemyPokeMake( u8 set_num, u16 tr_index, u8 level, B_TOWER_POKEMON* rental_poke, u16* enemy_poke_index, B_TOWER_POKEMON* enemy_poke, u8* pow_rnd, u32* personal_rnd, int check_count )
{
	int i;
	B_TOWER_POKEMON poke;
	const FACTORY_POKE_RANGE* poke_range;
	u16 poke_check_tbl[FACTORY_RENTAL_POKE_MAX*2];
	u16 item_check_tbl[FACTORY_RENTAL_POKE_MAX*2];

	poke_range = Factory_EnemyPokemonRangeGet( tr_index, level );

#if 0
	//パワー乱数代入
	for( i=0; i < set_num; i++ ){
		pow_rnd[i] = poke_range->pow_rnd;
	}
#endif
	
	//チェックテーブル作成
	//for( i=0; i < FACTORY_RENTAL_POKE_MAX ;i++ ){
	for( i=0; i < check_count ;i++ ){
		poke = *(B_TOWER_POKEMON*)(&rental_poke[i]);
		poke_check_tbl[i] = poke.mons_no;
		item_check_tbl[i] = poke.item_no;
	}

	//敵ポケモンのインデックス取得(被りポケ、アイテム、データ数、取得する数、代入先)
	//Factory_PokemonIndexCreate( poke_check_tbl, item_check_tbl, FACTORY_RENTAL_POKE_MAX,
	Factory_PokemonIndexCreate( poke_check_tbl, item_check_tbl, check_count,
								set_num, enemy_poke_index, 
								HEAPID_WORLD, poke_range, 0, pow_rnd );

	//敵ポケモンを一度に生成(personalなし)
	Frontier_PokemonParamCreateAll(	enemy_poke, enemy_poke_index, 
									pow_rnd, NULL, personal_rnd, set_num, HEAPID_WORLD,
									ARC_PL_BTD_PM );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	バトルファクトリー用　バトルパラメータ生成
 */
//--------------------------------------------------------------
BATTLE_PARAM* BtlFactory_CreateBattleParam( FACTORY_SCRWORK* wk, FRONTIER_EX_PARAM* ex_param )
{
	int i;
	u32	val32;
	u8 m_max,e_max;
	BATTLE_PARAM* bp;
	POKEMON_PARAM* pp;
	B_TOWER_TRAINER bt_trd;
	B_TOWER_TRAINER_ROM_DATA* p_rom_tr;
	MSGDATA_MANAGER* man;
	STRBUF* msg;
	MYSTATUS* my;

	//参加ポケモン数を取得
	m_max = Factory_GetMinePokeNum( wk->type );
	e_max = Factory_GetEnemyPokeNum( wk->type, FACTORY_FLAG_SOLO );

	//回復処理
	PokeParty_RecoverAll( wk->p_m_party );
	PokeParty_RecoverAll( wk->p_e_party );

	//戦闘パラメータの生成処理(field_battle.c)(fssc_factory.c FSSC_FactoryCallGetResultでDelete)
	bp = BattleParam_Create( HEAPID_WORLD, Factory_GetFightType(wk->type) );
	
	//戦闘パラメータの収集コア(フィールド)
	BattleParam_SetParamByGameDataCore( bp, NULL, 
										ex_param->savedata,
										ex_param->zone_id,
										ex_param->fnote_data,
										ex_param->bag_cursor,
										ex_param->battle_cursor);

	//現状は固定！
	bp->bg_id		= BG_ID_FACTORY;		//基本背景指定
	bp->ground_id	= GROUND_ID_FACTORY;	//基本地面指定

	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//MINE：選んだ手持ちポケモンをセット
	PokeParty_Init( bp->poke_party[POKEPARTY_MINE], m_max );

	//MINE：自分のポケモンデータセット
	pp = PokemonParam_AllocWork( HEAPID_WORLD );
	for( i=0; i < m_max ;i++ ){

		//POKEMON_PARAM構造体からPOKEMON_PARAM構造体へコピー
		PokeCopyPPtoPP( PokeParty_GetMemberPointer(wk->p_m_party,i), pp );

		//戦闘パラメータセット:ポケモンを加える
		BattleParam_AddPokemon( bp, pp, POKEPARTY_MINE );
		OS_Printf( "MINE1 power_rnd = %d\n", PokeParaGet(pp,ID_PARA_power_rnd,NULL) );
		OS_Printf( "MINE1 power_rnd = %d\n", PokeParaGet(pp,ID_PARA_power_rnd,NULL) );
		OS_Printf( "MINE1 pow_rnd = %d\n", PokeParaGet(pp,ID_PARA_pow_rnd,NULL) );
		OS_Printf( "MINE1 def_rnd = %d\n", PokeParaGet(pp,ID_PARA_def_rnd,NULL) );
		OS_Printf( "MINE1 spedef_rnd = %d\n", PokeParaGet(pp,ID_PARA_spedef_rnd,NULL) );
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
					Factory_GetEnemyPokeNum(wk->type,FACTORY_FLAG_SOLO) );

	//AIセット
	for( i=0; i < CLIENT_MAX ;i++ ){
		bp->trainer_data[i].aibit = Factory_GetTrAI( wk );
	}
	OS_Printf( "トレーナーのAI = %d\n", Factory_GetTrAI(wk) );

	//ENEMY1：戦闘パラメータセット:ポケモンを加える
	pp = PokemonParam_AllocWork( HEAPID_WORLD );
	for( i=0; i < e_max ;i++ ){

		//POKEMON_PARAM構造体からPOKEMON_PARAM構造体へコピー
		PokeCopyPPtoPP( PokeParty_GetMemberPointer(wk->p_e_party,i), pp );
		BattleParam_AddPokemon( bp, pp, POKEPARTY_ENEMY );
		OS_Printf( "ENEMY1 power_rnd = %d\n", PokeParaGet(pp,ID_PARA_power_rnd,NULL) );
		OS_Printf( "ENEMY1 pow_rnd = %d\n", PokeParaGet(pp,ID_PARA_pow_rnd,NULL) );
		OS_Printf( "ENEMY1 def_rnd = %d\n", PokeParaGet(pp,ID_PARA_def_rnd,NULL) );
		OS_Printf( "ENEMY1 spedef_rnd = %d\n", PokeParaGet(pp,ID_PARA_spedef_rnd,NULL) );
	}
	sys_FreeMemoryEz( pp );

	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//デバック情報表示
//OS_Printf("p_party count = %d\n", PokeParty_GetPokeCount(wk->p_party) );
//OS_Printf("bp_party[mine]count= %d\n",PokeParty_GetPokeCount(bp->poke_party[POKEPARTY_MINE]));
//OS_Printf("bp_party[enemy]count= %d\n",PokeParty_GetPokeCount(bp->poke_party[POKEPARTY_ENEMY]));

	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	switch( wk->type ){

	case FACTORY_TYPE_MULTI:
	case FACTORY_TYPE_WIFI_MULTI:

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
		p_rom_tr = Frontier_TrainerDataGet( &bt_trd, wk->tr_index[wk->round+FACTORY_LAP_ENEMY_MAX],
											HEAPID_WORLD, ARC_PL_BTD_TR );
		sys_FreeMemoryEz( p_rom_tr );

		//ENEMY2：トレーナーデータをセット
		BattleParamTrainerDataSet( bp, &bt_trd, e_max, CLIENT_NO_ENEMY2, HEAPID_WORLD );

		//ENEMY2：選んだ手持ちポケモンをセット
		PokeParty_Init( bp->poke_party[POKEPARTY_ENEMY_PAIR], 
						Factory_GetEnemyPokeNum(wk->type,FACTORY_FLAG_SOLO) );

		//ENEMY2：戦闘パラメータセット:ポケモンを加える
		pp = PokemonParam_AllocWork( HEAPID_WORLD );
		for( i=0; i < e_max ;i++ ){

			//POKEMON_PARAM構造体からPOKEMON_PARAM構造体へコピー
			PokeCopyPPtoPP( PokeParty_GetMemberPointer(wk->p_e_party,(e_max+i)), pp );
			BattleParam_AddPokemon( bp, pp, POKEPARTY_ENEMY_PAIR );
			OS_Printf( "ENEMY2 power_rnd = %d\n", PokeParaGet(pp,ID_PARA_power_rnd,NULL) );
			OS_Printf( "ENEMY2 pow_rnd = %d\n", PokeParaGet(pp,ID_PARA_pow_rnd,NULL) );
			OS_Printf( "ENEMY2 def_rnd = %d\n", PokeParaGet(pp,ID_PARA_def_rnd,NULL) );
			OS_Printf( "ENEMY2 spedef_rnd = %d\n", PokeParaGet(pp,ID_PARA_spedef_rnd,NULL) );
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
static u32 Factory_GetFightType( u8 type )
{
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//
	//ファクトリー用のFIGHT_TYPEを作成するか確認する！
	//
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

	switch( type ){

	case FACTORY_TYPE_SINGLE:
		return FIGHT_TYPE_BATTLE_TOWER_1vs1;

	case FACTORY_TYPE_DOUBLE:
		return FIGHT_TYPE_BATTLE_TOWER_2vs2;

	case FACTORY_TYPE_MULTI:
		return FIGHT_TYPE_BATTLE_TOWER_SIO_MULTI;	
		//return FIGHT_TYPE_MULTI_SIO;	

	case FACTORY_TYPE_WIFI_MULTI:
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
u8 Factory_GetLevel( FACTORY_SCRWORK* wk )
{
	if( wk->level == FACTORY_LEVEL_50 ){
		return 50;
	}

	return 100;
}

//--------------------------------------------------------------
/**
 * @brief	レンタル準備データ受信後、レンタルポケモンを一度に生成
 *
 * @param	wk
 *
 * @return	
 */
//--------------------------------------------------------------
void Factory_RentalPokeMake2( FACTORY_SCRWORK* wk )
{
	int i;
	POKEMON_PARAM* temp_poke;

	//レンタルポケモンを一度に生成(personalあり)
	Frontier_PokemonParamCreateAll(	wk->rental_poke, wk->rental_poke_index, wk->rental_pow_rnd,
									wk->rental_personal_rnd, NULL,
									FACTORY_RENTAL_POKE_MAX, HEAPID_WORLD, 
									ARC_PL_BTD_PM );

	PokeParty_InitWork( wk->p_m_party );		//POKEPARTYを初期化

	temp_poke = PokemonParam_AllocWork( HEAPID_WORLD );
	for( i=0; i < FACTORY_RENTAL_POKE_MAX ;i++ ){			//レンタル6匹
		Frontier_PokeParaMake( &wk->rental_poke[i], temp_poke, Factory_GetLevel(wk) );
		Frontier_PokePartyAdd( wk->sv, wk->p_m_party, temp_poke );
	}
	sys_FreeMemoryEz( temp_poke );

	return;
}

//--------------------------------------------------------------
/**
 * @brief	手持ちポケモンデータ受信後、パートナーの手持ちポケモンを一度に生成
 *
 * @param	wk
 *
 * @return	
 */
//--------------------------------------------------------------
void Factory_PairPokeMake( FACTORY_SCRWORK* wk )
{
	int i,poke_count;
	POKEMON_PARAM* temp_poke;

	//[2,3]	(相手がレンタルで選んだ2つ)
	B_TOWER_POKEMON bt_poke[FACTORY_COMM_POKE_NUM];

	//ポケモン数取得
	poke_count = PokeParty_GetPokeCount( wk->p_m_party );

	//[2,3]がすでに使われていたら削除
	for( i=poke_count; i > FACTORY_COMM_POKE_NUM ;i-- ){ 
		PokeParty_Delete( wk->p_m_party, (i-1) );			//pos=0オリジン
	}

	//パートナーの手持ちポケモンを一度に生成(personalあり)
	Frontier_PokemonParamCreateAll(	bt_poke, wk->pair_rental_poke_index, 
									wk->pair_rental_pow_rnd,
									wk->pair_rental_personal_rnd, NULL,
									FACTORY_COMM_POKE_NUM, HEAPID_WORLD, 
									ARC_PL_BTD_PM );

	temp_poke = PokemonParam_AllocWork( HEAPID_WORLD );
	for( i=0; i < FACTORY_COMM_POKE_NUM ;i++ ){

		//バトルタワー用ポケモンデータからPOKEMON_PARAMを生成
		Frontier_PokeParaMake( &bt_poke[i], temp_poke, Factory_GetLevel(wk) );
		Frontier_PokePartyAdd( wk->sv, wk->p_m_party, temp_poke );

		//手持ちにポケモンのインデックス格納
		wk->temoti_poke_index[i+FACTORY_COMM_POKE_NUM] = wk->pair_rental_poke_index[i];
	}
	sys_FreeMemoryEz( temp_poke );

	OS_Printf( "ポケモンパーティ数 = %d\n",	PokeParty_GetPokeCount(wk->p_m_party) );
	return;
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
BOOL Factory_CommCheck( u8 type )
{
	switch( type ){
	case FACTORY_TYPE_MULTI:
	case FACTORY_TYPE_WIFI_MULTI:
		return TRUE;
	};

	return FALSE;

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
static u16 Factory_GetTrAI( FACTORY_SCRWORK* wk )
{
	u16 ai,lap;

#if 1
	//ブレーン
	if( wk->type == FACTORY_TYPE_SINGLE ){
		if( (wk->tr_index[wk->round] == FACTORY_LEADER_TR_INDEX_1ST) ||
			(wk->tr_index[wk->round] == FACTORY_LEADER_TR_INDEX_2ND) ){
			return FR_AI_EXPERT;
		}
	}
#endif

	//周回数を取得
	lap = FactoryScr_CommGetLap( wk );

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
 * @param	wk		FACTORY_SCRWORK型のポインタ
 *
 * @return	"周回数"
 */
//--------------------------------------------------------------
u16 FactoryScr_CommGetLap( FACTORY_SCRWORK* wk )
{
	u16	lap;
	
	lap = wk->lap;

	if( Factory_CommCheck(wk->type) == TRUE ){

		//通信時には周回数の多いほうで抽選
		if( wk->pair_lap > wk->lap ){
			lap = wk->pair_lap;
		}
	}

	return lap;
}


