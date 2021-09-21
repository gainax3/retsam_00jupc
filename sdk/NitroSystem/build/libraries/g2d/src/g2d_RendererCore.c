/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d
  File:     g2d_RendererCore.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.34 $
 *---------------------------------------------------------------------------*/
#include <nnsys/g2d/g2d_RendererCore.h>
#include "g2di_RendererMtxCache.h"      // For MatrixCache & State


#include <nnsys/g2d/fmt/g2d_Cell_data.h>
#include <nnsys/g2d/g2d_OamSoftwareSpriteDraw.h>
#include <nnsys/g2d/g2d_CellTransferManager.h>




#define NNSi_G2D_RNDCORE_DUMMY_FLAG                FALSE
#define NNSi_G2D_RNDCORE_OAMENTORY_SUCCESSFUL      TRUE



static NNSG2dRndCoreInstance*     pTheInstance_ = NULL;
static const MtxFx32              mtxIdentity_ = { FX32_ONE,        0, 
                                                          0, FX32_ONE, 
                                                          0,        0 };
// 関数が Begin - End Rendering ブロック の中で呼ばれているか 
#define NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT(  ) \
       NNS_G2D_ASSERTMSG( pTheInstance_ != NULL, "Please call this method between Begin - End Rendering" ) 

// 関数が Begin - End Rendering ブロック の外で呼ばれているか 
#define NNS_G2D_RND_OUTSIDE_BEGINEND_ASSERT(  ) \
       NNS_G2D_ASSERTMSG( pTheInstance_ == NULL, "Please call this method outside Begin - End Rendering" ) 



//------------------------------------------------------------------------------
// MtxChache affine Index アクセサ
// 
//    NNS_G2D_SURFACETYPE_MAIN3D = 0x00,  // 3D グラフィックスエンジン   を使用
//    NNS_G2D_SURFACETYPE_MAIN2D = 0x01,  // 2D グラフィックスエンジン A を使用
//    NNS_G2D_SURFACETYPE_SUB2D  = 0x02,  // 2D グラフィックスエンジン B を使用
//    NNS_G2D_SURFACETYPE_MAX    = 0x03
//
// 注意：Enum 値 と密接に 依存している処理です 
//
static NNS_G2D_INLINE u16 GetMtxCacheAffineIndex_
( 
    const NNSG2dRndCore2DMtxCache*     pMtxCache,
    NNSG2dSurfaceType                  surfaceType,
    OAM_FLIP                           flipType
)
{
    return pMtxCache->affineIndex[flipType][surfaceType - NNS_G2D_SURFACETYPE_MAIN2D];
}

//------------------------------------------------------------------------------
// 
//
static NNS_G2D_INLINE void SetMtxCacheAffineIndex_
( 
    NNSG2dRndCore2DMtxCache*    pMtxCache,
    NNSG2dSurfaceType           surfaceType,
    OAM_FLIP                    flipType,
    u16                         affineIndex
)
{
    pMtxCache->affineIndex[flipType][surfaceType - NNS_G2D_SURFACETYPE_MAIN2D] = affineIndex;
}

//------------------------------------------------------------------------------
// 登録前に以前に登録されているかチェックします。このことによって重複登録を避けています。
// 
static NNS_G2D_INLINE BOOL IsMtxCacheRegisteredAsAffineParams_
(
    const u16 affineIdx
)
{
    return ( affineIdx != MtxCache_NOT_AVAILABLE ) ? TRUE : FALSE;
}



//------------------------------------------------------------------------------
static NNS_G2D_INLINE const MtxFx32* RndCoreGetCurrentMtx_()
{
    if( pTheInstance_->pCurrentMxt )
    { 
        //if( pTheInstance_->flipFlag != NNS_G2D_RENDERERFLIP_NONE )
        // TODO:警告を表示する
        // 設定された行列
        return pTheInstance_->pCurrentMxt;
    }else{
        //
        // 単位行列
        //    
        return &mtxIdentity_;
    }
}

//------------------------------------------------------------------------------
// 現在レンダラはアフィン変換(2D)が適用されている状態か？
static NNS_G2D_INLINE BOOL IsRndCore2DAffineTransformed_
(
    const NNSG2dRndCoreInstance* pRnd 
)
{
    return (BOOL)( pRnd->flipFlag == NNS_G2D_RENDERERFLIP_NONE && 
                   pRnd->pCurrentMtxCacheFor2D != NULL );
}

//------------------------------------------------------------------------------
// マッピングタイプなどから
// バイトサイズをキャラクタ数に変換するシフト値を求めます
static NNS_G2D_INLINE int GetShiftToConvByteTo2DChar_( GXOBJVRamModeChar mappingType )
{
    int shift = ( REG_GX_DISPCNT_EXOBJ_MASK & mappingType ) >> REG_GX_DISPCNT_EXOBJ_SHIFT;
    
    return shift;
}

//------------------------------------------------------------------------------
// シフトする値がシフト幅の境界上か判定します
static NNS_G2D_INLINE BOOL IsAlignedShiftValueBoundary_( u32 sizeByte, int shiftBit )
{
    const int mask = (0x1 << shiftBit) - 1;
    return (BOOL)( (mask & sizeByte) == 0);
}

//------------------------------------------------------------------------------
// バイト数、マッピングタイプなどから
// キャラクタ数を算出します
static NNS_G2D_INLINE u32 GetNum2DCharacter_( u32 sizeByte, GXOBJVRamModeChar mappingType )
{
   
    const int shiftBit  = 5 + GetShiftToConvByteTo2DChar_( mappingType ); // 5 means  /= 32 
    u32       numChar   = sizeByte >> shiftBit;
    
    NNS_G2D_ASSERT( IsAlignedShiftValueBoundary_( sizeByte, shiftBit ) ); 
    
    return numChar;
}

//------------------------------------------------------------------------------
// 3D Graphics Engine のテクスチャ設定を取得します
// 
static NNS_G2D_INLINE u32 GetTexBaseAddr3D_( const NNSG2dImageProxy* pImgProxy )
{
    NNS_G2D_NULL_ASSERT( pImgProxy );
    if( NNS_G2dIsImageReadyToUse( pImgProxy, NNS_G2D_VRAM_TYPE_3DMAIN ) )
    {
        return NNS_G2dGetImageLocation( pImgProxy, NNS_G2D_VRAM_TYPE_3DMAIN );
    }else{
        // TODO:警告
        return 0;
    }
}

//------------------------------------------------------------------------------
// 3D Graphics Engine のパレット設定を行います
//
static NNS_G2D_INLINE u32 GetPltBaseAddr3D_( const NNSG2dImagePaletteProxy* pPltProxy )
{
    NNS_G2D_NULL_ASSERT( pPltProxy );
    if( NNS_G2dIsImagePaletteReadyToUse( pPltProxy, NNS_G2D_VRAM_TYPE_3DMAIN ) )
    {
        return NNS_G2dGetImagePaletteLocation( pPltProxy, NNS_G2D_VRAM_TYPE_3DMAIN );
    }else{
        // TODO:警告
        return 0;
    }
}


