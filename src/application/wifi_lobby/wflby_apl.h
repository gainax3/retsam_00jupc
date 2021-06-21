//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_apl.h
 *	@brief		WiFiロビーアプリケーション管理システム
 *	@author		tomoya takahashi
 *	@data		2007.09.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WFLBY_APL_H__
#define __WFLBY_APL_H__

#include "wflby_system_def.h"
#include "field/wflby_counter.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///		メイン関数戻り値
//=====================================
typedef enum {
	WFLBY_APL_RET_NONE,
	WFLBY_APL_RET_END,
} WFLBY_APL_RET;

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	WiFiロビー　アプリ管理システム
//=====================================
typedef struct _WFLBY_APL	WFLBY_APL;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
extern WFLBY_APL* WFLBY_APL_Init( BOOL check_skip, SAVEDATA* p_save, WFLBY_COUNTER_TIME* p_wflby_counter, WFLBY_SYSTEM* p_system, u32 heapID );
extern void WFLBY_APL_Exit( WFLBY_APL* p_wk );
extern void WFLBY_APL_Start( WFLBY_APL* p_wk );
extern WFLBY_APL_RET WFLBY_APL_Main( WFLBY_APL* p_wk );
extern void WFLBY_APL_VBlank( WFLBY_APL* p_wk );

#endif		// __WFLBY_APL_H__

