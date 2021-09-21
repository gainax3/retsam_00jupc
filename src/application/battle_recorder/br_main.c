//==============================================================================
/**
 * @file	br_main.c
 * @brief	バトルレコーダー
 * @author	goto
 * @date	2007.07.26(木)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
#include "common.h"
#include "system/procsys.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "system/pm_str.h"
#include "system/clact_tool.h"
#include "system/msgdata_util.h"
#include "system/wordset.h"
#include "system/window.h"
#include "msgdata/msg.naix"
#include "msgdata/msg_fightmsg_dp.h"
#include "system/wipe.h"
#include "system/brightness.h"
#include "system/fontproc.h"
#include "system/lib_pack.h"
#include "system/fontoam.h"
#include "system/msg_ds_icon.h"
#include "gflib/msg_print.h"
#include "gflib/touchpanel.h"
#include "poketool/poke_tool.h"
#include "poketool/monsno.h"
#include "poketool/pokeicon.h"
#include "poketool/boxdata.h"
#include "system/bmp_menu.h"
#include "system/snd_tool.h"
#include "gflib/strbuf_family.h"

#include "system/pmfprint.h"

#include "include/savedata/misc.h"

#include "br_private.h"

static void BR_VramBankSet( GF_BGL_INI* bgl );
static void BR_CATS_Init( BR_WORK* wk );

//--------------------------------------------------------------
/**
 * @brief	システム初期化
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_SystemInit( BR_WORK* wk )
{
	///< 初期化
	sys_VBlankFuncChange( NULL, NULL );		///< VBlankセット
	sys_HBlankIntrStop();					///< HBlank割り込み停止
	GF_Disp_GX_VisibleControlInit();
	GF_Disp_GXS_VisibleControlInit();
	GX_SetVisiblePlane( 0 );
	GXS_SetVisiblePlane( 0 );
	
	initVramTransferManagerHeap( 4, HEAPID_BR );
	
	///< システムワークの作成	
	wk->sys.p_handle = ArchiveDataHandleOpen( ARC_BR_GRA, HEAPID_BR );	
	wk->sys.bgl		 = GF_BGL_BglIniAlloc( HEAPID_BR );
	wk->sys.pfd		 = PaletteFadeInit( HEAPID_BR );
	wk->sys.g3Dman	 = GF_G3DMAN_Init( HEAPID_BR, GF_G3DMAN_LNK, GF_G3DTEX_128K, GF_G3DMAN_LNK, GF_G3DPLT_64K, NULL );
	wk->sys.ssm_p	 = SoftSpriteInit( HEAPID_BR );
	NNS_G2dSetupSoftwareSpriteCamera();
	
	PaletteTrans_AutoSet( wk->sys.pfd, TRUE );
	PaletteFadeWorkAllocSet( wk->sys.pfd, FADE_MAIN_BG,	 0x200, HEAPID_BR );
	PaletteFadeWorkAllocSet( wk->sys.pfd, FADE_SUB_BG,	 0x200, HEAPID_BR );
	PaletteFadeWorkAllocSet( wk->sys.pfd, FADE_MAIN_OBJ, 0x200, HEAPID_BR );
	PaletteFadeWorkAllocSet( wk->sys.pfd, FADE_SUB_OBJ,	 0x200, HEAPID_BR );
	
	///< 設定とか
	BR_VramBankSet( wk->sys.bgl );
	BR_CATS_Init( wk );
	
	///< Touch Panel
	{
		InitTPSystem();
		InitTPNoBuff( 4 );
		
		wk->sys.touch = INPUT_MODE_DEF;
		
		BR_TouchPanel_Init( wk );
		
		MsgPrintTouchPanelFlagSet( MSG_TP_ON );
	}
	
	///< VBlank
	sys_VBlankFuncChange( BR_VBlank, wk );
	
	///< 通信初期化関連
	if ( wk->mode != BR_MODE_BROWSE )
	{
		BR_GDSRAP_Init( wk );		

	    CLACT_U_WmIcon_SetReserveAreaCharManager( NNS_G2D_VRAM_TYPE_2DMAIN, GX_OBJVRAMMODE_CHAR_1D_128K );
	    CLACT_U_WmIcon_SetReserveAreaPlttManager( NNS_G2D_VRAM_TYPE_2DMAIN );
	    CLACT_U_WmIcon_SetReserveAreaCharManager( NNS_G2D_VRAM_TYPE_2DSUB, GX_OBJVRAMMODE_CHAR_1D_128K );
	    CLACT_U_WmIcon_SetReserveAreaPlttManager( NNS_G2D_VRAM_TYPE_2DSUB );
		WirelessIconEasy();
		WirelessIconEasy_HoldLCD( TRUE, HEAPID_BR );
		{
			NNSG2dPaletteData *palData;
			void* dat = WirelessIconPlttResGet( HEAPID_BR );

			NNS_G2dGetUnpackedPaletteData( dat, &palData );
			PaletteWorkSet( wk->sys.pfd, palData->pRawData, FADE_MAIN_OBJ, 14 * 16, 32 );	/// last = palofs, palsize
			PaletteWorkSet( wk->sys.pfd, palData->pRawData, FADE_SUB_OBJ,  14 * 16, 32 );	/// last = palofs, palsize

			sys_FreeMemoryEz( dat );
		}
	}
	
	///< cursor
	{
		CURSOR_PARAM param;
		param.use		= CUR_MAX;
		param.heap_id	= HEAPID_BR;
		param.d_area	= CATS_D_AREA_MAIN;
		param.type		= wk->mode;
		param.surface_y	= SUB_SURFACE_FX_Y_BR;
		
		wk->cur_wk = Cursor_SystemCreate( param, wk->sys.csp, wk->sys.crp, wk->sys.pfd );
		Cursor_Create( wk->cur_wk, wk->sys.color_type );
		Cursor_Active( wk->cur_wk, FALSE );//TRUE );		
		Cursor_Visible( wk->cur_wk, FALSE );
		
		param.d_area	= CATS_D_AREA_SUB;
		param.surface_y	= SUB_SURFACE_FX_Y_BR;
		
		wk->cur_wk_top = Cursor_SystemCreate( param, wk->sys.csp, wk->sys.crp, wk->sys.pfd );
		Cursor_Create( wk->cur_wk_top, wk->sys.color_type );
		Cursor_Active( wk->cur_wk_top, FALSE );		
		Cursor_Visible( wk->cur_wk_top, FALSE );
	}
	
//	wk->tag_man.item_max = TAG_MAX;
//	wk->tag_man.stack_s	 = 0;

	///< font oam
	{
		wk->sys.fontoam_sys = FONTOAM_SysInit( ( TAG_MAX * 2 ) + 10, HEAPID_BR );
		wk->sys.man			= MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_battle_rec_dat, HEAPID_BR );
		wk->sys.man_ranking	= MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_br_ranking_dat, HEAPID_BR );
	//	FontProc_LoadFont( FONT_SYSTEM, HEAPID_BR );
	}
	
	///< 録画データの読みこみ
	BR_SaveData_GppHead_Load( wk );

	///< メニュー設定
	BattleRecorder_MenuSetting( wk );
	
	///< 背景読み込み
	BR_disp_BG_Load( wk );
	
	BR_DrawTCB_Add( wk );
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_SystemExit( BR_WORK* wk )
{	
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0,  VISIBLE_OFF );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1,  VISIBLE_OFF );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2,  VISIBLE_OFF );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3,  VISIBLE_OFF );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );
	GF_BGL_BGControlExit( wk->sys.bgl, GF_BGL_FRAME0_M );
	GF_BGL_BGControlExit( wk->sys.bgl, GF_BGL_FRAME1_M );
	GF_BGL_BGControlExit( wk->sys.bgl, GF_BGL_FRAME2_M );
	GF_BGL_BGControlExit( wk->sys.bgl, GF_BGL_FRAME3_M );
	GF_BGL_BGControlExit( wk->sys.bgl, GF_BGL_FRAME0_S );
	GF_BGL_BGControlExit( wk->sys.bgl, GF_BGL_FRAME1_S );
	GF_BGL_BGControlExit( wk->sys.bgl, GF_BGL_FRAME2_S );
	GF_BGL_BGControlExit( wk->sys.bgl, GF_BGL_FRAME3_S );
	sys_FreeMemoryEz( wk->sys.bgl );
	
	PaletteFadeWorkAllocFree( wk->sys.pfd, FADE_MAIN_BG );
	PaletteFadeWorkAllocFree( wk->sys.pfd, FADE_SUB_BG );
	PaletteFadeWorkAllocFree( wk->sys.pfd, FADE_MAIN_OBJ );
	PaletteFadeWorkAllocFree( wk->sys.pfd, FADE_SUB_OBJ );
	PaletteFadeFree( wk->sys.pfd );
	
	
	///< レコーダーカラーの保存
	if ( wk->mode == BR_MODE_BROWSE )
	{
		MISC* misc = SaveData_GetMisc( wk->save );
		MISC_SetBattleRecoderColor( misc, wk->sys.color_type );
	}
	
	ArchiveDataHandleClose( wk->sys.p_handle );
	
	TCB_Delete( wk->view_tcb );
	
	CATS_ResourceDestructor_S( wk->sys.csp, wk->sys.crp );
	CATS_FreeMemory( wk->sys.csp );
	
	WirelessIconEasyEnd();
			
	{
		u32 active;
		active = StopTP();
		
		BMN_Delete( wk->sys.btn );
		
		MsgPrintTouchPanelFlagSet( MSG_TP_OFF );
	}

	{
		int i;
		
		for ( i = 0; i < 4; i++ ){
			
			if ( wk->br_head[ i ] != NULL ){
				sys_FreeMemoryEz( wk->br_head[ i ] );
			}
			
			if ( wk->br_gpp[ i ] != NULL ){
				sys_FreeMemoryEz( wk->br_gpp[ i ] );
			}
		}
	}
	
	///< font oam
	{
	//	FontProc_UnloadFont( FONT_SYSTEM );	
		FONTOAM_SysDelete( wk->sys.fontoam_sys );
		MSGMAN_Delete( wk->sys.man );
		MSGMAN_Delete( wk->sys.man_ranking );
	}
	GF_G3D_Exit( wk->sys.g3Dman );
	SoftSpriteEnd( wk->sys.ssm_p );

	BR_GDSRAP_Exit( wk );
	
	sys_VBlankFuncChange( NULL, NULL );
	
	DellVramTransferManager();
}

static const GF_BGL_BGCNT_HEADER TextBgCntDat1[] = {
	///<FRAME0_M
	{
		0, 0, 0x0800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
		eBR_BG0_PRI, 0, 0, FALSE
	},
	///<FRAME1_M
	{
		0, 0, 0x0800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x2800, GX_BG_CHARBASE_0x14000, GX_BG_EXTPLTT_01,
		eBR_BG1_PRI, 0, 0, FALSE
	},
	///<FRAME2_M
	{
		0, 0, 0x0800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x3000, GX_BG_CHARBASE_0x18000, GX_BG_EXTPLTT_01,
		eBR_BG2_PRI, 0, 0, FALSE
	},
	///<FRAME3_M	MSG
	{
		0, 0, 0x0800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x3800, GX_BG_CHARBASE_0x1c000, GX_BG_EXTPLTT_01,
		eBR_BG3_PRI, 0, 0, FALSE
	},
};

static const GF_BGL_BGCNT_HEADER TextBgCntDat2[] = {
	///<FRAME0_S
	{
		0, 0, 0x0800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xc000, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
		eBR_BG0_PRI, 0, 0, FALSE
	},
	///<FRAME1_S
	{
		0, 0, 0x0800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xc800, GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01,
		eBR_BG1_PRI, 0, 0, FALSE
	},
	///<FRAME2_S
	{
		0, 0, 0x0800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xd000, GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_01,
		eBR_BG2_PRI, 0, 0, FALSE
	},
	///<FRAME3_S	MSG
	{
		0, 0, 0x0800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0xd800, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
		eBR_BG3_PRI, 0, 0, FALSE
	},
};

//--------------------------------------------------------------
/**
 * @brief	Bank設定
 *
 * @param	bgl	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BR_VramBankSet( GF_BGL_INI* bgl )
{
	GF_Disp_GX_VisibleControlInit();
	
	{	// BG SYSTEM
		GF_BGL_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_3D,
		};
		GF_BGL_InitBG( &BGsys_data );
	}
	
	//VRAM設定
	{
		GF_BGL_DISPVRAM vramSetTable = {
			GX_VRAM_BG_128_A,				// メイン2DエンジンのBG
			GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット
			GX_VRAM_SUB_BG_128_C,
//			GX_VRAM_SUB_BG_32_H,			// サブ2DエンジンのBG
			GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット
			GX_VRAM_OBJ_64_E,				// メイン2DエンジンのOBJ
			GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット
//			GX_VRAM_SUB_OBJ_128_D,
			GX_VRAM_SUB_OBJ_16_I,			// サブ2DエンジンのOBJ
			GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット
//			GX_VRAM_TEX_NONE,
//			GX_VRAM_TEXPLTT_NONE,
			GX_VRAM_TEX_0_B,				// テクスチャイメージスロット
			GX_VRAM_TEXPLTT_01_FG			// テクスチャパレットスロット
		};
		GF_Disp_SetBank( &vramSetTable );

		//VRAMクリア
		MI_CpuClear32((void*)HW_BG_VRAM, HW_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_BG_VRAM, HW_DB_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_OBJ_VRAM, HW_OBJ_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_OBJ_VRAM, HW_DB_OBJ_VRAM_SIZE);
	}
	
		//メイン画面フレーム設定
	{
		GF_BGL_BGControlSet( bgl, GF_BGL_FRAME0_M, &TextBgCntDat1[0], GF_BGL_MODE_TEXT );
		GF_BGL_BGControlSet( bgl, GF_BGL_FRAME1_M, &TextBgCntDat1[1], GF_BGL_MODE_TEXT );
		GF_BGL_BGControlSet( bgl, GF_BGL_FRAME2_M, &TextBgCntDat1[2], GF_BGL_MODE_TEXT );
		GF_BGL_BGControlSet( bgl, GF_BGL_FRAME3_M, &TextBgCntDat1[3], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( bgl, GF_BGL_FRAME0_M );
		GF_BGL_ScrClear( bgl, GF_BGL_FRAME1_M );
		GF_BGL_ScrClear( bgl, GF_BGL_FRAME2_M );
		GF_BGL_ScrClear( bgl, GF_BGL_FRAME3_M );
	}
	
	///< サブ画面フレーム設定
	{
		GF_BGL_BGControlSet( bgl, GF_BGL_FRAME0_S, &TextBgCntDat2[0], GF_BGL_MODE_TEXT );
		GF_BGL_BGControlSet( bgl, GF_BGL_FRAME1_S, &TextBgCntDat2[1], GF_BGL_MODE_TEXT );
		GF_BGL_BGControlSet( bgl, GF_BGL_FRAME2_S, &TextBgCntDat2[2], GF_BGL_MODE_TEXT );
		GF_BGL_BGControlSet( bgl, GF_BGL_FRAME3_S, &TextBgCntDat2[3], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( bgl, GF_BGL_FRAME0_S );
		GF_BGL_ScrClear( bgl, GF_BGL_FRAME1_S );
		GF_BGL_ScrClear( bgl, GF_BGL_FRAME2_S );
		GF_BGL_ScrClear( bgl, GF_BGL_FRAME3_S );
	}
	
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	
	///< SUB画面をメイン画面にするため
	BR_ChangeDisplay( DISP_3D_TO_SUB );
	
	G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2,
					  GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_OBJ,	7, 8 );
		
	G2S_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2,
					   GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_OBJ,  7, 8 );
}


//--------------------------------------------------------------
/**
 * @brief	メインとサブの3D面を入れ替える
 *
 * @param	mode	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_ChangeDisplay( int mode )
{	
	sys.disp3DSW = mode;//DISP_3D_TO_SUB;
	GF_Disp_DispSelect();
}

//--------------------------------------------------------------
/**
 * @brief	BG 2 をいつもの状態に戻す （ドレスアップで変更されるため）
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BG2_Recover( BR_WORK* wk )
{
	GF_BGL_BGControlSet( wk->sys.bgl, GF_BGL_FRAME2_M, &TextBgCntDat1[ 2 ], GF_BGL_MODE_TEXT );	
}

//--------------------------------------------------------------
/**
 * @brief	CATS の 初期化
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BR_CATS_Init( BR_WORK* wk )
{
	wk->sys.csp = CATS_AllocMemory( HEAPID_BR );
	{
		const TCATS_OAM_INIT OamInit = {
			0, 128 + 0, 0, 32,
			4, 128 - 4, 0, 32,
		};
		const TCATS_CHAR_MANAGER_MAKE Ccmm = {
			64 + 64,			///< ID管理 main + sub
			1024 * 0x40,		///< 64k
			 512 * 0x20,		///< 32k
		//	1024 * 0x40,		///< 32k
		//	GX_OBJVRAMMODE_CHAR_1D_64K,
		//	GX_OBJVRAMMODE_CHAR_1D_64K		///< 32Kだった
			GX_OBJVRAMMODE_CHAR_1D_128K,
			GX_OBJVRAMMODE_CHAR_1D_128K		///< 32Kだった
		};
		CATS_SystemInit( wk->sys.csp, &OamInit, &Ccmm, 16 + 16 );
	}
	
	{
		BOOL active;
		const TCATS_RESOURCE_NUM_LIST ResourceList = {
			64 + 64 ,	///< キャラ登録数 	main + sub
			16 + 16,	///< パレット登録数	main + sub
			128,		///< セル
			128,		///< セルアニメ
			16,			///< マルチセル
			16,			///< マルチセルアニメ
		};
		wk->sys.crp = CATS_ResourceCreate( wk->sys.csp );
//		wk->sys.vintr_crp = CATS_ResourceCreate( wk->sys.csp );

		active   = CATS_ClactSetInit( wk->sys.csp, wk->sys.crp, 64 + 64 + 64 );
		GF_ASSERT( active );
		
		active	  = CATS_ResourceManagerInit( wk->sys.csp, wk->sys.crp, &ResourceList );
		GF_ASSERT( active );

//		active   = CATS_ClactSetInit( wk->sys.csp, wk->sys.vintr_crp, 32 );
//		GF_ASSERT( active );
//		active	  = CATS_ResourceManagerInit( wk->sys.csp, wk->sys.vintr_crp, &ResourceList2 );
//		GF_ASSERT( active );
	}

	CLACT_U_SetSubSurfaceMatrix( CATS_EasyRenderGet( wk->sys.csp ), 0, SUB_SURFACE_FX_Y_BR );
}


//--------------------------------------------------------------
/**
 * @brief	VBlank
 *
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_VBlank( void* work )
{
	BR_WORK* wk = work;

	DoVramTransferManager();			///< Vram転送マネージャー実行
	
//	SoftSpriteTextureTrans( wk->sys.ssm_p );
	
//	CATS_RenderOamTrans();

	PaletteFadeTrans( wk->sys.pfd );
	
	GF_BGL_VBlankFunc( wk->sys.bgl );
	
//	CATS_Draw( wk->sys.crp );
	
	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}


//--------------------------------------------------------------
/**
 * @brief	更新
 *
 * @param	wk	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL BR_Main_Update( BR_WORK* wk, int* seq )
{
	BOOL bState = FALSE;
	
	switch( *seq ){
	
	case eMAIN_Start:
		///< 起動シーケンス
		bState = BR_start_MainTable[ wk->main_seq ]( wk );
		
		BR_Main_ProcSeqChange( wk, eMAIN_Input, bState, seq );
		
		if ( bState ){
			if ( wk->sys.touch == INPUT_MODE_TOUCH ){
				BR_Main_SeqChange( wk, 0 );
			}
			else {
				BR_Main_SeqChange( wk, 1 );
			}
		}		
		break;
		
	case eMAIN_Input:		
		///< Input
		bState = BR_input_MainTable[ wk->main_seq ]( wk );		
		///< touch panel の 操作
	//	BR_TouchPanel_Update( wk );		
		BR_Main_ProcSeqChange( wk, eMAIN_Blank, bState, seq );
		break;
		
	case eMAIN_Profile:
		///< ProFile
		bState = BR_prof_MainTable[ wk->main_seq ]( wk );		
		BR_Main_ProcSeqChange( wk, eMAIN_Blank, bState, seq );
		break;
		
	case eMAIN_Frontier:
		///< Frontier
		bState = BR_frec_MainTable[ wk->main_seq ]( wk );		
		BR_Main_ProcSeqChange( wk, eMAIN_Blank, bState, seq );
		break;
	
	case eMAIN_PhotoSend:
		bState = BR_photo_send_MainTable[ wk->main_seq ]( wk );		
		BR_Main_ProcSeqChange( wk, eMAIN_Blank, bState, seq );
		break;
		
	case eMAIN_PhotoRecv:
		bState = BR_photo_recv_MainTable[ wk->main_seq ]( wk );		
		BR_Main_ProcSeqChange( wk, eMAIN_Blank, bState, seq );
		break;
		
	case eMAIN_BoxSend:
		bState = BR_BoxS_MainTable[ wk->main_seq ]( wk );		
		BR_Main_ProcSeqChange( wk, eMAIN_Blank, bState, seq );
		break;
		
	case eMAIN_BoxRecv:
		bState = BR_BoxR_MainTable[ wk->main_seq ]( wk );		
		BR_Main_ProcSeqChange( wk, eMAIN_Blank, bState, seq );
		break;
	
	case eMAIN_ThisWeek:
	case eMAIN_LastWeek:
		bState = BR_WeeklyRanking_MainTable[ wk->main_seq ]( wk );		
		BR_Main_ProcSeqChange( wk, eMAIN_Blank, bState, seq );
		break;
	
	case eMAIN_CommBattleRanking:
	case eMAIN_BattleFrontierRanking:
		bState = BR_VideoRanking_MainTable[ wk->main_seq ]( wk );		
		BR_Main_ProcSeqChange( wk, eMAIN_Blank, bState, seq );
		break;
	
	case eMAIN_VideoNew30:
	//	bState = BR_New30_MainTable[ wk->main_seq ]( wk );		
		bState = BR_VideoRanking_MainTable[ wk->main_seq ]( wk );
		BR_Main_ProcSeqChange( wk, eMAIN_Blank, bState, seq );
		break;
		
	case eMAIN_VideoKuwasiku:
		bState = BR_Search_MainTable[ wk->main_seq ]( wk );		
		BR_Main_ProcSeqChange( wk, eMAIN_Blank, bState, seq );		
		break;

	case eMAIN_VideoNoSearch:
		bState = BR_Video_MainTable[ wk->main_seq ]( wk );		
		BR_Main_ProcSeqChange( wk, eMAIN_Blank, bState, seq );
		break;

	case eMAIN_VideoSend:
		bState = BR_Video_Send_MainTable[ wk->main_seq ]( wk );		
		BR_Main_ProcSeqChange( wk, eMAIN_Blank, bState, seq );
		break;
	
	case eMAIN_Blank:
		BR_Main_ProcSeqChange( wk, eMAIN_End, TRUE, seq );
		break;	
		
	case eMAIN_Delete:
		bState = BR_delete_MainTable[ wk->main_seq ]( wk );
		BR_Main_ProcSeqChange( wk, eMAIN_Blank, bState, seq );
		break;
	
	case eMAIN_Error:
		BR_Error_Main( wk );
		break;
	
	default:
		return TRUE;
	}

//	CATS_Draw( wk->sys.crp );
	
	GF_G3X_Reset();

	SoftSpriteMain( wk->sys.ssm_p );
	
	GF_G3_RequestSwapBuffers( GX_SORTMODE_MANUAL, GX_BUFFERMODE_Z );
	
	BR_GDSRAP_Main( wk );
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	プロックシーケンス切り替え
 *
 * @param	wk	
 * @param	next_seq	
 * @param	flag	
 * @param	seq	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_Main_ProcSeqChange( BR_WORK* wk, int next_seq, BOOL flag, int* seq )
{
	if ( flag ){		
		(*seq) = next_seq;		
		BR_Main_SeqChange( wk, 0 );
		BR_Main_SubSeqChange( wk, 0 );
	}
}

//--------------------------------------------------------------
/**
 * @brief	シーケンス切り替え
 *
 * @param	wk	
 * @param	next_seq	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_Main_SeqChange( BR_WORK* wk, int next_seq )
{
	wk->main_seq	= next_seq;
	wk->sub_seq		= 0;
	
	wk->wait		= 0;
}


//--------------------------------------------------------------
/**
 * @brief	内部サブシーケンス切り替え
 *
 * @param	wk	
 * @param	next_seq	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_Main_SubSeqChange( BR_WORK* wk, int next_seq )
{
	wk->sub_seq		= next_seq;
	
	wk->wait		= 0;
}


//--------------------------------------------------------------
/**
 * @brief	共通処理
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_Main_MenuChangeInit( BR_WORK* wk )
{
	BattleRecorder_MenuSetting( wk );									///< メニュー設定
	BR_Tag_ReSet( wk );													///< タグのリセット
		
	wk->tag_man.cur_pos = 0;											///< カーソル位置を0に
}

//--------------------------------------------------------------
/**
 * @brief	メニューの切り替え
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
BOOL BR_Main_MenuChangeNext( BR_WORK* wk )
{
	wk->menu_data_mem[ wk->tag_man.stack_s ] = wk->menu_data;
	wk->tag_man.menu_data_stack[ wk->tag_man.stack_s ] = &wk->menu_data[ wk->tag_man.cur_pos ];
	
	wk->menu_data = wk->menu_data[ wk->tag_man.cur_pos ].data;			///< メニュー切り替え
	
//	OS_Printf( "next stak_s = %2d\n", wk->tag_man.stack_s );
	
	wk->tag_man.stack_s++;												///< 上画面のタグ個数を増やす
	wk->tag_man.cur_pos = 0;											///< カーソル位置を0に

	if ( wk->menu_data == NULL ){
		
		return FALSE;
	}

	BR_Main_MenuChangeInit( wk );
	
	return TRUE;
}


//--------------------------------------------------------------
/**
 * @brief	メニューの切り替え
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
BOOL BR_Main_MenuChangeBack( BR_WORK* wk )
{
	wk->menu_data = wk->menu_data_mem[ wk->tag_man.stack_s ];
	
//	OS_Printf( "back stak_s = %2d\n", wk->tag_man.stack_s );
	
	BR_Main_MenuChangeInit( wk );
	
	return TRUE;
}


//--------------------------------------------------------------
/**
 * @brief	メニューデータの解析
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_MenuData_Analysis( BR_WORK* wk )
{
	int seq = wk->menu_data[ wk->tag_man.cur_pos ].seq;
	
	///　BR_UnlockMenuCheck　でアンロックを解除しないと使えない
	
	switch ( seq ) {
	case eMENU_DATA_SEQ_NONE:	///< 無し
	case eMENU_DATA_SEQ_SELECT:	///< 選択
	case eMENU_DATA_SEQ_BACK:	///< 戻る
	case eMENU_DATA_SEQ_BLANK:	///< 無し
		// 特に何もしない
		break;
		
	case eMENU_DATA_SEQ_END:	///< 終了
		BR_PostMessage( wk->message, FALSE, 0 );
		BR_Main_SeqChange( wk, eINPUT_END );
		break;
		
	case eMENU_DATA_PROF_INIT:	///< 自分の記録初期化
		BR_Main_ProcSeqChange( wk, eMAIN_Profile, TRUE, wk->proc_seq );
		break;
		
	case eMENU_DATA_FREC_INIT:	///< フロンティアの記録初期化
		BR_Main_ProcSeqChange( wk, eMAIN_Frontier, TRUE, wk->proc_seq );
		break;
	
	case eMENU_DATA_PHOTO_SEND:	///< 写真を送る
		BR_Main_ProcSeqChange( wk, eMAIN_PhotoSend, TRUE, wk->proc_seq );
		break;
		
	case eMENU_DATA_PHOTO_RECV:	///< 写真をみる(受信)
		BR_Main_ProcSeqChange( wk, eMAIN_PhotoRecv, TRUE, wk->proc_seq );
		break;
		
	case eMENU_DATA_BOX_SEND:	///< ボックスを送る
		BR_Main_ProcSeqChange( wk, eMAIN_BoxSend, TRUE, wk->proc_seq );
		break;
		
	case eMENU_DATA_BOX_RECV:	///< ボックスをみる(受信)
		BR_Main_ProcSeqChange( wk, eMAIN_BoxRecv, TRUE, wk->proc_seq );
		break;
				
	case eMENU_DATA_THIS_WEEK:	///< 今週のランキング
		BR_Main_ProcSeqChange( wk, eMAIN_ThisWeek, TRUE, wk->proc_seq );
		break;
		
	case eMENU_DATA_LAST_WEEK:	///< 先週のランキング
		BR_Main_ProcSeqChange( wk, eMAIN_LastWeek, TRUE, wk->proc_seq );
		break;
	
	case eMENU_DATA_COMM_BATTLE_RANKING:	///< 通信対戦ランキング
		BR_Main_ProcSeqChange( wk, eMAIN_CommBattleRanking, TRUE, wk->proc_seq );
		break;
		
	case eMENU_DATA_BATTLE_FRONTIER_RANKING:	///< バトルフロンティアランキング
		BR_Main_ProcSeqChange( wk, eMAIN_BattleFrontierRanking, TRUE, wk->proc_seq );
		break;
	
	case eMENU_DATA_VIDE_NEW30:					///< 最新３０件
		BR_Main_ProcSeqChange( wk, eMAIN_VideoNew30, TRUE, wk->proc_seq );
		break;
		
	case eMENU_DATA_VIDE_KUWASIKU:				///< 詳しく探す
		BR_Main_ProcSeqChange( wk, eMAIN_VideoKuwasiku, TRUE, wk->proc_seq );
		break;
		
	case eMENU_DATA_VIDE_NO_SEARCH:				///< データナンバーで検索
		BR_Main_ProcSeqChange( wk, eMAIN_VideoNoSearch, TRUE, wk->proc_seq );
		break;
		
	case eMENU_DATA_VIDE_SEND:					///< ビデオの送信
		BR_Main_ProcSeqChange( wk, eMAIN_VideoSend, TRUE, wk->proc_seq );
		break;
		
	case eMENU_DATA_REC_DELETE:					///< セーブの削除
		BR_Main_ProcSeqChange( wk, eMAIN_Delete, TRUE, wk->proc_seq );
		break;
	}
}


// =============================================================================
//
//
//	■タッチパネル関連
//
//
// =============================================================================
//--------------------------------------------------------------
/**
 * @brief	メニューロックチェック
 *
 * @param	wk	
 *
 * @retval	BOOL	TRUE=OK
 *
 */
