//==============================================================================
/**
 * @file	roulette_tool.h
 * @brief	バトルルーレット関連ツール類
 * @author	nohara
 * @date	2007.09.06
 */
//==============================================================================
#ifndef __ROULETTE_TOOL_H__
#define __ROULETTE_TOOL_H__

#include "common.h"
#include "frontier_tool.h"


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
	
	u8 dummy;	///<4バイト境界オフセット
}ROULETTE_POKE_RANGE;


//==============================================================================
//	外部関数宣言
//==============================================================================
extern void Roulette_EnemyLapAllTrainerIndexGet( u8 type, int lap, u16 trainer_index[], u8 num );
extern u32 Roulette_PokemonParamCreate(B_TOWER_POKEMON *pwd,
	u16 poke_index, int position, u8 pow_rnd, u32 personal_rnd, int heap_id);
extern void Roulette_PokemonParamCreateAll(B_TOWER_POKEMON *pwd, u16 poke_index[], 
	u8 pow_rnd[], u32 personal_rnd[], u32 ret_personal_rnd[], int set_num, int heap_id);
extern const ROULETTE_POKE_RANGE *Roulette_EnemyPokemonRangeGet(int trainer_index, 
	int roulette_level);


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
#include "savedata/roulette_savedata.h"		//ROULETTEDATA
#include "roulette_def.h"					//ROULETTE_LAP_ENEMY_MAX
#include "frontier_types.h"
#include "frontier_main.h"


//============================================================================================
//
//	バトルルーレット　スクリプトワーク
//
//============================================================================================
typedef struct{

	int	heapID;											//ヒープ
	SAVEDATA* sv;
	ROULETTEDATA* roulette_savedata;					//ルーレットセーブデータへのポインタ
	void* p_work;

	u8	type;											//シングル、ダブル、マルチ、wifiマルチ
	u8	round;											//今何人目？
	u8	random_flag;									//カーソルランダム移動onフラグ
	u8	decide_ev_no;									//決定したイベントナンバー

	int	weather;										//天候

	u16	rensyou;										//現在の連勝数
	u16	lap;											//現在の周回数

	u8	csr_speed_level;								//カーソルスピード
	u8	point;											//戦闘結果ポイント
	u8	brain_appear_msg_flag;							//ブレーン登場メッセージ表示したか
	u8	poke_safe_flag;									//誰もイベントの効果を発揮しなかった

	u16	rand_pos;										//共通のランダム
	u16 dummy77;

	u32	win_cnt;										//現在までの総勝利数
	int winlose_flag;									//勝ち負けフラグ

	//参加している手持ちポケモンの位置情報
	u8	mine_poke_pos[ROULETTE_ENTRY_POKE_MAX];
	u8	clear_flag;										//7連勝(クリア)したかフラグ(0か1)

	//アイテムアイコン
	CATS_ACT_PTR m_pokeicon[ROULETTE_MINE_POKE_MAX];
	CATS_ACT_PTR e_pokeicon[ROULETTE_ENEMY_POKE_MAX];

	//アイテム持っているかアイコン
	CATS_ACT_PTR m_itemkeep[ROULETTE_MINE_POKE_MAX];
	CATS_ACT_PTR e_itemkeep[ROULETTE_ENEMY_POKE_MAX];

	//ルーレット専用画面へ渡す用のポケモンデータ
	POKEPARTY* p_m_party;								//味方
	POKEPARTY* p_e_party;								//敵

	//敵トレーナーデータ
	u16 tr_index[ROULETTE_LAP_MULTI_ENEMY_MAX];			//トレーナーindexテーブル(*2=ダブル)

	u32	temp_hp[ROULETTE_MINE_POKE_MAX];
	u32	temp_pow[ROULETTE_MINE_POKE_MAX];
	u32	temp_def[ROULETTE_MINE_POKE_MAX];
	u32	temp_agi[ROULETTE_MINE_POKE_MAX];
	u32	temp_spepow[ROULETTE_MINE_POKE_MAX];
	u32	temp_spedef[ROULETTE_MINE_POKE_MAX];

	//対戦相手データ格納場所
	B_TOWER_PARTNER_DATA tr_data[2];

	//敵ポケモンデータ
	u16 enemy_poke_index[ROULETTE_ENEMY_POKE_MAX];		//ポケモンindexテーブル
	u8	enemy_pow_rnd[ROULETTE_ENEMY_POKE_MAX];			//ポケモンのパワー乱数
	u32 enemy_personal_rnd[ROULETTE_ENEMY_POKE_MAX];	//ポケモンの個性乱数
	B_TOWER_POKEMON enemy_poke[ROULETTE_ENEMY_POKE_MAX];//ポケモンデータ
	u16 dummy_enemy_eom;								//暫定処理(EOMが2byte余計に書き込まれるため)

	u16 itemno[ROULETTE_ENTRY_POKE_MAX];				//手持ちポケモンの所持アイテム保存用

	u16 ret_work[ROULETTE_RET_WORK_MAX];

	//アイテム持っているアイコンを表示するフラグ
	//u16 itemkeep_draw_flag[ROULETTE_ENTRY_POKE_MAX];

	//手持ちポケモン
	//u16 item[ROULETTE_MINE_POKE_MAX];					//道具

	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//通信用：データバッファ
	u16	send_buf[ROULETTE_COMM_BUF_LEN];
	u8	huge_buf[ROULETTE_HUGE_BUF_LEN];
	u8	recieve_huge_buf[ROULETTE_COMM_PLAYER_NUM][ROULETTE_HUGE_BUF_LEN];

	//通信用：基本情報
	u8	pair_retire_flag;								//リタイヤを選んだか=1
	u8	pair_trade_yesno_flag;							//交換したいか(0=したい、1=したくない)
	u16	pair_lap;										//周回数
	u16	pair_rensyou;									//現在の連勝数(27人目とかの表示に必要)
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

	u8	reverce_flag;									//パーティクル反転フラグ
	u8	pair_sel_type;
	u8	recieve_count;
	u8	dummy22;

	u16* list_del_work;

	u32	dummy_work;
}ROULETTE_SCRWORK;


