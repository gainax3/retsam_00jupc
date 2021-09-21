/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MATH - include
  File:     rand.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: rand.h,v $
  Revision 1.4  2006/01/18 02:12:28  kitase_hirotake
  do-indent

  Revision 1.3  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.2  2005/02/28 05:26:24  yosizaki
  do-indent.

  Revision 1.1  2004/12/14 13:11:59  takano_makoto
  FXからMATHへ移動

  Revision 1.1  2004/12/13 07:44:20  takano_makoto
  RNDからFXへ移動

  Revision 1.1  2004/12/09 12:35:22  takano_makoto
  Initial update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/types.h>

#ifndef NITRO_MATH_RAND_H_
#define NITRO_MATH_RAND_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    u64     x;                         // 乱数値
    u64     mul;                       // 乗数
    u64     add;                       // 加算する数
}
MATHRandContext32;


typedef struct
{
    u32     x;                         // 乱数値
    u32     mul;                       // 乗数
    u32     add;                       // 加算する数
}
MATHRandContext16;


/*---------------------------------------------------------------------------*
  Name:         MATH_InitRand32

  Description:  線形合同法による32bit乱数コンテキストを初期化します。

  Arguments:    context 乱数コンテキストへのポインタ
  
                seed    初期値として設定する乱数の種
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void MATH_InitRand32(MATHRandContext32 *context, u64 seed)
{
    context->x = seed;
    context->mul = (1566083941LL << 32) + 1812433253LL;
    context->add = 2531011;
}


/*---------------------------------------------------------------------------*
  Name:         MATH_Rand32

  Description:  線形合同法による32bit乱数取得関数

  Arguments:    context 乱数コンテキストへのポインタ
  
                max     取得数値の範囲を指定 0 ～ max-1 の範囲の値が取得できます。
                        0を指定した場合にはすべての範囲の32bit値となります。
  
  Returns:      32bitのランダム値
 *---------------------------------------------------------------------------*/
static inline u32 MATH_Rand32(MATHRandContext32 *context, u32 max)
{
    context->x = context->mul * context->x + context->add;

    // 引数maxが定数ならばコンパイラにより最適化される。
    if (max == 0)
    {
        return (u32)(context->x >> 32);
    }
    else
    {
        return (u32)(((context->x >> 32) * max) >> 32);
    }
}


/*---------------------------------------------------------------------------*
  Name:         MATH_InitRand16

  Description:  線形合同法による16bit乱数コンテキストを初期化します。

  Arguments:    context 乱数コンテキストへのポインタ
  
                seed    初期値として設定する乱数の種
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void MATH_InitRand16(MATHRandContext16 *context, u32 seed)
{
    context->x = seed;
    context->mul = 1566083941LL;
    context->add = 2531011;
}


/*---------------------------------------------------------------------------*
  Name:         MATH_Rand16

  Description:  線形合同法による16bit乱数取得関数

  Arguments:    context 乱数コンテキストへのポインタ
  
                max     取得数値の範囲を指定 0 ～ max-1 の範囲の値が取得できます。
                        0を指定した場合にはすべての範囲の16bit値となります。
  
  Returns:      16bitのランダム値
 *---------------------------------------------------------------------------*/
static inline u16 MATH_Rand16(MATHRandContext16 *context, u16 max)
{
    context->x = context->mul * context->x + context->add;

    // 引数maxが定数ならばコンパイラにより最適化される。
    if (max == 0)
    {
        return (u16)(context->x >> 16);
    }
    else
    {
        return (u16)(((context->x >> 16) * max) >> 16);
    }
}


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif // NITRO_MATH_RAND_H_
