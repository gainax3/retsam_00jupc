
/* @(#)e_gamma.c 1.2 95/01/04 */
/* $Id: e_gamma.c,v 1.4 2004/02/26 17:06:07 ceciliar Exp $ */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 *
 */

/* __ieee754_gamma(x)
 * Return the logarithm of the Gamma function of x.
 *
 * Method: call __ieee754_gamma_r
 */

#include "fdlibm.h"
#if _MSL_FLOATING_POINT
extern _INT32 signgam;		/*- cc 020130 -*/

#ifdef __STDC__
	double __ieee754_gamma(double x)
#else
	double __ieee754_gamma(x)
	double x;
#endif
{
	return __ieee754_gamma_r(x,&signgam);
}
#endif /* _MSL_FLOATING_POINT  */
