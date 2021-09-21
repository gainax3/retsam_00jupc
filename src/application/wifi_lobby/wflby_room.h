//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_room.h
 *	@brief		WiFiロビー	部屋システム
 *	@author		tomoya takahashi
 *	@data		2007.11.01
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WFLBY_ROOM_H__
#define __WFLBY_ROOM_H__

#include "savedata/savedata_def.h"
#include "application/wifi_lobby/wflby_def.h"
#include "system/procsys.h"
#include "application/wifi_lobby/wflby_system_def.h"


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	部屋戻り値
//=====================================
typedef enum {
	WFLBY_ROOM_RET_ROOM_OUT,			// 出口から出た
	WFLBY_ROOM_RET_ROOM_WORLDTIMER,		// 世界時計へ
	WFLBY_ROOM_RET_ROOM_TOPIC,			// トピックへ
	WFLBY_ROOM_RET_ROOM_BALLSLOW,		// 玉投げ
	WFLBY_ROOM_RET_ROOM_BALANCE,		// 玉乗り
	WFLBY_ROOM_RET_ROOM_BALLOON,		// 風船割り
	WFLBY_ROOM_RET_ROOM_FOOT1,			// 足跡ボード白
	WFLBY_ROOM_RET_ROOM_FOOT2,			// 足跡ボード黒
	WFLBY_ROOM_RET_ROOM_TIMEOUT,		// 部屋が終わった

	WFLBY_ROOM_RET_ROOM_ANKETO_INPUT,	// アンケート　入力
	WFLBY_ROOM_RET_ROOM_ANKETO_OUTPUT,	// アンケート　出力

#ifdef PM_DEBUG
	WFLBY_ROOM_RET_ROOM_RETURN,			// 部屋にもう一度戻る
#endif
} WFLBY_ROOM_RET;



//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	WiFiロビー部屋　セーブデータ
//=====================================
typedef struct {
	u16	pl_gridx;	// プレイヤー登場グリッドｘ
	u16	pl_gridy;	// プレイヤー登場グリッドｙ
	u16 pl_way;		// プレイヤー登場方向
	u16 pl_inevno;	// プレイヤー入場イベントナンバー
	u16	minigame;	// 遊びに行く（行った）ミニゲームナンバー
	u16	pad;
} WFLBY_ROOM_SAVE;


//-------------------------------------
///	パラメータ構造体
//=====================================
typedef struct {
	// ずっと一緒のデータ
	SAVEDATA*			p_save;
	WFLBY_SEASON_TYPE	season;
	WFLBY_ROOM_TYPE		room;

	// 部屋内部で使用する保存データ
	WFLBY_ROOM_SAVE		save;	// 初期値は全部０にしてください
	
	// roomシステムから上に返答するデータ
	WFLBY_ROOM_RET		ret;

	// おそらく基本情報など
	WFLBY_SYSTEM*		p_system;	// LOBBY情報
} WFLBY_ROOM_PARAM;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
extern PROC_RESULT WFLBY_ROOM_Init(PROC* p_proc, int* p_seq);
extern PROC_RESULT WFLBY_ROOM_Main(PROC* p_proc, int* p_seq);
extern PROC_RESULT WFLBY_ROOM_Exit(PROC* p_proc, int* p_seq);

#ifdef PM_DEBUG
extern PROC_RESULT WFLBY_ROOM_InitDebug(PROC* p_proc, int* p_seq);
extern PROC_RESULT WFLBY_ROOM_ExitDebug(PROC* p_proc, int* p_seq);
#endif

#endif		// __WFLBY_ROOM_H__

