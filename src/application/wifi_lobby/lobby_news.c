//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		lobby_news.c
 *	@brief		ロビーニュース
 *	@author		tomoya takahashi
 *	@data		2007.10.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#include "gflib/strbuf.h"
#include "gflib/msg_print.h"

#include "system/clact_util.h"
#include "system/wipe.h"
#include "system/render_oam.h"
#include "system/fontproc.h"
#include "system/msgdata.h"
#include "system/wordset.h"

#include "communication/communication.h"

#include "src/graphic/lobby_news.naix"

#include "msgdata/msg.naix"
#include "msgdata/msg_wflby_news.h"

#include "lobby_news.h"
#include "lobby_news_data.h"
#include "lobby_news_snd.h"
#include "wflby_snd.h"




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
#define WFLBY_DEBUG_LOBBY_NEWS_TOPIC_MAKE
#endif

#ifdef WFLBY_DEBUG_LOBBY_NEWS_TOPIC_MAKE
static u32 sc_WFLBY_DEBUG_LOBBY_NEWS_TOPIC_MAKE_TOPIC = NEWS_TOPICTYPE_CONNECT;
#endif


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	Vram転送マネージャ　タスク数
//=====================================
#define NEWSDRAW_VRAMTRANS_TASKNUM	( 48 )

//-------------------------------------
///	OAM設定
//=====================================
#define NEWSDRAW_OAM_CONTNUM	( 32 )
#define NEWSDRAW_RESMAN_NUM		( 4 )	// OAMリソースマネージャ数
#define NEWSDRAW_SF_MAT_Y		( FX32_CONST(256) )
static const CHAR_MANAGER_MAKE sc_NEWSDRAW_CHARMAN_INIT = {
	NEWSDRAW_OAM_CONTNUM,
	96*1024,	// 96K
	16*1024,	// 16K
	HEAPID_NEWSDRAW
};


//-------------------------------------
///	BANK設定
//=====================================
static const GF_BGL_DISPVRAM sc_NEWSDRAW_BANK = {
	GX_VRAM_BG_256_AB,				// メイン2DエンジンのBG
	GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット
	GX_VRAM_SUB_BG_128_C,			// サブ2DエンジンのBG
	GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット
	GX_VRAM_OBJ_96_EFG,				// メイン2DエンジンのOBJ
	GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット
	GX_VRAM_SUB_OBJ_16_I,			// サブ2DエンジンのOBJ
	GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット
	GX_VRAM_TEX_NONE,				// テクスチャイメージスロット
	GX_VRAM_TEXPLTT_NONE			// テクスチャパレットスロット
};

//-------------------------------------
///	BG設定
//=====================================
static const GF_BGL_SYS_HEADER sc_BGINIT = {
	GX_DISPMODE_GRAPHICS,
	GX_BGMODE_0,
	GX_BGMODE_0,
	GX_BG0_AS_2D
};


//-------------------------------------
///	ＢＧコントロールデータ
//=====================================
#define NEWSDRAW_BGCNT_NUM	( 5 )	// ＢＧコントロールテーブル数
static const u32 sc_NEWSDRAW_BGCNT_FRM[ NEWSDRAW_BGCNT_NUM ] = {
	GF_BGL_FRAME0_M,
	GF_BGL_FRAME1_M,
	GF_BGL_FRAME2_M,
	GF_BGL_FRAME3_M,
	GF_BGL_FRAME0_S,
};
static const GF_BGL_BGCNT_HEADER sc_NEWSDRAW_BGCNT_DATA[ NEWSDRAW_BGCNT_NUM ] = {
	{	// GF_BGL_FRAME0_M	メッセージ面
		0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
		0, 0, 0, FALSE
	},
	{	// GF_BGL_FRAME1_M	フレーム面
		0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
		1, 0, 0, FALSE
	},
	{	// GF_BGL_FRAME2_M	背景
		0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xd800, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
		3, 0, 0, FALSE
	},
	{	// GF_BGL_FRAME3_M	いろいろデータ表示
		0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xd000, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
		2, 0, 0, FALSE
	},
	
	// サブ
	{	// GF_BGL_FRAME0_S
		0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
		0, 0, 0, FALSE
	},
};

//-------------------------------------
///	パレットナンバー
//=====================================
enum {
	NEWS_PLTT_BACK,
	NEWS_PLTT_TIME,
	NEWS_PLTT_PLAYER,
	NEWS_PLTT_YOU,
	NEWS_PLTT_VIP,
	NEWS_PLTT_SUB,
	NEWS_PLTT_DUMMY00,
	NEWS_PLTT_FONT,
	NEWS_PLTT_FONT01,
	NEWS_PLTT_FONT02,
	NEWS_PLTT_TALKFONT,
} ;

//-------------------------------------
///	メッセージパレット内読み込み時カラーオフセット
//=====================================
enum {
	NEWS_PLTT_FONT_BACK,	// 背景カラー
	NEWS_PLTT_FONT_NORM,	// 通常文字色
	NEWS_PLTT_FONT_NORM_B,	// 通常文字色背景
	NEWS_PLTT_FONT_RED,		// 強調文字色
	NEWS_PLTT_FONT_RED_B,	// 強調文字色背景
	NEWS_PLTT_FONT_TRN,		// 通常トレーナー文字色
	NEWS_PLTT_FONT_TRN_B,	// 通常トレーナー文字色背景
	NEWS_PLTT_FONT_FONT,	// 通常文字色
	NEWS_PLTT_FONT_FONT_B,	// 通常文字色背景
	NEWS_PLTT_FONT_TRVIP,	// VIPトレーナー文字色
	NEWS_PLTT_FONT_TRVIP_B,	// VIPトレーナー文字色背景
} ;

//-------------------------------------
///	メッセージパレット内転送時カラーオフセット
//=====================================
enum {
	NEWS_PLTT_FONTTR_BACK,		// 背景カラー
	NEWS_PLTT_FONTTR_NORM,		// 通常文字色
	NEWS_PLTT_FONTTR_NORM_B,	// 通常文字色背景
	NEWS_PLTT_FONTTR_RED,		// 強調文字色
	NEWS_PLTT_FONTTR_RED_B,		// 強調文字色背景
	NEWS_PLTT_FONTTR_TR_00,		// トレーナー0文字色
	NEWS_PLTT_FONTTR_TR_00_B,	// トレーナー0文字色背景
	NEWS_PLTT_FONTTR_TR_01,		// トレーナー1文字色
	NEWS_PLTT_FONTTR_TR_01_B,	// トレーナー1文字色背景
	NEWS_PLTT_FONTTR_TR_02,		// トレーナー2文字色
	NEWS_PLTT_FONTTR_TR_02_B,	// トレーナー2文字色背景
	NEWS_PLTT_FONTTR_TR_03,		// トレーナー3文字色
	NEWS_PLTT_FONTTR_TR_03_B,	// トレーナー3文字色背景
	NEWS_PLTT_FONTTR_FONT,		// 通常文字色
	NEWS_PLTT_FONTTR_FONT_B,	// 通常文字色背景
} ;






//-------------------------------------
///	メインシーケンス
//=====================================
enum{
	NEWSDRAW_SEQ_FADEIN,
	NEWSDRAW_SEQ_FADEINWAIT,
	NEWSDRAW_SEQ_MAIN,
	NEWSDRAW_SEQ_FADEOUT,
	NEWSDRAW_SEQ_FADEOUTWAIT,
};

//-------------------------------------
///	スクリーンセット
//=====================================
enum{
	NEWSDRAW_TIME_BLOCKTYPE_NORMAL,
	NEWSDRAW_TIME_BLOCKTYPE_ICON,
	NEWSDRAW_TIME_BLOCKTYPE_RED,
	NEWSDRAW_TIME_BLOCKTYPE_NONE,
	NEWSDRAW_TIME_BLOCKTYPE_ICON1,
	NEWSDRAW_TIME_BLOCKTYPE_ICON2,
	NEWSDRAW_TIME_BLOCKTYPE_ICON3,
};
#define NEWSDRAW_SCRN_TIMEBLOCK_X	( 6 )	// スクリーン切り取り先
#define NEWSDRAW_SCRN_TIMEBLOCK_Y	( 0 )
#define NEWSDRAW_TIME_BLOCK_SIZX		(1)		// ブロックサイズX
#define NEWSDRAW_TIME_BLOCK_SIZY		(2)		// ブロックサイズY
#define NEWSDRAW_TIME_SCRN_DRAWX		(5)		// 書き込み開始左上キャラ位置
#define NEWSDRAW_TIME_SCRN_DRAWY		(4)		// 書き込み開始左上キャラ位置
enum{
	NEWSDRAW_PLAYERTYPE_NML,
	NEWSDRAW_PLAYERTYPE_OUT,
	NEWSDRAW_PLAYERTYPE_IN,
};
#define NEWSDRAW_SCRN_PLAYERBLOCK_X	(0)		// スクリーン切り取り先
#define NEWSDRAW_SCRN_PLAYERBLOCK_Y	(0)
#define NEWSDRAW_PLAYER_BLOCK_SIZX		(2)		// ブロックサイズX
#define NEWSDRAW_PLAYER_BLOCK_SIZY		(2)		// ブロックサイズY
#define NEWSDRAW_PLAYER_SCRN_DRAWX		(21)	// 書き込み開始左上キャラ位置
#define NEWSDRAW_PLAYER_SCRN_DRAWY		(4)		// 書き込み開始左上キャラ位置
#define NEWSDRAW_PLAYER_SCRN_PAT_X		(3)		// スクリーンに書き込まれている人のパターンの列
#define NEWSDRAW_PLAYER_SCRN_PAT_Y		(4)
#define NEWSDRAW_PLAYER_SCRN_OLDPAT_Y	(2)		// 古い人表示パターン開始位置


//-------------------------------------
///	トピックウィンドウ
//=====================================
#define	NEWSDRAW_TOPIC_NUM		( 3 )		// 表示するトピックスの数
#define NEWSDRAW_TOPIC_STRNUM	( 256 )		// 文字列数
#define NEWSDRAW_TOPIC_COUNT1	( 7 )		// １文字分のカウント
#define NEWSDRAW_TOPIC_COUNT2	( 6 )		// １文字分のカウント
#define NEWSDRAW_TOPIC_COUNT3	( 5 )		// １文字分のカウント
#define NEWSDRAW_TOPIC_CHARSIZ	( 12 )		// １文字のサイズ
#define NEWSDRAW_TOPIC_YOHAKU	( 256 )		// 文字を表示するときの余白（上下につく）
static const u8 NEWSDRAW_TOPIC_DRAW_Y[ NEWSDRAW_TOPIC_NUM ] = {
	15,
	18,
	21,
};
/*
static const u8 NEWSDRAW_TOPIC_COUNT[ NEWSDRAW_TOPIC_NUM ] = {
	NEWSDRAW_TOPIC_COUNT1,
	NEWSDRAW_TOPIC_COUNT2,
	NEWSDRAW_TOPIC_COUNT3,
};//*/
// BTS通信バグ601の対処	tomoya
#define NEWSDRAW_TOPIC_COUNT_STRNUM	( 48 )
static const u32 NEWSDRAW_TOPIC_COUNT[ NEWSDRAW_TOPIC_NUM ] = {
	NEWSDRAW_TOPIC_COUNT1*NEWSDRAW_TOPIC_COUNT_STRNUM,
	NEWSDRAW_TOPIC_COUNT2*NEWSDRAW_TOPIC_COUNT_STRNUM,
	NEWSDRAW_TOPIC_COUNT3*NEWSDRAW_TOPIC_COUNT_STRNUM,
};
#define NEWSDRAW_TOPIC_BMP_X	( 1 )		// ビットマップX
#define NEWSDRAW_TOPIC_BMP_SX	( 30 )		// ビットマップサイズX
#define NEWSDRAW_TOPIC_BMP_SY	( 2 )		// ビットマップサイズY
#define NEWSDRAW_TOPIC_BMP_CGX		( 0x300 )	// CGX
#define NEWSDRAW_TOPIC_BMP_CGXSIZ	( NEWSDRAW_TOPIC_BMP_SX*NEWSDRAW_TOPIC_BMP_SY )
#define NEWSDRAW_TOPIC_BMP_PAL	( NEWS_PLTT_FONT )	// パレット
#define NEWSDRAW_TOPIC_DMBMP_SX	( 180 )		// ビットマップサイズX(120文字入る)
#define NEWSDRAW_TOPIC_DMBMP_SY	( 2 )		// ビットマップサイズY



//-------------------------------------
///	時間ウィンドウ
//=====================================
#define NEWSDRAW_TIME_EFFECT_COUNT		(16)	// モザイクエフェクトウエイト
#define NEWSDRAW_TIME_TIME_EFFECT_COUNT	(32)	// モザイクエフェクトウエイト
#define NEWSDRAW_TIME_Y_SIZ				(4)		// 描画エリアブロックサイズ
#define NEWSDRAW_TIME_X_SIZ				(10)	// 描画エリアブロックサイズ
#define NEWSDRAW_TIME_BLOCKNUM		(NEWSDRAW_TIME_Y_SIZ*NEWSDRAW_TIME_X_SIZ)	// 描画エリアブロックサイズ
typedef struct{
	u32 icon;			// 表示するアイコン
} NEWSDRAW_TIME_EFF_ANM;
#define NEWSDRAW_TIME_EFF_DATA_NUM		( 4 )
static const NEWSDRAW_TIME_EFF_ANM NEWSDRAW_TIME_EFF_ANM_DATA[ NEWSDRAW_TIME_EFF_DATA_NUM ] = {
	{ NEWSDRAW_TIME_BLOCKTYPE_ICON },
	{ NEWSDRAW_TIME_BLOCKTYPE_ICON1 },
	{ NEWSDRAW_TIME_BLOCKTYPE_ICON2 },
	{ NEWSDRAW_TIME_BLOCKTYPE_ICON3 },
};


