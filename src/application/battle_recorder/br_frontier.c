//==============================================================================
/**
 * @file	br_frontier.c
 * @brief	簡単な説明を書く
 * @author	goto
 * @date	2007.09.14(金)
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
#include "msgdata/msg.naix"
#include "system/brightness.h"
#include "system/fontproc.h"
#include "system/lib_pack.h"
#include "gflib/msg_print.h"
#include "gflib/touchpanel.h"
#include "poketool/poke_tool.h"
#include "poketool/pokeicon.h"
#include "poketool/boxdata.h"
#include "system/snd_tool.h"
#include "gflib/strbuf_family.h"
#include "system/pm_overlay.h"
#include "system/wipe.h"

#include "src/field/syswork.h"
#include "src/field/sysflag.h"
#include "src/frontier/frontier_def.h"

#include "br_private.h"

/*

	上画面をメイン
	下画面をサブ	
	に入れ替えて使用しています。

*/

#define FREC_BGOBJ_MAX		( 5 )
#define FREC_HUMOBJ_MAX		( FREC_BGOBJ_MAX * 2 )
#define FREC_BP_MAX			( 4 )
#define FREC_WIN_MAX		( 1 )

#define FREC_NUM_PX			( 88 )
#define FREC_NUM_PY			( 72 )

enum {
	
	eFREC_PRI_BG	= 20,
	eFREC_PRI_HERO	= 10,
};

#define BR_FREC_PAL_NO		( eBG_PAL_FONT )


typedef struct {
	
	int color;

	f32				scale[ FREC_BGOBJ_MAX ];		///< サイズ
	int				range[ FREC_BGOBJ_MAX ];		///< 距離
	int				rad[ FREC_BGOBJ_MAX ];			///< 角度
	CATS_ACT_PTR	cap_hero[ FREC_BGOBJ_MAX ];		///< 人物OBJ 上
	CATS_ACT_PTR	cap_icon[ FREC_BGOBJ_MAX ];		///< 人物OBJ 上
	CATS_ACT_PTR	cap_bg[ FREC_BGOBJ_MAX ];		///< 背景OBJ 上
	
	CATS_ACT_PTR	cap_num[ FREC_BP_MAX + 1 ];		///< 数字OBJ 下
	
	int b_point;
	
	CATS_ACT_PTR	sbtn[ 2 ];
	OAM_BUTTON		obtn[ 2 ];					///< 送る　戻る　送るは常にビジブル
	
	int				eva;
	int				evb;
		
} FREC_WORK;

//==============================================================
// Prototype
//==============================================================
static void ButtonOAM_PoeSet( BR_WORK* wk, int mode );
static void ExTag_ResourceLoad_Small( BR_WORK* wk, int draw_area );
static void FontButton_Create( BR_WORK* wk );
static void FontButton_Delete( BR_WORK* wk );
static void BR_frec_ResLoad( BR_WORK* wk );
static void PalGrayScale(u16 *pal, u16 pal_start, u16 pal_size);
static void BR_frec_ResAdd( BR_WORK* wk );
static void BR_frec_ResDel( BR_WORK* wk );
static BOOL BR_frec_PhotoPosRollInSet( BR_WORK* wk );
static BOOL BR_frec_PhotoPosRollOutSet( BR_WORK* wk );
static void BR_frec_PhotoPosSet( BR_WORK* wk );
static BOOL BR_frec_Init( BR_WORK* wk );
static BOOL BR_frec_Main( BR_WORK* wk );
static BOOL BR_frec_End( BR_WORK* wk );



BOOL (* const BR_frec_MainTable[])( BR_WORK* wk ) = {
	BR_frec_Init,
	BR_frec_Main,
	BR_frec_End,
};


