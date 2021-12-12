//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_room.c
 *	@brief		WiFiロビー	部屋システム
 *	@author		tomoya takahashi
 *	@data		2007.11.01
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#include "gflib/button_man.h"

#include "system/wipe.h"
#include "system/pm_overlay.h"
#include "system/window.h"
#include "system/fontproc.h"
#include "system/wordset.h"
#include "system/msgdata.h"
#include "system/arc_tool.dat"
#include "system/arc_util.h"
#include "system/pm_debug_wifi.h"
#include "system/pm_debug_wifi.h"
#include "system/pmfprint.h"


#include "savedata/config.h"

#include "communication/communication.h"

#include "battle/trtype_def.h"
#include "battle/wazatype_icon.h"

#include "field/shop_gra.naix"

#include "msgdata/msg_wifi_hiroba.h"
#include "msgdata/msg_wifi_system.h"
#include "msgdata/msg.naix"

#include "graphic/wifi_lobby_other.naix"
#include "graphic/unionobj2d_onlyfront.naix"

#include "wifi/dwc_lobbylib.h"

#include "wflby_room_def.h"
#include "wflby_event.h"
#include "wflby_ev_def.h"
#include "wflby_snd.h"
#include "wflby_gadget.h"
#include "wflby_timeevent.h"




//-----------------------------------------------------------------------------
/**
 *					コーディング規約
 *		●関数名
 *				１文字目は大文字それ以降は小文字にする
 *		●変数名
 *				・変数共通
 *						constには c_ を付ける
 *						staticには s_ を付ける
 *						ポインタには p_ を付ける
 *						全て合わさると csp_ となる
 *				・グローバル変数
 *						１文字目は大文字
 *				・関数内変数
 *						小文字と”＿”と数字を使用する 関数の引数もこれと同じ
*/
//-----------------------------------------------------------------------------

#ifdef PM_DEBUG
//#define	WFLBY_DEBUG_ROOM_CAMERA		// カメラを変える
#define	WFLBY_DEBUG_ROOM_DPRINT		// デバック表示
#define WFLBY_DEBUG_ROOM_ITEMCHG	// ガジェット変更
//#define	WFLBY_DEBUG_ROOM_LIGHT	// ライトを表示
//#define WFLBY_DEBUG_ROOM_TRTYPE_CHG	//  トレーナタイプ変更
//#define WFLBY_DEBUG_ROOM_PRINT_TIME	// 処理速度を表示

#endif


#ifdef WFLBY_DEBUG_ROOM_CAMERA
static	WFLBY_DEBUG_ROOM_CAMERA_FLAG = 0;
#endif

#ifdef WFLBY_DEBUG_ROOM_LIGHT
typedef struct{
	GXRgb lightcolor[2];
	GXRgb matcolor[4];
} WFLBY_DEBUG_ROOM_LIGHTDATA;
#endif




#ifdef WFLBY_DEBUG_ROOM_PRINT_TIME

static OSTick	WFLBY_DEBUG_ROOM_PRINT_TIME_Tick;
#define WFLBY_DEBUG_ROOM_PRINT_TIME_TICK_INIT	WFLBY_DEBUG_ROOM_PRINT_TIME_Tick = OS_GetTick();
#define WFLBY_DEBUG_ROOM_PRINT_TIME_TICK_PRINT(line)	OS_TPrintf( "line[%d] time %d ms\n", (line), OS_TicksToMilliSeconds(OS_GetTick() - WFLBY_DEBUG_ROOM_PRINT_TIME_Tick) );
#define WFLBY_DEBUG_ROOM_PRINT_TIME_TICK_PRINT_DRAW(line)	OS_TPrintf( "line[%d] time %d ms\n", (line), OS_TicksToMilliSeconds(OS_GetTick() - WFLBY_DEBUG_ROOM_PRINT_TIME_Tick) );

#else		// WFLBY_DEBUG_ROOM_PRINT_TIME

#define		WFLBY_DEBUG_ROOM_PRINT_TIME_TICK_INIT			((void)0);
#define		WFLBY_DEBUG_ROOM_PRINT_TIME_TICK_PRINT(line)	((void)0);
#define		WFLBY_DEBUG_ROOM_PRINT_TIME_TICK_PRINT_DRAW(line)	((void)0);

#endif		// WFLBY_DEBUG_ROOM_PRINT_TIME


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------


//-------------------------------------
///	メインシーケンス
//=====================================
enum{
	WFLBY_ROOM_MAINSEQ_FADEIN,
	WFLBY_ROOM_MAINSEQ_FADEIN_WAIT,
	WFLBY_ROOM_MAINSEQ_MAIN,
	WFLBY_ROOM_MAINSEQ_ERR_MSG,
	WFLBY_ROOM_MAINSEQ_ERR_MSGWAIT,
	WFLBY_ROOM_MAINSEQ_TIMEOUT_MSG,
	WFLBY_ROOM_MAINSEQ_TIMEOUT_MSGWAIT,
	WFLBY_ROOM_MAINSEQ_FADEOUT,
	WFLBY_ROOM_MAINSEQ_FADEOUT_WAIT,
};

//-------------------------------------
///	3DOBJCONT関連
//=====================================
#define WFLBY_ROOM_3DOBJCONT_WKNUM		( 24 )

//-------------------------------------
///	表示システム関連
//=====================================
#define WFLBY_ROOM_VTRTSK_NUM			( 32 )	// VRAM転送タスク数

//-------------------------------------
// BG
//=====================================
enum{
	// メイン画面
	WFLBY_ROOM_BGCNT_MAIN_MSGWIN,
	
	// サブ画面
	WFLBY_ROOM_BGCNT_SUB_BACK,
	WFLBY_ROOM_BGCNT_SUB_BTTN_TR,
	WFLBY_ROOM_BGCNT_SUB_BTTN2_MSG,

	WFLBY_ROOM_BGCNT_NUM,	// ＢＧコントロールテーブル数
};
enum {	// メイン面パレット
	WFLBY_ROOM_BGPL_DMY,				//　ダミー
	WFLBY_ROOM_BGPL_TALKWIN,			// 会話ウィンドウ
	WFLBY_ROOM_BGPL_BOARDWIN,			// 看板ウィンドウ
	WFLBY_ROOM_BGPL_SYSWIN,				// システムウィンドウ
	WFLBY_ROOM_BGPL_TALKFONT_CL,		// 会話フォント
	WFLBY_ROOM_BGPL_SYSFONT_CL,			// システムフォント

	WFLBY_ROOM_BGPL_NUM,		// 部屋が使用するパレット
};
enum {	// サブ面パレット
	WFLBY_ROOM_BGSPL_BACK00,			// 背景やらなんやら
	WFLBY_ROOM_BGSPL_BACK01,			// 背景やらなんやら
	WFLBY_ROOM_BGSPL_BACK02,			// 背景やらなんやら
	WFLBY_ROOM_BGSPL_BACK03,			// 背景やらなんやら
	WFLBY_ROOM_BGSPL_BACK04,			// 背景やらなんやら
	WFLBY_ROOM_BGSPL_FONTCL,			// フォントカラー
	WFLBY_ROOM_BGSPL_NONE01,			// 
	WFLBY_ROOM_BGSPL_NONE02,			// 
	WFLBY_ROOM_BGSPL_NONE03,			// 
	WFLBY_ROOM_BGSPL_NONE04,			// 
	WFLBY_ROOM_BGSPL_BACK05,			// 背景やらなんやら
	WFLBY_ROOM_BGSPL_WAZATYPE0,			// 技タイプ0
	WFLBY_ROOM_BGSPL_WAZATYPE1,			// 技タイプ1
	WFLBY_ROOM_BGSPL_WAZATYPE2,			// 技タイプ2 
	WFLBY_ROOM_BGSPL_NONE08,			// 
	WFLBY_ROOM_BGSPL_BACK06,			// 背景やらなんやら

	WFLBY_ROOM_BGSPL_NUM,		// 部屋が使用するパレット
};

//-------------------------------------
// ウィンドウグラフィック
//=====================================
#define WFLBY_TALKWINGRA_CGX		( 1 )
#define WFLBY_BOARDWINGRA_CGX		( WFLBY_TALKWINGRA_CGX+TALK_WIN_CGX_SIZ )
#define WFLBY_SYSWINGRA_CGX			( WFLBY_BOARDWINGRA_CGX+(BOARD_WIN_CGX_SIZ+BOARD_MAP_CGX_SIZ) )
#define WFLBY_SYSWINGRA_CGXEND		( WFLBY_SYSWINGRA_CGX+MENU_WIN_CGX_SIZ )



//-------------------------------------
// OAM
//=====================================
#define WFLBY_ROOM_OAM_CONTNUM			( 24 )	// OAM管理数
#define WFLBY_ROOM_OAM_RESNUM			( 4 )	// リソース管理種類
#define WFLBY_ROOM_CLACT_NUM			( 24 )
#define WFLBY_ROOM_CLACT_SUBSURFACE_Y	( 256*FX32_ONE )


//-------------------------------------
//プロジェクションマトリクスを操作する際のＺオフセット
//=====================================
#define	WFLBY_ROOM_PRO_MAT_Z_OFS	(1)



//-------------------------------------
///	強制終了メッセージウエイト
//=====================================
#define WFLBY_MSG_WAIT	( 60 )


//-------------------------------------
// 会話ウィンドウ
//=====================================
#define WFLBY_TALKWIN_X			( 2 )
#define WFLBY_TALKWIN_Y			( 19 )
#define WFLBY_TALKWIN_SIZX		( 27 )
#define WFLBY_TALKWIN_SIZY		( 4 )
#define WFLBY_TALKWIN_CGX		( WFLBY_SYSWINGRA_CGXEND )
#define WFLBY_TALKWIN_CGXEND	( WFLBY_TALKWIN_CGX+(WFLBY_TALKWIN_SIZX*WFLBY_TALKWIN_SIZY) )
#define WFLBY_TALKWIN_STRBUFNUM	( 384 )	// 文字バッファ数
#define WFLBY_TALKWIN_MSGCOL	( GF_PRINTCOLOR_MAKE( 1, 2, 15 ) )	

//-------------------------------------
// リストウィンドウ
//=====================================
#define WFLBY_LISTWIN_X			( 16 )
#define WFLBY_LISTWIN_Y			( 3 )
#define WFLBY_LISTWIN_SIZX		( 15 )
#define WFLBY_LISTWIN_SIZY		( 18 )	// リスト最大サイズ
#define WFLBY_LISTWIN_CGX		( WFLBY_TALKWIN_CGXEND )
#define WFLBY_LISTWIN_CGXEND	( WFLBY_LISTWIN_CGX+(WFLBY_LISTWIN_SIZX*WFLBY_LISTWIN_SIZY) )
#define WFLBY_LISTWIN_MSGCOL	( GF_PRINTCOLOR_MAKE( 1, 2, 15 ) )	

#define WFLBY_LISTWIN_CLACT_RESNUM	(4)	// セルリソース数
#define WFLBY_LISTWIN_CLACT_OBJNUM	(2)	// アクターオブジェ数
#define WFLBY_LISTWIN_CLACT_RESCONT_ID	( 5000 )
#define WFLBY_LISTWIN_CLACT_X		( 192 )
static const u8 sc_WFLBY_LISTWIN_CLACT_Y[ WFLBY_LISTWIN_CLACT_OBJNUM ] = {
	20, 136
};

//-------------------------------------
///	エラー表示ウィンドウ
//=====================================
#define WFLBY_ERRWIN_X			( 4 )
#define WFLBY_ERRWIN_Y			( 4 )
#define WFLBY_ERRWIN_SIZX		( 23 )
#define WFLBY_ERRWIN_SIZY		( 16 )
#define WFLBY_ERRWIN_CGX		( WFLBY_SYSWINGRA_CGXEND )
#define WFLBY_ERRWIN_CGXEND		( WFLBY_ERRWIN_CGX+(WFLBY_ERRWIN_SIZX*WFLBY_ERRWIN_SIZY) )
#define WFLBY_ERRWIN_MSGCOL		( GF_PRINTCOLOR_MAKE( 1, 2, 15 ) )	

//-------------------------------------
///	サブウィンドウ
//=====================================
#define WFLBY_SUBWIN_MSGCOL		( GF_PRINTCOLOR_MAKE( 1, 2, 15 ) )	



//-------------------------------------
///	YesNoWin
//=====================================
#define WFLBY_YESNOWIN_DATANUM	( 2 )		// 項目数


//-------------------------------------
/// 広場専用メッセージ
//=====================================
#define WFLBY_DEFMSG_STRNUM		( 480 )


//-------------------------------------
// 下画面
//=====================================
enum {	// 描画状態
	WFLBY_UNDERWIN_DRAW_NONE,		// 何もなし
	WFLBY_UNDERWIN_DRAW_BTTN,		// ボタン表示
	WFLBY_UNDERWIN_DRAW_PROFILE,	// プロフィール表示
	WFLBY_UNDERWIN_DRAW_FLOAT,		// フロート用ボタン表示
	WFLBY_UNDERWIN_DRAW_BTTN_DUMMY,	// ボタン表示　ダミー（最初にタッチトイをもらうとき）
} ;
enum{	// 動作シーケンス
	WFLBY_UNDERWIN_SEQ_STARTWAIT,			// 開始待ち
	WFLBY_UNDERWIN_SEQ_STARTFADEOUT,		// 開始フェードアウト
	WFLBY_UNDERWIN_SEQ_STARTFADEOUTWAIT,	// 開始フェードアウト待ち
	WFLBY_UNDERWIN_SEQ_STARTFADEIN,			// 開始フェードイン
	WFLBY_UNDERWIN_SEQ_STARTFADEINWAIT,		// 開始フェードイン待ち
	WFLBY_UNDERWIN_SEQ_NORMAL,
	WFLBY_UNDERWIN_SEQ_FADEOUT,
	WFLBY_UNDERWIN_SEQ_FADEOUTWAIT,
	WFLBY_UNDERWIN_SEQ_CHANGE,
	WFLBY_UNDERWIN_SEQ_FASEIN,
	WFLBY_UNDERWIN_SEQ_FASEINWAIT,
};
#define WFLBY_UNDERWIN_FASE_DIV		( 3 )
#define WFLBY_UNDERWIN_FASE_SYNC	( 1 )

#define WFLBY_UNDERWIN_BGCG_END		( 320 )	// 背景とフレーム用CGXのオワリ



//-------------------------------------
// trainerカード
//=====================================
#define WFLBY_TRCARD_STRBUF	( 64 )		// 文字列バッファ要領
enum{	// ウィンドウ定数
	WFLBY_TRCARD_WIN_NAME,
	WFLBY_TRCARD_WIN_COUNTRY,
	WFLBY_TRCARD_WIN_TRSTART,
	WFLBY_TRCARD_WIN_LASTACTION,
	WFLBY_TRCARD_WIN_VIPAIKOTOBA,
	WFLBY_TRCARD_WIN_NUM,
};
#define WFLBY_TRCARD_WIN_NAME_X		( 8 )
#define WFLBY_TRCARD_WIN_NAME_Y		( 2 )
#define WFLBY_TRCARD_WIN_NAME_SIZX	( 16 )
#define WFLBY_TRCARD_WIN_NAME_SIZY	( 4 )
#define WFLBY_TRCARD_WIN_NAME_CGX	( WFLBY_UNDERWIN_BGCG_END )

#define WFLBY_TRCARD_WIN_COUNTRY_X		( 1 )
#define WFLBY_TRCARD_WIN_COUNTRY_Y		( 7 )
#define WFLBY_TRCARD_WIN_COUNTRY_SIZX	( 30 )
#define WFLBY_TRCARD_WIN_COUNTRY_SIZY	( 6 )
#define WFLBY_TRCARD_WIN_COUNTRY_CGX	( WFLBY_TRCARD_WIN_NAME_CGX+(WFLBY_TRCARD_WIN_NAME_SIZX*WFLBY_TRCARD_WIN_NAME_SIZY) )

#define WFLBY_TRCARD_WIN_TRSTART_X		( 1 )
#define WFLBY_TRCARD_WIN_TRSTART_Y		( 14 )
#define WFLBY_TRCARD_WIN_TRSTART_SIZX	( 19 )
#define WFLBY_TRCARD_WIN_TRSTART_SIZY	( 4 )
#define WFLBY_TRCARD_WIN_TRSTART_CGX	( WFLBY_TRCARD_WIN_COUNTRY_CGX+(WFLBY_TRCARD_WIN_COUNTRY_SIZX*WFLBY_TRCARD_WIN_COUNTRY_SIZY) )

#define WFLBY_TRCARD_WIN_LASTACTION_X		( 1 )
#define WFLBY_TRCARD_WIN_LASTACTION_Y		( 19 )
#define WFLBY_TRCARD_WIN_LASTACTION_SIZX	( 19 )
#define WFLBY_TRCARD_WIN_LASTACTION_SIZY	( 4 )
#define WFLBY_TRCARD_WIN_LASTACTION_CGX	( WFLBY_TRCARD_WIN_TRSTART_CGX+(WFLBY_TRCARD_WIN_TRSTART_SIZX*WFLBY_TRCARD_WIN_TRSTART_SIZY) )

#define WFLBY_TRCARD_WIN_VIPAIKOTOBA_X		( 0 )
#define WFLBY_TRCARD_WIN_VIPAIKOTOBA_Y		( 14 )
#define WFLBY_TRCARD_WIN_VIPAIKOTOBA_SIZX	( 22 )
#define WFLBY_TRCARD_WIN_VIPAIKOTOBA_SIZY	( 8 )
#define WFLBY_TRCARD_WIN_VIPAIKOTOBA_CGX	( WFLBY_TRCARD_WIN_COUNTRY_CGX+(WFLBY_TRCARD_WIN_COUNTRY_SIZX*WFLBY_TRCARD_WIN_COUNTRY_SIZY) )

// 技タイプ
#define WFLBY_TRCARD_WIN_WAZATYPE1_X		( 26 )
#define WFLBY_TRCARD_WIN_WAZATYPE1_Y		( 2 )
#define WFLBY_TRCARD_WIN_WAZATYPE1_SIZEX	( 4 )
#define WFLBY_TRCARD_WIN_WAZATYPE1_SIZEY	( 2 )
#define WFLBY_TRCARD_WIN_WAZATYPE1_CGX		( WFLBY_TRCARD_WIN_VIPAIKOTOBA_CGX+(WFLBY_TRCARD_WIN_VIPAIKOTOBA_SIZX*WFLBY_TRCARD_WIN_VIPAIKOTOBA_SIZY) )
#define WFLBY_TRCARD_WIN_WAZATYPE2_CGX		( WFLBY_TRCARD_WIN_WAZATYPE1_CGX+(WFLBY_TRCARD_WIN_WAZATYPE1_SIZEX*WFLBY_TRCARD_WIN_WAZATYPE1_SIZEY) )
static const u16 sc_WFLBY_TRCARD_WAZATYPE_CGX[ WFLBY_SELECT_TYPENUM ] = {
	WFLBY_TRCARD_WIN_WAZATYPE1_CGX,
	WFLBY_TRCARD_WIN_WAZATYPE2_CGX,
};

static const u16 sc_WFLBY_TRCARD_WAZATYPE_SCRN[ WFLBY_SELECT_TYPENUM ][WFLBY_TRCARD_WIN_WAZATYPE1_SIZEY][WFLBY_TRCARD_WIN_WAZATYPE1_SIZEX] ={
	{
		{ 
			WFLBY_TRCARD_WIN_WAZATYPE1_CGX+0, WFLBY_TRCARD_WIN_WAZATYPE1_CGX+1,
			WFLBY_TRCARD_WIN_WAZATYPE1_CGX+2, WFLBY_TRCARD_WIN_WAZATYPE1_CGX+3,
		},
		{ 
			WFLBY_TRCARD_WIN_WAZATYPE1_CGX+4, WFLBY_TRCARD_WIN_WAZATYPE1_CGX+5,
			WFLBY_TRCARD_WIN_WAZATYPE1_CGX+6, WFLBY_TRCARD_WIN_WAZATYPE1_CGX+7,
		},
	},
	{
		{ 
			WFLBY_TRCARD_WIN_WAZATYPE2_CGX+0, WFLBY_TRCARD_WIN_WAZATYPE2_CGX+1,
			WFLBY_TRCARD_WIN_WAZATYPE2_CGX+2, WFLBY_TRCARD_WIN_WAZATYPE2_CGX+3,
		},
		{ 
			WFLBY_TRCARD_WIN_WAZATYPE2_CGX+4, WFLBY_TRCARD_WIN_WAZATYPE2_CGX+5,
			WFLBY_TRCARD_WIN_WAZATYPE2_CGX+6, WFLBY_TRCARD_WIN_WAZATYPE2_CGX+7,
		},
	},
};


static const BMPWIN_DAT sc_WFLBY_TRCARD_WIN_DATA[ WFLBY_TRCARD_WIN_NUM ] = {
	{
		GF_BGL_FRAME2_S,
		WFLBY_TRCARD_WIN_NAME_X,
		WFLBY_TRCARD_WIN_NAME_Y,
		WFLBY_TRCARD_WIN_NAME_SIZX,
		WFLBY_TRCARD_WIN_NAME_SIZY,
		WFLBY_ROOM_BGSPL_FONTCL,
		WFLBY_TRCARD_WIN_NAME_CGX,
	},
	{
		GF_BGL_FRAME2_S,
		WFLBY_TRCARD_WIN_COUNTRY_X,
		WFLBY_TRCARD_WIN_COUNTRY_Y,
		WFLBY_TRCARD_WIN_COUNTRY_SIZX,
		WFLBY_TRCARD_WIN_COUNTRY_SIZY,
		WFLBY_ROOM_BGSPL_FONTCL,
		WFLBY_TRCARD_WIN_COUNTRY_CGX,
	},
	{
		GF_BGL_FRAME2_S,
		WFLBY_TRCARD_WIN_TRSTART_X,
		WFLBY_TRCARD_WIN_TRSTART_Y,
		WFLBY_TRCARD_WIN_TRSTART_SIZX,
		WFLBY_TRCARD_WIN_TRSTART_SIZY,
		WFLBY_ROOM_BGSPL_FONTCL,
		WFLBY_TRCARD_WIN_TRSTART_CGX,
	},
	{
		GF_BGL_FRAME2_S,
		WFLBY_TRCARD_WIN_LASTACTION_X,
		WFLBY_TRCARD_WIN_LASTACTION_Y,
		WFLBY_TRCARD_WIN_LASTACTION_SIZX,
		WFLBY_TRCARD_WIN_LASTACTION_SIZY,
		WFLBY_ROOM_BGSPL_FONTCL,
		WFLBY_TRCARD_WIN_LASTACTION_CGX,
	},
	{
		GF_BGL_FRAME2_S,
		WFLBY_TRCARD_WIN_VIPAIKOTOBA_X,
		WFLBY_TRCARD_WIN_VIPAIKOTOBA_Y,
		WFLBY_TRCARD_WIN_VIPAIKOTOBA_SIZX,
		WFLBY_TRCARD_WIN_VIPAIKOTOBA_SIZY,
		WFLBY_ROOM_BGSPL_FONTCL,
		WFLBY_TRCARD_WIN_VIPAIKOTOBA_CGX,
	},
};
// 文字色
#define WFLBY_TRCARD_COL_RED	( GF_PRINTCOLOR_MAKE( 3, 4, 0 ) )
#define WFLBY_TRCARD_COL_BLUE	( GF_PRINTCOLOR_MAKE( 7, 8, 0 ) )
#define WFLBY_TRCARD_COL_BLACK	( GF_PRINTCOLOR_MAKE( 1, 2, 0 ) )
// アイテムアイコン
#define WFLBY_TRCARD_ICON_RESID		( 100 )
#define WFLBY_TRCARD_ICON_PLTTNUM	( 3 )		// パレット本数
#define WFLBY_TRCARD_ICON_BGPRI		( 0 )		// BG優先順位
#define WFLBY_TRCARD_ICON_SFPRI		( 16 )		// 優先順位
#define WFLBY_TRCARD_ICON_X			( FX32_CONST( 208 ) )
#define WFLBY_TRCARD_ICON_Y			( FX32_CONST( 152 )+WFLBY_ROOM_CLACT_SUBSURFACE_Y )
#define WFLBY_TRCARD_TRVIEW_RESID	( 101 )
#define WFLBY_TRCARD_TRVIEW_BGPRI	( 3 )
#define WFLBY_TRCARD_TRVIEW_SFPRI	( 32 )

// フィールドトレーナＩＤをバトルトレーナタイプに変換するテーブル
typedef struct {
	u16 fld_trno;
	u16  btl_trtype;
	u16 x;
	u16 y;
} WFLBY_TRCARD_TRTYPE;
#define WFLBY_TRCARD_TRTYPE_NUM	( 18 )
static const WFLBY_TRCARD_TRTYPE sc_WFLBY_TRCARD_TRTYPE[WFLBY_TRCARD_TRTYPE_NUM]={
	{  HERO,		TRTYPE_BOY,		33,	35 },
	{  HEROINE,		TRTYPE_GIRL,	38,	36 },

	{  BOY1,		TRTYPE_SCHOOLB,	38,	25 },
	{  BOY3,		TRTYPE_MUSHI,	44,	32 },
	{  MAN3,		TRTYPE_ELITEM,	38,	44 },
	{  BADMAN,		TRTYPE_HEADS,	40,	40 },
	{  EXPLORE,		TRTYPE_ISEKI,	44,	26 },
	{  FIGHTER,		TRTYPE_KARATE,	44,	37 },
	{  GORGGEOUSM,	TRTYPE_PRINCE,	29,	40 },
	{  MYSTERY,		TRTYPE_ESPM,	42,	44 },

	{  GIRL1,		TRTYPE_MINI,	39,	34 },
	{  GIRL2,		TRTYPE_BATTLEG,	40,	36 },
	{  WOMAN2,		TRTYPE_SISTER,	38,	41 },
	{  WOMAN3,		TRTYPE_ELITEW,	36,	42 },
	{  IDOL,		TRTYPE_IDOL,	38,	38 },
	{  LADY,		TRTYPE_MADAM,	38,	39 },
	{  COWGIRL,		TRTYPE_COWGIRL,	38,	35 },
	{  GORGGEOUSW,	TRTYPE_PRINCESS,40,	44 },
};
// 履歴リソース
enum{
	WFLBY_TRCARD_RIREKI_RES_UNIONOBJ,
	WFLBY_TRCARD_RIREKI_RES_ICON,
	WFLBY_TRCARD_RIREKI_RES_HEROBOY,
	WFLBY_TRCARD_RIREKI_RES_HEROGIRL,
	WFLBY_TRCARD_RIREKI_RES_NUM,
};
#define WFLBY_TRCARD_RIREKI_RESID		( 150 )
#define WFLBY_TRCARD_RIREKI_BGPRI		( 0 )		// BG優先順位
// 書く履歴に対応したキャラクタリソースデータ
typedef struct{
	u8 arcid;
	u8 paloffs;
	u16 dataid;
	s16 ofs_x;
	s16 ofs_y;
} WFLBY_TRCARD_RIREKI_CHAR;
static const WFLBY_TRCARD_RIREKI_CHAR sc_WFLBY_TRCARD_RIREKI_CHAR[ WFLBY_LASTACTION_MAX ] = {
	{ WFLBY_TRCARD_RIREKI_RES_UNIONOBJ, 0, NARC_unionobj2d_onlyfront_only_front00_NCGR, -8, -6 },
	{ WFLBY_TRCARD_RIREKI_RES_UNIONOBJ, 6, NARC_unionobj2d_onlyfront_only_front01_NCGR, -8, -6 },
	{ WFLBY_TRCARD_RIREKI_RES_UNIONOBJ, 1, NARC_unionobj2d_onlyfront_only_front02_NCGR, -8, -6 },
	{ WFLBY_TRCARD_RIREKI_RES_UNIONOBJ, 2, NARC_unionobj2d_onlyfront_only_front03_NCGR, -8, -6 },
	{ WFLBY_TRCARD_RIREKI_RES_UNIONOBJ, 6, NARC_unionobj2d_onlyfront_only_front04_NCGR, -8, -6 },
	{ WFLBY_TRCARD_RIREKI_RES_UNIONOBJ, 5, NARC_unionobj2d_onlyfront_only_front05_NCGR, -8, -6 },
	{ WFLBY_TRCARD_RIREKI_RES_UNIONOBJ, 3, NARC_unionobj2d_onlyfront_only_front06_NCGR, -8, -6 },
	{ WFLBY_TRCARD_RIREKI_RES_UNIONOBJ, 4, NARC_unionobj2d_onlyfront_only_front07_NCGR, -8, -6 },
	{ WFLBY_TRCARD_RIREKI_RES_UNIONOBJ, 0, NARC_unionobj2d_onlyfront_only_front08_NCGR, -8, -6 },
	{ WFLBY_TRCARD_RIREKI_RES_UNIONOBJ, 7, NARC_unionobj2d_onlyfront_only_front09_NCGR, -8, -6 },
	{ WFLBY_TRCARD_RIREKI_RES_UNIONOBJ, 4, NARC_unionobj2d_onlyfront_only_front10_NCGR, -8, -6 },
	{ WFLBY_TRCARD_RIREKI_RES_UNIONOBJ, 1, NARC_unionobj2d_onlyfront_only_front11_NCGR, -8, -6 },
	{ WFLBY_TRCARD_RIREKI_RES_UNIONOBJ, 5, NARC_unionobj2d_onlyfront_only_front12_NCGR, -8, -6 },
	{ WFLBY_TRCARD_RIREKI_RES_UNIONOBJ, 5, NARC_unionobj2d_onlyfront_only_front13_NCGR, -8, -6 },
	{ WFLBY_TRCARD_RIREKI_RES_UNIONOBJ, 2, NARC_unionobj2d_onlyfront_only_front14_NCGR, -8, -6 },
	{ WFLBY_TRCARD_RIREKI_RES_UNIONOBJ, 3, NARC_unionobj2d_onlyfront_only_front15_NCGR, -8, -6 },

	{ WFLBY_TRCARD_RIREKI_RES_ICON, 0, NARC_wifi_lobby_other_profile_icon1_NCGR, 0, 0 },
	{ WFLBY_TRCARD_RIREKI_RES_ICON, 1, NARC_wifi_lobby_other_profile_icon2_NCGR, 0, 0 },
	{ WFLBY_TRCARD_RIREKI_RES_ICON, 0, NARC_wifi_lobby_other_profile_icon3_NCGR, 0, 0 },
	{ WFLBY_TRCARD_RIREKI_RES_ICON, 0, NARC_wifi_lobby_other_profile_icon6_NCGR, 0, 0 },
	{ WFLBY_TRCARD_RIREKI_RES_ICON, 1, NARC_wifi_lobby_other_profile_icon5_NCGR, 0, 0 },
	{ WFLBY_TRCARD_RIREKI_RES_ICON, 0, NARC_wifi_lobby_other_profile_icon4_NCGR, 0, 0 },
	{ WFLBY_TRCARD_RIREKI_RES_ICON, 1, NARC_wifi_lobby_other_profile_icon7_NCGR, 0, 0 },
	{ WFLBY_TRCARD_RIREKI_RES_ICON, 0, NARC_wifi_lobby_other_profile_icon8_NCGR, 0, 0 },
};
enum{
	WFLBY_TRCARD_RIREKI_SEX_MAN,	
	WFLBY_TRCARD_RIREKI_SEX_GIRL,	
	WFLBY_TRCARD_RIREKI_SEX_NUM,	
};
static const WFLBY_TRCARD_RIREKI_CHAR sc_WFLBY_TRCARD_RIREKI_HERO_CHAR[ WFLBY_TRCARD_RIREKI_SEX_NUM ] = {
	{ WFLBY_TRCARD_RIREKI_RES_HEROBOY, 0, NARC_unionobj2d_onlyfront_pl_boy01_NCGR, -8, -6 },
	{ WFLBY_TRCARD_RIREKI_RES_HEROGIRL, 0, NARC_unionobj2d_onlyfront_pl_girl01_NCGR, -8, -6 },
};
static const VecFx32 sc_WFLBY_TRCARD_RIREKI_POS[ WFLBY_LASTACT_BUFFNUM ] = {
	{ FX32_CONST(32), FX32_CONST(144)+WFLBY_ROOM_CLACT_SUBSURFACE_Y, 0 },
	{ FX32_CONST(56), FX32_CONST(144)+WFLBY_ROOM_CLACT_SUBSURFACE_Y, 0 },
	{ FX32_CONST(80), FX32_CONST(144)+WFLBY_ROOM_CLACT_SUBSURFACE_Y, 0 },
	{ FX32_CONST(104), FX32_CONST(144)+WFLBY_ROOM_CLACT_SUBSURFACE_Y, 0 },
	{ FX32_CONST(128), FX32_CONST(144)+WFLBY_ROOM_CLACT_SUBSURFACE_Y, 0 },
	{ FX32_CONST(152), FX32_CONST(144)+WFLBY_ROOM_CLACT_SUBSURFACE_Y, 0 },

	{ FX32_CONST(32), FX32_CONST(176)+WFLBY_ROOM_CLACT_SUBSURFACE_Y, 0 },
	{ FX32_CONST(56), FX32_CONST(176)+WFLBY_ROOM_CLACT_SUBSURFACE_Y, 0 },
	{ FX32_CONST(80), FX32_CONST(176)+WFLBY_ROOM_CLACT_SUBSURFACE_Y, 0 },
	{ FX32_CONST(104), FX32_CONST(176)+WFLBY_ROOM_CLACT_SUBSURFACE_Y, 0 },
	{ FX32_CONST(128), FX32_CONST(176)+WFLBY_ROOM_CLACT_SUBSURFACE_Y, 0 },
	{ FX32_CONST(152), FX32_CONST(176)+WFLBY_ROOM_CLACT_SUBSURFACE_Y, 0 },
};


//-------------------------------------
///	ガジェットボタン
//=====================================
enum{
	WFLBY_ROOM_UNDERWIN_BTTN_MODE_GADGET,		// ガジェット
	WFLBY_ROOM_UNDERWIN_BTTN_MODE_FLOAT,		// フロート
};
enum{	// アニメフレーム
	WFLBY_ROOM_UNDERWIN_BTTN_ANM_UP,
	WFLBY_ROOM_UNDERWIN_BTTN_ANM_DOWN,
	WFLBY_ROOM_UNDERWIN_BTTN_ANM_CENTER,
	WFLBY_ROOM_UNDERWIN_BTTN_ANM_NUM,
};
enum{	//  ボタン動作ステータス
	WFLBY_ROOM_UNDERWIN_BTTN_STATUS_ON,		// 押して大丈夫
	WFLBY_ROOM_UNDERWIN_BTTN_STATUS_WAIT,	// 押しちゃだめ
};
#define WFLBY_ROOM_UNDERWIN_BTTN_WAIT	( 90 )	// ボタンを押せない間隔
#define WFLBY_ROOM_UNDERWIN_BTTN_FLOAT_FRAME_NUM	(2)// フロート用ボタンのフレーム数

#define WFLBY_UNDERWIN_BTTN_ICON_BGCG_END		( WFLBY_UNDERWIN_BGCG_END+144 )	// 背景とフレーム用CGXのオワリ


