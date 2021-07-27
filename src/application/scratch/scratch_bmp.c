//==============================================================================================
/**
 * @file	scratch_bmp.c
 * @brief	「スクラッチ」BMP
 * @author	Satoshi Nohara
 * @date	07.12.11
 */
//==============================================================================================
#include "common.h"
#include "system/fontproc.h"
#include "system/pm_str.h"
#include "system/buflen.h"
#include "system/wordset.h"
#include "savedata/config.h"

#include "scratch_sys.h"
#include "scratch_bmp.h"


//==============================================================================================
//
//	定義
//
//==============================================================================================
//「下画面会話」
#define WIN_TALK_PX			(2)
#define WIN_TALK_PY			(1)
#define WIN_TALK_SX			(27)
#define WIN_TALK_SY			(4)
#define WIN_TALK_CGX		(1)

//「はい・いいえ」
#define WIN_YESNO_PX		(24)
#define WIN_YESNO_PY		(13)
#define WIN_YESNO_SX		(7)
#define WIN_YESNO_SY		(4)
#define WIN_YESNO_CGX		(WIN_TALK_CGX + WIN_TALK_SX * WIN_TALK_SY)

//「アイテム名1」
#define WIN_ITEM1_PX		(17) // MatchComment: 19 -> 17
#define WIN_ITEM1_PY		(3)
#define WIN_ITEM1_SX		(11)
#define WIN_ITEM1_SY		(5)
#define WIN_ITEM1_CGX		(WIN_YESNO_CGX + WIN_YESNO_SX * WIN_YESNO_SY)

//「アイテム名2」
#define WIN_ITEM2_PX		(17) // MatchComment: 19 -> 17
#define WIN_ITEM2_PY		(8)
#define WIN_ITEM2_SX		(11)
#define WIN_ITEM2_SY		(5)
#define WIN_ITEM2_CGX		(WIN_ITEM1_CGX + WIN_ITEM1_SX * WIN_ITEM1_SY)

//「アイテム名3」
#define WIN_ITEM3_PX		(17) // MatchComment: 19 -> 17
#define WIN_ITEM3_PY		(14)
#define WIN_ITEM3_SX		(11)
#define WIN_ITEM3_SY		(5)
#define WIN_ITEM3_CGX		(WIN_ITEM2_CGX + WIN_ITEM2_SX * WIN_ITEM2_SY)

//「アイテム名4」
#define WIN_ITEM4_PX		(17) // MatchComment: 19 -> 17
#define WIN_ITEM4_PY		(19)
#define WIN_ITEM4_SX		(11)
#define WIN_ITEM4_SY		(5)
#define WIN_ITEM4_CGX		(WIN_ITEM3_CGX + WIN_ITEM3_SX * WIN_ITEM3_SY)

//「ポケモンそろえる数1」
#define WIN_NUM1_PX			(10)
#define WIN_NUM1_PY			(3)
#define WIN_NUM1_SX			(5)
#define WIN_NUM1_SY			(3)
#define WIN_NUM1_CGX		(WIN_ITEM4_CGX + WIN_ITEM4_SX * WIN_ITEM4_SY)

//「ポケモンそろえる数2」
#define WIN_NUM2_PX			(10)
#define WIN_NUM2_PY			(8)
#define WIN_NUM2_SX			(5)
#define WIN_NUM2_SY			(3)
#define WIN_NUM2_CGX		(WIN_NUM1_CGX + WIN_NUM1_SX * WIN_NUM1_SY)

//「ポケモンそろえる数3」
#define WIN_NUM3_PX			(10)
#define WIN_NUM3_PY			(14)
#define WIN_NUM3_SX			(5)
#define WIN_NUM3_SY			(3)
#define WIN_NUM3_CGX		(WIN_NUM2_CGX + WIN_NUM2_SX * WIN_NUM2_SY)

//「ポケモンそろえる数4」
#define WIN_NUM4_PX			(10)
#define WIN_NUM4_PY			(19)
#define WIN_NUM4_SX			(5)
#define WIN_NUM4_SY			(3)
#define WIN_NUM4_CGX		(WIN_NUM3_CGX + WIN_NUM3_SX * WIN_NUM3_SY)

//「スクラッチスタート」
#define WIN_START_PX		(10)
#define WIN_START_PY		(9)
#define WIN_START_SX		(12)
#define WIN_START_SY		(4)
#define WIN_START_CGX		(WIN_NUM4_CGX + WIN_NUM4_SX * WIN_NUM4_SY)		//注意

