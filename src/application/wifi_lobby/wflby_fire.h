//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_fire.h
 *	@brief		WiFiLêÔÎÇ
 *	@author		tomoya takahashi
 *	@data		2008.03.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WFLBY_FIRE_H__
#define __WFLBY_FIRE_H__

#include "wflby_system.h"
#include "wflby_3dmapobj_cont.h"

//-----------------------------------------------------------------------------
/**
 *					èé¾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					\¢Ìé¾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	ÔÎÇVXe
//=====================================
typedef struct _WFLBY_FIRE_CONT WFLBY_FIRE_CONT;

//-----------------------------------------------------------------------------
/**
 *					vg^Cvé¾
*/
//-----------------------------------------------------------------------------
extern WFLBY_FIRE_CONT* WFLBY_FIRECONT_Init( const WFLBY_SYSTEM* cp_system, WFLBY_3DMAPOBJ_CONT* p_mapobjcont, u32 heapID );
extern void WFLBY_FIRECONT_Exit( WFLBY_FIRE_CONT* p_wk );
extern void WFLBY_FIRECONT_Main( WFLBY_FIRE_CONT* p_wk );

extern void WFLBY_FIRECONT_StopFireSe( WFLBY_FIRE_CONT* p_wk );

#endif		// __WFLBY_FIRE_H__

