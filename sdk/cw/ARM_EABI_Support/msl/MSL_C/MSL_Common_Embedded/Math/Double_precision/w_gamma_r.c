/* @(#)w_gamma_r.c 1.2 95/01/04 */
/* $Id: w_gamma_r.c,v 1.4 2004/02/26 17:03:30 ceciliar Exp $ */
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
 * wrapper double gamma_r(double x, int *signgamp)
 */

#include "fdlibm.h"
#if _MSL_FLOATING_POINT
#ifdef _REENTRANT
#ifdef __STDC__
	double gamma_r(double x, _INT32 *signgamp) /* wrapper lgamma_r */ /*- cc 020130 -*/
#else
	double gamma_r(x,signgamp)              /* wrapper lgamma_r */
        double x; _INT32 *signgamp;		/*- cc 020130 -*/
#endif
{
#ifdef _IEEE_LIBM
	return __ieee754_gamma_r(x,signgamp);
#else
        double y;
        y = __ieee754_gamma_r(x,signgamp);
        if(_LIB_VERSION == _IEEE_) return y;
        if(!isfinite(y)&& isfinite(x)) {
            if(floor(x)==x&&x<=0.0)
                return __kernel_standard(x,x,41); /* gamma pole */
            else
                return __kernel_standard(x,x,40); /* gamma overflow */
        } else
            return y;
#endif
}             
#endif /* _REENTRANT */
#endif /* _MSL_FLOATING_POINT  */
