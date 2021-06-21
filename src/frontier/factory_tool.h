//==============================================================================
/**
 * @file	factory_tool.h
 * @brief	バトルファクトリー関連ツール類
 * @author	Satoshi Nohara
 * @date	2007.03.27
 *
 * 松田さんから引継ぎ
 */
//==============================================================================
#ifndef __FACTORY_TOOL_H__
#define __FACTORY_TOOL_H__

#include "common.h"
#include "system/pms_data.h"
#include "savedata/b_tower.h"


//==============================================================================
//	定数定義
//==============================================================================


//==============================================================================
//	構造体定義
//==============================================================================
///レンタルポケモンのデータからの抽出範囲
typedef struct{
	u16 trainer_index;	///<トレーナーIndex(このIndex未満のトレーナーが対象)
	u16 start;			///<ポケモンIndex開始No
	u16 end;			///<ポケモンIndex終了No
	u8 pow_rnd;			///<パワー乱数
	u8 rankup_flag;		///<ランクアップ可能か
}FACTORY_POKE_RANGE;


//==============================================================================
//	外部関数宣言
//==============================================================================
extern void Factory_EnemyLapAllTrainerIndexGet( u8 type, int lap, u16 trainer_index[], u8 num );
extern u32 Factory_PokemonParamCreate(B_TOWER_POKEMON *pwd,
	u16 poke_index, int position, u8 pow_rnd, u32 personal_rnd, int heap_id);
extern void Factory_PokemonParamCreateAll(B_TOWER_POKEMON *pwd, u16 poke_index[], 
	u8 pow_rnd[], u32 personal_rnd[], u32 ret_personal_rnd[], int set_num, int heap_id);
extern const FACTORY_POKE_RANGE *Factory_EnemyPokemonRangeGet(int trainer_index, 
	int factory_level);
extern BOOL Factory_PokemonIndexCreate(const u16 check_pokeno[], const u16 check_itemno[], 
	int check_num, int get_count, u16 get_pokeindex[], int heap_id, const FACTORY_POKE_RANGE *range, u16 trade_count, u8 get_pow_rnd[] );
//extern const FACTORY_POKE_RANGE *Factory_MinePokemonRangeGet(int lap, int factory_level);


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
//	04.07.11追加
//
//==============================================================================
#include "battle/battle_common.h"
#include "poketool/pokeparty.h"
#include "poketool/poke_tool.h"
#include "savedata/factory_savedata.h"		//FACTORYDATA
#include "factory_def.h"					//FACTORY_LAP_ENEMY_MAX
#include "frontier_types.h"
#include "frontier_main.h"


