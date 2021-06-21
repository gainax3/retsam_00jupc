/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:34:02 $
 * $Revision: 1.95.2.4 $
 */

/*	Functions:
 *	---------	
 *		parse_format
 *		long2str
 *		longlong2str
 *		double2hex
 *		round_decimal
 *		float2str
 *		__pformatter
 *		__FileWrite
 *		__StringWrite
 *
 *	Routines
 *	--------
 *		
 *		vfprintf
 *		fprintf
 *
 *		vprintf
 *		printf
 *
 *		vsprintf
 *      vsnprintf
 *		sprintf
 *		snprintf
 */

#include <ansi_parms.h>
#include <critical_regions.h>
#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#if !_MSL_FLOATING_POINT && _MSL_FLOATING_POINT_IO
	#error Defining _MSL_FLOATING_POINT_IO cannot be on when _MSL_FLOATING_POINT is off!
#endif


#if _MSL_FLOATING_POINT_IO
	#include <ansi_fp.h>
	#include <float.h>
	#include <lconv.h>
	#include <locale.h>
	#include <msl_thread_local_data.h>
#endif /* _MSL_FLOATING_POINT_IO */

#ifndef __MWERKS__
	#define _MSL_COMPILE_FOR_SPEED 1
#else
	#if __option(optimize_for_size)
		#define _MSL_COMPILE_FOR_SPEED 0
	#else
		#define _MSL_COMPILE_FOR_SPEED 1
	#endif
#endif

#define conversion_buff_size	512
#define conversion_max			509
#define bad_conversion			0xFF

enum justification_options
{
	left_justification,
	right_justification,
	zero_fill
};

enum sign_options
{
	only_minus,
	sign_always,
	space_holder
};

enum argument_options
{
	normal_argument,
	char_argument,
	short_argument,
	long_argument,
	
#if _MSL_LONGLONG         				/*- mm 961219 -*/
	long_long_argument,                 /*- mm 961219 -*/
#endif /* _MSL_LONGLONG */  			/*- mm 961219 -*/
	
#if _MSL_WIDE_CHAR						/*- mm 990325 -*/
	wchar_argument,						/*- mm 990325 -*/
#endif /* _MSL_WIDE_CHAR */
	
#if _MSL_C99							/*- mm 030304 -*/
	intmax_argument,					/*- mm 020529 -*/
	size_t_argument,					/*- mm 020529 -*/
	ptrdiff_argument,					/*- mm 020529 -*/
#endif									/*- mm 030304 -*/
	
#ifdef __ALTIVEC__
	vector_argument,
#endif /* __ALTIVEC__ */
	
	long_double_argument
};


