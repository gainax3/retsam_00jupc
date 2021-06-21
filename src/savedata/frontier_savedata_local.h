//==============================================================================
/**
 * @file	frontier_savedata_local.h
 * @brief	フロンティアセーブデータ
 * @author	matsuda
 * @date	2007.04.26(木)
 */
//==============================================================================
#ifndef __FRONTIER_SAVEDATA_LOCAL_H__
#define __FRONTIER_SAVEDATA_LOCAL_H__

#include "b_tower_local.h"
#include "factory_savedata_local.h"
#include "stage_savedata_local.h"
#include "castle_savedata_local.h"
#include "roulette_savedata_local.h"
#include "savedata/wifilist.h"	//WIFILIST_FRIEND_MAX


//==============================================================================
//	構造体定義
//==============================================================================

//--------------------------------------------------------------
/**
 *	フロンティア施設のレコードデータ構造体
 */
//--------------------------------------------------------------
typedef struct _FRONTIER_RECORD_SAVEWORK{
	u16 record[FRID_MAX];
	u16 wififriend_record[WIFILIST_FRIEND_MAX][FRID_WIFI_MAX - FRID_MAX];//友達手帳毎に持つレコード
}FRONTIER_RECORD_SAVEWORK;


//--------------------------------------------------------------
/**
 *	フロンティアセーブワーク
 */
//--------------------------------------------------------------
struct _FRONTIER_SAVEWORK{
	
	//--------------------------------------------------------------
	//	各施設のレコードデータ
	//--------------------------------------------------------------
	FRONTIER_RECORD_SAVEWORK frontier_record;
	
	
	//--------------------------------------------------------------
	//	プレイデータ(共通)
	//--------------------------------------------------------------
	union{
		struct _BTLTOWER_PLAYWORK	play_tower;		//バトルタワー
		struct _FACTORYDATA			play_factory;	//ファクトリー
		struct _STAGEDATA			play_stage;		//ステージ
		struct _CASTLEDATA			play_castle;	//キャッスル
		struct _ROULETTEDATA		play_roulette;	//ルーレット
	//	FACTORY_PLAYWORK play_factory;		//ファクトリー
	};
	
	//--------------------------------------------------------------
	//	個別データ
	//--------------------------------------------------------------
	//バトルタワー
	struct{
		struct _BTLTOWER_SCOREWORK	score;			//スコアデータ
		struct _BTLTOWER_PLAYER_MSG	player_msg;		//WiFi プレイヤーメッセージデータ
		struct _BTLTOWER_WIFI_DATA	wifi;			//WiFi データ
	}tower;
	
	//バトルファクトリー
	struct{
		struct _FACTORYSCORE score;					//スコアデータ
	}factory;

	//バトルステージ
	struct{
		struct _STAGESCORE score;					//スコアデータ
	}stage;

	//バトルキャッスル
	struct{
		struct _CASTLESCORE score;					//スコアデータ
	}castle;

	//バトルルーレット
	struct{
		struct _ROULETTESCORE score;				//スコアデータ
	}roulette;

	//ファクトリー
//	struct{
//		u32 test;
//	}factory;
};




#endif	//__FRONTIER_SAVEDATA_LOCAL_H__
