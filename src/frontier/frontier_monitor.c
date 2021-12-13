//==============================================================================================
/**
 * @file	frontier_monitor.c
 * @brief	「バトルフロンティア」モニター
 * @author	Satoshi Nohara
 * @date	2007.12.6
 *
 * typeは0=SINGLEと考えています
 */
//==============================================================================================
#include "common.h"
#include "system/main.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "system/clact_tool.h"
#include "system/brightness.h"
#include "system/wipe.h"
#include "system/snd_tool.h"
#include "system/pm_str.h"
#include "system/buflen.h"
#include "system/wordset.h"
#include "system/bmp_menu.h"
#include "system/window.h"
#include "system/fontproc.h"

#include "savedata/config.h"
#include "savedata/frontier_savedata.h"
#include "savedata/frontier_ex.h"

#include "savedata/factory_savedata.h"
#include "savedata/stage_savedata.h"
#include "savedata/castle_savedata.h"
#include "savedata/roulette_savedata.h"
#include "savedata/b_tower.h"

#include "msgdata/msg.naix"					//NARC_msg_??_dat
#include "msgdata/msg_bf_seiseki.h"			//msg_??

#include "../field/fieldsys.h"				//scr_tool.hに必要
#include "../field/scr_tool.h"				//FactoryScr_GetWinRecordID
#include "../field/poketch/poketch_arc.h"	//ポケッチにあるボールの絵を使いまわし

#include "application/frontier_monitor.h"
#include "frontier_monitor_bmp.h"
#include "frontier_def.h"
#include "graphic/frontier_obj_def.h"
#include "graphic/frontier_bg_def.h"

#include "factory_def.h"
#include "stage_def.h"
#include "castle_def.h"
#include "roulette_def.h"


//==============================================================================================
//
//	定義
//
//==============================================================================================
//シーケンス定義
enum {
	SEQ_GAME_INIT,											//初期化
	SEQ_GAME_START,											//ゲーム開始
	SEQ_GAME_END,											//終了
};

//表示位置
enum{
	BF_STR_X	= (1*8),									//バトル○(施設名)

	TITLE_STR_SINGLE_X_TOWER	= (27*8),					//シングルせいせき
	TITLE_STR_SINGLE_X_FACTORY	= (27*8),					//シングルせいせき
	TITLE_STR_SINGLE_X_STAGE	= (27*8),					//シングルせいせき
	TITLE_STR_SINGLE_X_CASTLE	= (27*8),					//シングルせいせき
	TITLE_STR_SINGLE_X_ROULETTE	= (27*8),					//シングルせいせき

	TITLE_STR_X_TOWER	= (27*8),							//ダブル、マルチせいせき
	TITLE_STR_X_FACTORY = (27*8),							//ダブル、マルチせいせき
	TITLE_STR_X_STAGE	= (27*8),							//ダブル、マルチせいせき
	TITLE_STR_X_CASTLE	= (27*8),							//ダブル、マルチせいせき
	TITLE_STR_X_ROULETTE= (27*8),							//ダブル、マルチせいせき

	WIN_NUM_X	= (14*8),									//連勝数を書く位置
	CP_STR_X	= (22*8),
	TRD_NUM_X	= (28*8),
	//TRD_STR_X	= (27*8),
};

//Horizontal alignment
enum {
    FTRM_ALN_LEFT,
    FTRM_ALN_CENTER,
    FTRM_ALN_RIGHT,
};


//==============================================================================================
//
//	メッセージ関連の定義
//
//==============================================================================================
#define FRONTIER_MSG_BUF_SIZE		(800)//(1024)			//メッセージサイズ


//==============================================================================================
//
//	構造体宣言
//
//==============================================================================================
typedef struct{
	PROC* proc;											//PROCへのポインタ

	u8	sub_seq;										//シーケンス
	u8	type;											//シングル、ダブル、マルチ、wifiマルチ
	u8	fr_no;											//施設ナンバー
	u8	dummy;

	u16 monsno;
	u16 dummy2;

	GF_BGL_INI*	bgl;									//BGLへのポインタ
	//GF_BGL_BMPWIN bmpwin[FRONTIER_MONITOR_BMPWIN_MAX];	//BMPウィンドウデータ
	GF_BGL_BMPWIN bmpwin[16];	//BMPウィンドウデータ

	MSGDATA_MANAGER* msgman;							//メッセージマネージャー
	WORDSET* wordset;									//単語セット
	STRBUF* msg_buf;									//メッセージバッファポインタ
	STRBUF* tmp_buf;									//テンポラリバッファポインタ

	PALETTE_FADE_PTR pfd;								//パレットフェード

	const CONFIG* config;								//コンフィグポインタ
	SAVEDATA* sv;										//セーブデータポインタ
	FRONTIER_SAVEWORK* f_sv;
}FRONTIER_MONITOR_WORK;


//==============================================================================================
//
//	プロトタイプ宣言
//
//==============================================================================================
//PROC
PROC_RESULT FrontierMonitorProc_Init( PROC * proc, int * seq );
PROC_RESULT FrontierMonitorProc_Main( PROC * proc, int * seq );
PROC_RESULT FrontierMonitorProc_End( PROC * proc, int * seq );

//シーケンス
static BOOL Seq_GameInit( FRONTIER_MONITOR_WORK* wk );
static BOOL Seq_GameStart( FRONTIER_MONITOR_WORK* wk );
static BOOL Seq_GameEnd( FRONTIER_MONITOR_WORK* wk );

//設定
static void VBlankFunc( void * work );
static void SetVramBank(void);
static void SetBgHeader( GF_BGL_INI * ini );

//共通初期化、終了
static void Frontier_BgInit( FRONTIER_MONITOR_WORK* wk );
static void Frontier_BgExit( GF_BGL_INI * ini );

//ツール
static void NextSeq( FRONTIER_MONITOR_WORK* wk, int* seq, int next );
static int KeyCheck( int key );
static void BgCheck( void );
static u8 GetBmpNo( u8 fr_no );

//BGグラフィックデータ
static void Frontier_SetMainBgGraphic( FRONTIER_MONITOR_WORK * wk, u32 frm  );
static void Frontier_SetMainBgPalette( void );
static void Frontier_SetSubBgGraphic( FRONTIER_MONITOR_WORK * wk, u32 frm  );

//メッセージ関連
// MatchComment: FrontierWriteMsg has 10th (0-indexed) argument
static u8 FrontierWriteMsg( FRONTIER_MONITOR_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u8 f_col, u8 s_col, u8 b_col, u8 font, u8 fill, int align );
static u8 EasyMsg( FRONTIER_MONITOR_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u8 fill );
static void SetNumber( FRONTIER_MONITOR_WORK* wk, u32 bufID, s32 number );
static void Frontier_Write( FRONTIER_MONITOR_WORK* wk );

//ファクトリー
static void Factory_SetLv50Info( FRONTIER_MONITOR_WORK* wk );
static void Factory_SetLv100Info( FRONTIER_MONITOR_WORK* wk );
static u32 Factory_GetMsg( FRONTIER_MONITOR_WORK* wk, u8 level );

//ステージ
static void Stage_SetInfo( FRONTIER_MONITOR_WORK* wk );
static u32 Stage_GetMsg( FRONTIER_MONITOR_WORK* wk );
static void FrontierMonitor_PokeName( FRONTIER_MONITOR_WORK* wk );

//キャッスル
static void Castle_SetInfo( FRONTIER_MONITOR_WORK* wk );
static u32 Castle_GetMsg( FRONTIER_MONITOR_WORK* wk );

//ルーレット
static void Roulette_SetInfo( FRONTIER_MONITOR_WORK* wk );
static u32 Roulette_GetMsg( FRONTIER_MONITOR_WORK* wk );

//タワー
static void Tower_SetInfoSingleDouble( FRONTIER_MONITOR_WORK* wk );
static void Tower_SetInfoMulti( FRONTIER_MONITOR_WORK* wk );
static u32 Tower_GetMsg( FRONTIER_MONITOR_WORK* wk, u8 type );


//==============================================================================================
//
//	PROC
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	プロセス関数：初期化
 *
 * @param	proc	プロセスデータ
 * @param	seq		シーケンス
 *
 * @return	"処理状況"
 */
//--------------------------------------------------------------
PROC_RESULT FrontierMonitorProc_Init( PROC * proc, int * seq )
{
	int i;
	FRONTIER_MONITOR_WORK* wk;
	FRONTIER_MONITOR_CALL* bfm_call;

	sys_VBlankFuncChange( NULL, NULL );					//VBlankセット
	sys_HBlankIntrSet( NULL,NULL );						//HBlankセット
	GF_Disp_GX_VisibleControlInit();
	GF_Disp_GXS_VisibleControlInit();
	GX_SetVisiblePlane( 0 );
	GXS_SetVisiblePlane( 0 );

	sys_CreateHeap( HEAPID_BASE_APP, HEAPID_FRONTIER_MONITOR, 0x20000 );

	wk = PROC_AllocWork( proc, sizeof(FRONTIER_MONITOR_WORK), HEAPID_FRONTIER_MONITOR );
	memset( wk, 0, sizeof(FRONTIER_MONITOR_WORK) );

	wk->proc	= proc;
	wk->bgl		= GF_BGL_BglIniAlloc( HEAPID_FRONTIER_MONITOR );
	bfm_call	= (FRONTIER_MONITOR_CALL*)PROC_GetParentWork( proc );
	wk->sv		= bfm_call->sv;
	wk->f_sv	= SaveData_GetFrontier( wk->sv );
	wk->type	= bfm_call->type;
	wk->fr_no	= bfm_call->fr_no;
	wk->monsno	= bfm_call->monsno;
	wk->config	= SaveData_GetConfig( wk->sv );			//コンフィグポインタを取得

	Frontier_BgInit( wk );								//BG初期化

	//メッセージデータマネージャー作成
	wk->msgman = MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, 
								NARC_msg_bf_seiseki_dat, HEAPID_FRONTIER_MONITOR );

	wk->wordset = WORDSET_Create( HEAPID_FRONTIER_MONITOR );
	wk->msg_buf = STRBUF_Create( FRONTIER_MSG_BUF_SIZE, HEAPID_FRONTIER_MONITOR );
	wk->tmp_buf = STRBUF_Create( FRONTIER_MSG_BUF_SIZE, HEAPID_FRONTIER_MONITOR );

	//フォントパレット
	SystemFontPaletteLoad( PALTYPE_MAIN_BG, BFM_FONT_PAL * 32, HEAPID_FRONTIER_MONITOR );
	TalkFontPaletteLoad( PALTYPE_MAIN_BG, BFM_MSGFONT_PAL * 32, HEAPID_FRONTIER_MONITOR );

	//ビットマップ追加
	FrontierMonitorAddBmpWin( wk->bgl, wk->bmpwin, GetBmpNo(wk->fr_no) );

	sys_VBlankFuncChange( VBlankFunc, (void*)wk );		//VBlankセット
	
	(*seq) = SEQ_GAME_INIT;

	return PROC_RES_FINISH;
}

