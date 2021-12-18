/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FX - src
  File:     fx_asinacosidx.c

  Copyright 2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: fx_asinacosidx.c,v $
  Revision 1.2  2007/02/20 00:28:11  kitase_hirotake
  indent source

  Revision 1.1  2007/01/18 04:05:21  okubata_ryoma
  FX_AsinIdx, FX_AcosIdx 関数の追加


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/fx/fx_trig.h>
#include <nitro/fx/fx_cp.h>
#include <nitro/fx/fx_const.h>

/* size of array */
#define ARRAY_SIZE_90 1024
#define ARRAY_SIZE_180 2048
#define ARRAY_SIZE_270 3072
#define ARRAY_SIZE_360 4096

#define SINCOS_TABLE_SIZE (4096 * 2)

/*---------------------------------------------------------------------------*
  Name:         FX_AsinIdx

  Description:  Obtain approximation of arc sine by table reference.

  Arguments:    x            Y/X in fx32 format

  Returns:      result in u16 format (0 - 16384, 49152 - 65535)
 *---------------------------------------------------------------------------*/
u16 FX_AsinIdx(fx32 x)
{
    int     left;                      /* start key of index */
    int     right;                     /* end key of index */
    int     mid;                       /* middle key of index */
    fx16    value;                     /* search value */
    int     tmp = 0;

    SDK_MINMAX_ASSERT(x, FX32_SIN270, FX32_SIN90);

    /* xが正の場合は0-90でバイナリサーチ */
    if (x >= 0)
    {
        left = 0;
        right = ARRAY_SIZE_90;
    }
    /* xが負の場合は270-360でバイナリサーチ */
    else
    {
        left = ARRAY_SIZE_270;
        right = ARRAY_SIZE_360;
    }

    value = (fx16)x;

    /* バイナリサーチ */
    while (left <= right)
    {
        mid = (left + right) / 2;      /* calc of middle key */
        if (FX_SinCosTable_[mid * 2] == value)
        {
            break;
        }
        else if (FX_SinCosTable_[mid * 2] < value)
        {
            left = mid + 1;            /* adjustment of left(start) key */
        }
        else
        {
            right = mid - 1;           /* adjustment of right(end) key */
        }
    }
    tmp = (mid * 2) * 65536 / SINCOS_TABLE_SIZE;
    return (u16)tmp;
}

/*---------------------------------------------------------------------------*
  Name:         FX_AcosIdx

  Description:  Obtain approximation of arc cosine by table reference.

  Arguments:    x            Y/X in fx32 format

  Returns:      result in u16 format (0 - 32768)
 *---------------------------------------------------------------------------*/
u16 FX_AcosIdx(fx32 x)
{
    int     left;
    int     right;
    int     mid;
    fx16    value;
    int     tmp = 0;

    SDK_MINMAX_ASSERT(x, FX32_COS180, FX32_COS0);

    /* xが正の場合は0-90でバイナリサーチ */
    if (x >= 0)
    {
        left = 0;
        right = ARRAY_SIZE_90;
    }
    /* xが負の場合は90-180でバイナリサーチ */
    else
    {
        left = ARRAY_SIZE_90;
        right = ARRAY_SIZE_180;
    }

    value = (fx16)x;

    /* バイナリサーチ */
    while (left <= right)
    {
        mid = (left + right) / 2;
        if (FX_SinCosTable_[mid * 2 + 1] == value)
        {
            break;
        }
        else if (FX_SinCosTable_[mid * 2 + 1] < value)
        {
            right = mid - 1;
        }
        else
        {
            left = mid + 1;
        }
    }
    tmp = (mid * 2 + 1) * 65536 / SINCOS_TABLE_SIZE;
    return (u16)tmp;
}
