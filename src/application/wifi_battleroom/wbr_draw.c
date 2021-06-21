//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wbr_draw.c
 *	@brief		wifi	バトルルーム
 *	@author		tomoya takahashi
 *	@data		2007.02.15
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"
#include "system/clact_util.h"
#include "system/wipe.h"
#include "system/render_oam.h"

#define __WBR_DRAW_H_GLOBAL
#include "wbr_draw.h"

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
//-------------------------------------
///	メインシーケンス
//=====================================
enum{
	WBR_MAINSEQ_WIPEIN,	
	WBR_MAINSEQ_WIPEIN_WAIT,	
	WBR_MAINSEQ_MAIN,
	WBR_MAINSEQ_WIPEOUT,
	WBR_MAINSEQ_WIPEOUT_WAIT,
};

#define WBR_CHARMAN_CONTNUM	( 32 )	// キャラクタマネージャ管理数
#define WBR_PLTTMAN_CONTNUM	( 32 )	// キャラクタマネージャ管理数
#define WBR_CLACT_CONTNUM	( 64 )	// セルアクター管理数
#define WBR_CHARVRAM_MSIZE	( 128*1024 )	// OAMキャラクタサイズ
#define WBR_CHARVRAM_SSIZE	( 16*1024 )	// OAMキャラクタサイズ
#define WBR_BGFRAME_BACK	( GF_BGL_FRAME0_M )	// 背景面
#define WBR_BGFRAME_MSG		( GF_BGL_FRAME1_M )	// メッセージ面




//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	グラフィック関係ワーク
//=====================================
typedef struct {
	GF_BGL_INI*	p_bgl;
	CLACT_SET_PTR	p_clset;
	CLACT_U_EASYRENDER_DATA	renddata;
} WBR_GRAPHIC;


//-------------------------------------
///	wifi	バトルルーム	ワーク
//=====================================
typedef struct {
	WBR_GRAPHIC	graphic;	// グラフィックデータ
} WBR_WK;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
static void Wbr_Vblank( void* p_wk );


// グラフィック関係
static void Wbr_GraphicInit( WBR_GRAPHIC* p_gra, u32 heapID );
static void Wbr_GraphicExit( WBR_GRAPHIC* p_gra );
static void Wbr_GraphicVblank( WBR_GRAPHIC* p_gra );
static void Wbr_BankInit( void );
static void Wbr_BankExit( void );
static void Wbr_BgInit( WBR_GRAPHIC* p_gra, u32 heapID );
static void Wbr_BgExit( WBR_GRAPHIC* p_gra );
static void Wbr_BgLoad( WBR_GRAPHIC* p_gra, u32 heapID );
static void Wbr_BgRelease( WBR_GRAPHIC* p_gra );
static void Wbr_ClactInit( WBR_GRAPHIC* p_gra, u32 heapID );
static void Wbr_ClactExit( WBR_GRAPHIC* p_gra );
static void Wbr_ClactLoad( WBR_GRAPHIC* p_gra, u32 heapID );
static void Wbr_ClactRelease( WBR_GRAPHIC* p_gra );

//----------------------------------------------------------------------------
/**
 *	@brief	プロセス初期化
 *
 *	@param	proc	ワーク
 *	@param	seq		シーケンスポインタ
 *
 *	@retval	PROC_RES_CONTINUE = 0,		///<動作継続中
 *	@retval	PROC_RES_FINISH,			///<動作終了
 */