//--------------------------------------------------------------
/**
 * @brief	プロセス関数：メイン
 *
 * @param	proc	プロセスデータ
 * @param	seq		シーケンス
 *
 * @return	"処理状況"
 */
//--------------------------------------------------------------
PROC_RESULT FrontierMonitorProc_Main( PROC * proc, int * seq )
{
	FRONTIER_MONITOR_WORK * wk  = PROC_GetWork( proc );

	switch( *seq ){

	//初期化
	case SEQ_GAME_INIT:
		if( Seq_GameInit(wk) == TRUE ){
			NextSeq( wk, seq, SEQ_GAME_START );
		}
		break;

	//ゲーム開始
	case SEQ_GAME_START:
		if( Seq_GameStart(wk) == TRUE ){
			NextSeq( wk, seq, SEQ_GAME_END );		//終了
		}
		break;

	//終了
	case SEQ_GAME_END:
		if( Seq_GameEnd(wk) == TRUE ){
			return PROC_RES_FINISH;
		}
	}

	return PROC_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief	プロセス関数：終了
 *
 * @param	proc	プロセスデータ
 * @param	seq		シーケンス
 *
 * @return	"処理状況"
 */
//--------------------------------------------------------------
PROC_RESULT FrontierMonitorProc_End( PROC * proc, int * seq )
{
	int i;
	FRONTIER_MONITOR_WORK* wk = PROC_GetWork( proc );

	//パレットフェード開放
	PaletteFadeWorkAllocFree( wk->pfd, FADE_MAIN_OBJ );
	PaletteFadeWorkAllocFree( wk->pfd, FADE_MAIN_BG );

	//パレットフェードシステム開放
	PaletteFadeFree( wk->pfd );
	wk->pfd = NULL;

	MSGMAN_Delete( wk->msgman );						//メッセージマネージャ開放
	WORDSET_Delete( wk->wordset );
	STRBUF_Delete( wk->msg_buf );						//メッセージバッファ開放
	STRBUF_Delete( wk->tmp_buf );						//メッセージバッファ開放

	FrontierMonitorExitBmpWin( wk->bmpwin, GetBmpNo(wk->fr_no) );	//BMPウィンドウ開放

	Frontier_BgExit( wk->bgl );							//BGL削除

	PROC_FreeWork( proc );								//ワーク開放

	sys_VBlankFuncChange( NULL, NULL );					//VBlankセット
	sys_DeleteHeap( HEAPID_FRONTIER_MONITOR );

	return PROC_RES_FINISH;
}


//==============================================================================================
//
//	シーケンス
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	ゲーム初期化
 *
 * @param	wk		FRONTIER_MONITOR_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameInit( FRONTIER_MONITOR_WORK* wk )
{
	switch( wk->sub_seq ){

	case 0:
		wk->sub_seq++;
		break;

	//ブラックイン
	case 1:
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
						WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC*3, HEAPID_FRONTIER_MONITOR );
		Frontier_Write( wk );
		wk->sub_seq++;
		break;
		
	//フェード終了待ち
	case 2:
		if( WIPE_SYS_EndCheck() == TRUE ){
			return TRUE;
		}
		break;
	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	ゲーム開始
 *
 * @param	wk		FRONTIER_MONITOR_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameStart( FRONTIER_MONITOR_WORK* wk )
{
	int i;

	switch( wk->sub_seq ){

	//
	case 0:
		wk->sub_seq++;
		break;

	//
	case 1:
		//BgCheck();									//BGのON・OFF(デバック)

		//終了チェック
		if( (sys.cont & PAD_BUTTON_A) ||
			(sys.cont & PAD_BUTTON_B) ){
			return TRUE;
		}

		//wk->sub_seq++;
		break;
	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	シーケンス：終了へ
 *
 * @param	wk		FRONTIER_MONITOR_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameEnd( FRONTIER_MONITOR_WORK* wk )
{
	int i;

	switch( wk->sub_seq ){

	//フェードアウト
	case 0:
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
						WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_FRONTIER_MONITOR );
		wk->sub_seq++;
		break;

	//フェード終了待ち
	case 1:
		if( WIPE_SYS_EndCheck() == TRUE ){
			return TRUE;
		}
		break;
	};

	return FALSE;
}


//==============================================================================================
//
//	設定
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	VBlank関数
 *
 * @param	work	ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------
static void VBlankFunc( void * work )
{
	FRONTIER_MONITOR_WORK* wk = work;

	//パレット転送
	if( wk->pfd != NULL ){
		PaletteFadeTrans( wk->pfd );
	}

	GF_BGL_VBlankFunc( wk->bgl );

	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}

//--------------------------------------------------------------
/**
 * @brief	VRAM設定
 *
 * @param	none
 *
 * @return	none
 *
 * 細かく設定していないので注意！
 */
//--------------------------------------------------------------
static void SetVramBank(void)
{
	GF_BGL_DISPVRAM tbl = {
		GX_VRAM_BG_128_A,				//メイン2DエンジンのBG
		GX_VRAM_BGEXTPLTT_NONE,			//メイン2DエンジンのBG拡張パレット

		GX_VRAM_SUB_BG_128_C,			//サブ2DエンジンのBG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		//サブ2DエンジンのBG拡張パレット

		GX_VRAM_OBJ_128_B,				//メイン2DエンジンのOBJ
		GX_VRAM_OBJEXTPLTT_NONE,		//メイン2DエンジンのOBJ拡張パレット

		GX_VRAM_SUB_OBJ_16_I,			//サブ2DエンジンのOBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	//サブ2DエンジンのOBJ拡張パレット

		GX_VRAM_TEX_NONE,				//テクスチャイメージスロット
		GX_VRAM_TEXPLTT_NONE			//テクスチャパレットスロット
	};

	GF_Disp_SetBank( &tbl );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	BG設定
 *
 * @param	init	BGLデータ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void SetBgHeader( GF_BGL_INI * ini )
{
	/******************/
	//CHRA,SCRは要確認！
	/******************/

	{	//BG SYSTEM
		GF_BGL_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
		};
		GF_BGL_InitBG( &BGsys_data );
	}

	{	//BG(フォント)
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, BFM_FRAME_WIN, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ClearCharSet( BFM_FRAME_WIN, 32, 0, HEAPID_FRONTIER_MONITOR );
		GF_BGL_ScrClear( ini, BFM_FRAME_WIN );
	}

	{	//上画面
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, BFM_FRAME_BG, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ClearCharSet( BFM_FRAME_BG, 32, 0, HEAPID_FRONTIER_MONITOR );
		GF_BGL_ScrClear( ini, BFM_FRAME_BG );
	}

	//--------------------------------------------------------------------------------
	{	//下画面(ボール)
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x3000, GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, BFM_FRAME_SUB, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ClearCharSet( BFM_FRAME_SUB, 32, 0, HEAPID_FRONTIER_MONITOR );
		GF_BGL_ScrClear( ini, BFM_FRAME_SUB );
	}

	G2_SetBG0Priority( 0 );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
	return;
}


//==============================================================================================
//
//	共通初期化、終了
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	BG関連初期化
 *
 * @param	wk		FRONTIER_MONITOR_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Frontier_BgInit( FRONTIER_MONITOR_WORK* wk )
{
	SetVramBank();
	SetBgHeader( wk->bgl );

	//パレットフェードシステムワーク作成
	wk->pfd = PaletteFadeInit( HEAPID_FRONTIER_MONITOR );

	//リクエストデータをmallocしてセット
	PaletteFadeWorkAllocSet( wk->pfd, FADE_MAIN_OBJ, FADE_PAL_ALL_SIZE, HEAPID_FRONTIER_MONITOR );
	PaletteFadeWorkAllocSet( wk->pfd, FADE_MAIN_BG, FADE_PAL_ALL_SIZE, HEAPID_FRONTIER_MONITOR );

	//上画面背景
	Frontier_SetMainBgGraphic( wk, BFM_FRAME_BG );
	Frontier_SetMainBgPalette();

	//下画面背景、パレットセット
	Frontier_SetSubBgGraphic( wk, BFM_FRAME_SUB );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	BG解放
 *
 * @param	ini		BGLデータ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Frontier_BgExit( GF_BGL_INI * ini )
{
	//メイン画面の各面の表示コントロール(表示OFF)
	GF_Disp_GX_VisibleControl(	GX_PLANEMASK_BG0 | 
								GX_PLANEMASK_BG1 | 
								GX_PLANEMASK_BG2 |
								GX_PLANEMASK_BG3 | 
								GX_PLANEMASK_OBJ, 
								VISIBLE_OFF );

	//サブ画面の各面の表示コントロール(表示OFF)
	GF_Disp_GXS_VisibleControl(	GX_PLANEMASK_BG0 | 
								GX_PLANEMASK_BG1 | 
								GX_PLANEMASK_BG2 | 
								GX_PLANEMASK_BG3 | 
								GX_PLANEMASK_OBJ, 
								VISIBLE_OFF );

	//GF_BGL_BGControlSetで取得したメモリを開放
	GF_BGL_BGControlExit( ini, BFM_FRAME_WIN );
	GF_BGL_BGControlExit( ini, BFM_FRAME_BG );
	GF_BGL_BGControlExit( ini, BFM_FRAME_SUB );

	sys_FreeMemoryEz( ini );
	return;
}


//==============================================================================================
//
//	ツール
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	シーケンス変更
 *
 * @param	wk		FRONTIER_MONITOR_WORK型のポインタ
 * @param	seq		シーケンスのポインタ
 * @param	next	次のシーケンス定義
 *
 * @return	none
 */
//--------------------------------------------------------------
static void NextSeq( FRONTIER_MONITOR_WORK* wk, int* seq, int next )
{
	wk->sub_seq = 0;
	*seq		= next;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	キーチェック
 *
 * @param	key		チェックするキー
 *
 * @return	"結果"
 */
//--------------------------------------------------------------
static int KeyCheck( int key )
{
	return (sys.cont & key);
}

//--------------------------------------------------------------
/**
 * @brief	BGチェック
 *
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------
static void BgCheck(void)
{
	if( sys.cont & PAD_KEY_DOWN ){
		if( sys.cont & PAD_BUTTON_L ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
		}else if( sys.cont & PAD_BUTTON_B ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
		}
	}else if( sys.cont & PAD_KEY_RIGHT ){
		if( sys.cont & PAD_BUTTON_L ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
		}else if( sys.cont & PAD_BUTTON_B ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
		}
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	施設ナンバーからBMPナンバーを取得
 *
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------
static u8 GetBmpNo( u8 fr_no )
{
	u8 bmp_no;

	bmp_no = 0;

	switch( fr_no ){

	case FRONTIER_NO_FACTORY_LV50:
	case FRONTIER_NO_FACTORY_LV100:
		bmp_no = 0;
		break;

	case FRONTIER_NO_STAGE:
		bmp_no = 1;
		break;

	case FRONTIER_NO_CASTLE:
		bmp_no = 2;
		break;

	case FRONTIER_NO_ROULETTE:
		bmp_no = 3;
		break;

	case FRONTIER_NO_TOWER:
		bmp_no = 4;
		break;
	};

	return bmp_no;
}


//==============================================================================================
//
//	BGグラフィックデータ
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	グラフィックデータセット：上画面背景(シングル、ダブル)
 *
 * @param	wk		FRONTIER_MONITOR_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Frontier_SetMainBgGraphic( FRONTIER_MONITOR_WORK * wk, u32 frm  )
{
	ARCHANDLE* hdl;
	hdl = ArchiveDataHandleOpen( ARC_FRONTIER_BG, HEAPID_FRONTIER_MONITOR );

	ArcUtil_HDL_BgCharSet(	hdl, SEISEKI_NCGR_BIN, 
							wk->bgl, frm, 0, 0, 1, HEAPID_FRONTIER_MONITOR );

	ArcUtil_HDL_ScrnSet(hdl, SEISEKI_NSCR_BIN, 
						wk->bgl, frm, 0, 0, 1, HEAPID_FRONTIER_MONITOR );

	ArchiveDataHandleClose( hdl );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	上画面背景パレット設定
 *
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Frontier_SetMainBgPalette(void)
{
	void *buf;
	NNSG2dPaletteData *dat;

	buf = ArcUtil_PalDataGet(ARC_FRONTIER_BG, SEISEKI_NCLR, &dat,HEAPID_FRONTIER_MONITOR);
		
	DC_FlushRange( dat->pRawData, (sizeof(u16)*16*2) );
	GX_LoadBGPltt( dat->pRawData, 0, (sizeof(u16)*16*2) );		//メイン

	sys_FreeMemoryEz(buf);
	return;
}

//--------------------------------------------------------------
/**
 * @brief	グラフィックデータセット：下画面背景、パレットセット
 *
 * @param	wk		FRONTIER_MONITOR_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Frontier_SetSubBgGraphic( FRONTIER_MONITOR_WORK * wk, u32 frm  )
{
	ARCHANDLE* hdl;
		
	//とりあえずポケッチにある下画面を使いまわす
	hdl = ArchiveDataHandleOpen( ARC_POKETCH_IMG, HEAPID_FRONTIER_MONITOR );

	ArcUtil_HDL_BgCharSet(	hdl, NARC_poketch_before_lz_ncgr, wk->bgl, frm,
							0, 0, TRUE, HEAPID_FRONTIER_MONITOR );

	ArcUtil_HDL_ScrnSet(hdl, NARC_poketch_before_lz_nscr, wk->bgl, frm,
						0, 0, TRUE, HEAPID_FRONTIER_MONITOR );

	ArcUtil_HDL_PalSet(	hdl, NARC_poketch_before_nclr, PALTYPE_SUB_BG,
						0, 0x20, HEAPID_FRONTIER_MONITOR );
		
	ArchiveDataHandleClose( hdl );
	return;
}


//==============================================================================================
//
//	メッセージ関連
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	メッセージ表示
 *
 * @param	wk		FRONTIER_MONITOR_WORK型のポインタ
 * @param	win		ビットマップウィンドウ
 * @param	msg_id	メッセージID
 * @param	x		X座標(ドット単位)
 * @param	y		Y座標(ドット単位)
 * @param	f_col	文字色ナンバー(塗りつぶしカラーコード)
 * @param	s_col	影色ナンバー
 * @param	b_col	背景色ナンバー
 * @param	font	フォント種類
 * @param	fill	塗りつぶし有り無し
 * @param   align   水平方向の配置
 *
 * @return	"文字描画ルーチンのインデックス"
 */
//--------------------------------------------------------------
static u8 FrontierWriteMsg( FRONTIER_MONITOR_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u8 f_col, u8 s_col, u8 b_col, u8 font, u8 fill, int align )
{
	if( fill != 0 ){
		GF_BGL_BmpWinDataFill( win, b_col );			//塗りつぶし
	}

	MSGMAN_GetString( wk->msgman, msg_id, wk->tmp_buf );

	//登録された単語を使って文字列展開する
	WORDSET_ExpandStr( wk->wordset, wk->msg_buf, wk->tmp_buf );

    //MatchComment: string alignment
    switch(align){
    case FTRM_ALN_CENTER:
        x -= (FontProc_GetPrintStrWidth( FONT_SYSTEM, wk->msg_buf, 0 )+1)/2;
        break;
    case FTRM_ALN_RIGHT:
        x -= FontProc_GetPrintStrWidth( FONT_SYSTEM, wk->msg_buf, 0 );
        break;
    }

	return GF_STR_PrintColor( win, font, wk->msg_buf, x, y, MSG_ALLPUT, 
								GF_PRINTCOLOR_MAKE(f_col,s_col,b_col), NULL );
}
 
//--------------------------------------------------------------
/**
 * @brief	簡単メッセージ表示
 *
 * @param	win		GF_BGL_BMPWIN型のポインタ
 * @param	msg_id	メッセージID
 *
 * @return	"msg_index"
 */
//--------------------------------------------------------------
static u8 EasyMsg( FRONTIER_MONITOR_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u8 fill )
{
	return FrontierWriteMsg(	wk, win, msg_id, 0, 0,
							FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, fill, FTRM_ALN_LEFT );
}

//--------------------------------------------------------------
/**
 * @brief	数値をセット
 *
 * @param	wk		FRONTIER_WORK型のポインタ
 * @param	bufID	バッファID
 * @param	number	セットする数値
 *
 * @retval	none
 *
 * 4桁固定にしている
 */
//--------------------------------------------------------------
static void SetNumber( FRONTIER_MONITOR_WORK* wk, u32 bufID, s32 number )
{
	WORDSET_RegisterNumber( wk->wordset, bufID, number, 4, 
							NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	各施設ごとの情報を書き込む
 *
 * @param	wk		FRONTIER_WORK型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void Frontier_Write( FRONTIER_MONITOR_WORK* wk )
{
	int msg_id;

	switch( wk->fr_no ){

	//ファクトリー
	case FRONTIER_NO_FACTORY_LV50:
	case FRONTIER_NO_FACTORY_LV100:
		Factory_SetLv50Info( wk );	//「ＬＶ５０」
		Factory_SetLv100Info( wk );	//「オープン」
		break;

	//ステージ
	case FRONTIER_NO_STAGE:
		Stage_SetInfo( wk );
		break;

	case FRONTIER_NO_CASTLE:
		Castle_SetInfo( wk );
		break;

	case FRONTIER_NO_ROULETTE:
		Roulette_SetInfo( wk );
		break;

	case FRONTIER_NO_TOWER:
		if( (wk->type == BTWR_MODE_SINGLE) ||
			(wk->type == BTWR_MODE_DOUBLE) ){
			Tower_SetInfoSingleDouble( wk );
		}else{
			Tower_SetInfoMulti( wk );
		}
		break;

	//エラー回避
	default:
		Roulette_SetInfo( wk );
		break;

	};

	return;
}


//==============================================================================================
//
//	ファクトリー
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	LV50情報をセット
 *
 * @param	wk		FRONTIER_WORK型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void Factory_SetLv50Info( FRONTIER_MONITOR_WORK* wk )
{
	u8 no,clear_flag;
	int msg_id;
	u32 x;

	//----------------------------------------------------------
	no = BF_BMPWIN_TITLE;

	GF_BGL_BmpWinDataFill( &wk->bmpwin[no], FBMP_COL_NULL );			//塗りつぶし

	//「バトルファクトリー」
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list02, BF_STR_X, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0, FTRM_ALN_LEFT );

	if( wk->type == FACTORY_TYPE_SINGLE ){
		msg_id = msg_bf_seiseki_list06;
		x = TITLE_STR_SINGLE_X_FACTORY;
	}else if( wk->type == FACTORY_TYPE_DOUBLE ){
		msg_id = msg_bf_seiseki_list07;
		x = TITLE_STR_X_FACTORY;
	}else{
		msg_id = msg_bf_seiseki_list08;
		x = TITLE_STR_X_FACTORY;
	}

	//「シングルせいせき」
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_id, x, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0, FTRM_ALN_RIGHT );
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[no] );
	
	//----------------------------------------------------------
	no = BF_BMPWIN_ENTRY1;
	//「ＬＶ５０」
	EasyMsg( wk, &wk->bmpwin[no], msg_bf_seiseki_list13, 1 );
	//「レンタル／こうかん」
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list15, 28*8, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0, FTRM_ALN_RIGHT );
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[no] );

	//----------------------------------------------------------
	no = BF_BMPWIN_BEFORE;
	//「ぜんかい」「げんざい」
	EasyMsg( wk, &wk->bmpwin[no], Factory_GetMsg(wk,0), 1 );

	//「○○れんしょう」
	SetNumber(	wk, 0, 
		FrontierRecord_Get( wk->f_sv, FactoryScr_GetWinRecordID(FACTORY_LEVEL_50,wk->type),
							FRONTIER_RECORD_NOT_FRIEND) );
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list17, WIN_NUM_X, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0, FTRM_ALN_CENTER );

	//「○○かい」
	SetNumber(	wk, 0, 
		FrontierRecord_Get( wk->f_sv,FactoryScr_GetTradeRecordID(FACTORY_LEVEL_50,wk->type),
							FRONTIER_RECORD_NOT_FRIEND) );
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list19, TRD_NUM_X, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0, FTRM_ALN_RIGHT );
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[no] );
		
	//----------------------------------------------------------
	no = BF_BMPWIN_MAX;
	//「さいこう」
	EasyMsg( wk, &wk->bmpwin[no], msg_bf_seiseki_list10, 1 );

	//「○○れんしょう」
	SetNumber(	wk, 0, 
		FrontierRecord_Get(	wk->f_sv,FactoryScr_GetMaxWinRecordID(FACTORY_LEVEL_50,wk->type),
							FRONTIER_RECORD_NOT_FRIEND) );
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list17, WIN_NUM_X, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0, FTRM_ALN_CENTER );

	//「○○かい」
	SetNumber(	wk, 0,
		FrontierRecord_Get( wk->f_sv,FactoryScr_GetMaxTradeRecordID(FACTORY_LEVEL_50,wk->type),
							FRONTIER_RECORD_NOT_FRIEND) );
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list19, TRD_NUM_X, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0, FTRM_ALN_RIGHT );
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[no] );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	LV100情報をセット
 *
 * @param	wk		FRONTIER_WORK型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void Factory_SetLv100Info( FRONTIER_MONITOR_WORK* wk )
{
	u8 no;
	u32 x;

	//----------------------------------------------------------
	no = BF_BMPWIN_ENTRY2;
	//「オープンレベル」
	EasyMsg( wk, &wk->bmpwin[no], msg_bf_seiseki_list14, 1 );

	//「レンタル／こうかん」
//	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list15, 19*8, 0,
//					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0 );
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[no] );

	//----------------------------------------------------------
	no = BF_BMPWIN_BEFORE2;
	//「ぜんかい」「げんざい」
	EasyMsg( wk, &wk->bmpwin[no], Factory_GetMsg(wk,1), 1 );

	//「○○れんしょう」
	SetNumber(	wk, 0, 
		FrontierRecord_Get( wk->f_sv, FactoryScr_GetWinRecordID(FACTORY_LEVEL_OPEN,wk->type),
							FRONTIER_RECORD_NOT_FRIEND) );
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list17, WIN_NUM_X, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0, FTRM_ALN_CENTER );

	//「○○かい」
	SetNumber(	wk, 0, 
		FrontierRecord_Get( wk->f_sv, FactoryScr_GetTradeRecordID(FACTORY_LEVEL_OPEN,wk->type),
							FRONTIER_RECORD_NOT_FRIEND) );
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list19, TRD_NUM_X, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0, FTRM_ALN_RIGHT );
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[no] );
		
	//----------------------------------------------------------
	no = BF_BMPWIN_MAX2;
	//「さいこう」
	EasyMsg( wk, &wk->bmpwin[no], msg_bf_seiseki_list10, 1 );

	//「○○れんしょう」
	SetNumber(	wk, 0, 
		FrontierRecord_Get( wk->f_sv,FactoryScr_GetMaxWinRecordID(FACTORY_LEVEL_OPEN,wk->type),
							FRONTIER_RECORD_NOT_FRIEND) );
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list17, WIN_NUM_X, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0, FTRM_ALN_CENTER );

	//「○○かい」
	SetNumber(	wk, 0,
		FrontierRecord_Get( wk->f_sv, FactoryScr_GetMaxTradeRecordID(FACTORY_LEVEL_OPEN,wk->type),
							FRONTIER_RECORD_NOT_FRIEND) );
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list19, TRD_NUM_X, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0, FTRM_ALN_RIGHT );
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[no] );
	return;
}

