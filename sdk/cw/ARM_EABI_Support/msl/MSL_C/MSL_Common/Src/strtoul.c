/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:34:18 $
 * $Revision: 1.32.2.2 $
 */

/*
 *	Routines
 *	--------
 *		__strtoul
 *
 *	Implementation
 *	--------------
 *			
 *		The string scanner is implemented as an extended Finite State Machine.
 *		A state diagram for it can be found in an accompanying TeachText
 *		document, 'strtoul syntax' (too bad pictures can't be imbedded in
 *		comments) in the "MSL Technical Notes" directory. A textual description 
 *      of it follows.
 *			
 *		The state transition loop dispatches to the appropriate code for the
 *		current state, while simultaneously watching for terminating
 *		conditions (field width exhausted, EOF encountered, final state
 *		reached).
 *			
 *		start
 *			
 *			Skip leading spaces. Process optional sign. Once a non-space is seen,
 *			trasition to next state.
 *			
 *		check_for_zero
 *			
 *			If 'base' is zero, then the base is either octal if there is a leading
 *			'0', hexadecimal if there is a leading '0X', or decimal if there is no
 *			leading zero at all. Also, if 'base' is 16, the leading '0X' may be
 *			optionally preset.
 *			
 *			If 'base' is zero or 16, check for a leading zero. If there one,
 *			transition to the next state. Otherwise, skip to state 'need_digit' to
 *			look for at least one digit (here a 'digit' is any alphanumeric; which
 *			'digits' are actually valid depends on the determined number base).
 *			
 *		leading_zero
 *			
 *			Having seen a leading zero, look for a subsequent 'X' (or 'x'). If
 *			present, the base is 16: transition to state 'need_digit' to look for
 *			at least one digit. If 'base' is zero, then the base must be 8.
 *			Transition to the state 'digit_loop' to scan the rest of the number.
 *			
 *		need_digit
 *		digit_loop
 *			
 *			The actions of these two states are identical. However, 'digit_loop'
 *			is a final state, whereas 'need_digit' is not (i.e., if scanning
 *			terminates in 'need_digit', the scan is invalid.
 *			
 *			If 'base' is still zero at this point, then the base must be 10.
 *			
 *			Loop on valid digits for the established base in this state,
 *			accumulating the corresponding numerical value. Keep on the lookout
 *			for overflow.
 *			
 *		finished
 *		failure
 *			
 *			These states are forced upon seeing the end of the numeric string in
 *			the combined need_digit/digit_loop state to signal whether the
 *			termination was valid or not.
 *
 *
 */

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <strtoul.h>

enum scan_states 
{
	start           = 0x01,
	check_for_zero	= 0x02,
	leading_zero    = 0x04,
	need_digit      = 0x08,
	digit_loop      = 0x10,
	finished        = 0x20,
	failure         = 0x40
};

#define final_state(scan_state)	(scan_state & (finished | failure))

#define success(scan_state) (scan_state & (leading_zero | digit_loop | finished))

#define fetch()		(count++, (*ReadProc)(ReadProcArg, 0, __GetAChar))		/*- mm 990325 -*/
#define unfetch(c)	(*ReadProc)(ReadProcArg, c, __UngetAChar)				/*- mm 990325 -*/


