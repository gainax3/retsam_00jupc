/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - 
  File:     fx_vec.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: fx_vec.c,v $
  Revision 1.26  2006/04/27 10:42:19  kitase_hirotake
  tの値が大きすぎる場合のチェック追加

  Revision 1.25  2006/01/18 02:11:20  kitase_hirotake
  do-indent

  Revision 1.24  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.23  2005/02/28 05:26:13  yosizaki
  do-indent.

  Revision 1.22  2004/11/02 17:57:55  takano_makoto
  fix comments.

  Revision 1.21  2004/02/18 09:00:05  nishida_kenji
  Add comments for code32.h.

  Revision 1.20  2004/02/17 05:28:45  nishida_kenji
  Add VEC_Fx16DotProduct.

  Revision 1.19  2004/02/16 08:11:28  nishida_kenji
  Add VEC_MultAdd, VEC_MultSubtract, VEC_Distance.

  Revision 1.18  2004/02/16 04:10:54  nishida_kenji
  small fix.

  Revision 1.17  2004/02/16 02:45:52  nishida_kenji
  Add APIs for VecFx16.

  Revision 1.16  2004/02/12 11:21:14  nishida_kenji
  VEC_Add, VEC_Subtract in .c file.

  Revision 1.15  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.14  2004/02/03 06:42:57  nishida_kenji
  Add comments.

  Revision 1.13  2004/01/26 07:20:45  nishida_kenji
  include code32.h

  Revision 1.11  2004/01/22 02:45:54  nishida_kenji
  fix VEC_Mag

  Revision 1.10  2004/01/19 07:23:21  nishida_kenji
  small fix

  Revision 1.9  2004/01/18 04:39:40  nishida_kenji
  revises the interfaces for asynchronous calculation

  Revision 1.8  2004/01/17 13:24:23  nishida_kenji
  revise VEC_Normalize

  Revision 1.7  2004/01/09 09:27:28  nishida_kenji
  fix comments

  Revision 1.6  2003/12/09 11:43:04  nishida_kenji
  improve asserts

  $NoKeywords: $
 *---------------------------------------------------------------------------*/



#include <nitro/fx/fx_vec.h>
#include <nitro/fx/fx_cp.h>
#include <nitro/fx/fx_const.h>
#include <nitro/cp/divider.h>
#include <nitro/cp/sqrt.h>


/*---------------------------------------------------------------------*
Name:           VEC_Add

Description:    add two vectors.

Arguments:      a    a pointer to first vector
                b    a pointer to second vector
                ab   a pointer to resultant vector (a + b).
                     ok if ab == a or ab == b.

Return:         none.
 *---------------------------------------------------------------------*/
void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *ab)
{
    SDK_NULL_ASSERT(a);
    SDK_NULL_ASSERT(b);
    SDK_NULL_ASSERT(ab);

    ab->x = a->x + b->x;
    ab->y = a->y + b->y;
    ab->z = a->z + b->z;
}


/*---------------------------------------------------------------------*
Name:           VEC_Subtract

Description:    subtract one vector from another.

Arguments:      a       a pointer to first vector
                b       a pointer to second vector.
                a_b     a pointer to resultant vector (a - b).
                        ok if a_b == a or a_b == b.

Return:         none.
 *---------------------------------------------------------------------*/
void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *ab)
{
    SDK_NULL_ASSERT(a);
    SDK_NULL_ASSERT(b);
    SDK_NULL_ASSERT(ab);

    ab->x = a->x - b->x;
    ab->y = a->y - b->y;
    ab->z = a->z - b->z;
}


/*---------------------------------------------------------------------*
Name:           VEC_Fx16Add

Description:    add two vectors.

Arguments:      a    a pointer to first vector
                b    a pointer to second vector
                ab   a pointer to resultant vector (a + b).
                     ok if ab == a or ab == b.

Return:         none.
 *---------------------------------------------------------------------*/
void VEC_Fx16Add(const VecFx16 *a, const VecFx16 *b, VecFx16 *ab)
{
    SDK_NULL_ASSERT(a);
    SDK_NULL_ASSERT(b);
    SDK_NULL_ASSERT(ab);

    ab->x = (fx16)(a->x + b->x);
    ab->y = (fx16)(a->y + b->y);
    ab->z = (fx16)(a->z + b->z);
}


/*---------------------------------------------------------------------*
Name:           VEC_Fx16Subtract

Description:    subtract one vector from another.

Arguments:      a       a pointer to first vector
                b       a pointer to second vector.
                a_b     a pointer to resultant vector (a - b).
                        ok if a_b == a or a_b == b.

Return:         none.
 *---------------------------------------------------------------------*/
