/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:32:23 $
 * $Revision: 1.42.2.2 $
 */

/*
 *	Routines
 *	--------
 *		fgetc
 *		getc
 *		getchar
 *
 *		fgets
 *		gets
 *		gets_s
 *
 *		ungetc
 *
 *		fputc
 *		putc
 *		putchar
 *
 *		fputs
 *		puts
 *
 *		__get_char
 *		__put_char
 *
 *		__ungotten
 */

#define __STDC_WANT_SECURE_LIB__ 1
#include <stdio.h>
#include <stdint.h>
#include <wchar.h>
#include <ansi_files.h>
#include <buffer_io.h>
#include <char_io.h>
#include <misc_io.h>
#include <critical_regions.h>
#include <msl_secure_error.h>

#if !defined(_Old_DSP_IO_Interface)
int _MSL_CDECL __get_char(FILE * file)
{
	int	state, ioresult;
	
	file->buffer_len = 0;
	
	if (file->state.error || file->mode.file_kind == __closed_file)
		return(EOF);
	
	state = file->state.io_state;
	
	if (state == __writing || !(file->mode.io_mode & __read))
	{
		set_error(file);
		return(EOF);
	}
	
	if (state >= __rereading)
	{
		file->state.io_state--;
		
		if (state == __rereading)
			file->buffer_len = file->saved_buffer_len;
		
		return(file->ungetc_buffer[state - __rereading]);
	}
	else
		file->state.io_state = __reading;
	
	ioresult = __load_buffer(file, NULL, __align_buffer);
	
	if (ioresult || !file->buffer_len)
	{
		if (ioresult == __io_error)
			set_error(file);
		else
			set_eof(file);
		
		return(EOF);
	}
	
	file->buffer_len--;
	
	return(*file->buffer_ptr++);
}
#endif /* __get_char conditionals */

int _MSL_CDECL fgetc(FILE * file)
{
	int retval;									/*- mm 001013 -*/
	int crtrgn;									/*- mm 020627 -*/
	if (file == stdin)							/*- mm 020627 -*/
		crtrgn = stdin_access;					/*- mm 020627 -*/
	else if (file == stdout)					/*- mm 020627 -*/
		crtrgn = stdout_access;					/*- mm 020627 -*/
	else if (file == stderr)					/*- mm 020627 -*/
		crtrgn = stderr_access;					/*- mm 020627 -*/
	else										/*- mm 020627 -*/
		crtrgn = files_access;					/*- mm 020627 -*/
  	__begin_critical_region(crtrgn);			/*- mm 001013 -*/	/*- mm 020627 -*/
	retval = __getc(file);						/*- mm 001013 -*/
  	__end_critical_region(crtrgn);		 		/*- mm 001013 -*/	/*- mm 020627 -*/
  	return(retval);								/*- mm 001013 -*/
}

int _MSL_CDECL (getc)(FILE * file)
{
	int retval;									/*- mm 001013 -*/
	int crtrgn;									/*- mm 020627 -*/
	if (file == stdin)							/*- mm 020627 -*/
		crtrgn = stdin_access;					/*- mm 020627 -*/
	else if (file == stdout)					/*- mm 020627 -*/
		crtrgn = stdout_access;					/*- mm 020627 -*/
	else if (file == stderr)					/*- mm 020627 -*/
		crtrgn = stderr_access;					/*- mm 020627 -*/
	else										/*- mm 020627 -*/
		crtrgn = files_access;					/*- mm 020627 -*/
  	__begin_critical_region(crtrgn);		 	/*- mm 001013 -*/		/*- mm 020627 -*/
	retval = __getc(file);						/*- mm 001013 -*/
  	__end_critical_region(crtrgn);		 		/*- mm 001013 -*/		/*- mm 020627 -*/
  	return(retval);								/*- mm 001013 -*/
}

#if !defined(_Old_DSP_IO_Interface)
int _MSL_CDECL (getchar)(void)
{
	int retval;									 /*- mm 001013 -*/
  	__begin_critical_region(stdin_access);		 /*- mm 001013 -*/
	retval = getchar();							 /*- mm 001013 -*/
  	__end_critical_region(stdin_access);		 /*- mm 001013 -*/
  	return(retval);								 /*- mm 001013 -*/
}
#endif /* getchar conditionals */

