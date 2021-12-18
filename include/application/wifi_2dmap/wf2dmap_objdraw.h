//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wf2dmap_objdraw.h
 *	@brief		オブジェクトデータ表示モジュール
 *	@author		tomoya takahashi
 *	@data		2007.03.16
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WF2DMAP_OBJDRAW_H__
#define __WF2DMAP_OBJDRAW_H__

#include "application/wifi_2dmap/wf2dmap_common.h"
#include "application/wifi_2dmap/wf2dmap_obj.h"
#include "application/wifi_2dmap/wifi_2dchar.h"

#undef GLOBAL
#ifdef	__WF2DMAP_OBJDRAW_H_GLOBAL
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
///	表示システムワーク
//=====================================
typedef struct _WF2DMAP_OBJDRAWSYS WF2DMAP_OBJDRAWSYS;

//-------------------------------------
///	表示ワーク
//=====================================
typedef struct _WF2DMAP_OBJDRAWWK WF2DMAP_OBJDRAWWK;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

// システム生成
// Init_Shadow陰のパレットリソースは主人公のものを使用するため、初期化時に主人公のデータは読み込みを行います
GLOBAL WF2DMAP_OBJDRAWSYS* WF2DMAP_OBJDrawSysInit( CLACT_SET_PTR p_clset, PALETTE_FADE_PTR p_pfd, u32 objnum, u32 draw_type, u32 heapID );
GLOBAL WF2DMAP_OBJDRAWSYS* WF2DMAP_OBJDrawSysInit_Shadow( CLACT_SET_PTR p_clset, PALETTE_FADE_PTR p_pfd, u32 objnum, u32 hero_charid, WF_2DC_MOVETYPE hero_movetype,  u32 draw_type,u32 heapID );
GLOBAL void WF2DMAP_OBJDrawSysExit( WF2DMAP_OBJDRAWSYS* p_sys );

// オブジェクト登録基本設定変更
GLOBAL void WF2DMAP_OBJDrawSysDefBgPriSet( WF2DMAP_OBJDRAWSYS* p_sys, u32 bg_pri );
GLOBAL u32 WF2DMAP_OBJDrawSysDefBgPriGet( const WF2DMAP_OBJDRAWSYS* cp_sys );

// グラフィックデータの登録
// WF_2DC_MOVETYPEはinclude/applicaton/wifi_2dmap/wifi_2dchar.hに定義されています
GLOBAL void WF2DMAP_OBJDrawSysResSet( WF2DMAP_OBJDRAWSYS* p_sys, u32 charaid, WF_2DC_MOVETYPE movetype, u32 heapID );
GLOBAL void WF2DMAP_OBJDrawSysResDel( WF2DMAP_OBJDRAWSYS* p_sys, u32 charaid );
GLOBAL BOOL WF2DMAP_OBJDrawSysResCheck( const WF2DMAP_OBJDRAWSYS* cp_sys, u32 charaid );
GLOBAL void WF2DMAP_OBJDrawSysAllResDel( WF2DMAP_OBJDRAWSYS* p_sys );

// ユニオングラフィック登録
// ユニオンリソースは、boy1 boy3 man3 badman explore fighter gorggeousm mystery girl1 girl2 woman2 woman3 idol lady cowgirl gorggeousw の１６体
GLOBAL void WF2DMAP_OBJDrawSysUniResSet( WF2DMAP_OBJDRAWSYS* p_sys, WF_2DC_MOVETYPE movetype, u32 heap );
GLOBAL void WF2DMAP_OBJDrawSysUniResDel( WF2DMAP_OBJDRAWSYS* p_sys );

// オブジェクト登録
GLOBAL WF2DMAP_OBJDRAWWK* WF2DMAP_OBJDrawWkNew( WF2DMAP_OBJDRAWSYS* p_sys, const WF2DMAP_OBJWK* cp_objwk, BOOL hero, u32 heapID );
GLOBAL void WF2DMAP_OBJDrawWkDel( WF2DMAP_OBJDRAWWK* p_wk );

// オブジェクト表示データ更新	通常はこれでよい
GLOBAL void WF2DMAP_OBJDrawSysUpdata( WF2DMAP_OBJDRAWSYS* p_sys );
// オブジェクト表示データ個々に更新
GLOBAL void WF2DMAP_OBJDrawWkUpdata( WF2DMAP_OBJDRAWWK* p_wk );

// オブジェクト更新のONOFFフラグ設定　取得
GLOBAL void WF2DMAP_OBJDrawWkUpdataFlagSet( WF2DMAP_OBJDRAWWK* p_wk, BOOL flag );
GLOBAL BOOL WF2DMAP_OBJDrawWkUpdataFlagGet( const WF2DMAP_OBJDRAWWK* cp_wk );

// 特殊エフェクト動作
// WF2DMAP_OBJDrawWkUpdataFlagSetでオブジェクト更新をOFFしておかないと意味がありません
// AnimeEndを実行するまでずっと動作します
GLOBAL void WF2DMAP_OBJDrawWkKuruAnimeStart( WF2DMAP_OBJDRAWWK* p_wk );
GLOBAL void WF2DMAP_OBJDrawWkKuruAnimeMain( WF2DMAP_OBJDRAWWK* p_wk );
GLOBAL void WF2DMAP_OBJDrawWkKuruAnimeEnd( WF2DMAP_OBJDRAWWK* p_wk );

// 座標を設定
// WF2DMAP_OBJDrawWkUpdataFlagSetでオブジェクト更新をOFFしておかないと意味がありません
GLOBAL WF2DMAP_POS WF2DMAP_OBJDrawWkMatrixGet( const WF2DMAP_OBJDRAWWK* cp_wk );
GLOBAL void WF2DMAP_OBJDrawWkMatrixSet( WF2DMAP_OBJDRAWWK* p_wk, WF2DMAP_POS pos );

// 表示・非表示
GLOBAL BOOL WF2DMAP_OBJDrawWkDrawFlagGet( const WF2DMAP_OBJDRAWWK* cp_wk );
GLOBAL void WF2DMAP_OBJDrawWkDrawFlagSet( WF2DMAP_OBJDRAWWK* p_wk, BOOL flag );

// 表示優先順位の取得
GLOBAL u16 WF2DMAP_OBJDrawWkDrawPriGet( const WF2DMAP_OBJDRAWWK* cp_wk );

// 表示状態の設定
// WF2DMAP_OBJDrawWkUpdataFlagSetでオブジェクト更新をOFFしておかないと意味がありません
GLOBAL void WF2DMAP_OBJDrawWkWaySet( WF2DMAP_OBJDRAWWK* p_wk, WF2DMAP_WAY way );

// 表示優先順位取得
GLOBAL u32 WF2DMAP_OBJDrawWkDrawPriCalc( s16 y, BOOL hero );

// パレットナンバーの取得
GLOBAL u32 WF2DMAP_OBJDrawWkPaletteNoGet( const WF2DMAP_OBJDRAWWK* cp_wk );

// 陰の座標を設定
GLOBAL void WF2DMAP_OBJDrawWkShadowMatrixSet( WF2DMAP_OBJDRAWWK* p_wk, WF2DMAP_POS pos );



#undef	GLOBAL
#endif		// __WF2DMAP_OBJDRAW_H__

