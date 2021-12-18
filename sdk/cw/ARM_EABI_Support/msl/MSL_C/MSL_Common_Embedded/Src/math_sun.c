/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/09 16:59:52 $
 * $Revision: 1.5.2.3 $
 */
 
#include <ansi_parms.h>

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

#if _MSL_C99 || _MSL_USES_SUN_MATH_LIB


_MSL_IMP_EXP_C double _MSL_MATH_CDECL scalbn(double x, int n)
{
	double mant;
	int    exp;
	double result;
	 
	mant = frexp(x, &exp);
	exp += n;
	result = ldexp(mant, exp);
	
	return(result);
}

#endif

#if _MSL_C99
#if _MSL_USES_SUN_MATH_LIB

_MSL_IMP_EXP_C	int _MSL_MATH_CDECL isgreater(double x, double y) _MSL_CANT_THROW 
{ 
	return (__msl_relation(x, y) == 0); 
}

_MSL_IMP_EXP_C	int _MSL_MATH_CDECL isgreaterequal(double x, double y) _MSL_CANT_THROW 
{ 
	return (__msl_relation(x, y) % 2 == 0); 
}

_MSL_IMP_EXP_C	int _MSL_MATH_CDECL isless(double x, double y) _MSL_CANT_THROW 
{ 
	return (__msl_relation(x, y) == 1); 
}

_MSL_IMP_EXP_C	int _MSL_MATH_CDECL islessequal(double x, double y) _MSL_CANT_THROW 
{ 
	return ((__msl_relation(x, y)+1)/2 == 1);
} 

_MSL_IMP_EXP_C	int _MSL_MATH_CDECL islessgreater(double x, double y) _MSL_CANT_THROW 
{ 
	return (__msl_relation(x, y) <= 1); 
}

_MSL_IMP_EXP_C	int _MSL_MATH_CDECL isunordered(double x, double y) _MSL_CANT_THROW 
{ 
	return (__msl_relation(x, y) == 3); 
}


_MSL_IMP_EXP_C double _MSL_MATH_CDECL exp2(double x) _MSL_CANT_THROW
{
	#define _ln2  0.6931471805599453094172321
	return(exp(x * _ln2));
}

#if !defined(__arm)
_MSL_IMP_EXP_C long long _MSL_MATH_CDECL llrint(double x) _MSL_CANT_THROW
{
	return ((long long)rint(x));
}
#endif /* !defined(__arm) */


_MSL_IMP_EXP_C long long int _MSL_MATH_CDECL llround(double num)
{
	long long int result = 0;
	int   neg = 0;
	long int intpart;   
	double  fracpart;
	
	if (!isfinite(num))
		return(0);
	if (num < 0.0)
	{
		neg = 1;
		num = -num;
	}
	intpart = (long int)num;
	fracpart = num - (float)intpart;
	result = num - fracpart;
	if (fracpart >= 0.5)
		result += 1;
	if (neg)
		result = -result;
	return(result);
}

_MSL_IMP_EXP_C long long int _MSL_MATH_CDECL llroundf(float num)
{
	long long int result = 0;
	int   neg = 0;
	long int intpart;   
	float  fracpart;
	
	if (!isfinite(num))
		return(0);
	if (num < 0.0)
	{
		neg = 1;
		num = -num;
	}
	intpart = num;
	fracpart = num - (float)intpart;
	result = num - fracpart;
	if (fracpart >= 0.5)
		result += 1;
	if (neg)
		result = -result;
	return(result);
}

_MSL_IMP_EXP_C long long int _MSL_MATH_CDECL llroundl(long double num)
{
	long long int result = 0;
	int   neg = 0;
	long long int intpart;   
	long double  fracpart;
	
	if (!isfinite(num))
		return(0);
	if (num < 0.0)
	{
		neg = 1;
		num = -num;
	}
	intpart = num;
	fracpart = num - (float)intpart;
	result = num - fracpart;
	if (fracpart >= 0.5)
		result += 1;
	if (neg)
		result = -result;
	return(result);
}