char * _MSL_CDECL fgets(char * _MSL_RESTRICT s, int n, FILE * _MSL_RESTRICT file)
{
	char *	p = s;
	int			c;
	int crtrgn;								/*- mm 020627 -*/
	if (file == stdin)						/*- mm 020627 -*/
		crtrgn = stdin_access;				/*- mm 020627 -*/
	else if (file == stdout)				/*- mm 020627 -*/
		crtrgn = stdout_access;				/*- mm 020627 -*/
	else if (file == stderr)				/*- mm 020627 -*/
		crtrgn = stderr_access;				/*- mm 020627 -*/
	else									/*- mm 020627 -*/
		crtrgn = files_access;				/*- mm 020627 -*/
	
	if (--n < 0)
		return(NULL);
  	__begin_critical_region(crtrgn);		/*- mm 001013 -*/	/*- mm 020627 -*/
	
	if (n)
		do
		{
			c = __getc(file);
			
			if (c == EOF)
				if (file->state.eof && p != s)
					break;
				else
				{
  					__end_critical_region(crtrgn);		 /*- mm 001013 -*/		/*- mm 020627 -*/
					return(NULL);
				}
			
			*p++ = c;
		}
		while (c != '\n' && --n);
  	__end_critical_region(crtrgn);		 /*- mm 001013 -*/		/*- mm 020627 -*/
	
	*p = 0;
	
	return(s);
}

char * _MSL_CDECL gets(char * s)
{
	char *	p = s;
	int			c;
	FILE *	file = stdin;
  	__begin_critical_region(stdin_access);		 			/*- mm 001013 -*//*- mm 020627 -*/
	
	for (;;)
	{
		c = __getc(file);
		
		if (c == EOF)
			if (file->state.eof && p != s)
				break;
			else
			{
  				__end_critical_region(stdin_access);		 /*- mm 001013 -*//*- mm 020627 -*/
				return(NULL);
			}
		
		if (c == '\n')
			break;
		
		*p++ = c;
	}
  	__end_critical_region(stdin_access);					 /*- mm 001013 -*//*- mm 020627 -*/
	
	*p = 0;
	
	return(s);
}
#if !defined(_Old_DSP_IO_Interface)
char * _MSL_CDECL gets_s(char * s, rsize_t n)
{
	char *	p = s;
	int			c;
	FILE *	file = stdin;
	
	if ((n < 1) || (s == NULL) || (n > RSIZE_MAX))
	{
		__msl_undefined_behavior_s();
		return NULL;
	}
	
  	__begin_critical_region(stdin_access);		 			/*- mm 001013 -*//*- mm 020627 -*/
	
	for (;n;n--)
	{
		c = __getc(file);
		
		if (c == EOF)
			if (file->state.eof && p != s)
				break;
			else
			{
  				__end_critical_region(stdin_access);		 /*- mm 001013 -*//*- mm 020627 -*/
				return(NULL);
			}
		
		if (c == '\n')
			break;
		
		if (n == 1)
		{
			__ungetc(c, file);
			break;
		}
		
		*p++ = c;
	}
  	__end_critical_region(stdin_access);					 /*- mm 001013 -*//*- mm 020627 -*/
	
	*p = 0;
	
	return(s);
}

int _MSL_CDECL ungetc(int c, FILE * file)
{
	int crtrgn;								/*- mm 020627 -*/
	if (file == stdin)						/*- mm 020627 -*/
		crtrgn = stdin_access;				/*- mm 020627 -*/
	else									/*- mm 020627 -*/
		crtrgn = files_access;				/*- mm 020627 -*/
	
  	__begin_critical_region(crtrgn);		 			/*- mm 001013 -*//*- mm 020627 -*/
	c = __ungetc(c, file);
  	__end_critical_region(crtrgn);					/*- mm 001013 -*//*- mm 020627 -*/
	
	return(c);
}

int _MSL_CDECL __ungetc(int c, FILE * file)
{
	int state = file->state.io_state;
	
#if _MSL_WIDE_CHAR  && _MSL_C99				/*- mm 980205 -*/	/*- mm 030314 -*/
	if (fwide(file, -1) >= 0)
		return(EOF);
#endif /* _MSL_WIDE_CHAR && _MSL_C99*/      /*- mm 980205 -*/	/*- mm 030314 -*/

	if (state == __writing || state == __rereading + __ungetc_buffer_size - 1 || c == EOF)
		return(EOF);

	if (state < __rereading)
	{	
		file->saved_buffer_len = file->buffer_len;
		file->buffer_len       = 0;
		
		state = file->state.io_state = __rereading;
	}
	else
		state = ++file->state.io_state;
	
	file->ungetc_buffer[state - __rereading] = c;
	
	file->state.eof = 0;
	
	return((unsigned char) c);
}
/* end */ /*- mm 0405004 -*/