//------------------------------------------------------------------------------
// 現在はただキャストするだけです
static NNS_G2D_INLINE NNS_G2D_VRAM_TYPE SurfaceTypeToVramType_( NNSG2dSurfaceType   surfaceType )
{
    return (NNS_G2D_VRAM_TYPE)surfaceType;
}

//------------------------------------------------------------------------------
// 2D Graphics Engine のキャラクタオフセットを取得します
// 
static NNS_G2D_INLINE u32 GetCharacterBase2D_
( 
    const NNSG2dImageProxy* pImgProxy, 
    NNSG2dSurfaceType       type 
)
{
    
    NNS_G2D_NULL_ASSERT( pImgProxy );
    NNS_G2D_ASSERT( type == NNS_G2D_SURFACETYPE_MAIN2D || 
                    type == NNS_G2D_SURFACETYPE_SUB2D );

    
    {   
        const NNS_G2D_VRAM_TYPE       vramType = SurfaceTypeToVramType_( type );
   
        if( NNS_G2dIsImageReadyToUse( pImgProxy, vramType ) )
        {
            u32 baseAddr = NNS_G2dGetImageLocation( pImgProxy, vramType );
            
            return GetNum2DCharacter_( baseAddr, pImgProxy->attr.mappingType );     
            
        }else{
            // TODO:警告
            return 0;
        }
    }
}



//------------------------------------------------------------------------------
static NNS_G2D_INLINE BOOL IsOamAttrDoubleAffineMode_( const GXOamAttr* pAttr )
{
    return (BOOL)(pAttr->rsMode == 0x3);
}

//------------------------------------------------------------------------------
static NNS_G2D_INLINE void HandleCellCallBackFunc_
( 
    NNSG2dRndCoreDrawCellCallBack   pFunc,
    const NNSG2dCellData*           pCell 
) 
{
    //
    // コールバック関数呼び出しを行う
    //
    // 引数に行列はとらないこととした theInstance_ を 渡しているのだし…
    if( pFunc )
    {
        (*pFunc)( pTheInstance_, pCell ); 
    } 
}

//------------------------------------------------------------------------------
static NNS_G2D_INLINE void HandleCellOamBackFunc_
( 
    NNSG2dRndCoreDrawOamCallBack    pFunc,
    const NNSG2dCellData*           pCell,
    u16                             oamIdx 
) 
{
    //
    // コールバック関数呼び出しを行う
    //
    // 引数に行列はとらないこととした theInstance_ を 渡しているのだし…
    // 
    if( pFunc )
    {
        (*pFunc)( pTheInstance_, pCell, oamIdx ); 
    } 
}

//------------------------------------------------------------------------------
// フリップ行列を作成します
// ロードの直前に作成される様に動作が変更になりました。
static NNS_G2D_INLINE void MakeFlipMtx_
( 
    const MtxFx22*    pMtxSrc, 
    MtxFx22*          pMtxDst, 
    OAM_FLIP          type 
)
{
    NNS_G2D_NULL_ASSERT( pMtxSrc );
    NNS_G2D_NULL_ASSERT( pMtxDst );
    
    {
        *pMtxDst = *pMtxSrc;
        
        if( type & OAM_FLIP_H )
        {
            pMtxDst->_00 = -pMtxDst->_00;
            pMtxDst->_01 = -pMtxDst->_01;
        }
        
        if( type & OAM_FLIP_V )
        {
            pMtxDst->_10 = -pMtxDst->_10;
            pMtxDst->_11 = -pMtxDst->_11;
        }
    }

}

//------------------------------------------------------------------------------
static NNS_G2D_INLINE u16 LoadMtxCacheAsAffineParams_
( 
    NNSG2dRndCore2DMtxCache*               pMtxCache,
    const NNSG2dRndCoreSurface*            pSurface, 
    OAM_FLIP                               type  
)
{
    u16    affineIdx  = GetMtxCacheAffineIndex_( pMtxCache, pSurface->type, type );
    
    //
    // もし以前に Affine パラメータとして下位モジュールに登録されていない行列ならば...
    //
    if( !IsMtxCacheRegisteredAsAffineParams_( affineIdx ) )
    {
        NNS_G2D_NULL_ASSERT( pTheInstance_->pFuncOamAffineRegister );        
        
        //
        // 登録を行い、その結果をキャッシュに格納する 
        //
        {
            if( type == OAM_FLIP_NONE )
            {
                affineIdx = (*pTheInstance_->pFuncOamAffineRegister)( &pMtxCache->m22 );
            }else{
                MtxFx22     mtxTemp;
                MakeFlipMtx_( &pMtxCache->m22, &mtxTemp, type );
                affineIdx = (*pTheInstance_->pFuncOamAffineRegister)( &mtxTemp );
            }
                
            SetMtxCacheAffineIndex_( pMtxCache, pSurface->type, type, affineIdx );
        }
    }
    
    return affineIdx;
}


//------------------------------------------------------------------------------
static NNS_G2D_INLINE NNS_G2D_VRAM_TYPE 
ConvertSurfaceTypeToVramType_( NNSG2dSurfaceType surfaceType )
{
    return (NNS_G2D_VRAM_TYPE)(surfaceType);
}

//------------------------------------------------------------------------------
static NNS_G2D_INLINE u16 GetFlipedOBJPosX_( GXOamAttr* pOam, const GXOamShape shape )
{
    return NNS_G2dIsRndCoreFlipH( pTheInstance_ ) ? 
           (u16)( -NNS_G2dRepeatXinCellSpace(  (s16)pOam->x ) - NNS_G2dGetOamSizeX( &shape ) ):
           (u16)NNS_G2dRepeatXinCellSpace( (s16)pOam->x );
}
//------------------------------------------------------------------------------
static NNS_G2D_INLINE u16 GetFlipedOBJPosY_( GXOamAttr* pOam, const GXOamShape shape )
{
    return NNS_G2dIsRndCoreFlipV( pTheInstance_ ) ? 
           (u16)( -NNS_G2dRepeatYinCellSpace( (s16)pOam->y ) - NNS_G2dGetOamSizeY( &shape ) ) :
           (u16)NNS_G2dRepeatYinCellSpace( (s16)pOam->y );
}

//------------------------------------------------------------------------------
static NNS_G2D_INLINE void OverwriteOamFlipFlag_( GXOamAttr* pOam )
{
    NNS_G2D_NULL_ASSERT( pOam );
    {
        //
        // フリップ情報を操作する
        //        
        const BOOL bFlipH = 
           NNS_G2dIsRndCoreFlipH( pTheInstance_ ) ^ NNSi_G2dGetOamFlipH( pOam );
        const BOOL bFlipV = 
           NNS_G2dIsRndCoreFlipV( pTheInstance_ ) ^ NNSi_G2dGetOamFlipV( pOam );
           
        pOam->attr01 &= ~GX_OAM_ATTR01_HF_MASK;
        pOam->attr01 |= bFlipH << GX_OAM_ATTR01_HF_SHIFT;
           
        pOam->attr01 &= ~GX_OAM_ATTR01_VF_MASK;
        pOam->attr01 |= bFlipV << GX_OAM_ATTR01_VF_SHIFT;
    }
}


