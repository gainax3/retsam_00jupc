/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d
  File:     g2d_Renderer.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.61 $
 *---------------------------------------------------------------------------*/
#include <nnsys/g2d/g2d_Renderer.h>

#include "g2d_Internal.h"
#include "g2di_RendererMtxStack.hpp"    // For MatrixStack 
#include "g2di_RendererMtxState.h"      // For MatrixCache & State


#include <nnsys/g2d/fmt/g2d_Oam_data.h>
#include <nnsys/g2d/g2d_OamSoftwareSpriteDraw.h> // Auto Z offset 
#include <nnsys/g2d/g2d_SRTControl.h>








//------------------------------------------------------------------------------
#define NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT( pRnd ) \
    NNS_G2D_ASSERTMSG( (pRnd) != NULL, "Please call this method between Begin - End Rendering" ) \


//------------------------------------------------------------------------------
// 現在のレンダラ実体
// Begin-End Rendering() 呼び出し時に設定されます
static NNSG2dRendererInstance*      pCurrentInstance_   = NULL; 


//------------------------------------------------------------------------------
// 追加変数：Vram転送アニメのために追加
static u32      currenVramTransferHandle_ = NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE;




typedef struct MCRenderState
{
    u16                         currentCellAnimIdx;
    u16                         pad16_;
    NNSG2dRndCore2DMtxCache*    cellAnimMtxCacheTbl[256];
    BOOL                        bDrawMC;

}MCRenderState;

static MCRenderState        mcRenderState_;







//------------------------------------------------------------------------------
// NNSG2dRenderSurface リストが循環参照していないか調査します
// NNSG2dRenderSurface 登録などを行う 初期化時にASSERTマクロ内で呼ばれます。
// 
static BOOL IsNotCircularLinked_
( 
    const NNSG2dRenderSurface*      pList, 
    const NNSG2dRenderSurface*      pNew 
)
{
    const NNSG2dRenderSurface*  pCursor = pList;
    
    while( pCursor != NULL )
    {
        // 同じポインタを発見した
        if( pCursor == pNew ) 
        {
            // NG 
            return FALSE;
        }    
        pCursor = (const NNSG2dRenderSurface*)pCursor->pNextSurface;
    }
    
    // 循環参照が発生していない
    return TRUE;
}




//------------------------------------------------------------------------------
// OBJ の OAMAttiburte 内の パレット番号書き換えを 処理します
// DrawCellToSurface2D_() 
// DrawCellToSurface3D_()
// 内で実際に 描画命令が発行される直前に実行されます
// 
static NNS_G2D_INLINE void OBJPaletteChangeHandling_( GXOamAttr* pOam )
{
    const NNSG2dPaletteSwapTable* pTbl 
        = NNS_G2dGetRendererPaletteTbl( pCurrentInstance_ );
    
    NNS_G2D_NULL_ASSERT( pOam );
    
    if( pTbl != NULL )
    {
        // パレット番号を書き換えます
        const u16 newIdx 
           = NNS_G2dGetPaletteTableValue( pTbl, NNSi_G2dGetOamColorParam( pOam ) );
        pOam->cParam = newIdx;
    }
}

//------------------------------------------------------------------------------
// レンダラのフリップ状況を判定して、平行移動を設定します
static NNS_G2D_INLINE void FlipTranslate_( int x, int y )
{
    const int x_ = NNS_G2dIsRndCoreFlipH( &pCurrentInstance_->rendererCore ) ? -x : x;    
    const int y_ = NNS_G2dIsRndCoreFlipV( &pCurrentInstance_->rendererCore ) ? -y : y;
                            
    NNS_G2dTranslate( x_ << FX32_SHIFT , y_ << FX32_SHIFT, 0 );
}

//------------------------------------------------------------------------------
// NNSG2dSRTControl の 持つ affine変換情報をカレント行列に乗算します
// NNSG2dSRTControl の 種類は NNS_G2D_SRTCONTROLTYPE_SRT である必要があります。
//
//
static NNS_G2D_INLINE void SetSrtControlToMtxStack_( const NNSG2dSRTControl* pSrtCtrl )
{
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT( pCurrentInstance_ );
    NNS_G2D_NULL_ASSERT( pSrtCtrl );
    NNS_G2D_ASSERTMSG( pSrtCtrl->type == NNS_G2D_SRTCONTROLTYPE_SRT, "TODO: show msg " );
    
    // T
    if( NNSi_G2dSrtcIsAffineEnable( pSrtCtrl, NNS_G2D_AFFINEENABLE_TRANS ) )
    {
        //
        // フリップの結果を反映して、平行移動パラメータを更新します。
        //
        FlipTranslate_( pSrtCtrl->srtData.trans.x, pSrtCtrl->srtData.trans.y );                 
    }
    
    // R 
    if( NNSi_G2dSrtcIsAffineEnable( pSrtCtrl, NNS_G2D_AFFINEENABLE_ROTATE ) )
    {
        NNS_G2dRotZ( FX_SinIdx( pSrtCtrl->srtData.rotZ ), FX_CosIdx( pSrtCtrl->srtData.rotZ ) );
    }
    
    // S
    if( NNSi_G2dSrtcIsAffineEnable( pSrtCtrl, NNS_G2D_AFFINEENABLE_SCALE ) )
    {
        NNS_G2dScale( pSrtCtrl->srtData.scale.x, pSrtCtrl->srtData.scale.y, FX32_ONE );
    }

}


//------------------------------------------------------------------------------
// Vram 転送を利用するCellの描画の前後でコールします
static NNS_G2D_INLINE void BeginDrawVramTransferedCell_( u32 cellVramTransferHandle )
{
    NNS_G2D_ASSERT( currenVramTransferHandle_ == NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE );
    NNS_G2D_ASSERT( cellVramTransferHandle != NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE );
    
    currenVramTransferHandle_ = cellVramTransferHandle;
}

//------------------------------------------------------------------------------
// Vram 転送を利用するCellの描画の前後でコールします
static NNS_G2D_INLINE void EndDrawVramTransferedCell_( )
{
    NNS_G2D_ASSERT( currenVramTransferHandle_ != NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE );    
    currenVramTransferHandle_ = NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE;
}

//------------------------------------------------------------------------------
// レンダラはVram転送アニメCellを描画中か？
static NNS_G2D_INLINE BOOL IsRendererDrawingVramTransferedCell_( )
{
    return (BOOL)(currenVramTransferHandle_ != NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE);
}

//------------------------------------------------------------------------------
// 現在のram転送アニメCellのハンドルを取得します。
static NNS_G2D_INLINE u32 GetCurrentVramTransfereHandle_()
{
    return currenVramTransferHandle_;
}



