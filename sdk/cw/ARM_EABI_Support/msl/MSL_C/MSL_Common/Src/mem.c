/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:33:27 $
 * $Revision: 1.47.2.2 $
 */

/*
 *	Routines
 *	--------
 *		memcpy
 *		memcpy_s
 *		memmove
 *		memmove_s
 *		memset
 *		memchr
 *		memcmp
 */

#define __STDC_WANT_SECURE_LIB__ 1
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <mem_funcs.h>
#include <msl_secure_error.h>

#if defined(__INTEL__)
	
	#include <mem.x86.h> /*- mf 012099 -*/

#elif defined(__MACH__)

	/* 
	 * Faster standard implementations of memcpy, memmove, etc. all live in the System.framework
	 * so there is no need for MSL to try to reproduce them.  Just pull in the custom
	 * routines that MSL needs to operate.
	 */
	
	void * __memrchr(const void *src, int val, size_t n)
	{
		const unsigned char *p;
		unsigned long v = (val & 0xff);
		
		for (p = (unsigned char *) src + n, n++; --n;)
			if (*--p == v)
				return((void *) p);
		
		return(NULL);
	}

#else  /* Be, MacOS, all others. . */

	#if (__dest_os == __mac_os)
		#include <MacMemory.h>		/*- vss 990421 -*/
	#endif

	#if ((__dest_os == __mac_os) && defined(__POWERPC__))
		#define _MSL_LONG_COPY_MIN	32	/* NEVER let this be < 16 */
	#endif

