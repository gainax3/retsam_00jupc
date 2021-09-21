/* @(#)e_gamma_r.c 1.2 95/01/04 */
/* $Id: e_gamma_r.c,v 1.4 2004/02/26 17:06:07 ceciliar Exp $ */
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

/* __ieee754_gamma_r(x, signgamp)
 * Reentrant version of the logarithm of the Gamma function 
 * with user provide pointer for the sign of Gamma(x). 
 *
 * Method: See __ieee754_lgamma_r
 */

#include "fdlibm.h"
#if _MSL_FLOATING_POINT
#ifdef __STDC__
	double __ieee754_gamma_r(double x, _INT32 *signgamp)	/*- cc 020130 -*/
#else
	double __ieee754_gamma_r(x,signgamp)
	double x; _INT32 *signgamp;						/*- cc 020130 -*/
#endif
{
	return __ieee754_lgamma_r(x,signgamp);
}
#endif /* _MSL_FLOATING_POINT  */