//------------------------------------------------------------------------------
// レンダラコアモジュールにコールバックを登録し、動作をカスタマイズします。
// レンダラコアモジュールはカスタマイズによって、従来のレンダラモジュールと同等の動作を実現します。
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// セル描画前コールバック
// ●カリング処理
// ●2Dマトリクスキャッシュへのカレント行列のロード
// ●レンダラモジュールのセル描画前コールバックの呼び出し
//
static void RndCoreCBFuncBeforeCell_
(
    struct NNSG2dRndCoreInstance*   pRend,
    const NNSG2dCellData*           pCell
)    
{   
    NNS_G2D_NULL_ASSERT( pCurrentInstance_ );
    NNS_G2D_NULL_ASSERT( pCurrentInstance_->pCurrentSurface );
    {
       NNSG2dRenderSurface* pCurrentSurface = pCurrentInstance_->pCurrentSurface;
        
       //
       // カリング処理
       //
       if( pCurrentSurface->pFuncVisibilityCulling != NULL )
       { 
            
           if( !(*pCurrentSurface->pFuncVisibilityCulling)( pCell, 
                                                     NNSi_G2dGetCurrentMtx() , 
                                                     &pCurrentSurface->coreSurface.viewRect ) )
           {
              // 
              // pRend に 描画をスキップするように設定する
              // 
              pRend->bDrawEnable = FALSE;
              return;
           }else{
              pRend->bDrawEnable = TRUE;
           }
       }
       
       //
       // 従来のコールバックの呼び出し
       //
       if( *pCurrentSurface->pBeforeDrawCellBackFunc )
       {   
           (*pCurrentSurface->pBeforeDrawCellBackFunc)( pCurrentInstance_,
                                                 pCurrentSurface,
                                                 pCell,
                                                 NNSi_G2dGetCurrentMtx() );
       }
    }
}
//------------------------------------------------------------------------------
// セル描画後コールバック
//
// レンダコアモジュールにコールバック関数として登録されます。
static void RndCoreCBFuncAfterCell_
(
    struct NNSG2dRndCoreInstance*   pRend,
    const NNSG2dCellData*           pCell
)    
{
#pragma unused( pRend )
    NNS_G2D_NULL_ASSERT( pCurrentInstance_ );
    NNS_G2D_NULL_ASSERT( pCurrentInstance_->pCurrentSurface );
    {
       NNSG2dRenderSurface* pCurrentSurface = pCurrentInstance_->pCurrentSurface;
    
       if( *pCurrentSurface->pAfterDrawCellBackFunc )
       {
           (*pCurrentSurface->pAfterDrawCellBackFunc)( pCurrentInstance_,
                                                 pCurrentSurface,
                                                 pCell,
                                                 NNSi_G2dGetCurrentMtx() );
       }
    }
}
//------------------------------------------------------------------------------
// OBJ描画前コールバック
//
// レンダコアモジュールにコールバック関数として登録されます。
//
// Oamパラメータの上書き処理を行います。
// 
// 現在の実装では、pBeforeDrawOamBackFunc 呼び出しの前に パラメータ書き換えを行っているので
// pBeforeDrawOamBackFunc 内でOBJ単位カリングを実施する場合などに、描画されないOBJ
// に対するパラメータ上書き処理が発生してしまう可能性があります。
//
// OBJ単位カリングが実施されるOBJは少数のはずで、その場合の処理効率を重要視しないという判断のもとに
// 現在の実装になっています。
//
// 
static void RndCoreCBFuncBeforeOBJ_
(
    struct NNSG2dRndCoreInstance*   pRend,
    const NNSG2dCellData*           pCell,
    u16                             oamIdx
)
{
    GXOamAttr*    pTempOam = &pRend->currentOam;
    
    NNS_G2D_NULL_ASSERT( pCurrentInstance_ );
    NNS_G2D_NULL_ASSERT( pCurrentInstance_->pCurrentSurface );
    
    //
    // パラメータ書き換え
    //
    // パレット変換テーブル
    OBJPaletteChangeHandling_( pTempOam );
    
    if( pCurrentInstance_->overwriteEnableFlag != NNS_G2D_RND_OVERWRITE_NONE )
    {
       // 描画優先    
       if( NNS_G2dIsRendererOverwriteEnable( pCurrentInstance_, NNS_G2D_RND_OVERWRITE_PRIORITY ) )
       {
           pTempOam->priority = pCurrentInstance_->overwritePriority;
       }
        
       // パレット番号
       // 注意：こちらの方が優先されます。（パレット変換テーブルの結果を上書きします）
       if( NNS_G2dIsRendererOverwriteEnable( pCurrentInstance_, NNS_G2D_RND_OVERWRITE_PLTTNO ) )
       {
           pTempOam->cParam = pCurrentInstance_->overwritePlttNo;
       }
       
       // パレット番号(オフセットを加算する)
       if( NNS_G2dIsRendererOverwriteEnable( pCurrentInstance_, NNS_G2D_RND_OVERWRITE_PLTTNO_OFFS ) )
       {
           pTempOam->cParam = 0xF & ( pTempOam->cParam + pCurrentInstance_->overwritePlttNoOffset );
       }
       
       // モザイク
       if( NNS_G2dIsRendererOverwriteEnable( pCurrentInstance_, NNS_G2D_RND_OVERWRITE_MOSAIC ) )
       {
           G2_OBJMosaic( pTempOam, pCurrentInstance_->overwriteMosaicFlag );
       }
        
       // OBJモード
       if( NNS_G2dIsRendererOverwriteEnable( pCurrentInstance_, NNS_G2D_RND_OVERWRITE_OBJMODE ) )
       {
           G2_SetOBJMode( pTempOam, pCurrentInstance_->overwriteObjMode, G2_GetOBJColorParam(pTempOam));
       }
    }
    
    //
    // コールバック呼び出し
    //
    {
       NNSG2dRenderSurface* pCurrentSurface = pCurrentInstance_->pCurrentSurface;
       if( *pCurrentSurface->pBeforeDrawOamBackFunc )
       {
           (*pCurrentSurface->pBeforeDrawOamBackFunc)( pCurrentInstance_,
                                                 pCurrentSurface,
                                                 pCell,
                                                 oamIdx,
                                                 NNSi_G2dGetCurrentMtx() );
       }
    }
}
//------------------------------------------------------------------------------
// OBJ描画後コールバック
//
// レンダコアモジュールにコールバック関数として登録されます。
static void RndCoreCBFuncAfterOBJ_
(
    struct NNSG2dRndCoreInstance*   pRend,
    const NNSG2dCellData*           pCell,
    u16                             oamIdx
)
{
#pragma unused( pRend )
    NNS_G2D_NULL_ASSERT( pCurrentInstance_ );
    NNS_G2D_NULL_ASSERT( pCurrentInstance_->pCurrentSurface );
    {
       NNSG2dRenderSurface* pCurrentSurface = pCurrentInstance_->pCurrentSurface;
       if( *pCurrentSurface->pAfterDrawOamBackFunc )
       {
           (*pCurrentSurface->pAfterDrawOamBackFunc)( pCurrentInstance_,
                                                 pCurrentSurface,
                                                 pCell,
                                                 oamIdx,
                                                 NNSi_G2dGetCurrentMtx() );
       }
    }
}



