//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifi_p2pmatchfour.c
 *	@brief		wifi４人マッチング専用画面
 *	@author		tomoya takahashi
 *	@data		2007.05.22
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#include "wifi/dwc_rap.h"
#include "wifi/vchat.h"
#include "system/bmp_list.h"

#include "system/msgdata_util.h"
#include "system/wordset.h"
#include "msgdata/msg.naix"
#include "msgdata/msg_wifi_lobby.h"

#include "system/window.h"
#include "system/bmp_menu.h"
#include "system/wipe.h"
#include "system/brightness.h"
#include "system/fontproc.h"

#include "system/pm_overlay.h"

#include "communication/communication.h"
#include "communication/comm_state.h"

#include "comm_command_wfp2pmf.h"
#include "comm_command_wfp2pmf_func.h"

#include "application/wifi_2dmap/wf2dmap_obj.h"
#include "application/wifi_2dmap/wf2dmap_objdraw.h"

#include "wifi/dwc_rapfriend.h"

#include "system/snd_tool.h"
#include "wifi_p2pmatch_se.h"


#define __WIFI_P2PMATCHFOUR_H_GLOBAL
#include "wifi_p2pmatchfour.h"

#include "application\namein.h"
#include "wifi_p2pmatch_local.h"
#include "wifi_p2pmatchfour_local.h"

#include "wifip2pmatch.naix"			// グラフィックアーカイブ定義


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

// セルアクター
#define WIFI_P2PMATCH_RESNUM	(4)	// リソース数
#define WIFI_P2PMATCH_CLACTWK_NUM	(4)		// アクターワーク数
#define WIFI_P2PMATCH_LOADRESNUM	(4)		// 読み込むリソース数
#define WIFI_P2PMATCH_VRAMMAINSIZ	(128*1024)
#define WIFI_P2PMATCH_VRAMSUBSIZ	(16*1024)
#define WIFI_P2PMATCH_VRAMTRMAN_NUM	( 16 )

// メッセージ表示時間
#define WIFI_P2PMATCH_MSG_WAIT	( 30 )


#define _TIMING_GAME_CHECK  (13)// つながった直後
#define _TIMING_GAME_CHECK2  (14)// つながった直後
#define _TIMING_GAME_START  (15)// タイミングをそろえる
#define _TIMING_GAME_START2  (18)// タイミングをそろえる
#define _TIMING_BATTLE_END  (16)// タイミングをそろえる


#define RECONNECT_WAIT	(90)

#define KO_ENTRYNUM_CHECK_WAIT	(10*30)

#define CONNECT_TIMEOUT		(60*30)

//-------------------------------------
///	メイン分岐
//=====================================
enum {
	WFP2PMF_MAIN_WIPE_S,
	WFP2PMF_MAIN_WIPE_E,
	WFP2PMF_MAIN_MATCHWAIT,
	WFP2PMF_MAIN_EWIPE_S,
	WFP2PMF_MAIN_EWIPE_E,
} ;


//-------------------------------------
///	親分岐STATUS
//=====================================
enum {
	WFP2PMF_OYA_STATUS_INIT,				// 親の初期化
	WFP2PMF_OYA_STATUS_CONNECT_WAIT_INIT,	// CONNECT待ち初期化
	WFP2PMF_OYA_STATUS_CONNECT_WAIT,		// CONNECT待ち
	WFP2PMF_OYA_STATUS_CONNECT_SELECT_INIT00,// CONNECT待ち
	WFP2PMF_OYA_STATUS_CONNECT_SELECT_INIT01,// CONNECTした人を入れるか選択初期化
	WFP2PMF_OYA_STATUS_CONNECT_SELECT,		// CONNECTした人を入れるか選択
	WFP2PMF_OYA_STATUS_CONNECT_MIN_INIT,	// ゲーム開始したいが、人数が足りない
	WFP2PMF_OYA_STATUS_CONNECT_MIN,			// ゲーム開始したいが、人数が足りない
	WFP2PMF_OYA_STATUS_START_INIT00,			// ゲーム開始処理　開始
	WFP2PMF_OYA_STATUS_START_INIT01,			// ゲーム開始処理　開始
	WFP2PMF_OYA_STATUS_START_SELECT,		// ゲーム開始処理に行くか選択
	WFP2PMF_OYA_STATUS_START,				// ゲーム開始へ
	WFP2PMF_OYA_STATUS_STARTNUMCHECK,		// ゲーム開始　人数チェック
	WFP2PMF_OYA_STATUS_STARTSYNC_INIT,		// ゲーム開始同期
	WFP2PMF_OYA_STATUS_STARTSYNC,			// ゲーム開始同期
	WFP2PMF_OYA_STATUS_STATE_SEND,			// ゲーム開始	ステータス送信
	WFP2PMF_OYA_STATUS_STATE_CHECK,			// ゲーム開始	ステータスチェック
	WFP2PMF_OYA_STATUS_MYSTATUS_WAIT,		// ゲーム開始	データ待ち
	WFP2PMF_OYA_STATUS_GAME_START,			// 開始
	WFP2PMF_OYA_STATUS_END_INIT00,			// 待機状態をやめるか　開始
	WFP2PMF_OYA_STATUS_END_INIT01,			// 待機状態をやめるか　開始
	WFP2PMF_OYA_STATUS_END_SELECT,			// 待機状態をやめるか　選択
	WFP2PMF_OYA_STATUS_END2_INIT00,			// 待機状態をやめると　解散　開始
	WFP2PMF_OYA_STATUS_END2_INIT01,			// 待機状態をやめると　解散　開始
	WFP2PMF_OYA_STATUS_END2_SELECT,			// 待機状態をやめると　解散　選択
	WFP2PMF_OYA_STATUS_END3_INIT,			// 待機状態をやめると　解散　開始
	WFP2PMF_OYA_STATUS_END,					// 待機状態終了
	WFP2PMF_OYA_STATUS_ERR_INIT,			// 通信エラーにより終了 開始
	WFP2PMF_OYA_STATUS_ERR_INIT01,			// 通信エラーにより終了 開始
	WFP2PMF_OYA_STATUS_ERR,					// 通信エラーにより終了
	WFP2PMF_OYA_STATUS_VCHAT_INIT00,		// ボイスチャット変更	開始
	WFP2PMF_OYA_STATUS_VCHAT_INIT01,		// ボイスチャット変更	開始
	WFP2PMF_OYA_STATUS_VCHAT_SELECT,		// ボイスチャット変更	選択
	WFP2PMF_OYA_STATUS_NUM
} ;

//-------------------------------------
///	子分岐状態
//=====================================
enum {
	WFP2PMF_KO_STATUS_ENTRY_OYAWAITINIT,// 親の情報取得待ち
	WFP2PMF_KO_STATUS_ENTRY_OYAWAIT,	// 親の情報取得待ち
	WFP2PMF_KO_STATUS_ENTRY_INIT,		// ENTRY完了待ち開始
	WFP2PMF_KO_STATUS_ENTRY_WAIT,		// ENTRY完了待ち
	WFP2PMF_KO_STATUS_ENTRY_OKINIT,		// ENTRYOK初期化
	WFP2PMF_KO_STATUS_ENTRY_OK,			// ENTRYOK
	WFP2PMF_KO_STATUS_ENTRY_NGINIT,		// ENTRYNG初期化
	WFP2PMF_KO_STATUS_ENTRY_NG,			// ENTRYNG
	WFP2PMF_KO_STATUS_CONNECT_INIT,		// CONNECT完了待ち開始
	WFP2PMF_KO_STATUS_CONNECT_WAIT,		// CONNECT完了待ち
	WFP2PMF_KO_STATUS_START,			// 開始
	WFP2PMF_KO_STATUS_STATE_SEND,			// ゲーム開始	ステータス送信
	WFP2PMF_KO_STATUS_STATE_CHECK,			// ゲーム開始	ステータスチェック
	WFP2PMF_KO_STATUS_MYSTATUS_WAIT,		// ゲーム開始	データ待ち
	WFP2PMF_KO_STATUS_GAME_START,			// 開始
	WFP2PMF_KO_STATUS_ERR_INIT,			// エラー終了	開始
	WFP2PMF_KO_STATUS_ERR_INIT01,		// エラー終了	開始
	WFP2PMF_KO_STATUS_ERR,				// エラー終了
	WFP2PMF_KO_STATUS_VCHAT_INIT00,		// ボイスチャット変更	開始
	WFP2PMF_KO_STATUS_VCHAT_INIT01,		// ボイスチャット変更	開始
	WFP2PMF_KO_STATUS_VCHAT_SELECT,		// ボイスチャット変更	選択
	WFP2PMF_KO_STATUS_CONLOCK_INIT00,		// 通信LOCK中なので再接続
	WFP2PMF_KO_STATUS_CONLOCK_INIT01,		// 通信LOCK中なので再接続
	WFP2PMF_KO_STATUS_CONLOCK_SELECT,		// 通信LOCK中なので再接続
	WFP2PMF_KO_STATUS_CONLOCK_RECON,		// 通信LOCK中　再接続
	WFP2PMF_KO_STATUS_CONLOCK_RECON01,		// 通信LOCK中　再接続０１
	WFP2PMF_KO_STATUS_CONLOCK_RECONWAIT,	// 通信LOCK中　再接続待ち
	WFP2PMF_KO_STATUS_CONLOCK_GRAPHIC_RESET00,	// 通信LOCK中　画面構築
	WFP2PMF_KO_STATUS_CONLOCK_GRAPHIC_RESET01,	// 通信LOCK中　画面構築
	WFP2PMF_KO_STATUS_CONLOCK_DECON,		// 通信LOCK中　切断
	WFP2PMF_KO_STATUS_NUM
} ;

//-------------------------------------
///	描画オブジェクト動作シーケンス
//=====================================
enum {
	WFP2PMF_DRAWOBJ_MOVENONE,
	WFP2PMF_DRAWOBJ_MOVEINIT,
	WFP2PMF_DRAWOBJ_MOVEIN,
	WFP2PMF_DRAWOBJ_MOVENORMAL,
	WFP2PMF_DRAWOBJ_MOVENUM,
} ;
#define WFP2PMF_DRAWOBJ_INTIME	( 16 )	// 入出エフェクトタイム
// 各方向のINTIMEの差
static const u8 WFP2PMF_DRAWOBJ_INTIME_DIV[ WF2DMAP_WAY_NUM ] = {
	8,
	0,
	4,
	12,
};



//-------------------------------------
///	エントリーフラグ
//=====================================
enum {
	WFP2PMF_ENTRY_WAIT,
	WFP2PMF_ENTRY_KO,
	WFP2PMF_ENTRY_NG,
	WFP2PMF_ENTRY_LOCK,
} ;


//-------------------------------------
///	OAM表示シーケンス
//=====================================
enum {
	WFP2PMF_OAM_SEQ_NONE,
	WFP2PMF_OAM_SEQ_INIT,
	WFP2PMF_OAM_SEQ_WAIT,
} ;



//-------------------------------------
///		BGL
//=====================================
#define WFP2PMF_BACK_PAL		(0)	// 背景パレット
#define WFP2PMF_BACK_PALNUM		(18)// アニメの数
#define WFP2PMF_BACK_PALANM	(3)// アニメフレーム
#define WFP2PMF_TALK_PAL	(1)	// 文字パレット
// パレットアニメデータ
static const u8 WFP2PMF_BACK_PAL_IDX[ WFP2PMF_BACK_PALNUM ] = {
	5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 5, 5, 5,
};

// VCHATICON
#define WFP2PMF_VCHAT_ICON_FRM_NUM	( GF_BGL_FRAME2_M )
#define WFP2PMF_VCHAT_ICON_PAL		( 5 )
#define WFP2PMF_VCHAT_ICON_PALNUM	( 1 )
#define WFP2PMF_VCHAT_ICON_CG		( 0 )
#define WFP2PMF_VCHAT_ICON_CGSIZ	( 24*2 )
#define WFP2PMF_VCHAT_ICON_SIZ		(2)
#define WFP2PMF_VCHAT_ICON_WX		(18)
#define WFP2PMF_VCHAT_ICON_WY		(5)
enum{
	WFP2PMF_VCHATICON_NONE,
	WFP2PMF_VCHATICON_OFF,
	WFP2PMF_VCHATICON_ON,
};


// BG1
#define WFP2PMF_BG1_TALKWIN_CGX	( 1 )
#define WFP2PMF_BG1_TALKWIN_PAL	( 2 )
#define WFP2PMF_BG1_MENUWIN_CGX	( WFP2PMF_BG1_TALKWIN_CGX+TALK_WIN_CGX_SIZ )
#define WFP2PMF_BG1_MENUWIN_PAL	( 3 )

// BG2
#define WFP2PMF_BG2_MENUWIN_CGX	( WFP2PMF_VCHAT_ICON_CG+WFP2PMF_VCHAT_ICON_CGSIZ )
#define WFP2PMF_BG2_MENUWIN_PAL	( 4 )

// メッセージウィンドウ
#define WFP2PMF_MSGWIN_X		( 2 )
#define WFP2PMF_MSGWIN_Y		( 19 )
#define WFP2PMF_MSGWIN_SIZX		( 27 )
#define WFP2PMF_MSGWIN_SIZY		( 4 )
#define WFP2PMF_MSGWIN_PAL		( WFP2PMF_TALK_PAL )
#define WFP2PMF_MSGWIN_CGX		( WFP2PMF_BG1_MENUWIN_CGX + MENU_WIN_CGX_SIZ )
#define WFP2PMF_MSGWIN_SIZ		( WFP2PMF_MSGWIN_SIZX*WFP2PMF_MSGWIN_SIZY )

// タイトルウィンドウ
#define WFP2PMF_TITLEWIN_X		( 1 )
#define WFP2PMF_TITLEWIN_Y		( 1 )
#define WFP2PMF_TITLEWIN_SIZX	( 27 )
#define WFP2PMF_TITLEWIN_SIZY	( 2 )
#define WFP2PMF_TITLEWIN_PAL	( WFP2PMF_TALK_PAL )
#define WFP2PMF_TITLEWIN_CGX	( WFP2PMF_MSGWIN_CGX + WFP2PMF_MSGWIN_SIZ )
#define WFP2PMF_TITLEWIN_SIZ	( WFP2PMF_TITLEWIN_SIZX*WFP2PMF_TITLEWIN_SIZY )

// ボイスチャットウィンドウ
#define WFP2PMF_VCHATWIN_X		( 2 )
#define WFP2PMF_VCHATWIN_Y		( 16 )
#define WFP2PMF_VCHATWIN_SIZX	( 28 )
#define WFP2PMF_VCHATWIN_SIZY	( 2 )
#define WFP2PMF_VCHATWIN_PAL	( WFP2PMF_TALK_PAL )
#define WFP2PMF_VCHATWIN_CGX	( WFP2PMF_TITLEWIN_CGX + WFP2PMF_TITLEWIN_SIZ )
#define WFP2PMF_VCHATWIN_SIZ	( WFP2PMF_VCHATWIN_SIZX*WFP2PMF_VCHATWIN_SIZY )

// CONNECTリストウィンドウ
#define WFP2PMF_CONLISTWIN_X		( 1 )
#define WFP2PMF_CONLISTWIN_Y		( 5 )
#define WFP2PMF_CONLISTWIN_SIZX	( 20 )
#define WFP2PMF_CONLISTWIN_SIZY	( 8 )
#define WFP2PMF_CONLISTWIN_PAL	( WFP2PMF_TALK_PAL )
#define WFP2PMF_CONLISTWIN_CGX	( WFP2PMF_VCHATWIN_CGX + WFP2PMF_VCHATWIN_SIZ )
#define WFP2PMF_CONLISTWIN_SIZ	( WFP2PMF_CONLISTWIN_SIZX*WFP2PMF_CONLISTWIN_SIZY )
#define WFP2PMF_CONLISTWIN_ONELIST_Y	( 2 )
#define WFP2PMF_CONLISTWIN_ID_X	( 72 )

// NEWCONNECTリストウィンドウ
#define WFP2PMF_NEWCONWIN_X		( 23 )
#define WFP2PMF_NEWCONWIN_Y		( 5 )
#define WFP2PMF_NEWCONWIN_SIZX	( 7 )
#define WFP2PMF_NEWCONWIN_SIZY	( 5 )
#define WFP2PMF_NEWCONWIN_PAL	( WFP2PMF_TALK_PAL )
#define WFP2PMF_NEWCONWIN_CGX	( WFP2PMF_CONLISTWIN_CGX + WFP2PMF_CONLISTWIN_SIZ )
#define WFP2PMF_NEWCONWIN_SIZ	( WFP2PMF_NEWCONWIN_SIZX*WFP2PMF_NEWCONWIN_SIZY )

// yes no win
#define WFP2PMF_YESNOWIN_FRM_NUM	( GF_BGL_FRAME2_M )
#define WFP2PMF_YESNOWIN_X			( 25 )
#define WFP2PMF_YESNOWIN_Y			( 13 )
#define WFP2PMF_YESNOWIN_SIZX		( 5 )
#define WFP2PMF_YESNOWIN_SIZY		( 4 )
#define WFP2PMF_YESNOWIN_PAL		( WFP2PMF_TALK_PAL )
#define WFP2PMF_YESNOWIN_CGX		( WFP2PMF_BG2_MENUWIN_CGX + MENU_WIN_CGX_SIZ )
#define WFP2PMF_YESNOWIN_SIZ		( WFP2PMF_YESNOWIN_SIZX * WFP2PMF_YESNOWIN_SIZY )

static const BMPWIN_DAT WFP2PMF_YESNOBMPDAT = {
	WFP2PMF_YESNOWIN_FRM_NUM, WFP2PMF_YESNOWIN_X, WFP2PMF_YESNOWIN_Y,
	WFP2PMF_YESNOWIN_SIZX, WFP2PMF_YESNOWIN_SIZY, 
	WFP2PMF_YESNOWIN_PAL, WFP2PMF_YESNOWIN_CGX
};

#define	_COL_N_BLACK	( GF_PRINTCOLOR_MAKE( 1, 2, 15 ) )		// フォントカラー：黒
#define	_COL_N_BLACK_C	( GF_PRINTCOLOR_MAKE( 1, 2, 0 ) )		// フォントカラー：黒


//-------------------------------------
///	メッセージデータ msgdata
//=====================================
#define WFP2PMF_MSGDATA_STRNUM	 ( 256 )
#define WFP2PMF_MSGNO_NONE		(0xff)
#define WFP2PMF_MSGENDWAIT		( 30 )		//  メッセージ表示後待つウエイト



//-------------------------------------
///	タイトルデータ
//=====================================
static u8 WFP2PMF_TITLE[ WFP2PMF_TYPE_NUM ] = {
	msg_wifilobby_131,
	msg_wifilobby_150,
	msg_wifilobby_150,
	msg_wifilobby_150,
};


//-------------------------------------
///	オブジェクト登録データ
//=====================================
static const WF2DMAP_OBJDATA WFP2PMF_OBJData[ WIFI_P2PMATCH_NUM_MAX ] = {
	{// 親
		208, 48, 0, WF2DMAP_OBJST_NONE, 
		WF2DMAP_WAY_DOWN, 0
	},
	{
		232, 72, 1, WF2DMAP_OBJST_NONE, 
		WF2DMAP_WAY_LEFT, 0
	},
	{
		208, 96, 2, WF2DMAP_OBJST_NONE, 
		WF2DMAP_WAY_UP, 0
	},
	{
		184, 72, 3, WF2DMAP_OBJST_NONE, 
		WF2DMAP_WAY_RIGHT, 0
	},
};



//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	データ
//=====================================
typedef struct {
	TEST_MATCH_WORK*	p_match;		// サーバーと送受信する自分と友達の状態
	u8					oya;			// 親かどうか
	u8					status;			// 状態
	u8					new_con;		// 新しいコネクト
	u8					match_result;	// 上に返す結果
	s32					timer;			// タイマー
	s32					timeout;		// タイムアウト
	u8					timeout_flag;	// タイムアウトの有無
	u8					vchat_flag;		// Vchatフラグ
	u8					ko_newcon_flag;	// 子用新規接続があったかチェック
	u8					entry;			// エントリーフラグ
	u8					logout_in[WIFI_P2PMATCH_NUM_MAX];// LOGアウト検知
	u8					logout_id[WIFI_P2PMATCH_NUM_MAX];// LOGアウトした人のID
	u8					vchat_tmp[WIFI_P2PMATCH_NUM_MAX];	// １つ前のVCHAT
	u8					vchat_now[WIFI_P2PMATCH_NUM_MAX];	// １つ前のVCHAT
	u8					conlist_rewrite;// conlistを再描画
	u8					last_netid;		// 新規コネクトロックフラグ
	u8					oya_friendid;	// 親の友達コード
	u8					err_check;		// エラーチェックが必要か
	u8					entry_on[WIFI_P2PMATCH_NUM_MAX];// エントリーフラグ
	u8					oya_start;		// 親からゲーム開始を受信
	u8					ng_msg_idx;		// NGメッセージ
} WFP2PMF_DATA;