unsigned long _MSL_CDECL __strtoul(int			base, 
						int			max_width,
						int (_MSL_CDECL *ReadProc)(void *, int, int), 					/*- mm 990325 -*/
						void * ReadProcArg,									/*- mm 990325 -*/
						int		* chars_scanned,
						int		* negative,
						int		* overflow)
{
	int				scan_state = start;
	int				count      = 0;
	int             spaces     = 0;											/*- mm 010423 -*/
	unsigned long	value      = 0;
	unsigned long	value_max  = 0;
	int				c;
	
	*negative = *overflow = 0;
	
	if (base < 0 || base == 1 || base > 36 || max_width < 1)
		scan_state = failure;
	else
		c = fetch();
	
	if (base)
		value_max	= ULONG_MAX / base;
	
	while (count <= max_width && c != EOF && !final_state(scan_state)) 
	{
		switch (scan_state)
		{
			case start:
			
				if (isspace(c))
				{
					c = fetch();
					count--;						/*- mm 010423 -*/
					spaces++;						/*- mm 010423 -*/
					break;
				}
				
				if (c == '+')
					c = fetch();
				else if (c == '-')
				{
					c = fetch();
					*negative = 1;
				}
				
				scan_state = check_for_zero;
				
				break;
			
			case check_for_zero:
				
				if (base == 0 || base == 16)
				{
					if (c == '0')
					{
						scan_state = leading_zero;
						
						c = fetch();
						
						break;
					}
				}
				
				scan_state = need_digit;
				
				break;
			
			case leading_zero:
				
				if (c == 'X' || c == 'x')
				{
					base = 16;
					
					scan_state = need_digit;
					
					c = fetch();
					
					break;
				}
				
				if (base == 0)
					base = 8;
				
				scan_state = digit_loop;
				
				break;
			
			case need_digit:
			case digit_loop:
				
				if (base == 0)
					base = 10;
				
				if (!value_max)
					value_max	= ULONG_MAX / base;
				
				if (isdigit(c))
				{
					if ((c -= '0') >= base)
					{
						if (scan_state == digit_loop)
							scan_state = finished;
						else
							scan_state = failure;
						
						c += '0';
						
						break;
					}
				}
				else if (!isalpha(c) || (toupper(c) - 'A' + 10) >= base)
				{
					if (scan_state == digit_loop)
						scan_state = finished;
					else
						scan_state = failure;
					
					break;
				}
				else
					c = toupper(c) - 'A' + 10;
				
				if (value > value_max)
					*overflow = 1;
				
				value *= base;
				
				if (c > (ULONG_MAX - value))
					*overflow = 1;
				
				value += c;
				
				scan_state = digit_loop;	/* In case we entered in state need_digit */
				
				c = fetch();
				
				break;				
			
		}
	}
	
	if (!success(scan_state))
	{
		count = 0;
		value = 0;
		*chars_scanned = 0;
	}
	else
	{																/*- mm 010504 -*/
		count--;
		*chars_scanned = count + spaces;							/*- mm 010423 -*/
	}																/*- mm 010504 -*/
	
	unfetch(c);
	
	return(value);
}


#if (_MSL_LONGLONG && _MSL_C99)  					/*- mm 970110 -*/ /*- mm 030225 -*/

