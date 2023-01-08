/* Metrowerks ARM Runtime Support Library 
 * Copyright © 1995-2003 Metrowerks Corporation. All rights reserved. 
 * 
 * $Date: 2004/10/29 21:53:16 $ 
 * $Revision: 1.2 $ 
 */ 



/*
 *	semihosted_console_io.c	-	redirect console I/O through ARM semihosting interface
 *
 */

#include "console_io.h"
#include "semihosting.h"





/*
 *	sys_writec		-	write a character to the console via semihosting interface
 *
 */

static asm void sys_writec(register void *cp)
{
#ifndef __thumb // ARM version

		// save return address on stack
		str		lr,[sp,#-4]!

		mov		r1,cp
		mov		r0,#SYS_WRITEC
		swi		SEMIHOSTING_SWI

		// restore return address from stack and return
		ldr		pc,[sp],#4

#else // Thumb version

		// save return address on stack
		push	{lr}

		mov		r1,cp
		mov		r0,#SYS_WRITEC
		swi		SEMIHOSTING_SWI

		// restore return address from stack and return
		pop		{pc}

#endif 
}


/*
 *	sys_write0		-	write a nul-terminated string to the console via semihosting interface
 *
 */

static asm void sys_write0(register void *str)
{
#ifndef __thumb // ARM version

		// save return address on stack
		str		lr,[sp,#-4]!

		mov		r1,str
		mov		r0,#SYS_WRITE0
		swi		SEMIHOSTING_SWI

		// restore return address from stack and return
		ldr		pc,[sp],#4

#else // Thumb version

		// save return address on stack
		push	{lr}

		mov		r1,str
		mov		r0,#SYS_WRITE0
		swi		SEMIHOSTING_SWI

		// restore return address from stack and return
		pop		{pc}

#endif 
}


/*
 *	sys_readc		-	read a character from the console via semihosting interface
 *
 */

static asm int sys_readc(void)
{
#ifndef __thumb // ARM version

		// save return address on stack
		str		r14,[r13,#-4]!
		
		mov		r1,#0
		mov		r0,#SYS_READC
		swi		SEMIHOSTING_SWI	

		// restore return address from stack and return
		ldr		pc,[sp],#4

#else // Thumb version

		// save return address on stack
		push	{lr}

		mov		r1,#0
		mov		r0,#SYS_READC
		swi		SEMIHOSTING_SWI

		// restore return address from stack and return
		pop		{pc}

#endif 
}


/*
 *	sys_exit		-	terminate an application via semihosting interface
 *
 */

asm void sys_exit()
{
		mov		r1,#0
		mov		r0,#SYS_EXIT
		swi		SEMIHOSTING_SWI
		mov		pc,lr
}


/*
 *	__read_console	-	read from the console into 'buffer' until end-of-line or 'count' characters have been read
 *
 */

int __read_console(__std(__file_handle) handle, unsigned char *buffer, __std(size_t) *count, __std(__ref_con) ref_con)
{
	#pragma unused(handle, ref_con)
	__std(size_t) i, n;
	int c;

	for (i = 0, n = *count; i < n; i++) {
		buffer[i] = sys_readc();
		/*	Metrowerks debugger returns '\r', while ARM debugger returns '\n',
			for 'return' key */
		if (buffer[i] == '\r' || buffer[i] == '\n')
		{
			*count = i + 1;
			break;
		}
	}
	
	return(__no_io_error);
}


/*
 *	__write_console	-	write 'count' bytes from 'buffer' to the console
 *
 */

int	__write_console(__std(__file_handle) handle, unsigned char *buffer, __std(size_t) *count, __std(__ref_con) ref_con)
{
	#pragma unused(handle, ref_con)
	__std(size_t) i, n;
	
	for (i = 0, n = *count; i < n; i++)
		sys_writec(&buffer[i]);

	return(__no_io_error);
}


/*
 *	__close_console	-	close the console
 *
 */

int __close_console(__std(__file_handle) handle)
{
	#pragma unused(handle)

	return(__no_io_error);
}

/* Change record:
 * cc  Use generic reference constant instead of specific idle_proc in file I/O
 */