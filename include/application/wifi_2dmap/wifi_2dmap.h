//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifi_2dmap.h
 *	@brief		マップ当たり判定管理データ
 *	@author		tomoya takahashi
 *	@data		2007.03.05
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WIFI_2DMAP_H__
#define __WIFI_2DMAP_H__

#include "application/wifi_2dmap/wifi_2dcommon.h"

#undef GLOBAL
#ifdef	__WIFI_2DMAP_H_GLOBAL
#define	GLOBAL	/* */
#else
#define	GLOBAL	extern
#endif

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
#define		WF_MAP_AREAOVER_MAP		(0xffffffff)	// 範囲外のマップ

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	マップ管理システム
//=====================================
typedef struct _WF_MAP_SYS WF_MAP_SYS;


//-------------------------------------
///	管理マップデータ
//=====================================
typedef union {
	u32	data;
	struct {
		u16 flag;	// マップフラグ
		u8 param;	// パラメータ
		u8 event;	// イベントデータ
	};
} WF_MAP;
#define WF_MAP_PARAM_SHIFT	(16)	// マップパラメータシフト
#define WF_MAP_EVENT_SHIFT	(24)	// マップイベントシフト

/*
 *	管理マップデータの説明
 *		flag	当たり判定に使用する変数です。
 *				今そこに人がいるとか、ここには壁があるなどをチェックするときに使用します。
 *		param	ぶつかったものを判別するためのパラメータです。
 *				人との当たり判定のときは、paramにその人を識別する数字を入れておいて、
 *				ぶつかった人を特定します。
 *		event	主人公がその上に乗ったときに何かを起こしたい時に使用します。
 *				出口や、何かわななどを識別する数字を入れておき、乗っかったかチェックした
 *				結果により何かイベントを発行してください。
 *
 *		data	初期化するためのマップデータを作成するときやコピーするときに使用します。
 *				flag/param/eventをまとめてコピーすることができます。
 */

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

// システム作成破棄
GLOBAL WF_MAP_SYS* WF_MAP_SysInit( u16 xgrid, u16 ygrid, u32 heapID );
GLOBAL void WF_MAP_SysExit( WF_MAP_SYS* p_sys );

// 初期化マップデータで内部バッファを初期化する
GLOBAL void WF_MAP_SysDataSet( WF_MAP_SYS* p_sys, const WF_MAP* cp_def );

// データ設定取得
GLOBAL WF_MAP WF_MAP_DataGet( const WF_MAP_SYS* cp_sys, u16 x, u16 y );
GLOBAL void WF_MAP_DataSet( WF_MAP_SYS* p_sys, u16 x, u16 y, u16 mapflag, u8 param );
GLOBAL void WF_MAP_EventSet( WF_MAP_SYS* p_sys, u16 x, u16 y, u8 event );


#undef	GLOBAL
#endif		// __WIFI_2DMAP_H__

