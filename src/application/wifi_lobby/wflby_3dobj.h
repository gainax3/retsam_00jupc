//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_3dobj.h
 *	@brief		３Dビルボードアクターオブジェ管理システム
 *	@author		tomoya takahashi
 *	@data		2007.11.01
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WFLBY_3DOBJ_H__
#define __WFLBY_3DOBJ_H__

#include "application/wifi_2dmap/wf2dmap_obj.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	アニメ定数
//=====================================
typedef enum {
	WFLBY_3DOBJ_ANM_ROTA,
	WFLBY_3DOBJ_ANM_JUMP,
	WFLBY_3DOBJ_ANM_BATABATA,
	WFLBY_3DOBJ_ANM_NUM,
} WFLBY_3DOBJ_ANMTYPE;

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	システム構造体
//=====================================
typedef struct _WFLBY_3DOBJSYS WFLBY_3DOBJSYS;

//-------------------------------------
///	ワーク構造体
//=====================================
typedef struct _WFLBY_3DOBJWK WFLBY_3DOBJWK;



//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
// システム管理
extern WFLBY_3DOBJSYS* WFLBY_3DOBJSYS_Init( u32 objnum, u32 hero_sex, u32 heapID, u32 gheapID );
extern void WFLBY_3DOBJSYS_Exit( WFLBY_3DOBJSYS* p_sys );
extern void WFLBY_3DOBJSYS_Updata( WFLBY_3DOBJSYS* p_sys );
extern void WFLBY_3DOBJSYS_Draw( WFLBY_3DOBJSYS* p_sys );
extern void WFLBY_3DOBJSYS_VBlank( WFLBY_3DOBJSYS* p_sys );

// 出せるトレーナタイプなのかチェック
extern BOOL WFLBY_3DOBJSYS_CheckTrType( u32 trtype );

// 影操作
extern void WFLBY_3DOBJSYS_SetShadowAlpha( WFLBY_3DOBJSYS* p_sys, u32 alpha );
extern u32 WFLBY_3DOBJSYS_GetShadowAlpha( const WFLBY_3DOBJSYS* cp_sys );

// ワーク管理
extern WFLBY_3DOBJWK* WFLBY_3DOBJWK_New( WFLBY_3DOBJSYS* p_sys, const WF2DMAP_OBJWK* cp_objwk );
extern void WFLBY_3DOBJWK_Del( WFLBY_3DOBJWK* p_wk );

// 更新フラグ
extern void WFLBY_3DOBJWK_SetUpdata( WFLBY_3DOBJWK* p_wk, BOOL updata );
extern BOOL WFLBY_3DOBJWK_GetUpdata( const WFLBY_3DOBJWK* cp_wk );

// ライト
extern void WFLBY_3DOBJWK_SetLight( WFLBY_3DOBJWK* p_wk, u32 light_msk );

// カリングフラグ取得
extern BOOL WFLBY_3DOBJWK_GetCullingFlag( const WFLBY_3DOBJWK* cp_wk );

// 更新フラグをOFFにした後の動作
// 座標
extern void WFLBY_3DOBJWK_SetMatrix( WFLBY_3DOBJWK* p_wk, const WF2DMAP_POS* cp_pos );
extern void WFLBY_3DOBJWK_GetMatrix( const WFLBY_3DOBJWK* cp_wk, WF2DMAP_POS* p_pos );
extern void WFLBY_3DOBJWK_Set3DMatrix( WFLBY_3DOBJWK* p_wk, const VecFx32* cp_vec );
extern void WFLBY_3DOBJWK_Get3DMatrix( const WFLBY_3DOBJWK* cp_wk, VecFx32* p_vec );

// 方向
extern void WFLBY_3DOBJWK_SetWay( WFLBY_3DOBJWK* p_wk, WF2DMAP_WAY way );

// アニメ
extern void WFLBY_3DOBJWK_StartAnm( WFLBY_3DOBJWK* p_wk, WFLBY_3DOBJ_ANMTYPE anm );
extern void WFLBY_3DOBJWK_EndAnm( WFLBY_3DOBJWK* p_wk );
extern void WFLBY_3DOBJWK_SetAnmSpeed( WFLBY_3DOBJWK* p_wk, u8 speed );

// 表示・非表示
extern void WFLBY_3DOBJWK_SetDrawFlag( WFLBY_3DOBJWK* p_wk, BOOL flag );
extern BOOL WFLBY_3DOBJWK_GetDrawFlag( const WFLBY_3DOBJWK* cp_wk );


#endif		// __WFLBY_3DOBJ_H__

