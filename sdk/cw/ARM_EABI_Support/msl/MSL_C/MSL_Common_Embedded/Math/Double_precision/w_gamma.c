/* @(#)w_gamma.c 1.2 95/01/04 */
/* $Id: w_gamma.c,v 1.4 2004/02/26 17:03:30 ceciliar Exp $ */
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

/* double gamma(double x)
 * Return the logarithm of the Gamma function of x.
 *
 * Method: call gamma_r
 */

#include "fdlibm.h"
#if _MSL_FLOATING_POINT
extern _INT32 signgam; /*- cc 020130 -*/

#ifdef __STDC__
	double gamma(double x)
#else
	double gamma(x)
	double x;
#endif
{
#ifdef _IEEE_LIBM
	return __ieee754_gamma_r(x,&signgam);
#else
        double y;
        y = __ieee754_gamma_r(x,&signgam);
        if(_LIB_VERSION == _IEEE_) return y;
        if(!isfinite(y)&&isfinite(x)) {
            if(floor(x)==x&&x<=0.0)
                return __kernel_standard(x,x,41); /* gamma pole */
            else
                return __kernel_standard(x,x,40); /* gamma overflow */
        } else
            return y;
#endif
}             
#endif /* _MSL_FLOATING_POINT  */
