//==============================================================================================
/**
 * @file	factory_bmp.c
 * @brief	「バトルファクトリー」BMP
 * @author	Satoshi Nohara
 * @date	2007.03.15
 */
//==============================================================================================
#include "common.h"
#include "system/fontproc.h"
#include "system/pm_str.h"
#include "system/buflen.h"
#include "system/wordset.h"
#include "savedata/config.h"

#include "factory_sys.h"
#include "factory_bmp.h"


//==============================================================================================
//
//	定義
//
//==============================================================================================
//「トレーナー名」
#define WIN_TR1_PX			(1)
#define WIN_TR1_PY			(1)
#define WIN_TR1_SX			(10)
#define WIN_TR1_SY			(2)
#define WIN_TR1_CGX			(1)

//「パートナー名」
#define WIN_TR2_PX			(22)
#define WIN_TR2_PY			(1)
#define WIN_TR2_SX			(10)
#define WIN_TR2_SY			(2)
#define WIN_TR2_CGX			(WIN_TR1_CGX + WIN_TR1_SX * WIN_TR1_SY)

//「選択したポケモン名1」
#define WIN_POKE1_PX		(1)
#define WIN_POKE1_PY		(4)
#define WIN_POKE1_SX		(9)
#define WIN_POKE1_SY		(2)
#define WIN_POKE1_CGX		(WIN_TR2_CGX + WIN_TR2_SX * WIN_TR2_SY)

//「選択したポケモン名2」
#define WIN_POKE2_PX		(1)
#define WIN_POKE2_PY		(6)
#define WIN_POKE2_SX		(9)
#define WIN_POKE2_SY		(2)
#define WIN_POKE2_CGX		(WIN_POKE1_CGX + WIN_POKE1_SX * WIN_POKE1_SY)

//「選択したポケモン名3」
#define WIN_POKE3_PX		(1)
#define WIN_POKE3_PY		(8)
#define WIN_POKE3_SX		(9)
#define WIN_POKE3_SY		(2)
#define WIN_POKE3_CGX		(WIN_POKE2_CGX + WIN_POKE2_SX * WIN_POKE2_SY)

//「会話」
#define WIN_TALK_PX			(2)
#define WIN_TALK_PY			(19)
#define WIN_TALK_SX			(17)
#define WIN_TALK_SY			(4)
#define WIN_TALK_CGX		(WIN_POKE3_CGX + WIN_POKE3_SX * WIN_POKE3_SY)

//「はい・いいえ」
#define WIN_YESNO_PX		(23)
#define WIN_YESNO_PY		(19)
#define WIN_YESNO_SX		(8)
#define WIN_YESNO_SY		(4)
#define WIN_YESNO_CGX		(WIN_TALK_CGX + WIN_TALK_SX * WIN_TALK_SY)

//「つよさをみる」
#define WIN_SEL_PX			(23)
#define WIN_SEL_PY			(17)
#define WIN_SEL_SX			(8)
#define WIN_SEL_SY			(6)
#define WIN_SEL_CGX			(WIN_YESNO_CGX + WIN_YESNO_SX * WIN_YESNO_SY)

//「パートナーが選択したポケモン名1」
#define WIN_POKE4_PX		(22)
#define WIN_POKE4_PY		(4)
#define WIN_POKE4_SX		(9)
#define WIN_POKE4_SY		(2)
#define WIN_POKE4_CGX		(WIN_SEL_CGX + WIN_SEL_SX * WIN_SEL_SY)

//「パートナーが選択したポケモン名2」
#define WIN_POKE5_PX		(22)
#define WIN_POKE5_PY		(6)
#define WIN_POKE5_SX		(9)
#define WIN_POKE5_SY		(2)
#define WIN_POKE5_CGX		(WIN_POKE4_CGX + WIN_POKE4_SX * WIN_POKE4_SY)

//メニューウィンドウキャラ
#define BF_MENU_CGX_NUM		(1024-MENU_WIN_CGX_SIZ)

//会話ウィンドウキャラ
#define	BF_TALKWIN_CGX_SIZE	(TALK_WIN_CGX_SIZ)
#define	BF_TALKWIN_CGX_NUM	(BF_MENU_CGX_NUM - BF_TALKWIN_CGX_SIZE)


