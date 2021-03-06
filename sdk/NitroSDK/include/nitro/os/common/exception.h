/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - include
  File:     exception.h

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: exception.h,v $
  Revision 1.14  2007/03/26 00:05:43  yasu
  著作年度修正

  Revision 1.13  2007/02/23 01:10:06  yosizaki
  add OS_EnableUserExceptionHandlerOnDebugger.

  Revision 1.12  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.11  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.10  2005/02/28 05:26:01  yosizaki
  do-indent.

  Revision 1.9  2004/04/07 02:03:17  yada
  fix header comment

  Revision 1.8  2004/02/19 08:44:21  yada
  ユーザ例外ルーチンの型追加

  Revision 1.7  2004/02/05 07:27:19  yada
  履歴文字列のIRISだったものを NITRO から IRISに戻した。

  Revision 1.6  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.5  2004/01/30 04:31:56  yada
  例外表示の組み込み

  Revision 1.4  2004/01/16 01:17:55  yasu
  Support SDK_BB

  Revision 1.3  2004/01/15 02:40:01  yada
  IRIS_BB → SDK_BB へ変更

  Revision 1.2  2004/01/14 12:06:08  yada
  小修正

  Revision 1.1  2003/12/08 12:22:34  yada
  初版


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_OS_EXCEPTION_H_
#define NITRO_OS_EXCEPTION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/misc.h>
#include <nitro/types.h>

//--------------------------------------------------------------------------------
//---- Exception Handler
typedef void (*OSExceptionHandler) (u32, void *);


//--------------------------------------------------------------------------------
/******************************** Exception Vector ********************************/
/*---------------------------------------------------------------------------*
  Name:         OS_SetExceptionVectorUpper

  Description:  set exception vector to hi-address

  Arguments:    none

  Returns:      None
 *---------------------------------------------------------------------------*/
void    OS_SetExceptionVectorUpper(void);

/*---------------------------------------------------------------------------*
  Name:         OS_SetExceptionVectorLower

  Description:  set exception vector to low-address

  Arguments:    none

  Returns:      None
 *---------------------------------------------------------------------------*/
void    OS_SetExceptionVectorLower(void);




/******************************** Exception Handler ********************************/
/*---------------------------------------------------------------------------*
  Name:         OS_InitException

  Description:  initialize exception system of sdk os.
                should be called once at first.

  Arguments:    none

  Returns:      None
 *---------------------------------------------------------------------------*/
void    OS_InitException(void);



/*---------------------------------------------------------------------------*
  Name:         OS_SetUserExceptionHandler

  Description:  set user exception handler

  Arguments:    handler     user routine when exception occurred
                arg         argument in call user handler

  Returns:      None
 *---------------------------------------------------------------------------*/
void    OS_SetUserExceptionHandler(OSExceptionHandler handler, void *arg);

/*---------------------------------------------------------------------------*
  Name:         OS_EnableUserExceptionHandlerOnDebugger

  Description:  enable user exception handler even if running on the debugger.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    OS_EnableUserExceptionHandlerOnDebugger(void);



#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_OS_EXCEPTION_H_ */
#endif
