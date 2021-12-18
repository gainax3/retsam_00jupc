/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d - fmt
  File:     g2d_SRTControl_data.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.6 $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_SRTCONTROL_DATA_H_
#define NNS_G2D_SRTCONTROL_DATA_H_

#include <nitro/types.h>
#include <nnsys/g2d/fmt/g2d_Vec_data.h>


#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
typedef enum
{
    NNS_G2D_SRTCONTROLTYPE_INVALID,
    NNS_G2D_SRTCONTROLTYPE_SRT,
    NNS_G2D_SRTCONTROLTYPE_MTX2D,
    NNS_G2D_SRTCONTROLTYPE_MTX3D,   
    NNS_G2D_SRTCONTROLTYPE_MAX
    
}NNSG2dSRTControlType;

typedef enum NNSG2dAffineEnable
{
    NNS_G2D_AFFINEENABLE_NONE   = 0x00,
    NNS_G2D_AFFINEENABLE_SCALE  = 0x02,
    NNS_G2D_AFFINEENABLE_ROTATE = 0x04,
    NNS_G2D_AFFINEENABLE_TRANS  = 0x08,
    NNS_G2D_AFFINEENABLE_MAX    = 0x10
    
}NNSG2dAffineEnable;

typedef union
{
    struct 
    {
        NNSG2dFVec2     scale;
        NNSG2dSVec2     trans;
        u16             rotZ;
        u16             SRT_EnableFlag;
    };
    
    MtxFx32             mtx;
    
}NNSG2dSRTData;
    
//------------------------------------------------------------------------------
typedef struct
{
    NNSG2dSRTControlType      type;
    NNSG2dSRTData             srtData;
    
}NNSG2dSRTControl;


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_G2D_SRTCONTROL_DATA_H_

