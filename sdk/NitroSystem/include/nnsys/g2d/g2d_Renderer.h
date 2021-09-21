/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d
  File:     g2d_Renderer.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.34 $
 *---------------------------------------------------------------------------*/
//
// The abstraction layer of rendering HWs.
// Rendering HWs include 2D Graphic Engine A - B, and 3D Graphic Engine.
// 
#ifndef NNS_G2D_RENDERER_H_
#define NNS_G2D_RENDERER_H_


#include <nitro.h>

#include <nnsys/g2d/g2d_Softsprite.h>
#include <nnsys/g2d/g2d_Entity.h>
#include <nnsys/g2d/g2d_Node.h>
#include <nnsys/g2d/g2d_MultiCellAnimation.h>
#include <nnsys/g2d/g2d_PaletteTable.h>
#include <nnsys/g2d/fmt/g2d_Character_data.h>
#include <nnsys/g2d/g2d_RendererCore.h>


#ifdef __cplusplus
extern "C" {
#endif

//
// 改名を行った関数の別名
// 互換性維持のため別名として以前の関数を宣言します。
// 
#define NNS_G2dAddTargetSurface             NNS_G2dAddRendererTargetSurface
#define NNS_G2dSetCurrentImageProxy         NNS_G2dSetRendererImageProxy






/*---------------------------------------------------------------------------*
  Name:         NNSG2dRndCellCullingFunction
  
  Description:   を可視カリングする関数ポインタ
  
  Arguments:    pCell   : セル
                pMtx    : 変換行列
                pTrans  : 平行移動値
               
  Returns:      可視ならTRUE
  
 *---------------------------------------------------------------------------*/ 
typedef BOOL(*NNSG2dRndCellCullingFunction)
(
    const NNSG2dCellData*   pCell,
    const MtxFx32*          pMtx,
    const NNSG2dViewRect*   pViewRect
);

struct NNSG2dRendererInstance;
struct NNSG2dRenderSurface;
/*---------------------------------------------------------------------------*
  Name:         NNSG2dRndDrawCellCallBack
  
  Description:   描画時に呼び出されるコールバック
                （描画前と描画後に呼び出される）
  
  Arguments:    pRend   : レンダラへのポインタ
                pSurface: レンダサーフェスへのポインタ
                pCell   : セル
                pMtx    : 変換行列
               
  Returns:      無し
  
 *---------------------------------------------------------------------------*/ 
typedef void(*NNSG2dRndDrawCellCallBack)
(
    struct NNSG2dRendererInstance*         pRend,
    struct NNSG2dRenderSurface*            pSurface,
    const NNSG2dCellData*                  pCell,
    const MtxFx32*                         pMtx
);


/*---------------------------------------------------------------------------*
  Name:         NNSG2dRndDrawOamCallBack
  
  Description:   内のOamアトリビュート描画時に呼び出されるコールバック
                （描画前と描画後に呼び出される）
  
  Arguments:    pRend   : レンダラへのポインタ
                pSurface: レンダサーフェスへのポインタ
                pCell   : セル
                pMtx    : 変換行列
               
  Returns:      無し
  
 *---------------------------------------------------------------------------*/ 
typedef void(*NNSG2dRndDrawOamCallBack)
(
    struct NNSG2dRendererInstance*         pRend,
    struct NNSG2dRenderSurface*            pSurface,
    const NNSG2dCellData*                  pCell,
    u16                                    oamIdx,
    const MtxFx32*                         pMtx
);



/*---------------------------------------------------------------------------*
  Name:         NNSG2dRenderSurface

  Description:  レンダリング対象をあらわす構造
                
                Oamを登録する関数、OamAffineParamaterを登録する関数のポインタを持ち、
                実際に描画命令を発行する責任を持ちます。
                ( "描画命令を発行する" 部分 は 別概念として分離するかもしれない )
               
                他の実体へのポインタを持ちリスト構造を構築します。
                
 *---------------------------------------------------------------------------*/    
typedef struct NNSG2dRenderSurface
{
    //
    // NNSG2dRndCoreSurface 
    // レンダリング対象サーフェスの基本的な特性を保持します。
    //
    union
    {
        NNSG2dRndCoreSurface              coreSurface;
        struct
        {
            NNSG2dViewRect                    viewRect;                 
            BOOL                              bActive;                  
            NNSG2dSurfaceType                 type;                     
            
            NNSG2dRndCoreDrawCellCallBack     pBeforeCoreDrawCellBack;  
            NNSG2dRndCoreDrawCellCallBack     pAfterCoreDrawCellBack;
    
            NNSG2dRndCoreDrawOamCallBack      pBeforeCoreDrawOamBack;
            NNSG2dRndCoreDrawOamCallBack      pAfterCoreDrawOamBack;
        };
    };
    
    NNSG2dOamRegisterFunction         pFuncOamRegister;         // Oam 登録関数
    NNSG2dAffineRegisterFunction      pFuncOamAffineRegister;   // Affine 登録関数
    
    void*                             pNextSurface;             // 次のサーフェス
    
    
    NNSG2dRndCellCullingFunction      pFuncVisibilityCulling;   // 可視カリング関数
    
    NNSG2dRndDrawCellCallBack         pBeforeDrawCellBackFunc;
    NNSG2dRndDrawCellCallBack         pAfterDrawCellBackFunc;
    
    NNSG2dRndDrawOamCallBack          pBeforeDrawOamBackFunc;
    NNSG2dRndDrawOamCallBack          pAfterDrawOamBackFunc;
    
              
}
NNSG2dRenderSurface;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dRendererOptimizeHint

  Description:  Renderer 描画最適化に関する情報
                
 *---------------------------------------------------------------------------*/
typedef enum NNSG2dRendererOptimizeHint
{
    NNS_G2D_RDR_OPZHINT_NONE              = 0x0,
    NNS_G2D_RDR_OPZHINT_NOT_SR            = 0x1, // SR(Scale Rotate) 変換を使用しない
    NNS_G2D_RDR_OPZHINT_LOCK_PARAMS       = 0x2  // パラメータをロックする。
                                                 // 
                                                 // サーフェスが一枚の場合のみ指定可能。
                                                 // 画像プロクシ、サーフェス種類が 
                                                 // レンダラ Begin-End Renderring ブロック内で変更不可能となります。
                                                 // その結果、レンダラコアの Begin-End Renderring の実行を 
                                                 // レンダラ Begin-End Renderring と同じタイミングで実行することが可能となります。
                                                 //
                                                 // フラグを指定しない場合、レンダラコアの Begin-End Renderring の実行は、
                                                 // セルの描画毎に行われます。

}NNSG2dRendererOptimizeHint;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dRendererOverwriteParam

  Description:  OBJアトリビュート上書き項目

 *---------------------------------------------------------------------------*/
typedef enum NNSG2dRendererOverwriteParam
{
    NNS_G2D_RND_OVERWRITE_NONE          = 0x0, // OBJアトリビュート上書きを行わない
    NNS_G2D_RND_OVERWRITE_PRIORITY      = 0x1, // 描画優先度
    NNS_G2D_RND_OVERWRITE_PLTTNO        = 0x2, // パレット番号
    NNS_G2D_RND_OVERWRITE_MOSAIC        = 0x4, // モザイクON OFF
    NNS_G2D_RND_OVERWRITE_OBJMODE       = 0x8, // オブジェモード
    NNS_G2D_RND_OVERWRITE_PLTTNO_OFFS   = 0x10, // パレット番号(オフセット加算)
    NNS_G2D_RND_OVERWRITE_MAX
    
}NNSG2dRendererOverwriteParam;
// 
// そのほかの候補
// OBJMode
// モザイクONOFF

/*---------------------------------------------------------------------------*
  Name:         NNSG2dRendererInstance

  Description:  Renderer 実体
                NNSG2dRenderSurfaceのリストを保持します

 *---------------------------------------------------------------------------*/
typedef struct NNSG2dRendererInstance
{
    NNSG2dRndCoreInstance             rendererCore;        // レンダラコア
        
    
    NNSG2dRenderSurface*              pTargetSurfaceList;  // 描画対象サーフェスリスト
    NNSG2dRenderSurface*              pCurrentSurface;     // 現在描画処理中のサーフェス
                                                           // ( レンダラコアが描画処理中以外は NULL に設定される )
    
    const NNSG2dPaletteSwapTable*     pPaletteSwapTbl;     // カラーパレット変更テーブル(NULLも可)                                                       
    u32                               opzHint;             // 描画最適化のための情報
                                                           // NNSG2dRendererOptimizeHint
                                                           
    fx32                              spriteZoffsetStep;   // ソフトウェアスプライト自動Zオフセットの量
    
    u32                               overwriteEnableFlag; // NNSG2dRendererOverwriteParam
    u16                               overwritePriority;   // 描画優先度
    u16                               overwritePlttNo;     // パレット番号
    GXOamMode                         overwriteObjMode;                      
    BOOL                              overwriteMosaicFlag;
    
    u16                               overwritePlttNoOffset;// パレット番号(オフセット加算)
    u16                               pad16_;
}
NNSG2dRendererInstance;






//------------------------------------------------------------------------------
// 初期化関連
void NNS_G2dInitRenderer( NNSG2dRendererInstance* pRend );
void NNS_G2dAddRendererTargetSurface
( 
    NNSG2dRendererInstance* pRend, 
    NNSG2dRenderSurface* pNew 
);
void NNS_G2dInitRenderSurface( NNSG2dRenderSurface* pSurface );
void NNS_G2dSetRendererFlipMode
( 
    NNSG2dRendererInstance* pRend, 
    BOOL bFlipH, 
    BOOL bFlipV 
);

//------------------------------------------------------------------------------
// 描画関連


void NNS_G2dBeginRendering   ( NNSG2dRendererInstance* pRendererInstance );
void NNS_G2dBeginRenderingEx ( NNSG2dRendererInstance* pRendererInstance, u32 opzHint );

void NNS_G2dDrawCell           ( const NNSG2dCellData* pCell );
void NNS_G2dDrawCellAnimation  ( const NNSG2dCellAnimation* pCellAnim );
void NNS_G2dDrawMultiCell      ( const NNSG2dMultiCellInstance* pMC );
void NNS_G2dDrawMultiCellAnimation( const NNSG2dMultiCellAnimation* pMCAnim );

void NNS_G2dDrawEntity         ( NNSG2dEntity* pEntity );

void NNS_G2dEndRendering   ( );

//------------------------------------------------------------------------------
// 行列操作
void NNS_G2dPushMtx();
void NNS_G2dPopMtx();
void NNS_G2dTranslate(fx32 x, fx32 y, fx32 z );
void NNS_G2dSetTrans(fx32 x, fx32 y, fx32 z );
void NNS_G2dScale(fx32 x, fx32 y, fx32 z );
void NNS_G2dRotZ( fx32 sin, fx32 cos );





//------------------------------------------------------------------------------
// パレット変更関連
void NNS_G2dSetRendererPaletteTbl
( 
    NNSG2dRendererInstance*         pRend, 
    const NNSG2dPaletteSwapTable*   pTbl 
);
const NNSG2dPaletteSwapTable* NNS_G2dGetRendererPaletteTbl
( 
    NNSG2dRendererInstance* pRend 
);
void NNS_G2dResetRendererPaletteTbl
( 
    NNSG2dRendererInstance* pRend 
);

//------------------------------------------------------------------------------
// 画像情報関連
void NNS_G2dSetRendererImageProxy
( 
    NNSG2dRendererInstance*             pRend, 
    const NNSG2dImageProxy*             pImgProxy, 
    const NNSG2dImagePaletteProxy*      pPltProxy
);





//------------------------------------------------------------------------------
// inline 
//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNS_G2dSetRendererSpriteZoffset
( 
    NNSG2dRendererInstance* pRend, 
    fx32                    spriteZoffset 
)
{
    pRend->spriteZoffsetStep = spriteZoffset;
}



//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNS_G2dSetRendererAffineOverwriteMode
( 
    NNSG2dRendererInstance*                 pRend, 
    NNSG2dRendererAffineTypeOverwiteMode    affineMode
)
{
    NNS_G2D_NULL_ASSERT( pRend );
    pRend->rendererCore.affineOverwriteMode = affineMode;
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE NNSG2dRendererAffineTypeOverwiteMode 
NNS_G2dGetRendererAffineOverwriteMode
( 
    const NNSG2dRendererInstance* pRend
)
{
    NNS_G2D_NULL_ASSERT( pRend );
    return pRend->rendererCore.affineOverwriteMode;
}

//------------------------------------------------------------------------------
// Core へ 移動
NNS_G2D_INLINE void NNS_G2dSetRendererSurfaceActiveFlag
( 
    NNSG2dRenderSurface*    pSurface, 
    BOOL                    bActive
)
{
    NNS_G2D_NULL_ASSERT( pSurface );
    pSurface->coreSurface.bActive = bActive;
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL NNS_G2dGetRendererSurfaceActiveFlag
( 
    const NNSG2dRenderSurface*    pSurface
)
{
    NNS_G2D_NULL_ASSERT( pSurface );
    return pSurface->coreSurface.bActive;
}

//------------------------------------------------------------------------------
// アクセサ 描画優先度
NNS_G2D_INLINE void NNS_G2dSetRendererOverwritePriority
(
    NNSG2dRendererInstance*     pRend,
    u16                         Priority
)
{
    pRend->overwritePriority = Priority;
}

NNS_G2D_INLINE u16 NNS_G2dGetRendererOverwritePriority
(
    const NNSG2dRendererInstance* pRend
)
{
    return pRend->overwritePriority;
}
//------------------------------------------------------------------------------
// アクセサ
// パレット番号
NNS_G2D_INLINE void NNS_G2dSetRendererOverwritePlttNo
(
    NNSG2dRendererInstance*     pRend,
    u16                         plttNo
)
{
    pRend->overwritePlttNo = plttNo;
}

NNS_G2D_INLINE u16 NNS_G2dGetRendererOverwritePlttNo
(
    const NNSG2dRendererInstance* pRend    
)
{
    return pRend->overwritePlttNo;
}

//------------------------------------------------------------------------------
// 開発中です
NNS_G2D_INLINE void NNS_G2dSetRendererOverwritePlttNoOffset
(
    NNSG2dRendererInstance*     pRend,
    u16                         plttNoOffset
)
{
    pRend->overwritePlttNoOffset = plttNoOffset;
}

//------------------------------------------------------------------------------
// 開発中です
NNS_G2D_INLINE u16 NNS_G2dGetRendererOverwritePlttNoOffset
(
    const NNSG2dRendererInstance* pRend    
)
{
    return pRend->overwritePlttNoOffset;
}


NNS_G2D_INLINE void NNS_G2dSetRendererOverwriteMosaicFlag
(
    NNSG2dRendererInstance*     pRend,
    BOOL                        overwriteMosaicFlag
)
{
    pRend->overwriteMosaicFlag = overwriteMosaicFlag;
}

NNS_G2D_INLINE BOOL NNS_G2dGetRendererOverwriteMosaicFlag
(
    const NNSG2dRendererInstance* pRend    
)
{
    return pRend->overwriteMosaicFlag;
}

NNS_G2D_INLINE void NNS_G2dSetRendererOverwriteOBJMode
(
    NNSG2dRendererInstance*     pRend,
    GXOamMode                   overwriteObjMode
)
{
    pRend->overwriteObjMode = overwriteObjMode;
}

NNS_G2D_INLINE GXOamMode NNS_G2dGetRendererOverwriteOBJMode
(
    const NNSG2dRendererInstance* pRend    
)
{
    return pRend->overwriteObjMode;
}



//------------------------------------------------------------------------------
// OAM上書きフラグ操作
NNS_G2D_INLINE void 
NNS_G2dSetRendererOverwriteEnable
( 
    NNSG2dRendererInstance* pRnd, 
    NNSG2dRendererOverwriteParam flag 
)
{
    pRnd->overwriteEnableFlag |= flag;
}

NNS_G2D_INLINE void 
NNS_G2dSetRendererOverwriteDisable
( 
    NNSG2dRendererInstance* pRnd, 
    NNSG2dRendererOverwriteParam flag 
)
{
    pRnd->overwriteEnableFlag &= ~flag;
}
 
NNS_G2D_INLINE BOOL 
NNS_G2dIsRendererOverwriteEnable
( 
    const NNSG2dRendererInstance* pRnd, 
    NNSG2dRendererOverwriteParam flag 
)
{
    return (BOOL)( pRnd->overwriteEnableFlag & flag );
}


//------------------------------------------------------------------------------
// 描画最適化関連
/*---------------------------------------------------------------------------*
  Name:         NNS_G2dOptimizeRenderer

  Description:  レンダラの最適化を行います。
                本APIは廃止になりました。
                本関数を呼び出すと、警告メッセージが出力されます。
                
                本関数の代わりに、NNS_G2dBeginRenderingEx()が用意されました。
                
                
  Arguments:    pRend:      レンダラ実体
                
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dOptimizeRenderer( NNSG2dRendererInstance* pRend )
{
#pragma unused( pRend )
    OS_Warning("Sorry, NNS_G2dOptimizeRenderer() is obsolete.Please avoid calling this func.");
}



#ifdef __cplusplus
} /* extern "C" */
#endif



#endif // NNS_G2D_RENDERER_H_
