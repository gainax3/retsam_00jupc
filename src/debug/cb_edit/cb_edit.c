//==============================================================================
/**
 * @file	cb_sys.c
 * @brief	簡単な説明を書く
 * @author	goto
 * @date	2006.04.03(月)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
#ifdef PM_DEBUG

#include "common.h"
#include "poketool/poke_tool.h"
#include "system/procsys.h"
#include "system/snd_tool.h"
#include "include/system/lib_pack.h"
#include "include/system/brightness.h"
#include "include/system/heapdefine.h"
#include "include/system/touch_subwindow.h"
#include "include/gflib/strbuf.h"
#include "include/gflib/msg_print.h"

#include "include/system/fontproc.h"
#include "include/system/msgdata.h"
#include "include/system/arc_util.h"
#include "include/system/window.h"

#include "include/poketool/pokeicon.h"

#include "field/field_common.h"
#include "field/field_event.h"
#include "field/fieldsys.h"
#include "field/ev_mapchange.h"


#include "system/bmp_menu.h"
#include "system/palanm.h"
#include "system/clact_tool.h"

#include "include/application/cb_sys.h"
#include "battle/battle_common.h"
#include "system/wipe.h"

#include "battle/graphic/pl_batt_bg_def.h"

#include "application/custom_ball/data/cb_data_def.h"

#include "system/pm_overlay.h"


#include "wazaeffect/ball_effect_tool.h"

#include "application/custom_ball/cb_snd_def.h"
#include "pokeanime/p_anm_sys.h"

#include "include/field/tvtopic_extern.h"
#include "include/savedata/tv_work.h"

#include "include/system/main.h"
#include "cb_edit.h"
#include "d_test.naix"

FS_EXTERN_OVERLAY(bc_common);

static PROC_RESULT	CBE_ProcInit(PROC* proc, int* seq);
static PROC_RESULT	CBE_ProcMain(PROC* proc, int* seq);
static PROC_RESULT	CBE_ProcEnd(PROC* proc, int* seq);

static void CBE_pv_EditSealSave( CBE_WORK* wk );
static void CBE_pv_EditSealAdd( CBE_WORK* wk );
static void CBE_pv_EditSealDel( CBE_WORK* wk );
static void CBE_pv_NumOAM_Update( CBE_WORK* wk );
static void CBE_pv_NumOAM_Enable( CBE_WORK* wk, int flag );
static void CBE_pv_NumOAM_Create( CBE_WORK* wk );
static void CBE_pv_SealLoad( CBE_WORK* wk );
static void CBE_pv_PageData_Create(CB_MAIN_WORK* cbmw, int page_no);
static void CBE_pv_EditCallBack( u32 button, u32 event, void* work );
static void CBE_pv_PanelOAM_Change( CBE_WORK* wk, int num );
static void CBE_pv_PanelSys( CBE_WORK* wk, int button );
static void CBE_pv_PanelCallBack( u32 button, u32 event, void* work );
static void CBE_pv_ButtonHitPanel( CBE_WORK* wk );
static void CBE_pv_DispInit( CB_MAIN_WORK* cbmw );
static void CBE_pv_MainEdit( CBE_WORK* wk );
static void CBE_pv_PanelControl( CBE_WORK* wk );
static void CBE_pv_VramBankSet(GF_BGL_INI *bgl);

FS_EXTERN_OVERLAY(custom_ball);

const PROC_DATA CBE_ProcData = {			///< プロセス定義データ

	CBE_ProcInit,
	CBE_ProcMain,
	CBE_ProcEnd,	

	FS_OVERLAY_ID(custom_ball),
};

static void CBE_pv_VramBankSet(GF_BGL_INI *bgl)
{
	GF_Disp_GX_VisibleControlInit();
//	WIPE_ResetBrightness(WIPE_DISP_MAIN);
//	WIPE_ResetBrightness(WIPE_DISP_SUB);
	
	
	//VRAM設定
	{
		GF_BGL_DISPVRAM vramSetTable = {
			GX_VRAM_BG_128_A,				// メイン2DエンジンのBG
			GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット
			GX_VRAM_SUB_BG_32_H,			// サブ2DエンジンのBG
			GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット
			GX_VRAM_OBJ_64_E,				// メイン2DエンジンのOBJ
			GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット
			GX_VRAM_SUB_OBJ_16_I,			// サブ2DエンジンのOBJ
			GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット
			GX_VRAM_TEX_01_BC,				// テクスチャイメージスロット
			GX_VRAM_TEXPLTT_01_FG			// テクスチャパレットスロット
		};
		GF_Disp_SetBank( &vramSetTable );

		//VRAMクリア
		MI_CpuClear32((void*)HW_BG_VRAM, HW_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_BG_VRAM, HW_DB_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_OBJ_VRAM, HW_OBJ_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_OBJ_VRAM, HW_DB_OBJ_VRAM_SIZE);
	}

	// BG SYSTEM
	{
		GF_BGL_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_3D,
		};
		GF_BGL_InitBG( &BGsys_data );
	}

	//メイン画面フレーム設定
	{
		GF_BGL_BGCNT_HEADER TextBgCntDat[] = {
			///<FRAME1_M
			{
				0, 0, 0x0800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01,
				CB_BG_PRI_SCR_STRING, 0, 0, FALSE
			},
			///<FRAME2_M
			{
				0, 0, 0x2000, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000, GX_BG_EXTPLTT_01,
				CB_BG_PRI_SCR_SELECT, 0, 0, FALSE
			},
			///<FRAME3_M
			{
				0, 0, 0x1000, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x3000, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
				CB_BG_PRI_SCR_FIELD, 0, 0, FALSE
			},
		};
		GF_BGL_BGControlSet(bgl, GF_BGL_FRAME1_M, &TextBgCntDat[0], GF_BGL_MODE_TEXT );
		GF_BGL_BGControlSet(bgl, GF_BGL_FRAME2_M, &TextBgCntDat[1], GF_BGL_MODE_TEXT );
		GF_BGL_BGControlSet(bgl, GF_BGL_FRAME3_M, &TextBgCntDat[2], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, GF_BGL_FRAME1_M );
		GF_BGL_ScrClear(bgl, GF_BGL_FRAME2_M );
		GF_BGL_ScrClear(bgl, GF_BGL_FRAME3_M );

		G2_SetBG0Priority(BATTLE_3DBG_PRIORITY);
		GF_Disp_GX_VisibleControl(GX_PLANEMASK_BG0, VISIBLE_ON);
	}
	
	///< サブ画面フレーム設定
	{
		GF_BGL_BGCNT_HEADER TextBgCntDat[] = {
			{	/// font
				0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
				CB_BG_PRI_SCR_SUB_STRING, 0, 0, FALSE
			},
			{	/// boal
				0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
				CB_BG_PRI_SCR_BALL, 0, 0, FALSE
			},
			{	/// list
				0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
				CB_BG_PRI_SCR_LIST, 0, 0, FALSE
			},
			{	/// edit
				0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01,
				CB_BG_PRI_SCR_EDIT, 0, 0, FALSE
			},
		};
		GF_BGL_BGControlSet(bgl, GF_BGL_FRAME0_S, &TextBgCntDat[0], GF_BGL_MODE_TEXT);
		GF_BGL_BGControlSet(bgl, GF_BGL_FRAME1_S, &TextBgCntDat[1], GF_BGL_MODE_TEXT);
		GF_BGL_BGControlSet(bgl, GF_BGL_FRAME2_S, &TextBgCntDat[2], GF_BGL_MODE_TEXT);
		GF_BGL_BGControlSet(bgl, GF_BGL_FRAME3_S, &TextBgCntDat[3], GF_BGL_MODE_TEXT);
		GF_BGL_ScrClear(bgl, GF_BGL_FRAME0_S);
		GF_BGL_ScrClear(bgl, GF_BGL_FRAME1_S);
		GF_BGL_ScrClear(bgl, GF_BGL_FRAME2_S);
		GF_BGL_ScrClear(bgl, GF_BGL_FRAME3_S);
		GF_Disp_GXS_VisibleControl(CB_PLANE_LIST, VISIBLE_OFF);
	}
}

///< 現在の編集データをEditDataに戻す
static void CBE_pv_EditSealSave( CBE_WORK* wk )
{
	int i;	
	s16 x, y;
	CB_MAIN_WORK* cbmw;
	
	cbmw = wk->mw;	
	
	for ( i = 0; i < CB_DEF_SEAL_MAX; i++ ){
		
		if ( cbmw->seal[ i ].pop == FALSE
		||	 cbmw->seal[ i ].cap == NULL ){ continue; }
		
		CATS_ObjectPosGetCap( cbmw->seal[ i ].cap, &x, &y );
		
		wk->edit_core[ wk->edit_no ].cb_seal[ i ].seal_id = cbmw->seal[ i ].id;
		wk->edit_core[ wk->edit_no ].cb_seal[ i ].x		  = x;
		wk->edit_core[ wk->edit_no ].cb_seal[ i ].y		  = y;
	}
}

static void CBE_pv_EditSealAdd( CBE_WORK* wk )
{
	CBE_pv_SealLoad( wk );
}

static void CBE_pv_EditSealDel( CBE_WORK* wk )
{
	CB_MAIN_WORK* cbmw;
	
	cbmw = wk->mw;	
	
	CBS_Delete( cbmw );
}

static void CBE_pv_NumOAM_Update( CBE_WORK* wk )
{
	int i;
	
	for ( i = 0; i < 3; i++ ){
		
		if ( wk->cap[ i ] == NULL ){ continue; }
		
		CATS_ObjectUpdateCap( wk->cap[ i ] );
	}
}

static void CBE_pv_NumOAM_Enable( CBE_WORK* wk, int flag )
{
	int i;
	
	for ( i = 0; i < 3; i++ ){
		
		if ( wk->cap[ i ] == NULL ){ continue; }
		
		CATS_ObjectEnableCap( wk->cap[ i ], flag );
	}
}

static void CBE_pv_NumOAM_Create( CBE_WORK* wk )
{
	int i;
	static s16 pos[][ 2 ] = {
		{ 204, 20 },{ 220, 20 },{ 236, 20 },
	};
	
	CATS_ACT_PTR cap;
	TCATS_OBJECT_ADD_PARAM_S coap;
	
	CB_MAIN_WORK* cbmw = wk->mw;	
	
	
	{
		ARCHANDLE* p_handle;
		
		p_handle = ArchiveDataHandleOpen( ARC_TEST_FILE, HEAPID_CUSTOM_BALL );
			
		CATS_LoadResourceCharArcH(cbmw->sys.csp, cbmw->sys.crp,
								 p_handle, NARC_d_test_num_NCGR_bin, TRUE,
								 CATS_D_AREA_SUB,
								 99999);
								 
		CATS_LoadResourcePlttWorkArcH(cbmw->sys.pfd, FADE_SUB_OBJ,
								 cbmw->sys.csp, cbmw->sys.crp,
								 p_handle, NARC_d_test_num_NCLR, FALSE,
								 1, CATS_D_AREA_SUB, 99999);

		CATS_LoadResourceCellArcH(cbmw->sys.csp, cbmw->sys.crp,
								 p_handle, NARC_d_test_num_NCER_bin, TRUE,
								 99999);

		CATS_LoadResourceCellAnmArcH(cbmw->sys.csp, cbmw->sys.crp,
									p_handle, NARC_d_test_num_NANR_bin, TRUE,
									99999);	
		
		ArchiveDataHandleClose( p_handle );
	}
	
	for ( i = 0; i < 3; i++ ){
		
		coap.x		= pos[ i ][0];
		coap.y		= pos[ i ][1];
		coap.z		= 0;		
		coap.anm	= 0;
		coap.pri	= 0;
		coap.pal	= 0;
		coap.d_area = CATS_D_AREA_SUB;
		coap.bg_pri = 0;
		coap.vram_trans = 0;
		
		coap.id[0]	= 99999;
		coap.id[1]	= 99999;
		coap.id[2]	= 99999;
		coap.id[3]	= 99999;
		coap.id[4]	= CLACT_U_HEADER_DATA_NONE;
		coap.id[5]	= CLACT_U_HEADER_DATA_NONE;
		
		wk->cap[ i ] = CATS_ObjectAdd_S( cbmw->sys.csp, cbmw->sys.crp, &coap );
	}
	
	CBE_pv_NumOAM_Enable( wk, FALSE );
}

static void CBE_pv_SealLoad( CBE_WORK* wk )
{
	int i;
	CB_CORE* core;
	CB_MAIN_WORK* cbmw;
	
	cbmw = wk->mw;	
	core = &wk->edit_core[ wk->edit_no ];
	
	for ( i = 0; i < CB_DEF_SEAL_MAX; i++ ){
		
		if ( core->cb_seal[ i ].seal_id != 0 ){
			
			cbmw->seal[ i ].id	= core->cb_seal[ i ].seal_id;
			cbmw->seal[ i ].x	= core->cb_seal[ i ].x;
			cbmw->seal[ i ].y	= core->cb_seal[ i ].y;
			
		//	OS_Printf( " %3d, %3d, %3d\n", cbmw->seal[ i ].id, cbmw->seal[ i ].x, cbmw->seal[ i ].y );
			
			CBS_Add( cbmw, i );
			HitTable_CreateCap( cbmw->seal[ i ].hit, cbmw->seal[ i ].cap, FALSE );
			
			cbmw->seal[ i ].pop = TRUE;
		}
		else {
			
			cbmw->seal[ i ].id  = 0;
			cbmw->seal[ i ].pop = FALSE;
		}
		
	}
}

static void CBE_pv_PageData_Create(CB_MAIN_WORK* cbmw, int page_no)
{
	int i;
	int no;
	int add;
	int add_start;
	int  num;
	
	no = 0;
	add = 0;
	add_start  = page_no;
	add_start *= CB_DEF_PAGE_ITEMS;
	
	for (i = 0; i < CB_DEF_PAGE_ITEMS; i++){						///< ページデータ初期化
		cbmw->page.item[ i ] = 0;
	}
	
	///< セーブデータにある、もしくはバックアップデータが所有している場合
	///< 配布用シール3枚は取得できないようにする( -4 )
	for (i = 0; i < CB_DEF_ITEM_DATA_NUM - 4; i++){
		
		num  = 99;
		
		if ( num != 0 ){
			add++;
			
			if (add <= add_start){ continue; }						///< ページ開始位置を越え無ければ登録できない
			
			cbmw->page.item[ no ] = (i + 1);						///< 1オリジンなので + 1	
			no++;
			
			if (no >= CB_DEF_PAGE_ITEMS){ break; }
		}
		
	}
}

// -----------------------------------------------------------------------------

static const RECT_HIT_TBL hit_tbl_edit[] = {
	{ 16, 16 + 16,   8,   8 + 16	},	///< seal		
	{ 16, 16 + 16,  64,  64 + 16	},	///< seal
	{ 40, 40 + 16,   8,   8 + 16	},
	{ 40, 40 + 16,  64,  64 + 16	},
	{ 64, 64 + 16,   8,   8 + 16	},
	{ 64, 64 + 16,  64,  64 + 16	},
	{ 88, 88 + 16,   8,   8 + 16	},
	{ 88, 88 + 16,  64,  64 + 16	},
	
	{104+16,104+16+ 24,  8,   8 + 40	},	///< ↑
	{104+16,104+16+ 24, 56,  56 + 40	},	///< ↓
	
	{168-16, 188-16,  64-32,   64-32+24	},			///< PANEL
	{168-16, 188-16, 104-32,  104-32+24	},			///< SHOW
	{168-16, 188-16, 144-32,  144-32+40	},			///< SAVE
	{168-16, 188-16, 196-32,  196-32+40	},			///< LOAD
};

static const RECT_HIT_TBL hit_tbl_panel[] = {
	
	{ 32, 56, 183, 207 },	/// 1
	{ 32, 56, 207, 231 },	/// 2
	{ 32, 56, 231, 255 },	/// 3
	
	{ 56, 80, 183, 207 },	/// 4
	{ 56, 80, 207, 231 },	/// 5
	{ 56, 80, 231, 255 },	/// 6
	
	{ 80,104, 183, 207 },	/// 7
	{ 80,104, 207, 231 },	/// 8
	{ 80,104, 231, 255 },	/// 9
	
	{104,128, 183, 207 },	/// 0
	{104,128, 207, 231 },	/// C
	{104,128, 231, 255 },	/// 」
	
};

static const RECT_HIT_TBL hit_tbl_YesNo[] = {
	
	{ 168-24, 188-24,  72, 112 },	/// YES
	{ 168-24, 188-24, 128, 168 },	/// NO
	
};

static void CBE_pv_EditCallBack( u32 button, u32 event, void* work )
{
	CBE_WORK* wk;
	
	wk = (CBE_WORK*)work;
	
	///< 押されたときの反応
	if ( event == BMN_EVENT_TOUCH ){
			
		switch ( button ){
		
		case eCBE_UP:
			
			if ( wk->mw->page.no > 0 ){
				wk->mw->page.no--;
			}
			else {
				wk->mw->page.no = wk->mw->page.max - 1;
			}
			PageData_Del(wk->mw);
			CBE_pv_PageData_Create( wk->mw, wk->mw->page.no );
		//	CBE_pv_SealLoad( wk );		
			PageData_ResourceLoad( wk->mw );
			PageData_Add( wk->mw );
			
			Snd_SePlay( CB_SE_UP );
			break;
			
		case eCBE_DOWN:
		
			wk->mw->page.no++;
			wk->mw->page.no %= wk->mw->page.max;
			PageData_Del(wk->mw);
			CBE_pv_PageData_Create( wk->mw, wk->mw->page.no );
		//	CBE_pv_SealLoad( wk );		
			PageData_ResourceLoad( wk->mw );
			PageData_Add( wk->mw );
			
			Snd_SePlay( CB_SE_DOWN );
			break;
			
		case eCBE_PANEL:
			CBS_Enable( wk->mw, FALSE );
			CBE_pv_NumOAM_Enable( wk, TRUE );
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
			
			wk->btn_main = wk->btn_panel;
			wk->edit_old_no = wk->edit_no;
			
			CBE_pv_EditSealSave( wk );
			CBE_pv_EditSealDel( wk );
			
			Snd_SePlay( CB_SE_PLAY );
			break;
			
		case eCBE_SHOW:
		
			wk->seq = 1;
			wk->sub_seq = 0;
			Snd_SePlay( CB_SE_PLAY );
			break;
			
		case eCBE_SAVE:
			wk->save_or_load = 0;
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
			wk->btn_main = wk->btn_yes_no;
			Snd_SePlay( CB_SE_LIMIT );
			break;
			
		case eCBE_LOAD:
			wk->save_or_load = 1;
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
			wk->btn_main = wk->btn_yes_no;
			Snd_SePlay( CB_SE_LIMIT );
			break;	
		
		//case eCBE_SEAL_0:
		
		default:
			if ( event >= eCBE_SEAL_0 && event <= eCBE_SEAL_7 ){
				
				if ( CBS_IsPop( wk->mw ) == TRUE ){
					
					wk->mw->sys.hold = CBS_SearchPop( wk->mw, button );
					Snd_SePlay( CB_SE_POP );
				}
				else {
					
					Snd_SePlay( CB_SE_LIMIT );
				}
			}
			else if ( event >= eCBE_HIT_0 && event <= eCBE_HIT_7 ){
				Snd_SePlay( CB_SE_POP );
			}
			break;
		
			
		case eCBE_HIT_0:
		case eCBE_HIT_1:
		case eCBE_HIT_2:
		case eCBE_HIT_3:
		case eCBE_HIT_4:
		case eCBE_HIT_5:
		case eCBE_HIT_6:
		case eCBE_HIT_7:
			{
				int id;
				
				id = button - eCBE_HIT_0;
			
				CBS_SealHold( wk->mw, event, id );
			//	OS_Printf("id = %2d, index = [ %2d ]\n", wk->mw->seal[ id ].id, id);
				
				Snd_SePlay( CB_SE_POP );	
			}		
			break;
		
		}
		
	}
}

static void CBE_pv_PanelOAM_Change( CBE_WORK* wk, int num )
{
	int n1;
	int n2;
	int n3;
	
	n1 = (num / 100);
	n2 = (num % 100) / 10;
	n3 = (num % 100) % 10;
	
	CATS_ObjectAnimeSeqSetCap( wk->cap[ 0 ], n1 );
	CATS_ObjectAnimeSeqSetCap( wk->cap[ 1 ], n2 );
	CATS_ObjectAnimeSeqSetCap( wk->cap[ 2 ], n3 );
}

static void CBE_pv_PanelSys( CBE_WORK* wk, int button )
{
	static int button_tbl[] = {
		1,2,3,4,5,6,7,8,9,0,0xFF
	};
	
	int dat = button_tbl[ button ];
	
	if ( dat == 0xFF ){
		
		wk->edit_keta = 0xFF;
	}
	
	switch ( wk->edit_keta ){
	case 0:
		
		wk->edit_no = dat;
		wk->edit_keta++;
		break;
	
	case 1:
		
		wk->edit_no *= 10;
		wk->edit_no += dat;
		wk->edit_keta++;
		break;
		
	case 2:
		
		wk->edit_no *= 10;
		wk->edit_no += dat;
		wk->edit_keta++;
		break;
	
	case 3:
		break;
	
	default:
	
		wk->edit_no = 0;
		wk->edit_keta = 0;
		break;
	}
	
	if ( wk->edit_no >= (EDIT_CORE_MAX - 1) ){
		
		 wk->edit_no = EDIT_CORE_MAX - 1;
	}
	
	CBE_pv_PanelOAM_Change( wk, wk->edit_no );
	
//	OS_Printf( "桁 = %d num = %3d old = %3d\n", wk->edit_keta, wk->edit_no, wk->edit_old_no );
}

static void CBE_pv_PanelCallBack( u32 button, u32 event, void* work )
{
	CBE_WORK* wk;
	
	wk = (CBE_WORK*)work;
	
	///< 押されたときの反応
	if ( event == BMN_EVENT_TOUCH ){
		
		switch ( button ){
		
		case eCBE_P_1:
		case eCBE_P_2:
		case eCBE_P_3:
		case eCBE_P_4:
		case eCBE_P_5:
		case eCBE_P_6:
		case eCBE_P_7:
		case eCBE_P_8:
		case eCBE_P_9:
		case eCBE_P_0:
		case eCBE_P_C:
		
			CBE_pv_PanelSys( wk, button );
			Snd_SePlay( CB_SE_POP );
			break;

		case eCBE_P_E:
			CBE_pv_EditSealAdd( wk );
			CBS_Enable( wk->mw, TRUE );
			CBE_pv_NumOAM_Enable( wk, FALSE );
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
			wk->btn_main = wk->btn_edit;
			Snd_SePlay( CB_SE_PLAY );
			break;		
		}
	}
}

static void CBE_pv_YesNoCallBack( u32 button, u32 event, void* work )
{
	CBE_WORK* wk;
	
	wk = (CBE_WORK*)work;
	
	///< 押されたときの反応
	if ( event == BMN_EVENT_TOUCH ){
		
		switch ( button ){
		
		case 0:
			if ( wk->save_or_load == 0 ){
				OS_Printf( " mode = save --- save_ram : 0x%08x size : 0x%08x\n",&wk->edit_core[0], CBE_SAVE_SIZE );
				CBE_pv_EditSealSave( wk );
				CBE_pv_EditSealDel( wk );
				PMSVLD_Save( EDIT_BANK, &wk->edit_core[0], CBE_SAVE_SIZE);
				CBE_pv_EditSealAdd( wk );
				CBS_Enable( wk->mw, TRUE );
				CBE_pv_NumOAM_Enable( wk, FALSE );
			}
			else {
				OS_Printf( " mode = load --- save_ram : 0x%08x size : 0x%08x\n",&wk->edit_core[0], CBE_SAVE_SIZE );
				CBE_pv_EditSealSave( wk );
				CBE_pv_EditSealDel( wk );
				PMSVLD_Load( EDIT_BANK, &wk->edit_core[0], CBE_SAVE_SIZE);
				CBE_pv_EditSealAdd( wk );
				CBS_Enable( wk->mw, TRUE );
				CBE_pv_NumOAM_Enable( wk, FALSE );
			}
			Snd_SePlay( CB_SE_PLAY );
			break;
			
		case 1:			
			Snd_SePlay( CB_SE_LIMIT );
			break;		
		}
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
		wk->btn_main = wk->btn_edit;
	}	
}

static void CBE_pv_ButtonHitPanel( CBE_WORK* wk )
{
	int i;
	const RECT_HIT_TBL hit_tbl_dmy = { 0, 0, 0, 0 };
	
	for ( i = 0; i < EDIT_BTN_NUM; i++ ){
		wk->hit_tbl_edit[ i ] = hit_tbl_edit[ i ];
	}
	
	for ( ; i < EDIT_BTN_MAX; i++ ){
		wk->hit_tbl_edit[ i ] = hit_tbl_dmy;
		wk->mw->seal[ i - EDIT_BTN_NUM ].hit = &wk->hit_tbl_edit[ i ];
	}
	
	for ( i = 0; i < PANEL_BTN_NUM; i++ ){
		wk->hit_tbl_panel[ i ] = hit_tbl_panel[ i ];
	}
	
	wk->btn_edit  = BMN_Create(wk->hit_tbl_edit, EDIT_BTN_MAX,
							   CBE_pv_EditCallBack, wk, HEAPID_CUSTOM_BALL);
							  
	wk->btn_panel = BMN_Create(wk->hit_tbl_panel, PANEL_BTN_MAX,
							   CBE_pv_PanelCallBack, wk, HEAPID_CUSTOM_BALL);
							   					  
	wk->btn_yes_no = BMN_Create(hit_tbl_YesNo, 2,
							    CBE_pv_YesNoCallBack, wk, HEAPID_CUSTOM_BALL);
	
	wk->btn_main = wk->btn_edit;
	
	GF_ASSERT( wk->btn_main != NULL );
}


static void CBE_pv_DispInit( CB_MAIN_WORK* cbmw )
{
	ARCHANDLE* p_handle;
	ARCHANDLE* p_handle2;
	// カスタムボールグラフィックアーカイブハンドルオープン
	p_handle  = ArchiveDataHandleOpen( ARC_CUSTOM_BALL_GRA, HEAPID_CUSTOM_BALL );
	p_handle2 = ArchiveDataHandleOpen( ARC_TEST_FILE, HEAPID_CUSTOM_BALL );
	
	///< OBJ+BG読み込み

	CB_Select_TopResLoad(cbmw, p_handle);
//	CB_Select_BottomResLoad(cbmw, p_handle);
	{
		int arc_index = ARC_TEST_FILE;
		int chr_index = NARC_d_test_cb_bg_b_01_NCGR_bin;
		int scr_index = NARC_d_test_cb_bg_b_01_NSCR_bin;
		int pal_index = NARC_d_test_cb_bg_b_01_NCLR;
		int frame	  = CB_DEF_EDIT_SUB_BG_FRM;
		
		ArcUtil_HDL_BgCharSet(p_handle2, chr_index,
						  cbmw->sys.bgl, frame, 0, 0, 1, HEAPID_CUSTOM_BALL);
		ArcUtil_HDL_ScrnSet(p_handle2, scr_index,
						cbmw->sys.bgl, frame, 0, 0, 1, HEAPID_CUSTOM_BALL);
		PaletteWorkSet_Arc(cbmw->sys.pfd,
						   arc_index, pal_index,
						   HEAPID_CUSTOM_BALL, FADE_SUB_BG, 0x20 * 5, CB_ENUM_PAL_SEL_BG);
						   
		
		///< PANEL
		scr_index = NARC_d_test_cbe_NSCR_bin;
		frame	  = CB_DEF_LIST_SUB_BG_FRM;
						  
		ArcUtil_HDL_BgCharSet(p_handle2, chr_index,
						  cbmw->sys.bgl, frame, 0, 0, 1, HEAPID_CUSTOM_BALL);
		ArcUtil_HDL_ScrnSet(p_handle2, scr_index,
						cbmw->sys.bgl, frame, 0, 0, 1, HEAPID_CUSTOM_BALL);

		///< SAVE LOAD
		scr_index = NARC_d_test_cbe2_NSCR_bin;
		frame	  = CB_DEF_BALL_SUB_BG_FRM;
						  
		ArcUtil_HDL_ScrnSet(p_handle2, scr_index,
						cbmw->sys.bgl, frame, 0, 0, 1, HEAPID_CUSTOM_BALL);
						
		scr_index = NARC_d_test_cbe3_NSCR_bin;
		frame	  = CB_DEF_STRING_SUB_BG_FRM;
						  
		ArcUtil_HDL_ScrnSet(p_handle2, scr_index,
						cbmw->sys.bgl, frame, 0, 0, 1, HEAPID_CUSTOM_BALL);
	}

	CB_DispOn();
//	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );		///< 3D
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
//	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );		///< BG
	
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
//	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );	///< PANEL
//	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );	///< BG
	CB_Sys_MainTPSet(cbmw, FALSE);

	// カスタムボールグラフィックアーカイブハンドルクローズ
	ArchiveDataHandleClose( p_handle );
	ArchiveDataHandleClose( p_handle2 );
}

///< pv_MainEdit
static void CBE_pv_MainEdit( CBE_WORK* wk )
{
	CB_MAIN_WORK*	cbmw;
	
	cbmw = wk->mw;
	
			
	if ( cbmw->sys.hold == CB_DEF_NOT_HOLD ){
		
		BMN_Main( wk->btn_main );
	}
	else {
		u32 x, y;
		int id;
		BOOL bHit;
		
		id = cbmw->sys.hold;
		
		bHit = GF_TP_GetPointCont( &x, &y );
		
		if ( bHit ){
			
			CATS_ObjectPosSetCap( cbmw->seal[ id ].cap, x, y );
			CBS_ReleasePointCheckTypeSet( cbmw, id );
			CBS_EditDataRefrect( cbmw, id );
		}
		else {
			BOOL release;
			
			release = CBS_ReleasePointCheck( cbmw, id );
			
			HitTable_CreateCap( cbmw->seal[ id ].hit, cbmw->seal[ id ].cap, FALSE );
			
			if ( release == FALSE ){
				
				CBS_Release(cbmw, id);
			}
				
			Snd_SePlay( CB_SE_DROP );
			cbmw->sys.hold = CB_DEF_NOT_HOLD;
		}
	}
}

///< pv_Panel
static void CBE_pv_PanelControl( CBE_WORK* wk )
{
}



// -----------------------------------------------------------------------------
//
//	□■□■　プロック関連　□■□■
//
// -----------------------------------------------------------------------------
static PROC_RESULT	CBE_ProcInit(PROC* proc, int* seq)
{
	CBE_WORK*		wk;
	CB_MAIN_WORK*	cbmw;
	CB_PROC_WORK*	cbpw;
	
	///< 切り替え時に初期化していたもの
	{	
		sys_CreateHeap( HEAPID_BASE_APP, HEAPID_CUSTOM_BALL, CB_DEF_HEAP_SIZE );
		
		wk = PROC_AllocWork( proc, sizeof( CBE_WORK ), HEAPID_CUSTOM_BALL );
		
		wk->mw = sys_AllocMemory( HEAPID_CUSTOM_BALL, sizeof( CB_MAIN_WORK ) );
		memset( wk->mw, 0, sizeof( CB_MAIN_WORK ) );
		
		wk->p_wk = sys_AllocMemory( HEAPID_CUSTOM_BALL, sizeof( CB_PROC_WORK ) );
		memset( wk->mw, 0, sizeof( CB_PROC_WORK ) );
		
		wk->p_wk->save 		= ( (MAINWORK*)PROC_GetParentWork( proc ) )->savedata;
		wk->p_wk->cfg  		= SaveData_GetConfig( wk->p_wk->save );
		wk->p_wk->save_data = CB_SaveData_AllDataGet( wk->p_wk->save );
		
		wk->edit_no			= 0;
		
		Debug_CB_SaveData_Sample( wk->p_wk->save_data, DEBUG_CB_MODE_CLEAR );
	}
	
	{		
		int i;
		int poke_cnt;
		wk->p_wk->pparty = SaveData_GetTemotiPokemon( wk->p_wk->save );
		poke_cnt = PokeParty_GetPokeCount( wk->p_wk->pparty );
		wk->p_wk->poke_cnt = poke_cnt;
		for (i = 0; i < poke_cnt; i++){
			wk->p_wk->poke_para[i] = PokeParty_GetMemberPointer( wk->p_wk->pparty, i );
		}
		for ( ; i < TEMOTI_POKEMAX; i++){
			wk->p_wk->poke_para[i] = NULL;
		}
	}
	
	
	///< ProcInitで初期化していたもの
	{
		CB_Tool_SystemInit( HEAPID_CUSTOM_BALL );
		CB_Tool_DefaultBlendSet();
		cbmw = wk->mw;
		cbmw->sys.g3Dman = CB_3D_Init();
		
		cbpw = wk->p_wk;
		cbmw->cb_pw = cbpw;
		cbmw->handle = ArchiveDataHandleOpen( ARC_PM_EDIT, HEAPID_CUSTOM_BALL );
		
		cbmw->test_pp = PokemonParam_AllocWork( HEAPID_CUSTOM_BALL );
//		CB_Tool_TestPokemonCreate( cbmw );
		
		cbmw->sys.hold	= 0xFF;
		cbmw->page.no	= 0;
		cbmw->page.max	= (CB_Tool_SealCllectionGet(cbmw->cb_pw->save_data) / CB_DEF_PAGE_ITEMS) + 1;
		
			
		if ( cbmw->page.max > CB_DEF_PAGE_MAX ){
			 cbmw->page.max = CB_DEF_PAGE_MAX;
		}
		cbmw->page.max = CB_DEF_PAGE_MAX;
		
		cbmw->select[ 0 ] = CBProc_SelectPosGet( cbmw->cb_pw );
		cbmw->select[ 1 ] = CBProc_SelectPosGet( cbmw->cb_pw );
		
		///< エディットデータ
		{
			int i, j;
			for ( i = 0; i < EDIT_CORE_MAX; i++ ){
				
				for ( j = 0; j < CB_DEF_SEAL_MAX; j++ ){
				
					wk->edit_core[ i ].cb_seal[ j ].seal_id	= 0;
					wk->edit_core[ i ].cb_seal[ j ].x		= 0xFF;
					wk->edit_core[ i ].cb_seal[ j ].y		= 0xFF;
				}
			}
		}
		
		///< システム系の初期化
		{			
			cbmw->sys.bgl = GF_BGL_BglIniAlloc( HEAPID_CUSTOM_BALL );
			initVramTransferManagerHeap( CB_DEF_VRAM_TRANSFER_TASK_NUM, HEAPID_CUSTOM_BALL );
			
			cbmw->sys.pfd = PaletteFadeInit( HEAPID_CUSTOM_BALL );
			PaletteTrans_AutoSet( cbmw->sys.pfd, TRUE );
			PaletteFadeWorkAllocSet( cbmw->sys.pfd, FADE_MAIN_BG,  0x200, HEAPID_CUSTOM_BALL );
			PaletteFadeWorkAllocSet( cbmw->sys.pfd, FADE_SUB_BG,   0x200, HEAPID_CUSTOM_BALL );
			PaletteFadeWorkAllocSet( cbmw->sys.pfd, FADE_MAIN_OBJ, 0x200, HEAPID_CUSTOM_BALL );
			PaletteFadeWorkAllocSet( cbmw->sys.pfd, FADE_SUB_OBJ,  0x200, HEAPID_CUSTOM_BALL );	
			
			CBE_pv_VramBankSet( cbmw->sys.bgl );
			
			CB_Particle_Init();
			
			cbmw->sys.ssm_p = SoftSpriteInit( HEAPID_CUSTOM_BALL );
			cbmw->sys.pas = PokeAnm_AllocMemory( HEAPID_CUSTOM_BALL, 1, 0 );
			
			///< ウィンドウ
			{
				int win_type;
				
				win_type = CONFIG_GetWindowType( cbmw->cb_pw->cfg );
				CB_BMP_WindowResLoad( cbmw->sys.bgl, cbmw->sys.pfd, win_type );
				CB_BMP_WindowResLoad_Sub( cbmw->sys.bgl, cbmw->sys.pfd, win_type );
			}
			
			///< OAM
			CB_Tool_CatsInit( &cbmw->sys );
			
			///< TP
			{
				u32 active;
				InitTPSystem();
				active = InitTPNoBuff( 4 );
				if(active != TP_OK){
					OS_Printf("custom ball != TP_OK\n");
				}				
			}
						
			///< ボタンシステム
			// CB_Tool_ButtonInit(cbmw);

			sys_VBlankFuncChange(CB_Tool_VBlank, cbmw);
			
			///< --- font oam
			FontOam_SysInit(cbmw);
			
			///< サウンドデータロード(カスタムボール)(BGM引継ぎ)
			Snd_DataSetByScene( SND_SCENE_SUB_CUSTOM, 0, 0 );

			OS_Printf("□ custom ball init\n");
		}
	}
	
	///< ボタンの設定
	{
		CBE_pv_ButtonHitPanel( wk );
	}
	
	///< Itemデータの取得
	{
		wk->mw->cb_item_data = CB_SaveData_ItemDataGet(wk->mw->cb_pw->save_data);
		
		
	}
	
	///< OAM初期化
	{
		CBE_pv_PageData_Create( cbmw, cbmw->page.no );
		CBE_pv_SealLoad( wk );		
		PageData_ResourceLoad( wk->mw );
		PageData_Add( wk->mw );
		
		CBE_pv_NumOAM_Create( wk );
	}
	
	*seq = 0;
	wk->seq = 0;
	
	return PROC_RES_FINISH;
}


#define	AFF_SPEED		(0x20)
static void CB_Test_PokemonSSP_AnimeInit(CB_MAIN_WORK* cbmw)
{
	int monsno;
	int chr;
	
	monsno = PokeParaGet(cbmw->test_pp, ID_PARA_monsno, NULL);
	chr	   = PokeSeikakuGet(cbmw->test_pp);
	
	SoftSpriteAnimeSet(cbmw->sys.ssp, 1);
	
	PokeEditData_PrgAnimeSet( cbmw->handle,
							  cbmw->sys.pas,
							  cbmw->sys.ssp,
							  monsno,
							  PARA_FRONT,
							  PARA_HFLIP_OFF,
							  0 );
}

static void CB_Test_PokemonSSP_AppearInit(CB_MAIN_WORK* cbmw)
{
	SoftSpriteParaSet(cbmw->sys.ssp, SS_PARA_AFF_X, AFF_APPEAR_X_S);
	SoftSpriteParaSet(cbmw->sys.ssp, SS_PARA_AFF_Y, AFF_APPEAR_Y_S);	
}

static BOOL CB_Test_PokemonSSP_AppearMain(CB_MAIN_WORK* cbmw)
{
	if(SoftSpriteParaGet(cbmw->sys.ssp, SS_PARA_AFF_X) == AFF_APPEAR_X_B){
		return FALSE;
	}
	else if(SoftSpriteParaGet(cbmw->sys.ssp, SS_PARA_AFF_X) >= AFF_APPEAR_X_B){
		SoftSpriteParaSet(cbmw->sys.ssp, SS_PARA_AFF_X, AFF_APPEAR_X_B);
		SoftSpriteParaSet(cbmw->sys.ssp, SS_PARA_AFF_Y, AFF_APPEAR_Y_B);
		return FALSE;
	}
	else {
		SoftSpriteParaCalc(cbmw->sys.ssp, SS_PARA_AFF_X, AFF_SPEED);
		SoftSpriteParaCalc(cbmw->sys.ssp, SS_PARA_AFF_Y, AFF_SPEED);
		SoftSpriteOYCalc(cbmw->sys.ssp, cbmw->sys.height);
	}
	return TRUE;
}
static BOOL CB_Test_PokemonSSP_ReturnMain(CB_MAIN_WORK* cbmw)
{
	if(SoftSpriteParaGet(cbmw->sys.ssp, SS_PARA_AFF_X) == AFF_APPEAR_X_S){
		return FALSE;
	}
	else if(SoftSpriteParaGet(cbmw->sys.ssp, SS_PARA_AFF_X) <= AFF_APPEAR_X_S){
		SoftSpriteParaSet(cbmw->sys.ssp, SS_PARA_AFF_X, AFF_APPEAR_X_S);
		SoftSpriteParaSet(cbmw->sys.ssp, SS_PARA_AFF_Y, AFF_APPEAR_Y_S);
		return FALSE;
	}
	else {
		SoftSpriteParaCalc(cbmw->sys.ssp, SS_PARA_AFF_X, -AFF_SPEED);
		SoftSpriteParaCalc(cbmw->sys.ssp, SS_PARA_AFF_Y, -AFF_SPEED);
		SoftSpriteOYCalc(cbmw->sys.ssp, cbmw->sys.height);
	}
	return TRUE;
}

static PROC_RESULT	CBE_ProcMain(PROC* proc, int* seq)
{
	CBE_WORK*		wk;
	CB_MAIN_WORK*	cbmw;
	CB_PROC_WORK*	cbpw;
	
	wk = PROC_GetWork( proc );
	
	cbmw = wk->mw;
	cbpw = wk->p_wk;
	
	switch( *seq ){
	
	case 0:
		{
			///< 画面初期化
			CBE_pv_DispInit( cbmw );
			*seq = 1;
		}		
		break;
		
	case 1:
		{
			///< メイン処理
			
			switch ( wk->seq ){
			case 0:
				CBE_pv_MainEdit( wk );
				break;
				
			case 1:
			
				switch ( cbmw->sub_seq ){
				case 0:
					cbmw->sub_work = 0;				
					//CB_Tool_TestPokemonCreate(cbmw);
					PokeParaInit( cbmw->test_pp );
					{
						int dat = MONSNO_KIMAIRAN;
						
						PokeParaPut( cbmw->test_pp, ID_PARA_monsno, &dat );
						
						dat = 0x101010;

						PokeParaPut( cbmw->test_pp, ID_PARA_personal_rnd, &dat );
					}
					CB_Test_PokemonSSP_Add(cbmw);
					CB_Test_PokemonSSP_Vanish(cbmw, 1);
					cbmw->sub_seq++;
					break;
				
				case 1:
					{
						TBALL_MOVE_DATA bmd;

						bmd.type	= EBMT_STAY_BB;
						bmd.heap_id	= HEAPID_CUSTOM_BALL;
						bmd.mode	= EBMM_THROW;
						bmd.id		= BM_SUITABLE_ID;
						bmd.bg_pri	= BM_BG_PRI_CUSTOM;
						bmd.surface = 1;
						bmd.csp		= cbmw->sys.csp;
						bmd.pfd		= cbmw->sys.pfd;
						bmd.ball_id = PokeParaGet(cbmw->test_pp, ID_PARA_get_ball, NULL);

						cbmw->sys.bms	= BM_Init(&bmd);
						cbmw->sub_seq++;
					}
					break;
				
				case 2:
					{
						int		test_id;
						CB_CORE	test_core;
						TBALL_CUSTOM_PARAM bcp = { 0 };
						
						bcp.client_type = CLIENT_TYPE_BB;	
						bcp.pp			= cbmw->test_pp;
						
						test_id = cbmw->select[0] + 1;
						CBS_TestCoreData_Create(&test_core, cbmw);
						
						PokeParaPut(cbmw->test_pp, ID_PARA_cb_id,	(u8*)&test_id);
						PokeParaPut(cbmw->test_pp, ID_PARA_cb_core,	&test_core);
						
						cbmw->sys.bes	= BallEffect_Init(HEAPID_CUSTOM_BALL, &bcp);
						
						BallEffect_ResourceLoad(cbmw->sys.bes);
					}
					cbmw->sub_seq++;
					cbmw->sub_work = 0;
					break;
					
				case 3:
					
					if (BallEffect_IsLoadEnd(cbmw->sys.bes) != TRUE){
						break;
					}
				
					if (BM_IsOpen(cbmw->sys.bms) == TRUE){
						
						CB_Test_PokemonSSP_AppearMain(cbmw);
				
						BallEffect_ExecutedEz(cbmw->sys.bes);
						
						CB_Test_PokemonSSP_Vanish(cbmw, 0);
						CB_Test_PokemonSSP_AppearInit(cbmw);
						
						Snd_SePlay( CB_SE_BOMB );
					
						cbmw->sub_seq++;
					}
				
					break;
				
				case 4:
					{
						BOOL ret = CB_Test_PokemonSSP_AppearMain(cbmw);
						
						if (BM_EndCheck(cbmw->sys.bms) != FALSE){ break; }
						
						if (ret == FALSE){
															
							BM_Delete(cbmw->sys.bms);
							GF_Disp_GX_VisibleControl(GX_PLANEMASK_OBJ, VISIBLE_OFF);
							
							CB_Test_PokemonSSP_AnimeInit(cbmw);
							cbmw->sub_work = 0;
							cbmw->sub_seq++;
						}
					}
					break;
				case 5:
					if (BallEffect_EndCheck(cbmw->sys.bes) != FALSE){ break; }
					if (PokeAnm_IsFinished(cbmw->sys.pas, 0) != TRUE){ break; }
					if (SoftSpriteAnimeEndCheck(cbmw->sys.ssp) != FALSE){ break; }
					BallEffect_FreeMemory(cbmw->sys.bes);
					cbmw->sub_seq++;
					break;
					
				case 6:
					if ((++cbmw->sub_work) < CB_DEF_POKE_RETURN_WAIT){ break; }
					cbmw->sub_work = 0;
					{
						TBALL_MOVE_DATA bmd;

						bmd.type	= EBMT_STAY_BB;
						bmd.heap_id	= HEAPID_CUSTOM_BALL;
						bmd.mode	= EBMM_CLOSE;
						bmd.id		= BM_SUITABLE_ID;
						bmd.bg_pri	= BM_BG_PRI_CUSTOM;
						bmd.surface = 1;
						bmd.csp		= cbmw->sys.csp;
						bmd.pfd		= cbmw->sys.pfd;
						bmd.ball_id = PokeParaGet(cbmw->test_pp, ID_PARA_get_ball, NULL);

						cbmw->sys.bms	= BM_Init(&bmd);
						
						CB_Tool_DefaultBlendSet();
						GF_Disp_GX_VisibleControl(GX_PLANEMASK_OBJ, VISIBLE_ON);
						
						cbmw->sub_seq++;
					}
					break;
						
						
				case 7:
					{
						BOOL ret = CB_Test_PokemonSSP_ReturnMain(cbmw);
						
						if (BM_EndCheck(cbmw->sys.bms) == FALSE && ret == FALSE){
							CB_Test_PokemonSSP_Vanish(cbmw, 1);
							CB_Test_PokemonSSP_Del(cbmw);
							BM_Delete(cbmw->sys.bms);
							cbmw->sub_seq++;
						}
					}
					break;
					
				default:
					if (PaletteFadeCheck(cbmw->sys.pfd) != 0){ break; }
					wk->seq = 0;
					wk->sub_seq = 0;
					cbmw->sub_seq = 0;
					break;
				}
			
				break;
			}
			
			CBE_pv_NumOAM_Update( wk );
			
			CATS_Draw( wk->mw->sys.crp );
			SoftSpriteMain( cbmw->sys.ssm_p );
			CB_Particle_Main();
		}
		break;
	
	case 2:
		{

		}
		break;
	}	


	return PROC_RES_CONTINUE;
}


static PROC_RESULT	CBE_ProcEnd(PROC* proc, int* seq)
{
#if 0
	CBE_WORK*		wk;
	CB_MAIN_WORK*	cbmw;
	CB_PROC_WORK*	cbpw;
	
	wk = PROC_GetWork( proc );
	
	cbmw = wk->mw;
	cbpw = wk->p_wk;
	
	GF_Disp_GX_VisibleControl(GX_PLANEMASK_BG0,  VISIBLE_OFF);
	GF_Disp_GX_VisibleControl(GX_PLANEMASK_BG1,  VISIBLE_OFF);
	GF_Disp_GX_VisibleControl(GX_PLANEMASK_BG2,  VISIBLE_OFF);
	GF_Disp_GX_VisibleControl(GX_PLANEMASK_BG3,  VISIBLE_OFF);
	GF_Disp_GXS_VisibleControl(GX_PLANEMASK_BG0, VISIBLE_OFF);
	GF_Disp_GXS_VisibleControl(GX_PLANEMASK_BG1, VISIBLE_OFF);
	GF_Disp_GXS_VisibleControl(GX_PLANEMASK_BG2, VISIBLE_OFF);
	GF_Disp_GXS_VisibleControl(GX_PLANEMASK_BG3, VISIBLE_OFF);
	GF_BGL_BGControlExit(cbmw->sys.bgl, GF_BGL_FRAME1_M);
	GF_BGL_BGControlExit(cbmw->sys.bgl, GF_BGL_FRAME2_M);
	GF_BGL_BGControlExit(cbmw->sys.bgl, GF_BGL_FRAME3_M);
	GF_BGL_BGControlExit(cbmw->sys.bgl, GF_BGL_FRAME0_S);
	GF_BGL_BGControlExit(cbmw->sys.bgl, GF_BGL_FRAME1_S);
	GF_BGL_BGControlExit(cbmw->sys.bgl, GF_BGL_FRAME2_S);
	GF_BGL_BGControlExit(cbmw->sys.bgl, GF_BGL_FRAME3_S);
	

	sys_FreeMemoryEz(cbmw->sys.bgl);
	
	PaletteFadeWorkAllocFree(cbmw->sys.pfd, FADE_MAIN_BG);
	PaletteFadeWorkAllocFree(cbmw->sys.pfd, FADE_SUB_BG);
	PaletteFadeWorkAllocFree(cbmw->sys.pfd, FADE_MAIN_OBJ);
	PaletteFadeWorkAllocFree(cbmw->sys.pfd, FADE_SUB_OBJ);
	PaletteFadeFree(cbmw->sys.pfd);
	
	CBProc_SelectPosSet(cbmw->cb_pw, cbmw->select[0]);

	sys_FreeMemoryEz(cbmw->test_pp);
	CBS_Delete(cbmw);
	BMN_Delete(cbmw->sys.btn);
	
	///< soft sprite
	SoftSpriteEnd(cbmw->sys.ssm_p);
	PokeAnm_FreeMemory(cbmw->sys.pas);
	
	///< font oam
//	FontOam_SysDelete(cbmw);
	
	CB_Tool_CatsDelete(&cbmw->sys);
		
	DellVramTransferManager();
	
	
	GF_G3D_Exit(cbmw->sys.g3Dman);
	
	CB_Tool_SystemDelete( HEAPID_CUSTOM_BALL );
	
	ArchiveDataHandleClose( cbmw->handle );
	
	sys_FreeMemoryEz(cbmw);
	sys_FreeMemoryEz(cbpw);
		
	PROC_FreeWork(proc);	///< cbmw free
	
	///< タッチパネル
	{
		u32 active;
		active = StopTP();
		if(active != TP_OK){
			OS_Printf("custom ball != TP_OK\n");
		}
	}
	
	sys_DeleteHeap( HEAPID_CUSTOM_BALL );
		
	Overlay_UnloadID(FS_OVERLAY_ID(bc_common));
	
	OS_Printf("□ custom ball end\n");	
	
#endif
	return PROC_RES_FINISH;
}

#endif	//PM_DEBUG