#define WFLBY_BTTN_WIN_TITLE_X		( 1 )
#define WFLBY_BTTN_WIN_TITLE_Y		( 0 )
#define WFLBY_BTTN_WIN_TITLE_SIZX	( 24 )
#define WFLBY_BTTN_WIN_TITLE_SIZY	( 3 )
#define WFLBY_BTTN_WIN_TITLE_CGX	( WFLBY_UNDERWIN_BTTN_ICON_BGCG_END )
static const BMPWIN_DAT sc_WFLBY_BTTN_WIN_DATA = {
	GF_BGL_FRAME1_S,
	WFLBY_BTTN_WIN_TITLE_X,
	WFLBY_BTTN_WIN_TITLE_Y,
	WFLBY_BTTN_WIN_TITLE_SIZX,
	WFLBY_BTTN_WIN_TITLE_SIZY,
	WFLBY_ROOM_BGSPL_FONTCL,
	WFLBY_BTTN_WIN_TITLE_CGX,
};

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	グラフィックシステム
//=====================================
typedef struct {
	// BG
	GF_BGL_INI*				p_bgl;

	// OAM
    CLACT_SET_PTR           p_clactset;		// セルアクターセット
    CLACT_U_EASYRENDER_DATA renddata;       // 簡易レンダーデータ
    CLACT_U_RES_MANAGER_PTR p_resman[WFLBY_ROOM_OAM_RESNUM]; // キャラ・パレットリソースマネージャ
	NNSG2dCellTransferState*	p_celltransarray;		///< セルVram転送マネージャー領域

	// 3D
	GF_G3DMAN*		p_3dman;
} WFLBY_GRAPHICCONT;


//-------------------------------------
///	会話メッセージ表示
//=====================================
typedef struct {
	GF_BGL_BMPWIN	win;
	u32				msgno;
	u32				msgwait;
	STRBUF*			p_str;
	void*			p_timewait;
} WFLBY_ROOM_TALKMSG;

//-------------------------------------
///	リスト表示
//=====================================
typedef struct {
	BMPLIST_HEADER	data;
	GF_BGL_BMPWIN	win;
	BMPLIST_WORK*	p_listwk;
	BMP_MENULIST_DATA*	p_bmplist;
	u16					bmplistnum;
	u16					list_num;

	// 上下　セルアクター
	BOOL				clact_draw;
	CLACT_U_RES_OBJ_PTR		resObj[WFLBY_LISTWIN_CLACT_RESNUM];	// それぞれのリソースオブジェ数
	CLACT_WORK_PTR clact[ WFLBY_LISTWIN_CLACT_OBJNUM ];
} WFLBY_ROOM_LISTWIN;

//-------------------------------------
///	YESNOWIN
//=====================================
typedef struct {
	BMP_MENULIST_DATA*	p_bmplist;		// リスト
	BMPLIST_HEADER		bmplist;
} WFLBY_ROOM_YESNOWIN;


//-------------------------------------
///	エラーメッセージ表示
//=====================================
typedef struct {
	GF_BGL_BMPWIN	win;
} WFLBY_ROOM_ERRMSG;

//-------------------------------------
///	サブウィンドウ表示
//=====================================
typedef struct {
	GF_BGL_BMPWIN	win;
} WFLBY_ROOM_SUBWIN;

//-------------------------------------
///	広場メッセージデータ
//=====================================
typedef struct {
	WORDSET*			p_wordset;
	MSGDATA_MANAGER*	p_msgman[WFLBY_DEFMSG_TYPE_NUM];
	STRBUF*				p_str;
	STRBUF*				p_tmp;
} WFLBY_ROOM_DEFMSG;

//-------------------------------------
///	ガジェットボタン
//=====================================
typedef struct {
	s16	lock_wait;
	u8 seq;
	u8 bttnevent;
	u16 lastbttnevent;
	u16 bttnstop;		// ボタン停止時TRUE
	u16 mode;			// ボタンモード	WFLBY_ROOM_UNDERWIN_BTTN_MODE_～
	u16 bttn_itemno;	// ボタンが対応しているアイテムの値（modeによって中の値の意味が変わります）

	// ウィンドウ
	GF_BGL_BMPWIN		win;

	// ボタンマネージャ
	BUTTON_MAN*	p_bttnman;

	// リソース
	void*				p_scrnbuff[ WFLBY_ROOM_UNDERWIN_BTTN_ANM_NUM ];
	NNSG2dScreenData*	p_scrn[ WFLBY_ROOM_UNDERWIN_BTTN_ANM_NUM ];
} WFLBY_GADGET_BTTN;

//-------------------------------------
///	トレーナーカード
//=====================================
typedef struct {
	u32 aikotoba_draw;
	
	// ウィンドウ
	GF_BGL_BMPWIN		win[WFLBY_TRCARD_WIN_NUM];


	// ↓以下画面を表示するたびに更新をかけるもの
	// アイテムアイコン
	CLACT_WORK_PTR		p_itemicon;
	CLACT_U_RES_OBJ_PTR p_itemiconres[WFLBY_ROOM_OAM_RESNUM];

	CLACT_WORK_PTR		p_tr;
	CLACT_U_RES_OBJ_PTR p_trres[WFLBY_ROOM_OAM_RESNUM];


	// 履歴リソース
	CLACT_WORK_PTR		p_rireki[WFLBY_LASTACT_BUFFNUM];
	CLACT_U_RES_OBJ_PTR p_rirekires_pal[WFLBY_TRCARD_RIREKI_RES_NUM];
	CLACT_U_RES_OBJ_PTR p_rirekires_cg[WFLBY_LASTACT_BUFFNUM];
	CLACT_U_RES_OBJ_PTR p_rirekires_cell[WFLBY_TRCARD_RIREKI_RES_NUM];
	CLACT_U_RES_OBJ_PTR p_rirekires_anm[WFLBY_TRCARD_RIREKI_RES_NUM];

} WFLBY_TR_CARD;

//-------------------------------------
///	下画面　ワーク
//=====================================
typedef struct {
	u8					seq;			// 動作シーケンス
	u8					now;			// 今の描画状態
	u8					req;			// 描画変更リクエスト
	u8					req_tridx;		// リクエストを受けたトレーナIDX
	u16					req_aikotoba;	// 合言葉をひょうじするのか
	u16					bttn_start;		// 入り口でガジェット取得後TRUEになる
	WFLBY_TR_CARD		tr_card;
	WFLBY_GADGET_BTTN	bttn;
	ARCHANDLE*			p_handle;

	// フロートアニメ終了チェック用
	u16		flt_float_idx;
	u8		flt_anm_idx;
	u8		flt_anm_flag;

	// VBlankパレット転送データ
	// VBLANK転送タスク内でのみ使用可能	その他の場所では不定値
	u16					dataidx;
	u16					heapID;
} WFLBY_UNDER_WIN;

//-------------------------------------
///	各NPCローカル状態での反応可能フラグ
//=====================================
typedef struct {
	u8 talk_busy[ WFLBY_PLAYER_MAX ];			// 会話状態BUSY
	u8 info_draw_busy[ WFLBY_PLAYER_MAX ];		// 情報の表示BUSY
} WFLBY_NPC_BUSY;

//-------------------------------------
///	広場ワーク
//=====================================
typedef struct _WFLBY_ROOMWK{
	// 各種フラグ
	u8 end_flag;		// 終了フラグ
	u8 end_ret;			// WFLBY_ROOM_RETこれ
	u8 plno;			// プレイヤーナンバー
	u8 fade_flag;		// フェード中TRUE
	u8 err_check:4;		// 通信エラーをチェックするか
	u8 err_flag:4;		// 通信エラーフラグ
	u8 evno;			// 最近起動したイベント		イベントシステム内でのチェック用
	u8 timeout;			// タイムアウト
	u8 timeout_off;		// タイムアウトチェックを行うかのフラグ
	s32 msg_wait;

#if PL_T0867_080716_FIX
	// 主人公が、イベント後、移動する可能性があるのか？というフラグ
	// 他人が会話ジャンプを行うことができるのかに使用
	BOOL player_ev_after_move;
#endif

	// 各NPC反応可能フラグ
	WFLBY_NPC_BUSY npc_busy;
 
	// システム情報
	WFLBY_SYSTEM* p_system;

	// 広場セーブデータ
	WFLBY_ROOM_SAVE* p_save;

	// グラフィックシステム初期化
	WFLBY_GRAPHICCONT	graphic;

	// した画面
	WFLBY_UNDER_WIN		under_win;

	// メッセージシステム
	WFLBY_ROOM_DEFMSG	def_msg;

	// ウィンドウ
	WFLBY_ROOM_TALKMSG	talkwin;	// 会話ウィンドウ
	WFLBY_ROOM_TALKMSG	boardwin;	// 看板ウィンドウ
	WFLBY_ROOM_LISTWIN	listwin;	// リストウィンドウ
	WFLBY_ROOM_ERRMSG	errwin;		// エラーウィンドウ
	WFLBY_ROOM_YESNOWIN yesnowin;	// YESNOウィンドウ
	WFLBY_ROOM_SUBWIN	subwin;		// サブウィンドウ

	// ガジェット
	WFLBY_GADGET*			p_gadget;
	BOOL					gadget_flag;	// ガジェット動作フラグ

	// タイムイベント
	WFLBY_TIMEEVENT*		p_timeevent;

	// 各管理システム
	WFLBY_3DOBJCONT*		p_objcont;
	WFLBY_3DMAPOBJ_CONT*	p_mapobjcont;
	WFLBY_MAPCONT*			p_mapcont;
	WFLBY_CAMERA*			p_camera;
	WFLBY_EVENT*			p_event;

	// セーブデータ
	MYSTATUS*			p_mystatus;
} WFLBY_ROOMWK;



//-----------------------------------------------------------------------------
/**
 *				データテーブル
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	グラフィック関連
//=====================================
// エッジマーキング設定
//エッジマーキングテーブル
static const GXRgb sc_WFLBY_ROOM_EDGECOLOR[8] = {
    GX_RGB(0, 0, 0),
    GX_RGB(4, 4, 4),
    GX_RGB(4, 4, 4),
    GX_RGB(4, 4, 4),
    GX_RGB(4, 4, 4),
    GX_RGB(4, 4, 4),
    GX_RGB(4, 4, 4),
    GX_RGB(4, 4, 4)
};
// バンク設定
static const GF_BGL_DISPVRAM sc_WFLBY_ROOM_BANK = {
//	GX_VRAM_BG_32_FG,				// メイン2DエンジンのBG
	GX_VRAM_BG_128_B,				// メイン2DエンジンのBG
	GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット
	GX_VRAM_SUB_BG_32_H,			// サブ2DエンジンのBG
	GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット
//	GX_VRAM_OBJ_128_B,				// メイン2DエンジンのOBJ
	GX_VRAM_OBJ_32_FG,				// メイン2DエンジンのOBJ
	GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット
	GX_VRAM_SUB_OBJ_16_I,			// サブ2DエンジンのOBJ
	GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット
	GX_VRAM_TEX_01_AC,				// テクスチャイメージスロット
	GX_VRAM_TEXPLTT_0123_E			// テクスチャパレットスロット
};

// BG設定
static const GF_BGL_SYS_HEADER sc_BGINIT = {
	GX_DISPMODE_GRAPHICS,
	GX_BGMODE_0,
	GX_BGMODE_0,
	GX_BG0_AS_3D
};

// BGコントロール
static const u32 sc_WFLBY_ROOM_BGCNT_FRM[ WFLBY_ROOM_BGCNT_NUM ] = {
	GF_BGL_FRAME1_M,
	GF_BGL_FRAME0_S,
	GF_BGL_FRAME1_S,
	GF_BGL_FRAME2_S,
};
static const GF_BGL_BGCNT_HEADER sc_WFLBY_ROOM_BGCNT_DATA[ WFLBY_ROOM_BGCNT_NUM ] = {
	// メイン画面
	{	// GF_BGL_FRAME1_M
		0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
		0, 0, 0, FALSE
	},

	// サブ画面
	{	// GF_BGL_FRAME0_S
		0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
		2, 0, 0, FALSE
	},
	{	// GF_BGL_FRAME1_S
		0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
		1, 0, 0, FALSE
	},
	{	// GF_BGL_FRAME2_S
		0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
		0, 0, 0, FALSE
	},
};

// OAM設定
static const CHAR_MANAGER_MAKE sc_WFLBY_ROOM_CHARMAN_INIT = {
	WFLBY_ROOM_OAM_CONTNUM,
	32*1024,	// 128K->リストのカーソルをフィールドと共通で使用するので32Kにする
	16*1024,	// 16K
	HEAPID_WFLBY_ROOM
};


// YESNOWINデータ
static const BMPLIST_HEADER sc_WFLBY_ROOM_YESNO_HEADER = {
	NULL,
	NULL,
	NULL,
	NULL,

	0,								//リスト項目数
	WFLBY_YESNOWIN_DATANUM,			//表示最大項目数
	0,								//ラベル表示Ｘ座標
	8,								//項目表示Ｘ座標
	0,								//カーソル表示Ｘ座標
	0,								//表示Ｙ座標
	FBMP_COL_BLACK,					//表示文字色
	FBMP_COL_WHITE,					//表示背景色
	FBMP_COL_BLK_SDW,				//表示文字影色
	0,								//文字間隔Ｘ
	16,								//文字間隔Ｙ
	BMPLIST_NO_SKIP,				//ページスキップタイプ
	FONT_SYSTEM,					//文字指定(本来は u8 だけど、そんなに作らないと思うので)
	0,								//ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)

	NULL
};



//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_VBlank( void* p_work );

static void WFLBY_ROOM_GraphicInit( WFLBY_GRAPHICCONT* p_sys, SAVEDATA* p_save, u32 heapID );
static void WFLBY_ROOM_GraphicMain( WFLBY_GRAPHICCONT* p_sys );
static void WFLBY_ROOM_GraphicExit( WFLBY_GRAPHICCONT* p_sys );
static void WFLBY_ROOM_GraphicVblank( WFLBY_GRAPHICCONT* p_sys );
static void WFLBY_ROOM_DrawSys3DSetUp( void );

static void WFLBY_ROOM_RoomMain( WFLBY_ROOMWK* p_wk );
static void WFLBY_ROOM_RoomDraw( WFLBY_ROOMWK* p_wk );

static void WFLBY_ROOM_MapAnmCont( WFLBY_ROOMWK* p_wk );

static void WFLBY_ROOM_TalkWin_Init( WFLBY_ROOM_TALKMSG* p_wk, WFLBY_GRAPHICCONT* p_sys, SAVEDATA* p_save, u32 heapID );
static void WFLBY_ROOM_TalkWin_Exit( WFLBY_ROOM_TALKMSG* p_wk );
static void WFLBY_ROOM_TalkWin_Print( WFLBY_ROOM_TALKMSG* p_wk, const STRBUF* cp_str );
static void WFLBY_ROOM_TalkWin_PrintAll( WFLBY_ROOM_TALKMSG* p_wk, const STRBUF* cp_str );
static void WFLBY_ROOM_TalkWin_StartTimeWait( WFLBY_ROOM_TALKMSG* p_wk );
static void WFLBY_ROOM_TalkWin_StopTimeWait( WFLBY_ROOM_TALKMSG* p_wk );
static void WFLBY_ROOM_TalkWin_StopTimeWait_NoTrans( WFLBY_ROOM_TALKMSG* p_wk );
static BOOL WFLBY_ROOM_TalkWin_CheckTimeWait( const WFLBY_ROOM_TALKMSG* cp_wk );
static BOOL WFLBY_ROOM_TalkWin_EndWait( const WFLBY_ROOM_TALKMSG* cp_wk );
static void WFLBY_ROOM_TalkWin_Off( WFLBY_ROOM_TALKMSG* p_wk );

static void WFLBY_ROOM_TalkWin_Board_Init( WFLBY_ROOM_TALKMSG* p_wk, WFLBY_GRAPHICCONT* p_sys, SAVEDATA* p_save, u32 heapID );
static void WFLBY_ROOM_TalkWin_Board_Exit( WFLBY_ROOM_TALKMSG* p_wk );
static void WFLBY_ROOM_TalkWin_Board_Print( WFLBY_ROOM_TALKMSG* p_wk, const STRBUF* cp_str );
static void WFLBY_ROOM_TalkWin_Board_PrintAll( WFLBY_ROOM_TALKMSG* p_wk, const STRBUF* cp_str );
static void WFLBY_ROOM_TalkWin_Board_Off( WFLBY_ROOM_TALKMSG* p_wk );

static void WFLBY_ROOM_TalkWin_Board_ClearWin( WFLBY_ROOM_TALKMSG* p_wk );
static void WFLBY_ROOM_TalkWin_Board_PrintSimple( WFLBY_ROOM_TALKMSG* p_wk, u8 x, u8 y, const STRBUF* cp_str );
static void WFLBY_ROOM_TalkWin_Board_OnVReq( WFLBY_ROOM_TALKMSG* p_wk );
static void WFLBY_ROOM_TalkWin_Board_SetPalNo( WFLBY_ROOM_TALKMSG* p_wk, u16 pal );


static void WFLBY_ROOM_ListWin_Init( WFLBY_ROOM_LISTWIN* p_wk, WFLBY_GRAPHICCONT* p_sys, u32 heapID );
static void WFLBY_ROOM_ListWin_Exit( WFLBY_ROOM_LISTWIN* p_wk, WFLBY_GRAPHICCONT* p_sys );
static void WFLBY_ROOM_ListWin_CreateBmpList( WFLBY_ROOM_LISTWIN* p_wk, u32 num, u32 heapID , u32 a3);
static void WFLBY_ROOM_ListWin_DeleteBmpList( WFLBY_ROOM_LISTWIN* p_wk );
static void WFLBY_ROOM_ListWin_SetBmpListStr( WFLBY_ROOM_LISTWIN* p_wk, const STRBUF* cp_str, u32 param );
static const  BMP_MENULIST_DATA* WFLBY_ROOM_ListWin_GetBmpList( const WFLBY_ROOM_LISTWIN* cp_wk );
static BOOL WFLBY_ROOM_ListWin_CheckBmpListParam( const WFLBY_ROOM_LISTWIN* cp_wk, u32 param );
static void WFLBY_ROOM_ListWin_Start( WFLBY_ROOM_LISTWIN* p_wk, const BMPLIST_HEADER* cp_data, WFLBY_GRAPHICCONT* p_sys, u16 list_p, u16 cursor_p, u32 heapID, u8 cx, u8 cy, u8 szcx );
static u32 WFLBY_ROOM_ListWin_Main( WFLBY_ROOM_LISTWIN* p_wk );
static void WFLBY_ROOM_ListWin_End( WFLBY_ROOM_LISTWIN* p_wk, u16* p_list_p, u16* p_cursor_p );
static void WFLBY_ROOM_ListWin_CurCallBack( BMPLIST_WORK* p_wk,u32 param,u8 mode );
static void WFLBY_ROOM_ListWin_SetClactDraw( WFLBY_ROOM_LISTWIN* p_wk, BOOL flag );
static void WFLBY_ROOM_ListWin_ContClactDraw( WFLBY_ROOM_LISTWIN* p_wk );

static void WFLBY_ROOM_SubWin_Init( WFLBY_ROOM_SUBWIN* p_wk, WFLBY_GRAPHICCONT* p_sys, u32 heapID );
static void WFLBY_ROOM_SubWin_Exit( WFLBY_ROOM_SUBWIN* p_wk );
static void WFLBY_ROOM_SubWin_Start( WFLBY_ROOM_SUBWIN* p_wk, WFLBY_GRAPHICCONT* p_sys, u32 heapID, u8 cx, u8 cy, u8 szcx, u8 szcy );
static void WFLBY_ROOM_SubWin_End( WFLBY_ROOM_SUBWIN* p_wk );
static void WFLBY_ROOM_SubWin_PrintAll( WFLBY_ROOM_SUBWIN* p_wk, const STRBUF* cp_str, u8 x, u8 y );
static void WFLBY_ROOM_SubWin_Clear( WFLBY_ROOM_SUBWIN* p_wk );
static void WFLBY_ROOM_SubWin_ClearRect( WFLBY_ROOM_SUBWIN* p_wk, u16 x, u16 y, u16 sx, u16 sy );



static void WFLBY_ROOM_YesNoWin_Init( WFLBY_ROOM_YESNOWIN* p_wk, WFLBY_ROOM_DEFMSG* p_msg, u32 heapID );
static void WFLBY_ROOM_YesNoWin_Exit( WFLBY_ROOM_YESNOWIN* p_wk );


static void WFLBY_ROOM_ErrWin_Init( WFLBY_ROOM_ERRMSG* p_wk, WFLBY_GRAPHICCONT* p_sys, u32 heapID );
static void WFLBY_ROOM_ErrWin_Exit( WFLBY_ROOM_ERRMSG* p_wk );
static void WFLBY_ROOM_ErrWin_DrawErr( WFLBY_ROOM_ERRMSG* p_wk, const STRBUF* cp_str );
static void WFLBY_ROOM_ErrWin_DrawDwcErr( WFLBY_ROOM_ERRMSG* p_wk, WFLBY_ROOM_DEFMSG* p_msg );
static void WFLBY_ROOM_ErrWin_DrawLobbyErr( WFLBY_ROOM_ERRMSG* p_wk, WFLBY_ROOM_DEFMSG* p_msg, int errno );
static void WFLBY_ROOM_ErrWin_DrawSystemErr( WFLBY_ROOM_ERRMSG* p_wk, WFLBY_ROOM_DEFMSG* p_msg, WFLBY_SYSTEM_ERR_TYPE type );


static void WFLBY_ROOM_Msg_Init( WFLBY_ROOM_DEFMSG* p_wk, u32 heapID );
static void WFLBY_ROOM_Msg_Exit( WFLBY_ROOM_DEFMSG* p_wk );
static STRBUF* WFLBY_ROOM_Msg_Get( WFLBY_ROOM_DEFMSG* p_wk, WFLBY_DEFMSG_TYPE type, u32 strid );
static void WFLBY_ROOM_Msg_SetNumber( WFLBY_ROOM_DEFMSG* p_wk, u32  num, u32 keta, u32 bufid, NUMBER_DISPTYPE disptype );
static void WFLBY_ROOM_Msg_SetPlayerName( WFLBY_ROOM_DEFMSG* p_wk, const MYSTATUS* cp_mystatus, u32 bufid );
static void WFLBY_ROOM_Msg_SetMinigame( WFLBY_ROOM_DEFMSG* p_wk, WFLBY_GAMETYPE type, u32 bufid );
static void WFLBY_ROOM_Msg_SetTimeevent( WFLBY_ROOM_DEFMSG* p_wk, WFLBY_EVENTGMM_TYPE type, u32 bufid );
static void WFLBY_ROOM_Msg_SetItem( WFLBY_ROOM_DEFMSG* p_wk, WFLBY_ITEMTYPE item, u32 bufid );
static void WFLBY_ROOM_Msg_SetCountry( WFLBY_ROOM_DEFMSG* p_wk, u32 country, u32 bufid );
static void WFLBY_ROOM_Msg_SetArea( WFLBY_ROOM_DEFMSG* p_wk, u32 country, u32 area, u32 bufid );
static void WFLBY_ROOM_Msg_SetPMSWord( WFLBY_ROOM_DEFMSG* p_wk, u32 bufID, PMS_WORD word );
static void WFLBY_ROOM_Msg_ClearWordSet( WFLBY_ROOM_DEFMSG* p_wk );


static void WFLBY_ROOM_GadGet_SetFlag( WFLBY_ROOMWK* p_wk );

//-------------------------------------
///	下画面
//=====================================
static void WFLBY_ROOM_UNDERWIN_Init( WFLBY_UNDER_WIN* p_wk, const WFLBY_ROOM_SAVE* cp_save, WFLBY_GRAPHICCONT* p_sys, const MYSTATUS* cp_mystatus, u32 heapID );
static void WFLBY_ROOM_UNDERWIN_Exit( WFLBY_UNDER_WIN* p_wk, WFLBY_ROOM_TALKMSG* p_boardwin, WFLBY_GRAPHICCONT* p_sys );
static void WFLBY_ROOM_UNDERWIN_Main( WFLBY_UNDER_WIN* p_wk, WFLBY_ROOMWK* p_room, BOOL no_input, u32 heapID );
static BOOL WFLBY_ROOM_UNDERWIN_CheckFade( const WFLBY_UNDER_WIN* cp_wk );
static u32 WFLBY_ROOM_UNDERWIN_GetSeq( const WFLBY_UNDER_WIN* cp_wk );
static void WFLBY_ROOM_UNDERWIN_ReqTrCard( WFLBY_UNDER_WIN* p_wk, u32 idx, BOOL aikotoba );
static void WFLBY_ROOM_UNDERWIN_ChangeItemTrCard( WFLBY_UNDER_WIN* p_wk, WFLBY_GRAPHICCONT* p_sys, u32 item, u32 heapID );
static void WFLBY_ROOM_UNDERWIN_ReqBttn( WFLBY_UNDER_WIN* p_wk );
static void WFLBY_ROOM_UNDERWIN_ReqBttnFloat( WFLBY_UNDER_WIN* p_wk );
static u32 WFLBY_ROOM_UNDERWIN_GetTrCardPlIdx( const WFLBY_UNDER_WIN* cp_wk );
static BOOL WFLBY_ROOM_UNDERWIN_CheckTrCardDraw( const WFLBY_UNDER_WIN* cp_wk );
static BOOL WFLBY_ROOM_UNDERWIN_CheckTrCardCanOff( const WFLBY_UNDER_WIN* cp_wk );
static BOOL WFLBY_ROOM_UNDERWIN_CheckTrCardAikotobaDraw( const WFLBY_UNDER_WIN* cp_wk );
#if PL_T0862_080712_FIX
static BOOL WFLBY_ROOM_UNDERWIN_CheckReqNone( const WFLBY_UNDER_WIN* cp_wk );
#endif
static void WFLBY_ROOM_UNDERWIN_StartTrCard( WFLBY_UNDER_WIN* p_wk, WFLBY_ROOM_TALKMSG* p_boardwin, WFLBY_GRAPHICCONT* p_sys, WFLBY_ROOM_DEFMSG* p_msg, BOOL mydata, u32 heapID, const WFLBY_USER_PROFILE* cp_profile, const WFLBY_USER_PROFILE* cp_myprofile, BOOL vip, BOOL aikotoba, const WFLBY_AIKOTOBA_DATA* cp_aikotoba, BOOL draw_item );
static void WFLBY_ROOM_UNDERWIN_EndTrCard( WFLBY_UNDER_WIN* p_wk, WFLBY_ROOM_TALKMSG* p_boardwin, WFLBY_GRAPHICCONT* p_sys );
static void WFLBY_ROOM_UNDERWIN_StartBttn( WFLBY_UNDER_WIN* p_wk, WFLBY_GRAPHICCONT* p_sys, WFLBY_ROOM_DEFMSG* p_msg, u32 heapID, const WFLBY_USER_PROFILE* cp_profile );
static void WFLBY_ROOM_UNDERWIN_StartBttnFloat( WFLBY_UNDER_WIN* p_wk, WFLBY_GRAPHICCONT* p_sys, u32 heapID, u32 idx );
static void WFLBY_ROOM_UNDERWIN_EndBttn( WFLBY_UNDER_WIN* p_wk, WFLBY_GRAPHICCONT* p_sys );
static void WFLBY_ROOM_UNDERWIN_SetBttnStop( WFLBY_UNDER_WIN* p_wk, BOOL stop );
static void WFLBY_ROOM_UNDERWIN_PalTrans( WFLBY_UNDER_WIN* p_wk, WFLBY_GRAPHICCONT* p_sys, u32 dataidx, u32 heapID );
static void WFLBY_ROOM_UNDERWIN_PalTransVTcb( TCB_PTR p_tcb, void* p_work );

// common
static void WFLBY_ROOM_UNDERWIN_Common_Init( WFLBY_UNDER_WIN* p_wk, WFLBY_GRAPHICCONT* p_sys, ARCHANDLE* p_handle, u32 sex, u32 heapID );
static void WFLBY_ROOM_UNDERWIN_Common_Exit( WFLBY_UNDER_WIN* p_wk, WFLBY_GRAPHICCONT* p_sys );
static void WFLBY_ROOM_UNDERWIN_Common_LoadScrn( WFLBY_GRAPHICCONT* p_sys, ARCHANDLE* p_handle, u32 scrnid, u32 bg_frame, u32 cgx, u32 heapID );

// tr_card
static void WFLBY_ROOM_UNDERWIN_TrCard_Init( WFLBY_TR_CARD* p_wk, WFLBY_GRAPHICCONT* p_sys, ARCHANDLE* p_handle, u32 heapID );
static void WFLBY_ROOM_UNDERWIN_TrCard_Exit( WFLBY_TR_CARD* p_wk, WFLBY_ROOM_TALKMSG* p_boardwin, WFLBY_GRAPHICCONT* p_sys );
static void WFLBY_ROOM_UNDERWIN_TrCard_Main( WFLBY_TR_CARD* p_wk, WFLBY_ROOM_TALKMSG* p_boardwin );
static void WFLBY_ROOM_UNDERWIN_TrCard_Start( WFLBY_UNDER_WIN* p_ugwk, WFLBY_ROOM_TALKMSG* p_boardwin, WFLBY_TR_CARD* p_wk, WFLBY_GRAPHICCONT* p_sys, WFLBY_ROOM_DEFMSG* p_msg, BOOL mydata, ARCHANDLE* p_handle, u32 heapID, const WFLBY_USER_PROFILE* cp_profile, const WFLBY_USER_PROFILE* cp_myprofile, BOOL vip, BOOL aikotoba, const WFLBY_AIKOTOBA_DATA* cp_aikotoba, BOOL item_draw );
static void WFLBY_ROOM_UNDERWIN_TrCard_WriteWazaType( WFLBY_UNDER_WIN* p_ugwk, WFLBY_TR_CARD* p_wk, WFLBY_GRAPHICCONT* p_sys, u32 heapID, const WFLBY_USER_PROFILE* cp_profile );
static void WFLBY_ROOM_UNDERWIN_TrCard_End( WFLBY_TR_CARD* p_wk, WFLBY_ROOM_TALKMSG* p_boardwin, WFLBY_GRAPHICCONT* p_sys );
static void WFLBY_ROOM_UNDERWIN_TrCard_ChangeItem( WFLBY_TR_CARD* p_wk, WFLBY_GRAPHICCONT* p_sys, ARCHANDLE* p_handle, u32 heapID, WFLBY_ITEMTYPE itemno );
static BOOL WFLBY_ROOM_UNDERWIN_TrCard_CheckAikotobaDraw( const WFLBY_TR_CARD* cp_wk );
static void WFLBY_ROOM_UNDERWIN_TrCard_ItemIcon_Init( WFLBY_TR_CARD* p_wk, WFLBY_GRAPHICCONT* p_sys, ARCHANDLE* p_handle, u32 heapID, WFLBY_ITEMTYPE itemno, BOOL draw_flag );
static void WFLBY_ROOM_UNDERWIN_TrCard_ItemIcon_Exit( WFLBY_TR_CARD* p_wk, WFLBY_GRAPHICCONT* p_sys );
static void WFLBY_ROOM_UNDERWIN_TrCard_TrView_Init( WFLBY_TR_CARD* p_wk, WFLBY_GRAPHICCONT* p_sys, ARCHANDLE* p_handle, u32 heapID, u32 trtype );
static void WFLBY_ROOM_UNDERWIN_TrCard_TrView_Exit( WFLBY_TR_CARD* p_wk, WFLBY_GRAPHICCONT* p_sys );
static void WFLBY_ROOM_UNDERWIN_TrCard_TrView_PallColorChange( NNSG2dPaletteData* p_pltt );
static void WFLBY_ROOM_UNDERWIN_TrCard_WinClear( WFLBY_TR_CARD* p_wk );
static void WFLBY_ROOM_UNDERWIN_TrCard_WinPrint( WFLBY_TR_CARD* p_wk, WFLBY_ROOM_DEFMSG* p_msg, u32 winno, u32 strid, u8 x, u8 y, GF_PRINTCOLOR col );
static void WFLBY_ROOM_UNDERWIN_TrCard_WinPrintRightSide( WFLBY_TR_CARD* p_wk, WFLBY_ROOM_DEFMSG* p_msg, u32 winno, u32 strid, u8 x, u8 y, GF_PRINTCOLOR col );
static void WFLBY_ROOM_UNDERWIN_TrCard_WinOn( WFLBY_TR_CARD* p_wk, u32 idx );
static void WFLBY_ROOM_UNDERWIN_TrCard_WinSetName( WFLBY_TR_CARD* p_wk, WFLBY_ROOM_DEFMSG* p_msg,const MYSTATUS* cp_mystatus );
static void WFLBY_ROOM_UNDERWIN_TrCard_WinSetCountry( WFLBY_TR_CARD* p_wk, WFLBY_ROOM_DEFMSG* p_msg,const WFLBY_USER_PROFILE* cp_profile );
static void WFLBY_ROOM_UNDERWIN_TrCard_WinSetLastAction( WFLBY_TR_CARD* p_wk, WFLBY_ROOM_DEFMSG* p_msg,const WFLBY_USER_PROFILE* cp_profile );
static void WFLBY_ROOM_UNDERWIN_TrCard_WinSetAikotoba( WFLBY_TR_CARD* p_wk, WFLBY_ROOM_DEFMSG* p_msg, const WFLBY_AIKOTOBA_DATA* cp_aikotoba );
static void WFLBY_ROOM_UNDERWIN_TrCard_InitRireki( WFLBY_TR_CARD* p_wk, WFLBY_GRAPHICCONT* p_sys, ARCHANDLE* p_handle, u32 heapID, const WFLBY_USER_PROFILE* cp_profile, const WFLBY_USER_PROFILE* cp_myprofile );
static void WFLBY_ROOM_UNDERWIN_TrCard_ExitRireki( WFLBY_TR_CARD* p_wk, WFLBY_GRAPHICCONT* p_sys );
static const WFLBY_TRCARD_RIREKI_CHAR* WFLBY_ROOM_UNDERWIN_TrCard_GetRirekiData( WFLBY_LASTACTION_TYPE last_act, s32 userid, s32 myid, u32 sex );

// button
static void WFLBY_ROOM_UNDERWIN_Button_Init( WFLBY_GADGET_BTTN* p_wk, WFLBY_GRAPHICCONT* p_sys, ARCHANDLE* p_handle, u32 heapID );
static void WFLBY_ROOM_UNDERWIN_Button_Exit( WFLBY_GADGET_BTTN* p_wk );
static BOOL WFLBY_ROOM_UNDERWIN_Button_Main( WFLBY_GADGET_BTTN* p_wk, WFLBY_GRAPHICCONT* p_sys,  ARCHANDLE* p_handle, u32 heapID );
static void WFLBY_ROOM_UNDERWIN_Button_Start( WFLBY_GADGET_BTTN* p_wk, WFLBY_GRAPHICCONT* p_sys, WFLBY_ROOM_DEFMSG* p_msg, const WFLBY_USER_PROFILE* cp_profile, ARCHANDLE* p_handle, u32 heapID );
static void WFLBY_ROOM_UNDERWIN_Button_StartFloat( WFLBY_GADGET_BTTN* p_wk, WFLBY_GRAPHICCONT* p_sys, u32 idx, ARCHANDLE* p_handle, u32 heapID );
static void WFLBY_ROOM_UNDERWIN_Button_End( WFLBY_GADGET_BTTN* p_wk, WFLBY_GRAPHICCONT* p_sys );
static void WFLBY_ROOM_UNDERWIN_Button_DrawButton( WFLBY_GADGET_BTTN* p_wk, WFLBY_GRAPHICCONT* p_sys, u32 idx, ARCHANDLE* p_handle, u32 heapID );
static void WFLBY_ROOM_UNDERWIN_Button_BttnCallBack( u32 bttnno, u32 event, void* p_work );
static void WFLBY_ROOM_UNDERWIN_Button_BttnLockClear( WFLBY_GADGET_BTTN* p_wk );


#ifdef PM_DEBUG
PROC_RESULT WFLBY_ROOM_InitDebug(PROC* p_proc, int* p_seq)
{
	FS_EXTERN_OVERLAY(wifilobby_common);
	FS_EXTERN_OVERLAY(wifi_2dmapsys);

	Overlay_Load( FS_OVERLAY_ID(wifilobby_common), OVERLAY_LOAD_NOT_SYNCHRONIZE);
	Overlay_Load( FS_OVERLAY_ID(wifi_2dmapsys), OVERLAY_LOAD_NOT_SYNCHRONIZE);

	return WFLBY_ROOM_Init( p_proc, p_seq );
}

PROC_RESULT WFLBY_ROOM_ExitDebug(PROC* p_proc, int* p_seq)
{
	BOOL result;
	
	FS_EXTERN_OVERLAY(wifilobby_common);
	FS_EXTERN_OVERLAY(wifi_2dmapsys);

	result  = WFLBY_ROOM_Exit( p_proc, p_seq );

	Overlay_UnloadID( FS_OVERLAY_ID(wifilobby_common) );
	Overlay_UnloadID( FS_OVERLAY_ID(wifi_2dmapsys) );

	return result;
}
#endif


//----------------------------------------------------------------------------
/**
 *	@brief	部屋システム	初期化
 *
 *	@param	p_proc		プロセスワーク
 *	@param	p_seq		シーケンス
 *
 *	@retval	PROC_RES_CONTINUE = 0,		///<動作継続中
 *	@retval	PROC_RES_FINISH,			///<動作終了
 */