//-------------------------------------
///	　描画オブジェクトワーク
//=====================================
typedef struct {
	WF2DMAP_OBJWK*		p_objwk;		// オブジェクトシステム
	WF2DMAP_OBJDRAWWK*	p_objdrawwk;	// オブジェクト描画ワーク
	u16 seq;
	s16 count;
} WFP2PMF_DRAWOBJ;


//-------------------------------------
///	描画データ
//=====================================
typedef struct {
	GF_BGL_INI*			p_bgl;				// GF_BGL_INI
	WORDSET*			p_wordset;			// メッセージ展開用ワークマネージャー
	MSGDATA_MANAGER*	p_msgman;			// メッセージデータマネージャー
	STRBUF*				p_msgstr;			// メッセージ用文字列バッファ
	STRBUF*				p_msgtmp;			// メッセージ用文字列バッファ
	u8					msg_no;				// メッセージ完了検査ナンバー
	u8					msg_wait;			// メッセージウエイト
	u16					msg_speed;			// メッセージスピード

	CLACT_SET_PTR 			clactSet;						// セルアクターセット
	CLACT_U_EASYRENDER_DATA	renddata;						// 簡易レンダーデータ
	CLACT_U_RES_MANAGER_PTR	resMan[WIFI_P2PMATCH_RESNUM];	// キャラ・パレットリソースマネージャ

	WF2DMAP_OBJSYS*		p_objsys;			// オブジェクトシステム
	WF2DMAP_OBJDRAWSYS* p_objdraw;			// オブジェクト描画システム
	WFP2PMF_DRAWOBJ		objdata[WIFI_P2PMATCH_CLACTWK_NUM];	// オブジェクトデータ


	GF_BGL_BMPWIN		msgwin;				// 会話ウインドウ
	GF_BGL_BMPWIN		titlewin;			// 自分の状態表示
	GF_BGL_BMPWIN		vchatwin;			// ボイスチャットウィンドウ
	GF_BGL_BMPWIN		conlistwin;			// コネクトリストウィンドウ
	GF_BGL_BMPWIN		newconwin;			// 新規コネクトウィンドウ
	BMPMENU_WORK*		p_yesnowin;			// yes noウィンドウ

	void* p_vchatscrnbuf;
	NNSG2dScreenData*  p_vchatscrn;			// ボイスチャットアイコン

	void* p_timewaiticon;

	// 背景アニメ
	void*				p_plttbuff;
	NNSG2dPaletteData*	p_pltt;
	s16					pltt_count;
	s16					pltt_idx;
} WFP2PMF_DRAW;


//-------------------------------------
///	４人マッチング画面ワーク
//=====================================
typedef struct _WFP2P_WK{
	WFP2PMF_DATA	data;
	WFP2PMF_DRAW	draw;
} WFP2PMF_WK;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
static void VBlankFunc( void * work );

static BOOL WFP2PMF_MyDataOyaCheck( const WFP2PMF_DATA* cp_data );
static void WFP2PMF_GraphicInit( WFP2PMF_WK* p_wk, const WFP2PMF_INIT* cp_init, u32 heapID );
static void WFP2PMF_GraphicDelete( WFP2PMF_WK* p_wk, u32 heapID );
static void WFP2PMF_GraphicBankSet( void );
static void WFP2PMF_GraphicBGLInit( WFP2PMF_DRAW* p_draw, u32 heapID );
static void WFP2PMF_GraphicBGLDelete( WFP2PMF_DRAW* p_draw );
static void WFP2PMF_GraphicBmpInit( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, u32 heapID );
static void WFP2PMF_GraphicBmpDelete( WFP2PMF_DRAW* p_draw, u32 heapID );
static void WFP2PMF_GraphicMsgInit( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, u32 heapID );
static void WFP2PMF_GraphicMsgDelete( WFP2PMF_DRAW* p_draw );
static void WFP2PMF_GraphicBmpMsgInit( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, u32 heapID );
static void WFP2PMF_GraphicClactInit( WFP2PMF_DRAW* p_draw, u32 heapID );
static void WFP2PMF_GraphicClactDelete( WFP2PMF_DRAW* p_draw );

static void WFP2PMF_GraphicMsgBmpStrPut( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, u32 msg_idx, u32 heapID );
static void WFP2PMF_GraphicMsgBmpStrPutEx( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, u32 msg_idx, u8 wait, u32 heapID );
static void WFP2PMF_GraphicMsgBmpStrPutAll( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, u32 msg_idx, u32 heapID );
static BOOL WFP2PMF_GraphicMsgBmpStrMain( WFP2PMF_DRAW* p_draw );
static void WFP2PMF_GraphicMsgBmpStrOff( WFP2PMF_DRAW* p_draw );
static void WFP2PMF_GraphicNewConStrPut( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, int netid, u32 heapID );
static void WFP2PMF_GraphicNewConStrOff( WFP2PMF_DRAW* p_draw );
static void WFP2PMF_GraphicConlistStrPutAll( WFP2PMF_WK* p_wk, const WFP2PMF_INIT* cp_init, u32 heapID );
static void WFP2PMF_GraphicConlistStrPut( WFP2PMF_WK* p_wk, WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, int netid, u32 heapID );
static void WFP2PMF_GraphicConlistStrOff( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, int netid, u32 heapID );

static void WFP2PMF_WordSetPlayerNameSet( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, int netid, int set_no, u32 heapID );
static void WFP2PMF_WordSetPlayerIDSet( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, int netid, int set_no, u32 heapID );

static void WFP2PMF_PlttAnm( WFP2PMF_DRAW* p_draw );
static void WFP2PMF_PlttAnm_Trans( WFP2PMF_DRAW* p_draw, u32 idx );

static BOOL WFP2PMF_VchatSwitch( WFP2PMF_WK* p_wk, u32 heapID );


static void WFP2PMF_ErrCheck( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_param, u32 heapID );
static void WFP2PMF_ReConErrCheck( WFP2PMF_WK* p_wk );
static BOOL WFP2PMF_MatchCheck( const WFP2PMF_WK* cp_wk );
static BOOL WFP2PMF_CommWiFiMatchStart( u32 friendno, u32 type );
static BOOL WFP2PMF_CheckP2PMatchFriendStatus( const WFP2PMF_WK* cp_wk, const WFP2PMF_INIT* cp_param, int friend );

static void WFP2PMF_StatusChange( WFP2PMF_WK* p_wk, const WFP2PMF_INIT* cp_init );

static BOOL WFP2PMF_StatusVChatOn( WFP2PMF_WK* p_wk );
static BOOL WFP2PMF_StatusVChatOff( WFP2PMF_WK* p_wk );
static void WFP2PMF_StatusVChatOnBmpFade( WFP2PMF_WK* p_wk );
static void WFP2PMF_StatusVChatOffBmpFade( WFP2PMF_WK* p_wk );

static int WFP2PMF_GetEntryNum( WFP2PMF_WK* p_wk );
static BOOL WFP2PMF_SetEntryOya( WFP2PMF_WK* p_wk, u32 heapID );

static void WFP2PMF_CommSendNewEntryNg( WFP2PMF_WK* p_wk );
static void WFP2PMF_CommSendAllEntryNg( WFP2PMF_WK* p_wk );

static void WFP2PMF_LogOutCheckStart( WFP2PMF_WK* p_wk );
static void WFP2PMF_LogOutCheckEnd( WFP2PMF_WK* p_wk );
static BOOL WFP2PMF_LogOutCheck( WFP2PMF_WK* p_wk );
static void WFP2PMF_LogOutCallBack( u16 aid, void* pWork );

static BOOL WFP2PMF_ConListWriteCheck( WFP2PMF_WK* p_wk );
static void WFP2PMF_ConListWriteReq( WFP2PMF_WK* p_wk );

static void WFP2PMF_CommInfoSetEntry( WFP2PMF_WK* p_wk, u32 aid, u32 heapID );

static void WFP2PMF_DrawOamInit( WFP2PMF_WK* p_wk, u32 aid, u32 heapID );
static void WFP2PMF_DrawOamMain( WFP2PMF_WK* p_wk );
static void WFP2PMF_DrawOamDelete( WFP2PMF_WK* p_wk );

static BOOL WFP2PMF_NewConLockCheck( const WFP2PMF_WK* cp_wk );

static void WFP2PMF_NewConLockSend( WFP2PMF_WK* p_wk, u32 aid );
static void WFP2PMF_NewConLockNewCon( WFP2PMF_WK* p_wk );

static void WFP2PMF_KoRecvEntry( WFP2PMF_WK* p_wk, u32 heapID );

static void WFP2PMF_TimeWaitIconOn( WFP2PMF_WK* p_wk );
static void WFP2PMF_TimeWaitIconOff( WFP2PMF_WK* p_wk, const WFP2PMF_INIT* cp_init, u32 heapID );

static void WFP2PMF_DrawObjMoveNone( WFP2PMF_WK* p_wk, WFP2PMF_DRAWOBJ* p_obj );
static void WFP2PMF_DrawObjMoveInit( WFP2PMF_WK* p_wk, WFP2PMF_DRAWOBJ* p_obj );
static void WFP2PMF_DrawObjMoveIn( WFP2PMF_WK* p_wk, WFP2PMF_DRAWOBJ* p_obj );
static void WFP2PMF_DrawObjMoveNormal( WFP2PMF_WK* p_wk, WFP2PMF_DRAWOBJ* p_obj );

static void WFP2PMF_VchatCommSend( WFP2PMF_WK* p_wk );

static BOOL WFP2PMF_MatchOkCheck( WFP2PMF_WK* p_wk );

static void WFP2PMF_FNOTE_Set( WFP2PMF_INIT* p_param, u32 heapID );

static BOOL WFP2PMF_Oya_CheckConnectPlayer( const WFP2PMF_WK* cp_wk, const WFP2PMF_INIT* cp_param );


static void WFP2PMF_TimeOut_Start( WFP2PMF_WK* p_wk );
static void WFP2PMF_TimeOut_Stop( WFP2PMF_WK* p_wk );
static BOOL WFP2PMF_TimeOut_Main( WFP2PMF_WK* p_wk );

//-------------------------------------
/// 親分岐データ
//=====================================
static BOOL WFP2PMF_OyaInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaConnectWaitInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaConnectWait( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaConnectSelectInit00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaConnectSelectInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaConnectSelect( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaConnectMinInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );			
static BOOL WFP2PMF_OyaConnectMin( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );			
static BOOL WFP2PMF_OyaStartInit00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );			
static BOOL WFP2PMF_OyaStartInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );			
static BOOL WFP2PMF_OyaStartSelect( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );		
static BOOL WFP2PMF_OyaStart( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );				
static BOOL WFP2PMF_OyaStartNumCheck( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaStartSyncInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaStartSync( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaStateSend( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaStateCheck( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaMyStatusWait( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaGameStart( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_OyaEndInit00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );			
static BOOL WFP2PMF_OyaEndInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );			
static BOOL WFP2PMF_OyaEndSelect( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );			
static BOOL WFP2PMF_OyaEnd2Init00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );			
static BOOL WFP2PMF_OyaEnd2Init01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );			
static BOOL WFP2PMF_OyaEnd2Select( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );			
static BOOL WFP2PMF_OyaEnd3Init( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );			
static BOOL WFP2PMF_OyaEnd( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );					
static BOOL WFP2PMF_OyaErrInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );			
static BOOL WFP2PMF_OyaErrInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );			
static BOOL WFP2PMF_OyaErr( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );					
static BOOL WFP2PMF_OyaVchatInit00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );			
static BOOL WFP2PMF_OyaVchatInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );			
static BOOL WFP2PMF_OyaVchatSelect( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );		




