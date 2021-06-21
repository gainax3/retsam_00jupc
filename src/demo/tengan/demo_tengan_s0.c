//==============================================================================
/**
 * @file	demo_tengan_s0.c
 * @brief	目覚めるシーン
 * @author	goto
 * @date	2007.12.05(水)
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
static void demo_scene00_ResourceLoad( DEMO_SCENE_00_WORK* wk );
static void demo_scene00_ResourceDelete( DEMO_SCENE_00_WORK* wk );
static void demo_scene00_Illuminant_Create( DEMO_SCENE_00_WORK* wk );
static void Demo_3D_Draw( DEMO_SCENE_00_WORK* wk );
static void Demo_CameraSet( GF_CAMERA_PTR camera, VecFx32* target );

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	main_wk	
 *
 * @retval	void*	
 *
 */
//--------------------------------------------------------------
void* Demo_Tengan_Scene00_Init( DEMO_TENGAN_WORK* main_wk )
{
	DEMO_SCENE_00_WORK* wk = sys_AllocMemory( HEAPID_DEMO_TENGAN, sizeof( DEMO_SCENE_00_WORK ) );
	memset( wk, 0, sizeof( DEMO_SCENE_00_WORK ) );
		
	wk->sys		= &main_wk->sys;
	wk->param	= main_wk->param;
	
	demo_scene00_ResourceLoad( wk );
	demo_scene00_Illuminant_Create( wk );
	
	{
		DEFF_Laster_Init( &wk->s_disp.laster, HEAPID_DEMO_TENGAN );		
		DEFF_Laster_Start( &wk->s_disp.laster, 0, 191, 
						  DEMO_LASTER_SIN_ADDR, 
						  FX32_CONST( 2 ), ( 1 * 100 ),
						  LASTER_SCROLL_SBG1, 0, 0x1000, 1 );
	}	
	{
		DEMO_MOTION_BL_DATA mb = {
			GX_DISPMODE_VRAM_C,
			GX_BGMODE_0,
			GX_BG0_AS_3D,			
			GX_CAPTURE_SIZE_256x192,
			GX_CAPTURE_MODE_AB,
			GX_CAPTURE_SRCA_3D,
			GX_CAPTURE_SRCB_VRAM_0x00000,
			GX_CAPTURE_DEST_VRAM_C_0x00000,
			4,
			12,
			HEAPID_DEMO_TENGAN
		};
	//	wk->sys->motion_bl = DEMO_MOTION_BL_Init( &mb );
	}

//	wk->sys->camera_target.z = FX32_CONST( 68 );
	Demo_CameraSet( wk->sys->camera_p, &wk->sys->camera_target );
	wk->sys->camera_target.y += FX32_CONST( 25 );
	
	G2S_BlendNone();
	G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2,
					  GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,	7, 8 );	
	
	Demo_MainDispSet( DISP_3D_TO_SUB );	

//	Snd_BgmPlay( SEQ_D_RYAYHY );
//	Snd_DataSetByScene( SND_SCENE_GIRA, SEQ_D_RYAYHY, 0 );	// サウンドデータロード
//	Snd_DataSetByScene( SND_SCENE_SUB_CLIMAX, 0, 0 );		// サウンドデータロード(BGM引継ぎ)
	Snd_BgmFadeOut( 0, 10 );
	return wk;
}

