/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:34:02 $
 * $Revision: 1.87.2.3 $
 */

/*	Routines
 *	--------
 *		fscanf
 *		fscanf_s
 *		scanf
 *		scanf_s
 *		sscanf
 *		sscanf_s
 *		vfscanf
 *		vfscanf_s
 *		vsscanf
 *		vsscanf_s
 */

#define __STDC_WANT_SECURE_LIB__ 1
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <strtold.h>
#include <strtoul.h>
#include <critical_regions.h>
#include <msl_secure_error.h>

#if !_MSL_FLOATING_POINT && _MSL_FLOATING_POINT_IO
	#error Defining _MSL_FLOATING_POINT_IO cannot be on when _MSL_FLOATING_POINT is off!
#endif

#if _MSL_FLOATING_POINT_IO
	#include <ansi_fp.h>
	#include <float.h>
#endif /* _MSL_FLOATING_POINT_IO */

#define conversion_buff_size	512
#define conversion_max			509
#define bad_conversion			0xFF

enum argument_options
{
	normal_argument,
	char_argument,									/*- blc 980317 -*/
	short_argument,
	long_argument,
#if _MSL_C99										/*- mm 030304 -*/
	intmax_argument,								/*- mm 020529 -*/
	size_t_argument,								/*- mm 020529 -*/
	ptrdiff_argument,								/*- mm 020529 -*/
#endif /* _MSL_C99 */								/*- mm 030304 -*/
	long_long_argument,								/*- mm 961219 -*/
	double_argument,
	long_double_argument,
	wchar_argument									/*- mm 990407 -*/
};

typedef unsigned char char_map[32];

typedef struct
{
	unsigned char	suppress_assignment;
	unsigned char 	field_width_specified;
	unsigned char 	argument_options;
	unsigned char	conversion_char;
	int				field_width;
#ifdef __ALTIVEC__
	int	 vec;							/* doing vector conversion						*/
	int  vecCount;						/* number of vec fields to convert (4, 8, 16) 	*/
	char *vecSeperator;					/* separator string to insert between vec fields*/
	char *c_vecSeperator;				/* separator string to use for %c vec conversion*/
	int	 vecSepLen;						/* length of *vecSeperator or *c_vecSeperator 	*/
#endif /* __ALTIVEC__ */
	char_map		char_set;
} scan_format;

#ifdef __ALTIVEC__
#ifndef __VEC__
	#define vec_va_arg(ap)  (*(((MWVector128 *) (ap = (char *) ((((unsigned long) ap + 15) & ~(15)) + 16))) -1))
	#define va_arg_replacement2(ap, vec_val) (*va_arg(ap, MWVector128 *) = vec_val)
#endif /* __VEC__ */

union MWVector128 {						/* used to get at the vec field values 				*/
	#ifdef __VEC__
	__vector unsigned char			vuc;
	__vector signed char			vsc;
	__vector bool char				vbc;
	__vector unsigned short			vus;
	__vector signed short			vss;
	__vector unsigned long			vul;
	__vector signed long			vsl;
	__vector float					vf;
	__vector pixel					vp;
	#endif /* __VEC__ */
	unsigned char 		 			u8[16];
	signed   char 		 			s8[16];
	unsigned short		 			u16[8];
	signed   short		 			s16[8];
	unsigned long 		 			u32[4];
	signed   long 		 			s32[4];
	float		   		 			f32[4];
};
typedef union MWVector128 MWVector128;
#endif /* __ALTIVEC__ */

#define set_char_map(map, ch)  map[(unsigned char)ch>>3] |= (1 << (ch&7))			/*- mm 990716 -*/
#define tst_char_map(map, ch) (map[(unsigned char)ch>>3] &  (1 << (ch&7)))			/*- mm 990716 -*/

