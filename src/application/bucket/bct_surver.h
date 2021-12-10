//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		bct_surver.h
 *	@brief		サーバー
 *	@author		tomoya takahashi
 *	@data		2007.06.19
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __BCT_SURVER_H__
#define __BCT_SURVER_H__

#include "bct_common.h"

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
///	サーバーシステム
//=====================================
typedef struct _BCT_SURVER	BCT_SURVER;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
extern BCT_SURVER* BCT_SURVER_Init( u32 heapID, u32 timeover, u32 comm_num, const BCT_GAMEDATA* cp_gamedata );
extern void BCT_SURVER_Delete( BCT_SURVER* p_wk );

// メイン関数
// FALSEが帰ってきたら終了
// 終了命令とスコアをみんなに送る
extern BOOL BCT_SURVER_Main( BCT_SURVER* p_wk );

// ゲームレベルの変更をチェック
extern BOOL BCT_SURVER_CheckGameLevelChange( const BCT_SURVER* cp_wk );	// 変更があったか？
extern void BCT_SURVER_ClearGameLevelChange( BCT_SURVER* p_wk );		// フラグをクリア
extern u32	BCT_SURVER_GetGameLevel( const BCT_SURVER* cp_wk );			// 今のレベルを取得

// 木の実が入った情報を設定
extern void BCT_SURVER_SetNutData( BCT_SURVER* p_wk, const BCT_NUT_COMM* cp_data, u32 plno );

//  みんなのスコアを収集
extern void BCT_SURVER_ScoreSet( BCT_SURVER* p_wk, u32 score, u32 plno );
extern BOOL BCT_SURVER_ScoreAllUserGetCheck( const BCT_SURVER* cp_wk );
extern void BCT_SURVER_ScoreGet( BCT_SURVER* p_wk, BCT_SCORE_COMM* p_data );

// カウントダウンを進めるか設定
extern void BCT_SURVER_SetCountDown( BCT_SURVER* p_wk, BOOL flag );

#endif		// __BCT_SURVER_H__