//「３つそろった！」
#define WIN_ATARI_PX		(10)
#define WIN_ATARI_PY		(9)
#define WIN_ATARI_SX		(12)
#define WIN_ATARI_SY		(4)
#define WIN_ATARI_CGX		(WIN_NUM4_CGX + WIN_NUM4_SX * WIN_NUM4_SY)		//注意

//「ざんねん！」
#define WIN_HAZURE_PX		(10)
#define WIN_HAZURE_PY		(9)
#define WIN_HAZURE_SX		(12)
#define WIN_HAZURE_SY		(4)
#define WIN_HAZURE_CGX		(WIN_NUM4_CGX + WIN_NUM4_SX * WIN_NUM4_SY)		//注意

//「つぎのカードへ」
#define WIN_NEXT_PX			(10)
#define WIN_NEXT_PY			(21)
#define WIN_NEXT_SX			(13)
#define WIN_NEXT_SY			(3)
#define WIN_NEXT_CGX		(WIN_START_CGX + WIN_START_SX * WIN_START_SY)

//「やめる」
#define WIN_YAMERU_PX		(13)
#define WIN_YAMERU_PY		(21)
#define WIN_YAMERU_SX		(13)
#define WIN_YAMERU_SY		(3)
#define WIN_YAMERU_CGX		(WIN_START_CGX + WIN_START_SX * WIN_START_SY)	//注意

//「上画面会話」
#define WIN_SELECT_PX		(2)
#define WIN_SELECT_PY		(19)
#define WIN_SELECT_SX		(27)
#define WIN_SELECT_SY		(4)
//#define WIN_SELECT_CGX		(WIN_START_CGX + WIN_START_SX * WIN_START_SY)	//注意
#define WIN_SELECT_CGX		(1)	//注意

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
static const BMPWIN_DAT ScratchBmpData[] =
{
	{	// 0:「会話」
		SCRATCH_FRAME_D_MSG, WIN_TALK_PX, WIN_TALK_PY,
		WIN_TALK_SX, WIN_TALK_SY, BS_MSGFONT_PAL, WIN_TALK_CGX
	},
	{	// 1:「はい・いいえ」
		SCRATCH_FRAME_U_MSG, WIN_YESNO_PX, WIN_YESNO_PY,
		WIN_YESNO_SX, WIN_YESNO_SY, BS_FONT_PAL, WIN_YESNO_CGX
	},
	{	// 2:「アイテム名1」
		SCRATCH_FRAME_U_MSG, WIN_ITEM1_PX, WIN_ITEM1_PY,
		WIN_ITEM1_SX, WIN_ITEM1_SY, BS_FONT_PAL, WIN_ITEM1_CGX
	},	
	{	// 3:「アイテム名2」
		SCRATCH_FRAME_U_MSG, WIN_ITEM2_PX, WIN_ITEM2_PY,
		WIN_ITEM2_SX, WIN_ITEM2_SY, BS_FONT_PAL, WIN_ITEM2_CGX
	},	
	{	// 4:「アイテム名3」
		SCRATCH_FRAME_U_MSG, WIN_ITEM3_PX, WIN_ITEM3_PY,
		WIN_ITEM3_SX, WIN_ITEM3_SY, BS_FONT_PAL, WIN_ITEM3_CGX
	},	
	{	// 5:「アイテム名4」
		SCRATCH_FRAME_U_MSG, WIN_ITEM4_PX, WIN_ITEM4_PY,
		WIN_ITEM4_SX, WIN_ITEM4_SY, BS_FONT_PAL, WIN_ITEM4_CGX
	},	
	{	// 6:「アイテム数1」
		SCRATCH_FRAME_U_MSG, WIN_NUM1_PX, WIN_NUM1_PY,
		WIN_NUM1_SX, WIN_NUM1_SY, BS_FONT_PAL, WIN_NUM1_CGX
	},	
	{	// 7:「アイテム数2」
		SCRATCH_FRAME_U_MSG, WIN_NUM2_PX, WIN_NUM2_PY,
		WIN_NUM2_SX, WIN_NUM2_SY, BS_FONT_PAL, WIN_NUM2_CGX
	},	
	{	// 8:「アイテム数3」
		SCRATCH_FRAME_U_MSG, WIN_NUM3_PX, WIN_NUM3_PY,
		WIN_NUM3_SX, WIN_NUM3_SY, BS_FONT_PAL, WIN_NUM3_CGX
	},	
	{	// 9:「アイテム数4」
		SCRATCH_FRAME_U_MSG, WIN_NUM4_PX, WIN_NUM4_PY,
		WIN_NUM4_SX, WIN_NUM4_SY, BS_FONT_PAL, WIN_NUM4_CGX
	},	
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	{	//10:「スクラッチスタート」
		SCRATCH_FRAME_D_MSG, WIN_START_PX, WIN_START_PY,
		WIN_START_SX, WIN_START_SY, BS_FONT_PAL, WIN_START_CGX
	},	
	{	//11:「３つそろった！」
		SCRATCH_FRAME_D_MSG, WIN_ATARI_PX, WIN_ATARI_PY,
		WIN_ATARI_SX, WIN_ATARI_SY, BS_FONT_PAL, WIN_ATARI_CGX
	},	
	{	//12:「ざんねん」
		SCRATCH_FRAME_D_MSG, WIN_HAZURE_PX, WIN_HAZURE_PY,
		WIN_HAZURE_SX, WIN_HAZURE_SY, BS_FONT_PAL, WIN_HAZURE_CGX
	},	
	{	//13:「つぎのカードへ」
		SCRATCH_FRAME_D_MSG, WIN_NEXT_PX, WIN_NEXT_PY,
		WIN_NEXT_SX, WIN_NEXT_SY, BS_FONT_PAL, WIN_NEXT_CGX
	},	
	{	//14:「やめる」
		SCRATCH_FRAME_D_MSG, WIN_YAMERU_PX, WIN_YAMERU_PY,
		WIN_YAMERU_SX, WIN_YAMERU_SY, BS_FONT_PAL, WIN_YAMERU_CGX
	},	
	{	//15:「○まいめの　カードを　えらんでください」
		SCRATCH_FRAME_U_MSG, WIN_SELECT_PX, WIN_SELECT_PY,
		//WIN_SELECT_SX, WIN_SELECT_SY, BS_FONT_PAL, WIN_SELECT_CGX
		WIN_SELECT_SX, WIN_SELECT_SY, BS_MSGFONT_PAL, WIN_SELECT_CGX
	},	
};