static u32 Factory_GetMsg( FRONTIER_MONITOR_WORK* wk, u8 level )
{
	u8 clear_flag;
	u32 clear_id;

	if( level == 0 ){
		clear_id = FRID_FACTORY_MULTI_WIFI_CLEAR_BIT;
	}else{
		clear_id = FRID_FACTORY_MULTI_WIFI_CLEAR100_BIT;
	}

	//WIFIのみ特殊
	if( wk->type == FACTORY_TYPE_WIFI_MULTI ){

		clear_flag = FrontierRecord_Get(SaveData_GetFrontier(wk->sv), clear_id,
										Frontier_GetFriendIndex(clear_id) );
	}else{
		//クリアしたかフラグを取得
		clear_flag = (u8)FACTORYSCORE_GetScoreData(	SaveData_GetFactoryScore(wk->sv), 
													FACTORYSCORE_ID_CLEAR_FLAG, 
													(level*FACTORY_TYPE_MAX)+wk->type, NULL );
	}

	if( clear_flag == 1 ){
		//「げんざい」
		return msg_bf_seiseki_list23;
	}

	//「ぜんかい」
	return msg_bf_seiseki_list09;
}


//==============================================================================================
//
//	ステージ
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	情報をセット
 *
 * @param	wk		FRONTIER_WORK型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void Stage_SetInfo( FRONTIER_MONITOR_WORK* wk )
{
	u8 no;
	u16 before_monsno;
	int msg_id;
	u32 x;
	int num;
	FRONTIER_EX_SAVEDATA * fes;
	LOAD_RESULT load_result;

	//----------------------------------------------------------
	no = BS_BMPWIN_TITLE;

	GF_BGL_BmpWinDataFill( &wk->bmpwin[no], FBMP_COL_NULL );			//塗りつぶし

	//「バトルステージ」
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list04, BF_STR_X, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0, FTRM_ALN_LEFT );

	if( wk->type == STAGE_TYPE_SINGLE ){
		msg_id = msg_bf_seiseki_list06;
		x = TITLE_STR_SINGLE_X_STAGE;
	}else if( wk->type == STAGE_TYPE_DOUBLE ){
		msg_id = msg_bf_seiseki_list07;
		x = TITLE_STR_X_STAGE;
	}else{
		msg_id = msg_bf_seiseki_list08;
		x = TITLE_STR_X_STAGE;
	}

	//「シングルせいせき」
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_id, x, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0, FTRM_ALN_RIGHT );
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[no] );
	
	//----------------------------------------------------------
	no = BS_BMPWIN_ENTRY1;
	//ポケモン名セット
	FrontierMonitor_PokeName( wk );
	//EasyMsg( wk, &wk->bmpwin[no], msg_bf_seiseki_03_01, 1 );

	//FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list18, 19*8, 0,
	//FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list18, 4*8, 0,
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list18, 0, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0, FTRM_ALN_LEFT );
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[no] );

	//----------------------------------------------------------
	no = BS_BMPWIN_BEFORE;
	//「ぜんかい」
	EasyMsg( wk, &wk->bmpwin[no], msg_bf_seiseki_list09, 1 );

	//挑戦中のポケモンナンバーを取得
	before_monsno = FrontierRecord_Get(	SaveData_GetFrontier(wk->sv), 
							StageScr_GetMonsNoRecordID(wk->type),
							Frontier_GetFriendIndex(StageScr_GetMonsNoRecordID(wk->type)) );

	//挑戦中以外のポケモンの現在の連勝数は0にする
	if( wk->monsno != before_monsno ){
		num = 0;
	}else{
		num = FrontierRecord_Get(	wk->f_sv, StageScr_GetWinRecordID(wk->type),
									FRONTIER_RECORD_NOT_FRIEND );
	}
	OS_Printf( "monsno = %d\n", wk->monsno );
	OS_Printf( "num = %d\n", num );

	//「○○れんしょう」
	SetNumber(	wk, 0, num );
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list17, WIN_NUM_X, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0, FTRM_ALN_CENTER );
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[no] );
		
	//----------------------------------------------------------
	no = BS_BMPWIN_MAX;
	//「さいこう」
	EasyMsg( wk, &wk->bmpwin[no], msg_bf_seiseki_list10, 1 );

