//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_anketo.c
 *	@brief		アンケート画面
 *	@author		tomoya takahashi
 *	@data		2008.05.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#include "common.h"


#include "gflib/strbuf_family.h"	// 任意質問で使用

#include "system/clact_util.h"
#include "system/wipe.h"
#include "system/render_oam.h"
#include "system/pm_overlay.h"
#include "system/fontproc.h"
#include "system/msgdata.h"
#include "system/window.h"
#include "system/wordset.h"
#include "system/bmp_menu.h"

#include "communication/wm_icon.h"


#include "msgdata/msg.naix"
#include "msgdata/msg_wifi_hiroba.h"
#include "msgdata/msg_wifi_50_answers_a.h"
#include "msgdata/msg_wifi_50_answers_b.h"
#include "msgdata/msg_wifi_50_answers_c.h"
#include "msgdata/msg_wifi_50_questions.h"
#include "msgdata/msg_wifi_Special_answers_a.h"
#include "msgdata/msg_wifi_Special_answers_b.h"
#include "msgdata/msg_wifi_Special_answers_c.h"
#include "msgdata/msg_wifi_Special_questions.h"

#include "graphic/wifi_lobby_other.naix"
#include "graphic/lobby_news.naix"

#include "savedata/config.h"
#include "savedata/wifi_hiroba_save.h"

#include "wflby_anketo.h"
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
#endif


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	描画システム
//=====================================
// VRAM転送タスク
#define ANKETO_VRAMTRANS_TASKNUM		( 32 )
// OAM
#define ANKETO_OAM_CONTNUM		( 32 )
#define ANKETO_RESMAN_NUM		( 4 )	// OAMリソースマネージャ数
#define ANKETO_SF_MAT_Y		( FX32_CONST(256) )
static const CHAR_MANAGER_MAKE sc_ANKETO_CHARMAN_INIT = {
	ANKETO_OAM_CONTNUM,
	16*1024,	// 16K
	0,	// 16K
	HEAPID_ANKETO
};


//-------------------------------------
///	パレット設定
//=====================================
enum {
	ANKETO_PLTT_MAIN_BACK00,
	ANKETO_PLTT_MAIN_BACK01,
	ANKETO_PLTT_MAIN_BACK02,
	ANKETO_PLTT_MAIN_BACK03,
	ANKETO_PLTT_MAIN_TALKFONT,
	ANKETO_PLTT_MAIN_TALKWIN,
	ANKETO_PLTT_MAIN_SYSWIN,
} ;
#define ANKETO_PLTT_MAINBACK_PLTTNUM	(4)

#define ANKETO_PLTT_FONT_COL_INPUT		( GF_PRINTCOLOR_MAKE( 1, 2, 0 ) )
#define ANKETO_PLTT_FONT_COL_OUTPUT00	( GF_PRINTCOLOR_MAKE( 9, 10, 0 ) )
#define ANKETO_PLTT_FONT_COL_OUTPUT01	( GF_PRINTCOLOR_MAKE( 11, 12, 0 ) )
#define ANKETO_PLTT_FONT_COL_VIP00		( GF_PRINTCOLOR_MAKE( 13, 14, 0 ) )
#define ANKETO_PLTT_FONT_COL_VIP01		( GF_PRINTCOLOR_MAKE( 15, 14, 0 ) )

#define ANKETO_PLTT_TITLEFONT_COL_INPUT		( GF_PRINTCOLOR_MAKE( 7, 8, 0 ) )
#define ANKETO_PLTT_TITLEFONT_COL_OUTPUT00	( GF_PRINTCOLOR_MAKE( 3, 4, 0 ) )
#define ANKETO_PLTT_TITLEFONT_COL_OUTPUT01	( GF_PRINTCOLOR_MAKE( 5, 6, 0 ) )


//-------------------------------------
///	会話ウィンドウ
//=====================================
#define ANKETO_TALKWIN_CGX		(1)	
#define ANKETO_TALKWIN_PAL		(ANKETO_PLTT_MAIN_TALKWIN)
#define ANKETO_TALKWIN_CGXEND	(ANKETO_TALKWIN_CGX+TALK_WIN_CGX_SIZ)

// メイン画面のシステムウィンドウ
#define ANKETO_SYSTEMWIN_CGX		(ANKETO_TALKWIN_CGXEND)
#define ANKETO_SYSTEMWIN_PAL		(ANKETO_PLTT_MAIN_SYSWIN)
#define ANKETO_SYSTEMWIN_CGXEND		(ANKETO_SYSTEMWIN_CGX+MENU_WIN_CGX_SIZ)


#define ANKETO_TALK_TALKWIN_X		( 2 )
#define ANKETO_TALK_TALKWIN_Y		( 19 )
#define ANKETO_TALK_TALKWIN_SIZX	( 27 )
#define ANKETO_TALK_TALKWIN_SIZY	( 4 )
#define ANKETO_TALK_TALKWIN_CGX		( ANKETO_SYSTEMWIN_CGXEND )
#define ANKETO_TALK_TALKWIN_CGXEND	( ANKETO_TALK_TALKWIN_CGX+(ANKETO_TALK_TALKWIN_SIZX*ANKETO_TALK_TALKWIN_SIZY) )
#define ANKETO_TALK_COL			( GF_PRINTCOLOR_MAKE( 1, 2, 15 ) )

#define ANKETO_TALK_YESNOWIN_X		( 25 )
#define ANKETO_TALK_YESNOWIN_Y		( 13 )
#define ANKETO_TALK_YESNOWIN_SIZX	( 6 )
#define ANKETO_TALK_YESNOWIN_SIZY	( 4 )
#define ANKETO_TALK_YESNOWIN_CGX		( ANKETO_TALK_TALKWIN_CGXEND )
#define ANKETO_TALK_YESNOWIN_CGXEND	( ANKETO_TALK_YESNOWIN_CGX+(ANKETO_TALK_YESNOWIN_SIZX*ANKETO_TALK_YESNOWIN_SIZY) )
static const BMPWIN_DAT sc_YESNO_BMPDAT = {
	GF_BGL_FRAME1_M,
	ANKETO_TALK_YESNOWIN_X,		ANKETO_TALK_YESNOWIN_Y,
	ANKETO_TALK_YESNOWIN_SIZX,	ANKETO_TALK_YESNOWIN_SIZY,
	ANKETO_PLTT_MAIN_TALKFONT,	ANKETO_TALK_YESNOWIN_CGX,
};


//-------------------------------------
///	メッセージマネージャ定数
//=====================================
enum {
	ANKETO_MSGMAN_MSG,		// 通常メッセージ
	ANKETO_MSGMAN_Q_NORMAL,	// 質問ノーマル
	ANKETO_MSGMAN_Q_SPECIAL,// 質問SPECIAL
	ANKETO_MSGMAN_A_N_A,	// ノーマル	回答A
	ANKETO_MSGMAN_A_N_B,	// ノーマル	回答B
	ANKETO_MSGMAN_A_N_C,	// ノーマル	回答C
	ANKETO_MSGMAN_A_S_A,	// スペシャル	回答A
	ANKETO_MSGMAN_A_S_B,	// スペシャル	回答B
	ANKETO_MSGMAN_A_S_C,	// スペシャル	回答C
	ANKETO_MSGMAN_NUM,		// マネージャ数
} ;
#define ANKETO_MSGMAN_STRBUFNUM	(256)


//-------------------------------------
///	アンケート入力画面
//=====================================
enum {
	ANKETO_INPUT_BMP_ANS,
	ANKETO_INPUT_BMP_NUM,
} ;
#define ANKETO_INPUT_OAM_CONTID	( 20 )
#define ANKETO_INPUT_OAM_BGPRI	( 0 )

#define ANKETO_INPUTWIN_X			( 11 )
#define ANKETO_INPUTWIN_Y			( 7 )
#define ANKETO_INPUTWIN_SIZX		( 15 )
#define ANKETO_INPUTWIN_SIZY		( 8 )
#define ANKETO_INPUTWIN_CGX			( 1 )
#define ANKETO_INPUTWIN_CGXEND		( ANKETO_INPUTWIN_CGX+(ANKETO_INPUTWIN_SIZX*ANKETO_INPUTWIN_SIZY) )
#define ANKETO_INPUT_ANSWER_COL			( ANKETO_PLTT_FONT_COL_INPUT )
#define ANKETO_INPUTWIN_ANS00_Y		( 0 )
#define ANKETO_INPUTWIN_ANS01_Y		( 24 )
#define ANKETO_INPUTWIN_ANS02_Y		( 48 )
static const BMPWIN_DAT sc_ANKETO_INPUT_BMPDAT[ ANKETO_INPUT_BMP_NUM ] = {
	{
		GF_BGL_FRAME2_M,
		ANKETO_INPUTWIN_X,		ANKETO_INPUTWIN_Y,
		ANKETO_INPUTWIN_SIZX,	ANKETO_INPUTWIN_SIZY,
		ANKETO_PLTT_MAIN_BACK03,	ANKETO_INPUTWIN_CGX,
	},
};

enum{
	ANKETO_INPUT_SEQ_ANKETO_YARU_Q,		// アンケートやる？
	ANKETO_INPUT_SEQ_ANKETO_YARU_Q_ON,// アンケートやる？
	ANKETO_INPUT_SEQ_ANKETO_YARU_Q_WAIT,// アンケートやる？
//	ANKETO_INPUT_SEQ_ANKETO_RECV,		// 受信開始
//	ANKETO_INPUT_SEQ_ANKETO_RECVWAIT,	// 受信中
	ANKETO_INPUT_SEQ_ANKETO_Q_DRAW00,	// 質問描画
	ANKETO_INPUT_SEQ_ANKETO_Q_DRAW,		// 質問描画
	ANKETO_INPUT_SEQ_ANKETO_Q_SELECT,	// 回答選択
	ANKETO_INPUT_SEQ_ANKETO_SEND,		// 受信開始
	ANKETO_INPUT_SEQ_ANKETO_SENDWAIT,	// 受信中
	ANKETO_INPUT_SEQ_ANKETO_ARIGATO,	// ありがとうメッセージ描画
	ANKETO_INPUT_SEQ_ANKETO_END,		// 終了
	ANKETO_INPUT_SEQ_ANKETO_MASWAIT,	// メッセージ待ち
};
static const VecFx32 sc_ANKETO_INPUT_CURSOR_MAT[] = {
	{ FX32_CONST(72),	FX32_CONST(64),0 },
	{ FX32_CONST(72),	FX32_CONST(88),0 },
	{ FX32_CONST(72),	FX32_CONST(112),0 },
};



//-------------------------------------
///	アンケート出力画面
//=====================================
enum {
//	ANKETO_OUTPUT_SEQ_ANKETO_RECV,			// アンケート受信
//	ANKETO_OUTPUT_SEQ_ANKETO_RECVWAIT,		// アンケート受信中
	ANKETO_OUTPUT_SEQ_ANKETO_NQ_WRITE00,	// こんかいは
	ANKETO_OUTPUT_SEQ_ANKETO_NQ_WRITE01,	// 内容
	ANKETO_OUTPUT_SEQ_ANKETO_NQ_WRITE02,	// でした
	ANKETO_OUTPUT_SEQ_ANKETO_NQ_WRITE03,	// こんかいはこんなでした
	ANKETO_OUTPUT_SEQ_ANKETO_NRESULT,		// 結果表示
	ANKETO_OUTPUT_SEQ_ANKETO_NRESULT_MAIN,	// 結果表示
	ANKETO_OUTPUT_SEQ_ANKETO_NA_WAIT,		// Aボタン待ち
	ANKETO_OUTPUT_SEQ_ANKETO_YESNO_DRAW,	// YESNO表示
	ANKETO_OUTPUT_SEQ_ANKETO_YESNO_WAIT,	// YESNO待ち
	ANKETO_OUTPUT_SEQ_ANKETO_LQ_WIPEOUT,	// 一回フェード
	ANKETO_OUTPUT_SEQ_ANKETO_LQ_WIPEOUTWAIT,// 一回フェード
	ANKETO_OUTPUT_SEQ_ANKETO_LQ_WIPEIN,		// 一回フェード
	ANKETO_OUTPUT_SEQ_ANKETO_LQ_WIPEINWAIT,	// 一回フェード
	ANKETO_OUTPUT_SEQ_ANKETO_LQ_WRITE00,	// 前回は
	ANKETO_OUTPUT_SEQ_ANKETO_LQ_WRITE01,	// 内容
	ANKETO_OUTPUT_SEQ_ANKETO_LQ_WRITE02,	// でした
	ANKETO_OUTPUT_SEQ_ANKETO_LQ_WRITE03,	// 前回はこんなでした
	ANKETO_OUTPUT_SEQ_ANKETO_LRESULT,		// 結果表示
	ANKETO_OUTPUT_SEQ_ANKETO_LRESULT_MAIN,	// 結果表示
	ANKETO_OUTPUT_SEQ_ANKETO_LA_WAIT,		// Aボタン待ち

	ANKETO_OUTPUT_SEQ_ANKETO_ENDMSG,		// 終了メッセージ
	ANKETO_OUTPUT_SEQ_ANKETO_END,			// 終了

	ANKETO_OUTPUT_SEQ_ANKETO_MSGWAIT,		// メッセージ待ち
};
enum {
	ANKETO_OUTPUT_BMP_TITLE,
	ANKETO_OUTPUT_BMP_ANS,
	ANKETO_OUTPUT_BMP_ANS_BAR00,
	ANKETO_OUTPUT_BMP_ANS_BAR01,
	ANKETO_OUTPUT_BMP_ANS_BAR02,
	ANKETO_OUTPUT_BMP_ANS_MY,
	ANKETO_OUTPUT_BMP_NUM,
} ;

// TITLE
#define ANKETO_OUTPUTWIN_TITLE_X		( 7 )
#define ANKETO_OUTPUTWIN_TITLE_Y		( 1 )
#define ANKETO_OUTPUTWIN_TITLE_SIZX		( 18 )
#define ANKETO_OUTPUTWIN_TITLE_SIZY		( 3 )
#define ANKETO_OUTPUTWIN_TITLE_CGX		( 1 )
#define ANKETO_OUTPUTWIN_TITLE_CGXEND	( ANKETO_OUTPUTWIN_TITLE_CGX+(ANKETO_OUTPUTWIN_TITLE_SIZX*ANKETO_OUTPUTWIN_TITLE_SIZY) )

