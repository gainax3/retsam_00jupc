//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		bct_cllient.h
 *	@brief		クライアント
 *	@author		tomoya takahashi
 *	@data		2007.06.19
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __BCT_CLLIENT_H__
#define __BCT_CLLIENT_H__

#include "bct_common.h" 

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	開始動作イベント
//=====================================
enum {
	BCT_CLIENT_STARTEVENT_NONE,
	BCT_CLIENT_STARTEVENT_COUNT_START,
	BCT_CLIENT_STARTEVENT_NUM,
} ;

//-------------------------------------
///	終了動作イベント
//=====================================
enum {
	BCT_CLIENT_ENDEVENT_NONE,
	BCT_CLIENT_ENDEVENT_RESULTON,	// 結果発表を始める
	BCT_CLIENT_ENDEVENT_NUM,
} ;

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	クライアントワーク
//=====================================
typedef struct _BCT_CLIENT BCT_CLIENT;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
extern BCT_CLIENT* BCT_CLIENT_Init( u32 heapID, u32 timeover, u32 comm_num, u32 plno, BCT_GAMEDATA* cp_gamedata );
extern void BCT_CLIENT_Delete( BCT_CLIENT* p_wk );

extern BOOL BCT_CLIENT_StartMain( BCT_CLIENT* p_wk, u32 event );
extern BOOL BCT_CLIENT_EndMain( BCT_CLIENT* p_wk, u32 event );
extern BOOL BCT_CLIENT_Main( BCT_CLIENT* p_wk );
extern void BCT_CLIENT_VBlank( BCT_CLIENT* p_wk );
extern s32	BCT_CLIENT_GetTime( const BCT_CLIENT* cp_wk );

extern void BCT_CLIENT_NutsDataSet( BCT_CLIENT* p_wk, const BCT_NUT_COMM* cp_data );
extern BOOL BCT_CLIENT_NutsDataGet( BCT_CLIENT* p_wk, BCT_NUT_COMM* p_data );
extern void BCT_CLIENT_AllScoreSet( BCT_CLIENT* p_wk, const BCT_SCORE_COMM* cp_data );
extern u32 BCT_CLIENT_ScoreGet( const BCT_CLIENT* cp_wk );
extern void BCT_CLIENT_GameDataIdxSet( BCT_CLIENT* p_wk, u32 idx );
extern void BCT_CLIENT_MiddleScoreSet( BCT_CLIENT* p_wk, u32 score, u32 plno );
extern void BCT_CLIENT_GameTypeSet( BCT_CLIENT* p_wk, const BCT_GAME_TYPE_WK* cp_data );
extern BOOL BCT_CLIENT_GameTypeSetCheck( const BCT_CLIENT* cp_wk );

extern void BCT_CLIENT_TimeCountFlagSet( BCT_CLIENT* p_wk, BOOL flag );


#endif		// __BCT_CLLIENT_H__

