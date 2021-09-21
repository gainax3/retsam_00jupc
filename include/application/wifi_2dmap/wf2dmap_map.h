//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wf2dmap_map.h
 *	@brief		マップデータ管理システム
 *	@author		tomoya takahashi
 *	@data		2007.03.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WF2DMAP_MAP_H__
#define __WF2DMAP_MAP_H__

#include "application/wifi_2dmap/wf2dmap_common.h"

#undef GLOBAL
#ifdef	__WF2DMAP_MAP_H_GLOBAL
#define	GLOBAL	/* */
#else
#define	GLOBAL	extern
#endif

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
#define WF2DMAP_MAPDATA_NONE	(0xffffffff)

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	マップデータ構造体
//	データ構造
//	31～1 0		bit
//	param hit
//
//	1bitがヒットフラグ
//	残り31bitがパラメータ
//=====================================
typedef union {
	u32 data;
	struct {
		u32 hit:1;		// 当たり判定データ
		u32	param:31;	// パラメータ
	};
} WF2DMAP_MAP;
#define WF2DMAP_MAPDATA(hit, param)		(((param)<<1) | (hit))	// マップデータ作成

//-------------------------------------
///	マップデータ管理システムワーク
//=====================================
typedef struct _WF2DMAP_MAPSYS WF2DMAP_MAPSYS;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
// システム管理
GLOBAL WF2DMAP_MAPSYS* WF2DMAP_MAPSysInit( u16 xgrid, u16 ygrid, u32 heapID );
GLOBAL void WF2DMAP_MAPSysExit( WF2DMAP_MAPSYS* p_sys );

// マップデータ設定
GLOBAL void WF2DMAP_MAPSysDataSet( WF2DMAP_MAPSYS* p_sys, const WF2DMAP_MAP* cp_buff );
// マップの大きさ取得
GLOBAL u16 WF2DMAP_MAPSysGridXGet( const WF2DMAP_MAPSYS* cp_sys );
GLOBAL u16 WF2DMAP_MAPSysGridYGet( const WF2DMAP_MAPSYS* cp_sys );

// マップデータ取得
GLOBAL WF2DMAP_MAP WF2DMAP_MAPSysDataGet( const WF2DMAP_MAPSYS* cp_sys, u16 xgrid, u16 ygrid );
GLOBAL BOOL WF2DMAP_MAPSysHitGet( const WF2DMAP_MAPSYS* cp_sys, u16 xgrid, u16 ygrid );
GLOBAL u32 WF2DMAP_MAPSysParamGet( const WF2DMAP_MAPSYS* cp_sys, u16 xgrid, u16 ygrid );


#undef	GLOBAL
#endif		// __WF2DMAP_MAP_H__

