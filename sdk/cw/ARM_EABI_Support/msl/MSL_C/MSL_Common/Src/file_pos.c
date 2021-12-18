/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:33:05 $
 * $Revision: 1.28.2.2 $
 */

/*
 *	Routines
 *	--------
 *		ftell
 *		fseek
 *
 *		fgetpos
 *		fsetpos
 *
 *		rewind
 */

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <ansi_files.h>
#include <buffer_io.h>
#include <critical_regions.h>						/*- mm 001013 -*/

/*- mm 970708 -*/
/*
	 XXXdbg - declare _ftell() and _fseek() the way they should have been
	          in the first place and map ftell() and fseek() to use the
						underscore versions.  This is necessary for systems (like
						the BeOS) where fpos_t is really a 64 bit quantity.
*/
						
fpos_t _MSL_CDECL _ftell(FILE * _MSL_RESTRICT file)
{
	int charsInUndoBuffer = 0;			/*- jz/ma 971105 -*/
	fpos_t	position;
	
	/*- bkoz 970324 -*/
	unsigned char tmp_kind = file->mode.file_kind;
	if ( !(tmp_kind == __disk_file || tmp_kind == __console_file) || file->state.error)
	{
		errno = EFPOS;
		return(-1L);
	}
	
	if (file->state.io_state == __neutral)
		return(file->position);
	
	position = file->buffer_pos + (file->buffer_ptr - file->buffer);

	if (file->state.io_state >= __rereading)
	{
		charsInUndoBuffer = file->state.io_state - __rereading + 1;	/*- jz/ma 971105 -*/
		position -= charsInUndoBuffer;								/*- jz/ma 971105 -*/
	}


#if	(__dest_os == __win32_os || __dest_os == __wince_os || __dest_os == __dolphin_os)
	
	if (!file->mode.binary_io)
	{
	/*- jz/ma 971105 -*/
	/*	n is the number of characters before the current location.
		We can calculate that by buffer_ptr - buffer. However if
		there are characters in the undo buffer then we need to not
		consider those, so we subtract charsInUndoBuffe from n. */
	ptrdiff_t		n = file->buffer_ptr - file->buffer - charsInUndoBuffer;
	unsigned char *	p = file->buffer;
	
	while (n--)
		if (*p++ == '\n')
			position++;
	/*- jz/ma 971105 -*/
	}
	
#endif

	return(position);
}

long _MSL_CDECL ftell(FILE * file)
{
	long retval;								 /*- mm 001013 -*/
	int crtrgn;								/*- mm 020627 -*/

	if (file == stdin)						/*- mm 020627 -*/
		crtrgn = stdin_access;				/*- mm 020627 -*/
	else if (file == stdout)				/*- mm 020627 -*/
		crtrgn = stdout_access;				/*- mm 020627 -*/
	else if (file == stderr)				/*- mm 020627 -*/
		crtrgn = stderr_access;				/*- mm 020627 -*/
	else									/*- mm 020627 -*/
		crtrgn = files_access;				/*- mm 020627 -*/
  	__begin_critical_region(crtrgn);		 /*- mm 001013 -*/			/*- mm 020627 -*/

	retval = (long)_ftell(file);				 /*- mm 001013 -*/
	
  	__end_critical_region(crtrgn);		 /*- mm 001013 -*/

	return(retval);								 /*- mm 001013 -*/
}

int _MSL_CDECL fgetpos(FILE * _MSL_RESTRICT file, fpos_t * _MSL_RESTRICT pos)
{
	int crtrgn;								/*- mm 020627 -*/

	if (file == stdin)						/*- mm 020627 -*/
		crtrgn = stdin_access;				/*- mm 020627 -*/
	else if (file == stdout)				/*- mm 020627 -*/
		crtrgn = stdout_access;				/*- mm 020627 -*/
	else if (file == stderr)				/*- mm 020627 -*/
		crtrgn = stderr_access;				/*- mm 020627 -*/
	else									/*- mm 020627 -*/
		crtrgn = files_access;				/*- mm 020627 -*/
  	__begin_critical_region(crtrgn);		 /*- mm 001013 -*/			/*- mm 020627 -*/

	*pos = _ftell(file);
	
  	__end_critical_region(crtrgn);		 /*- mm 001013 -*/
	
	return(*pos == -1);
}

