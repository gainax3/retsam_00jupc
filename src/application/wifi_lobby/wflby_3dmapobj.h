//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_3dmapobj.h
 *	@brief		マップ表示物管理
 *	@author		tomoya takahashi
 *	@data		2007.11.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WFLBY_3DMAPOBJ_H__
#define __WFLBY_3DMAPOBJ_H__

#include "system/arc_tool.h"

#include "application/wifi_2dmap/wf2dmap_common.h"
#include "wflby_def.h"
#include "system/d3dobj.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	フロートタイプ定数
//=====================================
typedef enum {
	WFLBY_3DMAPOBJ_FLOAT_00,
	WFLBY_3DMAPOBJ_FLOAT_01,
	WFLBY_3DMAPOBJ_FLOAT_NUM
} WFLBY_3DMAPOBJ_FLOAT_TYPE;

//-------------------------------------
///	配置オブジェタイプ定数
//=====================================
typedef enum {
	WFLBY_3DMAPOBJ_BIGBRONZE,
	WFLBY_3DMAPOBJ_SMALLBRONZE00,	// それぞれ４パターン
	WFLBY_3DMAPOBJ_SMALLBRONZE01,	// 
	WFLBY_3DMAPOBJ_SMALLBRONZE02,	// 
	WFLBY_3DMAPOBJ_SMALLBRONZE03,	// 
	WFLBY_3DMAPOBJ_TREE,
	WFLBY_3DMAPOBJ_MG_00,
	WFLBY_3DMAPOBJ_MG_01,
	WFLBY_3DMAPOBJ_MG_02,
	WFLBY_3DMAPOBJ_TABLE,
	WFLBY_3DMAPOBJ_KANBAN,
	WFLBY_3DMAPOBJ_LAMP00,
	WFLBY_3DMAPOBJ_LAMP01,
	WFLBY_3DMAPOBJ_HANABI,
	WFLBY_3DMAPOBJ_HANABIBIG,
	WFLBY_3DMAPOBJ_BRHANABI,
	WFLBY_3DMAPOBJ_BRHANABIBIG,
	WFLBY_3DMAPOBJ_ANKETO,
	WFLBY_3DMAPOBJ_WK_NUM,
} WFLBY_3DMAPOBJ_WK_TYPE;

