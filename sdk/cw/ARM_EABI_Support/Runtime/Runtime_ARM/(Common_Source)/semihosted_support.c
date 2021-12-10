/* Metrowerks ARM Runtime Support Library 
 * Copyright © 1995-2003 Metrowerks Corporation. All rights reserved. 
 * 
 * $Date: 2004/12/02 22:20:51 $ 
 * $Revision: 1.2 $ 
 */ 



/*
 *	semihosted_support.c	- Semihosting cmd line, system, and time support	
 *
 */

#include "console_io.h"
#include "semihosting.h"
#include <string.h>
#include <time.h>

#define SEMIHOST_DST 0


/*
 *	sys_cmdline		-	return the cmd line from the host
 *
 */
static asm int sys_cmdline(register void * p) 
{
#ifndef __thumb // ARM version

		// save return address on stack
		str		lr,[sp,#-4]!

		mov		r1,p
		mov		r0,#SYS_GET_CMDLINE
		swi		SEMIHOSTING_SWI

		// restore return address from stack and return
		ldr		pc,[sp],#4

#else // Thumb version

		// save return address on stack
		push	{lr}

		mov		r1,p
		mov		r0,#SYS_GET_CMDLINE
		swi		SEMIHOSTING_SWI
				
		// restore return address from stack and return
		pop		{pc}

#endif 
}

/*
 * argc_argv 	-	Get argc argv from the host.
 * Requires support from the startup code to call
 * this function immediately before main.
 *
 */
int __argc_argv ( int max_args,char ** argv ) 
{
	semihost_cmdline_parms cmdline_parms;
	static char cmdline_storage[SEMIHOST_CMDLINE_MAX];
	int nargs=0;
	char *ptok;
	int rv;

	cmdline_parms.cmdline = cmdline_storage;
	cmdline_parms.len = SEMIHOST_CMDLINE_MAX;
	
	rv = sys_cmdline((void *)&cmdline_parms);
	if ( rv < 0 ) {
		return 0;
	}
	
	ptok = strtok(cmdline_parms.cmdline," ");
		
	while ( ptok != NULL && (nargs < max_args)) {
		argv[nargs] = ptok;
		nargs++;
		ptok = strtok(NULL," ");
	}
	
	return nargs;
}

/*
 *	sys_system		-	execute a system command
 *
 */
static asm int sys_system(register void * p) 
{
#ifndef __thumb // ARM version

		// save return address on stack
		str		lr,[sp,#-4]!

		mov		r1,p
		mov		r0,#SYS_SYSTEM
		swi		SEMIHOSTING_SWI

		// restore return address from stack and return
		ldr		pc,[sp],#4

#else // Thumb version

		// save return address on stack
		push	{lr}

		mov		r1,p
		mov		r0,#SYS_SYSTEM
		swi		SEMIHOSTING_SWI
				
		// restore return address from stack and return
		pop		{pc}

#endif 
}
	
/*
 *	system	-	system command
 *
 */
int	system ( char * cmd )
{

	semihost_system_parms system_parms;
	int rv;
	
	system_parms.cmd= cmd;
	system_parms.len = strlen(cmd);

	
	rv = sys_system((void *)&system_parms);
	
	return rv;
}	

/*
 * getenv - returns null as expected for bareboard
 *
 */
char * getenv(const char* name)
{
#pragma unused(name)
	
	return NULL;
}

/*
 *	sys_clock		-	execute a clock command
 *
 */
static asm int sys_clock() 
{
#ifndef __thumb // ARM version

		// save return address on stack
		str		lr,[sp,#-4]!

		mov		r1,#0
		mov		r0,#SYS_CLOCK
		swi		SEMIHOSTING_SWI

		// restore return address from stack and return
		ldr		pc,[sp],#4

#else // Thumb version

		// save return address on stack
		push	{lr}

		mov		r1,#0
		mov		r0,#SYS_CLOCK
		swi		SEMIHOSTING_SWI
				
		// restore return address from stack and return
		pop		{pc}

#endif 
}
	
/*
 *	__get_clock	-	clock command interface
 *
 */
int __get_clock ( )
{

	int rv;
	
	rv = sys_clock();
	
	return rv;
}

/*
 *	sys_time		-	execute a time command
 *
 */
static asm int sys_time() 
{
#ifndef __thumb // ARM version

		// save return address on stack
		str		lr,[sp,#-4]!

		mov		r0,#SYS_TIME
		swi		SEMIHOSTING_SWI

		// restore return address from stack and return
		ldr		pc,[sp],#4

#else // Thumb version

		// save return address on stack
		push	{lr}

		mov		r0,#SYS_TIME
		swi		SEMIHOSTING_SWI
				
		// restore return address from stack and return
		pop		{pc}

#endif 
}
	
/*
 *	__get_time	-	time command interface
 *
 */
int	__get_time ( )
{

	int rv;
	
	rv = sys_time();
	
	return rv;
}	

/*
 * __isdst - get DST setting from host
 *
 */
int __isdst()
{
    return SEMIHOST_DST;
}

/*
 * __to_gm_time - converted to GM time, not supported
 *
 */
#if !_MSL_TIME_T_AVAILABLE
typedef long time_t;
#endif
int __to_gm_time (time_t *lt)
{
    return -1;
}


	
