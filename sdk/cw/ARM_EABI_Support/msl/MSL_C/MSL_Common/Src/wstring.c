/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:34:37 $
 * $Revision: 1.41.2.2 $
 */

/*
 *	Routines
 *	--------
 *		wcslen
 *		wcsnlen_s
 *
 *		wcscpy
 *		wcscpy_s
 *		wcscat
 *		wcscmp
 *
 *		wcsncpy
 *		wcsncpy_s
 *		wcsncat
 *		wcsncat_s
 *		wcsncmp
 *
 *		wcscoll
 *		wcsxfrm
 *
 *		wcschr
 *		wcsrchr
 *		wcsstr
 *
 *		wcsspn
 *		wcscspn
 *		wcspbrk
 */

#pragma ANSI_strict off  					/*- vss 990729 -*/

#define __STDC_WANT_SECURE_LIB__ 1
#include <ansi_parms.h> 

#if _MSL_WIDE_CHAR						/*- mm 980204 -*/

#pragma ANSI_strict reset

#include <errno.h>
#include <stddef.h>      
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>
#include <msl_secure_error.h>

#pragma warn_possunwant off

/*
	This routine returns the length of the wide character string str
*/
size_t _MSL_CDECL (wcslen)(const wchar_t * str)
{
	size_t	len = -1;
	
#if !defined(__POWERPC__)
	
	do
		len++;
	while (*str++);
	
#else
	
	wchar_t * p = (wchar_t *) str - 1;
	
	do
		len++;
	while (*++p);
	
#endif
	
	return(len);
}

size_t _MSL_CDECL (wcsnlen_s)(const wchar_t * str, size_t maxsize)
{
	wchar_t *end;
	wchar_t *p;
	size_t	len = -1;
	
	if ((maxsize == 0) || (str == NULL))
		return 0;
	
	end = (wchar_t *) &(str[maxsize - 1]);
	p = (wchar_t *) str - 1;
	
	do
		len++;
	while ((end > p) && *++p);
	
	return len;
}

wchar_t * _MSL_CDECL (wcscpy)(wchar_t * _MSL_RESTRICT dst, const wchar_t * _MSL_RESTRICT src)
{
#if !defined(__POWERPC__)
	
	const	wchar_t * p = src;
				wchar_t * q = dst;
	
	while (*q++ = *p++);
	
#else
	
	const	wchar_t * p = (wchar_t *) src - 1;
				wchar_t * q = (wchar_t *) dst - 1;
	
	while (*++q = *++p);

#endif
	
	return(dst);
}

errno_t _MSL_CDECL (wcscpy_s)(wchar_t * _MSL_RESTRICT dst, rsize_t dst_max, const wchar_t * _MSL_RESTRICT src)
{
	if ((dst_max == 0) || (dst == NULL) || (src == NULL) || (dst_max == 0) || (dst_max > RSIZE_MAX))
	{
		__msl_undefined_behavior_s();
		if ((dst != NULL) && (dst_max > 0) && (dst_max <= RSIZE_MAX))
			dst[0] = 0;
		return ERANGE;
	}
	
	if (dst_max <= wcsnlen_s(src, dst_max))
	{
		__msl_undefined_behavior_s();
		dst[0] = 0;
		return ERANGE;
	}
	
	wcscpy(dst, src);
	return ENOERR;
}


wchar_t * _MSL_CDECL wcsncpy(wchar_t * _MSL_RESTRICT dst, const wchar_t * _MSL_RESTRICT src, size_t n)
{
#if !defined(__POWERPC__)
	
	const	wchar_t * p = src;
				wchar_t * q = dst;
	
	n++;
	
	while (--n)
		if (!(*q++ = *p++))
		{
			while (--n)
				*q++ = 0;
			break;
		}
	
#else
	
	const	wchar_t * p		= (const wchar_t *) src - 1;
				wchar_t * q		= (wchar_t *) dst - 1;
				wchar_t zero	= 0;
	
	n++;
	
	while (--n)
		if (!(*++q = *++p))
		{
			while (--n)
				*++q = 0;
			break;
		}

#endif
	
	return(dst);
}

