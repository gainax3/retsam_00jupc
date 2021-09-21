//==============================================================================================
/**
 * @file	roulette_bmp.c
 * @brief	「バトルルーレット」BMP
 * @author	Satoshi Nohara
 * @date	07.09.05
 */
//==============================================================================================
#include "common.h"
#include "system/fontproc.h"
#include "system/pm_str.h"
#include "system/buflen.h"
#include "system/wordset.h"
#include "savedata/config.h"

#include "roulette_sys.h"
#include "roulette_bmp.h"


//==============================================================================================
//
//	定義
//
//==============================================================================================
//メニューウィンドウキャラ
#define BR_MENU_CGX_NUM		(1024-MENU_WIN_CGX_SIZ)

//会話ウィンドウキャラ
#define	BR_TALKWIN_CGX_SIZE	(TALK_WIN_CGX_SIZ)
#define	BR_TALKWIN_CGX_NUM	(BR_MENU_CGX_NUM - BR_TALKWIN_CGX_SIZE)


//==============================================================================================
//
//	ランクアップ画面データ
//
//==============================================================================================
//「会話」
#define RANK_WIN_TALK_PX			(2)
#define RANK_WIN_TALK_PY			(19)
#define RANK_WIN_TALK_SX			(27)
#define RANK_WIN_TALK_SY			(4)
#define RANK_WIN_TALK_CGX			(1)

//「はい・いいえ」
#define RANK_WIN_YESNO_PX			(24)
#define RANK_WIN_YESNO_PY			(13)
#define RANK_WIN_YESNO_SX			(7)
#define RANK_WIN_YESNO_SY			(4)
#define RANK_WIN_YESNO_CGX			(RANK_WIN_TALK_CGX + RANK_WIN_TALK_SX * RANK_WIN_TALK_SY)

static const BMPWIN_DAT RouletteBmpData[] =
{
	{	// 0:「会話」
		BR_FRAME_WIN, RANK_WIN_TALK_PX, RANK_WIN_TALK_PY,
		RANK_WIN_TALK_SX, RANK_WIN_TALK_SY, BR_MSGFONT_PAL, RANK_WIN_TALK_CGX
	},
	{	// 1:「はい・いいえ」
		BR_FRAME_WIN, RANK_WIN_YESNO_PX, RANK_WIN_YESNO_PY,
		RANK_WIN_YESNO_SX, RANK_WIN_YESNO_SY, BR_FONT_PAL, RANK_WIN_YESNO_CGX
	},
};


//==============================================================================================
//
//	プロトタイプ宣言
//
//==============================================================================================
void RouletteAddBmpWin( GF_BGL_INI* bgl, GF_BGL_BMPWIN* win );
void RouletteExitBmpWin( GF_BGL_BMPWIN* win );
void RouletteOffBmpWin( GF_BGL_BMPWIN* win );
void RouletteWriteMenuWin( GF_BGL_INI* bgl, GF_BGL_BMPWIN* win );
void RouletteTalkWinPut( GF_BGL_BMPWIN * win, WINTYPE wintype );

//はい、いいえウィンドウ定義
#define YESNO_PX		(23)			//開始Ｘ
#define YESNO_PY		(19)			//開始Ｙ
#define YESNO_SX		(8)				//幅Ｘ
#define YESNO_SY		(4)				//幅Ｙ

//会話ウィンドウ定義
#define TALK_PX			(2)				//開始Ｘ
#define TALK_PY			(19)			//開始Ｙ
#define TALK_SX			(17)			//幅Ｘ
#define TALK_SY			(4)				//幅Ｙ


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
void RouletteAddBmpWin( GF_BGL_INI* bgl, GF_BGL_BMPWIN* win )
{
	u8 i;
	const BMPWIN_DAT* dat = RouletteBmpData;

	//ビットマップ追加
	for( i=0; i < ROULETTE_BMPWIN_MAX; i++ ){
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
void RouletteExitBmpWin( GF_BGL_BMPWIN* win )
{
	u16	i;

	for( i=0; i < ROULETTE_BMPWIN_MAX; i++ ){
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
void RouletteOffBmpWin( GF_BGL_BMPWIN* win )
{
	u16	i;

	for( i=0; i < ROULETTE_BMPWIN_MAX; i++ ){
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
void RouletteWriteMenuWin( GF_BGL_INI* bgl, GF_BGL_BMPWIN* win )
{
	//メニューウィンドウのグラフィックをセット
	//MenuWinGraphicSet( bgl, BR_FRAME_WIN, BR_MENU_CGX_NUM, BR_MENU_PAL, 0, HEAPID_ROULETTE );
	MenuWinGraphicSet(	bgl, GF_BGL_BmpWinGet_Frame(win), 
						BR_MENU_CGX_NUM, BR_MENU_PAL, 0, HEAPID_ROULETTE );

	//メニューウィンドウを描画
	BmpMenuWinWrite( win, WINDOW_TRANS_OFF, BR_MENU_CGX_NUM, BR_MENU_PAL );
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
void RouletteTalkWinPut( GF_BGL_BMPWIN * win, WINTYPE wintype )
{
	TalkWinGraphicSet( win->ini, GF_BGL_BmpWinGet_Frame(win), BR_TALKWIN_CGX_NUM,
						BR_TALKWIN_PAL, wintype, HEAPID_ROULETTE );

	GF_BGL_BmpWinDataFill( win, 15 );
	BmpTalkWinWrite( win, WINDOW_TRANS_ON, BR_TALKWIN_CGX_NUM, BR_TALKWIN_PAL );
	return;
}