//-------------------------------------
/// 子分岐データ
//=====================================
static BOOL WFP2PMF_KoEntryOyaWaitInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoEntryOyaWait( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoEntryInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoEntryWait( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoEntryOkInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoEntryOk( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoEntryNgInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoEntryNg( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoConnectInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoConnectWait( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoStart( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );	   
static BOOL WFP2PMF_KoStateSend( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoStateCheck( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoMyStatusWait( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoGameStart( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoErrInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );	   
static BOOL WFP2PMF_KoErrInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );	   
static BOOL WFP2PMF_KoErr( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );		   
static BOOL WFP2PMF_KoVchatInit00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );  
static BOOL WFP2PMF_KoVchatInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );  
static BOOL WFP2PMF_KoVchatSelect( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoConLockInit00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );  
static BOOL WFP2PMF_KoConLockInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );  
static BOOL WFP2PMF_KoConLockSelect( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoConLockReCon( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoConLockReCon01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoConLockReConWait( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoConLockReConGraphicReset00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoConLockReConGraphicReset01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );
static BOOL WFP2PMF_KoConLockDeCon( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID );


//-------------------------------------
///	親関数テーブル
//=====================================
static BOOL (* const pOyaFunc[ WFP2PMF_OYA_STATUS_NUM ])( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID ) = {
	WFP2PMF_OyaInit,								// WFP2PMF_OYA_STATUS_INIT,				// 親の初期化
	WFP2PMF_OyaConnectWaitInit,                     // WFP2PMF_OYA_STATUS_CONNECT_WAIT_INIT,	// CONNECT待ち初期化
	WFP2PMF_OyaConnectWait,                         // WFP2PMF_OYA_STATUS_CONNECT_WAIT,		// CONNECT待ち
	WFP2PMF_OyaConnectSelectInit00,                 // WFP2PMF_OYA_STATUS_CONNECT_SELECT_INIT00,// CONNECT待ち
	WFP2PMF_OyaConnectSelectInit01,                 // WFP2PMF_OYA_STATUS_CONNECT_SELECT_INIT01,// CONNECTした人を入れるか選択初期化
	WFP2PMF_OyaConnectSelect,                       // WFP2PMF_OYA_STATUS_CONNECT_SELECT,		// CONNECTした人を入れるか選択
	WFP2PMF_OyaConnectMinInit,                      // WFP2PMF_OYA_STATUS_CONNECT_MIN_INIT,	// ゲーム開始したいが、人数が足りない
	WFP2PMF_OyaConnectMin,                          // WFP2PMF_OYA_STATUS_CONNECT_MIN,			// ゲーム開始したいが、人数が足りない
	WFP2PMF_OyaStartInit00,                         // WFP2PMF_OYA_STATUS_START_INIT00,			// ゲーム開始処理　開始
	WFP2PMF_OyaStartInit01,                         // WFP2PMF_OYA_STATUS_START_INIT01,			// ゲーム開始処理　開始
	WFP2PMF_OyaStartSelect,                         // WFP2PMF_OYA_STATUS_START_SELECT,		// ゲーム開始処理に行くか選択
	WFP2PMF_OyaStart,                               // WFP2PMF_OYA_STATUS_START,				// ゲーム開始へ
	WFP2PMF_OyaStartNumCheck,                       // WFP2PMF_OYA_STATUS_STARTNUMCHECK,		// ゲーム開始　人数チェック
	WFP2PMF_OyaStartSyncInit,                       // WFP2PMF_OYA_STATUS_STARTSYNC_INIT,		// ゲーム開始同期
	WFP2PMF_OyaStartSync,                           // WFP2PMF_OYA_STATUS_STARTSYNC,			// ゲーム開始同期
	WFP2PMF_OyaStateSend,                           // WFP2PMF_OYA_STATUS_STATE_SEND,			// ゲーム開始	ステータス送信
	WFP2PMF_OyaStateCheck,                          // WFP2PMF_OYA_STATUS_STATE_CHECK,			// ゲーム開始	ステータスチェック
	WFP2PMF_OyaMyStatusWait,                        // WFP2PMF_OYA_STATUS_MYSTATUS_WAIT,		// ゲーム開始	データ待ち
	WFP2PMF_OyaGameStart,	                        // WFP2PMF_OYA_STATUS_GAME_START,			// 開始
	WFP2PMF_OyaEndInit00,                           // WFP2PMF_OYA_STATUS_END_INIT00,			// 待機状態をやめるか　開始
	WFP2PMF_OyaEndInit01,                           // WFP2PMF_OYA_STATUS_END_INIT01,			// 待機状態をやめるか　開始
	WFP2PMF_OyaEndSelect,                           // WFP2PMF_OYA_STATUS_END_SELECT,			// 待機状態をやめるか　選択
	WFP2PMF_OyaEnd2Init00,                          // WFP2PMF_OYA_STATUS_END2_INIT00,			// 待機状態をやめると　解散　開始
	WFP2PMF_OyaEnd2Init01,                          // WFP2PMF_OYA_STATUS_END2_INIT01,			// 待機状態をやめると　解散　開始
	WFP2PMF_OyaEnd2Select,                          // WFP2PMF_OYA_STATUS_END2_SELECT,			// 待機状態をやめると　解散　選択
	WFP2PMF_OyaEnd3Init,                            // WFP2PMF_OYA_STATUS_END3_INIT,			// 待機状態をやめると　解散　開始
	WFP2PMF_OyaEnd,                                 // WFP2PMF_OYA_STATUS_END,					// 待機状態終了
	WFP2PMF_OyaErrInit,                             // WFP2PMF_OYA_STATUS_ERR_INIT,			// 通信エラーにより終了 開始
	WFP2PMF_OyaErrInit01,                           // WFP2PMF_OYA_STATUS_ERR_INIT01,			// 通信エラーにより終了 開始
	WFP2PMF_OyaErr,                                 // WFP2PMF_OYA_STATUS_ERR,					// 通信エラーにより終了
	WFP2PMF_OyaVchatInit00,                         // WFP2PMF_OYA_STATUS_VCHAT_INIT00,		// ボイスチャット変更	開始
	WFP2PMF_OyaVchatInit01,                         // WFP2PMF_OYA_STATUS_VCHAT_INIT01,		// ボイスチャット変更	開始
	WFP2PMF_OyaVchatSelect,                         // WFP2PMF_OYA_STATUS_VCHAT_SELECT,		// ボイスチャット変更	選択
};

//-------------------------------------
///	子関数テーブル
//=====================================
static BOOL (* const pKoFunc[ WFP2PMF_KO_STATUS_NUM ])( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID ) = {
	WFP2PMF_KoEntryOyaWaitInit,					// WFP2PMF_KO_STATUS_ENTRY_OYAWAITINIT,// 親の情報取得待ち
	WFP2PMF_KoEntryOyaWait,                     // WFP2PMF_KO_STATUS_ENTRY_OYAWAIT,	// 親の情報取得待ち
	WFP2PMF_KoEntryInit,                        // WFP2PMF_KO_STATUS_ENTRY_INIT,		// ENTRY完了待ち開始
	WFP2PMF_KoEntryWait,                        // WFP2PMF_KO_STATUS_ENTRY_WAIT,		// ENTRY完了待ち
	WFP2PMF_KoEntryOkInit,                      // WFP2PMF_KO_STATUS_ENTRY_OKINIT,		// ENTRYOK初期化
	WFP2PMF_KoEntryOk,                          // WFP2PMF_KO_STATUS_ENTRY_OK,			// ENTRYOK
	WFP2PMF_KoEntryNgInit,                      // WFP2PMF_KO_STATUS_ENTRY_NGINIT,		// ENTRYNG初期化
	WFP2PMF_KoEntryNg,                          // WFP2PMF_KO_STATUS_ENTRY_NG,			// ENTRYNG
	WFP2PMF_KoConnectInit,                      // WFP2PMF_KO_STATUS_CONNECT_INIT,		// CONNECT完了待ち開始
	WFP2PMF_KoConnectWait,                      // WFP2PMF_KO_STATUS_CONNECT_WAIT,		// CONNECT完了待ち
	WFP2PMF_KoStart,                            // WFP2PMF_KO_STATUS_START,			// 開始
	WFP2PMF_KoStateSend,                        // WFP2PMF_KO_STATUS_STATE_SEND,			// ゲーム開始	ステータス送信
	WFP2PMF_KoStateCheck,                       // WFP2PMF_KO_STATUS_STATE_CHECK,			// ゲーム開始	ステータスチェック
	WFP2PMF_KoMyStatusWait,                     // WFP2PMF_KO_STATUS_MYSTATUS_WAIT,		// ゲーム開始	データ待ち
	WFP2PMF_KoGameStart,	                    // WFP2PMF_KO_STATUS_GAME_START,			// 開始
	WFP2PMF_KoErrInit,                          // WFP2PMF_KO_STATUS_ERR_INIT,			// エラー終了	開始
	WFP2PMF_KoErrInit01,                        // WFP2PMF_KO_STATUS_ERR_INIT01,		// エラー終了	開始
	WFP2PMF_KoErr,                              // WFP2PMF_KO_STATUS_ERR,				// エラー終了
	WFP2PMF_KoVchatInit00,                      // WFP2PMF_KO_STATUS_VCHAT_INIT00,		// ボイスチャット変更	開始
	WFP2PMF_KoVchatInit01,                      // WFP2PMF_KO_STATUS_VCHAT_INIT01,		// ボイスチャット変更	開始
	WFP2PMF_KoVchatSelect,                      // WFP2PMF_KO_STATUS_VCHAT_SELECT,		// ボイスチャット変更	選択
	WFP2PMF_KoConLockInit00,                    // WFP2PMF_KO_STATUS_CONLOCK_INIT00,		// 通信LOCK中なので再接続
	WFP2PMF_KoConLockInit01,                    // WFP2PMF_KO_STATUS_CONLOCK_INIT01,		// 通信LOCK中なので再接続
	WFP2PMF_KoConLockSelect,                    // WFP2PMF_KO_STATUS_CONLOCK_SELECT,		// 通信LOCK中なので再接続
	WFP2PMF_KoConLockReCon,                     // WFP2PMF_KO_STATUS_CONLOCK_RECON,		// 通信LOCK中　再接続
	WFP2PMF_KoConLockReCon01,                   // WFP2PMF_KO_STATUS_CONLOCK_RECON01,		// 通信LOCK中　再接続０１
	WFP2PMF_KoConLockReConWait,                 // WFP2PMF_KO_STATUS_CONLOCK_RECONWAIT,	// 通信LOCK中　再接続待ち
	WFP2PMF_KoConLockReConGraphicReset00,       // WFP2PMF_KO_STATUS_CONLOCK_GRAPHIC_RESET00,	// 通信LOCK中　画面構築
	WFP2PMF_KoConLockReConGraphicReset01,       // WFP2PMF_KO_STATUS_CONLOCK_GRAPHIC_RESET01,	// 通信LOCK中　画面構築
	WFP2PMF_KoConLockDeCon,                     // WFP2PMF_KO_STATUS_CONLOCK_DECON,		// 通信LOCK中　切断
};


// 描画オブジェクト動作
static void (* const pDrawObjMoveFunc[ WFP2PMF_DRAWOBJ_MOVENUM ])( WFP2PMF_WK* p_wk, WFP2PMF_DRAWOBJ* p_obj ) = {
	WFP2PMF_DrawObjMoveNone,
	WFP2PMF_DrawObjMoveInit,
	WFP2PMF_DrawObjMoveIn,
	WFP2PMF_DrawObjMoveNormal,
};


//----------------------------------------------------------------------------
/**
 * プロセス関数：初期化
 *
 * @param	proc	プロセスデータ
 * @param	seq		シーケンス
 *
 * @return	処理状況
 */
//-----------------------------------------------------------------------------
PROC_RESULT WifiP2PMatchFourProc_Init( PROC * proc, int * seq )
{
	WFP2PMF_WK* p_wk;
	WFP2PMF_INIT* p_init = PROC_GetParentWork(proc);
	BOOL result;

	// wifi_2dmapオーバーレイ読込み
	Overlay_Load( FS_OVERLAY_ID(wifi_2dmapsys), OVERLAY_LOAD_NOT_SYNCHRONIZE);

	// 通信中かチェック
	result = CommStateIsWifiConnect();
	GF_ASSERT( result == TRUE );

	// ヒープ作成
    sys_CreateHeap( HEAPID_BASE_APP, HEAPID_WIFI_FOURMATCH, 0x18000 );
    sys_CreateHeap( HEAPID_BASE_APP, HEAPID_WIFI_FOURMATCHVCHAT, 0xa000 );

	// プロセスワーク作成
	p_wk = PROC_AllocWork( proc, sizeof(WFP2PMF_WK), HEAPID_WIFI_FOURMATCH );
	MI_CpuFill8( p_wk, 0, sizeof(WFP2PMF_WK) );


	// VCHATワークの初期化
	MI_CpuFill8( p_wk->data.vchat_tmp, TRUE, sizeof(u8)*WIFI_P2PMATCH_NUM_MAX );// １つ前のVCHAT
	MI_CpuFill8( p_wk->data.vchat_now, TRUE, sizeof(u8)*WIFI_P2PMATCH_NUM_MAX );// １つ前のVCHAT



	// VRAM転送マネージャ初期化
	initVramTransferManagerHeap( WIFI_P2PMATCH_VRAMTRMAN_NUM, HEAPID_WIFI_FOURMATCH );


	// 通信データ取得
	p_wk->data.p_match = CommStateGetMatchWork();
	p_wk->data.p_match->myMatchStatus.vchat = p_wk->data.p_match->myMatchStatus.vchat_org;

	// 自分が親かチェック
	p_wk->data.oya = WFP2PMF_MyDataOyaCheck( &p_wk->data );

	// ワーク初期化
	p_wk->data.new_con = INVALID_NETID;

	// 通信コマンド設定
	CommCommandWFP2PMFInitialize( p_wk );

	// 切断チェック開始
	WFP2PMF_LogOutCheckStart( p_wk );


	// 描画データ初期化
	WFP2PMF_GraphicInit( p_wk, p_init, HEAPID_WIFI_FOURMATCH );

    // VBlank関数セット
    sys_VBlankFuncChange( VBlankFunc, p_wk );

	WirelessIconEasy();  // 接続中なのでアイコン表示

	// エラーチェック　開始
	p_wk->data.err_check = TRUE;


	return PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 * プロセス関数：メインループ
 *
 * @param	proc	プロセスデータ
 * @param	seq		シーケンス
 *
 * @return	処理状況
 */
//-----------------------------------------------------------------------------
PROC_RESULT WifiP2PMatchFourProc_Main( PROC * proc, int * seq )
{
	WFP2PMF_WK* p_wk = PROC_GetWork( proc );
	WFP2PMF_INIT* p_init = PROC_GetParentWork(proc);
	BOOL result;

	// メイン処理
	switch( *seq ){
	case WFP2PMF_MAIN_WIPE_S:
        // ワイプフェード開始
        WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK,
                        COMM_BRIGHTNESS_SYNC, 1, HEAPID_WIFI_FOURMATCH);
		(*seq)++;
		break;

	case WFP2PMF_MAIN_WIPE_E:
        if( WIPE_SYS_EndCheck() ){
            (*seq)++;
        }
		break;

	case WFP2PMF_MAIN_MATCHWAIT:

		// エラーチェック
		if( p_wk->data.err_check ){
			
			WFP2PMF_ErrCheck( p_wk, p_init, HEAPID_WIFI_FOURMATCH );
		}

		if( p_wk->data.oya ){

			// CommInfoがあるかをチェック
			if( CommInfoIsInitialize() == TRUE ){
				
				// みんなのVCHATフラグをチェックして送信
				WFP2PMF_VchatCommSend( p_wk );	

				// 新規POKEDATAを受信したらみんなに送信
				CommInfoSendArray_ServerSide();	// みんなからもらったPOKEDATAを送信

				// last_netIDがENTRYになる前にそれより大きいNETIDの人がNEWENTRYに
				// なったら再接続してもらう
				WFP2PMF_NewConLockNewCon( p_wk );
			}

			
			// 親の動作
			result = pOyaFunc[ p_wk->data.status ]( p_wk, p_init, HEAPID_WIFI_FOURMATCH );
		}else{


			result = pKoFunc[ p_wk->data.status ]( p_wk, p_init, HEAPID_WIFI_FOURMATCH );

			// CommInfoがあるかをチェック
			if( CommInfoIsInitialize() == TRUE ){
				//  自分以外の人のエントリー処理
				WFP2PMF_KoRecvEntry( p_wk, HEAPID_WIFI_FOURMATCH );
			}
		}

		// リストの描画チェック
		if( WFP2PMF_ConListWriteCheck( p_wk ) ){
			// リストの描画
			WFP2PMF_GraphicConlistStrPutAll( p_wk, p_init, HEAPID_WIFI_FOURMATCH );
		}

		// OAMメイン処理
		WFP2PMF_DrawOamMain( p_wk );

		// 終了
		if( result == TRUE ){

			// マッチング結果格納
			p_init->result = p_wk->data.match_result;
			p_init->vchat = p_wk->data.vchat_flag;

			(*seq)++;
		}
		break;

	case WFP2PMF_MAIN_EWIPE_S:
		// ワイプフェード開始
		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK,
						COMM_BRIGHTNESS_SYNC, 1, HEAPID_WIFI_FOURMATCH);
		(*seq)++;
		break;
	case WFP2PMF_MAIN_EWIPE_E:
        if( WIPE_SYS_EndCheck() ){
			return PROC_RES_FINISH;
        }
		break;
	}

	// パレットアニメ
	WFP2PMF_PlttAnm( &p_wk->draw );


	// OAM描画処理
	CLACT_Draw( p_wk->draw.clactSet );
	
    return PROC_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 * プロセス関数：終了処理
 *
 * @param	proc	プロセスデータ
 * @param	seq		シーケンス
 *
 * @return	処理状況
 */
//-----------------------------------------------------------------------------
PROC_RESULT WifiP2PMatchFourProc_End( PROC * proc, int * seq )
{
	WFP2PMF_WK* p_wk = PROC_GetWork( proc );
	WFP2PMF_INIT* p_init = PROC_GetParentWork(proc);


    sys_VBlankFuncChange( NULL, NULL );	// VBlankReset

	// LOGOUTチェック終了
	WFP2PMF_LogOutCheckEnd( p_wk );

	// OAM破棄処理
	WFP2PMF_DrawOamDelete( p_wk );

	// VCHATをとめる
	WFP2PMF_StatusVChatOff( p_wk );

	// 描画データ破棄
	WFP2PMF_GraphicDelete( p_wk, HEAPID_WIFI_FOURMATCH );

	// VRAMTRMAN
	DellVramTransferManager();
	
	// プロセスワーク破棄
    PROC_FreeWork( proc );				// PROCワーク開放

    sys_DeleteHeap( HEAPID_WIFI_FOURMATCHVCHAT );
    sys_DeleteHeap( HEAPID_WIFI_FOURMATCH );

	// オーバーレイ破棄
	Overlay_UnloadID( FS_OVERLAY_ID(wifi_2dmapsys) );

    return PROC_RES_FINISH;
}



// 通信データ受信
//----------------------------------------------------------------------------
/**	
 *	@brief	通信データ受信
 *
 *	@param	p_wk		ワーク
 *	@param	cp_data		データ
 */
//-----------------------------------------------------------------------------
void WFP2PMF_CommResultRecv( WFP2P_WK* p_wk, const WFP2PMF_COMM_RESULT* cp_data )
{
	TOMOYA_PRINT( "recv result [%d]\n", cp_data->flag );
			
	if( p_wk->data.oya == FALSE ){
		if( cp_data->netID == CommGetCurrentID() ){

			
			switch( cp_data->flag ){
			case WFP2PMF_CON_OK:
				p_wk->data.entry = WFP2PMF_ENTRY_KO;
				break;
			
			case WFP2PMF_CON_NG:
				p_wk->data.entry = WFP2PMF_ENTRY_NG;
				break;
				
			case WFP2PMF_CON_LOCK:
				p_wk->data.entry = WFP2PMF_ENTRY_LOCK;
				break;
			}
			
		}else{

			switch( cp_data->flag ){
			case WFP2PMF_CON_OK:
				p_wk->data.entry_on[ cp_data->netID ] = TRUE;
				break;
			
			case WFP2PMF_CON_NG:
			case WFP2PMF_CON_LOCK:
				if( CommInfoIsInitialize() == TRUE ){
					CommInfoDeletePlayer( cp_data->netID );
				}
				break;
			}
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ゲーム開始　同期開始
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void WFP2PMF_CommStartRecv( WFP2P_WK* p_wk )
{
	// 1回しかおくらないように
	if( p_wk->data.oya_start == FALSE ){
		p_wk->data.oya_start = TRUE;
		// 同期開始
		CommTimingSyncStart(_TIMING_GAME_CHECK);
	}
	OS_TPrintf( "sync start \n" );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ゲームVCHATデータ　取得
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void WFP2PMF_CommVchatRecv( WFP2P_WK* p_wk, const WFP2PMF_COMM_VCHAT* cp_data )
{
	if( p_wk->data.oya != TRUE ){	// 親じゃないときだけ
		memcpy( p_wk->data.vchat_now, cp_data->vchat_now, sizeof(u8)*WIFI_P2PMATCH_NUM_MAX );
	}
}



//-----------------------------------------------------------------------------
/**
 *		プライベート関数
 */
//-----------------------------------------------------------------------------
// 　VBLANK
static void VBlankFunc( void * work )
{
	WFP2PMF_WK* p_wk = work;

    // BG書き換え
	GF_BGL_VBlankFunc( p_wk->draw.p_bgl );

    // Vram転送マネージャー実行
    DoVramTransferManager();

    // レンダラ共有OAMマネージャVram転送
    REND_OAMTrans();
}

//----------------------------------------------------------------------------
/**
 *	@brief	自分のデータが親か子かチェック
 *
 *	@param	cp_data			ワーク
 *
 *	@retval	TRUE	親
 *	@retval	FALSE	子
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_MyDataOyaCheck( const WFP2PMF_DATA* cp_data )
{
	if( (CommGetCurrentID() == COMM_PARENT_ID) ){
		// 待機なら親
		return TRUE;
	}
	// それ以外は子
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	グラフィックデータ初期化
 *
 *	@param	p_wk		ワーク	
 *	@param	cp_init		初期化データ
 *	@param	heapID		ヒープ
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicInit( WFP2PMF_WK* p_wk, const WFP2PMF_INIT* cp_init, u32 heapID )
{
	// バンク設定
	WFP2PMF_GraphicBankSet();
	
	// BGL作成
	WFP2PMF_GraphicBGLInit( &p_wk->draw, heapID );
	
	// ビットマップデータ作成
	WFP2PMF_GraphicBmpInit( &p_wk->draw, cp_init, heapID );

	// メッセージデータ初期化
	WFP2PMF_GraphicMsgInit( &p_wk->draw, cp_init, heapID );

	// ビットマップにメッセージを書き込む
	WFP2PMF_GraphicBmpMsgInit( &p_wk->draw, cp_init, heapID );

	// リスト
	// 自分の名前とIDを設定する
	WFP2PMF_GraphicConlistStrPutAll( p_wk, cp_init, heapID );

	// セルアクター
	WFP2PMF_GraphicClactInit( &p_wk->draw, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	グラフィックデータ破棄
 *
 *	@param	p_wk	ワーク
 *	@param	heapID	ヒープID
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicDelete( WFP2PMF_WK* p_wk, u32 heapID )
{
	// セルアクター
	WFP2PMF_GraphicClactDelete( &p_wk->draw );
	
	// メッセージデータ破棄
	WFP2PMF_GraphicMsgDelete( &p_wk->draw );
	
	// ビットマップデータ破棄
	WFP2PMF_GraphicBmpDelete( &p_wk->draw, heapID );
	
	// BGL破棄
	WFP2PMF_GraphicBGLDelete( &p_wk->draw );
}

//----------------------------------------------------------------------------
/**
 *	@brief	バンク設定
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicBankSet( void )
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

//----------------------------------------------------------------------------
/**
 *	@brief	BGL初期化
 *		
 *	@param	p_draw		ワーク
 *	@param	heapID		ヒープ
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicBGLInit( WFP2PMF_DRAW* p_draw, u32 heapID )
{
	ARCHANDLE* p_handle;

	p_draw->p_bgl = GF_BGL_BglIniAlloc( heapID );

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
            0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
            GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
            2, 0, 0, FALSE
            };
        GF_BGL_BGControlSet( p_draw->p_bgl, GF_BGL_FRAME0_M, &TextBgCntDat, GF_BGL_MODE_TEXT );
        GF_BGL_ClearCharSet( GF_BGL_FRAME0_M, 32, 0, heapID);
        GF_BGL_ScrClear( p_draw->p_bgl, GF_BGL_FRAME0_M );


    }

    // メイン画面1
    {
        GF_BGL_BGCNT_HEADER TextBgCntDat = {
            0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
            GX_BG_SCRBASE_0xd000, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
            1, 0, 0, FALSE
            };
        GF_BGL_BGControlSet( p_draw->p_bgl, GF_BGL_FRAME1_M, &TextBgCntDat, GF_BGL_MODE_TEXT );
        GF_BGL_ClearCharSet( GF_BGL_FRAME1_M, 32, 0, heapID);
        GF_BGL_ScrClear( p_draw->p_bgl, GF_BGL_FRAME1_M );
    }

    // YesNoリスト
    {
        GF_BGL_BGCNT_HEADER TextBgCntDat = {
            0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
            GX_BG_SCRBASE_0xd800, GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_23,
            0, 0, 0, FALSE
        };
        GF_BGL_BGControlSet( p_draw->p_bgl, GF_BGL_FRAME2_M, &TextBgCntDat, GF_BGL_MODE_TEXT );
        GF_BGL_ClearCharSet( GF_BGL_FRAME2_M, 32, 0, heapID);
        GF_BGL_ScrClear( p_draw->p_bgl, GF_BGL_FRAME2_M );
    }

    GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );
    GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

	// サブ面は表示OFF
    GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
    GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
    GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
    GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );
    GF_Disp_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );


	p_handle = ArchiveDataHandleOpen( ARC_WIFIP2PMATCH_GRA, heapID );

	// VCHATアイコン用キャラクタの読込み
    ArcUtil_HDL_PalSet( p_handle, NARC_wifip2pmatch_wf_match_all_icon_NCLR, 
			PALTYPE_MAIN_BG,  WFP2PMF_VCHAT_ICON_PAL*32,
			WFP2PMF_VCHAT_ICON_PALNUM*32, heapID );
	ArcUtil_HDL_BgCharSet( p_handle, NARC_wifip2pmatch_wf_match_all_icon_NCGR,
			p_draw->p_bgl, WFP2PMF_VCHAT_ICON_FRM_NUM, WFP2PMF_VCHAT_ICON_CG*32, WFP2PMF_VCHAT_ICON_CGSIZ*32, FALSE, heapID );
	p_draw->p_vchatscrnbuf = ArcUtil_HDL_ScrnDataGet( p_handle, 
			NARC_wifip2pmatch_wf_match_all_icon_NSCR, FALSE,
			&p_draw->p_vchatscrn, heapID );

	// 背景読み込み
	{
		p_draw->p_plttbuff = ArcUtil_HDL_PalDataGet( p_handle, NARC_wifip2pmatch_wifi_check_bg_NCLR, 
				&p_draw->p_pltt, heapID );
		ArcUtil_HDL_BgCharSet( p_handle, NARC_wifip2pmatch_wifi_check_bg_NCGR,
				p_draw->p_bgl, GF_BGL_FRAME0_M, 0, 0, FALSE, heapID );
		ArcUtil_HDL_ScrnSet( p_handle, NARC_wifip2pmatch_wifi_check_bg_NSCR,
				p_draw->p_bgl, GF_BGL_FRAME0_M, 0, 0, FALSE, heapID );

		// パレットアニメ初期化
		// とりあえず、０フレーム目を送信
		WFP2PMF_PlttAnm_Trans( p_draw, 0 );
		p_draw->pltt_count	= WFP2PMF_BACK_PALANM;
		p_draw->pltt_idx	= 1;
	}
	
	ArchiveDataHandleClose( p_handle );
}

//----------------------------------------------------------------------------
/**
 *	@brief	BGL破棄
 *
 *	@param	p_draw	ワーク
 *
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicBGLDelete( WFP2PMF_DRAW* p_draw )
{
	sys_FreeMemoryEz( p_draw->p_vchatscrnbuf );
	sys_FreeMemoryEz( p_draw->p_plttbuff );
	
    GF_BGL_BGControlExit( p_draw->p_bgl, GF_BGL_FRAME2_M );
    GF_BGL_BGControlExit( p_draw->p_bgl, GF_BGL_FRAME1_M );
    GF_BGL_BGControlExit( p_draw->p_bgl, GF_BGL_FRAME0_M );

    sys_FreeMemoryEz( p_draw->p_bgl );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ビットマップ初期化
 *
 *	@param	p_draw		ワーク
 *	@param	cp_init		初期化データ
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicBmpInit( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, u32 heapID )
{
	int wintype;
	
	// パレット、キャラクタデータ転送
    TalkFontPaletteLoad( PALTYPE_MAIN_BG, WFP2PMF_TALK_PAL*0x20, heapID );
	wintype = CONFIG_GetWindowType(SaveData_GetConfig(cp_init->p_savedata)); 
	TalkWinGraphicSet(
		p_draw->p_bgl, GF_BGL_FRAME1_M, WFP2PMF_BG1_TALKWIN_CGX, WFP2PMF_BG1_TALKWIN_PAL,  wintype, heapID );
	MenuWinGraphicSet(
		p_draw->p_bgl, GF_BGL_FRAME1_M, WFP2PMF_BG1_MENUWIN_CGX, WFP2PMF_BG1_MENUWIN_PAL, 0, heapID );
	MenuWinGraphicSet(
		p_draw->p_bgl, GF_BGL_FRAME2_M, WFP2PMF_BG2_MENUWIN_CGX, WFP2PMF_BG2_MENUWIN_PAL, 0, heapID );

	// ビットマップ作成
	// メッセージウィンドウ
    GF_BGL_BmpWinAdd( p_draw->p_bgl , &p_draw->msgwin, GF_BGL_FRAME1_M,
			WFP2PMF_MSGWIN_X, WFP2PMF_MSGWIN_Y,
			WFP2PMF_MSGWIN_SIZX, WFP2PMF_MSGWIN_SIZY, 
			WFP2PMF_MSGWIN_PAL, WFP2PMF_MSGWIN_CGX );
    GF_BGL_BmpWinDataFill( &p_draw->msgwin, 15 );
    BmpTalkWinWrite(&p_draw->msgwin, WINDOW_TRANS_OFF,
		WFP2PMF_BG1_TALKWIN_CGX, WFP2PMF_BG1_TALKWIN_PAL );


	// タイトルウィンドウ
    GF_BGL_BmpWinAdd( p_draw->p_bgl , &p_draw->titlewin, GF_BGL_FRAME1_M,
			WFP2PMF_TITLEWIN_X, WFP2PMF_TITLEWIN_Y,
			WFP2PMF_TITLEWIN_SIZX, WFP2PMF_TITLEWIN_SIZY, 
			WFP2PMF_TITLEWIN_PAL, WFP2PMF_TITLEWIN_CGX );
    GF_BGL_BmpWinDataFill( &p_draw->titlewin, 15 );
	BmpMenuWinWrite( &p_draw->titlewin, WINDOW_TRANS_OFF, 
			WFP2PMF_BG1_MENUWIN_CGX, WFP2PMF_BG1_MENUWIN_PAL );


	// ボイスチャットウィンドウ
    GF_BGL_BmpWinAdd( p_draw->p_bgl , &p_draw->vchatwin, GF_BGL_FRAME1_M,
			WFP2PMF_VCHATWIN_X, WFP2PMF_VCHATWIN_Y,
			WFP2PMF_VCHATWIN_SIZX, WFP2PMF_VCHATWIN_SIZY, 
			WFP2PMF_VCHATWIN_PAL, WFP2PMF_VCHATWIN_CGX );
    GF_BGL_BmpWinDataFill( &p_draw->vchatwin, 0 );

	// CONNECTリスト
    GF_BGL_BmpWinAdd( p_draw->p_bgl , &p_draw->conlistwin, GF_BGL_FRAME1_M,
			WFP2PMF_CONLISTWIN_X, WFP2PMF_CONLISTWIN_Y,
			WFP2PMF_CONLISTWIN_SIZX, WFP2PMF_CONLISTWIN_SIZY, 
			WFP2PMF_CONLISTWIN_PAL, WFP2PMF_CONLISTWIN_CGX );
    GF_BGL_BmpWinDataFill( &p_draw->conlistwin, 15 );
	BmpMenuWinWrite( &p_draw->conlistwin, WINDOW_TRANS_OFF, 
			WFP2PMF_BG1_MENUWIN_CGX, WFP2PMF_BG1_MENUWIN_PAL );

	// NEWCONウィンドウ
    GF_BGL_BmpWinAdd( p_draw->p_bgl , &p_draw->newconwin, GF_BGL_FRAME1_M,
			WFP2PMF_NEWCONWIN_X, WFP2PMF_NEWCONWIN_Y,
			WFP2PMF_NEWCONWIN_SIZX, WFP2PMF_NEWCONWIN_SIZY, 
			WFP2PMF_NEWCONWIN_PAL, WFP2PMF_NEWCONWIN_CGX );
    GF_BGL_BmpWinDataFill( &p_draw->newconwin, 15 );
}


//----------------------------------------------------------------------------
/**
 *	@brief	ビットマップ破棄
 *
 *	@param	p_draw	ワーク 
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicBmpDelete( WFP2PMF_DRAW* p_draw, u32 heapID )
{
	// メッセージを消す
	WFP2PMF_GraphicMsgBmpStrOff( p_draw );
	
	// ビットマップの破棄
	// メッセージ
	BmpTalkWinClear( &p_draw->msgwin, WINDOW_TRANS_ON );
	GF_BGL_BmpWinDel( &p_draw->msgwin );

	// タイトル
	BmpMenuWinClear( &p_draw->titlewin, WINDOW_TRANS_ON );
	GF_BGL_BmpWinDel( &p_draw->titlewin );

	// ボイスチャット
	GF_BGL_BmpWinDel( &p_draw->vchatwin );

	// CONNECTリスト
	BmpMenuWinClear( &p_draw->conlistwin, WINDOW_TRANS_ON );
	GF_BGL_BmpWinDel( &p_draw->conlistwin );

	// 新コネクトリスト
	BmpMenuWinClear( &p_draw->newconwin, WINDOW_TRANS_ON );
	GF_BGL_BmpWinDel( &p_draw->newconwin );
	
	// YesNoのワークがあったら破棄
	if( p_draw->p_yesnowin != NULL ){
		BmpYesNoWinDel( p_draw->p_yesnowin, heapID );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージデータ初期化
 *
 *	@param	p_draw		ワーク
 *	@param	cp_init		初期化データ
 *	@param	heapID		ヒープ
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicMsgInit( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, u32 heapID )
{
    p_draw->p_wordset  = WORDSET_Create( heapID );
    p_draw->p_msgman = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_wifi_lobby_dat, heapID );
	p_draw->p_msgstr = STRBUF_Create( WFP2PMF_MSGDATA_STRNUM, heapID );
	p_draw->p_msgtmp = STRBUF_Create( WFP2PMF_MSGDATA_STRNUM, heapID );
	p_draw->msg_no = WFP2PMF_MSGNO_NONE;
	p_draw->msg_speed = CONFIG_GetMsgPrintSpeed(SaveData_GetConfig(cp_init->p_savedata));
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージデータ破棄
 *
 *	@param	p_draw		ワーク
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicMsgDelete( WFP2PMF_DRAW* p_draw )
{
	STRBUF_Delete( p_draw->p_msgtmp );
	STRBUF_Delete( p_draw->p_msgstr );
    MSGMAN_Delete( p_draw->p_msgman );
    WORDSET_Delete( p_draw->p_wordset );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ビットマップメッセージ初期化
 *
 *	@param	p_draw		ワーク
 *	@param	cp_ini		初期化データt
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicBmpMsgInit( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, u32 heapID )
{
	// タイトル
	switch( cp_init->type ){
	case WFP2PMF_TYPE_POFIN:				// ポフィン
		break;
		
	case WFP2PMF_TYPE_BUCKET:			// たまいれ
		WORDSET_RegisterWiFiLobbyGameName( p_draw->p_wordset, 0, WFLBY_GAME_BALLSLOW );
		break;
		
	case WFP2PMF_TYPE_BALANCE_BALL:		// たまのり	
		WORDSET_RegisterWiFiLobbyGameName( p_draw->p_wordset, 0, WFLBY_GAME_BALANCEBALL );
		break;
		
	case WFP2PMF_TYPE_BALLOON:			// ふうせんわり
		WORDSET_RegisterWiFiLobbyGameName( p_draw->p_wordset, 0, WFLBY_GAME_BALLOON );
		break;
	}
    MSGMAN_GetString(  p_draw->p_msgman, WFP2PMF_TITLE[ cp_init->type ], p_draw->p_msgtmp );
    WORDSET_ExpandStr(  p_draw->p_wordset, p_draw->p_msgstr, p_draw->p_msgtmp );
	GF_STR_PrintColor( &p_draw->titlewin, FONT_SYSTEM, p_draw->p_msgstr, 0, 0, MSG_NO_PUT, _COL_N_BLACK, NULL);
	GF_BGL_BmpWinOnVReq( &p_draw->titlewin );
	
	// リスト
	GF_BGL_BmpWinOnVReq( &p_draw->conlistwin );
	
	
	// VCHT
    MSGMAN_GetString(  p_draw->p_msgman, msg_wifilobby_132, p_draw->p_msgstr );
    GF_STR_PrintColor( &p_draw->vchatwin, FONT_SYSTEM, p_draw->p_msgstr, 0, 0, MSG_NO_PUT, _COL_N_BLACK_C, NULL);
    GF_BGL_BmpWinOnVReq( &p_draw->vchatwin );
	
	// メッセージ
    GF_BGL_BmpWinOnVReq( &p_draw->msgwin );
}

//----------------------------------------------------------------------------
/**
 *	@brief	セルアクター初期化
 *	
 *	@param	p_draw		ワーク
 *	@param	heapID		ヒープ
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicClactInit( WFP2PMF_DRAW* p_draw, u32 heapID )
{
	int i;

    // OAMマネージャーの初期化
    NNS_G2dInitOamManagerModule();

    // 共有OAMマネージャ作成
    // レンダラ用OAMマネージャ作成
    // ここで作成したOAMマネージャをみんなで共有する
    REND_OAMInit(
        0, 126,		// メイン画面OAM管理領域
        0, 31,		// メイン画面アフィン管理領域
        0, 126,		// サブ画面OAM管理領域
        0, 31,		// サブ画面アフィン管理領域
        heapID);


    // キャラクタマネージャー初期化
    {
        CHAR_MANAGER_MAKE cm = {
            WIFI_P2PMATCH_LOADRESNUM,
            WIFI_P2PMATCH_VRAMMAINSIZ,
            WIFI_P2PMATCH_VRAMSUBSIZ,
            0
        };
		cm.heap = heapID;
        InitCharManagerReg(&cm, GX_OBJVRAMMODE_CHAR_1D_128K, GX_OBJVRAMMODE_CHAR_1D_32K );
    }
    // パレットマネージャー初期化
    InitPlttManager(WIFI_P2PMATCH_LOADRESNUM, heapID);

    // 読み込み開始位置を初期化
    CharLoadStartAll();
    PlttLoadStartAll();

	//通信アイコン用にキャラ＆パレット制限
	CLACT_U_WmIcon_SetReserveAreaCharManager(NNS_G2D_VRAM_TYPE_2DMAIN, GX_OBJVRAMMODE_CHAR_1D_128K);
	CLACT_U_WmIcon_SetReserveAreaPlttManager(NNS_G2D_VRAM_TYPE_2DMAIN);
	

	// セルアクターセット作成
	p_draw->clactSet = CLACT_U_SetEasyInit( WIFI_P2PMATCH_CLACTWK_NUM, &p_draw->renddata, heapID );
	// キャラとパレットのリソースマネージャ作成
	for( i=0; i<WIFI_P2PMATCH_RESNUM; i++ ){
		p_draw->resMan[i] = CLACT_U_ResManagerInit(WIFI_P2PMATCH_LOADRESNUM, i, heapID);
	}


	// オブジェクトシステム初期化
	p_draw->p_objsys = WF2DMAP_OBJSysInit( WIFI_P2PMATCH_CLACTWK_NUM, heapID );
	p_draw->p_objdraw = WF2DMAP_OBJDrawSysInit( p_draw->clactSet, NULL, WIFI_P2PMATCH_CLACTWK_NUM, NNS_G2D_VRAM_TYPE_2DMAIN, heapID );

}

//----------------------------------------------------------------------------
/**
 *	@brief	セルアクター破棄
 *
 *	@param	p_draw		ワーク
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicClactDelete( WFP2PMF_DRAW* p_draw )
{
	int i;

	// オブジェクトワーク破棄
	WF2DMAP_OBJDrawSysExit( p_draw->p_objdraw );	
	WF2DMAP_OBJSysExit( p_draw->p_objsys );	

	// アクターの破棄
	CLACT_DestSet( p_draw->clactSet );

	for( i=0; i<WIFI_P2PMATCH_RESNUM; i++ ){
		CLACT_U_ResManagerDelete( p_draw->resMan[i] );
	}

    // リソース解放
    DeleteCharManager();
    DeletePlttManager();

    //OAMレンダラー破棄
    REND_OAM_Delete();
}


//----------------------------------------------------------------------------
/**
 *	@brief	メッセージデータ表示
 *
 *	@param	p_draw		ワーク
 *	@param	msg_idx		メッセージIDX
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicMsgBmpStrPut( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, u32 msg_idx, u32 heapID )
{
	WFP2PMF_GraphicMsgBmpStrPutEx( p_draw, cp_init, msg_idx, 0, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージ表示	メッセージ終了後のWAIT設定バージョン
 *
 *	@param	p_draw		ワーク
 *	@param	msg_idx		メッセージIDX
 *	@param	wait		待ち時間
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicMsgBmpStrPutEx( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, u32 msg_idx, u8 wait, u32 heapID )
{
	STRBUF* p_expand;
	int wintype;
	
	// １つ前の物が終わっていなかったらとめる
    if( p_draw->msg_no != WFP2PMF_MSGNO_NONE){
        if(GF_MSG_PrintEndCheck( p_draw->msg_no )!=0){
			TOMOYA_PRINT( "msg stop\n" );
            GF_STR_PrintForceStop( p_draw->msg_no );
			p_draw->msg_no = WFP2PMF_MSGNO_NONE;
        }
    }


	
	// 書き込み
    GF_BGL_BmpWinDataFill( &p_draw->msgwin, 15 );
	p_expand = STRBUF_Create( WFP2PMF_MSGDATA_STRNUM, heapID );
	MSGMAN_GetString(  p_draw->p_msgman, msg_idx, p_expand );
	WORDSET_ExpandStr( p_draw->p_wordset, p_draw->p_msgstr, p_expand );
    p_draw->msg_no = GF_STR_PrintColor( &p_draw->msgwin, FONT_TALK, p_draw->p_msgstr, 0, 0, p_draw->msg_speed, _COL_N_BLACK, NULL);
    GF_BGL_BmpWinOnVReq( &p_draw->msgwin );
	STRBUF_Delete( p_expand );


	// ウィンドウデータ再転送
	wintype = CONFIG_GetWindowType(SaveData_GetConfig(cp_init->p_savedata)); 
	TalkWinGraphicSet(
		p_draw->p_bgl, GF_BGL_FRAME1_M, WFP2PMF_BG1_TALKWIN_CGX, WFP2PMF_BG1_TALKWIN_PAL,  wintype, heapID );

	p_draw->msg_wait = wait;
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージを一気に表示
 *
 *	@param	p_draw		ワーク
 *	@param	msg_idx		メッセージIDX
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicMsgBmpStrPutAll( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, u32 msg_idx, u32 heapID )
{
	int wintype;

	// １つ前の物が終わっていなかったらとめる
    if( p_draw->msg_no != WFP2PMF_MSGNO_NONE ){
        if(GF_MSG_PrintEndCheck( p_draw->msg_no )!=0){
            GF_STR_PrintForceStop( p_draw->msg_no );
        }
		p_draw->msg_no = WFP2PMF_MSGNO_NONE;
    }
	
    GF_BGL_BmpWinDataFill( &p_draw->msgwin, 15 );
    MSGMAN_GetString(  p_draw->p_msgman, msg_idx, p_draw->p_msgstr );
    GF_STR_PrintColor( &p_draw->msgwin, FONT_TALK, p_draw->p_msgstr, 0, 0, MSG_NO_PUT, _COL_N_BLACK, NULL);
    GF_BGL_BmpWinOnVReq( &p_draw->msgwin );

	// ウィンドウデータ再転送
	wintype = CONFIG_GetWindowType(SaveData_GetConfig(cp_init->p_savedata)); 
	TalkWinGraphicSet(
		p_draw->p_bgl, GF_BGL_FRAME1_M, WFP2PMF_BG1_TALKWIN_CGX, WFP2PMF_BG1_TALKWIN_PAL,  wintype, heapID );

	p_draw->msg_wait = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージ表示完了チェック
 *
 *	@param	cp_draw		ワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	まだおわってない
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_GraphicMsgBmpStrMain( WFP2PMF_DRAW* p_draw )
{
	if( p_draw->msg_no == WFP2PMF_MSGNO_NONE ){
		if( p_draw->msg_wait > 0 ){
			p_draw->msg_wait --;
			return FALSE;
		}
	}else{

		if( GF_MSG_PrintEndCheck( p_draw->msg_no ) == 0 ){
			if( p_draw->msg_no != WFP2PMF_MSGNO_NONE ){
				p_draw->msg_no = WFP2PMF_MSGNO_NONE;
			}
		}
	}
	
	if( (p_draw->msg_no == WFP2PMF_MSGNO_NONE) && (p_draw->msg_wait == 0) ){
		return TRUE;
	}

	
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージ表示OFF処理
 *
 *	@param	p_draw	ワーク
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicMsgBmpStrOff( WFP2PMF_DRAW* p_draw )
{
	// １つ前の物が終わっていなかったらとめる
    if( p_draw->msg_no != WFP2PMF_MSGNO_NONE ){
        if(GF_MSG_PrintEndCheck( p_draw->msg_no )!=0){
            GF_STR_PrintForceStop( p_draw->msg_no );
        }
		p_draw->msg_no = WFP2PMF_MSGNO_NONE;
    }
    GF_BGL_BmpWinDataFill( &p_draw->msgwin, 15 );
    GF_BGL_BmpWinOnVReq( &p_draw->msgwin );
	p_draw->msg_wait = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	新規コネクトプレイヤーの表示
 *
 *	@param	p_draw		ワーク
 *	@param	cp_init		初期化データ
 *	@param	netid	友達ナンバー
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicNewConStrPut( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, int netid, u32 heapID )
{
	STRBUF* p_expand;
	STRBUF* p_setstr;

	p_expand = STRBUF_Create( WFP2PMF_MSGDATA_STRNUM, heapID );
	p_setstr = STRBUF_Create( WFP2PMF_MSGDATA_STRNUM, heapID );
	
	WFP2PMF_WordSetPlayerNameSet( p_draw, cp_init, netid, 0, heapID );
	WFP2PMF_WordSetPlayerIDSet( p_draw, cp_init, netid, 1, heapID );
	MSGMAN_GetString(  p_draw->p_msgman, msg_wifilobby_135, p_expand );
	WORDSET_ExpandStr( p_draw->p_wordset, p_setstr, p_expand );

	// 書き込む
    GF_BGL_BmpWinDataFill( &p_draw->newconwin, 15 );
    GF_STR_PrintColor( &p_draw->newconwin, FONT_SYSTEM, p_setstr, 0, 0, MSG_NO_PUT, _COL_N_BLACK, NULL);
	BmpMenuWinWrite( &p_draw->newconwin, WINDOW_TRANS_OFF, 
			WFP2PMF_BG1_MENUWIN_CGX, WFP2PMF_BG1_MENUWIN_PAL );
    GF_BGL_BmpWinOnVReq( &p_draw->newconwin );
	
	STRBUF_Delete( p_expand );
	STRBUF_Delete( p_setstr );
}

//----------------------------------------------------------------------------
/**
 *	@brief	新規コネクト表示OFF
 *	
 *	@param	p_draw	描画ワーク
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicNewConStrOff( WFP2PMF_DRAW* p_draw )
{
	// 新コネクトリスト
	BmpMenuWinClear( &p_draw->newconwin, WINDOW_TRANS_OFF );
	GF_BGL_BmpWinOffVReq( &p_draw->newconwin );
}

//----------------------------------------------------------------------------
/**
 *	@brief	リストの全描画
 *
 *	@param	p_wk		ワーク
 *	@param	cp_init		データ
 *	@param	heapID		ヒープ
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicConlistStrPutAll( WFP2PMF_WK* p_wk, const WFP2PMF_INIT* cp_init, u32 heapID )
{
	int i;
	int connum = WIFI_P2PMATCH_NUM_MAX;


	// COMMINFO　初期化づみチェック
	if( CommInfoIsInitialize() == FALSE ){
		return ;
	}
	
	// エントリー人数分表示
	for( i=0; i<connum; i++ ){
		
		if( (i == COMM_PARENT_ID) && (CommInfoIsNewName(i) == TRUE) ){	// 親は確実に
			WFP2PMF_GraphicConlistStrPut( p_wk, &p_wk->draw, cp_init, i, heapID );
		}else{
			if( (CommInfoGetEntry(i) == TRUE) ){	// その他はENTRY状態なら表示
				WFP2PMF_GraphicConlistStrPut( p_wk, &p_wk->draw, cp_init, i, heapID );
			}else{
				// 消す
				WFP2PMF_GraphicConlistStrOff( &p_wk->draw, cp_init, i, heapID );
			}
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	コネクトリスト　メッセージ表示
 *
 *	@param	p_draw		ワーク
 *	@param	cp_init		データ
 *	@param	netid	友達ナンバー
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicConlistStrPut( WFP2PMF_WK* p_wk, WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, int netid, u32 heapID )
{
	STRBUF* p_expand;
	STRBUF* p_setstr;
	u8 vchat;
	u8 friendno;

	p_expand = STRBUF_Create( WFP2PMF_MSGDATA_STRNUM, heapID );
	p_setstr = STRBUF_Create( WFP2PMF_MSGDATA_STRNUM, heapID );
	
	WFP2PMF_WordSetPlayerNameSet( p_draw, cp_init, netid, 0, heapID );
    MSGMAN_GetString(  p_draw->p_msgman, msg_wifilobby_133, p_expand );
    WORDSET_ExpandStr( p_draw->p_wordset, p_setstr, p_expand );

	// 書き込む
    GF_BGL_BmpWinFill( &p_draw->conlistwin, 15, 
			0, netid * (WFP2PMF_CONLISTWIN_ONELIST_Y*8), 
			(WFP2PMF_CONLISTWIN_SIZX*8), (WFP2PMF_CONLISTWIN_ONELIST_Y*8) );

	
    GF_STR_PrintColor( &p_draw->conlistwin, FONT_SYSTEM, p_setstr, 
			0, netid * (WFP2PMF_CONLISTWIN_ONELIST_Y*8),
			MSG_NO_PUT, _COL_N_BLACK, NULL);


	WFP2PMF_WordSetPlayerIDSet( p_draw, cp_init, netid, 0, heapID );
    MSGMAN_GetString(  p_draw->p_msgman, msg_wifilobby_134, p_expand );
    WORDSET_ExpandStr( p_draw->p_wordset, p_setstr, p_expand );

    GF_STR_PrintColor( &p_draw->conlistwin, FONT_SYSTEM, p_setstr, 
			WFP2PMF_CONLISTWIN_ID_X, netid * (WFP2PMF_CONLISTWIN_ONELIST_Y*8),
			MSG_NO_PUT, _COL_N_BLACK, NULL);
	
    GF_BGL_BmpWinOnVReq( &p_draw->conlistwin );

	// VCHATデータ描画
	if( netid == CommGetCurrentID() ){
		if( p_wk->data.p_match->myMatchStatus.vchat ){
			vchat = WFP2PMF_VCHATICON_ON;
		}else{
			vchat = WFP2PMF_VCHATICON_OFF;
		}
		friendno = 0;
	}else{
		if( p_wk->data.vchat_now[netid] ){
			vchat = WFP2PMF_VCHATICON_ON;
		}else{
			vchat = WFP2PMF_VCHATICON_OFF;
		}
	}
	// 友達ナンバが帰ってきていないなら描画しない
	GF_BGL_ScrWriteExpand( p_draw->p_bgl, WFP2PMF_VCHAT_ICON_FRM_NUM,
			WFP2PMF_VCHAT_ICON_WX, 
			WFP2PMF_VCHAT_ICON_WY+(netid*WFP2PMF_CONLISTWIN_ONELIST_Y), 
			WFP2PMF_VCHAT_ICON_SIZ, WFP2PMF_VCHAT_ICON_SIZ,
			p_draw->p_vchatscrn->rawData, 
			vchat*WFP2PMF_VCHAT_ICON_SIZ, 0,
			p_draw->p_vchatscrn->screenWidth/8,
			p_draw->p_vchatscrn->screenHeight/8 );
	GF_BGL_ScrPalChange( p_draw->p_bgl, WFP2PMF_VCHAT_ICON_FRM_NUM,
			WFP2PMF_VCHAT_ICON_WX, 
			WFP2PMF_VCHAT_ICON_WY+(netid*WFP2PMF_CONLISTWIN_ONELIST_Y), 
			WFP2PMF_VCHAT_ICON_SIZ, WFP2PMF_VCHAT_ICON_SIZ, WFP2PMF_VCHAT_ICON_PAL );
	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, WFP2PMF_VCHAT_ICON_FRM_NUM );
	
	STRBUF_Delete( p_expand );
	STRBUF_Delete( p_setstr );
}

//----------------------------------------------------------------------------
/**
 *	@brief	コネクトリスト表示非表示
 *
 *	@param	p_draw		ワーク
 *	@param	cp_init		データ
 *	@param	netid	友達ナンバー
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_GraphicConlistStrOff( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, int netid, u32 heapID )
{
	// 書き込む
    GF_BGL_BmpWinFill( &p_draw->conlistwin, 15, 
			0, netid * (WFP2PMF_CONLISTWIN_ONELIST_Y*8), 
			(WFP2PMF_CONLISTWIN_SIZX*8), (WFP2PMF_CONLISTWIN_ONELIST_Y*8) );

	GF_BGL_ScrWriteExpand( p_draw->p_bgl, WFP2PMF_VCHAT_ICON_FRM_NUM,
			WFP2PMF_VCHAT_ICON_WX, 
			WFP2PMF_VCHAT_ICON_WY+netid*WFP2PMF_CONLISTWIN_ONELIST_Y, 
			WFP2PMF_VCHAT_ICON_SIZ, WFP2PMF_VCHAT_ICON_SIZ,
			p_draw->p_vchatscrn->rawData, 
			WFP2PMF_VCHATICON_NONE*WFP2PMF_VCHAT_ICON_SIZ, 0,
			p_draw->p_vchatscrn->screenWidth/8,
			p_draw->p_vchatscrn->screenHeight/8 );
	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, WFP2PMF_VCHAT_ICON_FRM_NUM );

    GF_BGL_BmpWinOnVReq( &p_draw->conlistwin );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ワードセットにプレイヤー名を設定する
 *
 *	@param	p_draw		ワーク
 *	@param	cp_init		データ
 *	@param	netid	友達番号
 *	@param	set_no		ワードセットへの設定場所
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_WordSetPlayerNameSet( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, int netid, int set_no, u32 heapID )
{
	MYSTATUS* p_target;
	p_target = CommInfoGetMyStatus( netid );
	TOMOYA_PRINT( "player netid %d\n", netid );
	WORDSET_RegisterPlayerName( p_draw->p_wordset, set_no, p_target );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ワードセットにプレイヤーIDを設定する
 *
 *	@param	p_draw		ワーク
 *	@param	cp_init		データ
 *	@param	netid		NETID
 *	@param	set_no		ワードセットへの設定場所
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_WordSetPlayerIDSet( WFP2PMF_DRAW* p_draw, const WFP2PMF_INIT* cp_init, int netid, int set_no, u32 heapID )
{
	MYSTATUS* p_target;
	u16 id;
	p_target = CommInfoGetMyStatus( netid );
	id = MyStatus_GetID_Low( p_target );
	WORDSET_RegisterNumber( p_draw->p_wordset, set_no, id, 5, NUMBER_DISPTYPE_ZERO, NUMBER_CODETYPE_HANKAKU );
}

//----------------------------------------------------------------------------
/**
 *	@brief	パレットアニメメイン
 *
 *	@param	p_draw		ワーク
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_PlttAnm( WFP2PMF_DRAW* p_draw )
{
	if( p_draw->pltt_count <= 0 ){
		//  転送
		WFP2PMF_PlttAnm_Trans( p_draw, p_draw->pltt_idx );

		p_draw->pltt_count	= WFP2PMF_BACK_PALANM;
		p_draw->pltt_idx	= (p_draw->pltt_idx+1) % WFP2PMF_BACK_PALNUM;
	}else{
		p_draw->pltt_count --;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	パレット転送
 *
 *	@param	p_draw		描画ワーク
 *	@param	idx			インデックス
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_PlttAnm_Trans( WFP2PMF_DRAW* p_draw, u32 idx )
{
	BOOL result;
	u16* p_data;
	u32 num;

	num = WFP2PMF_BACK_PAL_IDX[ idx ];

	p_data = p_draw->p_pltt->pRawData;
	result = AddVramTransferManager( NNS_GFD_DST_2D_BG_PLTT_MAIN, WFP2PMF_BACK_PAL*32, 
			&p_data[ num*16 ], 32 );
	GF_ASSERT( result );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ボイスチャット　スイッチを切り替える
 *
 *	@param	p_wk	ワーク
 *
 *	@retval	TRUE	スイッチ　ON
 *	@retval	FALSE	スイッチ　OFF
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_VchatSwitch( WFP2PMF_WK* p_wk, u32 heapID )
{
    p_wk->data.p_match->myMatchStatus.vchat_org = 1 - p_wk->data.p_match->myMatchStatus.vchat_org;
    p_wk->data.p_match->myMatchStatus.vchat = p_wk->data.p_match->myMatchStatus.vchat_org;
    mydwc_setVChat( p_wk->data.p_match->myMatchStatus.vchat );
    mydwc_setMyInfo( &(p_wk->data.p_match->myMatchStatus), sizeof(_WIFI_MACH_STATUS) );
	
	if( p_wk->data.p_match->myMatchStatus.vchat == TRUE ){

		WFP2PMF_StatusVChatOnBmpFade( p_wk );

	}else{
		WFP2PMF_StatusVChatOffBmpFade( p_wk );
	}
	
    return p_wk->data.p_match->myMatchStatus.vchat_org;
}

//----------------------------------------------------------------------------
/**
 *	@brief	　エラーチェック処理
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_ErrCheck( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_param, u32 heapID )
{
	BOOL result;
	BOOL connect_num_errror;
	BOOL timeout;
//	BOOL parent_status_check;
	
	// エラーにいっていたら抜ける
	if( p_wk->data.oya ){
		if( (p_wk->data.status == WFP2PMF_OYA_STATUS_ERR_INIT) ||
			(p_wk->data.status == WFP2PMF_OYA_STATUS_ERR_INIT01) ||
			(p_wk->data.status == WFP2PMF_OYA_STATUS_ERR) ){
			return ;
		}

		// エントリー数が１以下でマッチしていないときは何もしなくてよい
		// ただし、エントリー数が２以上になったら何らかのチェックを行う必要がある
		// エントリー数１というのは自分がエントリーしている状態ということです
		if( CommWifiIsMatched() <= 2 ){	// マッチステート的におかしくなっていたら、エラーチェックする
			if( (WFP2PMF_GetEntryNum( p_wk ) <= 1) && (WFP2PMF_MatchCheck( p_wk ) == FALSE) ){
				return ;
			}
		}


		/*WIFIP2PModeCheckをいれるのでいらない	tomoya 
		// statusチェック切断処理
//		result = WFP2PMF_Oya_CheckConnectPlayer( p_wk, p_param );
		// statusが不正なひとがいたら終了へ
	
//		parent_status_check = TRUE;
//		//*/

	}else{
		if( (p_wk->data.status == WFP2PMF_KO_STATUS_ERR_INIT) ||
			(p_wk->data.status == WFP2PMF_KO_STATUS_ERR_INIT01) ||
			(p_wk->data.status == WFP2PMF_KO_STATUS_ERR) ||
			(p_wk->data.status == WFP2PMF_KO_STATUS_ENTRY_NG) ||
			(p_wk->data.status == WFP2PMF_KO_STATUS_ENTRY_NGINIT) ){
			return ;
		}

	}

	// タイムアウトチェック
	timeout = WFP2PMF_TimeOut_Main( p_wk );

	// 切断チェック
	result = WFP2PMF_LogOutCheck( p_wk );

	//  エントリー数より通信人数が減ったらエラーへ
	if( WFP2PMF_GetEntryNum( p_wk ) > CommGetConnectNum() ){
		connect_num_errror = TRUE;		
	}else{
		connect_num_errror = FALSE;		
	}
	
	// エラーチェック
/*
    if( (CommWifiIsMatched() == 3) || 
		(CommWifiIsMatched() > 4) ||		// 切断チェックは他でやる
*/
	// 080704	tomoya takahashi
    if( (CommWifiIsMatched() >= 3) ||	// Disconnectもみる
		(CommStateIsWifiError()) || 
		CommStateIsWifiDisconnect() || 
		(CommIsConnect(COMM_PARENT_ID) == FALSE) ||	// 親と切断した
		(WFP2PMF_MatchCheck( p_wk ) == FALSE) ||	// 誰かとつながっているのに、通信していないときはだめ
		(connect_num_errror == TRUE) ||		// エントリー数より通信人数がへった
		(timeout == TRUE) ||		// 接続タイムアウト
//		(parent_status_check == FALSE) ||	// 親と通信ステータスが一緒かチェック（_WIFI_MACH_STATUS）
		(result == TRUE) ){			// 切断

#ifdef WFP2P_DEBUG
		if( (CommWifiIsMatched() >= 3) ){
			TOMOYA_PRINT( "(CommWifiIsMatched() == %d)\n", CommWifiIsMatched() );
		}
		if( CommStateIsWifiError() ){
			TOMOYA_PRINT( "CommStateIsWifiError()\n" );
		}
		if( CommStateIsWifiDisconnect() ){
			TOMOYA_PRINT( "CommStateIsWifiDisconnect()\n" );
		}
		if( !CommIsConnect(COMM_PARENT_ID) ){
			TOMOYA_PRINT( "!CommIsConnect(COMM_PARENT_ID)\n" );
		}
		if( (WFP2PMF_MatchCheck( p_wk ) == FALSE) ){
			TOMOYA_PRINT( "(WFP2PMF_MatchCheck( p_wk ) == FALSE)\n" );
		}
		if( result ){
			TOMOYA_PRINT( "errcheck  logout\n" );
		}
		if( timeout ){
			TOMOYA_PRINT( "errcheck  timeout\n" );
		}
#endif

		// CommWifiIsMatched() == 4の切断処理は無視する
		if( p_wk->data.oya ){
			p_wk->data.status = WFP2PMF_OYA_STATUS_ERR_INIT;
		}else{
			p_wk->data.status = WFP2PMF_KO_STATUS_ERR_INIT;
		}
    }
}

//----------------------------------------------------------------------------
/**
 *	@brief	再接続時独自のエラーチェック
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_ReConErrCheck( WFP2PMF_WK* p_wk )
{
	// エラーチェック
#if 0
    if( (CommWifiIsMatched() == 3) || 
		(CommWifiIsMatched() > 4) ||		// 切断チェックは他でやる
		(CommStateIsWifiError())  ){	// 切断
#endif
    if( (CommWifiIsMatched() > 3) || 
		CommStateIsWifiDisconnect() ||
		(CommStateIsWifiError())  ){	// 切断

#ifdef WFP2P_DEBUG
		if( (CommWifiIsMatched() >= 3) ){
			TOMOYA_PRINT( "(CommWifiIsMatched() == %d)\n", CommWifiIsMatched() );
		}
		if( CommStateIsWifiError() ){
			TOMOYA_PRINT( "CommStateIsWifiError()\n" );
		}
#endif

		p_wk->data.status = WFP2PMF_KO_STATUS_ERR_INIT;
    }
}

//----------------------------------------------------------------------------
/**
 *	@brief	誰かとつながっているかチェック
 *
 *	@param	cp_wk	ワーク 
 *
 *	@retval	TRUE	つながっている
 *	@retval	FALSE	つながっていない
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_MatchCheck( const WFP2PMF_WK* cp_wk )
{
	if( CommGetConnectNum() > 0 ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	マッチング開始
 *
 *	@param	friendno		友達番号
 *	@param	type			マッチングタイプ
 *
 *	@retval	TRUE	成功
 *	@retval	FALSE	失敗
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_CommWiFiMatchStart( u32 friendno, u32 type )
{
	BOOL ret;

	ret = CommWifiBattleStart( friendno );
	if( ret == TRUE ){
		switch( type ){
		case WFP2PMF_TYPE_POFIN:				// ポフィン
			CommStateChangeWiFiPofin();
			break;
			
		case WFP2PMF_TYPE_BUCKET:			// たまいれ
		case WFP2PMF_TYPE_BALANCE_BALL:		// たまのり	
		case WFP2PMF_TYPE_BALLOON:			// ふうせんわり
			CommStateChangeWiFiClub();	
			break;
		}
	}
	return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親のステータスと自分のステータスが一致するかチェックする
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	一致
 *	@retval	FALSE	一致しない
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_CheckP2PMatchFriendStatus( const WFP2PMF_WK* cp_wk, const WFP2PMF_INIT* cp_param, int friend )
{
	if( friend == WIFILIST_FRIEND_MAX ){
		return TRUE;
	}

	switch( cp_param->type ){
	case WFP2PMF_TYPE_POFIN:
		if( (cp_wk->data.p_match->friendMatchStatus[ friend ].status == WIFI_STATUS_POFIN) ||
			(cp_wk->data.p_match->friendMatchStatus[ friend ].status == WIFI_STATUS_POFIN_WAIT) ){
			return TRUE;
		}
		break;

	case WFP2PMF_TYPE_BUCKET:
		if( (cp_wk->data.p_match->friendMatchStatus[ friend ].status == WIFI_STATUS_BUCKET) ||
			(cp_wk->data.p_match->friendMatchStatus[ friend ].status == WIFI_STATUS_BUCKET_WAIT) ){
			return TRUE;
		}
		break;

	case WFP2PMF_TYPE_BALANCE_BALL:
		if( (cp_wk->data.p_match->friendMatchStatus[ friend ].status == WIFI_STATUS_BALANCEBALL) ||
			(cp_wk->data.p_match->friendMatchStatus[ friend ].status == WIFI_STATUS_BALANCEBALL_WAIT) ){
			return TRUE;
		}
		break;

	case WFP2PMF_TYPE_BALLOON:
		if( (cp_wk->data.p_match->friendMatchStatus[ friend ].status == WIFI_STATUS_BALLOON) ||
			(cp_wk->data.p_match->friendMatchStatus[ friend ].status == WIFI_STATUS_BALLOON_WAIT) ){
			return TRUE;
		}
		break;
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	状態を、募集状態を実行状態に変更する
 *
 *	@param	p_wk		ワーク
 *	@param	cp_init		データ
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_StatusChange( WFP2PMF_WK* p_wk, const WFP2PMF_INIT* cp_init )
{
	switch( cp_init->type ){
	case WFP2PMF_TYPE_POFIN:
		p_wk->data.p_match->myMatchStatus.status = WIFI_STATUS_POFIN;
		break;

	case WFP2PMF_TYPE_BUCKET:
		p_wk->data.p_match->myMatchStatus.status = WIFI_STATUS_BUCKET;
		break;

	case WFP2PMF_TYPE_BALANCE_BALL:
		p_wk->data.p_match->myMatchStatus.status = WIFI_STATUS_BALANCEBALL;
		break;

	case WFP2PMF_TYPE_BALLOON:
		p_wk->data.p_match->myMatchStatus.status = WIFI_STATUS_BALLOON;
		break;
	}
	mydwc_setMyInfo( &(p_wk->data.p_match->myMatchStatus), sizeof(_WIFI_MACH_STATUS) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ボイスチャット	ON
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_StatusVChatOn( WFP2PMF_WK* p_wk )
{
	if( p_wk->data.vchat_flag == TRUE ){
		// もう動いている
		return FALSE;
	}
	
	// マッチングしていたらボイスチャットを動かす
	if( WFP2PMF_MatchCheck( p_wk ) == TRUE ){
		mydwc_startvchat( HEAPID_WIFI_FOURMATCHVCHAT );
		p_wk->data.vchat_flag = TRUE;

		TOMOYA_PRINT( "vcht on\n" );
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ボイスチャットOFF
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_StatusVChatOff( WFP2PMF_WK* p_wk )
{
	if( p_wk->data.vchat_flag == FALSE ){
		// もうとまってる
		return FALSE;
	}

	mydwc_stopvchat();
	p_wk->data.vchat_flag = FALSE;

	TOMOYA_PRINT( "vcht off\n" );

	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	VCHTの設定とBGMのフェード
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_StatusVChatOnBmpFade( WFP2PMF_WK* p_wk )
{
	if( WFP2PMF_StatusVChatOn( p_wk ) ){
		// BGMの音量を落とす
		Snd_VChatVolSetBySeqNo( Snd_NowBgmNoGet() );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	VCHTの設定とBGMのフェード
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_StatusVChatOffBmpFade( WFP2PMF_WK* p_wk )
{
	if( WFP2PMF_StatusVChatOff( p_wk ) ){
		Snd_PlayerSetInitialVolume( SND_HANDLE_FIELD, BGM_WIFILOBBY_VOL );
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	エントリー数を取得
 */
//-----------------------------------------------------------------------------
static int WFP2PMF_GetEntryNum( WFP2PMF_WK* p_wk )
{
	if( p_wk->data.oya ){
		return CommInfoGetEntryNum();
	}else{
		return CommInfoGetEntryNum();
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	親と自分より小さいNETIDの人はENTRY状態にする
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_SetEntryOya( WFP2PMF_WK* p_wk, u32 heapID )
{
	int i;
	int currentid;
	BOOL result = TRUE;


	// 親と自分のAIDより小さいAIDの子はエントリー状態にする
	currentid = CommGetCurrentID();
	currentid --;

//	TOMOYA_PRINT( "my id = %d\n", currentid + 1 );
	
	for( i=currentid; i>=COMM_PARENT_ID; i-- ){
		if( CommInfoIsBattleNewName( i ) == TRUE ){
			WFP2PMF_CommInfoSetEntry( p_wk, i, heapID );

			TOMOYA_PRINT( "entry id = %d\n", i );
			
		}else if( CommInfoGetEntry( i ) == FALSE ) {
			// NEWENTRYでもなくENTRYでもないときは、
			// まだ受け取っていない人のデータがある
			result = FALSE;
		}
	}
	
	return result;
}

//----------------------------------------------------------------------------
/**
 *	@brief	NEWエントリーの人に通信NGを送信
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_CommSendNewEntryNg( WFP2PMF_WK* p_wk )
{
	int i;
	WFP2PMF_COMM_RESULT result;
	BOOL send_result;

	result.flag = WFP2PMF_CON_NG;

	for( i= 1; i<WIFI_P2PMATCH_NUM_MAX; i++ ){
		if( CommInfoIsBattleNewName( i ) == TRUE ){
			result.netID  = i;
			send_result = CommSendData( CNM_WFP2PMF_RESULT, &result, sizeof(WFP2PMF_COMM_RESULT) );
			GF_ASSERT( send_result == TRUE );
			CommInfoDeletePlayer( i );	// いないことにする
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	つながっているすべてのプレイヤーに切断を通知
 *
 *	@param	p_wk	ワーク
 */	
//-----------------------------------------------------------------------------
static void WFP2PMF_CommSendAllEntryNg( WFP2PMF_WK* p_wk )
{
	int i;
	WFP2PMF_COMM_RESULT result;
	BOOL send_result;

	result.flag = WFP2PMF_CON_NG;

	for( i= 1; i<WIFI_P2PMATCH_NUM_MAX; i++ ){
		if( CommInfoIsNewName( i ) == TRUE ){
			result.netID  = i;
			send_result = CommSendData( CNM_WFP2PMF_RESULT, &result, sizeof(WFP2PMF_COMM_RESULT) );
			GF_ASSERT( send_result == TRUE );
			CommInfoDeletePlayer( i );	// いないことにする
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	LOG Outチェック開始
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_LogOutCheckStart( WFP2PMF_WK* p_wk )
{
	mydwc_setDisconnectCallback( WFP2PMF_LogOutCallBack, p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	LOG out　チェック終了
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_LogOutCheckEnd( WFP2PMF_WK* p_wk )
{
	mydwc_setDisconnectCallback( NULL, NULL );
}

//----------------------------------------------------------------------------
/**
 *	@brief	LOGOUTチェック
 *
 *	@param	p_wk	ワーク
 *
 *	@retval	TRUE	LOGアウト検知		ERR処理へ
 *	@retval	FALSE	LOGアウトなし　または関係ない人がLOGアウトした
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_LogOutCheck( WFP2PMF_WK* p_wk )
{
	int i;
	BOOL result = FALSE;
	
	for( i=0;i<WIFI_P2PMATCH_NUM_MAX; i++ ){
		
		if( p_wk->data.logout_in[i] == TRUE ){

			// その人がENTERじょうたいかチェック
			if( CommInfoGetEntry( p_wk->data.logout_id[i] ) == TRUE ){
				result = TRUE;
			}

			// ワーク初期化
			p_wk->data.logout_in[i] = FALSE;
			p_wk->data.logout_id[i] = 0;
		}
	}

	return result;
}

//----------------------------------------------------------------------------
/**
 *	@brief	LOGOUTコールバック	切断時に呼ばれる
 *
 *	@param	aid			切断したAID
 *	@param	pWork		ワーク
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_LogOutCallBack( u16 aid, void* pWork )
{
	WFP2PMF_WK* p_wk = pWork;
	
	p_wk->data.logout_in[aid] = TRUE;
	p_wk->data.logout_id[aid] = aid;
}

//----------------------------------------------------------------------------
/**
 *	@brief	コネクトリストを再描画する必要があるかチェック
 *
 *	@param	p_wk	ワーク
 *
 *	@retval	TRUE	再描画
 *	@retval FALSE	再描画の必要なし
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_ConListWriteCheck( WFP2PMF_WK* p_wk )
{
	int i;
	BOOL result = FALSE;
	
	// 誰かのVCHATフラグが変わっている
	for( i=0; i<WIFI_P2PMATCH_NUM_MAX; i++ ){
		if( p_wk->data.vchat_tmp[i] != p_wk->data.vchat_now[i] ){
			result = TRUE;
		}
		p_wk->data.vchat_tmp[i] = p_wk->data.vchat_now[i];
	}
	
	// 再描画フラグがたっている
	if( p_wk->data.conlist_rewrite ){
		p_wk->data.conlist_rewrite = FALSE;
		result = TRUE;
	}

	return result;
}

//----------------------------------------------------------------------------
/**
 *	@brief	コネクトリスト再描画フラグの設定
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_ConListWriteReq( WFP2PMF_WK* p_wk )
{
	p_wk->data.conlist_rewrite = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	COMMINFOのワークにエントリーを設定
 *
 *	@param	p_wk	ワーク
 *	@param	aid		AID
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_CommInfoSetEntry( WFP2PMF_WK* p_wk, u32 aid, u32 heapID )
{
	// ENTRYじゃなかったらENTRY
	if( CommInfoGetEntry( aid ) == FALSE ){
		CommInfoSetEntry( aid );

		// 前のNETIDを格納
		p_wk->data.last_netid = aid;
	
		// OAM描画開始
		WFP2PMF_DrawOamInit( p_wk, aid, heapID );

		// コネクトリスト再描画
		WFP2PMF_ConListWriteReq( p_wk );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	OAM描画開始
 *
 *	@param	p_wk		ワーク
 *	@param	aid			AID
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_DrawOamInit( WFP2PMF_WK* p_wk, u32 aid, u32 heapID )
{
	WF2DMAP_OBJDATA objdata;
	MYSTATUS* p_status;

	// もう作ってあるから何もしない
	if( p_wk->draw.objdata[aid].p_objwk != NULL ){
		return ;
	}
 
	// 初期化データコピー
	objdata = WFP2PMF_OBJData[ aid ];
	if( aid != CommGetCurrentID() ){
		p_status = CommInfoGetMyStatus( aid );
		GF_ASSERT( p_status != NULL );
		objdata.charaid = MyStatus_GetTrainerView( p_status );
	}else{
		if( p_wk->data.p_match->myMatchStatus.sex == PM_MALE ){
			objdata.charaid = HERO;
		}else{
			objdata.charaid = HEROINE;
		}
	}
	
	// オブジェクト初期化
	p_wk->draw.objdata[aid].p_objwk = WF2DMAP_OBJWkNew( p_wk->draw.p_objsys, &objdata );


	// 描画リソースの読込み
	if( WF2DMAP_OBJDrawSysResCheck( p_wk->draw.p_objdraw, objdata.charaid ) == FALSE ){
		WF2DMAP_OBJDrawSysResSet( p_wk->draw.p_objdraw, objdata.charaid, 
				WF_2DC_MOVETURN, heapID );
	}


	// 描画ワーク作成
	p_wk->draw.objdata[aid].p_objdrawwk = WF2DMAP_OBJDrawWkNew( 
			p_wk->draw.p_objdraw, p_wk->draw.objdata[aid].p_objwk,
			FALSE, heapID );

	// 最初は非表示
	WF2DMAP_OBJDrawWkDrawFlagSet( p_wk->draw.objdata[aid].p_objdrawwk, FALSE );

	// 動作開始
	p_wk->draw.objdata[aid].seq++;
}

//----------------------------------------------------------------------------
/**
 *	@brief	OAM描画メイン処理
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_DrawOamMain( WFP2PMF_WK* p_wk )
{
	int i;

	// 描画オブジェクト動作
	for( i=0; i<WIFI_P2PMATCH_NUM_MAX; i++ ){
		pDrawObjMoveFunc[p_wk->draw.objdata[i].seq]( p_wk, &p_wk->draw.objdata[i] );
	}

	// コマンドオブジェクト動作
	WF2DMAP_OBJSysMain( p_wk->draw.p_objsys );
	
	// 描画更新
	WF2DMAP_OBJDrawSysUpdata( p_wk->draw.p_objdraw );
}

//----------------------------------------------------------------------------
/**
 *	@brief	OAM描画破棄処理
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_DrawOamDelete( WFP2PMF_WK* p_wk )
{
	// 全リソース破棄
	WF2DMAP_OBJDrawSysAllResDel( p_wk->draw.p_objdraw );

	// 描画オブジェデータ破棄
	memset( p_wk->draw.objdata, 0, sizeof( WFP2PMF_DRAWOBJ )*WIFI_P2PMATCH_CLACTWK_NUM );
}

//----------------------------------------------------------------------------
/**
 *	@brief	子機に通信LOCKを送信
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_NewConLockSend( WFP2PMF_WK* p_wk, u32 aid )
{
	WFP2PMF_COMM_RESULT result;
	BOOL send_result;

	result.netID = aid;
	result.flag = WFP2PMF_CON_LOCK;
	send_result = CommSendData( CNM_WFP2PMF_RESULT, &result, sizeof(WFP2PMF_COMM_RESULT) );
	GF_ASSERT( send_result == TRUE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	last_netIDがENTRYになる前にそれより大きいNETIDの人がNEWENTRYに
 *          なったら再接続してもらう
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_NewConLockNewCon( WFP2PMF_WK* p_wk )
{
	int i;
	
	for( i=0; i<WIFI_P2PMATCH_NUM_MAX; i++ ){
		if( p_wk->data.last_netid+1 < i ){	// 最後にENTRYしたNETIDより大きい

			if( CommInfoIsBattleNewName( i ) == TRUE ){
				// 切断願いを送信
				WFP2PMF_NewConLockSend( p_wk, i );
				// ユーザーデータ破棄
				CommInfoDeletePlayer( i );
			}
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	子の受信した人のENTRY処理
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_KoRecvEntry( WFP2PMF_WK* p_wk, u32 heapID )
{
	int i;

	for( i=0; i<WIFI_P2PMATCH_NUM_MAX; i++ ){
		if( p_wk->data.entry_on[ i ] == TRUE ){

			if( CommInfoIsNewName( i ) == TRUE ){
				WFP2PMF_CommInfoSetEntry( p_wk, i, heapID );

				p_wk->data.entry_on[ i ] = FALSE;

				// 新規コネクトがあった
				p_wk->data.ko_newcon_flag = TRUE;
			}
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	TimeWaitIcon表示
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_TimeWaitIconOn( WFP2PMF_WK* p_wk )
{
	GF_ASSERT( p_wk->draw.p_timewaiticon == NULL );
	p_wk->draw.p_timewaiticon = TimeWaitIconAdd( &p_wk->draw.msgwin, WFP2PMF_BG1_TALKWIN_CGX );
}

//----------------------------------------------------------------------------
/**
 *	@brief	TimeWaitIcon　非表示
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_TimeWaitIconOff( WFP2PMF_WK* p_wk, const WFP2PMF_INIT* cp_init, u32 heapID )
{
	int wintype;
	
	if( p_wk->draw.p_timewaiticon == NULL ){
		return ;
	}

	TimeWaitIconDel(p_wk->draw.p_timewaiticon);  // タイマー止め
	p_wk->draw.p_timewaiticon = NULL;
	
	// ウィンドウデータ再転送
	wintype = CONFIG_GetWindowType(SaveData_GetConfig(cp_init->p_savedata)); 
	TalkWinGraphicSet(
		p_wk->draw.p_bgl, GF_BGL_FRAME1_M, WFP2PMF_BG1_TALKWIN_CGX, WFP2PMF_BG1_TALKWIN_PAL,  wintype, heapID );

}

//----------------------------------------------------------------------------
/**
 *	@brief	描画オブジェ	動作なし
 *	
 *	@param	p_wk		システムワーク
 *	@param	p_obj		オブジェクトデータ
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_DrawObjMoveNone( WFP2PMF_WK* p_wk, WFP2PMF_DRAWOBJ* p_obj )
{
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画オブジェクト　動作初期化
 *
 *	@param	p_wk		システムワーク
 *	@param	p_obj		オブジェクトデータ
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_DrawObjMoveInit( WFP2PMF_WK* p_wk, WFP2PMF_DRAWOBJ* p_obj )
{
	u32 way;
	
	// 描画更新停止
	WF2DMAP_OBJDrawWkUpdataFlagSet( p_obj->p_objdrawwk, FALSE );

	// くるくるアニメ設定
	WF2DMAP_OBJDrawWkKuruAnimeStart( p_obj->p_objdrawwk );

	// 描画開始
	WF2DMAP_OBJDrawWkDrawFlagSet( p_obj->p_objdrawwk, TRUE );

	// 入出動作へ
	p_obj->seq = WFP2PMF_DRAWOBJ_MOVEIN;

	// 入出動作エフェクトタイム
	way = WF2DMAP_OBJWkDataGet( p_obj->p_objwk, WF2DMAP_OBJPM_WAY );
	p_obj->count = WFP2PMF_DRAWOBJ_INTIME + WFP2PMF_DRAWOBJ_INTIME_DIV[way];
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画オブジェクト	入室
 *
 *	@param	p_wk		システムワーク
 *	@param	p_obj		オブジェクトデータ
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_DrawObjMoveIn( WFP2PMF_WK* p_wk, WFP2PMF_DRAWOBJ* p_obj )
{
	// くるくるアニメメイン
	WF2DMAP_OBJDrawWkKuruAnimeMain( p_obj->p_objdrawwk );

	// タイム計算
	p_obj->count --;
	if( p_obj->count <= 0 ){
		// 入出エフェクト終了
		WF2DMAP_OBJDrawWkKuruAnimeEnd( p_obj->p_objdrawwk );

		// 描画アップデート開始
		WF2DMAP_OBJDrawWkUpdataFlagSet( p_obj->p_objdrawwk, TRUE );

		// 通常描画へ
		p_obj->seq = WFP2PMF_DRAWOBJ_MOVENORMAL;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画オブジェクト	通常時
 *
 *	@param	p_wk		システムワーク
 *	@param	p_obj		オブジェクトデータ
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_DrawObjMoveNormal( WFP2PMF_WK* p_wk, WFP2PMF_DRAWOBJ* p_obj )
{
	// 特に何もしない
}

//----------------------------------------------------------------------------
/**
 *	@brief	VChatフラグを送信
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_VchatCommSend( WFP2PMF_WK* p_wk )
{
	int i;
	int roop_num;
	BOOL result = FALSE;
	int friendno;
	int vchat;
	WFP2PMF_COMM_VCHAT comm_vchat;

	// Infoがなくなったら呼ばない
	if( CommInfoIsInitialize() == FALSE ){
		
		return ;
	}

	// エントリー済みの人の数を取得
	roop_num = CommInfoGetEntryNum();
	
	// 新しい人が入ってきたら、今のボイスチャット状態は送信する
    if(CommInfoIsNewInfoData()){
		if(!CommIsSendCommand_ServerSize(CS_COMM_INFO_END)){
			result = TRUE;
		}
    }
	
	// 誰かのVCHATフラグが変わっている
	for( i=0; i<roop_num; i++ ){
		if( i==COMM_PARENT_ID ){
			vchat = p_wk->data.p_match->myMatchStatus.vchat;
		}else{
			friendno = CommInfoSearchWifiListIndex( i );
			GF_ASSERT( friendno != WIFILIST_FRIEND_MAX );
			vchat = p_wk->data.p_match->friendMatchStatus[ friendno ].vchat;
		}
		if( p_wk->data.vchat_now[i] != vchat ){
			result = TRUE;
		}
		p_wk->data.vchat_now[i] =vchat;
	}

	// フラグが変わっていたら送信する
	if( result == TRUE ){
		BOOL send_result;
		memcpy( comm_vchat.vchat_now, p_wk->data.vchat_now, sizeof(u8)*WIFI_P2PMATCH_NUM_MAX );
		CommSendData( CNM_WFP2PMF_VCHAT, &comm_vchat, sizeof(WFP2PMF_COMM_VCHAT) );
		TOMOYA_PRINT( "VCHATFLAG send\n" );
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	マッチングが完了してミニゲームの開始へ進むかチェック
 *
 *	@param	p_wk	ワーク
 *
 *	@retval	TRUE	マッチング完了ー＞ミニゲーム開始へ
 *	@retval	FALSE	まだ募集中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_MatchOkCheck( WFP2PMF_WK* p_wk )
{
	if(CommIsTimingSync(_TIMING_GAME_CHECK)){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ボウケンノート	データ設定
 *
 *	@param	p_param 
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_FNOTE_Set( WFP2PMF_INIT* p_param, u32 heapID )
{
	void* p_buf;
	FNOTE_DATA* p_fnote;
	_WIFI_MACH_STATUS* p_status;

	p_fnote		= SaveData_GetFNote( p_param->p_savedata );
	p_buf = FNOTE_SioIDOnlyDataMake( heapID, FNOTE_ID_PL_WIFICLUB );
	FNOTE_DataSave( p_fnote, p_buf, FNOTE_TYPE_SIO );
}


//----------------------------------------------------------------------------
/**
 *	@brief	接続してきたプレイヤーの状態をチェックする
 *
 *	@param	p_wk	ワーク
 *
 *	@retval	TRUE	正常
 *	@retval	FALSE	不正
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_Oya_CheckConnectPlayer( const WFP2PMF_WK* cp_wk, const WFP2PMF_INIT* cp_param )
{
	int i;
	int friend;
	BOOL ret;
	BOOL result;

	ret = TRUE;
	
	// 通信接続した人全員のSTATUSをチェックし、
	// 不正な人がいたら、切断する。
	for( i=0; i<WIFI_P2PMATCH_NUM_MAX; i++ ){
		if( i != CommGetCurrentID() ){
			if( CommIsConnect(i) == TRUE ){

				// STATUSチェック
				friend = CommInfoSearchWifiListIndex( i );
				if( friend != WIFILIST_FRIEND_MAX ){
					result = WFP2PMF_CheckP2PMatchFriendStatus( cp_wk, cp_param, friend );
				}else{

					// そもそもここにはこないはずだが、
					// その人を切断
					result = FALSE;
				}

				// 切断処理
				if( result == FALSE ){
					ret = FALSE;

					// その人を切断
					DWC_CloseConnectionHard(i);
				}
				
			}
		}
	}

	return ret;
}


//----------------------------------------------------------------------------
/**
 *	@brief	タイムアウト	スタート
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_TimeOut_Start( WFP2PMF_WK* p_wk )
{
	p_wk->data.timeout_flag = TRUE;
	p_wk->data.timeout		= CONNECT_TIMEOUT;
}


//----------------------------------------------------------------------------
/**
 *	@brief	タイムアウト	ストップ
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFP2PMF_TimeOut_Stop( WFP2PMF_WK* p_wk )
{
	p_wk->data.timeout_flag = FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイムアウト	メイン
 *
 *	@param	p_wk	ワーク
 *
 *	@retval	TRUE	エラー状態
 *	@retval	FALSE	通常
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_TimeOut_Main( WFP2PMF_WK* p_wk )
{
	// タイムウエイトチェック中か？
	if( p_wk->data.timeout_flag == FALSE ){
		return FALSE;
	}
	
	if( p_wk->data.timeout > 0 ){
		p_wk->data.timeout--;
		return FALSE;
	}
	return TRUE;
}




//-----------------------------------------------------------------------------
/**
 *		親　状態　関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	親		初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	// VCHATONOFF
	if( p_wk->data.p_match->myMatchStatus.vchat == TRUE ){

		WFP2PMF_StatusVChatOnBmpFade( p_wk );
	}

	// 募集中は、ClosedCallbackが来ても、切断処理に遷移しないようにロックする
	mydwc_SetClosedDisconnectFlag( FALSE );

	CommStateSetErrorCheck(FALSE,TRUE);	// 切断はエラーじゃない

	// OAM描画開始
	WFP2PMF_DrawOamInit( p_wk, CommGetCurrentID(), heapID );

	p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_WAIT_INIT;

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親		コネクト待ち状態初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaConnectWaitInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	// メッセージ
	WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_107, heapID );

	p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_WAIT;

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親		コネクト待ち状態
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaConnectWait( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	BOOL result;


	result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
	// メッセージ表示が終わるまでまつ
	if( result == FALSE ){
		return FALSE;
	}
	
	// 新規コネクト待ち
	// 通信中かチェック
	if( WFP2PMF_MatchCheck( p_wk ) == TRUE ){
		int new_con_netid;
		WFP2PMF_COMM_RESULT result;
		BOOL send_result;


		new_con_netid = CommInfoGetNewNameID();

		if( new_con_netid != INVALID_NETID ){

			// 自分だったらなにもしない
			if( p_wk->data.new_con != COMM_PARENT_ID ){
				// その人を知っているかチェック
				if( CommInfoSearchWifiListIndex( new_con_netid ) == WIFILIST_FRIEND_MAX ){
					// 知らないので、拒否する
					result.netID	= new_con_netid;
					result.flag		= WFP2PMF_CON_NG;		

					// 切断を命令する
					CommInfoDeletePlayer( new_con_netid );

					// 子機に通信切断を通知
					send_result = CommSendData( CNM_WFP2PMF_RESULT, &result, sizeof(WFP2PMF_COMM_RESULT) );
					GF_ASSERT( send_result == TRUE );

				}else{
					p_wk->data.new_con = new_con_netid;
				}
			}
		}
	}

	do{
		// 進むボタンを押した
		if( sys.trg & PAD_BUTTON_DECIDE ){
			Snd_SePlay( _SE_DESIDE );

			// 通信中かチェック
			if( WFP2PMF_MatchCheck( p_wk ) == FALSE ){
				// 誰ともマッチングしていない
				p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_MIN_INIT;
				break;
			}else{
			
				// エントリー数が足りているかチェック
				if( p_init->comm_min <= WFP2PMF_GetEntryNum( p_wk ) ){
					p_wk->data.status = WFP2PMF_OYA_STATUS_START_INIT00;
					break;
				}else{
					// 足りていないので、遊ぶのをやめるのか聞きにいく
					p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_MIN_INIT;
					break;
				}
			}
		}
		// キャンセル押しチェック
		if( sys.trg & PAD_BUTTON_CANCEL ){
			Snd_SePlay( _SE_DESIDE );
//			p_wk->data.status = WFP2PMF_OYA_STATUS_END_INIT00;	// 終了チェックを２から行う
			p_wk->data.status = WFP2PMF_OYA_STATUS_END2_INIT00;	// 終了チェックを２から行う
			break;
		}
		
		// 新コネクトがきた！
		if( p_wk->data.new_con != INVALID_NETID ){

			Snd_SePlay( _SE_DESIDE );

			p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_SELECT_INIT00;
			break;
		}

		// ボイスチャットチェック
		if( sys.trg & PAD_BUTTON_X ){
			p_wk->data.status = WFP2PMF_OYA_STATUS_VCHAT_INIT00;
			Snd_SePlay( _SE_DESIDE );
			break;
		}

	}while(0);
	
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親			新コネクト選択　初期化00
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaConnectSelectInit00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{

	// その人の情報表示
	WFP2PMF_GraphicNewConStrPut( &p_wk->draw, p_init, p_wk->data.new_con, heapID );

	// メッセージの表示
	WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_108, heapID );

	p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_SELECT_INIT01;

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親			新コネクト選択　初期化01
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaConnectSelectInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	BOOL result;
	result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
	// メッセージ表示が終わるまでまつ
	if( result == FALSE ){
		return FALSE;
	}


	// YESNOウィンドウを出す
	p_wk->draw.p_yesnowin = BmpYesNoSelectInit( p_wk->draw.p_bgl,
							&WFP2PMF_YESNOBMPDAT,
							WFP2PMF_BG2_MENUWIN_CGX, WFP2PMF_BG2_MENUWIN_PAL,
							heapID );
	p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_SELECT;
	
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親　新コネクト選択
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaConnectSelect( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	int yesno_ret;
	WFP2PMF_COMM_RESULT result;
	BOOL send_result;
	
	// YESならコネクト　Noなら切断
	yesno_ret = BmpYesNoSelectMain( p_wk->draw.p_yesnowin, heapID );

	
	if( yesno_ret != BMPMENU_NULL ){
		result.netID = p_wk->data.new_con;
	
		if( yesno_ret == 0 ){	// はいを選択
			result.flag = WFP2PMF_CON_OK;		

			// エントリー
			WFP2PMF_CommInfoSetEntry( p_wk, p_wk->data.new_con, heapID );

			
			// Vchat中なら
			if( p_wk->data.p_match->myMatchStatus.vchat == TRUE ){
				// ボイスチャットOFF
				WFP2PMF_StatusVChatOff( p_wk );
			
				// ボイスチャットON
				WFP2PMF_StatusVChatOnBmpFade( p_wk );
			}


			// 人数がMAXになったらゲーム開始へ
			if( p_init->comm_max <= WFP2PMF_GetEntryNum( p_wk ) ){
				p_wk->data.status = WFP2PMF_OYA_STATUS_START_INIT00;
			}else{
				// MAXになっていないなら待ち状態に戻す
				p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_WAIT_INIT;
			}

		}else{

			result.flag = WFP2PMF_CON_NG;		
			// キャンセルなら切断
			// 切断を命令する
			CommInfoDeletePlayer( p_wk->data.new_con );

			// 待ち状態に戻る
			p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_WAIT_INIT;
		}

		// 子機に通信OKを通知
		send_result = CommSendData( CNM_WFP2PMF_RESULT, &result, sizeof(WFP2PMF_COMM_RESULT) );
		GF_ASSERT( send_result == TRUE );

		// 新規コネクト終了
		p_wk->data.new_con = INVALID_NETID;
		WFP2PMF_GraphicNewConStrOff( &p_wk->draw );
		p_wk->draw.p_yesnowin = NULL;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親　開始したいけど、人数が足りなかったとき	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaConnectMinInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	// メッセージ表示
	WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_122, heapID );

	p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_MIN;
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親　開始したいけど、人数が足りなかったとき
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaConnectMin( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	BOOL result;

	result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
	// メッセージ表示が終わるまでまつ
	if( result == FALSE ){
		return FALSE;
	}

	// やめるか聞くシーケンスへ
	p_wk->data.status = WFP2PMF_OYA_STATUS_END_INIT00;
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親　開始	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaStartInit00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	// メッセージ表示	このメンバーでよろしいですが？
	WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_111, heapID );

	p_wk->data.status = WFP2PMF_OYA_STATUS_START_INIT01;
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親　開始	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaStartInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	BOOL result;

	result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
	// メッセージ表示が終わるまでまつ
	if( result == FALSE ){
		return FALSE;
	}

	// YESNOウィンドウを出す
	p_wk->draw.p_yesnowin = BmpYesNoSelectInit( p_wk->draw.p_bgl,
							&WFP2PMF_YESNOBMPDAT,
							WFP2PMF_BG2_MENUWIN_CGX, WFP2PMF_BG2_MENUWIN_PAL,
							heapID );
	p_wk->data.status = WFP2PMF_OYA_STATUS_START_SELECT;
	
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親　開始	選択
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaStartSelect( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	int yesno_ret;
	
	// YESならコネクト　Noなら切断
	yesno_ret = BmpYesNoSelectMain( p_wk->draw.p_yesnowin, heapID );
	if( yesno_ret != BMPMENU_NULL ){
		if( yesno_ret == 0 ){	// はいを選択

			p_wk->data.status = WFP2PMF_OYA_STATUS_START;
		}else{

			// 人数がMAXになったらゲーム終了へ
			if( p_init->comm_max <= WFP2PMF_GetEntryNum( p_wk ) ){
				p_wk->data.status = WFP2PMF_OYA_STATUS_END_INIT00;
			}else{
				p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_WAIT_INIT;
			}
		}

		// 終了
		p_wk->draw.p_yesnowin = NULL;
	}
	
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親　実際に開始
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaStart( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	// 同期開始タイマーセット
	p_wk->data.timer = 30;

	// ENTRY以外の人を切断する
	WFP2PMF_CommSendNewEntryNg( p_wk );

	// 通信待機中メッセージ表示
	WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_066, heapID );

	// 待ちアイコン表示
	WFP2PMF_TimeWaitIconOn( p_wk );

	// タイムウエイト開始
	WFP2PMF_TimeOut_Start( p_wk );

	// 人数チェックへ
	p_wk->data.status = WFP2PMF_OYA_STATUS_STARTNUMCHECK;

	// クライアントロックを一応解除
	mydwc_ResetClientBlock();
	
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親　開始前に　通信人数チェック
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaStartNumCheck( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	int result;
	
	
	// ENTRY人数と通信中の人数が一緒になったら終わる
	if( CommGetConnectNum() != CommInfoGetEntryNum() ){
		TOMOYA_PRINT( "connectnum notmatch\n" );
		// ENTRY以外の人を切断する
		WFP2PMF_CommSendNewEntryNg( p_wk );
		return FALSE;
	}

	// 新規コネクトロック
	result = mydwc_SetClientBlock();
	if( result == FALSE ){
		TOMOYA_PRINT( "mydwc_SetClientBlock false  %d  \n", result );
		return FALSE;
	}

	// ClosedCallbackがきたら切断処理へ遷移する
	mydwc_SetClosedDisconnectFlag( TRUE );
	// 同期開始へ
	p_wk->data.status = WFP2PMF_OYA_STATUS_STARTSYNC_INIT;

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親　同期開始
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaStartSyncInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	BOOL result;	

	if(p_wk->data.timer == 0){
		result = CommSendData( CNM_WFP2PMF_START, NULL, 0 );	// ゲーム開始
		if( result ){
			p_wk->data.status = WFP2PMF_OYA_STATUS_STARTSYNC;
		}
	}else{
		p_wk->data.timer--;
	}
	
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親　同期待ち
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaStartSync( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	if(CommIsTimingSync(_TIMING_GAME_CHECK)){

		// ステータス書き換え
		WFP2PMF_StatusChange( p_wk, p_init );

		CommToolTempDataReset();
		CommTimingSyncStart(_TIMING_GAME_CHECK2);
		p_wk->data.status = WFP2PMF_OYA_STATUS_STATE_SEND;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親　ステータス送信
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaStateSend( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	if(CommIsTimingSync(_TIMING_GAME_CHECK2)){
        u16 status = p_wk->data.p_match->myMatchStatus.status;
		BOOL result;
        result = CommToolSetTempData(CommGetCurrentID() ,&status);
		if( result == TRUE ){
	        p_wk->data.status = WFP2PMF_OYA_STATUS_STATE_CHECK;
		}
    }
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親　ステータスチェック
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaStateCheck( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	int i;
	int con_num;
	int current_id;
	const u16* cp_data;
	int ok_num;

	// エントリー数を取得
	con_num = WFP2PMF_GetEntryNum( p_wk );

	current_id = CommGetCurrentID();

	ok_num = 0;

	for( i=0; i<con_num; i++ ){

		if( current_id != i ){

			// ステータスが一緒かチェック
			cp_data = CommToolGetTempData( i );	

			// ステータスの受信が完了しているかチェック
			if( cp_data != NULL ){
				if( cp_data[ 0 ] == p_wk->data.p_match->myMatchStatus.status ){
					ok_num ++;
				}else{
					p_wk->data.status = WFP2PMF_OYA_STATUS_ERR_INIT;
				}
			}
		}else{
			// 自分はOK
			ok_num ++;
		}
	}

	// 全員のステータスを受信してチェック結果がOKなら次の処理へ
	if( ok_num == con_num ){
		p_wk->data.status = WFP2PMF_OYA_STATUS_MYSTATUS_WAIT;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親　ステータスチェック
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaMyStatusWait( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
//	CommInfoSendPokeData();
	CommStateSetErrorCheck(TRUE,TRUE);
	CommTimingSyncStart(_TIMING_GAME_START2);
	p_wk->data.status = WFP2PMF_OYA_STATUS_GAME_START;

	return FALSE;
}	

//----------------------------------------------------------------------------
/**
 *	@brief	親　	ゲーム開始
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaGameStart( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	if(CommIsTimingSync(_TIMING_GAME_START2)){
		// 同期開始など
		p_wk->data.match_result = TRUE;

		// ボウケンノート設定
		WFP2PMF_FNOTE_Set( p_init, heapID );
		
		// WaitIconを消す
		WFP2PMF_TimeWaitIconOff( p_wk, p_init, heapID );

		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親　終了チェック　初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaEndInit00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	// メッセージの表示
	WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_123, heapID );

	p_wk->data.status = WFP2PMF_OYA_STATUS_END_INIT01;

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親　終了チェック　初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaEndInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	BOOL result;
	result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
	// メッセージ表示が終わるまでまつ
	if( result == FALSE ){
		return FALSE;
	}

	// YESNOウィンドウを出す
	p_wk->draw.p_yesnowin = BmpYesNoSelectInitEx( p_wk->draw.p_bgl,
							&WFP2PMF_YESNOBMPDAT,
							WFP2PMF_BG2_MENUWIN_CGX, WFP2PMF_BG2_MENUWIN_PAL,
							1, heapID );
	p_wk->data.status = WFP2PMF_OYA_STATUS_END_SELECT;
	
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親　終了チェック
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaEndSelect( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	int yesno_ret;
	
	// YESならコネクト　Noなら切断
	yesno_ret = BmpYesNoSelectMain( p_wk->draw.p_yesnowin, heapID );
	if( yesno_ret != BMPMENU_NULL ){
		if( yesno_ret == 0 ){	// はいを選択
			p_wk->data.status = WFP2PMF_OYA_STATUS_END2_INIT00;
		}else{

			// 通信してるかチェック
			if( WFP2PMF_MatchCheck( p_wk ) ){
				// 人数がMAXになったらゲーム終了へ
				if( p_init->comm_max <= WFP2PMF_GetEntryNum( p_wk ) ){
					p_wk->data.status = WFP2PMF_OYA_STATUS_START_INIT00;
				}else{
					p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_WAIT_INIT;
				}
			}else{
				p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_WAIT_INIT;
			}
		}

		p_wk->draw.p_yesnowin = NULL;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親　終了チェック2
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaEnd2Init00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	// メッセージ表示	このメンバーでよろしいですが？
	WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_124, heapID );

	p_wk->data.status = WFP2PMF_OYA_STATUS_END2_INIT01;
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親　終了チェック2
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaEnd2Init01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	BOOL result;

	result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
	// メッセージ表示が終わるまでまつ
	if( result == FALSE ){
		return FALSE;
	}

	// YESNOウィンドウを出す
	p_wk->draw.p_yesnowin = BmpYesNoSelectInitEx( p_wk->draw.p_bgl,
							&WFP2PMF_YESNOBMPDAT,
							WFP2PMF_BG2_MENUWIN_CGX, WFP2PMF_BG2_MENUWIN_PAL,
							1,heapID );
	p_wk->data.status = WFP2PMF_OYA_STATUS_END2_SELECT;
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親　終了チェック2
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaEnd2Select( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	int yesno_ret;
	
	// YESならコネクト　Noなら切断
	yesno_ret = BmpYesNoSelectMain( p_wk->draw.p_yesnowin, heapID );
	if( yesno_ret != BMPMENU_NULL ){
		if( yesno_ret == 0 ){	// はいを選択

			p_wk->data.status = WFP2PMF_OYA_STATUS_END3_INIT;
		}else{

			// 通信してるかチェック
			if( WFP2PMF_MatchCheck( p_wk ) ){
				// 人数がMAXになったらゲーム終了へ
				if( p_init->comm_max <= WFP2PMF_GetEntryNum( p_wk ) ){
					p_wk->data.status = WFP2PMF_OYA_STATUS_START_INIT00;
				}else{
					p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_WAIT_INIT;
				}
			}else{
				p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_WAIT_INIT;
			}
		}

		// 終了
		p_wk->draw.p_yesnowin = NULL;
	}
	
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親　終了チェック3
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaEnd3Init( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_125, heapID );

	// メッセージ表示間隔
	p_wk->data.timer = WIFI_P2PMATCH_MSG_WAIT;

	p_wk->data.status = WFP2PMF_OYA_STATUS_END;
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親　終了処理
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaEnd( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	BOOL result;

	result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
	// メッセージ表示が終わるまでまつ
	if( result == FALSE ){
		return FALSE;
	}

	// メッセージ表示時間が終わるまでまつ
	p_wk->data.timer --;
	if( p_wk->data.timer > 0 ){
		return FALSE;
	}

	
	p_wk->data.match_result = FALSE;

	// マッチング中ならマッチングを解除
//	if( WFP2PMF_MatchCheck(p_wk) ){  //再度つなぎ直す処理へ移行する 080525/k.o/0193
        CommStateWifiMatchEnd();
//	}

	// みんな切断する
	WFP2PMF_CommSendAllEntryNg( p_wk );

	// 通信状態を元に戻す
	CommStateChangeWiFiLogin();


	CommStateSetErrorCheck(FALSE,TRUE);  // エラーがあったら反応する 080525/k.o/0193
	
	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親　エラー処理	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaErrInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	// YesNoのワークがあったら破棄
	if( p_wk->draw.p_yesnowin != NULL ){
		BmpYesNoWinDel( p_wk->draw.p_yesnowin, heapID );
		p_wk->draw.p_yesnowin = NULL;
	}
	WFP2PMF_GraphicNewConStrOff( &p_wk->draw );

	// 待ちアイコン非表示
	WFP2PMF_TimeWaitIconOff( p_wk, p_init, heapID );


	p_wk->data.status = WFP2PMF_OYA_STATUS_ERR_INIT01;
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親　エラー処理	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaErrInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	// メッセージ表示	このメンバーでよろしいですが？
	WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_120, heapID );

	p_wk->data.status = WFP2PMF_OYA_STATUS_ERR;

	// みんな切断する
	WFP2PMF_CommSendAllEntryNg( p_wk );

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親　エラー処理
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaErr( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	BOOL result;

	result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
	// メッセージ表示が終わるまでまつ
	if( result == FALSE ){
		return FALSE;
	}

	// マッチングを解除
    CommStateWifiMatchEnd();


	// 通信状態を元に戻す
	CommStateChangeWiFiLogin();

	// みんな切断する
	WFP2PMF_CommSendAllEntryNg( p_wk );


	// 終了処理
	p_wk->data.match_result = FALSE;

	CommStateSetErrorCheck(FALSE,TRUE);

	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親　vchat処理	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaVchatInit00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	u32 idx;

	if( p_wk->data.p_match->myMatchStatus.vchat == TRUE ){
		idx = msg_wifilobby_128;
	}else{
		idx = msg_wifilobby_127;
	}
	
	// メッセージ表示	このメンバーでよろしいですが？
	WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, idx, heapID );

	p_wk->data.status = WFP2PMF_OYA_STATUS_VCHAT_INIT01;
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親　vchat処理	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaVchatInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	BOOL result;

	result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
	// メッセージ表示が終わるまでまつ
	if( result == FALSE ){
		return FALSE;
	}

	// YESNOウィンドウを出す
	p_wk->draw.p_yesnowin = BmpYesNoSelectInit( p_wk->draw.p_bgl,
							&WFP2PMF_YESNOBMPDAT,
							WFP2PMF_BG2_MENUWIN_CGX, WFP2PMF_BG2_MENUWIN_PAL,
							heapID );
	p_wk->data.status = WFP2PMF_OYA_STATUS_VCHAT_SELECT;
	
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	親　vchat処理
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_OyaVchatSelect( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	int yesno_ret;
	
	// YESならコネクト　Noなら切断
	yesno_ret = BmpYesNoSelectMain( p_wk->draw.p_yesnowin, heapID );
	if( yesno_ret != BMPMENU_NULL ){
		if( yesno_ret == 0 ){	// はいを選択
			
			WFP2PMF_VchatSwitch( p_wk, heapID );

			// コネクトリスト再描画
			WFP2PMF_ConListWriteReq( p_wk );

		}
		p_wk->data.status = WFP2PMF_OYA_STATUS_CONNECT_WAIT_INIT;

		// 終了
		p_wk->draw.p_yesnowin = NULL;
	}
	
	return FALSE;
}



//-----------------------------------------------------------------------------
/**
 *		子　状態　関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	子　親の情報が届くまで待つ
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoEntryOyaWaitInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	CommInfoSendPokeData();	// Infoデータ送信

	TOMOYA_PRINT( "子　通信データ送信\n" );

	CommStateSetErrorCheck(FALSE,TRUE);	// 切断はエラーじゃない

	p_wk->data.status = WFP2PMF_KO_STATUS_ENTRY_OYAWAIT;
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	子　親の情報が届くまで待つ
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoEntryOyaWait( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{


	// 自分より若いNETIDの人のデータを受信したら
	// エントリー状態にする
	WFP2PMF_SetEntryOya( p_wk, heapID );

	// 親から切断されたらえらー処理へ
	if( CommIsConnect(COMM_PARENT_ID) == FALSE ){
		p_wk->data.status = WFP2PMF_KO_STATUS_ERR_INIT;
	}else{
		
		// 親のデータが飛んでくるのは待つ
		if( CommInfoGetEntry( COMM_PARENT_ID ) ){
			p_wk->data.status = WFP2PMF_KO_STATUS_ENTRY_INIT;
		}
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	子　エントリー待ち　初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoEntryInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{

	// 自分より若いNETIDの人のデータを受信したら
	// エントリー状態にする
	WFP2PMF_SetEntryOya( p_wk, heapID );

	// メッセージ
	WFP2PMF_WordSetPlayerNameSet( &p_wk->draw, p_init, COMM_PARENT_ID, 0, heapID );
	WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_118, heapID );

	// 待ちアイコン表示
	WFP2PMF_TimeWaitIconOn( p_wk );

	p_wk->data.status = WFP2PMF_KO_STATUS_ENTRY_WAIT;

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	子　エントリー待ち
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoEntryWait( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	BOOL result;

	// 自分より若いNETIDの人のデータを受信したら
	// エントリー状態にする
	WFP2PMF_SetEntryOya( p_wk, heapID );

	result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
	// メッセージ表示が終わるまでまつ
	if( result == FALSE ){
		return FALSE;
	}

	if( p_wk->data.entry != WFP2PMF_ENTRY_WAIT ){

		// WaitIconを消す
		WFP2PMF_TimeWaitIconOff( p_wk, p_init, heapID );

		switch( p_wk->data.entry ){
		case WFP2PMF_ENTRY_KO:
			p_wk->data.status = WFP2PMF_KO_STATUS_ENTRY_OKINIT;
			break;
		case WFP2PMF_ENTRY_NG:
			p_wk->data.status		= WFP2PMF_KO_STATUS_ENTRY_NGINIT;
			p_wk->data.ng_msg_idx	= msg_wifilobby_130;
			break;
		case WFP2PMF_ENTRY_LOCK:
			p_wk->data.status		= WFP2PMF_KO_STATUS_ENTRY_NGINIT;
			p_wk->data.ng_msg_idx	= msg_wifilobby_136;

			// 080707	tomoya
			// 再接続は、いったんクラブ画面にもどってからとなりました。
//			p_wk->data.status = WFP2PMF_KO_STATUS_CONLOCK_INIT00;
			break;
		}
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	子　エントリー許可おりがとき
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoEntryOkInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	// メッセージ
	WFP2PMF_WordSetPlayerNameSet( &p_wk->draw, p_init, COMM_PARENT_ID, 0, heapID );
	WFP2PMF_GraphicMsgBmpStrPutEx( &p_wk->draw, p_init, msg_wifilobby_129, WFP2PMF_MSGENDWAIT, heapID );

	p_wk->data.status = WFP2PMF_KO_STATUS_ENTRY_OK;

	// 自分をエントリーにする
	WFP2PMF_CommInfoSetEntry( p_wk, CommGetCurrentID(), HEAPID_WIFI_FOURMATCH );

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	子　エントリー許可おりがとき
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoEntryOk( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	BOOL result;

	// 自分より若いNETIDの人のデータを受信したら
	// エントリー状態にする
	WFP2PMF_SetEntryOya( p_wk, heapID );

	result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
	// メッセージ表示が終わるまでまつ
	if( result == FALSE ){
		return FALSE;
	}

	p_wk->data.status = WFP2PMF_KO_STATUS_CONNECT_INIT;

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	子　エントリー許可降りなかったとき
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoEntryNgInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	// メッセージ
	WFP2PMF_WordSetPlayerNameSet( &p_wk->draw, p_init, COMM_PARENT_ID, 0, heapID );
	WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, p_wk->data.ng_msg_idx, heapID );

	p_wk->data.status = WFP2PMF_KO_STATUS_ENTRY_NG;

	// 080703	tomoya	通信終了は、メッセージ表示時に行う
	// マッチングを解除
	CommStateWifiMatchEnd();

	// 通信状態を元に戻す
	CommStateChangeWiFiLogin();

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	子　エントリー許可降りなかったとき
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoEntryNg( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	BOOL result;

	result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
	// メッセージ表示が終わるまでまつ
	if( result == FALSE ){
		return FALSE;
	}

	// 080703	tomoya	通信終了は、メッセージ表示時に行う
#if 0
	// マッチングを解除
	CommStateWifiMatchEnd();

	// 通信状態を元に戻す
	CommStateChangeWiFiLogin();
#endif

	// 終了処理
	p_wk->data.match_result = FALSE;

	CommStateSetErrorCheck(FALSE,TRUE);

	return TRUE;

}


//----------------------------------------------------------------------------
/**
 *	@brief	子　コネクト開始
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoConnectInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{

	// 自分より若いNETIDの人のデータを受信したら
	// エントリー状態にする
	WFP2PMF_SetEntryOya( p_wk, heapID );

	// メッセージ
	WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_126, heapID );

	// 待ちアイコン表示
	WFP2PMF_TimeWaitIconOn( p_wk );

	p_wk->data.status = WFP2PMF_KO_STATUS_CONNECT_WAIT;

	// VCHATONOFF
	if( p_wk->data.p_match->myMatchStatus.vchat == TRUE ){

		WFP2PMF_StatusVChatOnBmpFade( p_wk );
	}


	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	子　メッセージ終了待ち
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoConnectWait( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	BOOL result;

	// 自分より若いNETIDの人のデータを受信したら
	// エントリー状態にする
	WFP2PMF_SetEntryOya( p_wk, heapID );

	result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
	// メッセージ表示が終わるまでまつ
	if( result == FALSE ){
		return FALSE;
	}


	p_wk->data.status = WFP2PMF_KO_STATUS_START;

	return FALSE;

}

//----------------------------------------------------------------------------
/**
 *	@brief	子　同期待ち
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoStart( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{

	// 自分より若いNETIDの人のデータを受信したら
	// エントリー状態にする
	WFP2PMF_SetEntryOya( p_wk, heapID );

	OS_TPrintf( "Ko start \n" );
    
	// 新しい人がつながったらVCHATリセット
	if( p_wk->data.ko_newcon_flag ){
		p_wk->data.ko_newcon_flag = FALSE;

		// Vchat中なら
		if( p_wk->data.p_match->myMatchStatus.vchat == TRUE ){
			// ボイスチャットOFF
			WFP2PMF_StatusVChatOff( p_wk );
			
			// ボイスチャットOPOFIN_WAITN
			WFP2PMF_StatusVChatOnBmpFade( p_wk );
		}

		// コネクトリスト再描画
		WFP2PMF_ConListWriteReq( p_wk );
	}

	if( WFP2PMF_MatchOkCheck( p_wk ) == TRUE ){
		CommToolTempDataReset();
		CommTimingSyncStart(_TIMING_GAME_CHECK2);

		WFP2PMF_TimeWaitIconOff( p_wk, p_init, heapID );

		// 通信待機メッセージ表示
		WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_066, heapID );

		// タイムウエイト開始
		WFP2PMF_TimeOut_Start( p_wk );

		// 子機側の人数チェックウエイト
		p_wk->data.timer = KO_ENTRYNUM_CHECK_WAIT;

		// 待ちアイコン表示
		WFP2PMF_TimeWaitIconOn( p_wk );
		
		p_wk->data.status = WFP2PMF_KO_STATUS_STATE_SEND;
		
	}else{

		// VChatOnOff
		if( sys.trg & PAD_BUTTON_X ){

			// 待ちアイコン非表示
			WFP2PMF_TimeWaitIconOff( p_wk, p_init, heapID );
			p_wk->data.status = WFP2PMF_KO_STATUS_VCHAT_INIT00;
		}
	}
	
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	子　ステータス送信
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoStateSend( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	int con_num;

    if(CommIsTimingSync(_TIMING_GAME_CHECK2)){
        u16 status = p_wk->data.p_match->myMatchStatus.status;
		BOOL result;

        // エントリー数を取得
        con_num = WFP2PMF_GetEntryNum( p_wk );

        if(CommGetConnectNum()!=con_num){
			p_wk->data.timer--;
			if( p_wk->data.timer <= 0 ){
	            // 違ったら切断処理へ
		        p_wk->data.status = WFP2PMF_KO_STATUS_ERR_INIT;
			}
			return FALSE;
        }


        result = CommToolSetTempData(CommGetCurrentID() ,&status);
		if( result == TRUE ){
	        p_wk->data.status = WFP2PMF_KO_STATUS_STATE_CHECK;
		}
        
    }
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	子　ステータスチェック
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoStateCheck( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	int i;
	int con_num;
	int current_id;
	const u16* cp_data;
	int ok_num;

	// エントリー数を取得
	con_num = WFP2PMF_GetEntryNum( p_wk );

	current_id = CommGetCurrentID();

	ok_num = 0;

	for( i=0; i<con_num; i++ ){

		if( current_id != i ){

			// ステータスが一緒かチェック
			cp_data = CommToolGetTempData( i );	

			// ステータスの受信が完了しているかチェック
			if( cp_data != NULL ){
				if( cp_data[ 0 ] == p_wk->data.p_match->myMatchStatus.status ){
					ok_num ++;
				}else{
					// 違ったら切断処理へ
					p_wk->data.status = WFP2PMF_KO_STATUS_ERR_INIT;
				}
			}
		}else{
			// 自分はOK
			ok_num ++;
		}
	}

	// 全員のステータスを受信してチェック結果がOKなら次の処理へ
	if( ok_num == con_num ){
		p_wk->data.status = WFP2PMF_KO_STATUS_MYSTATUS_WAIT;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	子　ステータス系送信
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoMyStatusWait( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
//	CommInfoSendPokeData();
	CommStateSetErrorCheck(TRUE,TRUE);
	CommTimingSyncStart(_TIMING_GAME_START2);
	p_wk->data.status = WFP2PMF_KO_STATUS_GAME_START;

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	子　	ゲーム開始
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoGameStart( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	if(CommIsTimingSync(_TIMING_GAME_START2)){

		// 待ちアイコン非表示
		WFP2PMF_TimeWaitIconOff( p_wk, p_init, heapID );

		// 同期開始など
		p_wk->data.match_result = TRUE;

		// ボウケンノート設定
		WFP2PMF_FNOTE_Set( p_init, heapID );

		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	子　エラー処理	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoErrInit( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{

	// YesNoのワークがあったら破棄
	if( p_wk->draw.p_yesnowin != NULL ){
		BmpYesNoWinDel( p_wk->draw.p_yesnowin, heapID );
		p_wk->draw.p_yesnowin = NULL;
	}
	WFP2PMF_GraphicNewConStrOff( &p_wk->draw );

	// 待ちアイコン表示
	WFP2PMF_TimeWaitIconOff( p_wk, p_init, heapID );


	p_wk->data.status = WFP2PMF_KO_STATUS_ERR_INIT01;
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	子　エラー処理	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoErrInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{

	// メッセージ表示	このメンバーでよろしいですが？
	WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_120, heapID );

	p_wk->data.status = WFP2PMF_KO_STATUS_ERR;

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	子　エラー処理
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoErr( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	BOOL result;

	result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
	// メッセージ表示が終わるまでまつ
	if( result == FALSE ){
		return FALSE;
	}

	// マッチングを解除
    CommStateWifiMatchEnd();


	// 通信状態を元に戻す
	CommStateChangeWiFiLogin();

	// 終了処理
	p_wk->data.match_result = FALSE;

	CommStateSetErrorCheck(FALSE,TRUE);

	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	子　ボイスチャットONOFF
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoVchatInit00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	u32 idx;

	// 自分より若いNETIDの人のデータを受信したら
	// エントリー状態にする
	WFP2PMF_SetEntryOya( p_wk, heapID );


	if( p_wk->data.p_match->myMatchStatus.vchat == TRUE ){
		idx = msg_wifilobby_128;
	}else{
		idx = msg_wifilobby_127;
	}
	
	// メッセージ表示	このメンバーでよろしいですが？
	WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, idx, heapID );

	p_wk->data.status = WFP2PMF_KO_STATUS_VCHAT_INIT01;
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	子　ボイスチャットONOFF
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoVchatInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	BOOL result;


	// 自分より若いNETIDの人のデータを受信したら
	// エントリー状態にする
	WFP2PMF_SetEntryOya( p_wk, heapID );


	// 募集が終わったらVCHAT変更処理はCANCEL
	if( WFP2PMF_MatchOkCheck( p_wk ) == TRUE ){
		p_wk->data.status = WFP2PMF_KO_STATUS_START;
		// メッセージ表示OFF
		WFP2PMF_GraphicMsgBmpStrOff( &p_wk->draw );
		return FALSE;
	}

	result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
	// メッセージ表示が終わるまでまつ
	if( result == FALSE ){
		return FALSE;
	}

	// YESNOウィンドウを出す
	p_wk->draw.p_yesnowin = BmpYesNoSelectInit( p_wk->draw.p_bgl,
							&WFP2PMF_YESNOBMPDAT,
							WFP2PMF_BG2_MENUWIN_CGX, WFP2PMF_BG2_MENUWIN_PAL,
							heapID );
	p_wk->data.status = WFP2PMF_KO_STATUS_VCHAT_SELECT;
	
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	子　ボイスチャットONOFF
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoVchatSelect( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	int yesno_ret;

	// 自分より若いNETIDの人のデータを受信したら
	// エントリー状態にする
	WFP2PMF_SetEntryOya( p_wk, heapID );

	// 募集が終わったらVCHAT変更処理はCANCEL
	if( WFP2PMF_MatchOkCheck( p_wk ) == TRUE ){
		p_wk->data.status = WFP2PMF_KO_STATUS_START;
		// メッセージ表示OFF
		WFP2PMF_GraphicMsgBmpStrOff( &p_wk->draw );
		BmpYesNoWinDel( p_wk->draw.p_yesnowin, heapID );
		p_wk->draw.p_yesnowin = NULL;
		return FALSE;
	}
	
	// YESならコネクト　Noなら切断
	yesno_ret = BmpYesNoSelectMain( p_wk->draw.p_yesnowin, heapID );
	if( yesno_ret != BMPMENU_NULL ){
		if( yesno_ret == 0 ){	// はいを選択
			
			WFP2PMF_VchatSwitch( p_wk, heapID );

			// コネクトリスト再描画
			WFP2PMF_ConListWriteReq( p_wk );

		}
		p_wk->data.status = WFP2PMF_KO_STATUS_CONNECT_INIT;

		// 終了
		p_wk->draw.p_yesnowin = NULL;
	}
	
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	通信ロック中のため　再接続　初期化００
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoConLockInit00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{

	// 親の友達numberを取得
	p_wk->data.oya_friendid = CommInfoSearchWifiListIndex( COMM_PARENT_ID );
	GF_ASSERT( p_wk->data.oya_friendid != WIFILIST_FRIEND_MAX );

	// ワードセットに名前をせっていしとく
	WFP2PMF_WordSetPlayerNameSet( &p_wk->draw, p_init, COMM_PARENT_ID, 0, heapID );


	CommStateWifiMatchEnd();

	// エラーチェック　OFF
	p_wk->data.err_check = FALSE;
	
	// メッセージ表示	このメンバーでよろしいですが？
	WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_136, heapID );

	p_wk->data.status = WFP2PMF_KO_STATUS_CONLOCK_INIT01;
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	通信ロック中のため　再接続　初期化０１
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoConLockInit01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	BOOL result;

	result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
	// メッセージ表示が終わるまでまつ
	if( result == FALSE ){
		return FALSE;
	}

	// YESNOウィンドウを出す
	p_wk->draw.p_yesnowin = BmpYesNoSelectInit( p_wk->draw.p_bgl,
							&WFP2PMF_YESNOBMPDAT,
							WFP2PMF_BG2_MENUWIN_CGX, WFP2PMF_BG2_MENUWIN_PAL,
							heapID );
	p_wk->data.status = WFP2PMF_KO_STATUS_CONLOCK_SELECT;
	
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	通信ロック中のため　再接続　選択
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoConLockSelect( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	int yesno_ret;

	
	// YESならコネクト　Noなら切断
	yesno_ret = BmpYesNoSelectMain( p_wk->draw.p_yesnowin, heapID );
	if( yesno_ret != BMPMENU_NULL ){
		if( yesno_ret == 0 ){	// はいを選択
			p_wk->data.status = WFP2PMF_KO_STATUS_CONLOCK_RECON;
		}else{
			p_wk->data.status = WFP2PMF_KO_STATUS_CONLOCK_DECON;
		}
		// 終了
		p_wk->draw.p_yesnowin = NULL;
	}
	
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	通信ロック中のため　再接続　再接続
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoConLockReCon( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{

	// メッセージ表示	～さんにもうしこみちゅうです
	WFP2PMF_GraphicMsgBmpStrPut( &p_wk->draw, p_init, msg_wifilobby_014, heapID );

	// 待ちアイコン表示
	WFP2PMF_TimeWaitIconOn( p_wk );

	// ウエイト
	p_wk->data.timer = RECONNECT_WAIT;

	p_wk->data.status = WFP2PMF_KO_STATUS_CONLOCK_RECON01;

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	通信ロック中のため　再接続　再接続
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoConLockReCon01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	BOOL result;


	// ウエイト
	if( p_wk->data.timer > 0 ){
		p_wk->data.timer--;
		return FALSE;
	}
	result = WFP2PMF_GraphicMsgBmpStrMain( &p_wk->draw );
	// メッセージ表示が終わるまでまつ
	if( result == FALSE ){
		return FALSE;
	}

	TOMOYA_PRINT( "wifi接続先 %d\n", p_wk->data.oya_friendid );

	// 再接続
	if( WFP2PMF_CommWiFiMatchStart( p_wk->data.oya_friendid, p_init->type ) ){

        p_wk->data.status = WFP2PMF_KO_STATUS_CONLOCK_RECONWAIT;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	通信ロック中のため　再接続　再接続待ち
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoConLockReConWait( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	// ここでは独自のERRチェックを行う
	WFP2PMF_ReConErrCheck( p_wk );
	
	// 待機
	if( (CommWifiIsMatched()==1) && (
		CommIsConnect(COMM_PARENT_ID) == TRUE) ){

		//
		CommSetWifiBothNet(FALSE);
			
		// 待ちアイコン表示
		WFP2PMF_TimeWaitIconOff( p_wk, p_init, heapID );

		// ワイプフェード開始
		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK,
						COMM_BRIGHTNESS_SYNC, 1, heapID);


		p_wk->data.status = WFP2PMF_KO_STATUS_CONLOCK_GRAPHIC_RESET00;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	通信ロック中のため　再接続　画面再構築
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoConLockReConGraphicReset00( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	if( WIPE_SYS_EndCheck() ){

		// まずは通信の開通
		CommInfoInitialize(p_init->p_savedata,NULL);   //Info初期化

		// エラーチェック　ON
		p_wk->data.err_check = TRUE;

		// 画面再構築
		{
			// OAM破棄処理
			WFP2PMF_DrawOamDelete( p_wk );
			// 描画データ破棄
			WFP2PMF_GraphicDelete( p_wk, heapID );

			// 描画ワークをクリーン
			memset( &p_wk->draw, 0, sizeof(WFP2PMF_DRAW) );

			// 切断チェック開始
			WFP2PMF_LogOutCheckStart( p_wk );

			// 描画データ初期化
			WFP2PMF_GraphicInit( p_wk, p_init, heapID );
			
			WirelessIconEasy();  // 接続中なのでアイコン表示
		}

		// ワークのクリア
		{
			p_wk->data.ko_newcon_flag = 0;
			p_wk->data.entry = 0;
			memset( p_wk->data.logout_in, 0, sizeof(u8)*WIFI_P2PMATCH_NUM_MAX );// LOGアウト検知
			memset( p_wk->data.logout_id, 0, sizeof(u8)*WIFI_P2PMATCH_NUM_MAX );// LOGアウト検知
			MI_CpuFill8( p_wk->data.vchat_tmp, TRUE, sizeof(u8)*WIFI_P2PMATCH_NUM_MAX );// １つ前のVCHAT
			MI_CpuFill8( p_wk->data.vchat_now, TRUE, sizeof(u8)*WIFI_P2PMATCH_NUM_MAX );// １つ前のVCHAT
			p_wk->data.conlist_rewrite = 0;	// conlistを再描画
			p_wk->data.oya_friendid = 0;	// 親の友達コード
			memset( p_wk->data.entry_on, 0, sizeof(u8)*WIFI_P2PMATCH_NUM_MAX );// エントリーフラグ
		}

		// ワイプフェード開始
        WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK,
                        COMM_BRIGHTNESS_SYNC, 1, heapID);

		p_wk->data.status = WFP2PMF_KO_STATUS_CONLOCK_GRAPHIC_RESET01;
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	通信ロック中のため　再接続　画面再構築
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoConLockReConGraphicReset01( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	if( WIPE_SYS_EndCheck() ){
		// 接続開始
		p_wk->data.status = WFP2PMF_KO_STATUS_ENTRY_OYAWAITINIT;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	通信ロック中のため　再接続　終了処理
 *
 *	@param	p_wk		ワーク
 *	@param	p_init		上のアプリからのデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	マッチング終了
 *	@retval	FALSE	マッチング中
 */
//-----------------------------------------------------------------------------
static BOOL WFP2PMF_KoConLockDeCon( WFP2PMF_WK* p_wk, WFP2PMF_INIT* p_init, u32 heapID )
{
	// マッチングを解除
    CommStateWifiMatchEnd();


	// 通信状態を元に戻す
	CommStateChangeWiFiLogin();

	// 終了処理
	p_wk->data.match_result = FALSE;

	CommStateSetErrorCheck(FALSE,TRUE);

	return TRUE;
}

