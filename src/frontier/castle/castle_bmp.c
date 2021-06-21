//==============================================================================================
/**
 * @file	castle_bmp.c
 * @brief	「バトルキャッスル」BMP
 * @author	Satoshi Nohara
 * @date	07.07.05
 */
//==============================================================================================
#include "common.h"
#include "system/fontproc.h"
#include "system/pm_str.h"
#include "system/buflen.h"
#include "system/wordset.h"
#include "savedata/config.h"

#include "castle_sys.h"
#include "castle_bmp.h"


//==============================================================================================
//
//	定義
//
//==============================================================================================
//メニューウィンドウキャラ
#define BC_MENU_CGX_NUM		(1024-MENU_WIN_CGX_SIZ)

//会話ウィンドウキャラ
#define	BC_TALKWIN_CGX_SIZE	(TALK_WIN_CGX_SIZ)
#define	BC_TALKWIN_CGX_NUM	(BC_MENU_CGX_NUM - BC_TALKWIN_CGX_SIZE)


//==============================================================================================
//
//	手持ちポケモン画面データ
//
//==============================================================================================
//「トレーナー名1,2」
#define MINE_WIN_TR1_PX			(2)
#define MINE_WIN_TR1_PY			(1)
#define MINE_WIN_TR1_SX			(29)
#define MINE_WIN_TR1_SY			(2)
#define MINE_WIN_TR1_CGX		(1)

//「もどる」
#define MINE_WIN_MODORU_PX		(26)
#define MINE_WIN_MODORU_PY		(19)
#define MINE_WIN_MODORU_SX		(4)
#define MINE_WIN_MODORU_SY		(3)
#define MINE_WIN_MODORU_CGX		(MINE_WIN_TR1_CGX + MINE_WIN_TR1_SX * MINE_WIN_TR1_SY)

//「LV99」
#define MINE_WIN_LV_PX			(0)
#define MINE_WIN_LV_PY			(4)
#define MINE_WIN_LV_SX			(32)
#define MINE_WIN_LV_SY			(2)
#define MINE_WIN_LV_CGX			(MINE_WIN_MODORU_CGX + MINE_WIN_MODORU_SX * MINE_WIN_MODORU_SY)

//「HP」
#define MINE_WIN_HP_PX			(0)
#define MINE_WIN_HP_PY			(9)
#define MINE_WIN_HP_SX			(32)
#define MINE_WIN_HP_SY			(2)
#define MINE_WIN_HP_CGX			(MINE_WIN_LV_CGX + MINE_WIN_LV_SX * MINE_WIN_LV_SY)

//「LV99」(フレームを変えている)
#define MINE_WIN_ITEM_LV_PX		(1)
#define MINE_WIN_ITEM_LV_PY		(7)
#define MINE_WIN_ITEM_LV_SX		(10)
#define MINE_WIN_ITEM_LV_SY		(2)
#define MINE_WIN_ITEM_LV_CGX	(MINE_WIN_CP_CGX + MINE_WIN_CP_SX * MINE_WIN_CP_SY)

//「HP」(フレームを変えている)
#define MINE_WIN_ITEM_HP_PX		(1)
#define MINE_WIN_ITEM_HP_PY		(13)
#define MINE_WIN_ITEM_HP_SX		(11)
#define MINE_WIN_ITEM_HP_SY		(2)
#define MINE_WIN_ITEM_HP_CGX	(MINE_WIN_ITEM_LV_CGX + MINE_WIN_ITEM_LV_SX * MINE_WIN_ITEM_LV_SY)

/////////////////////////////////////
//「つよさ・わざ」(★フレームを変えている)
#define MINE_WIN_STATUS_PX		(5)
#define MINE_WIN_STATUS_PY		(10)
#define MINE_WIN_STATUS_SX		(24)
#define MINE_WIN_STATUS_SY		(14)
#define MINE_WIN_STATUS_CGX		(1)			//注意

//「アイテムリスト」(★フレームを変えている)
#define MINE_WIN_LIST_PX		(12)
#define MINE_WIN_LIST_PY		(2)
#define MINE_WIN_LIST_SX		(19)
//#define MINE_WIN_LIST_SY		(14)
#define MINE_WIN_LIST_SY		(12)
#define MINE_WIN_LIST_CGX		(1)			//注意

