/* Metrowerks ARM Runtime Support Library 
 * Copyright © 1995-2003 Metrowerks Corporation. All rights reserved. 
 * 
 * $Date: 2004/10/04 15:22:07 $ 
 * $Revision: 1.4 $ 
 */ 

/*

FILE
	__arm_eabi_init.c

DESCRIPTION

	Use this file for C.
	
	Interface for board-level initialization and termination.
	
	If hardware initialization and pre-main user initialization are required,
	copy this file to your project directory and customize it (instead of
	customizing __start.c).
	
	TBD: targetting environments: bare board, Symbian, ARMulator.

*/

#include "file_io.h"
#include "semihosting.h"
#include "MWException.h"


/*
 *	_ExitProcess
 *
 *	MIPS EABI Runtime termination
 */

void _ExitProcess(void)
{
	sys_exit();
}


#ifndef _MAX_ATEXIT_FUNCS
	#define _MAX_ATEXIT_FUNCS 64
#endif

static DestructorChain atexit_funcs[_MAX_ATEXIT_FUNCS];
static long	atexit_curr_func = 0;

int __register_atexit(void (*func)(void))
{
	if (atexit_curr_func == _MAX_ATEXIT_FUNCS)
		return -1;
	__register_global_object(0, func, &atexit_funcs[atexit_curr_func++]);
	return 0;
}


// __init_registers, __init_hardware, __init_user suggested by Kobler 
__declspec(weak) asm void __init_registers(void)
{
	mov pc,lr
	
}

__declspec(weak) asm void __init_hardware(void)
{
	mov pc,lr
	
}

__declspec(weak) asm void __init_user(void)
{
	mov pc,lr
	
}

