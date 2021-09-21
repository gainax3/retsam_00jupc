//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		comm_command_wfp2pmf_func.h
 *	@brief		２～４人専用待合室	通信コマンド
 *	@author		tomoya takahashi
 *	@data		2007.05.24
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __COMM_COMMAND_WFP2PMF_FUNC_H__
#define __COMM_COMMAND_WFP2PMF_FUNC_H__


#include "wifi_p2pmatchfour_local.h"

#undef GLOBAL
#ifdef	__COMM_COMMAND_WFP2PMF_FUNC_H_GLOBAL
#define	GLOBAL	/* */
#else
#define	GLOBAL	extern
#endif

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
extern void CommCommandWFP2PMFInitialize( WFP2P_WK* p_wk );
extern void CommCommandWFP2PMF_MatchStartInitialize( void );	// wifi_p2pmatch.c内でコマンドを設定するときの関数

// 共通サイズ取得関数
extern int CommWFP2PMFGetZeroSize( void );
extern int CommWFP2PMFGetWFP2PMF_COMM_RESULTSize( void );
extern int CommWFP2PMFGetWFP2PMF_COMM_VCHATSize( void );

// 親からのリクエスト
extern void CommWFP2PMFGameResult( int netID, int size, void* pBuff, void* pWork );
extern void CommWFP2PMFGameStart( int netID, int size, void* pBuff, void* pWork );
extern void CommWFP2PMFGameVchat( int netID, int size, void* pBuff, void* pWork );



#undef	GLOBAL
#endif		// __COMM_COMMAND_WFP2PMF_FUNC_H__

