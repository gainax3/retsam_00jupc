/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - 
  File:     fx_mtx44.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: fx_mtx44.c,v $
  Revision 1.27  2007/04/24 11:17:52  yosizaki
  optimize MTX_PerspectiveW.

  Revision 1.26  2007/04/24 04:27:06  yosizaki
  add instances of projection-matrix functions.

  Revision 1.25  2006/01/18 02:11:20  kitase_hirotake
  do-indent

  Revision 1.24  2005/03/01 01:57:00  yosizaki
  copyright ‚Ì”N‚ðC³.

  Revision 1.23  2005/02/28 05:26:13  yosizaki
  do-indent.

  Revision 1.22  2004/11/02 17:57:55  takano_makoto
  fix comments.

  Revision 1.21  2004/06/21 10:23:55  takano_makoto
  move MTX_Frustum, MTX_Perspective, MTX_Ortho to inline function.

  Revision 1.20  2004/04/27 01:20:22  takano_makoto
  Modify MTX_Frustum, MTX_Perspective, MTX_Ortho.

  Revision 1.19  2004/04/27 01:16:16  takano_makoto
  Add MTX_FrustumW, MTX_PerspectiveW, MTX_OrthoW.

  Revision 1.18  2004/03/09 06:49:41  takano_makoto
  Bug fix in MTX_Concat44.

  Revision 1.17  2004/02/18 09:01:18  nishida_kenji
  Add comments for code32.h.

  Revision 1.16  2004/02/12 01:35:50  nishida_kenji
  Work around to optimization of simple matrix functions.

  Revision 1.15  2004/02/10 08:38:53  nishida_kenji
  Work around to optimization of simple matrix functions.

  Revision 1.14  2004/02/10 06:04:11  nishida_kenji
  Add MTX_TransApply44.

  Revision 1.13  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.12  2004/02/03 06:43:03  nishida_kenji
  Add comments.

  Revision 1.11  2004/01/27 00:41:21  nishida_kenji
  small fix and comments

  Revision 1.10  2004/01/26 07:20:45  nishida_kenji
  include code32.h

  Revision 1.8  2004/01/09 09:27:28  nishida_kenji
  fix comments

  Revision 1.7  2003/12/25 00:19:29  nishida_kenji
  convert INLINE to inline

  Revision 1.6  2003/12/12 08:08:34  nishida_kenji
  bug fix in MTX_Concat44

  Revision 1.5  2003/12/11 06:12:39  nishida_kenji
  add MTX_Concat

  Revision 1.4  2003/12/09 11:43:10  nishida_kenji
  improve asserts

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/code32.h>              // Always generate ARM binary for efficiency

#include <nitro/fx/fx_mtx44.h>
#include <nitro/fx/fx_vec.h>
#include <nitro/fx/fx_cp.h>
#include <nitro/mi/memory.h>

inline fx32 mul64(fx64 x, fx32 y)
{
    return (fx32)((x * y) >> FX32_SHIFT);
}

asm void MTX_Identity44_(register MtxFx44* pDst)
{
    mov   r2, #4096                    // FX32_ONE
    mov   r3, #0
    stmia r0!, {r2,r3}
    mov   r1, #0
    stmia r0!, {r1,r3}
    stmia r0!, {r1,r2,r3}
    stmia r0!, {r1,r3}
    stmia r0!, {r1,r2,r3}
    stmia r0!, {r1,r3}
    stmia r0!, {r1,r2}

    bx    lr
}

asm void MTX_Copy44To33_(register const MtxFx44* pSrc, register MtxFx33* pDst)
{
    ldmia r0!, {r2-r3, r12}
    add   r0, r0, #4
    stmia r1!, {r2-r3, r12}

    ldmia r0!, {r2-r3, r12}
    add   r0, r0, #4
    stmia r1!, {r2-r3, r12}

    ldmia r0!, {r2-r3, r12}
    add   r0, r0, #4
    stmia r1!, {r2-r3, r12}
    
    bx    lr
}

asm void MTX_Copy44To43_(register const MtxFx44* pSrc, register MtxFx43* pDst)
{
    ldmia r0!, {r2-r3, r12}
    add   r0, r0, #4
    stmia r1!, {r2-r3, r12}

    ldmia r0!, {r2-r3, r12}
    add   r0, r0, #4
    stmia r1!, {r2-r3, r12}

    ldmia r0!, {r2-r3, r12}
    add   r0, r0, #4
    stmia r1!, {r2-r3, r12}

    ldmia r0!, {r2-r3, r12}
    add   r0, r0, #4
    stmia r1!, {r2-r3, r12}

    bx    lr
}

