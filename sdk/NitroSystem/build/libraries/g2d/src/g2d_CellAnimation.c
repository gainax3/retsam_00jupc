/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d
  File:     g2d_CellAnimation.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.40 $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nnsys/g2d/g2d_CellAnimation.h>
#include <nnsys/g2d/g2d_SRTControl.h>

#include <nnsys/g2d/load/g2d_NCE_load.h>
#include <nnsys/g2d/fmt/g2d_Oam_data.h>

#include "g2d_Internal.h"

//------------------------------------------------------------------------------
// アニメーション変更結果を適用する
//
// 少し乱暴だが、どんなアニメフォーマットであってもNNSG2dAnimDataSRTでアクセスしてしまう
// (アニメフォーマットがNNS_G2D_ANIMELEMENT_INDEXの場合は SRT 部分には不正なデータが入る事となる）
//     
static void ApplyCurrentAnimResult_( NNSG2dCellAnimation* pCellAnim )
{
    const NNSG2dAnimDataSRT*   pAnimResult  = NULL;
    const NNSG2dCellDataBank*  pCellBank    = NULL;
    
    NNS_G2D_NULL_ASSERT( pCellAnim );
    NNS_G2D_NULL_ASSERT( pCellAnim->pCellDataBank );
    
    //
    // 表示時間がゼロである、アニメーションフレームの場合は、更新作業を何も行いません。
    //
    if( pCellAnim->animCtrl.pActiveCurrent->frames == 0 )
    {
        return; 
    }
    pAnimResult = (const NNSG2dAnimDataSRT*)NNS_G2dGetAnimCtrlCurrentElement( &pCellAnim->animCtrl );
    NNS_G2D_NULL_ASSERT( pAnimResult );

    
    //
    // 複数のフォーマットに対応することになる予定です
    //
    pAnimResult = (const NNSG2dAnimDataSRT*)NNS_G2dGetAnimCtrlCurrentElement( &pCellAnim->animCtrl );
    pCellBank   = pCellAnim->pCellDataBank;
    
    NNSI_G2D_DEBUGMSG0( "pAnimResult->index = %d\n", pAnimResult->index );
    
    pCellAnim->pCurrentCell 
        = NNS_G2dGetCellDataByIdx( pCellBank, pAnimResult->index );
    NNS_G2D_NULL_ASSERT( pCellAnim->pCurrentCell );
    
    //
    // SRT を 反映 
    //
    {
       const NNSG2dAnimationElement elemType 
           = NNSi_G2dGetAnimSequenceElementType( pCellAnim->animCtrl.pAnimSequence->animType );
       NNSi_G2dSrtcInitControl  ( &pCellAnim->srtCtrl, NNS_G2D_SRTCONTROLTYPE_SRT );
       
       if( elemType != NNS_G2D_ANIMELEMENT_INDEX )
       {
           if( elemType == NNS_G2D_ANIMELEMENT_INDEX_T )
           {
              const NNSG2dAnimDataT*  pAnmResT = (const NNSG2dAnimDataT*)pAnimResult;
              NNSi_G2dSrtcSetTrans     ( &pCellAnim->srtCtrl, pAnmResT->px, pAnmResT->py );// T
           }else{
              NNSi_G2dSrtcSetSRTScale  ( &pCellAnim->srtCtrl, pAnimResult->sx, pAnimResult->sy );// S    
              NNSi_G2dSrtcSetSRTRotZ   ( &pCellAnim->srtCtrl, pAnimResult->rotZ );// R    
              NNSi_G2dSrtcSetTrans     ( &pCellAnim->srtCtrl, pAnimResult->px, pAnimResult->py );// T    
           }
       }
    }
    
    
   
    //
    // もしVRAM転送情報が設定されているなら...
    //
    if( NNS_G2dCellDataBankHasVramTransferData( pCellBank ) && 
        NNSi_G2dIsCellAnimVramTransferHandleValid( pCellAnim ) )
    {
        const NNSG2dCellVramTransferData*   pCellTransferData 
            = NNSi_G2dGetCellVramTransferData( pCellBank, pAnimResult->index );   
        //
        // 転送をリクエストします
        //
        NNS_G2dSetCellTransferStateRequested( pCellAnim->cellTransferStateHandle,
                                              pCellTransferData->srcDataOffset,
                                              pCellTransferData->szByte );
    }
}

