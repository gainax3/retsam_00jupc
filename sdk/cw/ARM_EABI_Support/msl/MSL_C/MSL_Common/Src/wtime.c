/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:34:37 $
 * $Revision: 1.49.2.2 $
 */

/*
 *	Routines
 *	--------
 *		wcsftime
 */

#pragma ANSI_strict off  			/*- vss 990729 -*/

#include <ansi_parms.h>

#if _MSL_OS_TIME_SUPPORT
#if _MSL_C99					/*- mm 030304 -*/
#if _MSL_WIDE_CHAR				/*- mm 980204 -*/

#pragma ANSI_strict reset

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>
#include <wtime.h>					/*- mm 990809 -*/

#include <locale_api.h>
#include <msl_thread_local_data.h>
#include <time_api.h>				/*- mm 021220 -*/

static int _MSL_CDECL wemit(wchar_t * str, size_t size, size_t * max_size, const wchar_t * format_str, ...)
{
#if (defined(__PPC_EABI__) || defined(__MIPS__))
	va_list args;                            						/*- scm 970709 -*/	
	va_start( args, format_str );            						/*- scm 970709 -*/
#endif                                      						/*- scm 970709 -*/

	if (size > *max_size)
		return(0);

	*max_size -= size;

#if (defined(__PPC_EABI__) || defined(__MIPS__))
	return(vswprintf(str, size+1,  format_str, args)); 		/*- scm 970709 -*/	/*- mm 990322 -*/
#else                                        
	return(vswprintf(str, size+1, format_str, __va_start(format_str)));			/*- mm 990322 -*/
#endif                                       
}