static u16 pers = 0x0a66;
BOOL  Demo_Tengan_Scene00_Main( void* work )
{
	enum {
		UMA_DEMO_IN_FADE_INIT = 0,
		UMA_DEMO_IN_FADE_WAIT,
		UMA_ANIME_INIT,
		UMA_ANIME_WAIT,
		UMA_FADE_OUT,
		UMA_FADE_IN,
		UMA_DEMO_END,
	};
	DEMO_SCENE_00_WORK* wk = work;
	
	static const u16 wait_tbl[] = {
		210, 120, 120,
	};
	
	static const u16 voice_wait_tbl[] = {
		145, 119, 100,
	};

	static const u16 se_wait_tbl[] = {
		//100, 21, 19,
		//100, 20, 19,
		100, 19, 18,
	};
	static const u32 monsno[] = {
		MONSNO_REI, MONSNO_AI, MONSNO_HAI,
	};

	switch( wk->cut ){
	case UMA_DEMO_IN_FADE_INIT:
		WIPE_SYS_Start(	WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN,	WIPE_TYPE_FADEIN, WIPE_FADE_BLACK,
						COMM_BRIGHTNESS_SYNC, 1, HEAPID_DEMO_TENGAN  );
		wk->cut++;
	
	case UMA_DEMO_IN_FADE_WAIT:
		if( WIPE_SYS_EndCheck() == FALSE ) { break; }
		
		Snd_DataSetByScene( SND_SCENE_GIRA, SEQ_D_RYAYHY, 0 );	// サウンドデータロード
		Snd_DataSetByScene( SND_SCENE_SUB_CLIMAX, 0, 0 );		// サウンドデータロード(BGM引継ぎ)
		wk->cut = UMA_ANIME_INIT;
		break;
		
	case UMA_ANIME_INIT:
		D3DOBJ_SetDraw( &wk->m_disp.uma[ wk->poke_type ].obj, TRUE );
		wk->m_disp.uma[ wk->poke_type ].bAnime = TRUE;
		wk->m_disp.uma[ wk->poke_type ].bColorAnime = TRUE;
		
		wk->cut++;
		wk->wait = 0;
		break;
		
	case UMA_ANIME_WAIT:
		// タイミング見てSE再生
		if( wk->wait == se_wait_tbl[ wk->poke_type ] ){
			Snd_SePlay( DS_APPEAR2 );
		}

		if ( wk->wait == voice_wait_tbl[ wk->poke_type ] ){
			Snd_PMVoicePlayEx( PV_NORMAL, monsno[ wk->poke_type ], 0, 100, HEAPID_DEMO_TENGAN, 0 );
		}

		if ( ( ++wk->wait ) >= wait_tbl[ wk->poke_type ] ){
			wk->sys->light_color.brightness = 0;
			
			wk->wait = 0;
			wk->cut++;
		}
		break;
		
	case UMA_FADE_OUT:
		if ( (++wk->sys->light_color.brightness) != DEMO_BRIGHTNESS_WHITE ){
			G2_SetBlendBrightness( DEMO_BLEND_MASK_M, wk->sys->light_color.brightness );
		}
		else {
			wk->s_disp.illum[ wk->poke_type ].bActive = TRUE;
			D3DOBJ_SetDraw( &wk->m_disp.uma[ wk->poke_type ].obj, FALSE );
			wk->cut++;
			
			//Snd_SePlay( DS_APPEAR );
			wk->poke_type++;
			if ( wk->poke_type >= 3 ){
				wk->sys->light_color.brightness = 0;
				wk->cut = UMA_DEMO_END;
			}
		}
		break;
		
	case UMA_FADE_IN:
		if ( (--wk->sys->light_color.brightness) > 0 ){
			G2_SetBlendBrightness( DEMO_BLEND_MASK_M, wk->sys->light_color.brightness );
		}
		else {
			wk->cut = UMA_ANIME_INIT;
		}
		break;
	
	case UMA_DEMO_END:
		if ( (++wk->wait) < 30 * 4 ){ break; }
		if ( (++wk->sys->light_color.brightness) != DEMO_BRIGHTNESS_WHITE ){
			G2S_SetBlendBrightness( DEMO_BLEND_MASK_S, wk->sys->light_color.brightness );
		}
		else {
			WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_WHITE, 1, 1, HEAPID_DEMO_TENGAN  );
			wk->cut = 0;
			return FALSE;
		}
	}
	
	Demo_3D_Draw( wk );
	BG_PaletteFade_Sub_UMA( wk->sys, wk->poke_type );

	return TRUE;
}