//-------------------------------------
///	配置オブジェ	アニメタイプ定数
//=====================================
typedef enum {

	// 大きなオブジェ
	WFLBY_3DMAPOBJ_BIGBRONZEZ_ANM_ALL=0,	// 常駐アニメ
	WFLBY_3DMAPOBJ_BIGBRONZEZ_ANM_ALL01,	// 常駐アニメ
	WFLBY_3DMAPOBJ_BIGBRONZEZ_ANM_ALL02,	// 常駐アニメ
	WFLBY_3DMAPOBJ_BIGBRONZEZ_ANM_NUM,		// 数

	// 小さいオブジェ
	WFLBY_3DMAPOBJ_SMALLBRONZE_ANM_TOUCH = 0,	// 触れたときアニメ
	WFLBY_3DMAPOBJ_SMALLBRONZE_ANM_TOUCH00,	// 触れたときアニメ
	WFLBY_3DMAPOBJ_SMALLBRONZE_ANM_TOUCH01,	// 触れたときアニメ
	WFLBY_3DMAPOBJ_SMALLBRONZE_ANM_NUM,		// 数

	// 看板
	WFLBY_3DMAPOBJ_KANBAN_ANM_LAMP=0,	// 点灯アニメ
	WFLBY_3DMAPOBJ_KANBAN_ANM_NUM,		// 数

	// ランプ
	WFLBY_3DMAPOBJ_LAMP_ANM_ON=0,		// 点灯アニメ
	WFLBY_3DMAPOBJ_LAMP_ANM_YURE,		// ゆれアニメ
	WFLBY_3DMAPOBJ_LAMP_ANM_NUM,

	// 花火
	WFLBY_3DMAPOBJ_HANABI_ANM_NICA=0,	// 通常花火
	WFLBY_3DMAPOBJ_HANABI_ANM_NITA,		// 通常花火
	WFLBY_3DMAPOBJ_HANABI_ANM_NITP,		// 通常花火
	WFLBY_3DMAPOBJ_HANABI_ANM_NUM,		// 数

	// でかい花火
	WFLBY_3DMAPOBJ_HANABIBIG_ANM_NICA=0,	// 通常花火
	WFLBY_3DMAPOBJ_HANABIBIG_ANM_NITP,		// 通常花火
	WFLBY_3DMAPOBJ_HANABIBIG_ANM_NUM,		// 数

	// 銅像花火
	WFLBY_3DMAPOBJ_BRHANABI_ANM_NICA=0,		// 通常花火
	WFLBY_3DMAPOBJ_BRHANABI_ANM_NITA,		// 通常花火
	WFLBY_3DMAPOBJ_BRHANABI_ANM_NITP,		// 通常花火
	WFLBY_3DMAPOBJ_BRHANABI_ANM_NUM,		// 通常花火

	// 銅像でかい花火
	WFLBY_3DMAPOBJ_BRHANABIBIG_ANM_NICA=0,	// 通常花火
	WFLBY_3DMAPOBJ_BRHANABIBIG_ANM_NITA,		// 通常花火
	WFLBY_3DMAPOBJ_BRHANABIBIG_ANM_NITP,		// 通常花火
	WFLBY_3DMAPOBJ_BRHANABIBIG_ANM_NUM,		// 通常花火
	
	// ミニゲーム
	WFLBY_3DMAPOBJ_MG_ANM_ALL=0,	// 常にアニメ
	WFLBY_3DMAPOBJ_MG_ANM_MOVE,		// 動作中アニメ
	WFLBY_3DMAPOBJ_MG_ANM_NUM,		// 数
	

	// 全体で一番多いアニメ数
	WFLBY_3DMAPOBJ_ALL_ANM_MAX	= 3,
} WFLBY_3DMAPOBJ_WK_ANM_TYPE;
typedef enum {	// 配置オブジェアニメ　再生タイプ
	WFLBY_3DMAPOBJ_WK_ANM_LOOP,				// ループ
	WFLBY_3DMAPOBJ_WK_ANM_NOLOOP,			// ループなし
	WFLBY_3DMAPOBJ_WK_ANM_NOAUTO,			// フレームユーザ指定
	WFLBY_3DMAPOBJ_WK_ANM_BACKLOOP,			// 逆再生ループ
	WFLBY_3DMAPOBJ_WK_ANM_BACKNOLOOP,		// 逆再生ループなし
	WFLBY_3DMAPOBJ_WK_ANM_RANDWAIT,			// ランダムウエイト再生	ループ
	WFLBY_3DMAPOBJ_WK_ANM_RANDSTART,		// ランダムスタート再生	ループ
	WFLBY_3DMAPOBJ_WK_ANM_PLAYNUM,
} WFLBY_3DMAPOBJ_WK_ANM_PLAY;
#define WFLBY_3DMAPOBJ_WK_ANM_RANDWAIT_WAITDEF	( 20 )	// 初期ランダムウエイト定数



//-------------------------------------
///	フロートカラー定数
//=====================================
typedef enum {
	WFLBY_3DMAPOBJ_FLOAT_COL_00,
	WFLBY_3DMAPOBJ_FLOAT_COL_01,
	WFLBY_3DMAPOBJ_FLOAT_COL_02,
	WFLBY_3DMAPOBJ_FLOAT_COL_NUM,
} WFLBY_3DMAPOBJ_FLOAT_COL;



//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	マップ表示物管理システム
//=====================================
typedef struct _WFLBY_3DMAPOBJ WFLBY_3DMAPOBJ;

//-------------------------------------
///	フロートワーク
//=====================================
typedef struct _WFLBY_3DMAPOBJ_FLOAT	WFLBY_3DMAPOBJ_FLOAT;

