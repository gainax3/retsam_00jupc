//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		bucket.h
 *	@brief		バケットゲーム
 *	@author		tomoya takahashi
 *	@data		2007.06.19
 *															
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __BUCKET_H__
#define __BUCKET_H__

#include "savedata/savedata_def.h"

#include "system/procsys.h"

#include "application/wifi_lobby/minigame_tool.h"


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
///	バケットゲームプロセスワーク
//=====================================
typedef struct {
	WFLBY_MINIGAME_WK	lobby_wk;		//  ここにガジェットの状態が入っています。ゲーム後はこの値を基本情報に設定します。
	SAVEDATA*		p_save;
	u8 vchat;
	u8 wifi_lobby;
	u8 pad[2];
} BUCKET_PROC_WORK;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

extern PROC_RESULT BucketProc_Init( PROC * p_proc, int * p_seq );
extern PROC_RESULT BucketProc_Main( PROC* p_proc, int* p_seq );
extern PROC_RESULT BucketProc_End( PROC* p_proc, int* p_seq );


#endif		// __BUCKET_H__

