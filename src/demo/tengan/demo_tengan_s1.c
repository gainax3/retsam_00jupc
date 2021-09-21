//==============================================================================
/**
 * @file	demo_tengan_s1.c
 * @brief	メインデモ
 * @author	goto
 * @date	2008.02.26(火)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================

#include "common.h"
#include "gflib/camera.h"

#include "system/procsys.h"
#include "system/msgdata.h"
#include "system/font_arc.h"
#include "system/brightness.h"
#include "system/arc_tool.h"
#include "system/clact_tool.h"

#include "system/laster.h"
#include "system/wipe.h"

#include "demo_tengan_common.h"

//==============================================================
// Prototype
//==============================================================
static void demo_scene01_ResourceLoad( DEMO_SCENE_01_WORK* wk );
static void demo_scene01_ResourceDelete( DEMO_SCENE_01_WORK* wk );
static void demo_scene01_Illuminant_Create( DEMO_SCENE_01_WORK* wk );
static void Demo_3D_Draw( DEMO_SCENE_01_WORK* wk );
static void Demo_CameraSet( GF_CAMERA_PTR camera, VecFx32* target );

static MDL_CMD_TBL akagi_look_d[] = {
	{ eCMD_DW, 4, 0 },
	{ eCMD_NONE, 1 },
};

static MDL_CMD_TBL akagi_look_u[] = {
	{ eCMD_DW, 4, 0 },
	{ eCMD_UW, 4, 0 },
	{ eCMD_NONE, 1 },
};

static MDL_CMD_TBL akagi_look_atozusari[] = {
	{ eCMD_UM, 2, -( FX32_HALF >> 1 ) },
	{ eCMD_NONE, 1 },
};

static MDL_CMD_TBL akagi_look_osare[] = {
	{ eCMD_UWB, 2, -( FX32_HALF >> 1 ) },
	{ eCMD_NONE, 1 },
};


//--------------------------------------------------------------
/**
 * @brief	カメラ設定
 *
 * @param	camera	
 * @param	target	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void Demo_CameraSet( GF_CAMERA_PTR camera, VecFx32* target )
{
	CAMERA_ANGLE angle = { -0x29fe, 0, 0 };
	
	GFC_InitCameraTDA( target, 0x13c805, &angle, 0x0c01, GF_CAMERA_PERSPECTIV, TRUE, camera );

	GFC_AttachCamera( camera );	
	
	GFC_SetCameraClip( FX32_ONE * 10, FX32_ONE * 1008, camera );
}


//--------------------------------------------------------------
/**
 * @brief	リソースの管理
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void demo_scene01_ResourceLoad( DEMO_SCENE_01_WORK* wk )
{
	ARCHANDLE*		 hdl = wk->sys->p_handle;
	GF_BGL_INI*		 bgl = wk->sys->bgl;
	CATS_SYS_PTR	 csp = wk->sys->csp;
	CATS_RES_PTR	 crp = wk->sys->crp;
	PALETTE_FADE_PTR pfd = wk->sys->pfd;
	int oam_id = eOAM_ID_ILLUMINANT;

	ArcUtil_HDL_BgCharSet( hdl, NARC_demo_tengan_gra_g_demo_bg_NCGR, bgl, GF_BGL_FRAME1_S, 0, 0, 0, HEAPID_DEMO_TENGAN );
	ArcUtil_HDL_ScrnSet( hdl, NARC_demo_tengan_gra_g_demo_bg1_NSCR, bgl, GF_BGL_FRAME1_S,  0, 0, 0, HEAPID_DEMO_TENGAN );
	PaletteWorkSet_Arc( pfd, ARC_DEMO_TENGAN_GRA, NARC_demo_tengan_gra_g_demo_bg_NCLR, HEAPID_DEMO_TENGAN, FADE_SUB_BG, 0x20 * 2, 0 );		
	PaletteWorkSet_Arc( pfd, ARC_DEMO_TENGAN_GRA, NARC_demo_tengan_gra_g_demo_bg_NCLR, HEAPID_DEMO_TENGAN, FADE_MAIN_BG, 0x20 * 2, 0 );		

	CATS_LoadResourcePlttWorkArcH( pfd, FADE_SUB_OBJ,  csp, crp, hdl, NARC_demo_tengan_gra_g_demo_oam_NCLR, FALSE, 3, NNS_G2D_VRAM_TYPE_2DSUB,  oam_id );
	CATS_LoadResourceCellArcH( csp, crp, hdl, NARC_demo_tengan_gra_g_demo_oam_NCER, FALSE, oam_id );
	CATS_LoadResourceCellAnmArcH( csp, crp, hdl, NARC_demo_tengan_gra_g_demo_oam_NANR, FALSE, oam_id );
	CATS_LoadResourceCharArcH( csp, crp, hdl, NARC_demo_tengan_gra_g_demo_oam_NCGR, FALSE, NNS_G2D_VRAM_TYPE_2DSUB,  oam_id );
	
	///< 3D
	
	///< map
	res_load_init( &wk->m_disp.map, NARC_demo_tengan_gra_g_demo_map_nsbmd, wk->sys->p_handle );
	
	///< okage
	{
		int i;
		
		for ( i = 0; i < 4; i++ ){
			res_load_init( &wk->m_disp.okage[ i ], NARC_demo_tengan_gra_kage_nsbmd, wk->sys->p_handle );
				
			D3DOBJ_SetScale( &wk->m_disp.okage[ i ].obj, FX32_CONST( 1.2 ), FX32_CONST( 1.0 ), FX32_CONST( 1.2 ) );
		}
	}
	
	///< colum
	res_load_init( &wk->m_disp.colum, NARC_demo_tengan_gra_g_demo_colum_01_nsbmd, wk->sys->p_handle );
	res_anime_init( 0, &wk->m_disp.colum, NARC_demo_tengan_gra_g_demo_colum_01_nsbca, wk->sys->p_handle, &wk->sys->allocator );
	res_anime_init( 1, &wk->m_disp.colum, NARC_demo_tengan_gra_g_demo_colum_01_nsbma, wk->sys->p_handle, &wk->sys->allocator );
	
	///< siru
	res_load_init( &wk->m_disp.siru, NARC_demo_tengan_gra_g_demo_siru_nsbmd, wk->sys->p_handle );
	res_anime_init( 0, &wk->m_disp.siru, NARC_demo_tengan_gra_g_demo_siru_nsbca, wk->sys->p_handle, &wk->sys->allocator );
	res_anime_init( 1, &wk->m_disp.siru, NARC_demo_tengan_gra_g_demo_siru_nsbma, wk->sys->p_handle, &wk->sys->allocator );
	
	///< tama
	res_load_init( &wk->m_disp.tama, NARC_demo_tengan_gra_g_demo_tama_nsbmd, wk->sys->p_handle );
	res_anime_init( 0, &wk->m_disp.tama, NARC_demo_tengan_gra_g_demo_tama_nsbca, wk->sys->p_handle, &wk->sys->allocator );
	res_anime_init( 1, &wk->m_disp.tama, NARC_demo_tengan_gra_g_demo_tama_nsbta, wk->sys->p_handle, &wk->sys->allocator );
//	wk->m_disp.poke[ 0 ].anm_speed = FX32_CONST( 1 );
	
	///< dia
	res_load_init( &wk->m_disp.poke[ 0 ], NARC_demo_tengan_gra_sppoke4_nsbmd, wk->sys->p_handle );
	D3DOBJ_SetMatrix( &wk->m_disp.poke[ 0 ].obj, FX32_CONST( -50 ), FX32_CONST( +0 ), FX32_CONST( -50 ) );
	res_anime_init( 0, &wk->m_disp.poke[ 0 ], NARC_demo_tengan_gra_sppoke4_nsbtp, wk->sys->p_handle, &wk->sys->allocator );
	wk->m_disp.poke[ 0 ].bAnime = TRUE;
	wk->m_disp.poke[ 0 ].bAnimeLoop = TRUE;
	wk->m_disp.poke[ 0 ].anm_speed = FX32_HALF;
	
	///< pal
	res_load_init( &wk->m_disp.poke[ 1 ], NARC_demo_tengan_gra_sppoke5_nsbmd, wk->sys->p_handle );
	D3DOBJ_SetMatrix( &wk->m_disp.poke[ 1 ].obj, FX32_CONST( +50 ), FX32_CONST( +0 ), FX32_CONST( -50 ) );
	res_anime_init( 0, &wk->m_disp.poke[ 1 ], NARC_demo_tengan_gra_sppoke5_nsbtp, wk->sys->p_handle, &wk->sys->allocator );
	wk->m_disp.poke[ 1 ].bAnime = TRUE;
	wk->m_disp.poke[ 1 ].bAnimeLoop = TRUE;
	wk->m_disp.poke[ 1 ].anm_speed = FX32_HALF;
	
	///< gira
	res_load_init( &wk->m_disp.gira[ 0 ], NARC_demo_tengan_gra_g_demo_gira_a_nsbmd, wk->sys->p_handle );
	D3DOBJ_SetMatrix( &wk->m_disp.gira[ 0 ].obj, FX32_CONST( 0 ), FX32_CONST( -90 ), FX32_CONST( 0 ) );
	D3DOBJ_SetDraw( &wk->m_disp.gira[ 0 ].obj, FALSE );
	res_anime_init( 0, &wk->m_disp.gira[ 0 ], NARC_demo_tengan_gra_g_demo_gira_a_nsbca, wk->sys->p_handle, &wk->sys->allocator );
	res_anime_init( 1, &wk->m_disp.gira[ 0 ], NARC_demo_tengan_gra_g_demo_gira_a_nsbta, wk->sys->p_handle, &wk->sys->allocator );
	wk->m_disp.gira[ 0 ].bTexAnime = TRUE;
	
	res_load_init( &wk->m_disp.gira[ 1 ], NARC_demo_tengan_gra_g_demo_gira_b_nsbmd, wk->sys->p_handle );
	D3DOBJ_SetDraw(&wk->m_disp.gira[ 1 ].obj, FALSE );	
	res_anime_init( 0, &wk->m_disp.gira[ 1 ], NARC_demo_tengan_gra_g_demo_gira_b_nsbca, wk->sys->p_handle, &wk->sys->allocator );
	res_anime_init( 1, &wk->m_disp.gira[ 1 ], NARC_demo_tengan_gra_g_demo_gira_b_nsbta, wk->sys->p_handle, &wk->sys->allocator );
	wk->m_disp.gira[ 1 ].bTexAnime = TRUE;
	
	res_load_init( &wk->m_disp.gira[ 2 ], NARC_demo_tengan_gra_g_demo_gira_c_nsbmd, wk->sys->p_handle );
	D3DOBJ_SetDraw( &wk->m_disp.gira[ 2 ].obj, FALSE );
	res_anime_init( 0, &wk->m_disp.gira[ 2 ], NARC_demo_tengan_gra_g_demo_gira_c_nsbca, wk->sys->p_handle, &wk->sys->allocator );
	res_anime_init( 1, &wk->m_disp.gira[ 2 ], NARC_demo_tengan_gra_g_demo_gira_c_nsbta, wk->sys->p_handle, &wk->sys->allocator );
	wk->m_disp.gira[ 2 ].bTexAnime = TRUE;
	
	res_load_init( &wk->m_disp.gira[ 3 ], NARC_demo_tengan_gra_g_demo_gira_d_nsbmd, wk->sys->p_handle );
	D3DOBJ_SetDraw( &wk->m_disp.gira[ 3 ].obj, FALSE );
	res_anime_init( 0, &wk->m_disp.gira[ 3 ], NARC_demo_tengan_gra_g_demo_gira_d_nsbca, wk->sys->p_handle, &wk->sys->allocator );
	res_anime_init( 1, &wk->m_disp.gira[ 3 ], NARC_demo_tengan_gra_g_demo_gira_d_nsbta, wk->sys->p_handle, &wk->sys->allocator );
	wk->m_disp.gira[ 3 ].bTexAnime = TRUE;
		
	res_load_init( &wk->m_disp.gira[ 4 ], NARC_demo_tengan_gra_g_demo_gira_e_nsbmd, wk->sys->p_handle );
	D3DOBJ_SetDraw( &wk->m_disp.gira[ 4 ].obj, FALSE );
	res_anime_init( 0, &wk->m_disp.gira[ 4 ], NARC_demo_tengan_gra_g_demo_gira_e_nsbca, wk->sys->p_handle, &wk->sys->allocator );
	res_anime_init( 1, &wk->m_disp.gira[ 4 ], NARC_demo_tengan_gra_g_demo_gira_e_nsbta, wk->sys->p_handle, &wk->sys->allocator );
	wk->m_disp.gira[ 4 ].bTexAnime = TRUE;
	
	///< kage
	res_load_init( &wk->m_disp.kage[ 0 ], NARC_demo_tengan_gra_g_demo_kage01_nsbmd, wk->sys->p_handle );
	D3DOBJ_SetDraw( &wk->m_disp.kage[ 0 ].obj, FALSE );
	res_anime_init( 0, &wk->m_disp.kage[ 0 ], NARC_demo_tengan_gra_g_demo_kage01_nsbca, wk->sys->p_handle, &wk->sys->allocator );
	
	res_load_init( &wk->m_disp.kage[ 1 ], NARC_demo_tengan_gra_g_demo_kage02_nsbmd, wk->sys->p_handle );
	D3DOBJ_SetDraw( &wk->m_disp.kage[ 1 ].obj, FALSE );	
	
	///< hero or heroine
	if ( MyStatus_GetMySex( wk->param->my_status ) != PM_FEMALE ){
		res_load_init( &wk->m_disp.hobj[ 0 ], NARC_demo_tengan_gra_hero_nsbmd, wk->sys->p_handle );
		res_anime_init( 0, &wk->m_disp.hobj[ 0 ], NARC_demo_tengan_gra_hero_nsbtp, wk->sys->p_handle, &wk->sys->allocator );
	}
	else {
		res_load_init( &wk->m_disp.hobj[ 0 ], NARC_demo_tengan_gra_heroine_nsbmd, wk->sys->p_handle );
		res_anime_init( 0, &wk->m_disp.hobj[ 0 ], NARC_demo_tengan_gra_heroine_nsbtp, wk->sys->p_handle, &wk->sys->allocator );
	}
	D3DOBJ_SetMatrix( &wk->m_disp.hobj[ 0 ].obj, FX32_CONST( 1 ), FX32_CONST( +0 ), FX32_CONST( +140 ) );
	wk->m_disp.hobj[ 0 ].bAnimeLoop = TRUE;
	wk->m_disp.hobj[ 0 ].anm_speed = ( FX32_HALF >> 1 );
	wk->m_disp.hobj[ 0 ].anm_no = 2;

	///< akagi
	res_load_init( &wk->m_disp.hobj[ 1 ], NARC_demo_tengan_gra_gingaboss_nsbmd, wk->sys->p_handle );
	D3DOBJ_SetMatrix( &wk->m_disp.hobj[ 1 ].obj, FX32_CONST( 1 ), FX32_CONST( +0 ), FX32_CONST( +60 ) );
	res_anime_init( 0, &wk->m_disp.hobj[ 1 ], NARC_demo_tengan_gra_gingaboss_nsbtp, wk->sys->p_handle, &wk->sys->allocator );
	wk->m_disp.hobj[ 1 ].bAnimeLoop = TRUE;
	wk->m_disp.hobj[ 1 ].anm_speed = ( FX32_HALF >> 1 );
	wk->m_disp.hobj[ 1 ].anm_no = 2;
}


//--------------------------------------------------------------
/**
 * @brief	OMAの登録
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void demo_scene01_Illuminant_Create( DEMO_SCENE_01_WORK* wk )
{
	int i;
	
	ARCHANDLE*		 hdl = wk->sys->p_handle;
	GF_BGL_INI*		 bgl = wk->sys->bgl;
	CATS_SYS_PTR	 csp = wk->sys->csp;
	CATS_RES_PTR	 crp = wk->sys->crp;
	PALETTE_FADE_PTR pfd = wk->sys->pfd;
	TCATS_OBJECT_ADD_PARAM_S coap;

	coap.x		= 0;
	coap.y		= 0;
	coap.z		= 0;		
	coap.anm	= 0;
	coap.pri	= 0;
	coap.pal	= 0;
	coap.d_area = CATS_D_AREA_SUB;
	coap.bg_pri = 0;
	coap.vram_trans = 0;
	
	coap.id[4] = CLACT_U_HEADER_DATA_NONE;
	coap.id[5] = CLACT_U_HEADER_DATA_NONE;
	
	coap.id[0] = eOAM_ID_ILLUMINANT;
	coap.id[1] = eOAM_ID_ILLUMINANT;
	coap.id[2] = eOAM_ID_ILLUMINANT;
	coap.id[3] = eOAM_ID_ILLUMINANT;
	
	for ( i = 0; i < ILLUMINANT_MAX; i++ ){
		
		wk->s_disp.illum[ i ].cap = CATS_ObjectAdd_S( csp, crp, &coap );

		CATS_ObjectUpdateCap( wk->s_disp.illum[ i ].cap );
		CATS_ObjectAffineSetCap( wk->s_disp.illum[ i ].cap, CLACT_AFFINE_DOUBLE );
		CATS_ObjectScaleSetCap( wk->s_disp.illum[ i ].cap, 0.5f, 0.5f );		
		CATS_ObjectPaletteOffsetSetCap( wk->s_disp.illum[ i ].cap, i );		
		CATS_ObjectPosSetCap( wk->s_disp.illum[ i ].cap, 16 + ( i * 64 ), 64 );
//		CATS_ObjectObjModeSetCap( wk->s_disp.illum[ i ].cap, GX_OAM_MODE_XLU );
		
		wk->s_disp.illum[ i ].ai  = 1;
		wk->s_disp.illum[ i ].id  = i;
		wk->s_disp.illum[ i ].z	  = 50;
		wk->s_disp.illum[ i ].rad = i * 120;
		wk->s_disp.illum[ i ].rad2= wk->s_disp.illum[ i ].rad;
		wk->s_disp.illum[ i ].count = i;
		wk->s_disp.illum[ i ].para[0] = 1;
		wk->s_disp.illum[ i ].para[1] = 1;
		wk->s_disp.illum[ i ].para[2] = gf_rand() % 10;
		wk->s_disp.illum[ i ].para[3] = 0;
		wk->s_disp.illum[ i ].tcb = TCB_Add( Illuminant_AI_TCB, &wk->s_disp.illum[ i ], eTCB_PRI_ILLUM_AI );
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
static void demo_scene01_ResourceDelete( DEMO_SCENE_01_WORK* wk )
{
	int i;
	
	for ( i = 0; i < ILLUMINANT_MAX; i++ ){
		
		TCB_Delete( wk->s_disp.illum[ i ].tcb );
		CATS_ActorPointerDelete_S( wk->s_disp.illum[ i ].cap );
	}
	
//	D3DOBJ_MdlDelete( &wk->m_disp.map.mdl );
}


//--------------------------------------------------------------
/**
 * @brief	シーン１初期化
 *
 * @param	wk	
 *
 * @retval	void*	
 *
 */
