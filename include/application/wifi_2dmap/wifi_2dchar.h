//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifi_2dchar.h
 *	@brief		wifi２Dキャラクタ読み込みシステム
 *	@author		tomoya takahshi
 *	@data		2007.02.07
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WIFI_2DCHAR_H__
#define __WIFI_2DCHAR_H__

#include "wifi_2dcharcon.h"
#include "application/wifi_2dmap/wifi_2dcommon.h"
#include "include/gflib/clact.h"
#include "src/field/fieldobj_code.h"
#include "system/palanm.h"

#undef GLOBAL
#ifdef	__WIFI_2DCHAR_H_GLOBAL
#define	GLOBAL	/* */
#else
#define	GLOBAL	extern
#endif

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	動作タイプ
// 内部的には、セルアニメリソースの種類
//=====================================
typedef enum {
	// オーソドックスアニメ
	WF_2DC_MOVERUN = WF2DC_C_MOVERUN,		// 歩き＋振り向き＋走りアニメ（主人公のみ）
	WF_2DC_MOVENORMAL = WF2DC_C_MOVENORMAL,	// 歩き＋振り向きのみアニメ
	WF_2DC_MOVETURN = WF2DC_C_MOVETURN,	// 振り向きのみアニメ
	
	// 拡張アニメ
	
	// アニメーションタイプ数
	WF_2DC_MOVENUM = WF2DC_C_MOVENUM,
	
} WF_2DC_MOVETYPE;

//-------------------------------------
///	アニメーションタイプ
//　動作タイプがオーソドックスアニメの場合にのみ
//　使用できるフラグです。
//	
//=====================================
typedef enum {
	WF_2DC_ANMWAY = WF2DC_C_ANMWAY,	// 向き変えアニメ	1フレームで切り替わります
	WF_2DC_ANMROTA = WF2DC_C_ANMROTA,	// 回転アニメ		ループ

	// WF_2DC_MOVETURNモードでないと指定できません
	WF_2DC_ANMWALK = WF2DC_C_ANMWALK,	// 歩きアニメ		1歩8フレーム
	WF_2DC_ANMTURN = WF2DC_C_ANMTURN,	// 振り向きアニメ	2フレーム

	// WF_2DC_MOVERUNモードでないと指定できません
	WF_2DC_ANMRUN = WF2DC_C_ANMRUN,	// 走りアニメ		1歩4フレーム

	//  壁方向アニメ
	WF_2DC_ANMWALLWALK = WF2DC_C_ANMWALLWALK,	// 壁方向アニメ	1歩16フレーム

	// ゆっくり歩き
	WF_2DC_ANMSLOWWALK = WF2DC_C_ANMSLOWWALK,	// 1歩16フレーム

	// 高速歩き
	WF_2DC_ANMHIGHWALK2 = WF2DC_C_ANMHIGHWALK2,	// 1歩2フレーム
	WF_2DC_ANMHIGHWALK4 = WF2DC_C_ANMHIGHWALK4,	// 1歩4フレーム
	
	// 何も無いときはこれ
	WF_2DC_ANMNONE = WF2DC_C_ANMNONE,
	
	WF_2DC_ANMNUM = WF2DC_C_ANMNUM,
} WF_2DC_ANMTYPE;


//-------------------------------------
///	座標取得タイプ
typedef enum {
	WF_2DC_GET_X,
	WF_2DC_GET_Y
} WF_2DC_MAT;



//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
/// 2Dキャラクタ管理システム
typedef struct _WF_2DCSYS	WF_2DCSYS;

/// 2Dキャラクタワーク
typedef struct _WF_2DCWK	WF_2DCWK;

//-------------------------------------
///	ワーク登録データ
typedef struct {
	s16 x;
	s16 y;
	s16 pri;
	s16 bgpri;
} WF_2DC_WKDATA;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
// システムの生成
GLOBAL WF_2DCSYS* WF_2DC_SysInit( CLACT_SET_PTR p_clset, PALETTE_FADE_PTR p_pfd, u32 objnum, u32 heap );
GLOBAL void WF_2DC_SysExit( WF_2DCSYS* p_sys );

