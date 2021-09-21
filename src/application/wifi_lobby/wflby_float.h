//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_float.h
 *	@brief		フロート管理システム	(SYSTEMのフロートデータを描画に反映させるシステム)
 *	@author		tomoya takahashi
 *	@data		2008.01.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WFLBY_FLOAT_H__
#define __WFLBY_FLOAT_H__

#include "wflby_room_def.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	アニメ種類定数
//=====================================
typedef enum {
	WFLBY_FLOAT_ANM_SOUND,	// 音アニメ
	WFLBY_FLOAT_ANM_SHAKE,	// ゆれアニメ
	WFLBY_FLOAT_ANM_BODY,	// 胴体アニメ
	WFLBY_FLOAT_ANM_NUM,
} WFLBY_FLOAT_ANM_TYPE;

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	フロート管理ワーク
//=====================================
typedef struct _WFLBY_FLOAT_CONT WFLBY_FLOAT_CONT;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
extern WFLBY_FLOAT_CONT* WFLBY_FLOAT_CONT_Init( WFLBY_ROOMWK* p_rmwk, u32 heapID );
extern void WFLBY_FLOAT_CONT_Exit( WFLBY_FLOAT_CONT* p_sys );
extern void WFLBY_FLOAT_CONT_Main( WFLBY_FLOAT_CONT* p_sys );

extern BOOL WFLBY_FLOAT_CONT_CheckAnm( const WFLBY_FLOAT_CONT* cp_sys, u32 floatidx, WFLBY_FLOAT_ANM_TYPE anmidx );

#endif		// __WFLBY_FLOAT_H__

