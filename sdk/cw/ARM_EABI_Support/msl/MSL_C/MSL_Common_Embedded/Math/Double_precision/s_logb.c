/* @(#)s_logb.c 1.2 95/01/04 */
/* $Id: s_logb.c,v 1.6 2004/02/26 17:03:27 ceciliar Exp $ */
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
 * double logb(x)
 * IEEE 754 logb. Included to pass IEEE test suite. Not recommend.
 * Use ilogb instead.
 */

#include "fdlibm.h"
#if _MSL_FLOATING_POINT
#if _MSL_C99

#ifdef __STDC__
	double logb(double x)
#else
	double logb(x)
	double x;
#endif
{
	_INT32 lx,ix; /*- cc 020130 -*/
	ix = (__HI(x))&0x7fffffff;	/* high |x| */
	lx = __LO(x);			/* low x */
	if((ix|lx)==0) return -1.0/fabs(x);
	if(ix>=0x7ff00000) return x*x;
	if((ix>>=20)==0) 			/* IEEE 754 logb */
		return -1022.0; 
	else
		return (double) (ix-1023); 
}
#endif /* _MSL_FLOATING_POINT  */
#endif /* _MSL_C99 */