#if 0
	//「○○れんしょう」
	SetNumber(	wk, 0, 
				FrontierRecord_Get( wk->f_sv, StageScr_GetMaxWinRecordID(wk->type),
									FRONTIER_RECORD_NOT_FRIEND) );
#else
	fes = FrontierEx_Load( wk->sv, HEAPID_WORLD, &load_result );
	if( load_result != LOAD_RESULT_OK ){
		num = 0;	//セーブ破壊orキーと一致しない、場合は新規データと解釈し、カウンタ0とする
	}else{
		num = FrontierEx_StageRenshou_Get(	wk->sv, fes, 
											StageScr_GetExMaxWinRecordID(wk->type), wk->monsno );
	}
	OS_Printf( "num = %d\n", num );

#if 0
	//デバック
	OS_Printf( "num = %d\n", FrontierEx_StageRenshou_Get(	wk->sv, fes, 
										StageScr_GetExMaxWinRecordID(wk->type), MONSNO_BIIBARU ) );
	OS_Printf( "num = %d\n", FrontierEx_StageRenshou_Get(	wk->sv, fes, 
										StageScr_GetExMaxWinRecordID(wk->type), MONSNO_KAIRIKII ) );
	OS_Printf( "num = %d\n", FrontierEx_StageRenshou_Get(	wk->sv, fes, 
										StageScr_GetExMaxWinRecordID(wk->type), MONSNO_HIKOZARU ) );
