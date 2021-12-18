/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d - fmt
  File:     g2d_Oam_data.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.13 $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_OAM_DATA_H_
#define NNS_G2D_OAM_DATA_H_

#include <nitro/types.h>
#include <nnsys/g2d/g2d_config.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NNS_G2D_OAM_AFFINE_IDX_NONE                0xFFFE        // OAM マネージャに Affine を 使用しない OAMを登録するときに 指定するAffine Index 値

typedef struct objSize
{
    u16    x;
    u16    y;
}objSize;
/*
(shape & GX_OAM_ATTR01_SHAPE_MASK) >> GX_OAM_ATTR01_SHAPE_SHIFT
(shape & GX_OAM_ATTR01_SIZE_MASK) >> GX_OAM_ATTR01_SIZE_SHIFT
*/
/*
GX_OAM_SHAPE_8x8   = (0 << GX_OAM_ATTR01_SHAPE_SHIFT) | (0 << GX_OAM_ATTR01_SIZE_SHIFT),
GX_OAM_SHAPE_16x16 = (0 << GX_OAM_ATTR01_SHAPE_SHIFT) | (1 << GX_OAM_ATTR01_SIZE_SHIFT),
GX_OAM_SHAPE_32x32 = (0 << GX_OAM_ATTR01_SHAPE_SHIFT) | (2 << GX_OAM_ATTR01_SIZE_SHIFT),
GX_OAM_SHAPE_64x64 = (0 << GX_OAM_ATTR01_SHAPE_SHIFT) | (3 << GX_OAM_ATTR01_SIZE_SHIFT),
GX_OAM_SHAPE_16x8  = (1 << GX_OAM_ATTR01_SHAPE_SHIFT) | (0 << GX_OAM_ATTR01_SIZE_SHIFT),
GX_OAM_SHAPE_32x8  = (1 << GX_OAM_ATTR01_SHAPE_SHIFT) | (1 << GX_OAM_ATTR01_SIZE_SHIFT),
GX_OAM_SHAPE_32x16 = (1 << GX_OAM_ATTR01_SHAPE_SHIFT) | (2 << GX_OAM_ATTR01_SIZE_SHIFT),
GX_OAM_SHAPE_64x32 = (1 << GX_OAM_ATTR01_SHAPE_SHIFT) | (3 << GX_OAM_ATTR01_SIZE_SHIFT),
GX_OAM_SHAPE_8x16  = (2 << GX_OAM_ATTR01_SHAPE_SHIFT) | (0 << GX_OAM_ATTR01_SIZE_SHIFT),
GX_OAM_SHAPE_8x32  = (2 << GX_OAM_ATTR01_SHAPE_SHIFT) | (1 << GX_OAM_ATTR01_SIZE_SHIFT),
GX_OAM_SHAPE_16x32 = (2 << GX_OAM_ATTR01_SHAPE_SHIFT) | (2 << GX_OAM_ATTR01_SIZE_SHIFT),
GX_OAM_SHAPE_32x64 = (2 << GX_OAM_ATTR01_SHAPE_SHIFT) | (3 << GX_OAM_ATTR01_SIZE_SHIFT)
*/

//
// ツールから参照する場合は、ヘッダで定義されるインライン関数内で宣言します。
//
#ifdef NNS_FROM_TOOL
    #define NNS_G2D_OBJSIZE_TBL_STATIC static 
#else // NNS_FROM_TOOL
    #define NNS_G2D_OBJSIZE_TBL_STATIC
    extern const u16 NNSi_objSizeHTbl[3][4];
    extern const u16 NNSi_objSizeWTbl[3][4];
#endif // NNS_FROM_TOOL


#define NNS_G2D_DEFINE_NNSI_OBJSIZEWTBL        \
NNS_G2D_OBJSIZE_TBL_STATIC                     \
const u16 NNSi_objSizeWTbl[3][4]=               \
{                                              \
    {                                          \
        8,                                     \
        16,                                    \
        32,                                    \
        64                                     \
    },                                         \
    {                                          \
        16,                                    \
        32,                                    \
        32,                                    \
        64                                     \
    },                                         \
    {                                          \
        8,                                     \
        8,                                     \
        16,                                    \
        32                                     \
    }                                          \
}                                              \


#define NNS_G2D_DEFINE_NNSI_OBJSIZEHTBL        \
NNS_G2D_OBJSIZE_TBL_STATIC                     \
const u16 NNSi_objSizeHTbl[3][4]=               \
{                                              \
    {                                          \
        8,                                     \
        16,                                    \
        32,                                    \
        64,                                    \
    },                                         \
    {                                          \
        8,                                     \
        8,                                     \
        16,                                    \
        32,                                    \
    },                                         \
    {                                          \
        16,                                    \
        32,                                    \
        32,                                    \
        64                                     \
    }                                          \
}                                              \


//------------------------------------------------------------------------------
// g2di_OamUtil.hより移動
NNS_G2D_INLINE GXOamShape NNS_G2dGetOAMSize( const GXOamAttr* oamAttr )
{
    //const GXOamShape  result = (GXOamShape)( oamAttr->shape );
    const GXOamShape  result = (GXOamShape)((GX_OAM_ATTR01_SHAPE_MASK | GX_OAM_ATTR01_SIZE_MASK) & oamAttr->attr01);
    
    // GX_OAM_SHAPE_ASSERT(result);
    return result;
}

//------------------------------------------------------------------------------
// g2di_OamUtil.hより移動
NNS_G2D_INLINE int NNS_G2dGetOamSizeX( const GXOamShape* oamShape )
{
    // NNS_G2D_NULL_ASSERT(oamShape);
#ifdef NNS_FROM_TOOL
    NNS_G2D_DEFINE_NNSI_OBJSIZEWTBL;
#endif // NNS_FROM_TOOL
    return NNSi_objSizeWTbl[(*oamShape & GX_OAM_ATTR01_SHAPE_MASK) >> GX_OAM_ATTR01_SHAPE_SHIFT]
                           [(*oamShape & GX_OAM_ATTR01_SIZE_MASK) >> GX_OAM_ATTR01_SIZE_SHIFT];
}

//------------------------------------------------------------------------------
// g2di_OamUtil.hより移動
NNS_G2D_INLINE int NNS_G2dGetOamSizeY( const GXOamShape* oamShape )
{
#ifdef NNS_FROM_TOOL
    NNS_G2D_DEFINE_NNSI_OBJSIZEHTBL;
#endif // NNS_FROM_TOOL
    // NNS_G2D_NULL_ASSERT(oamShape);
    return NNSi_objSizeHTbl[(*oamShape & GX_OAM_ATTR01_SHAPE_MASK) >> GX_OAM_ATTR01_SHAPE_SHIFT]
                           [(*oamShape & GX_OAM_ATTR01_SIZE_MASK) >> GX_OAM_ATTR01_SIZE_SHIFT];
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_G2D_OAM_DATA_H_

