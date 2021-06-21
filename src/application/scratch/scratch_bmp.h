//==============================================================================================
/**
 * @file	scratch_bmp.h
 * @brief	「スクラッチ」BMP
 * @author	Satoshi Nohara
 * @date	08.12.11
 */
//==============================================================================================
#ifndef _SCRATCH_BMP_H_
#define _SCRATCH_BMP_H_

#include "scratch_sys.h"
#include "system/window.h"


//==============================================================================================
//
//	定義
//
//==============================================================================================
//ビットマップID
enum{
	BMPWIN_TALK = 0,
	BMPWIN_YESNO,
	BMPWIN_ITEM1,
	BMPWIN_ITEM2,
	BMPWIN_ITEM3,
	BMPWIN_ITEM4,
	BMPWIN_NUM1,
	BMPWIN_NUM2,
	BMPWIN_NUM3,
	BMPWIN_NUM4,

	BMPWIN_START,
	BMPWIN_ATARI,
	BMPWIN_HAZURE,
	BMPWIN_NEXT,
	BMPWIN_YAMERU,
	BMPWIN_SELECT,

	SCRATCH_BMPWIN_MAX,				//BMPウィンドウの数
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
extern void ScratchAddBmpWin( GF_BGL_INI* bgl, GF_BGL_BMPWIN* win );

//--------------------------------------------------------------
/**
 * @brief	ビットマップウィンドウ破棄	
 *
 * @param	win		ビットマップウィンドウ
 *
 * @return	none
 */
//--------------------------------------------------------------
extern void ScratchExitBmpWin( GF_BGL_BMPWIN* win );

//--------------------------------------------------------------
/**
 * @brief	ビットマップウィンドウオフ
 *
 * @param	win		ビットマップウィンドウ
 *
 * @return	none
 */
//--------------------------------------------------------------
extern void ScratchOffBmpWin( GF_BGL_BMPWIN* win );

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
extern void ScratchWriteMenuWin( GF_BGL_INI* bgl, GF_BGL_BMPWIN* win );

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
extern void ScratchTalkWinPut( GF_BGL_BMPWIN * win, WINTYPE wintype );


#endif //_SCRATCH_BMP_H_


