/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:34:03 $
 * $Revision: 1.70.2.2 $
 */

/*
 *	Routines
 *	--------
 *		strlen
 *		strnlen_s
 *
 *		strcpy
 *		strcpy_s
 *		strcat
 *		strcat_s
 *		strcmp
 *
 *		strncpy
 *		strncpy_s
 *		strncat
 *		strncat_s
 *		strncmp
 *
 *		strcoll
 *		strxfrm
 *
 *		strchr
 *		strrchr
 *		strstr
 *
 *		strspn
 *		strcspn
 *		strpbrk
 *		strtok
 *		strtok_s
 *
 *		strerror
 *		strerror_s
 */

#define __STDC_WANT_SECURE_LIB__ 1
#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <locale_api.h>
#include <msl_thread_local_data.h>
#include <msl_secure_error.h>

#pragma warn_possunwant off

#if !defined(__MACH__)
#if defined(__INTEL__)

	#include <string.x86.h>

#elif (!defined(__MC68K__) || _No_String_Inlines || !defined(__cplusplus))

	#if __dest_os != __n64_os && __dest_os != __palm_os
		#if !defined(__PPC_EABI__) 
			size_t (strlen)(const char * str)
			{
				size_t len = -1;
				
			#if !defined(__POWERPC__)
				
				do
					len++;
				while (*str++);
				
			#else
				
				unsigned char * p = (unsigned char *) str - 1;
				
				do
					len++;
				while (*++p);
				
			#endif
				
				return(len);
			}
		#endif /* __PPC_EABI__ */
	#endif /* __dest_os != __n64_os && __dest_os != __palm_os */

	#if __dest_os != __palm_os

		#define K1 0x80808080 
		#define K2 0xfefefeff

		#pragma ANSI_strict off
		
		char * (strcpy)(char * _MSL_RESTRICT dst, const char * _MSL_RESTRICT src)
		{
		#if (!defined(__POWERPC__) && !defined(__MIPS__) && !defined(__arm))
			const	char * p = src;
					char * q = dst;
			
			while (*q++ = *p++);
			
		#elif (defined(__PPC_EABI__) || defined(__MIPS__) || defined(__arm))
			
			/*
			 *	strcpy routine designed to minimized the number of
			 *	loads and stores.  We make sure that all the loads and
			 *  stores are properly aligned.
			 *
			 */
			register unsigned char *destb, *fromb;
			register unsigned int w, t, align;
			register unsigned int k1;
			register unsigned int k2;
			
			fromb = (unsigned char *)src;
			destb = (unsigned char *)dst;

			/*
			 *	If either the destination or the source are not 
			 *	aligned on the same boundary, we do a byte copy.  
			 *	Otherwise we align them to a word boundary.
			 */
			if ((align = ((int)fromb & 3)) != ((int)destb & 3)) {
				goto bytecopy;
			}

			if (align) {
				if ((*destb = *fromb) == 0) 
					return(dst);
				for (align = 3 - align; align; align--) {
					if ((*(++destb) = *(++fromb)) == 0) 
						return(dst);
				}
				++destb;
				++fromb;
			}

			/*	Source and destination are word aligned */
			
			k1 = K1;
			k2 = K2;

			/*	Load a word and test for a null byte. */
			w = *((int*)(fromb));
			/*
			 *	Here we need to test if one of the bytes in w is 0, in
			 *	which case we have reached the end of the string.  If we
			 *	reach the end of the string we go to the one-byte-copy 
			 *	block.  This code is endian-neutral.
			 *	The naive way would be to test each byte of the word, but
			 *	that would require at least four comparisons, few shifts,
			 *	and few branches.  This would be extremely costly especially
			 *	on machines where the conditional branch destroys the cache.
			 *		
			 *	Assertion:
			 *	----------
			 *	Let B be a byte.  The most significant bit of 
			 *	(B-1) & ~B will be set, if and only if B==0. 
			 *
			 *	Proof:
			 *	------
			 * 		B			| 	00	| 01..7F|	80	| 81..FF|
			 * 		B-1			|	FF	| 00..7E|	7F	| 80..FE|
			 * 		~B			|	FF	| FE..80|	7F	| 7E..00|
			 *	MSB((B-1)&~B)	|	1	| 0		|	0	| 0		|
			 *
			 *	Adding 0xFF to a byte will always generate a carry
			 *	except when the byte is 0.  So when working with 4
			 *	bytes at a time, we will add FF to the first byte and
			 *	FE to the other bytes because the carry will come 
			 *	from the preceding byte.
			 */
			t = w + k2;
 		 	t &= ~w;                       /*- mm 050126 -*/
			/*
			 *	Extract the most significant bit of each byte.  If one
			 *	bit is left it means we have seen a 0.
			 */
			t &= k1;
			if (t) goto bytecopy;
			--((int*)(destb));
			
			do {
				/*	Put the tested word into the incremented destination. */
				*(++((int*)(destb))) = w;
				/*	Load a word and increment source for testing. */
				w = *(++((int*)(fromb)));
				
				t = w + k2;
 		 		t &= ~w;                   /*- mm 050126 -*/
				t &= k1;
				if (t) goto adjust;
			} while(1);

		adjust:
			++((int*)(destb));
		bytecopy:
			if ((*destb = *fromb) == 0) 
				return(dst);
			do {
				if ((*(++destb) = *(++fromb)) == 0) 
					return(dst);
			} while(1);
		#else
			
			const	unsigned char * p = (unsigned char *) src - 1;
					unsigned char * q = (unsigned char *) dst - 1;
			
			while (*++q = *++p);
		#endif
			
			return(dst);
		}
		#pragma ANSI_strict reset

	#endif /* __dest_os != __palm_os */

