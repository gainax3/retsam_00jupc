//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_3dobjcont.h
 *	@brief		登場する人物をすべて管理するシステム	
 *				人物に対するリクエストもすべてここを当して行う
 *	@author		tomoya takahashi
 *	@data		2007.11.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WFLBY_3DOBJCONT_H__
#define __WFLBY_3DOBJCONT_H__

#include "application/wifi_2dmap/wf2dmap_common.h"
#include "application/wifi_2dmap/wf2dmap_obj.h"
#include "wflby_mapcont.h"
#include "wflby_def.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	プレイヤー動作タイプ
//=====================================
typedef enum {
	WFLBY_3DOBJCONT_MOVENONE,	// 何も動かさない
	WFLBY_3DOBJCONT_MOVEPLAYER,	// 主人公動作
	WFLBY_3DOBJCONT_MOVENPC,	// NPC動作
	WFLBY_3DOBJCONT_MOVEFLYUP,	// 飛んであがっている	飛んであがると描画更新フラグがOFFのままになります		FLYDOWNなどを呼ぶとまだ更新ONになります
	WFLBY_3DOBJCONT_MOVEFLYDOWN,// 飛んで落ちてくる
	WFLBY_3DOBJCONT_MOVEROTAUP,		// 回転しながら吹き飛ぶ
	WFLBY_3DOBJCONT_MOVEROTADOWN,	// 回転する吹き飛ぶ
	WFLBY_3DOBJCONT_MOVEROTALEFT,	// 回転しながら吹き飛ぶ
	WFLBY_3DOBJCONT_MOVEROTARIGHT,	// 回転する吹き飛ぶ
	WFLBY_3DOBJCONT_MOVEJUMP,	// ジャンプ
	WFLBY_3DOBJCONT_MOVENUM,	// 動作タイプ数
} WFLBY_3DOBJCONT_MOVETYPE;


//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	システム構造体
//=====================================
typedef struct _WFLBY_3DOBJCONT WFLBY_3DOBJCONT;

//-------------------------------------
///	ワーク構造体
//=====================================
typedef struct _WFLBY_3DPERSON WFLBY_3DPERSON;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
// システム管理
extern WFLBY_3DOBJCONT* WFLBY_3DOBJCONT_Init( u32 objnum, u32 hero_sex, const WFLBY_MAPCONT* cp_map, u32 heapID, u32 gheapID );
extern void WFLBY_3DOBJCONT_Exit( WFLBY_3DOBJCONT* p_sys );
extern void WFLBY_3DOBJCONT_Move( WFLBY_3DOBJCONT* p_sys );
extern void WFLBY_3DOBJCONT_ReqMove( WFLBY_3DOBJCONT* p_sys );
extern void WFLBY_3DOBJCONT_Draw( WFLBY_3DOBJCONT* p_sys );
extern void WFLBY_3DOBJCONT_VBlank( WFLBY_3DOBJCONT* p_sys );

// 影操作
extern void WFLBY_3DOBJCONT_InitShadowAlpha( WFLBY_3DOBJCONT* p_sys, WFLBY_LIGHT_NEON_ROOMTYPE roomtype );
extern void WFLBY_3DOBJCONT_SetShadowAlpha( WFLBY_3DOBJCONT* p_sys, WFLBY_LIGHT_NEON_ROOMTYPE roomtype );

// 当たり判定関係
extern const WFLBY_3DPERSON* WFLBY_3DOBJCONT_CheckSysGridHit( const WFLBY_3DOBJCONT* cp_sys, u16 gridx, u16 gridy );
extern BOOL WFLBY_3DOBJCONT_CheckGridHit( const WFLBY_3DPERSON* cp_wk, u16 gridx, u16 gridy );
extern BOOL WFLBY_3DOBJCONT_GetOpenGird4Way( WFLBY_3DOBJCONT* p_sys, const WFLBY_3DPERSON* cp_wk, u32* p_way, WF2DMAP_POS* p_pos );

// リクエストコマンド設定
extern void WFLBY_3DOBJCONT_SetReqCmd( WFLBY_3DOBJCONT* p_sys, const WF2DMAP_REQCMD* cp_cmd );

