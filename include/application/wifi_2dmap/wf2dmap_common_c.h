//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wf2dmap_common_c.h
 *	@brief
 *	@author	
 *	@data		2007.04.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WF2DMAP_COMMON_C_H__
#define __WF2DMAP_COMMON_C_H__

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
// 方向
#define 	WF2DMAP_WAY_C_UP	(0) 
#define 	WF2DMAP_WAY_C_DOWN	(1) 
#define 	WF2DMAP_WAY_C_LEFT	(2) 
#define 	WF2DMAP_WAY_C_RIGHT	(3) 
#define 	WF2DMAP_WAY_C_NUM	(4) 


//コマンド
#define 	WF2DMAP_CMD_C_NONE			(0) 	// なにもなし
#define 	WF2DMAP_CMD_C_TURN			(1) 	// ターンコマンド
#define 	WF2DMAP_CMD_C_WALK			(2) 	// 歩くコマンド
#define 	WF2DMAP_CMD_C_RUN			(3) 	// 走るコマンド
#define 	WF2DMAP_CMD_C_BUSY			(4) 	// 忙しい状態
#define 	WF2DMAP_CMD_C_WALLWALK		(5) 	// 壁方向歩きコマンド
#define 	WF2DMAP_CMD_C_SLOWWALK		(6) 	// ゆっくり歩きコマンド
#define 	WF2DMAP_CMD_C_WALK4			(7) 	// 早歩きコマンド
#define 	WF2DMAP_CMD_C_STAYWALK2		(8) 	// その場歩きコマンド
#define 	WF2DMAP_CMD_C_STAYWALK4		(9) 	// その場歩きコマンド
#define 	WF2DMAP_CMD_C_STAYWALK8		(10) 	// その場歩きコマンド
#define 	WF2DMAP_CMD_C_STAYWALK16	(11) 	// ゆっくりその場歩きコマンド
#define 	WF2DMAP_CMD_C_NUM			(12) 

#endif		// __WF2DMAP_COMMON_C_H__