//「会話」(フレームを変えている)
#define MINE_WIN_TALK_PX		(2)
#define MINE_WIN_TALK_PY		(19)
#define MINE_WIN_TALK_SX		(27)
#define MINE_WIN_TALK_SY		(4)
#define MINE_WIN_TALK_CGX		(MINE_WIN_STATUS_CGX + MINE_WIN_STATUS_SX * MINE_WIN_STATUS_SY)

//「会話(メニューウィンドウ)」(フレームを変えている)(★「会話」内に収まるようにする)
#define MINE_WIN_TALKMENU_PX	(2)
#define MINE_WIN_TALKMENU_PY	(19)
#define MINE_WIN_TALKMENU_SX	(20)
#define MINE_WIN_TALKMENU_SY	(4)
#define MINE_WIN_TALKMENU_CGX	(MINE_WIN_TALK_CGX + MINE_WIN_TALK_SX *MINE_WIN_TALK_SY)

//「会話(メニューウィンドウ2)」(フレームを変えている)(★「会話」内に収まるようにする)
#define MINE_WIN_TALKMENU2_PX	(2)
#define MINE_WIN_TALKMENU2_PY	(19)
#define MINE_WIN_TALKMENU2_SX	(17)
#define MINE_WIN_TALKMENU2_SY	(4)
#define MINE_WIN_TALKMENU2_CGX	(MINE_WIN_TALKMENU_CGX + MINE_WIN_TALKMENU_SX *MINE_WIN_TALKMENU_SY)

//「基本リスト1」(フレームを変えている)(★「リスト」内に収まるようにする」)
//かいふく、レンタル、つよさ、わざ、とじる
#define MINE_WIN_BASIC_LIST_PX	(23)
#define MINE_WIN_BASIC_LIST_PY	(13)
#define MINE_WIN_BASIC_LIST_SX	(8)
#define MINE_WIN_BASIC_LIST_SY	(10)
#define MINE_WIN_BASIC_LIST_CGX	(MINE_WIN_TALKMENU2_CGX + MINE_WIN_TALKMENU2_SX * MINE_WIN_TALKMENU2_SY)

//「回復リスト」(★フレームを変えている)
//ＨＰかいふく、ＰＰかいふく、すべてかいふく、ランクアップ、やめる
#define MINE_WIN_KAIHUKU_LIST_PX	(20)
#define MINE_WIN_KAIHUKU_LIST_PY	(7)
#define MINE_WIN_KAIHUKU_LIST_SX	(11)
#define MINE_WIN_KAIHUKU_LIST_SY	(10)
#define MINE_WIN_KAIHUKU_LIST_CGX	(MINE_WIN_BASIC_LIST_CGX + MINE_WIN_BASIC_LIST_SX * MINE_WIN_BASIC_LIST_SY)

//「レンタルリスト」(フレームを変えている)(★「リスト」内に収まるようにする」)
//きのみ、どうぐ、ランクアップ、やめる
#define MINE_WIN_RENTAL_LIST_PX		(22)
#define MINE_WIN_RENTAL_LIST_PY		(9)
#define MINE_WIN_RENTAL_LIST_SX		(9)
#define MINE_WIN_RENTAL_LIST_SY		(8)
#define MINE_WIN_RENTAL_LIST_CGX	(MINE_WIN_BASIC_LIST_CGX + MINE_WIN_BASIC_LIST_SX * MINE_WIN_BASIC_LIST_SY)

//「道具情報」(フレームを変えている)
#define MINE_WIN_ITEMINFO_PX	(7)
#define MINE_WIN_ITEMINFO_PY	(17)
#define MINE_WIN_ITEMINFO_SX	(23)
#define MINE_WIN_ITEMINFO_SY	(6)
#define MINE_WIN_ITEMINFO_CGX	(MINE_WIN_TALK_CGX + MINE_WIN_TALK_SX *MINE_WIN_TALK_SY)

//「はい・いいえ」(フレームを変えている)
#define MINE_WIN_YESNO_PX		(24)
#define MINE_WIN_YESNO_PY		(13)
#define MINE_WIN_YESNO_SX		(7)
#define MINE_WIN_YESNO_SY		(4)
#define MINE_WIN_YESNO_CGX		(MINE_WIN_ITEMINFO_CGX + MINE_WIN_ITEMINFO_SX *MINE_WIN_ITEMINFO_SY)

