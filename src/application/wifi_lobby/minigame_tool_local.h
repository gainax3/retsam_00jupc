//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		minigame_tool_local.h
 *	@brief		ミニゲームツールローカルヘッダ
 *	@author		tomoya takahshi
 *	@data		2007.11.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __MINIGAME_TOOL_LOCAL_H__
#define __MINIGAME_TOOL_LOCAL_H__

#include "application/wifi_lobby/minigame_tool.h"

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

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

extern void MNGM_ENTRY_SetRareGame( MNGM_ENTRYWK* p_wk,  MNGM_RAREGAME_TYPE type );


extern void MNGM_RESULT_SetKoRetry( MNGM_RESULTWK* p_wk, u32 netid,  BOOL retry );
extern void MNGM_RESULT_SetOyaRetry( MNGM_RESULTWK* p_wk, BOOL retry );
#endif		// __MINIGAME_TOOL_LOCAL_H__

