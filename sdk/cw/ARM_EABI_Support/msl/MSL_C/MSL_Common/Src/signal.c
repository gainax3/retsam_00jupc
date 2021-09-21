/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:34:03 $
 * $Revision: 1.18.2.2 $
 */

/*
 *	Routines
 *	--------
 *		signal
 *		raise
 */

#include <ansi_parms.h>

#if __dest_os != __mac_os_x

#include <critical_regions.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>

/*- ejs 011128: make these global for OS-specific use -*/
__signal_func_ptr signal_funcs[__msl_signal_max];

#if __dest_os != __win32_os /*- cc  010910 -*/

__signal_func_ptr signal(int signal, __signal_func_ptr signal_func)
{
	__signal_func_ptr	old_signal_func;
	
	if (signal < 1 || signal > __msl_signal_max)
	{
		errno = ESIGPARM;
		return(SIG_ERR);
	}
	
	__begin_critical_region(signal_funcs_access);
	
	old_signal_func = signal_funcs[signal-1];
	
	signal_funcs[signal-1] = signal_func;
	
	__end_critical_region(signal_funcs_access);
	
	return(old_signal_func);
}

#endif /* __dest_os != __win32_os */

int _MSL_CDECL raise(int signal)
{
	__signal_func_ptr	signal_func;
	
	if (signal < 1 || signal > __msl_signal_max)
		return(-1);
	
	__begin_critical_region(signal_funcs_access);
	
	signal_func = signal_funcs[signal-1];
	
	if (signal_func != SIG_IGN)
		signal_funcs[signal-1] = SIG_DFL;
	
	__end_critical_region(signal_funcs_access);
	
	if (signal_func == SIG_IGN || (signal_func == SIG_DFL && signal == SIGABRT))
			return(0);
	
	if (signal_func == SIG_DFL)
		exit(0);
	
	(*signal_func)(signal);
	
	return(0);
}

#endif /* __dest_os != __mac_os_x */

/* Change record:
 * JFH 950914 First code release.
 * JFH 951012 Moved __myraise back to unix.c
 * JFH 951215 Changed code to make SIGABRT handler of SIG_DFL to return instead of exit().
 * cc  010910 Made this signal only for !win 
 * ejs 011128 fixed bug with signal table sharing and SIGINT/SIGBREAK
 * cc  011203 Added _MSL_CDECL for new name mangling 
 * JWW 021010 Don't code signal() and raise() for Mach-O, instead use the system's version
 */