#if !(defined(__MC68K__) && !defined(_No_BlockMove))
	#if !defined(__PPC_EABI__)   /*- mm 010406 -*/
	void * (memcpy)(register void * _MSL_RESTRICT dst, register const void * _MSL_RESTRICT src, register size_t n)
	{
		#if __dest_os == __mac_os && !defined(_No_BlockMove)
			
				#if __POWERPC__  /*  PowerPC && mac_os optimization  */
		
/*Assumptions:		990129 BLC  
 * 
 * - Miss aligned reads are supported (on a G3 miss aligned reads seem to 
 * preform better than the code for manually aligning the data). To kept the 
 * penalty to a minimum, I use miss-aligned reads but aligned writes.
 * 
 * - The processor supports floating point, and if the both operands can 
 * become double aligned it is much faster to use doubles to copy the data.
 *
 * - The 2.2 compiler is required for optimal performance since it will
 *   unroll the loops.
 * 
 */
    			const   char    *p;
            			char    *q;

    			size_t  i;
    			size_t  n2;

    			if (n < _MSL_LONG_COPY_MIN) 
    			{
        			if (n > 0) 
        			{

            			q = ((char *) dst) - 1;
            			p = ((const char *) src) - 1;

            			for (i = 0; i < n; i++) 
            			{
                			*++q = *++p;
            			}
        			}
        			return dst;
    			}

    			/* doing a large copy */

    			q = (char *) dst;
    			p = ((const char *) src);
    			if (((unsigned long) q) & 0x07) 
    			{

       		 	p = ((char *) p) - 1;

        			do {
            			*q++ = *++p;
            			n--;
        			} while (((unsigned long) q) & 0x07);
        			p++;
    			}

    			/* dst is now double aligned */

    			if ((((unsigned long) p) & 0x07) != 0) 
    			{
        			/* src is not double aligned */

        			q = q - 4;
        			p = p - 4;

        			n2 = n / 4;
        			for (i = 0; i < n2; i++) 
        			{
            			q += sizeof(unsigned long);
            			p += sizeof(unsigned long);
            			*((unsigned long *) q) = *((unsigned long *) p);

            			n = n - 4;
        			}
       	 			p = p + 4;
        			q = q + 4;

    			} 
    			else 
    			{
        			/* dst and src are double aligned */

        			q = q - 8;
        			p = p - 8;

        			n2 = n / 8;
        			for (i = 0; i < n2; i++) 
        			{
            			q += sizeof(double);
            			p += sizeof(double);
            			*((double *) q) = *((double *) p);
            			n -= 8;
        			}
        			p = p + 8;
        			q = q + 8;
    			}

    			p--;
    			q--;
    			for (i = 0; i < n; i++) 
    			{
        			*++q = *++p;
    			}
    			
    			return dst;
			
			#else  /*  do BlockMoveData on non-PPC MacOS architectures */
			
				BlockMoveData(src, dst, n);
			
			#endif  /*  __POWERPC  */

		#else /* __dest_os != __mac_os || _No_BlockMove */

			const char * p	=	(char*)src;
			char  * q		=	(char*)dst;
		
			#if !defined(__MIPS__) \
			    && !defined(__SH__) \
			    && !defined(__MCORE__) \
			    && !defined(__arm) \
			    && !defined(__m56800__) \
			    && !defined(__m56800E__) /*- ah 010129 -*/
			    
				if (n >= _MSL_LONG_COPY_MIN)
				{
					if ((((int) dst ^ (int) src)) & 3)
						__copy_longs_unaligned(dst, src, n);
					else
						__copy_longs_aligned(dst, src, n);
			
					return(dst);
				}
			#endif		

			#if !defined(__POWERPC__)
		
				/*for (p = src, q = dst, n++; --n;)*/
				for(n++;--n;)
					*q++ = *p++;
		
			#else
		
				for (p = (const char *) src - 1, q = (char *) dst - 1, n++; --n;)
					*++q = *++p;

			#endif

		#endif /* __dest_os == __mac_os && !_No_BlockMove */
		
		return(dst);
	}

	#endif /*  (!__PPC_EABI__) */							/*- mm 010406 -*/

	void * (memmove)(void * dst, const void * src, size_t n)
	{
		#if __dest_os == __mac_os && !defined(_No_BlockMove)
			#if __POWERPC__	
				#if __VEC__
					vec_memmove(dst, src, n);
				#else
			
/*Assumptions:		990129 BLC  
 * 
 * - Miss aligned reads are supported (on a G3 miss aligned reads seem to 
 * preform better than the code for manually aligning the data). To kept the 
 * penalty to a minimum, I use miss-aligned reads but aligned writes.
 * 
 * - The processor supports floating point, and if the both operands can 
 * become double aligned it is much faster to use doubles to copy the data.
 *
 * - The 2.2 compiler is required for optimal performance since it will
 *   unroll the loops.
 * 
 */				
			
				const	char    *p;
            			char    *q;

    			size_t  i;
    			size_t  n2;

    			if (src > dst) 
    			{
       	 			return memcpy(dst, src, n);
    			}
    			if (dst == src) return dst;

    			p = (const char *) src + n;
    			q = (char *) dst + n;

    			if (n < _MSL_LONG_COPY_MIN) 
    			{

        			for (i = 0; i < n; i++) 
        			{
            			*--q = *--p;
        			}
        			return dst;
    			} 
    			else 
    			{

        		/* we are now handling more the __min_bytes_for_long_copy */

        		/* only align the destination */
        		/* don't use a counting loop here we don't want it unrolled!! */
        			while (((unsigned int) q) & 0x07) 
        			{
            			*--q = *--p;
            			n--;
        			}

        		/* the destination is now double aligned */

        			if ((((unsigned int) p) & 0x07)!=0) 
        			{
            			/* src is not double aligned */

            			n2 = n / 4;
            			for (i = 0; i < n2; i++) 
            			{
                			q -= sizeof(unsigned long);
                			p -= sizeof(unsigned long);
                			*((unsigned long *) q) = *((unsigned long *) p);
                			n -= 4;
            			}

        			} 
        			else 
        			{
            			/* dst and src are double aligned */

            			n2 = n / 8;
            			for (i = 0; i < n2; i++) 
            			{
                			q -= sizeof(double);
                			p -= sizeof(double);
                			*((double *) q) = *((double *) p);
                			n -= 8;
            			}
        			}

        			/* finish any odd bytes */
        			for (i = 0; i < n; i++) 
        			{
           	 			*--q = *--p;
        			}
    			}
    			
    			return dst;
	
			#endif /* __VEC__ */

			#else  /* end optimization __POWERPC__ && MacOS architectures */ 
			
				BlockMoveData(src, dst, n);
				
			#endif /* __POWERPC__  */

		#else /* __dest_os != __mac_os || _No_BlockMove */

			const	char * p;
					char * q;
					int	rev = ((unsigned long) src < (unsigned long) dst);
					
			#if !defined(__MIPS__) \
			    && !defined(__SH__) \
			    && !defined(__MCORE__) \
			    && !defined(__arm) \
			    && !defined(__m56800__) \
			    && !defined(__m56800E__) /*- ah 010129 -*/
			
				if (n >= _MSL_LONG_COPY_MIN)
				{
					if ((((int) dst ^ (int) src)) & 3)
						if (!rev)
							__copy_longs_unaligned(dst, src, n);
						else
							__copy_longs_rev_unaligned(dst, src, n);
					else
						if (!rev)
							__copy_longs_aligned(dst, src, n);
						else
							__copy_longs_rev_aligned(dst, src, n);
			
					return(dst);
				}
				
			#endif
			
			if (!rev)
			{
		
				#if !defined(__POWERPC__)
		
					for (p = src, q = dst, n++; --n;)
						*q++ = *p++;
	
				#else
		
					for (p = (const char *) src - 1, q = (char *) dst - 1, n++; --n;)
						*++q = *++p;
	
				#endif
		
			}
			else
			{
				for (p = (const char *) src + n, q = (char *) dst + n, n++; --n;)
					*--q = *--p;
			}

		#endif /* __dest_os == __mac_os && !_No_BlockMove */
	
		return(dst);
	}

	#endif /* !(__MC68K__ && !defined(_No_BlockMove)) */

	#if !defined(__PPC_EABI__)
	
		void * memset(void * dst, int val, size_t n)
		{
			__fill_mem(dst, val, n);
	
			return(dst);
		}
		
	#endif

	#ifndef UNDER_CE
	
		void * memchr(const void * src, int val, size_t n)
		{
			const unsigned char *	p;
	
			#if !defined(__POWERPC__)
				unsigned long	v = (val & 0xff);                /*- hh 980624 -*/
	
				for (p = (unsigned char *) src, n++; --n;)
					if (*p++ == v)
						return((void *) (p-1));
	
			#else
	
				unsigned long	v = (val & 0xff);                /*- mm 980425 -*/
	
				for (p = (unsigned char *) src - 1, n++; --n;)
					if ((*++p & 0xff) == v)						/*- mm 980425 -*/
						return((void *) p);

			#endif
	
			return(NULL);
		}
	#endif

	void * __memrchr(const void * src, int val, size_t n)
	{
		const unsigned char *	p;
	
		#if !defined(__POWERPC__)

			unsigned char	v = (val & 0xff);                /*- hh 980624 -*/
	
			for (p = (unsigned char *) src + n, n++; --n;)
				if (*--p == v)
					return((void *) p);
	
		#else
	
			unsigned long	v = (val & 0xff);                /*- hh 980624 -*/
	
			for (p = (unsigned char *) src + n, n++; --n;)
				if (*--p == v)
					return((void *) p);

		#endif
	
		return(NULL);
	}

	int memcmp(const void * src1, const void * src2, size_t n)
	{
		const	unsigned char * p1;
		const	unsigned char * p2;

		#if !defined(__POWERPC__)
	
			for (p1 = (const unsigned char *) src1, p2 = (const unsigned char *) src2, n++; --n;)
				if (*p1++ != *p2++)
					return((*--p1 < *--p2) ? -1 : +1);
	
		#else
	
			for (p1 = (const unsigned char *) src1 - 1, p2 = (const unsigned char *) src2 - 1, n++; --n;)
				if (*++p1 != *++p2)
					return((*p1 < *p2) ? -1 : +1);

		#endif
	
		return(0);
	}

