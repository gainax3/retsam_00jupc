/* @(#)e_lgamma.c 1.2 95/01/04 */
/* $Id: e_lgamma.c,v 1.5 2004/11/29 17:44:25 cgalvan Exp $ */
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

/* __ieee754_lgamma(x)
 * Return the logarithm of the Gamma function of x.
 *
 * Method: call __ieee754_lgamma_r
 */

#include "fdlibm.h"
#if _MSL_FLOATING_POINT
#if _MSL_C99
extern _INT32 signgam;		/*- cc 020130 -*/

#ifdef __STDC__
	double __ieee754_lgamma(double x)
#else
	double __ieee754_lgamma(x)
	double x;
#endif
{
	return __ieee754_lgamma_r(x,&signgam);
}
#endif /* _MSL_C99 */
#endif /* _MSL_FLOATING_POINT  */