//============================================================================================
//
//	バトルファクトリー　スクリプトワーク
//
//============================================================================================
typedef struct{

	int	heapID;											//ヒープ

	u8	type;											//シングル、ダブル、マルチ、wifiマルチ
	u8	level;											//LV50、オープン
	u8	round;											//今何人目？
	u8	mode;											//レンタル、交換

	u16	trade_count;									//交換回数
	u8	clear_flag;										//7連勝(クリア)したかフラグ(0か1)
	u8	brain_appear_msg_flag;							//ブレーン登場メッセージ表示したか

	u16	rensyou;										//現在の連勝数
	u16	lap;											//現在の周回数

	u32	win_cnt;										//現在までの総勝利数
	int winlose_flag;									//勝ち負けフラグ

	//敵トレーナーデータ
	u16 tr_index[FACTORY_LAP_MULTI_ENEMY_MAX];			//トレーナーindexテーブル(*2=ダブル)

	//対戦相手データ格納場所
	B_TOWER_PARTNER_DATA tr_data[2];

	//レンタルポケモンデータ
	u16 rental_poke_index[FACTORY_RENTAL_POKE_MAX];		//ポケモンindexテーブル
	u8	rental_pow_rnd[FACTORY_RENTAL_POKE_MAX];		//ポケモンのパワー乱数
	u32 rental_personal_rnd[FACTORY_RENTAL_POKE_MAX];	//ポケモンの個性乱数
	B_TOWER_POKEMON rental_poke[FACTORY_RENTAL_POKE_MAX];
	u16 dummy_rental_eom;								//暫定処理(EOMが2byte余計に書き込まれるため)

	//敵ポケモンデータ
	u16 enemy_poke_index[FACTORY_ENEMY_POKE_MAX];		//ポケモンindexテーブル
	u8	enemy_pow_rnd[FACTORY_ENEMY_POKE_MAX];			//ポケモンのパワー乱数
	u32 enemy_personal_rnd[FACTORY_ENEMY_POKE_MAX];		//ポケモンの個性乱数
	B_TOWER_POKEMON enemy_poke[FACTORY_ENEMY_POKE_MAX];	//ポケモンデータ
	u16 dummy_enemy_eom;								//暫定処理(EOMが2byte余計に書き込まれるため)

	//ファクトリー専用画面へ渡す用のポケモンデータ
	POKEPARTY* p_m_party;								//味方
	POKEPARTY* p_e_party;								//敵
	u16 ret_work[FACTORY_RET_WORK_MAX];
	u16 temoti_poke_index[FACTORY_PARTY_POKE_MAX];		//手持ちにもっているポケモンindexテーブル

	FACTORYDATA* factory_savedata;						//ファクトリーセーブデータへのポインタ
	SAVEDATA* sv;
	void* p_work;

	TCB_PTR	rail_move_tcb;								//マップのレール動作TCB
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//通信用：データバッファ
	u16	buf[FACTORY_COMM_BUF_LEN];

	//通信用：基本情報
	u8	pair_retire_flag;									//リタイヤを選んだか=1
	u8	pair_trade_yesno_flag;								//交換したいか(0=したい、1=したくない)
	u16	pair_lap;											//周回数

	u16	pair_trade_count;									//交換回数
	u16	pair_rensyou;										//現在の連勝数(27人目とかの表示に必要)

	//通信用：レンタルポケモンデータ
	u16 pair_rental_poke_index[FACTORY_RENTAL_POKE_MAX];	//ポケモンindex_tbl
	u8	pair_rental_pow_rnd[FACTORY_RENTAL_POKE_MAX];		//ポケモンのパワー乱数
	u32 pair_rental_personal_rnd[FACTORY_RENTAL_POKE_MAX];	//ポケモンの個性乱数
	B_TOWER_POKEMON pair_rental_poke[FACTORY_RENTAL_POKE_MAX];
	u16 dummy_pair_rental_eom;							//暫定処理(EOMが2byte余計に書き込まれるため)
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	
	//通信の送受信バッファ
	u8 recieve_count;										//受信データを受け取った人数をカウント

	u32 dummy_work;
}FACTORY_SCRWORK;


//============================================================================================
//
//	extern宣言
//
//============================================================================================
extern u8 Factory_GetMinePokeNum( u8 type );
extern u8 Factory_GetEnemyPokeNum( u8 type, BOOL flag );
extern void Factory_RentalPokeMake( u16 lap, u8 level, u16* rental_poke_index, B_TOWER_POKEMON* rental_poke, u8* pow_rnd, u32* personal_rnd, u16 trade_count, u16* poke_check_tbl, u16* item_check_tbl );
extern void Factory_EnemyPokeMake( u8 set_num, u16 tr_index, u8 level, B_TOWER_POKEMON* rental_poke, u16* enemy_poke_index, B_TOWER_POKEMON* enemy_poke, u8* pow_rnd, u32* personal_rnd, int check_count );
extern BATTLE_PARAM* BtlFactory_CreateBattleParam(FACTORY_SCRWORK* wk, FRONTIER_EX_PARAM* ex_param);
extern u8 Factory_GetLevel( FACTORY_SCRWORK* wk );
extern void Factory_RentalPokeMake2( FACTORY_SCRWORK* wk );
extern void Factory_PairPokeMake( FACTORY_SCRWORK* wk );
extern BOOL Factory_CommCheck( u8 type );
extern u16 FactoryScr_CommGetLap( FACTORY_SCRWORK* wk );


#endif	//__FACTORY_TOOL_H__


