//==============================================================================
/**
 * @file	demo_tengan_s2.c
 * @brief	2匹を償還する場面
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

#include "src/field/poketch/poketch_arc.h"

#include "demo_tengan_common.h"

//==============================================================
// Prototype
//==============================================================
static void demo_scene02_ResourceLoad( DEMO_SCENE_02_WORK* wk );
static void demo_scene02_ResourceDelete( DEMO_SCENE_02_WORK* wk );
static void demo_scene02_ResourceDelete_Tama( DEMO_SCENE_02_WORK* wk );
static void Demo_CameraSet( GF_CAMERA_PTR camera, VecFx32* target );
static void Demo_3D_Draw( DEMO_SCENE_02_WORK* wk );
extern void* Demo_Tengan_Scene02_Init( DEMO_TENGAN_WORK* main_wk );
extern BOOL Demo_Tengan_Scene02_Main( void* work );
extern BOOL Demo_Tengan_Scene02_Exit( void* work );

enum {
	
	eS2_FADE_WAIT,
	eS2_CAMERA_INIT,
	eS2_CAMERA_MOVE,
	eS2_MSG_DIA_PAL,
	eS2_DIA_ACTIVE,
	eS2_PAL_ACTIVE,		
	eS2_DIA_ON,
	eS2_PAL_ON,
	
	eS2_GINGA_BLANK,
	eS2_GINGA_FLASH_1,
	eS2_GINGA_FLASH_2,
	eS2_GINGA_INIT,	
	eS2_GINGA_ON,
	eS2_GINGA_WAIT,
	eS2_GINGA_END,
	
	eS2_DEMO_WIPE_OUT,
	eS2_DEMO_WIPE_WAIT,
	eS2_DEMO_END,		
};

static void SubBG_ResourceLoad( GF_BGL_INI* bgl, PALETTE_FADE_PTR pfd )
{
	int arc_index = ARC_POKETCH_IMG;
	int chr_index = NARC_poketch_before_lz_ncgr;
	int scr_index = NARC_poketch_before_lz_nscr;
	int pal_index = NARC_poketch_before_nclr;
	int frame	  = GF_BGL_FRAME0_S;
	int heap_id	  = HEAPID_DEMO_TENGAN;
	
	ArcUtil_BgCharSet(arc_index, chr_index, bgl, frame, 0, 0, 1, heap_id);					  
	ArcUtil_ScrnSet(arc_index, scr_index, bgl, frame, 0, 0, 1, heap_id);					
	PaletteWorkSet_Arc(pfd, arc_index, pal_index, heap_id, FADE_SUB_BG, 0x20 * 1, 0 );
}


///< 銀河読み込み
static void demo_scene02_ResourceLoad_Ginga( DEMO_SCENE_02_WORK* wk )
{
	ARCHANDLE*		 hdl = wk->sys->p_handle;
	GF_BGL_INI*		 bgl = wk->sys->bgl;
	CATS_SYS_PTR	 csp = wk->sys->csp;
	CATS_RES_PTR	 crp = wk->sys->crp;
	PALETTE_FADE_PTR pfd = wk->sys->pfd;

	res_load_init( &wk->m_disp.gira[ 0 ], NARC_demo_tengan_gra_ginga_nsbmd, wk->sys->p_handle );
	res_anime_init( 0, &wk->m_disp.gira[ 0 ], NARC_demo_tengan_gra_ginga_nsbca, wk->sys->p_handle, &wk->sys->allocator );
	res_anime_init( 1, &wk->m_disp.gira[ 0 ], NARC_demo_tengan_gra_ginga_nsbma, wk->sys->p_handle, &wk->sys->allocator );
	D3DOBJ_SetMatrix( &wk->m_disp.gira[ 0 ].obj, FX32_CONST( -48 ), FX32_CONST( -5 ), FX32_CONST( -70 ) );
	wk->m_disp.gira[ 0 ].bAnime = FALSE;

	res_load_init_Ex( &wk->m_disp.gira[ 0 ], &wk->m_disp.gira[ 1 ], NARC_demo_tengan_gra_ginga_nsbmd, wk->sys->p_handle );
	res_anime_init_Ex( 0, &wk->m_disp.gira[ 0 ], &wk->m_disp.gira[ 1 ], NARC_demo_tengan_gra_ginga_nsbca, wk->sys->p_handle, &wk->sys->allocator );
	res_anime_init_Ex( 1, &wk->m_disp.gira[ 0 ], &wk->m_disp.gira[ 1 ], NARC_demo_tengan_gra_ginga_nsbma, wk->sys->p_handle, &wk->sys->allocator );
	D3DOBJ_SetMatrix( &wk->m_disp.gira[ 1 ].obj, FX32_CONST( +48 ), FX32_CONST( -5 ), FX32_CONST( -70 ) );
	wk->m_disp.gira[ 1 ].bAnime = FALSE;
	
	wk->m_disp.gira[ 0 ].obj_scale = FX32_CONST( 0.1 );
	wk->m_disp.gira[ 1 ].obj_scale = FX32_CONST( 0.1 );

	D3DOBJ_SetScale( &wk->m_disp.gira[ 0 ].obj, wk->m_disp.gira[ 0 ].obj_scale, FX32_CONST( 1.0 ), wk->m_disp.gira[ 0 ].obj_scale );
	D3DOBJ_SetScale( &wk->m_disp.gira[ 1 ].obj, wk->m_disp.gira[ 1 ].obj_scale, FX32_CONST( 1.0 ), wk->m_disp.gira[ 1 ].obj_scale );
}

static void demo_scene02_ResourceLoad( DEMO_SCENE_02_WORK* wk )
{
	ARCHANDLE*		 hdl = wk->sys->p_handle;
	GF_BGL_INI*		 bgl = wk->sys->bgl;
	CATS_SYS_PTR	 csp = wk->sys->csp;
	CATS_RES_PTR	 crp = wk->sys->crp;
	PALETTE_FADE_PTR pfd = wk->sys->pfd;

	///< 3D
	
	///< マップと柱は同時に使う
	res_load_init( &wk->m_disp.map, NARC_demo_tengan_gra_g_demo_map_nsbmd, wk->sys->p_handle );
	res_load_init( &wk->m_disp.colum, NARC_demo_tengan_gra_g_demo_colum_01_nsbmd, wk->sys->p_handle );
	res_anime_init( 0, &wk->m_disp.colum, NARC_demo_tengan_gra_g_demo_colum_01_nsbca, wk->sys->p_handle, &wk->sys->allocator );
	res_anime_init( 1, &wk->m_disp.colum, NARC_demo_tengan_gra_g_demo_colum_01_nsbma, wk->sys->p_handle, &wk->sys->allocator );

	///< たま
	res_load_init( &wk->m_disp.kage[ 0 ], NARC_demo_tengan_gra_kurotama_nsbmd, wk->sys->p_handle );
	res_anime_init( 0, &wk->m_disp.kage[ 0 ], NARC_demo_tengan_gra_kurotama_nsbca, wk->sys->p_handle, &wk->sys->allocator );
	res_anime_init( 1, &wk->m_disp.kage[ 0 ], NARC_demo_tengan_gra_kurotama_nsbma, wk->sys->p_handle, &wk->sys->allocator );
	res_anime_init( 2, &wk->m_disp.kage[ 0 ], NARC_demo_tengan_gra_kurotama_nsbta, wk->sys->p_handle, &wk->sys->allocator );
	res_anime_init( 3, &wk->m_disp.kage[ 0 ], NARC_demo_tengan_gra_kurotama_nsbva, wk->sys->p_handle, &wk->sys->allocator );
	D3DOBJ_SetMatrix( &wk->m_disp.kage[ 0 ].obj, FX32_CONST( -48 ), FX32_CONST( -10 ), FX32_CONST( -70 ) );
	wk->m_disp.kage[ 0 ].bAnime = FALSE;
	wk->m_disp.kage[ 0 ].bFullAnime = FALSE;
	
	res_load_init( &wk->m_disp.kage[ 1 ], NARC_demo_tengan_gra_kurotama_nsbmd, wk->sys->p_handle );
	res_anime_init( 0, &wk->m_disp.kage[ 1 ], NARC_demo_tengan_gra_kurotama_nsbca, wk->sys->p_handle, &wk->sys->allocator );
	res_anime_init( 1, &wk->m_disp.kage[ 1 ], NARC_demo_tengan_gra_kurotama_nsbma, wk->sys->p_handle, &wk->sys->allocator );
	res_anime_init( 2, &wk->m_disp.kage[ 1 ], NARC_demo_tengan_gra_kurotama_nsbta, wk->sys->p_handle, &wk->sys->allocator );
	res_anime_init( 3, &wk->m_disp.kage[ 1 ], NARC_demo_tengan_gra_kurotama_nsbva, wk->sys->p_handle, &wk->sys->allocator );
	D3DOBJ_SetMatrix( &wk->m_disp.kage[ 1 ].obj, FX32_CONST( +48 ), FX32_CONST( -10 ), FX32_CONST( -70 ) );
	wk->m_disp.kage[ 1 ].bAnime = FALSE;
	wk->m_disp.kage[ 1 ].bFullAnime = FALSE;	
	
	///< okage
	{
		int i;
		
		for ( i = 0; i < 4; i++ ){
			res_load_init( &wk->m_disp.okage[ i ], NARC_demo_tengan_gra_kage_nsbmd, wk->sys->p_handle );
				
			D3DOBJ_SetScale( &wk->m_disp.okage[ i ].obj, FX32_CONST( 1.2 ), FX32_CONST( 1.0 ), FX32_CONST( 1.2 ) );
		}
	}
	
	
	///< dia
	res_load_init( &wk->m_disp.poke[ 0 ], NARC_demo_tengan_gra_sppoke4_nsbmd, wk->sys->p_handle );
	D3DOBJ_SetMatrix( &wk->m_disp.poke[ 0 ].obj, FX32_CONST( -50 ), FX32_CONST( +0 ), FX32_CONST( -50 ) );
	res_anime_init( 0, &wk->m_disp.poke[ 0 ], NARC_demo_tengan_gra_sppoke4_nsbtp, wk->sys->p_handle, &wk->sys->allocator );
	wk->m_disp.poke[ 0 ].bAnime = TRUE;
	wk->m_disp.poke[ 0 ].bAnimeLoop = TRUE;
	wk->m_disp.poke[ 0 ].anm_speed = FX32_HALF;
	D3DOBJ_SetDraw( &wk->m_disp.poke[ 0 ].obj, FALSE );
	D3DOBJ_SetDraw( &wk->m_disp.okage[ 0 ].obj, FALSE );
	
	///< pal
	res_load_init( &wk->m_disp.poke[ 1 ], NARC_demo_tengan_gra_sppoke5_nsbmd, wk->sys->p_handle );
	D3DOBJ_SetMatrix( &wk->m_disp.poke[ 1 ].obj, FX32_CONST( +50 ), FX32_CONST( +0 ), FX32_CONST( -50 ) );
	res_anime_init( 0, &wk->m_disp.poke[ 1 ], NARC_demo_tengan_gra_sppoke5_nsbtp, wk->sys->p_handle, &wk->sys->allocator );
	wk->m_disp.poke[ 1 ].bAnime = TRUE;
	wk->m_disp.poke[ 1 ].bAnimeLoop = TRUE;
	wk->m_disp.poke[ 1 ].anm_speed = FX32_HALF;
	D3DOBJ_SetDraw( &wk->m_disp.poke[ 1 ].obj, FALSE );
	D3DOBJ_SetDraw( &wk->m_disp.okage[ 1 ].obj, FALSE );
	
	
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

static void demo_scene02_ResourceDelete( DEMO_SCENE_02_WORK* wk )
{
	res_model_delete( &wk->m_disp.map, &wk->sys->allocator, 0 );	
	{
		int i;
		
		for ( i = 0; i < 4; i++ ){
			res_model_delete( &wk->m_disp.okage[ i ], &wk->sys->allocator, 0 );
		}
	}
	res_model_delete( &wk->m_disp.colum, &wk->sys->allocator, 2 );
	
	res_model_delete( &wk->m_disp.poke[ 0 ], &wk->sys->allocator, 1 );
	res_model_delete( &wk->m_disp.poke[ 1 ], &wk->sys->allocator, 1 );
	
	res_model_delete( &wk->m_disp.gira[ 0 ], &wk->sys->allocator, 2 );
	res_model_delete( &wk->m_disp.gira[ 1 ], &wk->sys->allocator, 2 );
	
	res_model_delete( &wk->m_disp.hobj[ 0 ], &wk->sys->allocator, 1 );
	res_model_delete( &wk->m_disp.hobj[ 1 ], &wk->sys->allocator, 1 );		
}

static void demo_scene02_ResourceDelete_Tama( DEMO_SCENE_02_WORK* wk )
{	
	res_model_delete( &wk->m_disp.kage[ 0 ], &wk->sys->allocator, 4 );
	res_model_delete( &wk->m_disp.kage[ 1 ], &wk->sys->allocator, 4 );

	wk->m_disp.kage[ 0 ].bInit = FALSE;
	wk->m_disp.kage[ 1 ].bInit = FALSE;
}

static void Demo_CameraSet( GF_CAMERA_PTR camera, VecFx32* target )
{
	CAMERA_ANGLE angle = { -0x29fe, 0, 0 };
	
	GFC_InitCameraTDA( target, 0x13c805, &angle, 0x0c01, GF_CAMERA_PERSPECTIV, TRUE, camera );

	GFC_AttachCamera( camera );	
	
	GFC_SetCameraClip( FX32_ONE * 10, FX32_ONE * 1008, camera );
}

static void Demo_3D_Draw( DEMO_SCENE_02_WORK* wk )
{
	GF_G3X_Reset();
	GFC_CameraLookAt();
	
	Demo_Light( wk->sys );
	Demo_Camera( wk->sys );
	
	D3D_Draw( &wk->m_disp.map );
	D3D_Draw( &wk->m_disp.colum );
		
	D3D_Draw( &wk->m_disp.poke[ 0 ] );
	D3D_Draw( &wk->m_disp.poke[ 1 ] );
	
	D3D_Draw( &wk->m_disp.kage[ 0 ] );
	D3D_Draw( &wk->m_disp.kage[ 1 ] );
	
	D3D_Draw( &wk->m_disp.gira[ 0 ] );
	D3D_Draw( &wk->m_disp.gira[ 1 ] );
	
	D3D_Draw( &wk->m_disp.hobj[ 0 ] );
	D3D_Draw( &wk->m_disp.hobj[ 1 ] );
	
	D3D_Draw( &wk->m_disp.okage[ 0 ] );
	D3D_Draw( &wk->m_disp.okage[ 1 ] );
	D3D_Draw( &wk->m_disp.okage[ 2 ] );
	D3D_Draw( &wk->m_disp.okage[ 3 ] );
	
	GF_G3_RequestSwapBuffers( GX_SORTMODE_AUTO, GX_BUFFERMODE_W );
}

void* Demo_Tengan_Scene02_Init( DEMO_TENGAN_WORK* main_wk )
{
	DEMO_SCENE_02_WORK* wk = sys_AllocMemory( HEAPID_DEMO_TENGAN, sizeof( DEMO_SCENE_02_WORK ) );
	
	memset( wk, 0, sizeof( DEMO_SCENE_02_WORK ) );
	
	wk->sys		= &main_wk->sys;
	wk->param	= main_wk->param;

	demo_scene02_ResourceLoad( wk );
	SubBG_ResourceLoad( wk->sys->bgl, wk->sys->pfd );

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
	wk->sys->camera_target.z = FX32_CONST( 34 );
	Demo_CameraSet( wk->sys->camera_p, &wk->sys->camera_target );
	
	Snd_DataSetByScene( SND_SCENE_SUB_CLIMAX, 0, 0 );	//サウンドデータロード(BGM引継ぎ)
		
	return wk;
}

static void ShakeTCB( TCB_PTR tcb, void* work )
{
	SHAKE* wk = work;
	
	switch ( wk->seq ){
	case 0:
		wk->cw->set_frame = 2;
		if ( wk->cnt ){
			if ( wk->main_seq == eS2_DIA_ACTIVE		 	///< ディアルガ出るとき
			||	 wk->main_seq == eS2_DIA_ON ){			///< ディアルガ出た後
				wk->cw->mx = +FX32_CONST( 2 );
			}
			else if ( wk->main_seq == eS2_PAL_ACTIVE ){	///< パルキア出るとき
				wk->cw->mx = +FX32_CONST( 4 );
			}
			else if ( wk->main_seq == 0xFF ){
				wk->cw->mx = +FX32_CONST( 6 );
			}
			else if ( wk->main_seq == eS2_PAL_ON ){		///< パルキア出た後
				wk->cw->mx = +FX32_CONST( 2 );
				wk->cw->set_frame = 4;
			}
		}
		else {
			if ( wk->main_seq == eS2_DIA_ACTIVE 
			||	 wk->main_seq == eS2_DIA_ON ){
				wk->cw->mx = -FX32_CONST( 2 );
			}
			else if ( wk->main_seq == eS2_PAL_ACTIVE ){
				wk->cw->mx = -FX32_CONST( 4 );
			}
			else if ( wk->main_seq == 0xFF ){
				wk->cw->mx = -FX32_CONST( 6 );
			}
			else if ( wk->main_seq == eS2_PAL_ON ){
				wk->cw->mx = -FX32_CONST( 2 );
				wk->cw->set_frame = 4;
			}
		}
		wk->cnt ^= 1;
		wk->cw->my = 0;
		wk->cw->mz = 0;
		Camera_Work_Init( wk->cw );
		wk->seq++;

	case 1:
		if ( Camera_Work_Main( wk->cw ) ){
			if ( wk->main_seq == eS2_GINGA_BLANK ){
				wk->seq++;
			}
			else {
				wk->seq--;
			}
		}
		break;

	default:
		TCB_Delete( tcb );
		break;
	}
}

static void ShakeInit( DEMO_SCENE_02_WORK* wk )
{
	wk->sys->cw.x = 0;
	wk->sys->cw.y = 0;
	wk->sys->cw.z = 0;	
	wk->sys->cw.camera_target = &wk->sys->camera_target;

	wk->sys->shake.cw				= &wk->sys->cw;
	wk->sys->shake.main_seq		 	= 0;

	TCB_Add( ShakeTCB, &wk->sys->shake, 0x1000 );
}

static void PokeOn_TCB( TCB_PTR tcb, void* work )
{
	POKE_ON* wk = work;
	
	f32 scale[] = {
		0.0f,0.3f,0.6f,1.0f,1.2f,1.1f,1.0f,
	};
	
	switch ( wk->seq ){
	case 0:
		D3DOBJ_SetScale( &wk->poke->obj, FX32_CONST( scale[ wk->wait ] ), FX32_CONST(1.00f), FX32_CONST(1.00f) );
		D3DOBJ_SetScale( &wk->okage->obj, FX32_CONST( scale[ wk->wait ] ), FX32_CONST(1.00f), FX32_CONST(1.00f) );
		wk->wait++;
	//	Snd_PMVoicePlayEx( PV_NORMAL, wk->monsno, wk->pan_x, 40, HEAPID_DEMO_TENGAN, 0 );
		D3DOBJ_SetDraw( &wk->poke->obj, TRUE );
		D3DOBJ_SetDraw( &wk->okage->obj, TRUE );
	//	D3DOBJ_SetDraw( &wk->okage->obj, TRUE );
		wk->seq++;
		break;
	
	case 1:
		if ( (++wk->wait) >= NELEMS( scale ) ){
			Snd_PMVoicePlayEx( PV_NORMAL, wk->monsno, wk->pan_x, 80, HEAPID_DEMO_TENGAN, 0 );
			wk->seq++;
		}
		else {
			D3DOBJ_SetScale( &wk->poke->obj, FX32_CONST( scale[ wk->wait ] ), FX32_CONST(1.00f), FX32_CONST(1.00f) );
			D3DOBJ_SetScale( &wk->okage->obj, FX32_CONST( scale[ wk->wait ] ), FX32_CONST(1.00f), FX32_CONST(1.00f) );
		}		
		break;
	
	default:
		TCB_Delete( tcb );
		break;
	}
}

static void PokeOn_Init( DEMO_SCENE_02_WORK* wk, int no, int monsno )
{
	POKE_ON* sub_wk = &wk->pokeon[ no ];

	sub_wk->seq		= 0;
	sub_wk->wait	= 0;
	sub_wk->monsno	= monsno;
	sub_wk->poke	= &wk->m_disp.poke[ no ];
	sub_wk->okage	= &wk->m_disp.okage[ no ];
	
	if ( no == 0 ){
		sub_wk->pan_x = -80;
	}
	else {
		sub_wk->pan_x = +80;
	}
	
	TCB_Add( PokeOn_TCB, sub_wk, 0x1000 );
}

BOOL Demo_Tengan_Scene02_Main( void* work )
{
	DEMO_SCENE_02_WORK* wk = work;
	
	switch ( wk->cut ){
	case eS2_FADE_WAIT:
		if ( WIPE_SYS_EndCheck() == FALSE ){ break; }
		wk->cut++;
		
	case eS2_CAMERA_INIT:
		///< カメラを移動させる
		{
			wk->sys->cw.x = 0;
			wk->sys->cw.y = 0;
			wk->sys->cw.z = 0;
			wk->sys->cw.set_frame = 60;
			wk->sys->cw.camera_p = wk->sys->camera_p;
			
			wk->sys->cw.mx = 0;
			wk->sys->cw.my = 0;
			wk->sys->cw.mz = -FX32_CONST( 80-34 );
			wk->sys->cw.camera_target = &wk->sys->camera_target;
			Camera_Work_Init( &wk->sys->cw );
			wk->cut++;		
		}
	
	case eS2_CAMERA_MOVE:
		///< 移動待ち
		if ( Camera_Work_Main( &wk->sys->cw ) ){
			wk->wait = 0;
			wk->cut++;
			Demo_MSG_Set( wk->sys, wk->param, msg_d05r0114_gingaboss_02 );
		}
		break;
	
	case eS2_MSG_DIA_PAL:
		if ( GF_MSG_PrintEndCheck( wk->sys->msg_index ) ){ break; }
		Demo_MSG_Off( wk->sys );
		Demo_MSG_Set( wk->sys, wk->param, msg_d05r0114_gingaboss_03 );
		wk->cut++;
		
	case eS2_DIA_ACTIVE:
		///< ディアルガスタート
		if ( GF_MSG_PrintEndCheck( wk->sys->msg_index ) ){ break; }
		#if 1	///< 仮
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
		ShakeInit( wk );
		Demo_MSG_Off( wk->sys );
		wk->m_disp.kage[ 0 ].bAnime = TRUE;
		wk->m_disp.kage[ 0 ].bFullAnime = TRUE;
		wk->cut++;

	case eS2_PAL_ACTIVE:
		///< パルキアスタート 2秒後
		if ( ( ++wk->wait ) >= 60 ){
			wk->sys->shake.main_seq = eS2_DIA_ACTIVE;
			wk->m_disp.kage[ 1 ].bAnime = TRUE;
			wk->m_disp.kage[ 1 ].bFullAnime = TRUE;		
			wk->cut++;
		}
		break;
				
	case eS2_DIA_ON:
		///< アニメ終わったら登場
	#ifdef PM_DEBUG
		if ( sys.trg & PAD_BUTTON_A ){
			OS_Printf( "wait %d\n", wk->wait );
		}
	#endif
		wk->wait++;
		if ( wk->wait ==   80 ){			///< ディアの玉が落ちてくる
			Snd_SePlayPan( DS_EFF_01, -70 );
		}
		if ( wk->wait ==  135 ){			///< パルキアの玉が落ちてくる
			Snd_SePlayPan( DS_EFF_01, +70 );
		}
		if ( wk->wait == 310 || wk->wait == 375 || wk->wait == 432 ){
			Snd_SePlay( DS_EFF_03 );
		}
		if ( wk->wait == 284 || wk->wait == 338 || wk->wait == 406 ){
			Snd_SePlay( DS_EFF_04 );
		}
		if ( wk->wait == 165 ){			///< 玉がでかくなる　ディア
			Snd_SePlayPan( DS_EFF_02, -70 );	
		}
		if ( wk->wait == 220 ){			///< 玉がでかくなる　パル
			Snd_SePlayPan( DS_EFF_02, +70 );	
		}
		if ( wk->wait == 470 ){			///< 線が入る　ディア
			Snd_SePlayPan( DS_EFF_05, -70 );	
		}
		if ( wk->wait == 520 ){			///< 線が入る　パル
			Snd_SePlayPan( DS_EFF_05, +70 );	
		}
		if ( wk->wait == 120 ){
			wk->sys->shake.main_seq = eS2_PAL_ACTIVE;
		}
		if ( wk->wait == 210 ){
			wk->sys->shake.main_seq = 0xFF;
		}
		if ( wk->m_disp.kage[ 0 ].bAnime == FALSE ){
			wk->sys->shake.main_seq = eS2_DIA_ON;
			PokeOn_Init( wk, 0, MONSNO_IA );
			Snd_SePlayPan( DS_EFF_06, -70 );
//			Snd_PMVoicePlayEx( PV_NORMAL, MONSNO_IA, -80, 40, HEAPID_DEMO_TENGAN, 0 );
//			D3DOBJ_SetDraw( &wk->m_disp.poke[ 0 ].obj, TRUE );
//			D3DOBJ_SetDraw( &wk->m_disp.okage[ 0 ].obj, TRUE );
			wk->cut++;
		}
		break;

	case eS2_PAL_ON:
		///< アニメ終わったら登場
		if ( wk->m_disp.kage[ 1 ].bAnime == FALSE ){
			wk->sys->shake.main_seq = eS2_PAL_ON;
			PokeOn_Init( wk, 1, MONSNO_EA );
			Snd_SePlayPan( DS_EFF_06, +70 );
		//	Snd_PMVoicePlayEx( PV_NORMAL, MONSNO_EA, +80, 40, HEAPID_DEMO_TENGAN, 0 );
		//	D3DOBJ_SetDraw( &wk->m_disp.poke[ 1 ].obj, TRUE );
		//	D3DOBJ_SetDraw( &wk->m_disp.okage[ 1 ].obj, TRUE );
			wk->cut++;
			wk->wait = 0;
		}
		break;
	
	case eS2_GINGA_BLANK:
		///< blank
		if ( (++wk->wait ) >= 30 ){
			wk->wait = 0;
			wk->cut++;
		}
		break;
	
	case eS2_GINGA_FLASH_1:
		wk->sys->shake.main_seq = eS2_GINGA_BLANK;
		if ( wk->flash == 0 ){
			if ( wk->sys->light_color.brightness < 8 ){
				wk->sys->light_color.brightness += 1;
				G2_SetBlendBrightness( DEMO_BLEND_MASK_M, wk->sys->light_color.brightness );		
			}
			else {
				wk->flash = 1;
			}
		}
		else {
			if ( wk->sys->light_color.brightness > 0 ){
				wk->sys->light_color.brightness -= 2;
				G2_SetBlendBrightness( DEMO_BLEND_MASK_M, wk->sys->light_color.brightness );		
			}
			else {
				wk->cut++;
				wk->flash = 0;
			}			
		}
		break;

	case eS2_GINGA_FLASH_2:
		if ( wk->flash == 0 ){
			if ( wk->sys->light_color.brightness < 12 ){
				wk->sys->light_color.brightness += 1;
				G2_SetBlendBrightness( DEMO_BLEND_MASK_M, wk->sys->light_color.brightness );		
			}
			else {
				wk->flash = 1;
			}
		}
		else {
			if ( wk->sys->light_color.brightness > 0 ){
				wk->sys->light_color.brightness -= 2;
				G2_SetBlendBrightness( DEMO_BLEND_MASK_M, wk->sys->light_color.brightness );		
			}
			else {
				wk->cut++;
				wk->flash = 0;
			}			
		}
		break;
	
	case eS2_GINGA_INIT:
		if ( wk->sys->light_color.brightness < DEMO_BRIGHTNESS_WHITE ){
			wk->sys->light_color.brightness += 2;
			G2_SetBlendBrightness( DEMO_BLEND_MASK_M, wk->sys->light_color.brightness );		
		}
		else {
			demo_scene02_ResourceDelete_Tama( wk );
			demo_scene02_ResourceLoad_Ginga( wk );
			wk->cut++;
		}
		break;
		
	case eS2_GINGA_ON:
		///< 銀河開始
		if ( wk->sys->light_color.brightness != 0 ){ 
			wk->sys->light_color.brightness--;
			G2_SetBlendBrightness( DEMO_BLEND_MASK_M, wk->sys->light_color.brightness );
		}
		else {
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
			//	wk->sys->motion_bl = DEMO_MOTION_BL_Init( &mb );
			}
			wk->m_disp.gira[ 0 ].bAnime		 = TRUE;		
			wk->m_disp.gira[ 0 ].bAnimeLoop  = TRUE;
			wk->m_disp.gira[ 0 ].bColorAnime = TRUE;
			wk->m_disp.gira[ 1 ].bAnime 	 = TRUE;
			wk->m_disp.gira[ 1 ].bAnimeLoop  = TRUE;
			wk->m_disp.gira[ 1 ].bColorAnime = TRUE;
			wk->cut++;
		}
		break;
		
	case eS2_GINGA_WAIT:
		if ( wk->m_disp.gira[ 0 ].obj_scale < FX32_CONST( 0.80 ) ){
			wk->m_disp.gira[ 0 ].obj_scale += FX32_CONST( 0.02 );
			wk->m_disp.gira[ 1 ].obj_scale += FX32_CONST( 0.02 );		
			D3DOBJ_SetScale( &wk->m_disp.gira[ 0 ].obj, wk->m_disp.gira[ 0 ].obj_scale, FX32_CONST( 1.0 ), wk->m_disp.gira[ 0 ].obj_scale );
			D3DOBJ_SetScale( &wk->m_disp.gira[ 1 ].obj, wk->m_disp.gira[ 1 ].obj_scale, FX32_CONST( 1.0 ), wk->m_disp.gira[ 1 ].obj_scale );
		}
		else {
			Demo_MSG_Set( wk->sys, wk->param, msg_d05r0114_gingaboss_04 );	///< 美しい～
			wk->cut++;
		}
		break;
	
	case eS2_GINGA_END:
		if ( GF_MSG_PrintEndCheck( wk->sys->msg_index ) ){ break; }
		Demo_MSG_Off( wk->sys );
//		if ( wk->wait >= ( 30 * 5 ) ){
#if 0
			if ( wk->m_disp.gira[ 0 ].obj_scale > FX32_CONST( 0.10 ) ){
				wk->m_disp.gira[ 0 ].obj_scale -= FX32_CONST( 0.02 );
				wk->m_disp.gira[ 1 ].obj_scale -= FX32_CONST( 0.02 );		
				D3DOBJ_SetScale( &wk->m_disp.gira[ 0 ].obj, wk->m_disp.gira[ 0 ].obj_scale, FX32_CONST( 1.0 ), wk->m_disp.gira[ 0 ].obj_scale );
				D3DOBJ_SetScale( &wk->m_disp.gira[ 1 ].obj, wk->m_disp.gira[ 1 ].obj_scale, FX32_CONST( 1.0 ), wk->m_disp.gira[ 1 ].obj_scale );
			}
			else {
				Demo_MSG_Set( wk->sys, wk->param, msg_d05r0114_gingaboss_05 );
				wk->cut++;
			}
#endif
		Demo_MSG_Set( wk->sys, wk->param, msg_d05r0114_gingaboss_05 );
		wk->cut++;
//		}
//		else {
//			wk->wait++;
//		}
		break;
	
	case eS2_DEMO_WIPE_OUT:
		if ( GF_MSG_PrintEndCheck( wk->sys->msg_index ) ){ break; }

		if ( wk->m_disp.gira[ 0 ].obj_scale > FX32_CONST( 0.10 ) ){
			wk->m_disp.gira[ 0 ].obj_scale -= FX32_CONST( 0.02 );
			wk->m_disp.gira[ 1 ].obj_scale -= FX32_CONST( 0.02 );		
			D3DOBJ_SetScale( &wk->m_disp.gira[ 0 ].obj, wk->m_disp.gira[ 0 ].obj_scale, FX32_CONST( 1.0 ), wk->m_disp.gira[ 0 ].obj_scale );
			D3DOBJ_SetScale( &wk->m_disp.gira[ 1 ].obj, wk->m_disp.gira[ 1 ].obj_scale, FX32_CONST( 1.0 ), wk->m_disp.gira[ 1 ].obj_scale );
		}
		else {
		//	Demo_MSG_Set( wk->sys, wk->param, msg_d05r0114_gingaboss_05 );
			WIPE_SYS_Start(	WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK,
							COMM_BRIGHTNESS_SYNC, 1, HEAPID_DEMO_TENGAN  );
			wk->cut++;
		}
		break;
	
	case eS2_DEMO_WIPE_WAIT:
		if( WIPE_SYS_EndCheck() == FALSE ) { break; }		
		Demo_MSG_Off( wk->sys );
		wk->cut++;
		break;
	
	case eS2_DEMO_END:
		wk->cut = 0;
		wk->wait = 0;
		return FALSE;
		break;
	}

	///< 影の補正	
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
	
	return TRUE;
//	return !( sys.trg );
}


BOOL Demo_Tengan_Scene02_Exit( void* work )
{
	DEMO_SCENE_02_WORK* wk = work;
	
	switch ( wk->cut ){
	case 0:
		demo_scene02_ResourceDelete( wk );
		DEMO_MOTION_BL_Delete( &wk->sys->motion_bl, GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D );
		wk->cut++;
		break;
	
	default:	
		sys_FreeMemoryEz( wk );
		return FALSE;
	}
	
	return TRUE;
}

