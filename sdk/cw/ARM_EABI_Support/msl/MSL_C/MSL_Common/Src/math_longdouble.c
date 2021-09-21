/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:33:27 $
 * $Revision: 1.4.2.2 $
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


#if !_MSL_USES_SUN_MATH_LIB

int ilogbl(long double x)
{
	if (x == 0.0)
		return (FP_ILOGB0);
	if (isinf(x))
		return (INT_MAX);
	if (isnan(x))
		return (FP_ILOGBNAN);
	return ((int)logbl(x));
}
#endif /*!(_MSL_USES_SUN_MATH_LIB) */

#if __dest_os != __mac_os
   
_MSL_IMP_EXP_C long double _MSL_MATH_CDECL erfl(long double x) _MSL_CANT_THROW
{
	long double div;
	long double argx;
	long double coefs[6] =
	{
		0.0705230784,
		0.0422820123,
		0.0092705272,
		0.0001520143,
		0.0002765672,
		0.0000430638
	};
	int index;
	
	argx = x;
	div  = 1;
	for (index = 0; index < 6; index++)
	{
		div += argx * coefs[index];
		argx *= x;
	}
	
	for (index = 0; index < 4; index++)
		div *= div;
		
	return(1.0 - 1.0 / div);
}

_MSL_IMP_EXP_C long double _MSL_MATH_CDECL erfcl(long double x) _MSL_CANT_THROW
{
	return((long double)(1.0 - erfl(x)));
}
   
_MSL_IMP_EXP_C long double _MSL_MATH_CDECL exp2l(long double x) _MSL_CANT_THROW
{
	#define _ln2  0.6931471805599453094172321

	return(exp(x * _ln2));
}

_MSL_IMP_EXP_C long double _MSL_MATH_CDECL lgammal(long double x) _MSL_CANT_THROW
{
	return(logl(tgammal(x)));
}

_MSL_IMP_EXP_C long double _MSL_MATH_CDECL nanl(const char* str) _MSL_CANT_THROW
{
	char temparg[32] = "NAN(";
	strcat(temparg, str);
	strcat(temparg, ")");
	
	return(strtold(temparg, (char**)NULL));
}
   
_MSL_IMP_EXP_C long double _MSL_MATH_CDECL tgammal(long double x) _MSL_CANT_THROW
{
	long double coefs[26] =
	{
		+1.0000000000000000,
		+0.5772156649915329,
		-0.6558780715202538,
		-0.0420026350340952,
		+0.1665386113822915,
		-0.0421977345555443,
		-0.0096219715278770,
		+0.0072189432466630,
		-0.0011651675918591,
		-0.0002152416741149,
		+0.0001280502823882,
		-0.0000201348547807,
		-0.0000012504934821,
		+0.0000011330272320,
		-0.0000002056338417,
		+0.0000000061160960,
		+0.0000000050020075,
		-0.0000000011812746,
		+0.0000000001043427,
		+0.0000000000077823,
		-0.0000000000036968,
		+0.0000000000005100,
		-0.0000000000000206,
		-0.0000000000000054,
		+0.0000000000000014,
		+0.0000000000000001
	};
	
	long double argx;
	long double div;
	int    index;
	
	div  = 0.0;
	argx = x;
	for (index = 0; index < 26; index++)
	{
		div += argx * coefs[index];
		argx *= x;
	}
	
	return(1.0/div);
}

#endif /*__dest_os == __mac_os */

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