#endif

	if( fes != NULL ){
		sys_FreeMemoryEz(fes);
	}

	SetNumber(	wk, 0, num );
#endif
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list17, WIN_NUM_X, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0, FTRM_ALN_CENTER );
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[no] );
	return;
}

static u32 Stage_GetMsg( FRONTIER_MONITOR_WORK* wk )
{
#if 0
	u8 clear_flag;

	//WIFIのみ特殊
	if( wk->type == STAGE_TYPE_WIFI_MULTI ){
		clear_flag = FrontierRecord_Get(SaveData_GetFrontier(wk->sv), 
										FRID_STAGE_MULTI_WIFI_CLEAR_BIT,
										Frontier_GetFriendIndex(FRID_STAGE_MULTI_WIFI_CLEAR_BIT) );
	}else{
		//クリアしたかフラグを取得
		clear_flag = (u8)STAGESCORE_GetScoreData(	SaveData_GetStageScore(wk->sv), 
													STAGESCORE_ID_CLEAR_FLAG, 
													wk->type, 0, NULL );
	}

	if( clear_flag == 1 ){
		//「げんざい」
		return msg_bf_seiseki_list23;
	}
#endif

	//「ぜんかい」
	return msg_bf_seiseki_list09;
}

//--------------------------------------------------------------------------------------------
/**
 * モンスターナンバーからポケモン名を指定バッファに登録
 *
 * @param	core		仮想マシン制御構造体へのポインタ
 *
 * @return	"0"
 */
//--------------------------------------------------------------------------------------------
static void FrontierMonitor_PokeName( FRONTIER_MONITOR_WORK* wk )
{
	MSGDATA_MANAGER* man;
	STRBUF* buf;

	man = MSGMAN_Create(MSGMAN_TYPE_DIRECT,ARC_MSG,NARC_msg_monsname_dat,HEAPID_FRONTIER_MONITOR);
	buf = MSGMAN_AllocString( man, wk->monsno );
	MSGMAN_Delete( man );

	//単語セット
	//バッファID
	//文字列
	//性別コード
	//単／複（TRUEで単数）
	//言語コード
	WORDSET_RegisterWord( wk->wordset, 0, buf, 0, 0, PM_LANG );
	STRBUF_Delete( buf );
	return;
}


//==============================================================================================
//
//	キャッスル
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	情報をセット
 *
 * @param	wk		FRONTIER_WORK型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