//-------------------------------------
///	プレイヤーウィンドウ
//=====================================
enum{
	NEWSDRAW_PLAYERWIN_SEQ_WAIT,		// 待機状態
	NEWSDRAW_PLAYERWIN_SEQ_INSIDE,		// 人が入ってきた
	NEWSDRAW_PLAYERWIN_SEQ_INSIDEMAIN,	// 人が入ってきた
	NEWSDRAW_PLAYERWIN_SEQ_OUTSIDE,		// 人が入ってきた
	NEWSDRAW_PLAYERWIN_SEQ_OUTSIDEMAIN,	// 人が入ってきた
};
#define	NEWSDRAW_PLAYERWIN_COUNT_INSIDE		( 32 )	// 入室カウント
#define	NEWSDRAW_PLAYERWIN_COUNT_OUTSIDE	( 32 )	// 退室カウント
#define NEWSDRAW_PLAYERWIN_ANIME_NUM		( 4 )	// この回数絵が切り替わる
#define	NEWSDRAW_PLAYERWIN_INSIDE_ONEFRAME	( NEWSDRAW_PLAYERWIN_COUNT_INSIDE/4 )	// 入室カウント
#define	NEWSDRAW_PLAYERWIN_OUTSIDE_ONEFRAME	( NEWSDRAW_PLAYERWIN_COUNT_OUTSIDE/4 )	// 退室カウント
#define NEWSDRAW_PLAYERWIN_DRAW_X			( 5 )	// 表示人数
#define NEWSDRAW_PLAYERWIN_DRAW_Y			( 4 )	// 表示人数


//-------------------------------------
///	タイトルウィンドウ
//=====================================
#define NEWSDRAW_TITLEWIN_STRNUM	( 128 )	// STRBUFの文字数
#define NEWSDRAW_TITLEWIN_COL		( GF_PRINTCOLOR_MAKE( NEWS_PLTT_FONTTR_FONT, NEWS_PLTT_FONTTR_FONT_B, 0 ) )	// 文字カラー
#define NEWSDRAW_TITLEWIN_APL_COL	( GF_PRINTCOLOR_MAKE( 3, 4, 0 ) )	// 文字カラー
enum {
	NEWSDRAW_TITLEWIN_APLNAME,	// この遊びの名前
	NEWSDRAW_TITLEWIN_TIME,		// 時間
	NEWSDRAW_TITLEWIN_PLAYER,	// プレイヤー表示
	NEWSDRAW_TITLEWIN_TOPIC,	// トピック
	NEWSDRAW_TITLEWIN_NUM,
} ;
// アプリ名
#define NEWSDRAW_TITLEWIN_APLNAME_X		( 11 )		// ビットマップX
#define NEWSDRAW_TITLEWIN_APLNAME_Y		( 0 )		// ビットマップY
#define NEWSDRAW_TITLEWIN_APLNAME_SX	( 20 )		// ビットマップサイズX
#define NEWSDRAW_TITLEWIN_APLNAME_SY	( 8 )		// ビットマップサイズY
#define NEWSDRAW_TITLEWIN_APLNAME_CGX	( 0x1 )	// CGX
#define NEWSDRAW_TITLEWIN_APLNAME_PAL	( NEWS_PLTT_TALKFONT )	// パレット
#define NEWSDRAW_TITLEWIN_APLNAME_DRAW_X	( 0 )
#define NEWSDRAW_TITLEWIN_APLNAME_DRAW_Y	( 6 )

// 時間
#define NEWSDRAW_TITLEWIN_TIME_X		( 0 )		// ビットマップX
#define NEWSDRAW_TITLEWIN_TIME_Y		( 6 )		// ビットマップY
#define NEWSDRAW_TITLEWIN_TIME_SX		( 6 )		// ビットマップサイズX
#define NEWSDRAW_TITLEWIN_TIME_SY		( 3 )		// ビットマップサイズY
#define NEWSDRAW_TITLEWIN_TIME_CGX		( NEWSDRAW_TITLEWIN_APLNAME_CGX + (NEWSDRAW_TITLEWIN_APLNAME_SX*NEWSDRAW_TITLEWIN_APLNAME_SY) )	// CGX
#define NEWSDRAW_TITLEWIN_TIME_PAL		( NEWS_PLTT_FONT )	// パレット
#define NEWSDRAW_TITLEWIN_TIME_DRAW_X	( 2 )
#define NEWSDRAW_TITLEWIN_TIME_DRAW_Y	( 4 )

// プレイヤー表示
#define NEWSDRAW_TITLEWIN_PLAYER_X		( 15 )		// ビットマップX
#define NEWSDRAW_TITLEWIN_PLAYER_Y		( 6 )		// ビットマップY
#define NEWSDRAW_TITLEWIN_PLAYER_SX		( 6 )		// ビットマップサイズX
#define NEWSDRAW_TITLEWIN_PLAYER_SY		( 5 )		// ビットマップサイズY
#define NEWSDRAW_TITLEWIN_PLAYER_CGX	( NEWSDRAW_TITLEWIN_TIME_CGX + (NEWSDRAW_TITLEWIN_TIME_SX*NEWSDRAW_TITLEWIN_TIME_SY) )	// CGX
#define NEWSDRAW_TITLEWIN_PLAYER_PAL	( NEWS_PLTT_FONT )	// パレット
#define NEWSDRAW_TITLEWIN_PLAYER_DRAW_X	( 4 )
#define NEWSDRAW_TITLEWIN_PLAYER_DRAW_Y	( 4 )

// トピック表示
#define NEWSDRAW_TITLEWIN_TOPIC_X		( 4 )		// ビットマップX
#define NEWSDRAW_TITLEWIN_TOPIC_Y		( 12 )		// ビットマップY
#define NEWSDRAW_TITLEWIN_TOPIC_SX		( 18 )		// ビットマップサイズX
#define NEWSDRAW_TITLEWIN_TOPIC_SY		( 3 )		// ビットマップサイズY
#define NEWSDRAW_TITLEWIN_TOPIC_CGX		( NEWSDRAW_TITLEWIN_PLAYER_CGX + (NEWSDRAW_TITLEWIN_PLAYER_SX*NEWSDRAW_TITLEWIN_PLAYER_SY) )	// CGX
#define NEWSDRAW_TITLEWIN_TOPIC_PAL		( NEWS_PLTT_FONT )	// パレット
#define NEWSDRAW_TITLEWIN_TOPIC_DRAW_X	( 4 )
#define NEWSDRAW_TITLEWIN_TOPIC_DRAW_Y	( 4 )

#ifdef NONEQUIVALENT
typedef struct{
	u8	x;
	u8	y;
	u8	sizx;
	u8	sizy;
	u16	cgx;
	u8	pal;
	u8	dx:4;
	u8	dy:4;
} NEWSDRAW_BMPDATA;
static const NEWSDRAW_BMPDATA NEWSDRAW_TITLE_BMPDATA[ NEWSDRAW_TITLEWIN_NUM ] = {
	{
		NEWSDRAW_TITLEWIN_APLNAME_X,
		NEWSDRAW_TITLEWIN_APLNAME_Y, 
		NEWSDRAW_TITLEWIN_APLNAME_SX, 
		NEWSDRAW_TITLEWIN_APLNAME_SY,
		NEWSDRAW_TITLEWIN_APLNAME_CGX,
		NEWSDRAW_TITLEWIN_APLNAME_PAL,
		NEWSDRAW_TITLEWIN_APLNAME_DRAW_X,NEWSDRAW_TITLEWIN_APLNAME_DRAW_Y
	},
	{
		NEWSDRAW_TITLEWIN_TIME_X,
		NEWSDRAW_TITLEWIN_TIME_Y, 
		NEWSDRAW_TITLEWIN_TIME_SX, 
		NEWSDRAW_TITLEWIN_TIME_SY,
		NEWSDRAW_TITLEWIN_TIME_CGX,
		NEWSDRAW_TITLEWIN_TIME_PAL,
		NEWSDRAW_TITLEWIN_TIME_DRAW_X,NEWSDRAW_TITLEWIN_TIME_DRAW_Y
	},
	{
		NEWSDRAW_TITLEWIN_PLAYER_X,
		NEWSDRAW_TITLEWIN_PLAYER_Y, 
		NEWSDRAW_TITLEWIN_PLAYER_SX, 
		NEWSDRAW_TITLEWIN_PLAYER_SY,
		NEWSDRAW_TITLEWIN_PLAYER_CGX,
		NEWSDRAW_TITLEWIN_PLAYER_PAL,
		NEWSDRAW_TITLEWIN_PLAYER_DRAW_X, NEWSDRAW_TITLEWIN_PLAYER_DRAW_Y
	},
	{
		NEWSDRAW_TITLEWIN_TOPIC_X,
		NEWSDRAW_TITLEWIN_TOPIC_Y, 
		NEWSDRAW_TITLEWIN_TOPIC_SX, 
		NEWSDRAW_TITLEWIN_TOPIC_SY,
		NEWSDRAW_TITLEWIN_TOPIC_CGX,
		NEWSDRAW_TITLEWIN_TOPIC_PAL,
		NEWSDRAW_TITLEWIN_TOPIC_DRAW_X, NEWSDRAW_TITLEWIN_TOPIC_DRAW_Y
	},
};
#else
// MatchComment: struct definition changed + TODO__fix_me use constants + color maker
typedef struct{
	u8	x;
	u8	y;
	u8	sizx;
	u8	sizy;
	u16	cgx;
	u8	pal;
	u8	dx;
	u8	dy;
    u8  unk09Switch;
    GF_PRINTCOLOR color;
} NEWSDRAW_BMPDATA;
static const NEWSDRAW_BMPDATA NEWSDRAW_TITLE_BMPDATA[ NEWSDRAW_TITLEWIN_NUM ] = {
	{
		6,
		0,
		20,
		8,
		1,
		10,
		80, 6,
		1,
		0x00030400,
	},
	{
		0,
		6,
		6,
		3,
		161,
		7,
		16, 4,
		1,
		0x000d0e00,
	},
	{
		15,
		6,
		6,
		5,
		179,
		7,
		24, 4,
		1,
		0x000d0e00,
	},
	{
		4,
		12,
		18,
		3,
		209,
		7,
		4, 4,
		0,
		0x000d0e00,
	},
};
#endif

// パレットオフセット
static u16 NEWSDRAW_TITLEWIN_PLTT_OFFS[ NEWSDRAW_TITLEWIN_NUM ] = {
	0, 0xc*2, 0xd*2, 0xf*2
};
// 音
static u32 NEWSDRAW_TITLEWIN_SND_TBL[ NEWSDRAW_TITLEWIN_NUM ] = {
	0, NEWSDRAW_NEWTIME, NEWSDRAW_NEWPLAYER, NEWSDRAW_NEWTOPIC
};
#define NEWSDRAW_TITLEWIN_EFFECT_COUNT		( 28 )	// パレットフェードのシンク数
#define NEWSDRAW_TITLEWIN_EFFECT_COUNT_HF	( 2 )	// パレットフェードの明るくなるまでのシンク数
#define NEWSDRAW_TITLEWIN_EFFECT_COUNT_WT	( 12 )	// ウエイト
#define NEWSDRAW_TITLEWIN_EFFECT_COUNT_HF2	( NEWSDRAW_TITLEWIN_EFFECT_COUNT - (NEWSDRAW_TITLEWIN_EFFECT_COUNT_HF+NEWSDRAW_TITLEWIN_EFFECT_COUNT_WT) )	// 消えるシンク数
#define NEWSDRAW_TITLEWIN_EFFECT_COL		( 0x000e )	// 基本パレットカラー
#define NEWSDRAW_TITLEWIN_EFFECT_COL_END	( 0x0019 )	// 変更後
#define NEWSDRAW_TITLEWIN_EFFECT_SND_PLY	( 0 )	// 音を鳴らすタイミング


//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	グラフィックセット
//=====================================
typedef struct {
	void*				p_buff;
	NNSG2dScreenData*	p_scrn;
} NEWSDRAW_SCRNSET;


//-------------------------------------
///	時間ウィンドウ
//=====================================
typedef struct {
	// データ
	WFLBY_TIME	last_rest_time;	// 以前更新した残り時間
	u8			effect_flag;	// モザイクエフェクトフラグ
	u8			updata;			// 更新フラグ
	u16			count;			// カウンタ
	u32			change_second;	// ブロック描画を変更する間隔（秒）
	

	// 描画データ
} NEWSDRAW_TIMEWIN;

//-------------------------------------
///	プレイヤーウィンドウ
//=====================================
typedef struct {
	// データ
	s16 count;
	u16 seq;

	// 描画データ
} NEWSDRAW_PLAYERWIN;

//-------------------------------------
///	トピック　トレーナカラーデータ
//=====================================
typedef struct {
	u8	trcol[ WFLBY_MINIGAME_MAX ];
} NEWSDRAW_TOPIC_TRCOL;


//-------------------------------------
///	トピックスウィンドウ
//=====================================
// 1トピックスデータ
typedef struct {
	// データ	
	u8				move;
	u8				pal;	
	u16				count;
	u16				count_max;
	u16				str_siz;
	STRBUF*			p_str;
	GF_BGL_BMPWIN	bmp;

	// トピックカラーデータ
	NEWSDRAW_TOPIC_TRCOL trcol;
} NEWSDRAW_TOPIC;
typedef struct {
	// データ
	NEWSDRAW_TOPIC			topic[NEWSDRAW_TOPIC_NUM];	// トピックデータ

	// 描画データ
	GF_BGL_BMPWIN	bmp[NEWSDRAW_TOPIC_NUM];
	STRBUF*			p_str;

	//  パレットデータ
	void*				p_plttbuff;
	NNSG2dPaletteData*	p_pltt;
} NEWSDRAW_TOPICWIN;


//-------------------------------------
///	タイトルワーク
//=====================================
// エフェクトワーク
typedef struct {
	u16		on;			// エフェクト常態か
	u16		col_count;	// カウンタ
	u16		buff;		// パレットバッファ
	u16		offs;		// パレット展開オフセット
	u32		sound;		// 音
} NEWSDRAW_TITLEEFF;
// ワーク
typedef struct {
	GF_BGL_BMPWIN		bmp[ NEWSDRAW_TITLEWIN_NUM ];
	NEWSDRAW_TITLEEFF	eff[ NEWSDRAW_TITLEWIN_NUM ];	// エフェクトワーク
} NEWSDRAW_TITLEWIN;