//------------------------------------------------------------------------------
static void DoAffineTransforme_
( 
    const MtxFx22*    pMtxSR, 
    GXOamAttr*        pOam, 
    NNSG2dFVec2*      pBaseTrans
)
{
    NNSG2dFVec2          objTrans;
    GXOamEffect          effectTypeAfter;
    
    // Get oam's position data in Obj Loacal coordinate, and convert it to world coordinate.
    NNS_G2dGetOamTransFx32( pOam, &objTrans );
    
    //
    // 倍角アフィンが設定されているOBJの場合、
    // 通常のOBJ表示位置に対して、位置補正値を付加しているので、
    // 補正値を一旦、取り去ります
    //
    NNSi_G2dRemovePositionAdjustmentFromDoubleAffineOBJ( pOam, &objTrans );               
                        
    // アフィン変換を行います
    MulMtx22( pMtxSR, &objTrans, &objTrans );
    
    //
    // アフィン変換されているなら、アフィンモード指定(倍角 or 普通)
    // （注意：従来のバージョンでは下位モジュールで行っていた処理です）
    // 
    // アフィンモードのみを設定したいのでアフィンパラメータ番号には適当な値を設定しています。
    // 適当なAPIがGXに用意されていないためです。
    // いずれ対策をする予定です。
    //
    if( pTheInstance_->affineOverwriteMode != NNS_G2D_RND_AFFINE_OVERWRITE_NONE )
    {
        if( pTheInstance_->affineOverwriteMode == NNS_G2D_RND_AFFINE_OVERWRITE_DOUBLE  ) 
        {
           effectTypeAfter =  GX_OAM_EFFECT_AFFINE_DOUBLE;
        }else{
           effectTypeAfter =  GX_OAM_EFFECT_AFFINE;
        }
                            
        G2_SetOBJEffect( pOam, effectTypeAfter, 0 );
    }
    
    //
    // 倍角アフィンモードと通常アフィン変換モードでは表示位置が異なるため
    // 倍角アフィンモード変換が指定されているOBJの場合は
    // 本関数で補正を行います。
    // 
    {
        // 倍角アフィンなら倍角アフィンOBJのための位置補正をおこなう
        const BOOL bShouldAdjust = G2_GetOBJEffect( pOam ) == GX_OAM_EFFECT_AFFINE_DOUBLE;
                                 
        NNSi_G2dAdjustDifferenceOfRotateOrientation( pOam, 
                                                     pMtxSR, 
                                                     &objTrans, 
                                                     bShouldAdjust );
    }
    
    // add base offset( left top position of Obj )
    objTrans.x += pBaseTrans->x;
    objTrans.y += pBaseTrans->y;
    G2_SetOBJPosition( pOam, objTrans.x >> FX32_SHIFT, objTrans.y >> FX32_SHIFT  );
}

//------------------------------------------------------------------------------                                                             
static NNS_G2D_INLINE void DoFlipTransforme_( GXOamAttr* pOam, NNSG2dFVec2* pBaseTrans )
{    
    //
    // フリップ処理
    //
    if( pTheInstance_->flipFlag != NNS_G2D_RENDERERFLIP_NONE )
    {    
        
        
        const GXOamShape shape = NNS_G2dGetOAMSize( pOam ); 
                                
        OverwriteOamFlipFlag_( pOam );
        
        if( NNS_G2dIsRndCoreFlipH( pTheInstance_ ) )
        {
            pOam->x = -pOam->x - NNS_G2dGetOamSizeX( &shape );
        }
        
        if( NNS_G2dIsRndCoreFlipV( pTheInstance_ ) )
        {
            pOam->y = (u8)(-pOam->y - NNS_G2dGetOamSizeY( &shape ));
        }
    }
    
    // add base offset( left top position of Obj )
    pOam->x += pBaseTrans->x >> FX32_SHIFT;
    pOam->y += pBaseTrans->y >> FX32_SHIFT;
}


//------------------------------------------------------------------------------
// レンダラが、描画開始の準備が整っているかを調査します。
// デバックビルド時にアサート文の中でのみ使用されます。
//
static BOOL IsRndCoreReadyForBeginRendering_( NNSG2dRndCoreInstance* pRnd )
{
    NNS_G2D_NULL_ASSERT( pRnd );
    {
       //
       // 画像プロクシが設定されているか？
       //
       if( !(pRnd->pImgProxy && pRnd->pPltProxy) )
       {
           OS_Warning("RendererCore:ImageProxy isn't ready.");
           return FALSE;
       }
       
       //
       // 描画対象サーフェスが設定されているか？
       //
       if( pRnd->pCurrentTargetSurface == NULL )
       {
           OS_Warning("RendererCore:TragetSurface isn't ready.");
           return FALSE;
       }else{
           // サーフェスはアクティブか？
           if( !pRnd->pCurrentTargetSurface->bActive )
           {
               OS_Warning("RendererCore:The current tragetSurface isn't active. Is it OK?");
               // 警告のみ、FALSE とはしない。
           }
       }
       
       //
       // 2D 描画の場合OAM登録関数が正しく設定されているか?       
       //
       if( pRnd->pCurrentTargetSurface->type == NNS_G2D_SURFACETYPE_MAIN2D ||
           pRnd->pCurrentTargetSurface->type == NNS_G2D_SURFACETYPE_SUB2D  )
       {
           if
           ( 
              ( pRnd->pFuncOamAffineRegister == NULL && 
                  pRnd->pFuncOamRegister == NULL      )            
           )
           {
               OS_Warning( "RendererCore:OAM-RegisterFunction For 2D Graphics Engine rendering isn't ready." );
               return FALSE;
           }
       }
    }
    // 検査合格
    return TRUE;
}

