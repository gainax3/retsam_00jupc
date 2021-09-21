//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_3dmapobj_data.h
 *	@brief		配置オブジェグラフィック読み込みデータ
 *	@author		tomoya takahashi
 *	@data		2007.11.20
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WFLBY_3DMAPOBJ_DATA_H__
#define __WFLBY_3DMAPOBJ_DATA_H__

#include "../wflby_3dmapobj.h"


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	地面定数
//=====================================
enum {	// アニメ定数
	WFLBY_3DMAPOBJ_MAP_ANM_LOBBY,
	WFLBY_3DMAPOBJ_MAP_ANM_WLDTIMER,
	WFLBY_3DMAPOBJ_MAP_ANM_FLAG,
	WFLBY_3DMAPOBJ_MAP_ANM_MAT,		// マットアニメ
	WFLBY_3DMAPOBJ_MAP_ANM_POLL,
	WFLBY_3DMAPOBJ_MAP_ANM_NUM,
} ;
enum {	// モデル数
	WFLBY_3DMAPOBJ_MAPOBJ_MAP,	// 地面オブジェ
	WFLBY_3DMAPOBJ_MAPOBJ_MAT,	// マットオブジェ
	WFLBY_3DMAPOBJ_MAPOBJ_NUM,	// 地面オブジェ数
} ;


//-------------------------------------
///	フロート定数
//=====================================
enum {	// アニメ定数
	WFLBY_3DMAPOBJ_FLOAT_ANM_00,
	WFLBY_3DMAPOBJ_FLOAT_ANM_01,		// 泣き声用
	WFLBY_3DMAPOBJ_FLOAT_ANM_02,		// 頭
	WFLBY_3DMAPOBJ_FLOAT_ANM_03,		// 尻尾
	WFLBY_3DMAPOBJ_FLOAT_ANM_NUM,

	WFLBY_3DMAOOBJ_FLOAT_NODE_NUM = 3,	// アニメにかかわってくるノードの数(頭と尻尾と泣き声用)
} ;


//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	配置オブジェクトデータ
// 5/28現在要素数109
//=====================================
typedef struct {
	// 配置オブジェ関係
	u32		obj_idx[WFLBY_3DMAPOBJ_WK_NUM];											// 18
	u32		obj_anm[WFLBY_3DMAPOBJ_WK_NUM][WFLBY_3DMAPOBJ_ALL_ANM_MAX];				// それぞれのオブジェ分のアニメ	使用しない場合は、mdlのDATAIDが入ってます									   // 18*3 = 54

	// FLOAT関係
	u32		float_idx[WFLBY_3DMAPOBJ_FLOAT_NUM];									// 2
	u32		float_texidx[WFLBY_3DMAPOBJ_FLOAT_NUM][WFLBY_3DMAPOBJ_FLOAT_COL_NUM];	// 2*3	6
	u32		float_anmidx[WFLBY_3DMAPOBJ_FLOAT_NUM][WFLBY_3DMAPOBJ_FLOAT_ANM_NUM];	// 2*4	8
	u32		float_nodenum[WFLBY_3DMAPOBJ_FLOAT_NUM];								// 2
	u32		float_anm_node[WFLBY_3DMAPOBJ_FLOAT_NUM][ WFLBY_3DMAOOBJ_FLOAT_NODE_NUM ];// 2*3 6

	// マップ関係
	u32		map_idx[ WFLBY_3DMAPOBJ_MAPOBJ_NUM ];									// 2
	u32		map_nodenum;															// マップのノード数	1
	u32		map_anmidx[ WFLBY_3DMAPOBJ_MAP_ANM_NUM ];								// 5
	u32		map_anmnodeidx[ WFLBY_3DMAPOBJ_MAP_ANM_NUM ];							// 5
} WFLBY_3DMAPOBJ_MDL_DATA;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

#endif		// __WFLBY_3DMAPOBJ_DATA_H__

