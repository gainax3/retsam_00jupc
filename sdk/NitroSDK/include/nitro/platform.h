/*---------------------------------------------------------------------------*
  Project:  NitroSDK - include
  File:     platform.h

  Copyright 2006,2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: platform.h,v $
  Revision 1.3  2007/08/22 05:22:32  yosizaki
  fix dependency.

  Revision 1.2  2007/02/20 00:28:10  kitase_hirotake
  indent source

  Revision 1.1  2006/06/27 08:21:58  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NITRO_PLATFORM_HEADER__
#define NITRO_PLATFORM_HEADER__


// for OS_DisableInterrupts()
#include <nitro/os/common/system.h>


/*****************************************************************************/
/* 共通の定義 */

/* エンディアンまたはビット順序の定義 */
#define PLATFORM_ENDIAN_BIG     0
#define PLATFORM_ENDIAN_LITTLE  1


/*****************************************************************************/
/* プラットフォーム固有の定義 */

/* エンディアン (PLATFORM_ENDIAN_*) */
#define PLATFORM_BYTES_ENDIAN           PLATFORM_ENDIAN_LITTLE

/* データアクセス時にアクセス幅での境界整合が必要なら 1, そうでないなら 0 */
#define PLATFORM_BYTES_ALIGN            1

/* プログラムが意識する必要のあるキャッシュラインのサイズ (不要なら1) */
#define PLATFORM_CACHE_SIZE             32

/* キャッシュラインの境界整合が必要な変数に指定する修飾子 (不要なら空の定義) */
#define PLATFORM_ATTRIBUTE_CACHEALIGN   ATTRIBUTE_ALIGN(32)

/* ブロックの先頭で呼び出し可能な割り込み禁止関数 */
#define PLATFORM_ENTER_CRITICALSECTION()    OSIntrMode bak_interrupt_mode_ = OS_DisableInterrupts()

/* 上記関数と対になる割り込み解除関数 */
#define PLATFORM_LEAVE_CRITICALSECTION()    (void)OS_RestoreInterrupts(bak_interrupt_mode_)


/*****************************************************************************/
/* 処理系固有の定義 */

#if	defined(SDK_CW) || defined(__MWERKS__)

/* ビットフィールドが詰められる順序 (PLATFORM_ENDIAN_*) */
#define PLATFORM_BITFIELDS_ENDIAN       PLATFORM_ENDIAN_LITTLE

/* コンパイル時の正当性判定マクロ */
#define PLATFORM_COMPILER_ASSERT(expr) \
    extern void platform_compiler_assert ## __LINE__ (char is[(expr) ? +1 : -1])

/*
 * 構造体が最大メンバサイズより大きな値でパディングされないよう
 * 明示的に構造体定義の } 以降へ指定する修飾子.
 */
#define PLATFORM_STRUCT_PADDING_FOOTER

/* 関数のインライン指定子 */
#define PLATFORM_ATTRIBUTE_INLINE       SDK_INLINE


#else
# TO BE DEFINED
#endif


/*****************************************************************************/
/* 共通の型定義 */

/*
 * サイズ指定整数構造体.
 *
 * これらは外部デバイスや通信路を経由して交換される整数データに対して
 * 不用意な直接アクセスの禁止を表明する目的で組み込み型のかわりに使用される.
 * 組み込み型からこの構造体へ格納する際には MI_Store* などの関数を,
 * この構造体から値を参照する際には MI_Load* などの関数を使用する.
 */

/* 8bit リトルエンディアン型 */
typedef struct PLATFORM_LE8
{
    unsigned char byte[1];
}
PLATFORM_STRUCT_PADDING_FOOTER PLATFORM_LE8;

/* 16bit リトルエンディアン型 */
typedef struct PLATFORM_LE16
{
    unsigned char byte[2];
}
PLATFORM_STRUCT_PADDING_FOOTER PLATFORM_LE16;

/* 32bit リトルエンディアン型 */
typedef struct PLATFORM_LE32
{
    unsigned char byte[4];
}
PLATFORM_STRUCT_PADDING_FOOTER PLATFORM_LE32;

/* 8bit ビッグエンディアン型 */
typedef struct PLATFORM_BE8
{
    unsigned char byte[1];
}
PLATFORM_STRUCT_PADDING_FOOTER PLATFORM_BE8;

/* 16bit ビッグエンディアン型 */
typedef struct PLATFORM_BE16
{
    unsigned char byte[2];
}
PLATFORM_STRUCT_PADDING_FOOTER PLATFORM_BE16;

/* 32bit ビッグエンディアン型 */
typedef struct PLATFORM_BE32
{
    unsigned char byte[4];
}
PLATFORM_STRUCT_PADDING_FOOTER PLATFORM_BE32;


/*****************************************************************************/


#endif /* NINTENDO_PLATFORM_HEADER__ */
