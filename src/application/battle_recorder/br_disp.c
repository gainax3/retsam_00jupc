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

#include "br_private.h"

//==============================================================
// Prototype
//==============================================================

///< TAG の プライオリティ
static const u8 TAG_OamPri[][ TAG_MAX ] = {
	
	{  5, 10, 15, 20, 25, },		///< した
	{ 25, 20, 15, 10,  5, },		///< うえ
};

//--------------------------------------------------------------
/**
 * @brief	BG展開
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_disp_BG_Load( BR_WORK* wk )
{	
	ARCHANDLE*		 hdl = wk->sys.p_handle;
	GF_BGL_INI*		 bgl = wk->sys.bgl;
	CATS_SYS_PTR	 csp = wk->sys.csp;
	CATS_RES_PTR	 crp = wk->sys.crp;
	PALETTE_FADE_PTR pfd = wk->sys.pfd;
	
	///< キャラ転送
	if ( wk->mode == BR_MODE_BROWSE ){	
		ArcUtil_HDL_BgCharSet( hdl, NARC_batt_rec_gra_batt_rec_browse_bg_NCGR, bgl, GF_BGL_FRAME1_M, 0, 0, 0, HEAPID_BR );
		ArcUtil_HDL_BgCharSet( hdl, NARC_batt_rec_gra_batt_rec_browse_bg_NCGR, bgl, GF_BGL_FRAME1_S, 0, 0, 0, HEAPID_BR );	
		ArcUtil_HDL_ScrnSet( hdl, NARC_batt_rec_gra_batt_rec_browse_bg0u_NSCR, bgl, GF_BGL_FRAME1_S, 0, 0, 0, HEAPID_BR );
		PaletteWorkSet_Arc( pfd, ARC_BR_GRA, BR_ColorPaletteID_Get( wk, GET_PAL_BG ), HEAPID_BR, FADE_MAIN_BG, 0x20 * BG_PAL_BASE_END, eBG_PAL_BASE_0 );	
		PaletteWorkSet_Arc( pfd, ARC_BR_GRA, BR_ColorPaletteID_Get( wk, GET_PAL_BG ), HEAPID_BR, FADE_SUB_BG,	 0x20 * BG_PAL_BASE_END, eBG_PAL_BASE_0 );	
		PaletteWorkSet_Arc( pfd, ARC_BR_GRA, BR_ColorPaletteID_Get( wk, GET_PAL_FONT ), HEAPID_BR, FADE_MAIN_BG,	 0x20, eBG_PAL_FONT*16 );	
		PaletteWorkSet_Arc( pfd, ARC_BR_GRA, BR_ColorPaletteID_Get( wk, GET_PAL_FONT ), HEAPID_BR, FADE_SUB_BG,	 0x20, eBG_PAL_FONT*16 );
	}
	else {
		ArcUtil_HDL_BgCharSet( hdl, NARC_batt_rec_gra_batt_rec_gds_bg_NCGR, bgl, GF_BGL_FRAME1_M, 0, 0, 0, HEAPID_BR );
		ArcUtil_HDL_BgCharSet( hdl, NARC_batt_rec_gra_batt_rec_gds_bg_NCGR, bgl, GF_BGL_FRAME1_S, 0, 0, 0, HEAPID_BR );
		ArcUtil_HDL_ScrnSet( hdl, NARC_batt_rec_gra_batt_rec_gds_bg0u_NSCR, bgl, GF_BGL_FRAME1_S, 0, 0, 0, HEAPID_BR );
		PaletteWorkSet_Arc( pfd, ARC_BR_GRA, NARC_batt_rec_gra_batt_rec_gds_bg_NCLR, HEAPID_BR, FADE_MAIN_BG, 0x20 * BG_PAL_BASE_END, eBG_PAL_BASE_0 );	
		PaletteWorkSet_Arc( pfd, ARC_BR_GRA, NARC_batt_rec_gra_batt_rec_gds_bg_NCLR, HEAPID_BR, FADE_SUB_BG,  0x20 * BG_PAL_BASE_END, eBG_PAL_BASE_0 );
		PaletteWorkSet_Arc( pfd, ARC_BR_GRA, NARC_batt_rec_gra_batt_rec_gds_font_NCLR, HEAPID_BR, FADE_MAIN_BG,	 0x20, eBG_PAL_FONT*16 );	
		PaletteWorkSet_Arc( pfd, ARC_BR_GRA, NARC_batt_rec_gra_batt_rec_gds_font_NCLR, HEAPID_BR, FADE_SUB_BG,	 0x20, eBG_PAL_FONT*16 );		
	}
	ArcUtil_HDL_ScrnSet( hdl, NARC_batt_rec_gra_batt_rec_browse_bg0d_NSCR, bgl, GF_BGL_FRAME1_S, 0, 0, 0, HEAPID_BR );
	ArcUtil_HDL_ScrnSet( hdl, NARC_batt_rec_gra_batt_rec_browse_bg0d_NSCR, bgl, GF_BGL_FRAME1_M, 0, 0, 0, HEAPID_BR );
	
	ColorConceChangePfd( pfd, FADE_SUB_BG, LOGO_PALETTE, 16, wk->sys.logo_color );
}

void BR_disp_BG_InfoWindowLoad( BR_WORK* wk, int frame, BOOL yesno )
{
	ARCHANDLE*		 hdl = wk->sys.p_handle;
	GF_BGL_INI*		 bgl = wk->sys.bgl;
	CATS_SYS_PTR	 csp = wk->sys.csp;
	CATS_RES_PTR	 crp = wk->sys.crp;
	PALETTE_FADE_PTR pfd = wk->sys.pfd;
	int nscr = NARC_batt_rec_gra_batt_rec_win2_NSCR;
	
	if ( yesno ){
		nscr = NARC_batt_rec_gra_batt_rec_win_NSCR;
	}
	
	ArcUtil_HDL_BgCharSet( hdl, NARC_batt_rec_gra_batt_rec_win_NCGR, bgl, frame, 0, 0, 0, HEAPID_BR );	
	ArcUtil_HDL_ScrnSet( hdl, nscr, bgl, frame, 0, 0, 0, HEAPID_BR );
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 * @param	frame	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_disp_BG_InfoWindowLoad_Single( BR_WORK* wk, int frame )
{
	ARCHANDLE*		 hdl = wk->sys.p_handle;
	GF_BGL_INI*		 bgl = wk->sys.bgl;
	CATS_SYS_PTR	 csp = wk->sys.csp;
	CATS_RES_PTR	 crp = wk->sys.crp;
	PALETTE_FADE_PTR pfd = wk->sys.pfd;
	int nscr = NARC_batt_rec_gra_batt_rec_win3_NSCR;
	ArcUtil_HDL_BgCharSet( hdl, NARC_batt_rec_gra_batt_rec_win_NCGR, bgl, frame, 0, 0, 0, HEAPID_BR );	
	ArcUtil_HDL_ScrnSet( hdl, nscr, bgl, frame, 0, 0, 0, HEAPID_BR );
}

//--------------------------------------------------------------
/**
 * @brief	サイドバーキャラ読み込み
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_SideBar_Load( BR_WORK* wk )
{	
	ARCHANDLE*		 hdl = wk->sys.p_handle;
	GF_BGL_INI*		 bgl = wk->sys.bgl;
	CATS_SYS_PTR	 csp = wk->sys.csp;
	CATS_RES_PTR	 crp = wk->sys.crp;
	PALETTE_FADE_PTR pfd = wk->sys.pfd;

	if ( wk->mode == BR_MODE_BROWSE ){	
		CATS_LoadResourcePlttWorkArcH( pfd, FADE_MAIN_OBJ, csp, crp, hdl, BR_ColorPaletteID_Get( wk, GET_PAL_OBJ ), FALSE, OAM_TAG_PAL_MAX, NNS_G2D_VRAM_TYPE_2DMAIN, eID_OAM_SIDE_BAR_M );
		CATS_LoadResourcePlttWorkArcH( pfd, FADE_SUB_OBJ,  csp, crp, hdl, BR_ColorPaletteID_Get( wk, GET_PAL_OBJ ), FALSE, OAM_TAG_PAL_MAX, NNS_G2D_VRAM_TYPE_2DSUB,  eID_OAM_SIDE_BAR_S );
	}
	else {
		CATS_LoadResourcePlttWorkArcH( pfd, FADE_MAIN_OBJ, csp, crp, hdl, NARC_batt_rec_gra_batt_rec_gds_obj_NCLR, FALSE, OAM_TAG_PAL_MAX, NNS_G2D_VRAM_TYPE_2DMAIN, eID_OAM_SIDE_BAR_M );
		CATS_LoadResourcePlttWorkArcH( pfd, FADE_SUB_OBJ,  csp, crp, hdl, NARC_batt_rec_gra_batt_rec_gds_obj_NCLR, FALSE, OAM_TAG_PAL_MAX, NNS_G2D_VRAM_TYPE_2DSUB,  eID_OAM_SIDE_BAR_S );
	}

	CATS_LoadResourceCharArcH( csp, crp, hdl, NARC_batt_rec_gra_batt_rec_browse_line_NCGR, FALSE, NNS_G2D_VRAM_TYPE_2DMAIN, eID_OAM_SIDE_BAR_M );
	CATS_LoadResourceCharArcH( csp, crp, hdl, NARC_batt_rec_gra_batt_rec_browse_line_NCGR, FALSE, NNS_G2D_VRAM_TYPE_2DSUB,  eID_OAM_SIDE_BAR_S );
	
	CATS_LoadResourceCellArcH( csp, crp, hdl, NARC_batt_rec_gra_batt_rec_line_64k_NCER, FALSE, eID_OAM_SIDE_BAR_M );
	CATS_LoadResourceCellAnmArcH( csp, crp, hdl, NARC_batt_rec_gra_batt_rec_line_64k_NANR, FALSE, eID_OAM_SIDE_BAR_M );
	
	CATS_LoadResourceCellArcH( csp, crp, hdl, NARC_batt_rec_gra_batt_rec_line_64k_NCER, FALSE, eID_OAM_SIDE_BAR_S );
	CATS_LoadResourceCellAnmArcH( csp, crp, hdl, NARC_batt_rec_gra_batt_rec_line_64k_NANR, FALSE, eID_OAM_SIDE_BAR_S );
}

//--------------------------------------------------------------
/**
 * @brief	登録
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_SideBar_Add( BR_WORK* wk )
{
	TCATS_OBJECT_ADD_PARAM_S	coap;
	CATS_SYS_PTR				csp = wk->sys.csp;
	CATS_RES_PTR				crp = wk->sys.crp;
	PALETTE_FADE_PTR			pfd = wk->sys.pfd;
		
	coap.x			= 0;
	coap.y			= 0;
	coap.z			= 0;		
	coap.anm		= 0;
	coap.pri		= 0;
	coap.d_area		= CATS_D_AREA_MAIN;
	coap.bg_pri		= eBR_BG2_PRI;
	coap.vram_trans = 0;
	coap.pal		= 0;
	coap.id[0]		= eID_OAM_SIDE_BAR_M;
	coap.id[1]		= eID_OAM_SIDE_BAR_M;
	coap.id[2]		= eID_OAM_SIDE_BAR_M;
	coap.id[3]		= eID_OAM_SIDE_BAR_M;
	coap.id[4]		= CLACT_U_HEADER_DATA_NONE;
	coap.id[5]		= CLACT_U_HEADER_DATA_NONE;
	
	{
		int i, n;
		
		const s16 px_tbl[] = {
			 82, 178, 118, 42, 150, 210,
		};
		const f32 s_tbl[] = {
			1.4f, 1.8f, 2.0f, 1.6f, 1.4f, 1.6f,
		};
		const s8 t_ofs[] = {
			-1, -3, +2, -1, -3, +2,
		};
		
		for ( n = 0; n < SIDE_BAR_TOTAL; n++ ){
			
			for ( i = 0; i < SIDE_BAR_OAM; i++ ){
				
				if ( i == 0 ){
					coap.d_area = CATS_D_AREA_MAIN;
					coap.id[0]	= eID_OAM_SIDE_BAR_M;
					coap.id[1]	= eID_OAM_SIDE_BAR_M;
					coap.id[2]	= eID_OAM_SIDE_BAR_M;
					coap.id[3]	= eID_OAM_SIDE_BAR_M;
				}
				else {
					coap.d_area = CATS_D_AREA_SUB;
					coap.id[0]	= eID_OAM_SIDE_BAR_S;
					coap.id[1]	= eID_OAM_SIDE_BAR_S;
					coap.id[2]	= eID_OAM_SIDE_BAR_S;
					coap.id[3]	= eID_OAM_SIDE_BAR_S;
				}
				
				wk->bar[ n ].cap[ i ] = CATS_ObjectAdd_S( csp, crp, &coap );
				
				if ( i == 0 ){
					BR_CATS_ObjectPosSetCap( wk->bar[ n ].cap[ i ], px_tbl[ n ], 192 / 2 );
				}
				else {
					BR_CATS_ObjectPosSetCap( wk->bar[ n ].cap[ i ], px_tbl[ n ], 192 / 2 );
				}
				CATS_ObjectUpdateCap( wk->bar[ n ].cap[ i ] );
				CATS_ObjectAffineSetCap( wk->bar[ n ].cap[ i ], CLACT_AFFINE_DOUBLE );
				CATS_ObjectScaleSetCap( wk->bar[ n ].cap[ i ], s_tbl[ n ], 1.0f );
				
				wk->bar[ n ].wait	= 0;
				wk->bar[ n ].id		= n % 3;
				wk->bar[ n ].state	= 0;
				wk->bar[ n ].scale	= s_tbl[ n ];
				wk->bar[ n ].move_f = SIDE_BAR_MOVE_F + ( t_ofs[ n ] * 2 );
				if ( n < SIDE_BAR_L ){
					wk->bar[ n ].rad	= 0;
					wk->bar[ n ].pos_t	=   0 + ( 8 + t_ofs[ n ] );
					wk->bar[ n ].move	= -1;
				}
				else {
					wk->bar[ n ].rad	= 0;
					wk->bar[ n ].pos_t	= 255 - ( 8 + t_ofs[ n ] );
					wk->bar[ n ].move	= +1;
				}
			}
//			wk->bar[ n ].tcb = TCB_Add( BR_SideBar_TCB, &wk->bar[ n ], 0x1000 );
			wk->bar[ n ].tcb = VIntrTCB_Add( BR_SideBar_TCB, &wk->bar[ n ], eVINTR_TCB_PRI_BAR );
		}
		
		///< stack_s が 0 じゃない場合、サイドバーを縮めておく
		if ( wk->tag_man.stack_s != 0 ){
			BOOL bActive;
			do {
				bActive = FALSE;				
				for ( n = 0; n < SIDE_BAR_TOTAL; n++ ){
					if ( wk->bar[ n ].state == eSB_TCB_STATE_STAY ){ continue; }
					
					BR_SideBar_TCB( wk->bar[ n ].tcb, &wk->bar[ n ] );
					bActive = TRUE;
				}
			} while ( bActive );
		}
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
void BR_SideBar_Delete( BR_WORK* wk )
{
	int i, n;
	
	for ( n = 0; n < SIDE_BAR_TOTAL; n++ ){
			
		for ( i = 0; i < SIDE_BAR_OAM; i++ ){
			CATS_ActorPointerDelete_S( wk->bar[ n ].cap[ i ] );
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief	TCB Delete
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_SideBar_TCB_Delete( BR_WORK* wk )
{
	int n;
	
	for ( n = 0; n < SIDE_BAR_TOTAL; n++ ){
		
		TCB_Delete( wk->bar[ n ].tcb );
	}
}


//--------------------------------------------------------------
/**
 * @brief	画像の読み込み
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_Tag_Load( BR_WORK* wk )
{
	ARCHANDLE*		 hdl = wk->sys.p_handle;
	GF_BGL_INI*		 bgl = wk->sys.bgl;
	CATS_SYS_PTR	 csp = wk->sys.csp;
	CATS_RES_PTR	 crp = wk->sys.crp;
	PALETTE_FADE_PTR pfd = wk->sys.pfd;

	int ncgr;
	int ncer;
	int nanr;
	
	switch ( wk->mode ){
	case BR_MODE_BROWSE:
		///< ブラウズモード
		ncgr = NARC_batt_rec_gra_batt_rec_browse_tag_NCGR;
		ncer = NARC_batt_rec_gra_batt_rec_tag_64k_NCER;
		nanr = NARC_batt_rec_gra_batt_rec_tag_64k_NANR;
		break;
	
	case BR_MODE_GDS_BV:
//	case BR_MODE_GDS_BV_RANK:
		///< バトルビデオ
		ncgr = NARC_batt_rec_gra_batt_rec_tag_gds_video_NCGR;
		ncer = NARC_batt_rec_gra_batt_rec_tag_gds_video_NCER;
		nanr = NARC_batt_rec_gra_batt_rec_tag_gds_video_NANR;		
		break;
	
	case BR_MODE_GDS_DRESS:
	case BR_MODE_GDS_BOX:
		///< ボックスヨット・ドレス
		ncgr = NARC_batt_rec_gra_batt_rec_tag_gds_box_photo_NCGR;
		ncer = NARC_batt_rec_gra_batt_rec_tag_gds_box_photo_NCER;
		nanr = NARC_batt_rec_gra_batt_rec_tag_gds_box_photo_NANR;
		break;
		
	case BR_MODE_GDS_RANK:
		///< 週間ランキング
		ncgr = NARC_batt_rec_gra_batt_rec_tag_gds_weekly_rank_NCGR;
		ncer = NARC_batt_rec_gra_batt_rec_tag_gds_weekly_rank_NCER;
		nanr = NARC_batt_rec_gra_batt_rec_tag_gds_weekly_rank_NANR;
		break;
	
	case BR_MODE_GDS_BV_RANK:
		///< バトルビデオ・ランキング
		ncgr = NARC_batt_rec_gra_batt_rec_tag_gds_video_rank_NCGR;
		ncer = NARC_batt_rec_gra_batt_rec_tag_gds_video_rank_NCER;
		nanr = NARC_batt_rec_gra_batt_rec_tag_gds_video_rank_NANR;
		break;
	
	default:
		///< その他
		ncgr = NARC_batt_rec_gra_batt_rec_gds_tag_NCGR;
		ncer = NARC_batt_rec_gra_batt_rec_tag__gds_64k_NCER;
		nanr = NARC_batt_rec_gra_batt_rec_tag__gds_64k_NANR;
		break;
	}

	CATS_LoadResourceCharArcH( csp, crp, hdl, ncgr, FALSE, NNS_G2D_VRAM_TYPE_2DMAIN, eID_OAM_TAG_M );
	CATS_LoadResourceCharArcH( csp, crp, hdl, ncgr, FALSE, NNS_G2D_VRAM_TYPE_2DSUB,  eID_OAM_TAG_S );	
	CATS_LoadResourceCellArcH( csp, crp, hdl, ncer, FALSE, eID_OAM_TAG_M );	
	CATS_LoadResourceCellArcH( csp, crp, hdl, ncer, FALSE, eID_OAM_TAG_S );
	CATS_LoadResourceCellAnmArcH( csp, crp, hdl, nanr, FALSE, eID_OAM_TAG_M );
	CATS_LoadResourceCellAnmArcH( csp, crp, hdl, nanr, FALSE, eID_OAM_TAG_S );
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
void BR_Tag_Release( BR_WORK* wk )
{
	CATS_FreeResourceChar( wk->sys.crp, eID_OAM_TAG_M );
	CATS_FreeResourceChar( wk->sys.crp, eID_OAM_TAG_S );
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
void BR_Tag_PosReset_Sub( BR_WORK* wk )
{
	int i;
	TAG_MANAGER* tag_man = &wk->tag_man;
	
	for ( i = 0; i < TAG_MAX; i ++ ){
		BR_CATS_ObjectPosSetCap( tag_man->tag_s[ i ].cap, TAG_MAIN_POS + 8, TAG_INIT_POS( 4, wk->tag_man.item_max ) + TAG_VANISH_BOTTOM );
		FONTOAM_SetMat( wk->tag_man.tag_s[ i ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
	}	
	i = 0;	
	do {
		s16 x, y;
		
		if ( wk->tag_man.stack_s == 0 ){ break; }
		
		BR_CATS_ObjectPosGetCap( wk->tag_man.tag_s[ i ].cap, &x, &y );
		
		if ( wk->tag_man.stack_s - 1 == i ){
			x = x;
			y = TAG_INIT_POS_U( TAG_MAX - 1 );
			CATS_ObjectPaletteOffsetSetCap( wk->tag_man.tag_s[ i ].cap, dOAM_PAL_TAG_1 );	
		}
		else {
			x = x - ( 4 * ( wk->tag_man.stack_s - i ) );
			y = ( 16 * ( wk->tag_man.stack_s - i - 1 ) ) + TAG_INIT_POS_U( TAG_MAX - ( wk->tag_man.stack_s - i ) );
			CATS_ObjectPaletteOffsetSetCap( wk->tag_man.tag_s[ i ].cap, dOAM_PAL_TAG_2 );	
		}
		
		BR_CATS_ObjectPosSetCap( wk->tag_man.tag_s[ i ].cap, x, y );
		
		i++;
		
	} while ( i < wk->tag_man.stack_s );
}

//--------------------------------------------------------------
/**
 * @brief	タグ追加
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_Tag_Add( BR_WORK* wk )
{
	int i;
	TAG_MANAGER* tag_man = &wk->tag_man;

	TCATS_OBJECT_ADD_PARAM_S	coap;
	CATS_SYS_PTR				csp = wk->sys.csp;
	CATS_RES_PTR				crp = wk->sys.crp;
	PALETTE_FADE_PTR			pfd = wk->sys.pfd;
		
	coap.x			= TAG_MAIN_POS;
	coap.y			= TAG_INIT_POS( 4, wk->tag_man.item_max ) + TAG_VANISH_BOTTOM_INIT;

	if ( wk->tag_man.item_max == 2 ){
		coap.y		= TAG_INIT_POS_2( 4, wk->tag_man.item_max ) + TAG_VANISH_BOTTOM_INIT;
	}
	coap.z			= 0;		
	coap.anm		= 0;
	coap.pri		= 0;
	coap.d_area		= CATS_D_AREA_MAIN;
	coap.bg_pri		= eBR_BG1_PRI;
	coap.vram_trans = 0;
	coap.pal		= dOAM_PAL_TAG_1;
	coap.id[0]		= eID_OAM_TAG_M;
	coap.id[1]		= eID_OAM_SIDE_BAR_M;
	coap.id[2]		= eID_OAM_TAG_M;
	coap.id[3]		= eID_OAM_TAG_M;
	coap.id[4]		= CLACT_U_HEADER_DATA_NONE;
	coap.id[5]		= CLACT_U_HEADER_DATA_NONE;
		
	for ( i = 0; i < TAG_MAX; i ++ ){
			
		coap.d_area		= CATS_D_AREA_SUB;
		coap.id[0]		= eID_OAM_TAG_S;
		coap.id[1]		= eID_OAM_SIDE_BAR_S;
		coap.id[2]		= eID_OAM_TAG_S;
		coap.id[3]		= eID_OAM_TAG_S;
		coap.pri		= TAG_OamPri[ 1 ][ i ];
		tag_man->tag_s[ i ].cap = CATS_ObjectAdd_S( csp, crp, &coap );
		CATS_ObjectUpdateCap( tag_man->tag_s[ i ].cap );
		CATS_ObjectEnableCap( tag_man->tag_s[ i ].cap, TRUE );
		BR_CATS_ObjectPosSetCap( tag_man->tag_s[ i ].cap, coap.x + 8, coap.y );

		coap.d_area		= CATS_D_AREA_MAIN;
		coap.id[0]		= eID_OAM_TAG_M;
		coap.id[1]		= eID_OAM_SIDE_BAR_M;
		coap.id[2]		= eID_OAM_TAG_M;
		coap.id[3]		= eID_OAM_TAG_M;
		coap.pri		= TAG_OamPri[ 0 ][ i ];
		tag_man->tag_m[ i ].cap = CATS_ObjectAdd_S( csp, crp, &coap );		
		CATS_ObjectUpdateCap( tag_man->tag_m[ i ].cap );
		BR_CATS_ObjectPosSetCap( tag_man->tag_m[ i ].cap, coap.x, coap.y );
		
		CATS_ObjectEnableCap( tag_man->tag_m[ i ].cap, wk->menu_data[ i ].active );
	}

	/// 上画面の座標復帰処理
	i = 0;
	do {
		s16 x, y;
		
		if ( wk->tag_man.stack_s == 0 ){ break; }
		
		BR_CATS_ObjectPosGetCap( wk->tag_man.tag_s[ i ].cap, &x, &y );
		
		if ( wk->tag_man.stack_s - 1 == i ){
			x = x;
			y = TAG_INIT_POS_U( TAG_MAX - 1 );
			CATS_ObjectPaletteOffsetSetCap( wk->tag_man.tag_s[ i ].cap, dOAM_PAL_TAG_1 );	
		}
		else {
			x = x - ( 4 * ( wk->tag_man.stack_s - i ) );
			y = ( 16 * ( wk->tag_man.stack_s - i - 1 ) ) + TAG_INIT_POS_U( TAG_MAX - ( wk->tag_man.stack_s - i ) );
			CATS_ObjectPaletteOffsetSetCap( wk->tag_man.tag_s[ i ].cap, dOAM_PAL_TAG_2 );	
		}
		
		BR_CATS_ObjectPosSetCap( wk->tag_man.tag_s[ i ].cap, x, y );
		
		i++;
		
	} while ( i < wk->tag_man.stack_s );
	
	ColorConceChangePfd( pfd, FADE_MAIN_OBJ, TAG_PALETTE, 16, wk->sys.logo_color );
}


//--------------------------------------------------------------
/**
 * @brief	タグのanimeなど再設定
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_Tag_ReSet( BR_WORK* wk )
{
	int i;
		
	for ( i = 0; i < TAG_MAX; i ++ ){
		
		CATS_ObjectEnableCap( wk->tag_man.tag_m[ i ].cap, wk->menu_data[ i ].active );
		
		if ( wk->menu_data[ i ].active == FALSE ){ continue; }
		
		CATS_ObjectAnimeSeqSetCap( wk->tag_man.tag_m[ i ].cap, wk->menu_data[ i ].ID );
		BR_disp_FontOAM_MegWrite( wk, &wk->tag_man.tag_m[ i ], wk->menu_data[ i ].msgID, &wk->menu_data[ i ] );
	}
}


//--------------------------------------------------------------
/**
 * @brief	フォントOAM再設定
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_Tag_FontOamReset( BR_WORK* wk )
{
	int i;
	
	for ( i = 0; i < TAG_MAX; i++ ){
		FONTOAM_ParentActorSet( wk->tag_man.tag_m[ i ].font_obj, wk->tag_man.tag_m[ i ].cap->act );
		FONTOAM_ParentActorSet( wk->tag_man.tag_s[ i ].font_obj, wk->tag_man.tag_s[ i ].cap->act );
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
void BR_Tag_SubRecover( BR_WORK* wk ){

	int i;
	
	if ( wk->tag_man.stack_s == 0 ){ return; }
			
	for ( i = 0; i < wk->tag_man.stack_s; i++ ){
		CATS_ObjectAnimeSeqSetCap( wk->tag_man.tag_s[ i ].cap, wk->tag_man.menu_data_stack[ i ]->ID );
		BR_disp_FontOAM_MegWrite( wk, &wk->tag_man.tag_s[ i ], wk->tag_man.menu_data_stack[ i ]->msgID, wk->tag_man.menu_data_stack[ i ] );
		FONTOAM_SetMat( wk->tag_man.tag_s[ i ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
		FONTOAM_SetDrawFlag( wk->tag_man.tag_s[ i ].font_obj, TRUE );
		CATS_ObjectEnableCap( wk->tag_man.tag_s[ i ].cap, TRUE );
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
void BR_Tag_Delete( BR_WORK* wk )
{
	int i;
	TAG_MANAGER* tag_man = &wk->tag_man;
	
	for ( i = 0; i < TAG_MAX; i ++ ){
			
		CATS_ActorPointerDelete_S( tag_man->tag_s[ i ].cap );
		CATS_ActorPointerDelete_S( tag_man->tag_m[ i ].cap );
		
	//	TCB_Delete( wk->bar[ n ].tcb );
	//	TCB_Delete( wk->bar[ n ].tcb );
	}
}


void BR_Tag_Enable( BR_WORK* wk, BOOL flag )
{
	int i;
	
	for ( i = 0; i < TAG_MAX; i ++ ){
		
		if ( wk->tag_man.tag_s[ i ].active == FALSE ){
			FONTOAM_SetDrawFlag( wk->tag_man.tag_s[ i ].font_obj, FALSE );
			CATS_ObjectEnableCap( wk->tag_man.tag_s[ i ].cap, FALSE );
		}
		else {
			FONTOAM_SetDrawFlag( wk->tag_man.tag_s[ i ].font_obj, flag );
			CATS_ObjectEnableCap( wk->tag_man.tag_s[ i ].cap, flag );
		}
		if ( wk->tag_man.tag_m[ i ].active == FALSE ){
			FONTOAM_SetDrawFlag( wk->tag_man.tag_m[ i ].font_obj, FALSE );
			CATS_ObjectEnableCap( wk->tag_man.tag_m[ i ].cap, FALSE );
		}
		else {	
			FONTOAM_SetDrawFlag( wk->tag_man.tag_m[ i ].font_obj, flag );
			CATS_ObjectEnableCap( wk->tag_man.tag_m[ i ].cap, flag );
		}
	}
}


//--------------------------------------------------------------
/**
 * @brief	サイドバーTCB
 *
 * @param	tcb	
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_SideBar_TCB( TCB_PTR tcb, void* work )
{
	SIDE_BAR* wk = work;

	wk->vcount++;
	wk->vcount %= 2;
	if ( wk->vcount ){ return; }
	
	switch ( wk->state ){
	case eSB_TCB_STATE_WAIT:
		{
			if ( wk->wait == SIDE_BAR_WAIT ){
				wk->state++;
				wk->wait = 0;
			}
			else {
				wk->wait++;
			}
		}
		break;
		
	case eSB_TCB_STATE_MOVE_1:
		{	
			fx32 ox1, oy1;
			fx32 ox2, oy2;
			
			if ( wk->wait == SIDE_BAR_SIN_MOVE_F ){
				wk->state++;
				wk->wait = 0;
			}
			else {				
				CATS_ObjectPosGetCapFx32( wk->cap[ 0 ], &ox1, &oy1 );
				CATS_ObjectPosGetCapFx32( wk->cap[ 1 ], &ox2, &oy2 );
				
				ox1 = ox1 + ( ( Sin360R( FX_GET_ROTA_NUM( wk->rad ) ) * 1 ) * ( wk->move * -1 ) );
				ox2 = ox1;
				
				wk->rad += 32;
				wk->rad %= 360;
				
				CATS_ObjectPosSetCapFx32( wk->cap[ 0 ], ox1, oy1 );
				CATS_ObjectPosSetCapFx32( wk->cap[ 1 ], ox2, oy2 );
				
				wk->wait++;
			}	
		}
		break;
	
	case eSB_TCB_STATE_MOVE_2:
		{
			s16 x, y1, y2;
			int rad[] = {
				90, 135, 270, 45, 225, 0,
			};
			
			CATS_ObjectPosGetCap( wk->cap[ 0 ], &x, &y1 );
			CATS_ObjectPosGetCap( wk->cap[ 1 ], &x, &y2 );
			
			if ( wk->wait == 0 ){
				
				wk->val_x  = wk->pos_t - x;
				wk->val_x /= wk->move_f;
				wk->wait++;
			}
			else if ( wk->wait == wk->move_f + 1 ){
				
				x = wk->pos_t;
				
				wk->rad = rad[ wk->id ];
				wk->state++;
				wk->wait = 0;
			}
			else {
				x += wk->val_x;
				wk->wait++;
			}
			
			CATS_ObjectPosSetCap( wk->cap[ 0 ], x, y1 );
			CATS_ObjectPosSetCap( wk->cap[ 1 ], x, y2 );
		}	
		break;
	
	case eSB_TCB_STATE_STAY:
		{		
			fx32 ox1, oy1;
			fx32 ox2, oy2;
			
			if ( wk->wait == 0 ){
				CATS_ObjectPosGetCapFx32( wk->cap[ 0 ], &wk->ox, &oy1 );
				wk->wait++;
			}
						
			CATS_ObjectPosGetCapFx32( wk->cap[ 0 ], &ox1, &oy1 );
			CATS_ObjectPosGetCapFx32( wk->cap[ 1 ], &ox2, &oy2 );
			
			ox1 = wk->ox + ( ( Sin360R( FX_GET_ROTA_NUM( wk->rad ) ) * 3 ) * ( wk->move * -1 ) );
			ox2 = ox1;
			
			if ( wk->id % 2 ){
				wk->rad -= 4;
			}
			else {
				wk->rad += 4;
			}
			wk->rad %= 360;
			
			CATS_ObjectPosSetCapFx32( wk->cap[ 0 ], ox1, oy1 );
			CATS_ObjectPosSetCapFx32( wk->cap[ 1 ], ox2, oy2 );
		}
		break;
	}
	
	if ( wk->state >= eSB_TCB_STATE_MOVE_2 && wk->scale > 0.1f ){
		 wk->scale -= 0.10f;
		 
		 CATS_ObjectScaleSetCap( wk->cap[ 0 ], wk->scale, 1.0f );
		 CATS_ObjectScaleSetCap( wk->cap[ 1 ], wk->scale, 1.0f );
	}
		
}



//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	cap	
 * @param	x	
 * @param	y	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_CATS_ObjectPosSetCap(CATS_ACT_PTR cap, s16 x, s16 y)
{
	CATS_ObjectPosSetCap_SubSurface( cap, x, y, SUB_SURFACE_FX_Y_BR );
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	cap	
 * @param	x	
 * @param	y	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_CATS_ObjectPosGetCap(CATS_ACT_PTR cap, s16* x, s16* y)
{
	CATS_ObjectPosGetCap_SubSurface( cap, x, y, SUB_SURFACE_FX_Y_BR );
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	cap	
 * @param	x	
 * @param	y	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_CATS_ObjectPosSetCapFx32(CATS_ACT_PTR cap, fx32 x, fx32 y)
{
	CATS_ObjectPosSetCapFx32_SubSurface( cap, x, y, SUB_SURFACE_FX_Y_BR );
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	cap	
 * @param	x	
 * @param	y	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_CATS_ObjectPosGetCapFx32(CATS_ACT_PTR cap, fx32* x, fx32* y)
{
	CATS_ObjectPosGetCapFx32_SubSurface( cap, x, y, SUB_SURFACE_FX_Y_BR );
}



// =============================================================================
//
//
//	■font oam 関連
//
//
// =============================================================================
//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 * @param	mes_no	
 * @param	tag_no	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_disp_FontOam_Create( BR_WORK* wk )
{
	s16 ox, oy;
	FONTOAM_INIT		finit;
	STRBUF*				str;
	int					vram_size;
	MSGDATA_MANAGER*	man;
	GF_BGL_BMPWIN		bmpwin;	
	TAG*				tag[2];
	int					d_area[2] = { NNS_G2D_VRAM_TYPE_2DMAIN, NNS_G2D_VRAM_TYPE_2DSUB };
	int					pal_id[2] = { eID_OAM_SIDE_BAR_M, eID_OAM_SIDE_BAR_S };
	
	int i,j;
		
	for ( i = 0; i < TAG_MAX; i++ ){
		
		tag[ 0 ] = &wk->tag_man.tag_m[ i ]; 
		tag[ 1 ] = &wk->tag_man.tag_s[ i ]; 
		
		for ( j = 0; j < 2; j++ ){
						
			GF_BGL_BmpWinInit( &bmpwin );
			GF_BGL_BmpWinObjAdd( wk->sys.bgl, &bmpwin, BR_TAG_BMP_WIX_SX, BR_TAG_BMP_WIX_SY, 0, 0 );
			
			tag[ j ]->font_data = FONTOAM_OAMDATA_Make( &bmpwin, HEAPID_BR );

			vram_size = FONTOAM_NeedCharSize( &bmpwin, d_area[ j ], HEAPID_BR );
			CharVramAreaAlloc( vram_size, CHARM_CONT_AREACONT, d_area[ j ], &tag[ j ]->font_vram );
			
			CATS_ObjectPosGetCap( tag[ j ]->cap, &ox, &oy );

			finit.fontoam_sys	= wk->sys.fontoam_sys;
			finit.bmp			= &bmpwin;
			finit.clact_set		= CATS_GetClactSetPtr( wk->sys.crp );
			finit.pltt			= CATS_PlttProxy( wk->sys.crp, pal_id[ j ] );
			finit.parent		= tag[ j ]->cap->act;
			finit.char_ofs		= tag[ j ]->font_vram.alloc_ofs;
			finit.x				= ox + FONT_OAM_TAG_OX;
			finit.y				= oy + FONT_OAM_TAG_OY;
			finit.bg_pri		= eBR_BG1_PRI;
			finit.soft_pri		= TAG_OamPri[ j ][ i ] - 1;
			finit.draw_area		= d_area[ j ];
			finit.heap			= HEAPID_BR;
			
			tag[ j ]->font_obj = FONTOAM_OAMDATA_Init( &finit, tag[ j ]->font_data );
			FONTOAM_SetPaletteOffsetAddTransPlttNo( tag[ j ]->font_obj, dOAM_PAL_TAG_1 );	

			GF_BGL_BmpWinDel( &bmpwin );

			FONTOAM_SetDrawFlag( tag[ j ]->font_obj, FALSE );
		}
	}
}

void BR_disp_FontOAM_MegWrite( BR_WORK* wk, TAG* tag, int mes_id, const BR_MENU_DATA* dat )
{
	STRBUF*				str;
	MSGDATA_MANAGER*	man;
	GF_BGL_BMPWIN		bmpwin;	
	
	WORDSET*			wset;
	STRBUF*				tmp1;
	STRBUF*				tmp2;
		
	man = wk->sys.man;
	
	
	if ( mes_id == msg_10 ){
		///< あきの場合	～のきろく	
		if ( wk->br_gpp[ dat->ex_param1 ] == NULL ){
			str	= MSGMAN_AllocString( man, mes_id );
		}
		else {
			wset = BR_WORDSET_Create( HEAPID_BR );
			tmp1 = MSGMAN_AllocString( wk->sys.man, msg_09 );
			tmp2 = GDS_Profile_CreateNameString( wk->br_gpp[ dat->ex_param1 ], HEAPID_BR );
			BR_ErrorStrChange( wk, tmp2 );
			str	 = STRBUF_Create( 255, HEAPID_BR );
		
			WORDSET_RegisterWord( wset, 0, tmp2, 0, TRUE, PM_LANG );	
			WORDSET_ExpandStr( wset, str, tmp1 );
			
			STRBUF_Delete( tmp1 );
			STRBUF_Delete( tmp2 );
			WORDSET_Delete( wset );
		}
	}
	else if ( mes_id == msg_102 ){
		///< あきの場合	～のきろくをけす
		if ( wk->br_gpp[ dat->ex_param1 ] == NULL ){
			str	= MSGMAN_AllocString( man, mes_id );
		}
		else {
			wset = BR_WORDSET_Create( HEAPID_BR );
			tmp1 = MSGMAN_AllocString( wk->sys.man, msg_09 );
			tmp2 = GDS_Profile_CreateNameString( wk->br_gpp[ dat->ex_param1 ], HEAPID_BR );
			BR_ErrorStrChange( wk, tmp2 );
			str	 = STRBUF_Create( 255, HEAPID_BR );
		
			WORDSET_RegisterWord( wset, 0, tmp2, 0, TRUE, PM_LANG );
			WORDSET_ExpandStr( wset, str, tmp1 );
			
			STRBUF_Delete( tmp1 );
			STRBUF_Delete( tmp2 );
			WORDSET_Delete( wset );
		}
	}
	else {
		///< それ以外
		str	= MSGMAN_AllocString( man, mes_id );
	}
	
	GF_BGL_BmpWinInit( &bmpwin );
	GF_BGL_BmpWinObjAdd( wk->sys.bgl, &bmpwin, BR_TAG_BMP_WIX_SX, BR_TAG_BMP_WIX_SY, 0, 0 );
	GF_STR_PrintColor( &bmpwin, FONT_SYSTEM, str, 0, 0, MSG_NO_PUT, PRINT_COL_BLACK, NULL );
	
	FONTOAM_OAMDATA_ResetBmp( tag->font_obj, tag->font_data, &bmpwin, HEAPID_BR );
		
	STRBUF_Delete(str);
	GF_BGL_BmpWinDel(&bmpwin);			
}

void BR_disp_FontOam_Delete( BR_WORK* wk )
{
	int i,j;
	
	TAG* tag[2];
		
	for ( i = 0; i < TAG_MAX; i++ ){
		
		tag[ 0 ] = &wk->tag_man.tag_m[ i ]; 
		tag[ 1 ] = &wk->tag_man.tag_s[ i ]; 
		
		for ( j = 0; j < 2; j++ ){		
			FONTOAM_OAMDATA_Free( tag[ j ]->font_data );
			FONTOAM_OAMDATA_Delete( tag[ j ]->font_obj );
			CharVramAreaFree( &tag[ j ]->font_vram );
		}
	}
}


// =============================================================================
//
//
//	■ FONT OAM
//
//
// =============================================================================
void FontOam_Create( OAM_BUTTON* obtn, BR_SYS* sys, int draw_area )
{
	s16 ox, oy;
	FONTOAM_INIT		finit;
	STRBUF*				str;
	int					vram_size;
	MSGDATA_MANAGER*	man;
	GF_BGL_BMPWIN		bmpwin;	
	int pal_id = eID_OAM_SIDE_BAR_M;
	
	if ( draw_area == NNS_G2D_VRAM_TYPE_2DSUB ){
		pal_id = eID_OAM_SIDE_BAR_S;
	}	
						
	GF_BGL_BmpWinInit( &bmpwin );
	GF_BGL_BmpWinObjAdd( sys->bgl, &bmpwin, BR_TAG_BMP_WIX_SX, BR_TAG_BMP_WIX_SY, 0, 0 );
	
	obtn->font_data = FONTOAM_OAMDATA_Make( &bmpwin, HEAPID_BR );

	vram_size = FONTOAM_NeedCharSize( &bmpwin, draw_area, HEAPID_BR );
	
	CharVramAreaAlloc( vram_size, CHARM_CONT_AREACONT, draw_area, &obtn->font_vram );
	
	CATS_ObjectPosGetCap( obtn->cap, &ox, &oy );

	finit.fontoam_sys	= sys->fontoam_sys;
	finit.bmp			= &bmpwin;
	finit.clact_set		= CATS_GetClactSetPtr( sys->crp );
	finit.pltt			= CATS_PlttProxy( sys->crp, pal_id );
	finit.parent		= obtn->cap->act;
	finit.char_ofs		= obtn->font_vram.alloc_ofs;
	finit.x				= ox + FONT_OAM_TAG_OX;
	finit.y				= oy + FONT_OAM_TAG_OY;
	finit.bg_pri		= eBR_BG1_PRI;
	finit.soft_pri		= 0;
	finit.draw_area		= draw_area;
	finit.heap			= HEAPID_BR;
	
	obtn->font_obj = FONTOAM_OAMDATA_Init( &finit, obtn->font_data );
	FONTOAM_SetPaletteOffsetAddTransPlttNo( obtn->font_obj, dOAM_PAL_TAG_1 );	

	GF_BGL_BmpWinDel( &bmpwin );

	FONTOAM_SetDrawFlag( obtn->font_obj, FALSE );
}

void FontOam_MsgSet( OAM_BUTTON* obtn, BR_SYS* sys, int msg_id )
{
	STRBUF*				str;
	MSGDATA_MANAGER*	man;
	GF_BGL_BMPWIN		bmpwin;	

	man = sys->man;
	
	str	= MSGMAN_AllocString( man, msg_id );
	
	GF_BGL_BmpWinInit( &bmpwin );
	GF_BGL_BmpWinObjAdd( sys->bgl, &bmpwin, BR_TAG_BMP_WIX_SX, BR_TAG_BMP_WIX_SY, 0, 0 );
	GF_STR_PrintColor( &bmpwin, FONT_SYSTEM, str, 0, 0, MSG_NO_PUT, PRINT_COL_BLACK, NULL );
	
	FONTOAM_OAMDATA_ResetBmp( obtn->font_obj, obtn->font_data, &bmpwin, HEAPID_BR );
		
	STRBUF_Delete(str);
	GF_BGL_BmpWinDel(&bmpwin);		
}

void FontOam_Delete( OAM_BUTTON* obtn )
{	
	FONTOAM_OAMDATA_Free( obtn->font_data );
	FONTOAM_OAMDATA_Delete( obtn->font_obj );
	CharVramAreaFree( &obtn->font_vram );
}

void ExTag_ResourceLoad( BR_WORK* wk, int draw_area )
{
	ARCHANDLE*		 hdl = wk->sys.p_handle;
	GF_BGL_INI*		 bgl = wk->sys.bgl;
	CATS_SYS_PTR	 csp = wk->sys.csp;
	CATS_RES_PTR	 crp = wk->sys.crp;
	PALETTE_FADE_PTR pfd = wk->sys.pfd;
	
/*
	CATS_LoadResourcePlttWorkArcH( pfd, FADE_MAIN_OBJ, csp, crp, hdl, BR_ColorPaletteID_Get( wk, GET_PAL_OBJ ), FALSE, 1, NNS_G2D_VRAM_TYPE_2DMAIN, eID_OAM_SIDE_BAR_M );
	CATS_LoadResourcePlttWorkArcH( pfd, FADE_SUB_OBJ,  csp, crp, hdl, BR_ColorPaletteID_Get( wk, GET_PAL_OBJ ), FALSE, 1, NNS_G2D_VRAM_TYPE_2DSUB,  eID_OAM_SIDE_BAR_S );
*/
	CATS_LoadResourceCharArcH( csp, crp, hdl, NARC_batt_rec_gra_batt_rec_gds_tag2_NCGR, FALSE, draw_area, eID_OAM_EX_TAG );
	CATS_LoadResourceCellArcH( csp, crp, hdl, NARC_batt_rec_gra_batt_rec_tag2__gds_64k_NCER, FALSE, eID_OAM_EX_TAG );
	CATS_LoadResourceCellAnmArcH( csp, crp, hdl, NARC_batt_rec_gra_batt_rec_tag2__gds_64k_NANR, FALSE, eID_OAM_EX_TAG );
}

