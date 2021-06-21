/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - 
  File:     fx_mtx33.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: fx_mtx33.c,v $
  Revision 1.24  2006/01/18 02:11:20  kitase_hirotake
  do-indent

  Revision 1.23  2005/03/01 01:57:00  yosizaki
  copyright ‚Ì”N‚ðC³.

  Revision 1.22  2005/02/28 05:26:13  yosizaki
  do-indent.

  Revision 1.21  2004/02/18 09:01:23  nishida_kenji
  Add comments for code32.h.

  Revision 1.20  2004/02/16 00:51:11  nishida_kenji
  Revise MTX_Inverse33.

  Revision 1.19  2004/02/12 01:35:50  nishida_kenji
  Work around to optimization of simple matrix functions.

  Revision 1.18  2004/02/10 06:31:56  nishida_kenji
  Work around to optimization of simple matrix functions.

  Revision 1.17  2004/02/06 04:50:54  nishida_kenji
  MI_CpuCopyFast for block copy.

  Revision 1.16  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.15  2004/02/03 06:43:03  nishida_kenji
  Add comments.

  Revision 1.14  2004/01/27 00:41:21  nishida_kenji
  small fix and comments

  Revision 1.13  2004/01/26 07:20:45  nishida_kenji
  include code32.h

  Revision 1.11  2004/01/17 07:06:57  nishida_kenji
  add MTX_MultVec33

  Revision 1.10  2004/01/09 09:27:28  nishida_kenji
  fix comments

  Revision 1.9  2003/12/25 00:19:29  nishida_kenji
  convert INLINE to inline

  Revision 1.8  2003/12/11 06:12:39  nishida_kenji
  add MTX_Concat

  Revision 1.7  2003/12/09 11:43:10  nishida_kenji
  improve asserts

  Revision 1.6  2003/12/04 07:32:09  nishida_kenji
  MTX_Inverse33, MTX_Inverse43

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>              // Always generate ARM binary for efficiency

#include <nitro/fx/fx_mtx33.h>
#include <nitro/fx/fx_const.h>
#include <nitro/fx/fx_cp.h>

inline fx32 mul64(fx64 x, fx32 y)
{
    return (fx32)((x * y) >> FX32_SHIFT);
}

