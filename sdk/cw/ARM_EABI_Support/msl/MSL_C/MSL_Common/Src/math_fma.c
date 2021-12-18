/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:33:27 $
 * $Revision: 1.5.2.2 $
 */

#include <ansi_parms.h>

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

#if !__has_intrinsic(__builtin_fma)
_MSL_IMP_EXP_C double _MSL_MATH_CDECL fma(double x, double y, double z) _MSL_CANT_THROW
{
	uint64_t p_hi, p_lo;
	int shift;
	unsigned int xp;
	uint32_t sx, sy, sz, sp, x_lo, y_lo, z_lo;
	int32_t ep;
	/* get most significant 32 bits */
	uint32_t x_hi = __UHI(x);
	uint32_t y_hi = __UHI(y);
	uint32_t z_hi = __UHI(z);
	/* extract exponent */
	int32_t ex = (int32_t)(x_hi & 0x7FF00000);
	int32_t ey = (int32_t)(y_hi & 0x7FF00000);
	int32_t ez = (int32_t)(z_hi & 0x7FF00000);
	/* test for cases we don't need extended precision */
	if (x == 0 || ex == 0x7FF00000 ||
	    y == 0 || ey == 0x7FF00000 ||
	    z == 0 || ez == 0x7FF00000)
		return x * y + z;
	/* all of x, y and z are now either FP_NORMAL or FP_SUBNORMAL
	   represent each x as:  sx = sign, ex = biased exponent, x_hi:x_lo 53 bit mantissa 
	   shift exponents down, they are still biased */
	ex >>= 20;
	ey >>= 20;
	ez >>= 20;
	/* extract sign bits */
	sx = x_hi & 0x80000000;
	sy = y_hi & 0x80000000;
	sz = z_hi & 0x80000000;
	/* mask off mantissas.  Explicitly store implicit leading bit.  And correct 0 exponent */
	x_hi &= 0x000FFFFF;
	if (ex == 0)
		ex = 1;
	else
		x_hi |= 0x00100000;
	y_hi &= 0x000FFFFF;
	if (ey == 0)
		ey = 1;
	else
		y_hi |= 0x00100000;
	z_hi &= 0x000FFFFF;
	if (ez == 0)
		ez = 1;
	else
		z_hi |= 0x00100000;
	/* extract least significant 32 bits */
	x_lo = __ULO(x);
	y_lo = __ULO(y);
	z_lo = __ULO(z);

	/* Perform p = x * y
	   p will be 118 bits stored in 2 uint64_t's, p_hi:p_lo (only 106 bits are actually generated)
	   compute least significant 32 bits of p_lo.  Use p_hi as an accumulator. */
	p_hi = (uint64_t)x_lo * y_lo;
	p_lo = p_hi & 0xFFFFFFFF;
	p_hi >>= 32;
	/* compute most significant 32 bits of p_lo. */
	p_hi += (uint64_t)x_hi * y_lo;
	p_hi += (uint64_t)x_lo * y_hi;
	p_lo |= p_hi << 32;
	p_hi >>= 32;
	/* compute remaining 42 bits */
	p_hi += (uint64_t)x_hi * y_hi;
	/* shift p_hi:p_lo left by 12 bits to "normalize" it */
	p_hi = (p_hi << 12) | (p_lo >> 52);
	p_lo <<= 12;
	/* compute exponent ep */
	ep = ex + ey - 0x3FF;
	/* compute sign sp */
	sp = sx ^ sy;

	/* Perform p + z */
	shift = ez - ep;
	/* -106 < shift < 55
	   if shift is too low, return p
	   if shift is too high, return z
	   otherwise do addition */
	if (shift >= 55)
		return z;  /* p is so small it cannot effect z */
	/* sticky guard bit for p */
	xp = 0;
	if (shift > -106)  /* if z is big enough so that it can effect p */
	{
		uint64_t q_hi, q_lo;
		unsigned int xq;
		/* put z into extended precision q_hi:q_lo */
		q_hi = (uint64_t)z_hi << 32;
		q_hi |= z_lo;
		q_lo = 0;
		/* sticky guard bit for q */
		xq = 0;
		/* make ez == ep */
		if (shift > 0)
		{
			/* shift p right by shift bits, and capture lost bits in xp
			   0 < shift < 55 */
			xp = (unsigned int)(((0xFFFFFFFFFFFFFFFFULL >> (64-shift)) & p_lo) != 0);
			p_lo = (p_hi << (64 - shift)) | (p_lo >> shift);
			p_hi >>= shift;
			ep = ez;
		}
		else if (shift < 0)
		{
			shift = -shift;
			/* shift q right by shift bits, and capture lost bits in xq
			   0 < shift < 106
			   q_lo is known to be 0 */
			if (shift < 64)
			{
				q_lo = q_hi << (64 - shift);
				q_hi >>= shift;
			}
			else
			{
				xq = (unsigned int)(((0xFFFFFFFFFFFFFFFFULL >> (128-shift)) & q_hi) != 0);
				q_lo = q_hi >> (shift - 64);
				q_hi = 0;
			}
			ez = ep;
		}
		/* Now ez == ep */
		if (sp ^ sz)  /* subtract */
		{
			int low_sign = 0;
			int high_sign = 0;
			if (p_lo < q_lo)
				low_sign = -1;
			else if (p_lo > q_lo)
				low_sign = 1;
			if (p_hi < q_hi)
				high_sign = -1;
			else if (p_hi > q_hi)
				high_sign = 1;
			if (high_sign < 0 || (high_sign == 0 && low_sign < 0) || (high_sign == 0 && low_sign == 0 && xp < xq))
			{
				/* p = q - p */
				if (low_sign > 0)
					--q_hi;
				p_lo = q_lo - p_lo;
				p_hi = q_hi - p_hi;
				sp = sz;
			}
			else
			{
				/* p = p - q */
				if (low_sign < 0)
					--p_hi;
				p_lo -= q_lo;
				p_hi -= q_hi;
			}
		}
		else  /* addition */
		{
			/* p += q */
			unsigned int carry = 0;
			/* lo */
			if (q_lo > 0xFFFFFFFFFFFFFFFFULL - p_lo)
				carry = 1;
			p_lo += q_lo;
			/* hi */
			p_hi += q_hi + carry;
		}
		/* x bit */
		xp |= xq;
	}
	/* normalize p */
	shift = 0;
	if (p_hi != 0)
		shift = (int)__msl_count_leading_zero64(p_hi) - 11;
	else if (p_lo != 0)
		shift = 53 + (int)__msl_count_leading_zero64(p_lo);
	else  /* p == 0 */
	{
		__ULO(x) = 0;
		__UHI(x) = sp;
		return x;
	}
	if (shift > 0)
	{
		/* shift left by shift bits
		   0 < shift < 116 */
		if (shift < 64)
		{
			p_hi = (p_hi << shift) | (p_lo >> (64 - shift));
			p_lo <<= shift;
		}
		else
		{
			p_hi = p_lo << (shift - 64);
			p_lo = 0;
		}
		ep -= shift;
	}
	else if (shift < 0)
	{
		shift = -shift;
		/* shift right by shift bits
		   0 < shift < 11 */
		xp |= (unsigned int)(((0xFFFFFFFFFFFFFFFFULL >> (64-shift)) & p_lo) != 0);
		p_lo = (p_hi << (64 - shift)) | (p_lo >> shift);
		p_hi >>= shift;
		ep += shift;
	}
	if (ep < 1)  /* denormal or 0 */
	{
		/* shift right by 1 - ep */
		shift = 1 - ep;
		if (shift < 64)
		{
			xp |= (unsigned int)(((0xFFFFFFFFFFFFFFFFULL >> (64-shift)) & p_lo) != 0);
			p_lo = (p_hi << (64 - shift)) | (p_lo >> shift);
			p_hi >>= shift;
			ep = 0;
		}
		else  /* return 0 */
		{
			__ULO(x) = 0;
			__UHI(x) = sp;
			return x;
		}
	}
	if (ep >= 0x7FF)
	{
		/* return +/- inf */
		__ULO(x) = 0;
		__UHI(x) = sp | 0x7FF00000;
		return x;
	}
	/* put exponent and sign into p_hi */
	p_hi &= 0x000FFFFFFFFFFFFF;
	p_hi |= ((uint64_t)sp << 32) | ((uint64_t)ep << 52);
	/* round p to 53 bits */
	switch (FLT_ROUNDS)
	{
	case 1:  /* to nearest (round to even on tie) */
		if ((p_lo & 0x8000000000000000ULL) && ((p_hi & 1) || (p_lo & 0x7FFFFFFFFFFFFFFFULL) || xp))
			++p_hi;
		break;
	case 2:  /* towards inf */
		if ((p_lo || xp) && !sp)
			++p_hi;
		break;
	case 3:  /* towards -inf */
		if ((p_lo || xp) && sp)
			++p_hi;
		break;
	case 0:  /* towards 0 */
	default: /* indeterminant */
		break;
	}
	__ULO(x) = (uint32_t)p_hi;
	__UHI(x) = (uint32_t)(p_hi >> 32);
	return x;
}
#endif /* !__has_intrinsic(__builtin_fma) */
	