//------------------------------------------------------------------------------
// Cell を 2D Graphics Engine を使用して 描画します
//
// 2D Graphics Engine レンタリング専用の処理です。
//
static void DrawCellToSurface2D_
( 
    const NNSG2dRndCoreSurface*    pSurface, 
    const NNSG2dCellData*          pCell
)
{
    NNSG2dFVec2          baseTrans;
    MtxFx22              mtxSR;
    const MtxFx32*       pCurrMtx = RndCoreGetCurrentMtx_();
    
    NNS_G2D_NULL_ASSERT( pSurface );
    NNS_G2D_NULL_ASSERT( pCell );
    
    
    // get current Mtx for affine transformation.
    NNSi_G2dGetMtxTrans( pCurrMtx, &baseTrans );  
    //
    // ビューローカル系への変換
    //                  
    baseTrans.x -= pSurface->viewRect.posTopLeft.x;
    baseTrans.y -= pSurface->viewRect.posTopLeft.y;
    
    {
        const u32   baseCharOffset  = pTheInstance_->base2DCharOffset;
        const BOOL  bAffined        = IsRndCore2DAffineTransformed_( pTheInstance_ );
        
        u16          i;
        GXOamAttr*   pTempOam = &pTheInstance_->currentOam;
        u16          oamAffinIdx = NNS_G2D_OAM_AFFINE_IDX_NONE;
                        
        for( i = 0; i < pCell->numOAMAttrs; i++ )
        {
            //
            // Oamアトリビュート描画前コールバック
            //
            pTheInstance_->bDrawEnable = TRUE;
            NNS_G2dCopyCellAsOamAttr( pCell, i, pTempOam );
            
            HandleCellOamBackFunc_( pSurface->pBeforeDrawOamBackFunc, pCell, i );
            
            //
            // 描画スキップ判定
            //
            if( !pTheInstance_->bDrawEnable )
            {
                continue;
            }
            
            
            
            //
            // キャラクタ番号オフセットの加算
            //
            pTempOam->charNo += baseCharOffset;
                
            
            //
            // 座標変換:フリップ処理かアフィン変換処理
            //
            if( bAffined )
            {       
                //
                // カレント行列スタックがロードの必要があるか判断し、
                // ロードしてインデックスを返す関数
                // OBJのフリップフラグをチェックし、適切なアフィン変換行列を作成するので
                // OBJ毎のループ内で毎回実行する必要があります。
                //
                oamAffinIdx = 
                LoadMtxCacheAsAffineParams_( pTheInstance_->pCurrentMtxCacheFor2D, 
                                             pSurface, 
                                             TO_OAM_FLIP( NNSi_G2dGetOamFlipH( pTempOam ), 
                                                          NNSi_G2dGetOamFlipV( pTempOam ) ));
                NNSi_G2dGetMtxRS( pCurrMtx, &mtxSR );
                DoAffineTransforme_( &mtxSR, pTempOam, &baseTrans );
            }else{
                oamAffinIdx = NNS_G2D_OAM_AFFINE_IDX_NONE;
                DoFlipTransforme_( pTempOam, &baseTrans );
            }

            //
            // 描画登録関数よびだし
            // 第三引数は使用されていないため、ダミー引数を与えています。
            //
            NNS_G2D_NULL_ASSERT( pTheInstance_->pFuncOamRegister );
            
            if( NNSi_G2D_RNDCORE_OAMENTORY_SUCCESSFUL 
                != (*pTheInstance_->pFuncOamRegister)( pTempOam, 
                                                      oamAffinIdx, 
                                                      NNSi_G2D_RNDCORE_DUMMY_FLAG ) )
            {
                // we have no capacity for new oam datas.
                return;
            }
             
            //
            // Oamアトリビュート描画後コールバック
            //
            HandleCellOamBackFunc_( pSurface->pAfterDrawOamBackFunc, pCell, i );
        }
    }
}

//------------------------------------------------------------------------------
static void DrawOamToSurface3D_
( 
    GXOamAttr*                     pOam
)
{
    // 行列設定
    G3_LoadMtx43( &pTheInstance_->mtxFor3DGE );
        
    if( pTheInstance_->flipFlag != NNS_G2D_RENDERERFLIP_NONE )
    {   
        const GXOamShape shape = NNS_G2dGetOAMSize( pOam );
    
        //
        // OBJ の 描画位置を取得する
        //
    	const s16 posX = (s16)GetFlipedOBJPosX_( pOam, shape );
        const s16 posY = (s16)GetFlipedOBJPosY_( pOam, shape );
        const s16 posZ = -1;      
        
        //
        // フリップ情報を操作する
        //        
        OverwriteOamFlipFlag_( pOam );
        
        
        NNS_G2dDrawOneOam3DDirectWithPosFast( posX, posY, posZ, 
                                      pOam, 
                                      &pTheInstance_->pImgProxy->attr, 
                                       pTheInstance_->baseTexAddr3D, 
                                       pTheInstance_->basePltAddr3D );        
    }else{
        const s16 posX = (s16)NNS_G2dRepeatXinCellSpace( (s16)pOam->x );
        const s16 posY = (s16)NNS_G2dRepeatYinCellSpace( (s16)pOam->y );
        const s16 posZ = -1;
        
        NNS_G2dDrawOneOam3DDirectWithPosFast( posX, posY, posZ, 
                                      pOam, 
                                      &pTheInstance_->pImgProxy->attr, 
                                       pTheInstance_->baseTexAddr3D, 
                                       pTheInstance_->basePltAddr3D );    
    }
    
}
//------------------------------------------------------------------------------
// Cell を 3D Graphics Engine を使用して 描画します
// 
static void DrawCellToSurface3D_
( 
    const NNSG2dRndCoreSurface*    pSurface, 
    const NNSG2dCellData*          pCell
)
{
    u16 i = 0;
    
    GXOamAttr*            pTempOam = &pTheInstance_->currentOam;
    
    
    NNS_G2D_NULL_ASSERT( pSurface );
    NNS_G2D_NULL_ASSERT( pCell );
    
    
    // Draw All Objects
    for( i = 0; i < pCell->numOAMAttrs; i++ )
    {
        //
        // Oamアトリビュート描画前コールバック
        //
        pTheInstance_->bDrawEnable = TRUE;
        NNS_G2dCopyCellAsOamAttr( pCell, i, pTempOam );
        
        HandleCellOamBackFunc_( pSurface->pBeforeDrawOamBackFunc, pCell, i );
        if( pTheInstance_->bDrawEnable )            
        {            
            // 描画
            DrawOamToSurface3D_( pTempOam );
        }
        //
        // Oamアトリビュート描画前コールバック
        //
        HandleCellOamBackFunc_( pSurface->pAfterDrawOamBackFunc, pCell, i );
    }
}