#if !defined(__arm)
_MSL_IMP_EXP_C long int _MSL_MATH_CDECL lrint(double num)
{
	long int result = 0;
	int   neg = 0;
	long int intpart;   
	double  fracpart;
	int rounding;
	
	if (!isfinite(num))
		return(num);
	if (num > LONG_MAX)
	{
		errno = ERANGE;
		return(LONG_MAX);
	}
	if (num < 0.0)
	{
		neg = 1;
		num = -num;
	}
	intpart = (long int)num;
	fracpart = num - (float)intpart;
	result = num - fracpart;
	if (fracpart > 0)
	{
		rounding = fegetround();
		switch(rounding)
		{
			case(FE_DOWNWARD):
				if (neg)
					result += 1.0;
				break;
			case(FE_TONEAREST):
				if (fracpart >= 0.5)
					result += 1.0;
				break;
			case(FE_TOWARDZERO):
				break;
			case(FE_UPWARD):
				if (!neg)
					result += 1.0;
				break;
			default:
				break;
		}
	}
	
	if (neg)
		result = -result;
	return(result);
}

_MSL_IMP_EXP_C long int _MSL_MATH_CDECL lrintf(float num)
{
	long int result = 0;
	int   neg = 0;
	long int intpart;   
	float  fracpart;
	int rounding;
	
	if (!isfinite(num))
		return(num);
	if (num > LONG_MAX)
	{
		errno = ERANGE;
		return(LONG_MAX);
	}
	if (num < 0.0)
	{
		neg = 1;
		num = -num;
	}
	intpart = (long int)num;
	fracpart = num - (float)intpart;
	result = num - fracpart;
	if (fracpart > 0)
	{
	
		rounding = fegetround();
		switch(rounding)
		{
			case(FE_DOWNWARD):
				if (neg)
					result += 1.0;
				break;
			case(FE_TONEAREST):
				if (fracpart >= 0.5)
					result += 1.0;
				break;
			case(FE_TOWARDZERO):
				break;
			case(FE_UPWARD):
				if (!neg)
					result += 1.0;
				break;
		}
	}
	if (neg)
		result = -result;
	return(result);
}
#endif /* !defined(__arm) */

_MSL_IMP_EXP_C long int _MSL_MATH_CDECL lrintl(long double num)
{
	long int result = 0;
	int   neg = 0;
	long int intpart;   
	long double  fracpart;
	int rounding;
	
	if (!isfinite(num))
		return(num);
	if (num > LONG_MAX)
	{
		errno = ERANGE;
		return(LONG_MAX);
	}
	if (num < 0.0)
	{
		neg = 1;
		num = -num;
	}
	intpart = (long int)num;
	fracpart = num - (float)intpart;
	result = num - fracpart;
	
	if (fracpart > 0)
	{
	
		rounding = fegetround();
		switch(rounding)
		{
			case(FE_DOWNWARD):
				if (neg)
					result += 1.0;
				break;
			case(FE_TONEAREST):
				if (fracpart >= 0.5)
					result += 1.0;
				break;
			case(FE_TOWARDZERO):
				break;
			case(FE_UPWARD):
				if (!neg)
					result += 1.0;
				break;
		}
	}
	if (neg)
		result = -result;
	return(result);
}

_MSL_IMP_EXP_C long int _MSL_MATH_CDECL lround(double num)
{
	long int result = 0;
	int   neg = 0;
	long int intpart;   
	double  fracpart;
	
	if (!isfinite(num))
		return(0);
	if (num < 0.0)
	{
		neg = 1;
		num = -num;
	}
	intpart = num;
	fracpart = num - (float)intpart;
	result = num - fracpart;
	if (fracpart >= 0.5)
		result += 1;
	if (neg)
		result = -result;
	return(result);
}

_MSL_IMP_EXP_C long int _MSL_MATH_CDECL lroundf(float num)
{
	long int result = 0;
	int   neg = 0;
	long int intpart;   
	float  fracpart;
	
	if (!isfinite(num))
		return(0);
	if (num < 0.0)
	{
		neg = 1;
		num = -num;
	}
	intpart = num;
	fracpart = num - (float)intpart;
	result = num - fracpart;
	if (fracpart >= 0.5)
		result += 1;
	if (neg)
		result = -result;
	return(result);
}

_MSL_IMP_EXP_C long int _MSL_MATH_CDECL lroundl(long double num)
{
	long int result = 0;
	int   neg = 0;
	long int intpart;   
	long double  fracpart;
	
	if (!isfinite(num))
		return(0);
	if (num < 0.0)
	{
		neg = 1;
		num = -num;
	}
	intpart = num;
	fracpart = num - (float)intpart;
	result = num - fracpart;
	if (fracpart >= 0.5)
		result += 1;
	if (neg)
		result = -result;
	return(result);
}

