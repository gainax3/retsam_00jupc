/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:32:22 $
 * $Revision: 1.41.2.2 $
 */

/*
 *	Routines
 *	--------
 *		setvbuf
 *		setbuf
 *
 *		__convert_from_newlines
 *		__convert_to_newlines
 *
 *		__prep_buffer
 *		__load_buffer
 *		__flush_buffer
 *
 *	Implementation Notes
 *	--------------------
 *
 *		Technically, there is no such thing as "unbuffered I/O" (_IONBF) in this
 *		implementation. With two exceptions (noted below) all I/O passes through a
 *		buffer.
 *		
 *		_IONBF forces the use of a single-character internal buffer that is
 *		associated with each file (whether used or not). Both _IOLBF and _IOFBF
 *		indicate the use of either a user-supplied or an internally-allocated
 *		buffer. A newly opened file effectively has already had a
 *		
 *		   setvbuf(f, NULL, _IOFBF, BUFSIZ)
 *		
 *		done for it. Any subsequent setbuf or setvbuf disposes of this buffer.
 *		Console input via SIOUX is line buffered automatically; there is no way to
 *		force it to do anything else.
 *		
 *		fread() and fwrite() will bypass buffers for binary I/O once the buffers
 *		are empty. Thus, if you are *only* doing fread() and/or fwrite() on a file
 *		in binary mode, then perhaps it makes sense to use _IONBF on a file;
 *		otherwise, using _IONBF will result in rather poor performance.
 *		
 *		While reading from/writing to a buffer, buffer_ptr points to the next byte
 *		to be read from/written to. buffer_pos indicated the file_position of the
 *		first byte in the buffer. Thus, the current file position can be inferred
 *		from buffer_pos and buffer_ptr (as well as the base address of the
 *		buffer). buffer_ptr is also used, when writing, to infer the number of
 *		bytes written. buffer_size is the number of bytes available in the buffer.
 *		buffer_len indicates the number of bytes left in the buffer to read or the
 *		number of bytes left in the buffer to be written to.
 *		
 *		The getc/putc macros depend on buffer_len being zero when a file's buffer
 *		is empty/full. This forces the macros to call the internal routines
 *		__get_char/__put_char, which are capable of much more sophisticated
 *		handling.
 *		
 *		buffer_len is also set to zero in cases where it is necessary to bypass
 *		the macros:
 *		
 *		o When calling fseek, fsetpos, rewind, and fflush. All these operations
 *		  force the buffer to flush and go to a "neutral" state where either
 *		  reading or writing is permitted next. The macros are thus disabled until
 *		  after the next byte is read/written. Note that it is safe at this time
 *		  to call setbuf or setvbuf, even though the standard says you can only
 *		  do it before the first I/O operation.
 *		  
 *		o Whenever the file's error flag is set. Thus, the macros are disabled and
 *		  all the I/O routines will punt until explicit action is taken to clear
 *		  the error flag.
 *		
 *		o At all times when writing a file in _IONBF or _IOLBF buffer mode:
 *		
 *		    _IONBF needs to flush the "buffer" after every character.
 *
 *			_IOLBF needs to watch for and flush after every newline character.
 */

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <buffer_io.h>
#include <critical_regions.h>						/*- mm 001013 -*/

#if(__dest_os == __mac_os    ||  \
	__dest_os == __mips_bare)  &&  \
	defined(__MWERKS__)
	
	#if !__option(mpwc_newline)
		#define _MSL_NEEDS_NEWLINE_CONVERT 1
	#endif
#endif

#ifndef _MSL_NEEDS_NEWLINE_CONVERT
	#define _MSL_NEEDS_NEWLINE_CONVERT 0
#endif
    
#if _MSL_NEEDS_NEWLINE_CONVERT

void __convert_from_newlines(unsigned char * buf, size_t * n)    /*- mm 970210 -*/
{                                                                /*- mm 970210 -*/
	unsigned char *	p = buf;                                     /*- mm 970210 -*/
	size_t			i = *n;                                      /*- mm 970210 -*/
	while (i--)                                                  /*- mm 970210 -*/
	{                                                            /*- mm 970210 -*/
		if (*p=='\r') *p = '\n';                                 /*- mm 970210 -*/
		else if (*p=='\n') *p = '\r';                            /*- mm 970210 -*/
		p++;                                                     /*- mm 970210 -*/
	}                                                            /*- mm 970210 -*/
}