#ifdef __ALTIVEC__
static const char * parse_format(const char * format_string, va_list * arg, scan_format * format)
#else
static const char * parse_format(const char * format_string, scan_format * format)
#endif /* __ALTIVEC__ */
{
	const char *	s = format_string;
	int				c;
	int				flag_found, invert;
#ifdef __ALTIVEC__
	int				vec_sep_ok = 1;
	scan_format		f = {0, 0, normal_argument, 0, INT_MAX, 0, 0, "", "", 0, {0}};
#else
	scan_format		f = {0, 0, normal_argument, 0, INT_MAX, {0}};
#endif /* __ALTIVEC__ */
	
	if ((c = *++s) == '%')
	{
		f.conversion_char = c;
		*format = f;
		return((const char *) s + 1);
	}
	
	if (c == '*')
	{
		f.suppress_assignment = 1;
		
		c = *++s;
	}
	
#ifdef __ALTIVEC__
	GetWidth:			 			/* goto here when we see we had a vec sep char flag */
#endif /* __ALTIVEC__ */
	
	if (isdigit(c))
	{
		f.field_width = 0;
		
		do
		{
			f.field_width = (f.field_width * 10) + (c - '0');
			c = *++s;
		}
		while (isdigit(c));
		
		if (f.field_width == 0)
		{
			f.conversion_char = bad_conversion;
			*format = f;						/*- mm 981104 -*/
			return((const char *) s + 1);
		}
		
		f.field_width_specified = 1;
	}
	
#ifdef __ALTIVEC__
	if (c == 'v')
	{
		f.vec = 1;
		c = *++s;
	}
#endif /* __ALTIVEC__ */

	flag_found = 1;
	
	switch (c)
	{
		case 'h':
			f.argument_options = short_argument;
			
			if (s [1] == 'h')
			{
				f.argument_options = char_argument;
				c = *++s; /* move past second h */
			}
#ifdef __ALTIVEC__
			else
				f.vecCount = 8;
#endif /* __ALTIVEC__ */
			
			break;
		
		case 'l':
			
			f.argument_options = long_argument;

#if _MSL_LONGLONG
			if (s [1] == 'l')
			{
				f.argument_options = long_long_argument;
				c = *++s; /* move past second l */
			}
#endif /* _MSL_LONGLONG */
#ifdef __ALTIVEC__
#if _MSL_LONGLONG
			else
#endif /* _MSL_LONGLONG */
				f.vecCount = 4;
#endif /* __ALTIVEC__ */
			
			break;
		case 'L':
			
			f.argument_options = long_double_argument;
			
			break;
#if _MSL_C99					/* mm 030304 -*/

/* begin mm 020529 insert */

		case 'j':
		
			f.argument_options = intmax_argument;
			break;
			
		case 'z':
		
			f.argument_options = size_t_argument;
			break;
			
		case 't':
		
			f.argument_options = ptrdiff_argument;
			break;
#endif /* _MSL_C99 */			/* mm 030304 -*/
			
/* end mm 020529 insert */

		default:
			
			flag_found = 0;
#ifdef __ALTIVEC__
			f.vecCount = 16;
#endif /* __ALTIVEC__ */
	}
	
	if (flag_found)
		c = *++s;
	
#ifdef __ALTIVEC__
	if (!f.vec && c == 'v' && f.vecCount != 0) 
	{
		f.vec = 1;
		c = *++s;
	} 
#endif /* __ALTIVEC__ */
	
	f.conversion_char = c;
	
	switch (c)
	{
		case 'd':
		case 'i':
		case 'u':
		case 'o':
		case 'x':
		case 'X':
			
			if (f.argument_options == long_double_argument)
			{
				f.conversion_char = bad_conversion;
				break;
			}

			break;
		
        case 'a':		/*- BLC 980317 -*/
        case 'A':		/*- mm  021217 -*/
		case 'f':
		case 'F':		/*- mm  021217 -*/
		case 'e':
		case 'E':
		case 'g':
		case 'G':
			
			if (f.argument_options == char_argument 
				|| f.argument_options == short_argument 
#if _MSL_C99 												/*- mm 030304 -*/
				|| f.argument_options == intmax_argument	/*- mm 020529 -*/
				|| f.argument_options == size_t_argument || f.argument_options == ptrdiff_argument 	/*- mm 020529 -*/
#endif /* _MSL_C99 */										/*- mm 030304 -*/
#if _MSL_LONGLONG
				|| f.argument_options == long_long_argument
#endif /* _MSL_LONGLONG */
			)
			{
				f.conversion_char = bad_conversion;
				break;
			}
			
			/* 'l' before float identifier means double */

			if (f.argument_options == long_argument)
				f.argument_options = double_argument;

			break;

		case 'p':
		
#ifdef __ALTIVEC__
			if (!f.vec)
#endif /* __ALTIVEC__ */

/* dsp56800 uses 16-bit pointers*/
#if __dest_os == __m56800_os || \
    __dest_os == __m56800E_os

	#ifdef __m56800E_lmm__
    	f.argument_options = long_argument;
	#else
		f.argument_options = short_argument;
	#endif 

#else
    f.argument_options = long_argument;
#endif


    f.conversion_char  = 'x';
			
    break;
			
		case 'c':
#ifdef __ALTIVEC__
			if (!f.vec)
#endif /* __ALTIVEC__ */
			if (f.argument_options == long_argument)			/*- mm 990407 -*/
				f.argument_options = wchar_argument;			/*- mm 990407 -*/
			else												/*- mm 990407 -*/
				if (f.argument_options != normal_argument)
					f.conversion_char = bad_conversion;
			
			break;
			
		case 's':
			
			if (f.argument_options == long_argument)			/*- mm 990407 -*/
				f.argument_options = wchar_argument;			/*- mm 990407 -*/
			else												/*- mm 990407 -*/
				if (f.argument_options != normal_argument)
					f.conversion_char = bad_conversion;
			
			{
				int				i;
				unsigned char *	p;
				
				for (i = sizeof(f.char_set), p = f.char_set; i; --i)
					*p++ = 0xFF;
				
				f.char_set[1] = 0xC1;	/* sets bits for HT, LF, VT, FF, and CR chars to zero */
				f.char_set[4] = 0xFE;	/* set bit for Sp char to zero */
			}
			
			break;
			
		case 'n':
			
			break;
		
		case '[':
			if (f.argument_options == long_argument)			/*- mm 990407 -*/
				f.argument_options = wchar_argument;			/*- mm 990407 -*/
			else												/*- mm 990407 -*/
				if (f.argument_options != normal_argument)
					f.conversion_char = bad_conversion;
			
			c = *++s;
			
			invert = 0;
			
			if (c == '^')
			{
				invert = 1;
				c = *++s;
			}
			
			if (c == ']')
			{
				set_char_map(f.char_set, ']');
				c = *++s;
			}
			
			while (c && c != ']')
			{
				int d;
				
				set_char_map(f.char_set, c);
				
				if (*(s+1) == '-' && (d = *(s+2)) != 0 && d != ']')
				{
					while (++c <= d)
						set_char_map(f.char_set, c);
					
					c = *(s += 3);
				}
				else
					c = *++s;
			}
			
			if (!c)
			{
				f.conversion_char = bad_conversion;
				break;
			}
			
			if (invert)
			{
				int							i;
				unsigned char *	p;
				
				for (i = sizeof(f.char_set), p = f.char_set; i; --i, ++p)
					*p = ~*p;
			}
			
			break;
		
		default:
#ifdef __ALTIVEC__
			if (!f.vec && vec_sep_ok)
			{	
				vec_sep_ok = 0;
				if (c == '@') {
					f.vecSeperator = va_arg(*arg, char *);
					f.vecSepLen    = strlen(f.vecSeperator);
					while (f.vecSepLen > 0 && *f.vecSeperator == ' ') {
						++f.vecSeperator;
						--f.vecSepLen;
					}
					while (f.vecSepLen > 0 && f.vecSeperator[f.vecSepLen-1] == ' ')
						--f.vecSepLen;
				} else {
					f.vecSeperator = (char *)s;
					f.vecSepLen    = 1;
				}
				f.c_vecSeperator = f.vecSeperator;
				c = *++s;
				if (!f.suppress_assignment && c == '*')
				{
					f.suppress_assignment = 1;
					c = *++s;
				}
				goto GetWidth;
			}
#endif /* __ALTIVEC__ */
			f.conversion_char = bad_conversion;
			
			break;
	}
	
	*format = f;
	
	#if 0 /* MSL debugging code */
	#if _MSL_OS_DISK_FILE_SUPPORT
	fprintf(stderr, "scan_format:\n"
					"  suppress_assignment   = %d\n"
					"  field_width_specified = %d\n"
					"  argument_options      = %d\n"
					"  conversion_char       = %c\n"
					"  field_width           = %ld\n"
					"  vec                   = %ld\n"
					"  vecCount              = %ld\n"
					"  vecSeperator          = %s\n"
					"  vecSepLen             = %ld\n",
					f.suppress_assignment,
					f.field_width_specified,
					f.argument_options,
					f.conversion_char,
					f.field_width,
					f.vec,
					f.vecCount,
					f.vecSeperator == 0 ? "<null>" : f.vecSeperator,
					f.vecSepLen);
	#endif /* _MSL_OS_DISK_FILE_SUPPORT */
	#endif /* 0 */
	
	return((const char *) s + 1);
}


