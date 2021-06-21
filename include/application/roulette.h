//==============================================================================================
/**
 * @file	roulette.h
 * @brief	「バトルルーレット」ヘッダー
 * @author	Satoshi Nohara
 * @date	2007.09.05
 */
//==============================================================================================
#ifndef _ROULETTE_H_
#define _ROULETTE_H_

#include "../../src/frontier/roulette_def.h"


//==============================================================================================
//
//	定義
//
//==============================================================================================


//==============================================================================================
//
//	ルーレット画面呼び出し　構造体宣言
//
//==============================================================================================
typedef struct{
	SAVEDATA* sv;

	u8	type; 
	u8	level;
	u8	mode; 
	u8	point;

	int* p_weather;									//天候
	u8*	p_csr_speed_level;							//カーソルスピードレベル
	u8*	p_decide_ev_no;								//決定したイベントナンバー
	u16* p_rand_pos;								//共通のランダム

	u16	pair_rensyou;
	u16	dummy2;

	u16	rensyou;
	u16	lap;

	//敵トレーナー画面の情報が公開されたかフラグ
	u8	enemy_temoti_flag[ROULETTE_ENEMY_POKE_MAX];
	u8	enemy_level_flag[ROULETTE_ENEMY_POKE_MAX];
	u8	enemy_tuyosa_flag[ROULETTE_ENEMY_POKE_MAX];
	u8	enemy_waza_flag[ROULETTE_ENEMY_POKE_MAX];

	POKEPARTY* p_m_party;							//味方
	POKEPARTY* p_e_party;							//敵
//	u16 ret_work[ROULETTE_RET_WORK_MAX];				//戻り値格納ワーク
	u16 ret_work;									//戻り値格納ワーク
	void* p_work;									//ROULETTE_SCRWORK(bf_scr_wk保存)

	u16 pair_cp;									//パートナーのCP

	u8	random_flag;								//カーソルランダム移動onフラグ
	u8	dummy_work;
	u16	dummy_work2;
}ROULETTE_CALL_WORK;


//==============================================================================================
//
//	extern宣言
//
//==============================================================================================
//extern const PROC_DATA RouletteProcData;				//プロセス定義データ
extern PROC_RESULT RouletteProc_Init( PROC * proc, int * seq );
extern PROC_RESULT RouletteProc_Main( PROC * proc, int * seq );
extern PROC_RESULT RouletteProc_End( PROC * proc, int * seq );


#endif //_ROULETTE_H_


