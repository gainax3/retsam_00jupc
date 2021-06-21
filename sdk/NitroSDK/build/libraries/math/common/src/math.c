/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MATH - 
  File:     math.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: math.c,v $
  Revision 1.8  2007/10/04 11:36:15  okubata_ryoma
  Copyright fix

  Revision 1.7  2007/10/03 02:34:33  seiki_masashi
  ARM9 上では thumb ライブラリでも常に CLZ 命令を呼び出すように変更

  Revision 1.6  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.5  2005/11/24 03:09:27  seiki_masashi
  MATH_CountPopulation の ARM アセンブラによる参考実装 (writtten by terui) をコメントとして追加

  Revision 1.4  2005/02/28 05:26:24  yosizaki
  do-indent.

  Revision 1.3  2005/02/18 07:12:45  seiki_masashi
  warning 対策

  Revision 1.2  2005/01/11 07:40:17  takano_makoto
  fix copyright header.

  Revision 1.1  2005/01/06 06:25:50  seiki_masashi
  ARM7用ライブラリも生成するように変更

  Revision 1.2  2004/12/15 09:17:38  seiki_masashi
  MATH_CountPopulation の追加

  Revision 1.1  2004/12/14 10:51:26  seiki_masashi
  MATH ジャンルを追加

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/math.h>

/*---------------------------------------------------------------------------*
  Name:         MATH_CountLeadingZerosFunc

  Description:  2進数32ビット表現で上位何ビットが0かを求める
                (CLZ 命令がない ARM7 or ARM9 Thumb 用)

  Arguments:    x

  Returns:      上位から連続する0のビット数
 *---------------------------------------------------------------------------*/
#if defined(SDK_ARM9) && (defined(SDK_CW) || defined(__MWERKS__))

#pragma thumb off
u32 MATH_CountLeadingZerosFunc(u32 x)
{
    // ARM9 上では、呼び出し元が Thumb だったとしても
    // ARM モードに切り替えて CLZ 命令を呼び出した方が
    // 速度もサイズも有利
    asm
    {
    clz     x, x}
    return  x;
}
#pragma thumb reset

#else // !ARM9 || !(CW || __MWERKS__)

u32 MATH_CountLeadingZerosFunc(u32 x)
{
    u32     y;
    u32     n = 32;

    // 二分探索で 0 が終わる場所を探す。
    y = x >> 16;
    if (y != 0)
    {
        n -= 16;
        x = y;
    }
    y = x >> 8;
    if (y != 0)
    {
        n -= 8;
        x = y;
    }
    y = x >> 4;
    if (y != 0)
    {
        n -= 4;
        x = y;
    }
    y = x >> 2;
    if (y != 0)
    {
        n -= 2;
        x = y;
    }
    y = x >> 1;
    if (y != 0)
    {
        n -= 2;
    }                                  // x == 0b10 or 0b11 -> n -= 2
    else
    {
        n -= x;
    }                                  // x == 0b00 or 0b01 -> n -= x

    return n;
}

#endif // ARM9 && (CW || __MWERKS__)

/*---------------------------------------------------------------------------*
  Name:         MATH_CountPopulation

  Description:  2進数32ビット表現で1のビット数を求める

  Arguments:    x

  Returns:      2進表現で1となるビット数
 *---------------------------------------------------------------------------*/
u8 MATH_CountPopulation(u32 x)
{
    // 以下、ARM コードにおいてはシフトと算術演算は同時に行えることに注意。
    // Release Build 以上ではストール無しの 13 命令 + bx lr となる。

    // 32bit を直接数えずに、最初は各 2bit 毎の 1 の数を
    // 同じ位置の 2bit に格納する。
    // すなわち、各 2bit で 00 -> 00, 01 -> 01, 10 -> 01, 11 -> 10 の変換。
    // x -> x' とすると、2bit 値では x' = x - (x >> 1)
    x -= ((x >> 1) & 0x55555555);

    // 4bit 単位で考え、上位 2bit と 2bit に格納されている 1 の数を足して
    // 最初のその位置の 4bit 中にあった 1 の数として格納する。
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);

    // 8bit 単位で同様に。
    // ただし、各桁の結果は最大で 8 であることから 4bit で収まるので
    // 事前にマスクする必要はない。
    x += (x >> 4);

    // 次の演算に備え、不要な部分をマスクする。
    x &= 0x0f0f0f0f;

    // 16bit 単位で上位 8bit と下位 8bit の和を取る。
    x += (x >> 8);

    // 32bit 単位で同様に。
    x += (x >> 16);

    // 下位 8bit の値が結果である。
    return (u8)x;
}

#if 0
// アセンブラによる参考実装
// ビットが立っていないことのほうが多い場合はこちらのほうが早い
/*---------------------------------------------------------------------------*
  Name:         MATH_CountPopulation_Asm

  Description:  値を32ビットで表した場合に 1 になっているビットの数を調べる。

  Arguments:    value   -   調査する値。

  Returns:      u32     -   ビットの数を返す。0 ～ 32 の数値。
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>
asm u32 MATH_CountPopulation_Asm(u32 value)
{
    mov     r1 ,    #0
    mov     r2 ,    #1

@loop:
    clz     r3 ,    r0
    rsbs    r3 ,    r3 ,    #31
    bcc     @end
    add     r1 ,    r1 ,    #1
    mvn     r3 ,    r2 ,    LSL r3
    and     r0 ,    r0 ,    r3
    b       @loop

@end:
    mov     r0 ,    r1
    bx      lr
}
#include <nitro/codereset.h>
#endif
