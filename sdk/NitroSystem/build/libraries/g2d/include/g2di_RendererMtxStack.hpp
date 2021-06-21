/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d
  File:     g2di_RendererMtxStack.hpp

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.13 $
 *---------------------------------------------------------------------------*/

//
// 本モジュールは、g2d_renderer.c 内で使用される MtrixStackを実装します。
// 本来、同一ファイル内に記述するべきものですが、コード保守性のために分割しています。
// 
//


//
// This module should be exposed inside the library only.
//

//
// Mtx32 is referenced by renderer module.
//

#ifndef NNS_G2DI_RENDERERMTXSTACK_HPP_
#define NNS_G2DI_RENDERERMTXSTACK_HPP_

#include <nitro.h>
#include <nnsys/g2d/fmt/g2d_Vec_data.h>
#include "g2di_Mtx32.h"



#define NNS_G2D_MAXLENGTH_OF_2DMTXSTACK         32
#define G2Di_RENDERER_MTX_SR_NONE               NNS_G2D_OAM_AFFINE_IDX_NONE

static MtxFx32        mtxStack_        [NNS_G2D_MAXLENGTH_OF_2DMTXSTACK];
static MtxFx32        mtxStackFor2DHW_ [NNS_G2D_MAXLENGTH_OF_2DMTXSTACK];
static fx32           fxZStack_[NNS_G2D_MAXLENGTH_OF_2DMTXSTACK];
static s32            stackPos_ = 0;

static u16            firstSRTransformedMtxIdx_   = G2Di_RENDERER_MTX_SR_NONE;


//------------------------------------------------------------------------------
// SR 変換を使用するか？
// SR 変換を使用しない場合に処理の効率化を行います。
// 
// SR 変換を使用しない場合は、平行移動変換が加減算のみで十分です。
// また、2DHW用の行列スタックの考慮も必要ありません。
//（bDonotUseSRTransform == TRUEでは、mtxStackFor2DHW_は更新されません）
//
static BOOL           bDonotUseSRTransform = FALSE;
static MtxFx32        mtxI_                = { FX32_ONE, 0, 0, FX32_ONE, 0, 0 };
//------------------------------------------------------------------------------





//------------------------------------------------------------------------------
static void           NNSi_G2dInitRndMtxStack();
static BOOL           NNSi_G2dIsStackEmpty();
static BOOL           NNSi_G2dStackHasEnoughCapacity();

static void           NNSi_G2dMtxPush     ();
static void           NNSi_G2dMtxPop      ();
static void           NNSi_G2dTranslate   ( fx32 x, fx32 y, fx32 z );
static void           NNSi_G2dSetTrans    ( fx32 x, fx32 y, fx32 z );
static void           NNSi_G2dRotate      ( fx32 sin, fx32 cos );
static void           NNSi_G2dScale       ( fx32 sx, fx32 sy );
static void           NNSi_G2dIdentity    ( );

static const MtxFx32* NNSi_G2dGetCurrentMtx();
static u16            NNSi_G2dGetMtxStackPos();

static void           NNSi_G2dSetRndMtxStackSRTransformed();
static BOOL           NNSi_G2dIsRndCurrentMtxSRTransformed();

// モジュール内部
static void RestoreRndMtxStackStateNotSR_();// 内部
static void CheckIfRndCurrentMtxIsSRTransformed_();

//------------------------------------------------------------------------------
// Renderer内部 の 状態を SR affine 変換 されていない状態 に 設定します。
// CheckIfRndCurrentMtxIsSRTransformed_() によって呼ばれます
// 
NNS_G2D_INLINE void RestoreRndMtxStackStateNotSR_()
{
    firstSRTransformedMtxIdx_ = G2Di_RENDERER_MTX_SR_NONE;
}

//------------------------------------------------------------------------------
// スタックの初期化
NNS_G2D_INLINE void NNSi_G2dInitRndMtxStack( )
{
    stackPos_                 = 0;
    RestoreRndMtxStackStateNotSR_();
}

//------------------------------------------------------------------------------
// SR 変換を使用するか？
// SR 変換を使用しない場合に処理の効率化を行います。
// 
NNS_G2D_INLINE void NNSi_G2dSetRndMtxStackSRTransformEnableFlag( BOOL bEnable )
{
    bDonotUseSRTransform = !bEnable;
}



