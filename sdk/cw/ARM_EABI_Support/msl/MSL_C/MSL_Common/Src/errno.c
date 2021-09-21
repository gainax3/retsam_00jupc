/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:32:46 $
 * $Revision: 1.23.2.2 $
 */

#pragma ANSI_strict off

#include <ansi_parms.h>
#include <errno.h>

#if !_MSL_THREADSAFE || !_MSL_LOCALDATA_AVAILABLE
	#if !defined(__MACH__)
		int errno;											/*- cc 011128 -*/
	#endif
	
	#if ((__dest_os == __mac_os) || (__dest_os == __mac_os_x))      /*- mm 010412 -*/
		short __MacOSErrNo;								/*- mm 010412 -*/	/*- cc 011128 -*/
	#endif															/*- mm 010412 -*/
#endif /* _MSL_THREADSAFE */

/* Change record:
 * JFH 950612 First code release.
 * CVT 960415 Add TLS support for Win32
 * mf  980512 wince changes
 * mm  010412 Added global for storing MacOS OSErr values
 * JWW 011027 Added case for Mach-O to use a single errno storage location from the BSD C
 * cc  011128 Made __tls _MSL_TLS
 * JWW 020130 Changed _MWMT to _MSL_THREADSAFE for consistency's sake
 */