//「レベル＋５・レベルー５・やめる」(フレームを変えている)
#define MINE_WIN_SEL_PX			(24)
#define MINE_WIN_SEL_PY			(11)
#define MINE_WIN_SEL_SX			(7)
#define MINE_WIN_SEL_SY			(6)
#define MINE_WIN_SEL_CGX		(MINE_WIN_YESNO_CGX + MINE_WIN_YESNO_SX *MINE_WIN_YESNO_SY)

//「名前＋ＣＰウィンドウ」(フレームを変えている)
#define MINE_WIN_CP_PX			(1)
#define MINE_WIN_CP_PY			(1)
#define MINE_WIN_CP_SX			(8)//(10)
#define MINE_WIN_CP_SY			(4)
#define MINE_WIN_CP_CGX			(MINE_WIN_SEL_CGX + MINE_WIN_SEL_SX *MINE_WIN_SEL_SY)

static const BMPWIN_DAT CastleMineBmpData[] =
{
	{	// 0:「トレーナー名1、2」
		BC_FRAME_WIN, MINE_WIN_TR1_PX, MINE_WIN_TR1_PY,
		MINE_WIN_TR1_SX, MINE_WIN_TR1_SY, BC_FONT_PAL, MINE_WIN_TR1_CGX
	},	
	{	// 1:「もどる」
		BC_FRAME_WIN, MINE_WIN_MODORU_PX, MINE_WIN_MODORU_PY,
		MINE_WIN_MODORU_SX, MINE_WIN_MODORU_SY, BC_FONT_PAL, MINE_WIN_MODORU_CGX
	},	
	{	// 3:「LV99」
		BC_FRAME_WIN, MINE_WIN_LV_PX, MINE_WIN_LV_PY,
		MINE_WIN_LV_SX, MINE_WIN_LV_SY, BC_FONT_PAL, MINE_WIN_LV_CGX
	},	
	{	// 4:「999/999」
		BC_FRAME_WIN, MINE_WIN_HP_PX, MINE_WIN_HP_PY,
		MINE_WIN_HP_SX, MINE_WIN_HP_SY, BC_FONT_PAL, MINE_WIN_HP_CGX
	},	
	//////////////////////
	{	// 5:「つよさ・わざ」
		BC_FRAME_TYPE, MINE_WIN_STATUS_PX, MINE_WIN_STATUS_PY,			//フレームが違うので注意！
		MINE_WIN_STATUS_SX, MINE_WIN_STATUS_SY, BC_FONT_PAL, MINE_WIN_STATUS_CGX
	},	
	{	// 6:「リスト」
		BC_FRAME_TYPE, MINE_WIN_LIST_PX, MINE_WIN_LIST_PY,				//フレームが違うので注意！
		MINE_WIN_LIST_SX, MINE_WIN_LIST_SY, BC_FONT_PAL, MINE_WIN_LIST_CGX
	},
	{	// 10:「会話」
		BC_FRAME_TYPE, MINE_WIN_TALK_PX, MINE_WIN_TALK_PY,				//フレームが違うので注意！
		MINE_WIN_TALK_SX, MINE_WIN_TALK_SY, BC_MSGFONT_PAL, MINE_WIN_TALK_CGX
	},
	{	// 11:「会話(メニューウィンドウ)」
		BC_FRAME_TYPE, MINE_WIN_TALKMENU_PX, MINE_WIN_TALKMENU_PY,		//フレームが違うので注意！
		MINE_WIN_TALKMENU_SX, MINE_WIN_TALKMENU_SY, BC_MSGFONT_PAL, MINE_WIN_TALKMENU_CGX
	},
	{	// 12:「会話(メニューウィンドウ2)」
		BC_FRAME_TYPE, MINE_WIN_TALKMENU2_PX, MINE_WIN_TALKMENU2_PY,	//フレームが違うので注意！
		MINE_WIN_TALKMENU2_SX, MINE_WIN_TALKMENU2_SY, BC_MSGFONT_PAL, MINE_WIN_TALKMENU2_CGX
	},
	{	// 9:「基本リスト1」
		BC_FRAME_TYPE, MINE_WIN_BASIC_LIST_PX, MINE_WIN_BASIC_LIST_PY,//フレームが違うので注意！
		MINE_WIN_BASIC_LIST_SX, MINE_WIN_BASIC_LIST_SY, BC_FONT_PAL, MINE_WIN_BASIC_LIST_CGX
	},
	{	// 7:「回復リスト」
		BC_FRAME_TYPE, MINE_WIN_KAIHUKU_LIST_PX, MINE_WIN_KAIHUKU_LIST_PY,//フレームが違うので注意！
		MINE_WIN_KAIHUKU_LIST_SX, MINE_WIN_KAIHUKU_LIST_SY, BC_FONT_PAL, MINE_WIN_KAIHUKU_LIST_CGX
	},
	{	// 8:「レンタルリスト」
		BC_FRAME_TYPE, MINE_WIN_RENTAL_LIST_PX, MINE_WIN_RENTAL_LIST_PY,//フレームが違うので注意！
		MINE_WIN_RENTAL_LIST_SX, MINE_WIN_RENTAL_LIST_SY, BC_FONT_PAL, MINE_WIN_RENTAL_LIST_CGX
	},
	{	// 13:「道具情報」
		BC_FRAME_TYPE, MINE_WIN_ITEMINFO_PX, MINE_WIN_ITEMINFO_PY,		//フレームが違うので注意！
		MINE_WIN_ITEMINFO_SX, MINE_WIN_ITEMINFO_SY, BC_MSGFONT_PAL, MINE_WIN_ITEMINFO_CGX
	},
	{	// 14:「はい・いいえ」
		BC_FRAME_TYPE, MINE_WIN_YESNO_PX, MINE_WIN_YESNO_PY,			//フレームが違うので注意！
		MINE_WIN_YESNO_SX, MINE_WIN_YESNO_SY, BC_FONT_PAL, MINE_WIN_YESNO_CGX
	},
	{	// 15:「レベル＋５・レベルー５・やめる」
		BC_FRAME_TYPE, MINE_WIN_SEL_PX, MINE_WIN_SEL_PY,				//フレームが違うので注意！
		MINE_WIN_SEL_SX, MINE_WIN_SEL_SY, BC_FONT_PAL, MINE_WIN_SEL_CGX
	},
	{	// 16:「名前＋CPウィンドウ」
		BC_FRAME_TYPE, MINE_WIN_CP_PX, MINE_WIN_CP_PY,					//フレームが違うので注意！
		MINE_WIN_CP_SX, MINE_WIN_CP_SY, BC_FONT_PAL, MINE_WIN_CP_CGX
	},
	{	// 17:「LV99」
		BC_FRAME_TYPE, MINE_WIN_ITEM_LV_PX, MINE_WIN_ITEM_LV_PY,
		MINE_WIN_ITEM_LV_SX, MINE_WIN_ITEM_LV_SY, BC_FONT_PAL, MINE_WIN_ITEM_LV_CGX
	},	
	{	// 18:「999/999」
		BC_FRAME_TYPE, MINE_WIN_ITEM_HP_PX, MINE_WIN_ITEM_HP_PY,
		MINE_WIN_ITEM_HP_SX, MINE_WIN_ITEM_HP_SY, BC_FONT_PAL, MINE_WIN_ITEM_HP_CGX
	},	

};