//-----------------------------------------------------------------------------
PROC_RESULT WbrProc_Init( PROC *proc,int *seq)
{
	WBR_WK* p_wk = NULL;
	WBR_PROC_PARAM* p_pp = (WBR_PROC_PARAM*)PROC_GetParentWork(proc);

	// プロセスワーク作成
	p_wk = PROC_AllocWork(proc,sizeof(WBR_WK),HEAPID_WIFI_BATTLEROOM);
	MI_CpuClear8(p_wk,sizeof(WBR_WK));
	
	// 割り込み設定
	sys_VBlankFuncChange( Wbr_Vblank, p_pp );	// VBlankセット
	sys_HBlankIntrStop();	//HBlank割り込み停止

	// グラフィック初期化
	Wbr_GraphicInit( &p_wk->graphic, HEAPID_WIFI_BATTLEROOM );

	return PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	プロセスメイン
 *
 *	@param	proc	ワーク
 *	@param	seq		シーケンスポインタ
 *
 *	@retval	PROC_RES_CONTINUE = 0,		///<動作継続中
 *	@retval	PROC_RES_FINISH,			///<動作終了
 */
//-----------------------------------------------------------------------------
PROC_RESULT WbrProc_Main( PROC *proc,int *seq)
{
	WBR_WK* p_wk = PROC_GetWork(proc);
	WBR_PROC_PARAM* p_pp = (WBR_PROC_PARAM*)PROC_GetParentWork(proc);

	switch( *seq ){
	case WBR_MAINSEQ_WIPEIN:	
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_DOORIN, WIPE_TYPE_FADEIN, 
				WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_WIFI_BATTLEROOM );
		(*seq)++;
		break;
		
	case WBR_MAINSEQ_WIPEIN_WAIT:	
		if( WIPE_SYS_EndCheck() == TRUE ){
			(*seq)++;
		}
		break;
		
	case WBR_MAINSEQ_MAIN:
		if( sys.trg & PAD_BUTTON_A ){
			(*seq)++;
		}
		break;
		
	case WBR_MAINSEQ_WIPEOUT:
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_DOOROUT, WIPE_TYPE_FADEOUT, 
				WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_WIFI_BATTLEROOM );
		(*seq)++;
		break;
		
	case WBR_MAINSEQ_WIPEOUT_WAIT:
		if( WIPE_SYS_EndCheck() == TRUE ){
			return PROC_RES_FINISH;
		}
		break;
		
	default:
		break;
	}

	return PROC_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	プロセス終了
 *
 *	@param	proc	ワーク
 *	@param	seq		シーケンスポインタ
 *
 *	@retval	PROC_RES_CONTINUE = 0,		///<動作継続中
 *	@retval	PROC_RES_FINISH,			///<動作終了
 */
//-----------------------------------------------------------------------------
PROC_RESULT WbrProc_End( PROC *proc,int *seq)
{
	WBR_WK* p_wk = PROC_GetWork(proc);
	WBR_PROC_PARAM* p_pp = (WBR_PROC_PARAM*)PROC_GetParentWork(proc);


	// グラフィック破棄
	Wbr_GraphicExit( &p_wk->graphic );	

	// プロセスワーク破棄
	PROC_FreeWork(proc);

	return PROC_RES_FINISH;
}



//-----------------------------------------------------------------------------
/**
 *				プライベート関数郡
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief	VBlank関数
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void Wbr_Vblank( void* p_wk )
{
	WBR_WK* p_wbr = p_wk;

	// グラフィックVブランク関数
	Wbr_GraphicVblank( &p_wbr->graphic );
}


//----------------------------------------------------------------------------
/**
 *	@brief	グラフィック初期化
 *
 *	@param	p_gra		グラフィックワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void Wbr_GraphicInit( WBR_GRAPHIC* p_gra, u32 heapID )
{
	// バンク設定
	Wbr_BankInit();

	// BGL初期化
	Wbr_BgInit( p_gra, heapID );

	// セルアクター初期化
	Wbr_ClactInit( p_gra, heapID );

	// BG読み込み
	Wbr_BgLoad( p_gra, heapID );

	// セルアクター読み込み
	Wbr_ClactLoad( p_gra, heapID );
}


//----------------------------------------------------------------------------
/**
 *	@brief	グラフィック破棄
 *
 *	@param	p_gra		グラフィックワーク
 */
