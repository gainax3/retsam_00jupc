/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:34:19 $
 * $Revision: 1.35.2.2 $
 */

/*
 *	Routines
 *	--------
 *		fgetwc				
 *		getwc				
 *		getwchar
 *
 *		fgetws				
 *
 *		ungetwc
 *
 *		fputwc				
 *		putwc				
 *		putwchar
 *
 *		fputws				
 *
 *		__get_wchar
 *		put_wchar
 *
 *		__ungotten
 *
 *		__fwide
 *		fwide
 */
 
#pragma ANSI_strict off  /*- vss 990729 -*/
 
#include <ansi_parms.h>

#if _MSL_WIDE_CHAR				/*- mm 980204 -*/

#pragma ANSI_strict reset

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <ansi_files.h>
#include <wstdio.h>
#include <wchar.h>
#include <critical_regions.h>						/*- mm 001013 -*/
#include <weof.h>

/*
	This function writes the wide character c 
	to the output stream file,
	and returns (wchar_t) c.
*/

	/* threadsafety protection is in fwrite */
wchar_t	_MSL_CDECL putwc(wchar_t c, FILE * file)
{
	if (fwide(file, 1) <= 0)			/*- mm 980206 -*/
		return(WEOF);					/*- mm 980206 -*/
	
	if( fwrite( (char*) &c, sizeof(c), 1, file) == 0)          /*- mm 980130 -*/
		return WEOF;
	
	return(c);
}

/*
	putwchar is identical to putwc with the
	second argument stdout
*/

/* threadsafety is in fwrite */
wchar_t	_MSL_CDECL putwchar(wchar_t c)				
{
		
	if (fwide(stdout, 1) <= 0)			/*- mm 980206 -*/
		return(WEOF);					/*- mm 980206 -*/
	if( fwrite( (char*) &c, sizeof(c), 1, stdout) == WEOF)	/*- mm 980130 -*/
		return WEOF;

	return(c);
}



/*
	This function is the same at putwc.
*/

wchar_t	_MSL_CDECL fputwc(wchar_t c, FILE * file)		
{
	return(putwc(c, file));
}


/*
	This function gets the wide character c 
	from the input stream file,
	and returns (wint_t) c.
*/

/*
	getwc
	
	Description
	
	If the next wide character is present from the input
	stream pointed to by file, then fgetwc function obtains
	that wide character and advances the associated file
	position indicator for the stream (if defined).
	
	
	Returns
	
	The fgetwc function returns the next wide character
	from the input stream pointed to by file. If the stream
	as at end-of-file, then ind-of-file indicator for the stream
	is set and fgetwc returns WEOF.
*/


wchar_t	_MSL_CDECL getwc(FILE * file)
{
	wchar_t c;
	int crtrgn;
	if (file->mode.file_kind == __console_file)
		crtrgn = stdin_access;
	else
		crtrgn = files_access;
	
  	__begin_critical_region(crtrgn);
	c = __getwc(file);
  	__end_critical_region(crtrgn);
	
	return(c);
}


wchar_t	_MSL_CDECL __getwc(FILE * file)					/*- jcm 971208 -*/	/*- 040505 -*/
{
	wchar_t		c;							/* wide char to hold the final result */

	if (fwide(file, 1) <= 0)							/*- mm 980206 -*/
		return(WEOF);									/*- mm 980206 -*/
	if (file->mode.file_kind == __console_file)			/*- mm 040113 -*/
		return(__getwchar());								/*- mm 040113 -*/	/*- mm 040505 -*/
	if(__fread( (char*) &c, sizeof(c), 1, file) == 0)     /*- mm 980130 -*/
		return WEOF;
	return(c);
}



wchar_t	_MSL_CDECL getwchar(void)
{
	wchar_t wch;
	
  	__begin_critical_region(stdin_access);
	wch = __getwchar();
  	__end_critical_region(stdin_access);
	
	return(wch);
}

