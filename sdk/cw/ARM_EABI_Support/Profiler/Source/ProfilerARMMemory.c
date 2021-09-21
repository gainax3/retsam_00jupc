/*
 *  ProfilerARMMemory.c
 *
 *  Copyright (c) 2003 Metrowerks Inc.  All rights reserved.
 *	METROWERKS CONFIDENTIAL
 *
 *	Port these routines to other platforms
 *	ratwell wuz here
 *
 */
 
#include "ProfilerIntern.h"
#include <stdlib.h>
#include <pool_alloc.h>
 
 /* ------------------------------------------------------------------
 * AllocateBuffer
 *
 * Attempt to allocate a buffer in MultiFinder Memory first, then fallback
 * to the application's heap.
 * -----------------------------------------------------------------*/

Handle AllocateBuffer(Size size, OSErr *error)
{
	Handle	h;
	
	char* buffer = (char*)malloc(size);
	
	if (buffer == NULL)
		*error = -1;
	else
		*error = 0;
	
	return buffer;
}

/* ------------------------------------------------------------------
 * ReleaseBuffer
 *
 * Release a buffer allocated by the above routine.
 * -----------------------------------------------------------------*/

void ReleaseBuffer(Handle *h)
{
	free(*h);
	*h = 0;
}

