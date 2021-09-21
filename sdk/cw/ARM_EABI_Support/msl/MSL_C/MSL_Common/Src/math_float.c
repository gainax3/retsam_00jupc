/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:33:27 $
 * $Revision: 1.2.2.2 $
 */

#include <ansi_parms.h>

#ifndef _INSIDE_X87D_C_

#undef	_MSL_USE_INLINE
#define	_MSL_USE_INLINE 1
#undef	_MSL_INLINE
#define	_MSL_INLINE _MSL_DO_NOT_INLINE

#endif	/*_INSIDE_X87D_C_*/

#if _MSL_C99
#if _MSL_FLOATING_POINT

#include <math.h>
#include <math_api.h>
#include <float.h>
#include <fenv.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#ifndef __MWERKS__
	#define _MSL_COMPILE_FOR_SPEED 1
#else
	#if __option(optimize_for_size)
		#define _MSL_COMPILE_FOR_SPEED 0
	#else
		#define _MSL_COMPILE_FOR_SPEED 1
	#endif
#endif /* __MWERKS__ */


_MSL_IMP_EXP_C float _MSL_MATH_CDECL cbrtf(float x) _MSL_CANT_THROW
{
	double xd;
	unsigned int* ix;
	unsigned int sign;
	int exponent;
	if (x == 0)
		return x;
	ix = (unsigned int*)&x;
/* 	if x is +/- inf or nan, return x, setting errors as appropriate									*/
	if (!isfinite(x))
	{
		if (math_errhandling)
		{
			if (isnan(x))
			{
				if (math_errhandling & MATH_ERRNO)
					errno = EDOM;
				if (math_errhandling & MATH_ERREXCEPT)
					feraiseexcept(FE_INVALID);
			}
			else
			{
				if (math_errhandling & MATH_ERRNO)
					errno = ERANGE;
				if (math_errhandling & MATH_ERREXCEPT)
					feraiseexcept(FE_OVERFLOW);
			}
		}
		return x;
	}

	sign = *ix & 0x80000000;
	*ix ^= sign;
	exponent = (int)(*ix >> 23);
	if (exponent == 0)
	{
		int c = __msl_count_leading_zero32(*ix) - 8;
		*ix <<= c;
		exponent -= c - 1;
	}
	exponent -= 126;
	*ix &= 0x007FFFFF;
	*ix |= 0x3F000000;
	switch (exponent % 3)
	{
		case -2:
		case 1:
			exponent += 2;
			*ix -= 0x01000000;
			break;
		case -1:
		case 2:
			exponent += 1;
			*ix -= 0x00800000;
			break;
	}
	xd = x;
	x = (float)((0.164705865585441 + (13.2513868634597 + (115.1358553761178 +
	                         (181.9414139157457 + 41.95971194004274 * xd) * xd) * xd) * xd) /
	    (1.0               + (33.30169492280659 + (161.6940396106312 +
	                         (142.8167287366127 + 13.64061797885738 * xd) * xd) * xd) * xd));
	exponent /= 3;
	exponent <<= 23;
	*ix += exponent;
	*ix |= sign;
	return x;
}

#if __dest_os != __mac_os
  
_MSL_IMP_EXP_C float _MSL_MATH_CDECL erff(float x) _MSL_CANT_THROW
{
	return((float)erfl(x));
}

_MSL_IMP_EXP_C float _MSL_MATH_CDECL erfcf(float x) _MSL_CANT_THROW
{
	return((float)(1.0 - erfl(x)));
}

_MSL_IMP_EXP_C float _MSL_MATH_CDECL exp2f(float x) _MSL_CANT_THROW
{
	#define _ln2  0.6931471805599453094172321
	return(exp(x * _ln2));
}

_MSL_IMP_EXP_C float _MSL_MATH_CDECL lgammaf(float x) _MSL_CANT_THROW
{
	return((float)(logl(tgammal(x))));
}
	
_MSL_IMP_EXP_C float _MSL_MATH_CDECL nanf(const char* str) _MSL_CANT_THROW
{
	char temparg[32] = "NAN(";
	strcat(temparg, str);
	strcat(temparg, ")");
	
	return(strtof(temparg, (char**)NULL));
}

