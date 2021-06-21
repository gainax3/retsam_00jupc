/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:32:48 $
 * $Revision: 1.37.2.3 $
 */

#define __STDC_WANT_SECURE_LIB__ 1
#include <ansi_parms.h>

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include <wctype.h>

#include <extras.h>
#include <sys/stat.h>

#include <ansi_files.h>
#include <time_api.h>
#include <file_io.h>

char * _MSL_CDECL strdup(const char *str) _MSL_CANT_THROW
{
		return __msl_strdup(str);
}

char * _MSL_CDECL strlwr (char *string) _MSL_CANT_THROW
{
	char *s = string;
	
	while (*s)
	{
		*s = tolower (*s);
		s++;
	}

	return string;
}

/* Convert unsigned integer to a string */
char * _MSL_CDECL ultoa(unsigned long val, char *str, int radix) _MSL_CANT_THROW 
{
	unsigned long theNum = val;
	int StrIndex = 0;

	do 
	{
		int CurDigit = theNum % radix;
		if (CurDigit > 9)
			str[StrIndex++] = CurDigit + 'A' - 10;
		else
			str[StrIndex++] = CurDigit + '0';
		
		theNum /= radix;
	} while (theNum);
		
	str[StrIndex++] = 0;
	
	/* Now reverse the string.*/
	__msl_strrev(str);
		
	return str;
}

/* Convert a value as a g format */
#if _MSL_FLOATING_POINT
char * _MSL_CDECL gcvt(double value, int digits, char *buffer) _MSL_CANT_THROW
{
    sprintf(buffer, "%.*g", digits, value);
    return buffer;
}
#endif

/* return unused heap memory */
int _MSL_CDECL heapmin(void) _MSL_CANT_THROW
{
    errno = ENOSYS;
    return -1;
}

/* Compare lexigraphically two strings */
int _MSL_CDECL stricmp(const char *s1, const char *s2) _MSL_CANT_THROW
{
    char c1, c2;
    while (1)
    {
    	c1 = tolower(*s1++);
    	c2 = tolower(*s2++);
        if (c1 < c2) return -1;
        if (c1 > c2) return 1;
        if (c1 == 0) return 0;
    }
#if CodeTEST
	return 1;		/*- ejs 030428: NOTREACHED but avoid warning due to complex code -*/
#endif    
}

/* Compare lexigraphically two strings up to a max length */
int _MSL_CDECL strnicmp(const char *s1, const char *s2, size_t n) _MSL_CANT_THROW
{
    return __msl_strnicmp(s1,s2, n);	
}

/* Uppercase a string */
char * _MSL_CDECL strupr(char *str) _MSL_CANT_THROW
{
	char *temp = str;							/*- mm 010116 -*/
	while (*temp)								/*- mm 010116 -*/
	{
		*temp= toupper(*temp);					/*- mm 010116 -*/
		temp++;									/*- mm 010116 -*/
	}
	return str;
}


/* Get the date in a string */
#if _MSL_OS_TIME_SUPPORT
char * _MSL_CDECL strdate(char *str) _MSL_CANT_THROW
{
    time_t timer;
    time(&timer);
    strftime(str, 10, "%m/%d/%y", localtime(&timer));
    return str;
}
#endif /* _MSL_OS_TIME_SUPPORT */

/* Set characters of string to character */
char * _MSL_CDECL strset(char *str, int c) _MSL_CANT_THROW
{
	char *save = str;

	while (*str)
	{
		*str++ = c;
	}
	
	return save;
}

/* Set first n characters of string to character */
char * _MSL_CDECL strnset(char *str, int c, size_t n) _MSL_CANT_THROW
{
	char *save = str;

	while (n && *str)
	{
		*(str++) = c;
		n--;
	}

	return save;
}

/* return pointer to first character in s1 that isn't in s2 */
char * _MSL_CDECL strspnp(char *s1, const char *s2) _MSL_CANT_THROW
{
	int len;						/*- mm 020906 -*/
	len = strspn(s1, s2);			/*- mm 020906 -*/
	if (len < strlen(s1))			/*- mm 020906 -*/
		return(s1+len);				/*- mm 020906 -*/
	else							/*- mm 020906 -*/
		return NULL;				/*- mm 020906 -*/
}

/* Case-insensitive string comparison */
int	_MSL_CDECL strncasecmp(const char *s1, const char *s2, size_t n) _MSL_CANT_THROW
{
	return __msl_strnicmp(s1,s2, n);	
}

int _MSL_CDECL strcmpi(const char *s1, const char *s2) _MSL_CANT_THROW
{
	return stricmp(s1,s2);
}