void VEC_Fx16Subtract(const VecFx16 *a, const VecFx16 *b, VecFx16 *ab)
{
    SDK_NULL_ASSERT(a);
    SDK_NULL_ASSERT(b);
    SDK_NULL_ASSERT(ab);

    ab->x = (fx16)(a->x - b->x);
    ab->y = (fx16)(a->y - b->y);
    ab->z = (fx16)(a->z - b->z);
}


#include <nitro/code32.h>              // Always generate ARM binary for efficiency
/*---------------------------------------------------------------------------*
  Name:         VEC_DotProduct

  Description:  Computes the dot product of two vectors.

  Arguments:    a            a pointer to a vector
                b            a pointer to a vector
  
  Returns:      dot product value in fx32 format
 *---------------------------------------------------------------------------*/
fx32 VEC_DotProduct(const VecFx32 *a, const VecFx32 *b)
{
    SDK_NULL_ASSERT(a);
    SDK_NULL_ASSERT(b);
    return (fx32)(((fx64)a->x * b->x +
                   (fx64)a->y * b->y + (fx64)a->z * b->z + (1 << (FX64_SHIFT - 1))) >> FX64_SHIFT);
}


/*---------------------------------------------------------------------------*
  Name:         VEC_Fx16DotProduct

  Description:  Computes the dot product of two vectors.

  Arguments:    a            a pointer to a vector
                b            a pointer to a vector
  
  Returns:      dot product value in fx32 format
 *---------------------------------------------------------------------------*/
fx32 VEC_Fx16DotProduct(const VecFx16 *a, const VecFx16 *b)
{
    fx32    tmp1, tmp2;
    SDK_NULL_ASSERT(a);
    SDK_NULL_ASSERT(b);

    tmp1 = (a->x * b->x) + (a->y * b->y);
    tmp2 = (a->z * b->z) + (1 << (FX64_SHIFT - 1));
    return (fx32)(((fx64)tmp1 + tmp2) >> FX64_SHIFT);
}


/*---------------------------------------------------------------------------*
  Name:         VEC_CrossProduct

  Description:  Computes the cross product of two vectors.

  Arguments:    a            a pointer to a vector
                b            a pointer to a vector
                axb          a pointer to the result vector,
                             OK if axb == a or axb == b.
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void VEC_CrossProduct(const VecFx32 *a, const VecFx32 *b, VecFx32 *axb)
{
    fx32    x, y, z;
    SDK_NULL_ASSERT(a);
    SDK_NULL_ASSERT(b);
    SDK_NULL_ASSERT(axb);

    x = (fx32)(((fx64)a->y * b->z - (fx64)a->z * b->y + (1 << (FX64_SHIFT - 1))) >> FX64_SHIFT);

    y = (fx32)(((fx64)a->z * b->x - (fx64)a->x * b->z + (1 << (FX64_SHIFT - 1))) >> FX64_SHIFT);

    z = (fx32)(((fx64)a->x * b->y - (fx64)a->y * b->x + (1 << (FX64_SHIFT - 1))) >> FX32_SHIFT);

    axb->x = x;
    axb->y = y;
    axb->z = z;
}


/*---------------------------------------------------------------------------*
  Name:         VEC_Fx16CrossProduct

  Description:  Computes the cross product of two vectors.

  Arguments:    a            a pointer to a vector
                b            a pointer to a vector
                axb          a pointer to the result vector,
                             OK if axb == a or axb == b.
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void VEC_Fx16CrossProduct(const VecFx16 *a, const VecFx16 *b, VecFx16 *axb)
{
    fx32    x, y, z;
    SDK_NULL_ASSERT(a);
    SDK_NULL_ASSERT(b);
    SDK_NULL_ASSERT(axb);

    x = ((a->y * b->z - a->z * b->y + (FX16_ONE >> 1)) >> FX16_SHIFT);
    y = ((a->z * b->x - a->x * b->z + (FX16_ONE >> 1)) >> FX16_SHIFT);
    z = ((a->x * b->y - a->y * b->x + (FX16_ONE >> 1)) >> FX16_SHIFT);

    SDK_WARNING(x <= FX16_MAX && x >= FX16_MIN &&
                y <= FX16_MAX && y >= FX16_MIN &&
                z <= FX16_MAX && z >= FX16_MIN,
                "VEC_Fx16CrossProduct: Overflow (0x%x, 0x%x, 0x%x) x (0x%x, 0x%x, 0x%x).",
                a->x, a->y, a->z, b->x, b->y, b->z);

    axb->x = (fx16)x;
    axb->y = (fx16)y;
    axb->z = (fx16)z;
}


/*---------------------------------------------------------------------------*
  Name:         VEC_Mag

  Description:  Computes the magnitude of a vector

  Arguments:    pSrc         a pointer to a vector
  
  Returns:      magnitude of the vector in fx32 format
 *---------------------------------------------------------------------------*/
