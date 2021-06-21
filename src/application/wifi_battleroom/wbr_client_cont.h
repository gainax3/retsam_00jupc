//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wbr_client_cont.h
 *	@brief		クライアントオブジェクト管理システム
 *	@author		tomoya takahashi
 *	@data		2007.04.19
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WBR_CLIENT_CONT_H__
#define __WBR_CLIENT_CONT_H__

#include "system/procsys.h"
#include "wbr_client_local.h"
#include "wbr_common.h"

#undef GLOBAL
#ifdef	__WBR_CLIENT_CONT_H_GLOBAL
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
///	上に返す次のシーケンスフラグ
//=====================================
typedef enum {
	WBR_CLIENTCONT_RET_NONE,
	WBR_CLIENTCONT_RET_END,
	WBR_CLIENTCONT_RET_TRCARD,
} WBR_CLIENTCONT_RETTYPE;

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

GLOBAL PROC_RESULT WBR_ClientCont_Init( PROC* p_proc, int* p_seq );
GLOBAL PROC_RESULT WBR_ClientCont_Main( PROC* p_proc, int* p_seq );
GLOBAL PROC_RESULT WBR_ClientCont_End( PROC* p_proc, int* p_seq );

GLOBAL void WBR_ClientCont_SetOyaGameData( void* p_procw, const WBR_OYA_COMM_GAME* cp_oyadata );
GLOBAL void WBR_ClientCont_SetTalkData( void* p_procw, const WBR_KO_COMM_TALK* cp_talkdata );
GLOBAL void WBR_ClientCont_SetEndFlag( void* p_procw );

#undef	GLOBAL
#endif		// __WBR_CLIENT_CONT_H__