#ifdef NONEQUIVALENT
static void Castle_SetInfo( FRONTIER_MONITOR_WORK* wk )
{
	u8 no;
	u16 now_cp;
	int msg_id;
	u32 x;

	//----------------------------------------------------------
	no = BC_BMPWIN_TITLE;

	GF_BGL_BmpWinDataFill( &wk->bmpwin[no], FBMP_COL_NULL );			//塗りつぶし

	//「バトルキャッスル」
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list03, BF_STR_X, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0 );

	if( wk->type == CASTLE_TYPE_SINGLE ){
		msg_id = msg_bf_seiseki_list06;
		x = TITLE_STR_SINGLE_X_CASTLE;
	}else if( wk->type == CASTLE_TYPE_DOUBLE ){
		msg_id = msg_bf_seiseki_list07;
		x = TITLE_STR_X_CASTLE;
	}else{
		msg_id = msg_bf_seiseki_list08;
		x = TITLE_STR_X_CASTLE;
	}

	//「シングルせいせき」
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_id, x, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0 );
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[no] );

	//----------------------------------------------------------
	no = BC_BMPWIN_ENTRY1;
	//「キャッスルポイント」
	EasyMsg( wk, &wk->bmpwin[no], msg_bf_seiseki_list16, 1 );
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[no] );

	//----------------------------------------------------------
	no = BC_BMPWIN_BEFORE;
	//「ぜんかい」「げんざい」
	EasyMsg( wk, &wk->bmpwin[no], Castle_GetMsg(wk), 1 );

	//「○○れんしょう」
	SetNumber(	wk, 0, 
				FrontierRecord_Get( wk->f_sv, CastleScr_GetWinRecordID(wk->type),
									FRONTIER_RECORD_NOT_FRIEND) );
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list21, WIN_NUM_X, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0 );

	//「○○ＣＰ」残りCPを表示
	now_cp = FrontierRecord_Get(SaveData_GetFrontier(wk->sv), CastleScr_GetCPRecordID(wk->type),
								Frontier_GetFriendIndex(CastleScr_GetCPRecordID(wk->type)) );
	//OS_Printf( "前回残りcp = %d\n", now_cp );
	SetNumber(	wk, 0, now_cp );
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list20, CP_STR_X, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0 );

	GF_BGL_BmpWinOnVReq( &wk->bmpwin[no] );
		
	//----------------------------------------------------------
	no = BC_BMPWIN_MAX;
	//「さいこう」
	EasyMsg( wk, &wk->bmpwin[no], msg_bf_seiseki_list10, 1 );

	//「○○れんしょう」
	SetNumber(	wk, 0, 
				FrontierRecord_Get( wk->f_sv, CastleScr_GetMaxWinRecordID(wk->type),
									FRONTIER_RECORD_NOT_FRIEND) );
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list21, WIN_NUM_X, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0 );

	//「○○ＣＰ」残りCPを表示
	now_cp = FrontierRecord_Get(SaveData_GetFrontier(wk->sv), 
							CastleScr_GetRemainderCPRecordID(wk->type),
							Frontier_GetFriendIndex(CastleScr_GetRemainderCPRecordID(wk->type)) );
	//OS_Printf( "最高残りcp = %d\n", now_cp );
	SetNumber(	wk, 0, now_cp );
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list20, CP_STR_X, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0 );

	GF_BGL_BmpWinOnVReq( &wk->bmpwin[no] );
	return;
}
#else
asm static void Castle_SetInfo( FRONTIER_MONITOR_WORK* wk )
{
	push {r3, r4, r5, r6, lr}
	sub sp, #0x1c
	add r5, r0, #0
	add r0, #0x10
	mov r1, #0
	bl GF_BGL_BmpWinDataFill
	mov r1, #0
	str r1, [sp]
	mov r0, #1
	str r0, [sp, #4]
	mov r0, #2
	str r0, [sp, #8]
	str r1, [sp, #0xc]
	str r1, [sp, #0x10]
	str r1, [sp, #0x14]
	str r1, [sp, #0x18]
	add r1, r5, #0
	add r0, r5, #0
	add r1, #0x10
	mov r2, #0x17
	mov r3, #8
	bl FrontierWriteMsg
	ldrb r0, [r5, #5]
	cmp r0, #0
	bne _021D1A30
	mov r2, #0x1a
	mov r3, #0xd8
	b _021D1A3E
_021D1A30:
	cmp r0, #1
	bne _021D1A3A
	mov r2, #0x1b
	mov r3, #0xd8
	b _021D1A3E
_021D1A3A:
	mov r2, #0x1c
	mov r3, #0xd8
_021D1A3E:
	mov r1, #0
	str r1, [sp]
	mov r0, #1
	str r0, [sp, #4]
	mov r0, #2
	str r0, [sp, #8]
	str r1, [sp, #0xc]
	str r1, [sp, #0x10]
	str r1, [sp, #0x14]
	add r1, r5, #0
	str r0, [sp, #0x18]
	add r0, r5, #0
	add r1, #0x10
	bl FrontierWriteMsg
	add r0, r5, #0
	add r0, #0x10
	bl GF_BGL_BmpWinOnVReq
	mov r2, #0
	str r2, [sp]
	mov r1, #1
	str r1, [sp, #4]
	mov r0, #2
	str r0, [sp, #8]
	str r2, [sp, #0xc]
	str r2, [sp, #0x10]
	str r1, [sp, #0x14]
	add r1, r5, #0
	str r0, [sp, #0x18]
	add r0, r5, #0
	add r1, #0x20
	mov r2, #0x25
	mov r3, #0x60
	bl FrontierWriteMsg
	add r0, r5, #0
	add r0, #0x20
	bl GF_BGL_BmpWinOnVReq
	add r0, r5, #0
	bl Castle_GetMsg
	add r1, r5, #0
	add r2, r0, #0
	add r0, r5, #0
	add r1, #0x30
	mov r3, #1
	bl EasyMsg
	ldrb r0, [r5, #5]
	bl CastleScr_GetWinRecordID
	add r1, r0, #0
	mov r0, #0x4b
	lsl r0, r0, #2
	ldr r0, [r5, r0]
	mov r2, #0xff
	bl FrontierRecord_Get
	add r2, r0, #0
	add r0, r5, #0
	mov r1, #0
	bl SetNumber
	mov r2, #0
	str r2, [sp]
	mov r1, #1
	str r1, [sp, #4]
	mov r0, #2
	str r0, [sp, #8]
	str r2, [sp, #0xc]
	str r2, [sp, #0x10]
	str r2, [sp, #0x14]
	str r1, [sp, #0x18]
	add r1, r5, #0
	add r0, r5, #0
	add r1, #0x30
	mov r2, #0x2a
	mov r3, #0x70
	bl FrontierWriteMsg
	mov r0, #0x4a
	lsl r0, r0, #2
	ldr r0, [r5, r0]
	bl SaveData_GetFrontier
	add r4, r0, #0
	ldrb r0, [r5, #5]
	bl CastleScr_GetCPRecordID
	add r6, r0, #0
	ldrb r0, [r5, #5]
	bl CastleScr_GetCPRecordID
	bl Frontier_GetFriendIndex
	add r2, r0, #0
	add r0, r4, #0
	add r1, r6, #0
	bl FrontierRecord_Get
	add r2, r0, #0
	add r0, r5, #0
	mov r1, #0
	bl SetNumber
	mov r1, #0
	str r1, [sp]
	mov r0, #1
	str r0, [sp, #4]
	mov r0, #2
	str r0, [sp, #8]
	str r1, [sp, #0xc]
	str r1, [sp, #0x10]
	str r1, [sp, #0x14]
	add r1, r5, #0
	str r0, [sp, #0x18]
	add r0, r5, #0
	add r1, #0x30
	mov r2, #0x29
	mov r3, #0xe0
	bl FrontierWriteMsg
	add r0, r5, #0
	add r0, #0x30
	bl GF_BGL_BmpWinOnVReq
	add r1, r5, #0
	add r0, r5, #0
	add r1, #0x40
	mov r2, #0x1f
	mov r3, #1
	bl EasyMsg
	ldrb r0, [r5, #5]
	bl CastleScr_GetMaxWinRecordID
	add r1, r0, #0
	mov r0, #0x4b
	lsl r0, r0, #2
	ldr r0, [r5, r0]
	mov r2, #0xff
	bl FrontierRecord_Get
	add r2, r0, #0
	add r0, r5, #0
	mov r1, #0
	bl SetNumber
	mov r2, #0
	str r2, [sp]
	mov r1, #1
	str r1, [sp, #4]
	mov r0, #2
	str r0, [sp, #8]
	str r2, [sp, #0xc]
	str r2, [sp, #0x10]
	str r2, [sp, #0x14]
	str r1, [sp, #0x18]
	add r1, r5, #0
	add r0, r5, #0
	add r1, #0x40
	mov r2, #0x2a
	mov r3, #0x70
	bl FrontierWriteMsg
	mov r0, #0x4a
	lsl r0, r0, #2
	ldr r0, [r5, r0]
	bl SaveData_GetFrontier
	add r4, r0, #0
	ldrb r0, [r5, #5]
	bl CastleScr_GetRemainderCPRecordID
	add r6, r0, #0
	ldrb r0, [r5, #5]
	bl CastleScr_GetRemainderCPRecordID
	bl Frontier_GetFriendIndex
	add r2, r0, #0
	add r0, r4, #0
	add r1, r6, #0
	bl FrontierRecord_Get
	add r2, r0, #0
	add r0, r5, #0
	mov r1, #0
	bl SetNumber
	mov r1, #0
	str r1, [sp]
	mov r0, #1
	str r0, [sp, #4]
	mov r0, #2
	str r0, [sp, #8]
	str r1, [sp, #0xc]
	str r1, [sp, #0x10]
	str r1, [sp, #0x14]
	add r1, r5, #0
	str r0, [sp, #0x18]
	add r0, r5, #0
	add r1, #0x40
	mov r2, #0x29
	mov r3, #0xe0
	bl FrontierWriteMsg
	add r5, #0x40
	add r0, r5, #0
	bl GF_BGL_BmpWinOnVReq
	add sp, #0x1c
	pop {r3, r4, r5, r6, pc}
}
#endif

static u32 Castle_GetMsg( FRONTIER_MONITOR_WORK* wk )
{
	u8 clear_flag;

	//WIFIのみ特殊
	if( wk->type == CASTLE_TYPE_WIFI_MULTI ){
		clear_flag = FrontierRecord_Get(SaveData_GetFrontier(wk->sv), 
										FRID_CASTLE_MULTI_WIFI_CLEAR_BIT,
										Frontier_GetFriendIndex(FRID_CASTLE_MULTI_WIFI_CLEAR_BIT) );
	}else{
		//クリアしたかフラグを取得
		clear_flag = (u8)CASTLESCORE_GetScoreData(	SaveData_GetCastleScore(wk->sv), 
													CASTLESCORE_ID_CLEAR_FLAG, 
													wk->type, 0, NULL );
	}

	if( clear_flag == 1 ){
		//「げんざい」
		return msg_bf_seiseki_list23;
	}

	//「ぜんかい」
	return msg_bf_seiseki_list09;
}


//==============================================================================================
//
//	ルーレット
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	情報をセット
 *
 * @param	wk		FRONTIER_WORK型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
#ifdef NONEQUIVALENT
static void Roulette_SetInfo( FRONTIER_MONITOR_WORK* wk )
{
	u8 no;
	int msg_id;
	u32 x;

	//----------------------------------------------------------
	no = BR_BMPWIN_TITLE;


	GF_BGL_BmpWinDataFill( &wk->bmpwin[no], FBMP_COL_NULL );			//塗りつぶし

	//「バトルルーレット」
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list05, BF_STR_X, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0 );

	if( wk->type == ROULETTE_TYPE_SINGLE ){
		msg_id = msg_bf_seiseki_list06;
		x = TITLE_STR_SINGLE_X_ROULETTE;
	}else if( wk->type == ROULETTE_TYPE_DOUBLE ){
		msg_id = msg_bf_seiseki_list07;
		x = TITLE_STR_X_ROULETTE;
	}else{
		msg_id = msg_bf_seiseki_list08;
		x = TITLE_STR_X_ROULETTE;
	}

	//「シングルせいせき」
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_id, x, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0 );
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[no] );

	//----------------------------------------------------------
	no = BR_BMPWIN_BEFORE;
	//「ぜんかい」「げんざい」
	EasyMsg( wk, &wk->bmpwin[no], Roulette_GetMsg(wk), 1 );

	//「○○れんしょう」
	SetNumber(	wk, 0, 
				FrontierRecord_Get( wk->f_sv, RouletteScr_GetWinRecordID(wk->type),
									FRONTIER_RECORD_NOT_FRIEND) );
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list22, WIN_NUM_X, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0 );

	GF_BGL_BmpWinOnVReq( &wk->bmpwin[no] );
		
	//----------------------------------------------------------
	no = BR_BMPWIN_MAX;
	//「さいこう」
	EasyMsg( wk, &wk->bmpwin[no], msg_bf_seiseki_list10, 1 );

	//「○○れんしょう」
	SetNumber(	wk, 0, 
				FrontierRecord_Get( wk->f_sv, RouletteScr_GetMaxWinRecordID(wk->type),
									FRONTIER_RECORD_NOT_FRIEND) );
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list22, WIN_NUM_X, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0 );

	GF_BGL_BmpWinOnVReq( &wk->bmpwin[no] );
	return;
}
#else
asm static void Roulette_SetInfo( FRONTIER_MONITOR_WORK* wk )
{
	push {r3, r4, lr}
	sub sp, #0x1c
	add r4, r0, #0
	add r0, #0x10
	mov r1, #0
	bl GF_BGL_BmpWinDataFill
	mov r1, #0
	str r1, [sp]
	mov r0, #1
	str r0, [sp, #4]
	mov r0, #2
	str r0, [sp, #8]
	str r1, [sp, #0xc]
	str r1, [sp, #0x10]
	str r1, [sp, #0x14]
	str r1, [sp, #0x18]
	add r1, r4, #0
	add r0, r4, #0
	add r1, #0x10
	mov r2, #0x19
	mov r3, #8
	bl FrontierWriteMsg
	ldrb r0, [r4, #5]
	cmp r0, #0
	bne _021D1C7C
	mov r2, #0x1a
	mov r3, #0xd8
	b _021D1C8A
_021D1C7C:
	cmp r0, #1
	bne _021D1C86
	mov r2, #0x1b
	mov r3, #0xd8
	b _021D1C8A
_021D1C86:
	mov r2, #0x1c
	mov r3, #0xd8
_021D1C8A:
	mov r1, #0
	str r1, [sp]
	mov r0, #1
	str r0, [sp, #4]
	mov r0, #2
	str r0, [sp, #8]
	str r1, [sp, #0xc]
	str r1, [sp, #0x10]
	str r1, [sp, #0x14]
	add r1, r4, #0
	str r0, [sp, #0x18]
	add r0, r4, #0
	add r1, #0x10
	bl FrontierWriteMsg
	add r0, r4, #0
	add r0, #0x10
	bl GF_BGL_BmpWinOnVReq
	add r0, r4, #0
	bl Roulette_GetMsg
	add r1, r4, #0
	add r2, r0, #0
	add r0, r4, #0
	add r1, #0x20
	mov r3, #1
	bl EasyMsg
	ldrb r0, [r4, #5]
	bl RouletteScr_GetWinRecordID
	add r1, r0, #0
	mov r0, #0x4b
	lsl r0, r0, #2
	ldr r0, [r4, r0]
	mov r2, #0xff
	bl FrontierRecord_Get
	add r2, r0, #0
	add r0, r4, #0
	mov r1, #0
	bl SetNumber
	mov r2, #0
	str r2, [sp]
	mov r1, #1
	str r1, [sp, #4]
	mov r0, #2
	str r0, [sp, #8]
	str r2, [sp, #0xc]
	str r2, [sp, #0x10]
	str r2, [sp, #0x14]
	str r1, [sp, #0x18]
	add r1, r4, #0
	add r0, r4, #0
	add r1, #0x20
	mov r2, #0x2b
	mov r3, #0x70
	bl FrontierWriteMsg
	add r0, r4, #0
	add r0, #0x20
	bl GF_BGL_BmpWinOnVReq
	add r1, r4, #0
	add r0, r4, #0
	add r1, #0x30
	mov r2, #0x1f
	mov r3, #1
	bl EasyMsg
	ldrb r0, [r4, #5]
	bl RouletteScr_GetMaxWinRecordID
	add r1, r0, #0
	mov r0, #0x4b
	lsl r0, r0, #2
	ldr r0, [r4, r0]
	mov r2, #0xff
	bl FrontierRecord_Get
	add r2, r0, #0
	add r0, r4, #0
	mov r1, #0
	bl SetNumber
	mov r2, #0
	str r2, [sp]
	mov r1, #1
	str r1, [sp, #4]
	mov r0, #2
	str r0, [sp, #8]
	str r2, [sp, #0xc]
	str r2, [sp, #0x10]
	str r2, [sp, #0x14]
	str r1, [sp, #0x18]
	add r1, r4, #0
	add r0, r4, #0
	add r1, #0x30
	mov r2, #0x2b
	mov r3, #0x70
	bl FrontierWriteMsg
	add r4, #0x30
	add r0, r4, #0
	bl GF_BGL_BmpWinOnVReq
	add sp, #0x1c
	pop {r3, r4, pc}
	// .align 2, 0
}
#endif

static u32 Roulette_GetMsg( FRONTIER_MONITOR_WORK* wk )
{
	u8 clear_flag;

	//WIFIのみ特殊
	if( wk->type == ROULETTE_TYPE_WIFI_MULTI ){
		clear_flag = FrontierRecord_Get(SaveData_GetFrontier(wk->sv), 
									FRID_ROULETTE_MULTI_WIFI_CLEAR_BIT,
									Frontier_GetFriendIndex(FRID_ROULETTE_MULTI_WIFI_CLEAR_BIT) );
	}else{
		//クリアしたかフラグを取得
		clear_flag = (u8)ROULETTESCORE_GetScoreData(SaveData_GetRouletteScore(wk->sv), 
													ROULETTESCORE_ID_CLEAR_FLAG, 
													wk->type, 0, NULL );
	}

	if( clear_flag == 1 ){
		//「げんざい」
		return msg_bf_seiseki_list23;
	}

	//「ぜんかい」
	return msg_bf_seiseki_list09;
}


//==============================================================================================
//
//	タワー
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	シングル、ダブル情報をセット
 *
 * @param	wk		FRONTIER_WORK型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
#ifdef NONEQUIVALENT
static void Tower_SetInfoSingleDouble( FRONTIER_MONITOR_WORK* wk )
{
	u8 no;
	int msg_id;
	u32 x;

	//----------------------------------------------------------
	no = BT_BMPWIN_TITLE;

	GF_BGL_BmpWinDataFill( &wk->bmpwin[no], FBMP_COL_NULL );			//塗りつぶし

	//「バトルタワー」
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list01, BF_STR_X, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0 );

	if( wk->type == BTWR_MODE_SINGLE ){
		msg_id = msg_bf_seiseki_list06;
		x = TITLE_STR_SINGLE_X_TOWER;
	}else{
		msg_id = msg_bf_seiseki_list07;
		x = TITLE_STR_X_TOWER;
	}

	//「シングルせいせき」
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_id, x, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0 );
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[no] );

	//----------------------------------------------------------
	no = BT_BMPWIN_BEFORE0;
	//「ぜんかい」
	EasyMsg( wk, &wk->bmpwin[no], Tower_GetMsg(wk,wk->type), 1 );

	//「○○れんしょう」
	SetNumber(	wk, 0, 
				FrontierRecord_Get( wk->f_sv, TowerScr_GetWinRecordID(wk->type),
									FRONTIER_RECORD_NOT_FRIEND) );
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list17, WIN_NUM_X, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0 );

	GF_BGL_BmpWinOnVReq( &wk->bmpwin[no] );
		
	//----------------------------------------------------------
	no = BT_BMPWIN_MAX0;
	//「さいこう」
	EasyMsg( wk, &wk->bmpwin[no], msg_bf_seiseki_list10, 1 );

	//「○○れんしょう」
	SetNumber(	wk, 0, 
				FrontierRecord_Get( wk->f_sv, TowerScr_GetMaxWinRecordID(wk->type),
									FRONTIER_RECORD_NOT_FRIEND) );
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list17, WIN_NUM_X, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0 );

	GF_BGL_BmpWinOnVReq( &wk->bmpwin[no] );
	return;
}
#else
asm static void Tower_SetInfoSingleDouble( FRONTIER_MONITOR_WORK* wk )
{
	push {r3, r4, lr}
	sub sp, #0x1c
	add r4, r0, #0
	add r0, #0x10
	mov r1, #0
	bl GF_BGL_BmpWinDataFill
	mov r1, #0
	str r1, [sp]
	mov r0, #1
	str r0, [sp, #4]
	mov r0, #2
	str r0, [sp, #8]
	str r1, [sp, #0xc]
	str r1, [sp, #0x10]
	str r1, [sp, #0x14]
	str r1, [sp, #0x18]
	add r1, r4, #0
	add r0, r4, #0
	add r1, #0x10
	mov r2, #0x15
	mov r3, #8
	bl FrontierWriteMsg
	ldrb r0, [r4, #5]
	cmp r0, #0
	bne _021D1DF6
	mov r2, #0x1a
	b _021D1DF8
_021D1DF6:
	mov r2, #0x1b
_021D1DF8:
	mov r1, #0
	str r1, [sp]
	mov r0, #1
	str r0, [sp, #4]
	mov r0, #2
	str r0, [sp, #8]
	str r1, [sp, #0xc]
	str r1, [sp, #0x10]
	str r1, [sp, #0x14]
	add r1, r4, #0
	str r0, [sp, #0x18]
	mov r3, #0xd8
	add r0, r4, #0
	add r1, #0x10
	bl FrontierWriteMsg
	add r0, r4, #0
	add r0, #0x10
	bl GF_BGL_BmpWinOnVReq
	ldrb r1, [r4, #5]
	add r0, r4, #0
	bl Tower_GetMsg
	add r1, r4, #0
	add r2, r0, #0
	add r0, r4, #0
	add r1, #0x30
	mov r3, #1
	bl EasyMsg
	ldrb r0, [r4, #5]
	bl TowerScr_GetWinRecordID
	add r1, r0, #0
	mov r0, #0x4b
	lsl r0, r0, #2
	ldr r0, [r4, r0]
	mov r2, #0xff
	bl FrontierRecord_Get
	add r2, r0, #0
	add r0, r4, #0
	mov r1, #0
	bl SetNumber
	mov r2, #0
	str r2, [sp]
	mov r1, #1
	str r1, [sp, #4]
	mov r0, #2
	str r0, [sp, #8]
	str r2, [sp, #0xc]
	str r2, [sp, #0x10]
	str r2, [sp, #0x14]
	str r1, [sp, #0x18]
	add r1, r4, #0
	add r0, r4, #0
	add r1, #0x30
	mov r2, #0x26
	mov r3, #0x70
	bl FrontierWriteMsg
	add r0, r4, #0
	add r0, #0x30
	bl GF_BGL_BmpWinOnVReq
	add r1, r4, #0
	add r0, r4, #0
	add r1, #0x40
	mov r2, #0x1f
	mov r3, #1
	bl EasyMsg
	ldrb r0, [r4, #5]
	bl TowerScr_GetMaxWinRecordID
	add r1, r0, #0
	mov r0, #0x4b
	lsl r0, r0, #2
	ldr r0, [r4, r0]
	mov r2, #0xff
	bl FrontierRecord_Get
	add r2, r0, #0
	add r0, r4, #0
	mov r1, #0
	bl SetNumber
	mov r2, #0
	str r2, [sp]
	mov r1, #1
	str r1, [sp, #4]
	mov r0, #2
	str r0, [sp, #8]
	str r2, [sp, #0xc]
	str r2, [sp, #0x10]
	str r2, [sp, #0x14]
	str r1, [sp, #0x18]
	add r1, r4, #0
	add r0, r4, #0
	add r1, #0x40
	mov r2, #0x26
	mov r3, #0x70
	bl FrontierWriteMsg
	add r4, #0x40
	add r0, r4, #0
	bl GF_BGL_BmpWinOnVReq
	add sp, #0x1c
	pop {r3, r4, pc}
}
#endif

//--------------------------------------------------------------
/**
 * @brief	マルチ情報をセット
 *
 * @param	wk		FRONTIER_WORK型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
#ifdef NONEQUIVALENT
static void Tower_SetInfoMulti( FRONTIER_MONITOR_WORK* wk )
{
	u8 no;
	u32 x;

	//----------------------------------------------------------
	no = BT_BMPWIN_TITLE;

	GF_BGL_BmpWinDataFill( &wk->bmpwin[no], FBMP_COL_NULL );			//塗りつぶし

	//「バトルタワー」
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list01, BF_STR_X, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0 );

	//「シングルせいせき」
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list08, TITLE_STR_X_TOWER, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0 );
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[no] );

	//----------------------------------------------------------
	no = BT_BMPWIN_ENTRY1;
	//「トレーナーとマルチ」
	EasyMsg( wk, &wk->bmpwin[no], msg_bf_seiseki_list11, 1 );
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[no] );

	//----------------------------------------------------------
	no = BT_BMPWIN_BEFORE;
	//「ぜんかい」
	EasyMsg( wk, &wk->bmpwin[no], Tower_GetMsg(wk,wk->type), 1 );

	//「○○れんしょう」
	SetNumber(	wk, 0, 
				FrontierRecord_Get( wk->f_sv, TowerScr_GetWinRecordID(wk->type),
									FRONTIER_RECORD_NOT_FRIEND) );
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list17, WIN_NUM_X, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0 );

	GF_BGL_BmpWinOnVReq( &wk->bmpwin[no] );
		
	//----------------------------------------------------------
	no = BT_BMPWIN_MAX;
	//「さいこう」
	EasyMsg( wk, &wk->bmpwin[no], msg_bf_seiseki_list10, 1 );

	//「○○れんしょう」
	SetNumber(	wk, 0, 
				FrontierRecord_Get( wk->f_sv, TowerScr_GetMaxWinRecordID(wk->type),
									FRONTIER_RECORD_NOT_FRIEND) );
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list17, WIN_NUM_X, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0 );

	GF_BGL_BmpWinOnVReq( &wk->bmpwin[no] );

	//----------------------------------------------------------
	no = BT_BMPWIN_ENTRY2;
	//「ともだちとマルチ」
	EasyMsg( wk, &wk->bmpwin[no], msg_bf_seiseki_list12, 1 );
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[no] );

	//----------------------------------------------------------
	no = BT_BMPWIN_BEFORE2;
	//「ぜんかい」
	EasyMsg( wk, &wk->bmpwin[no], Tower_GetMsg(wk,BTWR_MODE_COMM_MULTI), 1 );

	//「○○れんしょう」
	SetNumber(	wk, 0, 
		FrontierRecord_Get( wk->f_sv, TowerScr_GetWinRecordID(BTWR_MODE_COMM_MULTI),
							FRONTIER_RECORD_NOT_FRIEND) );
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list17, WIN_NUM_X, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0 );
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[no] );

	//----------------------------------------------------------
	no = BT_BMPWIN_MAX2;
	//「さいこう」
	EasyMsg( wk, &wk->bmpwin[no], msg_bf_seiseki_list10, 1 );

	//「○○れんしょう」
	SetNumber(	wk, 0, 
		FrontierRecord_Get( wk->f_sv,TowerScr_GetMaxWinRecordID(BTWR_MODE_COMM_MULTI),
							FRONTIER_RECORD_NOT_FRIEND) );
	FrontierWriteMsg(wk, &wk->bmpwin[no], msg_bf_seiseki_list17, WIN_NUM_X, 0,
					FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL, FONT_SYSTEM, 0 );
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[no] );
	return;
}
#else
asm static void Tower_SetInfoMulti( FRONTIER_MONITOR_WORK* wk )
{
	push {r3, r4, lr}
	sub sp, #0x1c
	add r4, r0, #0
	add r0, #0x10
	mov r1, #0
	bl GF_BGL_BmpWinDataFill
	mov r1, #0
	str r1, [sp]
	mov r0, #1
	str r0, [sp, #4]
	mov r0, #2
	str r0, [sp, #8]
	str r1, [sp, #0xc]
	str r1, [sp, #0x10]
	str r1, [sp, #0x14]
	str r1, [sp, #0x18]
	add r1, r4, #0
	add r0, r4, #0
	add r1, #0x10
	mov r2, #0x15
	mov r3, #8
	bl FrontierWriteMsg
	mov r1, #0
	str r1, [sp]
	mov r0, #1
	str r0, [sp, #4]
	mov r0, #2
	str r0, [sp, #8]
	str r1, [sp, #0xc]
	str r1, [sp, #0x10]
	str r1, [sp, #0x14]
	add r1, r4, #0
	str r0, [sp, #0x18]
	add r0, r4, #0
	add r1, #0x10
	mov r2, #0x1c
	mov r3, #0xd8
	bl FrontierWriteMsg
	add r0, r4, #0
	add r0, #0x10
	bl GF_BGL_BmpWinOnVReq
	add r1, r4, #0
	add r0, r4, #0
	add r1, #0x20
	mov r2, #0x20
	mov r3, #1
	bl EasyMsg
	add r0, r4, #0
	add r0, #0x20
	bl GF_BGL_BmpWinOnVReq
	ldrb r1, [r4, #5]
	add r0, r4, #0
	bl Tower_GetMsg
	add r1, r4, #0
	add r2, r0, #0
	add r0, r4, #0
	add r1, #0x50
	mov r3, #1
	bl EasyMsg
	ldrb r0, [r4, #5]
	bl TowerScr_GetWinRecordID
	add r1, r0, #0
	mov r0, #0x4b
	lsl r0, r0, #2
	ldr r0, [r4, r0]
	mov r2, #0xff
	bl FrontierRecord_Get
	add r2, r0, #0
	add r0, r4, #0
	mov r1, #0
	bl SetNumber
	mov r2, #0
	str r2, [sp]
	mov r1, #1
	str r1, [sp, #4]
	mov r0, #2
	str r0, [sp, #8]
	str r2, [sp, #0xc]
	str r2, [sp, #0x10]
	str r2, [sp, #0x14]
	str r1, [sp, #0x18]
	add r1, r4, #0
	add r0, r4, #0
	add r1, #0x50
	mov r2, #0x26
	mov r3, #0x70
	bl FrontierWriteMsg
	add r0, r4, #0
	add r0, #0x50
	bl GF_BGL_BmpWinOnVReq
	add r1, r4, #0
	add r0, r4, #0
	add r1, #0x60
	mov r2, #0x1f
	mov r3, #1
	bl EasyMsg
	ldrb r0, [r4, #5]
	bl TowerScr_GetMaxWinRecordID
	add r1, r0, #0
	mov r0, #0x4b
	lsl r0, r0, #2
	ldr r0, [r4, r0]
	mov r2, #0xff
	bl FrontierRecord_Get
	add r2, r0, #0
	add r0, r4, #0
	mov r1, #0
	bl SetNumber
	mov r2, #0
	str r2, [sp]
	mov r1, #1
	str r1, [sp, #4]
	mov r0, #2
	str r0, [sp, #8]
	str r2, [sp, #0xc]
	str r2, [sp, #0x10]
	str r2, [sp, #0x14]
	str r1, [sp, #0x18]
	add r1, r4, #0
	add r0, r4, #0
	add r1, #0x60
	mov r2, #0x26
	mov r3, #0x70
	bl FrontierWriteMsg
	add r0, r4, #0
	add r0, #0x60
	bl GF_BGL_BmpWinOnVReq
	add r1, r4, #0
	add r0, r4, #0
	add r1, #0x70
	mov r2, #0x21
	mov r3, #1
	bl EasyMsg
	add r0, r4, #0
	add r0, #0x70
	bl GF_BGL_BmpWinOnVReq
	add r0, r4, #0
	mov r1, #3
	bl Tower_GetMsg
	add r1, r4, #0
	add r2, r0, #0
	add r0, r4, #0
	add r1, #0x80
	mov r3, #1
	bl EasyMsg
	mov r0, #3
	bl TowerScr_GetWinRecordID
	add r1, r0, #0
	mov r0, #0x4b
	lsl r0, r0, #2
	ldr r0, [r4, r0]
	mov r2, #0xff
	bl FrontierRecord_Get
	add r2, r0, #0
	add r0, r4, #0
	mov r1, #0
	bl SetNumber
	mov r2, #0
	str r2, [sp]
	mov r1, #1
	str r1, [sp, #4]
	mov r0, #2
	str r0, [sp, #8]
	str r2, [sp, #0xc]
	str r2, [sp, #0x10]
	str r2, [sp, #0x14]
	str r1, [sp, #0x18]
	add r1, r4, #0
	add r0, r4, #0
	add r1, #0x80
	mov r2, #0x26
	mov r3, #0x70
	bl FrontierWriteMsg
	add r0, r4, #0
	add r0, #0x80
	bl GF_BGL_BmpWinOnVReq
	add r1, r4, #0
	add r0, r4, #0
	add r1, #0x90
	mov r2, #0x1f
	mov r3, #1
	bl EasyMsg
	mov r0, #3
	bl TowerScr_GetMaxWinRecordID
	add r1, r0, #0
	mov r0, #0x4b
	lsl r0, r0, #2
	ldr r0, [r4, r0]
	mov r2, #0xff
	bl FrontierRecord_Get
	add r2, r0, #0
	add r0, r4, #0
	mov r1, #0
	bl SetNumber
	mov r2, #0
	str r2, [sp]
	mov r1, #1
	str r1, [sp, #4]
	mov r0, #2
	str r0, [sp, #8]
	str r2, [sp, #0xc]
	str r2, [sp, #0x10]
	str r2, [sp, #0x14]
	str r1, [sp, #0x18]
	add r1, r4, #0
	add r0, r4, #0
	add r1, #0x90
	mov r2, #0x26
	mov r3, #0x70
	bl FrontierWriteMsg
	add r4, #0x90
	add r0, r4, #0
	bl GF_BGL_BmpWinOnVReq
	add sp, #0x1c
	pop {r3, r4, pc}
	// .align 2, 0
}
#endif
    
static u32 Tower_GetMsg( FRONTIER_MONITOR_WORK* wk, u8 type )
{
	u16 id;
	int flag;
	BTLTOWER_SCOREWORK* scoreSave;

	scoreSave = SaveData_GetTowerScoreData( wk->sv );

	//モードによって必要なIDを取得
	switch( type ){
	case BTWR_MODE_SINGLE:
		id = BTWR_SFLAG_SINGLE_RECORD;
		break;

	case BTWR_MODE_DOUBLE:
		id = BTWR_SFLAG_DOUBLE_RECORD;
		break;

	case BTWR_MODE_MULTI:
		id = BTWR_SFLAG_MULTI_RECORD;
		break;

	case BTWR_MODE_COMM_MULTI:
		id = BTWR_SFLAG_CMULTI_RECORD;
		break;

	case BTWR_MODE_WIFI:
		id = BTWR_SFLAG_WIFI_RECORD;
		break;

	//case BTWR_MODE_RETRY:
	//	break;

	case BTWR_MODE_WIFI_MULTI:		//07.08.06 プラチナ追加(COMM_MULTIのWIFI版 友達手帳)
		id = BTWR_SFLAG_WIFI_MULTI_RECORD;		//プラチナ追加
		break;

	//エラー回避
	default:
		id = BTWR_SFLAG_SINGLE_RECORD;
		break;
	};

	flag = TowerScoreData_SetFlags( scoreSave, id, BTWR_DATA_get );
	OS_Printf( "タワーのクリア状態 = %d\n", flag );

	if( flag == 1 ){
		//「げんざい」
		return msg_bf_seiseki_list23;
	}

	//「ぜんかい」
	return msg_bf_seiseki_list09;
}


