/* Metrowerks ARM Runtime Support Library 
 * Copyright © 1995-2002 Metrowerks Corporation. All rights reserved. 
 * 
 * $Date: 2004/10/29 21:53:16 $ 
 * $Revision: 1.5 $ 
 */ 


/*
 *	startup.c	-	entry-point for ARM programs
 *
 */
#include <string.h>
#include <stdlib.h>
#include "Runtime_configuration.h"

#if SUPPORT_SEMIHOST_ARGC_ARGV
#define __MAX_CMDLINE_ARGS 10
static char *argv[__MAX_CMDLINE_ARGS] = { 0 };
#else
static char *argv[] = { 0 };
#endif
	
extern int __argc_argv(char **, int);
extern char __SP_INIT[];
extern void __call_static_initializers(void);
extern int main(int, char **);

extern void _fp_init(void);

extern asm void __init_registers();
extern asm void __init_hardware();
extern asm void __init_user();

#if SUPPORT_ROM_TO_RAM
extern void __copy_rom_sections_to_ram(void);
extern char __S_romp[];
#endif

static void zero_fill_bss(void)
{
	extern char __START_BSS[];
	extern char __END_BSS[];
	
	memset(__START_BSS, 0, (__END_BSS - __START_BSS));
}

#ifdef __thumb // Thumb version
static void thumb_startup(void);
asm void thumb_startup(void)
{
		// Setup registers
		bl 	__init_registers


		// setup hardware
		bl __init_hardware
	
		//	setup the stack before we attempt anything else
		lda		r0,__SP_INIT
		// mtwx14319 - skip stack setup if __SP_INIT is 0
		// assume sp is already setup.
		cmp		r0,#0
		beq		skip_sp
		mov		sp,r0
		sub		sp,#4
		mov		r0,#0
		mvn		r0,r0
		str		r0,[sp,#0]
		add		sp,#4
skip_sp:
	
		//	zero-fill the .bss section
		bl		zero_fill_bss

#if SUPPORT_ROM_TO_RAM
		lda 	r0, __S_romp
		cmp		r0,#0
		beq		over
		bl		__copy_rom_sections_to_ram
over:	
#endif

		
		//      initialize the floating-point library
		bl      _fp_init
	
		//	call C++ static initializers
		bl		__call_static_initializers
		
		// initializations before main, user specific
		bl 		__init_user

		
		//	call main(argc, &argv)
#if SUPPORT_SEMIHOST_ARGC_ARGV
		lda		r1,argv
		mov		r0,#__MAX_CMDLINE_ARGS	// size of argv array
		bl		__argc_argv
		lda		r1,argv	// reload r1 w/ addr of argv
						// r0 is correct w/ rtn from argc_argv
#else
		mov		a1,#0
		lda		a2,argv
#endif	
		bl		main
		
		//	exit the program
		mov		a1,#0
		bl		exit
		
		//	should never get here
@1:		b		@1
}
#endif

#pragma thumb off
void __startup(void);
asm void __startup(void)
{
#ifndef __thumb // ARM version

		// Setup registers
		bl 	__init_registers


		// setup hardware
		bl __init_hardware
		
		// ----- access to memory is save now
		
		//	setup the stack before we attempt anything else
		lda		sp,__SP_INIT
		// mtwx14319 - skip stack setup if __SP_INIT is 0
		// assume sp is already setup.
		cmp		sp,#0
		beq		skip_sp
		mvn		r0,#0
		str		r0,[sp,#-4]!
skip_sp:
	
	
		// ----- stack is set up, you can call C functions now

		//	zero-fill the .bss section
		bl		zero_fill_bss

#if SUPPORT_ROM_TO_RAM
		lda 	r0, __S_romp
		cmp		r0,#0
		blne	__copy_rom_sections_to_ram	
#endif

		
		//      initialize the floating-point library
		bl      _fp_init
	
		//	call C++ static initializers
		bl		__call_static_initializers
		
		// initializations before main, user specific
		bl 		__init_user
		
		//	call main(argc, &argv)

#if SUPPORT_SEMIHOST_ARGC_ARGV
		lda		r1,argv
		mov		r0,#__MAX_CMDLINE_ARGS	// size of argv array
		bl		__argc_argv
		lda		r1,argv	// reload r1 w/ addr of argv
						// r0 is correct w/ rtn from argc_argv
#else
		mov		a1,#0
		lda		a2,argv
#endif			
		bl		main
		
		//	exit the program
		mov		a1,#0
		bl		exit
		
		//	should never get here
@1:		b		@1

#else // Thumb version

		//	we cannot start in Thumb mode, so call thumb_startup
		//	via interworking branch
		lda		r0,thumb_startup
		bx 		r0

#endif
}
#pragma thumb reset 

