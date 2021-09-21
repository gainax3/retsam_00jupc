//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_3dmapobj_cont.h
 *	@brief		マップオブジェ配置管理
 *	@author		tomoya takahashi
 *	@data		2007.11.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WFLBY_3DMAPOBJ_CONT_H__
#define __WFLBY_3DMAPOBJ_CONT_H__

#include "wflby_def.h"
#include "wflby_3dmapobj.h"
#include "wflby_mapcont.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	配置フロート登録最大数
//=====================================
#define WFLBY_3DMAPOBJ_CONT_MAPOBJFLOAT_MAX		( WFLBY_FLOAT_MAX )

//-------------------------------------
///	フロート座席最大数
//=====================================
#define WFLBY_3DMAPOBJ_CONT_MAPOBJFLOAT_SHEET_MAX	( WFLBY_FLOAT_ON_NUM )

//-------------------------------------
///	床ランプアニメ定数
//=====================================
typedef enum {
	WFLBY_3DMAPOBJ_CONT_LAMP_NONE,	// 何も出ていない状態
	WFLBY_3DMAPOBJ_CONT_LAMP_ON,	// NONEから光を出す状態にする		その後ゆれアニメになります
	WFLBY_3DMAPOBJ_CONT_LAMP_OFF,	// 光が出ている状態から光を消す。	その後NONE状態になります。
	WFLBY_3DMAPOBJ_CONT_LAMP_YURE,	// ゆれアニメ
} WFLBY_3DMAPOBJ_CONT_LAMP_ANM;



//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	システム構造体
//=====================================
typedef struct _WFLBY_3DMAPOBJ_CONT WFLBY_3DMAPOBJ_CONT;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
extern WFLBY_3DMAPOBJ_CONT* WFLBY_3DMAPOBJCONT_Init( WFLBY_SEASON_TYPE season, WFLBY_ROOM_TYPE room, const WFLBY_MAPCONT* cp_map, u32 heapID, u32 gheapID );
extern void WFLBY_3DMAPOBJCONT_Exit( WFLBY_3DMAPOBJ_CONT* p_sys );
extern void WFLBY_3DMAPOBJCONT_Main( WFLBY_3DMAPOBJ_CONT* p_sys );
extern void WFLBY_3DMAPOBJCONT_Draw( WFLBY_3DMAPOBJ_CONT* p_sys );
extern void WFLBY_3DMAPOBJCONT_VBlank( WFLBY_3DMAPOBJ_CONT* p_sys );


// パソコン、フロートなどに対する操作関数を追加していく

// フロート
extern void WFLBY_3DMAPOBJCONT_FLOAT_GetPos( const WFLBY_3DMAPOBJ_CONT* cp_sys, u32 idx, VecFx32* p_vec );
extern void WFLBY_3DMAPOBJCONT_FLOAT_SetPos( WFLBY_3DMAPOBJ_CONT* p_sys, u32 idx, fx32 x );
extern void WFLBY_3DMAPOBJCONT_FLOAT_SetOfsPos( WFLBY_3DMAPOBJ_CONT* p_sys, u32 idx, const VecFx32* cp_vec );
extern void WFLBY_3DMAPOBJCONT_FLOAT_GetSheetPos( const WFLBY_3DMAPOBJ_CONT* cp_sys, u32 idx, u32 no, VecFx32* p_vec );
extern BOOL WFLBY_3DMAPOBJCONT_FLOAT_SetAnmSound( WFLBY_3DMAPOBJ_CONT* p_sys, u32 idx );
extern BOOL WFLBY_3DMAPOBJCONT_FLOAT_SetAnmBody( WFLBY_3DMAPOBJ_CONT* p_sys, u32 idx );
extern void WFLBY_3DMAPOBJCONT_FLOAT_SetDraw( WFLBY_3DMAPOBJ_CONT* p_sys, u32 idx, BOOL flag );
extern BOOL WFLBY_3DMAPOBJCONT_FLOAT_GetDraw( const WFLBY_3DMAPOBJ_CONT* cp_sys, u32 idx );
extern void WFLBY_3DMAPOBJCONT_FLOAT_SetRot( WFLBY_3DMAPOBJ_CONT* p_sys, u32 idx, u16 x );
extern BOOL WFLBY_3DMAPOBJCONT_FLOAT_CheckAnmSound( const WFLBY_3DMAPOBJ_CONT* cp_sys, u32 idx );
extern BOOL WFLBY_3DMAPOBJCONT_FLOAT_CheckAnmBody( const WFLBY_3DMAPOBJ_CONT* cp_sys, u32 idx );

// マップ
extern void WFLBY_3DMAPOBJCONT_MAP_OnPoll( WFLBY_3DMAPOBJ_CONT* p_sys );
extern void WFLBY_3DMAPOBJCONT_MAP_OffPoll( WFLBY_3DMAPOBJ_CONT* p_sys );


// 銅像
extern void WFLBY_3DMAPOBJCONT_MAP_StartDouzouAnm( WFLBY_3DMAPOBJ_CONT* p_sys, u8 gridx, u8 gridy );
extern void WFLBY_3DMAPOBJCONT_MAP_SetDouzouFire( WFLBY_3DMAPOBJ_CONT* p_sys, BOOL flag );
extern void WFLBY_3DMAPOBJCONT_MAP_SetDouzouBigFire( WFLBY_3DMAPOBJ_CONT* p_sys, fx32 speed );
extern void WFLBY_3DMAPOBJCONT_MAP_OffDouzouLight( WFLBY_3DMAPOBJ_CONT* p_sys );

// 床ライト
extern void WFLBY_3DMAPOBJCONT_MAP_SetFloorLight( WFLBY_3DMAPOBJ_CONT* p_sys, WFLBY_3DMAPOBJ_CONT_LAMP_ANM anm );
extern BOOL WFLBY_3DMAPOBJCONT_MAP_CheckFloorLightYure( const WFLBY_3DMAPOBJ_CONT* cp_sys );
extern void WFLBY_3DMAPOBJCONT_MAP_SetFloorLightFire( WFLBY_3DMAPOBJ_CONT* p_sys, BOOL  flag, fx32 speed );
extern BOOL WFLBY_3DMAPOBJCONT_MAP_SetFloorLightFireIdx( WFLBY_3DMAPOBJ_CONT* p_sys, u32 idx, BOOL  flag,  fx32 speed );
extern void WFLBY_3DMAPOBJCONT_MAP_SetFloorLightBigFire( WFLBY_3DMAPOBJ_CONT* p_sys, fx32 speed );

// ミニゲームアニメ
extern void WFLBY_3DMAPOBJCONT_MAP_SetMGAnm( WFLBY_3DMAPOBJ_CONT* p_sys, WFLBY_GAMETYPE game, u32 num, BOOL recruit, BOOL play, BOOL light );


#endif		// __WFLBY_3DMAPOBJ_CONT_H__

