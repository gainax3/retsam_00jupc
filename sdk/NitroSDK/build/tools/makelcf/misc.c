/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - makelcf
  File:     misc.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: misc.c,v $
  Revision 1.5  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.4  2005/02/28 05:26:03  yosizaki
  do-indent.

  Revision 1.3  2004/06/30 09:27:21  yasu
  fix calling method for Free()

  Revision 1.2  2004/06/29 04:08:33  yasu
  add VBuffer

  Revision 1.1  2004/03/26 05:07:33  yasu
  support variables like as -DNAME=VALUE

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include	<stdio.h>
#include	<malloc.h>             // calloc()
#include	<stdlib.h>             // free(), exit()
#include	<string.h>             // strlen(), strcpy()
#include	<stdarg.h>             // va_start(),va_end()
#include	"misc.h"

BOOL    DebugMode = FALSE;

/*---------------------------------------------------------------------------*
 *   Memory Allocation Utilities
 *
 *	void*	Alloc( size_t size )
 *---------------------------------------------------------------------------*/

void   *Alloc(size_t size)
{
    void   *t = calloc(1, size);

    if (t == NULL)
    {
        error("Can't allocate memory.");
        exit(10);
    }
    return t;
}


void Free(void *p)
{
    void  **ptr = (void **)p;

    if (*ptr)
    {
        free(*ptr);
        (*ptr) = NULL;
    }
}


/*---------------------------------------------------------------------------*
 *   VBuffer
 *
 *	void	PutVBuffer( VBuffer* vbuf, char c )
 *---------------------------------------------------------------------------*/

void PutVBuffer(VBuffer * vbuf, char c)
{
    int     size;
    char   *tmp_buffer;

    if (vbuf->buffer == 0)
    {
        vbuf->size = VBUFFER_INITIAL_SIZE;
        vbuf->buffer = Alloc(vbuf->size);       // buffer is CALLOCed
    }
    size = strlen(vbuf->buffer);

    if (size >= vbuf->size - 1)
    {
        // Need buffer expansion
        vbuf->size *= 2;
        tmp_buffer = Alloc(vbuf->size); // buffer is CALLOCed
        strcpy(tmp_buffer, vbuf->buffer);
        Free(&vbuf->buffer);
        vbuf->buffer = tmp_buffer;
    }
    vbuf->buffer[size] = c;
    return;
}

char   *GetVBuffer(VBuffer * vbuf)
{
    return vbuf->buffer;
}

void InitVBuffer(VBuffer * vbuf)
{
    vbuf->buffer = 0;
    vbuf->size = 0;
}

void FreeVBuffer(VBuffer * vbuf)
{
    Free(&vbuf->buffer);
}

/*---------------------------------------------------------------------------*
 *  Debug print
 *	void   debug_printf( char* fmt, ... )
 *---------------------------------------------------------------------------*/

void debug_printf(const char *fmt, ...)
{
    va_list va;

    if (DebugMode)
    {
        va_start(va, fmt);
        vfprintf(stderr, fmt, va);
        va_end(va);
    }
}