//==============================================================================================
//
//	データ
//
//==============================================================================================
static const BMPWIN_DAT FactoryBmpData[] =
{
	{	// 0:「トレーナー名」
		BF_FRAME_WIN, WIN_TR1_PX, WIN_TR1_PY,
		WIN_TR1_SX, WIN_TR1_SY, BF_FONT_PAL, WIN_TR1_CGX
	},	
	{	// 1:「パートナー名」
		BF_FRAME_WIN, WIN_TR2_PX, WIN_TR2_PY,
		WIN_TR2_SX, WIN_TR2_SY, BF_FONT_PAL, WIN_TR2_CGX
	},	
	{	// 2:「ポケモン名1」
		BF_FRAME_WIN, WIN_POKE1_PX, WIN_POKE1_PY,
		WIN_POKE1_SX, WIN_POKE1_SY, BF_FONT_PAL, WIN_POKE1_CGX
	},	
	{	// 3:「ポケモン名2」
		BF_FRAME_WIN, WIN_POKE2_PX, WIN_POKE2_PY,
		WIN_POKE2_SX, WIN_POKE2_SY, BF_FONT_PAL, WIN_POKE2_CGX
	},	
	{	// 4:「ポケモン名3」
		BF_FRAME_WIN, WIN_POKE3_PX, WIN_POKE3_PY,
		WIN_POKE3_SX, WIN_POKE3_SY, BF_FONT_PAL, WIN_POKE3_CGX
	},	
	{	// 5:「会話」
		BF_FRAME_WIN, WIN_TALK_PX, WIN_TALK_PY,
		WIN_TALK_SX, WIN_TALK_SY, BF_MSGFONT_PAL, WIN_TALK_CGX
	},
	{	// 6:「はい・いいえ」
		BF_FRAME_WIN, WIN_YESNO_PX, WIN_YESNO_PY,
		WIN_YESNO_SX, WIN_YESNO_SY, BF_FONT_PAL, WIN_YESNO_CGX
	},
	{	// 7:「つよさをみる」
		BF_FRAME_WIN, WIN_SEL_PX, WIN_SEL_PY,
		WIN_SEL_SX, WIN_SEL_SY, BF_FONT_PAL, WIN_SEL_CGX
	},
	{	// 8:「パートナーのポケモン名1」
		BF_FRAME_WIN, WIN_POKE4_PX, WIN_POKE4_PY,
		WIN_POKE4_SX, WIN_POKE4_SY, BF_FONT_PAL, WIN_POKE4_CGX
	},	
	{	// 9:「パートナーのポケモン名2」
		BF_FRAME_WIN, WIN_POKE5_PX, WIN_POKE5_PY,
		WIN_POKE5_SX, WIN_POKE5_SY, BF_FONT_PAL, WIN_POKE5_CGX
	},	
};


//==============================================================================================
//
//	プロトタイプ宣言
//
//==============================================================================================
void FactoryAddBmpWin( GF_BGL_INI* bgl, GF_BGL_BMPWIN* win );
void FactoryExitBmpWin( GF_BGL_BMPWIN* win );
void FactoryOffBmpWin( GF_BGL_BMPWIN* win );
void FactoryWriteMenuWin( GF_BGL_INI* bgl, GF_BGL_BMPWIN* win );
void FactoryTalkWinPut( GF_BGL_BMPWIN * win, WINTYPE wintype );


//==============================================================================================
//
//	関数
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
void FactoryAddBmpWin( GF_BGL_INI* bgl, GF_BGL_BMPWIN* win )
{
	u8 i;
	const BMPWIN_DAT* dat = FactoryBmpData;

	//ビットマップ追加
	for( i=0; i < FACTORY_BMPWIN_MAX; i++ ){
		GF_BGL_BmpWinAddEx( bgl, &win[i], &dat[i] );
		GF_BGL_BmpWinDataFill( &win[i], FBMP_COL_NULL );		//塗りつぶし
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	ビットマップウィンドウ破棄	
 *
 * @param	win		ビットマップウィンドウ
 *
 * @return	none
 */
//--------------------------------------------------------------
void FactoryExitBmpWin( GF_BGL_BMPWIN* win )
{
	u16	i;

	for( i=0; i < FACTORY_BMPWIN_MAX; i++ ){
		GF_BGL_BmpWinDel( &win[i] );
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	ビットマップウィンドウオフ
 *
 * @param	win		ビットマップウィンドウ
 *
 * @return	none
 */
//--------------------------------------------------------------
void FactoryOffBmpWin( GF_BGL_BMPWIN* win )
{
	u16	i;

	for( i=0; i < FACTORY_BMPWIN_MAX; i++ ){
		GF_BGL_BmpWinOff( &win[i] );
	}

	return;
}

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
void FactoryWriteMenuWin( GF_BGL_INI* bgl, GF_BGL_BMPWIN* win )
{
	//メニューウィンドウのグラフィックをセット
	MenuWinGraphicSet( bgl, BF_FRAME_WIN, BF_MENU_CGX_NUM, BF_MENU_PAL, 0, HEAPID_FACTORY );

	//メニューウィンドウを描画
	BmpMenuWinWrite( win, WINDOW_TRANS_OFF, BF_MENU_CGX_NUM, BF_MENU_PAL );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	会話ウィンドウ表示
 *
 * @param	win		BMPデータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FactoryTalkWinPut( GF_BGL_BMPWIN * win, WINTYPE wintype )
{
	TalkWinGraphicSet( win->ini, GF_BGL_BmpWinGet_Frame(win), BF_TALKWIN_CGX_NUM,
						BF_TALKWIN_PAL, wintype, HEAPID_FACTORY );

	GF_BGL_BmpWinDataFill( win, 15 );
	//BmpTalkWinWrite( win, WINDOW_TRANS_ON, BF_TALKWIN_CGX_NUM, BF_TALKWIN_PAL );
	BmpTalkWinWrite( win, WINDOW_TRANS_OFF, BF_TALKWIN_CGX_NUM, BF_TALKWIN_PAL );
	return;
}


