//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifi_p2pmatchroom_map.h
 *	@brief		wifi	マッチングルーム	マップデータ
 *	@author		tomoya takahashi
 *	@data		2007.03.30
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WIFI_P2PMATCHROOM_MAP_H__
#define __WIFI_P2PMATCHROOM_MAP_H__

#include "application/wifi_2dmap/wf2dmap_common.h"
#include "application/wifi_2dmap/wf2dmap_map.h"

#undef GLOBAL
#ifdef	__WIFI_P2PMATCHROOM_MAP_H_GLOBAL
#define	GLOBAL	/* */
#else
#define	GLOBAL	extern
#endif

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	マップパラメータ
//=====================================
enum {
	MCR_MAPPM_NONE,
	MCR_MAPPM_BLOCK,	// いけないところ
	MCR_MAPPM_PC,		// Personal Computer
	MCR_MAPPM_MAP00,	// 緑マット
	MCR_MAPPM_MAP01,	// 黄色マット
	MCR_MAPPM_MAP02,	// 青マット
	MCR_MAPPM_MAP03,	// 赤マット
	MCR_MAPPM_OBJ00,	// オブジェ配置位置00
	MCR_MAPPM_OBJ01,	// オブジェ配置位置01
	MCR_MAPPM_OBJ02,	// オブジェ配置位置02
	MCR_MAPPM_OBJ03,	// オブジェ配置位置03
	MCR_MAPPM_OBJ04,	// オブジェ配置位置04
	MCR_MAPPM_OBJ05,	// オブジェ配置位置05
	MCR_MAPPM_OBJ06,	// オブジェ配置位置06
	MCR_MAPPM_OBJ07,	// オブジェ配置位置07
	MCR_MAPPM_EXIT,
};
#define WCR_MAPDATA_BLOCK	(WF2DMAP_MAPDATA(1,MCR_MAPPM_BLOCK))
#define WCR_MAPDATA_PC		(WF2DMAP_MAPDATA(1,MCR_MAPPM_PC))
#define WCR_MAPDATA_EXIT	(WF2DMAP_MAPDATA(0,MCR_MAPPM_EXIT))
#define WCR_MAPDATA_MAP00	(WF2DMAP_MAPDATA(0,MCR_MAPPM_MAP00))
#define WCR_MAPDATA_MAP01	(WF2DMAP_MAPDATA(0,MCR_MAPPM_MAP01))
#define WCR_MAPDATA_MAP02	(WF2DMAP_MAPDATA(0,MCR_MAPPM_MAP02))
#define WCR_MAPDATA_MAP03	(WF2DMAP_MAPDATA(0,MCR_MAPPM_MAP03))
#define WCR_MAPDATA_OBJ00	(WF2DMAP_MAPDATA(0,MCR_MAPPM_OBJ00))
#define WCR_MAPDATA_OBJ01	(WF2DMAP_MAPDATA(0,MCR_MAPPM_OBJ01))
#define WCR_MAPDATA_OBJ02	(WF2DMAP_MAPDATA(0,MCR_MAPPM_OBJ02))
#define WCR_MAPDATA_OBJ03	(WF2DMAP_MAPDATA(0,MCR_MAPPM_OBJ03))
#define WCR_MAPDATA_OBJ04	(WF2DMAP_MAPDATA(0,MCR_MAPPM_OBJ04))
#define WCR_MAPDATA_OBJ05	(WF2DMAP_MAPDATA(0,MCR_MAPPM_OBJ05))
#define WCR_MAPDATA_OBJ06	(WF2DMAP_MAPDATA(0,MCR_MAPPM_OBJ06))
#define WCR_MAPDATA_OBJ07	(WF2DMAP_MAPDATA(0,MCR_MAPPM_OBJ07))

#define WCR_MAPDATA_1BLOCKOBJNUM	( 8 )	// 1BLOCKに入る人物の数


//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
GLOBAL WF2DMAP_POS WcrMapGridSizGet( u32 no );
GLOBAL const WF2DMAP_MAP* WcrMapDataGet( u32 no );


#undef	GLOBAL
#endif		// __WIFI_P2PMATCHROOM_MAP_H__