void ExTag_ResourceLoad_Box( BR_WORK* wk, int draw_area )
{
	ARCHANDLE*		 hdl = wk->sys.p_handle;
	GF_BGL_INI*		 bgl = wk->sys.bgl;
	CATS_SYS_PTR	 csp = wk->sys.csp;
	CATS_RES_PTR	 crp = wk->sys.crp;
	PALETTE_FADE_PTR pfd = wk->sys.pfd;

	CATS_LoadResourceCharArcH( csp, crp, hdl, NARC_batt_rec_gra_batt_rec_gds_tag_exbox_NCGR, FALSE, draw_area, eID_OAM_EX_TAG );
	CATS_LoadResourceCellArcH( csp, crp, hdl, NARC_batt_rec_gra_batt_rec_tag_gds_exbox_NCER, FALSE, eID_OAM_EX_TAG );
	CATS_LoadResourceCellAnmArcH( csp, crp, hdl, NARC_batt_rec_gra_batt_rec_tag_gds_exbox_NANR, FALSE, eID_OAM_EX_TAG );
}

void ExTag_ResourceLoad_Rank( BR_WORK* wk, int draw_area )
{
	ARCHANDLE*		 hdl = wk->sys.p_handle;
	GF_BGL_INI*		 bgl = wk->sys.bgl;
	CATS_SYS_PTR	 csp = wk->sys.csp;
	CATS_RES_PTR	 crp = wk->sys.crp;
	PALETTE_FADE_PTR pfd = wk->sys.pfd;
	
	int ncgr = NARC_batt_rec_gra_batt_rec_tag_gds_weekly_rank_NCGR;
	int ncer = NARC_batt_rec_gra_batt_rec_tag_gds_weekly_rank_NCER;
	int nanr = NARC_batt_rec_gra_batt_rec_tag_gds_weekly_rank_NANR;
	
	CATS_LoadResourceCharArcH( csp, crp, hdl, ncgr, FALSE, draw_area, eID_OAM_EX_TAG );
	CATS_LoadResourceCellArcH( csp, crp, hdl, ncer, FALSE, eID_OAM_EX_TAG );
	CATS_LoadResourceCellAnmArcH( csp, crp, hdl, nanr, FALSE, eID_OAM_EX_TAG );
}