_MSL_IMP_EXP_C double _MSL_MATH_CDECL nearbyint(double num)
{
	long int result = 0;
	int   neg = 0;
	long int intpart;   
	double  fracpart;
	double arg = num;
	
	int rounding;
	
	if (!isfinite(num))
		return(num);
	if (num > LONG_MAX)
	{
		errno = ERANGE;
		result = LONG_MAX;
	}
	else
	{
		if (num < 0.0)
		{
			neg = 1;
			num = -num;
		}
		intpart = (long int)num;
		fracpart = num - (float)intpart;
		result = num - fracpart;
		if (fracpart > 0)
		{
		
			rounding = fegetround();
			switch(rounding)
			{
				case(FE_DOWNWARD):
					if (neg)
						result += 1.0;
					break;
				case(FE_TONEAREST):
					if (fracpart >= 0.5)
						result += 1.0;
					break;
				case(FE_TOWARDZERO):
					break;
				case(FE_UPWARD):
					if (!neg)
						result += 1.0;
					break;
			}
		}
		if (neg)
			result = -result;
	}
	return(result);
}

_MSL_IMP_EXP_C float _MSL_MATH_CDECL nearbyintf(float num)
{
	long int result = 0;
	int   neg = 0;
	long int intpart;   
	float  fracpart;
	int rounding;
	float arg = num;
	
	if (!isfinite(num))
		return(num);
	if (num > LONG_MAX)
	{
		errno = ERANGE;
		result = LONG_MAX;
	}
	else
	{
		if (num < 0.0)
		{
			neg = 1;
			num = -num;
		}
		intpart = (long int)num;
		fracpart = num - (float)intpart;
		result = num - fracpart;
		if (fracpart > 0)
		{
		
			rounding = fegetround();
			switch(rounding)
			{
				case(FE_DOWNWARD):
					if (neg)
						result += 1.0;
					break;
				case(FE_TONEAREST):
					if (fracpart >= 0.5)
						result += 1.0;
					break;
				case(FE_TOWARDZERO):
					break;
				case(FE_UPWARD):
					if (!neg)
						result += 1.0;
					break;
			}
		}
		if (neg)
			result = -result;
	}
	return(result);
}

_MSL_IMP_EXP_C long double _MSL_MATH_CDECL nearbyintl(long double num)
{
	long int result = 0;
	int   neg = 0;
	long int intpart;   
	long double  fracpart;
	int rounding;
	long double arg = num;
	
	if (!isfinite(num))
		return(num);
	if (num > LONG_MAX)
	{
		errno = ERANGE;
		result = LONG_MAX;
	}
	else
	{
		if (num < 0.0)
		{
			neg = 1;
			num = -num;
		}
		intpart = (long int)num;
		fracpart = num - (float)intpart;
		result = num - fracpart;
		
		if (fracpart > 0)
		{
		
			rounding = fegetround();
			switch(rounding)
			{
				case(FE_DOWNWARD):
					if (neg)
						result += 1.0;
					break;
				case(FE_TONEAREST):
					if (fracpart >= 0.5)
						result += 1.0;
					break;
				case(FE_TOWARDZERO):
					break;
				case(FE_UPWARD):
					if (!neg)
						result += 1.0;
					break;
			}
		}
		if (neg)
			result = -result;
	}
	return(result);
}


_MSL_IMP_EXP_C double _MSL_MATH_CDECL remquo(double x, double y, int *quo)
{
	double result;
	double fracpart;
	int sign = 0;
	
	result = remainder(x, y);
	if (x < 0)
	{
		sign = 1;
		x = -x;
	}
	if (y < 0)
	{
		sign ^= 1;
		y = -y;
	}
	*quo = (int)(x / y);
	fracpart = fabs((x / y) - *quo);
	if (fracpart > 0.5)
		*quo += 1;
	else if (fracpart == 0.5)
		*quo += (*quo % 2);
	if (sign)
		*quo = -*quo;	
	return(result);
}

_MSL_IMP_EXP_C float _MSL_MATH_CDECL remquof(float x, float y, int *quo)
{
	float result;
	float fracpart;
	int sign = 0;
	
	result = remainderf(x, y);
	if (x < 0)
	{
		sign = 1;
		x = -x;
	}
	if (y < 0)
	{
		sign ^= 1;
		y = -y;
	}
	*quo = (int)(x / y);
	fracpart = fabs((x / y) - *quo);
	if (fracpart > 0.5)
		*quo += 1;
	else if (fracpart == 0.5)
		*quo += (*quo % 2);
	if (sign)
		*quo = -*quo;	
	return(result);
}

