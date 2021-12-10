/* @(#)s_matherr.c 1.2 95/01/04 */
/* $Id: s_matherr.c,v 1.4 2004/02/26 17:03:27 ceciliar Exp $ */
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

#include "fdlibm.h"
#if _MSL_FLOATING_POINT

#ifdef __STDC__
	_INT32 matherr(struct exception *x)  /*- cc 020130 -*/
#else
	_INT32 matherr(x)					/*- cc 020130 -*/
	struct exception *x;
#endif
{
	_INT32 n=0;							/*- cc 020130 -*/
	if(x->arg1!=x->arg1) return 0;
	return n;
}

#endif /* _MSL_FLOATING_POINT  */