//-------------------------------------
///	その他の物ワーク
//	(表示非表示くらいしか出来ない)
//=====================================
typedef struct _WFLBY_3DMAPOBJ_WK	WFLBY_3DMAPOBJ_WK;

//-------------------------------------
///	アニメ終了コールバック
//=====================================
typedef void (*pWFLBY_3DMAPOBJ_WK_AnmCallBack)( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk );



//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
// システム管理
extern WFLBY_3DMAPOBJ* WFLBY_3DMAPOBJ_Init( u32 float_num, u32 objwk_num, u32 heapID, u32 gheapID );
extern void WFLBY_3DMAPOBJ_Exit( WFLBY_3DMAPOBJ* p_sys );
extern void WFLBY_3DMAPOBJ_Main( WFLBY_3DMAPOBJ* p_sys );
extern void WFLBY_3DMAPOBJ_Draw( WFLBY_3DMAPOBJ* p_sys );
extern void WFLBY_3DMAPOBJ_VBlank( WFLBY_3DMAPOBJ* p_sys );

// 部屋のリソース
extern void WFLBY_3DMAPOBJ_ResLoad( WFLBY_3DMAPOBJ* p_sys, WFLBY_ROOM_TYPE room, WFLBY_SEASON_TYPE season, u32 heapID, u32 gheapID );
extern void WFLBY_3DMAPOBJ_ResRelease( WFLBY_3DMAPOBJ* p_sys );

// マップ操作
extern void WFLBY_3DMAPOBJ_MAP_OnPoll( WFLBY_3DMAPOBJ* p_sys );
extern void WFLBY_3DMAPOBJ_MAP_OffPoll( WFLBY_3DMAPOBJ* p_sys );

// フロートワーク操作
extern WFLBY_3DMAPOBJ_FLOAT* WFLBY_3DMAPOBJ_FLOAT_Add( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_FLOAT_TYPE floattype, WFLBY_3DMAPOBJ_FLOAT_COL floatcol, const VecFx32* cp_mat );
extern void WFLBY_3DMAPOBJ_FLOAT_Del( WFLBY_3DMAPOBJ_FLOAT* p_wk );
extern void WFLBY_3DMAPOBJ_FLOAT_SetPos( WFLBY_3DMAPOBJ_FLOAT* p_wk, const VecFx32* cp_mat );
extern void WFLBY_3DMAPOBJ_FLOAT_SetOfsPos( WFLBY_3DMAPOBJ_FLOAT* p_wk, const VecFx32* cp_mat );
extern void WFLBY_3DMAPOBJ_FLOAT_GetPos( const WFLBY_3DMAPOBJ_FLOAT* cp_wk, VecFx32* p_mat );
extern void WFLBY_3DMAPOBJ_FLOAT_SetDraw( WFLBY_3DMAPOBJ_FLOAT* p_wk, BOOL flag );
extern BOOL WFLBY_3DMAPOBJ_FLOAT_GetDraw( const WFLBY_3DMAPOBJ_FLOAT* cp_wk );
extern BOOL WFLBY_3DMAPOBJ_FLOAT_SetAnmSound( WFLBY_3DMAPOBJ_FLOAT* p_wk );
extern BOOL WFLBY_3DMAPOBJ_FLOAT_SetAnmBody( WFLBY_3DMAPOBJ_FLOAT* p_wk );
extern void WFLBY_3DMAPOBJ_FLOAT_SetRotaX( WFLBY_3DMAPOBJ_FLOAT* p_wk, u16 x );
extern BOOL WFLBY_3DMAPOBJ_FLOAT_CheckAnmSound( const WFLBY_3DMAPOBJ_FLOAT* cp_wk );
extern BOOL WFLBY_3DMAPOBJ_FLOAT_CheckAnmBody( const WFLBY_3DMAPOBJ_FLOAT* cp_wk );