void __convert_to_newlines(unsigned char * buf, size_t * n)      /*- mm 970210 -*/
{                                                                /*- mm 970210 -*/
	unsigned char *	p = buf;                                     /*- mm 970210 -*/
	size_t			i = *n;                                      /*- mm 970210 -*/
	while (i--)                                                  /*- mm 970210 -*/
	{                                                            /*- mm 970210 -*/
		if (*p=='\n') *p = '\r';                                 /*- mm 970210 -*/
		else if (*p=='\r') *p = '\n';                            /*- mm 970210 -*/
		p++;                                                     /*- mm 970210 -*/
	}                                                            /*- mm 970210 -*/
}

#else

void _MSL_CDECL __convert_from_newlines(unsigned char * p, size_t * n)
{
#pragma unused(p,n)
}

void _MSL_CDECL __convert_to_newlines(unsigned char * p, size_t * n)
{
#pragma unused(p,n)
}

#endif /* _MSL_NEEDS_NEWLINE_CONVERT */

void _MSL_CDECL __prep_buffer(FILE * file)
{
	file->buffer_ptr    = file->buffer;
	file->buffer_len    = file->buffer_size;	
	file->buffer_len   -= file->position & file->buffer_alignment;
	file->buffer_pos    = file->position;
}

int _MSL_CDECL __load_buffer(FILE * file, size_t * bytes_loaded, int alignment)
{
	int	ioresult;
	
	__prep_buffer(file);
	
	if (alignment == __dont_align_buffer)
		file->buffer_len = file->buffer_size;	
	
	ioresult = (*file->read_proc)(file->handle, file->buffer, (size_t *) &file->buffer_len, file->ref_con);
	
	if (ioresult == __io_EOF)     								/*- mm 961031 -*/
		file->buffer_len = 0;
	
	if (bytes_loaded)
		*bytes_loaded = file->buffer_len;
	
	if (ioresult)
		return(ioresult);
	
	file->position += file->buffer_len;
#if	(__dest_os == __win32_os || __dest_os == __wince_os || __dest_os == __dolphin_os)
   /*- mm 971031 -*/
	/* In Windows, during input, CRLF is converted to \n, i.e. one
	   character less, and buffer_len is adjusted correspondingly.
	   However, the value of position must be adjusted so that it is
	   a disk file position and not a buffer position.  The following
	   code does this.*/
	if (!file->mode.binary_io)
	{
		ptrdiff_t		n = file->buffer_len;
		unsigned char *	p = file->buffer;
		
		while (n--)                    							/*- jz 971105 -*/
			if (*p++ == '\n')
				file->position++;
	}
	
#endif /*- mm 971031 -*/

	
	if (!file->mode.binary_io)
		__convert_to_newlines(file->buffer, (size_t *) &file->buffer_len);
	
	return(__no_io_error);
}

int _MSL_CDECL __flush_buffer(FILE * file, size_t * bytes_flushed)
{
	size_t buffer_len;
	int ioresult;

	buffer_len = file->buffer_ptr - file->buffer;
	
	if (buffer_len)
	{
		file->buffer_len = buffer_len;
		
		if (!file->mode.binary_io)
			__convert_from_newlines(file->buffer, (size_t *) &file->buffer_len);
		
		ioresult = (*file->write_proc)(file->handle, file->buffer, (size_t *) &file->buffer_len, file->ref_con);
		
		if (bytes_flushed)
			*bytes_flushed = file->buffer_len;
		
		if (ioresult)
			return(ioresult);
		
		file->position += file->buffer_len;
	}
	
	__prep_buffer(file);
	
	return(__no_io_error);
}