//============================================================================================
//
//	extern宣言
//
//============================================================================================
extern u8 Roulette_GetMinePokeNum( u8 type, BOOL flag );
extern u8 Roulette_GetEnemyPokeNum( u8 type, BOOL flag );
extern void Roulette_EnemyPokeMake( u8 set_num, u16 tr_index, u8 level, B_TOWER_POKEMON* rental_poke, u16* enemy_poke_index, B_TOWER_POKEMON* enemy_poke, u8* pow_rnd, u32* personal_rnd );
extern BATTLE_PARAM* BtlRoulette_CreateBattleParam(ROULETTE_SCRWORK* wk, FRONTIER_EX_PARAM* ex_param);
extern u8 Roulette_GetLevel( ROULETTE_SCRWORK* wk );
extern BOOL Roulette_CommCheck( u8 type );
extern void Roulette_EnemyPartySet( ROULETTE_SCRWORK* wk );
extern void Roulette_PokePartyRecoverAll( POKEPARTY* party );
extern void Roulette_BirthInfoSet( ROULETTE_SCRWORK* wk, POKEMON_PARAM* poke );
extern void Roulette_PokePartyAdd( ROULETTE_SCRWORK* wk, POKEPARTY* party, POKEMON_PARAM* poke );
extern u16 Roulette_GetPanelBP( ROULETTE_SCRWORK* wk, u8 panel_no );
extern u16 RouletteScr_CommGetLap( ROULETTE_SCRWORK* wk );
extern u8 Roulette_GetEvPanelColor( u8 panel_no );


#endif	//__ROULETTE_TOOL_H__