#endif /* __INTEL__ */

errno_t (memcpy_s)(register void * _MSL_RESTRICT dst, register rsize_t dst_max, register const void * _MSL_RESTRICT src, register rsize_t n)
{
	if (((dst == NULL) || (src == NULL)) || ((dst_max > RSIZE_MAX) || (n > RSIZE_MAX)))
	{
		__msl_undefined_behavior_s();
		return ERANGE;
	}
	
	if (dst_max < n)
	{
		__msl_undefined_behavior_s();
		memset(dst, 0, dst_max);
		return ERANGE;
	}
	
	(memcpy)(dst, src, n);
	return ENOERR;
}

errno_t (memmove_s)(void * dst, rsize_t dst_max, const void * src, rsize_t n)
{
	if (((dst == NULL) || (src == NULL)) || ((dst_max > RSIZE_MAX) || (n > RSIZE_MAX)))
	{
		__msl_undefined_behavior_s();
		return ERANGE;
	}
	
	if (dst_max < n)
	{
		__msl_undefined_behavior_s();
		memset(dst, 0, dst_max);
		return ERANGE;
	}
	
	(memmove)(dst, src, n);
	return ENOERR;
}


/* Change record:
 * JFH 950524 First code release.
 * JFH 951115 Sped up memset by having it use a routine which writes longs and,
 *			  when possible, does so in an unrolled loop.
 * JFH 951121 Sped up memcpy and memmove by having them use routines that copy longs
 *			  and, when possible, does so in an unrolled loop.
 * JFH 951230 Added explicit #include of <Memory.h>
 * JFH 960109 Added __memrchr (analogous to strrchr). Used by fwrite for line buffering.
 * JFH 960119 Bracketed #include of <Memory.h> by test for __mac_os
 * JFH 960122 Bracketed memcpy() and memmove() by #if <condition too complex to put here>
 *			  (they are inlined in <string.h> if this condition fails).
 * JFH 960301 Merged Be code into source. Most of that was actually shuffled off to an
 *			  #include file.
 * JFH 960429 Merged Win32 changes in. As above, most of that was actually shuffled off
 *			  CTV to an #include file.
 * mm  961227 Prevented duplicate declaration of memmove under c++/68k
 * SCM 970710 Added support for __PPC_EABI__.
 * bb  970718 Took out lines that prevented memcpy and memmove from being seen
 *            in standard library test.
 * mm  980425 Change to make memchr work for characters of value >127 on PPC.  MW02625 
 * mf  980429 changed macro __dest_os_== win32_os to __INTEL__ so all intel oses pick
 *            up optimized string
 *            functions (eg. wince and beos)           
 * hh  980624 Change to make memchr work for characters of value >127 on PPC.  MW02625 
 * mf  990120 changed intel header file name from mem.win32.h to mem.x86.h
 * BLC 990129 Optimized the PPC & MacOS memcpy and memmove functions
 * vss 990421 Update to Universal Headers 3.2
 * as  990808 modified memcpy for Hitachi SH
 * as  001117 modified memcpy for Motorola MCORE
 * cc  000326 removed dest_os to be_os 
 * mm  010406 Two more be_os removed
 * JWW 020917 Added _MSL_RESTRICT to get the restrict type specifier for certain C99 functions
 * JWW 031030 Added the __STDC_WANT_SECURE_LIB__ secure library extensions
 * JWW 040317 Updated the __STDC_WANT_SECURE_LIB__ secure library based on new secure specifications
 * JWW 040831 Obtain fast Mach-O versions of memcpy, memmove, etc. from the System.framework
 * cc  041110 Added _MSL_LONG_COPY_MIN macro
 */