//-----------------------------------------------------------------------------
static void Wbr_GraphicExit( WBR_GRAPHIC* p_gra )
{
	// BG読み込み
	Wbr_BgRelease( p_gra );

	// セルアクター読み込み
	Wbr_ClactRelease( p_gra );

	// BGL初期化
	Wbr_BgExit( p_gra );

	// セルアクター初期化
	Wbr_ClactExit( p_gra );

	// バンク設定
	Wbr_BankExit();
}

//----------------------------------------------------------------------------
/**
 *	@brief	VBlank関数
 *
 *	@param	p_gra	グラフィックワーク
 */
//-----------------------------------------------------------------------------
static void Wbr_GraphicVblank( WBR_GRAPHIC* p_gra )
{
    // BG書き換え
	GF_BGL_VBlankFunc( p_gra->p_bgl );

    // レンダラ共有OAMマネージャVram転送
    REND_OAMTrans();
}

//----------------------------------------------------------------------------
/**
 *	@brief	バンク設定
 */
//-----------------------------------------------------------------------------
static void Wbr_BankInit( void )
{
    GF_BGL_DISPVRAM tbl = {
        GX_VRAM_BG_256_AC,				// メイン2DエンジンのBG
        GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット
        GX_VRAM_SUB_BG_32_H,			// サブ2DエンジンのBG
        GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット
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
 *	@brief	バンク設定破棄
 */
//-----------------------------------------------------------------------------
static void Wbr_BankExit( void )
{
	// VRAM設定リセット
	GX_ResetBankForBG();			// メイン2DエンジンのBG
	GX_ResetBankForBGExtPltt();		// メイン2DエンジンのBG拡張パレット
	GX_ResetBankForSubBG();			// サブ2DエンジンのBG
	GX_ResetBankForSubBGExtPltt();	// サブ2DエンジンのBG拡張パレット
	GX_ResetBankForOBJ();			// メイン2DエンジンのOBJ
	GX_ResetBankForOBJExtPltt();	// メイン2DエンジンのOBJ拡張パレット
	GX_ResetBankForSubOBJ();		// サブ2DエンジンのOBJ
	GX_ResetBankForSubOBJExtPltt();	// サブ2DエンジンのOBJ拡張パレット
	GX_ResetBankForTex();			// テクスチャイメージ
	GX_ResetBankForTexPltt();		// テクスチャパレット
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG設定
 *
 *	@param	p_gra	グラフィックワーク
 *	@param	heapID	ヒープID
 */
//-----------------------------------------------------------------------------
static void Wbr_BgInit( WBR_GRAPHIC* p_gra, u32 heapID )
{
	GF_ASSERT( p_gra->p_bgl == NULL );
	
	// BGL作成
	p_gra->p_bgl = GF_BGL_BglIniAlloc( heapID );

	
	{
		GF_BGL_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0, GX_BG0_AS_3D
		};
		GF_BGL_InitBG(&BGsys_data);
	}

	{	// 背景
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x1000, 0, GF_BGL_SCRSIZ_512x256, GX_BG_COLORMODE_256,
			GX_BG_SCRBASE_0xc000, GX_BG_CHARBASE_0x20000, GX_BG_EXTPLTT_01,
			3, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( p_gra->p_bgl, WBR_BGFRAME_BACK, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ClearCharSet( WBR_BGFRAME_BACK, 32, 0, heapID );
		GF_BGL_ScrClear( p_gra->p_bgl, WBR_BGFRAME_BACK );
	}

	{	// メッセージ
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xc000, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( p_gra->p_bgl, WBR_BGFRAME_MSG, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ClearCharSet( WBR_BGFRAME_MSG, 32, 0, heapID );
		GF_BGL_ScrClear( p_gra->p_bgl, WBR_BGFRAME_MSG );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG設定破棄
 *
 *	@param	p_gra	グラフィックワーク
 */
//-----------------------------------------------------------------------------
static void Wbr_BgExit( WBR_GRAPHIC* p_gra )
{
    GF_BGL_BGControlExit( p_gra->p_bgl, WBR_BGFRAME_BACK );
    GF_BGL_BGControlExit( p_gra->p_bgl, WBR_BGFRAME_MSG	 );
	sys_FreeMemoryEz( p_gra->p_bgl );
	p_gra->p_bgl = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	BGデータ読み込み
 *
 *	@param	p_gra	グラフィックワーク
 *	@param	heapID	ヒープID
 */
//-----------------------------------------------------------------------------
static void Wbr_BgLoad( WBR_GRAPHIC* p_gra, u32 heapID )
{
}

//----------------------------------------------------------------------------
/**
 *	@brief	BGデータ破棄
 *
 *	@param	p_gra	グラフィックワーク
 */
//-----------------------------------------------------------------------------
static void Wbr_BgRelease( WBR_GRAPHIC* p_gra )
{
}

//----------------------------------------------------------------------------
/**
 *	@brief	セルアクターシステム初期化
 *
 *	@param	p_gra	グラフィックワーク
 *	@param	heapID	ヒープID
 */
//-----------------------------------------------------------------------------
static void Wbr_ClactInit( WBR_GRAPHIC* p_gra, u32 heapID )
{
	// OAMマネージャ初期化
    NNS_G2dInitOamManagerModule();

    // 共有OAMマネージャ作成
    // レンダラ用OAMマネージャ作成
    // ここで作成したOAMマネージャをみんなで共有する
    REND_OAMInit(
        0, 124,		// メイン画面OAM管理領域
        0, 31,		// メイン画面アフィン管理領域
        0, 124,		// サブ画面OAM管理領域
        0, 31,		// サブ画面アフィン管理領域
		heapID );
	
	// キャラクタマネージャー初期化
	{
		CHAR_MANAGER_MAKE cm = {
			WBR_CHARMAN_CONTNUM,
			WBR_CHARVRAM_MSIZE,
			WBR_CHARVRAM_SSIZE,
			0
		};
		cm.heap = heapID;
		InitCharManagerReg(&cm, GX_OBJVRAMMODE_CHAR_1D_128K, GX_OBJVRAMMODE_CHAR_1D_32K );
	}
	// パレットマネージャー初期化
	InitPlttManager(WBR_PLTTMAN_CONTNUM, heapID);

	// 読み込み開始位置を初期化
	CharLoadStartAll();
	PlttLoadStartAll();

	// 通信アイコン用にキャラ＆パレット制限
	CLACT_U_WmIcon_SetReserveAreaCharManager(NNS_G2D_VRAM_TYPE_2DMAIN, GX_OBJVRAMMODE_CHAR_1D_128K);
	CLACT_U_WmIcon_SetReserveAreaPlttManager(NNS_G2D_VRAM_TYPE_2DMAIN);

	// セルアクターセット作成
    p_gra->p_clset = CLACT_U_SetEasyInit( WBR_CLACT_CONTNUM, &p_gra->renddata, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	セルアクターシステム破棄
 *
 *	@param	p_gra	グラフィックデータワーク
 */
//-----------------------------------------------------------------------------
static void Wbr_ClactExit( WBR_GRAPHIC* p_gra )
{
    //OAMレンダラー破棄
    REND_OAM_Delete();

    // リソース解放
    DeleteCharManager();
    DeletePlttManager();

	// セルアクターセット破棄
    CLACT_DestSet(p_gra->p_clset);
}

//----------------------------------------------------------------------------
/**
 *	@brief	セルアクターデータ読み込み
 *
 *	@param	p_gra		グラフィックデータワーク
 *	@param	heapID		ヒープID
 */	
//-----------------------------------------------------------------------------
static void Wbr_ClactLoad( WBR_GRAPHIC* p_gra, u32 heapID )
{
}

//----------------------------------------------------------------------------
/**
 *	@brief	セルアクターデータ破棄
 *
 *	@param	p_gra		グラフィックデータワーク
 */
//-----------------------------------------------------------------------------
static void Wbr_ClactRelease( WBR_GRAPHIC* p_gra )
{
}