BOOL  Demo_Tengan_Scene00_Exit( void* work )
{
	DEMO_SCENE_00_WORK* wk = work;
	
	switch( wk->cut ){
	case 0:
		DEFF_Laster_Delete( &wk->s_disp.laster );
		demo_scene00_ResourceDelete( wk );
	//	DEMO_MOTION_BL_Delete( &wk->sys->motion_bl, GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D );
		wk->cut++;
		break;
		
	case 1:
		res_model_delete( &wk->m_disp.uma[ 0 ], &wk->sys->allocator, 2 );
		res_model_delete( &wk->m_disp.uma[ 1 ], &wk->sys->allocator, 2 );
		res_model_delete( &wk->m_disp.uma[ 2 ], &wk->sys->allocator, 2 );
		res_model_delete( &wk->m_disp.bg, &wk->sys->allocator, 0 );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
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
	CAMERA_ANGLE angle = { 1274, 0, 0 };
	
	GFC_InitCameraTDA( target, FX32_CONST( 200 ), &angle, 0x0a66, GF_CAMERA_PERSPECTIV, TRUE, camera );
	GFC_SetCameraView( GF_CAMERA_PERSPECTIV, camera );

	GFC_AttachCamera( camera );	
	
	GFC_SetCameraClip( FX32_CONST( 0.1 ), FX32_CONST( 2048 ), camera );
}


//--------------------------------------------------------------
/**
 * @brief	描画部分
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void Demo_3D_Draw( DEMO_SCENE_00_WORK* wk )
{
	GF_G3X_Reset();
	GFC_CameraLookAt();
	
	Demo_Light( wk->sys );
	Demo_Camera( wk->sys );
	
	D3D_Draw( &wk->m_disp.uma[ 0 ] );
	D3D_Draw( &wk->m_disp.uma[ 1 ] );
	D3D_Draw( &wk->m_disp.uma[ 2 ] );
	D3D_Draw( &wk->m_disp.bg );
	
	GF_G3_RequestSwapBuffers( GX_SORTMODE_AUTO, GX_BUFFERMODE_W );	
}


//--------------------------------------------------------------
/**
 * @brief	リソース読み込み
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void demo_scene00_ResourceLoad( DEMO_SCENE_00_WORK* wk )
{
	ARCHANDLE*		 hdl = wk->sys->p_handle;
	GF_BGL_INI*		 bgl = wk->sys->bgl;
	CATS_SYS_PTR	 csp = wk->sys->csp;
	CATS_RES_PTR	 crp = wk->sys->crp;
	PALETTE_FADE_PTR pfd = wk->sys->pfd;
	int oam_id = eOAM_ID_ILLUMINANT;

	ArcUtil_HDL_BgCharSet( hdl, NARC_demo_tengan_gra_uma_demo_bg_NCGR, bgl, GF_BGL_FRAME1_S, 0, 0, 0, HEAPID_DEMO_TENGAN );
	ArcUtil_HDL_ScrnSet( hdl, NARC_demo_tengan_gra_uma_demo_bg1_NSCR, bgl, GF_BGL_FRAME1_S,  0, 0, 0, HEAPID_DEMO_TENGAN );
	ArcUtil_HDL_ScrnSet( hdl, NARC_demo_tengan_gra_uma_demo_bg2_NSCR, bgl, GF_BGL_FRAME0_S,  0, 0, 0, HEAPID_DEMO_TENGAN );
	PaletteWorkSet_Arc( pfd, ARC_DEMO_TENGAN_GRA, NARC_demo_tengan_gra_uma_demo_bg_NCLR, HEAPID_DEMO_TENGAN, FADE_SUB_BG, 0x20 * 2, 0 );
		
	{
		const u16 black_ncl[] = { 0x0421 };
		PaletteWorkSet( pfd, &black_ncl, FADE_MAIN_BG, 0, 0x02 );
	}

	CATS_LoadResourcePlttWorkArcH( pfd, FADE_SUB_OBJ,  csp, crp, hdl, NARC_demo_tengan_gra_g_demo_oam_NCLR, FALSE, 3, NNS_G2D_VRAM_TYPE_2DSUB,  oam_id );
	CATS_LoadResourceCellArcH( csp, crp, hdl, NARC_demo_tengan_gra_g_demo_oam_NCER, FALSE, oam_id );
	CATS_LoadResourceCellAnmArcH( csp, crp, hdl, NARC_demo_tengan_gra_g_demo_oam_NANR, FALSE, oam_id );
	CATS_LoadResourceCharArcH( csp, crp, hdl, NARC_demo_tengan_gra_g_demo_oam_NCGR, FALSE, NNS_G2D_VRAM_TYPE_2DSUB,  oam_id );
	
	res_load_init( &wk->m_disp.bg, NARC_demo_tengan_gra_g_uma_bg_nsbmd, wk->sys->p_handle );

	res_load_init( 	   &wk->m_disp.uma[ 0 ], NARC_demo_tengan_gra_g_demo_u_nsbmd, wk->sys->p_handle );
	res_anime_init( 0, &wk->m_disp.uma[ 0 ], NARC_demo_tengan_gra_g_demo_u_nsbca, wk->sys->p_handle, &wk->sys->allocator );
	res_anime_init( 1, &wk->m_disp.uma[ 0 ], NARC_demo_tengan_gra_g_demo_u_nsbma, wk->sys->p_handle, &wk->sys->allocator );

	res_load_init( 	   &wk->m_disp.uma[ 1 ], NARC_demo_tengan_gra_g_demo_m_nsbmd, wk->sys->p_handle );
	res_anime_init( 0, &wk->m_disp.uma[ 1 ], NARC_demo_tengan_gra_g_demo_m_nsbca, wk->sys->p_handle, &wk->sys->allocator );
	res_anime_init( 1, &wk->m_disp.uma[ 1 ], NARC_demo_tengan_gra_g_demo_m_nsbma, wk->sys->p_handle, &wk->sys->allocator );
	
	res_load_init( 	   &wk->m_disp.uma[ 2 ], NARC_demo_tengan_gra_g_demo_a_nsbmd, wk->sys->p_handle );
	res_anime_init( 0, &wk->m_disp.uma[ 2 ], NARC_demo_tengan_gra_g_demo_a_nsbca, wk->sys->p_handle, &wk->sys->allocator );
	res_anime_init( 1, &wk->m_disp.uma[ 2 ], NARC_demo_tengan_gra_g_demo_a_nsbma, wk->sys->p_handle, &wk->sys->allocator );
	
	
	D3DOBJ_SetDraw( &wk->m_disp.uma[ 0 ].obj, TRUE );
	D3DOBJ_SetDraw( &wk->m_disp.uma[ 1 ].obj, FALSE );
	D3DOBJ_SetDraw( &wk->m_disp.uma[ 2 ].obj, FALSE );
#if 0	
	res_load_init( 	   &wk->m_disp.uma[ 2 ], NARC_demo_tengan_gra_g_demo_a_nsbmd, wk->sys->p_handle );
	res_anime_init( 0, &wk->m_disp.uma[ 2 ], NARC_demo_tengan_gra_g_demo_a_nsbca, wk->sys->p_handle, &wk->sys->allocator );
	res_anime_init( 1, &wk->m_disp.uma[ 2 ], NARC_demo_tengan_gra_g_demo_a_nsbma, wk->sys->p_handle, &wk->sys->allocator );
#endif	
//	wk->m_disp.uma[ 0 ].bAnime = TRUE;
//	wk->m_disp.uma[ 0 ].bColorAnime = TRUE;
}


//--------------------------------------------------------------
/**
 * @brief	リソース破棄
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void demo_scene00_ResourceDelete( DEMO_SCENE_00_WORK* wk )
{
	int i;
	
	for ( i = 0; i < ILLUMINANT_MAX; i++ ){
		
		TCB_Delete( wk->s_disp.illum[ i ].tcb );
		TCB_Delete( wk->s_disp.illum_dmy[ i ].tcb );
		CATS_ActorPointerDelete_S( wk->s_disp.illum[ i ].cap );
		CATS_ActorPointerDelete_S( wk->s_disp.illum_dmy[ i ].cap );
	}	
}

static void demo_scene00_Illuminant_Create( DEMO_SCENE_00_WORK* wk )
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
	coap.bg_pri = 2;
	coap.vram_trans = 0;
	
	coap.id[4] = CLACT_U_HEADER_DATA_NONE;
	coap.id[5] = CLACT_U_HEADER_DATA_NONE;
	
	coap.id[0] = eOAM_ID_ILLUMINANT;
	coap.id[1] = eOAM_ID_ILLUMINANT;
	coap.id[2] = eOAM_ID_ILLUMINANT;
	coap.id[3] = eOAM_ID_ILLUMINANT;
	
	for ( i = 0; i < ILLUMINANT_MAX; i++ ){
		
		wk->s_disp.illum_dmy[ i ].cap		= CATS_ObjectAdd_S( csp, crp, &coap );

		CATS_ObjectUpdateCap( wk->s_disp.illum_dmy[ i ].cap );
	#if 0
		CATS_ObjectAffineSetCap( wk->s_disp.illum_dmy[ i ].cap, CLACT_AFFINE_DOUBLE );
		CATS_ObjectScaleSetCap( wk->s_disp.illum_dmy[ i ].cap, 0.1f, 0.1f );		
	#endif
		CATS_ObjectEnableCap( wk->s_disp.illum_dmy[ i ].cap, FALSE );
		CATS_ObjectPaletteOffsetSetCap( wk->s_disp.illum_dmy[ i ].cap, i );		
		CATS_ObjectPosSetCap( wk->s_disp.illum_dmy[ i ].cap, 0, 0 );
		
//		CATS_ObjectObjModeSetCap( wk->s_disp.illum_dmy[ i ].cap, GX_OAM_MODE_XLU );
		
		wk->s_disp.illum_dmy[ i ].ai  = 1;
		wk->s_disp.illum_dmy[ i ].id  = i;
		wk->s_disp.illum_dmy[ i ].z	  = 50;
		wk->s_disp.illum_dmy[ i ].rad = i * 120;
		wk->s_disp.illum_dmy[ i ].rad2= wk->s_disp.illum_dmy[ i ].rad;
		wk->s_disp.illum_dmy[ i ].count = 0;
		wk->s_disp.illum_dmy[ i ].para[0] = 1;
		wk->s_disp.illum_dmy[ i ].para[1] = 1;
		wk->s_disp.illum_dmy[ i ].para[2] = gf_rand() % 10;
		wk->s_disp.illum_dmy[ i ].para[3] = 0;
		wk->s_disp.illum_dmy[ i ].tcb = TCB_Add( Illuminant_S0_AI_TCB, &wk->s_disp.illum_dmy[ i ], eTCB_PRI_ILLUM_AI - 1 );
	}
	
	for ( i = 0; i < ILLUMINANT_MAX; i++ ){
		s16 pos_x[] = { 180, -20, 280 };
		s16 pos_y[] = { 128, 190, 150 };
		f32 scale[] = { 0.1f, 0.3f, 0.2f };
		
		if ( i == 1 ){
			coap.bg_pri = 0;
		}
		else {
			coap.bg_pri = 2;
		}
			
		
		wk->s_disp.illum[ i ].cap		= CATS_ObjectAdd_S( csp, crp, &coap );
		wk->s_disp.illum[ i ].cap_dmy	= wk->s_disp.illum_dmy[ i ].cap;
		
		CATS_ObjectUpdateCap( wk->s_disp.illum[ i ].cap );
		CATS_ObjectAffineSetCap( wk->s_disp.illum[ i ].cap, CLACT_AFFINE_DOUBLE );
		CATS_ObjectScaleSetCap( wk->s_disp.illum[ i ].cap, scale[ i ], scale[ i ] );
		CATS_ObjectPaletteOffsetSetCap( wk->s_disp.illum[ i ].cap, i );		
		CATS_ObjectPosSetCap( wk->s_disp.illum[ i ].cap, pos_x[ i ], pos_y[ i ] );

//		CATS_ObjectObjModeSetCap( wk->s_disp.illum[ i ].cap, GX_OAM_MODE_XLU );

		wk->s_disp.illum[ i ].ai  = 0;
		wk->s_disp.illum[ i ].id  = i;
		wk->s_disp.illum[ i ].z	  = 50;
		wk->s_disp.illum[ i ].rad = i * 120;
		wk->s_disp.illum[ i ].rad2= wk->s_disp.illum[ i ].rad;
		wk->s_disp.illum[ i ].count = 0;
		wk->s_disp.illum[ i ].para[0] = 1;
		wk->s_disp.illum[ i ].para[1] = 1;
		wk->s_disp.illum[ i ].para[2] = gf_rand() % 10;
		wk->s_disp.illum[ i ].para[3] = 0;
		wk->s_disp.illum[ i ].tcb = TCB_Add( Illuminant_S0_AI_TCB, &wk->s_disp.illum[ i ], eTCB_PRI_ILLUM_AI );
	}
	
}