#if !__has_intrinsic(__builtin_fmaf)
_MSL_IMP_EXP_C float _MSL_MATH_CDECL fmaf(float x, float y, float z) _MSL_CANT_THROW
{
	uint64_t p_i;
	int shift;
	unsigned int xp;
	uint32_t sx, sy, sz, sp;
	int32_t ep;
	/* get as unsigned integral */
	uint32_t x_i = *(uint32_t*)&x;
	uint32_t y_i = *(uint32_t*)&y;
	uint32_t z_i = *(uint32_t*)&z;
	/* extract exponent */
	int32_t ex = (int32_t)(x_i & 0x7F800000);
	int32_t ey = (int32_t)(y_i & 0x7F800000);
	int32_t ez = (int32_t)(z_i & 0x7F800000);
	/* test for cases we don't need extended precision */
	if (x == 0 || ex == 0x7F800000 ||
	    y == 0 || ey == 0x7F800000 ||
	    z == 0 || ez == 0x7F800000)
		return x * y + z;
	/* all of x, y and z are now either FP_NORMAL or FP_SUBNORMAL
	   represent each x as:  sx = sign, ex = biased exponent, x_i 24 bit mantissa 
	   shift exponents down, they are still biased */
	ex >>= 23;
	ey >>= 23;
	ez >>= 23;
	/* extract sign bits */
	sx = x_i & 0x80000000;
	sy = y_i & 0x80000000;
	sz = z_i & 0x80000000;
	/* mask off mantissas.  Explicitly store implicit leading bit.  And correct 0 exponent */
	x_i &= 0x007FFFFF;
	if (ex == 0)
		ex = 1;
	else
		x_i |= 0x00800000;
	y_i &= 0x007FFFFF;
	if (ey == 0)
		ey = 1;
	else
		y_i |= 0x00800000;
	z_i &= 0x007FFFFF;
	if (ez == 0)
		ez = 1;
	else
		z_i |= 0x00800000;

	/* Perform p = x * y
	   p will be 57 bits stored in a uint64_t, p_i (only 48 bits are actually generated) */
	p_i = (uint64_t)x_i * y_i;
	/* shift p_i left by 9 bits to "normalize" it */
	p_i <<= 9;
	/* compute exponent ep */
	ep = ex + ey - 0x7F;
	/* compute sign sp */
	sp = sx ^ sy;

	/* Perform p + z */
	shift = ez - ep;
	/* -48 < shift < 26
	   if shift is too low, return p
	   if shift is too high, return z
	   otherwise do addition */
	if (shift >= 26)
		return z;  /* p is so small it cannot effect z */
	/* sticky guard bit for p */
	xp = 0;
	if (shift > -48)  /* if z is big enough so that it can effect p */
	{
		/* put z into extended precision q_i */
		uint64_t q_i = (uint64_t)z_i << 32;
		/* sticky guard bit for q */
		unsigned int xq = 0;
		/* make ez == ep */
		if (shift > 0)
		{
			/* shift p right by shift bits, and capture lost bits in xp
			   0 < shift < 26 */
			xp = (unsigned int)(((0xFFFFFFFFFFFFFFFFULL >> (64-shift)) & p_i) != 0);
			p_i >>= shift;
			ep = ez;
		}
		else if (shift < 0)
		{
			shift = -shift;
			/* shift q right by shift bits, and capture lost bits in xq
			   0 < shift < 48 */
			xq = (unsigned int)(((0xFFFFFFFFFFFFFFFFULL >> (64-shift)) & q_i) != 0);
			q_i >>= shift;
			ez = ep;
		}
		/* Now ez == ep */
		if (sp ^ sz)  /* subtract */
		{
			if (p_i < q_i || p_i == q_i && xp < xq)
			{
				/* p = q - p */
				p_i = q_i - p_i;
				sp = sz;
			}
			else
			{
				/* p = p - q */
				p_i -= q_i;
			}
		}
		else  /* addition */
		{
			/* p += q */
			p_i += q_i;
		}
		/* x bit */
		xp |= xq;
	}
	/* normalize p */
	shift = 0;
	if (p_i != 0)
		shift = (int)__msl_count_leading_zero64(p_i) - 8;
	else  /* p == 0 */
	{
		*(uint32_t*)&x = sp;
		return x;
	}
	if (shift > 0)
	{
		/* shift left by shift bits
		   0 < shift < 55 */
		p_i <<= shift;
		ep -= shift;
	}
	else if (shift < 0)
	{
		shift = -shift;
		/* shift right by shift bits
		   0 < shift < 8 */
		xp |= (unsigned int)(((0xFFFFFFFFFFFFFFFFULL >> (64-shift)) & p_i) != 0);
		p_i >>= shift;
		ep += shift;
	}
	if (ep < 1)  /* denormal or 0 */
	{
		/* shift right by 1 - ep */
		shift = 1 - ep;
		if (shift < 64)
		{
			xp |= (unsigned int)(((0xFFFFFFFFFFFFFFFFULL >> (64-shift)) & p_i) != 0);
			p_i >>= shift;
			ep = 0;
		}
		else  /* return 0 */
		{
			*(uint32_t*)&x = sp;
			return x;
		}
	}
	if (ep >= 0xFF)
	{
		/* return +/- inf */
		*(uint32_t*)&x = sp | 0x7F800000;
		return x;
	}
	/* put exponent and sign into p_i */
	p_i &= 0x007FFFFFFFFFFFFF;
	p_i |= ((uint64_t)sp << 32) | ((uint64_t)ep << 55);
	/* round p to 24 bits */
	switch (FLT_ROUNDS)
	{
	case 1:  /* to nearest (round to even on tie) */
		if ((p_i & 0x0000000080000000) && ((p_i & 0x0000000100000000) || (p_i & 0x000000007FFFFFFF) || xp))
			p_i += 0x0000000100000000;
		break;
	case 2:  /* towards inf */
		if (((p_i & 0x00000000FFFFFFFF) || xp) && !sp)
			p_i += 0x0000000100000000;
		break;
	case 3:  /* towards -inf */
		if (((p_i & 0x00000000FFFFFFFF) || xp) && sp)
			p_i += 0x0000000100000000;
		break;
	case 0:  /* towards 0 */
	default: /* indeterminant */
		break;
	}
	*(uint32_t*)&x = (uint32_t)(p_i >> 32);
	return x;
}
#endif /* !__has_intrinsic(__builtin_fmaf) */

#endif /* _MSL_FLOATING_POINT */

#endif /* _MSL_C99 */

/* Change record: 
 * cc  020124 Took out fma/fmaf functions to single file
 */