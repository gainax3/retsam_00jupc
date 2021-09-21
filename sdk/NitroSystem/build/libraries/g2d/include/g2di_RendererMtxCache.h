/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d
  File:     g2di_RendererMtxCache.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.11 $
 *---------------------------------------------------------------------------*/
 
#ifndef NNS_G2DI_RENDERERMTXCACHE_H_
#define NNS_G2DI_RENDERERMTXCACHE_H_

#include <nnsys/g2d/g2d_RendererCore.h> // For NNSG2dRndCore2DMtxCache, NNSG2dSurfaceType
#include "g2d_Internal.h"
#include "g2di_RendererMtxStack.hpp"// For MatrixStack 


//------------------------------------------------------------------------------
typedef enum OAM_FLIP
{
    OAM_FLIP_NONE   = 0,
    OAM_FLIP_H      = 0x01,
    OAM_FLIP_V      = 0x02,
    OAM_FLIP_HV     = 0x03,
    OAM_FLIP_MAX    = NNS_G2D_OAMFLIP_PATTERN_NUM

}OAM_FLIP;

#define OAM_FLIP_ASSERT( val )                                       \
        NNS_G2D_MINMAX_ASSERT( (val), OAM_FLIP_NONE, OAM_FLIP_HV )    \

// convert from flags to OAM_FLIP
#define TO_OAM_FLIP( bFlipH, bFlipV )           (OAM_FLIP)(( bFlipV << 1 ) | ( bFlipH ))


//------------------------------------------------------------------------------

#define G2Di_NUM_MTX_CACHE                      32



//------------------------------------------------------------------------------
// Renderer 2DMatrix Cache 
// 実際の 2D Graphics Enigine への Affine パラメータ登録を キャシュする役割を果たします。
// 重複する 登録は 実行されず過去の登録結果が返されます。
// 
// これによって、同一 NNSG2dRndCore2DMtxCache Index を 指定した場合 
// 同一 Affine パラメータ を参照することとなります。
//
// affineIndex は 規定値で Affine パラメータ登録 がなされていないという意味の 
// MtxCache_NOT_AVAILABLE を持ちます。
// 
// 規定値 への リセット は NNS_G2dEndRendering() で呼ばれる NNSi_G2dMCMCleanupMtxCache() によって行われます。
// つまり、Affineパラメータを共有可能なのは 
// 同一 NNS_G2dBeginRendering() NNS_G2dEndRendering() ブロック内 に限られることとなります。
//
// 
// 本モジュールはレンダラモジュールから直接操作されることはなく、
// RendererMtxState モジュールメソッド経由で操作されます。
//
//
// 関数命名 NNSi_RMC.....()          
static NNSG2dRndCore2DMtxCache             mtxCacheBuffer_[G2Di_NUM_MTX_CACHE];

//------------------------------------------------------------------------------
// 追加変数：NNSG2dRndCore2DMtxCacheをユーザから隠蔽するために導入
static u16                          currentMtxCachePos_ = 0;



//------------------------------------------------------------------------------
// モジュール内部限定関数
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// モジュール外部公開関数
//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNSi_RMCInitMtxCache()
{
    int i;
    for( i = 0; i < G2Di_NUM_MTX_CACHE; i++ )
    {
        NNS_G2dInitRndCore2DMtxCache( &mtxCacheBuffer_[i] );
    }
    currentMtxCachePos_ = 0;
}

NNS_G2D_INLINE void NNSi_RMCResetMtxCache()
{
    int i;
    //
    // 使用したところまで、初期化する
    //
    for( i = 0; i < currentMtxCachePos_; i++ )
    {
        NNS_G2dInitRndCore2DMtxCache( &mtxCacheBuffer_[i] );
    }
    currentMtxCachePos_ = 0;
}

//------------------------------------------------------------------------------
// インデックスで行列キャッシュを取得する
NNS_G2D_INLINE NNSG2dRndCore2DMtxCache* NNSi_RMCGetMtxCacheByIdx( u16 idx )
{
    NNS_G2D_MINMAX_ASSERT( idx, 0, G2Di_NUM_MTX_CACHE - 1);
    return &mtxCacheBuffer_[idx];
}             

//------------------------------------------------------------------------------
// 新たにひとつ行列キャッシュを使用する
NNS_G2D_INLINE u16 NNSi_RMCUseNewMtxCache()
{
    const u16 ret = currentMtxCachePos_;
    
    if( currentMtxCachePos_ < G2Di_NUM_MTX_CACHE - 1)
    {
       currentMtxCachePos_++;
    }else{
       // 行列キャッシュの枯渇
       NNS_G2D_WARNING( FALSE, "MtxCache is running out. G2d ignores the user request"
                               ", and uses MtxCache-Idx = 31.");
    }
    
    return ret;
}

#endif // NNS_G2DI_RENDERERMTXCACHE_H_