/* Metrowerks ARM Runtime Support Library 
 * Copyright © 1995-2003 Metrowerks Corporation. All rights reserved. 
 * 
 * $Date: 2004/05/13 15:53:40 $ 
 * $Revision: 1.1 $ 
 */ 

#ifdef __thumb

asm void __call_via_r0(void)
{
	bx		r0
}

asm void __call_via_r1(void)
{
	bx		r1
}

asm void __call_via_r2(void)
{
	bx		r2
}

asm void __call_via_r3(void)
{
	bx		r3
}

asm void __call_via_r4(void)
{
	bx		r4
}

asm void __call_via_r5(void)
{
	bx		r5
}

asm void __call_via_r6(void)
{
	bx		r6
}

asm void __call_via_r7(void)
{
	bx		r7
}

#endif