/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:34:02 $
 * $Revision: 1.22.2.2 $
 */

/*
 *	Routines
 *	--------
 *		qsort
 *		qsort_s
 *
 *	Implementation
 *	--------------
 *
 *		Here we use Heapsort, after Knuth's "The Art of Computer Programming, Vol. 3",
 *		Section 5.2.3. Heapsort was chosen because it requires no auxiliary storage and
 *		has excellent average *and* worst-case performance.
 *
 *	Future Enhancements
 *	-------------------
 *		
 *		Although this algorithm is theoretically efficient, it is not as efficient
 *		as it could be for certain special cases. Specifically, the Swap routine
 *		could be more efficient in the same way memcpy et al. could be, i.e., by
 *		moving longs instead of bytes whenever possible. This would only be
 *		beneficial with a large member_size, though.
 *		
 *		For smaller member_size's, many Swap operations could be eliminated
 *		entirely. We swap on each iteration through the 'sift' operation even
 *		though, in theory, we can pick up a value before the loop, put it in a
 *		temporary, and drop it in its final destination after the loop. Where we
 *		now swap in the loop, we would merely need to replace:
 *		
 *		   swap(ip,jp,member_size);
 *		
 *		with:
 *		
 *		   memcpy(ip,jp,member_size);
 *		
 *		which involves less work.
 *		
 *		We do the swap because we don't know, in general, how big a temporary we
 *		would be needing. We could conceivably malloc a temporary as needed and
 *		resort to the algorithm used here only when we can't get the memory
 *		needed. However, for reasonably small member_size's, say <= 64, we could
 *		use a statically allocated temporary.
 *		
 *		And finally, we can squeeze a lot of cycles for the special cases of
 *		member_size's of 1, 2, and 4 bytes, where we could presumably use a
 *		register for the temporary.
 */

#define __STDC_WANT_SECURE_LIB__ 1
#include <stdint.h>
#include <stdlib.h>
#include <msl_secure_error.h>

#define table_ptr(i)	(((char *) table_base) + (member_size * ((i) - 1)))

#if !defined(__POWERPC__)

#define swap(dst, src, cnt)                                             \
do {                                                                    \
  char *  p;                                                            \
  char *  q;                                                            \
  size_t  n = cnt;                                                      \
                                                                        \
  char tmp;                                                             \
                                                                        \
  for (p = src, q = dst, n++; --n;)                                     \
  {                                                                     \
    tmp = *q;                                                           \
    *q++ = *p;                                                          \
    *p++ = tmp;                                                         \
  }                                                                     \
} while (0)
	
#else

#define swap(dst, src, cnt)                                             \
do {                                                                    \
  char *  p;                                                            \
  char *  q;                                                            \
  size_t  n = cnt;                                                      \
                                                                        \
  unsigned long tmp;                                                    \
                                                                        \
  for (p = (char *) src - 1, q = (char *) dst - 1, n++; --n;)  			\
  {                                                                     \
    tmp = *++q;                                                         \
    *q = *++p;                                                          \
    *p = tmp;                                                           \
  }                                                                     \
} while (0)

#endif

void _MSL_CDECL qsort(void * table_base, size_t num_members, size_t member_size,
					 _compare_function compare_members)                          /*- mm 961031 -*/
{
	size_t	l, r, j;
	char *	lp;
	char *	rp;
	char *	ip;
	char *	jp;
	char *	kp;
	
	if (num_members < 2)
		return;
	
	r = num_members;
	l = (r / 2) + 1;
	
	lp = table_ptr(l);
	rp = table_ptr(r);
	
	for (;;)
	{
		if (l > 1)
		{
			l--;
			lp -= member_size;
		}
		else
		{
			swap(lp,rp,member_size);
			
			if (--r == 1)
				return;
				
			rp -= member_size;
		}
			
		j = l;
		
		jp = table_ptr(j);
		
		while (j*2 <= r)
		{
			j *= 2;
			
			ip = jp;
			jp = table_ptr(j);
			
			if (j < r)
			{
				kp = jp + member_size;
				
				if (compare_members(jp, kp) < 0)
				{
					j++;
					jp = kp;
				}
			}
			
			if (compare_members(ip,jp) < 0)
				swap(ip,jp,member_size);	
			else
				break;
		}
	}
}

void _MSL_CDECL qsort_s(void * table_base, rsize_t num_members, rsize_t member_size,
					 _compare_function_s compare_members, void * context)
{
	rsize_t	l, r, j;
	char *	lp;
	char *	rp;
	char *	ip;
	char *	jp;
	char *	kp;
	
	if ((num_members > RSIZE_MAX) || (member_size > RSIZE_MAX) || ((num_members > 0) &&
		(table_base == NULL) || (compare_members == NULL)))
	{
		__msl_undefined_behavior_s();
		return;
	}
	
	if (num_members < 2)
		return;
	
	r = num_members;
	l = (r / 2) + 1;
	
	lp = table_ptr(l);
	rp = table_ptr(r);
	
	for (;;)
	{
		if (l > 1)
		{
			l--;
			lp -= member_size;
		}
		else
		{
			swap(lp,rp,member_size);
			
			if (--r == 1)
				return;
				
			rp -= member_size;
		}
			
		j = l;
		
		jp = table_ptr(j);
		
		while (j*2 <= r)
		{
			j *= 2;
			
			ip = jp;
			jp = table_ptr(j);
			
			if (j < r)
			{
				kp = jp + member_size;
				
				if (compare_members(jp, kp, context) < 0)
				{
					j++;
					jp = kp;
				}
			}
			
			if (compare_members(ip,jp, context) < 0)
				swap(ip,jp,member_size);	
			else
				break;
		}
	}
}

/* Change record:
 * JFH 950718 First code release.
 * JFH 951204 Fixed small (!) bug in PPC version of swap. Was
 *            pre-incrementing on store instead of fetch.
 * mm  961031 Improved legibility of function declaration
 * cc  011203 Added _MSL_CDECL for new name mangling 
 * BLC 020829 Use of "unsigned char" for tmp in swap macro resulted in
 *            implicit conversion errors on 68K.
 * JWW 031030 Added the __STDC_WANT_SECURE_LIB__ secure library extensions
 */