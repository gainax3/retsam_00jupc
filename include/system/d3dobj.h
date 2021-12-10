//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		d3dobj.h
 *	@brief		フィールド３Dオブジェクト簡易作成
 *	@author		tomoya takahashi
 *	@data		2006.04.25
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __D3DOBJ_H__
#define __D3DOBJ_H__

#include <nnsys.h>
#include "system/arc_util.h"

#undef GLOBAL
#ifdef	__D3DOBJ_H_GLOBAL
#define	GLOBAL	/* */
#else
#define	GLOBAL	extern
#endif

//-----------------------------------------------------------------------------
/**
 *	【使用上の注意】
 *		・nsb～に2つ以上のものをくっつけて入れることも出来ます。
 *		　しかしこのシステムでは、
 *		　その中のインデックス0番目のものをいつも適用します。
 *
 *		・圧縮に非対応です。
 */
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
//	回転方向定数
//=====================================
enum{
	D3DOBJ_ROTA_WAY_X,
	D3DOBJ_ROTA_WAY_Y,
	D3DOBJ_ROTA_WAY_Z,
	D3DOBJ_ROTA_WAY_NUM,
};

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
//	３Dオブジェクト
//	モデル　テクスチャ
//	種類に１つでよいものです。
//=====================================
typedef struct {
	void*					pResMdl;		// モデルﾃﾞｰﾀ
	NNSG3dResMdlSet*		pModelSet;		// モデルセット
	NNSG3dResMdl*			pModel;			// モデルリソース
	NNSG3dResTex*			pMdlTex;		// モデルに貼り付けるテクスチャ
} D3DOBJ_MDL;

//-------------------------------------
//	３Dオブジェクト
//	アニメ
//	種類に１つでよいものです。
//=====================================
typedef struct {
	void*					pResAnm;		// アニメリソース
	void*					pOneResAnm;		// 1つ切り取ったリソース
	NNSG3dAnmObj*			pAnmObj;			// アニメーションオブジェ
	fx32 frame;
	BOOL res_copy;
} D3DOBJ_ANM;

//-------------------------------------
//	３D描画オブジェクト
//	出したいオブジェクトの数分必要なものです。
//=====================================
typedef struct {
	NNSG3dRenderObj			render;		// レンダーオブジェクト
	VecFx32 matrix;	// 座標
	VecFx32 scale;	// 拡縮
	BOOL	draw_flg;	// 描画フラグ
	u16		rota[ D3DOBJ_ROTA_WAY_NUM ];// 回転角X
	u16		dummy;
} D3DOBJ;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
// モデル
GLOBAL void D3DOBJ_MdlLoad( D3DOBJ_MDL* p_mdl, u32 arc_idx, u32 data_idx, u32 heap );
GLOBAL void D3DOBJ_MdlLoadH( D3DOBJ_MDL* p_mdl, ARCHANDLE* p_handle, u32 data_idx, u32 heap );
GLOBAL void D3DOBJ_MdlDelete( D3DOBJ_MDL* p_mdl );

// アニメ
GLOBAL void D3DOBJ_AnmLoad( D3DOBJ_ANM* p_anm, const D3DOBJ_MDL* cp_mdl, u32 arc_idx, u32 data_idx, u32 heap, NNSFndAllocator* pallocator );
GLOBAL void D3DOBJ_AnmLoadH( D3DOBJ_ANM* p_anm, const D3DOBJ_MDL* cp_mdl, ARCHANDLE* p_handle, u32 data_idx, u32 heap, NNSFndAllocator* pallocator );
GLOBAL void D3DOBJ_AnmLoad_Data( D3DOBJ_ANM* p_anm, const D3DOBJ_MDL* cp_mdl, void* p_data, NNSFndAllocator* pallocator );
GLOBAL void D3DOBJ_AnmDelete( D3DOBJ_ANM* p_anm, NNSFndAllocator* pallocator );
GLOBAL BOOL D3DOBJ_AnmNoLoop( D3DOBJ_ANM* p_anm, fx32 add );
GLOBAL void D3DOBJ_AnmLoop( D3DOBJ_ANM* p_anm, fx32 add );
GLOBAL void D3DOBJ_AnmSet( D3DOBJ_ANM* p_anm, fx32 num );
GLOBAL fx32 D3DOBJ_AnmGet( const D3DOBJ_ANM* cp_anm );
GLOBAL fx32 D3DOBJ_AnmGetFrameNum( const D3DOBJ_ANM* cp_anm );

// 描画オブジェ
GLOBAL void D3DOBJ_Init( D3DOBJ* p_draw, D3DOBJ_MDL* cp_mdl );
GLOBAL void D3DOBJ_AddAnm( D3DOBJ* p_draw, D3DOBJ_ANM* p_anm );
GLOBAL void D3DOBJ_DelAnm( D3DOBJ* p_draw, D3DOBJ_ANM* p_anm );
GLOBAL void D3DOBJ_Draw( D3DOBJ* p_draw );
GLOBAL void D3DOBJ_DrawRMtx( D3DOBJ* p_draw, const MtxFx33* cp_mtx );
GLOBAL void D3DOBJ_SetDraw( D3DOBJ* p_draw, BOOL flag );
GLOBAL BOOL D3DOBJ_GetDraw( const D3DOBJ* cp_draw );
GLOBAL void D3DOBJ_SetMatrix( D3DOBJ* p_draw, fx32 x, fx32 y, fx32 z );
GLOBAL void D3DOBJ_GetMatrix( const D3DOBJ* cp_draw, fx32* p_x, fx32* p_y, fx32* p_z );
GLOBAL void D3DOBJ_SetScale( D3DOBJ* p_draw, fx32 x, fx32 y, fx32 z );
GLOBAL void D3DOBJ_GetScale( const D3DOBJ* cp_draw, fx32* p_x, fx32* p_y, fx32* p_z );
GLOBAL void D3DOBJ_SetRota( D3DOBJ* p_draw, u16 rota, u32 way );
GLOBAL u16 D3DOBJ_GetRota( const D3DOBJ* cp_draw, u32 way );



#undef	GLOBAL
#endif		// __D3DOBJ_H__