//--------------------------------------------------------------
void* Demo_Tengan_Scene01_Init( DEMO_TENGAN_WORK* main_wk )
{	
	DEMO_SCENE_01_WORK* wk = sys_AllocMemory( HEAPID_DEMO_TENGAN, sizeof( DEMO_SCENE_01_WORK ) );
	
	memset( wk, 0, sizeof( DEMO_SCENE_01_WORK ) );
	
	wk->sys		= &main_wk->sys;
	wk->param	= main_wk->param;

	demo_scene01_ResourceLoad( wk );
	demo_scene01_Illuminant_Create( wk );
	
	{
		DEFF_Laster_Init( &wk->s_disp.laster, HEAPID_DEMO_TENGAN );		
		DEFF_Laster_Start( &wk->s_disp.laster, 0, 191, 
						  DEMO_LASTER_SIN_ADDR, 
						  FX32_CONST( 4 ), ( 1 * 100 ),
						  LASTER_SCROLL_SBG1, 0, 0x1000, 1 );
	}
	
	{
		G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2,
						  GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,	7, 8 );	
		G2S_SetBlendAlpha( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1,
						   GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_OBJ,  7, 10 );

		{	
			static const GXRgb edge_color_table[ 8 ] = {
				GX_RGB( 2, 2, 2),GX_RGB(10,10,10),GX_RGB(10,10,10),GX_RGB(10,10,10),
				GX_RGB(10,10,10),GX_RGB(10,10,10),GX_RGB(10,10,10),GX_RGB(10,10,10),
			};
			
			G3X_EdgeMarking( TRUE );
			G3X_SetEdgeColorTable( edge_color_table );
		}
	}	