//==============================================================================================
//
//	敵トレーナー画面データ
//
//==============================================================================================
//「トレーナー名1,2」
#define ENEMY_WIN_TR1_PX		(2)
#define ENEMY_WIN_TR1_PY		(1)
#define ENEMY_WIN_TR1_SX		(29)
#define ENEMY_WIN_TR1_SY		(2)
#define ENEMY_WIN_TR1_CGX		(1)

//「もどる」
#define ENEMY_WIN_MODORU_PX		(26)
#define ENEMY_WIN_MODORU_PY		(19)
#define ENEMY_WIN_MODORU_SX		(4)
#define ENEMY_WIN_MODORU_SY		(3)
#define ENEMY_WIN_MODORU_CGX	(ENEMY_WIN_TR1_CGX + ENEMY_WIN_TR1_SX * ENEMY_WIN_TR1_SY)

//「LV99」
#define ENEMY_WIN_LV_PX			(0)
#define ENEMY_WIN_LV_PY			(4)
#define ENEMY_WIN_LV_SX			(32)
#define ENEMY_WIN_LV_SY			(2)
#define ENEMY_WIN_LV_CGX		(ENEMY_WIN_MODORU_CGX + ENEMY_WIN_MODORU_SX * ENEMY_WIN_MODORU_SY)

