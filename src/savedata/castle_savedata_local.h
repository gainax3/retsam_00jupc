//==============================================================================================
/**
 * @file	castle_savedata_local.h
 * @brief	「バトルキャッスル」ヘッダー
 * @author	Satoshi Nohara
 * @date	2007.07.03
 */
//==============================================================================================
#ifndef	__CASTLE_SAVEDATA_LOCAL_H__
#define	__CASTLE_SAVEDATA_LOCAL_H__

#include "../frontier/castle_def.h"

//============================================================================================
//
//	プレイデータ
//
//============================================================================================
struct _CASTLEDATA{
	u8	type		:3;									//シングル、ダブル、マルチ、wifiマルチ
	u8	save_flag	:1;									//セーブ済みかどうかフラグ
	u8	dmy99		:4;
	u8	round;											//今何人目？
	u8	dmy89;

	//参加している手持ちポケモンの位置情報
	u8 mine_poke_pos[3];

	//敵トレーナーデータ
	u16 tr_index[CASTLE_LAP_MULTI_ENEMY_MAX];			//トレーナーindexテーブル[7*2]

	//手持ちポケモン
#if 0
	u16	hp[CASTLE_MINE_POKE_MAX];						//HP					//2*4	=8
	u16	pp[CASTLE_MINE_POKE_MAX][4];					//技4つのPP				//2*4x4	=32
	u8	condition[CASTLE_MINE_POKE_MAX];				//毒、麻痺などの状態	//1*4	=4
	u16 item[CASTLE_MINE_POKE_MAX];						//道具					//2*4   =8 = 52
#else
	//08.05.28 conditionはu32必要なので、ppを減らして対処
	u16	hp[CASTLE_MINE_POKE_MAX];						//HP					//2*4	=8
	u8	pp[CASTLE_MINE_POKE_MAX][4];					//技4つのPP				//1*4*4	=16
	u32	condition[CASTLE_MINE_POKE_MAX];				//毒、麻痺などの状態	//4*4	=16
	//u8	dmy35[4];									//余り					//1*4	=4
	u16 item[CASTLE_MINE_POKE_MAX];						//道具					//2*4   =8 = 52
#endif

	//敵ポケモンデータが被らないように保存
	u16 enemy_poke_index[CASTLE_LAP_MULTI_ENEMY_MAX];	//ポケモンindexテーブル

	u16 dummy;											//4byte境界				//116
};


//============================================================================================
//
//	成績データ
//
//============================================================================================
struct _CASTLESCORE{
	//↓WIFI以外のバトルタイプで使用している
	u8	clear_flag;										//7連勝(クリア)したかフラグ(ビット)

	u8	guide_flag;										//説明受けたか
	u8	dmy[2];
};


#endif	__CASTLE_SAVEDATA_LOCAL_H__