void ExTag_ResourceLoad_VideoRank( BR_WORK* wk, int draw_area )
{
	ARCHANDLE*		 hdl = wk->sys.p_handle;
	GF_BGL_INI*		 bgl = wk->sys.bgl;
	CATS_SYS_PTR	 csp = wk->sys.csp;
	CATS_RES_PTR	 crp = wk->sys.crp;
	PALETTE_FADE_PTR pfd = wk->sys.pfd;
	
	int ncgr = NARC_batt_rec_gra_batt_rec_tag_gds_video_rank_NCGR;
	int ncer = NARC_batt_rec_gra_batt_rec_tag_gds_video_rank_NCER;
	int nanr = NARC_batt_rec_gra_batt_rec_tag_gds_video_rank_NANR;
	
	CATS_LoadResourceCharArcH( csp, crp, hdl, ncgr, FALSE, draw_area, eID_OAM_EX_TAG );
	CATS_LoadResourceCellArcH( csp, crp, hdl, ncer, FALSE, eID_OAM_EX_TAG );
	CATS_LoadResourceCellAnmArcH( csp, crp, hdl, nanr, FALSE, eID_OAM_EX_TAG );
}

void ExTag_ResourceLoad_Save( BR_WORK* wk, int draw_area )
{
	ARCHANDLE*		 hdl = wk->sys.p_handle;
	GF_BGL_INI*		 bgl = wk->sys.bgl;
	CATS_SYS_PTR	 csp = wk->sys.csp;
	CATS_RES_PTR	 crp = wk->sys.crp;
	PALETTE_FADE_PTR pfd = wk->sys.pfd;
	
	int ncgr = NARC_batt_rec_gra_batt_rec_tag_gds_save_NCGR;
	int ncer = NARC_batt_rec_gra_batt_rec_tag_gds_save_NCER;
	int nanr = NARC_batt_rec_gra_batt_rec_tag_gds_save_NANR;
	
	CATS_LoadResourceCharArcH( csp, crp, hdl, ncgr, FALSE, draw_area, eID_OAM_EX_TAG );
	CATS_LoadResourceCellArcH( csp, crp, hdl, ncer, FALSE, eID_OAM_EX_TAG );
	CATS_LoadResourceCellAnmArcH( csp, crp, hdl, nanr, FALSE, eID_OAM_EX_TAG );
}

