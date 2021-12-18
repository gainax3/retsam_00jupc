/*---------------------------------------------------------------------------*
# Project:  NitroSDK - libraries - utest
  File:     utest_cui.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: utest_cui.c,v $
  Revision 1.3  2006/01/18 02:12:38  kitase_hirotake
  do-indent

  Revision 1.2  2005/10/14 10:42:33  yasu
  Warning 対策

  Revision 1.1  2005/06/24 02:19:25  yasu
  ユニットテストの追加

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include <nitro/utest.h>

static char assertMessage[UT_ASSERTMESSAGE_MAXLEN] = { '\0' };
static int assertMessageLen = 0;

/*---------------------------------------------------------------------------*
  Name:         UTi_GetMessage

  Description:  Get Assert Message
  
  Arguments:    None

  Returns:      Pointer for Message
 *---------------------------------------------------------------------------*/
char   *UTi_GetMessage(void)
{
    return assertMessage[0] ? assertMessage : "???";
}

/*---------------------------------------------------------------------------*
  Name:         UTi_ClearMessage

  Description:  Clear Assert Message Buffer
  
  Arguments:    None

  Returns:      Pointer for Message
 *---------------------------------------------------------------------------*/
void UTi_ClearMessage(void)
{
    assertMessageLen = 0;
    assertMessage[0] = '\0';
    return;
}

/*---------------------------------------------------------------------------*
  Name:         UTi_SetMessage/UTi_SetMessageWithFloat

  Description:  Set Assert Message
  
  Arguments:    None

  Returns:      Pointer for Message
 *---------------------------------------------------------------------------*/
void UTi_SetMessage(const char *filename, int linenum, const char *fmt, ...)
{
    va_list vlist;
    int     n;
    int     bufferLen;
    char   *bufferTop;

    va_start(vlist, fmt);

    bufferLen = (signed)sizeof(assertMessage) - assertMessageLen;
    bufferTop = assertMessage + assertMessageLen;
    n = OS_SNPrintf(bufferTop, (unsigned)bufferLen, "%s:%d ", filename, linenum);
    assertMessageLen += (n > bufferLen) ? bufferLen : n;

    bufferLen = (signed)sizeof(assertMessage) - assertMessageLen;
    bufferTop = assertMessage + assertMessageLen;
    n = OS_VSNPrintf(bufferTop, (unsigned)bufferLen, fmt, vlist);
    assertMessageLen += (n > bufferLen) ? bufferLen : n;

    va_end(vlist);
}

void UTi_SetMessageWithFloat(const char *filename, int linenum, const char *fmt, ...)
{
    va_list vlist;
    int     n;
    int     bufferLen;
    char   *bufferTop;

    va_start(vlist, fmt);

    bufferLen = (signed)sizeof(assertMessage) - assertMessageLen;
    bufferTop = assertMessage + assertMessageLen;
    n = OS_SNPrintf(bufferTop, (unsigned)bufferLen, "%s:%d ", filename, linenum);
    assertMessageLen += (n > bufferLen) ? bufferLen : n;

    bufferLen = (signed)sizeof(assertMessage) - assertMessageLen;
    bufferTop = assertMessage + assertMessageLen;
    n = vsnprintf(bufferTop, (unsigned)bufferLen, fmt, vlist);
    assertMessageLen += (n > bufferLen) ? bufferLen : n;

    va_end(vlist);
}

/*---------------------------------------------------------------------------*
  Name:         UTi_Printf/UTi_PrintfWithFloat

  Description:  Printf for character based user interface (console)

  Arguments:    Same as OS_Printf

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL void UTi_Printf(const char *fmt, ...)
{
#ifdef SDK_FINALROM
#pragma unused(fmt)
#else
    va_list vlist;

    va_start(vlist, fmt);
    OS_TVPrintf(fmt, vlist);
    va_end(vlist);
#endif
}

SDK_WEAK_SYMBOL void UTi_PrintfWithFloat(const char *fmt, ...)
{
#ifdef SDK_FINALROM
#pragma unused(fmt)
#else
    va_list vlist;

    va_start(vlist, fmt);
    OS_VPrintf(fmt, vlist);
    va_end(vlist);
#endif
}

SDK_WEAK_SYMBOL void UTi_PutString(const char *str)
{
#ifdef SDK_FINALROM
#pragma unused(str)
#else
    OS_PutString(str);
#endif
}
