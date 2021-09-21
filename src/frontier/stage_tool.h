//==============================================================================
/**
 * @file	stage_tool.h
 * @brief	バトルステージ関連ツール類
 * @author	nohara
 * @date	07.06.07
 */
//==============================================================================
#ifndef __STAGE_TOOL_H__
#define __STAGE_TOOL_H__

#include "common.h"
#include "system/pms_data.h"
#include "savedata/b_tower.h"

#include "battle/battle_common.h"
#include "poketool/pokeparty.h"
#include "poketool/poke_tool.h"
#include "savedata/stage_savedata.h"		//STAGEDATA
#include "stage_def.h"
#include "frontier_types.h"
#include "frontier_main.h"


//============================================================================================
//
//	バトルステージ　スクリプトワーク
//
//============================================================================================
typedef struct{

	int	heapID;											//ヒープ

	u8	type;											//シングル、ダブル、マルチ、wifiマルチ
	u8	round;											//今何人目？
	u8	clear_flag;										//7連勝(クリア)したかフラグ(0か1)
	u8	enemy_level;									//敵ポケモンのレベル

	u16	rensyou;										//現在の連勝数
	u16	lap;											//現在の周回数

	fx32 sqrt;											//平方根
	u32	win_cnt;										//現在までの総勝利数
	int winlose_flag;									//勝ち負けフラグ

	//敵トレーナーデータ
	u16 tr_index[STAGE_LAP_MULTI_ENEMY_MAX];			//トレーナーindexテーブル[18*2]

	//対戦相手データ格納場所
	B_TOWER_PARTNER_DATA tr_data[2];

	//参加している手持ちポケモンの位置情報
	u8 mine_poke_pos[STAGE_ENTRY_POKE_MAX];
	POKEPARTY* p_party;

	//敵ポケモンデータ
	u16 enemy_poke_index[STAGE_LAP_MULTI_ENEMY_MAX];	//ポケモンindexテーブル
	B_TOWER_POKEMON enemy_poke[STAGE_LAP_MULTI_ENEMY_MAX];	//ポケモンデータ
	u16 dummy_enemy_eom;								//暫定処理(EOMが2byte余計に書き込まれるため)

	//ポケモンのタイプ選択画面へ渡す用のポケモンデータ
	u16 ret_work;

	u8 poke_type;										//挑戦するポケモンのタイプ
	u8 csr_pos;											//選択した位置

	STAGEDATA* stage_savedata;							//ステージセーブデータへのポインタ
	SAVEDATA* sv;
	void* p_work;

	//タイプごとのレベル(ランク)
	u8 type_level[STAGE_TYPE_MAX][STAGE_TR_TYPE_MAX/2];	//[4][18/2]	4bit

	u16 itemno[STAGE_ENTRY_POKE_MAX];					//所持アイテム

	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//通信用：データバッファ
	u16	buf[STAGE_COMM_BUF_LEN];
	u8	huge_buf[STAGE_HUGE_BUF_LEN];
	u8	recieve_huge_buf[STAGE_COMM_PLAYER_NUM][STAGE_HUGE_BUF_LEN];

	u16 pair_poke_level[STAGE_ENTRY_POKE_MAX];				//相手のポケモンのレベル

	//通信用：基本情報
	u8	pair_retire_flag;									//リタイヤを選んだか=1

	POKEMON_PARAM* pair_poke;

	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

	//通信の送受信バッファ
	u8 recieve_count;										//受信データを受け取った人数をカウント

	u32 dummy_work;
}STAGE_SCRWORK;


//============================================================================================
//
//	extern宣言
//
//============================================================================================
extern void Stage_CreateTrType( u8 csr_pos, u8 set_num, u8 rank, u8 round, u16 tr_index[] );
extern void Stage_CreateTrIndex( u8 type, u8 set_num, int lap, u8 rank, u8 round, u16 tr_index[] );
extern void Stage_CreatePokeIndex( u8 set_num, u8 poketype, u8 rank, u8 round, u16 monsno, u16 poke_index[], u8 brain_count );
extern u8 Stage_GetMinePokeNum( u8 type );
extern u8 Stage_GetEnemyPokeNum( u8 type );
extern BATTLE_PARAM* BtlStage_CreateBattleParam( STAGE_SCRWORK* wk, FRONTIER_EX_PARAM* ex_param );
extern u32 Stage_PokemonParamCreate( B_TOWER_POKEMON *pwd, u16 poke_index, int position, u8 pow_rnd, u32 personal_rnd, int heap_id );
//extern void Stage_PokemonParamCreateAll( B_TOWER_POKEMON *pwd, u8 type_level, u16 poke_index[], int set_num, int heap_id, int arcPM );
extern u8 Stage_GetPokeType( u8 csr_pos );
extern BOOL Stage_CommCheck( u8 type );
extern u8 Stage_GetMineLevelRank( STAGE_SCRWORK* wk );
extern u16 StageScr_CommGetPokeLevel( STAGE_SCRWORK* wk );
extern fx32 StageScr_GetSqrt32( u32 param );


#endif	//__STAGE_TOOL_H__