// ANS
#define ANKETO_OUTPUTWIN_ANS_X			( 5 )
#define ANKETO_OUTPUTWIN_ANS_Y			( 5 )
#define ANKETO_OUTPUTWIN_ANS_SIZX		( 12 )
#define ANKETO_OUTPUTWIN_ANS_SIZY		( 9 )
#define ANKETO_OUTPUTWIN_ANS_CGX		( ANKETO_OUTPUTWIN_TITLE_CGXEND )
#define ANKETO_OUTPUTWIN_ANS_CGXEND		( ANKETO_OUTPUTWIN_ANS_CGX+(ANKETO_OUTPUTWIN_ANS_SIZX*ANKETO_OUTPUTWIN_ANS_SIZY) )
#define ANKETO_OUTPUTWIN_ANS00_Y		( 0 )
#define ANKETO_OUTPUTWIN_ANS01_Y		( 24 )
#define ANKETO_OUTPUTWIN_ANS02_Y		( 48 )

// BAR00
#define ANKETO_OUTPUTWIN_BAR00_X		( 0x11 )
#define ANKETO_OUTPUTWIN_BAR00_Y		( 5 )
#define ANKETO_OUTPUTWIN_BAR00_SIZX		( 11 )
#define ANKETO_OUTPUTWIN_BAR00_SIZY		( 2 )
#define ANKETO_OUTPUTWIN_BAR00_CGX		( ANKETO_OUTPUTWIN_ANS_CGXEND )
#define ANKETO_OUTPUTWIN_BAR00_CGXEND	( ANKETO_OUTPUTWIN_BAR00_CGX+(ANKETO_OUTPUTWIN_BAR00_SIZX*ANKETO_OUTPUTWIN_BAR00_SIZY) )

// BAR01
#define ANKETO_OUTPUTWIN_BAR01_X		( 0x11 )
#define ANKETO_OUTPUTWIN_BAR01_Y		( 8 )
#define ANKETO_OUTPUTWIN_BAR01_SIZX		( 11 )
#define ANKETO_OUTPUTWIN_BAR01_SIZY		( 2 )
#define ANKETO_OUTPUTWIN_BAR01_CGX		( ANKETO_OUTPUTWIN_BAR00_CGXEND )
#define ANKETO_OUTPUTWIN_BAR01_CGXEND	( ANKETO_OUTPUTWIN_BAR01_CGX+(ANKETO_OUTPUTWIN_BAR01_SIZX*ANKETO_OUTPUTWIN_BAR01_SIZY) )

// BAR02
#define ANKETO_OUTPUTWIN_BAR02_X		( 0x11 )
#define ANKETO_OUTPUTWIN_BAR02_Y		( 11 )
#define ANKETO_OUTPUTWIN_BAR02_SIZX		( 11 )
#define ANKETO_OUTPUTWIN_BAR02_SIZY		( 2 )
#define ANKETO_OUTPUTWIN_BAR02_CGX		( ANKETO_OUTPUTWIN_BAR01_CGXEND )
#define ANKETO_OUTPUTWIN_BAR02_CGXEND	( ANKETO_OUTPUTWIN_BAR02_CGX+(ANKETO_OUTPUTWIN_BAR02_SIZX*ANKETO_OUTPUTWIN_BAR02_SIZY) )

// MY
#define ANKETO_OUTPUTWIN_MY_X		( 3 )
#define ANKETO_OUTPUTWIN_MY_Y		( 15 )
#define ANKETO_OUTPUTWIN_MY_SIZX	( 25 )
#define ANKETO_OUTPUTWIN_MY_SIZY	( 2 )
#define ANKETO_OUTPUTWIN_MY_CGX		( ANKETO_OUTPUTWIN_BAR02_CGXEND )
#define ANKETO_OUTPUTWIN_MY_CGXEND	( ANKETO_OUTPUTWIN_MY_CGX+(ANKETO_OUTPUTWIN_MY_SIZX*ANKETO_OUTPUTWIN_MY_SIZY) )

#define ANKETO_OUTPUTWIN_MY_NAME_WRITE_SIZE_X	( 12*8 )
#define ANKETO_OUTPUTWIN_MY_ANSWER_WRITE_SIZE_X	( 13*8 )
#define ANKETO_OUTPUTWIN_MY_ANSWER_X			( 12*8 )



static const BMPWIN_DAT sc_ANKETO_OUTPUT_BMPDAT[ ANKETO_OUTPUT_BMP_NUM ] = {

	// TITLE
	{
		GF_BGL_FRAME2_M,
		ANKETO_OUTPUTWIN_TITLE_X,		ANKETO_OUTPUTWIN_TITLE_Y,
		ANKETO_OUTPUTWIN_TITLE_SIZX,	ANKETO_OUTPUTWIN_TITLE_SIZY,
		ANKETO_PLTT_MAIN_BACK03,	ANKETO_OUTPUTWIN_TITLE_CGX,
	},

	// ANS
	{
		GF_BGL_FRAME2_M,
		ANKETO_OUTPUTWIN_ANS_X,		ANKETO_OUTPUTWIN_ANS_Y,
		ANKETO_OUTPUTWIN_ANS_SIZX,	ANKETO_OUTPUTWIN_ANS_SIZY,
		ANKETO_PLTT_MAIN_BACK03,	ANKETO_OUTPUTWIN_ANS_CGX,
	},

	// BAR00
	{
		GF_BGL_FRAME2_M,
		ANKETO_OUTPUTWIN_BAR00_X,		ANKETO_OUTPUTWIN_BAR00_Y,
		ANKETO_OUTPUTWIN_BAR00_SIZX,	ANKETO_OUTPUTWIN_BAR00_SIZY,
		ANKETO_PLTT_MAIN_TALKFONT,	ANKETO_OUTPUTWIN_BAR00_CGX,
	},

	// BAR01
	{
		GF_BGL_FRAME2_M,
		ANKETO_OUTPUTWIN_BAR01_X,		ANKETO_OUTPUTWIN_BAR01_Y,
		ANKETO_OUTPUTWIN_BAR01_SIZX,	ANKETO_OUTPUTWIN_BAR01_SIZY,
		ANKETO_PLTT_MAIN_TALKFONT,	ANKETO_OUTPUTWIN_BAR01_CGX,
	},

	// BAR02
	{
		GF_BGL_FRAME2_M,
		ANKETO_OUTPUTWIN_BAR02_X,		ANKETO_OUTPUTWIN_BAR02_Y,
		ANKETO_OUTPUTWIN_BAR02_SIZX,	ANKETO_OUTPUTWIN_BAR02_SIZY,
		ANKETO_PLTT_MAIN_TALKFONT,	ANKETO_OUTPUTWIN_BAR02_CGX,
	},

	// MY
	{
		GF_BGL_FRAME2_M,
		ANKETO_OUTPUTWIN_MY_X,		ANKETO_OUTPUTWIN_MY_Y,
		ANKETO_OUTPUTWIN_MY_SIZX,	ANKETO_OUTPUTWIN_MY_SIZY,
		ANKETO_PLTT_MAIN_BACK03,	ANKETO_OUTPUTWIN_MY_CGX,
	},
	
};


// バー表示
#define ANKETO_BAR_100WIDTH	( 80 )
#define ANKETO_BAR_COUNTMAX	( 20 )
#define ANKETO_BAR_CG_X		( 247 )
#define ANKETO_BAR_CG_Y		( 16 )
#define ANKETO_BAR_CG_SIZX		( 1 )
#define ANKETO_BAR_CG_SIZY		( 16 )
#define ANKETO_BAR_CGFILE_X		( 256 )
#define ANKETO_BAR_CGFILE_Y		( 32 )
#define ANKETO_BAR_WRITE_X		( 4 )		// BMP描画開始位置


//-------------------------------------
///	アンケート質問	メッセージバッファ
//=====================================
#define ANKETO_MESSAGE_BUFFNUM	( 256 )

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	アンケート基本情報
//
//// 受信メッセージ
//=====================================
typedef struct {
	u32		question_no;
	u32		anketo_no;
	BOOL	special;
	BOOL	make_question;
	STRBUF* p_question;
	STRBUF* p_ans[ ANKETO_ANSWER_NUM ];
} ANKETO_QUESTION_DATA;


//-------------------------------------
///	アンケート入力画面の初期化
//=====================================
typedef struct {
	GF_BGL_BMPWIN win[ ANKETO_INPUT_BMP_NUM ];
	s32 cursor;
	CLACT_WORK_PTR		p_cursor_act;
	CLACT_U_RES_OBJ_PTR	p_cursor_res[ANKETO_RESMAN_NUM];
	u16 seq;
	u16 ret_seq;

	ANKETO_QUESTION_DATA question_now;
} ANKETO_INPUT;

//-------------------------------------
///	アンケート出力画面の初期化
//=====================================
typedef struct {
	GF_BGL_BMPWIN win[ ANKETO_OUTPUT_BMP_NUM ];
	u16 seq;
	u16 ret_seq;

	s32 count;	// バー表示カウンタ
	s32	draw_width[ANKETO_ANSWER_NUM]; 
	s32	drawend_width[ANKETO_ANSWER_NUM]; 

	void* p_charres;
	NNSG2dCharacterData* p_char;

	ANKETO_QUESTION_DATA now;
	ANKETO_QUESTION_DATA last;

	ANKETO_QUESTION_RESULT result_now;
	ANKETO_QUESTION_RESULT result_last;
} ANKETO_OUTPUT;


//-------------------------------------
///	画面構成ワーク
//=====================================
typedef union{
	ANKETO_INPUT input;
	ANKETO_OUTPUT output;
} ANKETO_LOCAL_WK;



//-------------------------------------
///	会話ウィンドウシステム
//=====================================
typedef struct {
	u32 msgno;
	s32 msgwait;
	GF_BGL_BMPWIN win;
	STRBUF*			p_str;
	void*			p_timewait;

	BMPMENU_WORK* p_yesno;
} ANKETO_TALKWIN;



//-------------------------------------
///	メッセージシステムの初期化
//=====================================
typedef struct {
	MSGDATA_MANAGER*	p_msgman[ANKETO_MSGMAN_NUM];		// 基本メッセージ
	WORDSET*			p_wordset;							// ワードセット
	STRBUF*				p_msgstr;							// メッセージバッファ
	STRBUF*				p_msgtmp;							// メッセージバッファ

	WFLBY_SYSTEM*		p_system;							// 名前表示用
} ANKETO_MSGMAN;


//-------------------------------------
///	描画系ワーク
//=====================================
typedef struct {
	// BG
	GF_BGL_INI*				p_bgl;

	// OAM
    CLACT_SET_PTR           p_clactset;		// セルアクターセット
    CLACT_U_EASYRENDER_DATA renddata;       // 簡易レンダーデータ
    CLACT_U_RES_MANAGER_PTR p_resman[ANKETO_RESMAN_NUM]; // キャラ・パレットリソースマネージャ
	// アーカイブ
	ARCHANDLE* p_handle;
} ANKETO_DRAWSYS;


//-------------------------------------
///	アンケート構造体
//=====================================
typedef struct {
	ANKETO_DRAWSYS	drawsys;
	ANKETO_MSGMAN	msgsys;
	ANKETO_LOCAL_WK	local_wk;
	ANKETO_TALKWIN	talkwin;
} ANKETO_WORK;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
static void ANKETO_VBlank( void* p_work );

//-------------------------------------
///	描画システム
//=====================================
static void ANKETO_InitGraphic( ANKETO_DRAWSYS* p_wk, SAVEDATA* p_save, u32 heapID );
static void ANKETO_ExitGraphic( ANKETO_DRAWSYS* p_wk );
static void ANKETO_MainGraphic( ANKETO_DRAWSYS* p_wk );
static void ANKETO_VBlankGraphic( ANKETO_DRAWSYS* p_wk );
static void ANKETO_BgInit( ANKETO_DRAWSYS* p_wk, CONFIG* p_config, u32 heapID );
static void ANKETO_BgExit( ANKETO_DRAWSYS* p_wk );
static void ANKETO_OamInit( ANKETO_DRAWSYS* p_wk, u32 heapID );
static void ANKETO_OamExit( ANKETO_DRAWSYS* p_wk );


//-------------------------------------
///	メッセージシステム
//=====================================
static void ANKETO_MsgManInit( ANKETO_MSGMAN* p_wk, WFLBY_SYSTEM* p_system, u32 heapID );
static void ANKETO_MsgManExit( ANKETO_MSGMAN* p_wk );
static STRBUF* ANKETO_MsgManGetStr( ANKETO_MSGMAN* p_wk, u32 type, u32 msg );
static STRBUF* ANKETO_MsgManGetQuestionStr( ANKETO_MSGMAN* p_wk, u32 msg );
static STRBUF* ANKETO_MsgManGetAnswerStr( ANKETO_MSGMAN* p_wk, u32 msg, u32 answer );
static void ANKETO_MsgManSetMyName( ANKETO_MSGMAN* p_wk, u32 bufno, u32 heapID );


//-------------------------------------
///	質問データワーク
//=====================================
static void ANKETO_QUESTION_DATA_DebugInit( ANKETO_QUESTION_DATA* p_wk );
static void ANKETO_QUESTION_DATA_Init( ANKETO_QUESTION_DATA* p_wk, BOOL now, u32 heapID );
static void ANKETO_QUESTION_DATA_Exit( ANKETO_QUESTION_DATA* p_wk );
static STRBUF* ANKETO_QUESTION_DATA_GetQuestionStr( const ANKETO_QUESTION_DATA* cp_wk, ANKETO_MSGMAN* p_msg );
static STRBUF* ANKETO_QUESTION_DATA_GetAnswerStr( const ANKETO_QUESTION_DATA* cp_wk, ANKETO_MSGMAN* p_msg, u32 answer );

//-------------------------------------
///	結果データワーク
//=====================================
static void ANKETO_QUESTION_RESULT_DebugInit( ANKETO_QUESTION_RESULT* p_wk );
static void ANKETO_QUESTION_RESULT_Init( ANKETO_QUESTION_RESULT* p_wk );
static u32 ANKETO_QUESTION_RESULT_GetBerWidth( const ANKETO_QUESTION_RESULT* cp_wk, u32 answer, u32 max_width );
static void ANKETO_QUESTION_RESULT_CalcHirobaResult( ANKETO_QUESTION_RESULT* p_wk, const WFLBY_SYSTEM* cp_system );


//-------------------------------------
///	入力画面
//=====================================
static void ANKETO_LOCAL_INPUT_Init( ANKETO_LOCAL_WK* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys, ANKETO_PARAM* p_param, u32 heapID );
static BOOL ANKETO_LOCAL_INPUT_Main( ANKETO_LOCAL_WK* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys, ANKETO_TALKWIN* p_talkwin, ANKETO_PARAM* p_param, u32 heapID );
static void ANKETO_LOCAL_INPUT_Exit( ANKETO_LOCAL_WK* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys );

static void ANKETO_INPUT_Init( ANKETO_INPUT* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys, u32 heapID );
static BOOL ANKETO_INPUT_Main( ANKETO_INPUT* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys, ANKETO_TALKWIN* p_talkwin, ANKETO_PARAM* p_param, u32 heapID );
static void ANKETO_INPUT_Exit( ANKETO_INPUT* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys );
static void ANKETO_INPUT_DrawAnswer( ANKETO_INPUT* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys );
static void ANKETO_INPUT_DrawCursor( ANKETO_INPUT* p_wk );
static void ANKETO_INPUT_AnmCursor( ANKETO_INPUT* p_wk );


//-------------------------------------
///	出力画面
//=====================================
static void ANKETO_LOCAL_OUTPUT_Init( ANKETO_LOCAL_WK* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys, ANKETO_PARAM* p_param, u32 heapID );
static BOOL ANKETO_LOCAL_OUTPUT_Main( ANKETO_LOCAL_WK* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys, ANKETO_TALKWIN* p_talkwin, ANKETO_PARAM* p_param, u32 heapID );
static void ANKETO_LOCAL_OUTPUT_Exit( ANKETO_LOCAL_WK* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys );

static void ANKETO_OUTPUT_Init( ANKETO_OUTPUT* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys, ANKETO_PARAM* p_param, u32 heapID );
static BOOL ANKETO_OUTPUT_Main( ANKETO_OUTPUT* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys, ANKETO_TALKWIN* p_talkwin, ANKETO_PARAM* p_param, u32 heapID );
static void ANKETO_OUTPUT_Exit( ANKETO_OUTPUT* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys );
static void ANKETO_OUTPUT_DrawTitle( ANKETO_OUTPUT* p_wk, ANKETO_MSGMAN* p_msg, u32 msgidx, GF_PRINTCOLOR col );
static void ANKETO_OUTPUT_DrawAnswer( ANKETO_OUTPUT* p_wk, const ANKETO_QUESTION_DATA* cp_data, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys, const WFLBY_ANKETO* cp_answer, u32 heapID, GF_PRINTCOLOR col, GF_PRINTCOLOR vipcol, BOOL vip );
static void ANKETO_OUTPUT_DrawBarStart( ANKETO_OUTPUT* p_wk, const ANKETO_QUESTION_RESULT* cp_data, ANKETO_DRAWSYS* p_drawsys, u32 palno );
static BOOL ANKETO_OUTPUT_DrawBarMain( ANKETO_OUTPUT* p_wk, ANKETO_DRAWSYS* p_drawsys );
static void ANKETO_OUTPUT_SetLastWeekGraphic( ANKETO_OUTPUT* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys, u32 heapID );


//-------------------------------------
///	会話ウィンドウ
//=====================================
static void ANKETO_TalkWin_Init( ANKETO_TALKWIN* p_wk, ANKETO_DRAWSYS* p_sys, SAVEDATA* p_save, u32 heapID );
static void ANKETO_TalkWin_Exit( ANKETO_TALKWIN* p_wk, u32 heapID );
static void ANKETO_TalkWin_Print( ANKETO_TALKWIN* p_wk, const STRBUF* cp_str );
static void ANKETO_TalkWin_PrintAll( ANKETO_TALKWIN* p_wk, const STRBUF* cp_str );
static void ANKETO_TalkWin_StartTimeWait( ANKETO_TALKWIN* p_wk );
static void ANKETO_TalkWin_StopTimeWait( ANKETO_TALKWIN* p_wk );
static BOOL ANKETO_TalkWin_CheckTimeWait( const ANKETO_TALKWIN* cp_wk );
static BOOL ANKETO_TalkWin_EndWait( const ANKETO_TALKWIN* cp_wk );
static void ANKETO_TalkWin_Off( ANKETO_TALKWIN* p_wk );
static void ANKETO_TalkWin_StartYesNo( ANKETO_TALKWIN* p_wk, ANKETO_DRAWSYS* p_sys, u32 heapID );
static u32	ANKETO_TalkWin_MainYesNo( ANKETO_TALKWIN* p_wk, u32 heapID );
static void ANKETO_TalkWin_EndYesNo( ANKETO_TALKWIN* p_wk, u32 heapID );


//-----------------------------------------------------------------------------
/**
 *					情報
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	BANK設定
//=====================================
static const GF_BGL_DISPVRAM sc_ANKETO_BANK = {
	GX_VRAM_BG_256_AB,				// メイン2DエンジンのBG
	GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット
	GX_VRAM_SUB_BG_128_C,			// サブ2DエンジンのBG
	GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット
	GX_VRAM_OBJ_16_G,				// メイン2DエンジンのOBJ
	GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット
	GX_VRAM_SUB_OBJ_NONE,			// サブ2DエンジンのOBJ
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
#define ANKETO_BGCNT_NUM	( 4 )	// ＢＧコントロールテーブル数
static const u32 sc_ANKETO_BGCNT_FRM[ ANKETO_BGCNT_NUM ] = {
	GF_BGL_FRAME0_M,
	GF_BGL_FRAME1_M,
	GF_BGL_FRAME2_M,
	GF_BGL_FRAME0_S,
};
static const GF_BGL_BGCNT_HEADER sc_ANKETO_BGCNT_DATA[ ANKETO_BGCNT_NUM ] = {
	{	// GF_BGL_FRAME0_M	（背景）
		0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_01,
		3, 0, 0, FALSE
	},
	{	// GF_BGL_FRAME1_M	（フレーム面）
		0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
		1, 0, 0, FALSE
	},
	{	// GF_BGL_FRAME2_M	（メッセージ面）
		0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
		2, 0, 0, FALSE
	},

	{	// GF_BGL_FRAME0_S	（した背景）
		0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
		0, 0, 0, FALSE
	},
};



//-------------------------------------
///	各画面	関数ポインタ
//=====================================
static void (*p_Init[ ANKETO_MOVE_TYPE_NUM ])( ANKETO_LOCAL_WK* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys, ANKETO_PARAM* p_param, u32 heapID ) = {
	ANKETO_LOCAL_INPUT_Init,
	ANKETO_LOCAL_OUTPUT_Init,
};
static BOOL (*p_Main[ ANKETO_MOVE_TYPE_NUM ])( ANKETO_LOCAL_WK* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys, ANKETO_TALKWIN* p_talkwin, ANKETO_PARAM* p_param, u32 heapID ) = {
	ANKETO_LOCAL_INPUT_Main,
	ANKETO_LOCAL_OUTPUT_Main,
};
static void (*p_Exit[ ANKETO_MOVE_TYPE_NUM ])( ANKETO_LOCAL_WK* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys ) = {
	ANKETO_LOCAL_INPUT_Exit,
	ANKETO_LOCAL_OUTPUT_Exit,
};



//----------------------------------------------------------------------------
/**
 *	@brief	アンケートプロセス	初期化
 */