/*---------------------------------------------------------------------------*
  Name:         MTX_TransApply44

  Description:  Calc T(x, y, z) * pSrc = pDst

  Arguments:    pSrc         a pointer to a 4x4 matrix
                pDst         a pointer to a 4x4 matrix
                x            x trans factor
                y            y trans factor
                z            z trans factor

  Returns:      none
 *---------------------------------------------------------------------------*/
void MTX_TransApply44(const MtxFx44 *pSrc, MtxFx44 *pDst, fx32 x, fx32 y, fx32 z)
{
    SDK_NULL_ASSERT(pSrc);
    SDK_NULL_ASSERT(pDst);

    if (pSrc != pDst)
    {
        MI_Copy48B(pSrc, pDst);
    }
    {
        fx64    xx = x;
        fx64    yy = y;
        fx64    zz = z;

        pDst->_30 =
            pSrc->_30 + (fx32)((xx * pSrc->_00 + yy * pSrc->_10 + zz * pSrc->_20) >> FX32_SHIFT);
        pDst->_31 =
            pSrc->_31 + (fx32)((xx * pSrc->_01 + yy * pSrc->_11 + zz * pSrc->_21) >> FX32_SHIFT);
        pDst->_32 =
            pSrc->_32 + (fx32)((xx * pSrc->_02 + yy * pSrc->_12 + zz * pSrc->_22) >> FX32_SHIFT);
        pDst->_33 =
            pSrc->_33 + (fx32)((xx * pSrc->_03 + yy * pSrc->_13 + zz * pSrc->_23) >> FX32_SHIFT);
    }
}

