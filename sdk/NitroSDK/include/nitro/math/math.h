/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MATH - 
  File:     math/math.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: math.h,v $
  Revision 1.15  2006/08/07 13:34:42  yosizaki
  remove prototype of MATH_CLZ.

  Revision 1.14  2006/06/26 13:06:44  yosizaki
  fix MATH_CLZ().

  Revision 1.13  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.12  2005/12/22 06:04:01  yosizaki
  add MATH_CTZ, LSB, MSB.
  change inline to SDK_INLINE.

  Revision 1.11  2005/03/04 11:09:27  yasu
  __declspec(weak) を SDK_WEAK_SYMBOL に置き換え

  Revision 1.10  2005/02/28 05:26:24  yosizaki
  do-indent.

  Revision 1.9  2005/01/21 09:54:42  yasu
  Copyright year 2004->2005

  Revision 1.8  2005/01/21 07:34:19  yasu
  merge with RC4branch

  Revision 1.7.2.1  2005/01/21 05:59:43  yasu
  Fix a bug "MATH_CountLeadingZeros() not inlined"

  Revision 1.7  2004/12/16 09:58:14  takano_makoto
  small fix

  Revision 1.6  2004/12/15 10:52:38  seiki_masashi
  MATH_ROUNDUP32,DOWN32 の追加

  Revision 1.5  2004/12/15 09:39:26  seiki_masashi
  MATH_ROUNDUP,DOWN の追加

  Revision 1.4  2004/12/15 09:17:07  seiki_masashi
  MATH_CountPopulation の追加

  Revision 1.3  2004/12/15 07:13:31  takano_makoto
  MATH_ABS, MATH_IAbs, MATH_CLAMPを追加

  Revision 1.2  2004/12/15 00:34:25  seiki_masashi
  inline 関数に static 修飾子の追加。
  MATH_MIN, MAX の引数の評価順序が引数の順番に一致するよう変更。

  Revision 1.1  2004/12/14 10:52:20  seiki_masashi
  MATH ジャンルを追加

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_MATH_MATH_H_
#define NITRO_MATH_MATH_H_

#include <nitro/misc.h>
#include <nitro/types.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
// Type definition
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Declaration of function
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Implementation of inline function
//----------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         MATH_ABS

  Description:  絶対値を返すマクロ。
                各引数は複数回評価されるため、副作用に注意。
  
  Arguments:    a

  Returns:      a < 0 なら -a, さもなくば a
 *---------------------------------------------------------------------------*/
#define MATH_ABS(a) ( ( (a) < 0 ) ? (-(a)) : (a) )

/*---------------------------------------------------------------------------*
  Name:         MATH_IAbs

  Description:  絶対値を返すインライン関数。
                インライン関数として実装されているので、副作用はない。
  
  Arguments:    a

  Returns:      a < 0 なら -a, さもなくば a
 *---------------------------------------------------------------------------*/
SDK_INLINE int MATH_IAbs(int a)
{
    return (a < 0) ? -a : a;
}


/*---------------------------------------------------------------------------*
  Name:         MATH_CLAMP

  Description:  lowからhighの範囲の値を取得するマクロ。
                各引数は複数回評価されるため、副作用に注意。
  
  Arguments:    x     
                low   最大値
                hight 最小値
  
  Returns:      x < low ならば low,  x > high ならば high さもなくば x
 *---------------------------------------------------------------------------*/
#define MATH_CLAMP(x, low, high) ( ( (x) > (high) ) ? (high) : ( ( (x) < (low) ) ? (low) : (x) ) )


/*---------------------------------------------------------------------------*
  Name:         MATH_MIN

  Description:  2つの引数のどちらか小さいほうを返す。
                マクロとして実装されているので、不等号演算子が定義されている
                型で使用可能。各引数は複数回評価されるため、副作用に注意。

  Arguments:    a, b

  Returns:      a < b なら a, さもなくば b
 *---------------------------------------------------------------------------*/
#define MATH_MIN(a,b) (((a) <= (b)) ? (a) : (b))


/*---------------------------------------------------------------------------*
  Name:         MATH_IMin

  Description:  2つの int 型整数引数のどちらか小さいほうを返す。
                インライン関数として実装されているので、副作用はない。

  Arguments:    a, b - int 型整数

  Returns:      a <= b なら a, さもなくば b
 *---------------------------------------------------------------------------*/