errno_t _MSL_CDECL wcsncpy_s(wchar_t * _MSL_RESTRICT dst, rsize_t dst_max, const wchar_t * _MSL_RESTRICT src, rsize_t n)
{
	rsize_t i;
	
	if ((dst_max == 0) || (dst == NULL) || (src == NULL) || (dst_max > RSIZE_MAX) || (n > RSIZE_MAX))
	{
		__msl_undefined_behavior_s();
		if ((dst != NULL) && (dst_max > 0) && (dst_max <= RSIZE_MAX))
			dst[0] = 0;
		return ERANGE;
	}
	
	if (dst_max <= n)
	{
		for (i = 0; i < dst_max; i++)
			if (src[i] == 0)
			{
				wcscpy(dst, src);
				return ENOERR;
			}
		__msl_undefined_behavior_s();
		dst[0] = 0;
		return ERANGE;
	}
	
	wcsncpy(dst, src, n);
	dst[n] = 0;
	return ENOERR;
}


wchar_t * _MSL_CDECL wcscat(wchar_t * _MSL_RESTRICT dst, const wchar_t * _MSL_RESTRICT src)
{
#if !defined(__POWERPC__)
	
	const	wchar_t * p = src;
				wchar_t * q = dst;
	
	while (*q++);
	
	q--;
	
	while (*q++ = *p++);
	
#else
	
	const	wchar_t * p = (wchar_t *) src - 1;
			wchar_t * q = (wchar_t *) dst - 1;
	
	while (*++q);
	
	q--;
	
	while (*++q = *++p);

#endif
	
	return(dst);
}

errno_t (wcscat_s)(wchar_t * _MSL_RESTRICT dst, rsize_t dst_max, const wchar_t * _MSL_RESTRICT src)
{
	rsize_t dst_cat_max;
	
	if ((dst == NULL) || (src == NULL) || (dst_max == 0) || (dst_max > RSIZE_MAX))
	{
		__msl_undefined_behavior_s();
		if ((dst != NULL) && (dst_max > 0) && (dst_max <= RSIZE_MAX))
			dst[0] = 0;
		return ERANGE;
	}
	
	dst_cat_max = dst_max - wcsnlen_s(dst, dst_max);
	
	if (dst_cat_max == 0)
	{
		__msl_undefined_behavior_s();
		dst[0] = 0;
		return ERANGE;
	}
	
	if (dst_cat_max <= wcsnlen_s(src, dst_max))
	{
		__msl_undefined_behavior_s();
		dst[0] = 0;
		return ERANGE;
	}
	
	wcscat(dst, src);
	return ENOERR;
}


wchar_t * _MSL_CDECL wcsncat(wchar_t * _MSL_RESTRICT dst, const wchar_t * _MSL_RESTRICT src, size_t n)
{
#if !defined(__POWERPC__)
	
	const	wchar_t * p = src;
				wchar_t * q = dst;
	
	while (*q++);
	
	q--; n++;
	
	while (--n)
		if (!(*q++ = *p++))
		{
			q--;
			break;
		}
	
	*q = 0;
	
#else
	
	const	wchar_t * p = (wchar_t *) src - 1;
				wchar_t * q = (wchar_t *) dst - 1;
	
	while (*++q);
	
	q--; n++;
	
	while (--n)
		if (!(*++q = *++p))
		{
			q--;
			break;
		}
	
	*++q = 0;

#endif
	
	return(dst);
}

errno_t _MSL_CDECL wcsncat_s(wchar_t * _MSL_RESTRICT dst, rsize_t dst_max, const wchar_t * _MSL_RESTRICT src, rsize_t n)
{
	rsize_t i;
	rsize_t dst_cat_max;
	
	if ((dst == NULL) || (src == NULL) || (dst_max > RSIZE_MAX) || (n > RSIZE_MAX))
	{
		__msl_undefined_behavior_s();
		if ((dst != NULL) && (dst_max > 0) && (dst_max <= RSIZE_MAX))
			dst[0] = 0;
		return ERANGE;
	}
	
	dst_cat_max = dst_max - wcsnlen_s(dst, dst_max);
	
	if (dst_cat_max == 0)
	{
		__msl_undefined_behavior_s();
		dst[0] = 0;
		return ERANGE;
	}
	
	if (dst_cat_max < n)
	{
		for (i = 0; i < dst_cat_max; i++)
			if (src[i] == 0)
			{
				wcsncat(dst, src, n);
				return ENOERR;
			}
		__msl_undefined_behavior_s();
		dst[0] = 0;
		return ERANGE;
	}
	
	wcsncat(dst, src, n);
	if (dst_cat_max != n)
		dst[dst_max-dst_cat_max+n] = 0;
	return ENOERR;
}


