/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - 
  File:     fx_mtx22.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: fx_mtx22.c,v $
  Revision 1.9  2006/01/18 02:11:20  kitase_hirotake
  do-indent

  Revision 1.8  2005/03/01 01:57:00  yosizaki
  copyright ÇÃîNÇèCê≥.

  Revision 1.7  2005/02/28 05:26:13  yosizaki
  do-indent.

  Revision 1.6  2004/11/02 17:57:55  takano_makoto
  fix comments.

  Revision 1.5  2004/05/27 09:40:26  takano_makoto
  ARM, THUMBÇÃéwíËïœçX

  Revision 1.4  2004/05/26 01:55:29  takano_makoto
  Fix bug in MTX_Identity22 and MTX_Transpose22

  Revision 1.3  2004/05/17 09:21:42  takano_makoto
  Add FX_Scale22() and FX_ScaleApply22().

  Revision 1.2  2004/04/15 06:10:14  takano_makoto
  Fix MTX_Identity22

  Revision 1.1  2004/04/15 05:58:59  takano_makoto
  Initial Upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/fx/fx_mtx22.h>
#include <nitro/fx/fx_const.h>
#include <nitro/fx/fx_cp.h>

inline fx32 mul64(fx64 x, fx32 y)
{
    return (fx32)((x * y) >> FX32_SHIFT);
}

#include <nitro/code32.h>              // Always generate ARM binary for efficiency

asm void MTX_Identity22_(register MtxFx22* pDst)
{
    mov  r1, #0
    mov  r2, #4096                     // FX32_ONE
    mov  r3, #0
    stmia r0!, {r2, r3}
    stmia r0!, {r1, r2}
    bx   lr
}

asm void MTX_Transpose22_(const register MtxFx22* pSrc, register MtxFx22* pDst)
{
    stmfd sp!, {r4}
    
    ldmia r0, {r2-r4, r12}
    stmia r1!, {r2, r4}
    stmia r1!, {r3, r12}
    
    ldmfd sp!, {r4}
    bx    lr
}

#include <nitro/code16.h>
asm void MTX_Scale22_(register MtxFx22* pDst, register fx32 x, register fx32 y)
{
    stmia r0!, {r1}
    mov r1, #0
    str r2, [r0, #8]
    mov r2, #0
    stmia r0!, {r1, r2}
    
    bx lr
}

asm void MTX_Rot22_(register MtxFx22 * pDst, register fx32 sinVal, register fx32 cosVal)
{
    str r2, [r0, #0]
    str r1, [r0, #4]
    neg r1, r1
    str r1, [r0, #8]
    str r2, [r0, #12]

    bx  lr
}

#include <nitro/codereset.h>

/*---------------------------------------------------------------------------*
  Name:         MTX_ScaleApply22

  Description:  This function performs the operation equivalent to
                MTX_Scale22 + MTX_Concat22.

  Arguments:    pSrc         a pointer to a 2x2 matrix
                pDst         a pointer to a 2x2 matrix
                x            x scale factor
                y            y scale factor
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void MTX_ScaleApply22(const MtxFx22 *pSrc, MtxFx22 *pDst, fx32 x, fx32 y)
{
    fx64    v;

    SDK_NULL_ASSERT(pSrc);
    SDK_NULL_ASSERT(pDst);

    v = (fx64)x;
    pDst->_00 = mul64(v, pSrc->_00);
    pDst->_01 = mul64(v, pSrc->_01);

    v = (fx64)y;
    pDst->_10 = mul64(v, pSrc->_10);
    pDst->_11 = mul64(v, pSrc->_11);
}


/*---------------------------------------------------------------------------*
  Name:         MTX_Inverse22

  Description:  Computes an inverse of a 2x2 matrix.

  Arguments:    pSrc         a pointer to a 2x2 matrix
                pDst         a pointer to a 2x2 matrix
  
  Returns:      -1 if *pSrc is not invertible.
                0 if success.
 *---------------------------------------------------------------------------*/
int MTX_Inverse22(const MtxFx22 *pSrc, MtxFx22 *pDst)
{
    MtxFx22 tmp;
    MtxFx22 *p;
    fx32    det;

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

    // Comptutes the determinant
    det = (fx32)(((fx64)pSrc->_00 * pSrc->_11 -
                  (fx64)pSrc->_01 * pSrc->_10 + (fx64)(FX32_ONE >> 1)) >> FX32_SHIFT);

    if (0 == det)
    {
        return -1;
    }

    FX_InvAsync(det);

    det = FX_GetDivResult();

    p->_00 = (fx32)(((fx64)det * pSrc->_11) >> FX32_SHIFT);
    p->_01 = -(fx32)(((fx64)det * pSrc->_01) >> FX32_SHIFT);

    p->_10 = -(fx32)(((fx64)det * pSrc->_10) >> FX32_SHIFT);
    p->_11 = (fx32)(((fx64)det * pSrc->_00) >> FX32_SHIFT);

    if (p == &tmp)
    {
        MI_Copy16B(&tmp, pDst);
    }
    return 0;
}


/*---------------------------------------------------------------------------*
  Name:         MTX_Concat22

  Description:  Concatenates two 2x2 matrices.
                The order of operation is A x B = AB.
                OK for any of ab == a == b.

  Arguments:    a            a pointer to a 2x2 matrix
                b            a pointer to a 2x2 matrix
                ab           a pointer to the resultant matrix
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void MTX_Concat22(const MtxFx22 *a, const MtxFx22 *b, MtxFx22 *ab)
{
    MtxFx22 tmp;
    MtxFx22 *p;

    register fx32 x, y;

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

    p->_00 = (fx32)(((fx64)x * b->_00 + (fx64)y * b->_10) >> FX32_SHIFT);
    p->_01 = (fx32)(((fx64)x * b->_01 + (fx64)y * b->_11) >> FX32_SHIFT);

    // row 1
    x = a->_10;
    y = a->_11;
    p->_10 = (fx32)(((fx64)x * b->_00 + (fx64)y * b->_10) >> FX32_SHIFT);
    p->_11 = (fx32)(((fx64)x * b->_01 + (fx64)y * b->_11) >> FX32_SHIFT);

    if (p == &tmp)
    {
        *ab = tmp;
    }
}