fx32 VEC_Mag(const VecFx32 *pSrc)
{
    fx64    t;
    fx32    rval;

    SDK_NULL_ASSERT(pSrc);
    SDK_ASSERT(!CP_IsSqrtBusy());

    t = (fx64)pSrc->x * pSrc->x;
    t += (fx64)pSrc->y * pSrc->y;
    t += (fx64)pSrc->z * pSrc->z;

    t <<= 2;                           // for better precision

    CP_SetSqrt64((u64)t);
    rval = ((fx32)CP_GetSqrtResult32() + 1) >> 1;
    return rval;
}


/*---------------------------------------------------------------------------*
  Name:         VEC_Fx16Mag

  Description:  Computes the magnitude of a vector

  Arguments:    pSrc         a pointer to a vector
  
  Returns:      magnitude of the vector in fx32 format
 *---------------------------------------------------------------------------*/
fx32 VEC_Fx16Mag(const VecFx16 *pSrc)
{
    fx64    t;
    fx32    rval;

    SDK_NULL_ASSERT(pSrc);
    SDK_ASSERT(!CP_IsSqrtBusy());

    t = pSrc->x * pSrc->x;
    t += pSrc->y * pSrc->y;
    t += pSrc->z * pSrc->z;

    t <<= 2;                           // for better precision

    CP_SetSqrt64((u64)t);
    rval = ((fx32)CP_GetSqrtResult32() + 1) >> 1;
    return rval;
}


