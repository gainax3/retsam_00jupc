/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:34:37 $
 * $Revision: 1.52.2.2 $
 */

/*	Routines
 *	--------
 *		wscanf
 *		wscanf_s
 *		fwscanf
 *		fwscanf_s
 *		swscanf
 *		swscanf_s
 *
 *		vwscanf
 *		vwscanf_s
 *		vswscanf
 *		vswscanf_s
 *		vfwscanf
 *		vfwscanf_s
 */

#pragma ANSI_strict off  		/*- vss 990729 -*/

#define __STDC_WANT_SECURE_LIB__ 1
#include <ansi_parms.h> 
  
#if _MSL_WIDE_CHAR				/*- mm 980204 -*/
#if _MSL_C99					/*- mm 030304 -*/

#pragma ANSI_strict reset
 
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>
#include <critical_regions.h>
#include <msl_secure_error.h>

#if !_MSL_FLOATING_POINT && _MSL_FLOATING_POINT_IO
	#error Defining _MSL_FLOATING_POINT_IO cannot be on when _MSL_FLOATING_POINT is off!
#endif

#if _MSL_FLOATING_POINT_IO
	#include <math.h>
#endif /* _MSL_FLOATING_POINT_IO */

#define conversion_buff_size	512
#define conversion_max			509
#define bad_conversion			0xFFFF

enum argument_options
{
	normal_argument,
	char_argument,
	short_argument,
	long_argument,
	intmax_argument,
	size_t_argument,
	ptrdiff_argument,
	long_long_argument,								/*- mm 961219 -*/
	double_argument,
	long_double_argument,
	wchar_argument									/*- mm 990407 -*/
};

typedef char char_map[8192];						/*- mm 990914 -*/

typedef struct
{
	unsigned char	suppress_assignment;
	unsigned char 	field_width_specified;
	unsigned char 	argument_options;
	wchar_t			conversion_char;
	int				field_width;
	char_map		char_set;
} scan_format;

#define set_char_map(map, ch)	 map[ch>>3] |= (1 << (ch&7))
#define tst_char_map(map, ch) 	(map[ch>>3] &  (1 << (ch&7)))

static const wchar_t * parse_format(const wchar_t * format_string, scan_format * format)
{
	const wchar_t *	s = format_string;
	wchar_t			c;
	int				flag_found, invert;
	/*scan_format		f = {0, 0, normal_argument, 0, INT_MAX, {0}};*/					/*- mm 000419 -*/
	/* The following performs the above initialization without a large static area */	/*- mm 000419 -*/
	scan_format		f;																	/*- mm 000419 -*/
	f.suppress_assignment	 = 0;														/*- mm 000419 -*/
	f.field_width_specified	 = 0;														/*- mm 000419 -*/
	f.argument_options		 = normal_argument;											/*- mm 000419 -*/
	f.conversion_char 		 = 0;														/*- mm 000419 -*/
	f.field_width 			 = INT_MAX;													/*- mm 000419 -*/
	memset(&f.char_set, 0, sizeof(char_map));											/*- mm 000419 -*/

	if ((c = *++s) == L'%')
	{
		f.conversion_char = c;
		*format = f;
		return((const wchar_t *) s + 1);
	}
	
	if (c == L'*')
	{
		f.suppress_assignment = 1;
		
		c = *++s;
	}
	
	if (iswdigit(c))
	{
		f.field_width = 0;
		
		do
		{
			f.field_width = (f.field_width * 10) + (c - L'0');
			c = *++s;
		}
		while (iswdigit(c));
		
		if (f.field_width == 0)
		{
			f.conversion_char = bad_conversion;
			*format = f;
			return((const wchar_t *) s + 1);
		}
		
		f.field_width_specified = 1;
	}
	
	flag_found = 1;
	
	switch (c)
	{
		case L'h':
			
			f.argument_options = short_argument;
			if (s [1] == L'h')							/*- mm 990616 -*/
			{											/*- mm 990616 -*/
				f.argument_options = char_argument;		/*- mm 990616 -*/
				c = *++s; /* move past second h */		/*- mm 990616 -*/
			}											/*- mm 990616 -*/
			
			break;
		
		case L'l':
			
			f.argument_options = long_argument;
			if (s [1] == L'l')								/*- mm 990616 -*/
			{												/*- mm 990616 -*/
				f.argument_options = long_long_argument;	/*- mm 990616 -*/
				c = *++s; /* move past second l */			/*- mm 990616 -*/
			}												/*- mm 990616 -*/
			
			break;
		
		case L'L':
			
			f.argument_options = long_double_argument;         /*-mm 961219 -*/	/*- mm 990616 -*/
			
			break;
		
		case L'j':
		
			f.argument_options = intmax_argument;
			break;
			
		case L'z':
		
			f.argument_options = size_t_argument;
			break;
			
		case L't':
		
			f.argument_options = ptrdiff_argument;
			break;
		
		default:
			
			flag_found = 0;
	}
	
	if (flag_found)
		c = *++s;
	
	f.conversion_char = c;
	
	switch (c)
	{
		case L'd':
		case L'i':
		case L'u':
		case L'o':
		case L'x':
		case L'X':
			
			if (f.argument_options == long_double_argument)
			{
				f.conversion_char = bad_conversion;
				break;
			}

			break;
		
		case L'a':						/*- mm 990616 -*/
		case L'A':
		case L'f':
		case L'F':
		case L'e':
		case L'E':
		case L'g':
		case L'G':
			
			if (f.argument_options == short_argument)
			{
				f.conversion_char = bad_conversion;
				break;
			}
			
			/* 'l' before float identifier means double */

			if (f.argument_options == long_argument)
				f.argument_options = double_argument;
			
			if (f.argument_options == long_long_argument)     /*- mm 961219 -*/
				f.argument_options = long_double_argument;    /*- mm 961219 -*/
			
			break;
		
		case L'p':
			
			f.argument_options = long_argument;
			f.conversion_char  = L'x';
			
			break;
			
		case L'c':
			
			if (f.argument_options == long_argument)			/*- mm 990407 -*/
				f.argument_options = wchar_argument;			/*- mm 990407 -*/
			else												/*- mm 990407 -*/
				if (f.argument_options != normal_argument)
					f.conversion_char = bad_conversion;
			
			break;
			
		case L's':
			
			if (f.argument_options == long_argument)			/*- mm 990407 -*/
				f.argument_options = wchar_argument;			/*- mm 990407 -*/
			else												/*- mm 990407 -*/
				if (f.argument_options != normal_argument)
					f.conversion_char = bad_conversion;
			
			{
				int		i;
				char *	p;
				
				for (i = sizeof(f.char_set), p = f.char_set; i; --i)
					*p++ = 0xFF;
				
				f.char_set[1] = 0xC1;	/* sets bits for HT, LF, VT, FF, and CR chars to zero */
				f.char_set[4] = 0xFE;	/* set bit for Sp char to zero */
			}
			
			break;
			
		case L'n':
			
			break;
		
		case L'[':
			if (f.argument_options == long_argument)			/*- mm 990624 -*/
				f.argument_options = wchar_argument;			/*- mm 990624 -*/
			else												/*- mm 990624 -*/
				if (f.argument_options != normal_argument)
					f.conversion_char = bad_conversion;
			
			c = *++s;
			
			invert = 0;
			
			if (c == L'^')
			{
				invert = 1;
				c = *++s;
			}
			
			if (c == L']')
			{
				set_char_map(f.char_set, L']');
				c = *++s;
			}
			
			while (c && c != L']')
			{
				int d;
				
				set_char_map(f.char_set, c);
				
				if (*(s+1) == L'-' && (d = *(s+2)) != 0 && d != L']')
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
				char *	p;															/*- mm 990408 -*/
				
				for (i = sizeof(f.char_set), p = (char*)f.char_set; i; --i, ++p)	/*- mm 990408 -*/
					*p = ~*p;
			}
			
			break;
		
		default:
			
			f.conversion_char = bad_conversion;
			
			break;
	}
	
	*format = f;
	
	return((const wchar_t *) s + 1);
}

