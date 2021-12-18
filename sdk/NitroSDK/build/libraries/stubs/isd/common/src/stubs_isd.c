/*---------------------------------------------------------------------------*
  Project:  NitroSDK - libraries - stubs
  File:     stubs_isd.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: stubs_isd.c,v $
  Revision 1.8  2006/01/18 02:12:29  kitase_hirotake
  do-indent

  Revision 1.7  2005/03/04 11:09:27  yasu
  __declspec(weak) を SDK_WEAK_SYMBOL に置き換え

  Revision 1.6  2005/02/28 05:26:32  yosizaki
  do-indent.

  Revision 1.5  2004/07/13 04:46:08  yada
  add _ISDbgLib_RegistOverlayInfo() and _ISDbgLib_UnregistOverlayInfo().
  add __declspec(weak) to all functions.

  Revision 1.4  2004/02/16 00:46:18  yasu
  add ISDPrintf

  Revision 1.3  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.2  2003/12/12 05:04:22  yasu
  _ISDbgLib_* 関数の追加

  Revision 1.1  2003/12/10 12:19:03  yasu
  ISD ライブラリの無い環境のために空の関数をダミーで用意し、stubs 下に置いた．

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

void    ISDPrintf(const char *, ...);
void    ISDPrint(const char *);
void    _ISDbgLib_Initialize(void);
void    _ISDbgLib_AllocateEmualtor(void);
void    _ISDbgLib_FreeEmulator(void);
void    _ISDbgLib_RegistOverlayInfo(void);
void    _ISDbgLib_UnregistOverlayInfo(void);

/*---------------------------------------------------------------------------*
      Dummy Function for IS Debugger Console Functions
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL void ISDPrintf(const char *, ...)
{
}

SDK_WEAK_SYMBOL void ISDPrint(const char *)
{
}

SDK_WEAK_SYMBOL void _ISDbgLib_Initialize(void)
{
}

SDK_WEAK_SYMBOL void _ISDbgLib_AllocateEmualtor(void)
{
}

SDK_WEAK_SYMBOL void _ISDbgLib_FreeEmulator(void)
{
}

SDK_WEAK_SYMBOL void _ISDbgLib_RegistOverlayInfo(void)
{
}

SDK_WEAK_SYMBOL void _ISDbgLib_UnregistOverlayInfo(void)
{
}