unsigned long long _MSL_CDECL __strtoull(int		  base, 
								int		  max_width,
								int (_MSL_CDECL *ReadProc)(void *, int, int), 	/*- mm 990325 -*/
								void * ReadProcArg,					/*- mm 990325 -*/
								int		* chars_scanned,
								int		* negative,
								int		* overflow)
{
	int				   scan_state = start;
	int				   count      = 0;
	int                spaces     = 0;								/*- mm 010423 -*/
	unsigned long long value      = 0;
	unsigned long long value_max  = 0;
	unsigned long long ullmax     = ULLONG_MAX;      				/*- mm 970102 -*/ /*- hh 980108 -*/
	int				   c;
	
	*negative = *overflow = 0;
	
	if (base < 0 || base == 1 || base > 36 || max_width < 1)
		scan_state = failure;
	else
		c = fetch();
	
	if (base)
		value_max	= ullmax / base;                  				/*- mm 970102 -*/
	
	while (count <= max_width && c != EOF && !final_state(scan_state)) 
	{
		switch (scan_state)
		{
			case start:
			
				if (isspace(c))
				{
					c = fetch();
					count--;						/*- mm 010423 -*/
					spaces++;						/*- mm 010423 -*/
					break;
				}
				
				if (c == '+')
					c = fetch();
				else if (c == '-')
				{
					c = fetch();
					
					*negative = 1;
				}
				
				scan_state = check_for_zero;
				
				break;
			
			case check_for_zero:
				
				if (base == 0 || base == 16)
				{
					if (c == '0')
					{
						scan_state = leading_zero;
						
						c = fetch();
						
						break;
					}
				}
				
				scan_state = need_digit;
				
				break;
			
			case leading_zero:
				
				if (c == 'X' || c == 'x')
				{
					base = 16;
					
					scan_state = need_digit;
					
					c = fetch();
					
					break;
				}
				
				if (base == 0)
					base = 8;
				
				scan_state = digit_loop;
				
				break;
			
			case need_digit:
			case digit_loop:
				
				if (base == 0)
					base = 10;
				
				if (!value_max)
					value_max	= ullmax / base;             /*- mm 970102 -*/
				
				if (isdigit(c))
				{
					if ((c -= '0') >= base)
					{
						if (scan_state == digit_loop)
							scan_state = finished;
						else
							scan_state = failure;
						
						c += '0';
						
						break;
					}
				}
				else if (!isalpha(c) || (toupper(c) - 'A' + 10) >= base)
				{
					if (scan_state == digit_loop)
						scan_state = finished;
					else
						scan_state = failure;
					
					break;
				}
				else
					c = toupper(c) - 'A' + 10;
				
				if (value > value_max)
					*overflow = 1;
				
				value *= base;
				
				if (c > (ullmax - value))            /*- mm 970102 -*/
					*overflow = 1;
				
				value += c;
				
				scan_state = digit_loop;	/* In case we entered in state need_digit */
				
				c = fetch();
				
				break;				
			
		}
	}
	
	if (!success(scan_state))
	{
		count = 0;
		value = 0;
		*chars_scanned = 0;
	}
	else
	{																/*- mm 010504 -*/
		count--;
		*chars_scanned = count + spaces;							/*- mm 010423 -*/
	}																/*- mm 010504 -*/
	
	unfetch(c);
	
	return(value);
}
#endif  /* _MSL_LONGLONG    */                               /*- mm 970110 -*/

unsigned long _MSL_CDECL strtoul(const char * _MSL_RESTRICT str, char ** _MSL_RESTRICT end, int base)
{
	unsigned long		value;
	int					count, negative, overflow;
	
	__InStrCtrl isc;
	isc.NextChar         = (char *)str;
	isc.NullCharDetected = 0;
	
	value = __strtoul(base, INT_MAX, &__StringRead, (void *)&isc, &count, &negative, &overflow);
	
	if (end)
		*end = (char *) str + count;
	
	if (overflow)
	{
		value = ULONG_MAX;
		errno = ERANGE;
	} 
	else if (negative)
		value = -value;
	
	return(value);
}


#if (_MSL_LONGLONG && _MSL_C99)                  /*- mm 970110 -*/  /*- mm 030225 -*/
unsigned long long _MSL_CDECL strtoull(const char * _MSL_RESTRICT str, char ** _MSL_RESTRICT end, int base)
{
	unsigned long long	value;
	int					count, negative, overflow;
	
	__InStrCtrl isc;
	isc.NextChar         = (char *)str;
	isc.NullCharDetected = 0;
	
	value = __strtoull(base, INT_MAX,  &__StringRead, (void *)&isc, &count, &negative, &overflow);
	
	if (end)
		*end = (char *) str + count;
	
	if (overflow)
	{
		value = ULLONG_MAX;                          /*- mm 970102 -*/ /*- hh 980108 -*/
		errno = ERANGE;
	} 
	else if (negative)
		value = -value;
	
	return(value);
}
#endif       /*_MSL_LONGLONG*/                /*- mm 970110 -*/

long _MSL_CDECL strtol(const char * _MSL_RESTRICT str, char ** _MSL_RESTRICT end, int base)
{
	unsigned long	uvalue;
	long			svalue;
	int				count, negative, overflow;
	
	__InStrCtrl isc;
	isc.NextChar         = (char *)str;
	isc.NullCharDetected = 0;
		
	uvalue = __strtoul(base, INT_MAX, &__StringRead, (void *)&isc, &count, &negative, &overflow);
	
	if (end)
		*end = (char *) str + count;
	
	if (overflow || (!negative && uvalue > LONG_MAX) || (negative && uvalue > -LONG_MIN))
	{
		svalue = (negative ? -LONG_MIN : LONG_MAX);
		errno = ERANGE;
	} else
		svalue = (negative ? (long)-uvalue : (long)uvalue);
	
	return(svalue);
}