// リソース登録
GLOBAL void WF_2DC_ResSet( WF_2DCSYS* p_sys, u32 view_type, u32 draw_type, WF_2DC_MOVETYPE movetype, u32 heap );
GLOBAL void WF_2DC_ResDel( WF_2DCSYS* p_sys, u32 view_type );
GLOBAL BOOL WF_2DC_ResCheck( const WF_2DCSYS* cp_sys, u32 view_type );
GLOBAL void WF_2DC_AllResDel( WF_2DCSYS* p_sys );

// ユニオンリソース登録
// ユニオンリソースは、boy1 boy3 man3 badman explore fighter gorggeousm mystery girl1 girl2 woman2 woman3 idol lady cowgirl gorggeousw の１６体
GLOBAL void WF_2DC_UnionResSet( WF_2DCSYS* p_sys, u32 draw_type, WF_2DC_MOVETYPE movetype, u32 heap );
GLOBAL void WF_2DC_UnionResDel( WF_2DCSYS* p_sys );

// 陰のリソース読み込み
// 陰リソースは主人公（男女どちらか）のリソースを読み込んだ後に登録してください。
// 主人公用のカラーパレットを使用して表示します。
// また、陰のリソースを破棄する前に主人公のリソースを破棄しないようにお願いいたします。
GLOBAL void WF_2DC_ShadowResSet( WF_2DCSYS* p_sys, u32 draw_type, u32 shadow_pri, u32 heap );
GLOBAL void WF_2DC_ShadowResDel( WF_2DCSYS* p_sys );

// アクター作成
GLOBAL WF_2DCWK* WF_2DC_WkAdd( WF_2DCSYS* p_sys, const WF_2DC_WKDATA* cp_data, u32 view_type, u32 heap );
GLOBAL void WF_2DC_WkDel( WF_2DCWK* p_wk );

// ワーク操作系
GLOBAL CLACT_WORK_PTR WF_2DC_WkClWkGet( WF_2DCWK* p_wk );
GLOBAL CONST_CLACT_WORK_PTR WF_2DC_WkConstClWkGet( const WF_2DCWK* cp_wk );
GLOBAL void WF_2DC_WkMatrixSet( WF_2DCWK* p_wk, s16 x, s16 y );
GLOBAL s16 WF_2DC_WkMatrixGet( WF_2DCWK* p_wk, WF_2DC_MAT x_y );
GLOBAL void WF_2DC_WkDrawPriSet( WF_2DCWK* p_wk, u16 pri );
GLOBAL u16 WF_2DC_WkDrawPriGet( const WF_2DCWK* cp_wk );
GLOBAL void WF_2DC_WkAnmAddFrame( WF_2DCWK* p_wk, fx32 frame );
GLOBAL void WF_2DC_WkDrawFlagSet( WF_2DCWK* p_wk, BOOL flag );
GLOBAL BOOL WF_2DC_WkDrawFlagGet( const WF_2DCWK* cp_wk );
GLOBAL WF_2DC_MOVETYPE WF_2DC_WkMoveTypeGet( const WF_2DCWK* cp_wk );
GLOBAL WF_2DC_ANMTYPE WF_2DC_WkAnmTypeGet( const WF_2DCWK* cp_wk );
GLOBAL void WF_2DC_WkAnmFrameSet( WF_2DCWK* p_wk, u16 frame );
GLOBAL u16 WF_2DC_WkAnmFrameGet( const WF_2DCWK* cp_wk );
GLOBAL void WF_2DC_WkShadowMatrixSet( WF_2DCWK* p_wk, s16 x, s16 y );

// Patternアニメ（歩き、振り向きなどは決まった動作をするので）
// WF_2DC_ANMWAYは、1フレームで終わります。
// 終了チェックをする必要はありません
GLOBAL void WF_2DC_WkPatAnmStart( WF_2DCWK* p_wk, WF_2DC_ANMTYPE anmtype, WF_COMMON_WAY anmway );
GLOBAL BOOL WF_2DC_WkPatAnmEndCheck( const WF_2DCWK* cp_wk );
GLOBAL void WF_2DC_WkPatAnmAddFrame( WF_2DCWK* p_wk );// Patternアニメ更新処理

#undef	GLOBAL
#endif		// __WIFI_2DCHAR_H__

