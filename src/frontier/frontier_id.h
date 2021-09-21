//==============================================================================
/**
 * @file	frontier_id.h
 * @brief	フロンティアマップで使用するセルIDなどの定義場所
 * @author	matsuda
 * @date	2007.09.18(火)
 */
//==============================================================================
#ifndef __FRONTIER_ID_H__
#define __FRONTIER_ID_H__

//==============================================================================
//	ポケモンアイコン
//==============================================================================
//キャラクタID
enum{
	//ポケモンアイコン
	FR_CHARID_POKEICON_0 = 2000,
	FR_CHARID_POKEICON_1,
	FR_CHARID_POKEICON_2,
	FR_CHARID_POKEICON_3,
	FR_CHARID_POKEICON_4,
	FR_CHARID_POKEICON_5,
	FR_CHARID_POKEICON_6,
	FR_CHARID_POKEICON_7,
	FR_CHARID_POKEICON_MAX,
	
	//アイテムアイコン
	FR_CHARID_ITEMICON,
	
	//カットインエフェクト
	FR_CHARID_CUTIN_FACE,		//顔
	FR_CHARID_CUTIN_VS,			//VSマーク
};

//パレットID
enum{
	//ポケモンアイコン
	FR_PLTTID_POKEICON = 2000,

	//アイテムアイコン
	FR_PLTTID_ITEMICON,

	//カットインエフェクト
	FR_PLTTID_CUTIN_FACE,		//顔
	FR_PLTTID_CUTIN_VS,			//VSマーク
	FR_PLTTID_CUTIN_NAME,		//トレーナー名
};

//セルID
enum{
	//ポケモンアイコン
	FR_CELLID_POKEICON = 2000,

	//アイテムアイコン
	FR_CELLID_ITEMICON,

	//カットインエフェクト
	FR_CELLID_CUTIN_FACE,		//顔
	FR_CELLID_CUTIN_VS,			//VSマーク
};

//セルアニメID
enum{
	//ポケモンアイコン
	FR_CELLANMID_POKEICON = 2000,

	//アイテムアイコン
	FR_CELLANMID_ITEMICON,

	//カットインエフェクト
	FR_CELLANMID_CUTIN_FACE,		//顔
	FR_CELLANMID_CUTIN_VS,			//VSマーク
};


#endif	//__FRONTIER_ID_H__