#ifdef __ALTIVEC__
union MWVector128
{
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

#ifndef __VEC__
	#define vec_va_arg(ap) (*(((MWVector128 *) (ap = (void *) ((((unsigned long) ap + 15) & ~(15)) + 16))) -1))
	#define va_arg_replacement1(ap, vec_val) (vec_val = vec_va_arg(ap))
#endif /* __VEC__ */
#endif /* __ALTIVEC__ */

#define MAX_SIG_DIG 32 

#if _MSL_FLOATING_POINT_IO
/*- ejs 021009 -*/
/*	
	The hex-float printing routine assumes an IEEE 754 float layout, i.e.:
	
	[sign_bit] [biased_exponent] [partial_mantissa]
	
	where the real_exponent is (biased_exponent - TARGET_FLOAT_MAX_EXP + 1),
	where the real_mantissa is "1.partial_mantissa" for 32 and 64 bit doubles 
	and ".partial_mantissa" for 80-bit doubles (with an explicit 1 bit), 
	and the value is (real_mantissa)*2^real_exponent*(sign_bit?-1:1).
*/
#define TARGET_FLOAT_BITS	_MSL_LONG_DOUBLE_SIZE
#define TARGET_FLOAT_BYTES	(TARGET_FLOAT_BITS/8)

/*	Define "maximum" exponent 
	The bias is max_exp-1, and the maximal value fitting
	in the exponent field is max_exp*2-1 */
#ifndef TARGET_FLOAT_MAX_EXP
#define TARGET_FLOAT_MAX_EXP	LDBL_MAX_EXP
#endif

/*	Define number of effective bits in mantissa
	This may include an implicit J-bit (i.e. for 1)
	in singles and short doubles. */
#ifndef TARGET_FLOAT_MANT_DIG
#define TARGET_FLOAT_MANT_DIG	LDBL_MANT_DIG
#endif

/*	Define whether the one bit is implicit in the format. */
#ifndef TARGET_FLOAT_IMPLICIT_J_BIT
  #if TARGET_FLOAT_BITS==32
    #define TARGET_FLOAT_IMPLICIT_J_BIT	1
  #elif TARGET_FLOAT_BITS==64
    #define TARGET_FLOAT_IMPLICIT_J_BIT	1
  #elif TARGET_FLOAT_BITS==80
    #define TARGET_FLOAT_IMPLICIT_J_BIT	0	/* for x86 */
  #else
    #error define TARGET_FLOAT_IMPLICIT_J_BIT
  #endif
#endif

/*	Derive values sizes */
#define TARGET_FLOAT_MANT_BITS		(TARGET_FLOAT_MANT_DIG - TARGET_FLOAT_IMPLICIT_J_BIT)
#define TARGET_FLOAT_EXP_BITS		(TARGET_FLOAT_BITS - TARGET_FLOAT_MANT_BITS - 1)

#endif /* _MSL_FLOATING_POINT_IO */

typedef struct 
{
	unsigned char	justification_options;
	unsigned char 	sign_options;
	unsigned char 	precision_specified;
	unsigned char	alternate_form;
	unsigned char 	argument_options;
	unsigned char	conversion_char;
	int			  	field_width;
	int				precision;
#ifdef __ALTIVEC__
	int		vecCount;			/* number of vector fields to convert (4, 8, 16) 		*/
	char 	*vecSeperator;		/* separator string to insert between vector fields 	*/
	char 	*c_vecSeperator;	/* separator string to use for %c vector conversion		*/
	int		vecSepLen;			/* length of *vecSeperator or *c_vecSeperator string	*/
	int		useSepString;		/* @ -- asking for a separator string instead of char	*/
	int		fbad;				/* used to control standard flags parsing				*/
	int		vecwidth,vecprec;	/* width and prec value before each conversion 			*/
	int 	useVecDefault;		/* default to "appropriate" vec access if none specified*/
	int		flagAllowed;		/* true means a flag is still possible					*/
	enum 						/* state of vector formatting parse... 					*/
	{ 
		FLAG_OR_Vcode_OK,		/*   initial state: sep flag or 'v' code allowed  		*/
		Vcode_REQUIRED,			/*   set to this state when sep flag seen  reqiring 'v'	*/
		FLAG_OR_Vcode_NOT_OK	/*   set this state when 'v' code qualifier seen 		*/
	} vec_state;
	enum 						/* how to access vector fields (as chars,shorts,longs)	*/
	{
		U8, U16, U32
	} vec_access;
#endif /* __ALTIVEC__ */
} print_format;


/* Parse the format specifiers so appropriate types, precision, and formatting are used */
static const char * parse_format(const char * format_string, va_list * arg, print_format * format)
{
	print_format	f;
	const char *	s = format_string;
	int				c;
	int				flag_found;
	
	f.justification_options	= right_justification;
	f.sign_options			= only_minus;
	f.precision_specified   = 0;
	f.alternate_form		= 0;
	f.argument_options		= normal_argument;
	f.field_width			= 0;
	f.precision				= 0;
	
	if ((c = *++s) == '%')
	{
		f.conversion_char = c;
		*format = f;
		return ((const char *) s + 1);
	}
	
#ifdef __ALTIVEC__
	f.vec_state 	 = FLAG_OR_Vcode_OK;
	f.vecSeperator   = " ";
	f.c_vecSeperator = "";
	f.vecSepLen	   	 = 1;
getFlags:
	f.useSepString = 0;
#endif /* __ALTIVEC__ */
	
	for (;;)
	{
		flag_found = 1;
		
		switch (c)
		{
			/* PREFIX -  left justify output */
			case '-':
				f.justification_options = left_justification;
				break;
			
			/* PREFIX +  always display the numeric sign */
			case '+':
				f.sign_options = sign_always;
				break;
			
			/* PREFIX space  prefix positive numbers with a space (sign placeholder) */
			case ' ':
				if (f.sign_options != sign_always)
					f.sign_options = space_holder;
				break;
			
			/* PREFIX #  use "alternate form" for output format */
			case '#':
				f.alternate_form = 1;
				break;
			
			/* PREFIX 0  zero fill instead of filling justification with spaces */
			case '0':
				if (f.justification_options != left_justification)
					f.justification_options = zero_fill;
				break;
			
#ifdef __ALTIVEC__
			/* PREFIX @  AltiVec extension, next argument provides vector separator string */
			case '@':
				if (f.vec_state == FLAG_OR_Vcode_OK)
				{
					f.useSepString = 1;
					break;
				}
				/* fall through */
#endif /* __ALTIVEC__ */
			
			/* Any other prefix character is not a flag, so continue processing format arguments */
			default:
				flag_found = 0;
				break;
		}
		
		if (flag_found)
		{
#ifdef __ALTIVEC__
			f.c_vecSeperator = (char *) s;
			f.vecSepLen	   	 = 1;
#endif /* __ALTIVEC__ */
			c = *++s;
		}
		else
			break;
	}
	
#ifdef __ALTIVEC__
	if (f.useSepString)
	{
		/* PREFIX @ was used, so obtain the next argument to find the separator string */
		f.vecSeperator = va_arg(*arg, char *);
		f.vecSepLen    = strlen(f.vecSeperator);
	}
#endif /* __ALTIVEC__ */
	
	/* Specifier * means next argument provides the field length */
	if (c == '*')
	{
		if ((f.field_width = va_arg(*arg, int)) < 0)
		{
			f.justification_options = left_justification;
			f.field_width           = -f.field_width;
		}
		
		c = *++s;
	}
	else
	{
		/* Field length specified by a decimal number, parse the number */
		while (isdigit(c))
		{
			f.field_width = (f.field_width * 10) + (c - '0');
			c = *++s;
		}
	}
	
	if (f.field_width > conversion_max)
	{
		f.conversion_char = bad_conversion;
		*format = f;
		return ((const char *) s + 1);
	}
	
	/* Specifier . means floating point precision or string length comes next */
	if (c == '.')
	{
		f.precision_specified = 1;
		
		/* Specifier * means next argument provides the precision or string length */
		if ((c = *++s) == '*')
		{
			if ((f.precision = va_arg(*arg, int)) < 0)
				f.precision_specified = 0;
			
			c = *++s;
		}
		else
		{
			/* Precision or string length specified by a decimal number, parse the number */
			while (isdigit(c))
			{
				f.precision = (f.precision * 10) + (c - '0');
				c = *++s;
			}
		}
	}
	
	flag_found = 1;
	
#ifdef __ALTIVEC__
	f.useVecDefault = 1;			/* use vec default access if none specified below 	*/
	
	if (c == 'v')					/* allow 'v' to qualify length or format code 		*/
	{
		f.vec_state = FLAG_OR_Vcode_NOT_OK;
		f.argument_options = vector_argument;
		c = *++s;
	}
#endif /* __ALTIVEC__ */		
	
	switch (c)
	{
		/* Specifier h  argument will be of type short */
		case 'h':
#ifdef __ALTIVEC__
			if (f.argument_options == normal_argument)	
				f.argument_options = short_argument;
			f.vecCount  	= 8;
			f.vec_access	= U16;
			f.useVecDefault = 0;
#else
			f.argument_options = short_argument;
#endif /* __ALTIVEC__ */
			
#if _MSL_C99						/*- mm 030218 -*/
			/* Specifier hh  argument will be of type char */
			if (s[1] == 'h')
			{
				f.argument_options = char_argument;
				c = *++s; /* move past second h */
			}
#endif /* _MSL_C99 */					/*- mm 030218 -*/
			
#ifdef __ALTIVEC__
			/* Specifier hv  argument will be of type vector short */
			if (s[1] == 'v')			/*- mm 030218 -*/ 
			{
				if (f.vec_state != FLAG_OR_Vcode_NOT_OK)
				{
					f.vec_state = FLAG_OR_Vcode_NOT_OK;
					f.argument_options = vector_argument;
					c = *++s;
				}
			}
#endif /* __ALTIVEC__ */
			break;
		
		/* Specifier l  argument will be of type long */
		case 'l':
#ifdef __ALTIVEC__
			if (f.argument_options == normal_argument)	
				f.argument_options = long_argument;
			f.vecCount		= 4;
			f.vec_access	= U32;
			f.useVecDefault = 0;
			/* Specifier lv  argument will be of type vector long */
			if (s[1] == 'v')
			{
				if (f.vec_state != FLAG_OR_Vcode_NOT_OK)
				{
					f.vec_state = FLAG_OR_Vcode_NOT_OK;
					f.argument_options = vector_argument;
					c = *++s;
				}
			}
#else
			f.argument_options = long_argument;
#endif /* __ALTIVEC__ */
			
#if _MSL_C99						/*- mm 030218 -*/
#if _MSL_LONGLONG
			/* Specifier ll  argument will be of type long long */
			if (s[1] == 'l')
			{
				f.argument_options = long_long_argument;
				c = *++s; /* move past second l */
#ifdef __ALTIVEC__
				f.vec_state = FLAG_OR_Vcode_NOT_OK;
#endif /* __ALTIVEC__ */
			}
#endif /* _MSL_LONGLONG */
#endif /* _MSL_C99 */					/*- mm 030218 -*/			
			break;
		
		/* Specifier L  argument will be of type long double */
		case 'L':
			f.argument_options = long_double_argument;
#ifdef __ALTIVEC__
			f.vec_state = FLAG_OR_Vcode_NOT_OK;
#endif /* __ALTIVEC__ */
			break;

/* begin mm 020529 insert */
#if _MSL_C99						/*- mm 030218 -*/
		/* Specifier j  argument will be of type intmax_t */
		case 'j':
			f.argument_options = intmax_argument;
			break;
		
		/* Specifier t  argument will be of type ptrdiff_t */
		case 't':
			f.argument_options = ptrdiff_argument;
			break;
		
		/* Specifier z  argument will be of type size_t */
		case 'z':
			f.argument_options = size_t_argument;
			break;
#endif /* _MSL_C99 */					/*- mm 030218 -*/
/* end mm 020529 insert */
		
		default:
			flag_found = 0;
			break;
	}
	
	if (flag_found)
		c = *++s;
	
#ifdef __ALTIVEC__
	if (f.vec_state == Vcode_REQUIRED && f.argument_options != vector_argument)
	{
		f.conversion_char = bad_conversion;
		*format = f;
		return s;
	}
#endif /* __ALTIVEC__ */
	
	f.conversion_char = c;
	
	switch (c)
	{
		/* CONVERSION diuoxX  argument is either type int (di) or unsigned int (uoxX) */
		case 'd':
		case 'i':
		case 'u':
		case 'o':
		case 'x':
		case 'X':
#ifdef __ALTIVEC__
			if (f.argument_options != vector_argument)
#endif /* __ALTIVEC__ */
			{
				if (f.argument_options == long_double_argument)
				{
					f.conversion_char = bad_conversion;      /*- mm 961219 -*/
					break;                                   /*- mm 961219 -*/
				}
			}
			if (!f.precision_specified)
				f.precision = 1;
			else if (f.justification_options == zero_fill)
				f.justification_options = right_justification;
			break;
		
		/* CONVERSION fF  argument is type double */
		case 'f':
#if _MSL_C99						/*- mm 030218 -*/
		case 'F':					/*- mm 990430 -*/
#endif /* _MSL_C99 */				/*- mm 030218 -*/
#ifdef __ALTIVEC__
			if (f.argument_options != vector_argument)
#endif /* __ALTIVEC__ */
			{
				if (f.argument_options == short_argument
#if _MSL_C99																/*- mm 030304 -*/  
					|| f.argument_options == intmax_argument				/*- mm 020529 -*/
					|| f.argument_options == size_t_argument
					|| f.argument_options == ptrdiff_argument				/*- mm 020529 -*/
#endif /* _MSL_C99 */
#if _MSL_LONGLONG
					|| f.argument_options == long_long_argument
#endif /* _MSL_LONGLONG */
				)  /*  defacto standard %lf is a double */
				{
					f.conversion_char = bad_conversion;
					break;
				}
			}
			
			if (!f.precision_specified)
				f.precision = 6;
			break;

#if _MSL_C99													/*- mm 030314 -*/
		/* CONVERSION aA  argument is type double */
	    case 'a':								
		case 'A':
		#if _MSL_FLOATING_POINT_IO
			if (!f.precision_specified)					
				f.precision = TARGET_FLOAT_MANT_BITS / 4;				
		#endif /* _MSL_FLOATING_POINT_IO */
			if (f.argument_options == short_argument
				|| f.argument_options == intmax_argument		/*- mm 020529 -*/
				|| f.argument_options == size_t_argument
				|| f.argument_options == ptrdiff_argument	 	/*- mm 020529 -*/
#if _MSL_LONGLONG
				|| f.argument_options == long_long_argument
#endif /* _MSL_LONGLONG */
				|| f.argument_options == char_argument)
			{
				f.conversion_char = bad_conversion;
			}
			break;
#endif /* _MSL_C99 */											/*- mm 030314 -*/
		
		/* CONVERSION gG  argument is type double */
	    case 'g':
		case 'G':
			if (!f.precision)
				f.precision = 1;
			/* fall through */
		
		/* CONVERSION eE  argument is type double */
		case 'e':
		case 'E':
#ifdef __ALTIVEC__
			if (f.argument_options != vector_argument)
#endif /* __ALTIVEC__ */
			if (f.argument_options == short_argument
#if _MSL_C99																/*- mm 030304 -*/
				|| f.argument_options == intmax_argument					/*- mm 020529 -*/
				|| f.argument_options == size_t_argument
				|| f.argument_options == ptrdiff_argument 					/*- mm 020529 -*/
#endif /* _MSL_C99 */														/*- mm 030304 -*/
#if _MSL_LONGLONG
				|| f.argument_options == long_long_argument
#endif /* _MSL_LONGLONG */
				|| f.argument_options == char_argument)						/*- mm 990208 -*/
			{
				f.conversion_char = bad_conversion;
				break;
			}
			
			if (!f.precision_specified)
				f.precision = 6;
			break;
		
		/* CONVERSION p  argument is type pointer to (void *) */
		case 'p':
			f.conversion_char  = 'x';
			f.alternate_form   = 1;
			
#ifdef __ALTIVEC__
			/* For AltiVec, precision is 8 with non-vector arguments, 1 with vector arguments */
			if (f.argument_options != vector_argument)
			{
				f.argument_options = long_argument;
				f.precision        = 8;
			} else
				f.precision        = 1;
			
#elif (__dest_os == __m56800_os  || __dest_os == __m56800E_os) && !defined(__m56800E_lmm__)
			/* For dsp56800x, use 16-bit pointers */
    	    f.argument_options = short_argument;
    		f.precision        = 4;
#else
		    
		    /* For everyone else, use precision 8 and long pointers */
		    f.argument_options = long_argument;
    		f.precision        = 8;
#endif /* __ALTIVEC__ */
			break;
		
		/* CONVERSION c  argument is type char (lc = wchar_t) */
		case 'c':
#ifdef __ALTIVEC__
			if (f.argument_options != vector_argument)
#endif /* __ALTIVEC__ */
#if _MSL_WIDE_CHAR				
			if (f.argument_options == long_argument)			/*- mm 990325 -*/
				f.argument_options = wchar_argument;			/*- mm 990325 -*/
			else												/*- mm 990325 -*/
#endif /* _MSL_WIDE_CHAR */										/*- mm 990325 -*/
			if (f.precision_specified || f.argument_options != normal_argument)
				f.conversion_char = bad_conversion;
			break;
		
		/* CONVERSION s  argument is string type (char *) (ls = (wchar_t *) */
		case 's':
#ifdef __ALTIVEC__
			if (f.argument_options == vector_argument)
				f.argument_options = normal_argument;
			else
#endif /* __ALTIVEC__ */
#if _MSL_WIDE_CHAR				
			if (f.argument_options == long_argument)			/*- mm 990325 -*/
				f.argument_options = wchar_argument;			/*- mm 990325 -*/
			else												/*- mm 990325 -*/
#endif /* _MSL_WIDE_CHAR */										/*- mm 990325 -*/
#if __dest_os == __win32_os
			if (f.argument_options == short_argument)
				f.argument_options = normal_argument;			/*- ejs 020218 -*/
			else
#endif /* __dest_os == __win32_os*/
			if (f.argument_options != normal_argument)			/*- mm 990325 -*/
				f.conversion_char = bad_conversion;
			break;
		
		/* CONVERSION n  argument is (int *) where number of written characters is to be placed */
		case 'n':
			if (f.argument_options == long_double_argument)
				f.conversion_char = bad_conversion;         	/*- mm 961219 -*/
			break;
		
/* EJS 021226 */
#if defined(__POWERPC__) && defined(__SPE__)
#if _MSL_FLOATING_POINT_IO
		/* CONVERSION rR  argument is type fractional */
		case 'R':
		case 'r':
			/*	Fractional types */
			/*	Note, the maximum number of digits, assuming 2^b = 10^d, 
				is: 16-bit fraction = 4.8 decimal digits, 
					32-bit fraction = 9.6 decimal digits, 
				and 64-bit fraction = 19.2 decimal digits. */
			if (!f.precision_specified)
				f.precision = 6;
			if (f.argument_options != short_argument
			&&	f.argument_options != normal_argument
			&&	f.argument_options != long_argument)
				f.conversion_char = bad_conversion;
			break;
#endif /* _MSL_FLOATING_POINT_IO */
#endif /* __POWERPC__ && __SPE__ */
		
		default:
#ifdef __ALTIVEC__
			if (f.vec_state == FLAG_OR_Vcode_OK)
			{
				f.vec_state = Vcode_REQUIRED;
				f.vecSeperator = f.c_vecSeperator = (char *) s;
				f.vecSepLen = 1;
				c = *++s;
				goto getFlags;
			}
#endif /* __ALTIVEC__ */
			
			f.conversion_char = bad_conversion;
			break;
	}
	
#ifdef __ALTIVEC__
	f.vecprec  = f.precision;		/* save original prec/width for vec conversions  	*/
	f.vecwidth = f.field_width;
#endif /* __ALTIVEC__ */
	
	*format = f;
	
	return ((const char *) s + 1);
}

#if _MSL_COMPILE_FOR_SPEED
static char * long2str(long num, char * buff, print_format format)
{
	unsigned long	unsigned_num, base;
	char *			p;
	int				n, digits;
	int				minus = 0;
	
	unsigned_num = num;
	minus        =   0;
	
	p = buff;
	
	*--p = 0;
	
	digits = 0;
	
	if (!num && !format.precision && !(format.alternate_form && format.conversion_char == 'o'))
		return (p);
	
	switch (format.conversion_char)
	{
		case 'd':
		case 'i':
			base =  10;
			
			if (num < 0)
			{
				if (num != 0x80000000)									/*- cc 051003 -*/
					unsigned_num = -num;								/*- mm 050104 -*/
				minus        = 1;
			}
			break;
		
		case 'o':
			base =   8;
			
			format.sign_options = only_minus;
			break;
		
		case 'u':
			base =  10;
			
			format.sign_options = only_minus;
			break;
		
		case 'x':
		case 'X':
			base =  16;
			
			format.sign_options = only_minus;
			break;
	}
	
	do
	{
		n = unsigned_num % base;
		
		unsigned_num /= base;
		
		if (n < 10)
			n += '0';
		else
		{
			n -= 10;
			
			if (format.conversion_char == 'x')
				n += 'a';
			else
				n += 'A';
		}
		
		*--p = n;
		
		++digits;
	}
	while (unsigned_num != 0);
	
	if (base == 8 && format.alternate_form && *p != '0')
	{
		*--p = '0';
		++digits;
	}
	
	if (format.justification_options == zero_fill)
	{
		format.precision = format.field_width;
		
		if (minus || format.sign_options != only_minus)
			--format.precision;
		
		if (base == 16 && format.alternate_form)
			format.precision -= 2;
	}
	
	if (buff - p + format.precision > conversion_max)
		return (NULL);
	
	while (digits < format.precision)
	{
		*--p = '0';
		++digits;
	}
	
	if (base == 16 && format.alternate_form)
	{
		*--p = format.conversion_char;
		*--p = '0';
	}
	
	if (minus)
		*--p = '-';
	else if (format.sign_options == sign_always)
		*--p = '+';
	else if (format.sign_options == space_holder)
		*--p = ' ';
	
	return (p);
}

#if _MSL_LONGLONG                          /*- mm 961219 -*/
static char * longlong2str(long long num, char * buff, print_format format)
{
	unsigned long long      unsigned_num, base;
	char *				    p;
	int						n, digits;
	int						minus = 0;
	
	unsigned_num = num;
	minus        =   0;
	
	p = buff;
	
	*--p = 0;
	
	digits = 0;
	
	if (!num && !format.precision && !(format.alternate_form && format.conversion_char == 'o'))
		return (p);
	
	switch (format.conversion_char)
	{
		case 'd':
		case 'i':
			base =  10;
			
			if (num < 0)
			{
				if (num != 0x8000000000000000LL)						/*- mm 050104 -*/
				   unsigned_num = -num;									/*- mm 050104 -*/
				minus        = 1;
			}
			break;
		
		case 'o':
			base =   8;
			
			format.sign_options = only_minus;
			break;
		
		case 'u':
			base =  10;
			
			format.sign_options = only_minus;
			break;
			
		case 'x':
		case 'X':
			base =  16;
			
			format.sign_options = only_minus;
			break;
	}
	
	do
	{
		n = unsigned_num % base;
		
		unsigned_num /= base;
		
		if (n < 10)
			n += '0';
		else
		{
			n -= 10;
			
			if (format.conversion_char == 'x')
				n += 'a';
			else
				n += 'A';
		}
		
		*--p = n;
		
		++digits;
	}
	while (unsigned_num != 0);
	
	if (base == 8 && format.alternate_form && *p != '0')
	{
		*--p = '0';
		++digits;
	}
	
	if (format.justification_options == zero_fill)
	{
		format.precision = format.field_width;
		
		if (minus || format.sign_options != only_minus)
			--format.precision;
		
		if (base == 16 && format.alternate_form)
			format.precision -= 2;
	}
	
	if (buff - p + format.precision > conversion_max)
		return (NULL);
	
	while (digits < format.precision)
	{
		*--p = '0';
		++digits;
	}
	
	if (base == 16 && format.alternate_form)
	{
		*--p = format.conversion_char;
		*--p = '0';
	}
	
	if (minus)
		*--p = '-';
	else if (format.sign_options == sign_always)
		*--p = '+';
	else if (format.sign_options == space_holder)
		*--p = ' ';
	
	return (p);
}
#endif /* _MSL_LONGLONG */

#else /* following case:  _MSL_COMPILE_FOR_SPEED 0 (optimize for size) */

static unsigned int set_base(char conversion_char)
{
	unsigned int base;
	
	switch (conversion_char)
	{
		case 'o':
			base =   8;
			break;
		
		case 'u':
			base =  10;
			break;
		
		case 'x':
		case 'X':
			base =  16;
			break;
	}
	
	return base;
}

static char * justify_conversion(unsigned int base, int digits, int minus, char * p,
	char * buff, print_format format)
{
	if (base == 8 && format.alternate_form && *p != '0')
	{
		*--p = '0';
		++digits;
	}
	
	if (format.justification_options == zero_fill)
	{
		format.precision = format.field_width;
		
		if (minus || format.sign_options != only_minus)
			--format.precision;
		
		if (base == 16 && format.alternate_form)
			format.precision -= 2;
	}
	
	if (buff - p + format.precision > conversion_max)
		return (NULL);
	
	while (digits < format.precision)
	{
		*--p = '0';
		++digits;
	}
	
	if (base == 16 && format.alternate_form)
	{
		*--p = format.conversion_char;
		*--p = '0';
	}
	
	if (minus)
		*--p = '-';
	else if (format.sign_options == sign_always)
		*--p = '+';
	else if (format.sign_options == space_holder)
		*--p = ' ';
	
	return (p);
}

static char * long2str(long num, char * buff, print_format format)
{
	unsigned long	unsigned_num, base;
	char *			p;
	int				n, digits;
	int				minus = 0;
	
	unsigned_num = num;
	minus        =   0;
	
	p = buff;
	
	*--p = 0;
	
	digits = 0;
	
	if (!num && !format.precision && !(format.alternate_form && format.conversion_char == 'o'))
		return (p);
	
	if ((format.conversion_char == 'd') || (format.conversion_char == 'i'))
	{
			base =  10;
			
			if (num < 0)
			{
				if (num != 0x80000000)											/*- mm 050104 -*/
					unsigned_num = -num;										/*- mm 050104 -*/
				minus        = 1;
			}
	}
	else
	{
		base = set_base(format.conversion_char);
		format.sign_options = only_minus;
	}
	
	do
	{
		n = unsigned_num % base;
		
		unsigned_num /= base;
		
		if (n < 10)
			n += '0';
		else
		{
			n -= 10;
			
			if (format.conversion_char == 'x')
				n += 'a';
			else
				n += 'A';
		}
		
		*--p = n;
		
		++digits;
	}
	while (unsigned_num != 0);
	
	return justify_conversion(base, digits, minus, p, buff, format);
}

#if _MSL_LONGLONG                          /*- mm 961219 -*/
static char * longlong2str(long long num, char * buff, print_format format)
{
	unsigned long long      unsigned_num, base;
	char *				    p;
	int						n, digits;
	int						minus = 0;
	
	unsigned_num = num;
	minus        =   0;
	
	p = buff;
	
	*--p = 0;
	
	digits = 0;
	
	if (!num && !format.precision && !(format.alternate_form && format.conversion_char == 'o'))
		return (p);
	
	if ((format.conversion_char == 'd') || (format.conversion_char == 'i'))
	{
			base =  10;
			
			if (num < 0)
			{
				if (num != 0x8000000000000000LL)									/*- mm 050104 -*/
					unsigned_num = -num;											/*- mm 050104 -*/
				minus        = 1;
			}
	}
	else
	{
		base = set_base(format.conversion_char);
		format.sign_options = only_minus;
	}
	
	do
	{
		n = unsigned_num % base;
		
		unsigned_num /= base;
		
		if (n < 10)
			n += '0';
		else
		{
			n -= 10;
			
			if (format.conversion_char == 'x')
				n += 'a';
			else
				n += 'A';
		}
		
		*--p = n;
		
		++digits;
	}
	while (unsigned_num != 0);
	
	return justify_conversion(base, digits, minus, p, buff, format);
}
#endif /* _MSL_LONGLONG */

#endif /* _MSL_COMPILE_FOR_SPEED */

#if _MSL_FLOATING_POINT_IO 
#if _MSL_FLOATING_POINT
/*- mm 990722 -*/ /*/ - mm 000414 -*/ /*- PMK 010703 -*/ /*- ejs 021001 -*/
static char * double2hex(long double num, char * buff, print_format format) 
{										/* num = fp#, buff = ascii representation of fp#, format = formating of the number, i.e. %a */
/*- variables, Common-*/	
	char			*p;					/* p points to char array that will be the ascii representation of fp # */
	unsigned char	*q;					/* q points to fp# */
	unsigned char 	working_byte;		/* temporary space to manipulate the fp# as a character */
	long double		ld;					/* ld is an internal variable, set equal to the floating point number */
	int				expbits, expmask;	/* facts about the exponent representation */
	unsigned		snum;				/* snum is an intermediate value used to calculate the exponent */
	long 			exp;				/* is the exponent of the fp # */
	print_format 	exp_format;
	int  			hex_precision;		/* hex_precision is used as a limit for the loop that writes the ascii fp# */ 
	int				mantissa_bit;		/* bit number stepping through mantissa */
	decform			form;		
	decimal			dec;	
	int				radix_marker;														/*- mm 011130 -*/
	
#if _MSL_C_LOCALE_ONLY	
radix_marker = '.';
#else
radix_marker = 	* (unsigned char *) _MSL_LOCALDATA(__lconv).decimal_point;		/*- mm 011130 -*/
#endif	
	p = buff;							/* p will be the ascii representation of the fp # */
	ld = num;
	
	if (format.precision > conversion_max)						/*- STEP#1: Check if conversion is even going to work -*/
		return(NULL);											/* might as well punt asap */
	form.style  = FLOATDECIMAL;
	form.digits = MAX_SIG_DIG;           
	__num2dec(&form, num, &dec);

#if _MSL_C99
	if (fabsl(num) == 0.0)
#else
	if (fabs(num) == 0.0)
#endif
	{														/*- mm 030422 -*/
		p = buff - 6;										/*- mm 030422 -*/
		strcpy(p, "0x0p0");									/*- mm 030422 -*/
		return(p);											/*- mm 030422 -*/
	}														/*- mm 030422 -*/
	
	if (*dec.sig.text == 'I')
	{			
		if (dec.sgn)
		{                              
			p = buff - 5;              
			if (format.conversion_char == 'A')	
				strcpy(p, "-INF");
			else					
				strcpy(p, "-inf"); 
		}                              
		else                           
		{                              
			p = buff - 4;              
			if (format.conversion_char == 'A')	
				strcpy(p, "INF");						
			else										
				strcpy(p, "inf");     
		}                              
		
		return(p);
	}
	else if (*dec.sig.text == 'N')
	{
		if (dec.sgn)
		{                              
			p = buff - 5;              
			if (format.conversion_char == 'A')	
				strcpy(p, "-NAN");
			else					
				strcpy(p, "-nan"); 
		}                              
		else                           
		{                              
			p = buff - 4;              
			if (format.conversion_char == 'A')	
				strcpy(p, "NAN");						
			else										
				strcpy(p, "nan");     
		}                              
		return(p);
	}

	exp_format.justification_options	= right_justification;/*- STEP4: format -*/
	exp_format.sign_options			= sign_always;
	exp_format.precision_specified   = 0;
	exp_format.alternate_form		= 0;
	exp_format.argument_options		= normal_argument;
	exp_format.field_width			= 0;
	exp_format.precision			= 1;
	exp_format.conversion_char		= 'd';
	
#if _MSL_LITTLE_ENDIAN
	{
		/* convert to big endian for consistent algorithm */
		int i;
		char *nump = (char *)&num;
		for (i = 0; i < TARGET_FLOAT_BYTES/2; i++)
		{
			char x = nump[i];
			nump[i] = nump[TARGET_FLOAT_BYTES-i-1];
			nump[TARGET_FLOAT_BYTES-i-1] = x;
		}
	}
#endif
														/* STEP5: isolate exponent */	
	/* get facts about representation */
	expbits = TARGET_FLOAT_EXP_BITS;
	expmask = (LDBL_MAX_EXP*2)-1;

#if TARGET_FLOAT_EXP_BITS > 32
#error fix for greater-than-32-bit exponents
#endif
	
	/* extract bits into high part of snum */
	snum = ((unsigned char *)&num)[0] << 25;
	if (TARGET_FLOAT_EXP_BITS > 7)
		snum |= ((unsigned char *)&num)[1] << 17;
	if (TARGET_FLOAT_EXP_BITS > 15)
		snum |= ((unsigned char *)&num)[2] << 9;
	if (TARGET_FLOAT_EXP_BITS > 23)
		snum |= ((unsigned char *)&num)[3] << 1;

	/* get the exponent proper, ignoring sign bit */
	snum	= (snum >> (32 - expbits)) & expmask;
	
	/* bias back into real exponent (LDBL_MAX_EXP = 0, LDBL_MAX_EXP+1 = 2^0, LDBL_MAX_EXP-1 = 2^-1) */
	exp		= snum - (TARGET_FLOAT_MAX_EXP-1);
	
	p = long2str(exp, buff, exp_format);
	if (format.conversion_char == 'a')
		*--p = 'p';
	else
		*--p = 'P';
	q = (unsigned char *)&num;

	/* point to end of the mantissa, so that we can emit a nybble at a time
		and end up directly at the j-bit (or where it would be, if it existed) */
	if (TARGET_FLOAT_IMPLICIT_J_BIT)
		mantissa_bit = (1 + expbits + format.precision * 4) - 1;
	else
		mantissa_bit = (1 + expbits + format.precision * 4) - 4;
	
	for (hex_precision = format.precision; hex_precision >= 1; hex_precision--)
	{
		if (mantissa_bit < TARGET_FLOAT_BITS)
		{
			int mantissa_byte;
			
			/* get the nybble */
			mantissa_byte = mantissa_bit >> 3;
			working_byte = (*(q + mantissa_byte)) >> (7 - (mantissa_bit & 7));

			/* nybble crosses a byte */
			if ((mantissa_bit & ~7) != ((mantissa_bit - 4) & ~7))
				working_byte |= ((*(q + (mantissa_byte - 1))) << 8) >> (7 - ((mantissa_bit) & 7));

			/* maybe we need to emit the radix marker? */
			if (!TARGET_FLOAT_IMPLICIT_J_BIT)
			{
				if (mantissa_bit == 1 + expbits)
				{
					*--p = radix_marker;							

					/* there is only one mantissa bit; we read part of the exponent */
					working_byte &= 0x1;
				}
			}
			
			if ((working_byte &= 0x0f) < 10)
				working_byte += '0';
			else
				if (format.conversion_char == 'a')
					working_byte += 'a' - 10;
				else
					working_byte += 'A' - 10;
		}
		else
			working_byte = '0'; 

		*--p = working_byte;
		mantissa_bit -= 4;
	}	
	
	if (TARGET_FLOAT_IMPLICIT_J_BIT)
	{
		if (format.precision || format.alternate_form)		/* STEP#6: emit implicit "1." */
			*--p = radix_marker;							/*- mm 011130 -*/
		*--p = '1'; 
	}
	if (format.conversion_char == 'a')
		*--p = 'x';
	else
		*--p = 'X';
	*--p = '0';
	if (dec.sgn)
		*--p = '-';
	else if (format.sign_options == sign_always)
		*--p = '+';
	else if (format.sign_options == space_holder)
		*--p = ' ';
	return(p); 											/*- STEP#7: return the fp# as ascii -*/
	}

static void round_decimal(decimal * dec, int new_length)
{
	char		c;
	char *	p;
	int			carry;
	
	if (new_length < 0)
	{
return_zero:
		dec->exp         =  0 ;
		dec->sig.length  =  1 ;
		*dec->sig.text   = '0';
		return;
	}
	
	if (new_length >= dec->sig.length)
		return;
	
	p     = (char *) dec->sig.text + new_length + 1;
	c     = *--p - '0';

	if( c == 5 )                                                     	/*- mm 970614 -*/
	{ 																	/*- mm 970614 -*/
		char *q = &((char *)dec->sig.text)[dec->sig.length];           	/*- mm 970614 -*/
	  	while( --q > p && *q == '0' ) 									/*- mm 970614 -*/
	  		;                           								/*- mm 970614 -*/
/* The following change is to fix WB1-24254                             /*- mm 010710 -*/
/*	  	if (new_length == 0)											/*- mm 991102 -*/ /*- mm 010710 -*/
/*	  		carry = 0;													/*- mm 991102 -*/ /*- mm 010710 -*/
/*	  	else															/*- mm 010710 -*/ /*- mm 010710 -*/
	  		carry = ( q == p ) ? p[-1] & 1 : 1;                         /*- mm 970614 -*/
	}                                                                	/*- mm 970614 -*/
	else                                                             	/*- mm 970614 -*/
	  carry = (c > 5);                                               	/*- mm 970614 -*/
	
	while (new_length)
	{
		c = *--p - '0' + carry;
		
		if ((carry = (c > 9)) != 0 || c == 0 ) 							/*- mm 970614 -*/
			--new_length;
		else
		{
			*p = c + '0';
			break;
		}
	}
	
	if (carry)
	{
		dec->exp        +=  1 ;
		dec->sig.length  =  1 ;
		*dec->sig.text   = '1';
		return;
	}
	else if (new_length == 0)
		goto return_zero;
	
	dec->sig.length = new_length;
}

static char * float2str(long double num, char * buff, print_format format)
{
	decimal		dec;
	decform		form;
	char *		p;
	char *		q;
	int				n, digits, sign;
	int				int_digits, frac_digits;
	int             radix_marker;													/*- mm 011130 -*/
	
#if _MSL_C_LOCALE_ONLY	
	radix_marker = '.';
#else
	radix_marker = 	* (unsigned char *) _MSL_LOCALDATA(__lconv).decimal_point;		/*- mm 011130 -*/
#endif
	
	if (format.precision > conversion_max)							/* might as well punt asap */
		return(NULL);

/*
 *	Note: If you look at <ansi_fp.h> you'll see that __num2dec only supports double.
 *				If you look at <float.h> you'll see that long double == double. Ergo, the
 *				difference is moot *until* a truly long double type is supported.
 */
	
	form.style  = FLOATDECIMAL;
	form.digits = MAX_SIG_DIG;           /*- mm 970609 -*/
	
	__num2dec(&form, num, &dec);
	
	p = (char *) dec.sig.text + dec.sig.length;					/* strip off trailing zeroes */
	
	while (dec.sig.length > 1 && *--p == '0')
	{
		--dec.sig.length;
		++dec.exp;
	}
	
	switch (*dec.sig.text)
	{
		case '0':
			
			/* dec.sgn = 0;		*/ /* print correctly signed zero --mf 060298 */
			dec.exp = 0;		/* __num2dec doesn't guarantee */
								/* this for zeroes             */
			break;
		
		case 'I':
			
			if (num < 0)                   /*- mm 970213 -*/
			{                              /*- mm 970213 -*/
				p = buff - 5;              /*- mm 970213 -*/	/* special cases */
				if (isupper(format.conversion_char))		/*- mm 990430 -*/
					strcpy(p, "-INF");						/*- mm 990430 -*/
				else										/*- mm 990430 -*/
					strcpy(p, "-inf");     /*- mm 970213 -*/	/*- mm 990430 -*/
			}                              /*- mm 970213 -*/
			else                           /*- mm 970213 -*/
			{                              /*- mm 970213 -*/
				p = buff - 4;              /*- mm 970213 -*/	/* special cases */
				if (isupper(format.conversion_char))		/*- mm 990430 -*/
					strcpy(p, "INF");						/*- mm 990430 -*/
				else										/*- mm 990430 -*/
					strcpy(p, "inf");     /*- mm 970213 -*/	/*- mm 990430 -*/
			}                              /*- mm 970213 -*/
			
			return(p);
			
		case 'N':
			
			if (dec.sgn)									/*- mm 990816 -*/
			{												/*- mm 990816 -*/
				p = buff - 5;								/*- mm 990816 -*/
				if (isupper(format.conversion_char))		/*- mm 990816 -*/
					strcpy(p, "-NAN");						/*- mm 990816 -*/
				else										/*- mm 990816 -*/
					strcpy(p, "-nan");						/*- mm 990816 -*/
			}												/*- mm 990816 -*/
			else											/*- mm 990816 -*/
			{												/*- mm 990816 -*/
				p = buff - 4;								/*- mm 990816 -*/
				if (isupper(format.conversion_char))		/*- mm 990816 -*/
					strcpy(p, "NAN");						/*- mm 990816 -*/
				else										/*- mm 990816 -*/
					strcpy(p, "nan");						/*- mm 990816 -*/
			}												/*- mm 990816 -*/
			
			return(p);
	}
	
	dec.exp += dec.sig.length - 1;											/* shift decimal point to   */
																			/* follow first digit	    */
	p = buff;
	
	*--p = 0;
	
	switch (format.conversion_char)
	{
		case 'g':
		case 'G':
			
			if (dec.sig.length > format.precision)
				round_decimal(&dec, format.precision);
			
			if (dec.exp < -4 || dec.exp >= format.precision)
			{
				if (format.alternate_form)
					--format.precision;
				else
					format.precision = dec.sig.length - 1;
				
				if (format.conversion_char == 'g')
					format.conversion_char = 'e';
				else
					format.conversion_char = 'E';
				
				goto e_format;
			}
			
			if (format.alternate_form)
				format.precision -= dec.exp + 1;
			else
				if ((format.precision = dec.sig.length - (dec.exp + 1)) < 0)
					format.precision = 0;
			
			goto f_format;
		
		case 'e':
		case 'E':
		e_format:
			
			if (dec.sig.length > format.precision + 1)
				round_decimal(&dec, format.precision + 1);
			
			n    = dec.exp;
			sign = '+';
			
			if (n < 0)
			{
				n    = -n;
				sign = '-';
			}
			
			for (digits = 0; n || digits < 2; ++digits)
			{
				*--p  = n % 10 + '0';
				n    /= 10;
			}
			
			*--p = sign;
			*--p = format.conversion_char;
			
			if (buff - p + format.precision > conversion_max)
				return(NULL);
			
			if (dec.sig.length < format.precision + 1)
				for (n = format.precision + 1 - dec.sig.length + 1; --n;)
					*--p = '0';
			
			for (n = dec.sig.length, q = (char *) dec.sig.text + dec.sig.length; --n;)
				*--p = *--q;
			
			if (format.precision || format.alternate_form)
				*--p = radix_marker;							/*- mm 011130 -*/
			
			*--p = *dec.sig.text;
			
			if (dec.sgn)
				*--p = '-';
			else if (format.sign_options == sign_always)
				*--p = '+';
			else if (format.sign_options == space_holder)
				*--p = ' ';
			
			break;
		
		case 'f':
#if _MSL_C99						/*- mm 030218 -*/
		case 'F':													/*- mm 990430 -*/
#endif /* _MSL_C99	*/					/*- mm 030218 -*/
		f_format:
			
			if ((frac_digits = -dec.exp + dec.sig.length - 1) < 0)
				frac_digits = 0;
			
			if (frac_digits > format.precision)
			{
				round_decimal(&dec, dec.sig.length - (frac_digits - format.precision));
				
				if ((frac_digits = -dec.exp + dec.sig.length - 1) < 0)
					frac_digits = 0;
			}
			
			if ((int_digits = dec.exp + 1) < 0)
				int_digits = 0;
			
			if (int_digits + frac_digits > conversion_max)
				return(NULL);
			
			q = (char *) dec.sig.text + dec.sig.length;
			
			for (digits = 0; digits < (format.precision - frac_digits); ++digits)
				*--p = '0';
			
			for (digits = 0; digits < frac_digits && digits < dec.sig.length; ++digits)
				*--p = *--q;
			
			for (; digits < frac_digits; ++digits)
				*--p = '0';
			
			if (format.precision || format.alternate_form)
				*--p = radix_marker;										/*- mm 011130 -*/
			
			if (int_digits)
			{
#if defined(__m56800__) || \
    defined(__m56800E__)
				for (digits = 0; digits < int_digits - (int)dec.sig.length; ++digits)
					*--p = '0';
#else
				for (digits = 0; digits < int_digits - dec.sig.length; ++digits)
					*--p = '0';
#endif
				
				for (; digits < int_digits; ++digits)
					*--p = *--q;
			}
			else
				*--p = '0';
			
			if (dec.sgn)
				*--p = '-';
			else if (format.sign_options == sign_always)
				*--p = '+';
			else if (format.sign_options == space_holder)
				*--p = ' ';
			
			break;
	}
	
	return(p);
}
#endif /* _MSL_FLOATING_POINT */	/*- cc 010821 -*/
#endif /* _MSL_FLOATING_POINT_IO */

static int __pformatter(void *(_MSL_CDECL *WriteProc)(void *, const char *, size_t), void * WriteProcArg, 
														const char * _MSL_RESTRICT format_str, va_list arg)	/*- mm 990325 -*/ /*- cc 020108 -*/
{
	int				num_chars, chars_written, field_width;
	const char *	format_ptr;
	const char *	curr_format;
	print_format	format;
	long			long_num;
#if _MSL_LONGLONG          /*- mm 961219 -*/
	long long   	long_long_num;       /*- mm 961219 -*/
#endif /* _MSL_LONGLONG */  /*- mm 961219 -*/
#if _MSL_FLOATING_POINT && _MSL_FLOATING_POINT_IO
	long double		long_double_num;
#endif /* _MSL_FLOATING_POINT */
	char			buff[conversion_buff_size];
	char *			buff_ptr;
	char *			string_end;
	char            fill_char = ' ';       /*- mm 960722 -*/
	
#ifdef __ALTIVEC__
	int vecIndex;				/* counts vector elements							*/
	MWVector128 vec_val;
#endif /* __ALTIVEC__ */

	format_ptr    = format_str;
	chars_written = 0;
	
	while (*format_ptr)
	{
		if (!(curr_format = strchr(format_ptr, '%')))
		{
			num_chars      = strlen(format_ptr);
			chars_written += num_chars;
			
			if (num_chars && !(*WriteProc)(WriteProcArg, format_ptr, num_chars))	/*- mm 990325 -*/
				return(-1);
			
			break;
		}
		
		num_chars      = curr_format - format_ptr;
		chars_written += num_chars;
		
		if (num_chars && !(*WriteProc)(WriteProcArg, format_ptr, num_chars))		/*- mm 990325 -*/
			return(-1);
		
		format_ptr = curr_format;
		
#if defined(__PPC_EABI__)				/*- cc 010821 -*/
		format_ptr = parse_format(format_ptr, (va_list *)arg, &format);
#else
		format_ptr = parse_format(format_ptr, (va_list *)&arg, &format);
#endif			

#ifdef __ALTIVEC__
		vecIndex = 0;
		do {				/* can be done repeatedly for each vector field */
#endif /* __ALTIVEC__ */
		
		switch (format.conversion_char)
		{
			case 'd':
			case 'i':
#ifdef __ALTIVEC__
				if (format.argument_options == vector_argument)
				{
					if (vecIndex == 0)
					{
						if (format.useVecDefault)	/* establish default vec access */
						{	
							format.vecCount   = 16;
							format.vec_access = U8;
						}
						#if defined(__VEC__)
							vec_val = va_arg(arg, MWVector128);
						#else
							va_arg_replacement1(arg, vec_val);
						#endif
					}
					switch (format.vec_access)
					{
						case U8:  long_num = vec_val.s8 [vecIndex]; break;
						case U16: long_num = vec_val.s16[vecIndex]; break;
						case U32: long_num = vec_val.s32[vecIndex]; break;
					}
				}
				else
#endif /* __ALTIVEC__ */
				if (format.argument_options == long_argument)
					long_num = va_arg(arg, long);
#if _MSL_LONGLONG                                          /*- mm 961219 -*/
	            else if (format.argument_options == long_long_argument)  /*- mm 961219 -*/
					long_long_num = va_arg(arg, long long);              /*- mm 961219 -*/
#endif /* _MSL_LONGLONG */                                    /*- mm 961219 -*/
#if _MSL_C99															 /*- mm 030304 -*/
				else if (format.argument_options == intmax_argument)	 /*- mm 020529 -*/
#if _MSL_LONGLONG
					long_long_num = va_arg(arg, intmax_t);				 /*- mm 020529 -*/  /*- mm 040811 -*/
#else
					long_num = va_arg(arg, intmax_t);					 /*- mm 020529 -*/
#endif /* _MSL_LONGLONG */
				else if (format.argument_options == size_t_argument)	 /*- mm 020529 -*/
					long_num = va_arg(arg, size_t);					 	 /*- mm 020529 -*/  
				else if (format.argument_options == ptrdiff_argument)	 /*- mm 020529 -*/
					long_num = va_arg(arg, ptrdiff_t);					 /*- mm 020529 -*/
#endif /* _MSL_C99 */													 /*- mm 030304 -*/
				else
					long_num = va_arg(arg, int);
				
				if (format.argument_options == short_argument)
					long_num = (short) long_num;
				if (format.argument_options == char_argument)
					long_num = (signed char) long_num;
					
#if _MSL_LONGLONG    
#if _MSL_C99                                      /*- mm 961219 -*/
                if ((format.argument_options == long_long_argument) ||   /*- mm 961219 -*/ /*- mm 040811 -*/
                		(format.argument_options == intmax_argument))	 /*- mm 040811 -*/
				{														 /*- bb 971019 -*/
				   if (!(buff_ptr = longlong2str(long_long_num,          /*- mm 961219 -*/
				                   buff + conversion_buff_size, format)))/*- mm 961219 -*/
					   goto conversion_error;                            /*- mm 961219 -*/
				}														 /*- bb 971019 -*/
				else 
#endif /* _MSL_C99 */			                                                    /*- mm 961219 -*/
#endif /* _MSL_LONGLONG */                                    /*- mm 961219 -*/
				if (!(buff_ptr = long2str(long_num, buff + conversion_buff_size, format)))
					goto conversion_error;
				
				num_chars = buff + conversion_buff_size - 1 - buff_ptr;
				
				break;
				
			case 'o':
			case 'u':
			case 'x':
			case 'X':
#ifdef __ALTIVEC__
				if (format.argument_options == vector_argument)
				{
					if (vecIndex == 0)
					{
						if (format.useVecDefault)	/* establish default vec access */
						{	
							format.vecCount   = 16;
							format.vec_access = U8;
						}
						#if defined(__VEC__)
							vec_val = va_arg(arg, MWVector128);
						#else
							va_arg_replacement1(arg, vec_val);
						#endif
					}
					switch (format.vec_access)
					{
						case U8:  long_num = vec_val.u8 [vecIndex]; break;
						case U16: long_num = vec_val.u16[vecIndex]; break;
						case U32: long_num = vec_val.u32[vecIndex]; break;
					}
				}
				else
#endif /* __ALTIVEC__ */
				if (format.argument_options == long_argument)
					long_num = va_arg(arg, unsigned long);
#if _MSL_LONGLONG                                          /*- mm 961219 -*/
	            else if (format.argument_options == long_long_argument)  /*- mm 961219 -*/
					long_long_num = va_arg(arg, long long);              /*- mm 961219 -*/
#endif /* _MSL_LONGLONG */                                    /*- mm 961219 -*/
#if _MSL_C99															 /*- mm 030304 -*/
				else if (format.argument_options == intmax_argument)	 /*- mm 020529 -*/
#if _MSL_LONGLONG
					long_long_num = va_arg(arg, intmax_t);				 /*- mm 020529 -*/ /*- mm 040811 -*/
#else
					long_num = va_arg(arg, intmax_t);					 /*- mm 020529 -*/
#endif /* _MSL_LONGLONG */
				else if (format.argument_options == size_t_argument)	 /*- mm 020529 -*/
					long_num = va_arg(arg, size_t);						 /*- mm 020529 -*/
				else if (format.argument_options == ptrdiff_argument)	 /*- mm 020529 -*/
					long_num = va_arg(arg, ptrdiff_t);					 /*- mm 020529 -*/
#endif /* _MSL_C99 */													 /*- mm 030304 -*/
				else
					long_num = va_arg(arg, unsigned int);
				
				if (format.argument_options == short_argument)
					long_num = (unsigned short) long_num;
				if (format.argument_options == char_argument)
					long_num = (unsigned char) long_num;
					
#if _MSL_LONGLONG 
#if _MSL_C99                                         /*- mm 961219 -*/
                if ((format.argument_options == long_long_argument) ||   /*- mm 961219 -*/ /*- mm 040811 -*/
                		(format.argument_options == intmax_argument))	 /*- mm 040811 -*/
                {														 /*- mf 971020 -*/
				   if (!(buff_ptr = longlong2str(long_long_num,          /*- mm 961219 -*/
				                   buff + conversion_buff_size, format)))/*- mm 961219 -*/
					   goto conversion_error;                            /*- mm 961219 -*/
				} 														 /*- mf 971020 -*/
				else   
#endif /* _MSL_C99 */				                                                  /*- mm 961219 -*/
#endif /* _MSL_LONGLONG */                                   /*- mm 961219 -*/
				if (!(buff_ptr = long2str(long_num, buff + conversion_buff_size, format)))
					goto conversion_error;
				
				num_chars = buff + conversion_buff_size - 1 - buff_ptr;
				
				break;

#if _MSL_FLOATING_POINT_IO
#if _MSL_FLOATING_POINT
			case 'f':
#if _MSL_C99						/*- mm 030218 -*/
			case 'F':													/*- mm 990430 -*/
#endif /* _MSL_C99	*/					/*- mm 030218 -*/
			case 'e':
			case 'E':
			case 'g':
			case 'G':
#ifdef __ALTIVEC__
				if (format.argument_options == vector_argument) 
				{
					if (vecIndex == 0) 
					{
						if (format.useVecDefault) 	/* establish default vec access */
						{		
							format.vecCount   = 4;
							format.vec_access = U32;
						}
						#if defined(__VEC__)
							vec_val = va_arg(arg, MWVector128);
						#else
							va_arg_replacement1(arg, vec_val);
						#endif
					}
					switch (format.vec_access)
					{
						case U8:  long_double_num = vec_val.u8 [vecIndex]; break;
						case U16: long_double_num = vec_val.u16[vecIndex]; break;
						case U32: long_double_num = vec_val.f32[vecIndex]; break;
					}
				}
				else
#endif /* __ALTIVEC__ */
				if (format.argument_options == long_double_argument)
					long_double_num = va_arg(arg, long double);
				else
					long_double_num = va_arg(arg, double);
				
				if (!(buff_ptr = float2str(long_double_num, buff + conversion_buff_size, format)))
					goto conversion_error;
				
				num_chars = buff + conversion_buff_size - 1 - buff_ptr;
				break;

#if _MSL_C99
			case 'a':
			case 'A':
				if (format.argument_options == long_double_argument)
					long_double_num = va_arg(arg, long double);
				else
					long_double_num = va_arg(arg, double);
				
				if (!(buff_ptr = double2hex(long_double_num, buff + conversion_buff_size, format)))
					goto conversion_error;
				
				num_chars = buff + conversion_buff_size - 1 - buff_ptr;
				break;
#endif /* _MSL_C99 */							/*- mm 000414 -*/
#endif /* _MSL_FLOATING_POINT */
#endif /* _MSL_FLOATING_POINT_IO */
			
			case 's':
			  #if _MSL_WIDE_CHAR  /*- vss 990624 -*/
				if (format.argument_options == wchar_argument)			/*- mm 990325 -*/
				{														/*- mm 990325 -*/
					wchar_t * wcs_ptr = va_arg(arg, wchar_t*);			/*- mm 990325 -*/
					if (wcs_ptr == NULL)
						wcs_ptr = L"";
					if ((num_chars = wcstombs(buff, wcs_ptr, sizeof(buff))) < 0)
						goto conversion_error;
					buff_ptr = &buff[0];								/*- mm 990325 -*/
				}
				else
			  #endif /* _MSL_WIDE_CHAR */
					buff_ptr = va_arg(arg, char *);
				if (buff_ptr == NULL)    /*- mm 970708 -*/
  					buff_ptr = "";       /*- mm 970708 -*/
				if (format.alternate_form)
				{
					num_chars = (unsigned char) *buff_ptr++;
					
					if (format.precision_specified && num_chars > format.precision)
						num_chars = format.precision;
				}
				else if (format.precision_specified)
				{
					num_chars = format.precision;
					
					if ((string_end = (char *) memchr(buff_ptr, 0, num_chars)) != 0)
						num_chars = string_end - buff_ptr;
				}
				else
					num_chars = strlen(buff_ptr);
				break;
			
			case 'n':
			{
#if defined(__m56800E__)
				int *buff_ptr;
				buff_ptr = va_arg(arg, int *);
#else
				buff_ptr = va_arg(arg, char *);
#endif /* __m56800E__ */				
				
				switch (format.argument_options)
				{
					case normal_argument:    * (int *)       buff_ptr = chars_written; break;
					case short_argument:     * (short *)     buff_ptr = chars_written; break;
					case long_argument:      * (long *)      buff_ptr = chars_written; break;
#if _MSL_C99
					case intmax_argument:    * (intmax_t *)  buff_ptr = chars_written; break;
					case size_t_argument:    * (size_t *) 	 buff_ptr = chars_written; break;
					case ptrdiff_argument:   * (ptrdiff_t *) buff_ptr = chars_written; break;
#endif /* _MSL_C99 */
#if _MSL_LONGLONG                                          /*- mm 961219 -*/
					case long_long_argument: * (long long *) buff_ptr = chars_written; break;
#endif /* _MSL_LONGLONG */                                  /*- mm 961219 -*/
				}
				
				continue;
			}
			
			case 'c':
				buff_ptr = buff;
#ifdef __ALTIVEC__
				if (format.argument_options == vector_argument)
				{
					if (vecIndex == 0) 
					{
						if (format.useVecDefault) 	/* establish default vec access */
						{	
							format.vecCount   = 16;
							format.vec_access = U8;
						}
						if (!format.useSepString)
						{
							format.vecSeperator = format.c_vecSeperator;
							format.vecSepLen    = *format.c_vecSeperator ? 1 : 0;
						}
						#if defined(__VEC__)
							vec_val  = va_arg(arg, MWVector128);
						#else
							va_arg_replacement1(arg, vec_val);
						#endif
					}
					switch (format.vec_access)
					{
						case U8:  *buff_ptr = vec_val.u8 [vecIndex]; break;
						case U16: *buff_ptr = vec_val.u16[vecIndex]; break;
						case U32: *buff_ptr = vec_val.u32[vecIndex]; break;
					}
					
				}
				else
#endif /* __ALTIVEC__ */		
				*buff_ptr = va_arg(arg, int);
				
				num_chars = 1;
				break;
			
/* EJS 021226 */
#if defined(__POWERPC__) && defined(__SPE__)
#if _MSL_FLOATING_POINT_IO
			case 'r':
				/* signed fixed-point */
				if (format.argument_options == short_argument)
					long_double_num = (short)va_arg(arg, long) / (long double)0x8000;
				else if (format.argument_options == normal_argument)
					long_double_num = va_arg(arg, long) / (long double)0x80000000UL;
				else
					long_double_num = va_arg(arg, long long) / (long double)0x8000000000000000ULL;

				format.conversion_char = 'f';
				if (!(buff_ptr = float2str(long_double_num, buff + conversion_buff_size, format)))
					goto conversion_error;
				
				num_chars = buff + conversion_buff_size - 1 - buff_ptr;
				break;
			
			case 'R':
				/* unsigned fixed-point */
				if (format.argument_options == short_argument)
					long_double_num = (unsigned short)va_arg(arg, unsigned long) / (long double)0x10000;
				else if (format.argument_options == normal_argument)
					long_double_num = va_arg(arg, unsigned long) / (long double)0x100000000ULL;
				else
					long_double_num = va_arg(arg, unsigned long long) / (2.0*(long double)0x8000000000000000ULL);

				format.conversion_char = 'f';
				if (!(buff_ptr = float2str(long_double_num, buff + conversion_buff_size, format)))
					goto conversion_error;
				
				num_chars = buff + conversion_buff_size - 1 - buff_ptr;
				break;
#endif /* _MSL_FLOATING_POINT_IO */
#endif /* __POWERPC__ && __SPE__ */

			case '%':
				buff_ptr = buff;
				
				*buff_ptr = '%';
				
				num_chars = 1;
				break;
				
			case bad_conversion:
			default:
			conversion_error:
				num_chars      = strlen(curr_format);
				chars_written += num_chars;
				
				if (num_chars && !(*WriteProc)(WriteProcArg, curr_format, num_chars))	/*- mm 990325 -*/
					return(-1);
				
				return(chars_written);
				break;
		}
		
		field_width = num_chars;
		
		if (format.justification_options != left_justification)
		{                                                                         /*- mm 960722 -*/
			fill_char = (format.justification_options == zero_fill) ? '0' : ' ';  /*- mm 960722 -*/
			if (((*buff_ptr == '+') || (*buff_ptr == '-') || (*buff_ptr == ' ')) 
														&& (fill_char == '0')) 	  /*- mm 970206 -*/ /*- mm 990831 -*/
			{	                                                                  /*- mm 970206 -*/
					if ((*WriteProc)(WriteProcArg, buff_ptr, 1) == 0)			  /*- mm 990325 -*/
					return(-1);                                                   /*- mm 970206 -*/
				++buff_ptr;                                                       /*- mm 970206 -*/
				num_chars--;                                                      /*- mm 970723 -*/
			}
			while (field_width < format.field_width)
			{
				if ((*WriteProc)(WriteProcArg, &fill_char, 1) == 0)  				/*- mm 990325 -*/
					return(-1);
					
				++field_width;
			}
		}                                                                       /*- mm 960722 -*/
		
		if (num_chars && !(*WriteProc)(WriteProcArg, buff_ptr, num_chars))		/*- mm 990325 -*/
			return(-1);
		
		if (format.justification_options == left_justification)
			while (field_width < format.field_width)
			{
				char blank = ' ';
				if ((*WriteProc)(WriteProcArg, &blank, 1) == 0)  				/*- mm 990325 -*/
					return(-1);
					
				++field_width;
			}
		
		chars_written += field_width;

#ifdef __ALTIVEC__
		/* Process vec types specially... */
		
		if (format.argument_options == vector_argument && ++vecIndex < format.vecCount) {
			if (format.vecSeperator && format.vecSepLen > 0) {
				if (format.vecSepLen == 1) 
				{
					if ((*WriteProc)(WriteProcArg, format.vecSeperator, 1) == 0)  /*- mm 990325 -*/
						return (-1);
				} else
					if (!(*WriteProc)(WriteProcArg, format.vecSeperator, format.vecSepLen)) /*- mm 990325 -*/
						return (-1);
				chars_written += format.vecSepLen;
			}
			format.field_width = format.vecwidth;
			format.precision   = format.vecprec;
			fill_char = ' ';
		}
		} while (format.argument_options == vector_argument && vecIndex < format.vecCount);
#endif /* __ALTIVEC__ */
	}
	
	return (chars_written);
}


void * _MSL_CDECL __FileWrite(void *File, const char * Buffer, size_t NumChars) /*- cc 020108 -*/
{
	return (__fwrite(Buffer, 1, NumChars, (FILE *)File) == NumChars ? File : 0);	/*- mm 040428 -*/
}


void * _MSL_CDECL __StringWrite(void * osc, const char * Buffer, size_t NumChars) /*- cc 020108 -*/
{
	size_t CharsToBeWritten;
	void * MemCpyResult;
	__OutStrCtrl * Oscp = (__OutStrCtrl*)osc;
	
	CharsToBeWritten = ((Oscp->CharsWritten + NumChars) <= Oscp->MaxCharCount) ? NumChars : 
															Oscp->MaxCharCount - Oscp->CharsWritten;
	MemCpyResult = (void *)memcpy(Oscp->CharStr + Oscp->CharsWritten, Buffer, CharsToBeWritten);
	Oscp->CharsWritten += CharsToBeWritten;
	/*return(MemCpyResult);*/										/*- mm 000404 -*/
	return((void*)1);												/*- mm 000404 -*/
}


#if (__dest_os != __n64_os)  																/*- ard 990128  -*/			/*- cc 010821 -*/
#if _MSL_CONSOLE_SUPPORT
int _MSL_CDECL printf(const char * _MSL_RESTRICT format, ...)														/*- cc 010821 -*/								
{
	int result;		
										
	#if _MSL_WIDE_CHAR && _MSL_C99				/*- mm 030314 -*/										
		  if (fwide(stdout, -1) >= 0)
		return(-1);
	#endif /* _MSL_WIDE_CHAR && _MSL_C99 */		/*- mm 030314 -*/
	
 	 __begin_critical_region(stdout_access);	/*- mm 020627 -*/												/*- mm 001013 -*/
	
	#if defined(__PPC_EABI__) || defined(__MIPS__)
	{ 
		va_list args;                            
		va_start( args, format );                
		result = __pformatter(&__FileWrite, (void *)stdout, format, args);					/*- mm 001013 -*/    
	}
	#else                                        
		result = __pformatter(&__FileWrite, (void *)stdout, format, __va_start(format));	/*- mm 001013 -*/
	#endif	/* __PPC_EABI__ || __MIPS__  */
	
  	__end_critical_region(stdout_access);				/*- mm 001013 -*//*- mm 020627 -*/	
  			
    return(result);										/*- mm 001013 -*/
}
#endif /* _MSL_CONSOLE_SUPPORT */
#endif /* __dest_os != __n64_os */


#if defined(__m56800E__)

void * __ConsoleWrite(void *File, const char * Buffer, size_t NumChars);

int printf_console(const char * format, ...)										
{
	int result;											
	result = __pformatter(&__ConsoleWrite, (void *)0, format, __va_start(format));
    return(result);
}

#endif


#if _MSL_OS_DISK_FILE_SUPPORT
int _MSL_CDECL fprintf(FILE * _MSL_RESTRICT file, const char * _MSL_RESTRICT format, ...)			/*- cc 010821 -*/
{
	int result;								/*- mm 001013 -*/
	int crtrgn;								/*- mm 020627 -*/

#if _MSL_WIDE_CHAR && _MSL_C99				/*- mm 030314 -*/						
	if (fwide(file, -1) >= 0)
		return(-1);
#endif /* _MSL_WIDE_CHAR && _MSL_C99 */ 	/*- mm 030314 -*/
	if (file == stdout)						/*- mm 020627 -*/
		crtrgn = stdout_access;				/*- mm 020627 -*/
	else if (file == stderr)				/*- mm 020627 -*/
		crtrgn = stderr_access;				/*- mm 020627 -*/
	else									/*- mm 020627 -*/
		crtrgn = files_access;				/*- mm 020627 -*/
  	__begin_critical_region(crtrgn);		 /*- mm 001013 -*/			/*- mm 020627 -*/

#if defined(__PPC_EABI__) || defined(__MIPS__)
	{
	va_list args;                            
                 
	va_start( args, format );                
	result = __pformatter(&__FileWrite, (void *)file, format, args);/*- mm 001013 -*/    
	}
#else                                        
	result = __pformatter(&__FileWrite, (void *)file, format, __va_start(format));/*- mm 001013 -*/
#endif	/* __PPC_EABI__ || __MIPS__ */

	__end_critical_region(crtrgn);				/*- mm 001013 -*//*- mm 020627 -*/

	return(result);										/* -mm 001013 -*/
}
#endif /* _MSL_OS_DISK_FILE_SUPPORT */


int _MSL_CDECL vprintf(const char * _MSL_RESTRICT format, va_list arg)
{
	int retval;											/*- mm 001013 -*/
#if _MSL_WIDE_CHAR && _MSL_C99							/*- mm 030314 -*/								
	if (fwide(stdout, -1) >= 0)
		return(-1);
#endif /* _MSL_WIDE_CHAR && _MSL_C99 */					/*- mm 030314 -*/
  	__begin_critical_region(stdout_access);		 		/*- mm 001013 -*/	/*- mm 020627 -*/
	retval = __pformatter(&__FileWrite, (void *)stdout, format, arg);		/*- mm 001013 -*/
	__end_critical_region(stdout_access);				/*- mm 001013 -*/	/*- mm 020627 -*/
	return(retval);										/*- mm 001013 -*/
}


int _MSL_CDECL vfprintf(FILE * _MSL_RESTRICT file, const char * _MSL_RESTRICT format, va_list arg)
{
	int retval;											/*- mm 001013 -*/
	int crtrgn;											/*- mm 020627 -*/
#if _MSL_WIDE_CHAR && _MSL_C99							/*- mm 030314 -*/					
	if (fwide(file, -1) >= 0)
		return(-1);
#endif /* _MSL_WIDE_CHAR && _MSL_C99 */					/*- mm 030314 -*/                                 

	if (file == stdout)						/*- mm 020627 -*/
		crtrgn = stdout_access;				/*- mm 020627 -*/
	else if (file == stderr)				/*- mm 020627 -*/
		crtrgn = stderr_access;				/*- mm 020627 -*/
	else									/*- mm 020627 -*/
		crtrgn = files_access;				/*- mm 020627 -*/
  	__begin_critical_region(crtrgn);		 /*- mm 001013 -*/			/*- mm 020627 -*/
	retval = __pformatter(&__FileWrite, (void *)file, format, arg);/*- mm 001013 -*/
	__end_critical_region(crtrgn);				/*- mm 001013 -*//*- mm 020627 -*/
	return(retval);										/*- mm 001013 -*/
}


int _MSL_CDECL vsnprintf(char * _MSL_RESTRICT s, size_t n, const char * _MSL_RESTRICT format, va_list arg)
{
	int		end;
	__OutStrCtrl osc;
	
	osc.CharStr      = s;
	osc.MaxCharCount = n;
	osc.CharsWritten = 0;
	
	end = __pformatter(&__StringWrite, &osc, format, arg);   					              
	
	if (s)													/*- mm 000404 -*/
		if (end < n)										/*- mm 030924 -*/
			s[end] = '\0';									/*- mm 030924 -*/
	else													/*- mm 030924 -*/
	   if (n > 0)											/*- mm 030924 -*/
		    s[n - 1] = '\0';								/*- mm 030924 -*/
	
	return(end);
}


int _MSL_CDECL vsprintf(char * _MSL_RESTRICT s, const char * _MSL_RESTRICT format, va_list arg)
{
	return(vsnprintf(s, ULONG_MAX, format, arg));			
}


int _MSL_CDECL snprintf(char * _MSL_RESTRICT s, size_t n, const char * _MSL_RESTRICT format, ...)	
{
#if defined(__PPC_EABI__) || defined(__MIPS__)									/*- cc 010821 -*/
{
	va_list args;                        
	va_start( args, format );            
	return(vsnprintf(s, n, format, args));					
} 
#else                                    
	return(vsnprintf(s, n, format, __va_start(format)));	
#endif /* __PPC_EABI__ || __MIPS__ */
}


#if (__dest_os != __n64_os)  							/*- ard 990126 -*/	/*- cc 010821 -*/
int _MSL_CDECL sprintf(char * _MSL_RESTRICT s, const char * _MSL_RESTRICT format, ...)
{
  #if defined(__PPC_EABI__) || defined(__MIPS__)
  	{ 														/*- cc 010821 -*/
		va_list args;                        
	
		va_start( args, format );            
		return(vsnprintf(s, ULONG_MAX, format, args));		
	}														/*- cc 010821 -*/
  #else                                    
	return(vsnprintf(s, ULONG_MAX, format, __va_start(format)));	
  #endif /* __PPC_EABI__ || __MIPS__ */
}
#endif /* __dest_os != __n64_os */

/* Change record:
 * JFH 950918 First code release.
 * JFH 950929 Fixed bug in 'f' format where a zero fractional part would be suppressed
 *			  regardless of the precision.
 * JFH 951010 Changed long2str to comply with Standard saying: "The result of converting
 *			  a zero value with a precision of zero is no characters."
 * JFH 951030 Corrected handling of 'g','G' conversions using the alternate form (#).
 * JFH 960222 Added casts from (void *) for C++ compatibility.
 * JFH 960219 Added #ifndef around area that handles floating point output. If disabled, you
 *			  can get away with not including MathLib just because you wanted to use printf.
 *			  There is also some ANSI lib code savings as well.
 * JFH 960907 Fixed bug in round_decimal where it *wasn't* rounding anything if all the
 *			  precision digits were zero.
 * JFH 960919 Changed handling of '+' flag so it doesn't affect how unsigned values (i.e.,
 *			  o,u,x,X) are displayed.
 * JFH 960924 Changed "round_decimal" to do rounding using more conventional rules of
 *			  arithmetic. :-)
 * MM  960722 Fill with zeros when required.
 * bkoz960829 add l.you's changes for powertv, defines for no floating point
 * mm  960930 Removed bitfields from printformat structure to compile with ANSI strict
 * mm  961220 Support for long long.
 * mani970101 Don't choke if a NULL string is passed to %s. Just print "(null)".
 * mm  970206 Corrected position of sign and left zero fill
 * mm  970213 Made to print -Inf for negative infinite values
 * mm  970609 Set the max number of significant digits tolarger than DBL_DIG
 * mm  970614 Modifications to remove trailing blanks for g format and to round to nearest or even
 * mm  970702 Modified Mani's fix to conform to Standard
 * mm  970708 Inserted Be changes
 * SCM 970709 Converted __va_start() to va_start() for PPC_EABI
 * mm  970723 Further correction to change of 970206  BW01232
 * beb 971019 Cleaned up confusing nested if statements
 * mf  971020 found another bad if block under long long support
 * FS  980113 Converted __va_start() to va_start() for MIPS_bare
 * mm  980206 Added wide-character support
 * BLC 980301 added vsnprintf, snprintf functions
 * BLC 980309 added format string support for 'll' and 'hh' type mods from C9X
 * MEA 980320 Changed the call to parse_format for PPC_EABI
 * mf  060298 should print 0 with correct sign(i.e. -0 when the number is minus zero(0x80000000)
 * mm  980715 corrected placement of null char in result of vsnprintf MW07942
 * ad  990128 guarded printf() and sprintf() with if __dest_os != __n64_os
 * vss 990203 Add AltiVec changes (contributed)
 * mm  990208 Change to make %lE, %le, %lG, and %lg be accepted but ignore the l (C9x Clause 7.13.6.1 para #3)
 * mm  990317 ReCorrected placement of null char in result of vsnprintf to match C9x (IL9903-1264)
 * mm  990325 Modified to make split between string functions andfile i/o
 * mm  990430 Added code for %F in accordance with C9x
 * mm  990430 Brought printing of inf into line with C9x
 * vss 990624 Added NO_WIDE_CHAR wrappers
 * mm  990430 Brought printing of nan into line with C9x
 * mm  990817 Deleted include of <string_io.h>
 * mm  990722 Added code for %a %A
 * mm  990722 Brought printing of NaN into line with C9x
 * mm  990831 Move blank where there is zero fill and a blank sign space holder IL9908-4231
 * mm  990901 Print correct sign for zero.  IL9908-4232
 * mm  991102 Corrected fix of 970614
 * mm  000404 Corrected problem with passing null string to snprintf  WB1-12370
 * mm  000414 Excluded %a %A from 68k because algorithm not correct for it
 * mm  001013 Threadsafety changes for fprintf, printf, vfprintf, vprintf
 * PMK 010703 Double2Hex( ) function will now work for little endian (X-86) architecture.
 * mm  010710 Corrected rounding to fix WB1-24254
 * cc  010821 Added EPPC to core
 * mm  011130 Changes for implementation of locale
 * cc  011203 Added _MSL_CDECL for new name mangling
 * JWW 020130 Changed _MWMT to _MSL_THREADSAFE for consistency's sake
 * ejs 020218 Support MS extension for %hs == %s
 * mm  020529 Support for C99 length modifiers j, z, and t.
 * JWW 020604 Added _MSL_LOCALDATA macro to make multithread code easier to read
 * mm  020627 Changes to prevent io on stdin, stdout, stderr and general files from interfering with each other
 * JWW 020917 Added _MSL_RESTRICT to get the restrict type specifier for certain C99 functions
 * ejs 021002 Fix little-endian and portability bugs in doubletohex()
 * cc  021004 Added _MSL_C_LOCALE_ONLY
 * ejs 021009 Make hex float printing really independent of 64-bit double format
 * JWW 021031 Made thread local data available for Mach-O
 * ejs 021226 Added %hR,%R,%lR,%hr,%r,%lr for e500 SPE PIM
 * mm  030218 Added some C99 wrappers
 * JWW 030224 Changed __MSL_LONGLONG_SUPPORT__ flag into the new more configurable _MSL_LONGLONG
 * JWW 030224 Changed __NO_WIDE_CHAR flag into the new more configurable _MSL_WIDE_CHAR
 * mm  030304 Added some more C99 wrappers
 * mm  030314 And more _MSL_C99 wrappers
 * mm  030422 Modifications to print the value zero under %a and %A with a zero exponent for C99
 * ejs 030627 Fix bug in %hh specifier with signed argument (use "signed char", not "char" conversion)
 * mm  030924 Fix to correct action of vsnprintf() invoked with n=0.  Thanks to Bogdan Marinescu
 * cc  040217 Changed _No_Floating_Point to _MSL_FLOATING_POINT
 * cc  040217 Changed _No_Disk_File_OS_Support to _MSL_OS_DISK_FILE_SUPPORT
 * mm  040428 Change to call __fwrite instead of fwrite to avoid nested critical regions.
 * mm  040811 Changes to correct handling arguments of type intmax_t  MTWX14449
 * mm  050104 Changes to correct handling of long and longlong arguments that are close to zero.
 * cc  051003 Changed long2str longlong constant to long
 */