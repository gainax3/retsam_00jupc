/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d
  File:     g2d_OAM_Types.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.3 $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_OAM_TYPES_H_
#define NNS_G2D_OAM_TYPES_H_
#include <nitro.h>

#ifdef __cplusplus
extern "C" {
#endif


//
// NOTICE ! 
// OAM's basic params and affine params are separated for simplicity. 
// This is a different from HW's data structure.  
//
//
// Affine params
// 
typedef struct NNSG2dOamAffineParams
{
    s16         PA;
    s16         PB;
    s16         PC;
    s16         PD;         
}
NNSG2dOamAffineParams;

//
// TODO: merge with NOBOamDataChunk ( both are same completely. )
//
typedef struct NNSG2dOamDataChunk
{
    u16         attr0;
    u16         attr1;
    u16         attr2;
}
NNSG2dOamDataChunk;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_G2D_OAM_TYPES_H_