#if (_MSL_LONGLONG && LLONG_MAX)                  /*- mm 970110 -*/  /*- mm 030225 -*/
long long _MSL_CDECL strtoll(const char * _MSL_RESTRICT str, char ** _MSL_RESTRICT end, int base)
{
	unsigned long long	uvalue;
	long long 			svalue;
	int			    	count, negative, overflow;
	
	__InStrCtrl isc;
	isc.NextChar         = (char *)str;
	isc.NullCharDetected = 0;
	
	uvalue = __strtoull(base, INT_MAX, &__StringRead, (void *)&isc, &count, &negative, &overflow);
	
	if (end)
		*end = (char *) str + count;
	
	if (overflow || (!negative && uvalue > LLONG_MAX) ||               /*- mm 970102 -*/ /*- hh 980108 -*/
	                            (negative && uvalue > -LLONG_MIN))     /*- mm 970102 -*/ /*- hh 980108 -*/
	{
		svalue = (negative ? -LLONG_MIN : LLONG_MAX);               /*- mm 970102 -*/ /*- hh 980108 -*/ /*- hh 980108 -*/
		errno = ERANGE;
	} else
		svalue = (negative ? (long long)-uvalue : (long long)uvalue);
	
	return(svalue);
}
#endif      /*_MSL_LONGLONG*/                                  /*- mm 970110 -*/

#if __dest_os != __palm_os

int _MSL_CDECL atoi(const char * str)
{
	return(strtol(str, NULL, 10));
}

long _MSL_CDECL atol(const char * str)
{
	return(strtol(str, NULL, 10));
}

#if _MSL_C99
#if _MSL_LONGLONG               											/*- cc 021209 -*/
long long _MSL_CDECL atoll(const char * str)
{
	return(strtoll(str, NULL, 10));
}
#endif
#endif /* _MSL_C99 */ 

#endif /* __dest_os != __palm_os */

/* Change record:
 * JFH 950616 First code release.
 * JFH 950727 Added code to ensure that *digits* as well as letters are within the acceptable
 *								 range for the numeric base.
 * JFH 951127 Merged relevant portions of strconv.c into this file.
 * JFH 960425 Fixed a bug where a char was pushed back into input stream at the end of
 *								 '__strtoul' after it was already converted to a binary digit.
 * mm  961220 Added code to support long long
 * mm  970102 Repaired strtoll and strtoull
 * mm  970110 Changed wrappers for msl long long support
 * mm  980720 Added support for inttypes.h
 * mm  981023 added wrappers round long long support
 * mm  981029 provided non long long support for strtoimax and strtoumax
 * mm  990325 Changes to allow use of input functions passed by pointer
 * mm  990817 Deleted include of <string_io.h
 * mm  000515 Removed definitions of strtoimax and strtoumax, now covered in cinttypes
 * mm  010423 Correction to prevent leading white space counting in field width in strtoul and strtoull
 * mm  010506 Correction to mm 010423 to make sure that in the event of failure, chars_scanned is set to zero.
 * cc  011203 Added _MSL_CDECL for new name mangling 
 * BLC 020829 implicit conversion errors fixes, atoi/atol not defined for Palm OS
 * JWW 020917 Added _MSL_RESTRICT to get the restrict type specifier for certain C99 functions
 * cc  021209 Added atoll
 * JWW 030224 Changed __MSL_LONGLONG_SUPPORT__ flag into the new more configurable _MSL_LONGLONG
 * mm  030225 Added C99 wrappers for LLONG_MAX and ULLONG_MAX
 * ejs 030723 Fix incorrect typecast in strtoll()
 */