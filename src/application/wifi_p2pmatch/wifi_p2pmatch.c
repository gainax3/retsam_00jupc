//============================================================================================
/**
 * @file	wifi_p2pmatch.c
 * @bfief	WIFIP2Pマッチング画面
 * @author	k.ohno
 * @date	06.04.07
 */
//============================================================================================

#include "common.h"
#include "system/procsys.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "system/pm_str.h"
#include "system/clact_tool.h"
#include "system/msgdata_util.h"
#include "system/wordset.h"
#include "msgdata/msg.naix"
#include "msgdata/msg_wifi_lobby.h"
#include "msgdata/msg_wifi_system.h"
#include "application\namein.h"
#include "system/wipe.h"
#include "system/fontproc.h"
#include "system/lib_pack.h"
#include "system/fontoam.h"
#include "system/window.h"
#include "gflib/msg_print.h"
#include "gflib/touchpanel.h"
#include "poketool/monsno.h"
#include "system/bmp_menu.h"
#include "system/bmp_list.h"
#include "application/wifi_p2pmatch.h"
#include "application/wifi_lobby/wflby_def.h"
#include "system/snd_tool.h"
#include "gflib/strbuf_family.h"
#include "msgdata/msg_opening_name.h"
#include "itemtool/itemsym.h"
#include "system/brightness.h"
#include "system/pmfprint.h"
#include "system/pm_overlay.h"
#include "system/pm_rtc.h"

#include "wifi/dwc_rap.h"
#include "wifi_p2pmatch_local.h"
#include "wifi_p2pmatch_se.h"
#include "wifi/dwc_rapfriend.h"

#include "field/sysflag.h"

#include "communication/communication.h"
#include "communication/comm_state.h"
#include "msgdata/msg_ev_win.h"
#include "savedata/zukanwork.h"
#include "savedata/wifihistory.h"
#include "savedata/config.h"
#include "savedata/poruto.h"
#include "savedata/frontier_savedata.h"
#include "itemtool/myitem.h"
#include "poketool/poke_tool.h"
#include "poketool/pokeparty.h"
#include "system/snd_tool.h"  //sndTOOL
#include "savedata/email_savedata.h"

#include "wifip2pmatch.naix"			// グラフィックアーカイブ定義

#include "include/system/pm_debug_wifi.h"

#include "application/connect_anm.h"

#include "comm_command_wfp2pmf_func.h"

// WIFI2DMAPシステムオーバーレイ
FS_EXTERN_OVERLAY(wifi_2dmapsys);

// WIFI　対戦AUTOﾓｰﾄﾞデバック
#ifdef _WIFI_DEBUG_TUUSHIN
extern WIFI_DEBUG_BATTLE_WK WIFI_DEBUG_BATTLE_Work;
#endif	//_WIFI_DEBUG_TUUSHIN


//-------------------------------------
///	マッチング用拡張ヒープサイズ
//=====================================
#define WIFI_P2PMATCH_EXHEAP_SIZE	( 0x10000 )
//#define WIFI_P2PMATCH_EXHEAP_SIZE	( 0x5000 )




///	ＢＧパレット定義 2005/09/15
//
//	0 ～ 5  : フィールドマップＢＧ用
//  6       : 天候
//  7       : 地名ウインドウ
//  8       : 看板ウインドウ（マップ、標識など）
//  9       : 看板ウインドウ（枠、フォント）
//  10      : メッセージウインドウ
//  11      : メニューウインドウ
//  12      : メッセージフォント
//  13      : システムフォント
//	14		: 未使用（ローカライズ用）
//	15		: デバッグ用（製品版では未使用）
#define FLD_WEATHER_PAL      (  6 )			//  天候
#define FLD_PLACENAME_PAL    (  7 )         //  地名ウインドウ
#define FLD_BOARD1FRAME_PAL  (  8 )         //  看板ウインドウ（マップ、標識など）
#define FLD_BOARD2FRAME_PAL  (  9 )         //  看板ウインドウ（枠、フォント）
#define FLD_MESFRAME_PAL     ( 10 )         //  メッセージウインドウ
#define FLD_MENUFRAME_PAL    ( 11 )         //  メニューウインドウ
#define FLD_MESFONT_PAL      ( 12 )         //  メッセージフォント
#define FLD_SYSFONT_PAL	     ( 13 )         //  システムフォント
#define FLD_LOCALIZE_PAL     ( 14 )         //	未使用（ローカライズ用）
#define FLD_DEBUG_PAL        ( 15 )         //	デバッグ用（製品版では未使用）

/*********************************************************************************************
	メイン画面のCGX割り振り		2006/01/12

		ウィンドウ枠	：	409 - 511
			会話、メニュー、地名、看板

		BMPウィンドウ１	：	297 - 408
			会話（最大）、看板、残りボール数

		BMPウィンドウ２	：	55 - 296
			メニュー（最大）、はい/いいえ、地名

*********************************************************************************************/

/*********************************************************************************************
	ウィンドウ枠
*********************************************************************************************/
// 会話ウィンドウキャラ
#define	TALK_WIN_CGX_SIZE	( 18+12 )
#define	TALK_WIN_CGX_NUM	( 512 - TALK_WIN_CGX_SIZE )
#define	TALK_WIN_PAL		( 10 )

// メニューウィンドウキャラ
#define	MENU_WIN_CGX_SIZE	( 9 )
#define	MENU_WIN_CGX_NUM	( TALK_WIN_CGX_NUM - MENU_WIN_CGX_SIZE )
#define	MENU_WIN_PAL		( 11 )

// 地名ウィンドウキャラ
#define	PLACE_WIN_CGX_SIZE	( 10 )
#define	PLACE_WIN_CGX_NUM	( MENU_WIN_CGX_NUM - PLACE_WIN_CGX_SIZE )
#define	PLACE_WIN_PAL		( 7 )

// 看板ウィンドウキャラ
#define	BOARD_WIN_CGX_SIZE	( 18+12 + 24 )
#define	BOARD_WIN_CGX_NUM	( PLACE_WIN_CGX_NUM - BOARD_WIN_CGX_SIZE )
#define	BOARD_WIN_PAL		( FLD_BOARD2FRAME_PAL )

/*********************************************************************************************
	BMPウィンドウ
*********************************************************************************************/
// 会話ウィンドウ（メイン）
#define	FLD_MSG_WIN_PX		( 2 )
#define	FLD_MSG_WIN_PY		( 19 )
#define	FLD_MSG_WIN_SX		( 27 )
#define	FLD_MSG_WIN_SY		( 4 )
#define	FLD_MSG_WIN_PAL		( FLD_MESFONT_PAL )
#define	FLD_MSG_WIN_CGX		( BOARD_WIN_CGX_NUM - ( FLD_MSG_WIN_SX * FLD_MSG_WIN_SY ) )

// 看板ウィンドウ（メイン）（会話と同じ位置（会話より小さい））
#define	FLD_BOARD_WIN_PX	( 9 )
#define	FLD_BOARD_WIN_PY	( 19 )
#define	FLD_BOARD_WIN_SX	( 20 )
#define	FLD_BOARD_WIN_SY	( 4 )
#define	FLD_BOARD_WIN_PAL	( FLD_BOARD2FRAME_PAL )
#define	FLD_BOARD_WIN_CGX	( FLD_MSG_WIN_CGX )

// 残りボール数（会話と同じ位置（会話より小さい））
#define	FLD_BALL_WIN_PX		( 1 )
#define	FLD_BALL_WIN_PY		( 1 )
#define	FLD_BALL_WIN_SX		( 12 )
#define	FLD_BALL_WIN_SY		( 4 )
#define	FLD_BALL_WIN_PAL	( FLD_SYSFONT_PAL )
#define	FLD_BALL_WIN_CGX	( FLD_MSG_WIN_CGX )

// メニューウィンドウ（メイン）
#define	FLD_MENU_WIN_PX		( 20 )
#define	FLD_MENU_WIN_PY		( 1 )
#define	FLD_MENU_WIN_SX		( 11 )
#define	FLD_MENU_WIN_SY		( 22 )
#define	FLD_MENU_WIN_PAL	( FLD_SYSFONT_PAL )
#define	FLD_MENU_WIN_CGX	( FLD_MSG_WIN_CGX - ( FLD_MENU_WIN_SX * FLD_MENU_WIN_SY ) )

// はい/いいえウィンドウ（メイン）（メニューと同じ位置（メニューより小さい））
#define	FLD_YESNO_WIN_PX	( 25 )
#define	FLD_YESNO_WIN_PY	( 13 )
#define	FLD_YESNO_WIN_SX	( 6 )
#define	FLD_YESNO_WIN_SY	( 4 )
#define	FLD_YESNO_WIN_PAL	( FLD_SYSFONT_PAL )
#define	FLD_YESNO_WIN_CGX	( FLD_MSG_WIN_CGX - ( FLD_YESNO_WIN_SX * FLD_YESNO_WIN_SY ) )

// 地名ウィンドウ（メニューと同じ位置（メニューより小さい））
#define FLD_PLACE_WIN_PX	( 0 )
#define FLD_PLACE_WIN_PY	( 0 )
#define	FLD_PLACE_WIN_SX	( 32 )
#define	FLD_PLACE_WIN_SY	( 3 )
#define	FLD_PLACE_WIN_CGX	( FLD_MSG_WIN_CGX - ( FLD_PLACE_WIN_SX * FLD_PLACE_WIN_SY ) )



// メッセージウィンドウ（サブ）
#define	FLD_MSG_WIN_S_PX	( 2  )
#define	FLD_MSG_WIN_S_PY	( 19 )
#define	FLD_MSG_WIN_S_PAL	( FLD_MESFONT_PAL )
#define	FLD_MSG_WIN_S_CGX	( MENU_WIN_CGX_NUM - ( FLD_MSG_WIN_SX * FLD_MSG_WIN_SY ) )

// 看板ウィンドウ（サブ）
#define	FLD_BOARD_WIN_S_PX	( 9 )
#define	FLD_BOARD_WIN_S_PY	( 19 )
#define	FLD_BOARD_WIN_S_SX	( 21 )
#define	FLD_BOARD_WIN_S_SY	( 4 )
#define	FLD_BOARD_WIN_S_PAL	( FLD_BOARD2FRAME_PAL )
#define	FLD_BOARD_WIN_S_CGX	( FLD_MSG_WIN_S_CGX )

// メニューウィンドウ（サブ）
#define	FLD_MENU_WIN_S_PX	( 25 )
#define	FLD_MENU_WIN_S_PY	( 1 )
#define	FLD_MENU_WIN_S_SX	( 6 )
#define	FLD_MENU_WIN_S_SY	( 16 )
#define	FLD_MENU_WIN_S_PAL	( FLD_SYSFONT_PAL )
#define	FLD_MENU_WIN_S_CGX	( MENU_WIN_CGX_NUM - ( FLD_MENU_WIN_SX * FLD_MENU_WIN_SY ) )



///	ビットマップ転送関数用定義
//------------------------------------------------------------------
#define	FBMP_TRANS_OFF	(0)
#define	FBMP_TRANS_ON	(1)
///	文字表示転送関数用定義
//------------------------------------------------------------------
#define	FBMPMSG_WAITON_SKIPOFF	(0)
#define	FBMPMSG_WAITON_SKIPON	(1)
#define	FBMPMSG_WAITOFF			(2)

//-------------------------------------------------------------------------
///	文字表示色定義(default)	-> gflib/fntsys.hへ移動
//------------------------------------------------------------------

#define WINCLR_COL(col)	(((col)<<4)|(col))

//-------------------------------------------------------------------------
///	文字表示スピード定義(default)
//------------------------------------------------------------------
#define	FBMP_MSG_SPEED_SLOW		(8)
#define	FBMP_MSG_SPEED_NORMAL	(4)
#define	FBMP_MSG_SPEED_FAST		(1)



//============================================================================================
//	定数定義
//============================================================================================
enum {
    SEQ_IN = 0,
    SEQ_MAIN,
    SEQ_OUT,
};

enum {
    _MENU_LIST,
    _MENU_INPUT,
    _MENU_MYCODE,
    _MENU_EXIT,
};

typedef enum {
    _CLACT_UP_CUR,
    _CLACT_DOWN_CUR,
    _CLACT_LINE_CUR,
    _CLACT_VCT_STOP,
    _CLACT_VCT_MOVE,
} _OAM_ANIM_E;

typedef struct{  // スクリーン用RECT構造体
    u8 lt_x;
    u8 lt_y;
    u8 rb_x;
    u8 rb_y;
} _SCR_RECT;

#define WF_CLACT_WKNUM	( 8 )
#define WF_CLACT_RESNUM	( 1 )
#define WF_FONTOAM_NUM	( 1 )

#define _PRINTTASK_MAX (8)
#define _TIMING_GAME_CHECK  (13)// つながった直後
#define _TIMING_GAME_CHECK2  (14)// つながった直後
#define _TIMING_GAME_START  (15)// タイミングをそろえる
#define _TIMING_GAME_START2  (18)// タイミングをそろえる
#define _TIMING_BATTLE_END  (16)// タイミングをそろえる

#define _RECONECTING_WAIT_TIME (20)  //再接続時間


// ユーザー表示面の設定
#define WIFIP2PMATCH_PLAYER_DISP_X	( 1 )
#define WIFIP2PMATCH_PLAYER_DISP_Y	( 1 )
#define WIFIP2PMATCH_PLAYER_DISP_SIZX	( 28 )
#define WIFIP2PMATCH_PLAYER_DISP_SIZY	( 2 )
#define WIFIP2PMATCH_PLAYER_DISP_NAME_X	( 32 )
#define WIFIP2PMATCH_PLAYER_DISP_ST_X	( 102 )
#define WIFIP2PMATCH_PLAYER_DISP_WINSIZ	(WIFIP2PMATCH_PLAYER_DISP_SIZX*WIFIP2PMATCH_PLAYER_DISP_SIZY)


//FRAME3
#define	_CGX_BOTTOM	( 512 )							// 会話ウィンドウキャラ
#define _CGX_TITLE_BOTTOM	(_CGX_BOTTOM - 18*3)	// タイトルウィンドウ
#define _CGX_USER_BOTTOM	(_CGX_TITLE_BOTTOM - WIFIP2PMATCH_PLAYER_DISP_WINSIZ)	// ユーザーウィンドウ

// FRAME1に転送するユーザーデータ背景
#define _CGX_USET_BACK_BOTTOM	( MENU_WIN_CGX_NUM - WIFIP2PMATCH_PLAYER_DISP_WINSIZ )

// FRAME1に転送するICONデータ
#define PLAYER_DISP_ICON_PLTTOFS	(8)
#define PLAYER_DISP_ICON_PLTTOFS_SUB (13)
#define PLAYER_DISP_ICON_PLTTNUM	(2)
#define PLAYER_DISP_ICON_CG_SIZX	(12)
#define PLAYER_DISP_ICON_CG_SIZY	(4)
#define PLAYER_DISP_ICON_CGX		( 0 )
#define PLAYER_DISP_ICON_SCRN_X	( 2 )
#define PLAYER_DISP_ICON_SCRN_Y	( 2 )
#define PLAYER_DISP_ICON_POS_X	( 2 )
#define PLAYER_DISP_ICON_POS_Y	( 1 )
#define PLAYER_DISP_VCTICON_POS_X	( 26 )
#define PLAYER_DISP_VCTICON_POS_Y	( 1 )
enum{
	PLAYER_DISP_ICON_IDX_NONE00,
	PLAYER_DISP_ICON_IDX_VCTNOT,
	PLAYER_DISP_ICON_IDX_VCTBIG,
	PLAYER_DISP_ICON_IDX_VCT,
	PLAYER_DISP_ICON_IDX_NORMAL,
	PLAYER_DISP_ICON_IDX_UNK,
	PLAYER_DISP_ICON_IDX_FIGHT,
	PLAYER_DISP_ICON_IDX_CHANGE,
	PLAYER_DISP_ICON_IDX_NONE,
	PLAYER_DISP_ICON_IDX_POFIN,
	PLAYER_DISP_ICON_IDX_MINIGAME,
	PLAYER_DISP_ICON_IDX_FRONTIER,
	PLAYER_DISP_ICON_IDX_NUM,
};


// 会話ウィンドウ	FRAME2
#define COMM_MESFRAME_PAL     ( 10 )         //  メッセージウインドウ
#define COMM_MENUFRAME_PAL    ( 11 )         //  メニューウインドウ
#define COMM_MESFONT_PAL      ( 12 )         //  メッセージフォント
#define COMM_SYSFONT_PAL	  ( 13 )         //  システムフォント
#define	COMM_TALK_WIN_CGX_SIZE	( 18+12 )
#define	COMM_TALK_WIN_CGX_NUM	( 512 - COMM_TALK_WIN_CGX_SIZE)
#define	COMM_MSG_WIN_PX		( 2 )
#define	COMM_MSG_WIN_PY		( 19 )
#define	COMM_MSG_WIN_SX		( 27 )
#define	COMM_MSG_WIN_SY		( 4 )
#define	COMM_MSG_WIN_PAL		( COMM_MESFONT_PAL )
#define	COMM_MSG_WIN_CGX		( (COMM_TALK_WIN_CGX_NUM - 73) - ( COMM_MSG_WIN_SX * COMM_MSG_WIN_SY ) )

#define	COMM_SYS_WIN_PX		( 4 )
#define	COMM_SYS_WIN_PY		( 4 )
#define	COMM_SYS_WIN_SX		( 23 )
#define	COMM_SYS_WIN_SY		( 16 )
#define	COMM_SYS_WIN_PAL		( COMM_MESFONT_PAL )
#define	COMM_SYS_WIN_CGX		( MENU_WIN_CGX_NUM - ( COMM_SYS_WIN_SX * COMM_SYS_WIN_SY ) )	// 通信システムウィンドウ転送先


// YesNoWin
#define YESNO_WIN_CGX	((COMM_SYS_WIN_CGX - (FLD_YESNO_WIN_SX*FLD_YESNO_WIN_SY)))
#define FRAME1_YESNO_WIN_CGX	((_CGX_USET_BACK_BOTTOM - (FLD_YESNO_WIN_SX*FLD_YESNO_WIN_SY)))


// メインメッセージキャラ
#define _NUKI_FONT_PALNO  (13)
#define	_COL_N_BLACK	( GF_PRINTCOLOR_MAKE( 1, 2, 0 ) )		// フォントカラー：黒
#define	_COL_N_WHITE	( GF_PRINTCOLOR_MAKE( 15, 14, 0 ) )		// フォントカラー：白
#define	_COL_N_RED	    ( GF_PRINTCOLOR_MAKE( 3, 4, 0 ) )		// フォントカラー：青
#define	_COL_N_BLUE	    ( GF_PRINTCOLOR_MAKE( 5, 6, 0 ) )		// フォントカラー：赤
#define	_COL_N_GRAY  	( GF_PRINTCOLOR_MAKE( 2, 14, 0 ) )		// フォントカラー：灰

// 名前表示BMP（上画面）
#define WIFIP2PMATCH_NAME_BMP_W	 ( 16 )
#define WIFIP2PMATCH_NAME_BMP_H	 (  2 )
#define WIFIP2PMATCH_NAME_BMP_SIZE (WIFIP2PMATCH_NAME_BMP_W * WIFIP2PMATCH_NAME_BMP_H)

// 会話ウインドウ表示位置定義
#define WIFIP2PMATCH_TALK_X		(  2 )
#define WIFIP2PMATCH_TALK_Y		(  19 )

#define WIFIP2PMATCH_TITLE_X		(   3  )
#define WIFIP2PMATCH_TITLE_Y		(   1  )
#define WIFIP2PMATCH_TITLE_W		(  26  )
#define WIFIP2PMATCH_TITLE_H		(   2  )


#define WIFIP2PMATCH_MSG_WIN_OFFSET 		(1 + TALK_WIN_CGX_SIZ     + MENU_WIN_CGX_SIZ)
#define WIFIP2PMATCH_TITLE_WIN_OFFSET		( WIFIP2PMATCH_MSG_WIN_OFFSET   + FLD_MSG_WIN_SX*FLD_MSG_WIN_SY )
#define WIFIP2PMATCH_NAME_WIN_OFFSET		( WIFIP2PMATCH_TITLE_WIN_OFFSET + WIFIP2PMATCH_TITLE_W*WIFIP2PMATCH_TITLE_H )
#define WIFIP2PMATCH_YESNO_WIN_OFFSET		( WIFIP2PMATCH_NAME_WIN_OFFSET  + WIFIP2PMATCH_NAME_BMP_SIZE*5 )


// VIEW面
enum{
	MCV_BTTN_FRIEND_TYPE_NONE,	// 設定していない
	MCV_BTTN_FRIEND_TYPE_RES,	// 予約
	MCV_BTTN_FRIEND_TYPE_IN,		// 登録済み
	MCV_BTTN_FRIEND_TYPE_MAX,	// タイプ最大値
};
#define MCV_PAL_BACK		( 0 )	// 背景パレットの開始位置
#define MCV_PAL_FRMNO		( 0 )	// 背景のバットパレット開始位置
#define MCV_PAL_BTTN		( 4 )	// ボタンパレットの開始位置
enum{
	MCV_PAL_BACK_0 = 0,
	MCV_PAL_BACK_1,
	MCV_PAL_BACK_2,
	MCV_PAL_BACK_3,
	MCV_PAL_BACK_NUM,

	MCV_PAL_BTTN_GIRL = 0,
	MCV_PAL_BTTN_MAN,
	MCV_PAL_BTTN_NONE,
	MCV_PAL_BTTNST_GIRL,
	MCV_PAL_BTTNST_MAN,
	MCV_PAL_BTTN_NUM	// 今のところ余りは
						// MCV_PAL_BTTN+MCV_PAL_BTTN_NUM～(PLAYER_DISP_ICON_PLTTOFS_SUB-1まで
};
// アイコンの転送位置
#define MCV_ICON_CGX	(0)
#define MCV_ICON_CGSIZ	(48)
#define MCV_ICON_PAL		(PLAYER_DISP_ICON_PLTTOFS_SUB)

#define MCV_CGX_BTTN2	(MCV_ICON_CGX+MCV_ICON_CGSIZ)	// FRAME2ユーザーデータ
#define MCV_CGX_BACK	(0)// FRAME0背景
#define MCV_SYSFONT_PAL	( 15 )	// システムフォント
// WIN設定
#define MCV_NAMEWIN_CGX		( 1 )	// 名前ウィンドウ開始位置
#define MCV_NAMEWIN_DEFX	( 4 )	// 基本位置
#define MCV_NAMEWIN_DEFY	( 1 )
#define MCV_NAMEWIN_OFSX	( 16 )	// 位置の移動値
#define MCV_NAMEWIN_OFSY	( 6 )
#define MCV_NAMEWIN_SIZX	( 9 )	// サイズ
#define MCV_NAMEWIN_SIZY	( 3 )
#define MCV_NAMEWIN_CGSIZ	( MCV_NAMEWIN_SIZX*MCV_NAMEWIN_SIZY )	// CGサイズ
#define MCV_NAMEWIN_CGALLSIZ	((MCV_NAMEWIN_CGSIZ*WCR_MAPDATA_1BLOCKOBJNUM)+MCV_NAMEWIN_CGX)// CGそうサイズ

#define MCV_STATUSWIN_CGX	( MCV_NAMEWIN_CGALLSIZ+1 )
#define MCV_STATUSWIN_DEFX	( 1 )	// 基本位置
#define MCV_STATUSWIN_DEFY	( 1 )
#define MCV_STATUSWIN_VCHATX ( 12 )	// VCHAT位置
#define MCV_STATUSWIN_OFSX	( 16 )	// 位置の移動値
#define MCV_STATUSWIN_OFSY	( 6 )
#define MCV_STATUSWIN_SIZX	( 2 )	// サイズ
#define MCV_STATUSWIN_SIZY	( 3 )
#define MCV_STATUSWIN_CGSIZ	( MCV_STATUSWIN_SIZX*MCV_STATUSWIN_SIZY )	// CGサイズ

#define MCV_USERWIN_CGX		( 1 )
#define MCV_USERWIN_X		( 1 )
#define MCV_USERWIN_Y		( 1 )
#define MCV_USERWIN_SIZX	( 30 )
#define MCV_USERWIN_SIZY	( 21 )


#define MCV_BUTTON_SIZX	( 16 )
#define MCV_BUTTON_SIZY	( 6 )
#define MCV_BUTTON_DEFX	(0)
#define MCV_BUTTON_DEFY	(0)
#define MCV_BUTTON_OFSX	(16)
#define MCV_BUTTON_OFSY	(6)

#define MCV_BUTTON_ICON_OFS_X	( 1 )
#define MCV_BUTTON_VCTICON_OFS_X	( 13 )
#define MCV_BUTTON_ICON_OFS_Y	( 1 )

#define MCV_BUTTON_FRAME_NUM	(4)	// ボタンアニメフレーム数

enum{
	MCV_BUTTON_TYPE_GIRL,
	MCV_BUTTON_TYPE_MAN,
	MCV_BUTTON_TYPE_NONE,
};

enum{
	MCV_USERDISP_OFF,	// ユーザー表示OFF
	MCV_USERDISP_INIT,	// ユーザー表示初期化
	MCV_USERDISP_ON,	// ユーザー表示ON
	MCV_USERDISP_ONEX,	// ユーザー表示強制ON
};

// PAGE 1
#define MCV_USERD_NAME_X	( 32 )
#define MCV_USERD_NAME_Y	( 8 )
#define MCV_USERD_ST_X	( 104 )
#define MCV_USERD_ST_Y	( 8 )
#define MCV_USERD_GR_X	( 8 )
#define MCV_USERD_GR_Y	( 32 )
#define MCV_USERD_VS_X	( 8 )
#define MCV_USERD_VS_Y	( 56 )
#define MCV_USERD_VS_WIN_X	( 120 )
#define MCV_USERD_VS_WIN_Y	( 56 )
#define MCV_USERD_VS_LOS_X	( 184 )
#define MCV_USERD_TR_X		( 8 )
#define MCV_USERD_TR_Y		( 80 )
#define MCV_USERD_TRNUM_X	( 152 )
#define MCV_USERD_DAY_X		( 8 )
#define MCV_USERD_DAY_Y		( 128 )
#define MCV_USERD_DAYNUM_X	( 152 )
#define MCV_USERD_ICON_X	( 2 )
#define MCV_USERD_VCTICON_X	( 28 )
#define MCV_USERD_ICON_Y	( 2 )
#define MCV_USERD_POFIN_X		( 8 )
#define MCV_USERD_POFIN_Y		( 104 )
#define MCV_USERD_POFINNUM_X	( 152 )

// PAGE2
#define MCV_USERD_BTTW_TITLE_X		(8)
#define MCV_USERD_BTTW_TITLE_Y		(0)
#define MCV_USERD_BTTW_LAST_X		(8)
#define MCV_USERD_BTTW_LAST_Y		(24)
#define MCV_USERD_BTTW_LASTNUM_X	(64)
#define MCV_USERD_BTTW_LASTNUM_Y	(24)
#define MCV_USERD_BTTW_MAX_X		(8)
#define MCV_USERD_BTTW_MAX_Y		(48)
#define MCV_USERD_BTTW_MAXNUM_X		(64)
#define MCV_USERD_BTTW_MAXNUM_Y		(48)

// PAGE3
#define MCV_USERD_BTFC_TITLE_X			(8)
#define MCV_USERD_BTFC_TITLE_Y			(0)
#define MCV_USERD_BTFC_LV50_X			(8)
#define MCV_USERD_BTFC_LV50_Y			(24)
#define MCV_USERD_BTFC_LV50K_X			(136)
#define MCV_USERD_BTFC_LV50K_Y			(24)
#define MCV_USERD_BTFC_LV50LAST_X		(8)
#define MCV_USERD_BTFC_LV50LAST_Y		(48)
#define MCV_USERD_BTFC_LV50LASTNUM_X	(64)
#define MCV_USERD_BTFC_LV50LASTNUM_Y	(48)
#define MCV_USERD_BTFC_LV50LASTTRNUM_X	(184)
#define MCV_USERD_BTFC_LV50LASTTRNUM_Y	(48)
#define MCV_USERD_BTFC_LV50MAX_X		(8)
#define MCV_USERD_BTFC_LV50MAX_Y		(64)
#define MCV_USERD_BTFC_LV50MAXNUM_X		(64)
#define MCV_USERD_BTFC_LV50MAXNUM_Y		(64)
#define MCV_USERD_BTFC_LV50MAXTRNUM_X	(184)
#define MCV_USERD_BTFC_LV50MAXTRNUM_Y	(64)
#define MCV_USERD_BTFC_OPN_X			(8)
#define MCV_USERD_BTFC_OPN_Y			(88)
#define MCV_USERD_BTFC_OPNK_X			(136)
#define MCV_USERD_BTFC_OPNK_Y			(88)
#define MCV_USERD_BTFC_OPNLAST_X		(8)
#define MCV_USERD_BTFC_OPNLAST_Y		(112)
#define MCV_USERD_BTFC_OPNLASTNUM_X	(64)
#define MCV_USERD_BTFC_OPNLASTNUM_Y	(112)
#define MCV_USERD_BTFC_OPNLASTTRNUM_X	(184)
#define MCV_USERD_BTFC_OPNLASTTRNUM_Y	(112)
#define MCV_USERD_BTFC_OPNMAX_X		(8)
#define MCV_USERD_BTFC_OPNMAX_Y		(128)
#define MCV_USERD_BTFC_OPNMAXNUM_X		(64)
#define MCV_USERD_BTFC_OPNMAXNUM_Y		(128)
#define MCV_USERD_BTFC_OPNMAXTRNUM_X	(184)
#define MCV_USERD_BTFC_OPNMAXTRNUM_Y	(128)


// PAGE4
#define MCV_USERD_BTKS_TITLE_X		(8)
#define MCV_USERD_BTKS_TITLE_Y		(0)
#define MCV_USERD_BTKS_K_X			(136)
#define MCV_USERD_BTKS_K_Y			(24)
#define MCV_USERD_BTKS_LAST_X		(8)
#define MCV_USERD_BTKS_LAST_Y		(48)
#define MCV_USERD_BTKS_LASTNUM_X	(64)
#define MCV_USERD_BTKS_LASTNUM_Y	(48)
#define MCV_USERD_BTKS_LASTCPNUM_X	(184)
#define MCV_USERD_BTKS_LASTCPNUM_Y	(48)
#define MCV_USERD_BTKS_MAX_X		(8)
#define MCV_USERD_BTKS_MAX_Y		(80)
#define MCV_USERD_BTKS_MAXNUM_X		(64)
#define MCV_USERD_BTKS_MAXNUM_Y		(80)
#define MCV_USERD_BTKS_MAXCPNUM_X	(184)
#define MCV_USERD_BTKS_MAXCPNUM_Y	(80)

// PAGE5
#define MCV_USERD_BTST_TITLE_X		(8)
#define MCV_USERD_BTST_TITLE_Y		(0)
#define MCV_USERD_BTST_K_X			(8)
#define MCV_USERD_BTST_K_Y			(24)
#define MCV_USERD_BTST_LAST_X		(8)
#define MCV_USERD_BTST_LAST_Y		(48)
#define MCV_USERD_BTST_LASTNUM_X	(64)
#define MCV_USERD_BTST_LASTNUM_Y	(48)
#define MCV_USERD_BTST_MAX_X		(8)
#define MCV_USERD_BTST_MAX_Y		(80)
#define MCV_USERD_BTST_MAXNUM_X		(64)
#define MCV_USERD_BTST_MAXNUM_Y		(80)

// PAGE6
#define MCV_USERD_BTRT_TITLE_X		(8)
#define MCV_USERD_BTRT_TITLE_Y		(0)
#define MCV_USERD_BTRT_LAST_X		(8)
#define MCV_USERD_BTRT_LAST_Y		(24)
#define MCV_USERD_BTRT_LASTNUM_X	(64)
#define MCV_USERD_BTRT_LASTNUM_Y	(24)
#define MCV_USERD_BTRT_MAX_X		(8)
#define MCV_USERD_BTRT_MAX_Y		(48)
#define MCV_USERD_BTRT_MAXNUM_X		(64)
#define MCV_USERD_BTRT_MAXNUM_Y		(48)

// PAGE7
#define MCV_USERD_MINI_TITLE_X		(8)
#define MCV_USERD_MINI_TITLE_Y		(0)
#define MCV_USERD_MINI_BC_X			(8)		// たまいれ
#define MCV_USERD_MINI_BC_Y			(24)	
#define MCV_USERD_MINI_BCNUM_X		(136)	// たまいれ　かず
#define MCV_USERD_MINI_BCNUM_Y		(24)
#define MCV_USERD_MINI_BB_X			(8)		// たまのり
#define MCV_USERD_MINI_BB_Y			(48)	
#define MCV_USERD_MINI_BBNUM_X		(136)	// たまのり　かず
#define MCV_USERD_MINI_BBNUM_Y		(48)
#define MCV_USERD_MINI_BL_X			(8)		// ふうせん
#define MCV_USERD_MINI_BL_Y			(72)	
#define MCV_USERD_MINI_BLNUM_X		(136)	// ふうせん　かず
#define MCV_USERD_MINI_BLNUM_Y		(72)

// フロンティア非表示スクリーン描画
#define MCV_USERD_NOFR_SCRN_X		( 0x1a )
#define MCV_USERD_NOFR_SCRN_Y		( 0 )
#define MCV_USERD_NOFR_SCRN_SIZX	( 0x1 )
#define MCV_USERD_NOFR_SCRN_SIZY	( 0x1 )

//  フロンティアタイプ
enum{
	MCV_FRONTIOR_TOWOR,
	MCV_FRONTIOR_FACTORY,
	MCV_FRONTIOR_FACTORY100,
	MCV_FRONTIOR_CASTLE,
	MCV_FRONTIOR_STAGE,
	MCV_FRONTIOR_ROULETTE,
	MCV_FRONTIOR_NUM,
} ;

#define VRANTRANSFERMAN_NUM	(32)	// VramTransferManagerタスク数



enum{
	WFP2P_POFIN_RET_OK,
	WFP2P_POFIN_RET_NUTSNONE,
	WFP2P_POFIN_RET_POFINMAX,
	WFP2P_POFIN_RET_POFINCASENONE,
};

enum{
	MCV_USERD_BTTN_ANM_LEFT_PUSH = 0,
	MCV_USERD_BTTN_ANM_LEFT_RELEASE,
	MCV_USERD_BTTN_ANM_BACK_PUSH,
	MCV_USERD_BTTN_ANM_BACK_RELEASE,
	MCV_USERD_BTTN_ANM_RIGHT_PUSH,
	MCV_USERD_BTTN_ANM_RIGHT_RELEASE,
};

enum{
	MCV_USERD_BTTN_MODE_WAIT,
	MCV_USERD_BTTN_MODE_NML,
	MCV_USERD_BTTN_MODE_NOBACK,
};

#define MCV_USERD_BTTN_RESCONTID	( 30 )	// リソースID
#define MCV_USERD_BTTN_BGPRI		(0)		// BG優先順位
#define MCV_USERD_BTTN_PRI			(128)		// BG優先順位
#define MCV_USERD_BTTN_Y			(172)	// ボタンY位置
#define MCV_USERD_BTTN_LEFT_X		(40)	// ←
#define MCV_USERD_BTTN_BACK_X		(128)	// もどる
#define MCV_USERD_BTTN_RIGHT_X		(224)	// →
#define MCV_USERD_BTTN_FONT_X		( -18 )	// もどる　文字位置
#define MCV_USERD_BTTN_FONT_Y		( -8 )	// もどる　文字位置
#define MCV_USERD_BTTN_FONT_SIZX	( 8 )	// もどる　文字描画範囲
#define MCV_USERD_BTTN_FONT_SIZY	( 2 )	// もどる　文字描画範囲
#define MCV_USERD_BTTN_FONT_CGXOFS	( 0 )	// CGXオフセット
#define MCV_USERD_BTTN_FONT_COL		( 0 )	// カラーパレット
#define MCV_USERD_BTTN_FONT_OAMPRI	( 0 )
#define MCV_USERD_BTTN_LEFT_SIZX	( 56 )	// ←サイズ
#define MCV_USERD_BTTN_BACK_SIZX	( 96 )	// もどるサイズ
#define MCV_USERD_BTTN_RIGHT_SIZX	( 56 )	// →さいず
#define MCV_USERD_BTTN_SIZY			( 32 )	// 縦さいず
#define MCV_USERD_BTTN_LEFT_HIT_X	( 8 )	// ←あたり判定用X
#define MCV_USERD_BTTN_BACK_HIT_X	( 80 )	// ←あたり判定用X
#define MCV_USERD_BTTN_RIGHT_HIT_X	( 192 )	// ←あたり判定用X
#define MCV_USERD_BTTN_RIGHT_HIT_Y	( 160 )	// ←あたり判定用Y
#define MCV_USERD_BTTN_ANMPUSHOK	( 3 )	// 押したと判断するボタンのアニメ
#define MCV_USERD_BTTN_ANMMAX		( 5 )	// アニメ内フレーム数
#define MCV_USERD_BTTN_ANMMAX_0ORG	( MCV_USERD_BTTN_ANMMAX-1 )	// アニメ内フレーム数

// FONTOAMY座標アニメデータ
static const s8 c_MCV_USER_BTTN_FONT_YANM[ MCV_USERD_BTTN_ANMMAX ] = {
	MCV_USERD_BTTN_FONT_Y, 
	MCV_USERD_BTTN_FONT_Y - 1, 
	MCV_USERD_BTTN_FONT_Y - 2, 
	MCV_USERD_BTTN_FONT_Y - 2, 
	MCV_USERD_BTTN_FONT_Y - 1, 
};

// ボタンシーケンス　PUSHアニメ
static const u8 c_MCV_USER_BTTN_ANM_PUSH[ MCV_USERD_BTTN_NUM ] = {
	MCV_USERD_BTTN_ANM_LEFT_PUSH,
	MCV_USERD_BTTN_ANM_BACK_PUSH,
	MCV_USERD_BTTN_ANM_RIGHT_PUSH,
};

// ボタンシーケンス　RELEASEアニメ
static const u8 c_MCV_USER_BTTN_ANM_RELEASE[ MCV_USERD_BTTN_NUM ] = {
	MCV_USERD_BTTN_ANM_LEFT_RELEASE,
	MCV_USERD_BTTN_ANM_BACK_RELEASE,
	MCV_USERD_BTTN_ANM_RIGHT_RELEASE,
};
 
static const TP_HIT_TBL	c_MCV_USER_BTTN_OAM_Hit[ MCV_USERD_BTTN_NUM ] = {
	{	// ←
		MCV_USERD_BTTN_RIGHT_HIT_Y, MCV_USERD_BTTN_RIGHT_HIT_Y+MCV_USERD_BTTN_SIZY,
		MCV_USERD_BTTN_LEFT_HIT_X, MCV_USERD_BTTN_LEFT_HIT_X+MCV_USERD_BTTN_LEFT_SIZX,
	},
	{	// もどる
		MCV_USERD_BTTN_RIGHT_HIT_Y, MCV_USERD_BTTN_RIGHT_HIT_Y+MCV_USERD_BTTN_SIZY,
		MCV_USERD_BTTN_BACK_HIT_X, MCV_USERD_BTTN_BACK_HIT_X+MCV_USERD_BTTN_BACK_SIZX,
	},
	{	// →
		MCV_USERD_BTTN_RIGHT_HIT_Y, MCV_USERD_BTTN_RIGHT_HIT_Y+MCV_USERD_BTTN_SIZY,
		MCV_USERD_BTTN_RIGHT_HIT_X, MCV_USERD_BTTN_RIGHT_HIT_X+MCV_USERD_BTTN_RIGHT_SIZX,
	},
};
enum{
	MCV_USERD_BTTN_RET_NONE,	// 何の反応もなし
	MCV_USERD_BTTN_RET_LEFT,	// 左がおされた
	MCV_USERD_BTTN_RET_BACK,	// もどるがおされた
	MCV_USERD_BTTN_RET_RIGHT,	// 右がおされた
};

#ifdef WFP2P_DEBUG	// 人をいっぱい出す
//#define WFP2PM_MANY_OBJ
#endif


static int _seqBackup;

//============================================================================================
//	プロトタイプ宣言
//============================================================================================
static _WIFI_MACH_STATUS* WifiFriendMatchStatusGet( WIFIP2PMATCH_WORK* wk, u32 idx );
static u8 WifiDwc_getFriendStatus( int idx );

static BOOL WifiP2PMatch_CommWifiBattleStart( WIFIP2PMATCH_WORK* wk, int friendno, int status );

/*** 関数プロトタイプ ***/
static void VBlankFunc( void * work );
static void VramBankSet(void);
static void BgInit( GF_BGL_INI * ini );
static void InitWork( WIFIP2PMATCH_WORK *wk );
static void FreeWork( WIFIP2PMATCH_WORK *wk );
static void BgExit( GF_BGL_INI * ini );
static void BgGraphicSet( WIFIP2PMATCH_WORK * wk, ARCHANDLE* p_handle );
static void char_pltt_manager_init(void);
static void InitCellActor(WIFIP2PMATCH_WORK *wk, ARCHANDLE* p_handle);
static void SetCellActor(WIFIP2PMATCH_WORK *wk);
static void BmpWinInit(WIFIP2PMATCH_WORK *wk, PROC* proc);
static void MainMenuMsgInit(WIFIP2PMATCH_WORK *wk);
static void SetCursor_Pos( CLACT_WORK_PTR act, int x, int y );
static void WindowSet(void);

// マッチングルーム管理用関数
static void MCRSYS_SetMoveObjWork( WIFIP2PMATCH_WORK* wk, MCR_MOVEOBJ* p_obj );
static void MCRSYS_DelMoveObjWork( WIFIP2PMATCH_WORK* wk, MCR_MOVEOBJ* p_obj );
static void MCRSYS_SetFriendObj( WIFIP2PMATCH_WORK* wk, u32 heapID );
static int MCRSYS_ContFiendInOut( WIFIP2PMATCH_WORK* wk );
static void MCRSYS_ContFriendStatus( WIFIP2PMATCH_WORK* wk, u32 heapID );
static MCR_MOVEOBJ* MCRSYS_GetMoveObjWork( WIFIP2PMATCH_WORK* wk, u32 friendNo );

static void WifiP2PMatchFriendListIconLoad( GF_BGL_INI* p_bgl, WIFIP2PMATCH_ICON* p_data, ARCHANDLE* p_handle, u32 heapID );
static void WifiP2PMatchFriendListIconRelease( WIFIP2PMATCH_ICON* p_data );
static void WifiP2PMatchFriendListIconWrite(  GF_BGL_INI* p_bgl, WIFIP2PMATCH_ICON* p_data, u32 frm, u8 cx, u8 cy, u32 icon_type, u32 col );
static void WifiP2PMatchFriendListStIconWrite( GF_BGL_INI* p_bgl, WIFIP2PMATCH_ICON* p_data, u32 frm, u8 cx, u8 cy, u32 status );
static void WifiP2PMatchFriendListBmpIconWrite(  GF_BGL_BMPWIN* p_bmp, WIFIP2PMATCH_ICON* p_data, u16 x, u16 y, u32 icon_type, u32 col );
static void WifiP2PMatchFriendListBmpStIconWrite( GF_BGL_BMPWIN* p_bmp, WIFIP2PMATCH_ICON* p_data, u16 x, u16 y, u32 status );
static int WifiP2PMatchBglFrmIconPalGet( u32 frm );

static int WifiP2PMatchFriendListStart( void );

// 友達データの強制表示処理
static void WifiP2PMatch_UserDispOn( WIFIP2PMATCH_WORK *wk, u32 friendNo, u32 heapID );
static void WifiP2PMatch_UserDispOff( WIFIP2PMATCH_WORK *wk, u32 heapID );
static void WifiP2PMatch_UserDispOff_Target( WIFIP2PMATCH_WORK *wk, u32 target_friend, u32 heapID );

static void WifiP2PMatch_UserDispOn_MyAcces( WIFIP2PMATCH_WORK *wk, u32 friendNo, u32 heapID );
static void WifiP2PMatch_UserDispOff_MyAcces( WIFIP2PMATCH_WORK *wk, u32 heapID );

// 友達データビューアー
static BOOL MCVSys_MoveCheck( const WIFIP2PMATCH_WORK *wk );
static void MCVSys_Init( WIFIP2PMATCH_WORK *wk, ARCHANDLE* p_handle, u32 heapID );
static void MCVSys_Exit( WIFIP2PMATCH_WORK *wk );
static u32 MCVSys_Updata( WIFIP2PMATCH_WORK *wk, u32 heapID );
static void MCVSys_UpdataBttn( WIFIP2PMATCH_WORK *wk );
static BOOL MCVSys_UserDispEndCheck( WIFIP2PMATCH_WORK *wk, u32 oambttn_ret );
static u32	MCVSys_UserDispGetFriend( const WIFIP2PMATCH_WORK* cp_wk );
static void MCVSys_UserDispOn( WIFIP2PMATCH_WORK *wk, u32 friendNo, u32 heapID );
static void MCVSys_UserDispOff( WIFIP2PMATCH_WORK *wk );
static void MCVSys_UserDispPageChange( WIFIP2PMATCH_WORK *wk, u32 oambttn_ret );
static void MCVSys_BttnSet( WIFIP2PMATCH_WORK *wk, u32 friendNo, u32 type );
static void MCVSys_BttnDel( WIFIP2PMATCH_WORK *wk, u32 friendNo );
static u32 MCVSys_BttnTypeGet( const WIFIP2PMATCH_WORK *wk, u32 friendNo );
static void MCVSys_ReWrite( WIFIP2PMATCH_WORK *wk, u32 heapID );
static void MCVSys_BttnAllWriteReq( WIFIP2PMATCH_WORK *wk );

static void MCVSys_BttnCallBack( u32 bttnid, u32 event, void* p_work );
static void MCVSys_BttnAnmMan( WIFIP2PMATCH_WORK *wk );
static void MCVSys_GraphicSet( WIFIP2PMATCH_WORK *wk, ARCHANDLE* p_handle, u32 heapID );
static void MCVSys_GraphicDel( WIFIP2PMATCH_WORK *wk );
static void MCVSys_GraphicScrnCGOfsChange( NNSG2dScreenData* p_scrn, u8 cgofs );
static void MCVSys_BackDraw( WIFIP2PMATCH_WORK *wk );
static void MCVSys_BttnDraw( WIFIP2PMATCH_WORK *wk );
static void MCVSys_UserDispDraw( WIFIP2PMATCH_WORK *wk, u32 heapID );
static void MCVSys_UserDispDrawType00( WIFIP2PMATCH_WORK *wk, u32 heapID );
static void MCVSys_UserDispDrawType01( WIFIP2PMATCH_WORK *wk, u32 heapID );
static void MCVSys_UserDispDrawType02( WIFIP2PMATCH_WORK *wk, u32 heapID );
static void MCVSys_UserDispDrawType03( WIFIP2PMATCH_WORK *wk, u32 heapID );
static void MCVSys_UserDispDrawType04( WIFIP2PMATCH_WORK *wk, u32 heapID );
static void MCVSys_UserDispDrawType05( WIFIP2PMATCH_WORK *wk, u32 heapID );
static void MCVSys_UserDispDrawType06( WIFIP2PMATCH_WORK *wk, u32 heapID );
static void MCVSys_UserDispDrawFrontierOffScrn( WIFIP2PMATCH_WORK *wk );
static void MCVSys_UserDispFrontiorNumDraw( WIFIP2PMATCH_WORK *wk, u32 strid, u32 factoryid, u32 friendno, u32 x, u32 y );
static void MCVSys_UserDispFrontiorTitleStrGet( WIFIP2PMATCH_WORK *wk, STRBUF* p_str, u32 factoryid, u32 friendno );
static void MCVSys_UserDispNumDraw( WIFIP2PMATCH_WORK *wk, u32 strid, u32 num, u32 x, u32 y );
static void MCVSys_BttnWrite( WIFIP2PMATCH_WORK *wk, u8 cx, u8 cy, u8 type, u8 frame );
static u32 MCVSys_StatusMsgIdGet( u32 status, int* col );
static void MCVSys_BttnWinDraw( WIFIP2PMATCH_WORK *wk, GF_BGL_BMPWIN* p_bmp, u32 friendNo, u32 frame, u32 area_id );
static void MCVSys_BttnStatusWinDraw( WIFIP2PMATCH_WORK *wk, GF_BGL_BMPWIN* p_stbmp, u32 friendNo, u32 frame, u32 area_id );
static void MCVSys_OamBttnInit( WIFIP2PMATCH_WORK *wk, ARCHANDLE* p_handle, u32 heapID );
static void MCVSys_OamBttnDelete( WIFIP2PMATCH_WORK *wk );
static void MCVSys_OamBttnOn( WIFIP2PMATCH_WORK *wk );
static void MCVSys_OamBttnOnNoBack( WIFIP2PMATCH_WORK *wk );
static void MCVSys_OamBttnOff( WIFIP2PMATCH_WORK *wk );
static u32 MCVSys_OamBttnMain( WIFIP2PMATCH_WORK *wk );
static void MCVSys_OamBttnCallBack( u32 bttn_no, u32 event, void* p_work );
static void MCVSys_OamBttnObjAnmStart( WIFIP2PMATCH_VIEW*  p_view, u32 bttn_no );
static BOOL MCVSys_OamBttnObjAnmMain( WIFIP2PMATCH_VIEW*  p_view, u32 bttn_no, u32 push_bttn, u32 event );
static s32 MCVSys_FirstFriendPageGet( const WIFIP2PMATCH_WORK * cp_wk );
static void MCVSys_FriendNameSet( WIFIP2PMATCH_WORK* p_wk, int friendno );

static void BmpWinDelete( WIFIP2PMATCH_WORK *wk );
static void _systemMessagePrint( WIFIP2PMATCH_WORK *wk, int msgno );
static void WifiP2PMatchMessagePrint( WIFIP2PMATCH_WORK *wk, int msgno , BOOL bSystem);
static void EndMessageWindowOff( WIFIP2PMATCH_WORK *wk );
static void WifiP2PMatchDataSendRecv( WIFIP2PMATCH_WORK *wk );
static void SequenceChange_MesWait( WIFIP2PMATCH_WORK *wk, int next );
static void _myStatusChange(WIFIP2PMATCH_WORK *wk, int status);
static void _myStatusChange_not_send(WIFIP2PMATCH_WORK *wk, int status);
static void _timeWaitIconDel(WIFIP2PMATCH_WORK *wk);
static void _friendNameExpand( WIFIP2PMATCH_WORK *wk, int friendNo);
static u32 _isPofinItemCheck(WIFIP2PMATCH_WORK* wk);


static void WifiP2P_SetLobbyBgm( void );
static BOOL WifiP2P_CheckLobbyBgm( void );


static void WifiP2P_Fnote_Set( WIFIP2PMATCH_WORK* wk, u32 idx );

static void _errorDisp(WIFIP2PMATCH_WORK* wk);

static void FriendRequestWaitOn( WIFIP2PMATCH_WORK* wk, BOOL msg_on );
static void FriendRequestWaitOff( WIFIP2PMATCH_WORK* wk );
static BOOL FriendRequestWaitFlagGet( const WIFIP2PMATCH_WORK* cp_wk );


// FuncTableからシーケンス遷移で呼ばれる関数
static int WifiP2PMatch_MainInit( WIFIP2PMATCH_WORK *wk, int seq );
static int _normalConnectYesNo( WIFIP2PMATCH_WORK *wk, int seq );
static int _normalConnectWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _differMachineInit( WIFIP2PMATCH_WORK *wk, int seq );
static int _differMachineNext( WIFIP2PMATCH_WORK *wk, int seq );
static int _differMachineOneMore( WIFIP2PMATCH_WORK *wk, int seq );
static int _firstYesNo( WIFIP2PMATCH_WORK *wk, int seq );
static int _poweroffInit( WIFIP2PMATCH_WORK *wk, int seq );
static int _poweroffYesNo( WIFIP2PMATCH_WORK *wk, int seq );
static int _poweroffWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _retryInit( WIFIP2PMATCH_WORK *wk, int seq );
static int _retryYesNo( WIFIP2PMATCH_WORK *wk, int seq );
static int _retryWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _retry( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_ReConnectingWait( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_ConnectingInit( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_Connecting( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_FirstSaving( WIFIP2PMATCH_WORK *wk, int seq );
static int _firstConnectEndMsg( WIFIP2PMATCH_WORK *wk, int seq );
static int _firstConnectEndMsgWait( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_FriendListInit( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_FriendList( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_VCTConnectInit2( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_VCTConnectInit( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_VCTConnectWait( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_VCTConnect( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_VCTConnectEndYesNo( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_VCTConnectEndWait( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_VCTDisconnect(WIFIP2PMATCH_WORK *wk, int seq);
static int WifiP2PMatch_BattleDisconnect(WIFIP2PMATCH_WORK *wk, int seq);
static int WifiP2PMatch_Disconnect(WIFIP2PMATCH_WORK *wk, int seq);
static int WifiP2PMatch_BattleConnectInit( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_BattleConnectWait( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_BattleConnect( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_MainReturn( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeSelectMenuInit( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeSelectMenuWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentRestart( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeSubSelectMenuWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeSelectRelInit( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeSelectRelYesNo( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeSelectRelWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _childModeMatchMenuInit( WIFIP2PMATCH_WORK *wk, int seq );
static int _childModeMatchMenuInit2( WIFIP2PMATCH_WORK *wk, int seq );
static int _childModeMatchMenuWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _childModeMatchMenuLoop( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_BCancelYesNoVCT( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_BCancelWaitVCT( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_BCancelYesNo( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_BCancelWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeCallMenuInit( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeCallMenuYesNo( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeCallMenuSend( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeCallMenuCheck( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeCallMyStatusWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _parentModeCallMenuWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _personalDataInit( WIFIP2PMATCH_WORK *wk, int seq );
static int _personalDataWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _personalDataEnd( WIFIP2PMATCH_WORK *wk, int seq );
static int _exitYesNo( WIFIP2PMATCH_WORK *wk, int seq );
static int _exitWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _exitExiting( WIFIP2PMATCH_WORK *wk, int seq );
static int _exitEnd( WIFIP2PMATCH_WORK *wk, int seq );
static int _nextBattleYesNo( WIFIP2PMATCH_WORK *wk, int seq );
static int _nextBattleWait( WIFIP2PMATCH_WORK *wk, int seq );
static int _vchatNegoCheck( WIFIP2PMATCH_WORK *wk, int seq );
static int _vchatNegoWait( WIFIP2PMATCH_WORK *wk, int seq );

static BOOL _myVChatStatusToggle(WIFIP2PMATCH_WORK *wk);
static BOOL _myVChatStatusToggleOrg(WIFIP2PMATCH_WORK *wk);
static BOOL _myVChatStatusOrgSet(WIFIP2PMATCH_WORK *wk);
static int _vchatToggleWait( WIFIP2PMATCH_WORK *wk, int seq );


static int WifiP2PMatch_EndWait( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_CheckAndEnd( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_CancelEnableWait( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_CancelEnableWaitDP( WIFIP2PMATCH_WORK *wk, int seq );
static int WifiP2PMatch_FirstSaving2( WIFIP2PMATCH_WORK *wk, int seq );



// レコードコーナーメインシーケンス用関数配列定義
static int (*FuncTable[])(WIFIP2PMATCH_WORK *wk, int seq)={
    WifiP2PMatch_MainInit,	// WIFIP2PMATCH_MODE_INIT  = 0,
    _normalConnectYesNo,    // WIFIP2PMATCH_NORMALCONNECT_YESNO
    _normalConnectWait,     // WIFIP2PMATCH_NORMALCONNECT_WAIT
    _differMachineInit,     // WIFIP2PMATCH_DIFFER_MACHINE_INIT
    _differMachineNext,     // WIFIP2PMATCH_DIFFER_MACHINE_NEXT
    _differMachineOneMore,  // WIFIP2PMATCH_DIFFER_MACHINE_ONEMORE
    _firstYesNo,            //WIFIP2PMATCH_FIRST_YESNO
    _poweroffInit,      // WIFIP2PMATCH_POWEROFF_INIT
    _poweroffYesNo,     // WIFIP2PMATCH_POWEROFF_YESNO
    _poweroffWait,      // WIFIP2PMATCH_POWEROFF_WAIT
    _retryInit,        //WIFIP2PMATCH_RETRY_INIT
    _retryYesNo,        //WIFIP2PMATCH_RETRY_YESNO
    _retryWait,         //WIFIP2PMATCH_RETRY_WAIT
    _retry,             //WIFIP2PMATCH_RETRY
    WifiP2PMatch_ConnectingInit,        // WIFIP2PMATCH_CONNECTING_INIT,
    WifiP2PMatch_Connecting,            // WIFIP2PMATCH_CONNECTING,
    _firstConnectEndMsg,                 //WIFIP2PMATCH_FIRST_ENDMSG
    _firstConnectEndMsgWait,    //WIFIP2PMATCH_FIRST_ENDMSG_WAIT
    WifiP2PMatch_FriendListInit,        // WIFIP2PMATCH_FRIENDLIST_INIT,
    WifiP2PMatch_FriendList,            // WIFIP2PMATCH_MODE_FRIENDLIST
    _vchatToggleWait,                   // WIFIP2PMATCH_VCHATWIN_WAIT
    WifiP2PMatch_VCTConnectInit2,        // WIFIP2PMATCH_MODE_VCT_CONNECT_INIT2
    WifiP2PMatch_VCTConnectInit,        // WIFIP2PMATCH_MODE_VCT_CONNECT_INIT
    WifiP2PMatch_VCTConnectWait,        // WIFIP2PMATCH_MODE_VCT_CONNECT_WAIT
    WifiP2PMatch_VCTConnect,        // WIFIP2PMATCH_MODE_VCT_CONNECT
    WifiP2PMatch_VCTConnectEndYesNo,   // WIFIP2PMATCH_MODE_VCT_CONNECTEND_YESNO
    WifiP2PMatch_VCTConnectEndWait,   // WIFIP2PMATCH_MODE_VCT_CONNECTEND_WAIT
    WifiP2PMatch_VCTDisconnect,   // WIFIP2PMATCH_MODE_VCT_DISCONNECT
    WifiP2PMatch_BattleDisconnect,   //WIFIP2PMATCH_MODE_BATTLE_DISCONNECT
    WifiP2PMatch_Disconnect,  //WIFIP2PMATCH_MODE_DISCONNECT
    WifiP2PMatch_BattleConnectInit,     // WIFIP2PMATCH_MODE_BATTLE_CONNECT_INIT
    WifiP2PMatch_BattleConnectWait,     // WIFIP2PMATCH_MODE_BATTLE_CONNECT_WAIT
    WifiP2PMatch_BattleConnect,        // WIFIP2PMATCH_MODE_BATTLE_CONNECT
    WifiP2PMatch_MainReturn,            // WIFIP2PMATCH_MODE_MAIN_MENU
    WifiP2PMatch_EndWait,			    // WIFIP2PMATCH_MODE_END_WAIT,
    WifiP2PMatch_CheckAndEnd,          //  WIFIP2PMATCH_MODE_CHECK_AND_END
    _parentModeSelectMenuInit,  // WIFIP2PMATCH_MODE_SELECT_INIT
    _parentModeSelectMenuWait,  // WIFIP2PMATCH_MODE_SELECT_WAIT
    _parentModeSubSelectMenuWait, // WIFIP2PMATCH_MODE_SUBBATTLE_WAIT
    _parentModeSelectRelInit,  //WIFIP2PMATCH_MODE_SELECT_REL_INIT
    _parentModeSelectRelYesNo, //WIFIP2PMATCH_MODE_SELECT_REL_YESNO
    _parentModeSelectRelWait,  //WIFIP2PMATCH_MODE_SELECT_REL_WAIT
    _childModeMatchMenuInit,   //WIFIP2PMATCH_MODE_MATCH_INIT
    _childModeMatchMenuInit2,   //WIFIP2PMATCH_MODE_MATCH_INIT
    _childModeMatchMenuWait,   //WIFIP2PMATCH_MODE_MATCH_WAIT
    _childModeMatchMenuLoop, //WIFIP2PMATCH_MODE_MATCH_LOOP
    WifiP2PMatch_BCancelYesNo, //WIFIP2PMATCH_MODE_BCANCEL_YESNO
    WifiP2PMatch_BCancelWait,    //WIFIP2PMATCH_MODE_BCANCEL_WAIT
    _parentModeCallMenuInit,//WIFIP2PMATCH_MODE_CALL_INIT
    _parentModeCallMenuYesNo,//WIFIP2PMATCH_MODE_CALL_YESNO
    _parentModeCallMenuSend, //WIFIP2PMATCH_MODE_CALL_SEND
    _parentModeCallMenuCheck, //WIFIP2PMATCH_MODE_CALL_CHECK
    _parentModeCallMyStatusWait, //WIFIP2PMATCH_MODE_MYSTATUS_WAIT   //51
    _parentModeCallMenuWait,//WIFIP2PMATCH_MODE_CALL_WAIT
    _personalDataInit,    //WIFIP2PMATCH_MODE_PERSONAL_INIT
    _personalDataWait,    //WIFIP2PMATCH_MODE_PERSONAL_WAIT
    _personalDataEnd,    //WIFIP2PMATCH_MODE_PERSONAL_END
    _exitYesNo,       //WIFIP2PMATCH_MODE_EXIT_YESNO
    _exitWait,        //WIFIP2PMATCH_MODE_EXIT_WAIT
    _exitExiting,        //WIFIP2PMATCH_MODE_EXITING
    _exitEnd,        //WIFIP2PMATCH_MODE_EXIT_END
    _nextBattleYesNo,  //WIFIP2PMATCH_NEXTBATTLE_YESNO
    _nextBattleWait,  //WIFIP2PMATCH_NEXTBATTLE_WAIT
    _vchatNegoCheck, //WIFIP2PMATCH_MODE_VCHAT_NEGO
    _vchatNegoWait, //WIFIP2PMATCH_MODE_VCHAT_NEGO_WAIT
    WifiP2PMatch_ReConnectingWait, //WIFIP2PMATCH_RECONECTING_WAIT
    WifiP2PMatch_BCancelYesNoVCT, //WIFIP2PMATCH_MODE_BCANCEL_YESNO_VCT
    WifiP2PMatch_BCancelWaitVCT,    //WIFIP2PMATCH_MODE_BCANCEL_WAIT_VCT
    _parentRestart,  //WIFIP2PMATCH_PARENT_RESTART
    WifiP2PMatch_FirstSaving, //WIFIP2PMATCH_FIRST_SAVING
    WifiP2PMatch_CancelEnableWait, //WIFIP2PMATCH_MODE_CANCEL_ENABLE_WAIT
    WifiP2PMatch_FirstSaving2, //WIFIP2PMATCH_FIRST_SAVING2
};

static const u8 ViewButtonFrame_y[ 4 ] = {
	8, 7, 5, 7
};


static const BMPWIN_DAT _yesNoBmpDat = {
    GF_BGL_FRAME2_M, FLD_YESNO_WIN_PX, FLD_YESNO_WIN_PY,
    FLD_YESNO_WIN_SX, FLD_YESNO_WIN_SY, FLD_YESNO_WIN_PAL, FLD_YESNO_WIN_CGX
    };

// フロンティアフラグが立っているかチェック
static BOOL _frontierInCheck( WIFIP2PMATCH_WORK * wk )
{
	return SysFlag_ArriveGet(SaveData_GetEventWork(wk->pSaveData),FLAG_ARRIVE_D32R0101);
}

// まぜまぜ料理フラグがたっているかチェック
static BOOL _pofinCaseCheck( WIFIP2PMATCH_WORK * wk )
{

	// ポルトケースがあるかチェック
	if( !MyItem_GetItemNum( SaveData_GetMyItem(wk->pSaveData),ITEM_POFINKEESU,HEAPID_WIFIP2PMATCH ) ){
		return FALSE;
	}
	return TRUE;
}

// ミニゲームフラグが立っているかチェック
static BOOL _miniGameInCheck( WIFIP2PMATCH_WORK * wk )
{
//	return FALSE;
	return TRUE;	// ミニゲームは常に見える状態にする
}

// 自分ステータスを取得する
static u32 _WifiMyStatusGet( WIFIP2PMATCH_WORK * p_wk, _WIFI_MACH_STATUS* p_status )
{
	u32 status;
	status = p_status->status;
	if( (status == WIFI_STATUS_FRONTIER) || 
		(status == WIFI_STATUS_FRONTIER_WAIT) ){

		if( _frontierInCheck( p_wk ) == FALSE ){
			status = WIFI_STATUS_UNKNOWN;	
		}
	}
	if( (status == WIFI_STATUS_BUCKET) ||
		(status == WIFI_STATUS_BUCKET_WAIT) ||
		(status == WIFI_STATUS_BALANCEBALL) ||
		(status == WIFI_STATUS_BALANCEBALL_WAIT) ||
		(status == WIFI_STATUS_BALLOON) ||
		(status == WIFI_STATUS_BALLOON_WAIT) ){

		if( _miniGameInCheck( p_wk ) == FALSE ){
			status = WIFI_STATUS_UNKNOWN;
		}
	}
	if( (status == WIFI_STATUS_POFIN) ||
		(status == WIFI_STATUS_POFIN_WAIT) ){

		if( _pofinCaseCheck( p_wk ) == FALSE ){
			status = WIFI_STATUS_UNKNOWN;
		}
	}
	return status;
}


// 2～4人であそべるか
static BOOL _wait2to4Mode( int status )
{
    switch(status){
      case WIFI_STATUS_POFIN_WAIT:    // ポフィン募集中
      case WIFI_STATUS_POFIN:		 // ポフィン中
		  
      case WIFI_STATUS_BUCKET_WAIT:    // バケット募集中
      case WIFI_STATUS_BUCKET:		 // バケット中

      case WIFI_STATUS_BALLOON_WAIT:    // バケット募集中
      case WIFI_STATUS_BALLOON:		 // バケット中

      case WIFI_STATUS_BALANCEBALL_WAIT:    // バケット募集中
      case WIFI_STATUS_BALANCEBALL:		 // バケット中
        return TRUE;
      default:
        return FALSE;
    }
}

static u32 _get2to4ModeEndSeq( int status )
{
    switch(status){
      case WIFI_STATUS_POFIN_WAIT:    // ポフィン募集中
      case WIFI_STATUS_POFIN:		 // ポフィン中
		return WIFI_P2PMATCH_POFIN;
      case WIFI_STATUS_BUCKET_WAIT:    // バケット募集中
      case WIFI_STATUS_BUCKET:		 // バケット中
		return WIFI_P2PMATCH_BUCKET;
      case WIFI_STATUS_BALLOON_WAIT:    // バケット募集中
      case WIFI_STATUS_BALLOON:		 // バケット中
		return WIFI_P2PMATCH_BALLOON;
      case WIFI_STATUS_BALANCEBALL_WAIT:    // バケット募集中
      case WIFI_STATUS_BALANCEBALL:		 // バケット中
		return WIFI_P2PMATCH_BALANCEBALL;
      default:
		break;
    }

	GF_ASSERT(0);
	return 0;
}

// ステートがBATTLEWAITかどうか
static BOOL _modeBattleWait(int status)
{
    switch(status){
      case WIFI_STATUS_SBATTLE50_WAIT:   // シングルLv50対戦募集中
      case WIFI_STATUS_SBATTLE100_WAIT:   // シングルLv100対戦募集中
      case WIFI_STATUS_SBATTLE_FREE_WAIT:   // シングルLv100対戦募集中
      case WIFI_STATUS_DBATTLE50_WAIT:   // ダブルLv50対戦募集中
      case WIFI_STATUS_DBATTLE100_WAIT:   // ダブルLv100対戦募集中
      case WIFI_STATUS_DBATTLE_FREE_WAIT:   // ダブルLv100対戦募集中
#ifdef WFP2P_DEBUG_EXON
      case WIFI_STATUS_MBATTLE_FREE_WAIT:   // マルチバトル　フリー対戦募集中
#endif
        return TRUE;
      default:
        return FALSE;
    }
}

// ステートがWAITかどうか
static BOOL _modeWait(int status)
{
    if(_modeBattleWait(status)){
        return TRUE;
    }
    switch(status){
      case WIFI_STATUS_TRADE_WAIT:    // 交換募集中
      case WIFI_STATUS_POFIN_WAIT:    // ポフィン募集中
      case WIFI_STATUS_FRONTIER_WAIT:    // フロンティア募集中
#ifdef WFP2P_DEBUG_EXON 
      case WIFI_STATUS_BATTLEROOM_WAIT:    // バトルルーム募集中
#endif
      case WIFI_STATUS_BUCKET_WAIT:    // バケット募集中
      case WIFI_STATUS_BALLOON_WAIT:    // バケット募集中
      case WIFI_STATUS_BALANCEBALL_WAIT:    // バケット募集中
        return TRUE;
    }
    return FALSE;
}

// ステートがBATTLEかどうか
static BOOL _modeBattle(int status)
{
    switch(status){
      case WIFI_STATUS_SBATTLE50:      // シングル対戦中
      case WIFI_STATUS_SBATTLE100:      // シングル対戦中
      case WIFI_STATUS_SBATTLE_FREE:      // シングル対戦中
      case WIFI_STATUS_DBATTLE50:      // ダブル対戦中
      case WIFI_STATUS_DBATTLE100:      // ダブル対戦中
      case WIFI_STATUS_DBATTLE_FREE:      // ダブル対戦中
		  
#ifdef WFP2P_DEBUG_EXON 
      case WIFI_STATUS_MBATTLE_FREE:      // マルチ対戦中
#endif
        return TRUE;
      default:
        return FALSE;
    }
}

// ステートがACTIVEかどうか
static BOOL _modeActive(int status)
{
    if(_modeBattle(status)){
        return TRUE;
    }
    switch(status){
      case WIFI_STATUS_TRADE:    // 交換中
      case WIFI_STATUS_POFIN:    // ポフィン中
      case WIFI_STATUS_FRONTIER:    // フロンティア中
#ifdef WFP2P_DEBUG_EXON 
      case WIFI_STATUS_BATTLEROOM:    // バトルルーム中
#endif
      case WIFI_STATUS_BUCKET:    // バケット中
      case WIFI_STATUS_BALLOON:    // バケット中
      case WIFI_STATUS_BALANCEBALL:    // バケット中
      case WIFI_STATUS_VCT:    // 交換中
        return TRUE;
    }
    return FALSE;
}




static BOOL _is2pokeMode(int status)
{
    switch(status){
      case WIFI_STATUS_TRADE_WAIT:
      case WIFI_STATUS_DBATTLE50_WAIT:
      case WIFI_STATUS_DBATTLE100_WAIT:
      case WIFI_STATUS_DBATTLE_FREE_WAIT:
        return TRUE;
    }
    return FALSE;
}

static int _convertState(int state)
{
    int ret = WIFI_STATUS_UNKNOWN;

    switch(state){
      case WIFI_STATUS_POFIN_WAIT:
        ret = WIFI_STATUS_POFIN;
        break;
      case WIFI_STATUS_FRONTIER_WAIT:
        ret = WIFI_STATUS_FRONTIER;
        break;
#ifdef WFP2P_DEBUG_EXON 
      case WIFI_STATUS_BATTLEROOM_WAIT:
        ret = WIFI_STATUS_BATTLEROOM;
        break;
#endif
      case WIFI_STATUS_BUCKET_WAIT:
        ret = WIFI_STATUS_BUCKET;
        break;
      case WIFI_STATUS_BALLOON_WAIT:
        ret = WIFI_STATUS_BALLOON;
        break;
      case WIFI_STATUS_BALANCEBALL_WAIT:
        ret = WIFI_STATUS_BALANCEBALL;
        break;
      case WIFI_STATUS_TRADE_WAIT:
        ret = WIFI_STATUS_TRADE;
        break;
      case WIFI_STATUS_SBATTLE50_WAIT:
        ret = WIFI_STATUS_SBATTLE50;
        break;
      case WIFI_STATUS_SBATTLE100_WAIT:
        ret = WIFI_STATUS_SBATTLE100;
        break;
      case WIFI_STATUS_SBATTLE_FREE_WAIT:
        ret = WIFI_STATUS_SBATTLE_FREE;
        break;
      case WIFI_STATUS_DBATTLE50_WAIT:
        ret = WIFI_STATUS_DBATTLE50;
        break;
      case WIFI_STATUS_DBATTLE100_WAIT:
        ret = WIFI_STATUS_DBATTLE100;
        break;
      case WIFI_STATUS_DBATTLE_FREE_WAIT:
        ret = WIFI_STATUS_DBATTLE_FREE;
        break;
      case WIFI_STATUS_LOGIN_WAIT:
        ret = WIFI_STATUS_VCT;
        break;
      case WIFI_STATUS_POFIN:
        ret = WIFI_STATUS_POFIN_WAIT;
        break;
      case WIFI_STATUS_FRONTIER:
        ret = WIFI_STATUS_FRONTIER_WAIT;
        break;
      case WIFI_STATUS_BUCKET:
        ret = WIFI_STATUS_BUCKET_WAIT;
        break;
      case WIFI_STATUS_BALLOON:
        ret = WIFI_STATUS_BALLOON_WAIT;
        break;
      case WIFI_STATUS_BALANCEBALL:
        ret = WIFI_STATUS_BALANCEBALL_WAIT;
        break;
#ifdef WFP2P_DEBUG_EXON 
      case WIFI_STATUS_BATTLEROOM:
        ret = WIFI_STATUS_BATTLEROOM_WAIT;
        break;
#endif
      case WIFI_STATUS_TRADE:
        ret = WIFI_STATUS_TRADE_WAIT;
        break;
      case WIFI_STATUS_SBATTLE50:
        ret = WIFI_STATUS_SBATTLE50_WAIT;
        break;
      case WIFI_STATUS_SBATTLE100:
        ret = WIFI_STATUS_SBATTLE100_WAIT;
        break;
      case WIFI_STATUS_SBATTLE_FREE:
        ret = WIFI_STATUS_SBATTLE_FREE_WAIT;
        break;
      case WIFI_STATUS_DBATTLE50:
        ret = WIFI_STATUS_DBATTLE50_WAIT;
        break;
      case WIFI_STATUS_DBATTLE100:
        ret = WIFI_STATUS_DBATTLE100_WAIT;
        break;
      case WIFI_STATUS_DBATTLE_FREE:
        ret = WIFI_STATUS_DBATTLE_FREE_WAIT;
        break;
#ifdef WFP2P_DEBUG_EXON 
      case WIFI_STATUS_MBATTLE_FREE:
        ret = WIFI_STATUS_MBATTLE_FREE_WAIT;
        break;
      case WIFI_STATUS_MBATTLE_FREE_WAIT:
        ret = WIFI_STATUS_MBATTLE_FREE;
        break;
#endif
      case WIFI_STATUS_VCT:
        ret = WIFI_STATUS_LOGIN_WAIT;
        break;
    }
    return ret;
}

static int _getBattlePokeNum(WIFIP2PMATCH_WORK* wk)
{
    POKEPARTY* pMyPoke = SaveData_GetTemotiPokemon(wk->pSaveData);
    POKEMON_PARAM* poke;
    int max = PokeParty_GetPokeCount(pMyPoke);
    int i,num = 0;

	for(i = 0 ; i < max ; i++){
		poke = PokeParty_GetMemberPointer(pMyPoke, i);
        if (PokeParaGet(poke, ID_PARA_hp, NULL) == 0) {
            continue;
        }
        if (PokeParaGet(poke, ID_PARA_tamago_flag, NULL)) {
            continue;
        }
        num++;
	}
    return num;
}

//--------------------------------------------------------------------------------------------
/**
 * ステータスにあわせて通信ステートも変更する
 * @param	status
 * @return	none
 */
//--------------------------------------------------------------------------------------------

static void _commStateChange(int status)
{
    if( (status == WIFI_STATUS_POFIN_WAIT) || (WIFI_STATUS_POFIN == status) ){    // ポフィン募集中
        CommStateChangeWiFiPofin();
    }
    else if(_wait2to4Mode(status)){
        CommStateChangeWiFiClub();	// WiFilobbyMinigameにすると
									// 退室処理がおかしくなる
									
//        CommStateChangeWiFiLogin();こっちをゆうこうにするとバグる。２人しかつながらない
    }
    else if(status == WIFI_STATUS_LOGIN_WAIT){
        CommStateChangeWiFiLogin();
    }
    else if( (status == WIFI_STATUS_FRONTIER_WAIT) || (status == WIFI_STATUS_FRONTIER) ){
		CommSetWifiBothNet(FALSE);
		CommStateChangeWiFiFactory();
    }
    else{
	    CommSetWifiBothNet(TRUE); // wifiの通信を非同期から同期に
        CommStateChangeWiFiBattle();
    }


}

//============================================================================================
//	プロセス関数
//============================================================================================

static void _graphicInit(WIFIP2PMATCH_WORK * wk)
{
	ARCHANDLE* p_handle;
	
    sys_VBlankFuncChange( NULL, NULL );	// VBlankセット
    sys_HBlankIntrStop();	//HBlank割り込み停止

    GF_Disp_GX_VisibleControlInit();
    GF_Disp_GXS_VisibleControlInit();
    GX_SetVisiblePlane( 0 );
    GXS_SetVisiblePlane( 0 );
	sys_KeyRepeatSpeedSet( SYS_KEYREPEAT_SPEED_DEF, SYS_KEYREPEAT_WAIT_DEF );

	p_handle = ArchiveDataHandleOpen( ARC_WIFIP2PMATCH_GRA, HEAPID_WIFIP2PMATCH );

    wk->bgl = GF_BGL_BglIniAlloc( HEAPID_WIFIP2PMATCH );
    // 文字列マネージャー生成
    wk->WordSet    = WORDSET_Create( HEAPID_WIFIP2PMATCH );
    wk->MsgManager = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_wifi_lobby_dat, HEAPID_WIFIP2PMATCH );
    wk->SysMsgManager = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_wifi_system_dat, HEAPID_WIFIP2PMATCH );

    // VRAM バンク設定
    VramBankSet();

    // BGLレジスタ設定
    BgInit( wk->bgl );

    //BGグラフィックセット
    BgGraphicSet( wk, p_handle );

	//パレットアニメシステム作成
	ConnectBGPalAnm_Init(&wk->cbp, p_handle, NARC_wifip2pmatch_conect_anm_NCLR, HEAPID_WIFIP2PMATCH);

    // VBlank関数セット
    sys_VBlankFuncChange( VBlankFunc, wk );

    // OBJキャラ、パレットマネージャー初期化
    char_pltt_manager_init();

    // CellActorシステム初期化
    InitCellActor(wk, p_handle);

    // CellActro表示登録
    SetCellActor(wk);

	ArchiveDataHandleClose( p_handle );
}


//--------------------------------------------------------------------------------------------
/**
 * プロセス関数：初期化
 *
 * @param	proc	プロセスデータ
 * @param	seq		シーケンス
 *
 * @return	処理状況
 */
//--------------------------------------------------------------------------------------------
PROC_RESULT WifiP2PMatchProc_Init( PROC * proc, int * seq )
{
    WIFIP2PMATCH_WORK * wk;
    WIFIP2PMATCH_PROC_PARAM* pParentWork;
	u32 result;

    switch(*seq){
      case 0:
		  
        pParentWork = PROC_GetParentWork(proc);

		// 通信拡張ヒープのメモリ確保
		// 080611	tomoya 追加
		if(CommStateIsWifiConnect()){       // 接続中
			switch( pParentWork->seq ){	// 1つ前の遊びで通信拡張ヒープを破棄していたら再確保
			case WIFI_P2PMATCH_END:
			case WIFI_P2PMATCH_UTIL:
			case WIFI_P2PMATCH_DPW_END:
			case WIFI_P2PMATCH_SBATTLE50:
			case WIFI_P2PMATCH_SBATTLE100:
			case WIFI_P2PMATCH_SBATTLE_FREE:
			case WIFI_P2PMATCH_DBATTLE50:
			case WIFI_P2PMATCH_DBATTLE100:
			case WIFI_P2PMATCH_DBATTLE_FREE:
			case WIFI_P2PMATCH_FRONTIER:
			case WIFI_P2PMATCH_TRADE:
				// バトルから戻ってきたなら拡張ヒープ作成
				sys_CreateHeap( HEAPID_BASE_APP, HEAPID_WIFIP2PMATCHEX, WIFI_P2PMATCH_EXHEAP_SIZE );
				break;

			default:
				break;
			}
		}else{
			// 通信前なら絶対に拡張ヒープを確保
			sys_CreateHeap( HEAPID_BASE_APP, HEAPID_WIFIP2PMATCHEX, WIFI_P2PMATCH_EXHEAP_SIZE );
		}

		// wifi_2dmapオーバーレイ読込み
		Overlay_Load( FS_OVERLAY_ID(wifi_2dmapsys), OVERLAY_LOAD_NOT_SYNCHRONIZE);

        if(pParentWork->seq == WIFI_P2PMATCH_DPW){ // 世界対戦なら
            sys_CreateHeap( HEAPID_BASE_APP, HEAPID_WIFIP2PMATCH, 0x70000 );
        }
        else{
//            sys_CreateHeap( HEAPID_BASE_APP, HEAPID_WIFIP2PMATCH, 0xb0000 );
            sys_CreateHeap( HEAPID_BASE_APP, HEAPID_WIFIP2PMATCH, 0xa0000 );
        }
        wk = PROC_AllocWork( proc, sizeof(WIFIP2PMATCH_WORK), HEAPID_WIFIP2PMATCH );
        MI_CpuFill8( wk, 0, sizeof(WIFIP2PMATCH_WORK) );

		// Vram転送マネージャ作成
		initVramTransferManagerHeap( VRANTRANSFERMAN_NUM, HEAPID_WIFIP2PMATCH );

        wk->MsgIndex = _PRINTTASK_MAX;
        wk->pSaveData = pParentWork->pSaveData;
        wk->pList = SaveData_GetWifiListData(wk->pSaveData);
        wk->initSeq = pParentWork->seq;    // P2PかDPWか
        wk->endSeq = WIFI_P2PMATCH_END;
        wk->preConnect = -1;

        GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0|GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3, VISIBLE_OFF );
        GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0|GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3, VISIBLE_OFF );

#if AFTERMASTER_061215_WIFIP2P_FADE_FIX
        // ワーク初期化
        InitWork( wk );
        // グラフィック初期化
        _graphicInit(wk);
        // ワイプフェード開始
        WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK,
                        COMM_BRIGHTNESS_SYNC, 1, HEAPID_WIFIP2PMATCH);

#else
        
        // ワーク初期化
        InitWork( wk );
        //        WIPE_ResetBrightness( WIPE_DISP_MAIN );
        WIPE_ResetBrightness( WIPE_DISP_SUB );
        WIPE_ResetWndMask( WIPE_DISP_SUB );
        // ワイプフェード開始
        WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK,
                        COMM_BRIGHTNESS_SYNC, 1, HEAPID_WIFIP2PMATCH);
        _graphicInit(wk);
#endif
        if(CommStateIsWifiConnect()){
			WirelessIconEasy();  // 接続中なのでアイコン表示
        }
        else{
            DwcOverlayStart();    // WIFIオーバーレイ
        }
		
		// タッチパネル開始
		result = InitTPNoBuff( 4 );
		GF_ASSERT( result == TP_OK );
		
        (*seq)++;
        break;
      case 1:
        wk = PROC_GetWork( proc );
        (*seq) = SEQ_IN;
        return PROC_RES_FINISH;
        break;
    }
    return PROC_RES_CONTINUE;
}




//--------------------------------------------------------------------------------------------
/**
 * プロセス関数：メイン
 *
 * @param	proc	プロセスデータ
 * @param	seq		シーケンス
 *
 * @return	処理状況
 */
//--------------------------------------------------------------------------------------------

PROC_RESULT WifiP2PMatchProc_Main( PROC * proc, int * seq )
{
    WIFIP2PMATCH_WORK * wk  = PROC_GetWork( proc );

    switch( *seq ){
      case SEQ_IN:
        if( WIPE_SYS_EndCheck() ){
            // ワイプ処理待ち
            *seq = SEQ_MAIN;


        }
        break;

      case SEQ_MAIN:

#ifdef _WIFI_DEBUG_TUUSHIN
		if( WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_MODE != _WIFI_DEBUG_NONE ){
			sys.trg |= PAD_KEY_DOWN;	//  したおしっぱ
			sys.cont |= PAD_KEY_DOWN;
			sys.trg |= PAD_BUTTON_A;
			sys.cont |= PAD_BUTTON_A;
		}
#endif

		
        // カーソル移動

        // シーケンス毎の動作
            if(FuncTable[wk->seq]!=NULL){
            static int seqBk=0;

            if(seqBk != wk->seq){
//                OHNO_PRINT("wp2p %d \n",wk->seq);
                OS_TPrintf("wp2p %d \n",wk->seq);
            }
            seqBk = wk->seq;

            *seq = (*FuncTable[wk->seq])( wk, *seq );
        }

        break;

      case SEQ_OUT:
        if( WIPE_SYS_EndCheck() ){
            return PROC_RES_FINISH;
        }
        break;
    }
#if 1
    if(wk->clactSet){
        CLACT_Draw( wk->clactSet );									// セルアクター常駐関数
    }
#endif
	if( WIFI_MCR_GetInitFlag( &wk->matchroom ) == TRUE ){
		WIFI_MCR_Draw( &wk->matchroom );
	}
	
	ConnectBGPalAnm_Main(&wk->cbp);
	
    return PROC_RES_CONTINUE;
}

#define DEFAULT_NAME_MAX		18

// ダイヤ・パールで変わるんだろう
#define MALE_NAME_START			0
#define FEMALE_NAME_START		18


//--------------------------------------------------------------------------------------------
/**
 * グラフィックにかかわる部分の終了処理
 * @param	wk	WIFIP2PMATCH_WORK
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void _graphicEnd(WIFIP2PMATCH_WORK* wk)
{
    int i;

    sys_VBlankFuncChange( NULL, NULL );	// VBlankReset

	ConnectBGPalAnm_End(&wk->cbp);

	// マッチングルーム破棄
	if( WIFI_MCR_GetInitFlag( &wk->matchroom ) == TRUE ){
		WIFI_MCR_Dest( &wk->matchroom );
	}
	// ビューアー破棄
	if( MCVSys_MoveCheck( wk ) == TRUE ){
		MCVSys_Exit( wk );
	}

	// アイコン破棄
	WifiP2PMatchFriendListIconRelease( &wk->icon );

#if 0
    // キャラ転送マネージャー破棄
    CLACT_U_CharManagerDelete(wk->resObjTbl[MAIN_LCD][CLACT_U_CHAR_RES]);

    // パレット転送マネージャー破棄
    CLACT_U_PlttManagerDelete(wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES]);
#endif

	// FontOAM破棄
	FONTOAM_SysDelete( wk->fontoam );

    // キャラ・パレット・セル・セルアニメのリソースマネージャー破棄
    for(i=0;i<CLACT_RESOURCE_NUM;i++){
        CLACT_U_ResManagerDelete(wk->resMan[i]);
    }
    // セルアクターセット破棄
    CLACT_DestSet(wk->clactSet);
	
    //OAMレンダラー破棄
    REND_OAM_Delete();

    // リソース解放
    DeleteCharManager();
    DeletePlttManager();

    // BMPウィンドウ開放
    BmpWinDelete( wk );

    // BGL削除
    BgExit( wk->bgl );

    // 親のプロックの処理
    //    UnionRoomView_ObjInit( (COMM_UNIONROOM_VIEW*)PROC_GetParentWork( proc ) );

    // タッチパネルシステム終了
    //	StopTP();


    // メッセージマネージャー・ワードセットマネージャー解放
    MSGMAN_Delete( wk->SysMsgManager );
    MSGMAN_Delete( wk->MsgManager );
    WORDSET_Delete( wk->WordSet );

	// オーバーレイ破棄
	Overlay_UnloadID( FS_OVERLAY_ID(wifi_2dmapsys) );
}


//--------------------------------------------------------------------------------------------
/**
 * プロセス関数：終了
 *
 * @param	proc	プロセスデータ
 * @param	seq		シーケンス
 *
 * @return	処理状況
 */
//--------------------------------------------------------------------------------------------
PROC_RESULT WifiP2PMatchProc_End( PROC * proc, int * seq )
{
    WIFIP2PMATCH_WORK  *wk    = PROC_GetWork( proc );
    WIFIP2PMATCH_PROC_PARAM* pParentWork;


    _graphicEnd(wk);

    pParentWork = PROC_GetParentWork(proc);
    pParentWork->seq = wk->endSeq;
    if( (wk->endSeq == WIFI_P2PMATCH_END) || (wk->endSeq == WIFI_P2PMATCH_DPW_END) ){  // オーバーレイ解除
        DwcOverlayEnd();
    }
    else{
        pParentWork->targetID = mydwc_getFriendIndex();
    }
	
    // ワーク解放
    FreeWork( wk );

    PROC_FreeWork( proc );				// PROCワーク開放
	
	// タッチパネル停止
	StopTP();

	// VramTransferマネージャ破棄
	DellVramTransferManager();

    sys_DeleteHeap( HEAPID_WIFIP2PMATCH );
    //    WIPE_ResetBrightness( WIPE_DISP_MAIN );
    //  WIPE_ResetBrightness( WIPE_DISP_SUB );
	

	// 拡張ヒープの破棄
	// 通信の終了と、バトル時は拡張ヒープを破棄する
	// それ以外は破棄しない
	switch( wk->endSeq ){
	case WIFI_P2PMATCH_END:
	case WIFI_P2PMATCH_UTIL:
	case WIFI_P2PMATCH_DPW_END:
	case WIFI_P2PMATCH_SBATTLE50:
	case WIFI_P2PMATCH_SBATTLE100:
	case WIFI_P2PMATCH_SBATTLE_FREE:
	case WIFI_P2PMATCH_DBATTLE50:
	case WIFI_P2PMATCH_DBATTLE100:
	case WIFI_P2PMATCH_DBATTLE_FREE:
	case WIFI_P2PMATCH_FRONTIER:
	case WIFI_P2PMATCH_TRADE:

		sys_DeleteHeap( HEAPID_WIFIP2PMATCHEX );
		break;

	default:
		break;
	}

	
	// BGMがポケセンのままならBGM音量を変更する
	if( WifiP2P_CheckLobbyBgm() == TRUE ){
		Snd_PlayerSetInitialVolume( SND_HANDLE_FIELD, BGM_POKECEN_VOL );
	}

#if 0	//↑で音量処理が入るのでいらない
	// VCHAT中の通信エラーの場合BGMの音量を元に戻す
	if( (wk->endSeq == WIFI_P2PMATCH_END) || (wk->endSeq == WIFI_P2PMATCH_DPW_END) ){  // オーバーレイ解除
		//音量セット
		Snd_PlayerSetInitialVolume( SND_HANDLE_FIELD, BGM_VOL_MAX );
    }
#endif


    return PROC_RES_FINISH;
}

//--------------------------------------------------------------------------------------------
/**
 * VBlank関数
 *
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void VBlankFunc( void * work )
{
    WIFIP2PMATCH_WORK* wk = work;

    // BG書き換え
	GF_BGL_VBlankFunc( wk->bgl );
    // セルアクター
    // Vram転送マネージャー実行
    DoVramTransferManager();

    // レンダラ共有OAMマネージャVram転送
    REND_OAMTrans();

	ConnectBGPalAnm_VBlank(&wk->cbp);

	// _retry関数内でマスター輝度を設定して、
	// きれいに復帰できるようにするためここで復帰
	if( wk->brightness_reset == TRUE ){
		WIPE_ResetBrightness( WIPE_DISP_MAIN );
		WIPE_ResetBrightness( WIPE_DISP_SUB );
		wk->brightness_reset = FALSE;
	}
	
// OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}


//--------------------------------------------------------------------------------------------
/**
 * VRAM設定
 *
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void VramBankSet(void)
{
    GF_BGL_DISPVRAM tbl = {
        GX_VRAM_BG_128_A,				// メイン2DエンジンのBG
        GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット

        GX_VRAM_SUB_BG_128_C,			// サブ2DエンジンのBG
        GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット

//        GX_VRAM_OBJ_64_E,				// メイン2DエンジンのOBJ
		GX_VRAM_OBJ_128_B,				// メイン2DエンジンのOBJ
        GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット

        GX_VRAM_SUB_OBJ_16_I,			// サブ2DエンジンのOBJ
        GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット

        GX_VRAM_TEX_NONE,				// テクスチャイメージスロット
        GX_VRAM_TEXPLTT_NONE			// テクスチャパレットスロット
        };
    GF_Disp_SetBank( &tbl );
}

//--------------------------------------------------------------------------------------------
/**
 * BG設定
 *
 * @param	ini		BGLデータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BgInit( GF_BGL_INI * ini )
{
    // BG SYSTEM
    {
        GF_BGL_SYS_HEADER BGsys_data = {
            GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
        };
        GF_BGL_InitBG( &BGsys_data );
    }

    // 背景面
    {
        GF_BGL_BGCNT_HEADER TextBgCntDat = {
            0, 0, 0x1000, 0, GF_BGL_SCRSIZ_512x256, GX_BG_COLORMODE_16,
            GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
            0, 0, 0, FALSE
            };
        GF_BGL_BGControlSet( ini, GF_BGL_FRAME0_M, &TextBgCntDat, GF_BGL_MODE_TEXT );
        GF_BGL_ClearCharSet( GF_BGL_FRAME0_M, 32, 0, HEAPID_WIFIP2PMATCH);
        GF_BGL_ScrClear( ini, GF_BGL_FRAME0_M );


    }

    // メイン画面1
    {
        GF_BGL_BGCNT_HEADER TextBgCntDat = {
            0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
            GX_BG_SCRBASE_0xd000, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
            1, 0, 0, FALSE
            };
        GF_BGL_BGControlSet( ini, GF_BGL_FRAME1_M, &TextBgCntDat, GF_BGL_MODE_TEXT );
        GF_BGL_ClearCharSet( GF_BGL_FRAME1_M, 32, 0, HEAPID_WIFIP2PMATCH);
        GF_BGL_ScrClear( ini, GF_BGL_FRAME1_M );
    }

    // リストなど
    {
        GF_BGL_BGCNT_HEADER TextBgCntDat = {
            0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
            GX_BG_SCRBASE_0xd800, GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_23,
            2, 0, 0, FALSE
            };
        GF_BGL_BGControlSet( ini, GF_BGL_FRAME2_M, &TextBgCntDat, GF_BGL_MODE_TEXT );
        GF_BGL_ClearCharSet( GF_BGL_FRAME2_M, 32, 0, HEAPID_WIFIP2PMATCH);
        GF_BGL_ScrClear( ini, GF_BGL_FRAME2_M );
    }

#if 1
    // いろいろに使う
    {
        GF_BGL_BGCNT_HEADER TextBgCntDat = {
            0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
            GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x18000, GX_BG_EXTPLTT_23,
            3, 0, 0, FALSE
            };
        GF_BGL_BGControlSet( ini, GF_BGL_FRAME3_M, &TextBgCntDat, GF_BGL_MODE_TEXT );
        GF_BGL_ClearCharSet( GF_BGL_FRAME3_M, 32, 0, HEAPID_WIFIP2PMATCH);
        GF_BGL_ScrClear( ini, GF_BGL_FRAME3_M );
    }
#endif

    // サブ画面テキスト面
    {
        GF_BGL_BGCNT_HEADER TextBgCntDat = {
            0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
            GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
            3, 0, 0, FALSE
            };
        GF_BGL_BGControlSet( ini, GF_BGL_FRAME0_S, &TextBgCntDat, GF_BGL_MODE_TEXT );
        GF_BGL_ScrClear( ini, GF_BGL_FRAME0_S );
    }
	// ユーザー状態表示面
    {
        GF_BGL_BGCNT_HEADER TextBgCntDat = {
            0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
            GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_01,
            0, 0, 0, FALSE
            };
        GF_BGL_BGControlSet( ini, GF_BGL_FRAME1_S, &TextBgCntDat, GF_BGL_MODE_TEXT );
        GF_BGL_ScrClear( ini, GF_BGL_FRAME1_S );
    }
	// ボタンorユーザーデータ時の背景
    {
        GF_BGL_BGCNT_HEADER TextBgCntDat = {
            0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
            GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
            2, 0, 0, FALSE
        };
        GF_BGL_BGControlSet( ini, GF_BGL_FRAME2_S, &TextBgCntDat, GF_BGL_MODE_TEXT );
        GF_BGL_ScrClear( ini, GF_BGL_FRAME2_S );
    }
	// ユーザーデータテキスト
    {
        GF_BGL_BGCNT_HEADER TextBgCntDat = {
            0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
            GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x18000, GX_BG_EXTPLTT_01,
            0, 0, 0, FALSE
            };
        GF_BGL_BGControlSet( ini, GF_BGL_FRAME3_S, &TextBgCntDat, GF_BGL_MODE_TEXT );
        GF_BGL_ScrClear( ini, GF_BGL_FRAME3_S );
    }

    GF_BGL_ClearCharSet( GF_BGL_FRAME0_M, 32, 0, HEAPID_WIFIP2PMATCH );
    GF_BGL_ClearCharSet( GF_BGL_FRAME1_M, 32, 0, HEAPID_WIFIP2PMATCH );
    GF_BGL_ClearCharSet( GF_BGL_FRAME2_M, 32, 0, HEAPID_WIFIP2PMATCH );
    GF_BGL_ClearCharSet( GF_BGL_FRAME3_M, 32, 0, HEAPID_WIFIP2PMATCH );
    GF_BGL_ClearCharSet( GF_BGL_FRAME0_S, 32, 0, HEAPID_WIFIP2PMATCH );
    GF_BGL_ClearCharSet( GF_BGL_FRAME1_S, 32, 0, HEAPID_WIFIP2PMATCH );
    GF_BGL_ClearCharSet( GF_BGL_FRAME2_S, 32, 0, HEAPID_WIFIP2PMATCH );
    GF_BGL_ClearCharSet( GF_BGL_FRAME3_S, 32, 0, HEAPID_WIFIP2PMATCH );

    GF_BGL_PrioritySet(GF_BGL_FRAME0_M , 3);  //はいけい
    GF_BGL_PrioritySet(GF_BGL_FRAME1_M , 1);   // yesno win　ユーザーデータの背景
    GF_BGL_PrioritySet(GF_BGL_FRAME3_M , 0);  // メッセージ
    GF_BGL_PrioritySet(GF_BGL_FRAME2_M , 0);   // menuリスト

    G2_BlendNone();
    G2S_BlendNone();
}

static void WindowSet(void)
{
    GX_SetVisibleWnd( GX_WNDMASK_NONE );
    GXS_SetVisibleWnd( GX_WNDMASK_NONE );
    /*
    GX_SetVisibleWnd( GX_WNDMASK_W0 );
	G2_SetWnd0InsidePlane( GX_WND_PLANEMASK_NONE, 1 );
	G2_SetWndOutsidePlane( GX_WND_PLANEMASK_NONE, 1 );
	G2_SetWnd0Position( 0, 255, 0, 192 );

	GXS_SetVisibleWnd( GX_WNDMASK_W0 );
	G2S_SetWnd0InsidePlane( GX_WND_PLANEMASK_NONE, 1 );
	G2S_SetWndOutsidePlane( GX_WND_PLANEMASK_NONE, 1 );
	G2S_SetWnd0Position( 0, 255, 0, 192 );
     */
}

//----------------------------------------------------------------------------
/**
 *	@brief	生成した動作オブジェクトワークを設定する
 *
 *	@param	wk		ワーク
 *	@param	p_obj	オブジェクトワーク
 */
//-----------------------------------------------------------------------------
static void MCRSYS_SetMoveObjWork( WIFIP2PMATCH_WORK* wk, MCR_MOVEOBJ* p_obj )
{
	int i;

	for( i=0; i<MATCHROOM_IN_OBJNUM; i++ ){
		if( wk->p_matchMoveObj[i] == NULL ){
			wk->p_matchMoveObj[i] = p_obj;
			return ;
		}
	}
	GF_ASSERT( 0 );
}
//----------------------------------------------------------------------------
/**
 *	@brief	引数のOBJが入っているワークをNULLにする
 *
 *	@param	wk		ワーク
 *	@param	p_obj	NULLを入れてほしいワークに入っているポインタ
 */
//-----------------------------------------------------------------------------
static void MCRSYS_DelMoveObjWork( WIFIP2PMATCH_WORK* wk, MCR_MOVEOBJ* p_obj )
{
	int i;

	for( i=0; i<MATCHROOM_IN_OBJNUM; i++ ){
		if( wk->p_matchMoveObj[i] == p_obj ){
			wk->p_matchMoveObj[i] = NULL;
			return ;
		}
	}
	GF_ASSERT( 0 );
}

//----------------------------------------------------------------------------
/**
 *	@brief	friendNOのオブジェクトワークを取得する
 *
 *	@param	wk			ワーク
 *	@param	friendNo	friendNO
 *
 *	@return	オブジェクトワーク
 */
//-----------------------------------------------------------------------------
static MCR_MOVEOBJ* MCRSYS_GetMoveObjWork( WIFIP2PMATCH_WORK* wk, u32 friendNo )
{
	int i;

	for( i=0; i<MATCHROOM_IN_OBJNUM; i++ ){
		if( wk->p_matchMoveObj[i] ){
			if( WIFI_MCR_GetFriendNo( wk->p_matchMoveObj[i] ) == friendNo ){
				return wk->p_matchMoveObj[i];
			}
		}
	}
	return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	アイコンデータ読込み
 *
 *	@param	p_bgl		BGL
 *	@param	p_data		データ格納先
 *	@param	p_handle	ハンドル
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatchFriendListIconLoad( GF_BGL_INI* p_bgl, WIFIP2PMATCH_ICON* p_data, ARCHANDLE* p_handle, u32 heapID )
{
	// パレット転送
	ArcUtil_HDL_PalSet( p_handle, NARC_wifip2pmatch_wf_match_all_icon_NCLR,
			PALTYPE_MAIN_BG, PLAYER_DISP_ICON_PLTTOFS*32, 
			PLAYER_DISP_ICON_PLTTNUM*32, heapID );

	ArcUtil_HDL_PalSet( p_handle, NARC_wifip2pmatch_wf_match_all_icon_NCLR,
			PALTYPE_SUB_BG, PLAYER_DISP_ICON_PLTTOFS_SUB*32, 
			PLAYER_DISP_ICON_PLTTNUM*32, heapID );

	// キャラクタデータ転送
	ArcUtil_HDL_BgCharSet( p_handle, NARC_wifip2pmatch_wf_match_all_icon_NCGR,
			p_bgl, GF_BGL_FRAME1_M, PLAYER_DISP_ICON_CGX, 0, FALSE, heapID );

	ArcUtil_HDL_BgCharSet( p_handle, NARC_wifip2pmatch_wf_match_all_icon_NCGR,
			p_bgl, GF_BGL_FRAME2_S, PLAYER_DISP_ICON_CGX, 0, FALSE, heapID );

	// さらにキャラクタデータを保存しておく
	if( p_data->p_charbuff == NULL ){
		p_data->p_charbuff = ArcUtil_HDL_CharDataGet( p_handle, NARC_wifip2pmatch_wf_match_all_icon_NCGR, 
				FALSE, &p_data->p_char, heapID);
	}

	// スクリーンデータ読込み	
	// リトライ時にスクリーンデータがある場合は読み込まない
	if( p_data->p_buff == NULL ){	
		p_data->p_buff = ArcUtil_HDL_ScrnDataGet( p_handle, NARC_wifip2pmatch_wf_match_all_icon_NSCR, 
				FALSE, &p_data->p_scrn, heapID);
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	Iconデータはき
 *
 *	@param	p_data	ワーク
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatchFriendListIconRelease( WIFIP2PMATCH_ICON* p_data )
{
	if( p_data->p_buff != NULL ){
		sys_FreeMemoryEz( p_data->p_buff );
		p_data->p_buff = NULL;
	}
	if( p_data->p_charbuff != NULL ){
		sys_FreeMemoryEz( p_data->p_charbuff );
		p_data->p_charbuff = NULL;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	アイコンを書き込む
 *
 *	@param	p_bgl		BGL
 *	@param	p_data		データ
 *	@param	frm			フレーム
 *	@param	cx			キャラクタｘ座標
 *	@param	cy			キャラクタｙ座標
 *	@param	icon_type	アイコンタイプ
 *	@param	col			0=灰 1=赤
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatchFriendListIconWrite(  GF_BGL_INI* p_bgl, WIFIP2PMATCH_ICON* p_data, u32 frm, u8 cx, u8 cy, u32 icon_type, u32 col )
{
	int pal;
	// 書き込み
	GF_BGL_ScrWriteExpand( p_bgl, frm, cx, cy,
			PLAYER_DISP_ICON_SCRN_X, PLAYER_DISP_ICON_SCRN_Y,
			p_data->p_scrn->rawData, 
			PLAYER_DISP_ICON_SCRN_X*icon_type, 0,
			p_data->p_scrn->screenWidth/8, p_data->p_scrn->screenHeight/8 );

	
	// パレット
	pal = WifiP2PMatchBglFrmIconPalGet( frm );

	// パレットナンバーをあわせる
	GF_BGL_ScrPalChange( p_bgl, frm, cx, cy,
			PLAYER_DISP_ICON_SCRN_X, PLAYER_DISP_ICON_SCRN_Y, pal+col );

	// 転送
	GF_BGL_LoadScreenV_Req( p_bgl, frm );
}

//----------------------------------------------------------------------------
/**
 *	@brief	Icon書き込み
 *
 *	@param	p_bgl		BGL
 *	@param	p_data		データ
 *	@param	frm			フレーム
 *	@param	cx			キャラクタｘ座標
 *	@param	cy			キャラクタｙ座標
 *	@param	status		状態
 *	@param	vctIcon		ボイスチャットONOFFフラグ
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatchFriendListStIconWrite( GF_BGL_INI* p_bgl, WIFIP2PMATCH_ICON* p_data, u32 frm, u8 cx, u8 cy, u32 status )
{
	u8 col=0;
	u8 scrn_idx=0;

	switch( status ){
	  case WIFI_STATUS_VCT:      // VCT中
		  scrn_idx = PLAYER_DISP_ICON_IDX_VCT;
		  break;

	  case WIFI_STATUS_SBATTLE50_WAIT:   // シングルLv50対戦募集中
	  case WIFI_STATUS_SBATTLE100_WAIT:   // シングルLv100対戦募集中
	  case WIFI_STATUS_SBATTLE_FREE_WAIT:   // シングルLv100対戦募集中
	  case WIFI_STATUS_DBATTLE50_WAIT:   // ダブルLv50対戦募集中
	  case WIFI_STATUS_DBATTLE100_WAIT:   // ダブルLv100対戦募集中
	  case WIFI_STATUS_DBATTLE_FREE_WAIT:   // ダブルLv100対戦募集中
#ifdef WFP2P_DEBUG_EXON 
	  case WIFI_STATUS_MBATTLE_FREE_WAIT:// マルチバトル募集中
#endif
		  col = 1;
	  case WIFI_STATUS_SBATTLE50:      // シングル対戦中
	  case WIFI_STATUS_SBATTLE100:      // シングル対戦中
	  case WIFI_STATUS_SBATTLE_FREE:      // シングル対戦中
	  case WIFI_STATUS_DBATTLE50:      // ダブル対戦中
	  case WIFI_STATUS_DBATTLE100:      // ダブル対戦中
	  case WIFI_STATUS_DBATTLE_FREE:      // ダブル対戦中
#ifdef WFP2P_DEBUG_EXON 
	  case WIFI_STATUS_MBATTLE_FREE:     // マルチバトル中
#endif
		  scrn_idx = PLAYER_DISP_ICON_IDX_FIGHT;
		  break;


	  case WIFI_STATUS_TRADE_WAIT:    // 交換募集中
#ifdef WFP2P_DEBUG_EXON 
	  case WIFI_STATUS_BATTLEROOM_WAIT:// バトルルーム募集中
#endif
		  col = 1;
	  case WIFI_STATUS_TRADE:          // 交換中
#ifdef WFP2P_DEBUG_EXON 
	  case WIFI_STATUS_BATTLEROOM:     // バトルルーム中
#endif
		  scrn_idx = PLAYER_DISP_ICON_IDX_CHANGE;
		  break;


		// ミニゲーム
	  case WIFI_STATUS_BUCKET_WAIT:// バケット募集中
	  case WIFI_STATUS_BALANCEBALL_WAIT:// バケット募集中
	  case WIFI_STATUS_BALLOON_WAIT:// バケット募集中
		  col = 1;

	  case WIFI_STATUS_BUCKET:     // バケット中
	  case WIFI_STATUS_BALANCEBALL:     // バケット中
	  case WIFI_STATUS_BALLOON:     // バケット中
		  scrn_idx = PLAYER_DISP_ICON_IDX_MINIGAME;
		  break;
     

	  case WIFI_STATUS_POFIN_WAIT:    // ポフィン募集中
		  col = 1;
	  case WIFI_STATUS_POFIN:          // ポフィン料理中
		  scrn_idx = PLAYER_DISP_ICON_IDX_POFIN;
		  break;

	  case WIFI_STATUS_FRONTIER_WAIT:    // フロンティア募集中
		  col = 1;
	  case WIFI_STATUS_FRONTIER:          // フロンティア中
		  scrn_idx = PLAYER_DISP_ICON_IDX_FRONTIER;
		  break;

	  case WIFI_STATUS_LOGIN_WAIT:    // 待機中　ログイン直後はこれ
		  scrn_idx = PLAYER_DISP_ICON_IDX_NORMAL;
		  break;
	  
	  case WIFI_STATUS_NONE:		// 何も無い
		  scrn_idx = PLAYER_DISP_ICON_IDX_NONE;
		  break;
		  
	  case WIFI_STATUS_UNKNOWN:   // 新たに作ったらこの番号以上になる
	  case WIFI_STATUS_PLAY_OTHER:
		  scrn_idx = PLAYER_DISP_ICON_IDX_UNK;
		  break;
	  default:
		  scrn_idx = PLAYER_DISP_ICON_IDX_UNK;
		  break;
	}

	// 書き込み
	WifiP2PMatchFriendListIconWrite( p_bgl, p_data, frm, cx, cy, scrn_idx, col );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ビットマップにアイコンデータを書き込む
 *
 *	@param	p_bmp		ビットマップ
 *	@param	p_data		アイコンデータ
 *	@param	x			書き込みｘ座標
 *	@param	y			書き込みｙ座標
 *	@param	icon_type	アイコンタイプ
 *	@param	col			カラー
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatchFriendListBmpIconWrite(  GF_BGL_BMPWIN* p_bmp, WIFIP2PMATCH_ICON* p_data, u16 x, u16 y, u32 icon_type, u32 col )
{
	int pal;
	
	// 書き込み
	GF_BGL_BmpWinPrint( p_bmp,
			p_data->p_char->pRawData, 
			(PLAYER_DISP_ICON_SCRN_X*8)*icon_type, 0,
			p_data->p_char->W*8, p_data->p_char->H*8,
			x, y,
			PLAYER_DISP_ICON_SCRN_X*8, PLAYER_DISP_ICON_SCRN_Y*8
			);

	// パレットNo取得
	pal = WifiP2PMatchBglFrmIconPalGet( p_bmp->frmnum );

	// パレットナンバーをあわせる
	GF_BGL_BmpWinSet_Pal( p_bmp, pal+col );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ビットマップにアイコンデータを書き込む
 *
 *	@param	p_bmp		ビットマップ
 *	@param	p_data		アイコンデータ
 *	@param	x			書き込みｘ座標
 *	@param	y			書き込みｙ座標
 *	@param	status		状態
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatchFriendListBmpStIconWrite( GF_BGL_BMPWIN* p_bmp, WIFIP2PMATCH_ICON* p_data, u16 x, u16 y, u32 status )
{
	u8 col=0;
	u8 scrn_idx=0;

	switch( status ){
	  case WIFI_STATUS_VCT:      // VCT中
		  scrn_idx = PLAYER_DISP_ICON_IDX_VCT;
		  break;

	  case WIFI_STATUS_SBATTLE50_WAIT:   // シングルLv50対戦募集中
	  case WIFI_STATUS_SBATTLE100_WAIT:   // シングルLv100対戦募集中
	  case WIFI_STATUS_SBATTLE_FREE_WAIT:   // シングルLv100対戦募集中
	  case WIFI_STATUS_DBATTLE50_WAIT:   // ダブルLv50対戦募集中
	  case WIFI_STATUS_DBATTLE100_WAIT:   // ダブルLv100対戦募集中
	  case WIFI_STATUS_DBATTLE_FREE_WAIT:   // ダブルLv100対戦募集中
#ifdef WFP2P_DEBUG_EXON 
	  case WIFI_STATUS_MBATTLE_FREE_WAIT:// マルチバトル募集中
#endif
		  col = 1;
	  case WIFI_STATUS_SBATTLE50:      // シングル対戦中
	  case WIFI_STATUS_SBATTLE100:      // シングル対戦中
	  case WIFI_STATUS_SBATTLE_FREE:      // シングル対戦中
	  case WIFI_STATUS_DBATTLE50:      // ダブル対戦中
	  case WIFI_STATUS_DBATTLE100:      // ダブル対戦中
	  case WIFI_STATUS_DBATTLE_FREE:      // ダブル対戦中
#ifdef WFP2P_DEBUG_EXON 
	  case WIFI_STATUS_MBATTLE_FREE:     // マルチバトル中
#endif
		  scrn_idx = PLAYER_DISP_ICON_IDX_FIGHT;
		  break;


	  case WIFI_STATUS_TRADE_WAIT:    // 交換募集中
#ifdef WFP2P_DEBUG_EXON 
	  case WIFI_STATUS_BATTLEROOM_WAIT:// バトルルーム募集中
#endif
		  col = 1;
	  case WIFI_STATUS_TRADE:          // 交換中
#ifdef WFP2P_DEBUG_EXON 
	  case WIFI_STATUS_BATTLEROOM:     // バトルルーム中
#endif
		  scrn_idx = PLAYER_DISP_ICON_IDX_CHANGE;
		  break;


	  case WIFI_STATUS_BUCKET_WAIT:// バケット募集中
	  case WIFI_STATUS_BALANCEBALL_WAIT:// バケット募集中
	  case WIFI_STATUS_BALLOON_WAIT:// バケット募集中
		  col = 1;

	  case WIFI_STATUS_BUCKET:     // バケット中
	  case WIFI_STATUS_BALANCEBALL:     // バケット中
	  case WIFI_STATUS_BALLOON:     // バケット中
		  scrn_idx = PLAYER_DISP_ICON_IDX_MINIGAME;
		  break;
		  

	  case WIFI_STATUS_POFIN_WAIT:    // ポフィン募集中
		  col = 1;
	  case WIFI_STATUS_POFIN:          // ポフィン料理中
		  scrn_idx = PLAYER_DISP_ICON_IDX_POFIN;
		  break;

	  case WIFI_STATUS_FRONTIER_WAIT:    // フロンティア募集中
		  col = 1;
	  case WIFI_STATUS_FRONTIER:          // フロンティア中
		  scrn_idx = PLAYER_DISP_ICON_IDX_FRONTIER;
		  break;

	  case WIFI_STATUS_LOGIN_WAIT:    // 待機中　ログイン直後はこれ
		  scrn_idx = PLAYER_DISP_ICON_IDX_NORMAL;
		  break;
	  
	  case WIFI_STATUS_NONE:   // 何も無い
		  scrn_idx = PLAYER_DISP_ICON_IDX_NONE;
		  break;
		  
	  case WIFI_STATUS_UNKNOWN:   // 新たに作ったらこの番号以上になる
	  case WIFI_STATUS_PLAY_OTHER:   // 新たに作ったらこの番号以上になる
		  scrn_idx = PLAYER_DISP_ICON_IDX_UNK;
		  break;
	  default:
		  scrn_idx = PLAYER_DISP_ICON_IDX_UNK;
		  break;
	}
	
	WifiP2PMatchFriendListBmpIconWrite( p_bmp, p_data, x, y, scrn_idx, col );
}

//----------------------------------------------------------------------------
/**
 *	@brief	frmのアイコンパレットナンバーを返す
 *
 *	@param	frm		BGL　FRAME
 *
 *	@return パレットNO
 */
//-----------------------------------------------------------------------------
static int WifiP2PMatchBglFrmIconPalGet( u32 frm )
{
	if( frm < GF_BGL_FRAME0_S ){
		return PLAYER_DISP_ICON_PLTTOFS;
	}
	return PLAYER_DISP_ICON_PLTTOFS_SUB;
}

//----------------------------------------------------------------------------
/**
 *	@brief	フレンドリスト開始宣言
 */
//-----------------------------------------------------------------------------
static int WifiP2PMatchFriendListStart( void )
{
	WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK,
					COMM_BRIGHTNESS_SYNC, 1, HEAPID_WIFIP2PMATCH);

	// 部屋のBGM設定
	WifiP2P_SetLobbyBgm();	

	return WIFIP2PMATCH_FRIENDLIST_INIT;
}

#define TALK_MESSAGE_BUF_NUM	( 190*2 )
#define TITLE_MESSAGE_BUF_NUM	( 90*2 )

//------------------------------------------------------------------
/**
 * ワーク初期化
 *
 * @param   wk		WIFIP2PMATCH_WORK*
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void InitWork( WIFIP2PMATCH_WORK *wk )
{
    int i;
    int flag;

#if 0
    for(i=0;i<WIFIP2PMATCH_MEMBER_MAX;i++){
        wk->TrainerName[i] = STRBUF_Create( PERSON_NAME_SIZE+EOM_SIZE, HEAPID_WIFIP2PMATCH );
        STRBUF_SetStringCode(wk->TrainerName[i] ,WifiList_GetFriendNamePtr(wk->pList,i));
    }
#endif
    wk->TalkString =  STRBUF_Create( TALK_MESSAGE_BUF_NUM, HEAPID_WIFIP2PMATCH );
    wk->pTemp = STRBUF_Create( TALK_MESSAGE_BUF_NUM, HEAPID_WIFIP2PMATCH );        // 入力登録時の一時バッファ


#if 0
    // 文字列バッファ作成
    for(i = 0;i < 4;i++){
        wk->MenuString[i]  = STRBUF_Create( TALK_MESSAGE_BUF_NUM, HEAPID_WIFIP2PMATCH );
    }
#endif
    wk->pExpStrBuf = STRBUF_Create( TALK_MESSAGE_BUF_NUM, HEAPID_WIFIP2PMATCH );
    wk->TitleString = STRBUF_Create( TITLE_MESSAGE_BUF_NUM, HEAPID_WIFIP2PMATCH );

    wk->seq = WIFIP2PMATCH_MODE_INIT;

}

//------------------------------------------------------------------
/**
 * $brief   ワーク解放
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void FreeWork( WIFIP2PMATCH_WORK *wk )
{
    int i;


#if 0
    for(i=0;i<WIFIP2PMATCH_MEMBER_MAX;i++){
        STRBUF_Delete( wk->TrainerName[i] );
    }
#endif
    STRBUF_Delete( wk->TitleString );
    STRBUF_Delete( wk->TalkString );
    STRBUF_Delete( wk->pTemp );

#if 0
    for(i = 0;i < 4;i++){
        STRBUF_Delete(wk->MenuString[i]);
    }
#endif
    STRBUF_Delete(wk->pExpStrBuf);

}

//--------------------------------------------------------------------------------------------
/**
 * BG解放
 *
 * @param	ini		BGLデータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BgExit( GF_BGL_INI * ini )
{

    GF_BGL_BGControlExit( ini, GF_BGL_FRAME3_S );
    GF_BGL_BGControlExit( ini, GF_BGL_FRAME2_S );
    GF_BGL_BGControlExit( ini, GF_BGL_FRAME1_S );
    GF_BGL_BGControlExit( ini, GF_BGL_FRAME0_S );
    GF_BGL_BGControlExit( ini, GF_BGL_FRAME3_M );
    GF_BGL_BGControlExit( ini, GF_BGL_FRAME2_M );
    GF_BGL_BGControlExit( ini, GF_BGL_FRAME1_M );
    GF_BGL_BGControlExit( ini, GF_BGL_FRAME0_M );

    sys_FreeMemoryEz( ini );
}


//--------------------------------------------------------------------------------------------
/**
 * グラフィックデータセット
 *
 * @param	wk		ポケモンリスト画面のワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------


static void BgGraphicSet( WIFIP2PMATCH_WORK * wk, ARCHANDLE* p_handle )
{
    GF_BGL_INI *bgl = wk->bgl;

    GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
    GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
    GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
    GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
    GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );

    // 上下画面ＢＧパレット転送
    ArcUtil_HDL_PalSet(    p_handle, NARC_wifip2pmatch_conect_NCLR, PALTYPE_MAIN_BG, 0, 0,  HEAPID_WIFIP2PMATCH);
    ArcUtil_HDL_PalSet(    p_handle, NARC_wifip2pmatch_conect_NCLR, PALTYPE_SUB_BG,  0, 0,  HEAPID_WIFIP2PMATCH);

    // 会話フォントパレット転送
    TalkFontPaletteLoad( PALTYPE_MAIN_BG, _NUKI_FONT_PALNO*0x20, HEAPID_WIFIP2PMATCH );
    TalkFontPaletteLoad( PALTYPE_MAIN_BG, COMM_MESFONT_PAL*0x20, HEAPID_WIFIP2PMATCH );
    //	TalkFontPaletteLoad( PALTYPE_SUB_BG,  13*0x20, HEAPID_WIFIP2PMATCH );


    // メイン画面BG2キャラ転送
    ArcUtil_HDL_BgCharSet( p_handle, NARC_wifip2pmatch_conect_NCGR, bgl,
                       GF_BGL_FRAME0_M, 0, 0, 0, HEAPID_WIFIP2PMATCH);

    // メイン画面BG2スクリーン転送
    ArcUtil_HDL_ScrnSet(   p_handle, NARC_wifip2pmatch_conect_01_NSCR, bgl,
                       GF_BGL_FRAME0_M, 0, 0, 0, HEAPID_WIFIP2PMATCH);


    // サブ画面BG0キャラ転送
    ArcUtil_HDL_BgCharSet( p_handle, NARC_wifip2pmatch_conect_sub_NCGR, bgl, GF_BGL_FRAME0_S, 0, 0, 0, HEAPID_WIFIP2PMATCH);

    // サブ画面BG0スクリーン転送
    ArcUtil_HDL_ScrnSet(   p_handle, NARC_wifip2pmatch_conect_sub_NSCR, bgl, GF_BGL_FRAME0_S, 0, 0, 0, HEAPID_WIFIP2PMATCH);

    {
        int type = CONFIG_GetWindowType(SaveData_GetConfig(wk->pSaveData));
        TalkWinGraphicSet(
            bgl, GF_BGL_FRAME2_M, COMM_TALK_WIN_CGX_NUM, COMM_MESFRAME_PAL,  type, HEAPID_WIFIP2PMATCH );
        MenuWinGraphicSet(
            bgl, GF_BGL_FRAME2_M, MENU_WIN_CGX_NUM, MENU_WIN_PAL, 0, HEAPID_WIFIP2PMATCH );
        MenuWinGraphicSet(
            bgl, GF_BGL_FRAME1_M, MENU_WIN_CGX_NUM, MENU_WIN_PAL, 0, HEAPID_WIFIP2PMATCH );
    }
    
}


//** CharManager PlttManager用 **//
#define WIFIP2PMATCH_CHAR_CONT_NUM				(20)
#define WIFIP2PMATCH_CHAR_VRAMTRANS_MAIN_SIZE		(128*1024)
#define WIFIP2PMATCH_CHAR_VRAMTRANS_SUB_SIZE		(16*1024)
#define WIFIP2PMATCH_PLTT_CONT_NUM				(20)

//-------------------------------------
//
//	キャラクタマネージャー
//	パレットマネージャーの初期化
//
//=====================================
static void char_pltt_manager_init(void)
{
    // キャラクタマネージャー初期化
    {
        CHAR_MANAGER_MAKE cm = {
            WIFIP2PMATCH_CHAR_CONT_NUM,
            WIFIP2PMATCH_CHAR_VRAMTRANS_MAIN_SIZE,
            WIFIP2PMATCH_CHAR_VRAMTRANS_SUB_SIZE,
            HEAPID_WIFIP2PMATCH
        };
        InitCharManagerReg(&cm, GX_OBJVRAMMODE_CHAR_1D_128K, GX_OBJVRAMMODE_CHAR_1D_32K );
    }
    // パレットマネージャー初期化
    InitPlttManager(WIFIP2PMATCH_PLTT_CONT_NUM, HEAPID_WIFIP2PMATCH);

    // 読み込み開始位置を初期化
    CharLoadStartAll();
    PlttLoadStartAll();
	//通信アイコン用にキャラ＆パレット制限
	CLACT_U_WmIcon_SetReserveAreaCharManager(NNS_G2D_VRAM_TYPE_2DMAIN, GX_OBJVRAMMODE_CHAR_1D_128K);
	CLACT_U_WmIcon_SetReserveAreaPlttManager(NNS_G2D_VRAM_TYPE_2DMAIN);
}


//------------------------------------------------------------------
/**
 * セルアクター初期化
 *
 * @param   wk		レーダー構造体のポインタ
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void InitCellActor(WIFIP2PMATCH_WORK *wk, ARCHANDLE* p_handle)
{
    int i;


    // OAMマネージャーの初期化
    NNS_G2dInitOamManagerModule();

    // 共有OAMマネージャ作成
    // レンダラ用OAMマネージャ作成
    // ここで作成したOAMマネージャをみんなで共有する
    REND_OAMInit(
        0, 126,		// メイン画面OAM管理領域
        0, 32,		// メイン画面アフィン管理領域
        0, 126,		// サブ画面OAM管理領域
        0, 32,		// サブ画面アフィン管理領域
        HEAPID_WIFIP2PMATCH);


    // セルアクター初期化
    wk->clactSet = CLACT_U_SetEasyInit( WF_CLACT_WKNUM, &wk->renddata, HEAPID_WIFIP2PMATCH );

    CLACT_U_SetSubSurfaceMatrix( &wk->renddata, 0, NAMEIN_SUB_ACTOR_DISTANCE );


    //リソースマネージャー初期化
    for(i=0;i<CLACT_RESOURCE_NUM;i++){		//リソースマネージャー作成
        wk->resMan[i] = CLACT_U_ResManagerInit(WF_CLACT_RESNUM, i, HEAPID_WIFIP2PMATCH);
    }

	// FontOAMシステム作成
	wk->fontoam = FONTOAM_SysInit( WF_FONTOAM_NUM, HEAPID_WIFIP2PMATCH );

	
#if 0
    //---------上画面用-------------------

    //chara読み込み
    wk->resObjTbl[MAIN_LCD][CLACT_U_CHAR_RES] =
        CLACT_U_ResManagerResAddArcChar_ArcHandle(wk->resMan[CLACT_U_CHAR_RES],
                                        p_handle,
                                        NARC_wifip2pmatch_conect_obj_NCGR, FALSE, 0, NNS_G2D_VRAM_TYPE_2DMAIN, HEAPID_WIFIP2PMATCH);

    //pal読み込み
    wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES] =
        CLACT_U_ResManagerResAddArcPltt_ArcHandle(wk->resMan[CLACT_U_PLTT_RES],
                                        p_handle,
                                        NARC_wifip2pmatch_conect_obj_NCLR, 0, 0, NNS_G2D_VRAM_TYPE_2DMAIN, 7, HEAPID_WIFIP2PMATCH);

    //cell読み込み
    wk->resObjTbl[MAIN_LCD][CLACT_U_CELL_RES] =
        CLACT_U_ResManagerResAddArcKindCell_ArcHandle(wk->resMan[CLACT_U_CELL_RES],
                                            p_handle,
                                            NARC_wifip2pmatch_conect_NCER, FALSE, 0, CLACT_U_CELL_RES,HEAPID_WIFIP2PMATCH);

    //同じ関数でanim読み込み
    wk->resObjTbl[MAIN_LCD][CLACT_U_CELLANM_RES] =
        CLACT_U_ResManagerResAddArcKindCell_ArcHandle(wk->resMan[CLACT_U_CELLANM_RES],
                                            p_handle,
                                            NARC_wifip2pmatch_conect_NANR, FALSE, 0, CLACT_U_CELLANM_RES,HEAPID_WIFIP2PMATCH);


    // リソースマネージャーから転送

    // Chara転送
    CLACT_U_CharManagerSetAreaCont( wk->resObjTbl[MAIN_LCD][CLACT_U_CHAR_RES] );
    //	CLACT_U_CharManagerSet( wk->resObjTbl[SUB_LCD][CLACT_U_CHAR_RES] );

    // パレット転送
    CLACT_U_PlttManagerSetCleanArea( wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES] );
    //	CLACT_U_PlttManagerSet( wk->resObjTbl[SUB_LCD][CLACT_U_PLTT_RES] );

#endif

}

#define TRAINER_NAME_POS_X		( 24 )
#define TRAINER_NAME_POS_Y		( 32 )
#define TRAINER_NAME_POS_SPAN	( 32 )

#define TRAINER_NAME_WIN_X		(  3 )
#define TRAINER1_NAME_WIN_Y		(  6 )
#define TRAINER2_NAME_WIN_Y		(  7 )

//------------------------------------------------------------------
/**
 * セルアクター登録
 *
 * @param   wk			WIFIP2PMATCH_WORK*
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void SetCellActor(WIFIP2PMATCH_WORK *wk)
{
#if 0
    int i;
    // セルアクターヘッダ作成
    CLACT_U_MakeHeader(&wk->clActHeader_m, 0, 0, 0, 0, CLACT_U_HEADER_DATA_NONE, CLACT_U_HEADER_DATA_NONE,
                       0, 0,
                       wk->resMan[CLACT_U_CHAR_RES],
                       wk->resMan[CLACT_U_PLTT_RES],
                       wk->resMan[CLACT_U_CELL_RES],
                       wk->resMan[CLACT_U_CELLANM_RES],
                       NULL,NULL);

    /*
	CLACT_U_MakeHeader(&wk->clActHeader_s, 1, 1, 1, 1, CLACT_U_HEADER_DATA_NONE, CLACT_U_HEADER_DATA_NONE,
	0, 0,
	wk->resMan[CLACT_U_CHAR_RES],
	wk->resMan[CLACT_U_PLTT_RES],
	wk->resMan[CLACT_U_CELL_RES],
	wk->resMan[CLACT_U_CELLANM_RES],
	NULL,NULL);
     */
    {
        //登録情報格納
        CLACT_ADD add;

        add.ClActSet	= wk->clactSet;
        add.ClActHeader	= &wk->clActHeader_m;

        add.mat.z		= 0;
        add.sca.x		= FX32_ONE;
        add.sca.y		= FX32_ONE;
        add.sca.z		= FX32_ONE;
        add.rot			= 0;
        add.pri			= 1;
        add.DrawArea	= NNS_G2D_VRAM_TYPE_2DMAIN;
        add.heap		= HEAPID_WIFIP2PMATCH;

        //セルアクター表示開始
        // サブ画面用(矢印の登録）
        for(i=0;i < _OAM_NUM;i++){
            add.mat.x = FX32_ONE *   TRAINER_NAME_POS_X;
            add.mat.y = FX32_ONE * ( TRAINER_NAME_POS_Y + TRAINER_NAME_POS_SPAN*i ) + NAMEIN_SUB_ACTOR_DISTANCE;
            wk->MainActWork[i] = CLACT_Add(&add);
            CLACT_SetAnmFlag(wk->MainActWork[i],1);
            CLACT_AnmChg( wk->MainActWork[i], i );
            CLACT_BGPriorityChg(wk->MainActWork[i], 2);
            CLACT_SetDrawFlag( wk->MainActWork[i], 0 );
        }

    }
#endif
    GF_Disp_GX_VisibleControl(  GX_PLANEMASK_OBJ, VISIBLE_ON );	//メイン画面OBJ面ＯＮ
    //	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );	//サブ画面OBJ面ＯＮ
}


//------------------------------------------------------------------
/**
 * BMPWIN処理（文字パネルにフォント描画）
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------

// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ALL) imatake 2007/01/17
// ウィンドウのパラメタを定数に置き換え、ウィンドウ位置を1つ右へ
#define TITLE_TEXT_X	(  5 )
#define TITLE_TEXT_Y	(  1 )
#define TITLE_TEXT_SX	( 22 )
#define TITLE_TEXT_SY	(  2 )
// ----------------------------------------------------------------------------

static void MainMenuMsgInit(WIFIP2PMATCH_WORK *wk)
{
	//　タイトルはいらなくなりました
#if 1	
	
    int i,col;

    if(GF_BGL_BmpWinAddCheck(&wk->MyInfoWinBack)){
        BmpMenuWinClear(&wk->MyInfoWinBack, WINDOW_TRANS_ON );
        GF_BGL_BmpWinDel(&wk->MyInfoWinBack);
    }
    // ----------------------------------------------------------------------------
    // localize_spec_mark(LANG_ALL) imatake 2007/01/17
    // ウィンドウのパラメタを定数に置き換え、ウィンドウ位置を1つ右へ
    GF_BGL_BmpWinAdd(wk->bgl, &wk->MyInfoWinBack, GF_BGL_FRAME3_M,
                     TITLE_TEXT_X, TITLE_TEXT_Y, TITLE_TEXT_SX, TITLE_TEXT_SY,
                     _NUKI_FONT_PALNO,  _CGX_BOTTOM - 18*3 );
    // ----------------------------------------------------------------------------
    GF_BGL_BmpWinDataFill( &wk->MyInfoWinBack, 0x8000 );
    MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_018, wk->TitleString );
    // ----------------------------------------------------------------------------
    // localize_spec_mark(LANG_ALL) imatake 2007/01/17
    // 「Ｗｉ?Ｆｉコネクションに　せつぞく」を中央寄せ
    {
        u32 xofs = FontProc_GetPrintCenteredPositionX(FONT_TALK, wk->TitleString, 0, TITLE_TEXT_SX * 8);
        GF_STR_PrintColor( &wk->MyInfoWinBack, FONT_TALK, wk->TitleString, xofs, 0, MSG_NO_PUT,_COL_N_WHITE, NULL);
    }
    // ----------------------------------------------------------------------------
    GF_BGL_BmpWinOnVReq(&wk->MyInfoWinBack);
#endif
    // MatchComment: TitleWin renamed to MyInfoWinBack?
}


// はい・いいえBMP（下画面）
#define YESNO_WIN_FRAME_CHAR	( 1 + TALK_WIN_CGX_SIZ )
#define YESNO_CHARA_OFFSET		( 1 + TALK_WIN_CGX_SIZ + MENU_WIN_CGX_SIZ + FLD_MSG_WIN_SX*FLD_MSG_WIN_SY )
#define YESNO_WINDOW_X			( 22 )
#define YESNO_WINDOW_Y1			(  7 )
#define YESNO_WINDOW_Y2			( 13 )
#define YESNO_CHARA_W			(  8 )
#define YESNO_CHARA_H			(  4 )

static const BMPWIN_DAT TouchYesNoBmpDat[2]={
    {
        GF_BGL_FRAME0_M, YESNO_WINDOW_X, YESNO_WINDOW_Y1,
        YESNO_CHARA_W, YESNO_CHARA_H, 13, YESNO_CHARA_OFFSET
        },
    {
        GF_BGL_FRAME0_M, YESNO_WINDOW_X, YESNO_WINDOW_Y2,
        YESNO_CHARA_W, YESNO_CHARA_H, 13, YESNO_CHARA_OFFSET+YESNO_CHARA_W*YESNO_CHARA_H

        }
};

//------------------------------------------------------------------
/**
 * $brief   確保したBMPWINを解放
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void BmpWinDelete( WIFIP2PMATCH_WORK *wk )
{
    int i;

	// メッセージ停止
	EndMessageWindowOff( wk );

    if(GF_BGL_BmpWinAddCheck(&wk->MyInfoWinBack)){
        GF_BGL_BmpWinDel(&wk->MyInfoWinBack);
    }
    if(GF_BGL_BmpWinAddCheck(&wk->MsgWin)){
        GF_BGL_BmpWinDel(&wk->MsgWin);
    }
    if(GF_BGL_BmpWinAddCheck(&wk->MyWin)){
        GF_BGL_BmpWinDel(&wk->MyWin);
    }
    if(GF_BGL_BmpWinAddCheck(&wk->MyInfoWin)){
        GF_BGL_BmpWinDel(&wk->MyInfoWin);
    }
    if(GF_BGL_BmpWinAddCheck(&wk->ListWin)){
        GF_BGL_BmpWinDel(&wk->ListWin);
    }
    if(wk->lw){
        BmpListExit(wk->lw, NULL, NULL);
    }
    if(wk->menulist){
        BMP_MENULIST_Delete( wk->menulist );
        wk->menulist = NULL;
    }
    
    if(GF_BGL_BmpWinAddCheck(&wk->SysMsgWin)){
        GF_BGL_BmpWinDel(&wk->SysMsgWin);
    }
}


//------------------------------------------------------------------
/**
 * $brief   開始時のメッセージ   WIFIP2PMATCH_MODE_INIT
 * @param   wk
 * @param   seq
 * @retval  none
 */
//------------------------------------------------------------------

static const BMPWIN_DAT _yesNoBmpDatSys = {
    GF_BGL_FRAME2_M, FLD_YESNO_WIN_PX, FLD_YESNO_WIN_PY,
    FLD_YESNO_WIN_SX, FLD_YESNO_WIN_SY, FLD_YESNO_WIN_PAL, YESNO_WIN_CGX
    };
static const BMPWIN_DAT _yesNoBmpDatSys2 = {
    GF_BGL_FRAME1_M, FLD_YESNO_WIN_PX, FLD_YESNO_WIN_PY+6,
    FLD_YESNO_WIN_SX, FLD_YESNO_WIN_SY, FLD_YESNO_WIN_PAL, FRAME1_YESNO_WIN_CGX
    };

static int WifiP2PMatch_MainInit( WIFIP2PMATCH_WORK *wk, int seq )
{
	u32 status;
	
    OHNO_PRINT("サウンド書き換え\n");
    if(CommStateIsWifiConnect()){       // 接続中

//        WirelessIconEasy();  // 接続中なのでアイコン表示
		
        _friendNameExpand(wk, mydwc_getFriendIndex());
        wk->pMatch = CommStateGetMatchWork();
        wk->pMatch->myMatchStatus.vchat = wk->pMatch->myMatchStatus.vchat_org;

		// status取得
		status = _WifiMyStatusGet( wk, &wk->pMatch->myMatchStatus );

        if( (status == WIFI_STATUS_TRADE) ||
			(status == WIFI_STATUS_POFIN) ||
			(status == WIFI_STATUS_FRONTIER) ||
#ifdef WFP2P_DEBUG_EXON 
			(status == WIFI_STATUS_BATTLEROOM) ||
#endif
			(status == WIFI_STATUS_BUCKET) ||
			(status == WIFI_STATUS_BUCKET_WAIT) ||
			(status == WIFI_STATUS_BALANCEBALL) ||
			(status == WIFI_STATUS_BALANCEBALL_WAIT) ||
			(status == WIFI_STATUS_BALLOON) ||
			(status == WIFI_STATUS_BALLOON_WAIT) ||
			(status == WIFI_STATUS_POFIN_WAIT)
			){	// 交換中orポフィンなら

#if 0		//080611	tomoya 拡張ヒープがメモリマップの一番上になるように作成
			// フロンティアから戻ってきても、拡張ヒープを作成
			if( (status == WIFI_STATUS_FRONTIER) ||
				(status == WIFI_STATUS_TRADE) ){
				// バトルから戻ってきたなら拡張ヒープ作成
				sys_CreateHeap( HEAPID_BASE_APP, HEAPID_WIFIP2PMATCHEX, WIFI_P2PMATCH_EXHEAP_SIZE );
			}
#endif


			// 080605 tomoya BTS:249の対処のため
			// 親の通信リセット処理後直ぐに接続してしまうのを回避
			// ステータスをLOGIN_WAITに変更
			_myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);
			
            CommInfoFinalize();
            CommStateWifiTradeMatchEnd();	// マッチングを切る
            wk->pMatch = CommStateGetMatchWork();
            wk->pMatch->myMatchStatus.vchat = wk->pMatch->myMatchStatus.vchat_org;
            wk->timer = _RECONECTING_WAIT_TIME;
            wk->seq = WIFIP2PMATCH_RECONECTING_WAIT;

        }
        else{
			// バトルから戻ってきたとき
            WifiP2PMatchMessagePrint(wk, msg_wifilobby_044, FALSE);

			// えらーチェック再開
		    CommStateSetErrorCheck(TRUE,TRUE);	
            CommTimingSyncStart(_TIMING_BATTLE_END);
            wk->seq = WIFIP2PMATCH_NEXTBATTLE_YESNO;

#if 0		//080611	tomoya 拡張ヒープがメモリマップの一番上になるように作成
			// バトルから戻ってきたなら拡張ヒープ作成
			sys_CreateHeap( HEAPID_BASE_APP, HEAPID_WIFIP2PMATCHEX, WIFI_P2PMATCH_EXHEAP_SIZE );
#endif
        }
        /*        CommStateWifiMatchEnd();
        wk->pMatch = CommStateGetMatchWork();
        wk->seq = WIFIP2PMATCH_FRIENDLIST_INIT;   */
    }
    else{        // 接続がまだ

#if 0	//	080611	tomoya 拡張ヒープがメモリマップの一番上になるように作成
		// 通信前なら拡張ヒープ作成
		sys_CreateHeap( HEAPID_BASE_APP, HEAPID_WIFIP2PMATCHEX, WIFI_P2PMATCH_EXHEAP_SIZE );
#endif

		//08.07.03
		Snd_DataSetByScene( SND_SCENE_P2P, SEQ_WIFILOBBY, 1 );

        MainMenuMsgInit(wk);// BMPWIN登録・描画
        if( !DWC_CheckHasProfile( WifiList_GetMyUserInfo(wk->pList) ) ){
            GF_BGL_PrioritySet(GF_BGL_FRAME0_M , 3);  //はいけい
            GF_BGL_PrioritySet(GF_BGL_FRAME1_M , 0);   // yesno win
            GF_BGL_PrioritySet(GF_BGL_FRAME3_M , 0);  // メッセージ
            GF_BGL_PrioritySet(GF_BGL_FRAME2_M , 1);   // menulist
            _systemMessagePrint(wk, dwc_message_0003);
            wk->pYesNoWork =
                BmpYesNoSelectInit( wk->bgl,
                                    &_yesNoBmpDatSys2,
                                    MENU_WIN_CGX_NUM, MENU_WIN_PAL,
                                    HEAPID_WIFIP2PMATCH );
            wk->seq = WIFIP2PMATCH_FIRST_YESNO;
        }
        else if( !DWC_CheckValidConsole(WifiList_GetMyUserInfo(wk->pList)) ){
            _systemMessagePrint(wk, dwc_message_0005);
            wk->seq = WIFIP2PMATCH_DIFFER_MACHINE_INIT;
        }
        else{
            WifiP2PMatchMessagePrint(wk, dwc_message_0002, TRUE);
            wk->seq = WIFIP2PMATCH_NORMALCONNECT_YESNO;
        }
    }
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   通常の接続  WIFIP2PMATCH_NORMALCONNECT_YESNO
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------
static int _normalConnectYesNo( WIFIP2PMATCH_WORK *wk, int seq )
{
    if( GF_MSG_PrintEndCheck( wk->MsgIndex ) == 0 ){
        wk->pYesNoWork =
            BmpYesNoSelectInit( wk->bgl,
                                &_yesNoBmpDatSys,
                                MENU_WIN_CGX_NUM, MENU_WIN_PAL,
                                HEAPID_WIFIP2PMATCH );
        wk->seq = WIFIP2PMATCH_NORMALCONNECT_WAIT;
    }
    return seq;
}


//------------------------------------------------------------------
/**
 * $brief   通常の接続  WIFIP2PMATCH_NORMALCONNECT_WAIT
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------
static int _normalConnectWait( WIFIP2PMATCH_WORK *wk, int seq )
{
    int i;
    int ret = BmpYesNoSelectMain(wk->pYesNoWork, HEAPID_WIFIP2PMATCH);
    if(ret == BMPMENU_NULL){  // まだ選択中
        return seq;
    }
    else{
        //BmpTalkWinClear( &wk->SysMsgWin, WINDOW_TRANS_ON );
        if(ret == 0){ // はいを選択した場合
            //接続開始
            wk->pMatch = CommStateWifiEnterLogin(wk->pSaveData,sizeof(TEST_MATCH_WORK));

            WifiP2PMatchMessagePrint(wk, dwc_message_0008, TRUE);
			GF_ASSERT( wk->timeWaitWork == NULL );
            wk->timeWaitWork = TimeWaitIconAdd( &wk->MsgWin, COMM_TALK_WIN_CGX_NUM );
            wk->seq = WIFIP2PMATCH_CONNECTING_INIT;
        }
        else{  // いいえを選択した場合
            wk->seq = WIFIP2PMATCH_MODE_END_WAIT;
        }
    }
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   別のDSだった場合  WIFIP2PMATCH_DIFFER_MACHINE_INIT
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _differMachineInit( WIFIP2PMATCH_WORK *wk, int seq )
{
    if(sys.trg & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)){
        _systemMessagePrint(wk,dwc_message_0006);
        GF_BGL_PrioritySet(GF_BGL_FRAME3_M , 0);  // メッセージ
        GF_BGL_PrioritySet(GF_BGL_FRAME2_M , 1);   //
        GF_BGL_PrioritySet(GF_BGL_FRAME1_M , 0);   // yesno win
        GF_BGL_PrioritySet(GF_BGL_FRAME0_M , 3);  //はいけい
        wk->pYesNoWork =
				BmpYesNoSelectInitEx( wk->bgl,
                                &_yesNoBmpDatSys2,
                                MENU_WIN_CGX_NUM, MENU_WIN_PAL,
                                1, HEAPID_WIFIP2PMATCH );	// いいえ　から始まる
/*            BmpYesNoSelectInit( wk->bgl,
                                &_yesNoBmpDatSys2,
                                MENU_WIN_CGX_NUM, MENU_WIN_PAL,
                                HEAPID_WIFIP2PMATCH );//*/
        wk->seq = WIFIP2PMATCH_DIFFER_MACHINE_NEXT;
    }

    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   別のDSだった場合  WIFIP2PMATCH_DIFFER_MACHINE_NEXT
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _differMachineNext( WIFIP2PMATCH_WORK *wk, int seq )
{
    int i;
    int ret = BmpYesNoSelectMain(wk->pYesNoWork, HEAPID_WIFIP2PMATCH);
    if(ret == BMPMENU_NULL){  // まだ選択中
        return seq;
    }
    else{
        BmpTalkWinClear( &wk->SysMsgWin, WINDOW_TRANS_ON );
        GF_BGL_BmpWinDel( &wk->SysMsgWin );
        if(ret == 0){ // はいを選択した場合
            _systemMessagePrint(wk,dwc_message_0007);
            wk->pYesNoWork =
                BmpYesNoSelectInitEx( wk->bgl,
                                    &_yesNoBmpDatSys2,
                                    MENU_WIN_CGX_NUM, MENU_WIN_PAL,
                                    1, HEAPID_WIFIP2PMATCH );	// いいえ　から始める
/*
                BmpYesNoSelectInit( wk->bgl,
                                    &_yesNoBmpDatSys2,
                                    MENU_WIN_CGX_NUM, MENU_WIN_PAL,
                                    HEAPID_WIFIP2PMATCH );//*/
            wk->seq = WIFIP2PMATCH_DIFFER_MACHINE_ONEMORE;
        }
        else{  // いいえを選択した場合
            wk->seq = WIFIP2PMATCH_MODE_END_WAIT;
        }
    }
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   別のDSだった場合  WIFIP2PMATCH_DIFFER_MACHINE_ONEMORE
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _differMachineOneMore( WIFIP2PMATCH_WORK *wk, int seq )
{
    int i;
    int ret = BmpYesNoSelectMain(wk->pYesNoWork, HEAPID_WIFIP2PMATCH);
    if(ret == BMPMENU_NULL){  // まだ選択中
        return seq;
    }
    else{

		// 優先順位を戻す
        GF_BGL_PrioritySet(GF_BGL_FRAME3_M , 0);  // メッセージ
        GF_BGL_PrioritySet(GF_BGL_FRAME2_M , 0);   //
        GF_BGL_PrioritySet(GF_BGL_FRAME1_M , 1);   // yesno win
        GF_BGL_PrioritySet(GF_BGL_FRAME0_M , 3);  //はいけい
		
        BmpTalkWinClear( &wk->SysMsgWin, WINDOW_TRANS_ON );
        GF_BGL_BmpWinDel( &wk->SysMsgWin );
        if(ret == 0){ // はいを選択した場合
            //接続開始
            WifiList_Init(wk->pList);
            EMAILSAVE_Init(SaveData_Get(wk->pSaveData, GMDATA_ID_EMAIL));
            FrontierRecord_DataCrear(SaveData_GetFrontier(wk->pSaveData));

            wk->seq = WIFIP2PMATCH_CONNECTING_INIT;
            wk->pMatch = CommStateWifiEnterLogin(wk->pSaveData,sizeof(TEST_MATCH_WORK));
            WifiP2PMatchMessagePrint(wk, dwc_message_0008, TRUE);
			GF_ASSERT( wk->timeWaitWork == NULL );
            wk->timeWaitWork = TimeWaitIconAdd( &wk->MsgWin, COMM_TALK_WIN_CGX_NUM );
            wk->bInitMessage = TRUE;
        }
        else{  // いいえを選択した場合
            wk->seq = WIFIP2PMATCH_MODE_END_WAIT;
        }
    }
    return seq;
}

//WIFIP2PMATCH_FIRST_YESNO
//------------------------------------------------------------------
/**
 * $brief   初回接続時の確認  WIFIP2PMATCH_FIRST_YESNO
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _firstYesNo( WIFIP2PMATCH_WORK *wk, int seq )
{
    int i;
    int ret = BmpYesNoSelectMain(wk->pYesNoWork, HEAPID_WIFIP2PMATCH);
    if(ret == BMPMENU_NULL){  // まだ選択中
        return seq;
    }
    else{

		// 表示優先順位を戻す
		GF_BGL_PrioritySet(GF_BGL_FRAME0_M , 3);  //はいけい
		GF_BGL_PrioritySet(GF_BGL_FRAME1_M , 1);   // yesno win
		GF_BGL_PrioritySet(GF_BGL_FRAME3_M , 0);  // メッセージ
		GF_BGL_PrioritySet(GF_BGL_FRAME2_M , 0);   // menulist
		
        BmpTalkWinClear( &wk->SysMsgWin, WINDOW_TRANS_ON );
        GF_BGL_BmpWinDel( &wk->SysMsgWin );
        if(ret == 0){ // はいを選択した場合
            //接続開始
            wk->seq = WIFIP2PMATCH_CONNECTING_INIT;
            wk->pMatch = CommStateWifiEnterLogin(wk->pSaveData,sizeof(TEST_MATCH_WORK));
            WifiP2PMatchMessagePrint(wk, dwc_message_0008, TRUE);
			GF_ASSERT( wk->timeWaitWork == NULL );
            wk->timeWaitWork = TimeWaitIconAdd( &wk->MsgWin, COMM_TALK_WIN_CGX_NUM );
            wk->bInitMessage = TRUE;
        }
        else{  // いいえを選択した場合
            wk->seq = WIFIP2PMATCH_MODE_END_WAIT;
        }
    }

    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   ログイン接続中  WIFIP2PMATCH_CONNECTING_INIT
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------
static int WifiP2PMatch_ConnectingInit( WIFIP2PMATCH_WORK *wk, int seq )
{
    GF_BGL_PrioritySet(GF_BGL_FRAME0_M , 3);  //はいけい
    GF_BGL_PrioritySet(GF_BGL_FRAME1_M , 1);   // yesno win
    GF_BGL_PrioritySet(GF_BGL_FRAME3_M , 0);  // メッセージ
    GF_BGL_PrioritySet(GF_BGL_FRAME2_M , 0);   //
    wk->seq = WIFIP2PMATCH_CONNECTING;
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   MyMatchStatus作成
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------
static void _makeMyMatchStatus(WIFIP2PMATCH_WORK* wk, int status)
{
    MYSTATUS* pMyStatus = SaveData_GetMyStatus(wk->pSaveData);
    ZUKAN_WORK* pZukan = SaveData_GetZukanWork(wk->pSaveData);
    POKEPARTY* pPokeParty = SaveData_GetTemotiPokemon(wk->pSaveData);
    WIFI_HISTORY* pHistry = SaveData_GetWifiHistory(wk->pSaveData);
    int max,i;

    OHNO_PRINT("_WIFI_STATUS_MAX_SIZE  %d\n",sizeof(_WIFI_MACH_STATUS));
    GF_ASSERT(_WIFI_STATUS_MAX_SIZE == sizeof(_WIFI_MACH_STATUS));

    max = PokeParty_GetPokeCount(pPokeParty);
    for(i = 0;i < max;i++){
        POKEMON_PARAM* poke = PokeParty_GetMemberPointer( pPokeParty, i);
        wk->pMatch->myMatchStatus.pokemonType[i] = PokeParaGet( poke, ID_PARA_monsno, NULL );
        wk->pMatch->myMatchStatus.hasItemType[i] = PokeParaGet( poke, ID_PARA_item, NULL );
    }
    wk->pMatch->myMatchStatus.version = MyStatus_GetRomCode(pMyStatus);
    wk->pMatch->myMatchStatus.regionCode = MyStatus_GetRegionCode(pMyStatus);
    wk->pMatch->myMatchStatus.pokemonZukan = ZukanWork_GetZenkokuZukanFlag(pZukan);
//    wk->pMatch->myMatchStatus.status = status;
    wk->pMatch->myMatchStatus.status = WIFI_STATUS_UNKNOWN;
    _myStatusChange_not_send(wk, status);	// BGM状態などを調整
    wk->pMatch->myMatchStatus.regulation = _REGULATION_BATTLE_TOWER;
    wk->pMatch->myMatchStatus.trainer_view = MyStatus_GetTrainerView(pMyStatus);
    wk->pMatch->myMatchStatus.sex = MyStatus_GetMySex(pMyStatus);
    wk->pMatch->myMatchStatus.nation = WIFIHISTORY_GetMyNation(pHistry);
    wk->pMatch->myMatchStatus.area = WIFIHISTORY_GetMyArea(pHistry);
    wk->pMatch->myMatchStatus.vchat = TRUE;
    wk->pMatch->myMatchStatus.vchat_org = TRUE;

    mydwc_setMyInfo((const char *)&wk->pMatch->myMatchStatus, _WIFI_STATUS_MAX_SIZE);
    mydwc_setFriendStateBuffer(&wk->pMatch->friendMatchStatus[0], _WIFI_STATUS_MAX_SIZE);

}

//------------------------------------------------------------------
/**
 * $brief   フレンドのstatus読み込み
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _readFriendMatchStatus(WIFIP2PMATCH_WORK* wk)
{
    int i,num = 0;
	_WIFI_MACH_STATUS* p_status;
	u32 status;

//    MI_CpuFill8(wk->index2No, 0, sizeof(WIFIP2PMATCH_MEMBER_MAX));
    for(i = 0;i < WIFIP2PMATCH_MEMBER_MAX;i++){
        wk->index2No[i] = 0;

		// その人は自分の友達か？
		if( wk->friend_num > i ){
			// このチェックを少し進化させる
			// 最新情報を受信して初めて登録される
			if(DWC_STATUS_OFFLINE != WifiDwc_getFriendStatus(i)){
				p_status = WifiFriendMatchStatusGet( wk, i );

				status = _WifiMyStatusGet( wk, p_status );
	//			if( (status != WIFI_STATUS_UNKNOWN) && (p_status->trainer_view != HERO) ){
				if( (status != WIFI_STATUS_NONE) && 
					(status != WIFI_STATUS_PLAY_OTHER) &&
					(p_status->trainer_view != HERO) ){
					wk->index2No[num] = i+1;
					num++;
				}else{

					if( (status == WIFI_STATUS_PLAY_OTHER) ){
						// もしその人のボタンが下に出ていたら消す
						// BTS:通信393の対処
						if( MCVSys_BttnTypeGet( wk, i+1 ) != MCV_BTTN_FRIEND_TYPE_NONE ){
							MCVSys_BttnDel( wk, i+1 );
						}
					}
				}
			}
		}
    }
    return num;
}

//------------------------------------------------------------------
/**
 * $brief   フレンドのstatus検査　こちらの状態と変わったら変わった数を返す
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _checkUserDataMatchStatus(WIFIP2PMATCH_WORK* wk)
{
    int i,num = 0;
	BOOL back_up_wait;
	BOOL now_wait;
	MCR_MOVEOBJ* p_obj;
	_WIFI_MACH_STATUS* p_status;
	u32 status;

    for(i = 0;i < WIFIP2PMATCH_MEMBER_MAX;i++){

		p_status = WifiFriendMatchStatusGet( wk, i );
		status = _WifiMyStatusGet( wk, p_status );
        if((wk->matchStatusBackup[i]  != status) ||
			(wk->matchVchatBackup[i]  != p_status->vchat) ){

			// オブジェクトワーク
			p_obj = MCRSYS_GetMoveObjWork( wk, i+1 );

			// そのオブジェが登録され、新しい状態が描画オブジェに反映されるまでは
			// バックアップステータスを更新しない
			if( p_obj ){

				// ステータスが待ち状態になったらジャンプさせる
				// 待ち状態から普通に戻ったら通常に戻す
				back_up_wait = _modeWait( wk->matchStatusBackup[i] );
				now_wait = _modeWait( status );
				if( (back_up_wait == TRUE) && (now_wait == FALSE) ){
					//NPCの通常動作に戻す
					WIFI_MCR_NpcMoveSet( &wk->matchroom, p_obj,  MCR_NPC_MOVE_NORMAL );
				}else if( (back_up_wait == FALSE) && (now_wait == TRUE) ){
					
					//NPCをジャンプ動作にする
					WIFI_MCR_NpcMoveSet( &wk->matchroom, p_obj,  MCR_NPC_MOVE_JUMP );
				}

				wk->matchStatusBackup[i] = status;
				wk->matchVchatBackup[i] = p_status->vchat;
				// 通信取得分を入れる
				OHNO_PRINT("通信取得分を入れる  %d %d\n", i, p_status->trainer_view);
				WifiList_SetFriendInfo(wk->pList, i,
									   WIFILIST_FRIEND_UNION_GRA,
									   p_status->trainer_view);
				WifiList_SetFriendInfo(wk->pList, i,
									   WIFILIST_FRIEND_SEX,
									   p_status->sex);
				num++;
			}
/*            { バトル中に一括で処理してあるので、ここからは削除
                WIFI_HISTORY* pHistry = SaveData_GetWifiHistory(wk->pSaveData);
                int nation = wk->pMatch->friendMatchStatus[i].nation;
                int area = wk->pMatch->friendMatchStatus[i].area;
                WIFIHIST_STAT stat = WIFIHISTORY_GetStat(pHistry, nation, area);
                if((WIFIHIST_STAT_NODATA == stat) || (WIFIHIST_STAT_NEW == stat)){
                    WIFIHISTORY_SetStat(pHistry, nation, area, stat);
                }
            } */
        }
    }
    return num;
}

//------------------------------------------------------------------
/**
 * $brief   エラー表示
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static void _errorDisp(WIFIP2PMATCH_WORK* wk)
{
    COMMSTATE_DWCERROR* pErr = CommStateGetWifiError();
    int msgno,no = pErr->errorCode;
    int type =  mydwc_errorType(pErr->errorCode, pErr->errorType);

    if((type == 11) || (no == ERRORCODE_0)){
        msgno = dwc_error_0015;
        type = 11;
    }
    else if(no == ERRORCODE_HEAP){
        msgno = dwc_error_0014;
        type = 12;
    }
    else{
		if( type >= 0 ){
	        msgno = dwc_error_0001 + type;
		}else{
	        msgno = dwc_error_0012;
		}
    }
    OHNO_PRINT("エラーメッセージ %d \n",msgno);
    EndMessageWindowOff(wk);
    WORDSET_RegisterNumber(wk->WordSet, 0, no,
                           5, NUMBER_DISPTYPE_ZERO, NUMBER_CODETYPE_DEFAULT);

	// エラーメッセージを表示しておくシンク数
    wk->timer = 30;

	// 上画面下の自分情報文字列をクリアする
    _systemMessagePrint(wk, msgno);

    switch(type){
      case 1:
      case 4:
      case 5:
      case 11:
        wk->seq = WIFIP2PMATCH_RETRY_INIT;  // 再接続かフィールドか
        break;
      case 6:
      case 7:
      case 8:
      case 9:
        wk->seq = WIFIP2PMATCH_RETRY_INIT;//WIFIP2PMATCH_POWEROFF_INIT;  // 電源を切るかフィールド
        break;
      case 10:
        wk->seq = WIFIP2PMATCH_RETRY_INIT;  // メニュー一覧へ
        break;
      case 0:
      case 2:
      case 3:
      default:
        wk->seq = WIFIP2PMATCH_MODE_CHECK_AND_END;  // フィールド
        break;
    }
}

//------------------------------------------------------------------
/**
 * $brief   電源を切ってWIFI設定へ  WIFIP2PMATCH_POWEROFF_INIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _poweroffInit( WIFIP2PMATCH_WORK *wk, int seq )
{
// WIFI設定に行かないように

    if(sys.trg & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)){
        if(GF_BGL_BmpWinAddCheck(&wk->SysMsgWin)){
            BmpTalkWinClear(&wk->SysMsgWin, WINDOW_TRANS_ON );
            GF_BGL_BmpWinDel( &wk->SysMsgWin );
        }
//        WifiP2PMatchMessagePrint(wk, msg_wifilobby_051, FALSE);
        wk->seq = WIFIP2PMATCH_POWEROFF_YESNO;
    }
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   電源を切ってWIFI設定へ  WIFIP2PMATCH_POWEROFF_YESNO
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _poweroffYesNo( WIFIP2PMATCH_WORK *wk, int seq )
{
// WIFI設定に行かないように
/*    if( GF_MSG_PrintEndCheck( wk->MsgIndex ) == 0 ){
        // はいいいえウインドウを出す
        wk->pYesNoWork =
            BmpYesNoSelectInit( wk->bgl,
                                &_yesNoBmpDat,
                                MENU_WIN_CGX_NUM, MENU_WIN_PAL,
                                HEAPID_WIFIP2PMATCH );
        wk->seq = WIFIP2PMATCH_POWEROFF_WAIT;
    }*/
    wk->seq = WIFIP2PMATCH_POWEROFF_WAIT;
    return seq;
}


//------------------------------------------------------------------
/**
 * $brief   電源を切ってWIFI設定へ  WIFIP2PMATCH_POWEROFF_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _poweroffWait( WIFIP2PMATCH_WORK *wk, int seq )
{
// WIFI設定に行かないように
    /*
    int i;
    int ret = BmpYesNoSelectMain(wk->pYesNoWork, HEAPID_WIFIP2PMATCH);
    if(ret == BMPMENU_NULL){  // まだ選択中
        return seq;
    }else if(ret == 0){ // はいを選択した場合
        CommStateWifiLogout();
        wk->endSeq = WIFI_P2PMATCH_UTIL;   //ソフトウエアリセット
        wk->seq = WIFIP2PMATCH_MODE_END_WAIT;
        EndMessageWindowOff(wk);
    }
    else{  // いいえを選択した場合
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_052, FALSE);
        wk->seq = WIFIP2PMATCH_RETRY_YESNO;
    }
*/
    wk->seq = WIFIP2PMATCH_RETRY_YESNO;
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   接続をリトライする  WIFIP2PMATCH_RETRY_INIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _retryInit( WIFIP2PMATCH_WORK *wk, int seq )
{
	if( wk->timer > 0 ){
		wk->timer --;
		return seq;
	}
	
    if(sys.trg & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)){
        if(GF_BGL_BmpWinAddCheck(&wk->SysMsgWin)){
            BmpTalkWinClear(&wk->SysMsgWin, WINDOW_TRANS_ON );
            GF_BGL_BmpWinDel( &wk->SysMsgWin );
        }
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_052, FALSE);
        wk->seq = WIFIP2PMATCH_RETRY_YESNO;
    }
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   接続をリトライする  WIFIP2PMATCH_RETRY_YESNO
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _retryYesNo( WIFIP2PMATCH_WORK *wk, int seq )
{
    if( GF_MSG_PrintEndCheck( wk->MsgIndex ) == 0 ){
        // はいいいえウインドウを出す
        wk->pYesNoWork =
            BmpYesNoSelectInit( wk->bgl,
                                &_yesNoBmpDat,
                                MENU_WIN_CGX_NUM, MENU_WIN_PAL,
                                HEAPID_WIFIP2PMATCH );
        wk->seq = WIFIP2PMATCH_RETRY_WAIT;
    }
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   接続をリトライする  WIFIP2PMATCH_RETRY_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _retryWait( WIFIP2PMATCH_WORK *wk, int seq )
{
    int i;
    int ret = BmpYesNoSelectMain(wk->pYesNoWork, HEAPID_WIFIP2PMATCH);
    if(ret == BMPMENU_NULL){  // まだ選択中
        return seq;
    }else if(ret == 0){ // はいを選択した場合
        CommStateWifiLogout();
        _myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);
        wk->seq = WIFIP2PMATCH_RETRY;
    }
    else{  // いいえを選択した場合
        _myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);	// VCHATの状態を元に戻すため
        CommStateWifiLogout();
        CommInfoFinalize();   //Info初期化
        wk->endSeq = WIFI_P2PMATCH_END;
        wk->seq = WIFIP2PMATCH_MODE_END_WAIT;
    }
    EndMessageWindowOff(wk);
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   接続をリトライ  WIFIP2PMATCH_RETRAY
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _retry( WIFIP2PMATCH_WORK *wk, int seq )
{
    int i;
	ARCHANDLE* p_handle;
    
    if(!CommIsVRAMDInitialize()){
		
		if( WIFI_MCR_GetInitFlag( &wk->matchroom ) == TRUE ){

			// ブラックアウト
			WIPE_SetBrightness( WIPE_DISP_MAIN, WIPE_FADE_BLACK );
			WIPE_SetBrightness( WIPE_DISP_SUB, WIPE_FADE_BLACK );

			WIFI_MCR_Dest( &wk->matchroom );
			for( i=0; i<MATCHROOM_IN_OBJNUM; i++ ){
				wk->p_matchMoveObj[i] = NULL;
			}

			// OAMバッファクリーン
			REND_OAM_UtilOamRamClear_Main( HEAPID_WIFIP2PMATCH );
			REND_OAM_UtilOamRamClear_Sub( HEAPID_WIFIP2PMATCH );
		}
		// ビューアー破棄
		if( MCVSys_MoveCheck( wk ) == TRUE ){
			MCVSys_Exit( wk );

		}
		
		// ビットマップOFF
		// ユーザーデータ
		if(GF_BGL_BmpWinAddCheck(&wk->MyInfoWinBack)){
			BmpMenuWinClear( &wk->MyInfoWinBack, WINDOW_TRANS_ON );
			GF_BGL_BmpWinDel(&wk->MyInfoWinBack);
		}
		if(GF_BGL_BmpWinAddCheck(&wk->MyInfoWin)){
			GF_BGL_BmpWinDel(&wk->MyInfoWin);
		}
		
        //接続開始
        GF_BGL_ScrClear(wk->bgl, GF_BGL_FRAME3_M);

		// グラフィックリセット
		p_handle = ArchiveDataHandleOpen( ARC_WIFIP2PMATCH_GRA, HEAPID_WIFIP2PMATCH );
		BgGraphicSet( wk, p_handle );
		ArchiveDataHandleClose( p_handle );


		// パレットフェード再開
		ConnectBGPalAnm_OccSet(&wk->cbp, TRUE);

		// ブライトネス状態リセっト
		// MCR画面破棄時に暗くした物を解除
		wk->brightness_reset = TRUE;
//		WIPE_ResetBrightness( WIPE_DISP_MAIN );
//		WIPE_ResetBrightness( WIPE_DISP_SUB );

        if( wk->menulist){
            GF_BGL_BmpWinDel(&wk->ListWin);
            BmpListExit(wk->lw, NULL, NULL);
            wk->lw=NULL;
            BMP_MENULIST_Delete( wk->menulist );
            wk->menulist=NULL;
        }

//        wk->friendMatchReadCount;
        MI_CpuClear8(wk->index2No, WIFIP2PMATCH_MEMBER_MAX);
        MI_CpuClear8(wk->index2NoBackUp, WIFIP2PMATCH_MEMBER_MAX);
        MI_CpuClear8(wk->matchStatusBackup, WIFIP2PMATCH_MEMBER_MAX*sizeof(int));
        MI_CpuClear8(wk->matchVchatBackup, WIFIP2PMATCH_MEMBER_MAX*sizeof(int));

#if 0
		for(i=0;i < _OAM_NUM;i++){
            CLACT_SetDrawFlag( wk->MainActWork[i], 0 );
        }
#endif
        MainMenuMsgInit(wk);
        wk->pMatch = CommStateWifiEnterLogin(wk->pSaveData,sizeof(TEST_MATCH_WORK));
        WifiP2PMatchMessagePrint(wk, dwc_message_0008, TRUE);
		GF_ASSERT( wk->timeWaitWork == NULL );
        wk->timeWaitWork = TimeWaitIconAdd( &wk->MsgWin, COMM_TALK_WIN_CGX_NUM );
        wk->seq = WIFIP2PMATCH_CONNECTING_INIT;
    }
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   ログイン接続中  WIFIP2PMATCH_CONNECTING
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------
#ifdef NONEQUIVALENT
static int WifiP2PMatch_Connecting( WIFIP2PMATCH_WORK *wk, int seq )
{
    int i;

    // ----------------------------------------------------------------------------
    // localize_spec_mark(LANG_ALL) imatake 2007/02/15
    // メッセージ表示中に接続が完了した場合に対処
    // MatchComment: actually, the below if statement that was removed from DP JP -> US was readded in platinum
    // ----------------------------------------------------------------------------

    if( GF_MSG_PrintEndCheck( wk->MsgIndex ) != 0 ){
        return seq;
    }

    if( mydwc_getSaving()) {
        wk->seq = WIFIP2PMATCH_FIRST_SAVING;
        WifiP2PMatchMessagePrint(wk, dwc_message_0015, TRUE);
		GF_ASSERT( wk->timeWaitWork == NULL );
        wk->timeWaitWork = TimeWaitIconAdd( &wk->MsgWin, COMM_TALK_WIN_CGX_NUM );
    }
    
    if(CommStateIsWifiLoginState()){
        OHNO_PRINT("WIFI接続完了\n");
        // ----------------------------------------------------------------------------
        // localize_spec_mark(LANG_ALL) imatake 2007/02/15
        // メッセージ表示中に接続が完了した場合に対処
        if(wk->MsgIndex != _PRINTTASK_MAX){
            if(GF_MSG_PrintEndCheck(wk->MsgIndex)!=0){
                GF_STR_PrintForceStop(wk->MsgIndex);
                wk->MsgIndex = _PRINTTASK_MAX;
            }
        }
        // ----------------------------------------------------------------------------
        if( wk->bInitMessage ){  // 初回接続時にはセーブシーケンスへ
//            SaveData_SaveParts(wk->pSaveData, SVBLK_ID_NORMAL);  //セーブ中
            wk->seq = WIFIP2PMATCH_FIRST_ENDMSG;
        }
        else{
            _makeMyMatchStatus(wk, WIFI_STATUS_LOGIN_WAIT);
            _readFriendMatchStatus( wk );
            wk->seq = WifiP2PMatchFriendListStart();
        }
    }
    else if(CommStateIsWifiError() || (CommWifiIsMatched() == 3)){
        _errorDisp(wk);
    }
    return seq;
}
#else
asm static int WifiP2PMatch_Connecting( WIFIP2PMATCH_WORK *wk, int seq )
{
	push {r3, r4, r5, lr}
	add r5, r0, #0
	mov r0, #6
	lsl r0, r0, #6
	ldr r0, [r5, r0]
	add r4, r1, #0
	lsl r0, r0, #0x18
	lsr r0, r0, #0x18
	bl GF_MSG_PrintEndCheck
	cmp r0, #0
	beq _0222FABC
	add r0, r4, #0
	pop {r3, r4, r5, pc}
_0222FABC:
	bl mydwc_getSaving
	cmp r0, #0
	beq _0222FAF6
	mov r0, #0xd1
	mov r1, #0x45
	lsl r0, r0, #2
	str r1, [r5, r0]
	add r0, r5, #0
	mov r1, #0x1e
	mov r2, #1
	bl WifiP2PMatchMessagePrint
	mov r0, #0x62
	lsl r0, r0, #2
	ldr r0, [r5, r0]
	cmp r0, #0
	beq _0222FAE4
	bl GF_AssertFailedWarningCall
_0222FAE4:
	mov r1, #0xb3
	lsl r1, r1, #2
	add r0, r5, r1
	sub r1, #0xea
	bl TimeWaitIconAdd
	mov r1, #0x62
	lsl r1, r1, #2
	str r0, [r5, r1]
_0222FAF6:
	bl CommStateIsWifiLoginState
	cmp r0, #0
	beq _0222FB2A
	mov r0, #0xd9
	lsl r0, r0, #2
	ldr r1, [r5, r0]
	cmp r1, #0
	beq _0222FB10
	mov r1, #0x10
	sub r0, #0x20
	str r1, [r5, r0]
	b _0222FB40
_0222FB10:
	add r0, r5, #0
	mov r1, #0x10
	bl _makeMyMatchStatus
	add r0, r5, #0
	bl _readFriendMatchStatus
	bl WifiP2PMatchFriendListStart
	mov r1, #0xd1
	lsl r1, r1, #2
	str r0, [r5, r1]
	b _0222FB40
_0222FB2A:
	bl CommStateIsWifiError
	cmp r0, #0
	bne _0222FB3A
	bl CommWifiIsMatched
	cmp r0, #3
	bne _0222FB40
_0222FB3A:
	add r0, r5, #0
	bl _errorDisp
_0222FB40:
	add r0, r4, #0
	pop {r3, r4, r5, pc}
}
#endif

//------------------------------------------------------------------
/**
 * $brief   初回セーブ処理
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------
static int WifiP2PMatch_FirstSaving( WIFIP2PMATCH_WORK *wk, int seq )

{
    if( GF_MSG_PrintEndCheck( wk->MsgIndex ) != 0 ){
        return seq;
    }
    if( mydwc_getSaving()) {
        ///      SaveData_SaveParts(wk->pSaveData, SVBLK_ID_NORMAL);  //セーブ中
        SaveData_DivSave_Init(wk->pSaveData, SVBLK_ID_NORMAL); ///セーブ開始
        wk->seq = WIFIP2PMATCH_FIRST_SAVING2;
        return seq;
    }
    if(CommStateIsWifiLoginState()){
        OHNO_PRINT("WIFI接続完了\n");
        if( wk->bInitMessage ){
            wk->seq = WIFIP2PMATCH_FIRST_ENDMSG;
        }
        else{
            _makeMyMatchStatus(wk, WIFI_STATUS_LOGIN_WAIT);
            _readFriendMatchStatus( wk );
            wk->seq = WifiP2PMatchFriendListStart();
        }
    }
    else if(CommStateIsWifiError() || (CommWifiIsMatched() == 3)){
        _errorDisp(wk);
    }
    return seq;
}



//------------------------------------------------------------------
/**
 * $brief   初回セーブ処理  WIFIP2PMATCH_FIRST_SAVING2
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------
static int WifiP2PMatch_FirstSaving2( WIFIP2PMATCH_WORK *wk, int seq )
{
    if(mydwc_getSaving()){
        SAVE_RESULT result = SaveData_DivSave_Main(wk->pSaveData);
        if (result != SAVE_RESULT_CONTINUE && result != SAVE_RESULT_LAST) {
            mydwc_resetSaving();
        }
        else{
            return seq;
        }
    }
    if(CommStateIsWifiLoginState()){
        OHNO_PRINT("WIFI接続完了\n");
        if( wk->bInitMessage ){ 
            wk->seq = WIFIP2PMATCH_FIRST_ENDMSG;
        }
        else{
            _makeMyMatchStatus(wk, WIFI_STATUS_LOGIN_WAIT);
            _readFriendMatchStatus( wk );
            wk->seq = WifiP2PMatchFriendListStart();
        }
    }
    else if(CommStateIsWifiError() || (CommWifiIsMatched() == 3)){
        _errorDisp(wk);
    }
    return seq;
}



//------------------------------------------------------------------
/**
 * $brief   切断＋VCT待ちに移行するまで待つ  WIFIP2PMATCH_RECONECTING_WAIT
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------
static int WifiP2PMatch_ReConnectingWait( WIFIP2PMATCH_WORK *wk, int seq )
{
    int i;

    if(wk->timer > 0){
        wk->timer--;
        return seq;
    }

    OHNO_PRINT("CommStateIsWifiLoginState %d  \n",CommStateIsWifiLoginState());
    OHNO_PRINT("CommWifiIsMatched %d  \n",CommWifiIsMatched());
    OHNO_PRINT("CommStateIsWifiDisconnect %d  \n",CommStateIsWifiDisconnect());
    OHNO_PRINT("CommIsConnect %d  \n",CommIsConnect(COMM_PARENT_ID));

    if(CommStateIsWifiLoginMatchState()){
        OHNO_PRINT("WIFI接続完了\n");
        wk->seq = WifiP2PMatchFriendListStart();
    }
    else if(CommStateIsWifiError() || (CommWifiIsMatched() == 3)){
        _errorDisp(wk);
    }
    else if((CommWifiIsMatched() >= 4) || CommStateIsWifiDisconnect() || !CommIsConnect(COMM_PARENT_ID)){
        CommStateWifiMatchEnd();

        wk->seq = WifiP2PMatchFriendListStart();
    }
    return seq;
}




//------------------------------------------------------------------
/**
 * $brief   最初接続完了  WIFIP2PMATCH_FIRST_ENDMSG
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _firstConnectEndMsg( WIFIP2PMATCH_WORK *wk, int seq )
{
    EndMessageWindowOff(wk);
    _systemMessagePrint(wk, dwc_message_0004);
    wk->seq = WIFIP2PMATCH_FIRST_ENDMSG_WAIT;
#if AFTER_MASTER_070410_WIFIAPP_N22_EUR_FIX
    wk->bInitMessage = FALSE;
#endif //AFTER_MASTER_070410_WIFIAPP_N22_EUR_FIX
    return seq;

}

//WIFIP2PMATCH_FIRST_ENDMSG_WAIT
static int _firstConnectEndMsgWait( WIFIP2PMATCH_WORK *wk, int seq )
{
    if(sys.trg & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)){
        if(wk->initSeq == WIFI_P2PMATCH_DPW){ // 世界対戦なら
            wk->endSeq = WIFI_P2PMATCH_DPW_END;   // 終了処理へ
            wk->seq = WIFIP2PMATCH_MODE_END_WAIT;
            CommStateWifiLogout();
        }
        else if( 0==WifiList_GetFriendDataNum(wk->pList) ){  //フレンドがいない
            WifiP2PMatchMessagePrint(wk, dwc_message_0011, TRUE);
            wk->seq = WIFIP2PMATCH_MODE_EXITING;
            wk->timer = 1;
        }
        else{
            _makeMyMatchStatus(wk, WIFI_STATUS_LOGIN_WAIT);
            _readFriendMatchStatus( wk );
            wk->seq = WifiP2PMatchFriendListStart();
            if(GF_BGL_BmpWinAddCheck(&wk->SysMsgWin)){
                BmpTalkWinClear(&wk->SysMsgWin, WINDOW_TRANS_ON );
                GF_BGL_BmpWinDel( &wk->SysMsgWin );
            }
        }
    }
    return seq;

}

//------------------------------------------------------------------
/**
 * $brief   自分の状態を表示
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static void	_userDataInfoDisp(WIFIP2PMATCH_WORK * wk)
{
    int col,sex, vct_icon;
	int msg_id;
    MYSTATUS* pMy = SaveData_GetMyStatus(wk->pSaveData);
    STRBUF* pSTRBUF = MyStatus_CreateNameString(pMy, HEAPID_WIFIP2PMATCH);
	u32 status;

    if(!GF_BGL_BmpWinAddCheck(&wk->MyInfoWin)){
        STRBUF_Delete(pSTRBUF);
        return;
    }

	// 初期化
    GF_BGL_BmpWinDataFill( &wk->MyInfoWin, 0 );

	sex = MyStatus_GetMySex(pMy);

	if( sex == PM_MALE ){
		col = _COL_N_BLUE;
	}
	else{
		col = _COL_N_RED;
	}
    GF_STR_PrintColor( &wk->MyInfoWin, FONT_SYSTEM,
                       pSTRBUF, WIFIP2PMATCH_PLAYER_DISP_NAME_X, 0,
                       MSG_NO_PUT, col, NULL);
    STRBUF_Delete(pSTRBUF);

	status = _WifiMyStatusGet( wk, &wk->pMatch->myMatchStatus );

	// msg_idとカラーを取得
	msg_id = MCVSys_StatusMsgIdGet( status, &col );
    if( status == WIFI_STATUS_VCT ){      // VCT中
        if(mydwc_getFriendIndex() == -1){
            MSGMAN_GetString(wk->MsgManager, msg_id, wk->pExpStrBuf);
        }
        else{
            sex = WifiList_GetFriendInfo(wk->pList, mydwc_getFriendIndex(), WIFILIST_FRIEND_SEX);
            if( PM_MALE == sex){
                col = _COL_N_BLUE;
            }
            else if( PM_FEMALE == sex){
                col = _COL_N_RED;
            }
            STRBUF_SetStringCode(wk->pExpStrBuf ,WifiList_GetFriendNamePtr(wk->pList,mydwc_getFriendIndex()));
        }
	}else{
		MSGMAN_GetString(wk->MsgManager, msg_id, wk->pExpStrBuf);
	}
	GF_STR_PrintColor( &wk->MyInfoWin, FONT_SYSTEM, wk->pExpStrBuf,
					   WIFIP2PMATCH_PLAYER_DISP_ST_X, 0,
					   MSG_NO_PUT, col, NULL);

	//状態アイコン設定
	// Frame1の対応する位置にアイコンを転送
	WifiP2PMatchFriendListStIconWrite( wk->bgl, &wk->icon, GF_BGL_FRAME1_M, 
			PLAYER_DISP_ICON_POS_X, PLAYER_DISP_ICON_POS_Y,
			status );
	if( wk->pMatch->myMatchStatus.vchat ){
		vct_icon = PLAYER_DISP_ICON_IDX_NONE;
	}else{
		vct_icon = PLAYER_DISP_ICON_IDX_VCTNOT;
	}

	WifiP2PMatchFriendListIconWrite( wk->bgl, &wk->icon, GF_BGL_FRAME1_M, 
			PLAYER_DISP_VCTICON_POS_X, PLAYER_DISP_VCTICON_POS_Y,
			vct_icon, 0 );
	
    GF_BGL_BmpWinOnVReq(&wk->MyInfoWin);
}

//------------------------------------------------------------------
/**
 * $brief   一ラインコールバック
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static void	_lineCallBack(BMPLIST_WORK * lw, u32 param, u8 y)
{
    WIFIP2PMATCH_WORK* wk = (WIFIP2PMATCH_WORK*)BmpListParamGet(lw,BMPLIST_ID_WORK);
    int gmmNo = 0,col = _COL_N_BLACK;
	_WIFI_MACH_STATUS* p_status;
	u32 status;

    if((param == WIFIP2PMATCH_MEMBER_MAX) || (param == BMPLIST_CANCEL)){
        return;
    }

	p_status = WifiFriendMatchStatusGet( wk, param );
	status = _WifiMyStatusGet( wk,  p_status );
    switch(status){
      case WIFI_STATUS_VCT:      // VCT中
        gmmNo = msg_wifilobby_027;
        col = _COL_N_GRAY;
        break;
      case WIFI_STATUS_SBATTLE50:      // 対戦中
      case WIFI_STATUS_SBATTLE100:      // 対戦中
      case WIFI_STATUS_SBATTLE_FREE:      // 対戦中
      case WIFI_STATUS_DBATTLE50:      // 対戦中
      case WIFI_STATUS_DBATTLE100:      // 対戦中
      case WIFI_STATUS_DBATTLE_FREE:      // 対戦中
#ifdef WFP2P_DEBUG_EXON 
      case WIFI_STATUS_MBATTLE_FREE:      // 対戦中
#endif
        gmmNo = msg_wifilobby_024;
        col = _COL_N_GRAY;
        break;
      case WIFI_STATUS_SBATTLE50_WAIT:   // 対戦募集中
        gmmNo = msg_wifilobby_060;
        break;
      case WIFI_STATUS_SBATTLE100_WAIT:   // 対戦募集中
        gmmNo = msg_wifilobby_061;
        break;
      case WIFI_STATUS_SBATTLE_FREE_WAIT:   // 対戦募集中
        gmmNo = msg_wifilobby_059;
        break;
      case WIFI_STATUS_DBATTLE50_WAIT:   // 対戦募集中
        gmmNo = msg_wifilobby_063;
        break;
      case WIFI_STATUS_DBATTLE100_WAIT:   // 対戦募集中
        gmmNo = msg_wifilobby_064;
        break;
      case WIFI_STATUS_DBATTLE_FREE_WAIT:   // 対戦募集中
        gmmNo = msg_wifilobby_062;
        break;
#ifdef WFP2P_DEBUG_EXON 
      case WIFI_STATUS_MBATTLE_FREE_WAIT:   // 対戦募集中
        gmmNo = msg_wifilobby_062;
        break;
#endif
      case WIFI_STATUS_TRADE:          // 交換中
        gmmNo = msg_wifilobby_026;
        col = _COL_N_GRAY;
        break;
      case WIFI_STATUS_TRADE_WAIT:    // 交換募集中
        gmmNo = msg_wifilobby_025;
        break;
      case WIFI_STATUS_POFIN_WAIT:    // ポフィン募集中
        gmmNo = msg_wifilobby_100;
        break;
      case WIFI_STATUS_FRONTIER_WAIT:    // フロンティア募集中
        gmmNo = msg_wifilobby_138;
        break;
#ifdef WFP2P_DEBUG_EXON 
      case WIFI_STATUS_BATTLEROOM_WAIT:    // バトルルーム募集中
        gmmNo = msg_wifilobby_debug_00;
        break;
#endif
      case WIFI_STATUS_BUCKET_WAIT:    // バケット募集中
      case WIFI_STATUS_BALANCEBALL_WAIT:    // バケット募集中
      case WIFI_STATUS_BALLOON_WAIT:    // バケット募集中
        gmmNo = msg_wifilobby_147;
        break;
      case WIFI_STATUS_LOGIN_WAIT:    // 待機中　ログイン直後はこれ
        gmmNo = msg_wifilobby_046;
        break;
      case WIFI_STATUS_UNKNOWN:
      default:
        gmmNo = msg_wifilobby_056;
        break;
    }
    OHNO_PRINT("文字コールバック呼びだし %d %d %d\n",param,y,gmmNo);

    if(gmmNo != 0){
        MSGMAN_GetString(wk->MsgManager, gmmNo, wk->pExpStrBuf);

        {
            int length = FontProc_GetPrintStrWidth( FONT_SYSTEM, wk->pExpStrBuf, 0 );
            int x      = 164 - length;

            GF_STR_PrintColor( &wk->ListWin, FONT_SYSTEM, wk->pExpStrBuf,
                               x, y, MSG_NO_PUT, col, NULL);
            GF_BGL_BmpWinOnVReq(&wk->ListWin);
        }
    }

    {
        int sex = WifiList_GetFriendInfo(wk->pList, param, WIFILIST_FRIEND_SEX);
        col = _COL_N_BLACK;
        if( PM_MALE == sex){
            col = _COL_N_BLUE;
        }
        else if( PM_FEMALE == sex){
            col = _COL_N_RED;
        }

//        GF_STR_PrintColor( &wk->ListWin, FONT_SYSTEM, wk->TrainerName[param],
  //                         8, y, MSG_NO_PUT, col, NULL);
        GF_BGL_BmpWinOnVReq(&wk->ListWin);
    }
}

//------------------------------------------------------------------
/**
 * $brief   アイコン表示
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static void	_iconDisp(WIFIP2PMATCH_WORK * wk, int status, int vctIcon, int x,int y,int x2)
{
    u16 btl[]={ 0x17,0x18,0xc18,0x20 };  //ばとる
    u16 chg[]={ 0x25,0x26,0x2a,0xc25 };  //こうかん
    u16 wai[]={ 0x2f,0x42f,0x82f,0xc2f };  //たいき
    u16 vct[]={ 0x4a,0x4b,0x084a,0x084b };  //VCT
    u16 vctoff[]={ 0x48,0x49,0x0848,0x0849 };  //VCToff
    u16 hate[]={ 0x44,0x45,0x46,0x47 };  //?
    u16 none[]={ 0x6,0x6,0x6,0x6 };  //none
    u16* pData;
    // 0x06-----------なし
    u16* pScrAddr = GF_BGL_ScreenAdrsGet( wk->bgl, GF_BGL_FRAME0_M );
    int pos,col = 0;

    switch(status){
      case WIFI_STATUS_VCT:      // VCT中
        pData = vct;
        break;
      case WIFI_STATUS_SBATTLE50_WAIT:   // 対戦募集中
      case WIFI_STATUS_SBATTLE100_WAIT:   // 対戦募集中
      case WIFI_STATUS_SBATTLE_FREE_WAIT:   // 対戦募集中
      case WIFI_STATUS_DBATTLE50_WAIT:   // 対戦募集中
      case WIFI_STATUS_DBATTLE100_WAIT:   // 対戦募集中
      case WIFI_STATUS_DBATTLE_FREE_WAIT:   // 対戦募集中
#ifdef WFP2P_DEBUG_EXON 
      case WIFI_STATUS_MBATTLE_FREE_WAIT:   // 対戦募集中
#endif
        col = 0x1000;
      case WIFI_STATUS_SBATTLE50:      // 対戦中
      case WIFI_STATUS_SBATTLE100:      // 対戦中
      case WIFI_STATUS_SBATTLE_FREE:      // 対戦中
      case WIFI_STATUS_DBATTLE50:      // 対戦中
      case WIFI_STATUS_DBATTLE100:      // 対戦中
      case WIFI_STATUS_DBATTLE_FREE:      // 対戦中
#ifdef WFP2P_DEBUG_EXON 
      case WIFI_STATUS_MBATTLE_FREE:      // 対戦中
#endif
        pData = btl;
        break;
      case WIFI_STATUS_TRADE_WAIT:    // 交換募集中
      case WIFI_STATUS_POFIN_WAIT:    // 料理募集中
      case WIFI_STATUS_FRONTIER_WAIT:    // フロンティア募集中
#ifdef WFP2P_DEBUG_EXON 
      case WIFI_STATUS_BATTLEROOM_WAIT:// 交換募集中
#endif
      case WIFI_STATUS_BUCKET_WAIT:// 交換募集中
      case WIFI_STATUS_BALANCEBALL_WAIT:// 交換募集中
      case WIFI_STATUS_BALLOON_WAIT:// 交換募集中
        col = 0x1000;
      case WIFI_STATUS_TRADE:          // 交換中
      case WIFI_STATUS_POFIN:          // 料理中
      case WIFI_STATUS_FRONTIER:          // フロンティア中
#ifdef WFP2P_DEBUG_EXON 
      case WIFI_STATUS_BATTLEROOM:     // 交換中
#endif
      case WIFI_STATUS_BUCKET:     // 交換中
      case WIFI_STATUS_BALANCEBALL:     // 交換中
      case WIFI_STATUS_BALLOON:     // 交換中
        pData = chg;
        break;
      case WIFI_STATUS_LOGIN_WAIT:    // 待機中　ログイン直後はこれ
        pData = wai;
        break;
      case WIFI_STATUS_UNKNOWN:
        pData = hate;
        break;
      default:
        pData = none;
        break;
    }

    pos = x + y * 32;
    pScrAddr[pos] = pData[0]+col;
    pScrAddr[pos+1] = pData[1]+col;
    pScrAddr[pos+32] = pData[2]+col;
    pScrAddr[pos+33] = pData[3]+col;

    if(vctIcon){
        pData = none;
    }
    else{
        pData = vctoff;
    }
    pos = x2 + y * 32;
    pScrAddr[pos] = pData[0];
    pScrAddr[pos+1] = pData[1];
    pScrAddr[pos+32] = pData[2];
    pScrAddr[pos+33] = pData[3];
}

//------------------------------------------------------------------
/**
 * $brief   一列表示ごとのコールバック関数
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static void	_iconMenuDisp(WIFIP2PMATCH_WORK * wk, int toppos)
{
    u16* pData;
    // 0x06-----------なし
    u16* pScrAddr = GF_BGL_ScreenAdrsGet( wk->bgl, GF_BGL_FRAME0_M );
    int pos,status,i,vchat;
	_WIFI_MACH_STATUS* p_status;


    for(i = 0; i < WIFIP2PMATCH_DISP_MEMBER_MAX;i++){
        pos = toppos + i;
        if(pos >= WIFIP2PMATCH_MEMBER_MAX){
            status = WIFI_STATUS_NONE;
            vchat = 1;
        }
        else if(wk->index2No[pos]==0){
            status = WIFI_STATUS_NONE;
            vchat = 1;
        }
        else{
			p_status = WifiFriendMatchStatusGet( wk, wk->index2No[pos] - 1 );
			status = _WifiMyStatusGet( wk, p_status );
            vchat = p_status->vchat;
        }
        pos = 3 + ((5+(i*3)) * 32);
        _iconDisp(wk, status, vchat, 3, 5+(i*3), 27);
    }
	status = _WifiMyStatusGet( wk, &wk->pMatch->myMatchStatus );
    vchat = wk->pMatch->myMatchStatus.vchat;
    if(status == WIFI_STATUS_VCT){
        status = WIFI_STATUS_NONE;
    }
    _iconDisp(wk, status, vchat, 3, 21, 27);
    GF_BGL_LoadScreenReq(wk->bgl, GF_BGL_FRAME0_M);  // スクリーンに更新
}

//------------------------------------------------------------------
/**
 * $brief   カーソルコールバック
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static void _curCallBack(BMPLIST_WORK * wk,u32 param,u8 mode)
{
    if(mode == 0){
        Snd_SePlay(_SE_CURSOR);
    }
}


///選択メニューのリスト
static const BMPLIST_HEADER _friendInfoMenuListHeader = {
    NULL,			// 表示文字データポインタ
    _curCallBack,					// カーソル移動ごとのコールバック関数
    _lineCallBack,					// 一列表示ごとのコールバック関数
    NULL,                   // GF_BGL_BMPWIN
    (WIFIP2PMATCH_MEMBER_MAX+2),	// リスト項目数
    WIFIP2PMATCH_DISP_MEMBER_MAX,	// 表示最大項目数
    0,						// ラベル表示Ｘ座標
    8,						// 項目表示Ｘ座標
    0,						// カーソル表示Ｘ座標
    0,						// 表示Ｙ座標
    FBMP_COL_BLACK,			// 文字色
    0x8000,			// 背景色 ぬき？
    FBMP_COL_BLK_SDW,		// 文字影色
    0,						// 文字間隔Ｘ
    24,						// 文字間隔Ｙ
    BMPLIST_LRKEY_SKIP,		// ページスキップタイプ
    FONT_SYSTEM,				// 文字指定
    1,						// ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)
    NULL                    // work
    };

static int _addListString(WIFIP2PMATCH_WORK* wk)
{
    int i,num=0;

    for(i = 0; i < WIFIP2PMATCH_MEMBER_MAX; i++){
        if( wk->index2No[i] != 0){
            BMP_MENULIST_AddString( wk->menulist, wk->pTemp, wk->index2No[i] - 1 );
            //            BMP_MENULIST_AddString( wk->menulist, wk->TrainerName[wk->index2No[i] - 1], wk->index2No[i] - 1 );
            num++;
        }
    }
    BMP_MENULIST_AddArchiveString( wk->menulist, wk->MsgManager,
                                   msg_wifilobby_028, WIFIP2PMATCH_MEMBER_MAX );
    BMP_MENULIST_AddArchiveString( wk->menulist, wk->MsgManager,
                                   msg_wifilobby_047, BMPLIST_CANCEL );
    return num;
}

static void _menuFriendList(WIFIP2PMATCH_WORK* wk)
{
    int i,num = 0;
    BMPLIST_HEADER list_h;

    wk->menulist = BMP_MENULIST_Create( WIFIP2PMATCH_MEMBER_MAX+2, HEAPID_WIFIP2PMATCH );


    num = _addListString(wk);

    if(GF_BGL_BmpWinAddCheck(&wk->ListWin)){
        BmpTalkWinClear( &wk->ListWin, WINDOW_TRANS_ON );
        GF_BGL_BmpWinDel(&wk->ListWin);
    }
    //BMPウィンドウ生成
    GF_BGL_BmpWinAdd(wk->bgl,&wk->ListWin,
                     GF_BGL_FRAME3_M, 6, 5, 21, 14, FLD_SYSFONT_PAL, 1);


    list_h = _friendInfoMenuListHeader;
    list_h.list = wk->menulist;
    list_h.win = &wk->ListWin;
    list_h.count = num + 2;
    list_h.work = wk;
    wk->lw = BmpListSet(&list_h, 0, 0, HEAPID_WIFIP2PMATCH);


}

//------------------------------------------------------------------
/**
 * $brief   フレンドリスト表示初期化   WIFIP2PMATCH_FRIENDLIST_INIT
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------

static void _userDataDisp(WIFIP2PMATCH_WORK* wk)
{
    if(GF_BGL_BmpWinAddCheck(&wk->MyInfoWin)){
        GF_BGL_BmpWinDel(&wk->MyInfoWin);
    }
    if(GF_BGL_BmpWinAddCheck(&wk->MyInfoWinBack)){
        BmpMenuWinClear( &wk->MyInfoWinBack, WINDOW_TRANS_ON );
        GF_BGL_BmpWinDel(&wk->MyInfoWinBack);
    }

    GF_BGL_BmpWinAdd(wk->bgl,&wk->MyInfoWin,
                     GF_BGL_FRAME3_M, 
					 WIFIP2PMATCH_PLAYER_DISP_X, WIFIP2PMATCH_PLAYER_DISP_Y,
					 WIFIP2PMATCH_PLAYER_DISP_SIZX, WIFIP2PMATCH_PLAYER_DISP_SIZY,
                     FLD_SYSFONT_PAL, _CGX_USER_BOTTOM );


    GF_BGL_BmpWinAdd(wk->bgl,&wk->MyInfoWinBack,
                     GF_BGL_FRAME1_M, 
					 WIFIP2PMATCH_PLAYER_DISP_X, WIFIP2PMATCH_PLAYER_DISP_Y,
					 WIFIP2PMATCH_PLAYER_DISP_SIZX, WIFIP2PMATCH_PLAYER_DISP_SIZY,
                     FLD_SYSFONT_PAL, _CGX_USET_BACK_BOTTOM );

	// 初期化
    GF_BGL_BmpWinDataFill( &wk->MyInfoWinBack, 15 );
    GF_BGL_BmpWinOnVReq(&wk->MyInfoWinBack);
	

	// システムウィンドウ設定
	BmpMenuWinWrite( &wk->MyInfoWinBack, WINDOW_TRANS_ON, MENU_WIN_CGX_NUM, MENU_WIN_PAL );

}

// ダミーオブジェクトを登録しまくる
static void DEBUG_DummyObjIn( WIFIP2PMATCH_WORK *wk, int num )
{
	int i;
	MCR_MOVEOBJ* p_moveobj;
	static const u8 view_list[ 32 ][2] = {
		{ BOY1, PM_MALE },
		{ BOY3, PM_MALE }, 
		{ MAN3, PM_MALE }, 
		{ BADMAN, PM_MALE }, 
		{ EXPLORE, PM_MALE }, 
		{ FIGHTER, PM_MALE },
		{ GORGGEOUSM, PM_MALE }, 
		{ MYSTERY, PM_MALE },   
		{ GIRL1, PM_FEMALE }, 
		{ GIRL2, PM_FEMALE }, 
		{ WOMAN2, PM_FEMALE }, 
		{ WOMAN3, PM_FEMALE }, 
		{ IDOL, PM_FEMALE }, 
		{ LADY, PM_FEMALE }, 
		{ COWGIRL, PM_FEMALE }, 
		{ GORGGEOUSW, PM_FEMALE },
		{ BOY1, PM_MALE },
		{ BOY3, PM_MALE }, 
		{ MAN3, PM_MALE }, 
		{ BADMAN, PM_MALE }, 
		{ EXPLORE, PM_MALE }, 
		{ FIGHTER, PM_MALE },
		{ GORGGEOUSM, PM_MALE }, 
		{ MYSTERY, PM_MALE },   
		{ GIRL1, PM_FEMALE }, 
		{ GIRL2, PM_FEMALE }, 
		{ WOMAN2, PM_FEMALE }, 
		{ WOMAN3, PM_FEMALE }, 
		{ IDOL, PM_FEMALE }, 
		{ LADY, PM_FEMALE }, 
		{ COWGIRL, PM_FEMALE }, 
		{ GORGGEOUSW, PM_FEMALE },
	};

	for( i=0; i<num; i++ ){
		// 自分を出す
		p_moveobj = WIFI_MCR_SetNpc( &wk->matchroom, view_list[i][0], i+1 );

		// みんな飛び跳ねる
		WIFI_MCR_NpcMoveSet( &wk->matchroom, p_moveobj, MCR_NPC_MOVE_JUMP );
		MCRSYS_SetMoveObjWork( wk, p_moveobj );	// 登録したワークを保存しておく

	}
}

//------------------------------------------------------------------
/**
 * $brief   フレンドリスト表示初期化   WIFIP2PMATCH_FRIENDLIST_INIT
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_FriendListInit( WIFIP2PMATCH_WORK *wk, int seq )
{
    int i,x;
    int num = 0;
	ARCHANDLE* p_handle;
	MCR_MOVEOBJ* p_moveobj;
	int obj_code;

	// ワイプ終了待ち
	if( WIPE_SYS_EndCheck() == FALSE ){
		return seq;
	}

	// エラーチェック
    if( CommStateIsWifiError() == FALSE ){
	
		// 切断から再VCHAT募集状態に変わったのかをチェック
		if(!CommStateIsWifiLoginMatchState()){
			return seq;
		}
	}

	// コールバックを設定
	mydwc_setConnectModeCheckCallback( WIFIP2PModeCheck );

	// ここから先は通信エラーをシステムウィンドウで出す
//	CommStateSetErrorCheck(FALSE,FALSE);	//この関数の下にあるので削除
	
	ConnectBGPalAnm_OccSet(&wk->cbp, FALSE);

	p_handle = ArchiveDataHandleOpen( ARC_WIFIP2PMATCH_GRA, HEAPID_WIFIP2PMATCH );


	// アイコンデータ読込み
	WifiP2PMatchFriendListIconLoad( wk->bgl, &wk->icon, p_handle, HEAPID_WIFIP2PMATCH );

    _myVChatStatusOrgSet(wk);
	
//    _timeWaitIconDel(wk);		timeWait内でMsgWinを破棄しているということはメッセージ終了でもOK↓
	EndMessageWindowOff(wk);
	
    GF_BGL_ScrClear(wk->bgl, GF_BGL_FRAME3_M);

#if 0
    if(GF_BGL_BmpWinAddCheck(&wk->MyInfoWinBack)){
        BmpMenuWinClear(&wk->MyInfoWinBack, WINDOW_TRANS_ON);
        GF_BGL_BmpWinDel(&wk->MyInfoWinBack);
    }
#endif
    if(wk->menulist){
        GF_BGL_BmpWinDel(&wk->ListWin);
        BmpListExit(wk->lw, NULL, NULL);
        wk->lw = NULL;
        BMP_MENULIST_Delete( wk->menulist );
        wk->menulist = NULL;
    }
	if( WIFI_MCR_GetInitFlag( &wk->matchroom ) == FALSE ){

		if( wk->pMatch->myMatchStatus.sex == PM_MALE ){
			obj_code = HERO;
		}else{
			obj_code = HEROINE;
		}
		
		// マッチングルーム初期化
		wk->friend_num = WifiList_GetFriendDataLastIdx( wk->pList );
#ifdef WFP2PM_MANY_OBJ
		wk->friend_num = 32;
#endif
		WIFI_MCR_Init( &wk->matchroom, HEAPID_WIFIP2PMATCH, p_handle, wk->bgl, obj_code, wk->friend_num );

		// 自分を出す
		p_moveobj = WIFI_MCR_SetPlayer( &wk->matchroom, obj_code );
		MCRSYS_SetMoveObjWork( wk, p_moveobj );	// 登録したワークを保存しておく
#ifdef WFP2PM_MANY_OBJ
	DEBUG_DummyObjIn( wk, wk->friend_num );
#endif
	}
	// ビューアー初期化
	if( MCVSys_MoveCheck( wk ) == FALSE ){
		MCVSys_Init( wk, p_handle, HEAPID_WIFIP2PMATCH );
	}

	// 今の状態を書き込む
	MCVSys_ReWrite( wk, HEAPID_WIFIP2PMATCH );

#if 0
    GF_BGL_BmpWinAdd(wk->bgl, &wk->MyInfoWinBack, GF_BGL_FRAME3_M, 4, 1, 21, 2,
                     _NUKI_FONT_PALNO,  _CGX_TITLE_BOTTOM );
    GF_BGL_BmpWinDataFill( &wk->MyInfoWinBack, 0x8000 );
    MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_019, wk->TitleString );
    GF_STR_PrintColor( &wk->MyInfoWinBack, FONT_TALK, wk->TitleString, 0, 0, MSG_NO_PUT, _COL_N_WHITE, NULL);
    GF_BGL_BmpWinOnVReq(&wk->MyInfoWinBack);
#endif

	// ユーザーデータ表示
    EndMessageWindowOff(wk);
    _readFriendMatchStatus(wk);

	// ユーザーデータのBMP作成
	_userDataDisp(wk);

// WIFI　対戦AUTOﾓｰﾄﾞデバック
#ifndef _WIFI_DEBUG_TUUSHIN
    _myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);
#else	//_WIFI_DEBUG_TUUSHIN

	if( WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_BATTLE == FALSE ){
		
		if( WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_MODE == _WIFI_DEBUG_MODE_X ){
			_myStatusChange(wk, WIFI_STATUS_SBATTLE50_WAIT);
			WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_A_REQ = FALSE;
			WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_B_REQ = FALSE;
		}else{
			_myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);
			WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_A_REQ = TRUE;
			WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_B_REQ = FALSE;
		}

		WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_BATTLE = TRUE;
	}else{
		WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_BATTLE = FALSE;
		_myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);
	}
#endif	//_WIFI_DEBUG_TUUSHIN

	// 通信状態を元に戻す
	CommStateChangeWiFiLogin();

    wk->preConnect = -1;

	// 080605 tomoya BTS:249の対処のため
	// 親の通信リセット処理後直ぐに接続してしまうのを回避
	// （接続されてもVCHATの状態のまま、FriendListシーケンスに移行させる）
//	mydwc_ResetNewPlayer();	// ちなみにmydwc_returnLobby内でも初期化してる
#if 0
    CLACT_SetDrawFlag( wk->MainActWork[_CLACT_LINE_CUR], 1 );
#endif

	ArchiveDataHandleClose( p_handle );

	// ワイプイン
	WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK,
					COMM_BRIGHTNESS_SYNC, 1, HEAPID_WIFIP2PMATCH);
    wk->seq = WIFIP2PMATCH_MODE_FRIENDLIST;
    CommStateSetErrorCheck(FALSE,FALSE);   ///335
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   誰かが接続してきた場合
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _checkParentConnect(WIFIP2PMATCH_WORK *wk)
{
    if( CommWifiIsMatched()==1 ){
        return TRUE;
    }
    return FALSE;
}

static BOOL _checkParentNewPlayer( WIFIP2PMATCH_WORK *wk )
{
	if( (mydwc_IsNewPlayer() != -1) ){
		// ボイスチャットとBGM音量の関係を整理 tomoya takahashi
		// ここでVCHATをONにしないと
		// DWC_RAP内でVCHATを開始しなし
		mydwc_setVChat(wk->pMatch->myMatchStatus.vchat);
		OS_TPrintf( "Connect VCHAT set\n" );
		return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------------
/**
 * $brief   メインリストの書き換え
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static void _mainListReWrite(WIFIP2PMATCH_WORK *wk)
{
    int num,num2,checkMatch;
    u16 list,cursor;

    num = _readFriendMatchStatus(wk);
    num2 = _checkUserDataMatchStatus(wk);
    if((wk->friendMatchReadCount != num) || (num2 != 0)){
        OHNO_PRINT("人数が変わった %d %d\n", num, num2);
        //_readFriendMatchStatus(wk);  //再計算
        BMP_MENULIST_STRBUFDelete(wk->menulist);
        //BMP_MENULIST_Delete(wk->menulist);
        //wk->menulist = BMP_MENULIST_Create( WIFIP2PMATCH_MEMBER_MAX+2, HEAPID_WIFIP2PMATCH );
        BmpListParamSet(wk->lw ,BMPLIST_ID_COUNT, num + 2);
        if(num > (WIFIP2PMATCH_DISP_MEMBER_MAX-2)){
            BmpListParamSet(wk->lw ,BMPLIST_ID_LINE, WIFIP2PMATCH_DISP_MEMBER_MAX);
        }
        else{
            BmpListParamSet(wk->lw ,BMPLIST_ID_LINE, num+2);
        }
        _addListString(wk);

	// 「せつだん　する」の下に空白行が出来るのを抑制
	BmpListPosGet(wk->lw, &list, &cursor);
	if(num >= 3 && (num - 3) <= list){
	  list = num - 3;
	  if(list < 0)
	    list = 0;
	}
        BmpListMainOutControl(wk->lw, NULL,list,cursor,0,NULL,NULL);
	
        BmpListRewrite(wk->lw);
        wk->friendMatchReadCount = num;
    }

    BmpListPosGet(wk->lw, &list, &cursor);
    // カーソル位置の再表示
    if((wk->friendMatchReadCount+2) <= (list+cursor)){
      while((wk->friendMatchReadCount+2) <= (list+cursor)){
        if(list != 0){
	  list--;
        }
        else if(cursor != 0){
	  cursor--;
        }
      }
      OHNO_PRINT("カーソル位置変更 %d %d \n",list,cursor);
      BmpListMainOutControl(wk->lw, NULL,list,cursor,0,NULL,NULL);
      BmpListRewrite(wk->lw);
    }

    _iconMenuDisp(wk, list);
#if 0
    SetCursor_Pos(wk->MainActWork[_CLACT_LINE_CUR], 2*8, 8 * (cursor * 3 + 6));

    if(list > 0){    // 上カーソル
        SetCursor_Pos(wk->MainActWork[_CLACT_UP_CUR], 0x80, 32);
        CLACT_SetDrawFlag( wk->MainActWork[_CLACT_UP_CUR], 1 );
    }
    else{
        CLACT_SetDrawFlag( wk->MainActWork[_CLACT_UP_CUR], 0 );
    }
    if((list < (wk->friendMatchReadCount - WIFIP2PMATCH_DISP_MEMBER_MAX + 2))){
        SetCursor_Pos(wk->MainActWork[_CLACT_DOWN_CUR], 0x80, 0x9c);
        CLACT_SetDrawFlag( wk->MainActWork[_CLACT_DOWN_CUR], 1 );
    }
    else{
        CLACT_SetDrawFlag( wk->MainActWork[_CLACT_DOWN_CUR], 0 );
    }
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	友達の出入りを管理する
 *
 *	@param	wk	ワーク
 *
 *	@retval	今の友達の数
 */
//-----------------------------------------------------------------------------
static int MCRSYS_ContFiendInOut( WIFIP2PMATCH_WORK* wk )
{
	int friend_num;
	int i, j;
	MCR_MOVEOBJ* p_obj;
	BOOL match;
	_WIFI_MACH_STATUS* p_status;
	BOOL in_flag;
	BOOL out_flag;

	// 最新版の友達数と、友達ナンバー配列を作成
	friend_num = _readFriendMatchStatus( wk );
//	wk->friendMatchReadCount = friend_num;

	in_flag = FALSE;
	out_flag = FALSE;

	// バックアップと比較する
	if( memcmp( wk->index2No, wk->index2NoBackUp, sizeof(u8)*WIFIP2PMATCH_MEMBER_MAX ) != 0 ){

		// 人が変わった
		// バックアップ側にいて、最新版にいないやつはOUT
		// 最新版にいて、バックアップ版にいないやつはIN
		for( i=0; i<WIFIP2PMATCH_MEMBER_MAX; i++ ){
			
			// OUTチェック
			match = FALSE;
			for( j=0; j<WIFIP2PMATCH_MEMBER_MAX; j++ ){
				if( wk->index2No[j] == wk->index2NoBackUp[i] ){
					match = TRUE;
					break;
				}
			}
			// いなかったので削除
			// 一気に32人入ってきたとき0がいないという風になってしまうので
			// ０じゃないかもチェック
			if( (match == FALSE) && (wk->index2NoBackUp[i] != 0) ){
				p_obj = MCRSYS_GetMoveObjWork( wk, wk->index2NoBackUp[i] );
				if( p_obj != NULL ){
					WIFI_MCR_DelPeopleReq( &wk->matchroom, p_obj );
					MCRSYS_DelMoveObjWork( wk, p_obj );
					MCVSys_BttnDel( wk, wk->index2NoBackUp[i] );
					WifiP2PMatch_UserDispOff_Target( wk, wk->index2NoBackUp[i], HEAPID_WIFIP2PMATCH );	// その人がした画面に詳細表示されているなら消す
					out_flag = TRUE;
				}
			}

			// INチェック
			match = FALSE;
			for( j=0; j<WIFIP2PMATCH_MEMBER_MAX; j++ ){
				if( wk->index2No[i] == wk->index2NoBackUp[j] ){
					match = TRUE;
					break;
				}
			}
			// 前はいなかったので追加
			if( (match == FALSE) && (wk->index2No[i] != 0) ){
				p_status = WifiFriendMatchStatusGet( wk, wk->index2No[i]-1 );

				p_obj = WIFI_MCR_SetNpc( &wk->matchroom, 
						p_status->trainer_view, wk->index2No[i] );


				// 登録できたかチェック
				if( p_obj ){

					MCVSys_BttnSet( wk, wk->index2No[i], MCV_BTTN_FRIEND_TYPE_IN );
					MCRSYS_SetMoveObjWork( wk, p_obj );

					in_flag = TRUE;
				}else{

					// オブジェが登録できなかったので予約登録
					// （これだとボタンを押すことが出来ない）
					MCVSys_BttnSet( wk, wk->index2No[i], MCV_BTTN_FRIEND_TYPE_RES );
					wk->index2No[i] = 0;	// 入ってきたのはなかったことにする
				}
			}
		}

		// 最新版に変更
		memcpy( wk->index2NoBackUp, wk->index2No, sizeof(u8)*WIFIP2PMATCH_MEMBER_MAX );

		// ボタン全描画リクエスト
		MCVSys_BttnAllWriteReq( wk );

		if( in_flag == TRUE ){
			Snd_SePlay( _SE_INOUT );
		}else if( out_flag == TRUE ){
			Snd_SePlay( _SE_INOUT );
		}
	}	


	return friend_num;
}

//----------------------------------------------------------------------------
/**
 *	@brief	友達の状態変化を表示
 *
 *	@param	wk	ワーク
 */
//-----------------------------------------------------------------------------
static void MCRSYS_ContFriendStatus( WIFIP2PMATCH_WORK* wk, u32 heapID )
{
	int i;
	u32 friendID;
	u32 type;
	BOOL vchat;
	int change_num;

	// 状態変更数を取得（今までと動きを一緒にするためにこうしておく）
    change_num = _checkUserDataMatchStatus( wk );

	// 皆の状態変化を登録する
	if( change_num > 0 ){
		MCVSys_ReWrite( wk, heapID );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	友達オブジェクトの登録破棄管理
 *
 *	@param	wk	ワーク
 */
//-----------------------------------------------------------------------------
static void MCRSYS_SetFriendObj( WIFIP2PMATCH_WORK* wk, u32 heapID )
{
	// 友達ナンバーが変わっていたらいなくなった友達を消し、増えた友達を登録
	MCRSYS_ContFiendInOut( wk );

	// 状態が変わったら何かを変える
	MCRSYS_ContFriendStatus( wk, heapID );
}


//------------------------------------------------------------------
/**
 * $brief   フレンドリスト表示中処理 WIFIP2PMATCH_MODE_FRIENDLIST
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int WifiP2PMatch_FriendList( WIFIP2PMATCH_WORK *wk, int seq )
{
    u32	ret = MCR_RET_NONE;
    int checkMatch;
	u32 check_friend;
	MCR_MOVEOBJ* p_obj;
	u32 status;


	// ワイプ終了待ち
	if( WIPE_SYS_EndCheck() == FALSE ){
		return seq;
	}

	// エラーチェック
    if( CommStateIsWifiError()){  
//        wk->localTime=0;
        _errorDisp(wk);
        return seq;
    }

	// ボイスチャットONOFF繰り返しでここの画面ではまる為
	// えら状態になったらDISCONNECT処理に遷移
	if( CommWifiIsMatched()==4 ){
		// この処理は、_parentModeCallMenuInitからもってきました
        // 適合しない=>切断処理へ
        _friendNameExpand(wk, mydwc_getFriendIndex());
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
        wk->seq = WIFIP2PMATCH_MODE_DISCONNECT;
        _myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);
        CommStateWifiMatchEnd();
		return seq;
	}

	/*
	 * 080607 tomoya
	 * comm_state.c内でAUTOにするところをやめて、
	 * すべて、comm_stateの外側（アプリ側）で設定変更するようにしました。
	 * 
	// ここに来ているのに、CommStateErrorCheckのWi-FiがAUTOなら
	// それを解除
	// 080605 tomoya  comm_state.c _stateWifiMatchEnd内でAUTOになるため
	if( CommStateGetErrorCheck() == TRUE ){
		CommStateSetErrorCheck( FALSE, FALSE );
	}
	//*/

 //   _mainListReWrite(wk);
 
	// 友達が現れたら出す
	MCRSYS_SetFriendObj( wk, HEAPID_WIFIP2PMATCH );
	MCVSys_UpdataBttn( wk );


	// パソコンアニメが動いていたら終了
	WIFI_MCR_PCAnmOff( &wk->matchroom );	

	// ボイスチャットのONOFFはプレイヤーが動けるときにだけ設定できる
	if( WIFI_MCR_PlayerMovePauseGet( &wk->matchroom ) == FALSE ){
		// ボイスチャット
		if(mydwc_IsNewPlayer() == -1){
			if(PAD_BUTTON_X & sys.trg){
				if(_myVChatStatusToggleOrg(wk)){
					WifiP2PMatchMessagePrint(wk, msg_wifilobby_054, FALSE);
				}
				else{
					WifiP2PMatchMessagePrint(wk, msg_wifilobby_055, FALSE);
				}
				Snd_SePlay(_SE_DESIDE);
				_userDataInfoDisp(wk);
				wk->seq = WIFIP2PMATCH_VCHATWIN_WAIT;
				WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );	// した画面初期化
				return seq;
			}
		}


		// した画面も動かない
		check_friend = MCVSys_Updata( wk, HEAPID_WIFIP2PMATCH );	// した画面も動かす
		p_obj = MCRSYS_GetMoveObjWork( wk, check_friend );
		if( check_friend != 0 ){
			if( p_obj ){
				WIFI_MCR_CursorOn( &wk->matchroom, p_obj );
			}else{
				WIFI_MCR_CursorOff( &wk->matchroom );
			}
		}else{
			WIFI_MCR_CursorOff( &wk->matchroom );
		}

	}

#ifdef WFP2P_DEBUG   /// ＶＣＨＡＴ音質切り替え
#if 0
    if(PAD_BUTTON_Y & sys.trg){
        switch(wk->vctEnc){
          case VCHAT_2BIT_ADPCM:
            wk->vctEnc = VCHAT_3BIT_ADPCM;
            WifiP2PMatchMessagePrint(wk, msg_wifilobby_debug_03, FALSE);
            break;
          case VCHAT_3BIT_ADPCM:
            wk->vctEnc = VCHAT_4BIT_ADPCM;
            WifiP2PMatchMessagePrint(wk, msg_wifilobby_debug_04, FALSE);
            break;
          case VCHAT_4BIT_ADPCM:
          default:
            wk->vctEnc = VCHAT_2BIT_ADPCM;
            WifiP2PMatchMessagePrint(wk, msg_wifilobby_debug_02, FALSE);
            break;
        }
        mydwc_setVchat(wk->vctEnc);
        wk->seq = WIFIP2PMATCH_VCHATWIN_WAIT;
		WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );	// した画面初期化
        return seq;
    }
#endif
#endif

	// 友達からこちらに接続してきたときの処理
    checkMatch = _checkParentConnect(wk);
    if( (0 !=  checkMatch) && (wk->preConnect != -1) ){ // 接続してきた
        OHNO_PRINT("接続を拾ってしまっている-------------\n");
        Snd_SePlay(_SE_OFFER);
        wk->seq = WIFIP2PMATCH_MODE_CALL_INIT;
        return seq;
    }
    if((wk->preConnect == -1) && (mydwc_IsNewPlayer() != -1)){	// 通常のコネクト開始
        wk->preConnect = mydwc_IsNewPlayer();
        _friendNameExpand(wk, wk->preConnect);
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_043, FALSE);

		// ボイスチャット設定
		mydwc_setVChat(wk->pMatch->myMatchStatus.vchat);// ボイスチャットとBGM音量の関係を整理 tomoya takahashi
		OS_TPrintf( "Connect VCHAT set\n" );

		// つながった人のデータ表示
//		WifiP2PMatch_UserDispOn( wk, wk->preConnect+1, HEAPID_WIFIP2PMATCH );

        wk->localTime = 0;
    }
    else if((wk->preConnect != -1) && (mydwc_IsNewPlayer() == -1)){	// 自分的には新しくコネクトしてきたのに、実際はコネクトしてきていなかったとき？

/*
        GF_BGL_BmpWinDel(&wk->ListWin);
        BmpListExit(wk->lw, NULL, NULL);
        wk->lw = NULL;
        BMP_MENULIST_Delete( wk->menulist );
        wk->menulist = NULL;
//*/

		// ボイスチャット設定
		mydwc_setVChat(FALSE);// ボイスチャットとBGM音量の関係を整理 tomoya takahashi
        _friendNameExpand(wk, wk->preConnect);
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
        CommStateWifiMatchEnd();
        wk->seq = WIFIP2PMATCH_MODE_VCT_DISCONNECT;
        return seq;
    }

	// VCHAT　ON　状態なのに、新しいコネクトが-1ならVCHATをOFFにする
	// 080615	tomoya
	if( (mydwc_IsVChat() == TRUE) && (mydwc_IsNewPlayer() == -1) ){
		mydwc_setVChat(FALSE);// ボイスチャットとBGM音量の関係を整理 tomoya takahashi
	}
	
/*    if(wk->localTime){
        wk->localTime--;
        if(wk->localTime == 0){
            EndMessageWindowOff(wk);
        }
    }*/

//    _mainListReWrite(wk);


	// 状態を取得
	status = _WifiMyStatusGet( wk, &wk->pMatch->myMatchStatus );

	// 誰も自分に接続してきていないときだけリストを動かせる
    if(wk->preConnect == -1){

		// CANCELボタンでも待機状態をクリア
		if( sys.trg & PAD_BUTTON_CANCEL ){
			if(_modeWait(status)){	// 待ち状態のとき
				Snd_SePlay(_SE_DESIDE);
				wk->seq = WIFIP2PMATCH_MODE_SELECT_REL_INIT;  // 解除
				WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );	// した画面初期化
				return seq;
			}
		}
		
		ret = WIFI_MCR_Main( &wk->matchroom );
		WIFI_MCR_PCAnmMain( &wk->matchroom );	// パソコンアニメメイン
//        ret = BmpListMain(wk->lw);
    }
    switch(ret){
      case MCR_RET_NONE:
        return seq;
      case MCR_RET_CANCEL:
        Snd_SePlay(_SE_DESIDE);
        if(_modeWait(status)){	// 待ち状態のとき
            wk->seq = WIFIP2PMATCH_MODE_SELECT_REL_INIT;  // 自分が待機の場合必ず解除
        }
        else{	// それか終了チェックへ
            wk->endSeq = WIFI_P2PMATCH_END;
            wk->seq = WIFIP2PMATCH_MODE_EXIT_YESNO;
            WifiP2PMatchMessagePrint(wk, dwc_message_0010, TRUE);
            //        wk->localTime=0;
            ret = BMPLIST_CANCEL;
        }
		WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );	// した画面初期化
        return seq;

      case MCR_RET_MYSELECT:
//        wk->localTime=0;
        Snd_SePlay(_SE_DESIDE);
        if(_modeWait(status)){
			WIFI_MCR_PCAnmStart( &wk->matchroom );	// pcアニメ開始
            wk->seq = WIFIP2PMATCH_MODE_SELECT_REL_INIT;  // 自分が待機の場合必ず解除
        }
        else{  // 募集の行で選択したとき
            if(status == WIFI_STATUS_LOGIN_WAIT){

				// 自分の状態が、マッチングまでいっているかチェックする
				// 080628	tomoya
				if( CommStateIsWifiLoginMatchState() == TRUE ){
					WIFI_MCR_PCAnmStart( &wk->matchroom );	// pcアニメ開始
	                wk->seq = WIFIP2PMATCH_MODE_SELECT_INIT;
				}
            }
        }
		WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );	// した画面初期化
        return seq;
        break;

	case MCR_RET_SELECT:
        Snd_SePlay(_SE_DESIDE);
        if(_modeWait(status)){
            wk->seq = WIFIP2PMATCH_MODE_SELECT_REL_INIT;  // 自分が待機の場合必ず解除
        }
        else{  // 人の名前ー＞マッチングへ
            wk->seq = WIFIP2PMATCH_MODE_MATCH_INIT;
        }
		WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );	// した画面初期化
		return seq;
		break;

	default:
		GF_ASSERT(0);
		break;
    }
	/*
    GF_BGL_BmpWinDel(&wk->ListWin);
    BmpListExit(wk->lw, NULL, NULL);
    wk->lw = NULL;
    BMP_MENULIST_Delete( wk->menulist );
    wk->menulist = NULL;
	//*/
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   VCTONOFFウインドウを閉じる
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _vchatToggleWait( WIFIP2PMATCH_WORK *wk, int seq )
{
    if( 0 !=  _checkParentNewPlayer(wk)){ // 接続してきた
        wk->seq = WIFIP2PMATCH_MODE_FRIENDLIST;
		// 主人公の動作を許可
		FriendRequestWaitOff( wk );
    }
    else if( GF_MSG_PrintEndCheck( wk->MsgIndex ) == 0 ){
        wk->seq = WIFIP2PMATCH_MODE_FRIENDLIST;
		// 主人公の動作を許可
		FriendRequestWaitOff( wk );
        EndMessageWindowOff(wk);
    }
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   VCT接続開始
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_VCTConnectInit2( WIFIP2PMATCH_WORK *wk, int seq )        // WIFIP2PMATCH_MODE_VCT_CONNECT_INIT2
{
	_WIFI_MACH_STATUS* p_status;
	int vchat;
	int status;

	// VChatフラグを取得
	p_status = WifiFriendMatchStatusGet( wk, wk->friendNo - 1 );
	vchat	= p_status->vchat;
	status	= _WifiMyStatusGet( wk, p_status );

    wk->cancelEnableTimer--;
    if(wk->cancelEnableTimer < 0  ){
        wk->seq = WIFIP2PMATCH_MODE_CANCEL_ENABLE_WAIT;
    }
    else
        if(CommWifiIsMatched() == 5){
        _friendNameExpand(wk, mydwc_getFriendIndex());
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_012, FALSE);
        wk->seq = WIFIP2PMATCH_MODE_VCT_DISCONNECT;
    }
    else if(CommWifiIsMatched() == 3 ){
        _friendNameExpand(wk, mydwc_getFriendIndex());
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
        wk->seq = WIFIP2PMATCH_MODE_VCT_DISCONNECT;
    }
    else if((CommWifiIsMatched() == 4) || (CommStateIsWifiDisconnect())){
        _friendNameExpand(wk, mydwc_getFriendIndex());
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_016, FALSE);
        wk->seq = WIFIP2PMATCH_MODE_VCT_DISCONNECT;
    }
    else if(CommStateIsWifiError()){
        _errorDisp(wk);
    }
#if PLFIX_T869
	// VCHATフラグがOFFになっていたら通信切断
    else if(vchat == FALSE){
        _friendNameExpand(wk, mydwc_getFriendIndex());
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
        wk->seq = WIFIP2PMATCH_MODE_VCT_DISCONNECT;
	}
#endif
	// 相手の状態が通常状態でない場合切断
	else if( status != WIFI_STATUS_LOGIN_WAIT ){
        _friendNameExpand(wk, mydwc_getFriendIndex());
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
        wk->seq = WIFIP2PMATCH_MODE_VCT_DISCONNECT;
	}
	else if(CommWifiIsMatched()==1){  // つながった
	//    _timeWaitIconDel(wk);		timeWait内でMsgWinを破棄しているということはメッセージ終了でもOK↓
		EndMessageWindowOff(wk);
        _myStatusChange(wk, WIFI_STATUS_VCT);  // VCT中になる
        wk->seq = WIFIP2PMATCH_MODE_VCT_CONNECT; // 次に行く際に呼ぶ
		WifiList_SetLastPlayDate( wk->pList, wk->friendNo - 1);	// 最後に遊んだ日付は、VCTがつながったときに設定する
    }

    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   VCT接続開始
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_VCTConnectInit( WIFIP2PMATCH_WORK *wk, int seq )        // WIFIP2PMATCH_MODE_VCT_CONNECT_INIT
{

	// 080703	tomoya takahashi
	// やはりはまるときはあるようなので、
	//　すぐに次にいくように修正する
	_friendNameExpand(wk, mydwc_getFriendIndex());
	WifiP2PMatchMessagePrint(wk, msg_wifilobby_011, FALSE);
	_myStatusChange(wk, WIFI_STATUS_VCT);  // VCT中になる
	wk->seq = WIFIP2PMATCH_MODE_VCT_CONNECT_WAIT; // 次に行く際に呼ぶ

	WifiList_SetLastPlayDate( wk->pList, mydwc_getFriendIndex());	// 最後に遊んだ日付は、VCTがつながったときに設定する

#if 0
    if(CommWifiIsMatched()==1){  // つながった
        _friendNameExpand(wk, mydwc_getFriendIndex());
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_011, FALSE);
        _myStatusChange(wk, WIFI_STATUS_VCT);  // VCT中になる
        wk->seq = WIFIP2PMATCH_MODE_VCT_CONNECT_WAIT; // 次に行く際に呼ぶ
    }
#endif

    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   VCT接続まち
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_VCTConnectWait( WIFIP2PMATCH_WORK *wk, int seq )        // WIFIP2PMATCH_MODE_VCT_CONNECT_WAIT
{

	_WIFI_MACH_STATUS* p_status;
	int vchat;
	int status;

	if(CommStateIsWifiError()){
        _errorDisp(wk);
		return seq;
    }

	// 080624　追加
	// 通信リセットされるとindexが0より小さくなる
	// そしたら切断
	if( mydwc_getFriendIndex() < 0 ){
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_016, FALSE);
        wk->seq = WIFIP2PMATCH_MODE_VCT_DISCONNECT;
		return seq;
	}

	// 080624　追加
	// statusをチェック
	p_status = WifiFriendMatchStatusGet( wk, mydwc_getFriendIndex() );
	status	= _WifiMyStatusGet( wk, p_status );
	if( (status != WIFI_STATUS_LOGIN_WAIT) && (status != WIFI_STATUS_VCT) ){
		_friendNameExpand(wk, mydwc_getFriendIndex());
		WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
		wk->seq = WIFIP2PMATCH_MODE_VCT_DISCONNECT;
		return seq;
	}
	
    if( GF_MSG_PrintEndCheck( wk->MsgIndex ) == 0 ){
        if(sys.trg & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)){
            EndMessageWindowOff(wk);
            wk->seq = WIFIP2PMATCH_MODE_VCT_CONNECT; // 次に行く際に呼ぶ
        }
        else{
            WifiP2PMatch_VCTConnect(wk,seq);
        }
    }
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   VCT接続中  WIFIP2PMATCH_MODE_VCT_CONNECT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_VCTConnect( WIFIP2PMATCH_WORK *wk, int seq )
{
	_WIFI_MACH_STATUS* p_status;
	int status;


	// 080624　変更
	//if( mydwc_getFriendIndex() < 0 ){
    //    wk->seq = WIFIP2PMATCH_MODE_VCT_DISCONNECT;
	//}
	// 通信リセットされるとindexが0より小さくなる
	// そしたら切断
	if( mydwc_getFriendIndex() < 0 ){
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_016, FALSE);
        wk->seq = WIFIP2PMATCH_MODE_VCT_DISCONNECT;
		return seq;
	}

	
	// VChatフラグを取得
	p_status = WifiFriendMatchStatusGet( wk, mydwc_getFriendIndex() );
	status	= _WifiMyStatusGet( wk, p_status );

//    _mainListReWrite(wk);
	// 友達が現れたら出す
	MCRSYS_SetFriendObj( wk, HEAPID_WIFIP2PMATCH );
	MCVSys_UpdataBttn( wk );

#if 0
    CLACT_SetDrawFlag( wk->MainActWork[_CLACT_VCT_MOVE], 0 );
    CLACT_SetDrawFlag( wk->MainActWork[_CLACT_VCT_STOP], 0 );
#endif
    if(CommWifiIsMatched() == 5){
        _friendNameExpand(wk, mydwc_getFriendIndex());
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_012, FALSE);
        wk->seq = WIFIP2PMATCH_MODE_VCT_DISCONNECT;
    }
    else if(CommWifiIsMatched() == 3 ){
        _friendNameExpand(wk, mydwc_getFriendIndex());
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
        wk->seq = WIFIP2PMATCH_MODE_VCT_DISCONNECT;
    }
    else if((CommWifiIsMatched() == 4) || (CommStateIsWifiDisconnect())){
        _friendNameExpand(wk, mydwc_getFriendIndex());
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_016, FALSE);
        wk->seq = WIFIP2PMATCH_MODE_VCT_DISCONNECT;
    }
    else if(CommStateIsWifiError()){
        _errorDisp(wk);
    }
	// 相手の状態が通常状態orVCT状態でない場合切断
	else if( (status != WIFI_STATUS_LOGIN_WAIT) && (status != WIFI_STATUS_VCT) ){
        _friendNameExpand(wk, mydwc_getFriendIndex());
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
        wk->seq = WIFIP2PMATCH_MODE_VCT_DISCONNECT;
	}
    else if(sys.trg & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)){
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_017, FALSE);
        wk->seq = WIFIP2PMATCH_MODE_VCT_CONNECTEND_YESNO;
    }
    else{
		// アイコン書き換え
        if(mydwc_IsSendVoiceAndInc()){
			WifiP2PMatchFriendListIconWrite( wk->bgl, &wk->icon, GF_BGL_FRAME1_M,
				PLAYER_DISP_ICON_POS_X, PLAYER_DISP_ICON_POS_Y, PLAYER_DISP_ICON_IDX_VCTBIG, 0 );
        }
        else{
			WifiP2PMatchFriendListIconWrite( wk->bgl, &wk->icon, GF_BGL_FRAME1_M,
				PLAYER_DISP_ICON_POS_X, PLAYER_DISP_ICON_POS_Y, PLAYER_DISP_ICON_IDX_VCT, 0 );
        }
    }

    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   VCT接続完了
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_VCTConnectEndYesNo( WIFIP2PMATCH_WORK *wk, int seq )        // WIFIP2PMATCH_MODE_VCT_CONNECT
{
	if(CommStateIsWifiError()){
        _errorDisp(wk);
		return seq;
    }
	
    //Snd_PlayerSetInitialVolume( SND_HANDLE_BGM, PV_VOL_DEFAULT/3 );
    if( GF_MSG_PrintEndCheck( wk->MsgIndex ) == 0 ){
        WifiList_SetLastPlayDate( wk->pList, mydwc_getFriendIndex());	// 最後に遊んだ日付は、VCTがつながったときに設定する
        // はいいいえウインドウを出す
        wk->pYesNoWork =
            BmpYesNoSelectInit( wk->bgl,
                                &_yesNoBmpDat,
                                MENU_WIN_CGX_NUM, MENU_WIN_PAL,
                                HEAPID_WIFIP2PMATCH );
        wk->seq = WIFIP2PMATCH_MODE_VCT_CONNECTEND_WAIT;
    }
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   VCT接続終了  WIFIP2PMATCH_MODE_VCT_CONNECTEND_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_VCTConnectEndWait( WIFIP2PMATCH_WORK *wk, int seq )        // WIFIP2PMATCH_MODE_VCT_CONNECT
{
    int i;
    int ret = BmpYesNoSelectMain(wk->pYesNoWork, HEAPID_WIFIP2PMATCH);

    if(ret == BMPMENU_NULL){  // まだ選択中
        if((CommWifiIsMatched() >= 4) || CommStateIsWifiDisconnect() || !CommIsConnect(COMM_PARENT_ID)){

			// ユーザーデータOFF
//			WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );
//
			EndMessageWindowOff(wk);

            _myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);
            BmpYesNoWinDel( wk->pYesNoWork, HEAPID_WIFIP2PMATCH );
            wk->preConnect = -1;
            wk->timer = _RECONECTING_WAIT_TIME;
            wk->seq = WIFIP2PMATCH_RECONECTING_WAIT;
        }else if( CommStateIsWifiError() ){
			_errorDisp(wk);
		}
        return seq;
    }else if(ret == 0){ // はいを選択した場合

        if(!CommStateIsWifiError()){
            _myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);
            CommStateWifiBattleMatchEnd();
            wk->preConnect = -1;
            wk->timer = _RECONECTING_WAIT_TIME;
            wk->seq = WIFIP2PMATCH_RECONECTING_WAIT;
        }else{
			_errorDisp(wk);
		}
    }
    else{  // いいえを選択した場合
        wk->seq = WIFIP2PMATCH_MODE_VCT_CONNECT;
    }
    EndMessageWindowOff(wk);
    return seq;
}



//WIFIP2PMATCH_MODE_VCT_DISCONNECT
static int WifiP2PMatch_VCTDisconnect(WIFIP2PMATCH_WORK *wk, int seq)
{
	u32 status;
	
	if(CommStateIsWifiError()){
        _errorDisp(wk);
		return seq;
    }
	
	
	// ユーザーデータOFF
//	WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );

	status = _WifiMyStatusGet( wk, &wk->pMatch->myMatchStatus );
	
    if(status != WIFI_STATUS_LOGIN_WAIT){
        _myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);
    }
    if( GF_MSG_PrintEndCheck( wk->MsgIndex ) != 0 ){
        wk->timer = 30;
        return seq;
    }
    wk->timer--;
    if((sys.trg & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)) || (wk->timer==0)){
        EndMessageWindowOff(wk);
        CommStateWifiMatchEnd();

		// 通信状態を元に戻す
		CommStateChangeWiFiLogin();
		
//        if(wk->menulist==NULL){
		if( WIFI_MCR_GetInitFlag( &wk->matchroom ) == FALSE ){
            wk->timer = _RECONECTING_WAIT_TIME;
            wk->seq = WIFIP2PMATCH_RECONECTING_WAIT;
        }
        else{

			// VCHATを元に戻す
			_myVChatStatusOrgSet( wk );
			_userDataInfoDisp(wk);
			
            wk->preConnect = -1;
            wk->seq = WIFIP2PMATCH_MODE_FRIENDLIST;

			// 主人公の動作を許可
			FriendRequestWaitOff( wk );
        }
    }
    return seq;
}

//WIFIP2PMATCH_MODE_BATTLE_DISCONNECT
static int WifiP2PMatch_BattleDisconnect(WIFIP2PMATCH_WORK *wk, int seq)
{
    if( GF_MSG_PrintEndCheck( wk->MsgIndex ) != 0 ){
        return seq;
    }
    if(sys.trg & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)){
        EndMessageWindowOff(wk);
        _myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);
        CommStateWifiMatchEnd();
        wk->timer = _RECONECTING_WAIT_TIME;
        wk->seq = WIFIP2PMATCH_RECONECTING_WAIT;
    }
    return seq;
}

//WIFIP2PMATCH_MODE_DISCONNECT
static int WifiP2PMatch_Disconnect(WIFIP2PMATCH_WORK *wk, int seq)
{
	// ユーザーデータOFF
//	WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );
//
	// エラーチェック
    if( CommStateIsWifiError() ){
        _errorDisp(wk);
        return seq;
    }


    if( GF_MSG_PrintEndCheck( wk->MsgIndex ) != 0 ){
        wk->timer = 30;
        return seq;
    }
    wk->timer--;
    if((sys.trg & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)) || (wk->timer==0)){
        EndMessageWindowOff(wk);
        _myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);

		// 通信状態を元に戻す
		CommStateChangeWiFiLogin();
//        if(wk->menulist==NULL){
		if( WIFI_MCR_GetInitFlag( &wk->matchroom ) == FALSE ){
            wk->seq = WifiP2PMatchFriendListStart();
        }
        else{

			// VCHATをオリジナルに戻す
			_myVChatStatusOrgSet( wk );
			_userDataInfoDisp(wk);
			
            wk->preConnect = -1;
            wk->seq = WIFIP2PMATCH_MODE_FRIENDLIST;
			// 主人公の動作を許可
			FriendRequestWaitOff( wk );
        }
    }
    return seq;
}

static int WifiP2PMatch_BattleConnectInit( WIFIP2PMATCH_WORK *wk, int seq )    // WIFIP2PMATCH_MODE_BATTLE_CONNECT_INIT
{
    return seq;
}

static int WifiP2PMatch_BattleConnectWait( WIFIP2PMATCH_WORK *wk, int seq )     // WIFIP2PMATCH_MODE_BATTLE_CONNECT_WAIT
{
    return seq;
}

static int WifiP2PMatch_BattleConnect( WIFIP2PMATCH_WORK *wk, int seq )        // WIFIP2PMATCH_MODE_BATTLE_CONNECT
{
    return seq;
}


//------------------------------------------------------------------
/**
 * $brief   カーソル位置を変更する
 *
 * @param   act		アクターのポインタ
 * @param   x
 * @param   y
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void SetCursor_Pos( CLACT_WORK_PTR act, int x, int y )
{
    VecFx32 mat;

    mat.x = FX32_CONST( x );
    mat.y = FX32_CONST( y );
    mat.z = 0;
    CLACT_SetMatrix( act, &mat);

}

//------------------------------------------------------------------
/**
 * $brief   ボタンを押すと終了  WIFIP2PMATCH_MODE_CHECK_AND_END
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int WifiP2PMatch_CheckAndEnd( WIFIP2PMATCH_WORK *wk, int seq )
{
	if( wk->timer > 0 ){
		wk->timer --;
		return seq;
	}

    if(sys.trg & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)){


        WifiP2PMatchMessagePrint(wk, dwc_message_0011, TRUE);
        wk->seq = WIFIP2PMATCH_MODE_EXITING;
        wk->timer = 1;
		
/*
        wk->endSeq = WIFI_P2PMATCH_END;
        wk->seq = WIFIP2PMATCH_MODE_END_WAIT;
        CommStateWifiLogout();
//*/
    }
    return seq;
}



//------------------------------------------------------------------
/**
 * $brief   募集の取り消し WIFIP2PMATCH_MODE_SELECT_REL_INIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _parentModeSelectRelInit( WIFIP2PMATCH_WORK *wk, int seq )
{
	WIFI_MCR_PCAnmMain( &wk->matchroom );	// パソコンアニメメイン
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_007, FALSE);
    wk->seq = WIFIP2PMATCH_MODE_SELECT_REL_YESNO;
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   募集の取り消し  WIFIP2PMATCH_MODE_SELECT_REL_YESNO
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _parentModeSelectRelYesNo( WIFIP2PMATCH_WORK* wk, int seq )
{
	WIFI_MCR_PCAnmMain( &wk->matchroom );	// パソコンアニメメイン
    if( GF_MSG_PrintEndCheck( wk->MsgIndex ) == 0 ){
        // はいいいえウインドウを出す
        wk->pYesNoWork =
            BmpYesNoSelectInit( wk->bgl,
                                &_yesNoBmpDat,
                                MENU_WIN_CGX_NUM, MENU_WIN_PAL,
                                HEAPID_WIFIP2PMATCH );
        wk->seq = WIFIP2PMATCH_MODE_SELECT_REL_WAIT;
    }
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   募集の取り消し  WIFIP2PMATCH_MODE_SELECT_REL_WAIT
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static int _parentModeSelectRelWait( WIFIP2PMATCH_WORK* wk, int seq )
{
    int i;
    int ret = BmpYesNoSelectMain(wk->pYesNoWork, HEAPID_WIFIP2PMATCH);

	WIFI_MCR_PCAnmMain( &wk->matchroom );	// パソコンアニメメイン

    if((wk->preConnect == -1) && (mydwc_IsNewPlayer() != -1)){  // 接続があった

		// ボイスチャットとBGM音量の関係を整理 tomoya takahashi
		mydwc_setVChat(wk->pMatch->myMatchStatus.vchat);
		OS_TPrintf( "Connect VCHAT set\n" );

		// すでにYesNoSelectMainで解放されてなければ
		if( ret == BMPMENU_NULL ){
			BmpYesNoWinDel( wk->pYesNoWork, HEAPID_WIFIP2PMATCH );
		}
        ret = 1;  // いいえに変更
    }
    if(ret == BMPMENU_NULL){  // まだ選択中
        return seq;
    }else if(ret == 0){ // はいを選択した場合
        //書き込み
        _myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);

		// 通信状態を元に戻す
		CommStateChangeWiFiLogin();

		// 主人公の動作を許可
		FriendRequestWaitOff( wk );
    }
    else{  // いいえを選択した場合

		// 主人公動作停止を再度表示
		FriendRequestWaitOff( wk );
		FriendRequestWaitOn( wk, TRUE );
    }
    wk->seq = WIFIP2PMATCH_MODE_FRIENDLIST;

    return seq;
}



//------------------------------------------------------------------
/**
 * $brief   待機状態になる為の選択メニュー WIFIP2PMATCH_MODE_SELECT_INIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
#define WIFI_STSET_SINGLEBATTLE	(WIFI_STATUS_NONE)		// 選択肢の無いものを割り当てる
#define WIFI_STSET_DOUBLEBATTLE	(WIFI_STATUS_VCT)
#define WIFI_STSET_MINIGAME		(WIFI_STATUS_UNKNOWN)

typedef struct {
    u32  str_id;
    u32  param;
} _infoMenu;

//  メニューリスト
#define WIFI_PARENTINFO_MENULIST_MAX	  ( 10 )
#ifdef WFP2P_DEBUG_EXON 
#define WIFI_PARENTINFO_MENULIST_COMMON	  ( 5 )	// 変動しない部分の数
#else
#define WIFI_PARENTINFO_MENULIST_COMMON	  ( 3 )	// 変動しない部分の数
#endif
_infoMenu _parentInfoMenuList[ WIFI_PARENTINFO_MENULIST_MAX ] = {
	// 変動しない部分
    { msg_wifilobby_057, (u32)WIFI_STSET_SINGLEBATTLE },
    { msg_wifilobby_058, (u32)WIFI_STSET_DOUBLEBATTLE },
    { msg_wifilobby_025, (u32)WIFI_STATUS_TRADE_WAIT },
#ifdef WFP2P_DEBUG_EXON 
    { msg_wifilobby_debug_00, (u32)WIFI_STATUS_BATTLEROOM_WAIT },
    { msg_wifilobby_debug_01, (u32)WIFI_STATUS_MBATTLE_FREE_WAIT },
#endif
};

// 追加リストデータ
static _infoMenu _minigameMenu = {
	msg_wifilobby_147, (u32)WIFI_STSET_MINIGAME
};
static _infoMenu _frontierMenu = {
	msg_wifilobby_138, (u32)WIFI_STATUS_FRONTIER_WAIT
};
static _infoMenu _pofinMenu = {
	msg_wifilobby_100, (u32)WIFI_STATUS_POFIN_WAIT
};
static _infoMenu _endMenu = { 
	msg_wifilobby_032, (u32)BMPLIST_CANCEL
};


// ミニゲーム
_infoMenu _parentMiniGameInfoMenuList[] = {
    { msg_wifilobby_mg02, (u32)WIFI_STATUS_BUCKET_WAIT },
    { msg_wifilobby_mg02, (u32)WIFI_STATUS_BALANCEBALL_WAIT },
    { msg_wifilobby_mg02, (u32)WIFI_STATUS_BALLOON_WAIT },
    { msg_wifilobby_032, (u32)BMPLIST_CANCEL },
};

_infoMenu _parentSingleInfoMenuList[] = {
    { msg_wifilobby_059, (u32)WIFI_STATUS_SBATTLE_FREE_WAIT },
    { msg_wifilobby_060, (u32)WIFI_STATUS_SBATTLE50_WAIT },
    { msg_wifilobby_061, (u32)WIFI_STATUS_SBATTLE100_WAIT },
    { msg_wifilobby_032, (u32)BMPLIST_CANCEL },
};

_infoMenu _parentDoubleInfoMenuList[] = {
    { msg_wifilobby_062, (u32)WIFI_STATUS_DBATTLE_FREE_WAIT },
    { msg_wifilobby_063, (u32)WIFI_STATUS_DBATTLE50_WAIT },
    { msg_wifilobby_064, (u32)WIFI_STATUS_DBATTLE100_WAIT },
    { msg_wifilobby_032, (u32)BMPLIST_CANCEL },
};


///選択メニューのリスト
static BMPLIST_HEADER _parentInfoMenuListHeader = {
    NULL,			// 表示文字データポインタ
    _curCallBack,					// カーソル移動ごとのコールバック関数
    NULL,					// 一列表示ごとのコールバック関数
    NULL,                   //
    WIFI_PARENTINFO_MENULIST_COMMON,// リスト項目数
    WIFI_PARENTINFO_MENULIST_COMMON,// 表示最大項目数
    0,						// ラベル表示Ｘ座標
    8,						// 項目表示Ｘ座標
    0,						// カーソル表示Ｘ座標
    0,						// 表示Ｙ座標
    FBMP_COL_BLACK,			// 文字色
    FBMP_COL_WHITE,			// 背景色
    FBMP_COL_BLK_SDW,		// 文字影色
    0,						// 文字間隔Ｘ
    16,						// 文字間隔Ｙ
    BMPLIST_NO_SKIP,		// ページスキップタイプ
    FONT_SYSTEM,				// 文字指定
    0,						// ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)
    NULL                    // work
};

///選択メニューのリスト
static const BMPLIST_HEADER _parentInfoBattleMenuListHeader = {
    NULL,			// 表示文字データポインタ
    _curCallBack,					// カーソル移動ごとのコールバック関数
    NULL,					// 一列表示ごとのコールバック関数
    NULL,                   //
    NELEMS(_parentSingleInfoMenuList),	// リスト項目数
    NELEMS(_parentSingleInfoMenuList),	// 表示最大項目数
    0,						// ラベル表示Ｘ座標
    8,						// 項目表示Ｘ座標
    0,						// カーソル表示Ｘ座標
    0,						// 表示Ｙ座標
    FBMP_COL_BLACK,			// 文字色
    FBMP_COL_WHITE,			// 背景色
    FBMP_COL_BLK_SDW,		// 文字影色
    0,						// 文字間隔Ｘ
    16,						// 文字間隔Ｙ
    BMPLIST_NO_SKIP,		// ページスキップタイプ
    FONT_SYSTEM,				// 文字指定
    0,						// ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)
    NULL                    // work
};

///選択メニューのリスト
static const BMPLIST_HEADER _parentInfoMiniGameMenuListHeader = {
    NULL,			// 表示文字データポインタ
    _curCallBack,					// カーソル移動ごとのコールバック関数
    NULL,					// 一列表示ごとのコールバック関数
    NULL,                   //
    NELEMS(_parentMiniGameInfoMenuList),	// リスト項目数
    NELEMS(_parentMiniGameInfoMenuList),	// 表示最大項目数
    0,						// ラベル表示Ｘ座標
    8,						// 項目表示Ｘ座標
    0,						// カーソル表示Ｘ座標
    0,						// 表示Ｙ座標
    FBMP_COL_BLACK,			// 文字色
    FBMP_COL_WHITE,			// 背景色
    FBMP_COL_BLK_SDW,		// 文字影色
    0,						// 文字間隔Ｘ
    16,						// 文字間隔Ｙ
    BMPLIST_NO_SKIP,		// ページスキップタイプ
    FONT_SYSTEM,				// 文字指定
    0,						// ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)
    NULL                    // work
};


#define PARENTMENU_Y	( 3 )
static int _parentModeSelectMenuInit( WIFIP2PMATCH_WORK *wk, int seq )
{
    int i,length;
    BMPLIST_HEADER list_h;
	_infoMenu* p_menu;

	// リスト基本情報設定
	length = WIFI_PARENTINFO_MENULIST_COMMON;
	list_h = _parentInfoMenuListHeader;
	p_menu = _parentInfoMenuList;
	
	// 料理の有無
	if( _pofinCaseCheck( wk ) == TRUE ){
		length ++;			//  項目数追加
		list_h.count ++;	// リスト最大項目数追加
		list_h.line ++;		// リスト表示最大数追加

		p_menu[ length-1 ] = _pofinMenu;
	}

	// フロンティアの有無
	if( _frontierInCheck( wk ) == TRUE ){
		length ++;			//  項目数追加
		list_h.count ++;	// リスト最大項目数追加
		list_h.line ++;		// リスト表示最大数追加

		p_menu[ length-1 ] = _frontierMenu;
	}

	// ミニゲームの有無
	if( _miniGameInCheck( wk ) == TRUE ){
		length ++;			//  項目数追加
		list_h.count ++;	// リスト最大項目数追加
		list_h.line ++;		// リスト表示最大数追加

		p_menu[ length-1 ] =_minigameMenu;
	}

	// 終了の追加
	{
		length ++;			//  項目数追加
		list_h.count ++;	// リスト最大項目数追加
		list_h.line ++;		// リスト表示最大数追加

		p_menu[ length-1 ] =_endMenu;
	}

    wk->submenulist = BMP_MENULIST_Create( length , HEAPID_WIFIP2PMATCH );
    for(i=0; i< length ; i++){
        BMP_MENULIST_AddArchiveString( wk->submenulist, wk->MsgManager, p_menu[i].str_id, p_menu[i].param );
    }
    if(GF_BGL_BmpWinAddCheck(&wk->SubListWin)){
        BmpMenuWinClear(&wk->SubListWin,WINDOW_TRANS_ON);
        GF_BGL_BmpWinDel(&wk->SubListWin);
    }
    //BMPウィンドウ生成
    GF_BGL_BmpWinAdd(wk->bgl,&wk->SubListWin,
                     GF_BGL_FRAME2_M, 16, PARENTMENU_Y, 15, length * 2, FLD_SYSFONT_PAL, FLD_MENU_WIN_CGX - length * 2);
    BmpMenuWinWrite(&wk->SubListWin, WINDOW_TRANS_OFF, MENU_WIN_CGX_NUM, MENU_WIN_PAL );
    list_h.list = wk->submenulist;
    list_h.win = &wk->SubListWin;

    wk->sublw = BmpListSet(&list_h, 0, wk->battleCur, HEAPID_WIFIP2PMATCH);
    GF_BGL_BmpWinOnVReq(&wk->SubListWin);


    WifiP2PMatchMessagePrint(wk, msg_wifilobby_006, FALSE);


	WIFI_MCR_PCAnmMain( &wk->matchroom );	// パソコンアニメメイン

    wk->seq = WIFIP2PMATCH_MODE_SELECT_WAIT;
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   待機状態になる為の選択メニュー WIFIP2PMATCH_MODE_SELECT_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _battleSubMenuInit( WIFIP2PMATCH_WORK *wk, int ret );
static int _parentModeSelectMenuWait( WIFIP2PMATCH_WORK *wk, int seq )
{
    u32	ret;
    int num = _getBattlePokeNum(wk);
	int pofin_ret = _isPofinItemCheck(wk);

	WIFI_MCR_PCAnmMain( &wk->matchroom );	// パソコンアニメメイン


    if( GF_MSG_PrintEndCheck( wk->MsgIndex ) != 0 ){

		// ここで接続してくる可能性もある	080617	tomoya
		if( 0 !=  _checkParentNewPlayer(wk)){ // 接続してきた
			wk->seq = WIFIP2PMATCH_MODE_FRIENDLIST;
			BmpMenuWinClear(&wk->SubListWin, WINDOW_TRANS_ON );
			GF_BGL_BmpWinDel(&wk->SubListWin);
			BmpListExit(wk->sublw, NULL, &wk->battleCur);
			BMP_MENULIST_Delete( wk->submenulist );
	        EndMessageWindowOff(wk);
		}
        return seq;
    }
    ret = BmpListMain(wk->sublw);

    if( 0 !=  _checkParentNewPlayer(wk)){ // 接続してきた
        ret = BMPLIST_CANCEL;
    }
    switch(ret){
      case BMPLIST_NULL:
        return seq;
      case BMPLIST_CANCEL:
        wk->seq = WIFIP2PMATCH_MODE_FRIENDLIST;
        EndMessageWindowOff(wk);
        Snd_SePlay(_SE_DESIDE);
        break;
      default:
        Snd_SePlay(_SE_DESIDE);
		// ポフィンアイテムチェック
		if( (ret == WIFI_STATUS_POFIN_WAIT) && (pofin_ret != WFP2P_POFIN_RET_OK) ){
			if( pofin_ret == WFP2P_POFIN_RET_NUTSNONE ){
	            WifiP2PMatchMessagePrint(wk, msg_wifilobby_105, FALSE);			
			}else if( pofin_ret == WFP2P_POFIN_RET_POFINCASENONE ){
				WifiP2PMatchMessagePrint(wk, msg_wifilobby_121, FALSE);
			}else{
				WifiP2PMatchMessagePrint(wk, msg_wifilobby_106, FALSE);
			}
            wk->seq = WIFIP2PMATCH_MODE_DISCONNECT;
		}
		// 交換チェック
		else if((ret == WIFI_STATUS_TRADE_WAIT) && (2 > num )){
            WifiP2PMatchMessagePrint(wk, msg_wifilobby_053, FALSE);
            wk->seq = WIFIP2PMATCH_MODE_DISCONNECT;
        }
		// ダブルバトルチェック
        else if((ret == WIFI_STSET_DOUBLEBATTLE) && (2 > num)){
            WifiP2PMatchMessagePrint(wk, msg_wifilobby_067, FALSE);
            wk->seq = WIFIP2PMATCH_MODE_DISCONNECT;
        }
		// サブリスト処理へ
        else if((ret == WIFI_STSET_SINGLEBATTLE) || 
				(ret == WIFI_STSET_DOUBLEBATTLE) || 
				(ret == WIFI_STSET_MINIGAME) ){
//            _myStatusChange(wk, ret);
            BmpMenuWinClear(&wk->SubListWin, WINDOW_TRANS_ON );
            GF_BGL_BmpWinDel(&wk->SubListWin);
            BmpListExit(wk->sublw, NULL,  &wk->battleCur);
            BMP_MENULIST_Delete( wk->submenulist );
            _battleSubMenuInit(wk, ret);
            wk->seq = WIFIP2PMATCH_MODE_SUBBATTLE_WAIT;
            return seq;
//            EndMessageWindowOff(wk);
        }
        else{
            _myStatusChange(wk, ret);
            wk->seq = WIFIP2PMATCH_MODE_FRIENDLIST;
            EndMessageWindowOff(wk);
        }
        // wifipofinの時は、wifi通信をwifi pofin モードにして
		// ポフィン待合しつへ
		{
			BOOL  msg_on = TRUE;
			
            if( _wait2to4Mode(ret) && (wk->seq == WIFIP2PMATCH_MODE_FRIENDLIST) ){
                wk->seq = WIFIP2PMATCH_PARENT_RESTART;  // 再起動を待つ
                WifiP2PMatch_CommWifiBattleStart( wk, -1, ret );
                _commStateChange( ret );
                _myStatusChange(wk, ret);
                wk->endSeq = _get2to4ModeEndSeq( ret );
				msg_on = FALSE;
			}
            else{
				WifiP2PMatch_CommWifiBattleStart( wk, -1, ret );	// 交換でも通信方法を変更するためにここに移動
                _commStateChange( ret );
/*				↑commStateChangeでよい
				if(ret == WIFI_STATUS_FRONTIER_WAIT){
//					WifiP2PMatch_CommWifiBattleStart( wk, -1, ret );
//
					CommSetWifiBothNet(FALSE);
					CommStateChangeWiFiFactory();
				}
*/
            }

			if( wk->seq == WIFIP2PMATCH_MODE_FRIENDLIST ){
				// 動作停止させる
				FriendRequestWaitOn( wk, msg_on );
			}
		}
        break;
    }
    BmpMenuWinClear(&wk->SubListWin, WINDOW_TRANS_ON );
    GF_BGL_BmpWinDel(&wk->SubListWin);
    BmpListExit(wk->sublw, NULL, &wk->battleCur);
    BMP_MENULIST_Delete( wk->submenulist );

    return seq;

}

//------------------------------------------------------------------
/**
 * $brief   ゲームの内容が決まったので、親として再初期化  WIFIP2PMATCH_PARENT_RESTART
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _parentRestart( WIFIP2PMATCH_WORK *wk, int seq )
{
    u32	ret;

	// エラーチェック
    if( CommStateIsWifiError() ){
        _errorDisp(wk);
        return seq;
    }

	WIFI_MCR_PCAnmMain( &wk->matchroom );	// パソコンアニメメイン
    if( 0 !=  _checkParentNewPlayer(wk)){ // 接続してきた

        wk->seq = WIFIP2PMATCH_MODE_FRIENDLIST;
    }
    // wifipofinの時は、wifi通信をwifi pofin モードにして ポフィン待合しつへ
    if(CommStateIsWifiLoginMatchState()){
//        CommStateChangeWiFiLogin();
//        _myStatusChange(wk, ret);
//

		// 通信エラーチェック
		// 080630	tomoya takahashi
		CommStateSetErrorCheck(FALSE,TRUE);	// 切断はエラーじゃない

        CommSetWifiBothNet(FALSE);
        WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK,
                        COMM_BRIGHTNESS_SYNC, 1, HEAPID_WIFIP2PMATCH);
//        wk->endSeq = _get2to4ModeEndSeq( ret );
        wk->bRetryBattle = FALSE;

        CommInfoInitialize( wk->pSaveData, NULL );

        // 自分を教える
        CommInfoSendPokeData();

        // 自分はエントリー
        CommInfoSetEntry( CommGetCurrentID() );
        seq = SEQ_OUT;						//終了シーケンスへ
    }
    return seq;

}



//------------------------------------------------------------------
/**
 * $brief   バトルの詳細部分を決める WIFIP2PMATCH_MODE_SUBBATTLE_INIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _battleSubMenuInit( WIFIP2PMATCH_WORK *wk, int ret )
{
    int i,length;
    BMPLIST_HEADER list_h;
    _infoMenu* pMenu;
    
    switch(ret){
	case WIFI_STSET_SINGLEBATTLE:
        pMenu = _parentSingleInfoMenuList;
        length = NELEMS(_parentSingleInfoMenuList);
        wk->bSingle = 1;
		list_h = _parentInfoBattleMenuListHeader;
		break;

	case WIFI_STSET_DOUBLEBATTLE:
        pMenu = _parentDoubleInfoMenuList;
        length = NELEMS(_parentDoubleInfoMenuList);
        wk->bSingle = 0;
	    list_h = _parentInfoBattleMenuListHeader;
		break;

	case WIFI_STSET_MINIGAME:
        pMenu = _parentMiniGameInfoMenuList;
        length = NELEMS(_parentMiniGameInfoMenuList);
		list_h = _parentInfoMiniGameMenuListHeader;
		wk->bSingle = 2;
		break;
    }

    wk->submenulist = BMP_MENULIST_Create( length , HEAPID_WIFIP2PMATCH );
    for(i=0; i< length ; i++){
		if( pMenu[i].str_id != msg_wifilobby_mg02 ){
	        BMP_MENULIST_AddArchiveString( wk->submenulist, wk->MsgManager, pMenu[i].str_id, pMenu[i].param );
		}else{
			// ミニゲームはタグで表示する（ちょっとむりやりすぎ・・・）
			WORDSET_RegisterWiFiLobbyGameName( wk->WordSet, 0, i );
			MSGMAN_GetString(  wk->MsgManager, pMenu[i].str_id, wk->pExpStrBuf );
			WORDSET_ExpandStr( wk->WordSet, wk->TitleString, wk->pExpStrBuf );
			BMP_MENULIST_AddString( wk->submenulist, wk->TitleString, pMenu[i].param );
		}
    }
    if(GF_BGL_BmpWinAddCheck(&wk->SubListWin)){
        BmpMenuWinClear(&wk->SubListWin, WINDOW_TRANS_ON );
        GF_BGL_BmpWinDel(&wk->SubListWin);
    }
    //BMPウィンドウ生成
    GF_BGL_BmpWinAdd(wk->bgl,&wk->SubListWin,
                     GF_BGL_FRAME2_M, 16, 9, 15, length * 2, FLD_SYSFONT_PAL, FLD_MENU_WIN_CGX - length * 2);
    BmpMenuWinWrite(&wk->SubListWin, WINDOW_TRANS_OFF, MENU_WIN_CGX_NUM, MENU_WIN_PAL );
    list_h.list = wk->submenulist;
    list_h.win = &wk->SubListWin;
    wk->sublw = BmpListSet(&list_h, 0, wk->singleCur[wk->bSingle], HEAPID_WIFIP2PMATCH);
    GF_BGL_BmpWinOnVReq(&wk->SubListWin);

//    WifiP2PMatchMessagePrint(wk, msg_wifilobby_006, FALSE);

//    wk->seq = WIFIP2PMATCH_MODE_SELECT_WAIT;
    return TRUE;
}



//------------------------------------------------------------------
/**
 * $brief   待機状態になる為の選択メニュー WIFIP2PMATCH_MODE_SUBBATTLE_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _parentModeSubSelectMenuWait( WIFIP2PMATCH_WORK *wk, int seq )
{
    u32	ret;

	WIFI_MCR_PCAnmMain( &wk->matchroom );	// パソコンアニメメイン

	// ここで接続してくる可能性もある	080617	tomoya
	if( 0 !=  _checkParentNewPlayer(wk)){ // 接続してきた
		wk->seq = WIFIP2PMATCH_MODE_FRIENDLIST;
		BmpMenuWinClear(&wk->SubListWin, WINDOW_TRANS_ON );
		GF_BGL_BmpWinDel(&wk->SubListWin);
		BmpListExit(wk->sublw, NULL, &wk->singleCur[wk->bSingle]);
		BMP_MENULIST_Delete( wk->submenulist );
		EndMessageWindowOff(wk);
		return seq;
	}

    if( GF_MSG_PrintEndCheck( wk->MsgIndex ) != 0 ){
		return seq;
    }
    ret = BmpListMain(wk->sublw);
    switch(ret){
      case BMPLIST_NULL:
        return seq;
      case BMPLIST_CANCEL:
        Snd_SePlay(_SE_DESIDE);
        wk->seq = WIFIP2PMATCH_MODE_SELECT_INIT;
        break;
      default:
        Snd_SePlay(_SE_DESIDE);
        _myStatusChange(wk, ret);
        wk->seq = WIFIP2PMATCH_MODE_FRIENDLIST;
        EndMessageWindowOff(wk);
        break;
    }


	// wifipofinの時は、wifi通信をwifi pofin モードにして
	// ポフィン待合しつへ
//	{
	if( wk->seq == WIFIP2PMATCH_MODE_FRIENDLIST ){	// 何か選択したときのみ、CANCELはだめ
		BOOL msg_on = TRUE;
		if( _wait2to4Mode(ret) ){
            wk->seq = WIFIP2PMATCH_PARENT_RESTART;  // 再起動を待つ

			WifiP2PMatch_CommWifiBattleStart( wk, -1, ret );

			_commStateChange(ret);
            _myStatusChange(wk, ret);
            wk->endSeq = _get2to4ModeEndSeq( ret );
			msg_on = FALSE;

        }
        else {
			WifiP2PMatch_CommWifiBattleStart( wk, -1, ret );
            CommStateChangeWiFiBattle();
        }
		// 動作停止させる
		FriendRequestWaitOn( wk, msg_on );
    //    _commStateChange(ret);
	}


	
    BmpMenuWinClear(&wk->SubListWin, WINDOW_TRANS_ON );
    GF_BGL_BmpWinDel(&wk->SubListWin);
    BmpListExit(wk->sublw, NULL, &wk->singleCur[wk->bSingle]);
    BMP_MENULIST_Delete( wk->submenulist );
    return seq;

}

//------------------------------------------------------------------
/**
 * $brief   マッチング申し込み WIFIP2PMATCH_MODE_MATCH_INIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

enum{
    _CONNECTING = 1,
    _INFOVIEW,
};


static const struct {
    u32  str_id;
    u32  param;
}_childMatchMenuList[] = {
    { msg_wifilobby_030, (u32)_CONNECTING },
//    { msg_wifilobby_031, (u32)_INFOVIEW },
    { msg_wifilobby_032, (u32)BMPLIST_CANCEL },
};

///選択メニューのリスト
static const BMPLIST_HEADER _childMatchMenuListHeader = {
    NULL,			// 表示文字データポインタ
    _curCallBack,					// カーソル移動ごとのコールバック関数
    NULL,					// 一列表示ごとのコールバック関数
    NULL,                   //
    NELEMS(_childMatchMenuList),	// リスト項目数
    NELEMS(_childMatchMenuList),	// 表示最大項目数
    0,						// ラベル表示Ｘ座標
    8,						// 項目表示Ｘ座標
    0,						// カーソル表示Ｘ座標
    0,						// 表示Ｙ座標
    FBMP_COL_BLACK,			// 文字色
    FBMP_COL_WHITE,			// 背景色
    FBMP_COL_BLK_SDW,		// 文字影色
    0,						// 文字間隔Ｘ
    16,						// 文字間隔Ｙ
    BMPLIST_NO_SKIP,		// ページスキップタイプ
    FONT_SYSTEM,				// 文字指定
    0,						// ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)
    NULL                    // work
    };


static int _childModeMatchMenuInit( WIFIP2PMATCH_WORK *wk, int seq )
{
	int gmmno;
	u16 friendNo,status;
	_WIFI_MACH_STATUS* p_status;
	MCR_MOVEOBJ* p_player;
	MCR_MOVEOBJ* p_npc;
	u32 way;
	
	friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
	//NPCを自分の方向に向ける
	p_player = MCRSYS_GetMoveObjWork( wk, 0 );
	GF_ASSERT( p_player );

	p_npc = MCRSYS_GetMoveObjWork( wk, friendNo );
	// 相手がいなくなったり
	// 相手のステータスが違うものに変わったら、
	// 表示を消して元に戻る
	if( p_npc == NULL ){
		_friendNameExpand(wk, friendNo - 1);
		WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
		wk->seq = WIFIP2PMATCH_MODE_DISCONNECT;
		return seq;
	}

	way = WIFI_MCR_GetRetWay( p_player );
	WIFI_MCR_NpcPauseOn( &wk->matchroom, p_npc, way );


	// 状態保存
	p_status = WifiFriendMatchStatusGet( wk, friendNo - 1 );
	status = _WifiMyStatusGet( wk, p_status );
	wk->keepStatus = status;


    _friendNameExpand(wk, friendNo - 1);
    if(status == WIFI_STATUS_TRADE_WAIT){
        gmmno = msg_wifilobby_004;
    }
    else if(status == WIFI_STATUS_POFIN_WAIT){
        gmmno = msg_wifilobby_141;
    }
    else if(status == WIFI_STATUS_POFIN){
        gmmno = msg_wifilobby_104;
    }
    else if(status == WIFI_STATUS_FRONTIER_WAIT){
        gmmno = msg_wifilobby_137;
    }
    else if(status == WIFI_STATUS_FRONTIER){
        gmmno = msg_wifilobby_140;
    }
#ifdef WFP2P_DEBUG_EXON 
    else if(status == WIFI_STATUS_BATTLEROOM_WAIT){
        gmmno = msg_wifilobby_debug_00;
    }
    else if(status == WIFI_STATUS_BATTLEROOM){
        gmmno = msg_wifilobby_debug_00;
    }
#endif
    else if(status == WIFI_STATUS_BUCKET_WAIT){
		WORDSET_RegisterWiFiLobbyGameName( wk->WordSet, 1, WFLBY_GAME_BALLSLOW );
        gmmno = msg_wifilobby_144;
    }
    else if(status == WIFI_STATUS_BUCKET){
        gmmno = msg_wifilobby_149;
    }
    else if(status == WIFI_STATUS_BALANCEBALL_WAIT){
		WORDSET_RegisterWiFiLobbyGameName( wk->WordSet, 1, WFLBY_GAME_BALANCEBALL );
        gmmno = msg_wifilobby_144;
    }
    else if(status == WIFI_STATUS_BALANCEBALL){
        gmmno = msg_wifilobby_149;
    }
    else if(status == WIFI_STATUS_BALLOON_WAIT){
		WORDSET_RegisterWiFiLobbyGameName( wk->WordSet, 1, WFLBY_GAME_BALLOON );
        gmmno = msg_wifilobby_144;
    }
    else if(status == WIFI_STATUS_BALLOON){
        gmmno = msg_wifilobby_149;
    }
    else if(_modeBattleWait(status)){
        gmmno = msg_wifilobby_003;
    }
    else if(status == WIFI_STATUS_TRADE){
        gmmno = msg_wifilobby_049;
    }
    else if(_modeBattle(status)){
        gmmno = msg_wifilobby_048;
    }
    else if(status == WIFI_STATUS_VCT){
        gmmno = msg_wifilobby_050;
    }
    else{
        gmmno = msg_wifilobby_005;
    }
    WifiP2PMatchMessagePrint(wk, gmmno, FALSE);
    wk->seq = WIFIP2PMATCH_MODE_MATCH_INIT2;
    return seq;
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージの終了を見て、リストを出します
 */
//-----------------------------------------------------------------------------
static int _childModeMatchMenuInit2( WIFIP2PMATCH_WORK *wk, int seq )
{
	MCR_MOVEOBJ* p_npc;
	u32 checkMatch;
	int friendNo, status;
	_WIFI_MACH_STATUS* p_status;


	// エラーチェック
    if( CommStateIsWifiError() ){
//        wk->localTime=0;
        _errorDisp(wk);
        return seq;
    }

	// 話しかけている友達ナンバー取得
	friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
	p_npc = MCRSYS_GetMoveObjWork( wk, friendNo );

    checkMatch = _checkParentNewPlayer(wk);
    if( 0 !=  checkMatch ){ // 接続してきた
		// NPCを元に戻す
		WIFI_MCR_NpcPauseOff( &wk->matchroom, p_npc );
        EndMessageWindowOff(wk);
		wk->seq = WIFIP2PMATCH_MODE_FRIENDLIST;
		return seq;	
    }

	// 相手がいなくなったり
	// 相手のステータスが違うものに変わったら、
	// 表示を消して元に戻る
	if( p_npc == NULL ){

		// NPCを元に戻す
		WIFI_MCR_NpcPauseOff( &wk->matchroom, p_npc );

		BmpListExit( wk->sublw, NULL, NULL );
		_friendNameExpand(wk, friendNo - 1);
		WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
		wk->seq = WIFIP2PMATCH_MODE_DISCONNECT;
		return seq;
	}else{
		p_status = WifiFriendMatchStatusGet( wk, friendNo - 1 );
		status = _WifiMyStatusGet( wk, p_status );

		// 状態がかわったり
		if((wk->keepStatus != status)){

			// NPCを元に戻す
			WIFI_MCR_NpcPauseOff( &wk->matchroom, p_npc );

			_friendNameExpand(wk, friendNo - 1);
			WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
			wk->seq = WIFIP2PMATCH_MODE_DISCONNECT;
			return seq;
		}
	}
	
	
    if( GF_MSG_PrintEndCheck( wk->MsgIndex ) != 0 ){
        return seq;
    }

	{
		int i,length;
		BMPLIST_HEADER list_h;
		u16 friendNo,status,vchat;
		_WIFI_MACH_STATUS* p_status;

		length = NELEMS(_childMatchMenuList);
		list_h = _childMatchMenuListHeader;
		friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
		p_status = WifiFriendMatchStatusGet( wk, friendNo - 1 );
		status = _WifiMyStatusGet( wk, p_status );
		vchat = p_status->vchat;
		wk->keepVChat = vchat;
		vchat += wk->pMatch->myMatchStatus.vchat;

		// その人の情報を表示
	//	WifiP2PMatch_UserDispOn_MyAcces( wk, friendNo, HEAPID_WIFIP2PMATCH );


		wk->submenulist = BMP_MENULIST_Create( length , HEAPID_WIFIP2PMATCH );
		i = 0;

		for(i = 0; i < NELEMS(_childMatchMenuList);i++){
			if(i == 0){
				if(_modeActive(status) || (status == WIFI_STATUS_NONE) ||
					(status == WIFI_STATUS_PLAY_OTHER) || 
				   (status >= WIFI_STATUS_UNKNOWN)){  // もうしこむを出さない条件
					list_h.line -= 1;
					list_h.count -= 1;
					length -= 1;
				}
				else if(status == WIFI_STATUS_LOGIN_WAIT){
					if(vchat == 2){
						BMP_MENULIST_AddArchiveString( wk->submenulist, wk->MsgManager,
													   msg_wifilobby_027,
													   _childMatchMenuList[i].param );
					}
					else{
						list_h.line -= 1;
						list_h.count -= 1;
						length -= 1;
					}
				}
				else{
					BMP_MENULIST_AddArchiveString( wk->submenulist, wk->MsgManager,
												   _childMatchMenuList[i].str_id,
												   _childMatchMenuList[i].param );
				}
			}
			else{
				BMP_MENULIST_AddArchiveString( wk->submenulist, wk->MsgManager,
											   _childMatchMenuList[i].str_id,
											   _childMatchMenuList[i].param );
			}
		}
		//BMPウィンドウ生成
		GF_BGL_BmpWinAdd(wk->bgl,&wk->SubListWin,
						 GF_BGL_FRAME2_M, 16, 11+ ((3-length)*2), 15  , length * 2,
						 FLD_SYSFONT_PAL, FLD_MENU_WIN_CGX);
		BmpMenuWinWrite(&wk->SubListWin, WINDOW_TRANS_OFF, MENU_WIN_CGX_NUM, MENU_WIN_PAL );
		list_h.list = wk->submenulist;
		list_h.win = &wk->SubListWin;
		wk->sublw = BmpListSet(&list_h, 0, 0, HEAPID_WIFIP2PMATCH);
		GF_BGL_BmpWinOnVReq(&wk->SubListWin);
	}

    wk->seq = WIFIP2PMATCH_MODE_MATCH_WAIT;
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   未知のアイテム検査
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static BOOL _isItemCheck(_WIFI_MACH_STATUS* pTargetStatus)
{
    int i;
    
    for(i = 0; i < _POKEMON_NUM; i++){
        if(MONSNO_DAMETAMAGO == pTargetStatus->pokemonType[i]){
            return FALSE;
        }
        if(MONSNO_MAX < pTargetStatus->pokemonType[i]){
            return FALSE;
        }
        if(ITEM_DATA_MAX < pTargetStatus->hasItemType[i]){
            return FALSE;
        }
    }
    return TRUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	ポフィンアイテムチェック
 *
 *	@param	wk		システムワーク
 *
 *	@retval	WFP2P_POFIN_RET_OK,			大丈夫
 *	@retval	WFP2P_POFIN_RET_NUTSNONE,	木の実がない
 *	@retval	WFP2P_POFIN_RET_POFINMAX,	ポフィンケースがいっぱい
 */
//-----------------------------------------------------------------------------
static u32 _isPofinItemCheck(WIFIP2PMATCH_WORK* wk)
{
	//木の実を持っているかチェック
	if(!MyItem_CheckItemPocket(SaveData_GetMyItem(wk->pSaveData),BAG_POKE_NUTS)){
		return WFP2P_POFIN_RET_NUTSNONE;
	}
	// ポルトケースがあるかチェック
	if( !MyItem_GetItemNum( SaveData_GetMyItem(wk->pSaveData),ITEM_POFINKEESU,HEAPID_WIFIP2PMATCH ) ){
		return WFP2P_POFIN_RET_POFINCASENONE;
	}
	//ポルトケースがいっぱいでないかチェック
	if(PORUTO_GetDataNum(SaveData_GetPorutoBlock(wk->pSaveData)) >= PORUTO_STOCK_MAX){
		return WFP2P_POFIN_RET_POFINMAX;
	}
	return WFP2P_POFIN_RET_OK;
}


//----------------------------------------------------------------------------
/**
 *	@brief	ロビー用のBGMを設定する
 */
//-----------------------------------------------------------------------------
static void WifiP2P_SetLobbyBgm( void )
{
	u32 bgm;

	// ボイスチャットなしにする
	mydwc_setVChat(FALSE);		// ボイスチャットとBGM音量の関係を整理 tomoya takahashi

	if( WifiP2P_CheckLobbyBgm() == FALSE ){

		if( GF_RTC_IsNightTime() == FALSE ){			//FALSE=昼、TRUE=夜
			bgm = SEQ_PC_01;
		}else{
			bgm = SEQ_PC_02;
		}
		Snd_SceneSet( SND_SCENE_DUMMY );
		Snd_DataSetByScene( SND_SCENE_P2P, bgm, 1 );	//wifiロビー再生
	}else{

		// もうＢＧＭ流れていても、音量だけ元に戻す
		Snd_PlayerSetInitialVolumeBySeqNo( Snd_NowBgmNoGet(), BGM_WIFILOBBY_VOL );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ロビー用のBGMが再生中か取得する
 *
 *	@retval	TRUE	再生中
 *	@retval	FALSE	他のBGMが再生中
 */
//-----------------------------------------------------------------------------
static BOOL WifiP2P_CheckLobbyBgm( void )
{
	u32 now_bgm;

	now_bgm = Snd_NowBgmNoGet();

	if( (now_bgm != SEQ_PC_01) &&
		(now_bgm != SEQ_PC_02) ){
		return FALSE;
	}
	return TRUE;
}

	

//----------------------------------------------------------------------------
/**
 *	@brief	ボウケンノートデータ設定	友達と遊んだ
 *
 *	@param	wk	ワーク
 *	@param	idx	インデックス
 */
//-----------------------------------------------------------------------------
static void WifiP2P_Fnote_Set( WIFIP2PMATCH_WORK* wk, u32 idx )
{
	void* p_buf;
	FNOTE_DATA* p_fnote;

	p_fnote		= SaveData_GetFNote( wk->pSaveData );
	p_buf = FNOTE_SioIDOnlyDataMake( HEAPID_WIFIP2PMATCH, FNOTE_ID_PL_WIFICLUB );
	FNOTE_DataSave( p_fnote, p_buf, FNOTE_TYPE_SIO );
}

//------------------------------------------------------------------
/**
 * $brief   つなぎに行く選択メニュー WIFIP2PMATCH_MODE_MATCH_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _childModeMatchMenuWait( WIFIP2PMATCH_WORK *wk, int seq )
{
    u32	ret;
    int status,friendNo,message = 0,vchat;
    u16 mainCursor;
    int checkMatch;
	MCR_MOVEOBJ* p_npc;
	_WIFI_MACH_STATUS* p_status;

    ret = BmpListMain(wk->sublw);

    checkMatch = _checkParentNewPlayer(wk);
    if( 0 !=  checkMatch ){ // 接続してきた
        ret = BMPLIST_CANCEL;
    }

	// 話しかけている友達ナンバー取得
	friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
	p_npc = MCRSYS_GetMoveObjWork( wk, friendNo );


	// エーラーチェック
    if(CommStateIsWifiError()){
		BmpMenuWinClear(&wk->SubListWin, WINDOW_TRANS_ON );
		GF_BGL_BmpWinDel(&wk->SubListWin);
		BmpListExit(wk->sublw, NULL, NULL);
		BMP_MENULIST_Delete( wk->submenulist );

		if( p_npc != NULL ){
			WIFI_MCR_NpcPauseOff( &wk->matchroom, p_npc );
		}

        _errorDisp(wk);
		return seq;
    }
	

    switch(ret){
      case BMPLIST_NULL:

		// 相手がいなくなったり
		// 相手のステータスが違うものに変わったら、
		// 表示を消して元に戻る
		if( p_npc == NULL ){
			BmpListExit( wk->sublw, NULL, NULL );
			_friendNameExpand(wk, friendNo - 1);
			WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
			wk->seq = WIFIP2PMATCH_MODE_DISCONNECT;
			message = 1;
			break;
		}else{
			p_status = WifiFriendMatchStatusGet( wk, friendNo - 1 );
			status = _WifiMyStatusGet( wk, p_status );
			vchat = p_status->vchat;

			// 状態がかわったり
			// 4人募集のゲームじゃないのに、VCHATフラグが変わったら切断
			if((wk->keepStatus != status) || 
				(( _wait2to4Mode(status) == FALSE ) && (wk->keepVChat != vchat)) ){

				// NPCを元に戻す
				// ここでやってしまうとPauseOffを２重でよんでしまう
//				WIFI_MCR_NpcPauseOff( &wk->matchroom, p_npc );

				_friendNameExpand(wk, friendNo - 1);
				WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
				wk->seq = WIFIP2PMATCH_MODE_DISCONNECT;
				message = 1;
				break;
			}
		}
        return seq;

      case BMPLIST_CANCEL:
        Snd_SePlay(_SE_DESIDE);

        wk->seq = WIFIP2PMATCH_MODE_FRIENDLIST;
        break;
      default:
        Snd_SePlay(_SE_DESIDE);
        if(ret == _CONNECTING){  // 子機が親機に接続
            wk->seq = WIFIP2PMATCH_MODE_FRIENDLIST;
//            BmpListDirectPosGet(wk->lw,&mainCursor);
//		    friendNo = wk->index2No[mainCursor];
            wk->friendNo = friendNo;
            if(  friendNo != 0 ){
                int num = _getBattlePokeNum(wk);
				int pofin_check = _isPofinItemCheck( wk );

				p_status = WifiFriendMatchStatusGet( wk, friendNo - 1 );
                status = _WifiMyStatusGet( wk, p_status );
                vchat = p_status->vchat;


				// 状態がかわったり
				// 4人募集のゲームじゃないのに、VCHATフラグが変わったら切断
				if((wk->keepStatus != status) || 
					(( _wait2to4Mode(status) == FALSE ) && (wk->keepVChat != vchat)) ){
                    _friendNameExpand(wk, friendNo - 1);
                    WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
                    wk->seq = WIFIP2PMATCH_MODE_DISCONNECT;
                    message = 1;
                    break;
                }
				// 不正アイテムチェック
                if(!_isItemCheck(p_status)){
                    _friendNameExpand(wk, friendNo - 1);
                    WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
                    wk->seq = WIFIP2PMATCH_MODE_DISCONNECT;
                    message = 1;
                    break;
                }
				// モンスター２対チェック
                if(_is2pokeMode(status) && (2 > num)){
                    if(WIFI_STATUS_TRADE_WAIT==status){
                        WifiP2PMatchMessagePrint(wk, msg_wifilobby_053, FALSE);
                    }
                    else{
                        WifiP2PMatchMessagePrint(wk, msg_wifilobby_067, FALSE);
                    }
                    wk->seq = WIFIP2PMATCH_MODE_DISCONNECT;
                    message = 1;
                    break;
                }
				// ポフィンケースチェック
				if( (status == WIFI_STATUS_POFIN_WAIT) && (pofin_check != WFP2P_POFIN_RET_OK) ){
					if( pofin_check == WFP2P_POFIN_RET_NUTSNONE ){
                        WifiP2PMatchMessagePrint(wk, msg_wifilobby_105, FALSE);
					}else if( pofin_check == WFP2P_POFIN_RET_POFINCASENONE ){
                        WifiP2PMatchMessagePrint(wk, msg_wifilobby_121, FALSE);
					}else{
                        WifiP2PMatchMessagePrint(wk, msg_wifilobby_106, FALSE);
					}
                    wk->seq = WIFIP2PMATCH_MODE_DISCONNECT;
                    message = 1;
					break;
				}
				// VCTモードチェック
				// 2～4人マッチングの場合VCTモードを合わせる必要はなし
				if( _wait2to4Mode( status ) == FALSE ){
					
					if(vchat != wk->pMatch->myMatchStatus.vchat){  // 自分のVCHATと状態が違う場合メッセージ
						if(vchat){
							WifiP2PMatchMessagePrint(wk, msg_wifilobby_069, FALSE);
						}
						else{
							WifiP2PMatchMessagePrint(wk, msg_wifilobby_070, FALSE);
						}

						// 080703	tomoya takahashi
						// VCHATのON・OFFを聞く箇所にいったら
						// もう応募中ということにして、
						// ３者の応募ができないようにする
                        _myStatusChange(wk, _convertState(status));  // 
						
						wk->seq = WIFIP2PMATCH_MODE_VCHAT_NEGO;
						message = 1;
						break;
					}
				}
                status = _convertState(status);
                if(WIFI_STATUS_UNKNOWN == status){
                    break;
                }
                if( WifiDwc_getFriendStatus(friendNo - 1) == DWC_STATUS_MATCH_SC_SV ){
					TOMOYA_PRINT( "wifi接続先 %d\n", friendNo - 1 );

                    if( WifiP2PMatch_CommWifiBattleStart( wk, friendNo - 1, status ) ){
                        wk->cancelEnableTimer = _CANCELENABLE_TIMER;
						_commStateChange(status);
                        _myStatusChange(wk, status);  // 接続中になる
                        _friendNameExpand(wk, friendNo - 1);
                        WifiP2PMatchMessagePrint(wk,msg_wifilobby_014, FALSE);
						GF_ASSERT( wk->timeWaitWork == NULL );
                        wk->timeWaitWork = TimeWaitIconAdd( &wk->MsgWin, COMM_TALK_WIN_CGX_NUM );
                        if(status != WIFI_STATUS_VCT){
                            wk->seq = WIFIP2PMATCH_MODE_MATCH_LOOP;  // マッチングへ
                        }
                        else{
                            wk->cancelEnableTimer = _CANCELENABLE_TIMER;
                            wk->seq = WIFIP2PMATCH_MODE_VCT_CONNECT_INIT2;  //VCT専用へ
                        }
                        message = 1;
                    }else{
						_friendNameExpand(wk, friendNo - 1);
						WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
						wk->seq = WIFIP2PMATCH_MODE_DISCONNECT;
						message = 1;
					}
                }else{

					_friendNameExpand(wk, friendNo - 1);
					WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
					wk->seq = WIFIP2PMATCH_MODE_DISCONNECT;
					message = 1;
					break;
				}
            }
        }
        else if(ret == _INFOVIEW){
            wk->seq = WIFIP2PMATCH_MODE_PERSONAL_INIT;
        }
        break;
    }


	// 後始末
	
    if(message==0){
        EndMessageWindowOff(wk);
    }
    BmpMenuWinClear(&wk->SubListWin, WINDOW_TRANS_ON );
    GF_BGL_BmpWinDel(&wk->SubListWin);
    BmpListExit(wk->sublw, NULL, NULL);
    BMP_MENULIST_Delete( wk->submenulist );

	// NPCを元に戻す
	if( p_npc != NULL ){
		WIFI_MCR_NpcPauseOff( &wk->matchroom, p_npc );
	}

	// 戻るだけなら人の情報を消す
	if( wk->seq == WIFIP2PMATCH_MODE_FRIENDLIST ){
		
		// その人の情報を消す
//		WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );
		
	}else{

		// 繋がりにいくとき

	}

    return seq;
}


//WIFIP2PMATCH_MODE_MATCH_LOOP
static int _childModeMatchMenuLoop( WIFIP2PMATCH_WORK *wk, int seq )
{
	int status;
	

    wk->cancelEnableTimer--;
    if(wk->cancelEnableTimer < 0  ){
        wk->seq = WIFIP2PMATCH_MODE_CANCEL_ENABLE_WAIT;
    }
    else if(CommWifiIsMatched() == 3 ){
        _friendNameExpand(wk, mydwc_getFriendIndex());
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
        wk->seq = WIFIP2PMATCH_MODE_VCT_DISCONNECT;
    }
    else if(CommWifiIsMatched() == 5){
        _friendNameExpand(wk, mydwc_getFriendIndex());
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_012, FALSE);
        wk->seq = WIFIP2PMATCH_MODE_VCT_DISCONNECT;
    }
    else if((CommWifiIsMatched() == 4) || (CommStateIsWifiDisconnect())){
        _friendNameExpand(wk, mydwc_getFriendIndex());
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
        wk->seq = WIFIP2PMATCH_MODE_VCT_DISCONNECT;
    }
    else if(CommStateIsWifiError()){
        _errorDisp(wk);
    }
    else if(CommWifiIsMatched()==1){  // 相手に接続した

		// ２～４人募集でないとき
		status = _WifiMyStatusGet( wk, &wk->pMatch->myMatchStatus );
		if( _wait2to4Mode( status ) == FALSE ){
	        
			wk->seq = WIFIP2PMATCH_MODE_CALL_YESNO; // 子機親機共通
		//    _timeWaitIconDel(wk);		timeWait内でMsgWinを破棄しているということはメッセージ終了でもOK↓
			EndMessageWindowOff(wk);
			CommInfoInitialize(wk->pSaveData,NULL);   //Info初期化
			wk->timer = 30;
			
		}else if( CommIsConnect(COMM_PARENT_ID) == TRUE ){

			// メッセージ停止
			EndMessageWindowOff( wk );

			CommInfoInitialize(wk->pSaveData,NULL);   //Info初期化

	
			// 通信エラーチェック
			// 080630 tomoya takahashi
			CommStateSetErrorCheck(FALSE,TRUE);	// 切断はエラーじゃない


			// ２～４人募集の時
			// ポフィンなら通信方式をポフィン用にする
            _commStateChange(status);
            wk->endSeq = _get2to4ModeEndSeq( status );
			CommSetWifiBothNet(FALSE); // wifiの通信を同期から非同期に
			WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK,
							COMM_BRIGHTNESS_SYNC, 1, HEAPID_WIFIP2PMATCH);
            wk->bRetryBattle = FALSE;
			seq = SEQ_OUT;						//終了シーケンスへ
		}else{

			// 通信バグ:324	
			// Matched()は1が帰ってきて、CommIsConnectではFALSEが帰ってきている
			// WiFiP2P通信が可能な状態なのに親と接続していないならエラー処理へ
			if( CommGetWifiConnect() == TRUE ){
				_friendNameExpand(wk, mydwc_getFriendIndex());
				WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
				wk->seq = WIFIP2PMATCH_MODE_VCT_DISCONNECT;
			}
		}
    }
    return seq;
}

//-------------------------------------VCTのキャンセル機能を急遽実装
//-------------------------------------VCTのキャンセル機能を急遽取り外し

//------------------------------------------------------------------
/**
 * $brief   Bキャンセル  WIFIP2PMATCH_MODE_BCANCEL_YESNO_VCT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_BCancelYesNoVCT( WIFIP2PMATCH_WORK *wk, int seq )
{
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   Bキャンセル  WIFIP2PMATCH_MODE_BCANCEL_WAIT_VCT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_BCancelWaitVCT( WIFIP2PMATCH_WORK *wk, int seq )
{
    return seq;
}


//------------------------------------------------------------------
/**
 * $brief   DWC切断 WIFIP2PMATCH_MODE_CANCEL_ENABLE_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_CancelEnableWait( WIFIP2PMATCH_WORK *wk, int seq )
{
    _myVChatStatusOrgSet(wk);
    _myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);
    CommStateWifiMatchEnd();
    wk->preConnect = -1;
    wk->seq = WIFIP2PMATCH_MODE_FRIENDLIST;
    // 主人公の動作を許可
    FriendRequestWaitOff( wk );
    EndMessageWindowOff(wk);
    return seq;
}


//------------------------------------------

//------------------------------------------------------------------
/**
 * $brief   Bキャンセル  WIFIP2PMATCH_MODE_BCANCEL_YESNO
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_BCancelYesNo( WIFIP2PMATCH_WORK *wk, int seq )        // WIFIP2PMATCH_MODE_VCT_CONNECT
{
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   Bキャンセル  WIFIP2PMATCH_MODE_BCANCEL_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_BCancelWait( WIFIP2PMATCH_WORK *wk, int seq )        // WIFIP2PMATCH_MODE_VCT_CONNECT
{
    return seq;
}


//------------------------------------------------------------------
/**
 * $brief   親機受付まち。子機から応募があったことを知らせる
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _parentModeCallMenuInit( WIFIP2PMATCH_WORK *wk, int seq )
{
    int mySt;
    int targetSt;
	_WIFI_MACH_STATUS* p_status;
	int myvchat;

	p_status = WifiFriendMatchStatusGet( wk, mydwc_getFriendIndex() );
    mySt = _WifiMyStatusGet( wk,&wk->pMatch->myMatchStatus );
    targetSt = _WifiMyStatusGet( wk,p_status );
	myvchat	= wk->pMatch->myMatchStatus.vchat;
    
    if((mySt == WIFI_STATUS_DBATTLE50_WAIT)&&(targetSt == WIFI_STATUS_DBATTLE50)){
        wk->seq = WIFIP2PMATCH_MODE_CALL_YESNO;  //ok
    }
    else if((mySt == WIFI_STATUS_DBATTLE100_WAIT)&&(targetSt == WIFI_STATUS_DBATTLE100)){
        wk->seq = WIFIP2PMATCH_MODE_CALL_YESNO;  //ok
    }
    else if((mySt == WIFI_STATUS_DBATTLE_FREE_WAIT)&&(targetSt == WIFI_STATUS_DBATTLE_FREE)){
        wk->seq = WIFIP2PMATCH_MODE_CALL_YESNO;  //ok
    }
#ifdef WFP2P_DEBUG_EXON 
    else if((mySt == WIFI_STATUS_MBATTLE_FREE_WAIT)&&(targetSt == WIFI_STATUS_MBATTLE_FREE)){
        wk->seq = WIFIP2PMATCH_MODE_CALL_YESNO;  //ok
    }
#endif
    else if((mySt == WIFI_STATUS_SBATTLE50_WAIT)&&(targetSt == WIFI_STATUS_SBATTLE50)){
        wk->seq = WIFIP2PMATCH_MODE_CALL_YESNO;  //ok
    }
    else if((mySt == WIFI_STATUS_SBATTLE100_WAIT)&&(targetSt == WIFI_STATUS_SBATTLE100)){
        wk->seq = WIFIP2PMATCH_MODE_CALL_YESNO;  //ok
    }
    else if((mySt == WIFI_STATUS_SBATTLE_FREE_WAIT)&&(targetSt == WIFI_STATUS_SBATTLE_FREE)){
        wk->seq = WIFIP2PMATCH_MODE_CALL_YESNO;  //ok
    }
    else if((mySt == WIFI_STATUS_TRADE_WAIT)&&(targetSt == WIFI_STATUS_TRADE)){
        wk->seq = WIFIP2PMATCH_MODE_CALL_YESNO;  //ok
    }
    else if((mySt == WIFI_STATUS_POFIN_WAIT)&&(targetSt == WIFI_STATUS_POFIN)){
        wk->seq = WIFIP2PMATCH_MODE_CALL_YESNO;  //ok
    }
    else if((mySt == WIFI_STATUS_FRONTIER_WAIT)&&(targetSt == WIFI_STATUS_FRONTIER)){
        wk->seq = WIFIP2PMATCH_MODE_CALL_YESNO;  //ok
    }
#ifdef WFP2P_DEBUG_EXON 
    else if((mySt == WIFI_STATUS_BATTLEROOM_WAIT)&&(targetSt == WIFI_STATUS_BATTLEROOM)){
        wk->seq = WIFIP2PMATCH_MODE_CALL_YESNO;  //ok
    }
#endif
    else if((mySt == WIFI_STATUS_BUCKET_WAIT)&&(targetSt == WIFI_STATUS_BUCKET)){
        wk->seq = WIFIP2PMATCH_MODE_CALL_YESNO;  //ok
    }
    else if((mySt == WIFI_STATUS_BALANCEBALL_WAIT)&&(targetSt == WIFI_STATUS_BALANCEBALL)){
        wk->seq = WIFIP2PMATCH_MODE_CALL_YESNO;  //ok
    }
    else if((mySt == WIFI_STATUS_BALLOON_WAIT)&&(targetSt == WIFI_STATUS_BALLOON)){
        wk->seq = WIFIP2PMATCH_MODE_CALL_YESNO;  //ok
    }
    else if((mySt == WIFI_STATUS_LOGIN_WAIT)&&(targetSt == WIFI_STATUS_VCT)&&(myvchat > 0)){	// 080703 tomoya VCHATがONであることを条件に追加
        wk->seq = WIFIP2PMATCH_MODE_VCT_CONNECT_INIT; //o
        return seq;
    }
    else{
        // 適合しない=>切断処理へ
        _friendNameExpand(wk, mydwc_getFriendIndex());
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
        wk->seq = WIFIP2PMATCH_MODE_DISCONNECT;
        _myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);
        CommStateWifiMatchEnd();
        return seq;
    }
    _friendNameExpand(wk, mydwc_getFriendIndex());
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_008, FALSE);
    CommInfoInitialize(wk->pSaveData,NULL);   //Info初期化
    wk->seq = WIFIP2PMATCH_MODE_CALL_YESNO;
    wk->timer = 30;
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   コネクションはろうとしている期間中のエラー表示
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static BOOL _connectingErrFunc(WIFIP2PMATCH_WORK *wk)
{
    if(CommWifiIsMatched() >= 3){
        OS_TPrintf("_connectingErrFunc%d \n",CommWifiIsMatched());
        _friendNameExpand(wk, wk->preConnect);
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
        wk->seq = WIFIP2PMATCH_MODE_VCT_DISCONNECT;
    }
    else if(CommStateIsWifiError()){
        _errorDisp(wk);
    }
    else{

#if PL_T0857_080711_FIX
		// 友達のSTATUSとVCHATをチェック	違っていたら切断
		{
			int mySt;
			int targetSt, targetSt_org;
			_WIFI_MACH_STATUS* p_status;
			int myvchat, targetvchat;

			p_status = WifiFriendMatchStatusGet( wk, mydwc_getFriendIndex() );
			mySt		= _WifiMyStatusGet( wk,&wk->pMatch->myMatchStatus );
			targetSt_org= _WifiMyStatusGet( wk,p_status );
			targetSt	= _convertState(targetSt_org);
			myvchat		= wk->pMatch->myMatchStatus.vchat;
			targetvchat	= p_status->vchat;

			OS_TPrintf( "check mystart=%d tastatus=%d tastatus_org=%d myvchat=%d tavchat=%d \n", 
					mySt, targetSt, targetSt_org, myvchat, targetvchat );

			if( ((mySt != targetSt) && (mySt != targetSt_org)) ||
				(myvchat != targetSt) ){
				_friendNameExpand(wk, mydwc_getFriendIndex());
				WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
				wk->seq = WIFIP2PMATCH_MODE_VCT_DISCONNECT;

			}else{

				return FALSE;
			}
		}
#else
        return FALSE;
#endif
    }
    wk->bRetryBattle = FALSE;
    return TRUE;
}

//------------------------------------------------------------------
/**
 * $brief   つながるべきステート確認  WIFIP2PMATCH_MODE_CALL_YESNO
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _parentModeCallMenuYesNo( WIFIP2PMATCH_WORK *wk, int seq )
{
    if((CommWifiIsMatched() >= 4) || CommStateIsWifiDisconnect() || !CommIsConnect(COMM_PARENT_ID)){
        OS_TPrintf("%d %d %d\n",CommWifiIsMatched(),CommStateIsWifiDisconnect(),CommIsConnect(COMM_PARENT_ID));
        
        if(wk->bRetryBattle){
            WifiP2PMatchMessagePrint(wk, msg_wifilobby_065, FALSE);
        }
        else{
            _friendNameExpand(wk, wk->preConnect);
            WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);
        }
        wk->bRetryBattle = FALSE;
        wk->seq = WIFIP2PMATCH_MODE_VCT_DISCONNECT;
    }
    else if(_connectingErrFunc(wk)){
    }
    else{

		
        if(wk->timer==0){
            if(CommIsTimingSync(_TIMING_GAME_CHECK)){
                CommToolTempDataReset();
                CommTimingSyncStart(_TIMING_GAME_CHECK2);
                wk->bRetryBattle = FALSE;
                wk->seq = WIFIP2PMATCH_MODE_CALL_SEND;
            }
        }
        else{
            wk->timer--;
            if(wk->timer == 0){
                CommTimingSyncStart(_TIMING_GAME_CHECK);
            }
        }
    }
    return seq;
}

//
//------------------------------------------------------------------
/**
 * $brief   つながるべきステートを送信  WIFIP2PMATCH_MODE_CALL_SEND
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _parentModeCallMenuSend( WIFIP2PMATCH_WORK *wk, int seq )
{
    if(_connectingErrFunc(wk)){
    }
    else if(CommIsTimingSync(_TIMING_GAME_CHECK2)){
        u16 status = _WifiMyStatusGet( wk, &wk->pMatch->myMatchStatus );
		BOOL result;
		
        result = CommToolSetTempData(CommGetCurrentID() ,&status);
		if( result ){
	        wk->seq = WIFIP2PMATCH_MODE_CALL_CHECK;
		}
    }
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   つながるべきステート解析  WIFIP2PMATCH_MODE_CALL_CHECK
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _parentModeCallMenuCheck( WIFIP2PMATCH_WORK *wk, int seq )
{
    const u16* pData;
    int id=0;
    if(CommGetCurrentID() == COMM_PARENT_ID){
        id = 1;
    }
    else{
        id = COMM_PARENT_ID;
    }
    pData = CommToolGetTempData(id);
    if(_connectingErrFunc(wk)){
    }
    else if(pData!=NULL){
        u16 org_status = _WifiMyStatusGet( wk, &wk->pMatch->myMatchStatus );
		u16 status = _convertState(org_status);
        if((pData[0] == status) || (pData[0] == org_status)){
            CommTimingSyncStart(_TIMING_GAME_START);
            wk->seq = WIFIP2PMATCH_MODE_MYSTATUS_WAIT;
        }
        else{  // 異なるステートを選択した場合
            _friendNameExpand(wk, mydwc_getFriendIndex());
            WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
            wk->seq = WIFIP2PMATCH_MODE_VCT_DISCONNECT;
        }
    }
    return seq;
}



//------------------------------------------------------------------
/**
 * $brief   タイミングコマンド待ち WIFIP2PMATCH_MODE_MYSTATUS_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _parentModeCallMyStatusWait( WIFIP2PMATCH_WORK *wk, int seq )
{
    int status;

    if(_connectingErrFunc(wk)){
    }
    else if(CommIsTimingSync(_TIMING_GAME_START)){
        CommInfoSendPokeData();
        CommStateSetErrorCheck(TRUE,TRUE);
        CommTimingSyncStart(_TIMING_GAME_START2);
        wk->seq = WIFIP2PMATCH_MODE_CALL_WAIT;
    }
    return seq;
}


static BOOL _parent_MsgEndCheck( WIFIP2PMATCH_WORK *wk )
{
	if( GF_MSG_PrintEndCheck( wk->MsgIndex ) == 0 ){
		return TRUE;
	}

	if( GF_BGL_BmpWinAddCheck( &wk->MsgWin ) == FALSE ){
		return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------------
/**
 * $brief   タイミングコマンド待ち WIFIP2PMATCH_MODE_CALL_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static int _parentModeCallMenuWait( WIFIP2PMATCH_WORK *wk, int seq )
{
    int status;
    int checkFriend[COMM_MACHINE_MAX];
	_WIFI_MACH_STATUS* p_status;

    if(_connectingErrFunc(wk)){
    }
//    else if(CommIsTimingSync(_TIMING_GAME_START2)){
    else if(CommIsTimingSync(_TIMING_GAME_START2) && (_parent_MsgEndCheck( wk ) == TRUE) ){		// メッセージの終了も待つように変更 08.06.01	tomoya
        dwc_friendAutoInputCheck(wk->pSaveData, checkFriend, HEAPID_WIFIP2PMATCH);
        
        EndMessageWindowOff(wk);

		p_status = WifiFriendMatchStatusGet( wk, mydwc_getFriendIndex() );
        status = _WifiMyStatusGet( wk, p_status );
        if((status == WIFI_STATUS_TRADE_WAIT) || (status == WIFI_STATUS_TRADE)){
            status = WIFI_STATUS_TRADE;
            wk->endSeq = WIFI_P2PMATCH_TRADE;
        }
        else if((status == WIFI_STATUS_POFIN_WAIT) || (status == WIFI_STATUS_POFIN)){
			status = WIFI_STATUS_POFIN;
			wk->endSeq = WIFI_P2PMATCH_POFIN;
        }
        else if((status == WIFI_STATUS_FRONTIER_WAIT) || (status == WIFI_STATUS_FRONTIER)){
			status = WIFI_STATUS_FRONTIER;
			wk->endSeq = WIFI_P2PMATCH_FRONTIER;
        }
#ifdef WFP2P_DEBUG_EXON 
        else if((status == WIFI_STATUS_BATTLEROOM_WAIT) || (status == WIFI_STATUS_BATTLEROOM)){
            status = WIFI_STATUS_BATTLEROOM;
            wk->endSeq = WIFI_P2PMATCH_BATTLEROOM;
        }
#endif
        else if((status == WIFI_STATUS_BUCKET_WAIT) || (status == WIFI_STATUS_BUCKET)){
            status = WIFI_STATUS_BUCKET;
            wk->endSeq = WIFI_P2PMATCH_BUCKET;
        }
        else if((status == WIFI_STATUS_BALANCEBALL_WAIT) || (status == WIFI_STATUS_BALANCEBALL)){
            status = WIFI_STATUS_BALANCEBALL;
            wk->endSeq = WIFI_P2PMATCH_BALANCEBALL;
        }
        else if((status == WIFI_STATUS_BALLOON_WAIT) || (status == WIFI_STATUS_BALLOON)){
            status = WIFI_STATUS_BALLOON;
            wk->endSeq = WIFI_P2PMATCH_BALLOON;
        }
        else if((status == WIFI_STATUS_SBATTLE50_WAIT) || (status == WIFI_STATUS_SBATTLE50)){
            status = WIFI_STATUS_SBATTLE50;
            wk->endSeq = WIFI_P2PMATCH_SBATTLE50;
        }
        else if((status == WIFI_STATUS_SBATTLE100_WAIT) || (status == WIFI_STATUS_SBATTLE100)){
            status = WIFI_STATUS_SBATTLE100;
            wk->endSeq = WIFI_P2PMATCH_SBATTLE100;
        }
        else if((status == WIFI_STATUS_SBATTLE_FREE_WAIT) || (status == WIFI_STATUS_SBATTLE_FREE)){
            status = WIFI_STATUS_SBATTLE_FREE;
            wk->endSeq = WIFI_P2PMATCH_SBATTLE_FREE;
        }
        else if((status == WIFI_STATUS_DBATTLE50_WAIT) || (status == WIFI_STATUS_DBATTLE50)){
            status = WIFI_STATUS_DBATTLE50;
            wk->endSeq = WIFI_P2PMATCH_DBATTLE50;
        }
        else if((status == WIFI_STATUS_DBATTLE100_WAIT) || (status == WIFI_STATUS_DBATTLE100)){
            status = WIFI_STATUS_DBATTLE100;
            wk->endSeq = WIFI_P2PMATCH_DBATTLE100;
        }
        else if((status == WIFI_STATUS_DBATTLE_FREE_WAIT) || (status == WIFI_STATUS_DBATTLE_FREE)){
            status = WIFI_STATUS_DBATTLE_FREE;
            wk->endSeq = WIFI_P2PMATCH_DBATTLE_FREE;
        }
#ifdef WFP2P_DEBUG_EXON 
		else if((status == WIFI_STATUS_MBATTLE_FREE_WAIT) || (status == WIFI_STATUS_MBATTLE_FREE)){
            status = WIFI_STATUS_MBATTLE_FREE;
            wk->endSeq = WIFI_P2PMATCH_MBATTLE_FREE;
        }
#endif

// WIFI　対戦AUTOﾓｰﾄﾞデバック
#ifdef _WIFI_DEBUG_TUUSHIN
		WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_A_REQ = FALSE;
		WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_TOUCH_REQ = FALSE;
#endif	//_WIFI_DEBUG_TUUSHIN

		// ボウケンノート
		WifiP2P_Fnote_Set( wk, mydwc_getFriendIndex() );

        _myStatusChange(wk, status);  // 接続中になる
        WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK,
                        COMM_BRIGHTNESS_SYNC, 1, HEAPID_WIFIP2PMATCH);

		
        seq = SEQ_OUT;						//終了シーケンスへ
    }
    return seq;
}


//------------------------------------------------------------------
/**
 * $brief   メインメニューの戻る際の初期化
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------
static int WifiP2PMatch_MainReturn( WIFIP2PMATCH_WORK *wk, int seq )
{
    GF_BGL_ScrClear(wk->bgl, GF_BGL_FRAME3_M);
    wk->mainCur = 0;
    wk->seq = WifiP2PMatchFriendListStart();
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   個人データ表示初期化 WIFIP2PMATCH_MODE_PERSONAL_INIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _personalDataInit( WIFIP2PMATCH_WORK *wk, int seq )
{
    u16 mainCursor, friendNo;
    int num,length,x,width;

#if 0
    GF_BGL_ScrClear(wk->bgl, GF_BGL_FRAME3_M);
    ArcUtil_ScrnSet(   ARC_WIFIP2PMATCH_GRA, NARC_wifip2pmatch_conect_03_NSCR, wk->bgl,
                       GF_BGL_FRAME1_M, 0, 0, 0, HEAPID_WIFIP2PMATCH);

#if 0
    CLACT_SetDrawFlag( wk->MainActWork[_CLACT_DOWN_CUR], 0 );
    CLACT_SetDrawFlag( wk->MainActWork[_CLACT_LINE_CUR], 0 );
    CLACT_SetDrawFlag( wk->MainActWork[_CLACT_UP_CUR], 0 );
#endif
	
    if(GF_BGL_BmpWinAddCheck(&wk->MyWin)){
        BmpMenuWinClear(&wk->MyWin, WINDOW_TRANS_ON );
        GF_BGL_BmpWinDel(&wk->MyWin);
    }
    GF_BGL_BmpWinAdd(wk->bgl,&wk->MyWin,
                     GF_BGL_FRAME3_M, 3, 2, 0x1a, 14, FLD_SYSFONT_PAL, 1);
    width = ( 0x1a * 8 )-2;
    GF_BGL_BmpWinDataFill( &wk->MyWin, 0x8000 );

//    BmpListDirectPosGet(wk->lw,&mainCursor);
//    friendNo = wk->index2No[mainCursor] - 1;
	friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
	friendNo --;
    OHNO_PRINT("フレンド番号  %d\n", friendNo);
    _friendNameExpand(wk, friendNo);
    MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_033, wk->pExpStrBuf );
    WORDSET_ExpandStr( wk->WordSet, wk->TitleString, wk->pExpStrBuf );
    GF_STR_PrintColor( &wk->MyWin, FONT_TALK, wk->TitleString, 0, 0, MSG_NO_PUT, _COL_N_BLACK, NULL);
        GF_BGL_BmpWinOnVReq(&wk->MyWin);

    WifiP2PMatchMessagePrint(wk, msg_wifilobby_042, FALSE);


    // ----------------------------------------------------------------------------
    // localize_spec_mark(LANG_ALL) imatake 2007/01/29
    // グループ名を右寄せ
    MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_034, wk->TitleString );
    GF_STR_PrintColor( &wk->MyWin, FONT_SYSTEM, wk->TitleString, 0, 8*2, MSG_NO_PUT, _COL_N_BLACK, NULL);
    {
        MYSTATUS* pTarget = MyStatus_AllocWork(HEAPID_WIFIP2PMATCH);
        MyStatus_SetMyName(pTarget, WifiList_GetFriendGroupNamePtr(wk->pList,friendNo));
        WORDSET_RegisterPlayerName( wk->WordSet, 0, pTarget);
        sys_FreeMemoryEz(pTarget);
    }
    MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_034_tag, wk->pExpStrBuf );
    WORDSET_ExpandStr( wk->WordSet, wk->TitleString, wk->pExpStrBuf );
    length = FontProc_GetPrintStrWidth(FONT_SYSTEM, wk->TitleString, 0);
    x      = width - length;
    GF_STR_PrintColor( &wk->MyWin, FONT_SYSTEM, wk->TitleString, x, 8*2, MSG_NO_PUT, _COL_N_BLACK, NULL);
    // ----------------------------------------------------------------------------

    // 対戦成績
    MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_035, wk->TitleString );
    GF_STR_PrintColor( &wk->MyWin, FONT_SYSTEM, wk->TitleString, 0, 8*4, MSG_NO_PUT, _COL_N_BLACK, NULL);
    // かち
    num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_BATTLE_WIN);
    WORDSET_RegisterNumber(wk->WordSet, 0, num, 4, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
    MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_036, wk->pExpStrBuf );
    WORDSET_ExpandStr( wk->WordSet, wk->TitleString, wk->pExpStrBuf );
    GF_STR_PrintColor( &wk->MyWin, FONT_SYSTEM, wk->TitleString, 30, 8*6, MSG_NO_PUT, _COL_N_BLACK, NULL);
    // まけ
    num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_BATTLE_LOSE);
    WORDSET_RegisterNumber(wk->WordSet, 0, num, 4, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
    MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_037, wk->pExpStrBuf );
    WORDSET_ExpandStr( wk->WordSet, wk->TitleString, wk->pExpStrBuf );

    length = FontProc_GetPrintStrWidth( FONT_SYSTEM, wk->TitleString, 0 );
    x      = width - length;
    GF_STR_PrintColor( &wk->MyWin, FONT_SYSTEM, wk->TitleString, x, 8*6, MSG_NO_PUT, _COL_N_BLACK, NULL);
    // ポケモン交換
    MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_038, wk->TitleString );
    GF_STR_PrintColor( &wk->MyWin, FONT_SYSTEM, wk->TitleString, 0,  8*8, MSG_NO_PUT, _COL_N_BLACK, NULL);

    num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_TRADE_NUM);
    WORDSET_RegisterNumber(wk->WordSet, 0, num, 4, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
    // ----------------------------------------------------------------------------
    // localize_spec_mark(LANG_ALL) imatake 2007/01/25
    // 回数によって単位の単複を出し分け
    if (num == 1) {
        MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_039, wk->pExpStrBuf );
    } else {
        MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_039_plural, wk->pExpStrBuf );
    }
    // ----------------------------------------------------------------------------
    WORDSET_ExpandStr( wk->WordSet, wk->TitleString, wk->pExpStrBuf );

    length = FontProc_GetPrintStrWidth( FONT_SYSTEM, wk->TitleString, 0 );
    x      = width - length;
    GF_STR_PrintColor( &wk->MyWin, FONT_SYSTEM, wk->TitleString, x, 8*8, MSG_NO_PUT, _COL_N_BLACK, NULL);
    //最後の日付
    MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_040, wk->TitleString );
    GF_STR_PrintColor( &wk->MyWin, FONT_SYSTEM, wk->TitleString, 0, 8*10, MSG_NO_PUT, _COL_N_BLACK, NULL);

    num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_LASTBT_DAY);
    if(num!=0){
        WORDSET_RegisterNumber(wk->WordSet, 2, num, 2, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
        num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_LASTBT_YEAR);
        WORDSET_RegisterNumber(wk->WordSet, 0, num, 4, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
        num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_LASTBT_MONTH);
        // ----------------------------------------------------------------------------
        // localize_spec_mark(LANG_ALL) imatake 2007/01/29
        // 月の表示を単語表記に変更
        WORDSET_RegisterMonthName( wk->WordSet, 1, num );
        // ----------------------------------------------------------------------------
        MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_041, wk->pExpStrBuf );
        WORDSET_ExpandStr( wk->WordSet, wk->TitleString, wk->pExpStrBuf );
        length = FontProc_GetPrintStrWidth( FONT_SYSTEM, wk->TitleString, 0 );
        x      = width - length;
        GF_STR_PrintColor( &wk->MyWin, FONT_SYSTEM, wk->TitleString, x, 8*12, MSG_NO_PUT, _COL_N_BLACK, NULL);
    }
    GF_BGL_BmpWinOnVReq(&wk->MyWin);
#endif
	
    wk->seq = WIFIP2PMATCH_MODE_PERSONAL_WAIT;
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   個人データ表示 WIFIP2PMATCH_MODE_PERSONAL_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _personalDataWait( WIFIP2PMATCH_WORK *wk, int seq )
{
    if( GF_MSG_PrintEndCheck( wk->MsgIndex ) != 0 ){
        return seq;
    }
    if( 0 !=  _checkParentConnect(wk)){ // 接続してきた
        wk->seq = WIFIP2PMATCH_MODE_PERSONAL_END;
    }
    if(sys.trg & (PAD_BUTTON_CANCEL|PAD_BUTTON_DECIDE)){
        wk->seq = WIFIP2PMATCH_MODE_PERSONAL_END;
    }
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   個人データ表示おわり WIFIP2PMATCH_MODE_PERSONAL_END
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _personalDataEnd( WIFIP2PMATCH_WORK *wk, int seq )
{
    int i,x;
    int num = 0;

#if 0
    BmpMenuWinClear(&wk->MyWin, WINDOW_TRANS_ON );
    GF_BGL_BmpWinDel(&wk->MyWin);
    GF_BGL_ScrClear(wk->bgl, GF_BGL_FRAME1_M);
    GF_BGL_ScrClear(wk->bgl, GF_BGL_FRAME3_M);
#endif
    EndMessageWindowOff(wk);

#if 0
    GF_BGL_BmpWinDataFill( &wk->MyInfoWinBack, 0x8000 );
    MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_019, wk->TitleString );
    GF_STR_PrintColor( &wk->MyInfoWinBack, FONT_TALK, wk->TitleString, 0, 0, MSG_NO_PUT, _COL_N_WHITE, NULL);
        GF_BGL_BmpWinOnVReq(&wk->MyInfoWinBack);

    _readFriendMatchStatus(wk);
	 _userDataDisp(wk);
    _myStatusChange(wk, wk->pMatch->myMatchStatus.status);
#endif

    wk->seq = WIFIP2PMATCH_MODE_FRIENDLIST;
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   終了確認メッセージ  WIFIP2PMATCH_MODE_EXIT_YESNO
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _exitYesNo( WIFIP2PMATCH_WORK *wk, int seq )
{
    if( GF_MSG_PrintEndCheck( wk->MsgIndex ) == 0 ){
        // はいいいえウインドウを出す
        wk->pYesNoWork =
            BmpYesNoSelectInit( wk->bgl,
                                &_yesNoBmpDat,
                                MENU_WIN_CGX_NUM, MENU_WIN_PAL,
                                HEAPID_WIFIP2PMATCH );
        wk->seq = WIFIP2PMATCH_MODE_EXIT_WAIT;
    }
	
// WIFI　対戦AUTOﾓｰﾄﾞデバック
#ifdef _WIFI_DEBUG_TUUSHIN
	WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_A_REQ = TRUE;
	WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_B_REQ = FALSE;
#endif	//_WIFI_DEBUG_TUUSHIN
	
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   終了確認メッセージ  WIFIP2PMATCH_MODE_EXIT_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _exitWait( WIFIP2PMATCH_WORK *wk, int seq )
{
    int i;
    int ret = BmpYesNoSelectMain(wk->pYesNoWork, HEAPID_WIFIP2PMATCH);

    if( GF_MSG_PrintEndCheck( wk->MsgIndex ) != 0 ){
        return seq;
    }

	//  接続チェック
    if( 0 !=  _checkParentNewPlayer(wk)){ // 接続してきた
		if(ret == BMPMENU_NULL){
			BmpYesNoWinDel( wk->pYesNoWork, HEAPID_WIFIP2PMATCH );
		}
		ret = BMPMENU_CANCEL;	// CANCEL
	}
	
    if(ret == BMPMENU_NULL){  // まだ選択中
        return seq;
    }else if(ret == 0){ // はいを選択した場合
        WifiP2PMatchMessagePrint(wk, dwc_message_0011, TRUE);
        wk->seq = WIFIP2PMATCH_MODE_EXITING;
        wk->timer = 1;
    }
    else{  // いいえを選択した場合
        EndMessageWindowOff(wk);
        wk->seq = WIFIP2PMATCH_MODE_FRIENDLIST;

		// 主人公の動作を許可
		FriendRequestWaitOff( wk );
    }
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   終了確認メッセージ  WIFIP2PMATCH_MODE_EXIT_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _exitExiting( WIFIP2PMATCH_WORK *wk, int seq )
{
    if( GF_MSG_PrintEndCheck( wk->MsgIndex ) != 0 ){
        return seq;
    }
    if(wk->timer == 1){
        wk->timer = 0;
        CommStateWifiLogout();  // 終了
    }
    if(!CommStateIsInitialize()){
        OHNO_PRINT("切断した時にフレンドコードを詰める\n");
        WifiList_FormUpData(wk->pList);  // 切断した時にフレンドコードを詰める
        //SaveData_SaveParts(wk->pSaveData, SVBLK_ID_NORMAL);  //セーブ中
        WifiP2PMatchMessagePrint(wk, dwc_message_0012, TRUE);
        wk->seq = WIFIP2PMATCH_MODE_EXIT_END;
        wk->timer = 30;
    }
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   終了完了  WIFIP2PMATCH_MODE_EXIT_END
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------


static int _exitEnd( WIFIP2PMATCH_WORK *wk, int seq )
{
    if( GF_MSG_PrintEndCheck( wk->MsgIndex ) != 0 ){
        return seq;
    }
    wk->timer--;
    if(wk->timer==0){
        wk->endSeq = WIFI_P2PMATCH_END;
        wk->seq = WIFIP2PMATCH_MODE_END_WAIT;
        EndMessageWindowOff(wk);
    }

// WIFI　対戦AUTOﾓｰﾄﾞデバック
#ifdef _WIFI_DEBUG_TUUSHIN
	WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_A_REQ = FALSE;
	WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_B_REQ = FALSE;
	WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_TOUCH_REQ = FALSE;
#endif	//_WIFI_DEBUG_TUUSHIN
    return seq;
}


//------------------------------------------------------------------
/**
 * $brief   もう一度対戦するか聞く WIFIP2PMATCH_NEXTBATTLE_YESNO
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _nextBattleYesNo( WIFIP2PMATCH_WORK *wk, int seq )
{
//    CommStateSetErrorCheck(FALSE,FALSE);
    if(CommIsTimingSync(_TIMING_BATTLE_END)==FALSE){
        return seq;
    }
	// 通信同期中に電源を切られたら、ずっと同期待ちしてしまうので、通信同期後にオートエラーチェックを
	// はずす
    CommStateSetErrorCheck(FALSE,TRUE);	
    if( GF_MSG_PrintEndCheck( wk->MsgIndex ) == 0 ){
        // はいいいえウインドウを出す
        wk->pYesNoWork =
            BmpYesNoSelectInit( wk->bgl,
                                &_yesNoBmpDat,
                                MENU_WIN_CGX_NUM, MENU_WIN_PAL,
                                HEAPID_WIFIP2PMATCH );
        wk->seq = WIFIP2PMATCH_NEXTBATTLE_WAIT;
    }
	
// WIFI　対戦AUTOﾓｰﾄﾞデバック
#ifdef _WIFI_DEBUG_TUUSHIN
	WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_A_REQ = FALSE;
	WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_TOUCH_REQ = FALSE;
	WIFI_DEBUG_BATTLE_Work.DEBUG_WIFI_B_REQ = TRUE;
#endif	//_WIFI_DEBUG_TUUSHIN
	
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   もう一度対戦するか聞く WIFIP2PMATCH_NEXTBATTLE_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _nextBattleWait( WIFIP2PMATCH_WORK *wk, int seq )
{
    int i;
    int ret;

    if( GF_MSG_PrintEndCheck( wk->MsgIndex ) != 0 ){
        return seq;
    }

    if(CommStateIsWifiLoginState() || CommStateIsWifiDisconnect() || (CommWifiIsMatched() >= 3)){  // 切断した扱いとな
        BmpYesNoWinDel(wk->pYesNoWork, HEAPID_WIFIP2PMATCH);
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_065, FALSE);
        wk->seq = WIFIP2PMATCH_MODE_BATTLE_DISCONNECT;
    }
    else if(CommStateIsWifiError()){
        BmpYesNoWinDel(wk->pYesNoWork, HEAPID_WIFIP2PMATCH);
        _errorDisp(wk);
    }
    else{
        ret = BmpYesNoSelectMain(wk->pYesNoWork, HEAPID_WIFIP2PMATCH);
        if(ret == BMPMENU_NULL){  // まだ選択中
            return seq;
        }else if(ret == 0){ // はいを選択した場合
            //EndMessageWindowOff(wk);
            WifiP2PMatchMessagePrint(wk, msg_wifilobby_073, FALSE);
			GF_ASSERT( wk->timeWaitWork == NULL );
            wk->timeWaitWork = TimeWaitIconAdd( &wk->MsgWin, COMM_TALK_WIN_CGX_NUM );
            wk->bRetryBattle = TRUE;
            wk->seq = WIFIP2PMATCH_MODE_CALL_YESNO;
            wk->timer = 30;
        }
        else{  // いいえを選択した場合
            EndMessageWindowOff(wk);
            CommInfoFinalize();
            CommStateWifiBattleMatchEnd();
            wk->timer = _RECONECTING_WAIT_TIME;
            wk->seq = WIFIP2PMATCH_RECONECTING_WAIT;
        }
        wk->pMatch = CommStateGetMatchWork();
        wk->pMatch->myMatchStatus.vchat = wk->pMatch->myMatchStatus.vchat_org;
    }
    return seq;
}


//------------------------------------------------------------------
/**
 * $brief   VCAHTを変更してもいいかどうか聞く WIFIP2PMATCH_MODE_VCHAT_NEGO
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _vchatNegoCheck( WIFIP2PMATCH_WORK *wk, int seq )
{
	// 相手の状態がかわったりしたら終わる
	{
		int status,friendNo,vchat;
		MCR_MOVEOBJ* p_npc;
		_WIFI_MACH_STATUS* p_status;

		// 話しかけている友達ナンバー取得
		friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
		p_npc = MCRSYS_GetMoveObjWork( wk, friendNo );

		// 相手がいなくなったり
		// 相手のステータスが違うものに変わったら、
		// 表示を消して元に戻る
		if( p_npc == NULL ){
			_friendNameExpand(wk, friendNo - 1);
			WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
			wk->seq = WIFIP2PMATCH_MODE_DISCONNECT;
			return seq;
		}else{
			p_status = WifiFriendMatchStatusGet( wk, friendNo - 1 );
			status = _WifiMyStatusGet( wk, p_status );
			vchat = p_status->vchat;

			// 状態がかわったり
			if((wk->keepStatus != status) || (wk->keepVChat != vchat)){
				_friendNameExpand(wk, friendNo - 1);
				WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
				wk->seq = WIFIP2PMATCH_MODE_DISCONNECT;
				return seq;
			}
		}
	}	
	
    if( GF_MSG_PrintEndCheck( wk->MsgIndex ) == 0 ){
        // はいいいえウインドウを出す
        wk->pYesNoWork =
            BmpYesNoSelectInit( wk->bgl,
                                &_yesNoBmpDat,
                                MENU_WIN_CGX_NUM, MENU_WIN_PAL,
                                HEAPID_WIFIP2PMATCH );
        wk->seq = WIFIP2PMATCH_MODE_VCHAT_NEGO_WAIT;
    }
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   VCAHTを変更してもいいかどうか聞く
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _vchatNegoWait( WIFIP2PMATCH_WORK *wk, int seq )
{
    int i;
    int ret,status;

	// 相手の状態がかわったりしたら終わる
	{
		int friendNo,vchat;
		MCR_MOVEOBJ* p_npc;
		_WIFI_MACH_STATUS* p_status;

		// 話しかけている友達ナンバー取得
		friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
		p_npc = MCRSYS_GetMoveObjWork( wk, friendNo );

		// 相手がいなくなったり
		// 相手のステータスが違うものに変わったら、
		// 表示を消して元に戻る
		if( p_npc == NULL ){
			BmpYesNoWinDel(wk->pYesNoWork, HEAPID_WIFIP2PMATCH);
			_friendNameExpand(wk, friendNo - 1);
			WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
			wk->seq = WIFIP2PMATCH_MODE_DISCONNECT;
			return seq;
		}else{
			p_status = WifiFriendMatchStatusGet( wk, friendNo - 1 );
			status = _WifiMyStatusGet( wk, p_status );
			vchat = p_status->vchat;

			// 状態がかわったり
			if((wk->keepStatus != status) || (wk->keepVChat != vchat)){
				BmpYesNoWinDel(wk->pYesNoWork, HEAPID_WIFIP2PMATCH);
				_friendNameExpand(wk, friendNo - 1);
				WifiP2PMatchMessagePrint(wk, msg_wifilobby_013, FALSE);
				wk->seq = WIFIP2PMATCH_MODE_DISCONNECT;
				return seq;
			}
		}
	}	


    if(CommWifiIsMatched() >= 3){
        BmpYesNoWinDel(wk->pYesNoWork, HEAPID_WIFIP2PMATCH);
		WifiP2PMatchMessagePrint(wk, msg_wifilobby_015, FALSE);	// 反応がない・・・
        CommStateWifiMatchEnd();
        wk->seq = WIFIP2PMATCH_MODE_DISCONNECT;
		return seq;
    }
    else if(CommStateIsWifiError()){
        BmpYesNoWinDel(wk->pYesNoWork, HEAPID_WIFIP2PMATCH);
        _errorDisp(wk);
    }
    else{
        ret = BmpYesNoSelectMain(wk->pYesNoWork, HEAPID_WIFIP2PMATCH);
        if(ret == BMPMENU_NULL){  // まだ選択中
            return seq;
        }else if(ret == 0){ // はいを選択した場合
            // 接続開始
            status = _convertState(wk->keepStatus);
            if(WIFI_STATUS_UNKNOWN == status){   // 未知のステートの場合何も無いように戻る
            }
            else if( WifiDwc_getFriendStatus(wk->friendNo - 1) == DWC_STATUS_MATCH_SC_SV ){
				_myVChatStatusToggle(wk); // 自分のVCHATを反転
                if( WifiP2PMatch_CommWifiBattleStart( wk, wk->friendNo - 1, status ) ){
                    wk->cancelEnableTimer = _CANCELENABLE_TIMER;
					_commStateChange(status);
                    CommSetWifiBothNet(FALSE);  // VCT中は同期送信の必要ない
                    _myStatusChange(wk, status);  // 接続中になる
                    _friendNameExpand(wk, wk->friendNo - 1);
                    WifiP2PMatchMessagePrint(wk,msg_wifilobby_014, FALSE);
					GF_ASSERT( wk->timeWaitWork == NULL );
                    wk->timeWaitWork = TimeWaitIconAdd( &wk->MsgWin, COMM_TALK_WIN_CGX_NUM );
                    if(status != WIFI_STATUS_VCT){
                        wk->seq = WIFIP2PMATCH_MODE_MATCH_LOOP;  // マッチングへ
                        return seq;
                    }
                    else{
                        wk->cancelEnableTimer = _CANCELENABLE_TIMER;
                        wk->seq = WIFIP2PMATCH_MODE_VCT_CONNECT_INIT2;  //VCT専用へ
                        return seq;
                    }
                }
            }
        }
        // いいえを選択した場合  もしくは適合しない場合
        EndMessageWindowOff(wk);


		// その人の情報を消す
//		WifiP2PMatch_UserDispOff( wk, HEAPID_WIFIP2PMATCH );

		// ステータスをもどす
		// 080703	tomoya takahashi
        _myStatusChange(wk, WIFI_STATUS_LOGIN_WAIT);

		// VCHAT元に戻す
		_myVChatStatusOrgSet( wk );
		_userDataInfoDisp(wk);
        wk->seq = WIFIP2PMATCH_MODE_FRIENDLIST;
        return seq;
        
    }
    return seq;
}







// WIFIP2PMATCH_MODE_END_WAIT
//------------------------------------------------------------------
/**
 * $brief   終了
 *
 * @param   wk
 * @param   seq
 *
 * @retval  int
 */
//------------------------------------------------------------------
static int 	WifiP2PMatch_EndWait( WIFIP2PMATCH_WORK *wk, int seq )
{
    if(!CommStateIsInitialize()){
        WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK,
                        COMM_BRIGHTNESS_SYNC, 1, HEAPID_WIFIP2PMATCH);
        seq = SEQ_OUT;						//終了シーケンスへ
    }
    return seq;
}

//------------------------------------------------------------------
/**
 * $brief   会話ウインドウ表示
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------


static void WifiP2PMatchMessagePrint( WIFIP2PMATCH_WORK *wk, int msgno, BOOL bSystem )
{
    // 文字列取得
    u8 speed = CONFIG_GetMsgPrintSpeed(SaveData_GetConfig(wk->pSaveData));

	// TimeWaitIcon破棄
	_timeWaitIconDel( wk );

    if(GF_BGL_BmpWinAddCheck(&wk->SysMsgWin)){
        BmpTalkWinClear(&wk->SysMsgWin, WINDOW_TRANS_ON );
        GF_BGL_BmpWinDel( &wk->SysMsgWin);
    }
    if(GF_BGL_BmpWinAddCheck(&wk->MsgWin)){
        BmpTalkWinClear(&wk->MsgWin, WINDOW_TRANS_ON );
        GF_BGL_BmpWinDel(&wk->MsgWin);
    }
    if(wk->MsgIndex != _PRINTTASK_MAX){
        if(GF_MSG_PrintEndCheck(wk->MsgIndex)!=0){
            GF_STR_PrintForceStop(wk->MsgIndex);
            wk->MsgIndex = _PRINTTASK_MAX;
        }
    }
    
    GF_BGL_BmpWinAdd(wk->bgl, &wk->MsgWin,
                     GF_BGL_FRAME2_M,
                     COMM_MSG_WIN_PX, COMM_MSG_WIN_PY,
                     COMM_MSG_WIN_SX, COMM_MSG_WIN_SY,
                     COMM_MESFONT_PAL, COMM_MSG_WIN_CGX);
    if(bSystem){
        MSGMAN_GetString(  wk->SysMsgManager, msgno, wk->pExpStrBuf );
    }
    else{
        MSGMAN_GetString(  wk->MsgManager, msgno, wk->pExpStrBuf );
    }
    WORDSET_ExpandStr( wk->WordSet, wk->TalkString, wk->pExpStrBuf );
    // 会話ウインドウ枠描画
    GF_BGL_BmpWinDataFill(&wk->MsgWin, 15 );
    BmpTalkWinWrite(&wk->MsgWin, WINDOW_TRANS_OFF,COMM_TALK_WIN_CGX_NUM, COMM_MESFRAME_PAL );
    // 文字列描画開始
	MsgPrintSkipFlagSet( MSG_SKIP_ON );		// メッセージスキップON
	MsgPrintAutoFlagSet( MSG_AUTO_OFF );	// メッセージ自動送りOFF
    wk->MsgIndex = GF_STR_PrintSimple( &wk->MsgWin, FONT_TALK, wk->TalkString,
                                       0, 0, speed, NULL);
    GF_BGL_BmpWinOnVReq(&wk->MsgWin);
}

//------------------------------------------------------------------
/**
 * $brief   システムメッセージウインドウ表示
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------

static void _systemMessagePrint( WIFIP2PMATCH_WORK *wk, int msgno )
{
    _timeWaitIconDel(wk);
    if(GF_BGL_BmpWinAddCheck(&wk->SysMsgWin)){
        BmpTalkWinClear(&wk->SysMsgWin, WINDOW_TRANS_ON );
        GF_BGL_BmpWinDel( &wk->SysMsgWin);
    }
    if(GF_BGL_BmpWinAddCheck(&wk->MsgWin)){
        BmpTalkWinClear(&wk->MsgWin, WINDOW_TRANS_ON );
        GF_BGL_BmpWinDel(&wk->MsgWin);
    }
/*	
// 　プラチナでは、自分のステータスが常に上に表示されているように仕様変更されたため、
// 　消してしまうと困るので、無効
#if AFTER_MASTER_070410_WIFIAPP_N20_EUR_FIX
    if(GF_BGL_BmpWinAddCheck(&wk->MyInfoWin)){
        BmpTalkWinClear( &wk->MyInfoWin, WINDOW_TRANS_ON );
        GF_BGL_BmpWinDel(&wk->MyInfoWin);
    }
#endif //AFTER_MASTER_070410_WIFIAPP_N20_EUR_FIX
//*/
    if(wk->MsgIndex != _PRINTTASK_MAX){
        if(GF_MSG_PrintEndCheck(wk->MsgIndex)!=0){
            GF_STR_PrintForceStop(wk->MsgIndex);
            wk->MsgIndex = _PRINTTASK_MAX;
        }
    }

    GF_BGL_BmpWinAdd(wk->bgl, &wk->SysMsgWin,
                     GF_BGL_FRAME2_M,
                     COMM_SYS_WIN_PX, COMM_SYS_WIN_PY,
                     COMM_SYS_WIN_SX, COMM_SYS_WIN_SY,
                     COMM_MESFONT_PAL, COMM_SYS_WIN_CGX);
    MSGMAN_GetString(  wk->SysMsgManager, msgno, wk->pExpStrBuf );
    WORDSET_ExpandStr( wk->WordSet, wk->TalkString, wk->pExpStrBuf );
    // 会話ウインドウ枠描画
    GF_BGL_BmpWinDataFill(&wk->SysMsgWin, 15 );
    BmpMenuWinWrite(&wk->SysMsgWin, WINDOW_TRANS_OFF, MENU_WIN_CGX_NUM, MENU_WIN_PAL );
    // 文字列描画開始
    wk->MsgIndex = GF_STR_PrintSimple( &wk->SysMsgWin, FONT_TALK,
                                       wk->TalkString, 0, 0, MSG_NO_PUT, NULL);
    GF_BGL_BmpWinOnVReq(&wk->SysMsgWin);
}

//------------------------------------------------------------------
/**
 * $brief   友人番号の名前をexpandする
 * @param   msg_index
 * @retval  int		    int friend = mydwc_getFriendIndex();

 */
//------------------------------------------------------------------
static void _friendNameExpand( WIFIP2PMATCH_WORK *wk, int friendNo)
{
    if(friendNo != -1){
        MYSTATUS* pTarget = MyStatus_AllocWork(HEAPID_WIFIP2PMATCH);
        MyStatus_SetMyName(pTarget, WifiList_GetFriendNamePtr(wk->pList,friendNo));
        WORDSET_RegisterPlayerName( wk->WordSet, 0, pTarget);
        sys_FreeMemoryEz(pTarget);
    }
}


//------------------------------------------------------------------
/**
 * $brief
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static void EndMessageWindowOff( WIFIP2PMATCH_WORK *wk )
{
    _timeWaitIconDel(wk);
    if(wk->MsgIndex != _PRINTTASK_MAX){
        if(GF_MSG_PrintEndCheck(wk->MsgIndex)!=0){
            GF_STR_PrintForceStop(wk->MsgIndex);
            wk->MsgIndex = _PRINTTASK_MAX;
        }
    }
    if(GF_BGL_BmpWinAddCheck(&wk->MsgWin)){
        BmpTalkWinClear(&wk->MsgWin, WINDOW_TRANS_ON );
        GF_BGL_BmpWinDel( &wk->MsgWin );
    }
}

//------------------------------------------------------------------
/**
 * $brief
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static void _myStatusChange(WIFIP2PMATCH_WORK *wk, int status)
{
    if(wk->pMatch==NULL){  //0707
        return;
    }
	_myStatusChange_not_send( wk, status );
    mydwc_setMyInfo( &(wk->pMatch->myMatchStatus), sizeof(_WIFI_MACH_STATUS) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	自分のステータス設定		ただ送信はしない
 *
 *	@param	wk
 *	@param	status
 */
//-----------------------------------------------------------------------------
static void _myStatusChange_not_send(WIFIP2PMATCH_WORK *wk, int status)
{
	int org_status;

    if(wk->pMatch==NULL){  //0707
        return;
    }
    
	org_status = _WifiMyStatusGet( wk, &wk->pMatch->myMatchStatus );
	
    if(org_status != status){

        wk->pMatch->myMatchStatus.status = status;
        if(_modeBattle(status) || (status == WIFI_STATUS_TRADE)|| (status == WIFI_STATUS_FRONTIER)||
#ifdef WFP2P_DEBUG_EXON 
				 (status == WIFI_STATUS_BATTLEROOM) ||
#endif
				 (status == WIFI_STATUS_BUCKET) || 
				 (status == WIFI_STATUS_BALANCEBALL) || 
				 (status == WIFI_STATUS_BALLOON) || 
				 (status == WIFI_STATUS_POFIN)){
            //     Snd_BgmFadeOut( 24, BGM_FADE_VCHAT_TIME);
		} 
        else if(status == WIFI_STATUS_VCT){
            Snd_BgmFadeOut( 0, BGM_FADE_VCHAT_TIME); // VCT状態へ
			
        }
        else if(status == WIFI_STATUS_LOGIN_WAIT){    // 待機中　ログイン直後はこれ

			// ボイスチャットなしにする
			mydwc_setVChat(FALSE);		// ボイスチャットとBGM音量の関係を整理 tomoya takahashi

			if( WifiP2P_CheckLobbyBgm() == TRUE ){
				// WiFi広場に来たときはどんなボリュームでも通常ボリュームにする
		        Snd_BgmFadeIn( BGM_WIFILOBBY_VOL, BGM_FADE_VCHAT_TIME, BGM_FADEIN_START_VOL_NOW);
			}
#if 0
			//ボイスチャットが終了しているので、設定音量を初期値に戻す！
			//本当は設定してある初期音量に戻さないといけないが。。とりあえずこれで対処！
			Snd_PlayerSetInitialVolumeBySeqNo( _BGM_MAIN, BGM_WIFILOBBY_VOL );
			Snd_PlayerMoveVolume( SND_HANDLE_BGM, BGM_WIFILOBBY_START_VOL, 0 );
            Snd_BgmFadeIn( BGM_VOL_MAX, BGM_FADE_VCHAT_TIME, BGM_FADEIN_START_VOL_NOW);
            OHNO_PRINT("Snd_BgmFadeIn\n");
#endif
        }
    }
    _userDataInfoDisp(wk);
}

//------------------------------------------------------------------
/**
 * $brief   VCHATフラグの切り替え
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static BOOL _myVChatStatusToggle(WIFIP2PMATCH_WORK *wk)
{
    wk->pMatch->myMatchStatus.vchat = 1 - wk->pMatch->myMatchStatus.vchat;
//    mydwc_setVChat(wk->pMatch->myMatchStatus.vchat);	// ボイスチャットとBGM音量の関係を整理 tomoya takahashi
    mydwc_setMyInfo( &(wk->pMatch->myMatchStatus), sizeof(_WIFI_MACH_STATUS) );
    return wk->pMatch->myMatchStatus.vchat;
}

//------------------------------------------------------------------
/**
 * $brief   VCHATフラグの切り替え
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static BOOL _myVChatStatusToggleOrg(WIFIP2PMATCH_WORK *wk)
{
    wk->pMatch->myMatchStatus.vchat_org = 1 - wk->pMatch->myMatchStatus.vchat_org;
    wk->pMatch->myMatchStatus.vchat = wk->pMatch->myMatchStatus.vchat_org;
//    mydwc_setVChat(wk->pMatch->myMatchStatus.vchat);	// ボイスチャットとBGM音量の関係を整理 tomoya takahashi
    mydwc_setMyInfo( &(wk->pMatch->myMatchStatus), sizeof(_WIFI_MACH_STATUS) );
    return wk->pMatch->myMatchStatus.vchat_org;
}

//------------------------------------------------------------------
/**
 * $brief   VCHATフラグをオリジナルにもどす
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static BOOL _myVChatStatusOrgSet(WIFIP2PMATCH_WORK *wk)
{
	OS_TPrintf( "change org %d\n", wk->pMatch->myMatchStatus.vchat_org );
    wk->pMatch->myMatchStatus.vchat = wk->pMatch->myMatchStatus.vchat_org;
//    mydwc_setVChat(wk->pMatch->myMatchStatus.vchat);// ボイスチャットとBGM音量の関係を整理 tomoya takahashi
    mydwc_setMyInfo( &(wk->pMatch->myMatchStatus), sizeof(_WIFI_MACH_STATUS) );
    return wk->pMatch->myMatchStatus.vchat_org;
}


//------------------------------------------------------------------
/**
 * $brief
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static void _timeWaitIconDel(WIFIP2PMATCH_WORK *wk)
{
    if(wk->timeWaitWork){
        TimeWaitIconTaskDel(wk->timeWaitWork);  // タイマー止め
        //TimeWaitIconDel(wk->timeWaitWork);
        //        GF_BGL_ScrClear(wk->bgl, GF_BGL_FRAME2_M);
        wk->timeWaitWork = NULL;
        // ----------------------------------------------------------------------------
        // localize_spec_mark(LANG_ALL) imatake 2007/02/13
        // ウィンドウを消去する際、そのウィンドウに書き込むタスクも一緒に消去
        // MatchComment: this localization change from DP US didn't make it into platinum
        //if(wk->MsgIndex != _PRINTTASK_MAX){
        //    if(GF_MSG_PrintEndCheck(wk->MsgIndex)!=0){
        //        GF_STR_PrintForceStop(wk->MsgIndex);
        //        wk->MsgIndex = _PRINTTASK_MAX;
        //    }
        //}
        // ----------------------------------------------------------------------------
        if(GF_BGL_BmpWinAddCheck(&wk->MsgWin)){
            BmpTalkWinClear( &wk->MsgWin, WINDOW_TRANS_ON );
            GF_BGL_BmpWinDel( &wk->MsgWin );
        }
        //      }
        //		GF_BGL_LoadScreenReq( wk->bgl, GF_BGL_FRAME2_M );
    }
}

//----------------------------------------------------------------------------
/**
 *	@brief	ユーザーデータ強制表示処理
 *
 *	@param	wk			システムワーク
 *	@param	friendNo	友達番号
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatch_UserDispOn( WIFIP2PMATCH_WORK *wk, u32 friendNo, u32 heapID )
{
	MCR_MOVEOBJ* p_obj;
	
	MCVSys_UserDispOn( wk, friendNo, heapID );
	p_obj = MCRSYS_GetMoveObjWork( wk, friendNo );
	if( p_obj ){
		WIFI_MCR_CursorOn( &wk->matchroom, p_obj );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ユーザーデータ強制表示処理終了
 *
 *	@param	wk			システムワーク
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatch_UserDispOff( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
	MCVSys_UserDispOff( wk );
	WIFI_MCR_CursorOff( &wk->matchroom );

	// 再描画
	MCVSys_ReWrite( wk, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	表示中の人なら強制的に表示終了
 *
 *	@param	wk				システムワーク
 *	@param	target_friend	ターゲットの人物のインデックス
 *	@param	heapID			ヒープID
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatch_UserDispOff_Target( WIFIP2PMATCH_WORK *wk, u32 target_friend, u32 heapID )
{
	if( MCVSys_UserDispGetFriend( wk ) == target_friend ){
		WifiP2PMatch_UserDispOff( wk, heapID );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	自分でアクセスしたときのユーザーデータ強制表示処理
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatch_UserDispOn_MyAcces( WIFIP2PMATCH_WORK *wk, u32 friendNo, u32 heapID )
{
	MCVSys_UserDispOn( wk, friendNo, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	自分でアクセスしたときのユーザーデータ強制表示処理終了
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatch_UserDispOff_MyAcces( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
	MCVSys_UserDispOff( wk );

	// 再描画
	MCVSys_ReWrite( wk, HEAPID_WIFIP2PMATCH );
}


//----------------------------------------------------------------------------
/**
 *	@brief	動作しているかチェック
 *
 *	@param	wk	システムワーク
 *
 *	@retval	TRUE	動作中
 *	@retval	FALSE	非動作中
 */
//-----------------------------------------------------------------------------
static BOOL MCVSys_MoveCheck( const WIFIP2PMATCH_WORK *wk )
{
	if( wk->view.p_bttnman == NULL ){
		return FALSE;
	}
	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	友達データビューアー初期化
 *
 *	@param	wk			システムワーク
 *	@param	p_handle	ハンドル
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void MCVSys_Init( WIFIP2PMATCH_WORK *wk, ARCHANDLE* p_handle, u32 heapID )
{
	static const TP_HIT_TBL bttndata[ WCR_MAPDATA_1BLOCKOBJNUM ] = {
		{	0,		47,		0,		119 },
		{	48,		95,		0,		119 },
		{	96,		143,	0,		119 },
		{	144,	191,	0,		119 },

		{	0,		47,		128,	255 },
		{	48,		95,		128,	255 },
		{	96,		143,	128,	255 },
		{	144,	191,	128,	255 },
	};

	memset( &wk->view, 0, sizeof(WIFIP2PMATCH_VIEW) );

	// 表示設定
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );	
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );	
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );	
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );
	
	// ボタンデータ初期化
	wk->view.p_bttnman = BMN_Create( bttndata, WCR_MAPDATA_1BLOCKOBJNUM, MCVSys_BttnCallBack, wk, heapID );

	// ワードセット初期化
	wk->view.p_wordset = WORDSET_Create( heapID );

	// グラフィックデータ読込み
	MCVSys_GraphicSet( wk, p_handle, heapID );

	// とりあえず更新
	wk->view.bttn_allchg = TRUE;
	MCVSys_BackDraw( wk );
	MCVSys_BttnDraw( wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	友達データビューアー破棄
 *
 *	@param	wk			システムワーク
 */
//-----------------------------------------------------------------------------
static void MCVSys_Exit( WIFIP2PMATCH_WORK *wk )
{
	if( MCVSys_MoveCheck( wk ) == FALSE ){
		return;  
	}
	
	// グラフィックはき
	MCVSys_GraphicDel( wk );

	// ボタンマネージャ破棄
	BMN_Delete( wk->view.p_bttnman );	
	wk->view.p_bttnman = NULL;

	// ワードセット破棄
	WORDSET_Delete( wk->view.p_wordset );
	wk->view.p_wordset = NULL;

	// BG２、３は非表示にしておく
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );	
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );
}

//----------------------------------------------------------------------------
/**
 *	@brief	友達データビューアーアップデート
 *
 *	@param	wk			システムワーク
 *
 *	@retval	押されている友達番号	+ 1
 *	@retval	0	押されていない
 */
//-----------------------------------------------------------------------------
static u32 MCVSys_Updata( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
	u32 map_param, map_param2;
	u32 oambttn_ret;
	BOOL userd_end;

	// 現在フレーム設定
	map_param = WIFI_MCR_GetPlayerOnMapParam( &wk->matchroom );
	
	// オブジェクトの位置だと下を見ないとちゃんとしたフレームの位置を取得できない
	map_param2 = WIFI_MCR_GetPlayerOnUnderMapParam( &wk->matchroom );
	if( (map_param2 >= MCR_MAPPM_MAP00) &&
		(map_param2 <= MCR_MAPPM_MAP03) ){
		map_param = map_param2;
	}
	
	if( (map_param >= MCR_MAPPM_MAP00) &&
		(map_param <= MCR_MAPPM_MAP03) ){
		if( wk->view.frame_no != (map_param - MCR_MAPPM_MAP00) ){
			wk->view.frame_no = (map_param - MCR_MAPPM_MAP00);

		
			Snd_SePlay( _SE_TBLCHANGE );

			// 背景カラー変更	
			MCVSys_BackDraw( wk );

			// ボタンも書き換える
			wk->view.button_on = TRUE;
			wk->view.bttn_allchg = TRUE;	// ボタン強制変更 
		}
	}


	// ユーザーデータ表示処理
	if( wk->view.user_disp == MCV_USERDISP_INIT ){
		wk->view.user_disp = MCV_USERDISP_ON;
		wk->view.user_dispno = WF_USERDISPTYPE_NRML;
		MCVSys_UserDispDraw( wk, heapID );
		MCVSys_OamBttnOn( wk );	// OAMボタン表示
	}


	if( wk->view.user_disp == MCV_USERDISP_OFF ){

		// ボタンメイン
		BMN_Main( wk->view.p_bttnman );

		// ボタンアニメ処理
		MCVSys_BttnAnmMan( wk );

		// 表示メイン
		if( wk->view.button_on == TRUE ){
			MCVSys_BttnDraw( wk );
			wk->view.button_on = FALSE;
		}
	}

	// 歩いたり、タッチパネルに触れていたらユーザー表示をOFFする
	if( (wk->view.user_disp == MCV_USERDISP_ON) || 
		(wk->view.user_disp == MCV_USERDISP_ONEX) ){

		// した画面OAMボタンメイン
		oambttn_ret = MCVSys_OamBttnMain( wk );

		//  USERD終了チェック
		userd_end = MCVSys_UserDispEndCheck( wk, oambttn_ret );
		
		// なにかキーを押すか、「もどる」を押したら終了する
		if( userd_end == TRUE ){
			wk->view.bttn_chg_friendNo = wk->view.touch_friendNo;	// この友達のボタンを更新してもらう
			wk->view.touch_friendNo = 0;
			wk->view.touch_frame = 0;
			wk->view.button_on = TRUE;
			wk->view.bttn_allchg = TRUE;
			wk->view.user_disp = MCV_USERDISP_OFF;
			MCVSys_OamBttnOff( wk );	// ボタンOFF
			Snd_SePlay( _SE_DESIDE );	// キャンセル音
		}else{
			// 左右のボタンが押されていたらページ切り替え１
			if( oambttn_ret != MCV_USERD_BTTN_RET_NONE ){
				Snd_SePlay( _SE_DESIDE );						// ページ変更音
				MCVSys_UserDispPageChange( wk, oambttn_ret );	// 変更処理
				MCVSys_UserDispDraw( wk, heapID );
			}
		}
	}

	return wk->view.touch_friendNo;
}

//----------------------------------------------------------------------------
/**
 *	@brief	友達データビューアーアップデート	ボタンの表示のみ
 *
 *	@param	wk	システムワーク 
 */
//-----------------------------------------------------------------------------
static void MCVSys_UpdataBttn( WIFIP2PMATCH_WORK *wk )
{
	if( wk->view.user_disp == MCV_USERDISP_OFF ){

		// 表示メイン
		if( wk->view.button_on == TRUE ){
			MCVSys_BttnDraw( wk );
			wk->view.button_on = FALSE;
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ユーザーディスプレイを終了するかチェック
 *
 *	@param	wk			ワーク
 *	@param	oambttn_ret	OAMボタンメイン戻り値
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	しない
 */
//-----------------------------------------------------------------------------
static BOOL MCVSys_UserDispEndCheck( WIFIP2PMATCH_WORK *wk, u32 oambttn_ret )
{
	// 強制表示しているので終了しない
	if( (wk->view.user_disp == MCV_USERDISP_ONEX) ){
		return FALSE;
	}
	
	//  移動はcontボタンはトリガー
	if( (sys.cont & (PAD_KEY_LEFT|PAD_KEY_RIGHT|PAD_KEY_UP|PAD_KEY_DOWN)) ||
		(sys.trg & (PAD_BUTTON_A|PAD_BUTTON_B|PAD_BUTTON_X)) ||
		(oambttn_ret == MCV_USERD_BTTN_RET_BACK) ){
		return TRUE;
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	今詳細表示している人の友達IDXを取得
 *
 *	@param	cp_wk	ワーク
 *		
 *	@return	友達インデックス	(0なら表示してない)
 */
//-----------------------------------------------------------------------------
static u32	MCVSys_UserDispGetFriend( const WIFIP2PMATCH_WORK* cp_wk )
{
	return cp_wk->view.touch_friendNo;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ユーザーデータ表示
 *
 *	@param	wk			ワーク
 *	@param	friendNo	友達番号
 */
//-----------------------------------------------------------------------------
static void MCVSys_UserDispOn( WIFIP2PMATCH_WORK *wk, u32 friendNo, u32 heapID )
{
	if( MCVSys_MoveCheck(wk) == TRUE ){
		wk->view.touch_friendNo = friendNo;
		wk->view.touch_frame	= 2;
		wk->view.user_disp		= MCV_USERDISP_ONEX;
		wk->view.user_dispno = WF_USERDISPTYPE_NRML;
		MCVSys_UserDispDraw( wk, heapID );
		MCVSys_OamBttnOnNoBack( wk );	//  強制表示用ボタン表示
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ユーザーデータOFF
 *
 *	@param	wk			ワーク
 */
//-----------------------------------------------------------------------------
static void MCVSys_UserDispOff( WIFIP2PMATCH_WORK *wk )
{
	if( MCVSys_MoveCheck(wk) == TRUE ){
		wk->view.touch_friendNo = 0;
		wk->view.touch_frame = 0;
		wk->view.user_disp		= MCV_USERDISP_OFF;
		wk->view.button_on		= TRUE;
		wk->view.bttn_allchg = TRUE;
		MCVSys_OamBttnOff( wk );	// ボタンOFF
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ページ変更処理
 *	
 *	@param	wk				ワーク	
 *	@param	oambttn_ret		ボタン戻り値
 */
//-----------------------------------------------------------------------------
static void MCVSys_UserDispPageChange( WIFIP2PMATCH_WORK *wk, u32 oambttn_ret )
{
	u32 typenum;

	// フロンティア有無で、表示数を変える
	if( _frontierInCheck(wk) == TRUE ){
		typenum = WF_USERDISPTYPE_NUM;
	}else{
		typenum = WF_USERDISPTYPE_MINI+1;	// ミニゲームまで表示
	}
	
	if( oambttn_ret == MCV_USERD_BTTN_RET_RIGHT ){
		wk->view.user_dispno = (wk->view.user_dispno+1) % typenum;
	}else{
		wk->view.user_dispno --;
		if( wk->view.user_dispno < 0 ){
			wk->view.user_dispno += typenum;
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ボタンイベントコールバック
 *
 *	@param	bttnid		ボタンID
 *	@param	event		イベント種類
 *	@param	p_work		ワーク
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnCallBack( u32 bttnid, u32 event, void* p_work )
{
	WIFIP2PMATCH_WORK *wk = p_work;
	u32 friendNo;

	// 友達番号取得
	friendNo = (wk->view.frame_no * WCR_MAPDATA_1BLOCKOBJNUM) + bttnid;
	friendNo ++;	// 自分が０だから

	// 動作中かチェック
	if( wk->view.bttnfriendNo[ friendNo-1 ] != MCV_BTTN_FRIEND_TYPE_IN ){
		// 動作していないので何もしない
		return ;
	}

	// すでにボタン動作中なので反応しない
	if( wk->view.touch_friendNo != 0 ){
		return ;
	}

	switch( event ){
	case BMN_EVENT_TOUCH:		///< 触れた瞬間
		wk->view.touch_friendNo = friendNo;
		Snd_SePlay( _SE_DESIDE );
		break;

	default:
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	グラフィックデータ設定
 *
 *	@param	wk			システムワーク
 *	@param	p_handle	アーカイブハンドル
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void MCVSys_GraphicSet( WIFIP2PMATCH_WORK *wk, ARCHANDLE* p_handle, u32 heapID )
{
	int i, j;
	int x, y;
	
	// BG設定
	// FRAME0_Sスクリーンデータクリーン
    GF_BGL_ScrClear( wk->bgl, GF_BGL_FRAME0_S );
	// パレット転送
	ArcUtil_HDL_PalSet( p_handle, NARC_wifip2pmatch_wf_match_btm_NCLR,	// 背景用
			PALTYPE_SUB_BG, MCV_PAL_BACK*32, MCV_PAL_BACK_NUM*32, heapID );
	ArcUtil_HDL_PalSet( p_handle, NARC_wifip2pmatch_wf_match_btm_button_NCLR,	// ボタン用
			PALTYPE_SUB_BG, MCV_PAL_BTTN*32, MCV_PAL_BTTN_NUM*32, heapID );

	// キャラクタ転送
	ArcUtil_HDL_BgCharSet( p_handle, NARC_wifip2pmatch_wf_match_btm_NCGR,
			wk->bgl, GF_BGL_FRAME0_S, MCV_CGX_BACK, 0, FALSE, heapID );
	ArcUtil_HDL_BgCharSet( p_handle, NARC_wifip2pmatch_wf_match_btm_button_NCGR,
			wk->bgl, GF_BGL_FRAME2_S, MCV_CGX_BTTN2, 0, FALSE, heapID );

	// スクリーン読込みor転送
	// 背景はキャラクタ位置がずれているのでスクリーンデータを書き換える	
	ArcUtil_HDL_ScrnSet( p_handle, NARC_wifip2pmatch_wf_match_btm_NSCR, wk->bgl, GF_BGL_FRAME0_S, 0, 0, FALSE, heapID );
	
	// ボタンスクリーン読込み
	wk->view.p_bttnbuff = ArcUtil_HDL_ScrnDataGet( p_handle, NARC_wifip2pmatch_wf_match_btm_button_NSCR, FALSE, &wk->view.p_bttnscrn, heapID );
	MCVSys_GraphicScrnCGOfsChange( wk->view.p_bttnscrn, MCV_CGX_BTTN2 );

	// ユーザースクリーン読込み
	for( i=0; i<WF_USERDISPTYPE_NUM; i++ ){
		wk->view.p_userbuff[i] = ArcUtil_HDL_ScrnDataGet( p_handle, NARC_wifip2pmatch_wf_match_btm_result00_NSCR+i, FALSE, &wk->view.p_userscrn[i], heapID );
		MCVSys_GraphicScrnCGOfsChange( wk->view.p_userscrn[i], MCV_CGX_BTTN2 );
	}

	// ダミースクリーン読み込み
	wk->view.p_useretcbuff = ArcUtil_HDL_ScrnDataGet( p_handle, NARC_wifip2pmatch_wf_match_btm_etc_NSCR, FALSE, &wk->view.p_useretcscrn, heapID );
	MCVSys_GraphicScrnCGOfsChange( wk->view.p_useretcscrn, MCV_CGX_BTTN2 );
	
	

	// フォントパレット読込み
	TalkFontPaletteLoad( PALTYPE_SUB_BG, MCV_SYSFONT_PAL*32, heapID );

	// ビットマップ作成
	for( i=0; i<WCR_MAPDATA_1BLOCKOBJNUM; i++ ){
		x = i/4;	// 配置は2*4
		y = i%4;	
		GF_BGL_BmpWinInit( &wk->view.nameWin[i] );
		GF_BGL_BmpWinAdd( wk->bgl, &wk->view.nameWin[i],
				GF_BGL_FRAME1_S, 
				MCV_NAMEWIN_DEFX+(MCV_NAMEWIN_OFSX*x),
				MCV_NAMEWIN_DEFY+(MCV_NAMEWIN_OFSY*y),
				MCV_NAMEWIN_SIZX, MCV_NAMEWIN_SIZY,
				MCV_SYSFONT_PAL, MCV_NAMEWIN_CGX+(MCV_NAMEWIN_CGSIZ*i) );
		// 透明にして展開
		GF_BGL_BmpWinDataFill( &wk->view.nameWin[i], 0 );
		GF_BGL_BmpWinOnVReq( &wk->view.nameWin[i] );

		// 状態面書き込み先
		for( j=0; j<WF_VIEW_STATUS_NUM; j++ ){
			GF_BGL_BmpWinInit( &wk->view.statusWin[i][j] );
			GF_BGL_BmpWinAdd( wk->bgl, &wk->view.statusWin[i][j],
					GF_BGL_FRAME1_S, 
					MCV_STATUSWIN_DEFX+(MCV_STATUSWIN_OFSX*x)+(j*MCV_STATUSWIN_VCHATX),
					MCV_STATUSWIN_DEFY+(MCV_STATUSWIN_OFSY*y),
					MCV_STATUSWIN_SIZX, MCV_STATUSWIN_SIZY,
					PLAYER_DISP_ICON_PLTTOFS_SUB, 
					MCV_STATUSWIN_CGX+(MCV_STATUSWIN_CGSIZ*((i*2)+j)) );
			GF_BGL_BmpWinDataFill( &wk->view.statusWin[i][j], 0 );
			GF_BGL_BmpWinOnVReq( &wk->view.statusWin[i][j] );
		}
		
	}
	GF_BGL_BmpWinAdd( wk->bgl, &wk->view.userWin,
			GF_BGL_FRAME3_S, 
			MCV_USERWIN_X, MCV_USERWIN_Y,
			MCV_USERWIN_SIZX, MCV_USERWIN_SIZY,
			MCV_SYSFONT_PAL, MCV_USERWIN_CGX );
	// 透明にして展開
	GF_BGL_BmpWinDataFill( &wk->view.userWin, 0 );
	GF_BGL_BmpWinOnVReq( &wk->view.userWin );


	// サブ画面OAM初期化
	MCVSys_OamBttnInit( wk, p_handle, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	グラフィックデータ破棄
 *
 *	@param	wk	システムワーク
 */
//-----------------------------------------------------------------------------
static void MCVSys_GraphicDel( WIFIP2PMATCH_WORK *wk )
{
	int i, j;

	// サブ画面OAM破棄
	MCVSys_OamBttnDelete( wk );
	
	// ビットマップ破棄
	for( i=0; i<WCR_MAPDATA_1BLOCKOBJNUM; i++ ){
		GF_BGL_BmpWinDel( &wk->view.nameWin[i] );
		for( j=0; j<WF_VIEW_STATUS_NUM; j++ ){
			GF_BGL_BmpWinDel( &wk->view.statusWin[i][j] );
		}
	}
	GF_BGL_BmpWinDel( &wk->view.userWin );
	
	// ボタンスクリーン破棄
	sys_FreeMemoryEz( wk->view.p_bttnbuff );

	// ユーザーウインドウ
	for( i=0; i<WF_USERDISPTYPE_NUM; i++ ){
		sys_FreeMemoryEz( wk->view.p_userbuff[i] );
	}

	// ダミースクリーン破棄
	sys_FreeMemoryEz( wk->view.p_useretcbuff );
}

//----------------------------------------------------------------------------
/**
 *	@brief	SCRNのキャラクタNoを転送先のアドレス分足す
 *
 *	@param	p_scrn	スクリーンデータ
 *	@param	cgofs	キャラクタオフセット（キャラクタ単位）
 */
//-----------------------------------------------------------------------------
static void MCVSys_GraphicScrnCGOfsChange( NNSG2dScreenData* p_scrn, u8 cgofs )
{
	u16* p_scrndata;
	int i, j;
	int siz_x, siz_y;

	p_scrndata = (u16*)p_scrn->rawData;
	siz_x = p_scrn->screenWidth/8;
	siz_y = p_scrn->screenHeight/8;

	for( i=0; i<siz_y; i++ ){
		for( j=0; j<siz_x; j++ ){
			p_scrndata[ (i*siz_x)+j ] += cgofs;
		}  
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ボタン設定
 *
 *	@param	wk			システムワーク
 *	@param	friendNo	友達番号
 *	@param	type		設定タイプ
 *
 *	type
 *		MCV_BTTN_FRIEND_TYPE_RES,	// 予約
 *		MCV_BTTN_FRIEND_TYPE_IN,	// 登録済み
 *		
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnSet( WIFIP2PMATCH_WORK *wk, u32 friendNo, u32 type )
{
	GF_ASSERT( friendNo > 0 );
	GF_ASSERT( type > MCV_BTTN_FRIEND_TYPE_NONE );
	GF_ASSERT( type < MCV_BTTN_FRIEND_TYPE_MAX );
	wk->view.bttnfriendNo[ friendNo-1 ] = type;
	wk->view.button_on = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ボタンからはずす
 *
 *	@param	wk			システムワーク
 *	@param	friendNo	友達番号
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnDel( WIFIP2PMATCH_WORK *wk, u32 friendNo )
{
	GF_ASSERT( friendNo > 0 );
	wk->view.bttnfriendNo[ friendNo-1 ] = MCV_BTTN_FRIEND_TYPE_NONE;
	wk->view.button_on = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ボタンの表示タイプを取得する
 *
 *	@param	wk			ワーク
 *	@param	friendNo	友達number
 *	
 *	@return	表示タイプ
 *	type
 *		MCV_BTTN_FRIEND_TYPE_RES,	// 予約
 *		MCV_BTTN_FRIEND_TYPE_IN,	// 登録済み
 *		MCV_BTTN_FRIEND_TYPE_NONE,	// なし
 */
//-----------------------------------------------------------------------------
static u32 MCVSys_BttnTypeGet( const WIFIP2PMATCH_WORK *wk, u32 friendNo )
{
	GF_ASSERT( friendNo > 0 );
	return wk->view.bttnfriendNo[ friendNo-1 ];
}

//----------------------------------------------------------------------------
/**
 *	@brief	強制書き直し
 *
 *	@param	wk			システムワーク
 */
//-----------------------------------------------------------------------------
static void MCVSys_ReWrite( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
	if( wk->view.user_disp != MCV_USERDISP_OFF ){
		MCVSys_UserDispDraw( wk, heapID );
	}else{
		wk->view.bttn_allchg = TRUE;
		MCVSys_BttnDraw( wk );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ボタンをすべて書くリクエストを出す
 *
 *	@param	wk			
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnAllWriteReq( WIFIP2PMATCH_WORK *wk )
{
	wk->view.bttn_allchg = TRUE;
	wk->view.button_on = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	背景描画
 *
 *	@param	wk	システムワーク
 */
//-----------------------------------------------------------------------------
static void MCVSys_BackDraw( WIFIP2PMATCH_WORK *wk )
{
	// 背景のカラーを変える
	GF_BGL_ScrPalChange( wk->bgl, GF_BGL_FRAME0_S, 0, 0,
			32, 24, wk->view.frame_no+MCV_PAL_FRMNO );

	GF_BGL_LoadScreenV_Req( wk->bgl, GF_BGL_FRAME0_S );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ボタン表示
 *
 *	@param	wk	システムワーク
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnDraw( WIFIP2PMATCH_WORK *wk )
{
	int i, j;
	int sex;
	int bttn_type;
	int x, y;
	int friend_no;
	int frame;
	BOOL write_change_masterflag;	// 強制的にすべてのボタンを書き換える
	BOOL write_change_localflag;	// ここのボタンごとの書き換えチェック

	// 全書き換えチェック
	if( wk->view.bttn_allchg == TRUE ){
		wk->view.bttn_allchg = FALSE;
		write_change_masterflag = TRUE;

		// スクリーンクリア
		GF_BGL_ScrFill( wk->bgl, GF_BGL_FRAME2_S, 0, 0, 0, 32, 24, 0 );
	}else{
		write_change_masterflag = FALSE;
	}


	for( i=0; i<WCR_MAPDATA_1BLOCKOBJNUM; i++ ){
		
		friend_no = (wk->view.frame_no*WCR_MAPDATA_1BLOCKOBJNUM) + i + 1;
		// 表示タイプ設定
		if( wk->view.bttnfriendNo[friend_no - 1] != MCV_BTTN_FRIEND_TYPE_NONE ){
			sex = WifiList_GetFriendInfo( wk->pList, friend_no - 1, WIFILIST_FRIEND_SEX );
			if(sex == PM_MALE){
				bttn_type = MCV_BUTTON_TYPE_MAN;
			}else{
				bttn_type = MCV_BUTTON_TYPE_GIRL;
			}
		}else{
			bttn_type = MCV_BUTTON_TYPE_NONE;
		}

		// ボタン表示座標
		x = i/4;
		y = i%4;

		// 押されているボタンかチェック
		if( friend_no == wk->view.touch_friendNo ){
			frame = wk->view.touch_frame;
		}else{
			frame = 0;
		}


		// ボタンの形が変わっているかチェック
		if( (friend_no == wk->view.touch_friendNo) || 
			(friend_no == wk->view.bttn_chg_friendNo) ){
			write_change_localflag = TRUE;
		}else{
			write_change_localflag = FALSE;
		}

		// 書き換えるかチェック
		if( (write_change_masterflag == TRUE) ||
			(write_change_localflag == TRUE) ){

			// ボタン
			MCVSys_BttnWrite( wk, 
					MCV_BUTTON_DEFX+(MCV_BUTTON_OFSX*x), MCV_BUTTON_DEFY+(MCV_BUTTON_OFSY*y),
					bttn_type, frame );
			
			// 名前表示
			if( bttn_type != MCV_BUTTON_TYPE_NONE ){

				// 名前の表示
				MCVSys_BttnWinDraw( wk, &wk->view.nameWin[i], friend_no, frame, i );	
				MCVSys_BttnStatusWinDraw( wk, wk->view.statusWin[i], friend_no, frame, i );
			}else{

				// 透明にする
				GF_BGL_BmpWinDataFill( &wk->view.nameWin[i], 0 );
				GF_BGL_BmpWinOnVReq( &wk->view.nameWin[i] );
				for( j=0; j<WF_VIEW_STATUS_NUM; j++ ){
					GF_BGL_BmpWinDataFill( &wk->view.statusWin[i][j], 0 );
					GF_BGL_BmpWinOnVReq( &wk->view.statusWin[i][j] );
				}
			}
		}
	}

	GF_BGL_LoadScreenV_Req( wk->bgl, GF_BGL_FRAME2_S );

	// メッセージ面の表示設定
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );	
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ユーザーデータ表示
 *
 *	@param	wk	システムワーク
 */
//-----------------------------------------------------------------------------
static void MCVSys_UserDispDraw( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
	static void (*writeFunc[ WF_USERDISPTYPE_NUM ])( WIFIP2PMATCH_WORK *wk, u32 heapID ) = 
	{
		MCVSys_UserDispDrawType00,
		MCVSys_UserDispDrawType06,
		MCVSys_UserDispDrawType01,
		MCVSys_UserDispDrawType02,
		MCVSys_UserDispDrawType03,
		MCVSys_UserDispDrawType04,
		MCVSys_UserDispDrawType05
	};
	int sex;
	int pal;

	sex = WifiList_GetFriendInfo( wk->pList, 
			wk->view.touch_friendNo - 1, WIFILIST_FRIEND_SEX );

	if(sex == PM_FEMALE){
		pal = MCV_PAL_BTTN+MCV_PAL_BTTNST_GIRL;
	}else{
		pal = MCV_PAL_BTTN+MCV_PAL_BTTNST_MAN;
	}
	
	GF_BGL_ScreenBufSet( wk->bgl, GF_BGL_FRAME2_S, wk->view.p_userscrn[wk->view.user_dispno]->rawData,
			wk->view.p_userscrn[wk->view.user_dispno]->szByte );

	GF_BGL_ScrPalChange( wk->bgl, GF_BGL_FRAME2_S, 0, 0,
			32, 24, pal );

	// BG３面のスクリーンをクリア
    GF_BGL_ScrClear(wk->bgl, GF_BGL_FRAME3_S);

	// フロンティア非表示モードチェック
	if( _frontierInCheck( wk ) == FALSE ){
		MCVSys_UserDispDrawFrontierOffScrn( wk );	// 非表示スクリーン設定
	}
	

	// その人のことを描画
    GF_BGL_BmpWinDataFill( &wk->view.userWin, 0x0 );

	// 描画
	writeFunc[ wk->view.user_dispno ]( wk, heapID );

	GF_BGL_LoadScreenV_Req( wk->bgl, GF_BGL_FRAME2_S );
    GF_BGL_BmpWinOnVReq(&wk->view.userWin);

	// メッセージ面の表示設定
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );	
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
}

// 通常
#ifdef NONEQUIVALENT
static void MCVSys_UserDispDrawType00( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
	int sex;
	int col;
	int msg_id;
	int friendNo;
	int num;
	int vct_icon;
	_WIFI_MACH_STATUS* p_status;
	u32 status;
	
	friendNo = wk->view.touch_friendNo - 1;

	sex = WifiList_GetFriendInfo( wk->pList, friendNo, WIFILIST_FRIEND_SEX );

	// トレーナー名
	if( sex == PM_MALE ){
		col = _COL_N_BLUE;
	}else{
		col = _COL_N_RED;
	}
    MCVSys_FriendNameSet(wk, friendNo);
    MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_033, wk->pExpStrBuf );
    WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );
    GF_STR_PrintColor( &wk->view.userWin, FONT_TALK, wk->TitleString, 
			MCV_USERD_NAME_X, MCV_USERD_NAME_Y, MSG_NO_PUT, col, NULL);

	p_status = WifiFriendMatchStatusGet( wk, friendNo );
	status = _WifiMyStatusGet( wk, p_status );

	// 状態
	msg_id = MCVSys_StatusMsgIdGet( status, &col );
    MSGMAN_GetString(wk->MsgManager, msg_id, wk->pExpStrBuf);
	GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->pExpStrBuf,
					   MCV_USERD_ST_X, MCV_USERD_ST_Y,
					   MSG_NO_PUT, col, NULL);

	// グループ
    {
        MYSTATUS* pTarget = MyStatus_AllocWork(HEAPID_WIFIP2PMATCH);
        MyStatus_SetMyName(pTarget, WifiList_GetFriendGroupNamePtr(wk->pList,friendNo));
        WORDSET_RegisterPlayerName( wk->view.p_wordset, 0, pTarget);
        sys_FreeMemoryEz(pTarget);
		MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_034, wk->pExpStrBuf );
		WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_GR_X, MCV_USERD_GR_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);
    }

    // 対戦成績
	{
		MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_035, wk->TitleString );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, 
				MCV_USERD_VS_X, MCV_USERD_VS_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);
		// かち
		num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_BATTLE_WIN);
		WORDSET_RegisterNumber(wk->view.p_wordset, 0, num, 4, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
		MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_036, wk->pExpStrBuf );
		WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_VS_WIN_X, MCV_USERD_VS_WIN_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);
		// まけ
		num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_BATTLE_LOSE);
		WORDSET_RegisterNumber(wk->view.p_wordset, 0, num, 4, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
		MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_037, wk->pExpStrBuf );
		WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, 
				MCV_USERD_VS_LOS_X, MCV_USERD_VS_WIN_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);
	}
	// ポケモン交換
	{
		MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_038, wk->TitleString );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_TR_X,  MCV_USERD_TR_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);

		num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_TRADE_NUM);
		WORDSET_RegisterNumber(wk->view.p_wordset, 0, num, 4, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
		MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_039, wk->pExpStrBuf );
		WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );

		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_TRNUM_X, MCV_USERD_TR_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);
	}

	// 料理数
	if( _pofinCaseCheck( wk ) == TRUE ){
		MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_102, wk->TitleString );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_POFIN_X,  MCV_USERD_POFIN_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);

		num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_POFIN_NUM);
		WORDSET_RegisterNumber(wk->view.p_wordset, 0, num, 4, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
		MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_103, wk->pExpStrBuf );
		WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );

		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_POFINNUM_X, MCV_USERD_POFIN_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);

	}
	
	// 最後に遊んだ日付
	{
		MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_040, wk->TitleString );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, 
				MCV_USERD_DAY_X, MCV_USERD_DAY_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);

		num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_LASTBT_DAY);
		if(num!=0){
			WORDSET_RegisterNumber(wk->view.p_wordset, 2, num, 2, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
			num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_LASTBT_YEAR);
			WORDSET_RegisterNumber(wk->view.p_wordset, 0, num, 4, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
			num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_LASTBT_MONTH);
			WORDSET_RegisterNumber(wk->view.p_wordset, 1, num, 2, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
			MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_041, wk->pExpStrBuf );
			WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );
			GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, 
					MCV_USERD_DAYNUM_X, MCV_USERD_DAY_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);
		}
	}

	// アイコン
	WifiP2PMatchFriendListStIconWrite( wk->bgl, &wk->icon, GF_BGL_FRAME2_S, 
			MCV_USERD_ICON_X, MCV_USERD_ICON_Y,
			status );
	if( p_status->vchat ){
		vct_icon = PLAYER_DISP_ICON_IDX_NONE;
	}else{
		vct_icon = PLAYER_DISP_ICON_IDX_VCTNOT;
	}
	WifiP2PMatchFriendListIconWrite( wk->bgl, &wk->icon, GF_BGL_FRAME2_S, 
			MCV_USERD_VCTICON_X, MCV_USERD_ICON_Y,
			vct_icon, 0 );

}
#else
asm static void MCVSys_UserDispDrawType00( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
	push {r3, r4, r5, r6, r7, lr}
	sub sp, #0x18
	add r5, r0, #0
	ldr r0, =0x00000B89 // _02233CD4
	mov r2, #8
	ldrb r0, [r5, r0]
	sub r4, r0, #1
	ldr r0, [r5, #0]
	add r1, r4, #0
	bl WifiList_GetFriendInfo
	cmp r0, #0
	bne _0223395E
	ldr r0, =0x00050600 // _02233CD8
	b _02233962
_0223395E:
	mov r0, #0xc1
	lsl r0, r0, #0xa
_02233962:
	str r0, [sp, #0x14]
	add r0, r5, #0
	add r1, r4, #0
	bl MCVSys_FriendNameSet
	mov r2, #0x5a
	lsl r2, r2, #2
	ldr r0, [r5, r2]
	add r2, #8
	ldr r2, [r5, r2]
	mov r1, #0x2a
	bl MSGMAN_GetString
	mov r2, #0x5e
	ldr r0, =0x00000B18 // _02233CDC
	lsl r2, r2, #2
	ldr r1, [r5, r2]
	sub r2, #8
	ldr r0, [r5, r0]
	ldr r2, [r5, r2]
	bl WORDSET_ExpandStr
	mov r0, #8
	str r0, [sp]
	mov r2, #0xff
	str r2, [sp, #4]
	ldr r0, [sp, #0x14]
	add r2, #0x79
	str r0, [sp, #8]
	mov r0, #0
	str r0, [sp, #0xc]
	ldr r0, =0x00000D14 // _02233CE0
	ldr r2, [r5, r2]
	add r0, r5, r0
	mov r1, #1
	mov r3, #0x20
	bl GF_STR_PrintColor
	add r0, r5, #0
	add r1, r4, #0
	bl WifiFriendMatchStatusGet
	str r0, [sp, #0x10]
	ldr r1, [sp, #0x10]
	add r0, r5, #0
	bl _WifiMyStatusGet
	add r1, sp, #0x14
	add r7, r0, #0
	bl MCVSys_StatusMsgIdGet
	mov r2, #0x5a
	lsl r2, r2, #2
	add r1, r0, #0
	ldr r0, [r5, r2]
	add r2, #8
	ldr r2, [r5, r2]
	bl MSGMAN_GetString
	mov r0, #8
	str r0, [sp]
	mov r2, #0xff
	str r2, [sp, #4]
	ldr r0, [sp, #0x14]
	mov r1, #0
	str r0, [sp, #8]
	ldr r0, =0x00000D14 // _02233CE0
	str r1, [sp, #0xc]
	add r2, #0x71
	ldr r2, [r5, r2]
	add r0, r5, r0
	mov r3, #0x68
	bl GF_STR_PrintColor
	mov r0, #0x36
	bl MyStatus_AllocWork
	add r6, r0, #0
	ldr r0, [r5, #0]
	add r1, r4, #0
	bl WifiList_GetFriendGroupNamePtr
	add r1, r0, #0
	add r0, r6, #0
	bl MyStatus_SetMyName
	ldr r0, =0x00000B18 // _02233CDC
	mov r1, #0
	ldr r0, [r5, r0]
	add r2, r6, #0
	bl WORDSET_RegisterPlayerName
	add r0, r6, #0
	bl sys_FreeMemoryEz
	mov r2, #0x5a
	lsl r2, r2, #2
	ldr r0, [r5, r2]
	add r2, #0x10
	ldr r2, [r5, r2]
	mov r1, #0x2b
	bl MSGMAN_GetString
	mov r0, #0x20
	str r0, [sp]
	mov r2, #0xff
	str r2, [sp, #4]
	ldr r0, =0x00010200 // _02233CE4
	mov r1, #0
	str r0, [sp, #8]
	ldr r0, =0x00000D14 // _02233CE0
	str r1, [sp, #0xc]
	add r2, #0x79
	ldr r2, [r5, r2]
	add r0, r5, r0
	mov r3, #8
	bl GF_STR_PrintColor
	mov r1, #0x96
	mov r0, #0x5a
	add r2, r1, #0
	lsl r0, r0, #2
	add r2, #0xda
	ldr r0, [r5, r0]
	ldr r2, [r5, r2]
	bl MSGMAN_GetString
	mov r2, #0x5e
	ldr r0, =0x00000B18 // _02233CDC
	lsl r2, r2, #2
	ldr r1, [r5, r2]
	sub r2, #8
	ldr r0, [r5, r0]
	ldr r2, [r5, r2]
	bl WORDSET_ExpandStr
	mov r1, #0x5e
	lsl r1, r1, #2
	mov r0, #0
	ldr r1, [r5, r1]
	add r2, r0, #0
	bl FontProc_GetPrintStrWidth
	mov r1, #0xe8
	sub r3, r1, r0
	mov r0, #0x20
	str r0, [sp]
	mov r2, #0xff
	str r2, [sp, #4]
	ldr r0, =0x00010200 // _02233CE4
	mov r1, #0
	str r0, [sp, #8]
	ldr r0, =0x00000D14 // _02233CE0
	str r1, [sp, #0xc]
	add r2, #0x79
	ldr r2, [r5, r2]
	add r0, r5, r0
	bl GF_STR_PrintColor
	mov r2, #0x5a
	lsl r2, r2, #2
	ldr r0, [r5, r2]
	add r2, #0x10
	ldr r2, [r5, r2]
	mov r1, #0x2c
	bl MSGMAN_GetString
	mov r0, #0x38
	str r0, [sp]
	mov r2, #0xff
	str r2, [sp, #4]
	ldr r0, =0x00010200 // _02233CE4
	mov r1, #0
	str r0, [sp, #8]
	ldr r0, =0x00000D14 // _02233CE0
	str r1, [sp, #0xc]
	add r2, #0x79
	ldr r2, [r5, r2]
	add r0, r5, r0
	mov r3, #8
	bl GF_STR_PrintColor
	ldr r0, [r5, #0]
	add r1, r4, #0
	mov r2, #1
	bl WifiList_GetFriendInfo
	add r2, r0, #0
	mov r0, #1
	str r0, [sp]
	str r0, [sp, #4]
	ldr r0, =0x00000B18 // _02233CDC
	mov r1, #0
	ldr r0, [r5, r0]
	mov r3, #4
	bl WORDSET_RegisterNumber
	mov r2, #0x5a
	lsl r2, r2, #2
	ldr r0, [r5, r2]
	add r2, #8
	ldr r2, [r5, r2]
	mov r1, #0x2d
	bl MSGMAN_GetString
	mov r2, #0x5e
	ldr r0, =0x00000B18 // _02233CDC
	lsl r2, r2, #2
	ldr r1, [r5, r2]
	sub r2, #8
	ldr r0, [r5, r0]
	ldr r2, [r5, r2]
	bl WORDSET_ExpandStr
	mov r0, #0x38
	str r0, [sp]
	mov r2, #0xff
	str r2, [sp, #4]
	ldr r0, =0x00010200 // _02233CE4
	mov r1, #0
	str r0, [sp, #8]
	ldr r0, =0x00000D14 // _02233CE0
	str r1, [sp, #0xc]
	add r2, #0x79
	ldr r2, [r5, r2]
	add r0, r5, r0
	mov r3, #0x78
	bl GF_STR_PrintColor
	ldr r0, [r5, #0]
	add r1, r4, #0
	mov r2, #2
	bl WifiList_GetFriendInfo
	add r2, r0, #0
	mov r0, #1
	str r0, [sp]
	str r0, [sp, #4]
	ldr r0, =0x00000B18 // _02233CDC
	mov r1, #0
	ldr r0, [r5, r0]
	mov r3, #4
	bl WORDSET_RegisterNumber
	mov r0, #0x5a
	mov r1, #0x2e
	lsl r0, r0, #2
	lsl r2, r1, #3
	ldr r0, [r5, r0]
	ldr r2, [r5, r2]
	bl MSGMAN_GetString
	mov r2, #0x5e
	ldr r0, =0x00000B18 // _02233CDC
	lsl r2, r2, #2
	ldr r1, [r5, r2]
	sub r2, #8
	ldr r0, [r5, r0]
	ldr r2, [r5, r2]
	bl WORDSET_ExpandStr
	mov r1, #0x5e
	lsl r1, r1, #2
	mov r0, #0
	ldr r1, [r5, r1]
	add r2, r0, #0
	bl FontProc_GetPrintStrWidth
	mov r1, #0xe8
	sub r3, r1, r0
	mov r0, #0x38
	str r0, [sp]
	mov r2, #0xff
	str r2, [sp, #4]
	ldr r0, =0x00010200 // _02233CE4
	mov r1, #0
	str r0, [sp, #8]
	ldr r0, =0x00000D14 // _02233CE0
	str r1, [sp, #0xc]
	add r2, #0x79
	ldr r2, [r5, r2]
	add r0, r5, r0
	bl GF_STR_PrintColor
	mov r0, #0x5a
	mov r1, #0x2f
	lsl r0, r0, #2
	lsl r2, r1, #3
	ldr r0, [r5, r0]
	ldr r2, [r5, r2]
	bl MSGMAN_GetString
	mov r0, #0x50
	str r0, [sp]
	mov r2, #0xff
	str r2, [sp, #4]
	ldr r0, =0x00010200 // _02233CE4
	mov r1, #0
	str r0, [sp, #8]
	ldr r0, =0x00000D14 // _02233CE0
	str r1, [sp, #0xc]
	add r2, #0x79
	ldr r2, [r5, r2]
	add r0, r5, r0
	mov r3, #8
	bl GF_STR_PrintColor
	ldr r0, [r5, #0]
	add r1, r4, #0
	mov r2, #3
	bl WifiList_GetFriendInfo
	add r2, r0, #0
	mov r0, #1
	str r0, [sp]
	str r0, [sp, #4]
	ldr r0, =0x00000B18 // _02233CDC
	mov r1, #0
	ldr r0, [r5, r0]
	mov r3, #4
	bl WORDSET_RegisterNumber
	mov r2, #0x5a
	lsl r2, r2, #2
	ldr r0, [r5, r2]
	add r2, #8
	ldr r2, [r5, r2]
	mov r1, #0x30
	bl MSGMAN_GetString
	mov r2, #0x5e
	ldr r0, =0x00000B18 // _02233CDC
	lsl r2, r2, #2
	ldr r1, [r5, r2]
	sub r2, #8
	ldr r0, [r5, r0]
	ldr r2, [r5, r2]
	bl WORDSET_ExpandStr
	mov r1, #0x5e
	lsl r1, r1, #2
	mov r0, #0
	ldr r1, [r5, r1]
	add r2, r0, #0
	bl FontProc_GetPrintStrWidth
	mov r1, #0xe8
	sub r3, r1, r0
	mov r0, #0x50
	str r0, [sp]
	mov r2, #0xff
	str r2, [sp, #4]
	ldr r0, =0x00010200 // _02233CE4
	mov r1, #0
	str r0, [sp, #8]
	ldr r0, =0x00000D14 // _02233CE0
	str r1, [sp, #0xc]
	add r2, #0x79
	ldr r2, [r5, r2]
	add r0, r5, r0
	bl GF_STR_PrintColor
	add r0, r5, #0
	bl _pofinCaseCheck
	cmp r0, #1
	bne _02233CF4
	mov r2, #0x5a
	lsl r2, r2, #2
	ldr r0, [r5, r2]
	add r2, #0x10
	ldr r2, [r5, r2]
	mov r1, #0x31
	bl MSGMAN_GetString
	mov r0, #0x68
	str r0, [sp]
	mov r2, #0xff
	str r2, [sp, #4]
	ldr r0, =0x00010200 // _02233CE4
	mov r1, #0
	str r0, [sp, #8]
	ldr r0, =0x00000D14 // _02233CE0
	str r1, [sp, #0xc]
	add r2, #0x79
	ldr r2, [r5, r2]
	add r0, r5, r0
	mov r3, #8
	bl GF_STR_PrintColor
	ldr r0, [r5, #0]
	add r1, r4, #0
	mov r2, #9
	bl WifiList_GetFriendInfo
	add r2, r0, #0
	mov r0, #1
	str r0, [sp]
	str r0, [sp, #4]
	ldr r0, =0x00000B18 // _02233CDC
	mov r1, #0
	ldr r0, [r5, r0]
	mov r3, #4
	bl WORDSET_RegisterNumber
	mov r2, #0x5a
	lsl r2, r2, #2
	ldr r0, [r5, r2]
	add r2, #8
	ldr r2, [r5, r2]
	mov r1, #0x32
	bl MSGMAN_GetString
	mov r2, #0x5e
	ldr r0, =0x00000B18 // _02233CDC
	lsl r2, r2, #2
	ldr r1, [r5, r2]
	sub r2, #8
	ldr r0, [r5, r0]
	ldr r2, [r5, r2]
	bl WORDSET_ExpandStr
	mov r1, #0x5e
	lsl r1, r1, #2
	mov r0, #0
	ldr r1, [r5, r1]
	add r2, r0, #0
	bl FontProc_GetPrintStrWidth
	mov r1, #0xe8
	sub r3, r1, r0
	mov r0, #0x68
	str r0, [sp]
	mov r2, #0xff
	str r2, [sp, #4]
	ldr r0, =0x00010200 // _02233CE4
	mov r1, #0
	str r0, [sp, #8]
	ldr r0, =0x00000D14 // _02233CE0
	b _02233CE8
// _02233CD4: .4byte 0x00000B89
// _02233CD8: .4byte 0x00050600
// _02233CDC: .4byte 0x00000B18
// _02233CE0: .4byte 0x00000D14
// _02233CE4: .4byte 0x00010200
_02233CE8:
	str r1, [sp, #0xc]
	add r2, #0x79
	ldr r2, [r5, r2]
	add r0, r5, r0
	bl GF_STR_PrintColor
_02233CF4:
	mov r2, #0x5a
	lsl r2, r2, #2
	ldr r0, [r5, r2]
	add r2, #0x10
	ldr r2, [r5, r2]
	mov r1, #0x33
	bl MSGMAN_GetString
	mov r0, #0x80
	str r0, [sp]
	mov r2, #0xff
	str r2, [sp, #4]
	ldr r0, =0x00010200 // _02233E10
	mov r1, #0
	str r0, [sp, #8]
	ldr r0, =0x00000D14 // _02233E14
	str r1, [sp, #0xc]
	add r2, #0x79
	ldr r2, [r5, r2]
	add r0, r5, r0
	mov r3, #8
	bl GF_STR_PrintColor
	ldr r0, [r5, #0]
	add r1, r4, #0
	mov r2, #6
	bl WifiList_GetFriendInfo
	add r2, r0, #0
	beq _02233DC8
	mov r0, #0
	str r0, [sp]
	mov r0, #1
	str r0, [sp, #4]
	ldr r0, =0x00000B18 // _02233E18
	mov r1, #2
	ldr r0, [r5, r0]
	add r3, r1, #0
	bl WORDSET_RegisterNumber
	ldr r0, [r5, #0]
	add r1, r4, #0
	mov r2, #4
	bl WifiList_GetFriendInfo
	mov r1, #0
	add r2, r0, #0
	str r1, [sp]
	mov r0, #1
	str r0, [sp, #4]
	ldr r0, =0x00000B18 // _02233E18
	mov r3, #4
	ldr r0, [r5, r0]
	bl WORDSET_RegisterNumber
	ldr r0, [r5, #0]
	add r1, r4, #0
	mov r2, #5
	bl WifiList_GetFriendInfo
	add r2, r0, #0
	ldr r0, =0x00000B18 // _02233E18
	mov r1, #1
	ldr r0, [r5, r0]
	bl WORDSET_RegisterMonthName
	mov r2, #0x5a
	lsl r2, r2, #2
	ldr r0, [r5, r2]
	add r2, #8
	ldr r2, [r5, r2]
	mov r1, #0x4a
	bl MSGMAN_GetString
	mov r2, #0x5e
	ldr r0, =0x00000B18 // _02233E18
	lsl r2, r2, #2
	ldr r1, [r5, r2]
	sub r2, #8
	ldr r0, [r5, r0]
	ldr r2, [r5, r2]
	bl WORDSET_ExpandStr
	mov r1, #0x5e
	lsl r1, r1, #2
	mov r0, #0
	ldr r1, [r5, r1]
	add r2, r0, #0
	bl FontProc_GetPrintStrWidth
	mov r1, #0xe8
	sub r3, r1, r0
	mov r0, #0x80
	str r0, [sp]
	mov r2, #0xff
	str r2, [sp, #4]
	ldr r0, =0x00010200 // _02233E10
	mov r1, #0
	str r0, [sp, #8]
	ldr r0, =0x00000D14 // _02233E14
	str r1, [sp, #0xc]
	add r2, #0x79
	ldr r2, [r5, r2]
	add r0, r5, r0
	bl GF_STR_PrintColor
_02233DC8:
	mov r3, #2
	str r3, [sp]
	mov r0, #0x57
	ldr r1, =0x00000B08 // _02233E1C
	str r7, [sp, #4]
	lsl r0, r0, #2
	ldr r0, [r5, r0]
	add r1, r5, r1
	mov r2, #6
	bl WifiP2PMatchFriendListStIconWrite
	ldr r0, [sp, #0x10]
	add r0, #0x21
	str r0, [sp, #0x10]
	ldrb r0, [r0]
	cmp r0, #0
	beq _02233DEE
	mov r1, #8
	b _02233DF0
_02233DEE:
	mov r1, #1
_02233DF0:
	mov r0, #2
	str r0, [sp]
	str r1, [sp, #4]
	mov r0, #0
	str r0, [sp, #8]
	mov r0, #0x57
	lsl r0, r0, #2
	ldr r1, =0x00000B08 // _02233E1C
	ldr r0, [r5, r0]
	add r1, r5, r1
	mov r2, #6
	mov r3, #0x1c
	bl WifiP2PMatchFriendListIconWrite
	add sp, #0x18
	pop {r3, r4, r5, r6, r7, pc}
	// .align 2, 0
// _02233E10: .4byte 0x00010200
// _02233E14: .4byte 0x00000D14
// _02233E18: .4byte 0x00000B18
// _02233E1C: .4byte 0x00000B08
}
#endif

// バトルタワー
static void MCVSys_UserDispDrawType01( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
	int friendNo;

	friendNo = wk->view.touch_friendNo - 1;

	// タイトル
	{
		MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_bf01, wk->TitleString );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_BTTW_TITLE_X,  MCV_USERD_BTTW_TITLE_Y, MSG_NO_PUT, _COL_N_WHITE, NULL);
	}

	// 前回記録
	{
		
		MCVSys_UserDispFrontiorTitleStrGet( wk, wk->TitleString, MCV_FRONTIOR_TOWOR, friendNo );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_BTTW_LAST_X,  MCV_USERD_BTTW_LAST_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);

		MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf13, 
				FRID_TOWER_MULTI_WIFI_RENSHOU_CNT, friendNo, 
				MCV_USERD_BTTW_LASTNUM_X, MCV_USERD_BTTW_LASTNUM_Y );
	}

	// 最高記録
	{
		MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_bf07, wk->TitleString );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_BTTW_MAX_X,  MCV_USERD_BTTW_MAX_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);

		MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf13, 
				FRID_TOWER_MULTI_WIFI_RENSHOU, friendNo, 
				MCV_USERD_BTTW_MAXNUM_X, MCV_USERD_BTTW_MAXNUM_Y );
	}
}

// バトルファクトリー
#ifdef NONEQUIVALENT
static void MCVSys_UserDispDrawType02( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
	int friendNo;

	friendNo = wk->view.touch_friendNo - 1;

	// タイトル
	{
		MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_bf02, wk->TitleString );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_BTTW_TITLE_X,  MCV_USERD_BTTW_TITLE_Y, MSG_NO_PUT, _COL_N_WHITE, NULL);
	}

	// LV50
	{
		// タイトル
		MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_bf08, wk->TitleString );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_BTFC_LV50_X,  MCV_USERD_BTFC_LV50_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);

		MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_bf10, wk->TitleString );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_BTFC_LV50K_X,  MCV_USERD_BTFC_LV50K_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);

		// 前回
		MCVSys_UserDispFrontiorTitleStrGet( wk, wk->TitleString, MCV_FRONTIOR_FACTORY, friendNo );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_BTFC_LV50LAST_X,  MCV_USERD_BTFC_LV50LAST_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);

		MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf13, 
				FRID_FACTORY_MULTI_WIFI_RENSHOU_CNT, friendNo, 
				MCV_USERD_BTFC_LV50LASTNUM_X, MCV_USERD_BTFC_LV50LASTNUM_Y );

		MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf14, 
				FRID_FACTORY_MULTI_WIFI_TRADE_CNT, friendNo, 
				MCV_USERD_BTFC_LV50LASTTRNUM_X, MCV_USERD_BTFC_LV50LASTTRNUM_Y );

		// 最高
		MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_bf07, wk->TitleString );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_BTFC_LV50MAX_X,  MCV_USERD_BTFC_LV50MAX_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);

		MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf13, 
				FRID_FACTORY_MULTI_WIFI_RENSHOU, friendNo, 
				MCV_USERD_BTFC_LV50MAXNUM_X, MCV_USERD_BTFC_LV50MAXNUM_Y );

		MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf14, 
				FRID_FACTORY_MULTI_WIFI_TRADE, friendNo, 
				MCV_USERD_BTFC_LV50MAXTRNUM_X, MCV_USERD_BTFC_LV50MAXTRNUM_Y );
	}

	// OPEN
	{
		// タイトル
		MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_bf09, wk->TitleString );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_BTFC_OPN_X,  MCV_USERD_BTFC_OPN_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);

		MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_bf10, wk->TitleString );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_BTFC_OPNK_X,  MCV_USERD_BTFC_OPNK_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);

		// 前回
		MCVSys_UserDispFrontiorTitleStrGet( wk, wk->TitleString, MCV_FRONTIOR_FACTORY100, friendNo );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_BTFC_OPNLAST_X,  MCV_USERD_BTFC_OPNLAST_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);

		MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf13, 
				FRID_FACTORY_MULTI_WIFI_RENSHOU100_CNT, friendNo, 
				MCV_USERD_BTFC_OPNLASTNUM_X, MCV_USERD_BTFC_OPNLASTNUM_Y );

		MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf14, 
				FRID_FACTORY_MULTI_WIFI_TRADE100_CNT, friendNo, 
				MCV_USERD_BTFC_OPNLASTTRNUM_X, MCV_USERD_BTFC_OPNLASTTRNUM_Y );

		// 最高
		MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_bf07, wk->TitleString );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_BTFC_OPNMAX_X,  MCV_USERD_BTFC_OPNMAX_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);

		MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf13, 
				FRID_FACTORY_MULTI_WIFI_RENSHOU100, friendNo, 
				MCV_USERD_BTFC_OPNMAXNUM_X, MCV_USERD_BTFC_OPNMAXNUM_Y );

		MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf14, 
				FRID_FACTORY_MULTI_WIFI_TRADE100, friendNo, 
				MCV_USERD_BTFC_OPNMAXTRNUM_X, MCV_USERD_BTFC_OPNMAXTRNUM_Y );
	}
}
#else
asm static void MCVSys_UserDispDrawType02( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
	push {r3, r4, r5, lr}
	sub sp, #0x10
	add r5, r0, #0
	ldr r0, =0x00000B89 // _0223415C
	mov r2, #0x5a
	ldrb r0, [r5, r0]
	lsl r2, r2, #2
	mov r1, #0x35
	sub r4, r0, #1
	ldr r0, [r5, r2]
	add r2, #0x10
	ldr r2, [r5, r2]
	bl MSGMAN_GetString
	mov r1, #0
	str r1, [sp]
	mov r2, #0xff
	str r2, [sp, #4]
	ldr r0, =0x000F0E00 // _02234160
	add r2, #0x79
	str r0, [sp, #8]
	str r1, [sp, #0xc]
	ldr r0, =0x00000D14 // _02234164
	ldr r2, [r5, r2]
	add r0, r5, r0
	mov r3, #8
	bl GF_STR_PrintColor
	mov r2, #0x5a
	lsl r2, r2, #2
	ldr r0, [r5, r2]
	add r2, #0x10
	ldr r2, [r5, r2]
	mov r1, #0x3c
	bl MSGMAN_GetString
	mov r0, #0x18
	str r0, [sp]
	mov r2, #0xff
	str r2, [sp, #4]
	ldr r0, =0x00010200 // _02234168
	mov r1, #0
	str r0, [sp, #8]
	ldr r0, =0x00000D14 // _02234164
	str r1, [sp, #0xc]
	add r2, #0x79
	ldr r2, [r5, r2]
	add r0, r5, r0
	mov r3, #8
	bl GF_STR_PrintColor
	mov r2, #0x5a
	lsl r2, r2, #2
	ldr r0, [r5, r2]
	add r2, #0x10
	ldr r2, [r5, r2]
	mov r1, #0x3e
	bl MSGMAN_GetString
	mov r1, #0x5e
	lsl r1, r1, #2
	mov r0, #0
	ldr r1, [r5, r1]
	add r2, r0, #0
	bl FontProc_GetPrintStrWidth
	mov r1, #0xe8
	sub r3, r1, r0
	mov r0, #0x18
	str r0, [sp]
	mov r2, #0xff
	str r2, [sp, #4]
	ldr r0, =0x00010200 // _02234168
	mov r1, #0
	str r0, [sp, #8]
	ldr r0, =0x00000D14 // _02234164
	str r1, [sp, #0xc]
	add r2, #0x79
	ldr r2, [r5, r2]
	add r0, r5, r0
	bl GF_STR_PrintColor
	mov r1, #0x5e
	lsl r1, r1, #2
	ldr r1, [r5, r1]
	add r0, r5, #0
	mov r2, #1
	add r3, r4, #0
	bl MCVSys_UserDispFrontiorTitleStrGet
	mov r0, #0x30
	str r0, [sp]
	mov r2, #0xff
	str r2, [sp, #4]
	ldr r0, =0x00010200 // _02234168
	mov r1, #0
	str r0, [sp, #8]
	ldr r0, =0x00000D14 // _02234164
	str r1, [sp, #0xc]
	add r2, #0x79
	ldr r2, [r5, r2]
	add r0, r5, r0
	mov r3, #8
	bl GF_STR_PrintColor
	mov r0, #0x68
	str r0, [sp]
	mov r0, #0x30
	str r0, [sp, #4]
	add r0, r5, #0
	mov r1, #0x41
	mov r2, #0x73
	add r3, r4, #0
	bl MCVSys_UserDispFrontiorNumDraw
	mov r0, #0xd4
	str r0, [sp]
	mov r0, #0x30
	str r0, [sp, #4]
	add r0, r5, #0
	mov r1, #0x42
	mov r2, #0x75
	add r3, r4, #0
	bl MCVSys_UserDispFrontiorNumDraw
	mov r2, #0x5a
	lsl r2, r2, #2
	ldr r0, [r5, r2]
	add r2, #0x10
	ldr r2, [r5, r2]
	mov r1, #0x3b
	bl MSGMAN_GetString
	mov r0, #0x40
	str r0, [sp]
	mov r2, #0xff
	str r2, [sp, #4]
	ldr r0, =0x00010200 // _02234168
	mov r1, #0
	str r0, [sp, #8]
	ldr r0, =0x00000D14 // _02234164
	str r1, [sp, #0xc]
	add r2, #0x79
	ldr r2, [r5, r2]
	add r0, r5, r0
	mov r3, #8
	bl GF_STR_PrintColor
	mov r0, #0x68
	str r0, [sp]
	mov r0, #0x40
	str r0, [sp, #4]
	add r0, r5, #0
	mov r1, #0x41
	mov r2, #0x72
	add r3, r4, #0
	bl MCVSys_UserDispFrontiorNumDraw
	mov r0, #0xd4
	str r0, [sp]
	mov r0, #0x40
	str r0, [sp, #4]
	add r0, r5, #0
	mov r1, #0x42
	mov r2, #0x74
	add r3, r4, #0
	bl MCVSys_UserDispFrontiorNumDraw
	mov r2, #0x5a
	lsl r2, r2, #2
	ldr r0, [r5, r2]
	add r2, #0x10
	ldr r2, [r5, r2]
	mov r1, #0x3d
	bl MSGMAN_GetString
	mov r0, #0x58
	str r0, [sp]
	mov r2, #0xff
	str r2, [sp, #4]
	ldr r0, =0x00010200 // _02234168
	mov r1, #0
	str r0, [sp, #8]
	ldr r0, =0x00000D14 // _02234164
	str r1, [sp, #0xc]
	add r2, #0x79
	ldr r2, [r5, r2]
	add r0, r5, r0
	mov r3, #8
	bl GF_STR_PrintColor
	mov r2, #0x5a
	lsl r2, r2, #2
	ldr r0, [r5, r2]
	add r2, #0x10
	ldr r2, [r5, r2]
	mov r1, #0x3e
	bl MSGMAN_GetString
	mov r1, #0x5e
	lsl r1, r1, #2
	mov r0, #0
	ldr r1, [r5, r1]
	add r2, r0, #0
	bl FontProc_GetPrintStrWidth
	mov r1, #0xe8
	sub r3, r1, r0
	mov r0, #0x58
	str r0, [sp]
	mov r2, #0xff
	str r2, [sp, #4]
	ldr r0, =0x00010200 // _02234168
	mov r1, #0
	str r0, [sp, #8]
	ldr r0, =0x00000D14 // _02234164
	str r1, [sp, #0xc]
	add r2, #0x79
	ldr r2, [r5, r2]
	add r0, r5, r0
	bl GF_STR_PrintColor
	mov r1, #0x5e
	lsl r1, r1, #2
	ldr r1, [r5, r1]
	add r0, r5, #0
	mov r2, #2
	add r3, r4, #0
	bl MCVSys_UserDispFrontiorTitleStrGet
	mov r0, #0x70
	str r0, [sp]
	mov r2, #0xff
	str r2, [sp, #4]
	ldr r0, =0x00010200 // _02234168
	mov r1, #0
	str r0, [sp, #8]
	ldr r0, =0x00000D14 // _02234164
	str r1, [sp, #0xc]
	add r2, #0x79
	ldr r2, [r5, r2]
	add r0, r5, r0
	mov r3, #8
	bl GF_STR_PrintColor
	mov r0, #0x68
	str r0, [sp]
	mov r0, #0x70
	str r0, [sp, #4]
	add r0, r5, #0
	mov r1, #0x41
	mov r2, #0x77
	add r3, r4, #0
	bl MCVSys_UserDispFrontiorNumDraw
	mov r0, #0xd4
	str r0, [sp]
	mov r0, #0x70
	str r0, [sp, #4]
	add r0, r5, #0
	mov r1, #0x42
	mov r2, #0x79
	add r3, r4, #0
	bl MCVSys_UserDispFrontiorNumDraw
	mov r2, #0x5a
	lsl r2, r2, #2
	ldr r0, [r5, r2]
	add r2, #0x10
	ldr r2, [r5, r2]
	mov r1, #0x3b
	bl MSGMAN_GetString
	mov r0, #0x80
	str r0, [sp]
	mov r2, #0xff
	str r2, [sp, #4]
	ldr r0, =0x00010200 // _02234168
	mov r1, #0
	str r0, [sp, #8]
	ldr r0, =0x00000D14 // _02234164
	str r1, [sp, #0xc]
	add r2, #0x79
	ldr r2, [r5, r2]
	add r0, r5, r0
	mov r3, #8
	bl GF_STR_PrintColor
	mov r0, #0x68
	str r0, [sp]
	mov r0, #0x80
	str r0, [sp, #4]
	add r0, r5, #0
	mov r1, #0x41
	mov r2, #0x76
	add r3, r4, #0
	bl MCVSys_UserDispFrontiorNumDraw
	mov r0, #0xd4
	str r0, [sp]
	mov r0, #0x80
	str r0, [sp, #4]
	add r0, r5, #0
	mov r1, #0x42
	mov r2, #0x78
	add r3, r4, #0
	bl MCVSys_UserDispFrontiorNumDraw
	add sp, #0x10
	pop {r3, r4, r5, pc}
	// .align 2, 0
// _0223415C: .4byte 0x00000B89
// _02234160: .4byte 0x000F0E00
// _02234164: .4byte 0x00000D14
// _02234168: .4byte 0x00010200
}
#endif

// バトルキャッスル
static void MCVSys_UserDispDrawType03( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
	int friendNo;

	friendNo = wk->view.touch_friendNo - 1;

	// タイトル
	{
		MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_bf03, wk->TitleString );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_BTKS_TITLE_X,  MCV_USERD_BTKS_TITLE_Y, MSG_NO_PUT, _COL_N_WHITE, NULL);

		MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_bf11, wk->TitleString );
        {
            // MatchComment: new change in plat US
            u32 width = 180 - (FontProc_GetPrintStrWidth(FONT_SYSTEM, wk->TitleString, 0) + 1) / 2;
            GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, width,  MCV_USERD_BTKS_K_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);
        }
	}

	// 前回記録
	{
		MCVSys_UserDispFrontiorTitleStrGet( wk, wk->TitleString, MCV_FRONTIOR_CASTLE, friendNo );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_BTKS_LAST_X,  MCV_USERD_BTKS_LAST_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);

		MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf16, 
				FRID_CASTLE_MULTI_WIFI_RENSHOU_CNT, friendNo, 
				MCV_USERD_BTKS_LASTNUM_X, MCV_USERD_BTKS_LASTNUM_Y );

		MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf15, 
				FRID_CASTLE_MULTI_WIFI_CP, friendNo, 
				MCV_USERD_BTKS_LASTCPNUM_X, MCV_USERD_BTKS_LASTCPNUM_Y );
	}

	// 最高記録
	{
		MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_bf07, wk->TitleString );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_BTKS_MAX_X,  MCV_USERD_BTKS_MAX_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);

		MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf16, 
				FRID_CASTLE_MULTI_WIFI_RENSHOU, friendNo, 
				MCV_USERD_BTKS_MAXNUM_X, MCV_USERD_BTKS_MAXNUM_Y );

		MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf15, 
				FRID_CASTLE_MULTI_WIFI_REMAINDER_CP, friendNo, 
				MCV_USERD_BTKS_MAXCPNUM_X, MCV_USERD_BTKS_MAXCPNUM_Y );
	}
}

// バトルステージ
static void MCVSys_UserDispDrawType04( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
	int friendNo;
	int num;
	FRONTIER_SAVEWORK* p_fsave;
	STRBUF* p_str;

	p_fsave = SaveData_GetFrontier( wk->pSaveData );

	friendNo = wk->view.touch_friendNo - 1;

	{
		MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_bf04, wk->TitleString );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_BTST_TITLE_X,  MCV_USERD_BTST_TITLE_Y, MSG_NO_PUT, _COL_N_WHITE, NULL);

		num = FrontierRecord_Get( p_fsave, FRID_STAGE_MULTI_WIFI_MONSNO, friendNo );
		p_str = MSGDAT_UTIL_GetMonsName( num, heapID );
		{
            // MatchComment: new change in plat US
            u32 width = 33 - (FontProc_GetPrintStrWidth(FONT_SYSTEM, p_str, 0) + 1) / 2;
            GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, p_str, width,  MCV_USERD_BTST_K_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);
        }
            
		STRBUF_Delete( p_str );
	}

	// 前回記録
	{
		MCVSys_UserDispFrontiorTitleStrGet( wk, wk->TitleString, MCV_FRONTIOR_STAGE, friendNo );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_BTST_LAST_X,  MCV_USERD_BTST_LAST_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);

		MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf13, 
				FRID_STAGE_MULTI_WIFI_RENSHOU_CNT, friendNo, 
				MCV_USERD_BTST_LASTNUM_X, MCV_USERD_BTST_LASTNUM_Y );
	}

	// 最高記録
	{
		MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_bf07, wk->TitleString );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_BTST_MAX_X,  MCV_USERD_BTST_MAX_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);

		MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf13, 
				FRID_STAGE_MULTI_WIFI_RENSHOU, friendNo, 
				MCV_USERD_BTST_MAXNUM_X, MCV_USERD_BTST_MAXNUM_Y );
	}
}

// バトルルーレット
static void MCVSys_UserDispDrawType05( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
	int friendNo;

	friendNo = wk->view.touch_friendNo - 1;

	// タイトル
	{
		MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_bf05, wk->TitleString );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_BTRT_TITLE_X,  MCV_USERD_BTRT_TITLE_Y, MSG_NO_PUT, _COL_N_WHITE, NULL);
	}

	// 前回記録
	{
		MCVSys_UserDispFrontiorTitleStrGet( wk, wk->TitleString, MCV_FRONTIOR_ROULETTE, friendNo );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_BTRT_LAST_X,  MCV_USERD_BTRT_LAST_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);

		MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf17, 
				FRID_ROULETTE_MULTI_WIFI_RENSHOU_CNT, friendNo, 
				MCV_USERD_BTRT_LASTNUM_X, MCV_USERD_BTRT_LASTNUM_Y );
	}

	// 最高記録
	{
		MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_bf07, wk->TitleString );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_BTRT_MAX_X,  MCV_USERD_BTRT_MAX_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);

		MCVSys_UserDispFrontiorNumDraw( wk, msg_wifilobby_bf17, 
				FRID_ROULETTE_MULTI_WIFI_RENSHOU, friendNo, 
				MCV_USERD_BTRT_MAXNUM_X, MCV_USERD_BTRT_MAXNUM_Y );
	}
}

// ミニゲーム
static void MCVSys_UserDispDrawType06( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
	int friendNo;
	WIFI_LIST* p_list;

	p_list	= SaveData_GetWifiListData( wk->pSaveData );

	friendNo = wk->view.touch_friendNo - 1;

	// タイトル
	{
		MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_mg01, wk->TitleString );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_MINI_TITLE_X,  MCV_USERD_MINI_TITLE_Y, MSG_NO_PUT, _COL_N_WHITE, NULL);
	}

	// たまいれ
	{
		WORDSET_RegisterWiFiLobbyGameName( wk->view.p_wordset, 0, WFLBY_GAME_BALLSLOW );
		MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_mg02, wk->pExpStrBuf );
		WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_MINI_BC_X,  MCV_USERD_MINI_BC_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);

		MCVSys_UserDispNumDraw( wk, msg_wifilobby_mg05, 
				WifiList_GetFriendInfo( p_list, friendNo, WIFILIST_FRIEND_BALLSLOW_NUM ), 
				MCV_USERD_MINI_BCNUM_X, MCV_USERD_MINI_BCNUM_Y );
	}

	// たまのり
	{
		WORDSET_RegisterWiFiLobbyGameName( wk->view.p_wordset, 0, WFLBY_GAME_BALANCEBALL );
		MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_mg02, wk->pExpStrBuf );
		WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_MINI_BB_X,  MCV_USERD_MINI_BB_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);

		MCVSys_UserDispNumDraw( wk, msg_wifilobby_mg05, 
				WifiList_GetFriendInfo( p_list, friendNo, WIFILIST_FRIEND_BALANCEBALL_NUM ),
				MCV_USERD_MINI_BBNUM_X, MCV_USERD_MINI_BBNUM_Y );
	}

	// ふうせん
	{
		WORDSET_RegisterWiFiLobbyGameName( wk->view.p_wordset, 0, WFLBY_GAME_BALLOON );
		MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_mg02, wk->pExpStrBuf );
		WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );
		GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, MCV_USERD_MINI_BL_X,  MCV_USERD_MINI_BL_Y, MSG_NO_PUT, _COL_N_BLACK, NULL);

		MCVSys_UserDispNumDraw( wk, msg_wifilobby_mg05, 
				WifiList_GetFriendInfo( p_list, friendNo, WIFILIST_FRIEND_BALLOON_NUM ),
				MCV_USERD_MINI_BLNUM_X, MCV_USERD_MINI_BLNUM_Y );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	フロンティア非表示モードのスクリーンに変更
 *
 *	@param	wk		ワーク
 */
//-----------------------------------------------------------------------------
static void MCVSys_UserDispDrawFrontierOffScrn( WIFIP2PMATCH_WORK *wk )
{
	int i;
	int roop;

	// フロンティア施設数取得
	roop = WF_USERDISPTYPE_NUM - WF_USERDISPTYPE_BLTW; 

	for( i=0; i<roop; i++ ){
		GF_BGL_ScrWriteExpand( wk->bgl, GF_BGL_FRAME2_S,
				MCV_USERD_NOFR_SCRN_X+(MCV_USERD_NOFR_SCRN_SIZX*i), MCV_USERD_NOFR_SCRN_Y, 
				MCV_USERD_NOFR_SCRN_SIZX, MCV_USERD_NOFR_SCRN_SIZY, 
				wk->view.p_useretcscrn->rawData,
				0, 0, 
				wk->view.p_useretcscrn->screenWidth/8, wk->view.p_useretcscrn->screenHeight/8 );
	}

	// パレット変更
	GF_BGL_ScrPalChange( wk->bgl, GF_BGL_FRAME2_S, 
			MCV_USERD_NOFR_SCRN_X, MCV_USERD_NOFR_SCRN_Y,
			(MCV_USERD_NOFR_SCRN_SIZX*roop), MCV_USERD_NOFR_SCRN_SIZY, 
			MCV_PAL_BTTN+MCV_PAL_BTTN_NONE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ファクトリーデータ表示
 *
 *	@param	wk			ワーク
 *	@param	strid		文字ID
 *	@param	factoryid	ファクトリーデータID
 *	@param	friendno	友達番号
 *	@param	x			表示X
 *	@param	y			表示Y
 */
//-----------------------------------------------------------------------------
static void MCVSys_UserDispFrontiorNumDraw( WIFIP2PMATCH_WORK *wk, u32 strid, u32 factoryid, u32 friendno, u32 x, u32 y )
{
	int num;
	FRONTIER_SAVEWORK* p_fsave;

	p_fsave = SaveData_GetFrontier( wk->pSaveData );

	num = FrontierRecord_Get(p_fsave, factoryid, friendno);
	
	MCVSys_UserDispNumDraw( wk, strid, num, x, y );
}

//----------------------------------------------------------------------------
/**
 *	@brief	フロンティア　タイトルメッセージ取得
 *
 *	@param	wk			ワーク
 *	@param	p_str		文字列格納先
 *	@param	factoryid	ファクトリー　セーブデータID
 *	@param	friendno	友達ナンバー
 *
 *	factoryid
 *		MCV_FRONTIOR_TOWOR,
 *		MCV_FRONTIOR_FACTORY,
 *		MCV_FRONTIOR_FACTORY100,
 *		MCV_FRONTIOR_CASTLE,
 *		MCV_FRONTIOR_STAGE,
 *		MCV_FRONTIOR_ROULETTE,
 *		
 */
//-----------------------------------------------------------------------------
static void MCVSys_UserDispFrontiorTitleStrGet( WIFIP2PMATCH_WORK *wk, STRBUF* p_str, u32 factoryid, u32 friendno )
{
	FRONTIER_SAVEWORK* p_fsave;
	BOOL clear_flag;
	u32 stridx;
	static const u32 sc_MCVSYS_FRONTIOR_CLEAR_BIT[ MCV_FRONTIOR_NUM ] = {
		FRID_TOWER_MULTI_WIFI_CLEAR_BIT,
		FRID_FACTORY_MULTI_WIFI_CLEAR_BIT,
		FRID_FACTORY_MULTI_WIFI_CLEAR100_BIT,
		FRID_CASTLE_MULTI_WIFI_CLEAR_BIT,
		FRID_STAGE_MULTI_WIFI_CLEAR_BIT,
		FRID_ROULETTE_MULTI_WIFI_CLEAR_BIT,
	};

	GF_ASSERT( factoryid < MCV_FRONTIOR_NUM );

	p_fsave		= SaveData_GetFrontier( wk->pSaveData );

	clear_flag = FrontierRecord_Get(p_fsave, sc_MCVSYS_FRONTIOR_CLEAR_BIT[factoryid], friendno);
	if( clear_flag == FALSE ){
		stridx = msg_wifilobby_bf06;
	}else{
		stridx = msg_wifilobby_bf18;
	}
	MSGMAN_GetString(  wk->MsgManager, stridx, p_str );
}

//----------------------------------------------------------------------------
/**
 *	@brief	数字を表示する処理
 *
 *	@param	wk		ワーク
 *	@param	strid	メッセージID
 *	@param	num		数字
 *	@param	x		ｘドット座標
 *	@param	y		ｙドット座標
 */
//-----------------------------------------------------------------------------
static void MCVSys_UserDispNumDraw( WIFIP2PMATCH_WORK *wk, u32 strid, u32 num, u32 x, u32 y )
{
	WORDSET_RegisterNumber(wk->view.p_wordset, 0, num, 4, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
	MSGMAN_GetString(  wk->MsgManager, strid, wk->pExpStrBuf );
	WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );

    {
        // MatchComment: new change in plat US
        u32 width = x - (FontProc_GetPrintStrWidth(FONT_SYSTEM, wk->TitleString, 0) + 1) / 2;
        GF_STR_PrintColor( &wk->view.userWin, FONT_SYSTEM, wk->TitleString, width, y, MSG_NO_PUT, _COL_N_BLACK, NULL);
    }
}

//----------------------------------------------------------------------------
/**
 *	@brief	ボタン描画
 *
 *	@param	wk		システムワーク
 *	@param	cx		ｘキャラ座標
 *	@param	cy		ｙキャラ座標
 *	@param	type	ボタンタイプ
 *	@param	frame	フレーム
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnWrite( WIFIP2PMATCH_WORK *wk, u8 cx, u8 cy, u8 type, u8 frame )
{
	u16 reed_x, reed_y;
	u16* p_buff;

	GF_ASSERT( frame < 4 );

	// frame 3のときは1を表示する
	if( frame == 3 ){
		frame = 1;
	}

	if( (frame < 2) ){
		reed_x = MCV_BUTTON_SIZX * frame;
		reed_y = MCV_BUTTON_SIZY * type;
		
		GF_BGL_ScrWriteExpand( wk->bgl, GF_BGL_FRAME2_S,
				cx, cy, MCV_BUTTON_SIZX, MCV_BUTTON_SIZY, 
				wk->view.p_bttnscrn->rawData,
				reed_x, reed_y, 
				32, 18 );
	}else{
		reed_x = 0;
		reed_y = MCV_BUTTON_SIZY * type;
		p_buff = (u16*)wk->view.p_bttnscrn->rawData;
		
		GF_BGL_ScrWriteExpand( wk->bgl, GF_BGL_FRAME2_S,
				cx, cy, MCV_BUTTON_SIZX, MCV_BUTTON_SIZY, 
				&p_buff[ 32*18 ],
				reed_x, reed_y, 
				16, 18 );
	}

	// パレットカラーを合わせる
	GF_BGL_ScrPalChange( wk->bgl, GF_BGL_FRAME2_S, cx, cy,
			MCV_BUTTON_SIZX, MCV_BUTTON_SIZY, type+MCV_PAL_BTTN );
}

//----------------------------------------------------------------------------
/**
 *	@brief	状態に対応したメッセージデータ表示
 *
 *	@param	status	状態
 *
 *	@return	メッセージデータ
 */
//-----------------------------------------------------------------------------
static u32 MCVSys_StatusMsgIdGet( u32 status, int* col )
{
	u32 msg_id;
	*col = _COL_N_BLACK;
    switch(status){
      case WIFI_STATUS_VCT:      // VCT中
		msg_id = msg_wifilobby_027;
		*col = _COL_N_GRAY;
        break;
      case WIFI_STATUS_SBATTLE50:      // 対戦中
      case WIFI_STATUS_SBATTLE100:      // 対戦中
      case WIFI_STATUS_SBATTLE_FREE:      // 対戦中
      case WIFI_STATUS_DBATTLE50:      // 対戦中
      case WIFI_STATUS_DBATTLE100:      // 対戦中
      case WIFI_STATUS_DBATTLE_FREE:      // 対戦中
#ifdef WFP2P_DEBUG_EXON 
      case WIFI_STATUS_MBATTLE_FREE:      // 対戦中
#endif
		msg_id = msg_wifilobby_024;
		*col = _COL_N_GRAY;
        break;
      case WIFI_STATUS_SBATTLE50_WAIT:   // 対戦募集中
		msg_id = msg_wifilobby_060;
        break;
      case WIFI_STATUS_SBATTLE100_WAIT:   // 対戦募集中
		msg_id = msg_wifilobby_061;
        break;
      case WIFI_STATUS_SBATTLE_FREE_WAIT:   // 対戦募集中
		msg_id = msg_wifilobby_059;
        break;
      case WIFI_STATUS_DBATTLE50_WAIT:   // 対戦募集中
		msg_id = msg_wifilobby_063;
        break;
      case WIFI_STATUS_DBATTLE100_WAIT:   // 対戦募集中
		msg_id = msg_wifilobby_064;
        break;
      case WIFI_STATUS_DBATTLE_FREE_WAIT:   // 対戦募集中
		msg_id = msg_wifilobby_062;
        break;
#ifdef WFP2P_DEBUG_EXON 
      case WIFI_STATUS_MBATTLE_FREE_WAIT:   // 対戦募集中
		msg_id = msg_wifilobby_062;
        break;
#endif
      case WIFI_STATUS_TRADE:          // 交換中
		msg_id = msg_wifilobby_026;
		*col = _COL_N_GRAY;
        break;
      case WIFI_STATUS_TRADE_WAIT:    // 交換募集中
		msg_id = msg_wifilobby_025;
        break;
      case WIFI_STATUS_POFIN:          // ポフィン中
		msg_id = msg_wifilobby_101;
		*col = _COL_N_GRAY;
        break;
      case WIFI_STATUS_POFIN_WAIT:    // ポフィン募集中
		msg_id = msg_wifilobby_100;
        break;
      case WIFI_STATUS_FRONTIER:          // フロンティア中
		msg_id = msg_wifilobby_139;
		*col = _COL_N_GRAY;
        break;
      case WIFI_STATUS_FRONTIER_WAIT:    // フロンティア募集中
		msg_id = msg_wifilobby_138;
        break;
#ifdef WFP2P_DEBUG_EXON 
      case WIFI_STATUS_BATTLEROOM:          // バトルルーム中
		msg_id = msg_wifilobby_debug_00;
		*col = _COL_N_GRAY;
        break;
      case WIFI_STATUS_BATTLEROOM_WAIT:    // バトルルーム募集中
		msg_id = msg_wifilobby_debug_00;
        break;
#endif
      case WIFI_STATUS_BUCKET:          // バケット中
      case WIFI_STATUS_BALANCEBALL:          // バケット中
      case WIFI_STATUS_BALLOON:          // バケット中
		msg_id = msg_wifilobby_148;
		*col = _COL_N_GRAY;
        break;
      case WIFI_STATUS_BUCKET_WAIT:    // バケット募集中
      case WIFI_STATUS_BALANCEBALL_WAIT:    // バケット募集中
      case WIFI_STATUS_BALLOON_WAIT:    // バケット募集中
		msg_id = msg_wifilobby_147;
        break;
      case WIFI_STATUS_LOGIN_WAIT:    // 待機中　ログイン直後はこれ
		msg_id = msg_wifilobby_046;
        break;
      default:
		msg_id = msg_wifilobby_056;
		break;
    }
	return msg_id;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ボタン上のトレーナー名の表示
 *
 *	@param	wk			システムワーク
 *	@param	p_bmp		ビットマップ
 *	@param	friendNo	友達ナンバー
 *	@param	frame		ボタンフレーム
 *	@param	area_id		配置ID
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnWinDraw( WIFIP2PMATCH_WORK *wk, GF_BGL_BMPWIN* p_bmp, u32 friendNo, u32 frame, u32 area_id )
{
	int sex;
	int col;
	int y;

	// 画面クリーン
	GF_BGL_BmpWinDataFill( p_bmp, 0 );

	//y座標を取得
	y = ViewButtonFrame_y[ frame ];

	sex = WifiList_GetFriendInfo( wk->pList, friendNo-1, WIFILIST_FRIEND_SEX );
	// トレーナー名
	if( sex == PM_MALE ){
		col = _COL_N_BLUE;
	}else{
		col = _COL_N_RED;
	}
    MCVSys_FriendNameSet(wk, friendNo-1);
    MSGMAN_GetString(  wk->MsgManager, msg_wifilobby_033, wk->pExpStrBuf );
    WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );
    GF_STR_PrintColor( p_bmp, FONT_TALK, wk->TitleString, 
			0, y, MSG_NO_PUT, col, NULL);

	GF_BGL_BmpWinOnVReq( p_bmp );
}

//----------------------------------------------------------------------------
/**
 *	@brief	状態ウィンドウの描画
 *
 *	@param	wk			しすてむワーク
 *	@param	p_stbmp		状態ビットマップ
 *	@param	friendNo	友達番号
 *	@param	frame		ボタンフレーム
 *	@param	area_id		配置ID
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnStatusWinDraw( WIFIP2PMATCH_WORK *wk, GF_BGL_BMPWIN* p_stbmp, u32 friendNo, u32 frame, u32 area_id )
{
	int y;
	int i;
	int vct_icon;
	_WIFI_MACH_STATUS* p_status;
	u32 status;

	p_status = WifiFriendMatchStatusGet( wk, friendNo - 1 );
	status = _WifiMyStatusGet( wk, p_status );

	// アイコン表示
	y = ViewButtonFrame_y[ frame ];

	for( i=0; i<WF_VIEW_STATUS_NUM; i++ ){
		// 画面クリーン
		GF_BGL_BmpWinDataFill( &p_stbmp[i], 0 );
		if( i==0 ){
			WifiP2PMatchFriendListBmpStIconWrite( &p_stbmp[i], &wk->icon, 
					0, y,
					status );
		}else{

			if( p_status->vchat ){
				vct_icon = PLAYER_DISP_ICON_IDX_NONE;
			}else{
				vct_icon = PLAYER_DISP_ICON_IDX_VCTNOT;
			}
			WifiP2PMatchFriendListBmpIconWrite( &p_stbmp[i], &wk->icon, 
					0, y,
					vct_icon, 0 );
		}
		GF_BGL_BmpWinOnVReq( &p_stbmp[i] );
	}

}

//----------------------------------------------------------------------------
/**
 *	@brief	ボタンアニメメイン
 *
 *	@param	wk		システムワーク
 */
//-----------------------------------------------------------------------------
static void MCVSys_BttnAnmMan( WIFIP2PMATCH_WORK *wk )
{
	static const u8 BttnAnmFrame[ MCV_BUTTON_FRAME_NUM ] = {
		0, 1, 2, 1
	};

	// 動作しているかチェック
	if( wk->view.touch_friendNo == 0 ){
		return ;
	}

	// 動作
	wk->view.button_count ++;
	if( wk->view.button_count >= BttnAnmFrame[ wk->view.touch_frame ] ){
		wk->view.button_count = 0;
		wk->view.touch_frame ++;

		wk->view.button_on = TRUE;

		// 終了チェック
		if( wk->view.touch_frame >= MCV_BUTTON_FRAME_NUM ){
			wk->view.button_count = 0;
			wk->view.touch_frame = 0;
			wk->view.user_disp = MCV_USERDISP_INIT;
		}
	}
	
}

//----------------------------------------------------------------------------
/**
 *	@brief	サブ画面　OAMボタン　初期化
 *
 *	@param	wk			ワーク
 *	@param	p_handle	ハンドル
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void MCVSys_OamBttnInit( WIFIP2PMATCH_WORK *wk, ARCHANDLE* p_handle, u32 heapID )
{
	BOOL result;
	int i;
	CLACT_HEADER head;
	GF_BGL_BMPWIN bttn_font;
	STRBUF* p_str;
	FONTOAM_OAM_DATA_PTR p_fontoam_data;
	int fontoam_cgsize;
	FONTOAM_INIT fontoam_add;
    u32 width; // MatchComment: new variable
	CLACT_ADD add[ MCV_USERD_BTTN_NUM ] = {
		{	// LEFT
			NULL, NULL,
			{ FX32_CONST(MCV_USERD_BTTN_LEFT_X), FX32_CONST(MCV_USERD_BTTN_Y)+NAMEIN_SUB_ACTOR_DISTANCE, 0 },
			{ FX32_ONE, FX32_ONE, FX32_ONE },
			0, MCV_USERD_BTTN_PRI, NNS_G2D_VRAM_TYPE_2DSUB,
			0
		},
		{	// BACK
			NULL, NULL,
			{ FX32_CONST(MCV_USERD_BTTN_BACK_X), FX32_CONST(MCV_USERD_BTTN_Y)+NAMEIN_SUB_ACTOR_DISTANCE, 0 },
			{ FX32_ONE, FX32_ONE, FX32_ONE },
			0, MCV_USERD_BTTN_PRI, NNS_G2D_VRAM_TYPE_2DSUB,
			0
		},
		{	// RIGHT
			NULL, NULL,
			{ FX32_CONST(MCV_USERD_BTTN_RIGHT_X), FX32_CONST(MCV_USERD_BTTN_Y)+NAMEIN_SUB_ACTOR_DISTANCE, 0 },
			{ FX32_ONE, FX32_ONE, FX32_ONE },
			0, MCV_USERD_BTTN_PRI, NNS_G2D_VRAM_TYPE_2DSUB,
			0
		},
	};

	
	// 一応表示をOFF
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );
//	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	
	// リソース読込み
	wk->view.button_res[0] = CLACT_U_ResManagerResAddArcChar_ArcHandle(
			wk->resMan[0], p_handle, NARC_wifip2pmatch_wf_match_btm_oam_NCGR,
			FALSE, MCV_USERD_BTTN_RESCONTID,
			NNS_G2D_VRAM_TYPE_2DSUB, heapID );
	wk->view.button_res[1] = CLACT_U_ResManagerResAddArcPltt_ArcHandle(
			wk->resMan[1], p_handle, NARC_wifip2pmatch_wf_match_btm_oam_NCLR,
			FALSE, MCV_USERD_BTTN_RESCONTID,
			NNS_G2D_VRAM_TYPE_2DSUB, 8, heapID );
	wk->view.button_res[2] = CLACT_U_ResManagerResAddArcKindCell_ArcHandle(
			wk->resMan[2], p_handle, NARC_wifip2pmatch_wf_match_btm_oam_NCER,
			FALSE, MCV_USERD_BTTN_RESCONTID, CLACT_U_CELL_RES,
			heapID );
	wk->view.button_res[3] = CLACT_U_ResManagerResAddArcKindCell_ArcHandle(
			wk->resMan[3], p_handle, NARC_wifip2pmatch_wf_match_btm_oam_NANR,
			FALSE, MCV_USERD_BTTN_RESCONTID, CLACT_U_CELLANM_RES,
			heapID );

	// VRAM展開
	result = CLACT_U_CharManagerSetCharModeAdjustAreaCont( wk->view.button_res[0] );
	GF_ASSERT( result );
	result = CLACT_U_PlttManagerSetCleanArea( wk->view.button_res[1] );
	GF_ASSERT( result );
	CLACT_U_ResManagerResOnlyDelete( wk->view.button_res[0] );
	CLACT_U_ResManagerResOnlyDelete( wk->view.button_res[1] );

	// ヘッダー作成
	CLACT_U_MakeHeader( &head, 
			MCV_USERD_BTTN_RESCONTID, MCV_USERD_BTTN_RESCONTID,
			MCV_USERD_BTTN_RESCONTID, MCV_USERD_BTTN_RESCONTID,
			CLACT_U_HEADER_DATA_NONE, CLACT_U_HEADER_DATA_NONE,
			0, MCV_USERD_BTTN_BGPRI,
			wk->resMan[0], wk->resMan[1], wk->resMan[2], wk->resMan[3],
			NULL, NULL );
		

	// アクター作成
	for( i=0; i<MCV_USERD_BTTN_NUM; i++ ){
		add[i].ClActSet = wk->clactSet;
		add[i].ClActHeader = &head;
		add[i].heap = heapID;
		wk->view.button_act[i] = CLACT_Add( &add[i] );
		CLACT_AnmChg( wk->view.button_act[i], c_MCV_USER_BTTN_ANM_RELEASE[i] );
	}

	// ボタンフォント読込み
	FontProc_LoadFont( FONT_BUTTON, heapID );

	// もどる　描画用領域作成
	GF_BGL_BmpWinInit( &bttn_font );
	GF_BGL_BmpWinObjAdd( wk->bgl, &bttn_font, 
			MCV_USERD_BTTN_FONT_SIZX, MCV_USERD_BTTN_FONT_SIZY,
			MCV_USERD_BTTN_FONT_CGXOFS, MCV_USERD_BTTN_FONT_COL );

	// 文字列取得
	p_str = STRBUF_Create( 64, heapID );
	MSGMAN_GetString( wk->MsgManager, msg_wifilobby_143, p_str );

    {
        // 文字列を書き込む
        width = -((FontProc_GetPrintStrWidth(FONT_BUTTON, wk->TitleString, 0) + 1) / 2);
        GF_STR_PrintColor( &bttn_font, FONT_BUTTON, p_str, 0, 0, MSG_NO_PUT, GF_PRINTCOLOR_MAKE(1,2,0), NULL );
    }

	STRBUF_Delete( p_str );

	// 文字列OAMデータ作成
	p_fontoam_data = FONTOAM_OAMDATA_Make( &bttn_font, heapID );

	// サイズ取得
	fontoam_cgsize = FONTOAM_OAMDATA_NeedCharSize( p_fontoam_data, NNS_G2D_VRAM_TYPE_2DSUB );

	// CharAreaAlloc
	result = CharVramAreaAlloc( fontoam_cgsize, CHARM_CONT_AREACONT,
			NNS_G2D_VRAM_TYPE_2DSUB, &wk->view.back_fontoam_cg );
	GF_ASSERT(result);

	// fontoam作成
	fontoam_add.fontoam_sys = wk->fontoam;
	fontoam_add.bmp = &bttn_font;
	fontoam_add.clact_set = wk->clactSet;
	fontoam_add.pltt = CLACT_U_PlttManagerGetProxy( wk->view.button_res[1], NULL );
	fontoam_add.parent = wk->view.button_act[MCV_USERD_BTTN_BACK];
	fontoam_add.char_ofs = wk->view.back_fontoam_cg.alloc_ofs;
	fontoam_add.x = width; // MCV_USERD_BTTN_FONT_X -> width
	fontoam_add.y = MCV_USERD_BTTN_FONT_Y;
	fontoam_add.bg_pri = MCV_USERD_BTTN_BGPRI;
	fontoam_add.soft_pri = MCV_USERD_BTTN_FONT_OAMPRI;
	fontoam_add.draw_area = NNS_G2D_VRAM_TYPE_2DSUB;
	fontoam_add.heap = heapID;
	wk->view.back_fontoam = FONTOAM_OAMDATA_Init( &fontoam_add, p_fontoam_data );

	// OAM構成データ破棄
	FONTOAM_OAMDATA_Free( p_fontoam_data );

	// BMP破棄
	GF_BGL_BmpWinDel( &bttn_font );
	
	// 文字列フォント破棄
	FontProc_UnloadFont( FONT_BUTTON );

	// ボタンあたり判定データ作成
	wk->view.p_oambttnman = BMN_Create( c_MCV_USER_BTTN_OAM_Hit, MCV_USERD_BTTN_NUM, 
			MCVSys_OamBttnCallBack, wk, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ボタンデータの破棄
 *
 *	@param	wk	ワーク
 */
//-----------------------------------------------------------------------------
static void MCVSys_OamBttnDelete( WIFIP2PMATCH_WORK *wk )
{
	int i;

	// ボタンマネージャ破棄
	BMN_Delete( wk->view.p_oambttnman );

	// フォントOAM破棄
	FONTOAM_OAMDATA_Delete( wk->view.back_fontoam );

	// キャラ領域破棄
	CharVramAreaFree( &wk->view.back_fontoam_cg );
	
	// ワークの破棄
	for( i=0; i<MCV_USERD_BTTN_NUM; i++ ){
		CLACT_Delete( wk->view.button_act[i] );
	}

	// リソース破棄
	CLACT_U_CharManagerDelete( wk->view.button_res[0] );
	CLACT_U_PlttManagerDelete( wk->view.button_res[1] );
	for( i=0; i<CLACT_RESOURCE_NUM; i++ ){
		CLACT_U_ResManagerResDelete( wk->resMan[i], wk->view.button_res[i] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	OAMボタンON
 *
 *	@param	wk	ワーク
 */
//-----------------------------------------------------------------------------
static void MCVSys_OamBttnOn( WIFIP2PMATCH_WORK *wk )
{
	int i;
	
	// 表示状態にする
	wk->view.buttonact_on = MCV_USERD_BTTN_MODE_NML;

	// 表示ON
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

	// パラメータ初期化
	wk->view.touch_button = 0;

	// アニメ初期化
	for( i=0; i<MCV_USERD_BTTN_NUM; i++ ){
		MCVSys_OamBttnObjAnmStart( &wk->view, i );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ボタン表示　BACK非表示モード	
 *
 *	@param	wk	ワーク
 */
//-----------------------------------------------------------------------------
static void MCVSys_OamBttnOnNoBack( WIFIP2PMATCH_WORK *wk )
{
	MCVSys_OamBttnOn( wk );

	// もどるだけ非表示
	CLACT_SetDrawFlag( wk->view.button_act[MCV_USERD_BTTN_BACK], FALSE );
	FONTOAM_SetDrawFlag( wk->view.back_fontoam, FALSE );

	// 表示状態にする
	wk->view.buttonact_on = MCV_USERD_BTTN_MODE_NOBACK;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ボタンOFF
 *
 *	@param	wk 
 */
//-----------------------------------------------------------------------------
static void MCVSys_OamBttnOff( WIFIP2PMATCH_WORK *wk )
{
	// 戻るが表示OFFになってるかもしれないのでONにしておく
	if( wk->view.buttonact_on == MCV_USERD_BTTN_MODE_NOBACK ){
		CLACT_SetDrawFlag( wk->view.button_act[MCV_USERD_BTTN_BACK], TRUE );
		FONTOAM_SetDrawFlag( wk->view.back_fontoam, TRUE );
	}
	
	// 非表示状態にする
	wk->view.buttonact_on = MCV_USERD_BTTN_MODE_WAIT;

	// 表示をOFF
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );

}

//----------------------------------------------------------------------------
/**
 *	@brief	ボタン非表示処理
 *
 *	@param	wk 
 *
 *	@retval	MCV_USERD_BTTN_RET_NONE,	// 何の反応もなし
 *	@retval	MCV_USERD_BTTN_RET_LEFT,	// 左がおされた
 *	@retval	MCV_USERD_BTTN_RET_BACK,	// もどるがおされた
 *	@retval	MCV_USERD_BTTN_RET_RIGHT,	// 右がおされた
 */
//-----------------------------------------------------------------------------
static u32 MCVSys_OamBttnMain( WIFIP2PMATCH_WORK *wk )
{
	int i;
	BOOL result;
	
	// 表示ONじょうたいかチェック
	if( wk->view.buttonact_on == MCV_USERD_BTTN_MODE_WAIT ){
		return MCV_USERD_BTTN_RET_NONE;
	}


	// ボタンイベントデータ消去
	wk->view.touch_button = MCV_USERD_BTTN_NUM;	// 何にも反応しないようにありえないボタン番号
	wk->view.touch_button_event = 0xff;	// 何にも反応しないようにありえない数字をいれておく

	// ボタンマネージャ実行
	BMN_Main( wk->view.p_oambttnman );

	// ボタンアニメコントロール
	for( i=0; i<MCV_USERD_BTTN_NUM; i++ ){

		// バック非表示モードならバックの処理を行わない
		if( (wk->view.buttonact_on == MCV_USERD_BTTN_MODE_NOBACK) &&
			(i == MCV_USERD_BTTN_BACK) ){
			continue;
		}
		
		result = MCVSys_OamBttnObjAnmMain( &wk->view, i, wk->view.touch_button, wk->view.touch_button_event );
		if( result == TRUE ){
			// ボタンを押した
			return MCV_USERD_BTTN_RET_LEFT + i;
		}
	}

	return MCV_USERD_BTTN_RET_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ボタンコールバック
 *
 *	@param	bttn_no		ボタンNo
 *	@param	event		イベントNo
 *	@param	p_work		ワーク
 */
//-----------------------------------------------------------------------------
static void MCVSys_OamBttnCallBack( u32 bttn_no, u32 event, void* p_work )
{
	WIFIP2PMATCH_WORK* p_wk = p_work;
	p_wk->view.touch_button = bttn_no;
	p_wk->view.touch_button_event = event;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ボタンアニメ開始
 *
 *	@param	p_view		ワーク
 *	@param	bttn_no		ボタンナンバー
 */
//-----------------------------------------------------------------------------
static void MCVSys_OamBttnObjAnmStart( WIFIP2PMATCH_VIEW*  p_view, u32 bttn_no )
{
	// アニメ初期化	RELEASEアニメ最終フレームに設定
	CLACT_AnmChg( p_view->button_act[ bttn_no ], c_MCV_USER_BTTN_ANM_RELEASE[bttn_no] );
	CLACT_AnmFrameSet( p_view->button_act[ bttn_no ], MCV_USERD_BTTN_ANMMAX_0ORG );

	// 文字列の座標初期化
	if( bttn_no == MCV_USERD_BTTN_BACK ){
		FONTOAM_SetMat( p_view->back_fontoam, MCV_USERD_BTTN_FONT_X, c_MCV_USER_BTTN_FONT_YANM[0] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメメイン
 *
 *	@param	p_view		ワーク
 *	@param	bttn_no		ボタンナンバー
 *	@param	push_bttn	押されているボタン
 *	@param	event		イベント
 *
 *	@retval	TRUE	押し終わった
 *	@retval	FALSE	押されていない又は押し終わってない
 */
//-----------------------------------------------------------------------------
static BOOL MCVSys_OamBttnObjAnmMain( WIFIP2PMATCH_VIEW*  p_view, u32 bttn_no, u32 push_bttn, u32 event )
{
	BOOL ret = FALSE;
	u32 anm_frame;
	u32 anm_seq;
	
	// 長い時間押しているとき＝MCV_USERD_BTTN_ANMPUSHOKまでいったらTRUEを返す
	// 短い時間押しているとき＝離されたときにTRUEを返す
	
	
	// 自分がおされているかチェック
	if( (push_bttn == bttn_no)  ){

		anm_frame = CLACT_AnmFrameGet( p_view->button_act[bttn_no] );

		// 押されているとき
		if( (event == BMN_EVENT_TOUCH)  || (event == BMN_EVENT_HOLD) ){
	 
			// イベントが最初のタッチだったらアニメ設定
			if( event == BMN_EVENT_TOUCH ){
				CLACT_AnmChg( p_view->button_act[ bttn_no ], c_MCV_USER_BTTN_ANM_PUSH[bttn_no] );
			}
			
			// アニメがPUSHOK以下の時のみ動かす
			if( anm_frame < MCV_USERD_BTTN_ANMPUSHOK ){
				
				CLACT_AnmFrameChg( p_view->button_act[bttn_no], FX32_CONST(2) );
				anm_frame = CLACT_AnmFrameGet( p_view->button_act[bttn_no] );

				// 「もどる」ならフォントも動かす
				if( bttn_no == MCV_USERD_BTTN_BACK ){
					FONTOAM_SetMat( p_view->back_fontoam, MCV_USERD_BTTN_FONT_X, 
							c_MCV_USER_BTTN_FONT_YANM[anm_frame] );
				}

				// ボタンが下までアニメしたら押したことにする
				// この次のフレームからは上のanm_frameチェックで
				// ひっかかるのでここは初めてOKにきたときにのみ
				// 実行される
				if( anm_frame >= MCV_USERD_BTTN_ANMPUSHOK ){
					ret = TRUE;
				}
			}
			
		}
		// 離したとき
		else if( event == BMN_EVENT_RELEASE ){
			if( anm_frame < MCV_USERD_BTTN_ANMPUSHOK ){
				// ボタンを押したことになるフレームまでいっていなければ
				// ボタンを押したことにする
				ret = TRUE;
			}
		}
	}else{
		
		anm_seq = CLACT_AnmGet( p_view->button_act[bttn_no] );
		anm_frame = CLACT_AnmFrameGet( p_view->button_act[bttn_no] );
	
		// アニメがPUSHアニメだったらRELEASEアニメに切り替える
		if( anm_seq == c_MCV_USER_BTTN_ANM_PUSH[bttn_no] ){
			CLACT_AnmChg( p_view->button_act[bttn_no], c_MCV_USER_BTTN_ANM_RELEASE[bttn_no] );
			CLACT_AnmFrameSet( p_view->button_act[bttn_no], MCV_USERD_BTTN_ANMMAX_0ORG - anm_frame );
		}

		// アニメが終わるまでアニメさせる
		CLACT_AnmFrameChg( p_view->button_act[bttn_no], FX32_CONST(2) );
	}

	return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	最初に人がいるページを返す
 *
 *	@param	cp_wk	ワーク
 *
 *	@return	ページ	（誰もいないなら０ページ目を返す）
 */
//-----------------------------------------------------------------------------
static s32 MCVSys_FirstFriendPageGet( const WIFIP2PMATCH_WORK * cp_wk )
{
	int i;
	s32 friend_no;

	friend_no = 0xffff;	// ありえない最大値で初期化

	for( i=0; i<WIFIP2PMATCH_MEMBER_MAX; i++ ){
		if( (cp_wk->index2NoBackUp[i] != 0) && (friend_no > cp_wk->index2NoBackUp[i]) ){
			friend_no = cp_wk->index2NoBackUp[i];
		}
	}

	if( friend_no == 0xffff ){
		return 0;
	}

	return ( (friend_no - 1) / WCR_MAPDATA_1BLOCKOBJNUM );
}

//----------------------------------------------------------------------------
/**
 *	@brief	Vierのほうのワードセットに人の名前を設定
 *	
 *	@param	p_wk			ワーク
 *	@param	friendno		友達ナンバー
 */
//-----------------------------------------------------------------------------
static void MCVSys_FriendNameSet( WIFIP2PMATCH_WORK* p_wk, int friendno )
{
    if(friendno != -1){
        MYSTATUS* pTarget = MyStatus_AllocWork(HEAPID_WIFIP2PMATCH);
        MyStatus_SetMyName(pTarget, WifiList_GetFriendNamePtr(p_wk->pList,friendno));
        WORDSET_RegisterPlayerName( p_wk->view.p_wordset, 0, pTarget);
        sys_FreeMemoryEz(pTarget);
    }
}

//----------------------------------------------------------------------------
/**
 *	@brief	友達マッチデータのステータス取得
 *
 *	@param	wk			システムワーク
 *	@param	idx			インデックス
 */
//-----------------------------------------------------------------------------
static _WIFI_MACH_STATUS* WifiFriendMatchStatusGet( WIFIP2PMATCH_WORK* wk, u32 idx )
{
	GF_ASSERT( idx < WIFIP2PMATCH_MEMBER_MAX );

#ifdef WFP2PM_MANY_OBJ
	idx = 0;
#endif
	return &wk->pMatch->friendMatchStatus[ idx ];
}

//----------------------------------------------------------------------------
/**
 *	@brief	友達の状態を取得する
 *
 *	@param	idx		インデックス
 */
//-----------------------------------------------------------------------------
static u8 WifiDwc_getFriendStatus( int idx )
{
#ifdef WFP2PM_MANY_OBJ
	idx = 0;
#endif
	return mydwc_getFriendStatus( idx );
}

//----------------------------------------------------------------------------
/**
 *	@brief	P2P通信ゲームを開始する。
 *
 *	@param	friendno	ターゲット番号	（-1なら親）
 *	@param	status		ステータス
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static BOOL WifiP2PMatch_CommWifiBattleStart( WIFIP2PMATCH_WORK* wk, int friendno, int status )
{
	// 080707	tomoya takahashi
	// 同期通信ワークを初期化
	CommToolInitialize( HEAPID_COMMUNICATION );// COMMUNICATONにしているのは、comm_systemの中でもCOMMUNICATIONでやっているためです。
	
	// ボイスチャット設定
	mydwc_setVChat(wk->pMatch->myMatchStatus.vchat);// ボイスチャットとBGM音量の関係を整理 tomoya takahashi

	// 4人で遊ぶゲームのときは、通信の一部をHEAP_WORLDから確保する
	// その他は通常の通信バッファからとる
	if( _wait2to4Mode( status ) == TRUE ){
		mydwc_recvHeapChange( TRUE, HEAPID_WIFIP2PMATCHEX );
	}else{
		mydwc_recvHeapChange( FALSE, HEAPID_WIFIP2PMATCHEX );
	}

	// 接続する前に４人募集で送られてくる可能性のある
	// コマンドを設定する
	CommCommandWFP2PMF_MatchStartInitialize();
	
	return CommWifiBattleStart( friendno );
}

//----------------------------------------------------------------------------
/**
 *	@brief	友達募集中　設定ON
 *
 *	@param	wk	ワーク
 */
//-----------------------------------------------------------------------------
static void FriendRequestWaitOn( WIFIP2PMATCH_WORK* wk, BOOL msg_on )
{
	if( wk->friend_request_wait == FALSE ){
		// メッセージをだして、プレイヤー動作を停止する
		wk->friend_request_wait = TRUE;
		WIFI_MCR_PlayerMovePause( &wk->matchroom, TRUE );

		if( msg_on == TRUE ){
			WifiP2PMatchMessagePrint( wk, msg_wifilobby_142, FALSE );	
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	友達募集中　設定OFF
 *
 *	@param	wk	ワーク
 */
//-----------------------------------------------------------------------------
static void FriendRequestWaitOff( WIFIP2PMATCH_WORK* wk )
{
	if( wk->friend_request_wait == TRUE ){
		// メッセージを消して、プレイヤー動作を開始する
		wk->friend_request_wait = FALSE;
		EndMessageWindowOff( wk );
		WIFI_MCR_PlayerMovePause( &wk->matchroom, FALSE );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	友達募集中フラグ取得
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	募集中
 *	@retval	FALSE	ぼしゅうしていない
 */
//-----------------------------------------------------------------------------
static BOOL FriendRequestWaitFlagGet( const WIFIP2PMATCH_WORK* cp_wk )
{
	return cp_wk->friend_request_wait;
}