//------------------------------------------------------------------------------
// Renderer内部 の 状態を SR affine 変換 されている状態 に 設定します。
// NNS_G2dScale() NNS_G2dRotZ() から呼ばれます。
//
// SR affine 変換 されている状態 時に呼び出すことは不正です
//
NNS_G2D_INLINE void NNSi_G2dSetRndMtxStackSRTransformed()
{
    NNS_G2D_ASSERTMSG( firstSRTransformedMtxIdx_ ==  G2Di_RENDERER_MTX_SR_NONE,
                    "firstSRTransformedMtxIdx_ must be set only in the first time of using SR transform." );    
    
    NNSI_G2D_DEBUGMSG1( "Affine SR == ON : in stack_position %d \n", 
                         NNSi_G2dGetMtxStackPos() );
                         
    
    firstSRTransformedMtxIdx_ = NNSi_G2dGetMtxStackPos();
}

//------------------------------------------------------------------------------
// カレント行列が SR 変換されているか調査します
// SR 変換 されている場合は OBJ を Affine 変換 する必要があります ( 2D Graphics Engine )
//
//
NNS_G2D_INLINE BOOL NNSi_G2dIsRndCurrentMtxSRTransformed()
{
    return (BOOL)( !bDonotUseSRTransform && 
                   firstSRTransformedMtxIdx_ != G2Di_RENDERER_MTX_SR_NONE );
}

//------------------------------------------------------------------------------
// 
// NNS_G2dPopMtx() 内部で呼び出されます。
// NNS_G2dPopMtx() 後の カレント行列が SR affine 変換 されているか調査します。
// SR affine 変換 されていない状態と判定された場合は Renderer内部 の 状態を 更新します。
//
NNS_G2D_INLINE void CheckIfRndCurrentMtxIsSRTransformed_()
{
    if( NNSi_G2dIsRndCurrentMtxSRTransformed() )
    {
        //
        // 行列スタックの現在の位置を取得します
        // 現在の位置が最初に SR 変換された位置よりも 前ならば、
        // NNS_G2dPopMtx() 後の カレント行列 は  SR 変換されていないとみなせます
        // 
        const u16 currentIdx = NNSi_G2dGetMtxStackPos();
        {
            if( firstSRTransformedMtxIdx_ > currentIdx )
            {
                NNSI_G2D_DEBUGMSG1( "Affine SR == OFF : in stack_position %d \n", 
                                     currentIdx );
                                     
                RestoreRndMtxStackStateNotSR_();
            }
        }
    }
}


