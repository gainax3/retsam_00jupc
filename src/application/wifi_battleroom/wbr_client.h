//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wbr_client.h
 *	@brief		クライアントシステム
 *	@author		tomoya takahashi
 *	@data		2007.02.22
 *
 *	歩き回り描画PROCや
 *	その他PROCに分岐する処理を管理
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WBR_CLIENT_H__
#define __WBR_CLIENT_H__

#include "wbr_common.h"
#include "include/field/field.h"

#undef GLOBAL
#ifdef	__WBR_CLIENT_H_GLOBAL
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
///	状態
//=====================================
enum {
	WBR_CLIENT_SEQ_START_WAIT,		// 親からの開始命令待ち
	WBR_CLIENT_SEQ_COMMDATA_SEND,	// 共通データ送信
	WBR_CLIENT_SEQ_COMMDATA_WAIT,	// 共通データ受信待ち
	WBR_CLIENT_SEQ_WALK_START,
	WBR_CLIENT_SEQ_WALK_ENDWAIT,
	WBR_CLIENT_SEQ_TRCARD_START,
	WBR_CLIENT_SEQ_TRCARD_ENDWAIT,
	WBR_CLIENT_SEQ_END,
} ;
//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	クライアントワーク
//=====================================
typedef struct _WBR_CLIENT WBR_CLIENT;

//-------------------------------------
/// クライアントシステム初期化データ
//=====================================
typedef struct {
	u32 netid;
	const WBR_MYSTATUS* cp_mystatus;
	FIELDSYS_WORK* p_fsys;
} WBR_CLIENT_INIT;



//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
GLOBAL WBR_CLIENT* WBR_ClientInit( const WBR_CLIENT_INIT* p_init, u32 heapID );
GLOBAL void WBR_ClientExit( WBR_CLIENT* p_sys );

GLOBAL BOOL WBR_ClientMain( WBR_CLIENT* p_sys );

GLOBAL void WBR_ClientSeqSet( WBR_CLIENT* p_sys, u32 seq );
GLOBAL void WBR_ClientCommDataSet( WBR_CLIENT* p_sys, const WBR_COMM_COMMON* cp_data );
GLOBAL void WBR_ClientGameDataSet( WBR_CLIENT* p_sys, const WBR_OYA_COMM_GAME* cp_data );
GLOBAL void WBR_ClientTalkDataSet( WBR_CLIENT* p_sys, const WBR_KO_COMM_TALK* cp_data );
GLOBAL void WBR_ClientGameEndSet( WBR_CLIENT* p_sys );

GLOBAL u8* WBR_ClientCommDataBuffPtrGet( WBR_CLIENT* p_sys, u32 netid );

#undef	GLOBAL
#endif		// __WBR_CLIENT_H__