int _MSL_CDECL strncmpi(const char *s1, const char *s2, size_t n) _MSL_CANT_THROW
{
	return __msl_strnicmp(s1,s2, n);	
}	
	
int _MSL_CDECL strcasecmp(const char *s1, const char *s2) _MSL_CANT_THROW
{
	return stricmp(s1,s2);
}

int _MSL_CDECL stricoll(const char *s1, const char *s2)  _MSL_CANT_THROW
{
	return stricmp(s1, s2);
}

int _MSL_CDECL strncoll(const char *s1, const char *s2, __std(size_t) sz) _MSL_CANT_THROW
{
	return strncmp(s1, s2, sz);
}

int _MSL_CDECL strnicoll(const char *s1, const char *s2, __std(size_t) sz) _MSL_CANT_THROW  
{
	return strnicmp(s1, s2, sz);
}

char * _MSL_CDECL itoa(int val, char *str, int radix) _MSL_CANT_THROW
{
	return __msl_itoa(val, str, radix);
}

/* reverse a string in place */
char * _MSL_CDECL strrev(char * str) _MSL_CANT_THROW
{
	return __msl_strrev(str);
}

#if _MSL_OS_DISK_FILE_SUPPORT
int _MSL_CDECL filelength(int fileno) _MSL_CANT_THROW
{
	struct stat st;
	if (fstat(fileno, &st)) return -1;
	return st.st_size;
}

#if ((__dest_os == __win32_os || __dest_os == __mac_os) && _MSL_POSIX)
	int _MSL_CDECL chsize( int handle, long size ) _MSL_CANT_THROW
	{
		FILE			*infile;
		struct stat		buff;
		int 			status;
		long int		file_size;
		int 			i;
		
		/* check the size */
		if( size < 0 )
		{
			errno = EINVAL;
			return -1;
		}

		/* check if we have write permission */
		if((fstat( handle, &buff)) == 0)
		{		
			if(!( buff.st_mode & S_IWUSR))
			{
				/* we don't have write permission */
				errno = EACCES;
				return -1;
			}
		}	
		else
		{   
			/* fstat not able to obtain the file-status information -- errno set by fstat */
			return -1;
		}
	  
	 	if( (infile = fdopen( handle, "w" ))== NULL) 	
		{
			errno = EIO;
			return -1;
		}
			
		file_size = (unsigned long) buff.st_size;

		if((size - file_size) == 0)
			return 0; 
		else 
		{
			if( (unsigned long) size > file_size ) /* need to extend the file */
			{
				if((fseek( infile, 0, SEEK_END )) != 0 )
				{
					errno = EIO;
					return -1;
				}	

				for( i = file_size; i < size; i++ )
				{	
					if((fputc( 0, infile )) == EOF )
					{
						errno = EIO;
						return -1;
					}
				}
				
				/*flush the stream for the new eof to take effect */
				if((status = fflush( infile )) != 0 )
				{
					errno = EIO;
					return -1;
				}		
				
			}
			else /* need to truncate the file */
			{	
				/*flush the stream before changing the size */
				status = fflush( infile );
				if( status != 0 )
				{
					errno = EIO;
					return -1;
				}
				
				status = __msl_setfilesize(handle, size);
				if( status != 0 )
				{
					errno = EIO;
					return -1;
				}
			}
			return 0; 
		}
	}
#endif /* ((__dest_os == __win32_os || __dest_os == __mac_os) && _MSL_POSIX) */

#endif /* _MSL_OS_DISK_FILE_SUPPORT */

#if _MSL_WIDE_CHAR		
#if _MSL_C99

/* lowercase all characters in str */
int _MSL_CDECL wtoi(const wchar_t *_a) _MSL_CANT_THROW    
{
	return wcstol(_a, NULL, 10);
}
#endif /* _MSL_C99 */

wchar_t * _MSL_CDECL wcslwr (wchar_t *str) _MSL_CANT_THROW
{
	wchar_t *saved = str;

	while (*str)
	{
		*str = towlower (*str);
		str++;
	}
  
	return saved;
}

/* uppercase all characters in str  */
wchar_t * _MSL_CDECL wcsupr (wchar_t *str) _MSL_CANT_THROW
{
	wchar_t *saved = str;

	while (*str)
	{
		*str = towupper (*str);
		str++;
	}
  
	return saved;
}

