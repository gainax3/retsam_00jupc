/* @(#)s_fabs.c 1.2 95/01/04 */
/* $Id: s_fabs.c,v 1.8.6.1 2005/07/25 20:53:35 cgalvan Exp $ */
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
 * fabs(x) returns the absolute value of x.
 */
 
#include "fdlibm.h"

#if !__COLDFIRE_HW_FPU__

#if !defined(__MIPS__)

#if _MSL_FLOATING_POINT

#ifndef __USING_INTRINSIC_FABS__  /*- cc 020729 -*/

#ifdef __STDC__
	 double   fabs(double x)	/* wrapper pow */
#else
	double fabs(x)
	double x;
#endif
{
	__HI(x) &= 0x7fffffff;
        return x;
}

#endif /* __USING_INTRINSIC_FABS__	*/ /*- cc 020729 -*/

#endif /* !defined(__MIPS__)	*/

#endif /* _MSL_FLOATING_POINT  */

#endif /*!__COLDFIRE_HW_FPU__*/