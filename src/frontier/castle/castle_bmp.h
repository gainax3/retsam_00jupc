//==============================================================================================
/**
 * @file	castle_bmp.h
 * @brief	「バトルキャッスル」BMP
 * @author	Satoshi Nohara
 * @date	08.07.05
 */
//==============================================================================================
#ifndef _CASTLE_BMP_H_
#define _CASTLE_BMP_H_

#include "castle_sys.h"
#include "system/window.h"


//==============================================================================================
//
//	定義
//
//==============================================================================================
//BMPデータナンバー
enum{
	CASTLE_BMP_MINE = 0,		//手持ちポケモン画面
	CASTLE_BMP_ENEMY,			//敵トレーナー画面
	CASTLE_BMP_MAX,
};


//==============================================================================================
//
//	手持ちポケモン　データ
//
//==============================================================================================
//ビットマップID
enum{
	MINE_BMPWIN_TR1 = 0,
	MINE_BMPWIN_MODORU,
	MINE_BMPWIN_LV,
	MINE_BMPWIN_HP,
	MINE_BMPWIN_STATUS,
	MINE_BMPWIN_LIST,
	MINE_BMPWIN_TALK,
	MINE_BMPWIN_TALKMENU,
	MINE_BMPWIN_TALKMENU2,
	MINE_BMPWIN_BASIC_LIST,
	MINE_BMPWIN_KAIHUKU_LIST,
	MINE_BMPWIN_RENTAL_LIST,
	MINE_BMPWIN_ITEMINFO,
	MINE_BMPWIN_YESNO,
	MINE_BMPWIN_SEL,
	MINE_BMPWIN_CP,
	MINE_BMPWIN_ITEM_LV,
	MINE_BMPWIN_ITEM_HP,
	CASTLE_MINE_BMPWIN_MAX,			//BMPウィンドウの数
};


//==============================================================================================
//
//	敵トレーナー　データ
//
//==============================================================================================
//ビットマップID
enum{
	ENEMY_BMPWIN_TR1 = 0,
	ENEMY_BMPWIN_MODORU,
	ENEMY_BMPWIN_LV,
	ENEMY_BMPWIN_HP,
	ENEMY_BMPWIN_STATUS,
	ENEMY_BMPWIN_BASIC_LIST,
	ENEMY_BMPWIN_BASIC2_LIST,
	ENEMY_BMPWIN_TALK,
	ENEMY_BMPWIN_TALKMENU,
	ENEMY_BMPWIN_TALKMENU2,
	ENEMY_BMPWIN_YESNO,
	ENEMY_BMPWIN_SEL,
	CASTLE_ENEMY_BMPWIN_MAX,		//BMPウィンドウの数
};


//==============================================================================================
//
//	extern宣言
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	ビットマップ追加
 *
 * @param	ini		BGLデータ
 * @param	win		ビットマップウィンドウ
 *
 * @return	none
 */
//--------------------------------------------------------------
extern void CastleAddBmpWin( GF_BGL_INI* bgl, GF_BGL_BMPWIN* win, u8 bmp_no );

//--------------------------------------------------------------
/**
 * @brief	ビットマップウィンドウ破棄	
 *
 * @param	win		ビットマップウィンドウ
 *
 * @return	none
 */
//--------------------------------------------------------------
extern void CastleExitBmpWin( GF_BGL_BMPWIN* win, u8 bmp_no );

//--------------------------------------------------------------
/**
 * @brief	ビットマップウィンドウオフ
 *
 * @param	win		ビットマップウィンドウ
 *
 * @return	none
 */
//--------------------------------------------------------------
extern void CastleOffBmpWin( GF_BGL_BMPWIN* win, u8 bmp_no );

//--------------------------------------------------------------
/**
 * @brief	メニューウィンドウ表示
 *
 * @param	ini		BGLデータ
 * @param	win		ビットマップウィンドウ
 *
 * @return	none
 */
//--------------------------------------------------------------
extern void CastleWriteMenuWin( GF_BGL_INI* bgl, GF_BGL_BMPWIN* win );

//--------------------------------------------------------------------------------------------
/**
 * 会話ウィンドウ表示
 *
 * @param	win		BMPデータ
 * @param	wintype	ウィンドウタイプ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void CastleTalkWinPut( GF_BGL_BMPWIN * win, WINTYPE wintype );


#endif //_CASTLE_BMP_H_


