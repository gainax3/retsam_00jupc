/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:32:08 $
 * $Revision: 1.28.2.3 $
 */

/*
 *	Routines
 *	--------
 *		__msl_assertion_failed
 */

#include <ansi_parms.h>
#include <assert.h>
#include <assert_api.h>
#include <stdio.h>
#include <stdlib.h>

void __msl_assertion_failed(char const *condition, char const *filename, char const *funcname, int lineno)
{
#if !_MSL_OS_DISK_FILE_SUPPORT

	#if _MSL_CONSOLE_SUPPORT
	#if _MSL_ASSERT_DISPLAYS_FUNC
		printf("Assertion (%s) failed in \"%s\", function \"%s\", line %d\n", condition, filename, funcname, lineno);
	#else
		printf("Assertion (%s) failed in \"%s\", line %d\n", condition, filename, lineno);						
	#endif /* _MSL_ASSERT_DISPLAYS_FUNC */
	#endif /* _MSL_CONSOLE_SUPPORT */

#else /* _MSL_OS_DISK_FILE_SUPPORT */

	#if _MSL_ASSERT_DISPLAYS_FUNC
		fprintf(stderr, "Assertion (%s) failed in \"%s\", function \"%s\", line %d\n", condition, filename, funcname, lineno);
	#else
		fprintf(stderr, "Assertion (%s) failed in \"%s\", line %d\n", condition, filename, lineno);
	#endif /* _MSL_ASSERT_DISPLAYS_FUNC*/

#endif /* _MSL_OS_DISK_FILE_SUPPORT */

abort();
}

/* Change record:
 * JFH 950612 First code release.
 * MM  950616 Added include of signal.h and code to replace abort()
 * JFH 950918 Undid previous change. abort() should behave properly now.
 * JFH 960426 Merged Win32 changes in.
 *						CTV
 * hh  980122 Replaced <windows.h> with the following four includes because it is seriously
 *            broken.  The following 4 includes must be carefully ordered as shown, because
 *            they are broken too.
 * mf  980428 added support for windows ce-- do not need to explicitly include x86_prefix.h
 *            since it is included by ansi_parms.h
 * mf  980510 added new function __whar_assertion_failed to support unicode.  This was due to
 *            microsoft's version of swprintf not being standard. the ms implementation of tchar
 *            maps tsprintf into the ms implementation of swprintf if UNICODE is defined. our implementation
 *            is different(takes an int arg to limit size of string as specified in the standard.                       
 * mf  980512 wince changes   
 * mf  980515 needed windows headers before standard headers and therefore needed to also
 *            put include of ansi_parms first so that WIN32 env. is defined before prototype
 *            of MessageBox          
 * mm  981109 Changed __assertion_failed so that when __WINSIOUX defined, responding to 
 *     		  assertion failure box does not terminate WinSIOUX.
 * mm  990419 Changes to avoid messages when warnings when #pragma const_strings on in force IL9903-1579       
 * as  000823 added #if __dest_os == __mcore_bare to __assertion_failed()
 * MEA 001107 added ... || defined(_No_Disk_File_OS_Support)  to __assertion_failed()
 * cc  010821 Added eppc to core
 * JWW 010905 Moved to using platform specific implementations of __assertion_failed
 * JWW 010905 Renamed to __msl_assertion_failed so new C99 parameters won't conflict with old libraries
 * cc  010910 Added include of stdio.h and stdlib.h
 * cc  020219 Redid the check for _No_Disk_File_OS_Support & _No_Console
 * cc  040217 Changed _No_Disk_File_OS_Support to _MSL_OS_DISK_FILE_SUPPORT
 * cc  050517 Changed assert to work with _MSL_ASSERT_DISPLAYS_FUNC
 */