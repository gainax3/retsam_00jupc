/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:33:27 $
 * $Revision: 1.7.2.4 $
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

#if !(_MSL_USES_SUN_MATH_LIB) && __dest_os != __mac_os
#include <w_lgamma.c>
#endif


#ifndef __MWERKS__
	#define _MSL_COMPILE_FOR_SPEED 1
#else
	#if __option(optimize_for_size)
		#define _MSL_COMPILE_FOR_SPEED 0
	#else
		#define _MSL_COMPILE_FOR_SPEED 1
	#endif
#endif /* __MWERKS__ */


#if __dest_os != __mac_os

_MSL_IMP_EXP_C double _MSL_MATH_CDECL nexttoward(double x, long double y) _MSL_CANT_THROW
{
	int increase;
	int positive;
	unsigned long* lx = (unsigned long*)&x;
	unsigned long* ly = (unsigned long*)&y;
#if _MSL_LITTLE_ENDIAN
	unsigned long x_exp = lx[1] & 0x7FF00000;
	if ((ly[1] & 0x7FF00000) == 0x7FF00000 && (ly[1] & 0x000FFFFF) | ly[0] ||
	    x == y)
		return (double)y;
	if (x_exp == 0x7FF00000 && lx[1] & 0x000FFFFF | lx[0])
		return x;
#else
	unsigned long x_exp = lx[0] & 0x7FF00000;
	if ((ly[0] & 0x7FF00000) == 0x7FF00000 && (ly[0] & 0x000FFFFF) | ly[1] ||
	    x == y)
		return (double)y;
	if (x_exp == 0x7FF00000 && lx[0] & 0x000FFFFF | lx[1])
		return x;
#endif /* _MSL_LITTLE_ENDIAN */
	increase = y > x;
	positive = x > 0;
	if (x_exp != 0x7FF00000 || increase != positive)
	{
		unsigned long long* llx = (unsigned long long*)&x;
		if (x == 0)
		{
			if (increase)
				*llx = 0x0000000000000001ULL;
			else
				*llx = 0x8000000000000001ULL;
		}
		else
		{
			if (increase == positive)
				++(*llx);
			else
				--(*llx);
		}
	#if _MSL_LITTLE_ENDIAN
		x_exp = lx[1] & 0x7FF00000;
	#else
		x_exp = lx[0] & 0x7FF00000;
	#endif /* _MSL_LITTLE_ENDIAN */
		if (x_exp == 0 && (math_errhandling & MATH_ERREXCEPT))
			feraiseexcept(FE_UNDERFLOW | FE_INEXACT);
		else if (x_exp == 0x7FF00000)
		{
			if (math_errhandling & MATH_ERRNO)
				errno = ERANGE;
			if (math_errhandling & MATH_ERREXCEPT)
				feraiseexcept(FE_OVERFLOW | FE_INEXACT);
		}
	}
	return x;
}
#endif

#if !_MSL_USES_SUN_MATH_LIB


_MSL_IMP_EXP_C double _MSL_MATH_CDECL cbrt(double x) _MSL_CANT_THROW
{
	unsigned int sign;
	int exponent;
	double y;
/* 		if x is 0 or -0 return x 																	*/
	if (x == 0)
		return x;
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

/* 	extract biased exponent (x is either normal or denormal											*/
	sign = __UHI(x) & 0x80000000;
	__UHI(x) ^= sign;
	exponent = (int)(__UHI(x) >> 20);
	if (exponent == 0)  /* if x is denormal															*/
	{
		/* normalize x by shifting left until the first 1 bit is in									*/
		/* position 11																				*/
		int c = __msl_count_leading_zero32(__UHI(x)) - 11;
		if (c == 21)
			c += __msl_count_leading_zero32(__ULO(x));
		if (c < 32)
		{
			__UHI(x) = (__UHI(x) << c) | (__ULO(x) >> (32 - c));
			__ULO(x) <<= c;
		}
		else
		{
			__UHI(x) = __ULO(x) << (c - 32);
			__ULO(x) = 0;
		}
		/* decrement exponent by the number of leading 0 bits in mantissa							*/
		exponent -= c - 1;
	}

	/* unbias exponent and normalize x to [0.5, 1)													*/
	exponent -= 1022;
	__UHI(x) &= 0x000FFFFF;
	__UHI(x) |= 0x3FE00000;
	/* make exponent evenly divisible by 3, normalizing x to [0.125, 1)								*/
	switch (exponent % 3)
	{
		case -2:
		case 1:
			exponent += 2;
			__UHI(x) -= 0x00200000;
			break;
		case -1:
		case 2:
			exponent += 1;
			__UHI(x) -= 0x00100000;
			break;
	}
	/* estimate first guess to cube root of x [0.125, 1) 											*/
	/* use polynomial derived from minimizing the error  from 0.125 to 1                        	*/
	y = (0.1950252994681516 + (8.93386164028292 +  (34.95109317740758 + 14.90468137136715 * x) * x) * x) /
	    (1.0                + (17.79274627019952 + (34.47634312279896 + 5.715646840256109 * x) * x) * x);
	/* perform newton iterations to improve the estimate											*/
	y = (2 * y + x / (y*y)) / 3;
	/* put result of final estimate back in x														*/
	x = (2 * y + x / (y*y)) / 3;
	/* x is now the cube root of [0.125, 1) -> [0.5, 1)												*/
	/* complete by taking the cube root of the exponent, and loading that							*/
	/* exponent into x																				*/
	exponent /= 3;
	exponent <<= 20;
	__UHI(x) += exponent;
	/* restore original sign of x																	*/
	__UHI(x) |= sign; 
	/*x = x * sign; */
	return x;
}

_MSL_IMP_EXP_C int _MSL_MATH_CDECL ilogb(double x)
{
	if (x == 0.0)
		return (FP_ILOGB0);
	if (isinf(x))
		return (INT_MAX);
	if (isnan(x))
		return (FP_ILOGBNAN);
	return ((int)logb(x));
}

#if __dest_os != __mac_os

_MSL_IMP_EXP_C double _MSL_MATH_CDECL tgamma(double x) _MSL_CANT_THROW
{
	return((double)tgammal(x));
}

_MSL_IMP_EXP_C double _MSL_MATH_CDECL erf(double x) _MSL_CANT_THROW
{
	return((double)erfl(x));
}

_MSL_IMP_EXP_C double _MSL_MATH_CDECL erfc(double x) _MSL_CANT_THROW
{
	return((double)(1.0 - erfl(x)));
}

/*_MSL_IMP_EXP_C double _MSL_MATH_CDECL lgamma(double x) _MSL_CANT_THROW
{
	long double result2 = 0.0;				/*- mm 050510 
	long double result1 = 1.0;				/*- mm 050510 
	long double x1;							/*- mm 050510 
	
	for (x1 = x-1; x1 >= 1;  x1--)			/*- mm 050510 
		result1 *= x1;						/*- mm 050510 
	result2 = logl(result1);				/*- mm 050510 
	return(result2);
} 
*/

_MSL_IMP_EXP_C double _MSL_MATH_CDECL nextafter(double x, double y) _MSL_CANT_THROW
{
	return nexttoward(x, (long double)y);
}
#endif /* #if __dest_os == __mac_os */

#endif /*_MSL_USES_SUN_MATH_LIB */

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
 * cc  041203 Moving back nextafter from math_sun.c
 */