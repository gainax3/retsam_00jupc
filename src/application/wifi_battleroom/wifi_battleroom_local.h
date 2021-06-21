//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifi_battleroom_local.h
 *	@brief		wifi バトルルームローカルヘッダ
 *	@author		tomoya takahashi
 *	@data		2007.02.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WIFI_BATTLEROOM_LOCAL_H__
#define __WIFI_BATTLEROOM_LOCAL_H__

#include "include/field/field.h"
#include "wbr_common.h"
#include "wbr_surver.h"
#include "wbr_client.h"


#undef GLOBAL
#ifdef	__WIFI_BATTLEROOM_LOCAL_H_GLOBAL
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


//-------------------------------------
///		wifi バトルルームシステム
//=====================================
typedef struct _WIFI_BATTLEROOM {

	FIELDSYS_WORK* p_fsys;
	
	// 現在の状態
	u32 status;
	
	u32	netid;	// 自分のNETID
	
	// 親データ
	WBR_SURVER* p_surver;

	// 子データ
	WBR_CLIENT* p_client;

	// マネージャのタスク
	TCB_PTR tcb;

	// ステータス
	WBR_MYSTATUS	mystatus;
}WIFI_BATTLEROOM;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
#undef	GLOBAL
#endif		// __WIFI_BATTLEROOM_LOCAL_H__

