/* @(#)w_log.c 1.2 95/01/04 */
/* $Id: w_log.c,v 1.3 2004/02/26 17:03:30 ceciliar Exp $ */
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
 * wrapper log(x)
 */

#include "fdlibm.h"

#if _MSL_FLOATING_POINT

#ifdef __STDC__
	 double   log(double x)	/* wrapper pow */
#else
	double log(x)			/* wrapper log */
	double x;
#endif
{
#ifdef _IEEE_LIBM
	return __ieee754_log(x);
#else
	double z;
	z = __ieee754_log(x);
	if(_LIB_VERSION == _IEEE_ || isnan(x) || x > 0.0) return z;
	if(x==0.0)
	    return __kernel_standard(x,x,16); /* log(0) */
	else 
	    return __kernel_standard(x,x,17); /* log(x<0) */
#endif
}
#endif /* _MSL_FLOATING_POINT  */
