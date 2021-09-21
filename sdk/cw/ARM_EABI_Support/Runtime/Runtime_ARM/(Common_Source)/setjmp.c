
/* Metrowerks ARM Runtime Support Library 
 * Copyright © 1995-2003 Metrowerks Corporation. All rights reserved. 
 * 
 * $Date: 2004/05/13 15:53:35 $ 
 * $Revision: 1.1 $ 
 */ 

/*
 *	setjmp.c	-	setjmp() and longjmp() routines for ARM family
 *
 *	THEORY OF OPERATION
 *
 *	The runtime support routines __setjmp() and longjmp() support the C <setjmp.h>
 *	facilities. __setjmp() captures the state of the program in a jmp_buf data structure
 *	which has the following C definition:
 *
 *		typedef long jmp_buf[16];		 // saved registers (see below for order)
 *
 *	setjmp() and longjmp() are defined as follows:
 *
 *		int __setjmp(jmp_buf env);
 *		#define setjmp(env) __setjmp(env)
 *		void longjmp(jmp_buf env, int val);
 *
 */

#include <setjmp.h>

// BC Allow this file to exist in otherwise Thumb library for now. 
#pragma thumb off

/*
 *	__setjmp	-	C setjmp() routine
 *
 *	On entry a1 points to a jmp_buf struct. On exit, a1 is 0.
 *
 */

asm int __setjmp(register jmp_buf env)
{
		stmia	a1,{v1-v8,sp,lr}
		mov		a1,#0
		bx		lr
}


/*
 *	longjmp		-	C longjmp() routine
 *
 *	On entry a1 points to a jmp_buf struct and a2 contains the return value.
 *	On exit, a1 contains 1 if a2 was 0, otherwise it contains the value from a2.
 *
 */

asm void longjmp(register jmp_buf env, register int val)
{
		ldmia	a1,{v1-v8,sp,lr}
		movs	a1,a2
		moveq	a1,#1
		bx		lr
}

#pragma thumb reset
