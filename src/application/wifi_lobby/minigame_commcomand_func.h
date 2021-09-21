//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		minigame_commcomand_func.h
 *	@brief		ミニゲームツール通信コマンド
 *	@author		tomoya takahashi
 *	@data		2007.11.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __MINIGAME_COMMCOMAND_FUNC_H__
#define __MINIGAME_COMMCOMAND_FUNC_H__

#include "minigame_tool_local.h"

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
// 通信初期化関数
extern void CommCommandMNGMInitialize( MNGM_RESULTWK* p_wk );
extern void CommCommandMNGMEntryInitialize( MNGM_ENTRYWK* p_wk );

// 共通サイズ取得関数
extern int CommMNGMGetZeroSize( void );
extern int CommMNGMGetu32Size( void );

// 親ー＞子　Rareゲームタイプ
extern void CommMNGMRareGame( int netID, int size, void* pBuff, void* pWork );


// 子ー>親	もういちど
extern void CommMNGMRetryYes( int netID, int size, void* pBuff, void* pWork );
// 子ー>親	もうやらない
extern void CommMNGMRetryNo( int netID, int size, void* pBuff, void* pWork );

// 親ー>子	もういちど
extern void CommMNGMRetryOk( int netID, int size, void* pBuff, void* pWork );
// 親ー>子	もうやらない
extern void CommMNGMRetryNg( int netID, int size, void* pBuff, void* pWork );

#endif		// __MINIGAME_COMMCOMAND_FUNC_H__

