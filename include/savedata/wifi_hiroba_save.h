//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wifi_hiroba_save.h
 *	@brief		Wi-Fiひろば	セーブデータ
 *	@author	
 *	@data		2008.05.22
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WIFI_HIROBA_SAVE_H__
#define __WIFI_HIROBA_SAVE_H__

// savedata
#include "savedata/savedata_def.h"	//SAVEDATA参照のため
#include "savedata/savedata.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
#define WFLBY_SAVEDATA_QUESTION_NONE	( 0xffffffff )	// 無効アンケート通し番号

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	セーブデータ
//=====================================
typedef struct _WFLBY_SAVEDATA WFLBY_SAVEDATA;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
// セーブデータシステムが依存する関数
extern int SAVEDATA_WFLBY_GetWorkSize( void );
extern void SAVEDATA_WFLBY_InitWork( WFLBY_SAVEDATA* p_wk );
extern WFLBY_SAVEDATA* SAVEDATA_WFLBY_AllocWork( u32 heapID );

// セーブデータワーク取得
extern WFLBY_SAVEDATA* SAVEDATA_WFLBY_GetWork( SAVEDATA* p_sv );

// データ取得
extern u32 SAVEDATA_WFLBY_GetQuestionNumber( const WFLBY_SAVEDATA* cp_wk );
extern u32 SAVEDATA_WFLBY_GetMyAnswer( const WFLBY_SAVEDATA* cp_wk );

// データ設定
extern void SAVEDATA_WFLBY_SetQuestionNumber( WFLBY_SAVEDATA* p_wk, u32 number );
extern void SAVEDATA_WFLBY_SetMyAnswer( WFLBY_SAVEDATA* p_wk, u32 answer );


#endif		// __WIFI_HIROBA_SAVE_H__