SDK_INLINE int MATH_IMin(int a, int b)
{
    return (a <= b) ? a : b;
}

/*---------------------------------------------------------------------------*
  Name:         MATH_MAX

  Description:  2つの引数のどちらか大きいほうを返す。
                マクロとして実装されているので、不等号演算子が定義されている
                型で使用可能。各引数は複数回評価されるため、副作用に注意。

  Arguments:    a, b

  Returns:      a >= b なら a, さもなくば b
 *---------------------------------------------------------------------------*/
#define MATH_MAX(a,b) (((a) >= (b)) ? (a) : (b))

/*---------------------------------------------------------------------------*
  Name:         MATH_IMax

  Description:  2つの int 型整数引数のどちらか大きいほうを返す。
                インライン関数として実装されているので、副作用はない。

  Arguments:    a, b - int 型整数

  Returns:      a >= b なら a, さもなくば b
 *---------------------------------------------------------------------------*/
SDK_INLINE int MATH_IMax(int a, int b)
{
    return (a >= b) ? a : b;
}

/*---------------------------------------------------------------------------*
  Name:         MATH_ROUNDUP

  Description:  切り上げた数値を返すマクロ。
  
  Arguments:    x
                base - 2 のべき乗の基数

  Returns:      x 以上の最小の base の倍数
 *---------------------------------------------------------------------------*/
#define MATH_ROUNDUP(x, base) (((x) + ((base)-1)) & ~((base)-1))

/*---------------------------------------------------------------------------*
  Name:         MATH_ROUNDDOWN

  Description:  切り下げた数値を返すマクロ。
  
  Arguments:    x
                base - 2 のべき乗の基数

  Returns:      x 以下の最大の base の倍数
 *---------------------------------------------------------------------------*/
#define MATH_ROUNDDOWN(x, base) ((x) & ~((base)-1))

/*---------------------------------------------------------------------------*
  Name:         MATH_ROUNDUP32

  Description:  32の倍数に切り上げた数値を返すマクロ。
  
  Arguments:    x

  Returns:      x 以上の最小の32の倍数
 *---------------------------------------------------------------------------*/
#define MATH_ROUNDUP32(x) MATH_ROUNDUP(x, 32)

/*---------------------------------------------------------------------------*
  Name:         MATH_ROUNDDOWN32

  Description:  32の倍数に切り下げた数値を返すマクロ。
  
  Arguments:    x

  Returns:      x 以下の最大の32の倍数
 *---------------------------------------------------------------------------*/
#define MATH_ROUNDDOWN32(x) MATH_ROUNDDOWN(x, 32)

/*---------------------------------------------------------------------------*
  Name:         MATH_CountLeadingZeros

  Description:  2進数32ビット表現で上位何ビットが0かを求める。
                ARM9 の ARM コードでは 1 命令。

  Arguments:    x

  Returns:      上位から連続する0のビット数
 *---------------------------------------------------------------------------*/
u32     MATH_CountLeadingZerosFunc(u32 x);

#if !defined(PLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32)

/* clz is available ARM mode only */
#ifdef  SDK_ARM9
#if     defined(SDK_CW) || defined(__MWERKS__)
#pragma thumb off
SDK_INLINE u32 MATH_CountLeadingZerosInline(u32 x)
{
    asm
    {
    clz     x, x}
    return  x;
}

#pragma thumb reset
#elif   defined(SDK_ADS)
TO BE   DEFINED
#elif   defined(SDK_GCC)
TO BE   DEFINED
#endif
#endif
#endif /* PLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32 */

#ifndef MATH_CountLeadingZeros
#if       defined(PLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32)
#define MATH_CountLeadingZeros(x) PLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32(x)
#elif     defined(SDK_ARM9) && defined(SDK_CODE_ARM)
#define MATH_CountLeadingZeros(x) MATH_CountLeadingZerosInline(x)
#else                                  // not (ARM9 && CODE_ARM)
#define MATH_CountLeadingZeros(x) MATH_CountLeadingZerosFunc(x)
#endif                                 // ARM9 && CODE_ARM
#endif