int _MSL_CDECL wcscmp(const wchar_t * str1, const wchar_t * str2)
{
#if !defined(__POWERPC__)
	
	const	wchar_t * p1 = (wchar_t *) str1;
	const	wchar_t * p2 = (wchar_t *) str2;
				wchar_t		c1, c2;
	
	while ((c1 = *p1++) == (c2 = *p2++))
		if (!c1)
			return(0);

#else
	
	const	wchar_t * p1 = (wchar_t *) str1 - 1;
	const	wchar_t * p2 = (wchar_t *) str2 - 1;
				wchar_t		c1, c2;
		
	while ((c1 = *++p1) == (c2 = *++p2))
		if (!c1)
			return(0);

#endif
	
	return(c1 - c2);
}


int _MSL_CDECL wcsncmp(const wchar_t * str1, const wchar_t * str2, size_t n)
{
#if !defined(__POWERPC__)
	
	const	wchar_t * p1 = (wchar_t *) str1;
	const	wchar_t * p2 = (wchar_t *) str2;
				wchar_t		c1, c2;
	
	n++;
	
	while (--n)
		if ((c1 = *p1++) != (c2 = *p2++))
			return(c1 - c2);
		else if (!c1)
			break;
	
#else
	
	const	wchar_t * p1 = (wchar_t *) str1 - 1;
	const	wchar_t * p2 = (wchar_t *) str2 - 1;
				wchar_t		c1, c2;
	
	n++;
	
	while (--n)
		if ((c1 = *++p1) != (c2 = *++p2))
			return(c1 - c2);
		else if (!c1)
			break;

#endif
	
	return(0);
}


wchar_t * _MSL_CDECL wcschr(const wchar_t * str, const wchar_t chr)
{
#if !defined(__POWERPC__)

	const wchar_t * p = str;
	      wchar_t   c = chr;
	      wchar_t   ch;
	
	while(ch = *p++)
		if (ch == c)
			return((wchar_t *) (p - 1));
	
	return(c ? 0 : (wchar_t *) (p - 1));

#else

	const wchar_t * p = (wchar_t *) str - 1;
	      wchar_t   c = (chr & 0xffff);               		/*- mm 970327 -*/
	      wchar_t   ch;
	
	while(ch = (*++p & 0xffff))                             /*- mm 970327 -*/
		if (ch == c)
			return((wchar_t *) p);
	
	return(c ? 0 : (wchar_t *) p);

#endif
}


int _MSL_CDECL wcscoll(const wchar_t *str1, const wchar_t * str2)
{
	return(wcscmp(str1, str2));
}

size_t _MSL_CDECL wcsxfrm(wchar_t * _MSL_RESTRICT str1, const wchar_t * _MSL_RESTRICT str2, size_t n)
{
	size_t len = wcslen(str2);				/*- mm 990630 -*/
	if (n > 0)								/*- mm 990630 -*/
	{										/*- mm 990630 -*/
		wcsncpy(str1, str2, n-1);			/*- mm 990630 -*/
		if (n < len) 						/*- mm 990630 -*/
			str1[n-1] = L'\0';				/*- mm 990630 -*/
	}										/*- mm 990630 -*/
	return(len);
}


wchar_t * _MSL_CDECL wcsrchr(const wchar_t * str, wchar_t chr)
{
#if !defined(__POWERPC__)

	const wchar_t * p = str;
	const wchar_t * q = 0;
	      wchar_t   c = chr;
	      wchar_t   ch;
	
	while(ch = *p++)
		if (ch == c)
			q = p - 1;
	
	if (q)
		return((wchar_t *) q);
	
	return(c ? 0 : (wchar_t *) (p - 1));

#else

	const wchar_t * p = (wchar_t *) str - 1;
	const wchar_t * q = 0;
	      wchar_t   c = (chr & 0xff);						/*- bb 970530 -*/
	      wchar_t   ch;
	
	while(ch = (*++p & 0xff))								/*- bb 970530 -*/
		if (ch == c)
			q = p;
	
	if (q)
		return((wchar_t *) q);
	
	return(c ? 0 : (wchar_t *) p);

#endif
}


typedef char char_map[8192];									/*- mm 990914 -*/
#define set_char_map(map, ch)  map[ch>>3] |= (1 << (ch&7))
#define tst_char_map(map, ch) (map[ch>>3] &  (1 << (ch&7)))


