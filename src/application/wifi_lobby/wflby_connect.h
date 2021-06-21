//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_connect.h
 *	@brief		Wi-Fiロビーに接続	切断処理プロック
 *	@author		tomoya takahashi
 *	@data		2007.12.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WFLBY_CONNECT_H__
#define __WFLBY_CONNECT_H__

#include "system/procsys.h"

#include "savedata/savedata_def.h"

#include "field/wflby_counter.h"

#include "wflby_system_def.h"

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
///	パラメータ
//=====================================
typedef struct {
	SAVEDATA* p_save;
	WFLBY_SYSTEM* p_system;	
	BOOL	check_skip;
	WFLBY_COUNTER_TIME* p_wflby_counter;	// WiFiひろば入室時間格納先

	// 中で中に入ったならＴＲＵＥが入る
	BOOL enter;

} WFLBY_CONNECT_PARAM;

//-------------------------------------
///	退室パラメータ
//=====================================
typedef struct {
	SAVEDATA* p_save;
	WFLBY_SYSTEM* p_system;	
	BOOL timeout;

} WFLBY_DISCONNECT_PARAM;



//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

extern PROC_RESULT WFLBY_CONNECT_Init(PROC* p_proc, int* p_seq);
extern PROC_RESULT WFLBY_CONNECT_Main(PROC* p_proc, int* p_seq);
extern PROC_RESULT WFLBY_CONNECT_Exit(PROC* p_proc, int* p_seq);

extern PROC_RESULT WFLBY_DISCONNECT_Init(PROC* p_proc, int* p_seq);
extern PROC_RESULT WFLBY_DISCONNECT_Main(PROC* p_proc, int* p_seq);
extern PROC_RESULT WFLBY_DISCONNECT_Exit(PROC* p_proc, int* p_seq);


#endif		// __WFLBY_CONNECT_H__