//「HP」
#define ENEMY_WIN_HP_PX			(0)
#define ENEMY_WIN_HP_PY			(9)
#define ENEMY_WIN_HP_SX			(32)
#define ENEMY_WIN_HP_SY			(2)
#define ENEMY_WIN_HP_CGX		(ENEMY_WIN_LV_CGX + ENEMY_WIN_LV_SX * ENEMY_WIN_LV_SY)

/////////////////////////////////////////////////////////////////////////////////////////////
//「つよさ・わざ」(フレームを変えている)
#define ENEMY_WIN_STATUS_PX		(4)
#define ENEMY_WIN_STATUS_PY		(10)
#define ENEMY_WIN_STATUS_SX		(26)
#define ENEMY_WIN_STATUS_SY		(14)
#define ENEMY_WIN_STATUS_CGX	(1)

//「基本リスト1」(フレームを変えている)(★「つよさ・わざ」内に収まるようにする」)
//しらべる、レベル、じょうほう、とじる
#define ENEMY_WIN_BASIC_LIST_PX		(23)
#define ENEMY_WIN_BASIC_LIST_PY		(15)
#define ENEMY_WIN_BASIC_LIST_SX		(8)
#define ENEMY_WIN_BASIC_LIST_SY		(8)
#define ENEMY_WIN_BASIC_LIST_CGX	(1)		//注意

//「基本リスト2」(フレームを変えている)(★「つよさ・わざ」内に収まるようにする)
//つよさ、わざ、ランクアップ、やめる
#define ENEMY_WIN_BASIC2_LIST_PX	(22)
#define ENEMY_WIN_BASIC2_LIST_PY	(9)
#define ENEMY_WIN_BASIC2_LIST_SX	(9)
#define ENEMY_WIN_BASIC2_LIST_SY	(8)
#define ENEMY_WIN_BASIC2_LIST_CGX	(ENEMY_WIN_STATUS_CGX+ENEMY_WIN_STATUS_SX*ENEMY_WIN_STATUS_SY)

//「会話」(フレームを変えている)
#define ENEMY_WIN_TALK_PX		(2)
#define ENEMY_WIN_TALK_PY		(19)
#define ENEMY_WIN_TALK_SX		(27)
#define ENEMY_WIN_TALK_SY		(4)
#define ENEMY_WIN_TALK_CGX	(ENEMY_WIN_BASIC2_LIST_CGX + ENEMY_WIN_BASIC2_LIST_SX * ENEMY_WIN_BASIC2_LIST_SY)

//「会話(メニューウィンドウ)」(フレームを変えている)(★「会話」内に収まるようにする)
#define ENEMY_WIN_TALKMENU_PX	(2)
#define ENEMY_WIN_TALKMENU_PY	(19)
#define ENEMY_WIN_TALKMENU_SX	(20)
#define ENEMY_WIN_TALKMENU_SY	(4)
#define ENEMY_WIN_TALKMENU_CGX	(ENEMY_WIN_TALK_CGX + ENEMY_WIN_TALK_SX *ENEMY_WIN_TALK_SY)

//「会話(メニューウィンドウ2)」(フレームを変えている)(★「会話」内に収まるようにする)
#define ENEMY_WIN_TALKMENU2_PX	(2)
#define ENEMY_WIN_TALKMENU2_PY	(19)
#define ENEMY_WIN_TALKMENU2_SX	(17)
#define ENEMY_WIN_TALKMENU2_SY	(4)
#define ENEMY_WIN_TALKMENU2_CGX	(ENEMY_WIN_TALKMENU_CGX+ENEMY_WIN_TALKMENU_SX*ENEMY_WIN_TALKMENU_SY)

//「はい・いいえ」(フレームを変えている)
#define ENEMY_WIN_YESNO_PX		(24)
#define ENEMY_WIN_YESNO_PY		(13)
#define ENEMY_WIN_YESNO_SX		(7)
#define ENEMY_WIN_YESNO_SY		(4)
#define ENEMY_WIN_YESNO_CGX	(ENEMY_WIN_TALKMENU2_CGX+ENEMY_WIN_TALKMENU2_SX*ENEMY_WIN_TALKMENU2_SY)