void ExTag_ResourceDelete( BR_WORK* wk )
{
	CATS_FreeResourceChar( wk->sys.crp, eID_OAM_EX_TAG );
	CATS_FreeResourceCell( wk->sys.crp, eID_OAM_EX_TAG );
	CATS_FreeResourceCellAnm( wk->sys.crp, eID_OAM_EX_TAG );
}

CATS_ACT_PTR ExTag_Add( BR_WORK* wk, int d_area )
{
	CATS_ACT_PTR cap;
	TCATS_OBJECT_ADD_PARAM_S coap;
	
	coap.x		= 128;
	coap.y		=  96;
	coap.z		= 0;		
	coap.anm	= 0;
	coap.pri	= 1;
	coap.pal	= 0;
	coap.d_area = d_area;
	coap.bg_pri = eBR_BG1_PRI;
	coap.vram_trans = 0;
	
	coap.id[4] = CLACT_U_HEADER_DATA_NONE;
	coap.id[5] = CLACT_U_HEADER_DATA_NONE;
	
	coap.id[0]		= eID_OAM_EX_TAG;
	coap.id[2]		= eID_OAM_EX_TAG;
	coap.id[3]		= eID_OAM_EX_TAG;

	if ( d_area == NNS_G2D_VRAM_TYPE_2DMAIN ){
		coap.id[1]		= eID_OAM_SIDE_BAR_M;
	}
	else {
		coap.id[1]		= eID_OAM_SIDE_BAR_S;
	}
	
	cap = CATS_ObjectAdd_S( wk->sys.csp, wk->sys.crp, &coap );
	
	CATS_ObjectPaletteSetCap( cap, 2 );
	CATS_ObjectAnimeSeqSetCap( cap, 0 );
	CATS_ObjectUpdateCap( cap );
	
	return cap;
}



