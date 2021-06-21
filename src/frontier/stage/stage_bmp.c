//==============================================================================================
/**
 * @file	stage_bmp.c
 * @brief	「バトルステージ」BMP
 * @author	Satoshi Nohara
 * @date	07.06.08
 */
//==============================================================================================
#include "common.h"
#include "system/fontproc.h"
#include "system/pm_str.h"
#include "system/buflen.h"
#include "system/wordset.h"
#include "savedata/config.h"

#include "stage_sys.h"
#include "stage_bmp.h"


//==============================================================================================
//
//	定義
//
//==============================================================================================
//「会話」
#define WIN_TALK_PX			(2)
#define WIN_TALK_PY			(19)
#define WIN_TALK_SX			(27)
#define WIN_TALK_SY			(4)
#define WIN_TALK_CGX		(1)

//「はい・いいえ」
#define WIN_YESNO_PX		(24)
#define WIN_YESNO_PY		(13)
#define WIN_YESNO_SX		(7)
#define WIN_YESNO_SY		(4)
#define WIN_YESNO_CGX		(WIN_TALK_CGX + WIN_TALK_SX * WIN_TALK_SY)

//「選択したポケモン名1」
#define WIN_POKE1_PX		(10)
#define WIN_POKE1_PY		(20)//(21)
#define WIN_POKE1_SX		(9)
#define WIN_POKE1_SY		(2)
#define WIN_POKE1_CGX		(WIN_YESNO_CGX + WIN_YESNO_SX * WIN_YESNO_SY)

//「タイプ名(とりあえずきれいに収まらないので画面全体を確保)」(フレームを変えている)
#define WIN_TYPE_PX			(1)
#define WIN_TYPE_PY			(0)
#define WIN_TYPE_SX			(30)
#define WIN_TYPE_SY			(22)
#if 0
#define WIN_TYPE_CGX		(WIN_POKE1_CGX + WIN_POKE1_SX * WIN_POKE1_SY)
#else
#define WIN_TYPE_CGX		(1)
#endif

//メニューウィンドウキャラ
#define BS_MENU_CGX_NUM		(1024-MENU_WIN_CGX_SIZ)

//会話ウィンドウキャラ
#define	BS_TALKWIN_CGX_SIZE	(TALK_WIN_CGX_SIZ)
#define	BS_TALKWIN_CGX_NUM	(BS_MENU_CGX_NUM - BS_TALKWIN_CGX_SIZE)


//==============================================================================================
//
//	データ
//
//==============================================================================================
static const BMPWIN_DAT StageBmpData[] =
{
	{	// 0:「会話」
		BS_FRAME_WIN, WIN_TALK_PX, WIN_TALK_PY,
		WIN_TALK_SX, WIN_TALK_SY, BS_MSGFONT_PAL, WIN_TALK_CGX
	},
	{	// 1:「はい・いいえ」
		BS_FRAME_WIN, WIN_YESNO_PX, WIN_YESNO_PY,
		WIN_YESNO_SX, WIN_YESNO_SY, BS_FONT_PAL, WIN_YESNO_CGX
	},
	{	// 2:「ポケモン名1」
		BS_FRAME_WIN, WIN_POKE1_PX, WIN_POKE1_PY,
		WIN_POKE1_SX, WIN_POKE1_SY, BS_FONT_PAL, WIN_POKE1_CGX
	},	
	{	// 3:「タイプ名」
		BS_FRAME_TYPE, WIN_TYPE_PX, WIN_TYPE_PY,					//フレームが違うので注意！
		WIN_TYPE_SX, WIN_TYPE_SY, BS_FONT_PAL, WIN_TYPE_CGX
	},	
};


//==============================================================================================
//
//	プロトタイプ宣言
//
//==============================================================================================
void StageAddBmpWin( GF_BGL_INI* bgl, GF_BGL_BMPWIN* win );
void StageExitBmpWin( GF_BGL_BMPWIN* win );
void StageOffBmpWin( GF_BGL_BMPWIN* win );
void StageWriteMenuWin( GF_BGL_INI* bgl, GF_BGL_BMPWIN* win );
void StageTalkWinPut( GF_BGL_BMPWIN * win, WINTYPE wintype );

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
void StageAddBmpWin( GF_BGL_INI* bgl, GF_BGL_BMPWIN* win )
{
	u8 i;
	const BMPWIN_DAT* dat = StageBmpData;

	//ビットマップ追加
	for( i=0; i < STAGE_BMPWIN_MAX; i++ ){
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
void StageExitBmpWin( GF_BGL_BMPWIN* win )
{
	u16	i;

	for( i=0; i < STAGE_BMPWIN_MAX; i++ ){
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
void StageOffBmpWin( GF_BGL_BMPWIN* win )
{
	u16	i;

	for( i=0; i < STAGE_BMPWIN_MAX; i++ ){
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
void StageWriteMenuWin( GF_BGL_INI* bgl, GF_BGL_BMPWIN* win )
{
	//メニューウィンドウのグラフィックをセット
	MenuWinGraphicSet( bgl, BS_FRAME_WIN, BS_MENU_CGX_NUM, BS_MENU_PAL, 0, HEAPID_STAGE );

	//メニューウィンドウを描画
	BmpMenuWinWrite( win, WINDOW_TRANS_OFF, BS_MENU_CGX_NUM, BS_MENU_PAL );
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
void StageTalkWinPut( GF_BGL_BMPWIN * win, WINTYPE wintype )
{
	TalkWinGraphicSet( win->ini, GF_BGL_BmpWinGet_Frame(win), BS_TALKWIN_CGX_NUM,
						BS_TALKWIN_PAL, wintype, HEAPID_STAGE );

	GF_BGL_BmpWinDataFill( win, 15 );
	BmpTalkWinWrite( win, WINDOW_TRANS_ON, BS_TALKWIN_CGX_NUM, BS_TALKWIN_PAL );
	return;
}