//「レベル＋５・レベルー５・やめる」(フレームを変えている)
#define ENEMY_WIN_SEL_PX		(24)
#define ENEMY_WIN_SEL_PY		(11)
#define ENEMY_WIN_SEL_SX		(7)
#define ENEMY_WIN_SEL_SY		(6)
#define ENEMY_WIN_SEL_CGX		(ENEMY_WIN_YESNO_CGX + ENEMY_WIN_YESNO_SX * ENEMY_WIN_YESNO_SY)

static const BMPWIN_DAT CastleEnemyBmpData[] =
{
	{	// 0:「トレーナー名1、2」
		BC_FRAME_WIN, ENEMY_WIN_TR1_PX, ENEMY_WIN_TR1_PY,
		ENEMY_WIN_TR1_SX, ENEMY_WIN_TR1_SY, BC_FONT_PAL, ENEMY_WIN_TR1_CGX
	},	
	{	// 1:「もどる」
		BC_FRAME_WIN, ENEMY_WIN_MODORU_PX, ENEMY_WIN_MODORU_PY,
		ENEMY_WIN_MODORU_SX, ENEMY_WIN_MODORU_SY, BC_FONT_PAL, ENEMY_WIN_MODORU_CGX
	},	
	{	// 2:「LV99」
		BC_FRAME_WIN, ENEMY_WIN_LV_PX, ENEMY_WIN_LV_PY,
		ENEMY_WIN_LV_SX, ENEMY_WIN_LV_SY, BC_FONT_PAL, ENEMY_WIN_LV_CGX
	},	
	{	// 3:「999/999」
		BC_FRAME_WIN, ENEMY_WIN_HP_PX, ENEMY_WIN_HP_PY,
		ENEMY_WIN_HP_SX, ENEMY_WIN_HP_SY, BC_FONT_PAL, ENEMY_WIN_HP_CGX
	},	
	{	// 4:「つよさ・わざ」
		BC_FRAME_TYPE, ENEMY_WIN_STATUS_PX, ENEMY_WIN_STATUS_PY,		//フレームが違うので注意！
		ENEMY_WIN_STATUS_SX, ENEMY_WIN_STATUS_SY, BC_FONT_PAL, ENEMY_WIN_STATUS_CGX
	},	
	{	// 5:「基本リスト1」
		BC_FRAME_TYPE, ENEMY_WIN_BASIC_LIST_PX, ENEMY_WIN_BASIC_LIST_PY,//フレームが違うので注意！
		ENEMY_WIN_BASIC_LIST_SX, ENEMY_WIN_BASIC_LIST_SY, BC_FONT_PAL, ENEMY_WIN_BASIC_LIST_CGX
	},
	{	// 6:「基本リスト2」
		BC_FRAME_TYPE, ENEMY_WIN_BASIC2_LIST_PX, ENEMY_WIN_BASIC2_LIST_PY,//フレームが違うので注意！
		ENEMY_WIN_BASIC2_LIST_SX, ENEMY_WIN_BASIC2_LIST_SY, BC_FONT_PAL, ENEMY_WIN_BASIC2_LIST_CGX
	},
	{	// 7:「会話」
		BC_FRAME_TYPE, ENEMY_WIN_TALK_PX, ENEMY_WIN_TALK_PY,			//フレームが違うので注意！
		ENEMY_WIN_TALK_SX, ENEMY_WIN_TALK_SY, BC_MSGFONT_PAL, ENEMY_WIN_TALK_CGX
	},
	{	// 8:「会話(メニューウィンドウ)」
		BC_FRAME_TYPE, ENEMY_WIN_TALKMENU_PX, ENEMY_WIN_TALKMENU_PY,	//フレームが違うので注意！
		ENEMY_WIN_TALKMENU_SX, ENEMY_WIN_TALKMENU_SY, BC_MSGFONT_PAL, ENEMY_WIN_TALKMENU_CGX
	},
	{	// 9:「会話(メニューウィンドウ2)」
		BC_FRAME_TYPE, ENEMY_WIN_TALKMENU2_PX, ENEMY_WIN_TALKMENU2_PY,	//フレームが違うので注意！
		ENEMY_WIN_TALKMENU2_SX, ENEMY_WIN_TALKMENU2_SY, BC_MSGFONT_PAL, ENEMY_WIN_TALKMENU2_CGX
	},
	{	// 10:「はい・いいえ」
		BC_FRAME_TYPE, ENEMY_WIN_YESNO_PX, ENEMY_WIN_YESNO_PY,			//フレームが違うので注意！
		ENEMY_WIN_YESNO_SX, ENEMY_WIN_YESNO_SY, BC_FONT_PAL, ENEMY_WIN_YESNO_CGX
	},
	{	// 11:「レベル＋５・レベルー５・やめる」
		BC_FRAME_TYPE, ENEMY_WIN_SEL_PX, ENEMY_WIN_SEL_PY,				//フレームが違うので注意！
		ENEMY_WIN_SEL_SX, ENEMY_WIN_SEL_SY, BC_FONT_PAL, ENEMY_WIN_SEL_CGX
	},
};