_MSL_IMP_EXP_C float _MSL_MATH_CDECL nextafterf(float x, float y) _MSL_CANT_THROW
{
	return nexttowardf(x, (long double)y);
}

_MSL_IMP_EXP_C float _MSL_MATH_CDECL nexttowardf(float x, long double y) _MSL_CANT_THROW
{
	int increase;
	int positive;
	unsigned long* lx = (unsigned long*)&x;
	unsigned long* ly = (unsigned long*)&y;
	unsigned long x_exp = *lx & 0x7F800000;
#if _MSL_LITTLE_ENDIAN
	if ((ly[1] & 0x7FF00000) == 0x7FF00000 && (ly[1] & 0x000FFFFF) | ly[0] ||
	    x == y)
		return (float)y;
#else
	if ((ly[0] & 0x7FF00000) == 0x7FF00000 && (ly[0] & 0x000FFFFF) | ly[1] ||
	    x == y)
		return (float)y;
#endif /* _MSL_LITTLE_ENDIAN */
	if (x_exp == 0x7F800000 && *lx & 0x007FFFFF)
		return x;
	increase = y > x;
	positive = x > 0;
	if (x_exp != 0x7F800000 || increase != positive)
	{
		if (x == 0)
		{
			if (increase)
				*lx = 0x00000001UL;
			else
				*lx = 0x80000001UL;
		}
		else
		{
			if (increase == positive)
				++(*lx);
			else
				--(*lx);
		}
		x_exp = *lx & 0x7F800000;
		if (x_exp == 0 && (math_errhandling & MATH_ERREXCEPT))
			feraiseexcept(FE_UNDERFLOW | FE_INEXACT);
		else if (x_exp == 0x7F800000)
		{
			if (math_errhandling & MATH_ERRNO)
				errno = ERANGE;
			if (math_errhandling & MATH_ERREXCEPT)
				feraiseexcept(FE_OVERFLOW | FE_INEXACT);
		}
	}
	return x;
}

_MSL_IMP_EXP_C float _MSL_MATH_CDECL tgammaf(float x) _MSL_CANT_THROW
{
	return((float)tgammal(x));
}

#endif /* #if __dest_os == __mac_os */

#endif /* _MSL_FLOATING_POINT */
#endif /* _MSL_C99 */	

/* Change record: 
 * cc  020124 Needed for __fpclassifyf and other _MSL_INLINE items
 * JWW 020212 Removed the unnecessary __cdecl stuff
 * JWW 020308 Use new _MSL_DO_NOT_INLINE macro for bringing inlined functions into the library
 * mm  021108 Added erf, nan and gamma functions
 * mm  021120 Added nexttowardl()
 * cc  021121 Removed redefined of nexttowardl.  It lives as an inline in math_XXX.h.
 * cc  021216 Added cbrt and family for Michael
 * cc  030110 Fixed so it can be added to mac os and not affect math lib
 * mm  030128 Revised cbrt() and cbrtf() based on Howard's version
 * cc  030221 Added _MSL_USES_SUN_MATH_LIB flag
 * mm  030520 Added C99 wrappers
 * JWW 030321 Added fma, fmaf, and assortied helper functions (thanks to Howard)
 * mm  030522 Added C99 wrappers
 * mm  030529 Added modf() and modff()
 * mm  030605 Added ilogb() and ilogbl()
 * ejs 030613 Standardize the __builtin_xxx types
 * cc  031019 Added Michael's round, roundf,and roundl for sun math
 * cc  030924 Added Michael's lround, lroundf, lroundl, llround, llroundf, 
 *            and llroundl for sun math
 * cc  031003 Added Michael's lrint,lrint and lrintl for sun math
 * cc  031029 Added Michael's nearbyintl, nearbyintf, nearbyint, remquo,
 *            remquol, and remquof for sun math
 * cc  031029 Changed return types of nearbyint, nearbyintf, and nearbyintl
 * cc  040204 Changed float fracpart to long double fracpart in long double versions of 
 *			  nearbyint, lrint, lround, and llround. MTWX10358
 * hh  040410 Added several more intrinsics, changed implementation of __msl_generic_count_leading_zero32
 *            and __msl_generic_count_trailing_zero32
 * cc  040504 Added _MSL_FLOATING_POINT flag check
 * cc  040518 File has been split
 */