size_t _MSL_CDECL wcsftime(wchar_t * _MSL_RESTRICT str, size_t max_size, const wchar_t * _MSL_RESTRICT format_str, 
																						const struct tm * _MSL_RESTRICT timeptr)
{
	struct tm			tm;
	const struct tm		default_tm = {0, 0, 0, 1, 0, 0, 1, 0, -1};
	size_t 				num_chars, chars_written, space_remaining;
	const wchar_t *		format_ptr;
	const wchar_t *		curr_format;
	int					n;
#if _MSL_FLOATING_POINT
	int 				ISO8601Year, ISO8601WeekNo;
#endif
	char				temp_string[32];
	wchar_t				wtemp_string[32];
	struct __locale *	current_locale_ptr;
	size_t				char_cnt;
	const char *		charptr;							

	current_locale_ptr = &_MSL_LOCALDATA(_current_locale);				
	
	if ((space_remaining = --max_size) <= 0)
		return(0);

	tm = default_tm;

	if (timeptr)
	{
		tm = *timeptr;

		if (mktime(&tm) == (time_t) -1)
			tm = default_tm;
	}

	format_ptr    = format_str;
	chars_written = 0;

	while (*format_ptr)
	{
		if (!(curr_format = wcschr(format_ptr, '%')))
		{
			if ((num_chars = wcslen(format_ptr)) != 0)
			{
				if (num_chars <= space_remaining)						
				{
					wmemcpy(str, format_ptr, num_chars);

					chars_written   += num_chars;
					str             += num_chars;
					space_remaining -= num_chars;
				}
				else
					return (0);
			}

			break;
		}

		if ((num_chars = curr_format - format_ptr) != 0)
		{
			if (num_chars <= space_remaining)							
			{
				wmemcpy(str, format_ptr, num_chars);

				chars_written   += num_chars;
				str             += num_chars;
				space_remaining -= num_chars;							
			}
			else														
				return (0);												
		}

		format_ptr = curr_format;
		if ((*(format_ptr+1) == L'E') || (*(format_ptr+1) == L'O'))	
			++format_ptr;											
			
		switch (*++format_ptr)
		{
			case L'a':
				strcpy(temp_string, __MSL_extract_name(current_locale_ptr->time_cmpt_ptr->Day_Names, tm.tm_wday * 2));	/*- cc 030131 -*/
				num_chars = wemit(str, strlen(temp_string), &space_remaining, L"%s", temp_string);
				break;

			case L'A':
				strcpy(temp_string, __MSL_extract_name(current_locale_ptr->time_cmpt_ptr->Day_Names, tm.tm_wday * 2 + 1));/*- cc 030131 -*/
				num_chars = wemit(str, strlen(temp_string), &space_remaining, L"%s", temp_string);
				break;

			case L'b':
			case L'h':
				num_chars = wemit(str, 3, &space_remaining, L"%.3s",
					__MSL_extract_name(current_locale_ptr->time_cmpt_ptr->MonthNames, tm.tm_mon * 2 + 1));
				break;

			case L'B':
				strcpy(temp_string, __MSL_extract_name(current_locale_ptr->time_cmpt_ptr->MonthNames, tm.tm_mon * 2 + 1));/*- cc 030131 -*/
				num_chars = wemit(str, strlen(temp_string), &space_remaining, L"%s", temp_string);
				break;

			case L'c':
				charptr = current_locale_ptr->time_cmpt_ptr->DateTime_Format;
				char_cnt = mbsrtowcs(wtemp_string, &charptr, 32, NULL);
				if (char_cnt == -1)
					return (0);
				num_chars = wcsftime(str, space_remaining+1, wtemp_string, &tm);		
				space_remaining -= num_chars;
				break;

			case L'd':
				num_chars = wemit(str, 2, &space_remaining, L"%.2d", tm.tm_mday);
				break;
			
			case L'D':																		
				num_chars = wcsftime(str, space_remaining+1, L"%m/%d/%y", &tm);				
				break;																		

			case L'e':																		
				num_chars = wemit(str, 2, &space_remaining, L"%2d", tm.tm_mday);			
				break;																		

		#if _MSL_C99
			case L'F':																		
				num_chars = wemit(str, 10, &space_remaining, L"%.4d-%.2d-%.2d", 			
											tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);	
				break;	
				
		#if _MSL_FLOATING_POINT																	
			case 'g':																		
				ISO8601WeekNo = __MSL_ISO8601Week(timeptr, &ISO8601Year);							
				num_chars = wemit(str, 2, &space_remaining, L"%.2d", ISO8601Year % 100);   	
				break;																		

			case 'G':																		
				ISO8601WeekNo = __MSL_ISO8601Week(timeptr, &ISO8601Year);							
				num_chars = wemit(str, 4, &space_remaining, L"%.4d", ISO8601Year + 1900);		
				break;	
		#endif /* _MSL_FLOATING_POINT */
		#endif /* _MSL_C99 */

			case L'H':
				num_chars = wemit(str, 2, &space_remaining, L"%.2d", tm.tm_hour);
				break;

			case L'I':
				num_chars = wemit(str, 2, &space_remaining, L"%.2d", (n = tm.tm_hour % 12) ? n : 12);
				break;

			case L'j':
				num_chars = wemit(str, 3, &space_remaining, L"%.3d", tm.tm_yday + 1);
				break;

			case L'm':
				num_chars = wemit(str, 2, &space_remaining, L"%.2d", tm.tm_mon + 1);
				break;

			case L'M':
				num_chars = wemit(str, 2, &space_remaining, L"%.2d", tm.tm_min);
				break;

			case L'n':																		
				num_chars = wemit(str, 2, &space_remaining, L"\n");							
				break;

			case L'p':
				strcpy(temp_string, __MSL_extract_name(current_locale_ptr->time_cmpt_ptr->am_pm, tm.tm_hour < 12 ? 0 : 1));
				num_chars = wemit(str, 2, &space_remaining, L"%s", temp_string);
				break;
				
			case L'r':
				charptr = current_locale_ptr->time_cmpt_ptr->Twelve_hr_format;
				char_cnt = mbsrtowcs(wtemp_string, &charptr, 32, NULL);
				if (char_cnt == -1)
					return (0);
				num_chars = wcsftime(str, space_remaining+1, wtemp_string, &tm);		
				space_remaining -= num_chars;
				break;
			
			case L'R':
				num_chars = wcsftime(str, space_remaining+1, L"%H:%M", &tm); 
				space_remaining -= num_chars;	
				break;

			case L'S':
				num_chars = wemit(str, 2, &space_remaining, L"%.2d", tm.tm_sec);
				break;

			case 't':
				num_chars = wemit(str, 2, &space_remaining, L"\t");							
				break;

			case L'T':
				num_chars = wcsftime(str, space_remaining+1, L"%H:%M:%S", &tm); 
				space_remaining -= num_chars;	
				break;

			case 'u':
				if (tm.tm_wday == 0)														
					num_chars = wemit(str, 1, &space_remaining, L"7");						
				else																		
					num_chars = wemit(str, 1, &space_remaining, L"%.1d", tm.tm_wday);			
				break;																		

			case L'U':
				num_chars = wemit(str, 2, &space_remaining, L"%.2d", __msl_week_num(&tm, 0));
				break;

		#if _MSL_FLOATING_POINT
			case 'V':																		
				ISO8601WeekNo = __MSL_ISO8601Week(timeptr, &ISO8601Year);					
				num_chars = wemit(str, 2, &space_remaining, L"%.2d", ISO8601WeekNo);			
				break;	
		#endif /* _MSL_FLOATING_POINT */	
															
			case L'w':
				num_chars = wemit(str, 1, &space_remaining, L"%.1d", tm.tm_wday);
				break;

			case L'W':
				num_chars = wemit(str, 2, &space_remaining, L"%.2d", __msl_week_num(&tm, 1));
				break;

			case L'x':
				charptr = current_locale_ptr->time_cmpt_ptr->Date_Format;				/*- mm 040519 -*/
				char_cnt = mbsrtowcs(wtemp_string, &charptr, 32, NULL);					/*- mm 040519 -*/
				if (char_cnt == -1)														/*- mm 040519 -*/
					return (0);															/*- mm 040519 -*/
				num_chars = wcsftime(str, space_remaining+1, wtemp_string, &tm);		/*- mm 040519 -*/
				space_remaining -= num_chars;											/*- mm 040519 -*/
				break;

			case L'X':
				charptr = current_locale_ptr->time_cmpt_ptr->Time_Format;				/*- mm 040519 -*/
				char_cnt = mbsrtowcs(wtemp_string, &charptr, 32, NULL);					/*- mm 040519 -*/
				if (char_cnt == -1)														/*- mm 040519 -*/
					return (0);															/*- mm 040519 -*/
				num_chars = wcsftime(str, space_remaining+1, wtemp_string, &tm);		/*- mm 040519 -*/
				space_remaining -= num_chars;											/*- mm 040519 -*/
				break;

			case L'y':
			case L'C':																/*- mm 021218 -*/
				num_chars = wemit(str, 2, &space_remaining, L"%.2d", tm.tm_year % 100);   /*- mm 970728 -*/
				break;

			case L'Y':
				num_chars = wemit(str, 4, &space_remaining, L"%.4d", tm.tm_year + 1900);
				break;

		#if _MSL_C99
		#if _MSL_FLOATING_POINT
			case L'z':					/*- mm 030103 -*/
			{
				time_t local, utc, now;
			    struct tm localtm, *utctmptr;
			    double diff, diffmins, diffhours;
			    now = time(NULL);
			    utctmptr = gmtime(&now);
				
			    if (utctmptr == NULL)
			    	num_chars = wemit(str, 4, &space_remaining, L"0000");
			    else
			    {
			    	localtm = *localtime(&now);
				    local = mktime(&localtm);
				    utc   = mktime(utctmptr);
				    diff  = difftime(local, utc);
				    diffhours = (int)diff/3600;
				    diffmins  = abs(diff/60 - diffhours*60);
				    num_chars = wemit(str, 5, &space_remaining, L"%+03.0f%02.0f", diffhours, diffmins);
				}
				break;
				    
			}
		#endif /* _MSL_FLOATING_POINT */
		#endif /* _MSL_C99 */

			case L'Z':	
				if (*current_locale_ptr->time_cmpt_ptr->TimeZone == '\0')												
				{
					num_chars = 0;
					str[0] = '\0';
				}
				else
				    num_chars = wemit(str, strlen(current_locale_ptr->time_cmpt_ptr->TimeZone), &space_remaining, L"%s",	
				                                                     current_locale_ptr->time_cmpt_ptr->TimeZone);		
				break;
				
			case L'%':                    									
				num_chars = wemit(str,2, &space_remaining, L"%%", *format_ptr);
				break;

			default:
				num_chars = wemit(str, 2, &space_remaining, L"%%%c", *format_ptr);
				break;
		}

		if (!num_chars)
			return(0);

		chars_written += num_chars;
		str           += num_chars;

		++format_ptr;
	}

	*str = 0;
	if (max_size < chars_written) 
		return (0);
	else 
		return(chars_written);
}

