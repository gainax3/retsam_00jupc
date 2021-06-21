/* Metrowerks ARM Runtime Support Library 
 * Copyright © 1995-2003 Metrowerks Corporation. All rights reserved. 
 * 
 * $Date: 2004/05/13 15:53:10 $ 
 * $Revision: 1.1 $ 
 */ 


/*
 *	ptmf.c	-	pointer-to-member-function support for ARM/Thumb
 *
 */

#include <CPlusLib.h>


/*
 *	__ptmf_null	-	a NULL pointer-to-member-function
 *
 */

const PTMF __ptmf_null = { 0, 0, 0 };


/*
 *	__ptmf_test	-	test pointer-to-member-function for null
 *
 *	A PTMF is null if all fields are 0. We return 0 if the PTMF is null, 1 otherwise.
 *
 */

long __ptmf_test(const PTMF *ptmf)
{
	if (ptmf->this_delta == 0 && ptmf->vtbl_offset == 0 && ptmf->func_data.func_addr == 0)
		return(0);

	return(1);
}


/*
 *	__ptmf_cmpr	-	compare two pointer-to-member-functions
 *
 *	Two PTMF's are equal if all fields are equal.
 *	We return 0 if the PTMFs are equal, 1 otherwise.
 *
 */

long __ptmf_cmpr(const PTMF *ptmf1, const PTMF *ptmf2)
{
	if (ptmf1->this_delta == ptmf2->this_delta
	&&	ptmf1->vtbl_offset == ptmf2->vtbl_offset
	&&	ptmf1->func_data.func_addr == ptmf2->func_data.func_addr)
		return(0);
	
	return(1);
}


/*
 *	__ptmf_cast	-	copy/cast a pointer to member function
 *
 */

PTMF *__ptmf_cast(long offset, const PTMF *ptmfrom, PTMF *ptmto)
{
	ptmto->this_delta = ptmfrom->this_delta + offset;
	ptmto->vtbl_offset = ptmfrom->vtbl_offset;
	ptmto->func_data = ptmfrom->func_data;
	return ptmto;
}


/*
 *	__ptmf_scall	-	call through a pointer-to-member-function
 *
 *	The outgoing arguments are in a1-a4; 'this' is in a1. A pointer to the
 *	PTMF is in IP. We call a member function from a PTMF like this:
 *
 *		this += ptmf->this_delta;
 *		if (ptmf->vtbl_offset >= 0) {	// virtual member function pointer
 *			vtbl = * (long *) (this + ptmf->func_data.ventry_offset);
 *			fp = * (long *) (vtbl + ptmf->vtbl_offset);
 *		}
 *		else	// nonvirtual member function pointer
 *			fp = ptmf->func_data.func_addr;
 *		jump indirect through fp;
 *	
 */

asm void __ptmf_scall(...)
{
#ifndef __thumb // ARM version

		str		r4,[sp,#-4]!				/*	save scratch register(s)	*/
		ldr		r4,[ip,#PTMF.this_delta]	/*	amount to add to 'this'	*/
		add		a1,a1,r4					/*	this += ptmf->this_delta	*/
		ldr		r4,[ip,#PTMF.vtbl_offset]	/*	-1 or offset to function in vtable	*/
		ldr		ip,[ip,#PTMF.func_data]		/*	function pointer or offset to vtable	*/
		cmp		r4,#0						/*	ptmf->vtbl_offset >= 0 ?	*/
		blt		@1
		ldr		ip,[a1,ip]					/*	get pointer to vtable	*/
		ldr		ip,[ip,r4]					/*	get pointer to member function	*/
@1		ldr		r4,[sp],#4					/*	restore scratch register(s)	*/
		mov		pc,ip						/*	jump to member function	*/

#else // Thumb version

		push	{r3,r4}						/*	save scratch register(s)	*/
		mov		r3,ip
		ldr		r4,[r3,#PTMF.this_delta]	/*	amount to add to 'this'	*/
		add		a1,a1,r4					/*	this += ptmf->this_delta	*/
		ldr		r4,[r3,#PTMF.vtbl_offset]	/*	-1 or offset to function in vtable	*/
		ldr		r3,[r3,#PTMF.func_data]		/*	function pointer or offset to vtable	*/
		cmp		r4,#0						/*	ptmf->vtbl_offset >= 0 ?	*/
		blt		@1
		ldr		r3,[a1,r3]					/*	get pointer to vtable	*/
		ldr		r3,[r3,r4]					/*	get pointer to member function	*/
@1		mov		ip,r3
		pop		{r3, r4}					/*	restore scratch register(s)	*/
		mov		pc,ip						/*	jump to member function	*/

#endif
}


/*
 *	__ptmf_scall4	-	call through a pointer-to-member-function (aggregate return type)
 *
 *	The outgoing arguments are in a1-a4; the hidden argument for the function result
 *	is in a1; 'this' is in a2. A pointer to the PTMF is in IP.  We call a member
 *	function from a PTMF like this:
 *
 *		this += ptmf->this_delta;
 *		if (ptmf->vtbl_offset >= 0) {	// virtual member function pointer
 *			vtbl = * (long *) (this + ptmf->func_data.ventry_offset);
 *			fp = * (long *) (vtbl + ptmf->vtbl_offset);
 *		}
 *		else	// nonvirtual member function pointer
 *			fp = ptmf->func_data.func_addr;
 *		jump indirect through fp;
 *
 */

asm void __ptmf_scall4(...)
{
#ifndef __thumb // ARM version

		str		r4,[sp,#-4]!				/*	save scratch register(s)	*/
		ldr		r4,[ip,#PTMF.this_delta]	/*	amount to add to 'this'	*/
		add		a2,a2,r4					/*	this += ptmf->this_delta	*/
		ldr		r4,[ip,#PTMF.vtbl_offset]	/*	-1 or offset to function in vtable	*/
		ldr		ip,[ip,#PTMF.func_data]		/*	function pointer or offset to vtable	*/
		cmp		r4,#0						/*	ptmf->vtbl_offset >= 0 ?	*/
		blt		@1
		ldr		ip,[a2,ip]					/*	get pointer to vtable	*/
		ldr		ip,[ip,r4]					/*	get pointer to member function	*/
@1		ldr		r4,[sp],#4					/*	restore scratch register(s)	*/
		mov		pc,ip						/*	jump to member function	*/

#else // Thumb version

		push	{r3,r4}						/*	save scratch register(s)	*/
		mov		r3,ip
		ldr		r4,[r3,#PTMF.this_delta]	/*	amount to add to 'this'	*/
		add		a2,a2,r4					/*	this += ptmf->this_delta	*/
		ldr		r4,[r3,#PTMF.vtbl_offset]	/*	-1 or offset to function in vtable	*/
		ldr		r3,[r3,#PTMF.func_data]		/*	function pointer or offset to vtable	*/
		cmp		r4,#0						/*	ptmf->vtbl_offset >= 0 ?	*/
		blt		@1
		ldr		r3,[a2,r3]					/*	get pointer to vtable	*/
		ldr		r3,[r3,r4]					/*	get pointer to member function	*/
@1		mov		ip,r3
		pop		{r3,r4}						/*	restore scratch register(s)	*/
		mov		pc,ip						/*	jump to member function	*/

#endif
}