//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL NNSi_G2dIsStackEmpty()
{
    // return ( (u32)pMtxCurr <=  (u32)&mtxStack_[0] ) ? TRUE : FALSE;
    return (BOOL)(stackPos_ <= 0);
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL NNSi_G2dStackHasEnoughCapacity()
{
    // return ( (u32)(pMtxCurr + 1) < (u32)&mtxStack_[NNS_G2D_MAXLENGTH_OF_2DMTXSTACK] ) ? TRUE : FALSE;
    return (BOOL)(stackPos_ + 1 < NNS_G2D_MAXLENGTH_OF_2DMTXSTACK);
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNSi_G2dMtxPush()
{
    if( NNSi_G2dStackHasEnoughCapacity() )
    {
        // copy current.
        const int nextPos_ = stackPos_ + 1;
        
        if( bDonotUseSRTransform )
        {
            
            mtxStack_[nextPos_]._20 = mtxStack_[stackPos_]._20;
            mtxStack_[nextPos_]._21 = mtxStack_[stackPos_]._21;
            fxZStack_[nextPos_]     = fxZStack_[stackPos_];
            
        }else{
            
            mtxStack_[nextPos_]         = mtxStack_[stackPos_];
            mtxStackFor2DHW_[nextPos_]  = mtxStackFor2DHW_[stackPos_];
            fxZStack_[nextPos_]         = fxZStack_[stackPos_];
            
        }
        stackPos_ = nextPos_;
    }else{
        OS_Warning( "The mtxStack overflow in G2D library." );
    }
}
//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNSi_G2dMtxPop()
{
    if( !NNSi_G2dIsStackEmpty() )
    {
        stackPos_--;
        //
        // 行列スタックの現在の位置を取得します
        // 現在の位置が最初に SR 変換された位置よりも 前ならば、
        // NNS_G2dPopMtx() 後の カレント行列 は  SR 変換されていないとみなせます
        // 
        {
            if( firstSRTransformedMtxIdx_ > stackPos_ )
            {
                NNSI_G2D_DEBUGMSG1( "Affine SR == OFF : in stack_position %d \n", 
                                     stackPos_ );
                                     
                RestoreRndMtxStackStateNotSR_();
            }
        }
    }else{
        OS_Warning( "The mtxStack is empty, so we can't pop it any more" );
    } 
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNSi_G2dTranslate( fx32 x, fx32 y, fx32 z )
{
    if( bDonotUseSRTransform )
    {  
        mtxStack_[stackPos_]._20 += x;
        mtxStack_[stackPos_]._21 += y;
        fxZStack_[stackPos_] += z;
    }else{
        MtxFx32 temp;
 
        NNSi_G2dSetTranslate( &temp, x, y );
        NNSi_G2dMtxConcat32( &temp, &mtxStack_[stackPos_], &mtxStack_[stackPos_] );
        NNSi_G2dMtxConcat32( &temp, &mtxStackFor2DHW_[stackPos_], &mtxStackFor2DHW_[stackPos_] );
        
        fxZStack_[stackPos_] += z;
    }
    //NNSi_G2dDumpMtx32( pMtxCurr );
}

//------------------------------------------------------------------------------
// 平行移動を設定する
// push pop の 使用を回避することが可能となる
NNS_G2D_INLINE void NNSi_G2dSetTrans( fx32 x, fx32 y, fx32 z )
{
    mtxStack_[stackPos_]._20 = x;
    mtxStack_[stackPos_]._21 = y;
    if( !bDonotUseSRTransform )
    {  
        mtxStackFor2DHW_[stackPos_]._20 = x;
        mtxStackFor2DHW_[stackPos_]._21 = y;
    }
    
    fxZStack_[stackPos_] = z;
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNSi_G2dRotate( fx32 sin, fx32 cos )
{
    NNS_G2D_ASSERTMSG( !bDonotUseSRTransform, 
        "Renderer module is the -- DonotUseSRTransform -- Mode.You can't use SR Transfomation.");
        
    {
        MtxFx32 temp;
        NNSi_G2dSetRotate( &temp, sin, cos );
      
        NNSi_G2dMtxConcat32( &temp, &mtxStack_[stackPos_], &mtxStack_[stackPos_] );
        NNSi_G2dMtxConcat32( &temp, &mtxStackFor2DHW_[stackPos_], &mtxStackFor2DHW_[stackPos_] );
    }
}
//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNSi_G2dScale( fx32 sx, fx32 sy )
{
    NNS_G2D_ASSERTMSG( !bDonotUseSRTransform, 
        "Renderer module is the -- DonotUseSRTransform -- Mode.You can't use SR Transfomation.");
        
    {
        MtxFx32 temp;
        NNSi_G2dSetScale( &temp, sx, sy );
        NNSi_G2dMtxConcat32( &temp, &mtxStack_[stackPos_], &mtxStack_[stackPos_] );
        NNSi_G2dSetScaleFor2DHW( &temp, sx, sy );
        NNSi_G2dMtxConcat32( &temp, &mtxStackFor2DHW_[stackPos_], &mtxStackFor2DHW_[stackPos_] );
    }
}

//------------------------------------------------------------------------------
static void NNSi_G2dIdentity( )
{
    
    if( bDonotUseSRTransform )
    {
        mtxStack_[stackPos_]._20 = 0;
        mtxStack_[stackPos_]._21 = 0;
        
    }else{
        MtxFx32* pMtxCurr = &mtxStack_[stackPos_];
        
        pMtxCurr->_00 = FX32_ONE;
        pMtxCurr->_01 = 0;
        pMtxCurr->_10 = 0;
        pMtxCurr->_11 = FX32_ONE;
        pMtxCurr->_20 = 0;
        pMtxCurr->_21 = 0;
        
        mtxStackFor2DHW_[stackPos_] = *pMtxCurr;
    }
    
    fxZStack_[stackPos_] = 0;
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE const MtxFx32* NNSi_G2dGetCurrentMtx()
{
    if( bDonotUseSRTransform )
    {
        mtxI_._20 = mtxStack_[stackPos_]._20;
        mtxI_._21 = mtxStack_[stackPos_]._21;
        
        return &mtxI_;
        
    }else{
        return &mtxStack_[stackPos_];
    }
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE const MtxFx32* NNSi_G2dGetCurrentMtxFor2DHW()
{
    if( bDonotUseSRTransform )
    {
        mtxI_._20 = mtxStack_[stackPos_]._20;
        mtxI_._21 = mtxStack_[stackPos_]._21;
        
        return &mtxI_;
        
    }else{
        return &mtxStackFor2DHW_[stackPos_];
    }
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE fx32 NNSi_G2dGetCurrentZ()
{
    return fxZStack_[stackPos_];
}

//------------------------------------------------------------------------------
// 現在のMtxStackの位置を所得します
NNS_G2D_INLINE u16 NNSi_G2dGetMtxStackPos( )
{
    // return (u16)(( (u32)pMtxCurr - (u32)&mtxStack_[0] ) / sizeof( MtxFx32 ) );
    return (u16)( stackPos_ );
}







#endif // NNS_G2DI_RENDERERMTXSTACK_HPP_