wchar_t	_MSL_CDECL __getwchar(void)						/*- jcm 971208 -*/	/*- mm 040505 -*/
{
	wchar_t wch     = 0;							/* wide char to hold the final result */
	char mbbuffer[MB_LEN_MAX]; 							/*- mm 040113 -*/
	char * mbbufptr = mbbuffer;							/*- mm 040113 -*/
	int  mbresult   = -1;								/*- mm 040113 -*/
	int  mbstlen    = 1;								/*- mm 040113 -*/
	int i;												/*- mm 040113 -*/

	for (i = 0; i < MB_LEN_MAX; i++)
		mbbuffer[i] = 0x00;
	if (fwide(stdin, 1) <= 0)							/*- mm 980206 -*/
		return(WEOF);									/*- mm 980206 -*/
	/* __fread does not have threadsafety  */ /*- mm 001018 -*/  /*- mm 040505 -*/
	while(mbresult == -1)								/*- mm 040113 -*/
	{													/*- mm 040113 -*/
		if(__fread(mbbufptr++, 1, mbstlen, stdin) == 0)   /*- mm 980102 -*/  /*- mm 040505 -*/
		return WEOF;
		mbresult = mbtowc(&wch, mbbuffer, mbstlen++);	/*- mm 040113 -*/
	}													/*- mm 040113 -*/

	return(wch);										/*- mm 040113 -*/
}


/*
	This function is the same at getwc.
*/

wchar_t	_MSL_CDECL fgetwc(FILE * file)					/*- jcm 971208 -*/
{
	wchar_t	c;
	
  	/* getwc calls fread, which has threadsafety, adding critical regions here would
  	   lead to deadlock 	*/				/*- mm 001013 -*/
	c = getwc(file);
	return(c);
}

wchar_t	_MSL_CDECL __fgetwc(FILE * file)					/*- jcm 971208 -*/ /*- mm 040505 -*/
{
	wchar_t	c;
	
  	/* __getwc calls __fread, which does not have threadsafety, to avoid nested critical regions	*/	/*- mm 001013 -*/
	c = __getwc(file);
	return(c);
}


wchar_t _MSL_CDECL ungetwc(wchar_t c, FILE * file)
{
	int crtrgn;
	if (file == stdin)
		crtrgn = stdin_access;
	else
		crtrgn = files_access;
	
  	__begin_critical_region(crtrgn);
	c = __ungetwc(c, file);
  	__end_critical_region(crtrgn);
	
	return(c);
}

wchar_t _MSL_CDECL __ungetwc(wchar_t c, FILE * file)			/*- mm 040505 -*/
{
	int state = file->state.io_state;

	if (fwide(file, 1) <= 0)				/*- mm 980206 -*/
		return(WEOF);						/*- mm 980206 -*/
	
	if (state == __writing || state == __rereading + __ungetc_buffer_size - 1 || c == WEOF)
		return(WEOF);

	if (state < __rereading)
	{	
		file->saved_buffer_len = file->buffer_len;
		file->buffer_len       = 0;
		
		state = file->state.io_state = __rereading;
	}
	else
		state = ++file->state.io_state;
	
	file->ungetwc_buffer[state - __rereading] = c;
	
	file->state.eof = 0;

	return((wchar_t) c);
}

/*
	fputws
	
	Description
	
	The fputws writes the wide string pointed to
	by s to the stream pointed to by file. The terminating
	mull wide character is not written.

	Returns
	
	The fputws function returns EOF if a write or encoding error
	occurs: otherwise, it returns a nonnegative value.
	
*/

int	_MSL_CDECL fputws(const wchar_t * _MSL_RESTRICT s, FILE * _MSL_RESTRICT file)
{
    int     i; 
    int     retval = 1;
    int     crtrgn;						/*- mm 020627 -*/
    wchar_t	c;

	if (fwide(file, 1) <= 0)			/*- mm 980206 -*/
		return(WEOF);					/*- mm 980206 -*/
	if (file == stdout)						/*- mm 020627 -*/
		crtrgn = stdout_access;				/*- mm 020627 -*/
	else if (file == stderr)				/*- mm 020627 -*/
		crtrgn = stderr_access;				/*- mm 020627 -*/
	else									/*- mm 020627 -*/
		crtrgn = files_access;				/*- mm 020627 -*/
  	__begin_critical_region(crtrgn);		 /*- mm 001013 -*/			/*- mm 020627 -*/

    for (i = 0; s[i] != L'\0'; i++) 				/* go through the string until reaching NULL */
    { 
       /*errno = 0; 									/* initialize errno to zero */ /*- mm 010404 -*/
       c = s[i];									/* get the current character */
       
	   if (__fwrite((void*) &c, sizeof(c), 1, file) == 0) /*- mm 001018 -*/
          retval = WEOF; 							/* return wide EOF error */
    } 
  	__end_critical_region(crtrgn);		 /*- mm 001013 -*//*- mm 020627 -*/

	return(retval);
}


