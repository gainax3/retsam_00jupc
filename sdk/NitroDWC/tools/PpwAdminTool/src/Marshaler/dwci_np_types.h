/*---------------------------------------------------------------------------*
  Project:  RevolutionDWC public include file
  File:     ./nonport/pc/dwci_np_types.h

  Copyright 2005-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/

#ifndef DWCI_NP_TYPES_H_
#define DWCI_NP_TYPES_H_

// Endian
#define SDK_LITTLE_ENDIAN
#define SDK_IS_LITTLE_ENDIAN	1
#define SDK_IS_BIG_ENDIAN	0

#ifdef  SDK_ASM
#else  //SDK_ASM

#ifdef __cplusplus
extern "C"
{
#endif

    typedef unsigned char u8;
    typedef unsigned short int u16;
    typedef unsigned long u32;

#ifdef WIN32
    typedef unsigned __int64 u64;
#else
typedef unsigned long long int u64;
#endif

    typedef signed char s8;
    typedef signed short int s16;
    typedef signed long s32;

#ifdef WIN32
    typedef signed __int64 s64;
#else
typedef signed long long int s64;
#endif

    typedef volatile u8 vu8;
    typedef volatile u16 vu16;
    typedef volatile u32 vu32;
    typedef volatile u64 vu64;

    typedef volatile s8 vs8;
    typedef volatile s16 vs16;
    typedef volatile s32 vs32;
    typedef volatile s64 vs64;

    typedef float f32;
    typedef volatile f32 vf32;

    /*
    io_register_list_XX.hで使用するマクロと型
    */

    typedef u8 REGType8;
    typedef u16 REGType16;
    typedef u32 REGType32;
    typedef u64 REGType64;

    typedef vu8 REGType8v;
    typedef vu16 REGType16v;
    typedef vu32 REGType32v;
    typedef vu64 REGType64v;


#ifndef SDK_BOOL_ALREADY_DEFINED_
#ifndef BOOL
    typedef int BOOL;
#endif //BOOL
#endif //SDK_ALREAD_DEFINE_BOOL_

#ifndef TRUE
    // Any non zero value is considered TRUE
#define TRUE                    1
#endif //TRUE

#ifndef FALSE
#define FALSE                   0
#endif // FALSE


#ifndef NULL
#ifdef  __cplusplus
#define NULL                    0
#else  // __cplusplus
#define NULL                ((void *)0)
#endif // __cplusplus
#endif // NULL

    // for compatibility with GAMECUBE
#if     defined(SDK_CW) || defined(__MWERKS__)
#ifndef ATTRIBUTE_ALIGN
#define ATTRIBUTE_ALIGN(num) __attribute__ ((aligned(num)))
#endif
#endif

    // Weak symbol
#if     defined(SDK_CW) || defined(__MWERKS__)
#define SDK_WEAK_SYMBOL  __declspec(weak)
#elif   defined(SDK_PRODG)
#define SDK_WEAK_SYMBOL
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif //SDK_ASM

/* NITRO_TYPES_H_ */
#endif


/* static inline を inline にして、コードサイズを削減したい場合は、マクロ SDK_INLINE を変更してください。
   (static inline を inline にした関数に宣言がある場合は、SDK_DECL_INLINE も変更してください。)
   変更した場合はライブラリを再ビルドしてください。

   ただし、inline 関数の実体化の不具合が修正されたバージョンのコンパイラを使用していて
   その不具合を回避するために inline を static inline にしていた関数の場合のみ
   このマクロを使用して static inline を inline に変更するとコードサイズを削減することができます。

   注意：このマクロは現段階では、FX_, VEC_, MTX_ シリーズのみ有効となっています。*/
#define SDK_INLINE      static inline
#define SDK_DECL_INLINE static

/// 1970年からの経過秒数
typedef s64 DWCUtcTime;

//---- unit of tick
typedef u32 DWCTick;

// C標準のtmと互換性のある型
typedef struct
{
    int	tm_sec;
    int	tm_min;
    int	tm_hour;
    int	tm_mday;
    int	tm_mon;
    int	tm_year;
    int	tm_wday;
    int	tm_yday;
    int	tm_isdst;
}
DWCTmTime;