//--------------------------------------------------------------
/**
 * @brief	タグの破棄 必要になるすべての作業を行う NormalTag_RecoverAllOp と対で使う事前提
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void NormalTag_DeleteAllOp( BR_WORK* wk )
{
	BR_Tag_Enable( wk, FALSE );	///< font oam enable
	BR_Tag_Delete( wk );		///< CAP 解放
	BR_Tag_Release( wk );		///< タグの解放
}

//--------------------------------------------------------------
/**
 * @brief	タグ の復帰 必要になるすべての作業を行う
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void NormalTag_RecoverAllOp( BR_WORK* wk )
{
	BR_Tag_Load( wk );
	BR_Stack_Menu_Back( wk );
	BattleRecorder_MenuSetting( wk );
	BR_Tag_Add( wk );
	BR_Tag_Enable( wk, TRUE );
	BR_Tag_ReSet( wk );
	BR_Tag_FontOamReset( wk );
	BR_Tag_PosReset_Sub( wk );
	BR_Tag_SubRecover( wk );	
	
//	OS_Printf( "stack = %d\n", wk->tag_man.stack_s );
}


//--------------------------------------------------------------
/**
 * @brief	結果的に１段階戻さず、そのまま使う
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void NormalTag_RecoverAllOp_Simple( BR_WORK* wk )
{
	BR_Tag_Load( wk );
	BR_Stack_Menu_Back( wk );
	BattleRecorder_MenuSetting( wk );
	BR_Tag_Add( wk );
	BR_Tag_Enable( wk, TRUE );
	BR_Tag_ReSet( wk );
	BR_Tag_FontOamReset( wk );
	BR_Tag_PosReset_Sub( wk );
	BR_Tag_SubRecover( wk );	
	wk->tag_man.stack_s += 1;				///< proc init で Menu_Backが呼ばれ1段階下がってしまうので応急処置

//	OS_Printf( "stack = %d\n", wk->tag_man.stack_s );
}


//--------------------------------------------------------------
/**
 * @brief	ブレンドの初期化
 *
 * @param	eva	
 * @param	evb	
 * @param	mp1	
 * @param	mp2	
 * @param	sp1	
 * @param	sp2	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void Plate_AlphaInit( int* eva, int* evb, int mp1, int mp2, int sp1, int sp2, int plane )
{
	*eva =  0;
	*evb = 31;
	
	if ( plane == ePLANE_ALL ){	
		G2_SetBlendAlpha( mp1, mp2,	*eva, *evb );
		G2S_SetBlendAlpha( sp1, sp2, *eva, *evb );
	}
	if ( plane == ePLANE_MAIN ){	
		G2_SetBlendAlpha( mp1, mp2,	*eva, *evb );
	}
	if ( plane == ePLANE_SUB ){	
		G2S_SetBlendAlpha( sp1, sp2, *eva, *evb );
	}
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	eva	
 * @param	evb	
 * @param	mp1	
 * @param	mp2	
 * @param	sp1	
 * @param	sp2	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void Plate_AlphaSetPlane( int* eva, int* evb, int mp1, int mp2, int sp1, int sp2, int plane )
{
	if ( plane == ePLANE_ALL ){	
		G2_SetBlendAlpha( mp1, mp2,	*eva, *evb );
		G2S_SetBlendAlpha( sp1, sp2, *eva, *evb );	
	}
	if ( plane == ePLANE_MAIN ){	
		G2_SetBlendAlpha( mp1, mp2,	*eva, *evb );
	}
	if ( plane == ePLANE_SUB ){	
		G2S_SetBlendAlpha( sp1, sp2, *eva, *evb );
	}
}

//--------------------------------------------------------------
/**
 * @brief	デフォルトの初期化
 *
 * @param	eva	
 * @param	evb	
 *
 * @retval	inline void	
 *
 */
