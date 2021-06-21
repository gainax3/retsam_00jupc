//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifi_2dcommon.h
 *	@brief		wifi_2dシステム共通ヘッダ
 *	@author		tomoya takahashi
 *	@data		2007.03.06
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WIFI_2DCOMMON_H__
#define __WIFI_2DCOMMON_H__

#undef GLOBAL
#ifdef	__WIFI_2DCOMMON_H_GLOBAL
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
///	向いている方向
typedef enum {
	WF_COMMON_TOP,
	WF_COMMON_BOTTOM,
	WF_COMMON_LEFT,
	WF_COMMON_RIGHT,
	WF_COMMON_WAYNUM
} WF_COMMON_WAY;

#define WF_COMMON_WALK_FRAME	(8)		// 歩きフレーム数
#define WF_COMMON_TURN_FRAME	(2)		// 振り向きフレーム数
#define WF_COMMON_RUN_FRAME		(4)		// 走りフレーム数
#define WF_COMMON_WALLWALK_FRAME (16)	// 壁歩きフレーム数
#define WF_COMMON_SLOWWALK_FRAME (16)	// ゆっくりあるきフレーム数
#define WF_COMMON_HIGHWALK2_FRAME (2)	// 高速あるきフレーム数
#define WF_COMMON_HIGHWALK4_FRAME (4)	// 高速あるきフレーム数

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
#undef	GLOBAL
#endif		// __WIFI_2DCOMMON_H__