#endif /* ungetc conditionals */

int _MSL_CDECL __ungotten(FILE * file)
{
	if (file->state.io_state < __rereading)
		return(EOF);
	
	return(file->ungetc_buffer[file->state.io_state - __rereading]);
}

#if !defined(_Old_DSP_IO_Interface)

int _MSL_CDECL __put_char(int c, FILE * file)
{
	int	kind = file->mode.file_kind;
	
	file->buffer_len = 0;
	
	if (file->state.error || kind == __closed_file)
		return(EOF);
	
#if _MSL_OS_DISK_FILE_SUPPORT	
	if (kind == __console_file)
		__stdio_atexit();
#endif
	
	if (file->state.io_state == __neutral)
		if (file->mode.io_mode & __write)
		{
#if _MSL_OS_DISK_FILE_SUPPORT
			if (file->mode.io_mode & __append)
				if (_fseek(file, 0, SEEK_END))						/*- mm 040428 -*/
					return(0);
#endif
			
			file->state.io_state = __writing;
			
			__prep_buffer(file);
		}
	
	if (file->state.io_state != __writing)
	{
		set_error(file);
		return(EOF);
	}
	
	if (file->mode.buffer_mode == _IOFBF || file->buffer_ptr - file->buffer == file->buffer_size)
		if (__flush_buffer(file, NULL))
		{
			set_error(file);
			return(EOF);
		}
	
	 file->buffer_len--;
	*file->buffer_ptr++ = c;
	
	if (file->mode.buffer_mode != _IOFBF)
	{
		if ((file->mode.buffer_mode == _IONBF || c == '\n'))
			if (__flush_buffer(file, NULL))
			{
				set_error(file);
				return(EOF);
			}
		
		file->buffer_len = 0;
	}
	
	return((unsigned char) c);
}

#endif  /* __put_char conditionals */

int _MSL_CDECL fputc(int c, FILE * file)
{
	int retval;									 /*- mm 001013 -*/
 	int crtrgn;								/*- mm 020627 -*/
	if (file == stdout)						/*- mm 020627 -*/
		crtrgn = stdout_access;				/*- mm 020627 -*/
	else if (file == stderr)				/*- mm 020627 -*/
		crtrgn = stderr_access;				/*- mm 020627 -*/
	else									/*- mm 020627 -*/
		crtrgn = files_access;				/*- mm 020627 -*/
 	__begin_critical_region(crtrgn);		 /*- mm 001013 -*//*- mm 020627 -*/
	retval = __putc(c, file);					 /*- mm 001013 -*/
  	__end_critical_region(crtrgn);		 /*- mm 001013 -*//*- mm 020627 -*/
  	return(retval);								 /*- mm 001013 -*/
}

int _MSL_CDECL (putc)(int c, FILE * file)
{
	int retval;									 /*- mm 001013 -*/
	int crtrgn;								/*- mm 020627 -*/
	if (file == stdout)						/*- mm 020627 -*/
		crtrgn = stdout_access;				/*- mm 020627 -*/
	else if (file == stderr)				/*- mm 020627 -*/
		crtrgn = stderr_access;				/*- mm 020627 -*/
	else									/*- mm 020627 -*/
		crtrgn = files_access;				/*- mm 020627 -*/
  	__begin_critical_region(crtrgn);		 /*- mm 001013 -*//*- mm 020627 -*/
	retval = __putc(c, file);					 /*- mm 001013 -*/
  	__end_critical_region(crtrgn);		 /*- mm 001013 -*/
  	return(retval);								 /*- mm 001013 -*//*- mm 020627 -*/
}

#if !defined(_Old_DSP_IO_Interface)
int _MSL_CDECL (putchar)(int c)
{
	int retval;									 /*- mm 001013 -*/
  	__begin_critical_region(stdout_access);		 /*- mm 001013 -*//*- mm 020627 -*/
	retval = __putc(c,stdout);					 /*- mm 001013 -*/
  	__end_critical_region(stdout_access);		 /*- mm 001013 -*//*- mm 020627 -*/
  	return(retval);								 /*- mm 001013 -*/
}
#endif /* putchar conditionals */

