/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d - fmt
  File:     g2d_Vec_data.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.4 $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_VEC_DATA_H_
#define NNS_G2D_VEC_DATA_H_

#include <nitro/types.h>


#ifdef __cplusplus
extern "C" {
#endif


//------------------------------------------------------------------------------
typedef struct NNSG2dSVec2
{
    s16     x;        
    s16     y;
    
}NNSG2dSVec2;

//------------------------------------------------------------------------------
typedef struct NNSG2dFVec2
{
    fx32     x;        
    fx32     y;
    
}NNSG2dFVec2;

//------------------------------------------------------------------------------
#ifdef SDK_ADS
typedef struct
{
    fx32 _00, _01;
    fx32 _10, _11;
    fx32 _20, _21;
    
}
MtxFx32;
#else
typedef union
{
    struct
    {
        fx32 _00, _01;
        fx32 _10, _11;
        fx32 _20, _21;
    };
    fx32 m[3][2];
    fx32 a[6];
}
MtxFx32;
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_G2D_VEC_DATA_H_

