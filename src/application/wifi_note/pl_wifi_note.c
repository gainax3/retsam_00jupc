//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		pl_wifi_note.c
 *	@brief		友達手帳	プラチナバージョン
 *	@author		tomoya takahshi
 *	@data		2007.07.23
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"
#include "dwc.h"

#include "gflib/strbuf.h"
#include "gflib/strbuf_family.h"

#include "system/wordset.h"
#include "system/msgdata.h"
#include "system/window.h"
#include "system/fontproc.h"
#include "system/wipe.h"
#include "system/bmp_menu.h"
#include "system/bmp_list.h"
#include "system/pm_overlay.h"
#include "system/gra_tool.h"
#include "system/msgdata_util.h"

#if PL_T0861_080712_FIX
#include "system/pmfprint.h"
#endif

#include "savedata/config.h"
#include "savedata/wifilist.h"
#include "savedata/frontier_savedata.h"
#include "savedata/factory_savedata.h"
#include "savedata/stage_savedata.h"
#include "savedata/castle_savedata.h"
#include "savedata/roulette_savedata.h"
#include "savedata/b_tower.h"

#include "wifi/dwc_rapfriend.h"

#include "msgdata/msg.naix"
#include "msgdata/msg_wifi_note.h"

#include "application/pl_wifi_note.h"
#include "application/codein/codein.h"
#include "application/namein.h"
#include "application/wifi_2dmap/wifi_2dchar.h"
#include "application/wifi_lobby/wflby_def.h"

#include "itemtool/itemsym.h"
#include "itemtool/myitem.h"

#include "frontier/factory_def.h"

#include "field/union_beacon_tool.h"
#include "field/sysflag.h"

#include "pl_wifinote.naix"
#include "wifi_note_snd.h"
#include "savedata/frontier_savedata.h"

// MatchComment: new change in plat US
#include "system/pmfprint.h"

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
//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------


// WIFI2DMAPシステムオーバーレイ
FS_EXTERN_OVERLAY(wifi_2dmapsys);

//-------------------------------------
///	文字列バッファサイズ
//=====================================
#define WFNOTE_STRBUF_SIZE	(128)

//-------------------------------------
///	CLACT
//=====================================
#define CLACT_RESOURCE_NUM		( 4 )	// リソースマネージャ数
#define CLACT_WKNUM				( 64 )	// ワーク数
#define CLACT_RESNUM			( 64 )	// 読み込むリソース数
#define CLACT_MAIN2_REND_Y_S32	( 256 )	// スクロールさせないサーフェース面のY座標位置
#define CLACT_MAIN2_REND_Y		( FX32_CONST(256) )	// スクロールさせないサーフェース面のY座標位置
#define CLACT_MAIN_VRAM_SIZ		( 0x14000 )
#define CLACT_SUB_VRAM_SIZ		( 0 )

//-------------------------------------
///	VRAMTRANFER_MAN
//=====================================
#define VRAMTR_MAN_NUM		(16)

//-------------------------------------
///	BG
//=====================================
enum{	// BG識別定数
	WFNOTE_BG_MAIN_BACK,
	WFNOTE_BG_MAIN_MSG,
	WFNOTE_BG_MAIN_SCROLL,
	WFNOTE_BG_MAIN_SCRMSG,
	WFNOTE_BG_SUB_BACK,
	WFNOTE_BG_NUM,
};
enum{	// パレットテーブル
	// メイン
	WFNOTE_BGPL_MAIN_BACK_0 = 0,
	WFNOTE_BGPL_MAIN_BACK_1,
	WFNOTE_BGPL_MAIN_BACK_2,
	WFNOTE_BGPL_MAIN_BACK_3,
	WFNOTE_BGPL_MAIN_BACK_4,
	WFNOTE_BGPL_MAIN_BACK_5,
	WFNOTE_BGPL_MAIN_BACK_6,
	WFNOTE_BGPL_MAIN_MSGFONT,
	WFNOTE_BGPL_MAIN_TALKWIN,
	WFNOTE_BGPL_MAIN_MENUWIN,
	WFNOTE_BGPL_MAIN_TRGRA,

	// サブ
	WFNOTE_BGPL_SUB_BACK_0 = 0,
	WFNOTE_BGPL_SUB_BACK_1,
	WFNOTE_BGPL_SUB_BACK_2,
	WFNOTE_BGPL_SUB_BACK_3,
	WFNOTE_BGPL_SUB_BACK_4,
};
#define WFNOTE_BGPL_MAIN_BACK_NUM	((WFNOTE_BGPL_MAIN_BACK_6+1) - WFNOTE_BGPL_MAIN_BACK_0)
#define WFNOTE_BGPL_SUB_BACK_NUM	((WFNOTE_BGPL_SUB_BACK_4+1) - WFNOTE_BGPL_SUB_BACK_0)
// 基本CGX	MAIN_MSG面に読み込まれます
#define WFNOTE_BGCGX_MAIN_MSG_MENU	(1)
#define WFNOTE_BGCGX_MAIN_MSG_TALK	(WFNOTE_BGCGX_MAIN_MSG_MENU+MENU_WIN_CGX_SIZ)
#define WFNOTE_BGCGX_MAIN_MSG_TALKEND	(WFNOTE_BGCGX_MAIN_MSG_TALK+TALK_WIN_CGX_SIZ)

// 基本タイトルビットマップ
#define WFNOTE_BG_TITLEBMP_X	( 1 )
#define WFNOTE_BG_TITLEBMP_Y	( 0 )
#define WFNOTE_BG_TITLEBMP_SIZX	( 24 )
#define WFNOTE_BG_TITLEBMP_SIZY	( 3 )
#define WFNOTE_BG_TITLEBMP_CGX	( WFNOTE_BGCGX_MAIN_MSG_TALKEND )
#define WFNOTE_BG_TITLEBMP_CGX_END	( WFNOTE_BG_TITLEBMP_CGX + (WFNOTE_BG_TITLEBMP_SIZX*WFNOTE_BG_TITLEBMP_SIZY) )
#define WFNOTE_BG_TITLEBMP_PL_Y	( 4 )

// 基本OAMのデータ
#define WFNOTE_OAM_COMM_CONTID	( 100 )	// 基本的OAMの管理ID

// FONTカラー
#define	WFNOTE_COL_BLACK	( GF_PRINTCOLOR_MAKE( 1, 2, 0 ) )		// フォントカラー：黒
#define	WFNOTE_COL_WHITE	( GF_PRINTCOLOR_MAKE( 15, 2, 0 ) )		// フォントカラー：白
#define	WFNOTE_COL_RED	    ( GF_PRINTCOLOR_MAKE( 3, 4, 0 ) )		// フォントカラー：赤
#define	WFNOTE_COL_BLUE	    ( GF_PRINTCOLOR_MAKE( 5, 6, 0 ) )		// フォントカラー：青

// 描画エリア数
enum{	
	WFNOTE_DRAWAREA_MAIN,
	WFNOTE_DRAWAREA_RIGHT,
	WFNOTE_DRAWAREA_LEFT,
	WFNOTE_DRAWAREA_NUM,
};

//  基本スクリーンデータ
enum{
	WFNOTE_SCRNDATA_BACK,	// 背景用
	WFNOTE_SCRNDATA_KIT,		// 書き込み用
	WFNOTE_SCRNDATA_NUM,		// 
};	

//-------------------------------------
///	STATUS
//=====================================
enum {
	WFNOTE_STATUS_MODE_SELECT,	// 最初の選択画面
	WFNOTE_STATUS_FRIENDLIST,	// 友達データ表示
	WFNOTE_STATUS_CODEIN,		// 友達コード入力
	WFNOTE_STATUS_MYCODE,		// 自分の友達コード確認
	WFNOTE_STATUS_FRIENDINFO,	// 友達詳細データ
	WFNOTE_STATUS_END,			// 終了
	WFNOTE_STATUS_NUM
} ;
typedef enum{
	WFNOTE_STRET_CONTINUE,	// つづく
	WFNOTE_STRET_FINISH,	// 作業は終わった
} WFNOTE_STRET;
#define WFNOTE_STATUS_CHANGE_WAIT	(4)	// 変更時ウエイト

//-------------------------------------
///	PROC SEQ
//=====================================
enum {
	WFNOTE_PROCSEQ_MAIN,
	WFNOTE_PROCSEQ_WAIT,
} ;

//-------------------------------------
///	友達データ新規設定関数　戻り値
//=====================================
enum{
	WFNOTE_DATA_NEWFRIENDSET_OK,	// 設定できた
	WFNOTE_DATA_NEWFRIENDSET_FRIENDKEYNG,	// 友達コードが不正
	WFNOTE_DATA_NEWFRIENDSET_SETING,	// もう設定してあった
	WFNOTE_DATA_NEWFRIENDSET_NUM

};

// 矢印データ
#define WFNOTE_YAZIRUSHINUM		(2)	// 矢印の数
#define WFNOTE_YAZIRUSHI_XLEF	(FX32_CONST(6))
#define WFNOTE_YAZIRUSHI_XRIGHT	(FX32_CONST(249))
#define WFNOTE_YAZIRUSHI_Y		(FX32_CONST(98))
#define WFNOTE_YAZIRUSHI_PRI		( 8 )

// タイトルメッセージスピード
#define WFNOTE_TITLE_MSG_SPEED	( 2 ) 



//-------------------------------------
///	MODESELECT
//=====================================
// BMP
#define WFNOTE_MODESELECT_MSG_X		( 3 )	// 開始位置（キャラ単位）
#define WFNOTE_MODESELECT_MSG_Y		( 4 )	// 開始位置（キャラ単位）
#define WFNOTE_MODESELECT_MSG_SIZX	( 26 )	// サイズ（キャラ単位）
#define WFNOTE_MODESELECT_MSG_SIZY	( 20 )	// サイズ（キャラ単位）
#define WFNOTE_MODESELECT_CGX		( 1 )	// キャラクタオフセット

#define WFNOTE_MODESELECT_TALK_X		( 2 )	// 開始位置（キャラ単位）
#define WFNOTE_MODESELECT_TALK_Y		( 19 )	// 開始位置（キャラ単位）
#define WFNOTE_MODESELECT_TALK_SIZX		( 27 )	// サイズ（キャラ単位）
#define WFNOTE_MODESELECT_TALK_SIZY		( 4 )	// サイズ（キャラ単位）
#define WFNOTE_MODESELECT_TALKCGX		( WFNOTE_BG_TITLEBMP_CGX_END )	// キャラクタオフセット

#define WFNOTE_MODESELECT_YESNO_FRM		( GF_BGL_FRAME1_M )
#define WFNOTE_MODESELECT_YESNO_X		( 25 )	// 開始位置（キャラ単位）
#define WFNOTE_MODESELECT_YESNO_Y		( 13 )	// 開始位置（キャラ単位）
#define WFNOTE_MODESELECT_YESNO_SIZX		( 6 )	// サイズ（キャラ単位）
#define WFNOTE_MODESELECT_YESNO_SIZY		( 4 )	// サイズ（キャラ単位）
#define WFNOTE_MODESELECT_YESNOCGX		( WFNOTE_MODESELECT_TALKCGX+(WFNOTE_MODESELECT_TALK_SIZX*WFNOTE_MODESELECT_TALK_SIZY) )	// キャラクタオフセット

// 動作シーケンス
enum{
	WFNOTE_MODESELECT_SEQ_INIT,
	WFNOTE_MODESELECT_SEQ_FADEINWAIT,
	WFNOTE_MODESELECT_SEQ_INIT_NOFADE,
	WFNOTE_MODESELECT_SEQ_MAIN,
	WFNOTE_MODESELECT_SEQ_FADEOUT,
	WFNOTE_MODESELECT_SEQ_FADEOUTWAIT,

	// コード入力から戻ってきたとき用
	WFNOTE_MODESELECT_SEQ_CODECHECK_INIT,
	WFNOTE_MODESELECT_SEQ_CODECHECK_FADEINWAIT,
	WFNOTE_MODESELECT_SEQ_CODECHECK_MSGWAIT,		//登録確認メッセージON
	WFNOTE_MODESELECT_SEQ_CODECHECK_MSGYESNOWAIT,	//はいいいえ選択待ち
	WFNOTE_MODESELECT_SEQ_CODECHECK_RESLUTMSGWAIT,	//選択結果表示
};
// カーソル
enum{
	WFNOTE_MODESELECT_CURSOR_FRIENDLIST,
	WFNOTE_MODESELECT_CURSOR_CODEIN,
	WFNOTE_MODESELECT_CURSOR_MYCODE,
	WFNOTE_MODESELECT_CURSOR_END,
	WFNOTE_MODESELECT_CURSOR_NUM,
};
#define WFNOTE_MODESELECT_CURSOR_CSIZX	( 28 )	// カーソル部分の横サイズ（キャラ）
#define WFNOTE_MODESELECT_CURSOR_CSIZY	( 4 )	// カーソル部分の縦サイズ（キャラ）
#define WFNOTE_MODESELECT_CURSOR_PAL_ON	( WFNOTE_BGPL_SUB_BACK_1 )	// 選択時
#define WFNOTE_MODESELECT_CURSOR_PAL_OFF ( WFNOTE_BGPL_SUB_BACK_0 )	// 非選択時


//-------------------------------------
///	FRIENDLIST
//=====================================
#define WFNOTE_FRIENDLIST_PAGEFRIEND_NUM	(8)	// 1ページにいる人の数
#define WFNOTE_FRIENDLIST_PAGEFRIEND_HALF	(WFNOTE_FRIENDLIST_PAGEFRIEND_NUM/2)	// 1ページにいる人の数
#define WFNOTE_FRIENDLIST_PAGE_NUM			(4)	// ページ総数
#define WFNOTE_FRIENDLIST_FRIENDMAX			( WFNOTE_FRIENDLIST_PAGEFRIEND_NUM*WFNOTE_FRIENDLIST_PAGE_NUM )	// 表示できる友達の最大値
#define WFNOTE_FRIENDLIST_2DCHAR_NUM		(16)// 2dcharワーク数
enum{	// カーソルデータすう
	WFNOTE_FRIENDLIST_CURSORDATA_0,
	WFNOTE_FRIENDLIST_CURSORDATA_1,
	WFNOTE_FRIENDLIST_CURSORDATA_2,
	WFNOTE_FRIENDLIST_CURSORDATA_3,
	WFNOTE_FRIENDLIST_CURSORDATA_4,
	WFNOTE_FRIENDLIST_CURSORDATA_5,
	WFNOTE_FRIENDLIST_CURSORDATA_6,
	WFNOTE_FRIENDLIST_CURSORDATA_7,
	WFNOTE_FRIENDLIST_CURSORDATA_BACK,
	WFNOTE_FRIENDLIST_CURSORDATA_NUM,
	WFNOTE_FRIENDLIST_CURSORDATA_OFF,	// カーソルを消すときのPOS
};

// ビットマップ
#define WFNOTE_FRIENDLIST_TEXT_OFSX	( 4 )	// 開始位置ｘ
#define WFNOTE_FRIENDLIST_TEXT_OFSY	( 4 )	// 開始位置ｙ
#define WFNOTE_FRIENDLIST_TEXT_SIZX	( 26 )	// サイズｘ
#define WFNOTE_FRIENDLIST_TEXT_SIZY	( 17 )	// サイズｙ
#define WFNOTE_FRIENDLIST_TEXT_CGX0	( 1 )	// サイズｙ
#define WFNOTE_FRIENDLIST_TEXT_CGX1	( WFNOTE_FRIENDLIST_TEXT_CGX0+(WFNOTE_FRIENDLIST_TEXT_SIZX*WFNOTE_FRIENDLIST_TEXT_SIZY) )	// サイズｙ

#define WFNOTE_FRIENDLIST_BACKMSG_X	( 13 )	// 開始位置ｘ
#define WFNOTE_FRIENDLIST_BACKMSG_Y	( 21 )	// 開始位置ｙ
#define WFNOTE_FRIENDLIST_BACKMSG_SIZX	( 8 )	// サイズｘ
#define WFNOTE_FRIENDLIST_BACKMSG_SIZY	( 3 )	// サイズｙ
#define WFNOTE_FRIENDLIST_BACKMSG_CGX	( WFNOTE_BG_TITLEBMP_CGX_END )

#define WFNOTE_FRIENDLIST_MENU_X	( 16 )	// 開始位置ｘ
#define WFNOTE_FRIENDLIST_MENU_Y	( 9 )	// 開始位置ｙ
#define WFNOTE_FRIENDLIST_MENU_SIZX	( 15 )	// サイズｘ
#define WFNOTE_FRIENDLIST_MENU_SIZY	( 8 )	// サイズｙ
#define WFNOTE_FRIENDLIST_MENU_CGX	( WFNOTE_FRIENDLIST_BACKMSG_CGX+(WFNOTE_FRIENDLIST_BACKMSG_SIZX*WFNOTE_FRIENDLIST_BACKMSG_SIZY) )

#define WFNOTE_FRIENDLIST_TALK_X		( 2 )	// 開始位置（キャラ単位）
#define WFNOTE_FRIENDLIST_TALK_Y		( 19 )	// 開始位置（キャラ単位）
#define WFNOTE_FRIENDLIST_TALK_SIZX		( 27 )	// サイズ（キャラ単位）
#define WFNOTE_FRIENDLIST_TALK_SIZY		( 4 )	// サイズ（キャラ単位）
#define WFNOTE_FRIENDLIST_TALK_CGX		( WFNOTE_FRIENDLIST_MENU_CGX+(WFNOTE_FRIENDLIST_MENU_SIZX*WFNOTE_FRIENDLIST_MENU_SIZY) )	// キャラクタオフセット

#define WFNOTE_FRIENDLIST_YESNO_FRM		( GF_BGL_FRAME1_M )
#define WFNOTE_FRIENDLIST_YESNO_X		( 25 )	// 開始位置（キャラ単位）
#define WFNOTE_FRIENDLIST_YESNO_Y		( 13 )	// 開始位置（キャラ単位）
#define WFNOTE_FRIENDLIST_YESNO_SIZX		( 6 )	// サイズ（キャラ単位）
#define WFNOTE_FRIENDLIST_YESNO_SIZY		( 4 )	// サイズ（キャラ単位）
#define WFNOTE_FRIENDLIST_YESNOCGX		( WFNOTE_FRIENDLIST_TALK_CGX+(WFNOTE_FRIENDLIST_TALK_SIZX*WFNOTE_FRIENDLIST_TALK_SIZY) )	// キャラクタオフセット
	

// 動作シーケンス
enum{
	WFNOTE_FRIENDLIST_SEQ_INIT_PAGEINIT,
	WFNOTE_FRIENDLIST_SEQ_INIT,
	WFNOTE_FRIENDLIST_SEQ_MAIN,
	WFNOTE_FRIENDLIST_SEQ_SCRLLINIT,
	WFNOTE_FRIENDLIST_SEQ_SCRLL,

	WFNOTE_FRIENDLIST_SEQ_MENUINIT,	//　メニュー選択
	WFNOTE_FRIENDLIST_SEQ_MENUWAIT,	// メニュー選択待ち
	WFNOTE_FRIENDLIST_SEQ_INFOINIT,		// 詳しく見るへ
	WFNOTE_FRIENDLIST_SEQ_INFO,		// 詳しく見るへ
	WFNOTE_FRIENDLIST_SEQ_INFORET,	// 詳しく見るへ
	WFNOTE_FRINEDLIST_SEQ_NAMECHG_INIT,	// 名前変更
	WFNOTE_FRINEDLIST_SEQ_NAMECHG_WIPE,	// 名前変更
	WFNOTE_FRINEDLIST_SEQ_NAMECHG_WAIT,	// 名前変更
	WFNOTE_FRINEDLIST_SEQ_NAMECHG_WAITWIPE,	// 名前変更
	WFNOTE_FRINEDLIST_SEQ_NAMECHG_WAITWIPEWAIT,	// 名前変更
	WFNOTE_FRIENDLIST_SEQ_DELETE_INIT,	// 破棄
	WFNOTE_FRIENDLIST_SEQ_DELETE_YESNODRAW,	// 破棄
	WFNOTE_FRIENDLIST_SEQ_DELETE_WAIT,	// 破棄YESNOをまってから処理
	WFNOTE_FRIENDLIST_SEQ_DELETE_ANMSTART,	// 破棄YESNOをまってから処理
	WFNOTE_FRIENDLIST_SEQ_DELETE_ANMWAIT,	// 破棄YESNOをまってから処理
	WFNOTE_FRIENDLIST_SEQ_DELETE_END,		//  破棄YESNOをまってから処理
	WFNOTE_FRIENDLIST_SEQ_CODE_INIT,	// コード切り替え
	WFNOTE_FRIENDLIST_SEQ_CODE_WAIT,	// 何かキーを押してもらう
	WFNOTE_FRIENDLIST_SEQ_END,
};

// しおり
#define WFNOTE_FRIENDLIST_SHIORI_X		( 0x16 )	// 位置（キャラサイズ）
#define WFNOTE_FRIENDLIST_SHIORI_Y		( 0 )		// 位置(キャラサイズ)
#define WFNOTE_FRIENDLIST_SHIORI_SIZX	( 9 )		// サイズ（キャラクタサイズ）
#define WFNOTE_FRIENDLIST_SHIORI_SIZY	( 3 )		// サイズ（キャラクタサイズ）
#define WFNOTE_FRIENDLIST_SHIORI_ONEX	( 2 )		// １ページ分のサイズ
#define WFNOTE_FRIENDLIST_SHIORI_OFSX	( 1 )		// ONEXの中で表示するキャラクタまでの数
#define WFNOTE_FRIENDLIST_SHIORI_OFSY	( 1 )		// ONEXの中で表示するキャラクタまでの数
#define WFNOTE_FRIENDLIST_SHIORISCRN_X	( 0 )		// スクリーン読み込みｘ
#define WFNOTE_FRIENDLIST_SHIORISCRN_Y	( 4 )		// スクリーン読み込みｙ
#define WFNOTE_FRIENDLIST_CL_SHIORISCRN_X	( 0x10 )		// スクリーン読み込みｘ
#define WFNOTE_FRIENDLIST_CL_SHIORISCRN_Y	( 0x0 )		// スクリーン読み込みｙ
#define WFNOTE_FRIENDLIST_SHIORISCRNPOS_X	( 3 )	// ポジションスクリーン読み込みｘ
#define WFNOTE_FRIENDLIST_EFFECTSHIORISCRNPOS_X	( 4 )	// エフェクト用ポジションスクリーン読み込みｘ
enum{												// ポジションスクリーン読み込みｙ
	WFNOTE_FRIENDLIST_SHIORISCRNPOS0_Y = 0,
	WFNOTE_FRIENDLIST_SHIORISCRNPOS1_Y,
	WFNOTE_FRIENDLIST_SHIORISCRNPOS2_Y,
	WFNOTE_FRIENDLIST_SHIORISCRNPOS3_Y,
};
#define WFNOTE_FRIENDLIST_SHIORIPOS_SIZ	(1)			// ポジションスクリーンの大きさ

// もどる表示位置
#define WFNOTE_FRIENDLIST_BACK_X		( 11 )	// 戻る表示位置
#define WFNOTE_FRIENDLIST_BACK_Y		( 20 )	// 戻る表示位置
#define WFNOTE_FRIENDLIST_BACK_SIZX		( 10 )	// 戻る表示サイズ
#define WFNOTE_FRIENDLIST_BACK_SIZY		( 4 )	// 戻る表示サイズ
#define WFNOTE_FRIENDLIST_SCRBACK_X		( 6 )	// 戻る読み込み先位置
#define WFNOTE_FRIENDLIST_SCRBACK_Y		( 0 )	// 戻る読み込み先位置
#define WFNOTE_FRIENDLIST_CL_SCRBACK_X		( 0 )	// 戻る読み込み先位置
#define WFNOTE_FRIENDLIST_CL_SCRBACK_Y		( 8 )	// 戻る読み込み先位置

// プレイヤー表示位置（sc_WFNOTE_FRIENDLIST_CURSORDATAないの位置データからのオフセット）
#define WFNOTE_FRIENDLIST_PLAYER_X		( 8 )
#define WFNOTE_FRIENDLIST_PLAYER_Y		( 6+CLACT_MAIN2_REND_Y_S32 )
#define WFNOTE_FRIENDLIST_PRI			( 8 )
#define WFNOTE_FRIENDLIST_BGPRI			( 1 )
// はてなの場合
#define WFNOTE_FRIENDLIST_NOTPlAYER_SIZX	(3)
#define WFNOTE_FRIENDLIST_NOTPlAYER_SIZY	(4)
#define WFNOTE_FRIENDLIST_NOTPlAYERSCRN_X	(0)
#define WFNOTE_FRIENDLIST_NOTPlAYERSCRN_Y	(0)

// プレイヤー名表示位置（sc_WFNOTE_FRIENDLIST_CURSORDATAないの位置データからのオフセット）
#define WFNOTE_FRIENDLIST_PLAYERNAME_X	( 0 )
#define WFNOTE_FRIENDLIST_PLAYERNAME_Y	( -24 )

// SEQ_MAIN関数戻り値
enum{
	WFNOTE_FRIENDLIST_SEQMAINRET_NONE,
	WFNOTE_FRIENDLIST_SEQMAINRET_PAGECHANGELEFT,// ページ変更
	WFNOTE_FRIENDLIST_SEQMAINRET_PAGECHANGERIGHT,// ページ変更
	WFNOTE_FRIENDLIST_SEQMAINRET_CANCEL,	// 戻る
	WFNOTE_FRIENDLIST_SEQMAINRET_SELECTPL,	// 人を選択した
};


// スクロール
#define WFNOTE_FIRENDLIST_SCRLL_COUNT	(12)	// スクロールカウント（4の倍数）
#define WFNOTE_FRIENDLIST_SCRLL_SIZX	(256)	// スクロールサイズ


// メニューデータ
enum{
	WFNOTE_FRIENDLIST_MENU_NML,		// 通常メニュー
	WFNOTE_FRIENDLIST_MENU_CODE,	// 友達番号のみ登録時のメニュー
	WFNOTE_FRIENDLIST_MENU_NUM,
};
#define WFNOTE_FRIENDLIST_MENU_LISTNUM	(4)	// メニューリストの項目数

// 詳細画面へフェードのウエイト
#define WFNOTE_FRIENDLIST_INFO_WAIT	(4)

//  消しゴムアクター
#define WFNOTE_FRIENDLIST_CLEARACT_Y	( -4 + CLACT_MAIN2_REND_Y_S32 )

//-------------------------------------
///	CODEIN
//=====================================
enum{	// シーケンス
	WFNOTE_CODEIN_SEQ_NAMEIN,
	WFNOTE_CODEIN_SEQ_NAMEIN_WAIT,
	WFNOTE_CODEIN_SEQ_CODEIN_WAIT,
	WFNOTE_CODEIN_SEQ_END,

	WFNOTE_CODEIN_SEQ_NAMEINONLY,	// 名前入力のみ
	WFNOTE_CODEIN_SEQ_NAMEINONLY_WAIT,	// 名前入力のみ
	WFNOTE_CODEIN_SEQ_NAMEINONLY_END,	// 名前入力のみ
	
};
#define FRIENDCODE_MAXLEN	(12)	// 友達コード文字数

//-------------------------------------
///	MYCODE
//=====================================
// BMP	メッセージ
#define WFNOTE_MYCODE_MSG_X		( 5 )	// 開始位置（キャラ単位）
#define WFNOTE_MYCODE_MSG_Y		( 13 )	// 開始位置（キャラ単位）
#define WFNOTE_MYCODE_MSG_SIZX	( 23 )	// サイズ（キャラ単位）
#define WFNOTE_MYCODE_MSG_SIZY	( 8 )	// サイズ（キャラ単位）
#define WFNOTE_MYCODE_CGX		( 1 )	// キャラクタオフセット
// BMP	コード
#define WFNOTE_MYCODE_CODE_X		( 9 )	// 開始位置（キャラ単位）
#define WFNOTE_MYCODE_CODE_Y		( 8 )	// 開始位置（キャラ単位）
#define WFNOTE_MYCODE_CODE_SIZX		( 15 )	// サイズ（キャラ単位）
#define WFNOTE_MYCODE_CODE_SIZY		( 2 )	// サイズ（キャラ単位）
#define WFNOTE_MYCODE_CODE_CGX		( WFNOTE_MYCODE_CGX+(WFNOTE_MYCODE_MSG_SIZX*WFNOTE_MYCODE_MSG_SIZY) )	// キャラクタオフセット
#define WFNOTE_MYCODE_CODE_PR_X		( 4 )	// 表示座標

// シーケンス
enum{
	WFNOTE_MYCODE_SEQ_INIT,
	WFNOTE_MYCODE_SEQ_MAIN,
};

//-------------------------------------
///	FRIENDINFO
//=====================================
#define WFNOTE_FRIENDINFO_PAGE_NUM					( 7 )	// ページ数
#define WFNOTE_FRIENDINFO_FRONTIEROFFPAGE_NUM		( 2 )	// フロンティア無しページ数

// シーケンス
enum{
	WFNOTE_FRIENDINFO_SEQ_INIT,
	WFNOTE_FRIENDINFO_SEQ_INITRP,
	WFNOTE_FRIENDINFO_SEQ_MAIN,
	WFNOTE_FRIENDINFO_SEQ_SCRINIT,
	WFNOTE_FRIENDINFO_SEQ_SCR,
	WFNOTE_FRIENDINFO_SEQ_FRIENDCHG,
	WFNOTE_FRIENDINFO_SEQ_END,
};

// ビットマップ
#define WFNOTE_FRIENDINFO_TEXT_CGX0	( 1 )	// サイズｙ
#define WFNOTE_FRIENDINFO_TEXT_CGX1	( WFNOTE_FRIENDINFO_TEXT_CGX0+(500) )	// サイズｙ


// しおり
#define WFNOTE_FRIENDINFO_SHIORI_X		( 22 )	// 位置（キャラサイズ）
#define WFNOTE_FRIENDINFO_SHIORI_Y		( 0 )		// 位置(キャラサイズ)
#define WFNOTE_FRIENDINFO_SHIORI_SIZX	( 9 )		// サイズ（キャラクタサイズ）
#define WFNOTE_FRIENDINFO_SHIORI_SIZY	( 3 )		// サイズ（キャラクタサイズ）
#define WFNOTE_FRIENDINFO_SHIORI_ONEX	( 1 )		// １ページ分のサイズ
#define WFNOTE_FRIENDINFO_SHIORI_OFSX	( 1 )		// ONEXの中で表示するキャラクタまでの数
#define WFNOTE_FRIENDINFO_SHIORI_OFSY	( 1 )		// ONEXの中で表示するキャラクタまでの数
#define WFNOTE_FRIENDINFO_SHIORISCRN_X	( 16 )		// スクリーン読み込みｘ
#define WFNOTE_FRIENDINFO_SHIORISCRN_Y	( 4 )		// スクリーン読み込みｙ
#define WFNOTE_FRIENDINFO_CL_SHIORISCRN_X	( 0x10 )		// スクリーン読み込みｘ
#define WFNOTE_FRIENDINFO_CL_SHIORISCRN_Y	( 0x0 )		// スクリーン読み込みｙ
#define WFNOTE_FRIENDINFO_SHIORISCRNPOS_X	( 5 )	// ポジションスクリーン読み込みｘ
#define WFNOTE_FRIENDINFO_SHIORISCRNPOS_Y	( 0 )	// ポジションスクリーン読み込みｘ
#define WFNOTE_FRIENDINFO_EFFECTSHIORISCRNPOS_X	( 5 )	// エフェクト用ポジションスクリーン読み込みｘ
#define WFNOTE_FRIENDINFO_EFFECTSHIORISCRNPOS_Y	( 1 )	// エフェクト用ポジションスクリーン読み込みｘ
#define WFNOTE_FRIENDINFO_SHIORIPOS_SIZ	(1)			// ポジションスクリーンの大きさ

// スクロール
#define WFNOTE_FRIENDINFO_SCRLL_COUNT	(4)	// スクロールカウント
#define WFNOTE_FRIENDINFO_SCRLL_SIZX	(256)	// スクロールサイズ



// ページ００
enum{
	WFNOTE_FRIENDINFO_PAGE00_BA_GR,
	WFNOTE_FRIENDINFO_PAGE00_BA_BT,
	WFNOTE_FRIENDINFO_PAGE00_BA_TR,
	WFNOTE_FRIENDINFO_PAGE00_BA_DAY,
	WFNOTE_FRIENDINFO_PAGE00_BA_TRG,
	WFNOTE_FRIENDINFO_PAGE00_BA_NUM,
};
#define WFNOTE_FRIENDINFO_PAGE00_BA_GR_X	( 14 )	// ビットマップエリア	グループと対戦
#define WFNOTE_FRIENDINFO_PAGE00_BA_GR_Y	( 4 )	// ビットマップエリア	グループと対戦
#define WFNOTE_FRIENDINFO_PAGE00_BA_GR_SIZX	( 16 )	// ビットマップエリア	グループと対戦
#define WFNOTE_FRIENDINFO_PAGE00_BA_GR_SIZY ( 6 )	// ビットマップエリア	グループと対戦
#define WFNOTE_FRIENDINFO_PAGE00_BA_BT_X	( 22 )	// ビットマップエリア	対戦成績
#define WFNOTE_FRIENDINFO_PAGE00_BA_BT_Y	( 10 )	// ビットマップエリア	対戦成績
#define WFNOTE_FRIENDINFO_PAGE00_BA_BT_SIZX	( 8 )	// ビットマップエリア	対戦成績
#define WFNOTE_FRIENDINFO_PAGE00_BA_BT_SIZY ( 5 )	// ビットマップエリア	対戦成績
#define WFNOTE_FRIENDINFO_PAGE00_BA_TR_X	( 14 )	// ビットマップエリア	交換とポフィン
#define WFNOTE_FRIENDINFO_PAGE00_BA_TR_Y	( 15 )	// ビットマップエリア	交換とポフィン
#define WFNOTE_FRIENDINFO_PAGE00_BA_TR_SIZX	( 17 )	// ビットマップエリア	交換とポフィン
#define WFNOTE_FRIENDINFO_PAGE00_BA_TR_SIZY ( 6 )	// ビットマップエリア	交換とポフィン
#define WFNOTE_FRIENDINFO_PAGE00_BA_DAY_X	( 2 )	// ビットマップエリア	日にち
#define WFNOTE_FRIENDINFO_PAGE00_BA_DAY_Y	( 21 )	// ビットマップエリア	日にち
#define WFNOTE_FRIENDINFO_PAGE00_BA_DAY_SIZX	( 28 )	// ビットマップエリア	日にち
#define WFNOTE_FRIENDINFO_PAGE00_BA_DAY_SIZY ( 3 )	// ビットマップエリア	日にち
#define WFNOTE_FRIENDINFO_PAGE00_BA_TRG_X	( 1 )	// ビットマップエリア	トレーナーグラフィック
#define WFNOTE_FRIENDINFO_PAGE00_BA_TRG_Y	( 5 )	// ビットマップエリア	トレーナーグラフィック
#define WFNOTE_FRIENDINFO_PAGE00_BA_TRG_SIZX	( 11 )	// ビットマップエリア	トレーナーグラフィック
#define WFNOTE_FRIENDINFO_PAGE00_BA_TRG_SIZY ( 13 )	// ビットマップエリア	トレーナーグラフィック
#define WFNOTE_FRIENDINFO_PAGE00_GRT_X	( 0 )	// グループタイトル
#define WFNOTE_FRIENDINFO_PAGE00_GRT_Y	( 4 )	// グループタイトル
#define WFNOTE_FRIENDINFO_PAGE00_GRT2_X ( 128 )
#define WFNOTE_FRIENDINFO_PAGE00_GRT2_Y ( 4 )
#define WFNOTE_FRIENDINFO_PAGE00_BTT_X	( 0 )	// 対戦成績タイトル
#define WFNOTE_FRIENDINFO_PAGE00_BTT_Y	( 28 )	// 対戦成績タイトル
#define WFNOTE_FRIENDINFO_PAGE00_BTW_X	( 0 )	// 対戦成績かち
#define WFNOTE_FRIENDINFO_PAGE00_BTW_Y	( 4 )	// 対戦成績かち
#define WFNOTE_FRIENDINFO_PAGE00_BTL_X	( 0 )	// 対戦成績まけ
#define WFNOTE_FRIENDINFO_PAGE00_BTL_Y	( 20 )	// 対戦成績まけ
#define WFNOTE_FRIENDINFO_PAGE00_BTWLN_X ( 64 )// 対戦成績かちまけの数字
#define WFNOTE_FRIENDINFO_PAGE00_TRT_X	( 0 )	// ポケモン交換タイトル
#define WFNOTE_FRIENDINFO_PAGE00_TRT_Y	( 4 )	// ポケモン交換タイトル
#define WFNOTE_FRIENDINFO_PAGE00_TRN_X	( 128 )	// ポケモン交換値
#define WFNOTE_FRIENDINFO_PAGE00_TRN_Y	( 4 )	// ポケモン交換値
#define WFNOTE_FRIENDINFO_PAGE00_POT_X	( 0 )	// ポフィンタイトル
#define WFNOTE_FRIENDINFO_PAGE00_POT_Y	( 20 )	// ポフィンタイトル
#define WFNOTE_FRIENDINFO_PAGE00_PON_X	( 128 )	// ポフィン値
#define WFNOTE_FRIENDINFO_PAGE00_PON_Y	( 20 )	// ポフィン値
#define WFNOTE_FRIENDINFO_PAGE00_DAYT_X	( 0 )	// 最後に遊んだ日付タイトル
#define WFNOTE_FRIENDINFO_PAGE00_DAYT_Y	( 6 )	// 最後に遊んだ日付タイトル
#define WFNOTE_FRIENDINFO_PAGE00_DAYN_X	( 224 )	// 最後に遊んだ日付値
#define WFNOTE_FRIENDINFO_PAGE00_DAYN_Y	( 6 )	// 最後に遊んだ日付値
#define WFNOTE_FRIENDINFO_PAGE00_TRNNA_X	( 44 )	// トレーナー名前
#define WFNOTE_FRIENDINFO_PAGE00_TRNNA_Y	( 88 )	// トレーナー名前
#define WFNOTE_FRIENDINFO_PAGE00_TRNGR_X	( 4 )	// トレーナーグラフィック
#define WFNOTE_FRIENDINFO_PAGE00_TRNGR_Y	( 4 )	// トレーナーグラフィック
#define WFNOTE_FRIENDINFO_PAGE00_TRGRA_SIZX	( 10 )	// トレーナーグラフィックサイズｘ
#define WFNOTE_FRIENDINFO_PAGE00_TRGRA_SIZY	( 10 )	// トレーナーグラフィックサイズｙ
#define WFNOTE_FRIENDINFO_PAGE00_TRGRA_CUT_X	(0)	// 切り取り先
#define WFNOTE_FRIENDINFO_PAGE00_TRGRA_CUT_Y	(0)	// 切り取り先
#define WFNOTE_FRIENDINFO_PAGE00_TRNGR_CSIZX ( 11 )	// トレーナーグラフィック
#define WFNOTE_FRIENDINFO_PAGE00_TRNGR_CSIZY ( 11 )	// トレーナーグラフィック

// ページ０１
enum{
	WFNOTE_FRIENDINFO_PAGE01_BA,
	WFNOTE_FRIENDINFO_PAGE01_BA_NUM,
};
#define WFNOTE_FRIENDINFO_PAGE01_BA_X	( 2 )	// ビットマップエリア
#define WFNOTE_FRIENDINFO_PAGE01_BA_Y	( 4 )	// ビットマップエリア
#define WFNOTE_FRIENDINFO_PAGE01_BA_SIZX ( 20 )	// ビットマップエリア
#define WFNOTE_FRIENDINFO_PAGE01_BA_SIZY ( 9 )	// ビットマップエリア
#define WFNOTE_FRIENDINFO_PAGE01_BTT_X		( 0 )	// バトルタワータイトル
#define WFNOTE_FRIENDINFO_PAGE01_BTT_Y		( 8 )	// バトルタワータイトル
#define WFNOTE_FRIENDINFO_PAGE01_BTL_X		( 0 )	// バトルタワータイトル
#define WFNOTE_FRIENDINFO_PAGE01_BTLN_X		( 56 )	// バトルタワータイトル
#define WFNOTE_FRIENDINFO_PAGE01_BTL_Y		( 32 )	// バトルタワータイトル
#define WFNOTE_FRIENDINFO_PAGE01_BTM_X		( 0 )	// バトルタワータイトル
#define WFNOTE_FRIENDINFO_PAGE01_BTMN_X		( 56 )	// バトルタワータイトル
#define WFNOTE_FRIENDINFO_PAGE01_BTM_Y		( 56 )	// バトルタワータイトル

// ページ02
enum{
	WFNOTE_FRIENDINFO_PAGE02_BA_TT,		// タイトル
	WFNOTE_FRIENDINFO_PAGE02_LV50TT,	// LV50タイトル
	WFNOTE_FRIENDINFO_PAGE02_LV50NO,	// LV50値
	WFNOTE_FRIENDINFO_PAGE02_LV100TT,	// LV100タイトル
	WFNOTE_FRIENDINFO_PAGE02_LV100NO,	// LV100値
	WFNOTE_FRIENDINFO_PAGE02_BA_NUM,
};

#define WFNOTE_FRIENDINFO_PAGE02_BA_TT_X	( 2 )	// ビットマップエリア	タイトル
#define WFNOTE_FRIENDINFO_PAGE02_BA_TT_Y	( 5 )	// ビットマップエリア	タイトル
#define WFNOTE_FRIENDINFO_PAGE02_BA_TT_SIZX ( 14 )	// ビットマップエリア	タイトル
#define WFNOTE_FRIENDINFO_PAGE02_BA_TT_SIZY ( 2 )	// ビットマップエリア	タイトル
#define WFNOTE_FRIENDINFO_PAGE02_BA_LV50TT_X	( 2 )	// ビットマップエリア	LV50タイトル
#define WFNOTE_FRIENDINFO_PAGE02_BA_LV50TT_Y	( 8 )	// ビットマップエリア	LV50タイトル
#define WFNOTE_FRIENDINFO_PAGE02_BA_LV50TT_SIZX ( 28 )	// ビットマップエリア	LV50タイトル
#define WFNOTE_FRIENDINFO_PAGE02_BA_LV50TT_SIZY ( 2 )	// ビットマップエリア	LV50タイトル
#define WFNOTE_FRIENDINFO_PAGE02_BA_LV50NO_X	( 2 )	// ビットマップエリア	LV50値
#define WFNOTE_FRIENDINFO_PAGE02_BA_LV50NO_Y	( 11 )	// ビットマップエリア	LV50値
#define WFNOTE_FRIENDINFO_PAGE02_BA_LV50NO_SIZX ( 28 )	// ビットマップエリア	LV50値
#define WFNOTE_FRIENDINFO_PAGE02_BA_LV50NO_SIZY ( 4 )	// ビットマップエリア	LV50値
#define WFNOTE_FRIENDINFO_PAGE02_BA_LV100TT_X	( 2 )	// ビットマップエリア	LV100タイトル
#define WFNOTE_FRIENDINFO_PAGE02_BA_LV100TT_Y	( 16 )	// ビットマップエリア	LV100タイトル
#define WFNOTE_FRIENDINFO_PAGE02_BA_LV100TT_SIZX ( 28 )	// ビットマップエリア	LV100タイトル
#define WFNOTE_FRIENDINFO_PAGE02_BA_LV100TT_SIZY ( 2 )	// ビットマップエリア	LV100タイトル
#define WFNOTE_FRIENDINFO_PAGE02_BA_LV100NO_X	( 2 )	// ビットマップエリア	LV100値
#define WFNOTE_FRIENDINFO_PAGE02_BA_LV100NO_Y	( 19 )	// ビットマップエリア	LV100値
#define WFNOTE_FRIENDINFO_PAGE02_BA_LV100NO_SIZX ( 28 )	// ビットマップエリア	LV100値
#define WFNOTE_FRIENDINFO_PAGE02_BA_LV100NO_SIZY ( 4 )	// ビットマップエリア	LV100値
#define WFNOTE_FRIENDINFO_PAGE02_TT_X		( 0 )	// タイトル
#define WFNOTE_FRIENDINFO_PAGE02_TT_Y		( 0 )	// タイトル
#define WFNOTE_FRIENDINFO_PAGE02_LV50TT_X	( 0 )	// LV50タイトル
#define WFNOTE_FRIENDINFO_PAGE02_LV50TT2_X	( 120 )	// LV50タイトル
#define WFNOTE_FRIENDINFO_PAGE02_LV50TT_Y	( 0 )	// LV50タイトル
#define WFNOTE_FRIENDINFO_PAGE02_LV50LT_X	( 0 )	// LV50前回
#define WFNOTE_FRIENDINFO_PAGE02_LV50LN0_X	( 56 )	// LV50前回
#define WFNOTE_FRIENDINFO_PAGE02_LV50LN1_X	( 168 )	// LV50前回
#define WFNOTE_FRIENDINFO_PAGE02_LV50L_Y	( 0 )	// LV50前回
#define WFNOTE_FRIENDINFO_PAGE02_LV50MT_X	( 0 )	// LV50最高
#define WFNOTE_FRIENDINFO_PAGE02_LV50MN0_X	( 56 )	// LV50最高
#define WFNOTE_FRIENDINFO_PAGE02_LV50MN1_X	( 168 )	// LV50最高
#define WFNOTE_FRIENDINFO_PAGE02_LV50M_Y	( 16 )	// LV50最高
#define WFNOTE_FRIENDINFO_PAGE02_LV100TT_X	( 0 )	// LV100タイトル
#define WFNOTE_FRIENDINFO_PAGE02_LV100TT2_X	( 120 )	// LV100タイトル
#define WFNOTE_FRIENDINFO_PAGE02_LV100TT_Y	( 0 )	// LV100タイトル
#define WFNOTE_FRIENDINFO_PAGE02_LV100LT_X	( 0 )	// LV100前回
#define WFNOTE_FRIENDINFO_PAGE02_LV100LN0_X	( 56 )	// LV100前回
#define WFNOTE_FRIENDINFO_PAGE02_LV100LN1_X	( 168 )	// LV100前回
#define WFNOTE_FRIENDINFO_PAGE02_LV100L_Y	( 0 )	// LV100前回
#define WFNOTE_FRIENDINFO_PAGE02_LV100MT_X	( 0 )	// LV100最高
#define WFNOTE_FRIENDINFO_PAGE02_LV100MN0_X	( 56 )	// LV100最高
#define WFNOTE_FRIENDINFO_PAGE02_LV100MN1_X	( 168 )	// LV100最高
#define WFNOTE_FRIENDINFO_PAGE02_LV100M_Y	( 16 )	// LV100最高


// ページ03
enum{
	WFNOTE_FRIENDINFO_PAGE03_BA,
	WFNOTE_FRIENDINFO_PAGE03_BA_NUM,
};
#define WFNOTE_FRIENDINFO_PAGE03_BA_X	( 2 )	// ビットマップエリア
#define WFNOTE_FRIENDINFO_PAGE03_BA_Y	( 5 )	// ビットマップエリア
#define WFNOTE_FRIENDINFO_PAGE03_BA_SIZX ( 28 )	// ビットマップエリア
#define WFNOTE_FRIENDINFO_PAGE03_BA_SIZY ( 11 )	// ビットマップエリア

#define WFNOTE_FRIENDINFO_PAGE03_TT_X	( 0 )	// タイトル
#define WFNOTE_FRIENDINFO_PAGE03_TT_Y	( 0 )	// タイトル
#define WFNOTE_FRIENDINFO_PAGE03_CPT_X	( 104 )	// キャッスルポイントタイトル
#define WFNOTE_FRIENDINFO_PAGE03_CPT_Y	( 20 )	// キャッスルポイントタイトル
#define WFNOTE_FRIENDINFO_PAGE03_CPLT_X	( 0 )	// キャッスルポイント前回
#define WFNOTE_FRIENDINFO_PAGE03_CPLN0_X	( 56 )	// キャッスルポイント前回
#define WFNOTE_FRIENDINFO_PAGE03_CPLN1_X	( 168 )	// キャッスルポイント前回
#define WFNOTE_FRIENDINFO_PAGE03_CPL_Y	( 48 )	// キャッスルポイント前回
#define WFNOTE_FRIENDINFO_PAGE03_CPMT_X	( 0 )	// キャッスルポイント最大
#define WFNOTE_FRIENDINFO_PAGE03_CPMN0_X	( 56 )	// キャッスルポイント最大
#define WFNOTE_FRIENDINFO_PAGE03_CPMN1_X	( 168 )	// キャッスルポイント最大
#define WFNOTE_FRIENDINFO_PAGE03_CPM_Y	( 72 )	// キャッスルポイント最大


// ページ04
enum{
	WFNOTE_FRIENDINFO_PAGE04_BA,
	WFNOTE_FRIENDINFO_PAGE04_BA_NUM,
};
#define WFNOTE_FRIENDINFO_PAGE04_BA_X	( 2 )	// ビットマップエリア
#define WFNOTE_FRIENDINFO_PAGE04_BA_Y	( 5 )	// ビットマップエリア
#define WFNOTE_FRIENDINFO_PAGE04_BA_SIZX ( 20 )	// ビットマップエリア
#define WFNOTE_FRIENDINFO_PAGE04_BA_SIZY ( 11 )	// ビットマップエリア

#define WFNOTE_FRIENDINFO_PAGE04_TT_X	( 0 )	// タイトル
#define WFNOTE_FRIENDINFO_PAGE04_TT_Y	( 0 )	// タイトル
#define WFNOTE_FRIENDINFO_PAGE04_TT2_Y	( 24 )	// タイトル
#define WFNOTE_FRIENDINFO_PAGE04_LT_X	( 0 )	// 前回
#define WFNOTE_FRIENDINFO_PAGE04_LN_X	( 56 )	// 前回
#define WFNOTE_FRIENDINFO_PAGE04_L_Y	( 48 )	// 前回
#define WFNOTE_FRIENDINFO_PAGE04_MT_X	( 0 )	// 最大
#define WFNOTE_FRIENDINFO_PAGE04_MN_X	( 56 )	// 最大
#define WFNOTE_FRIENDINFO_PAGE04_M_Y	( 72 )	// 最大


// ページ05
enum{
	WFNOTE_FRIENDINFO_PAGE05_BA,
	WFNOTE_FRIENDINFO_PAGE05_BA_NUM,
};
#define WFNOTE_FRIENDINFO_PAGE05_BA_X	( 2 )	// ビットマップエリア
#define WFNOTE_FRIENDINFO_PAGE05_BA_Y	( 5 )	// ビットマップエリア
#define WFNOTE_FRIENDINFO_PAGE05_BA_SIZX ( 20 )	// ビットマップエリア
#define WFNOTE_FRIENDINFO_PAGE05_BA_SIZY ( 11 )	// ビットマップエリア

#define WFNOTE_FRIENDINFO_PAGE05_TT_X	( 0 )	// タイトル
#define WFNOTE_FRIENDINFO_PAGE05_TT_Y	( 0 )	// タイトル
#define WFNOTE_FRIENDINFO_PAGE05_LT_X	( 0 )	// 前回
#define WFNOTE_FRIENDINFO_PAGE05_LN_X	( 56 )	// 前回
#define WFNOTE_FRIENDINFO_PAGE05_L_Y	( 24 )	// 前回
#define WFNOTE_FRIENDINFO_PAGE05_MT_X	( 0 )	// 最大
#define WFNOTE_FRIENDINFO_PAGE05_MN_X	( 56 )	// 最大
#define WFNOTE_FRIENDINFO_PAGE05_M_Y	( 48 )	// 最大

// ページ06
enum{
	WFNOTE_FRIENDINFO_PAGE06_BA,
	WFNOTE_FRIENDINFO_PAGE06_BA_NUM,
};
#define WFNOTE_FRIENDINFO_PAGE06_BA_X	( 2 )	// ビットマップエリア
#define WFNOTE_FRIENDINFO_PAGE06_BA_Y	( 5 )	// ビットマップエリア
#define WFNOTE_FRIENDINFO_PAGE06_BA_SIZX ( 22 )	// ビットマップエリア
#define WFNOTE_FRIENDINFO_PAGE06_BA_SIZY ( 11 )	// ビットマップエリア

#define WFNOTE_FRIENDINFO_PAGE06_TT_X	( 0 )	// タイトル
#define WFNOTE_FRIENDINFO_PAGE06_TT_Y	( 0 )	// タイトル
#define WFNOTE_FRIENDINFO_PAGE06_BC_X	( 0 )	// たまなげ
#define WFNOTE_FRIENDINFO_PAGE06_BC_N_X	( 120 )	// 値
#define WFNOTE_FRIENDINFO_PAGE06_BC_Y	( 24 )	// Y
#define WFNOTE_FRIENDINFO_PAGE06_BB_X	( 0 )	// たまのり
#define WFNOTE_FRIENDINFO_PAGE06_BB_N_X	( 120 )	// 値
#define WFNOTE_FRIENDINFO_PAGE06_BB_Y	( 48 )	// Y
#define WFNOTE_FRIENDINFO_PAGE06_BL_X	( 0 )	// たまのり
#define WFNOTE_FRIENDINFO_PAGE06_BL_N_X	( 120 )	// 値
#define WFNOTE_FRIENDINFO_PAGE06_BL_Y	( 72 )	// Y


// ページ変更ウエイト
#define WFNOTE_FRIENDINFO_PAGECHG_WAIT	( 4 )



//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	メニューデータ構造体
//=====================================
typedef struct {
	u32 strid;
	u32 param;
} WFNOTE_MENU_DATA;


//-------------------------------------
//	スクリーンエリアデータ
//=====================================
typedef struct {
	s16 scrn_x;		// 描画エリア開始スクリーンｘ
	s16 scrn_y;		// 描画エリア開始スクリーンy
	u8 scrn_sizx;	// 描画エリアサイズスクリーンｘ
	u8 scrn_sizy;	// 描画エリアサイズスクリーンｙ
} WFNOTE_SCRNAREA;


//-------------------------------------
//	スクリーンデータ
//=====================================
typedef struct {
	void* p_scrnbuff[WFNOTE_SCRNDATA_NUM];	// スクリーンデータ
	NNSG2dScreenData* p_scrn[WFNOTE_SCRNDATA_NUM];	// スクリーンデータ
} WFNOTE_SCRNDATA;


//-------------------------------------
///	コード入力結果データ
//=====================================
typedef struct {
	STRBUF* p_name;
	STRBUF* p_code;
} WFNOTE_CODEIN_DATA;

//-------------------------------------
///	名前入力初期化情報
//=====================================
typedef struct {
	const STRCODE* cp_str;
	BOOL init;
} WFNOTE_NAMEIN_INITDATA;


//-------------------------------------
///	友達インデックスデータ
//=====================================
typedef struct {
	u32 friendnum;	// 友達の総数
	u8 fridx[WFNOTE_FRIENDLIST_FRIENDMAX];	// 友達インデックス
} WFNOTE_IDXDATA;



//-------------------------------------
///	グローバルデータワーク
//=====================================
typedef struct {
	SAVEDATA* p_save;	// セーブデータ
	u8 subseq;			// サブシーケンス
	u8 reqstatus;		// 次に進みたいステータス
	u8 reqsubseq;		// 次のステータスの開始サブシーケンス
	s8 select_listidx;	// 選択したリストインデックス

	WFNOTE_IDXDATA idx;	// 友達データインデックス

	WFNOTE_CODEIN_DATA codein;	// コード入力画面で入力したデータ
	WFNOTE_NAMEIN_INITDATA namein_init;	// 名前入力画面初期化データ
} WFNOTE_DATA;

//-------------------------------------
///	表示ワーク
//=====================================
typedef struct {
	GF_BGL_INI* p_bgl;		// bgl
	CLACT_SET_PTR p_clact;	// アクター
	NNSG2dRenderSurface scrll_surface;	// スクロール用サーフェース
	CLACT_U_EASYRENDER_DATA	renddata;	// レンダーデータ
	CLACT_U_RES_MANAGER_PTR	p_resman[CLACT_RESOURCE_NUM];	// リソースマネージャ
	WORDSET* p_wordset;			// メッセージ展開用ワークマネージャー
	MSGDATA_MANAGER* p_msgman;	// 名前入力メッセージデータマネージャー
	ARCHANDLE* p_handle;		// グラフィックアーカイブハンドル
	GF_BGL_BMPWIN title;		// タイトル用ビットマップ
	STRBUF* p_titlestr;			// タイトル文字列
	STRBUF* p_titletmp;			// タイトル文字列
	u32 title_stridx;			// タイトル描画インデックス
	CLACT_HEADER clheader;		// 基本的なOAMのヘッダー
	CLACT_U_RES_OBJ_PTR p_resobj[CLACT_RESOURCE_NUM];// リソースオブジェ

	CLACT_WORK_PTR p_yazirushi[WFNOTE_YAZIRUSHINUM];	// 矢印アクター

	WFNOTE_SCRNDATA scrn;	// 基本スクリーンデータ

} WFNOTE_DRAW;

//-------------------------------------
///	モード選択画面ワーク
//=====================================
typedef struct {
	// データ
	u32 cursor;	// カーソル
	
	// グラフィック
	GF_BGL_BMPWIN msg;	// メッセージ面	
	void* p_scrnbuff;	// スクリーンデータ
	NNSG2dScreenData* p_scrn;	// スクリーンデータ
	GF_BGL_BMPWIN talk;	// 会話ウィンドウ
	BMPMENU_WORK* p_yesno;// YESNOウィンドウ
	u32 talkmsg_idx;	// 会話メッセージidx
	u32 msg_speed;	// 会話メッセージスピード
	STRBUF* p_str;
} WFNOTE_MODESELECT;

//-------------------------------------
///	友達データ表示画面ワーク
//	描画エリアデータ
//=====================================
typedef struct {
	WFNOTE_SCRNAREA scrn_area;	// 描画スクリーンエリア
	GF_BGL_BMPWIN text;	// メッセージ面
	WF_2DCWK* p_clwk[WFNOTE_FRIENDLIST_PAGEFRIEND_NUM];
} WFNOTE_FRIENDLIST_DRAWAREA;

//-------------------------------------
///	友達データカーソルデータ
//=====================================
typedef struct {
	WFNOTE_SCRNAREA scrn_data;
	u32 bg_frame;
} WFNOTE_FRIELDLIST_CURSORDATA;


//-------------------------------------
///	友達データ表示画面ワーク
//=====================================
typedef struct {
	s16 page;	// ページ
	s16 lastpage;
	s16 pos;	// ページ内の位置
	s16 lastpos;// 1つ前の位置

	WFNOTE_FRIENDLIST_DRAWAREA drawdata[WFNOTE_DRAWAREA_NUM];
	s16 count;	// スクロールカウンタ
	u16 way;	// スクロール方向
	WF_2DCSYS* p_charsys;	// キャラクタ表示システム

	GF_BGL_BMPWIN backmsg;	// 戻るメッセージ用

	GF_BGL_BMPWIN talk;	// トークメッセージ用 
	STRBUF* p_talkstr;
	u32 msgidx;			
	u32 msgspeed;
	GF_BGL_BMPWIN menu;	// メニューリスト用
	BMPLIST_DATA* p_menulist[WFNOTE_FRIENDLIST_MENU_NUM];	// メニューリスト
    BMPLIST_WORK* p_listwk;	// BMPメニューワーク
    
	BMPMENU_WORK* p_yesno;	// YESNOウィンドウ

	s32 wait;

	CLACT_WORK_PTR p_clearact;	// 消しゴムアクター
	u32 last_frame;
	
} WFNOTE_FRIENDLIST;

//-------------------------------------
///	友達コード入力画面ワーク
//=====================================
typedef struct {
	PROC* p_subproc;	// サブプロセス
	NAMEIN_PARAM* p_namein;	// 名前入力パラメータ
	CODEIN_PARAM* p_codein;	// コード入力パラメータ
} WFNOTE_CODEIN;

//-------------------------------------
///	自分の友達コード確認画面ワーク
//=====================================
typedef struct {
	// グラフィックデータ
	GF_BGL_BMPWIN msg;	// メッセージ面	
	GF_BGL_BMPWIN code;	// コード面	
	void* p_scrnbuff;	// スクリーンデータ
	NNSG2dScreenData* p_scrn;	// スクリーンデータ
} WFNOTE_MYCODE;



//-------------------------------------
///	友達詳細情報描画画面
///	画面構成スクリーンデータ
//=====================================
typedef struct {
	void* p_scrnbuff[ WFNOTE_FRIENDINFO_PAGE_NUM ];	// スクリーンデータ
	NNSG2dScreenData* p_scrn[ WFNOTE_FRIENDINFO_PAGE_NUM ];	// スクリーンデータ
} WFNOTE_FRIENDINFO_SCRNDATA;

//-------------------------------------
///	友達詳細情報描画画面
//	表示エリア情報
//=====================================
typedef struct {
	WFNOTE_SCRNAREA scrnarea;
	u32 cgx;
	GF_BGL_BMPWIN*	p_msg[ WFNOTE_FRIENDINFO_PAGE_NUM ];	//表示物
	u32 msgnum[ WFNOTE_FRIENDINFO_PAGE_NUM ];		// 今のmsgビットマップ数
} WFNOTE_FRIENDINFO_DRAWAREA;

//-------------------------------------
///	友達詳細情報表示画面ワーク
//=====================================
typedef struct {
	s16 page;	// 現在のページ
	s16 lastpage;	// １つ前のページ
	s32 count;	// スクロールカウント
	WF_COMMON_WAY way;	// 方向
	u16 wait;	// ウエイト
	u16 rep;	// 上下リピート判定
	BOOL frontier_draw;	// フロンティア描画有無
	
	
	WFNOTE_FRIENDINFO_DRAWAREA drawarea[WFNOTE_DRAWAREA_NUM];// 表示エリア
	WFNOTE_FRIENDINFO_SCRNDATA scrn;	// 画面構成スクリーンデータ
} WFNOTE_FRIENDINFO;

//-------------------------------------
///	ビットマップ登録データ
//=====================================
typedef struct {
	u8 pos_x;
	u8 pos_y;
	u8 siz_x;
	u8 siz_y;
} WFNOTE_FRIENDINFO_BMPDATA;



//-------------------------------------
///	友達手帳ワーク
//=====================================
typedef struct {
	u32 status;			// 今の状態
	WFNOTE_DRAW draw;	// 表示システムワーク
	WFNOTE_DATA data;	// データワーク
	
	// statusワーク
	WFNOTE_MODESELECT	mode_select;	// モードセレクト
	WFNOTE_FRIENDLIST	friendlist;		// 友達データ表示
	WFNOTE_CODEIN		codein;			// 友達コード入力
	WFNOTE_MYCODE		mycode;			// 自分の友達コード確認
	WFNOTE_FRIENDINFO	friendinfo;		// 友達詳細表示画面
	
	s32 wait;
} WFNOTE_WK;



//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	全体関数
//=====================================
static void WFNOTE_VBlank( void* p_work );
static void WFNOTE_DrawInit( WFNOTE_WK* p_wk, u32 heapID );
static void WFNOTE_DrawExit( WFNOTE_WK* p_wk );


//-------------------------------------
///	データワーク関数
//=====================================
static void WFNOTE_DATA_Init( WFNOTE_DATA* p_data, void* p_save, u32 heapID );
static void WFNOTE_DATA_Exit( WFNOTE_DATA* p_data );
static void WFNOTE_DATA_SetReqStatus( WFNOTE_DATA* p_data, u8 reqstatus, u8 reqsubseq );
static void WFNOTE_DATA_CODEIN_Set( WFNOTE_DATA* p_data, const STRBUF* cp_name, const STRBUF* cp_code );
static u32 WFNOTE_DATA_NewFriendDataSet( WFNOTE_DATA* p_data, STRBUF* p_code, STRBUF* p_name );
static void WFNOTE_DATA_NAMEIN_INIT_Set( WFNOTE_DATA* p_data, const STRCODE* cp_str );
static void WFNOTE_DATA_SELECT_ListIdxSet( WFNOTE_DATA* p_data, u32 idx );
static void WFNOTE_DATA_FrIdxMake( WFNOTE_DATA* p_data );
static u32 WFNOTE_DATA_FrIdxGetIdx( const WFNOTE_DATA* cp_data, u32 idx );

//-------------------------------------
///	描画ワーク関数
//=====================================
static void WFNOTE_DRAW_Init( WFNOTE_DRAW* p_draw, const WFNOTE_DATA* cp_data, u32 heapID );
static void WFNOTE_DRAW_Main( WFNOTE_DRAW* p_draw );
static void WFNOTE_DRAW_VBlank( WFNOTE_DRAW* p_draw );
static void WFNOTE_DRAW_Exit( WFNOTE_DRAW* p_draw );
static void WFNOTE_DRAW_BankInit( void );
static void WFNOTE_DRAW_BgInit( WFNOTE_DRAW* p_draw, u32 heapID );
static void WFNOTE_DRAW_BgExit( WFNOTE_DRAW* p_draw );
static void WFNOTE_DRAW_ClactInit( WFNOTE_DRAW* p_draw, u32 heapID );
static void WFNOTE_DRAW_ClactExit( WFNOTE_DRAW* p_draw );
static void WFNOTE_DRAW_MsgInit( WFNOTE_DRAW* p_draw, u32 heapID );
static void WFNOTE_DRAW_MsgExit( WFNOTE_DRAW* p_draw );
static void WFNOTE_DRAW_BmpInit( WFNOTE_DRAW* p_draw, const WFNOTE_DATA* cp_data, u32 heapID );
static void WFNOTE_DRAW_BmpExit( WFNOTE_DRAW* p_draw );
static void WFNOTE_DRAW_SCRNDATA_Init( WFNOTE_DRAW* p_draw, u32 heapID );
static void WFNOTE_DRAW_SCRNDATA_Exit( WFNOTE_DRAW* p_draw );
static void WFNOTE_DRAW_BmpTitleWrite( WFNOTE_DRAW* p_draw, u32 msg_idx );
static void WFNOTE_DRAW_BmpTitleOff( WFNOTE_DRAW* p_draw );
static void WFNOTE_DRAW_FriendCodeSetWordset( WFNOTE_DRAW* p_draw, u64 code );
static void WFNOTE_DRAW_FriendNameSetWordset( WFNOTE_DRAW* p_draw, SAVEDATA* p_save, u32 idx, u32 heapID );
static void WFNOTE_DRAW_FriendGroupSetWordset( WFNOTE_DRAW* p_draw, SAVEDATA* p_save, u32 idx, u32 heapID );
static BOOL WFNOTE_DRAW_FriendDaySetWordset( WFNOTE_DRAW* p_draw, SAVEDATA* p_save, u32 idx );
static void WFNOTE_DRAW_NumberSetWordset( WFNOTE_DRAW* p_draw, u32 num );
static void WFNOTE_DRAW_WflbyGameSetWordSet( WFNOTE_DRAW* p_draw, u32 num );
static void WFNOTE_DRAW_YazirushiInit( WFNOTE_DRAW* p_draw, u32 heapID );
static void WFNOTE_DRAW_YazirushiExit( WFNOTE_DRAW* p_draw );
static void WFNOTE_DRAW_YazirushiSetDrawFlag( WFNOTE_DRAW* p_draw, BOOL flag );
static void WFNOTE_DRAW_YazirushiSetAnmFlag( WFNOTE_DRAW* p_draw, BOOL flag );



//-------------------------------------
///	選択モード関数
//=====================================
static void WFNOTE_MODESELECT_Init( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static WFNOTE_STRET WFNOTE_MODESELECT_Main( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static void WFNOTE_MODESELECT_Exit( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static void WFNOTE_MODESELECT_DrawInit( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static void WFNOTE_MODESELECT_DrawExit( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static void WFNOTE_MODESELECT_SEQ_Init( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static BOOL WFNOTE_MODESELECT_SEQ_Main( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static void WFNOTE_MODESELECT_CursorScrnPalChange( WFNOTE_MODESELECT* p_wk, WFNOTE_DRAW* p_draw );
static void WFNOTE_MODESELECT_EndDataSet( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data );
static void WFNOTE_MODESELECT_DrawOff( WFNOTE_MODESELECT* p_wk, WFNOTE_DRAW* p_draw );
static void WFNOTE_MODESELECT_TalkMsgPrint( WFNOTE_MODESELECT* p_wk, WFNOTE_DRAW* p_draw, u32 msgidx, u32 heapID );
static BOOL WFNOTE_MODESELECT_TalkMsgEndCheck( const WFNOTE_MODESELECT* cp_wk );
static void WFNOTE_MODESELECT_TalkMsgOff( WFNOTE_MODESELECT* p_wk );
static BOOL WFNOTE_MODESELECT_StatusChengeCheck( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data );

//-------------------------------------
///	友達データ表示関数
//=====================================
static void WFNOTE_FRIENDLIST_Init( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static WFNOTE_STRET WFNOTE_FRIENDLIST_Main( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static void WFNOTE_FRIENDLIST_Exit( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static void WFNOTE_FRIENDLIST_DrawInit( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static void WFNOTE_FRIENDLIST_DrawExit( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static void WFNOTE_FRIENDLIST_SEQ_DrawOn( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static void WFNOTE_FRIENDLIST_SEQ_DrawOff( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static u32 WFNOTE_FRIENDLIST_SEQ_Main( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static void WFNOTE_FRIENDLIST_SEQ_ScrllInit( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static BOOL WFNOTE_FRIENDLIST_SEQ_ScrllMain( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static void WFNOTE_FRIENDLIST_SEQ_MenuInit( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static u32 WFNOTE_FRIENDLIST_SEQ_MenuWait( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static BOOL WFNOTE_FRIENDLIST_SEQ_CodeInit( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static void WFNOTE_FRIENDLIST_SEQ_DeleteInit( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static BOOL WFNOTE_FRIENDLIST_SEQ_DeleteYesNoDraw( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static u32 WFNOTE_FRIENDLIST_SEQ_DeleteWait( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static void WFNOTE_FRIENDLIST_DrawPage( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 page, u32 pos, u32 draw_area, u32 heapID );
static void WFNOTE_FRIENDLIST_DrawShiori( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DRAW* p_draw, u32 page );
static void WFNOTE_FRIENDLIST_DrawShioriEff00( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DRAW* p_draw, u32 page0, u32 page1 );
static void WFNOTE_FRIENDLIST_DrawShioriEff01( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DRAW* p_draw, u32 page0, u32 page1 );
static void WFNOTE_FRIENDLIST_ScrSeqChange( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WF_COMMON_WAY way );
static void WFNOTE_FRIENDLIST_TalkMsgWrite( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DRAW* p_draw, u32 msgid, u32 heapID );
static BOOL WFNOTE_FRIENDLIST_TalkMsgEndCheck( const WFNOTE_FRIENDLIST* cp_wk );
static void WFNOTE_FRIENDLIST_TalkMsgOff( WFNOTE_FRIENDLIST* p_wk );
static u32 WFNOTE_FRIENDLIST_FRIDX_IdxGet( const WFNOTE_FRIENDLIST* cp_wk );
static void WFNOTE_FRIENDLIST_MENULISTCallBack( BMPLIST_WORK * p_wk, u32 param, u8 mode );
static void WFNOTE_FRIENDLIST_DeleteAnmStart( WFNOTE_FRIENDLIST* p_wk );
static BOOL WFNOTE_FRIENDLIST_DeleteAnmMain( WFNOTE_FRIENDLIST* p_wk );
static void WFNOTE_FRIENDLIST_DeleteAnmOff( WFNOTE_FRIENDLIST* p_wk );
static void WFNOTE_FRIENDLIST_DRAWAREA_Init( WFNOTE_FRIENDLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, const WFNOTE_SCRNAREA* cp_area, u32 text_cgx, u32 heapID );
static void WFNOTE_FRIENDLIST_DRAWAREA_Exit( WFNOTE_FRIENDLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw );
static void WFNOTE_FRIENDLIST_DRAWAREA_BackWrite( WFNOTE_FRIENDLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, const WFNOTE_SCRNDATA* cp_scrn );
static void WFNOTE_FRIENDLIST_DRAWAREA_CursorWrite( WFNOTE_FRIENDLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, u32 pos );
static void WFNOTE_FRIENDLIST_DRAWAREA_WritePlayer( WFNOTE_FRIENDLIST_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, WF_2DCSYS* p_charsys, const WFNOTE_SCRNDATA* cp_scrn, const WFNOTE_IDXDATA* cp_idx, u32 page, u32 heapID );
static void WFNOTE_FRIENDLIST_DRAWAREA_DrawOff( WFNOTE_FRIENDLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, const WFNOTE_SCRNDATA* cp_scrn );
static void WFNOTE_FRIENDLIST_DRAWAREA_CharWkDel( WFNOTE_FRIENDLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw );
static void WFNOTE_FRIENDLIST_DRAWAREA_SetPlayer( WFNOTE_FRIENDLIST_DRAWAREA* p_wk, WF_2DCSYS* p_charsys, WFNOTE_DRAW* p_draw, const WFNOTE_SCRNDATA* cp_scrn, u32 pos, WIFI_LIST* p_list, u32 idx, u32 heapID );
static void WFNOTE_FRIENDLIST_DRAWAREA_SetNoPlayer( WFNOTE_FRIENDLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, const WFNOTE_SCRNDATA* cp_scrn, u32 pos );


//-------------------------------------
///	友達コード入力画面ワーク
//=====================================
static void WFNOTE_CODEIN_Init( WFNOTE_CODEIN* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static WFNOTE_STRET WFNOTE_CODEIN_Main( WFNOTE_CODEIN* p_wk, WFNOTE_WK* p_sys, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static void WFNOTE_CODEIN_Exit( WFNOTE_CODEIN* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static void WFNOTE_CODEIN_DrawInit( WFNOTE_CODEIN* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static void WFNOTE_CODEIN_DrawExit( WFNOTE_CODEIN* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static NAMEIN_PARAM* WFNOTE_CODEIN_NameInParamMake( WFNOTE_CODEIN* p_wk, WFNOTE_DATA* p_data, u32 heapID );

//-------------------------------------
///	自分の友達コード確認画面ワーク
//=====================================
static void WFNOTE_MYCODE_Init( WFNOTE_MYCODE* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static WFNOTE_STRET WFNOTE_MYCODE_Main( WFNOTE_MYCODE* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static void WFNOTE_MYCODE_Exit( WFNOTE_MYCODE* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static void WFNOTE_MYCODE_DrawInit( WFNOTE_MYCODE* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static void WFNOTE_MYCODE_DrawExit( WFNOTE_MYCODE* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static void WFNOTE_MYCODE_SEQ_Init( WFNOTE_MYCODE* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static void WFNOTE_MYCODE_DrawOff( WFNOTE_MYCODE* p_wk, WFNOTE_DRAW* p_draw );

//-------------------------------------
///	友達詳細情報
//=====================================
static void WFNOTE_FRIENDINFO_Init( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static WFNOTE_STRET WFNOTE_FRIENDINFO_Main( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static void WFNOTE_FRIENDINFO_Exit( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static void WFNOTE_FRIENDINFO_ScrollInit( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static BOOL WFNOTE_FRIENDINFO_ScrollMain( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static void WFNOTE_FRIENDINFO_DrawInit( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static void WFNOTE_FRIENDINFO_DrawExit( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw );
static void WFNOTE_FRIENDINFO_DrawOn( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static void WFNOTE_FRIENDINFO_DrawOff( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DRAW* p_draw );
static void WFNOTE_FRINEDINFO_DrawPage( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 drawarea, u32 page, u32 heapID );
static void WFNOTE_FRIENDINFO_ShioriDraw( WFNOTE_FRIENDINFO* p_wk,  WFNOTE_DRAW* p_draw, u32 page, u32 heapID );
static void WFNOTE_FRIENDINFO_ShioriOff( WFNOTE_FRIENDINFO* p_wk,  WFNOTE_DRAW* p_draw );
static void WFNOTE_FRIENDINFO_ShioriAnm00( WFNOTE_FRIENDINFO* p_wk,  WFNOTE_DRAW* p_draw, u32 page00, u32 page01 );
static void WFNOTE_FRIENDINFO_ShioriAnm01( WFNOTE_FRIENDINFO* p_wk,  WFNOTE_DRAW* p_draw, u32 page00, u32 page01 );
static BOOL WFNOTE_FRIENDINFO_SelectListIdxAdd( WFNOTE_DATA* p_data, WF_COMMON_WAY way );
static void WFNOTE_FRIENDINFO_SCRNDATA_Init( WFNOTE_FRIENDINFO_SCRNDATA* p_scrn, WFNOTE_DRAW* p_draw, u32 heapID );
static void WFNOTE_FRIENDINFO_SCRNDATA_Exit( WFNOTE_FRIENDINFO_SCRNDATA* p_scrn );
static void WFNOTE_FRIENDINFO_DRAWAREA_Init( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, const WFNOTE_SCRNAREA* cp_scrnarea, u32 cgx, u32 heapID );
static void WFNOTE_FRIENDINFO_DRAWAREA_Exit( WFNOTE_FRIENDINFO_DRAWAREA* p_wk );
static void WFNOTE_FRIENDINFO_DRAWAREA_MSGBmpInit( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, u32 idx, u32 num, const WFNOTE_FRIENDINFO_BMPDATA* cp_data, u32 heapID );
static void WFNOTE_FRIENDINFO_DRAWAREA_MSGBmpExit( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, u32 idx );
static void WFNOTE_FRIENDINFO_DRAWAREA_MSGBmpOnVReq( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, u32 idx );
static void WFNOTE_FRIENDINFO_DRAWAREA_MSGBmpOffVReq( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, u32 idx );
static void WFNOTE_FRIENDINFO_DRAWAREA_Page( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 page, const WFNOTE_FRIENDINFO_SCRNDATA* cp_scrn, u32 heapID );
static void WFNOTE_FRIENDINFO_DRAWAREA_PageOff( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw );
static void WFNOTE_FRIENDINFO_DRAW_Page00( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static void WFNOTE_FRIENDINFO_DRAW_Page01( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static void WFNOTE_FRIENDINFO_DRAW_Page02( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static void WFNOTE_FRIENDINFO_DRAW_Page03( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static void WFNOTE_FRIENDINFO_DRAW_Page04( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static void WFNOTE_FRIENDINFO_DRAW_Page05( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static void WFNOTE_FRIENDINFO_DRAW_Page06( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID );
static void WFNOTE_FRIENDINFO_DRAW_Clean( WFNOTE_DRAW* p_draw, const WFNOTE_SCRNAREA* cp_scrnarea );
static void WFNOTE_FRIENDINFO_DRAW_Bmp( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, u32 page, u32 bmp, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 msg_idx, u32 x, u32 y, u32 col, STRBUF* p_str, STRBUF* p_tmp, int align );
static void WFNOTE_FRIENDINFO_PageChange( WFNOTE_FRIENDINFO* p_wk, s32 add );
static BOOL WFNOTE_FRIENDINFO_PofinCaseCheck( WFNOTE_DATA* p_data, u32 heapID );




//-----------------------------------------------------------------------------
/**
 *		データ
 */
//-----------------------------------------------------------------------------
//　スクリーンエリア
static const WFNOTE_SCRNAREA sc_SCRNAREA[ WFNOTE_DRAWAREA_NUM ] = {
	{ 0, 0, 32, 24 },
	{ 32, 0, 32, 24 },
	{ -32, 0, 32, 24 }
};

// BGコントロールデータ
static const GF_BGL_BGCNT_HEADER sc_WFNOTE_BGCNT[ WFNOTE_BG_NUM ] = {
	{	// WFNOTE_BG_MAIN_BACK
		0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xc000, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
		3, 0, 0, FALSE
	},
	{	// WFNOTE_BG_MAIN_MSG
		0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xc800, GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01,
		0, 0, 0, FALSE
	},
	{	// WFNOTE_BG_MAIN_SCROLL
		0, 0, 0x1000, 0, GF_BGL_SCRSIZ_512x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xd000, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
		2, 0, 0, FALSE
	},
	{	// WFNOTE_BG_MAIN_SCRMSG
		0, 0, 0x1000, 0, GF_BGL_SCRSIZ_512x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x20000, GX_BG_EXTPLTT_01,
		1, 0, 0, FALSE
	},
	{	// WFNOTE_BG_SUB_BACK
		0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xc000, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
		0, 0, 0, FALSE
	},
};

// BGフレームテーブル
static const u32 sc_WFNOTE_BGFRM[ WFNOTE_BG_NUM ] = {
	GF_BGL_FRAME0_M,
	GF_BGL_FRAME1_M,
	GF_BGL_FRAME2_M,
	GF_BGL_FRAME3_M,
	GF_BGL_FRAME0_S,
};

// スクロール用サーフェース
static NNSG2dViewRect s_SCRLLSURFACE_RECT = {
	{ 0, CLACT_MAIN2_REND_Y },
	{ SURFACE_WIDTH, SURFACE_HEIGHT }
};

//-------------------------------------
///	MODESELECT
//=====================================
static const BMPWIN_DAT WFNOTE_MODESELECT_YESNOWIN = {
    WFNOTE_MODESELECT_YESNO_FRM, 
	WFNOTE_MODESELECT_YESNO_X, WFNOTE_MODESELECT_YESNO_Y,
    WFNOTE_MODESELECT_YESNO_SIZX, WFNOTE_MODESELECT_YESNO_SIZY,
	WFNOTE_BGPL_MAIN_MSGFONT, WFNOTE_MODESELECT_YESNOCGX
};

//-------------------------------------
///	FRIENDLIST
//=====================================
static const WFNOTE_FRIELDLIST_CURSORDATA sc_WFNOTE_FRIENDLIST_CURSORDATA[ WFNOTE_FRIENDLIST_CURSORDATA_NUM ] = {
	// プレイヤー0～8
	{	
		{ 1, 4, 15, 4 },
		GF_BGL_FRAME2_M
	},
	{
		{ 1, 8, 15, 4 },
		GF_BGL_FRAME2_M
	},
	{
		{ 1, 12, 15, 4 },
		GF_BGL_FRAME2_M
	},
	{
		{ 1, 16, 15, 4 },
		GF_BGL_FRAME2_M
	},
	{	
		{ 16, 4, 15, 4 },
		GF_BGL_FRAME2_M
	},
	{
		{ 16, 8, 15, 4 },
		GF_BGL_FRAME2_M
	},
	{
		{ 16, 12, 15, 4 },
		GF_BGL_FRAME2_M
	},
	{
		{ 16, 16, 15, 4 },
		GF_BGL_FRAME2_M
	},
	// 戻る
	{
		{ WFNOTE_FRIENDLIST_BACK_X, WFNOTE_FRIENDLIST_BACK_Y, 
			WFNOTE_FRIENDLIST_BACK_SIZX, WFNOTE_FRIENDLIST_BACK_SIZY },
		GF_BGL_FRAME0_M
	},
};

static const u8 sc_POSSCRNY[ WFNOTE_FRIENDLIST_PAGE_NUM ] = {
	WFNOTE_FRIENDLIST_SHIORISCRNPOS0_Y,
	WFNOTE_FRIENDLIST_SHIORISCRNPOS1_Y,
	WFNOTE_FRIENDLIST_SHIORISCRNPOS2_Y,
	WFNOTE_FRIENDLIST_SHIORISCRNPOS3_Y,
};

// メニューデータ
static const WFNOTE_MENU_DATA sc_WFNOTE_FRIENDLIST_MENU[WFNOTE_FRIENDLIST_MENU_NUM][WFNOTE_FRIENDLIST_MENU_LISTNUM] = {
	{
		{ msg_wifi_note_09, (u32)WFNOTE_FRIENDLIST_SEQ_INFOINIT },
		{ msg_wifi_note_10, (u32)WFNOTE_FRINEDLIST_SEQ_NAMECHG_INIT },
		{ msg_wifi_note_11, (u32)WFNOTE_FRIENDLIST_SEQ_DELETE_INIT },
		{ msg_wifi_note_12, (u32)WFNOTE_FRIENDLIST_SEQ_MAIN },
	},
	{
		{ msg_wifi_note_33, (u32)WFNOTE_FRIENDLIST_SEQ_CODE_INIT },
		{ msg_wifi_note_10, (u32)WFNOTE_FRINEDLIST_SEQ_NAMECHG_INIT },
		{ msg_wifi_note_11, (u32)WFNOTE_FRIENDLIST_SEQ_DELETE_INIT },
		{ msg_wifi_note_12, (u32)WFNOTE_FRIENDLIST_SEQ_MAIN },
	}
};
static const BMPLIST_HEADER WFNOTE_FRINEDLIST_MENUHEADER = {
    NULL,			// 表示文字データポインタ
    WFNOTE_FRIENDLIST_MENULISTCallBack,		// カーソル移動ごとのコールバック関数
    NULL,					// 一列表示ごとのコールバック関数
    NULL,                   // 
    WFNOTE_FRIENDLIST_MENU_LISTNUM,	// リスト項目数
    WFNOTE_FRIENDLIST_MENU_LISTNUM,	// 表示最大項目数
    0,						// ラベル表示Ｘ座標
    8,						// 項目表示Ｘ座標
    0,						// カーソル表示Ｘ座標
    0,						// 表示Ｙ座標
    FBMP_COL_BLACK,			// 文字色
    FBMP_COL_WHITE,			// 背景色
    FBMP_COL_BLK_SDW,		// 文字影色
    0,						// 文字間隔Ｘ
    16,						// 文字間隔Ｙ
    BMPLIST_LRKEY_SKIP,		// ページスキップタイプ
    FONT_SYSTEM,				// 文字指定
    0,						// ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)
    NULL                    // work
};

// YESNOウィンドウ
static const BMPWIN_DAT WFNOTE_FRIENDLIST_YESNOWIN = {
    WFNOTE_FRIENDLIST_YESNO_FRM, 
	WFNOTE_FRIENDLIST_YESNO_X, WFNOTE_FRIENDLIST_YESNO_Y,
    WFNOTE_FRIENDLIST_YESNO_SIZX, WFNOTE_FRIENDLIST_YESNO_SIZY,
	WFNOTE_BGPL_MAIN_MSGFONT, WFNOTE_FRIENDLIST_YESNOCGX
};



//-------------------------------------
///	FRIENDINFO
//=====================================
static const WFNOTE_FRIENDINFO_BMPDATA	sc_PAGE00MSG[ WFNOTE_FRIENDINFO_PAGE00_BA_NUM ] = {
	{ 
		WFNOTE_FRIENDINFO_PAGE00_BA_GR_X, WFNOTE_FRIENDINFO_PAGE00_BA_GR_Y,
		WFNOTE_FRIENDINFO_PAGE00_BA_GR_SIZX, WFNOTE_FRIENDINFO_PAGE00_BA_GR_SIZY,
	},
	{ 
		WFNOTE_FRIENDINFO_PAGE00_BA_BT_X, WFNOTE_FRIENDINFO_PAGE00_BA_BT_Y,
		WFNOTE_FRIENDINFO_PAGE00_BA_BT_SIZX, WFNOTE_FRIENDINFO_PAGE00_BA_BT_SIZY,
	},
	{ 
		WFNOTE_FRIENDINFO_PAGE00_BA_TR_X, WFNOTE_FRIENDINFO_PAGE00_BA_TR_Y,
		WFNOTE_FRIENDINFO_PAGE00_BA_TR_SIZX, WFNOTE_FRIENDINFO_PAGE00_BA_TR_SIZY,
	},
	{ 
		WFNOTE_FRIENDINFO_PAGE00_BA_DAY_X, WFNOTE_FRIENDINFO_PAGE00_BA_DAY_Y,
		WFNOTE_FRIENDINFO_PAGE00_BA_DAY_SIZX, WFNOTE_FRIENDINFO_PAGE00_BA_DAY_SIZY,
	},
	{ 
		WFNOTE_FRIENDINFO_PAGE00_BA_TRG_X, WFNOTE_FRIENDINFO_PAGE00_BA_TRG_Y,
		WFNOTE_FRIENDINFO_PAGE00_BA_TRG_SIZX, WFNOTE_FRIENDINFO_PAGE00_BA_TRG_SIZY,
	},
};
static const WFNOTE_FRIENDINFO_BMPDATA	sc_PAGE01MSG[ WFNOTE_FRIENDINFO_PAGE01_BA_NUM ] = {
	{ 
		WFNOTE_FRIENDINFO_PAGE01_BA_X, WFNOTE_FRIENDINFO_PAGE01_BA_Y,
		WFNOTE_FRIENDINFO_PAGE01_BA_SIZX, WFNOTE_FRIENDINFO_PAGE01_BA_SIZY,
	},
};
static const WFNOTE_FRIENDINFO_BMPDATA	sc_PAGE02MSG[ WFNOTE_FRIENDINFO_PAGE02_BA_NUM ] = {
	{ 
		WFNOTE_FRIENDINFO_PAGE02_BA_TT_X, WFNOTE_FRIENDINFO_PAGE02_BA_TT_Y,
		WFNOTE_FRIENDINFO_PAGE02_BA_TT_SIZX, WFNOTE_FRIENDINFO_PAGE02_BA_TT_SIZY,
	},
	{ 
		WFNOTE_FRIENDINFO_PAGE02_BA_LV50TT_X, WFNOTE_FRIENDINFO_PAGE02_BA_LV50TT_Y,
		WFNOTE_FRIENDINFO_PAGE02_BA_LV50TT_SIZX, WFNOTE_FRIENDINFO_PAGE02_BA_LV50TT_SIZY,
	},
	{ 
		WFNOTE_FRIENDINFO_PAGE02_BA_LV50NO_X, WFNOTE_FRIENDINFO_PAGE02_BA_LV50NO_Y,
		WFNOTE_FRIENDINFO_PAGE02_BA_LV50NO_SIZX, WFNOTE_FRIENDINFO_PAGE02_BA_LV50NO_SIZY,
	},
	{ 
		WFNOTE_FRIENDINFO_PAGE02_BA_LV100TT_X, WFNOTE_FRIENDINFO_PAGE02_BA_LV100TT_Y,
		WFNOTE_FRIENDINFO_PAGE02_BA_LV100TT_SIZX, WFNOTE_FRIENDINFO_PAGE02_BA_LV100TT_SIZY,
	},
	{ 
		WFNOTE_FRIENDINFO_PAGE02_BA_LV100NO_X, WFNOTE_FRIENDINFO_PAGE02_BA_LV100NO_Y,
		WFNOTE_FRIENDINFO_PAGE02_BA_LV100NO_SIZX, WFNOTE_FRIENDINFO_PAGE02_BA_LV100NO_SIZY,
	},
};
static const WFNOTE_FRIENDINFO_BMPDATA	sc_PAGE03MSG[ WFNOTE_FRIENDINFO_PAGE03_BA_NUM ] = {
	{ 
		WFNOTE_FRIENDINFO_PAGE03_BA_X, WFNOTE_FRIENDINFO_PAGE03_BA_Y,
		WFNOTE_FRIENDINFO_PAGE03_BA_SIZX, WFNOTE_FRIENDINFO_PAGE03_BA_SIZY,
	},
};
static const WFNOTE_FRIENDINFO_BMPDATA	sc_PAGE04MSG[ WFNOTE_FRIENDINFO_PAGE04_BA_NUM ] = {
	{ 
		WFNOTE_FRIENDINFO_PAGE04_BA_X, WFNOTE_FRIENDINFO_PAGE04_BA_Y,
		WFNOTE_FRIENDINFO_PAGE04_BA_SIZX, WFNOTE_FRIENDINFO_PAGE04_BA_SIZY,
	},
};
static const WFNOTE_FRIENDINFO_BMPDATA	sc_PAGE05MSG[ WFNOTE_FRIENDINFO_PAGE05_BA_NUM ] = {
	{ 
		WFNOTE_FRIENDINFO_PAGE05_BA_X, WFNOTE_FRIENDINFO_PAGE05_BA_Y,
		WFNOTE_FRIENDINFO_PAGE05_BA_SIZX, WFNOTE_FRIENDINFO_PAGE05_BA_SIZY,
	},
};
static const WFNOTE_FRIENDINFO_BMPDATA	sc_PAGE06MSG[ WFNOTE_FRIENDINFO_PAGE06_BA_NUM ] = {
	{ 
		WFNOTE_FRIENDINFO_PAGE06_BA_X, WFNOTE_FRIENDINFO_PAGE06_BA_Y,
		WFNOTE_FRIENDINFO_PAGE06_BA_SIZX, WFNOTE_FRIENDINFO_PAGE06_BA_SIZY,
	},
};

// すべてまとめたテーブル
static const WFNOTE_FRIENDINFO_BMPDATA* scp_WFNOTE_FRIENDINFO_PAGEBMP[ WFNOTE_FRIENDINFO_PAGE_NUM ] = {
	sc_PAGE00MSG, sc_PAGE06MSG, sc_PAGE01MSG, sc_PAGE02MSG,
	sc_PAGE03MSG, sc_PAGE04MSG, sc_PAGE05MSG,
	
};
static const u8 sc_WFNOTE_FRIENDINFO_PAGEBMPNUM[ WFNOTE_FRIENDINFO_PAGE_NUM ] = {
	WFNOTE_FRIENDINFO_PAGE00_BA_NUM,
	WFNOTE_FRIENDINFO_PAGE06_BA_NUM,
	WFNOTE_FRIENDINFO_PAGE01_BA_NUM,
	WFNOTE_FRIENDINFO_PAGE02_BA_NUM,
	WFNOTE_FRIENDINFO_PAGE03_BA_NUM,
	WFNOTE_FRIENDINFO_PAGE04_BA_NUM,
	WFNOTE_FRIENDINFO_PAGE05_BA_NUM,
};
 






//----------------------------------------------------------------------------
/**
 *	@brief	プロセス　初期化
 *
 *	@param	proc		プロセスワーク
 *	@param	seq			シーケンス
 *
 *	@retval PROC_RES_CONTINUE = 0,		///<動作継続中
 *	@retval	PROC_RES_FINISH,			///<動作終了
 */
//-----------------------------------------------------------------------------
PROC_RESULT WifiNoteProc_Init( PROC * proc, int * seq )
{
	WFNOTE_WK* p_wk;

	// wifi_2dcharを使用するためにOVERLAYをLOAD
	Overlay_Load( FS_OVERLAY_ID(wifi_2dmapsys), OVERLAY_LOAD_NOT_SYNCHRONIZE);
	
	// ワーク作成
	sys_CreateHeap( HEAPID_BASE_APP, HEAPID_WIFINOTE, 0x70000 );
	p_wk = PROC_AllocWork( proc, sizeof(WFNOTE_WK), HEAPID_WIFINOTE );
	memset( p_wk, 0, sizeof(WFNOTE_WK) );

	// データワーク初期
	WFNOTE_DATA_Init( &p_wk->data, PROC_GetParentWork(proc), HEAPID_WIFINOTE );

	// 描画システムワーク初期化
	WFNOTE_DRAW_Init( &p_wk->draw, &p_wk->data, HEAPID_WIFINOTE );

	// 各画面のワーク初期化
	WFNOTE_MODESELECT_Init( &p_wk->mode_select, &p_wk->data, &p_wk->draw, HEAPID_WIFINOTE );
	WFNOTE_FRIENDLIST_Init( &p_wk->friendlist, &p_wk->data, &p_wk->draw, HEAPID_WIFINOTE );
	WFNOTE_CODEIN_Init( &p_wk->codein, &p_wk->data, &p_wk->draw, HEAPID_WIFINOTE );
	WFNOTE_MYCODE_Init( &p_wk->mycode, &p_wk->data, &p_wk->draw, HEAPID_WIFINOTE );
	WFNOTE_FRIENDINFO_Init( &p_wk->friendinfo, &p_wk->data, &p_wk->draw, HEAPID_WIFINOTE );

	// 割り込み
    sys_VBlankFuncChange( WFNOTE_VBlank, p_wk );
	sys_HBlankIntrStop();

	return PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	プロセス　メイン処理
 *
 *	@param	proc		プロセスワーク
 *	@param	seq			シーケンス
 *
 *	@retval PROC_RES_CONTINUE = 0,		///<動作継続中
 *	@retval	PROC_RES_FINISH,			///<動作終了
 */
//-----------------------------------------------------------------------------
PROC_RESULT WifiNoteProc_Main( PROC * proc, int * seq )
{
	WFNOTE_WK* p_wk  = PROC_GetWork( proc );
	WFNOTE_STRET stret;

	switch( *seq ){
	case WFNOTE_PROCSEQ_MAIN:
		
		switch( p_wk->status ){
		case WFNOTE_STATUS_MODE_SELECT:	// 最初の選択画面
			stret = WFNOTE_MODESELECT_Main( &p_wk->mode_select, &p_wk->data, &p_wk->draw, HEAPID_WIFINOTE );
			break;
			
		case WFNOTE_STATUS_FRIENDLIST:	// 友達データ表示
			stret = WFNOTE_FRIENDLIST_Main( &p_wk->friendlist, &p_wk->data, &p_wk->draw, HEAPID_WIFINOTE );
			break;
			
		case WFNOTE_STATUS_CODEIN:		// 友達コード入力
			stret = WFNOTE_CODEIN_Main( &p_wk->codein, p_wk, &p_wk->data, &p_wk->draw, HEAPID_WIFINOTE );
			break;
			
		case WFNOTE_STATUS_MYCODE:		// 自分の友達コード確認
			stret = WFNOTE_MYCODE_Main( &p_wk->mycode, &p_wk->data, &p_wk->draw );
			break;

		case WFNOTE_STATUS_FRIENDINFO:	// 友達詳細データ
			stret = WFNOTE_FRIENDINFO_Main( &p_wk->friendinfo, &p_wk->data, &p_wk->draw,  HEAPID_WIFINOTE );
			break;
			
		case WFNOTE_STATUS_END:			// 終了
			return PROC_RES_FINISH;
		}

		// 状態切り替え処理
		if( stret == WFNOTE_STRET_FINISH ){
			p_wk->status = p_wk->data.reqstatus;
			p_wk->data.subseq = p_wk->data.reqsubseq;
			p_wk->wait = WFNOTE_STATUS_CHANGE_WAIT;
			(*seq) = WFNOTE_PROCSEQ_WAIT;
		}
		break;

	case WFNOTE_PROCSEQ_WAIT:
		p_wk->wait --;
		if( p_wk->wait <= 0 ){
			(*seq) = WFNOTE_PROCSEQ_MAIN;
		}
		
		break;
	}


	// 描画メイン
	WFNOTE_DRAW_Main( &p_wk->draw );

	return PROC_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	プロセス　破棄
 *
 *	@param	proc		プロセスワーク
 *	@param	seq			シーケンス
 *
 *	@retval PROC_RES_CONTINUE = 0,		///<動作継続中
 *	@retval	PROC_RES_FINISH,	GF_BGL_BmpWinInit		///<動作終了
 */
//-----------------------------------------------------------------------------
PROC_RESULT WifiNoteProc_End( PROC * proc, int * seq )
{
	WFNOTE_WK* p_wk  = PROC_GetWork( proc );

	// 割り込み
    sys_VBlankFuncChange( NULL, NULL );
	sys_HBlankIntrStop();

	// 各画面のワーク破棄
	WFNOTE_MODESELECT_Exit( &p_wk->mode_select, &p_wk->data, &p_wk->draw );
	WFNOTE_FRIENDLIST_Exit( &p_wk->friendlist, &p_wk->data, &p_wk->draw );
	WFNOTE_CODEIN_Exit( &p_wk->codein, &p_wk->data, &p_wk->draw );
	WFNOTE_MYCODE_Exit( &p_wk->mycode, &p_wk->data, &p_wk->draw );
	WFNOTE_FRIENDINFO_Exit( &p_wk->friendinfo, &p_wk->data, &p_wk->draw );

	// 描画システムワーク破棄
	WFNOTE_DRAW_Exit( &p_wk->draw );

	// データワーク破棄
	WFNOTE_DATA_Exit( &p_wk->data );

	PROC_FreeWork( proc );				// PROCワーク開放
	sys_DeleteHeap( HEAPID_WIFINOTE );

	// オーバーレイ破棄
	Overlay_UnloadID( FS_OVERLAY_ID(wifi_2dmapsys) );

	return PROC_RES_FINISH;
}




//-----------------------------------------------------------------------------
/**
 *		プライベート関数
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	全体関数
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	VBlank関数
 *
 *	@param	p_work	システムワーク
 */
//-----------------------------------------------------------------------------
static void WFNOTE_VBlank( void* p_work )
{
	WFNOTE_WK* p_wk = p_work;

	// 描画VBLANK
	WFNOTE_DRAW_VBlank( &p_wk->draw );
}

//----------------------------------------------------------------------------
/**
 *	@brief	WFNOTE　初期化
 *
 *	@param	p_wk		ワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DrawInit( WFNOTE_WK* p_wk, u32 heapID )
{
	// 描画システムワーク初期化
	WFNOTE_DRAW_Init( &p_wk->draw, &p_wk->data, heapID );

	// 各画面のワーク初期化
	WFNOTE_MODESELECT_DrawInit( &p_wk->mode_select, &p_wk->data, &p_wk->draw, heapID );
	WFNOTE_FRIENDLIST_DrawInit( &p_wk->friendlist, &p_wk->data, &p_wk->draw, heapID );
	WFNOTE_CODEIN_DrawInit( &p_wk->codein, &p_wk->data, &p_wk->draw, heapID );
	WFNOTE_MYCODE_DrawInit( &p_wk->mycode, &p_wk->data, &p_wk->draw, heapID );
	WFNOTE_FRIENDINFO_DrawInit( &p_wk->friendinfo, &p_wk->data, &p_wk->draw, heapID );

	// 割り込み
    sys_VBlankFuncChange( WFNOTE_VBlank, p_wk );
	sys_HBlankIntrStop();

#if PL_T0861_080712_FIX
	MsgPrintSkipFlagSet(MSG_SKIP_ON);
	MsgPrintAutoFlagSet(MSG_AUTO_OFF);
	MsgPrintTouchPanelFlagSet(MSG_TP_OFF);
#endif

}

//----------------------------------------------------------------------------
/**
 *	@brief	WFNOTE	破棄
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DrawExit( WFNOTE_WK* p_wk )
{
	// 割り込み
    sys_VBlankFuncChange( NULL, NULL );
	sys_HBlankIntrStop();

	// 各画面のワーク破棄
	WFNOTE_MODESELECT_DrawExit( &p_wk->mode_select, &p_wk->data, &p_wk->draw );
	WFNOTE_FRIENDLIST_DrawExit( &p_wk->friendlist, &p_wk->data, &p_wk->draw );
	WFNOTE_CODEIN_DrawExit( &p_wk->codein, &p_wk->data, &p_wk->draw );
	WFNOTE_MYCODE_DrawExit( &p_wk->mycode, &p_wk->data, &p_wk->draw );
	WFNOTE_FRIENDINFO_DrawExit( &p_wk->friendinfo, &p_wk->data, &p_wk->draw );

	// 描画システムワーク破棄
	WFNOTE_DRAW_Exit( &p_wk->draw );
}


//-------------------------------------
///	データワーク関数
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	データワーク初期化
 *
 *	@param	p_data		データワーク
 *	@param	p_save		セーブデータ
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DATA_Init( WFNOTE_DATA* p_data, void* p_save, u32 heapID )
{
	p_data->p_save = p_save;
	p_data->codein.p_name = STRBUF_Create( WFNOTE_STRBUF_SIZE, heapID );
	p_data->codein.p_code = STRBUF_Create( WFNOTE_STRBUF_SIZE, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	データワーク破棄
 *
 *	@param	p_data	データワーク
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DATA_Exit( WFNOTE_DATA* p_data )
{
	STRBUF_Delete( p_data->codein.p_name );
	STRBUF_Delete( p_data->codein.p_code );
}


//----------------------------------------------------------------------------
/**
 *	@brief	リクエストステータス設定
 *
 *	@param	p_data		ワーク
 *	@param	reqstatus	リクエストステータス
 *	@param	reqsubseq	リクエストサブシーケンス
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DATA_SetReqStatus( WFNOTE_DATA* p_data, u8 reqstatus, u8 reqsubseq )
{
	p_data->reqstatus = reqstatus;
	p_data->reqsubseq = reqsubseq;
}

//----------------------------------------------------------------------------
/**
 *	@brief	データにコード入力画面の結果を設定する
 *
 *	@param	p_data		ワーク
 *	@param	cp_name		名前
 *	@param	cp_code		コード
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DATA_CODEIN_Set( WFNOTE_DATA* p_data, const STRBUF* cp_name, const STRBUF* cp_code )
{
	if( cp_name ){
		STRBUF_Copy( p_data->codein.p_name, cp_name );
	}
	if( cp_code ){
		STRBUF_Copy( p_data->codein.p_code, cp_code );
	}
}	

//----------------------------------------------------------------------------
/**
 *	@brief	最新の友達データ設定
 *
 *	@param	p_data		ワーク
 *	@param	p_code		コード文字列
 *	@param	p_name		名前文字列
 */
//-----------------------------------------------------------------------------
static u32 WFNOTE_DATA_NewFriendDataSet( WFNOTE_DATA* p_data, STRBUF* p_code, STRBUF* p_name )
{
	int i;
	WIFI_LIST* p_list;
	BOOL result;
	BOOL set_check;
	u64 friendcode;
	u64 mycode;
    DWCFriendData *p_dwc_list = NULL;
    DWCUserData *p_dwc_userdata = NULL;
    int ret;
	int pos;

	p_list = SaveData_GetWifiListData( p_data->p_save );

	mycode = DWC_CreateFriendKey( WifiList_GetMyUserInfo(p_list) ); 

	for( i=0; i<WIFILIST_FRIEND_MAX; i++ ){
		result = WifiList_IsFriendData( p_list, i );
		if( result == FALSE ){
			friendcode = STRBUF_GetNumber(p_code, &set_check);	

			if( (set_check == TRUE) && (friendcode !=mycode) ){
				
				p_dwc_userdata = WifiList_GetMyUserInfo( p_list );
				if(!DWC_CheckFriendKey(p_dwc_userdata, friendcode)){
					// 友達コードが違う
					return WFNOTE_DATA_NEWFRIENDSET_FRIENDKEYNG;
				}
				ret = dwc_checkFriendCodeByToken(p_data->p_save, friendcode, &pos);
				if(ret == DWCFRIEND_INLIST){	
					// もう設定ずみ
					return WFNOTE_DATA_NEWFRIENDSET_SETING;
				}
				// 成功
				p_dwc_list = WifiList_GetDwcDataPtr(p_list, i);
				DWC_CreateFriendKeyToken(p_dwc_list, friendcode);
				// 名前と性別設定	中性で設定
				WifiList_SetFriendName(p_list, i, p_name);
				WifiList_SetFriendInfo(p_list, i, WIFILIST_FRIEND_SEX, PM_NEUTRAL);
				return WFNOTE_DATA_NEWFRIENDSET_OK;
			}else{
				// 友達コードが違う
				return WFNOTE_DATA_NEWFRIENDSET_FRIENDKEYNG;
			}
		}
	}

	GF_ASSERT(0);
	// ここはとおらないはず
	return WFNOTE_DATA_NEWFRIENDSET_OK;
}

//----------------------------------------------------------------------------
/**
 *	@brief	名前入力	初期化データ設定
 *
 *	@param	p_data		データ
 *	@param	cp_str		文字列
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DATA_NAMEIN_INIT_Set( WFNOTE_DATA* p_data, const STRCODE* cp_str )
{
	p_data->namein_init.cp_str = cp_str;
	p_data->namein_init.init = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	選択したリストインデックスを設定する
 *
 *	@param	p_data		データ
 *	@param	idx			インデックス
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DATA_SELECT_ListIdxSet( WFNOTE_DATA* p_data, u32 idx )
{
	p_data->select_listidx = idx;
}

//----------------------------------------------------------------------------
/**
 *	@brief	友達リスト作成
 *		
 *	@param	p_data		データ
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DATA_FrIdxMake( WFNOTE_DATA* p_data )
{
	int i;
	WIFI_LIST* p_list;

	memset( &p_data->idx, 0, sizeof(WFNOTE_IDXDATA) );

	p_list = SaveData_GetWifiListData( p_data->p_save );

	p_data->idx.friendnum = 0;
	for( i=0; i<WFNOTE_FRIENDLIST_FRIENDMAX; i++ ){
		if( WifiList_IsFriendData( p_list, i ) == TRUE ){
			p_data->idx.fridx[ p_data->idx.friendnum ] = i;
			p_data->idx.friendnum ++;
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	WIFIリストテーブルから友達番号を取得する
 *
 *	@param	cp_data		データ
 *	@param	idx			リストインデックス
 *
 *	@return	友達番号
 */
//-----------------------------------------------------------------------------
static u32 WFNOTE_DATA_FrIdxGetIdx( const WFNOTE_DATA* cp_data, u32 idx )
{
	return cp_data->idx.fridx[ idx ];
}



//-------------------------------------
///	描画ワーク関数
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	描画ワーク初期化
 *
 *	@param	p_draw		ワーク
 *	@param	cp_data		データ
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DRAW_Init( WFNOTE_DRAW* p_draw, const WFNOTE_DATA* cp_data, u32 heapID )
{
	// まず行うもの
	sys.disp3DSW = DISP_3D_TO_MAIN;
	GF_Disp_DispSelect();
	p_draw->p_handle = ArchiveDataHandleOpen( ARC_WIFINOTE_GRA, heapID );

	WFNOTE_DRAW_BankInit();
	WFNOTE_DRAW_BgInit( p_draw, heapID );
	WFNOTE_DRAW_ClactInit( p_draw, heapID );
	WFNOTE_DRAW_MsgInit( p_draw, heapID );
	WFNOTE_DRAW_BmpInit( p_draw, cp_data, heapID );
	WFNOTE_DRAW_YazirushiInit( p_draw, heapID );
	WFNOTE_DRAW_SCRNDATA_Init( p_draw, heapID );

	// OAM面表示
    GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

    // MatchComment: new change in plat US
    MsgPrintSkipFlagSet(MSG_SKIP_ON);
    MsgPrintAutoFlagSet(MSG_AUTO_OFF);
    MsgPrintTouchPanelFlagSet(MSG_TP_OFF);
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画ワークメイン
 *
 *	@param	p_draw		ワーク
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DRAW_Main( WFNOTE_DRAW* p_draw )
{
	CLACT_Draw( p_draw->p_clact );
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画ワークVブランク
 *
 *	@param	p_draw		ワーク
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DRAW_VBlank( WFNOTE_DRAW* p_draw )
{
	GF_BGL_VBlankFunc( p_draw->p_bgl );
	DoVramTransferManager();
	REND_OAMTrans();
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画ワーク　破棄
 *
 *	@param	p_draw	ワーク
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DRAW_Exit( WFNOTE_DRAW* p_draw )
{
	WFNOTE_DRAW_YazirushiExit( p_draw );
	WFNOTE_DRAW_SCRNDATA_Exit( p_draw );
	WFNOTE_DRAW_BmpExit( p_draw );
	WFNOTE_DRAW_MsgExit( p_draw );
	WFNOTE_DRAW_ClactExit( p_draw );
	WFNOTE_DRAW_BgExit( p_draw );

	ArchiveDataHandleClose( p_draw->p_handle );
}

//----------------------------------------------------------------------------
/**
 *	@brief	バンク設定
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DRAW_BankInit( void )
{
    GF_BGL_DISPVRAM tbl = {
		GX_VRAM_BG_256_AB,				// メイン2DエンジンのBG
		GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット

		GX_VRAM_SUB_BG_128_C,			// サブ2DエンジンのBG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット

		GX_VRAM_OBJ_80_EF,				// メイン2DエンジンのOBJ
		GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット

		GX_VRAM_SUB_OBJ_16_I,			// サブ2DエンジンのOBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット

		GX_VRAM_TEX_NONE,				// テクスチャイメージスロット
		GX_VRAM_TEXPLTT_NONE			// テクスチャパレットスロット
	};
	GF_Disp_SetBank( &tbl );
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG初期化
 *
 *	@param	p_draw		ワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DRAW_BgInit( WFNOTE_DRAW* p_draw, u32 heapID )
{
	int i;
	
	p_draw->p_bgl = GF_BGL_BglIniAlloc( heapID );

    // BG SYSTEM
	{	
		GF_BGL_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
		};
		GF_BGL_InitBG( &BGsys_data );
	}

	// BGコントロール設定
	for( i=0; i<WFNOTE_BG_NUM; i++ ){
		GF_BGL_BGControlSet( p_draw->p_bgl, sc_WFNOTE_BGFRM[i], &sc_WFNOTE_BGCNT[i], GF_BGL_MODE_TEXT );
		GF_BGL_ClearCharSet( sc_WFNOTE_BGFRM[i], 32, 0, heapID );
		GF_BGL_ScrClear( p_draw->p_bgl, sc_WFNOTE_BGFRM[i] );
	}

	// 基本的なバックグラウンド読込み＆転送
	// キャラクタデータは、スクロール面にも転送
	ArcUtil_HDL_PalSet( p_draw->p_handle, NARC_pl_wifinote_techo_NCLR, PALTYPE_MAIN_BG,
			WFNOTE_BGPL_MAIN_BACK_0, WFNOTE_BGPL_MAIN_BACK_NUM*32, heapID );
	ArcUtil_HDL_PalSet( p_draw->p_handle, NARC_pl_wifinote_techo_NCLR, PALTYPE_SUB_BG,
			WFNOTE_BGPL_SUB_BACK_0, WFNOTE_BGPL_SUB_BACK_NUM*32, heapID );
	
	ArcUtil_HDL_BgCharSet( p_draw->p_handle, NARC_pl_wifinote_techo_NCGR, 
			p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_BACK ], 0, 0, FALSE, heapID );
	ArcUtil_HDL_BgCharSet( p_draw->p_handle, NARC_pl_wifinote_techo_NCGR, 
			p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_SCROLL ], 0, 0, FALSE, heapID );
	ArcUtil_HDL_BgCharSet( p_draw->p_handle, NARC_pl_wifinote_techo_sub_NCGR, 
			p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_SUB_BACK ], 0, 0, FALSE, heapID );
	
	ArcUtil_HDL_ScrnSet( p_draw->p_handle, NARC_pl_wifinote_techo_main_NSCR,
			p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_BACK ], 0, 0, FALSE, heapID );
	ArcUtil_HDL_ScrnSet( p_draw->p_handle, NARC_pl_wifinote_techo_sub_NSCR,
			p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_SUB_BACK ], 0, 0, FALSE, heapID );


}

//----------------------------------------------------------------------------
/**
 *	@brief	BG破棄
 *
 *	@param	p_draw		ワーク 
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DRAW_BgExit( WFNOTE_DRAW* p_draw )
{
	int i;
	
	// BGコントロール破棄
	for( i=0; i<WFNOTE_BG_NUM; i++ ){
		GF_BGL_BGControlExit( p_draw->p_bgl, sc_WFNOTE_BGFRM[i] );
	}

	sys_FreeMemoryEz( p_draw->p_bgl );
	p_draw->p_bgl = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	セルアクター初期化
 *
 *	@param	p_draw		ワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DRAW_ClactInit( WFNOTE_DRAW* p_draw, u32 heapID )
{
	int i;
	BOOL result;
	
	// OAMマネージャーの初期化
	NNS_G2dInitOamManagerModule();

	// Vram転送マネージャ初期化
	initVramTransferManagerHeap( VRAMTR_MAN_NUM, heapID );

	// 共有OAMマネージャ作成
	// レンダラ用OAMマネージャ作成
	// ここで作成したOAMマネージャをみんなで共有する
	REND_OAMInit( 
			0, 126,		// メイン画面OAM管理領域
			0, 30,		// メイン画面アフィン管理領域
			0, 126,		// サブ画面OAM管理領域
			0, 30,		// サブ画面アフィン管理領域
			heapID );
	
	// キャラクタマネージャー初期化
	{
		CHAR_MANAGER_MAKE cm = {
			CLACT_RESNUM,
			CLACT_MAIN_VRAM_SIZ,
			CLACT_SUB_VRAM_SIZ,
			0
		};
		cm.heap = heapID;
		InitCharManagerReg(&cm, GX_OBJVRAMMODE_CHAR_1D_128K, GX_OBJVRAMMODE_CHAR_1D_128K );
	}
	// パレットマネージャー初期化
	InitPlttManager(CLACT_RESNUM, heapID);

	// 読み込み開始位置を初期化
	CharLoadStartAll();
	PlttLoadStartAll();
	
	
	// セルアクター初期化
	p_draw->p_clact = CLACT_U_SetEasyInit( CLACT_WKNUM, &p_draw->renddata, heapID );
	
	// スクロール用サーフェースの作成
	REND_OAM_SetSurface( &p_draw->scrll_surface, &s_SCRLLSURFACE_RECT,
			NNS_G2D_SURFACETYPE_MAIN2D, &p_draw->renddata.Rend );
	
	//リソースマネージャー初期化
	for(i=0;i<CLACT_RESOURCE_NUM;i++){		//リソースマネージャー作成
		p_draw->p_resman[i] = CLACT_U_ResManagerInit(CLACT_RESNUM, i, heapID);	
	}


	// 基本的なリソースを読み込み
	p_draw->p_resobj[0] = CLACT_U_ResManagerResAddArcChar_ArcHandle( 
			p_draw->p_resman[0], p_draw->p_handle, NARC_pl_wifinote_techo_obj_NCGR,
			FALSE, WFNOTE_OAM_COMM_CONTID, NNS_G2D_VRAM_TYPE_2DMAIN, heapID );
	p_draw->p_resobj[1] = CLACT_U_ResManagerResAddArcPltt_ArcHandle( 
			p_draw->p_resman[1], p_draw->p_handle, NARC_pl_wifinote_techo_NCLR,
			FALSE, WFNOTE_OAM_COMM_CONTID, NNS_G2D_VRAM_TYPE_2DMAIN, 5, heapID );
	p_draw->p_resobj[2] = CLACT_U_ResManagerResAddArcKindCell_ArcHandle( 
			p_draw->p_resman[2], p_draw->p_handle, NARC_pl_wifinote_techo_NCER,
			FALSE, WFNOTE_OAM_COMM_CONTID, CLACT_U_CELL_RES, heapID );
	p_draw->p_resobj[3] = CLACT_U_ResManagerResAddArcKindCell_ArcHandle( 
			p_draw->p_resman[3], p_draw->p_handle, NARC_pl_wifinote_techo_NANR,
			FALSE, WFNOTE_OAM_COMM_CONTID, CLACT_U_CELLANM_RES, heapID );

	// VRAM転送
	result = CLACT_U_CharManagerSetAreaCont( p_draw->p_resobj[0] );	
	GF_ASSERT( result );
	result = CLACT_U_PlttManagerSetCleanArea( p_draw->p_resobj[1] );	
	GF_ASSERT( result );
	CLACT_U_ResManagerResOnlyDelete( p_draw->p_resobj[0] );
	CLACT_U_ResManagerResOnlyDelete( p_draw->p_resobj[1] );

	// ヘッダー作成
	CLACT_U_MakeHeader( &p_draw->clheader, WFNOTE_OAM_COMM_CONTID, WFNOTE_OAM_COMM_CONTID, 
			WFNOTE_OAM_COMM_CONTID, WFNOTE_OAM_COMM_CONTID, CLACT_U_HEADER_DATA_NONE, 
			CLACT_U_HEADER_DATA_NONE, FALSE, 0,
			p_draw->p_resman[0], p_draw->p_resman[1], p_draw->p_resman[2], p_draw->p_resman[3],
			NULL, NULL );
}

//----------------------------------------------------------------------------
/**
 *	@brief	セルアクター破棄
 *
 *	@param	p_draw	ワーク
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DRAW_ClactExit( WFNOTE_DRAW* p_draw )
{
	int i;

	// 基本OAMリソース破棄
	CLACT_U_CharManagerDelete( p_draw->p_resobj[0] );
	CLACT_U_PlttManagerDelete( p_draw->p_resobj[1] );
	for( i=0; i<CLACT_RESOURCE_NUM; i++ ){
		CLACT_U_ResManagerResDelete( p_draw->p_resman[i], p_draw->p_resobj[i] );
	}
	
	// リソースマネージャ破棄
	for(i=0;i<CLACT_RESOURCE_NUM;i++){		//リソースマネージャー作成
		CLACT_U_ResManagerDelete( p_draw->p_resman[i] );	
	}
	
	// セルアクターセット破棄
	CLACT_DestSet( p_draw->p_clact );

	// キャラクタマネージャ破棄
	DeleteCharManager();
	
	// パレットマネージャ破棄
	DeletePlttManager();
	
	// レンダラー破棄
	REND_OAM_Delete();
	
	// Vram転送マネージャ破棄
	DellVramTransferManager();
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージ　初期化
 *
 *	@param	p_draw		ワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DRAW_MsgInit( WFNOTE_DRAW* p_draw, u32 heapID )
{
    p_draw->p_wordset = WORDSET_Create( heapID );
    p_draw->p_msgman = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_wifi_note_dat, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージ　破棄
 *
 *	@param	p_draw		ワーク
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DRAW_MsgExit( WFNOTE_DRAW* p_draw )
{
	MSGMAN_Delete( p_draw->p_msgman );
	WORDSET_Delete( p_draw->p_wordset );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ビットマップ関連初期化
 *
 *	@param	p_draw
 *	@param	cp_data
 *	@param	heapID 
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DRAW_BmpInit( WFNOTE_DRAW* p_draw, const WFNOTE_DATA* cp_data, u32 heapID )
{
	int type;

	// 基本ウィンドウデータ読込み
	type = CONFIG_GetWindowType(SaveData_GetConfig(cp_data->p_save));
	TalkFontPaletteLoad( PALTYPE_MAIN_BG, WFNOTE_BGPL_MAIN_MSGFONT*0x20, heapID );
	TalkWinGraphicSet(
		p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_MSG], WFNOTE_BGCGX_MAIN_MSG_TALK,
		WFNOTE_BGPL_MAIN_TALKWIN,  type, heapID );
	MenuWinGraphicSet(
		p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_MSG], WFNOTE_BGCGX_MAIN_MSG_MENU,
		WFNOTE_BGPL_MAIN_MENUWIN, 0, heapID );
	
	// 基本ビットマップ
	GF_BGL_BmpWinInit( &p_draw->title );
	GF_BGL_BmpWinAdd( p_draw->p_bgl, &p_draw->title, 
			sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_MSG], 
			WFNOTE_BG_TITLEBMP_X, WFNOTE_BG_TITLEBMP_Y, 
			WFNOTE_BG_TITLEBMP_SIZX, WFNOTE_BG_TITLEBMP_SIZY,
			WFNOTE_BGPL_MAIN_MSGFONT, WFNOTE_BG_TITLEBMP_CGX );
	
	// タイトル用文字列バッファ作成
	p_draw->p_titlestr = STRBUF_Create( WFNOTE_STRBUF_SIZE, heapID );
	p_draw->p_titletmp = STRBUF_Create( WFNOTE_STRBUF_SIZE, heapID );
}

//----------------------------------------------------------------------------
/**	
 *	@brief	ビットマップ破棄
 *
 *	@param	p_draw	ワーク
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DRAW_BmpExit( WFNOTE_DRAW* p_draw )
{
	STRBUF_Delete( p_draw->p_titlestr );
	STRBUF_Delete( p_draw->p_titletmp );
	GF_BGL_BmpWinDel( &p_draw->title );
}

//----------------------------------------------------------------------------
/**
 *	@brief	基本スクリーンデータ	初期化
 *
 *	@param	p_draw		描画システム
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DRAW_SCRNDATA_Init( WFNOTE_DRAW* p_draw, u32 heapID )
{
	static const u8 sc_SCRNDATA[ WFNOTE_SCRNDATA_NUM ] = {
		NARC_pl_wifinote_techo_02_NSCR,
		NARC_pl_wifinote_techo_02a_NSCR,
	};
	int i;
	// スクリーンデータ初期化
	for( i=0; i<WFNOTE_SCRNDATA_NUM; i++ ){	
		p_draw->scrn.p_scrnbuff[i] = ArcUtil_HDL_ScrnDataGet( p_draw->p_handle, 
				sc_SCRNDATA[i], FALSE, &p_draw->scrn.p_scrn[i], heapID );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	スクリーンデータ破棄
 *
 *	@param	p_draw	描画システム
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DRAW_SCRNDATA_Exit( WFNOTE_DRAW* p_draw )
{
	int i;

	// スクリーンデータ初期化
	for( i=0; i<WFNOTE_SCRNDATA_NUM; i++ ){	
		sys_FreeMemoryEz( p_draw->scrn.p_scrnbuff[i] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイトルの描画
 *
 *	@param	p_draw	描画ワーク
 *	@param	msg_idx	メッセージID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DRAW_BmpTitleWrite( WFNOTE_DRAW* p_draw, u32 msg_idx )
{
	// タスク動作停止
	if( GF_MSG_PrintEndCheck( p_draw->title_stridx ) == 1 ){
		GF_STR_PrintForceStop( p_draw->title_stridx );
	}
	
	// クリア
	GF_BGL_BmpWinDataFill( &p_draw->title, 0 );

	MSGMAN_GetString( p_draw->p_msgman, msg_idx, p_draw->p_titletmp );
	WORDSET_ExpandStr( p_draw->p_wordset, p_draw->p_titlestr, p_draw->p_titletmp );
    p_draw->title_stridx = GF_STR_PrintColor(&p_draw->title, FONT_TALK, 
			p_draw->p_titlestr, 0, WFNOTE_BG_TITLEBMP_PL_Y, WFNOTE_TITLE_MSG_SPEED, 
			WFNOTE_COL_WHITE, NULL);

    GF_BGL_BmpWinOnVReq(&p_draw->title);
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイトル表示Off
 *
 *	@param	p_draw		ワーク
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DRAW_BmpTitleOff( WFNOTE_DRAW* p_draw )
{
	// タスク動作停止
	if( GF_MSG_PrintEndCheck( p_draw->title_stridx ) == 1 ){
		GF_STR_PrintForceStop( p_draw->title_stridx );
	}

	GF_BGL_BmpWinOffVReq(&p_draw->title);
}

//----------------------------------------------------------------------------
/**
 *	@brief	友達コードをワードセットに設定
 *
 *	@param	p_draw	描画システム
 *	@param	code	コード
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DRAW_FriendCodeSetWordset( WFNOTE_DRAW* p_draw, u64 code )
{
    u64 num;

    num = code / 100000000;
    WORDSET_RegisterNumber(p_draw->p_wordset, 1, num, 4,NUMBER_DISPTYPE_ZERO,NUMBER_CODETYPE_DEFAULT);
    num = (code/10000) % 10000;
    WORDSET_RegisterNumber(p_draw->p_wordset, 2, num, 4,NUMBER_DISPTYPE_ZERO,NUMBER_CODETYPE_DEFAULT);
    num = code % 10000;
    WORDSET_RegisterNumber(p_draw->p_wordset, 3, num, 4,NUMBER_DISPTYPE_ZERO,NUMBER_CODETYPE_DEFAULT);

}

//----------------------------------------------------------------------------
/**
 *	@brief	友達の名前を設定する
 *
 *	@param	p_draw		描画システム
 *	@param	p_save		セーブデータ
 *	@param	idx			友達のインデックス
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DRAW_FriendNameSetWordset( WFNOTE_DRAW* p_draw, SAVEDATA* p_save, u32 idx, u32 heapID )
{
	WIFI_LIST* p_wifilist;
	MYSTATUS* p_mystatus;

	p_wifilist = SaveData_GetWifiListData( p_save );
	p_mystatus = MyStatus_AllocWork( heapID );	
	MyStatus_SetMyName( p_mystatus, WifiList_GetFriendNamePtr(p_wifilist, idx) );
	WORDSET_RegisterPlayerName( p_draw->p_wordset, 0, p_mystatus );
	sys_FreeMemoryEz( p_mystatus );
}

//----------------------------------------------------------------------------
/**
 *	@brief	友達のグループをワードセットに設定する
 *
 *	@param	p_draw		描画システム
 *	@param	p_save		データ
 *	@param	idx			インデックス
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DRAW_FriendGroupSetWordset( WFNOTE_DRAW* p_draw, SAVEDATA* p_save, u32 idx, u32 heapID )
{
	MYSTATUS* p_mystatus = MyStatus_AllocWork( heapID );
	WIFI_LIST* p_wifilist = SaveData_GetWifiListData( p_save );
	MyStatus_SetMyName(p_mystatus, WifiList_GetFriendGroupNamePtr( p_wifilist, idx ));
	WORDSET_RegisterPlayerName( p_draw->p_wordset, 0, p_mystatus );
	sys_FreeMemoryEz( p_mystatus );
}

//----------------------------------------------------------------------------
/**
 *	@brief	友達と最後に遊んだ日にちを設定
 *
 *	@param	p_draw		描画システム
 *	@param	p_save		セーブデータ
 *	@param	idx			友達リストインデックス
 *
 *	@retval	TRUE	日にちがあった
 *	@retval	FALSE	日にちがなかった
 */
//-----------------------------------------------------------------------------
static BOOL WFNOTE_DRAW_FriendDaySetWordset( WFNOTE_DRAW* p_draw, SAVEDATA* p_save, u32 idx )
{
	WIFI_LIST* p_wifilist = SaveData_GetWifiListData( p_save );
	u32 num;
	BOOL ret = TRUE;

	num = WifiList_GetFriendInfo( p_wifilist, idx, WIFILIST_FRIEND_LASTBT_DAY);
	if( num == 0 ){
		ret = FALSE;	// ０ということは１回も遊んでいない
	}
	// MatchComment: NUMBER_DISPTYPE_SPACE -> NUMBER_DISPTYPE_LEFT
	WORDSET_RegisterNumber( p_draw->p_wordset, 2, num, 2, NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);
	num = WifiList_GetFriendInfo( p_wifilist, idx, WIFILIST_FRIEND_LASTBT_YEAR);
	// MatchComment: NUMBER_DISPTYPE_SPACE -> NUMBER_DISPTYPE_LEFT
	WORDSET_RegisterNumber( p_draw->p_wordset, 0, num, 4, NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);
	num = WifiList_GetFriendInfo( p_wifilist, idx, WIFILIST_FRIEND_LASTBT_MONTH);
    // MatchComment: new change in plat US
    WORDSET_RegisterMonthName(p_draw->p_wordset, 1, num);

	return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ワードセットに数字を設定する
 *
 *	@param	p_draw		描画システム
 *	@param	num			数字
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DRAW_NumberSetWordset( WFNOTE_DRAW* p_draw, u32 num )
{
	WORDSET_RegisterNumber( p_draw->p_wordset, 0, num, 4, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
}

//----------------------------------------------------------------------------
/**
 *	@brief	ワードセットにロビーミニゲーム名を設定
 *
 *	@param	p_draw
 *	@param	num 
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DRAW_WflbyGameSetWordSet( WFNOTE_DRAW* p_draw, u32 num )
{
	WORDSET_RegisterWiFiLobbyGameName( p_draw->p_wordset, 0, num );
}

//----------------------------------------------------------------------------
/**
 *	@brief	矢印初期化
 *
 *	@param	p_draw	描画ワーク
 *	@param	heapID	ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DRAW_YazirushiInit( WFNOTE_DRAW* p_draw, u32 heapID )
{
	static CLACT_ADD yazirushiadd[WFNOTE_YAZIRUSHINUM] = {
		{	// ←
			NULL, NULL,
			{ WFNOTE_YAZIRUSHI_XLEF, WFNOTE_YAZIRUSHI_Y, 0 },
			{ FX32_ONE, FX32_ONE, FX32_ONE },
			0, WFNOTE_YAZIRUSHI_PRI,
			NNS_G2D_VRAM_TYPE_2DMAIN, 0
		},
		{	// →
			NULL, NULL,
			{ WFNOTE_YAZIRUSHI_XRIGHT, WFNOTE_YAZIRUSHI_Y, 0 },
			{ FX32_ONE, FX32_ONE, FX32_ONE },
			0, WFNOTE_YAZIRUSHI_PRI,
			NNS_G2D_VRAM_TYPE_2DMAIN, 0
		},
	};
	int i;

	// 矢印アクター作成
	for( i=0; i<WFNOTE_YAZIRUSHINUM; i++ ){
		yazirushiadd[i].ClActSet = p_draw->p_clact;
		yazirushiadd[i].ClActHeader = &p_draw->clheader;
		yazirushiadd[i].heap	= heapID;
		p_draw->p_yazirushi[i] = CLACT_Add( &yazirushiadd[i] );
		CLACT_SetAnmFlag( p_draw->p_yazirushi[i], TRUE );
		CLACT_SetDrawFlag( p_draw->p_yazirushi[i], FALSE );
		CLACT_AnmChg( p_draw->p_yazirushi[i],  i );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	矢印破棄
 *
 *	@param	p_draw	描画ワーク
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DRAW_YazirushiExit( WFNOTE_DRAW* p_draw )
{
	int i;
	// 矢印アクター破棄
	for( i=0; i<WFNOTE_YAZIRUSHINUM; i++ ){
		CLACT_Delete( p_draw->p_yazirushi[i] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	矢印表示設定
 *
 *	@param	p_draw	描画ワーク
 *	@param	flag	フラグ
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DRAW_YazirushiSetDrawFlag( WFNOTE_DRAW* p_draw, BOOL flag )
{
	int i;
	for( i=0; i<WFNOTE_YAZIRUSHINUM; i++ ){
		CLACT_SetDrawFlag( p_draw->p_yazirushi[i], flag );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	矢印アニメ設定
 *
 *	@param	p_draw	描画ワーク
 *	@param	flag	フラグ
 */
//-----------------------------------------------------------------------------
static void WFNOTE_DRAW_YazirushiSetAnmFlag( WFNOTE_DRAW* p_draw, BOOL flag )
{
	int i;
	for( i=0; i<WFNOTE_YAZIRUSHINUM; i++ ){
		CLACT_SetAnmFlag( p_draw->p_yazirushi[i], flag );
	}
}



//-------------------------------------
///	選択モード関数
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	モード選択画面　ワーク初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画ワーク	
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_MODESELECT_Init( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	WFNOTE_MODESELECT_DrawInit( p_wk, p_data, p_draw, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	モード選択画面	メイン
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *	@param	heapID		ヒープID
 *
 *	@retval	WFNOTE_STRET_CONTINUE,	// つづく
 *	@retval	WFNOTE_STRET_FINISH,	// 作業は終わった
 */
//-----------------------------------------------------------------------------
static WFNOTE_STRET WFNOTE_MODESELECT_Main( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	BOOL result;
	
	switch( p_data->subseq ){
	case WFNOTE_MODESELECT_SEQ_INIT:	// 初期化
		WFNOTE_MODESELECT_SEQ_Init( p_wk, p_data, p_draw );
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_HOLEIN, WIPE_TYPE_HOLEIN, WIPE_FADE_BLACK, 
				WIPE_DEF_DIV, WIPE_DEF_SYNC, heapID);
		p_data->subseq = WFNOTE_MODESELECT_SEQ_FADEINWAIT;
		break;
		
	case WFNOTE_MODESELECT_SEQ_FADEINWAIT:
		if( WIPE_SYS_EndCheck() ){
			p_data->subseq = WFNOTE_MODESELECT_SEQ_MAIN;
		}
		break;

	case WFNOTE_MODESELECT_SEQ_INIT_NOFADE:
		WFNOTE_MODESELECT_SEQ_Init( p_wk, p_data, p_draw );
		p_data->subseq = WFNOTE_MODESELECT_SEQ_MAIN;
		break;

	case WFNOTE_MODESELECT_SEQ_MAIN:
		result = WFNOTE_MODESELECT_SEQ_Main( p_wk, p_data, p_draw );
		if( result == TRUE ){

			// その状態に遷移して大丈夫かチェック
			if( WFNOTE_MODESELECT_StatusChengeCheck( p_wk, p_data ) == TRUE ){

				// カーソル位置に対応した終了設定を行う
				WFNOTE_MODESELECT_EndDataSet( p_wk, p_data );
				
				// 終了チェック
				if( (p_wk->cursor == WFNOTE_MODESELECT_CURSOR_END) ||
					(p_wk->cursor == WFNOTE_MODESELECT_CURSOR_CODEIN) ){
					p_data->subseq = WFNOTE_MODESELECT_SEQ_FADEOUT;
				}else{
					WFNOTE_MODESELECT_DrawOff( p_wk, p_draw );
					return WFNOTE_STRET_FINISH;
				}
			}else{

				if( p_wk->cursor == WFNOTE_MODESELECT_CURSOR_CODEIN ){
					// メッセージをだして
					// メッセージ終了待ちする
					WFNOTE_MODESELECT_TalkMsgPrint( p_wk, p_draw, msg_wifi_note_30, heapID );
					p_data->subseq = WFNOTE_MODESELECT_SEQ_CODECHECK_RESLUTMSGWAIT;
				}
			}
		}
		break;


		// そのまま終了するとき
	case WFNOTE_MODESELECT_SEQ_FADEOUT:
        WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_HOLEOUT, WIPE_TYPE_HOLEOUT, WIPE_FADE_BLACK, 
                        WIPE_DEF_DIV, WIPE_DEF_SYNC, heapID);

		p_data->subseq ++;
		break;
		
	case WFNOTE_MODESELECT_SEQ_FADEOUTWAIT:
		if( WIPE_SYS_EndCheck() == TRUE ){
			return WFNOTE_STRET_FINISH;
		}
		break;

		// コード入力画面からの復帰
	case WFNOTE_MODESELECT_SEQ_CODECHECK_INIT:
		WFNOTE_MODESELECT_SEQ_Init( p_wk, p_data, p_draw );
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_HOLEIN, WIPE_TYPE_HOLEIN, WIPE_FADE_BLACK, 
				WIPE_DEF_DIV, WIPE_DEF_SYNC, heapID);
		p_data->subseq = WFNOTE_MODESELECT_SEQ_CODECHECK_FADEINWAIT;
		break;
		
	case WFNOTE_MODESELECT_SEQ_CODECHECK_FADEINWAIT:
		if( WIPE_SYS_EndCheck() ){
			u64 code;
			MYSTATUS* p_friend;
			// メッセージの表示
			// 名前を設定
			p_friend = MyStatus_AllocWork( heapID );
			MyStatus_SetMyName(p_friend, STRBUF_GetStringCodePointer(p_data->codein.p_name));
			WORDSET_RegisterPlayerName( p_draw->p_wordset, 0, p_friend );
			sys_FreeMemoryEz( p_friend );

			// 友達コード設定
			code = STRBUF_GetNumber(p_data->codein.p_code, &result);
			if( result ){	// 数字に変換できたか
				WFNOTE_DRAW_FriendCodeSetWordset( p_draw, code );

				WFNOTE_MODESELECT_TalkMsgPrint( p_wk, p_draw, msg_wifi_note_26, heapID );
				p_data->subseq = WFNOTE_MODESELECT_SEQ_CODECHECK_MSGWAIT;
			}else{

				// コードが間違っていると表示
				WFNOTE_MODESELECT_TalkMsgPrint( p_wk, p_draw, msg_wifi_note_27, heapID );
				p_data->subseq = WFNOTE_MODESELECT_SEQ_CODECHECK_RESLUTMSGWAIT;  // ともだちコード違う表示
			}
		}
		break;
		
	case WFNOTE_MODESELECT_SEQ_CODECHECK_MSGWAIT:		//登録確認メッセージON
		if( WFNOTE_MODESELECT_TalkMsgEndCheck( p_wk ) == FALSE ){
			break;
		}

		// はいいいえウィンドウ表示
        p_wk->p_yesno =
            BmpYesNoSelectInit( p_draw->p_bgl,
                                &WFNOTE_MODESELECT_YESNOWIN,
                                WFNOTE_BGCGX_MAIN_MSG_MENU, WFNOTE_BGPL_MAIN_MENUWIN,
                                heapID );
        p_data->subseq = WFNOTE_MODESELECT_SEQ_CODECHECK_MSGYESNOWAIT;
		break;
		
	case WFNOTE_MODESELECT_SEQ_CODECHECK_MSGYESNOWAIT:	//はいいいえ選択待ち
		{
			int ret;
			u32 set_ret;
			static const u8 msgdata[ WFNOTE_DATA_NEWFRIENDSET_NUM ] = {
				0,
				msg_wifi_note_27,
				msg_wifi_note_31,
			};
			

			ret = BmpYesNoSelectMain( p_wk->p_yesno, heapID );

			switch( ret ){
			case BMPMENU_NULL:	// 何もおきてない
				break;
				
			case BMPMENU_CANCEL:	// いいえ
				// メッセージを消して終わる
				WFNOTE_MODESELECT_TalkMsgOff( p_wk );
				p_data->subseq = WFNOTE_MODESELECT_SEQ_MAIN;
				break;
				
			default:		// はい
				// 登録作業をして結果を表示
				set_ret = WFNOTE_DATA_NewFriendDataSet( p_data, p_data->codein.p_code, p_data->codein.p_name );
				if( set_ret == WFNOTE_DATA_NEWFRIENDSET_OK ){
					// 成功したのでそのまま終わる
					WFNOTE_MODESELECT_TalkMsgOff( p_wk );
					p_data->subseq = WFNOTE_MODESELECT_SEQ_MAIN;
				}else{
					WFNOTE_MODESELECT_TalkMsgPrint( p_wk, p_draw, msgdata[set_ret], heapID );
					p_data->subseq = WFNOTE_MODESELECT_SEQ_CODECHECK_RESLUTMSGWAIT;  // ともだちコード違う表示
				}
				break;
			}
		}
		break;
		
	case WFNOTE_MODESELECT_SEQ_CODECHECK_RESLUTMSGWAIT:	//選択結果表示
		if( WFNOTE_MODESELECT_TalkMsgEndCheck( p_wk ) == FALSE ){
			break;
		}
		if( sys.trg & PAD_BUTTON_DECIDE ){
			WFNOTE_MODESELECT_TalkMsgOff( p_wk );
			p_data->subseq = WFNOTE_MODESELECT_SEQ_MAIN;
		}
		break;
	}
	return WFNOTE_STRET_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	モード選択画面	ワーク破棄
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 */
//-----------------------------------------------------------------------------
static void WFNOTE_MODESELECT_Exit( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
	WFNOTE_MODESELECT_DrawExit( p_wk, p_data, p_draw );
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画関係の初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_MODESELECT_DrawInit( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	STRBUF* p_str;
	STRBUF* p_tmp;
	int i;
	static const u8 sc_msgidx[ 4 ] = {	// msg_idx
		msg_wifi_note_02,
		msg_wifi_note_03,
		msg_wifi_note_04,
		msg_wifi_note_05,
	};
    static const int sc_pos[4] = {	// 座標
        -4,
        -4,
        -4,
        0,
    };
    static const u8 sOv64_223241C[] = {
        8,
        48,
        88,
        128,
    };

	
	// ビットマップウィンドウ作成
	GF_BGL_BmpWinInit( &p_wk->msg );
	GF_BGL_BmpWinAdd( p_draw->p_bgl, &p_wk->msg, 
			sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_SCRMSG], 
			WFNOTE_MODESELECT_MSG_X, WFNOTE_MODESELECT_MSG_Y, 
			WFNOTE_MODESELECT_MSG_SIZX, WFNOTE_MODESELECT_MSG_SIZY, 
			WFNOTE_BGPL_MAIN_MSGFONT, WFNOTE_MODESELECT_CGX );

	GF_BGL_BmpWinInit( &p_wk->talk );
	GF_BGL_BmpWinAdd( p_draw->p_bgl, &p_wk->talk, 
			sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_MSG], 
			WFNOTE_MODESELECT_TALK_X, WFNOTE_MODESELECT_TALK_Y, 
			WFNOTE_MODESELECT_TALK_SIZX, WFNOTE_MODESELECT_TALK_SIZY, 
			WFNOTE_BGPL_MAIN_MSGFONT, WFNOTE_MODESELECT_TALKCGX );

	// クリーン
	GF_BGL_BmpWinDataFill( &p_wk->msg, 0 );
	GF_BGL_BmpWinDataFill( &p_wk->talk, 0 );

	// 会話ウィンドウ用メッセージバッファ確保
	p_wk->p_str = STRBUF_Create( WFNOTE_STRBUF_SIZE, heapID );

	// メッセージスピード取得
	p_wk->msg_speed = CONFIG_GetMsgPrintSpeed(SaveData_GetConfig(p_data->p_save)); 

	// メッセージ書き込み
	p_str = STRBUF_Create( WFNOTE_STRBUF_SIZE, heapID );
	p_tmp = STRBUF_Create( WFNOTE_STRBUF_SIZE, heapID );

	// 自分の名前をワードセットに設定
	WORDSET_RegisterPlayerName( p_draw->p_wordset, 0, SaveData_GetMyStatus( p_data->p_save ) );
	
	for( i=0; i<4; i++ ){
        int x,y,xofs;
		MSGMAN_GetString( p_draw->p_msgman, sc_msgidx[i], p_tmp );
		WORDSET_ExpandStr( p_draw->p_wordset, p_str, p_tmp );
        //MatchComment: center align; temp vars needed
        x = sc_pos[i] + FontProc_GetPrintCenteredPositionX( FONT_SYSTEM, p_str, 0, WFNOTE_FRIENDLIST_TEXT_SIZX*8 );
        y = sOv64_223241C[i];
		GF_STR_PrintColor( &p_wk->msg, FONT_SYSTEM, 
				p_str, x, y, MSG_NO_PUT, WFNOTE_COL_BLACK, NULL);
	}
	
	STRBUF_Delete( p_str );
	STRBUF_Delete( p_tmp );

	// カーソルスクリーン読み込み
	p_wk->p_scrnbuff = ArcUtil_HDL_ScrnDataGet( p_draw->p_handle,
			NARC_pl_wifinote_techo_01_NSCR, FALSE, &p_wk->p_scrn, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画関係の破棄
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 */
//-----------------------------------------------------------------------------
static void WFNOTE_MODESELECT_DrawExit( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
	// 会話文字列バッファ破棄
	STRBUF_Delete( p_wk->p_str );
	
	// ビットマップ破棄
	GF_BGL_BmpWinDel( &p_wk->msg );
	GF_BGL_BmpWinDel( &p_wk->talk );

	// スクリーンデータ破棄
	sys_FreeMemoryEz( p_wk->p_scrnbuff );
}

//----------------------------------------------------------------------------
/**
 *	@brief	シーケンス初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データワーク
 *	@param	p_draw		描画システム
 */
//-----------------------------------------------------------------------------
static void WFNOTE_MODESELECT_SEQ_Init( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
	// タイトル表示
	WFNOTE_DRAW_BmpTitleWrite( p_draw, msg_wifi_note_01 );
	// 文字列描画
	GF_BGL_BmpWinOnVReq( &p_wk->msg );
	// ボタン表示
	GF_BGL_ScrWrite( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_SCROLL ],
			p_wk->p_scrn->rawData, 0, 0, 
			p_wk->p_scrn->screenWidth/8, p_wk->p_scrn->screenHeight/8 );
	// スクロール座標を元に戻す
	GF_BGL_ScrollReq( p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_SCROLL], GF_BGL_SCROLL_X_SET, 0 );
	GF_BGL_ScrollReq( p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_SCROLL], GF_BGL_SCROLL_Y_SET, 0 );
	// カーソル位置のパレットを設定
	WFNOTE_MODESELECT_CursorScrnPalChange( p_wk, p_draw );

	// BG面転送
	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_SCROLL] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	モード選択画面メイン
 *
 *	@param	p_wk		モード選択画面ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画ワーク
 *
 *	@retval	TRUE	何かを選択
 *	@retval	FALSE	何もおきていない
 */
//-----------------------------------------------------------------------------
static BOOL WFNOTE_MODESELECT_SEQ_Main( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
	BOOL move = FALSE;
	
	// 選択チェック
	if( sys.trg & PAD_BUTTON_DECIDE ){
        Snd_SePlay(WIFINOTE_DECIDE_SE);
		return TRUE;
	}

	// キャンセルチェック
	if( sys.trg & PAD_BUTTON_CANCEL ){
        Snd_SePlay(WIFINOTE_BS_SE);
		p_wk->cursor = WFNOTE_MODESELECT_CURSOR_END;	// 終了にする
		return TRUE;
	}

	// 移動チェック
	if( sys.repeat & PAD_KEY_UP ){
		if( p_wk->cursor > 0 ){
            p_wk->cursor--;
			move = TRUE;
		}
	}else if( sys.repeat & PAD_KEY_DOWN ){
		if( p_wk->cursor < WFNOTE_MODESELECT_CURSOR_END ){
            p_wk->cursor++;
			move = TRUE;
		}
	}

	// 移動したら音と描画更新
	if( move == TRUE ){
		Snd_SePlay( WIFINOTE_MOVE_SE );
		WFNOTE_MODESELECT_CursorScrnPalChange( p_wk, p_draw );
		GF_BGL_LoadScreenV_Req( p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_SCROLL] );
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	カーソルの位置に合わせてスクリーンのパレット
 *
 *	@param	p_wk	ワーク
 *	@param	p_draw	描画システム
 */
//-----------------------------------------------------------------------------
static void WFNOTE_MODESELECT_CursorScrnPalChange( WFNOTE_MODESELECT* p_wk, WFNOTE_DRAW* p_draw )
{
	int i;
	static const u8 sc_pos[WFNOTE_MODESELECT_CURSOR_NUM][2] = {
		// x , y
		{ 2, 4 },
		{ 2, 9 },
		{ 2, 14 },
		{ 2, 19 },
	};
	u8 pal;

	for( i=0; i<WFNOTE_MODESELECT_CURSOR_NUM; i++ ){
		if( p_wk->cursor == i ){
			pal = WFNOTE_MODESELECT_CURSOR_PAL_ON;
		}else{
			pal = WFNOTE_MODESELECT_CURSOR_PAL_OFF;
		}
		GF_BGL_ScrPalChange( p_draw->p_bgl, 
				sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_SCROLL],
				sc_pos[i][0], sc_pos[i][1],
				WFNOTE_MODESELECT_CURSOR_CSIZX, WFNOTE_MODESELECT_CURSOR_CSIZY,
				pal );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	カーソル位置に対応した終了データを設定する
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 */
//-----------------------------------------------------------------------------
static void WFNOTE_MODESELECT_EndDataSet( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data )
{
	static const u8 reqstatus[ WFNOTE_MODESELECT_CURSOR_NUM ] = {
		WFNOTE_STATUS_FRIENDLIST,
		WFNOTE_STATUS_CODEIN,
		WFNOTE_STATUS_MYCODE,
		WFNOTE_STATUS_END
	};
	WFNOTE_DATA_SetReqStatus( p_data, reqstatus[p_wk->cursor], 0 );
}

//----------------------------------------------------------------------------
/**
 *	@brief	表示物を全部消す
 *
 *	@param	p_wk		ワーク
 *	@param	p_draw		描画ワーク
 */
//-----------------------------------------------------------------------------
static void WFNOTE_MODESELECT_DrawOff( WFNOTE_MODESELECT* p_wk, WFNOTE_DRAW* p_draw )
{
	// タイトル
	WFNOTE_DRAW_BmpTitleOff( p_draw );

	// SCROLL面
	GF_BGL_ScrFill( p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_SCROLL], 0, 0, 0, 32, 24, 0 );
	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_SCROLL] );

	// SCRMSG面
	GF_BGL_ScrFill( p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_SCRMSG], 0, 0, 0, 32, 24, 0 );
	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_SCRMSG] );

	// msg
	GF_BGL_BmpWinOffVReq(&p_wk->msg);
}	

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージプリント
 *
 *	@param	p_wk		ワーク
 *	@param	p_draw		描画ワーク
 *	@param	msgidx		メッセージIDX
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_MODESELECT_TalkMsgPrint( WFNOTE_MODESELECT* p_wk, WFNOTE_DRAW* p_draw, u32 msgidx, u32 heapID )
{
	STRBUF* p_tmp;

	p_tmp = STRBUF_Create( WFNOTE_STRBUF_SIZE, heapID );
	
	// クリア
	GF_BGL_BmpWinDataFill( &p_wk->talk, 15 );

	MSGMAN_GetString( p_draw->p_msgman, msgidx, p_tmp );
	WORDSET_ExpandStr( p_draw->p_wordset, p_wk->p_str, p_tmp );
    p_wk->talkmsg_idx = GF_STR_PrintColor(&p_wk->talk, FONT_TALK, 
			p_wk->p_str, 0, 0, p_wk->msg_speed, WFNOTE_COL_BLACK, NULL);
    BmpTalkWinWrite( &p_wk->talk, WINDOW_TRANS_OFF, 
			WFNOTE_BGCGX_MAIN_MSG_TALK, WFNOTE_BGPL_MAIN_TALKWIN );

    GF_BGL_BmpWinOnVReq(&p_wk->talk);

	STRBUF_Delete( p_tmp );
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージの終了待ち
 *	
 *	@param	cp_wk	ワーク
 *	
 *	@retval	TRUE	終了
 *	@retval	FALSE	まだ
 */
//-----------------------------------------------------------------------------
static BOOL WFNOTE_MODESELECT_TalkMsgEndCheck( const WFNOTE_MODESELECT* cp_wk )
{
    if( GF_MSG_PrintEndCheck( cp_wk->talkmsg_idx ) == 0 ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	会話ウインドウを消す
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFNOTE_MODESELECT_TalkMsgOff( WFNOTE_MODESELECT* p_wk )
{
	BmpTalkWinClear( &p_wk->talk, WINDOW_TRANS_OFF );
    GF_BGL_BmpWinOffVReq(&p_wk->talk);
}

//----------------------------------------------------------------------------
/**
 *	@brief	カーソル位置のステータスに遷移しても大丈夫かチェックする
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *
 *	@retval	TRUE	大丈夫
 *	@retval	FALSE	だめ
 */
//-----------------------------------------------------------------------------
static BOOL WFNOTE_MODESELECT_StatusChengeCheck( WFNOTE_MODESELECT* p_wk, WFNOTE_DATA* p_data )
{
	int  i;
	WIFI_LIST* p_list;
	
	if( p_wk->cursor == WFNOTE_MODESELECT_CURSOR_CODEIN ){
		// あきがあるかチェック
		p_list = SaveData_GetWifiListData( p_data->p_save );
		for(i=0;i<WIFILIST_FRIEND_MAX;i++){
			if( !WifiList_IsFriendData( p_list, i ) ){  // 
				return TRUE;
			}
		}
	}else{

		// 友達登録以外は無条件でOK
		return TRUE;
	}

	return FALSE;
}



//-------------------------------------
///	友達データ表示関数
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	友達データ表示ワーク	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDLIST_Init( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	p_wk->msgspeed = CONFIG_GetMsgPrintSpeed(SaveData_GetConfig(p_data->p_save));
	
	WFNOTE_FRIENDLIST_DrawInit( p_wk, p_data, p_draw,  heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	友達データ表示ワーク	メイン
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *	@param	heapID		ヒープID
 *
 *	@retval	WFNOTE_STRET_CONTINUE,	// つづく
 *	@retval	WFNOTE_STRET_FINISH,	// 作業は終わった
 */
//-----------------------------------------------------------------------------
static WFNOTE_STRET WFNOTE_FRIENDLIST_Main( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	u32 result;
	WIFI_LIST* p_wifilist;
	u32 listidx;

	switch( p_data->subseq ){
	case WFNOTE_FRIENDLIST_SEQ_INIT_PAGEINIT:	// 表示するページを初期化するとき
		p_wk->page = 0;			
		p_wk->lastpage = 0;			
		p_wk->pos = 0;
		p_wk->lastpos = 0;
	case WFNOTE_FRIENDLIST_SEQ_INIT:	// 以前の状態で表示
		// リストデータ作成
		WFNOTE_DATA_FrIdxMake( p_data );

		// 描画初期化
		WFNOTE_FRIENDLIST_SEQ_DrawOn( p_wk, p_data, p_draw, heapID );	

		p_data->subseq  = WFNOTE_FRIENDLIST_SEQ_MAIN;
		break;

		// リスト動作
	case WFNOTE_FRIENDLIST_SEQ_MAIN:
		result = WFNOTE_FRIENDLIST_SEQ_Main( p_wk, p_data,  p_draw );
		switch( result ){
		case WFNOTE_FRIENDLIST_SEQMAINRET_NONE:
			break;
			
		case WFNOTE_FRIENDLIST_SEQMAINRET_PAGECHANGELEFT:// ページ変更
			p_wk->lastpage = p_wk->page;
			p_wk->page --;
			if( p_wk->page < 0 ){
				p_wk->page += WFNOTE_FRIENDLIST_PAGE_NUM;
			}
			WFNOTE_FRIENDLIST_ScrSeqChange( p_wk, p_data, WF_COMMON_LEFT );
			break;
			
		case WFNOTE_FRIENDLIST_SEQMAINRET_PAGECHANGERIGHT:// ページ変更
			p_wk->lastpage = p_wk->page;
			p_wk->page = (p_wk->page + 1) % WFNOTE_FRIENDLIST_PAGE_NUM;
			WFNOTE_FRIENDLIST_ScrSeqChange( p_wk, p_data, WF_COMMON_RIGHT );
			break;
			
		case WFNOTE_FRIENDLIST_SEQMAINRET_CANCEL:	// 戻る
			p_data->subseq = WFNOTE_FRIENDLIST_SEQ_END;
			break;
			
		case WFNOTE_FRIENDLIST_SEQMAINRET_SELECTPL:	// 人を選択した
			p_data->subseq = WFNOTE_FRIENDLIST_SEQ_MENUINIT;
			break;
		}
		break;
		
		// スクロール制御
	case WFNOTE_FRIENDLIST_SEQ_SCRLLINIT:
		WFNOTE_FRIENDLIST_SEQ_ScrllInit( p_wk, p_data, p_draw, heapID );
		p_data->subseq = WFNOTE_FRIENDLIST_SEQ_SCRLL;
		break;
		
	case WFNOTE_FRIENDLIST_SEQ_SCRLL:

		result = WFNOTE_FRIENDLIST_SEQ_ScrllMain( p_wk, p_data, p_draw, heapID );
		if( result == TRUE ){
			p_data->subseq = WFNOTE_FRIENDLIST_SEQ_MAIN;
		}
		break;

	case WFNOTE_FRIENDLIST_SEQ_MENUINIT:	//　メニュー選択

		// 選択したリストインデックスをデータ構造体に設定
		WFNOTE_DATA_SELECT_ListIdxSet( p_data, 
				WFNOTE_FRIENDLIST_FRIDX_IdxGet( p_wk ) );
		
		WFNOTE_FRIENDLIST_SEQ_MenuInit( p_wk, p_data, p_draw, heapID );
		p_data->subseq = WFNOTE_FRIENDLIST_SEQ_MENUWAIT;
		break;
		
	case WFNOTE_FRIENDLIST_SEQ_MENUWAIT:	// メニュー選択待ち
		p_data->subseq = WFNOTE_FRIENDLIST_SEQ_MenuWait( p_wk, p_data, p_draw, heapID );
		break;
		
	case WFNOTE_FRIENDLIST_SEQ_INFOINIT:		// 詳しく見るへ
		p_wk->wait = WFNOTE_FRIENDLIST_INFO_WAIT;
		p_data->subseq = WFNOTE_FRIENDLIST_SEQ_INFO;
		break;

	case WFNOTE_FRIENDLIST_SEQ_INFO:		// 詳しく見るへ
		// 画面切り替えがきれいに見えるようにちょっとwait
		p_wk->wait --;
		if( p_wk->wait <= 0 ){
			WFNOTE_FRIENDLIST_SEQ_DrawOff( p_wk, p_data, p_draw );
			WFNOTE_DATA_SetReqStatus( p_data, WFNOTE_STATUS_FRIENDINFO, 0 );
			return WFNOTE_STRET_FINISH; 
		}
		break;

	case WFNOTE_FRIENDLIST_SEQ_INFORET: // 詳しく見るから戻ってきたとき
		// ページを設定
		p_wk->page = p_data->select_listidx / WFNOTE_FRIENDLIST_PAGEFRIEND_NUM;
		p_wk->pos = p_data->select_listidx % WFNOTE_FRIENDLIST_PAGEFRIEND_NUM;
		p_data->subseq = WFNOTE_FRIENDLIST_SEQ_INIT;	// 初期化へ
		break;
		
	case WFNOTE_FRINEDLIST_SEQ_NAMECHG_INIT:	// 名前変更
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_HOLEOUT, WIPE_TYPE_HOLEOUT, WIPE_FADE_BLACK, 
				WIPE_DEF_DIV, WIPE_DEF_SYNC, heapID);

		// 文字列初期化データを設定
		listidx = WFNOTE_FRIENDLIST_FRIDX_IdxGet( p_wk );
		p_wifilist = SaveData_GetWifiListData( p_data->p_save );
		WFNOTE_DATA_NAMEIN_INIT_Set( p_data, WifiList_GetFriendNamePtr( p_wifilist, p_data->idx.fridx[listidx] ) );	
		p_data->subseq = WFNOTE_FRINEDLIST_SEQ_NAMECHG_WIPE;
		break;

	case WFNOTE_FRINEDLIST_SEQ_NAMECHG_WIPE:	// 名前変更
		if( WIPE_SYS_EndCheck() ){
			// 名前入力画面へ
			WFNOTE_FRIENDLIST_SEQ_DrawOff( p_wk, p_data, p_draw );
			WFNOTE_DATA_SetReqStatus( p_data, WFNOTE_STATUS_CODEIN, WFNOTE_CODEIN_SEQ_NAMEINONLY );
			return WFNOTE_STRET_FINISH;
		}
		break;
		
	case WFNOTE_FRINEDLIST_SEQ_NAMECHG_WAIT:	// 名前変更
		// 名前を変更
		listidx = WFNOTE_FRIENDLIST_FRIDX_IdxGet( p_wk );
		p_wifilist = SaveData_GetWifiListData( p_data->p_save );
		WifiList_SetFriendName( p_wifilist, p_data->idx.fridx[listidx], p_data->codein.p_name );
	case WFNOTE_FRINEDLIST_SEQ_NAMECHG_WAITWIPE:	// 名前変更	何もしないで復帰
		// 描画初期化
		WFNOTE_FRIENDLIST_SEQ_DrawOn( p_wk, p_data, p_draw, heapID );
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_HOLEIN, WIPE_TYPE_HOLEIN, WIPE_FADE_BLACK, 
				WIPE_DEF_DIV, WIPE_DEF_SYNC, heapID);
		p_data->subseq = WFNOTE_FRINEDLIST_SEQ_NAMECHG_WAITWIPEWAIT;
		break;
		
	case WFNOTE_FRINEDLIST_SEQ_NAMECHG_WAITWIPEWAIT:	// 名前変更
		if( WIPE_SYS_EndCheck() ){
			p_data->subseq = WFNOTE_FRIENDLIST_SEQ_MAIN;
		}
		break;
		
	case WFNOTE_FRIENDLIST_SEQ_DELETE_INIT:	// 破棄
		WFNOTE_FRIENDLIST_SEQ_DeleteInit( p_wk, p_data, p_draw, heapID );
		p_data->subseq = WFNOTE_FRIENDLIST_SEQ_DELETE_YESNODRAW;
		break;

	case WFNOTE_FRIENDLIST_SEQ_DELETE_YESNODRAW:	// 破棄	YESNOウィンドウ表示
		result = WFNOTE_FRIENDLIST_SEQ_DeleteYesNoDraw( p_wk, p_data, p_draw, heapID );
		if( result == TRUE ){
			p_data->subseq = WFNOTE_FRIENDLIST_SEQ_DELETE_WAIT;
		}
		break;
		
	case WFNOTE_FRIENDLIST_SEQ_DELETE_WAIT:	// 破棄YESNOをまってから処理
		result = BmpYesNoSelectMain( p_wk->p_yesno, heapID );

		switch( result ){
		case BMPMENU_NULL:	// 何もおきてない
			break;	
			
		case BMPMENU_CANCEL:	// いいえ
			// メッセージを消して矢印を出す
			WFNOTE_FRIENDLIST_TalkMsgOff( p_wk );
			WFNOTE_DRAW_YazirushiSetDrawFlag( p_draw, TRUE );
			p_data->subseq =  WFNOTE_FRIENDLIST_SEQ_MAIN;
			break;
			
		default:		// はい
			WFNOTE_FRIENDLIST_TalkMsgOff( p_wk );
			p_data->subseq =  WFNOTE_FRIENDLIST_SEQ_DELETE_ANMSTART;
			break;
		}

		break;

	case WFNOTE_FRIENDLIST_SEQ_DELETE_ANMSTART:	// 破棄YESNOをまってから処理
		WFNOTE_FRIENDLIST_DeleteAnmStart( p_wk );
		p_data->subseq = WFNOTE_FRIENDLIST_SEQ_DELETE_ANMWAIT;
		break;
		
	case WFNOTE_FRIENDLIST_SEQ_DELETE_ANMWAIT:	// 破棄YESNOをまってから処理
		if( WFNOTE_FRIENDLIST_DeleteAnmMain( p_wk ) == TRUE ){
			// 破棄処理
			p_wifilist = SaveData_GetWifiListData( p_data->p_save );
			listidx = WFNOTE_FRIENDLIST_FRIDX_IdxGet( p_wk );
			WifiList_ResetData( p_wifilist, p_data->idx.fridx[ listidx ] );
			//フレンド毎に持つフロンティアデータも削除 2008.05.24(土) matsuda
			FrontierRecord_ResetData(
				SaveData_GetFrontier(p_data->p_save), p_data->idx.fridx[ listidx ]);

			// 友達インデックスを作り直す
			// 友達の表示を書き直す
			// リストデータ作成
			WFNOTE_DATA_FrIdxMake( p_data );

			// 描画初期化
			WFNOTE_FRIENDLIST_DrawPage( p_wk, p_data, p_draw, p_wk->page, p_wk->pos, WFNOTE_DRAWAREA_MAIN, heapID );

			// メッセージを消して矢印を出す
			WFNOTE_DRAW_YazirushiSetDrawFlag( p_draw, TRUE );
			p_data->subseq =  WFNOTE_FRIENDLIST_SEQ_DELETE_END;
		}
		break;

	case WFNOTE_FRIENDLIST_SEQ_DELETE_END:
		WFNOTE_FRIENDLIST_DeleteAnmOff( p_wk );
		p_data->subseq =  WFNOTE_FRIENDLIST_SEQ_MAIN;
		break;
		
	case WFNOTE_FRIENDLIST_SEQ_CODE_INIT:	// コード切り替え
		result = WFNOTE_FRIENDLIST_SEQ_CodeInit( p_wk, p_data, p_draw, heapID );
		if( result == TRUE ){
			p_data->subseq = WFNOTE_FRIENDLIST_SEQ_CODE_WAIT;
		}else{
			WFNOTE_DRAW_YazirushiSetDrawFlag( p_draw, TRUE );	// 矢印表示
			p_data->subseq = WFNOTE_FRIENDLIST_SEQ_MAIN;
		}
		break;
		
	case WFNOTE_FRIENDLIST_SEQ_CODE_WAIT:	// 何かキーを押してもらう
		if( WFNOTE_FRIENDLIST_TalkMsgEndCheck( p_wk ) == FALSE ){
			break;
		}
		
		if( sys.trg ){	// なにかキーが押されたら元に戻る
			WFNOTE_DRAW_YazirushiSetDrawFlag( p_draw, TRUE );	// 矢印表示
			p_data->subseq = WFNOTE_FRIENDLIST_SEQ_MAIN;
			WFNOTE_FRIENDLIST_TalkMsgOff( p_wk );
		}
		break;
		
	case WFNOTE_FRIENDLIST_SEQ_END:
		WFNOTE_FRIENDLIST_SEQ_DrawOff( p_wk, p_data, p_draw );
		WFNOTE_DATA_SetReqStatus( p_data, WFNOTE_STATUS_MODE_SELECT, WFNOTE_MODESELECT_SEQ_INIT_NOFADE );
		return WFNOTE_STRET_FINISH; 
	}
	return WFNOTE_STRET_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	友達データ表示ワーク	破棄
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDLIST_Exit( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
	WFNOTE_FRIENDLIST_DrawExit( p_wk, p_data, p_draw );
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDLIST_DrawInit( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	static const u16 sc_TEXTCGX[ WFNOTE_DRAWAREA_NUM ] = {
		WFNOTE_FRIENDLIST_TEXT_CGX0,
		WFNOTE_FRIENDLIST_TEXT_CGX1,
		WFNOTE_FRIENDLIST_TEXT_CGX1,
	};
	int i, j;
	STRBUF* p_str;
    BMPLIST_HEADER list_h;
	CLACT_ADD clearadd = {
		NULL,
		NULL,
		{ 0,0,0 },	
		{ FX32_ONE,FX32_ONE,FX32_ONE },	
		0,
		0,
		NNS_G2D_VRAM_TYPE_2DMAIN,
		0
	};

	//  描画エリア作成
	for( i=0; i<WFNOTE_DRAWAREA_NUM; i++ )
	{
		WFNOTE_FRIENDLIST_DRAWAREA_Init( &p_wk->drawdata[ i ], p_draw, 
				&sc_SCRNAREA[i], sc_TEXTCGX[i], heapID );
	}

	// キャラクタデータ初期化
	p_wk->p_charsys = WF_2DC_SysInit( p_draw->p_clact, NULL, WFNOTE_FRIENDLIST_2DCHAR_NUM, heapID );

	// ユニオンキャラクタを読み込む
	WF_2DC_UnionResSet( p_wk->p_charsys, NNS_G2D_VRAM_TYPE_2DMAIN, 
			WF_2DC_MOVETURN, heapID );

		
	// 戻るメッセージ作成
	GF_BGL_BmpWinInit( &p_wk->backmsg );	
	GF_BGL_BmpWinAdd( p_draw->p_bgl, &p_wk->backmsg, 
			sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_MSG ], 
			WFNOTE_FRIENDLIST_BACKMSG_X, WFNOTE_FRIENDLIST_BACKMSG_Y,
			WFNOTE_FRIENDLIST_BACKMSG_SIZX, WFNOTE_FRIENDLIST_BACKMSG_SIZY,
			WFNOTE_BGPL_MAIN_MSGFONT, WFNOTE_FRIENDLIST_BACKMSG_CGX );
	GF_BGL_BmpWinDataFill( &p_wk->backmsg, 0 );

	p_str = STRBUF_Create( WFNOTE_STRBUF_SIZE, heapID );

	MSGMAN_GetString( p_draw->p_msgman, msg_wifi_note_08, p_str );
    //MatchComment: center align
    GF_STR_PrintColor( &p_wk->backmsg, FONT_SYSTEM, 
			p_str,
			FontProc_GetPrintCenteredPositionX( FONT_SYSTEM, p_str, 0, WFNOTE_FRIENDLIST_BACKMSG_SIZX*8 ), 0, MSG_NO_PUT, WFNOTE_COL_BLACK, NULL);
	STRBUF_Delete( p_str );

	// メニュー作成
	GF_BGL_BmpWinInit( &p_wk->menu );
	GF_BGL_BmpWinAdd( p_draw->p_bgl, &p_wk->menu, 
			sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_MSG ], 
			WFNOTE_FRIENDLIST_MENU_X, WFNOTE_FRIENDLIST_MENU_Y,
			WFNOTE_FRIENDLIST_MENU_SIZX, WFNOTE_FRIENDLIST_MENU_SIZY,
			WFNOTE_BGPL_MAIN_MSGFONT, WFNOTE_FRIENDLIST_MENU_CGX );
	GF_BGL_BmpWinDataFill( &p_wk->menu, 0 );

	// メニューデータ作成
	for( i=0; i<WFNOTE_FRIENDLIST_MENU_NUM; i++ ){
		p_wk->p_menulist[i] = BMP_MENULIST_Create( WFNOTE_FRIENDLIST_MENU_LISTNUM, heapID );
		for( j=0; j<WFNOTE_FRIENDLIST_MENU_LISTNUM; j++ ){
			BMP_MENULIST_AddArchiveString( p_wk->p_menulist[i], p_draw->p_msgman,
					sc_WFNOTE_FRIENDLIST_MENU[i][j].strid, sc_WFNOTE_FRIENDLIST_MENU[i][j].param );
		}
	}

	// 会話ウィンドウ作成
	GF_BGL_BmpWinInit( &p_wk->talk );
	GF_BGL_BmpWinAdd( p_draw->p_bgl, &p_wk->talk, 
			sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_MSG ], 
			WFNOTE_FRIENDLIST_TALK_X, WFNOTE_FRIENDLIST_TALK_Y,
			WFNOTE_FRIENDLIST_TALK_SIZX, WFNOTE_FRIENDLIST_TALK_SIZY,
			WFNOTE_BGPL_MAIN_MSGFONT, WFNOTE_FRIENDLIST_TALK_CGX );
	GF_BGL_BmpWinDataFill( &p_wk->talk, 15 );
	p_wk->p_talkstr = STRBUF_Create( WFNOTE_STRBUF_SIZE, heapID );

	// 消しゴムアクターの作成
	clearadd.ClActSet = p_draw->p_clact;
	clearadd.ClActHeader = &p_draw->clheader;
	clearadd.heap = heapID;
	p_wk->p_clearact = CLACT_Add( &clearadd );
	// とりあえず非表示
	CLACT_SetDrawFlag( p_wk->p_clearact, FALSE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画破棄
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDLIST_DrawExit( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
	int i;

	// 消しゴムアクター
	CLACT_Delete( p_wk->p_clearact );

	// 会話うウィンドウ
	GF_BGL_BmpWinDel( &p_wk->talk );
	STRBUF_Delete( p_wk->p_talkstr );

	// メニューデータ破棄
	for( i=0; i<WFNOTE_FRIENDLIST_MENU_NUM; i++ ){
	    BMP_MENULIST_Delete( p_wk->p_menulist[i] );
	}
	GF_BGL_BmpWinDel( &p_wk->menu );

	// メッセージ面破棄
	GF_BGL_BmpWinDel( &p_wk->backmsg );

	// ユニオンキャラクタを読み込む
	WF_2DC_UnionResDel( p_wk->p_charsys );

	// キャラクタデータ初期化
	WF_2DC_SysExit( p_wk->p_charsys );


	//  描画エリア作成
	for( i=0; i<WFNOTE_DRAWAREA_NUM; i++ ){
		WFNOTE_FRIENDLIST_DRAWAREA_Exit( &p_wk->drawdata[ i ], p_draw );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	描画開始関数
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDLIST_SEQ_DrawOn( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	int i;
	
	// タイトル描画
	WFNOTE_DRAW_BmpTitleWrite( p_draw, msg_wifi_note_06 );

	// 戻る描画
	GF_BGL_ScrWriteExpand( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_BACK ],
			WFNOTE_FRIENDLIST_BACK_X,
			WFNOTE_FRIENDLIST_BACK_Y,
			WFNOTE_FRIENDLIST_BACK_SIZX, WFNOTE_FRIENDLIST_BACK_SIZY,
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->rawData, 
			WFNOTE_FRIENDLIST_SCRBACK_X, WFNOTE_FRIENDLIST_SCRBACK_Y, 
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenWidth/8,
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenHeight/8 );

	// 戻るのメッセージ描画
	GF_BGL_BmpWinOnVReq( &p_wk->backmsg );

	// BG面転送
	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_BACK] );


	// 矢印描画
	WFNOTE_DRAW_YazirushiSetDrawFlag( p_draw, TRUE );

	//  最初のページ描画
	WFNOTE_FRIENDLIST_DrawPage( p_wk, p_data, p_draw, p_wk->page, p_wk->pos, WFNOTE_DRAWAREA_MAIN, heapID );

}

//----------------------------------------------------------------------------
/**
 *	@brief	表示OFF　
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDLIST_SEQ_DrawOff( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
	int i;
	
	// タイトルOFF
	WFNOTE_DRAW_BmpTitleOff( p_draw );

	// 戻る非表示
	GF_BGL_BmpWinOffVReq( &p_wk->backmsg );

	// 描画エリアの表示OFF
	for( i=0; i<WFNOTE_DRAWAREA_NUM; i++ ){
		WFNOTE_FRIENDLIST_DRAWAREA_DrawOff( &p_wk->drawdata[i], 
				p_draw, &p_draw->scrn );
	}

	// 矢印描画OFF
	WFNOTE_DRAW_YazirushiSetDrawFlag( p_draw, FALSE );

	// 背景を元に戻す
	// もどる
	GF_BGL_ScrWriteExpand( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_BACK ],
			WFNOTE_FRIENDLIST_BACK_X,
			WFNOTE_FRIENDLIST_BACK_Y,
			WFNOTE_FRIENDLIST_BACK_SIZX, WFNOTE_FRIENDLIST_BACK_SIZY,
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->rawData, 
			WFNOTE_FRIENDLIST_CL_SCRBACK_X, WFNOTE_FRIENDLIST_CL_SCRBACK_Y, 
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenWidth/8,
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenHeight/8 );
	// しおりぶぶん
	GF_BGL_ScrWriteExpand( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_BACK ],
			WFNOTE_FRIENDLIST_SHIORI_X, WFNOTE_FRIENDLIST_SHIORI_Y,
			WFNOTE_FRIENDLIST_SHIORI_SIZX, WFNOTE_FRIENDLIST_SHIORI_SIZY,
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->rawData, 
			WFNOTE_FRIENDLIST_CL_SHIORISCRN_X, WFNOTE_FRIENDLIST_CL_SHIORISCRN_Y, 
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenWidth/8,
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenHeight/8 );
	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_BACK ] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	友達リスト表示	メイン
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *
 *	@retval	WFNOTE_FRIENDLIST_SEQMAINRET_NONE,
 *	@retval	WFNOTE_FRIENDLIST_SEQMAINRET_PAGECHANGE,// ページ変更
 *	@retval	WFNOTE_FRIENDLIST_SEQMAINRET_CANCEL,	// 戻る
 *	@retval	WFNOTE_FRIENDLIST_SEQMAINRET_SELECTPL,	// 人を選択した
 *
 */
//-----------------------------------------------------------------------------
static u32 WFNOTE_FRIENDLIST_SEQ_Main( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
	u16 lastpos;
	BOOL move;
	BOOL pagechang;
	WF_COMMON_WAY way;
	
	if( sys.trg & PAD_BUTTON_DECIDE ){
	
		// キャンセルチェック
		if( p_wk->pos == WFNOTE_FRIENDLIST_CURSORDATA_BACK ){
			Snd_SePlay(WIFINOTE_DECIDE_SE);
			return WFNOTE_FRIENDLIST_SEQMAINRET_CANCEL;
		}
	
		// その位置に友達がいるかチェック
		if( p_data->idx.friendnum <= p_wk->pos + (p_wk->page*WFNOTE_FRIENDLIST_PAGEFRIEND_NUM) ){
			return WFNOTE_FRIENDLIST_SEQMAINRET_NONE;	// いない
		}
		
		// 友達の情報を表示する
		Snd_SePlay(WIFINOTE_DECIDE_SE);
		return WFNOTE_FRIENDLIST_SEQMAINRET_SELECTPL;
	}

	if( sys.trg & PAD_BUTTON_CANCEL ){
		// モード選択に戻る
        Snd_SePlay(WIFINOTE_BS_SE);
		return WFNOTE_FRIENDLIST_SEQMAINRET_CANCEL;
	}

	lastpos = p_wk->pos;	// 前の座標を保存しておく	もどる　からリストにカーソルが移動するときに使用する
	move = FALSE;
	pagechang = FALSE;
	
	if( sys.repeat & PAD_KEY_UP ){
		if( p_wk->pos == WFNOTE_FRIENDLIST_CURSORDATA_BACK ){
			//  戻る位置から上に行くときは１つ前にいたところに戻る
			p_wk->pos  = p_wk->lastpos;
			move = TRUE;
			WFNOTE_DRAW_YazirushiSetDrawFlag( p_draw, TRUE );
		}else{
			if( p_wk->pos < WFNOTE_FRIENDLIST_PAGEFRIEND_HALF ){
				if( p_wk->pos > 0 ){
					p_wk->pos --;
					move = TRUE;
				}
			}else{
				if( p_wk->pos > WFNOTE_FRIENDLIST_PAGEFRIEND_HALF ){
					p_wk->pos --;
					move =  TRUE;
				}
			}
		}
	}else if( sys.repeat & PAD_KEY_DOWN ){
		if( (p_wk->pos == 3) || (p_wk->pos == 7) ){
			// 一番したの項目にいて下が押されたらBACKに移動
			p_wk->pos = WFNOTE_FRIENDLIST_CURSORDATA_BACK;
			move = TRUE;
			// 矢印を消す
			WFNOTE_DRAW_YazirushiSetDrawFlag( p_draw, FALSE );
		}else{
			
			if( p_wk->pos < WFNOTE_FRIENDLIST_PAGEFRIEND_HALF ){
				if( p_wk->pos < 3 ){
					p_wk->pos ++;
					move = TRUE;
				}
			}else{
				if( p_wk->pos < 7 ){
					p_wk->pos ++;
					move =  TRUE;
				}
			}
		}
	}else if( sys.repeat & PAD_KEY_LEFT ){
		// BACKの時は左は無効
		if( p_wk->pos != WFNOTE_FRIENDLIST_CURSORDATA_BACK ){
			if( p_wk->pos < WFNOTE_FRIENDLIST_PAGEFRIEND_HALF ){
				p_wk->pos += WFNOTE_FRIENDLIST_PAGEFRIEND_HALF;	
				move = TRUE;
				pagechang = TRUE;
				way = WF_COMMON_LEFT;
			}else{
				p_wk->pos -= WFNOTE_FRIENDLIST_PAGEFRIEND_HALF;	
				move = TRUE;
			}
		}
	}else if( sys.repeat & PAD_KEY_RIGHT ){
		// BACKの時は→は無効
		if( p_wk->pos != WFNOTE_FRIENDLIST_CURSORDATA_BACK ){
			if( p_wk->pos < WFNOTE_FRIENDLIST_PAGEFRIEND_HALF ){
				p_wk->pos += WFNOTE_FRIENDLIST_PAGEFRIEND_HALF;	
				move = TRUE;
			}else{
				p_wk->pos -= WFNOTE_FRIENDLIST_PAGEFRIEND_HALF;	
				move = TRUE;
				pagechang = TRUE;
				way = WF_COMMON_RIGHT;
			}
		}
	}

	if( pagechang == TRUE ){
		// スクロール処理へ移行する
		if(way == WF_COMMON_LEFT){
			return WFNOTE_FRIENDLIST_SEQMAINRET_PAGECHANGELEFT;
		}
		return WFNOTE_FRIENDLIST_SEQMAINRET_PAGECHANGERIGHT;
	}else{
		if( move == TRUE ){

			Snd_SePlay( WIFINOTE_MOVE_SE );
			
			// カーソルが動いた
			WFNOTE_FRIENDLIST_DRAWAREA_CursorWrite( &p_wk->drawdata[WFNOTE_DRAWAREA_MAIN], p_draw, p_wk->pos );
			p_wk->lastpos = lastpos;
		}
	}
	return WFNOTE_FRIENDLIST_SEQMAINRET_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	スクロールシーケンス初期化
 *	
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDLIST_SEQ_ScrllInit( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	u16 draw_area;

	if( p_wk->way == WF_COMMON_LEFT ){
		draw_area = WFNOTE_DRAWAREA_LEFT;
	}else{
		draw_area = WFNOTE_DRAWAREA_RIGHT;
	}
	
	// スクロールする先に情報を記入
	WFNOTE_FRIENDLIST_DrawPage( p_wk, p_data, p_draw, p_wk->page, p_wk->pos, draw_area, heapID );

	// カーソルを消す
	WFNOTE_FRIENDLIST_DRAWAREA_CursorWrite( &p_wk->drawdata[ WFNOTE_DRAWAREA_MAIN ], p_draw, WFNOTE_FRIENDLIST_CURSORDATA_OFF );

	// 矢印をとめる
	WFNOTE_DRAW_YazirushiSetAnmFlag( p_draw, FALSE );

	// スクロール音
	Snd_SePlay( WIFINOTE_SCRLL_SE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	スクロールシーケンス	メイン
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
static BOOL WFNOTE_FRIENDLIST_SEQ_ScrllMain( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	u32 draw_area;
	s32 scrll_x;
	NNSG2dViewRect rect;
	u32 idx;
	static void (*pSHIORIFUNC[ 4 ])( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DRAW* p_draw, u32 page0, u32 page1 ) = {
		WFNOTE_FRIENDLIST_DrawShioriEff00,
		NULL,
		WFNOTE_FRIENDLIST_DrawShioriEff01,
		NULL
	};

	// カウンタが終わったらすべてを描画して終わる
	if( p_wk->count >= WFNOTE_FIRENDLIST_SCRLL_COUNT ){

		// スクロール座標を元に戻す
		GF_BGL_ScrollReq( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_SCROLL ], GF_BGL_SCROLL_X_SET, 0 );
		GF_BGL_ScrollReq( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_SCRMSG ], GF_BGL_SCROLL_X_SET, 0 );
		// OAMにも反映
		rect = s_SCRLLSURFACE_RECT;
		RNDP_SetSurfaceRect( &p_draw->scrll_surface, &rect );

		// 作業用グラフィックを消す
		if( p_wk->way == WF_COMMON_LEFT ){
			draw_area = WFNOTE_DRAWAREA_LEFT;
		}else{
			draw_area = WFNOTE_DRAWAREA_RIGHT;
		}
		WFNOTE_FRIENDLIST_DRAWAREA_DrawOff( &p_wk->drawdata[ draw_area ], p_draw, &p_draw->scrn );

		// メイン面に表示
		WFNOTE_FRIENDLIST_DrawPage( p_wk, p_data, p_draw, p_wk->page, p_wk->pos, WFNOTE_DRAWAREA_MAIN, heapID );

		// 矢印動作開始
		WFNOTE_DRAW_YazirushiSetAnmFlag( p_draw, TRUE );
		return TRUE;
	}
	
	scrll_x = (p_wk->count * WFNOTE_FRIENDLIST_SCRLL_SIZX) / WFNOTE_FIRENDLIST_SCRLL_COUNT;

	if( p_wk->way == WF_COMMON_LEFT ){
		scrll_x = -scrll_x;
	}

	//  スクロール座標設定
	GF_BGL_ScrollReq( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_SCROLL ], GF_BGL_SCROLL_X_SET, scrll_x );
	GF_BGL_ScrollReq( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_SCRMSG ], GF_BGL_SCROLL_X_SET, scrll_x );

	// OAMにも反映
	rect = s_SCRLLSURFACE_RECT;
	rect.posTopLeft.x += scrll_x << FX32_SHIFT;
	RNDP_SetSurfaceRect( &p_draw->scrll_surface, &rect );

	// アニメーション
	if( p_wk->count % (WFNOTE_FIRENDLIST_SCRLL_COUNT/4) ){
		idx = p_wk->count / (WFNOTE_FIRENDLIST_SCRLL_COUNT/4);
		if( pSHIORIFUNC[idx] ){
			pSHIORIFUNC[idx]( p_wk, p_draw, p_wk->page, p_wk->lastpage );
		}
	}

	// カウント
	p_wk->count ++;
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	メニュー表示	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDLIST_SEQ_MenuInit( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	BMPLIST_HEADER list_h;
	WIFI_LIST* p_wifilist;
	u32 sex;
	u32 listidx;

	// wifiリストから性別を取得
	p_wifilist = SaveData_GetWifiListData( p_data->p_save );
	listidx = WFNOTE_FRIENDLIST_FRIDX_IdxGet( p_wk );
	sex = WifiList_GetFriendInfo( p_wifilist, p_data->idx.fridx[ listidx ], WIFILIST_FRIEND_SEX );
	

	list_h = WFNOTE_FRINEDLIST_MENUHEADER;

	if( sex == PM_NEUTRAL ){	
		list_h.list = p_wk->p_menulist[ WFNOTE_FRIENDLIST_MENU_CODE ];
	}else{
		list_h.list = p_wk->p_menulist[ WFNOTE_FRIENDLIST_MENU_NML ];
	}
	list_h.win = &p_wk->menu;
	
	// メニューの表示
	p_wk->p_listwk = BmpListSet( &list_h, 0, 0, heapID );
    BmpMenuWinWrite( &p_wk->menu, WINDOW_TRANS_OFF, WFNOTE_BGCGX_MAIN_MSG_MENU, WFNOTE_BGPL_MAIN_MENUWIN );
	GF_BGL_BmpWinOnVReq( &p_wk->menu );

	// メッセージ表示
	WFNOTE_DRAW_FriendNameSetWordset( p_draw, p_data->p_save, p_data->idx.fridx[ listidx ], heapID );
	WFNOTE_FRIENDLIST_TalkMsgWrite( p_wk, p_draw, msg_wifi_note_13, heapID );
	// やじるし非表示
	WFNOTE_DRAW_YazirushiSetDrawFlag( p_draw, FALSE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	メニュー表示	選択待ち
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *	@param	heapID		ヒープID
 *
 *	@return	次に進んでほしいシーケンス
 */
//-----------------------------------------------------------------------------
static u32 WFNOTE_FRIENDLIST_SEQ_MenuWait( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	u32 ret;
	u32 ret_seq = WFNOTE_FRIENDLIST_SEQ_MENUWAIT;
	
	
	// メッセージ終了待ち
	if( WFNOTE_FRIENDLIST_TalkMsgEndCheck( p_wk ) == FALSE ){
		return ret_seq;
	}

	// リスト選択
	ret = BmpListMain( p_wk->p_listwk ); 

    switch(ret){
    case BMPLIST_NULL:
		return ret_seq;
    case BMPLIST_CANCEL:
        Snd_SePlay(WIFINOTE_MENU_BS_SE);
		WFNOTE_DRAW_YazirushiSetDrawFlag( p_draw, TRUE );
		
		ret_seq = WFNOTE_FRIENDLIST_SEQ_MAIN;
        break;
    default:
        Snd_SePlay(WIFINOTE_MENU_DECIDE_SE);
		ret_seq = ret;
        break;
    }
    WFNOTE_FRIENDLIST_TalkMsgOff( p_wk );
    BmpMenuWinClear( &p_wk->menu, WINDOW_TRANS_OFF );
	GF_BGL_BmpWinOffVReq( &p_wk->menu );
    BmpListExit( p_wk->p_listwk, NULL, NULL);

	// MAINにもどるので矢印を出す
	if( ret_seq == WFNOTE_FRIENDLIST_SEQ_MAIN ){
		WFNOTE_DRAW_YazirushiSetDrawFlag( p_draw, TRUE );
	}
	
	return ret_seq;
}

//----------------------------------------------------------------------------
/**
 *	@brief	コード表示	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	表示できた
 *	@retval	FALSE	表示できない
 */	
//-----------------------------------------------------------------------------
static BOOL WFNOTE_FRIENDLIST_SEQ_CodeInit( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	WIFI_LIST* p_wifilist;
	DWCFriendData* p_frienddata;
	u64 code;
	u32 listidx;

	p_wifilist = SaveData_GetWifiListData( p_data->p_save );
	listidx = WFNOTE_FRIENDLIST_FRIDX_IdxGet( p_wk );
	p_frienddata = WifiList_GetDwcDataPtr( p_wifilist,
			p_data->idx.fridx[ listidx ] ); 
	code = DWC_GetFriendKey( p_frienddata );
	if(code!=0){
		// コード表示
		Snd_SePlay(WIFINOTE_DECIDE_SE);
		WFNOTE_DRAW_FriendCodeSetWordset( p_draw, code );
		WFNOTE_DRAW_FriendNameSetWordset( p_draw, p_data->p_save, p_data->idx.fridx[ listidx ], heapID );
		WFNOTE_FRIENDLIST_TalkMsgWrite( p_wk, p_draw, msg_wifi_note_32, heapID );
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	破棄処理	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDLIST_SEQ_DeleteInit( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	u32 listidx;

	listidx = WFNOTE_FRIENDLIST_FRIDX_IdxGet( p_wk );
	WFNOTE_DRAW_FriendNameSetWordset( p_draw, p_data->p_save, p_data->idx.fridx[ listidx ], heapID );
	WFNOTE_FRIENDLIST_TalkMsgWrite( p_wk, p_draw, msg_wifi_note_29, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	YesNoウィンドウの表示
 *
 *	@param	p_wk	ワーク
 *	@param	p_data	データ
 *	@param	p_draw	描画システム
 *	@param	heapID	ヒープID
 *
 *	@retval	TRUE	終了
 *	@retval	TRUE	途中
 */
//-----------------------------------------------------------------------------
static BOOL WFNOTE_FRIENDLIST_SEQ_DeleteYesNoDraw( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	if( WFNOTE_FRIENDLIST_TalkMsgEndCheck( p_wk ) == FALSE ){
		return FALSE;
	}

	// YesNoウィンドウ設定
	p_wk->p_yesno =
		BmpYesNoSelectInit( p_draw->p_bgl,
							&WFNOTE_MODESELECT_YESNOWIN,
							WFNOTE_BGCGX_MAIN_MSG_MENU, WFNOTE_BGPL_MAIN_MENUWIN,
							heapID );
	return TRUE;	
}

//----------------------------------------------------------------------------
/**
 *	@brief	破棄処理　YesNo選択後破棄処理
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *	@param	heapID		ヒープID
 *
 *	@retval	YesNo選択の戻り値
 */
//-----------------------------------------------------------------------------
static u32 WFNOTE_FRIENDLIST_SEQ_DeleteWait( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	return  TRUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	ページ描画関数
 *	
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *	@param	page		ページ
 *	@param	pos			カーソル位置
 *	@param	draw_area	描画エリア
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDLIST_DrawPage( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 page, u32 pos, u32 draw_area, u32 heapID )
{
	// メイン面への描画かチェック
	if( WFNOTE_DRAWAREA_MAIN == draw_area ){
		// ページとカーソル変更
		p_wk->page = page;
		p_wk->pos = pos;

		// ページに対応したしおりを表示
		WFNOTE_FRIENDLIST_DrawShiori( p_wk, p_draw, page );
	}

	// 背景を描画しなおす
	WFNOTE_FRIENDLIST_DRAWAREA_BackWrite( &p_wk->drawdata[draw_area], 
			p_draw, &p_draw->scrn );

	// ページデータ描画
	WFNOTE_FRIENDLIST_DRAWAREA_WritePlayer( &p_wk->drawdata[draw_area], p_data, 
			p_draw, p_wk->p_charsys, &p_draw->scrn, &p_data->idx, page, heapID );

	// カーソル描画
	WFNOTE_FRIENDLIST_DRAWAREA_CursorWrite( &p_wk->drawdata[draw_area], p_draw, pos );
}

//----------------------------------------------------------------------------
/**
 *	@brief	しおりの表示
 *	
 *	@param	p_wk	ワーク
 *	@param	p_draw	描画システム
 *	@param	page	ページ数
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDLIST_DrawShiori( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DRAW* p_draw, u32 page )
{
	u32 x;
	
	// 大本をまず描画
	GF_BGL_ScrWriteExpand( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_BACK ],
			WFNOTE_FRIENDLIST_SHIORI_X, WFNOTE_FRIENDLIST_SHIORI_Y,
			WFNOTE_FRIENDLIST_SHIORI_SIZX, WFNOTE_FRIENDLIST_SHIORI_SIZY,
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->rawData, 
			WFNOTE_FRIENDLIST_SHIORISCRN_X, WFNOTE_FRIENDLIST_SHIORISCRN_Y, 
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenWidth/8,
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenHeight/8 );

	// 今のページのしおりを描画
	x = WFNOTE_FRIENDLIST_SHIORI_X + (WFNOTE_FRIENDLIST_SHIORI_ONEX*page) + WFNOTE_FRIENDLIST_SHIORI_OFSX;
	GF_BGL_ScrWriteExpand( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_BACK ],
			x, WFNOTE_FRIENDLIST_SHIORI_Y + WFNOTE_FRIENDLIST_SHIORI_OFSY,
			WFNOTE_FRIENDLIST_SHIORIPOS_SIZ, WFNOTE_FRIENDLIST_SHIORIPOS_SIZ,
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->rawData, 
			WFNOTE_FRIENDLIST_SHIORISCRNPOS_X, sc_POSSCRNY[page], 
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenWidth/8,
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenHeight/8 );

	// BG面転送
	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_BACK] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	しおりアニメ　シーン０
 *
 *	@param	p_wk		ワーク
 *	@param	p_draw		データ
 *	@param	page0		ページ１
 *	@param	page1		ページ２
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDLIST_DrawShioriEff00( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DRAW* p_draw, u32 page0, u32 page1 )
{
	u32 x;

	x = WFNOTE_FRIENDLIST_SHIORI_X + (WFNOTE_FRIENDLIST_SHIORI_ONEX*page1) + WFNOTE_FRIENDLIST_SHIORI_OFSX;
	GF_BGL_ScrWriteExpand( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_BACK ],
			x, WFNOTE_FRIENDLIST_SHIORI_Y + WFNOTE_FRIENDLIST_SHIORI_OFSY,
			WFNOTE_FRIENDLIST_SHIORIPOS_SIZ, WFNOTE_FRIENDLIST_SHIORIPOS_SIZ,
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->rawData, 
			WFNOTE_FRIENDLIST_EFFECTSHIORISCRNPOS_X, sc_POSSCRNY[page1], 
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenWidth/8,
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenHeight/8 );

	// BG面転送
	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_BACK] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	しおりアニメ　シーン１
 *
 *	@param	p_wk		ワーク
 *	@param	p_draw		データ
 *	@param	page0		ページ１
 *	@param	page1		ページ２
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDLIST_DrawShioriEff01( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DRAW* p_draw, u32 page0, u32 page1 )
{
	u32 x;
	
	// 大本をまず描画
	GF_BGL_ScrWriteExpand( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_BACK ],
			WFNOTE_FRIENDLIST_SHIORI_X, WFNOTE_FRIENDLIST_SHIORI_Y,
			WFNOTE_FRIENDLIST_SHIORI_SIZX, WFNOTE_FRIENDLIST_SHIORI_SIZY,
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->rawData, 
			WFNOTE_FRIENDLIST_SHIORISCRN_X, WFNOTE_FRIENDLIST_SHIORISCRN_Y, 
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenWidth/8,
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenHeight/8 );

	// 今のページのしおりを描画
	x = WFNOTE_FRIENDLIST_SHIORI_X + (WFNOTE_FRIENDLIST_SHIORI_ONEX*page0) + WFNOTE_FRIENDLIST_SHIORI_OFSX;
	GF_BGL_ScrWriteExpand( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_BACK ],
			x, WFNOTE_FRIENDLIST_SHIORI_Y + WFNOTE_FRIENDLIST_SHIORI_OFSY,
			WFNOTE_FRIENDLIST_SHIORIPOS_SIZ, WFNOTE_FRIENDLIST_SHIORIPOS_SIZ,
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->rawData, 
			WFNOTE_FRIENDLIST_SHIORISCRNPOS_X, sc_POSSCRNY[page0], 
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenWidth/8,
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenHeight/8 );

	// BG面転送
	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_BACK] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	スクロールシーケンスに変更する
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	way			方向
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDLIST_ScrSeqChange( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DATA* p_data, WF_COMMON_WAY way )
{
	p_data->subseq = WFNOTE_FRIENDLIST_SEQ_SCRLLINIT;
	p_wk->way = way;
	p_wk->count = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージ表示
 *
 *	@param	p_wk		ワーク
 *	@param	p_draw		描画システム
 *	@param	msgid		メッセージID
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDLIST_TalkMsgWrite( WFNOTE_FRIENDLIST* p_wk, WFNOTE_DRAW* p_draw, u32 msgid, u32 heapID )
{
	STRBUF* p_tmp;

	// クリア
	GF_BGL_BmpWinDataFill( &p_wk->talk, 15 );

	p_tmp = STRBUF_Create( WFNOTE_STRBUF_SIZE, heapID );

	MSGMAN_GetString( p_draw->p_msgman, msgid, p_tmp );
	WORDSET_ExpandStr( p_draw->p_wordset, p_wk->p_talkstr, p_tmp );
    p_wk->msgidx = GF_STR_PrintColor(&p_wk->talk, FONT_TALK, 
			p_wk->p_talkstr, 0, 0, p_wk->msgspeed, WFNOTE_COL_BLACK, NULL);
    BmpTalkWinWrite( &p_wk->talk, WINDOW_TRANS_OFF, 
			WFNOTE_BGCGX_MAIN_MSG_TALK, WFNOTE_BGPL_MAIN_TALKWIN );

    GF_BGL_BmpWinOnVReq(&p_wk->talk);

	STRBUF_Delete( p_tmp );
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージ表示終了待ち
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
static BOOL WFNOTE_FRIENDLIST_TalkMsgEndCheck( const WFNOTE_FRIENDLIST* cp_wk )
{
    if( GF_MSG_PrintEndCheck( cp_wk->msgidx ) == 0 ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージを非表示にする
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDLIST_TalkMsgOff( WFNOTE_FRIENDLIST* p_wk )
{
	BmpTalkWinClear( &p_wk->talk, WINDOW_TRANS_OFF );
    GF_BGL_BmpWinOffVReq(&p_wk->talk);

	// 戻るの文字が消えるので表示
	GF_BGL_BmpWinOnVReq( &p_wk->backmsg );
}

//----------------------------------------------------------------------------
/**
 *	@brief	現在選択中の友達リストインデックスを取得
 *
 *	@param	cp_wk		ワーク
 *
 *	@return	友達リストインデックス
 */
//-----------------------------------------------------------------------------
static u32 WFNOTE_FRIENDLIST_FRIDX_IdxGet( const WFNOTE_FRIENDLIST* cp_wk )
{
	u32 num;
	num = cp_wk->page * WFNOTE_FRIENDLIST_PAGEFRIEND_NUM;
	num += cp_wk->pos;
	return num;
}

//----------------------------------------------------------------------------
/**
 *	@brief	メニューの１行移動ごとのコールバック
 *
 *	@param	p_wk		ワーク
 *	@param	param		パラメータ
 *	@param	mode		モード
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDLIST_MENULISTCallBack( BMPLIST_WORK * p_wk, u32 param, u8 mode )
{
	if( mode == 0 ){
		Snd_SePlay(WIFINOTE_MENU_MOVE_SE);
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	リスト破棄アニメ
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDLIST_DeleteAnmStart( WFNOTE_FRIENDLIST* p_wk )
{
	VecFx32 vec;
	u32 pos;

	
	// 表示ON
	CLACT_SetDrawFlag( p_wk->p_clearact, TRUE );

	// アニメ初期
	CLACT_AnmChg( p_wk->p_clearact, 2 );
	p_wk->last_frame = 0;

	// オートアニメ設定
	CLACT_SetAnmFlag( p_wk->p_clearact, TRUE );

	pos = p_wk->pos;

	// ポジションにあわせて座標を設定
	vec.x = ((MATH_ABS(sc_SCRNAREA[ WFNOTE_DRAWAREA_MAIN ].scrn_x)+sc_WFNOTE_FRIENDLIST_CURSORDATA[pos].scrn_data.scrn_x) * 8);
	vec.y = ((MATH_ABS(sc_SCRNAREA[ WFNOTE_DRAWAREA_MAIN ].scrn_y)+sc_WFNOTE_FRIENDLIST_CURSORDATA[pos].scrn_data.scrn_y) * 8) + WFNOTE_FRIENDLIST_CLEARACT_Y;
	vec.x <<= FX32_SHIFT;
	vec.y <<= FX32_SHIFT;
	CLACT_SetMatrix( p_wk->p_clearact, &vec );

	// ウィンドウマスク設定ON
	G2_SetWndOBJInsidePlane( GX_WND_PLANEMASK_BG0|GX_WND_PLANEMASK_BG1|GX_WND_PLANEMASK_BG2, FALSE );
	G2_SetWndOutsidePlane( GX_WND_PLANEMASK_BG0|GX_WND_PLANEMASK_BG1|GX_WND_PLANEMASK_BG2|GX_WND_PLANEMASK_BG3|GX_WND_PLANEMASK_OBJ, TRUE );
	GX_SetVisibleWnd( GX_WNDMASK_OW );//*/

	// 消しゴム音
	Snd_SePlay( WIFINOTE_CLEAN_SE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	リスト破棄アニメ	メイン
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static BOOL WFNOTE_FRIENDLIST_DeleteAnmMain( WFNOTE_FRIENDLIST* p_wk )
{
	u32 frame;
	
	if( CLACT_AnmActiveCheck( p_wk->p_clearact ) == FALSE ){
		return TRUE;
	}

	// アニメが切り替わったら消しゴム音
	frame = CLACT_AnmFrameGet( p_wk->p_clearact );
	if( (frame - p_wk->last_frame) >= 2 ){
		p_wk->last_frame = frame;
		Snd_SePlay( WIFINOTE_CLEAN_SE );
	}
	
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	アニメ表示状態を消す
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDLIST_DeleteAnmOff( WFNOTE_FRIENDLIST* p_wk )
{
	GX_SetVisibleWnd( GX_WNDMASK_NONE );
	CLACT_SetDrawFlag( p_wk->p_clearact, FALSE );
	CLACT_SetAnmFlag( p_wk->p_clearact, FALSE );
}


//----------------------------------------------------------------------------
/**
 *	@brief	描画エリアワーク	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_draw		描画システム
 *	@param	cp_area		スクリーンエリア情報
 *	@param	text_cgx	テキスト面のｃｇｘ
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDLIST_DRAWAREA_Init( WFNOTE_FRIENDLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, const WFNOTE_SCRNAREA* cp_area, u32 text_cgx, u32 heapID )
{
	// スクリーンデータ保存
	p_wk->scrn_area = *cp_area;

	// ビットマップ
	GF_BGL_BmpWinInit( &p_wk->text );
	GF_BGL_BmpWinAdd( p_draw->p_bgl, &p_wk->text, 
			sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_SCRMSG ], 
			WFNOTE_FRIENDLIST_TEXT_OFSX+MATH_ABS(cp_area->scrn_x), 
			WFNOTE_FRIENDLIST_TEXT_OFSY+MATH_ABS(cp_area->scrn_y),
			WFNOTE_FRIENDLIST_TEXT_SIZX, WFNOTE_FRIENDLIST_TEXT_SIZY,
			WFNOTE_BGPL_MAIN_MSGFONT, text_cgx );
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画エリアワーク	破棄
 *
 *	@param	p_wk		ワーク
 *	@param	p_draw		描画システム
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDLIST_DRAWAREA_Exit( WFNOTE_FRIENDLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw )
{
	GF_BGL_BmpWinDel( &p_wk->text );
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画エリアに背景描画
 *
 *	@param	p_wk		ワーク
 *	@param	p_draw		描画システム
 *	@param	cp_scrn		スクリーンデータ
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDLIST_DRAWAREA_BackWrite( WFNOTE_FRIENDLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, const WFNOTE_SCRNDATA* cp_scrn )
{
	// 背景描画
	GF_BGL_ScrWrite( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_SCROLL ],
			cp_scrn->p_scrn[WFNOTE_SCRNDATA_BACK]->rawData, 
			MATH_ABS(p_wk->scrn_area.scrn_x), MATH_ABS(p_wk->scrn_area.scrn_y), 
			cp_scrn->p_scrn[WFNOTE_SCRNDATA_BACK]->screenWidth/8,
			cp_scrn->p_scrn[WFNOTE_SCRNDATA_BACK]->screenHeight/8 );

	// BG面転送
	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_SCROLL] );
	
}

//----------------------------------------------------------------------------
/**
 *	@brief	カーソル描画
 *
 *	@param	p_wk		ワーク
 *	@param	p_draw		描画システム
 *	@param	pos			カーソル位置
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDLIST_DRAWAREA_CursorWrite( WFNOTE_FRIENDLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, u32 pos )
{
	int i;
	int pal;

	for( i=0; i<WFNOTE_FRIENDLIST_CURSORDATA_NUM; i++ ){
		if( pos == i ){
			pal = WFNOTE_BGPL_MAIN_BACK_6;
		}else{
			pal = WFNOTE_BGPL_MAIN_BACK_0;
		}

		// パレットカラーを合わせる
		GF_BGL_ScrPalChange( p_draw->p_bgl, 
				sc_WFNOTE_FRIENDLIST_CURSORDATA[i].bg_frame,
				MATH_ABS(p_wk->scrn_area.scrn_x) + sc_WFNOTE_FRIENDLIST_CURSORDATA[i].scrn_data.scrn_x,
				MATH_ABS(p_wk->scrn_area.scrn_y) + sc_WFNOTE_FRIENDLIST_CURSORDATA[i].scrn_data.scrn_y,
				sc_WFNOTE_FRIENDLIST_CURSORDATA[i].scrn_data.scrn_sizx,
				sc_WFNOTE_FRIENDLIST_CURSORDATA[i].scrn_data.scrn_sizy, pal );
	}

	// BG面転送
	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_SCROLL] );
	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_BACK] );
	
	
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤー情報表示
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *	@param	p_charsys	キャラクタシステム
 *	@param	cp_scrn		スクリーンシステム
 *	@param	cp_idx		インデックスデータ
 *	@param	page		ページ
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDLIST_DRAWAREA_WritePlayer( WFNOTE_FRIENDLIST_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, WF_2DCSYS* p_charsys, const WFNOTE_SCRNDATA* cp_scrn, const WFNOTE_IDXDATA* cp_idx, u32 page, u32 heapID )
{
	int i;
	int idx;
	WIFI_LIST* p_list;

	p_list = SaveData_GetWifiListData( p_data->p_save );
	
	// クリア
	GF_BGL_BmpWinDataFill( &p_wk->text, 0 );

	// アクターが残っていたらはき
	WFNOTE_FRIENDLIST_DRAWAREA_CharWkDel( p_wk, p_draw );
	
	// 名前とアクターの生成
	idx = page * WFNOTE_FRIENDLIST_PAGEFRIEND_NUM;
	for( i=0; i<WFNOTE_FRIENDLIST_PAGEFRIEND_NUM; i++ ){
		if( cp_idx->friendnum > (idx+i) ){
			// 表示する
			WFNOTE_FRIENDLIST_DRAWAREA_SetPlayer( p_wk, p_charsys, p_draw, cp_scrn, i, p_list, cp_idx->fridx[idx+i],  heapID );
		}else{
			// プレイヤーは表示しない
			WFNOTE_FRIENDLIST_DRAWAREA_SetNoPlayer( p_wk, p_draw, cp_scrn, i );
		}
	}

	// 書き込んだBGを更新
	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_SCROLL ] );

	// テキスト面表示
	GF_BGL_BmpWinOnVReq( &p_wk->text );
}

//----------------------------------------------------------------------------
/**
 *	@brief	表示OFF
 *
 *	@param	p_wk		ワーク
 *	@param	p_draw		描画システム
 *	@param	cp_scrn		スクリーンデータ
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDLIST_DRAWAREA_DrawOff( WFNOTE_FRIENDLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, const WFNOTE_SCRNDATA* cp_scrn )
{
	// アクター破棄
	WFNOTE_FRIENDLIST_DRAWAREA_CharWkDel( p_wk, p_draw );

	// 表示OFF
	GF_BGL_BmpWinOffVReq( &p_wk->text );

	// 背景クリア
	GF_BGL_ScrFill( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_SCROLL ],
			0, 
			MATH_ABS(p_wk->scrn_area.scrn_x), MATH_ABS(p_wk->scrn_area.scrn_y), 
			p_wk->scrn_area.scrn_sizx,
			p_wk->scrn_area.scrn_sizy, 0 );

	// BG面転送
	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_SCROLL] );

	// スクリーン面もクリア
	GF_BGL_ScrFill( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_SCRMSG ],
			0, 
			MATH_ABS(p_wk->scrn_area.scrn_x), MATH_ABS(p_wk->scrn_area.scrn_y), 
			p_wk->scrn_area.scrn_sizx,
			p_wk->scrn_area.scrn_sizy, 0 );
	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_SCRMSG] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	一気にキャラクタワークを破棄する
 *
 *	@param	p_wk		ワーク
 *	@param	p_draw		描画システム
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDLIST_DRAWAREA_CharWkDel( WFNOTE_FRIENDLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw )
{
	int i;

	for( i=0; i<WFNOTE_FRIENDLIST_PAGEFRIEND_NUM; i++ ){
		if( p_wk->p_clwk[ i ] ){
			WF_2DC_WkDel( p_wk->p_clwk[ i ] );
			p_wk->p_clwk[ i ] = NULL;
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーデータ表示
 *
 *	@param	p_wk		ワーク
 *	@param	p_charsys	キャラクタシステム
 *	@param	p_draw		描画システム
 *	@param	cp_scrn		スクリーンデータ
 *	@param	pos			位置
 *	@param	p_list		WiFiリスト
 *	@param	idx			WiFiリスト内のインデックス
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDLIST_DRAWAREA_SetPlayer( WFNOTE_FRIENDLIST_DRAWAREA* p_wk, WF_2DCSYS* p_charsys, WFNOTE_DRAW* p_draw, const WFNOTE_SCRNDATA* cp_scrn, u32 pos, WIFI_LIST* p_list, u32 idx, u32 heapID )
{
	int sex;
	int view;
	WF_2DC_WKDATA adddata;
	STRBUF* p_str;
	int x, y;
    static const GF_PRINTCOLOR sc_SEXCOL[] = {WFNOTE_COL_BLUE,WFNOTE_COL_RED,WFNOTE_COL_BLACK}; 

	view = WifiList_GetFriendInfo( p_list, idx, WIFILIST_FRIEND_UNION_GRA );
	sex = WifiList_GetFriendInfo( p_list, idx, WIFILIST_FRIEND_SEX );

	if( sex == PM_NEUTRAL ){
		// いない絵を表示
		WFNOTE_FRIENDLIST_DRAWAREA_SetNoPlayer( p_wk, p_draw, cp_scrn, pos );
	}else{
		// キャラクタ表示
		adddata.x = ((p_wk->scrn_area.scrn_x+sc_WFNOTE_FRIENDLIST_CURSORDATA[pos].scrn_data.scrn_x) * 8) + WFNOTE_FRIENDLIST_PLAYER_X;
		adddata.y = ((p_wk->scrn_area.scrn_y+sc_WFNOTE_FRIENDLIST_CURSORDATA[pos].scrn_data.scrn_y) * 8) + WFNOTE_FRIENDLIST_PLAYER_Y;
		adddata.pri = WFNOTE_FRIENDLIST_PRI;
		adddata.bgpri = WFNOTE_FRIENDLIST_BGPRI;
		p_wk->p_clwk[ pos ] = WF_2DC_WkAdd( p_charsys, &adddata, view, heapID );
	}

	//  名前表示
	x = ((sc_WFNOTE_FRIENDLIST_CURSORDATA[pos].scrn_data.scrn_x) * 8) + WFNOTE_FRIENDLIST_PLAYERNAME_X;
	y = ((sc_WFNOTE_FRIENDLIST_CURSORDATA[pos].scrn_data.scrn_y) * 8) + WFNOTE_FRIENDLIST_PLAYERNAME_Y;
	p_str = STRBUF_Create( WFNOTE_STRBUF_SIZE, heapID );
    STRBUF_SetStringCode( p_str, WifiList_GetFriendNamePtr(p_list,idx) );
    GF_STR_PrintColor( &p_wk->text, FONT_SYSTEM, 
			p_str,
			x, y, MSG_NO_PUT, sc_SEXCOL[sex], NULL);

	STRBUF_Delete( p_str );
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーのいない状態を表示
 *
 *	@param	p_wk		ワーク
 *	@param	p_draw		描画システム
 *	@param	cp_scrn		スクリーンデータ
 *	@param	pos			位置
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDLIST_DRAWAREA_SetNoPlayer( WFNOTE_FRIENDLIST_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, const WFNOTE_SCRNDATA* cp_scrn, u32 pos )
{
	//  その位置のスクリーンにはてなマークを出す
	GF_BGL_ScrWriteExpand( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_SCROLL ],
			MATH_ABS(p_wk->scrn_area.scrn_x) + sc_WFNOTE_FRIENDLIST_CURSORDATA[pos].scrn_data.scrn_x,
			MATH_ABS(p_wk->scrn_area.scrn_y) + sc_WFNOTE_FRIENDLIST_CURSORDATA[pos].scrn_data.scrn_y,
			WFNOTE_FRIENDLIST_NOTPlAYER_SIZX, WFNOTE_FRIENDLIST_NOTPlAYER_SIZY,
			cp_scrn->p_scrn[WFNOTE_SCRNDATA_KIT]->rawData, 
			WFNOTE_FRIENDLIST_NOTPlAYERSCRN_X, WFNOTE_FRIENDLIST_NOTPlAYERSCRN_Y, 
			cp_scrn->p_scrn[WFNOTE_SCRNDATA_KIT]->screenWidth/8,
			cp_scrn->p_scrn[WFNOTE_SCRNDATA_KIT]->screenHeight/8 );

	// BG面転送
	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_SCROLL] );

}


//-------------------------------------
///	友達コード入力画面ワーク
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	友達コード入力画面	初期化
 *
 *	@param	p_wk		ワーク	
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_CODEIN_Init( WFNOTE_CODEIN* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	WFNOTE_CODEIN_DrawInit( p_wk, p_data, p_draw, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	友達コード入力画面	メイン関数
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		システムワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static WFNOTE_STRET WFNOTE_CODEIN_Main( WFNOTE_CODEIN* p_wk, WFNOTE_WK* p_sys, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	int block[ CODE_BLOCK_MAX ];


	switch( p_data->subseq ){
	case WFNOTE_CODEIN_SEQ_NAMEIN:

		// 名前入力、コード入力パラメータ作成
		p_wk->p_namein = WFNOTE_CODEIN_NameInParamMake( p_wk, p_data, heapID );
		CodeIn_BlockDataMake_4_4_4( block );
        // MatchComment: use sub_2089400 instead of CodeInput_ParamCreate
        p_wk->p_codein = sub_2089400( heapID, FRIENDCODE_MAXLEN, block, SaveData_GetConfig(p_data->p_save), 0, 0 );

		WFNOTE_DrawExit( p_sys );	// 画面ワーク全破棄
        p_wk->p_subproc = PROC_Create( &NameInProcData, p_wk->p_namein, heapID );
		p_data->subseq = WFNOTE_CODEIN_SEQ_NAMEIN_WAIT;
		break;
		
	case WFNOTE_CODEIN_SEQ_NAMEIN_WAIT:
		// 終了待ち
		if( ProcMain( p_wk->p_subproc ) == FALSE ){
			break;
		}
		PROC_Delete( p_wk->p_subproc );

		// キャンセルチェック
		if( p_wk->p_namein->cancel==0 ){
			// 次へ

			p_wk->p_subproc = PROC_Create( &CodeInput_ProcData, p_wk->p_codein, heapID ); 
			p_data->subseq = WFNOTE_CODEIN_SEQ_CODEIN_WAIT;
		}else{
			// 終了へ
			WFNOTE_DATA_SetReqStatus( p_data, WFNOTE_STATUS_MODE_SELECT, WFNOTE_MODESELECT_SEQ_INIT );
			p_data->subseq = WFNOTE_CODEIN_SEQ_END;
		}
		break;
		
	case WFNOTE_CODEIN_SEQ_CODEIN_WAIT:
		// 終了待ち
		if( ProcMain( p_wk->p_subproc ) == FALSE ){
			break;
		}
		PROC_Delete( p_wk->p_subproc );

		// データを設定
		WFNOTE_DATA_CODEIN_Set( p_data, p_wk->p_namein->strbuf, p_wk->p_codein->strbuf );
		WFNOTE_DATA_SetReqStatus( p_data, WFNOTE_STATUS_MODE_SELECT, WFNOTE_MODESELECT_SEQ_CODECHECK_INIT );
		p_data->subseq = WFNOTE_CODEIN_SEQ_END;
		break;
		
	case WFNOTE_CODEIN_SEQ_END:
		// 名前入力、コード入力画面の破棄
		NameIn_ParamDelete( p_wk->p_namein );
		CodeInput_ParamDelete( p_wk->p_codein );

		WFNOTE_DrawInit( p_sys, heapID );	// 画面復帰
		return WFNOTE_STRET_FINISH; 



	case WFNOTE_CODEIN_SEQ_NAMEINONLY:	// 名前入力のみ
		// 名前入力、コード入力パラメータ作成
		p_wk->p_namein = WFNOTE_CODEIN_NameInParamMake( p_wk, p_data, heapID );
		WFNOTE_DrawExit( p_sys );	// 画面ワーク全破棄
        p_wk->p_subproc = PROC_Create( &NameInProcData, p_wk->p_namein, heapID );
		p_data->subseq = WFNOTE_CODEIN_SEQ_NAMEINONLY_WAIT;
		break;
		
	case WFNOTE_CODEIN_SEQ_NAMEINONLY_WAIT:	// 名前入力のみ
		// 終了待ち
		if( ProcMain( p_wk->p_subproc ) == FALSE ){
			break;
		}
		PROC_Delete( p_wk->p_subproc );

		// キャンセルチェック
		if( p_wk->p_namein->cancel==0 ){
			// ちゃんと入力されたので、その場合のFRIENDLISTに戻る
			WFNOTE_DATA_SetReqStatus( p_data, WFNOTE_STATUS_FRIENDLIST, WFNOTE_FRINEDLIST_SEQ_NAMECHG_WAIT );
			// データを設定
			WFNOTE_DATA_CODEIN_Set( p_data, p_wk->p_namein->strbuf, NULL );
			p_data->subseq = WFNOTE_CODEIN_SEQ_NAMEINONLY_END;
		}else{
			// 終了へ
			WFNOTE_DATA_SetReqStatus( p_data, WFNOTE_STATUS_FRIENDLIST, WFNOTE_FRINEDLIST_SEQ_NAMECHG_WAITWIPE );
			p_data->subseq = WFNOTE_CODEIN_SEQ_NAMEINONLY_END;
		}
		break;
		
	case WFNOTE_CODEIN_SEQ_NAMEINONLY_END:	// 名前入力のみ
		// 名前入力、コード入力画面の破棄
		NameIn_ParamDelete( p_wk->p_namein );

		WFNOTE_DrawInit( p_sys, heapID );	// 画面復帰
		return WFNOTE_STRET_FINISH; 
	}
	return WFNOTE_STRET_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	友達コード入力画面	破棄
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 */
//-----------------------------------------------------------------------------
static void WFNOTE_CODEIN_Exit( WFNOTE_CODEIN* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{

	WFNOTE_CODEIN_DrawExit( p_wk, p_data, p_draw );
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画関係の初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *	@param	heapID		ヒープ
 */
//-----------------------------------------------------------------------------
static void WFNOTE_CODEIN_DrawInit( WFNOTE_CODEIN* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画関係の破棄
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム 
 */
//-----------------------------------------------------------------------------
static void WFNOTE_CODEIN_DrawExit( WFNOTE_CODEIN* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
}

//----------------------------------------------------------------------------
/**
 *	@brief	名前入力　ワーク作成
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static NAMEIN_PARAM* WFNOTE_CODEIN_NameInParamMake( WFNOTE_CODEIN* p_wk, WFNOTE_DATA* p_data, u32 heapID )
{
	NAMEIN_PARAM* p_param;

	p_param = NameIn_ParamAllocMake( heapID, NAMEIN_FRIENDNAME, 0, 
							NAMEIN_PERSON_LENGTH, 
							SaveData_GetConfig(p_data->p_save) );

	// 名前入力画面初期化dataチェック
	if( p_data->namein_init.init == TRUE ){
		#if( PM_LANG == LANG_JAPAN )
		{
			if( IsJapaneseCode( *p_data->namein_init.cp_str ) ){
				STRBUF_SetStringCode( p_param->strbuf, p_data->namein_init.cp_str );
			}else{
				STRBUF_Clear( p_param->strbuf );
			}
		}
		#else
		{
			if( IsJapaneseCode( *p_data->namein_init.cp_str ) == FALSE ){
				STRBUF_SetStringCode( p_param->strbuf, p_data->namein_init.cp_str );
			}else{
				STRBUF_Clear( p_param->strbuf );
			}
		}
		#endif
		p_data->namein_init.init = FALSE;
	}
	return p_param;
}

//-------------------------------------
///	自分の友達コード確認画面ワーク
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	自分の友達コード確認画面ワーク初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *	@param	heapID		ヒープ
 */
//-----------------------------------------------------------------------------
static void WFNOTE_MYCODE_Init( WFNOTE_MYCODE* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	WFNOTE_MYCODE_DrawInit( p_wk, p_data, p_draw, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	自分の友達コード確認画面 メイン処理
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *
 *	@retval	WFNOTE_STRET_CONTINUE,	// つづく
 *	@retval	WFNOTE_STRET_FINISH,	// 作業は終わった
 */
//-----------------------------------------------------------------------------
static WFNOTE_STRET WFNOTE_MYCODE_Main( WFNOTE_MYCODE* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
	switch( p_data->subseq ){
	case WFNOTE_MYCODE_SEQ_INIT:
		WFNOTE_MYCODE_SEQ_Init( p_wk, p_data, p_draw );
		p_data->subseq = WFNOTE_MYCODE_SEQ_MAIN;
		break;
		
	case WFNOTE_MYCODE_SEQ_MAIN:
		if( sys.trg & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL) ){
			Snd_SePlay(WIFINOTE_DECIDE_SE);
			WFNOTE_DATA_SetReqStatus( p_data, WFNOTE_STATUS_MODE_SELECT, WFNOTE_MODESELECT_SEQ_INIT_NOFADE );
			WFNOTE_MYCODE_DrawOff( p_wk, p_draw );	// 表示OFF
			return WFNOTE_STRET_FINISH; 
		}
		break;
	}
	return WFNOTE_STRET_CONTINUE; 
}

//----------------------------------------------------------------------------
/**
 *	@brief	自分の友達コード確認画面ワーク破棄
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 */
//-----------------------------------------------------------------------------
static void WFNOTE_MYCODE_Exit( WFNOTE_MYCODE* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
	WFNOTE_MYCODE_DrawExit( p_wk, p_data, p_draw );
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画関係の初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_MYCODE_DrawInit( WFNOTE_MYCODE* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	u64 code;
	DWCUserData* p_userdata;
	WIFI_LIST* p_list;
	STRBUF* p_str;
	STRBUF* p_tmp;
    int x;

	// ビットマップ作成
	GF_BGL_BmpWinInit( &p_wk->msg );
	GF_BGL_BmpWinAdd( p_draw->p_bgl, &p_wk->msg, 
			sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_SCRMSG], 
			WFNOTE_MYCODE_MSG_X, WFNOTE_MYCODE_MSG_Y, 
			WFNOTE_MYCODE_MSG_SIZX, WFNOTE_MYCODE_MSG_SIZY, 
			WFNOTE_BGPL_MAIN_MSGFONT, WFNOTE_MYCODE_CGX );
	GF_BGL_BmpWinInit( &p_wk->code );
	GF_BGL_BmpWinAdd( p_draw->p_bgl, &p_wk->code, 
			sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_SCRMSG], 
			WFNOTE_MYCODE_CODE_X, WFNOTE_MYCODE_CODE_Y, 
			WFNOTE_MYCODE_CODE_SIZX, WFNOTE_MYCODE_CODE_SIZY, 
			WFNOTE_BGPL_MAIN_MSGFONT, WFNOTE_MYCODE_CODE_CGX );

	GF_BGL_BmpWinDataFill( &p_wk->msg, 0 );
	GF_BGL_BmpWinDataFill( &p_wk->code, 0 );

	p_str = STRBUF_Create( WFNOTE_STRBUF_SIZE, heapID );
	p_tmp = STRBUF_Create( WFNOTE_STRBUF_SIZE, heapID );

	// 自分のコード取得
	p_list = SaveData_GetWifiListData(p_data->p_save);
	p_userdata = WifiList_GetMyUserInfo( p_list );
	code = DWC_CreateFriendKey( p_userdata );

	if( code != 0 ){
		// コードあり
		// メッセージ
        MSGMAN_GetString( p_draw->p_msgman, msg_wifi_note_24, p_str );
        x = (WFNOTE_MYCODE_MSG_SIZX*8 - FontProc_GetPrintMaxLineWidth( FONT_SYSTEM, p_str, 0 ))/2;
		GF_STR_PrintColor( &p_wk->msg, FONT_SYSTEM,
						   p_str, x, 0, MSG_NO_PUT, WFNOTE_COL_BLACK, NULL);

		// 数字
		WFNOTE_DRAW_FriendCodeSetWordset( p_draw, code );
        MSGMAN_GetString( p_draw->p_msgman, msg_wifi_note_28, p_tmp );
		WORDSET_ExpandStr( p_draw->p_wordset, p_str, p_tmp );
        x = FontProc_GetPrintCenteredPositionX( FONT_SYSTEM, p_str, 0, WFNOTE_MYCODE_CODE_SIZX*8 );
		GF_STR_PrintColor( &p_wk->code, FONT_SYSTEM,
						   p_str, x, 0,
						   MSG_NO_PUT, WFNOTE_COL_BLACK, NULL);
	}else{
		// コードなし
		// メッセージ
        MSGMAN_GetString( p_draw->p_msgman, msg_wifi_note_25, p_str );
        x = (WFNOTE_MYCODE_MSG_SIZX*8 - FontProc_GetPrintMaxLineWidth( FONT_SYSTEM, p_str, 0 ))/2;
		GF_STR_PrintColor( &p_wk->msg, FONT_SYSTEM,
						   p_str, x, 0, MSG_NO_PUT, WFNOTE_COL_BLACK, NULL);

		// 数字
	}

	STRBUF_Delete( p_str );
	STRBUF_Delete( p_tmp );

	// スクリーンデータ読み込み
	p_wk->p_scrnbuff = ArcUtil_HDL_ScrnDataGet( p_draw->p_handle,
			NARC_pl_wifinote_techo_04_NSCR, FALSE, &p_wk->p_scrn, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画関係の破棄
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 */
//-----------------------------------------------------------------------------
static void WFNOTE_MYCODE_DrawExit( WFNOTE_MYCODE* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
	GF_BGL_BmpWinDel( &p_wk->msg );
	GF_BGL_BmpWinDel( &p_wk->code );
	sys_FreeMemoryEz( p_wk->p_scrnbuff );
}

//----------------------------------------------------------------------------
/**
 *	@brief	初期化　シーケンス
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 */
//-----------------------------------------------------------------------------
static void WFNOTE_MYCODE_SEQ_Init( WFNOTE_MYCODE* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
	
	// タイトル設定
	WORDSET_RegisterPlayerName( p_draw->p_wordset, 0, SaveData_GetMyStatus( p_data->p_save ) );
	WFNOTE_DRAW_BmpTitleWrite( p_draw, msg_wifi_note_23 );

	// 背景設定
	GF_BGL_ScrWrite( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_SCROLL ],
			p_wk->p_scrn->rawData, 0, 0, 
			p_wk->p_scrn->screenWidth/8, p_wk->p_scrn->screenHeight/8 );
	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_SCROLL] );

	// 文字列設定
    GF_BGL_BmpWinOnVReq(&p_wk->msg);
    GF_BGL_BmpWinOnVReq(&p_wk->code);
}

//----------------------------------------------------------------------------
/**
 *	@brief	表示OFF
 *
 *	@param	p_wk		ワーク
 *	@param	p_draw		描画システム
 */
//-----------------------------------------------------------------------------
static void WFNOTE_MYCODE_DrawOff( WFNOTE_MYCODE* p_wk, WFNOTE_DRAW* p_draw )
{
	// タイトル
	WFNOTE_DRAW_BmpTitleOff( p_draw );

	// SCROLL面
	GF_BGL_ScrFill( p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_SCROLL], 0, 0, 0, 32, 24, 0 );
	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_SCROLL] );

	// msg
	GF_BGL_BmpWinOffVReq(&p_wk->msg);
	GF_BGL_BmpWinOffVReq(&p_wk->code);

	
	// SCRMSG面
	GF_BGL_ScrFill( p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_SCRMSG], 0, 0, 0, 32, 24, 0 );
	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_SCRMSG] );
}



//-------------------------------------
///	友達詳細情報
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	友達詳細表示画面	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDINFO_Init( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	// SaveDataからフロンティアを表示するかチェックする
	// 今は常に表示
	p_wk->frontier_draw = SysFlag_ArriveGet(SaveData_GetEventWork(p_data->p_save),FLAG_ARRIVE_D32R0101);
	
	WFNOTE_FRIENDINFO_DrawInit( p_wk, p_data, p_draw, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	友達詳細表示画面	メイン
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static WFNOTE_STRET WFNOTE_FRIENDINFO_Main( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	BOOL result;

	switch( p_data->subseq ){
	case WFNOTE_FRIENDINFO_SEQ_INIT:
		p_wk->rep = FALSE;
		p_wk->page = 0;
	case WFNOTE_FRIENDINFO_SEQ_INITRP:
		// 描画開始
		WFNOTE_FRIENDINFO_DrawOn( p_wk, p_data, p_draw, heapID );
		p_data->subseq = WFNOTE_FRIENDINFO_SEQ_MAIN;
		break;
		
	case WFNOTE_FRIENDINFO_SEQ_MAIN:
		if( sys.trg & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL) ){
			Snd_SePlay( WIFINOTE_DECIDE_SE );
			p_data->subseq = WFNOTE_FRIENDINFO_SEQ_END;
		}else{
			if( sys.repeat & PAD_KEY_LEFT ){
				WFNOTE_FRIENDINFO_PageChange( p_wk, -1 );
				p_data->subseq = WFNOTE_FRIENDINFO_SEQ_SCRINIT;
				p_wk->way = WF_COMMON_LEFT;
			}else if( sys.repeat & PAD_KEY_RIGHT ){
				WFNOTE_FRIENDINFO_PageChange( p_wk, 1 );
				p_data->subseq = WFNOTE_FRIENDINFO_SEQ_SCRINIT;
				p_wk->way = WF_COMMON_RIGHT;
			}else if( sys.cont & PAD_KEY_UP ){
				p_data->subseq = WFNOTE_FRIENDINFO_SEQ_FRIENDCHG;
				p_wk->way = WF_COMMON_TOP;
				p_wk->wait = WFNOTE_FRIENDINFO_PAGECHG_WAIT;
			}else if( sys.cont & PAD_KEY_DOWN ){
				p_data->subseq = WFNOTE_FRIENDINFO_SEQ_FRIENDCHG;
				p_wk->way = WF_COMMON_BOTTOM;
				p_wk->wait = WFNOTE_FRIENDINFO_PAGECHG_WAIT;
			}
		}
		if( (sys.cont & (PAD_KEY_UP|PAD_KEY_DOWN)) == 0 ){
			p_wk->rep = FALSE;
		}
		break;
		
	case WFNOTE_FRIENDINFO_SEQ_SCRINIT:
		WFNOTE_FRIENDINFO_ScrollInit( p_wk, p_data, p_draw, heapID );
		WFNOTE_FRIENDINFO_ScrollMain( p_wk, p_data, p_draw, heapID );
		p_data->subseq = WFNOTE_FRIENDINFO_SEQ_SCR;
		break;
		
	case WFNOTE_FRIENDINFO_SEQ_SCR:
		result = WFNOTE_FRIENDINFO_ScrollMain( p_wk, p_data, p_draw, heapID );
		if( result == TRUE ){
			p_data->subseq = WFNOTE_FRIENDINFO_SEQ_MAIN;
		}
		break;

		// 表示する人をかえる
	case WFNOTE_FRIENDINFO_SEQ_FRIENDCHG:
		if( p_wk->rep ){	// リピート中ならウエイト
			p_wk->wait --;
			if( p_wk->wait > 0 ){
				break;
			}
		}
		
		result = WFNOTE_FRIENDINFO_SelectListIdxAdd( p_data, p_wk->way );
		if( result ){

			Snd_SePlay( WIFINOTE_MOVE_SE );

			WFNOTE_FRIENDINFO_DrawOff( p_wk, p_draw );
			WFNOTE_DATA_SetReqStatus( p_data, WFNOTE_STATUS_FRIENDINFO, WFNOTE_FRIENDINFO_SEQ_INITRP );
			// 次すぐ上下が押されていたらリピート
			p_wk->rep = TRUE;
			return WFNOTE_STRET_FINISH;
		}

		// 変更できなかったので元に戻る
		p_data->subseq = WFNOTE_FRIENDINFO_SEQ_MAIN;
		break;
		
	case WFNOTE_FRIENDINFO_SEQ_END:
		WFNOTE_FRIENDINFO_DrawOff( p_wk, p_draw );
		WFNOTE_DATA_SetReqStatus( p_data, WFNOTE_STATUS_FRIENDLIST, WFNOTE_FRIENDLIST_SEQ_INFORET );
		return WFNOTE_STRET_FINISH;
	}
	return WFNOTE_STRET_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	友達詳細表示画面	破棄
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDINFO_Exit( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
	WFNOTE_FRIENDINFO_DrawExit( p_wk, p_data, p_draw );
}

//----------------------------------------------------------------------------
/**
 *	@brief	スクロール初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDINFO_ScrollInit( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	u32 drawarea;
	
	p_wk->count = 0;

	// 矢印のアニメをとめる
	WFNOTE_DRAW_YazirushiSetAnmFlag( p_draw, FALSE );	

	// スクロール方向にページを書き込む
	if( p_wk->way == WF_COMMON_LEFT ){
		drawarea = WFNOTE_DRAWAREA_LEFT;
	}else{
		drawarea = WFNOTE_DRAWAREA_RIGHT;
	}
	WFNOTE_FRINEDINFO_DrawPage( p_wk, p_data, p_draw, drawarea, p_wk->page, heapID );

	// スクロール音
	Snd_SePlay( WIFINOTE_SCRLL_SE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	スクロールメイン
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
static BOOL WFNOTE_FRIENDINFO_ScrollMain( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	u32 draw_area;
	s32 scrll_x;
	NNSG2dViewRect rect;
	u32 idx;
	static void (*pSHIORIFUNC[ 4 ])( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DRAW* p_draw, u32 page0, u32 page1 ) = {
		WFNOTE_FRIENDINFO_ShioriAnm00,
		NULL,
		WFNOTE_FRIENDINFO_ShioriAnm01,
		NULL
	};

	// カウンタが終わったらすべてを描画して終わる
	if( p_wk->count > WFNOTE_FRIENDINFO_SCRLL_COUNT ){
		

		// スクロール座標を元に戻す
		GF_BGL_ScrollReq( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_SCROLL ], GF_BGL_SCROLL_X_SET, 0 );
		GF_BGL_ScrollReq( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_SCRMSG ], GF_BGL_SCROLL_X_SET, 0 );

		// 作業用グラフィックを消す
		if( p_wk->way == WF_COMMON_LEFT ){
			draw_area = WFNOTE_DRAWAREA_LEFT;
		}else{
			draw_area = WFNOTE_DRAWAREA_RIGHT;
		}
		// メイン面に表示
		WFNOTE_FRINEDINFO_DrawPage( p_wk, p_data, p_draw, WFNOTE_DRAWAREA_MAIN, 
				p_wk->page, heapID );

		// 矢印動作開始
		WFNOTE_DRAW_YazirushiSetAnmFlag( p_draw, TRUE );
		return TRUE;
	}

	
	scrll_x = (p_wk->count * WFNOTE_FRIENDINFO_SCRLL_SIZX) / WFNOTE_FRIENDINFO_SCRLL_COUNT;

	if( p_wk->way == WF_COMMON_LEFT ){
		scrll_x = -scrll_x;
	}

	//  スクロール座標設定
	GF_BGL_ScrollReq( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_SCROLL ], GF_BGL_SCROLL_X_SET, scrll_x );
	GF_BGL_ScrollReq( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_SCRMSG ], GF_BGL_SCROLL_X_SET, scrll_x );

	// アニメーション
	if( p_wk->count % (WFNOTE_FRIENDINFO_SCRLL_COUNT/4) ){
		idx = p_wk->count / (WFNOTE_FRIENDINFO_SCRLL_COUNT/4);
		if( pSHIORIFUNC[idx] ){
			pSHIORIFUNC[idx]( p_wk, p_draw, p_wk->page, p_wk->lastpage );
		}
	}

	// カウント
	p_wk->count ++;

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *	@param	heapID		ヒープ
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDINFO_DrawInit( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	int i, j;
	static const u32 sc_DRAWAREA_CGX[ WFNOTE_DRAWAREA_NUM ] = {
		WFNOTE_FRIENDINFO_TEXT_CGX0,
		WFNOTE_FRIENDINFO_TEXT_CGX1,
		WFNOTE_FRIENDINFO_TEXT_CGX1,
	};
	
	// 各ページのスクリーンデータ読み込み
	WFNOTE_FRIENDINFO_SCRNDATA_Init( &p_wk->scrn, p_draw, heapID );

	// 描画エリア初期化
	for( i=0; i<WFNOTE_DRAWAREA_NUM; i++ ){	
		WFNOTE_FRIENDINFO_DRAWAREA_Init( &p_wk->drawarea[i],  p_draw, &sc_SCRNAREA[i],
				sc_DRAWAREA_CGX[i], heapID );
		// 描画エリア内のビットマップ作成
		for( j=0; j<WFNOTE_FRIENDINFO_PAGE_NUM; j++ ){
			WFNOTE_FRIENDINFO_DRAWAREA_MSGBmpInit( &p_wk->drawarea[i], p_draw, j,
					sc_WFNOTE_FRIENDINFO_PAGEBMPNUM[j], 
					scp_WFNOTE_FRIENDINFO_PAGEBMP[j], heapID );
		}
	}


}

//----------------------------------------------------------------------------
/**
 *	@brief	描画破棄
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 */	
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDINFO_DrawExit( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw )
{
	int i, j;

	// 描画エリア破棄
	for( i=0; i<WFNOTE_DRAWAREA_NUM; i++ ){	
		for( j=0; j<WFNOTE_FRIENDINFO_PAGE_NUM; j++ ){
			WFNOTE_FRIENDINFO_DRAWAREA_MSGBmpExit( &p_wk->drawarea[i], j );
		}
		WFNOTE_FRIENDINFO_DRAWAREA_Exit( &p_wk->drawarea[i] );
	}
	
	// 各ページのスクリーンデータ破棄
	WFNOTE_FRIENDINFO_SCRNDATA_Exit( &p_wk->scrn );
}

//----------------------------------------------------------------------------
/**
 *	@brief	表示開始
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDINFO_DrawOn( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	// タイトル表示
	WFNOTE_DRAW_FriendNameSetWordset( p_draw, p_data->p_save, 
			p_data->idx.fridx[p_data->select_listidx], heapID );
	WFNOTE_DRAW_BmpTitleWrite( p_draw, msg_wifi_note_14 );

	// ページの表示
	WFNOTE_FRINEDINFO_DrawPage( p_wk, p_data, p_draw, WFNOTE_DRAWAREA_MAIN, 
			p_wk->page, heapID );	

	// 矢印表示
	WFNOTE_DRAW_YazirushiSetDrawFlag( p_draw, TRUE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	表示OFF
 *
 *	@param	p_wk	ワーク
 *	@param	p_draw	描画システム
 *
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDINFO_DrawOff( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DRAW* p_draw )
{
	int i;
	
	WFNOTE_DRAW_YazirushiSetDrawFlag( p_draw, FALSE );

	for( i=0; i<WFNOTE_DRAWAREA_NUM; i++ ){
		WFNOTE_FRIENDINFO_DRAWAREA_PageOff( &p_wk->drawarea[ i ], p_draw );
	}
	WFNOTE_FRIENDINFO_ShioriOff( p_wk, p_draw );

	WFNOTE_DRAW_BmpTitleOff( p_draw );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ページの表示
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *	@param	drawarea	描画エリア
 *	@param	page		ページ
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRINEDINFO_DrawPage( WFNOTE_FRIENDINFO* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 drawarea, u32 page, u32 heapID )
{
	if( drawarea == WFNOTE_DRAWAREA_MAIN ){
		p_wk->page = page;	// 表示ページ更新
		// しおり更新
		WFNOTE_FRIENDINFO_ShioriDraw( p_wk, p_draw, page, heapID );
	}
	// ページ描画
	WFNOTE_FRIENDINFO_DRAWAREA_Page( &p_wk->drawarea[ drawarea ], p_data, 
			p_draw, page, &p_wk->scrn, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	しおり描画	ON
 *
 *	@param	p_wk		ワーク
 *	@param	p_draw		描画システム
 *	@param	page		ページ
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDINFO_ShioriDraw( WFNOTE_FRIENDINFO* p_wk,  WFNOTE_DRAW* p_draw, u32 page, u32 heapID )
{
	u32 x;
	
	// 大本をまず描画
	// フロンティアOFFなら大本しおりを変更
	if( p_wk->frontier_draw == TRUE ){
		GF_BGL_ScrWriteExpand( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_BACK ],
				WFNOTE_FRIENDINFO_SHIORI_X, WFNOTE_FRIENDINFO_SHIORI_Y,
				WFNOTE_FRIENDINFO_SHIORI_SIZX, WFNOTE_FRIENDINFO_SHIORI_SIZY,
				p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->rawData, 
				WFNOTE_FRIENDINFO_SHIORISCRN_X, WFNOTE_FRIENDINFO_SHIORISCRN_Y, 
				p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenWidth/8,
				p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenHeight/8 );
	}else{
		GF_BGL_ScrWriteExpand( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_BACK ],
				WFNOTE_FRIENDINFO_SHIORI_X, WFNOTE_FRIENDINFO_SHIORI_Y,
				WFNOTE_FRIENDINFO_SHIORI_SIZX, WFNOTE_FRIENDINFO_SHIORI_SIZY,
				p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->rawData, 
				WFNOTE_FRIENDINFO_SHIORISCRN_X, WFNOTE_FRIENDINFO_SHIORISCRN_Y+3, 
				p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenWidth/8,
				p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenHeight/8 );
	}

	// 今のページのしおりを描画
	x = WFNOTE_FRIENDINFO_SHIORI_X + (WFNOTE_FRIENDINFO_SHIORI_ONEX*page) + WFNOTE_FRIENDINFO_SHIORI_OFSX;
	GF_BGL_ScrWriteExpand( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_BACK ],
			x, WFNOTE_FRIENDINFO_SHIORI_Y + WFNOTE_FRIENDINFO_SHIORI_OFSY,
			WFNOTE_FRIENDINFO_SHIORIPOS_SIZ, WFNOTE_FRIENDINFO_SHIORIPOS_SIZ,
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->rawData, 
			WFNOTE_FRIENDINFO_SHIORISCRNPOS_X, WFNOTE_FRIENDINFO_SHIORISCRNPOS_Y, 
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenWidth/8,
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenHeight/8 );


	// BG面転送
	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_BACK] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	しおりOFF
 *	
 *	@param	p_wk		ワーク	
 *	@param	p_draw		描画システム
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDINFO_ShioriOff( WFNOTE_FRIENDINFO* p_wk,  WFNOTE_DRAW* p_draw )
{
	// しおりぶぶん
	GF_BGL_ScrWriteExpand( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_BACK ],
			WFNOTE_FRIENDINFO_SHIORI_X, WFNOTE_FRIENDINFO_SHIORI_Y,
			WFNOTE_FRIENDINFO_SHIORI_SIZX, WFNOTE_FRIENDINFO_SHIORI_SIZY,
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->rawData, 
			WFNOTE_FRIENDINFO_CL_SHIORISCRN_X, WFNOTE_FRIENDINFO_CL_SHIORISCRN_Y, 
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenWidth/8,
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenHeight/8 );
	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_BACK ] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	しおりアニメ００
 *	
 *	@param	p_wk	ワーク
 *	@param	p_draw	描画システム
 *	@param	page00	今のページ
 *	@param	page01	前のページ
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDINFO_ShioriAnm00( WFNOTE_FRIENDINFO* p_wk,  WFNOTE_DRAW* p_draw, u32 page00, u32 page01 )
{
	int x; 

	// 今のページのしおりを描画
	x = WFNOTE_FRIENDINFO_SHIORI_X + (WFNOTE_FRIENDINFO_SHIORI_ONEX*page01) + WFNOTE_FRIENDINFO_SHIORI_OFSX;
	GF_BGL_ScrWriteExpand( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_BACK ],
			x, WFNOTE_FRIENDINFO_SHIORI_Y + WFNOTE_FRIENDINFO_SHIORI_OFSY,
			WFNOTE_FRIENDINFO_SHIORIPOS_SIZ, WFNOTE_FRIENDINFO_SHIORIPOS_SIZ,
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->rawData, 
			WFNOTE_FRIENDINFO_EFFECTSHIORISCRNPOS_X, 
			WFNOTE_FRIENDINFO_EFFECTSHIORISCRNPOS_Y, 
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenWidth/8,
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenHeight/8 );

	// BG面転送
	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_BACK] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	しおりアニメ０１
 *	
 *	@param	p_wk	ワーク
 *	@param	p_draw	描画システム
 *	@param	page00	今のページ
 *	@param	page01	前のページ
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDINFO_ShioriAnm01( WFNOTE_FRIENDINFO* p_wk,  WFNOTE_DRAW* p_draw, u32 page00, u32 page01 )
{
	int x; 

	// 大本をまず描画
	// フロンティアOFFなら大本しおりを変更
	if( p_wk->frontier_draw == TRUE ){
		GF_BGL_ScrWriteExpand( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_BACK ],
				WFNOTE_FRIENDINFO_SHIORI_X, WFNOTE_FRIENDINFO_SHIORI_Y,
				WFNOTE_FRIENDINFO_SHIORI_SIZX, WFNOTE_FRIENDINFO_SHIORI_SIZY,
				p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->rawData, 
				WFNOTE_FRIENDINFO_SHIORISCRN_X, WFNOTE_FRIENDINFO_SHIORISCRN_Y, 
				p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenWidth/8,
				p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenHeight/8 );
	}else{
		GF_BGL_ScrWriteExpand( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_BACK ],
				WFNOTE_FRIENDINFO_SHIORI_X, WFNOTE_FRIENDINFO_SHIORI_Y,
				WFNOTE_FRIENDINFO_SHIORI_SIZX, WFNOTE_FRIENDINFO_SHIORI_SIZY,
				p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->rawData, 
				WFNOTE_FRIENDINFO_SHIORISCRN_X, WFNOTE_FRIENDINFO_SHIORISCRN_Y+3, 
				p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenWidth/8,
				p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenHeight/8 );
	}

	// 今のページのしおりを描画
	x = WFNOTE_FRIENDINFO_SHIORI_X + (WFNOTE_FRIENDINFO_SHIORI_ONEX*page00) + WFNOTE_FRIENDINFO_SHIORI_OFSX;
	GF_BGL_ScrWriteExpand( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_BACK ],
			x, WFNOTE_FRIENDINFO_SHIORI_Y + WFNOTE_FRIENDINFO_SHIORI_OFSY,
			WFNOTE_FRIENDINFO_SHIORIPOS_SIZ, WFNOTE_FRIENDINFO_SHIORIPOS_SIZ,
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->rawData, 
			WFNOTE_FRIENDINFO_SHIORISCRNPOS_X, 
			WFNOTE_FRIENDINFO_SHIORISCRNPOS_Y, 
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenWidth/8,
			p_draw->scrn.p_scrn[WFNOTE_SCRNDATA_KIT]->screenHeight/8 );

	// BG面転送
	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_BACK] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	選択リストインデックス値　変更
 *
 *	@param	p_data	データ
 *	@param	way		方向		上かした
 *
 *	@retval	TRUE	変更した
 *	@retval	FALSE	変更できない
 */
//-----------------------------------------------------------------------------
static BOOL WFNOTE_FRIENDINFO_SelectListIdxAdd( WFNOTE_DATA* p_data, WF_COMMON_WAY way )
{
	int i;
	u32 sex;
	s32 select_listidx;
	WIFI_LIST* p_wifilist;

	p_wifilist = SaveData_GetWifiListData( p_data->p_save );
	select_listidx = p_data->select_listidx;
	
	if( way == WF_COMMON_TOP ){
		for( i=0; i<p_data->idx.friendnum-1; i++ ){
			select_listidx --;
			if( select_listidx < 0 ){
				select_listidx += p_data->idx.friendnum;
			}
			
			// その人の性別をチェック
			sex = WifiList_GetFriendInfo( p_wifilist, p_data->idx.fridx[select_listidx], WIFILIST_FRIEND_SEX );
			if( sex != PM_NEUTRAL ){
				p_data->select_listidx = select_listidx;
				return TRUE;
			}
		}
	}else{
		for( i=0; i<p_data->idx.friendnum-1; i++ ){
			select_listidx = (select_listidx + 1) % p_data->idx.friendnum;
			
			// その人の性別をチェック
			sex = WifiList_GetFriendInfo( p_wifilist, p_data->idx.fridx[select_listidx], WIFILIST_FRIEND_SEX );
			if( sex != PM_NEUTRAL ){
				p_data->select_listidx = select_listidx;
				return TRUE;
			}
		}
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	スクリーンデータ初期化
 *
 *	@param	p_scrn		スクリーンデータ格納先
 *	@param	p_draw		描画システム
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDINFO_SCRNDATA_Init( WFNOTE_FRIENDINFO_SCRNDATA* p_scrn, WFNOTE_DRAW* p_draw, u32 heapID )
{
	static const u8 sc_SCRNIDX[WFNOTE_FRIENDINFO_PAGE_NUM] = {
		NARC_pl_wifinote_techo_03_00_NSCR,
		NARC_pl_wifinote_techo_03_06_NSCR,
		NARC_pl_wifinote_techo_03_01_NSCR,
		NARC_pl_wifinote_techo_03_02_NSCR,
		NARC_pl_wifinote_techo_03_03_NSCR,
		NARC_pl_wifinote_techo_03_04_NSCR,
		NARC_pl_wifinote_techo_03_05_NSCR,
	};
	int i;

	for( i=0; i<WFNOTE_FRIENDINFO_PAGE_NUM; i++ ){
		p_scrn->p_scrnbuff[i] = ArcUtil_HDL_ScrnDataGet(  
				p_draw->p_handle, sc_SCRNIDX[i], FALSE, 
				&p_scrn->p_scrn[i], heapID );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	スクリーンデータ破棄
 *
 *	@param	p_scrn		スクリーンデータ
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDINFO_SCRNDATA_Exit( WFNOTE_FRIENDINFO_SCRNDATA* p_scrn )
{
	int i;

	for( i=0; i<WFNOTE_FRIENDINFO_PAGE_NUM; i++ ){
		sys_FreeMemoryEz( p_scrn->p_scrnbuff[i] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	表示エリアワーク	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_draw		描画システム
 *	@param	cp_scrnarea	表示エリアデータ
 *	@param	cgx			メッセージ面のCGX
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDINFO_DRAWAREA_Init( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, const WFNOTE_SCRNAREA* cp_scrnarea, u32 cgx, u32 heapID )
{
	int i;
	p_wk->scrnarea = *cp_scrnarea;
	p_wk->cgx = cgx;
	
	for( i=0; i<WFNOTE_FRIENDINFO_PAGE_NUM; i++ ){
		p_wk->p_msg[i] = NULL;
		p_wk->msgnum[i] = 0;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画エリア　破棄
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDINFO_DRAWAREA_Exit( WFNOTE_FRIENDINFO_DRAWAREA* p_wk )
{
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージビットマップ	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_draw		描画システム
 *	@param	idx			インデックス
 *	@param	num			個数
 *	@param	cp_data		構成データ
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDINFO_DRAWAREA_MSGBmpInit( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw, u32 idx, u32 num, const WFNOTE_FRIENDINFO_BMPDATA* cp_data, u32 heapID )
{
	int i;
	u16 cgx;
	
	p_wk->p_msg[idx] = GF_BGL_BmpWinAllocGet( heapID, num );
	p_wk->msgnum[idx] = num;

	// 作成
	cgx = p_wk->cgx;
	for( i=0; i<num; i++ ){
		GF_BGL_BmpWinInit( &p_wk->p_msg[idx][i] );
		GF_BGL_BmpWinAdd( p_draw->p_bgl, &p_wk->p_msg[idx][i], 
				sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_SCRMSG ],
				MATH_ABS(p_wk->scrnarea.scrn_x) + cp_data[i].pos_x,
				MATH_ABS(p_wk->scrnarea.scrn_y) + cp_data[i].pos_y,
				cp_data[i].siz_x, cp_data[i].siz_y,
				WFNOTE_BGPL_MAIN_MSGFONT, cgx );
		GF_BGL_BmpWinDataFill( &p_wk->p_msg[idx][i], 0 );
		cgx += ( cp_data[i].siz_x * cp_data[i].siz_y );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージビットマップ破棄
 *
 *	@param	p_wk	ワーク
 *	@param	idx		インデックス
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDINFO_DRAWAREA_MSGBmpExit( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, u32 idx )
{
	if( p_wk->p_msg[idx] ){
		GF_BGL_BmpWinFree( p_wk->p_msg[idx], p_wk->msgnum[idx] );
		p_wk->p_msg[idx] = NULL;
		p_wk->msgnum[idx] = 0;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	更新
 *
 *	@param	p_wk	ワーク 
 *	@param	idx		インデックス
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDINFO_DRAWAREA_MSGBmpOnVReq( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, u32 idx )
{
	int i;
	for( i=0; i<p_wk->msgnum[idx]; i++ ){
		GF_BGL_BmpWinOnVReq( &p_wk->p_msg[idx][i] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	表示OFF
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDINFO_DRAWAREA_MSGBmpOffVReq( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, u32 idx )
{
	int i;

	for( i=0; i<p_wk->msgnum[idx]; i++ ){
		GF_BGL_BmpWinOffVReq( &p_wk->p_msg[idx][i] );
		GF_BGL_BmpWinDataFill( &p_wk->p_msg[idx][i], 0 );
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	描画アリアにページの書き込み
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画ワーク
 *	@param	page		ページ
 *	@param	cp_scrn		スクリーン描画データ
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDINFO_DRAWAREA_Page( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 page, const WFNOTE_FRIENDINFO_SCRNDATA* cp_scrn, u32 heapID )
{
	static void (*pWrite[ WFNOTE_FRIENDINFO_PAGE_NUM ])( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID ) = {
		WFNOTE_FRIENDINFO_DRAW_Page00,
		WFNOTE_FRIENDINFO_DRAW_Page06,
		WFNOTE_FRIENDINFO_DRAW_Page01,
		WFNOTE_FRIENDINFO_DRAW_Page02,
		WFNOTE_FRIENDINFO_DRAW_Page03,
		WFNOTE_FRIENDINFO_DRAW_Page04,
		WFNOTE_FRIENDINFO_DRAW_Page05,
	};

	// 今までの表示クリア
	WFNOTE_FRIENDINFO_DRAWAREA_PageOff( p_wk, p_draw );

	// 背景描画
	GF_BGL_ScrWrite( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_SCROLL ],
			cp_scrn->p_scrn[page]->rawData, 
			MATH_ABS(p_wk->scrnarea.scrn_x), MATH_ABS(p_wk->scrnarea.scrn_y), 
			cp_scrn->p_scrn[page]->screenWidth/8,
			cp_scrn->p_scrn[page]->screenHeight/8 );
	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_SCROLL] );
	
	// ページ書き込み
	pWrite[page]( p_wk, p_data, p_draw, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ぺーじOFF 
 *
 *	@param	p_wk	ワーク 
 *	@param	p_draw	描画システム
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDINFO_DRAWAREA_PageOff( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, WFNOTE_DRAW* p_draw )
{
	int i;
	for( i=0; i<WFNOTE_FRIENDINFO_PAGE_NUM; i++ ){
		WFNOTE_FRIENDINFO_DRAWAREA_MSGBmpOffVReq( p_wk, i );
	}
	WFNOTE_FRIENDINFO_DRAW_Clean( p_draw, &p_wk->scrnarea );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ページの書き込み
 *
 *	@param	p_wk		描画エリアワーク
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDINFO_DRAW_Page00( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	STRBUF* p_str;
	STRBUF* p_tmp;
	WIFI_LIST* p_wifilist;
	u32 num;
	BOOL result;
	u32 sex;
	u32 col;
	u32 no;
    TR_CLACT_GRA arcdata;
	void* p_trgrabuff;
	u32 x, y;


	p_wifilist = SaveData_GetWifiListData( p_data->p_save );

	p_str = STRBUF_Create( WFNOTE_STRBUF_SIZE, heapID );
	p_tmp = STRBUF_Create( WFNOTE_STRBUF_SIZE, heapID );

	// グループ
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 0, WFNOTE_FRIENDINFO_PAGE00_BA_GR,
			p_data, p_draw, msg_wifi_note_15, 
			WFNOTE_FRIENDINFO_PAGE00_GRT_X, WFNOTE_FRIENDINFO_PAGE00_GRT_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp, 0 );

    // MatchComment: Above message was split in two
    // TODO: append new msg idxs to msg_wifi_note.h
    WFNOTE_DRAW_FriendGroupSetWordset( p_draw, p_data->p_save,
            p_data->idx.fridx[p_data->select_listidx], heapID );
    WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 0, WFNOTE_FRIENDINFO_PAGE00_BA_GR,
            p_data, p_draw, 61,
            WFNOTE_FRIENDINFO_PAGE00_GRT2_X, WFNOTE_FRIENDINFO_PAGE00_GRT2_Y,
            WFNOTE_COL_BLACK, p_str, p_tmp, 2 );

    // 対戦成績
    {
		WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 0, WFNOTE_FRIENDINFO_PAGE00_BA_GR,
				p_data, p_draw, msg_wifi_note_16, 
				WFNOTE_FRIENDINFO_PAGE00_BTT_X, WFNOTE_FRIENDINFO_PAGE00_BTT_Y,
				WFNOTE_COL_BLACK, p_str, p_tmp, 0 );

		// かち
		WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 0, WFNOTE_FRIENDINFO_PAGE00_BA_BT,
				p_data, p_draw, msg_wifi_note_17, 
				WFNOTE_FRIENDINFO_PAGE00_BTW_X, WFNOTE_FRIENDINFO_PAGE00_BTW_Y,
				WFNOTE_COL_BLACK, p_str, p_tmp, 0 );
		num = WifiList_GetFriendInfo( p_wifilist, p_data->idx.fridx[p_data->select_listidx], WIFILIST_FRIEND_BATTLE_WIN );
		WFNOTE_DRAW_NumberSetWordset( p_draw, num );
		WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 0, WFNOTE_FRIENDINFO_PAGE00_BA_BT,
				p_data, p_draw, msg_wifi_note_59, 
				WFNOTE_FRIENDINFO_PAGE00_BTWLN_X, WFNOTE_FRIENDINFO_PAGE00_BTW_Y,
				WFNOTE_COL_BLACK, p_str, p_tmp, 2 );


		// まけ
		WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 0, WFNOTE_FRIENDINFO_PAGE00_BA_BT,
				p_data, p_draw, msg_wifi_note_18, 
				WFNOTE_FRIENDINFO_PAGE00_BTL_X, WFNOTE_FRIENDINFO_PAGE00_BTL_Y,
				WFNOTE_COL_BLACK, p_str, p_tmp, 0 );
		num = WifiList_GetFriendInfo( p_wifilist, p_data->idx.fridx[p_data->select_listidx], WIFILIST_FRIEND_BATTLE_LOSE );
		WFNOTE_DRAW_NumberSetWordset( p_draw, num );
		WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 0, WFNOTE_FRIENDINFO_PAGE00_BA_BT,
				p_data, p_draw, msg_wifi_note_59, 
				WFNOTE_FRIENDINFO_PAGE00_BTWLN_X, WFNOTE_FRIENDINFO_PAGE00_BTL_Y,
				WFNOTE_COL_BLACK, p_str, p_tmp, 2 );
	}

	// ポケモン交換
	{
		WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 0, WFNOTE_FRIENDINFO_PAGE00_BA_TR,
				p_data, p_draw, msg_wifi_note_19, 
				WFNOTE_FRIENDINFO_PAGE00_TRT_X, WFNOTE_FRIENDINFO_PAGE00_TRT_Y,
				WFNOTE_COL_BLACK, p_str, p_tmp, 0 );
		num = WifiList_GetFriendInfo( p_wifilist, p_data->idx.fridx[p_data->select_listidx], WIFILIST_FRIEND_TRADE_NUM );
		WFNOTE_DRAW_NumberSetWordset( p_draw, num );
		WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 0, WFNOTE_FRIENDINFO_PAGE00_BA_TR,
				p_data, p_draw, msg_wifi_note_20, 
				WFNOTE_FRIENDINFO_PAGE00_TRN_X, WFNOTE_FRIENDINFO_PAGE00_TRN_Y,
				WFNOTE_COL_BLACK, p_str, p_tmp, 2 );
	}

	// 料理回数
	if( WFNOTE_FRIENDINFO_PofinCaseCheck( p_data, heapID ) ){
		WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 0, WFNOTE_FRIENDINFO_PAGE00_BA_TR,
				p_data, p_draw, msg_wifi_note_34, 
				WFNOTE_FRIENDINFO_PAGE00_POT_X, WFNOTE_FRIENDINFO_PAGE00_POT_Y,
				WFNOTE_COL_BLACK, p_str, p_tmp, 0 );
		num = WifiList_GetFriendInfo( p_wifilist, p_data->idx.fridx[p_data->select_listidx], WIFILIST_FRIEND_POFIN_NUM );
		WFNOTE_DRAW_NumberSetWordset( p_draw, num );
		WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 0, WFNOTE_FRIENDINFO_PAGE00_BA_TR,
				p_data, p_draw, msg_wifi_note_35, 
				WFNOTE_FRIENDINFO_PAGE00_PON_X, WFNOTE_FRIENDINFO_PAGE00_PON_Y,
				WFNOTE_COL_BLACK, p_str, p_tmp, 2 );
	}

	// 最後に遊んだ日付
	{
		WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 0, WFNOTE_FRIENDINFO_PAGE00_BA_DAY,
				p_data, p_draw, msg_wifi_note_21, 
				WFNOTE_FRIENDINFO_PAGE00_DAYT_X, WFNOTE_FRIENDINFO_PAGE00_DAYT_Y,
				WFNOTE_COL_WHITE, p_str, p_tmp, 0 );
		result = WFNOTE_DRAW_FriendDaySetWordset( p_draw, p_data->p_save, 
				p_data->idx.fridx[p_data->select_listidx] );
		if( result ){
			WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 0, WFNOTE_FRIENDINFO_PAGE00_BA_DAY,
					p_data, p_draw, msg_wifi_note_22, 
					WFNOTE_FRIENDINFO_PAGE00_DAYN_X, WFNOTE_FRIENDINFO_PAGE00_DAYN_Y,
					WFNOTE_COL_WHITE, p_str, p_tmp, 2 );
		}
	}

	sex = WifiList_GetFriendInfo( p_wifilist, p_data->idx.fridx[p_data->select_listidx], WIFILIST_FRIEND_SEX );

	// トレーナーの絵表示
	{
		num = WifiList_GetFriendInfo( p_wifilist, p_data->idx.fridx[p_data->select_listidx], WIFILIST_FRIEND_UNION_GRA );
        no = UnionView_GetTrainerInfo(sex, num, UNIONVIEW_TRTYPE);
        TrCLACTGraDataGet( no, PARA_FRONT, &arcdata);
		p_trgrabuff = sys_AllocMemory( heapID, (WFNOTE_FRIENDINFO_PAGE00_TRGRA_SIZX*WFNOTE_FRIENDINFO_PAGE00_TRGRA_SIZY)*32 );
		ChangesInto_1D_from_2D( arcdata.arcID, arcdata.ncbrID, heapID, 
				WFNOTE_FRIENDINFO_PAGE00_TRGRA_CUT_X, 
				WFNOTE_FRIENDINFO_PAGE00_TRGRA_CUT_Y,
				WFNOTE_FRIENDINFO_PAGE00_TRGRA_SIZX, 
				WFNOTE_FRIENDINFO_PAGE00_TRGRA_SIZY, p_trgrabuff);
		// ビットマップに書き込む
		GF_BGL_BmpWinPrint(
				&p_wk->p_msg[0][WFNOTE_FRIENDINFO_PAGE00_BA_TRG],
				p_trgrabuff,
				0, 0,
				WFNOTE_FRIENDINFO_PAGE00_TRGRA_SIZX*8, 
				WFNOTE_FRIENDINFO_PAGE00_TRGRA_SIZY*8,
				WFNOTE_FRIENDINFO_PAGE00_TRNGR_X, 
				WFNOTE_FRIENDINFO_PAGE00_TRNGR_Y,
				WFNOTE_FRIENDINFO_PAGE00_TRGRA_SIZX*8, 
				WFNOTE_FRIENDINFO_PAGE00_TRGRA_SIZY*8 );
		sys_FreeMemoryEz( p_trgrabuff );

		// トレーナーパレット転送
		ArcUtil_PalSet( arcdata.arcID, arcdata.nclrID, PALTYPE_MAIN_BG,
				WFNOTE_BGPL_MAIN_TRGRA*32, 32, heapID );
	}

	// トレーナーの名前
	{
		if( sex == PM_MALE ){
			col = WFNOTE_COL_BLUE;
		}else if( sex == PM_FEMALE ){
			col = WFNOTE_COL_RED;
		}else{
			col = WFNOTE_COL_BLACK;
		}
		WFNOTE_DRAW_FriendNameSetWordset( p_draw, p_data->p_save, p_data->idx.fridx[p_data->select_listidx],
				heapID );
		WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 0, WFNOTE_FRIENDINFO_PAGE00_BA_TRG,
				p_data, p_draw, msg_wifi_note_07, 
				WFNOTE_FRIENDINFO_PAGE00_TRNNA_X, WFNOTE_FRIENDINFO_PAGE00_TRNNA_Y,
				col, p_str, p_tmp, 1 );
	}
	
	WFNOTE_FRIENDINFO_DRAWAREA_MSGBmpOnVReq( p_wk, 0 );

	// VReqでスクリーンにキャラクタデータが展開されるので
	// トレーナーの絵のパレット設定
	{
		x = GF_BGL_BmpWinGet_PosX( &p_wk->p_msg[0][ WFNOTE_FRIENDINFO_PAGE00_BA_TRG ] );
		y = GF_BGL_BmpWinGet_PosY( &p_wk->p_msg[0][ WFNOTE_FRIENDINFO_PAGE00_BA_TRG ] );
		GF_BGL_ScrPalChange(
					p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_SCRMSG ],
					x, 
					y,
					WFNOTE_FRIENDINFO_PAGE00_TRNGR_CSIZX, 
					WFNOTE_FRIENDINFO_PAGE00_TRNGR_CSIZY, 
					WFNOTE_BGPL_MAIN_TRGRA );
	}

	STRBUF_Delete( p_str );
	STRBUF_Delete( p_tmp );
}

#ifdef NONEQUIVALENT
static void WFNOTE_FRIENDINFO_DRAW_Page01( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	STRBUF* p_str;
	STRBUF* p_tmp;
	FRONTIER_SAVEWORK*  p_fsave;
	u32 num;
	BOOL clear_flag;

	p_fsave		= SaveData_GetFrontier( p_data->p_save );
	p_str = STRBUF_Create( WFNOTE_STRBUF_SIZE, heapID );
	p_tmp = STRBUF_Create( WFNOTE_STRBUF_SIZE, heapID );

	// タイトル
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 2, WFNOTE_FRIENDINFO_PAGE01_BA,
			p_data, p_draw, msg_wifi_note_36, 
			WFNOTE_FRIENDINFO_PAGE01_BTT_X, WFNOTE_FRIENDINFO_PAGE01_BTT_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp );

	// 前回
	{
		u32 title_stridx;
		clear_flag = FrontierRecord_Get(p_fsave, FRID_TOWER_MULTI_WIFI_CLEAR_BIT, p_data->idx.fridx[p_data->select_listidx]);
		if( clear_flag == FALSE ){
			title_stridx = msg_wifi_note_41;
		}else{
			title_stridx = msg_wifi_note_58;
		}
		WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 2, WFNOTE_FRIENDINFO_PAGE01_BA,
				p_data, p_draw, title_stridx, 
				WFNOTE_FRIENDINFO_PAGE01_BTL_X, WFNOTE_FRIENDINFO_PAGE01_BTL_Y,
				WFNOTE_COL_BLACK, p_str, p_tmp );
		num = FrontierRecord_Get(p_fsave, FRID_TOWER_MULTI_WIFI_RENSHOU_CNT, p_data->idx.fridx[p_data->select_listidx]);
		WFNOTE_DRAW_NumberSetWordset( p_draw, num );
		WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 2, WFNOTE_FRIENDINFO_PAGE01_BA,
				p_data, p_draw, msg_wifi_note_48, 
				WFNOTE_FRIENDINFO_PAGE01_BTLN_X, WFNOTE_FRIENDINFO_PAGE01_BTL_Y,
				WFNOTE_COL_BLACK, p_str, p_tmp );
	}

	//  最高
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 2, WFNOTE_FRIENDINFO_PAGE01_BA,
			p_data, p_draw, msg_wifi_note_42, 
			WFNOTE_FRIENDINFO_PAGE01_BTM_X, WFNOTE_FRIENDINFO_PAGE01_BTM_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp );
	num = FrontierRecord_Get(p_fsave, FRID_TOWER_MULTI_WIFI_RENSHOU, p_data->idx.fridx[p_data->select_listidx]);
	WFNOTE_DRAW_NumberSetWordset( p_draw, num );
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 2, WFNOTE_FRIENDINFO_PAGE01_BA,
			p_data, p_draw, msg_wifi_note_48, 
			WFNOTE_FRIENDINFO_PAGE01_BTMN_X, WFNOTE_FRIENDINFO_PAGE01_BTM_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp );

	WFNOTE_FRIENDINFO_DRAWAREA_MSGBmpOnVReq( p_wk, 2 );

	STRBUF_Delete( p_str );
	STRBUF_Delete( p_tmp );
}
#else
asm static void WFNOTE_FRIENDINFO_DRAW_Page01( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	push {r3, r4, r5, r6, r7, lr}
	sub sp, #0x28
	add r5, r1, #0
	str r0, [sp, #0x20]
	ldr r0, [r5, #0]
	add r6, r3, #0
	add r4, r2, #0
	bl SaveData_GetFrontier
	str r0, [sp, #0x24]
	mov r0, #0x80
	add r1, r6, #0
	bl STRBUF_Create
	add r7, r0, #0
	mov r0, #0x80
	add r1, r6, #0
	bl STRBUF_Create
	add r6, r0, #0
	str r4, [sp]
	mov r0, #0x19
	str r0, [sp, #4]
	mov r2, #0
	str r2, [sp, #8]
	mov r0, #8
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _02231660
	mov r1, #2
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	ldr r0, [sp, #0x20]
	add r3, r5, #0
	str r2, [sp, #0x1c]
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	mov r2, #7
	ldrsb r2, [r5, r2]
	ldr r0, [sp, #0x24]
	mov r1, #0x64
	add r2, r5, r2
	ldrb r2, [r2, #0xc]
	bl FrontierRecord_Get
	cmp r0, #0
	bne _0223158A
	mov r0, #0x1e
	b _0223158C
_0223158A:
	mov r0, #0x1f
_0223158C:
	str r4, [sp]
	str r0, [sp, #4]
	mov r2, #0
	str r2, [sp, #8]
	mov r0, #0x20
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _02231660
	mov r1, #2
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	ldr r0, [sp, #0x20]
	add r3, r5, #0
	str r2, [sp, #0x1c]
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	mov r2, #7
	ldrsb r2, [r5, r2]
	ldr r0, [sp, #0x24]
	mov r1, #0x71
	add r2, r5, r2
	ldrb r2, [r2, #0xc]
	bl FrontierRecord_Get
	add r1, r0, #0
	add r0, r4, #0
	bl WFNOTE_DRAW_NumberSetWordset
	str r4, [sp]
	mov r0, #0x26
	str r0, [sp, #4]
	mov r0, #0xa0
	str r0, [sp, #8]
	mov r0, #0x20
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _02231660
	mov r1, #2
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	ldr r0, [sp, #0x20]
	str r1, [sp, #0x1c]
	mov r2, #0
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	str r4, [sp]
	mov r0, #0x20
	str r0, [sp, #4]
	mov r2, #0
	str r2, [sp, #8]
	mov r0, #0x38
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _02231660
	mov r1, #2
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	ldr r0, [sp, #0x20]
	add r3, r5, #0
	str r2, [sp, #0x1c]
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	mov r2, #7
	ldrsb r2, [r5, r2]
	ldr r0, [sp, #0x24]
	mov r1, #0x70
	add r2, r5, r2
	ldrb r2, [r2, #0xc]
	bl FrontierRecord_Get
	add r1, r0, #0
	add r0, r4, #0
	bl WFNOTE_DRAW_NumberSetWordset
	str r4, [sp]
	mov r0, #0x26
	str r0, [sp, #4]
	mov r0, #0xa0
	str r0, [sp, #8]
	mov r0, #0x38
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _02231660
	mov r1, #2
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	ldr r0, [sp, #0x20]
	mov r2, #0
	add r3, r5, #0
	str r1, [sp, #0x1c]
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	ldr r0, [sp, #0x20]
	mov r1, #2
	bl WFNOTE_FRIENDINFO_DRAWAREA_MSGBmpOnVReq
	add r0, r7, #0
	bl STRBUF_Delete
	add r0, r6, #0
	bl STRBUF_Delete
	add sp, #0x28
	pop {r3, r4, r5, r6, r7, pc}
	nop
// _02231660: .4byte 0x00010200
}
#endif

#ifdef NONEQUIVALENT
static void WFNOTE_FRIENDINFO_DRAW_Page02( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	STRBUF* p_str;
	STRBUF* p_tmp;
	FRONTIER_SAVEWORK* p_fsave;
	u32 num;
	BOOL clear_flag;
	u32 stridx;

	p_fsave = SaveData_GetFrontier( p_data->p_save );
	p_str = STRBUF_Create( WFNOTE_STRBUF_SIZE, heapID );
	p_tmp = STRBUF_Create( WFNOTE_STRBUF_SIZE, heapID );

	// タイトル
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 3, WFNOTE_FRIENDINFO_PAGE02_BA_TT,
			p_data, p_draw, msg_wifi_note_37, 
			WFNOTE_FRIENDINFO_PAGE02_TT_X, WFNOTE_FRIENDINFO_PAGE02_TT_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp );

	// LV50
	{
		// タイトル
		WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 3, WFNOTE_FRIENDINFO_PAGE02_LV50TT,
				p_data, p_draw, msg_wifi_note_43, 
				WFNOTE_FRIENDINFO_PAGE02_LV50TT_X, WFNOTE_FRIENDINFO_PAGE02_LV50TT_Y,
				WFNOTE_COL_BLACK, p_str, p_tmp );
		WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 3, WFNOTE_FRIENDINFO_PAGE02_LV50TT,
				p_data, p_draw, msg_wifi_note_45, 
				WFNOTE_FRIENDINFO_PAGE02_LV50TT2_X, WFNOTE_FRIENDINFO_PAGE02_LV50TT_Y,
				WFNOTE_COL_BLACK, p_str, p_tmp );

		//前回
		clear_flag = FrontierRecord_Get(p_fsave, FRID_FACTORY_MULTI_WIFI_CLEAR_BIT, p_data->idx.fridx[p_data->select_listidx]);
		if( clear_flag == FALSE ){
			stridx = msg_wifi_note_41;
		}else{
			stridx = msg_wifi_note_58;
		}
		WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 3, WFNOTE_FRIENDINFO_PAGE02_LV50NO,
				p_data, p_draw, stridx, 
				WFNOTE_FRIENDINFO_PAGE02_LV50LT_X, WFNOTE_FRIENDINFO_PAGE02_LV50L_Y,
				WFNOTE_COL_BLACK, p_str, p_tmp );

		// 連勝
		num = FrontierRecord_Get(p_fsave, FRID_FACTORY_MULTI_WIFI_RENSHOU_CNT, p_data->idx.fridx[p_data->select_listidx]);
		WFNOTE_DRAW_NumberSetWordset( p_draw, num );
		WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 3, WFNOTE_FRIENDINFO_PAGE02_LV50NO,
				p_data, p_draw, msg_wifi_note_48, 
				WFNOTE_FRIENDINFO_PAGE02_LV50LN0_X, WFNOTE_FRIENDINFO_PAGE02_LV50L_Y,
				WFNOTE_COL_BLACK, p_str, p_tmp );
		// 交換
		num = FrontierRecord_Get(p_fsave, FRID_FACTORY_MULTI_WIFI_TRADE_CNT, p_data->idx.fridx[p_data->select_listidx]);
		WFNOTE_DRAW_NumberSetWordset( p_draw, num );
		WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 3, WFNOTE_FRIENDINFO_PAGE02_LV50NO,
				p_data, p_draw, msg_wifi_note_49, 
				WFNOTE_FRIENDINFO_PAGE02_LV50LN1_X, WFNOTE_FRIENDINFO_PAGE02_LV50L_Y,
				WFNOTE_COL_BLACK, p_str, p_tmp );


		//最高
		WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 3, WFNOTE_FRIENDINFO_PAGE02_LV50NO,
				p_data, p_draw, msg_wifi_note_42, 
				WFNOTE_FRIENDINFO_PAGE02_LV50MT_X, WFNOTE_FRIENDINFO_PAGE02_LV50M_Y,
				WFNOTE_COL_BLACK, p_str, p_tmp );

		// 連勝
		num = FrontierRecord_Get(p_fsave, FRID_FACTORY_MULTI_WIFI_RENSHOU, p_data->idx.fridx[p_data->select_listidx]);
		WFNOTE_DRAW_NumberSetWordset( p_draw, num );
		WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 3, WFNOTE_FRIENDINFO_PAGE02_LV50NO,
				p_data, p_draw, msg_wifi_note_48, 
				WFNOTE_FRIENDINFO_PAGE02_LV50MN0_X, WFNOTE_FRIENDINFO_PAGE02_LV50M_Y,
				WFNOTE_COL_BLACK, p_str, p_tmp );
		// 交換
		num = FrontierRecord_Get(p_fsave, FRID_FACTORY_MULTI_WIFI_TRADE, p_data->idx.fridx[p_data->select_listidx]);
		WFNOTE_DRAW_NumberSetWordset( p_draw, num );
		WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 3, WFNOTE_FRIENDINFO_PAGE02_LV50NO,
				p_data, p_draw, msg_wifi_note_49, 
				WFNOTE_FRIENDINFO_PAGE02_LV50MN1_X, WFNOTE_FRIENDINFO_PAGE02_LV50M_Y,
				WFNOTE_COL_BLACK, p_str, p_tmp );
	}

	// LV100
	{
		// タイトル
		WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 3, WFNOTE_FRIENDINFO_PAGE02_LV100TT,
				p_data, p_draw, msg_wifi_note_44, 
				WFNOTE_FRIENDINFO_PAGE02_LV100TT_X, WFNOTE_FRIENDINFO_PAGE02_LV100TT_Y,
				WFNOTE_COL_BLACK, p_str, p_tmp );
		WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 3, WFNOTE_FRIENDINFO_PAGE02_LV100TT,
				p_data, p_draw, msg_wifi_note_45, 
				WFNOTE_FRIENDINFO_PAGE02_LV100TT2_X, WFNOTE_FRIENDINFO_PAGE02_LV100TT_Y,
				WFNOTE_COL_BLACK, p_str, p_tmp );

		//前回
		clear_flag = FrontierRecord_Get(p_fsave, FRID_FACTORY_MULTI_WIFI_CLEAR100_BIT, p_data->idx.fridx[p_data->select_listidx]);
		if( clear_flag == FALSE ){
			stridx = msg_wifi_note_41;
		}else{
			stridx = msg_wifi_note_58;
		}
		WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 3, WFNOTE_FRIENDINFO_PAGE02_LV100NO,
				p_data, p_draw, stridx, 
				WFNOTE_FRIENDINFO_PAGE02_LV100LT_X, WFNOTE_FRIENDINFO_PAGE02_LV100L_Y,
				WFNOTE_COL_BLACK, p_str, p_tmp );

		// 連勝
		num = FrontierRecord_Get(p_fsave, FRID_FACTORY_MULTI_WIFI_RENSHOU100_CNT, p_data->idx.fridx[p_data->select_listidx]);
		WFNOTE_DRAW_NumberSetWordset( p_draw, num );
		WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 3, WFNOTE_FRIENDINFO_PAGE02_LV100NO,
				p_data, p_draw, msg_wifi_note_48, 
				WFNOTE_FRIENDINFO_PAGE02_LV100LN0_X, WFNOTE_FRIENDINFO_PAGE02_LV100L_Y,
				WFNOTE_COL_BLACK, p_str, p_tmp );
		// 交換
		num = FrontierRecord_Get(p_fsave, FRID_FACTORY_MULTI_WIFI_TRADE100_CNT, p_data->idx.fridx[p_data->select_listidx]);
		WFNOTE_DRAW_NumberSetWordset( p_draw, num );
		WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 3, WFNOTE_FRIENDINFO_PAGE02_LV100NO,
				p_data, p_draw, msg_wifi_note_49, 
				WFNOTE_FRIENDINFO_PAGE02_LV100LN1_X, WFNOTE_FRIENDINFO_PAGE02_LV100L_Y,
				WFNOTE_COL_BLACK, p_str, p_tmp );


		//最高
		WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 3, WFNOTE_FRIENDINFO_PAGE02_LV100NO,
				p_data, p_draw, msg_wifi_note_42, 
				WFNOTE_FRIENDINFO_PAGE02_LV100MT_X, WFNOTE_FRIENDINFO_PAGE02_LV100M_Y,
				WFNOTE_COL_BLACK, p_str, p_tmp );

		// 連勝
		num = FrontierRecord_Get(p_fsave, FRID_FACTORY_MULTI_WIFI_RENSHOU100, p_data->idx.fridx[p_data->select_listidx]);
		WFNOTE_DRAW_NumberSetWordset( p_draw, num );
		WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 3, WFNOTE_FRIENDINFO_PAGE02_LV100NO,
				p_data, p_draw, msg_wifi_note_48, 
				WFNOTE_FRIENDINFO_PAGE02_LV100MN0_X, WFNOTE_FRIENDINFO_PAGE02_LV100M_Y,
				WFNOTE_COL_BLACK, p_str, p_tmp );
		// 交換
		num = FrontierRecord_Get(p_fsave, FRID_FACTORY_MULTI_WIFI_TRADE100, p_data->idx.fridx[p_data->select_listidx]);
		WFNOTE_DRAW_NumberSetWordset( p_draw, num );
		WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 3, WFNOTE_FRIENDINFO_PAGE02_LV100NO,
				p_data, p_draw, msg_wifi_note_49, 
				WFNOTE_FRIENDINFO_PAGE02_LV100MN1_X, WFNOTE_FRIENDINFO_PAGE02_LV100M_Y,
				WFNOTE_COL_BLACK, p_str, p_tmp );
	}
	

	WFNOTE_FRIENDINFO_DRAWAREA_MSGBmpOnVReq( p_wk, 3 );

	STRBUF_Delete( p_str );
	STRBUF_Delete( p_tmp );
}
#else
asm static void WFNOTE_FRIENDINFO_DRAW_Page02( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	push {r3, r4, r5, r6, r7, lr}
	sub sp, #0x28
	add r5, r1, #0
	str r0, [sp, #0x20]
	ldr r0, [r5, #0]
	add r6, r3, #0
	add r4, r2, #0
	bl SaveData_GetFrontier
	str r0, [sp, #0x24]
	mov r0, #0x80
	add r1, r6, #0
	bl STRBUF_Create
	add r7, r0, #0
	mov r0, #0x80
	add r1, r6, #0
	bl STRBUF_Create
	add r6, r0, #0
	str r4, [sp]
	mov r0, #0x1a
	str r0, [sp, #4]
	mov r2, #0
	str r2, [sp, #8]
	ldr r0, =0x00010200 // _022319FC
	str r2, [sp, #0xc]
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	ldr r0, [sp, #0x20]
	str r2, [sp, #0x1c]
	mov r1, #3
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	str r4, [sp]
	mov r0, #0x21
	str r0, [sp, #4]
	mov r1, #0
	str r1, [sp, #8]
	ldr r0, =0x00010200 // _022319FC
	str r1, [sp, #0xc]
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	str r1, [sp, #0x1c]
	ldr r0, [sp, #0x20]
	mov r1, #3
	mov r2, #1
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	str r4, [sp]
	mov r0, #0x23
	str r0, [sp, #4]
	mov r0, #0xe0
	str r0, [sp, #8]
	mov r0, #0
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _022319FC
	mov r1, #3
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	mov r0, #2
	str r0, [sp, #0x1c]
	ldr r0, [sp, #0x20]
	mov r2, #1
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	mov r2, #7
	ldrsb r2, [r5, r2]
	ldr r0, [sp, #0x24]
	mov r1, #0x66
	add r2, r5, r2
	ldrb r2, [r2, #0xc]
	bl FrontierRecord_Get
	cmp r0, #0
	bne _0223170C
	mov r0, #0x1e
	b _0223170E
_0223170C:
	mov r0, #0x1f
_0223170E:
	str r4, [sp]
	str r0, [sp, #4]
	mov r1, #0
	str r1, [sp, #8]
	ldr r0, =0x00010200 // _022319FC
	str r1, [sp, #0xc]
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	str r1, [sp, #0x1c]
	ldr r0, [sp, #0x20]
	mov r1, #3
	mov r2, #2
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	mov r2, #7
	ldrsb r2, [r5, r2]
	ldr r0, [sp, #0x24]
	mov r1, #0x73
	add r2, r5, r2
	ldrb r2, [r2, #0xc]
	bl FrontierRecord_Get
	add r1, r0, #0
	add r0, r4, #0
	bl WFNOTE_DRAW_NumberSetWordset
	str r4, [sp]
	mov r0, #0x26
	str r0, [sp, #4]
	mov r0, #0x70
	str r0, [sp, #8]
	mov r0, #0
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _022319FC
	mov r1, #3
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	mov r0, #1
	str r0, [sp, #0x1c]
	ldr r0, [sp, #0x20]
	mov r2, #2
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	mov r2, #7
	ldrsb r2, [r5, r2]
	ldr r0, [sp, #0x24]
	mov r1, #0x75
	add r2, r5, r2
	ldrb r2, [r2, #0xc]
	bl FrontierRecord_Get
	add r1, r0, #0
	add r0, r4, #0
	bl WFNOTE_DRAW_NumberSetWordset
	str r4, [sp]
	mov r0, #0x27
	str r0, [sp, #4]
	mov r0, #0xe0
	str r0, [sp, #8]
	mov r0, #0
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _022319FC
	mov r2, #2
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	ldr r0, [sp, #0x20]
	str r2, [sp, #0x1c]
	mov r1, #3
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	str r4, [sp]
	mov r0, #0x20
	str r0, [sp, #4]
	mov r1, #0
	str r1, [sp, #8]
	mov r0, #0x10
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _022319FC
	mov r2, #2
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	str r1, [sp, #0x1c]
	ldr r0, [sp, #0x20]
	mov r1, #3
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	mov r2, #7
	ldrsb r2, [r5, r2]
	ldr r0, [sp, #0x24]
	mov r1, #0x72
	add r2, r5, r2
	ldrb r2, [r2, #0xc]
	bl FrontierRecord_Get
	add r1, r0, #0
	add r0, r4, #0
	bl WFNOTE_DRAW_NumberSetWordset
	str r4, [sp]
	mov r0, #0x26
	str r0, [sp, #4]
	mov r0, #0x70
	str r0, [sp, #8]
	mov r0, #0x10
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _022319FC
	mov r1, #3
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	mov r0, #1
	str r0, [sp, #0x1c]
	ldr r0, [sp, #0x20]
	mov r2, #2
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	mov r2, #7
	ldrsb r2, [r5, r2]
	ldr r0, [sp, #0x24]
	mov r1, #0x74
	add r2, r5, r2
	ldrb r2, [r2, #0xc]
	bl FrontierRecord_Get
	add r1, r0, #0
	add r0, r4, #0
	bl WFNOTE_DRAW_NumberSetWordset
	str r4, [sp]
	mov r0, #0x27
	str r0, [sp, #4]
	mov r0, #0xe0
	str r0, [sp, #8]
	mov r0, #0x10
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _022319FC
	mov r2, #2
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	ldr r0, [sp, #0x20]
	str r2, [sp, #0x1c]
	mov r1, #3
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	str r4, [sp]
	mov r0, #0x22
	str r0, [sp, #4]
	mov r1, #0
	str r1, [sp, #8]
	ldr r0, =0x00010200 // _022319FC
	str r1, [sp, #0xc]
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	str r1, [sp, #0x1c]
	mov r1, #3
	ldr r0, [sp, #0x20]
	add r2, r1, #0
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	mov r1, #3
	str r4, [sp]
	mov r0, #0x23
	str r0, [sp, #4]
	mov r0, #0xe0
	str r0, [sp, #8]
	mov r0, #0
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _022319FC
	add r2, r1, #0
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	mov r0, #2
	str r0, [sp, #0x1c]
	ldr r0, [sp, #0x20]
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	mov r2, #7
	ldrsb r2, [r5, r2]
	ldr r0, [sp, #0x24]
	mov r1, #0x68
	add r2, r5, r2
	ldrb r2, [r2, #0xc]
	bl FrontierRecord_Get
	cmp r0, #0
	bne _022318A6
	mov r0, #0x1e
	b _022318A8
_022318A6:
	mov r0, #0x1f
_022318A8:
	str r4, [sp]
	str r0, [sp, #4]
	mov r1, #0
	str r1, [sp, #8]
	ldr r0, =0x00010200 // _022319FC
	str r1, [sp, #0xc]
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	str r1, [sp, #0x1c]
	ldr r0, [sp, #0x20]
	mov r1, #3
	mov r2, #4
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	mov r2, #7
	ldrsb r2, [r5, r2]
	ldr r0, [sp, #0x24]
	mov r1, #0x77
	add r2, r5, r2
	ldrb r2, [r2, #0xc]
	bl FrontierRecord_Get
	add r1, r0, #0
	add r0, r4, #0
	bl WFNOTE_DRAW_NumberSetWordset
	str r4, [sp]
	mov r0, #0x26
	str r0, [sp, #4]
	mov r0, #0x70
	str r0, [sp, #8]
	mov r0, #0
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _022319FC
	mov r1, #3
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	mov r0, #1
	str r0, [sp, #0x1c]
	ldr r0, [sp, #0x20]
	mov r2, #4
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	mov r2, #7
	ldrsb r2, [r5, r2]
	ldr r0, [sp, #0x24]
	mov r1, #0x79
	add r2, r5, r2
	ldrb r2, [r2, #0xc]
	bl FrontierRecord_Get
	add r1, r0, #0
	add r0, r4, #0
	bl WFNOTE_DRAW_NumberSetWordset
	str r4, [sp]
	mov r0, #0x27
	str r0, [sp, #4]
	mov r0, #0xe0
	str r0, [sp, #8]
	mov r0, #0
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _022319FC
	mov r1, #3
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	mov r0, #2
	str r0, [sp, #0x1c]
	ldr r0, [sp, #0x20]
	mov r2, #4
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	str r4, [sp]
	mov r0, #0x20
	str r0, [sp, #4]
	mov r1, #0
	str r1, [sp, #8]
	mov r0, #0x10
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _022319FC
	mov r2, #4
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	str r1, [sp, #0x1c]
	ldr r0, [sp, #0x20]
	mov r1, #3
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	mov r2, #7
	ldrsb r2, [r5, r2]
	ldr r0, [sp, #0x24]
	mov r1, #0x76
	add r2, r5, r2
	ldrb r2, [r2, #0xc]
	bl FrontierRecord_Get
	add r1, r0, #0
	add r0, r4, #0
	bl WFNOTE_DRAW_NumberSetWordset
	str r4, [sp]
	mov r0, #0x26
	str r0, [sp, #4]
	mov r0, #0x70
	str r0, [sp, #8]
	mov r0, #0x10
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _022319FC
	mov r1, #3
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	mov r0, #1
	str r0, [sp, #0x1c]
	ldr r0, [sp, #0x20]
	mov r2, #4
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	mov r2, #7
	ldrsb r2, [r5, r2]
	ldr r0, [sp, #0x24]
	mov r1, #0x78
	add r2, r5, r2
	ldrb r2, [r2, #0xc]
	bl FrontierRecord_Get
	add r1, r0, #0
	add r0, r4, #0
	bl WFNOTE_DRAW_NumberSetWordset
	str r4, [sp]
	mov r0, #0x27
	str r0, [sp, #4]
	mov r0, #0xe0
	str r0, [sp, #8]
	mov r0, #0x10
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _022319FC
	mov r1, #3
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	mov r0, #2
	str r0, [sp, #0x1c]
	ldr r0, [sp, #0x20]
	mov r2, #4
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	ldr r0, [sp, #0x20]
	mov r1, #3
	bl WFNOTE_FRIENDINFO_DRAWAREA_MSGBmpOnVReq
	add r0, r7, #0
	bl STRBUF_Delete
	add r0, r6, #0
	bl STRBUF_Delete
	add sp, #0x28
	pop {r3, r4, r5, r6, r7, pc}
	// .align 2, 0
// _022319FC: .4byte 0x00010200
}
#endif

#ifdef NONEQUIVALENT
static void WFNOTE_FRIENDINFO_DRAW_Page03( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	STRBUF* p_str;
	STRBUF* p_tmp;
	FRONTIER_SAVEWORK* p_fsave;
	u32 num;
	BOOL clear_flag;
	u32 stridx;

	p_fsave = SaveData_GetFrontier( p_data->p_save );
	p_str = STRBUF_Create( WFNOTE_STRBUF_SIZE, heapID );
	p_tmp = STRBUF_Create( WFNOTE_STRBUF_SIZE, heapID );


	// タイトル
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 4, WFNOTE_FRIENDINFO_PAGE03_BA,
			p_data, p_draw, msg_wifi_note_38, 
			WFNOTE_FRIENDINFO_PAGE03_TT_X, WFNOTE_FRIENDINFO_PAGE03_TT_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp );
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 4, WFNOTE_FRIENDINFO_PAGE03_BA,
			p_data, p_draw, msg_wifi_note_46, 
			WFNOTE_FRIENDINFO_PAGE03_CPT_X, WFNOTE_FRIENDINFO_PAGE03_CPT_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp );

	// 前回
	clear_flag = FrontierRecord_Get(p_fsave, FRID_CASTLE_MULTI_WIFI_CLEAR_BIT, p_data->idx.fridx[p_data->select_listidx]);
	if( clear_flag == FALSE ){	
		stridx = msg_wifi_note_41;
	}else{
		stridx = msg_wifi_note_58;
	}
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 4, WFNOTE_FRIENDINFO_PAGE03_BA,
			p_data, p_draw, stridx, 
			WFNOTE_FRIENDINFO_PAGE03_CPLT_X, WFNOTE_FRIENDINFO_PAGE03_CPL_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp );
	// くりあ
	num = FrontierRecord_Get(p_fsave, FRID_CASTLE_MULTI_WIFI_RENSHOU_CNT, p_data->idx.fridx[p_data->select_listidx]);
	WFNOTE_DRAW_NumberSetWordset( p_draw, num );
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 4, WFNOTE_FRIENDINFO_PAGE03_BA,
			p_data, p_draw, msg_wifi_note_51, 
			WFNOTE_FRIENDINFO_PAGE03_CPLN0_X, WFNOTE_FRIENDINFO_PAGE03_CPL_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp );
	// CP
	num = FrontierRecord_Get(p_fsave, FRID_CASTLE_MULTI_WIFI_CP, p_data->idx.fridx[p_data->select_listidx]);
	WFNOTE_DRAW_NumberSetWordset( p_draw, num );
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 4, WFNOTE_FRIENDINFO_PAGE03_BA,
			p_data, p_draw, msg_wifi_note_50, 
			WFNOTE_FRIENDINFO_PAGE03_CPLN1_X, WFNOTE_FRIENDINFO_PAGE03_CPL_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp );


	// 最高
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 4, WFNOTE_FRIENDINFO_PAGE03_BA,
			p_data, p_draw, msg_wifi_note_42, 
			WFNOTE_FRIENDINFO_PAGE03_CPMT_X, WFNOTE_FRIENDINFO_PAGE03_CPM_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp );
	// くりあ
	num = FrontierRecord_Get(p_fsave, FRID_CASTLE_MULTI_WIFI_RENSHOU, p_data->idx.fridx[p_data->select_listidx]);
	WFNOTE_DRAW_NumberSetWordset( p_draw, num );
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 4, WFNOTE_FRIENDINFO_PAGE03_BA,
			p_data, p_draw, msg_wifi_note_51, 
			WFNOTE_FRIENDINFO_PAGE03_CPMN0_X, WFNOTE_FRIENDINFO_PAGE03_CPM_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp );
	// CP
	num = FrontierRecord_Get(p_fsave, FRID_CASTLE_MULTI_WIFI_REMAINDER_CP, p_data->idx.fridx[p_data->select_listidx]);
	WFNOTE_DRAW_NumberSetWordset( p_draw, num );
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 4, WFNOTE_FRIENDINFO_PAGE03_BA,
			p_data, p_draw, msg_wifi_note_50, 
			WFNOTE_FRIENDINFO_PAGE03_CPMN1_X, WFNOTE_FRIENDINFO_PAGE03_CPM_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp );


	WFNOTE_FRIENDINFO_DRAWAREA_MSGBmpOnVReq( p_wk, 4 );


	STRBUF_Delete( p_str );
	STRBUF_Delete( p_tmp );
}
#else
asm static void WFNOTE_FRIENDINFO_DRAW_Page03( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	push {r3, r4, r5, r6, r7, lr}
	sub sp, #0x28
	add r5, r1, #0
	str r0, [sp, #0x20]
	ldr r0, [r5, #0]
	add r6, r3, #0
	add r4, r2, #0
	bl SaveData_GetFrontier
	str r0, [sp, #0x24]
	mov r0, #0x80
	add r1, r6, #0
	bl STRBUF_Create
	add r7, r0, #0
	mov r0, #0x80
	add r1, r6, #0
	bl STRBUF_Create
	add r6, r0, #0
	str r4, [sp]
	mov r0, #0x1b
	str r0, [sp, #4]
	mov r2, #0
	str r2, [sp, #8]
	ldr r0, =0x00010200 // _02231BDC
	str r2, [sp, #0xc]
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	ldr r0, [sp, #0x20]
	str r2, [sp, #0x1c]
	mov r1, #4
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	str r4, [sp]
	mov r0, #0x24
	str r0, [sp, #4]
	mov r0, #0xe0
	str r0, [sp, #8]
	mov r0, #0x14
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _02231BDC
	mov r1, #4
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	mov r0, #2
	str r0, [sp, #0x1c]
	ldr r0, [sp, #0x20]
	mov r2, #0
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	mov r2, #7
	ldrsb r2, [r5, r2]
	ldr r0, [sp, #0x24]
	mov r1, #0x6c
	add r2, r5, r2
	ldrb r2, [r2, #0xc]
	bl FrontierRecord_Get
	cmp r0, #0
	bne _02231A86
	mov r0, #0x1e
	b _02231A88
_02231A86:
	mov r0, #0x1f
_02231A88:
	str r4, [sp]
	str r0, [sp, #4]
	mov r2, #0
	str r2, [sp, #8]
	mov r0, #0x30
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _02231BDC
	mov r1, #4
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	ldr r0, [sp, #0x20]
	add r3, r5, #0
	str r2, [sp, #0x1c]
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	mov r2, #7
	ldrsb r2, [r5, r2]
	ldr r0, [sp, #0x24]
	mov r1, #0x87
	add r2, r5, r2
	ldrb r2, [r2, #0xc]
	bl FrontierRecord_Get
	add r1, r0, #0
	add r0, r4, #0
	bl WFNOTE_DRAW_NumberSetWordset
	str r4, [sp]
	mov r0, #0x29
	str r0, [sp, #4]
	mov r0, #0x70
	str r0, [sp, #8]
	mov r0, #0x30
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _02231BDC
	mov r1, #4
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	mov r0, #1
	str r0, [sp, #0x1c]
	ldr r0, [sp, #0x20]
	mov r2, #0
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	mov r2, #7
	ldrsb r2, [r5, r2]
	ldr r0, [sp, #0x24]
	mov r1, #0x88
	add r2, r5, r2
	ldrb r2, [r2, #0xc]
	bl FrontierRecord_Get
	add r1, r0, #0
	add r0, r4, #0
	bl WFNOTE_DRAW_NumberSetWordset
	str r4, [sp]
	mov r0, #0x28
	str r0, [sp, #4]
	mov r0, #0xe0
	str r0, [sp, #8]
	mov r0, #0x30
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _02231BDC
	mov r1, #4
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	mov r0, #2
	str r0, [sp, #0x1c]
	ldr r0, [sp, #0x20]
	mov r2, #0
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	str r4, [sp]
	mov r0, #0x20
	str r0, [sp, #4]
	mov r2, #0
	str r2, [sp, #8]
	mov r0, #0x48
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _02231BDC
	mov r1, #4
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	ldr r0, [sp, #0x20]
	add r3, r5, #0
	str r2, [sp, #0x1c]
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	mov r2, #7
	ldrsb r2, [r5, r2]
	ldr r0, [sp, #0x24]
	mov r1, #0x86
	add r2, r5, r2
	ldrb r2, [r2, #0xc]
	bl FrontierRecord_Get
	add r1, r0, #0
	add r0, r4, #0
	bl WFNOTE_DRAW_NumberSetWordset
	str r4, [sp]
	mov r0, #0x29
	str r0, [sp, #4]
	mov r0, #0x70
	str r0, [sp, #8]
	mov r0, #0x48
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _02231BDC
	mov r1, #4
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	mov r0, #1
	str r0, [sp, #0x1c]
	ldr r0, [sp, #0x20]
	mov r2, #0
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	mov r2, #7
	ldrsb r2, [r5, r2]
	ldr r0, [sp, #0x24]
	mov r1, #0x8a
	add r2, r5, r2
	ldrb r2, [r2, #0xc]
	bl FrontierRecord_Get
	add r1, r0, #0
	add r0, r4, #0
	bl WFNOTE_DRAW_NumberSetWordset
	str r4, [sp]
	mov r0, #0x28
	str r0, [sp, #4]
	mov r0, #0xe0
	str r0, [sp, #8]
	mov r0, #0x48
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _02231BDC
	mov r1, #4
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	mov r0, #2
	str r0, [sp, #0x1c]
	ldr r0, [sp, #0x20]
	mov r2, #0
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	ldr r0, [sp, #0x20]
	mov r1, #4
	bl WFNOTE_FRIENDINFO_DRAWAREA_MSGBmpOnVReq
	add r0, r7, #0
	bl STRBUF_Delete
	add r0, r6, #0
	bl STRBUF_Delete
	add sp, #0x28
	pop {r3, r4, r5, r6, r7, pc}
	nop
// _02231BDC: .4byte 0x00010200
}
#endif

#ifdef NONEQUIVALENT
static void WFNOTE_FRIENDINFO_DRAW_Page04( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	STRBUF* p_str;
	STRBUF* p_tmp;
	STRBUF* p_monsstr;
	FRONTIER_SAVEWORK* p_fsave;
	u32 num;
	BOOL clear_flag;
	u32 stridx;

	p_fsave = SaveData_GetFrontier( p_data->p_save );
	p_str = STRBUF_Create( WFNOTE_STRBUF_SIZE, heapID );
	p_tmp = STRBUF_Create( WFNOTE_STRBUF_SIZE, heapID );


	// タイトル
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 5, WFNOTE_FRIENDINFO_PAGE04_BA,
			p_data, p_draw, msg_wifi_note_39, 
			WFNOTE_FRIENDINFO_PAGE04_TT_X, WFNOTE_FRIENDINFO_PAGE04_TT_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp );

	num = FrontierRecord_Get(p_fsave, FRID_STAGE_MULTI_WIFI_MONSNO, p_data->idx.fridx[p_data->select_listidx]);
	p_monsstr = MSGDAT_UTIL_GetMonsName( num, heapID );
	GF_STR_PrintColor( &p_wk->p_msg[5][WFNOTE_FRIENDINFO_PAGE04_BA],
			FONT_SYSTEM, p_monsstr,
			WFNOTE_FRIENDINFO_PAGE04_TT_X, WFNOTE_FRIENDINFO_PAGE04_TT2_Y,
			MSG_NO_PUT, WFNOTE_COL_BLACK, NULL);
	STRBUF_Delete( p_monsstr );

	// 前回
	clear_flag = FrontierRecord_Get(p_fsave, FRID_STAGE_MULTI_WIFI_CLEAR_BIT, p_data->idx.fridx[p_data->select_listidx]);
	if( clear_flag == FALSE ){	
		stridx = msg_wifi_note_41;
	}else{
		stridx = msg_wifi_note_58;
	}
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 5, WFNOTE_FRIENDINFO_PAGE04_BA,
			p_data, p_draw, stridx, 
			WFNOTE_FRIENDINFO_PAGE04_LT_X, WFNOTE_FRIENDINFO_PAGE04_L_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp );
	// 値
	num = FrontierRecord_Get(p_fsave, FRID_STAGE_MULTI_WIFI_RENSHOU_CNT, p_data->idx.fridx[p_data->select_listidx]);
	WFNOTE_DRAW_NumberSetWordset( p_draw, num );
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 5, WFNOTE_FRIENDINFO_PAGE04_BA,
			p_data, p_draw, msg_wifi_note_48, 
			WFNOTE_FRIENDINFO_PAGE04_LN_X, WFNOTE_FRIENDINFO_PAGE04_L_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp );

	// 最高
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 5, WFNOTE_FRIENDINFO_PAGE04_BA,
			p_data, p_draw, msg_wifi_note_42, 
			WFNOTE_FRIENDINFO_PAGE04_MT_X, WFNOTE_FRIENDINFO_PAGE04_M_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp );
	// 値
	num = FrontierRecord_Get(p_fsave, FRID_STAGE_MULTI_WIFI_RENSHOU, p_data->idx.fridx[p_data->select_listidx]);
	WFNOTE_DRAW_NumberSetWordset( p_draw, num );
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 5, WFNOTE_FRIENDINFO_PAGE04_BA,
			p_data, p_draw, msg_wifi_note_48, 
			WFNOTE_FRIENDINFO_PAGE04_MN_X, WFNOTE_FRIENDINFO_PAGE04_M_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp );

	WFNOTE_FRIENDINFO_DRAWAREA_MSGBmpOnVReq( p_wk, 5 );

	STRBUF_Delete( p_str );
	STRBUF_Delete( p_tmp );
}
#else
asm static void WFNOTE_FRIENDINFO_DRAW_Page04( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	push {r3, r4, r5, r6, r7, lr}
	sub sp, #0x30
	add r5, r1, #0
	str r0, [sp, #0x20]
	ldr r0, [r5, #0]
	str r3, [sp, #0x24]
	add r4, r2, #0
	bl SaveData_GetFrontier
	str r0, [sp, #0x28]
	ldr r1, [sp, #0x24]
	mov r0, #0x80
	bl STRBUF_Create
	add r7, r0, #0
	ldr r1, [sp, #0x24]
	mov r0, #0x80
	bl STRBUF_Create
	add r6, r0, #0
	str r4, [sp]
	mov r0, #0x1c
	str r0, [sp, #4]
	mov r2, #0
	str r2, [sp, #8]
	ldr r0, =0x00010200 // _02231D54
	str r2, [sp, #0xc]
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	ldr r0, [sp, #0x20]
	mov r1, #5
	add r3, r5, #0
	str r2, [sp, #0x1c]
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	mov r2, #7
	ldrsb r2, [r5, r2]
	ldr r0, [sp, #0x28]
	mov r1, #0x7c
	add r2, r5, r2
	ldrb r2, [r2, #0xc]
	bl FrontierRecord_Get
	ldr r1, [sp, #0x24]
	bl MSGDAT_UTIL_GetMonsName
	str r0, [sp, #0x2c]
	mov r0, #0x18
	str r0, [sp]
	mov r0, #0xff
	str r0, [sp, #4]
	ldr r0, =0x00010200 // _02231D54
	mov r1, #0
	str r0, [sp, #8]
	ldr r0, [sp, #0x20]
	str r1, [sp, #0xc]
	ldr r0, [r0, #0x20]
	ldr r2, [sp, #0x2c]
	add r3, r1, #0
	bl GF_STR_PrintColor
	ldr r0, [sp, #0x2c]
	bl STRBUF_Delete
	mov r2, #7
	ldrsb r2, [r5, r2]
	ldr r0, [sp, #0x28]
	mov r1, #0x6a
	add r2, r5, r2
	ldrb r2, [r2, #0xc]
	bl FrontierRecord_Get
	cmp r0, #0
	bne _02231C7A
	mov r0, #0x1e
	b _02231C7C
_02231C7A:
	mov r0, #0x1f
_02231C7C:
	str r4, [sp]
	str r0, [sp, #4]
	mov r2, #0
	str r2, [sp, #8]
	mov r0, #0x30
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _02231D54
	mov r1, #5
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	ldr r0, [sp, #0x20]
	add r3, r5, #0
	str r2, [sp, #0x1c]
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	mov r2, #7
	ldrsb r2, [r5, r2]
	ldr r0, [sp, #0x28]
	mov r1, #0x7b
	add r2, r5, r2
	ldrb r2, [r2, #0xc]
	bl FrontierRecord_Get
	add r1, r0, #0
	add r0, r4, #0
	bl WFNOTE_DRAW_NumberSetWordset
	str r4, [sp]
	mov r0, #0x26
	str r0, [sp, #4]
	mov r0, #0xa0
	str r0, [sp, #8]
	mov r0, #0x30
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _02231D54
	mov r1, #5
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	mov r0, #2
	str r0, [sp, #0x1c]
	ldr r0, [sp, #0x20]
	mov r2, #0
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	str r4, [sp]
	mov r0, #0x20
	str r0, [sp, #4]
	mov r2, #0
	str r2, [sp, #8]
	mov r0, #0x48
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _02231D54
	mov r1, #5
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	ldr r0, [sp, #0x20]
	add r3, r5, #0
	str r2, [sp, #0x1c]
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	mov r2, #7
	ldrsb r2, [r5, r2]
	ldr r0, [sp, #0x28]
	mov r1, #0x7a
	add r2, r5, r2
	ldrb r2, [r2, #0xc]
	bl FrontierRecord_Get
	add r1, r0, #0
	add r0, r4, #0
	bl WFNOTE_DRAW_NumberSetWordset
	str r4, [sp]
	mov r0, #0x26
	str r0, [sp, #4]
	mov r0, #0xa0
	str r0, [sp, #8]
	mov r0, #0x48
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _02231D54
	mov r1, #5
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	mov r0, #2
	str r0, [sp, #0x1c]
	ldr r0, [sp, #0x20]
	mov r2, #0
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	ldr r0, [sp, #0x20]
	mov r1, #5
	bl WFNOTE_FRIENDINFO_DRAWAREA_MSGBmpOnVReq
	add r0, r7, #0
	bl STRBUF_Delete
	add r0, r6, #0
	bl STRBUF_Delete
	add sp, #0x30
	pop {r3, r4, r5, r6, r7, pc}
	nop
// _02231D54: .4byte 0x00010200
}
#endif

#ifdef NONEQUIVALENT
static void WFNOTE_FRIENDINFO_DRAW_Page05( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	STRBUF* p_str;
	STRBUF* p_tmp;
	FRONTIER_SAVEWORK* p_fsave;
	u32 num;
	BOOL clear_flag;
	u32 stridx;

	p_fsave = SaveData_GetFrontier( p_data->p_save );
	p_str = STRBUF_Create( WFNOTE_STRBUF_SIZE, heapID );
	p_tmp = STRBUF_Create( WFNOTE_STRBUF_SIZE, heapID );

	// タイトル
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 6, WFNOTE_FRIENDINFO_PAGE05_BA,
			p_data, p_draw, msg_wifi_note_40, 
			WFNOTE_FRIENDINFO_PAGE05_TT_X, WFNOTE_FRIENDINFO_PAGE05_TT_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp );

	// 前回
	clear_flag = FrontierRecord_Get(p_fsave, FRID_ROULETTE_MULTI_WIFI_CLEAR_BIT, p_data->idx.fridx[p_data->select_listidx]);
	if( clear_flag == FALSE ){	
		stridx = msg_wifi_note_41;
	}else{
		stridx = msg_wifi_note_58;
	}
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 6, WFNOTE_FRIENDINFO_PAGE05_BA,
			p_data, p_draw, stridx, 
			WFNOTE_FRIENDINFO_PAGE05_LT_X, WFNOTE_FRIENDINFO_PAGE05_L_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp );
	// 値
	num = FrontierRecord_Get(p_fsave, FRID_ROULETTE_MULTI_WIFI_RENSHOU_CNT, p_data->idx.fridx[p_data->select_listidx]);
	WFNOTE_DRAW_NumberSetWordset( p_draw, num );
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 6, WFNOTE_FRIENDINFO_PAGE05_BA,
			p_data, p_draw, msg_wifi_note_57, 
			WFNOTE_FRIENDINFO_PAGE05_LN_X, WFNOTE_FRIENDINFO_PAGE05_L_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp );

	// 最高
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 6, WFNOTE_FRIENDINFO_PAGE05_BA,
			p_data, p_draw, msg_wifi_note_42, 
			WFNOTE_FRIENDINFO_PAGE05_MT_X, WFNOTE_FRIENDINFO_PAGE05_M_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp );
	// 値
	num = FrontierRecord_Get(p_fsave, FRID_ROULETTE_MULTI_WIFI_RENSHOU, p_data->idx.fridx[p_data->select_listidx]);
	WFNOTE_DRAW_NumberSetWordset( p_draw, num );
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 6, WFNOTE_FRIENDINFO_PAGE05_BA,
			p_data, p_draw, msg_wifi_note_57, 
			WFNOTE_FRIENDINFO_PAGE05_MN_X, WFNOTE_FRIENDINFO_PAGE05_M_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp );



	WFNOTE_FRIENDINFO_DRAWAREA_MSGBmpOnVReq( p_wk, 6 );

	STRBUF_Delete( p_str );
	STRBUF_Delete( p_tmp );
}
#else
asm static void WFNOTE_FRIENDINFO_DRAW_Page05( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	push {r3, r4, r5, r6, r7, lr}
	sub sp, #0x28
	add r5, r1, #0
	str r0, [sp, #0x20]
	ldr r0, [r5, #0]
	add r6, r3, #0
	add r4, r2, #0
	bl SaveData_GetFrontier
	str r0, [sp, #0x24]
	mov r0, #0x80
	add r1, r6, #0
	bl STRBUF_Create
	add r7, r0, #0
	mov r0, #0x80
	add r1, r6, #0
	bl STRBUF_Create
	add r6, r0, #0
	str r4, [sp]
	mov r0, #0x1d
	str r0, [sp, #4]
	mov r2, #0
	str r2, [sp, #8]
	ldr r0, =0x00010200 // _02231E90
	str r2, [sp, #0xc]
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	ldr r0, [sp, #0x20]
	mov r1, #6
	add r3, r5, #0
	str r2, [sp, #0x1c]
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	mov r2, #7
	ldrsb r2, [r5, r2]
	ldr r0, [sp, #0x24]
	mov r1, #0x6e
	add r2, r5, r2
	ldrb r2, [r2, #0xc]
	bl FrontierRecord_Get
	cmp r0, #0
	bne _02231DB8
	mov r0, #0x1e
	b _02231DBA
_02231DB8:
	mov r0, #0x1f
_02231DBA:
	str r4, [sp]
	str r0, [sp, #4]
	mov r2, #0
	str r2, [sp, #8]
	mov r0, #0x18
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _02231E90
	mov r1, #6
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	ldr r0, [sp, #0x20]
	add r3, r5, #0
	str r2, [sp, #0x1c]
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	mov r2, #7
	ldrsb r2, [r5, r2]
	ldr r0, [sp, #0x24]
	mov r1, #0x8f
	add r2, r5, r2
	ldrb r2, [r2, #0xc]
	bl FrontierRecord_Get
	add r1, r0, #0
	add r0, r4, #0
	bl WFNOTE_DRAW_NumberSetWordset
	str r4, [sp]
	mov r0, #0x2a
	str r0, [sp, #4]
	mov r0, #0xa0
	str r0, [sp, #8]
	mov r0, #0x18
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _02231E90
	mov r1, #6
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	mov r0, #2
	str r0, [sp, #0x1c]
	ldr r0, [sp, #0x20]
	mov r2, #0
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	str r4, [sp]
	mov r0, #0x20
	str r0, [sp, #4]
	mov r2, #0
	str r2, [sp, #8]
	mov r0, #0x30
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _02231E90
	mov r1, #6
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	ldr r0, [sp, #0x20]
	add r3, r5, #0
	str r2, [sp, #0x1c]
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	mov r2, #7
	ldrsb r2, [r5, r2]
	ldr r0, [sp, #0x24]
	mov r1, #0x8e
	add r2, r5, r2
	ldrb r2, [r2, #0xc]
	bl FrontierRecord_Get
	add r1, r0, #0
	add r0, r4, #0
	bl WFNOTE_DRAW_NumberSetWordset
	str r4, [sp]
	mov r0, #0x2a
	str r0, [sp, #4]
	mov r0, #0xa0
	str r0, [sp, #8]
	mov r0, #0x30
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _02231E90
	mov r1, #6
	str r0, [sp, #0x10]
	str r7, [sp, #0x14]
	str r6, [sp, #0x18]
	mov r0, #2
	str r0, [sp, #0x1c]
	ldr r0, [sp, #0x20]
	mov r2, #0
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	ldr r0, [sp, #0x20]
	mov r1, #6
	bl WFNOTE_FRIENDINFO_DRAWAREA_MSGBmpOnVReq
	add r0, r7, #0
	bl STRBUF_Delete
	add r0, r6, #0
	bl STRBUF_Delete
	add sp, #0x28
	pop {r3, r4, r5, r6, r7, pc}
	// .align 2, 0
// _02231E90: .4byte 0x00010200
}
#endif

// ミニゲーム
#ifdef NONEQUIVALENT
static void WFNOTE_FRIENDINFO_DRAW_Page06( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	STRBUF* p_str;
	STRBUF* p_tmp;
	FRONTIER_SAVEWORK* p_fsave;
	WIFI_LIST* p_wifilist;
	u32 num;

	p_fsave		= SaveData_GetFrontier( p_data->p_save );
	p_wifilist	= SaveData_GetWifiListData( p_data->p_save );
	p_str = STRBUF_Create( WFNOTE_STRBUF_SIZE, heapID );
	p_tmp = STRBUF_Create( WFNOTE_STRBUF_SIZE, heapID );

	// タイトル
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 1, WFNOTE_FRIENDINFO_PAGE06_BA,
			p_data, p_draw, msg_wifi_note_52, 
			WFNOTE_FRIENDINFO_PAGE06_TT_X, WFNOTE_FRIENDINFO_PAGE06_TT_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp );

	// たまいれ
	WFNOTE_DRAW_WflbyGameSetWordSet( p_draw, WFLBY_GAME_BALLSLOW );
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 1, WFNOTE_FRIENDINFO_PAGE06_BA,
			p_data, p_draw, msg_wifi_note_53, 
			WFNOTE_FRIENDINFO_PAGE06_BC_X, WFNOTE_FRIENDINFO_PAGE06_BC_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp );
	// 値
	num = WifiList_GetFriendInfo( p_wifilist, p_data->idx.fridx[p_data->select_listidx], WIFILIST_FRIEND_BALLSLOW_NUM );
	WFNOTE_DRAW_NumberSetWordset( p_draw, num );
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 1, WFNOTE_FRIENDINFO_PAGE06_BA,
			p_data, p_draw, msg_wifi_note_56, 
			WFNOTE_FRIENDINFO_PAGE06_BC_N_X, WFNOTE_FRIENDINFO_PAGE06_BC_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp );

	// たまのり
	WFNOTE_DRAW_WflbyGameSetWordSet( p_draw, WFLBY_GAME_BALANCEBALL );
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 1, WFNOTE_FRIENDINFO_PAGE06_BA,
			p_data, p_draw, msg_wifi_note_53, 
			WFNOTE_FRIENDINFO_PAGE06_BB_X, WFNOTE_FRIENDINFO_PAGE06_BB_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp );
	// 値
	num = WifiList_GetFriendInfo( p_wifilist, p_data->idx.fridx[p_data->select_listidx], WIFILIST_FRIEND_BALANCEBALL_NUM );
	WFNOTE_DRAW_NumberSetWordset( p_draw, num );
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 1, WFNOTE_FRIENDINFO_PAGE06_BA,
			p_data, p_draw, msg_wifi_note_56, 
			WFNOTE_FRIENDINFO_PAGE06_BB_N_X, WFNOTE_FRIENDINFO_PAGE06_BB_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp );

	// ふうせん
	WFNOTE_DRAW_WflbyGameSetWordSet( p_draw, WFLBY_GAME_BALLOON );
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 1, WFNOTE_FRIENDINFO_PAGE06_BA,
			p_data, p_draw, msg_wifi_note_53, 
			WFNOTE_FRIENDINFO_PAGE06_BL_X, WFNOTE_FRIENDINFO_PAGE06_BL_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp );
	// 値
	num = WifiList_GetFriendInfo( p_wifilist, p_data->idx.fridx[p_data->select_listidx], WIFILIST_FRIEND_BALLOON_NUM );
	WFNOTE_DRAW_NumberSetWordset( p_draw, num );
	WFNOTE_FRIENDINFO_DRAW_Bmp( p_wk, 1, WFNOTE_FRIENDINFO_PAGE06_BA,
			p_data, p_draw, msg_wifi_note_56, 
			WFNOTE_FRIENDINFO_PAGE06_BL_N_X, WFNOTE_FRIENDINFO_PAGE06_BL_Y,
			WFNOTE_COL_BLACK, p_str, p_tmp );


	WFNOTE_FRIENDINFO_DRAWAREA_MSGBmpOnVReq( p_wk, 1 );

	STRBUF_Delete( p_str );
	STRBUF_Delete( p_tmp );
}
#else
asm static void WFNOTE_FRIENDINFO_DRAW_Page06( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 heapID )
{
	push {r3, r4, r5, r6, r7, lr}
	sub sp, #0x28
	add r5, r1, #0
	str r0, [sp, #0x20]
	ldr r0, [r5, #0]
	add r4, r2, #0
	add r7, r3, #0
	bl SaveData_GetFrontier
	ldr r0, [r5, #0]
	bl SaveData_GetWifiListData
	str r0, [sp, #0x24]
	mov r0, #0x80
	add r1, r7, #0
	bl STRBUF_Create
	add r6, r0, #0
	mov r0, #0x80
	add r1, r7, #0
	bl STRBUF_Create
	add r7, r0, #0
	str r4, [sp]
	mov r0, #0x2b
	str r0, [sp, #4]
	mov r2, #0
	str r2, [sp, #8]
	ldr r0, =0x00010200 // _02232034
	str r2, [sp, #0xc]
	str r0, [sp, #0x10]
	str r6, [sp, #0x14]
	str r7, [sp, #0x18]
	ldr r0, [sp, #0x20]
	mov r1, #1
	add r3, r5, #0
	str r2, [sp, #0x1c]
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	add r0, r4, #0
	mov r1, #0
	bl WFNOTE_DRAW_WflbyGameSetWordSet
	str r4, [sp]
	mov r0, #0x2c
	str r0, [sp, #4]
	mov r2, #0
	str r2, [sp, #8]
	mov r0, #0x18
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _02232034
	mov r1, #1
	str r0, [sp, #0x10]
	str r6, [sp, #0x14]
	str r7, [sp, #0x18]
	ldr r0, [sp, #0x20]
	add r3, r5, #0
	str r2, [sp, #0x1c]
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	mov r1, #7
	ldrsb r1, [r5, r1]
	ldr r0, [sp, #0x24]
	mov r2, #0xa
	add r1, r5, r1
	ldrb r1, [r1, #0xc]
	bl WifiList_GetFriendInfo
	add r1, r0, #0
	add r0, r4, #0
	bl WFNOTE_DRAW_NumberSetWordset
	str r4, [sp]
	mov r0, #0x2d
	str r0, [sp, #4]
	mov r0, #0xa0
	str r0, [sp, #8]
	mov r0, #0x18
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _02232034
	mov r1, #1
	str r0, [sp, #0x10]
	str r6, [sp, #0x14]
	str r7, [sp, #0x18]
	mov r0, #2
	str r0, [sp, #0x1c]
	ldr r0, [sp, #0x20]
	mov r2, #0
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	add r0, r4, #0
	mov r1, #1
	bl WFNOTE_DRAW_WflbyGameSetWordSet
	str r4, [sp]
	mov r0, #0x2c
	str r0, [sp, #4]
	mov r2, #0
	str r2, [sp, #8]
	mov r0, #0x30
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _02232034
	mov r1, #1
	str r0, [sp, #0x10]
	str r6, [sp, #0x14]
	str r7, [sp, #0x18]
	ldr r0, [sp, #0x20]
	add r3, r5, #0
	str r2, [sp, #0x1c]
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	mov r1, #7
	ldrsb r1, [r5, r1]
	ldr r0, [sp, #0x24]
	mov r2, #0xb
	add r1, r5, r1
	ldrb r1, [r1, #0xc]
	bl WifiList_GetFriendInfo
	add r1, r0, #0
	add r0, r4, #0
	bl WFNOTE_DRAW_NumberSetWordset
	str r4, [sp]
	mov r0, #0x2d
	str r0, [sp, #4]
	mov r0, #0xa0
	str r0, [sp, #8]
	mov r0, #0x30
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _02232034
	mov r1, #1
	str r0, [sp, #0x10]
	str r6, [sp, #0x14]
	str r7, [sp, #0x18]
	mov r0, #2
	str r0, [sp, #0x1c]
	ldr r0, [sp, #0x20]
	mov r2, #0
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	add r0, r4, #0
	mov r1, #2
	bl WFNOTE_DRAW_WflbyGameSetWordSet
	str r4, [sp]
	mov r0, #0x2c
	str r0, [sp, #4]
	mov r2, #0
	str r2, [sp, #8]
	mov r0, #0x48
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _02232034
	mov r1, #1
	str r0, [sp, #0x10]
	str r6, [sp, #0x14]
	str r7, [sp, #0x18]
	ldr r0, [sp, #0x20]
	add r3, r5, #0
	str r2, [sp, #0x1c]
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	mov r1, #7
	ldrsb r1, [r5, r1]
	ldr r0, [sp, #0x24]
	mov r2, #0xc
	add r1, r5, r1
	ldrb r1, [r1, #0xc]
	bl WifiList_GetFriendInfo
	add r1, r0, #0
	add r0, r4, #0
	bl WFNOTE_DRAW_NumberSetWordset
	str r4, [sp]
	mov r0, #0x2d
	str r0, [sp, #4]
	mov r0, #0xa0
	str r0, [sp, #8]
	mov r0, #0x48
	str r0, [sp, #0xc]
	ldr r0, =0x00010200 // _02232034
	mov r1, #1
	str r0, [sp, #0x10]
	str r6, [sp, #0x14]
	str r7, [sp, #0x18]
	mov r0, #2
	str r0, [sp, #0x1c]
	ldr r0, [sp, #0x20]
	mov r2, #0
	add r3, r5, #0
	bl WFNOTE_FRIENDINFO_DRAW_Bmp
	ldr r0, [sp, #0x20]
	mov r1, #1
	bl WFNOTE_FRIENDINFO_DRAWAREA_MSGBmpOnVReq
	add r0, r6, #0
	bl STRBUF_Delete
	add r0, r7, #0
	bl STRBUF_Delete
	add sp, #0x28
	pop {r3, r4, r5, r6, r7, pc}
	nop
// _02232034: .4byte 0x00010200
}
#endif

//----------------------------------------------------------------------------
/**
 *	@brief	表示面のクリア
 *
 *	@param	p_draw	描画システム
 *	@param	cp_scrnarea	スクリーンエリア
 */
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDINFO_DRAW_Clean( WFNOTE_DRAW* p_draw, const WFNOTE_SCRNAREA* cp_scrnarea )
{
	// 背景クリア
	GF_BGL_ScrFill( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_SCROLL ],
			0, 
			MATH_ABS(cp_scrnarea->scrn_x), MATH_ABS(cp_scrnarea->scrn_y), 
			32,
			24, 0 );

	// BG面転送
	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_SCROLL] );

	// スクリーン面もクリア
	GF_BGL_ScrFill( p_draw->p_bgl, sc_WFNOTE_BGFRM[ WFNOTE_BG_MAIN_SCRMSG ],
			0, 
			MATH_ABS(cp_scrnarea->scrn_x), MATH_ABS(cp_scrnarea->scrn_y), 
			32,
			24, 0 );
	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, sc_WFNOTE_BGFRM[WFNOTE_BG_MAIN_SCRMSG] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ビットマップへの文字の描画
 *	
 *	@param	p_wk		描画エリアワーク
 *	@param	page		ページ
 *	@param	bmp			ビットマップインデックス
 *	@param	p_data		データ
 *	@param	p_draw		描画システム
 *	@param	msg_idx		メッセージインデックス
 *	@param	x			ｘ
 *	@param	y			ｙ
 *	@param	col			カラー
 *	@param	p_str		使用するSTRBUF
 *	@param	p_tmp		使用するSTRBUF
 */
//-----------------------------------------------------------------------------
#ifdef NONEQUIVALENT
static void WFNOTE_FRIENDINFO_DRAW_Bmp( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, u32 page, u32 bmp, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 msg_idx, u32 x, u32 y, u32 col, STRBUF* p_str, STRBUF* p_tmp, int align )
{
	MSGMAN_GetString( p_draw->p_msgman, msg_idx, p_tmp );
	WORDSET_ExpandStr( p_draw->p_wordset, p_str, p_tmp );
    GF_STR_PrintColor( &p_wk->p_msg[page][bmp], FONT_SYSTEM, 
			p_str, x, y, MSG_NO_PUT, col, NULL);
}
#else
// MatchComment: this prototype is incorrect, it should have an extra arg after p_tmp 
asm static void WFNOTE_FRIENDINFO_DRAW_Bmp( WFNOTE_FRIENDINFO_DRAWAREA* p_wk, u32 page, u32 bmp, WFNOTE_DATA* p_data, WFNOTE_DRAW* p_draw, u32 msg_idx, u32 x, u32 y, u32 col, STRBUF* p_str, STRBUF* p_tmp, int align )
{
	push {r3, r4, r5, r6, r7, lr}
	sub sp, #0x18
	add r7, r0, #0
	mov r0, #0x63
	str r1, [sp, #0x10]
	str r2, [sp, #0x14]
	ldr r6, [sp, #0x30]
	lsl r0, r0, #2
	ldr r0, [r6, r0]
	ldr r1, [sp, #0x34]
	ldr r2, [sp, #0x48]
	ldr r5, [sp, #0x38]
	ldr r4, [sp, #0x44]
	bl MSGMAN_GetString
	mov r0, #0x62
	lsl r0, r0, #2
	ldr r0, [r6, r0]
	ldr r2, [sp, #0x48]
	add r1, r4, #0
	bl WORDSET_ExpandStr
	ldr r0, [sp, #0x4c]
	cmp r0, #1
	beq _022320F0
	cmp r0, #2
	beq _02232102
	b _0223210E
_022320F0:
	mov r0, #0
	add r1, r4, #0
	add r2, r0, #0
	bl FontProc_GetPrintStrWidth
	add r0, r0, #1
	lsr r0, r0, #1
	sub r5, r5, r0
	b _0223210E
_02232102:
	mov r0, #0
	add r1, r4, #0
	add r2, r0, #0
	bl FontProc_GetPrintStrWidth
	sub r5, r5, r0
_0223210E:
	ldr r0, [sp, #0x3c]
	mov r1, #0
	str r0, [sp]
	mov r0, #0xff
	str r0, [sp, #4]
	ldr r0, [sp, #0x40]
	add r3, r5, #0
	str r0, [sp, #8]
	ldr r0, [sp, #0x10]
	str r1, [sp, #0xc]
	lsl r0, r0, #2
	add r0, r7, r0
	ldr r2, [r0, #0xc]
	ldr r0, [sp, #0x14]
	lsl r0, r0, #4
	add r0, r2, r0
	add r2, r4, #0
	bl GF_STR_PrintColor
	add sp, #0x18
	pop {r3, r4, r5, r6, r7, pc}
}
#endif

//----------------------------------------------------------------------------
/**
 *	@brief	ページ変更
 *
 *	@param	p_wk		ワーク
 *	@param	page		ページ
 */	
//-----------------------------------------------------------------------------
static void WFNOTE_FRIENDINFO_PageChange( WFNOTE_FRIENDINFO* p_wk, s32 add )
{
	int page_num;

	// フロンティアを描画するかチェック
	if( p_wk->frontier_draw == TRUE ){
		page_num = WFNOTE_FRIENDINFO_PAGE_NUM;
	}else{
		page_num = WFNOTE_FRIENDINFO_FRONTIEROFFPAGE_NUM;
	}
	
	if( add > 0 ){
		p_wk->lastpage = p_wk->page;
		p_wk->page = (p_wk->page + add) % page_num;
	}else if( add < 0 ){
		p_wk->lastpage = p_wk->page;
		p_wk->page = p_wk->page + add;
		if( p_wk->page < 0 ){
			p_wk->page += page_num;
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ポフィンケース持っているかチェック
 *	
 *	@param	p_data	ワーク
 */
//-----------------------------------------------------------------------------
static BOOL WFNOTE_FRIENDINFO_PofinCaseCheck( WFNOTE_DATA* p_data, u32 heapID )
{
	// ポルトケースがあるかチェック
	if( !MyItem_GetItemNum( SaveData_GetMyItem(p_data->p_save),ITEM_POFINKEESU, heapID ) ){
		return FALSE;
	}
	return TRUE;
}
