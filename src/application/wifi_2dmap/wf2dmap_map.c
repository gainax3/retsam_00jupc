//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wf2dmap_map.c
 *	@brief		マップデータ管理システム
 *	@author		tomoya takahashi
 *	@data		2007.03.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#define __WF2DMAP_MAP_H_GLOBAL
#include "application/wifi_2dmap/wf2dmap_map.h"

//-----------------------------------------------------------------------------
/**
 *					コーディング規約
 *		●関数名
 *				１文字目は大文字それ以降は小文字にする
 *		●変数名
 *				・変数共通
 *						constには c_ を付ける
 *						staticには s_ を付ける
 *						ポインタには p_ を付ける
 *						全て合わさると csp_ となる
 *				・グローバル変数
 *						１文字目は大文字
 *				・関数内変数
 *						小文字と”＿”と数字を使用する 関数の引数もこれと同じ
*/
//-----------------------------------------------------------------------------
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
///	マップデータ管理システムワーク
//=====================================
typedef struct _WF2DMAP_MAPSYS {
	u16 xgrid;
	u16 ygrid;
	WF2DMAP_MAP* p_buff;
}WF2DMAP_MAPSYS;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/**
 *	@brief	マップデータ管理システム初期化
 *
 *	@param	xgrid		ｘグリッド数
 *	@param	ygrid		ｙグリッド数
 *	@param	heapID		使用ヒープ
 *
 *	@return	作成したマップシステム
 */
//-----------------------------------------------------------------------------
WF2DMAP_MAPSYS* WF2DMAP_MAPSysInit( u16 xgrid, u16 ygrid, u32 heapID )
{
	WF2DMAP_MAPSYS* p_sys;

	// システム作成
	p_sys = sys_AllocMemory( heapID, sizeof(WF2DMAP_MAPSYS) );
	GF_ASSERT( p_sys );

	// システムデータ作成
	p_sys->xgrid = xgrid;
	p_sys->ygrid = ygrid;

	p_sys->p_buff = sys_AllocMemory( heapID, sizeof(WF2DMAP_MAP)*(p_sys->xgrid*p_sys->ygrid) );
	GF_ASSERT( p_sys->p_buff );
	memset( p_sys->p_buff, 0, sizeof(WF2DMAP_MAP)*(p_sys->xgrid*p_sys->ygrid) );
	
	return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	マップデータ管理システム破棄
 *	
 *	@param	p_sys	マップ管理システム
 */
//-----------------------------------------------------------------------------
void WF2DMAP_MAPSysExit( WF2DMAP_MAPSYS* p_sys )
{
	GF_ASSERT( p_sys );
	sys_FreeMemoryEz( p_sys->p_buff );
	sys_FreeMemoryEz( p_sys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	Xグリッドサイズ取得
 *
 *	@param	cp_sys	マップ管理システム
 *	
 *	@return	ｘグリッドサイズ
 */
//-----------------------------------------------------------------------------
u16 WF2DMAP_MAPSysGridXGet( const WF2DMAP_MAPSYS* cp_sys )
{
	return cp_sys->xgrid;
}

//----------------------------------------------------------------------------
/**
 *	@brief	Yグリッドサイズ取得
 *
 *	@param	cp_sys	マップ管理システム
 *	
 *	@return	Yグリッドサイズ
 */
//-----------------------------------------------------------------------------
u16 WF2DMAP_MAPSysGridYGet( const WF2DMAP_MAPSYS* cp_sys )
{
	return cp_sys->ygrid;
}

//----------------------------------------------------------------------------
/**
 *	@brief	マップデータ設定
 *
 *	@param	p_sys		マップ管理システム
 *	@param	cp_buff		設定マップデータバッファ
 */
//-----------------------------------------------------------------------------
void WF2DMAP_MAPSysDataSet( WF2DMAP_MAPSYS* p_sys, const WF2DMAP_MAP* cp_buff )
{
	GF_ASSERT( p_sys );
	memcpy( p_sys->p_buff, cp_buff, sizeof(WF2DMAP_MAP)*(p_sys->xgrid*p_sys->ygrid) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	マップデータ取得
 *
 *	@param	cp_sys		マップ管理システム
 *	@param	xgrid		ｘグリッド
 *	@param	ygrid		ｙグリッド
 *
 *	@retval	マップデータ
 *	@retval	WF2DMAP_MAPDATA_NONE	マップ範囲外
 */
//-----------------------------------------------------------------------------
WF2DMAP_MAP WF2DMAP_MAPSysDataGet( const WF2DMAP_MAPSYS* cp_sys, u16 xgrid, u16 ygrid )
{
	WF2DMAP_MAP retmap = {WF2DMAP_MAPDATA_NONE};
	
	GF_ASSERT( cp_sys );
	if( (cp_sys->xgrid > xgrid) && (cp_sys->ygrid > ygrid) ){
		retmap = cp_sys->p_buff[ (cp_sys->xgrid*ygrid)+xgrid ];
	}
	return retmap;
}

//----------------------------------------------------------------------------
/**
 *	@brief	マップあたり判定データ取得
 *
 *	@param	cp_sys	マップ管理システム
 *	@param	xgrid	ｘグリッド
 *	@param	ygrid	ｙグリッド
 *
 *	@retval	TRUE	当たり判定あり
 *	@retval	FALSE	当たり判定なし
 */
//-----------------------------------------------------------------------------
BOOL WF2DMAP_MAPSysHitGet( const WF2DMAP_MAPSYS* cp_sys, u16 xgrid, u16 ygrid )
{
	WF2DMAP_MAP mapdata;

	GF_ASSERT( cp_sys );
	
	mapdata = WF2DMAP_MAPSysDataGet( cp_sys, xgrid, ygrid );
	if( mapdata.data == WF2DMAP_MAPDATA_NONE ){
		return TRUE;	// 範囲外は当たり判定ありにする
	}
	return mapdata.hit;
}

//----------------------------------------------------------------------------
/**
 *	@brief	マップパラメータデータ取得
 *
 *	@param	cp_sys	マップ管理システム
 *	@param	xgrid	ｘグリッド
 *	@param	ygrid	ｙグリッド
 *
 *	@return	パラメータデータ
 */
//-----------------------------------------------------------------------------
u32 WF2DMAP_MAPSysParamGet( const WF2DMAP_MAPSYS* cp_sys, u16 xgrid, u16 ygrid )
{
	WF2DMAP_MAP mapdata;

	GF_ASSERT( cp_sys );
	
	mapdata = WF2DMAP_MAPSysDataGet( cp_sys, xgrid, ygrid );
	if( mapdata.data == WF2DMAP_MAPDATA_NONE ){
		return 0;
	}
	return mapdata.param;
}

