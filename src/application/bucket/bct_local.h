//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		bct_local.h
 *	@brief		BCTのローカル構造体
 *	@author		tomoya takahashi
 *	@data		2007.06.20
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __BCT_LOCAL_H__
#define __BCT_LOCAL_H__

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
typedef struct _BUCKET_WK BUCKET_WK;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
extern void Bucket_StartSet( BUCKET_WK* p_wk );
extern void Bucket_EndSet( BUCKET_WK* p_wk );
extern void Bucket_SurverNutsSet( BUCKET_WK* p_wk, const BCT_NUT_COMM* cp_data, u32 netid );
extern void Bucket_ClientNutsSet( BUCKET_WK* p_wk, const BCT_NUT_COMM* cp_data, u32 netid );
extern void Bucket_SurverScoreSet( BUCKET_WK* p_wk, u32 score, u32 netid );
extern void Bucket_ClientAllScoreSet( BUCKET_WK* p_wk, const BCT_SCORE_COMM* cp_data, u32 netid );
extern void Bucket_ClientGameDataIdxSet( BUCKET_WK* p_wk, u32 idx );
extern void Bucket_ClientMiddleScoreSet( BUCKET_WK* p_wk, u32 score, u32 netid );
extern void Bucket_ClientMiddleScoreOkSet( BUCKET_WK* p_wk );


#endif		// __BCT_LOCAL_H__