static int __sformatter(int (_MSL_CDECL *ReadProc)(void *, int, int), void * ReadProcArg, 
														const char * format_str, va_list arg, int is_secure)  /*- mm 990325 -*/
{
	int					num_chars, chars_read, items_assigned, conversions;
	int					base, negative, overflow;
	int 				rval;					/*- mm 011207 -*/
	const char *		format_ptr;
	char				format_char;         	/*- mm 961002 -*/
	char             	c;          			/*- mm 961002 -*//*- mm 990409 -*/
	scan_format			format;
	long				long_num;
	unsigned long		u_long_num;
#if _MSL_LONGLONG					/*- mm 961219 -*/
	long long   		long_long_num = 0;			/*- mm 961219 -*/
	unsigned long long 	u_long_long_num;		/*- mm 961219 -*/
#endif /* _MSL_LONGLONG */						/*- mm 961219 -*/
#if _MSL_FLOATING_POINT && _MSL_FLOATING_POINT_IO
	long double			long_double_num;
#endif /* _MSL_FLOATING_POINT */
	char *				arg_ptr;
	int					elem_valid;
	size_t				elem_maxsize;
	int match_failure = 0;
	int terminate  = 0;			/*- mm 990608 -*/
	
#ifdef __ALTIVEC__
	int vecIndex;						/* counts vector elements */
	MWVector128 vec_val;

	#define GOTO_GET_REMAINING_VEC_FIELDS(lbl) 										\
		{																			\
			if (!match_failure)														\
				if ((c = (*ReadProc)(ReadProcArg, 0, __GetAChar)) != EOF && ++vecIndex < format.vecCount) {		\
					int sepSeen = (format.vecSepLen == 0);							\
					if (!sepSeen) {													\
						int i = 0;													\
						while (isspace(c)) {										\
							c = (*ReadProc)(ReadProcArg, 0, 1);						\
							++chars_read;											\
						}															\
						while (c != EOF) {											\
							sepSeen = (c == format.vecSeperator[i]);				\
							if (!sepSeen)											\
								break;												\
							c = (*ReadProc)(ReadProcArg, 0, __GetAChar);			\
							++chars_read;											\
							if (++i >= format.vecSepLen)							\
								break;												\
						}															\
					}																\
					if (sepSeen && c != EOF) {										\
						while (isspace(c)) {										\
							c = (*ReadProc)(ReadProcArg, 0, __GetAChar);			\
							++chars_read;											\
						}															\
						(*ReadProc)(ReadProcArg, c, __UngetAChar);					\
						goto lbl;													\
					}																\
				} else																\
					(*ReadProc)(ReadProcArg, c, __UngetAChar);						\
		}
#endif /* __ALTIVEC__ */

	format_ptr     = format_str;
	chars_read     = 0;
	items_assigned = 0;
	conversions    = 0;
	
	while (!terminate && (format_char = *format_ptr) != 0) /*- mm 990311 -*/	/*- mm 990608 -*/
	{
		if (isspace(format_char))
		{
			do
				format_char = *++format_ptr;
			while (isspace(format_char));
			
			if (!match_failure)
			{
				while (isspace(c = (*ReadProc)(ReadProcArg, 0, __GetAChar)))			/*- mm 990325 -*/
					++chars_read;
				
				(*ReadProc)(ReadProcArg, c, __UngetAChar);								/*- mm 990325 -*/
			}
			continue;
		}
		
		if ((format_char != '%') && (!match_failure))
		{
			if ((c = (*ReadProc)(ReadProcArg, 0, __GetAChar)) != (unsigned char)format_char)      /*- mm 980331 -*//*- mm 990325 -*/
			{
				(*ReadProc)(ReadProcArg, c, __UngetAChar);							/*- mm 990325 -*/
				if (!is_secure)
					goto exit;
				else
				{
					match_failure = 1;
					++format_ptr;
					continue;
				}
			}
			
			++chars_read;
			++format_ptr;
			
			continue;
		}
#ifndef __ALTIVEC__
		format_ptr = parse_format(format_ptr, &format);
#else
		format_ptr = parse_format(format_ptr, (va_list *)&arg, &format);
		
		if (format.vec)
		{
			vecIndex 	 	= 0;
			vec_val.u32[0]	= 0;
			vec_val.u32[1]	= 0;
			vec_val.u32[2]	= 0;
			vec_val.u32[3]	= 0;
			
			/* Leading input blanks skipped only in vector cases */
			if (!match_failure)
			{
				while (isspace(c = (*ReadProc)(ReadProcArg, 0, __GetAChar)))			/*- mm 990325 -*/
					++chars_read;
				(*ReadProc)(ReadProcArg, c, __UngetAChar);								/*- mm 990325 -*/
			}
		} else
#endif /* __ALTIVEC__ */

#if !defined(__m56800E__)
																					/*- mm 990608 -*/
		if (!format.suppress_assignment && format.conversion_char != '%')
		{
			arg_ptr = va_arg(arg, char *);
		}	
		else
#endif /* __m56800E__ */
        {
			arg_ptr = 0;
		}
			
		if ((format.conversion_char != 'n') && (!match_failure) && (*ReadProc)(ReadProcArg, 0, __TestForError))	/*- mm 990608 -*/
		{																					/*- mm 990608 -*/
			if (!is_secure)
				goto exit;																		/*- mm 990608 -*/
			else
				match_failure = 1;
		}	/*- vss 990909 -*/
		
		switch (format.conversion_char)
		{
			case 'd':
				
				base = 10;
				
				goto signed_int;
				
			case 'i':
				
				base = 0;
				
			signed_int:
				
				if (match_failure)
				{
					long_num = 0;
#if _MSL_LONGLONG && _MSL_C99
					long_long_num = 0;
#endif /* _MSL_LONGLONG && _MSL_C99 */
				}
				else
				{
#if _MSL_LONGLONG && _MSL_C99                                   	/*- mm 961219 -*/	/*- mm 030317 -*/
	                if ((format.argument_options == long_long_argument) || /*- mm 961219 -*/ /*- mm 040811 -*/
	                		(format.argument_options == intmax_argument))	/*- mm 040811 -*/
	                   u_long_long_num =                               /*- mm 961219 -*/
	                      __strtoull(base, format.field_width, ReadProc, ReadProcArg, &num_chars, &negative, &overflow);
	                else                                               /*- mm 961219 -*/
#endif /* _MSL_LONGLONG && _MSL_C99 */                            /*- mm 961219 -*/	/*- mm 030317 -*/
					u_long_num
						= __strtoul(base, format.field_width, ReadProc, ReadProcArg, &num_chars, &negative, &overflow);
					
					if (!num_chars)
					{
						if (!is_secure)
							goto exit;
						else
						{
							match_failure = 1;
							long_num = 0;
#if _MSL_LONGLONG && _MSL_C99
							long_long_num = 0;
#endif /* _MSL_LONGLONG && _MSL_C99 */
							goto signed_int_assign;
						}
					}
					
					chars_read += num_chars;
					
#if _MSL_LONGLONG && _MSL_C99                                    /*- mm 961219 -*/
	                if ((format.argument_options == long_long_argument) || /*- mm 961219 -*/ /*- mm 040811 -*/
	                		(format.argument_options == intmax_argument))	/*- mm 040811 -*/
	                   long_long_num = (negative ? -u_long_long_num : u_long_long_num); /*- mm 961219 -*/
	                else                                               /*- mm 961219 -*/
#endif /* _MSL_LONGLONG */                                         /*- mm 961219 -*/
					long_num = (negative ? -u_long_num : u_long_num);
				}
				
			signed_int_assign:
#ifdef __ALTIVEC__
				if (format.vec)
				{
					if (!format.suppress_assignment)
					{
						switch (format.argument_options)
						{
							case normal_argument: vec_val.s8 [vecIndex] = long_num; break;
							case short_argument:  vec_val.s16[vecIndex] = long_num; break;
							case long_argument:   vec_val.s32[vecIndex] = long_num; break;
						}
						GOTO_GET_REMAINING_VEC_FIELDS(signed_int);
						
						#ifdef __VEC__
						*va_arg(arg, MWVector128 *) = vec_val;
						#else
						va_arg_replacement2(arg, vec_val);
						#endif /* __VEC__ */
						if (!match_failure)
							++items_assigned;
					}
					else
						GOTO_GET_REMAINING_VEC_FIELDS(signed_int);
				}
				else
#endif /* __ALTIVEC__ */

#if defined(__m56800E__)
			{
				int * arg_iptr = 0;
				if (format.argument_options == char_argument)
					arg_ptr = va_arg(arg, char *);
				else
					arg_iptr = va_arg(arg, int *);

				switch (format.argument_options)
				{
					case normal_argument:
						if (arg_iptr) 	(*((int *) arg_iptr)) = long_num;
						break;
					case short_argument:
						if (arg_iptr) 	(*((short *) arg_iptr)) = long_num;
						break;
					case long_argument:
						if (arg_iptr) 	(*((long *) arg_iptr)) = long_num;
						break;
					case char_argument:
						if (arg_ptr) 	(*((signed char *) arg_ptr)) = long_num;
						break;
#if _MSL_LONGLONG                                    /*- mm 961219 -*/
					case long_long_argument:
						if (arg_iptr) 	(*((long long *) arg_iptr)) = long_long_num;
						break;
#endif /* _MSL_LONGLONG */                                          /*- mm 961219 -*/
				}
				if (!match_failure)
					++items_assigned;
			}
#else /* !__m56800E__ */

				if (arg_ptr)
				{
					switch (format.argument_options)
					{
						case normal_argument:  * (int *)         arg_ptr = long_num; break;
						case char_argument:    * (signed char *) arg_ptr = long_num; break;
						case short_argument:   * (short *)       arg_ptr = long_num; break;
						case long_argument:    * (long *)        arg_ptr = long_num; break;
#if _MSL_C99																				/*- mm 030304 -*/
#if _MSL_LONGLONG
						case intmax_argument:  * (intmax_t *)    arg_ptr = long_long_num; break;	/*- mm 020529 -*/ /*- mm 040811 -*/
#else
						case intmax_argument:  * (intmax_t *)    arg_ptr = long_num; break;	/*- mm 020529 -*/
#endif /* _MSL_LONGLONG */
						case size_t_argument:  * (size_t *) 	 arg_ptr = long_num; break;	/*- mm 020529 -*/
						case ptrdiff_argument: * (ptrdiff_t *) 	 arg_ptr = long_num; break;	/*- mm 020529 -*/
#endif /* _MSL_C99 */																		/*- mm 030304 -*/
#if _MSL_LONGLONG                                    /*- mm 961219 -*/
                        case long_long_argument: * (long long *) arg_ptr = long_long_num; break;
#endif /* _MSL_LONGLONG */                                             /*- mm 961219 -*/
					}
					if (!match_failure)
						++items_assigned;
				}
#endif /* __m56800E__ */
				++conversions;
				
				break;
				
			case 'o':
				
				base = 8;
				
				goto unsigned_int;
				
			case 'u':
				
				base = 10;
				
				goto unsigned_int;
				
			case 'x':
			case 'X':
				
				base = 16;
				
			unsigned_int:
				
				if (match_failure)
				{
					u_long_num = 0;
#if _MSL_LONGLONG && _MSL_C99
					u_long_long_num = 0;
#endif /* _MSL_LONGLONG && _MSL_C99 */
				}
				else
				{
#if _MSL_LONGLONG && _MSL_C99                                    /*- mm 961219 -*/	/*- mm 030317 -*/
	                if ((format.argument_options == long_long_argument) || /*- mm 961219 -*/  /*- mm 040811 -*/
	                		(format.argument_options == intmax_argument))	/*- mm 040811 -*/
	                   u_long_long_num =                               /*- mm 961219 -*/
	                      __strtoull(base, format.field_width, ReadProc, ReadProcArg, &num_chars, &negative, &overflow);
	                else                                               /*- mm 961219 -*/
#endif /* _MSL_LONGLONG && _MSL_C99 */                            /*- mm 961219 -*/	/*- mm 030317 -*/
					u_long_num
						= __strtoul(base, format.field_width, ReadProc, ReadProcArg, &num_chars, &negative, &overflow);
					
					if (!num_chars)
					{
						if (!is_secure)
							goto exit;
						else
						{
							match_failure = 1;
							u_long_num = 0;
#if _MSL_LONGLONG && _MSL_C99
							u_long_long_num = 0;
#endif /* _MSL_LONGLONG && _MSL_C99 */
							goto unsigned_int_assign;
						}
					}
					
					chars_read += num_chars;
					
					if (negative)
#if _MSL_LONGLONG                                    /*- mm 961219 -*/
	                if (format.argument_options == long_long_argument) /*- mm 961219 -*/
						u_long_long_num = -u_long_long_num;            /*- mm 961219 -*/
	                else                                               /*- mm 961219 -*/
#endif /* _MSL_LONGLONG */                                         /*- mm 961219 -*/
						u_long_num = -u_long_num;
				}
				
			unsigned_int_assign:
#ifdef __ALTIVEC__
				if (format.vec)
				{
					if (!format.suppress_assignment)
					{
						switch (format.argument_options)
						{
							case normal_argument: vec_val.u8 [vecIndex] = u_long_num; break;
							case short_argument:  vec_val.u16[vecIndex] = u_long_num; break;
							case long_argument:   vec_val.u32[vecIndex] = u_long_num; break;
						}
						GOTO_GET_REMAINING_VEC_FIELDS(unsigned_int);
						
						#ifdef __VEC__
						*va_arg(arg, MWVector128 *) = vec_val;
						#else
						va_arg_replacement2(arg, vec_val);
						#endif /* __VEC__ */
						if (!match_failure)
							++items_assigned;
					}
					else
						GOTO_GET_REMAINING_VEC_FIELDS(unsigned_int);
				}
				else
#endif /* __ALTIVEC__ */
#if defined(__m56800E__)
			{
				int * arg_iptr = 0;
				if (format.argument_options == char_argument)
					arg_ptr = va_arg(arg,char *);
				else
					arg_iptr = va_arg(arg,int *);

				switch (format.argument_options)
				{
					case normal_argument:
						if (arg_iptr) 	(*((unsigned int *) arg_iptr)) = u_long_num;
						break;
					case short_argument:
						if (arg_iptr) 	(*((unsigned short *) arg_iptr)) = u_long_num;
						break;
					case long_argument:
						if (arg_iptr) 	(*((unsigned long *) arg_iptr)) = u_long_num;
						break;
					case char_argument:
						if (arg_ptr) 	(*((unsigned char *) arg_ptr)) = u_long_num;
						break;
#if _MSL_LONGLONG                                    /*- mm 961219 -*/
					case long_long_argument:
						if (arg_iptr) 	(*((unsigned long long *) arg_iptr)) = u_long_long_num;
						break;
#endif /* _MSL_LONGLONG */                                        /*- mm 961219 -*/
				}
				if (!match_failure)
					++items_assigned;
			}
#else /* !__m56800E__ */
				if (arg_ptr)
				{
					switch (format.argument_options)
					{
						case normal_argument:  * (unsigned int *)   	arg_ptr = u_long_num; break;
						case char_argument:    * (unsigned char *)  	arg_ptr = u_long_num; break;
						case short_argument:   * (unsigned short *) 	arg_ptr = u_long_num; break;
						case long_argument:    * (unsigned long *)  	arg_ptr = u_long_num; break;
#if _MSL_C99							/* mm 030304 -*/
#if _MSL_LONGLONG
						case intmax_argument:  * (intmax_t *)    	arg_ptr = u_long_long_num; break;	/*- mm 020529 -*/ /*- mm 040811 -*/
#else
						case intmax_argument:  * (intmax_t *)    	arg_ptr = u_long_num; break;	/*- mm 020529 -*/
#endif /* _MSL_LONGLONG */
						case size_t_argument:  * (size_t *) 	 	arg_ptr = u_long_num; break;	/*- mm 020529 -*/
						case ptrdiff_argument: * (ptrdiff_t *) 	 	arg_ptr = u_long_num; break;	/*- mm 020529 -*/
#if _MSL_LONGLONG                                    /*- mm 961219 -*/
                        case long_long_argument: * (unsigned long long *) arg_ptr = u_long_long_num; break;
#endif /* _MSL_LONGLONG */             					                                            /*- mm 961219 -*/
#endif /* _MSL_C99 */  								/* mm 030304 -*/
					}
					
					if (!match_failure)
						++items_assigned;
				}
#endif /* __m56800E__ */
				
				++conversions;
				
				break;

#if _MSL_FLOATING_POINT
#if _MSL_FLOATING_POINT_IO
			case 'a':		/*- BLC 980317 -*/
			case 'A':		/*- mm  021217 -*/			
			case 'f':
			case 'F':		/*- mm  021217 -*/
			case 'e':
			case 'E':
			case 'g':
			case 'G':
			flt:
				if (match_failure)
					long_double_num = NAN;
				else
				{
					long_double_num = __strtold(format.field_width, ReadProc, ReadProcArg, &num_chars, &overflow);
					
					if (!num_chars)
					{
						if (!is_secure)
							goto exit;
						else
						{
							match_failure = 1;
							long_double_num = NAN;
							goto assign_float;
						}
					}
					
					chars_read += num_chars;
				}
				
			assign_float:
#ifdef __ALTIVEC__
				if (format.vec)
				{
					if (!format.suppress_assignment)
					{
						vec_val.f32[vecIndex] = long_double_num;
						
						if (format.argument_options == normal_argument)
							format.vecCount = 4;
						GOTO_GET_REMAINING_VEC_FIELDS(flt);

						#ifdef __VEC__
						*va_arg(arg, MWVector128 *) = vec_val;
						#else
						va_arg_replacement2(arg, vec_val);
						#endif /* __VEC__ */
						if (!match_failure)
							++items_assigned;
					}
					else
						GOTO_GET_REMAINING_VEC_FIELDS(flt);
				}
				else
#endif /* __ALTIVEC__ */

#if defined(__m56800E__)
			{
				float * arg_fptr;
#define	arg_ptr	arg_fptr
				arg_fptr = va_arg(arg, float *);
#endif /* __m56800E__ */
				if (arg_ptr)
				{
					switch (format.argument_options)
					{
						case normal_argument:      * (float *)       arg_ptr = long_double_num; break;
						case double_argument:      * (double *)      arg_ptr = long_double_num; break;
						case long_double_argument: * (long double *) arg_ptr = long_double_num; break;
					}
					
					if (!match_failure)
						++items_assigned;
				}
#if defined(__m56800E__)
#undef	arg_ptr
			}
#endif /* __m56800E__ */
				
				++conversions;

				break;

#endif /* _MSL_FLOATING_POINT */
#endif /*_MSL_FLOATING_POINT_IO */
			case 'c':
				
				if (!format.field_width_specified)
					format.field_width = 1;
#ifdef __ALTIVEC__
				if (format.vec)
				{
					if (!format.suppress_assignment)
					{
						char *ptr;
						int width;
						
						if (vecIndex == 0) {
							format.vecSeperator = format.c_vecSeperator;
							format.vecSepLen    = *format.c_vecSeperator ? 1 : 0;
						}
					str1:
						switch (format.argument_options)
						{
							case long_argument:
								if (format.field_width > 4)
									goto exit;
								ptr = (char *)&vec_val.u8[4*vecIndex + 4 - format.field_width];
								break;
							case short_argument:
								if (format.field_width > 2)
									goto exit;
								ptr = (char *)&vec_val.u8[2*vecIndex + 2 - format.field_width];
								break;
							case normal_argument:
								if (format.field_width > 1)
									goto exit;
								ptr = (char *)&vec_val.u8[vecIndex];
								break;
						}
						
						num_chars = 0;
						width = format.field_width;
						while (width-- && (c = (*ReadProc)(ReadProcArg, 0, __GetAChar)) != EOF)	/*- mm 990325 -*/
						{
							/*if  (c == 0) 														/*- mm 971202 -*/
							/*	return(EOF);                                         			/*- mm 971202 -*/
							*ptr++ = c;
							++num_chars;
						}
						
						if (!num_chars)
							goto exit;
						
						chars_read += num_chars;
						GOTO_GET_REMAINING_VEC_FIELDS(str1);
						#if __VEC__
						*va_arg(arg, MWVector128 *) = vec_val;
						#else
						va_arg_replacement2(arg, vec_val);
						#endif
					
						++items_assigned;
					}
					else
					{
						int width;
					str2:
						num_chars = 0;
						width = format.field_width;
						while (format.field_width-- && 
									((rval = ((*ReadProc)(ReadProcArg, 0, __GetAChar))) != EOF))	/*- mm 990325 -*/	/*- mm 011207 -*/
						{
							c = rval;
							++num_chars;
						}
						c = rval;																/*- mm 020103 -*/
						
						if (!num_chars)
							goto exit;
						
						GOTO_GET_REMAINING_VEC_FIELDS(str2);
					}
				}
				else
#endif /* __ALTIVEC__ */
#if defined(__m56800E__)
                if(format.suppress_assignment)
                {
				    arg_ptr = 0;
				}
				else
				{
					arg_ptr = va_arg(arg, char *);
				}
#endif /* __m56800E__ */
				if (arg_ptr)
				{
					if (is_secure)
					{
						elem_valid = 1;
						elem_maxsize = va_arg(arg, rsize_t);
					}
					
					num_chars = 0;
					
					if (match_failure)
					{
						if (elem_maxsize > 0)
							*arg_ptr = 0;
						continue;
					}
					else
					{
						char * arg_start = arg_ptr;
						
						while (format.field_width-- &&
							(!is_secure || ((elem_valid = (elem_maxsize > num_chars)) != 0)) &&
							((rval = ((*ReadProc)(ReadProcArg, 0, __GetAChar))) != EOF))	/*- mm 990325 -*/ /*- mm 010426 -*/ /*- mm 011207 -*/
						{
							c = rval;													/*- mm 010426 -*/
#if _MSL_WIDE_CHAR
							if (format.argument_options == wchar_argument)		/*- mm 990407 -*/
							{
								mbtowc(((wchar_t*)arg_ptr), (char*)(&c), 1);
								arg_ptr++;
							}
							else
#endif /* _MSL_WIDE_CHAR */													/*- mm 990407 -*/
								*arg_ptr++ = c;
							++num_chars;
						}
						
						c = rval;													/*- mm 020103 -*/
						if (!num_chars || (is_secure && !elem_valid))
						{
							if (!is_secure)
								goto exit;
							else
							{
								match_failure = 1;
								if (elem_maxsize > 0)
									*arg_start = 0;
								continue;
							}
						}
						
						chars_read += num_chars;
						
						++items_assigned;
					}
				}
				else
				{
					num_chars = 0;
					
					while (format.field_width-- && 
						((rval = ((*ReadProc)(ReadProcArg, 0, __GetAChar))) != EOF))	/*- mm 011207 -*/
					{
						c = rval;
						++num_chars;
					}
					c = rval;															/*- mm 020103 -*/
					if (!num_chars)
						goto exit;
				}
				
				++conversions;
				
				break;
			
			case '%':
#ifdef __ALTIVEC__
				if (format.vec)
				{
					if (!is_secure)
						goto exit;
					else
					{
						match_failure = 1;
						continue;
					}
				}
#endif /* __ALTIVEC__ */
				if (match_failure)
					continue;
				else
				{
					while (isspace(c = (*ReadProc)(ReadProcArg, 0, __GetAChar)))	/*- mm 990325 -*/
						++chars_read;
					
					if (c != '%')
					{
						(*ReadProc)(ReadProcArg, c, __UngetAChar);					/*- mm 990325 -*/
						if (!is_secure)
							goto exit;
						else
						{
							match_failure = 1;
							continue;
						}
					}
					
					++chars_read;
				}
				break;
			
			case 's':
#ifdef __ALTIVEC__
				if (format.vec)
				{
					if (!is_secure)
						goto exit;
					else
					{
						match_failure = 1;
						continue;
					}
				}
#endif /* __ALTIVEC__ */
				if (!match_failure)
				{
					c = (*ReadProc)(ReadProcArg, 0, __GetAChar);			 /*- mm 990325 -*/  /*- mm 990413 -*/
					while(isspace(c))                                        /*- mm 970218 -*/
					{                                                        /*- mm 970218 -*/
						++chars_read;                                        /*- mm 970218 -*/
						c = (*ReadProc)(ReadProcArg, 0, __GetAChar);         /*- mm 970218 -*/	/*- mm 990325 -*/
					}                                                        /*- mm 970218 -*/
					
					(*ReadProc)(ReadProcArg, c, __UngetAChar);				/*- mm 990325 -*/
				}
			case '[':
#ifdef __ALTIVEC__
				if (format.vec)
				{
					if (!is_secure)
						goto exit;
					else
					{
						match_failure = 1;
						continue;
					}
				}
#endif /* __ALTIVEC__ */
#if defined(__m56800E__)
                if(format.suppress_assignment)
                {
				    arg_ptr = 0;
				}
				else
				{
					arg_ptr = va_arg(arg, char *);
				}
#endif /* __m56800E__ */
				if (arg_ptr)
				{
					if (is_secure)
					{
						elem_valid = 1;
						elem_maxsize = va_arg(arg, rsize_t) - 1;
					}
					
					num_chars = 0;
					
					if (match_failure)
					{
						if (elem_maxsize > 0)
							*arg_ptr = 0;
						continue;
					}
					else
					{
						char * arg_start = arg_ptr;
						
						while (format.field_width-- && 
							(!is_secure || ((elem_valid = (elem_maxsize >= num_chars)) != 0)) &&
									((rval = ((*ReadProc)(ReadProcArg, 0, __GetAChar))) != EOF)) /*- mm 011207 -*/
						{
							c = rval;											/*- mm 011207 -*/
							
							if (!tst_char_map(format.char_set, c))				/*- mm 011207 -*/
								break;											/*- mm 011207 -*/
#if _MSL_WIDE_CHAR
							if (format.argument_options == wchar_argument)		/*- mm 990420 -*/
							{													/*- mm 990420 -*/
								mbtowc(((wchar_t*)arg_ptr), (char*)&c, 1);		/*- mm 990420 -*/
								arg_ptr = (char*)((wchar_t*)arg_ptr + 1);		/*- mm 990420 -*/
							}													/*- mm 990420 -*/
							else
#endif /* _MSL_WIDE_CHAR */													/*- mm 990420 -*/
								*arg_ptr++ = c;
							++num_chars;
						}
						c = rval;												/*- mm 020103 -*/
						
						if (!num_chars || (is_secure && !elem_valid))
						{
							(*ReadProc)(ReadProcArg, c, __UngetAChar);          /*- mm 961114 -*/	/*- mm 990325 -*/
							if (!is_secure)
								goto exit;
							else
							{
								match_failure = 1;
								if (elem_maxsize > 0)
									*arg_start = 0;
								continue;
							}
						}
						
						chars_read += num_chars;
						
						if (format.argument_options == wchar_argument)				/*- mm 990420 -*/
							*(wchar_t*)arg_ptr = L'\0';								/*- mm 990420 -*/
						else														/*- mm 990420 -*/
							*arg_ptr = 0;
						
						++items_assigned;
					}
				}
				else
				{
					num_chars = 0;
					
					while (format.field_width-- && 
								((rval = ((*ReadProc)(ReadProcArg, 0, __GetAChar))) != EOF))	/*- mm 011207 -*/
					{																			/*- mm 011207 -*/
						c = rval;																/*- mm 011207 -*/
						if (!tst_char_map(format.char_set, c))									/*- mm 011207 -*/
							break;																/*- mm 011207 -*/
						
						++num_chars;
					}
					c = rval;													/*- mm 020103 -*/
					
					if (!num_chars)
					{
						(*ReadProc)(ReadProcArg, c, __UngetAChar);               /*- mm 970513 -*//*- mm 990325 -*/
						/*goto exit;*/
						break;													 /*- mm 010818 -*/
					}
					chars_read += num_chars;          /*- mm 970501 -*/
				}
				
				if (format.field_width >= 0)
					(*ReadProc)(ReadProcArg, c, __UngetAChar);					/*- mm 990325 -*/
				
				++conversions;
				
				break;
			
			case 'n':
				
#if defined(__m56800E__)
			{
				int * arg_iptr = 0;
				if (format.argument_options == char_argument)
					arg_ptr = va_arg(arg,char *);
				else
					arg_iptr = va_arg(arg,int *);

				switch (format.argument_options)
				{
					case normal_argument:
						if (arg_iptr) 	(*((int *) arg_iptr)) = chars_read;
						break;
					case short_argument:
						if (arg_iptr) 	(*((short *) arg_iptr)) = chars_read;
						break;
					case long_argument:
						if (arg_iptr) 	(*((long *) arg_iptr)) = chars_read;
						break;
					case char_argument:
						if (arg_ptr) 	(*((char *) arg_ptr)) = chars_read;
						break;
#if _MSL_LONGLONG                                    /*- mm 961219 -*/
					case long_long_argument:
						if (arg_iptr) 	(*((long long *) arg_iptr)) = chars_read;
						break;
#endif /* _MSL_LONGLONG */                                    /*- mm 961219 -*/
				}
			}
#else /* !__m56800E__ */
				if (arg_ptr)
					switch (format.argument_options)
					{
						case normal_argument: * (int *)   arg_ptr = chars_read; break;
						case short_argument:  * (short *) arg_ptr = chars_read; break;
						case long_argument:   * (long *)  arg_ptr = chars_read; break;
						case char_argument:   * (char *)  arg_ptr = chars_read; break;			/*- mm 990414 -*/
#if _MSL_LONGLONG                                    /*- mm 961219 -*/
                        case long_long_argument: * (long long *) arg_ptr = chars_read; break;
#endif /* _MSL_LONGLONG */                                        /*- mm 961219 -*/
					}
#endif /* __m56800E__ */
				continue;
				
			case bad_conversion:
			default:
				
				goto exit;
		}
		
	}
	
exit:
	
	if ((*ReadProc)(ReadProcArg, 0, __TestForError) && conversions == 0)			/*- mm 990325 -*/
		return(EOF);
	
	return(items_assigned);
}