//--------------------------------------------------------------
void Plate_AlphaInit_Default( int* eva, int* evb, int plane )
{
	Plate_AlphaInit( eva, evb, DEF_MP1, DEF_MP2, DEF_SP1, DEF_SP2, plane );
}

//--------------------------------------------------------------
/**
 * @brief	アルファフェード
 *
 * @param	eva	
 * @param	evb	
 * @param	mode	0 = in		1 = out	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL Plate_AlphaFade( int* eva, int* evb, int mode, int plane )
{
	BOOL bEnd = TRUE;
	
	if ( mode == 0 ){		
		if ( *eva < 8 ){
			 *eva += 1;
			 bEnd = FALSE;
		}
		if ( *evb > 7 ){
			 *evb -= 3;
			 bEnd = FALSE;
		}
	}
	else {
		if ( *eva >  0 ){
			 *eva -= 1;
			 bEnd = FALSE;
		}
		if ( *evb < 31 ){
			 *evb += 3;
			 bEnd = FALSE;
		}
	}	
	if ( plane == ePLANE_ALL ){			
		G2_ChangeBlendAlpha( *eva, *evb );
		G2S_ChangeBlendAlpha( *eva, *evb );
	}
	if ( plane == ePLANE_MAIN ){	
		G2_ChangeBlendAlpha( *eva, *evb );
	}
	if ( plane == ePLANE_SUB ){	
		G2S_ChangeBlendAlpha( *eva, *evb );
	}
	
	return bEnd;
}


//--------------------------------------------------------------
/**
 * @brief	パレットフェード
 *
 * @param	pfd	
 * @param	color	
 * @param	logo_color	
 * @param	mode		0 = in	1 = out	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL BR_PaletteFade( int* color, int mode )
{
	BOOL bEnd = TRUE;
	
	if ( mode == 1 ){		
		if ( *color < 16 ){			
			 *color += 2;
			 bEnd = FALSE;
		}
	}
	else {		
		if ( *color > 0 ){			
			 *color -= 2;
			 bEnd = FALSE;
		}	
	}
	
	return bEnd;	
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	color	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_PaletteFadeIn_Init( int* color )
{
	*color = 16;
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	color	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_PaletteFadeOut_Init( int* color )
{
	*color = 0;
}