//==============================================================================================
//
//	プロトタイプ宣言
//
//==============================================================================================
void ScratchAddBmpWin( GF_BGL_INI* bgl, GF_BGL_BMPWIN* win );
void ScratchExitBmpWin( GF_BGL_BMPWIN* win );
void ScratchOffBmpWin( GF_BGL_BMPWIN* win );
void ScratchWriteMenuWin( GF_BGL_INI* bgl, GF_BGL_BMPWIN* win );
void ScratchTalkWinPut( GF_BGL_BMPWIN * win, WINTYPE wintype );

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
void ScratchAddBmpWin( GF_BGL_INI* bgl, GF_BGL_BMPWIN* win )
{
	u8 i;
	const BMPWIN_DAT* dat = ScratchBmpData;

	//ビットマップ追加
	for( i=0; i < SCRATCH_BMPWIN_MAX; i++ ){
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
void ScratchExitBmpWin( GF_BGL_BMPWIN* win )
{
	u16	i;

	for( i=0; i < SCRATCH_BMPWIN_MAX; i++ ){
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
void ScratchOffBmpWin( GF_BGL_BMPWIN* win )
{
	u16	i;

	for( i=0; i < SCRATCH_BMPWIN_MAX; i++ ){
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
void ScratchWriteMenuWin( GF_BGL_INI* bgl, GF_BGL_BMPWIN* win )
{
	//メニューウィンドウのグラフィックをセット
	MenuWinGraphicSet( bgl, SCRATCH_FRAME_D_MSG, BS_MENU_CGX_NUM, BS_MENU_PAL, 0, HEAPID_SCRATCH );

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
void ScratchTalkWinPut( GF_BGL_BMPWIN * win, WINTYPE wintype )
{
	TalkWinGraphicSet( win->ini, GF_BGL_BmpWinGet_Frame(win), BS_TALKWIN_CGX_NUM,
						BS_TALKWIN_PAL, wintype, HEAPID_SCRATCH );

	GF_BGL_BmpWinDataFill( win, 15 );
	BmpTalkWinWrite( win, WINDOW_TRANS_ON, BS_TALKWIN_CGX_NUM, BS_TALKWIN_PAL );
	return;
}


