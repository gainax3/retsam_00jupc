/*---------------------------------------------------------------------------*
  Project:  NitroSDK - stubs - cw
  File:     vsnprintf.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: vsnprintf.c,v $
  Revision 1.5  2006/01/18 02:12:38  kitase_hirotake
  do-indent

  Revision 1.4  2005/02/28 05:26:32  yosizaki
  do-indent.

  Revision 1.3  2004/03/07 23:50:24  yasu
  fix

  Revision 1.2  2004/03/07 23:49:12  yasu
  include code32.h to avoid byte access

  Revision 1.1  2004/03/05 09:09:35  yasu
  add vsnprintf stubs

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#pragma  warn_padding off
#include <stdio.h>
#pragma  warn_padding reset
#include <stdarg.h>


/*---------------------------------------------------------------------------*
  Name:         vsnprintf

  Description:  Pseudo version of vsnprintf

  Arguments:    same as vsnprintf

  Returns:      length of string
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>              // avoid byte access
int vsnprintf(char *s, size_t, const char *format, va_list)
{
    int     i = 0;
    do
    {
        s[i] = format[i];
    }
    while (format[i++]);

    return i;
}

#include <nitro/codereset.h>
