//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		minigame_tool.c
 *	@brief		ミニゲームツール
 *	@author		tomoya takahashi
 *	@data		2007.10.22
 *
 *
 *	作った後の反省会
 *		開始画面と結果画面はPROCとして作成したほうがよかった。
 *		今のシステムだと開始画面と結果画面を出しているのはミニゲーム側ですが、
 *
 *		ミニゲーム監視システム
 *				ー＞開始画面PROC
 *				ー＞ミニゲームPROC
 *				ー＞結果画面PROC
 *		とミニゲームを監視しているシステムが呼び出すようにするべきでした。
 *
 *		こうすることで開始画面と結果画面の変更が直接ミニゲームに影響しなくなり、
 *		開発がもっとスムーズだったと思います。
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#include "system/fontproc.h"
#include "system/clact_util.h"
#include "system/wipe.h"
#include "system/msgdata.h"
#include "system/wordset.h"
#include "system/particle.h"
#include "system/window.h"
#include "system/touch_subwindow.h"
#include "system/laster.h"

#include "savedata/config.h"
#include "savedata/wifilist.h"

#include "contest/con_tool.h"

#include "msgdata/msg.naix"
#include "msgdata/msg_minigame_tool.h"
#include "msgdata/msg_wifi_place_msg_world.h"

#include "graphic/wlmngm_tool.naix"

#include "minigame_tool_snd.h"
#include "application/wifi_lobby/minigame_tool.h"
#include "minigame_tool_local.h"
#include "minigame_commcomand.h"
#include "minigame_commcomand_func.h"
#include "wflby_def.h"

#include "communication/communication.h"
#include "wifi/dwc_rapfriend.h"

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
//#define DEBUG_SYNCSTART_A	// 同期の開始をAボタンにする
//#define DEBUG_MINIGAME_AUTO_PLAY	// WiFiクラブでのみAUTOループプレイ
//#define DEBUG_MINIGAME_DISCONNECT	// Aボタンで切断
#endif


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

#define MNGM_RESCONTID			( 5000 )


//-------------------------------------
///	汎用システム
//=====================================
// セルアクターリソース管理
#define MNGM_CLACTRES_RESMAN_NUM	( 4 )		// リソースマネージャ数
#define MNGM_CLACTRES_DEF_BG_PRI	( 1 )		// BG優先順位

// VRAM転送アニメ　ワーク数
#define MNGM_VRAMTR_WK_NUM			( 16 )

// メッセージ管理
#define MNGM_MSG_STRBUF_NUM			( 128 )
#define MNGM_MSG_PLTT				( 12 )
#define MNGM_MSG_COLOR				( GF_PRINTCOLOR_MAKE( 15,14,0 ) )
#define MNGM_MSG_VIP_COLOR			( GF_PRINTCOLOR_MAKE( 5,6,0 ) )
#define MNGM_MSG_TALKCOLOR			( GF_PRINTCOLOR_MAKE( 1,2,15 ) )
#define MNGM_MSG_TIME_1DATA			( 100 )	// miri秒での1秒の単位
#define MNGM_MSG_TIME_1DATADIV		( 30 )	// 1秒のシンク数

// 会話ウィンドウ管理
enum{
	MNGM_TALKWIN_IDX_MAIN,
	MNGM_TALKWIN_IDX_SUB,
	MNGM_TALKWIN_IDX_NUM,
};
#define MNGM_TALKWIN_CGX			( 1 )	// 風船のウィンドウキャラクタ分
#define MNGM_TALKWIN_PAL			( 13 )
#define MNGM_TALKWIN_BMP_X			( 2 )
#define MNGM_TALKWIN_BMP_Y			( 19 )
#define MNGM_TALKWIN_BMP_Y_SUB		( 1 )
#define MNGM_TALKWIN_BMP_SIZX		( 27 )
#define MNGM_TALKWIN_BMP_SIZY		( 4 )
#define	MNGM_TALKWIN_BMP_PAL		( MNGM_MSG_PLTT )
#define	MNGM_TALKWIN_BMP_CGX		( (MNGM_TALKWIN_CGX + TALK_WIN_CGX_SIZ) )
#define MNGM_TALKWIN_MSG_SPEED		( 1 )
#define MNGM_TALKWIN_BMP_CGX_END	( MNGM_TALKWIN_BMP_CGX + (MNGM_TALKWIN_BMP_SIZX*MNGM_TALKWIN_BMP_SIZY) )
  
//-------------------------------------
///	プレイやーPLATE
//=====================================
#define MNGM_PLAYER_PLATE_SCRN_SIZ_X	( 32 )
#define MNGM_PLAYER_PLATE_SCRN_SIZ_Y	( 6 )
enum{// BGパレットナンバー
	MNGM_PLAYER_BG_PLTT_RED,
	MNGM_PLAYER_BG_PLTT_BLUE,
	MNGM_PLAYER_BG_PLTT_GREEN,
	MNGM_PLAYER_BG_PLTT_YELLOW,
	MNGM_PLAYER_BG_PLTT_ANM,
	MNGM_PLAYER_BG_PLTT_BACK,
	MNGM_PLAYER_BG_PLTT_XX00,
	MNGM_PLAYER_BG_PLTT_XX01,
};
enum{// アニメ用BGパレットナンバー
	MNGM_PLAYER_BG_ANMPLTT_BALLOON,
	MNGM_PLAYER_BG_ANMPLTT_BALLSLOW,
	MNGM_PLAYER_BG_ANMPLTT_BALANCE,
	MNGM_PLAYER_BG_ANMPLTT_TOPANM0_0,
	MNGM_PLAYER_BG_ANMPLTT_TOPANM0_1,
	MNGM_PLAYER_BG_ANMPLTT_TOPANM0_2,
	MNGM_PLAYER_BG_ANMPLTT_TOPANM0_3,
	MNGM_PLAYER_BG_ANMPLTT_TOPANM1_0,
	MNGM_PLAYER_BG_ANMPLTT_TOPANM1_1,
	MNGM_PLAYER_BG_ANMPLTT_TOPANM1_2,
	MNGM_PLAYER_BG_ANMPLTT_TOPANM1_3,
	MNGM_PLAYER_BG_ANMPLTT_ANM
};
// PLATEのOAM位置
#define MNGM_PLATE_CLACT_PLATE_ANM_NUM	( 4 )
static const s16 sc_MNGM_PLATE_CLACT_PLATE_POS[ MNGM_PLATE_CLACT_PLATE_ANM_NUM ][2] = {
	{ 128, 44 },	// Y座標はsc_MNGM_PLAYER_PLATE_DATAのtopメンバからのオフセット
	{ 236, 24 },
	{ 4, 24 },
	{ 128, 4 }
};
// BGパレット数
#define MNGM_BGPLTT_NUM			( 9 )
static const u8 sc_MNGM_BG_PLTT_PLNO_TBL[ WFLBY_MINIGAME_MAX ] = {
	0,1,3,2
};
// OAMアニメシーケンス
enum{
	MNGM_PLAYER_OAM_ANMSEQ_1P,
	MNGM_PLAYER_OAM_ANMSEQ_3P,
	MNGM_PLAYER_OAM_ANMSEQ_4P,
	MNGM_PLAYER_OAM_ANMSEQ_2P,
	MNGM_PLAYER_OAM_ANMSEQ_TOP,
	MNGM_PLAYER_OAM_ANMSEQ_SECOND,
	MNGM_PLAYER_OAM_ANMSEQ_THIRD,
	MNGM_PLAYER_OAM_ANMSEQ_FORE,
};
// OAMパレット数
#define MNGM_PLAYER_OAM_PLTT_NUM   (5)
// ランキングOAMの位置
// Y座標はsc_MNGM_PLAYER_PLATE_DATAのtopメンバからのオフセット
#define MNGM_PLATE_CLACT_RANK_X	( 24 )
#define MNGM_PLATE_CLACT_RANK_Y	( 21 )

// PLATEの動作
#define	MNGM_PLATE_MOVE_START		( -255 )
#define	MNGM_PLATE_MOVE_SPEED		( 28 )
#define	MNGM_PLATE_MOVE_COUNT		( 8 )
#define MNGM_PLATE_MOVE_ENTRY_END	( -8 )
#define MNGM_PLATE_MOVE_WAIT_ENTRY	( 3 )
static const s16 sc_MNGM_PLATE_MOVE_RESULT_END[ WFLBY_MINIGAME_MAX ] = {
	0, -10, -16,  -24
};


//-------------------------------------
///	BG面のスクロールスピード
//=====================================
#define MNGM_BGSCROLL_SPEED	( 2 )



//-------------------------------------
///	エントリー画面ワーク
//=====================================
enum{
	MNGM_ENTRY_SEQ_WIPEIN_INIT,
	MNGM_ENTRY_SEQ_WIPEIN_WAIT,
	MNGM_ENTRY_SEQ_LOGOIN_WAIT,
	MNGM_ENTRY_SEQ_MSGON_INIT,
	MNGM_ENTRY_SEQ_MSGON_WAIT,
	MNGM_ENTRY_SEQ_BGMOVE_START,
	MNGM_ENTRY_SEQ_LOGOOUT_WAIT,
	MNGM_ENTRY_SEQ_PLLIST_ON,
	MNGM_ENTRY_SEQ_BGMOVE_MAIN,
	MNGM_ENTRY_SEQ_BGMOVE_WAIT,
	MNGM_ENTRY_SEQ_BGMOVE_SYNC,
	MNGM_ENTRY_SEQ_BGMOVE_OUT,
	MNGM_ENTRY_SEQ_BGMOVE_OUTWAIT,
	MNGM_ENTRY_SEQ_WIPEOUT_INIT,
	MNGM_ENTRY_SEQ_WIPEOUT_WAIT,
	MNGM_ENTRY_SEQ_RAREGAME_WAIT,
	MNGM_ENTRY_SEQ_END,
};
#define MNGM_ENTRY_MSGWAIT			( 64 )	// メッセージ表示ウエイト
#define MNGM_ENTRY_WAIT				( 92 )	// テーブルを見せるウエイト
#define MNGM_ENTRY_OUTMOVEWAIT		( 8 )	// テーブルを見せるウエイト

// BGL
#define MNGM_ENTRY_BMP_X		( 5 )
#define MNGM_ENTRY_BMP_Y		( 1 )
#define MNGM_ENTRY_BMP_Y_DIF	( 6 )
#define MNGM_ENTRY_BMP_SIZX		( 26 )
#define MNGM_ENTRY_BMP_SIZY		( 4 )
#define MNGM_ENTRY_BMP_PAL		( MNGM_MSG_PLTT )
#define MNGM_ENTRY_BMP_CGX		( 513 )

// OAM設定
#define MNGM_ENTRY_CLACT_RESNUM		( 1 )	// セルアクターセットのワーク数
#define MNGM_ENTRY_CLACT_WKNUM		( 16 )	// セルアクターセットのワーク数
#define MNGM_ENTRY_CLACT_CHARNUM	( 1 )	// セルアクターキャラクタのワーク数
#define MNGM_ENTRY_CLACT_PLTTNUM	( 1 )	// セルアクターパレットのワーク数

// エントリー表示メッセージ位置
#define MNGM_ENTRY_DRAWNAME_X	( 8 )


//-------------------------------------
///	リザルト画面ワーク
//=====================================
enum{	// 玉いれ
	MNGM_RESULT_BALLSLOW_SEQ_WIPEIN_INIT,
	MNGM_RESULT_BALLSLOW_SEQ_WIPEIN_WAIT,
	MNGM_RESULT_BALLSLOW_SEQ_MSGON_INIT,
	MNGM_RESULT_BALLSLOW_SEQ_MSGON_WAIT,
	MNGM_RESULT_BALLSLOW_SEQ_MSGON_WAIT2,
	MNGM_RESULT_BALLSLOW_SEQ_COUNTDOWN_INIT,
	MNGM_RESULT_BALLSLOW_SEQ_COUNTDOWN,
	MNGM_RESULT_BALLSLOW_SEQ_TBLMOVE_WAIT,
	MNGM_RESULT_BALLSLOW_SEQ_EFFECT_ON,
	MNGM_RESULT_BALLSLOW_SEQ_EFFECT_WAIT,

	// ガジェット出す場合
	MNGM_RESULT_BALLSLOW_SEQ_GADGET_MSG00_ON,	// 順位に対するメッセージ
	MNGM_RESULT_BALLSLOW_SEQ_GADGET_MSG00_WAIT,
	MNGM_RESULT_BALLSLOW_SEQ_GADGET_MSG_ON,		// ガジェットに対するメッセージ
	MNGM_RESULT_BALLSLOW_SEQ_GADGET_MSG_WAIT,
	MNGM_RESULT_BALLSLOW_SEQ_GADGET_MSG_WAIT2,
	
	// 終了同期
	MNGM_RESULT_BALLSLOW_SEQ_SYNCSTART,
	MNGM_RESULT_BALLSLOW_SEQ_SYNCWAIT,

	// そのまま終了する場合
	MNGM_RESULT_BALLSLOW_SEQ_WIPEOUT_INIT,
	MNGM_RESULT_BALLSLOW_SEQ_WIPEOUT_WAIT,

	// 再挑戦を聞く場合
	MNGM_RESULT_BALLSLOW_SEQ_RETRY_INIT,
	MNGM_RESULT_BALLSLOW_SEQ_RETRY_MAIN,

	MNGM_RESULT_BALLSLOW_SEQ_END,

	// 全員のスコアが０のときの処理
	MNGM_RESULT_BALLSLOW_SEQ_ALLSCORE0_WAIT,
	MNGM_RESULT_BALLSLOW_SEQ_ALLSCORE0_DRAW,
};

enum{	// 風千わり
	MNGM_RESULT_BALLOON_SEQ_WIPEIN_INIT,
	MNGM_RESULT_BALLOON_SEQ_WIPEIN_WAIT,
	MNGM_RESULT_BALLOON_SEQ_MSGON_INIT,
	MNGM_RESULT_BALLOON_SEQ_MSGON_WAIT,
	MNGM_RESULT_BALLOON_SEQ_MSGON_WAIT2,

	// 風船表示部分
	MNGM_RESULT_BALLOON_SEQ_TBLON,
	MNGM_RESULT_BALLOON_SEQ_BALLOON_ON,
	MNGM_RESULT_BALLOON_SEQ_BALLOON_ONWAIT,

	// 割った数表示
	MNGM_RESULT_BALLOON_SEQ_GADGET_MSG_00ON,
	MNGM_RESULT_BALLOON_SEQ_GADGET_MSG_00WAIT,

	// ガジェット出す場合
	MNGM_RESULT_BALLOON_SEQ_GADGET_MSG_ON,
	MNGM_RESULT_BALLOON_SEQ_GADGET_MSG_WAIT,
	MNGM_RESULT_BALLOON_SEQ_GADGET_MSG_WAIT2,

	// エフェクトを出す
	MNGM_RESULT_BALLOON_SEQ_EFFECT_WAIT,
	
	
	// 終了同期
	MNGM_RESULT_BALLOON_SEQ_SYNCSTART,
	MNGM_RESULT_BALLOON_SEQ_SYNCWAIT,

	// そのまま終了する場合
	MNGM_RESULT_BALLOON_SEQ_WIPEOUT_INIT,
	MNGM_RESULT_BALLOON_SEQ_WIPEOUT_WAIT,

	// 再挑戦を聞く場合
	MNGM_RESULT_BALLOON_SEQ_RETRY_INIT,
	MNGM_RESULT_BALLOON_SEQ_RETRY_MAIN,

	MNGM_RESULT_BALLOON_SEQ_END,
};

#define MNGM_RESULT_MSGDRAW_WAIT		( 102 )	// ウエイト
#define MNGM_RESULT_RESULT_DRAW_WAIT	( 30 )// 結果を見せる時間
#define MNGM_RESULT_RESULT_SYNC_WAIT	( 102 )// 結果を見せる時間
#define MNGM_RESULT_BALLOON_DRAW_WAIT	( 16 )	// 風船を出すまでの時間

// OAM設定
#define MNGM_RESULT_CLACT_RESNUM	( 2 )	// セルアクターセットのワーク数
#define MNGM_RESULT_CLACT_WKNUM		( 32 )	// セルアクターセットのワーク数
#define MNGM_RESULT_CLACT_CHARNUM	( 2 )	// セルアクターキャラクタのワーク数
#define MNGM_RESULT_CLACT_PLTTNUM	( 2 )	// セルアクターパレットのワーク数

// 結果表示ウエイト
#define MNGM_RESULT_DRAWON_WAIT_34	( 16 )	// 表示間隔
#define MNGM_RESULT_DRAWON_WAIT_12	( 32 )	// 表示間隔
#define MNGM_RESULT_DRAWNAME_X	( 8 )
#define MNGM_RESULT_DRAWNAME_Y	( 0 )
#define MNGM_RESULT_DRAWSCORE_X	( 176 )
#define MNGM_RESULT_DRAWSCORE_Y	( 0 )

// パレット転送アニメ
#define MNGM_RESULT_PALANM_DEFTRPAL	( 0 )	// 基本転送パレット位置
#define MNGM_RESULT_PALANM_FRAME	( 8 )	// パレットアニメフレーム

// 結果画面のスクリーン数
enum{
	MNGM_RESULT_SCRN_TBL,
	MNGM_RESULT_SCRN_NUM,
};

// リトライワーク
#define MNGM_RESULT_RETRY_CHAR_OFFS		(MNGM_TALKWIN_BMP_CGX_END)	// キャラクタオフセット
#define MNGM_RESULT_RETRY_PLTT_OFFS		(2)	// パレットオフセット
#define MNGM_RESULT_RETRY_X				(24)
#define MNGM_RESULT_RETRY_Y				(8)
enum{
	MNGM_RESULT_RETRY_SEQ_WIPEIN,		// ワイプアウト
	MNGM_RESULT_RETRY_SEQ_WIPEINWAIT,	// ワイプアウトウエイト
	MNGM_RESULT_RETRY_SEQ_MSGON,		// メッセージ表示
	MNGM_RESULT_RETRY_SEQ_MSGWAIT,	
	MNGM_RESULT_RETRY_SEQ_TPON,			// タッチボタン表示
	MNGM_RESULT_RETRY_SEQ_TPMAIN,
	MNGM_RESULT_RETRY_SEQ_RECV,			// みんなの選択を受信
	MNGM_RESULT_RETRY_SEQ_RECVMSGON,	// 誰かがやめるを選んだらつごうが・・・
	MNGM_RESULT_RETRY_SEQ_RECVMSGWAIT,	// 
	MNGM_RESULT_RETRY_SEQ_RECVMSGWAIT2,	// 
	MNGM_RESULT_RETRY_SEQ_WIPEOUT,		// ワイプアウト
	MNGM_RESULT_RETRY_SEQ_WIPEOUTWAIT,	// ワイプアウトウエイト
	MNGM_RESULT_RETRY_SEQ_END,
};

// BALLOON
enum{
	MNGM_RESULT_BALLOON_SEQ_ON,
	MNGM_RESULT_BALLOON_SEQ_BLN_ON,
	MNGM_RESULT_BALLOON_SEQ_BLN_ONWAIT,
	MNGM_RESULT_BALLOON_SEQ_BLN_END,
};
// OAM
#define MNGM_RESULT_BALLOON_X				( 5 )
#define MNGM_RESULT_BALLOON_Y				( 3 )
#define MNGM_RESULT_BALLOON_CLACT_NUM		( MNGM_RESULT_BALLOON_X*MNGM_RESULT_BALLOON_Y )
#define MNGM_RESULT_BALLOON_CONTID			( MNGM_RESCONTID+1 )
#define MNGM_RESULT_BALLOON_BG_PRI			( 0 )
#define MNGM_RESULT_BALLOON_DEF_X			( 74 )
#define MNGM_RESULT_BALLOON_SIZ_X			( 24 )
#define MNGM_RESULT_BALLOON_DEF_Y			( 40 )
#define MNGM_RESULT_BALLOON_SIZ_Y			( 36 )
#define MNGM_RESULT_BALLOON_ANM_RED			( 2 )

// BG
#define MNGM_RESULT_BALLOON_SYSWIN_CGX		( MNGM_TALKWIN_BMP_CGX_END )
#define MNGM_RESULT_BALLOON_SYSWIN_PAL		( MNGM_TALKWIN_PAL+1 )
#define MNGM_RESULT_BALLOON_SYSWIN_SCRNSIZX	( 32 )
#define MNGM_RESULT_BALLOON_SYSWIN_SCRNSIZY	( 20 )
/*
#define MNGM_RESULT_BALLOON_BMP_X			( 4 )
#define MNGM_RESULT_BALLOON_BMP_Y			( 3 )
#define MNGM_RESULT_BALLOON_BMP_SIZX		( 23 )
#define MNGM_RESULT_BALLOON_BMP_SIZY		( 14 )
#define MNGM_RESULT_BALLOON_BMP_PAL			( MNGM_MSG_PLTT )
#define MNGM_RESULT_BALLOON_BMP_CGX			( MNGM_RESULT_BALLOON_SYSWIN_CGX+MENU_WIN_CGX_SIZ )
*/

// 動き
#define MNGM_RESULT_BALLOON_WAIT			( 8 )




//-------------------------------------
///	START	TIMEUP	ワーク
//=====================================
#define MNGM_COUNT_OAMRESNUM	(1)
enum{	// REDY	START
	MNGM_COUNT_START_SEQ_NONE,
	MNGM_COUNT_START_SEQ_INIT,
	MNGM_COUNT_START_SEQ_MSK,
	MNGM_COUNT_START_SEQ_ANM,
	MNGM_COUNT_START_SEQ_MSK2,
	MNGM_COUNT_START_SEQ_END,
};
enum{	// TIME UP
	MNGM_COUNT_TIMEUP_SEQ_NONE,
	MNGM_COUNT_TIMEUP_SEQ_INIT,
	MNGM_COUNT_TIMEUP_SEQ_MSK,
	MNGM_COUNT_TIMEUP_SEQ_ANM,
	MNGM_COUNT_TIMEUP_SEQ_MSK2,
	MNGM_COUNT_TIMEUP_SEQ_END,
};
#define MNGM_COUNT_ANM_X	( 128 )
#define MNGM_COUNT_ANM_Y	( 97 )
#define MNGM_COUNT_ANM_PRI	( 64 )
#define MNGM_COUNT_TIMEUP_ANM_NUM	( 50 )
#define MNGM_COUNT_REDY_ANM			( 7 )
#define MNGM_COUNT_3_ANM			( 9 )
#define MNGM_COUNT_2_ANM			( 11 )
#define MNGM_COUNT_1_ANM			( 13 )
#define MNGM_COUNT_STARTFRAME		( 15 )
#define MNGM_COUNT_STARTPALANMTIMING	( 4 )	// パレットアニメタイミング
// マスクシステム
enum{
	MNGM_COUNT_MSK_OAM_TOP,
	MNGM_COUNT_MSK_OAM_BTTM,
	MNGM_COUNT_MSK_OAM_NUM,
};
#define MNGM_COUNT_MSK_X		( 128 )
#define MNGM_COUNT_MSK_Y		( 96 )
#define MNGM_COUNT_MSK_Y_D		( 2 )
#define MNGM_COUNT_MSK_PRI		( 0 )
#define MNGM_COUNT_MSK_MY		( 23 )	// Yに移動する値
#define MNGM_COUNT_MSK_COUNT	( 8 )	// 動作カウント
//#define MNGM_COUNT_MSK_COUNT	( 512 )	// 動作カウント
static const VecFx32 sc_MNGM_COUNT_MSK_MAT[ MNGM_COUNT_MSK_OAM_NUM ] = {
	{ FX32_CONST( MNGM_COUNT_MSK_X ), FX32_CONST( MNGM_COUNT_MSK_Y ), 0 },
	{ FX32_CONST( MNGM_COUNT_MSK_X ), FX32_CONST( MNGM_COUNT_MSK_Y+MNGM_COUNT_MSK_Y_D ), 0 },
};

#define MNGM_COUNT_MSK_SY0		( MNGM_COUNT_MSK_Y - MNGM_COUNT_MSK_MY )
#define MNGM_COUNT_MSK_SY1		( (MNGM_COUNT_MSK_Y+MNGM_COUNT_MSK_Y_D) + MNGM_COUNT_MSK_MY )
enum{
	MNGM_COUNT_MSK_HBUF_READ,
	MNGM_COUNT_MSK_HBUF_WRITE,
	MNGM_COUNT_MSK_HBUF_NUM,
};



//-------------------------------------
///	タイトルロゴシステム
//=====================================
#define MNGM_TITLELOGO_BMP_X	(6)
#define MNGM_TITLELOGO_BMP_Y	(11)
#define MNGM_TITLELOGO_BMP_SIZX	(20)
#define MNGM_TITLELOGO_BMP_SIZY	(10)
#define MNGM_TITLELOGO_BMP_PAL	(8)
#define MNGM_TITLELOGO_BMP_CGX	(MNGM_ENTRY_BMP_CGX)
static const GF_PRINTCOLOR sc_MNGM_TITLELOGO_BMP_COL[ WFLBY_GAME_P2PGAME_NUM ] = {
	GF_PRINTCOLOR_MAKE( 7,8,9 ),
	GF_PRINTCOLOR_MAKE( 4,5,6 ),
	GF_PRINTCOLOR_MAKE( 1,2,3 ),
};

#define MNGM_TITLELOGO_BMP_SIZXDOT	(MNGM_TITLELOGO_BMP_SIZX*8)

// 動作シーケンス
enum{
	MNGM_TITLELOGO_INMOVESEQ_IN,
	MNGM_TITLELOGO_INMOVESEQ_BOUND00,
	MNGM_TITLELOGO_INMOVESEQ_BOUND01,
	MNGM_TITLELOGO_INMOVESEQ_END,
};
enum{
	MNGM_TITLELOGO_OUTMOVESEQ_OUT,
	MNGM_TITLELOGO_OUTMOVESEQ_END,
};
// 動作の値		風船割りはY座標に反映	玉いれと玉乗りは横
// 動作０	イン
static const fx32 sc_MNGM_TITLELOGO_MOVE00_START[ WFLBY_GAME_P2PGAME_NUM ] = {
	-FX32_CONST(200),
	FX32_CONST(200),
	FX32_CONST(160),
};
static const fx32 sc_MNGM_TITLELOGO_MOVE00_END[ WFLBY_GAME_P2PGAME_NUM ] = {
	FX32_CONST(0),
	FX32_CONST(0),
	FX32_CONST(0),
};
static const fx32 sc_MNGM_TITLELOGO_MOVE00_SPEED[ WFLBY_GAME_P2PGAME_NUM ] = {
	FX32_CONST(0),
	FX32_CONST(0),
	FX32_CONST(0),
};
#define MNGM_TITLELOGO_MOVE00_COUNTMAX	( 16 )

// 動作１	はねる０	初期位置は動作０の終了位置
static const fx32 sc_MNGM_TITLELOGO_MOVE01_END[ WFLBY_GAME_P2PGAME_NUM ] = {
	-FX32_CONST(18),
	FX32_CONST(18),
	FX32_CONST(16),
};
static const fx32 sc_MNGM_TITLELOGO_MOVE01_SPEED[ WFLBY_GAME_P2PGAME_NUM ] = {
	-FX32_CONST(9),
	FX32_CONST(9),
	FX32_CONST(8),
};
#define MNGM_TITLELOGO_MOVE01_COUNTMAX	( 4 )

// 動作２	はねる１	初期位置は動作１の終了位置　　終了位置は動作０の終了値
static const fx32 sc_MNGM_TITLELOGO_MOVE02_SPEED[ WFLBY_GAME_P2PGAME_NUM ] = {
	FX32_CONST(0),
	FX32_CONST(0),
	FX32_CONST(0),
};
#define MNGM_TITLELOGO_MOVE02_COUNTMAX	( 4 )

// 動作３	アウト	初期位置は動作０の終了位置
static const fx32 sc_MNGM_TITLELOGO_MOVE03_END[ WFLBY_GAME_P2PGAME_NUM ] = {
	FX32_CONST(200),
	-FX32_CONST(200),
	-FX32_CONST(160),
};
static const fx32 sc_MNGM_TITLELOGO_MOVE03_SPEED[ WFLBY_GAME_P2PGAME_NUM ] = {
	FX32_CONST(0),
	FX32_CONST(0),
	FX32_CONST(0),
};
#define MNGM_TITLELOGO_MOVE03_COUNTMAX	( 8 )




//-------------------------------------
///	HBLANKBGスクロールシステム
//=====================================
#define WFLBY_HBLANK_BGSCR_BUFNUM	(192)


//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	汎用システム
//=====================================

//	汎用加速動作
typedef struct {
	fx32 x;
	fx32 s_x;
	fx32 s_s;	// 初速度
	fx32 s_a;	// 加速度
	int count_max;
} MNGM_ADDMOVE_WORK;



// セルアクターリソース管理
typedef struct {
	BOOL				data;		// データがあるか
	CLACT_U_RES_OBJ_PTR	p_resobj[ MNGM_CLACTRES_RESMAN_NUM ];
	CLACT_HEADER		header;
} MNGM_CLACTRESOBJ;

typedef struct {
	CLACT_U_RES_MANAGER_PTR	p_resman[MNGM_CLACTRES_RESMAN_NUM];
	MNGM_CLACTRESOBJ*		p_obj;
	u32						objnum;
} MNGM_CLACTRES;

// セルアクター管理
typedef struct{
    CLACT_SET_PTR           p_clactset;		// セルアクターセット
    CLACT_U_EASYRENDER_DATA renddata;       // 簡易レンダーデータ
} MNGM_CLACT;

// BGL管理
typedef struct{
	u32					frame;
	GF_BGL_BGCNT_HEADER	cnt;
} MNGM_BGL_DATA;
typedef struct {
	GF_BGL_INI*				p_bgl;	// bgl
	const MNGM_BGL_DATA*	cp_tbl;	// bglデータテーブル 
	u32						tblnum;	// データテーブル数
} MNGM_BGL;

//  メッセージデータ管理
typedef struct {
	MSGDATA_MANAGER*	p_msgman;
	WORDSET*			p_wordset;
	STRBUF*				p_str;
	STRBUF*				p_tmp;
} MNGM_MSG;

// トークウィンドウシステム
typedef struct {
	GF_BGL_BMPWIN	win[MNGM_TALKWIN_IDX_NUM];
	u16				msg_no[MNGM_TALKWIN_IDX_NUM];
	STRBUF*			p_str[MNGM_TALKWIN_IDX_NUM];
	void*			p_timewait[MNGM_TALKWIN_IDX_NUM];
	BOOL			vip;
} MNGM_TALKWIN;

// プレートのプレイヤー人数に対応した配置位置データ
typedef struct {
	u8	top[WFLBY_MINIGAME_MAX];		// 表示ｙキャラクタ位置
	u8	scrn_y[WFLBY_MINIGAME_MAX];		// スクリーン読み込みYキャラクタ位置
	u8	oam_anm[WFLBY_MINIGAME_MAX];	// OAMのアニメ
} MNGM_PLAYER_PLATE_DATA;

// エントリープレイヤー名テーブルオブジェ
// そのうち動いたりするだろうから分けとく
typedef struct {
	GF_BGL_BMPWIN		win;
	CLACT_WORK_PTR		p_clwk;			// アクター
	CLACT_WORK_PTR		p_rank;			// 順位アクター
	u16					playernum;		// ユーザ数
	u8					idx;			// ユーザのテーブルインデックス
	u8					plidx;			// 自分から見たそのユーザのテーブルインデックス

	MNGM_ADDMOVE_WORK	draw_x;			// 描画ｘ位置
	s16					draw_x_count;	// 動作counter
	s16					draw_x_wait;	// 動作開始までのウエイト
	s32					draw_x_snd;		// 鳴らす音
} MNGM_PLATE_PLAYER;

// 通信プレイヤーデータ
typedef struct{
	u8	nation[ WFLBY_MINIGAME_MAX ];	// 国ID
	u8	area[ WFLBY_MINIGAME_MAX ];		// 地域ID
	const MYSTATUS* cp_status[ WFLBY_MINIGAME_MAX ];	// それぞれの人のデータ
} MNGM_COMM_PDATA;

// タイトルロゴ
typedef struct {
	GF_BGL_BMPWIN		bmp;
	u32					gametype;
	STRBUF*				p_str;

	MNGM_ADDMOVE_WORK	move;			// 動作ワーク
	s16					count;			// カウンタ
	s16					seq;			// シーケンス
} MNGM_TITLE_LOGO;


