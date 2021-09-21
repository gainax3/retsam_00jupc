/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:34:07 $
 * $Revision: 1.9.2.3 $
 */

#include <ansi_files.h>
#include <stdio.h>
#include <stdio.posix.h>	/* need this for fileno */

/*
 *	int fileno(FILE *stream)
 *
 *		Returns the file number associated with a stream.
 */

#if _MSL_OS_DISK_FILE_SUPPORT

int _MSL_CDECL fileno(FILE *stream) _MSL_CANT_THROW
{
	return (stream->handle);
}


int _MSL_CDECL fcloseall(void) _MSL_CANT_THROW
{
	__close_all();
	return 0;
}

#endif /* _MSL_OS_DISK_FILE_SUPPORT */

/* Change record:
 * cc  000404 Removed io.h and added stdio.h and unix.h 
 * cc  000517 Cleanup of umask return type 
 * cc  000517 Changed #include to cstdio 
 * cc  010622 Removed _ on umask and renamed posixparams to _MSL_POSIX_PARAMS
 * JWW 010927 Moved fileno to new stdio_posix.c file
 * cc  011203 Added _MSL_CDECL for new name mangling 
 * cc  020221 Added #include
 * JWW 040924 Added fcloseall (in stdio_posix.c even though it's GNU and not POSIX)
 */