//==============================================================================================
//
//	プロトタイプ宣言
//
//==============================================================================================
void CastleAddBmpWin( GF_BGL_INI* bgl, GF_BGL_BMPWIN* win, u8 bmp_no );
void CastleExitBmpWin( GF_BGL_BMPWIN* win, u8 bmp_no );
void CastleOffBmpWin( GF_BGL_BMPWIN* win, u8 bmp_no );
void CastleWriteMenuWin( GF_BGL_INI* bgl, GF_BGL_BMPWIN* win );
void CastleTalkWinPut( GF_BGL_BMPWIN * win, WINTYPE wintype );

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


typedef struct{
	const BMPWIN_DAT* dat;				//BMPデータ
	u32	max;							//登録最大数
}CASTLE_BMP;

//
static const CASTLE_BMP castle_bmp[CASTLE_BMP_MAX] = {
	{ CastleMineBmpData,	CASTLE_MINE_BMPWIN_MAX	},
	{ CastleEnemyBmpData,	CASTLE_ENEMY_BMPWIN_MAX	},
};


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
void CastleAddBmpWin( GF_BGL_INI* bgl, GF_BGL_BMPWIN* win, u8 bmp_no )
{
	u8 i;
	const BMPWIN_DAT* dat = castle_bmp[bmp_no].dat;
	u32 max = castle_bmp[bmp_no].max;

	//ビットマップ追加
	for( i=0; i < max; i++ ){
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
void CastleExitBmpWin( GF_BGL_BMPWIN* win, u8 bmp_no )
{
	u16	i;
	u32 max = castle_bmp[bmp_no].max;

	for( i=0; i < max; i++ ){
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
void CastleOffBmpWin( GF_BGL_BMPWIN* win, u8 bmp_no )
{
	u16	i;
	u32 max = castle_bmp[bmp_no].max;

	for( i=0; i < max; i++ ){
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
void CastleWriteMenuWin( GF_BGL_INI* bgl, GF_BGL_BMPWIN* win )
{
	//メニューウィンドウのグラフィックをセット
	//MenuWinGraphicSet( bgl, BC_FRAME_WIN, BC_MENU_CGX_NUM, BC_MENU_PAL, 0, HEAPID_CASTLE );
	MenuWinGraphicSet(	bgl, GF_BGL_BmpWinGet_Frame(win), 
						BC_MENU_CGX_NUM, BC_MENU_PAL, 0, HEAPID_CASTLE );

	//メニューウィンドウを描画
	BmpMenuWinWrite( win, WINDOW_TRANS_OFF, BC_MENU_CGX_NUM, BC_MENU_PAL );
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
void CastleTalkWinPut( GF_BGL_BMPWIN * win, WINTYPE wintype )
{
	TalkWinGraphicSet( win->ini, GF_BGL_BmpWinGet_Frame(win), BC_TALKWIN_CGX_NUM,
						BC_TALKWIN_PAL, wintype, HEAPID_CASTLE );

	GF_BGL_BmpWinDataFill( win, 15 );
	BmpTalkWinWrite( win, WINDOW_TRANS_OFF, BC_TALKWIN_CGX_NUM, BC_TALKWIN_PAL );
	GF_BGL_BmpWinOnVReq( win );
	return;
}


