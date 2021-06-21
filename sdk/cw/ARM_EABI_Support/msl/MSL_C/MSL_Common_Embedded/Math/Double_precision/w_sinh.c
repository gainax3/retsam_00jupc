/* @(#)w_sinh.c 1.2 95/01/04 */
/* $Id: w_sinh.c,v 1.3 2004/02/26 17:03:30 ceciliar Exp $ */
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
 * wrapper sinh(x)
 */

#include "fdlibm.h"
#if _MSL_FLOATING_POINT
#ifdef __STDC__
	double sinh(double x)		/* wrapper sinh */
#else
	double sinh(x)			/* wrapper sinh */
	double x;
#endif
{
#ifdef _IEEE_LIBM
	return __ieee754_sinh(x);
#else
	double z; 
	z = __ieee754_sinh(x);
	if(_LIB_VERSION == _IEEE_) return z;
	if(!isfinite(z)&&isfinite(x)) {
	    return __kernel_standard(x,x,25); /* sinh overflow */
	} else
	    return z;
#endif
}
#endif /* _MSL_FLOATING_POINT  */