//--------------------------------------------------------------
BOOL BR_UnlockMenuCheck( BR_WORK* wk )
{
	int	ex_param1 = wk->menu_data[ wk->tag_man.cur_pos ].ex_param1;
	BOOL bFlag = TRUE;
	
	switch ( wk->mode ){
	default:		
		if ( ex_param1 == LOADDATA_MYREC
		||	 ex_param1 == LOADDATA_DOWNLOAD0
		||	 ex_param1 == LOADDATA_DOWNLOAD1
		||	 ex_param1 == LOADDATA_DOWNLOAD2 ){
			
			if ( wk->br_head[ ex_param1 ] == NULL ){
				Tool_InfoMessage( wk, msg_info_000 );
				bFlag = FALSE;
			}
		}
		break;
	
	case BR_MODE_GDS_DRESS:		
		if ( ex_param1 == IMC_SAVEDATA_TELEVISION_MYDATA ){	
			///< セーブがそもそもあるのか？
			IMC_TELEVISION_SAVEDATA* dress = ImcSaveData_GetTelevisionSaveData( SaveData_GetImcSaveData( wk->save ), ex_param1 );					
			bFlag = ImcSaveData_CheckTelevisionData( dress );
			if ( bFlag == FALSE ){
				Tool_InfoMessage( wk, msg_info_021 );
			}
		#if 0
			else {
				///< １度送ったドレスか？
				if ( MISC_GetGdsSend_Dress( SaveData_GetMisc( wk->save ) ) ){
					Tool_InfoMessage( wk, msg_info_022 );
					bFlag = FALSE;
				}
			}
		#endif
		}
		break;
	
	case BR_MODE_GDS_BOX:
		///< ボックスを送ったか？
		if ( ex_param1 == BR_EX_DATA_BOX_SHOT_SEND ){			
			if ( ExParam2_Get( wk ) ){				
				Tool_InfoMessage( wk, msg_info_004 );
				bFlag = FALSE;
			}
		}
		break;
		
	case BR_MODE_GDS_BV:
		///< １度送ったビデオか？
	//	if ( ex_param1 == BR_EX_DATA_VIDEO_SEND ){
		if ( ex_param1 == BR_EX_SEND_CHECK ){//BR_EX_DATA_VIDEO_SEND ){
			if ( wk->br_head[ LOADDATA_MYREC ] == NULL ){
				Tool_InfoMessage( wk, msg_726 );
				bFlag = FALSE;			
			}
		#if 0
			else {
				if ( MISC_GetGdsSend_Video( SaveData_GetMisc( wk->save ) ) ){
					Tool_InfoMessage( wk, msg_724 );
					bFlag = FALSE;
				}
			}
		#endif
		}	
		break;
	case BR_MODE_GDS_RANK:
	case BR_MODE_GDS_BV_RANK:
		///< なんも制限無し
		break;

	}

	if ( bFlag == FALSE ){		
		Snd_SePlay( eBR_SND_NG );
	}
	else {
		Snd_SePlay( eBR_SND_TOUCH );
	}		
	
	return bFlag;
}

