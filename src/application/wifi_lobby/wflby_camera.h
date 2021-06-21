//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_camera.h
 *	@brief		WiFi広場カメラ	
 *	@author		tomoya takahashi
 *	@data		2007.11.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WFLBY_CAMERA_H__
#define __WFLBY_CAMERA_H__

#include "wflby_3dobjcont.h"

#ifdef PM_DEBUG
#define WFLBY_CAMERA_DEBUG
#endif

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	カメラシステム
//=====================================
typedef struct _WFLBY_CAMERA WFLBY_CAMERA;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
extern WFLBY_CAMERA* WFLBY_CAMERA_Init( u32 heapID );
extern void WFLBY_CAMERA_Exit( WFLBY_CAMERA* p_sys );
extern void WFLBY_CAMERA_Draw( WFLBY_CAMERA* p_sys );
extern void WFLBY_CAMERA_SetTarget( WFLBY_CAMERA* p_sys, s32 posx, s32 posy );
extern void WFLBY_CAMERA_SetTarget3D( WFLBY_CAMERA* p_sys, fx32 x, fx32 y, fx32 z );
extern void WFLBY_CAMERA_SetTargetPerson3D( WFLBY_CAMERA* p_sys, fx32 x, fx32 y, fx32 z );
extern void WFLBY_CAMERA_SetTargetPerson( WFLBY_CAMERA* p_sys, const WFLBY_3DPERSON* cp_person );
extern const WFLBY_3DPERSON* WFLBY_CAMERA_GetTargetPerson( const WFLBY_CAMERA* cp_sys );
extern void WFLBY_CAMERA_ResetTargetPerson( WFLBY_CAMERA* p_sys );

#ifdef WFLBY_CAMERA_DEBUG
extern WFLBY_CAMERA* WFLBY_CAMERA_DEBUG_CameraInit( u32 heapID );
#endif

#endif		// __WFLBY_CAMERA_H__

