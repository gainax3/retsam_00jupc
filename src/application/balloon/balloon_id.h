//==============================================================================
/**
 * @file	balloon_id.h
 * @brief	キャラID、セルID等の定義場所
 * @author	matsuda
 * @date	2007.11.27(火)
 */
//==============================================================================
#ifndef __BALLOON_ID_H__
#define __BALLOON_ID_H__


//==============================================================================
//	キャラID
//==============================================================================
enum{
	CHARID_BALLOON_START = 10000,
	
	CHARID_TOUCH_PEN,		///<タッチペン
	CHARID_COUNTER_WIN,		///<カウンターのウィンドウ
	CHARID_BALLOON_AIM,		///<照準
	
	//-- サブ画面 --//
	CHARID_SUB_BALLOON_MIX,			///<色々なキャラごちゃ混ぜ
};

//==============================================================================
//	パレットID
//==============================================================================
enum{
	PLTTID_BALLOON_START = 10000,

	PLTTID_OBJ_COMMON,	///<常駐OBJパレット
	
	PLTTID_TOUCH_PEN,	///<タッチペン
	PLTTID_COUNTER,		///<カウンター
	PLTTID_COUNTER_WIN,	///<カウンターのウィンドウ
	PLTTID_OBJ_AIM,		///<照準
	
	//-- サブ画面 --//
	PLTTID_SUB_OBJ_COMMON,	///<サブ画面の常駐OBJパレット
};

//--------------------------------------------------------------
//	常駐OBJパレットオフセット
//--------------------------------------------------------------
enum{
	PALOFS_NONE = 0,

	
	BALLOON_COMMON_PAL_NUM = 3,	///<常駐OBJパレットの本数
};

//--------------------------------------------------------------
//	サブ画面の常駐OBJパレットオフセット
//--------------------------------------------------------------
enum{
	PALOFS_SUB_AIR_BLUE = 1,			///<空気：青
	PALOFS_SUB_AIR_RED = 2,				///<空気：赤
	PALOFS_SUB_AIR_GREEN = 4,			///<空気：緑
	PALOFS_SUB_AIR_YELLOW = 3,			///<空気：黄

	PALOFS_SUB_EXPLODED = 5,				///<爆発時の紙ふぶき
	PALOFS_SUB_EXPLODED_SMOKE = 0,			///<爆発時の煙
	PALOFS_SUB_EXPLODED_CHIP_BLUE =6,		///<破片:青
	PALOFS_SUB_EXPLODED_CHIP_RED =7,		///<破片:赤
	PALOFS_SUB_EXPLODED_CHIP_YELLOW =8,		///<破片:黄

	PALOFS_SUB_ICON_BALLOON = 0,	///<風船アイコン
	
	PALOFS_SUB_JOINT = 0,			///<ジョイント
	PALOFS_SUB_BOOSTER_BLUE = 1,	///<ブースター(青)
	PALOFS_SUB_BOOSTER_RED = 2,		///<ブースター(赤)
	PALOFS_SUB_BOOSTER_YELLOW = 3,	///<ブースター(黄)
	PALOFS_SUB_BOOSTER_GREEN = 4,	///<ブースター(緑)
	PALOFS_SUB_BOOSTER_HIT = 0,		///<ブースター:ヒットエフェクト
	PALOFS_SUB_BOOSTER_LAND_SMOKE = 0,	///<ブースター:着地の煙
	PALOFS_SUB_BOOSTER_SHADOW = 1,	///<ブースター:影
	
	BALLOON_SUB_COMMON_PAL_NUM = 9,	///<サブ画面の常駐OBJパレットの本数
};

//==============================================================================
//	セルID
//==============================================================================
enum{
	CELLID_BALLOON_START = 10000,

	CELLID_TOUCH_PEN,		///<タッチペン
	CELLID_COUNTER_WIN,		///<カウンターのウィンドウ
	CELLID_BALLOON_AIM,		///<照準

	//-- サブ画面 --//
	CELLID_SUB_BALLOON_MIX,			///<色々なセルごちゃ混ぜ
};

//==============================================================================
//	セルアニメID
//==============================================================================
enum{
	CELLANMID_BALLOON_START = 10000,

	CELLANMID_TOUCH_PEN,		///<タッチペン
	CELLANMID_COUNTER_WIN,		///<カウンターのウィンドウ
	CELLANMID_BALLOON_AIM,		///<照準

	//-- サブ画面 --//
	CELLANMID_SUB_BALLOON_MIX,		///<色々なセルアニメごちゃ混ぜ
};

//==============================================================================
//	マルチセルID
//==============================================================================
enum{
	MCELLID_BALLOON_START = 10000,
};

//==============================================================================
//	マルチセルアニメID
//==============================================================================
enum{
	MCELLANMID_BALLOON_START = 10000,
};



#endif	//__BALLOON_ID_H__
