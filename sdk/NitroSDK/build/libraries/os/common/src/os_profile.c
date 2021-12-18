/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS
  File:     os_profile.c

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_profile.c,v $
  Revision 1.5  2005/03/04 11:09:27  yasu
  __declspec(weak) ‚ð SDK_WEAK_SYMBOL ‚É’u‚«Š·‚¦

  Revision 1.4  2005/02/28 05:26:29  yosizaki
  do-indent.

  Revision 1.3  2004/05/12 05:09:51  yada
  modify SDK_XXX / OS_XXX option
  make __PROFILE_ENTRY/EXIT be weak

  Revision 1.2  2004/05/10 12:11:46  yada
  OS_PROFILE -> SDK_PROFILE

  Revision 1.1  2004/05/10 11:38:37  yada
  first release

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/os.h>


#ifndef OS_PROFILE_AVAILABLE

SDK_WEAK_SYMBOL asm void __PROFILE_ENTRY( void )
{
    bx   lr
}
SDK_WEAK_SYMBOL asm void __PROFILE_EXIT( void )
{
    bx   lr
}
#endif // ifdef OS_PROFILE_AVAILABLE
