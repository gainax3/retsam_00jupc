/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CP - include
  File:     sqrt.h

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: sqrt.h,v $
  Revision 1.22  2007/08/24 04:35:36  okubata_ryoma
  fix about headerfiles.

  Revision 1.21  2006/08/09 06:51:20  takano_makoto
  delete unused macro SDK_CP_NO_SAFE

  Revision 1.20  2006/01/18 02:12:29  kitase_hirotake
  do-indent

  Revision 1.19  2005/02/28 05:26:03  yosizaki
  do-indent.

  Revision 1.18  2005/02/21 12:22:44  takano_makoto
  fix SDK_CP_NO_SAFE bug.

  Revision 1.17  2004/09/02 07:45:52  yada
  only change comment

  Revision 1.16  2004/05/06 10:45:13  takano_makoto
  Modify include file.

  Revision 1.15  2004/05/06 02:56:05  takano_makoto
  Change to thread safe.

  Revision 1.14  2004/04/07 02:03:17  yada
  fix header comment

  Revision 1.13  2004/02/05 08:38:44  yada
  only modify comment

  Revision 1.12  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.11  2004/01/18 04:35:55  nishida_kenji
  renames functions(ex. CP_GetSqrtResult_8 -> CP_GetSqrtResult8)

  Revision 1.10  2004/01/18 01:24:13  nishida_kenji
  CP_IsSqrtBusy returns s32

  Revision 1.9  2004/01/17 02:28:38  nishida_kenji
  convert 'inline' to 'static inline' in header files

  Revision 1.8  2003/12/25 00:19:29  nishida_kenji
  convert INLINE to inline

  Revision 1.7  2003/12/08 12:09:56  yada
  インクルードするファイル変更

  Revision 1.6  2003/11/14 04:02:16  yada
  レジスタのアクセスマクロ REG_XXX → reg_XXX とした。

  Revision 1.5  2003/11/05 00:58:16  yada
  キャストを厳しく。

  Revision 1.4  2003/11/04 09:19:43  yada
  一部の定数に(u32)をつけた

  Revision 1.3  2003/10/30 11:42:45  yada
  レジスタの型にREGTypexx を使うようにした。

  Revision 1.2  2003/10/30 04:59:49  yasu
  レジスタ定義ファイルの変更 $IrisSDK/include/iris/hw 下

  Revision 1.1  2003/10/30 00:37:52  yada
  初版作成

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_CP_SQRT_H_
#define NITRO_CP_SQRT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/cp/context.h>

//---- sqrt calculation mode
#define CP_SQRT_32BIT_MODE         (0UL << REG_CP_SQRTCNT_MODE_SHIFT)   // 32bit mode
#define CP_SQRT_64BIT_MODE         (1UL << REG_CP_SQRTCNT_MODE_SHIFT)   // 64bit mode


//================================================================================
//            sqrt control register setting
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         CP_SetSqrtControl

  Description:  set value to sqrt control register

  Arguments:    param : value to set

  Returns:      None
 *---------------------------------------------------------------------------*/
static inline void CP_SetSqrtControl(u16 param)
{
    reg_CP_SQRTCNT = param;
}


//================================================================================
//            sqrt parameter setting
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         CP_SetSqrtImm64_NS_
                CP_SetSqrtImm32_NS_

  Description:  set value to sqrt parameter.
                not set bit-mode.
                not thread safe.

  Arguments:    param : parameter to set

  Returns:      None
 *---------------------------------------------------------------------------*/
static inline void CP_SetSqrtImm64_NS_(u64 param)
{
    *((REGType64 *)REG_SQRT_PARAM_ADDR) = param;
}
static inline void CP_SetSqrtImm32_NS_(u32 param)
{
    *((REGType32 *)REG_SQRT_PARAM_ADDR) = param;
}

/*---------------------------------------------------------------------------*
  Name:         CP_SetSqrtImm64
                CP_SetSqrtImm32

  Description:  set value to sqrt parameter.
                not set bit-mode.

  Arguments:    param : parameter to set

  Returns:      None
 *---------------------------------------------------------------------------*/
static inline void CP_SetSqrtImm64(u64 param)
{
    *((REGType64 *)REG_SQRT_PARAM_ADDR) = param;
}

static inline void CP_SetSqrtImm32(u32 param)
{
    *((REGType32 *)REG_SQRT_PARAM_ADDR) = param;
}

/*---------------------------------------------------------------------------*
  Name:         CP_SetSqrt64
                CP_SetSqrt32

  Description:  set value to sqrt parameter.
                set bit-mode.

  Arguments:    param : parameter to set

  Returns:      None
 *---------------------------------------------------------------------------*/
static inline void CP_SetSqrt64(u64 param)
{
    reg_CP_SQRTCNT = CP_SQRT_64BIT_MODE;
    CP_SetSqrtImm64_NS_(param);
}

static inline void CP_SetSqrt32(u32 param)
{
    reg_CP_SQRTCNT = CP_SQRT_32BIT_MODE;
    CP_SetSqrtImm32_NS_(param);
}


//================================================================================
//            wait to finish
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         CP_IsSqrtBusy

  Description:  check sqrt calculation finished

  Arguments:    None.

  Returns:      non-0 if busy, 0 if finish
 *---------------------------------------------------------------------------*/
static inline s32 CP_IsSqrtBusy(void)
{
    return (reg_CP_SQRTCNT & REG_CP_SQRTCNT_BUSY_MASK);
}

/*---------------------------------------------------------------------------*
  Name:         CP_WaitSqrt

  Description:  wait till sqrt calculation finish

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void CP_WaitSqrt(void)
{
    while (CP_IsSqrtBusy())
    {
    }
}

//================================================================================
//            getting result
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         CP_GetSqrtResultImm8 / 16 / 32

  Description:  read result.
                not make sure to finish.

  Arguments:    None.

  Returns:      result
 *---------------------------------------------------------------------------*/
static inline u32 CP_GetSqrtResultImm32(void)
{
    return (u32)(*((REGType32 *)REG_SQRT_RESULT_ADDR));
}
static inline u16 CP_GetSqrtResultImm16(void)
{
    return (u16)(*((REGType16 *)REG_SQRT_RESULT_ADDR));
}
static inline u8 CP_GetSqrtResultImm8(void)
{
    return (u8)(*((REGType8 *)REG_SQRT_RESULT_ADDR));
}

/*---------------------------------------------------------------------------*
  Name:         CP_GetSqrtResult8 / 16 / 32

  Description:  wait till finish, and read result.

  Arguments:    None.

  Returns:      result
 *---------------------------------------------------------------------------*/
static inline u32 CP_GetSqrtResult32(void)
{
    CP_WaitSqrt();
    return CP_GetSqrtResultImm32();
}
static inline u16 CP_GetSqrtResult16(void)
{
    CP_WaitSqrt();
    return CP_GetSqrtResultImm16();
}
static inline u8 CP_GetSqrtResult8(void)
{
    CP_WaitSqrt();
    return CP_GetSqrtResultImm8();
}

#ifdef __cplusplus
} /* extern "C" */
#endif


/* NITRO_CP_SQRT_H_ */
#endif