//------------------------------------------------------------------------------
// レンダコア 2D サーフェスに対するレンダリングを開始します。
static NNS_G2D_INLINE void BeginRndCoreRendering2D_
(
    NNSG2dRendererInstance*  pRnd,
    NNSG2dRenderSurface*     pSurface 
)
{
    NNS_G2D_NULL_ASSERT( pRnd );
    NNS_G2D_ASSERT( pRnd->pCurrentSurface == NULL ); 
    NNS_G2D_ASSERT( pSurface->type != NNS_G2D_SURFACETYPE_MAIN3D );
    NNS_G2D_NULL_ASSERT( pSurface );
    
    //
    // レンダコアに、サーフェス設定を行う
    //
    pRnd->pCurrentSurface = pSurface;
    NNS_G2dSetRndCoreSurface( &pRnd->rendererCore, &pSurface->coreSurface );
       
    //
    // 2D 描画用に登録関数の設定
    //
    {
        {
            NNS_G2dSetRndCoreOamRegisterFunc( &pRnd->rendererCore,
                                              pSurface->pFuncOamRegister,
                                              pSurface->pFuncOamAffineRegister );
        }
    }
    
    NNS_G2dRndCoreBeginRendering( &pRnd->rendererCore );
}

//------------------------------------------------------------------------------
// レンダコア 3D サーフェスに対するレンダリングを開始します。
static NNS_G2D_INLINE void BeginRndCoreRendering3D_
(
    NNSG2dRendererInstance*  pRnd, 
    NNSG2dRenderSurface*     pSurface 
)
{
    NNS_G2D_NULL_ASSERT( pRnd );
    NNS_G2D_ASSERT( pRnd->pCurrentSurface == NULL ); 
    NNS_G2D_ASSERT( pSurface->type == NNS_G2D_SURFACETYPE_MAIN3D );
    NNS_G2D_NULL_ASSERT( pSurface );
    
    //
    // レンダコアに、サーフェス設定を行う
    //
    pRnd->pCurrentSurface = pSurface;
    NNS_G2dSetRndCoreSurface( &pRnd->rendererCore, &pSurface->coreSurface );
       
    
    NNS_G2dRndCoreBeginRendering( &pRnd->rendererCore );
}

//------------------------------------------------------------------------------
// レンダコアの描画を終了します。
static NNS_G2D_INLINE void EndRndCoreRendering_( void )
{
    NNS_G2D_NULL_ASSERT( pCurrentInstance_ );
    NNS_G2D_NULL_ASSERT( pCurrentInstance_->pCurrentSurface );
    
    pCurrentInstance_->pCurrentSurface = NULL;
    NNS_G2dRndCoreEndRendering( );
}


//------------------------------------------------------------------------------
// レンダコアモジュール 2D描画
// DrawCellImpl_ 関数の可読性向上のために別関数としています。
static NNS_G2D_INLINE void DoRenderByRndCore2D_( 
    const NNSG2dCellData*    pCell ,
    NNSG2dRndCoreInstance*   pRndCore
)
{
#pragma unused( pRndCore )
    NNS_G2D_NULL_ASSERT( pCell );
    NNS_G2D_NULL_ASSERT( pRndCore );
    //
    // レンダラにマトリクスキャッシュを設定します。
    // (事前にZ値を設定しておく必要があります。)
    //
    {
       NNSG2dRndCore2DMtxCache* pMtx2D = NULL;

       //
       // アフィン変換していれば、2Dアフィン変換用のMtxCacheを設定する。
       //
       if( NNSi_G2dIsRndCurrentMtxSRTransformed() )
       {
           //
           // マルチセルの描画中ならば...
           //
           if( mcRenderState_.bDrawMC )
           {
               //
               // 以前に読み込まれていないかテーブルを確認する
               // マルチセル内の同一セルアニメーションは必ず同じアフィンパラメータを参照するはず。
               // レンダラは一度アフィン変換を使用して描画したセルアニメーションの行列キャッシュをテーブルに記憶している。
               //
               pMtx2D 
                  = mcRenderState_.cellAnimMtxCacheTbl[mcRenderState_.currentCellAnimIdx];
               //
               // 以前に描画されたことのないセルアニメーションならば...
               //
               if( pMtx2D == NULL )
               {
                  //
                  // カレント行列をマトリクスキャッシュにロードします。
                  //
                  NNSi_G2dMCMStoreCurrentMtxToMtxCache();
                  //
                  // ロードが完了した、マトリクスキャッシュを取得します
                  //
                  pMtx2D = NNSi_G2dMCMGetCurrentMtxCache();
                  //
                  // 行列キャッシュをテーブルに記憶します
                  //
                  mcRenderState_.cellAnimMtxCacheTbl[mcRenderState_.currentCellAnimIdx] = pMtx2D;
              }
           }else{
               //
               // カレント行列をマトリクスキャッシュにロードします。
               //
               NNSi_G2dMCMStoreCurrentMtxToMtxCache();
               //
               // ロードが完了した、マトリクスキャッシュを取得します
               //
               pMtx2D = NNSi_G2dMCMGetCurrentMtxCache();
           }
       }                                                   
       //
       // アフィン変換用の行列キャッシュを設定
       //
       NNS_G2dSetRndCoreCurrentMtx2D( NNSi_G2dGetCurrentMtx(), pMtx2D );
    }
    
    
    //
    // レンダコアに描画処理を委譲する
    //
    if( IsRendererDrawingVramTransferedCell_( ) )
    {
        NNS_G2dRndCoreDrawCellVramTransfer( pCell, GetCurrentVramTransfereHandle_() );
    }else{
        NNS_G2dRndCoreDrawCell( pCell );
    }
}
              
//------------------------------------------------------------------------------
// レンダコアモジュール 3D描画
// DrawCellImpl_ 関数の可読性向上のために別関数としています。
static NNS_G2D_INLINE void DoRenderByRndCore3D_
( 
    const NNSG2dCellData*    pCell ,
    NNSG2dRndCoreInstance*   pRndCore
)
{
    NNS_G2D_NULL_ASSERT( pCell );
    NNS_G2D_NULL_ASSERT( pRndCore );
    
    //
    // 3D 用 Z値を設定します。
    //
    NNS_G2dSetRndCore3DSoftSpriteZvalue( pRndCore, NNSi_G2dGetCurrentZ() );
    //
    // レンダラコアにカレント行列を設定します。
    // (事前にZ値を設定しておく必要があります。)
    //
    NNS_G2dSetRndCoreCurrentMtx3D( NNSi_G2dGetCurrentMtx() );
    //
    // レンダコアに描画処理を委譲する
    //
    if( IsRendererDrawingVramTransferedCell_( ) )
    {
        NNS_G2dRndCoreDrawCellVramTransfer( pCell, GetCurrentVramTransfereHandle_() );
    }else{
        NNS_G2dRndCoreDrawCell( pCell );
    }
}