#if !defined(_Old_DSP_IO_Interface)

int _MSL_CDECL __FileRead(void *File, int ch, int Action)
{
	switch(Action)
	{
		case __GetAChar:
			return(__getc((FILE *)File));											/*- mm 040429 -*/
		case __UngetAChar:
			if (ch != EOF)							/*- mm 041029 -*/
				return(__ungetc(ch, (FILE *)File));	/*- mm 041029 -*/						/*- mm 040504 -*/
			else									/*- mm 041029 -*/
				return 0;							/*- mm 041029 -*/
		case __TestForError:
			return(ferror((FILE *)File) || feof((FILE *)File));
	}
	return 0;  /* to satisfy compiler */
}
#endif /*!defined(_Old_DSP_IO_Interface) */

int _MSL_CDECL __StringRead(void * isc, int ch, int Action)
{
	char RetVal;
	__InStrCtrl * Iscp = (__InStrCtrl*)isc;
	switch(Action)
	{
		case __GetAChar:
			RetVal = *(Iscp->NextChar);
			if (RetVal == '\0')
			{
				Iscp->NullCharDetected = 1;
				return (EOF);
			}
			else
			{
				Iscp->NextChar++;
				return((unsigned char)RetVal);	/*- mm 010426 -*/
			}
			
		case __UngetAChar:
			if (!Iscp->NullCharDetected)		/*- mm 990413 -*/
				Iscp->NextChar--;				/*- mm 990413 -*/
			else								/*- mm 990413 -*/
				Iscp->NullCharDetected = 0;		/*- mm 990413 -*/
			return(ch);
			
		case __TestForError:
			return(Iscp->NullCharDetected);
	}
	return 0;  /* to satisfy compiler */
}

