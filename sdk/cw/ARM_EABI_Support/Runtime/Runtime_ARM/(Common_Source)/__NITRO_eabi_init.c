/* Metrowerks ARM Runtime Support Library 
 * Copyright © 1995-2003 Metrowerks Corporation. All rights reserved. 
 * 
 * $Date: 2004/07/13 19:27:40 $ 
 * $Revision: 1.2 $ 
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
#include <nitro/os/common/arena.h>
#include <nitro/os/common/alloc.h>

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
void __init_registers(void)
{
	
}

#define MAIN_HEAP_SIZE 0x2000
void __init_hardware(void)
{
	void *nstart;
	void *heapstart;
	OSHeapHandle handle;
	
	OS_Init();
	OS_InitArena();
	
	nstart = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 2);
	OS_SetMainArenaLo(nstart);
	
	heapstart = OS_AllocFromMainArenaLo(MAIN_HEAP_SIZE, 32);
	handle = OS_CreateHeap(OS_ARENA_MAIN, heapstart, (void *) ((u32) heapstart + MAIN_HEAP_SIZE));
	
	OS_SetCurrentHeap(OS_ARENA_MAIN, handle);
}

void __init_user(void)
{
	
}

