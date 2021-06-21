/* @(#)s_ceil.c 1.2 95/01/04 */
/* $Id: s_ceil.c,v 1.7 2004/02/26 17:03:26 ceciliar Exp $ */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

/*
 * ceil(x)
 * Return x rounded toward -inf to integral value
 * Method:
 *	Bit twiddling.
 * Exception:
 *	Inexact flag raised if x not equal to ceil(x).
 */

#include "fdlibm.h"
#if _MSL_FLOATING_POINT

#if !( defined(__MIPS__) && __MIPS_ISA3__ && __MIPS_double_fpu__)

#ifdef __STDC__
static const double big = 1.0e300;
#else
static double big = 1.0e300;
#endif

#ifdef __STDC__
	double ceil(double x)
#else
	double ceil(x)
	double x;
#endif
{
	_INT32 i0,i1,j0; /*- cc 020130 -*/
	_UINT32 i,j; /*- cc 020130 -*/
	i0 =  __HI(x);
	i1 =  __LO(x);
	j0 = ((i0>>20)&0x7ff)-0x3ff;
	if(j0<20) {
	    if(j0<0) { 	/* raise inexact if x != 0 */
		if(big+x>0.0) {/* return 0*sign(x) if |x|<1 */
		    if(i0<0) {i0=0x80000000;i1=0;} 
		    else if((i0|i1)!=0) { i0=0x3ff00000;i1=0;}
		}
	    } else {
		i = (0x000fffff)>>j0;
		if(((i0&i)|i1)==0) return x; /* x is integral */
		if(big+x>0.0) {	/* raise inexact flag */
		    if(i0>0) i0 += (0x00100000)>>j0;
		    i0 &= (~i); i1=0;
		}
	    }
	} else if (j0>51) {
	    if(j0==0x400) return x+x;	/* inf or NaN */
	    else return x;		/* x is integral */
	} else {
	    i = ((_UINT32)(0xffffffff))>>(j0-20); /*- cc 020130 -*/
	    if((i1&i)==0) return x;	/* x is integral */
	    if(big+x>0.0) { 		/* raise inexact flag */
		if(i0>0) {
		    if(j0==20) i0+=1; 
		    else {
			j = i1 + (1<<(52-j0));
			if(j<i1) i0+=1;	/* got a carry */
			i1 = j;
		    }
		}
		i1 &= (~i);
	    }
	}
	__HI(x) = i0;
	__LO(x) = i1;
	return x;
}
#endif /* MIPS */
#endif /* _MSL_FLOATING_POINT  */