//	wk->sys->camera_target.z = FX32_CONST( 68 );
	Demo_CameraSet( wk->sys->camera_p, &wk->sys->camera_target );
	wk->sys->camera_target.y = FX32_CONST( 0 );
//	wk->sys->camera_target.z = FX32_CONST( 68 );

	Demo_MainDispSet( DISP_3D_TO_SUB );
	Demo_BlendBrightness( main_wk, DEMO_BRIGHTNESS_WHITE );
	#if 0
		{
			DEMO_MOTION_BL_DATA mb = {
				GX_DISPMODE_VRAM_C,
				GX_BGMODE_0,
				GX_BG0_AS_3D,			
				GX_CAPTURE_SIZE_256x192,
				GX_CAPTURE_MODE_AB,
				GX_CAPTURE_SRCA_2D3D,
				GX_CAPTURE_SRCB_VRAM_0x00000,
				GX_CAPTURE_DEST_VRAM_C_0x00000,
				4,
				12,
				HEAPID_DEMO_TENGAN
			};
			wk->sys->motion_bl = DEMO_MOTION_BL_Init( &mb );
		}
	#endif
	return wk;
}

//--------------------------------------------------------------
/**
 * @brief	シーン１メイン
 *
 * @param	work	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL Demo_Tengan_Scene01_Main( void* work )
{
	enum {
		FADE_IN = 0,		
		FADE_IN_WAIT,
		CAM_MOVE_INIT_1,
		CAM_MOVE_1,
		AKAGI_MSG_1,		
		CAM_MOVE_INIT_2,
		CAM_MOVE_2,
		AKAGI_MSG_2,		
		SHADOW_MAIN,		
		CAM_MOVE_INIT_3,
		CAM_MOVE_3,		
		GIRA_INIT,
		GIRA_MAIN,
		GIRA_MAIN_2,
		GIRA_MAIN_3,
		GIRA_MAIN_4,
		GIRA_MAIN_5,
		GIRA_MAIN_6,
		GIRA_MAIN_7,
		GIRA_MAIN_8,		
		END_FADE_OUT,
		END_FADE_OUT_WAIT,
	};
	DEMO_SCENE_01_WORK* wk = ( DEMO_SCENE_01_WORK* )work;
	
	switch ( wk->cut ){
	case FADE_IN:
		WIPE_SYS_Start(	WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_WHITE,
						COMM_BRIGHTNESS_SYNC, 1, HEAPID_DEMO_TENGAN  );
		wk->sys->light_color.brightness = 0;		
		G2_SetBlendBrightness( DEMO_BLEND_MASK_M, wk->sys->light_color.brightness );
		G2S_SetBlendBrightness( DEMO_BLEND_MASK_S, wk->sys->light_color.brightness );
		wk->cut++;
	
	case FADE_IN_WAIT:		
		if( WIPE_SYS_EndCheck() == FALSE ) { break; }

		wk->cut++;
		break;
		
	case CAM_MOVE_INIT_1:	
		{
			wk->sys->cw.x = 20;
			wk->sys->cw.y = 0;
			wk->sys->cw.z = 0;
			wk->sys->cw.set_frame = 60;
			wk->sys->cw.camera_p = wk->sys->camera_p;	
			
			wk->sys->cw.mx = 0;
			wk->sys->cw.my = 0;
			wk->sys->cw.mz = 0;
			wk->sys->cw.camera_target = &wk->sys->camera_target;	
			Camera_Work_Init( &wk->sys->cw );
			wk->cut++;
			
			{
			//	Snd_BgmPlay( SEQ_EYE_CHAMP );
			//	Snd_BgmPlay( SEQ_BA_CHANP );
			//	Snd_DataSetByScene( SND_SCENE_GIRA, SEQ_PL_EV_GIRA, 0 );	// サウンドデータロード		
			//	Snd_BgmPlay( SND_SCENE_GIRA );
			//	Snd_BgmPlay( SEQ_THE_EVENT02 );		
			}
		}
		break;
	
	case CAM_MOVE_1:
		if ( Camera_Work_Main( &wk->sys->cw ) ){
			Demo_MSG_Set( wk->sys, wk->param, msg_d05r0114_gingaboss_06 );
			wk->cut++;
		}
		break;
	
	case AKAGI_MSG_1:
		if ( GF_MSG_PrintEndCheck( wk->sys->msg_index ) ){ break; }
		Demo_MSG_Off( wk->sys );
		wk->cut++;		
		break;
	
	case CAM_MOVE_INIT_2:
		{			
			wk->sys->cw.x = 0;
			wk->sys->cw.y = 0;
			wk->sys->cw.z = 0;
			wk->sys->cw.set_frame = 60;
			wk->sys->cw.camera_p = wk->sys->camera_p;
			
			wk->sys->cw.mx = 0;
			wk->sys->cw.my = 0;
			wk->sys->cw.mz = FX32_CONST( 70 );
			wk->sys->cw.camera_target = &wk->sys->camera_target;
			Camera_Work_Init( &wk->sys->cw );
			wk->cut++;
		}
		break;
		
	case CAM_MOVE_2:
		if ( Camera_Work_Main( &wk->sys->cw ) == FALSE ){ break; }
		if ( ++wk->wait < 10 ){ break; }		
		Demo_MSG_Set( wk->sys, wk->param, msg_d05r0114_gingaboss_07 );
		Demo_Anime_Start( &wk->m_disp.hobj[ 1 ], akagi_look_d );
		wk->cut++;
		wk->wait = 0;
		break;
	
	case AKAGI_MSG_2:
		if ( GF_MSG_PrintEndCheck( wk->sys->msg_index ) ){ break; }
		Demo_MSG_Off( wk->sys );	
		Demo_MSG_Set( wk->sys, wk->param, msg_d05r0114_gingaboss_08 );
		Snd_BgmFadeOut( 0, 10 );
		wk->cut++;
		break;
				
	case SHADOW_MAIN:
		if ( GF_MSG_PrintEndCheck( wk->sys->msg_index ) ){ break; }
		Demo_MSG_Off( wk->sys );
		wk->wait++;
		if ( wk->wait == 15 ){
			{
				int i;
				for ( i = 0; i < 3; i++ ){
					wk->s_disp.illum[ i ].ai %= 2;
				 	wk->s_disp.illum[ i ].ai += 2;
				}
			}
		}
		if ( wk->wait < 30 ){ break; }
		
		if ( wk->wait == 31 ){
			//Snd_SePlay( DS_DARK );
		}
		
		if ( wk->sys->light_color.brightness > BRIGHTNESS_MIN ){
			if ( wk->wait % 2 ) {		
				wk->sys->light_color.brightness--;
			}
			G2_SetBlendBrightness( DEMO_BLEND_MASK_M, wk->sys->light_color.brightness );
			G2S_SetBlendBrightness( DEMO_BLEND_MASK_S, wk->sys->light_color.brightness );
		}
		else {
			Demo_MSG_Set( wk->sys, wk->param, msg_d05r0114_gingaboss_09 );		
		//	Demo_Anime_Start( &wk->m_disp.hobj[ 1 ], akagi_look_u );
			wk->wait = 0;
			wk->cut++;
		}
		break;
		
	case CAM_MOVE_INIT_3:
		if ( GF_MSG_PrintEndCheck( wk->sys->msg_index ) ){ break; }		
		Demo_MSG_Off( wk->sys );		
		Demo_Anime_Start( &wk->m_disp.hobj[ 1 ], akagi_look_u );
		{			
			wk->sys->cw.x = 0;
			wk->sys->cw.y = 0;
			wk->sys->cw.z = 0;
			wk->sys->cw.set_frame = 90;
			wk->sys->cw.camera_p = wk->sys->camera_p;
			
			wk->sys->cw.mx = 0;
			wk->sys->cw.my = 0;
			wk->sys->cw.mz = -FX32_CONST( 80 );
			wk->sys->cw.camera_target = &wk->sys->camera_target;
			Camera_Work_Init( &wk->sys->cw );
			wk->cut++;
		}
		break;
		
	case CAM_MOVE_3:
		if ( Camera_Work_Main( &wk->sys->cw ) == FALSE ){ break; }
		wk->cut++;
		wk->wait = 0;
		break;
			
	case GIRA_INIT:
		
		wk->m_disp.kage[ 0 ].bAnime = TRUE;					///< 影 0 anime = TRUE
		D3DOBJ_SetDraw( &wk->m_disp.kage[ 0 ].obj, TRUE );	///< 影
		Snd_BgmPlay( SEQ_PL_EV_GIRA );
		Snd_DataSetByScene( SND_SCENE_SUB_CLIMAX, 0, 0 );	//サウンドデータロード(BGM引継ぎ)
		wk->cut++;
		break;
			
	case GIRA_MAIN:
		if ( (++wk->wait ) == 60 ){
			Demo_Anime_Start( &wk->m_disp.hobj[ 1 ], akagi_look_atozusari );
		}
		if ( wk->wait == 15 
		||	 wk->wait == 45
		||	 wk->wait == 75 
		||	 wk->wait == 95
		||	 wk->wait == 115
		||	 wk->wait == 130
		||	 wk->wait == 145 ){
			Snd_SePlay( DS_HOLE );
			Snd_PlayerSetTrackPitchBySeqNo( DS_HOLE, 0xffff, 
									(wk->wait / 30 * 32) + (wk->wait % 32 * 10) );	//ピッチ
		}
		if ( wk->m_disp.kage[ 0 ].bAnime == FALSE ){
			D3DOBJ_SetDraw( &wk->m_disp.kage[ 0 ].obj, FALSE );
			D3DOBJ_SetDraw( &wk->m_disp.kage[ 1 ].obj, TRUE );
			D3DOBJ_SetDraw( &wk->m_disp.gira[ 0 ].obj, TRUE );
			wk->m_disp.gira[ 0 ].bAnime = TRUE;
			wk->wait = 0;
			wk->cut++;
		}
		break;
	
	case GIRA_MAIN_2:
		if ( wk->m_disp.gira[ 0 ].obj.matrix.y < FX32_CONST( -50 ) ){
			wk->m_disp.gira[ 0 ].obj.matrix.y += FX32_HALF;//FX32_CONST( 1 );
		}
		else {
			wk->wait = 0;
			wk->cut++;
		}
		break;
	
	case GIRA_MAIN_3:
		if ( wk->m_disp.gira[ 0 ].bAnime == FALSE ){
			wk->m_disp.gira[ 1 ].obj.matrix.y = wk->m_disp.gira[ 0 ].obj.matrix.y;
			wk->m_disp.gira[ 1 ].bAnime = TRUE;
			wk->m_disp.gira[ 1 ].bAnimeLoop = TRUE;
			wk->m_disp.colum.bAnime = TRUE;
			wk->m_disp.colum.bColorAnime = TRUE;
			wk->m_disp.tama.bAnime = TRUE;
			wk->m_disp.tama.bTexAnime = TRUE;
			D3DOBJ_SetDraw( &wk->m_disp.gira[ 0 ].obj, FALSE );
			D3DOBJ_SetDraw( &wk->m_disp.gira[ 1 ].obj, TRUE );
			wk->cut++;
		}
		break;
	
	case GIRA_MAIN_4:
		///< 昇ってるトコ
		wk->wait++;
		
		if ( wk->wait == 1 ){
			Snd_SePlay( DS_WAVE );
		}
		if ( wk->wait == 20 ){
			Demo_Anime_Start( &wk->m_disp.hobj[ 1 ], akagi_look_osare );
		}
		
		if ( wk->wait == 15 + 25 ){
			Snd_PMVoicePlayEx( PV_NORMAL, MONSNO_IA, -80, 40, HEAPID_DEMO_TENGAN, 0 );
		}
		if ( wk->wait == 40 + 25 ){
			Snd_PMVoicePlayEx( PV_NORMAL, MONSNO_EA, +80, 40, HEAPID_DEMO_TENGAN, 0 );
		}
		if ( wk->wait < 15 + 25 ){
			wk->m_disp.poke[ 0 ].obj.matrix.z -= FX32_HALF >> 1;
		}
		else {			
			wk->m_disp.poke[ 0 ].obj.matrix.z -= FX32_HALF;
		}
		if ( wk->wait < 40 + 25 ){
			wk->m_disp.poke[ 1 ].obj.matrix.z -= FX32_HALF >> 1;
		}
		else {
			wk->m_disp.poke[ 1 ].obj.matrix.z -= FX32_HALF;
		}
		
		if ( wk->m_disp.gira[ 1 ].obj.matrix.y < FX32_CONST( 0 ) ){
			wk->m_disp.gira[ 1 ].obj.matrix.y += FX32_HALF;//FX32_CONST( 1 );
		}
		else {
			wk->m_disp.gira[ 1 ].obj.matrix.y = FX32_CONST( 0 );
			wk->wait = 0;
			Demo_MSG_Set( wk->sys, wk->param, msg_d05r0114_gingaboss_10 );
			wk->cut++;
		}
		break;
	
	case GIRA_MAIN_5:
		///< 羽開くトコ
		if ( GF_MSG_PrintEndCheck( wk->sys->msg_index ) ){ break; }
		Demo_MSG_Off( wk->sys );
//		if ( ++wk->wait <= 90 ){ break; }
		
		wk->m_disp.siru.bAnime		= TRUE;
		wk->m_disp.siru.bAnimeLoop	= FALSE;
		wk->m_disp.siru.bColorAnime = TRUE;
			
		wk->m_disp.gira[ 2 ].bAnime = TRUE;
		D3DOBJ_SetDraw( &wk->m_disp.gira[ 1 ].obj, FALSE );
		D3DOBJ_SetDraw( &wk->m_disp.gira[ 2 ].obj, TRUE );
		Snd_PMVoicePlayEx( PV_FLDEVENT, MONSNO_KIMAIRAN, 0, PV_VOL_MAX, HEAPID_DEMO_TENGAN, 0 );
		wk->wait = 0;
		wk->cut++;
		break;
	
	case GIRA_MAIN_6:			
		if ( wk->m_disp.gira[ 2 ].bAnime == FALSE ){
			Demo_MSG_Set( wk->sys, wk->param, msg_d05r0114_gingaboss_11 );
			wk->m_disp.gira[ 3 ].bAnime = TRUE;
			wk->m_disp.gira[ 3 ].bAnimeLoop = TRUE;
			
			wk->m_disp.siru.bAnime = FALSE;
			D3DOBJ_SetDraw( &wk->m_disp.siru.obj, FALSE );

			D3DOBJ_SetDraw( &wk->m_disp.gira[ 2 ].obj, FALSE );
			D3DOBJ_SetDraw( &wk->m_disp.gira[ 3 ].obj, TRUE );
			wk->cut++;
		}
		break;
	
	case GIRA_MAIN_7:
		if ( GF_MSG_PrintEndCheck( wk->sys->msg_index ) ){ break; }
		if ( wk->wait == 0 ){
			Demo_MSG_Off( wk->sys );
		}
		Snd_BgmPlay( SEQ_PL_EV_GIRA2 );
//		if ( ++wk->wait <= 90 ){ break; }				
		wk->m_disp.gira[ 4 ].bAnime = TRUE;
		D3DOBJ_SetDraw( &wk->m_disp.gira[ 3 ].obj, FALSE );
		D3DOBJ_SetDraw( &wk->m_disp.gira[ 4 ].obj, TRUE );
		wk->wait = 0;
		wk->cut++;
		break;
		
	case GIRA_MAIN_8:
		if ( (++wk->wait) == 238 ){
			Snd_PMVoicePlayEx( PV_FLDEVENT, MONSNO_KIMAIRAN, 0, PV_VOL_MAX, HEAPID_DEMO_TENGAN, 0 );
		//	wk->sys->msg_index = Demo_MSG_Set( wk->sys, wk->param, 5 );
		}

		if ( wk->wait == 170 ){
			Demo_MSG_Set( wk->sys, wk->param, msg_d05r0114_gingaboss_12 );
//			wk->sys->msg_index = Demo_MSG_Set( wk->sys, wk->param, eMSG_1_6 );
		}		
		if ( wk->wait == 180 ){
//			wk->sys->msg_index = Demo_MSG_Set( wk->sys, wk->param, eMSG_1_7 );
		}
		if ( wk->wait == 210 ){
//			wk->sys->msg_index = Demo_MSG_Set( wk->sys, wk->param, eMSG_1_8 );
		}
		if ( wk->wait == 220 ){
//			wk->sys->msg_index = Demo_MSG_Set( wk->sys, wk->param, eMSG_1_9 );
		}
		if ( wk->wait == 230 ){
//			wk->sys->msg_index = Demo_MSG_Set( wk->sys, wk->param, eMSG_1_10 );
		}

		if ( wk->m_disp.gira[ 4 ].bAnime == FALSE ){
			if ( GF_MSG_PrintEndCheck( wk->sys->msg_index ) ){
				GF_STR_PrintForceStop( wk->sys->msg_index );
			}
			wk->sys->light_color.brightness = BRIGHTNESS_BK;
			G2_SetBlendBrightness( DEMO_BLEND_MASK_M, wk->sys->light_color.brightness );
			G2S_SetBlendBrightness( DEMO_BLEND_MASK_S, wk->sys->light_color.brightness );
			{
				u16 bgm = Snd_NowBgmNoGet();				
				Snd_BgmStop( bgm, 0 );		
			}
			Demo_MSG_Off( wk->sys );
			wk->cut++;
			wk->wait = 0;
		}
		break;
	
	case END_FADE_OUT:
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, 1, 1, HEAPID_DEMO_TENGAN  );
		wk->cut++;
		
	case END_FADE_OUT_WAIT:
		if( WIPE_SYS_EndCheck() == FALSE ) { break; }
		wk->sys->light_color.brightness = 0;
		G2_SetBlendBrightness( DEMO_BLEND_MASK_M, wk->sys->light_color.brightness );
		G2S_SetBlendBrightness( DEMO_BLEND_MASK_S, wk->sys->light_color.brightness );		
		wk->cut++;
		break;
	
	default:
		if ( (++wk->wait) >= 60 ){
			wk->cut = 0;
			return FALSE;
		}
		break;
	}
	
	wk->m_disp.okage[ 0 ].obj.matrix = wk->m_disp.poke[ 0 ].obj.matrix;
	wk->m_disp.okage[ 1 ].obj.matrix = wk->m_disp.poke[ 1 ].obj.matrix;
	wk->m_disp.okage[ 2 ].obj.matrix = wk->m_disp.hobj[ 0 ].obj.matrix;
	wk->m_disp.okage[ 3 ].obj.matrix = wk->m_disp.hobj[ 1 ].obj.matrix;
	
	#define KAGE_OFS ( FX32_ONE * 2 )
	wk->m_disp.okage[ 0 ].obj.matrix.z -= KAGE_OFS;
	wk->m_disp.okage[ 1 ].obj.matrix.z -= KAGE_OFS;
	wk->m_disp.okage[ 2 ].obj.matrix.z -= KAGE_OFS;
	wk->m_disp.okage[ 3 ].obj.matrix.z -= KAGE_OFS;
	
	wk->m_disp.okage[ 2 ].obj.matrix.x -= FX32_ONE;
	wk->m_disp.okage[ 3 ].obj.matrix.x -= FX32_ONE;
	#undef KAGE_OFS
	
	Demo_3D_Draw( wk );
	BG_PaletteFade_Sub( wk->sys );
	
	return TRUE;	
}


//--------------------------------------------------------------
/**
 * @brief	シーン１終了
 *
 * @param	work	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL Demo_Tengan_Scene01_Exit( void* work )
{
	DEMO_SCENE_01_WORK* wk = ( DEMO_SCENE_01_WORK* )work;
	
	switch( wk->cut ){
	case 0:
	#if 0
		TCB_Delete( wk->s_disp.illum[ 0 ].tcb );
		TCB_Delete( wk->s_disp.illum[ 1 ].tcb );
		TCB_Delete( wk->s_disp.illum[ 2 ].tcb );
	#endif
		demo_scene01_ResourceDelete( wk );
		DEFF_Laster_Delete( &wk->s_disp.laster );
		wk->cut++;
		break;
	
	case 1:
			
		res_model_delete( &wk->m_disp.map, &wk->sys->allocator, 0 );
		
		{
			int i;
			
			for ( i = 0; i < 4; i++ ){
				res_model_delete( &wk->m_disp.okage[ i ], &wk->sys->allocator, 0 );
			}
		}
		res_model_delete( &wk->m_disp.colum, &wk->sys->allocator, 2 );
		res_model_delete( &wk->m_disp.siru, &wk->sys->allocator, 2 );
		res_model_delete( &wk->m_disp.tama, &wk->sys->allocator, 2 );
		
		res_model_delete( &wk->m_disp.poke[ 0 ], &wk->sys->allocator, 1 );
		res_model_delete( &wk->m_disp.poke[ 1 ], &wk->sys->allocator, 1 );
		
		res_model_delete( &wk->m_disp.gira[ 0 ], &wk->sys->allocator, 2 );
		res_model_delete( &wk->m_disp.gira[ 1 ], &wk->sys->allocator, 2 );
		res_model_delete( &wk->m_disp.gira[ 2 ], &wk->sys->allocator, 2 );
		res_model_delete( &wk->m_disp.gira[ 3 ], &wk->sys->allocator, 2 );
		res_model_delete( &wk->m_disp.gira[ 4 ], &wk->sys->allocator, 2 );
		
		res_model_delete( &wk->m_disp.kage[ 0 ], &wk->sys->allocator, 1 );
		res_model_delete( &wk->m_disp.kage[ 1 ], &wk->sys->allocator, 0 );
		
		res_model_delete( &wk->m_disp.hobj[ 0 ], &wk->sys->allocator, 1 );
		res_model_delete( &wk->m_disp.hobj[ 1 ], &wk->sys->allocator, 1 );	
		
		wk->cut++;
		break;

	default:	
		sys_FreeMemoryEz( wk );	
		return FALSE;
	}
	
	return TRUE;	
}


//--------------------------------------------------------------
/**
 * @brief	3D 描画部分
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void Demo_3D_Draw( DEMO_SCENE_01_WORK* wk )
{
	GF_G3X_Reset();
	GFC_CameraLookAt();
	
	Demo_Light( wk->sys );
	Demo_Camera( wk->sys );
	
	D3D_Draw( &wk->m_disp.map );
	D3D_Draw( &wk->m_disp.colum );
	D3D_Draw( &wk->m_disp.siru );
	D3D_Draw( &wk->m_disp.tama );
	
	D3D_Draw( &wk->m_disp.kage[ 0 ] );
	D3D_Draw( &wk->m_disp.kage[ 1 ] );

	D3D_Draw( &wk->m_disp.gira[ 0 ] );
	D3D_Draw( &wk->m_disp.gira[ 1 ] );
	D3D_Draw( &wk->m_disp.gira[ 2 ] );
	D3D_Draw( &wk->m_disp.gira[ 3 ] );
	D3D_Draw( &wk->m_disp.gira[ 4 ] );
	
	D3D_Draw( &wk->m_disp.poke[ 0 ] );
	D3D_Draw( &wk->m_disp.poke[ 1 ] );
	
	D3D_Draw( &wk->m_disp.hobj[ 0 ] );
	D3D_Draw( &wk->m_disp.hobj[ 1 ] );
	
	D3D_Draw( &wk->m_disp.okage[ 0 ] );
	D3D_Draw( &wk->m_disp.okage[ 1 ] );
	D3D_Draw( &wk->m_disp.okage[ 2 ] );
	D3D_Draw( &wk->m_disp.okage[ 3 ] );
	
	GF_G3_RequestSwapBuffers( GX_SORTMODE_AUTO, GX_BUFFERMODE_W );
}