static int __wsformatter(wint_t (*wReadProc)(void *, wint_t, int), void * wReadProcArg, 
									const wchar_t * format_str, va_list arg, int is_secure)  /*- mm 990325 -*/
{
	int					num_chars, chars_read, items_assigned, conversions, i;
	int					base, negative, overflow;
	const wchar_t *		format_ptr;
	wchar_t				format_char;           /*- mm 961002 -*/
	wchar_t             c;	                 /*- mm 961002 -*/
	scan_format			format;
	long				long_num;
	unsigned long		u_long_num;
#if _MSL_LONGLONG                /*- mm 961219 -*/
	long long   		long_long_num = 0;         /*- mm 961219 -*/
	unsigned long long 	u_long_long_num;       /*- mm 961219 -*/
#endif /* _MSL_LONGLONG */                     /*- mm 961219 -*/
#if _MSL_FLOATING_POINT && _MSL_FLOATING_POINT_IO
	long double			long_double_num;
#endif /* _MSL_FLOATING_POINT */
	wchar_t *			arg_ptr;
	int					elem_valid;
	size_t				elem_maxsize;
	int match_failure = 0;
	int terminate  = 0;			/*- mm 990608 -*/
	
	format_ptr     = format_str;
	chars_read     = 0;
	items_assigned = 0;
	conversions    = 0;
	
	while (!terminate && (format_char = *format_ptr) != 0) /*- mm 990311 -*/	/*- mm 990608 -*/
	{
		if (iswspace(format_char))
		{
			do
				format_char = *++format_ptr;
			while (iswspace(format_char));
			
			if (!match_failure)
			{
				while ((iswspace)(c = (*wReadProc)(wReadProcArg, 0, __GetAwChar)))	/*- mm 990325 -*/  /*- mm 990409 -*/
					++chars_read;
				
				(*wReadProc)(wReadProcArg, c, __UngetAwChar);						/*- mm 990325 -*/
			}
			continue;
		}
		
		if ((format_char != L'%') && (!match_failure))
		{
			if ((c =(*wReadProc)(wReadProcArg, 0, __GetAwChar)) != format_char) /*- mm 990325 -*/
			{
				(*wReadProc)(wReadProcArg, c, __UngetAwChar);					/*- mm 990325 -*/
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
		
		format_ptr = parse_format(format_ptr, &format);
		
		if (!format.suppress_assignment && format.conversion_char != L'%')
			arg_ptr = va_arg(arg, wchar_t *);						/*- JWW 011027 -*/
		else
			arg_ptr = 0;
		
		if ((format.conversion_char != L'n') && (!match_failure) && (*wReadProc)(wReadProcArg, 0, __TestForwcsError))	/*- mm 990608 -*/
		{																					/*- mm 990608 -*/
			if (!is_secure)
				goto exit;																	/*- mm 990608 -*/
			else
				match_failure = 1;
		}																					/*- mm 990608 -*/
		
		switch (format.conversion_char)
		{
			case L'd':
				
				base = 10;
				
				goto signed_int;
				
			case L'i':
				
				base = 0;
				
			signed_int:
				
				if (match_failure)
				{
					long_num = 0;
#if _MSL_LONGLONG
					long_long_num = 0;
#endif /* _MSL_LONGLONG */
				}
				else
				{
#if _MSL_LONGLONG
	                if ((format.argument_options == long_long_argument) || /*- mm 961219 -*/
	                		(format.argument_options == intmax_argument))
	                   u_long_long_num =                               /*- mm 961219 -*/
	                      __wcstoull(base, format.field_width, wReadProc, 
	                      		wReadProcArg, &num_chars, &negative, &overflow);	/*- mm 990325 -*/
	                else                                               /*- mm 961219 -*/
#endif /* _MSL_LONGLONG */
					u_long_num
						= __wcstoul(base, format.field_width, wReadProc, 
									wReadProcArg, &num_chars, &negative, &overflow);/*- mm 990325 -*/
					
					if (!num_chars)
					{
						if (!is_secure)
							goto exit;
						else
						{
							match_failure = 1;
							long_num = 0;
#if _MSL_LONGLONG
							long_long_num = 0;
#endif /* _MSL_LONGLONG */
							goto signed_int_assign;
						}
					}
					
					chars_read += num_chars;
					
#if _MSL_LONGLONG                                    /*- mm 961219 -*/
	                if ((format.argument_options == long_long_argument) || /*- mm 961219 -*/
	                		(format.argument_options == intmax_argument))
	                   long_long_num = (negative ? -u_long_long_num : u_long_long_num); /*- mm 961219 -*/
	                else                                               /*- mm 961219 -*/
#endif /* _MSL_LONGLONG */                                         /*- mm 961219 -*/
					long_num = (negative ? -u_long_num : u_long_num);
				}
				
			signed_int_assign:
				if (arg_ptr)
				{
					switch (format.argument_options)
					{
						case normal_argument:  * (int *)         arg_ptr = long_num; break;
						case char_argument:    * (signed char *) arg_ptr = long_num; break;
						case short_argument:   * (short *)       arg_ptr = long_num; break;
						case long_argument:    * (long *)        arg_ptr = long_num; break;
#if _MSL_LONGLONG
						case intmax_argument:  * (intmax_t *)    arg_ptr = long_long_num; break;	/*- mm 020529 -*/ /*- mm 040811 -*/
#else
						case intmax_argument:  * (intmax_t *)    arg_ptr = long_num; break;	/*- mm 020529 -*/
#endif /* _MSL_LONGLONG */
						case size_t_argument:  * (size_t *) 	 arg_ptr = long_num; break;	/*- mm 020529 -*/
						case ptrdiff_argument: * (ptrdiff_t *) 	 arg_ptr = long_num; break;	/*- mm 020529 -*/
#if _MSL_LONGLONG                                    /*- mm 961219 -*/
                        case long_long_argument: * (long long *) arg_ptr = long_long_num; break;
#endif /* _MSL_LONGLONG */                                             /*- mm 961219 -*/
					}
					if (!match_failure)
						++items_assigned;
				}
				
				++conversions;
				
				break;
				
			case L'o':
				
				base = 8;
				
				goto unsigned_int;
				
			case L'u':
				
				base = 10;
				
				goto unsigned_int;
				
			case L'x':
			case L'X':
				
				base = 16;
				
			unsigned_int:
				
				if (match_failure)
				{
					u_long_num = 0;
#if _MSL_LONGLONG
					u_long_long_num = 0;
#endif /* _MSL_LONGLONG */
				}
				else
				{
#if _MSL_LONGLONG			                                    /*- mm 961219 -*/	/*- mm 030317 -*/
	                if ((format.argument_options == long_long_argument) || /*- mm 961219 -*/
	                		(format.argument_options == intmax_argument))
	                   u_long_long_num =                               /*- mm 961219 -*/
	                      __wcstoull(base, format.field_width, wReadProc, 
	                      			wReadProcArg, &num_chars, &negative, &overflow);	/*- mm 990325 -*/
	                else                                               /*- mm 961219 -*/
#endif /* _MSL_LONGLONG */			                            /*- mm 961219 -*/	/*- mm 030317 -*/
					u_long_num
						= __wcstoul(base, format.field_width, wReadProc, 
									wReadProcArg, &num_chars, &negative, &overflow);	/*- mm 990325 -*/
					
					if (!num_chars)
					{
						if (!is_secure)
							goto exit;
						else
						{
							match_failure = 1;
							u_long_num = 0;
#if _MSL_LONGLONG
							u_long_long_num = 0;
#endif /* _MSL_LONGLONG */
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
				if (arg_ptr)
				{
					switch (format.argument_options)
					{
						case normal_argument:  * (unsigned int *)   	arg_ptr = u_long_num; break;
						case char_argument:    * (unsigned char *)  	arg_ptr = u_long_num; break;
						case short_argument:   * (unsigned short *) 	arg_ptr = u_long_num; break;
						case long_argument:    * (unsigned long *)  	arg_ptr = u_long_num; break;
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
					}
					
					if (!match_failure)
						++items_assigned;
				}
				
				++conversions;
				
				break;

#if _MSL_FLOATING_POINT
#if _MSL_FLOATING_POINT_IO
			case L'a':						/*- mm 990616 -*/
			case L'A':
			case L'f':
			case L'F':
			case L'e':
			case L'E':
			case L'g':
			case L'G':
				if (match_failure)
					long_double_num = NAN;
				else
				{
					long_double_num = __wcstold(format.field_width, wReadProc, 
											wReadProcArg, &num_chars, &overflow);	/*- mm 990325 -*/
					
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
				
				++conversions;

				break;

#endif /* _MSL_FLOATING_POINT */
#endif /*_MSL_FLOATING_POINT_IO */
			case L'c':
				
				if (!format.field_width_specified)
					format.field_width = 1;
				
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
						wchar_t * arg_start = arg_ptr;
						
						while (format.field_width-- &&
							(!is_secure || ((elem_valid = (elem_maxsize > num_chars)) != 0)) &&
							(c = (*wReadProc)(wReadProcArg, 0, __GetAwChar)) != WEOF)    /*- mm 980202 -*/ /*- mm 990325 -*/ /*- mm 990624 -*/
						{
							if (format.argument_options == wchar_argument)		/*- mm 990407 -*/
								*arg_ptr++ = c;									/*- mm 990407 -*/
							else												/*- mm 990407 -*/
							{
								i = wctomb((char *)arg_ptr, c);					/*- mm 990407 -*/
								if (i < 0)										/*- mm 990407 -*/
									goto exit;									/*- mm 990407 -*/
								arg_ptr = (wchar_t*)((char *)arg_ptr + i);							/*- mm 990407 -*/
							}
							++num_chars;
						}
						
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
						(c = (*wReadProc)(wReadProcArg, 0, __GetAwChar)) != WEOF)   /*- mm 980202 -*/ /*- mm 990325 -*/ /*- mm 990624 -*/
						++num_chars;
					
					if (!num_chars)
						goto exit;
				}
				
				++conversions;
				
				break;
			
			case L'%':
				if (match_failure)
					continue;
				else
				{
					while ((iswspace)(c = (*wReadProc)(wReadProcArg, 0, __GetAwChar)))		/*- mm 990409 -*/
						++chars_read;

					if (c != L'%')
					{
						(*wReadProc)(wReadProcArg, c, __UngetAwChar);	
						goto exit;
					}
					
					++chars_read;
				}
				break;
			
			case L's':
				if (!match_failure)
				{
					c = (*wReadProc)(wReadProcArg, 0, __GetAwChar);           /*- mm 970218 -*/	/*- mm 990325 -*/ /*- mm 990413 -*/
					while(iswspace(c))                                        /*- mm 970218 -*/
					{                                                         /*- mm 970218 -*/
						++chars_read;                                         /*- mm 970218 -*/
						c = (*wReadProc)(wReadProcArg, 0, __GetAwChar); /*- mm 970218 -*/ /*- mm 980130 -*/ /*- mm 990325 -*/
					}                                                        /*- mm 970218 -*/
					
					(*wReadProc)(wReadProcArg, c, __UngetAwChar);			/*- mm 990325 -*/
				}
			case L'[':
				
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
						wchar_t * arg_start = arg_ptr;
						
						while (format.field_width-- && 
							(!is_secure || ((elem_valid = (elem_maxsize >= num_chars)) != 0)) &&
								(c = (*wReadProc)(wReadProcArg, 0, __GetAwChar)) != WEOF && 
									tst_char_map(format.char_set, c)) /*- mm 980202 -*/	/*- mm 990325 -*/ 	/*- mm 990624 -*/
						{
							if (format.argument_options == wchar_argument)									/*- mm 990407 -*/
							{
								*(wchar_t*)arg_ptr = c;									/*- mm 990407 -*/	/*- mm 990616 -*/
								arg_ptr = (wchar_t*)((char *)arg_ptr + sizeof(wchar_t));/*- mm 990407 -*/ 	/*- mm 990624 -*/
							}																				/*- mm 990420 -*/
							else
							{
								i = wctomb((char *)arg_ptr, c);							/*- mm 990407 -*/
								if (i < 0)												/*- mm 990407 -*/
								{
									if (!is_secure)
										goto exit;											/*- mm 990407 -*/
									else
									{
										match_failure = 1;
										if (elem_maxsize > 0)
											*arg_start = 0;
										continue;
									}
								}
								arg_ptr = (wchar_t*)((char *)arg_ptr + i);				/*- mm 990407 -*/
							}
								
							++num_chars;
						}
						
						if (!num_chars || (is_secure && !elem_valid))
						{
							(*wReadProc)(wReadProcArg, c, __UngetAwChar);  /*- mm 961114 -*/	/*- mm 990325 -*/
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
						
						if (format.argument_options == wchar_argument)
							*arg_ptr = 0;
						else
							*(char *)arg_ptr = 0;
						
						++items_assigned;
					}
				}
				else
				{
					num_chars = 0;
					
					while (format.field_width-- && 
							(c = (*wReadProc)(wReadProcArg, 0, __GetAwChar)) != WEOF && 
									tst_char_map(format.char_set, c)) /*- mm 980202 -*/	/*- mm 990325 -*/ /*- mm 990624 -*/
						++num_chars;
					
					if (!num_chars)
					{
						(*wReadProc)(wReadProcArg, c, __UngetAwChar);  /*- mm 970513 -*/ /*- mm 990325 -*/
						/*goto exit;*/
						break;
					}
					chars_read += num_chars;          /*- mm 970501 -*/
				}
				
				if (format.field_width >= 0)
					(*wReadProc)(wReadProcArg, c, __UngetAwChar);	/*- mm 990325 -*/
				
				++conversions;
				
				break;
			
			case L'n':
				
				if (arg_ptr)
					switch (format.argument_options)
					{
						case normal_argument: * (int *)   arg_ptr = chars_read; break;
						case short_argument:  * (short *) arg_ptr = chars_read; break;
						case long_argument:   * (long *)  arg_ptr = chars_read; break;
						case char_argument:   * (char *)  arg_ptr = chars_read; break;			/*- mm 990616 -*/
#if _MSL_LONGLONG                                    /*- mm 961219 -*/
                        case long_long_argument: * (long long *) arg_ptr = chars_read; break;
#endif                                                             /*- mm 961219 -*/
					}
				
				continue;
				
			case bad_conversion:
			default:
				
				goto exit;
		}
		
	}
	
exit:
	
	if ((*wReadProc)(wReadProcArg, 0, __TestForwcsError) && conversions == 0)	/*- mm 990325 -*/
		return(EOF);                                            /*- mm 980202 -*/
	
	return(items_assigned);
}

/*- mm 990325 -*/
wint_t __wFileRead(void *File, wint_t ch, int Action)
{
	switch(Action)
	{
		case __GetAwChar:
			return(__fgetwc((FILE *)File));				/*- mm 040505 -*/
		case __UngetAwChar:
			return(__ungetwc(ch, (FILE *)File));		/*- mm 040505 -*/
		case __TestForwcsError:
			return(ferror((FILE *)File) || feof((FILE *)File));
	}
	return 0;  /* to satisfy compiler */
}

wint_t __wStringRead(void * isc, wint_t ch, int Action)
{
	wchar_t RetVal;
	__wInStrCtrl * Iscp = (__wInStrCtrl*)isc;
	switch(Action)
	{
		case __GetAwChar:
			RetVal = *(Iscp->wNextChar);
			if (RetVal == L'\0')
			{
				Iscp->wNullCharDetected = 1;
				return (WEOF);						/*- mm 990624 -*/
			}
			else
			{
				Iscp->wNextChar++;
				return(RetVal);
			}
		case __UngetAwChar:
			if (!Iscp->wNullCharDetected)			/*- mm 990413 -*/
				Iscp->wNextChar--;					/*- mm 990413 -*/
			else									/*- mm 990413 -*/
				Iscp->wNullCharDetected = 0;		/*- mm 990413 -*/
			return(ch);
		case __TestForwcsError:
			return(Iscp->wNullCharDetected);
	}
	return 0;  /* to satisfy compiler */
}


/*
	fwscanf
	
	Description
	
	The fwscanf function reads input fromthe stream 
	pointed to by file, under control of the wide string
	pointed to by format that specifies that admissible input
	sequences and how they are to be converted for assignment,
	using subsequent arguments as pointers to the objects to
	recieve the converted input. If there are insufficient
	arguments for the format, the behaviour is undefined. If the
	format is exhausted while arguments remain, the excess
	arguments are evaluated (as always) but are otherwise
	ignored.
	
	Returns
	
	The fwscanf function returns the value of the macro
	EOF if an input failure occurs before and conversion.
	Otherwise, the fwscanf function returns the number of input
	items assigned, whcih can be fewer than provided for, or
	even zero, in the event of an early matching failure.
*/

int _MSL_CDECL fwscanf(FILE * _MSL_RESTRICT file, const wchar_t * _MSL_RESTRICT format, ...)
{
	int retval;												/*- mm 001018 -*/
	int crtrgn;
#if (defined(__PPC_EABI__) || defined(__MIPS__))	
	va_list args;
#endif

	if (file == stdin)						/*- mm 020627 -*/
		crtrgn = stdin_access;				/*- mm 020627 -*/
	else									/*- mm 020627 -*/
		crtrgn = files_access;				/*- mm 020627 -*/
#if (defined(__PPC_EABI__) || defined(__MIPS__))
	if ((file == NULL) || (fwide(file, 1) <= 0))						/*- mm 000404 -*/
		return(EOF);
	va_start( args, format );
  	__begin_critical_region(crtrgn);		 /*- mm 001018 -*//*- mm 020627 -*/
	retval = __wsformatter(&__wFileRead, (void *)file, format, args, 0);	/*- mm 990804 -*/ /*- mm 001018 -*/
#else
	if ((file == NULL) || (fwide(file, 1) <= 0))						/*- mm 000404 -*/
		return(EOF);
  	__begin_critical_region(crtrgn);		 /*- mm 001018 -*//*- mm 020627 -*/
	retval = __wsformatter(&__wFileRead, (void *)file, format, __va_start(format), 0);/*- mm 001018 -*/
#endif
  	__end_critical_region(crtrgn);		 /*- mm 001018 -*//*- mm 020627 -*/
	return(retval);								 /*- mm 001018 -*/
}

int _MSL_CDECL fwscanf_s(FILE * _MSL_RESTRICT file, const wchar_t * _MSL_RESTRICT format, ...)
{
	int retval;												/*- mm 001018 -*/
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
	if (fwide(file, 1) <= 0)						/*- mm 000404 -*/
		return(EOF);
	va_start( args, format );
  	__begin_critical_region(crtrgn);		 /*- mm 001018 -*//*- mm 020627 -*/
	retval = __wsformatter(&__wFileRead, (void *)file, format, args, 1);	/*- mm 990804 -*/ /*- mm 001018 -*/
#else
	if ((file == NULL) || (format == NULL))
	{
		__msl_undefined_behavior_s();
		return(EOF);
	}
	if (fwide(file, 1) <= 0)						/*- mm 000404 -*/
		return(EOF);
  	__begin_critical_region(crtrgn);		 /*- mm 001018 -*//*- mm 020627 -*/
	retval = __wsformatter(&__wFileRead, (void *)file, format, __va_start(format), 1);/*- mm 001018 -*/
#endif
  	__end_critical_region(crtrgn);		 /*- mm 001018 -*//*- mm 020627 -*/
	return(retval);								 /*- mm 001018 -*/
}


/*
	wscanf
	
	Description
	
	The wscanf function is equivalent to fwscanf with the 
	argument stdin interposed before the arguments to wscanf

	Returns
	
	The wscanf function returns the value of the macro EOF
	if an input failure occurs before any conversion. Otherwise,
	the wscanf function returns the number of iniput items 
	assigned, whcih can be fewer than provided for, or even
	zero, in the event of an early matching failure.
*/

int _MSL_CDECL wscanf(const wchar_t * _MSL_RESTRICT format, ...)
{
	int retval;									/*- mm 001018 -*/
#if (defined(__PPC_EABI__) || defined(__MIPS__))
	va_list args;
	if (fwide(stdin, 1) <= 0)
		return(EOF);
	
	va_start( args, format );
  	__begin_critical_region(stdin_access);		 /*- mm 001018 -*/ /*- mm 020627 -*/
	retval = __wsformatter(&__wFileRead, (void *)stdin, format, args, 0);	/*- mm 990804 -*//*- mm 001018 -*/
#else
	if (fwide(stdin, 1) <= 0)
		return(EOF);
  	__begin_critical_region(stdin_access);		 /*- mm 001018 -*/ /*- mm 020627 -*/
	retval = __wsformatter(&__wFileRead, (void *)stdin, format, __va_start(format), 0);/*- mm 001018 -*/
#endif
  	__end_critical_region(stdin_access);		 /*- mm 001018 -*/ /*- mm 020627 -*/
	return(retval);								 /*- mm 001018 -*/
}

int _MSL_CDECL wscanf_s(const wchar_t * _MSL_RESTRICT format, ...)
{
	int retval;									/*- mm 001018 -*/
#if (defined(__PPC_EABI__) || defined(__MIPS__))
	va_list args;
	if (format == NULL)
	{
		__msl_undefined_behavior_s();
		return(EOF);
	}
	if (fwide(stdin, 1) <= 0)
		return(EOF);
	
	va_start( args, format );
  	__begin_critical_region(stdin_access);		 /*- mm 001018 -*/ /*- mm 020627 -*/
	retval = __wsformatter(&__wFileRead, (void *)stdin, format, args, 1);	/*- mm 990804 -*//*- mm 001018 -*/
#else
	if (format == NULL)
	{
		__msl_undefined_behavior_s();
		return(EOF);
	}
	if (fwide(stdin, 1) <= 0)
		return(EOF);
  	__begin_critical_region(stdin_access);		 /*- mm 001018 -*/ /*- mm 020627 -*/
	retval = __wsformatter(&__wFileRead, (void *)stdin, format, __va_start(format), 1);/*- mm 001018 -*/
#endif
  	__end_critical_region(stdin_access);		 /*- mm 001018 -*/ /*- mm 020627 -*/
	return(retval);								 /*- mm 001018 -*/
}

/*
	vwscanf
	
	Description
	
	The vwscanf function is equivalent to wscanf with the 
	variable argument list replaced by arg, which shall have
	been initialized by the va_start macro (and possibly
	subsequent va_arg calls). The vwscanf function does not
	invoke the va_end macro

	Returns
	
	The vwscanf function returns the value of the macro EOF
	if an input failure occurs before any conversion. Otherwise,
	the wscanf function returns the number of iniput items 
	assigned, whcih can be fewer than provided for, or even
	zero, in the event of an early matching failure.
*/

int _MSL_CDECL vwscanf(const wchar_t * _MSL_RESTRICT format, va_list arg)
{
	int retval;									/*- mm 001018 -*/
	if (fwide(stdin, 1) <= 0)
		return(EOF);
  	__begin_critical_region(stdin_access);		 /*- mm 001018 -*/ /*- mm 020627 -*/
	 retval = __wsformatter(&__wFileRead, (void *)stdin, format, arg, 0);  /*- mm 001018 -*/
  	__end_critical_region(stdin_access);		 /*- mm 001018 -*/ /*- mm 020627 -*/
	return(retval);								 /*- mm 001018 -*/
}

int _MSL_CDECL vwscanf_s(const wchar_t * _MSL_RESTRICT format, va_list arg)
{
	int retval;									/*- mm 001018 -*/
	if (format == NULL)
	{
		__msl_undefined_behavior_s();
		return(EOF);
	}
	if (fwide(stdin, 1) <= 0)
		return(EOF);
  	__begin_critical_region(stdin_access);		 /*- mm 001018 -*/ /*- mm 020627 -*/
	 retval = __wsformatter(&__wFileRead, (void *)stdin, format, arg, 1);  /*- mm 001018 -*/
  	__end_critical_region(stdin_access);		 /*- mm 001018 -*/ /*- mm 020627 -*/
	return(retval);								 /*- mm 001018 -*/
}

/*  vfwscanf

       Description

       [#2] The vfwscanf function is equivalent  to  fwscanf,  with
       the variable argument list replaced by arg, which shall have
       been  initialized  by  the  va_start  macro  (and   possibly
       subsequent  va_arg  calls).   The vfwscanf function does not
       invoke the va_end macro.

       Returns

       [#3] The vfwscanf function returns the value  of  the  macro
       EOF  if  an  input  failure  occurs  before  any conversion.
       Otherwise, the vfwscanf function returns the number of input
       items  assigned,  which  can  be fewer than provided for, or
       even zero, in the event of an early matching failure.
*/
int _MSL_CDECL vfwscanf(FILE * _MSL_RESTRICT stream, const wchar_t * _MSL_RESTRICT format, va_list arg)				/*- mm 990829 -*/
{
	int retval;									/*- mm 001018 -*/
	int crtrgn;
	if ((stream == NULL) || (fwide(stream, 1) <= 0))								/*- mm 000404 -*/
		return(EOF);
	if (stream == stdin)						/*- mm 020627 -*/
		crtrgn = stdin_access;				/*- mm 020627 -*/
	else									/*- mm 020627 -*/
		crtrgn = files_access;				/*- mm 020627 -*/
  	__begin_critical_region(crtrgn);		 /*- mm 001018 -*/			/*- mm 020627 -*/
	retval = __wsformatter(&__wFileRead, (void *)stream, format, arg, 0); /*- mm 001018 -*/
  	__end_critical_region(crtrgn);		 /*- mm 001018 -*/ /*- mm 020627 -*/
	return(retval);								 /*- mm 001018 -*/
}

int _MSL_CDECL vfwscanf_s(FILE * _MSL_RESTRICT stream, const wchar_t * _MSL_RESTRICT format, va_list arg)				/*- mm 990829 -*/
{
	int retval;									/*- mm 001018 -*/
	int crtrgn;
	if ((stream == NULL) || (format == NULL))
	{
		__msl_undefined_behavior_s();
		return(EOF);
	}
	if ((stream == NULL) || (fwide(stream, 1) <= 0))								/*- mm 000404 -*/
		return(EOF);
	if (stream == stdin)						/*- mm 020627 -*/
		crtrgn = stdin_access;				/*- mm 020627 -*/
	else									/*- mm 020627 -*/
		crtrgn = files_access;				/*- mm 020627 -*/
  	__begin_critical_region(crtrgn);		 /*- mm 001018 -*/			/*- mm 020627 -*/
	retval = __wsformatter(&__wFileRead, (void *)stream, format, arg, 1); /*- mm 001018 -*/
  	__end_critical_region(crtrgn);		 /*- mm 001018 -*/ /*- mm 020627 -*/
	return(retval);								 /*- mm 001018 -*/
}


/*
	vswscanf
	
	Description
	
	The vswscanf function is equivalent to swscanf with the 
	variable argument list replaced by arg, which shall have
	been initialized by the va_start macro (and possibly
	subsequent va_arg calls). The vwscanf function does not
	invoke the va_end macro

	Returns
	
	The vswscanf function returns the value of the macro EOF
	if an input failure occurs before any conversion. Otherwise,
	the wscanf function returns the number of iniput items 
	assigned, whcih can be fewer than provided for, or even
	zero, in the event of an early matching failure.
*/

int _MSL_CDECL vswscanf(const wchar_t * _MSL_RESTRICT s, const wchar_t * _MSL_RESTRICT format, va_list arg)  /*- mm 990828 -*/
{
	__wInStrCtrl wisc;
	wisc.wNextChar         = (wchar_t *)s;
	if ((s == NULL) || (*wisc.wNextChar == L'\0'))	/*- mm 990617 -*/
		return(EOF);							/*- mm 990617 -*/
	wisc.wNullCharDetected = 0;
	
	return(__wsformatter(&__wStringRead, (void *)&wisc, format, arg, 0));
}

int _MSL_CDECL vswscanf_s(const wchar_t * _MSL_RESTRICT s, const wchar_t * _MSL_RESTRICT format, va_list arg)  /*- mm 990828 -*/
{
	__wInStrCtrl wisc;
	
	if ((s == NULL) || (format == NULL))
	{
		__msl_undefined_behavior_s();
		return(EOF);
	}
	
	wisc.wNextChar         = (wchar_t *)s;
	wisc.wNullCharDetected = (*wisc.wNextChar == L'\0');
	
	return(__wsformatter(&__wStringRead, (void *)&wisc, format, arg, 1));
}


/*
	swscanf
	
	Description
	
	The swscanf function is equivalent to fwscanf, except
	that the argument s specifies a wide string from which
	the input is to be obtained, rather that from a stream. Reaching
	the end of the wide string is equivalent to encountering
	end-of-file for the fwscanf function.
	

	Returns
	
	The swscanf function returns the value of the macro EOF
	if an input failure occurs before any conversion. Otherwise,
	the wscanf function returns the number of iniput items 
	assigned, whcih can be fewer than provided for, or even
	zero, in the event of an early matching failure.
*/


int _MSL_CDECL swscanf(const wchar_t * _MSL_RESTRICT s, const wchar_t * _MSL_RESTRICT format, ...)
{
#if (defined(__PPC_EABI__) || defined(__MIPS__))
	va_list args;
	va_start( args, format );
	return(vswscanf(s, format, args));						/*- mm 990828 -*/
#else
	return(vswscanf(s, format, __va_start(format)));		/*- mm 990828 -*/
#endif
}

int _MSL_CDECL swscanf_s(const wchar_t * _MSL_RESTRICT s, const wchar_t * _MSL_RESTRICT format, ...)
{
#if (defined(__PPC_EABI__) || defined(__MIPS__))
	va_list args;
	
	va_start( args, format );
	return(vswscanf_s(s, format, args));					/*- mm 990828 -*/
#else
	return(vswscanf_s(s, format, __va_start(format)));		/*- mm 990828 -*/
#endif
}

#endif /* _MSL_C99 */										/*- mm 030304 -*/
#endif /* _MSL_WIDE_CHAR */									/*- mm 981020 -*/

/* Change record:
 * JCM 980121 First code release.
 * mm  980206 Added call to fwide()
 * mm  981020 Added #if _MSL_WIDE_CHAR wrappers
 * mm  990325 Changes to split string scan functions from file i/o
 * mm  990407 Changes to handle %ls and %lc
 * mm  990408 Correction to invert char set map code.
 * mm  990409 Correction to avoid using macro for iswspace() where there are side-effects.
 * mm  990409 Change to handle NULL source string.
 * mm  990413 Corrected handling of case where source text quits early IL9904-0736
 * mm  990624 Corrected assignment to wide-char target
 * mm  990624 Implemented the %l[ specification
 * mm  990804 Corrected 	calls under __PPC_EABI__ || __MIPS__
 * mm  990805 Changed vfwscanf to wsformatter in embedded targets
 * mm  990817 Deleted include of <string_io.h>
 * mm  990914 Corrected size of char_map to allow for full wchar character set IR9909-0523
 * mm  000404 Inserted code to detect null input fle  WB1-12370
 * mm  000419 Initialized char_map without large static area, thanks to Andreas
 * mm  001018 Added threadsafety to vwscanf, fwscanf, wscanf, and vfwscanf
 * JWW 011027 Use wchar_t in va_arg, not unsigned short, since w_char_t may not really be short
 * cc  011203 Added _MSL_CDECL for new name mangling 
 * blc 011211 Added _MSL_NO_MATH_LIB macro check
 * mm  020627 Changes to prevent io on stdin, stdout, stderr and general files from interfering with each other
 * JWW 020917 Added _MSL_RESTRICT to get the restrict type specifier for certain C99 functions
 * JWW 030224 Changed __MSL_LONGLONG_SUPPORT__ flag into the new more configurable _MSL_LONGLONG
 * JWW 030224 Changed __NO_WIDE_CHAR flag into the new more configurable _MSL_WIDE_CHAR
 * mm  030304 Added _MSL_C99 wrapper
 * JWW 031030 Added the __STDC_WANT_SECURE_LIB__ secure library extensions
 * cc  040217 Changed _No_Floating_Point to _MSL_FLOATING_POINT
 * JWW 040317 Updated the __STDC_WANT_SECURE_LIB__ secure library based on new secure specifications
 * mm  040505 Changes to __wFileRead to avoid nested critical regions
 */