#endif /* _MSL_WIDE_CHAR */												/*- mm 981020 -*/
#endif /* _MSL_C99 */													/*- mm 030304 -*/
#endif /* _MSL_OS_TIME_SUPPORT */
/*- end of mm 021220 -*/

/* Change record:
 * JCM 980128 First code release.
 * mf  980514 wince changes
 * mm  981020 Added #if _MSL_WIDE_CHAR wrappers
 * mm  990322 Allow for the fact that the count wsprintf includes the terminating null char.  See C9x
 * mm  990322 Make two char string constants into wchar_t constants.	
 * mm  990331 Correct length to allow for the fact that swfprintf takes %s to designate a char and not wide char str
 * mm  990809 Added include of wtime.h
 * cc  000326 Removed dest_os to be_os  
 * mm  010327 Commented wasctime and wctime out since they are not defined in C99 and wasctime tampered with gmtime's threadlocal data
 * mm  010421 Corrected setting of tm_isdst
 * cc  010510 Changed __add to __msl_add
 * cc  010510 Changed __div to __msl_div
 * cc  010510 Changed __ladd to __msl_ladd
 * cc  010510 Changed __lmul to __msl_lmul
 * cc  010510 Changed __mod to __msl_mod
 * mm  010521 Deleted #include of ThreadLocalData.h
 * JWW 010918 Use time_api.h to get clock and time APIs
 * cc  011203 Added _MSL_CDECL for new name mangling 
 * cc  010210 Added changes from msl_core_1 for e68k team
 * JWW 020917 Added _MSL_RESTRICT to get the restrict type specifier for certain C99 functions
 * mm  021220 Many changes to wcsftime() to bring into C99 conformance.
 * mm  030103 Corrected case of case label in wcsftime()
 * cc  030131 Added cast to const char *
 * JWW 030224 Changed __NO_WIDE_CHAR flag into the new more configurable _MSL_WIDE_CHAR
 * mm  030304 Added _MSL_C99 wrapper
 * cc  040204 The _No_Time_OS_Support flag has been deprecated and replaced with _MSL_OS_TIME_SUPPORT
 * cc  040217 Changed _No_Floating_Point to _MSL_FLOATING_POINT
 * mm  040519 Changed code to deal with %x and %X
 */