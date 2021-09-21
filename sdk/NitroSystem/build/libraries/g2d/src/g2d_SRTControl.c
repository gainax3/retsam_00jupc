/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d
  File:     g2d_SRTControl.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.8 $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nnsys/g2d/g2d_SRTControl.h>


#define ASSERT_SRTControlType_VALID( val ) \
        NNS_G2D_MINMAX_ASSERT( (val), NNS_G2D_SRTCONTROLTYPE_SRT, NNS_G2D_SRTCONTROLTYPE_MTX3D )

#define ASSERT_SRT_MODE( val )  NNS_G2D_ASSERTMSG( ( val ) == NNS_G2D_SRTCONTROLTYPE_SRT, \
                  "You can use the function only if its NNSG2dSRTControlType is NNS_G2D_SRTCONTROLTYPE_SRT" );

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dSrtcSetTrans TODO:

  Description:  NNSG2dSRTControl の平行移動値を設定します
                
                
  Arguments:    pCtrl:         NNSG2dSRTControl
                    x:         平行移動値 x
                    y:         平行移動値 y
                    
                    NNSG2dSRTControlType が 
                    NNS_G2D_SRTCONTROLTYPE_SRTである必要があります
                    
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNSi_G2dSrtcSetTrans( NNSG2dSRTControl* pCtrl, s16 x, s16 y )
{
    
    
    if( pCtrl->type == NNS_G2D_SRTCONTROLTYPE_SRT )
    {
        NNSi_G2dSrtcAffineFlagON( pCtrl, NNS_G2D_AFFINEENABLE_TRANS );
        
        pCtrl->srtData.trans.x = x;
        pCtrl->srtData.trans.y = y;
    }else{
        NNS_G2D_ASSERTMSG( FALSE, "NOT implemented, avoid calling me");
    }
}




/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dSrtcSetSRTRotZ TODO:

  Description:  NNSG2dSRTControl の回転値を設定します
                
                
  Arguments:    pCtrl:         NNSG2dSRTControl
                 rotZ:         Z軸回転
                    
                    NNSG2dSRTControlType が 
                    NNS_G2D_SRTCONTROLTYPE_SRTである必要があります
                    
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNSi_G2dSrtcSetSRTRotZ( NNSG2dSRTControl* pCtrl, u16 rotZ )
{
    if( pCtrl->type == NNS_G2D_SRTCONTROLTYPE_SRT )
    {
        NNSi_G2dSrtcAffineFlagON( pCtrl, NNS_G2D_AFFINEENABLE_ROTATE );
        
        pCtrl->srtData.rotZ = rotZ;
    }else{
        NNS_G2D_ASSERTMSG( FALSE, "NOT implemented, avoid calling me");
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dSrtcSetSRTScale TODO:

  Description:  NNSG2dSRTControl のスケール値を設定します
                
                
  Arguments:    pCtrl:         NNSG2dSRTControl
                    x:         スケール値 x
                    y:         スケール値 y
                    
                    NNSG2dSRTControlType が 
                    NNS_G2D_SRTCONTROLTYPE_SRTである必要があります
                    
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNSi_G2dSrtcSetSRTScale( NNSG2dSRTControl* pCtrl, fx32 x, fx32 y )
{
    if( pCtrl->type == NNS_G2D_SRTCONTROLTYPE_SRT )
    {
        NNSi_G2dSrtcAffineFlagON( pCtrl, NNS_G2D_AFFINEENABLE_SCALE );
        
        pCtrl->srtData.scale.x = x;
        pCtrl->srtData.scale.y = y;
    }else{
        NNS_G2D_ASSERTMSG( FALSE, "NOT implemented, avoid calling me");
    }
    
}


/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dSrtcInitControl TODO:

  Description:  NNSG2dSRTControl を初期化します
                
                
  Arguments:    pCtrl:         NNSG2dSRTControl
                 type:         NNSG2dSRTControlのデータフォーマット
                    
                    
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNSi_G2dSrtcInitControl( NNSG2dSRTControl* pCtrl, NNSG2dSRTControlType type )
{
    NNS_G2D_NULL_ASSERT( pCtrl );
    ASSERT_SRTControlType_VALID( type );
    
    pCtrl->type = type;
    
    NNSi_G2dSrtcSetInitialValue( pCtrl );
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dSrtcSetInitialValue TODO:

  Description:  NNSG2dSRTControl に初期値を設定します
                
                
  Arguments:    pCtrl:         NNSG2dSRTControl
                    
                    NNSG2dSRTControlType が 
                    NNS_G2D_SRTCONTROLTYPE_SRTである必要があります
                    
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNSi_G2dSrtcSetInitialValue    ( NNSG2dSRTControl* pCtrl )
{
    NNS_G2D_NULL_ASSERT( pCtrl );
    ASSERT_SRT_MODE( pCtrl->type ); 
    
    MI_CpuFill16( &pCtrl->srtData, 0, sizeof( NNSG2dSRTData ) );
    
    pCtrl->srtData.scale.x = FX32_ONE;
    pCtrl->srtData.scale.y = FX32_ONE;
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dSrtcBuildMatrixFromSRT_2D TODO:

  Description:  NNSG2dSRTControl の SR 情報を 2D Graphics Engine 用行列として算出します
                
                
  Arguments:    pCtrl:         NNSG2dSRTControl
                 pDst:         出力行列
                    
                    NNSG2dSRTControlType が 
                    NNS_G2D_SRTCONTROLTYPE_SRTである必要があります
                    
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNSi_G2dSrtcBuildMatrixFromSRT_2D( const NNSG2dSRTControl* pCtrl , MtxFx22* pDst )
{
#pragma unused( pCtrl )
#pragma unused( pDst )
    NNS_G2D_NULL_ASSERT( pCtrl );
    NNS_G2D_NULL_ASSERT( pDst );
    ASSERT_SRT_MODE( pCtrl->type ); 
    
    MTX_Identity22( pDst );
    
    // r
    MTX_Rot22( pDst, FX_SinIdx( pCtrl->srtData.rotZ ), FX_CosIdx( pCtrl->srtData.rotZ ) );
    // s 
    MTX_ScaleApply22( pDst, pDst, pCtrl->srtData.scale.x, pCtrl->srtData.scale.y );
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dSrtcBuildMatrixFromSRT_3D TODO:

  Description:  NNSG2dSRTControl の SR 情報を 3D Graphics Engine 用に算出します
                
                
  Arguments:    pCtrl:         NNSG2dSRTControl
                 pDst:         出力行列
                    
                    NNSG2dSRTControlType が 
                    NNS_G2D_SRTCONTROLTYPE_SRTである必要があります
                    
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNSi_G2dSrtcBuildMatrixFromSRT_3D( const NNSG2dSRTControl* pCtrl , MtxFx22* pDst )
{
#pragma unused( pCtrl )
#pragma unused( pDst )
    NNS_G2D_NULL_ASSERT( pCtrl );
    NNS_G2D_NULL_ASSERT( pDst );
    ASSERT_SRT_MODE( pCtrl->type );       
    
    // r
    MTX_Rot22( pDst, FX_SinIdx( pCtrl->srtData.rotZ ), FX_CosIdx( pCtrl->srtData.rotZ ) );
    // s  
    MTX_ScaleApply22( pDst, pDst, pCtrl->srtData.scale.x, pCtrl->srtData.scale.y );
}