#if !defined(_Old_DSP_IO_Interface)

int _MSL_CDECL fscanf(FILE * _MSL_RESTRICT file, const char * _MSL_RESTRICT format, ...)
{
	int retval;												/*- mm 001013 -*/
	int crtrgn;
#if (defined(__PPC_EABI__) || defined(__MIPS__))	
	va_list args;
#endif

	if (file == stdin)						/*- mm 020627 -*/
		crtrgn = stdin_access;				/*- mm 020627 -*/
	else									/*- mm 020627 -*/
		crtrgn = files_access;				/*- mm 020627 -*/
#if (defined(__PPC_EABI__) || defined(__MIPS__))
	if (file == NULL)										/*- mm 000404 -*/
		return(EOF);										/*- mm 000404 -*/
#if _MSL_WIDE_CHAR && _MSL_C99								/*- mm 980205 -*/	/*- mm 030314 -*/
	if  (fwide(file, -1) >= 0)								/*- mm 990618 -*/	/*- mm 000404 -*/
		return(EOF);
#endif /* _MSL_WIDE_CHAR && _MSL_C99 */                     /*- mm 980205 -*/	/*- mm 030314 -*/
	va_start( args, format );
  	__begin_critical_region(crtrgn);		 /*- mm 001013 -*/			/*- mm 020627 -*/
	retval = __sformatter(&__FileRead, (void *)file, format, args, 0); /*- mm 001013 -*/
#else
	if (file == NULL)										/*- mm 000404 -*/
		return(EOF);										/*- mm 000404 -*/
#if _MSL_WIDE_CHAR && _MSL_C99								/*- mm 980205 -*/	/*- mm 030314 -*/
	if (fwide(file, -1) >= 0)								/*- mm 990618 -*/	/*- mm 000404 -*/
		return(EOF);
#endif /* _MSL_WIDE_CHAR && _MSL_C99 */                     /*- mm 980205 -*/	/*- mm 030314 -*/
  	__begin_critical_region(crtrgn);		 /*- mm 001013 -*/			/*- mm 020627 -*/
	retval = __sformatter(&__FileRead, (void *)file, format, __va_start(format), 0); /*- mm 001013 -*/
#endif
  	__end_critical_region(crtrgn);	 /*- mm 001013 -*/				/*- mm 020627 -*/
	return(retval);								 /*- mm 001013 -*/
}

