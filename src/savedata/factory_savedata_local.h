//==============================================================================================
/**
 * @file	factory_savedata_local.h
 * @brief	「バトルファクトリー」ヘッダー
 * @author	Satoshi Nohara
 * @date	2007.03.28
 */
//==============================================================================================
#ifndef	__FACTORY_SAVEDATA_LOCAL_H__
#define	__FACTORY_SAVEDATA_LOCAL_H__

#include "../frontier/factory_def.h"

//============================================================================================
//
//	プレイデータ
//
//============================================================================================
struct _FACTORYDATA{
	u8	level		:1;										//LV50、オープン
	u8	type		:3;										//シングル、ダブル、マルチ、wifiマルチ
	u8	save_flag	:1;										//セーブ済みかどうかフラグ
	u8	dmy99		:3;
	u8	round;												//今何人目？
	u16	dummy;

	//敵トレーナーデータ
	u16 tr_index[FACTORY_LAP_MULTI_ENEMY_MAX];				//トレーナーindexテーブル

	//手持ちのデータ
	u16 temoti_poke_index[FACTORY_MINE_POKE_MAX];			//ポケモンindexテーブル
	u8	temoti_pow_rnd[FACTORY_MINE_POKE_MAX];				//ポケモンのpow乱数
	u32 temoti_personal_rnd[FACTORY_MINE_POKE_MAX];			//ポケモンの個性乱数

	//敵のデータ
	u16 enemy_poke_index[FACTORY_ENEMY_POKE_MAX];			//ポケモンindexテーブル
	u8	enemy_pow_rnd[FACTORY_ENEMY_POKE_MAX];				//ポケモンのpow乱数
	u32 enemy_personal_rnd[FACTORY_ENEMY_POKE_MAX];			//ポケモンの個性乱数
};


//============================================================================================
//
//	成績データ
//
//============================================================================================
struct _FACTORYSCORE{
	//↓WIFI以外のバトルタイプで使用している
	u8	clear_flag;											//7連勝(クリア)したかフラグ(ビット)
	u8	dmy[3];
};


#endif	__FACTORY_SAVEDATA_LOCAL_H__


