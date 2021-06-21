//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifi_p2pmatchfour.h
 *	@brief		wifi４人マッチング専用画面
 *	@author		tomoya takahashi
 *	@data		2007.05.22
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WIFI_P2PMATCHFOUR_H__
#define __WIFI_P2PMATCHFOUR_H__

#include "system/procsys.h"
#include "application/wifi_p2pmatch_def.h"

#undef GLOBAL
#ifdef	__WIFI_P2PMATCHFOUR_H_GLOBAL
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
///	マッチングタイプ
//=====================================
enum {
	WFP2PMF_TYPE_POFIN,				// ポフィン
	WFP2PMF_TYPE_BUCKET,			// たまいれ
	WFP2PMF_TYPE_BALANCE_BALL,		// たまのり	
	WFP2PMF_TYPE_BALLOON,			// ふうせんわり
	WFP2PMF_TYPE_NUM,
} ;

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	マッチング内容　構造体
//=====================================
typedef struct {
	u8 type;		// マッチングタイプ
	u8 comm_min;	// 通信最低人数
	u8 comm_max;	// 通信最大人数

	u8 result;		// 正常に終了したのか、マッチング画面に戻るのか
	u8 vchat;		// VCHAT ON/OFF
	u8 pad[3];
	
	SAVEDATA*  p_savedata;

} WFP2PMF_INIT;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
GLOBAL PROC_RESULT WifiP2PMatchFourProc_Init( PROC * proc, int * seq );
GLOBAL PROC_RESULT WifiP2PMatchFourProc_Main( PROC * proc, int * seq );
GLOBAL PROC_RESULT WifiP2PMatchFourProc_End( PROC * proc, int * seq );


#undef	GLOBAL
#endif		// __WIFI_P2PMATCHFOUR_H__

