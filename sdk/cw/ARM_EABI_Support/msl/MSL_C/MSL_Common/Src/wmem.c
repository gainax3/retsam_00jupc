/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:34:36 $
 * $Revision: 1.31.2.2 $
 */

/*
 *	Routines
 *	--------
 *		wmemcpy
 *		wmemcpy_s
 *		wmemmove
 *		wmemmove_s
 *		wmemset
 *		wmemchr
 *		wmemcmp
 */

#pragma ANSI_strict off

#define __STDC_WANT_SECURE_LIB__ 1
#include <ansi_parms.h>

#if _MSL_WIDE_CHAR


#pragma ANSI_strict reset

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>
#include <msl_secure_error.h>

wchar_t * _MSL_CDECL (wmemcpy)(wchar_t * _MSL_RESTRICT dst, const wchar_t * _MSL_RESTRICT src, size_t n)
{
	return memcpy(dst, src, n * sizeof(wchar_t));					/*- JWW 011207 -*/
}

errno_t _MSL_CDECL (wmemcpy_s)(wchar_t * _MSL_RESTRICT dst, rsize_t dst_max, const wchar_t * _MSL_RESTRICT src, rsize_t n)
{
	if (((dst == NULL) || (src == NULL)) || ((dst_max > RSIZE_MAX) || (n > RSIZE_MAX)))
	{
		__msl_undefined_behavior_s();
		return ERANGE;
	}
	
	if (dst_max < n)
	{
		__msl_undefined_behavior_s();
		memset(dst, 0, dst_max * sizeof(wchar_t));
		return ERANGE;
	}
	
	memcpy(dst, src, n * sizeof(wchar_t));
	return ENOERR;
}

wchar_t * _MSL_CDECL (wmemmove)(wchar_t * dst, const wchar_t * src, size_t n)
{
	return memmove(dst, src, n * sizeof(wchar_t));					/*- JWW 011207 -*/
}

errno_t _MSL_CDECL (wmemmove_s)(wchar_t * dst, size_t dst_max, const wchar_t * src, size_t n)
{
	if (((dst == NULL) || (src == NULL)) || ((dst_max > RSIZE_MAX) || (n > RSIZE_MAX)))
	{
		__msl_undefined_behavior_s();
		return ERANGE;
	}
	
	if (dst_max < n)
	{
		__msl_undefined_behavior_s();
		memset(dst, 0, dst_max * sizeof(wchar_t));
		return ERANGE;
	}
	
	memmove(dst, src, n * sizeof(wchar_t));
	return ENOERR;
}

wchar_t * _MSL_CDECL (wmemset)(wchar_t * dst, wchar_t val, size_t n)
{
	wchar_t *save = dst;
	
	while (n)
	{
		*dst++ = val;
		n--;
	}
	
	return(save);													/*- mm 990712 -*/
}

wchar_t * _MSL_CDECL (wmemchr)(const wchar_t * src, wchar_t val, size_t n)
{
	while (n)
	{
		if (*src == val) return (wchar_t *) src;
		src++;
		n--;
	}
	
	return(NULL);
}

int _MSL_CDECL (wmemcmp)(const wchar_t * src1, const wchar_t * src2, size_t n)	/*- mm 990629 -*/
{
	int diff = 0;													/*- mm 990629 -*/
	
	while (n)
	{
		diff = *src1 - *src2;
		if (diff) break;
		src1++;
		src2++;
		n--;
	}
	
	return(diff);
}

#endif /* #if _MSL_WIDE_CHAR */									/*- mm 981030 -*/

/* Change record:
 * jcm 980126 First code release.
 * mm  980909 Corrected prototypes to refer to wchar_t* instead of void*
 * mm  981030 Added #if _MSL_WIDE_CHAR wrappers
 * mm  981111 Corrected wmemchr() and wmemcmp() to return a meaningful result 
 *			  and wmemset to work correctly---IL9811-0490	
 * blc 990112 Fixed all the implementation so they actually worked
 * mm  990629 Corrected the signature and working of wmemcmp
 * mm  990712 Fixed wmemset to return correct value
 * cc  011203 Added _MSL_CDECL for new name mangling 
 * JWW 011207 Do not assume wchar_t is only 2 bytes long
 * JWW 020917 Added _MSL_RESTRICT to get the restrict type specifier for certain C99 functions
 * JWW 030224 Changed __NO_WIDE_CHAR flag into the new more configurable _MSL_WIDE_CHAR
 * mm  030304 Added _MSL_C99 wrapper
 * JWW 031030 Added the __STDC_WANT_SECURE_LIB__ secure library extensions
 * JWW 040317 Updated the __STDC_WANT_SECURE_LIB__ secure library based on new secure specifications
 */