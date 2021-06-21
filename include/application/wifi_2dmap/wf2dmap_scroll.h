//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wf2dmap_scroll.h
 *	@brief		スクロールデータ管理+スクロール座標管理モジュール
 *	@author		tomoya takahashi
 *	@data		2007.03.19
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WF2DMAP_SCROLL_H__
#define __WF2DMAP_SCROLL_H__

#include "application/wifi_2dmap/wf2dmap_common.h"
#include "application/wifi_2dmap/wf2dmap_map.h"
#include "application/wifi_2dmap/wf2dmap_obj.h"
#include "application/wifi_2dmap/wf2dmap_scroll.h"

#undef GLOBAL
#ifdef	__WF2DMAP_SCROLL_H_GLOBAL
#define	GLOBAL	/* */
#else
#define	GLOBAL	extern
#endif

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
///	スクロール情報
//=====================================
typedef struct _WF2DMAP_SCROLL {
	s16 draw_top;		// 表示矩形上座標
	s16 draw_left;		// 表示矩形左座標
}WF2DMAP_SCROLL;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

// スクロール管理関数
GLOBAL void WF2DMAP_SCRContSysMain( WF2DMAP_SCROLL* p_data, const WF2DMAP_OBJWK* cp_objwk );



// データ設定＆取得
GLOBAL void WF2DMAP_SCROLLSysDataInit( WF2DMAP_SCROLL* p_sys );
GLOBAL void WF2DMAP_SCROLLSysDataSet( WF2DMAP_SCROLL* p_sys, s16 draw_top, s16 draw_left );
GLOBAL s16 WF2DMAP_SCROLLSysTopGet( const WF2DMAP_SCROLL* cp_sys );
GLOBAL s16 WF2DMAP_SCROLLSysLeftGet( const WF2DMAP_SCROLL* cp_sys );
GLOBAL void WF2DMAP_SCROLLSysTopAdd( WF2DMAP_SCROLL* p_sys, s16 add );
GLOBAL void WF2DMAP_SCROLLSysLeftAdd( WF2DMAP_SCROLL* p_sys, s16 add );


 
#undef	GLOBAL
#endif		// __WF2DMAP_SCROLL_H__