//-----------------------------------------------------------------------------
PROC_RESULT WFLBY_ROOM_Init(PROC* p_proc, int* p_seq)
{
	WFLBY_ROOMWK* p_wk;
	WFLBY_ROOM_PARAM* p_param;

#ifdef WFLBY_DEBUG_ROOM_CAMERA
	OS_TPrintf( "■カメラ操作\n" );
	OS_TPrintf( "	正射影と透視射影を切り替える	SELECT\n" );
#endif

#ifdef WFLBY_DEBUG_ROOM_ITEMCHG
//	OS_TPrintf( "■タッチトイ操作" );
//	OS_TPrintf( " タッチトイを次のものに変える	L\n" );
#endif

	OS_TPrintf( "sub char max %d\n", WFLBY_TRCARD_WIN_VIPAIKOTOBA_CGX );

	// パラメータ取得
	p_param = PROC_GetParentWork( p_proc );

	// ヒープ作成
	sys_CreateHeap( HEAPID_BASE_APP, HEAPID_WFLBY_ROOM, 0x3a000 );
	sys_CreateHeap( HEAPID_BASE_APP, HEAPID_WFLBY_ROOMGRA, 0x3d000 );

	// ワーク作成
	p_wk = PROC_AllocWork( p_proc, sizeof(WFLBY_ROOMWK), HEAPID_WFLBY_ROOM );
	memset( p_wk, 0, sizeof(WFLBY_ROOMWK) );

	// 部屋保存データ設定先を保存
	p_wk->p_save = &p_param->save;

	// システム情報を取得
	p_wk->p_system = p_param->p_system;

	// プレイヤーナンバー設定
	p_wk->plno = WFLBY_SYSTEM_GetMyIdx( p_wk->p_system );
	GF_ASSERT( WFLBY_SYSTEM_GetMyIdx( p_wk->p_system ) != DWC_LOBBY_USERIDTBL_IDX_NONE );

	// メッセージ表示関係を設定
	MsgPrintSkipFlagSet(MSG_SKIP_ON);
	MsgPrintAutoFlagSet(MSG_AUTO_OFF);
	MsgPrintTouchPanelFlagSet(MSG_TP_OFF);


	// セーブデータ設定
	p_wk->p_mystatus = SaveData_GetMyStatus( p_param->p_save );

	// グラフィックシステム初期化
	WFLBY_ROOM_GraphicInit( &p_wk->graphic, p_param->p_save, HEAPID_WFLBY_ROOM );

	// マップ管理システム作成
	p_wk->p_mapcont = WFLBY_MAPCONT_Init( HEAPID_WFLBY_ROOM );


	// オブジェクト管理システム作成
	{
		u32 sex;
		sex = MyStatus_GetMySex( p_wk->p_mystatus );
		p_wk->p_objcont = WFLBY_3DOBJCONT_Init( WFLBY_ROOM_3DOBJCONT_WKNUM, sex, 
				p_wk->p_mapcont, HEAPID_WFLBY_ROOM, HEAPID_WFLBY_ROOMGRA );	

	}

	// マップオブジェ管理システム作成
	p_wk->p_mapobjcont = WFLBY_3DMAPOBJCONT_Init( p_param->season, p_param->room, p_wk->p_mapcont, HEAPID_WFLBY_ROOM, HEAPID_WFLBY_ROOMGRA );
	

	// カメラ初期化
	p_wk->p_camera = WFLBY_CAMERA_Init( HEAPID_WFLBY_ROOM );

	// ウィンドウ
	WFLBY_ROOM_TalkWin_Init( &p_wk->talkwin, &p_wk->graphic, p_param->p_save, HEAPID_WFLBY_ROOM );
	WFLBY_ROOM_TalkWin_Board_Init( &p_wk->boardwin, &p_wk->graphic, p_param->p_save, HEAPID_WFLBY_ROOM );
	WFLBY_ROOM_ListWin_Init( &p_wk->listwin, &p_wk->graphic, HEAPID_WFLBY_ROOM );
	WFLBY_ROOM_ErrWin_Init( &p_wk->errwin, &p_wk->graphic, HEAPID_WFLBY_ROOM );
	WFLBY_ROOM_SubWin_Init( &p_wk->subwin, &p_wk->graphic, HEAPID_WFLBY_ROOM );

	// エラーメッセージ
	WFLBY_ROOM_Msg_Init( &p_wk->def_msg, HEAPID_WFLBY_ROOM );

	// YESNOウィンドウ
	WFLBY_ROOM_YesNoWin_Init( &p_wk->yesnowin, &p_wk->def_msg, HEAPID_WFLBY_ROOM );


	// した画面初期化
	WFLBY_ROOM_UNDERWIN_Init( &p_wk->under_win, p_wk->p_save, &p_wk->graphic, p_wk->p_mystatus, HEAPID_WFLBY_ROOM );

	// タッチトイを表示できる状態なら表示する
	{
		const WFLBY_USER_PROFILE* cp_profile;

		cp_profile = WFLBY_SYSTEM_GetMyProfileLocal( p_wk->p_system );
		if( WFLBY_SYSTEM_CheckGetItem( cp_profile ) == TRUE ){
			WFLBY_ROOM_UNDERWIN_StartBttn( &p_wk->under_win, &p_wk->graphic, &p_wk->def_msg,
					HEAPID_WFLBY_ROOM, 
					cp_profile );
		}
	}

	// イベントシステム作成
	p_wk->p_event = WFLBY_EVENT_Init( p_wk, HEAPID_WFLBY_ROOM );

	// ゲームイベント開始
	WFLBY_EV_DEF_StartPlayer( p_wk );
	WFLBY_EV_DEF_StartNpc( p_wk );

	// ガジェット初期化
	p_wk->p_gadget = WFLBY_GADGET_Init( p_wk->p_system, p_wk->p_mapcont, p_wk->p_camera, p_wk->p_objcont, HEAPID_WFLBY_ROOM, HEAPID_WFLBY_ROOMGRA );

	// タイムイベント初期化
	p_wk->p_timeevent = WFLBY_TIMEEVENT_Init( HEAPID_WFLBY_ROOM, p_wk );

	// VBlank関数設定
	sys_VBlankFuncChange( WFLBY_ROOM_VBlank, p_wk );
	sys_HBlankIntrStop();	//HBlank割り込み停止
	
	return	PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	部屋システム	メイン
 */	
//-----------------------------------------------------------------------------
PROC_RESULT WFLBY_ROOM_Main(PROC* p_proc, int* p_seq)
{
	WFLBY_ROOMWK* p_wk;
	WFLBY_ROOM_PARAM* p_param;
	BOOL result;
	
	p_wk	= PROC_GetWork( p_proc );
	p_param = PROC_GetParentWork( p_proc );

	switch( *p_seq ){
	case WFLBY_ROOM_MAINSEQ_FADEIN:
		WIPE_SYS_Start(WIPE_PATTERN_WMS, 
				WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
				WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_WFLBY_ROOM );
		p_wk->fade_flag = TRUE;
		(*p_seq) ++;
		break;
		
	case WFLBY_ROOM_MAINSEQ_FADEIN_WAIT:
		result = WIPE_SYS_EndCheck();
		if( result == TRUE ){
			p_wk->fade_flag = FALSE;
			(*p_seq) ++;
		}
		break;
		
	case WFLBY_ROOM_MAINSEQ_MAIN:

#ifdef WFLBY_DEBUG_ROOM_WLDTIMER_AUTO
		if( WFLBY_DEBUG_ROOM_WFLBY_TIMER_AUTO == TRUE ){
			sys.trg		|= PAD_KEY_RIGHT;
			sys.cont	|= PAD_KEY_RIGHT;

			if( (gf_mtRand() % 200) == 0 ){
				sys.trg		|= PAD_BUTTON_A;
				sys.cont	|= PAD_BUTTON_A;
			}
		}
#endif

#ifdef WFLBY_DEBUG_ROOM_MINIGAME_AUTO
		if( WFLBY_DEBUG_ROOM_MINIGAME_AUTO_FLAG == TRUE ){
			if( (gf_mtRand() % 10) == 0 ){
				sys.trg		|= (PAD_KEY_LEFT | PAD_BUTTON_A);
				sys.cont	|= PAD_KEY_LEFT;
			}
		}
#endif

#ifdef _WIFI_DEBUG_TUUSHIN
		{
			static int roop = 100;

			roop --;
			// 終了へ
			if( roop <= 0 ){
				(*p_seq) = WFLBY_ROOM_MAINSEQ_FADEOUT;
				break;
			}
		}
#endif
		

		// エラーが発生していたら終了
		if( p_wk->err_check == TRUE ){

			if( (WFLBY_ERR_CheckError() == TRUE) || 
				(WFLBY_SYSTEM_GetError( p_wk->p_system ) != WFLBY_SYSTEM_ERR_NONE) ){
				p_wk->err_flag = TRUE;
			}
		}

		// タイムアウト終了チェック
		if( p_wk->timeout_off == FALSE ){
			if( WFLBY_SYSTEM_Event_GetEnd( p_wk->p_system ) == TRUE ){
				// タイムアウト状態の終了
				p_wk->timeout	= TRUE;
				WFLBY_ROOM_SetEndRet( p_wk, WFLBY_ROOM_RET_ROOM_TIMEOUT );
			}
		}

		// 終了チェック
		if( (p_wk->end_flag == TRUE) || (p_wk->err_flag == TRUE) || (p_wk->timeout == TRUE) ){

			// エラー状態
			if( p_wk->err_flag ){
				(*p_seq) = WFLBY_ROOM_MAINSEQ_ERR_MSG;

			// タイムアウト状態
			}else if( p_wk->timeout == TRUE ){
				(*p_seq) = WFLBY_ROOM_MAINSEQ_TIMEOUT_MSG;

			// 通常終了
			}else{
				(*p_seq) = WFLBY_ROOM_MAINSEQ_FADEOUT;
			}
//			p_wk->fade_flag = TRUE;
		}
		break;

	// エラーメッセージ
	case WFLBY_ROOM_MAINSEQ_ERR_MSG:
		{
			// 全メッセージを消す
			WFLBY_ROOM_TalkWin_Off( &p_wk->talkwin );
			WFLBY_ROOM_ListWin_End( &p_wk->listwin, NULL, NULL );
			WFLBY_ROOM_SubWin_End( &p_wk->subwin );
			WFLBY_ROOM_YesNoWin_Exit( &p_wk->yesnowin );
			
			if( CommStateIsWifiError() ){
				// DWC系エラー
				WFLBY_ROOM_ErrWin_DrawDwcErr( &p_wk->errwin, &p_wk->def_msg);
			}
			else if( CommStateWifiLobbyError() ){
				// ロビー系エラー
				WFLBY_ROOM_ErrWin_DrawLobbyErr( &p_wk->errwin, &p_wk->def_msg, 
						DWC_LOBBY_GetErr() );
			}
			else {
				// ロビーシステム系エラー
				WFLBY_ROOM_ErrWin_DrawSystemErr( &p_wk->errwin, &p_wk->def_msg, 
					WFLBY_SYSTEM_GetError( p_wk->p_system )	);
			}
			(*p_seq) = WFLBY_ROOM_MAINSEQ_ERR_MSGWAIT;
		}
		break;
		
	// メッセージ終了待ち
	case WFLBY_ROOM_MAINSEQ_ERR_MSGWAIT:
		if( sys.trg & PAD_BUTTON_DECIDE ){
			(*p_seq) = WFLBY_ROOM_MAINSEQ_FADEOUT;
		}
		break;

	// タイムアウトメッセージ
	case WFLBY_ROOM_MAINSEQ_TIMEOUT_MSG:
		{
			// 全メッセージを消す
			WFLBY_ROOM_TalkWin_Off( &p_wk->talkwin );
			WFLBY_ROOM_ListWin_End( &p_wk->listwin, NULL, NULL );
			WFLBY_ROOM_SubWin_End( &p_wk->subwin );
			WFLBY_ROOM_YesNoWin_Exit( &p_wk->yesnowin );

			WFLBY_ROOM_TalkWin_Print( &p_wk->talkwin, 
					WFLBY_ROOM_Msg_Get( &p_wk->def_msg, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_end_01 ) );
			
			(*p_seq) = WFLBY_ROOM_MAINSEQ_TIMEOUT_MSGWAIT;

			p_wk->msg_wait = WFLBY_MSG_WAIT;
		}
		break;
		
	// タイムアウトメッセージ待ち
	case WFLBY_ROOM_MAINSEQ_TIMEOUT_MSGWAIT:
		if( WFLBY_ROOM_TalkWin_EndWait( &p_wk->talkwin ) ){
			p_wk->msg_wait --;
			if( p_wk->msg_wait <= 0 ){
				(*p_seq) = WFLBY_ROOM_MAINSEQ_FADEOUT;
			}
		}
		break;
		
	case WFLBY_ROOM_MAINSEQ_FADEOUT:
		{
			u32 seq;

			// サブ面状態でフェードの処理を分岐する
			seq = WFLBY_ROOM_UNDERWIN_GetSeq( &p_wk->under_win );

			// ノーマル状態ならワイプ開始
			if( seq == WFLBY_UNDERWIN_SEQ_NORMAL ){
				WIPE_SYS_Start(WIPE_PATTERN_WMS, 
						WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
						WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_WFLBY_ROOM );

				p_wk->fade_flag = TRUE;
				(*p_seq) ++;
				break;
			}

			// 開始待ち中ならメイン画面だけワイプ
			if( seq == WFLBY_UNDERWIN_SEQ_STARTWAIT ){
				WIPE_SYS_Start(WIPE_PATTERN_WMS, 
						WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
						WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_WFLBY_ROOM );

				p_wk->fade_flag = TRUE;
				(*p_seq) ++;
				break;
			}

			// その他の場合はいつかNORMALになる
			
		}
		break;
		
	case WFLBY_ROOM_MAINSEQ_FADEOUT_WAIT:
		result = WIPE_SYS_EndCheck();
		if( result == TRUE ){
			p_wk->fade_flag = FALSE;
			return	PROC_RES_FINISH;
		}
		break;
	}


	// エラー中タイムアウト時には更新しない
	// 部屋メイン処理
	WFLBY_ROOM_RoomMain( p_wk );

	// 部屋描画処理
	WFLBY_ROOM_RoomDraw( p_wk );

	// ライトを何段階かで変更
#ifdef WFLBY_DEBUG_ROOM_LIGHT
	{
		static const WFLBY_DEBUG_ROOM_LIGHTDATA sc_WFLBY_DEBUG_Light[ 2 ] = {
			{
				{
					GX_RGB( 23,23,25 ),
					GX_RGB( 31,31,31 ),
				},
				{
					GX_RGB( 16,16,16 ),
					GX_RGB( 14,14,14 ),
					GX_RGB( 20,20,20 ),
					GX_RGB( 16,16,16 ),
				},
			},
			{
				{
					GX_RGB( 11,11,16 ),
					GX_RGB( 31,31,31 ),
				},
				{
					GX_RGB( 14,14,16 ),
					GX_RGB( 10,10,10 ),
					GX_RGB( 14,14,16 ),
					GX_RGB( 8,8,11 ),
				},
			}
		};
		static u32 WFLBY_DEBUG_LightNum = 1;
		int i;
		
		if( sys.trg & PAD_BUTTON_R ){
			for( i=0; i<2;  i++ ){
				NNS_G3dGlbLightColor( i, sc_WFLBY_DEBUG_Light[WFLBY_DEBUG_LightNum].lightcolor[i] );
			}
			NNS_G3dGlbMaterialColorDiffAmb( sc_WFLBY_DEBUG_Light[WFLBY_DEBUG_LightNum].matcolor[0],
					sc_WFLBY_DEBUG_Light[WFLBY_DEBUG_LightNum].matcolor[1],  FALSE );
			NNS_G3dGlbMaterialColorSpecEmi( sc_WFLBY_DEBUG_Light[WFLBY_DEBUG_LightNum].matcolor[2], 
					sc_WFLBY_DEBUG_Light[WFLBY_DEBUG_LightNum].matcolor[3],  FALSE );

			WFLBY_DEBUG_LightNum = (WFLBY_DEBUG_LightNum+1) % 2;
		}
	}
#endif

	// 現状を表示
#ifdef	WFLBY_DEBUG_ROOM_DPRINT
	if( sys.trg & PAD_BUTTON_Y ){
		NNS_GfdDumpLnkTexVramManager();
		NNS_GfdDumpLnkPlttVramManager();

		OS_TPrintf( "HEAPID_WFLBY_GLB free size 0x%x\n", sys_GetHeapFreeSize( HEAPID_WFLOBBY ) );
		OS_TPrintf( "HEAPID_WFLBY_ROOM free size 0x%x\n", sys_GetHeapFreeSize( HEAPID_WFLBY_ROOM ) );
		OS_TPrintf( "HEAPID_WFLBY_ROOMGRA free size 0x%x\n", sys_GetHeapFreeSize( HEAPID_WFLBY_ROOMGRA ) );
		OS_TPrintf( "HEAPID_CONN free size 0x%x\n", sys_GetHeapFreeSize( HEAPID_COMMUNICATION ) );
		OS_TPrintf( "HEAPID_WIFIMENU free size size 0x%x\n", sys_GetHeapFreeSize( HEAPID_WIFIMENU ) );
		OS_TPrintf( "HEAPID_APP free size size 0x%x\n", sys_GetHeapFreeSize( HEAPID_BASE_APP ) );

	}
#endif

	
	return	PROC_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	部屋システム	サブ
 */
//-----------------------------------------------------------------------------
PROC_RESULT WFLBY_ROOM_Exit(PROC* p_proc, int* p_seq)
{
	WFLBY_ROOMWK* p_wk;
	WFLBY_ROOM_PARAM* p_param;
	
	p_wk	= PROC_GetWork( p_proc );
	p_param = PROC_GetParentWork( p_proc );

	// 終了データの設定
	{
		p_param->ret = p_wk->end_ret;
	}

	// 割り込み設定
	sys_VBlankFuncChange( NULL, NULL );
	sys_HBlankIntrStop();	//HBlank割り込み停止

	// タイムイベント破棄
	WFLBY_TIMEEVENT_Exit( p_wk->p_timeevent );

	// ガジェット破棄
	WFLBY_GADGET_Exit( p_wk->p_gadget );

	// イベントシステム破棄
	WFLBY_EVENT_Exit( p_wk->p_event );

	// した画面破棄
	WFLBY_ROOM_UNDERWIN_Exit( &p_wk->under_win, &p_wk->boardwin, &p_wk->graphic );

	// エラーメッセージ
	WFLBY_ROOM_Msg_Exit( &p_wk->def_msg );

	// ウィンドウ破棄
	WFLBY_ROOM_TalkWin_Exit( &p_wk->talkwin );
	WFLBY_ROOM_TalkWin_Board_Exit( &p_wk->boardwin );
	WFLBY_ROOM_ListWin_Exit( &p_wk->listwin, &p_wk->graphic );
	WFLBY_ROOM_ErrWin_Exit( &p_wk->errwin );
	WFLBY_ROOM_YesNoWin_Exit( &p_wk->yesnowin );
	WFLBY_ROOM_SubWin_Exit( &p_wk->subwin );

	// カメラ破棄
	WFLBY_CAMERA_Exit( p_wk->p_camera );

	// マップオブジェ管理システム破棄
	WFLBY_3DMAPOBJCONT_Exit( p_wk->p_mapobjcont );
	
	// オブジェクト管理システム破棄
	WFLBY_3DOBJCONT_Exit( p_wk->p_objcont );	

	// マップ管理システム破棄
	WFLBY_MAPCONT_Exit( p_wk->p_mapcont );

	// グラフィックシステム破棄
	WFLBY_ROOM_GraphicExit( &p_wk->graphic );

	// ワーク破棄
	sys_FreeMemoryEz( p_wk );

	// ヒープ破棄
	sys_DeleteHeap( HEAPID_WFLBY_ROOM );
	sys_DeleteHeap( HEAPID_WFLBY_ROOMGRA );


	// Seを全停止
	Snd_SeStopAll( 0 );
	Snd_PMVoiceStop( 0 );

	return	PROC_RES_FINISH;
}





//----------------------------------------------------------------------------
/**
 *	@brief	システムデータを取得する
 */
//-----------------------------------------------------------------------------
WFLBY_SYSTEM* WFLBY_ROOM_GetSystemData( WFLBY_ROOMWK* p_wk )
{
	return p_wk->p_system;
}

//----------------------------------------------------------------------------
/**
 *	@brief	部屋セーブデータを取得する
 */
//-----------------------------------------------------------------------------
WFLBY_ROOM_SAVE* WFLBY_ROOM_GetSaveData( WFLBY_ROOMWK* p_wk )
{
	return p_wk->p_save;
}

//----------------------------------------------------------------------------
/**
 *	@brief	人物管理システムを取得する
 */
//-----------------------------------------------------------------------------
WFLBY_3DOBJCONT* WFLBY_ROOM_GetObjCont( WFLBY_ROOMWK* p_wk )
{
	return p_wk->p_objcont;
}

//----------------------------------------------------------------------------
/**
 *	@brief	マップおぶじぇ管理システムを取得する
 */
//-----------------------------------------------------------------------------
WFLBY_3DMAPOBJ_CONT* WFLBY_ROOM_GetMapObjCont( WFLBY_ROOMWK* p_wk )
{
	return p_wk->p_mapobjcont;
}

//----------------------------------------------------------------------------
/**
 *	@brief	マップ管理システムを取得する
 */
//-----------------------------------------------------------------------------
WFLBY_MAPCONT* WFLBY_ROOM_GetMapCont( WFLBY_ROOMWK* p_wk )
{
	return p_wk->p_mapcont;
}

//----------------------------------------------------------------------------
/**
 *	@brief	カメラ管理システムを取得する
 */
//-----------------------------------------------------------------------------
WFLBY_CAMERA* WFLBY_ROOM_GetCamera( WFLBY_ROOMWK* p_wk )
{
	return p_wk->p_camera;
}

//----------------------------------------------------------------------------
/**
 *	@brief	イベント管理システムを取得する
 */
//-----------------------------------------------------------------------------
WFLBY_EVENT* WFLBY_ROOM_GetEvent( WFLBY_ROOMWK* p_wk )
{
	return p_wk->p_event;
}



//----------------------------------------------------------------------------
/**
 *	@brief	エラーチェックフラグを設定
 *
 *	@param	p_wk	ワーク
 *	@param	flag	フラグ
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_SetErrorCheckFlag( WFLBY_ROOMWK* p_wk, BOOL flag )
{
	p_wk->err_check = flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief	エラーチェックフラグを取得
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	チェックしてる
 *	@retval	FALSE	チェックしてない
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_ROOM_GetErrorCheckFlag( const WFLBY_ROOMWK* cp_wk )
{
	return cp_wk->err_check;
}


//----------------------------------------------------------------------------
/**
 *	@brief	主人公プレイヤーナンバーを取得する
 */
//-----------------------------------------------------------------------------
u32 WFLBY_ROOM_GetPlno( const WFLBY_ROOMWK* cp_wk )
{
	return cp_wk->plno;
}

//----------------------------------------------------------------------------
/**
 *	@brief	フェード中かチェック
 *	@retval	TRUE	フェード中
 *	@retval	FALSE	フェードしてない
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_ROOM_GetFadeFlag( const WFLBY_ROOMWK* cp_wk )
{
	return cp_wk->fade_flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief	終了フラグ取得
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	続行
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_ROOM_GetEndFlag( const WFLBY_ROOMWK* cp_wk )
{
	return cp_wk->end_flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ROOMを呼んだシステムに返す値
 *
 *	@return	値
 */
//-----------------------------------------------------------------------------
u8 WFLBY_ROOM_GetEndRet( const WFLBY_ROOMWK* cp_wk )
{
	return cp_wk->end_ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	終了フラグを設定する
 *
 *	@param	p_wk		ワーク
 *	@param	flag		フラグ
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_SetEndFlag( WFLBY_ROOMWK* p_wk, BOOL flag )
{
	p_wk->end_flag = flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief	終了時にROOMを呼んだシステムに返す値
 *
 *	@param	p_wk		ワーク
 *	@param	retdata		データ
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_SetEndRet( WFLBY_ROOMWK* p_wk,  u8 retdata )
{
	p_wk->end_ret = retdata;
}

//----------------------------------------------------------------------------
/**
 *	@brief	イベントナンバーを設定する
 *
 *	@param	p_wk		ワーク
 *	@param	evno		イベントナンバー
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_SetEvNo( WFLBY_ROOMWK* p_wk, u8 evno )
{
	p_wk->evno = evno;
}

//----------------------------------------------------------------------------
/**
 *	@brief	イベントナンバーを取得する
 *
 *	@param	cp_wk		ワーク
 *
 *	@return	イベントナンバー
 */
//-----------------------------------------------------------------------------
u8 WFLBY_ROOM_GetEvNo( const WFLBY_ROOMWK* cp_wk )
{
	return cp_wk->evno;
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイムアウトチェックを停止します
 *		
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_OffTimeOutCheck( WFLBY_ROOMWK* p_wk )
{
	p_wk->timeout_off = TRUE;
}

#if PL_T0867_080716_FIX
//----------------------------------------------------------------------------
/**
 *	@brief	主人公がイベント後、オートで移動する可能性があるのか？を設定
 *
 *	@param	cp_wk		ワーク
 *	@param	flag		フラグ
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_SetPlayerEventAfterMove( WFLBY_ROOMWK* p_wk, BOOL flag )
{
	p_wk->player_ev_after_move = flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief	主人公がイベント後、オートで移動する可能性があるのか？を設定
 *
 *	@param	cp_wk		ワーク
 *
 *	@retval	TRUE	移動する		他人の会話ジャンプ不可能
 *	@retval	FALSE	移動しない		他人の会話ジャンプ可能
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_ROOM_CheckPlayerEventAfterMove( const WFLBY_ROOMWK* cp_wk )
{
	return cp_wk->player_ev_after_move;
}
#endif

//----------------------------------------------------------------------------
/**
 *	@brief	NPC会話ビジー状態設定
 *
 *	@param	p_wk		ワーク
 *	@param	plno		プレイヤーナンバー
 *	@param	busy		BUSY状態	TRUE：BUSY	FALSE：NORMAL
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_SetNpcTalkBusy( WFLBY_ROOMWK* p_wk, u8 plno, BOOL busy )
{
	GF_ASSERT( plno < WFLBY_PLAYER_MAX );
	p_wk->npc_busy.talk_busy[ plno ] = busy;
}

//----------------------------------------------------------------------------
/**
 *	@brief	NPC会話可能かチェック
 *
 *	@param	cp_wk		ワーク
 *	@param	plno		プレイヤーナンバー
 *
 *	@retval	TRUE	会話不可能
 *	@retval	FALSE	会話可能
 */
//-----------------------------------------------------------------------------
BOOL WFBLY_ROOM_CheckNpcTalkBusy( const WFLBY_ROOMWK* cp_wk, u8 plno )
{
	return cp_wk->npc_busy.talk_busy[ plno ];
}

//----------------------------------------------------------------------------
/**
 *	@brief	NPC状態表示可能設定		NPCの動作を主人公側が操作できる必要もあります。
 *
 *	@param	p_wk		ワーク
 *	@param	plno		プレイヤーナンバー
 *	@param	busy		BUSY状態	TRUE:BUSY		FALSE:NORMAL
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_SetNpcInfoDrawBusy( WFLBY_ROOMWK* p_wk, u8 plno, BOOL busy )
{
	GF_ASSERT( plno < WFLBY_PLAYER_MAX );
	p_wk->npc_busy.info_draw_busy[ plno ] = busy;
}

//----------------------------------------------------------------------------
/**
 *	@brief	NPC状態表示可能かチェック	NPCの動作を主人公側が操作できる必要もあります。
 *
 *	@param	cp_wk		ワーク
 *	@param	plno		プレイヤーナンバー
 *
 *	@retval	TRUE	会話不可能
 *	@retval	FALSE	会話可能
 */
//-----------------------------------------------------------------------------
BOOL WFBLY_ROOM_CheckNpcInfoDrawBusy( const WFLBY_ROOMWK* cp_wk, u8 plno )
{
	return cp_wk->npc_busy.info_draw_busy[ plno ];
}





//----------------------------------------------------------------------------
/**
 *	@brief	メッセージ表示
 *
 *	@param	p_wk		ワーク
 *	@param	cp_str		文字列
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_TALKWIN_PrintStr( WFLBY_ROOMWK* p_wk, const STRBUF* cp_str )
{
	WFLBY_ROOM_TalkWin_Print( &p_wk->talkwin, cp_str );
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージ表示	一度に全部表示
 *
 *	@param	p_wk		ワーク
 *	@param	cp_str		文字列
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_TALKWIN_PrintStrAllPut( WFLBY_ROOMWK* p_wk, const STRBUF* cp_str )
{
	WFLBY_ROOM_TalkWin_PrintAll( &p_wk->talkwin, cp_str );
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージの表示が完了したかチェック
 *
 *	@param	cp_wk		ワーク
 *
 *	@retval	TRUE	メッセージ終了
 *	@retval	FALSE	メッセージ表示中
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_ROOM_TALKWIN_WaitEnd( const WFLBY_ROOMWK* cp_wk )
{
	return WFLBY_ROOM_TalkWin_EndWait( &cp_wk->talkwin );
}

//----------------------------------------------------------------------------
/**
 *	@brief	時間ウエイトアイコンを表示する
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_TALKWIN_StartTimeWait( WFLBY_ROOMWK* p_wk )
{
	WFLBY_ROOM_TalkWin_StartTimeWait( &p_wk->talkwin );
}

//----------------------------------------------------------------------------
/**
 *	@brief	時間ウエイトアイコンを消す
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_TALKWIN_StopTimeWait( WFLBY_ROOMWK* p_wk )
{
	WFLBY_ROOM_TalkWin_StopTimeWait( &p_wk->talkwin );
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイムウエイト表示中かチェック
 *
 *	@param	cp_wk		ワーク
 *
 *	@retval	TRUE	タイムウエイト表示中
 *	@retval	FALSE	タイムウエイト出してません
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_ROOM_TALKWIN_CheckTimeWait( const WFLBY_ROOMWK* cp_wk )
{
	return WFLBY_ROOM_TalkWin_CheckTimeWait( &cp_wk->talkwin );
}

//----------------------------------------------------------------------------
/**
 *	@brief	会話ウィンドウを消す
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_TALKWIN_Off( WFLBY_ROOMWK* p_wk )
{
	WFLBY_ROOM_TalkWin_Off( &p_wk->talkwin );
}




//----------------------------------------------------------------------------
/**
 *	@brief	看板ウィンドウ	メッセージ表示
 *
 *	@param	p_wk		ワーク
 *	@param	cp_str		文字列
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_BOARDWIN_PrintStr( WFLBY_ROOMWK* p_wk, const STRBUF* cp_str )
{
	WFLBY_ROOM_TalkWin_Board_Print( &p_wk->boardwin, cp_str );
}

//----------------------------------------------------------------------------
/**
 *	@brief	看板ウィンドウ	メッセージ一気に表示
 *
 *	@param	p_wk		ワーク
 *	@param	cp_str		文字列
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_BOARDWIN_PrintStrAllPut( WFLBY_ROOMWK* p_wk, const STRBUF* cp_str )
{
	WFLBY_ROOM_TalkWin_Board_PrintAll( &p_wk->boardwin, cp_str );
}

//----------------------------------------------------------------------------
/**
 *	@brief	看板ウィンドウ	メッセージ表示完了待ち
 *
 *	@param	cp_wk		ワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_ROOM_BOARDWIN_WaitEnd( const WFLBY_ROOMWK* cp_wk )
{
	return WFLBY_ROOM_TalkWin_EndWait( &cp_wk->boardwin );
}

//----------------------------------------------------------------------------
/**
 *	@brief	看板ウィンドウの表示消し
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_BOARDWIN_Off( WFLBY_ROOMWK* p_wk )
{
	WFLBY_ROOM_TalkWin_Board_Off( &p_wk->boardwin );
}

//----------------------------------------------------------------------------
/**
 *	@brief	項目リスト作成
 *
 *	@param	p_wk		ワーク
 *	@param	num			リスト数
 */
//-----------------------------------------------------------------------------

void WFLBY_ROOM_LISTWIN_CreateBmpList( WFLBY_ROOMWK* p_wk, u32 num )
{
	WFLBY_ROOM_ListWin_CreateBmpList( &p_wk->listwin, num, HEAPID_WFLBY_ROOMGRA, 0 );
}

// MatchComment: New function
void WFLBY_ROOM_LISTWIN_CreateBmpList_WithThirdArg( WFLBY_ROOMWK* p_wk, u32 num, u32 a2 )
{
	WFLBY_ROOM_ListWin_CreateBmpList( &p_wk->listwin, num, HEAPID_WFLBY_ROOMGRA, a2 );
}

//----------------------------------------------------------------------------
/**
 *	@brief	項目リスト破棄
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_LISTWIN_DeleteBmpList( WFLBY_ROOMWK* p_wk )
{
	WFLBY_ROOM_ListWin_DeleteBmpList( &p_wk->listwin );
}

//----------------------------------------------------------------------------
/**
 *	@brief	項目の追加
 *
 *	@param	p_wk		ワーク
 *	@param	cp_str		文字列
 *	@param	param		パラメータ
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_LISTWIN_SetBmpListStr( WFLBY_ROOMWK* p_wk, const STRBUF* cp_str, u32 param )
{
	WFLBY_ROOM_ListWin_SetBmpListStr( &p_wk->listwin, cp_str, param );
}

//----------------------------------------------------------------------------
/**
 *	@brief	項目リストポインタを取得
 *
 *	@param	cp_wk		ワーク
 *
 *	@return	ポインタ
 */
//-----------------------------------------------------------------------------
const BMP_MENULIST_DATA* WFLBY_ROOM_LISTWIN_GetBmpList( const WFLBY_ROOMWK* cp_wk )
{
	return WFLBY_ROOM_ListWin_GetBmpList( &cp_wk->listwin );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ビットマップリストにparamが登録されているかチェックする
 *
 *	@param	cp_wk		ワーク
 *	@param	param		パラメータ
 *
 *	@retval	TRUE	ある
 *	@retval	FALSE	ない
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_ROOM_LISTWIN_CheckBmpListParam( const WFLBY_ROOMWK* cp_wk, u32 param )
{
	return WFLBY_ROOM_ListWin_CheckBmpListParam( &cp_wk->listwin, param );
}


//----------------------------------------------------------------------------
/**
 *	@brief	リストウィンドウ	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	cp_data		表示リストデータ
 *	@param	list_p		リスト初期位置
 *	@param	cursor_p	リスト初期カーソル
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_LISTWIN_Start( WFLBY_ROOMWK* p_wk, const BMPLIST_HEADER* cp_data, u16 list_p, u16 cursor_p )
{
	WFLBY_ROOM_ListWin_Start( &p_wk->listwin, cp_data, &p_wk->graphic, list_p, cursor_p, HEAPID_WFLBY_ROOM, WFLBY_LISTWIN_X, WFLBY_LISTWIN_Y, WFLBY_LISTWIN_SIZX );
}


//----------------------------------------------------------------------------
/**
 *	@brief	リストウィンドウ	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	cp_data		表示リストデータ
 *	@param	list_p		リスト初期位置
 *	@param	cursor_p	リスト初期カーソル
 *	@param	cx			ビットマップキャラｘ
 *	@param	cy			ビットマップキャラｙ
 *	@param	szcx		ビットマップキャラサイズｘ
 */
//-----------------------------------------------------------------------------
extern void WFLBY_ROOM_LISTWIN_Start_Ex( WFLBY_ROOMWK* p_wk, const BMPLIST_HEADER* cp_data, u16 list_p, u16 cursor_p, u8 cx, u8 cy, u8 szcx )
{
	WFLBY_ROOM_ListWin_Start( &p_wk->listwin, cp_data, &p_wk->graphic, list_p, cursor_p, HEAPID_WFLBY_ROOM, cx, cy, szcx );
}

//----------------------------------------------------------------------------
/**
 *	@brief	リストウィンドウ	メイン
 *
 *	@param	p_wk	ワーク
 *
 * @retval	"param = 選択パラメータ"
 * @retval	"BMPLIST_NULL = 選択中"
 * @retval	"BMPLIST_CANCEL	= キャンセル(Ｂボタン)"
 */
//-----------------------------------------------------------------------------
u32 WFLBY_ROOM_LISTWIN_Main( WFLBY_ROOMWK* p_wk )
{
	return WFLBY_ROOM_ListWin_Main( &p_wk->listwin );
}

//----------------------------------------------------------------------------
/**
 *	@brief	リストウィンドウ	終了
 *
 *	@param	p_wk		ワーク
 *	@param	p_list_p	リストカーソル位置取得
 *	@param	p_cursor_p	カーソル位置取得
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_LISTWIN_End( WFLBY_ROOMWK* p_wk, u16* p_list_p, u16* p_cursor_p )
{
	WFLBY_ROOM_ListWin_End( &p_wk->listwin, p_list_p, p_cursor_p );
}

//----------------------------------------------------------------------------
/**
 *	@brief	リストウィンドウ	矢印　ONOFF
 *
 *	@param	p_wk		ワーク
 *	@param	flag		表示フラグ	TRUE：表示
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_LISTWIN_YAZIRUSHI_SetDraw( WFLBY_ROOMWK* p_wk, BOOL flag )
{
	WFLBY_ROOM_ListWin_SetClactDraw( &p_wk->listwin, flag );
}


//----------------------------------------------------------------------------
/**
 *	@brief	サブウィンドウ表示
 *
 *	@param	p_wk		ワーク
 *	@param	cx			ｘキャラ位置
 *	@param	cy			ｙキャラ位置
 *	@param	szcx		ｘキャラサイズ
 *	@param	szcy		ｙキャラサイズ
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_SUBWIN_Start( WFLBY_ROOMWK* p_wk, u8 cx, u8 cy, u8 szcx, u8 szcy )
{
	WFLBY_ROOM_SubWin_Start( &p_wk->subwin, &p_wk->graphic, HEAPID_WFLBY_ROOM, cx, cy, szcx, szcy );
}

//----------------------------------------------------------------------------
/**
 *	@brief	サブウィンドウ表示OFF
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_SUBWIN_End( WFLBY_ROOMWK* p_wk )
{
	WFLBY_ROOM_SubWin_End( &p_wk->subwin );
}

//----------------------------------------------------------------------------
/**
 *	@brief	サブウィンドウ	メッセージ表示
 *
 *	@param	p_wk		ワーク
 *	@param	cp_str		メッセージ
 *	@param	x			ｘドット座標
 *	@param	y			ｙドット座標
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_SUBWIN_Print( WFLBY_ROOMWK* p_wk, const STRBUF* cp_str, u8 x, u8 y )
{
	WFLBY_ROOM_SubWin_PrintAll( &p_wk->subwin, cp_str, x, y );
}

//----------------------------------------------------------------------------
/**
 *	@brief	サブウィンドウ	表示物クリア
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_SUBWIN_Clear( WFLBY_ROOMWK* p_wk )
{
	WFLBY_ROOM_SubWin_Clear( &p_wk->subwin );
}

//----------------------------------------------------------------------------
/**
 *	@brief	矩形クリア処理
 *
 *	@param	p_wk	ワーク
 *	@param	x		ｘ座標
 *	@param	y		ｙ座標
 *	@param	sx		ｘサイズ
 *	@param	sy		ｙサイズ
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_SUBWIN_ClearRect( WFLBY_ROOMWK* p_wk, u16 x, u16 y, u16 sx, u16 sy )
{
	WFLBY_ROOM_SubWin_ClearRect( &p_wk->subwin, x, y, sx, sy );
}

//----------------------------------------------------------------------------
/**
 *	@brief	YESNOウィンドウ	開始
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_YESNOWIN_Start( WFLBY_ROOMWK* p_wk )
{
	WFLBY_ROOM_ListWin_Start( &p_wk->listwin, &p_wk->yesnowin.bmplist, &p_wk->graphic, 
			0, 0, HEAPID_WFLBY_ROOM, 25, 13, 6 );
}

//----------------------------------------------------------------------------
/**
 *	@brief	YESNOウィンドウ	開始	いいえから開始
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_YESNOWIN_StartNo( WFLBY_ROOMWK* p_wk )
{
	WFLBY_ROOM_ListWin_Start( &p_wk->listwin, &p_wk->yesnowin.bmplist, &p_wk->graphic, 
			0, WFLBY_ROOM_YESNO_NO, HEAPID_WFLBY_ROOM, 25, 13, 6 );
}

//----------------------------------------------------------------------------
/**
 *	@brief	YESNOウィンドウ　メイン
 *
 *	@param	p_wk	ワーク
 *
 *	@retval	WFLBY_ROOM_YESNO_OK,	// はい
 *	@retval	WFLBY_ROOM_YESNO_NO,	// いいえ
 *	@retval	WFLBY_ROOM_YESNO_WAIT,	// 選び中
 */
//-----------------------------------------------------------------------------
WFLBY_ROOM_YESNO_RET WFLBY_ROOM_YESNOWIN_Main( WFLBY_ROOMWK* p_wk )
{
	u32 result;

	result = WFLBY_ROOM_ListWin_Main( &p_wk->listwin );
	switch( result ){
	// いいえ
	case BMPLIST_CANCEL:
		Snd_SePlay( WFLBY_SND_CURSOR );
	case WFLBY_ROOM_YESNO_NO:
		return WFLBY_ROOM_YESNO_NO;

	//  はい
	case WFLBY_ROOM_YESNO_OK:
		return WFLBY_ROOM_YESNO_OK;

	// 選択中
	default:
		break;
	}

	return WFLBY_ROOM_YESNO_WAIT;
}

//----------------------------------------------------------------------------
/**
 *	@brief	YESNOウィンドウ　終了
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_YESNOWIN_End( WFLBY_ROOMWK* p_wk )
{
	// リスト消す
	WFLBY_ROOM_ListWin_End( &p_wk->listwin, NULL, NULL );
}

//----------------------------------------------------------------------------
/**
 *	@brief	広場専用メッセージデータの設定
 *
 *	@param	p_wk		ワーク
 *	@param	type		メッセージタイプ
 *	@param	stridx		文字列IDX
 *
 *	@return	メッセージデータ
 */
//-----------------------------------------------------------------------------
STRBUF* WFLBY_ROOM_MSG_Get( WFLBY_ROOMWK* p_wk, WFLBY_DEFMSG_TYPE type, u32 stridx )
{
	return WFLBY_ROOM_Msg_Get( &p_wk->def_msg, type, stridx );
}

//----------------------------------------------------------------------------
/**
 *	@brief	数字を設定
 *
 *	@param	p_wk		ワーク
 *	@param	num			数字
 *	@param	keta		桁
 *	@param	bufid		バッファＩＤ
 *	@param	disptype	左つめとか
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_MSG_SetNumber( WFLBY_ROOMWK* p_wk, u32  num, u32 keta, u32 bufid, NUMBER_DISPTYPE disptype )
{
	WFLBY_ROOM_Msg_SetNumber( &p_wk->def_msg, num, keta, bufid, disptype );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ワードセットにプレイヤー名を設定
 *
 *	@param	p_wk		ワーク
 *	@param	cp_mystatus	MYSTATUS
 *	@param	bufid		バッファID
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_MSG_SetPlayerName( WFLBY_ROOMWK* p_wk, const MYSTATUS* cp_mystatus, u32 bufid )
{
	WFLBY_ROOM_Msg_SetPlayerName( &p_wk->def_msg, cp_mystatus, bufid );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ワードセットにプレイヤー名を設定
 *
 *	@param	p_wk		ワーク
 *	@param	idx			インデックス
 *	@param	bufid		バッファID
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_MSG_SetIdxPlayerName( WFLBY_ROOMWK* p_wk, u32 idx, u32 bufid )
{
	MYSTATUS* p_status;
	const WFLBY_USER_PROFILE* cp_profile;

	p_status	= MyStatus_AllocWork( HEAPID_WFLBY_ROOM );
	if( idx == WFLBY_SYSTEM_GetMyIdx( p_wk->p_system ) ){
		cp_profile	= WFLBY_SYSTEM_GetMyProfileLocal( p_wk->p_system );	
	}else{
		cp_profile	= WFLBY_SYSTEM_GetUserProfile( p_wk->p_system, idx );	
	}
	WFLBY_SYSTEM_GetProfileMyStatus( cp_profile, p_status, HEAPID_WFLBY_ROOM );
	WFLBY_ROOM_Msg_SetPlayerName( &p_wk->def_msg, p_status, bufid );

	sys_FreeMemoryEz( p_status );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ミニゲームを設定
 *
 *	@param	p_wk		ワーク
 *	@param	type		ミニゲームタイプ
 *	@param	bufid		バッファID
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_MSG_SetMinigame( WFLBY_ROOMWK* p_wk, WFLBY_GAMETYPE type, u32 bufid )
{
	WFLBY_ROOM_Msg_SetMinigame( &p_wk->def_msg, type, bufid );
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイムイベント名を設定
 *
 *	@param	p_wk		ワーク
 *	@param	type		タイムイベントGMMタイプ
 *	@param	bufid		バッファID
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_MSG_SetTimeEvent( WFLBY_ROOMWK* p_wk, WFLBY_EVENTGMM_TYPE type, u32 bufid )
{
	WFLBY_ROOM_Msg_SetTimeevent( &p_wk->def_msg, type, bufid );
}

//----------------------------------------------------------------------------
/**
 *	@brief	アイテムを設定
 *
 *	@param	p_wk		ワーク
 *	@param	item		アイテム
 *	@param	bufid		バッファID
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_MSG_SetItem( WFLBY_ROOMWK* p_wk, WFLBY_ITEMTYPE item, u32 bufid )
{
	WFLBY_ROOM_Msg_SetItem( &p_wk->def_msg, item, bufid );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ワードセットのクリア
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_MSG_ClearWordSet( WFLBY_ROOMWK* p_wk )
{
	WFLBY_ROOM_Msg_ClearWordSet( &p_wk->def_msg );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ワードセットにタイムゾーンのメッセージを追加する
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_MSG_SetAisatsuJapan( WFLBY_ROOMWK* p_wk, u32 bufID, u32 time_zone )
{
	WORDSET_RegisterWiFiLobbyAisatsuJapan( p_wk->def_msg.p_wordset, bufID, time_zone );
}
void WFLBY_ROOM_MSG_SetAisatsuEnglish( WFLBY_ROOMWK* p_wk, u32 bufID, u32 time_zone )
{
	WORDSET_RegisterWiFiLobbyAisatsuEnglish( p_wk->def_msg.p_wordset, bufID, time_zone );
}
void WFLBY_ROOM_MSG_SetAisatsuFrance( WFLBY_ROOMWK* p_wk, u32 bufID, u32 time_zone )
{
	WORDSET_RegisterWiFiLobbyAisatsuFrance( p_wk->def_msg.p_wordset, bufID, time_zone );
}
void WFLBY_ROOM_MSG_SetAisatsuItaly( WFLBY_ROOMWK* p_wk, u32 bufID, u32 time_zone )
{
	WORDSET_RegisterWiFiLobbyAisatsuItaly( p_wk->def_msg.p_wordset, bufID, time_zone );
}
void WFLBY_ROOM_MSG_SetAisatsuGerMany( WFLBY_ROOMWK* p_wk, u32 bufID, u32 time_zone )
{
	WORDSET_RegisterWiFiLobbyAisatsuGerMany( p_wk->def_msg.p_wordset, bufID, time_zone );
}
void WFLBY_ROOM_MSG_SetAisatsuSpain( WFLBY_ROOMWK* p_wk, u32 bufID, u32 time_zone )
{
	WORDSET_RegisterWiFiLobbyAisatsuSpain( p_wk->def_msg.p_wordset, bufID, time_zone );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ワードセットに技タイプ名を設定
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_MSG_SetWazaType( WFLBY_ROOMWK* p_wk, u32 bufID, WFLBY_POKEWAZA_TYPE type )
{
	type = WFLBY_BattleWazaType_Get( type );
	WORDSET_RegisterPokeTypeName( p_wk->def_msg.p_wordset, bufID, type );
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナカードの表示
 *
 *	@param	p_wk		ワーク
 *	@param	idx			インデックス
 *	@param	aikotoba	合言葉がある場合表示するか	TRUE:表示する
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_UNDERWIN_TrCardOn( WFLBY_ROOMWK* p_wk, u32 idx, BOOL aikotoba )
{
	// 表示変更する必要があるかチェック
	if( (WFLBY_ROOM_UNDERWIN_GetTrCardPlIdx( &p_wk->under_win ) == idx) &&
		(WFLBY_ROOM_UNDERWIN_CheckTrCardDraw( &p_wk->under_win ) == TRUE) &&
#if PL_T0862_080712_FIX
		(WFLBY_ROOM_UNDERWIN_CheckReqNone( &p_wk->under_win ) == FALSE) &&
#endif
		(WFLBY_ROOM_UNDERWIN_CheckTrCardAikotobaDraw( &p_wk->under_win ) == aikotoba) ){
		return;
	}
	WFLBY_ROOM_UNDERWIN_ReqTrCard( &p_wk->under_win, idx, aikotoba );
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナーカードガジェット変更
 *
 *	@param	p_wk		ワーク
 *	@param	item		変更するアイテム
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_UNDERWIN_TrCardGadGetChange( WFLBY_ROOMWK* p_wk, WFLBY_ITEMTYPE item )
{
	WFLBY_ROOM_UNDERWIN_ChangeItemTrCard( &p_wk->under_win, &p_wk->graphic, item,  HEAPID_WFLBY_ROOM );
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナカード　技タイプをを再描画
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_UNDERWIN_TrCardWazaTypeWrite( WFLBY_ROOMWK* p_wk )
{
	const WFLBY_USER_PROFILE* cp_profile;
	u32 plidx;

	// 表示変更する必要があるかチェック
	if( (WFLBY_ROOM_UNDERWIN_CheckTrCardDraw( &p_wk->under_win ) == FALSE) ){
		return;
	}

	plidx		= WFLBY_ROOM_UNDERWIN_GetTrCardPlIdx( &p_wk->under_win );
	cp_profile	= WFLBY_SYSTEM_GetUserProfile( p_wk->p_system, plidx );

	WFLBY_ROOM_UNDERWIN_TrCard_WriteWazaType( &p_wk->under_win, &p_wk->under_win.tr_card, &p_wk->graphic, HEAPID_WFLBY_ROOM, cp_profile );
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナカードの非表示
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_UNDERWIN_TrCardOff( WFLBY_ROOMWK* p_wk )
{
	WFLBY_ROOM_UNDERWIN_ReqBttn( &p_wk->under_win );
}

//----------------------------------------------------------------------------
/**
 *	@brief	表示中のプレイヤーIDを取得する
 *
 *	@param	cp_wk		ワーク
 *
 *	@return プレイヤーナンバー
 */
//-----------------------------------------------------------------------------
u32 WFLBY_ROOM_UNDERWIN_TrCardGetPlIdx( const WFLBY_ROOMWK* cp_wk )
{
	return WFLBY_ROOM_UNDERWIN_GetTrCardPlIdx( &cp_wk->under_win );
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナカードを表示しているかチェック
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	トレーナカードを表示している
 *	@retval	FALSE	トレーナカードを表示していない
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_ROOM_UNDERWIN_TrCardGetDraw( const WFLBY_ROOMWK* cp_wk )
{
	return WFLBY_ROOM_UNDERWIN_CheckTrCardDraw( &cp_wk->under_win );
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナカードをOFFにすることがかのうか？
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	可能
 *	@retval	FALSE	不可能
 *
 *	ちょっとわかりにくいですが、
 *	リクエストがNONEでトレーナーカード表示中か？ということです。
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_ROOM_UNDERWIN_TrCardCheckCanOff( const WFLBY_ROOMWK* cp_wk )
{
	return WFLBY_ROOM_UNDERWIN_CheckTrCardCanOff( &cp_wk->under_win );
}

//----------------------------------------------------------------------------
/**
 *	@brief	した画面が通常状態かチェックする
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	通常状態
 *	@retval	FALSE	その他の状態
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_ROOM_UNDERWIN_CheckSeqNormal( const WFLBY_ROOMWK* cp_wk )
{
	u32 seq;

	seq = WFLBY_ROOM_UNDERWIN_GetSeq( &cp_wk->under_win );
	if( seq == WFLBY_UNDERWIN_SEQ_NORMAL ){
		return TRUE;
	}
	return FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	フロートボタン表示
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_UNDERWIN_FloatBttnOn( WFLBY_ROOMWK* p_wk )
{
	WFLBY_ROOM_UNDERWIN_ReqBttnFloat( &p_wk->under_win );
}

//----------------------------------------------------------------------------
/**
 *	@brief	フロートボタン非表示
 *	
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_UNDERWIN_FloatBttnOff( WFLBY_ROOMWK* p_wk )
{
	WFLBY_ROOM_UNDERWIN_ReqBttn( &p_wk->under_win );
}

//----------------------------------------------------------------------------
/**
 *	@brief	フロートアニメ中かチェック
 *
 *	@param	cp_wk		ワーク
 *	@param	flaotidx	フロートインデックス
 *
 *	@retval	TRUE	アニメ中
 *	@retval	FALSE	アニメしてない
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_ROOM_FLOAT_CheckShakeAnm( const WFLBY_ROOMWK* cp_wk, u32 floatidx )
{
	return WFLBY_TIMEEVENT_FLOAT_CheckAnm( cp_wk->p_timeevent, floatidx, WFLBY_FLOAT_ANM_SHAKE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ガジェットを取得したらTRUEを入れる
 *
 *	@param	p_wk		ワーク
 *	@param	get			ガジェット
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_GadGet_Get( WFLBY_ROOMWK* p_wk )
{
	p_wk->under_win.bttn_start = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ボタンの動作状態を設定する	ボタン反応を停止させることが出来ます。
 *
 *	@param	p_wk		ワーク
 *	@param	stop		停止させるときTRUE
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_GadGet_SetStop( WFLBY_ROOMWK* p_wk, BOOL stop )
{
	WFLBY_ROOM_UNDERWIN_SetBttnStop( &p_wk->under_win, stop );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ガジェットロック	クリア
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_GadGet_ClearLockWait( WFLBY_ROOMWK* p_wk )
{
	WFLBY_ROOM_UNDERWIN_Button_BttnLockClear( &p_wk->under_win.bttn );
}

//----------------------------------------------------------------------------
/**
 *	@brief	自分がガジェットを起動するかチェック
 *
 *	@param	p_wk		ワーク
 *
 *	@retval	TRUE	起動
 *	@retval	FALSE	しない
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_ROOM_GadGet_GetFlag( WFLBY_ROOMWK* p_wk )
{
	BOOL ret;
	
	ret = p_wk->gadget_flag;
	p_wk->gadget_flag = FALSE;
	return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ガジェットを流す
 *
 *	@param	p_wk		ワーク
 *	@param	idx			インデックス
 */
//-----------------------------------------------------------------------------
void WFLBY_ROOM_GadGet_Play( WFLBY_ROOMWK* p_wk, u32 idx )
{
	const WFLBY_USER_PROFILE* cp_profile;
	u32 gadget;
	
	GF_ASSERT( idx < WFLBY_PLAYER_MAX );

	cp_profile	= WFLBY_SYSTEM_GetUserProfile( p_wk->p_system, idx );
	gadget		= WFLBY_SYSTEM_GetProfileItem( cp_profile );
	WFLBY_GADGET_Start( p_wk->p_gadget, idx, gadget );

	// 自分だったらみんなに送る
	if( idx == WFLBY_SYSTEM_GetMyIdx( p_wk->p_system ) ){
		WFLBY_SYSTEM_SendGadGetData( p_wk->p_system, gadget );
	}

}

//----------------------------------------------------------------------------
/**
 *	@brief	ガジェットが終わるのかチェック
 *
 *	@param	cp_wk		ワーク
 *	@param	idx			インデックス
 *
 *	@retval	TRUE		終わった
 *	@retval	FALSE		途中
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_ROOM_GadGet_PlayWait( const WFLBY_ROOMWK* cp_wk, u32 idx )
{
	GF_ASSERT( idx < WFLBY_PLAYER_MAX );

	return WFLBY_GADGET_EndWait( cp_wk->p_gadget, idx );
}







//-----------------------------------------------------------------------------
/**
 *		プライベート関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	VBLANK関数
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_VBlank( void* p_work )
{
	WFLBY_ROOMWK* p_wk = p_work;


	WFLBY_3DMAPOBJCONT_VBlank( p_wk->p_mapobjcont );
	WFLBY_3DOBJCONT_VBlank( p_wk->p_objcont );
	WFLBY_ROOM_GraphicVblank( &p_wk->graphic );
}

//----------------------------------------------------------------------------
/**
 *	@brief	表示システム初期化
 *
 *	@param	p_sys		システム
 *	@param	p_save		セーブデータ
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_GraphicInit( WFLBY_GRAPHICCONT* p_sys, SAVEDATA* p_save, u32 heapID )
{

	// 半透明レジスタOFF
	G2_BlendNone();
	G2S_BlendNone();

	// Vram転送マネージャ作成
	initVramTransferManagerHeap( WFLBY_ROOM_VTRTSK_NUM, heapID );

	// バンク設定
	GF_Disp_SetBank( &sc_WFLBY_ROOM_BANK );

	// 描画面変更
	// メインとサブを切り替える
	sys.disp3DSW = DISP_3D_TO_MAIN;
	GF_Disp_DispSelect();

	// バックグラウンドを黒にする
	{
		GF_BGL_BackGroundColorSet( GF_BGL_FRAME0_M, 0 );
	}

	// BGL
	{
		int i;

		GF_BGL_InitBG(&sc_BGINIT);

		p_sys->p_bgl = GF_BGL_BglIniAlloc( heapID );

		for( i=0; i<WFLBY_ROOM_BGCNT_NUM; i++ ){
			GF_BGL_BGControlSet( p_sys->p_bgl, 
					sc_WFLBY_ROOM_BGCNT_FRM[i], &sc_WFLBY_ROOM_BGCNT_DATA[i],
					GF_BGL_MODE_TEXT );
			GF_BGL_ClearCharSet( sc_WFLBY_ROOM_BGCNT_FRM[i], 32, 0, heapID);
			GF_BGL_ScrClear( p_sys->p_bgl, sc_WFLBY_ROOM_BGCNT_FRM[i] );
		}
	}


	// ウィンドウ設定
	{
		CONFIG* p_config;
		u8 winnum;

		p_config = SaveData_GetConfig( p_save );
		winnum = CONFIG_GetWindowType( p_config );

		// 文字パレット
		SystemFontPaletteLoad( PALTYPE_MAIN_BG, WFLBY_ROOM_BGPL_SYSFONT_CL*32, heapID );
		TalkFontPaletteLoad( PALTYPE_MAIN_BG, WFLBY_ROOM_BGPL_TALKFONT_CL*32, heapID );

		// ウィンドウグラフィック
		MenuWinGraphicSet(
				p_sys->p_bgl, sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_MAIN_MSGWIN], WFLBY_SYSWINGRA_CGX,
				WFLBY_ROOM_BGPL_SYSWIN, 0, heapID );
		TalkWinGraphicSet(
				p_sys->p_bgl, sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_MAIN_MSGWIN], WFLBY_TALKWINGRA_CGX, 
				WFLBY_ROOM_BGPL_TALKWIN, winnum, heapID );
		BoardWinGraphicSet(
			p_sys->p_bgl, sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_MAIN_MSGWIN],
			WFLBY_BOARDWINGRA_CGX, WFLBY_ROOM_BGPL_BOARDWIN, 
			BOARD_TYPE_INFO, 0, heapID );

		// パレットだけ、ロビー用パレットを使用する
		ArcUtil_PalSet( ARC_WIFILOBBY_OTHER_GRA, NARC_wifi_lobby_other_lobby_board_NCLR, 
				PALTYPE_MAIN_BG, WFLBY_ROOM_BGPL_BOARDWIN*32, 32, heapID );
	}


	// OAM
	{
		int i;

		// OAMマネージャーの初期化
		NNS_G2dInitOamManagerModule();

		// 共有OAMマネージャ作成
		// レンダラ用OAMマネージャ作成
		// ここで作成したOAMマネージャをみんなで共有する
		REND_OAMInit(
			0, 126,     // メイン画面OAM管理領域
			0, 31,      // メイン画面アフィン管理領域
			0, 126,     // サブ画面OAM管理領域
			0, 31,      // サブ画面アフィン管理領域
			heapID);


		// キャラクタマネージャー初期化
		InitCharManagerReg(&sc_WFLBY_ROOM_CHARMAN_INIT, GX_OBJVRAMMODE_CHAR_1D_32K, GX_OBJVRAMMODE_CHAR_1D_64K ); //リストのカーソルをフィールドと共通で使用するので32Kにする
	
		// パレットマネージャー初期化
		InitPlttManager(WFLBY_ROOM_OAM_CONTNUM, heapID);

		// 読み込み開始位置を初期化
		CharLoadStartAll();
		PlttLoadStartAll();

		//通信アイコン用にキャラ＆パレット制限
		CLACT_U_WmIcon_SetReserveAreaCharManager(NNS_G2D_VRAM_TYPE_2DMAIN, GX_OBJVRAMMODE_CHAR_1D_32K);	//リストのカーソルをフィールドと共通で使用するので32Kにする
		CLACT_U_WmIcon_SetReserveAreaPlttManager(NNS_G2D_VRAM_TYPE_2DMAIN);


		// 下画面に通信アイコンを出す
		WirelessIconEasy();  // 接続中なのでアイコン表示


		// セルアクター作成
		p_sys->p_clactset = CLACT_U_SetEasyInit( WFLBY_ROOM_CLACT_NUM, &p_sys->renddata, heapID );
		CLACT_U_SetSubSurfaceMatrix( &p_sys->renddata, 0, WFLBY_ROOM_CLACT_SUBSURFACE_Y );

		// リソースマネージャ作成
		for( i=0; i<WFLBY_ROOM_OAM_RESNUM; i++ ){
			p_sys->p_resman[i] = CLACT_U_ResManagerInit(WFLBY_ROOM_OAM_CONTNUM, i, heapID);
		}

		// セルVram転送マネージャ作成
		p_sys->p_celltransarray = InitCellTransfer(WFLBY_ROOM_OAM_CONTNUM, heapID );
		

		// 表示開始
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	}

	// 3D設定
	{
		p_sys->p_3dman = GF_G3DMAN_Init( heapID, GF_G3DMAN_LNK, GF_G3DTEX_256K, 
				GF_G3DMAN_LNK, GF_G3DPLT_64K, WFLBY_ROOM_DrawSys3DSetUp );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画システム	メイン処理
 *
 *	@param	p_sys		システム
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_GraphicMain( WFLBY_GRAPHICCONT* p_sys )
{
	CLACT_Draw( p_sys->p_clactset );

	UpdateCellTransfer();
}

//----------------------------------------------------------------------------
/**
 *	@brief	グラフィックシステム破棄
 *	
 *	@param	p_sys		システム
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_GraphicExit( WFLBY_GRAPHICCONT* p_sys )
{
	// VRAM転送マネージャ破棄
	DellVramTransferManager();

	// BGの破棄
	{
		int i;

		for( i=0; i<WFLBY_ROOM_BGCNT_NUM; i++ ){
			GF_BGL_BGControlExit( p_sys->p_bgl, sc_WFLBY_ROOM_BGCNT_FRM[i] );
		}

		// BGL破棄
		sys_FreeMemoryEz( p_sys->p_bgl );
	}

	// OAMの破棄
	{
		int i;

		DeleteCellTransfer( p_sys->p_celltransarray );
		p_sys->p_celltransarray = NULL;

		// リソースマネージャ作成
		for( i=0; i<WFLBY_ROOM_OAM_RESNUM; i++ ){
			CLACT_U_ResManagerDelete( p_sys->p_resman[i] );
		}

		// セルアクター破棄
		CLACT_DestSet( p_sys->p_clactset );


		// リソース解放
		DeleteCharManager();
		DeletePlttManager();

		//OAMレンダラー破棄
		REND_OAM_Delete();
	}

	// BGの破棄
	{
		GF_G3D_Exit( p_sys->p_3dman );
	}
	
}

//----------------------------------------------------------------------------
/**
 *	@brief	グラフィックVBlank処理
 *
 *	@param	p_sys		システム
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_GraphicVblank( WFLBY_GRAPHICCONT* p_sys )
{
    // BG書き換え
    GF_BGL_VBlankFunc( p_sys->p_bgl );

    // レンダラ共有OAMマネージャVram転送
    REND_OAMTrans();

	// Vram転送
	DoVramTransferManager();
}

//----------------------------------------------------------------------------
/**
 *	@brief	3Dセットアップ
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_DrawSys3DSetUp( void )
{
	// ３Ｄ使用面の設定(表示＆プライオリティー)
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
    G2_SetBG0Priority(1);

	// 各種描画モードの設定(シェード＆アンチエイリアス＆半透明)
    G3X_SetShading( GX_SHADING_TOON );
    G3X_AntiAlias( TRUE );
	G3X_AlphaTest( FALSE, 0 );	// アルファテスト　　オフ
	G3X_AlphaBlend( TRUE );		// アルファブレンド　オン

	G3X_EdgeMarking( TRUE );
	G3X_SetEdgeColorTable(sc_WFLBY_ROOM_EDGECOLOR);
	G3X_SetFog( FALSE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x8000, 0 );

	// クリアカラーの設定
    G3X_SetClearColor(GX_RGB(26,26,26),0,0x7fff,63,FALSE);	//color,alpha,depth,polygonID,fog
	// ビューポートの設定
    G3_ViewPort(0, 0, 255, 191);

	{
		VecFx16 vec;
		// ライト0
		{
			NNS_G3dGlbLightVector( 0, 0xfffff805, 0xfffff224, 0x6e );
		}
		// ライト1
		{
			VEC_Fx16Set( &vec, 0, 0, FX16_ONE );
			VEC_Fx16Normalize( &vec, &vec );
			NNS_G3dGlbLightVector( 1, vec.x, vec.y, vec.z );
		}
		// ライト2
		// 世界時計やニュースなどクローズ後明かりを消すもの用
		{
			VEC_Fx16Set( &vec, 0, 0, FX16_ONE );
			VEC_Fx16Normalize( &vec, &vec );
			NNS_G3dGlbLightVector( 2, vec.x, vec.y, vec.z );
			NNS_G3dGlbLightColor( 2, GX_RGB( 31,31,31 ) );
		}
		// ライト3
		{
			VEC_Fx16Set( &vec, 0, 0, FX16_ONE );
			VEC_Fx16Normalize( &vec, &vec );
			NNS_G3dGlbLightVector( 3, vec.x, vec.y, vec.z );
			NNS_G3dGlbLightColor( 3, GX_RGB( 31,31,31 ) );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	メイン動作
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_RoomMain( WFLBY_ROOMWK* p_wk )
{
	WFLBY_DEBUG_ROOM_PRINT_TIME_TICK_INIT;

	// 人物動作
	WFLBY_3DOBJCONT_Move( p_wk->p_objcont );
	WFLBY_DEBUG_ROOM_PRINT_TIME_TICK_PRINT( __LINE__ );


	// エラーとタイムアウトで動作しない
	if( (p_wk->err_flag == FALSE) && (p_wk->timeout == FALSE) ){	

		// イベント実行	BEFORE
		{
			WFLBY_EVENT_MainBefore( p_wk->p_event );
		}
		WFLBY_DEBUG_ROOM_PRINT_TIME_TICK_PRINT( __LINE__ );

	}


	// 配置オブジェ動作
	WFLBY_3DMAPOBJCONT_Main( p_wk->p_mapobjcont );
	WFLBY_DEBUG_ROOM_PRINT_TIME_TICK_PRINT( __LINE__ );


	// エラーとタイムアウトで動作しない
	if( (p_wk->err_flag == FALSE) && (p_wk->timeout == FALSE) ){	
		// 人物動作リクエスト反映
		WFLBY_3DOBJCONT_ReqMove( p_wk->p_objcont );
		WFLBY_DEBUG_ROOM_PRINT_TIME_TICK_PRINT( __LINE__ );

		// イベント実行	AFTER
		{
			WFLBY_EVENT_MainAfter( p_wk->p_event );
		}
		WFLBY_DEBUG_ROOM_PRINT_TIME_TICK_PRINT( __LINE__ );

		// タイムイベントメイン
		WFLBY_TIMEEVENT_Main( p_wk->p_timeevent );
		WFLBY_DEBUG_ROOM_PRINT_TIME_TICK_PRINT( __LINE__ );
	}


	// 配置オブジェアニメ管理
	WFLBY_ROOM_MapAnmCont( p_wk );
	WFLBY_DEBUG_ROOM_PRINT_TIME_TICK_PRINT( __LINE__ );

	
	// した画面メイン処理
	if( p_wk->fade_flag == FALSE ){
		BOOL no_input;
		// 終了処理にいっていたら、タッチなど受け付けない
		if( (p_wk->err_flag == TRUE) || (p_wk->timeout == TRUE) || (p_wk->end_flag == TRUE) ){	
			no_input = TRUE;
		}else{
			no_input = FALSE;
		}
		WFLBY_ROOM_UNDERWIN_Main( &p_wk->under_win, p_wk, no_input, HEAPID_WFLBY_ROOM );
	}
	WFLBY_DEBUG_ROOM_PRINT_TIME_TICK_PRINT( __LINE__ );

	// ガジェットアニメ
	WFLBY_GADGET_Main( p_wk->p_gadget );
	WFLBY_DEBUG_ROOM_PRINT_TIME_TICK_PRINT( __LINE__ );

	// ミニゲーム参加ロックカウンタ


#ifdef WFLBY_DEBUG_ROOM_CAMERA

	// カメラを変える
	if( sys.trg & PAD_BUTTON_SELECT ){
		WFLBY_DEBUG_ROOM_CAMERA_FLAG ^= 1;
		WFLBY_CAMERA_Exit( p_wk->p_camera );
		if( WFLBY_DEBUG_ROOM_CAMERA_FLAG ){
			p_wk->p_camera = WFLBY_CAMERA_DEBUG_CameraInit( HEAPID_WFLBY_ROOM );
		}else{
			p_wk->p_camera = WFLBY_CAMERA_Init( HEAPID_WFLBY_ROOM );
		}
		// ターゲットを設定
		WFLBY_CAMERA_SetTargetPerson( p_wk->p_camera,
				WFLBY_3DOBJCONT_GetPlayer( p_wk->p_objcont ) );
	}
#endif

#ifdef WFLBY_DEBUG_ROOM_ITEMCHG
	// タッチトイを変える
	if( sys.trg & PAD_BUTTON_L ){
		WFLBY_USER_PROFILE* p_profile;
		u32 item;

		p_profile	= WFLBY_SYSTEM_GetMyProfileLocal( p_wk->p_system );
		item		= WFLBY_SYSTEM_GetProfileItem( p_profile );
		item		= (item + 1) % WFLBY_ITEM_NUM;
		WFLBY_SYSTEM_SetMyItem( p_wk->p_system, item );

		OS_TPrintf( "=====  DEBUG Item Change %d  =======\n", item );
	}
#endif

}

//----------------------------------------------------------------------------
/**
 *	@brief	描画処理
 *
 *	@param	p_wk	ワーク
 */	
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_RoomDraw( WFLBY_ROOMWK* p_wk )
{
	MtxFx44 org_pm;
		
	WFLBY_DEBUG_ROOM_PRINT_TIME_TICK_INIT;
	
	GF_G3X_Reset();


	// カメラ設定
	WFLBY_CAMERA_Draw( p_wk->p_camera );
	WFLBY_DEBUG_ROOM_PRINT_TIME_TICK_PRINT_DRAW( __LINE__ );

	// マップの表示
	WFLBY_3DMAPOBJCONT_Draw( p_wk->p_mapobjcont );
	WFLBY_DEBUG_ROOM_PRINT_TIME_TICK_PRINT_DRAW( __LINE__ );

	//プロジェクションマトリクスの変換
#if 0
	{
		const MtxFx44 *m;
		MtxFx44 pm;

		m = NNS_G3dGlbGetProjectionMtx();

		org_pm = *m;
		pm = org_pm;
		pm._32 += FX_Mul(pm._22,WFLBY_ROOM_PRO_MAT_Z_OFS*FX32_ONE);
		NNS_G3dGlbSetProjectionMtx(&pm);
//		NNS_G3dGlbFlush();		//　ジオメトリコマンドを転送
	}
#endif
	
	// 人物の表示
	WFLBY_3DOBJCONT_Draw( p_wk->p_objcont );
	WFLBY_DEBUG_ROOM_PRINT_TIME_TICK_PRINT_DRAW( __LINE__ );

	//プロジェクションマトリクスを元に戻す
#if 0
	{
		NNS_G3dGlbSetProjectionMtx(&org_pm);
		NNS_G3dGlbFlush();		//　ジオメトリコマンドを転送
	}
#endif

	// ガジェット
	WFLBY_GADGET_Draw( p_wk->p_gadget );
	WFLBY_DEBUG_ROOM_PRINT_TIME_TICK_PRINT_DRAW( __LINE__ );
	
	GF_G3_RequestSwapBuffers(GX_SORTMODE_AUTO,GX_BUFFERMODE_Z);

	// 描画システムメイン
	WFLBY_ROOM_GraphicMain( &p_wk->graphic );
}

//----------------------------------------------------------------------------
/**
 *	@brief	マップアニメコントロール
 *
 *	@param	p_wk	ワーク
 *
 *	誰がアニメを管理するかわからんものはここで管理する
 */
//-----------------------------------------------------------------------------
static  void WFLBY_ROOM_MapAnmCont( WFLBY_ROOMWK* p_wk )
{
	int i;
	BOOL play;
	BOOL recruit;
	u32 num;
	u32 gametype;
	BOOL minigame_end;

	// ミニゲーム終了してるかチェック
	minigame_end = WFLBY_SYSTEM_Event_GetMiniGameStop( p_wk->p_system );
	
	// ミニゲーム
	for( i=0; i<DWC_LOBBY_MG_NUM; i++ ){
		recruit = DWC_LOBBY_MG_CheckRecruit( i );				// 募集中か
		if( recruit == TRUE ){
			if( DWC_LOBBY_MG_CheckEntryOk( i ) == FALSE ){		// 募集中でまだあそんでないか
				play = TRUE;
			}else{
				play = FALSE;
			}
		}else{
			play = FALSE;	// 募集してないのであそんでるわけない
		}
		num = DWC_LOBBY_MG_GetEntryNum( i );
		WFLBY_3DMAPOBJCONT_MAP_SetMGAnm(  p_wk->p_mapobjcont, WFLBY_GAME_BALLSLOW+i, num, 
				recruit, play, minigame_end );
	}
}





//----------------------------------------------------------------------------
/**
 *	@brief	会話ウィンドウ初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		グラフィックワーク
 *	@param	p_save		セーブデータ
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_TalkWin_Init( WFLBY_ROOM_TALKMSG* p_wk, WFLBY_GRAPHICCONT* p_sys, SAVEDATA* p_save, u32 heapID )
{

	//  ビットマップ確保
	GF_BGL_BmpWinAdd(
				p_sys->p_bgl, &p_wk->win, 
				sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_MAIN_MSGWIN],
				WFLBY_TALKWIN_X, WFLBY_TALKWIN_Y,
				WFLBY_TALKWIN_SIZX, WFLBY_TALKWIN_SIZY, WFLBY_ROOM_BGPL_TALKFONT_CL,
				WFLBY_TALKWIN_CGX );

	// クリーン
	GF_BGL_BmpWinDataFill( &p_wk->win, 15 );

	// 文字列バッファ作成
	p_wk->p_str = STRBUF_Create( WFLBY_TALKWIN_STRBUFNUM, heapID );

	// メッセージ表示ウエイトを設定
	{
		CONFIG* p_config;
		p_config = SaveData_GetConfig( p_save );
		p_wk->msgwait = CONFIG_GetMsgPrintSpeed( p_config );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	会話ウィンドウ破棄
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_TalkWin_Exit( WFLBY_ROOM_TALKMSG* p_wk )
{
	//  すべて停止
	if( GF_MSG_PrintEndCheck( p_wk->msgno ) ){
		GF_STR_PrintForceStop( p_wk->msgno );
	}
	WFLBY_ROOM_TalkWin_StopTimeWait_NoTrans( p_wk );
	
	// 文字列バッファ破棄
	STRBUF_Delete( p_wk->p_str );

	// ビットマップ破棄
	GF_BGL_BmpWinDel( &p_wk->win );
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージの表示処理
 *
 *	@param	p_wk		ワーク
 *	@param	cp_str		文字列
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_TalkWin_Print( WFLBY_ROOM_TALKMSG* p_wk, const STRBUF* cp_str )
{
	// メッセージ表示中なら消す
	if( GF_MSG_PrintEndCheck( p_wk->msgno ) ){
		GF_STR_PrintForceStop( p_wk->msgno );
	}

	// ビットマップのクリア
	GF_BGL_BmpWinDataFill( &p_wk->win, 15 );
	
	// 文字列コピー
	STRBUF_Copy( p_wk->p_str, cp_str );
	p_wk->msgno = GF_STR_PrintColor( &p_wk->win, FONT_TALK, p_wk->p_str, 0, 0,
			p_wk->msgwait, WFLBY_TALKWIN_MSGCOL, NULL );

	// ウィンドウを書き込む
	BmpTalkWinWrite( &p_wk->win, WINDOW_TRANS_OFF, WFLBY_TALKWINGRA_CGX, WFLBY_ROOM_BGPL_TALKWIN );
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージ表示	一度に全部表示する
 *
 *	@param	p_wk		ワーク
 *	@param	cp_str		文字列
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_TalkWin_PrintAll( WFLBY_ROOM_TALKMSG* p_wk, const STRBUF* cp_str )
{
	// メッセージ表示中なら消す
	if( GF_MSG_PrintEndCheck( p_wk->msgno ) ){
		GF_STR_PrintForceStop( p_wk->msgno );
	}

	// ビットマップのクリア
	GF_BGL_BmpWinDataFill( &p_wk->win, 15 );
	
	// 文字列コピー
	STRBUF_Copy( p_wk->p_str, cp_str );
	GF_STR_PrintColor( &p_wk->win, FONT_TALK, p_wk->p_str, 0, 0,
			MSG_NO_PUT, WFLBY_TALKWIN_MSGCOL, NULL );

	// ウィンドウを書き込む
	BmpTalkWinWrite( &p_wk->win, WINDOW_TRANS_OFF, WFLBY_TALKWINGRA_CGX, WFLBY_ROOM_BGPL_TALKWIN );
	GF_BGL_BmpWinOnVReq( &p_wk->win );
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイムウエイトアイコン開始
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_TalkWin_StartTimeWait( WFLBY_ROOM_TALKMSG* p_wk )
{
	GF_ASSERT( p_wk->p_timewait == NULL );
	
	p_wk->p_timewait = TimeWaitIconAdd( &p_wk->win, WFLBY_TALKWINGRA_CGX );
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイムウエイトアイコン終了
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_TalkWin_StopTimeWait( WFLBY_ROOM_TALKMSG* p_wk )
{
	if( p_wk->p_timewait != NULL ){
		TimeWaitIconDel( p_wk->p_timewait );
		p_wk->p_timewait = NULL;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイムウエイトアイコン終了
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_TalkWin_StopTimeWait_NoTrans( WFLBY_ROOM_TALKMSG* p_wk )
{
	if( p_wk->p_timewait != NULL ){
		TimeWaitIconTaskDel( p_wk->p_timewait );
		p_wk->p_timewait = NULL;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイムウエイトを表示しているかチェック
 *
 *	@param	cp_wk		ワーク
 *
 *	@retval	TRUE	タイムウエイト中
 *	@retval	FALSE	タイムウエイト表示してない
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_ROOM_TalkWin_CheckTimeWait( const WFLBY_ROOM_TALKMSG* cp_wk )
{
	if( cp_wk->p_timewait != NULL ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージ表示終了
 *
 *	@param	cp_wk		ワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_ROOM_TalkWin_EndWait( const WFLBY_ROOM_TALKMSG* cp_wk )
{
	if( GF_MSG_PrintEndCheck( cp_wk->msgno ) == 0 ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	会話ウィンドウ非表示
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_TalkWin_Off( WFLBY_ROOM_TALKMSG* p_wk )
{
	// メッセージ表示中なら消す
	if( GF_MSG_PrintEndCheck( p_wk->msgno ) ){
		GF_STR_PrintForceStop( p_wk->msgno );
	}

	// タイムウエイトも消す
	WFLBY_ROOM_TalkWin_StopTimeWait_NoTrans( p_wk );

	// 全体を消す
	BmpTalkWinClear( &p_wk->win, WINDOW_TRANS_OFF );

	// Vリクエスト
	GF_BGL_BmpWinOffVReq( &p_wk->win );
}



//----------------------------------------------------------------------------
/**
 *	@brief	会話ウィンドウ　派生	看板ウィンドウ	初期化
 *
 *	@param	p_wk			ワーク
 *	@param	p_sys			システム
 *	@param	p_save			セーブデータ
 *	@param	heapID			ヒープＩＤ
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_TalkWin_Board_Init( WFLBY_ROOM_TALKMSG* p_wk, WFLBY_GRAPHICCONT* p_sys, SAVEDATA* p_save, u32 heapID )
{
	WFLBY_ROOM_TalkWin_Init( p_wk, p_sys, p_save, heapID );
	// パレットナンバーだけ変更
	GF_BGL_BmpWinSet_Pal( &p_wk->win, WFLBY_ROOM_BGPL_BOARDWIN );
}

//----------------------------------------------------------------------------
/**
 *	@brief	会話ウィンドウ　派生	看板ウィンドウ	破棄
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_TalkWin_Board_Exit( WFLBY_ROOM_TALKMSG* p_wk )
{
	WFLBY_ROOM_TalkWin_Exit( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	会話ウィンドウ　派生	看板ウィンドウ　メッセージ書き込み
 *
 *	@param	p_wk		ワーク
 *	@param	cp_str		文字列
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_TalkWin_Board_Print( WFLBY_ROOM_TALKMSG* p_wk, const STRBUF* cp_str )
{
	// メッセージ表示中なら消す
	if( GF_MSG_PrintEndCheck( p_wk->msgno ) ){
		GF_STR_PrintForceStop( p_wk->msgno );
	}

	// ビットマップのクリア
	GF_BGL_BmpWinDataFill( &p_wk->win, 15 );
	
	// 文字列コピー
	STRBUF_Copy( p_wk->p_str, cp_str );
	p_wk->msgno = GF_STR_PrintSimple( &p_wk->win, FONT_TALK, p_wk->p_str, 0, 0,
			p_wk->msgwait, NULL );

	// ウィンドウを書き込む
	BmpBoardWinWrite( &p_wk->win, WINDOW_TRANS_OFF, WFLBY_BOARDWINGRA_CGX, WFLBY_ROOM_BGPL_BOARDWIN, BOARD_TYPE_INFO );
}

//----------------------------------------------------------------------------
/**
 *	@brief	会話ウィンドウ	派生	看板ウィンドウ　メッセージ全部描画
 *
 *	@param	p_wk
 *	@param	cp_str 
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_TalkWin_Board_PrintAll( WFLBY_ROOM_TALKMSG* p_wk, const STRBUF* cp_str )
{
	// メッセージ表示中なら消す
	if( GF_MSG_PrintEndCheck( p_wk->msgno ) ){
		GF_STR_PrintForceStop( p_wk->msgno );
	}

	// ビットマップのクリア
	GF_BGL_BmpWinDataFill( &p_wk->win, 15 );
	
	// 文字列コピー
	STRBUF_Copy( p_wk->p_str, cp_str );
	GF_STR_PrintSimple( &p_wk->win, FONT_TALK, p_wk->p_str, 0, 0,
			MSG_NO_PUT, NULL );

	// ウィンドウを書き込む
	BmpBoardWinWrite( &p_wk->win, WINDOW_TRANS_OFF, WFLBY_BOARDWINGRA_CGX, WFLBY_ROOM_BGPL_BOARDWIN, BOARD_TYPE_INFO );
	GF_BGL_BmpWinOnVReq( &p_wk->win );
}

//----------------------------------------------------------------------------
/**
 *	@brief	看板ウィンドウをOFFさせる
 *
 *	@param	p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_TalkWin_Board_Off( WFLBY_ROOM_TALKMSG* p_wk )
{
	// メッセージ表示中なら消す
	if( GF_MSG_PrintEndCheck( p_wk->msgno ) ){
		GF_STR_PrintForceStop( p_wk->msgno );
	}

	// タイムウエイトも消す
	WFLBY_ROOM_TalkWin_StopTimeWait_NoTrans( p_wk );

	// 全体を消す
	BmpBoardWinClear( &p_wk->win, BOARD_TYPE_INFO, WINDOW_TRANS_OFF );

	// Vリクエスト
	GF_BGL_BmpWinOffVReq( &p_wk->win );
}

//----------------------------------------------------------------------------
/**
 *	@brief	看板ボード　クリアウィンドウ
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_TalkWin_Board_ClearWin( WFLBY_ROOM_TALKMSG* p_wk )
{
	// メッセージ表示中なら消す
	if( GF_MSG_PrintEndCheck( p_wk->msgno ) ){
		GF_STR_PrintForceStop( p_wk->msgno );
	}

	// ビットマップのクリア
	GF_BGL_BmpWinDataFill( &p_wk->win, 15 );
}

//----------------------------------------------------------------------------
/**
 *	@brief	シンプルにメッセージを書き込む
 *
 *	@param	p_wk	ワーク
 *	@param	x		ｘ座標
 *	@param	y		ｙ座標
 *	@param	cp_str	文字列
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_TalkWin_Board_PrintSimple( WFLBY_ROOM_TALKMSG* p_wk, u8 x, u8 y, const STRBUF* cp_str )
{
	// 文字列コピー
	STRBUF_Copy( p_wk->p_str, cp_str );
	GF_STR_PrintSimple( &p_wk->win, FONT_TALK, p_wk->p_str, x, y,
			MSG_NO_PUT, NULL );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ウィンドウの表示	VBLANK中に
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_TalkWin_Board_OnVReq( WFLBY_ROOM_TALKMSG* p_wk )
{
	// ウィンドウを書き込む
	BmpBoardWinWrite( &p_wk->win, WINDOW_TRANS_OFF, WFLBY_BOARDWINGRA_CGX, WFLBY_ROOM_BGPL_BOARDWIN, BOARD_TYPE_INFO );
	GF_BGL_BmpWinOnVReq( &p_wk->win );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ビットマップウィンドウのパレット番号を設定
 *
 *	@param	p_wk	ワーク
 *	@param	pal		パレット番号
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_TalkWin_Board_SetPalNo( WFLBY_ROOM_TALKMSG* p_wk, u16 pal )
{
	// パレットナンバーだけ変更
	GF_BGL_BmpWinSet_Pal( &p_wk->win, pal );
}




//----------------------------------------------------------------------------
/**
 *	@brief	リストシステム		初期化
 *
 *	@param	p_wk	ワーク
 *	@param	p_sys	グラフィックワーク
 *	@param	heapID	ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_ListWin_Init( WFLBY_ROOM_LISTWIN* p_wk, WFLBY_GRAPHICCONT* p_sys, u32 heapID )
{
	CLACT_HEADER head;
	CLACT_ADD_SIMPLE add;
	int i;

	// アクターリソース読み込み
	ARCHANDLE* p_handle = ArchiveDataHandleOpen( ARC_SHOP_GRA, heapID );
	// リソース読み込み
	p_wk->resObj[ CLACT_U_CHAR_RES ] = CLACT_U_ResManagerResAddArcChar_ArcHandle( 
			p_sys->p_resman[ CLACT_U_CHAR_RES ],
			p_handle, NARC_shop_gra_shop_arw_NCGR, 
			FALSE, WFLBY_LISTWIN_CLACT_RESCONT_ID, 
			NNS_G2D_VRAM_TYPE_2DMAIN, heapID );	

	p_wk->resObj[ CLACT_U_PLTT_RES ] = CLACT_U_ResManagerResAddArcPltt_ArcHandle( 
			p_sys->p_resman[ CLACT_U_PLTT_RES ],
			p_handle, NARC_shop_gra_shop_obj_NCLR, 
			FALSE, WFLBY_LISTWIN_CLACT_RESCONT_ID, 
			NNS_G2D_VRAM_TYPE_2DMAIN, 1,  heapID );	

	p_wk->resObj[ CLACT_U_CELL_RES ] = CLACT_U_ResManagerResAddArcKindCell_ArcHandle( 
			p_sys->p_resman[ CLACT_U_CELL_RES ],
			p_handle, NARC_shop_gra_shop_arw_NCER, 
			FALSE, WFLBY_LISTWIN_CLACT_RESCONT_ID, 
			CLACT_U_CELL_RES, heapID );	

	p_wk->resObj[ CLACT_U_CELLANM_RES ] = CLACT_U_ResManagerResAddArcKindCell_ArcHandle( 
			p_sys->p_resman[ CLACT_U_CELLANM_RES ],
			p_handle, NARC_shop_gra_shop_arw_NANR, 
			FALSE, WFLBY_LISTWIN_CLACT_RESCONT_ID, 
			CLACT_U_CELLANM_RES, heapID );

	ArchiveDataHandleClose( p_handle );


	CLACT_U_CharManagerSetAreaCont( p_wk->resObj[ CLACT_U_CHAR_RES ] );
	CLACT_U_PlttManagerSetCleanArea( p_wk->resObj[ CLACT_U_PLTT_RES ] );

	// ヘッダー作成
	CLACT_U_MakeHeader(
			&head,
			WFLBY_LISTWIN_CLACT_RESCONT_ID,
			WFLBY_LISTWIN_CLACT_RESCONT_ID,
			WFLBY_LISTWIN_CLACT_RESCONT_ID,
			WFLBY_LISTWIN_CLACT_RESCONT_ID,
			CLACT_U_HEADER_DATA_NONE,
			CLACT_U_HEADER_DATA_NONE,
			0, 0,
			p_sys->p_resman[ 0 ],
			p_sys->p_resman[ 1 ],
			p_sys->p_resman[ 2 ],
			p_sys->p_resman[ 3 ],
			NULL, NULL );
	
	// アクター登録
	add.ClActSet = p_sys->p_clactset;
	add.ClActHeader = &head;
	add.pri		= 0;
	add.DrawArea = NNS_G2D_VRAM_TYPE_2DMAIN;
	add.heap	= heapID;
	add.mat.x	= WFLBY_LISTWIN_CLACT_X * FX32_ONE;
	for( i=0; i<WFLBY_LISTWIN_CLACT_OBJNUM; i++ ){
		add.mat.y	= sc_WFLBY_LISTWIN_CLACT_Y[ i ] * FX32_ONE;
		p_wk->clact[i] = CLACT_AddSimple( &add );
		CLACT_AnmChg( p_wk->clact[ i ], i );
		CLACT_SetAnmFlag( p_wk->clact[ i ], TRUE );
		CLACT_SetDrawFlag( p_wk->clact[ i ], FALSE );
	}

	p_wk->clact_draw = FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	リストシステム		破棄
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_ListWin_Exit( WFLBY_ROOM_LISTWIN* p_wk, WFLBY_GRAPHICCONT* p_sys )
{
	int i;
	
	// リストデータが残っていたらはき
	if( p_wk->p_bmplist != NULL ){
		WFLBY_ROOM_ListWin_DeleteBmpList( p_wk ); // TODO__fix_me
	}
	
	// 動作していたらすべてはき
	if( p_wk->p_listwk != NULL ){
		WFLBY_ROOM_ListWin_End( p_wk, NULL, NULL );
	}

	// アクター破棄
	for( i=0; i<WFLBY_LISTWIN_CLACT_OBJNUM; i++ ){
		CLACT_Delete( p_wk->clact[i] );
		p_wk->clact[i] = NULL;
	}

	CLACT_U_CharManagerDelete( p_wk->resObj[ CLACT_U_CHAR_RES ] );
	CLACT_U_PlttManagerDelete( p_wk->resObj[ CLACT_U_PLTT_RES ] );

	for( i=0; i<WFLBY_LISTWIN_CLACT_RESNUM; i++ ){
		CLACT_U_ResManagerResDelete( p_sys->p_resman[i], p_wk->resObj[i] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	項目リストを作成する
 *	
 *	@param	p_wk		ワーク
 *	@param	num			リスト数
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_ListWin_CreateBmpList( WFLBY_ROOM_LISTWIN* p_wk, u32 num, u32 heapID, u32 a3 )
{
	GF_ASSERT( p_wk->p_bmplist == NULL );
	p_wk->p_bmplist		= BMP_MENULIST_Create( num, heapID );
	p_wk->bmplistnum	= num;

    //MatchComment: fill loop
    {
        int i;

        for(i = 0; i < num; i++){
            p_wk->p_bmplist[ i ].param = a3;
        }
    }
}

//----------------------------------------------------------------------------
/**
 *	@brief	項目リストを破棄する
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_ListWin_DeleteBmpList( WFLBY_ROOM_LISTWIN* p_wk )
{
	if( p_wk->p_bmplist != NULL ){
		BMP_MENULIST_Delete( p_wk->p_bmplist );
		p_wk->p_bmplist		= NULL;
		p_wk->bmplistnum	= 0;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	項目の追加
 *	
 *	@param	p_wk		ワーク
 *	@param	cp_str		文字列
 *	@param	param		パラメータ
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_ListWin_SetBmpListStr( WFLBY_ROOM_LISTWIN* p_wk, const STRBUF* cp_str, u32 param )
{
	BMP_MENULIST_AddString( p_wk->p_bmplist, cp_str, param );
}

//----------------------------------------------------------------------------
/**
 *	@brief	項目リストを取得する
 *
 *	@param	cp_wk		ワーク
 *
 *	@return	項目リスト
 */	
//-----------------------------------------------------------------------------
static const BMP_MENULIST_DATA* WFLBY_ROOM_ListWin_GetBmpList( const WFLBY_ROOM_LISTWIN* cp_wk )
{
	return cp_wk->p_bmplist;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ビットマップリスト	にparamがもう入っているかチェックする
 *
 *	@param	cp_wk		ワーク
 *	@param	param		パラメータ
 *
 *	@retval	TRUE	ある
 *	@retval	FALSE	なし
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_ROOM_ListWin_CheckBmpListParam( const WFLBY_ROOM_LISTWIN* cp_wk, u32 param )
{
	int i;

	for( i=0; i<cp_wk->bmplistnum; i++ ){
		if( cp_wk->p_bmplist[i].param == param ){
			return  TRUE;
		}
	}
	return FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	リストシステム	表示開始
 *
 *	@param	p_wk		ワーク
 *	@param	cp_data		データ
 *	@param	p_sys		表示システム
 *	@param	list_p		初期化リスト位置
 *	@param	cursor_p	カーソル位置
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_ListWin_Start( WFLBY_ROOM_LISTWIN* p_wk, const BMPLIST_HEADER* cp_data, WFLBY_GRAPHICCONT* p_sys, u16 list_p, u16 cursor_p, u32 heapID, u8 cx, u8 cy, u8 szcx )
{
	GF_ASSERT( p_wk->p_listwk == NULL );
	// サイズが間に合っているかチェック
	GF_ASSERT( (cp_data->line*2) < WFLBY_LISTWIN_SIZY );

	// ヘッダーコピー
	p_wk->data		= *cp_data;
	p_wk->data.win	= &p_wk->win;

	// リスト総数
	p_wk->list_num	= cp_data->count;

	// リストコールバック設定
	p_wk->data.call_back = WFLBY_ROOM_ListWin_CurCallBack;

	// ビットマップだけ作成
	GF_BGL_BmpWinAdd(
				p_sys->p_bgl, &p_wk->win, 
				sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_MAIN_MSGWIN],
				cx, cy,
				szcx, cp_data->line*2, WFLBY_ROOM_BGPL_SYSFONT_CL,
				WFLBY_LISTWIN_CGX );

	// クリーン
	GF_BGL_BmpWinDataFill( &p_wk->win, 15 );
	
	// リスト作成
    // MatchComment: move BmpMenuWinWrite above BmpListSet call & assignment
    BmpMenuWinWrite( &p_wk->win, WINDOW_TRANS_OFF, WFLBY_SYSWINGRA_CGX, WFLBY_ROOM_BGPL_SYSWIN );
    p_wk->p_listwk = BmpListSet(&p_wk->data, list_p, cursor_p, heapID );

	// 表示
	GF_BGL_BmpWinOnVReq( &p_wk->win );
}

//----------------------------------------------------------------------------
/**
 *	@brief	リストシステム	メイン
 *
 *	@param	p_wk	ワーク
 *
 * @retval	"param = 選択パラメータ"
 * @retval	"BMPLIST_NULL = 選択中"
 * @retval	"BMPLIST_CANCEL	= キャンセル(Ｂボタン)"
 */
//-----------------------------------------------------------------------------
static u32 WFLBY_ROOM_ListWin_Main( WFLBY_ROOM_LISTWIN* p_wk )
{
	u32 result;
	
	if( p_wk->p_listwk == NULL ){
		return BMPLIST_CANCEL;
	}
	result = BmpListMain( p_wk->p_listwk );


	switch( result ){
	case BMPLIST_NULL:
	case BMPLIST_CANCEL:
		//  アクター制御
		WFLBY_ROOM_ListWin_ContClactDraw( p_wk );
		break;
		
	default:
		Snd_SePlay(WFLBY_SND_CURSOR);
		break;
	}

	return result;
}

//----------------------------------------------------------------------------
/**
 *	@brief	リストシステム	破棄
 *
 *	@param	p_wk		ワーク
 *	@param	p_list_p	リスト位置格納先
 *	@param	p_cursor_p	カーソル位置格納先
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_ListWin_End( WFLBY_ROOM_LISTWIN* p_wk, u16* p_list_p, u16* p_cursor_p )
{
	int i;
	
	if( p_wk->p_listwk == NULL ){
		return ;
	}
	BmpListExit( p_wk->p_listwk, p_list_p, p_cursor_p );
	p_wk->p_listwk = NULL;

	// ウィンドウを破棄
    BmpMenuWinClear( &p_wk->win, WINDOW_TRANS_OFF );
	GF_BGL_BmpWinOffVReq( &p_wk->win );
	GF_BGL_BmpWinDel( &p_wk->win );

	// OAM非表示
	p_wk->clact_draw = FALSE;
	for( i=0; i<WFLBY_LISTWIN_CLACT_OBJNUM; i++ ){
		CLACT_SetDrawFlag( p_wk->clact[i], FALSE );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ビットマップリストカーソルコールバック
 *
 *	@param	p_wk		リストワーク
 *	@param	param		パラメータ
 *	@param	mode		モード 
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_ListWin_CurCallBack( BMPLIST_WORK* p_wk,u32 param,u8 mode )
{
    if(mode == 0){
        Snd_SePlay(WFLBY_SND_CURSOR);
    }
}

//----------------------------------------------------------------------------
/**
 *	@brief	矢印アクターの表示設定
 *
 *	@param	p_wk	ワーク	
 *	@param	flag	フラグ
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_ListWin_SetClactDraw( WFLBY_ROOM_LISTWIN* p_wk, BOOL flag )
{
	p_wk->clact_draw = flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief	リスト　セルアクター表示制御
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_ListWin_ContClactDraw( WFLBY_ROOM_LISTWIN* p_wk )
{
	u16 list_pos;

	// 非表示中はずっと非表示
	if( p_wk->clact_draw == FALSE ){
		CLACT_SetDrawFlag( p_wk->clact[0], FALSE );
		CLACT_SetDrawFlag( p_wk->clact[1], FALSE );
		return ;
	}
	
	// リスト位置取得
	BmpListPosGet( p_wk->p_listwk, &list_pos, NULL );
	
	if( (list_pos <= 0) ){
		CLACT_SetDrawFlag( p_wk->clact[ 0 ], FALSE );
	}else{
		CLACT_SetDrawFlag( p_wk->clact[ 0 ], TRUE );
	}
	if( list_pos >= (p_wk->list_num - 7) ){
		CLACT_SetDrawFlag( p_wk->clact[1], FALSE );
	}else{
		CLACT_SetDrawFlag( p_wk->clact[1], TRUE );
	}
}



//----------------------------------------------------------------------------
/**
 *	@brief	サブウィンドウ	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		描画システム
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_SubWin_Init( WFLBY_ROOM_SUBWIN* p_wk, WFLBY_GRAPHICCONT* p_sys, u32 heapID )
{
	// とくになし
}

//----------------------------------------------------------------------------
/**
 *	@brief	サブウィンドウ	破棄
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_SubWin_Exit( WFLBY_ROOM_SUBWIN* p_wk )
{
	WFLBY_ROOM_SubWin_End( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	サブウィンドウ	開始
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		描画システム
 *	@param	heapID		ヒープID
 *	@param	cx			ｘキャラ位置
 *	@param	cy			ｙキャラ位置
 *	@param	szcx		ｘキャラサイズ
 *	@param	szcy		ｙキャラサイズ
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_SubWin_Start( WFLBY_ROOM_SUBWIN* p_wk, WFLBY_GRAPHICCONT* p_sys, u32 heapID, u8 cx, u8 cy, u8 szcx, u8 szcy )
{
	// ビットマップだけ作成
	GF_BGL_BmpWinAdd(
				p_sys->p_bgl, &p_wk->win, 
				sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_MAIN_MSGWIN],
				cx, cy,
				szcx, szcy, WFLBY_ROOM_BGPL_SYSFONT_CL,
				WFLBY_LISTWIN_CGX );

    BmpMenuWinWrite( &p_wk->win, WINDOW_TRANS_OFF, WFLBY_SYSWINGRA_CGX, WFLBY_ROOM_BGPL_SYSWIN );

	// クリーン
	GF_BGL_BmpWinDataFill( &p_wk->win, 15 );

	// 表示
	GF_BGL_BmpWinOnVReq( &p_wk->win );
}

//----------------------------------------------------------------------------
/**
 *	@brief	サブウィンドウ	終了
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_SubWin_End( WFLBY_ROOM_SUBWIN* p_wk )
{
	// ウィンドウを書きしていなければ破棄する
	if( GF_BGL_BmpWinAddCheck( &p_wk->win ) == TRUE ){
		// ウィンドウを破棄
		BmpMenuWinClear( &p_wk->win, WINDOW_TRANS_OFF );
		GF_BGL_BmpWinOffVReq( &p_wk->win );
		GF_BGL_BmpWinDel( &p_wk->win );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	サブウィンドウに文字列を描画
 *
 *	@param	p_wk		ワーク
 *	@param	cp_str		文字列
 *	@param	x			ｘ座標（ドット）
 *	@param	y			ｙ座標（ドット）
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_SubWin_PrintAll( WFLBY_ROOM_SUBWIN* p_wk, const STRBUF* cp_str, u8 x, u8 y )
{
	// 表示
	GF_STR_PrintColor( &p_wk->win, FONT_SYSTEM, cp_str, x, y,
			MSG_NO_PUT, WFLBY_SUBWIN_MSGCOL, NULL );

	GF_BGL_BmpWinOnVReq( &p_wk->win );
}

//----------------------------------------------------------------------------
/**
 *	@brief	サブウィンドウ表示をクリア
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_SubWin_Clear( WFLBY_ROOM_SUBWIN* p_wk )
{
	// クリーン
	GF_BGL_BmpWinDataFill( &p_wk->win, 15 );
}

//----------------------------------------------------------------------------
/**
 *	@brief	サブウィンドウ表示のクリア
 *
 *	@param	p_wk		ワーク
 *	@param	x			ｘ座標		全部ドット単位
 *	@param	y			ｙ座標
 *	@param	sx			サイズｘ
 *	@param	sy			サイズｙ
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_SubWin_ClearRect( WFLBY_ROOM_SUBWIN* p_wk, u16 x, u16 y, u16 sx, u16 sy )
{
	// クリーン
	GF_BGL_BmpWinFill( &p_wk->win, 15, x, y, sx, sy );
}




//----------------------------------------------------------------------------
/**
 *	@brief	YesNoウィンドウデータ作成
 *
 *	@param	p_wk		ワーク	
 *	@param	p_msg		メッセージデータ
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_YesNoWin_Init( WFLBY_ROOM_YESNOWIN* p_wk, WFLBY_ROOM_DEFMSG* p_msg, u32 heapID )
{
	STRBUF* p_str;

	GF_ASSERT( p_wk->p_bmplist == NULL );
	
	p_wk->p_bmplist = BMP_MENULIST_Create( WFLBY_YESNOWIN_DATANUM, heapID );

	// データ作成
	p_str = WFLBY_ROOM_Msg_Get(p_msg, WFLBY_DEFMSG_TYPE_HIROBA, msg_wifi_hirobawin_02 );
	BMP_MENULIST_AddString( p_wk->p_bmplist, p_str, WFLBY_ROOM_YESNO_OK );
	p_str = WFLBY_ROOM_Msg_Get(p_msg, WFLBY_DEFMSG_TYPE_HIROBA, msg_wifi_hirobawin_01 );
	BMP_MENULIST_AddString( p_wk->p_bmplist, p_str, WFLBY_ROOM_YESNO_NO );


	// ビットマップリストヘッダーに設定
	p_wk->bmplist			= sc_WFLBY_ROOM_YESNO_HEADER;
	p_wk->bmplist.count		= WFLBY_YESNOWIN_DATANUM;
	p_wk->bmplist.list		= p_wk->p_bmplist;
}

//----------------------------------------------------------------------------
/**
 *	@brief	YesNoウィンドウ　リストデータ破棄
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_YesNoWin_Exit( WFLBY_ROOM_YESNOWIN* p_wk )
{
	if( p_wk->p_bmplist != NULL ){
		BMP_MENULIST_Delete( p_wk->p_bmplist );
		p_wk->p_bmplist = NULL;
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	エラーウィンドウシステム	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		システムワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_ErrWin_Init( WFLBY_ROOM_ERRMSG* p_wk, WFLBY_GRAPHICCONT* p_sys, u32 heapID )
{
	//  ビットマップ確保
	GF_BGL_BmpWinAdd(
				p_sys->p_bgl, &p_wk->win, 
				sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_MAIN_MSGWIN],
				WFLBY_ERRWIN_X, WFLBY_ERRWIN_Y,
				WFLBY_ERRWIN_SIZX, WFLBY_ERRWIN_SIZY, WFLBY_ROOM_BGPL_SYSFONT_CL,
				WFLBY_ERRWIN_CGX );

	// クリーン
	GF_BGL_BmpWinDataFill( &p_wk->win, 15 );
}

//----------------------------------------------------------------------------
/**
 *	@brief	エラー表示システム	破棄
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_ErrWin_Exit( WFLBY_ROOM_ERRMSG* p_wk )
{
	// ビットマップ破棄
	GF_BGL_BmpWinDel( &p_wk->win );
}

//----------------------------------------------------------------------------
/**
 *	@brief	エラーメッセージの表示
 *	
 *	@param	p_wk		ワーク
 *	@param	cp_str		文字列
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_ErrWin_DrawErr( WFLBY_ROOM_ERRMSG* p_wk, const STRBUF* cp_str )
{
	GF_STR_PrintColor( &p_wk->win, FONT_SYSTEM, cp_str, 0, 0,
			MSG_NO_PUT, WFLBY_ERRWIN_MSGCOL, NULL );

	// ウィンドウを書き込む
	BmpMenuWinWrite( &p_wk->win, WINDOW_TRANS_OFF, WFLBY_SYSWINGRA_CGX, WFLBY_ROOM_BGPL_SYSWIN );
	GF_BGL_BmpWinOnVReq( &p_wk->win );
}

//----------------------------------------------------------------------------
/**
 *	@brief	DWCエラーの表示処理
 *
 *	@param	p_wk		ワーク
 *	@param	p_msg		メッセージワーク
 *	@param	errno		エラーナンバー
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_ErrWin_DrawDwcErr( WFLBY_ROOM_ERRMSG* p_wk, WFLBY_ROOM_DEFMSG* p_msg )
{
	u32 msgno;
	STRBUF*  p_str;
    COMMSTATE_DWCERROR* pErr;

    pErr = CommStateGetWifiError();
	// メッセージ取得
	msgno = WFLBY_ERR_GetStrID(  pErr->errorCode,  pErr->errorType );	// メッセージＮＯ取得
	WFLBY_ROOM_Msg_SetNumber( p_msg, pErr->errorCode, 5, 0, NUMBER_DISPTYPE_ZERO );	// ＥＲＲＮＯ設定
	p_str = WFLBY_ROOM_Msg_Get( p_msg, WFLBY_DEFMSG_TYPE_ERR, msgno );

	// 描画
	WFLBY_ROOM_ErrWin_DrawErr( p_wk, p_str );

}

//----------------------------------------------------------------------------
/**
 *	@brief	ロビー内エラーの表示
 *
 *	@param	p_wk		ワーク
 *	@param	p_msg		メッセージシステム
 *	@param	errno		エラーナンバー
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_ErrWin_DrawLobbyErr( WFLBY_ROOM_ERRMSG* p_wk, WFLBY_ROOM_DEFMSG* p_msg, int errno )
{
	u32 msgno;
	s32 draw_no;
	STRBUF*  p_str;

	draw_no = DWC_LOBBY_GetErrNo( errno );

	// メッセージ取得
	WFLBY_ROOM_Msg_SetNumber( p_msg, draw_no, 5, 0, NUMBER_DISPTYPE_ZERO );	// ＥＲＲＮＯ設定
	p_str = WFLBY_ROOM_Msg_Get( p_msg, WFLBY_DEFMSG_TYPE_ERR, dwc_lobby_0001 );

	// 描画
	WFLBY_ROOM_ErrWin_DrawErr( p_wk, p_str );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ロビーシステム処理　の	エラー表示
 *
 *	@param	p_wk		ワーク
 *	@param	p_msg		メッセージシステム
 *	@param	type		エラータイプ
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_ErrWin_DrawSystemErr( WFLBY_ROOM_ERRMSG* p_wk, WFLBY_ROOM_DEFMSG* p_msg, WFLBY_SYSTEM_ERR_TYPE type )
{
	STRBUF*  p_str;

	// メッセージ取得
	p_str = WFLBY_ROOM_Msg_Get( p_msg, WFLBY_DEFMSG_TYPE_ERR, dwc_error_0015 );
	// 描画
	WFLBY_ROOM_ErrWin_DrawErr( p_wk, p_str );
}




//----------------------------------------------------------------------------
/**
 *	@brief	メッセージシステム作成	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	heapID		ヒープ
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_Msg_Init( WFLBY_ROOM_DEFMSG* p_wk, u32 heapID )
{
	int i;
	static const u32 sc_DEFMSG_INX[ WFLBY_DEFMSG_TYPE_NUM ] = {
		NARC_msg_wifi_h_info_dat,
		NARC_msg_wifi_hiroba_dat,
		NARC_msg_wifi_system_dat,
		NARC_msg_wifi_aisatu_dat
	};
	
	p_wk->p_wordset = WORDSET_CreateEx( WORDSET_DEFAULT_SETNUM, WORDSET_COUNTRY_BUFLEN, heapID );
	for( i=0; i<WFLBY_DEFMSG_TYPE_NUM; i++ ){
		p_wk->p_msgman[i]	= MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, sc_DEFMSG_INX[i], heapID );
	}
	p_wk->p_str		= STRBUF_Create( WFLBY_DEFMSG_STRNUM, heapID );
	p_wk->p_tmp		= STRBUF_Create( WFLBY_DEFMSG_STRNUM, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージ	破棄
 *
 *	@param	p_wk		ワーク
 */	
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_Msg_Exit( WFLBY_ROOM_DEFMSG* p_wk )
{
	int i;
	WORDSET_Delete( p_wk->p_wordset );
	for( i=0; i<WFLBY_DEFMSG_TYPE_NUM; i++ ){
		MSGMAN_Delete( p_wk->p_msgman[i] );
	}
	STRBUF_Delete( p_wk->p_str );
	STRBUF_Delete( p_wk->p_tmp );
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージ取得
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static STRBUF* WFLBY_ROOM_Msg_Get( WFLBY_ROOM_DEFMSG* p_wk, WFLBY_DEFMSG_TYPE type, u32 strid )
{
	MSGMAN_GetString( p_wk->p_msgman[type], strid, p_wk->p_tmp );
	WORDSET_ExpandStr( p_wk->p_wordset, p_wk->p_str, p_wk->p_tmp );
	return p_wk->p_str;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ワードセットに数字を設定
 *
 *	@param	p_wk		ワーク
 *	@param	num			数字
 *	@param	keta		桁
 *	@param	bufid		バッファＩＤ
 *	@param	disptype	左つめとか
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_Msg_SetNumber( WFLBY_ROOM_DEFMSG* p_wk, u32  num, u32 keta, u32 bufid, NUMBER_DISPTYPE disptype )
{
	WORDSET_RegisterNumber( p_wk->p_wordset, bufid, num, keta, disptype, NUMBER_CODETYPE_DEFAULT );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ワードセットにプレイヤー名を設定
 *
 *	@param	p_wk		ワーク
 *	@param	cp_mystatus	MYSTATUS
 *	@param	bufid		バッファID
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_Msg_SetPlayerName( WFLBY_ROOM_DEFMSG* p_wk, const MYSTATUS* cp_mystatus, u32 bufid )
{
	WORDSET_RegisterPlayerName( p_wk->p_wordset, bufid, cp_mystatus );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ミニゲームを設定
 *
 *	@param	p_wk		ワーク
 *	@param	type		ミニゲームタイプ
 *	@param	bufid		バッファID
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_Msg_SetMinigame( WFLBY_ROOM_DEFMSG* p_wk, WFLBY_GAMETYPE type, u32 bufid )
{
	WORDSET_RegisterWiFiLobbyGameName( p_wk->p_wordset, bufid, type );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ミニゲームを設定
 *
 *	@param	p_wk		ワーク
 *	@param	type		タイムイベントタイプ
 *	@param	bufid		バッファID
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_Msg_SetTimeevent( WFLBY_ROOM_DEFMSG* p_wk, WFLBY_EVENTGMM_TYPE type, u32 bufid )
{
	WORDSET_RegisterWiFiLobbyEventName( p_wk->p_wordset, bufid, type );
}

//----------------------------------------------------------------------------
/**
 *	@brief	アイテムを設定
 *
 *	@param	p_wk		ワーク
 *	@param	item		ガジェットタイプ
 *	@param	bufid		バッファID
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_Msg_SetItem( WFLBY_ROOM_DEFMSG* p_wk, WFLBY_ITEMTYPE item, u32 bufid )
{
	WORDSET_RegisterWiFiLobbyItemName( p_wk->p_wordset, bufid, item );
}

//----------------------------------------------------------------------------
/**
 *	@brief	国名を設定
 *
 *	@param	p_wk		ワーク	
 *	@param	country		国ID
 *	@param	bufid		バッファID
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_Msg_SetCountry( WFLBY_ROOM_DEFMSG* p_wk, u32 country, u32 bufid )
{
	WORDSET_RegisterCountryName( p_wk->p_wordset, bufid, country );
}

//----------------------------------------------------------------------------
/**
 *	@brief	地域名を設定
 *
 *	@param	p_wk		ワーク
 *	@param	country		国ID
 *	@param	area		地域ID
 *	@param	bufid		バッファID
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_Msg_SetArea( WFLBY_ROOM_DEFMSG* p_wk, u32 country, u32 area, u32 bufid )
{
	WORDSET_RegisterLocalPlaceName( p_wk->p_wordset, bufid, country, area );
}

//----------------------------------------------------------------------------
/**
 *	@brief	簡易会話を設定
 *
 *	@param	p_wk		ワーク
 *	@param	bufID		バッファID
 *	@param	word		単語ID
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_Msg_SetPMSWord( WFLBY_ROOM_DEFMSG* p_wk, u32 bufID, PMS_WORD word )
{
	WORDSET_RegisterPMSWord( p_wk->p_wordset, bufID, word );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ワードセットのバッファクリア
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_Msg_ClearWordSet( WFLBY_ROOM_DEFMSG* p_wk )
{
	WORDSET_ClearAllBuffer( p_wk->p_wordset ) ;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ガジェット起動フラグを設定する
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_GadGet_SetFlag( WFLBY_ROOMWK* p_wk )
{
	p_wk->gadget_flag = TRUE;
}




//----------------------------------------------------------------------------
/**
 *	@brief	した画面初期化処理
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		システムワーク
 *	@param	cp_mystatus	MYSTATUS
 *	@param	heapID		ヒープ
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_Init( WFLBY_UNDER_WIN* p_wk, const WFLBY_ROOM_SAVE* cp_save, WFLBY_GRAPHICCONT* p_sys, const MYSTATUS* cp_mystatus, u32 heapID )
{
	u32 sex;

	// 性別取得
	sex = MyStatus_GetMySex( cp_mystatus );
	
	p_wk->p_handle = ArchiveDataHandleOpen( ARC_WIFILOBBY_OTHER_GRA, heapID );
	WFLBY_ROOM_UNDERWIN_Common_Init( p_wk, p_sys, p_wk->p_handle, sex, heapID );

	// トレーナカード
	WFLBY_ROOM_UNDERWIN_TrCard_Init( &p_wk->tr_card, p_sys, p_wk->p_handle, heapID );

	// ボタンの初期化
	WFLBY_ROOM_UNDERWIN_Button_Init( &p_wk->bttn, p_sys, p_wk->p_handle, heapID );
	
	// シーケンス状態を調整
	p_wk->req = WFLBY_UNDERWIN_DRAW_NONE;
	p_wk->now = WFLBY_UNDERWIN_DRAW_NONE;
	if( cp_save->pl_inevno == WFLBY_EV_DEF_PLAYERIN_NORMAL ){
		p_wk->seq = WFLBY_UNDERWIN_SEQ_STARTWAIT;

		// バックパレットカラーを黒にしておく
		GF_BGL_BackGroundColorSet( GF_BGL_FRAME0_S, 0 );
		
		// サブ面全部非表示
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );
	}else{
		p_wk->seq = WFLBY_UNDERWIN_SEQ_NORMAL;
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	した画面破棄処理
 *
 *	@param	p_wk		ワーク
 *	@param	p_boardwin	看板ワーク
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_Exit( WFLBY_UNDER_WIN* p_wk, WFLBY_ROOM_TALKMSG* p_boardwin, WFLBY_GRAPHICCONT* p_sys )
{
	// ボタン破棄
	WFLBY_ROOM_UNDERWIN_Button_Exit( &p_wk->bttn );
	
	// トレーナカード
	WFLBY_ROOM_UNDERWIN_TrCard_Exit( &p_wk->tr_card, p_boardwin, p_sys );

	WFLBY_ROOM_UNDERWIN_Common_Exit( p_wk, p_sys );
	ArchiveDataHandleClose( p_wk->p_handle );
}

//----------------------------------------------------------------------------
/**
 *	@brief	メイン動作
 *
 *	@param	p_wk		ワーク
 *	@param	p_room		部屋ワーク
 *	@param	no_input	ボタン入力など受け付けないとき：TRUE
 *	@param	heapID
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_Main( WFLBY_UNDER_WIN* p_wk, WFLBY_ROOMWK* p_room, BOOL no_input, u32 heapID )
{
	WFLBY_SYSTEM*		p_system;
	WFLBY_GRAPHICCONT*	p_sys;
	WFLBY_ROOM_DEFMSG*	p_msg;
	BOOL result;

	p_system	= p_room->p_system;
	p_sys		= &p_room->graphic;
	p_msg		= &p_room->def_msg;
	
	// 動作シーケンス
	switch( p_wk->seq ){
	// 開始待ち
	case WFLBY_UNDERWIN_SEQ_STARTWAIT:			
		if( p_wk->bttn_start == TRUE ){
			p_wk->seq ++;
		}
		break;
	// 開始フェードアウト
	case WFLBY_UNDERWIN_SEQ_STARTFADEOUT:		
		WIPE_SYS_Start(WIPE_PATTERN_S, 
				WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
				WIPE_FADE_WHITE, 4, 1, heapID );
		p_wk->seq ++;
		break;
	// 開始フェードアウト待ち
	case WFLBY_UNDERWIN_SEQ_STARTFADEOUTWAIT:	
		if( WIPE_SYS_EndCheck() == TRUE ){

			// 自分のトレーナカードを表示
#if 0
			{
				const WFLBY_USER_PROFILE* cp_myprofile;
				BOOL vip;

				// 自分のプロフィール取得
				cp_myprofile = WFLBY_SYSTEM_GetMyProfileLocal( p_system );

				// VIPデータを取得
				vip			= WFLBY_SYSTEM_GetUserVipFlag( p_system, WFLBY_SYSTEM_GetMyIdx( p_system ) );
				WFLBY_ROOM_UNDERWIN_StartTrCard( p_wk, &p_room->boardwin, p_sys, p_msg, TRUE, heapID, 
						cp_myprofile, cp_myprofile, vip, FALSE, NULL, FALSE );

				// 状態をスタートプロフィール状態にする
				p_wk->now = WFLBY_UNDERWIN_DRAW_PROFILE;
			}
#endif
			// ガジェット表示
			{
				const WFLBY_USER_PROFILE* cp_profile;
				cp_profile = WFLBY_SYSTEM_GetMyProfileLocal( p_system );
				WFLBY_ROOM_UNDERWIN_StartBttn( p_wk, p_sys,  &p_room->def_msg, heapID, cp_profile );
				p_wk->now = WFLBY_UNDERWIN_DRAW_BTTN_DUMMY;
			}

			// サブ面全部表示開始
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
			p_wk->seq ++;
		}
		break;
	// 開始フェードイン
	case WFLBY_UNDERWIN_SEQ_STARTFADEIN:			
		WIPE_SYS_Start(WIPE_PATTERN_S, 
				WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
				WIPE_FADE_WHITE, 6, 1, heapID );
		p_wk->seq ++;
		break;
	// 開始フェードイン待ち
	case WFLBY_UNDERWIN_SEQ_STARTFADEINWAIT:		
		if( WIPE_SYS_EndCheck() == TRUE ){
			p_wk->seq ++;
		}
		break;

	case WFLBY_UNDERWIN_SEQ_NORMAL:

		// 入力を受け付けない場合
		// なにもしない
		if( no_input == TRUE ){
			break;
		}


		switch( p_wk->now ){
		// ガジェットボタン
		case WFLBY_UNDERWIN_DRAW_BTTN:
			result = WFLBY_ROOM_UNDERWIN_Button_Main( &p_wk->bttn, p_sys, p_wk->p_handle, heapID );
			if( result == TRUE ){
				// 自分のガジェット起動
				WFLBY_ROOM_GadGet_SetFlag( p_room );
			}
			break;

		// ガジェットボタン　ダミー
		case WFLBY_UNDERWIN_DRAW_BTTN_DUMMY:
			break;
			
		// フロート
		case WFLBY_UNDERWIN_DRAW_FLOAT:
			// アニメ再生中なら再生中のアニメ終了待ち
			if( p_wk->flt_anm_flag ){
				if( WFLBY_TIMEEVENT_FLOAT_CheckAnm( p_room->p_timeevent, 
							p_wk->flt_float_idx, p_wk->flt_anm_idx ) == FALSE ){
					p_wk->flt_anm_flag = FALSE;

					// ボタン復帰
					WFLBY_ROOM_UNDERWIN_Button_BttnLockClear( &p_wk->bttn );
				}
			}

			
			result = WFLBY_ROOM_UNDERWIN_Button_Main( &p_wk->bttn, p_sys, p_wk->p_handle, heapID );
			if( result == TRUE ){
				// フロートアニメ起動
				WFLBY_SYSTEM_SetFloatAnm( p_system, WFLBY_SYSTEM_GetMyIdx( p_system ) );
				WFLBY_SYSTEM_SendFloatAnm( p_system );	// アニメデータ送信

				// 再生したフロートアニメ情報取得
				{
					u32 reserve_idx, float_idx, float_ofs;

					reserve_idx = WFLBY_SYSTEM_GetFloatPlIdxReserve( p_system, WFLBY_SYSTEM_GetMyIdx( p_system ) );
					WFLBY_SYSTEM_GetFloatIdxOfs( reserve_idx, &float_idx, &float_ofs );

					p_wk->flt_anm_flag	= TRUE;
					p_wk->flt_float_idx	= float_idx;
					p_wk->flt_anm_idx	= float_ofs;
				}
			}
			break;

		// プロフィール
		case WFLBY_UNDERWIN_DRAW_PROFILE:
			WFLBY_ROOM_UNDERWIN_TrCard_Main( &p_wk->tr_card, &p_room->boardwin );
			break;

		default:
			break;
		}


		// 変更リクエストが来ていたら変更する
		if( p_wk->req != WFLBY_UNDERWIN_DRAW_NONE ){
			p_wk->seq ++;
		}
		break;

	case WFLBY_UNDERWIN_SEQ_FADEOUT:

		WIPE_SYS_Start(WIPE_PATTERN_S, 
				WIPE_TYPE_FADEIN, WIPE_TYPE_FADEOUT, 
				WIPE_FADE_BLACK, WFLBY_UNDERWIN_FASE_DIV, WFLBY_UNDERWIN_FASE_SYNC, heapID );
		p_wk->seq ++;
		break;

	case WFLBY_UNDERWIN_SEQ_FADEOUTWAIT:
		if( WIPE_SYS_EndCheck() == TRUE ){

			// 破棄処理
			switch( p_wk->now ){
			case WFLBY_UNDERWIN_DRAW_BTTN:
			case WFLBY_UNDERWIN_DRAW_BTTN_DUMMY:
			case WFLBY_UNDERWIN_DRAW_FLOAT:
				WFLBY_ROOM_UNDERWIN_EndBttn( p_wk, p_sys );
				break;

			case WFLBY_UNDERWIN_DRAW_PROFILE:
				WFLBY_ROOM_UNDERWIN_EndTrCard( p_wk, &p_room->boardwin, p_sys );
				break;
			}
			
			p_wk->seq ++;
		}
		break;

	case WFLBY_UNDERWIN_SEQ_CHANGE:
		{
			const WFLBY_USER_PROFILE* cp_profile;
			BOOL vip;
			BOOL aikotoba;
			const WFLBY_AIKOTOBA_DATA* cp_aikotoba;

#if PL_G0200_080716_FIX
			// プロフィールの取得が不可能なのであれば、ボタン表示にする
			if( p_wk->req == WFLBY_UNDERWIN_DRAW_PROFILE ){
				// 自分かチェック
				if( p_wk->req_tridx != WFLBY_SYSTEM_GetMyIdx( p_system ) ){
					cp_profile = WFLBY_SYSTEM_GetUserProfile( p_system, p_wk->req_tridx );
					if( cp_profile == NULL ){
						p_wk->req = WFLBY_UNDERWIN_DRAW_BTTN;
					}
				}
			}
#endif
			

			switch( p_wk->req ){
			case WFLBY_UNDERWIN_DRAW_BTTN:
			case WFLBY_UNDERWIN_DRAW_BTTN_DUMMY:
				cp_profile = WFLBY_SYSTEM_GetMyProfileLocal( p_system );
				WFLBY_ROOM_UNDERWIN_StartBttn( p_wk, p_sys,  &p_room->def_msg, heapID, cp_profile );
				break;

			case WFLBY_UNDERWIN_DRAW_PROFILE:
				{
					BOOL  mydata;
					const WFLBY_USER_PROFILE* cp_myprofile;

					// 自分のプロフィール取得
					cp_myprofile = WFLBY_SYSTEM_GetMyProfileLocal( p_system );

					// 自分かチェック
					if( p_wk->req_tridx == WFLBY_SYSTEM_GetMyIdx( p_system ) ){
						cp_profile = WFLBY_SYSTEM_GetMyProfileLocal( p_system );
						mydata = TRUE;
					}else{
						cp_profile = WFLBY_SYSTEM_GetUserProfile( p_system, p_wk->req_tridx );
						mydata = FALSE;
					}

					// VIPデータを取得
					vip			= WFLBY_SYSTEM_GetUserVipFlag( p_system, p_wk->req_tridx );
					if( p_wk->req_aikotoba == TRUE ){	// 合言葉表示があるときだけ合言葉の情報を取得する
						aikotoba	= WFLBY_SYSTEM_GetUserAikotobaFlag( p_system, p_wk->req_tridx );
						cp_aikotoba	= WFLBY_SYSTEM_GetUserAikotobaStr( p_system, p_wk->req_tridx );
						if( cp_aikotoba == NULL ){	// 合言葉がなかったら絶対に合言葉表示なしにする（念のため）
							aikotoba = FALSE;
						}
					}else{
						aikotoba	= FALSE;
						cp_aikotoba	= NULL;
					}
					WFLBY_ROOM_UNDERWIN_StartTrCard( p_wk, &p_room->boardwin, p_sys, p_msg, mydata, heapID, cp_profile, cp_myprofile, vip, aikotoba, cp_aikotoba, TRUE );
				}
				break;

			case WFLBY_UNDERWIN_DRAW_FLOAT:
				{
					u32 idx;
					u32 floatidx;
					u32 floatoffs;
					idx = WFLBY_SYSTEM_GetFloatPlIdxReserve( p_system, WFLBY_SYSTEM_GetMyIdx( p_system ) );
					if( idx == DWC_LOBBY_USERIDTBL_IDX_NONE ){
						GF_ASSERT(0);
						// 予約してない
						idx = 0;	// 0にしてしまう
					}
					WFLBY_SYSTEM_GetFloatIdxOfs( idx, &floatidx, &floatoffs );
					WFLBY_ROOM_UNDERWIN_StartBttnFloat( p_wk, p_sys, heapID, floatoffs );
				}
				break;
			}

			
			p_wk->req = WFLBY_UNDERWIN_DRAW_NONE;
			p_wk->seq ++;
		}
		break;

	case WFLBY_UNDERWIN_SEQ_FASEIN:
		WIPE_SYS_Start(WIPE_PATTERN_S, 
				WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
				WIPE_FADE_BLACK, WFLBY_UNDERWIN_FASE_DIV, WFLBY_UNDERWIN_FASE_SYNC, heapID );

		// 次がトレーナーカードなら一度トレーナーカードのメインを実行
		if( p_wk->now == WFLBY_UNDERWIN_DRAW_PROFILE ){
			WFLBY_ROOM_UNDERWIN_TrCard_Main( &p_wk->tr_card, &p_room->boardwin );
		}
		
		p_wk->seq ++;
		break;

	case WFLBY_UNDERWIN_SEQ_FASEINWAIT:

		if( WIPE_SYS_EndCheck() == TRUE ){
			p_wk->seq = WFLBY_UNDERWIN_SEQ_NORMAL;
		}
		break;

	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	した画面がフェード中でないかチェック
 *
 *	@param	p_wk	ワーク
 *
 *	@retval	TRUE	フェード中
 *	@retval	FALSE	フェードしてない
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_ROOM_UNDERWIN_CheckFade( const WFLBY_UNDER_WIN* cp_wk )
{
	if( cp_wk->seq == WFLBY_UNDERWIN_SEQ_NORMAL ){
		return FALSE;
	}
	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	した画面のシーケンスを取得する
 *
 *	@param	cp_wk	ワーク
 *
 *	@return	シーケンス
 */
//-----------------------------------------------------------------------------
static u32 WFLBY_ROOM_UNDERWIN_GetSeq( const WFLBY_UNDER_WIN* cp_wk )
{
	return cp_wk->seq;
}

//----------------------------------------------------------------------------
/**
 *	@brief	表示切替リクエスト
 *
 *	@param	p_wk		ワーク
 *	@param	idx			インデックス
 *	@param	aikotoba	合言葉をひょうじするのか
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_ReqTrCard( WFLBY_UNDER_WIN* p_wk, u32 idx, BOOL aikotoba )
{
	p_wk->req			= WFLBY_UNDERWIN_DRAW_PROFILE;
	p_wk->req_tridx		= idx;
	p_wk->req_aikotoba	= aikotoba;
}

//----------------------------------------------------------------------------
/**
 *	@brief	した画面トレーナカード	アイテムアイコン変更
 *	
 *	@param	p_wk	ワーク
 *	@param	p_sys	グラフィックワーク
 *	@param	item	アイテム
 *	@param	heapID	ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_ChangeItemTrCard( WFLBY_UNDER_WIN* p_wk, WFLBY_GRAPHICCONT* p_sys, u32 item, u32 heapID )
{
	GF_ASSERT( item < WFLBY_ITEM_NUM );
	GF_ASSERT( p_wk->now == WFLBY_UNDERWIN_DRAW_PROFILE );

	// へんこうできるのか？
	if( ( p_wk->now == WFLBY_UNDERWIN_DRAW_PROFILE ) ){
		// 変更
		WFLBY_ROOM_UNDERWIN_TrCard_ChangeItem( &p_wk->tr_card, p_sys, p_wk->p_handle, heapID, item );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ボタン切り替えリクエスト
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_ReqBttn( WFLBY_UNDER_WIN* p_wk )
{
	if( p_wk->now != WFLBY_UNDERWIN_DRAW_BTTN ){
		p_wk->req = WFLBY_UNDERWIN_DRAW_BTTN;
	} 
}

//----------------------------------------------------------------------------
/**
 *	@brief	フロートボタン切り替えリクエスト
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_ReqBttnFloat( WFLBY_UNDER_WIN* p_wk )
{
	if( p_wk->now != WFLBY_UNDERWIN_DRAW_FLOAT ){
		p_wk->req = WFLBY_UNDERWIN_DRAW_FLOAT;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナカードのプレイヤーインデックスを取得する
 *
 *	@param	cp_wk	ワーク
 *
 *	@return	プレイヤーインデックス
 */
//-----------------------------------------------------------------------------
static u32 WFLBY_ROOM_UNDERWIN_GetTrCardPlIdx( const WFLBY_UNDER_WIN* cp_wk )
{
	return cp_wk->req_tridx;
}

//----------------------------------------------------------------------------
/**
 *	@brief	trainerカードを表示しているかを取得する
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	trainerカード表示中
 *	@retval	FALSE	trainerカード表示してない
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_ROOM_UNDERWIN_CheckTrCardDraw( const WFLBY_UNDER_WIN* cp_wk )
{
	if( ( cp_wk->req == WFLBY_UNDERWIN_DRAW_PROFILE ) ){
		return TRUE;
	}
	if( ( cp_wk->now == WFLBY_UNDERWIN_DRAW_PROFILE ) ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナカードをOFFにすることがかのうか？
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	可能
 *	@retval	FALSE	不可能
 *
 *	ちょっとわかりにくいですが、
 *	リクエストがNONEでトレーナーカード表示中か？ということです。
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_ROOM_UNDERWIN_CheckTrCardCanOff( const WFLBY_UNDER_WIN* cp_wk )
{
	if( ( cp_wk->req == WFLBY_UNDERWIN_DRAW_NONE ) && ( cp_wk->now == WFLBY_UNDERWIN_DRAW_PROFILE ) ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナーカード合言葉を表示しているかチェック
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	表示してる
 *	@retval	FALSE	表示してない
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_ROOM_UNDERWIN_CheckTrCardAikotobaDraw( const WFLBY_UNDER_WIN* cp_wk )
{
	return WFLBY_ROOM_UNDERWIN_TrCard_CheckAikotobaDraw( &cp_wk->tr_card );
}

#if PL_T0862_080712_FIX
//----------------------------------------------------------------------------
/**
 *	@brief	リクエストがかかっているかチェック
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	かかっている
 *	@retval	FALSE	かかってない
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_ROOM_UNDERWIN_CheckReqNone( const WFLBY_UNDER_WIN* cp_wk )
{
	if( cp_wk->req != WFLBY_UNDERWIN_DRAW_NONE ){
		return  TRUE;
	}
	return FALSE;
}
#endif

//----------------------------------------------------------------------------
/**
 *	@brief	ステータス表示開始
 *
 *	@param	p_wk		ワーク
 *	@param	p_boardwin	看板ウィンドウシステム
 *	@param	p_sys		グラフィックシステム
 *	@param	p_msg		メッセージ
 *	@param	mydata		自分のデータか
 *	@param	heapID		ヒープＩＤ	
 *	@param	cp_profile	ユーザプロフィール
 *	@param	cp_myprofileユーザプロフィール
 *	@param	vip			VIPフラグ
 *	@param	aikotoba	合言葉描画フラグ
 *	@param	cp_aikotoba	合言葉文字列
 *	@param	draw_item	アイテムを表示するか
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_StartTrCard( WFLBY_UNDER_WIN* p_wk, WFLBY_ROOM_TALKMSG* p_boardwin, WFLBY_GRAPHICCONT* p_sys, WFLBY_ROOM_DEFMSG* p_msg, BOOL mydata, u32 heapID, const WFLBY_USER_PROFILE* cp_profile, const WFLBY_USER_PROFILE* cp_myprofile, BOOL vip, BOOL aikotoba, const WFLBY_AIKOTOBA_DATA* cp_aikotoba, BOOL draw_item )
{

	// プロフィールを表示中ならいったん消す
	if( p_wk->now == WFLBY_UNDERWIN_DRAW_PROFILE ){
		WFLBY_ROOM_UNDERWIN_EndTrCard( p_wk, p_boardwin, p_sys );
	}
	
	WFLBY_ROOM_UNDERWIN_TrCard_Start( p_wk, p_boardwin, &p_wk->tr_card, p_sys, p_msg, mydata, p_wk->p_handle, heapID, cp_profile, cp_myprofile, vip, aikotoba, cp_aikotoba, draw_item );
	p_wk->now = WFLBY_UNDERWIN_DRAW_PROFILE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナカード表示ＯＦＦ
 *
 *	@param	p_wk		ワーク
 *	@param	p_boardwin	看板ウィンドウ
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_EndTrCard( WFLBY_UNDER_WIN* p_wk, WFLBY_ROOM_TALKMSG* p_boardwin, WFLBY_GRAPHICCONT* p_sys )
{
	WFLBY_ROOM_UNDERWIN_TrCard_End( &p_wk->tr_card, p_boardwin, p_sys );
	p_wk->now = WFLBY_UNDERWIN_DRAW_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ボタンの起動
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		システム
 *	@param	p_msg		メッセージ
 *	@param	heapID		ヒープID
 *	@param	cp_profile	プロフィール
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_StartBttn( WFLBY_UNDER_WIN* p_wk, WFLBY_GRAPHICCONT* p_sys, WFLBY_ROOM_DEFMSG* p_msg, u32 heapID, const WFLBY_USER_PROFILE* cp_profile )
{
	// ボタンを表示中ならいったん消す
	if( (p_wk->now == WFLBY_UNDERWIN_DRAW_BTTN) ||
		(p_wk->now == WFLBY_UNDERWIN_DRAW_BTTN_DUMMY) ||
		(p_wk->now == WFLBY_UNDERWIN_DRAW_FLOAT) ){
		WFLBY_ROOM_UNDERWIN_EndBttn( p_wk, p_sys );
	}
	
	WFLBY_ROOM_UNDERWIN_Button_Start( &p_wk->bttn, p_sys, p_msg, cp_profile, 
			p_wk->p_handle, heapID );
	p_wk->now = WFLBY_UNDERWIN_DRAW_BTTN;
}

//----------------------------------------------------------------------------
/**
 *	@brief	フロートボタンの起動
 *
 *	@param	p_wk			ワーク
 *	@param	p_sys			システム
 *	@param	heapID			ヒープＩＤ
 *	@param	idx				フロートたっているところインデックス
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_StartBttnFloat( WFLBY_UNDER_WIN* p_wk, WFLBY_GRAPHICCONT* p_sys, u32 heapID, u32 idx )
{
	// ボタンを表示中ならいったん消す
	if( p_wk->now == WFLBY_UNDERWIN_DRAW_FLOAT ){
		WFLBY_ROOM_UNDERWIN_EndBttn( p_wk, p_sys );
	}
	
	WFLBY_ROOM_UNDERWIN_Button_StartFloat( &p_wk->bttn, p_sys, idx, 
			p_wk->p_handle, heapID );
	p_wk->now = WFLBY_UNDERWIN_DRAW_FLOAT;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ボタンの終了
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_EndBttn( WFLBY_UNDER_WIN* p_wk, WFLBY_GRAPHICCONT* p_sys )
{
	WFLBY_ROOM_UNDERWIN_Button_End( &p_wk->bttn, p_sys );

	p_wk->now = WFLBY_UNDERWIN_DRAW_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ボタン動作停止設定
 *
 *	@param	p_wk		ワーク
 *	@param	stop		停止させるときTRUE
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_SetBttnStop( WFLBY_UNDER_WIN* p_wk, BOOL stop )
{
	p_wk->bttn.bttnstop = stop;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WIFIロビー下画面用パレットVBlank転送システム
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		システム
 *	@param	dataidx		データインデックス
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_PalTrans( WFLBY_UNDER_WIN* p_wk, WFLBY_GRAPHICCONT* p_sys, u32 dataidx, u32 heapID )
{
	p_wk->dataidx	= dataidx;
	p_wk->heapID	= heapID;
	// タスク生成
	VWaitTCB_Add( WFLBY_ROOM_UNDERWIN_PalTransVTcb, p_wk, 0 );
}

//----------------------------------------------------------------------------
/**
 *	@brief	パレット転送TCB
 *
 *	@param	p_tcb		TCB
 *	@param	p_work		ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_PalTransVTcb( TCB_PTR p_tcb, void* p_work )
{
	WFLBY_UNDER_WIN* p_wk = p_work;
	
	// バックグラウンド
	{
		// カラーパレット
		ArcUtil_HDL_PalSet( p_wk->p_handle, 
				p_wk->dataidx, 
				PALTYPE_SUB_BG, 0, 
				0, p_wk->heapID );
	}

	//  文字書き込み用
	{
		SystemFontPaletteLoad( PALTYPE_SUB_BG, WFLBY_ROOM_BGSPL_FONTCL*32, p_wk->heapID );
	}

	// 技タイプ書き込み用パレット
	{
		ArcUtil_PalSet( WazaTypeIcon_ArcIDGet(), 
				WazaTypeIcon_PlttIDGet(), PALTYPE_SUB_BG, WFLBY_ROOM_BGSPL_WAZATYPE0*32, 3*32, p_wk->heapID );
	}

	// タスク破棄
	TCB_Delete( p_tcb );
}



//----------------------------------------------------------------------------
/**
 *	@brief	共通グラフィック初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		システムワーク
 *	@param	p_handle	ハンドル
 *	@param	sex			自分の性別
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_Common_Init( WFLBY_UNDER_WIN* p_wk, WFLBY_GRAPHICCONT* p_sys, ARCHANDLE* p_handle, u32 sex, u32 heapID )
{
	// バックグラウンド
	{
		// カラーパレット
		if( sex == PM_MALE ){
			ArcUtil_HDL_PalSet( p_handle, 
					NARC_wifi_lobby_other_gadget_sub_NCLR, 
					PALTYPE_SUB_BG, 0, 
					0, heapID );
		}else{
			ArcUtil_HDL_PalSet( p_handle, 
					NARC_wifi_lobby_other_gadget_sub_2_NCLR, 
					PALTYPE_SUB_BG, 0, 
					0, heapID );
		}

		// キャラクタ
		ArcUtil_HDL_BgCharSet( p_handle, 
				NARC_wifi_lobby_other_gadget_bg_NCGR, p_sys->p_bgl, 
				sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_SUB_BACK], 0, 0, FALSE, heapID );
	}

	//  文字書き込み用
	{
		SystemFontPaletteLoad( PALTYPE_SUB_BG, WFLBY_ROOM_BGSPL_FONTCL*32, heapID );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	共通グラフィック破棄
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_Common_Exit( WFLBY_UNDER_WIN* p_wk, WFLBY_GRAPHICCONT* p_sys )
{
}

//----------------------------------------------------------------------------
/**
 *	@brief	スクリーン書き込み共通処理
 *
 *	@param	p_sys		描画システム
 *	@param	p_handle	ハンドル
 *	@param	scrnid		スクリーンＩＤ
 *	@param	bg_frame	BGフレーム
 *	@param	cgofs		ＣＧofs
 *	@param	heapID		ヒープＩＤ
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_Common_LoadScrn( WFLBY_GRAPHICCONT* p_sys, ARCHANDLE* p_handle, u32 scrnid, u32 bg_frame, u32 cgofs, u32 heapID )
{
	void* p_buff;
	NNSG2dScreenData* p_scrn;
	p_buff = ArcUtil_HDL_ScrnDataGet( p_handle, scrnid,  FALSE,
			 &p_scrn, heapID );

	// スクリーンキャラクタオフセット値を書き換え
	{
		int i;
		u16* p_scrndata;
		int size;

		size = p_scrn->szByte / 2;

		// スクリーンデータ代入
		p_scrndata = (u16*)p_scrn->rawData;

		for(i=0; i<size; i++){
			p_scrndata[ i ] += cgofs;
		}
	}

	// ＢＧＬに書き込む
	GF_BGL_ScrWrite( p_sys->p_bgl, bg_frame, p_scrn->rawData, 
			0, 0, p_scrn->screenWidth/8, p_scrn->screenHeight/8 );

	// 転送フラグを立てる
	GF_BGL_LoadScreenV_Req( p_sys->p_bgl, bg_frame );

	sys_FreeMemoryEz( p_buff );
}

//----------------------------------------------------------------------------
/**
 *	@brief	trainerカード	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		グラフィックワーク
 *	@param	p_handle	ハンドル
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_TrCard_Init( WFLBY_TR_CARD* p_wk, WFLBY_GRAPHICCONT* p_sys, ARCHANDLE* p_handle, u32 heapID )
{
	// ウィンドウ初期化
	{
		int i;

		for( i=0; i<WFLBY_TRCARD_WIN_NUM; i++ ){
			GF_BGL_BmpWinAddEx( p_sys->p_bgl, &p_wk->win[i], &sc_WFLBY_TRCARD_WIN_DATA[i] );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーなーカード破棄処理
 *
 *	@param	p_wk		ワーク
 *	@param	p_boardwin	看板ウィンドウシステム
 *	@param	p_sys		描画システム
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_TrCard_Exit( WFLBY_TR_CARD* p_wk, WFLBY_ROOM_TALKMSG* p_boardwin, WFLBY_GRAPHICCONT* p_sys )
{
	// Endが呼ばれていない状態なら破棄
	if( p_wk->p_itemicon != NULL ){
		WFLBY_ROOM_UNDERWIN_TrCard_End( p_wk, p_boardwin, p_sys );
	}

	// ウィンドウ破棄
	{
		int i;

		for( i=0; i<WFLBY_TRCARD_WIN_NUM; i++ ){
			GF_BGL_BmpWinDel( &p_wk->win[i] );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	trainerカードメイン処理
 *
 *	@param	p_wk		ワーク
 *	@param	p_boardwin	看板ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_TrCard_Main( WFLBY_TR_CARD* p_wk, WFLBY_ROOM_TALKMSG* p_boardwin )
{
}

//----------------------------------------------------------------------------
/**
 *	@brief	trainerカード	開始
 *
 *	@param	p_wk		ワーク
 *	@param	p_boardwin	看板ウィンドウシステム
 *	@param	p_sys		グラフィックワーク
 *	@param	p_msg		メッセージ
 *	@param	mydata		自分のデータか
 *	@param	p_handle	ハンドル
 *	@param	heapID		ヒープID
 *	@param	cp_profile	プロフィール
 *	@param	cp_myprofile自分のプロフィール
 *	@param	vip			VIPフラグ
 *	@param	aikotoba	合言葉描画フラグ
 *	@param	cp_aikotoba	合言葉文字列
 *	@param	item_draw	タッチトイを表示するか
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_TrCard_Start( WFLBY_UNDER_WIN* p_ugwk, WFLBY_ROOM_TALKMSG* p_boardwin, WFLBY_TR_CARD* p_wk, WFLBY_GRAPHICCONT* p_sys, WFLBY_ROOM_DEFMSG* p_msg, BOOL mydata, ARCHANDLE* p_handle, u32 heapID, const WFLBY_USER_PROFILE* cp_profile, const WFLBY_USER_PROFILE* cp_myprofile, BOOL vip, BOOL aikotoba, const WFLBY_AIKOTOBA_DATA* cp_aikotoba, BOOL item_draw )
{
	GF_PRINTCOLOR col;
	MYSTATUS* p_mystatus;

	// ワードセットの中身クリア
	WFLBY_ROOM_Msg_ClearWordSet( p_msg );
	
	// 性別からカラーを取得
	if( WFLBY_SYSTEM_GetProfileSex( cp_profile ) == PM_MALE ){
		col = WFLBY_TRCARD_COL_BLUE;

		//  VBlank期間カラーパレット転送
		if( vip == FALSE ){
			WFLBY_ROOM_UNDERWIN_PalTrans( p_ugwk, p_sys, NARC_wifi_lobby_other_gadget_sub_NCLR, heapID );
		}else{
			// VIPならVIPカラー
			WFLBY_ROOM_UNDERWIN_PalTrans( p_ugwk, p_sys, NARC_wifi_lobby_other_gadget_sub_3_NCLR, heapID );
		}
	}else{
		col = WFLBY_TRCARD_COL_RED;

		//  VBlank期間カラーパレット転送
		if( vip == FALSE ){
			WFLBY_ROOM_UNDERWIN_PalTrans( p_ugwk, p_sys, NARC_wifi_lobby_other_gadget_sub_2_NCLR, heapID );
		}else{
			// VIPならVIPカラー
			WFLBY_ROOM_UNDERWIN_PalTrans( p_ugwk, p_sys, NARC_wifi_lobby_other_gadget_sub_3_NCLR, heapID );
		}
	}

	// 描画スクリーンクリア
	GF_BGL_ScrClearCodeVReq( p_sys->p_bgl, sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_SUB_BACK], 0 );
	GF_BGL_ScrClearCodeVReq( p_sys->p_bgl, sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_SUB_BTTN_TR], 0 );
	GF_BGL_ScrClearCodeVReq( p_sys->p_bgl, sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_SUB_BTTN2_MSG], 0 );
	WFLBY_ROOM_UNDERWIN_TrCard_WinClear( p_wk );
	
	// スクリーン転送
	WFLBY_ROOM_UNDERWIN_Common_LoadScrn( p_sys, p_handle, 
			NARC_wifi_lobby_other_profile2_NSCR, 
			sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_SUB_BACK], 0, heapID );

	// プロフィールからMyStatusを作成する
	p_mystatus = MyStatus_AllocWork( heapID );
	WFLBY_SYSTEM_GetProfileMyStatus( cp_profile, p_mystatus, heapID );
	
	// ウィンドウへの書き込み
	// 名前
	{
		u32 namesize;
		
		// ID No
		WFLBY_ROOM_UNDERWIN_TrCard_WinPrint( p_wk, p_msg, WFLBY_TRCARD_WIN_NAME, 
				msg_hiroba_profile_01, 0, 0, col );
		// ID
		WFLBY_ROOM_Msg_SetNumber( p_msg, 
				MyStatus_GetID_Low( p_mystatus ), 5, 0, NUMBER_DISPTYPE_ZERO );
		WFLBY_ROOM_UNDERWIN_TrCard_WinPrintRightSide( p_wk, p_msg, WFLBY_TRCARD_WIN_NAME, 
				msg_hiroba_profile_05_01, 122, 0, WFLBY_TRCARD_COL_BLACK );

		// 名前
		WFLBY_ROOM_UNDERWIN_TrCard_WinPrint( p_wk, p_msg, WFLBY_TRCARD_WIN_NAME, 
				msg_hiroba_profile_02, 0, 16, col );
		// なまえ
		WFLBY_ROOM_UNDERWIN_TrCard_WinSetName( p_wk, p_msg, p_mystatus );
		WFLBY_ROOM_UNDERWIN_TrCard_WinPrintRightSide( p_wk, p_msg, WFLBY_TRCARD_WIN_NAME, 
				msg_hiroba_profile_05_02, 122, 16, WFLBY_TRCARD_COL_BLACK );

		// ウィンドウの転送
		WFLBY_ROOM_UNDERWIN_TrCard_WinOn( p_wk, WFLBY_TRCARD_WIN_NAME );
	}

	// 地域名
	{
		// すんでるばしょ
		WFLBY_ROOM_UNDERWIN_TrCard_WinPrint( p_wk, p_msg, WFLBY_TRCARD_WIN_COUNTRY, 
				msg_hiroba_profile_03, 0, 0, col );

		// 地域があるか
		if( WFLBY_SYSTEM_GetProfileNation( cp_profile ) == WFLBY_WLDDATA_NATION_NONE ) {
			// ダミーメッセージ
			WFLBY_ROOM_UNDERWIN_TrCard_WinPrint( p_wk, p_msg, WFLBY_TRCARD_WIN_COUNTRY, 
					msg_hiroba_profile_06, 32, 16, WFLBY_TRCARD_COL_BLACK );
		}else{

			// 国名
			WFLBY_ROOM_UNDERWIN_TrCard_WinSetCountry( p_wk, p_msg, cp_profile );
			WFLBY_ROOM_UNDERWIN_TrCard_WinPrint( p_wk, p_msg, WFLBY_TRCARD_WIN_COUNTRY, 
					msg_hiroba_profile_05_03, 32, 16, WFLBY_TRCARD_COL_BLACK );
		 }


		// ウィンドウの転送
		WFLBY_ROOM_UNDERWIN_TrCard_WinOn( p_wk, WFLBY_TRCARD_WIN_COUNTRY );
		
	}

	// 合言葉があるー＞合言葉表示
	//		　 ないー＞冒険を始めたときと、最後にしたこと表示
	if( aikotoba ){
		// 合言葉
		p_wk->aikotoba_draw = TRUE;

		// タイトル
		WFLBY_ROOM_UNDERWIN_TrCard_WinSetName( p_wk, p_msg, p_mystatus );
		WFLBY_ROOM_UNDERWIN_TrCard_WinPrint( p_wk, p_msg, WFLBY_TRCARD_WIN_VIPAIKOTOBA, 
				msg_hiroba_profile_07, 8, 0, col );

		// 合言葉設定
		WFLBY_ROOM_UNDERWIN_TrCard_WinSetAikotoba( p_wk, p_msg, cp_aikotoba );
		WFLBY_ROOM_UNDERWIN_TrCard_WinPrint( p_wk, p_msg, WFLBY_TRCARD_WIN_VIPAIKOTOBA, 
				msg_hiroba_profile_10, 6, 24, WFLBY_TRCARD_COL_BLACK );
		WFLBY_ROOM_UNDERWIN_TrCard_WinPrint( p_wk, p_msg, WFLBY_TRCARD_WIN_VIPAIKOTOBA, 
				msg_hiroba_profile_11, 90, 24, WFLBY_TRCARD_COL_BLACK );
		WFLBY_ROOM_UNDERWIN_TrCard_WinPrint( p_wk, p_msg, WFLBY_TRCARD_WIN_VIPAIKOTOBA, 
				msg_hiroba_profile_12, 6, 44, WFLBY_TRCARD_COL_BLACK );
		WFLBY_ROOM_UNDERWIN_TrCard_WinPrint( p_wk, p_msg, WFLBY_TRCARD_WIN_VIPAIKOTOBA, 
				msg_hiroba_profile_13, 90, 44, WFLBY_TRCARD_COL_BLACK );

		// ウィンドウの転送
		WFLBY_ROOM_UNDERWIN_TrCard_WinOn( p_wk, WFLBY_TRCARD_WIN_VIPAIKOTOBA );
		
	}else{
		p_wk->aikotoba_draw = FALSE;

		// ぼうけんをはじめたとき
		WFLBY_ROOM_UNDERWIN_TrCard_WinPrint( p_wk, p_msg, WFLBY_TRCARD_WIN_VIPAIKOTOBA, 
				msg_hiroba_profile_05, 8, 0, col );

		// ウィンドウの転送
		WFLBY_ROOM_UNDERWIN_TrCard_WinOn( p_wk, WFLBY_TRCARD_WIN_VIPAIKOTOBA );

		// 行動履歴を１２件出す
		WFLBY_ROOM_UNDERWIN_TrCard_InitRireki( p_wk, p_sys, p_handle, heapID, cp_profile, cp_myprofile );
		
#if 0
		//  冒険をはじめたとき
		{
			RTCDate date;
			RTCTime time;
			s64	start_sec;
			BOOL sec_check;

			// ぼうけんをはじめたとき
			WFLBY_ROOM_UNDERWIN_TrCard_WinPrint( p_wk, p_msg, WFLBY_TRCARD_WIN_TRSTART, 
					msg_hiroba_profile_04, 0, 0, col );

			//  日付取得
			sec_check	= WFLBY_SYSTEM_CheckProfileGameStartTime( cp_profile );

			if( sec_check == TRUE ){
				start_sec	= WFLBY_SYSTEM_GetProfileGameStartTime( cp_profile );
				RTC_ConvertSecondToDateTime( &date, &time, 
						start_sec );
				
				// 年
				// yearの中身は2007なら07 2008年なら08が入っているので2000を足す
				WFLBY_ROOM_Msg_SetNumber( p_msg, 2000+date.year, 4, 0, NUMBER_DISPTYPE_ZERO );
				WFLBY_ROOM_UNDERWIN_TrCard_WinPrint( p_wk, p_msg, WFLBY_TRCARD_WIN_TRSTART, 
						msg_hiroba_profile_05_04, 58, 16, WFLBY_TRCARD_COL_BLACK );
				// 月
				WFLBY_ROOM_Msg_SetNumber( p_msg, date.month, 2, 0, NUMBER_DISPTYPE_ZERO );
				WFLBY_ROOM_UNDERWIN_TrCard_WinPrint( p_wk, p_msg, WFLBY_TRCARD_WIN_TRSTART, 
						msg_hiroba_profile_05_05, 102, 16, WFLBY_TRCARD_COL_BLACK );
				// 日
				WFLBY_ROOM_Msg_SetNumber( p_msg, date.day, 2, 0, NUMBER_DISPTYPE_ZERO );
				WFLBY_ROOM_UNDERWIN_TrCard_WinPrint( p_wk, p_msg, WFLBY_TRCARD_WIN_TRSTART, 
						msg_hiroba_profile_05_05, 128, 16, WFLBY_TRCARD_COL_BLACK );
			}else{

				// 全部？？？？

				// 年
				WFLBY_ROOM_UNDERWIN_TrCard_WinPrint( p_wk, p_msg, WFLBY_TRCARD_WIN_TRSTART, 
						msg_hiroba_profile_08, 58, 16, WFLBY_TRCARD_COL_BLACK );
				// 月
				WFLBY_ROOM_UNDERWIN_TrCard_WinPrint( p_wk, p_msg, WFLBY_TRCARD_WIN_TRSTART, 
						msg_hiroba_profile_09, 102, 16, WFLBY_TRCARD_COL_BLACK );
				// 日
				WFLBY_ROOM_UNDERWIN_TrCard_WinPrint( p_wk, p_msg, WFLBY_TRCARD_WIN_TRSTART, 
						msg_hiroba_profile_09, 128, 16, WFLBY_TRCARD_COL_BLACK );

			}

			// ウィンドウの転送
			WFLBY_ROOM_UNDERWIN_TrCard_WinOn( p_wk, WFLBY_TRCARD_WIN_TRSTART );

		}


		//  最後にしたこと
		{
			// ぼうけんをはじめたとき
			WFLBY_ROOM_UNDERWIN_TrCard_WinPrint( p_wk, p_msg, WFLBY_TRCARD_WIN_LASTACTION, 
					msg_hiroba_profile_05, 0, 0, col );

			// したこと
			WFLBY_ROOM_UNDERWIN_TrCard_WinSetLastAction( p_wk, p_msg, cp_profile );
			WFLBY_ROOM_UNDERWIN_TrCard_WinPrint( p_wk, p_msg, WFLBY_TRCARD_WIN_LASTACTION, 
					msg_hiroba_profile_05_06, 32, 16, WFLBY_TRCARD_COL_BLACK );

			// ウィンドウの転送
			WFLBY_ROOM_UNDERWIN_TrCard_WinOn( p_wk, WFLBY_TRCARD_WIN_LASTACTION );
		}
#endif
	}

	// アイテムアイコン設定
	WFLBY_ROOM_UNDERWIN_TrCard_ItemIcon_Init( p_wk, p_sys, p_handle, heapID, 
			WFLBY_SYSTEM_GetProfileItem( cp_profile ), item_draw );



	// トレーナグラフィック表示
#ifndef WFLBY_DEBUG_ROOM_TRTYPE_CHG
	{
		u32 tr_type;
		if( mydata == TRUE ){
			// 自分のデータなら、
			// 主人公のキャラクタ
			if( WFLBY_SYSTEM_GetProfileSex( cp_profile ) == PM_FEMALE ){
				tr_type = HEROINE;
			}else{
				tr_type = HERO;
			}
		}else{

			// 他人のデータなら、
			// そのままｍ
			tr_type = WFLBY_SYSTEM_GetProfileTrType( cp_profile );
		}
		WFLBY_ROOM_UNDERWIN_TrCard_TrView_Init( p_wk, p_sys, p_handle, heapID,
				tr_type );
	}
#else
	{
		static u32 trtype = 0;

		WFLBY_ROOM_UNDERWIN_TrCard_TrView_Init( p_wk, p_sys, p_handle, heapID,
				sc_WFLBY_TRCARD_TRTYPE[trtype].fld_trno );
		trtype = (trtype+1) % WFLBY_TRCARD_TRTYPE_NUM;
	}
#endif

	// 技タイプ表示
	WFLBY_ROOM_UNDERWIN_TrCard_WriteWazaType( p_ugwk, p_wk, p_sys, heapID, cp_profile );

	// MyStatus破棄
	sys_FreeMemoryEz( p_mystatus );
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイプを描画する
 *
 *	@param	p_ugwk		した画面ワーク
 *	@param	p_wk		プロフィールワーク
 *	@param	p_sys		描画システム
 *	@param	heapID		ヒープID	（グラフィックを読み込む　テンポラリのRAMの確保のみ）
 *	@param	cp_profile	プロフィールワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_TrCard_WriteWazaType( WFLBY_UNDER_WIN* p_ugwk, WFLBY_TR_CARD* p_wk, WFLBY_GRAPHICCONT* p_sys, u32 heapID, const WFLBY_USER_PROFILE* cp_profile )
{
	u16 type;
	int i;
	u16 bt_type;
	void* p_char;
	NNSG2dCharacterData* p_chardata;


	for( i=0; i<WFLBY_SELECT_TYPENUM; i++ ){
		type = WFLBY_SYSTEM_GetProfileWazaType( cp_profile, i );
		
		if( type != WFLBY_POKEWAZA_TYPE_NONE ){

			// 広場内技タイプからバトル用技タイプに変更
			bt_type = WFLBY_BattleWazaType_Get( type );

			// キャラクタ展開
			p_char = ArcUtil_Load( WazaTypeIcon_ArcIDGet(),
					WazaTypeIcon_CgrIDGet( bt_type ),
					TRUE, heapID, ALLOC_BOTTOM );
			NNS_G2dGetUnpackedCharacterData( p_char, &p_chardata );
			GF_BGL_LoadCharacter(p_sys->p_bgl, 
					sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_SUB_BTTN2_MSG],
					p_chardata->pRawData, 
					WFLBY_TRCARD_WIN_WAZATYPE1_SIZEX*WFLBY_TRCARD_WIN_WAZATYPE1_SIZEY*32,
					sc_WFLBY_TRCARD_WAZATYPE_CGX[i] );
			sys_FreeMemoryEz( p_char );

			// スクリーン書き込み
			GF_BGL_ScrWriteExpand( p_sys->p_bgl, sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_SUB_BTTN2_MSG],
					WFLBY_TRCARD_WIN_WAZATYPE1_X,
					WFLBY_TRCARD_WIN_WAZATYPE1_Y+(WFLBY_TRCARD_WIN_WAZATYPE1_SIZEY*i), 
					WFLBY_TRCARD_WIN_WAZATYPE1_SIZEX, 
					WFLBY_TRCARD_WIN_WAZATYPE1_SIZEY,
					sc_WFLBY_TRCARD_WAZATYPE_SCRN[i],					
					0, 0, 
					WFLBY_TRCARD_WIN_WAZATYPE1_SIZEX, WFLBY_TRCARD_WIN_WAZATYPE1_SIZEY );

			// パレット設定
			GF_BGL_ScrPalChange( p_sys->p_bgl, 
					sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_SUB_BTTN2_MSG],
					WFLBY_TRCARD_WIN_WAZATYPE1_X,
					WFLBY_TRCARD_WIN_WAZATYPE1_Y+(WFLBY_TRCARD_WIN_WAZATYPE1_SIZEY*i), 
					WFLBY_TRCARD_WIN_WAZATYPE1_SIZEX, WFLBY_TRCARD_WIN_WAZATYPE1_SIZEY,
					WFLBY_ROOM_BGSPL_WAZATYPE0 + WazaTypeIcon_PlttOffsetGet( bt_type ) );

			GF_BGL_LoadScreenV_Req( p_sys->p_bgl, 
					sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_SUB_BTTN2_MSG] );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナカード描画終了
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_TrCard_End( WFLBY_TR_CARD* p_wk, WFLBY_ROOM_TALKMSG* p_boardwin, WFLBY_GRAPHICCONT* p_sys )
{
	// 合言葉ウィンドウ破棄
	if( p_wk->aikotoba_draw == TRUE ){
		p_wk->aikotoba_draw = FALSE;
	}else{

		// 履歴ワーク破棄
		WFLBY_ROOM_UNDERWIN_TrCard_ExitRireki( p_wk, p_sys );
	}
	
	
	// アイコン破棄
	WFLBY_ROOM_UNDERWIN_TrCard_ItemIcon_Exit( p_wk, p_sys );

	// トレーナグラフィック破棄
	WFLBY_ROOM_UNDERWIN_TrCard_TrView_Exit( p_wk, p_sys );
	
	// 描画スクリーンクリア
	GF_BGL_ScrClear( p_sys->p_bgl, sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_SUB_BACK] );
	GF_BGL_ScrClear( p_sys->p_bgl, sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_SUB_BTTN_TR] );
	GF_BGL_ScrClear( p_sys->p_bgl, sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_SUB_BTTN2_MSG] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナーカードアイテムを変更
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		グラフィックワーク
 *	@param	p_handle	アーカイブハンドル
 *	@param	heapID		ヒープID
 *	@param	itemno		アイテムID
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_TrCard_ChangeItem( WFLBY_TR_CARD* p_wk, WFLBY_GRAPHICCONT* p_sys, ARCHANDLE* p_handle, u32 heapID, WFLBY_ITEMTYPE itemno )
{
	
	// アイコン破棄
	WFLBY_ROOM_UNDERWIN_TrCard_ItemIcon_Exit( p_wk, p_sys );

	// アイテムアイコン設定
	WFLBY_ROOM_UNDERWIN_TrCard_ItemIcon_Init( p_wk, p_sys, p_handle, heapID, 
			itemno, TRUE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナーカード	合言葉表示中かチェック
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	合言葉表示中
 *	@retval	FALSE	合言葉非表示中
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_ROOM_UNDERWIN_TrCard_CheckAikotobaDraw( const WFLBY_TR_CARD* cp_wk )
{
	return cp_wk->aikotoba_draw;
}

//----------------------------------------------------------------------------
/**
 *	@brief	アイテムアイコン	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		描画システム
 *	@param	p_handle	ハンドル
 *	@param	heapID		ヒープＩＤ
 *	@param	itemno		アイテムＮＯ
 *	@param	draw_flag	表示するか
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_TrCard_ItemIcon_Init( WFLBY_TR_CARD* p_wk, WFLBY_GRAPHICCONT* p_sys, ARCHANDLE* p_handle, u32 heapID, WFLBY_ITEMTYPE itemno, BOOL draw_flag )
{
	u32 itemtype;
	u32 itemanm;
	GF_ASSERT( itemno < WFLBY_ITEM_NUM );
	GF_ASSERT( p_wk->p_itemicon == NULL );

	// アイテムのタイプ
	itemtype	= itemno / WFLBY_ITEM_DANKAI;
	itemanm		= itemno % WFLBY_ITEM_DANKAI;
	
	// itemnoのアイコンリソースを読み込む
	{
		BOOL result;
		// キャラクタ
		p_wk->p_itemiconres[0] = CLACT_U_ResManagerResAddArcChar_ArcHandle( 
				p_sys->p_resman[0], p_handle,
				NARC_wifi_lobby_other_gadget_icon1_NCGR + (itemtype * 3),
				FALSE, WFLBY_TRCARD_ICON_RESID, NNS_G2D_VRAM_TYPE_2DSUB, heapID );
		// VRAM転送
		result =CLACT_U_CharManagerSetAreaCont( p_wk->p_itemiconres[0] );
		GF_ASSERT( result );
		CLACT_U_ResManagerResOnlyDelete( p_wk->p_itemiconres[0] );
		// パレット
		p_wk->p_itemiconres[1] = CLACT_U_ResManagerResAddArcPltt_ArcHandle( 
				p_sys->p_resman[1], p_handle,
				NARC_wifi_lobby_other_gadget_oam_NCLR, 
				FALSE, WFLBY_TRCARD_ICON_RESID, 
				NNS_G2D_VRAM_TYPE_2DSUB, WFLBY_TRCARD_ICON_PLTTNUM, heapID );
		result =CLACT_U_PlttManagerSetCleanArea( p_wk->p_itemiconres[1] );
		GF_ASSERT( result );
		CLACT_U_ResManagerResOnlyDelete( p_wk->p_itemiconres[1] );
		// セル	
		p_wk->p_itemiconres[2] = CLACT_U_ResManagerResAddArcKindCell_ArcHandle( 
				p_sys->p_resman[2], p_handle,
				NARC_wifi_lobby_other_gadget_icon1_NCER + (itemtype * 3),
				FALSE, WFLBY_TRCARD_ICON_RESID, CLACT_U_CELL_RES, heapID );
		// セルアニメ	
		p_wk->p_itemiconres[3] = CLACT_U_ResManagerResAddArcKindCell_ArcHandle( 
				p_sys->p_resman[3], p_handle,
				NARC_wifi_lobby_other_gadget_icon1_NANR + (itemtype * 3),
				FALSE, WFLBY_TRCARD_ICON_RESID, CLACT_U_CELLANM_RES, heapID );

	}

	// 追加
	{
		CLACT_HEADER header;
		CLACT_ADD_SIMPLE add = {NULL};
		// ヘッダー作成
		CLACT_U_MakeHeader( &header, WFLBY_TRCARD_ICON_RESID, WFLBY_TRCARD_ICON_RESID,
				WFLBY_TRCARD_ICON_RESID, WFLBY_TRCARD_ICON_RESID,
				CLACT_U_HEADER_DATA_NONE, CLACT_U_HEADER_DATA_NONE,
				0, WFLBY_TRCARD_ICON_BGPRI,
				p_sys->p_resman[0],
				p_sys->p_resman[1],
				p_sys->p_resman[2],
				p_sys->p_resman[3],
				NULL, NULL );

		add.ClActSet	= p_sys->p_clactset;
		add.ClActHeader = &header;
		add.pri			= WFLBY_TRCARD_ICON_SFPRI;
		add.DrawArea	= NNS_G2D_VRAM_TYPE_2DSUB;
		add.heap		= heapID;
		add.mat.x		= WFLBY_TRCARD_ICON_X;
		add.mat.y		= WFLBY_TRCARD_ICON_Y;
		p_wk->p_itemicon = CLACT_AddSimple( &add );

		// アニメ設定
		CLACT_AnmChg( p_wk->p_itemicon, itemanm );

		CLACT_SetDrawFlag( p_wk->p_itemicon, draw_flag );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	アイテムアイコン	破棄
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		描画システム
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_TrCard_ItemIcon_Exit( WFLBY_TR_CARD* p_wk, WFLBY_GRAPHICCONT* p_sys )
{
	// Icon破棄
	CLACT_Delete( p_wk->p_itemicon );
	p_wk->p_itemicon = NULL;

	// リソース破棄
	{
		int i;
		
		// VRAM
		CLACT_U_CharManagerDelete( p_wk->p_itemiconres[0] );
		CLACT_U_PlttManagerDelete( p_wk->p_itemiconres[1] );

		// リソース
		for( i=0; i<WFLBY_ROOM_OAM_RESNUM; i++ ){
			CLACT_U_ResManagerResDelete( p_sys->p_resman[i], p_wk->p_itemiconres[i] );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナ見た目表示
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		描画システム
 *	@param	p_handle	ハンドル
 *	@param	heapID		ヒープ
 *	@param	trtype		トレーナタイプ
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_TrCard_TrView_Init( WFLBY_TR_CARD* p_wk, WFLBY_GRAPHICCONT* p_sys, ARCHANDLE* p_handle, u32 heapID, u32 trtype )
{
	u32 btl_trtype;
	u16 x, y;
	u32 i;
	TR_CLACT_GRA tr_gradata;
	
	GF_ASSERT( p_wk->p_tr == NULL );

	// トレーナタイプグラフィック取得
	for( i=0; i<WFLBY_TRCARD_TRTYPE_NUM; i++ ){
		if( sc_WFLBY_TRCARD_TRTYPE[i].fld_trno == trtype ){
			btl_trtype	= sc_WFLBY_TRCARD_TRTYPE[i].btl_trtype;
			x			= sc_WFLBY_TRCARD_TRTYPE[i].x;
			y			= sc_WFLBY_TRCARD_TRTYPE[i].y;
		}
	}
	TrCLACTGraDataGet( btl_trtype, PARA_FRONT, &tr_gradata );
	
	// トレーナリソースを読み込む
	{
		BOOL result;
		// キャラクタ
		p_wk->p_trres[0] = CLACT_U_ResManagerResAddArcChar( 
				p_sys->p_resman[0], tr_gradata.arcID,
				tr_gradata.ncgrID,
				FALSE, WFLBY_TRCARD_TRVIEW_RESID, NNS_G2D_VRAM_TYPE_2DSUB, heapID );
		result = CLACT_U_CharManagerSetAreaCont( p_wk->p_trres[0] );
		GF_ASSERT( result );
		// パレット
		p_wk->p_trres[1] = CLACT_U_ResManagerResAddArcPltt( 
				p_sys->p_resman[1], tr_gradata.arcID,
				tr_gradata.nclrID, 
				FALSE, WFLBY_TRCARD_TRVIEW_RESID, 
				NNS_G2D_VRAM_TYPE_2DSUB, 1, heapID );
		WFLBY_ROOM_UNDERWIN_TrCard_TrView_PallColorChange( CLACT_U_ResManagerGetResObjResPltt( p_wk->p_trres[1] ) );
		result =CLACT_U_PlttManagerSetCleanArea( p_wk->p_trres[1] );
		GF_ASSERT( result );
		CLACT_U_ResManagerResOnlyDelete( p_wk->p_trres[1] );
		// セル	
		p_wk->p_trres[2] = CLACT_U_ResManagerResAddArcKindCell( 
				p_sys->p_resman[2], tr_gradata.arcID,
				tr_gradata.ncerID,
				FALSE, WFLBY_TRCARD_TRVIEW_RESID, CLACT_U_CELL_RES, heapID );
		// セルアニメ	
		p_wk->p_trres[3] = CLACT_U_ResManagerResAddArcKindCell( 
				p_sys->p_resman[3], tr_gradata.arcID,
				tr_gradata.nanrID,
				FALSE, WFLBY_TRCARD_TRVIEW_RESID, CLACT_U_CELLANM_RES, heapID );

	}

	// 追加
	{
		CLACT_HEADER header;
		CLACT_ADD_SIMPLE add = {NULL};
		// ヘッダー作成
		CLACT_U_MakeHeader( &header, WFLBY_TRCARD_TRVIEW_RESID, WFLBY_TRCARD_TRVIEW_RESID,
				WFLBY_TRCARD_TRVIEW_RESID, WFLBY_TRCARD_TRVIEW_RESID,
				CLACT_U_HEADER_DATA_NONE, CLACT_U_HEADER_DATA_NONE,
				TRUE, WFLBY_TRCARD_TRVIEW_BGPRI,
				p_sys->p_resman[0],
				p_sys->p_resman[1],
				p_sys->p_resman[2],
				p_sys->p_resman[3],
				NULL, NULL );

		add.ClActSet	= p_sys->p_clactset;
		add.ClActHeader = &header;
		add.pri			= WFLBY_TRCARD_TRVIEW_SFPRI;
		add.DrawArea	= NNS_G2D_VRAM_TYPE_2DSUB;
		add.heap		= heapID;
		add.mat.x		= FX32_CONST( x );
		add.mat.y		= FX32_CONST( y ) + WFLBY_ROOM_CLACT_SUBSURFACE_Y;
		p_wk->p_tr = CLACT_AddSimple( &add );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナ見た目破棄
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		描画システム
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_TrCard_TrView_Exit( WFLBY_TR_CARD* p_wk, WFLBY_GRAPHICCONT* p_sys )
{
	DelVramTransData( CLACT_ImageProxyGet( p_wk->p_tr ) );
	CLACT_Delete( p_wk->p_tr );
	p_wk->p_tr = NULL;

	// リソース破棄
	{
		int i;
		
		// VRAM
		CLACT_U_CharManagerDelete( p_wk->p_trres[0] );
		CLACT_U_PlttManagerDelete( p_wk->p_trres[1] );

		// リソース
		for( i=0; i<WFLBY_ROOM_OAM_RESNUM; i++ ){
			CLACT_U_ResManagerResDelete( p_sys->p_resman[i], p_wk->p_trres[i] );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	パレットの色を変更
 *
 *	@param	p_pltt	パレット
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_TrCard_TrView_PallColorChange( NNSG2dPaletteData* p_pltt )
{
	int i, r, g, b, y;
	int pal_size;
	u16* p_pal;

	// セピアカラーにする
	
	pal_size	= p_pltt->szByte / 2;	// 1パレット単位のサイズにする
	p_pal		= p_pltt->pRawData;

	for(i = 0; i < pal_size; i++){
		r = ((*p_pal) & 0x1f);
		g = (((*p_pal) >> 5) & 0x1f);
		b = (((*p_pal) >> 10) & 0x1f);

		y = ((r)*76 + (g)*151 + (b)*29) >> 8;

		r = ((y)*256)>>8;
		g = ((y)*200)>>8;
		b = ((y)*128)>>8;

		if(r > 31){ r = 31; }

		*p_pal = (u16)((b<<10)|(g<<5)|r);
		p_pal++;
	}


}


//----------------------------------------------------------------------------
/**
 *	@brief	trainerカードウィンドウクリア
 *	
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_TrCard_WinClear( WFLBY_TR_CARD* p_wk )
{
	int i;
	for( i=0; i<WFLBY_TRCARD_WIN_NUM; i++ ){
		GF_BGL_BmpWinDataFill( &p_wk->win[i], 0 );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ウィンドウの表示
 *
 *	@param	p_wk		ワーク
 *	@param	p_msg		メッセージ
 *	@param	winno		ウィンドウナンバー
 *	@param	strid		メッセージid
 *	@param	x			ｘ座標
 *	@param	y			ｙ座標
 *	@param	col			カラー
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_TrCard_WinPrint( WFLBY_TR_CARD* p_wk, WFLBY_ROOM_DEFMSG* p_msg, u32 winno, u32 strid, u8 x, u8 y, GF_PRINTCOLOR col )
{
	STRBUF* p_str;
	
	// 表示
	p_str = WFLBY_ROOM_Msg_Get( p_msg, WFLBY_DEFMSG_TYPE_HIROBA, strid );
	GF_STR_PrintColor( &p_wk->win[ winno ], FONT_SYSTEM, p_str, 
			x, y, MSG_NO_PUT, col, NULL );
}
// 右端表示バージョン
static void WFLBY_ROOM_UNDERWIN_TrCard_WinPrintRightSide( WFLBY_TR_CARD* p_wk, WFLBY_ROOM_DEFMSG* p_msg, u32 winno, u32 strid, u8 x, u8 y, GF_PRINTCOLOR col )
{
	STRBUF* p_str;
	u32 strsize;
	s32 draw_x;
	
	// 表示
	p_str = WFLBY_ROOM_Msg_Get( p_msg, WFLBY_DEFMSG_TYPE_HIROBA, strid );

	strsize = FontProc_GetPrintStrWidth( FONT_SYSTEM, p_str, 0 );

	draw_x = x - strsize;
	if( draw_x < 0 ){
		draw_x = 0;
	}

	GF_STR_PrintColor( &p_wk->win[ winno ], FONT_SYSTEM, p_str, 
			draw_x, y, MSG_NO_PUT, col, NULL );
}

//----------------------------------------------------------------------------
/**
 *	@brief	トレーナカードon
 *	
 *	@param	p_wk	ワーク
 *	@param	idx		インデックス
 *
 * idx
	WFLBY_TRCARD_WIN_NAME,
	WFLBY_TRCARD_WIN_COUNTRY,
	WFLBY_TRCARD_WIN_TRSTART,
	WFLBY_TRCARD_WIN_LASTACTION,
	WFLBY_TRCARD_WIN_VIPAIKOTOBA,
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_TrCard_WinOn( WFLBY_TR_CARD* p_wk, u32 idx )
{
	GF_BGL_BmpWinOnVReq( &p_wk->win[idx] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	名前の設定
 *
 *	@param	p_wk		ワーク
 *	@param	cp_mystatus	Myステータス
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_TrCard_WinSetName( WFLBY_TR_CARD* p_wk, WFLBY_ROOM_DEFMSG* p_msg,const MYSTATUS* cp_mystatus )
{
	WFLBY_ROOM_Msg_SetPlayerName( p_msg, cp_mystatus, 0 );
}

//----------------------------------------------------------------------------
/**
 *	@brief	国と地名を設定
 *
 *	@param	p_wk		ワーク
 *	@param	cp_profile	プロフィール
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_TrCard_WinSetCountry( WFLBY_TR_CARD* p_wk, WFLBY_ROOM_DEFMSG* p_msg,const WFLBY_USER_PROFILE* cp_profile )
{
	u32 nation;
	u32 area;

	nation = WFLBY_SYSTEM_GetProfileNation( cp_profile );
	area = WFLBY_SYSTEM_GetProfileArea( cp_profile );
	
	WFLBY_ROOM_Msg_SetCountry( p_msg, nation, 0 );
	WFLBY_ROOM_Msg_SetArea( p_msg, nation, area, 1 );
}

//----------------------------------------------------------------------------
/**
 *	@brief	最後にしたことを設定
 *
 *	@param	p_wk		ワーク
 *	@param	cp_profile	プロフィール
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_TrCard_WinSetLastAction( WFLBY_TR_CARD* p_wk, WFLBY_ROOM_DEFMSG* p_msg,const WFLBY_USER_PROFILE* cp_profile )
{
	u32 action;
//	action = WFLBY_SYSTEM_GetProfileLastAction( cp_profile, 0 );
	action = 0;
	WFLBY_ROOM_Msg_SetMinigame( p_msg, action, 0  );
	
}

//----------------------------------------------------------------------------
/**
 *	@brief	合言葉設定
 *
 *	@param	p_wk		ワーク
 *	@param	p_msg		メッセージワーク
 *	@param	cp_aikotoba	合言葉データ
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_TrCard_WinSetAikotoba( WFLBY_TR_CARD* p_wk, WFLBY_ROOM_DEFMSG* p_msg, const WFLBY_AIKOTOBA_DATA* cp_aikotoba )
{
	int i;
	for( i=0; i<WFLBY_AIKOTOBA_WORD_NUM; i++ ){
		WFLBY_ROOM_Msg_SetPMSWord( p_msg, i, cp_aikotoba->word[i] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	履歴を表示する
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		グラフィックシステム	
 *	@param	p_handle	アーカイブハンドル
 *	@param	heapID		ヒープID
 *	@param	cp_profile	プロフィール
 *	@param	cp_myprofile自分のプロフィール
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_TrCard_InitRireki( WFLBY_TR_CARD* p_wk, WFLBY_GRAPHICCONT* p_sys, ARCHANDLE* p_handle, u32 heapID, const WFLBY_USER_PROFILE* cp_profile, const WFLBY_USER_PROFILE* cp_myprofile )
{
	int i;
	ARCHANDLE* p_union_handle;
	ARCHANDLE* p_work_handle;
	BOOL result;
	u32 last_action;
	u32 last_action_userid;
	u32 my_userid;
	u32 my_sex;
	CLACT_ADD_SIMPLE add;
	CLACT_HEADER	 header;
	const WFLBY_TRCARD_RIREKI_CHAR* cp_chardata;
	static const u16 sc_PAL[ WFLBY_TRCARD_RIREKI_RES_NUM ][3] = {
		{ ARC_2DUNIONOBJ_FRONT, NARC_unionobj2d_onlyfront_wf_match_top_trainer_NCLR, 8 },
		{ ARC_WIFILOBBY_OTHER_GRA, NARC_wifi_lobby_other_profile_icon_NCLR, 2 },
		{ ARC_2DUNIONOBJ_FRONT, NARC_unionobj2d_onlyfront_pl_boy01_NCLR, 1 },
		{ ARC_2DUNIONOBJ_FRONT, NARC_unionobj2d_onlyfront_pl_girl01_NCLR, 1 },
	};
	static const u16 sc_CELL[ WFLBY_TRCARD_RIREKI_RES_NUM ] = {
		NARC_unionobj2d_onlyfront_only_front00_NCER,
		NARC_wifi_lobby_other_profile_icon1_NCER,
		NARC_unionobj2d_onlyfront_only_front_boy_NCER,
		NARC_unionobj2d_onlyfront_only_front_girl_NCER,
	};
	static const u16 sc_ANM[ WFLBY_TRCARD_RIREKI_RES_NUM ] = {
		NARC_unionobj2d_onlyfront_only_front00_NANR,
		NARC_wifi_lobby_other_profile_icon1_NANR,
		NARC_unionobj2d_onlyfront_only_front_boy_NANR,
		NARC_unionobj2d_onlyfront_only_front_girl_NANR,
	};


	// ユニオンオブジェ　表向きのアーカイブハンドルオープン
	p_union_handle = ArchiveDataHandleOpen( ARC_2DUNIONOBJ_FRONT, heapID );


	// 自分のユーザIDX取得
	my_userid	= WFLBY_SYSTEM_GetProfileUserID( cp_myprofile );
	my_sex		= WFLBY_SYSTEM_GetProfileSex( cp_myprofile );
 

	// パレットとセルデータは全部読み込む
	for( i=0; i<WFLBY_TRCARD_RIREKI_RES_NUM; i++ ){

		// WFLBY_TRCARD_RIREKI_RES_HEROBOYとWFLBY_TRCARD_RIREKI_RES_HEROGIRLは
		// 自分の性別で読み込むか決める
		if( i == WFLBY_TRCARD_RIREKI_RES_HEROBOY ){
			if( my_sex == PM_FEMALE ){
				p_wk->p_rirekires_pal[i] = NULL;
				p_wk->p_rirekires_cell[i] = NULL;
				p_wk->p_rirekires_anm[i] = NULL;
				continue;
			}
		}
		else if( i == WFLBY_TRCARD_RIREKI_RES_HEROGIRL ){
			if( my_sex == PM_MALE ){
				p_wk->p_rirekires_pal[i] = NULL;
				p_wk->p_rirekires_cell[i] = NULL;
				p_wk->p_rirekires_anm[i] = NULL;
				continue;
			}
		}

		if( sc_PAL[i][0] == ARC_2DUNIONOBJ_FRONT ){
			p_work_handle = p_union_handle;
		}else{
			p_work_handle = p_handle;
		}

		p_wk->p_rirekires_pal[i] = CLACT_U_ResManagerResAddArcPltt_ArcHandle( 
				p_sys->p_resman[1], p_work_handle,
				sc_PAL[i][1], 
				FALSE, WFLBY_TRCARD_RIREKI_RESID+i, 
				NNS_G2D_VRAM_TYPE_2DSUB, sc_PAL[i][2], heapID );
		result =CLACT_U_PlttManagerSetCleanArea( p_wk->p_rirekires_pal[i] );
		GF_ASSERT( result );
		CLACT_U_ResManagerResOnlyDelete( p_wk->p_rirekires_pal[i] );


		// セル	
		p_wk->p_rirekires_cell[i] = CLACT_U_ResManagerResAddArcKindCell_ArcHandle( 
				p_sys->p_resman[2], p_work_handle,
				sc_CELL[i],
				FALSE, WFLBY_TRCARD_RIREKI_RESID+i, CLACT_U_CELL_RES, heapID );
		// セルアニメ	
		p_wk->p_rirekires_anm[i] = CLACT_U_ResManagerResAddArcKindCell_ArcHandle( 
				p_sys->p_resman[3], p_work_handle,
				sc_ANM[i],
				FALSE, WFLBY_TRCARD_RIREKI_RESID+i, CLACT_U_CELLANM_RES, heapID );
	}

	// キャラクタ読み込み
	for( i=0; i<WFLBY_LASTACT_BUFFNUM; i++ ){
		
		last_action = WFLBY_SYSTEM_GetProfileLastAction( cp_profile, i );
		last_action_userid = WFLBY_SYSTEM_GetProfileLastActionUserID( cp_profile, i );

		if( last_action == WFLBY_LASTACTION_MAX ){

			p_wk->p_rirekires_cg[i] = NULL;
			p_wk->p_rireki[i] = NULL;
		}else{

			// 履歴キャラクタデータを取得
			cp_chardata = WFLBY_ROOM_UNDERWIN_TrCard_GetRirekiData( last_action, last_action_userid, my_userid, my_sex );

			if( cp_chardata->arcid == WFLBY_TRCARD_RIREKI_RES_ICON ){
				p_work_handle = p_handle;
			}else{
				p_work_handle = p_union_handle;
			}

			// キャラクタ
			p_wk->p_rirekires_cg[i] = CLACT_U_ResManagerResAddArcChar_ArcHandle( 
					p_sys->p_resman[0], p_work_handle,
					cp_chardata->dataid,
					FALSE, WFLBY_TRCARD_RIREKI_RESID+i, NNS_G2D_VRAM_TYPE_2DSUB, heapID );
			// VRAM転送
			result =CLACT_U_CharManagerSetAreaCont( p_wk->p_rirekires_cg[i] );
			GF_ASSERT( result );
			CLACT_U_ResManagerResOnlyDelete( p_wk->p_rirekires_cg[i] );

			// アクター作成
			CLACT_U_MakeHeader( &header, WFLBY_TRCARD_RIREKI_RESID+i, 
					WFLBY_TRCARD_RIREKI_RESID + cp_chardata->arcid,
					WFLBY_TRCARD_RIREKI_RESID + cp_chardata->arcid,
					WFLBY_TRCARD_RIREKI_RESID + cp_chardata->arcid,
					CLACT_U_HEADER_DATA_NONE, CLACT_U_HEADER_DATA_NONE,
					0, WFLBY_TRCARD_RIREKI_BGPRI,
					p_sys->p_resman[0],
					p_sys->p_resman[1],
					p_sys->p_resman[2],
					p_sys->p_resman[3],
					NULL, NULL );

			add.ClActSet	= p_sys->p_clactset;
			add.ClActHeader = &header;
			add.pri			= 0;
			add.DrawArea	= NNS_G2D_VRAM_TYPE_2DSUB;
			add.heap		= heapID;
			add.mat			= sc_WFLBY_TRCARD_RIREKI_POS[i];
			add.mat.x		+= FX32_CONST(cp_chardata->ofs_x);
			add.mat.y		+= FX32_CONST(cp_chardata->ofs_y);

			p_wk->p_rireki[i] = CLACT_AddSimple( &add );

			// パレットオフセット設定
			CLACT_PaletteOffsetChgAddTransPlttNo( p_wk->p_rireki[i], cp_chardata->paloffs );
		}
	}

	ArchiveDataHandleClose( p_union_handle );
	
}

//----------------------------------------------------------------------------
/**
 *	@brief	履歴表示OFF
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		描画システム
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_TrCard_ExitRireki( WFLBY_TR_CARD* p_wk, WFLBY_GRAPHICCONT* p_sys )
{
	int i;
	
	// アクター破棄＋キャラクタVRAM	
	for( i=0; i<WFLBY_LASTACT_BUFFNUM; i++ ){
		if( p_wk->p_rirekires_cg[i] != NULL ){
			CLACT_Delete( p_wk->p_rireki[i] );
			p_wk->p_rireki[i] = NULL;
			
			CLACT_U_CharManagerDelete( p_wk->p_rirekires_cg[i] );
			CLACT_U_ResManagerResDelete( p_sys->p_resman[0], p_wk->p_rirekires_cg[i] );
			p_wk->p_rirekires_cg[i] = NULL;
		}
	}
	// パレット セル　アニメ
	for( i=0; i<WFLBY_TRCARD_RIREKI_RES_NUM; i++ ){
		if( p_wk->p_rirekires_pal[i] != NULL ){
			CLACT_U_PlttManagerDelete( p_wk->p_rirekires_pal[i] );
			CLACT_U_ResManagerResDelete( p_sys->p_resman[1], p_wk->p_rirekires_pal[i] );
			CLACT_U_ResManagerResDelete( p_sys->p_resman[2], p_wk->p_rirekires_cell[i] );
			CLACT_U_ResManagerResDelete( p_sys->p_resman[3], p_wk->p_rirekires_anm[i] );
			p_wk->p_rirekires_pal[i] = NULL;
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	履歴で読み込むキャラクタデータを取得する
 *
 *	@param	last_act	アクション内容
 *	@param	userid		そのユーザID
 *	@param	myid		自分のID
 */
//-----------------------------------------------------------------------------
static const WFLBY_TRCARD_RIREKI_CHAR* WFLBY_ROOM_UNDERWIN_TrCard_GetRirekiData( WFLBY_LASTACTION_TYPE last_act, s32 userid, s32 myid, u32 sex )
{
	// 自分と同じ人なら自分の絵を出す
	if( userid == myid ){
		return &sc_WFLBY_TRCARD_RIREKI_HERO_CHAR[ sex ];
	}
	return &sc_WFLBY_TRCARD_RIREKI_CHAR[ last_act ];
}



//----------------------------------------------------------------------------
/**
 *	@brief	した画面	ガジェットボタン	初期化
 *
 *	@param	p_wk			ワーク
 *	@param	p_sys			描画システム
 *	@param	p_handle		ハンドル
 *	@param	heapID			ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_Button_Init( WFLBY_GADGET_BTTN* p_wk, WFLBY_GRAPHICCONT* p_sys, ARCHANDLE* p_handle, u32 heapID )
{
	int i;

	// ビットマップウィンドウ初期化
	GF_BGL_BmpWinAddEx( p_sys->p_bgl, &p_wk->win, &sc_WFLBY_BTTN_WIN_DATA );
	
	// 各スクリーンデータを読み込む
	for( i=0; i<WFLBY_ROOM_UNDERWIN_BTTN_ANM_NUM; i++ ){
		p_wk->p_scrnbuff[i] = ArcUtil_HDL_ScrnDataGet( p_handle, 
				NARC_wifi_lobby_other_gadget_but0_n_NSCR+i, FALSE,
				&p_wk->p_scrn[i], heapID );
	}

	// ボタンマネージャ作成
	{
		static const TP_HIT_TBL sc_HitTbl = { 32, 160, 40, 216 };

		p_wk->p_bttnman = BMN_Create( &sc_HitTbl, 1, 
				WFLBY_ROOM_UNDERWIN_Button_BttnCallBack, p_wk, heapID );
	}

	p_wk->bttnstop = TRUE;	// 最初は反応なし
}

//----------------------------------------------------------------------------
/**
 *	@brief	ボタン破棄処理
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		描画システム
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_Button_Exit( WFLBY_GADGET_BTTN* p_wk )
{
	int i;

	// ボタンマネージャ破棄
	BMN_Delete( p_wk->p_bttnman );	

	//  ビットマップ破棄
	GF_BGL_BmpWinDel( &p_wk->win );
	
	// 全リソース破棄
	for( i=0; i<WFLBY_ROOM_UNDERWIN_BTTN_ANM_NUM; i++ ){
		sys_FreeMemoryEz( p_wk->p_scrnbuff[i] );
	}
	p_wk->seq = WFLBY_ROOM_UNDERWIN_BTTN_STATUS_ON;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ガジェットボタン	メイン動作
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		システム
 *	@param	p_handle	ハンドル
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	ガジェット起動
 *	@retval	FALSE	ガジェット起動なし
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_ROOM_UNDERWIN_Button_Main( WFLBY_GADGET_BTTN* p_wk, WFLBY_GRAPHICCONT* p_sys,  ARCHANDLE* p_handle, u32 heapID )
{
	BOOL ret = FALSE;

	// ガジェット反応ありのときだけボタンマネージャ
	if( p_wk->bttnstop == FALSE ){
		BMN_Main( p_wk->p_bttnman );
	}else{
		p_wk->bttnevent = BMN_EVENT_RELEASE;
	}
	

	// シーケンス管理
	if( p_wk->seq == WFLBY_ROOM_UNDERWIN_BTTN_STATUS_WAIT ){
		if( p_wk->lock_wait == FALSE ){
			p_wk->seq = WFLBY_ROOM_UNDERWIN_BTTN_STATUS_ON;
			WFLBY_ROOM_UNDERWIN_Button_DrawButton( p_wk, p_sys, WFLBY_ROOM_UNDERWIN_BTTN_ANM_UP, p_handle, heapID );
		}
	}

	// イベントとが変わった！
	if( p_wk->bttnevent != p_wk->lastbttnevent ){
		p_wk->lastbttnevent = p_wk->bttnevent;

		// 絵を変更
		if( p_wk->bttnevent == BMN_EVENT_HOLD ){
			WFLBY_ROOM_UNDERWIN_Button_DrawButton( p_wk, p_sys, WFLBY_ROOM_UNDERWIN_BTTN_ANM_DOWN, p_handle, heapID );
		}else{
			// 押していいときとだめなときで初期グラフィックを変える
			if( p_wk->seq == WFLBY_ROOM_UNDERWIN_BTTN_STATUS_ON ){
				WFLBY_ROOM_UNDERWIN_Button_DrawButton( p_wk, p_sys, WFLBY_ROOM_UNDERWIN_BTTN_ANM_UP, p_handle, heapID );
			}else{
				WFLBY_ROOM_UNDERWIN_Button_DrawButton( p_wk, p_sys, WFLBY_ROOM_UNDERWIN_BTTN_ANM_CENTER, p_handle, heapID );
			}
		}

		// ボタン起動チェック
		if( (p_wk->seq == WFLBY_ROOM_UNDERWIN_BTTN_STATUS_ON) &&
			(p_wk->bttnevent == BMN_EVENT_HOLD) ){
			ret = TRUE;
			p_wk->seq		= WFLBY_ROOM_UNDERWIN_BTTN_STATUS_WAIT;
			p_wk->lock_wait	= TRUE;
		}
	}

	return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ガジェットボタン	開始
 *
 *	@param	p_wk			ワーク
 *	@param	p_sys			描画システム
 *	@param	p_msg			メッセージ
 *	@param	cp_profile		プロフィール
 *	@param	p_handle		ハンドル
 *	@param	heapID			ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_Button_Start( WFLBY_GADGET_BTTN* p_wk, WFLBY_GRAPHICCONT* p_sys, WFLBY_ROOM_DEFMSG* p_msg, const WFLBY_USER_PROFILE* cp_profile, ARCHANDLE* p_handle, u32 heapID )
{
	// バックグラウンド設定
	{
		ArcUtil_HDL_ScrnSet( p_handle, NARC_wifi_lobby_other_gadget_bg_NSCR,
				p_sys->p_bgl, sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_SUB_BACK],
				0, 0, FALSE, heapID );
	}

	// 動作モード設定
	p_wk->mode = WFLBY_ROOM_UNDERWIN_BTTN_MODE_GADGET;

	// 上に出すがジェットの絵を出す
	{

		p_wk->bttn_itemno = WFLBY_SYSTEM_GetProfileItem( cp_profile );
		ArcUtil_HDL_BgCharSet( p_handle, NARC_wifi_lobby_other_gadget_1a_NCGR+p_wk->bttn_itemno,
				p_sys->p_bgl, sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_SUB_BTTN2_MSG],
				WFLBY_UNDERWIN_BGCG_END, 0, FALSE, heapID );

		WFLBY_ROOM_UNDERWIN_Common_LoadScrn( p_sys, p_handle, 
				NARC_wifi_lobby_other_gadget_1a_NSCR+p_wk->bttn_itemno, 
				sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_SUB_BTTN2_MSG],
				WFLBY_UNDERWIN_BGCG_END, heapID );
	}

	// タイトル表示
	{	
		STRBUF* p_str;
		
		// 表示
		p_str = WFLBY_ROOM_Msg_Get( p_msg, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_profile_14 );
		GF_BGL_BmpWinDataFill( &p_wk->win, 0 );
		GF_STR_PrintColor( &p_wk->win, FONT_SYSTEM, p_str, 
				0, 4, MSG_ALLPUT, GF_PRINTCOLOR_MAKE( 15, 14, 0 ), NULL );
	}


	// 押していいときとだめなときで初期グラフィックを変える
	if( p_wk->seq == WFLBY_ROOM_UNDERWIN_BTTN_STATUS_ON ){
		WFLBY_ROOM_UNDERWIN_Button_DrawButton( p_wk, p_sys, WFLBY_ROOM_UNDERWIN_BTTN_ANM_UP, p_handle, heapID );
	}else{
		WFLBY_ROOM_UNDERWIN_Button_DrawButton( p_wk, p_sys, WFLBY_ROOM_UNDERWIN_BTTN_ANM_CENTER, p_handle, heapID );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ボタン開始	フロート用
 *
 *	@param	p_wk			ワーク
 *	@param	p_sys			描画システム
 *	@param	idx				フロート乗り込み位置インデックス（０～２）
 *	@param	p_handle		アーカイブハンドル
 *	@param	heapID			ヒープＩＤ
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_Button_StartFloat( WFLBY_GADGET_BTTN* p_wk, WFLBY_GRAPHICCONT* p_sys, u32 idx, ARCHANDLE* p_handle, u32 heapID )
{
	GF_ASSERT( idx < WFLBY_FLOAT_ON_NUM );
	
	// バックグラウンド設定
	{
		ArcUtil_HDL_ScrnSet( p_handle, NARC_wifi_lobby_other_gadget_bg_NSCR,
				p_sys->p_bgl, sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_SUB_BACK],
				0, 0, FALSE, heapID );
	}

	// 動作モード設定
	p_wk->mode			= WFLBY_ROOM_UNDERWIN_BTTN_MODE_FLOAT;
	p_wk->bttn_itemno	= idx;

	// 上に出す絵を出す
	ArcUtil_HDL_BgCharSet( p_handle, NARC_wifi_lobby_other_float_a_NCGR+p_wk->bttn_itemno,
			p_sys->p_bgl, sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_SUB_BTTN2_MSG],
			WFLBY_UNDERWIN_BGCG_END, 0, FALSE, heapID );


	// 押していいときとだめなときで初期グラフィックを変える
	if( p_wk->seq == WFLBY_ROOM_UNDERWIN_BTTN_STATUS_ON ){
		WFLBY_ROOM_UNDERWIN_Button_DrawButton( p_wk, p_sys, WFLBY_ROOM_UNDERWIN_BTTN_ANM_UP, p_handle, heapID );
	}else{
		WFLBY_ROOM_UNDERWIN_Button_DrawButton( p_wk, p_sys, WFLBY_ROOM_UNDERWIN_BTTN_ANM_CENTER, p_handle, heapID );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ガジェットボタン	開始
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		描画システム
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_Button_End( WFLBY_GADGET_BTTN* p_wk, WFLBY_GRAPHICCONT* p_sys )
{
	// スクリーンクリア
	GF_BGL_ScrClear( p_sys->p_bgl, sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_SUB_BACK] );
	GF_BGL_ScrClear( p_sys->p_bgl, sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_SUB_BTTN_TR] );
	GF_BGL_ScrClear( p_sys->p_bgl, sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_SUB_BTTN2_MSG] );

	// スクロール座標設定
	GF_BGL_ScrollSet( p_sys->p_bgl, sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_SUB_BTTN2_MSG],
			GF_BGL_SCROLL_Y_SET, 0 );
}



//----------------------------------------------------------------------------
/**
 *	@brief	ボタン描画	
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		描画システム
 *	@param	idx			インデックス
 *	@param	p_handle	ハンドル
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_Button_DrawButton( WFLBY_GADGET_BTTN* p_wk, WFLBY_GRAPHICCONT* p_sys, u32 idx, ARCHANDLE* p_handle, u32 heapID )
{
	static const s8 sc_BUTTON_Y[ WFLBY_ROOM_UNDERWIN_BTTN_ANM_NUM ] = {
		0, 9, 6
	};
/*
	GF_BGL_ScrWrite( p_sys->p_bgl, sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_SUB_BTTN_TR],
			(const void*)p_wk->p_scrn[idx]->rawData, 0, 0, 32, 24 );//*/

	GF_BGL_ScrWriteExpand(
				p_sys->p_bgl, sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_SUB_BTTN_TR],
				0, 3, 32, 21,
				(const void*)p_wk->p_scrn[idx]->rawData, 0, 3, 32, 32 );
	

	GF_BGL_LoadScreenV_Req( p_sys->p_bgl , sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_SUB_BTTN_TR] );


	// フロートモードならフローログラフィックも変更
	if( p_wk->mode == WFLBY_ROOM_UNDERWIN_BTTN_MODE_FLOAT ){
		static const u8 sc_FLOAT_BTTN_IDX[ WFLBY_ROOM_UNDERWIN_BTTN_ANM_NUM ] = {
			0, 1, 0
		};
		u32 scrn_idx;
		
		// フロートボタンのときはスクリーンも転送
		scrn_idx = (p_wk->bttn_itemno*WFLBY_ROOM_UNDERWIN_BTTN_FLOAT_FRAME_NUM)+sc_FLOAT_BTTN_IDX[ idx ];
		WFLBY_ROOM_UNDERWIN_Common_LoadScrn( p_sys, p_handle, 
				NARC_wifi_lobby_other_float_a1_NSCR+scrn_idx, 
				sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_SUB_BTTN2_MSG],
				WFLBY_UNDERWIN_BGCG_END, heapID );
	}

	// idxによって。sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_SUB_BTTN2_MSG]面の
	// Y座標を調整
	GF_BGL_ScrollReq( p_sys->p_bgl, sc_WFLBY_ROOM_BGCNT_FRM[WFLBY_ROOM_BGCNT_SUB_BTTN2_MSG],
			GF_BGL_SCROLL_Y_SET, sc_BUTTON_Y[idx] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ボタンコールバック
 *
 *	@param	bttnno		ボタンナンバー
 *	@param	event		イベント
 *	@param	p_work		ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_Button_BttnCallBack( u32 bttnno, u32 event, void* p_work )
{
	WFLBY_GADGET_BTTN* p_wk = p_work;

	p_wk->bttnevent = event;

	
	// 瞬間のデータはいらない
	if( p_wk->bttnevent == BMN_EVENT_TOUCH ){
		p_wk->bttnevent = BMN_EVENT_HOLD;
	}else{
		// 離した種類もいらない
		if( p_wk->bttnevent == BMN_EVENT_SLIDEOUT ){
			p_wk->bttnevent = BMN_EVENT_RELEASE;
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ボタン　ロックウエイトをクリア
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_ROOM_UNDERWIN_Button_BttnLockClear( WFLBY_GADGET_BTTN* p_wk )
{
	p_wk->lock_wait = FALSE;
}