//------------------------------------------------------------------------------
// セルアニメーションを初期化します。
// NNS_G2dInitCellAnimation の実装部分です。
static NNS_G2D_INLINE void InitCellAnimationImpl_
( 
    NNSG2dCellAnimation*        pCellAnim, 
    const NNSG2dAnimSequence*   pAnimSeq, 
    const NNSG2dCellDataBank*   pCellDataBank,
    u32                         cellTransferStateHandle 
)
{
    NNS_G2D_NULL_ASSERT( pCellAnim );
    NNS_G2D_NULL_ASSERT( pAnimSeq );
    NNS_G2D_NULL_ASSERT( pCellDataBank );
    

    pCellAnim->pCellDataBank            = pCellDataBank;
    pCellAnim->cellTransferStateHandle   = cellTransferStateHandle;
    
    // TODO: srtCtrl の 種類 は アニメーションの形式に応じて、設定する必要がある
    //       バインドのたびに再度設定する必要がある。
    NNSi_G2dSrtcInitControl( &pCellAnim->srtCtrl, NNS_G2D_SRTCONTROLTYPE_SRT );
    
    NNS_G2dInitAnimCtrl( &pCellAnim->animCtrl );
    NNS_G2dSetCellAnimationSequence( pCellAnim, pAnimSeq );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitCellAnimation

  Description:  NNSG2dCellAnimation  実体を初期化します
                
  Arguments:    pCellAnim:           [OUT]  セルアニメーション実体
                pAnimSeq:            [IN]   アニメーションデータ
                pCellDataBank:       [IN]   セルデータバンク
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dInitCellAnimation
( 
    NNSG2dCellAnimation*        pCellAnim, 
    const NNSG2dAnimSequence*   pAnimSeq, 
    const NNSG2dCellDataBank*   pCellDataBank 
)
{
    NNS_G2D_NULL_ASSERT( pCellAnim );
    NNS_G2D_NULL_ASSERT( pAnimSeq );
    NNS_G2D_NULL_ASSERT( pCellDataBank );
    
    InitCellAnimationImpl_( pCellAnim, 
                            pAnimSeq, 
                            pCellDataBank, 
                            NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE ); 
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitCellAnimationVramTransfered

  Description:  VRAM転送アニメーションをする、セルアニメーションを初期化します。
                
  Arguments:    pCellAnim:           [OUT] セルアニメーション実体
                pAnimSeq:            [IN]  アニメーションデータ
                pCellBank:           [IN]  セルデータバンク
                vramStateHandle:     [IN]  セルVRAM転送状態オブジェクトのハンドル
                dstAddr3D:           [IN]  転送先データ(3D 用)
                dstAddr2DMain:       [IN]  転送先データ(2D Main用)
                dstAddr2DSub:        [IN]  転送先データ(2D Sub 用)
                pSrcNCGR:            [IN]  転送元データ(NCGR)
                pSrcNCBR:            [IN]  転送元データ(NCBR)
                szSrcData:           [IN]  転送元データサイズ
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dInitCellAnimationVramTransfered
(
    NNSG2dCellAnimation*        pCellAnim, 
    const NNSG2dAnimSequence*   pAnimSeq, 
    const NNSG2dCellDataBank*   pCellBank,
    u32                         vramStateHandle,
    u32                         dstAddr3D,
    u32                         dstAddr2DMain,
    u32                         dstAddr2DSub,
    const void*                pSrcNCGR,
    const void*                pSrcNCBR,
    u32                         szSrcData
     
)
{
    //
    // VRAM転送情報を持っていること
    //
    NNS_G2D_ASSERT( NNS_G2dCellDataBankHasVramTransferData( pCellBank ) );
    
    //
    // Vram転送関連の初期化
    //
    {                                                
        const NNSG2dVramTransferData*    pVramData = 
            (const NNSG2dVramTransferData*)pCellBank->pVramTransferData;
        
        // 設定ワークの初期化
        NNSi_G2dInitCellTransferState( vramStateHandle,
                                        dstAddr3D,              // dst 転送先Vramアドレスの設定
                                        dstAddr2DMain,          // dst 転送先Vramアドレスの設定
                                        dstAddr2DSub,           // dst 転送先Vramアドレスの設定
                                        pVramData->szByteMax,   // dst 転送する最大データサイズ
                                        pSrcNCGR,               // src 2D char-data
                                        pSrcNCBR,               // src 3D Tex-data
                                        szSrcData );            // src サイズ 
           
        NNSi_G2dSetCellAnimVramTransferHandle( pCellAnim, vramStateHandle );
    }
    
    //
    // Cell アニメーションの初期化
    //
    InitCellAnimationImpl_( pCellAnim, pAnimSeq, pCellBank, vramStateHandle );        
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetCellAnimationSequence

  Description:  セルアニメーションにアニメーションシーケンスを設定します。

  Arguments:    pCellAnim   :      [OUT]     セルアニメーション実体
                pAnimSeq    ：     [IN]      アニメーションシーケンス
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetCellAnimationSequence
( 
    NNSG2dCellAnimation*        pCellAnim, 
    const NNSG2dAnimSequence*   pAnimSeq 
)
{
    NNS_G2D_NULL_ASSERT( pCellAnim );
    NNS_G2D_NULL_ASSERT( pAnimSeq );
    NNS_G2D_ASSERTMSG( NNS_G2dGetAnimSequenceAnimType( pAnimSeq ) == NNS_G2D_ANIMATIONTYPE_CELL, 
                       "A cell-Animation's Data is expected");
    
    NNS_G2dBindAnimCtrl( &pCellAnim->animCtrl, pAnimSeq );
    ApplyCurrentAnimResult_( pCellAnim );
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetCellAnimationSequenceNoReset

  Description:  セルアニメーションにアニメーションシーケンスを設定します。
                内部の再生アニメーションフレーム番号、現在フレーム表示時間をリセットしません。

  Arguments:    pCellAnim   :    [OUT]      セルアニメーション実体
                pAnimSeq    ：   [IN]       アニメーションシーケンス
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetCellAnimationSequenceNoReset
( 
    NNSG2dCellAnimation*        pCellAnim, 
    const NNSG2dAnimSequence*   pAnimSeq 
)
{
    NNS_G2D_NULL_ASSERT( pCellAnim );
    NNS_G2D_NULL_ASSERT( pAnimSeq );
    NNS_G2D_ASSERTMSG( NNS_G2dGetAnimSequenceAnimType( pAnimSeq ) == NNS_G2D_ANIMATIONTYPE_CELL,
                       "A cell-Animation's Data is expected");
    
    {
        const u16 frameIdx = NNS_G2dGetAnimCtrlCurrentFrame( &pCellAnim->animCtrl );
        
        // カウンターのリセットをしない
        pCellAnim->animCtrl.pAnimSequence = pAnimSeq;
        
        if( !NNS_G2dSetAnimCtrlCurrentFrameNoResetCurrentTime( &pCellAnim->animCtrl, frameIdx ) )
        {
            NNS_G2dResetAnimationState( &pCellAnim->animCtrl );
        }
    }
    
    ApplyCurrentAnimResult_( pCellAnim );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTickCellAnimation

  Description:  NNSG2dCellAnimation の 時間を進めます
                
  Arguments:    pCellAnim:          [OUT] セルアニメーション実体
                frames:             [IN]  進める時間（フレーム）
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dTickCellAnimation( NNSG2dCellAnimation* pCellAnim, fx32 frames )
{
    NNS_G2D_NULL_ASSERT( pCellAnim );
    NNS_G2D_NULL_ASSERT( pCellAnim->animCtrl.pAnimSequence );
    NNS_G2D_ASSERTMSG( NNS_G2dGetAnimSequenceAnimType( pCellAnim->animCtrl.pAnimSequence ) 
        == NNS_G2D_ANIMATIONTYPE_CELL, "A cell-Animation's Data is expected");
    
    if( NNS_G2dTickAnimCtrl( &pCellAnim->animCtrl, frames ) )
    {
        //
        // Frame更新が起きたなら、アニメーション結果を反映する
        //
        ApplyCurrentAnimResult_( pCellAnim );
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetCellAnimationCurrentFrame

  Description:  セルアニメーションのアニメーションフレームを設定します。
                不正なフレーム番号が指定された場合は、何もしません。
                
  Arguments:    pCellAnim   :           [OUT] セルアニメーション実体
                frameIndex  ：          [IN]  アニメーションフレーム番号
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetCellAnimationCurrentFrame
( 
    NNSG2dCellAnimation*    pCellAnim, 
    u16                     frameIndex 
)
{
    NNS_G2D_NULL_ASSERT( pCellAnim );
    NNS_G2D_NULL_ASSERT( pCellAnim->animCtrl.pAnimSequence );
    
    if( NNS_G2dSetAnimCtrlCurrentFrame( &pCellAnim->animCtrl, frameIndex ) )
    {
        //
        // Frame更新が起きたなら、アニメーション結果を反映する
        //
        ApplyCurrentAnimResult_( pCellAnim );
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dRestartCellAnimation

  Description:  セルアニメーションの再生を先頭から再スタートします。
                反転再生している場合は、シーケンス終端から再生を再スタートします。
                
  Arguments:    pCellAnim   :           [OUT] セルアニメーション実体
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dRestartCellAnimation
( 
    NNSG2dCellAnimation*        pCellAnim
)
{
    NNSG2dAnimController* pAnmCtrl = NULL;
    NNS_G2D_NULL_ASSERT( pCellAnim );   
    
    // 再生アニメーションフレームを先頭にセットします。
    pAnmCtrl = NNS_G2dGetCellAnimationAnimCtrl( pCellAnim );
    NNS_G2dResetAnimCtrlState( pAnmCtrl );
    
    // 再生をスタートします。
    NNS_G2dStartAnimCtrl( pAnmCtrl );
    
    // アニメーション結果を反映します。
    ApplyCurrentAnimResult_( pCellAnim );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetCellAnimationSpeed

  Description:  セルアニメーションの再生スピードを設定します。
                
  Arguments:    pCellAnim   :           [OUT] セルアニメーション実体
                speed       ：          [IN]  再生スピード
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetCellAnimationSpeed
(
    NNSG2dCellAnimation*     pCellAnim,
    fx32                     speed  
)
{
    NNS_G2D_NULL_ASSERT( pCellAnim );
    NNS_G2D_NULL_ASSERT( pCellAnim->animCtrl.pAnimSequence );
    
    NNS_G2dSetAnimCtrlSpeed( &pCellAnim->animCtrl, speed );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dMakeCellToOams

  Description:  CellをOam列に変換します
                
  Arguments:    pDstOams:           [OUT] Oamが格納されるバッファ
                numDstOam:          [IN] バッファ長
                pCell:              [IN] 変換元のCell
                pMtxSR:             [IN] Cellに設定する変換     （任意）
                pBaseTrans:         [IN] Cellに設定する平行移動 （任意）
                bDoubleAffine:      [IN] DoubleAffine モードかどうか
                
  Returns:      実際に変換したOamAttribute数
  
 *---------------------------------------------------------------------------*/
u16 NNS_G2dMakeCellToOams
( 
    GXOamAttr*              pDstOams,
    u16                     numDstOam, 
    const NNSG2dCellData*   pCell, 
    const MtxFx22*          pMtxSR, 
    const NNSG2dFVec2*      pBaseTrans,
    u16                     affineIndex,
    BOOL                    bDoubleAffine 
)
{
    u16             i = 0;
    NNSG2dFVec2     objTrans;
    GXOamAttr*      pDstOam = NULL;
    const u16       numOBJ 
        = ( numDstOam < pCell->numOAMAttrs ) ? numDstOam : pCell->numOAMAttrs;
    
    
    for( i = 0; i < numOBJ; i++ )
    {
        pDstOam = &pDstOams[i];
        
        NNS_G2dCopyCellAsOamAttr( pCell, i, pDstOam );
        
        //
        // OBJの位置を変更する必要があるなら...
        //
        if( pMtxSR != NULL || pBaseTrans != NULL )
        {
            //
            // 位置の取得
            //
            NNS_G2dGetOamTransFx32( pDstOam, &objTrans );
                    
            //
            // affine 変換が指定されているなら...
            // 
            if( pMtxSR != NULL )
            {   
                //
                // 倍角アフィンが設定されているOBJなら、
                // NITRO-CHARACTER が付加している補正値を一旦、取り去ります
                //
                NNSi_G2dRemovePositionAdjustmentFromDoubleAffineOBJ( pDstOam, 
                                                                     &objTrans );               
                {
                    // 上書き
                    const GXOamEffect effectTypeAfter = ( bDoubleAffine ) ? 
                                             GX_OAM_EFFECT_AFFINE_DOUBLE : GX_OAM_EFFECT_AFFINE;                
                    const BOOL bShouldAdjust = ( effectTypeAfter  == GX_OAM_EFFECT_AFFINE_DOUBLE );
        
                    MulMtx22( pMtxSR, &objTrans, &objTrans );
                    
                    // affine Index の設定
                    G2_SetOBJEffect( pDstOam, effectTypeAfter, affineIndex );
                    
                    NNSi_G2dAdjustDifferenceOfRotateOrientation( pDstOam, 
                                                                 pMtxSR, 
                                                                 &objTrans, 
                                                                 bShouldAdjust );
                }
            }
            //
            // 平行移動成分が指定されているなら...
            // 
            if( pBaseTrans != NULL )
            {
                objTrans.x += pBaseTrans->x;
                objTrans.y += pBaseTrans->y;
            }
            
            //
            // 書き戻し
            //
            // 0x800 => 四捨五入を意図しています
            G2_SetOBJPosition( pDstOam, 
                               ( objTrans.x + 0x800 )>> FX32_SHIFT, 
                               ( objTrans.y + 0x800 )>> FX32_SHIFT );
        }
    }
    return numOBJ;
}

