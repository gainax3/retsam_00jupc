//==============================================================================
/**
 * @file	castle_tool.h
 * @brief	バトルキャッスル関連ツール類
 * @author	nohara
 * @date	2007.07.04
 */
//==============================================================================
#ifndef __CASTLE_TOOL_H__
#define __CASTLE_TOOL_H__

#include "common.h"
#include "system/pms_data.h"
#include "savedata/b_tower.h"
#include "savedata/frontier_savedata.h"


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
}CASTLE_POKE_RANGE;


//==============================================================================
//	外部関数宣言
//==============================================================================
extern void Castle_EnemyLapAllTrainerIndexGet( u8 type, int lap, u16 trainer_index[], u8 num );
extern u32 Castle_PokemonParamCreate(B_TOWER_POKEMON *pwd,
	u16 poke_index, int position, u8 pow_rnd, u32 personal_rnd, int heap_id);
extern void Castle_PokemonParamCreateAll(B_TOWER_POKEMON *pwd, u16 poke_index[], 
	u8 pow_rnd[], u32 personal_rnd[], u32 ret_personal_rnd[], int set_num, int heap_id);
extern const CASTLE_POKE_RANGE *Castle_EnemyPokemonRangeGet(int trainer_index, 
	int castle_level);


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
#include "savedata/castle_savedata.h"		//CASTLEDATA
#include "castle_def.h"					//CASTLE_LAP_ENEMY_MAX
#include "frontier_types.h"
#include "frontier_main.h"


//============================================================================================
//
//	バトルキャッスル　スクリプトワーク
//
//============================================================================================
typedef struct{

	int	heapID;											//ヒープ
	SAVEDATA* sv;
	CASTLEDATA* castle_savedata;						//キャッスルセーブデータへのポインタ
	void* p_work;

	u8	type;											//シングル、ダブル、マルチ、wifiマルチ
	u8	round;											//今何人目？
	u8	hinsi_flag;										//瀕死ポケモンがいるかフラグ
	u8	brain_appear_msg_flag;							//ブレーン登場メッセージ表示したか

	u16	rensyou;										//現在の連勝数
	u16	lap;											//現在の周回数

	u32	win_cnt;										//現在までの総勝利数
	int winlose_flag;									//勝ち負けフラグ

	u16 temp_start_cp;									//挑戦時のCPを退避
	u16 temp_now_cp;									//現在のCPを退避

	//参加している手持ちポケモンの位置情報
	u8	mine_poke_pos[CASTLE_ENTRY_POKE_MAX];
	u8	clear_flag;										//7連勝(クリア)したかフラグ(0か1)

	//キャッスル専用画面へ渡す用のポケモンデータ
	POKEPARTY* p_m_party;								//味方
	POKEPARTY* p_e_party;								//敵

	//敵トレーナーデータ
	u16 tr_index[CASTLE_LAP_MULTI_ENEMY_MAX];			//トレーナーindexテーブル(*2=ダブル)

	//対戦相手データ格納場所
	B_TOWER_PARTNER_DATA tr_data[2];

	//敵ポケモンデータ
	u16 enemy_poke_index[CASTLE_ENEMY_POKE_MAX];		//ポケモンindexテーブル
	u8	enemy_pow_rnd[CASTLE_ENEMY_POKE_MAX];			//ポケモンのパワー乱数
	u32 enemy_personal_rnd[CASTLE_ENEMY_POKE_MAX];		//ポケモンの個性乱数
	B_TOWER_POKEMON enemy_poke[CASTLE_ENEMY_POKE_MAX];	//ポケモンデータ
	u16 dummy_enemy_eom;								//暫定処理(EOMが2byte余計に書き込まれるため)

	u16 itemno[CASTLE_ENTRY_POKE_MAX];					//所持アイテム

	//敵トレーナー画面の情報が公開されたかフラグ
	u8	enemy_temoti_flag[CASTLE_ENEMY_POKE_MAX];
	u8	enemy_level_flag[CASTLE_ENEMY_POKE_MAX];
	u8	enemy_tuyosa_flag[CASTLE_ENEMY_POKE_MAX];
	u8	enemy_waza_flag[CASTLE_ENEMY_POKE_MAX];

	u16 ret_work[CASTLE_RET_WORK_MAX];

	//手持ちポケモン
	u16	hp[CASTLE_MINE_POKE_MAX];						//HP
	u16	pp[CASTLE_MINE_POKE_MAX][4];					//技4つのPP
	u8	condition[CASTLE_MINE_POKE_MAX];				//毒、麻痺などの状態
	u16 item[CASTLE_MINE_POKE_MAX];						//道具

	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//通信用：データバッファ
	u16	send_buf[CASTLE_COMM_BUF_LEN];
	u8	huge_buf[CASTLE_HUGE_BUF_LEN];
	u8	recieve_huge_buf[CASTLE_COMM_PLAYER_NUM][CASTLE_HUGE_BUF_LEN];

	//通信用：基本情報
	u8	pair_retire_flag;									//リタイヤを選んだか=1
	u8	pair_trade_yesno_flag;								//交換したいか(0=したい、1=したくない)
	u16	pair_lap;											//周回数

	u16	pair_trade_count;									//交換回数
	u16	pair_rensyou;										//現在の連勝数(27人目とかの表示に必要)
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

	u8	sel_type;
	u8	pair_sel_type;
	u8	recieve_count;
	u8	parent_check_flag;

	u16 pair_cp;

	u16* list_del_work;

	u32	dummy_work;
}CASTLE_SCRWORK;


//============================================================================================
//
//	extern宣言
//
//============================================================================================
extern u8 Castle_GetMinePokeNum( u8 type, BOOL flag );
extern u8 Castle_GetEnemyPokeNum( u8 type, BOOL flag );
extern void Castle_EnemyPokeMake( u8 set_num, u16 tr_index, u8 level, B_TOWER_POKEMON* rental_poke, u16* enemy_poke_index, B_TOWER_POKEMON* enemy_poke, u8* pow_rnd, u32* personal_rnd );
extern BATTLE_PARAM* BtlCastle_CreateBattleParam(CASTLE_SCRWORK* wk, FRONTIER_EX_PARAM* ex_param);
extern u8 Castle_GetLevel( CASTLE_SCRWORK* wk );
extern BOOL Castle_CommCheck( u8 type );
extern void Castle_EnemyPartySet( CASTLE_SCRWORK* wk );
extern void Castle_PokePartyRecoverAll( POKEPARTY* party );
extern void Castle_BirthInfoSet( CASTLE_SCRWORK* wk, POKEMON_PARAM* poke );
extern void Castle_PokePartyAdd( CASTLE_SCRWORK* wk, POKEPARTY* party, POKEMON_PARAM* poke );
extern u16 CastleScr_CommGetLap( CASTLE_SCRWORK* wk );
extern void Castle_LapNumWrite( GF_BGL_INI* ini, CASTLE_SCRWORK* wk, u32 frm );
extern u16 Castle_GetLap( u16 lap );
extern void Castle_CPRecord_Sub( FRONTIER_SAVEWORK* fro_sv, u8 type, int value );


#endif	//__CASTLE_TOOL_H__