//------------------------------------------------------------------------------
// セルを描画します。
static void DrawCellImpl_( const NNSG2dCellData* pCell )
{
    NNSG2dRndCoreInstance*   pRndCore   = NULL;
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT( pCurrentInstance_ );
    NNS_G2D_NULL_ASSERT( pCell );

    pRndCore = &pCurrentInstance_->rendererCore;
    
    {
        NNSG2dRenderSurface*      pSurface = pCurrentInstance_->pTargetSurfaceList;
        //
        // サーフェスを一枚しか利用しない場合は、セル描画ごとに行われる、レンダコアへのサーフェス
        // パラメータの設定を回避可能です。
        //
        if( pCurrentInstance_->opzHint & NNS_G2D_RDR_OPZHINT_LOCK_PARAMS )
        {
           // 
           // 描画関数ごとの BeginRndCoreRenderingXX_() EndRndCoreRendering_()
           // 呼び出しを行いません。その分高速に動作します。
           // 
           //
           // 2D サーフェスに対する描画ならば...
           //       
           if( pSurface->type != NNS_G2D_SURFACETYPE_MAIN3D )
           {
              //
              // 実際の描画処理( 2D )
              //                    
              DoRenderByRndCore2D_( pCell, pRndCore );    
           }else{
              //
              // 実際の描画処理( 3D )
              //                    
              DoRenderByRndCore3D_( pCell, pRndCore );
           }
        }else{
           // for each render surfaces...     
           while( pSurface )
           {
              if( pSurface->bActive )
              {                
                  //
                  // 2D サーフェスに対する描画ならば...
                  //
                  if( pSurface->type != NNS_G2D_SURFACETYPE_MAIN3D )
                  {
                     //
                     // 実際の描画処理( 2D )
                     //                    
                     BeginRndCoreRendering2D_( pCurrentInstance_, pSurface );
                         DoRenderByRndCore2D_( pCell, pRndCore );
                     EndRndCoreRendering_();
                  }else{
                     //
                     // 実際の描画処理( 3D )
                     //                    
                     BeginRndCoreRendering3D_( pCurrentInstance_, pSurface );
                         DoRenderByRndCore3D_( pCell, pRndCore );
                     EndRndCoreRendering_();
                  }
              }
              // Next surface ... 
              pSurface = pSurface->pNextSurface;
           }
        }
    }
}



//------------------------------------------------------------------------------
static void DrawCellAnimationImpl_( const NNSG2dCellAnimation* pCellAnim )
{
    NNSG2dCellData*      pCell = NULL;
    NNS_G2D_NULL_ASSERT( pCellAnim );
    
    pCell = (NNSG2dCellData*)NNS_G2dGetCellAnimationCurrentCell( pCellAnim );
    NNS_G2D_NULL_ASSERT( pCell );
           
    //
    // SRTアニメーションが使用されていない場合は、無用な Push Pop を 回避します。
    //
    if( pCellAnim->srtCtrl.srtData.SRT_EnableFlag == NNS_G2D_AFFINEENABLE_NONE )
    {
           //
           // VRAM 転送アニメを利用するCell ならば...
           //
           if( NNSi_G2dIsCellAnimVramTransferHandleValid( pCellAnim ) )
           {
                
              BeginDrawVramTransferedCell_( NNSi_G2dGetCellAnimVramTransferHandle( pCellAnim ) );
                  DrawCellImpl_( pCell );
              EndDrawVramTransferedCell_();
                
           }else{
                  DrawCellImpl_( pCell );
           }
    }else{
       NNS_G2dPushMtx(); 
           SetSrtControlToMtxStack_( &pCellAnim->srtCtrl );        
           //
           // VRAM 転送アニメを利用するCell ならば...
           //
           if( NNSi_G2dIsCellAnimVramTransferHandleValid( pCellAnim ) )
           {
                
              BeginDrawVramTransferedCell_( NNSi_G2dGetCellAnimVramTransferHandle( pCellAnim ) );
                  DrawCellImpl_( pCell );
              EndDrawVramTransferedCell_();
                
           }else{
                  DrawCellImpl_( pCell );
           }     
       NNS_G2dPopMtx(1);
    }
}


//------------------------------------------------------------------------------
// Node の 描画をします
// 外部公開関数から内部関数に変更になりました。
static void DrawNode_( const NNSG2dNode* pNode )
{
    NNS_G2D_NULL_ASSERT( pNode );
    NNS_G2D_ASSERTMSG( pNode->type == NNS_G2D_NODETYPE_CELL, "NNS_G2D_NODETYPE_CELL is expected." );
    
    if( pNode->bVisible )
    {
        //
        // TODO:
        // codes below this line should be "pNode->type" depended.
        // for now, we expect the "pNode->type" is always "NNS_G2D_NODETYPE_CELL"
        //        
        NNSG2dCellAnimation*    pCellAnim = (NNSG2dCellAnimation*)pNode->pContent;
        NNS_G2D_NULL_ASSERT( pCellAnim );
        
        //
        // pNode->srtCtrl.srtData.SRT_EnableFlag == NNS_G2D_AFFINEENABLE_NONE
        // となる場合は少ないはずなので、他所のように条件分岐によってPush Pop を回避する
        // 処理をしません。
        // 
        NNS_G2dPushMtx();
            SetSrtControlToMtxStack_( &pNode->srtCtrl );    
            {                       
                DrawCellAnimationImpl_( pCellAnim );
            }
        NNS_G2dPopMtx(1);
    }
}

//------------------------------------------------------------------------------
static NNS_G2D_INLINE void DrawNode2_
( 
    const NNSG2dMultiCellInstance*         pMC, 
    const NNSG2dMultiCellHierarchyData*    pNodeData 
)
{
    const u16 cellAnimIdx          = NNSi_G2dGetMC2NodeCellAinmIdx( pNodeData );
    const NNSG2dMCCellAnimation*   cellAnimArray = pMC->pCellAnimInstasnces;
    NNS_G2D_MINMAX_ASSERT( cellAnimIdx, 0, pMC->pCurrentMultiCell->numCellAnim );
    mcRenderState_.currentCellAnimIdx = cellAnimIdx;
    
    NNS_G2dPushMtx();
       FlipTranslate_( pNodeData->posX, pNodeData->posY );
       {                       
           DrawCellAnimationImpl_( &cellAnimArray[cellAnimIdx].cellAnim );
       }
    NNS_G2dPopMtx(1);
}

