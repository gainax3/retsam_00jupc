//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_mapcont.c
 *	@brief		マップ管理システム
 *	@author		tomoya	takahashi
 *	@data		2007.11.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#include "system/arc_util.h"

#include "wflby_mapcont.h"
#include "map_conv/wflby_mapdata.h"
#include "map_conv/wflby_maparc.h"

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
#define WFLBY_MAPCONT_PARAM_SHIFT	( 15 )
#define WFLBY_MAPCONT_EVENT_MSK		( 0x00007fff )

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	マップシステム初期化
//=====================================
typedef struct _WFLBY_MAPCONT{
	WF2DMAP_MAPSYS* p_map;	
}WFLBY_MAPCONT;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

// システム作成
//----------------------------------------------------------------------------
/**
 *	@brief	マップ管理	初期化
 *
 *	@param	heapID	ヒープID
 *
 *	@return	システムワーク
 */
//-----------------------------------------------------------------------------
WFLBY_MAPCONT* WFLBY_MAPCONT_Init( u32 heapID )
{
	WFLBY_MAPCONT* p_sys;
	void* p_mapdata;

	p_sys = sys_AllocMemory( heapID, sizeof(WFLBY_MAPCONT) );
	
	// システム作成
	p_sys->p_map = WF2DMAP_MAPSysInit( WFLBY_MAPSIZE_X,  WFLBY_MAPSIZE_Y, heapID );
	
	// マップデータの読み込み
	p_mapdata = ArcUtil_Load( ARC_WFLBY_MAP, NARC_wflby_map_wflby_map_dat, FALSE, heapID, ALLOC_BOTTOM);
	WF2DMAP_MAPSysDataSet( p_sys->p_map, p_mapdata );

	sys_FreeMemoryEz( p_mapdata );

	return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	マップシステムの破棄
 *
 *	@param	p_sys	システム
 */
//-----------------------------------------------------------------------------
void WFLBY_MAPCONT_Exit( WFLBY_MAPCONT* p_sys )
{
	WF2DMAP_MAPSysExit( p_sys->p_map );
	sys_FreeMemoryEz( p_sys );
}

// マップの大きさ取得
//----------------------------------------------------------------------------
/**
 *	@brief	マップの大きさを取得
 *	
 *	@param	cp_sys	システムワーク
 */
//-----------------------------------------------------------------------------
u16 WFLBY_MAPCONT_GridXGet( const WFLBY_MAPCONT* cp_sys )
{
	return WFLBY_MAPSIZE_X;
}
u16 WFLBY_MAPCONT_GridYGet( const WFLBY_MAPCONT* cp_sys )
{
	return WFLBY_MAPSIZE_Y;
}

// マップデータ取得
//----------------------------------------------------------------------------
/**
 *	@brief	マップデータの取得
 *
 *	@param	cp_sys	システムワーク
 *	@param	xgrid	ｘグリッド
 *	@param	ygrid	ｙグリッド
 *
 *	@return	マップデータ
 */
//-----------------------------------------------------------------------------
WF2DMAP_MAP WFLBY_MAPCONT_DataGet( const WFLBY_MAPCONT* cp_sys, u16 xgrid, u16 ygrid )
{
	return WF2DMAP_MAPSysDataGet( cp_sys->p_map, xgrid, ygrid );
}

//----------------------------------------------------------------------------
/**
 *	@brief	当たり判定の有無チェック
 *
 *	@param	cp_sys	システムワーク
 *	@param	xgrid	ｘグリッド
 *	@param	ygrid	ｙグリッド
 *
 *	@return	マップデータ
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_MAPCONT_HitGet( const WFLBY_MAPCONT* cp_sys, u16 xgrid, u16 ygrid )
{
	return WF2DMAP_MAPSysHitGet( cp_sys->p_map, xgrid, ygrid );
}

//----------------------------------------------------------------------------
/**
 *	@brief	MAPOBJID取得	
 *
 *	@param	cp_sys		システムワーク
 *	@param	xgrid		ｘグリッド
 *	@param	ygrid		ｙグリッド
 *
 *	@return	WFLBY_MAPOBJID_～
 */
//-----------------------------------------------------------------------------
u32 WFLBY_MAPCONT_ObjIDGet( const WFLBY_MAPCONT* cp_sys, u16 xgrid, u16 ygrid )
{
	u32 param;
	param = WF2DMAP_MAPSysParamGet( cp_sys->p_map, xgrid, ygrid );
	return (param >> WFLBY_MAPCONT_PARAM_SHIFT);
}

//----------------------------------------------------------------------------
/**
 *	@brief	MAPEVID取得
 *
 *	@param	cp_sys		システムワーク
 *	@param	xgrid		ｘグリッド
 *	@param	ygrid		ｙグリッド
 *
 *	@return	WFLBY_MAPEVID_～
 */
//-----------------------------------------------------------------------------
u32 WFLBY_MAPCONT_EventGet( const WFLBY_MAPCONT* cp_sys, u16 xgrid, u16 ygrid )
{
	u32 param;
	param = WF2DMAP_MAPSysParamGet( cp_sys->p_map, xgrid, ygrid );
	param &= WFLBY_MAPCONT_EVENT_MSK;
	return param;
}

//----------------------------------------------------------------------------
/**
 *	@brief	OBJIDのグリッドを検索する
 *
 *	@param	cp_sys		システムワーク
 *	@param	objid		検索OBJID
 *	@param	p_gridx		グリッドX格納先
 *	@param	p_gridy		グリッドY格納先
 *	@param	no			何個目のデータか指定	（OBJIDのデータの指定個数位置）
 *
 *	@retval	TRUE	あった
 *	@retval	FALSE	なかった
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_MAPCONT_SarchObjID( const WFLBY_MAPCONT* cp_sys, u32 objid, u16* p_gridx, u16* p_gridy, u32 no )
{
	int i, j, count;
	u32 get_objid;
	
	count = 0;
	
	for( i=0; i<WFLBY_MAPSIZE_Y; i++ ){
		for( j=0; j<WFLBY_MAPSIZE_X; j++ ){
			get_objid = WFLBY_MAPCONT_ObjIDGet( cp_sys, j, i );
			if( get_objid == objid ){

				// 指定個数位置かチェック
				if( count >= no ){
					*p_gridx = j;
					*p_gridy = i;
					return TRUE;
				}
				count ++;
			}
		}
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	オブジェクトIDの数を数える
 *
 *	@param	cp_sys		システム
 *	@param	objid		オブジェクトID
 *
 *	@return	数
 */
//-----------------------------------------------------------------------------
u32 WFLBY_MAPCONT_CountObjID( const WFLBY_MAPCONT* cp_sys, u32 objid )
{
	int i, j,  count;
	u32 get_objid;
	
	count =0;
	for( i=0; i<WFLBY_MAPSIZE_Y; i++ ){
		for( j=0; j<WFLBY_MAPSIZE_X; j++ ){
			get_objid = WFLBY_MAPCONT_ObjIDGet( cp_sys, j, i );
			if( get_objid == objid ){
				count ++;
			}
		}
	}
	return count;
}

//----------------------------------------------------------------------------
/**
 *	@brief	上に乗ったら起動するのかチェック
 *
 *	@param	event	イベントNO
 *
 *	@retval	TRUE	起動する
 *	@retval	FALSE	起動しない
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_MAPCONT_Event_CheckMount( u32 event )
{
	static const u8 sc_WFLBY_MAPCONT_EVENT_MOUNT[] = {
		WFLBY_MAPEVID_EV_PLAYER_OUT,
		WFLBY_MAPEVID_EV_FOOT_00_00,
		WFLBY_MAPEVID_EV_FOOT_00_01,
		WFLBY_MAPEVID_EV_FOOT_00_02,
		WFLBY_MAPEVID_EV_FOOT_00_03,
		WFLBY_MAPEVID_EV_FOOT_00_04,
		WFLBY_MAPEVID_EV_FOOT_00_05,
		WFLBY_MAPEVID_EV_FOOT_00_06,
		WFLBY_MAPEVID_EV_FOOT_00_07,
		WFLBY_MAPEVID_EV_FOOT_01_00,
		WFLBY_MAPEVID_EV_FOOT_01_01,
		WFLBY_MAPEVID_EV_FOOT_01_02,
		WFLBY_MAPEVID_EV_FOOT_01_03,
		WFLBY_MAPEVID_EV_FOOT_01_04,
		WFLBY_MAPEVID_EV_FOOT_01_05,
		WFLBY_MAPEVID_EV_FOOT_01_06,
		WFLBY_MAPEVID_EV_FOOT_01_07,
		WFLBY_MAPEVID_EV_WLDTIMER_00,
		WFLBY_MAPEVID_EV_WLDTIMER_01,
		WFLBY_MAPEVID_EV_WLDTIMER_02,
		WFLBY_MAPEVID_EV_WLDTIMER_03,
		WFLBY_MAPEVID_EV_TOPIC_00,
		WFLBY_MAPEVID_EV_TOPIC_01,
		WFLBY_MAPEVID_EV_TOPIC_02,
		WFLBY_MAPEVID_EV_TOPIC_03,
		WFLBY_MAPEVID_EV_BALLSLOW,
		WFLBY_MAPEVID_EV_BALANCE,
		WFLBY_MAPEVID_EV_BALLOON,
		WFLBY_MAPEVID_EV_FLOAT00,
		WFLBY_MAPEVID_EV_FLOAT01,
		WFLBY_MAPEVID_EV_FLOAT02,
		WFLBY_MAPEVID_EV_ANKETO_BOARD,
	};
	int i;

	for( i=0; i<NELEMS(sc_WFLBY_MAPCONT_EVENT_MOUNT); i++ ){
		if( event == sc_WFLBY_MAPCONT_EVENT_MOUNT[i] ){
			return TRUE;
		}
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	目の前でその方向にキーを押していたら起動するのかチェック
 *
 *	@param	event	イベントNO
 *
 *	@retval	TRUE	起動する
 *	@retval	FALSE	起動しない
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_MAPCONT_Event_CheckFrontKey( u32 event )
{
	static const u8 sc_WFLBY_MAPCONT_EVENT_FRONTKEY[] = {
		WFLBY_MAPEVID_EV_KANBAN00,
		WFLBY_MAPEVID_EV_KANBAN01,
		WFLBY_MAPEVID_EV_KANBAN02,
	};
	int i;

	for( i=0; i<NELEMS(sc_WFLBY_MAPCONT_EVENT_FRONTKEY); i++ ){
		if( event == sc_WFLBY_MAPCONT_EVENT_FRONTKEY[i] ){
			return TRUE;
		}
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	目の前でその方向を向いてA押していたら起動するのかチェック
 *
 *	@param	event	イベントNO
 *
 *	@retval	TRUE	起動する
 *	@retval	FALSE	起動しない
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_MAPCONT_Event_CheckFrontDecide( u32 event )
{
	static const u8 sc_WFLBY_MAPCONT_EVENT_FRONTA[] = {
		WFLBY_MAPEVID_EV_SW_IN,
		WFLBY_MAPEVID_EV_SW_TOPIC,
		WFLBY_MAPEVID_EV_SW_TOPIC2,
		WFLBY_MAPEVID_EV_KANBAN00,
		WFLBY_MAPEVID_EV_KANBAN01,
		WFLBY_MAPEVID_EV_KANBAN02,
		WFLBY_MAPEVID_EV_ANKETO_MAN,
	};
	int i;

	for( i=0; i<NELEMS(sc_WFLBY_MAPCONT_EVENT_FRONTA); i++ ){
		if( event == sc_WFLBY_MAPCONT_EVENT_FRONTA[i] ){
			return TRUE;
		}
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	銅像イベントを起動するかチェック
 *
 *	@param	event		イベント
 *
 *	@retval	TRUE	起動する
 *	@retval	FALSE	起動しない
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_MAPCONT_Event_CheckFrontDouzou( u32 event )
{

	static const u8 sc_WFLBY_MAPCONT_EVENT_FRONTDZ[] = {
		WFLBY_MAPEVID_EV_DZ_00,
		WFLBY_MAPEVID_EV_DZ_01,
		WFLBY_MAPEVID_EV_DZ_02,
		WFLBY_MAPEVID_EV_DZ_03,
	};
	int i;

	for( i=0; i<NELEMS(sc_WFLBY_MAPCONT_EVENT_FRONTDZ); i++ ){
		if( event == sc_WFLBY_MAPCONT_EVENT_FRONTDZ[i] ){
			return TRUE;
		}
	}

	return FALSE;
}


//  出来るだけつかわないほうがいい関数
//----------------------------------------------------------------------------
/**
 *	@brief	マップコアシステム取得（wf2dmap_map）
 *
 *	@param	cp_sys	システムワーク
 *	
 *	@return	マップコアシステム
 */
//-----------------------------------------------------------------------------
const WF2DMAP_MAPSYS* WFLBY_MAPCONT_GetMapCoreSys( const WFLBY_MAPCONT* cp_sys )
{
	return cp_sys->p_map;
}