//--------------------------------------------------------------
/**
 * @brief	コールバック
 *
 * @param	button	
 * @param	event	
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BR_ButtonManagerCallBack( u32 button, u32 event, void* work )
{
	BR_WORK* wk = work;
	
	if ( event != BMN_EVENT_TOUCH ){ return; }
	
	wk->tag_man.cur_pos = button;

	///< 判定しない処理
	{
		BOOL bLock = BR_UnlockMenuCheck( wk );		
		if ( bLock == FALSE ){ return; }
	}

	///< タグをタッチした時
	if ( button >= eHRT_TAG_0 && button <= eHRT_TAG_4 ){
		
		s16 x, y;
		BR_CATS_ObjectPosGetCap( wk->tag_man.tag_m[ button ].cap, &x, &y );
		Cursor_PosSetFix( wk->cur_wk, x + TAG_CIRCLE_CX, y );
		Cursor_PosSet( wk->cur_wk, x + TAG_CIRCLE_CX, y );
		
		wk->ex_param1 = wk->menu_data[ wk->tag_man.cur_pos ].ex_param1;
					
		///< 決定
		BR_Main_SeqChange( wk, wk->menu_data[ wk->tag_man.cur_pos ].tag_seq );				
		if ( wk->menu_data[ wk->tag_man.cur_pos ].tag_seq == eINPUT_END ){
			BR_PostMessage( wk->message, FALSE, 0 );
		}
		else {
			BR_TouchPanel_TagHRT_ReSet( wk );			///< ここでタグの当たり判定消す
			Cursor_Visible( wk->cur_wk, TRUE );
		}

		if ( wk->ex_param1 == BR_EX_WIFI_END ){
			///< wi-fi終了チェックに行くとき
			Tool_WiFiEnd_Message( wk, msg_10001 );
		}
		else if ( wk->ex_param1 == BR_EX_SEND_CHECK ){
			///< バトルビデオをほんとうに送るか確認するとき
			Tool_WiFiEnd_Message( wk, msg_info_032 );
		}
		else {
			if ( wk->bWiFiEnd ){
				Tool_InfoMessageDel( wk );
				wk->bWiFiEnd = FALSE;
			}
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief	タッチパネル初期化
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_TouchPanel_Init( BR_WORK* wk )
{
	int i;
	
	///< コードの当たり判定
	for ( i = 0; i < eHRT_MAX; i ++ ){
				
		wk->tag_man.tag_m[ i ].hit = &wk->sys.rht[ i ];
	}

	wk->sys.btn = BMN_Create( wk->sys.rht, eHRT_MAX, 
							  BR_ButtonManagerCallBack, wk, HEAPID_BR );	
}

//--------------------------------------------------------------
/**
 * @brief	タグの当たり判定設定
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_TouchPanel_TagHRT_Set( BR_WORK* wk )
{
	int i;
	s16 x, y;
	
	if ( wk->menu_data == NULL ){
	//	BR_TouchPanel_TagHRT_ReSet( wk );
		return;
	}
	
	for ( i = 0; i < TAG_MAX; i++ ){
		
	//	OS_Printf( " hit rect 0 = %d, %d\n", wk->tag_man.tag_m[ i ].hit->rect.top, wk->tag_man.tag_m[ i ].hit->rect.bottom );

		if ( wk->menu_data[ i ].active == FALSE ){
			wk->tag_man.tag_m[ i ].hit->rect.top	= 0;
			wk->tag_man.tag_m[ i ].hit->rect.bottom	= 0;
			wk->tag_man.tag_m[ i ].hit->rect.left	= 0;
			wk->tag_man.tag_m[ i ].hit->rect.right	= 0;
			
	//		OS_Printf( " hit rect 1 = %d, %d\n", wk->tag_man.tag_m[ i ].hit->rect.top, wk->tag_man.tag_m[ i ].hit->rect.bottom );
			continue;
		}
		
		CATS_ObjectPosGetCap( wk->tag_man.tag_m[ i ].cap, &x, &y );
		
		wk->tag_man.tag_m[ i ].hit->rect.top	= y - TAG_HIT_HEIGHT;
		wk->tag_man.tag_m[ i ].hit->rect.bottom	= y + TAG_HIT_HEIGHT;
		wk->tag_man.tag_m[ i ].hit->rect.left	= x - 0;
		wk->tag_man.tag_m[ i ].hit->rect.right	= x + TAG_HIT_RIGHT;
		
	//	OS_Printf( " hit rect 1 = %d, %d\n", wk->tag_man.tag_m[ i ].hit->rect.top, wk->tag_man.tag_m[ i ].hit->rect.bottom );
	}
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_TouchPanel_TagHRT_ReSet( BR_WORK* wk )
{
	int i;
	s16 x, y;
	
	for ( i = 0; i < TAG_MAX; i++ ){
		
		CATS_ObjectPosGetCap( wk->tag_man.tag_m[ i ].cap, &x, &y );
		
		wk->tag_man.tag_m[ i ].hit->rect.top	= 0;
		wk->tag_man.tag_m[ i ].hit->rect.bottom	= 0;
		wk->tag_man.tag_m[ i ].hit->rect.left	= 0;
		wk->tag_man.tag_m[ i ].hit->rect.right	= 0;
	}
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_TouchPanel_Update( BR_WORK* wk )
{
	BMN_Main( wk->sys.btn );

#ifdef BR_AUTO_SERCH
	if ( sys.trg & PAD_BUTTON_SELECT ){
		g_debug_auto ^= 1;
		g_debug_count = 0;
		g_debug_wait  = 0;
	}
	if ( g_debug_auto ){
//		if ( (++g_debug_wait) >= ( 30 * 195 ) || sys.cont )
		{
			g_debug_count++;
			if ( g_debug_count == 1 ){
				BR_ButtonManagerCallBack( eHRT_TAG_1, BMN_EVENT_TOUCH, wk );
			}
			else {
				g_debug_wait  = 0;
				g_debug_count = 0;
				BR_ButtonManagerCallBack( eHRT_TAG_0, BMN_EVENT_TOUCH, wk );
			}
		}
	}
#endif
}

//--------------------------------------------------------------
/**
 * @brief	１段階戻す処理
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_Stack_Menu_Back( BR_WORK* wk )
{
	if ( wk->tag_man.stack_s > 0 ){
		 wk->tag_man.stack_s--;
	}
	else {
		if ( wk->tag_man.stack_s != 0 ){
			wk->tag_man.stack_s = 0;
			GF_ASSERT_MSG( 0, " top menu に 戻すよ " );
		}
	}
	wk->menu_data = wk->menu_data_mem[ wk->tag_man.stack_s ];
}

//--------------------------------------------------------------
/**
 * @brief	ワーククリア
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_RecoverWorkClear( BR_WORK* wk )
{
	wk->recover_wk.bRecover = FALSE;
	wk->recover_wk.last_seq	= 0xFF;
	
	{
		int i;
		
		for ( i = 0; i < BR_RECOVER_WORK_MAX; i++ ){
			
			wk->recover_wk.param[ i ] = 0;
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief	ワークチェック
 *
 * @param	wk	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL BR_RecoverCheck( BR_WORK* wk )
{
	return wk->recover_wk.bRecover;
}


//--------------------------------------------------------------
/**
 * @brief	最後のシーケンスゲット
 *
 * @param	wk	
 *
 * @retval	int	
 *
 */
//--------------------------------------------------------------
int BR_RecoverSeqGet( BR_WORK* wk )
{
	return wk->recover_wk.last_seq;
}


//--------------------------------------------------------------
/**
 * @brief	最後のシーケンスセット
 *
 * @param	wk	
 * @param	seq	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_RecoverSeqSet( BR_WORK* wk, int seq )
{
	wk->recover_wk.bRecover = TRUE;
	wk->recover_wk.last_seq = seq;
}

