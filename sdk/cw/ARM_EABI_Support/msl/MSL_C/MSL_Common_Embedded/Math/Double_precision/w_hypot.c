/* @(#)w_hypot.c 1.2 95/01/04 */
/* $Id: w_hypot.c,v 1.5 2004/02/26 17:03:30 ceciliar Exp $ */
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
 * wrapper hypot(x,y)
 */

#include "fdlibm.h"
#if _MSL_FLOATING_POINT
#if _MSL_C99  

#ifdef __STDC__
	double hypot(double x, double y)/* wrapper hypot */
#else
	double hypot(x,y)		/* wrapper hypot */
	double x,y;
#endif
{
#ifdef _IEEE_LIBM
	return __ieee754_hypot(x,y);
#else
	double z;
	z = __ieee754_hypot(x,y);
	if(_LIB_VERSION == _IEEE_) return z;
	if((!isfinite(z))&&isfinite(x)&&isfinite(y))
	    return __kernel_standard(x,y,4); /* hypot overflow */
	else
	    return z;
#endif
}
#endif /* _MSL_C99 */
#endif /* _MSL_FLOATING_POINT  */