//------------------------------------------------------------------------------
static NNS_G2D_INLINE void DrawCellImpl_
( 
    const NNSG2dCellData*   pCell, 
    u32                     cellVramTransferHandle 
)
{
    const NNSG2dRndCoreSurface*   pSurface = pTheInstance_->pCurrentTargetSurface;
    
    //
    // アクティブでなければ描画処理は何もしない
    //
    if( !pSurface->bActive )
    {
        return;
    }
    
    // カリング、マトリクスロードもコールバックに含まれる
    pTheInstance_->bDrawEnable = TRUE;
    HandleCellCallBackFunc_( pSurface->pBeforeDrawCellBackFunc, pCell );
    if( pTheInstance_->bDrawEnable )
    {        
        //
        // VRAM 転送セルならば、セルの描画が行われたことをVRAM転送状態オブジェクトに記録する
        //
        if( cellVramTransferHandle != NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE )
        {
            NNSi_G2dSetVramTransferCellDrawnFlag
            ( 
                cellVramTransferHandle, 
                ConvertSurfaceTypeToVramType_( pSurface->type ),
                TRUE 
            );
        }
        
        //
        // サーフェスの種類ごとに...
        //
        switch( pSurface->type )
        {
        case NNS_G2D_SURFACETYPE_MAIN3D:
            DrawCellToSurface3D_( pSurface, pCell ); 
            break;
        case NNS_G2D_SURFACETYPE_MAIN2D:
        case NNS_G2D_SURFACETYPE_SUB2D:
            DrawCellToSurface2D_( pSurface, pCell );
            break;
        case NNS_G2D_SURFACETYPE_MAX:
        default:
            NNS_G2D_ASSERT(FALSE);
        }
    }
    HandleCellCallBackFunc_( pSurface->pAfterDrawCellBackFunc, pCell );
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitRndCore

  Description:  レンダラコアモジュールを初期化します。
                NNSG2dRndCoreInstanceを利用する前に必ず実行するようにしてください。
                
                
  Arguments:    pRnd          [OUT] レンダラコア
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dInitRndCore( NNSG2dRndCoreInstance* pRnd )
{
    MI_CpuFill16( pRnd, 0x0, sizeof( NNSG2dRndCoreInstance ) );
    
    pRnd->pCurrentTargetSurface = NULL;
    
    pRnd->affineOverwriteMode  = NNS_G2D_RND_AFFINE_OVERWRITE_DOUBLE;
    
    pRnd->pImgProxy = NULL;
    pRnd->pPltProxy = NULL;
    
    pRnd->flipFlag = NNS_G2D_RENDERERFLIP_NONE;
    
    pRnd->bDrawEnable = TRUE;
    
    pRnd->pCurrentMtxCacheFor2D = NULL;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitRndCoreSurface

  Description:  レンダラコア描画対象サーフェスを初期化します。
                NNSG2dRndCoreSurfaceを利用する前に必ず実行するようにしてください。
                
                
  Arguments:    pSurface          [OUT] レンダラコア描画対象サーフェス
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dInitRndCoreSurface( NNSG2dRndCoreSurface* pSurface )
{
    MI_CpuFill16( pSurface, 0x0, sizeof( NNSG2dRndCoreSurface ) );
    
    pSurface->bActive   = TRUE;
    pSurface->type      = NNS_G2D_SURFACETYPE_MAX;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetRndCoreImageProxy

  Description:  レンダラコアモジュールが描画の際に参照する画像プロクシを設定します。
                レンダラモジュールとは異なり、Begin-EndRenderingブロック内で
                画像プロクシを変更することは不正です。
                これは、レンダラコアモジュールが
                BeginRenderingのタイミングでパラメータ計算を行うためです。
                
                
                本関数は、Begin-EndRenderingブロックの外で呼び出す必要があります。
                                
  Arguments:    pRnd                      [OUT] レンダラコア
                pImgProxy                 [IN]  画像プロクシ
                pPltProxy                 [IN]  画像パレットプロクシ
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetRndCoreImageProxy
( 
    NNSG2dRndCoreInstance*              pRnd,
    const NNSG2dImageProxy*             pImgProxy, 
    const NNSG2dImagePaletteProxy*      pPltProxy
)
{
    NNS_G2D_RND_OUTSIDE_BEGINEND_ASSERT();
    NNS_G2D_NULL_ASSERT( pRnd );
    NNS_G2D_NULL_ASSERT( pImgProxy );
    NNS_G2D_NULL_ASSERT( pPltProxy );
    
    pRnd->pImgProxy = pImgProxy;
    pRnd->pPltProxy = pPltProxy;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetRndCoreOamRegisterFunc

  Description:  レンダラコアモジュールが描画OBJを表すOAMを外部モジュールに登録する際に使用する
                関数ポインタ群を設定します。
                
                OAM登録関数は、設定されているサーフェスが、
                2D グラフィックスエンジン描画サーフェスである場合、
                必ず設定されてある必要があります。
                
                本関数は別に、OAM登録関数としてブロック転送登録関数を設定する
                NNS_G2dSetRndCoreOamRegisterFuncEx()も用意されています。
                
                注意：本関数を呼び出すと、ブロック転送登録関数がリセットされます。
                
                
                本関数は、Begin-EndRenderingブロックの外で呼び出す必要があります。
                                
  Arguments:    pRnd                             [OUT] レンダラコア
                pFuncOamRegister                 [IN]  OAM登録関数
                pFuncOamAffineRegister           [IN]  アフィンパラメータ登録関数
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetRndCoreOamRegisterFunc
(
    NNSG2dRndCoreInstance*          pRnd, 
    NNSG2dOamRegisterFunction       pFuncOamRegister,
    NNSG2dAffineRegisterFunction    pFuncOamAffineRegister
)
{
    NNS_G2D_RND_OUTSIDE_BEGINEND_ASSERT();
    NNS_G2D_NULL_ASSERT( pRnd );
    NNS_G2D_NULL_ASSERT( pFuncOamRegister );
    NNS_G2D_NULL_ASSERT( pFuncOamAffineRegister );
    
    pRnd->pFuncOamRegister       = pFuncOamRegister;
    pRnd->pFuncOamAffineRegister = pFuncOamAffineRegister;
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetRndCoreAffineOverwriteMode

  Description:  アフィン変換モードの上書き動作を設定します。
                
                上書き動作の種類を表す列挙子の定義は以下のとおりです。
                
                typedef enum NNSG2dRendererAffineTypeOverwiteMode
                {
                    NNS_G2D_RND_AFFINE_OVERWRITE_NONE,  // 上書きしない
                    NNS_G2D_RND_AFFINE_OVERWRITE_NORMAL,// 通常のアフィン変換方式に設定
                    NNS_G2D_RND_AFFINE_OVERWRITE_DOUBLE // 倍角アフィン変換方式に設定
                  
                }NNSG2dRendererAffineTypeOverwiteMode;
                
                
                本関数は、Begin-EndRenderingブロックの内外で呼び出しが可能です。
                
                
  Arguments:    pCurrentMxt         [IN]  アフィン変換行列
                mode                [IN]  アフィン変換モードの上書き動作をあらわす列挙子
                                         （NNSG2dRendererAffineTypeOverwiteMode）
                                    
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetRndCoreAffineOverwriteMode
( 
    NNSG2dRndCoreInstance*                  pRnd, 
    NNSG2dRendererAffineTypeOverwiteMode    mode 
)
{
    NNS_G2D_NULL_ASSERT( pRnd );
    
    pRnd->affineOverwriteMode = mode;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetRndCoreCurrentMtx3D

  Description:  アフィン変換行列を設定します。
                アフィン変換行列は、
                 ・3D グラフィックスエンジン描画時にジオメトリエンジンに設定されるカレント行列
                に使用されます。
                
                設定した変換行列はBegin-EndRenderingブロック内で有効です。
                NNS_G2dRndCoreEndRendering()呼び出し時に設定がリセットされますので、
                新たに描画処理を行う際には、再度本関数を呼び出して設定を行う必要があります。
                
                
                内部で、ジオメトリエンジンのカレント行列に設定する4x3 行列を計算します。
                本関数は、3D グラフィックスエンジン描画時のみに使用されます。
                2D グラフィックスエンジン描画時に本関数を呼び出した場合
                警告がメッセージが出力されます。
                
                本関数は、Begin-EndRenderingブロックの内で呼び出す必要があります。
                
                
  Arguments:    pCurrentMxt         [IN]  カレント変換行列
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetRndCoreCurrentMtx3D
( 
    const MtxFx32*              pCurrentMxt
)
{
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT();
    NNS_G2D_NULL_ASSERT( pCurrentMxt );
    NNS_G2D_WARNING( pTheInstance_->pCurrentTargetSurface->type == NNS_G2D_SURFACETYPE_MAIN3D,
       "This method works only for 3D graphics engine rendering." );
       
    {
       NNSG2dRndCoreInstance*      pRnd = pTheInstance_;
         
       pRnd->pCurrentMxt            = pCurrentMxt;
        
       //
       // 3D Graphics Engine で描画する場合、ジオメトリエンジンのカレント行列に設定する
       // 4x3 行列を計算しておく。
       // 2D Graphics Engine で描画する場合は、
       // pRnd->pCurrentMxtは、セルのアフィン変換に使用するだけなので 3x2 行列のままで十分である。
       //
       {
           //
           // ジオメトリエンジンにロードする4x3行列の生成
           //
           {
              pRnd->mtxFor3DGE._00 = pCurrentMxt->_00;
              pRnd->mtxFor3DGE._01 = pCurrentMxt->_01;
              pRnd->mtxFor3DGE._02 = 0;

              pRnd->mtxFor3DGE._10 = pCurrentMxt->_10;
              pRnd->mtxFor3DGE._11 = pCurrentMxt->_11;
              pRnd->mtxFor3DGE._12 = 0;

              pRnd->mtxFor3DGE._20 = 0;
              pRnd->mtxFor3DGE._21 = 0;
              pRnd->mtxFor3DGE._22 = FX32_ONE;

              pRnd->mtxFor3DGE._30 = pCurrentMxt->_20;
              pRnd->mtxFor3DGE._31 = pCurrentMxt->_21;
              pRnd->mtxFor3DGE._32 = pRnd->zFor3DSoftwareSprite;
           }
       }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetRndCoreCurrentMtx2D

  Description:  2D グラフィックスエンジン描画の際のOBJに適用するアフィン変換行列と
                アフィン変換パラメータを設定します。
                アフィン変換行列は、
                 ・2D グラフィックスエンジン描画時にCPU上で行われるOBJ位置の座標変換
                に使用されます。
                
                アフィンパラメータは行列キャッシュを使用して設定します。
                行列キャッシュへのポインタとしてNULLが指定された場合はアフィン変換を行わないものとして
                判断します。
                
                設定したアフィンパラメータはBegin-EndRenderingブロック内で有効です。
                NNS_G2dRndCoreEndRendering()呼び出し時に設定がリセットされますので、
                再度本関数を呼び出して設定を行う必要があります。
                
                
                本関数は、2D グラフィックスエンジン描画時のみに使用されます。
                3D グラフィックスエンジン描画時に本関数を呼び出した場合
                警告がメッセージが出力されます。
                
                本関数は、Begin-EndRenderingブロックの内で呼び出す必要があります。
                
                
  Arguments:    pMtx                          [IN]  カレント変換行列
                pCurrentMtxCacheFor2D         [IN]  アフィンパラメータを格納した行列キャッシュ
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
static BOOL CheckMtx2DParamsValid
(
    const MtxFx32*                 pMtx,
    NNSG2dRndCore2DMtxCache*       pCurrentMtxCacheFor2D
)
{
    if( !( pMtx->_00 == FX32_ONE && pMtx->_01 == 0 &&
           pMtx->_10 == 0        && pMtx->_11 == FX32_ONE ) )
    {
       if( pCurrentMtxCacheFor2D == NULL )
       {
           NNS_G2D_WARNING( FALSE,
              "Make sure that you have to specified the affine-mtx for the 2D graphics engine when you use affine transformation." );
           return FALSE;
       }
    }else{
       //
       // とりあえず、警告は行わない事とする
       //
       /*
       if( pCurrentMtxCacheFor2D != NULL )
       {
           NNS_G2D_WARNING( FALSE,
              "The affine mtx setting is useless when you don't use affine transformation." );
           return FALSE;
       }
       */
    }
    return TRUE;
}
//------------------------------------------------------------------------------
void NNS_G2dSetRndCoreCurrentMtx2D
( 
    const MtxFx32*                 pMtx,
    NNSG2dRndCore2DMtxCache*       pCurrentMtxCacheFor2D
)
{
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT();
    NNS_G2D_NULL_ASSERT( pMtx );
    
    NNS_G2D_WARNING( pTheInstance_->pCurrentTargetSurface->type != NNS_G2D_SURFACETYPE_MAIN3D,
       "This method works only for 2D graphics engine rendering." );
    NNS_G2D_ASSERT( CheckMtx2DParamsValid( pMtx, pCurrentMtxCacheFor2D ) );
    
           
    {
       NNSG2dRndCoreInstance*      pRnd = pTheInstance_;
       
       pRnd->pCurrentMxt            = pMtx;
       pRnd->pCurrentMtxCacheFor2D  = pCurrentMtxCacheFor2D;
    }
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetRndCore3DSoftSpriteZvalue

  Description:  ソフトウェアスプライト描画の際に使用されるスプライトのZ値を設定します。
                
  Arguments:    pRnd        [OUT] レンダラコア
                z           [IN]  ソフトウェアスプライト描画に使用されるz値
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetRndCore3DSoftSpriteZvalue( NNSG2dRndCoreInstance* pRnd, fx32 z )
{
    pRnd->zFor3DSoftwareSprite = z;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetRndCoreSurface

  Description:  レンダラコアに描画対象サーフェスを設定します。
  
                Begin-EndRenderingブロックの外で呼び出す必要があります。
                
  Arguments:    pRnd        [OUT] レンダラコア
                pSurface    [IN]  描画対象サーフェス
                
  Returns:      なし。
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetRndCoreSurface
( 
    NNSG2dRndCoreInstance*      pRnd, 
    NNSG2dRndCoreSurface*       pSurface 
)
{
    NNS_G2D_RND_OUTSIDE_BEGINEND_ASSERT();
    NNS_G2D_NULL_ASSERT( pRnd );
    NNS_G2D_NULL_ASSERT( pSurface );
    //
    // TODO:アクティブかどうか調査し、警告メッセージを表示する
    //
    pRnd->pCurrentTargetSurface = pSurface;
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dIsRndCoreFlipH

  Description:  レンダラコアの描画フリップ状態を取得します。
                
  Arguments:    pRnd        [IN]  レンダラコア
                
  Returns:      水平方向フリップ状況 ON のとき TRUE
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dIsRndCoreFlipH( const NNSG2dRndCoreInstance* pRnd )
{
    NNS_G2D_NULL_ASSERT( pRnd );
    return (BOOL)( (pRnd->flipFlag & NNS_G2D_RENDERERFLIP_H) != 0 );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dIsRndCoreFlipV

  Description:  レンダラコアの描画フリップ状態を取得します。
                
  Arguments:    pRnd        [IN]  レンダラコア
                
  Returns:      垂直方向フリップ状況 ON のとき TRUE
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dIsRndCoreFlipV( const NNSG2dRndCoreInstance* pRnd )
{
    NNS_G2D_NULL_ASSERT( pRnd );
    return (BOOL)( (pRnd->flipFlag & NNS_G2D_RENDERERFLIP_V) != 0 );
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetRndCoreFlipMode

  Description:  レンダラコアの描画フリップ状態を設定します。
                Begin-EndRenderingブロックの内外どちらでも呼び出し可能です。
                アフィン変換機能利用時の本関数の呼び出しは禁止されており、アサートに失敗します。
                
  Arguments:    pRnd        [OUT] レンダラコア
                bFlipH      [IN]  水平方向フリップ
                bFlipV      [IN]  垂直方向フリップ
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetRndCoreFlipMode
( 
    NNSG2dRndCoreInstance* pRnd, 
    BOOL bFlipH, 
    BOOL bFlipV 
)
{
    //
    // 現在SR変換されていないか調査する
    // 
    NNS_G2D_NULL_ASSERT( pRnd );
    NNS_G2D_WARNING( pRnd->pCurrentMtxCacheFor2D == NULL, 
       "You can't use the flip function using affine transformation." );

    if( bFlipH )
    {
        pRnd->flipFlag |= NNS_G2D_RENDERERFLIP_H;
    }else{
        pRnd->flipFlag &= ~NNS_G2D_RENDERERFLIP_H;
    }
    
    if( bFlipV )
    {
        pRnd->flipFlag |= NNS_G2D_RENDERERFLIP_V;
    }else{
        pRnd->flipFlag &= ~NNS_G2D_RENDERERFLIP_V;
    }
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dRndCoreBeginRendering

  Description:  レンダラコアの描画を開始します。
                各種描画開始のための前処理を行います。
                
                本関数を呼び出した後は、画像プロクシ、対象レンダサーフェスのパラメータの
                変更は行えませんので注意してください。
                本関数を呼んだ後、描画終了後に必ず NNS_G2dRndCoreEndRendering() を呼び出してください。
                
                Begin-EndRenderingブロック内でのBegin-EndRendering関数の呼び出し(入れ子呼び出し)
                は禁止されており、呼び出しを行った場合は、アサートに失敗します。
                
  Arguments:    pRnd        [IN]  レンダラコア
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dRndCoreBeginRendering( NNSG2dRndCoreInstance* pRnd )
{
    NNS_G2D_NULL_ASSERT( pRnd );
    NNS_G2D_ASSERT( pTheInstance_ == NULL );
    NNS_G2D_ASSERTMSG( IsRndCoreReadyForBeginRendering_( pRnd ), 
       "NNSG2dRndCoreInstance isn't ready to begin rendering." );
    
    pTheInstance_ = pRnd;
    
    //
    // 3D 描画固有前処理
    //
    if( pRnd->pCurrentTargetSurface->type == NNS_G2D_SURFACETYPE_MAIN3D )
    {
        // カメラ設定    
        G3_MtxMode( GX_MTXMODE_PROJECTION );
        G3_PushMtx();
        G3_Translate( -pRnd->pCurrentTargetSurface->viewRect.posTopLeft.x, 
                      -pRnd->pCurrentTargetSurface->viewRect.posTopLeft.y, 0 );
        G3_MtxMode( GX_MTXMODE_POSITION );
        
        //
        // ベースアドレスの計算
        //
        pRnd->baseTexAddr3D = GetTexBaseAddr3D_( pTheInstance_->pImgProxy );
        pRnd->basePltAddr3D = GetPltBaseAddr3D_( pTheInstance_->pPltProxy );
        
    }else{
    //
    // 2D 描画固有前処理
    //
        pTheInstance_->base2DCharOffset 
           = GetCharacterBase2D_( pTheInstance_->pImgProxy, 
                                  pTheInstance_->pCurrentTargetSurface->type );
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dRndCoreEndRendering

  Description:  レンダラコアの描画を終了します。
                各種描画終了のための後処理を行います。
                
                OAM登録関数としてOAMブロック転送関数が設定されている場合は
                本関数内で、ブロック転送関数を使用して、外部モジュールへと内部OAMバッファの
                内容がコピーされます。
                転送は必ず成功することが期待されており、転送に失敗した場合は
                アサートに失敗します。
                （転送に失敗する場合は、内部OAMバッファのバッファサイズの設定に
                誤りがあることが予想されます。）
                
  Arguments:    なし
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dRndCoreEndRendering( void )
{
    NNS_G2D_NULL_ASSERT( pTheInstance_ );
     
    //
    // 3D 描画固有後処理
    //
    if( pTheInstance_->pCurrentTargetSurface->type == NNS_G2D_SURFACETYPE_MAIN3D )
    {
        // カメラ設定(restore)
        G3_MtxMode( GX_MTXMODE_PROJECTION );
        G3_PopMtx(1);
        G3_MtxMode( GX_MTXMODE_POSITION );
        
    }else{
    //
    // 2D 描画固有後処理
    //    
        pTheInstance_->base2DCharOffset = 0;
        //
        // アフィン変換設定のリセット
        //
        pTheInstance_->pCurrentMxt = NULL;
        pTheInstance_->pCurrentMtxCacheFor2D = NULL;
    }    
    pTheInstance_ = NULL;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dRndCoreDrawCell

  Description:  セルを描画します。
                Begin-EndRenderingブロックの内で呼び出す必要があります。
                
  Arguments:    pCell:                    [IN]  セルデータ
  
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dRndCoreDrawCell( const NNSG2dCellData* pCell )
{
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT();
    NNS_G2D_NULL_ASSERT( pCell );
    
    DrawCellImpl_( pCell, NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE );   
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dRndCoreDrawCellVramTransfer

  Description:  VRAM転送セルを描画します。
                Begin-EndRenderingブロックの内で呼び出す必要があります。
                
  Arguments:    pCell:                    [IN]  セルデータ
                cellVramTransferHandle:   [IN]  セルVRAM転送状態オブジェクトのハンドル   
  
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dRndCoreDrawCellVramTransfer( const NNSG2dCellData* pCell, u32 cellVramTransferHandle )
{
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT();
    NNS_G2D_NULL_ASSERT( pCell );
    
    NNS_G2D_ASSERT( cellVramTransferHandle != NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE );
    DrawCellImpl_( pCell, cellVramTransferHandle );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dRndCoreDrawCellFast2D

  Description:  2D グラフィックエンジン(OBJ機能)を利用してセルを高速に描画します。
                
                本関数は、コールバック関数呼び出し、セルのアフィン変換をサポートしません。
                2D用アフィン変換行列（pCurrentMtxCacheFor2D）が設定された状態で、
                本関数を呼び出した場合、アサートに失敗します。
                
                Begin-EndRenderingブロックの内で呼び出す必要があります。
                
  Arguments:    pCell:      [IN]  セルデータ
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dRndCoreDrawCellFast2D( const NNSG2dCellData* pCell )
{
    const NNSG2dRndCoreSurface*   pSurface = pTheInstance_->pCurrentTargetSurface;    
    NNSG2dFVec2          baseTrans;
    const MtxFx32*       pCurrMtx = RndCoreGetCurrentMtx_();
    
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT();
    
    
    
    NNS_G2D_ASSERTMSG( pSurface->type == NNS_G2D_SURFACETYPE_MAIN2D || 
                      pSurface->type == NNS_G2D_SURFACETYPE_SUB2D,
                      "This method can work only for the 2D Graphics Engine." );
       
    NNS_G2D_ASSERTMSG( pTheInstance_->pCurrentMtxCacheFor2D == NULL,
                      "You can't use this method using affine transfomation." );
    NNS_G2D_NULL_ASSERT( pCell );   
    //------------------------------------------------------------------------------
    // セル を 2D Graphics Engine を使用して 描画します
    // コールバック呼び出しなどをサポートしませんが高速に動作するバージョンです。
    // 

    // get current Mtx for affine transformation.
    NNSi_G2dGetMtxTrans( pCurrMtx, &baseTrans );  
       
    //
    // ビューローカル系への変換
    //                  
    baseTrans.x -= pSurface->viewRect.posTopLeft.x;
    baseTrans.y -= pSurface->viewRect.posTopLeft.y;
    //
    // NNS_G2dMakeCellToOams() とほぼ同じ処理を行います。
    // （ただし、フリップをサポートします。）
    //
    //
    // ブロック転送を使用しないバージョン
    // OBJ登録の度に、描画登録関数よびだしのオーバーヘッドが発生します。
    // ブロック転送を使用するバージョンと比べて、パフォーマンス的には不利ですが、
    // もともと本関数の描画登録処理の1フレームに占める割合は小さいので
    // （128 個X2 のOBJを登録したとしても）、問題はないと判断し本方式を採用することとします。
    // 
    {
       u16          i;
       GXOamAttr*   pTempOam;
       const u32    baseCharOffset = pTheInstance_->base2DCharOffset;

       for( i = 0; i < pCell->numOAMAttrs; i++ )
       {
           pTempOam = &pTheInstance_->currentOam;
                        
           NNS_G2dCopyCellAsOamAttr( pCell, i, pTempOam );
           
           DoFlipTransforme_( pTempOam, &baseTrans );
                        
           pTempOam->charNo += baseCharOffset;
                        
           //
           // 描画登録関数よびだし
           //
           NNS_G2D_NULL_ASSERT( pTheInstance_->pFuncOamRegister );
           if( FALSE == (*pTheInstance_->pFuncOamRegister)( pTempOam, NNS_G2D_OAM_AFFINE_IDX_NONE, FALSE ) )
           {
              // we have no capacity for new oam datas.
              return;
           }
       }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetRndCoreCellCloneSource3D

  Description:  NNS_G2dRndCoreDrawCellClone3D()で参照される、UVパラメータを事前計算し、
                UVパラメータキャッシュとして保存します。
                入力された、セルが同一の画像データを持つOBJから構成されていると想定して、
                インデックス0番のOBJについてUV値を計算し、キャッシュとして保存します。
                
                注意：フリップされたスプライトのUVパラメータは通常のUVパラメータとは異なります。
                したがって同一の画像データを持つOBJとはみなせないことになります。
                3D サーフェス専用の処理です。現在のサーフェス種類が、
                NNS_G2D_SURFACETYPE_MAIN3D以外の場合はアサートに失敗します。
                
                Begin-EndRenderingブロックの内で呼び出す必要があります。
                
  Arguments:    pCell:      [IN]  セルデータ
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetRndCoreCellCloneSource3D( const NNSG2dCellData* pCell )
{
    
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT();
    NNS_G2D_ASSERTMSG( pTheInstance_->pCurrentTargetSurface->type == NNS_G2D_SURFACETYPE_MAIN3D,
                          "This method can work only for the 3D Graphics Engine." );
    NNS_G2D_NULL_ASSERT( pCell );         
    
    {   
        GXOamAttr*            pOam = &pTheInstance_->currentOam;
        // 先頭のOBJを代表として...
        NNS_G2dCopyCellAsOamAttr( pCell, 0, pOam );
        // パラメータキャッシュとして設定します
        // この後の描画について、
        // テクスチャパラメータはこの値が参照されます。
        
        
        //
        // レンダラのフリップ設定をOAMアトリビュートに反映する
        //        
        OverwriteOamFlipFlag_( pOam );
               
               
        NNS_G2dSetOamSoftEmuSpriteParamCache
        (
           pOam, 
           &pTheInstance_->pImgProxy->attr, 
           pTheInstance_->baseTexAddr3D, 
           pTheInstance_->basePltAddr3D 
        );    
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dRndCoreDrawCellClone3D

  Description:  同一の画像データを持つOBJから構成されるセルをソフトウェアスプライトを
                使用して高速に描画します。
                
                同一の画像データを持つOBJとはソフトウェアスプライト（テクスチャ付四角ポリゴン）
                として描画する際にUV値が同一になるOBJのことです。
                
                同一の画像データを持つOBJであれば、セル中のOBJは単数でも複数でもかまいません。
                本関数は、事前に計算された、UVパラメータキャッシュを利用して描画をおこないます。
                通常のソフトウェアスプライト描画で必要となる、UV値の計算処理を省くことが可能になるため
                、高速に動作します。
                UVパラメータキャッシュを設定するために、
                実行前にNNS_G2dSetRndCoreCellCloneSource3D()を呼び出して、パラメータの初期化
                を済ませておく必要があります。
                
                注意：フリップされたスプライトのUVパラメータは通常のUVパラメータとは異なります。
                したがって同一の画像データを持つOBJとはみなせないことになります。
                
                
                本関数はアフィン変換をサポートします。各種コールバック呼び出しをサポートしません。
                
                3D サーフェス専用の処理です。現在のサーフェス種類が、
                NNS_G2D_SURFACETYPE_MAIN3D以外の場合はアサートに失敗します。
                
                Begin-EndRenderingブロックの内で呼び出す必要があります。
                
  Arguments:    pCell:      [IN]  セルデータ
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dRndCoreDrawCellClone3D( const NNSG2dCellData* pCell )
{
    u16 i = 0;
    
    GXOamAttr*            pOam = &pTheInstance_->currentOam;
    
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT();
    NNS_G2D_ASSERTMSG( pTheInstance_->pCurrentTargetSurface->type == NNS_G2D_SURFACETYPE_MAIN3D,
                          "This method can work only for the 3D Graphics Engine." );
    NNS_G2D_NULL_ASSERT( pTheInstance_->pCurrentTargetSurface );
    NNS_G2D_NULL_ASSERT( pCell );
    
    if( pTheInstance_->flipFlag != NNS_G2D_RENDERERFLIP_NONE )
    {
        //
        // フリップ描画
        //
        for( i = 0; i < pCell->numOAMAttrs; i++ )
        {
           NNS_G2dCopyCellAsOamAttr( pCell, i, pOam );
           
           // 行列設定
           G3_LoadMtx43( &pTheInstance_->mtxFor3DGE );
    
           // 描画
           {               
               const GXOamShape shape = NNS_G2dGetOAMSize( pOam );
                
               //
               // OBJ の 描画位置を取得する
               //
               const s16 posX = (s16)GetFlipedOBJPosX_( pOam, shape );
               const s16 posY = (s16)GetFlipedOBJPosY_( pOam, shape );
               const s16 posZ = -1;      
                
               //
               // レンダラのフリップ設定をOAMアトリビュートに反映する
               //        
               OverwriteOamFlipFlag_( pOam );
               
                
               NNS_G2dDrawOneOam3DDirectUsingParamCacheFast( posX, posY, posZ, pOam );    
           }
        }
    }else{
        //
        // フリップ無し描画
        //
        for( i = 0; i < pCell->numOAMAttrs; i++ )
        {
           NNS_G2dCopyCellAsOamAttr( pCell, i, pOam );
           
           // 行列設定
           G3_LoadMtx43( &pTheInstance_->mtxFor3DGE );
    
           {
               const s16 posX = (s16)NNS_G2dRepeatXinCellSpace( (s16)pOam->x );
               const s16 posY = (s16)NNS_G2dRepeatYinCellSpace( (s16)pOam->y );
               const s16 posZ = -1;
                               
               NNS_G2dDrawOneOam3DDirectUsingParamCacheFast( posX, posY, posZ, pOam );
           }
        }
    }
}