//------------------------------------------------------------------------------
// 自動Z値オフセットが有効か？
static NNS_G2D_INLINE BOOL IsAutoZoffsetEnable_( void )
{
    NNS_G2D_NULL_ASSERT( pCurrentInstance_ );
    return (BOOL)pCurrentInstance_->spriteZoffsetStep;
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitRenderer

  Description:  レンダラ 実体を初期化します
                
                
  Arguments:    pRend:      [OUT] レンダラ 実体
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dInitRenderer( NNSG2dRendererInstance* pRend )
{
    NNS_G2D_NULL_ASSERT( pRend );
    
    NNS_G2dInitRndCore( &pRend->rendererCore );
    
    pRend->pTargetSurfaceList   = NULL;
    pRend->pCurrentSurface      = NULL;
    pRend->pPaletteSwapTbl      = NULL;
    
    //
    // 最適化ヒント、規定値ではすべて無効となっています(最適化は行われない)
    //
    pRend->opzHint = NNS_G2D_RDR_OPZHINT_NONE;
    
    pRend->spriteZoffsetStep = 0;
    
    // overwriteEnableFlag は NNSG2dRendererOverwriteParam
    pRend->overwriteEnableFlag = NNS_G2D_RND_OVERWRITE_NONE; 
    pRend->overwritePriority   = 0;
    pRend->overwritePlttNo     = 0;  
    pRend->overwriteObjMode    = GX_OAM_MODE_NORMAL;
    pRend->overwriteMosaicFlag = FALSE;
    pRend->overwritePlttNoOffset = 0;
    
    //
    // マトリクスキャッシュの初期化
    //
    NNSi_G2dMCMInitMtxCache();
    //
    // マトリクススタックモジュールの利用モードを設定する。
    //
    NNSi_G2dSetRndMtxStackSRTransformEnableFlag( TRUE );
    
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dAddRendererTargetSurface

  Description:  レンダラ に NNSG2dRenderSurfaceを追加します
                
                
  Arguments:    pRend:      [OUT] レンダラ 実体
                pNew:       [IN]  追加する NNSG2dRenderSurface
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dAddRendererTargetSurface( NNSG2dRendererInstance* pRend, NNSG2dRenderSurface* pNew )
{
    NNS_G2D_NULL_ASSERT( pRend );
    NNS_G2D_NULL_ASSERT( pNew );
    NNS_G2D_ASSERTMSG( IsNotCircularLinked_( pRend->pTargetSurfaceList, pNew ),
        "Circular linked lists is detected in NNS_G2dAddRendererTargetSurface()" );
    
    // add_front
    pNew->pNextSurface          = pRend->pTargetSurfaceList;
    pRend->pTargetSurfaceList   = pNew;
    
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitRenderSurface

  Description:  レンダサーフェス を初期化します。
                内部でレンダラコアモジュールにコールバック関数を登録しています。
                
                
  Arguments:    pSurface:      [OUT] レンダサーフェス 実体
                
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dInitRenderSurface( NNSG2dRenderSurface* pSurface )
{
    NNS_G2D_NULL_ASSERT( pSurface );
    
    MI_CpuFill16( pSurface, 0x0, sizeof( NNSG2dRenderSurface ) );
    
    pSurface->coreSurface.bActive = TRUE;
    
    pSurface->coreSurface.type = NNS_G2D_SURFACETYPE_MAX;
    
    //
    // レンダラコアモジュールにコールバック関数を登録します
    //
    {
        NNSG2dRndCoreSurface* pS = &pSurface->coreSurface;
        
        pS->pBeforeDrawCellBackFunc   = RndCoreCBFuncBeforeCell_;
        pS->pAfterDrawCellBackFunc    = RndCoreCBFuncAfterCell_;
        pS->pBeforeDrawOamBackFunc    = RndCoreCBFuncBeforeOBJ_;
        pS->pAfterDrawOamBackFunc     = RndCoreCBFuncAfterOBJ_;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dBeginRendering

  Description:  レンダラ 描画前の設定を行います
                レンダラ 描画メソッドを呼ぶ前に呼んでください。
                Begin End Rendering 内 で 呼び出さないでください。
                
                他に、本関数と同様の役割を果たし、描画最適化のためのヒントフラグを指定可能な
                NNS_G2dBeginRenderingEx()関数が用意されています。
                
                
  Arguments:    pRendererInstance:      [IN]  レンダラ 実体
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dBeginRendering( NNSG2dRendererInstance* pRendererInstance )
{
    NNS_G2D_NULL_ASSERT( pRendererInstance );
    NNS_G2D_ASSERTMSG( pCurrentInstance_ == NULL, 
        "Must be NULL, Make sure calling Begin - End correctly." );
    
    pCurrentInstance_ = pRendererInstance;
    
    NNSi_G2dMCMCleanupMtxCache();
    
    G3_PushMtx();            
    
    G3_Identity();
    NNSi_G2dIdentity();
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dBeginRenderingEx

  Description:  レンダラ 描画前の設定を行います
                レンダラ 描画メソッドを呼ぶ前に呼んでください。
                Begin End Rendering 内 で 呼び出さないでください。
                本関数は、描画最適化のためのヒントフラグを指定可能です。
                
                ヒントは NNSG2dRendererOptimizeHint 列挙子の論理和
                で作成します。
                
                最適化ヒントフラグはNNS_G2dEndRendering()によってリセットされます。
                つまり、最適化ヒントフラグ は レンダラの Begin - End Rendering 
                ブロック内でのみ有効となります。
                
                本関数は、最適化のための前処理を行ったあと、
                通常のNNS_G2dBeginRendering()を呼び出しています。
                
                
  Arguments:    pRendererInstance       [IN]  レンダラ 
                opzHint                 [IN]  最適化ヒント
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dBeginRenderingEx 
( 
    NNSG2dRendererInstance* pRendererInstance, 
    u32                     opzHint 
)
{
    NNS_G2D_NULL_ASSERT( pRendererInstance );
    NNS_G2D_ASSERT( pRendererInstance->opzHint == NNS_G2D_RDR_OPZHINT_NONE );
    
    pRendererInstance->opzHint = opzHint;
    //
    // 描画最適化のための前処理
    //
    {
       if( opzHint & NNS_G2D_RDR_OPZHINT_NOT_SR )
       {
           NNSi_G2dSetRndMtxStackSRTransformEnableFlag( FALSE );
       }
       
       //
       // サーフェスを一枚しか利用しない場合は、セル描画ごとに行われる、レンダコアへのサーフェス
       // パラメータの設定を Begin - End のタイミングで一回行うのみに軽減可能です。
       //
       if( opzHint & NNS_G2D_RDR_OPZHINT_LOCK_PARAMS )
       {
           NNSG2dRndCoreInstance*  pRndCore = &pRendererInstance->rendererCore;
           NNSG2dRenderSurface*    pSurface = pRendererInstance->pTargetSurfaceList;
            
           NNS_G2D_ASSERTMSG( pSurface->pNextSurface == NULL,
              "The number of target surface must be ONE. when you spesified the NNS_G2D_RDR_OPZHINT_LOCK_PARAMS flag." );  
              
           if( pSurface->bActive )
           {                
              //
              // 2D サーフェスに対する描画ならば...
              //
              if( pSurface->type != NNS_G2D_SURFACETYPE_MAIN3D )
              {
                  BeginRndCoreRendering2D_( pRendererInstance, pSurface );
              }else{
                  BeginRndCoreRendering3D_( pRendererInstance, pSurface );
              }
           }
       }
    }

    
    NNS_G2dBeginRendering( pRendererInstance );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dEndRendering

  Description:  レンダラ 描画後の設定を行います
                描画後に変更された内部状態を元に戻します。
                
                最適化ヒントが指定されている(NNS_G2dBeginRenderingEx()を使用する)
                場合には、最適化処理の後処理を行います。
                
                最適化ヒントフラグは本関数によってリセットされます。
                つまり、最適化ヒントフラグ は レンダラの Begin - End Rendering 
                ブロック内でのみ有効となります。
                
                
  Arguments:    なし
  
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dEndRendering()
{
    // check push pop are called correctly.
    NNS_G2D_NULL_ASSERT( pCurrentInstance_ );
    
    
    G3_PopMtx(1);
    
    {
        //
        // 最適化フラグが設定されていれば、その処理をおこなう
        //
        const u32 opzHint = pCurrentInstance_->opzHint;
        if( opzHint != NNS_G2D_RDR_OPZHINT_NONE )
        {
            //
            // スタックの SR変換設定をもとに戻します。
            //
            if( opzHint & NNS_G2D_RDR_OPZHINT_NOT_SR )
            {
                NNSi_G2dSetRndMtxStackSRTransformEnableFlag( TRUE );
            }
            
            //
            // サーフェスを一枚しか利用しない場合は、セル描画ごとに行われる、レンダコアへのサーフェス
            // パラメータの設定を レンダラの Begin - End Renderingのタイミングで一回行うのみに軽減可能です。
            //
            if( opzHint & NNS_G2D_RDR_OPZHINT_LOCK_PARAMS )
            {
                EndRndCoreRendering_();
            }
            //
            // 最適化フラグをリセットします。
            // ( つまり最適化フラグ は Begin－End内でのみ有効です。)
            //
            pCurrentInstance_->opzHint = NNS_G2D_RDR_OPZHINT_NONE;
        }    
    }
    
    pCurrentInstance_ = NULL;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dDrawCell

  Description:  セル の 描画をします。
                
                
  Arguments:    pCell           : [IN]  描画する セル 実体             
                                  
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dDrawCell( const NNSG2dCellData* pCell )
{
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT( pCurrentInstance_ );
    NNS_G2D_NULL_ASSERT( pCell );
    
    if( IsAutoZoffsetEnable_() )
    {
        const fx32 offset = NNSi_G2dGetOamSoftEmuAutoZOffsetStep();
        NNSi_G2dSetOamSoftEmuAutoZOffsetFlag( TRUE );
        NNSi_G2dSetOamSoftEmuAutoZOffsetStep( pCurrentInstance_->spriteZoffsetStep );
        
        DrawCellImpl_( pCell );
        
        NNSi_G2dSetOamSoftEmuAutoZOffsetFlag( FALSE );
        NNSi_G2dSetOamSoftEmuAutoZOffsetStep( offset );
        NNSi_G2dResetOamSoftEmuAutoZOffset();
    }else{
        DrawCellImpl_( pCell );
    }
}
   


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dDrawCellAnimation

  Description:  セルアニメーション の 描画をします。
                
                
  Arguments:    pMC             : [IN]  描画する セルアニメーション        
                                  
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dDrawCellAnimation( const NNSG2dCellAnimation* pCellAnim )
{
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT( pCurrentInstance_ );
    NNS_G2D_NULL_ASSERT( pCellAnim );
    
    if( IsAutoZoffsetEnable_() )
    {
        const fx32 offset = NNSi_G2dGetOamSoftEmuAutoZOffsetStep();
        NNSi_G2dSetOamSoftEmuAutoZOffsetFlag( TRUE );
        NNSi_G2dSetOamSoftEmuAutoZOffsetStep( pCurrentInstance_->spriteZoffsetStep );
        
        DrawCellAnimationImpl_( pCellAnim );
        
        NNSi_G2dSetOamSoftEmuAutoZOffsetFlag( FALSE );
        NNSi_G2dSetOamSoftEmuAutoZOffsetStep( offset );
        NNSi_G2dResetOamSoftEmuAutoZOffset();
    }else{
        DrawCellAnimationImpl_( pCellAnim );
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dDrawMultiCell

  Description:  マルチセル の 描画をします。
                
                
  Arguments:    pMC             : [IN]  描画する マルチセル 実体             
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dDrawMultiCell
( 
    const NNSG2dMultiCellInstance*      pMC 
)
{
    u16 i;
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT( pCurrentInstance_ );
    NNS_G2D_NULL_ASSERT( pMC );
    if( pMC->mcType == NNS_G2D_MCTYPE_SHARE_CELLANIM )
    {
       //
       // マルチセル2D描画用の行列キャッシュテーブルを使用するセルアニメ数分だけ初期化します。
       //
       for( i = 0; i < pMC->pCurrentMultiCell->numCellAnim; i++ )
       {
           mcRenderState_.cellAnimMtxCacheTbl[i] = NULL;
       }
       mcRenderState_.bDrawMC = TRUE;
        
       if( IsAutoZoffsetEnable_() )
       {
           const fx32 offset = NNSi_G2dGetOamSoftEmuAutoZOffsetStep();
           NNSi_G2dSetOamSoftEmuAutoZOffsetFlag( TRUE );
           NNSi_G2dSetOamSoftEmuAutoZOffsetStep( pCurrentInstance_->spriteZoffsetStep );
            
           for( i = 0; i < pMC->pCurrentMultiCell->numNodes; i++ )
           {
              DrawNode2_( pMC, &pMC->pCurrentMultiCell->pHierDataArray[i] );
           }
            
           NNSi_G2dSetOamSoftEmuAutoZOffsetFlag( FALSE );
           NNSi_G2dSetOamSoftEmuAutoZOffsetStep( offset );
           NNSi_G2dResetOamSoftEmuAutoZOffset();
       }else{
           for( i = 0; i < pMC->pCurrentMultiCell->numNodes; i++ )
           {
              DrawNode2_( pMC, &pMC->pCurrentMultiCell->pHierDataArray[i] );
           }
       }
        
       mcRenderState_.bDrawMC = FALSE;
    
    }else{
       const NNSG2dNode* pNode = pMC->pCellAnimInstasnces;
       if( IsAutoZoffsetEnable_() )
       {
           const fx32 offset = NNSi_G2dGetOamSoftEmuAutoZOffsetStep();
           
           NNSi_G2dSetOamSoftEmuAutoZOffsetFlag( TRUE );
           NNSi_G2dSetOamSoftEmuAutoZOffsetStep( pCurrentInstance_->spriteZoffsetStep );
            
           for( i = 0; i < pMC->pCurrentMultiCell->numNodes; i++ )
           {
              DrawNode_( &pNode[i] );
           }
            
           NNSi_G2dSetOamSoftEmuAutoZOffsetFlag( FALSE );
           NNSi_G2dSetOamSoftEmuAutoZOffsetStep( offset );
           NNSi_G2dResetOamSoftEmuAutoZOffset();
       }else{
           for( i = 0; i < pMC->pCurrentMultiCell->numNodes; i++ )
           {
              DrawNode_( &pNode[i] );
           }
       }
    }
}






/*---------------------------------------------------------------------------*
  Name:         NNS_G2dDrawMultiCellAnimation

  Description:  マルチセルアニメーション の 描画をします。
                NNS_G2dDrawMultiCell() を内部で呼び出しています。
                
                マルチセルアニメーションのSRTアニメーション結果を
                考慮する点が、NNS_G2dDrawMultiCell()と異なる点です。
                
                
  Arguments:    pMCAnim         : [IN]  描画する マルチセルアニメーション
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dDrawMultiCellAnimation
( 
    const NNSG2dMultiCellAnimation*     pMCAnim 
)
{
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT( pCurrentInstance_ );
    NNS_G2D_NULL_ASSERT( pMCAnim );
    
    if( pMCAnim->srtCtrl.srtData.SRT_EnableFlag == NNS_G2D_AFFINEENABLE_NONE )
    {
        NNS_G2dDrawMultiCell( &pMCAnim->multiCellInstance );    
    }else{
        NNS_G2dPushMtx();
            SetSrtControlToMtxStack_( &pMCAnim->srtCtrl );
            NNS_G2dDrawMultiCell( &pMCAnim->multiCellInstance );    
        NNS_G2dPopMtx(1);
    }
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dDrawEntity

  Description:  Entity の 描画をします。
                エンティティデータがパレット変換テーブルを持っていた場合は、
                パレット変換テーブルを使用して描画を行います。
                
                
  Arguments:    pEntity          : [IN]  描画する Entity 実体             
                               
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dDrawEntity( NNSG2dEntity* pEntity )
{
    BOOL bAffined = FALSE;
    
    BOOL bPaletteChange                 = FALSE;
    const NNSG2dPaletteSwapTable* pTbl  = NULL;
    
    
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT( pCurrentInstance_ );
    NNS_G2D_NULL_ASSERT( pEntity );
    NNS_G2D_ASSERT_ENTITY_VALID( pEntity );
    
    //
    // パレットを設定します
    //
    bPaletteChange = NNS_G2dIsEntityPaletteTblEnable( pEntity );
    if( bPaletteChange )
    {
        pTbl = NNS_G2dGetRendererPaletteTbl( pCurrentInstance_ );
        NNS_G2dSetRendererPaletteTbl( pCurrentInstance_, pEntity->pPaletteTbl );
    }
    
    //
    // Entity の 種類に応じて描画をします
    //
    {       
        switch( pEntity->pEntityData->type )
        {
        case NNS_G2D_ENTITYTYPE_CELL:
            {
                NNSG2dCellAnimation*  
                  pCellAnim = (NNSG2dCellAnimation*)pEntity->pDrawStuff;
                NNS_G2D_NULL_ASSERT( pCellAnim );
                NNS_G2dDrawCellAnimation( pCellAnim );
            }
            break;
        case NNS_G2D_ENTITYTYPE_MULTICELL:
            {
                NNSG2dMultiCellAnimation*  
                  pMCAnim = (NNSG2dMultiCellAnimation*)pEntity->pDrawStuff;
                NNS_G2D_NULL_ASSERT( pMCAnim );
                NNS_G2dDrawMultiCellAnimation( pMCAnim );
            }
            break;
        default:
            NNS_G2D_ASSERTMSG( FALSE, "TODO: msg ");
        }    
    }
    
    //
    // パレットを元にもどします
    //
    if( bPaletteChange )
    {
        if( pTbl != NULL )
        {
            NNS_G2dSetRendererPaletteTbl( pCurrentInstance_, pTbl );
        }else{
            NNS_G2dResetRendererPaletteTbl( pCurrentInstance_ );
        }
    }
}




/*---------------------------------------------------------------------------*
  Name:         NNS_G2dPushMtx 

  Description:  レンダラ内部の行列スタックを操作します。
                行列スタックはCPUによって処理されます。
                
                2D 3D Graphics engine で scale パラメータの解釈が異なるため
                内部に 2D 用 3D 用 2つの行列スタックを持っています(NNS_G2dScale)。
                
                
  Arguments:    なし
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dPushMtx()
{
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT( pCurrentInstance_ );
    //
    // レンダラモジュールの場合は、パフォーマンス的に有利なのようなので、
    // グラフィックスエンジンの使用状況によらずCPU で 行列計算を行うようにした。
    //
    if( !(pCurrentInstance_->opzHint & NNS_G2D_RDR_OPZHINT_NOT_SR) )
    {
        const u16 lastPos = NNSi_G2dGetMtxStackPos();
        NNSi_G2dMtxPush();
        {
            const u16 newPos = NNSi_G2dGetMtxStackPos();
            //
            // カレント行列の状態を更新する    
            //
            NNSi_G2dMCMSetMtxStackPushed( newPos, lastPos );
        }
    }else{
        NNSi_G2dMtxPush();
    }    
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dPopMtx 

  Description:  レンダラ内部の 行列スタックをポップします
                
  Arguments:    なし
                 
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dPopMtx()
{ 
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT( pCurrentInstance_ );
    NNSi_G2dMtxPop();
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTranslate 

  Description:  レンダラ内部の カレント行列に 平行移動 行列を乗算します
                
  Arguments:    x:      [IN]  trans x
                y:      [IN]  trans y
                z:      [IN]  trans z
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dTranslate(fx32 x, fx32 y, fx32 z )
{
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT( pCurrentInstance_ );
    NNSi_G2dTranslate( x, y, z );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTranslate 

  Description:  レンダラ内部の カレント行列の 平行移動 成分を設定します。
                
  Arguments:    x:      [IN]  trans x
                y:      [IN]  trans y
                z:      [IN]  trans z
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetTrans(fx32 x, fx32 y, fx32 z )
{
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT( pCurrentInstance_ );
    NNSi_G2dSetTrans( x, y, z );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dScale 

  Description:  レンダラ内部の カレント行列に スケール 行列を乗算します
                
  Arguments:    x:      [IN]  scale x
                y:      [IN]  scale y
                z:      [IN]  scale z
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dScale(fx32 x, fx32 y, fx32 z )
{
#pragma unused( z )
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT( pCurrentInstance_ );
    NNSi_G2dScale( x, y );
    // 
    // affine transform を 含む場合、Mtxを HW に適用する必要がある( 2D Graphics Engine )
    // 現在 SR transform を 含まない 状態であれば 新たに 
    //      SR transform を 含む     状態に設定する
    //
    if( !NNSi_G2dIsRndCurrentMtxSRTransformed() )
    {
        NNS_G2D_WARNING( pCurrentInstance_->rendererCore.flipFlag == NNS_G2D_RENDERERFLIP_NONE, 
                    "You can't use affine transformation using flip function." );
        NNSi_G2dSetRndMtxStackSRTransformed();
    }
    
    NNSi_G2dMCMSetCurrentMtxSRChanged();
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dRotZ 

  Description:  レンダラ内部の カレント行列に 回転 行列を乗算します
                
  Arguments:    sin:      [IN]  sin 値
                cos:      [IN]  cos 値
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dRotZ( fx32 sin, fx32 cos )
{
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT( pCurrentInstance_ );
    NNSi_G2dRotate( sin, cos );
    
    // 
    // SR transform を 含む場合、Mtxを HW に適用する必要がある( 2D Graphics Engine )
    // 現在 SR transform を 含まない 状態であれば 新たに 
    //      SR transform を 含む     状態に設定する
    //
    if( !NNSi_G2dIsRndCurrentMtxSRTransformed() )
    {
        NNS_G2D_WARNING( pCurrentInstance_->rendererCore.flipFlag == NNS_G2D_RENDERERFLIP_NONE, 
                    "You can't use affine transformation using flip function." );
        NNSi_G2dSetRndMtxStackSRTransformed();
    }
    
    NNSi_G2dMCMSetCurrentMtxSRChanged();   
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetRendererFlipMode 

  Description:  レンダラのフリップ描画設定を設定します。
                注意：
                    フリップ描画が有効になっている場合はアフィン変換機能を利用できません。
                
                Begin - End rendering ブロックの内外どちらでも呼び出すことが可能です。
                
                
  Arguments:    pRend:       [OUT] レンダラ 実体
                bFlipH:      [IN]  Hフリップを利用するか？
                bFlipV:      [IN]  Vフリップを利用するか？
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetRendererFlipMode
( 
    NNSG2dRendererInstance* pRend, 
    BOOL bFlipH, 
    BOOL bFlipV 
)
{    
    NNS_G2D_WARNING( !NNSi_G2dIsRndCurrentMtxSRTransformed(), 
       "You can't use the flip function using affine transformation." );
    NNS_G2D_NULL_ASSERT( pRend );
    
    NNS_G2dSetRndCoreFlipMode( &pRend->rendererCore, bFlipH, bFlipV );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetRendererPaletteTbl 

  Description:  レンダラ 実体に  カラーパレット変更テーブル設定を設定します
  
                Begin - End rendering ブロックの内外どちらでも呼び出すことが可能です。
                
  Arguments:    pRend:      [OUT] レンダラ 実体
                pTbl:       [IN]  パレット変換テーブル
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetRendererPaletteTbl
( 
    NNSG2dRendererInstance*         pRend, 
    const NNSG2dPaletteSwapTable*   pTbl 
)
{
    NNS_G2D_NULL_ASSERT( pRend );
    NNS_G2D_NULL_ASSERT( pTbl );
    
    pRend->pPaletteSwapTbl = pTbl;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetRendererPaletteTbl TODO:

  Description:  レンダラ 実体 の カラーパレット変更テーブル設定を取得します
                
                Begin - End rendering ブロックの内外どちらでも呼び出すことが可能です。
                
  Arguments:    pRend:      [OUT] レンダラ 実体
                
  Returns:      カラーパレット変更テーブル
  
 *---------------------------------------------------------------------------*/
const NNSG2dPaletteSwapTable* 
NNS_G2dGetRendererPaletteTbl( NNSG2dRendererInstance* pRend )
{
    NNS_G2D_NULL_ASSERT( pRend );
    
    return pRend->pPaletteSwapTbl;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dResetRendererPaletteTbl 

  Description:  レンダラ 実体に の カラーパレット変更テーブル設定をリセットします
  
                Begin - End rendering ブロックの内外どちらでも呼び出すことが可能です。
                
  Arguments:    pRend:      [OUT] レンダラ 実体
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dResetRendererPaletteTbl( NNSG2dRendererInstance* pRend )
{
    NNS_G2D_NULL_ASSERT( pRend );
    pRend->pPaletteSwapTbl = NULL;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetRendererImageProxy 

  Description:  レンダラ 実体に 画像情報、パレット情報を設定します。
                レンダラ 実体を使用する前に実行してください。
                
                
                本関数は、Begin End Rendering()ブロック内でも使用が可能です。
                
                注意：
                最適化フラグ、NNS_G2D_RDR_OPZHINT_LOCK_PARAMS フラグを指定して
                レンダリングを行っている場合は、Begin End Rendering()ブロック内での
                使用が禁止されます。
                これは、レンダラコアモジュールのBegin-End Rendering()呼び出しが
                レンダラモジュールのBegin-End Rendering()と同じタイミングで呼び出される
                様になるからです。
                
                レンダラコアモジュールでは、
                Begin-End Rendering()内で画像プロクシ設定を切り替えることは禁止されています。
                ( ImageProxyに関連するパラメータの前計算をBegin Rendering()で行っているためです。）
                
                
                
                
                
  Arguments:    pRend:      [OUT] レンダラ 実体
                pImgProxy:  [IN]  レンダラ に設定する 画像情報
                pPltProxy:  [IN]  レンダラ に設定する パレット情報
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetRendererImageProxy
( 
    NNSG2dRendererInstance*             pRend, 
    const NNSG2dImageProxy*             pImgProxy, 
    const NNSG2dImagePaletteProxy*      pPltProxy
)
{
    NNS_G2D_NULL_ASSERT( pRend );
    NNS_G2D_NULL_ASSERT( pImgProxy );
    NNS_G2D_NULL_ASSERT( pPltProxy );
    
    SDK_WARNING( pImgProxy->attr.bExtendedPlt == pPltProxy->bExtendedPlt, 
        "Palette type mismatching was detected.\n Make sure that you use the correct palette." );
    
    NNS_G2D_WARNING( !(pRend->opzHint & NNS_G2D_RDR_OPZHINT_LOCK_PARAMS),
        "Avoid calling this function, when you specified the optimize flag NNS_G2D_RDR_OPZHINT_LOCK_PARAMS." );
    
    NNS_G2dSetRndCoreImageProxy( &pRend->rendererCore, pImgProxy, pPltProxy );
}

