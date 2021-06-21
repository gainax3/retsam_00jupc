/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:33:28 $
 * $Revision: 1.23.2.2 $
 */
 
/*
 *	Routines
 *	--------
 *		clearerr
 *
 *		feof
 *		ferror
 *
 *		perror
 *
 *		__stdio_atexit
 */

#include "ansi_files.h"
#include "critical_regions.h"
#include <errno.h>
#include "file_io.h"
#include "misc_io.h"
#include <stdlib.h>
#include <string.h>
#include "abort_exit.h"

void _MSL_CDECL clearerr(FILE * file)
{
	file->state.eof   = 0;
	file->state.error = 0;
}

int _MSL_CDECL (feof)(FILE * file)
{
	return(feof(file));
}

int _MSL_CDECL (ferror)(FILE * file)
{
	return(ferror(file));
}

#if _MSL_OS_DISK_FILE_SUPPORT
void _MSL_CDECL perror(const char * s)
{
	char	errstr[__max_errstr];
	if (s)
		fprintf(stderr, "%s: ", s);
	
	fprintf(stderr, "%s\n", __strerror(errno, errstr));
}

void _MSL_CDECL __stdio_atexit(void)
{
	__stdio_exit = __close_all;
}
#endif


/* Change record:
 * JFH 950904 First code release.
 * JFH 951215 Changed stdio_atexit to new abort/exit handling architecture.
 * JFH 960122 Bracketed feof() and ferror() by #ifndef __cplusplus
 *								 (they are inlined in <stdio.h>).
 * JFH 960212 Tossed call to __setup_exit in __stdio_atexit.
 * JFH 960301 For Be only, __setup_exit was restored for the time being.
 *								 Their __stdio_atexit was cleaned up up a bit also.
 * SCM 970710 Disabled __stdio_atexit when __no_os is defined.
 * MEA 970720 Changed __no_os to _No_Disk_File_OS_Support.
 * cc  000326 removed dest_os to be_os 
 * cc  011203 Added _MSL_CDECL for new name mangling 
 * cc  030131 Redid check of _No_Disk_File_OS_Support in perror
 * cc  040217 Changed _No_Disk_File_OS_Support to _MSL_OS_DISK_FILE_SUPPORT
 */