/*
	fgetws
	
	Description
	
	The fgetws function reads at most one less than the
	number of wide characters specified by n from the stream
	pointed to by file into the array pointed to by s. No
	additionaly wide characters are read after a new-line wide
	character (which is retained) or after end-of-file. A null
	wide character is written immediately after the last wide
	character read into the array.

	Returns
	
	The fgetws function returns s if successful. If end-
	of-file is encountered and no characters have been read into 
	the array, the contents of the array remain unchanged and a 
	null pointer is returned. If a read or encoding error occurs
	during the operatioin, the array contents are indeterminate
	and a null pointer is returned.
*/

wchar_t * _MSL_CDECL fgetws(wchar_t * _MSL_RESTRICT s, int n, FILE * _MSL_RESTRICT file)
{
	wchar_t *	p = s;
	wchar_t *   retval = s;
	wchar_t			c;
	int         crtrgn;
	
	if (fwide(file, 1) <= 0)			/*- mm 980206 -*/
		return(NULL);					/*- mm 980206 -*/

	if (--n < 0)
		return(NULL);
	if (file == stdin)						/*- mm 020627 -*/
		crtrgn = stdin_access;				/*- mm 020627 -*/
	else									/*- mm 020627 -*/
		crtrgn = files_access;				/*- mm 020627 -*/
  	__begin_critical_region(crtrgn);		 /*- mm 001013 -*/	/*- mm 020627 -*/
	if (n)
		do
		{
			if (__fread( (void*) &c, sizeof(c), 1, file) == 0) /*- mm 001018 -*/
			{
				if (file->state.eof && p == s)
					retval = NULL;
				break;
			}
			*p++ = c;
		}
		while (c != L'\n' && --n);
	
	if (retval != NULL)
	*p = L'\0';
  	__end_critical_region(crtrgn);		 /*- mm 001013 -*//*- mm 020627 -*/
	return(retval);							 /*- mm 001013 -*/

}

int _MSL_CDECL __fwide(FILE* stream, int mode)
{
	return fwide(stream, mode);
}

int _MSL_CDECL fwide(FILE* stream, int mode)
{
	int orientation;
	int result;
	
	if ((stream == NULL) || (stream->mode.file_kind == __closed_file))  /*- mm 990618 -*/
		return 0;   /* there is no associated stream therefore no orientation */
	orientation = stream->mode.file_orientation;
	switch (orientation)
	{
		case __unoriented:
			if (mode > 0)
				stream->mode.file_orientation = __wide_oriented;
			else if (mode < 0)
				stream->mode.file_orientation = __char_oriented;
			result = mode;
			break;
			
		case __wide_oriented:
			result = 1;
			break;
			
		case __char_oriented:
			result = -1;
			break;
	}
	return result;
}
			
#endif /* _MSL_WIDE_CHAR */				/*- mm 981020 -*/
	

/* Change record:
 * JCM 980121 First code release.
 * mm  980206 Added cide to handle wide char unget properly and implementation of fwide.
 * mm  981020 Added __NO_WIDE_CHAR wrappers.
 * mm  990618 Added protection to fwide against a NULL file pointer
 * mm  001018 Added threadsafety protection.
 * mm  010404 Delete local instance of errno that is never really used.
 * cc  011203 Added _MSL_CDECL for new name mangling 
 * mm  020627 Changes to prevent io on stdin, stdout, stderr and general files from interfering with each other
 * JWW 020917 Added _MSL_RESTRICT to get the restrict type specifier for certain C99 functions
 * JWW 030224 Changed __NO_WIDE_CHAR flag into the new more configurable _MSL_WIDE_CHAR
 * mm  030304 Added _MSL_C99 wrapper
 * mm  040113 Changes to make getwchar work correctly.
 * mm  040505 Changes to avoid nested critical regions
 */