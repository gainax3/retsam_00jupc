//==============================================================================
/**
 * @file	balloon.h
 * @brief	風船割りのヘッダ
 * @author	matsuda
 * @date	2007.11.06(火)
 */
//==============================================================================
#ifndef __BALLOON_H__
#define __BALLOON_H__

// とりあえずここに入れます、松田さんの書き方じゃないなら消してください。
#include "application/wifi_lobby/minigame_tool.h"

//==============================================================================
//	構造体定義
//==============================================================================
//--------------------------------------------------------------
/**
 *	風船割りゲームプロセスワーク(parent_work)
 */
//--------------------------------------------------------------
typedef struct {
	WFLBY_MINIGAME_WK lobby_wk;
	SAVEDATA* p_save;
	u8 vchat;
	u8 wifi_lobby;
	u8 pad[2];
	
#ifdef PM_DEBUG
	int debug_offline;		///<TRUE:デバッグ用のオフラインモード
#endif
}BALLOON_PROC_WORK;


//==============================================================================
//	外部関数宣言
//==============================================================================
extern PROC_RESULT BalloonProc_Init( PROC * proc, int * seq );
extern PROC_RESULT BalloonProc_Main( PROC * proc, int * seq );
extern PROC_RESULT BalloonProc_End(PROC *proc, int *seq);


#endif		// __BALLOON_H__