int _MSL_CDECL fscanf_s(FILE * _MSL_RESTRICT file, const char * _MSL_RESTRICT format, ...)
{
	int retval;												/*- mm 001013 -*/
	int crtrgn;
#if (defined(__PPC_EABI__) || defined(__MIPS__))
	va_list args;
#endif
	
	if (file == stdin)						/*- mm 020627 -*/
		crtrgn = stdin_access;				/*- mm 020627 -*/
	else									/*- mm 020627 -*/
		crtrgn = files_access;				/*- mm 020627 -*/
#if defined(__PPC_EABI__) || defined(__MIPS__)
	if ((file == NULL) || (format == NULL))
	{
		__msl_undefined_behavior_s();
		return(EOF);
	}
#if _MSL_WIDE_CHAR && _MSL_C99								/*- mm 980205 -*/	/*- mm 030314 -*/
	if  (fwide(file, -1) >= 0)								/*- mm 990618 -*/	/*- mm 000404 -*/
		return(EOF);
#endif /* _MSL_WIDE_CHAR && _MSL_C99 */                     /*- mm 980205 -*/	/*- mm 030314 -*/
	va_start( args, format );
  	__begin_critical_region(crtrgn);		 /*- mm 001013 -*/			/*- mm 020627 -*/
	retval = __sformatter(&__FileRead, (void *)file, format, args, 1); /*- mm 001013 -*/
#else
	if ((file == NULL) || (format == NULL))
	{
		__msl_undefined_behavior_s();
		return(EOF);
	}
#if _MSL_WIDE_CHAR && _MSL_C99								/*- mm 980205 -*/	/*- mm 030314 -*/
	if (fwide(file, -1) >= 0)								/*- mm 990618 -*/	/*- mm 000404 -*/
		return(EOF);
#endif /* _MSL_WIDE_CHAR && _MSL_C99 */                     /*- mm 980205 -*/	/*- mm 030314 -*/
  	__begin_critical_region(crtrgn);		 /*- mm 001013 -*/			/*- mm 020627 -*/
	retval = __sformatter(&__FileRead, (void *)file, format, __va_start(format), 1); /*- mm 001013 -*/
#endif
  	__end_critical_region(crtrgn);	 /*- mm 001013 -*/				/*- mm 020627 -*/
	return(retval);								 /*- mm 001013 -*/
}