int _MSL_CDECL fputs(const char * _MSL_RESTRICT s, FILE * _MSL_RESTRICT file)
{
	int	c;
	int retval = 0;								 /*- mm 001013 -*/
	int crtrgn;								/*- mm 020627 -*/
	if (file == stdout)						/*- mm 020627 -*/
		crtrgn = stdout_access;				/*- mm 020627 -*/
	else if (file == stderr)				/*- mm 020627 -*/
		crtrgn = stderr_access;				/*- mm 020627 -*/
	else									/*- mm 020627 -*/
		crtrgn = files_access;				/*- mm 020627 -*/
  	__begin_critical_region(crtrgn);		 /*- mm 001013 -*//*- mm 020627 -*/
	while ((c = *s++) != 0)
		if (__putc(c, file) == EOF)
		{
			retval = EOF;						 /*- mm 001013 -*/
			break;								 /*- mm 001013 -*/
		}
  	__end_critical_region(crtrgn);		 /*- mm 001013 -*//*- mm 020627 -*/
	return(retval);								 /*- mm 001013 -*/
}

int _MSL_CDECL puts(const char * s)
{
	int		c;
	int     retval = 0;							 /*- mm 001013 -*/
	FILE *	file = stdout;
	
  	__begin_critical_region(stdout_access);		 /*- mm 001013 -*//*- mm 020627 -*/
	while ((c = *s++) != 0)
		if (__putc(c, file) == EOF)
		{
			retval = EOF;						 /*- mm 001013 -*/
			break;								 /*- mm 001013 -*/
		}
	if (retval != EOF)							 /*- mm 001013 -*/
		retval = __putc('\n', file);			 /*- mm 001013 -*/
	
  	__end_critical_region(stdout_access);		 /*- mm 001013 -*//*- mm 020627 -*/
	return(retval);								 /*- mm 001013 -*/
}

/* Change record:
 * JFH 950908 First code release.
 * JFH 951016 Added __ungotten for SIOUX PascalHook
 * JFH 960108 At the behest of Be, I put in real output line buffering.
 * JFH 960122 Bracketed getc(), getchar(), putc(), and putchar() by #ifndef __cplusplus
 *			  (they are inlined in <stdio.h>).
 * JFH 960212 Added call to __stdio_atexit in __put_char for console files (so they will
 *			  get flushed and closed properly).
 * JFH 960219 Added check for closed files to __get_char and __put_char.
 * JFH 960304 Allowed ungetc to work when a file is in the __neutral io_state.
 * JFH 960425 Modified __get_char, ungetc, and __ungotten to account for multi-level 'ungetc'.
 * FS  970612 Added #if __dest_os == __ppc_eabi_bare for Motorola 821
 * SCM 970710 Removed __ppc_eabi_bare conditions after implementing
 *			  serial console IO for non-os systems.
 * MEA 970720 Changed __no_os to _No_Disk_File_OS_Support.
 * mm  980204 Changes to file structure to support wide characters
 * mm  981029 Excluded certain functions for __m56800__
 * US  000504 Added variation of puts() for __m56800__
 * mm  001013 Added threadsafety to fputs, puts, putc, ungetc, gets, fgets, fgetc, 
 *			  getc, getchar
 * cc  011203 Added _MSL_CDECL for new name mangling 
 * mm  020627 Changes to prevent io on stdin, stdout, stderr and general files from interfering with each other
 * JWW 020917 Added _MSL_RESTRICT to get the restrict type specifier for certain C99 functions
 * JWW 030224 Changed __NO_WIDE_CHAR flag into the new more configurable _MSL_WIDE_CHAR
 * mm  030314 Added _MSL_C99 wrapper for fwide.
 * JWW 031030 Added the __STDC_WANT_SECURE_LIB__ secure library extensions
 * cc  040217 Changed _No_Disk_File_OS_Support to _MSL_OS_DISK_FILE_SUPPORT
 * JWW 040317 Updated the __STDC_WANT_SECURE_LIB__ secure library based on new secure specifications
 * mm  040428 Change to call _fseek to avoid nested critical regions
 * mm  040504 Changes to avoid nested critical regions with ungetc().
 */