//-------------------------------------
///	HBLANKBGスクロールシステム
//=====================================
typedef struct {
	MNGM_BGL* p_bgl;
	s16 scrll_x[2][WFLBY_HBLANK_BGSCR_BUFNUM];	// buffer
	LASTER_SYS_PTR	p_laster;
} MNGM_HBLANK_PLATEBGSCR;


//-------------------------------------
///	エントリー画面ワーク
//=====================================

typedef struct _MNGM_ENTRYWK{
	u8						seq;							// シーケンス
	u8						wait;							// ウエイトカウンタ
	u16						heapID;							// ヒープ
	u16						gametype;						// ゲームタイプ
	u8						raregame_type;					// rareゲームタイプ
	u8						raregame_recv;					// rareゲームタイプを受信したか
	MNGM_ENRES_PARAM		comm_param;						// 共通パラメータ
	MNGM_COMM_PDATA			comm_pdata;						// プレイヤーデータ
	MNGM_BGL				bgl;							// BGシステム
	MNGM_MSG				msg;							// メッセージシステム
	MNGM_TALKWIN			talkwin;						// 会話ウィンドウ
	MNGM_CLACT				clact;							// セルアクターシステム
	MNGM_CLACTRES			clres;							// セルアクターリソース
	MNGM_CLACTRESOBJ*		p_resobj;						// リソースオブジェ
	MNGM_TITLE_LOGO			titlelogo;						// タイトルロゴ
	void*					p_scrnbuf;						// スクリーンバッファ
	NNSG2dScreenData*		p_scrn;							// スクリーンデータ
	MNGM_PLATE_PLAYER		playertbl[WFLBY_MINIGAME_MAX];	// プレイヤーテーブル
	MNGM_HBLANK_PLATEBGSCR	bgscrl;							// BGスクロールシステム
	TCB_PTR					tcb;							// タスク
	TCB_PTR					tcb_vwait;						// VBlankタスク
}MNGM_ENTRYWK;


//-------------------------------------
///	結果画面ワーク
//=====================================
//-------------------------------------
///	3Dシステムワーク
//=====================================
typedef struct {
	GF_G3DMAN*	p_3dman;
	PTC_PTR		p_ptc;
	void*		p_ptc_work;
} MNGM_3DSYS;

//-------------------------------------
///	リトライワーク
//=====================================
typedef struct {
	u8				seq;	// シーケンス
	u8				vchat;	// ボイスチャット
	u16				wait;
	TOUCH_SW_SYS*	p_ts;	// タッチシステム
	TOUCH_SW_PARAM	param;

	// リトライ親受信用
	// リトライ子受信用
	u8					recv_num;
	u8					recv_replay;
	u8					ko_recv;
	u8					ko_recv_replay;
} MNGM_RETRY_WK;

//-------------------------------------
///	風船アニメワーク
//=====================================
typedef struct {
//	GF_BGL_BMPWIN		win;
	void*				p_scrnbuff;
	NNSG2dScreenData*	p_scrn;
	CLACT_WORK_PTR		p_clwk[MNGM_RESULT_BALLOON_CLACT_NUM];
	MNGM_CLACTRESOBJ*	p_resobj;
	u8					seq;
	u8					count;
	u8					idx;
	u8					num;
} MNGM_BALLOON_WK;


typedef struct _MNGM_RESULTWK{
	u16						wait;							// ウエイトカウンタ
	u16						heapID;							// ヒープ
	u8						seq;							// シーケンス
	u8						draw_nation;					// 国名を表示するのか
	u8						end_flag;						// 終了フラグ
	u8						gametype;
	u8						replay;							// 再度遊ぶかフラグ
	u8						gadget_update;					// ガジェットアップデートフラグ
	u8						pad[2];
	MNGM_ENRES_PARAM		comm_param;						// 共通パラメータ
	MNGM_RESULT_PARAM		param;							// パラメータ
	MNGM_COMM_PDATA			comm_pdata;						// プレイヤーデータ
	MNGM_BGL				bgl;							// BGシステム
	MNGM_MSG				msg;							// メッセージシステム
	MNGM_TALKWIN			talkwin;						// 会話ウィンドウ
	MNGM_CLACT				clact;							// セルアクターシステム
	MNGM_CLACTRES			clres;							// セルアクターリソース
	MNGM_CLACTRESOBJ*		p_resobj;						// リソースオブジェ
	MNGM_RETRY_WK			retrysys;						// リトライシステム
	MNGM_BALLOON_WK			balloon;						// 風船ワーク
	void*					p_scrnbuf[MNGM_RESULT_SCRN_NUM];// スクリーンバッファ
	NNSG2dScreenData*		p_scrn[MNGM_RESULT_SCRN_NUM];	// スクリーンデータ
	MNGM_PLATE_PLAYER		playertbl[WFLBY_MINIGAME_MAX];	// プレイヤーテーブル
	MNGM_HBLANK_PLATEBGSCR	bgscrl;							// BGスクロールシステム
	TCB_PTR					tcb;							// タスク
	TCB_PTR					tcb_vwait;						// タスク
	WFLBY_ITEMTYPE			last_gadget;					// アップデート前のガジェット

	// 順位表示システム用
	u16						draw_result;					// 今表示する順位
	u16						draw_result_wait;				// 表示する順位までのウエイト

	// パレットエフェクト用
	void*					p_plttbuf;						// パレットバッファ
	NNSG2dPaletteData*		p_pltt;							// パレットデータ
	u16						pltt_time;
	u16						pltt_start;

	// 1位ME再生
	BOOL	top_me_play;	

}MNGM_RESULTWK;


//-------------------------------------
///	START	TIMEUP	ワーク
//=====================================
// OAMマスクシステム
typedef struct {
	CLACT_WORK_PTR	p_msk[MNGM_COUNT_MSK_OAM_NUM];
	s16				count;
	s16				move_y;
	u8				mskon[MNGM_COUNT_MSK_HBUF_NUM][192];
	TCB_PTR			p_tcb;
	GXWndPlane		off_outplane;
	GXWndPlane		on_outplane;
	int				def_wnd1;
} MNGM_COUNT_MSKWK;

typedef struct _MNGM_COUNTWK{
	u32						heapID;		// heap
	u16						seq;		// シーケンス
	s16						count;		// カウント
	CLACT_SET_PTR			p_clset;	// セルアクターセット
	ARCHANDLE*				p_handle;	// ハンドル
	MNGM_CLACTRES			resman;		// リソースマネージャ
	MNGM_CLACTRESOBJ*		p_resobj;	// リソースオブジェ
	TCB_PTR					p_tcb;		// 動作タスク

	CLACT_WORK_PTR			p_anm;		// アニメOAM
	MNGM_COUNT_MSKWK		msk;		// マスクシステム
}MNGM_COUNTWK;






//-----------------------------------------------------------------------------
/**
 *				データ関連
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	Rareゲームデータ
//=====================================
static const u8 sc_MNGM_RAREGAME_BUCKET_TBL[ MNGM_RAREGAME_BUCKET_NUM ] = {
	MNGM_RAREPAR_BUCKET_NORMAL, 
	MNGM_RAREPAR_BUCKET_REVERSE,
	MNGM_RAREPAR_BUCKET_BIG
};
static const u8 sc_MNGM_RAREGAME_BALANCEBALL_TBL[ MNGM_RAREGAME_BALANCEBALL_NUM ] = {
	MNGM_RAREPAR_BALANCEBALL_NORMAL, 
	MNGM_RAREPAR_BALANCEBALL_BIG,
};
static const u8 sc_MNGM_RAREGAME_BALLOON_TBL[ MNGM_RAREGAME_BALLOON_NUM ] = {
	MNGM_RAREPAR_BALLOON_NORMAL, 
	MNGM_RAREPAR_BALLOON_THICK,
	MNGM_RAREPAR_BALLOON_FINE,
};




//-------------------------------------
///	CLACTデータワーク
//=====================================
static const CHAR_MANAGER_MAKE sc_MNGM_CHARMAN_INIT = {
	0,
	16*1024,	// 16K
	16*1024,	// 16K
	0
};


//-------------------------------------
///	エントリー画面ワーク
//=====================================
// バンク設定
static const GF_BGL_DISPVRAM sc_MNGM_ENTRY_BANK = {
	GX_VRAM_BG_256_AB,				// メイン2DエンジンのBG
	GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット
	GX_VRAM_SUB_BG_32_H,			// サブ2DエンジンのBG
	GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット
	GX_VRAM_OBJ_16_G,				// メイン2DエンジンのOBJ
	GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット
	GX_VRAM_SUB_OBJ_16_I,			// サブ2DエンジンのOBJ
	GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット
	GX_VRAM_TEX_NONE,				// テクスチャイメージスロット
	GX_VRAM_TEXPLTT_NONE			// テクスチャパレットスロット
};
// BGL設定
static const GF_BGL_SYS_HEADER sc_MNGM_ENTRY_BGINIT = {
	GX_DISPMODE_GRAPHICS,
	GX_BGMODE_0,
	GX_BGMODE_0,
	GX_BG0_AS_2D
};
#define MNGM_ENTRY_BGL_USE	( 5 )
static const MNGM_BGL_DATA sc_MNGM_ENTRY_BGCNT[MNGM_ENTRY_BGL_USE] = {
	{	// 背景
		GF_BGL_FRAME3_M,
		{
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
			3, 0, 0, FALSE
		},
	},
	{	// テーブル&タイトルロゴメッセージ面
		GF_BGL_FRAME1_M,
		{
			0, 0, 0x2000, 0, GF_BGL_SCRSIZ_512x512, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xc800, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
			1, 0, 0, FALSE
		},
	},
	{
		GF_BGL_FRAME2_M,
		{
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xc000, GX_BG_CHARBASE_0x18000, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		},
	},
	{	// タイトルロゴ&テーブル用文字面
		GF_BGL_FRAME0_M,
		{
			0, 0, 0x2000, 0, GF_BGL_SCRSIZ_512x512, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xa000, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
			2, 0, 0, FALSE
		},
	},

	// サブ面
	{
		GF_BGL_FRAME0_S,
		{
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		},
	},
};


//-------------------------------------
///	共通
//=====================================
// PLATE表示位置データ
// 表示位置とアニメNOは自分の位置から時計回りにデータが配置されています。
//	1		2 1		 2
//					1 3
//	0		 0		 0
// 読み込み先はプレイヤーナンバーの昇順にデータが配置されています。
static const MNGM_PLAYER_PLATE_DATA sc_MNGM_PLAYER_PLATE_DATA[ WFLBY_MINIGAME_MAX ] = {
	{	// 参加1人
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0},
	},
	{	// 参加2人
		{14, 4, 0, 0},		// 表示位置は、ユーザのプレイヤIDで変化します。
		{18, 0, 0, 0},		// 読み込み先はユーザのプレイやIDで変化しません。
		{ 0, 3, 0, 0},		// アニメNOはユーザのプレイヤIDで変化します。
	},
	{	// 参加3人
		{14, 8, 2, 0},
		{18, 0, 6, 0},
		{ 0, 2, 1, 0},
	},
	{	// 参加4人
		{18, 12, 0,  6,},
		{18,  0, 6, 12,},
		{ 0,  2, 3,  1,},
	},
};
// プレイ人数分の玉いれ風千割用のテーブル参照インデックス
// このインデックスでsc_MNGM_PLAYER_PLATE_DATAの各テーブルを参照する
static const u8	sc_MNGM_PLAYER_PLATE_BSBLIDX_DATA[ WFLBY_MINIGAME_MAX ][ WFLBY_MINIGAME_MAX ] = {
	// 参加1人
	{ 0,0,0,0 },
	// 参加2人
	{ 0,1,0,0 },
	// 参加3人
	{ 0,2,1,0 },
	// 参加4人
	{ 0,3,1,2 },
};

#if 0
// プレイ人数分の玉乗り用のテーブル参照インデックス
// 自分は常に０番目の数字を見る
// その他はプレイヤーIDの昇順にこのテーブルを見る
// このインデックスでsc_MNGM_PLAYER_PLATE_DATAの各テーブルを参照する
static const u8	sc_MNGM_PLAYER_PLATE_BBIDX_DATA[ WFLBY_MINIGAME_MAX ][ WFLBY_MINIGAME_MAX ] = {
	// 参加1人
	{ 0,0,0,0 },
	// 参加2人
	{ 0,1,0,0 },
	// 参加3人
	{ 0,1,2,0 },
	// 参加4人
	{ 0,1,2,3 },
};
#endif



//-------------------------------------
///	リザルト画面ワーク
//=====================================
// バンク設定
static const GF_BGL_DISPVRAM sc_MNGM_RESULT_BANK = {
	GX_VRAM_BG_256_AB,				// メイン2DエンジンのBG
	GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット
	GX_VRAM_SUB_BG_32_H,			// サブ2DエンジンのBG
	GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット
	GX_VRAM_OBJ_16_G,				// メイン2DエンジンのOBJ
	GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット
	GX_VRAM_SUB_OBJ_16_I,			// サブ2DエンジンのOBJ
	GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット
	GX_VRAM_TEX_NONE,				// テクスチャイメージスロット
	GX_VRAM_TEXPLTT_NONE			// テクスチャパレットスロット
};
// BGL設定
static const GF_BGL_SYS_HEADER sc_MNGM_RESULT_BGINIT = {
	GX_DISPMODE_GRAPHICS,
	GX_BGMODE_0,
	GX_BGMODE_0,
	GX_BG0_AS_2D
};
#define MNGM_RESULT_BGL_USE	( 6 )
static const MNGM_BGL_DATA sc_MNGM_RESULT_BGCNT[MNGM_RESULT_BGL_USE] = {
	{	// 背景
		GF_BGL_FRAME3_M,
		{
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
			3, 0, 0, FALSE
		},
	},
	{	// テーブル
		GF_BGL_FRAME1_M,
		{
			0, 0, 0x1000, 0, GF_BGL_SCRSIZ_512x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xd800, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
			2, 0, 0, FALSE
		},
	},
	{	// メッセージ２面
		GF_BGL_FRAME2_M,
		{
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xd000, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		},
	},
	{	// テーブル用メッセージ
		GF_BGL_FRAME0_M,
		{
			0, 0, 0x1000, 0, GF_BGL_SCRSIZ_512x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xc000, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
			1, 0, 0, FALSE
		},
	},

	// サブ面
	{
		GF_BGL_FRAME0_S,
		{
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		},
	},
	{
		GF_BGL_FRAME1_S,
		{
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		},
	},
};






//-----------------------------------------------------------------------------
/**
 *				メッセージテーブル
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	ノーマル・VIP　メッセージ対応表 
//=====================================
typedef struct {
	u16 normal;
	u16 vip;
} MNGM_MSG_DATA;
static const MNGM_MSG_DATA sc_MNGM_MSG_DATA[] = {
	{ msg_00, msg_00 },
	{ msg_01, msg_01 },
	{ msg_02, msg_02_2 },
	{ msg_03, msg_03 },
	{ msg_04, msg_04 },
	{ msg_05, msg_05 },
	{ msg_06, msg_06 },
	{ msg_07, msg_07 },
	{ msg_08, msg_08 },
	{ msg_09, msg_09 },
	{ msg_10, msg_10_2 },
	{ msg_11, msg_11 },
	{ msg_12, msg_12 },
	{ msg_13, msg_13 },
	{ msg_14, msg_14 },
	{ msg_15, msg_15 },
	{ msg_16, msg_16 },
	{ msg_17, msg_17_2 },
	{ msg_18, msg_18_2 },
	{ msg_19, msg_19 },
};





//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	汎用システム
//=====================================
//	汎用加速動作
static void MNGM_AddMoveReqFx( MNGM_ADDMOVE_WORK* p_wk, fx32 s_x, fx32 e_x, fx32 s_s, int count_max );
static BOOL	MNGM_AddMoveMainFx( MNGM_ADDMOVE_WORK* p_wk, s32 count );

// スクリーンキャラクタオフセットを書き換える
static void MNGM_SCRN_AddCharOfs( NNSG2dScreenData* p_scrn, u32 char_offs );

// セルアクターリソース管理
static void MNGM_CLACTRES_Init( MNGM_CLACTRES* p_wk, u32 objnum, u32 heapID );
static void MNGM_CLACTRES_Exit( MNGM_CLACTRES* p_wk );
static MNGM_CLACTRESOBJ* MNGM_CLACTRES_Load( MNGM_CLACTRES* p_wk, ARCHANDLE* p_handle, u32 pal_idx, u32 palnum, u32 cg_idx, u32 cell_idx, u32 anm_idx, u32 contid, u32 heapID );
static CLACT_WORK_PTR MNGM_CLACTRES_Add( MNGM_CLACTRESOBJ* p_obj, CLACT_SET_PTR p_clset, s16 x, s16 y, u16 pri, u32 heapID );
static void MNGM_CLACTRES_OBJ_Init( MNGM_CLACTRES* p_wk, MNGM_CLACTRESOBJ* p_obj, ARCHANDLE* p_handle, u32 pal_idx, u32 palnum, u32 cg_idx, u32 cell_idx, u32 anm_idx, u32 contid, u32 heapID );
static void MNGM_CLACTRES_OBJ_Exit( MNGM_CLACTRES* p_wk, MNGM_CLACTRESOBJ* p_obj );

// BGL管理
static void MNGM_BGL_Init( MNGM_BGL* p_wk, const GF_BGL_SYS_HEADER* cp_sys, const MNGM_BGL_DATA* cp_cnt, u32 cnt_num, u32 heapID );
static void MNGM_BGL_Exit( MNGM_BGL* p_wk );
static void MNGM_BGL_VBlank( MNGM_BGL* p_wk );

// CLACT管理
static void MNGM_CLACT_Init( MNGM_CLACT* p_wk, u32 objnum, u32 charnum, u32 plttnum, u32 heapID );
static void MNGM_CLACT_Exit( MNGM_CLACT* p_wk );
static void MNGM_CLACT_Draw( MNGM_CLACT* p_wk );
static void MNGM_CLACT_VBlank( MNGM_CLACT* p_wk );

//  メッセージ管理
static void MNGM_MSG_Init( MNGM_MSG* p_wk, u32 heapID );
static void MNGM_MSG_Exit( MNGM_MSG* p_wk );
static void MNGM_MSG_ClearStrBuff( MNGM_MSG* p_wk );
static void MNGM_MSG_SetPlayerName( MNGM_MSG* p_wk, const MYSTATUS* cp_player );
static void MNGM_MSG_SetNationName( MNGM_MSG* p_wk, u32 nation );
static void MNGM_MSG_SetAreaName( MNGM_MSG* p_wk, u32 nation, u32 area );
static void MNGM_MSG_SetScore( MNGM_MSG* p_wk, u32 number, u32 keta );
static void MNGM_MSG_SetBalloonNum( MNGM_MSG* p_wk, u32 number );
static void MNGM_MSG_SetTime( MNGM_MSG* p_wk, u32 time );
static void MNGM_MSG_SetGadget( MNGM_MSG* p_wk, u32 gadget );
static void MNGM_MSG_SetGameName( MNGM_MSG* p_wk, u32 game );
static void MNGM_MSG_GetStr( MNGM_MSG* p_wk, STRBUF* p_str, u32 msgidx );
static void MNGM_MSG_Print( MNGM_MSG* p_wk, u32 no, GF_BGL_BMPWIN* p_win, u8 x, u8 y );
static void MNGM_MSG_PrintRightSide( MNGM_MSG* p_wk, u32 no, GF_BGL_BMPWIN* p_win, u8 x, u8 y );
static u32 MNGM_MSG_PrintScr( MNGM_MSG* p_wk, u32 no, GF_BGL_BMPWIN* p_win, STRBUF* p_str, u32 wait );
static void MNGM_MSG_PrintColor( MNGM_MSG* p_wk, u32 no, GF_BGL_BMPWIN* p_win, u8 x, u8 y, GF_PRINTCOLOR col );

// 会話ウィンドウ
static void MNGM_TALKWIN_Init( MNGM_TALKWIN* p_wk, MNGM_BGL* p_bgl, SAVEDATA* p_save, BOOL vip, u32 heapID );
static void MNGM_TALKWIN_Exit( MNGM_TALKWIN* p_wk );
static void MNGM_TALKWIN_MsgPrint( MNGM_TALKWIN* p_wk, MNGM_MSG* p_msg, u32 msgidx, u32 idx );
static void MNGM_TALKWIN_MsgOff( MNGM_TALKWIN* p_wk, u32 idx );
static BOOL MNGM_TALKWIN_MsgEndCheck( const MNGM_TALKWIN* p_wk, u32 idx );
static void MNGM_TALKWIN_SetPos( MNGM_TALKWIN* p_wk, u8 x, u8 y, u32 idx );
static void MNGM_TALKWIN_SetTimeWork( MNGM_TALKWIN* p_wk, u32 idx );
static void MNGM_TALKWIN_CleanTimeWork( MNGM_TALKWIN* p_wk, u32 idx );

// プレイヤーPLATE
static void MNGM_PLATE_PLAYERTBL_Init( MNGM_PLATE_PLAYER* p_player, u32 player_num, u32 gametype, NNSG2dScreenData* p_scrn, MNGM_BGL* p_bgl, u32 idx, u32 myplayerid, MNGM_CLACT* p_clact, MNGM_CLACTRESOBJ* p_oamres, u32 heapID );
static void MNGM_PLATE_PLAYERTBL_DrawName( MNGM_PLATE_PLAYER* p_player, MNGM_MSG* p_msg, const MYSTATUS* cp_status, s16 x, s16 y, u32 vip );
static void MNGM_PLATE_PLAYERTBL_DrawNation( MNGM_PLATE_PLAYER* p_player, MNGM_MSG* p_msg, BOOL draw_nation, u32 nation, u32 area, s16 x );
static void MNGM_PLATE_PLAYERTBL_DrawRank( MNGM_PLATE_PLAYER* p_player, u32 rank );
static void MNGM_PLATE_PLAYERTBL_DrawScore( MNGM_PLATE_PLAYER* p_player, MNGM_MSG* p_msg, u32 score, s16 x, s16 y, u32 keta );
static void MNGM_PLATE_PLAYERTBL_DrawTime( MNGM_PLATE_PLAYER* p_player, MNGM_MSG* p_msg, u32 frame, s16 x, s16 y );
static void MNGM_PLATE_PLAYERTBL_BgWriteVReq( MNGM_PLATE_PLAYER* p_player, MNGM_BGL* p_bgl );
static void MNGM_PLATE_PLAYERTBL_DrawOamWay( MNGM_PLATE_PLAYER* p_player );
static void MNGM_PLATE_PLAYERTBL_Delete( MNGM_PLATE_PLAYER* p_player );
static void MNGM_PLATE_PLAYERTBL_StartMove( MNGM_PLATE_PLAYER* p_player, s16 end_x, s16 wait );
static void MNGM_PLATE_PLAYERTBL_StartMoveRet( MNGM_PLATE_PLAYER* p_player, s16 wait );
static BOOL MNGM_PLATE_PLAYERTBL_MainMove( MNGM_PLATE_PLAYER* p_player );
static void MNGM_PLATE_PLAYERTBL_SetOamMatrix( MNGM_PLATE_PLAYER* p_wk, s32 draw_x );

// プレイヤーPLATE
static u32 MNGM_PLAYER_PLATE_BSBLIDX_Get( u32 playernum, u32 playerid, u32 myplayerid );


// HBLANKBGスクロールシステム
static void MNGM_PLAYER_PLATE_HBLANK_BGSCR_Init( MNGM_HBLANK_PLATEBGSCR* p_wk, MNGM_BGL* p_bgl, u32 heapID );
static void MNGM_PLAYER_PLATE_HBLANK_BGSCR_Exit( MNGM_HBLANK_PLATEBGSCR* p_wk );
static void MNGM_PLAYER_PLATE_HBLANK_BGSCR_SetPlateScr( MNGM_HBLANK_PLATEBGSCR* p_wk, const MNGM_PLATE_PLAYER* cp_plate );
static void MNGM_PLAYER_PLATE_HBLANK_BGSCR_VBlank( MNGM_HBLANK_PLATEBGSCR* p_wk );
static void MNGM_PLAYER_PLATE_HBLANK_BGSCR_HBlank( void* p_work );


// プレイヤー通信データ
static void MNGM_COMMPDATA_Init( MNGM_COMM_PDATA* p_wk, const MNGM_ENRES_PARAM* cp_comm_param );

// 国名を表示するのか、地域名を表示するのかのチェック
static BOOL MNGM_COMMPARAM_CheckDrawNation( const MNGM_ENRES_PARAM* cp_wk, const MNGM_COMM_PDATA* cp_data );

// ガジェットのアップデート
extern WFLBY_ITEMTYPE MNGM_ITEM_Update( WFLBY_ITEMTYPE data );

// タイトルロゴシステム
static void MNGM_TITLELOGO_Init( MNGM_TITLE_LOGO* p_wk, MNGM_BGL* p_bglwk, MNGM_MSG* p_msg, u32 gametype, ARCHANDLE* p_handle, u32 heapID );
static void MNGM_TITLELOGO_Exit( MNGM_TITLE_LOGO* p_wk );
static void MNGM_TITLELOGO_InStart( MNGM_TITLE_LOGO* p_wk, MNGM_BGL* p_bglwk, u32 heapID );
static void MNGM_TITLELOGO_OutStart( MNGM_TITLE_LOGO* p_wk, MNGM_BGL* p_bglwk );
static BOOL MNGM_TITLELOGO_InMain( MNGM_TITLE_LOGO* p_wk, MNGM_BGL* p_bglwk );
static BOOL MNGM_TITLELOGO_OutMain( MNGM_TITLE_LOGO* p_wk, MNGM_BGL* p_bglwk );
static void MNGM_TITLELOGO_SetMoveMatrix( MNGM_TITLE_LOGO* p_wk, MNGM_BGL* p_bglwk, s32 num );
static void MNGM_TITLELOGO_SetMoveMatrixVReq( MNGM_TITLE_LOGO* p_wk, MNGM_BGL* p_bglwk, s32 num );


// 背景パレット転送
static void MNGM_BACKPLTT_Trans( ARCHANDLE* p_handle, u32 gametype, u32 heapID );

//-------------------------------------
///	エントリー画面ワーク
//=====================================
static MNGM_ENTRYWK* MNGM_ENTRY_CommonInit( const MNGM_ENRES_PARAM* cp_commparam, u32 gametype, u32 heapID );
static void MNGM_ENTRY_GraphicLoad( MNGM_ENTRYWK* p_wk, u32 heapID );
static void MNGM_ENTRY_GraphicDelete( MNGM_ENTRYWK* p_wk );
static void MNGM_ENTRY_Tcb( TCB_PTR tcb, void* p_work );
static void MNGM_ENTRY_VWaitTcb( TCB_PTR tcb, void* p_work );
static MNGM_RAREGAME_TYPE MNGM_ENTRY_RareGameSelect( const u8* cp_ParTbl, u32 num );
static BOOL MNGM_ENTRY_PLATE_ALLMove( MNGM_ENTRYWK* p_wk );


//-------------------------------------
///	リザルト画面ワーク
//=====================================
static MNGM_RESULTWK* MNGM_RESULT_CommonInit( const MNGM_ENRES_PARAM* cp_commparam, const MNGM_RESULT_PARAM* cp_param, u32 gametype, u32 heapID );
static void MNGM_RESULT_3DInit( MNGM_3DSYS* p_wk, u32 heapID );
static void MNGM_RESULT_3DExit( MNGM_3DSYS* p_wk );
static void MNGM_RESULT_3DMain( MNGM_3DSYS* p_wk );
static void MNGM_RESULT_3DDraw( MNGM_3DSYS* p_wk );
static void MNGM_RESULT_3DAnmLoad( MNGM_3DSYS* p_wk, ARCHANDLE* p_handle, u32 dataidx, u32 heapID );
static void MNGM_RESULT_3DAnmRelease( MNGM_3DSYS* p_wk );
static void MNGM_RESULT_3DAnmStart( MNGM_3DSYS* p_wk, u32 emitnum );
static BOOL MNGM_RESULT_3DAnmEndCheck( const MNGM_3DSYS* cp_wk );
static void MNGM_RESULT_GraphicLoad( MNGM_RESULTWK* p_wk, u32 heapID );
static void MNGM_RESULT_GraphicDelete( MNGM_RESULTWK* p_wk );
static void MNGM_RESULT_Tcb_BallSlowBalanceBall( TCB_PTR tcb, void* p_work );
static void MNGM_RESULT_Tcb_Balloon( TCB_PTR tcb, void* p_work );
static void MNGM_RESULT_VWaitTcb( TCB_PTR tcb, void* p_work );
static void MNGM_RESULT_PalTrEffectInit( MNGM_RESULTWK* p_wk, ARCHANDLE* p_handle, u32 heapID );
static void MNGM_RESULT_PalTrEffectStart( MNGM_RESULTWK* p_wk );
static void MNGM_RESULT_PalTrEffect( MNGM_RESULTWK* p_wk );
static void MNGM_RESULT_PalTrEffectExit( MNGM_RESULTWK* p_wk );
static void MNGM_RESULT_Retry_Init( MNGM_RETRY_WK* p_wk, MNGM_BGL* p_bgl, u32 vchat, u32 heapID );
static void MNGM_RESULT_Retry_Exit( MNGM_RETRY_WK* p_wk );
static BOOL MNGM_RESULT_Retry_Main( MNGM_RETRY_WK* p_wk, MNGM_TALKWIN* p_talkwin, MNGM_MSG* p_msg, u32 gametype, u32 heapID );
static BOOL MNGM_RESULT_RetryGet( const MNGM_RETRY_WK* cp_wk );
static void MNGM_RESULT_RetryOyaRecv( MNGM_RETRY_WK* p_wk, u32 netid, BOOL replay, u32 playernum );
static void MNGM_RESULT_RetryKoRecv( MNGM_RETRY_WK* p_wk, BOOL replay );
static void MNGM_RESULT_Balloon_GraphicInit( MNGM_BALLOON_WK* p_wk, MNGM_BGL* p_bgl, MNGM_CLACT* p_clact, MNGM_CLACTRES* p_clres, ARCHANDLE* p_handle, u32 heapID );
static void MNGM_RESULT_Balloon_GraphicExit( MNGM_BALLOON_WK* p_wk );
static void MNGM_RESULT_Balloon_Start( MNGM_BALLOON_WK* p_wk, u32 num );
static BOOL MNGM_RESULT_Balloon_Main( MNGM_BALLOON_WK* p_wk, MNGM_BGL* p_bgl );

static void MNGM_RESULT_BallslowBalancePlayerTblDraw( MNGM_RESULTWK* p_wk, u32 plidx, u32 rank );
static u32 MNGM_RESULT_BallslowBalanceGetNoTouchScore( const MNGM_RESULTWK* cp_wk );
static void MNGM_RESULT_BallslowBalance_SendMinigameTopResult( MNGM_RESULTWK* p_wk );
static void MNGM_RESULT_Balloon_SendMinigameTopResult( MNGM_RESULTWK* p_wk );

static BOOL MNGM_RESULT_PlayerTblMove( MNGM_RESULTWK* p_wk );

static void MNGM_RESULT_SndTopMePlay( MNGM_RESULTWK* p_wk );
static BOOL MNGM_RESULT_SndTopMeEndWait( MNGM_RESULTWK* p_wk );
static void MNGM_RESULT_SetPlayNum( MNGM_RESULTWK* p_wk );




//-------------------------------------
///	START	TIMEUP	ワーク
//=====================================
static void MNGM_COUNT_MskInit( MNGM_COUNT_MSKWK* p_wk, MNGM_CLACTRESOBJ* p_resobj, CLACT_SET_PTR p_clset, u32 heapID );
static BOOL MNGM_COUNT_MskMain( MNGM_COUNT_MSKWK* p_wk );
static BOOL MNGM_COUNT_MskReMain( MNGM_COUNT_MSKWK* p_wk );
static void MNGM_COUNT_MskCommon( MNGM_COUNT_MSKWK* p_wk, s32 add );
static void MNGM_COUNT_MskExit( MNGM_COUNT_MSKWK* p_wk );
static void MNGM_COUNT_MskSetMsk( MNGM_COUNT_MSKWK* p_wk );
static void MNGM_COUNT_MskSetMskVBlank( TCB_PTR tcb, void* p_work );
static void MNGM_COUNT_MskSetMskHBlank( void* p_work );

static void MNGM_COUNT_StartTcb( TCB_PTR tcb, void* p_work );
static void MNGM_COUNT_TimeUpTcb( TCB_PTR tcb, void* p_work );



//-------------------------------------
///	エントリー画面、結果画面共通パラメータツール
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	エントリー画面、結果画面共通パラメータ作成
 *
 *	@param	p_param		パラメータ格納先
 *	@param	lobby_flag	ロビーフラグ
 *	@param	p_save		セーブデータ
 *	@param	vchat		ボイスチャット
 *	@param	p_lobby_wk	ロビーワーク
 */