_MSL_IMP_EXP_C long double _MSL_MATH_CDECL remquol(long double x, long double y, int *quo)
{
	long double result;
	long double fracpart;
	int sign = 0;
	
	result = remainder(x, y);
	if (x < 0)
	{
		sign = 1;
		x = -x;
	}
	if (y < 0)
	{
		sign ^= 1;
		y = -y;
	}
	*quo = (int)(x / y);
	fracpart = fabs((x / y) - *quo);
	if (fracpart == 0.5)
		*quo += (*quo % 2);
	else if (fracpart > 0.5)
		*quo += 1;

	if (sign)
		*quo = -*quo;	
	return(result);
}

_MSL_IMP_EXP_C double _MSL_MATH_CDECL round(double num)
{
	double result = 0;
	int   neg = 0;
	_INT32 intpart;   
	double  fracpart;
	
	if (!isfinite(num))
		return(num);
	if (num < 0.0)
	{
		neg = 1;
		num = -num;
	}
	intpart = num;
	fracpart = num - (float)intpart;
	result = num - fracpart;
	if (fracpart >= 0.5)
		result += 1.0;
	if (neg)
		result = -result;
	return(result);
}


_MSL_IMP_EXP_C float _MSL_MATH_CDECL roundf(float num)
{
	float result = 0;
	int   neg = 0;
	_INT32 intpart;   
	float  fracpart;
	
	if (!isfinite(num))
		return(num);
	if (num < 0.0)
	{
		neg = 1;
		num = -num;
	}
	intpart = num;
	fracpart = num - (float)intpart;
	result = num - fracpart;
	if (fracpart >= 0.5)
		result += 1.0;
	if (neg)
		result = -result;
	return(result);
}

_MSL_IMP_EXP_C long double _MSL_MATH_CDECL roundl(long double num)
{
	long double result = 0;
	int   neg = 0;
	long int intpart;   
	float  fracpart;
	
	if (!isfinite(num))
		return(num);
	if (num < 0.0)
	{
		neg = 1;
		num = -num;
	}
	intpart = num;
	fracpart = num - (float)intpart;
	result = num - fracpart;
	if (fracpart >= 0.5)
		result += 1.0;
	if (neg)
		result = -result;
	return(result);
}

#if (FLT_RADIX != 2)
#error Unsupported value of FLT_RADIX
#endif



_MSL_IMP_EXP_C long double _MSL_MATH_CDECL scalbnl(long double  x, int n)
{
	long double mant;
	int    exp;
	long double result;
	 
	mant = frexp(x, &exp);
	exp += n;
	result = ldexpf(mant, exp);

	return(result);
}


_MSL_IMP_EXP_C double _MSL_MATH_CDECL tgamma(double x) _MSL_CANT_THROW
{
	return((double)tgammal(x));
}


_MSL_IMP_EXP_C double _MSL_MATH_CDECL trunc(double num)
{
	int   neg = 0;
	long int intpart;   
	double  result;
	
	if (!isfinite(num))
		return(0);
	if (num < 0.0)
	{
		neg = 1;
		num = -num;
	}
	intpart = num;
	result = (double)intpart;
	if (neg)
		result = -result;
	return(result);
}

_MSL_IMP_EXP_C float _MSL_MATH_CDECL truncf(float num)
{
	int   neg = 0;
	long int intpart;   
	float  result;
	
	if (!isfinite(num))
		return(0);
	if (num < 0.0)
	{
		neg = 1;
		num = -num;
	}
	intpart = num;
	result = (float)intpart;
	if (neg)
		result = -result;
	return(result);
}

_MSL_IMP_EXP_C long double _MSL_MATH_CDECL truncl(long double num)
{
	int   neg = 0;
	long int intpart;   
	long double  result;
	
	if (!isfinite(num))
		return(0);
	if (num < 0.0)
	{
		neg = 1;
		num = -num;
	}
	intpart = num;
	result = (long double)intpart;
	if (neg)
		result = -result;
	return(result);
}

#endif /* _MSL_C99 */
	
#endif /* _MSL_FLOATING_POINT */

#endif /* _MSL_USES_SUN_MATH_LIB */

/* Change record: 
 * cc  020124 Took out Sun specific functions from math.c
 * cc  041203 Moving nextafter to math_double.c
 * cc  051031 Removed raise of FE_INEXACT 
 */