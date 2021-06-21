//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wf2dmap_cmdq.h
 *	@brief		コマンドキュー
 *	@author		tomoya takahashi
 *	@data		2007.03.28
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WF2DMAP_CMDQ_H__
#define __WF2DMAP_CMDQ_H__

#include "application/wifi_2dmap/wf2dmap_common.h"

#undef GLOBAL
#ifdef	__WF2DMAP_CMDQ_H_GLOBAL
#define	GLOBAL	/* */
#else
#define	GLOBAL	extern
#endif

//-----------------------------------------------------------------------------
/**
 *		アクションコマンド	キュー
 *		（あったら便利なので作成しました）
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	アクションコマンド	キュー
//=====================================
typedef struct _WF2DMAP_ACTCMDQ WF2DMAP_ACTCMDQ;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
//　システムワーク
GLOBAL WF2DMAP_ACTCMDQ* WF2DMAP_ACTCMDQSysInit( u32 buffnum, u32 heapID );
GLOBAL void WF2DMAP_ACTCMDQSysExit( WF2DMAP_ACTCMDQ* p_sys );

// アクションコマンドを設定
GLOBAL void WF2DMAP_ACTCMDQSysCmdPush( WF2DMAP_ACTCMDQ* p_sys, const WF2DMAP_ACTCMD* cp_buff );
// コマンドデータ取得 
GLOBAL BOOL WF2DMAP_ACTCMDQSysCmdPop( WF2DMAP_ACTCMDQ* p_sys, WF2DMAP_ACTCMD* p_cmd );
// バッファ数取得
GLOBAL u32 WF2DMAP_ACTCMDQSysBuffNumGet( const WF2DMAP_ACTCMDQ* cp_sys );



//-----------------------------------------------------------------------------
/**
 *		リクエストコマンドキュー
 *		（あったら便利なので作成しました）
 */
//-----------------------------------------------------------------------------

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
//-------------------------------------
///	リクエストコマンドキュー
//=====================================
typedef struct _WF2DMAP_REQCMDQ WF2DMAP_REQCMDQ;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
//　システムワーク
GLOBAL WF2DMAP_REQCMDQ* WF2DMAP_REQCMDQSysInit( u32 buffnum, u32 heapID );
GLOBAL void WF2DMAP_REQCMDQSysExit( WF2DMAP_REQCMDQ* p_sys );

// リクエストコマンドを設定
GLOBAL void WF2DMAP_REQCMDQSysCmdPush( WF2DMAP_REQCMDQ* p_sys, const WF2DMAP_REQCMD* cp_cmd );
// コマンドデータ取得 
GLOBAL BOOL WF2DMAP_REQCMDQSysCmdPop( WF2DMAP_REQCMDQ* p_sys, WF2DMAP_REQCMD* p_cmd );

// バッファ数取得
GLOBAL u32 WF2DMAP_REQCMDQSysBuffNumGet( const WF2DMAP_REQCMDQ* cp_sys );

#undef	GLOBAL
#endif		// __WF2DMAP_CMDQ_H__