//-----------------------------------------------------------------------------
void MNGM_ENRES_PARAM_Init( MNGM_ENRES_PARAM* p_param, BOOL lobby_flag, SAVEDATA* p_save, BOOL vchat, WFLBY_MINIGAME_WK* p_lobby_wk )
{
	int i;
	int count;
	u32 netid;
	MYSTATUS* p_status;

	memset( p_param,  0, sizeof(MNGM_ENRES_PARAM) );
	
	p_param->num = CommInfoGetEntryNum();
	netid = CommGetCurrentID();

	// PLNO順のNETIDテーブルを作成
	count = 0;
	for( i=0; i<WFLBY_MINIGAME_MAX; i++ ){
		p_status = CommInfoGetMyStatus( i );
		if( p_status != NULL ){
			
			// 自分のNETIDならPLNOを保存
			if( netid == i ){
				p_param->my_playerid = count;
			}
			
			// plnoのところにNETIDを保存
			p_param->pnetid[count] = i;
			count ++;
		}
	}
	// CommGetConnectNumの戻り値と実際のMYSTATUSの受信数に矛盾がある
	GF_ASSERT( count == p_param->num );

	// replay質問処理について
	if( lobby_flag == FALSE ){
		p_param->replay = TRUE;
	}

	p_param->p_save	= p_save;
	p_param->vchat	= vchat;

	// VIPフラグをコピー
	p_param->p_lobby_wk = p_lobby_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	NETIDからPLNOを取得する
 *
 *	@param	cp_param	エントリー画面、結果画面共通パラメータ
 *	@param	netid		ネットID
 *
 *	@retval	PLNO	（プレイヤーナンバー）
 *	@retval	当てはまらないNETIDならWFLBY_MINIGAME_MAXを返す
 */
//-----------------------------------------------------------------------------
u32 MNGM_ENRES_PARAM_GetNetIDtoPlNO( const MNGM_ENRES_PARAM* cp_param, u32 netid )
{
	int i;

	for( i=0; i<cp_param->num; i++ ){
		if( cp_param->pnetid[i] == netid ){
			return i;
		}
	}
	return WFLBY_MINIGAME_MAX;
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーNOからNETIDを取得する
 *
 *	@param	cp_param	エントリー画面、結果画面共通パラメータ
 *	@param	plno		プレイヤーナンバー
 *
 *	@return	NETID
 */
//-----------------------------------------------------------------------------
u32 MNGM_ENRES_PARAM_GetPlNOtoNetID( const MNGM_ENRES_PARAM* cp_param, u32 plno )
{
	GF_ASSERT( plno < cp_param->num );

	return cp_param->pnetid[ plno ];
}


//----------------------------------------------------------------------------
/**
 *	@brief	そのNETIDの人がVIPかどうかチェックする
 *
 *	@param	cp_param		パラメータ
 *	@param	netid			ネットID
 *
 *	@retval	TRUE	VIP
 *	@retval	FALSE	VIPじゃない
 *
 *	VIPの時は、その人の名前を青色にしてください
 */
//-----------------------------------------------------------------------------
BOOL MNGM_ENRES_PARAM_GetVipFlag( const MNGM_ENRES_PARAM* cp_param, u32 netid )
{
	u32 plidx;
	BOOL vip;
	
	if( netid >= WFLBY_MINIGAME_MAX ){
		GF_ASSERT( netid < WFLBY_MINIGAME_MAX );
		return FALSE;
	}
	
	plidx = cp_param->p_lobby_wk->plidx.plidx[ netid ];
	if( plidx == DWC_LOBBY_USERIDTBL_IDX_NONE ){
		return FALSE;
	}
	
	vip =  cp_param->p_lobby_wk->vipflag.vip[ plidx ];

//	OS_TPrintf( "plidx %d,  vip %d\n", plidx, vip );
	return vip;
}

//----------------------------------------------------------------------------
/**
 *	@brief	MYステータスを取得
 *
 *	@param	cp_param		パラメータ
 *	@param	netid			ネットID
 *
 *	@retval	MYステータス
 *	@retval	NULL			そのひといない
*/
//-----------------------------------------------------------------------------
MYSTATUS* MNGM_ENRES_PARAM_GetMystatus( const MNGM_ENRES_PARAM* cp_param, u32 netid )
{
	u32 plno;

	if( netid >= WFLBY_MINIGAME_MAX ){
		GF_ASSERT( netid < WFLBY_MINIGAME_MAX );
		return NULL;
	}

	// Wi-Fiクラブのとき
	if( cp_param->replay == TRUE ){
		plno		= MNGM_ENRES_PARAM_GetNetIDtoPlNO( cp_param, netid );

		// 不正NAMEで名前が置換されている可能性があるので
		// 自分の名前はセーブデータからとってくる
		if( plno == cp_param->my_playerid ){
			return SaveData_GetMyStatus( cp_param->p_save );
		}
		return CommInfoGetMyStatus( netid );
	}

	// Wi-Fi広場のとき
	GF_ASSERT( cp_param->p_lobby_wk != NULL );
	return cp_param->p_lobby_wk->p_mystate[netid];
}

 

//-------------------------------------
///	エントリー画面ワーク
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	エントリーワーク作成
 *
 *	@param	cp_param		パラメータ
 *	@param	heapID 
 *
 *	@return	エントリーワーク
 */
//-----------------------------------------------------------------------------
// 玉投げ
MNGM_ENTRYWK* MNGM_ENTRY_InitBallSlow( const MNGM_ENRES_PARAM* cp_commparam, u32 heapID )
{
	return MNGM_ENTRY_CommonInit( cp_commparam, WFLBY_GAME_BALLSLOW, heapID );
}

// Balanceボール
MNGM_ENTRYWK* MNGM_ENTRY_InitBalanceBall( const MNGM_ENRES_PARAM* cp_commparam, u32 heapID )
{
	return MNGM_ENTRY_CommonInit( cp_commparam, WFLBY_GAME_BALANCEBALL, heapID );
}

// ふうせんわり
MNGM_ENTRYWK* MNGM_ENTRY_InitBalloon( const MNGM_ENRES_PARAM* cp_commparam, u32 heapID )
{
	return MNGM_ENTRY_CommonInit( cp_commparam, WFLBY_GAME_BALLOON, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	エントリー画面破棄
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void MNGM_ENTRY_Exit( MNGM_ENTRYWK* p_wk )
{
	
	// TCB破棄
	TCB_Delete( p_wk->tcb );
	TCB_Delete( p_wk->tcb_vwait );

	// ラスターシステム破棄
	MNGM_PLAYER_PLATE_HBLANK_BGSCR_Exit( &p_wk->bgscrl );

	// タイトルロゴワーク破棄
	MNGM_TITLELOGO_Exit( &p_wk->titlelogo );

	
	// プレイヤー分のデータ破棄
	{
		int i;
		for( i=0; i<p_wk->comm_param.num; i++ ){
			MNGM_PLATE_PLAYERTBL_Delete( &p_wk->playertbl[i] );
		}
	}

	// メッセージシステム破棄
	MNGM_TALKWIN_Exit( &p_wk->talkwin );
	
	// グラフィック破棄
	MNGM_ENTRY_GraphicDelete( p_wk );

	// メッセージシステム破棄
	MNGM_MSG_Exit( &p_wk->msg );	

	// グラフィック設定解除
	MNGM_CLACTRES_Exit( &p_wk->clres );
	MNGM_CLACT_Exit( &p_wk->clact );
	MNGM_BGL_Exit( &p_wk->bgl );

	// ワーク破棄
	sys_FreeMemoryEz( p_wk );	
}

//----------------------------------------------------------------------------
/**
 *	@brief	エントリー画面終了待ち
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
BOOL MNGM_ENTRY_Wait( const MNGM_ENTRYWK* cp_wk )
{
	if( cp_wk->seq >= MNGM_ENTRY_SEQ_END ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	rareゲームタイプの取得
 *
 *	@param	cp_wk	ワーク
 *
 *	@return	rareゲームタイプ
 */
//-----------------------------------------------------------------------------
MNGM_RAREGAME_TYPE MNGM_ENTRY_GetRareGame( const MNGM_ENTRYWK* cp_wk )
{
	GF_ASSERT( cp_wk->raregame_recv == TRUE );
	return cp_wk->raregame_type;
}


//-------------------------------------
///	結果画面ワーク
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	結果画面ワーク	作成
 *
 *	@return	ワーク
 */
//-----------------------------------------------------------------------------
// ボール投げ
MNGM_RESULTWK* MNGM_RESULT_InitBallSlow( const MNGM_ENRES_PARAM* cp_commparam, const MNGM_RESULT_PARAM* cp_param, u32 heapID )
{
	return MNGM_RESULT_CommonInit( cp_commparam, cp_param, WFLBY_GAME_BALLSLOW, heapID );
}

// Balanceボール
MNGM_RESULTWK* MNGM_RESULT_InitBalanceBall( const MNGM_ENRES_PARAM* cp_commparam, const MNGM_RESULT_PARAM* cp_param, u32 heapID )
{
	return MNGM_RESULT_CommonInit( cp_commparam, cp_param, WFLBY_GAME_BALANCEBALL, heapID );
}

// 風千割
MNGM_RESULTWK* MNGM_RESULT_InitBalloon( const MNGM_ENRES_PARAM* cp_commparam, const MNGM_RESULT_PARAM* cp_param, u32 heapID )
{
	return MNGM_RESULT_CommonInit( cp_commparam, cp_param, WFLBY_GAME_BALLOON, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	結果画面破棄
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
void MNGM_RESULT_Exit( MNGM_RESULTWK* p_wk )
{
	

	// TCB破棄
	TCB_Delete( p_wk->tcb );
	TCB_Delete( p_wk->tcb_vwait );

	// ラスターシステム破棄
	MNGM_PLAYER_PLATE_HBLANK_BGSCR_Exit( &p_wk->bgscrl );
	
	// プレイヤー分のデータ破棄
	{
		int i;
		for( i=0; i<p_wk->comm_param.num; i++ ){
			MNGM_PLATE_PLAYERTBL_Delete( &p_wk->playertbl[i] );
		}
	}

	// リトライワークの破棄
	MNGM_RESULT_Retry_Exit( &p_wk->retrysys );
	
	// グラフィック破棄
	MNGM_RESULT_GraphicDelete( p_wk );

	// メッセージシステム破棄
	MNGM_TALKWIN_Exit( &p_wk->talkwin );

	// メッセージシステム破棄
	MNGM_MSG_Exit( &p_wk->msg );	

	// グラフィック設定解除
	MNGM_CLACTRES_Exit( &p_wk->clres );
	MNGM_CLACT_Exit( &p_wk->clact );
	MNGM_BGL_Exit( &p_wk->bgl );

	//  アルファOFF
	G2_BlendNone();
	G2S_BlendNone();

	//
	DellVramTransferManager();


	// ワーク破棄
	sys_FreeMemoryEz( p_wk );	
}

//----------------------------------------------------------------------------
/**
 *	@brief	結果画面	処理終了チェック関数
 *
 *	@param	cp_wk		ワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
BOOL MNGM_RESULT_Wait( const MNGM_RESULTWK* cp_wk )
{
	return cp_wk->end_flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief	再度遊ぶのかチェック
 *
 *	@param	cp_wk		ワーク
 *
 *	@retval	TRUE	再度遊ぶ
 *	@retval	FALSE	もう遊ばない
 */
//-----------------------------------------------------------------------------
BOOL MNGM_RESULT_GetReplay( const MNGM_RESULTWK* cp_wk )
{
	return cp_wk->replay;
}


// 結果画面パラメータ生成ツール
//----------------------------------------------------------------------------
/**
 *	@brief	結果画面パラメータRank生成ツール
 *
 *	@param	p_param		パラメータ
 *	@param	plnum		プレイヤー数
 */
//-----------------------------------------------------------------------------
void MNGM_RESULT_CalcRank( MNGM_RESULT_PARAM* p_param, u32 plnum )
{
	u8 rankplnotbl[ WFLBY_MINIGAME_MAX ];	// 順位の順にPLNOを格納する
	int i, j;
	
	for( i=0; i<plnum; i++ ){
		
		// 自分のデータ格納先を求める
		for( j=i; j>0; j-- ){

			// jの位置のPLNOのスコアと自分のスコアを比較し
			// 多かったらその次のPLNOのスコアと自分のスコアを比較する
			if( p_param->score[ i ] <= p_param->score[ rankplnotbl[ j-1 ] ] ){
				break;
			}else{
				rankplnotbl[ j ] = rankplnotbl[ j-1 ];
			}
		}

		// jの位置に自分のPLNOを入れる
		rankplnotbl[ j ] = i;
	}

	// ためしに並びを表示
	{
		for( i=0; i<plnum; i++ ){
			OS_TPrintf( "%d	", p_param->score[ rankplnotbl[ i ] ] );
		}
		OS_TPrintf( "\n" );
	}

	// スコアの多き順に並んだPLNOTBLを使用して順位を格納していく
	{
		u32 rank;
		for( i=0; i<plnum; i++ ){
			rank = i;	// 基本の順位で初期化
			
			if( i>0 ){
				// 前の人と同じスコアなら同じ順位
				if( p_param->score[ rankplnotbl[ i ] ] == p_param->score[ rankplnotbl[ i-1 ] ] ){
					rank = p_param->result[ rankplnotbl[ i-1 ] ];
				}
			}

			p_param->result[ rankplnotbl[ i ] ] = rank;
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	Rareゲームタイプを設定
 *
 *	@param	p_wk		ワーク
 *	@param	type		タイプ
 */
//-----------------------------------------------------------------------------
void MNGM_ENTRY_SetRareGame( MNGM_ENTRYWK* p_wk,  MNGM_RAREGAME_TYPE type )
{
	p_wk->raregame_type = type;
	p_wk->raregame_recv	= TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	子からきた続けるかデータを受信
 *
 *	@param	p_wk		ワーク
 *	@param	netid		ネットID
 *	@param	retry		replayかどうか
 */
//-----------------------------------------------------------------------------
void MNGM_RESULT_SetKoRetry( MNGM_RESULTWK* p_wk, u32 netid,  BOOL retry )
{
	if( p_wk->comm_param.my_playerid == 0 ){
		MNGM_RESULT_RetryOyaRecv( &p_wk->retrysys, netid, retry, p_wk->comm_param.num );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	親から続けるかデータを受信
 *
 *	@param	p_wk		ワーク
 *	@param	retry		リトライ
 */
//-----------------------------------------------------------------------------
void MNGM_RESULT_SetOyaRetry( MNGM_RESULTWK* p_wk, BOOL retry )
{
	MNGM_RESULT_RetryKoRecv( &p_wk->retrysys, retry );
}



//-------------------------------------
///	切断エラーチェック
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	切断エラーチェック
 *
 *	@retval	TRUE	切断エラーがあった
 *	@retval	FALSE	切断エラーなし
 */
//-----------------------------------------------------------------------------
BOOL MNGM_ERROR_CheckDisconnect( MNGM_ENRES_PARAM* p_commparam )
{
	BOOL ret = FALSE;
	
	if( p_commparam->p_lobby_wk != NULL ){
		
#ifdef DEBUG_MINIGAME_DISCONNECT
		if( sys.cont & PAD_BUTTON_A ){
			ret = TRUE;
		}
#endif

		// 通信人数が変わったらエラー
		if( p_commparam->num != CommGetConnectNum() ){
			ret = TRUE;
		}

		//  切断エラー
		if( CommStateIsWifiDisconnect() == TRUE ){
			ret = TRUE;
		}

		// マッチングエラー
		if( CommWifiIsMatched() >= 2 ){
			ret = TRUE;
		}

		// 
		if( ret == TRUE ){
			p_commparam->p_lobby_wk->error_end = TRUE;
		}

		//  エラー終了状態ならTRUEを返し続ける
		if( p_commparam->p_lobby_wk->error_end == TRUE ){
			ret = TRUE;
		}
	}
	return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	切断処理
 *
 *	@param	p_commparam		ワーク
 *
 *	@retval	TRUE	切断完了
 *	@retval	FALSE	切断中
 */
//-----------------------------------------------------------------------------
BOOL MNGM_ERROR_DisconnectWait( const MNGM_ENRES_PARAM* cp_commparam )
{
	GF_ASSERT( cp_commparam->p_lobby_wk != NULL );
	GF_ASSERT( cp_commparam->p_lobby_wk->error_end == TRUE );
	
	if( CommInfoIsInitialize() == TRUE ){

		// 080624	戻るときにコマンドが送られてくる可能性があるので、
		// ここで完全に切断させる
		// 通信を終了させる
		// Info終了
		CommInfoFinalize();

		// 通信切断
		CommStateWifiP2PEnd();
	}else{

		// 切断完了
		if( CommStateIsWifiLoginMatchState() == TRUE ){
			return TRUE;
		}
	}

	return FALSE;
}



//-------------------------------------
///	START	TIMEUP	ワーク
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	OAMエフェクトワーク	を作成する
 *
 *	@param	p_clset		セルアクターセット
 *	@param	heapID		ヒープID
 *
 *	@return	ワーク
 */
//-----------------------------------------------------------------------------
MNGM_COUNTWK* MNGM_COUNT_Init( CLACT_SET_PTR p_clset, u32 heapID )
{
	MNGM_COUNTWK* p_wk;

	p_wk = sys_AllocMemory( heapID, sizeof( MNGM_COUNTWK ) );
	memset( p_wk, 0, sizeof( MNGM_COUNTWK ) );

	p_wk->p_clset = p_clset;
	
	// ハンドルオープン
	p_wk->p_handle = ArchiveDataHandleOpen( ARC_WLMNGM_TOOL_GRA, heapID );

	// リソースマネージャ作成
	MNGM_CLACTRES_Init( &p_wk->resman, MNGM_COUNT_OAMRESNUM, heapID );

	// リソース読み込み
	p_wk->p_resobj = MNGM_CLACTRES_Load( &p_wk->resman,
			p_wk->p_handle, 
			NARC_wlmngm_tool_font_boad_NCLR, MNGM_COUNT_PALNUM,
			NARC_wlmngm_tool_font_boad_NCGR,
			NARC_wlmngm_tool_font_boad_NCER,
			NARC_wlmngm_tool_font_boad_NANR,
			MNGM_RESCONTID,
			heapID );


	return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	OAMワーク	破棄
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
void MNGM_COUNT_Exit( MNGM_COUNTWK* p_wk )
{
	// タスク破棄
	if( p_wk->p_tcb != NULL ){
		// 動作破棄
		TCB_Delete( p_wk->p_tcb );
		p_wk->p_tcb = NULL;
	}
	
	// アクター破棄
	if( p_wk->p_anm != NULL ){
		CLACT_Delete( p_wk->p_anm );
	}

	// マスクシステム破棄
	MNGM_COUNT_MskExit( &p_wk->msk );

	// リソース破棄
	MNGM_CLACTRES_Exit( &p_wk->resman );

	// ハンドル破棄
	ArchiveDataHandleClose( p_wk->p_handle );

	// ワーク破棄
	sys_FreeMemoryEz( p_wk );
}

//--------------------------------------------------------------
/**
 * @brief   パレット展開位置を取得
 *
 * @param   p_wk		ワーク
 * @param   p_clset		セルアクターセット
 *
 * @retval  パレット位置
 */
//--------------------------------------------------------------
int MNGM_PalNoGet( MNGM_COUNTWK* p_wk )
{
	CLACT_U_RES_OBJ_PTR res 
		= CLACT_U_ResManagerGetIDResObjPtr( p_wk->resman.p_resman[1], MNGM_RESCONTID );
	return CLACT_U_PlttManagerGetPlttNo(res, NNS_G2D_VRAM_TYPE_2DMAIN );
}

//----------------------------------------------------------------------------
/**
 *	@brief	Redy	Start	開始
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void MNGM_COUNT_StartStart( MNGM_COUNTWK* p_wk )
{
	GF_ASSERT( p_wk->p_tcb == NULL );
	p_wk->p_tcb = TCB_Add( MNGM_COUNT_StartTcb, p_wk, 0 );
	p_wk->seq = MNGM_COUNT_START_SEQ_INIT;
}
void MNGM_COUNT_StartTimeUp( MNGM_COUNTWK* p_wk )
{
	GF_ASSERT( p_wk->p_tcb == NULL );
	p_wk->p_tcb = TCB_Add( MNGM_COUNT_TimeUpTcb, p_wk, 0 );
	p_wk->seq = MNGM_COUNT_TIMEUP_SEQ_INIT;

	Snd_SePlay( MNGM_SND_TIMEUP ); 
}

//----------------------------------------------------------------------------
/**
 *	@brief	エフェクトが完了したかチェックMNGM_ENTRY_SEQ_END
 *
 *	@param	p_wk	ワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
BOOL MNGM_COUNT_Wait( const MNGM_COUNTWK* p_wk )
{
	if( p_wk->seq == 0 ){
		return TRUE;
	}
	return FALSE;
}





//-----------------------------------------------------------------------------
/**
 *				プライベート関数
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	汎用システム
//=====================================
//	汎用加速動作
//----------------------------------------------------------------------------
/**
 *	@brief	加速移動計算ワーク	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	s_x			開始位置
 *	@param	e_x			終了位置
 *	@param	s_s			初速度
 *	@param	count_max	カウント最大値
 */
//-----------------------------------------------------------------------------
static void MNGM_AddMoveReqFx( MNGM_ADDMOVE_WORK* p_wk, fx32 s_x, fx32 e_x, fx32 s_s, int count_max )
{
	fx32 t_x_t;	// タイムの２乗
	fx32 vot;	// 初速度＊タイム
	fx32 dis;
	fx32 a;

	dis = e_x - s_x;
	
	// 加速値を求める
	// a = 2(x - vot)/(t*t)
	t_x_t = (count_max * count_max) << FX32_SHIFT;
	vot = FX_Mul( s_s, count_max * FX32_ONE );
	vot = dis - vot;
	vot = FX_Mul( vot, 2*FX32_ONE );
	a = FX_Div( vot, t_x_t );

	p_wk->x = s_x;
	p_wk->s_x = s_x;
	p_wk->s_s = s_s;
	p_wk->s_a = a;
	p_wk->count_max = count_max;
}

//----------------------------------------------------------------------------
/**
 *	@brief	加速移動計算ワーク	メイン
 *
 *	@param	p_wk		ワーク
 *	@param	count		カウンタ
 */
//-----------------------------------------------------------------------------
static BOOL	MNGM_AddMoveMainFx( MNGM_ADDMOVE_WORK* p_wk, s32 count )
{
	fx32 dis;
	fx32 t_x_t;
	fx32 calc_work;
	fx32 vot;
	BOOL ret;

	if( count >= p_wk->count_max ){
		count = p_wk->count_max;
		ret = TRUE;
	}else{
		ret = FALSE;
	}
	
	// 等加速度運動
	// dis = vot + 1/2( a*(t*t) )
	vot = FX_Mul( p_wk->s_s, count << FX32_SHIFT );
	t_x_t = (count * count) << FX32_SHIFT;
	calc_work = FX_Mul( p_wk->s_a, t_x_t );
	calc_work = FX_Div( calc_work, 2*FX32_ONE );	// 1/2(a*(t*t))
	dis = vot + calc_work;	///<移動距離

	p_wk->x = p_wk->s_x + dis;

	return ret;
}


// スクリーン
//----------------------------------------------------------------------------
/**
 *	@brief	スクリーンのキャラクタオフセットを変更する
 *	
 *	@param	p_scrn		スクリーンバッファ
 *	@param	char_offs	変更するオフセット
 */
//-----------------------------------------------------------------------------
static void MNGM_SCRN_AddCharOfs( NNSG2dScreenData* p_scrn, u32 char_offs )
{
	int i;
	u16* p_scrndata;
	int size;

	size = p_scrn->szByte / 2;

	// スクリーンデータ代入
	p_scrndata = (u16*)p_scrn->rawData;

	for(i=0; i<size; i++){
		p_scrndata[ i ] += char_offs;
	}
}

// セルアクターリソース管理
//----------------------------------------------------------------------------
/**
 *	@brief	セルアクターリソース管理システム	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	objnum		オブジェクト数
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void MNGM_CLACTRES_Init( MNGM_CLACTRES* p_wk, u32 objnum, u32 heapID )
{
	int i;

	// リソースマネージャ作成
	for( i=0; i<MNGM_CLACTRES_RESMAN_NUM; i++ ){
		p_wk->p_resman[i] = CLACT_U_ResManagerInit(objnum, i, heapID);
	}

	// リソースオブジェクト作成
	p_wk->p_obj = sys_AllocMemory( heapID, sizeof(MNGM_CLACTRESOBJ)* objnum );
	memset( p_wk->p_obj, 0, sizeof(MNGM_CLACTRESOBJ)* objnum );
	p_wk->objnum = objnum;
}

//----------------------------------------------------------------------------
/**
 *	@brief	セルアクターリソース管理システム	破棄
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_CLACTRES_Exit( MNGM_CLACTRES* p_wk )
{
	int i;

	// 全リソースオブジェの破棄
	for( i=0; i<p_wk->objnum; i++ ){
		if( p_wk->p_obj[i].data == TRUE ){
			MNGM_CLACTRES_OBJ_Exit( p_wk, &p_wk->p_obj[i] );
		}
	}

	// リソースオブジェを破棄
	for( i=0; i<MNGM_CLACTRES_RESMAN_NUM; i++ ){
		CLACT_U_ResManagerDelete( p_wk->p_resman[i] );
	}

	// リソースオブジェ破棄
	sys_FreeMemoryEz( p_wk->p_obj );
}

//----------------------------------------------------------------------------
/**
 *	@brief	リソースを読み込む
 *
 *	@param	p_wk			ワーク
 *	@param	p_handle		ハンドル
 *	@param	pal_idx			パレットIDX
 *	@param	palnum			パレット数
 *	@param	cg_idx			キャラクタIDX
 *	@param	cell_idx		セルIDX
 *	@param	anm_idx			アニメIDX
 *	@param	contid			管理ID
 *	@param	heapID			ヒープID
 */
//-----------------------------------------------------------------------------
static MNGM_CLACTRESOBJ* MNGM_CLACTRES_Load( MNGM_CLACTRES* p_wk, ARCHANDLE* p_handle, u32 pal_idx, u32 palnum, u32 cg_idx, u32 cell_idx, u32 anm_idx, u32 contid, u32 heapID )
{
	int i;
	MNGM_CLACTRESOBJ* p_obj = NULL;
	
	// 空いてるオブジェを探す
	for( i=0; i<p_wk->objnum; i++ ){
		if( p_wk->p_obj[i].data == FALSE ){
			p_obj = &p_wk->p_obj[i];
			break;
		}
	}
	GF_ASSERT( p_obj );
	
	MNGM_CLACTRES_OBJ_Init( p_wk, p_obj, p_handle, pal_idx, palnum, cg_idx, cell_idx, anm_idx, contid, heapID );

	return p_obj;
}

//----------------------------------------------------------------------------
/**
 *	@brief	アクターの登録
 *
 *	@param	p_obj		オブジェクト
 *	@param	p_clset		セルアクターセット
 *	@param	x			ｘ座標
 *	@param	y			ｙ座標
 *	@param	pri			優先順位
 *	@param	heapID		ヒープID
 *
 *	@return	アクター
 */
//-----------------------------------------------------------------------------
static CLACT_WORK_PTR MNGM_CLACTRES_Add( MNGM_CLACTRESOBJ* p_obj, CLACT_SET_PTR p_clset, s16 x, s16 y, u16 pri, u32 heapID )
{
	CLACT_ADD_SIMPLE add;


	add.ClActSet		= p_clset;
	add.ClActHeader		= &p_obj->header;
	add.mat.x			= x << FX32_SHIFT;
	add.mat.y			= y << FX32_SHIFT;
	add.pri				= pri;
	add.DrawArea		= NNS_G2D_VRAM_TYPE_2DMAIN;
	add.heap			= heapID;

	return CLACT_AddSimple( &add );
}

//----------------------------------------------------------------------------
/**
 *	@brief	オブジェクトの読み込み
 *
 *	@param	p_wk		ワーク
 *	@param	p_obj		リソースオブジェクト
 *	@param	p_handle	ハンドル
 *	@param	pal_idx		パレットIDX
 *	@param	palnum		パレット数
 *	@param	cg_idx		キャラクタIDX
 *	@param	cell_idx	セルIDX
 *	@param	anm_idx		アニメIDX
 *	@param	contid		管理ID
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void MNGM_CLACTRES_OBJ_Init( MNGM_CLACTRES* p_wk, MNGM_CLACTRESOBJ* p_obj, ARCHANDLE* p_handle, u32 pal_idx, u32 palnum, u32 cg_idx, u32 cell_idx, u32 anm_idx, u32 contid, u32 heapID )
{
	BOOL result;
	
	GF_ASSERT( p_obj->data == FALSE );

	p_obj->data = TRUE;
	
	
	// キャラクタ読み込み
	p_obj->p_resobj[0] = CLACT_U_ResManagerResAddArcChar_ArcHandle( 
				p_wk->p_resman[0], p_handle,
				cg_idx,
				FALSE, contid, NNS_G2D_VRAM_TYPE_2DMAIN, heapID );

	// パレット読み込み
	p_obj->p_resobj[1] = CLACT_U_ResManagerResAddArcPltt_ArcHandle( 
			p_wk->p_resman[1], p_handle,
			pal_idx,
			FALSE, contid, 
			NNS_G2D_VRAM_TYPE_2DMAIN, palnum, heapID );

	// セル読み込み
	p_obj->p_resobj[2] = CLACT_U_ResManagerResAddArcKindCell_ArcHandle( 
				p_wk->p_resman[2], p_handle,
				cell_idx,
				FALSE, contid, CLACT_U_CELL_RES, heapID );

	// アニメ読み込み
	p_obj->p_resobj[3] = CLACT_U_ResManagerResAddArcKindCell_ArcHandle( 
				p_wk->p_resman[3], p_handle,
				anm_idx,
				FALSE, contid, CLACT_U_CELLANM_RES, heapID );


	// VRAM展開
	result =CLACT_U_CharManagerSetAreaCont( p_obj->p_resobj[ 0 ] );
	GF_ASSERT( result );
	CLACT_U_ResManagerResOnlyDelete( p_obj->p_resobj[ 0 ] );
	result =CLACT_U_PlttManagerSetCleanArea( p_obj->p_resobj[ 1 ] );
	GF_ASSERT( result );
	CLACT_U_ResManagerResOnlyDelete( p_obj->p_resobj[ 1 ] );

	// ヘッダー作成
	CLACT_U_MakeHeader( &p_obj->header, contid, contid, contid, contid,
					CLACT_U_HEADER_DATA_NONE, CLACT_U_HEADER_DATA_NONE,
					0, MNGM_CLACTRES_DEF_BG_PRI,
					p_wk->p_resman[0],
					p_wk->p_resman[1],
					p_wk->p_resman[2],
					p_wk->p_resman[3],
					NULL, NULL );
}

//----------------------------------------------------------------------------
/**
 *	@brief	リソースオブジェの破棄
 *
 *	@param	p_wk		ワーク
 *	@param	p_obj		オブジェ
 */
//-----------------------------------------------------------------------------
static void MNGM_CLACTRES_OBJ_Exit( MNGM_CLACTRES* p_wk, MNGM_CLACTRESOBJ* p_obj )
{
	int i;

	GF_ASSERT( p_obj->data == TRUE );

	p_obj->data = FALSE;
	
	// VRAMから破棄
	CLACT_U_CharManagerDelete( p_obj->p_resobj[0] );
	CLACT_U_PlttManagerDelete( p_obj->p_resobj[1] );

	// リソース破棄
	for( i=0; i<MNGM_CLACTRES_RESMAN_NUM; i++ ){
		CLACT_U_ResManagerResDelete( p_wk->p_resman[i], 
				p_obj->p_resobj[i] );
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	BGLシステム初期化
 *
 *	@param	p_wk		ワーク
 *	@param	cp_sys		システムヘッダー
 *	@param	cp_cnt		管理ヘッダー
 *	@param	cnt_num		管理データ数
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void MNGM_BGL_Init( MNGM_BGL* p_wk, const GF_BGL_SYS_HEADER* cp_sys, const MNGM_BGL_DATA* cp_cnt, u32 cnt_num, u32 heapID )
{
	
	// BG設定
	GF_BGL_InitBG(cp_sys);

	p_wk->p_bgl = GF_BGL_BglIniAlloc( heapID );
	p_wk->cp_tbl = cp_cnt;
	p_wk->tblnum = cnt_num;

	// メインとサブを切り替える
	sys.disp3DSW = DISP_3D_TO_MAIN;
	GF_Disp_DispSelect();


	// BGコントロール設定
	{
		int i;

		for( i=0; i<cnt_num; i++ ){
			GF_BGL_BGControlSet( p_wk->p_bgl, 
					cp_cnt[i].frame, &cp_cnt[i].cnt,
					GF_BGL_MODE_TEXT );
			GF_BGL_ClearCharSet( cp_cnt[i].frame, 32, 0, heapID);
			GF_BGL_ScrClear( p_wk->p_bgl, cp_cnt[i].frame );

			// スクロール位置初期化
			GF_BGL_ScrollSet( p_wk->p_bgl, cp_cnt[i].frame, GF_BGL_SCROLL_X_SET, 0 );
			GF_BGL_ScrollSet( p_wk->p_bgl, cp_cnt[i].frame, GF_BGL_SCROLL_Y_SET, 0 );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	BGL管理システム	破棄
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_BGL_Exit( MNGM_BGL* p_wk )
{

	// スクロールクリーン
	GF_BGL_ScrollSet( p_wk->p_bgl, GF_BGL_FRAME3_M, GF_BGL_SCROLL_Y_SET, 0 );

	// ＢＧコントロール破棄
	{
		int i;

		for( i=0; i<p_wk->tblnum; i++ ){
			GF_BGL_BGControlExit( p_wk->p_bgl, p_wk->cp_tbl[i].frame );
		}
	}
	
	// BGL破棄
	sys_FreeMemoryEz( p_wk->p_bgl );
}

//----------------------------------------------------------------------------
/**
 *	@brief	VBlank関数
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_BGL_VBlank( MNGM_BGL* p_wk )
{
    // BG書き換え
    GF_BGL_VBlankFunc( p_wk->p_bgl );
}


//----------------------------------------------------------------------------
/**
 *	@brief	セルアクター初期化
 *
 *	@param	p_wk		ワーク
 *	@param	objnum		オブジェクト数
 *	@param	charnum		キャラクタ数
 *	@param	plttnum		パレット数
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void MNGM_CLACT_Init( MNGM_CLACT* p_wk, u32 objnum, u32 charnum, u32 plttnum, u32 heapID )
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
	{

		CHAR_MANAGER_MAKE char_init = sc_MNGM_CHARMAN_INIT;
		char_init.CharDataNum	= charnum;
		char_init.heap			= heapID;
	    InitCharManagerReg(&char_init, GX_OBJVRAMMODE_CHAR_1D_32K, GX_OBJVRAMMODE_CHAR_1D_32K );
	}
    // パレットマネージャー初期化
    InitPlttManager(plttnum, heapID);

    // 読み込み開始位置を初期化
    CharLoadStartAll();
    PlttLoadStartAll();

    //通信アイコン用にキャラ＆パレット制限
    CLACT_U_WmIcon_SetReserveAreaCharManager(NNS_G2D_VRAM_TYPE_2DMAIN, GX_OBJVRAMMODE_CHAR_1D_32K);
    CLACT_U_WmIcon_SetReserveAreaPlttManager(NNS_G2D_VRAM_TYPE_2DMAIN);
    

    // セルアクターセット作成
    p_wk->p_clactset = CLACT_U_SetEasyInit( objnum, &p_wk->renddata, heapID );

	// 下画面に通信アイコンを出す
	WirelessIconEasy();  // 接続中なのでアイコン表示


	// 表示開始
    GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
    GF_Disp_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
}

//----------------------------------------------------------------------------
/**
 *	@brief	セルアクター破棄
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_CLACT_Exit( MNGM_CLACT* p_wk )
{
    // アクターの破棄
    CLACT_DestSet( p_wk->p_clactset );

    // リソース解放
    DeleteCharManager();
    DeletePlttManager();

    //OAMレンダラー破棄
    REND_OAM_Delete();
}

//----------------------------------------------------------------------------
/**
 *	@brief	セルアクター表示処理
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_CLACT_Draw( MNGM_CLACT* p_wk )
{
	CLACT_Draw( p_wk->p_clactset );
}

//----------------------------------------------------------------------------
/**
 *	@brief	セルアクター	VBlank処理
 *
 *	@param p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_CLACT_VBlank( MNGM_CLACT* p_wk )
{
    // レンダラ共有OAMマネージャVram転送
    REND_OAMTrans();
}



//----------------------------------------------------------------------------
/**
 *	@brief	メッセージシステム	初期化
 *
 *	@param	p_wk		ワーク		
 *	@param	heapID		ひーぷID
 */	
//-----------------------------------------------------------------------------
static void MNGM_MSG_Init( MNGM_MSG* p_wk, u32 heapID )
{
	p_wk->p_msgman	= MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_minigame_tool_dat, heapID );
	p_wk->p_wordset	= WORDSET_CreateEx( WORDSET_DEFAULT_SETNUM, WORDSET_COUNTRY_BUFLEN, heapID );
	p_wk->p_str		= STRBUF_Create( MNGM_MSG_STRBUF_NUM, heapID );
	p_wk->p_tmp		= STRBUF_Create( MNGM_MSG_STRBUF_NUM, heapID );

	// メッセージパレットを転送する
    TalkFontPaletteLoad( PALTYPE_MAIN_BG, MNGM_MSG_PLTT*0x20, heapID );
    TalkFontPaletteLoad( PALTYPE_SUB_BG,  MNGM_MSG_PLTT*0x20, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージシステム	破棄
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_MSG_Exit( MNGM_MSG* p_wk )
{
	STRBUF_Delete( p_wk->p_tmp );
	STRBUF_Delete( p_wk->p_str );
	WORDSET_Delete( p_wk->p_wordset );
	MSGMAN_Delete( p_wk->p_msgman );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ワードセット無いのメッセージバッファをすべてから文字でクリア
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_MSG_ClearStrBuff( MNGM_MSG* p_wk )
{
	WORDSET_ClearAllBuffer( p_wk->p_wordset );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ユーザー名表示
 *		
 *	@param	p_wk			ワーク
 *	@param	cp_player		プレイヤー名
 */
//-----------------------------------------------------------------------------
static void MNGM_MSG_SetPlayerName( MNGM_MSG* p_wk, const MYSTATUS* cp_player )
{
	WORDSET_RegisterPlayerName( p_wk->p_wordset, 0, cp_player );
}

//----------------------------------------------------------------------------
/**
 *	@brief	国名を登録する
 *
 *	@param	p_wk			ワーク
 *	@param	nation			国ID
 */
//-----------------------------------------------------------------------------
static void MNGM_MSG_SetNationName( MNGM_MSG* p_wk, u32 nation )
{
	WORDSET_RegisterCountryName( p_wk->p_wordset, 0, nation );
}

//----------------------------------------------------------------------------
/**
 *	@brief	地域名を登録する
 *
 *	@param	p_wk			ワーク
 *	@param	nation			国ID
 *	@param	area			地域ID
 */
//-----------------------------------------------------------------------------
static void MNGM_MSG_SetAreaName( MNGM_MSG* p_wk, u32 nation, u32 area )
{
	WORDSET_RegisterLocalPlaceName( p_wk->p_wordset, 0, nation, area );
}

//----------------------------------------------------------------------------
/**
 *	@brief	数字を格納する
 *
 *	@param	p_wk		ワーク
 *	@param	number		ナンバー
 *	@param	keta		桁
 */
//-----------------------------------------------------------------------------
static void MNGM_MSG_SetScore( MNGM_MSG* p_wk, u32 number, u32 keta )
{
	WORDSET_RegisterNumber( p_wk->p_wordset, 0, number,
			keta, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT );
}

//----------------------------------------------------------------------------
/**
 *	@brief	割った風船の数を格納する
 *
 *	@param	p_wk		ワーク
 *	@param	number		数
 */
//-----------------------------------------------------------------------------
static void MNGM_MSG_SetBalloonNum( MNGM_MSG* p_wk, u32 number )
{
	WORDSET_RegisterNumber( p_wk->p_wordset, 0, number,
			2, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT );
}

//----------------------------------------------------------------------------
/**
 *	@brief	時間を格納する
 *
 *	@param	p_wk		ワーク
 *	@param	time		時間
 */
//-----------------------------------------------------------------------------
static void MNGM_MSG_SetTime( MNGM_MSG* p_wk, u32 time )
{
	u32 sec;
	u32 miri;

	miri = (time * MNGM_MSG_TIME_1DATA) / MNGM_MSG_TIME_1DATADIV;
	sec  = miri / MNGM_MSG_TIME_1DATA;	// 100miri秒で1秒
	miri -= sec*MNGM_MSG_TIME_1DATA;
	
	WORDSET_RegisterNumber( p_wk->p_wordset, 0, sec,
			2, NUMBER_DISPTYPE_ZERO, NUMBER_CODETYPE_DEFAULT );

	WORDSET_RegisterNumber( p_wk->p_wordset, 1, miri,
			2, NUMBER_DISPTYPE_ZERO, NUMBER_CODETYPE_DEFAULT );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ガジェット名を設定する
 *
 *	@param	p_wk		ワーク
 *	@param	gadget		ガジェットナンバー
 */
//-----------------------------------------------------------------------------
static void MNGM_MSG_SetGadget( MNGM_MSG* p_wk, u32 gadget )
{
	WORDSET_RegisterWiFiLobbyItemName( p_wk->p_wordset, 1, gadget );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ゲーム名を設定する
 *	
 *	@param	p_wk		ワーク
 *	@param	game		ゲームナンバー
 */
//-----------------------------------------------------------------------------
static void MNGM_MSG_SetGameName( MNGM_MSG* p_wk, u32 game )
{
	WORDSET_RegisterWiFiLobbyGameName( p_wk->p_wordset, 0, game );
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージデータを取得する
 *
 *	@param	p_wk	ワーク
 *	@param	p_str	メッセージデータ格納先
 *	@param	msgidx	メッセージインデックス
 */
//-----------------------------------------------------------------------------
static void MNGM_MSG_GetStr( MNGM_MSG* p_wk, STRBUF* p_str, u32 msgidx )
{
	MSGMAN_GetString( p_wk->p_msgman, msgidx, p_wk->p_tmp );
	WORDSET_ExpandStr( p_wk->p_wordset, p_str, p_wk->p_tmp );
}


//----------------------------------------------------------------------------
/**
 *	@brief	メッセージプリント
 *
 *	@param	p_wk		ワーク
 *	@param	no			メッセージNO
 *	@param	p_win		ウィンドウ
 *	@param	x			ｘ座標
 *	@param	y			ｙ座標
 */
//-----------------------------------------------------------------------------
static void MNGM_MSG_Print( MNGM_MSG* p_wk, u32 no, GF_BGL_BMPWIN* p_win, u8 x, u8 y )
{
	MNGM_MSG_PrintColor( p_wk, no, p_win, x, y, MNGM_MSG_COLOR );
}

//----------------------------------------------------------------------------
/**
 *	@brief	右端つめにして文字列を表示
 *
 *	@param	p_wk		ワーク
 *	@param	no			メッセージNO
 *	@param	p_win		ウィンドウ
 *	@param	x			右端ｘ座標
 *	@param	y			ｙ座標
 */
//-----------------------------------------------------------------------------
static void MNGM_MSG_PrintRightSide( MNGM_MSG* p_wk, u32 no, GF_BGL_BMPWIN* p_win, u8 x, u8 y )
{
	u32 strsize;
	s32 draw_x;
	
	MSGMAN_GetString( p_wk->p_msgman, no, p_wk->p_tmp );
	WORDSET_ExpandStr( p_wk->p_wordset, p_wk->p_str, p_wk->p_tmp );

	strsize = FontProc_GetPrintStrWidth( FONT_SYSTEM, p_wk->p_str, 0 );
	draw_x = x - strsize;
	if( draw_x < 0 ){
		draw_x = 0;
	}
	
	GF_STR_PrintColor( p_win, FONT_SYSTEM, p_wk->p_str,
			draw_x,y,MSG_NO_PUT, MNGM_MSG_COLOR, NULL);
}

//----------------------------------------------------------------------------
/**

 *	@brief	メッセージプリント
 *
 *	@param	p_wk		ワーク
 *	@param	no			メッセージNO
 *	@param	p_win		ウィンドウ
 *	@param	p_str		文字バッファ
 *	@param	wait		メッセージ描画ウエイト
 *
 *	@retval	メッセージナンバー
 */
//-----------------------------------------------------------------------------
static u32 MNGM_MSG_PrintScr( MNGM_MSG* p_wk, u32 no, GF_BGL_BMPWIN* p_win, STRBUF* p_str, u32 wait )
{
	MSGMAN_GetString( p_wk->p_msgman, no, p_wk->p_tmp );
	WORDSET_ExpandStr( p_wk->p_wordset, p_str, p_wk->p_tmp );
	
	return GF_STR_PrintColor( p_win, FONT_TALK, p_str,
			0, 0, wait, MNGM_MSG_TALKCOLOR, NULL);
}

//----------------------------------------------------------------------------
/**
 *	@brief	色指定メッセージ描画
 *
 *	@param	p_wk		ワーク
 *	@param	no			メッセージNO
 *	@param	p_win		ウィンドウ
 *	@param	x			ｘ座標
 *	@param	y			ｙ座標
 *	@param	col			色
 */
//-----------------------------------------------------------------------------
static void MNGM_MSG_PrintColor( MNGM_MSG* p_wk, u32 no, GF_BGL_BMPWIN* p_win, u8 x, u8 y, GF_PRINTCOLOR col )
{
	MSGMAN_GetString( p_wk->p_msgman, no, p_wk->p_tmp );
	WORDSET_ExpandStr( p_wk->p_wordset, p_wk->p_str, p_wk->p_tmp );
	
	GF_STR_PrintColor( p_win, FONT_SYSTEM, p_wk->p_str,
			x,y,MSG_NO_PUT, col, NULL);
}

//----------------------------------------------------------------------------
/**
 *	@brief	会話ウィンドウ初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_bgl		ｂｇシステム
 *	@param	p_save		セーブ
 *	@param	vip			VIPフラグ
 *	@param	heapID		ヒープ
 */
//-----------------------------------------------------------------------------
static void MNGM_TALKWIN_Init( MNGM_TALKWIN* p_wk, MNGM_BGL* p_bgl, SAVEDATA* p_save, BOOL vip, u32 heapID )
{
	u32 type = CONFIG_GetWindowType(SaveData_GetConfig(p_save)); 
	u32 frame;
	int i;
	u8 y;

	//  オートメッセージ設定
	ConTool_MsgPrintFlagSet( TRUE );
	
	// トークウィンドウのウィンドウを転送
    TalkWinGraphicSet(
            p_bgl->p_bgl, GF_BGL_FRAME2_M, MNGM_TALKWIN_CGX, 
			MNGM_TALKWIN_PAL,  type, heapID );
    TalkWinGraphicSet(
            p_bgl->p_bgl, GF_BGL_FRAME0_S, MNGM_TALKWIN_CGX, 
			MNGM_TALKWIN_PAL,  type, heapID );

	// ウィンドウの作成
	for( i=0; i<MNGM_TALKWIN_IDX_NUM; i++ ){
		if( i==MNGM_TALKWIN_IDX_MAIN ){
			frame = GF_BGL_FRAME2_M;
			y = MNGM_TALKWIN_BMP_Y;
		}else{
			frame = GF_BGL_FRAME0_S;
			y = MNGM_TALKWIN_BMP_Y_SUB;
		}
		GF_BGL_BmpWinAdd( p_bgl->p_bgl, &p_wk->win[i], frame,
				MNGM_TALKWIN_BMP_X, y,
				MNGM_TALKWIN_BMP_SIZX, MNGM_TALKWIN_BMP_SIZY,
				MNGM_TALKWIN_BMP_PAL, MNGM_TALKWIN_BMP_CGX);
		GF_BGL_BmpWinDataFill( &p_wk->win[i], 0 );

		// メッセージ用文字列バッファ作成
		p_wk->p_str[i] = STRBUF_Create( MNGM_MSG_STRBUF_NUM, heapID );
	}

	p_wk->vip = vip;
}

//----------------------------------------------------------------------------
/**
 *	@brief	会話ウィンドウ	破棄
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_TALKWIN_Exit( MNGM_TALKWIN* p_wk )
{
	int i;
	
	// ウィンドウと文字列バッファを破棄
	for( i=0; i<MNGM_TALKWIN_IDX_NUM; i++ ){
		MNGM_TALKWIN_MsgOff( p_wk, i );
		STRBUF_Delete( p_wk->p_str[i] );
		GF_BGL_BmpWinDel( &p_wk->win[i] );
	}

	//  オートメッセージ設定破棄
	ConTool_MsgPrintFlagReset();
}

//----------------------------------------------------------------------------
/**
 *	@brief	会話ウィンドウ	プリント
 *
 *	@param	p_wk		ワーク
 *	@param	p_msg		ウィンドウ	
 *	@param	msgidx		メッセージIDX
 *	@param	idx			メインかサブか
 */
//-----------------------------------------------------------------------------
static void MNGM_TALKWIN_MsgPrint( MNGM_TALKWIN* p_wk, MNGM_MSG* p_msg, u32 msgidx, u32 idx )
{
	int i;
	
	// VIPならmsgidxをVIP用のものにする
	if( p_wk->vip == TRUE ){
		for( i=0; i<NELEMS(sc_MNGM_MSG_DATA); i++ ){
			if( sc_MNGM_MSG_DATA[i].normal == msgidx ){
				msgidx = sc_MNGM_MSG_DATA[i].vip;
			}
		}
	}
	
	
	// 表示中ならとめる
	if( GF_MSG_PrintEndCheck( p_wk->msg_no[idx] ) ){
		GF_STR_PrintForceStop( p_wk->msg_no[idx] );
	}
	
	// 時間ウィンドウも止める
	MNGM_TALKWIN_CleanTimeWork( p_wk, idx );
	
	GF_BGL_BmpWinDataFill( &p_wk->win[idx], 15 );
	p_wk->msg_no[idx] = MNGM_MSG_PrintScr( p_msg, msgidx, &p_wk->win[idx],
			p_wk->p_str[idx], MNGM_TALKWIN_MSG_SPEED );

	// ウインドウを書き込む
    BmpTalkWinWrite(&p_wk->win[idx], WINDOW_TRANS_OFF, MNGM_TALKWIN_CGX, MNGM_TALKWIN_PAL );

	// 転送リクエスト
	GF_BGL_BmpWinOnVReq( &p_wk->win[idx] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージウィンドウを消す
 *
 *	@param	p_wk		ワーク
 *	@param	idx			メインかサブか
 */
//-----------------------------------------------------------------------------
static void MNGM_TALKWIN_MsgOff( MNGM_TALKWIN* p_wk, u32 idx )
{
	MNGM_TALKWIN_CleanTimeWork( p_wk, idx );
	// 表示中ならとめる
	if( GF_MSG_PrintEndCheck( p_wk->msg_no[idx] ) ){
		GF_STR_PrintForceStop( p_wk->msg_no[idx] );
	}
	BmpTalkWinClear( &p_wk->win[idx], WINDOW_TRANS_OFF );
	GF_BGL_BmpWinOffVReq( &p_wk->win[idx] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージウィンドウ	メッセージ終了チェック
 *
 *	@param	p_wk		ワーク
 *	@param	idx			メインかサブか
 *	
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
static BOOL MNGM_TALKWIN_MsgEndCheck( const MNGM_TALKWIN* p_wk, u32 idx )
{
	u32 result;
	result = GF_MSG_PrintEndCheck(p_wk->msg_no[idx]);
	if( result == 0 ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージウィンドウ	座標設定
 *	
 *	@param	p_wk		ワーク
 *	@param	x			ｘ座標
 *	@param	y			ｙ座標
 *	@param	idx			メインかサブか
 */
//-----------------------------------------------------------------------------
static void MNGM_TALKWIN_SetPos( MNGM_TALKWIN* p_wk, u8 x, u8 y, u32 idx )
{
	GF_BGL_BmpWinSet_PosX( &p_wk->win[idx], x );
	GF_BGL_BmpWinSet_PosY( &p_wk->win[idx], y );
}

//----------------------------------------------------------------------------
/**
 *	@brief	時間マークを出す
 *	
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_TALKWIN_SetTimeWork( MNGM_TALKWIN* p_wk, u32 idx )
{
	if( p_wk->p_timewait[idx] == NULL ){
		p_wk->p_timewait[idx] = TimeWaitIconAdd( &p_wk->win[idx], MNGM_TALKWIN_CGX );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	時間マークを消す
 *
 *	@param	p_wk	ワーク
 *	@param	idx		インデックス
 */
//-----------------------------------------------------------------------------
static void MNGM_TALKWIN_CleanTimeWork( MNGM_TALKWIN* p_wk, u32 idx )
{
	if( p_wk->p_timewait[idx] ){
        TimeWaitIconTaskDel(p_wk->p_timewait[idx]);  // タイマー止め
		p_wk->p_timewait[idx] = NULL;
	}
}






//-------------------------------------
///	エントリー画面ワーク
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	エントリー画面作成	共通
 *
 *	@param	cp_commparam	共通パラメータ
 *	@param	gametype		ゲームタイプ
 *	@param	heapID			ヒープID
 *
 *	@return	ワーク
 */
//-----------------------------------------------------------------------------
static MNGM_ENTRYWK* MNGM_ENTRY_CommonInit( const MNGM_ENRES_PARAM* cp_commparam, u32 gametype, u32 heapID )
{
	MNGM_ENTRYWK* p_wk;
	int i;

	// BGM再生
	Snd_DataSetByScene( SND_SCENE_WIFI_LOBBY_GAME, SEQ_PL_WIFIGAME, 0 );

	p_wk = sys_AllocMemory( heapID, sizeof(MNGM_ENTRYWK) );
	memset( p_wk, 0, sizeof(MNGM_ENTRYWK) );

	// パラメータをコピー
	memcpy( &p_wk->comm_param, cp_commparam, sizeof(MNGM_ENRES_PARAM) );

	// ゲームタイプ設定
	p_wk->gametype = gametype;

	G2_BlendNone();
	G2S_BlendNone();
	GX_SetVisibleWnd(0);
	GXS_SetVisibleWnd(0);

	// バンク設定
	GF_Disp_SetBank( &sc_MNGM_ENTRY_BANK );

	// ヒープID保存
	p_wk->heapID = heapID;

	// 通信コマンド設定
	CommCommandMNGMEntryInitialize( p_wk );

	// 通信データを設定
	MNGM_COMMPDATA_Init( &p_wk->comm_pdata, &p_wk->comm_param );

	// BG
	MNGM_BGL_Init( &p_wk->bgl, &sc_MNGM_ENTRY_BGINIT, sc_MNGM_ENTRY_BGCNT, MNGM_ENTRY_BGL_USE, heapID );

	// CLACT
	MNGM_CLACT_Init( &p_wk->clact, MNGM_ENTRY_CLACT_WKNUM, MNGM_ENTRY_CLACT_CHARNUM, MNGM_ENTRY_CLACT_PLTTNUM, heapID );

	// CLACTRES
	MNGM_CLACTRES_Init( &p_wk->clres, MNGM_ENTRY_CLACT_RESNUM, heapID );

	// メッセージシステム作成
	MNGM_MSG_Init( &p_wk->msg, heapID );	

	// グラフィック読み込み
	MNGM_ENTRY_GraphicLoad( p_wk, heapID );

	// メッセージシステム初期化
	{
		BOOL vip;
		u32 netid;

		netid = MNGM_ENRES_PARAM_GetPlNOtoNetID( &p_wk->comm_param, p_wk->comm_param.my_playerid );
		
		vip = MNGM_ENRES_PARAM_GetVipFlag( &p_wk->comm_param, netid );
		MNGM_TALKWIN_Init( &p_wk->talkwin, &p_wk->bgl, p_wk->comm_param.p_save, 
				vip,  heapID );
	}


	// タスク生成
	p_wk->tcb		= TCB_Add( MNGM_ENTRY_Tcb, p_wk, 0 );
	p_wk->tcb_vwait = VWaitTCB_Add( MNGM_ENTRY_VWaitTcb, p_wk, 0 );

	// VCHAT ON
	if( p_wk->comm_param.vchat ){
		// ボイスチャット開始
		mydwc_startvchat( heapID );
	}
	
	return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	エントリー画面	グラフィック読み込み
 *
 *	@param	p_wk		ワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void MNGM_ENTRY_GraphicLoad( MNGM_ENTRYWK* p_wk, u32 heapID )
{
	ARCHANDLE* p_handle;

	p_handle = ArchiveDataHandleOpen( ARC_WLMNGM_TOOL_GRA, heapID );

	// パレット
	ArcUtil_HDL_PalSet( p_handle, NARC_wlmngm_tool_minigame_win_NCLR,
			PALTYPE_MAIN_BG, 0, MNGM_BGPLTT_NUM*32, heapID );
	
	// キャラクタ	
	ArcUtil_HDL_BgCharSet( p_handle, NARC_wlmngm_tool_minigame_win_bg_NCGR,
			p_wk->bgl.p_bgl, 
			GF_BGL_FRAME1_M, 0, 0,
			FALSE, heapID );

	// スクリーン
	p_wk->p_scrnbuf =  ArcUtil_HDL_ScrnDataGet( p_handle,
			NARC_wlmngm_tool_minigame_win_bg1_NSCR,
			FALSE, &p_wk->p_scrn, heapID );

	// 背景設定
	ArcUtil_HDL_ScrnSet( p_handle, NARC_wlmngm_tool_minigame_win_bg0_NSCR, p_wk->bgl.p_bgl,
			GF_BGL_FRAME3_M, 0, 0, FALSE, heapID );
	// 背景パレット転送
	MNGM_BACKPLTT_Trans( p_handle, p_wk->gametype, heapID );
	

	// OAMを読み込む
	p_wk->p_resobj = MNGM_CLACTRES_Load( &p_wk->clres, p_handle, 
			NARC_wlmngm_tool_minigame_win_oam_NCLR, MNGM_PLAYER_OAM_PLTT_NUM,
			NARC_wlmngm_tool_minigame_win_oam_NCGR,
			NARC_wlmngm_tool_minigame_win_oam_NCER,
			NARC_wlmngm_tool_minigame_win_oam_NANR,
			MNGM_RESCONTID, heapID );

	// タイトルロゴ
	MNGM_TITLELOGO_Init( &p_wk->titlelogo, &p_wk->bgl, &p_wk->msg, p_wk->gametype, p_handle, heapID );

	ArchiveDataHandleClose( p_handle );
}

//----------------------------------------------------------------------------
/**
 *	@brief	エントリー画面	グラフィック	破棄
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_ENTRY_GraphicDelete( MNGM_ENTRYWK* p_wk )
{
	// スクリーンを破棄
	sys_FreeMemoryEz( p_wk->p_scrnbuf );
}

//----------------------------------------------------------------------------
/**
 *	@brief	エントリー画面	タスク
 *
 *	@param	tcb			TCB
 *	@param	p_work		ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_ENTRY_Tcb( TCB_PTR tcb, void* p_work )
{
	MNGM_ENTRYWK* p_wk = p_work;
	BOOL  result;

	switch( p_wk->seq ){
	case MNGM_ENTRY_SEQ_WIPEIN_INIT:
		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEIN, 
				WIPE_TYPE_FADEIN, WIPE_FADE_OUTCOLOR, 
				WIPE_DEF_DIV, WIPE_DEF_SYNC, p_wk->heapID );

		// タイトルロゴ描画
		MNGM_TITLELOGO_InStart( &p_wk->titlelogo, &p_wk->bgl, p_wk->heapID );
		p_wk->seq ++;
		break;
		
	case MNGM_ENTRY_SEQ_WIPEIN_WAIT:
		MNGM_TITLELOGO_InMain( &p_wk->titlelogo, &p_wk->bgl );	// ロゴイン　処理
		result = WIPE_SYS_EndCheck();
		if( result == TRUE ){
			p_wk->seq ++;
		}
		break;

	// ロゴ入場ウエイト
	case MNGM_ENTRY_SEQ_LOGOIN_WAIT:
		result = MNGM_TITLELOGO_InMain( &p_wk->titlelogo, &p_wk->bgl );	// ロゴイン　処理
		if( result == TRUE ){
			p_wk->seq++;
		}
		break;

	// エントリーメッセージ表示
	case MNGM_ENTRY_SEQ_MSGON_INIT:
		MNGM_TALKWIN_MsgPrint( &p_wk->talkwin, &p_wk->msg, msg_00, MNGM_TALKWIN_IDX_MAIN );
		p_wk->seq ++;
		break;
		
	case MNGM_ENTRY_SEQ_MSGON_WAIT:
		result = MNGM_TALKWIN_MsgEndCheck( &p_wk->talkwin, MNGM_TALKWIN_IDX_MAIN );
		if( result ){
			p_wk->seq ++;
			
			p_wk->wait = MNGM_ENTRY_MSGWAIT;
		}
		break;
		
	case MNGM_ENTRY_SEQ_BGMOVE_START:
		if( p_wk->wait > 0 ){
			p_wk->wait --;
			if( p_wk->wait == 0 ){
				MNGM_TALKWIN_MsgOff( &p_wk->talkwin, MNGM_TALKWIN_IDX_MAIN );

				MNGM_TITLELOGO_OutStart( &p_wk->titlelogo, &p_wk->bgl );
				p_wk->seq++;
			}
		}
		break;

	// ロゴアウト待ち
	case MNGM_ENTRY_SEQ_LOGOOUT_WAIT:
		result = MNGM_TITLELOGO_OutMain( &p_wk->titlelogo, &p_wk->bgl );
		if( result == TRUE ){
			p_wk->seq ++;
		}
		break;

	
	// プレイヤーリスト表示
	case MNGM_ENTRY_SEQ_PLLIST_ON:
		// プレイヤー分のデータ作成
		{
			int i;
			BOOL draw_nation;	// 国IDで地域を表示するか
			BOOL vip;
			u32 netid;

			//  HBLANK処理開始
			MNGM_PLAYER_PLATE_HBLANK_BGSCR_Init( &p_wk->bgscrl, &p_wk->bgl, p_wk->heapID );

			// 国名で表示するかチェック
			draw_nation = MNGM_COMMPARAM_CheckDrawNation( &p_wk->comm_param, &p_wk->comm_pdata );
			
			for( i=0; i<p_wk->comm_param.num; i++ ){
				MNGM_PLATE_PLAYERTBL_Init( &p_wk->playertbl[i], p_wk->comm_param.num, 
						p_wk->gametype,
						p_wk->p_scrn, &p_wk->bgl, i, p_wk->comm_param.my_playerid,
						&p_wk->clact, p_wk->p_resobj, 
						p_wk->heapID );

				// 名前表示
				netid = MNGM_ENRES_PARAM_GetPlNOtoNetID( &p_wk->comm_param, i );
				vip = MNGM_ENRES_PARAM_GetVipFlag( &p_wk->comm_param, netid );
				MNGM_PLATE_PLAYERTBL_DrawName( &p_wk->playertbl[i], &p_wk->msg, p_wk->comm_pdata.cp_status[i], MNGM_ENTRY_DRAWNAME_X, 0, vip );
				// 国名表示
				MNGM_PLATE_PLAYERTBL_DrawNation( &p_wk->playertbl[i], &p_wk->msg, 
						draw_nation, p_wk->comm_pdata.nation[i],
						p_wk->comm_pdata.area[i], MNGM_ENTRY_DRAWNAME_X );

				// 動作パラメータ初期化
				MNGM_PLATE_PLAYERTBL_StartMove( &p_wk->playertbl[i], MNGM_PLATE_MOVE_ENTRY_END, MNGM_PLATE_MOVE_WAIT_ENTRY*i );
			}
		}


		//  書き込み命令
		{
			int i;
			for( i=0; i<p_wk->comm_param.num; i++ ){
				
				MNGM_PLATE_PLAYERTBL_BgWriteVReq( &p_wk->playertbl[i], &p_wk->bgl );
				MNGM_PLATE_PLAYERTBL_DrawOamWay( &p_wk->playertbl[i] );
			}
		}

		p_wk->wait = MNGM_ENTRY_WAIT;
		p_wk->seq ++;
		break;
		
	case MNGM_ENTRY_SEQ_BGMOVE_MAIN:
		if( MNGM_ENTRY_PLATE_ALLMove( p_wk ) == TRUE ){
			p_wk->seq ++;
		}
		break;

	case MNGM_ENTRY_SEQ_BGMOVE_WAIT:
#ifdef DEBUG_SYNCSTART_A
		if( sys.trg & PAD_BUTTON_A ){
			p_wk->seq ++;
			CommTimingSyncStart( MNGM_SYNC_END );

			// VCHAT OFF
			if( p_wk->comm_param.vchat ){
				// ボイスチャット終了
				mydwc_stopvchat();
			}

		}
#else
		p_wk->wait --;
		if( p_wk->wait == 0 ){
			p_wk->seq ++;
			CommTimingSyncStart( MNGM_SYNC_END );

			// VCHAT OFF
			if( p_wk->comm_param.vchat ){
				// ボイスチャット終了
				mydwc_stopvchat();
			}
		}
#endif
		break;

	case MNGM_ENTRY_SEQ_BGMOVE_SYNC:
		if( CommIsTimingSync( MNGM_SYNC_END ) ){
			p_wk->seq ++;
		}
		break;

	case MNGM_ENTRY_SEQ_BGMOVE_OUT:
		{
			int i;
			for( i=0; i<p_wk->comm_param.num; i++ ){
				// 動作パラメータ初期化
				MNGM_PLATE_PLAYERTBL_StartMoveRet( &p_wk->playertbl[i], i*MNGM_PLATE_MOVE_WAIT_ENTRY );
			}
			p_wk->seq ++;
			p_wk->wait = MNGM_ENTRY_OUTMOVEWAIT;
		}
		break;

	case MNGM_ENTRY_SEQ_BGMOVE_OUTWAIT:
		p_wk->wait--;
		MNGM_ENTRY_PLATE_ALLMove( p_wk );

		if( p_wk->wait <= 0 ){
			p_wk->seq ++;
		}
		break;
	
	case MNGM_ENTRY_SEQ_WIPEOUT_INIT:
		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEOUT, 
				WIPE_TYPE_FADEIN, WIPE_FADE_BLACK, 
				WIPE_DEF_DIV, WIPE_DEF_SYNC, p_wk->heapID );

		MNGM_ENTRY_PLATE_ALLMove( p_wk );

		// Rareゲームデータ送信
		if( p_wk->comm_param.my_playerid == 0 ){
			u32 raregame_type;
			
			switch( p_wk->gametype ){
			case WFLBY_GAME_BALLSLOW:	// 玉投げ
				raregame_type = MNGM_ENTRY_RareGameSelect( sc_MNGM_RAREGAME_BUCKET_TBL, MNGM_RAREGAME_BUCKET_NUM );
				break;

			case WFLBY_GAME_BALANCEBALL:// 玉乗り
				raregame_type = MNGM_ENTRY_RareGameSelect( sc_MNGM_RAREGAME_BALANCEBALL_TBL, MNGM_RAREGAME_BALANCEBALL_NUM );
				break;

			case WFLBY_GAME_BALLOON:	// ふうせんわり
				raregame_type = MNGM_ENTRY_RareGameSelect( sc_MNGM_RAREGAME_BALLOON_TBL, MNGM_RAREGAME_BALLOON_NUM );
				break;
			}

			CommSendData( CNM_MNGM_ENTRY_RAREGAME, &raregame_type, sizeof(u32) );
		}

		p_wk->seq ++;
		break;
		
	case MNGM_ENTRY_SEQ_WIPEOUT_WAIT:
		MNGM_ENTRY_PLATE_ALLMove( p_wk );
		result = WIPE_SYS_EndCheck();
		if( result == TRUE ){

			// ラスターシステム破棄
			MNGM_PLAYER_PLATE_HBLANK_BGSCR_Exit( &p_wk->bgscrl );

			p_wk->seq ++;
		}
		break;

	case MNGM_ENTRY_SEQ_RAREGAME_WAIT:
		if( p_wk->raregame_recv == TRUE ){
			p_wk->seq ++;
		}
		break;

	case MNGM_ENTRY_SEQ_END:
		break;
	}

	MNGM_CLACT_Draw( &p_wk->clact );

	// スクリーン面をスクロールさせる
	GF_BGL_ScrollReq( p_wk->bgl.p_bgl, GF_BGL_FRAME3_M, 
			GF_BGL_SCROLL_Y_INC, MNGM_BGSCROLL_SPEED );
	GF_BGL_ScrollReq( p_wk->bgl.p_bgl, GF_BGL_FRAME1_S, 
			GF_BGL_SCROLL_Y_INC, MNGM_BGSCROLL_SPEED );
}

//----------------------------------------------------------------------------
/**
 *	@brief	VBlankタスク
 */
//-----------------------------------------------------------------------------
static void MNGM_ENTRY_VWaitTcb( TCB_PTR tcb, void* p_work )
{
	MNGM_ENTRYWK* p_wk = p_work;

	// BGのVBLANK処理
    MNGM_BGL_VBlank( &p_wk->bgl );

	// OAMのVBLANK処理
	MNGM_CLACT_VBlank( &p_wk->clact );

	// ラスターシステム
	MNGM_PLAYER_PLATE_HBLANK_BGSCR_VBlank( &p_wk->bgscrl );
}

//----------------------------------------------------------------------------
/**
 *	@brief	rareゲームタイプの選択処理
 *
 *	@param	sc_ParTbl	テーブル
 *	@param	num			rareデータ数
 *
 *	@return	rareゲームタイプ
 */
//-----------------------------------------------------------------------------
static MNGM_RAREGAME_TYPE MNGM_ENTRY_RareGameSelect( const u8* cp_ParTbl, u32 num )
{
	u32 rand;
	u32 check;
	int i;

	rand = gf_mtRand() % 100;

//	OS_TPrintf( "rand %d\n", rand );
	
	check = 0;
	for( i=0; i<num; i++ ){
		check += cp_ParTbl[i];
//		OS_TPrintf( "check %d\n", check );
		if( check > rand ){
			return i;
		}
	}

	GF_ASSERT(0);	// ありえない
	return num-1;
}

//----------------------------------------------------------------------------
/**
 *	@brief	エントリー画面用	全PLATE動作
 *	
 *	@param	p_wk	わーく
 */
//-----------------------------------------------------------------------------
static BOOL MNGM_ENTRY_PLATE_ALLMove( MNGM_ENTRYWK* p_wk )
{
	int i;
	BOOL result;
	BOOL plate_end = TRUE;

	// 全部のPLATEを動かす
	for( i=0; i<p_wk->comm_param.num; i++ ){


		// 初期化されてるかチェック
		if( p_wk->playertbl[i].p_clwk != NULL ){

			result = MNGM_PLATE_PLAYERTBL_MainMove( &p_wk->playertbl[i] );
			MNGM_PLAYER_PLATE_HBLANK_BGSCR_SetPlateScr( &p_wk->bgscrl, &p_wk->playertbl[i] );
			if( result == FALSE ){
				plate_end = FALSE;
			}
		}else{
			plate_end = FALSE;
		}
	}

	return plate_end;
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーテーブル	初期化
 *
 *	@param	p_player		プレイヤーテーブル
 *	@param	player_num		プレイヤー数
 *	@param	gametype		ゲームタイプ
 *	@param	p_scrn			スクリーンデータ
 *	@param	p_bgl			BGL管理システム	
 *	@param	idx				インデックス
 *	@param	p_clact			セルアクター
 *	@param	p_oamres		セルリソース
 *	@param	heapID			ヒープID
 */
//-----------------------------------------------------------------------------
static void MNGM_PLATE_PLAYERTBL_Init( MNGM_PLATE_PLAYER* p_player, u32 player_num, u32 gametype, NNSG2dScreenData* p_scrn, MNGM_BGL* p_bgl, u32 idx, u32 myplayerid, MNGM_CLACT* p_clact, MNGM_CLACTRESOBJ* p_oamres, u32 heapID )
{
	s32 player_idx;
	// player_idxでインデックス参照するテーブルは
	// topとoam_anmのテーブル
	// 位置が変わるテーブルをplayer_idxで参照する

	// 位置はユーザのIDでインデックスが変化する
	switch( gametype ){
	case WFLBY_GAME_BALLSLOW:	// 玉投げ
	case WFLBY_GAME_BALLOON:	// ふうせんわり
		player_idx = MNGM_PLAYER_PLATE_BSBLIDX_Get( player_num, idx, myplayerid );
		break;
		
	// 玉乗りだけ、並びかかなり変わるため複雑になる。
	// 自分は常に０のインデックスを見て、
	// それ以外は、昇順に並ぶ形になる
	case WFLBY_GAME_BALANCEBALL:		// 玉乗り
		if( myplayerid > idx ){		
			// 自分より小さいプレイヤーは自分の分ずらす
			player_idx = idx + 1;
		}else if( myplayerid == idx ){
			// 自分は常に0
			player_idx = 0;
		}else{
			// 自分以降のプレイヤーはそのまま
			player_idx = idx;
		}
//		player_idx = sc_MNGM_PLAYER_PLATE_BBIDX_DATA[ player_num-1 ][ player_idx ];
		break;

	default:
		player_idx = idx;
		break;
	}

	// 表示プライオリティ設定
	GF_BGL_PrioritySet( GF_BGL_FRAME1_M, 2 );
	GF_BGL_PrioritySet( GF_BGL_FRAME0_M, 1 );
	
	// とりあえずその人の領域を作成して、データを書き込む
	GF_BGL_BmpWinAdd(
				p_bgl->p_bgl, &p_player->win, GF_BGL_FRAME0_M,
				MNGM_ENTRY_BMP_X, 
				MNGM_ENTRY_BMP_Y + sc_MNGM_PLAYER_PLATE_DATA[ player_num-1 ].top[ player_idx ], 
				MNGM_ENTRY_BMP_SIZX, MNGM_ENTRY_BMP_SIZY, MNGM_ENTRY_BMP_PAL,
				MNGM_ENTRY_BMP_CGX + ((MNGM_ENTRY_BMP_SIZX*MNGM_ENTRY_BMP_SIZY)*idx) );
	GF_BGL_BmpWinDataFill( &p_player->win, 0 );


	// スクリーンを書き込む
	GF_BGL_ScrWriteExpand(
				p_bgl->p_bgl, GF_BGL_FRAME1_M,
				0, sc_MNGM_PLAYER_PLATE_DATA[ player_num-1 ].top[ player_idx ], 
				MNGM_PLAYER_PLATE_SCRN_SIZ_X, MNGM_PLAYER_PLATE_SCRN_SIZ_Y,
				p_scrn->rawData, 0, 
				sc_MNGM_PLAYER_PLATE_DATA[ player_num-1 ].scrn_y[ idx ], 
				p_scrn->screenWidth/8,
				p_scrn->screenHeight/8 );


	// OAM設定する
	{
		u8 anm;
		u8 x, y;

		anm = sc_MNGM_PLAYER_PLATE_DATA[ player_num-1 ].oam_anm[ player_idx ];
		x	= sc_MNGM_PLATE_CLACT_PLATE_POS[anm][0];
		y	= (sc_MNGM_PLAYER_PLATE_DATA[ player_num-1 ].top[ player_idx ]*8) + sc_MNGM_PLATE_CLACT_PLATE_POS[anm][1];
		p_player->p_clwk = MNGM_CLACTRES_Add( p_oamres, p_clact->p_clactset, x, y, 0, heapID );
		CLACT_AnmChg( p_player->p_clwk, anm );
		CLACT_PaletteOffsetChg( p_player->p_clwk, idx );
		CLACT_SetDrawFlag( p_player->p_clwk, FALSE );
		CLACT_SetAnmFlag( p_player->p_clwk, TRUE );
	}

	// RankOAM設定
	{
		u8 x, y;

		x	= MNGM_PLATE_CLACT_RANK_X;
		y	= (sc_MNGM_PLAYER_PLATE_DATA[ player_num-1 ].top[ player_idx ]*8) + MNGM_PLATE_CLACT_RANK_Y;
		p_player->p_rank = MNGM_CLACTRES_Add( p_oamres, p_clact->p_clactset, x, y, 0, heapID );
		CLACT_AnmChg( p_player->p_rank, 0 );
		CLACT_SetDrawFlag( p_player->p_rank, FALSE );
	}

	p_player->idx		= idx;
	p_player->plidx		= player_idx;
	p_player->playernum = player_num;

}

//----------------------------------------------------------------------------
/**
 *	@brief	自分の名前を描画する
 *
 *	@param	p_player		プレイヤーテーブル
 *	@param	cp_status		その人のステータスデータ
 *	@param	x				表示ｘ座標
 *	@param	y				表示ｙ座標
 *	@param	vip				そいつがVIPかどうか
 */
//-----------------------------------------------------------------------------
static void MNGM_PLATE_PLAYERTBL_DrawName( MNGM_PLATE_PLAYER* p_player, MNGM_MSG* p_msg, const MYSTATUS* cp_status, s16 x, s16 y, u32 vip )
{
	MNGM_MSG_SetPlayerName( p_msg, cp_status );
	if( vip == TRUE ){
		MNGM_MSG_PrintColor( p_msg, msg_10, &p_player->win, x, y, MNGM_MSG_VIP_COLOR );
	}else{
		MNGM_MSG_Print( p_msg, msg_10, &p_player->win, x, y );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	地名を描画する
 *
 *	@param	p_player		プレイヤーテーブル
 *	@param	p_msg			メッセージシステム
 *	@param	draw_nation		国名を表示するのか
 *	@param	nation			国ID
 *	@param	area			地域ID
 */
//-----------------------------------------------------------------------------
static void MNGM_PLATE_PLAYERTBL_DrawNation( MNGM_PLATE_PLAYER* p_player, MNGM_MSG* p_msg, BOOL draw_nation, u32 nation, u32 area, s16 x )
{
	// 出身地を書き込む
	if(draw_nation){
		if( nation != country000 ){
			MNGM_MSG_ClearStrBuff( p_msg );
			MNGM_MSG_SetNationName( p_msg, nation );
			MNGM_MSG_Print( p_msg, msg_11, &p_player->win, x,16 );
		}
	}else{
		if( nation != country000 ){
			MNGM_MSG_ClearStrBuff( p_msg );
			MNGM_MSG_SetAreaName( p_msg, nation, area );
			MNGM_MSG_Print( p_msg, msg_12, &p_player->win, x,16 );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	その人の順位を書き込む
 *
 *	@param	p_player		プレイヤーテーブル
 *	@param	rank			ランク
 */
//-----------------------------------------------------------------------------
static void MNGM_PLATE_PLAYERTBL_DrawRank( MNGM_PLATE_PLAYER* p_player, u32 rank )
{
	CLACT_SetDrawFlag( p_player->p_rank, TRUE );
	CLACT_AnmChg( p_player->p_rank, MNGM_PLAYER_OAM_ANMSEQ_TOP+rank );

	CLACT_SetAnmFlag( p_player->p_rank, TRUE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	スコアを表示する
 *	
 *	@param	p_player		プレイヤーテーブル
 *	@param	p_msg			メッセージシステム
 *	@param	score			スコア
 *	@param	x				右端x座標
 *	@param	y				y座標
 *	@param	keta			桁数
 */
//-----------------------------------------------------------------------------
static void MNGM_PLATE_PLAYERTBL_DrawScore( MNGM_PLATE_PLAYER* p_player, MNGM_MSG* p_msg, u32 score, s16 x, s16 y, u32 keta )
{
	MNGM_MSG_SetScore( p_msg, score, keta );
	if( keta == 5 ){
		MNGM_MSG_PrintRightSide( p_msg, msg_14, &p_player->win, x, y );
	}else{
		MNGM_MSG_PrintRightSide( p_msg, msg_13, &p_player->win, x, y );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	時間を表示する
 *	
 *	@param	p_player	プレイヤーテーブル
 *	@param	p_msg		メッセージシステム
 *	@param	frame		時間
 *	@param	x			右端ｘ座標
 *	@param	y			ｙ座標
 */
//-----------------------------------------------------------------------------
static void MNGM_PLATE_PLAYERTBL_DrawTime( MNGM_PLATE_PLAYER* p_player, MNGM_MSG* p_msg, u32 frame, s16 x, s16 y )
{
	MNGM_MSG_SetTime( p_msg, frame );
	MNGM_MSG_PrintRightSide( p_msg, msg_13, &p_player->win, x, y );
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG書き込み命令を出す
 *
 *	@param	p_player		プレイヤーテーブル
 *	@param	p_bgl			BGLシステム
 */
//-----------------------------------------------------------------------------
static void MNGM_PLATE_PLAYERTBL_BgWriteVReq( MNGM_PLATE_PLAYER* p_player, MNGM_BGL* p_bgl )
{
	GF_BGL_BmpWinOnVReq( &p_player->win );
	GF_BGL_LoadScreenV_Req( p_bgl->p_bgl, GF_BGL_FRAME1_M );
}

//----------------------------------------------------------------------------
/**
 *	@brief	方向を示すマークを表示
 *
 *	@param	p_player	ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_PLATE_PLAYERTBL_DrawOamWay( MNGM_PLATE_PLAYER* p_player )
{
	CLACT_SetDrawFlag( p_player->p_clwk, TRUE );
}


//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーテーブル	破棄
 *
 *	@param	p_player		プレイヤーテーブル
 */
//-----------------------------------------------------------------------------
static void MNGM_PLATE_PLAYERTBL_Delete( MNGM_PLATE_PLAYER* p_player )
{
	if( p_player->p_clwk ){
		CLACT_Delete( p_player->p_clwk );
		p_player->p_clwk = NULL;
	}
	if( p_player->p_rank ){
		CLACT_Delete( p_player->p_rank );
		p_player->p_rank = NULL;
	}
	if( GF_BGL_BmpWinAddCheck( &p_player->win ) == TRUE ){
		GF_BGL_BmpWinDel( &p_player->win );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	PLATEの表示位置動作を設定
 *
 *	@param	p_player		プレイヤーテーブル
 *	@param	end_x			終端位置ｘ座標
 *	@param	wait			ウエイト
 */
//-----------------------------------------------------------------------------
static void MNGM_PLATE_PLAYERTBL_StartMove( MNGM_PLATE_PLAYER* p_player, s16 end_x, s16 wait )
{
	MNGM_AddMoveReqFx( &p_player->draw_x, FX32_CONST(MNGM_PLATE_MOVE_START),
			FX32_CONST(end_x), FX32_CONST(MNGM_PLATE_MOVE_SPEED), MNGM_PLATE_MOVE_COUNT );
	p_player->draw_x_count	= 0;
	p_player->draw_x_wait	= wait;
	p_player->draw_x_snd	= MNGM_SND_BAR_IN;		// 鳴らす音

	// OAMにも座標を設定する
	MNGM_PLATE_PLAYERTBL_SetOamMatrix( p_player, -(p_player->draw_x.x>>FX32_SHIFT) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	PLATEの表示位置	今いるところから画面端に移動する
 *
 *	@param	p_player		プレイヤーテーブル
 *	@param	wait			ウエイト
 */
//-----------------------------------------------------------------------------
static void MNGM_PLATE_PLAYERTBL_StartMoveRet( MNGM_PLATE_PLAYER* p_player, s16 wait )
{
	MNGM_AddMoveReqFx( &p_player->draw_x, p_player->draw_x.x,
			FX32_CONST(MNGM_PLATE_MOVE_START), 0, MNGM_PLATE_MOVE_COUNT );
	p_player->draw_x_count	= 0;
	p_player->draw_x_wait	= wait;
	p_player->draw_x_snd	= MNGM_SND_BAR_OUT;		// 鳴らす音

	// OAMにも座標を設定する
	MNGM_PLATE_PLAYERTBL_SetOamMatrix( p_player, -(p_player->draw_x.x>>FX32_SHIFT) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	PLATE表示位置動作　メイン
 *
 *	@param	p_player	プレイヤーテーブル
 *
 *	@retval	TRUE	完了
 *	@retval	FLASE	途中
 */
//-----------------------------------------------------------------------------
static BOOL MNGM_PLATE_PLAYERTBL_MainMove( MNGM_PLATE_PLAYER* p_player )
{
	BOOL ret;
	u8 x, y;
	if(p_player->draw_x_wait <= 0){
		if( p_player->draw_x_count == 0 ){
			// 最初に１回音を出す
			Snd_SePlay( p_player->draw_x_snd );
		}
		p_player->draw_x_count++;
		ret = MNGM_AddMoveMainFx( &p_player->draw_x, p_player->draw_x_count );

		// OAMにも座標を設定する
		MNGM_PLATE_PLAYERTBL_SetOamMatrix( p_player, -(p_player->draw_x.x>>FX32_SHIFT) );
		return ret;
	}
	p_player->draw_x_wait--;
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	OAMの座標を設定
 *
 *	@param	p_wk		プレイヤーテーブルワーク
 *	@param	draw_x		描画位置
 */
//-----------------------------------------------------------------------------
static void MNGM_PLATE_PLAYERTBL_SetOamMatrix( MNGM_PLATE_PLAYER* p_wk, s32 draw_x )
{
	u8 anm;
	u8 x, y;
	VecFx32 mat;
	
	// 方向
	anm = sc_MNGM_PLAYER_PLATE_DATA[ p_wk->playernum-1 ].oam_anm[ p_wk->plidx ];
	x	= sc_MNGM_PLATE_CLACT_PLATE_POS[anm][0];
	y	= (sc_MNGM_PLAYER_PLATE_DATA[ p_wk->playernum-1 ].top[ p_wk->plidx ]*8) + sc_MNGM_PLATE_CLACT_PLATE_POS[anm][1];
	
	mat.x = (x+draw_x)<<FX32_SHIFT;
	mat.y = y<<FX32_SHIFT;
	CLACT_SetMatrix( p_wk->p_clwk, &mat );


	// ランキング
	x	= MNGM_PLATE_CLACT_RANK_X;
	y	= (sc_MNGM_PLAYER_PLATE_DATA[ p_wk->playernum-1 ].top[ p_wk->plidx ]*8) + MNGM_PLATE_CLACT_RANK_Y;
	
	mat.x = (x+draw_x)<<FX32_SHIFT;
	mat.y = y<<FX32_SHIFT;
	CLACT_SetMatrix( p_wk->p_rank, &mat );
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーPLATE
 *
 *	@param	playernum		プレイヤー数
 *	@param	playerid		表示する人のID
 *	@param	myplayerid		自分のID
 *
 *	@return	その人が参照するべきsc_MNGM_PLAYER_PLATE_DATAのインデックス
 */
//-----------------------------------------------------------------------------
static u32 MNGM_PLAYER_PLATE_BSBLIDX_Get( u32 playernum, u32 playerid, u32 myplayerid )
{
	s32 myidx, plidx, playeridx;
	int i;

	// 自分のテーブルインデックスと表示する人のテーブルインデックスを取得
	for( i=0; i<playernum; i++ ){
		if( sc_MNGM_PLAYER_PLATE_BSBLIDX_DATA[ playernum-1 ][ i ] == playerid ){
			plidx = i;
		}
		if( sc_MNGM_PLAYER_PLATE_BSBLIDX_DATA[ playernum-1 ][ i ] == myplayerid ){
			myidx = i;
		}
	}

	playeridx = plidx - myidx;
	if( playeridx < 0 ){
		playeridx += playernum;
	}

	return playeridx;
}


//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーPLATE	HBLANK処理	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_bgl		BGL管理システム
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void MNGM_PLAYER_PLATE_HBLANK_BGSCR_Init( MNGM_HBLANK_PLATEBGSCR* p_wk, MNGM_BGL* p_bgl, u32 heapID )
{
	int i, j;
	
	GF_ASSERT( p_wk->p_laster == NULL );

	p_wk->p_bgl = p_bgl;

	// 初期スクロール座標設定
	GF_BGL_ScrollSet( p_wk->p_bgl->p_bgl, GF_BGL_FRAME0_M, GF_BGL_SCROLL_X_SET, MNGM_PLATE_MOVE_START );
	GF_BGL_ScrollSet( p_wk->p_bgl->p_bgl, GF_BGL_FRAME1_M, GF_BGL_SCROLL_X_SET, MNGM_PLATE_MOVE_START );

	// bufferの値を初期化
	for( i=0; i<2; i++ ){
		MI_CpuFill16( &p_wk->scrll_x[i], MNGM_PLATE_MOVE_START, sizeof(s16)*WFLBY_HBLANK_BGSCR_BUFNUM );
	}

	p_wk->p_laster = LASTER_Init( heapID, &p_wk->scrll_x[0], &p_wk->scrll_x[1] );

	// 最初はスイッチbufferOFF
	LASTER_SwitchFlagSet( p_wk->p_laster, LASTER_SWITCH_OFF );

	// HBLANK関数設定
	sys_HBlankIntrSet( MNGM_PLAYER_PLATE_HBLANK_BGSCR_HBlank, p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーPLATE　HBLANK処理	破棄
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static void MNGM_PLAYER_PLATE_HBLANK_BGSCR_Exit( MNGM_HBLANK_PLATEBGSCR* p_wk )
{
	if( p_wk->p_laster != NULL ){
		sys_HBlankIntrSet( NULL, NULL );

		LASTER_Delete( p_wk->p_laster );
		p_wk->p_laster = NULL;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーPLATE	HBLANK処理	値設定
 *
 *	@param	p_wk		ワーク
 *	@param	cp_plate	PLATEワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_PLAYER_PLATE_HBLANK_BGSCR_SetPlateScr( MNGM_HBLANK_PLATEBGSCR* p_wk, const MNGM_PLATE_PLAYER* cp_plate )
{
	s16* p_buff;
	int i;
	int idx;

	GF_ASSERT( p_wk->p_laster != NULL );

	p_buff = LASTER_GetWriteBuff( p_wk->p_laster );

	//  座標設定
	for( i=0; i<MNGM_PLAYER_PLATE_SCRN_SIZ_Y*8; i++ ){
		idx = i-1;
		idx += sc_MNGM_PLAYER_PLATE_DATA[ cp_plate->playernum-1 ].top[ cp_plate->plidx ]*8;
		if( idx < 0 ){
			idx += WFLBY_HBLANK_BGSCR_BUFNUM;
		}
		p_buff[idx] = cp_plate->draw_x.x>>FX32_SHIFT;
	}

	// bufferを書き換えたのでスイッチbuffer
	LASTER_SwitchFlagSet( p_wk->p_laster, LASTER_SWITCH_ON );
}

//----------------------------------------------------------------------------
/**
 *	@brief	HBLANKシステムVBLANK処理
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_PLAYER_PLATE_HBLANK_BGSCR_VBlank( MNGM_HBLANK_PLATEBGSCR* p_wk )
{
	if( p_wk->p_laster == NULL ){
		return ;
	}

	// bufferのスイッチング
	LASTER_VBlank( p_wk->p_laster );

	// bufferをスイッチしたので次の書き換え完了までスイッチOFF
	LASTER_SwitchFlagSet( p_wk->p_laster, LASTER_SWITCH_OFF );
}

//----------------------------------------------------------------------------
/**
 *	@brief	HBLANK処理
 *
 *	@param	p_work
 */
//-----------------------------------------------------------------------------
static void MNGM_PLAYER_PLATE_HBLANK_BGSCR_HBlank( void* p_work )
{
	MNGM_HBLANK_PLATEBGSCR* p_wk = p_work;
	int v_count;
	const s16* cp_buff;
	
	v_count = GX_GetVCount();
	if( v_count >= WFLBY_HBLANK_BGSCR_BUFNUM ){
		return ;
	}

	cp_buff = LASTER_GetReadBuff( p_wk->p_laster );

#if PL_T0849_080709_FIX
	if( GX_IsHBlank() ){
		GF_BGL_ScrollSet( p_wk->p_bgl->p_bgl, GF_BGL_FRAME0_M, GF_BGL_SCROLL_X_SET, cp_buff[v_count] );
		GF_BGL_ScrollSet( p_wk->p_bgl->p_bgl, GF_BGL_FRAME1_M, GF_BGL_SCROLL_X_SET, cp_buff[v_count] );
	}
#else
	GF_BGL_ScrollSet( p_wk->p_bgl->p_bgl, GF_BGL_FRAME0_M, GF_BGL_SCROLL_X_SET, cp_buff[v_count] );
	GF_BGL_ScrollSet( p_wk->p_bgl->p_bgl, GF_BGL_FRAME1_M, GF_BGL_SCROLL_X_SET, cp_buff[v_count] );
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	通信プレイヤーデータを設定
 *
 *	@param	p_wk			ワーク
 *	@param	cp_comm_param	プレイヤーIDに対応したNETID
 */
//-----------------------------------------------------------------------------
static void MNGM_COMMPDATA_Init( MNGM_COMM_PDATA* p_wk, const MNGM_ENRES_PARAM* cp_comm_param )
{
	int i;

	for( i=0; i<cp_comm_param->num; i++ ){
		p_wk->cp_status[i]	= MNGM_ENRES_PARAM_GetMystatus( cp_comm_param, cp_comm_param->pnetid[i] );
		GF_ASSERT(  p_wk->cp_status[i] );
		p_wk->nation[i]		= CommInfoGetMyNation( cp_comm_param->pnetid[i] );
		p_wk->area[i]		= CommInfoGetMyArea( cp_comm_param->pnetid[i] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	国名で表示するのかチェック
 *
 *	@param	cp_wk	パラメータ
 *	@param	cp_data	通信基本データ
 *
 *	@retval	TRUE	国名を表示
 *	@retval	FALSE	地域名を表示
 */
//-----------------------------------------------------------------------------
static BOOL MNGM_COMMPARAM_CheckDrawNation( const MNGM_ENRES_PARAM* cp_wk, const MNGM_COMM_PDATA* cp_data )
{
	int i;
	BOOL draw_nation;	// 国IDで地域を表示するか

	draw_nation = FALSE;
	for( i=0; i<cp_wk->num-1; i++ ){
		// みんなの国が違うか、国がダミー（country000）のとき国名で表示
		if( (cp_data->nation[i] != cp_data->nation[i+1]) || 
			(cp_data->nation[i] == country000) || 
			(cp_data->nation[i+1] == country000) ){
			draw_nation = TRUE;
		}
	}

	return draw_nation;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ガジェットアップデート
 *
 *	@param	data	今のガジェットデータ
 *
 *	@return	Update後のガジェットデータ
 */
//-----------------------------------------------------------------------------
WFLBY_ITEMTYPE MNGM_ITEM_Update( WFLBY_ITEMTYPE data )
{
	u32 gadget_type;
	u32 gadget_offs;
	WFLBY_ITEMTYPE ret_gadget;

	gadget_offs = data % WFLBY_ITEM_DANKAI;
	gadget_type = data / WFLBY_ITEM_DANKAI;
	if( (gadget_offs+1) < WFLBY_ITEM_DANKAI ){
		gadget_offs ++;
	}
	
	ret_gadget = gadget_type * WFLBY_ITEM_DANKAI;
	ret_gadget += gadget_offs;

	return ret_gadget;
}


//----------------------------------------------------------------------------
/**
 *	@brief	タイトルロゴ	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_bglwk		BGLワーク
 *	@param	gametype	ゲームタイプ
 *	@param	p_handle	ハンドル
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void MNGM_TITLELOGO_Init( MNGM_TITLE_LOGO* p_wk, MNGM_BGL* p_bglwk, MNGM_MSG* p_msg, u32 gametype, ARCHANDLE* p_handle, u32 heapID )
{
	GF_BGL_BmpWinAdd( p_bglwk->p_bgl, &p_wk->bmp, GF_BGL_FRAME1_M,
			MNGM_TITLELOGO_BMP_X, MNGM_TITLELOGO_BMP_Y,
			MNGM_TITLELOGO_BMP_SIZX, MNGM_TITLELOGO_BMP_SIZY,
			MNGM_TITLELOGO_BMP_PAL, MNGM_TITLELOGO_BMP_CGX);
	GF_BGL_BmpWinDataFill( &p_wk->bmp, 0 );

	// メッセージを生成
	p_wk->p_str = STRBUF_Create( MNGM_MSG_STRBUF_NUM, heapID );
	MNGM_MSG_SetGameName( p_msg, gametype );
	MNGM_MSG_GetStr( p_msg, p_wk->p_str, msg_16 );

	p_wk->gametype = gametype;

	// 描画OFF
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );	

	// キャラクタスクリーン転送
	ArcUtil_HDL_BgCharSet( p_handle, NARC_wlmngm_tool_minigame_win_poke_NCGR,
			p_bglwk->p_bgl, 
			GF_BGL_FRAME0_M, 0, 0,
			FALSE, heapID );
	ArcUtil_HDL_ScrnSet( p_handle, NARC_wlmngm_tool_minigame_win_bg2_NSCR, p_bglwk->p_bgl,
			GF_BGL_FRAME0_M, 0, 0, FALSE, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイトルロゴ	破棄
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_TITLELOGO_Exit( MNGM_TITLE_LOGO* p_wk )
{
	GF_BGL_BmpWinDel( &p_wk->bmp );

	STRBUF_Delete( p_wk->p_str );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ロゴの入場処理開始
 *
 *	@param	p_wk		ワーク
 *	@param	heapID		ヒープID
 */	
//-----------------------------------------------------------------------------
static void MNGM_TITLELOGO_InStart( MNGM_TITLE_LOGO* p_wk, MNGM_BGL* p_bglwk, u32 heapID )
{
	u32 x_size;
	u32 draw_x;

	// 動作パラメータの初期化
	p_wk->count = 0;
	p_wk->seq = 0;
	MNGM_AddMoveReqFx( &p_wk->move, 
			sc_MNGM_TITLELOGO_MOVE00_START[p_wk->gametype], 
			sc_MNGM_TITLELOGO_MOVE00_END[p_wk->gametype],
			sc_MNGM_TITLELOGO_MOVE00_SPEED[p_wk->gametype],
			MNGM_TITLELOGO_MOVE00_COUNTMAX );

	// 初期位置設定
	MNGM_TITLELOGO_SetMoveMatrix( p_wk, p_bglwk, p_wk->move.x>>FX32_SHIFT );


	
	// ボタンフォントで書き込む
	FontProc_LoadFont( FONT_BUTTON, heapID );	//ボタンフォントのロード

	// 中央に表示する
	x_size	= FontProc_GetPrintStrWidth( FONT_BUTTON, p_wk->p_str, 0 );
	draw_x	= (MNGM_TITLELOGO_BMP_SIZXDOT/2) - (x_size/2);

	GF_STR_PrintColor(
		&p_wk->bmp, FONT_BUTTON, p_wk->p_str, draw_x,
		0, MSG_ALLPUT, sc_MNGM_TITLELOGO_BMP_COL[ p_wk->gametype ], NULL );

	FontProc_UnloadFont( FONT_BUTTON );				//ボタンフォントの破棄
	
	// 描画ON
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );	
}

//----------------------------------------------------------------------------
/**
 *	@brief	退場の処理開始
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_TITLELOGO_OutStart( MNGM_TITLE_LOGO* p_wk, MNGM_BGL* p_bglwk )
{
	// 動作パラメータの初期化
	p_wk->count = 0;
	p_wk->seq = 0;
	MNGM_AddMoveReqFx( &p_wk->move, 
			sc_MNGM_TITLELOGO_MOVE00_END[p_wk->gametype], 
			sc_MNGM_TITLELOGO_MOVE03_END[p_wk->gametype],
			sc_MNGM_TITLELOGO_MOVE03_SPEED[p_wk->gametype],
			MNGM_TITLELOGO_MOVE03_COUNTMAX );

	// 初期位置設定
	MNGM_TITLELOGO_SetMoveMatrixVReq( p_wk, p_bglwk, p_wk->move.x>>FX32_SHIFT );
}


//----------------------------------------------------------------------------
/**
 *	@brief	タイトルロゴ	メイン
 *
 *	@param	p_wk	ワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
static BOOL MNGM_TITLELOGO_InMain( MNGM_TITLE_LOGO* p_wk, MNGM_BGL* p_bglwk )
{
	BOOL result;
	
	switch( p_wk->seq ){
	case MNGM_TITLELOGO_INMOVESEQ_IN:
		result = MNGM_AddMoveMainFx( &p_wk->move, p_wk->count );
		MNGM_TITLELOGO_SetMoveMatrixVReq( p_wk, p_bglwk, p_wk->move.x>>FX32_SHIFT );
		p_wk->count ++;
		if( result == TRUE ){
			//  
			Snd_SePlay( MNGM_SND_TITLE );

			// 次の動作の初期化
			MNGM_AddMoveReqFx( &p_wk->move, 
					sc_MNGM_TITLELOGO_MOVE00_END[p_wk->gametype], 
					sc_MNGM_TITLELOGO_MOVE01_END[p_wk->gametype],
					sc_MNGM_TITLELOGO_MOVE01_SPEED[p_wk->gametype],
					MNGM_TITLELOGO_MOVE01_COUNTMAX );
			p_wk->seq++;	
			p_wk->count = 0;
		}
		break;

	case MNGM_TITLELOGO_INMOVESEQ_BOUND00:
		result = MNGM_AddMoveMainFx( &p_wk->move, p_wk->count );
		MNGM_TITLELOGO_SetMoveMatrixVReq( p_wk, p_bglwk, p_wk->move.x>>FX32_SHIFT );
		p_wk->count ++;
		if( result == TRUE ){
			// 次の動作の初期化
			MNGM_AddMoveReqFx( &p_wk->move, 
					sc_MNGM_TITLELOGO_MOVE01_END[p_wk->gametype], 
					sc_MNGM_TITLELOGO_MOVE00_END[p_wk->gametype],
					sc_MNGM_TITLELOGO_MOVE02_SPEED[p_wk->gametype],
					MNGM_TITLELOGO_MOVE02_COUNTMAX );
			p_wk->seq++;	
			p_wk->count = 0;
		}
		break;

	case MNGM_TITLELOGO_INMOVESEQ_BOUND01:
		result = MNGM_AddMoveMainFx( &p_wk->move, p_wk->count );
		MNGM_TITLELOGO_SetMoveMatrixVReq( p_wk, p_bglwk, p_wk->move.x>>FX32_SHIFT );
		p_wk->count ++;
		if( result == TRUE ){
			p_wk->seq++;
		}
		break;

	case MNGM_TITLELOGO_INMOVESEQ_END:
		return TRUE;
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイトルロゴ	アウトメイン
 *
 *	@param	p_wk	ワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
static BOOL MNGM_TITLELOGO_OutMain( MNGM_TITLE_LOGO* p_wk, MNGM_BGL* p_bglwk )
{
	BOOL result;
	
	switch( p_wk->seq ){
	case MNGM_TITLELOGO_OUTMOVESEQ_OUT:
		result = MNGM_AddMoveMainFx( &p_wk->move, p_wk->count );
		MNGM_TITLELOGO_SetMoveMatrixVReq( p_wk, p_bglwk, p_wk->move.x>>FX32_SHIFT );
		p_wk->count ++;
		if( result == TRUE ){
			p_wk->seq++;
		}
		break;
		
	case MNGM_TITLELOGO_OUTMOVESEQ_END:
		GF_BGL_BmpWinOff( &p_wk->bmp );

		// 描画OFF
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );	

		// 初期位置設定
		MNGM_TITLELOGO_SetMoveMatrix( p_wk, p_bglwk, 0 );

		// BG面をクリーン
		GF_BGL_ScrClear( p_bglwk->p_bgl, GF_BGL_FRAME0_M );
		GF_BGL_ScrClear( p_bglwk->p_bgl, GF_BGL_FRAME1_M );

		// 描画ON
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );	
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	動作座標を設定する
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_TITLELOGO_SetMoveMatrix( MNGM_TITLE_LOGO* p_wk, MNGM_BGL* p_bglwk, s32 num )
{
	switch( p_wk->gametype ){

	case WFLBY_GAME_BALLSLOW:	// 玉投げ
	case WFLBY_GAME_BALANCEBALL:	// 玉乗り
		GF_BGL_ScrollSet( p_bglwk->p_bgl, GF_BGL_FRAME0_M, 
				GF_BGL_SCROLL_X_SET, num );
		GF_BGL_ScrollSet( p_bglwk->p_bgl, GF_BGL_FRAME1_M, 
				GF_BGL_SCROLL_X_SET, num );
		break;

	case WFLBY_GAME_BALLOON:		// ふうせんわり
		GF_BGL_ScrollSet( p_bglwk->p_bgl, GF_BGL_FRAME0_M, 
				GF_BGL_SCROLL_Y_SET, num );
		GF_BGL_ScrollSet( p_bglwk->p_bgl, GF_BGL_FRAME1_M, 
				GF_BGL_SCROLL_Y_SET, num );
		break;

	default:
		GF_ASSERT(0);
		break;
	}
}
static void MNGM_TITLELOGO_SetMoveMatrixVReq( MNGM_TITLE_LOGO* p_wk, MNGM_BGL* p_bglwk, s32 num )
{
	switch( p_wk->gametype ){

	case WFLBY_GAME_BALLSLOW:	// 玉投げ
	case WFLBY_GAME_BALANCEBALL:	// 玉乗り
		GF_BGL_ScrollReq( p_bglwk->p_bgl, GF_BGL_FRAME0_M, 
				GF_BGL_SCROLL_X_SET, num );
		GF_BGL_ScrollReq( p_bglwk->p_bgl, GF_BGL_FRAME1_M, 
				GF_BGL_SCROLL_X_SET, num );
		break;

	case WFLBY_GAME_BALLOON:		// ふうせんわり
		GF_BGL_ScrollReq( p_bglwk->p_bgl, GF_BGL_FRAME0_M, 
				GF_BGL_SCROLL_Y_SET, num );
		GF_BGL_ScrollReq( p_bglwk->p_bgl, GF_BGL_FRAME1_M, 
				GF_BGL_SCROLL_Y_SET, num );
		break;

	default:
		GF_ASSERT(0);
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	背景パレット転送
 *
 *	@param	p_handle		ハンドル
 *	@param	gametype		ゲームタイプ
 */
//-----------------------------------------------------------------------------
static void MNGM_BACKPLTT_Trans( ARCHANDLE* p_handle, u32 gametype, u32 heapID )
{
	u32 palnum;
	//  ゲームタイプに合わせて背景色を変える
	switch( gametype ){
	case WFLBY_GAME_BALLSLOW:	// 玉投げ
		palnum = MNGM_PLAYER_BG_ANMPLTT_BALLSLOW;
		break;
	case WFLBY_GAME_BALANCEBALL:	// 玉乗り
		palnum = MNGM_PLAYER_BG_ANMPLTT_BALANCE;
		break;
	case WFLBY_GAME_BALLOON:		// ふうせんわり
		palnum = MNGM_PLAYER_BG_ANMPLTT_BALLOON;
		break;

	default:
		palnum = MNGM_PLAYER_BG_ANMPLTT_BALLSLOW;
		break;
	}

	ArcUtil_HDL_PalSetEx( p_handle, NARC_wlmngm_tool_minigame_win_ani_NCLR, 
			PALTYPE_MAIN_BG, palnum*32, MNGM_PLAYER_BG_PLTT_BACK*32, 32, heapID );

	ArcUtil_HDL_PalSetEx( p_handle, NARC_wlmngm_tool_minigame_win_ani_NCLR, 
			PALTYPE_SUB_BG, palnum*32, MNGM_PLAYER_BG_PLTT_BACK*32, 32, heapID );
}




//-------------------------------------
///	リザルト画面ワーク
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	リザルト画面ワーク	作成
 *
 *	@param	cp_commparam		共通パラメータ
 *	@param	cp_param			パラメータ
 *	@param	heapID				ヒープID
 */
//-----------------------------------------------------------------------------
static MNGM_RESULTWK* MNGM_RESULT_CommonInit( const MNGM_ENRES_PARAM* cp_commparam, const MNGM_RESULT_PARAM* cp_param, u32 gametype, u32 heapID )
{
	MNGM_RESULTWK* p_wk;
	int i;

	p_wk = sys_AllocMemory( heapID, sizeof(MNGM_RESULTWK) );
	memset( p_wk, 0, sizeof(MNGM_RESULTWK) );

	// パラメータをコピー
	memcpy( &p_wk->param, cp_param, sizeof(MNGM_RESULT_PARAM) );
	memcpy( &p_wk->comm_param, cp_commparam, sizeof(MNGM_ENRES_PARAM) );

	// ゲームタイプ設定
	p_wk->gametype = gametype;

	// 通信コマンド設定
	CommCommandMNGMInitialize( p_wk );

	// レジスタリセット
	G2_BlendNone();
	G2S_BlendNone();
	GX_SetVisibleWnd(0);
	GXS_SetVisibleWnd(0);
	

	// バンク設定
	GF_Disp_SetBank( &sc_MNGM_RESULT_BANK );

	// ヒープID保存
	p_wk->heapID = heapID;

	// 通信データを設定
	MNGM_COMMPDATA_Init( &p_wk->comm_pdata, &p_wk->comm_param );

	// VramTransferManager初期化
	initVramTransferManagerHeap( MNGM_VRAMTR_WK_NUM, heapID );


	// BG
	MNGM_BGL_Init( &p_wk->bgl, &sc_MNGM_RESULT_BGINIT, sc_MNGM_RESULT_BGCNT, MNGM_RESULT_BGL_USE, heapID );

	// OAM
	MNGM_CLACT_Init( &p_wk->clact, MNGM_RESULT_CLACT_WKNUM, MNGM_RESULT_CLACT_CHARNUM, MNGM_RESULT_CLACT_PLTTNUM, heapID );

	// CLACT
	MNGM_CLACTRES_Init( &p_wk->clres, MNGM_RESULT_CLACT_RESNUM, heapID );

	// メッセージシステム作成
	MNGM_MSG_Init( &p_wk->msg, heapID );	

	// グラフィック読み込み
	MNGM_RESULT_GraphicLoad( p_wk, heapID );

	// メッセージシステム初期化
	{
		BOOL vip;
		u32 netid;

		netid = MNGM_ENRES_PARAM_GetPlNOtoNetID( &p_wk->comm_param, p_wk->comm_param.my_playerid );
		vip = MNGM_ENRES_PARAM_GetVipFlag( &p_wk->comm_param, netid );
		MNGM_TALKWIN_Init( &p_wk->talkwin, &p_wk->bgl, p_wk->comm_param.p_save, 
				vip,  heapID );
	}

	// リトライワーク作成
	MNGM_RESULT_Retry_Init( &p_wk->retrysys, &p_wk->bgl, p_wk->comm_param.vchat, heapID );

	// 国IDで地域を表示するかチェック
	{
		int i;

		// 国名で表示するかチェック
		p_wk->draw_nation = MNGM_COMMPARAM_CheckDrawNation( &p_wk->comm_param, &p_wk->comm_pdata );
 
	}

	// ゲームによって変更する部分
	// プレイヤーIDで配置を変更するか設定
	// タスク生成
	switch( p_wk->gametype ){
	case WFLBY_GAME_BALLSLOW:	// 玉投げ
	case WFLBY_GAME_BALANCEBALL:	// 玉乗り
		p_wk->tcb			 = TCB_Add( MNGM_RESULT_Tcb_BallSlowBalanceBall, p_wk, 0 );
		break;
		
	case WFLBY_GAME_BALLOON:		// ふうせんわり
		p_wk->tcb			 = TCB_Add( MNGM_RESULT_Tcb_Balloon, p_wk, 0 );
		break;

	default:
		GF_ASSERT(0);	// おかしい
		p_wk->tcb			 = TCB_Add( MNGM_RESULT_Tcb_BallSlowBalanceBall, p_wk, 0 );
		break;
	}
		
	p_wk->tcb_vwait = VWaitTCB_Add( MNGM_RESULT_VWaitTcb, p_wk, 0 );

	// VCHAT ON
	if( p_wk->comm_param.vchat ){
		// ボイスチャット開始
		mydwc_startvchat( heapID );
	}
	
	return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	3Dシステム初期化
 *
 *	@param	p_wk		ワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void MNGM_RESULT_3DInit( MNGM_3DSYS* p_wk, u32 heapID )
{
	p_wk->p_3dman = GF_G3DMAN_Init( heapID, GF_G3DMAN_LNK, GF_G3DTEX_128K, 
				GF_G3DMAN_LNK, GF_G3DPLT_64K, NULL );
	Particle_SystemWorkInit();		// パーティクル初期化
}

//----------------------------------------------------------------------------
/**
 *	@brief	３Dシステム破棄
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_RESULT_3DExit( MNGM_3DSYS* p_wk )
{
	Particle_SystemExitAll();		//  パーティクル破棄

    GF_G3D_Exit( p_wk->p_3dman );
}

//----------------------------------------------------------------------------
/**
 *	@brief	３Dシステムメイン
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_RESULT_3DMain( MNGM_3DSYS* p_wk )
{
	Particle_CalcAll();
}

//----------------------------------------------------------------------------
/**
 *	@brief	３Dシステム描画
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_RESULT_3DDraw( MNGM_3DSYS* p_wk )
{
    //３Ｄ描画開始
    GF_G3X_Reset();
	
	// パーティクル表示処理
	Particle_DrawAll();

    /* ジオメトリ＆レンダリングエンジン関連メモリのスワップ */
    GF_G3_RequestSwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_Z);
}

//----------------------------------------------------------------------------
/**
 *	@brief	３Dシステム	パーティクルアニメ読み込み
 *
 *	@param	p_wk		ワーク
 *	@param	p_handle	ハンドル
 *	@param	dataidx		データインデックス
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void MNGM_RESULT_3DAnmLoad( MNGM_3DSYS* p_wk, ARCHANDLE* p_handle, u32 dataidx, u32 heapID )
{
	void* p_res;
	GF_CAMERA_PTR p_camera;

	// パーティクルワーク作成
	p_wk->p_ptc_work = sys_AllocMemory( heapID, PARTICLE_LIB_HEAP_SIZE );
	p_wk->p_ptc = Particle_SystemCreate( 
			NULL, NULL, 
			p_wk->p_ptc_work, PARTICLE_LIB_HEAP_SIZE, 
			TRUE, heapID );
	GF_ASSERT( p_wk->p_ptc );


	// カメラ設定
	Particle_CameraTypeSet(p_wk->p_ptc, GF_CAMERA_ORTHO);
	p_camera = Particle_GetCameraPtr( p_wk->p_ptc );
	GFC_SetCameraClip(FX32_ONE, FX32_ONE * 900, p_camera);

	// リソース読み込み＆設定
	p_res = ArcUtil_HDL_Load(p_handle, dataidx, FALSE, heapID, ALLOC_TOP);
	Particle_ResourceSet(p_wk->p_ptc, p_res, PTC_AUTOTEX_LNK|PTC_AUTOPLTT_LNK, FALSE);
}

//----------------------------------------------------------------------------
/**
 *	@brief	３Dシステム	パーティクルアニメ破棄
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_RESULT_3DAnmRelease( MNGM_3DSYS* p_wk )
{
	Particle_SystemExit( p_wk->p_ptc );
	sys_FreeMemoryEz( p_wk->p_ptc_work );
	p_wk->p_ptc			= NULL;
	p_wk->p_ptc_work	= NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	３Dシステム	パーティクルアニメ開始
 *
 *	@param	p_wk		ワーク
 *	@param	emitnum		エミッタ数
 *	@param	p_pos		位置
 */
//-----------------------------------------------------------------------------
static void MNGM_RESULT_3DAnmStart( MNGM_3DSYS* p_wk, u32 emitnum )
{
	int i;
	VecFx32 pos = {0,0,0};

	// エミッタの登録
	Particle_CreateEmitter( p_wk->p_ptc, emitnum, &pos );
}

//----------------------------------------------------------------------------
/**
 *	@brief	３Dシステム	パーティクルアニメ	終了チェック
 *
 *	@param	cp_wk		ワーク
 */
//-----------------------------------------------------------------------------
static BOOL MNGM_RESULT_3DAnmEndCheck( const MNGM_3DSYS* cp_wk )
{
	// 動作中のエミッタがなくなれば動作終了
	if( Particle_GetEmitterNum(cp_wk->p_ptc) == 0 ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	リザルト画面グラフィック読み込み
 *
 *	@param	p_wk		ワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void MNGM_RESULT_GraphicLoad( MNGM_RESULTWK* p_wk, u32 heapID )
{
	ARCHANDLE* p_handle;

	p_handle = ArchiveDataHandleOpen( ARC_WLMNGM_TOOL_GRA, heapID );

	// パレット
	ArcUtil_HDL_PalSet( p_handle, NARC_wlmngm_tool_minigame_win_NCLR,
			PALTYPE_MAIN_BG, 0, MNGM_BGPLTT_NUM*32, heapID );
	ArcUtil_HDL_PalSet( p_handle, NARC_wlmngm_tool_minigame_win_NCLR,
			PALTYPE_SUB_BG, 0, MNGM_BGPLTT_NUM*32, heapID );
	
	// キャラクタ	
	ArcUtil_HDL_BgCharSet( p_handle, NARC_wlmngm_tool_minigame_win_bg_NCGR,
			p_wk->bgl.p_bgl, 
			GF_BGL_FRAME1_M, 0, 0,
			FALSE, heapID );
	ArcUtil_HDL_BgCharSet( p_handle, NARC_wlmngm_tool_minigame_win_bg_NCGR,
			p_wk->bgl.p_bgl, 
			GF_BGL_FRAME1_S, 0, 0x4000,
			FALSE, heapID );

	// スクリーン
	{
		int i;
		for( i=0; i<MNGM_RESULT_SCRN_NUM; i++ ){
			p_wk->p_scrnbuf[i] =  ArcUtil_HDL_ScrnDataGet( p_handle,
					NARC_wlmngm_tool_minigame_win_bg1_NSCR+i,
					FALSE, &p_wk->p_scrn[i], heapID );
		}
	}

	// 背景設定
	ArcUtil_HDL_ScrnSet( p_handle, NARC_wlmngm_tool_minigame_win_bg0_NSCR, p_wk->bgl.p_bgl,
			GF_BGL_FRAME3_M, 0, 0, FALSE, heapID );
	ArcUtil_HDL_ScrnSet( p_handle, NARC_wlmngm_tool_minigame_win_bg0_NSCR, p_wk->bgl.p_bgl,
			GF_BGL_FRAME1_S, 0, 0, FALSE, heapID );

	// 背景用パレット転送
	MNGM_BACKPLTT_Trans( p_handle, p_wk->gametype, heapID );


	// OAMを読み込む
	p_wk->p_resobj = MNGM_CLACTRES_Load( &p_wk->clres, p_handle, 
			NARC_wlmngm_tool_minigame_win_oam_NCLR, MNGM_PLAYER_OAM_PLTT_NUM,
			NARC_wlmngm_tool_minigame_win_oam_NCGR,
			NARC_wlmngm_tool_minigame_win_oam_NCER,
			NARC_wlmngm_tool_minigame_win_oam_NANR,
			MNGM_RESCONTID, heapID );

	// パレットアニメエフェクト
	MNGM_RESULT_PalTrEffectInit( p_wk, p_handle, heapID );


	// BALLOON
	MNGM_RESULT_Balloon_GraphicInit( &p_wk->balloon, &p_wk->bgl, &p_wk->clact,
			&p_wk->clres, p_handle, heapID );

	ArchiveDataHandleClose( p_handle );
}

//----------------------------------------------------------------------------
/**
 *	@brief	結果画面用グラフィック破棄
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_RESULT_GraphicDelete( MNGM_RESULTWK* p_wk )
{
	// BALLOON
	MNGM_RESULT_Balloon_GraphicExit( &p_wk->balloon );
	
	// スクリーンを破棄
	{
		int i;
		for( i=0; i<MNGM_RESULT_SCRN_NUM; i++ ){
			sys_FreeMemoryEz( p_wk->p_scrnbuf[i] );
		}
	}

	// パレットアニメエフェクトを破棄
	MNGM_RESULT_PalTrEffectExit( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	結果画面メインタスク
 *
 *	@param	tcb
 *	@param	p_work 
 */
//-----------------------------------------------------------------------------
static void MNGM_RESULT_Tcb_BallSlowBalanceBall( TCB_PTR tcb, void* p_work )
{
	MNGM_RESULTWK* p_wk = p_work;
	BOOL result;

	switch( p_wk->seq ){
	case MNGM_RESULT_BALLSLOW_SEQ_WIPEIN_INIT:

		// アップデート前のガジェットを求める
		p_wk->last_gadget = p_wk->comm_param.p_lobby_wk->gadget;

		// replayフラグがTRUEならWiFi友達部屋なのでガジェットはアップデートしない
		if( p_wk->comm_param.replay == TRUE ){
			p_wk->gadget_update = FALSE;
		}else{
			u32 notouch_score;

			// 何もしてないときのスコアを取得
			notouch_score = MNGM_RESULT_BallslowBalanceGetNoTouchScore( p_wk );

			OS_TPrintf( "score %d\n", p_wk->param.score[ p_wk->comm_param.my_playerid ] );
			
			// ガジェットアップデートチェック
			// 自分が順位１位だったらガジェットをアップデート
			// スコアがnotouch_scoreより大きい必要がある
			if( (p_wk->param.result[ p_wk->comm_param.my_playerid ] == 0) && 
				(p_wk->param.score[ p_wk->comm_param.my_playerid ] > notouch_score) ){
				// ガジェットアップデート
				p_wk->comm_param.p_lobby_wk->gadget = MNGM_ITEM_Update( p_wk->comm_param.p_lobby_wk->gadget );
				p_wk->gadget_update = TRUE;

			}else{
				p_wk->gadget_update = FALSE;
			}

			// 親はトピック作成
			if( p_wk->comm_param.my_playerid == 0 ){
				MNGM_RESULT_BallslowBalance_SendMinigameTopResult( p_wk );
			}
		}



		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEIN, 
				WIPE_TYPE_FADEIN, WIPE_FADE_OUTCOLOR, 
				WIPE_DEF_DIV, WIPE_DEF_SYNC, p_wk->heapID );
		p_wk->seq ++;
		break;
		
	case MNGM_RESULT_BALLSLOW_SEQ_WIPEIN_WAIT:
		result = WIPE_SYS_EndCheck();
		if( result ){
			p_wk->seq ++;
		}
		break;

	case MNGM_RESULT_BALLSLOW_SEQ_MSGON_INIT:
		MNGM_TALKWIN_MsgPrint( &p_wk->talkwin, &p_wk->msg, msg_01, MNGM_TALKWIN_IDX_MAIN );
		p_wk->seq ++;
		break;
		
	case MNGM_RESULT_BALLSLOW_SEQ_MSGON_WAIT:
		result = MNGM_TALKWIN_MsgEndCheck( &p_wk->talkwin, MNGM_TALKWIN_IDX_MAIN );
		if( result ){
			p_wk->wait = MNGM_ENTRY_MSGWAIT;
			p_wk->seq ++;
		}
		break;

	case MNGM_RESULT_BALLSLOW_SEQ_MSGON_WAIT2:
		if( p_wk->wait > 0 ){
			p_wk->wait --;
			if( p_wk->wait == 0 ){
				int i;
				BOOL all_score_0 = TRUE;
				u32 notouch_score;


				MNGM_TALKWIN_MsgOff( &p_wk->talkwin, MNGM_TALKWIN_IDX_MAIN );

				// HBLANKシステム初期化
				MNGM_PLAYER_PLATE_HBLANK_BGSCR_Init( &p_wk->bgscrl, &p_wk->bgl, p_wk->heapID );

				// 何もしてないときのスコアを取得
				notouch_score = MNGM_RESULT_BallslowBalanceGetNoTouchScore( p_wk );

				// 全員のスコアがnotouch_scoreのときは全員をp_wk->comm_param.num位として表示する
				for( i=0; i<p_wk->comm_param.num; i++ ){
					if( p_wk->param.score[ i ] > notouch_score ){
						all_score_0 = FALSE;
					}
				}
				if( all_score_0 == TRUE ){
					p_wk->draw_result_wait	= MNGM_RESULT_DRAWON_WAIT_34;
					p_wk->seq				= MNGM_RESULT_BALLSLOW_SEQ_ALLSCORE0_WAIT;
				}else{

					// そうでなければ４位から表示
					p_wk->seq ++;
					p_wk->draw_result = 4;
				}
			}
		}
		break;
		
	// 次に表示する順位を検索する
	case MNGM_RESULT_BALLSLOW_SEQ_COUNTDOWN_INIT:
		{
			int i;
			BOOL sarch;
			
			sarch = FALSE;
			do{
				p_wk->draw_result --;	// 次の表示順位にする
				// 次に表示する順位を検索する
				for( i=0; i<p_wk->comm_param.num; i++ ){
					if( p_wk->param.result[ i ] == p_wk->draw_result ){
						sarch = TRUE;;
					}
				}
			}while( sarch == FALSE );

			// 表示までのウエイトを設定
			switch( p_wk->draw_result ){
			case 3:
			case 2:
				p_wk->draw_result_wait = MNGM_RESULT_DRAWON_WAIT_34;
				break;

			case 1:
			case 0:
				p_wk->draw_result_wait = MNGM_RESULT_DRAWON_WAIT_12;
				break;

			default :
				GF_ASSERT(0);	// おかしい
				break;
			}

			// テーブル動作
			MNGM_RESULT_PlayerTblMove( p_wk );

			p_wk->seq ++;
		}
		break;

	// 順位の表示
	// 2位と1位は同時に表示する
	case MNGM_RESULT_BALLSLOW_SEQ_COUNTDOWN:
		p_wk->draw_result_wait --;

		// テーブル動作
		MNGM_RESULT_PlayerTblMove( p_wk );

		if( p_wk->draw_result_wait > 0 ){
			break;
		}
		
		// 表示処理
		{
			int i;
			BOOL draw;

			for( i=0; i<p_wk->comm_param.num; i++ ){
				draw = FALSE;
				if( p_wk->param.result[i] == p_wk->draw_result ){
					draw = TRUE;
				}else{
					if( (p_wk->draw_result == 1) && 
						(p_wk->param.result[i] == 0) ){
						draw = TRUE;
					}
				}

				// 表示
				if( draw ){
					MNGM_RESULT_BallslowBalancePlayerTblDraw( p_wk, i, p_wk->param.result[i] );
				}
			}
		}

		// まだ2位を表示してなければ続ける
		if( p_wk->draw_result > 1 ){
			p_wk->seq  = MNGM_RESULT_BALLSLOW_SEQ_COUNTDOWN_INIT;
		}else{
			p_wk->seq ++;
		}

		break;

	// テーブル動作が全部終わるのを待つ
	case MNGM_RESULT_BALLSLOW_SEQ_TBLMOVE_WAIT:
		if( MNGM_RESULT_PlayerTblMove( p_wk ) ){
			p_wk->seq++;
		}
		break;
		
	case MNGM_RESULT_BALLSLOW_SEQ_EFFECT_ON:
		p_wk->seq ++;

		// 1位の人の場所にエフェクトを出す
		MNGM_RESULT_PalTrEffectStart( p_wk );

		p_wk->wait = MNGM_RESULT_RESULT_DRAW_WAIT;
		break;

	case MNGM_RESULT_BALLSLOW_SEQ_EFFECT_WAIT:

		if( p_wk->wait > 0 ){
			p_wk->wait --;
		}
		if( p_wk->wait == 0 ){	

			p_wk->seq = MNGM_RESULT_BALLSLOW_SEQ_GADGET_MSG00_ON;
			p_wk->wait = 0;
		}
		break;

	// 順位に対するメッセージ
	case MNGM_RESULT_BALLSLOW_SEQ_GADGET_MSG00_ON:
		{
			u32 msgid;

			// 順位メッセージ
			if( p_wk->param.result[ p_wk->comm_param.my_playerid ] == 0 ){
				msgid = msg_17;
			}else{
				msgid = msg_18;
			}

			// トレーナ名設定
			MNGM_MSG_SetPlayerName( &p_wk->msg, 
					p_wk->comm_pdata.cp_status[ p_wk->comm_param.my_playerid ] );
			MNGM_TALKWIN_MsgPrint( &p_wk->talkwin, &p_wk->msg, msgid, MNGM_TALKWIN_IDX_MAIN );
		}
		p_wk->seq ++;
		break;

	case MNGM_RESULT_BALLSLOW_SEQ_GADGET_MSG00_WAIT:
		result = MNGM_TALKWIN_MsgEndCheck( &p_wk->talkwin, MNGM_TALKWIN_IDX_MAIN );
		if( result ){

			if( MNGM_RESULT_SndTopMeEndWait( p_wk ) == TRUE ){

				// WiFiクラブならガジェットのことはいわない
				if( p_wk->comm_param.replay == TRUE ){
					p_wk->wait	= MNGM_RESULT_MSGDRAW_WAIT;	// 描画ウエイト
					p_wk->seq	= MNGM_RESULT_BALLSLOW_SEQ_GADGET_MSG_WAIT2;
				}else{
					p_wk->wait	= MNGM_RESULT_MSGDRAW_WAIT;
					p_wk->seq	++;
				}
			}
		}
		break;

	// ガジェットメッセージ処理
	case MNGM_RESULT_BALLSLOW_SEQ_GADGET_MSG_ON:

		// メッセージ更新待ち
		if( p_wk->wait > 0 ){
			p_wk->wait --;
			break;
		}

		{
			u32 msgid;

			// ガジェットがもうレベルMAXでアップしなかったとき
			if( p_wk->gadget_update ){

				// ガジェット設定
				MNGM_MSG_SetGadget( &p_wk->msg, p_wk->comm_param.p_lobby_wk->gadget );
				if( p_wk->last_gadget == p_wk->comm_param.p_lobby_wk->gadget ){
					msgid = msg_19;
				}else{
					msgid = msg_02;
				}
			}else{

				// WiFi広場なのでガジェットがアップしなかった
				msgid = msg_09;
			}

			// トレーナ名設定
			MNGM_MSG_SetPlayerName( &p_wk->msg, 
					p_wk->comm_pdata.cp_status[ p_wk->comm_param.my_playerid ] );
			MNGM_TALKWIN_MsgPrint( &p_wk->talkwin, &p_wk->msg, msgid, MNGM_TALKWIN_IDX_MAIN );
		}
		p_wk->seq  = MNGM_RESULT_BALLSLOW_SEQ_GADGET_MSG_WAIT;
		break;
		
	case MNGM_RESULT_BALLSLOW_SEQ_GADGET_MSG_WAIT:
		result = MNGM_TALKWIN_MsgEndCheck( &p_wk->talkwin, MNGM_TALKWIN_IDX_MAIN );
		if( result ){
			p_wk->wait = MNGM_RESULT_MSGDRAW_WAIT;
			p_wk->seq ++;
		}
		break;

	case MNGM_RESULT_BALLSLOW_SEQ_GADGET_MSG_WAIT2:
		if( p_wk->wait > 0 ){
			p_wk->wait --;
			if( p_wk->wait == 0 ){
				MNGM_TALKWIN_MsgOff( &p_wk->talkwin, MNGM_TALKWIN_IDX_MAIN );
				p_wk->wait = MNGM_RESULT_RESULT_DRAW_WAIT;
				p_wk->seq ++;

			}
		}
		break;

	// 通信同期開始
	case MNGM_RESULT_BALLSLOW_SEQ_SYNCSTART:
#ifdef DEBUG_SYNCSTART_A
		if( sys.trg & PAD_BUTTON_A ){
			p_wk->seq ++;
			CommTimingSyncStart( MNGM_SYNC_RESULT_END );

			// VCHAT OFF
			if( p_wk->comm_param.vchat ){
				// ボイスチャット終了
				mydwc_stopvchat();
			}
		}
#else

		if( p_wk->wait > 0 ){
			p_wk->wait--;
		}else{
		
			CommTimingSyncStart( MNGM_SYNC_RESULT_END );

			// VCHAT OFF
			if( p_wk->comm_param.vchat ){
				// ボイスチャット終了
				mydwc_stopvchat();
			}
			p_wk->seq ++;
		}	
#endif

		break;
		
	case MNGM_RESULT_BALLSLOW_SEQ_SYNCWAIT:
		if( CommIsTimingSync( MNGM_SYNC_RESULT_END ) ){

			// リトライを聞くならリトライへ
			if( p_wk->comm_param.replay ){
				p_wk->seq = MNGM_RESULT_BALLSLOW_SEQ_RETRY_INIT;
			}else{
				p_wk->seq ++;
			}
		}
		break;
		
	case MNGM_RESULT_BALLSLOW_SEQ_WIPEOUT_INIT:
		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEOUT, 
				WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, 
				WIPE_DEF_DIV, WIPE_DEF_SYNC, p_wk->heapID );
		p_wk->seq ++;
		break;
		
	case MNGM_RESULT_BALLSLOW_SEQ_WIPEOUT_WAIT:
		result = WIPE_SYS_EndCheck();
		if( result ){
			p_wk->seq  = MNGM_RESULT_BALLSLOW_SEQ_END;
		}
		break;

	// 再挑戦を聞く場合
	case MNGM_RESULT_BALLSLOW_SEQ_RETRY_INIT:

		// 遊んだ回数を設定
		MNGM_RESULT_SetPlayNum( p_wk );
		
		p_wk->seq ++;
		break;

	case MNGM_RESULT_BALLSLOW_SEQ_RETRY_MAIN:
		result = MNGM_RESULT_Retry_Main( &p_wk->retrysys, &p_wk->talkwin, &p_wk->msg, p_wk->gametype, p_wk->heapID );
		if( result ){
			// リトライ設定
			p_wk->replay = MNGM_RESULT_RetryGet( &p_wk->retrysys );
			p_wk->seq ++;
		}
		break;
		
	case MNGM_RESULT_BALLSLOW_SEQ_END:

		// HBLANKシステム破棄
		MNGM_PLAYER_PLATE_HBLANK_BGSCR_Exit( &p_wk->bgscrl );
		
		p_wk->end_flag = TRUE;
		break;


	// 全員のスコアが０のときの処理
	case MNGM_RESULT_BALLSLOW_SEQ_ALLSCORE0_WAIT:
		p_wk->draw_result_wait --;
		if( p_wk->draw_result_wait <= 0 ){
			p_wk->seq ++;
		}
		break;
		
	case MNGM_RESULT_BALLSLOW_SEQ_ALLSCORE0_DRAW:
		{
			int i;
			
			// 描画
			for( i=0; i<p_wk->comm_param.num; i++ ){
				// 順位を書き換える
				p_wk->param.result[i] = p_wk->comm_param.num-1;
				MNGM_RESULT_BallslowBalancePlayerTblDraw( p_wk, i, p_wk->param.result[i] );
			}

			// エフェクト描画に戻る
			p_wk->seq = MNGM_RESULT_BALLSLOW_SEQ_TBLMOVE_WAIT;
		}
		break;

	}

	// パレットエフェクト処理
	MNGM_RESULT_PalTrEffect( p_wk );	// 1位のスクリーンだけエフェクト


	// CLACT描画
	MNGM_CLACT_Draw( &p_wk->clact );


	// スクリーン面をスクロールさせる
	GF_BGL_ScrollReq( p_wk->bgl.p_bgl, GF_BGL_FRAME3_M, 
			GF_BGL_SCROLL_Y_INC, MNGM_BGSCROLL_SPEED );
	GF_BGL_ScrollReq( p_wk->bgl.p_bgl, GF_BGL_FRAME1_S, 
			GF_BGL_SCROLL_Y_INC, MNGM_BGSCROLL_SPEED );
}


//----------------------------------------------------------------------------
/**
 *	@brief	風船わり用タスク
 */
//-----------------------------------------------------------------------------
static void MNGM_RESULT_Tcb_Balloon( TCB_PTR tcb, void* p_work )
{
	MNGM_RESULTWK* p_wk = p_work;
	BOOL result;

	switch( p_wk->seq ){
	case MNGM_RESULT_BALLOON_SEQ_WIPEIN_INIT:

		// replayフラグがTRUEならWiFi友達部屋なのでガジェットはアップデートしない
		if( p_wk->comm_param.replay == TRUE ){
			p_wk->gadget_update = FALSE;
		}else{
			// ガジェットアップデートチェック
			if( p_wk->param.balloon >= MNGM_BALLOON_GADGET_UPDATE ){
				// ガジェットアップデート
				p_wk->comm_param.p_lobby_wk->gadget = MNGM_ITEM_Update( p_wk->comm_param.p_lobby_wk->gadget );
				p_wk->gadget_update = TRUE;
			}else{
				p_wk->gadget_update = FALSE;
			}

			// 親なら結果のニュースを作成
			if( p_wk->comm_param.my_playerid == 0 ){
				MNGM_RESULT_Balloon_SendMinigameTopResult( p_wk );
			}
		}

		
		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEIN, 
				WIPE_TYPE_FADEIN, WIPE_FADE_OUTCOLOR, 
				WIPE_DEF_DIV, WIPE_DEF_SYNC, p_wk->heapID );
		p_wk->seq ++;
		break;
		
	case MNGM_RESULT_BALLOON_SEQ_WIPEIN_WAIT:
		result = WIPE_SYS_EndCheck();
		if( result ){
			p_wk->seq ++;

			p_wk->draw_result = 4;
		}
		break;

	case MNGM_RESULT_BALLOON_SEQ_MSGON_INIT:
		MNGM_TALKWIN_MsgPrint( &p_wk->talkwin, &p_wk->msg, msg_01, MNGM_TALKWIN_IDX_MAIN );
		p_wk->seq ++;
		break;
		
	case MNGM_RESULT_BALLOON_SEQ_MSGON_WAIT:
		result = MNGM_TALKWIN_MsgEndCheck( &p_wk->talkwin, MNGM_TALKWIN_IDX_MAIN );
		if( result ){
			p_wk->wait = MNGM_RESULT_MSGDRAW_WAIT;
			p_wk->seq ++;
		}
		break;

	case MNGM_RESULT_BALLOON_SEQ_MSGON_WAIT2:
		if( p_wk->wait > 0 ){
			p_wk->wait --;
			if( p_wk->wait == 0 ){
				MNGM_TALKWIN_MsgOff( &p_wk->talkwin, MNGM_TALKWIN_IDX_MAIN );
				p_wk->seq ++;
			}
		}
		break;
		
	// 風船表示部分
	case MNGM_RESULT_BALLOON_SEQ_TBLON:
		{
			int i;
			BOOL  vip;
			u32 netid;

			MNGM_PLAYER_PLATE_HBLANK_BGSCR_Init( &p_wk->bgscrl, &p_wk->bgl, p_wk->heapID );
			
			for( i=0; i<p_wk->comm_param.num; i++ ){
				MNGM_PLATE_PLAYERTBL_Init( &p_wk->playertbl[i], p_wk->comm_param.num, 
						p_wk->gametype,
						p_wk->p_scrn[ MNGM_RESULT_SCRN_TBL ], &p_wk->bgl, i, p_wk->comm_param.my_playerid,
						&p_wk->clact, p_wk->p_resobj, 
						p_wk->heapID );

				// 名前表示
				netid = MNGM_ENRES_PARAM_GetPlNOtoNetID( &p_wk->comm_param, i );
				vip = MNGM_ENRES_PARAM_GetVipFlag( &p_wk->comm_param, netid );
				MNGM_PLATE_PLAYERTBL_DrawName( &p_wk->playertbl[i], &p_wk->msg, p_wk->comm_pdata.cp_status[i], MNGM_RESULT_DRAWNAME_X, 0, vip );
				// 国名表示
				MNGM_PLATE_PLAYERTBL_DrawNation( &p_wk->playertbl[i], &p_wk->msg, 
						p_wk->draw_nation, p_wk->comm_pdata.nation[i],
						p_wk->comm_pdata.area[i], MNGM_RESULT_DRAWNAME_X );

				// 表示
				MNGM_PLATE_PLAYERTBL_BgWriteVReq( &p_wk->playertbl[i], &p_wk->bgl );
				MNGM_PLATE_PLAYERTBL_DrawOamWay( &p_wk->playertbl[i] );

				// 動作パラメータ初期化
				MNGM_PLATE_PLAYERTBL_StartMove( &p_wk->playertbl[i], MNGM_PLATE_MOVE_ENTRY_END, MNGM_PLATE_MOVE_WAIT_ENTRY*i );
			}
		}
		p_wk->seq ++;
		p_wk->wait = MNGM_RESULT_BALLOON_DRAW_WAIT;
		break;
		
	case MNGM_RESULT_BALLOON_SEQ_BALLOON_ON:
		result = MNGM_RESULT_PlayerTblMove( p_wk );
		if( result == TRUE ){

			p_wk->wait --;
			if( p_wk->wait <= 0 ){
				MNGM_RESULT_Balloon_Start( &p_wk->balloon, p_wk->param.balloon );
				p_wk->seq ++;
			}
		}
		break;
		
	case MNGM_RESULT_BALLOON_SEQ_BALLOON_ONWAIT:
		result = MNGM_RESULT_Balloon_Main( &p_wk->balloon, &p_wk->bgl );
		if( result == TRUE ){

			if( p_wk->gadget_update == TRUE ){
				MNGM_RESULT_SndTopMePlay( p_wk );
			}

			p_wk->seq ++;
			
		}
		break;

	// 風船を割った数を表示
	case MNGM_RESULT_BALLOON_SEQ_GADGET_MSG_00ON:
		MNGM_MSG_SetBalloonNum( &p_wk->msg, p_wk->param.balloon );
		MNGM_TALKWIN_MsgPrint( &p_wk->talkwin, &p_wk->msg, msg_07, MNGM_TALKWIN_IDX_MAIN );
		p_wk->wait = MNGM_RESULT_MSGDRAW_WAIT;
		p_wk->seq ++;
		break;

	case MNGM_RESULT_BALLOON_SEQ_GADGET_MSG_00WAIT:
		result = MNGM_TALKWIN_MsgEndCheck( &p_wk->talkwin, MNGM_TALKWIN_IDX_MAIN );
		if( result == TRUE ){

			if( p_wk->wait <= 0 ){

				if( MNGM_RESULT_SndTopMeEndWait( p_wk ) == TRUE ){
					
					// WiFiクラブのときはガジェットは関係ない
					if( p_wk->comm_param.replay == TRUE ){
						p_wk->wait = MNGM_RESULT_MSGDRAW_WAIT;
						p_wk->seq = MNGM_RESULT_BALLOON_SEQ_GADGET_MSG_WAIT2;
					}else{
						p_wk->seq ++;
					}
				}	
			}else{
				p_wk->wait --;
			}
		}
		break;

	// ガジェットメッセージ処理
	case MNGM_RESULT_BALLOON_SEQ_GADGET_MSG_ON:
		{
			u32 msg_no;
			if( p_wk->gadget_update ){
				msg_no = msg_08;
			}else{
				msg_no = msg_09;
			}
			MNGM_TALKWIN_MsgPrint( &p_wk->talkwin, &p_wk->msg, msg_no, MNGM_TALKWIN_IDX_MAIN );
		}
		p_wk->seq ++;
		break;
		
	case MNGM_RESULT_BALLOON_SEQ_GADGET_MSG_WAIT:
		result = MNGM_TALKWIN_MsgEndCheck( &p_wk->talkwin, MNGM_TALKWIN_IDX_MAIN );
		if( result ){
			p_wk->wait = MNGM_RESULT_MSGDRAW_WAIT;

			if( p_wk->gadget_update ){
				p_wk->seq  = MNGM_RESULT_BALLOON_SEQ_EFFECT_WAIT;
			}else{
				p_wk->seq ++;
			}
		}
		break;

	// エフェクトを出さないときのウエイト
	case MNGM_RESULT_BALLOON_SEQ_GADGET_MSG_WAIT2:
		if( p_wk->wait > 0 ){
			p_wk->wait --;
			if( p_wk->wait == 0 ){
				p_wk->seq  = MNGM_RESULT_BALLOON_SEQ_SYNCSTART;
			}
		}
		break;
		
	case MNGM_RESULT_BALLOON_SEQ_EFFECT_WAIT:
		
		// 経過時間も計算
		if( p_wk->wait > 0 ){
			p_wk->wait --;
		}
		
		if( p_wk->wait == 0 ){
			p_wk->seq  = MNGM_RESULT_BALLOON_SEQ_SYNCSTART;
		}
		break;

	// 通信同期開始
	case MNGM_RESULT_BALLOON_SEQ_SYNCSTART:
		// VCHAT OFF
		if( p_wk->comm_param.vchat ){
			// ボイスチャット終了
			mydwc_stopvchat();
		}
		CommTimingSyncStart( MNGM_SYNC_RESULT_END );
		p_wk->seq ++;
		break;
		
	case MNGM_RESULT_BALLOON_SEQ_SYNCWAIT:
		if( CommIsTimingSync( MNGM_SYNC_RESULT_END ) ){

			// リトライを聞くp_wk->draw_nationならリトライへ
			if( p_wk->comm_param.replay ){

				p_wk->seq = MNGM_RESULT_BALLOON_SEQ_RETRY_INIT;
			}else{
				p_wk->seq ++;
			}
		}
		break;
		
	case MNGM_RESULT_BALLOON_SEQ_WIPEOUT_INIT:
		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEOUT, 
				WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, 
				WIPE_DEF_DIV, WIPE_DEF_SYNC, p_wk->heapID );
		p_wk->seq ++;
		break;
		
	case MNGM_RESULT_BALLOON_SEQ_WIPEOUT_WAIT:
		result = WIPE_SYS_EndCheck();
		if( result ){
			p_wk->seq  = MNGM_RESULT_BALLOON_SEQ_END;
		}
		break;

	// 再挑戦を聞く場合
	case MNGM_RESULT_BALLOON_SEQ_RETRY_INIT:

		// 遊んだ回数を設定
		MNGM_RESULT_SetPlayNum( p_wk );
		
		p_wk->seq ++;
		break;

	case MNGM_RESULT_BALLOON_SEQ_RETRY_MAIN:
		result = MNGM_RESULT_Retry_Main( &p_wk->retrysys, &p_wk->talkwin, &p_wk->msg, p_wk->gametype, p_wk->heapID );
		if( result ){
			// リトライ設定
			p_wk->replay = MNGM_RESULT_RetryGet( &p_wk->retrysys );
			p_wk->seq ++;
		}
		break;
		
	case MNGM_RESULT_BALLOON_SEQ_END:

		MNGM_PLAYER_PLATE_HBLANK_BGSCR_Exit( &p_wk->bgscrl );
		p_wk->end_flag = TRUE;
		break;
	}

	// パレットエフェクト処理
	MNGM_RESULT_PalTrEffect( p_wk );	// 1位のスクリーンだけエフェクト

	// CLACT描画
	MNGM_CLACT_Draw( &p_wk->clact );


	// スクリーン面をスクロールさせる
	GF_BGL_ScrollReq( p_wk->bgl.p_bgl, GF_BGL_FRAME3_M, 
			GF_BGL_SCROLL_Y_INC, MNGM_BGSCROLL_SPEED );
	GF_BGL_ScrollReq( p_wk->bgl.p_bgl, GF_BGL_FRAME1_S, 
			GF_BGL_SCROLL_Y_INC, MNGM_BGSCROLL_SPEED );
}

//----------------------------------------------------------------------------
/**
 *	@brief	結果画面VWaitタスク
 */
//-----------------------------------------------------------------------------
static void MNGM_RESULT_VWaitTcb( TCB_PTR tcb, void* p_work )
{
	MNGM_RESULTWK* p_wk = p_work;


    // Vram転送マネージャー実行
    DoVramTransferManager();

	// BGのVBLANK処理
    MNGM_BGL_VBlank( &p_wk->bgl );

	// OAMのVBLANK処理
	MNGM_CLACT_VBlank( &p_wk->clact );

	// HBLANKシステム
	MNGM_PLAYER_PLATE_HBLANK_BGSCR_VBlank( &p_wk->bgscrl );
}

//----------------------------------------------------------------------------
/**
 *	@brief	パレット転送アニメエフェクト	初期化
 *
 *	@param	p_wk			ワーク
 *	@param	p_handle		ハンドル
 *	@param	heapID			ヒープID
 */
//-----------------------------------------------------------------------------
static void MNGM_RESULT_PalTrEffectInit( MNGM_RESULTWK* p_wk, ARCHANDLE* p_handle, u32 heapID )
{
	// パレットデータ取得
	p_wk->p_plttbuf = ArcUtil_HDL_PalDataGet( p_handle, 
			NARC_wlmngm_tool_minigame_win_ani_NCLR, 
			&p_wk->p_pltt, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	エフェクトを開始
 *
 *	@param	p_wk			ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_RESULT_PalTrEffectStart( MNGM_RESULTWK* p_wk )
{
	int i;
	
	p_wk->pltt_start = TRUE;

	// 1位がいたら音を鳴らす
	for( i=0; i<p_wk->comm_param.num; i++ ){
		if( p_wk->param.result[i] == 0 ){

			// それが自分なら音を鳴らす
			if( p_wk->comm_param.my_playerid == i ){
				MNGM_RESULT_SndTopMePlay( p_wk );
				break;
			}
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	パレット転送アニメエフェクト	メイン
 *
 *	@param	p_wk			ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_RESULT_PalTrEffect( MNGM_RESULTWK* p_wk )
{
	int i;
	u32 tr_addr;	// 書き込み先アドレス
	u8* p_rd_addr;	// 読み込み先アドレス
	BOOL result;

	if( p_wk->pltt_start == FALSE ){
		return;
	}

	// 転送時間チェック
	if( (p_wk->pltt_time == 0) || 
		(p_wk->pltt_time == MNGM_RESULT_PALANM_FRAME) ){

		for( i=0; i<p_wk->comm_param.num; i++ ){
			if( p_wk->param.result[i] == 0 ){	// 1位のときだけ転送する

				tr_addr		= (sc_MNGM_BG_PLTT_PLNO_TBL[i]*32);
				p_rd_addr	= p_wk->p_pltt->pRawData; 

				// 転送データアドレスを検索
				if( p_wk->pltt_time == 0 ){
					p_rd_addr += ((MNGM_PLAYER_BG_ANMPLTT_TOPANM0_0+sc_MNGM_BG_PLTT_PLNO_TBL[i])*32);
				}else{
					p_rd_addr += ((MNGM_PLAYER_BG_ANMPLTT_TOPANM1_0+sc_MNGM_BG_PLTT_PLNO_TBL[i])*32);
				}
				
				// 対応したテーブルのパレットを転送
				result = AddVramTransferManager( NNS_GFD_DST_2D_BG_PLTT_MAIN, 
						tr_addr, p_rd_addr, 32 );
				GF_ASSERT( result );	// タスク設定ができなかった
			}	
		}

	}

	p_wk->pltt_time = (p_wk->pltt_time + 1) % (MNGM_RESULT_PALANM_FRAME*2);
}

//----------------------------------------------------------------------------
/**
 *	@brief	パレット転送アニメエフェクト	破棄
 *
 *	@param	p_wk			ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_RESULT_PalTrEffectExit( MNGM_RESULTWK* p_wk )
{
	sys_FreeMemoryEz( p_wk->p_plttbuf );
}

//----------------------------------------------------------------------------
/**
 *	@brief	リトライシステム初期化
 *
 *	@param	p_wk		ワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void MNGM_RESULT_Retry_Init( MNGM_RETRY_WK* p_wk, MNGM_BGL* p_bgl, u32 vchat, u32 heapID )
{

	memset( p_wk, 0, sizeof(MNGM_RETRY_WK) );
	
	p_wk->param.p_bgl		= p_bgl->p_bgl;
	p_wk->param.bg_frame	= GF_BGL_FRAME0_S;
	p_wk->param.char_offs	= MNGM_RESULT_RETRY_CHAR_OFFS;
	p_wk->param.pltt_offs = MNGM_RESULT_RETRY_PLTT_OFFS;
	p_wk->param.x			= MNGM_RESULT_RETRY_X;
	p_wk->param.y			= MNGM_RESULT_RETRY_Y;
	p_wk->p_ts = TOUCH_SW_AllocWork( heapID );

	p_wk->vchat	 = vchat;


	// リトライ受信データ
	p_wk->recv_replay = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	リトライシステム	破棄
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_RESULT_Retry_Exit( MNGM_RETRY_WK* p_wk )
{
	TOUCH_SW_FreeWork( p_wk->p_ts );
}

//----------------------------------------------------------------------------
/**
 *	@brief	リトライシステム	メイン
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static BOOL MNGM_RESULT_Retry_Main( MNGM_RETRY_WK* p_wk, MNGM_TALKWIN* p_talkwin, MNGM_MSG* p_msg, u32 gametype, u32 heapID )
{
	u32 result;
	
	switch( p_wk->seq ){
	case MNGM_RESULT_RETRY_SEQ_WIPEIN:
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, 
				WIPE_TYPE_FADEIN, WIPE_FADE_BLACK,
				WIPE_DEF_DIV, WIPE_DEF_SYNC, heapID );

		// サブにアイコンを送信
		WirelessIconEasy_HoldLCD( FALSE, heapID );

		// VCHAT ON
		if( p_wk->vchat ){
			// ボイスチャット開始
			mydwc_startvchat( heapID );
		}
		p_wk->seq++;
		break;
		
	case MNGM_RESULT_RETRY_SEQ_WIPEINWAIT:
		if( WIPE_SYS_EndCheck() ){
			p_wk->seq ++;
		}
		break;
		
	case MNGM_RESULT_RETRY_SEQ_MSGON:
		MNGM_MSG_SetGameName( p_msg, gametype );
		MNGM_TALKWIN_MsgPrint( p_talkwin, p_msg, msg_03, MNGM_TALKWIN_IDX_SUB );
		p_wk->seq ++;
		break;
		
	case MNGM_RESULT_RETRY_SEQ_MSGWAIT:
		result = MNGM_TALKWIN_MsgEndCheck( p_talkwin, MNGM_TALKWIN_IDX_SUB );
		if( result == TRUE ){
			p_wk->seq ++;
		}
		break;
		
	case MNGM_RESULT_RETRY_SEQ_TPON:	
		TOUCH_SW_Init( p_wk->p_ts, &p_wk->param );
		p_wk->seq ++;
		break;
		
	case MNGM_RESULT_RETRY_SEQ_TPMAIN:
		result = TOUCH_SW_Main( p_wk->p_ts );

		// AUTOプレイ
#ifdef DEBUG_MINIGAME_AUTO_PLAY
		result = TOUCH_SW_RET_YES;
#endif
		
		if( (result == TOUCH_SW_RET_YES) ||
			(result == TOUCH_SW_RET_NO)	){

			
			if( result == TOUCH_SW_RET_YES ){
				CommSendData( CNM_MNGM_RETRY_YES, NULL, 0 );
			}else if( result == TOUCH_SW_RET_NO ){
				CommSendData( CNM_MNGM_RETRY_NO, NULL, 0 );
			}

			MNGM_TALKWIN_MsgPrint( p_talkwin, p_msg, msg_15, MNGM_TALKWIN_IDX_SUB );
			MNGM_TALKWIN_SetTimeWork( p_talkwin, MNGM_TALKWIN_IDX_SUB );
			p_wk->seq ++;

		}

		break;

	case MNGM_RESULT_RETRY_SEQ_RECV:			// みんなの選択を受信
		if( p_wk->ko_recv ){
			if( p_wk->ko_recv_replay ){
				p_wk->seq = MNGM_RESULT_RETRY_SEQ_WIPEOUT;	// OK
			}else{
				p_wk->seq ++;
			}
		}
		break;
		
	case MNGM_RESULT_RETRY_SEQ_RECVMSGON:	// 誰かがやめるを選んだらつごうが・・・
		MNGM_TALKWIN_MsgPrint( p_talkwin, p_msg, msg_06, MNGM_TALKWIN_IDX_SUB );
		p_wk->seq ++;
		break;
		
	case MNGM_RESULT_RETRY_SEQ_RECVMSGWAIT:	// 
		result = MNGM_TALKWIN_MsgEndCheck( p_talkwin, MNGM_TALKWIN_IDX_SUB );
		if( result == TRUE ){
			p_wk->seq ++;
			p_wk->wait = MNGM_RESULT_MSGDRAW_WAIT;
		}
		break;

	case MNGM_RESULT_RETRY_SEQ_RECVMSGWAIT2:
		p_wk->wait --;
		if( p_wk->wait == 0 ){
			p_wk->seq ++;
		}
		break;
		
	case MNGM_RESULT_RETRY_SEQ_WIPEOUT:		// ワイプアウト
		WIPE_SYS_Start( WIPE_PATTERN_S, WIPE_TYPE_FADEOUT, 
				WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK,
				WIPE_DEF_DIV, WIPE_DEF_SYNC, heapID );
		p_wk->seq ++;
		break;
		
	case MNGM_RESULT_RETRY_SEQ_WIPEOUTWAIT:	// ワイプアウトウエイト
		if( WIPE_SYS_EndCheck() ){

			// メインにアイコンを送信
			WirelessIconEasyEnd();

			// VCHAT OFF
			if( p_wk->vchat ){
				// ボイスチャット終了
				mydwc_stopvchat();
			}
			p_wk->seq ++;
		}
		break;

	case MNGM_RESULT_RETRY_SEQ_END:
		return TRUE;
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	リトライするかを取得する
 *
 *	@param	cp_wk	ワーク
 *	
 *	@retval	TRUE	リトライする
 *	@retval	FALSE	リトライしない
 */
//-----------------------------------------------------------------------------
static BOOL MNGM_RESULT_RetryGet( const MNGM_RETRY_WK* cp_wk )
{
	return cp_wk->ko_recv_replay;
}

//----------------------------------------------------------------------------
/**
 *	@brief	リトライ親の受信処理
 *
 *	@param	p_wk		ワーク
 *	@param	netid	プレイヤーナンバー
 *	@param	replay		replay
 *	@param	playernum	プレイヤー数
 */
//-----------------------------------------------------------------------------
static void MNGM_RESULT_RetryOyaRecv( MNGM_RETRY_WK* p_wk, u32 netid, BOOL replay, u32 playernum )
{
	// もう受信して無いかチェック
	if( (p_wk->recv_num & (1<<netid)) == 0 ){

		if( replay == FALSE ){	
			p_wk->recv_replay = FALSE;
		}
		
		// 受信完了
		p_wk->recv_num |= (1<<netid);
	}

	// みんなから受信したか？
	{
		int i;
		int recv_num;

		recv_num = 0;
		for( i=0; i<WFLBY_MINIGAME_MAX; i++ ){
			if( (p_wk->recv_num & (1<<i)) ){
				recv_num ++;
			}
		}

		// みんなから受け取ったらreplayするか送信
		if( recv_num == playernum ){
			if( p_wk->recv_replay ){
				CommSendData( CNM_MNGM_RETRY_OK, NULL, 0 );
			}else{
				CommSendData( CNM_MNGM_RETRY_NG, NULL, 0 );
			}
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	親からリトライを受信
 *
 *	@param	p_wk		ワーク
 *	@param	replay		リトライ
 */
//-----------------------------------------------------------------------------
static void MNGM_RESULT_RetryKoRecv( MNGM_RETRY_WK* p_wk, BOOL replay )
{
	p_wk->ko_recv = TRUE;
	p_wk->ko_recv_replay = replay;
}

//----------------------------------------------------------------------------
/**
 *	@brief	BALLOONグラフィック初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_bgl		BGLシステム
 *	@param	p_clact		セルアクター
 *	@param	p_clres		リソースシステム
 *	@param	p_handle	ハンドル
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void MNGM_RESULT_Balloon_GraphicInit( MNGM_BALLOON_WK* p_wk, MNGM_BGL* p_bgl, MNGM_CLACT* p_clact, MNGM_CLACTRES* p_clres, ARCHANDLE* p_handle, u32 heapID )
{
	memset( p_wk, 0, sizeof(MNGM_BALLOON_WK) );

/*
	//	システムウィンドウ
	{
		// キャラクタ、パレット転送
		MenuWinGraphicSet( p_bgl->p_bgl, GF_BGL_FRAME2_M, MNGM_RESULT_BALLOON_SYSWIN_CGX,
			MNGM_RESULT_BALLOON_SYSWIN_PAL, MENU_TYPE_SYSTEM, heapID );

		// ウィンドウ作成
		GF_BGL_BmpWinAdd( p_bgl->p_bgl, &p_wk->win, GF_BGL_FRAME2_M,
				MNGM_RESULT_BALLOON_BMP_X, MNGM_RESULT_BALLOON_BMP_Y,
				MNGM_RESULT_BALLOON_BMP_SIZX, MNGM_RESULT_BALLOON_BMP_SIZY,
				MNGM_RESULT_BALLOON_BMP_PAL, MNGM_RESULT_BALLOON_BMP_CGX);
		GF_BGL_BmpWinDataFill( &p_wk->win, 15 );
	}
//*/

	//　独自ウィンドウ
	{
		ArcUtil_HDL_PalSet( p_handle, NARC_wlmngm_tool_mini_fusen_score_NCLR, PALTYPE_MAIN_BG, 
				MNGM_RESULT_BALLOON_SYSWIN_PAL*32, 32, heapID );
		ArcUtil_HDL_BgCharSet( p_handle, NARC_wlmngm_tool_mini_fusen_score_NCGR,
				p_bgl->p_bgl, GF_BGL_FRAME2_M, MNGM_RESULT_BALLOON_SYSWIN_CGX, 0, FALSE, heapID );
		p_wk->p_scrnbuff = ArcUtil_HDL_ScrnDataGet( p_handle, NARC_wlmngm_tool_mini_fusen_score_NSCR, 
				FALSE, &p_wk->p_scrn, heapID );
	}

	//　OAM	
	{
		int i, j;
		int idx;
		int x, y;
		
		// OAMを読み込む
		p_wk->p_resobj = MNGM_CLACTRES_Load( p_clres, p_handle, 
				NARC_wlmngm_tool_result_fusen_NCLR, 1,
				NARC_wlmngm_tool_result_fusen_NCGR,
				NARC_wlmngm_tool_result_fusen_NCER,
				NARC_wlmngm_tool_result_fusen_NANR,
				MNGM_RESULT_BALLOON_CONTID, heapID );

		// アクターを作成
		y = MNGM_RESULT_BALLOON_DEF_Y;
		for( i=0; i<MNGM_RESULT_BALLOON_Y; i++ ){
			x = MNGM_RESULT_BALLOON_DEF_X;

			for( j=0; j<MNGM_RESULT_BALLOON_X; j++ ){
				idx = (i*MNGM_RESULT_BALLOON_X)+j;
				p_wk->p_clwk[idx] = MNGM_CLACTRES_Add( p_wk->p_resobj,
						p_clact->p_clactset, x, y, 0, heapID );
				CLACT_SetDrawFlag( p_wk->p_clwk[idx], FALSE );
				CLACT_AnmChg( p_wk->p_clwk[idx], i );
				CLACT_BGPriorityChg( p_wk->p_clwk[idx], MNGM_RESULT_BALLOON_BG_PRI );

				x += MNGM_RESULT_BALLOON_SIZ_X;
			}

			y += MNGM_RESULT_BALLOON_SIZ_Y;
		}
	}
	
}

//----------------------------------------------------------------------------
/**
 *	@brief	BALLOON	グラフィック破棄
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_RESULT_Balloon_GraphicExit( MNGM_BALLOON_WK* p_wk )
{

	//  OAM破棄
	{
		int i;

		for( i=0; i<MNGM_RESULT_BALLOON_CLACT_NUM; i++ ){
			CLACT_Delete( p_wk->p_clwk[i] );
		}
	}

	

	// システムウィンドウ破棄
	{
		sys_FreeMemoryEz( p_wk->p_scrnbuff );
		// ウィンドウ作成
//		GF_BGL_BmpWinDel( &p_wk->win ); 
	}
	
}

//----------------------------------------------------------------------------
/**
 *	@brief	動作開始
 *
 *	@param	p_wk	ワーク
 *	@param	num		割った数
 */
//-----------------------------------------------------------------------------
static void MNGM_RESULT_Balloon_Start( MNGM_BALLOON_WK* p_wk, u32 num )
{
	p_wk->num = num;
	p_wk->seq = 0;
	p_wk->idx = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	BALLOONウィンドウ	処理
 *
 *	@param	p_wk	ワーク
 *	@param	p_bgl	BGLシステム
 *
 *	@retval	TRUE	オワリ
 *	@ratval	FALSE	まだ
 */
//-----------------------------------------------------------------------------
static BOOL MNGM_RESULT_Balloon_Main( MNGM_BALLOON_WK* p_wk, MNGM_BGL* p_bgl )
{
	switch( p_wk->seq ){
	case MNGM_RESULT_BALLOON_SEQ_ON:

		MNGM_SCRN_AddCharOfs( p_wk->p_scrn,  MNGM_RESULT_BALLOON_SYSWIN_CGX ); 
		GF_BGL_ScrWrite( p_bgl->p_bgl, GF_BGL_FRAME2_M, p_wk->p_scrn->rawData, 
				0, 0,
				MNGM_RESULT_BALLOON_SYSWIN_SCRNSIZX, MNGM_RESULT_BALLOON_SYSWIN_SCRNSIZY );
		GF_BGL_ScrPalChange( p_bgl->p_bgl, GF_BGL_FRAME2_M, 0, 0,
				MNGM_RESULT_BALLOON_SYSWIN_SCRNSIZX, MNGM_RESULT_BALLOON_SYSWIN_SCRNSIZY,
				MNGM_RESULT_BALLOON_SYSWIN_PAL );
		GF_BGL_LoadScreenV_Req( p_bgl->p_bgl, GF_BGL_FRAME2_M );

		p_wk->seq ++;
		break;
		
	case MNGM_RESULT_BALLOON_SEQ_BLN_ON:

		// オワリの数になったら終わり
		if( p_wk->idx == p_wk->num ){
			p_wk->seq = MNGM_RESULT_BALLOON_SEQ_BLN_END;
			break;	
		}
		Snd_SePlay( MNGM_SND_BALLOON );
		
		CLACT_SetDrawFlag( p_wk->p_clwk[ (p_wk->idx % MNGM_RESULT_BALLOON_CLACT_NUM) ], TRUE );
		p_wk->count = MNGM_RESULT_BALLOON_WAIT;
		p_wk->seq ++;
		break;
		
	case MNGM_RESULT_BALLOON_SEQ_BLN_ONWAIT:
		p_wk->count--;
		if( p_wk->count == 0 ){

			p_wk->idx ++;
			if( (p_wk->idx % MNGM_RESULT_BALLOON_CLACT_NUM) == 0 ){
				int i;

				// さらにオワリでなければ
				if( p_wk->idx != p_wk->num ){

					for( i=0; i<MNGM_RESULT_BALLOON_CLACT_NUM; i++ ){
						// 全OAMをクリーン
						CLACT_SetDrawFlag( p_wk->p_clwk[i], FALSE );
						// 全OAMを赤に変更
						CLACT_AnmChg( p_wk->p_clwk[i], MNGM_RESULT_BALLOON_ANM_RED );
					}
				}
			}
			p_wk->seq = MNGM_RESULT_BALLOON_SEQ_BLN_ON;
		}
		break;
		
	case MNGM_RESULT_BALLOON_SEQ_BLN_END:
		return TRUE;
	}
	return FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	タマ投げとBalanceボールのプレイヤーテーブルを描画する処理
 *
 *	@param	p_wk		ワーク
 *	@param	plidx		プレイヤーIDX
 *	@param	rank		Rank
 */
//-----------------------------------------------------------------------------
static void MNGM_RESULT_BallslowBalancePlayerTblDraw( MNGM_RESULTWK* p_wk, u32 plidx, u32 rank )
{
	BOOL  vip;
	u32 keta;
	u32 netid;
	
	MNGM_PLATE_PLAYERTBL_Init( &p_wk->playertbl[plidx], 
			p_wk->comm_param.num, p_wk->gametype,
			p_wk->p_scrn[ MNGM_RESULT_SCRN_TBL ], &p_wk->bgl, plidx, 
			p_wk->comm_param.my_playerid,
			&p_wk->clact, 
			p_wk->p_resobj, 
			p_wk->heapID );

	// 名前表示
	netid = MNGM_ENRES_PARAM_GetPlNOtoNetID( &p_wk->comm_param, plidx );
	vip = MNGM_ENRES_PARAM_GetVipFlag( &p_wk->comm_param, netid );
	MNGM_PLATE_PLAYERTBL_DrawName( &p_wk->playertbl[plidx], &p_wk->msg, p_wk->comm_pdata.cp_status[plidx], MNGM_RESULT_DRAWNAME_X, MNGM_RESULT_DRAWNAME_Y, vip );
	// 国名表示
	MNGM_PLATE_PLAYERTBL_DrawNation( &p_wk->playertbl[plidx], &p_wk->msg, 
			p_wk->draw_nation, p_wk->comm_pdata.nation[plidx],
			p_wk->comm_pdata.area[plidx], MNGM_RESULT_DRAWNAME_X );

	// 玉乗りと玉いれで変わる部分
	if( p_wk->gametype == WFLBY_GAME_BALLSLOW ){
		keta = 5;
	}else{
		keta = 6;
	}

	// スコア表示
	MNGM_PLATE_PLAYERTBL_DrawScore( &p_wk->playertbl[plidx], &p_wk->msg,
			p_wk->param.score[plidx], 
			MNGM_RESULT_DRAWSCORE_X + (-sc_MNGM_PLATE_MOVE_RESULT_END[WFLBY_MINIGAME_MAX-(rank+1)]),
			MNGM_RESULT_DRAWSCORE_Y, keta );

	// 書き込み命令
	MNGM_PLATE_PLAYERTBL_BgWriteVReq( &p_wk->playertbl[plidx], &p_wk->bgl );

	// 自分にだけOAMの方向を出す
//	if( plidx == 0 ){	// plidxが0なら自分
//		MNGM_PLATE_PLAYERTBL_DrawOamWay( &p_wk->playertbl[plidx] );
//	}

	// 順位表示
	MNGM_PLATE_PLAYERTBL_DrawRank( &p_wk->playertbl[plidx], rank );

	// Rankの動作開始
	MNGM_PLATE_PLAYERTBL_StartMove( &p_wk->playertbl[plidx], sc_MNGM_PLATE_MOVE_RESULT_END[rank], 0 );
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーテーブル	動作
 *
 *	@param	p_wk	ワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
static BOOL MNGM_RESULT_PlayerTblMove( MNGM_RESULTWK* p_wk )
{
	int i;
	BOOL result;
	BOOL plate_end = TRUE;

	// 全部のPLATEを動かす
	for( i=0; i<p_wk->comm_param.num; i++ ){

		// 初期化されてるかチェック
		if( p_wk->playertbl[i].p_clwk != NULL ){

			result = MNGM_PLATE_PLAYERTBL_MainMove( &p_wk->playertbl[i] );
			MNGM_PLAYER_PLATE_HBLANK_BGSCR_SetPlateScr( &p_wk->bgscrl, &p_wk->playertbl[i] );
			if( result == FALSE ){
				plate_end = FALSE;
			}
		}else{
			plate_end = FALSE;
		}
	}

	return plate_end;
}


//----------------------------------------------------------------------------
/**
 *	@brief	1位のMEを再生する
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_RESULT_SndTopMePlay( MNGM_RESULTWK* p_wk )
{
	if( p_wk->top_me_play == FALSE ){
		Snd_MePlay( SEQ_PL_WINMINI2 );
		p_wk->top_me_play = TRUE;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	1位のMEの再生終了待ち
 *
 *	@param	p_wk	ワーク
 *
 *	@retval	TRUE	ME再生完了
 *	@retval	FALSE	ME再生中
 */
//-----------------------------------------------------------------------------
static BOOL MNGM_RESULT_SndTopMeEndWait( MNGM_RESULTWK* p_wk )
{
	if( p_wk->top_me_play == TRUE ){

		// 終了待ち＋BGM復帰
		if( Snd_MePlayCheckBgmPlay() == 0 ){ 
			p_wk->top_me_play = FALSE;
		}
	}

	if( p_wk->top_me_play == TRUE ){
		return FALSE;
	}else{
		return TRUE;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	友達と遊んだ回数を設定する
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_RESULT_SetPlayNum( MNGM_RESULTWK* p_wk )
{
	WIFI_LIST* p_list = SaveData_GetWifiListData( p_wk->comm_param.p_save );
	int i;
	int netID, ret;
	int pos;
	DWCFriendData* p_friend_data;

	for( i=0; i<p_wk->comm_param.num; i++ ){
		netID = p_wk->comm_param.pnetid[i];
		p_friend_data = CommInfoGetDWCFriendCode( netID );
		ret = dwc_checkfriendByToken( p_wk->comm_param.p_save,
				p_friend_data, &pos );
		switch( ret ){
		case DWCFRIEND_INLIST:// … すでに、同じデータがリスト上にある。この場合は何もする必要なし。
		case DWCFRIEND_OVERWRITE:// … すでに、同じデータがリスト上にあるが、上書きすることが望ましい場合。
			switch(p_wk->gametype){
			case WFLBY_GAME_BALLSLOW:	// 玉投げ
				WifiList_AddMinigameBallSlow( p_list, pos, 1 );
				break;
			case WFLBY_GAME_BALANCEBALL:	// 玉乗り
				WifiList_AddMinigameBalanceBall( p_list, pos, 1 );
				break;
			case WFLBY_GAME_BALLOON:		// ふうせんわり
				WifiList_AddMinigameBalloon( p_list, pos, 1 );
				break;
			}
			break;

		default:
			// 友達登録していない
			break;
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ワーク内のgametypeの値を見て、何もしていないときのスコアを取得する
 *
 *	@param	cp_wk	ワーク
 *
 *	@return	何もしてない時のスコア
 */
//-----------------------------------------------------------------------------
static u32 MNGM_RESULT_BallslowBalanceGetNoTouchScore( const MNGM_RESULTWK* cp_wk )
{
	u32 ret;
	switch( cp_wk->gametype ){
	case WFLBY_GAME_BALLSLOW:
		ret = 0;
		break;

	case WFLBY_GAME_BALANCEBALL:
		ret = MNGM_BALANCEBALL_NOTOUCH_SCORE;
		break;

	case WFLBY_GAME_BALLOON:
		// これはBALLSLOWとBALANCEBALL用
		GF_ASSERT(0);
		ret = 0;
		break;
	}
	return ret;
}


//----------------------------------------------------------------------------
/**
 *	@brief	タマ投げ　玉乗り　ミニゲーム結果データ送信
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_RESULT_BallslowBalance_SendMinigameTopResult( MNGM_RESULTWK* p_wk )
{
	int i;
	u32 topidx[ WFLBY_MINIGAME_MAX ] = {0};
	u32 topnum;
	u32 notouch_score;
	
	// 1位がいたら送信する
	topnum = 0;
	notouch_score = MNGM_RESULT_BallslowBalanceGetNoTouchScore( p_wk );
	for( i=0; i<p_wk->comm_param.num; i++ ){
		if( (p_wk->param.result[i] == 0) && (p_wk->param.score[i] != notouch_score) ){
			topidx[ topnum ] = p_wk->comm_param.p_lobby_wk->plidx.plidx[i];
			topnum ++;
		}
	}
	if( topnum > 0 ){
		WFLBY_SYSTEM_TOPIC_SendMiniGameTopResult( p_wk->comm_param.p_lobby_wk->p_wflbysystem,
				p_wk->gametype, topnum, topidx[0], topidx[1], topidx[2], topidx[3] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	風千割	ミニゲーム結果データ送信
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_RESULT_Balloon_SendMinigameTopResult( MNGM_RESULTWK* p_wk )
{
	if( p_wk->comm_param.num > 0 ){
		if( p_wk->param.balloon >= MNGM_BALLOON_GADGET_UPDATE ){

			WFLBY_SYSTEM_TOPIC_SendMiniGameTopResult( p_wk->comm_param.p_lobby_wk->p_wflbysystem,
					p_wk->gametype, p_wk->comm_param.num, 
					p_wk->comm_param.p_lobby_wk->plidx.plidx[0],
					p_wk->comm_param.p_lobby_wk->plidx.plidx[1], 
					p_wk->comm_param.p_lobby_wk->plidx.plidx[2], 
					p_wk->comm_param.p_lobby_wk->plidx.plidx[3] );
		}
	}
}





//-------------------------------------
///	START	TIMEUP	ワーク
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	OAMマスクシステム
 *
 *	@param	p_wk		ワーク
 *	@param	p_resobj	リソースオブジェ
 *	@param	p_clset		セルアクターセット
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void MNGM_COUNT_MskInit( MNGM_COUNT_MSKWK* p_wk, MNGM_CLACTRESOBJ* p_resobj, CLACT_SET_PTR p_clset, u32 heapID )
{
	int i;
	int wnd_draw;

	memset( p_wk->mskon[MNGM_COUNT_MSK_HBUF_READ], 0, 192 );
	memset( p_wk->mskon[MNGM_COUNT_MSK_HBUF_WRITE], 0, 192 );
	p_wk->count		= 0;
	p_wk->move_y	= 0;
	p_wk->p_tcb		= NULL;

	for( i=0; i<MNGM_COUNT_MSK_OAM_NUM; i++ ){
		p_wk->p_msk[i] = MNGM_CLACTRES_Add( p_resobj, p_clset, 
				sc_MNGM_COUNT_MSK_MAT[i].x >> FX32_SHIFT, 
				sc_MNGM_COUNT_MSK_MAT[i].y >> FX32_SHIFT, 
				MNGM_COUNT_MSK_PRI, heapID );

		CLACT_AnmChg( p_wk->p_msk[i], 5 );
		CLACT_BGPriorityChg( p_wk->p_msk[i], 0 );
	}

	// HBLANK関数設定
	wnd_draw = GX_GetVisibleWnd();
	wnd_draw &= ~GX_WNDMASK_W0;
	GX_SetVisibleWnd( wnd_draw );
	G2_SetWnd0InsidePlane( GX_WND_PLANEMASK_BG0|GX_WND_PLANEMASK_BG1|GX_WND_PLANEMASK_BG2|GX_WND_PLANEMASK_BG3|GX_WND_PLANEMASK_OBJ, TRUE );
	G2_SetWnd0Position( 0, 0, 0, 0 );
	p_wk->off_outplane	= G2_GetWndOutsidePlane(); 
	p_wk->def_wnd1 = GX_GetVisibleWnd() & GX_WNDMASK_W1;
	p_wk->on_outplane.planeMask	= GX_WND_PLANEMASK_BG0|GX_WND_PLANEMASK_BG1|GX_WND_PLANEMASK_BG2|GX_WND_PLANEMASK_BG3;
	p_wk->on_outplane.effect	= TRUE;
	sys_HBlankIntrSet( MNGM_COUNT_MskSetMskHBlank, p_wk );


	// 下はフリップさせる
	CLACT_SetFlip( p_wk->p_msk[ MNGM_COUNT_MSK_OAM_BTTM ], CLACT_FLIP_V );

	// マスク設定
	MNGM_COUNT_MskSetMsk( p_wk );

	
}

//----------------------------------------------------------------------------
/**
 *	@brief	マスクメイン	
 *
 *	@param	p_wk	ワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
static BOOL MNGM_COUNT_MskMain( MNGM_COUNT_MSKWK* p_wk )
{
	BOOL ret = FALSE;

	p_wk->count ++;
	if( p_wk->count >= MNGM_COUNT_MSK_COUNT ){
		p_wk->count = MNGM_COUNT_MSK_COUNT;
		ret = TRUE;
	}
	MNGM_COUNT_MskCommon( p_wk, 1 );
		
	return ret;
}
static BOOL MNGM_COUNT_MskReMain( MNGM_COUNT_MSKWK* p_wk )
{
	BOOL ret = FALSE;

	p_wk->count --;
	if( p_wk->count <= 0 ){
		p_wk->count = 1;
		ret = TRUE;

		CLACT_SetDrawFlag( p_wk->p_msk[0], FALSE );
		CLACT_SetDrawFlag( p_wk->p_msk[1], FALSE );
	}
	MNGM_COUNT_MskCommon( p_wk, -1 );
		
	return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	マスク	動作	共通部分
 *
 *	@param	p_wk 
 *	@param	add
 */
//-----------------------------------------------------------------------------
static void MNGM_COUNT_MskCommon( MNGM_COUNT_MSKWK* p_wk, s32 add )
{
	VecFx32 mat;
	int i;
	int move_y_oam;

	p_wk->move_y = (p_wk->count * MNGM_COUNT_MSK_MY) / MNGM_COUNT_MSK_COUNT;

	if( ((p_wk->count+add) <= MNGM_COUNT_MSK_COUNT) && ((p_wk->count+add) >= 0) ){
		move_y_oam = ((p_wk->count+add) * MNGM_COUNT_MSK_MY) / MNGM_COUNT_MSK_COUNT;
	}else{
		move_y_oam = p_wk->move_y;
	}
			
	for( i=0; i<MNGM_COUNT_MSK_OAM_NUM; i++ ){
		mat = sc_MNGM_COUNT_MSK_MAT[i];
		if( i==0 ){
			mat.y -= FX32_CONST( move_y_oam );
		}else{
			mat.y += FX32_CONST( move_y_oam );
		}

		CLACT_SetMatrix( p_wk->p_msk[i], &mat );
	}

	MNGM_COUNT_MskSetMsk( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	マスクシステム	破棄
 *		
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_COUNT_MskExit( MNGM_COUNT_MSKWK* p_wk )
{
	int i;

	if( p_wk->p_tcb ){
		TCB_Delete( p_wk->p_tcb );
		p_wk->p_tcb = NULL;
	}

	for( i=0; i<MNGM_COUNT_MSK_OAM_NUM; i++ ){
		if( p_wk->p_msk[i] != NULL ){
			CLACT_Delete( p_wk->p_msk[i] );
			p_wk->p_msk[i] = NULL;
		}
	}

	// HBLANK破棄
	sys_HBlankIntrStop();

	// ウィンドウマスク破棄
	GX_SetVisibleWnd( p_wk->def_wnd1 );
	G2_SetWndOutsidePlane( p_wk->off_outplane.planeMask, p_wk->off_outplane.effect );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ウィンドウマスク設定
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_COUNT_MskSetMsk( MNGM_COUNT_MSKWK* p_wk )
{
	int i;

	memset( p_wk->mskon[MNGM_COUNT_MSK_HBUF_WRITE], 0, 192 );
	// マスク設定バッファを作成する
	// 上側
	for( i=0; i<MNGM_COUNT_MSK_MY-p_wk->move_y; i++ ){
		p_wk->mskon[MNGM_COUNT_MSK_HBUF_WRITE][ MNGM_COUNT_MSK_SY0+i ] = TRUE;
		p_wk->mskon[MNGM_COUNT_MSK_HBUF_WRITE][ MNGM_COUNT_MSK_SY1-i ] = TRUE;
	}
	
	if( p_wk->p_tcb == NULL ){
		
		p_wk->p_tcb = VWaitTCB_Add( MNGM_COUNT_MskSetMskVBlank, p_wk, 0 );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	VBlank期間中にウィンドウ設定
 *
 *	@param	tcb		TCB
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_COUNT_MskSetMskVBlank( TCB_PTR tcb, void* p_work )
{
	MNGM_COUNT_MSKWK* p_wk = p_work;

	// バッファをスイッチ
	memcpy( p_wk->mskon[MNGM_COUNT_MSK_HBUF_READ], p_wk->mskon[MNGM_COUNT_MSK_HBUF_WRITE], 192 );

	TCB_Delete( p_wk->p_tcb );
	p_wk->p_tcb = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	HBLANK関数
 *
 *	@param	p_work	ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_COUNT_MskSetMskHBlank( void* p_work )
{
	MNGM_COUNT_MSKWK*	p_wk = p_work;
	s32					vcount;
	int wnd_draw;
	
	vcount = GX_GetVCount();

	vcount = (vcount + 1) % 256;	// 次のラインの設定を行う
	
	if( vcount < 192 ){
		wnd_draw = GX_GetVisibleWnd();

#if PL_T0849_080709_FIX
		if( GX_IsHBlank() ){
			if( (vcount < MNGM_COUNT_MSK_SY0) || (vcount > MNGM_COUNT_MSK_SY1) ){
				G2_SetWndOutsidePlane( p_wk->off_outplane.planeMask, p_wk->off_outplane.effect );
				wnd_draw |= p_wk->def_wnd1;
			}else{
				G2_SetWndOutsidePlane( p_wk->on_outplane.planeMask, p_wk->on_outplane.effect );
				wnd_draw &= ~(GX_WNDMASK_W1 | p_wk->def_wnd1);
			}
			

			if( p_wk->mskon[ MNGM_COUNT_MSK_HBUF_READ ][ vcount ] == TRUE ){
				GX_SetVisibleWnd( wnd_draw|GX_WNDMASK_W0 );
			}else{
				wnd_draw &= ~GX_WNDMASK_W0;
				GX_SetVisibleWnd( wnd_draw );
			}
		}
#else

		if( (vcount < MNGM_COUNT_MSK_SY0) || (vcount > MNGM_COUNT_MSK_SY1) ){
			G2_SetWndOutsidePlane( p_wk->off_outplane.planeMask, p_wk->off_outplane.effect );
			wnd_draw |= p_wk->def_wnd1;
		}else{
			G2_SetWndOutsidePlane( p_wk->on_outplane.planeMask, p_wk->on_outplane.effect );
			wnd_draw &= ~(GX_WNDMASK_W1 | p_wk->def_wnd1);
		}
		

		if( p_wk->mskon[ MNGM_COUNT_MSK_HBUF_READ ][ vcount ] == TRUE ){
			GX_SetVisibleWnd( wnd_draw|GX_WNDMASK_W0 );
		}else{
			wnd_draw &= ~GX_WNDMASK_W0;
			GX_SetVisibleWnd( wnd_draw );
		}
#endif
	}
}



//----------------------------------------------------------------------------
/**
 *	@brief	開始	エフェクト	タスク
 *
 *	@param	tcb			TCB
 *	@param	p_work		ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_COUNT_StartTcb( TCB_PTR tcb, void* p_work )
{
	MNGM_COUNTWK* p_wk = p_work;
	BOOL result;
	u16 last_seq;
	u16 now_seq;

	switch( p_wk->seq ){
	case MNGM_COUNT_START_SEQ_NONE:
		break;
		
	// 初期化
	case MNGM_COUNT_START_SEQ_INIT:
		MNGM_COUNT_MskInit( &p_wk->msk, p_wk->p_resobj, 
				p_wk->p_clset, p_wk->heapID );

		p_wk->p_anm = MNGM_CLACTRES_Add( p_wk->p_resobj, 
				p_wk->p_clset, 
				MNGM_COUNT_ANM_X, 
				MNGM_COUNT_ANM_Y, 
				MNGM_COUNT_ANM_PRI, p_wk->heapID );

		CLACT_BGPriorityChg( p_wk->p_anm, 0 );
		p_wk->seq ++;
		break;
		
	// マスクシステム
	case MNGM_COUNT_START_SEQ_MSK:
		result = MNGM_COUNT_MskMain( &p_wk->msk );
		if( result == TRUE ){
			p_wk->seq ++;
			p_wk->count = 0;
		}
		break;
		
	// アニメ
	case MNGM_COUNT_START_SEQ_ANM:

		// 変更前のフレームNO
		last_seq = CLACT_AnmFrameGet( p_wk->p_anm );
		
		// アニメ
		CLACT_AnmFrameChg( p_wk->p_anm, FX32_CONST(2) );

		// 変更後のふれーむNO
		now_seq = CLACT_AnmFrameGet( p_wk->p_anm );

		// 変更前と後でフレームが切り替わったら音を出す
		if( last_seq != now_seq ){
			switch( now_seq ){
			case MNGM_COUNT_REDY_ANM:
				Snd_SePlay( MNGM_SND_COUNT );
				break;
				
			case MNGM_COUNT_3_ANM:
				Snd_SePlay( MNGM_SND_COUNT );
				break;
				
			case MNGM_COUNT_2_ANM:
				Snd_SePlay( MNGM_SND_COUNT );
				break;
				
			case MNGM_COUNT_1_ANM:
				Snd_SePlay( MNGM_SND_COUNT );
				break;
				
			case MNGM_COUNT_STARTFRAME:
				Snd_SePlay( MNGM_SND_START );
				break;

			default:
				break;
			}
		}

		// パレットアニメ
		if( now_seq == MNGM_COUNT_STARTFRAME ){
			if( p_wk->count == 0 ){
				CLACT_PaletteOffsetChgAddTransPlttNo( p_wk->p_anm, 0 );
			}else if( p_wk->count == MNGM_COUNT_STARTPALANMTIMING ){
				CLACT_PaletteOffsetChgAddTransPlttNo( p_wk->p_anm, 1 );
			}

			p_wk->count ++;
			if( p_wk->count >= MNGM_COUNT_STARTPALANMTIMING*2 ){
				p_wk->count = 0;
			}
		}
		
		result = CLACT_AnmActiveCheck( p_wk->p_anm );
		if( result == FALSE ){
			p_wk->seq ++;
			CLACT_PaletteOffsetChgAddTransPlttNo( p_wk->p_anm, 0 );
		}
		break;

	case MNGM_COUNT_START_SEQ_MSK2:
		result = MNGM_COUNT_MskReMain( &p_wk->msk );
		if( result == TRUE ){
			// アクター破棄
			CLACT_Delete( p_wk->p_anm );
			p_wk->p_anm = NULL;
			p_wk->seq ++;
			p_wk->count = 0;
		}
		break;

	case MNGM_COUNT_START_SEQ_END:
		p_wk->count ++;
		if( p_wk->count < 2 ){
			break;
		}

		p_wk->seq = MNGM_COUNT_START_SEQ_NONE;

		// マスクシステム破棄
		MNGM_COUNT_MskExit( &p_wk->msk );

		// 動作破棄
		TCB_Delete( p_wk->p_tcb );
		p_wk->p_tcb = NULL;
		break;

	default:
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	終了	エフェクト	タスク
 *
 *	@param	tcb			TCB
 *	@param	p_work		ワーク
 */
//-----------------------------------------------------------------------------
static void MNGM_COUNT_TimeUpTcb( TCB_PTR tcb, void* p_work )
{
	MNGM_COUNTWK* p_wk = p_work;
	BOOL result;

	switch( p_wk->seq ){
	case MNGM_COUNT_TIMEUP_SEQ_NONE:
		break;
		
	// 初期化
	case MNGM_COUNT_TIMEUP_SEQ_INIT:
		MNGM_COUNT_MskInit( &p_wk->msk, p_wk->p_resobj, 
				p_wk->p_clset, p_wk->heapID );

		p_wk->p_anm = MNGM_CLACTRES_Add( p_wk->p_resobj, 
				p_wk->p_clset, 
				MNGM_COUNT_ANM_X, 
				MNGM_COUNT_ANM_Y, 
				MNGM_COUNT_ANM_PRI, p_wk->heapID );

		CLACT_AnmChg( p_wk->p_anm, 1 );
		CLACT_BGPriorityChg( p_wk->p_anm, 0 );
		p_wk->seq ++;
		break;
		
	// マスクシステム
	case MNGM_COUNT_TIMEUP_SEQ_MSK:
		result = MNGM_COUNT_MskMain( &p_wk->msk );
		if( result == TRUE ){
			CLACT_SetAnmFlag( p_wk->p_anm, TRUE );
			p_wk->seq ++;
			p_wk->count = 0;
		}
		break;
		
	// アニメ
	case MNGM_COUNT_TIMEUP_SEQ_ANM:

		p_wk->count ++;
		if( p_wk->count >= MNGM_COUNT_TIMEUP_ANM_NUM ){
			p_wk->seq ++;
		}
		break;

	case MNGM_COUNT_TIMEUP_SEQ_MSK2:
		result = MNGM_COUNT_MskReMain( &p_wk->msk );
		if( result == TRUE ){
			// アクター破棄
			CLACT_Delete( p_wk->p_anm );
			p_wk->p_anm = NULL;
			p_wk->count = 0;
			p_wk->seq ++;
		}
		break;

	case MNGM_COUNT_TIMEUP_SEQ_END:
		p_wk->count ++;
		if( p_wk->count < 2 ){
			break;
		}
		p_wk->seq = MNGM_COUNT_TIMEUP_SEQ_NONE;

		// マスクシステム破棄
		MNGM_COUNT_MskExit( &p_wk->msk );

		// 動作破棄
		TCB_Delete( p_wk->p_tcb );
		p_wk->p_tcb = NULL;
		break;
		
	default:
		break;
	}
}



