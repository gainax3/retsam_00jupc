//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifi_2dobj.h
 *	@brief		wifi 2dシステム　動作管理
 *	@author		tomoya takahashi
 *	@data		2007.03.06
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WIFI_2DOBJ_H__
#define __WIFI_2DOBJ_H__

#include "application/wifi_2dmap/wifi_2dcommon.h"
#include "application/wifi_2dmap/wifi_2dmap.h"

#undef GLOBAL
#ifdef	__WIFI_2DOBJ_H_GLOBAL
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
///	マップフラグ
//	当たり判定に使用する定数です。
//=====================================
typedef enum {
	WF_MAP_NONE = 0,
	WF_MAP_PLAYER,			// 人のいるところ
	WF_MAP_PLAYER_NEXT,		// 次人が来るところ
	WF_MAP_BLOCK,			// 人以外のいけないところ
	WF_MAP_FLAGNUM,
} WF_MAP_FLAG;


//-------------------------------------
///	動作状態定数
//=====================================
typedef enum {
	WF_OBJ_STATUS_NONE,	// 待機状態
	WF_OBJ_STATUS_TURN,	// 振り向き状態	
	WF_OBJ_STATUS_WALK,	// 歩き状態	
	WF_OBJ_STATUS_RUN,	// 走り状態	
	WF_OBJ_STATUS_NUM,	// 
} WF_OBJ_STATUS;

//-------------------------------------
///	パラメータ指定定数
//=====================================
typedef enum {
	WF_OBJ_PARAM_X,			// X座標
	WF_OBJ_PARAM_Y,			// Y座標
	WF_OBJ_PARAM_PLAYID,	// プレイヤー識別ID	（AIDなどを指定）
	WF_OBJ_PARAM_STATUS,	// 今の状態
	WF_OBJ_PARAM_WAY,	// 向いている方向
	WF_OBJ_PARAM_NUM
} WF_OBJ_PARAM;


//-------------------------------------
///	座標関連
//=====================================
#define WF_OBJ_GRID_SIZ	( 16 )	// 1グリッドサイズ
#define WF_OBJ_GRID_TO_POS(x)	( (x)*WF_OBJ_GRID_SIZ )	// グリッドを座標に変換
#define WF_OBJ_POS_TO_GRID(x)	( (x)/WF_OBJ_GRID_SIZ )	// 座標をグリッドに変換

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	管理システム
//=====================================
typedef struct _WF_OBJ_SYS WF_OBJ_SYS;

//-------------------------------------
///	オブジェクトワーク
//=====================================
typedef struct _WF_OBJ_WK WF_OBJ_WK;

//-------------------------------------
///	オブジェクトワーク初期化データ
//=====================================
typedef struct {
	s16 x;		// x座標
	s16 y;		// y座標
	u16 playid;	// プレイヤー識別ID	（AIDなどを指定）
	u16 way;	// 方向（WF_COMMON_WAY）
} WF_OBJ_WKDATA;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
// システム作成　はき
GLOBAL WF_OBJ_SYS* WF_OBJ_SysInit( WF_MAP_SYS* p_mapdata, u32 objnum, u32 heapID );
GLOBAL void WF_OBJ_SysExit( WF_OBJ_SYS* p_sys );

// オブジェクト生成
GLOBAL WF_OBJ_WK* WF_OBJ_WkNew( WF_OBJ_SYS* p_sys, const WF_OBJ_WKDATA* cp_data );
GLOBAL void WF_OBJ_WkDel( WF_OBJ_SYS* p_sys, WF_OBJ_WK* p_wk );


// オブジェクト操作
GLOBAL BOOL WF_OBJ_WkMoveReq( WF_OBJ_SYS* p_sys, WF_OBJ_WK* p_wk, WF_OBJ_STATUS req, WF_COMMON_WAY way );
GLOBAL BOOL WF_OBJ_WkMain( WF_OBJ_SYS* p_sys, WF_OBJ_WK* p_wk );
GLOBAL s32 WF_OBJ_WkParamGet( const WF_OBJ_WK* cp_wk, WF_OBJ_PARAM param );
GLOBAL void WF_OBJ_WkParamSet( WF_OBJ_WK* p_wk, WF_OBJ_PARAM param, s32 num );
GLOBAL WF_MAP WF_OBJ_WkPosMapGet( const WF_OBJ_SYS* cp_sys, const WF_OBJ_WK* cp_wk );
GLOBAL WF_MAP WF_OBJ_WkWayPosMapGet( const WF_OBJ_SYS* cp_sys, const WF_OBJ_WK* cp_wk, WF_COMMON_WAY way );


#undef	GLOBAL
#endif		// __WIFI_2DOBJ_H__

