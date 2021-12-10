/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:34:07 $
 * $Revision: 1.8.2.3 $
 */

#include <sys/stat.h>
#if _MSL_OS_DISK_FILE_SUPPORT

_MSL_POSIX_PARAMS __new_umask = 0;

/*  	if !_MSL_POSIX _MSL_POSIX_PARAMS is an int else it is of type mode_t
 *
 *		_MSL_POSIX_PARAMS _umask( _MSL_POSIX_PARAMS __pmode )
 *
 *		Sets the file permission mask so that all future
 *      files will be created to the mode specified by __pmode 
 */

_MSL_POSIX_PARAMS _MSL_CDECL umask(_MSL_POSIX_PARAMS __pmode) _MSL_CANT_THROW
{
	_MSL_POSIX_PARAMS __oldmask = __new_umask;
	
	__new_umask = __pmode;
	
	return __oldmask;
}
#endif /* _MSL_OS_DISK_FILE_SUPPORT */
/* Change record:
 * cc  000404 Removed io.h and added stdio.h and unix.h 
 * cc  000517 Cleanup of umask return type 
 * cc  000517 Changed #include to cstdio 
 * cc  010622 Removed _ on umask and renamed posixparams to _MSL_POSIX_PARAMS
 * JWW 010927 Renamed from unix.c to stat.c
 * cc  011203 Added _MSL_CDECL for new name mangling 
 * ejs 040108 Use <sys/stat.h>, not <stat.h>
 */