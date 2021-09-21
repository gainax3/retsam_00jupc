//==============================================================================================
/**
 * @file	frontier_bmp.h
 * @brief	「フロンティア」BMP
 * @author	Satoshi Nohara
 * @date	2007.12.6
 */
//==============================================================================================
#ifndef _FRONTIER_MONITOR_BMP_H_
#define _FRONTIER_MONITOR_BMP_H_

#include "system/window.h"


//==============================================================================================
//
//	定義
//
//==============================================================================================
//ファクトリー：ビットマップID
enum{
	BF_BMPWIN_TITLE = 0,
	BF_BMPWIN_ENTRY1,
	BF_BMPWIN_BEFORE,
	BF_BMPWIN_MAX,
	BF_BMPWIN_ENTRY2,
	BF_BMPWIN_BEFORE2,
	BF_BMPWIN_MAX2,
	FACTORY_MONITOR_BMPWIN_MAX,				//BMPウィンドウの数
};

//ステージ：ビットマップID
enum{
	BS_BMPWIN_TITLE = 0,
	BS_BMPWIN_ENTRY1,
	BS_BMPWIN_BEFORE,
	BS_BMPWIN_MAX,
	STAGE_MONITOR_BMPWIN_MAX,				//BMPウィンドウの数
};

//キャッスル：ビットマップID
enum{
	BC_BMPWIN_TITLE = 0,
	BC_BMPWIN_ENTRY1,
	BC_BMPWIN_BEFORE,
	BC_BMPWIN_MAX,
	CASTLE_MONITOR_BMPWIN_MAX,				//BMPウィンドウの数
};

//ルーレット：ビットマップID
enum{
	BR_BMPWIN_TITLE = 0,
	BR_BMPWIN_BEFORE,
	BR_BMPWIN_MAX,
	ROULETTE_MONITOR_BMPWIN_MAX,				//BMPウィンドウの数
};

//タワー：ビットマップID
enum{
	BT_BMPWIN_TITLE = 0,
	BT_BMPWIN_ENTRY1,
	BT_BMPWIN_BEFORE0,
	BT_BMPWIN_MAX0,
	BT_BMPWIN_BEFORE,
	BT_BMPWIN_MAX,
	BT_BMPWIN_ENTRY2,
	BT_BMPWIN_BEFORE2,
	BT_BMPWIN_MAX2,
	TOWER_MONITOR_BMPWIN_MAX,					//BMPウィンドウの数
};

#define FRONTIER_MONITOR_BMP_MAX		(5)		//

//パレット定義
#define BFM_MSGFONT_PAL			(12)				//MSGフォント使用パレットナンバー
#define BFM_FONT_PAL			(13)				//SYSTEMフォント使用パレットナンバー

#define BFM_FRAME_WIN	(GF_BGL_FRAME0_M)
#define BFM_FRAME_BG	(GF_BGL_FRAME2_M)			//MULTIとレンタル決定時
#define BFM_FRAME_SUB	(GF_BGL_FRAME2_S)			//ボール


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
extern void FrontierMonitorAddBmpWin( GF_BGL_INI* bgl, GF_BGL_BMPWIN* win, u8 bmp_no );

//--------------------------------------------------------------
/**
 * @brief	ビットマップウィンドウ破棄	
 *
 * @param	win		ビットマップウィンドウ
 *
 * @return	none
 */
//--------------------------------------------------------------
extern void FrontierMonitorExitBmpWin( GF_BGL_BMPWIN* win, u8 bmp_no );

//--------------------------------------------------------------
/**
 * @brief	ビットマップウィンドウオフ
 *
 * @param	win		ビットマップウィンドウ
 *
 * @return	none
 */
//--------------------------------------------------------------
extern void FrontierMonitorOffBmpWin( GF_BGL_BMPWIN* win, u8 bmp_no );


#endif //_FRONTIER_MONITOR_BMP_H_