wchar_t * _MSL_CDECL wcspbrk(const wchar_t * str, const wchar_t * set)
{
	const 	wchar_t *	p;
	      	wchar_t		c;
			char_map	map;									 /*- mm 000419 -*/
	memset(&map, 0, sizeof(char_map));   /* initialize map */    /*- mm 000419 -*/
	
#if !defined(__POWERPC__)
	
	p = (wchar_t *) set;

	while (c = *p++)
		set_char_map(map, c);
	
	p = (wchar_t *) str;
	
	while (c = *p++)
		if (tst_char_map(map, c))
			return((wchar_t *) (p - 1));
			
	return(NULL);

#else
	
	p = (wchar_t *) set - 1;

	while (c = *++p)
		set_char_map(map, c);
	
	p = (wchar_t *) str - 1;
	
	while (c = *++p)
		if (tst_char_map(map, c))
			return((wchar_t *) p);
			
	return(NULL);

#endif
}


size_t _MSL_CDECL wcsspn(const wchar_t * str, const wchar_t * set)
{
	const wchar_t *	p;
	      wchar_t	c;
		  char_map	map;									     /*- mm 000419 -*/
	memset(&map, 0, sizeof(char_map));   /* initialize map */    /*- mm 000419 -*/
	
#if !defined(__POWERPC__)
	
	p = (wchar_t *) set;

	while (c = *p++)
		set_char_map(map, c);
	
	p = (wchar_t *) str;
	
	while (c = *p++)
		if (!tst_char_map(map, c))
			break;
			
	return(p - (wchar_t *) str - 1);

#else
	
	p = (wchar_t *) set - 1;

	while (c = *++p)
		set_char_map(map, c);
	
	p = (wchar_t *) str - 1;
	
	while (c = *++p)
		if (!tst_char_map(map, c))
			break;
			
	return(p - (wchar_t *) str);

#endif
}





size_t _MSL_CDECL wcscspn(const wchar_t * str, const wchar_t * set)
{
	const wchar_t *	p;
	      wchar_t	c;
		  char_map	map;										 /*- mm 000419 -*/
	memset(&map, 0, sizeof(char_map));   /* initialize map */    /*- mm 000419 -*/
	
#if !defined(__POWERPC__)
	
	p = (wchar_t *) set;

	while (c = *p++)
		set_char_map(map, c);
	
	p = (wchar_t *) str;
	
	while (c = *p++)
		if (tst_char_map(map, c))
			break;
			
	return(p - (wchar_t *) str - 1);

#else
	
	p = (wchar_t *) set - 1;

	while (c = *++p)
		set_char_map(map, c);
	
	p = (wchar_t *) str - 1;
	
	while (c = *++p)
		if (tst_char_map(map, c))
			break;
			
	return(p - (wchar_t *) str);

#endif
}


/* Code deleted here mm 010316  The version of wcstok for Windows made use of the thread-local
   data belonging to strtok, which breaks the requirement that strtok behave as if no other library
   function calls it.  Further, the Standards committe learnt from its experience with strtok that
   requiring a state that the library had to look after was a bad thing and so they gave wcstok an
   extra parameter so that thread-local storage is no longer needed for it.                         */