static void ButtonOAM_PoeSet( BR_WORK* wk, int mode )
{
	FREC_WORK* swk = wk->sub_work;
	
	if ( mode == 0 ){
		CATS_ObjectPosSetCap( swk->obtn[ 1 ].cap, 128, 232 );
	}
	else {
		CATS_ObjectPosSetCap( swk->obtn[ 1 ].cap, 80, 232 );
	}
	FONTOAM_SetMat( swk->obtn[ 1 ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
}

static void ExTag_ResourceLoad_Small( BR_WORK* wk, int draw_area )
{
	ARCHANDLE*		 hdl = wk->sys.p_handle;
	GF_BGL_INI*		 bgl = wk->sys.bgl;
	CATS_SYS_PTR	 csp = wk->sys.csp;
	CATS_RES_PTR	 crp = wk->sys.crp;
	PALETTE_FADE_PTR pfd = wk->sys.pfd;
	CATS_LoadResourceCharArcH( csp, crp, hdl, NARC_batt_rec_gra_batt_rec_gds_tag3_NCGR, FALSE, draw_area, eID_OAM_EX_TAG );
	CATS_LoadResourceCellArcH( csp, crp, hdl, NARC_batt_rec_gra_batt_rec_tag3__gds_64k_NCER, FALSE, eID_OAM_EX_TAG );
	CATS_LoadResourceCellAnmArcH( csp, crp, hdl, NARC_batt_rec_gra_batt_rec_tag3__gds_64k_NANR, FALSE, eID_OAM_EX_TAG );
}

static void FontButton_Create( BR_WORK* wk )
{
	FREC_WORK* swk = wk->sub_work;

	ExTag_ResourceLoad_Small( wk, NNS_G2D_VRAM_TYPE_2DSUB );		
	swk->obtn[ 1 ].cap = ExTag_Add( wk, NNS_G2D_VRAM_TYPE_2DSUB );
	FontOam_Create( &swk->obtn[ 1 ], &wk->sys, NNS_G2D_VRAM_TYPE_2DSUB );
	FontOam_MsgSet( &swk->obtn[ 1 ], &wk->sys, msg_05 );
	CATS_ObjectPosSetCap( swk->obtn[ 1 ].cap, 128, 232 );
	FONTOAM_SetMat( swk->obtn[ 1 ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
	FONTOAM_SetDrawFlag( swk->obtn[ 1 ].font_obj, TRUE );	
	swk->sbtn[ 1 ] = ExTag_Add( wk, NNS_G2D_VRAM_TYPE_2DSUB );	
	CATS_ObjectPosSetCap( swk->sbtn[ 1 ], 220, 160 + 64 );
	CATS_ObjectAnimeSeqSetCap( swk->sbtn[ 1 ], 2 );
	CATS_ObjectEnableCap( swk->sbtn[ 1 ], FALSE );	
	ButtonOAM_PoeSet( wk, 1 );	
}

static void FontButton_Delete( BR_WORK* wk )
{
	FREC_WORK* swk = wk->sub_work;	
	FontOam_Delete( &swk->obtn[ 1 ] );
	CATS_ActorPointerDelete_S( swk->obtn[ 1 ].cap );
	CATS_ActorPointerDelete_S( swk->sbtn[ 1 ] );	
	ExTag_ResourceDelete( wk );			
}


//--------------------------------------------------------------
/**
 * @brief	OAMリソース読み込み
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BR_frec_ResLoad( BR_WORK* wk )
{
	FREC_WORK* swk = wk->sub_work;
		
	ARCHANDLE*		 hdl = wk->sys.p_handle;
	GF_BGL_INI*		 bgl = wk->sys.bgl;
	CATS_SYS_PTR	 csp = wk->sys.csp;
	CATS_RES_PTR	 crp = wk->sys.crp;
	PALETTE_FADE_PTR pfd = wk->sys.pfd;
	
	///< BG  下画面
	ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR, wk->sys.bgl, GF_BGL_FRAME2_S, 0, 0, 0, HEAPID_BR );
	ArcUtil_HDL_ScrnSet( hdl, NARC_batt_rec_gra_batt_rec_browse_bg1d_NSCR, bgl, GF_BGL_FRAME2_S, 0, 0, 0, HEAPID_BR );
	
	///< OAM 下画面
	CATS_LoadResourcePlttWorkArcH( pfd, FADE_SUB_OBJ,  csp, crp, hdl, BR_ColorPaletteID_Get( wk, GET_PAL_FONT ), FALSE, 1, NNS_G2D_VRAM_TYPE_2DSUB,  eID_OAM_BP_NUM );
	CATS_LoadResourceCharArcH( csp, crp, hdl, NARC_batt_rec_gra_batt_rec_browse_bpfont_NCGR, FALSE, NNS_G2D_VRAM_TYPE_2DSUB, eID_OAM_BP_NUM );
	CATS_LoadResourceCellArcH( csp, crp, hdl, NARC_batt_rec_gra_batt_rec_bpfont_64k_NCER, FALSE, eID_OAM_BP_NUM );
	CATS_LoadResourceCellAnmArcH( csp, crp, hdl, NARC_batt_rec_gra_batt_rec_bpfont_64k_NANR, FALSE, eID_OAM_BP_NUM );
	
	///< OAM 上画面 背景
	CATS_LoadResourcePlttWorkArcH( pfd, FADE_MAIN_OBJ, csp, crp, hdl, BR_ColorPaletteID_Get( wk, GET_PAL_PHOTO ), FALSE, 6, NNS_G2D_VRAM_TYPE_2DMAIN, eID_OAM_BG_PHOTO );
	CATS_LoadResourceCharArcH( csp, crp, hdl, NARC_batt_rec_gra_batt_rec_browse_photo_NCGR, FALSE, NNS_G2D_VRAM_TYPE_2DMAIN,  eID_OAM_BG_PHOTO );
	CATS_LoadResourceCellArcH( csp, crp, hdl, NARC_batt_rec_gra_batt_rec_photo_64k_NCER, FALSE, eID_OAM_BG_PHOTO );
	CATS_LoadResourceCellAnmArcH( csp, crp, hdl, NARC_batt_rec_gra_batt_rec_photo_64k_NANR, FALSE, eID_OAM_BG_PHOTO );
	
	///< OAM 上画面 人物
	{
		int i;			
		int	ncl[ FREC_BGOBJ_MAX ] = { NARC_wifi2dchar_towerboss_NCLR, NARC_wifi2dchar_brains1_NCLR, NARC_wifi2dchar_brains4_NCLR, NARC_wifi2dchar_brains3_NCLR, NARC_wifi2dchar_brains2_16_NCLR };
		int ncg[ FREC_BGOBJ_MAX ] = { NARC_wifi2dchar_towerboss_NCGR, NARC_wifi2dchar_brains1_NCGR, NARC_wifi2dchar_brains4_NCGR, NARC_wifi2dchar_brains3_NCGR, NARC_wifi2dchar_brains2_NCGR };
		int	nce = NARC_wifi2dchar_npc_NCER;
		int nca = NARC_wifi2dchar_npc_NANR;
		
		for ( i = 0; i < FREC_BGOBJ_MAX; i++ ){
			if ( i == 3 ){
				CATS_LoadResourcePlttWorkArcH( pfd, FADE_MAIN_OBJ, csp, crp, hdl, NARC_batt_rec_gra_bf_brain3p_NCLR, 0, 1, NNS_G2D_VRAM_TYPE_2DMAIN, eID_OAM_HUM_ICON + i );
				CATS_LoadResourceCharArcH( csp, crp, hdl, NARC_batt_rec_gra_bf_brain3p_NCGR, 0, NNS_G2D_VRAM_TYPE_2DMAIN, eID_OAM_HUM_ICON + i );
			}
			else {
				CATS_LoadResourcePlttWorkArc( pfd, FADE_MAIN_OBJ, csp, crp, ARC_WIFI2DCHAR, ncl[ i ], 0, 1, NNS_G2D_VRAM_TYPE_2DMAIN, eID_OAM_HUM_ICON + i );
				CATS_LoadResourceCharArcModeAdjustAreaCont(	csp, crp, ARC_WIFI2DCHAR, ncg[ i ], 0, NNS_G2D_VRAM_TYPE_2DMAIN, eID_OAM_HUM_ICON + i );
			}
		}
		CATS_LoadResourceCellArc( csp, crp, ARC_WIFI2DCHAR, nce, 0, eID_OAM_HUM_ICON );
		CATS_LoadResourceCellAnmArc( csp, crp, ARC_WIFI2DCHAR, nca, 0, eID_OAM_HUM_ICON );
		
		///< 3P用
		CATS_LoadResourceCellArcH( csp, crp, hdl, NARC_batt_rec_gra_bf_brain3p_NCER, FALSE, eID_OAM_HUM_ICON + 1 );
		CATS_LoadResourceCellAnmArcH( csp, crp, hdl, NARC_batt_rec_gra_bf_brain3p_NANR, FALSE, eID_OAM_HUM_ICON + 1 );
	}
	
	///< OAM 上画面 主人公
	{
		int	ncl = NARC_wifi2dchar_pl_boy01_NCLR;
		int ncg = NARC_wifi2dchar_pl_boy01_NCGR;
		int	nce = NARC_wifi2dchar_npc_NCER;
		int nca = NARC_wifi2dchar_npc_NANR;
		int sex = GDS_Profile_GetSex( wk->br_gpp[ 0 ] );

		csp = wk->sys.csp;
		crp = wk->sys.crp;
		pfd = wk->sys.pfd;
		
		if ( sex != PM_MALE ){
			ncl = NARC_wifi2dchar_pl_girl01_NCLR;
			ncg = NARC_wifi2dchar_pl_girl01_NCGR;
		}	

		CATS_LoadResourcePlttWorkArc( pfd, FADE_MAIN_OBJ, csp, crp, ARC_WIFI2DCHAR, ncl, 0, 1, NNS_G2D_VRAM_TYPE_2DMAIN, eID_OAM_HERO_ICON );
		CATS_LoadResourceCellArc( csp, crp, ARC_WIFI2DCHAR, nce, 0, eID_OAM_HERO_ICON );
		CATS_LoadResourceCellAnmArc( csp, crp, ARC_WIFI2DCHAR, nca, 0, eID_OAM_HERO_ICON );
		CATS_LoadResourceCharArcModeAdjustAreaCont(	csp, crp, ARC_WIFI2DCHAR, ncg, 0, NNS_G2D_VRAM_TYPE_2DMAIN, eID_OAM_HERO_ICON );		
	}
}

static void PalGrayScale(u16 *pal, u16 pal_start, u16 pal_size)
{
	int i, r, g, b;
	u32 c;
	
	for(i = 0; i < pal_start; i++ ){
		pal++;
	}
	for( ; i < pal_start + pal_size; i++){
		r = ((*pal) & 0x1f);
		g = (((*pal) >> 5) & 0x1f);
		b = (((*pal) >> 10) & 0x1f);

		c = RGBtoY(r,g,b);

		*pal = (u16)((c<<10)|(c<<5)|c);
		pal++;
	}
}

static void BR_PalGray( BR_WORK* wk )
{
	FREC_WORK*		 swk = wk->sub_work;
	
	GF_BGL_INI*		 bgl = wk->sys.bgl;
	CATS_SYS_PTR	 csp = wk->sys.csp;
	CATS_RES_PTR	 crp = wk->sys.crp;
	PALETTE_FADE_PTR pfd = wk->sys.pfd;
	
	int i;
	int rec_state[ 5 ];
	
	{
		EVENTWORK* ev = SaveData_GetEventWork( wk->save );
		rec_state[ 0 ] = SysWork_MemoryPrintTower( ev );
		rec_state[ 1 ] = SysWork_MemoryPrintFactory( ev );
		rec_state[ 2 ] = SysWork_MemoryPrintRoulette( ev );
		rec_state[ 3 ] = SysWork_MemoryPrintCastle( ev );
		rec_state[ 4 ] = SysWork_MemoryPrintStage( ev );
	}

	for ( i = 0; i < FREC_BGOBJ_MAX; i++ ){		
		if ( rec_state[ i ] != MEMORY_PRINT_NONE ){			
			int bg_no[] = { 0,1,4,3,2 };			
			if ( rec_state[ i ] == MEMORY_PRINT_PUT_OK_1ST ){		
				PALETTE_FADE_PTR pfd = wk->sys.pfd;
				u16* pal;
				pal = PaletteWorkTransWorkGet( pfd, FADE_MAIN_OBJ );
				PalGrayScale( pal, ( eOMA_PAL_BG_OAM + bg_no[ i ] ) * 16, 16 );
			}
		}
	}
	PaletteTrans_AutoSet( pfd, TRUE );
}

//--------------------------------------------------------------
/**
 * @brief	OAM 登録
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BR_frec_ResAdd( BR_WORK* wk )
{
	int i;
	int zero_pos = 0;
	int num_para[ FREC_BP_MAX + 1 ];
	int rec_state[ 5 ];
	
	FREC_WORK*		 swk = wk->sub_work;
	
	GF_BGL_INI*		 bgl = wk->sys.bgl;
	CATS_SYS_PTR	 csp = wk->sys.csp;
	CATS_RES_PTR	 crp = wk->sys.crp;
	PALETTE_FADE_PTR pfd = wk->sys.pfd;
	
	TCATS_OBJECT_ADD_PARAM_S coap;
	
	
	///< 人物系
	coap.x		= 0;
	coap.y		= 256;
	coap.z		= 0;		
	coap.anm	= 0;
	coap.pri	= eFREC_PRI_HERO;
	coap.pal	= 0;
	coap.d_area = CATS_D_AREA_MAIN;
	coap.bg_pri = 0;
	coap.vram_trans = 0;
	
	coap.id[4] = CLACT_U_HEADER_DATA_NONE;
	coap.id[5] = CLACT_U_HEADER_DATA_NONE;
	
	///< 主人公
	coap.id[0] = eID_OAM_HERO_ICON;
	coap.id[1] = eID_OAM_HERO_ICON;
	coap.id[2] = eID_OAM_HERO_ICON;
	coap.id[3] = eID_OAM_HERO_ICON;

	for ( i = 0; i < FREC_BGOBJ_MAX; i++ ){
		swk->cap_hero[ i ] = CATS_ObjectAdd_S( wk->sys.csp, wk->sys.crp, &coap );
		CATS_ObjectAnimeSeqSetCap( swk->cap_hero[ i ], eHERO_BOTTOM );
		CATS_ObjectUpdateCap( swk->cap_hero[ i ] );
		CATS_ObjectScaleSetCap( swk->cap_hero[ i ], swk->scale[ i ], swk->scale[ i ] );
	}
	
	///< 5人

	for ( i = 0; i < FREC_BGOBJ_MAX; i++ ){
		coap.id[0] = eID_OAM_HUM_ICON + i;
		coap.id[1] = eID_OAM_HUM_ICON + i;
		if ( i == 3 ){
			coap.id[2] = eID_OAM_HUM_ICON + 1;
			coap.id[3] = eID_OAM_HUM_ICON + 1;
		}
		else {
			coap.id[2] = eID_OAM_HUM_ICON;
			coap.id[3] = eID_OAM_HUM_ICON;
		}
		swk->cap_icon[ i ] = CATS_ObjectAdd_S( wk->sys.csp, wk->sys.crp, &coap );
		
		if ( i == 3 ){
			CATS_ObjectAnimeSeqSetCap( swk->cap_icon[ i ], 0 );
		}
		else {
			CATS_ObjectAnimeSeqSetCap( swk->cap_icon[ i ], eHERO_BOTTOM );
		}
		CATS_ObjectUpdateCap( swk->cap_icon[ i ] );
		CATS_ObjectScaleSetCap( swk->cap_icon[ i ], swk->scale[ i ], swk->scale[ i ] );
		CATS_ObjectPaletteSetCap( swk->cap_icon[ i ], eOMA_PAL_HUM + i );
	}
	
	///< BG
	coap.id[0] = eID_OAM_BG_PHOTO;
	coap.id[1] = eID_OAM_BG_PHOTO;
	coap.id[2] = eID_OAM_BG_PHOTO;
	coap.id[3] = eID_OAM_BG_PHOTO;
	coap.id[4] = CLACT_U_HEADER_DATA_NONE;
	coap.id[5] = CLACT_U_HEADER_DATA_NONE;
	coap.pri   = eFREC_PRI_BG;	
	{
		EVENTWORK* ev = SaveData_GetEventWork( wk->save );
		rec_state[ 0 ] = SysWork_MemoryPrintTower( ev );
		rec_state[ 1 ] = SysWork_MemoryPrintFactory( ev );
		rec_state[ 2 ] = SysWork_MemoryPrintRoulette( ev );
		rec_state[ 3 ] = SysWork_MemoryPrintCastle( ev );
		rec_state[ 4 ] = SysWork_MemoryPrintStage( ev );
	}
	for ( i = 0; i < FREC_BGOBJ_MAX; i++ ){
		swk->cap_bg[ i ] = CATS_ObjectAdd_S( wk->sys.csp, wk->sys.crp, &coap );
		OS_Printf( " state = %d\n", rec_state[ i ] );
		
		if ( rec_state[ i ] == MEMORY_PRINT_NONE ){
			CATS_ObjectAnimeSeqSetCap( swk->cap_bg[ i ], 5 );					///< 未クリア
			CATS_ObjectPaletteSetCap( swk->cap_bg[ i ], eOMA_PAL_BG_OAM + 5 );
			CATS_ObjectEnableCap( swk->cap_icon[ i ], FALSE );
			CATS_ObjectEnableCap( swk->cap_hero[ i ], FALSE );
		}
		else {
			int bg_no[] = { 0,1,4,3,2 };
			CATS_ObjectAnimeSeqSetCap( swk->cap_bg[ i ], bg_no[ i ] );
			CATS_ObjectPaletteSetCap( swk->cap_bg[ i ], eOMA_PAL_BG_OAM + bg_no[ i ] );
		}
		CATS_ObjectUpdateCap( swk->cap_bg[ i ] );
		CATS_ObjectScaleSetCap( swk->cap_bg[ i ], swk->scale[ i ], swk->scale[ i ] );
	}
	
	
	
	///< NUM
	coap.id[0] = eID_OAM_BP_NUM;
	coap.id[1] = eID_OAM_BP_NUM;
	coap.id[2] = eID_OAM_BP_NUM;
	coap.id[3] = eID_OAM_BP_NUM;
	coap.id[4] = CLACT_U_HEADER_DATA_NONE;
	coap.id[5] = CLACT_U_HEADER_DATA_NONE;
	coap.d_area = CATS_D_AREA_SUB;
	coap.pri   = 0;

	{
		int para = 1000;
		int data = swk->b_point;
		BOOL fast = FALSE;
		for ( i = 0; i < FREC_BP_MAX; i++ ){
			num_para[ i ] = data / para;
			data %= para;
			para /= 10;
			if ( num_para[ i ] == 0 && fast == FALSE ){
				 zero_pos++;
			}
			else {
				fast = TRUE;
			}
		}
		num_para[ FREC_BP_MAX ] = 10; // anime = BP
	}
	
	for ( i = 0; i < FREC_BP_MAX + 1; i++ ){
		swk->cap_num[ i ] = CATS_ObjectAdd_S( wk->sys.csp, wk->sys.crp, &coap );
		if ( num_para[ i ] == 0 ){
			if ( i < zero_pos ){
				CATS_ObjectEnableCap( swk->cap_num[ i ], FALSE );
			}
		}
		CATS_ObjectAnimeSeqSetCap( swk->cap_num[ i ], num_para[ i ] );
		BR_CATS_ObjectPosSetCap( swk->cap_num[ i ], FREC_NUM_PX + ( 16*i ), FREC_NUM_PY );
		CATS_ObjectUpdateCap( swk->cap_num[ i ] );
	}
	CATS_ObjectEnableCap( swk->cap_num[ FREC_BP_MAX - 1 ], TRUE );		///< １桁目
	CATS_ObjectEnableCap( swk->cap_num[ FREC_BP_MAX - 0 ], TRUE );		///< BP は必ず表示するので
	
	for ( i = 0; i < FREC_BGOBJ_MAX; i++ ){
		CATS_ObjectAffineSetCap( swk->cap_hero[ i ], CLACT_AFFINE_DOUBLE );
		CATS_ObjectAffineSetCap( swk->cap_icon[ i ], CLACT_AFFINE_DOUBLE );
		CATS_ObjectAffineSetCap( swk->cap_bg[ i ],   CLACT_AFFINE_DOUBLE );
	}
	
}


//--------------------------------------------------------------
/**
 * @brief	削除
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BR_frec_ResDel( BR_WORK* wk )
{
	int i;
	
	FREC_WORK* swk = wk->sub_work;
	
	///< 人物OBJ削除
	for ( i = 0; i < FREC_BGOBJ_MAX; i++ ){		
		CATS_FreeResourceChar( wk->sys.crp, eID_OAM_HUM_ICON + i );	
		CATS_FreeResourcePltt( wk->sys.crp, eID_OAM_HUM_ICON + i );
		CATS_ActorPointerDelete_S( swk->cap_icon[ i ] );
	}
	CATS_FreeResourceCell( wk->sys.crp, eID_OAM_HUM_ICON );
	CATS_FreeResourceCellAnm( wk->sys.crp, eID_OAM_HUM_ICON );	
	CATS_FreeResourceCell( wk->sys.crp, eID_OAM_HUM_ICON + 1 );
	CATS_FreeResourceCellAnm( wk->sys.crp, eID_OAM_HUM_ICON + 1 );	
	
	///< 主人公削除
	for ( i = 0; i < FREC_BGOBJ_MAX; i++ ){
		CATS_ActorPointerDelete_S( swk->cap_hero[ i ] );
	}
	CATS_FreeResourceChar( wk->sys.crp, eID_OAM_HERO_ICON );
	CATS_FreeResourcePltt( wk->sys.crp, eID_OAM_HERO_ICON );
	CATS_FreeResourceCell( wk->sys.crp, eID_OAM_HERO_ICON );
	CATS_FreeResourceCellAnm( wk->sys.crp, eID_OAM_HERO_ICON );
	
	///< 背景削除
	for ( i = 0; i < FREC_BGOBJ_MAX; i++ ){
		CATS_ActorPointerDelete_S( swk->cap_bg[ i ] );
	}
	CATS_FreeResourceChar( wk->sys.crp, eID_OAM_BG_PHOTO );
	CATS_FreeResourcePltt( wk->sys.crp, eID_OAM_BG_PHOTO );
	CATS_FreeResourceCell( wk->sys.crp, eID_OAM_BG_PHOTO );
	CATS_FreeResourceCellAnm( wk->sys.crp, eID_OAM_BG_PHOTO );
	
	for ( i = 0; i < FREC_BP_MAX + 1; i++ ){
		CATS_ActorPointerDelete_S( swk->cap_num[ i ] );
	}
	CATS_FreeResourceChar( wk->sys.crp, eID_OAM_BP_NUM );
	CATS_FreeResourcePltt( wk->sys.crp, eID_OAM_BP_NUM );
	CATS_FreeResourceCell( wk->sys.crp, eID_OAM_BP_NUM );
	CATS_FreeResourceCellAnm( wk->sys.crp, eID_OAM_BP_NUM );
}

//--------------------------------------------------------------
/**
 * @brief	座標設定
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static BOOL BR_frec_PhotoPosRollInSet( BR_WORK* wk )
{
	FREC_WORK* swk = wk->sub_work;
	
	int i;
	s16 x,y;
	fx32 ox, oy;
	
	for ( i = 0; i < FREC_BGOBJ_MAX; i++ ){
		
		swk->rad[ i ] += 20;
		swk->rad[ i ] %= 360;
	}

//	if ( swk->color != 0 ){	 swk->color -= 1; }
//	ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, 0xFFF0, swk->color, wk->sys.logo_color );	///< main	oam
	Plate_AlphaFade( &swk->eva, &swk->evb, eFADE_MODE_IN, ePLANE_ALL );
				
	for ( i = 0; i < FREC_BGOBJ_MAX; i++ ){
		
		if ( swk->rad[ 0 ] != 0 ){
			 swk->scale[ i ] += 0.05f;
		}
		else {
			CATS_ObjectAffineSetCap( swk->cap_hero[ i ], CLACT_AFFINE_NONE );
			CATS_ObjectAffineSetCap( swk->cap_icon[ i ], CLACT_AFFINE_NONE );
			CATS_ObjectAffineSetCap( swk->cap_bg[ i ],   CLACT_AFFINE_NONE );
			swk->scale[ i ] = 1.0f;
		}

		ox = ( 128 << FX32_SHIFT ) + Sin360R( swk->rad[ i ] ) * swk->range[ i ];
		oy = ( 106 << FX32_SHIFT ) - Cos360R( swk->rad[ i ] ) * swk->range[ i ];

		CATS_ObjectPosSetCapFx32( swk->cap_bg[ i ], ox, oy );		
		CATS_ObjectPosGetCap( swk->cap_bg[ i ], &x, &y );		
		CATS_ObjectPosSetCap( swk->cap_hero[ i ], x - 32, y - 2 );
		CATS_ObjectPosSetCap( swk->cap_icon[ i ], x + 16, y - 2 );
				
		CATS_ObjectScaleSetCap( swk->cap_bg[ i ],   swk->scale[ i ], swk->scale[ i ] );
		CATS_ObjectScaleSetCap( swk->cap_hero[ i ], swk->scale[ i ], swk->scale[ i ] );
		CATS_ObjectScaleSetCap( swk->cap_icon[ i ], swk->scale[ i ], swk->scale[ i ] );
	}
	
	if ( swk->rad[ 0 ] == 0 ){ return FALSE; }

	return TRUE;
}

static BOOL BR_frec_PhotoPosRollOutSet( BR_WORK* wk )
{
	FREC_WORK* swk = wk->sub_work;
	
	int i;
	s16 x,y;
	fx32 ox, oy;
	
	for ( i = 0; i < FREC_BGOBJ_MAX; i++ ){
		
		swk->rad[ i ] += 20;
		swk->rad[ i ] %= 360;
	}
	
	if ( swk->rad[ 0 ] == 20 ){			
		for ( i = 0; i < FREC_BGOBJ_MAX; i++ ){
			CATS_ObjectAffineSetCap( swk->cap_hero[ i ], CLACT_AFFINE_DOUBLE );
			CATS_ObjectAffineSetCap( swk->cap_icon[ i ], CLACT_AFFINE_DOUBLE );
			CATS_ObjectAffineSetCap( swk->cap_bg[ i ],   CLACT_AFFINE_DOUBLE );
		}
	}

//	if ( swk->color != 16 ){ swk->color += 1; }
//	ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, 0xFFF0, swk->color, wk->sys.logo_color );	///< main	oam
	Plate_AlphaFade( &swk->eva, &swk->evb, eFADE_MODE_OUT, ePLANE_ALL );
				
	for ( i = 0; i < FREC_BGOBJ_MAX; i++ ){
		
		if ( swk->rad[ 0 ] != 0 ){
			 swk->scale[ i ] -= 0.05f;
		}
		else {
			CATS_ObjectEnableCap( swk->cap_bg[ i ],   CATS_ENABLE_FALSE );
			CATS_ObjectEnableCap( swk->cap_hero[ i ], CATS_ENABLE_FALSE );
			CATS_ObjectEnableCap( swk->cap_icon[ i ], CATS_ENABLE_FALSE );
		}
		
		ox = ( 128 << FX32_SHIFT ) - Sin360R( swk->rad[ i ] ) * swk->range[ i ];
		oy = ( 106 << FX32_SHIFT ) - Cos360R( swk->rad[ i ] ) * swk->range[ i ];

		CATS_ObjectPosSetCapFx32( swk->cap_bg[ i ], ox, oy );		
		CATS_ObjectPosGetCap( swk->cap_bg[ i ], &x, &y );		
		CATS_ObjectPosSetCap( swk->cap_hero[ i ], x - 32, y - 2 );
		CATS_ObjectPosSetCap( swk->cap_icon[ i ], x + 16, y - 2 );
				
		CATS_ObjectScaleSetCap( swk->cap_bg[ i ],   swk->scale[ i ], swk->scale[ i ] );
		CATS_ObjectScaleSetCap( swk->cap_hero[ i ], swk->scale[ i ], swk->scale[ i ] );
		CATS_ObjectScaleSetCap( swk->cap_icon[ i ], swk->scale[ i ], swk->scale[ i ] );
	}
	
	if ( swk->rad[ 0 ] == 0 ){ return FALSE; }

	return TRUE;
}

static void BR_frec_PhotoPosSet( BR_WORK* wk )
{
	FREC_WORK* swk = wk->sub_work;
	
	int i;
	int rad = 0;
	s16 x,y;
	fx32 ox, oy;
	
	static s16 p_tbl[][ 2 ] = {
		{ 128,  32+10 },
		{ 189,  77+10 },
		{ 172, 124+10 },
		{  83, 124+10 },
		{  66,  77+10 },
	};
		
	for ( i = 0; i < FREC_BGOBJ_MAX; i++ ){
		
		ox = p_tbl[ i ][ 0 ] << FX32_SHIFT;
		oy = p_tbl[ i ][ 1 ] << FX32_SHIFT;
		
		CATS_ObjectPosSetCapFx32( swk->cap_bg[ i ], ox, oy );
		
		CATS_ObjectPosGetCap( swk->cap_bg[ i ], &x, &y );
		
		CATS_ObjectPosSetCap( swk->cap_hero[ i ], x - 32, y - 2 );
		CATS_ObjectPosSetCap( swk->cap_icon[ i ], x + 16, y - 2 );
	}
}


//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	wk	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL BR_frec_Init( BR_WORK* wk )
{
	FREC_WORK* swk;
	
	if ( wk->sub_seq == 0 ){		
		swk = sys_AllocMemory( HEAPID_BR, sizeof( FREC_WORK ) );
		MI_CpuFill8( swk, 0, sizeof( FREC_WORK ) );
		wk->sub_work = swk;
		swk->color = 0;
		
		{
			int i;
			
			for ( i = 0; i < FREC_BGOBJ_MAX; i++ ){
				
				if ( i == 2 || i == 3 ){
					swk->range[ i ] = 52;
				}
				else {
					swk->range[ i ] = 64;
				}
				swk->rad[ i ] =  i * ( 360 / FREC_BGOBJ_MAX );
				swk->scale[ i ] = 0.2f;
			}
		}
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
		
		Plate_AlphaInit_Default( &swk->eva, &swk->evb, ePLANE_ALL );
		Plate_AlphaSetPlane( &swk->eva, &swk->evb, 0, 0, GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG1, ePLANE_ALL );
		Snd_SePlay( eBR_SND_FRONTIER_IN );
		wk->sub_seq++;
	}
	else {		
		swk = wk->sub_work;
	}
	
	switch ( wk->sub_seq ){
	case 1:
		if ( BR_PaletteFade( &swk->color, eFADE_MODE_OUT ) ){
			///< バトルポイントゲット
			BTLTOWER_SCOREWORK* bs = SaveData_GetTowerScoreData( wk->save );
			swk->b_point = TowerScoreData_SetBattlePoint( bs, 0, BTWR_DATA_get );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, 			 swk->color, wk->sys.logo_color );	///< main	oam
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	 LINE_OTHER_SUB_PALETTE, swk->color, wk->sys.logo_color );	///< sub	oam
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,	 LOGO_PALETTE,			 swk->color, wk->sys.logo_color );	///< sub	bg
		break;
	
	case 2:
		CATS_SystemActiveSet( wk, TRUE );
		NormalTag_DeleteAllOp( wk );						///< tag delete
		CATS_SystemActiveSet( wk, FALSE );
		
		BR_ChangeDisplayVIntr( wk, DISP_3D_TO_MAIN );				///< main = top
//		BR_ChangeDisplay( DISP_3D_TO_MAIN );
		Cursor_Visible( wk->cur_wk, FALSE );
		
		PrioritySet_Common();
		GF_BGL_PrioritySet( GF_BGL_FRAME2_S, 2 );
		
		BR_frec_ResLoad( wk );
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	 LINE_OTHER_SUB_PALETTE | 0x0010, swk->color, wk->sys.logo_color );	///< sub	oam
		wk->sub_seq++;
		break;
		
	case 3:
		if ( BR_IsChangeDisplay( wk ) == FALSE ){ break; }
		CATS_SystemActiveSet( wk, TRUE );
		FontButton_Create( wk );
		BR_frec_ResAdd( wk );
		BR_PalGray( wk );
		CATS_SystemActiveSet( wk, FALSE );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
		wk->sub_seq++;
		break;
	
	case 4:
		{
			BOOL bActive = BR_frec_PhotoPosRollInSet( wk );		
			Plate_AlphaFade( &swk->eva, &swk->evb, eFADE_MODE_IN, ePLANE_ALL );
			BR_PaletteFade( &swk->color, eFADE_MODE_IN );
			if ( bActive == FALSE ){
				wk->sub_seq++;
				BR_frec_PhotoPosSet( wk );				
			}
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE | 0x0010, swk->color, wk->sys.logo_color );
		}
		break;
		
	default:		
	//	Slider_Init( &wk->sys.slider );
		BR_Main_SeqChange( wk, eFREC_MAIN );		
		break;
	}	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL BR_frec_Main( BR_WORK* wk )
{
	FREC_WORK* swk = wk->sub_work;
	
	{
		u32 x, y;
		BOOL bHit = FALSE;
		
		bHit = GF_TP_GetPointTrg( &x, &y );
		
		if ( bHit ){
			if ( x > 10 * 8 && x < 22 * 8
			&&	 y > 19 * 8 && y < 22 * 8 ){
				Snd_SePlay( eBR_SND_TOUCH );
				BR_Main_SeqChange( wk, eFREC_END );
			}
		}	
	}
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL BR_frec_End( BR_WORK* wk )
{
	FREC_WORK* swk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		
		if ( swk->color == 0 ){
			Snd_SePlay( eBR_SND_FRONTIER_OUT );
		}
		if ( swk->color != 16 ){
			swk->color += 2;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE | 0x0010, swk->color, wk->sys.logo_color );
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,   0x4018, swk->color, wk->sys.logo_color );
		
		if ( BR_frec_PhotoPosRollOutSet( wk ) == FALSE ){
			swk->color = 0;
			wk->sub_seq++;
		}
		break;
	
	case 1:
		wk->sub_seq++;
	
	case 2:
		///< 削除とか
		CATS_SystemActiveSet( wk, TRUE );
		BR_frec_ResDel( wk );
		FontButton_Delete( wk );		
		CATS_SystemActiveSet( wk, FALSE );	
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );	
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
		
		swk->color = 16;
		swk->color = 0;
		wk->sub_seq++;
		break;
	
	case 3:	
		BR_ChangeDisplay( DISP_3D_TO_SUB );
		CATS_SystemActiveSet( wk, TRUE );
		NormalTag_RecoverAllOp( wk );
		CATS_SystemActiveSet( wk, FALSE );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
		wk->sub_seq++;
		break;
		
	case 4:
		if ( swk->color != 0 ){
			swk->color -= 2;
		}
		else {
			wk->sub_seq++;
			BR_PaletteFadeIn_Init( &swk->color );
			ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, 				swk->color, wk->sys.logo_color );	///< main	oam
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE | 0x0010, swk->color, wk->sys.logo_color );
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG, 	 LOGO_PALETTE,				swk->color, wk->sys.logo_color );	///< sub	bg
		}
		break;
	
	default:
		if ( BR_PaletteFade( &swk->color, eFADE_MODE_IN ) ){
			G2_SetBlendAlpha(  GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_OBJ,	7, 8 );		
			G2S_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_OBJ,  7, 8 );			
			sys_FreeMemoryEz( swk );
			PaletteFadeClear( wk );
			BR_PaletteFadeIn_Init( &swk->color );
			ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, 16, wk->sys.logo_color );	///< main	oam
			BR_Main_ProcSeqChange( wk, eMAIN_Input, TRUE, wk->proc_seq );
			BR_Main_SeqChange( wk, eINPUT_MENU_IN );
		}
		else {
//			ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, 				swk->color, wk->sys.logo_color );	///< main	oam
//			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,  LINE_OTHER_SUB_PALETTE | 0x0010,	swk->color, wk->sys.logo_color );	///< sub	oam
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, swk->color, wk->sys.logo_color );	///< main	oam
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG, 	 LOGO_PALETTE,				swk->color, wk->sys.logo_color );	///< sub	bg
		}
		break;
	}
					   
	return FALSE;
}