int _MSL_CDECL setvbuf(FILE * _MSL_RESTRICT file, char * _MSL_RESTRICT buff, int mode, size_t size)
{
	int kind = file->mode.file_kind;
	int crtrgn;						/*- mm 020627 -*/
	
	if (mode == _IONBF)           /*- mani 970101 -*/  /*- mm 970708 -*/
		fflush(file);             /*- mani 970101 -*/  /*- mm 970708 -*/

	if (file->state.io_state != __neutral || kind == __closed_file ) /*- bkoz 970318 -*/
		return(-1);
	
	if (mode != _IONBF && mode != _IOLBF && mode != _IOFBF)
		return(-1);
	
	if (file->buffer && file->state.free_buffer)
#if _MSL_OS_DISK_FILE_SUPPORT
		free(file->buffer);
#else
		return(-1);
#endif
	if (file == stdin)						/*- mm 020627 -*/
		crtrgn = stdin_access;				/*- mm 020627 -*/
	else if (file == stdout)				/*- mm 020627 -*/
		crtrgn = stdout_access;				/*- mm 020627 -*/
	else if (file == stderr)				/*- mm 020627 -*/
		crtrgn = stderr_access;				/*- mm 020627 -*/
	else									/*- mm 020627 -*/
		crtrgn = files_access;				/*- mm 020627 -*/
  	__begin_critical_region(crtrgn);		 /*- mm 001013 -*/			/*- mm 020627 -*/
	
	file->mode.buffer_mode  = mode;
	file->state.free_buffer = 0;
	file->buffer            = &file->char_buffer;
	file->buffer_ptr        = &file->char_buffer;
	file->buffer_size       = 1;
	file->buffer_len        = 0;
	file->buffer_alignment  = 0;
	
	if (mode == _IONBF || size < 1)
	{
		*(file->buffer_ptr) = '\0';           	/*- mm 970306 -*/
  		__end_critical_region(crtrgn); 	/*- mm 010907 -*/			/*- mm 020627 -*/
		return(0);
	}
	
	if (!buff)
	{
#if _MSL_OS_DISK_FILE_SUPPORT
		if (!(buff = (char *) malloc(size)))
		{
  			__end_critical_region(crtrgn); /*- mm 001013 -*/			/*- mm 020627 -*/
			return(-1);
		}
			
		file->state.free_buffer = 1;
#else
  		__end_critical_region(crtrgn);	 /*- mm 001013 -*/				/*- mm 020627 -*/
		return(-1);
#endif
	}
	
	file->buffer      = (unsigned char *) buff;
	file->buffer_ptr  = file->buffer;
	file->buffer_size = size;
	
#if (__dest_os == __mac_os) || (__dest_os == __mac_os_x)

#define alignment_mask	(512L - 1L)

	file->buffer_alignment = 0;
	
	if (file->mode.file_kind == __disk_file && !(size & alignment_mask))
		file->buffer_alignment = alignment_mask;		/* if buffer is a multiple of 512, take steps */
														/* to align buffers on block bounderies       */
														/*		(see __prep_buffer)                     */
#else

	file->buffer_alignment = 0;
	
#endif
  	__end_critical_region(crtrgn);		 /*- mm 001013 -*/			/*- mm 020627 -*/
	
	return(0);
}

void _MSL_CDECL setbuf(FILE * _MSL_RESTRICT file, char * _MSL_RESTRICT buff)
{
	if (buff)
		setvbuf(file, buff, _IOFBF, BUFSIZ);
	else
		setvbuf(file, 0, _IONBF, 0);
}

/* Change record:
 * JFH 950824 First code release.
 * JFH 951108 Fixed a small problem in setvbuf where passing a size of 1 always
 *			  caused _IONBF behavior, which was a problem for sscanf of a single-char
 *			  string.
 * JFH 960108 At the behest of Be, I put in real output line buffering.
 * JFH 960119 Removed paranoid (*pos_proc)() calls in __load_buffer and __flush_buffer.
 * JFH 960122 Added casts from (void *) for C++ compatibility.
 * JFH 960304 Added checking to the 'mode' parameter of setvbuf.
 * JFH 960426 Merged Win32 changes in.
 * mm  961031 Changes for Pascal
 * bkoz961223 line 89 changed from if dest os == mac os
 * mani970101 Make unbuffered output really unbuffered.
 * mm  970210 Straighten out the \n \r schism
 * mm  970306 When setting a v buffer, if size == 0, then set value of 1 char buffer to '\0'
 * bkoz970318 line 199 allow setvbuf to work on console files
 * FS  970612 Wrapped _flush_buffer() routine with #if __dest_os == __ppc_eabi_bare
 			  since this routine is os dependent.
 * SCM 970710 Unwrapped _flush_buffer() to enable serial console IO.
 * MEA 970720 Changed __no_os to _No_Disk_File_OS_Support.
 * mm  970708 Inserted Be changes
 * mm  971031 Adjustment to preserve file->position as a disk position in Windows.
 * jz  971105 Fixes one-off bug, from Jay Zipnick
 * vss 980825 Seemed to have lost the logic when merging these sources - this caused a
 *            problem with MPW's newlines
 * beb 990804 Add e68k
 * mm  001013 Threadsafety for setvbuf
 * ah  001031 changes for dsp56800
 * ah  010131 back to _Old_DSP_IO_Interface
 * cc  000326 removed dest_os to be_os 
 * mm  010907 Inserted missing end_critical_region statement
 * cc  011203 Added _MSL_CDECL for new name mangling 
 * mm  020627 Changes to prevent io on stdin, stdout, stderr and general files from interfering with each other
 * JWW 020906 Use generic reference constant instead of specific idle_proc in file I/O
 * JWW 020917 Added _MSL_RESTRICT to get the restrict type specifier for certain C99 functions
 * mm  021008 Deleted _emb_68k wrapper from decision to use Mac newline conversion, _emb_68k uses Windows terminals
 * cc  040217 Changed _No_Disk_File_OS_Support to _MSL_OS_DISK_FILE_SUPPORT
 */