//-------------------------------------
///	表示システム
//=====================================
typedef struct {
	// BG
	GF_BGL_INI*				p_bgl;

	// OAM
    CLACT_SET_PTR           p_clactset;		// セルアクターセット
    CLACT_U_EASYRENDER_DATA renddata;       // 簡易レンダーデータ
    CLACT_U_RES_MANAGER_PTR p_resman[NEWSDRAW_RESMAN_NUM]; // キャラ・パレットリソースマネージャ

	// アーカイブ
	ARCHANDLE* p_handle;
} NEWSDRAW_DRAWSYS;


//-------------------------------------
///	ワーク
//=====================================
typedef struct {
	// 描画データ
	NEWS_DATA*			p_data;
	const WFLBY_TIME*	cp_nowtime;

	// 描画システム
	NEWSDRAW_DRAWSYS	draw;

	// グラフィックスクリーンセット
	NEWSDRAW_SCRNSET	scrn;

	// 時間ウィンドウ
	NEWSDRAW_TIMEWIN	time;

	// プレイヤーウィンドウ
	NEWSDRAW_PLAYERWIN	player;

	// トピックウィンドウ
	NEWSDRAW_TOPICWIN	topic;

	// タイトルウィンドウ
	NEWSDRAW_TITLEWIN	title;

} NEWSDRAW_WK;



//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

// 全体
static void NEWSDRAW_WkVBlank( void* p_work );
static void NEWSDRAW_WkDraw( NEWSDRAW_WK* p_wk, const WFLBY_SYSTEM* cp_system );



// 描画システム
static void NEWSDRAW_DrawSysInit( NEWSDRAW_DRAWSYS* p_wk, u32 heapID );
static void NEWSDRAW_DrawSysExit( NEWSDRAW_DRAWSYS* p_wk );
static void NEWSDRAW_DrawSysDraw( NEWSDRAW_DRAWSYS* p_wk );
static void NEWSDRAW_DrawSysVBlank( NEWSDRAW_DRAWSYS* p_wk );
static void NEWSDRAW_DrawSysBgInit( NEWSDRAW_DRAWSYS* p_wk, u32 heapID );
static void NEWSDRAW_DrawSysBgExit( NEWSDRAW_DRAWSYS* p_wk );
static void NEWSDRAW_DrawSysOamInit( NEWSDRAW_DRAWSYS* p_wk, u32 heapID );
static void NEWSDRAW_DrawSysOamExit( NEWSDRAW_DRAWSYS* p_wk );

// スクリーンセット
static void NEWSDRAW_ScrnSetInit( NEWSDRAW_SCRNSET* p_wk, NEWSDRAW_DRAWSYS* p_draw, u32 heapID );
static void NEWSDRAW_ScrnSetExit( NEWSDRAW_SCRNSET* p_wk );
static void NEWSDRAW_ScrnWriteTimeBlock( NEWSDRAW_SCRNSET* p_wk, NEWSDRAW_DRAWSYS* p_draw, u32 block_type, u8 x, u8 y );
static void NEWSDRAW_ScrnWritePlayer( NEWSDRAW_SCRNSET* p_wk, NEWSDRAW_DRAWSYS* p_draw, u8 x, u8 y, u32 player_type, u32 sex, BOOL mydata, BOOL olddata, BOOL vip );

// 時間ウィンドウ
static void NEWSDRAW_TimeWinInit( NEWSDRAW_TIMEWIN* p_wk, NEWSDRAW_DRAWSYS* p_draw, NEWSDRAW_SCRNSET* p_scrn, const NEWS_DATA* cp_data, const WFLBY_TIME* cp_nowtime, const WFLBY_SYSTEM* cp_system, u32 heapID );
static void NEWSDRAW_TimeWinExit( NEWSDRAW_TIMEWIN* p_wk );
static BOOL NEWSDRAW_TimeWinMain( NEWSDRAW_TIMEWIN* p_wk, const NEWS_DATA* cp_data, const WFLBY_TIME* cp_nowtime );
static void NEWSDRAW_TimeWinDraw( NEWSDRAW_TIMEWIN* p_wk, NEWSDRAW_SCRNSET* p_scrn, NEWSDRAW_DRAWSYS* p_draw );
static void NEWSDRAW_TimeWinDrawMosaic( NEWSDRAW_TIMEWIN* p_wk, NEWSDRAW_SCRNSET* p_scrn, NEWSDRAW_DRAWSYS* p_draw );
static void NEWSDRAW_TimeWinDrawUpdate( NEWSDRAW_TIMEWIN* p_wk, NEWSDRAW_SCRNSET* p_scrn, NEWSDRAW_DRAWSYS* p_draw );

// プレイヤーウィンドウ
static void NEWSDRAW_PlayerWinInit( NEWSDRAW_PLAYERWIN* p_wk, NEWSDRAW_DRAWSYS* p_draw, NEWSDRAW_SCRNSET* p_scrn, const NEWS_DATA* cp_data, const WFLBY_SYSTEM* cp_system, u32 heapID );
static void NEWSDRAW_PlayerWinExit( NEWSDRAW_PLAYERWIN* p_wk );
static BOOL NEWSDRAW_PlayerWinMain( NEWSDRAW_PLAYERWIN* p_wk, const NEWS_DATA* cp_data );
static void NEWSDRAW_PlayerWinDraw( NEWSDRAW_PLAYERWIN* p_wk, NEWSDRAW_SCRNSET* p_scrn, NEWSDRAW_DRAWSYS* p_draw, const NEWS_DATA* cp_data, const WFLBY_SYSTEM* cp_system );
static void NEWSDRAW_PlayerWinWrite( NEWSDRAW_SCRNSET* p_scrn, NEWSDRAW_DRAWSYS* p_draw, const NEWS_DATA* cp_data, const WFLBY_SYSTEM* cp_system, u32 player_type );
static void NEWSDRAW_PlayerWinInsideAnim( NEWSDRAW_SCRNSET* p_scrn, NEWSDRAW_DRAWSYS* p_draw, const NEWS_DATA* cp_data, const WFLBY_SYSTEM* cp_system, u32 count );
static void NEWSDRAW_PlayerWinOutsideAnim( NEWSDRAW_SCRNSET* p_scrn, NEWSDRAW_DRAWSYS* p_draw, const NEWS_DATA* cp_data, const WFLBY_SYSTEM* cp_system, u32 count );


// トピックデータからトピックカラーデータを取得する
static void NEWSDRAW_TopicTrColDataGet( NEWSDRAW_TOPIC_TRCOL* p_wk, const NEWS_DATA* cp_data, NEWS_TOPICTYPE type, const WFLBY_SYSTEM* cp_system );
static void NEWSDRAW_TopicTrColDataTrans( const NEWSDRAW_TOPIC_TRCOL* cp_wk, const NNSG2dPaletteData* cp_pltt, u32 plttidx );

// トピックスウィンドウ
static void NEWSDRAW_TopicWinInit( NEWSDRAW_TOPICWIN* p_wk, NEWSDRAW_DRAWSYS* p_draw, u32 heapID );
static void NEWSDRAW_TopicWinExit( NEWSDRAW_TOPICWIN* p_wk );
static BOOL NEWSDRAW_TopicWinMain( NEWSDRAW_TOPICWIN* p_wk, NEWS_DATA* p_data, const WFLBY_SYSTEM* cp_system, u32 heapID );
static void NEWSDRAW_TopicWinDraw( NEWSDRAW_TOPICWIN* p_wk );
static void NEWSDRAW_TopicInit( NEWSDRAW_TOPIC* p_wk, NEWSDRAW_DRAWSYS* p_draw, u32 idx, u32 heapID );
static void NEWSDRAW_TopicExit( NEWSDRAW_TOPIC* p_wk );
static void NEWSDRAW_TopicStart( NEWSDRAW_TOPIC* p_wk, const STRBUF* cp_str, u32 speed, const NEWSDRAW_TOPIC_TRCOL* cp_trcol, const NNSG2dPaletteData* cp_pltt );
static void NEWSDRAW_TopicEnd( NEWSDRAW_TOPIC* p_wk );
static BOOL NEWSDRAW_TopicMain( NEWSDRAW_TOPIC* p_wk );
static void NEWSDRAW_TopicDraw( const NEWSDRAW_TOPIC* cp_wk, GF_BGL_BMPWIN* p_bmp );


// タイトルウィンドウ
static void NEWSDRAW_TitleWinInit( NEWSDRAW_TITLEWIN* p_wk, NEWSDRAW_DRAWSYS* p_draw, u32 heapID );
static void NEWSDRAW_TitleWinExit( NEWSDRAW_TITLEWIN* p_wk );
static void NEWSDRAW_TitleWinDraw( NEWSDRAW_TITLEWIN* p_wk );
static void NEWSDRAW_TitleWinEffectStart( NEWSDRAW_TITLEWIN* p_wk, u32 title_no );
static void NEWSDRAW_TitleEffInit( NEWSDRAW_TITLEEFF* p_wk, u16 offs, u32 snd );
static void NEWSDRAW_TitleEffExit( NEWSDRAW_TITLEEFF* p_wk );
static void NEWSDRAW_TitleEffStart( NEWSDRAW_TITLEEFF* p_wk );
static void NEWSDRAW_TitleEffMain( NEWSDRAW_TITLEEFF* p_wk );
static void NEWSDRAW_TitleEffDraw( NEWSDRAW_TITLEEFF* p_wk );



#ifdef WFLBY_DEBUG_LOBBY_NEWS_TOPIC_MAKE
static void NEWSDRAW_DEBUG_TopicMake( NEWSDRAW_WK* p_wk, WFLBY_SYSTEM* p_system, u32 heapID );
#endif



//----------------------------------------------------------------------------
/**
 *	@brief	ロビーニュース	初期化
 *
 *	@param	p_proc		プロックワーク
 *	@param	p_seq		シーケンス
 *
 *	@retval	PROC_RES_CONTINUE = 0,		///<動作継続中
 *	@retval	PROC_RES_FINISH,			///<動作終了
 */
