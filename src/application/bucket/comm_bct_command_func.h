//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		comm_bct_command_func.h
 *	@brief		バケットミニゲーム　通信コマンド
 *	@author		tomoya takahashi
 *	@data		2007.06.20
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __COMM_BCT_COMMAND_FUNC_H__
#define __COMM_BCT_COMMAND_FUNC_H__

#include "bct_local.h"

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
extern void CommCommandBCTInitialize( BUCKET_WK* p_wk );

// 共通サイズ取得関数
extern int CommBCTGetZeroSize( void );
extern int CommBCTGetNutsSize( void );
extern int CommBCTGetAllScoreSize( void );
extern int CommBCTGetScoreSize( void );
extern int CommBCTGetGameDataIdxSize( void );

// 親からのリクエスト
extern void CommBCTGameStart( int netID, int size, void* pBuff, void* pWork );
extern void CommBCTGameEnd( int netID, int size, void* pBuff, void* pWork );

// 木の実データ通信
extern void CommBCTNuts( int netID, int size, void* pBuff, void* pWork );

// スコア通信
extern void CommBCTScore( int netID, int size, void* pBuff, void* pWork );
extern void CommBCTAllScore( int netID, int size, void* pBuff, void* pWork );
extern void CommBCTMiddleScore( int netID, int size, void* pBuff, void* pWork );
extern void CommBCTMiddleScoreOk( int netID, int size, void* pBuff, void* pWork );

// ゲームレベル
extern void CommBCTGameDataIdx( int netID, int size, void* pBuff, void* pWork );



#endif		// __COMM_BCT_COMMAND_FUNC_H__

