//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifi_2dmap_common.h
 *	@brief		wifi_2dmap共通データ
 *	@author		tomoya takahashi
 *	@data		2007.03.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WIFI_2DMAP_COMMON_H__
#define __WIFI_2DMAP_COMMON_H__

#include "src/field/fieldobj_code.h"
#include "application/wifi_2dmap/wf2dmap_common_c.h"

#undef GLOBAL
#ifdef	__WIFI_2DMAP_COMMON_H_GLOBAL
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
///	方向定数
//=====================================
typedef enum {
	WF2DMAP_WAY_UP =	WF2DMAP_WAY_C_UP,
	WF2DMAP_WAY_DOWN =  WF2DMAP_WAY_C_DOWN,
	WF2DMAP_WAY_LEFT =  WF2DMAP_WAY_C_LEFT,
	WF2DMAP_WAY_RIGHT = WF2DMAP_WAY_C_RIGHT,
	WF2DMAP_WAY_NUM =   WF2DMAP_WAY_C_NUM,
} WF2DMAP_WAY;

//-------------------------------------
///	コマンド定数
//=====================================
typedef enum {
	WF2DMAP_CMD_NONE = WF2DMAP_CMD_C_NONE,	// なにもなし
	WF2DMAP_CMD_TURN = WF2DMAP_CMD_C_TURN,	// ターンコマンド
	WF2DMAP_CMD_WALK = WF2DMAP_CMD_C_WALK,	// 歩くコマンド
	WF2DMAP_CMD_RUN =  WF2DMAP_CMD_C_RUN, // 走るコマンド
	WF2DMAP_CMD_BUSY = WF2DMAP_CMD_C_BUSY,	// 忙しい状態
	WF2DMAP_CMD_WALLWALK = WF2DMAP_CMD_C_WALLWALK,	// 壁方向歩きコマンド
	WF2DMAP_CMD_SLOWWALK = WF2DMAP_CMD_C_SLOWWALK,	// ゆっくり歩きコマンド
	WF2DMAP_CMD_WALK4	 = WF2DMAP_CMD_C_WALK4,	// ゆっくり歩きコマンド
	WF2DMAP_CMD_STAYWALK2 = WF2DMAP_CMD_C_STAYWALK2,// その場歩きコマンド
	WF2DMAP_CMD_STAYWALK4 = WF2DMAP_CMD_C_STAYWALK4,// その場歩きコマンド
	WF2DMAP_CMD_STAYWALK8 = WF2DMAP_CMD_C_STAYWALK8,// その場歩きコマンド
	WF2DMAP_CMD_STAYWALK16 = WF2DMAP_CMD_C_STAYWALK16,// ゆっくりその場歩きコマンド
	WF2DMAP_CMD_NUM =  WF2DMAP_CMD_C_NUM,
} WF2DMAP_CMD;


//-------------------------------------
///	動作フレーム数
//=====================================
#define WF2DMAP_FRAME_TURN		(2)
#define WF2DMAP_FRAME_WALK		(8)
#define WF2DMAP_FRAME_RUN		(4)
#define WF2DMAP_FRAME_WALLWALK	(16)
#define WF2DMAP_FRAME_SLOWWALK	(16)
#define WF2DMAP_FRAME_WALK2		(2)
#define WF2DMAP_FRAME_WALK4		(4)

//-------------------------------------
///	グリッド関係
//=====================================
#define WF2DMAP_GRID_SIZ		(16)	// グリッドサイズ
#define WF2DMAP_GRID2POS(x)	((x)*WF2DMAP_GRID_SIZ)	// グリッド->座標	
#define WF2DMAP_POS2GRID(x)	((x)/WF2DMAP_GRID_SIZ)	// 座標->グリッド	

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	座標構造体
//=====================================
typedef struct _WF2DMAP_POS{
	s16 x;
	s16 y;
} WF2DMAP_POS;

//-------------------------------------
///	リクエストコマンド
//=====================================
typedef struct {
	u16 cmd;
	u8 way;
	u8 playid;
} WF2DMAP_REQCMD;

//-------------------------------------
///	アクションコマンド
//=====================================
typedef struct {
	WF2DMAP_POS	pos;
	u16 cmd;
	u8 way;
	u8 playid;
} WF2DMAP_ACTCMD;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
#undef	GLOBAL
#endif		// __WIFI_2DMAP_COMMON_H__