wchar_t * _MSL_CDECL wcstok(wchar_t * _MSL_RESTRICT str, const wchar_t * _MSL_RESTRICT set, wchar_t ** _MSL_RESTRICT ptr)			/*- mm 000422 -*/
{
	wchar_t 		* p, * q;
	static wchar_t 	* n	= (wchar_t *) L"";		/*- cc 011128 -*/	/*- ejs 020219 -*/
	wchar_t 		* s;						/*- cc 011128 -*/	/*- ejs 020219 -*/
	wchar_t			c;
	char_map		map;							 /*- mm 000419 -*/
	
	memset(&map, 0, sizeof(char_map));   /* initialize map */    /*- mm 000419 -*/
	
	if (str)
		s = (wchar_t *) str;
	else
		if (*ptr)								/*- mm 000422 -*/
			s = (wchar_t *) *ptr;				/*- mm 000422 -*/
		else									/*- mm 000422 -*/
			return(NULL);						/*- mm 000422 -*/	
	
	
#if !defined(__POWERPC__)
	
	p = (wchar_t *) set;

	while (c = *p++)
		set_char_map(map, c);
	
	p = s;
	
	while (c = *p++)
		if (!tst_char_map(map, c))
			break;
	
	if (!c)
	{
		/*s = n;*/											/*- ejs 020219 -*/
		*ptr = NULL;										/*- mm 000422 -*/
		return(NULL);
	}
	
	q = p - 1;
	
	while (c = *p++)
		if (tst_char_map(map, c))
			break;
	
	if (!c)
		s = n;
	else
	{
		s    = p;
		*--p = 0;
	}
	
	if (q == NULL)												/*- mm 000422 -*/
		*ptr = NULL;											/*- mm 000422 -*/
	else														/*- mm 000422 -*/
		*ptr = s;												/*- mm 000422 -*/
	return(( wchar_t *) q);

#else   /* __POWERPC__ */
	
	p = ( wchar_t *) set - 1;

	while (c = *++p)
		set_char_map(map, c);
	
	p = s - 1;
	
	while (c = *++p)
		if (!tst_char_map(map, c))
			break;
	
	if (!c)
	{
		/*s = n;*/											/*- ejs 020219 -*/
		*ptr = NULL;										/*- mm 000422 -*/
		return(NULL);
	}
	
	q = p;
	
	while (c = *++p)
		if (tst_char_map(map, c))
			break;
	
	if (!c)
		s = n;
	else
	{
		s  = p + 1;
		*p = L'\0';
	}
	
	if (q == NULL)												/*- mm 000422 -*/
		*ptr = NULL;											/*- mm 000422 -*/
	else														/*- mm 000422 -*/
		*ptr = s;												/*- mm 000422 -*/
	return(( wchar_t *) q);

#endif   /* __POWERPC__ */
}





wchar_t * _MSL_CDECL wcsstr(const wchar_t * str, const wchar_t * pat)
{
#if !defined(__POWERPC__)

	wchar_t * s1 = (wchar_t *) str;
	wchar_t * p1 = (wchar_t *) pat;
	wchar_t firstc, c1, c2;
	
	if (!(firstc = *p1++))		/* if pat is an empty string we return str */
		return((wchar_t *) str);

	while(c1 = *s1++)
		if (c1 == firstc)
		{
			const wchar_t * s2 = s1;
			const wchar_t * p2 = p1;
			
			while ((c1 = *s2++) == (c2 = *p2++) && c1);
			
			if (!c2)
				return((wchar_t *) s1 - 1);
		}
	
	return(NULL);

#else

	wchar_t * s1 = (wchar_t *) str-1;
	wchar_t * p1 = (wchar_t *) pat-1;
	wchar_t firstc, c1, c2;
	
	if (!(firstc = *++p1))		/* if pat is an empty string we return str */
		return((wchar_t *) str);

	while(c1 = *++s1)
		if (c1 == firstc)
		{
			const wchar_t * s2 = s1-1;
			const wchar_t * p2 = p1-1;
			
			while ((c1 = *++s2) == (c2 = *++p2) && c1);
			
			if (!c2)
				return((wchar_t *) s1);
		}
	
	return(NULL);

#endif
}

#pragma warn_possunwant reset

#endif /* #if _MSL_WIDE_CHAR */			/*- mm 981030 -*/

/* Change record:
 * JCM 980121 First code release.
 * mf  980318 the header file string.win32.h does not implement
              wide versions of the string functions so these 
              were not defined at all
 * mf  980512 wince changes                 
 * mm  981030 Added #if _MSL_WIDE_CHAR wrappers
 * mm  990630 Corrected wcsxfrm
 * mm  990914 Corrected size of char_map to allow for full wchar character set IR9909-0523
 * mm  000419 Initialized char_map objects without using a static object---thanks to Andreas
 * mm  000422 Make wcstok work according to C99
 * mm  000505 Correction to mm000422 for Windows               
 * mm  010316 Deleted the Windows version of wcstok---see note in source.
 * mm  010521 Deleted #include of ThreadLocalData.h
 * cc  011128 Made __tls _MSL_TLS
 * cc  011203 Added _MSL_CDECL for new name mangling 
 * ejs 020219 Removed _MSL_TLS from wcstok: this implementation does not need thread-local storage
 * JWW 020917 Added _MSL_RESTRICT to get the restrict type specifier for certain C99 functions
 * JWW 030224 Changed __NO_WIDE_CHAR flag into the new more configurable _MSL_WIDE_CHAR
 * mm  030304 Added _MSL_C99 wrapper
 * JWW 031030 Added the __STDC_WANT_SECURE_LIB__ secure library extensions
 * JWW 040317 Updated the __STDC_WANT_SECURE_LIB__ secure library based on new secure specifications
 */