int _MSL_CDECL wcsicmp(const wchar_t *s1, const wchar_t *s2) _MSL_CANT_THROW
{
    wchar_t c1, c2;
    while (1)
    {
    	c1 = towlower(*s1++);
    	c2 = towlower(*s2++);
        if (c1 < c2) return -1;
        if (c1 > c2) return 1;
        if (c1 == 0) return 0;
    }
#if CodeTEST
	return 1;		/*- ejs 030428: NOTREACHED but avoid warning due to complex code -*/
#endif    
}

/* case-insensitive compare of s1, s2, stopping after n characters  */
int _MSL_CDECL wcsnicmp(const wchar_t *s1, const wchar_t *s2, size_t n) _MSL_CANT_THROW
{
    int i;
    wchar_t c1, c2;
    for (i=0; i<n; i++)
    {
        c1 = towlower(*s1++);
        c2 = towlower(*s2++);
        if (c1 < c2) return -1;
        if (c1 > c2) return 1;
        if (!c1) return 0;
    }
    return 0;
}

/* reverse order of characters in str */
wchar_t * _MSL_CDECL wcsrev(wchar_t *str) _MSL_CANT_THROW
{
	int SmallIndex = 0;
	int BigIndex = wcslen(str) - 1;
	
	while (SmallIndex < BigIndex) {
		wchar_t Temp = str[SmallIndex];
		
		str[SmallIndex] = str[BigIndex];
		str[BigIndex] = Temp;
		
		SmallIndex++;
		BigIndex--;
	}
	
	return str;
}

/* set all characters of str to wc  */
wchar_t * _MSL_CDECL wcsset(wchar_t *str, wchar_t wc) _MSL_CANT_THROW
{
	wchar_t *saved = str;

	while (*str)
	{
		*str++ = wc;
	}
  
	return saved;
}


/* set characters of str to wc, up to max of n characters */
wchar_t * _MSL_CDECL wcsnset(wchar_t *str, wchar_t wc, size_t n) _MSL_CANT_THROW
{
	wchar_t *saved = str;

	while (*str && n--)
	{
		*str = wc;
	}
  
	return saved;
}

/* return pointer to first character in s1 that isn't in s2 */
wchar_t * _MSL_CDECL wcsspnp(const wchar_t *s1, const wchar_t *s2) _MSL_CANT_THROW
{
	int len;								/*- mm 020906 -*/
	len = wcsspn(s1, s2);					/*- mm 020906 -*/
	if (len < wcslen(s1))					/*- mm 020906 -*/
		return((wchar_t*)(s1+len));			/*- mm 020906 -*/
	else									/*- mm 020906 -*/
		return NULL;						/*- mm 020906 -*/
}

wchar_t * _MSL_CDECL wcsdup (const wchar_t *str) _MSL_CANT_THROW
{
	wchar_t * rval = (wchar_t *)__std(malloc)((__std(wcslen)(str)+1) * sizeof (wchar_t));
				
	if (rval) {
		__std(wcscpy)(rval, str);
	}
	
	return rval;
}	

wchar_t * _MSL_CDECL wstrrev(wchar_t * str) _MSL_CANT_THROW
{
	int SmallIndex = 0;
	int BigIndex = wcslen(str) - 1;
	
	while (SmallIndex < BigIndex) {
		wchar_t Temp = str[SmallIndex];
		
		str[SmallIndex] = str[BigIndex];
		str[BigIndex] = Temp;
		
		SmallIndex++;
		BigIndex--;
	}
	
	return str;
}

int _MSL_CDECL wcsicoll(const wchar_t *s1, const wchar_t *s2)  _MSL_CANT_THROW
{
	return wcsicmp(s1, s2);
}

int _MSL_CDECL wcsncoll(const wchar_t *s1, const wchar_t *s2, __std(size_t) sz) _MSL_CANT_THROW 
{
	return wcsncmp(s1, s2, sz);
}

int _MSL_CDECL wcsnicoll(const wchar_t *s1, const wchar_t *s2, __std(size_t) sz) _MSL_CANT_THROW  
{
	return wcsnicmp(s1, s2, sz);
}

wchar_t * _MSL_CDECL itow(int val, wchar_t *str, int radix)  _MSL_CANT_THROW
{
	char IsNegative = 0;
	int theNum = val;
	unsigned int theUNum;							/*- mm 000209 -*/
	int StrIndex = 0;

	if (val < 0)									/*- mm 000209 -*/
	{												/*- mm 000209 -*/
		theUNum    = -val;							/*- mm 000209 -*/
		IsNegative = 1;								/*- mm 000209 -*/
	}												/*- mm 000209 -*/
	else											/*- mm 000209 -*/
		theUNum = val;								/*- mm 000209 -*/

	do 												
	{												
		int CurDigit = theUNum % radix;				/*- mm 000209 -*/
		if (CurDigit > 9)
			str[StrIndex++] = CurDigit + L'A' - 10;
		else
			str[StrIndex++] = CurDigit + L'0';

		theUNum /= radix;							/*- mm 000209 -*/
	} while (theUNum);								/*- mm 000209 -*/
	if (IsNegative) 
		str[StrIndex++] = '-';
	str[StrIndex++] = 0;

		/* Now reverse the string.*/
	wstrrev(str);

	return str;
}