int _MSL_CDECL _fseek(FILE * file, fpos_t offset, int mode)
{
	fpos_t			position;
	__pos_proc	pos_proc;
	
	/*- bkoz 970324 -*/
	unsigned char tmp_kind = file->mode.file_kind;
	if ( !(tmp_kind == __disk_file ) || file->state.error)
	{
		errno = EFPOS;
		return(-1);
	}
	
	if (file->state.io_state == __writing)
	{
		if (__flush_buffer(file, NULL) != __no_io_error)
		{
			set_error(file);
			errno = EFPOS;
			return(-1);
		}
	}
	
	if (mode == SEEK_CUR)
	{

		mode = SEEK_SET;
	
		if ((position = _ftell(file)) < 0)
			position = 0;
		
		offset += position;
	}
	
/* this block commented back in by BK 970530 after adding the line changing
   state to __reading.  this fixes the problems seen with ifstream */
   
#if	!(__dest_os	== __win32_os || __dest_os	== __wince_os) /* this optimization breaks C++ ifstream */ /*- bk 970527 -*/
	/* 970507  mm begin */
	if ((mode != SEEK_END) && (file->mode.io_mode != __read_write) &&        /*- mm 970623 -*/
			((file->state.io_state == __reading) || (file->state.io_state == __rereading)))
	{	/* When in read mode, check for a position that is within the bounds of the current buffer */
		if ((offset >= file->position) || offset < file->buffer_pos)
		{	/* not within buffer */
			file->state.io_state = __neutral;
		}
		else
		{	/* within the buffer, reset the buffer_ptr and buffer_len */
			file->buffer_ptr = file->buffer + (offset - file->buffer_pos);	
			file->buffer_len = file->position - offset;
			file->state.io_state = __reading;
		}
	}
	else
	{	/* not reading */
		file->state.io_state = __neutral;
	}
	
	if (file->state.io_state == __neutral)
	{
		if ((pos_proc = file->position_proc) != 0 && (*pos_proc)(file->handle, &offset, mode, file->ref_con))
		{
			set_error(file);
			errno = EFPOS;
			return(-1);
		}
		
		file->state.eof      = 0;
		file->position       = offset;
		file->buffer_len     = 0;
	}
	/*- mm 970507 -*/
#else
	if ((pos_proc = file->position_proc) != 0 && (*pos_proc)(file->handle, &offset, mode, file->ref_con))
	{
		set_error(file);
		errno = EFPOS;
		return(-1);
	}
		
	file->state.io_state = __neutral;
	file->state.eof      = 0;
	file->position       = offset;
	file->buffer_len     = 0;
#endif

	return(0);
}

int _MSL_CDECL fseek(FILE * file, long offset, int mode)
{
	fpos_t real_offset = (fpos_t)offset;
	int retval;								 /*- mm 001013 -*/
	
	int crtrgn;								/*- mm 020627 -*/

	if (file == stdin)						/*- mm 020627 -*/
		crtrgn = stdin_access;				/*- mm 020627 -*/
	else if (file == stdout)				/*- mm 020627 -*/
		crtrgn = stdout_access;				/*- mm 020627 -*/
	else if (file == stderr)				/*- mm 020627 -*/
		crtrgn = stderr_access;				/*- mm 020627 -*/
	else									/*- mm 020627 -*/
		crtrgn = files_access;				/*- mm 020627 -*/
  	__begin_critical_region(crtrgn);		 /*- mm 001013 -*/			/*- mm 020627 -*/
	
	retval = _fseek(file, real_offset, mode);/*- mm 000928 -*/

  	__end_critical_region(crtrgn);		 /*- mm 001013 -*/

	return(retval);							 /*- mm 001013 -*/
}

int _MSL_CDECL fsetpos(FILE * file, const fpos_t * pos)
{
	int retval;									 /*- mm 001013 -*/
	int crtrgn;								/*- mm 020627 -*/

	if (file == stdin)						/*- mm 020627 -*/
		crtrgn = stdin_access;				/*- mm 020627 -*/
	else if (file == stdout)				/*- mm 020627 -*/
		crtrgn = stdout_access;				/*- mm 020627 -*/
	else if (file == stderr)				/*- mm 020627 -*/
		crtrgn = stderr_access;				/*- mm 020627 -*/
	else									/*- mm 020627 -*/
		crtrgn = files_access;				/*- mm 020627 -*/
  	__begin_critical_region(crtrgn);		 /*- mm 001013 -*/			/*- mm 020627 -*/
		
	retval = _fseek(file, *pos, SEEK_SET);		 /*- mm 001013 -*/

  	__end_critical_region(crtrgn);		 /*- mm 001013 -*/
	
	return(retval);								 /*- mm 001013 -*/
}

     /*- mm 970708 -*/

void _MSL_CDECL rewind(FILE * file)
{
	file->state.error = 0;				/* give the seek some hope of success */
	
	fseek(file, 0, SEEK_SET);
	
	file->state.error = 0;				/* the standard says we have to do this
										 * even if the seek fails (though it's
										 * admittedly not likely to
										 */
}

/* Change record:
 * JFH 950828 First code release.
 * JFH 960425 Modified ftell to account for multi-level 'ungetc'.
 * JFH 960429 Merged Win32 changes in.
 * bkoz970320 line 32 changed so that console files don't bail out of ftell
 * bkoz970324 line 78 changed so that console files don't bail out of fseek
 * mm  970507 Change so that a seek that stays within the buffer doesn't
 *			  require the buffer to be reloaded. (change put on hold)
 * mm  970623 Change to prevent in buffer seeking with update files since these files can change from reading
 *            to writing and vice-versa through use of seek---see ANSI C 7.9.5.3
 * mm  970708 Inserted Be changes
 * mm  971103 Change to Windows code to do nothing if fseek stays where it is.  i.e.
              it is certain that the target of the seek is in the buffer.
 * jz  971105 ( &ma ) Changes to calculate the number of new lines in buffer correctly.  Changes from Mitch Adler and Jay Zipnick
 * mf  980227 undid change mm 971103
 * mm  001013 Threadsafety changes for fgetpos, fseek, fsetpos, ftell
 * cc  011203 Added _MSL_CDECL for new name mangling 
 * mm  020627 Changes to prevent io on stdin, stdout, stderr and general files from interfering with each other
 * JWW 020906 Use generic reference constant instead of specific idle_proc in file I/O
 * JWW 020917 Added _MSL_RESTRICT to get the restrict type specifier for certain C99 functions
 */