//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_mapcont.h
 *	@brief		マップ管理システム
 *	@author		tomoya	takahashi
 *	@data		2007.11.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WFLBY_MAPCONT_H__
#define __WFLBY_MAPCONT_H__

#include "application/wifi_2dmap/wf2dmap_map.h"
#include "map_conv/wflby_mapdata.h"

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
///	マップシステム初期化
//=====================================
typedef struct _WFLBY_MAPCONT WFLBY_MAPCONT;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
// システム作成
extern WFLBY_MAPCONT* WFLBY_MAPCONT_Init( u32 heapID );
extern void WFLBY_MAPCONT_Exit( WFLBY_MAPCONT* p_sys );

// マップの大きさ取得
extern u16 WFLBY_MAPCONT_GridXGet( const WFLBY_MAPCONT* cp_sys );
extern u16 WFLBY_MAPCONT_GridYGet( const WFLBY_MAPCONT* cp_sys );

// マップデータ取得
extern WF2DMAP_MAP WFLBY_MAPCONT_DataGet( const WFLBY_MAPCONT* cp_sys, u16 xgrid, u16 ygrid );
extern BOOL WFLBY_MAPCONT_HitGet( const WFLBY_MAPCONT* cp_sys, u16 xgrid, u16 ygrid );
extern u32 WFLBY_MAPCONT_ObjIDGet( const WFLBY_MAPCONT* cp_sys, u16 xgrid, u16 ygrid );
extern u32 WFLBY_MAPCONT_EventGet( const WFLBY_MAPCONT* cp_sys, u16 xgrid, u16 ygrid );

// マップデータ検索
extern BOOL WFLBY_MAPCONT_SarchObjID( const WFLBY_MAPCONT* cp_sys, u32 objid, u16* p_gridx, u16* p_gridy, u32 no );
extern u32 WFLBY_MAPCONT_CountObjID( const WFLBY_MAPCONT* cp_sys, u32 objid );

// イベント起動タイプチェック
extern BOOL WFLBY_MAPCONT_Event_CheckMount( u32 event );
extern BOOL WFLBY_MAPCONT_Event_CheckFrontKey( u32 event );
extern BOOL WFLBY_MAPCONT_Event_CheckFrontDecide( u32 event );
extern BOOL WFLBY_MAPCONT_Event_CheckFrontDouzou( u32 event );



//  出来るだけつかわないほうがいい関数
extern const WF2DMAP_MAPSYS* WFLBY_MAPCONT_GetMapCoreSys( const WFLBY_MAPCONT* cp_sys );

#endif		// __WFLBY_MAPCONT_H__

