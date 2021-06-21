
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

#include "system/fontproc.h"

#include "demo_tengan_common.h"

//void Camera_Work_

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 * @param	msg	
 *
 * @retval	int	
 *
 */
//--------------------------------------------------------------
int Demo_MSG_Set( DEMO_SYSTEM* sys, DEMO_TENGAN_PARAM* param, int msg )
{
	int index;
	MSGDATA_MANAGER* man;
	STRBUF* str;
	STRBUF*	tmp = STRBUF_Create( 255, HEAPID_DEMO_TENGAN );
	u8 wait = 0;

	man = sys->man;
	str	= MSGMAN_AllocString( man, msg );
	
	if ( msg == msg_d05r0114_gingaboss_08 ){
		STRBUF*		my_name = MyStatus_CreateNameString( param->my_status, HEAPID_DEMO_TENGAN );
		WORDSET*	wset = WORDSET_Create( HEAPID_DEMO_TENGAN );
		
		WORDSET_RegisterWord( wset, 0, my_name, 0, TRUE, PM_LANG );
		WORDSET_ExpandStr( wset, tmp, str );
		
		STRBUF_Delete( my_name );
		WORDSET_Delete( wset );
	}
	else {
		STRBUF_Copy( tmp, str );
	}
	
//	if ( msg < eMSG_1_5 ){
//		wait = MSGSPEED_FAST;//CONFIG_GetMsgSpeed( param->cfg );
		wait = CONFIG_GetMsgPrintSpeed( param->cfg );
		OS_Printf( "msg wait = %d\n" );
//	}
	
	GF_BGL_BmpWinDataFill( &sys->win, 0xFF );
	index = GF_STR_PrintSimple( &sys->win, FONT_TALK, tmp, 0, 0, wait, NULL );
	GF_BGL_BmpWinOn( &sys->win );
	
	BmpTalkWinWrite( &sys->win, WINDOW_TRANS_ON, 500, 15 );

	STRBUF_Delete( str );
	STRBUF_Delete( tmp );		
	
	sys->msg_index = index;	///< 戻り値で返して使うけど、ワーク一緒だからとりあえず入れとく
	
	return index;
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
void Demo_MSG_Off( DEMO_SYSTEM* sys )
{
	BmpTalkWinClear( &sys->win, WINDOW_TRANS_OFF );
	GF_BGL_BmpWinOff( &sys->win );	
}

//--------------------------------------------------------------
/**
 * @brief	ライト設定
 *
 * @param	sys
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void Demo_Light( DEMO_SYSTEM* sys )
{
	VecFx32 vec0 = { 0, -FX32_ONE, -FX32_ONE };
	VecFx32 vec1 = { -2043,-3548,110 };
	
	///< ギラティナのライト
	VEC_Normalize( &vec0, &vec0 );
	NNS_G3dGlbLightVector( 0, vec0.x, vec0.y, vec0.z );
	NNS_G3dGlbLightColor( 0, GX_RGB( 31, 31, 31 ) );
	
	///< MAP側のライト
	VEC_Normalize( &vec1, &vec1 );
	NNS_G3dGlbLightVector( 1, vec1.x, vec1.y, vec1.z );
	NNS_G3dGlbLightColor( 1, GX_RGB( 23, 23, 25 ) );

	NNS_G3dGlbMaterialColorDiffAmb( GX_RGB( 16,16,16 ), GX_RGB( 14,14,14 ), FALSE );
	NNS_G3dGlbMaterialColorSpecEmi( GX_RGB( 20,20,20 ), GX_RGB( 16,16,16 ), FALSE );	
}


//--------------------------------------------------------------
/**
 * @brief	カメラ操作
 *
 * @param	sys	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void Demo_Camera( DEMO_SYSTEM* sys )
{
	CAMERA_ANGLE angle = GFC_GetCameraAngle( sys->camera_p );
	VecFx32		 pos   = GFC_GetCameraPos( sys->camera_p );

	GFC_SetCameraPos( &pos, sys->camera_p );
	GFC_SetCameraAngleRev( &angle, sys->camera_p );
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	cw	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void Camera_Work_Init( CAMERA_WORK* cw )
{
	CAMERA_ANGLE angle;
	cw->work[ 0 ] = cw->set_frame;
	cw->work[ 1 ] = ANGLE( cw->x / cw->set_frame );
	cw->work[ 2 ] = ANGLE( cw->y / cw->set_frame );
	cw->work[ 3 ] = ANGLE( cw->z / cw->set_frame );
	
	cw->mwork[ 1 ] = cw->mx / cw->set_frame;
	cw->mwork[ 2 ] = cw->my / cw->set_frame;
	cw->mwork[ 3 ] = cw->mz / cw->set_frame;
	
	angle = GFC_GetCameraAngle( cw->camera_p );
	cw->angle.x = angle.x + ANGLE( cw->x );
	cw->angle.y = angle.y + ANGLE( cw->y );
	cw->angle.z = angle.z + ANGLE( cw->z );
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	cw	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL Camera_Work_Main( CAMERA_WORK* cw )
{
	BOOL bEnd = FALSE;
	CAMERA_ANGLE angle = GFC_GetCameraAngle( cw->camera_p );
	
	if ( cw->work[ 0 ] == 0 ){ return TRUE; }
	
	if ( (--cw->work[ 0 ] ) == 0 ){
		angle.x = cw->angle.x;
		angle.y = cw->angle.y;
		angle.z = cw->angle.z;
		bEnd = TRUE;	
	}
	else {
		angle.x += ( cw->work[ 1 ] );
		angle.y += ( cw->work[ 2 ] );
		angle.z += ( cw->work[ 3 ] );
		
		cw->camera_target->x += cw->mwork[ 1 ];
		cw->camera_target->y += cw->mwork[ 2 ];
		cw->camera_target->z += cw->mwork[ 3 ];
	}	
	
	GFC_SetCameraAngleRev( &angle, cw->camera_p );
	
	return bEnd;
}

//--------------------------------------------------------------
/**
 * @brief	１個描画関数 アニメ考慮
 *
 * @param	mdl	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void D3D_Draw( T3D_MODEL* mdl )
{
	if ( mdl->bInit == FALSE ){ return; }
	
	if ( mdl->bAnime ){
		
		BOOL bAnime = FALSE;
		
		if ( mdl->bAnimeLoop ){
			
			if ( mdl->anm_no == 0 ){
				D3DOBJ_AnmLoop( &mdl->anm[ 0 ], mdl->anm_speed );
			}
			else {
				Demo_Anime_Command( mdl );
				Demo_Anime( mdl );
			}
		}
		else {
			if ( mdl->bFullAnime ){
				int i;
				for ( i = 0; i < 4; i++ ){
					bAnime = D3DOBJ_AnmNoLoop( &mdl->anm[ i ], mdl->anm_speed );
				}
			}
			else {
				bAnime = D3DOBJ_AnmNoLoop( &mdl->anm[ 0 ], mdl->anm_speed );
			}
		}
		
		if ( mdl->bTexAnime ){
			D3DOBJ_AnmLoop( &mdl->anm[ 1 ], mdl->anm_speed );
		}
		if ( mdl->bColorAnime ){
			D3DOBJ_AnmNoLoop( &mdl->anm[ 1 ], mdl->anm_speed );
		}		
		if ( bAnime == TRUE ){
			mdl->bAnime = FALSE;
		}
	}

	NNS_G3dGePushMtx();

	D3DOBJ_Draw( &mdl->obj );
	
	NNS_G3dGePopMtx( 1 );
}



//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	mdl	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void res_delete( T3D_MODEL* mdl )
{
	D3DOBJ_MdlDelete( &mdl->mdl );
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	no	
 * @param	mdl	
 * @param	allocator	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void res_anime_delete( int no, T3D_MODEL* mdl, NNSFndAllocator* allocator )
{
	D3DOBJ_AnmDelete( &mdl->anm[ no ], allocator );
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	mdl	
 * @param	allocator	
 * @param	anm_num	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void res_model_delete( T3D_MODEL* mdl, NNSFndAllocator* allocator, int anm_num )
{
	int i;
	
	res_delete( mdl );
	
	for ( i = 0; i < anm_num; i++ ){
		res_anime_delete( i, mdl, allocator );
	}
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	mdl	
 * @param	id	
 * @param	hdl	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void res_load_init( T3D_MODEL* mdl, int id, ARCHANDLE* hdl )
{
	D3DOBJ_MdlLoadH( &mdl->mdl, hdl, id, HEAPID_DEMO_TENGAN );
	D3DOBJ_Init( &mdl->obj, &mdl->mdl );
	D3DOBJ_SetMatrix( &mdl->obj, 0, 0, 0 );
    D3DOBJ_SetScale( &mdl->obj, FX32_CONST(1.00f), FX32_CONST(1.00f), FX32_CONST(1.00f) );
    D3DOBJ_SetDraw( &mdl->obj, TRUE );	
    
    mdl->bInit = TRUE;
}

void res_load_init_Ex( T3D_MODEL* base_mdl, T3D_MODEL* mdl,  int id, ARCHANDLE* hdl )
{
	D3DOBJ_Init( &mdl->obj, &base_mdl->mdl );
	D3DOBJ_SetMatrix( &mdl->obj, 0, 0, 0 );
    D3DOBJ_SetScale( &mdl->obj, FX32_CONST(1.00f), FX32_CONST(1.00f), FX32_CONST(1.00f) );
    D3DOBJ_SetDraw( &mdl->obj, TRUE );	
    
    mdl->bInit = TRUE;	
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	no	
 * @param	mdl	
 * @param	id	
 * @param	hdl	
 * @param	allocator	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void res_anime_init( int no, T3D_MODEL* mdl, int id, ARCHANDLE* hdl, NNSFndAllocator* allocator )
{
	D3DOBJ_AnmLoadH( &mdl->anm[ no ], &mdl->mdl, hdl, id, HEAPID_DEMO_TENGAN, allocator );
	D3DOBJ_AnmSet( &mdl->anm[ no ], 0 );
	D3DOBJ_AddAnm( &mdl->obj, &mdl->anm[ no ] );
	
	mdl->anm_speed = FX32_CONST( 1 );
	
	mdl->cmd_index = 0xFF;
	mdl->cmd_count = 0xFF;
}

void res_anime_init_Ex( int no, T3D_MODEL* base_mdl, T3D_MODEL* mdl, int id, ARCHANDLE* hdl, NNSFndAllocator* allocator )
{
	D3DOBJ_AnmLoadH( &mdl->anm[ no ], &base_mdl->mdl, hdl, id, HEAPID_DEMO_TENGAN, allocator );
	D3DOBJ_AnmSet( &mdl->anm[ no ], 0 );
	D3DOBJ_AddAnm( &mdl->obj, &mdl->anm[ no ] );
	
	mdl->anm_speed = FX32_CONST( 1 );
	
	mdl->cmd_index = 0xFF;
	mdl->cmd_count = 0xFF;		
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	sys	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BG_PaletteFade_Sub( DEMO_SYSTEM* sys )
{	
		
	if ( sys->evy_pat == 0 ){

		if ( sys->evy < 31 ){ sys->evy++; } else { sys->evy_pat ^= 1; }
	}
	else {

		if ( sys->evy >  0 ){ sys->evy--; } else { sys->evy_pat ^= 1; }
	}
	
	SoftFadePfd( sys->pfd, FADE_SUB_BG, 0, 4, sys->evy / 8, 0xCCCC );
	SoftFadePfd( sys->pfd, FADE_SUB_BG, 4, 6, sys->evy / 8, 0xCCCC );
}

//--------------------------------------------------------------
/**
 * @brief	UMA
 *
 * @param	sys	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BG_PaletteFade_Sub_UMA( DEMO_SYSTEM* sys, int poke_type )
{
	int type;
	const u8 param[] = {
		4, 4, 4,
	};
	
	const fade_tbl[][ 2 ] = {
		{ 0, 31 },
		{ 0, 31 },
		{ 0, 31 },
	};
	
	const u8 mm_tbl[][ 2 ] = {
		{ 0, 32 },
		{ 0, 32 },
		{ 0, 32 },
	};
	
	const int color[] = {
	//	0x001f, 0x001f, 0x001f,
		0x0010, 0x0010, 0x0010,
	};
	
	
	type = poke_type;
	if ( poke_type >= NELEMS( color ) ){
		 type = NELEMS( color ) - 1;
	}
	
	if ( sys->evy_pat == 2 ){	
		if ( sys->evy_wait < ( ( type + 1 ) * 2 ) ){
			sys->evy_wait++;
		}
		else {
			sys->evy_pat  = 0;
			sys->evy_wait = 0;
		}
		
		return;
	}
		
	if ( sys->evy_pat == 0 ){

		if ( sys->evy < fade_tbl[ type ][ 1 ] ){ sys->evy++; } else { sys->evy_pat++; }
	}
	else {

		if ( sys->evy > fade_tbl[ type ][ 0 ] ){ sys->evy--; } else { sys->evy_pat++; }
	}
	
	SoftFadePfd( sys->pfd, FADE_SUB_BG, mm_tbl[ type ][ 0 ], mm_tbl[ type ][ 1 ], sys->evy / param[ type ], color[ type ] );
}



//--------------------------------------------------------------
/**
 * @brief	メインディスプレイがどっちか指定する
 *
 * @param	mode	DISP_3D_TO_SUB	DISP_3D_TO_MAIN
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void Demo_MainDispSet( int mode )
{
	sys.disp3DSW = mode;
	GF_Disp_DispSelect();	
}


//--------------------------------------------------------------
/**
 * @brief	ブライトネス設定
 *
 * @param	wk	
 * @param	para	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void Demo_BlendBrightness( DEMO_TENGAN_WORK* wk, int para )
{
	wk->sys.light_color.brightness = para;
	G2_SetBlendBrightness(  DEMO_BLEND_MASK_M, wk->sys.light_color.brightness );
	G2S_SetBlendBrightness( DEMO_BLEND_MASK_S, wk->sys.light_color.brightness );
}

