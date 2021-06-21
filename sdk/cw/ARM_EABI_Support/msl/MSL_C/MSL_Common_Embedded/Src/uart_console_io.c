/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/09 16:59:53 $
 * $Revision: 1.19.2.2 $
 */
 
 /*
  *	Routines
  *	--------
  *		__read_console
  *		__write_console
  *		__close_console
  *
  *	Implementation
  *	--------------
  *
  *		These routines implement low-level console IO routines using
  *		any UART library which conforms to the MetroTRK UART library
  *		interface.  They are intended to provide basic console IO
  *		services in an embedded environment which lacks OS console
  *		support.
  */
#pragma ANSI_strict off	/* _No_Console will be empty file */

#if _MSL_CONSOLE_SUPPORT

#if defined(__dest_os) && defined(__dolphin_os) && __dest_os==__dolphin_os
#include <console_io.h>
#include <file_io.h>
#endif

#include "UART.h"
#include "console_io.h"
#include "uart_console_config.h"
#include "stdio.h"

#if defined(__dest_os) && defined(__dolphin_os) && __dest_os==__dolphin_os

#define DEVKIT_NO_CONSOLE_MASK	(0x20000000)

/* 
 * Enable the host io and serial IO output functions
 * added in GCN 1.3.
 */
#include <dolphin/os.h>
#define __GCN_1_3_COMPATIBILITY__ 1
#endif

/* 
 * This is defined in the TRK library for GAMECUBE
 * It is used to check if this message should be 
 * sent to TRK. If not connected, the message would
 * not be handled, and hence a exception would occur
 */
 
int		__TRK_write_console	  (__file_handle handle, unsigned char * buffer, 
							size_t * count, __idle_proc idle_proc);

static UARTError __init_uart_console(void);


/************************************************************************/
#if  !__GCN_1_3_COMPATIBILITY__

/*#pragma overload int __read_console(__file_handle handle, unsigned char * buffer, size_t * count, __idle_proc idle_proc);*/

int __read_console(__file_handle handle, unsigned char * buffer, size_t * count, __ref_con ref_con)
{
#pragma unused(handle,ref_con)

	size_t limit;
	UARTError err;

	if (__init_uart_console() != kUARTNoError)
		return( __io_error );

	/*
	** A console read should return when it encounters a line-ending
	** or after it has read the requested number of characters.
	*/

	limit = *count;
	*count = 0;
	err = kUARTNoError;

	while( *count < limit && err == kUARTNoError ) /*- cc 020724 -*/
	{
		/* Changed this to ReadUARTN and set the count to 1
		 * since it is the same functionality as ReadUART1,
		 * but ReadUART1 broke end user's implementations
		 */
		err = ReadUARTN( (char *)buffer, 1 );
#if __PPC_EABI__
        /* eppc wants backspace handled here */
		if (*buffer == '\b')
		{
			if (*count != 0)
			{
				--(*count);
				--buffer;	
			}				
			continue;	
		}
#endif
		++(*count);
		if (*buffer == '\n')
			break;
			
		if (*buffer == '\r')
			*buffer = '\n';
			break;	
			
		++buffer;
	}

	return( (err == kUARTNoError) ? __no_io_error : __io_error );
}

#endif

/************************************************************************/

int __write_console(__file_handle handle, unsigned char * buffer, size_t * count, __ref_con ref_con)
{
#pragma unused(handle,ref_con)


	// skip the initialization if this is a TDEV 
	// Note there is no OS mask for TDEV or
	// no console IO currently.
	// sw 02102003
	
#if defined(__dest_os) && defined(__dolphin_os) && __dest_os==__dolphin_os
	if ( !(OSGetConsoleType() & DEVKIT_NO_CONSOLE_MASK ))
	{
#endif
	if (__init_uart_console() != kUARTNoError)
		return( __io_error );

	if (WriteUARTN( buffer, *count ) != kUARTNoError)
	{
		*count = 0;
		return( __io_error );
	}
		
#if defined(__dest_os) && defined(__dolphin_os) && __dest_os==__dolphin_os
	}
#endif

#if __GCN_1_3_COMPATIBILITY__
	/* 
	 * If TRK is connected, send a message out to it as well  
	 * as to the UART. This uses the mslsupp.c file supplied 
	 * with TRK that knows how to interface with TRK in order
	 * to do serial and host i/o.
	 * 
	 * NOTE this requires a TRK with a) framing enabled
	 * and b) this function implemented that only starts up
	 * when a connect message is received. Otherwise TRK
	 * will get a trap (twi) instruction before it has even
	 * been brought up, and hence the app will crash.
	 */

	__TRK_write_console (handle, buffer, count, idle_proc);
#endif
	
	return( __no_io_error );
	
}


/************************************************************************/
#pragma overload int __close_console(__file_handle handle);

int		__close_console	  (__file_handle handle)
{
	#pragma unused( handle )

	return( __no_io_error );
}



/************************************************************************/
UARTError __init_uart_console(void)
{
	UARTError err = kUARTNoError;
	static int initialized = 0;
	
	if (initialized == 0)
	{
		err = InitializeUART( UART_CONSOLE_BAUD_RATE );
		
		if (err == kUARTNoError)
			initialized = 1;
	}
	
	return( err );
}

#endif /* _MSL_CONSOLE_SUPPORT */

#if __GCN_1_3_COMPATIBILITY__ && !_MSL_OS_DISK_FILE_SUPPORT

/*
 * Usually unimplemented
 */
 
#pragma overload int __delete_file (const char * name);
 
int __delete_file (const char * name)
{
	return kUARTNoError;
}

/*
 * Usually unimplemented
 */

#pragma overload int __rename_file (const char * old_name, const char * new_name);

int __rename_file (const char * old_name, const char * new_name)
{
	return kUARTNoError;
}

/*
 * Usually unimplemented
 */
#pragma overload void __temp_file_name	(char * name_str, void * ptr);

void __temp_file_name	(char * name_str, void * ptr)
{
	if ( name_str )
	{
		name_str[0] = 0;
	}
}

#endif

/*	Change Record
 *	09-Jul-97 SCM	Created
 *	20-Aug-99 SCM	Enhanced __read_console() to return when it receives an EOL.
 *	29-Nov-99 SW	Changed ReadUART1 to ReadUARTN and set the count to 1 since it
 *					is the same functionality as ReadUART1, but ReadUART1 broke
 *					end user's implementations (SCM - copied comment here and
 *					checked in).
 * 020130 cc  Added changes for e68K group
 * 020131 cc  Removed changes that the e68k group did
 * 020724 cc  Added in fix given to eppc
 * 030110 ejs Add #pragma overload (merge from EPPC) and hide file IO defs unless GCN
 * cc  040217 Changed _No_Disk_File_OS_Support to _MSL_OS_DISK_FILE_SUPPORT
 * 040528 sl  Improved guarding of __dolphin_os to allow building without prefix and 
 *            relax ANSI extension to prepare for empty source file in no console
 */