asm void MTX_Identity33_(register MtxFx33* pDst)
{
    mov  r2, #4096                     // FX32_ONE
    str  r2,  [r0, #32]
    mov  r3, #0
    stmia r0!, {r2, r3}
    mov   r1, #0
    stmia r0!, {r1, r3}
    stmia r0!, {r2, r3}
    stmia r0!, {r1, r3}
    bx   lr
}

asm void MTX_Copy33To43_(const register MtxFx33* pSrc, register MtxFx43* pDst)
{
    ldmia r0!, {r2-r3, r12}
    stmia r1!, {r2-r3, r12}
    ldmia r0!, {r2-r3, r12}
    stmia r1!, {r2-r3, r12}
    ldmia r0!, {r2-r3, r12}
    stmia r1!, {r2-r3, r12}

    mov  r2, #0
    str  r2, [r1, #0]
    str  r2, [r1, #4]
    str  r2, [r1, #8]

    bx    lr
}


asm void MTX_Copy33To44_(const register MtxFx33* pSrc, register MtxFx44* pDst)
{
    stmfd sp!, {r4}
    
    mov   r12, #0
    ldmia r0!, {r2-r4}
    stmia r1!, {r2-r4, r12}
    ldmia r0!, {r2-r4}
    stmia r1!, {r2-r4, r12}
    ldmia r0!, {r2-r4}
    stmia r1!, {r2-r4, r12}

    mov   r2,  #4096                    // FX32_ONE
    str   r12, [r1, #0]
    str   r12, [r1, #4]
    str   r12, [r1, #8]
    str   r2,  [r1, #12]
    
    ldmfd sp!, {r4}
    bx    lr
}


asm void MTX_Transpose33_(const register MtxFx33* pSrc, register MtxFx33* pDst)
{
    stmfd sp!, {r4-r9}

    ldmia r0, {r2-r9, r12}
    stmia r1!, {r2, r5, r8}
    stmia r1!, {r3, r6, r9}
    stmia r1!, {r4, r7, r12}

    ldmfd sp!, {r4-r9}
    bx    lr
}

#include <nitro/code16.h>
asm void MTX_Scale33_(register MtxFx33* pDst, register fx32 x, register fx32 y, register fx32 z)
{
    stmia r0!, {r1}
    mov r1, #0
    str r3, [r0, #28]
    mov r3, #0
    stmia r0!, {r1, r3}
    stmia r0!, {r1, r2, r3}
    mov r2, #0
    stmia r0!, {r1, r3}

    bx lr
}
#include <nitro/code32.h>

/*---------------------------------------------------------------------------*
  Name:         MTX_ScaleApply33

  Description:  This function performs the operation equivalent to
                MTX_Scale33 + MTX_Concat33.

  Arguments:    pSrc         a pointer to a 3x3 matrix
                pDst         a pointer to a 3x3 matrix
                x            x scale factor
                y            y scale factor
                z            z scale factor
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void MTX_ScaleApply33(const MtxFx33 *pSrc, MtxFx33 *pDst, fx32 x, fx32 y, fx32 z)
{
    fx64    v;

    SDK_NULL_ASSERT(pSrc);
    SDK_NULL_ASSERT(pDst);

    v = (fx64)x;
    pDst->_00 = mul64(v, pSrc->_00);
    pDst->_01 = mul64(v, pSrc->_01);
    pDst->_02 = mul64(v, pSrc->_02);

    v = (fx64)y;
    pDst->_10 = mul64(v, pSrc->_10);
    pDst->_11 = mul64(v, pSrc->_11);
    pDst->_12 = mul64(v, pSrc->_12);

    v = (fx64)z;
    pDst->_20 = mul64(v, pSrc->_20);
    pDst->_21 = mul64(v, pSrc->_21);
    pDst->_22 = mul64(v, pSrc->_22);
}

#include <nitro/code16.h>
asm void MTX_RotX33_(register MtxFx33 * pDst, register fx32 sinVal, register fx32 cosVal)
{
    mov r3, #1
    lsl r3, r3, #12
    str r3, [r0, #0]
    mov r3, #0
    str r3, [r0, #4]
    str r3, [r0, #8]
    str r3, [r0, #12]
    str r2, [r0, #16]
    str r1, [r0, #20]
    str r3, [r0, #24]
    neg r1, r1
    str r1, [r0, #28]
    str r2, [r0, #32]

    bx  lr
}
#include <nitro/code32.h>

#include <nitro/code16.h>
asm void MTX_RotY33_(register MtxFx33 * pDst, register fx32 sinVal, register fx32 cosVal)
{
    str r2, [r0, #0]
    str r2, [r0, #32]
    mov r3, #0
    str r3, [r0, #4]
    str r3, [r0, #12]
    str r3, [r0, #20]
    str r3, [r0, #28]
    neg r2, r1
    mov r3, #1
    lsl r3, r3, #12
    str r1, [r0, #24]
    str r2, [r0, #8]
    str r3, [r0, #16]

    bx  lr
}
#include <nitro/code32.h>

#include <nitro/code16.h>
asm void MTX_RotZ33_(register MtxFx33 * pDst, register fx32 sinVal, register fx32 cosVal)
{
    stmia r0!, {r2}
    mov   r3, #0
    stmia r0!, {r1, r3}
    neg   r1, r1
    stmia r0!, {r1, r2}
    mov   r1, #1
    lsl   r1, r1, #12
    str   r3, [r0, #0]
    str   r3, [r0, #4]
    str   r3, [r0, #8]
    str   r1, [r0, #12]

    bx    lr
}
#include <nitro/code32.h>

/*---------------------------------------------------------------------------*
  Name:         MTX_RotAxis33

  Description:  Sets a rotation matrix about an arbitrary axis

  Arguments:    pDst         a pointer to a 3x3 matrix
                vec          a pointer to a vector containing the x,y,z axis
                             components. *vec must be a unit vector.
                sinVal       sine of an angle of rotation
                cosVal       cosine of an angle of rotation
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void MTX_RotAxis33(MtxFx33 *pDst, const VecFx32 *vec, fx32 sinVal, fx32 cosVal)
{
    fx64    t, ss, cc, xx, yy, zz;
    fx32    t01;
    fx32    s2;
    SDK_NULL_ASSERT(pDst);
    SDK_NULL_ASSERT(vec);

    t = (fx64)(FX32_ONE - (fx32)cosVal);
    ss = (fx64)sinVal;
    cc = (fx64)cosVal;
    xx = (fx64)vec->x;
    yy = (fx64)vec->y;
    zz = (fx64)vec->z;

    t01 = (fx32)((xx * xx * t) >> (FX64_SHIFT + FX64_SHIFT));
    pDst->_00 = t01 + cosVal;

    t01 = (fx32)((yy * yy * t) >> (FX64_SHIFT + FX64_SHIFT));
    pDst->_11 = t01 + cosVal;

    t01 = (fx32)((zz * zz * t) >> (FX64_SHIFT + FX64_SHIFT));
    pDst->_22 = t01 + cosVal;

    t01 = (fx32)((t * xx * yy) >> (FX64_SHIFT + FX64_SHIFT));
    s2 = (fx32)((ss * zz) >> FX64_SHIFT);
    pDst->_01 = t01 + s2;
    pDst->_10 = t01 - s2;

    t01 = (fx32)((t * xx * zz) >> (FX64_SHIFT + FX64_SHIFT));
    s2 = (fx32)((ss * yy) >> FX64_SHIFT);
    pDst->_02 = t01 - s2;
    pDst->_20 = t01 + s2;

    t01 = (fx32)((t * yy * zz) >> (FX64_SHIFT + FX64_SHIFT));
    s2 = (fx32)((ss * xx) >> FX64_SHIFT);
    pDst->_12 = t01 + s2;
    pDst->_21 = t01 - s2;
}


/*---------------------------------------------------------------------------*
  Name:         MTX_Inverse33

  Description:  Computes an inverse of a 3x3 matrix.

  Arguments:    pSrc         a pointer to a 3x3 matrix
                pDst         a pointer to a 3x3 matrix
  
  Returns:      -1 if *pSrc is not invertible.
                0 if success.
 *---------------------------------------------------------------------------*/
int MTX_Inverse33(const MtxFx33 *pSrc, MtxFx33 *pDst)
{
    MtxFx33 tmp;
    MtxFx33 *p;
    fx32    det;
    fx32    det00, det10, det20;
    fx32    tmp01, tmp02, tmp11, tmp12;
    fx32    tmp21, tmp22;

    SDK_NULL_ASSERT(pSrc);
    SDK_NULL_ASSERT(pDst);

    if (pSrc == pDst)
    {
        p = &tmp;
    }
    else
    {
        p = pDst;
    }

    // Computes the sub-determinant
    det00 = (fx32)(((fx64)pSrc->_11 * pSrc->_22 -
                    (fx64)pSrc->_12 * pSrc->_21 + (fx64)(FX32_ONE >> 1)) >> FX32_SHIFT);
    det10 = (fx32)(((fx64)pSrc->_10 * pSrc->_22 -
                    (fx64)pSrc->_12 * pSrc->_20 + (fx64)(FX32_ONE >> 1)) >> FX32_SHIFT);
    det20 = (fx32)(((fx64)pSrc->_10 * pSrc->_21 -
                    (fx64)pSrc->_11 * pSrc->_20 + (fx64)(FX32_ONE >> 1)) >> FX32_SHIFT);

    // Comptutes the determinant
    det = (fx32)(((fx64)pSrc->_00 * det00 -
                  (fx64)pSrc->_01 * det10 +
                  (fx64)pSrc->_02 * det20 + (fx64)(FX32_ONE >> 1)) >> FX32_SHIFT);
    if (0 == det)
    {
        return -1;
    }

    FX_InvAsync(det);

    tmp01 = (fx32)(((fx64)pSrc->_01 * pSrc->_22 - (fx64)pSrc->_21 * pSrc->_02) >> FX32_SHIFT);
    tmp02 = (fx32)(((fx64)pSrc->_01 * pSrc->_12 - (fx64)pSrc->_11 * pSrc->_02) >> FX32_SHIFT);
    tmp11 = (fx32)(((fx64)pSrc->_00 * pSrc->_22 - (fx64)pSrc->_20 * pSrc->_02) >> FX32_SHIFT);
    tmp12 = (fx32)(((fx64)pSrc->_00 * pSrc->_12 - (fx64)pSrc->_10 * pSrc->_02) >> FX32_SHIFT);

    det = FX_GetDivResult();
    p->_00 = (fx32)(((fx64)det * det00) >> FX32_SHIFT);
    p->_01 = -(fx32)(((fx64)det * tmp01) >> FX32_SHIFT);
    p->_02 = (fx32)(((fx64)det * tmp02) >> FX32_SHIFT);

    p->_10 = -(fx32)(((fx64)det * det10) >> FX32_SHIFT);
    p->_11 = (fx32)(((fx64)det * tmp11) >> FX32_SHIFT);
    p->_12 = -(fx32)(((fx64)det * tmp12) >> FX32_SHIFT);

    p->_20 = (fx32)(((fx64)det * det20) >> FX32_SHIFT);

    tmp21 = (fx32)(((fx64)pSrc->_00 * pSrc->_21 - (fx64)pSrc->_20 * pSrc->_01) >> FX32_SHIFT);
    p->_21 = -(fx32)(((fx64)det * tmp21) >> FX32_SHIFT);

    tmp22 = (fx32)(((fx64)pSrc->_00 * pSrc->_11 - (fx64)pSrc->_10 * pSrc->_01) >> FX32_SHIFT);
    p->_22 = (fx32)(((fx64)det * tmp22) >> FX32_SHIFT);

    if (p == &tmp)
    {
        MI_Copy36B(&tmp, pDst);
    }
    return 0;
}


/*---------------------------------------------------------------------------*
  Name:         MTX_Concat33

  Description:  Concatenates two 3x3 matrices.
                The order of operation is A x B = AB.
                OK for any of ab == a == b.

  Arguments:    a            a pointer to a 3x3 matrix
                b            a pointer to a 3x3 matrix
                ab           a pointer to the resultant matrix
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void MTX_Concat33(const MtxFx33 *a, const MtxFx33 *b, MtxFx33 *ab)
{
    MtxFx33 tmp;
    MtxFx33 *p;

    register fx32 x, y, z;
    register fx32 xx, yy, zz;

    SDK_NULL_ASSERT(a);
    SDK_NULL_ASSERT(b);
    SDK_NULL_ASSERT(ab);

    if (ab == b)
    {
        p = &tmp;
    }
    else
    {
        p = ab;
    }

    // compute (a x b) -> p

    // row 0
    x = a->_00;
    y = a->_01;
    z = a->_02;

    p->_00 = (fx32)(((fx64)x * b->_00 + (fx64)y * b->_10 + (fx64)z * b->_20) >> FX32_SHIFT);
    p->_01 = (fx32)(((fx64)x * b->_01 + (fx64)y * b->_11 + (fx64)z * b->_21) >> FX32_SHIFT);

    xx = b->_02;
    yy = b->_12;
    zz = b->_22;

    p->_02 = (fx32)(((fx64)x * xx + (fx64)y * yy + (fx64)z * zz) >> FX32_SHIFT);

    // row 1
    x = a->_10;
    y = a->_11;
    z = a->_12;

    p->_12 = (fx32)(((fx64)x * xx + (fx64)y * yy + (fx64)z * zz) >> FX32_SHIFT);
    p->_11 = (fx32)(((fx64)x * b->_01 + (fx64)y * b->_11 + (fx64)z * b->_21) >> FX32_SHIFT);

    xx = b->_00;
    yy = b->_10;
    zz = b->_20;

    p->_10 = (fx32)(((fx64)x * xx + (fx64)y * yy + (fx64)z * zz) >> FX32_SHIFT);

    // row 2
    x = a->_20;
    y = a->_21;
    z = a->_22;

    p->_20 = (fx32)(((fx64)x * xx + (fx64)y * yy + (fx64)z * zz) >> FX32_SHIFT);
    p->_21 = (fx32)(((fx64)x * b->_01 + (fx64)y * b->_11 + (fx64)z * b->_21) >> FX32_SHIFT);
    p->_22 = (fx32)(((fx64)x * b->_02 + (fx64)y * b->_12 + (fx64)z * b->_22) >> FX32_SHIFT);

    if (p == &tmp)
    {
        *ab = tmp;
    }
}


/*---------------------------------------------------------------------------*
  Name:         MTX_MultVec33

  Description:  Multiplies a vector by a 3x3 matrix.
                vec x m = dst

  Arguments:    vec          a pointer to the source vector to multiply
                m            a pointer to a 3x3 matrix
                dst          a pointer to the result vector
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void MTX_MultVec33(const VecFx32 *vec, const MtxFx33 *m, VecFx32 *dst)
{
    register fx32 x, y, z;
    SDK_NULL_ASSERT(vec);
    SDK_NULL_ASSERT(m);
    SDK_NULL_ASSERT(dst);

    x = vec->x;
    y = vec->y;
    z = vec->z;

    dst->x = (fx32)(((fx64)x * m->_00 + (fx64)y * m->_10 + (fx64)z * m->_20) >> FX32_SHIFT);
    dst->y = (fx32)(((fx64)x * m->_01 + (fx64)y * m->_11 + (fx64)z * m->_21) >> FX32_SHIFT);
    dst->z = (fx32)(((fx64)x * m->_02 + (fx64)y * m->_12 + (fx64)z * m->_22) >> FX32_SHIFT);
}

#include <nitro/codereset.h>