#endif /* !__MC68K__ || _No_String_Inlines || !defined(__cplusplus) */

#if !defined(__INTEL__)

	char * strncpy(char * _MSL_RESTRICT dst, const char * _MSL_RESTRICT src, size_t n)
	{
		#if !defined(__POWERPC__)
			
			const	char * p = src;
			char * q = dst;
			
			n++;
			
			while (--n)
				if (!(*q++ = *p++))
				{
					while (--n)
						*q++ = 0;
					break;
				}
			
		#else
			
			const	unsigned char * p		= (const unsigned char *) src - 1;
			unsigned char * q		= (unsigned char *) dst - 1;
			unsigned char zero	= 0;
			
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

	#if __dest_os != __palm_os

		char * strcat(char * _MSL_RESTRICT dst, const char * _MSL_RESTRICT src)
		{
		#if !defined(__POWERPC__)
			
			const	char * p = src;
						char * q = dst;
			
			while (*q++);
			
			q--;
			
			while (*q++ = *p++);
			
		#else
			
			const	unsigned char * p = (unsigned char *) src - 1;
						unsigned char * q = (unsigned char *) dst - 1;
			
			while (*++q);
			
			q--;
			
			while (*++q = *++p);

		#endif
			
			return(dst);
		}

	#endif /* __dest_os != __palm_os */

	char * strncat(char * _MSL_RESTRICT dst, const char * _MSL_RESTRICT src, size_t n)
	{
		#if !defined(__POWERPC__)
			
			const	char * p = src;
						char * q = dst;
			
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
			
			const	unsigned char * p = (unsigned char *) src - 1;
						unsigned char * q = (unsigned char *) dst - 1;
			
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

	#if __dest_os != __palm_os

	#pragma ANSI_strict off
	
		int strcmp(const char * str1, const char * str2)
		{
			#if (!defined(__POWERPC__) && !defined(__MIPS__) && !defined(__MC68K__) && !defined(__arm))
				
				const	unsigned char * p1 = (unsigned char *) str1;
				const	unsigned char * p2 = (unsigned char *) str2;
							unsigned char		c1, c2;
				
				while ((c1 = *p1++) == (c2 = *p2++))
					if (!c1)
						return(0);

				return(c1 - c2);

			#elif (defined(__POWERPC__) || defined(__MIPS__) || defined(__MC68K__) || defined(__arm)) /*- cc 010821 -*/

				/*
				 *	strcmp routine designed to minimized the number of
				 *	loads and stores.  We make sure that all the loads and
				 *  stores are properly aligned.
				 *
				 */
				register unsigned char *left = (unsigned char *)str1;
				register unsigned char *right = (unsigned char *)str2;
				unsigned int k1, k2, align, l1, r1, x;

				/*	Check the simple case of the first byte being different. */
				l1 = *left;
				r1 = *right;
				if (l1 - r1) {
					return (l1 - r1);
				}
				
				/*
				 *	If either the destination or the source are not 
				 *	aligned on the same boundary, we do a byte copy.  
				 *	Otherwise we align them to a word boundary. 
				 */ 
				if ((align = ((int)left & 3)) != ((int)right & 3)) {
					goto bytecopy;
				}
				if (align) {
					/*	Continuation of test of first byte. */
					if (l1 == 0) {
						return (0);
					}
					for (align = 3 - align; align; align--) {
						l1 = *(++left);
						r1 = *(++right);
						if (l1 - r1) {
							return (l1 - r1);
						}
						if (l1 == 0) {
							return (0);
						}
					}
					left++;
					right++;
				}

				/*	Strings are word aligned */
				
				k1 = K1;
				k2 = K2;

				/*	Load a word from each string. */
				l1 = *(int*)left;
				r1 = *(int*)right;
				/*	See comments in strcpy function. */
				x = l1 + k2;
				x &= ~l1;						/*- mm 050126 -*/
				if (x & k1) {
					goto adjust;
				}
				while(l1 == r1) {
					/*	Load a word and increment strings. */
					l1 = *(++((int*)(left)));
					r1 = *(++((int*)(right)));
					x = l1 + k2;
					if (x & k1) {
						goto adjust;
					}
				}
				
			/*	l1 and r1 are not equal.  */
			#if _MSL_LITTLE_ENDIAN
				--left;
				--right;
				goto bytecopy;
			#endif

			adjust:
				l1 = *left;
				r1 = *right;
				if (l1 - r1) {
					return (l1 - r1);
				}
			bytecopy:
				if (l1 == 0) {
					return (0);
				}
				do {
					l1 = *(++left);
					r1 = *(++right);
					if (l1 - r1) {
						return (l1 - r1);
					}
					if (l1 == 0) {
						return (0);
					}
				} while (1);

			#else
				
				const	unsigned char * p1 = (unsigned char *) str1 - 1;
				const	unsigned char * p2 = (unsigned char *) str2 - 1;
						unsigned long		c1, c2;
					
				while ((c1 = *++p1) == (c2 = *++p2))
					if (!c1)
						return(0);

				return(c1 - c2);
			#endif	
		}
		
		#pragma ANSI_strict reset

	#endif /* __dest_os != __palm_os */

	int strncmp(const char * str1, const char * str2, size_t n)
	{
		#if !defined(__POWERPC__)
			
			const	unsigned char * p1 = (unsigned char *) str1;
			const	unsigned char * p2 = (unsigned char *) str2;
						unsigned char		c1, c2;
			
			n++;
			
			while (--n)
				if ((c1 = *p1++) != (c2 = *p2++))
					return(c1 - c2);
				else if (!c1)
					break;
			
		#else
			
			const	unsigned char * p1 = (unsigned char *) str1 - 1;
			const	unsigned char * p2 = (unsigned char *) str2 - 1;
						unsigned long		c1, c2;
			
			n++;
			
			while (--n)
				if ((c1 = *++p1) != (c2 = *++p2))
					return(c1 - c2);
				else if (!c1)
					break;

		#endif
		
		return(0);
	}

	#if __dest_os != __n64_os && __dest_os != __palm_os
		char * strchr(const char * str, int chr)
		{
		#if !defined(__POWERPC__)

			const char * p = str;
			      char   c = chr;
			      char   ch;
			
			while(ch = *p++)
				if (ch == c)
					return((char *) (p - 1));
			
			return(c ? 0 : (char *) (p - 1));

		#else

			const unsigned char * p = (unsigned char *) str - 1;
			      unsigned long   c = (chr & 0xff);               /*- mm 970327 -*/
			      unsigned long   ch;
			
			while(ch = *++p)                             /*- mm 970327 -*/ /*- mm 980604 -*/
				if (ch == c)
					return((char *) p);
			
			return(c ? 0 : (char *) p);

		#endif
		}

	#endif /* __dest_os != __n64_os && __dest_os != __palm_os */
	
#endif /* __INTEL__ */
#endif /* __MACH__ */

/* begin complete rewrite */	/*- mm 020718 -*/
int _MSL_CDECL strcoll(const char *str1, const char * str2)
{
	int i;
	char * str1coll;
	char * str2coll;
	int res;
	int len1, len2;
											
	len1 	 = MB_LEN_MAX * (strlen(str1) + 1);
	len2 	 = MB_LEN_MAX * (strlen(str2) + 1);
	str1coll = malloc(len1);
	str2coll = malloc(len2);
	i 		 = strxfrm(str1coll, str1, len1);
	i 		 = strxfrm(str2coll, str2, len2);
	res 	 = strcmp(str1coll, str2coll);
	free(str1coll);
	free(str2coll);
	return(res);
}

size_t _MSL_CDECL strxfrm(char * _MSL_RESTRICT str1, const char * _MSL_RESTRICT str2, size_t n)
{
	int len = strlen(str2);
	
	unsigned char * chptr;
	unsigned char * collptr;
	unsigned char * weight1;
	unsigned char * weight2;
	unsigned char * wt1ptr, *wt2ptr, *tempptr;
	struct __locale *    current_locale_ptr;
	int offset;
	int tabsize;
	int i;
	unsigned short ch;
	int chrval;
	size_t         reslen;
	unsigned char *localstr1 = malloc(MB_LEN_MAX * (strlen(str2)+1));						/*- mm 020731 -*/
	
	current_locale_ptr = &_MSL_LOCALDATA(_current_locale);
	offset             = current_locale_ptr->coll_cmpt_ptr->char_start_value;
	tabsize            = current_locale_ptr->coll_cmpt_ptr->char_coll_tab_size;
	chptr              = (unsigned char*)str2;
	collptr            = (unsigned char*)localstr1;
	weight1            = malloc(len);
	weight2            = malloc(len);
	for (wt1ptr = weight1; wt1ptr < (weight1 + len); wt1ptr++)
		*wt1ptr = '\0';
	for (wt2ptr = weight2; wt2ptr < (weight2 + len); wt2ptr++)
		*wt2ptr = '\0';
	wt1ptr = weight1;
	wt2ptr = weight2;
		
	for (i = 0; i < strlen(str2); i++)
	{
		chrval = (int)*chptr++;
		if ((chrval >= offset) && (chrval <= (offset + tabsize)))
		{
			ch = *(current_locale_ptr->coll_cmpt_ptr->char_coll_table_ptr + (chrval - offset));
			if (ch != 0)
			{
				*collptr++ = (unsigned char)(ch & 0x00ff);
				if ((ch & 0x0f00) != 0)
					*wt1ptr++ = (unsigned char)((ch & 0x0f00) >> 8);
				if ((ch & 0xf000) != 0)
					*wt2ptr++ = (unsigned char)((ch & 0xf000) >> 12);
			}
		}
	}
	if (current_locale_ptr->coll_cmpt_ptr->char_spec_accents)
		for (tempptr = wt1ptr - 1; tempptr >= weight1; tempptr--)
			*collptr++ = *tempptr;
	else
		for (tempptr = weight1; tempptr < wt1ptr; tempptr++)
			*collptr++ = *tempptr;
		
	for (tempptr = weight2; tempptr < wt2ptr; tempptr++)
		*collptr++ = *tempptr;
	
	*collptr = '\0';
	free(weight1);
	free(weight2);
	reslen = strlen((const char *)localstr1);
	if (str1 != NULL)
		strncpy(str1, (const char *)localstr1, n);
	free(localstr1);									/*- mm 040602 -*/
	return(reslen);
}
/* end complete rewrite */	/*- mm 020718 -*/

char * _MSL_CDECL strrchr(const char * str, int chr)
{
	#if !defined(__POWERPC__)

		const char * p = str;
		const char * q = 0;
		      char   c = (char)chr;
		      char   ch;
		
		while(ch = *p++)
			if (ch == c)
				q = p - 1;
		
		if (q)
			return((char *) q);
		
		return(c ? 0 : (char *) (p - 1));

	#else

		const unsigned char * p = (unsigned char *) str - 1;
		const unsigned char * q = 0;
		      unsigned long   c = (chr & 0xff);				/*- bb 970530 -*/
		      unsigned long   ch;
		
		while(ch = *++p)								/*- bb 970530 -*/ /*- mm 980604 -*/
			if (ch == c)
				q = p;
		
		if (q)
			return((char *) q);
		
		return(c ? 0 : (char *) p);

	#endif
}

typedef unsigned char char_map[32];

#define set_char_map(map, ch)  map[(unsigned char)(ch)>>3] |= (unsigned char)(1 << ((ch)&7))
#define tst_char_map(map, ch) (map[(unsigned char)(ch)>>3] &  (unsigned char)(1 << ((ch)&7)))

char * _MSL_CDECL strpbrk(const char * str, const char * set)
{
	const unsigned char * p;
	int c;
	char_map map = {0};
	
	#if !defined(__POWERPC__)
		
		p = (unsigned char *) set;

		while (c = *p++)
			set_char_map(map, c);
		
		p = (unsigned char *) str;
		
		while (c = *p++)
			if (tst_char_map(map, c))
				return((char *) (p - 1));
				
		return(NULL);

	#else
		
		p = (unsigned char *) set - 1;

		while (c = *++p)
			set_char_map(map, c);
		
		p = (unsigned char *) str - 1;
		
		while (c = *++p)
			if (tst_char_map(map, c))
				return((char *) p);
				
		return(NULL);

	#endif
}

size_t _MSL_CDECL strspn(const char * str, const char * set)
{
	const unsigned char *	p;
	int	c;
	char_map map = {0};
	
	#if !defined(__POWERPC__)
		
		p = (unsigned char *) set;

		while (c = *p++)
			set_char_map(map, c);
		
		p = (unsigned char *) str;
		
		while (c = *p++)
			if (!tst_char_map(map, c))
				break;
				
		return (size_t)(p - (unsigned char *) str - 1);

	#else
		
		p = (unsigned char *) set - 1;

		while (c = *++p)
			set_char_map(map, c);
		
		p = (unsigned char *) str - 1;
		
		while (c = *++p)
			if (!tst_char_map(map, c))
				break;
				
		return(p - (unsigned char *) str);

#endif
}

size_t _MSL_CDECL strcspn(const char * str, const char * set)
{
	const unsigned char *	p;
	      int							c;
				char_map				map = {0};
	
	#if !defined(__POWERPC__)
		
		p = (unsigned char *) set;

		while (c = *p++)
			set_char_map(map, c);
		
		p = (unsigned char *) str;
		
		while (c = *p++)
			if (tst_char_map(map, c))
				break;
				
		return(size_t)(p - (unsigned char *) str - 1);

	#else
		
		p = (unsigned char *) set - 1;

		while (c = *++p)
			set_char_map(map, c);
		
		p = (unsigned char *) str - 1;
		
		while (c = *++p)
			if (tst_char_map(map, c))
				break;
				
		return(p - (unsigned char *) str);

	#endif
}

char * _MSL_CDECL strtok(char * _MSL_RESTRICT str, const char * _MSL_RESTRICT set)
{
	#if !_MSL_THREADSAFE || !_MSL_LOCALDATA_AVAILABLE
		static unsigned char *	strtok_n = (unsigned char *) "";
		static unsigned char *	strtok_s = (unsigned char *) "";
	#endif
	
	unsigned char *p, *q;
    int c;
    
	char_map map = {0};
	
	if (str)
		_MSL_LOCALDATA(strtok_s) = (unsigned char *) str;
	
	#if !defined(__POWERPC__)
	
		p = (unsigned char *) set;
	
		while (c = *p++)
			set_char_map(map, c);
	
		p = _MSL_LOCALDATA(strtok_s);
	
		while (c = *p++)
			if (!tst_char_map(map, c))
				break;
	
		if (!c)
		{
			_MSL_LOCALDATA(strtok_s) = _MSL_LOCALDATA(strtok_n);
			return(NULL);
		}
	
		q = p - 1;
	
		while (c = *p++)
			if (tst_char_map(map, c))
				break;
	
		if (!c)
			_MSL_LOCALDATA(strtok_s) = _MSL_LOCALDATA(strtok_n);
		else
		{
			_MSL_LOCALDATA(strtok_s) = p;
			*--p = 0;
		}
	
		return((char *) q);
	
	#else
	
		p = (unsigned char *) set - 1;
		
		while (c = *++p)
			set_char_map(map, c);
		
		p = _MSL_LOCALDATA(strtok_s) - 1;
		
		while (c = *++p)
			if (!tst_char_map(map, c))
				break;
		
		if (!c)
		{
			_MSL_LOCALDATA(strtok_s) = _MSL_LOCALDATA(strtok_n);
			return(NULL);
		}
		
		q = p;
		
		while (c = *++p)
			if (tst_char_map(map, c))
				break;
		
		if (!c)
			_MSL_LOCALDATA(strtok_s) = _MSL_LOCALDATA(strtok_n);
		else
		{
			_MSL_LOCALDATA(strtok_s) = p + 1;
			*p = 0;
		}
		
		return((char *) q);
	
	#endif
}

char * _MSL_CDECL strtok_s(char * _MSL_RESTRICT str, const char * _MSL_RESTRICT set, char ** _MSL_RESTRICT ptr)
{
	unsigned char *p, *q;
    int c;
    
	unsigned char *	n = (unsigned char *) "";
	char_map map = {0};
	
	if (((set == NULL) || (ptr == NULL)) || ((*ptr == NULL) && (str == NULL)))
	{
		__msl_undefined_behavior_s();
		return NULL;
	}
	
	if (str)
		*ptr = str;
	
	#if !defined(__POWERPC__)
		
		p = (unsigned char *) set;
		
		while (c = *p++)
			set_char_map(map, c);
		
		p = (unsigned char *) *ptr;
		
		while (c = *p++)
			if (!tst_char_map(map, c))
				break;
		
		if (!c)
		{
			*ptr = (char *) n;
			return(NULL);
		}
		
		q = p - 1;
		
		while (c = *p++)
			if (tst_char_map(map, c))
				break;
		
		if (!c)
			*ptr = (char *) n;
		else
		{
			*ptr = (char *) p;
			*--p = 0;
		}
		
		return((char *) q);
		
	#else
		
		p = (unsigned char *) set - 1;
		
		while (c = *++p)
			set_char_map(map, c);
		
		p = (unsigned char *) *ptr - 1;
		
		while (c = *++p)
			if (!tst_char_map(map, c))
				break;
		
		if (!c)
		{
			*ptr = (char *) n;
			return(NULL);
		}
		
		q = p;
		
		while (c = *++p)
			if (tst_char_map(map, c))
				break;
		
		if (!c)
			*ptr = (char *) n;
		else
		{
			*ptr = (char *) p + 1;
			*p = 0;
		}
		
		return((char *) q);
		
	#endif
}

char * _MSL_CDECL strstr(const char * str, const char * pat)
{
	#if !defined(__POWERPC__)

		unsigned char * s1 = (unsigned char *) str;
		unsigned char * p1 = (unsigned char *) pat;
		unsigned char firstc, c1, c2;
		
		if ((pat == NULL) || (!(firstc = *p1++)))
											/*- mm 980424 -*/
											/*- beb 971017 -*/
			return((char *) str);

		while(c1 = *s1++)
			if (c1 == firstc)
			{
				const unsigned char * s2 = s1;
				const unsigned char * p2 = p1;
				
				while ((c1 = *s2++) == (c2 = *p2++) && c1);
				
				if (!c2)
					return((char *) s1 - 1);
			}
		
		return(NULL);

	#else

		unsigned char * s1 = (unsigned char *) str-1;
		unsigned char * p1 = (unsigned char *) pat-1;
		unsigned long firstc, c1, c2;
		
		if ((pat == NULL) || (!(firstc = *++p1)))    /*- vss 980807 -*/
											/*- mm 980424  -*/
											/*- beb 971017 -*/
			return((char *) str);

		while(c1 = *++s1)
			if (c1 == firstc)
			{
				const unsigned char * s2 = s1-1;
				const unsigned char * p2 = p1-1;
				
				while ((c1 = *++s2) == (c2 = *++p2) && c1);
				
				if (!c2)
					return((char *) s1);
			}
		
		return(NULL);

	#endif
}

size_t (strnlen_s)(const char * str, size_t maxsize)
{
	unsigned char *end;
	unsigned char *p;
	size_t len = -1;
	
	if ((maxsize == 0) || (str == NULL))
		return 0;
	
	end = (unsigned char *) &(str[maxsize - 1]);
	p = (unsigned char *) str - 1;
	
	do
		len++;
	while ((end > p) && *++p);
	
	return len;
}

errno_t (strcpy_s)(char * _MSL_RESTRICT dst, rsize_t dst_max, const char * _MSL_RESTRICT src)
{
	if ((dst_max == 0) || (dst == NULL) || (src == NULL) || (dst_max > RSIZE_MAX))
	{
		__msl_undefined_behavior_s();
		return ERANGE;
	}
	
	if (dst_max <= strnlen_s(src, dst_max))
	{
		__msl_undefined_behavior_s();
		dst[0] = 0;
		return ERANGE;
	}
	
	strcpy(dst, src);
	return ENOERR;
}

errno_t strncpy_s(char * _MSL_RESTRICT dst, rsize_t dst_max, const char * _MSL_RESTRICT src, rsize_t n)
{
	rsize_t i;
	
	if ((dst_max == 0) || (dst == NULL) || (src == NULL) || (dst_max > RSIZE_MAX) || (n > RSIZE_MAX))
	{
		__msl_undefined_behavior_s();
		return ERANGE;
	}
	
	if (dst_max <= n)
	{
		for (i = 0; i < dst_max; i++)
			if (src[i] == 0)
			{
				strcpy(dst, src);
				return ENOERR;
			}
		__msl_undefined_behavior_s();
		dst[0] = 0;
		return ERANGE;
	}
	
	strncpy(dst, src, n);
	dst[n] = 0;
	return ENOERR;
}

errno_t (strcat_s)(char * _MSL_RESTRICT dst, rsize_t dst_max, const char * _MSL_RESTRICT src)
{
	rsize_t dst_cat_max;
	
	if ((dst == NULL) || (src == NULL) || (dst_max > RSIZE_MAX))
	{
		__msl_undefined_behavior_s();
		return ERANGE;
	}
	
	dst_cat_max = dst_max - strnlen_s(dst, dst_max);
	
	if (dst_cat_max == 0)
	{
		__msl_undefined_behavior_s();
		return ERANGE;
	}
	
	if (dst_cat_max <= strnlen_s(src, dst_max))
	{
		__msl_undefined_behavior_s();
		dst[0] = 0;
		return ERANGE;
	}
	
	strcat(dst, src);
	return ENOERR;
}

errno_t strncat_s(char * _MSL_RESTRICT dst, rsize_t dst_max, const char * _MSL_RESTRICT src, rsize_t n)
{
	rsize_t i;
	rsize_t dst_cat_max;
	
	if ((dst == NULL) || (src == NULL) || (dst_max > RSIZE_MAX) || (n > RSIZE_MAX))
	{
		__msl_undefined_behavior_s();
		return ERANGE;
	}
	
	dst_cat_max = dst_max - strnlen_s(dst, dst_max);
	
	if (dst_cat_max == 0)
	{
		__msl_undefined_behavior_s();
		return ERANGE;
	}
	
	if (dst_cat_max < n)
	{
		for (i = 0; i < dst_cat_max; i++)
			if (src[i] == 0)
			{
				strncat(dst, src, n);
				return ENOERR;
			}
		__msl_undefined_behavior_s();
		dst[0] = 0;
		return ERANGE;
	}
	
	strncat(dst, src, n);
	if (dst_cat_max != n)
		dst[dst_max-dst_cat_max+n] = 0;
	return ENOERR;
}

#if __dest_os != __palm_os

	char * _MSL_CDECL strerror(int errnum)
	{
		static char errstr[__max_errstr];
		
		return(__strerror(errnum, errstr));
	}

	errno_t _MSL_CDECL strerror_s(char *s, rsize_t maxsize, errno_t errnum)
	{
		int realsize;
		char errstr[__max_errstr];
		
		if ((s == NULL) || (maxsize > RSIZE_MAX) || (maxsize == 0))
		{
			__msl_undefined_behavior_s();
			return ERANGE;
		}
		
		__strerror(errnum, errstr);
		realsize = strlen(errstr);
		
		if (maxsize <= realsize)
		{
			if (maxsize > 0)
			{
				strncpy(s, errstr, maxsize-1);
				s[maxsize-1] = 0;
				if (maxsize > 3)
				{
					s[maxsize-2] = '.';
					s[maxsize-3] = '.';
					s[maxsize-4] = '.';
				}
			}
			return ERANGE;
		}
		
		strcpy(s, errstr);
		return ENOERR;
	}

	char * _MSL_CDECL __strerror(int errnum, char * str)
	{
		switch (errnum)
		{
	#if _MSL_STRERROR_KNOWS_ERROR_NAMES
			/* begin mm 010412 changes */    								/*- mm 010412 -*/
			case E2BIG:			strcpy(str, "Argument list too long");				break;
			case EACCES:		strcpy(str, "Permission denied");					break;
			case EAGAIN:		strcpy(str, "Resource temporarily unavailable");	break;
			case EBADF:			strcpy(str, "Bad file descriptor");					break;	
			case EBUSY:			strcpy(str, "Device busy");							break;	
			case ECHILD:		strcpy(str, "No child processes");					break;	
			case EDEADLK:		strcpy(str, "Resource deadlock avoided");			break;	
			case EDOM:			strcpy(str, "Numerical argument out of domain");	break;	
			case EEXIST:		strcpy(str, "File exists");							break;	
			case EFAULT:		strcpy(str, "Bad address");							break;	
			case EFBIG:			strcpy(str, "File too large");						break;	
			case EFPOS:			strcpy(str, "File Position Error");					break;	
	#if _MSL_C99												/*- mm 030304 -*/	
			case EILSEQ:		strcpy(str, "Wide character encoding error");		break;	
	#endif /* _MSL_C99 */										/*- mm 030304 -*/	
			case EINTR:			strcpy(str, "Interrupted system call");				break;	
			case EINVAL:		strcpy(str, "Invalid argument");					break;	
			case EIO:			strcpy(str, "Input/output error");					break;	
			case EISDIR:		strcpy(str, "Is a directory");						break;	
	#if ((__dest_os == __mac_os) || (__dest_os == __mac_os_x))
			case EMACOSERR:		sprintf(str, "Mac OS error  (%d)", __MacOSErrNo); 	break;
	#endif
			case EMFILE:		strcpy(str, "Too many open files");					break;	
			case EMLINK:		strcpy(str, "Too many links");						break;	
			case ENAMETOOLONG:	strcpy(str, "File name too long");					break;	
			case ENFILE:		strcpy(str, "Too many open files in system");		break;	
			case ENODEV:		strcpy(str, "Operation not supported by device");	break;	
			case ENOENT:		strcpy(str, "No such file or directory");			break;	
			case ENOERR:		strcpy(str, "No error detected");					break;
			case ENOEXEC:		strcpy(str, "Exec format error");					break;	
			case ENOLCK:		strcpy(str, "No locks available");					break;	
			case ENOMEM:		strcpy(str, "Cannot allocate memory");				break;	
			case ENOSPC:		strcpy(str, "No space left on device");				break;	
			case ENOSYS:		strcpy(str, "Function not implemented");			break;		
			case ENOTDIR:		strcpy(str, "Not a directory");						break;	
			case ENOTEMPTY:		strcpy(str, "Directory not empty");					break;	
			case ENOTTY:		strcpy(str, "Inappropriate ioctl for device");		break;	
			case ENXIO:			strcpy(str, "Device not configured");				break;	
			case EPERM:			strcpy(str, "Operation not permitted");				break;	
			case EPIPE:			strcpy(str, "Broken pipe");							break;	
			case ERANGE:		strcpy(str, "Result too large");					break;	
			case EROFS:			strcpy(str, "Read-only file system");				break;	
			case ESIGPARM:		strcpy(str, "Signal error");						break;	
			case ESPIPE:		strcpy(str, "Illegal seek");						break;	
			case ESRCH:			strcpy(str, "No such process");						break;	
			case EUNKNOWN:		strcpy(str, "Unknown error");						break;	
			case EXDEV:			strcpy(str, "Cross-device link");					break;
			/* end mm 010412 changes */    									/*- mm 010412 -*/
	#endif /* _MSL_STRERROR_KNOWS_ERROR_NAMES */
			default:			sprintf(str, "Unknown Error (%d)", errnum); 		break;
		}
		
		return(str);
	}

#endif /* __dest_os != __palm_os */

#pragma warn_possunwant reset

/* Change record:
 * JFH 950524 First code release.
 * JFH 950623 Fixed strstr's tendency to not find patterns that aren't
 *			  suffixes of the target string. Dumb mistake.
 * JFH 951129 Tweaked strncpy, strcmp, and strncmp for efficiency.
 * JFH 951208 Fixed bug in PPC version of strpbrk (returned p-1 instead of p).
 * JFH 951208 Fixed bug in PPC version of strrchr (was copy of PPC version of strchr).
 * JFH 960122 Bracketed strlen() and strcpy() by #if <condition too complex to put here>
 *			  (they are inlined in <string.h> if this condition fails).
 * JFH 960429 Merged Win32 changes in.
 * KO  961219 Added some Win32 ifdefs to use my thread local data structure rather
 *            than static local variables.
 * mm  970327 Change to strchr to allow chars beyond 128 to work with signed chars
 * bb  970530 Applied mm970327 change to the strrchr routine.
 * beb 971017 Fix string return value, should be str, not s1
 * mm  980424 Modify strstr so that if the pattern pointer is NULL, a pointer to str is
 *            returned.  The Standard leaves this undefined so defined a consistent result.
 * mf  980429 changed macro __dest_os_== win32_os to __INTEL__ so all intel oses pick
 *            up optimized string
 *            functions (eg. wince and beos)
 * mf  980512 wince changes
 * mm  980604 Removed part of mm970327 from strchr and strrchr as not needed to simplify code.
 *            MW06890
 * vss 980804 PPC must be pre-increment
 * vss 980807 Fix inadvertant mistype of increment operator on pointer to pattern in strstr
 * mf  981116 mips optimizations for strcmp etc...
 * mf  990120 changed intel header file name from string.win32.h to string.x86.h
 * mm  990630 Corrections to strxfrm
 * ad  990128 Added n64 to diff out strlen and strchr
 * pc  990802 Don't need 8 bytes of storage for a const
 * cc  000326 removed dest_os to be_os
 * mm  010412 Rewrote error codes
 * mm  010521 Added _MWMT wrappers
 * cc  010531 Added _GetThreadLocalData's flag
 * cc  010821 Added eppc to core
 * cc  011128 Made __tls _MSL_TLS
 * cc  011203 Added _MSL_CDECL for new name mangling
 * JWW 020130 Changed _MWMT to _MSL_THREADSAFE for consistency's sake
 * mm  020718 Changes to strcoll and strxfrm for locale specification of collate
 * mm  020731 Change for behavior of Windows string function when given null argument
 * BLC 020829 Changes for CW Palm OS V9's MSL for Palm OS
 * JWW 020917 Added _MSL_RESTRICT to get the restrict type specifier for certain C99 functions
 * BLC 020921 Enabled strncpy for Palm OS, since Palm's version doesn't match standard
 * BLC 020925 Enabled strstr for Palm OS, again, OS version doesn't match standard.
 * cc  021004 Added _MSL_STRERROR_KNOWS_ERROR_NAMES to bring down data size
 * JWW 021031 Made thread local data available for Mach-O
 * mm  030304 Added _MSL_C99 wrapper
 * cc  030820 Changed strcoll and strfrm to use MB_LEN_MAX
 * cc  030926 Added BE check to strcmp
 * JWW 031030 Added the __STDC_WANT_SECURE_LIB__ secure library extensions
 * JWW 040317 Updated the __STDC_WANT_SECURE_LIB__ secure library based on new secure specifications
 * mm  040602 Freed a piece of malloced storage
 * JWW 040908 Obtain fast Mach-O versions of strcpy, strcmp, etc. from the System.framework
 * mm  050126 Correct alignment cases.
 */