// 配置オブジェ操作
extern WFLBY_3DMAPOBJ_WK* WFLBY_3DMAPOBJ_WK_Add( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK_TYPE objtype, u32 gridx, u32 gridy );
extern void WFLBY_3DMAPOBJ_WK_Del( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk );
extern WFLBY_3DMAPOBJ_WK_TYPE WFLBY_3DMAPOBJ_WK_GetType( const WFLBY_3DMAPOBJ_WK* cp_wk );
extern void WFLBY_3DMAPOBJ_WK_SetPos( WFLBY_3DMAPOBJ_WK* p_wk, WF2DMAP_POS pos );
extern WF2DMAP_POS WFLBY_3DMAPOBJ_WK_GetPos( const WFLBY_3DMAPOBJ_WK* cp_wk );
extern void WFLBY_3DMAPOBJ_WK_AddAnm( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk, WFLBY_3DMAPOBJ_WK_ANM_TYPE anm, WFLBY_3DMAPOBJ_WK_ANM_PLAY play );
extern void WFLBY_3DMAPOBJ_WK_AddAnmAnmCallBack( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk, WFLBY_3DMAPOBJ_WK_ANM_TYPE anm, WFLBY_3DMAPOBJ_WK_ANM_PLAY play, pWFLBY_3DMAPOBJ_WK_AnmCallBack p_callback );
extern void WFLBY_3DMAPOBJ_WK_DelAnm( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk, WFLBY_3DMAPOBJ_WK_ANM_TYPE anm );
extern void WFLBY_3DMAPOBJ_WK_DelAnmAll( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk );
extern BOOL WFLBY_3DMAPOBJ_WK_CheckAnm( const WFLBY_3DMAPOBJ* cp_sys, const WFLBY_3DMAPOBJ_WK* cp_wk, WFLBY_3DMAPOBJ_WK_ANM_TYPE anm );
extern void WFLBY_3DMAPOBJ_WK_SetAnmFrame( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk, WFLBY_3DMAPOBJ_WK_ANM_TYPE anm, fx32 frame );	// アニメタイプがWFLBY_3DMAPOBJ_WK_ANM_NOAUTOのとき専用
extern BOOL WFLBY_3DMAPOBJ_WK_GetAnmFlag( const WFLBY_3DMAPOBJ_WK* cp_wk, WFLBY_3DMAPOBJ_WK_ANM_TYPE anm );
extern fx32 WFLBY_3DMAPOBJ_WK_GetAnmFrame( const WFLBY_3DMAPOBJ_WK* cp_wk, WFLBY_3DMAPOBJ_WK_ANM_TYPE anm );
extern void WFLBY_3DMAPOBJ_WK_SetRandWait( WFLBY_3DMAPOBJ_WK* p_wk, u32 wait );
extern void WFLBY_3DMAPOBJ_WK_SetDraw( WFLBY_3DMAPOBJ_WK* p_wk, BOOL flag );
extern void WFLBY_3DMAPOBJ_WK_SetLightFlag( WFLBY_3DMAPOBJ* p_sys, WFLBY_3DMAPOBJ_WK* p_wk, u32 light_flag );
extern void WFLBY_3DMAPOBJ_WK_SetAnmSpeed( WFLBY_3DMAPOBJ_WK* p_wk, fx32 speed );
extern fx32 WFLBY_3DMAPOBJ_WK_GetAnmSpeed( const WFLBY_3DMAPOBJ_WK* cp_wk );
extern void WFLBY_3DMAPOBJ_WK_SetAlpha( WFLBY_3DMAPOBJ_WK* p_wk, u8 alpha, u8 def_alpha );
extern void WFLBY_3DMAPOBJ_WK_ResetAlpha( WFLBY_3DMAPOBJ_WK* p_wk );
extern void WFLBY_3DMAPOBJ_WK_SetScale( WFLBY_3DMAPOBJ_WK* p_wk, fx32 x, fx32 y, fx32 z );


#endif		// __WFLBY_3DMAPOBJ_H__