/*---------------------------------------------------------------------------*
  Name:         VEC_Normalize

  Description:  Normalizes a vector

  Arguments:    pSrc         a pointer to a vector
                pDst         a pointer to the normalized vector
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void VEC_Normalize(const VecFx32 *pSrc, VecFx32 *pDst)
{
    fx64    t;
    s32     sqrt;

    SDK_NULL_ASSERT(pSrc);
    SDK_NULL_ASSERT(pDst);
    SDK_ASSERT(!CP_IsDivBusy() && !CP_IsSqrtBusy());

    t = (fx64)pSrc->x * pSrc->x;
    t += (fx64)pSrc->y * pSrc->y;
    t += (fx64)pSrc->z * pSrc->z;      // in s39.24 format
    SDK_ASSERTMSG(t > 0, "*pSrc is (0, 0, 0)");

    SDK_WARNING(1LL << (FX32_SHIFT + FX32_SHIFT + FX64C_SHIFT) >= (u64)t, "*pSrc is too large!");

    CP_SetDiv64_64(1LL << (FX32_SHIFT + FX32_SHIFT + FX64C_SHIFT), (u64)t);
    CP_SetSqrt64((u64)(t << 2));

    sqrt = (s32)CP_GetSqrtResult32();  // in s18.13 format    
    t = CP_GetDivResult64();           // in s31.32 format(fx64c)

    t = t * sqrt;                      // in s18.45 format
    pDst->x = (fx32)((t * pSrc->x + (1LL << (32 + FX32_SHIFT))) >> (32 + FX32_SHIFT + 1));
    pDst->y = (fx32)((t * pSrc->y + (1LL << (32 + FX32_SHIFT))) >> (32 + FX32_SHIFT + 1));
    pDst->z = (fx32)((t * pSrc->z + (1LL << (32 + FX32_SHIFT))) >> (32 + FX32_SHIFT + 1));
}


/*---------------------------------------------------------------------------*
  Name:         VEC_Fx16Normalize

  Description:  Normalizes a vector

  Arguments:    pSrc         a pointer to a vector
                pDst         a pointer to the normalized vector
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void VEC_Fx16Normalize(const VecFx16 *pSrc, VecFx16 *pDst)
{
    fx64    t;
    s32     sqrt;

    SDK_NULL_ASSERT(pSrc);
    SDK_NULL_ASSERT(pDst);
    SDK_ASSERT(!CP_IsDivBusy() && !CP_IsSqrtBusy());

    t = pSrc->x * pSrc->x;
    t += pSrc->y * pSrc->y;
    t += pSrc->z * pSrc->z;            // in s39.24 format
    SDK_ASSERTMSG(t > 0, "*pSrc is (0, 0, 0)");

    CP_SetDiv64_64(1LL << (FX32_SHIFT + FX32_SHIFT + FX64C_SHIFT), (u64)t);
    CP_SetSqrt64((u64)(t << 2));

    sqrt = (s32)CP_GetSqrtResult32();  // in s18.13 format    
    t = CP_GetDivResult64();           // in s31.32 format(fx64c)

    t = t * sqrt;                      // in s18.45 format
    pDst->x = (fx16)((t * pSrc->x + (1LL << (32 + FX32_SHIFT))) >> (32 + FX32_SHIFT + 1));
    pDst->y = (fx16)((t * pSrc->y + (1LL << (32 + FX32_SHIFT))) >> (32 + FX32_SHIFT + 1));
    pDst->z = (fx16)((t * pSrc->z + (1LL << (32 + FX32_SHIFT))) >> (32 + FX32_SHIFT + 1));
}


/*---------------------------------------------------------------------------*
  Name:         VEC_MultAdd

  Description:  Computes 'a * v1 + v2'.

  Arguments:    a            fixed point number(fx32)
                v1           a pointer to a vector
                v2           a pointer to a vector
                pDest        a pointer to a resultant vector
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void VEC_MultAdd(fx32 a, const VecFx32 *v1, const VecFx32 *v2, VecFx32 *pDest)
{
    SDK_NULL_ASSERT(v1);
    SDK_NULL_ASSERT(v2);
    SDK_NULL_ASSERT(pDest);

    pDest->x = v2->x + (fx32)(((fx64)a * v1->x) >> FX32_SHIFT);
    pDest->y = v2->y + (fx32)(((fx64)a * v1->y) >> FX32_SHIFT);
    pDest->z = v2->z + (fx32)(((fx64)a * v1->z) >> FX32_SHIFT);
}


/*---------------------------------------------------------------------------*
  Name:         VEC_MultSubtract

  Description:  Computes 'a * v1 - v2'.

  Arguments:    a            fixed point number(fx32)
                v1           a pointer to a vector
                v2           a pointer to a vector
                pDest        a pointer to a resultant vector
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void VEC_MultSubtract(fx32 a, const VecFx32 *v1, const VecFx32 *v2, VecFx32 *pDest)
{
    SDK_NULL_ASSERT(v1);
    SDK_NULL_ASSERT(v2);
    SDK_NULL_ASSERT(pDest);

    pDest->x = -v2->x + (fx32)(((fx64)a * v1->x) >> FX32_SHIFT);
    pDest->y = -v2->y + (fx32)(((fx64)a * v1->y) >> FX32_SHIFT);
    pDest->z = -v2->z + (fx32)(((fx64)a * v1->z) >> FX32_SHIFT);
}


/*---------------------------------------------------------------------------*
  Name:         VEC_Distance

  Description:  Computes |v1 - v2|

  Arguments:    v1           a pointer to a vector
                v2           a pointer to a vector
  
  Returns:      distance between v1 and v2
 *---------------------------------------------------------------------------*/
fx32 VEC_Distance(const VecFx32 *v1, const VecFx32 *v2)
{
    fx64    tmp;
    fx32    diff;

    SDK_NULL_ASSERT(v1);
    SDK_NULL_ASSERT(v2);

    diff = v1->x - v2->x;
    tmp = (fx64)diff *diff;

    diff = v1->y - v2->y;
    tmp += (fx64)diff *diff;

    diff = v1->z - v2->z;
    tmp += (fx64)diff *diff;

    tmp <<= 2;
    CP_SetSqrt64((u64)tmp);

    return ((fx32)CP_GetSqrtResult32() + 1) >> 1;
}

/*---------------------------------------------------------------------------*
  Name:         VEC_Fx16Distance

  Description:  Computes |v1 - v2|

  Arguments:    v1           a pointer to a vector
                v2           a pointer to a vector
  
  Returns:      distance between v1 and v2
 *---------------------------------------------------------------------------*/
fx32 VEC_Fx16Distance(const VecFx16 *v1, const VecFx16 *v2)
{
    fx64    tmp;
    fx32    diff;

    SDK_NULL_ASSERT(v1);
    SDK_NULL_ASSERT(v2);

    diff = v1->x - v2->x;
    tmp = diff * diff;

    diff = v1->y - v2->y;
    tmp += diff * diff;

    diff = v1->z - v2->z;
    tmp += diff * diff;

    tmp <<= 2;
    CP_SetSqrt64((u64)tmp);

    return ((fx32)CP_GetSqrtResult32() + 1) >> 1;
}


#include <nitro/codereset.h>