asm void MTX_Transpose44_(register const MtxFx44* pSrc, register MtxFx44* pDst)
{
    stmfd sp!, {r4-r11}

    ldr   r12, [r0, #48]
    ldmia r0!, {r2-r11}
    stmia r1!, {r2, r6, r10, r12}
    ldr   r12, [r0, #12]
    stmia r1!, {r3, r7, r11, r12}

    ldmia r0,  {r10-r11}
    ldr   r12, [r0, #16]
    stmia r1!, {r4, r8, r10, r12}

    ldr   r12, [r0, #20]
    stmia r1!, {r5, r9, r11, r12}
    
    ldmfd sp!, {r4-r11}
    bx    lr
}


#include <nitro/code16.h>
asm void MTX_Scale44_(register MtxFx44 * pDst, register fx32 x, register fx32 y, register fx32 z)
{
    stmia r0!, {r1}
    mov   r1, #0
    str   r2, [r0, #16]
    mov   r2, #0
    stmia r0!, {r1, r2}
    stmia r0!, {r1, r2}
    add   r0, r0, #4
    stmia r0!, {r1, r2}
    stmia r0!, {r1, r2, r3}
    stmia r0!, {r1, r2}
    mov   r3, #1
    lsl   r3, r3, #12
    stmia r0!, {r1, r2}
    str   r3, [r0, #0]
    bx lr
}
#include <nitro/code32.h>




/*---------------------------------------------------------------------------*
  Name:         MTX_ScaleApply44

  Description:  This function performs the operation equivalent to
                MTX_Scale44 + MTX_Concat44.

  Arguments:    pSrc         a pointer to a 4x4 matrix
                pDst         a pointer to a 4x4 matrix
                x            x scale factor
                y            y scale factor
                z            z scale factor
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void MTX_ScaleApply44(const MtxFx44 *pSrc, MtxFx44 *pDst, fx32 x, fx32 y, fx32 z)
{
    fx64    v;

    SDK_NULL_ASSERT(pSrc);
    SDK_NULL_ASSERT(pDst);

    v = (fx64)x;
    pDst->_00 = mul64(v, pSrc->_00);
    pDst->_01 = mul64(v, pSrc->_01);
    pDst->_02 = mul64(v, pSrc->_02);
    pDst->_03 = mul64(v, pSrc->_03);

    v = (fx64)y;
    pDst->_10 = mul64(v, pSrc->_10);
    pDst->_11 = mul64(v, pSrc->_11);
    pDst->_12 = mul64(v, pSrc->_12);
    pDst->_13 = mul64(v, pSrc->_13);

    v = (fx64)z;
    pDst->_20 = mul64(v, pSrc->_20);
    pDst->_21 = mul64(v, pSrc->_21);
    pDst->_22 = mul64(v, pSrc->_22);
    pDst->_23 = mul64(v, pSrc->_23);

    if (pSrc != pDst)
    {
        register fx32 t1, t2;
        t1 = pSrc->_30;
        t2 = pSrc->_31;
        pDst->_30 = t1;
        pDst->_31 = t2;

        t1 = pSrc->_32;
        t2 = pSrc->_33;
        pDst->_32 = t1;
        pDst->_33 = t2;
    }
}

#include <nitro/code16.h>
asm void MTX_RotX44_(register MtxFx44 * pDst, register fx32 sinVal, register fx32 cosVal)
{
    
    str   r2, [r0, #20]
    str   r2, [r0, #40]
    str   r1, [r0, #24]
    neg   r1, r1
    str   r1, [r0, #36]

    mov   r1, #1
    mov   r2, #0
    lsl   r1, r1, #12
    mov   r3, #0

    stmia r0!, {r1, r2, r3}
    stmia r0!, {r2, r3}
    add   r0, #8
    stmia r0!, {r2, r3}
    add   r0, #8
    stmia r0!, {r2, r3}
    stmia r0!, {r2, r3}
    str   r1, [r0, #0]

    bx    lr
}
#include <nitro/code32.h>

#include <nitro/code16.h>
asm void MTX_RotY44_(register MtxFx44 * pDst, register fx32 sinVal, register fx32 cosVal)
{
    str   r2, [r0, #0]
    str   r2, [r0, #40]
    str   r1, [r0, #32]
    neg   r1, r1
    str   r1, [r0, #8]

    mov   r3, #1
    mov   r1, #0
    lsl   r3, r3, #12
    mov   r2, #0

    str   r2, [r0, #4]
    add   r0, #12
    stmia r0!, {r1, r2, r3}
    stmia r0!, {r1, r2}
    str   r2, [r0, #4]
    add   r0, #12
    stmia r0!, {r1, r2}
    stmia r0!, {r1, r2, r3}

    bx    lr
}
#include <nitro/code32.h>

#include <nitro/code16.h>
asm void MTX_RotZ44_(register MtxFx44 * pDst, register fx32 sinVal, register fx32 cosVal)
{
    str   r2, [r0, #0]
    str   r2, [r0, #20]
    str   r1, [r0, #4]
    neg   r1, r1
    str   r1, [r0, #16]

    mov   r3, #1
    mov   r1, #0
    lsl   r3, r3, #12
    mov   r2, #0

    add   r0, #8
    stmia r0!, {r1, r2}
    add   r0, #8
    stmia r0!, {r1, r2}
    stmia r0!, {r1, r2, r3}
    stmia r0!, {r1, r2}
    stmia r0!, {r1, r2, r3}

    bx    lr
}
#include <nitro/code32.h>


/*---------------------------------------------------------------------------*
  Name:         MTX_RotAxis44

  Description:  Sets a rotation matrix about an arbitrary axis.

  Arguments:    pDst         a pointer to a 4x4 matrix
                vec          a pointer to a vector containing the x,y,z axis
                             components. *vec must be a unit vector.
                sinVal       sine of an angle of rotation
                cosVal       cosine of an angle of rotation
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void MTX_RotAxis44(MtxFx44 *pDst, const VecFx32 *vec, fx32 sinVal, fx32 cosVal)
{
    fx64    t, ss, cc, xx, yy, zz;
    fx32    t01;
    fx32    s2;
    SDK_NULL_ASSERT(pDst);
    SDK_NULL_ASSERT(vec);

    t = (fx64)(FX32_ONE - cosVal);
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

    pDst->_03 = pDst->_13 = pDst->_23 = pDst->_30 = pDst->_31 = pDst->_32 = 0;
    pDst->_33 = FX32_ONE;
}


/*---------------------------------------------------------------------------*
  Name:         MTX_Concat44

  Description:  Concatenates two 4x4 matrices.
                The order of operation is A x B = AB.
                OK for any of ab == a == b.

  Arguments:    a            a pointer to a 4x4 matrix
                b            a pointer to a 4x4 matrix
                ab           a pointer to the resultant matrix
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void MTX_Concat44(const MtxFx44 *a, const MtxFx44 *b, MtxFx44 *ab)
{
    MtxFx44 tmp;
    MtxFx44 *p;

    register fx32 x, y, z, w;
    register fx32 xx, yy, zz, ww;

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
    w = a->_03;

    p->_00 =
        (fx32)(((fx64)x * b->_00 + (fx64)y * b->_10 + (fx64)z * b->_20 +
                (fx64)w * b->_30) >> FX32_SHIFT);
    p->_01 =
        (fx32)(((fx64)x * b->_01 + (fx64)y * b->_11 + (fx64)z * b->_21 +
                (fx64)w * b->_31) >> FX32_SHIFT);
    p->_03 =
        (fx32)(((fx64)x * b->_03 + (fx64)y * b->_13 + (fx64)z * b->_23 +
                (fx64)w * b->_33) >> FX32_SHIFT);

    xx = b->_02;
    yy = b->_12;
    zz = b->_22;
    ww = b->_32;

    p->_02 = (fx32)(((fx64)x * xx + (fx64)y * yy + (fx64)z * zz + (fx64)w * ww) >> FX32_SHIFT);

    // row 1
    x = a->_10;
    y = a->_11;
    z = a->_12;
    w = a->_13;

    p->_12 = (fx32)(((fx64)x * xx + (fx64)y * yy + (fx64)z * zz + (fx64)w * ww) >> FX32_SHIFT);
    p->_11 =
        (fx32)(((fx64)x * b->_01 + (fx64)y * b->_11 + (fx64)z * b->_21 +
                (fx64)w * b->_31) >> FX32_SHIFT);
    p->_13 =
        (fx32)(((fx64)x * b->_03 + (fx64)y * b->_13 + (fx64)z * b->_23 +
                (fx64)w * b->_33) >> FX32_SHIFT);

    xx = b->_00;
    yy = b->_10;
    zz = b->_20;
    ww = b->_30;

    p->_10 = (fx32)(((fx64)x * xx + (fx64)y * yy + (fx64)z * zz + (fx64)w * ww) >> FX32_SHIFT);

    // row 2
    x = a->_20;
    y = a->_21;
    z = a->_22;
    w = a->_23;

    p->_20 = (fx32)(((fx64)x * xx + (fx64)y * yy + (fx64)z * zz + (fx64)w * ww) >> FX32_SHIFT);
    p->_21 =
        (fx32)(((fx64)x * b->_01 + (fx64)y * b->_11 + (fx64)z * b->_21 +
                (fx64)w * b->_31) >> FX32_SHIFT);
    p->_23 =
        (fx32)(((fx64)x * b->_03 + (fx64)y * b->_13 + (fx64)z * b->_23 +
                (fx64)w * b->_33) >> FX32_SHIFT);

    xx = b->_02;
    yy = b->_12;
    zz = b->_22;
    ww = b->_32;

    p->_22 = (fx32)(((fx64)x * xx + (fx64)y * yy + (fx64)z * zz + (fx64)w * ww) >> FX32_SHIFT);

    // row 3
    x = a->_30;
    y = a->_31;
    z = a->_32;
    w = a->_33;

    p->_32 = (fx32)(((fx64)x * xx + (fx64)y * yy + (fx64)z * zz + (fx64)w * ww) >> FX32_SHIFT);
    p->_31 =
        (fx32)(((fx64)x * b->_01 + (fx64)y * b->_11 + (fx64)z * b->_21 +
                (fx64)w * b->_31) >> FX32_SHIFT);
    p->_30 =
        (fx32)(((fx64)x * b->_00 + (fx64)y * b->_10 + (fx64)z * b->_20 +
                (fx64)w * b->_30) >> FX32_SHIFT);
    p->_33 =
        (fx32)(((fx64)x * b->_03 + (fx64)y * b->_13 + (fx64)z * b->_23 +
                (fx64)w * b->_33) >> FX32_SHIFT);

    if (p == &tmp)
    {
        *ab = tmp;
    }
}

/*---------------------------------------------------------------------------*
  Name:         MTX_FrustumW

  Description:  Computes a 4x4 perspective projection matrix from a specified
                view volume.

  Arguments:    t            top coordinate of view volume at the near clipping plane
                b            bottom coordinate of view volume at the near clipping plane
                l            left coordinate of view volume at near clipping plane
                r            right coordinate of view volume at near clipping plane
                n            positive distance from camera to near clipping plane
                f            positive distance from camera to far clipping plane
                mtx          4x4 matrix to be set if not NULL

  Returns:      none
 *---------------------------------------------------------------------------*/
void MTX_FrustumW(fx32 t, fx32 b, fx32 l, fx32 r, fx32 n, fx32 f, fx32 scaleW, MtxFx44 *mtx)
{
    fx64c   inv1, inv2;
    fx32    dblN;

    SDK_NULL_ASSERT(mtx);
    SDK_ASSERTMSG(t != b, "MTX_Frustum: 't' and 'b' clipping planes are equal.");
    SDK_ASSERTMSG(l != r, "MTX_Frustum: 'l' and 'r' clipping planes are equal.");
    SDK_ASSERTMSG(n != f, "MTX_Frustum: 'n' and 'f' clipping planes are equal.");

    SDK_MINMAX_ASSERT(f, -0x10000 * FX32_ONE, 0x10000 * FX32_ONE - 1);
    SDK_MINMAX_ASSERT(n, -0x10000 * FX32_ONE, 0x10000 * FX32_ONE - 1);

    /* ”ñ“¯ŠúœŽZ‚Æ•½s‚µ‚Äs—ñ‚ð‰Šú‰» */
    {
        FX_InvAsync(r - l);
        mtx->_01 = 0;
        mtx->_02 = 0;
        mtx->_03 = 0;
        mtx->_10 = 0;
        mtx->_12 = 0;
        mtx->_13 = 0;
        mtx->_23 = -scaleW;
        mtx->_30 = 0;
        mtx->_31 = 0;
        mtx->_33 = 0;
        dblN = n << 1;
        inv1 = FX_GetInvResultFx64c();
    }
    {
        FX_InvAsyncImm(t - b);
        if (scaleW != FX32_ONE)
        {
            inv1 = (inv1 * scaleW) / FX32_ONE;
        }
        mtx->_00 = FX_Mul32x64c(dblN, inv1);
        inv2 = FX_GetInvResultFx64c();
    }
    {
        FX_InvAsyncImm(n - f);
        if (scaleW != FX32_ONE)
        {
            inv2 = (inv2 * scaleW) / FX32_ONE;
        }
        mtx->_11 = FX_Mul32x64c(dblN, inv2);
        mtx->_20 = FX_Mul32x64c(r + l, inv1);
        mtx->_21 = FX_Mul32x64c(t + b, inv2);
        inv1 = FX_GetInvResultFx64c();
    }
    if (scaleW != FX32_ONE)
    {
        inv1 = (inv1 * scaleW) / FX32_ONE;
    }
    mtx->_22 = FX_Mul32x64c(f + n, inv1);
    mtx->_32 = FX_Mul32x64c(FX_Mul(dblN, f), inv1);
}

/*---------------------------------------------------------------------------*
  Name:         MTX_PerspectiveW

  Description:  Computes a 4x4 perspective projection matrix from field of
                view and aspect ratio.

  Arguments:    fovySin      sine value of fovy
                fovyCos      cosine value of fovy
                aspect       ratio of view window width:height (X / Y)
                n            positive distance from camera to near clipping plane
                f            positive distance from camera to far clipping plane
                scaleW       W scale parameter that adjust precision of view volume.
                mtx          4x4 matrix to be set if not NULL

  Returns:      none
 *---------------------------------------------------------------------------*/
void MTX_PerspectiveW(fx32 fovySin, fx32 fovyCos, fx32 aspect, fx32 n, fx32 f, fx32 scaleW, MtxFx44 *mtx)
{
    fx32    one_tan;
    fx64    t;

    SDK_NULL_ASSERT(mtx);
    SDK_ASSERTMSG(fovySin > 0 && fovySin < FX32_ONE, "G3_Perspective: sine of fovy out of range.");
    SDK_ASSERTMSG(fovyCos > -FX32_ONE
                  && fovyCos < FX32_ONE, "G3_Perspective: cosine of fovy out of range.");
    SDK_ASSERTMSG(aspect != 0, "G3_Perspective: aspect is 0.");

    SDK_MINMAX_ASSERT(f, -0x10000 * FX32_ONE, 0x10000 * FX32_ONE - 1);
    SDK_MINMAX_ASSERT(n, -0x10000 * FX32_ONE, 0x10000 * FX32_ONE - 1);

    one_tan = FX_Div((fx32)fovyCos, (fx32)fovySin);
    FX_InvAsyncImm(n - f);
    if (scaleW != FX32_ONE)
    {
        one_tan = (one_tan * scaleW) / FX32_ONE;
    }
    mtx->_01 = 0;
    mtx->_02 = 0;
    mtx->_03 = 0;
    mtx->_10 = 0;
    mtx->_11 = one_tan;
    mtx->_12 = 0;
    mtx->_13 = 0;
    mtx->_20 = 0;
    mtx->_21 = 0;
    mtx->_23 = -scaleW;
    mtx->_30 = 0;
    mtx->_31 = 0;
    mtx->_33 = 0;
    t = FX_GetInvResultFx64c();
    FX_DivAsyncImm(one_tan, aspect);
    if (scaleW != FX32_ONE)
    {
        t = (t * scaleW) / FX32_ONE;
    }
    mtx->_22 = FX_Mul32x64c(f + n, t);
    mtx->_32 = FX_Mul32x64c(FX_Mul(n << 1, f), t);
    mtx->_00 = FX_GetDivResult();
}

/*---------------------------------------------------------------------------*
  Name:         MTX_OrthoW

  Description:  Computes a 4x4 orthographic projection matrix.

  Arguments:    t            top coordinate of parallel view volume
                b            bottom coordinate of parallel view volume
                l            left coordinate of parallel view volume
                r            right coordinate of parallel view volume
                n            positive distance from camera to near clipping plane
                f            positive distance from camera to far clipping plane
                scaleW       W scale parameter that adjust precision of view volume.
                mtx          4x4 matrix to be set if not NULL

  Returns:      none
 *---------------------------------------------------------------------------*/
void MTX_OrthoW(fx32 t, fx32 b, fx32 l, fx32 r, fx32 n, fx32 f, fx32 scaleW, MtxFx44 *mtx)
{
    fx64c   inv1, inv2, inv3;

    SDK_NULL_ASSERT(mtx);
    SDK_ASSERTMSG(t != b, "G3_Ortho: 't' and 'b' clipping planes are equal.");
    SDK_ASSERTMSG(l != r, "G3_Ortho: 'l' and 'r' clipping planes are equal.");
    SDK_ASSERTMSG(n != f, "G3_Ortho: 'n' and 'f' clipping planes are equal.");

    SDK_MINMAX_ASSERT(f, -0x10000 * FX32_ONE, 0x10000 * FX32_ONE - 1);
    SDK_MINMAX_ASSERT(n, -0x10000 * FX32_ONE, 0x10000 * FX32_ONE - 1);

    {
        FX_InvAsync(r - l);
        mtx->_01 = 0;
        mtx->_02 = 0;
        mtx->_03 = 0;
        mtx->_10 = 0;
        mtx->_12 = 0;
        mtx->_13 = 0;
        mtx->_20 = 0;
        mtx->_21 = 0;
        mtx->_23 = 0;
        mtx->_33 = scaleW;
        inv1 = FX_GetInvResultFx64c();
    }
    {
        FX_InvAsyncImm(t - b);
        if (scaleW != FX32_ONE)
        {
            inv1 = (inv1 * scaleW) / FX32_ONE;
        }
        mtx->_00 = FX_Mul32x64c(FX32_ONE * 2, inv1);
        inv2 = FX_GetInvResultFx64c();
    }
    {
        FX_InvAsyncImm(n - f);
        if (scaleW != FX32_ONE)
        {
            inv2 = (inv2 * scaleW) / FX32_ONE;
        }
        mtx->_11 = FX_Mul32x64c(FX32_ONE * 2, inv2);
        inv3 = FX_GetInvResultFx64c();
    }

    if (scaleW != FX32_ONE)
    {
        inv3 = (inv3 * scaleW) / FX32_ONE;
    }
    mtx->_22 = FX_Mul32x64c(FX32_ONE * 2, inv3);
    mtx->_30 = FX_Mul32x64c(-r - l, inv1);
    mtx->_31 = FX_Mul32x64c(-t - b, inv2);
    mtx->_32 = FX_Mul32x64c(f + n, inv3);
}

#include <nitro/codereset.h>