/*- mm 000831 -*/
int _MSL_CDECL vscanf(const char * _MSL_RESTRICT format, va_list arg)
{
	int retval;									/*- mm 001013 -*/
#if _MSL_WIDE_CHAR && _MSL_C99					/*- mm 030314 -*/										
	if (fwide(stdin, -1) >= 0)
		return(EOF);
#endif /* _MSL_WIDE_CHAR && _MSL_C99 */ 		/*- mm 030314 -*/                                
  	__begin_critical_region(stdin_access);		/*- mm 001013 -*//*- mm 020627 -*/
	retval = __sformatter(&__FileRead, (void *)stdin, format,  arg, 0); /*- mm 001013 -*/
  	__end_critical_region(stdin_access);		 					 /*- mm 001013 -*//*- mm 020627 -*/
	return(retval);								 					 /*- mm 001013 -*/
}
/*- mm 000831 -*/

int _MSL_CDECL scanf(const char * _MSL_RESTRICT format, ...)
{
	int retval;												/*- mm 001013 -*/
#if (defined(__PPC_EABI__) || defined(__MIPS__))
	va_list args;
#if _MSL_WIDE_CHAR && _MSL_C99								/*- mm 980205 -*/	/*- mm 030314 -*/
	if (fwide(stdin, -1) >= 0)
		return(EOF);
#endif /* _MSL_WIDE_CHAR && _MSL_C99 */                     /*- mm 980205 -*/	/*- mm 030314 -*/
	
	va_start( args, format );
  	__begin_critical_region(stdin_access);		 			/*- mm 001013 -*/	/*- mm 020627 -*/
	retval = __sformatter(&__FileRead, (void *)stdin, format,  args, 0); /*- mm 001013 -*/
#else
#if _MSL_WIDE_CHAR && _MSL_C99								/*- mm 980205 -*/	/*- mm 030314 -*/
	if (fwide(stdin, -1) >= 0)
		return(EOF);
#endif /* _MSL_WIDE_CHAR && _MSL_C99 */                     /*- mm 980205 -*/	/*- mm 030314 -*/
  	__begin_critical_region(stdin_access);		 			/*- mm 001013 -*/	/*- mm 020627 -*/
	retval = __sformatter(&__FileRead, (void *)stdin, format, __va_start(format), 0); /*- mm 001013 -*/
#endif
  	__end_critical_region(stdin_access);		 			/*- mm 001013 -*/	/*- mm 020627 -*/
	return(retval);								 			/*- mm 001013 -*/
}

int _MSL_CDECL scanf_s(const char * _MSL_RESTRICT format, ...)
{
	int retval;												/*- mm 001013 -*/
#if (defined(__PPC_EABI__) || defined(__MIPS__))
	va_list args;
	if (format == NULL)
	{
		__msl_undefined_behavior_s();
		return(EOF);
	}
#if _MSL_WIDE_CHAR && _MSL_C99								/*- mm 980205 -*/	/*- mm 030314 -*/
	if (fwide(stdin, -1) >= 0)
		return(EOF);
#endif /* _MSL_WIDE_CHAR && _MSL_C99 */                     /*- mm 980205 -*/	/*- mm 030314 -*/
	
	va_start( args, format );
  	__begin_critical_region(stdin_access);		 			/*- mm 001013 -*/	/*- mm 020627 -*/
	retval = __sformatter(&__FileRead, (void *)stdin, format,  args, 1); /*- mm 001013 -*/
#else
	if (format == NULL)
	{
		__msl_undefined_behavior_s();
		return(EOF);
	}
#if _MSL_WIDE_CHAR && _MSL_C99								/*- mm 980205 -*/	/*- mm 030314 -*/
	if (fwide(stdin, -1) >= 0)
		return(EOF);
#endif /* _MSL_WIDE_CHAR && _MSL_C99 */                     /*- mm 980205 -*/	/*- mm 030314 -*/
  	__begin_critical_region(stdin_access);		 			/*- mm 001013 -*/	/*- mm 020627 -*/
	retval = __sformatter(&__FileRead, (void *)stdin, format, __va_start(format), 1); /*- mm 001013 -*/
#endif
  	__end_critical_region(stdin_access);		 			/*- mm 001013 -*/	/*- mm 020627 -*/
	return(retval);								 			/*- mm 001013 -*/
}

int _MSL_CDECL vfscanf(FILE * _MSL_RESTRICT file, const char * _MSL_RESTRICT format, va_list arg)		/*- mm 990828 -*/
{
	int retval;														/*- mm 001013 -*/
	int crtrgn;								/*- mm 020627 -*/
	if (file == stdin)						/*- mm 020627 -*/
		crtrgn = stdin_access;				/*- mm 020627 -*/
	else									/*- mm 020627 -*/
		crtrgn = files_access;				/*- mm 020627 -*/
	if (file == NULL)												/*- mm 000404 -*/
		return(EOF);												/*- mm 000404 -*/																	/* mm 990828 */
#if _MSL_WIDE_CHAR && _MSL_C99										/*- mm 990828 -*/	/*- mm 030314 -*/
	if (fwide(file, -1) >= 0)										/*- mm 990828 -*/
		return(EOF);												/*- mm 990828 -*/
#endif /* _MSL_WIDE_CHAR && _MSL_C99 */                             /*- mm 990828 -*/	/*- mm 030314 -*/
  	__begin_critical_region(crtrgn);								/*- mm 001013 -*/	/*- mm 020627 -*/
	retval = __sformatter(&__FileRead, (void *)file, format, arg, 0);  /*- mm 001013 -*/
  	__end_critical_region(crtrgn);		 							/*- mm 001013 -*/	/*- mm 020627 -*/ 
	return(retval);								 					/*- mm 001013 -*/
}																	/*- mm 990828 -*/

