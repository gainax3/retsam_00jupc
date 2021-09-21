//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_anketo.h
 *	@brief		アンケート画面
 *	@author		tomoya takahashi
 *	@data		2008.05.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WFLBY_ANKETO_H__
#define __WFLBY_ANKETO_H__


#include "savedata/savedata_def.h"

#include "system/procsys.h"

#include "application/wifi_lobby/wflby_def.h"
#include "application/wifi_lobby/wflby_system_def.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	動作タイプ
//=====================================
typedef enum{
	ANKETO_MOVE_TYPE_INPUT,	// 入力
	ANKETO_MOVE_TYPE_VIEW,	// 見た目

	ANKETO_MOVE_TYPE_NUM,	// 数
} ANKETO_MOVE_TYPE;

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	アンケートデータ
//=====================================
typedef struct {
	SAVEDATA*				p_save;		// セーブデータ
	WFLBY_SYSTEM*			p_system;
	ANKETO_MOVE_TYPE		move_type;	// 動作タイプ
} ANKETO_PARAM;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

extern PROC_RESULT ANKETO_Init(PROC* p_proc, int* p_seq);
extern PROC_RESULT ANKETO_Main(PROC* p_proc, int* p_seq);
extern PROC_RESULT ANKETO_Exit(PROC* p_proc, int* p_seq);

#endif		// __WFLBY_ANKETO_H__