//-----------------------------------------------------------------------------
PROC_RESULT ANKETO_Init(PROC* p_proc, int* p_seq)
{
	ANKETO_WORK*	p_wk;
	ANKETO_PARAM*	p_param;


	p_param = PROC_GetParentWork( p_proc );

	//ヒープエリア作成
	sys_CreateHeap( HEAPID_BASE_APP, HEAPID_ANKETO, 0x50000 );


	// ワーク作成
	p_wk = PROC_AllocWork( p_proc, sizeof(ANKETO_WORK), HEAPID_ANKETO );
	memset( p_wk, 0, sizeof(ANKETO_WORK) );


	// 描画システム初期化
	ANKETO_InitGraphic( &p_wk->drawsys, p_param->p_save, HEAPID_ANKETO );

	// メッセージシステム初期化
	ANKETO_MsgManInit( &p_wk->msgsys, p_param->p_system, HEAPID_ANKETO );

	// 会話ウィンドウ初期化
	ANKETO_TalkWin_Init( &p_wk->talkwin,  &p_wk->drawsys, p_param->p_save, HEAPID_ANKETO );

	// それぞれの画面の初期化
	GF_ASSERT( p_param->move_type < ANKETO_MOVE_TYPE_NUM );
	p_Init[ p_param->move_type ]( &p_wk->local_wk, &p_wk->msgsys, &p_wk->drawsys, p_param, HEAPID_ANKETO );

	// 割り込み設定
	sys_VBlankFuncChange( ANKETO_VBlank, p_wk );
	sys_HBlankIntrStop();	//HBlank割り込み停止

	return	PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	アンケートプロセス	メイン
 */
//-----------------------------------------------------------------------------
PROC_RESULT ANKETO_Main(PROC* p_proc, int* p_seq)
{
	ANKETO_WORK* p_wk;
	ANKETO_PARAM* p_param;
	BOOL result;

	p_wk	= PROC_GetWork( p_proc );
	p_param = PROC_GetParentWork( p_proc );

	switch( (*p_seq) ){
	case 0:
		WIPE_SYS_Start(WIPE_PATTERN_WMS, 
				WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
				WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_ANKETO );

		// BGMをフェードアウト
		WFLBY_SYSTEM_SetBGMVolumeDown( p_param->p_system, TRUE );

		(*p_seq) ++;
		break;

	case 1:
		result = WIPE_SYS_EndCheck();
		if( result == TRUE ){
			(*p_seq) ++;
		}
		break;

	case 2:

		// 終了チェック
		if( WIPE_SYS_EndCheck() == TRUE ){

			if( WFLBY_SYSTEM_Event_GetEndCM( p_param->p_system ) == TRUE ){
				WFLBY_SYSTEM_APLFLAG_SetForceEnd( p_param->p_system );
				(*p_seq) ++;
				break;
			}

			if( WFLBY_ERR_CheckError() == TRUE ){
				(*p_seq) ++;
				break;
			}
		}
		

		// それぞれのメイン関数
		result = p_Main[ p_param->move_type ]( &p_wk->local_wk, &p_wk->msgsys, &p_wk->drawsys, &p_wk->talkwin, p_param, HEAPID_ANKETO );

		if( result == TRUE ){
			(*p_seq) ++;
		}
		break;

	case 3:
		WIPE_SYS_Start(WIPE_PATTERN_WMS, 
				WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
				WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_ANKETO );
		(*p_seq) ++;
		break;

	case 4:
		result = WIPE_SYS_EndCheck();
		if( result == TRUE ){

			// BTS通信549バグの修正
			// TimeWaitIconの停止
			ANKETO_TalkWin_StopTimeWait( &p_wk->talkwin );
			return PROC_RES_FINISH;
		}
		break;
	}

	// 描画システム
	ANKETO_MainGraphic( &p_wk->drawsys );
	return PROC_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	アンケートプロセス	破棄
 */
//-----------------------------------------------------------------------------
PROC_RESULT ANKETO_Exit(PROC* p_proc, int* p_seq)
{
	ANKETO_WORK* p_wk;
	ANKETO_PARAM* p_param;

	p_wk	= PROC_GetWork( p_proc );
	p_param = PROC_GetParentWork( p_proc );

	// 割り込み設定
	sys_VBlankFuncChange( NULL, NULL );
	sys_HBlankIntrStop();	//HBlank割り込み停止


	// それぞれの破棄
	p_Exit[ p_param->move_type ]( &p_wk->local_wk, &p_wk->msgsys, &p_wk->drawsys );

	// 会話ウィンドウ破棄
	ANKETO_TalkWin_Exit( &p_wk->talkwin, HEAPID_ANKETO );

	// メッセージシステム破棄
	ANKETO_MsgManExit( &p_wk->msgsys );

	// グラフィックワーク破棄
	ANKETO_ExitGraphic( &p_wk->drawsys );

	//ワーク破棄
	PROC_FreeWork( p_proc );
	
	//ヒープ破棄
	sys_DeleteHeap( HEAPID_ANKETO );

	// SEストップ
	Snd_SeStopAll(0);

	return PROC_RES_FINISH;
}





//-----------------------------------------------------------------------------
/**
 *			ぷらいべーと関数
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief	アンケートVBlank処理
 *
 *	@param	p_work	ワーク
 */
//-----------------------------------------------------------------------------
static void ANKETO_VBlank( void* p_work )
{
	ANKETO_WORK* p_wk = p_work;
	
	ANKETO_VBlankGraphic( &p_wk->drawsys );
}



//----------------------------------------------------------------------------
/**
 *	@brief	グラフィック初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_save		セーブデータ
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void ANKETO_InitGraphic( ANKETO_DRAWSYS* p_wk, SAVEDATA* p_save, u32 heapID )
{
	CONFIG*			p_config;
	
	// コンフィグデータ取得
	p_config = SaveData_GetConfig( p_save );
	
	// アーカイブハンドル初期化
	p_wk->p_handle = ArchiveDataHandleOpen( ARC_WIFILOBBY_OTHER_GRA, heapID );

	// Vram転送マネージャ作成
	initVramTransferManagerHeap( ANKETO_VRAMTRANS_TASKNUM, heapID );

	// バンク設定
	GF_Disp_SetBank( &sc_ANKETO_BANK );

	// BG初期化
	ANKETO_BgInit( p_wk, p_config, heapID );	

	// OAM設定
	ANKETO_OamInit( p_wk, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	グラフィック破棄
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void ANKETO_ExitGraphic( ANKETO_DRAWSYS* p_wk )
{
	// アーカイブハンドル
	ArchiveDataHandleClose( p_wk->p_handle );

	// Vram転送マネージャ破棄
	DellVramTransferManager();
	
	// BG設定
	ANKETO_BgExit( p_wk );

	// OAM設定
	ANKETO_OamExit( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	グラフィックメイン処理
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void ANKETO_MainGraphic( ANKETO_DRAWSYS* p_wk )
{
	CLACT_Draw( p_wk->p_clactset );
}

//----------------------------------------------------------------------------
/**
 *	@brief	グラフィックVBlank処理
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void ANKETO_VBlankGraphic( ANKETO_DRAWSYS* p_wk )
{
    // BG書き換え
    GF_BGL_VBlankFunc( p_wk->p_bgl );

    // レンダラ共有OAMマネージャVram転送
    REND_OAMTrans();

	// Vram転送
	DoVramTransferManager();
}



//----------------------------------------------------------------------------
/**
 *	@brief	BG初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_config	コンフィグデータ
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void ANKETO_BgInit( ANKETO_DRAWSYS* p_wk, CONFIG* p_config, u32 heapID )
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

		for( i=0; i<ANKETO_BGCNT_NUM; i++ ){
			GF_BGL_BGControlSet( p_wk->p_bgl, 
					sc_ANKETO_BGCNT_FRM[i], &sc_ANKETO_BGCNT_DATA[i],
					GF_BGL_MODE_TEXT );
			GF_BGL_ClearCharSet( sc_ANKETO_BGCNT_FRM[i], 32, 0, heapID);
			GF_BGL_ScrClear( p_wk->p_bgl, sc_ANKETO_BGCNT_FRM[i] );
		}
	}

	// フォントカラー
    TalkFontPaletteLoad( PALTYPE_MAIN_BG, ANKETO_PLTT_MAIN_TALKFONT*0x20, heapID );

	// トークウィンドウ
	{
		u8 win_num = CONFIG_GetWindowType( p_config );
		TalkWinGraphicSet( p_wk->p_bgl, GF_BGL_FRAME1_M,
				ANKETO_TALKWIN_CGX, ANKETO_TALKWIN_PAL,
				win_num, heapID );
	}

	// システムウィンドウ
    MenuWinGraphicSet(
        p_wk->p_bgl, GF_BGL_FRAME1_M, 
		ANKETO_SYSTEMWIN_CGX, ANKETO_SYSTEMWIN_PAL, 0, heapID );


	// した画面に、Wi－Fiマークを出す
	{
		ArcUtil_BgCharSet(ARC_LOBBY_NEWS, NARC_lobby_news_wifi_mark_bg_NCGR, p_wk->p_bgl, GF_BGL_FRAME0_S, 0, 0, FALSE, heapID);
		ArcUtil_ScrnSet(ARC_LOBBY_NEWS, NARC_lobby_news_wifi_mark_bg_NSCR, p_wk->p_bgl, GF_BGL_FRAME0_S, 0, 0, FALSE, heapID);
		ArcUtil_PalSet( ARC_LOBBY_NEWS, NARC_lobby_news_lobby_news_bg_NCLR, PALTYPE_SUB_BG, 0, 0, heapID );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG破棄
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void ANKETO_BgExit( ANKETO_DRAWSYS* p_wk )
{
	// ＢＧコントロール破棄
	{
		int i;

		for( i=0; i<ANKETO_BGCNT_NUM; i++ ){
			GF_BGL_BGControlExit( p_wk->p_bgl, sc_ANKETO_BGCNT_FRM[i] );
		}
	}
	
	// BGL破棄
	sys_FreeMemoryEz( p_wk->p_bgl );
}

//----------------------------------------------------------------------------
/**
 *	@brief	OAMシステムの初期化
 *
 *	@param	p_wk		ワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void ANKETO_OamInit( ANKETO_DRAWSYS* p_wk, u32 heapID )
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
    InitCharManagerReg(&sc_ANKETO_CHARMAN_INIT, GX_OBJVRAMMODE_CHAR_1D_32K, GX_OBJVRAMMODE_CHAR_1D_32K );
    // パレットマネージャー初期化
    InitPlttManager(ANKETO_OAM_CONTNUM, heapID);

    // 読み込み開始位置を初期化
    CharLoadStartAll();
    PlttLoadStartAll();

    //通信アイコン用にキャラ＆パレット制限
    CLACT_U_WmIcon_SetReserveAreaCharManager(NNS_G2D_VRAM_TYPE_2DMAIN, GX_OBJVRAMMODE_CHAR_1D_32K);
    CLACT_U_WmIcon_SetReserveAreaPlttManager(NNS_G2D_VRAM_TYPE_2DMAIN);
    

    // セルアクターセット作成
    p_wk->p_clactset = CLACT_U_SetEasyInit( ANKETO_OAM_CONTNUM, &p_wk->renddata, heapID );

    // キャラとパレットのリソースマネージャ作成
    for( i=0; i<ANKETO_RESMAN_NUM; i++ ){
        p_wk->p_resman[i] = CLACT_U_ResManagerInit(ANKETO_OAM_CONTNUM, i, heapID);
    }

	// 下画面に通信アイコンを出す
	WirelessIconEasy();  // 接続中なのでアイコン表示


	// 表示開始
    GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
    GF_Disp_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
}

//----------------------------------------------------------------------------
/**
 *	@brief	OAMシステムの破棄
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void ANKETO_OamExit( ANKETO_DRAWSYS* p_wk )
{
    int i;

    // アクターの破棄
    CLACT_DestSet( p_wk->p_clactset );

    for( i=0; i<ANKETO_RESMAN_NUM; i++ ){
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
 *	@brief	メッセージ管理システム初期化
 *
 *	@param	p_wk		ワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void ANKETO_MsgManInit( ANKETO_MSGMAN* p_wk, WFLBY_SYSTEM* p_system, u32 heapID )
{
	int i;
	static const sc_ANKETO_MSGMANTBL[ ANKETO_MSGMAN_NUM ] = {
		NARC_msg_wifi_hiroba_dat,
		NARC_msg_wifi_50_questions_dat,
		NARC_msg_wifi_Special_questions_dat,
		NARC_msg_wifi_50_answers_a_dat,
		NARC_msg_wifi_50_answers_b_dat,
		NARC_msg_wifi_50_answers_c_dat,
		NARC_msg_wifi_Special_answers_a_dat,
		NARC_msg_wifi_Special_answers_b_dat,
		NARC_msg_wifi_Special_answers_c_dat,
	};

	p_wk->p_system = p_system;

	// メッセージマネージャ
	for( i=0; i<ANKETO_MSGMAN_NUM; i++ ){
		p_wk->p_msgman[i] = MSGMAN_Create(MSGMAN_TYPE_NORMAL,ARC_MSG,
				sc_ANKETO_MSGMANTBL[i],heapID );
	}

	// ワードセット
	p_wk->p_wordset = WORDSET_Create( heapID );

	// 共有メッセージ領域確保
	p_wk->p_msgstr = STRBUF_Create( ANKETO_MSGMAN_STRBUFNUM, heapID );
	p_wk->p_msgtmp = STRBUF_Create( ANKETO_MSGMAN_STRBUFNUM, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージ管理システム破棄
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void ANKETO_MsgManExit( ANKETO_MSGMAN* p_wk )
{
	int i;

	for( i=0; i<ANKETO_MSGMAN_NUM; i++ ){
		MSGMAN_Delete(p_wk->p_msgman[i]);
	}

	WORDSET_Delete( p_wk->p_wordset );

	// 共有メッセージバッファ
	STRBUF_Delete( p_wk->p_msgstr );
	STRBUF_Delete( p_wk->p_msgtmp );
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージの取得
 *
 *	@param	p_wk	ワーク
 *	@param	type	マネージャタイプ
 *	@param	msg		メッセージナンバー
 *
 *	@return	メッセージ
 */
//-----------------------------------------------------------------------------
static STRBUF* ANKETO_MsgManGetStr( ANKETO_MSGMAN* p_wk, u32 type, u32 msg )
{
	GF_ASSERT( type < ANKETO_MSGMAN_NUM );
	
	MSGMAN_GetString( p_wk->p_msgman[type], msg, p_wk->p_msgtmp );
	WORDSET_ExpandStr( p_wk->p_wordset, p_wk->p_msgstr, p_wk->p_msgtmp );
	return p_wk->p_msgstr;
}

//----------------------------------------------------------------------------
/**
 *	@brief	msgナンバーにあった質問を返す
 *
 *	@param	p_wk	ワーク
 *	@param	msg		メッセージナンバー（０～４９通常　５０～５９特別）
 *
 *	@return	メッセージ
 */
//-----------------------------------------------------------------------------
static STRBUF* ANKETO_MsgManGetQuestionStr( ANKETO_MSGMAN* p_wk, u32 msg )
{
	if( msg >= (ANKETO_NORMAL_Q_NUM+ANKETO_SPECIAL_Q_NUM) ){
		GF_ASSERT( msg >= (ANKETO_NORMAL_Q_NUM+ANKETO_SPECIAL_Q_NUM) );
		msg = 0;
	}
	
	if( msg < ANKETO_NORMAL_Q_NUM ){
		return ANKETO_MsgManGetStr( p_wk, ANKETO_MSGMAN_Q_NORMAL, msg );
	}
		
	return ANKETO_MsgManGetStr( p_wk, ANKETO_MSGMAN_Q_SPECIAL, msg-ANKETO_NORMAL_Q_NUM );
}

//----------------------------------------------------------------------------
/**
 *	@brief	msgナンバーにあった回答を返す
 *
 *	@param	p_wk	ワーク
 *	@param	msg		メッセージナンバー（０～４９通常　５０～５９特別）
 *	@param	answer	回答タイプ（ANKETO_ANSWER_A　・・・）
 *
 *	@return	回答
 */
//-----------------------------------------------------------------------------
static STRBUF* ANKETO_MsgManGetAnswerStr( ANKETO_MSGMAN* p_wk, u32 msg, u32 answer )
{
	if( msg >= (ANKETO_NORMAL_Q_NUM+ANKETO_SPECIAL_Q_NUM) ){
		GF_ASSERT( msg >= (ANKETO_NORMAL_Q_NUM+ANKETO_SPECIAL_Q_NUM) );
		msg = 0;
	}
	
	if( msg < ANKETO_NORMAL_Q_NUM ){
		return ANKETO_MsgManGetStr( p_wk, ANKETO_MSGMAN_A_N_A+answer, msg );
	}
		
	return ANKETO_MsgManGetStr( p_wk, ANKETO_MSGMAN_A_S_A+answer, msg - ANKETO_NORMAL_Q_NUM );
}

//----------------------------------------------------------------------------
/**
 *	@brief	自分の名前をワードセットに設定
 *
 *	@param	p_wk		ワーク
 *	@param	bufno		バッファID
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void ANKETO_MsgManSetMyName( ANKETO_MSGMAN* p_wk, u32 bufno, u32 heapID )
{
	MYSTATUS* p_mystatus;
	const WFLBY_USER_PROFILE* cp_profile;

	p_mystatus = MyStatus_AllocWork( heapID );
	cp_profile = WFLBY_SYSTEM_GetMyProfileLocal( p_wk->p_system );
	WFLBY_SYSTEM_GetProfileMyStatus( cp_profile, p_mystatus, heapID );

	// 名前を設定
	WORDSET_RegisterPlayerName( p_wk->p_wordset, bufno, p_mystatus );

	sys_FreeMemoryEz( p_mystatus );
}




//----------------------------------------------------------------------------
/**
 *	@brief	LOCAL_WKを共通ワークとしてインプットを管理する
 */
//-----------------------------------------------------------------------------
static void ANKETO_LOCAL_INPUT_Init( ANKETO_LOCAL_WK* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys, ANKETO_PARAM* p_param, u32 heapID )
{
	ANKETO_INPUT_Init( &p_wk->input, p_msg, p_drawsys, heapID );
}

static BOOL ANKETO_LOCAL_INPUT_Main( ANKETO_LOCAL_WK* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys, ANKETO_TALKWIN* p_talkwin, ANKETO_PARAM* p_param, u32 heapID )
{
	return ANKETO_INPUT_Main( &p_wk->input, p_msg, p_drawsys, p_talkwin, p_param, heapID );
}

static void ANKETO_LOCAL_INPUT_Exit( ANKETO_LOCAL_WK* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys )
{
	ANKETO_INPUT_Exit( &p_wk->input, p_msg, p_drawsys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	入力画面	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_msg		メッセージシステム
 *	@param	p_drawsys	描画システム
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void ANKETO_INPUT_Init( ANKETO_INPUT* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys, u32 heapID )
{
	// 背景設定
	{
		ArcUtil_HDL_PalSet( p_drawsys->p_handle, NARC_wifi_lobby_other_bg_NCLR, PALTYPE_MAIN_BG, ANKETO_PLTT_MAIN_BACK00, ANKETO_PLTT_MAINBACK_PLTTNUM*32, heapID );
		ArcUtil_HDL_BgCharSet( p_drawsys->p_handle, NARC_wifi_lobby_other_bg_NCGR, p_drawsys->p_bgl, GF_BGL_FRAME0_M, 0, 0, FALSE, heapID );
		ArcUtil_HDL_ScrnSet( p_drawsys->p_handle, NARC_wifi_lobby_other_bg00_NSCR, p_drawsys->p_bgl, GF_BGL_FRAME0_M, 0, 0, FALSE, heapID );
	}

	// OAM読み込み
	{
		BOOL result;

		// キャラクタ読み込み
		p_wk->p_cursor_res[0] = CLACT_U_ResManagerResAddArcChar_ArcHandle( 
				p_drawsys->p_resman[0], p_drawsys->p_handle,
				NARC_wifi_lobby_other_oam_NCGR,
				FALSE, ANKETO_INPUT_OAM_CONTID, NNS_G2D_VRAM_TYPE_2DMAIN, heapID );

		p_wk->p_cursor_res[1] = CLACT_U_ResManagerResAddArcPltt_ArcHandle( 
				p_drawsys->p_resman[1], p_drawsys->p_handle,
				NARC_wifi_lobby_other_oam_NCLR,
				FALSE, ANKETO_INPUT_OAM_CONTID, 
				NNS_G2D_VRAM_TYPE_2DMAIN, 3, heapID );
		result =CLACT_U_CharManagerSetAreaCont( p_wk->p_cursor_res[0] );
		GF_ASSERT( result );
		result =CLACT_U_PlttManagerSetCleanArea( p_wk->p_cursor_res[1] );
		GF_ASSERT( result );
		CLACT_U_ResManagerResOnlyDelete( p_wk->p_cursor_res[0] );
		CLACT_U_ResManagerResOnlyDelete( p_wk->p_cursor_res[1] );

		// セル
		p_wk->p_cursor_res[2] = CLACT_U_ResManagerResAddArcKindCell_ArcHandle( 
				p_drawsys->p_resman[2], p_drawsys->p_handle,
				NARC_wifi_lobby_other_oam_NCER,
				FALSE, ANKETO_INPUT_OAM_CONTID, CLACT_U_CELL_RES, heapID );

		// セルアニメ
		p_wk->p_cursor_res[3] = CLACT_U_ResManagerResAddArcKindCell_ArcHandle( 
				p_drawsys->p_resman[3], p_drawsys->p_handle,
				NARC_wifi_lobby_other_oam_NANR,
				FALSE, ANKETO_INPUT_OAM_CONTID, CLACT_U_CELLANM_RES, heapID );
	}

	// cursor作成
	{
		CLACT_HEADER		header;
		CLACT_ADD_SIMPLE	add = {0};

		// ヘッダー作成
		CLACT_U_MakeHeader( &header, ANKETO_INPUT_OAM_CONTID, 
				ANKETO_INPUT_OAM_CONTID, 
				ANKETO_INPUT_OAM_CONTID, 
				ANKETO_INPUT_OAM_CONTID, 
				CLACT_U_HEADER_DATA_NONE, CLACT_U_HEADER_DATA_NONE,
				0, ANKETO_INPUT_OAM_BGPRI,
				p_drawsys->p_resman[0],
				p_drawsys->p_resman[1],
				p_drawsys->p_resman[2],
				p_drawsys->p_resman[3],
				NULL, NULL );


		add.ClActSet	= p_drawsys->p_clactset;
		add.ClActHeader = &header;
		add.pri			= 0;
		add.DrawArea	= NNS_G2D_VRAM_TYPE_2DMAIN;
		add.heap		= heapID;

		// アクター作成
		p_wk->p_cursor_act = CLACT_AddSimple( &add );
		CLACT_SetDrawFlag( p_wk->p_cursor_act, FALSE );
	}
	
	// ウィンドウ作成
	{
		int i;

		for( i=0; i<ANKETO_INPUT_BMP_NUM; i++ ){
			GF_BGL_BmpWinAddEx( p_drawsys->p_bgl, &p_wk->win[i], 
					&sc_ANKETO_INPUT_BMPDAT[i] );
		}
	}

	ANKETO_QUESTION_DATA_Init( &p_wk->question_now, TRUE, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	入力画面メイン
 *
 *	@param	p_wk		ワーク
 *	@param	p_msg		メッセージシステム
 *	@param	p_drawsys	描画システム
 *	@param	heapID		ヒープID	＊＊＊渡すけど、INPUT_Exitがいつ呼ばれても大丈夫なようにすること
 */
//-----------------------------------------------------------------------------
static BOOL ANKETO_INPUT_Main( ANKETO_INPUT* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys, ANKETO_TALKWIN* p_talkwin, ANKETO_PARAM* p_param, u32 heapID )
{
	u32 msg_idx;
	STRBUF* p_str;
	
	switch( p_wk->seq ){
	case ANKETO_INPUT_SEQ_ANKETO_YARU_Q:		// アンケートやる？
		p_str = ANKETO_MsgManGetStr( p_msg, ANKETO_MSGMAN_MSG, msg_survey_monitor_b_n02 );
		ANKETO_TalkWin_Print( p_talkwin, p_str );

		p_wk->ret_seq = ANKETO_INPUT_SEQ_ANKETO_YARU_Q_ON;
		p_wk->seq = ANKETO_INPUT_SEQ_ANKETO_MASWAIT;
		break;

	case ANKETO_INPUT_SEQ_ANKETO_YARU_Q_ON:// アンケートやる？
		ANKETO_TalkWin_StartYesNo( p_talkwin, p_drawsys, heapID );
		p_wk->seq = ANKETO_INPUT_SEQ_ANKETO_YARU_Q_WAIT;
		break;

	case ANKETO_INPUT_SEQ_ANKETO_YARU_Q_WAIT:// アンケートやる？
		{
			u32 result;

			result = ANKETO_TalkWin_MainYesNo( p_talkwin, heapID );
			switch( result ){
			// はい
			case 0:
				// 選択へ
				p_wk->seq = ANKETO_INPUT_SEQ_ANKETO_Q_DRAW00;
				break;

			// のー
			case BMPMENU_CANCEL:
				// 終了へ
				p_wk->seq = ANKETO_INPUT_SEQ_ANKETO_END;
				break;
			}
		}
		break;


#if 0
	case ANKETO_INPUT_SEQ_ANKETO_RECV:		// 受信開始

		Snd_SePlay( ANKETO_SND_RECV_WAIT );

//		ANKETO_QUESTION_DATA_DebugInit( &p_wk->question_now );

		// 受信メッセージ
		p_str = ANKETO_MsgManGetStr( p_msg, ANKETO_MSGMAN_MSG, msg_survey_monitor_a_02_01 );
		ANKETO_TalkWin_PrintAll( p_talkwin, p_str );

		// タイムウエイト表示
		ANKETO_TalkWin_StartTimeWait( p_talkwin );

		p_wk->seq = ANKETO_INPUT_SEQ_ANKETO_RECVWAIT;
		break;

	case ANKETO_INPUT_SEQ_ANKETO_RECVWAIT:	// 受信中

		if( (gf_mtRand() % 100) == 0 ){

			Snd_SePlay( ANKETO_SND_RECV );

			ANKETO_TalkWin_StopTimeWait( p_talkwin );


			p_str = ANKETO_MsgManGetStr( p_msg, ANKETO_MSGMAN_MSG, msg_survey_monitor_a_02_02 );
			ANKETO_TalkWin_Print( p_talkwin, p_str );

			p_wk->ret_seq = ANKETO_INPUT_SEQ_ANKETO_Q_DRAW00;
			p_wk->seq = ANKETO_INPUT_SEQ_ANKETO_MASWAIT;
		}
		break;
#endif

	case ANKETO_INPUT_SEQ_ANKETO_Q_DRAW00:	// 質問描画
		if( p_wk->question_now.special == FALSE ){
			msg_idx = msg_survey_monitor_b_n01;
		}else{
			msg_idx = msg_survey_monitor_b_s01;
		}
		p_str = ANKETO_MsgManGetStr( p_msg, ANKETO_MSGMAN_MSG, msg_idx );
		ANKETO_TalkWin_Print( p_talkwin, p_str );

		p_wk->ret_seq = ANKETO_INPUT_SEQ_ANKETO_Q_DRAW;
		p_wk->seq = ANKETO_INPUT_SEQ_ANKETO_MASWAIT;
		break;

	case ANKETO_INPUT_SEQ_ANKETO_Q_DRAW:		// 質問描画
		// 回答内容を表示
		ANKETO_INPUT_DrawAnswer( p_wk, p_msg, p_drawsys );

		// 質問を表示
		p_str = ANKETO_QUESTION_DATA_GetQuestionStr( &p_wk->question_now, p_msg );
		ANKETO_TalkWin_PrintAll( p_talkwin, p_str );

		// OAMを表示
		ANKETO_INPUT_DrawCursor( p_wk );

		// OAMアニメ
		ANKETO_INPUT_AnmCursor( p_wk );
		
		p_wk->seq = ANKETO_INPUT_SEQ_ANKETO_Q_SELECT;
		break;

	case ANKETO_INPUT_SEQ_ANKETO_Q_SELECT:	// 回答選択

		// Aで選択
		if( sys.trg & PAD_BUTTON_DECIDE ){
			Snd_SePlay( ANKETO_SND_SELECT );

			p_wk->seq ++;
			break;
		}
		
		if( sys.trg & PAD_KEY_UP ){
			if( (p_wk->cursor - 1) >= 0 ){
				Snd_SePlay( ANKETO_SND_CURSOR );
				p_wk->cursor --;
			}
		}else if( sys.trg & PAD_KEY_DOWN ){
			if( (p_wk->cursor + 1) < ANKETO_ANSWER_NUM ){
				Snd_SePlay( ANKETO_SND_CURSOR );
				p_wk->cursor ++;
			}
		}

		ANKETO_INPUT_DrawCursor( p_wk );
		break;

	case ANKETO_INPUT_SEQ_ANKETO_SEND:		// 受信開始

		// 基本情報に選択項目を設定
		WFLBY_SYSTEM_SetAnketoData( p_param->p_system, 
				p_wk->question_now.anketo_no, p_wk->cursor );

		// サーバに送信
		DWC_LOBBY_ANKETO_SubMit( p_wk->cursor );

		// 受信メッセージ
		p_str = ANKETO_MsgManGetStr( p_msg, ANKETO_MSGMAN_MSG, msg_survey_monitor_b_02_01 );
		ANKETO_TalkWin_PrintAll( p_talkwin, p_str );

		// タイムウエイト表示
		ANKETO_TalkWin_StartTimeWait( p_talkwin );

		Snd_SePlay( ANKETO_SND_SEND_WAIT );

		p_wk->seq = ANKETO_INPUT_SEQ_ANKETO_SENDWAIT;
		break;

	case ANKETO_INPUT_SEQ_ANKETO_SENDWAIT:	// 受信中
		{
			u32 send_result;
			send_result = DWC_LOBBY_ANKETO_WaitSubMit();
			if( send_result != DWC_LOBBY_ANKETO_STATE_SENDING ){

				Snd_SePlay( ANKETO_SND_SEND );

				ANKETO_TalkWin_StopTimeWait( p_talkwin );


				p_str = ANKETO_MsgManGetStr( p_msg, ANKETO_MSGMAN_MSG, msg_survey_monitor_b_02_02 );
				ANKETO_TalkWin_Print( p_talkwin, p_str );

				p_wk->ret_seq = ANKETO_INPUT_SEQ_ANKETO_ARIGATO;
				p_wk->seq = ANKETO_INPUT_SEQ_ANKETO_MASWAIT;
			}
		}
		break;

	case ANKETO_INPUT_SEQ_ANKETO_ARIGATO:	// ありがとうメッセージ描画
		if( p_wk->question_now.special == FALSE ){
			msg_idx = msg_survey_monitor_b_n03;
		}else{
			msg_idx = msg_survey_monitor_b_s03;
		}
		p_str = ANKETO_MsgManGetStr( p_msg, ANKETO_MSGMAN_MSG, msg_idx );
		ANKETO_TalkWin_Print( p_talkwin, p_str );

		p_wk->ret_seq = ANKETO_INPUT_SEQ_ANKETO_END;
		p_wk->seq = ANKETO_INPUT_SEQ_ANKETO_MASWAIT;
		break;

	case ANKETO_INPUT_SEQ_ANKETO_END:		// 終了
		// 会話ウィンドウ非表示
		ANKETO_TalkWin_Off( p_talkwin );
		return TRUE;

	case ANKETO_INPUT_SEQ_ANKETO_MASWAIT:	// メッセージ待ち
		if( ANKETO_TalkWin_EndWait( p_talkwin ) == TRUE ){
			p_wk->seq = p_wk->ret_seq;
		}
		break;

	default:
		GF_ASSERT(0);
		break;

	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	入力画面	破棄
 *
 *	@param	p_wk		ワーク
 *	@param	p_msg		メッセージシステム
 *	@param	p_drawsys	描画システム
 */
//-----------------------------------------------------------------------------
static void ANKETO_INPUT_Exit( ANKETO_INPUT* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys )
{
	// 質問データ
	ANKETO_QUESTION_DATA_Exit( &p_wk->question_now );
	
	// ウィンドウ破棄
	{
		int i;

		for( i=0; i<ANKETO_INPUT_BMP_NUM; i++ ){
			GF_BGL_BmpWinDel( &p_wk->win[i] );
		}
	}


	// cursor破棄
	{
		CLACT_Delete( p_wk->p_cursor_act );
	}

	// OAM破棄
	{
		int i;
		
		// VRAM領域破棄
		CLACT_U_CharManagerDelete( p_wk->p_cursor_res[0] );
		CLACT_U_PlttManagerDelete( p_wk->p_cursor_res[1] );

		for( i=0; i<ANKETO_RESMAN_NUM; i++ ){
			CLACT_U_ResManagerResDelete( p_drawsys->p_resman[i], p_wk->p_cursor_res[i] );
		
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	回答を描画する
 *
 *	@param	p_wk		ワーク
 *	@param	p_msg		メッセージワーク
 *	@param	p_drawsys	描画システム
 */
//-----------------------------------------------------------------------------
static void ANKETO_INPUT_DrawAnswer( ANKETO_INPUT* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys )
{
	STRBUF* p_str;

	GF_BGL_BmpWinDataFill( &p_wk->win[ANKETO_INPUT_BMP_ANS], 0 );

	p_str = ANKETO_QUESTION_DATA_GetAnswerStr( &p_wk->question_now, p_msg, ANKETO_ANSWER_A );
	GF_STR_PrintColor( &p_wk->win[ANKETO_INPUT_BMP_ANS], 
			FONT_TALK, p_str, 0, ANKETO_INPUTWIN_ANS00_Y,
			MSG_NO_PUT, ANKETO_INPUT_ANSWER_COL, NULL );

	p_str = ANKETO_QUESTION_DATA_GetAnswerStr( &p_wk->question_now, p_msg, ANKETO_ANSWER_B );
	GF_STR_PrintColor( &p_wk->win[ANKETO_INPUT_BMP_ANS], 
			FONT_TALK, p_str, 0, ANKETO_INPUTWIN_ANS01_Y,
			MSG_NO_PUT, ANKETO_INPUT_ANSWER_COL, NULL );

	p_str = ANKETO_QUESTION_DATA_GetAnswerStr( &p_wk->question_now, p_msg, ANKETO_ANSWER_C );
	GF_STR_PrintColor( &p_wk->win[ANKETO_INPUT_BMP_ANS], 
			FONT_TALK, p_str, 0, ANKETO_INPUTWIN_ANS02_Y,
			MSG_NO_PUT, ANKETO_INPUT_ANSWER_COL, NULL );

	GF_BGL_BmpWinOnVReq( &p_wk->win[ANKETO_INPUT_BMP_ANS] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	cursor描画
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void ANKETO_INPUT_DrawCursor( ANKETO_INPUT* p_wk )
{
	CLACT_SetDrawFlag( p_wk->p_cursor_act, TRUE );

	CLACT_SetMatrix( p_wk->p_cursor_act, &sc_ANKETO_INPUT_CURSOR_MAT[ p_wk->cursor ] );
}
static void ANKETO_INPUT_AnmCursor( ANKETO_INPUT* p_wk )
{
	CLACT_SetAnmFlag( p_wk->p_cursor_act, TRUE );
}



//----------------------------------------------------------------------------
/**
 *	@brief	会話ウィンドウシステム	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		描画システム
 *	@param	p_save		セーブデータ
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void ANKETO_TalkWin_Init( ANKETO_TALKWIN* p_wk, ANKETO_DRAWSYS* p_sys, SAVEDATA* p_save, u32 heapID )
{
	//  ビットマップ確保
	GF_BGL_BmpWinAdd(
				p_sys->p_bgl, &p_wk->win, 
				GF_BGL_FRAME1_M,
				ANKETO_TALK_TALKWIN_X, ANKETO_TALK_TALKWIN_Y,
				ANKETO_TALK_TALKWIN_SIZX, ANKETO_TALK_TALKWIN_SIZY,
				ANKETO_PLTT_MAIN_TALKFONT,
				ANKETO_TALK_TALKWIN_CGX );

	// クリーン
	GF_BGL_BmpWinDataFill( &p_wk->win, 15 );

	// 文字列バッファ作成
	p_wk->p_str = STRBUF_Create( ANKETO_MSGMAN_STRBUFNUM, heapID );

	// メッセージ表示ウエイトを設定
	{
		CONFIG* p_config;
		p_config = SaveData_GetConfig( p_save );
		p_wk->msgwait = CONFIG_GetMsgPrintSpeed( p_config );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	会話ウィンドウ	破棄
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void ANKETO_TalkWin_Exit( ANKETO_TALKWIN* p_wk, u32 heapID )
{
	//  すべて停止
	if( GF_MSG_PrintEndCheck( p_wk->msgno ) ){
		GF_STR_PrintForceStop( p_wk->msgno );
	}
	ANKETO_TalkWin_StopTimeWait( p_wk );
	
	// 文字列バッファ破棄
	STRBUF_Delete( p_wk->p_str );

	// ビットマップ破棄
	GF_BGL_BmpWinDel( &p_wk->win );

	// YESNOウィンドウ破棄
	ANKETO_TalkWin_EndYesNo( p_wk, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	会話開始
 *
 *	@param	p_wk		ワーク
 *	@param	cp_str		文字
 */
//-----------------------------------------------------------------------------
static void ANKETO_TalkWin_Print( ANKETO_TALKWIN* p_wk, const STRBUF* cp_str )
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
			p_wk->msgwait, ANKETO_TALK_COL, NULL );

	// ウィンドウを書き込む
	BmpTalkWinWrite( &p_wk->win, WINDOW_TRANS_OFF, ANKETO_TALKWIN_CGX, ANKETO_TALKWIN_PAL );
}

//----------------------------------------------------------------------------
/**
 *	@brief	会話開始	全表示
 *
 *	@param	p_wk		ワーク
 *	@param	cp_str		文字列
 */
//-----------------------------------------------------------------------------
static void ANKETO_TalkWin_PrintAll( ANKETO_TALKWIN* p_wk, const STRBUF* cp_str )
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
			MSG_NO_PUT, ANKETO_TALK_COL, NULL );

	// ウィンドウを書き込む
	BmpTalkWinWrite( &p_wk->win, WINDOW_TRANS_OFF, ANKETO_TALKWIN_CGX, ANKETO_TALKWIN_PAL );
	GF_BGL_BmpWinOnVReq( &p_wk->win );
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイムアウトウエイト	開始
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void ANKETO_TalkWin_StartTimeWait( ANKETO_TALKWIN* p_wk )
{
	GF_ASSERT( p_wk->p_timewait == NULL );
	
	p_wk->p_timewait = TimeWaitIconAdd( &p_wk->win, ANKETO_TALKWIN_CGX );
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイムアウトウエイト	終了
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void ANKETO_TalkWin_StopTimeWait( ANKETO_TALKWIN* p_wk )
{
	if( p_wk->p_timewait != NULL ){
		TimeWaitIconDel( p_wk->p_timewait );
		p_wk->p_timewait = NULL;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイムウエイト	チェック
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	タイムウエイト中
 *	@retval	FALSE	タイムウエイト表示してない
 */
//-----------------------------------------------------------------------------
static BOOL ANKETO_TalkWin_CheckTimeWait( const ANKETO_TALKWIN* cp_wk )
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
static BOOL ANKETO_TalkWin_EndWait( const ANKETO_TALKWIN* cp_wk )
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
static void ANKETO_TalkWin_Off( ANKETO_TALKWIN* p_wk )
{
	// メッセージ表示中なら消す
	if( GF_MSG_PrintEndCheck( p_wk->msgno ) ){
		GF_STR_PrintForceStop( p_wk->msgno );
	}

	// タイムウエイトも消す
	ANKETO_TalkWin_StopTimeWait( p_wk );

	// 全体を消す
	BmpTalkWinClear( &p_wk->win, WINDOW_TRANS_OFF );

	// Vリクエスト
	GF_BGL_BmpWinOffVReq( &p_wk->win );
}

//----------------------------------------------------------------------------
/**
 *	@brief	YesNoウィンドウ	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		描画システム
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void ANKETO_TalkWin_StartYesNo( ANKETO_TALKWIN* p_wk, ANKETO_DRAWSYS* p_sys, u32 heapID )
{
	GF_ASSERT( p_wk->p_yesno == NULL );
	p_wk->p_yesno = BmpYesNoSelectInit( p_sys->p_bgl, &sc_YESNO_BMPDAT, 
			ANKETO_SYSTEMWIN_CGX, ANKETO_SYSTEMWIN_PAL, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	YesNoウィンドウ	メイン
 *
 *	@param	p_wk	ワーク
 *
 * @retval	"BMPMENU_NULL	選択されていない"
 * @retval	"0				はいを選択"
 * @retval	"BMPMENU_CANCEL	いいえorキャンセル"
 */
//-----------------------------------------------------------------------------
static u32	ANKETO_TalkWin_MainYesNo( ANKETO_TALKWIN* p_wk, u32 heapID )
{
	u32 ret;
	ret = BmpYesNoSelectMain( p_wk->p_yesno, heapID );
	if( ret != BMPMENU_NULL ){
		p_wk->p_yesno = NULL;
	}
	return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	YesNoウィンドウ破棄
 *
 *	@param	p_wk	ワーク
 *	@param	heapID	ヒープID
 */
//-----------------------------------------------------------------------------
static void ANKETO_TalkWin_EndYesNo( ANKETO_TALKWIN* p_wk, u32 heapID )
{
	if( p_wk->p_yesno != NULL ){
		BmpYesNoWinDel( p_wk->p_yesno, heapID );
		p_wk->p_yesno = NULL;
	}
}



//----------------------------------------------------------------------------
/**
 *	@brief	ローカルワークを使用して出力画面を表示
 */
//-----------------------------------------------------------------------------
static void ANKETO_LOCAL_OUTPUT_Init( ANKETO_LOCAL_WK* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys, ANKETO_PARAM* p_param, u32 heapID )
{
	ANKETO_OUTPUT_Init( &p_wk->output, p_msg, p_drawsys, p_param, heapID );
}

static BOOL ANKETO_LOCAL_OUTPUT_Main( ANKETO_LOCAL_WK* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys, ANKETO_TALKWIN* p_talkwin, ANKETO_PARAM* p_param, u32 heapID )
{
	return ANKETO_OUTPUT_Main( &p_wk->output, p_msg, p_drawsys, p_talkwin, p_param, heapID );
}

static void ANKETO_LOCAL_OUTPUT_Exit( ANKETO_LOCAL_WK* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys )
{
	ANKETO_OUTPUT_Exit( &p_wk->output, p_msg, p_drawsys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	出力画面	初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_msg		メッセージワーク
 *	@param	p_drawsys	描画システム
 *	@param	p_param		パラメータ
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void ANKETO_OUTPUT_Init( ANKETO_OUTPUT* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys, ANKETO_PARAM* p_param, u32 heapID )
{
	// 背景設定
	{
		ArcUtil_HDL_PalSet( p_drawsys->p_handle, NARC_wifi_lobby_other_bg_NCLR, PALTYPE_MAIN_BG, ANKETO_PLTT_MAIN_BACK00, ANKETO_PLTT_MAINBACK_PLTTNUM*32, heapID );
		ArcUtil_HDL_BgCharSet( p_drawsys->p_handle, NARC_wifi_lobby_other_bg_NCGR, p_drawsys->p_bgl, GF_BGL_FRAME0_M, 0, 0, FALSE, heapID );
		ArcUtil_HDL_ScrnSet( p_drawsys->p_handle, NARC_wifi_lobby_other_bg01_NSCR, p_drawsys->p_bgl, GF_BGL_FRAME0_M, 0, 0, FALSE, heapID );
	}

	// ウィンドウ作成
	{
		int i;

		for( i=0; i<ANKETO_OUTPUT_BMP_NUM; i++ ){
			GF_BGL_BmpWinAddEx( p_drawsys->p_bgl, &p_wk->win[i], 
					&sc_ANKETO_OUTPUT_BMPDAT[i] );
		}
	}

	// タイトル
	ANKETO_OUTPUT_DrawTitle( p_wk, p_msg, msg_survey_monitor_a_04_header, ANKETO_PLTT_TITLEFONT_COL_OUTPUT00 );

	// bar用のキャラクタ読み込み
	{
		p_wk->p_charres = ArcUtil_HDL_CharDataGet( p_drawsys->p_handle, NARC_wifi_lobby_other_bg_NCGR, FALSE, &p_wk->p_char, heapID );
	}

	// アンケート内容と結果を受信
	{
		ANKETO_QUESTION_DATA_Init( &p_wk->now, TRUE, heapID );
		ANKETO_QUESTION_DATA_Init( &p_wk->last, FALSE, heapID );

		// 基本情報からアンケート結果を集計
		// もう集計してるなら、そのデータを使用する
		if( WFLBY_SYSTEM_GetAnketoResult( p_param->p_system, &p_wk->result_now ) == FALSE ){
			ANKETO_QUESTION_RESULT_CalcHirobaResult( &p_wk->result_now, p_param->p_system );
			WFLBY_SYSTEM_SetAnketoResult( p_param->p_system, &p_wk->result_now );	// 集計データを設定
		}

		ANKETO_QUESTION_RESULT_Init( &p_wk->result_last );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	出力画面	メイン処理
 *
 *	@param	p_wk		ワーク
 *	@param	p_msg		メッセージワーク
 *	@param	p_drawsys	描画システム
 *	@param	p_talkwin	会話ウィンドウ
 *	@param	heapID		ヒープID
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
static BOOL ANKETO_OUTPUT_Main( ANKETO_OUTPUT* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys, ANKETO_TALKWIN* p_talkwin, ANKETO_PARAM* p_param, u32 heapID )
{
	STRBUF* p_str;
	u32 msg_idx;
	
	switch( p_wk->seq ){
#if 0
	case ANKETO_OUTPUT_SEQ_ANKETO_RECV:			// アンケート受信
		// 受信関連のメッセージ表示
		p_str = ANKETO_MsgManGetStr( p_msg, ANKETO_MSGMAN_MSG, msg_survey_monitor_a_02_01 );
		ANKETO_TalkWin_PrintAll( p_talkwin, p_str );


		// タイムウエイト表示
		ANKETO_TalkWin_StartTimeWait( p_talkwin );

		Snd_SePlay( ANKETO_SND_RECV_WAIT );

		p_wk->seq ++;
		break;

	case ANKETO_OUTPUT_SEQ_ANKETO_RECVWAIT:		// アンケート受信中

		// 受信完了
		if( (gf_mtRand() % 100) == 0 ){

			Snd_SePlay( ANKETO_SND_RECV );

			// アンケート内容と結果を受信
			{
				ANKETO_QUESTION_DATA_DebugInit( &p_wk->now );
				ANKETO_QUESTION_DATA_DebugInit( &p_wk->last );

				// 基本情報からアンケート結果を集計
				// もう集計してるなら、そのデータを使用する
				if( WFLBY_SYSTEM_GetAnketoResult( p_param->p_system, &p_wk->result_now ) == FALSE ){
					ANKETO_QUESTION_RESULT_CalcHirobaResult( &p_wk->result_now, p_param->p_system, &p_wk->now );
					WFLBY_SYSTEM_SetAnketoResult( p_param->p_system, &p_wk->result_now );	// 集計データを設定
				}
		
				ANKETO_QUESTION_RESULT_DebugInit( &p_wk->result_last );
			}
			
			ANKETO_TalkWin_StopTimeWait( p_talkwin );


			p_str = ANKETO_MsgManGetStr( p_msg, ANKETO_MSGMAN_MSG, msg_survey_monitor_a_02_02 );
			ANKETO_TalkWin_Print( p_talkwin, p_str );

			p_wk->ret_seq = ANKETO_OUTPUT_SEQ_ANKETO_NQ_WRITE00;
			p_wk->seq = ANKETO_OUTPUT_SEQ_ANKETO_MSGWAIT;
		}
		break;
#endif

	case ANKETO_OUTPUT_SEQ_ANKETO_NQ_WRITE00:	// こんかいは
		if( p_wk->now.special == FALSE ){
			msg_idx = msg_survey_monitor_a_n03_01;
		}else{
			msg_idx = msg_survey_monitor_a_s03_01;
		}
		p_str = ANKETO_MsgManGetStr( p_msg, ANKETO_MSGMAN_MSG, msg_idx );
		ANKETO_TalkWin_Print( p_talkwin, p_str );

		p_wk->ret_seq = ANKETO_OUTPUT_SEQ_ANKETO_NQ_WRITE01;
		p_wk->seq = ANKETO_OUTPUT_SEQ_ANKETO_MSGWAIT;
		break;

	case ANKETO_OUTPUT_SEQ_ANKETO_NQ_WRITE01:	// 内容
		p_str = ANKETO_QUESTION_DATA_GetQuestionStr( &p_wk->now, p_msg );
		ANKETO_TalkWin_Print( p_talkwin, p_str );

		p_wk->ret_seq = ANKETO_OUTPUT_SEQ_ANKETO_NQ_WRITE02;
		p_wk->seq = ANKETO_OUTPUT_SEQ_ANKETO_MSGWAIT;
		break;

	case ANKETO_OUTPUT_SEQ_ANKETO_NQ_WRITE02:	// でした
		if( sys.trg & PAD_BUTTON_DECIDE ){

			p_str = ANKETO_MsgManGetStr( p_msg, ANKETO_MSGMAN_MSG, msg_survey_monitor_a_n03_02 );
			ANKETO_TalkWin_Print( p_talkwin, p_str );

			p_wk->ret_seq = ANKETO_OUTPUT_SEQ_ANKETO_NQ_WRITE03;
			p_wk->seq = ANKETO_OUTPUT_SEQ_ANKETO_MSGWAIT;
		}
		break;

	case ANKETO_OUTPUT_SEQ_ANKETO_NQ_WRITE03:	// こんかいはこんなでした
		if( p_wk->now.special == FALSE ){
			msg_idx = msg_survey_monitor_a_n04;
		}else{
			msg_idx = msg_survey_monitor_a_s04;
		}
		p_str = ANKETO_MsgManGetStr( p_msg, ANKETO_MSGMAN_MSG, msg_idx );
		ANKETO_TalkWin_Print( p_talkwin, p_str );

		p_wk->ret_seq = ANKETO_OUTPUT_SEQ_ANKETO_NRESULT;
		p_wk->seq = ANKETO_OUTPUT_SEQ_ANKETO_MSGWAIT;
		break;

	case ANKETO_OUTPUT_SEQ_ANKETO_NRESULT:		// 結果表示
		{
			WFLBY_ANKETO anketo_data;
			const WFLBY_USER_PROFILE* cp_profile;
			BOOL vip;

			cp_profile = WFLBY_SYSTEM_GetMyProfileLocal( p_param->p_system );
			WFLBY_SYSTEM_GetProfileAnketoData( cp_profile, &anketo_data );

			vip = WFLBY_SYSTEM_GetUserVipFlag( p_param->p_system, WFLBY_SYSTEM_GetMyIdx( p_param->p_system ) );
		
			// 回答内容を表示する
			ANKETO_OUTPUT_DrawAnswer( p_wk, &p_wk->now, p_msg, p_drawsys, 
					&anketo_data, heapID, ANKETO_PLTT_FONT_COL_OUTPUT00, ANKETO_PLTT_FONT_COL_VIP00, vip );

			// バー表示開始
			ANKETO_OUTPUT_DrawBarStart( p_wk, &p_wk->result_now, p_drawsys, ANKETO_PLTT_MAIN_BACK01 );

			Snd_SePlay( ANKETO_SND_BAR );
		}

		p_wk->seq = ANKETO_OUTPUT_SEQ_ANKETO_NRESULT_MAIN;
		break;

	case ANKETO_OUTPUT_SEQ_ANKETO_NRESULT_MAIN:	// 結果表示
		{
			BOOL result;
			result = ANKETO_OUTPUT_DrawBarMain( p_wk, p_drawsys );
			if( result == TRUE ){
				// SEをとめる
				//Snd_SeStopBySeqNo( ANKETO_SND_BAR, 0 );
				
				p_wk->seq = ANKETO_OUTPUT_SEQ_ANKETO_NA_WAIT;
			}
		}
		break;

	case ANKETO_OUTPUT_SEQ_ANKETO_NA_WAIT:		// Aボタン待ち

		if( sys.trg & PAD_BUTTON_DECIDE ){

			// 先週の結果はある？
			if( p_wk->last.question_no != DWC_LOBBY_ANKETO_NOT_QUESTION ){
				p_wk->seq = ANKETO_OUTPUT_SEQ_ANKETO_YESNO_DRAW;
			}else{
				// 終了メッセージ後終わる
				p_str = ANKETO_MsgManGetStr( p_msg, ANKETO_MSGMAN_MSG, msg_survey_monitor_a_06_00 );
				ANKETO_TalkWin_Print( p_talkwin, p_str );

				p_wk->ret_seq = ANKETO_OUTPUT_SEQ_ANKETO_ENDMSG;
				p_wk->seq = ANKETO_OUTPUT_SEQ_ANKETO_MSGWAIT;
			}
		}
		break;

	case ANKETO_OUTPUT_SEQ_ANKETO_YESNO_DRAW:	// YESNO表示
		p_str = ANKETO_MsgManGetStr( p_msg, ANKETO_MSGMAN_MSG, msg_survey_monitor_a_05 );
		ANKETO_TalkWin_PrintAll( p_talkwin, p_str );
		ANKETO_TalkWin_StartYesNo( p_talkwin, p_drawsys, heapID );

		p_wk->seq = ANKETO_OUTPUT_SEQ_ANKETO_YESNO_WAIT;
		break;

	case ANKETO_OUTPUT_SEQ_ANKETO_YESNO_WAIT:	// YESNO待ち
		{
			u32 result;
			result = ANKETO_TalkWin_MainYesNo( p_talkwin, heapID );
			switch( result ){
			// はい
			case 0:
				// 先週表示へ
				p_wk->seq = ANKETO_OUTPUT_SEQ_ANKETO_LQ_WIPEOUT;
				break;

			// のー
			case BMPMENU_CANCEL:
				// 終了へ
				p_wk->seq = ANKETO_OUTPUT_SEQ_ANKETO_ENDMSG;
				break;
			}
		}
		break;

	case ANKETO_OUTPUT_SEQ_ANKETO_LQ_WIPEOUT:	// 一回フェード
		WIPE_SYS_Start(WIPE_PATTERN_WMS, 
				WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
				WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, heapID );

		p_wk->seq = ANKETO_OUTPUT_SEQ_ANKETO_LQ_WIPEOUTWAIT;
		break;

	case ANKETO_OUTPUT_SEQ_ANKETO_LQ_WIPEOUTWAIT:// 一回フェード
		if( WIPE_SYS_EndCheck() == TRUE ){
			// 画面を切り替える
			ANKETO_OUTPUT_SetLastWeekGraphic( p_wk, p_msg, p_drawsys, heapID );

			p_wk->seq = ANKETO_OUTPUT_SEQ_ANKETO_LQ_WIPEIN;
		}
		break;

	case ANKETO_OUTPUT_SEQ_ANKETO_LQ_WIPEIN:		// 一回フェード
		WIPE_SYS_Start(WIPE_PATTERN_WMS, 
				WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
				WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, heapID );

		p_wk->seq = ANKETO_OUTPUT_SEQ_ANKETO_LQ_WIPEINWAIT;
		break;

	case ANKETO_OUTPUT_SEQ_ANKETO_LQ_WIPEINWAIT:	// 一回フェード
		if( WIPE_SYS_EndCheck() == TRUE ){
			p_wk->seq = ANKETO_OUTPUT_SEQ_ANKETO_LQ_WRITE00;
		}
		break;


	case ANKETO_OUTPUT_SEQ_ANKETO_LQ_WRITE00:	// 前回は
		if( p_wk->last.special == FALSE ){
			msg_idx = msg_survey_monitor_a_n06_01;
		}else{
			msg_idx = msg_survey_monitor_a_s06_01;
		}
		p_str = ANKETO_MsgManGetStr( p_msg, ANKETO_MSGMAN_MSG, msg_idx );
		ANKETO_TalkWin_Print( p_talkwin, p_str );

		p_wk->ret_seq = ANKETO_OUTPUT_SEQ_ANKETO_LQ_WRITE01;
		p_wk->seq = ANKETO_OUTPUT_SEQ_ANKETO_MSGWAIT;
		break;

	case ANKETO_OUTPUT_SEQ_ANKETO_LQ_WRITE01:	// 内容
		p_str = ANKETO_QUESTION_DATA_GetQuestionStr( &p_wk->last, p_msg );
		ANKETO_TalkWin_Print( p_talkwin, p_str );

		p_wk->ret_seq = ANKETO_OUTPUT_SEQ_ANKETO_LQ_WRITE02;
		p_wk->seq = ANKETO_OUTPUT_SEQ_ANKETO_MSGWAIT;
		break;

	case ANKETO_OUTPUT_SEQ_ANKETO_LQ_WRITE02:	// でした
		if( sys.trg & PAD_BUTTON_DECIDE ){

			p_str = ANKETO_MsgManGetStr( p_msg, ANKETO_MSGMAN_MSG, msg_survey_monitor_a_n06_02 );
			ANKETO_TalkWin_Print( p_talkwin, p_str );

			p_wk->ret_seq = ANKETO_OUTPUT_SEQ_ANKETO_LQ_WRITE03;
			p_wk->seq = ANKETO_OUTPUT_SEQ_ANKETO_MSGWAIT;
		}
		break;

	case ANKETO_OUTPUT_SEQ_ANKETO_LQ_WRITE03:	// 前回はこんなでした
		if( p_wk->last.special == FALSE ){
			msg_idx = msg_survey_monitor_a_n07;
		}else{
			msg_idx = msg_survey_monitor_a_s07;
		}
		p_str = ANKETO_MsgManGetStr( p_msg, ANKETO_MSGMAN_MSG, msg_idx );
		ANKETO_TalkWin_Print( p_talkwin, p_str );

		p_wk->ret_seq = ANKETO_OUTPUT_SEQ_ANKETO_LRESULT;
		p_wk->seq = ANKETO_OUTPUT_SEQ_ANKETO_MSGWAIT;
		break;

	case ANKETO_OUTPUT_SEQ_ANKETO_LRESULT:		// 結果表示
		{
			WFLBY_ANKETO anketo;
			WFLBY_SAVEDATA* p_hirobasave;
			BOOL vip;

			// セーブデータから１つ前に答えたアンケートデータ取得
			p_hirobasave		= SAVEDATA_WFLBY_GetWork( p_param->p_save );
			anketo.anketo_no	= SAVEDATA_WFLBY_GetQuestionNumber( p_hirobasave );
			anketo.select		= SAVEDATA_WFLBY_GetMyAnswer( p_hirobasave );

			vip = WFLBY_SYSTEM_GetUserVipFlag( p_param->p_system, WFLBY_SYSTEM_GetMyIdx( p_param->p_system ) );
		
			// 回答内容を表示する
			ANKETO_OUTPUT_DrawAnswer( p_wk, &p_wk->last, p_msg, p_drawsys, &anketo, heapID, 
					ANKETO_PLTT_FONT_COL_OUTPUT01, ANKETO_PLTT_FONT_COL_VIP01, vip );

			// バー表示開始
			ANKETO_OUTPUT_DrawBarStart( p_wk, &p_wk->result_last, p_drawsys, ANKETO_PLTT_MAIN_BACK02 );

			Snd_SePlay( ANKETO_SND_BAR );
		}
		p_wk->seq = ANKETO_OUTPUT_SEQ_ANKETO_LRESULT_MAIN;
		break;

	case ANKETO_OUTPUT_SEQ_ANKETO_LRESULT_MAIN:	// 結果表示
		{
			BOOL result;
			result = ANKETO_OUTPUT_DrawBarMain( p_wk, p_drawsys );
			if( result == TRUE ){

				// SEをとめる
				//Snd_SeStopBySeqNo( ANKETO_SND_BAR, 0 );
				p_wk->seq = ANKETO_OUTPUT_SEQ_ANKETO_LA_WAIT;
			}
		}
		break;

	case ANKETO_OUTPUT_SEQ_ANKETO_LA_WAIT:		// Aボタン待ち
		if( sys.trg & PAD_BUTTON_DECIDE ){
			// 終了へ
			p_wk->seq = ANKETO_OUTPUT_SEQ_ANKETO_ENDMSG;
		}
		break;

	case ANKETO_OUTPUT_SEQ_ANKETO_ENDMSG:		// 終了メッセージ
		p_str = ANKETO_MsgManGetStr( p_msg, ANKETO_MSGMAN_MSG, msg_survey_monitor_a_08 );
		ANKETO_TalkWin_Print( p_talkwin, p_str );

		p_wk->ret_seq = ANKETO_OUTPUT_SEQ_ANKETO_END;
		p_wk->seq = ANKETO_OUTPUT_SEQ_ANKETO_MSGWAIT;
		break;

	case ANKETO_OUTPUT_SEQ_ANKETO_END:			// 終了
		// 会話ウィンドウ非表示
		ANKETO_TalkWin_Off( p_talkwin );
		return TRUE;	

	case ANKETO_OUTPUT_SEQ_ANKETO_MSGWAIT:		// メッセージ待ち
		if( ANKETO_TalkWin_EndWait( p_talkwin ) == TRUE ){
			p_wk->seq = p_wk->ret_seq;
		}
		break;

	default:
		GF_ASSERT(0);
		break;
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	出力画面	破棄
 *
 *	@param	p_wk		ワーク
 *	@param	p_msg		メッセージワーク
 *	@param	p_drawsys	描画システム
 */
//-----------------------------------------------------------------------------
static void ANKETO_OUTPUT_Exit( ANKETO_OUTPUT* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys )
{
	// データ破棄
	{
		ANKETO_QUESTION_DATA_Exit( &p_wk->now );
		ANKETO_QUESTION_DATA_Exit( &p_wk->last );
	}
	
	// キャラクタデータ破棄
	sys_FreeMemoryEz( p_wk->p_charres );
	p_wk->p_charres = NULL;
	
	// ウィンドウ破棄
	{
		int i;

		for( i=0; i<ANKETO_OUTPUT_BMP_NUM; i++ ){
			GF_BGL_BmpWinDel( &p_wk->win[i] );
		}
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	タイトルの描画
 *
 *	@param	p_wk		ワーク
 *	@param	p_msg		メッセージ
 *	@param	msgidx		メッセージIDX
 */
//-----------------------------------------------------------------------------
static void ANKETO_OUTPUT_DrawTitle( ANKETO_OUTPUT* p_wk, ANKETO_MSGMAN* p_msg, u32 msgidx, GF_PRINTCOLOR col )
{
	STRBUF* p_str;
	u32 width;
	s32 x;
	

	GF_BGL_BmpWinDataFill( &p_wk->win[ANKETO_OUTPUT_BMP_TITLE], 0 );

	p_str = ANKETO_MsgManGetStr( p_msg, ANKETO_MSGMAN_MSG, msgidx );

	// センタリング
	width = FontProc_GetPrintStrWidth( FONT_TALK, p_str, 0 );
	x = ((ANKETO_OUTPUTWIN_TITLE_SIZX*8) - width) / 2;
	GF_STR_PrintColor( &p_wk->win[ANKETO_OUTPUT_BMP_TITLE], 
			FONT_TALK, p_str, x, 4,
			MSG_NO_PUT, col, NULL );

	GF_BGL_BmpWinOnVReq( &p_wk->win[ANKETO_OUTPUT_BMP_TITLE] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	回答を表示する
 *
 *	@param	p_wk		ワーク
 *	@param	cp_data		データ
 *	@param	p_msg		メッセージ
 *	@param	p_drawsys	描画システム
 *	@param	cp_answer	自分の答え
 *	@param	heapID
 *	@param	col			表示カラー
 *	@param	vip			VIP
 */
//-----------------------------------------------------------------------------
static void ANKETO_OUTPUT_DrawAnswer( ANKETO_OUTPUT* p_wk, const ANKETO_QUESTION_DATA* cp_data, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys, const WFLBY_ANKETO* cp_answer, u32 heapID, GF_PRINTCOLOR col, GF_PRINTCOLOR vipcol, BOOL vip )
{
	STRBUF* p_str;

	GF_BGL_BmpWinDataFill( &p_wk->win[ANKETO_OUTPUT_BMP_ANS], 0 );

	GF_BGL_BmpWinDataFill( &p_wk->win[ANKETO_OUTPUT_BMP_ANS_MY], 0 );

	p_str = ANKETO_QUESTION_DATA_GetAnswerStr( cp_data, p_msg, ANKETO_ANSWER_A );
	GF_STR_PrintColor( &p_wk->win[ANKETO_OUTPUT_BMP_ANS], 
			FONT_TALK, p_str, 0, ANKETO_OUTPUTWIN_ANS00_Y,
			MSG_NO_PUT, col, NULL );

	p_str = ANKETO_QUESTION_DATA_GetAnswerStr( cp_data, p_msg, ANKETO_ANSWER_B );
	GF_STR_PrintColor( &p_wk->win[ANKETO_OUTPUT_BMP_ANS], 
			FONT_TALK, p_str, 0, ANKETO_OUTPUTWIN_ANS01_Y,
			MSG_NO_PUT, col, NULL );

	p_str = ANKETO_QUESTION_DATA_GetAnswerStr( cp_data, p_msg, ANKETO_ANSWER_C );
	GF_STR_PrintColor( &p_wk->win[ANKETO_OUTPUT_BMP_ANS], 
			FONT_TALK, p_str, 0, ANKETO_OUTPUTWIN_ANS02_Y,
			MSG_NO_PUT, col, NULL );

	// 自分の答え
	if( (cp_answer->anketo_no == cp_data->anketo_no) && (cp_answer->select < ANKETO_ANSWER_NUM) ){	// 通し番号が一致するかチェック
		s32 str_width;
		s32 write_x;
		GF_PRINTCOLOR mycol;

		if( vip == TRUE ){
			mycol = vipcol;	
		}else{
			mycol = col;	
		}
		
		ANKETO_MsgManSetMyName( p_msg, 0, heapID );
		p_str = ANKETO_MsgManGetStr( p_msg, ANKETO_MSGMAN_MSG, msg_survey_monitor_a_04_pselect );
		str_width	= FontProc_GetPrintStrWidth( FONT_TALK, p_str, 0 );
		write_x		= ((ANKETO_OUTPUTWIN_MY_NAME_WRITE_SIZE_X - str_width) / 2);
		
		GF_STR_PrintColor( &p_wk->win[ANKETO_OUTPUT_BMP_ANS_MY], 
				FONT_TALK, p_str, write_x, 0,
				MSG_NO_PUT, mycol, NULL );

		// 答えの文字列を書き込む
		p_str = ANKETO_QUESTION_DATA_GetAnswerStr( cp_data, p_msg, cp_answer->select );
		str_width	= FontProc_GetPrintStrWidth( FONT_TALK, p_str, 0 );
		write_x		= ((ANKETO_OUTPUTWIN_MY_ANSWER_WRITE_SIZE_X - str_width) / 2);
		
		GF_STR_PrintColor( &p_wk->win[ANKETO_OUTPUT_BMP_ANS_MY], 
				FONT_TALK, p_str, ANKETO_OUTPUTWIN_MY_ANSWER_X+write_x, 0,
				MSG_NO_PUT, col, NULL );
	}
	

	GF_BGL_BmpWinOnVReq( &p_wk->win[ANKETO_OUTPUT_BMP_ANS_MY] );
	GF_BGL_BmpWinOnVReq( &p_wk->win[ANKETO_OUTPUT_BMP_ANS] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	バー表示開始
 *
 *	@param	p_wk			ワーク
 *	@param	cp_data			データ
 *	@param	p_drawsys		描画システム
 */
//-----------------------------------------------------------------------------
static void ANKETO_OUTPUT_DrawBarStart( ANKETO_OUTPUT* p_wk, const ANKETO_QUESTION_RESULT* cp_data, ANKETO_DRAWSYS* p_drawsys, u32 palno )
{
	int i;
	
	p_wk->count = 0;
	for( i=0; i<ANKETO_ANSWER_NUM; i++ ){
		p_wk->draw_width[ i ] = 0;
		p_wk->drawend_width[ i ] = ANKETO_QUESTION_RESULT_GetBerWidth( cp_data, i, ANKETO_BAR_100WIDTH ); 
		GF_BGL_BmpWinSet_Pal( &p_wk->win[ ANKETO_OUTPUT_BMP_ANS_BAR00+i ], palno );

		GF_BGL_BmpWinDataFill( &p_wk->win[ ANKETO_OUTPUT_BMP_ANS_BAR00+i ], 0 );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	バー表示メイン
 *
 *	@param	p_wk		ワーク
 *	@param	p_drawsys	描画システム
 *
 *	@retval	TRUE	バー表示終了
 *	@retval	FLASE	バー表示中
 */
//-----------------------------------------------------------------------------
static BOOL ANKETO_OUTPUT_DrawBarMain( ANKETO_OUTPUT* p_wk, ANKETO_DRAWSYS* p_drawsys )
{
	s32 draw_width;
	s32 draw_start;
	s32 width;
	int i, j;
	int skip_count;

	// 最後まで書いてたらオワリ
	if( p_wk->count >= ANKETO_BAR_COUNTMAX ){
		return TRUE;
	}
	
	// カウントの表示長さを求める
	p_wk->count ++;
	draw_width = (p_wk->count * ANKETO_BAR_100WIDTH) / ANKETO_BAR_COUNTMAX;
	
	// 描画しないといけない幅がまだ
	skip_count = 0;
	for( i=0; i<ANKETO_ANSWER_NUM; i++ ){

		// 終わってるかチェック
		if( p_wk->draw_width[i] == p_wk->drawend_width[i] ){
			skip_count ++;
			continue;
		}

		// 自分がその長さより大きかったら無条件に伸ばす
		if( p_wk->drawend_width[ i ] >= draw_width ){
			// そのまま描画
			width		= draw_width - p_wk->draw_width[ i ];
			draw_start	= p_wk->draw_width[ i ];
			p_wk->draw_width[ i ] = draw_width;

		// あとは、残り部分を描画する
		}else{
			width		= p_wk->drawend_width[ i ] - p_wk->draw_width[ i ];
			draw_start	= p_wk->draw_width[ i ];
			p_wk->draw_width[ i ] = p_wk->drawend_width[ i ];
		}

		// 描画
		for( j=0; j<width; j++ ){
			GF_BGL_BmpWinPrint( &p_wk->win[ ANKETO_OUTPUT_BMP_ANS_BAR00+i ], p_wk->p_char->pRawData,
					ANKETO_BAR_CG_X, ANKETO_BAR_CG_Y, 
					ANKETO_BAR_CGFILE_X, ANKETO_BAR_CGFILE_Y,
					ANKETO_BAR_WRITE_X+draw_start+j, 0,
					ANKETO_BAR_CG_SIZX, ANKETO_BAR_CG_SIZY );
		}
		GF_BGL_BmpWinOnVReq( &p_wk->win[ ANKETO_OUTPUT_BMP_ANS_BAR00+i ] );
	}

	// 全部終わってたら終わり
	if( skip_count == ANKETO_ANSWER_NUM ){
		return TRUE;
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	先週のアンケート結果表示グラフィックの表示
 *
 *	@param	p_wk		ワーク
 *	@param	p_msg		メッセージ
 *	@param	p_drawsys	描画システム
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void ANKETO_OUTPUT_SetLastWeekGraphic( ANKETO_OUTPUT* p_wk, ANKETO_MSGMAN* p_msg, ANKETO_DRAWSYS* p_drawsys, u32 heapID )
{
	// 背景設定
	{
		ArcUtil_HDL_ScrnSet( p_drawsys->p_handle, NARC_wifi_lobby_other_bg02_NSCR, p_drawsys->p_bgl, GF_BGL_FRAME0_M, 0, 0, FALSE, heapID );
	}

	// ウィンドウ作成
	{
		int i;

		for( i=0; i<ANKETO_OUTPUT_BMP_NUM; i++ ){
			GF_BGL_BmpWinDataFill( &p_wk->win[i], 0 );
			GF_BGL_BmpWinOn( &p_wk->win[i] );
		}
	}

	// タイトル
	ANKETO_OUTPUT_DrawTitle( p_wk, p_msg, msg_survey_monitor_a_07_header, ANKETO_PLTT_TITLEFONT_COL_OUTPUT01 );
}




//----------------------------------------------------------------------------
/**
 *	@brief	質問内容ワーク	デバック初期化
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void ANKETO_QUESTION_DATA_DebugInit( ANKETO_QUESTION_DATA* p_wk )
{
	p_wk->special			= gf_mtRand() % 2;
	p_wk->question_no		= gf_mtRand() % (ANKETO_NORMAL_Q_NUM+ANKETO_SPECIAL_Q_NUM);
	p_wk->anketo_no			= 0;
	p_wk->make_question		= 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	質問内容ワーク	初期化
 *
 *	@param	p_wk	ワーク
 *	@param	now		今の質問か
 *	@param	heapID	ヒープID
 */
//-----------------------------------------------------------------------------
static void ANKETO_QUESTION_DATA_Init( ANKETO_QUESTION_DATA* p_wk, BOOL now, u32 heapID )
{
	u32 data_start;
	u32 msg_start;
	u16* p_str;
	int i;
	
	// メッセージバッファ作成
	p_wk->p_question = STRBUF_Create( ANKETO_MESSAGE_BUFFNUM, heapID );
	for( i=0; i<ANKETO_ANSWER_NUM; i++ ){
		p_wk->p_ans[i] = STRBUF_Create( ANKETO_MESSAGE_BUFFNUM, heapID );
	}
	
	if( now == TRUE ){
		data_start	= DWC_LOBBY_ANKETO_DATA_NOW_QUESTION_SERIAL;
		msg_start	= DWC_LOBBY_ANKETO_MESSAGE_NOW_QUESTION;
	}else{
		data_start	= DWC_LOBBY_ANKETO_DATA_LAST_QUESTION_SERIAL;
		msg_start	= DWC_LOBBY_ANKETO_MESSAGE_LAST_QUESTION;
	}

	p_wk->special		= DWC_LOBBY_ANKETO_GetData( data_start+DWC_LOBBY_ANKETO_DATA_NOW_QUESTION_SPECIAL );
	p_wk->question_no	= DWC_LOBBY_ANKETO_GetData( data_start+DWC_LOBBY_ANKETO_DATA_NOW_QUESTION_NO );
	p_wk->anketo_no		= DWC_LOBBY_ANKETO_GetData( data_start+DWC_LOBBY_ANKETO_DATA_NOW_QUESTION_SERIAL );
	p_wk->make_question	= DWC_LOBBY_ANKETO_GetData( data_start+DWC_LOBBY_ANKETO_DATA_NOW_QUESTION_NINI );

	if( p_wk->make_question == TRUE ){
		// 質問と回答案を取得
		p_str = DWC_LOBBY_ANKETO_GetMessage( msg_start+DWC_LOBBY_ANKETO_MESSAGE_NOW_QUESTION );
		GF_ASSERT( p_str );
		STRBUF_SetStringCode( p_wk->p_question, p_str );
		for( i=0; i<ANKETO_ANSWER_NUM; i++ ){
			p_str = DWC_LOBBY_ANKETO_GetMessage( msg_start+DWC_LOBBY_ANKETO_MESSAGE_NOW_ANSWER_00+i );
			GF_ASSERT( p_str );
			STRBUF_SetStringCode( p_wk->p_ans[i], p_str );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	アンケートデータの破棄
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void ANKETO_QUESTION_DATA_Exit( ANKETO_QUESTION_DATA* p_wk )
{
	int i;
	
	// メッセージバッファ破棄
	STRBUF_Delete( p_wk->p_question );
	for( i=0; i<ANKETO_ANSWER_NUM; i++ ){
		STRBUF_Delete( p_wk->p_ans[i] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	質問内容ワーク　質問メッセージ取得
 *
 *	@param	p_wk	質問ワーク
 *	@param	p_msg	メッセージワーク
 */
//-----------------------------------------------------------------------------
static STRBUF* ANKETO_QUESTION_DATA_GetQuestionStr( const ANKETO_QUESTION_DATA* cp_wk, ANKETO_MSGMAN* p_msg )
{
	if( cp_wk->make_question == FALSE ){
		return ANKETO_MsgManGetQuestionStr( p_msg, cp_wk->question_no );
	}
	return cp_wk->p_question;
}

//----------------------------------------------------------------------------
/**
 *	@brief	質問内容ワーク	回答メッセージ取得
 *
 *	@param	p_wk		ワーク
 *	@param	p_msg		メッセージワーク
 *	@param	answer		回答タイプ（ANKETO_ANSWER_A　・・・）
 */
//-----------------------------------------------------------------------------
static STRBUF* ANKETO_QUESTION_DATA_GetAnswerStr( const ANKETO_QUESTION_DATA* cp_wk, ANKETO_MSGMAN* p_msg, u32 answer )
{
	if( cp_wk->make_question == FALSE ){
		return ANKETO_MsgManGetAnswerStr( p_msg, cp_wk->question_no, answer );
	}
	return cp_wk->p_ans[ answer ];
}  

//----------------------------------------------------------------------------
/**
 *	@brief	結果ワーク	デバック初期化
 */
//-----------------------------------------------------------------------------
static void ANKETO_QUESTION_RESULT_DebugInit( ANKETO_QUESTION_RESULT* p_wk )
{
	int i;
	
	p_wk->ans_all = 0;
	for( i=0; i<ANKETO_ANSWER_NUM; i++ ){
		p_wk->ans[i] = gf_mtRand() % 0xffffffff;
		p_wk->ans_all += p_wk->ans[i];
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	前回の結果を求める
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static void ANKETO_QUESTION_RESULT_Init( ANKETO_QUESTION_RESULT* p_wk )
{
	int i;

	p_wk->ans_all = 0;
	for( i=0; i<ANKETO_ANSWER_NUM; i++ ){
		p_wk->ans[i] = DWC_LOBBY_ANKETO_GetData( DWC_LOBBY_ANKETO_DATA_LAST_RESULT_A+i );
		p_wk->ans_all += p_wk->ans[i];
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	バーの長さを求める
 *
 *	@param	cp_wk		ワーク
 *	@param	answer		結果ナンバー
 *	@param	max_width	最大の長さ
 *
 *	@return	長さ
 */
//-----------------------------------------------------------------------------
static u32 ANKETO_QUESTION_RESULT_GetBerWidth( const ANKETO_QUESTION_RESULT* cp_wk, u32 answer, u32 max_width )
{
	u64 max_width_64, answer_64;
	GF_ASSERT( answer < ANKETO_ANSWER_NUM );

	max_width_64	= max_width;
	answer_64		= cp_wk->ans[ answer ];
	
	return (u32)(((u64)(max_width_64 * answer_64)) / cp_wk->ans_all);
}

//----------------------------------------------------------------------------
/**
 *	@brief	今いるひろば内の結果を集計
 *
 *	@param	p_wk		ワーク
 *	@param	cp_system	システム
 */
//-----------------------------------------------------------------------------
static void ANKETO_QUESTION_RESULT_CalcHirobaResult( ANKETO_QUESTION_RESULT* p_wk, const WFLBY_SYSTEM* cp_system )
{
	const WFLBY_USER_PROFILE* cp_profile;
	WFLBY_ANKETO anketo;
	int i;
	u32 lang;
	BOOL sum;

	memset( p_wk, 0, sizeof(ANKETO_QUESTION_RESULT) );

	for( i=0; i<WFLBY_PLAYER_MAX; i++ ){
		cp_profile = WFLBY_SYSTEM_GetUserProfile( cp_system, i );
		if( cp_profile != NULL ){
			lang = WFLBY_SYSTEM_GetProfileRagionCodeOrg( cp_profile );
			WFLBY_SYSTEM_GetProfileAnketoData( cp_profile, &anketo );
			sum = DWC_LOBBY_ANKETO_GetLanguageSummarize( DWC_LOBBY_ANKETO_LANGUAGE_NOW, lang );
			if( (sum == TRUE) && (anketo.select < ANKETO_ANSWER_NUM) ){
				p_wk->ans[ anketo.select ] ++;
				p_wk->ans_all ++;
			}
		}
	}
}