//  ワーク登録、破棄、検索
extern WFLBY_3DPERSON* WFLBY_3DOBJCONT_AddPlayer( WFLBY_3DOBJCONT* p_sys, u32 plid );
extern WFLBY_3DPERSON* WFLBY_3DOBJCONT_AddPlayerEx( WFLBY_3DOBJCONT* p_sys, u32 plid, u32 gridx,  u32 gridy );
extern WFLBY_3DPERSON* WFLBY_3DOBJCONT_AddNpc( WFLBY_3DOBJCONT* p_sys, u32 plid, u32 trtype );
extern void WFLBY_3DOBJCONT_Delete( WFLBY_3DPERSON* p_wk );
extern WFLBY_3DPERSON* WFLBY_3DOBJCONT_GetPlIDWk( WFLBY_3DOBJCONT* p_sys, u32 plid );
extern WFLBY_3DPERSON* WFLBY_3DOBJCONT_GetPlayer( WFLBY_3DOBJCONT* p_sys );
extern WFLBY_3DPERSON* WFLBY_3DOBJCONT_GetPierrot( WFLBY_3DOBJCONT* p_sys );

// ワーク操作
extern void WFLBY_3DOBJCONT_SetWkPos( WFLBY_3DPERSON* p_wk, WF2DMAP_POS pos );
extern void WFLBY_3DOBJCONT_SetWkPosAndWay( WFLBY_3DPERSON* p_wk, WF2DMAP_POS pos, WF2DMAP_WAY way );
extern WF2DMAP_POS WFLBY_3DOBJCONT_GetWkPos( const WFLBY_3DPERSON* cp_wk );
extern u32 WFLBY_3DOBJCONT_GetWkObjData( const WFLBY_3DPERSON* cp_wk, WF2DMAP_OBJPARAM pm );
extern WF2DMAP_OBJWK* WFLBY_3DOBJCONT_GetWkObjWk( WFLBY_3DPERSON* p_wk );
extern void WFLBY_3DOBJCONT_SetWkReqCmd( WFLBY_3DOBJCONT* p_sys, const WFLBY_3DPERSON* cp_wk, WF2DMAP_CMD cmd, WF2DMAP_WAY way );
extern void WFLBY_3DOBJCONT_SetWkActCmd( WFLBY_3DOBJCONT* p_sys, const WFLBY_3DPERSON* cp_wk, WF2DMAP_CMD cmd, WF2DMAP_WAY way );
extern void WFLBY_3DOBJCONT_SetWkMove( WFLBY_3DOBJCONT* p_sys, WFLBY_3DPERSON* p_wk, WFLBY_3DOBJCONT_MOVETYPE movetype );
extern BOOL WFLBY_3DOBJCONT_CheckWkMoveEnd( const WFLBY_3DPERSON* cp_wk );
extern WFLBY_3DOBJCONT_MOVETYPE WFLBY_3DOBJCONT_GetWkMove( const WFLBY_3DPERSON* cp_wk );
extern WFLBY_3DPERSON* WFLBY_3DOBJCONT_GetFrontPerson( WFLBY_3DOBJCONT* p_sys, const WFLBY_3DPERSON* cp_wk );
extern BOOL WFLBY_3DOBJCONT_GetCullingFlag( const WFLBY_3DPERSON* cp_wk );

// 外側から表示部分のみ変更する
extern void WFLBY_3DOBJCONT_DRAW_SetUpdata( WFLBY_3DPERSON* p_wk, BOOL updata );
extern BOOL WFLBY_3DOBJCONT_DRAW_GetUpdata( const WFLBY_3DPERSON* cp_wk );
extern void WFLBY_3DOBJCONT_DRAW_SetMatrix( WFLBY_3DPERSON* p_wk, const WF2DMAP_POS* cp_pos );
extern void WFLBY_3DOBJCONT_DRAW_Set3DMatrix( WFLBY_3DPERSON* p_wk, const VecFx32* cp_vec );
extern void WFLBY_3DOBJCONT_DRAW_Get3DMatrix( const WFLBY_3DPERSON* cp_wk, VecFx32* p_vec );
extern void WFLBY_3DOBJCONT_DRAW_SetWay( WFLBY_3DPERSON* p_wk, WF2DMAP_WAY way );
extern void WFLBY_3DOBJCONT_DRAW_SetAnmJump( WFLBY_3DPERSON* p_wk, BOOL flag );
extern void WFLBY_3DOBJCONT_DRAW_SetAnmRota( WFLBY_3DPERSON* p_wk, BOOL flag );
extern void WFLBY_3DOBJCONT_DRAW_SetAnmBata( WFLBY_3DPERSON* p_wk, BOOL flag );
extern void WFLBY_3DOBJCONT_DRAW_SetLight( WFLBY_3DPERSON* p_wk, u32 light_msk );
extern void WFLBY_3DOBJCONT_DRAW_SetDraw( WFLBY_3DPERSON* p_wk,  BOOL flag );
extern BOOL WFLBY_3DOBJCONT_DRAW_GetDraw( const WFLBY_3DPERSON* cp_wk );

#endif		// __WFLBY_3DOBJCONT_H__