//-----------------------------------------------------------------------------
PROC_RESULT NEWS_DRAW_Init(PROC* p_proc, int* p_seq)
{
	NEWSDRAW_WK* p_wk;
	NEWS_DRAW_DATA* p_param;
	
	p_param = PROC_GetParentWork( p_proc );

	//ヒープエリア作成
	sys_CreateHeap( HEAPID_BASE_APP, HEAPID_NEWSDRAW, 0x50000 );

	// ワーク作成
	p_wk = PROC_AllocWork( p_proc, sizeof(NEWSDRAW_WK), HEAPID_NEWSDRAW );
	memset( p_wk, 0, sizeof(NEWSDRAW_WK) );

	// データ格納
	p_wk->p_data		= p_param->p_data;
	p_wk->cp_nowtime	= p_param->cp_nowtime;

	// 表示システム初期化
	NEWSDRAW_DrawSysInit( &p_wk->draw, HEAPID_NEWSDRAW );

	// スクリーンセット初期化
	NEWSDRAW_ScrnSetInit( &p_wk->scrn, &p_wk->draw, HEAPID_NEWSDRAW );

	// 時間ウィンドウ初期化
	NEWSDRAW_TimeWinInit( &p_wk->time, &p_wk->draw, &p_wk->scrn, p_wk->p_data, p_wk->cp_nowtime, p_param->p_system, HEAPID_NEWSDRAW );

	// プレイヤーウィンドウ初期化
	NEWSDRAW_PlayerWinInit( &p_wk->player, &p_wk->draw, &p_wk->scrn, p_wk->p_data, p_param->p_system, HEAPID_NEWSDRAW );

	// トピックウィンドウ初期化
	NEWSDRAW_TopicWinInit( &p_wk->topic, &p_wk->draw, HEAPID_NEWSDRAW );

	// タイトル作成
	NEWSDRAW_TitleWinInit( &p_wk->title, &p_wk->draw, HEAPID_NEWSDRAW );

	// 割り込み設定
	sys_VBlankFuncChange( NEWSDRAW_WkVBlank, p_wk );
	sys_HBlankIntrStop();	//HBlank割り込み停止

	
	return PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ロビーニュース	メイン
 */
//-----------------------------------------------------------------------------
PROC_RESULT NEWS_DRAW_Main(PROC* p_proc, int* p_seq)
{
	NEWSDRAW_WK* p_wk;
	NEWS_DRAW_DATA* p_param;
	BOOL result;
	
	p_wk	= PROC_GetWork( p_proc );
	p_param = PROC_GetParentWork( p_proc );

#ifdef WFLBY_DEBUG_ROOM_WLDTIMER_AUTO
	WFLBY_DEBUG_ROOM_WFLBY_TIMER_AUTO = TRUE;
	sys.trg		|= PAD_BUTTON_B;
	sys.cont	|= PAD_BUTTON_B;
#endif

	switch( *p_seq ){
	case NEWSDRAW_SEQ_FADEIN:
		WIPE_SYS_Start(WIPE_PATTERN_WMS, 
				WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
				WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_NEWSDRAW );

		// BGMをフェードアウト
		WFLBY_SYSTEM_SetBGMVolumeDown( p_param->p_system, TRUE );

		// ロビーに入ってきた音を出す
//		Snd_SePlay( WFLBY_SND_NEWSIN );

		(*p_seq) ++;
		break;
		
	case NEWSDRAW_SEQ_FADEINWAIT:
		result = WIPE_SYS_EndCheck();
		if( result == TRUE ){
			(*p_seq) ++;
		}
		break;
		
	case NEWSDRAW_SEQ_MAIN:

		if( (sys.trg & PAD_BUTTON_B) || 
			(WFLBY_SYSTEM_Event_GetEndCM( p_param->p_system ) == TRUE) ||
			(WFLBY_ERR_CheckError() == TRUE) ){
			if( (WFLBY_SYSTEM_Event_GetEndCM( p_param->p_system ) == TRUE) ){
				WFLBY_SYSTEM_APLFLAG_SetForceEnd( p_param->p_system );
			}
			(*p_seq) = NEWSDRAW_SEQ_FADEOUT;
		}else{

#ifdef WFLBY_DEBUG_LOBBY_NEWS_TOPIC_MAKE
			if( sys.trg & PAD_BUTTON_R ){
				NEWSDRAW_DEBUG_TopicMake( p_wk, p_param->p_system, HEAPID_NEWSDRAW );
			}
#endif
			
			

			// 時間ウィンドウ処理
			result = NEWSDRAW_TimeWinMain( &p_wk->time, p_wk->p_data, p_wk->cp_nowtime );
			if( result == TRUE ){
				NEWSDRAW_TitleWinEffectStart( &p_wk->title, NEWSDRAW_TITLEWIN_TIME );
			}

			// プレイヤーウィンドウ処理
			result = NEWSDRAW_PlayerWinMain( &p_wk->player, p_wk->p_data );
			if( result == TRUE ){
				NEWSDRAW_TitleWinEffectStart( &p_wk->title, NEWSDRAW_TITLEWIN_PLAYER );
			}

			// トピックウィンドウ処理
			result = NEWSDRAW_TopicWinMain( &p_wk->topic, p_wk->p_data, p_param->p_system, HEAPID_NEWSDRAW );
			if( result == TRUE ){
				NEWSDRAW_TitleWinEffectStart( &p_wk->title, NEWSDRAW_TITLEWIN_TOPIC );
			}

		}
		break;
		
	case NEWSDRAW_SEQ_FADEOUT:
		WIPE_SYS_Start(WIPE_PATTERN_WMS, 
				WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
				WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_NEWSDRAW );
		(*p_seq) ++;
		break;
		
	case NEWSDRAW_SEQ_FADEOUTWAIT:
		result = WIPE_SYS_EndCheck();
		if( result == TRUE ){
			return PROC_RES_FINISH;
		}
		break;
	}


	// 描画
	NEWSDRAW_WkDraw( p_wk, p_param->p_system );

	return PROC_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ロビーニュース	破棄
 */
//-----------------------------------------------------------------------------
PROC_RESULT NEWS_DRAW_Exit(PROC* p_proc, int* p_seq)
{
	NEWSDRAW_WK* p_wk;
	NEWS_DRAW_DATA* p_param;
	
	p_wk	= PROC_GetWork( p_proc );
	p_param = PROC_GetParentWork( p_proc );

	// タイトル破棄
	NEWSDRAW_TitleWinExit( &p_wk->title );

	// 時間ウィンドウ破棄
	NEWSDRAW_TimeWinExit( &p_wk->time );

	// プレイヤーウィンドウ破棄
	NEWSDRAW_PlayerWinExit( &p_wk->player );

	// トピックウィンドウ破棄
	NEWSDRAW_TopicWinExit( &p_wk->topic );

	// スクリーンセット破棄
	NEWSDRAW_ScrnSetExit( &p_wk->scrn );

	// 割り込み設定
	sys_VBlankFuncChange( NULL, NULL );
	sys_HBlankIntrStop();	//HBlank割り込み停止


	// 描画システム破棄
	NEWSDRAW_DrawSysExit( &p_wk->draw );

	//ワーク破棄
	PROC_FreeWork( p_proc );
	
	//ヒープ破棄
	sys_DeleteHeap( HEAPID_NEWSDRAW );

	return PROC_RES_FINISH;
}





//-----------------------------------------------------------------------------
/**
 *			プライベート関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	VBLANK処理
 *
 *	@param	p_work 
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_WkVBlank( void* p_work )
{
	NEWSDRAW_WK* p_wk = p_work;

	NEWSDRAW_DrawSysVBlank( &p_wk->draw );
}

//----------------------------------------------------------------------------
/**
 *	@brief	表示処理
 *
 *	@param	p_wk		ワーク
 *	@param	cp_system	システムワーク
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_WkDraw( NEWSDRAW_WK* p_wk, const WFLBY_SYSTEM* cp_system )
{
	// 時間ウィンドウ表示処理
	NEWSDRAW_TimeWinDraw( &p_wk->time, &p_wk->scrn, &p_wk->draw );

	// プレイヤーウィンドウ表示処理
	NEWSDRAW_PlayerWinDraw( &p_wk->player, &p_wk->scrn, &p_wk->draw, p_wk->p_data, cp_system );

	// トピックの描画
	NEWSDRAW_TopicWinDraw( &p_wk->topic );

	// タイトルウィンドウ処理
	NEWSDRAW_TitleWinDraw( &p_wk->title );
	
	// 描画システム描画
	NEWSDRAW_DrawSysDraw( &p_wk->draw );
}


//----------------------------------------------------------------------------
/**
 *	@brief	表示物の初期化
 *
 *	@param	p_wk		表示システムワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_DrawSysInit( NEWSDRAW_DRAWSYS* p_wk, u32 heapID )
{
	// アーカイブハンドル
	p_wk->p_handle = ArchiveDataHandleOpen( ARC_LOBBY_NEWS, heapID );

	// Vram転送マネージャ作成
	initVramTransferManagerHeap( NEWSDRAW_VRAMTRANS_TASKNUM, heapID );
	
	// バンク設定
	GF_Disp_SetBank( &sc_NEWSDRAW_BANK );

	// BG設定
	NEWSDRAW_DrawSysBgInit( p_wk, heapID );

	// OAM設定
	NEWSDRAW_DrawSysOamInit( p_wk, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	表示物の破棄
 *
 *	@param	p_wk		表示システムワーク
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_DrawSysExit( NEWSDRAW_DRAWSYS* p_wk )
{
	// アーカイブハンドル
	ArchiveDataHandleClose( p_wk->p_handle );

	// Vram転送マネージャ破棄
	DellVramTransferManager();
	
	// BG設定
	NEWSDRAW_DrawSysBgExit( p_wk );

	// OAM設定
	NEWSDRAW_DrawSysOamExit( p_wk );

}

//----------------------------------------------------------------------------
/**
 *	@brief	描画処理
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_DrawSysDraw( NEWSDRAW_DRAWSYS* p_wk )
{
	CLACT_Draw( p_wk->p_clactset );
}

//----------------------------------------------------------------------------
/**
 *	@brief	VBlank処理
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_DrawSysVBlank( NEWSDRAW_DRAWSYS* p_wk )
{
    // BG書き換え
    GF_BGL_VBlankFunc( p_wk->p_bgl );

    // レンダラ共有OAMマネージャVram転送
    REND_OAMTrans();

	// Vram転送
	DoVramTransferManager();
}

// BG
static void NEWSDRAW_DrawSysBgInit( NEWSDRAW_DRAWSYS* p_wk, u32 heapID )
{
	// BG設定
	GF_BGL_InitBG(&sc_BGINIT);
	
	// BGL作成
	p_wk->p_bgl = GF_BGL_BglIniAlloc( heapID );

	// メインとサブを切り替える
	sys.disp3DSW = DISP_3D_TO_MAIN;
	GF_Disp_DispSelect();


	// BGコントロール設定
	{
		int i;

		for( i=0; i<NEWSDRAW_BGCNT_NUM; i++ ){
			GF_BGL_BGControlSet( p_wk->p_bgl, 
					sc_NEWSDRAW_BGCNT_FRM[i], &sc_NEWSDRAW_BGCNT_DATA[i],
					GF_BGL_MODE_TEXT );
			GF_BGL_ClearCharSet( sc_NEWSDRAW_BGCNT_FRM[i], 32, 0, heapID);
			GF_BGL_ScrClear( p_wk->p_bgl, sc_NEWSDRAW_BGCNT_FRM[i] );
		}
	}

	// BG設定
	{
		// パレット
		ArcUtil_HDL_PalSet( p_wk->p_handle, NARC_lobby_news_lobby_news_bg_NCLR, 
				PALTYPE_MAIN_BG, 0, 0, heapID );

		//  フォントカラー設定
		TalkFontPaletteLoad( PALTYPE_MAIN_BG, NEWS_PLTT_TALKFONT*32, heapID );

		// キャラクタ
		ArcUtil_HDL_BgCharSet( p_wk->p_handle,
				NARC_lobby_news_lobby_news_bg_NCGR, 
				p_wk->p_bgl, GF_BGL_FRAME1_M, 0, 0, FALSE, heapID );

		// スクリーン
		ArcUtil_HDL_ScrnSet(p_wk->p_handle, 
				NARC_lobby_news_lobby_news_bg1_NSCR, 
				p_wk->p_bgl, GF_BGL_FRAME1_M, 0, 0, FALSE, heapID);
		ArcUtil_HDL_ScrnSet(p_wk->p_handle, 
				NARC_lobby_news_lobby_news_bg2_NSCR, 
				p_wk->p_bgl, GF_BGL_FRAME2_M, 0, 0, FALSE, heapID);

	}

	// サブ画面設定
	{
		// パレット
		ArcUtil_HDL_PalSet( p_wk->p_handle, NARC_lobby_news_lobby_news_bg_NCLR, 
				PALTYPE_SUB_BG, 0, 0, heapID );

		// キャラクタ
		ArcUtil_HDL_BgCharSet( p_wk->p_handle,
				NARC_lobby_news_wifi_mark_bg_NCGR, 
				p_wk->p_bgl, GF_BGL_FRAME0_S, 0, 0, FALSE, heapID );

		// スクリーン
		ArcUtil_HDL_ScrnSet(p_wk->p_handle, 
				NARC_lobby_news_wifi_mark_bg_NSCR, 
				p_wk->p_bgl, GF_BGL_FRAME0_S, 0, 0, FALSE, heapID);
	}

	// 基本キャラクタパレットフレーム
	// バックグラウンドカラー設定
//	GF_BGL_BackGroundColorSet( GF_BGL_FRAME0_M, GX_RGB( 0,0,0 ) );
}
static void NEWSDRAW_DrawSysBgExit( NEWSDRAW_DRAWSYS* p_wk )
{
	// ＢＧコントロール破棄
	{
		int i;

		for( i=0; i<NEWSDRAW_BGCNT_NUM; i++ ){
			GF_BGL_BGControlExit( p_wk->p_bgl, sc_NEWSDRAW_BGCNT_FRM[i] );
		}
	}
	
	// BGL破棄
	sys_FreeMemoryEz( p_wk->p_bgl );

	// メインとサブを元に戻す
	sys.disp3DSW = DISP_3D_TO_MAIN;
	GF_Disp_DispSelect();
}

// OAM
static void NEWSDRAW_DrawSysOamInit( NEWSDRAW_DRAWSYS* p_wk, u32 heapID )
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
    InitCharManagerReg(&sc_NEWSDRAW_CHARMAN_INIT, GX_OBJVRAMMODE_CHAR_1D_128K, GX_OBJVRAMMODE_CHAR_1D_32K );
    // パレットマネージャー初期化
    InitPlttManager(NEWSDRAW_OAM_CONTNUM, heapID);

    // 読み込み開始位置を初期化
    CharLoadStartAll();
    PlttLoadStartAll();

    //通信アイコン用にキャラ＆パレット制限
    CLACT_U_WmIcon_SetReserveAreaCharManager(NNS_G2D_VRAM_TYPE_2DMAIN, GX_OBJVRAMMODE_CHAR_1D_128K);
    CLACT_U_WmIcon_SetReserveAreaPlttManager(NNS_G2D_VRAM_TYPE_2DMAIN);
    

    // セルアクターセット作成
    p_wk->p_clactset = CLACT_U_SetEasyInit( NEWSDRAW_OAM_CONTNUM, &p_wk->renddata, heapID );

    // サーフェース位置を移動させる
    CLACT_U_SetSubSurfaceMatrix( &p_wk->renddata, 0, NEWSDRAW_SF_MAT_Y );
    
    // キャラとパレットのリソースマネージャ作成
    for( i=0; i<NEWSDRAW_RESMAN_NUM; i++ ){
        p_wk->p_resman[i] = CLACT_U_ResManagerInit(NEWSDRAW_OAM_CONTNUM, i, heapID);
    }

	// 下画面に通信アイコンを出す
	WirelessIconEasy();  // 接続中なのでアイコン表示

	// 表示開始
    GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
    GF_Disp_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
}
static void NEWSDRAW_DrawSysOamExit( NEWSDRAW_DRAWSYS* p_wk )
{
    int i;

    // アクターの破棄
    CLACT_DestSet( p_wk->p_clactset );

    for( i=0; i<NEWSDRAW_RESMAN_NUM; i++ ){
        CLACT_U_ResManagerDelete( p_wk->p_resman[i] );
    }

    // リソース解放
    DeleteCharManager();
    DeletePlttManager();

    //OAMレンダラー破棄
    REND_OAM_Delete();
}




//----------------------------------------------------------------------------
/**
 *	@brief	スクリーンセット初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_draw		描画ワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_ScrnSetInit( NEWSDRAW_SCRNSET* p_wk, NEWSDRAW_DRAWSYS* p_draw, u32 heapID )
{
	p_wk->p_buff = ArcUtil_HDL_ScrnDataGet( p_draw->p_handle, 
			NARC_lobby_news_lobby_news_chara_NSCR,
			FALSE, &p_wk->p_scrn, heapID);
}

//----------------------------------------------------------------------------
/**
 *	@brief	スクリーンセット破棄
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_ScrnSetExit( NEWSDRAW_SCRNSET* p_wk )
{
	sys_FreeMemoryEz( p_wk->p_buff );
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイムブロックを書き込む
 *
 *	@param	p_wk		ワーク
 *	@param	p_draw		描画システム
 *	@param	block_type	ブロックタイプ
 *	@param	x			Xブロック位置{0～NEWSDRAW_TIME_X_SIZ-1}
 *	@param	y 			Yブロック位置{0～NEWSDRAW_TIME_Y_SIZ-1}
 *
 *	block_type
 *		NEWSDRAW_TIME_BLOCKTYPE_NORMAL,	// 通常
 *		NEWSDRAW_TIME_BLOCKTYPE_ICON,	// アイコン
 *		NEWSDRAW_TIME_BLOCKTYPE_RED,	// レッド
 *		NEWSDRAW_TIME_BLOCKTYPE_NONE,	// 何もなし
 *		NEWSDRAW_TIME_BLOCKTYPE_ICON1,	// アイコン　アニメ１
 *		NEWSDRAW_TIME_BLOCKTYPE_ICON2,	// アイコン　アニメ２
 *		NEWSDRAW_TIME_BLOCKTYPE_ICON3,	// アイコン　アニメ３
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_ScrnWriteTimeBlock( NEWSDRAW_SCRNSET* p_wk, NEWSDRAW_DRAWSYS* p_draw, u32 block_type, u8 x, u8 y )
{
	GF_BGL_ScrWriteExpand( p_draw->p_bgl, GF_BGL_FRAME3_M, 
			NEWSDRAW_TIME_SCRN_DRAWX+(x*NEWSDRAW_TIME_BLOCK_SIZX), 
			NEWSDRAW_TIME_SCRN_DRAWY+(y*NEWSDRAW_TIME_BLOCK_SIZY),
			NEWSDRAW_TIME_BLOCK_SIZX, NEWSDRAW_TIME_BLOCK_SIZY,
			p_wk->p_scrn->rawData,
			NEWSDRAW_SCRN_TIMEBLOCK_X + block_type, 
			NEWSDRAW_SCRN_TIMEBLOCK_Y,
			p_wk->p_scrn->screenWidth/8,
			p_wk->p_scrn->screenHeight/8 );

	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, GF_BGL_FRAME3_M );
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーデータ表示
 *
 *	@param	p_wk			ワーク
 *	@param	p_draw			描画システム
 *	@param	x				ｘ描画位置
 *	@param	y				ｙ描画位置
 *	@param	player_type		プレイヤータイプ
 *	@param	sex				性別
 *	@param	mydata			自分のデータか
 *	@param	olddata			昔のデータか
 *	@param	vip				VIPかどうか
 *
 *	player_type
			NEWSDRAW_PLAYERTYPE_NML,
			NEWSDRAW_PLAYERTYPE_OUT,
			NEWSDRAW_PLAYERTYPE_IN,
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_ScrnWritePlayer( NEWSDRAW_SCRNSET* p_wk, NEWSDRAW_DRAWSYS* p_draw, u8 x, u8 y, u32 player_type, u32 sex, BOOL mydata, BOOL olddata, BOOL vip )
{
	u32 r_x, r_y;

	r_x = player_type % NEWSDRAW_PLAYER_SCRN_PAT_X;
	r_y = player_type / NEWSDRAW_PLAYER_SCRN_PAT_X;

	// 古い人の書いてあるスクリーン位置に移動
	if( olddata ){
		r_y += NEWSDRAW_PLAYER_SCRN_OLDPAT_Y;
	}

	// 性別分動かす
	if( sex == PM_FEMALE ){
		r_y ++;
	}
	
	GF_BGL_ScrWriteExpand( p_draw->p_bgl, GF_BGL_FRAME3_M, 
			NEWSDRAW_PLAYER_SCRN_DRAWX+(x*NEWSDRAW_PLAYER_BLOCK_SIZX), 
			NEWSDRAW_PLAYER_SCRN_DRAWY+(y*NEWSDRAW_PLAYER_BLOCK_SIZY),
			NEWSDRAW_PLAYER_BLOCK_SIZX, NEWSDRAW_PLAYER_BLOCK_SIZY,
			p_wk->p_scrn->rawData,
			NEWSDRAW_SCRN_PLAYERBLOCK_X + (r_x*NEWSDRAW_PLAYER_BLOCK_SIZX), 
			NEWSDRAW_SCRN_PLAYERBLOCK_Y + (r_y*NEWSDRAW_PLAYER_BLOCK_SIZY),
			p_wk->p_scrn->screenWidth/8,
			p_wk->p_scrn->screenHeight/8 );

	// vipは青くする
	if( vip ){
		GF_BGL_ScrPalChange( p_draw->p_bgl, GF_BGL_FRAME3_M,
				NEWSDRAW_PLAYER_SCRN_DRAWX+(x*NEWSDRAW_PLAYER_BLOCK_SIZX), 
				NEWSDRAW_PLAYER_SCRN_DRAWY+(y*NEWSDRAW_PLAYER_BLOCK_SIZY),
				NEWSDRAW_PLAYER_BLOCK_SIZX, NEWSDRAW_PLAYER_BLOCK_SIZY,
				NEWS_PLTT_VIP );
	}else{

		// 自分のデータはオレンジくする
		if( mydata ){
			GF_BGL_ScrPalChange( p_draw->p_bgl, GF_BGL_FRAME3_M,
					NEWSDRAW_PLAYER_SCRN_DRAWX+(x*NEWSDRAW_PLAYER_BLOCK_SIZX), 
					NEWSDRAW_PLAYER_SCRN_DRAWY+(y*NEWSDRAW_PLAYER_BLOCK_SIZY),
					NEWSDRAW_PLAYER_BLOCK_SIZX, NEWSDRAW_PLAYER_BLOCK_SIZY,
					NEWS_PLTT_YOU );
		}
	}

	GF_BGL_LoadScreenV_Req( p_draw->p_bgl, GF_BGL_FRAME3_M );
}





//----------------------------------------------------------------------------
/**
 *	@brief	時間ウィンドウ	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_draw		描画システム
 *	@param	p_scrn		スクリーンセット
 *	@pamra	cp_data		データ
 *	@param	cp_nowtime	今の時間
 *	@param	cp_system	システムワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_TimeWinInit( NEWSDRAW_TIMEWIN* p_wk, NEWSDRAW_DRAWSYS* p_draw, NEWSDRAW_SCRNSET* p_scrn, const NEWS_DATA* cp_data, const WFLBY_TIME* cp_nowtime, const WFLBY_SYSTEM* cp_system, u32 heapID )
{
	u32 closetime;
	u32 closetime_minute;
	
	// 残り時間初期値
	// ロック後の時間＋１分
	closetime = WFLBY_SYSTEM_GetCloseTime( cp_system );	
	closetime_minute = closetime / 60;	// 秒単位なので分単位に
	p_wk->last_rest_time.hour	= 0;
	p_wk->last_rest_time.minute = closetime_minute + 1;
	p_wk->last_rest_time.second	= 0;

	// ブロックの描画を書き換える間隔
	p_wk->change_second = closetime / NEWSDRAW_TIME_BLOCKNUM;	

//	OS_TPrintf( "closetime %d minute %d change %d\n", closetime, closetime_minute, p_wk->change_second );

	// モザイクアニメ設定ON
	p_wk->effect_flag	= TRUE;
	p_wk->count			= 0;

	// 描画更新
	p_wk->updata		= FALSE;

	// 1度実行
	NEWSDRAW_TimeWinMain( p_wk, cp_data, cp_nowtime );
	
	// 描画データ初期化
	NEWSDRAW_TimeWinDraw( p_wk, p_scrn, p_draw );
}

//----------------------------------------------------------------------------
/**
 *	@brief	時間ウィンドウ	破棄
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_TimeWinExit( NEWSDRAW_TIMEWIN* p_wk )
{
	// 描画データ破棄
}

//----------------------------------------------------------------------------
/**
 *	@brief	時間ウィンドウ	メイン
 *
 *	@param	p_wk		ワーク
 *	@param	cp_data		データ
 *	@param	cp_nowtime	現在時間
 *
 *	@retval	TRUE	表示更新した
 *	@retval	FALSE	表示更新してない
 */
//-----------------------------------------------------------------------------
static BOOL NEWSDRAW_TimeWinMain( NEWSDRAW_TIMEWIN* p_wk, const NEWS_DATA* cp_data, const WFLBY_TIME* cp_nowtime )
{
	BOOL result;
	WFLBY_TIME time;
	WFLBY_TIME rest_time;
	WFLBY_TIME rest_sum;
	BOOL ret = FALSE;
	
	// LOCKしているかチェック
	result = NEWS_DSET_GetLockTime( cp_data, &time );	
	if( result == FALSE ){
		// モザイクアニメ
		return ret;
	}

	// モザイクアニメ中なら停止する
	if( p_wk->effect_flag == TRUE ){
		p_wk->effect_flag = FALSE;
		
		// 書き換える
		p_wk->updata	= TRUE;
		ret				= TRUE;
	}

	// 今のLOCKからの経過でスクリーンに書き込む
	WFLBY_TIME_Sub( cp_nowtime, &time, &rest_time );

	// 残り時間と１つ前の残り時間にp_wk->change_second秒
	// 以上の幅があれば、書き直す
	WFLBY_TIME_Sub( &rest_time, &p_wk->last_rest_time, &rest_sum );
	if( (rest_sum.second >= p_wk->change_second) ||
		(rest_sum.minute > 0) ||
		(rest_sum.hour	 > 0) ){

		// 書き換える
		p_wk->updata = TRUE;

		// 書き換えた時間を保存
		p_wk->last_rest_time = rest_time;

		// 書き換えた
		ret = TRUE;
	}

	return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	時間ウィンドウ描画処理
 *
 *	@param	p_wk	ワーク
 *	@param	p_scrn	スクリーンセット
 *	@param	p_draw	描画システム
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_TimeWinDraw( NEWSDRAW_TIMEWIN* p_wk, NEWSDRAW_SCRNSET* p_scrn, NEWSDRAW_DRAWSYS* p_draw )
{
	
	if( p_wk->effect_flag == TRUE ){
		NEWSDRAW_TimeWinDrawMosaic( p_wk, p_scrn, p_draw );
	}else{
		NEWSDRAW_TimeWinDrawUpdate( p_wk, p_scrn, p_draw );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	時間データ描画アップデート	エフェクト時
 *
 *	@param	p_wk		ワーク
 *	@param	p_scrn		スクリーン
 *	@param	p_draw		描画システム
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_TimeWinDrawMosaic( NEWSDRAW_TIMEWIN* p_wk, NEWSDRAW_SCRNSET* p_scrn, NEWSDRAW_DRAWSYS* p_draw )
{
	p_wk->count ++;
	if( NEWSDRAW_TIME_EFFECT_COUNT < p_wk->count ){
		p_wk->count = 0;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	時間データ描画アップデート
 *
 *	@param	p_wk		ワーク
 *	@param	p_scrn		スクリーン
 *	@param	p_draw		描画システム
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_TimeWinDrawUpdate( NEWSDRAW_TIMEWIN* p_wk, NEWSDRAW_SCRNSET* p_scrn, NEWSDRAW_DRAWSYS* p_draw )
{
	int i, j;
	u32 block;
	u16 block_type;
	s32 x, y;
	s32 block_num;

	block = p_wk->last_rest_time.second;
	block += p_wk->last_rest_time.minute * 60;
	block += p_wk->last_rest_time.hour * 3600;

	// 表示ブロック数
	block /= p_wk->change_second;

	// アップデートフラグで書き換える
	if( p_wk->updata ){

		// カウンタ初期化
		p_wk->count = 0;

		p_wk->updata = FALSE;

		// スクリーン領域をClean
		GF_BGL_ScrFill( p_draw->p_bgl, GF_BGL_FRAME3_M, 0,
				NEWSDRAW_TIME_SCRN_DRAWX, NEWSDRAW_TIME_SCRN_DRAWY,
				NEWSDRAW_TIME_X_SIZ*NEWSDRAW_TIME_BLOCK_SIZX,
				NEWSDRAW_TIME_Y_SIZ*NEWSDRAW_TIME_BLOCK_SIZY,
				0 );
		GF_BGL_LoadScreenV_Req( p_draw->p_bgl, GF_BGL_FRAME3_M );
		
		
		// 残り時間でスクリーンを表示する
		for( i=0; i<NEWSDRAW_TIME_Y_SIZ; i++ ){
			
			y = (NEWSDRAW_TIME_Y_SIZ-1)-i;
			
			for( j=0; j<NEWSDRAW_TIME_X_SIZ; j++ ){
			
				x = (NEWSDRAW_TIME_X_SIZ-1)-j;	

				block_num = ((y*NEWSDRAW_TIME_X_SIZ)+x);
				
				if( block_num >= block ){
					if( block_num == block ){
						block_type = NEWSDRAW_TIME_BLOCKTYPE_ICON;
//						OS_Printf( "icon first x[%d] y[%d] block[%d]\n", x, y, block );
					}else if( y==(NEWSDRAW_TIME_Y_SIZ-1) ){
						block_type = NEWSDRAW_TIME_BLOCKTYPE_RED;
					}else{
						block_type = NEWSDRAW_TIME_BLOCKTYPE_NORMAL;
					}
					// 書き込み
					NEWSDRAW_ScrnWriteTimeBlock( p_scrn, p_draw, block_type, 
							x, y );
				}
			}
		}
		
	}else{

		// BLOCK値のオーバーチェック
		if( block < NEWSDRAW_TIME_BLOCKNUM ){
			u32 eff_count;
			u32 icon_anm;
			u32 icon_one_time;
			
			// エフェクトカウント処理
			p_wk->count ++;

			// エフェクトカウント値計算
			eff_count = p_wk->count % NEWSDRAW_TIME_TIME_EFFECT_COUNT;

			// 座標
			x = block % NEWSDRAW_TIME_X_SIZ;
			y = block / NEWSDRAW_TIME_X_SIZ;

			// カウント
			if( eff_count == 0 ){
//				OS_Printf( "icon eff x[%d] y[%d] block[%d]\n", x, y, block );

				// iconを１つ減らすタイミング	change_secondは秒単位なので描画フレーム単位にする
				icon_one_time = (p_wk->change_second * 30) / NEWSDRAW_TIME_EFF_DATA_NUM;

				// 書き込むアイコンのタイプを取得
				icon_anm = NEWSDRAW_TIME_BLOCKTYPE_ICON3;
				for( i=0; i<NEWSDRAW_TIME_EFF_DATA_NUM; i++ ){
					if( icon_one_time*(i+1) > p_wk->count ){
						icon_anm = NEWSDRAW_TIME_EFF_ANM_DATA[i].icon;
						break;
					}
				}
				// 書き込み
				NEWSDRAW_ScrnWriteTimeBlock( p_scrn, p_draw, 
						icon_anm, x, y );
			}else if( eff_count == (NEWSDRAW_TIME_TIME_EFFECT_COUNT/2) ){
				// 書き込み
				NEWSDRAW_ScrnWriteTimeBlock( p_scrn, p_draw, 
						NEWSDRAW_TIME_BLOCKTYPE_NONE, x, y );
			}
		}
	}

}





//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーウィンドウ	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_draw		描画システム
 *	@param	p_scrn		スクリーンデータ
 *	@param	cp_data		描画データ
 *	@param	heapID		ヒープ
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_PlayerWinInit( NEWSDRAW_PLAYERWIN* p_wk, NEWSDRAW_DRAWSYS* p_draw, NEWSDRAW_SCRNSET* p_scrn, const NEWS_DATA* cp_data, const WFLBY_SYSTEM* cp_system, u32 heapID )
{
	// 描画初期化
	{
		// 初期データの描画
		NEWSDRAW_PlayerWinWrite( p_scrn, p_draw, cp_data, cp_system, NEWSDRAW_PLAYERTYPE_NML );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーウィンドウ	破棄
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_PlayerWinExit( NEWSDRAW_PLAYERWIN* p_wk )
{
	// 描画破棄
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーウィンドウ	メイン
 *	
 *	@param	p_wk		ワーク
 *	@param	cp_data		データ
 *
 *	@retval	TRUE	更新した
 *	@retval	FALSE	更新しなかった
 */
//-----------------------------------------------------------------------------
static BOOL NEWSDRAW_PlayerWinMain( NEWSDRAW_PLAYERWIN* p_wk, const NEWS_DATA* cp_data )
{
	u32 event;
	BOOL ret = FALSE;
	
	// イベントが起こったら再描画する
	event = NEWS_DSET_GetPlayerEvent( cp_data );
	switch( event ){
	case NEWS_ROOMEV_NONE:
		break;
		
	case NEWS_ROOMEV_IN:		// 誰か入ってきた
		// 人が入ってきたシーケンスへ
		p_wk->seq = NEWSDRAW_PLAYERWIN_SEQ_INSIDE;
		ret = TRUE;
		break;
		
	case NEWS_ROOMEV_OUT:	// 誰か出て行った
		// 人が出てったシーケンスへ
		p_wk->seq = NEWSDRAW_PLAYERWIN_SEQ_OUTSIDE;
		ret = TRUE;
		break;
	}

	return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーウィンドウ　アニメ
 *
 *	@param	p_wk		ワーク
 *	@param	p_scrn		スクリーンセット
 *	@param	p_draw		描画システム
 *	@param	cp_data		データ
 *	@param	cp_system	システムデータ
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_PlayerWinDraw( NEWSDRAW_PLAYERWIN* p_wk, NEWSDRAW_SCRNSET* p_scrn, NEWSDRAW_DRAWSYS* p_draw, const NEWS_DATA* cp_data, const WFLBY_SYSTEM* cp_system )
{
	switch( p_wk->seq ){
	case NEWSDRAW_PLAYERWIN_SEQ_WAIT:		// 待機状態
		break;
		
	case NEWSDRAW_PLAYERWIN_SEQ_INSIDE:		// 人が入ってきた
		p_wk->seq = NEWSDRAW_PLAYERWIN_SEQ_INSIDEMAIN;
		p_wk->count = 0;
		break;
		
	case NEWSDRAW_PLAYERWIN_SEQ_INSIDEMAIN:	// 人が入ってきた
		NEWSDRAW_PlayerWinInsideAnim( p_scrn, p_draw, cp_data, cp_system, p_wk->count );
		p_wk->count ++;
		if( p_wk->count >= NEWSDRAW_PLAYERWIN_COUNT_INSIDE ){
			p_wk->seq = NEWSDRAW_PLAYERWIN_SEQ_WAIT;
		}
		break;
		
	case NEWSDRAW_PLAYERWIN_SEQ_OUTSIDE:		// 人が入ってきた
		p_wk->seq = NEWSDRAW_PLAYERWIN_SEQ_OUTSIDEMAIN;
		p_wk->count = 0;
		break;

	case NEWSDRAW_PLAYERWIN_SEQ_OUTSIDEMAIN:	// 人が入ってきた
		NEWSDRAW_PlayerWinOutsideAnim( p_scrn, p_draw, cp_data, cp_system, p_wk->count );
		p_wk->count ++;
		if( p_wk->count >= NEWSDRAW_PLAYERWIN_COUNT_OUTSIDE ){
			p_wk->seq = NEWSDRAW_PLAYERWIN_SEQ_WAIT;
		}
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	人の絵を表示する
 *
 *	@param	p_scrn		スクリーンワーク
 *	@param	p_draw		描画システム
 *	@param	cp_data		描画データ
 *	@param	player_type	表示プレイヤータイプ
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_PlayerWinWrite( NEWSDRAW_SCRNSET* p_scrn, NEWSDRAW_DRAWSYS* p_draw, const NEWS_DATA* cp_data, const WFLBY_SYSTEM* cp_system, u32 player_type )
{
	int i, j;
	u32 sex;
	BOOL mydata;
	BOOL olddata;
	BOOL newdata;
	BOOL inside;
	BOOL vip;
	u32 player;
	u32 draw_player_type;

	// スクリーン領域をClean
	GF_BGL_ScrFill( p_draw->p_bgl, GF_BGL_FRAME3_M, 0,
			NEWSDRAW_PLAYER_SCRN_DRAWX, NEWSDRAW_PLAYER_SCRN_DRAWY,
			NEWSDRAW_PLAYERWIN_DRAW_X*NEWSDRAW_PLAYER_BLOCK_SIZX,
			NEWSDRAW_PLAYERWIN_DRAW_Y*NEWSDRAW_PLAYER_BLOCK_SIZY,
			0 );

	// 全プレイヤーを表示する
	for( i=0; i<NEWSDRAW_PLAYERWIN_DRAW_Y; i++ ){
		for( j=0; j<NEWSDRAW_PLAYERWIN_DRAW_X; j++ ){
			
			player	= (i*NEWSDRAW_PLAYERWIN_DRAW_X)+j;

			inside	= NEWS_DSET_CheckPlayerInside( cp_data, player );
			if( inside ){
				
				// 表示パラメータの取得
				sex		= NEWS_DSET_GetPlayerSex( cp_data, player );
				mydata	= NEWS_DSET_GetPlayerSpecialMy( cp_data, player );
				olddata	= NEWS_DSET_GetPlayerSpecialOld( cp_data, player );
				newdata	= NEWS_DSET_GetPlayerNewPlayer( cp_data, player );
				vip		= WFLBY_SYSTEM_GetUserVipFlag( cp_system, player );

				// 新しく入ってきた人の表示の場合は常に通常フレームを表示する
				if( (player_type == NEWSDRAW_PLAYERTYPE_IN) && (newdata == TRUE) ){
					draw_player_type = NEWSDRAW_PLAYERTYPE_NML;
				}else{
					draw_player_type = player_type;
				}

				// 表示
				NEWSDRAW_ScrnWritePlayer( p_scrn, p_draw,
						j, i, draw_player_type, sex, mydata, olddata, vip );
			}
		}
	}

}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーインサイドアニメ描画
 *
 *	@param	p_scrn		スクリーンワーク
 *	@param	p_draw		描画システム
 *	@param	cp_data		データ
 *	@param	cp_system	システムワーク
 *	@param	count		カウンタ
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_PlayerWinInsideAnim( NEWSDRAW_SCRNSET* p_scrn, NEWSDRAW_DRAWSYS* p_draw, const NEWS_DATA* cp_data, const WFLBY_SYSTEM* cp_system, u32 count )
{	
	u32 player_type;
	
	// 絵変更タイミングチェック
	if( (count % NEWSDRAW_PLAYERWIN_INSIDE_ONEFRAME) == 0 ){

		player_type = count / NEWSDRAW_PLAYERWIN_INSIDE_ONEFRAME;
		if( (player_type % 2) == 0 ){
			player_type = NEWSDRAW_PLAYERTYPE_IN;
		}else{
			player_type = NEWSDRAW_PLAYERTYPE_NML;
		}
		NEWSDRAW_PlayerWinWrite( p_scrn, p_draw, cp_data, cp_system, player_type );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーアウトサイドアニメ描画
 *
 *	@param	p_scrn		スクリーンワーク
 *	@param	p_draw		描画システム
 *	@param	cp_data		データ
 *	@param	cp_system	システムワーク
 *	@param	count		カウンタ
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_PlayerWinOutsideAnim( NEWSDRAW_SCRNSET* p_scrn, NEWSDRAW_DRAWSYS* p_draw, const NEWS_DATA* cp_data, const WFLBY_SYSTEM* cp_system, u32 count )
{
	u32 player_type;
	
	// 絵変更タイミングチェック
	if( (count % NEWSDRAW_PLAYERWIN_INSIDE_ONEFRAME) == 0 ){

		player_type = count / NEWSDRAW_PLAYERWIN_OUTSIDE_ONEFRAME;
		if( (player_type % 2) == 0 ){
			player_type = NEWSDRAW_PLAYERTYPE_OUT;
		}else{
			player_type = NEWSDRAW_PLAYERTYPE_NML;
		}
		NEWSDRAW_PlayerWinWrite( p_scrn, p_draw, cp_data, cp_system, player_type );
	}
}





//----------------------------------------------------------------------------
/**
 *	@brief	トレーナーカラーデータ取得
 *
 *	@param	p_wk		ワーク
 *	@param	cp_data		データ
 *	@param	type		タイプ
 *	@param	cp_system	システムデータ
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_TopicTrColDataGet( NEWSDRAW_TOPIC_TRCOL* p_wk, const NEWS_DATA* cp_data, NEWS_TOPICTYPE type, const WFLBY_SYSTEM* cp_system )
{
	int i;
	u32 pl_idx;
	BOOL vip;

	for( i=0; i<WFLBY_MINIGAME_MAX; i++ ){
		pl_idx = NEWS_DSET_GetTopicUserIdx( cp_data, type, i );
		if( pl_idx != WFLBY_PLAYER_MAX ){

			vip = WFLBY_SYSTEM_GetUserVipFlag( cp_system, pl_idx );
			if( vip == TRUE ){
				p_wk->trcol[i] = NEWS_PLTT_FONT_TRVIP;	// そいつはVIP！
			}else{
				p_wk->trcol[i] = NEWS_PLTT_FONT_TRN;	// こいつは普通
			}
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	パレット転送処理
 *
 *	@param	cp_wk		ワーク
 *	@param	cp_pltt		パレットデータ
 *	@param	plttidx		転送先パレットインデックス
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_TopicTrColDataTrans( const NEWSDRAW_TOPIC_TRCOL* cp_wk, const NNSG2dPaletteData* cp_pltt, u32 plttidx )
{
	BOOL result;
	int i;
	const u8* cp_plttdata;

	cp_plttdata = cp_pltt->pRawData;
	
	for( i=0; i<WFLBY_MINIGAME_MAX; i++ ){
		// トレーナーカラーに合った色を転送するタスクを作成
		result = AddVramTransferManager( 
				NNS_GFD_DST_2D_BG_PLTT_MAIN,
				(plttidx*0x20)+(NEWS_PLTT_FONTTR_TR_00*2)+(i*0x4),	// 転送先
				(void*)(&cp_plttdata[ (NEWS_PLTT_FONT*0x20)+(cp_wk->trcol[i]*2) ]), 
				0x4 );
		GF_ASSERT( result );
	}
}



//----------------------------------------------------------------------------
/**
 *	@brief	トピックウィンドウ	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_draw		描画システム
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_TopicWinInit( NEWSDRAW_TOPICWIN* p_wk, NEWSDRAW_DRAWSYS* p_draw, u32 heapID )
{
	int i;

	// トピックワーク初期化
	for( i=0; i<NEWSDRAW_TOPIC_NUM; i++ ){
		NEWSDRAW_TopicInit( &p_wk->topic[i], p_draw, i, heapID );

		// ビットマップ初期化
		GF_BGL_BmpWinAdd(
					p_draw->p_bgl, &p_wk->bmp[i], GF_BGL_FRAME3_M,
					NEWSDRAW_TOPIC_BMP_X, NEWSDRAW_TOPIC_DRAW_Y[i],
					NEWSDRAW_TOPIC_BMP_SX, NEWSDRAW_TOPIC_BMP_SY,
					NEWSDRAW_TOPIC_BMP_PAL+i, 
					NEWSDRAW_TOPIC_BMP_CGX+(NEWSDRAW_TOPIC_BMP_CGXSIZ*i) );

		GF_BGL_BmpWinDataFill( &p_wk->bmp[i], 0 );
		
		GF_BGL_BmpWinOnVReq( &p_wk->bmp[i] );
	}

	// 文字列取得
	p_wk->p_str = STRBUF_Create( NEWSDRAW_TOPIC_STRNUM, heapID );
	

	// パレットデータ
	p_wk->p_plttbuff = ArcUtil_HDL_PalDataGet( p_draw->p_handle, 
			NARC_lobby_news_lobby_news_bg_NCLR,
			&p_wk->p_pltt, heapID);


	// 通常の文字色を１パレット内の0xD、0xEに転送する
	GF_BGL_PaletteSet( GF_BGL_FRAME0_M, 
			&((u8*)p_wk->p_pltt->pRawData)[ (NEWS_PLTT_FONT*0x20)+(NEWS_PLTT_FONT_FONT*2) ],
			0x4, (NEWS_PLTT_FONT*0x20)+(NEWS_PLTT_FONTTR_FONT*2) );
	
	GF_BGL_PaletteSet( GF_BGL_FRAME0_M, 
			&((u8*)p_wk->p_pltt->pRawData)[ (NEWS_PLTT_FONT*0x20)+(NEWS_PLTT_FONT_FONT*2) ],
			0x4, (NEWS_PLTT_FONT01*0x20)+(NEWS_PLTT_FONTTR_FONT*2) );

	GF_BGL_PaletteSet( GF_BGL_FRAME0_M, 
			&((u8*)p_wk->p_pltt->pRawData)[ (NEWS_PLTT_FONT*0x20)+(NEWS_PLTT_FONT_FONT*2) ],
			0x4, (NEWS_PLTT_FONT02*0x20)+(NEWS_PLTT_FONTTR_FONT*2) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	トピックウィンドウ	破棄
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_TopicWinExit( NEWSDRAW_TOPICWIN* p_wk )
{
	int i;
	
	// パレットデータ破棄
	sys_FreeMemoryEz( p_wk->p_plttbuff );
	
	// 文字列破棄
	STRBUF_Delete( p_wk->p_str );

	// トピックデータ破棄
	for( i=0; i<NEWSDRAW_TOPIC_NUM; i++ ){
		// ビットマップ破棄
		GF_BGL_BmpWinDel( &p_wk->bmp[i] );
		
		NEWSDRAW_TopicExit( &p_wk->topic[i] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	トピックウィンドウ	メイン
 *
 *	@param	p_wk		ワーク
 *	@param	p_data		データ
 *	@param	cp_system	広場システムデータ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	変更があった
 *	@retval	FALSE	変更がなかった
 */
//-----------------------------------------------------------------------------
static BOOL NEWSDRAW_TopicWinMain( NEWSDRAW_TOPICWIN* p_wk, NEWS_DATA* p_data, const WFLBY_SYSTEM* cp_system, u32 heapID )
{
	int i;
	BOOL result;
	NEWS_TOPICTYPE type;
	NEWSDRAW_TOPIC_TRCOL trcol;
	WFLBY_VIPFLAG vip_flag;

	// VIPフラグ取得
	WFLBY_SYSTEM_GetVipFlagWk( cp_system, &vip_flag );
	
	// 下段のトピックから動かす
	for( i=NEWSDRAW_TOPIC_NUM-1; i>=0; i-- ){
		if( p_wk->topic[i].move ){
			result = NEWSDRAW_TopicMain( &p_wk->topic[i] );
			if( result == TRUE ){
				// １つ下の段にデータを写す
				if( (i+1) < NEWSDRAW_TOPIC_NUM ){
					NEWSDRAW_TopicStart( &p_wk->topic[i+1], p_wk->topic[i].p_str, NEWSDRAW_TOPIC_COUNT[i+1], &p_wk->topic[i].trcol, p_wk->p_pltt );
				}
				NEWSDRAW_TopicEnd( &p_wk->topic[i] );
			} 
		}
	}

	// 1段目のトピックが空いてたら新規トピック追加
	if( p_wk->topic[0].move == FALSE ){

		// トピックがある限り、生成できるトピックを１つ再生する
		while( NEWS_DSET_CheckToppic( p_data ) == TRUE ){
			type = NEWS_DSET_GetTopicType( p_data ); 
			// 先頭データ取得
			result = NEWS_DSET_GetTopicData( p_data, &vip_flag, type, p_wk->p_str, heapID );
			if( result == TRUE ){

				// トピックを追加できたのでオワリ
				NEWSDRAW_TopicTrColDataGet( &trcol, p_data, type, cp_system );
				NEWSDRAW_TopicStart( &p_wk->topic[0], p_wk->p_str, NEWSDRAW_TOPIC_COUNT[0], &trcol, p_wk->p_pltt );

				// 先頭のデータを消す
				NEWS_DSET_RemoveTopTopic( p_data );
				return TRUE;
			}

			// 先頭のデータを消す
			NEWS_DSET_RemoveTopTopic( p_data );
		}
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	トピックウィンドウ	描画
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_TopicWinDraw( NEWSDRAW_TOPICWIN* p_wk )
{
	int i;

	
	// トピック情報をビットマップに書き込む
	for( i=0; i<NEWSDRAW_TOPIC_NUM; i++ ){
		if( p_wk->topic[i].move == TRUE ){
			// 書き込む
			NEWSDRAW_TopicDraw( &p_wk->topic[i], &p_wk->bmp[i] );
		} 
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	トピック	初期化
 *	
 *	@param	p_wk		ワーク
 *	@param	p_draw		描画システム
 *	@param	idx			このトピックのインデックス値
 *	@param	heapID		ヒープ
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_TopicInit( NEWSDRAW_TOPIC* p_wk, NEWSDRAW_DRAWSYS* p_draw, u32 idx, u32 heapID )
{
	p_wk->move	= FALSE;
	p_wk->count	= 0;
	p_wk->p_str = STRBUF_Create(NEWSDRAW_TOPIC_STRNUM, heapID );

	p_wk->pal	= NEWS_PLTT_FONT + idx;

	// ダミーBMPWIN作成
	GF_BGL_BmpWinAdd(
				p_draw->p_bgl, &p_wk->bmp, GF_BGL_FRAME3_M,
				0, 0,
				NEWSDRAW_TOPIC_DMBMP_SX, NEWSDRAW_TOPIC_DMBMP_SY,
				NEWSDRAW_TOPIC_BMP_PAL, 0 );
}

//----------------------------------------------------------------------------
/**
 *	@brief	トピック	破棄
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_TopicExit( NEWSDRAW_TOPIC* p_wk )
{
	STRBUF_Delete( p_wk->p_str );

	//  ダミーBMPWIN破棄
	GF_BGL_BmpWinDel( &p_wk->bmp );
}

//----------------------------------------------------------------------------
/**
 *	@brief	トピック	開始
 *
 *	@param	p_wk		ワーク
 *	@param	cp_str		文字列
 *	@param	speed		スクロールに使用するシンク数
 *	@param	cp_trcol	トレーナーカラー
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_TopicStart( NEWSDRAW_TOPIC* p_wk, const STRBUF* cp_str, u32 speed, const NEWSDRAW_TOPIC_TRCOL* cp_trcol, const NNSG2dPaletteData* cp_pltt )
{
	// 文章コピー
	STRBUF_Copy( p_wk->p_str, cp_str );
	p_wk->move		= TRUE;
	p_wk->count		= 0;

// BTS通信バグ601の対処	tomoya
//	p_wk->count_max	= speed * STRBUF_GetLen( cp_str );	// 文字数から最大カウントを求める
	p_wk->count_max	= speed;
	p_wk->str_siz	= FontProc_GetPrintStrWidth( FONT_TALK, cp_str, 0 );
	p_wk->str_siz	+= NEWSDRAW_TOPIC_YOHAKU;	// 上の余白部分

	// 文字列長さオーバー
	GF_ASSERT( (NEWSDRAW_TOPIC_DMBMP_SX*8) >= p_wk->str_siz );

	// ダミービットマップに書き込む
	GF_BGL_BmpWinDataFill( &p_wk->bmp, 0 );

	// 文字を書き込む
	GF_STR_PrintSimple( &p_wk->bmp, FONT_TALK, p_wk->p_str,
			0, 0,
			MSG_NO_PUT, NULL);

	// トレーナーカラーデータを保存
	p_wk->trcol = *cp_trcol;


	// トレーナの文字色を合わせる
	NEWSDRAW_TopicTrColDataTrans( &p_wk->trcol, cp_pltt, p_wk->pal );
}

//----------------------------------------------------------------------------
/**
 *	@brief	トピック終了
 *	
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_TopicEnd( NEWSDRAW_TOPIC* p_wk )
{
	p_wk->move = FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	トピック	メイン
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static BOOL NEWSDRAW_TopicMain( NEWSDRAW_TOPIC* p_wk )
{
	BOOL ret = TRUE;
	
	if( (p_wk->count + 1) <= p_wk->count_max ){
		p_wk->count ++;
		ret = FALSE;
	}

	return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	トピックの描画	
 *
 *	@param	cp_wk	ワーク
 *	@param	p_bmp	描画先ビットマップ
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_TopicDraw( const NEWSDRAW_TOPIC* cp_wk, GF_BGL_BMPWIN* p_bmp )
{
	s32	x;	// 基準となるｘ座標
	s32 wx;	// 書き込み先ｘ
	s32 rx;	// 読み込み先ｘ
	s32 sx;	// 読み込みサイズｘ

	// 基準ｘ座標取得
	x = ( cp_wk->count * cp_wk->str_siz ) / cp_wk->count_max;

	// 余白部分、本文全表示部分で書くｘの値が変わる
	if( x < NEWSDRAW_TOPIC_YOHAKU ){
		// 余白部分
		wx = 255 - x;
		rx = 0;
		sx = x;
	}else{
		// 後ろが空白の部分
		wx = 0;
		rx = x - NEWSDRAW_TOPIC_YOHAKU;
		sx = (cp_wk->str_siz - x);
		if( sx > 255 ){
			sx = 255;
		}
	}

	// ビットマップクリーン
	GF_BGL_BmpWinFill( p_bmp, 0,
			0, 0,
			255, NEWSDRAW_TOPIC_DMBMP_SY*8 );	

	// 書き込む
	GF_BGL_BmpWinPrintEx( p_bmp,
			cp_wk->bmp.chrbuf,		// キャラクタバッファ
			rx, 0,					//  chrbuf読み込み開始位置
			NEWSDRAW_TOPIC_DMBMP_SX*8,	// chrbufのサイズ
			NEWSDRAW_TOPIC_DMBMP_SY*8,	// chrbufのサイズ
			wx, 0,						// 書き込み先位置
			sx, NEWSDRAW_TOPIC_DMBMP_SY*8, 15 );// 書き込みサイズ

	GF_BGL_BmpWinOnVReq( p_bmp );
}





//----------------------------------------------------------------------------
/**
 *	@brief	タイトルウィンドウ	描画初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_draw		描画システム
 *	@param	heapID		ヒープ
 */
//-----------------------------------------------------------------------------
#ifdef NONEQUIVALENT
static void NEWSDRAW_TitleWinInit( NEWSDRAW_TITLEWIN* p_wk, NEWSDRAW_DRAWSYS* p_draw, u32 heapID )
{
	int i;
	MSGDATA_MANAGER*	p_msgman;
	STRBUF*				p_str;

	memset( p_wk, 0, sizeof(NEWSDRAW_TITLEWIN) );

	// メッセージデータ初期化
	p_msgman	= MSGMAN_Create(MSGMAN_TYPE_NORMAL,ARC_MSG,NARC_msg_wflby_news_dat,heapID );
	p_str		= STRBUF_Create( NEWSDRAW_TITLEWIN_STRNUM, heapID );

	for( i=0; i<NEWSDRAW_TITLEWIN_NUM; i++ ){

		// タイトルエフェクト初期化
		NEWSDRAW_TitleEffInit( &p_wk->eff[i], NEWSDRAW_TITLEWIN_PLTT_OFFS[i], NEWSDRAW_TITLEWIN_SND_TBL[i] );

		// メッセージ
		GF_BGL_BmpWinAdd(
				p_draw->p_bgl, &p_wk->bmp[i], GF_BGL_FRAME0_M,
				NEWSDRAW_TITLE_BMPDATA[i].x, NEWSDRAW_TITLE_BMPDATA[i].y,
				NEWSDRAW_TITLE_BMPDATA[i].sizx, NEWSDRAW_TITLE_BMPDATA[i].sizy,
				NEWSDRAW_TITLE_BMPDATA[i].pal, NEWSDRAW_TITLE_BMPDATA[i].cgx );

		GF_BGL_BmpWinDataFill( &p_wk->bmp[i], 0 );

		// タイトルを書き込む
		MSGMAN_GetString( p_msgman, wflby_news_00+i, p_str );
		

/*
		if( i==NEWSDRAW_TITLEWIN_APLNAME ){
			FontProc_LoadFont( FONT_BUTTON, heapID );	//ボタンフォントのロード
			
			GF_STR_PrintColor(
				&p_wk->bmp[i], FONT_BUTTON, p_str, NEWSDRAW_TITLE_BMPDATA[i].dx,
				NEWSDRAW_TITLE_BMPDATA[i].dy, MSG_NO_PUT, NEWSDRAW_TITLEWIN_COL, NULL );

			FontProc_UnloadFont( FONT_BUTTON );				//ボタンフォントの破棄
		}else{
			GF_STR_PrintColor(
				&p_wk->bmp[i], FONT_SYSTEM, p_str, NEWSDRAW_TITLE_BMPDATA[i].dx,
				NEWSDRAW_TITLE_BMPDATA[i].dy, MSG_NO_PUT, NEWSDRAW_TITLEWIN_COL, NULL );
		}
//*/


		if( i==NEWSDRAW_TITLEWIN_APLNAME ){
			GF_STR_PrintColor(
				&p_wk->bmp[i], FONT_SYSTEM, p_str, NEWSDRAW_TITLE_BMPDATA[i].dx,
				NEWSDRAW_TITLE_BMPDATA[i].dy, MSG_NO_PUT, NEWSDRAW_TITLEWIN_APL_COL, NULL );
		}else{
	
			GF_STR_PrintColor(
				&p_wk->bmp[i], FONT_SYSTEM, p_str, NEWSDRAW_TITLE_BMPDATA[i].dx,
				NEWSDRAW_TITLE_BMPDATA[i].dy, MSG_NO_PUT, NEWSDRAW_TITLEWIN_COL, NULL );
		}
		
		GF_BGL_BmpWinOnVReq( &p_wk->bmp[i] );
	}

	// メッセージデータ破棄
	STRBUF_Delete( p_str );
	MSGMAN_Delete( p_msgman );
}
#else
asm static void NEWSDRAW_TitleWinInit( NEWSDRAW_TITLEWIN* p_wk, NEWSDRAW_DRAWSYS* p_draw, u32 heapID )
{
	push {r3, r4, r5, r6, r7, lr}
	sub sp, #0x40
	str r1, [sp, #0x18]
	str r2, [sp, #0x1c]
	mov r1, #0
	mov r2, #0x70
	str r0, [sp, #0x14]
	bl memset
	ldr r2, =0x0000028D // _0225D6CC
	ldr r3, [sp, #0x1c]
	mov r0, #0
	mov r1, #0x1a
	bl MSGMAN_Create
	str r0, [sp, #0x38]
	ldr r1, [sp, #0x1c]
	mov r0, #0x80
	bl STRBUF_Create
	str r0, [sp, #0x34]
	mov r0, #0
	str r0, [sp, #0x3c]
	ldr r0, =NEWSDRAW_TITLEWIN_SND_TBL // _0225D6D0
	ldr r5, =NEWSDRAW_TITLE_BMPDATA // _0225D6D4
	str r0, [sp, #0x28]
	ldr r0, =NEWSDRAW_TITLEWIN_PLTT_OFFS // _0225D6D8
	str r0, [sp, #0x24]
	ldr r0, [sp, #0x14]
	str r0, [sp, #0x20]
	add r0, #0x40
	str r0, [sp, #0x20]
_0225D5BC:
	ldr r1, [sp, #0x24]
	ldr r2, [sp, #0x28]
	ldrh r1, [r1]
	ldr r0, [sp, #0x20]
	ldr r2, [r2, #0]
	bl NEWSDRAW_TitleEffInit
	ldrb r0, [r5, #1]
	ldr r1, [sp, #0x14]
	mov r2, #0
	str r0, [sp]
	ldrb r0, [r5, #2]
	str r0, [sp, #4]
	ldrb r0, [r5, #3]
	str r0, [sp, #8]
	ldrb r0, [r5, #6]
	str r0, [sp, #0xc]
	ldrh r0, [r5, #4]
	str r0, [sp, #0x10]
	ldr r0, [sp, #0x18]
	ldrb r3, [r5]
	ldr r0, [r0, #0]
	bl GF_BGL_BmpWinAdd
	ldr r0, [sp, #0x14]
	mov r1, #0
	bl GF_BGL_BmpWinDataFill
	ldr r0, [sp, #0x38]
	ldr r1, [sp, #0x3c]
	ldr r2, [sp, #0x34]
	bl MSGMAN_GetString
	ldr r0, [sp, #0x34]
	bl STRBUF_GetLines
	str r0, [sp, #0x30]
	ldr r0, [sp, #0x34]
	ldrb r6, [r5, #8]
	bl STRBUF_GetLen
	ldr r1, [sp, #0x1c]
	add r0, r0, #1
	bl STRBUF_Create
	add r4, r0, #0
	ldr r0, [sp, #0x30]
	mov r7, #0
	cmp r0, #0
	bls _0225D68A
_0225D620:
	ldr r1, [sp, #0x34]
	add r0, r4, #0
	add r2, r7, #0
	bl STRBUF_CopyLine
	ldrb r0, [r5, #9]
	cmp r0, #0
	beq _0225D63A
	cmp r0, #1
	beq _0225D640
	cmp r0, #2
	beq _0225D656
	b _0225D666
_0225D63A:
	ldrb r0, [r5, #7]
	str r0, [sp, #0x2c]
	b _0225D666
_0225D640:
	mov r0, #0
	add r1, r4, #0
	add r2, r0, #0
	bl FontProc_GetPrintMaxLineWidth
	add r0, r0, #1
	ldrb r1, [r5, #7]
	lsr r0, r0, #1
	sub r0, r1, r0
	str r0, [sp, #0x2c]
	b _0225D666
_0225D656:
	mov r0, #0
	add r1, r4, #0
	add r2, r0, #0
	bl FontProc_GetPrintMaxLineWidth
	ldrb r1, [r5, #7]
	sub r0, r1, r0
	str r0, [sp, #0x2c]
_0225D666:
	str r6, [sp]
	mov r0, #0xff
	str r0, [sp, #4]
	ldr r0, [r5, #0xc]
	ldr r3, [sp, #0x2c]
	str r0, [sp, #8]
	mov r0, #0
	str r0, [sp, #0xc]
	ldr r0, [sp, #0x14]
	mov r1, #0
	add r2, r4, #0
	bl GF_STR_PrintColor
	ldr r0, [sp, #0x30]
	add r7, r7, #1
	add r6, #0x10
	cmp r7, r0
	blo _0225D620
_0225D68A:
	add r0, r4, #0
	bl STRBUF_Delete
	ldr r0, [sp, #0x14]
	bl GF_BGL_BmpWinOnVReq
	ldr r0, [sp, #0x28]
	add r5, #0x10
	add r0, r0, #4
	str r0, [sp, #0x28]
	ldr r0, [sp, #0x24]
	add r0, r0, #2
	str r0, [sp, #0x24]
	ldr r0, [sp, #0x20]
	add r0, #0xc
	str r0, [sp, #0x20]
	ldr r0, [sp, #0x14]
	add r0, #0x10
	str r0, [sp, #0x14]
	ldr r0, [sp, #0x3c]
	add r0, r0, #1
	str r0, [sp, #0x3c]
	cmp r0, #4
	blt _0225D5BC
	ldr r0, [sp, #0x34]
	bl STRBUF_Delete
	ldr r0, [sp, #0x38]
	bl MSGMAN_Delete
	add sp, #0x40
	pop {r3, r4, r5, r6, r7, pc}
	nop
// _0225D6CC: .4byte 0x0000028D
// _0225D6D0: .4byte NEWSDRAW_TITLEWIN_SND_TBL
// _0225D6D4: .4byte NEWSDRAW_TITLE_BMPDATA
// _0225D6D8: .4byte NEWSDRAW_TITLEWIN_PLTT_OFFS
}
#endif

//----------------------------------------------------------------------------
/**
 *	@brief	タイトルウィンドウ破棄
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_TitleWinExit( NEWSDRAW_TITLEWIN* p_wk )
{
	int i;


	for( i=0; i<NEWSDRAW_TITLEWIN_NUM; i++ ){
		GF_BGL_BmpWinDel( &p_wk->bmp[i] );

		// タイトルエフェクト破棄
		NEWSDRAW_TitleEffExit( &p_wk->eff[i] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイトルウィンドウメイン
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_TitleWinDraw( NEWSDRAW_TITLEWIN* p_wk )
{
	int i;

	for( i=0; i<NEWSDRAW_TITLEWIN_NUM; i++ ){
		NEWSDRAW_TitleEffDraw( &p_wk->eff[i] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイトルウィンドウ		エフェクト開始
 *
 *	@param	p_wk		ワーク
 *	@param	title_no	タイトルNO
	NEWSDRAW_TITLEWIN_TIME,		// 時間
	NEWSDRAW_TITLEWIN_PLAYER,	// プレイヤー表示
	NEWSDRAW_TITLEWIN_TOPIC,	// トピック
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_TitleWinEffectStart( NEWSDRAW_TITLEWIN* p_wk, u32 title_no )
{
	GF_ASSERT( title_no < NEWSDRAW_TITLEWIN_NUM );
	GF_ASSERT( title_no != NEWSDRAW_TITLEWIN_APLNAME );

	NEWSDRAW_TitleEffStart( &p_wk->eff[title_no] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイトルウィンドウ	エフェクト初期化
 *
 *	@param	p_wk		ワーク
 *	@param	offs		パレットオフセット
 *	@param	snd			サウンドNO
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_TitleEffInit( NEWSDRAW_TITLEEFF* p_wk, u16 offs, u32 snd )
{
	memset( p_wk, 0, sizeof(NEWSDRAW_TITLEEFF) );
	p_wk->offs = offs;
	p_wk->sound  = snd;
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイトルウィンドウ	エフェクト破棄
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_TitleEffExit( NEWSDRAW_TITLEEFF* p_wk )
{
	memset( p_wk, 0, sizeof(NEWSDRAW_TITLEEFF) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイトルウィンドウ	エフェクト開始
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_TitleEffStart( NEWSDRAW_TITLEEFF* p_wk )
{
	p_wk->on		= TRUE;
	p_wk->col_count = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイトルウィンドウ	エフェクトメイン
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void NEWSDRAW_TitleEffDraw( NEWSDRAW_TITLEEFF* p_wk )
{
	u32 evy;
	u16 col;

	// 終了チェック
	if( p_wk->col_count > NEWSDRAW_TITLEWIN_EFFECT_COUNT ){
		p_wk->on = FALSE;
	}

	
	if( p_wk->on == FALSE ){
		return ;
	}

	// 音を鳴らすタイミング
	if( p_wk->col_count == NEWSDRAW_TITLEWIN_EFFECT_SND_PLY ){
		Snd_SePlay( p_wk->sound );
	}

	// カラー係数を求める
	if( p_wk->col_count < NEWSDRAW_TITLEWIN_EFFECT_COUNT_HF ){
		evy = (p_wk->col_count * 16) / NEWSDRAW_TITLEWIN_EFFECT_COUNT_HF;
	
	// 待機中
	}else if( p_wk->col_count < NEWSDRAW_TITLEWIN_EFFECT_COUNT_HF+NEWSDRAW_TITLEWIN_EFFECT_COUNT_WT ){
		evy = 16;
		
	// 消えていく
	}else {
		evy = ((p_wk->col_count - (NEWSDRAW_TITLEWIN_EFFECT_COUNT_HF+NEWSDRAW_TITLEWIN_EFFECT_COUNT_WT)) * 16) / NEWSDRAW_TITLEWIN_EFFECT_COUNT_HF2;
		evy = 16 - evy;
	}

	//  設定
	col = NEWSDRAW_TITLEWIN_EFFECT_COL;
	SoftFade( &col, &p_wk->buff, 1, evy, NEWSDRAW_TITLEWIN_EFFECT_COL_END );

	// 転送
	DC_FlushRange( &p_wk->buff, 2 );
	GX_LoadBGPltt( 
			&p_wk->buff,
			p_wk->offs,
			2 );
	
	// カウント
	p_wk->col_count ++;
}



#ifdef WFLBY_DEBUG_LOBBY_NEWS_TOPIC_MAKE
static void NEWSDRAW_DEBUG_TopicMake( NEWSDRAW_WK* p_wk, WFLBY_SYSTEM* p_system, u32 heapID )
{
	MYSTATUS* p_status;
	const WFLBY_USER_PROFILE* cp_profile;

	cp_profile = WFLBY_SYSTEM_GetMyProfileLocal( p_system );
	p_status = MyStatus_AllocWork( heapID );
	WFLBY_SYSTEM_GetProfileMyStatus( cp_profile, p_status, heapID );
	
	switch( sc_WFLBY_DEBUG_LOBBY_NEWS_TOPIC_MAKE_TOPIC ){
	case NEWS_TOPICTYPE_CONNECT:			// 挨拶
		{
			NEWS_DATA_SET_CONNECT data;
			data.cp_p1	= p_status;
			data.cp_p2	= p_status;
			data.idx_p1	= WFLBY_SYSTEM_GetMyIdx( p_system );
			data.idx_p2	= WFLBY_SYSTEM_GetMyIdx( p_system );
			NEWS_DSET_SetConnect( p_wk->p_data, &data );
		}
		break;

	case NEWS_TOPICTYPE_ITEM:			// ガジェットの交換
		{
			NEWS_DATA_SET_ITEM data;
			data.cp_p1	= p_status;
			data.cp_p2	= p_status;
			data.idx_p1	= WFLBY_SYSTEM_GetMyIdx( p_system );
			data.idx_p2	= WFLBY_SYSTEM_GetMyIdx( p_system );
			data.item	= 0;
			NEWS_DSET_SetItem( p_wk->p_data, &data );
		}
		break;

	case NEWS_TOPICTYPE_MINIGAME:		// ミニゲーム
		{
			NEWS_DATA_SET_MINIGAME data;
			data.minigame = gf_mtRand() % 3;
			data.num	= 4;
			data.cp_p1	= p_status;
			data.cp_p2	= p_status;
			data.cp_p3	= p_status;
			data.cp_p4	= p_status;
			data.idx_p1	= WFLBY_SYSTEM_GetMyIdx( p_system );
			data.idx_p2	= WFLBY_SYSTEM_GetMyIdx( p_system );
			data.idx_p3	= WFLBY_SYSTEM_GetMyIdx( p_system );
			data.idx_p4	= WFLBY_SYSTEM_GetMyIdx( p_system );
			data.play	= TRUE;
			NEWS_DSET_SetMiniGame( p_wk->p_data, &data );
		}
		break;

	case NEWS_TOPICTYPE_FOOTBOARD:		// あしあとボード
		{
			NEWS_DATA_SET_FOOTBOARD data;
			data.board = WFLBY_GAME_FOOTWHITE;
			data.num	= 6;
			data.cp_p1	= p_status;
			data.idx_p1	= WFLBY_SYSTEM_GetMyIdx( p_system );
			NEWS_DSET_SetFootBoard( p_wk->p_data, &data );
		}
		break;

	case NEWS_TOPICTYPE_WORLDTIMER:		// 世界時計
		{
			NEWS_DATA_SET_WORLDTIMER	data;
			data.num	= 4;
			data.cp_p1	= p_status;
			data.cp_p2	= p_status;
			data.cp_p3	= p_status;
			data.cp_p4	= p_status;
			data.idx_p1	= WFLBY_SYSTEM_GetMyIdx( p_system );
			data.idx_p2	= WFLBY_SYSTEM_GetMyIdx( p_system );
			data.idx_p3	= WFLBY_SYSTEM_GetMyIdx( p_system );
			data.idx_p4	= WFLBY_SYSTEM_GetMyIdx( p_system );
			NEWS_DSET_SetWorldTimer( p_wk->p_data, &data );
		}
		break;

	case NEWS_TOPICTYPE_LOBBYNEWS:		// ロビーニュース
		{
			NEWS_DATA_SET_LOBBYNEWS	data;
			data.num	= 4;
			data.cp_p1	= p_status;
			data.cp_p2	= p_status;
			data.cp_p3	= p_status;
			data.cp_p4	= p_status;
			data.idx_p1	= WFLBY_SYSTEM_GetMyIdx( p_system );
			data.idx_p2	= WFLBY_SYSTEM_GetMyIdx( p_system );
			data.idx_p3	= WFLBY_SYSTEM_GetMyIdx( p_system );
			data.idx_p4	= WFLBY_SYSTEM_GetMyIdx( p_system );
			NEWS_DSET_SetLobbyNews( p_wk->p_data, &data );
		}
		break;

	case NEWS_TOPICTYPE_TIMEEVENT:		// 時間イベント
		{
			NEWS_DATA_SET_TIMEEVENT	data;
			data.cp_time	= p_wk->cp_nowtime;
			data.event_no	= WFLBY_EVENT_FIRE;
			NEWS_DSET_SetTimeEvent( p_wk->p_data, &data );
		}
		break;

	case NEWS_TOPICTYPE_VIPIN:			// VIPが入室したときのニュース
		{
			NEWS_DATA_SET_TIMEEVENT	data;
			data.cp_time	= p_wk->cp_nowtime;
			data.event_no	= WFLBY_EVENT_PARADE;
			NEWS_DSET_SetTimeEvent( p_wk->p_data, &data );
		}
		break;
	}

	sc_WFLBY_DEBUG_LOBBY_NEWS_TOPIC_MAKE_TOPIC = (sc_WFLBY_DEBUG_LOBBY_NEWS_TOPIC_MAKE_TOPIC + 1) % NEWS_TOPICTYPE_NUM;

	sys_FreeMemoryEz( p_status );
}
#endif