/*---------------------------------------------------------------------------*
  Name:         MATH_CLZ

  Description:  MATH_CountLeadingZeros の別名
                2進数32ビット表現で上位何ビットが0かを求める。
                ARM9 の ARM コードでは 1 命令。

  Arguments:    x

  Returns:      上位から連続する0のビット数
 *---------------------------------------------------------------------------*/
#define MATH_CLZ(x) MATH_CountLeadingZeros(x)
/*---------------------------------------------------------------------------*
  Name:         MATH_ILog2

  Description:  u32 の引数 x に 2 を底とした対数 log2(x) の整数部分を求める。
                ただし、x == 0 の場合は特別に -1 を返す。
                ARM9 の ARM コードでは CLZ 命令を使い 2 命令となる。

  Arguments:    x - u32

  Returns:      log2(x) when x > 0, or -1 when x == 0
 *---------------------------------------------------------------------------*/
        SDK_INLINE int MATH_ILog2(u32 x)
{
    return (int)(31 - MATH_CountLeadingZeros(x));
}

/*---------------------------------------------------------------------------*
  Name:         MATH_CountPopulation

  Description:  2進数32ビット表現で1のビット数を求める

  Arguments:    x

  Returns:      2進表現で1となるビット数
 *---------------------------------------------------------------------------*/
u8      MATH_CountPopulation(u32 x);


/*---------------------------------------------------------------------------*
  Name:         MATH_CountTrailingZeros

  Description:  2進数32ビット表現で下位何ビットが0かを求める。
                MATH_CountLeadingZeros 関数を使用する。

  Arguments:    x             判定に使用する u32 の値

  Returns:      下位から連続する0のビット数
 *---------------------------------------------------------------------------*/
SDK_INLINE u32 MATH_CountTrailingZeros(u32 x)
{
    return (u32)(32 - MATH_CountLeadingZeros((u32)(~x & (x - 1))));
}

/*---------------------------------------------------------------------------*
  Name:         MATH_CTZ

  Description:  MATH_CountTrailingZeros の別名
                2進数32ビット表現で下位何ビットが0かを求める。
                MATH_CountLeadingZeros 関数を使用する。

  Arguments:    x             判定に使用する u32 の値

  Returns:      下位から連続する0のビット数
 *---------------------------------------------------------------------------*/
#define MATH_CTZ(x) MATH_CountTrailingZeros(x)

/*---------------------------------------------------------------------------*
  Name:         MATH_GetLeastSignificantBit

  Description:  2進数32ビット表現で1になっている最下位ビットを求める。

  Arguments:    x             判定に使用する u32 の値

  Returns:      1になっている最下位ビットをあらわすu32 の値
 *---------------------------------------------------------------------------*/
SDK_INLINE u32 MATH_GetLeastSignificantBit(u32 x)
{
    return (u32)(x & -(s32)x);
}

/*---------------------------------------------------------------------------*
  Name:         MATH_LSB

  Description:  MATH_GetLeastSignificantBit の別名
                2進数32ビット表現で1になっている最下位ビットを求める。

  Arguments:    x             判定に使用する u32 の値

  Returns:      1になっている最下位ビットをあらわすu32 の値
 *---------------------------------------------------------------------------*/
#define MATH_LSB(x) MATH_GetLeastSignificantBit(x)

/*---------------------------------------------------------------------------*
  Name:         MATH_GetMostSignificantBit

  Description:  2進数32ビット表現で1になっている最上位ビットを求める。

  Arguments:    x             判定に使用する u32 の値

  Returns:      1になっている最上位ビットをあらわすu32 の値
 *---------------------------------------------------------------------------*/
SDK_INLINE u32 MATH_GetMostSignificantBit(u32 x)
{
    return (u32)(x & ((s32)0x80000000 >> MATH_CountLeadingZeros(x)));
}

/*---------------------------------------------------------------------------*
  Name:         MATH_MSB

  Description:  MATH_GetMostSignificantBit の別名
                2進数32ビット表現で1になっている最上位ビットを求める。

  Arguments:    x             判定に使用する u32 の値

  Returns:      1になっている最上位ビットをあらわすu32 の値
 *---------------------------------------------------------------------------*/
#define MATH_MSB(x) MATH_GetMostSignificantBit(x)


#ifdef __cplusplus
}/* extern "C" */
#endif

/* NITRO_MATH_MATH_H_ */
#endif
