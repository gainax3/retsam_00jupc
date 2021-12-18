/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/09 16:57:36 $
 * $Revision: 1.2.4.2 $
 */

/*
 *	Routines
 *	--------
 *		abort
 *
 *		atexit
 *		exit
 *
 *		__atexit
 *		__exit
 *
 *		__setup_exit
 */

#include <ansi_parms.h>
#include <abort_exit.h>
#include <critical_regions.h>
#include <misc_io.h>
#include <signal.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <pool_alloc.h>

#define max_funcs 64

int	__aborting = 0;	

static void (*__atexit_funcs[max_funcs])(void);
static long	__atexit_curr_func = 0;

void _MSL_CDECL (* __stdio_exit)  (void) = 0;
void _MSL_CDECL (* __console_exit)(void) = 0;

void _MSL_CDECL abort(void)
{
	raise(SIGABRT);
	
	__aborting = 1;
	
	exit(EXIT_FAILURE);
}

int _MSL_CDECL atexit(void (_MSL_CDECL *func)(void))
{	
	int result;
	
	__begin_critical_region(atexit_funcs_access);
	
	result = __register_atexit(func);
	
	__end_critical_region(atexit_funcs_access);
	
	return result;
}

int __atexit(void (_MSL_CDECL *func)(void))
{	
	if (__atexit_curr_func == max_funcs)
		return(-1);
	
	__begin_critical_region(atexit_funcs_access);
	
	__atexit_funcs[__atexit_curr_func++] = func;
	
	__end_critical_region(atexit_funcs_access);
	
	return 0;
}


void _MSL_CDECL exit(int status)
{
    if (!__aborting)
	{
		__destroy_global_chain(); /* need to move destroy global chain above __stdio_exit 
								     as some static objects may have destructors that flush streams */
	
		if (__stdio_exit)
		{
			(*__stdio_exit)();
			__stdio_exit = 0;
		}
	}
	
	__exit(status);
}


void _MSL_CDECL __exit(int status)
{
	#pragma unused(status)
	
	__begin_critical_region(atexit_funcs_access);
		
	while (__atexit_curr_func > 0)
		(*__atexit_funcs[--__atexit_curr_func])();
	
	__end_critical_region(atexit_funcs_access);
	
	__kill_critical_regions(); 
	
	if (__console_exit)
	{
		(*__console_exit)();
		__console_exit = 0;
	}
	
	fflush(NULL);
	
	_ExitProcess();													
}
