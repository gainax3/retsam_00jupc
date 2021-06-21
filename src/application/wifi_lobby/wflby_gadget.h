//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_gadget.h
 *	@brief		ガジェットアニメーションシステム
 *	@author		tomoya takahashi
 *	@data		2008.01.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WFLBY_GADGET_H__
#define __WFLBY_GADGET_H__

#include "wflby_def.h"
#include "wflby_3dobjcont.h"
#include "wflby_camera.h"
#include "wflby_mapcont.h"
#include "wflby_system.h"

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
///	ガジェットシステム
//=====================================
typedef struct _WFLBY_GADGET WFLBY_GADGET;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
extern WFLBY_GADGET* WFLBY_GADGET_Init( WFLBY_SYSTEM* p_system, WFLBY_MAPCONT* p_mapcont, WFLBY_CAMERA* p_camera, WFLBY_3DOBJCONT* p_objcont, u32 heapID, u32 gheapID );
extern void WFLBY_GADGET_Exit( WFLBY_GADGET* p_sys );
extern void WFLBY_GADGET_Main( WFLBY_GADGET* p_sys );
extern void WFLBY_GADGET_Draw( WFLBY_GADGET* p_sys );
extern void WFLBY_GADGET_Start( WFLBY_GADGET* p_sys, u32 idx, WFLBY_ITEMTYPE gadget );
extern BOOL WFLBY_GADGET_EndWait( const WFLBY_GADGET* cp_sys, u32 idx );

#endif		// __WFLBY_GADGET_H__