int _MSL_CDECL vfscanf_s(FILE * _MSL_RESTRICT file, const char * _MSL_RESTRICT format, va_list arg)		/*- mm 990828 -*/
{
	int retval;														/*- mm 001013 -*/
	int crtrgn;								/*- mm 020627 -*/
	if ((file == NULL) || (format == NULL))
	{
		__msl_undefined_behavior_s();
		return(EOF);
	}
	if (file == stdin)						/*- mm 020627 -*/
		crtrgn = stdin_access;				/*- mm 020627 -*/
	else									/*- mm 020627 -*/
		crtrgn = files_access;				/*- mm 020627 -*/
#if _MSL_WIDE_CHAR && _MSL_C99										/*- mm 990828 -*/	/*- mm 030314 -*/
	if (fwide(file, -1) >= 0)										/*- mm 990828 -*/
		return(EOF);												/*- mm 990828 -*/
#endif /* _MSL_WIDE_CHAR && _MSL_C99 */                             /*- mm 990828 -*/	/*- mm 030314 -*/
  	__begin_critical_region(crtrgn);								/*- mm 001013 -*/	/*- mm 020627 -*/
	retval = __sformatter(&__FileRead, (void *)file, format, arg, 1);  /*- mm 001013 -*/
  	__end_critical_region(crtrgn);		 							/*- mm 001013 -*/	/*- mm 020627 -*/ 
	return(retval);								 					/*- mm 001013 -*/
}																	/*- mm 990828 -*/
#endif /* !defined(_Old_DSP_IO_Interface) */

int _MSL_CDECL vsscanf(const char * _MSL_RESTRICT s, const char * _MSL_RESTRICT format, va_list arg)		/*- mm 990828 -*/
{
	__InStrCtrl isc;
	isc.NextChar         = (char *)s;
	if ((s == NULL) || (*isc.NextChar == '\0'))						/*- mm 990617 -*/
		return(EOF);												/*- mm 990617 -*/
	isc.NullCharDetected = 0;
	
	return(__sformatter(&__StringRead, (void *)&isc, format, arg, 0));
}

int _MSL_CDECL vsscanf_s(const char * _MSL_RESTRICT s, const char * _MSL_RESTRICT format, va_list arg)		/*- mm 990828 -*/
{
	__InStrCtrl isc;
	
	if ((s == NULL) || (format == NULL))
	{
		__msl_undefined_behavior_s();
		return(EOF);
	}
	
	isc.NextChar         = (char *)s;
	isc.NullCharDetected = (*isc.NextChar == '\0');
	
	return(__sformatter(&__StringRead, (void *)&isc, format, arg, 1));
}

int _MSL_CDECL sscanf(const char * _MSL_RESTRICT s, const char * _MSL_RESTRICT format, ...)
{
#if (defined(__PPC_EABI__) || defined(__MIPS__))
	va_list args;
	
	va_start( args, format );
	return(vsscanf(s, format, args));				/*- mm 990828 -*/
#else
	return(vsscanf(s, format, __va_start(format)));	/*- mm 990828 -*/
#endif
}

int _MSL_CDECL sscanf_s(const char * _MSL_RESTRICT s, const char * _MSL_RESTRICT format, ...)
{
#if (defined(__PPC_EABI__) || defined(__MIPS__))
	va_list args;
	
	va_start( args, format );
	return(vsscanf_s(s, format, args));				/*- mm 990828 -*/
#else
	return(vsscanf_s(s, format, __va_start(format)));	/*- mm 990828 -*/
#endif
}

/* Change record:
 * JFH 950926 First code release.
 * JFH 950929 Fixed misinterpretation of Standard concerning what to do when EOF
 *			  is reached. Now format string continues to be processed until input
 *			  is absolutely necessary (i.e., '%n' still processed even if preceding
 *			  directive hit EOF).
 * JFH 960212 Fixed longstanding (yet undiscovered) bug in parse_format's handling
 *			  of field widths. field_width was initialized to INT_MAX as a default.
 *			  If a digit was seen, field_width was *not* being zeroed before
 *			  accumulating digits in the usual manner.
 * JFH 960130 Changed vfscanf and vsscanf to __vfscanf and __vsscanf, made them
 *			  non-static, and added their prototypes to <stdio.h>.
 * JFH 960219 added #ifndef around area that handles floating point input. If disabled, you
 *			  can get away with not including MathLib just because you wanted to use scanf.
 *			  There is also some ANSI lib code savings as well.
 * JFH 960304 Fixed bug in handling of strings (%s) and scansets (%[]) where the last char
 *			  fetched was being ungotten *even if* the conversion ended because the
 *			  field width was exhausted (thus causing the last char of the previous field
 *			  to be rescanned).
 * JFH 960421 Fix bug in scanset inversion (*p++ = ~*p is ill-formed because order of
 *			  evaluation is not guaranteed).
 * JFH 960829 added l. you's changes for powertv, no floating point
 * mm  960930 Removed bitfields from scan_format structure to compile with ANSI strict
 * mm  961002 Corrected problem recognizing eof
 * mm  961114 make sure non matching character is left unread
 * mm  961220 Additions for support of long long
 * mm  970218 To make sscanf work with null source string.
 * mm  970501 To count input characters even when assignment suppressed
 * mm  970513 Make sure non matching character is left unread even when assignment suppressed
 * SCM 970709 Converted __va_start() calls to va_start() for PPC_EABI.
 * FS  980113 Converted __va_start() calls to va_start() for MIPS_Bare.
 * mm  971202 To complete mm 970218 
 * mm  980204 Added wide character support
 * blc 980317 Added ll (long long)
 *			  Fixed L (long double) prefix
 *            added a support for decimal input (symmetric to printf)
 *            added hh (char) prefix
 * blc 980318 Changed hh to be (signed char) unless explicitly unsigned
 * mm  980331 Modification to allow characters >= 128, e.g. § or É to be matched from format
 *            string correctly. MW06720
 * mm  981104 Inserted missing setting of *format IL9810-2059
 * vss 990203 Added AltiVec changes (contributed)
 * mm  990325 Changes to split string scanf functions from file i/o
 * mm  990407 Added code to handle %lc
 * mm  990413 Corrected handling of case where source text quits early IL9904-0736
 * mm  990414 Make %hhn work
 * mm  990413 Corrected handling of case where source text quits early IL9904-0736
 * mm  990527 Corrected of string read function in case of unget and made %n work after end of file
 * mm  990617 Correction for case where pointer to source string for sscanf == NULL or source string is null
 * mm  990716 Correction to code for checking format.char_set
 * mm  990817 Deleted include of <string_io.h>
 * mm  990828 Restored vfscanf and vsscanf since they are defined in C9x
 * vss 990909 Fixed Altivec bug with va_args not being assigned on time
 * mm  000404 Corrected detection of null input file
 * mm  000831 Added vscanf since it is defined in C99 WB1-16280
 * mm  001013 Added threadsafety
 * mm  010426 Correction to detect difference between character 0xff and eof.
 * mm  010818 Correction to make sure that if no characters are ignored, scanf doesn't abort.  WB1-26254 
 * cc  011203 Added _MSL_CDECL for new name mangling
 * mm  011207 Changes to detect characters of value 0xff as distinct from EOF 
 * mm  020103 Corrected mm 011207 changes
 * cc  020109 More _MSL_CDECL 
 * mm  020529 Support for C99 length modifiers j, z, and t.
 * mm  020627 Changes to prevent io on stdin, stdout, stderr and general files from interfering with each other
 * JWW 020917 Added _MSL_RESTRICT to get the restrict type specifier for certain C99 functions
 * mm  021217 Changes to bring scanf up to C99
 * JWW 030224 Changed __MSL_LONGLONG_SUPPORT__ flag into the new more configurable _MSL_LONGLONG
 * JWW 030224 Changed __NO_WIDE_CHAR flag into the new more configurable _MSL_WIDE_CHAR
 * mm  030304 Added some _MSL_C99 wrappers
 * mm  030314 Added some more _MSL_C99 wrappers
 * mm  030317 And some more
 * JWW 031030 Added the __STDC_WANT_SECURE_LIB__ secure library extensions
 * cc  040217 Changed _No_Floating_Point to _MSL_FLOATING_POINT 
 * cc  040217 Changed _No_Disk_File_OS_Support to _MSL_OS_DISK_FILE_SUPPORT
 * JWW 040317 Updated the __STDC_WANT_SECURE_LIB__ secure library based on new secure specifications
 * mm  040429 Changes to avoid nesting of critical regions
 * mm  040811 Changes to handle arguments of type intmax_t
 * cc  041014 Added _Old_DSP_IO_Interface checks
 * mm  041029 Changed to handle an empty string under %s and return EOF, not zero.
 * cc  050504 Changed back to old way of handling empty string
 */