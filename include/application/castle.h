//==============================================================================================
/**
 * @file	castle.h
 * @brief	「バトルファクトリー」ヘッダー
 * @author	Satoshi Nohara
 * @date	2007.07.04
 */
//==============================================================================================
#ifndef _CASTLE_H_
#define _CASTLE_H_

#include "../../src/frontier/castle_def.h"


//==============================================================================================
//
//	定義
//
//==============================================================================================


//==============================================================================================
//
//	ファクトリー画面呼び出し　構造体宣言
//
//==============================================================================================
typedef struct{
	SAVEDATA* sv;

	u8	type; 
	u8	level;
	u8	mode; 
	u8	dummy;

	//敵トレーナー画面の情報が公開されたかフラグ
	u8	enemy_temoti_flag[CASTLE_ENEMY_POKE_MAX];
	u8	enemy_level_flag[CASTLE_ENEMY_POKE_MAX];
	u8	enemy_tuyosa_flag[CASTLE_ENEMY_POKE_MAX];
	u8	enemy_waza_flag[CASTLE_ENEMY_POKE_MAX];

	POKEPARTY* p_m_party;							//味方
	POKEPARTY* p_e_party;							//敵
	u16 ret_work;									//戻り値格納ワーク
	void* p_work;									//CASTLE_SCRWORK(bf_scr_wk保存)

	u16 pair_cp;									//パートナーのCP
	u16 dummy3;

	u32 dummy_work;
}CASTLE_CALL_WORK;


//==============================================================================================
//
//	extern宣言
//
//==============================================================================================
//extern const PROC_DATA CastleProcData;				//プロセス定義データ
extern PROC_RESULT CastleMineProc_Init( PROC * proc, int * seq );
extern PROC_RESULT CastleMineProc_Main( PROC * proc, int * seq );
extern PROC_RESULT CastleMineProc_End( PROC * proc, int * seq );
extern PROC_RESULT CastleEnemyProc_Init( PROC * proc, int * seq );
extern PROC_RESULT CastleEnemyProc_Main( PROC * proc, int * seq );
extern PROC_RESULT CastleEnemyProc_End( PROC * proc, int * seq );
extern PROC_RESULT CastleRankProc_Init( PROC * proc, int * seq );
extern PROC_RESULT CastleRankProc_Main( PROC * proc, int * seq );
extern PROC_RESULT CastleRankProc_End( PROC * proc, int * seq );


#endif //_CASTLE_H_