double _MSL_CDECL watof(const wchar_t * str) _MSL_CANT_THROW
{
	return(wcstod(str, NULL));
}

#endif /* _MSL_WIDE_CHAR */


#if _MSL_OS_TIME_SUPPORT

char * _MSL_CDECL asctime_r(const struct tm *tm, char *s) _MSL_CANT_THROW
{
	__msl_asciitime(*tm, s);
	return s;
}

char * _MSL_CDECL ctime_r(const time_t *timer, char *s) _MSL_CANT_THROW
{
	return(asctime_r(localtime(timer), s));
}

struct tm * _MSL_CDECL localtime_r(const time_t *timer, struct tm *tm) _MSL_CANT_THROW
{
	return localtime_s(timer, tm);
}

struct tm * _MSL_CDECL gmtime_r(const time_t *timer, struct tm *tm) _MSL_CANT_THROW
{
	return gmtime_s(timer, tm);
}

#endif /*_MSL_OS_TIME_SUPPORT */


int _MSL_CDECL rand_r(unsigned int *context) _MSL_CANT_THROW
{
	*context = *context * 1103515245 + 12345;
	return((int)(*context >> 16) & 0x7FFF);
}

int _MSL_CDECL strerror_r(int errnum, char *str, size_t buflen) _MSL_CANT_THROW
{
	char errstr[__max_errstr];
	
	__strerror(errnum, errstr);
	strncpy(str, errstr, buflen);
	if (buflen > 1)
		str[buflen - 1] = 0;
	
	return 0;
}

char * _MSL_CDECL strtok_r(char * _MSL_RESTRICT str, const char * _MSL_RESTRICT set, char ** _MSL_RESTRICT ptr)
{
	return strtok_s(str, set, ptr);
}

/* Change record:
 * cc  000510 made __myraise an inline in unix.h
 * cc  000511 moved common source from extras.win32.c 
 * cc  000511 added <cwchar>, <cwctype>
 * as  000821 added guards for wide character support
 * mm  010116 Changes to make _strupr return pointer to beginning of string instead of end.  Josef's fix.
 * cc  010130 Changed _splitpath to support UNC pathnames
 * mm  010412 Avoided setting errno to negative value.
 * cc  010605 Made _itoa call __msl_itoa, _strrev call __msl_strrev, _itoa call __msl_itoa,and
 *			  _strnicmp call __msl_strnicmp
 * cc  010605 Added #include of file_io.h
 * cc  010713 Made all the functions _ and non_ 
 * cc  010714 Added strcmpi and strncmpi
 * cc  010715 Removed makepath & splitpath
 * cc  010725 Changed _strdup & strdup to call __msl_strdup
 * cc  010924 Added Hisham's version of chsize  
 * ejs 011128 Added strcoll/wcscoll variants
 * cc  011203 Added _MSL_CDECL for new name mangling 
 * cc  020130 Added changes for e68k group
 * mm  020906 Rewrote strspnp and wcsspnp because of error reported in WB1-38641
 * cc  021119 Moved watof from the C lib to extras
 * cc  020106 Fixed chsize to change file size
 * JWW 030224 Changed __NO_WIDE_CHAR flag into the new more configurable _MSL_WIDE_CHAR
 * ejs 030428 Added change to avoid compiler warnings when CodeTEST enabled
 * cc  030722 Moved underscored functions out of extras common headers/source.  
 *            They now live in the extras_xxxx_win32.h headers/source.
 * JWW 031103 Added asctime_r, ctime_r, localtime_r, gmtime_r, rand_r, and strerror_r
 * ejs 040108 Use <sys/stat.h>, not <stat.h>
 * cc  040204 The _No_Time_OS_Support flag has been deprecated and replaced with _MSL_OS_TIME_SUPPORT
 * cc  040217 Changed _No_Floating_Point to _MSL_FLOATING_POINT
 * cc  040217 Changed _No_Disk_File_OS_Support to _MSL_OS_DISK_FILE_SUPPORT
 */