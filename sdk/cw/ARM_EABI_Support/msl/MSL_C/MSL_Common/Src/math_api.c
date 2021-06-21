/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/20 20:48:50 $
 * $Revision: 1.9.2.3 $
 */

#include <ansi_parms.h>
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


#if !__has_intrinsic(__builtin___count_bits32)
unsigned _MSL_MATH_CDECL __msl_generic_count_bits32(_UINT32 x) _MSL_CANT_THROW
{
	x -= (x >> 1) & 0x55555555;
	x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
	x = (x + (x >> 4)) & 0x0F0F0F0F;
	x += x >> 8;
	x += x >> 16;
	return (unsigned)x & 0xFF;
}
#endif /* !__has_intrinsic(__builtin___count_leading_zero32) */

#if !__has_intrinsic(__builtin___count_leading_zero32)
unsigned int _MSL_MATH_CDECL __msl_generic_count_leading_zero32(_UINT32 x) _MSL_CANT_THROW
{
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	return 32U - __msl_count_bits32(x);
}
#endif /* !__has_intrinsic(__builtin___count_leading_zero32) */

#if _MSL_LONGLONG

#if !__has_intrinsic(__builtin___count_bits64)
unsigned int _MSL_MATH_CDECL __msl_generic_count_bits64(_UINT64 x) _MSL_CANT_THROW
{
	x -= (x >> 1) & 0x5555555555555555;
	x = (x & 0x3333333333333333) + ((x >> 2) & 0x3333333333333333);
	x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0F;
	x += x >> 8;
	x += x >> 16;
	x += x >> 32;
	return (unsigned)x & 0xFF;
}
#endif /* !__has_intrinsic(__builtin___count_bits64) */

#if !__has_intrinsic(__builtin___count_leading_zero64)
unsigned int _MSL_MATH_CDECL __msl_generic_count_leading_zero64(_UINT64 x) _MSL_CANT_THROW
{
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	x |= (x >> 32);
	return 64U - __msl_count_bits64(x);
}
#endif /* !__has_intrinsic(__builtin___count_leading_zero64) */

#endif /* _MSL_LONGLONG */


#if _MSL_C99

#if (_MSL_USES_SUN_MATH_LIB && _MSL_FLOATING_POINT)

short __msl_relation(double x, double y)
{
	if (isnan(x) || isnan(y))
		return(3);          /* unordered    */
	else if (x > y)
		return(0);			/* greater then */
	else if (x < y)
		return(1);			/* less than    */
	else
		return(2);			/* equals       */
}

#endif /* _MSL_USES_SUN_MATH_LIB && _MSL_FLOATING_POINT*/

#endif /* _MSL_C99 */



#if __dest_os != __mac_os

#if _MSL_FLOATING_POINT

/*#if (_MSL_C99 || _MSL_USES_SUN_MATH_LIB)*/

#if _MSL_C99  || _MSL_USES_SUN_MATH_LIB
int _MSL_MATH_CDECL __signbitf(float x) _MSL_CANT_THROW
{
	return((_INT32)(__HI(x)&0x80000000));
}


int _MSL_MATH_CDECL __fpclassifyf(float x) _MSL_CANT_THROW
{
	switch( (*(_INT32*)&x)&0x7f800000 )
	{
		case 0x7f800000:
		{
			if((*(_INT32*)&x)&0x007fffff) return FP_NAN;
			else return FP_INFINITE;
			break;
		}
		case 0:
		{
			if((*(_INT32*)&x)&0x007fffff) return FP_SUBNORMAL;
			else return FP_ZERO; 
			break; 
		}
	}
	return FP_NORMAL;
}  

#endif /* _MSL_C99 || _MSL_USES_SUN_MATH_LIB */

#if _MSL_C99  || _MSL_USES_SUN_MATH_LIB || _MSL_C_HAS_CPP_EXTENSIONS

int _MSL_MATH_CDECL __signbitd(double x) _MSL_CANT_THROW
{
	return((_INT32)(__UHI(x)&0x80000000));
}

int _MSL_MATH_CDECL __fpclassifyd(double x) _MSL_CANT_THROW
{
	switch(__HI(x)&0x7ff00000 )
	{
		case 0x7ff00000:
		{
			if((__HI(x)&0x000fffff) || (__LO(x)&0xffffffff)) return FP_NAN;
			else return FP_INFINITE;
			break;
		}
		case 0:
		{
			if((__HI(x)&0x000fffff) || (__LO(x)&0xffffffff)) return FP_SUBNORMAL;
			else return FP_ZERO; 
			break; 
		}

	}
	return FP_NORMAL;
} 
#endif /* _MSL_C_HAS_CPP_EXTENSIONS */
#endif /*_MSL_FLOATING_POINT */
#endif  /*__dest_os != __mac_os*/


/* Change record: 
 * cc  040518 Broke math_api functions out of math.c
 * cc  040920 Added signbit fix
 * cc  041108 Added _MSL_USES_SUN_MATH_LIB check around __fpclassifyd/f and __signbitd/f
 * MA  060220 Resored _MSL_USES_SUN_MATH_LIB to compile __fpclassifyd/f when non _MSL_C99
 */