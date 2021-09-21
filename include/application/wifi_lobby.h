//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wifi_lobby.h
 *	@brief		WiFiロビーメインシステム
 *	@author		tomoya takahashi
 *	@data		2007.09.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WIFI_LOBBY_H__
#define __WIFI_LOBBY_H__

#include "system/procsys.h"
#include "savedata/savedata_def.h"
#include "field/wflby_counter.h"

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
///	プロックパラメータ
//=====================================
typedef struct {
	WFLBY_COUNTER_TIME* p_wflby_counter;	// WiFiひろば入室時間格納先
	SAVEDATA*			p_save;				// セーブデータ
	BOOL				check_skip;			// 接続確認をスキップする　TRUE：スキップ	FALSE：通常
} WFLBY_PROC_PARAM;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
extern PROC_RESULT WFLBYProc_Init( PROC* p_proc, int* p_seq );
extern PROC_RESULT WFLBYProc_Main( PROC* p_proc, int* p_seq );
extern PROC_RESULT WFLBYProc_Exit( PROC* p_proc, int* p_seq );



#endif		// __WIFI_LOBBY_H__

