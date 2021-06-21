//==============================================================================================
/**
 * @file	roulette_savedata_local.h
 * @brief	「バトルルーレット」ヘッダー
 * @author	Satoshi Nohara
 * @date	2007.09.05
 */
//==============================================================================================
#ifndef	__ROULETTE_SAVEDATA_LOCAL_H__
#define	__ROULETTE_SAVEDATA_LOCAL_H__

#include "..\frontier\roulette_def.h"

//============================================================================================
//
//	プレイデータ
//
//============================================================================================
struct _ROULETTEDATA{
	u8	type		:3;									//シングル、ダブル、マルチ、wifiマルチ
	u8	save_flag	:1;									//セーブ済みかどうかフラグ
	u8	random_flag	:1;									//カーソルのランダム移動onフラグ
	u8	dmy99		:3;
	u8	round;											//今何人目？
	u8	csr_speed_level;								//カーソルのスピードレベル

	//参加している手持ちポケモンの位置情報
	u8 mine_poke_pos[ROULETTE_ENTRY_POKE_MAX];			//

	//手持ちポケモン
	u16 item[ROULETTE_MINE_POKE_MAX];					//道具

	//敵トレーナーデータ
	u16 tr_index[ROULETTE_LAP_MULTI_ENEMY_MAX];			//トレーナーindexテーブル[7*2]

	//敵ポケモンデータが被らないように保存
	u16 enemy_poke_index[ROULETTE_LAP_MULTI_ENEMY_MAX];	//ポケモンindexテーブル

	u16 dummy;											//4byte境界
};


//============================================================================================
//
//	成績データ
//
//============================================================================================
struct _ROULETTESCORE{
	//↓WIFI以外のバトルタイプで使用している
	u8	clear_flag;										//7連勝(クリア)したかフラグ(ビット)
	u8	dmy[3];
};


#endif	__ROULETTE_SAVEDATA_LOCAL_H__


