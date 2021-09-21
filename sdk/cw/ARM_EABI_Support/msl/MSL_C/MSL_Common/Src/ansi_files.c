/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:32:08 $
 * $Revision: 1.43.2.2 $
 */

/*
 *	Routines
 *	--------
 *		__find_unopened_file
 *		__init_file
 *		__close_all
 *		__flush_all
 */

#include <ansi_parms.h>
#include <ansi_files.h>
#include <console_io.h>
#include <critical_regions.h>
#include <file_io.h>
#include <stdlib.h>  
#include <stdio.h>
#include <string.h> 

#if !_MSL_CONSOLE_SUPPORT && !_MSL_NULL_CONSOLE_ROUTINES
	#error Turning off _MSL_CONSOLE_SUPPORT requires _MSL_NULL_CONSOLE_ROUTINES!
#endif

#if _MSL_FILE_CONSOLE_ROUTINES && !_MSL_FILE_CONSOLE_ROUTINES
	#error Turning on _MSL_FILE_CONSOLE_ROUTINES requires _MSL_FILE_CONSOLE_ROUTINES!
#endif

#if _MSL_FILE_CONSOLE_ROUTINES && _MSL_NULL_CONSOLE_ROUTINES
	#error _MSL_FILE_CONSOLE_ROUTINES and _MSL_NULL_CONSOLE_ROUTINES cannot both be on at the same time!
#endif


FILE	__files[_STATIC_FILES] =   /*- mm 961031 -*/  /*- mm 961203 -*/	/*- mm 981007 -*/
	{
		{
			0,												/* handle */
			{												/* mode   */
				__must_exist,								/* open_mode   */
				__read,										/* io_mode     */
				console_buff_mode,							/* buffer_mode */    /*- mm 981015 -*/
#if _MSL_CONSOLE_FILE_IS_DISK_FILE
				__disk_file,
#else
				__console_file,								/* file_kind   */
#endif /* _MSL_CONSOLE_FILE_IS_DISK_FILE */
#if _MSL_WIDE_CHAR				/*- mm 980204 -*/
				__unoriented,								/* file orientation */
#endif /* _MSL_WIDE_CHAR */	/*- mm 980204 -*/
				0											/* binary_io   */
			},
			{												/* state */
				/*__reading,*/								/* io_state    */
				__neutral, 			/*- bkoz 970320 -*/
				0,											/* free_buffer */
				0,											/* eof         */
				0											/* error       */
			},
#if _MSL_OS_DISK_FILE_SUPPORT
			0,												/* is_dynamically_allocated */ 	/*- mm 981007 -*/
#endif /* _MSL_OS_DISK_FILE_SUPPORT */														/*- mm 981007 -*/
			0,												/* char_buffer      */
			0,												/* char_buffer_overflow */
			{0, 0},											/* ungetc_buffer    */
#if _MSL_WIDE_CHAR				/*- mm 980204 -*/
			{0, 0},											/* ungetwc_buffer   */
#endif /* _MSL_WIDE_CHAR */		/*- mm 980204 -*/
			0,												/* position         */
			stdin_buff,										/* buffer           */
			console_buff_size,								/* buffer_size      */
			stdin_buff,										/* buffer_ptr       */
			0,												/* buffer_len       */
			0,												/* buffer_alignment */
			0,												/* saved_buffer_len */
			0,												/* buffer_pos       */
			0,												/* position_proc    */
#if _MSL_NULL_CONSOLE_ROUTINES
			0,												/* read_proc        */   /*- mm 981015 -*/
			0,												/* write_proc       */   /*- mm 981015 -*/
			0,												/* close_proc       */   /*- mm 981015 -*/
#elif _MSL_FILE_CONSOLE_ROUTINES
			__read_file,									/* read_proc        */
			__write_file,									/* write_proc       */
			__close_file,									/* close_proc       */
#else /* regular console routines */
			__read_console,									/* read_proc        */
			__write_console,								/* write_proc       */
			__close_console,								/* close_proc       */
#endif /* _MSL_NULL_CONSOLE_ROUTINES */
			0,												/* ref_con			*/
#if _MSL_OS_DISK_FILE_SUPPORT													/*- mm 981007 -*/
			&__files[1]										/* next structure   */	/*- mm 981007 -*/
#endif /* _MSL_OS_DISK_FILE_SUPPORT */												/*- mm 981007 -*/
		},



		{
			1,												/* handle */
			{												/* mode   */
				__must_exist,								/* open_mode   */
				__write,									/* io_mode   */
				console_buff_mode,							/* buffer_mode */        /*- mm 981015 -*/
#if _MSL_CONSOLE_FILE_IS_DISK_FILE
				__disk_file,
#else
				__console_file,								/* file_kind   */
#endif /* _MSL_CONSOLE_FILE_IS_DISK_FILE */
#if _MSL_WIDE_CHAR				/*- mm 980204 -*/
				__unoriented,								/* file orientation */
#endif /* _MSL_WIDE_CHAR */		/*- mm 980204 -*/
				0											/* binary_io   */
			},
			{												/* state */
				/*__writing,*/																/*- bkoz 970313 -*/
				__neutral,									/* io_state    */
				0,											/* free_buffer */
				0,											/* eof         */
				0											/* error       */
			},
#if _MSL_OS_DISK_FILE_SUPPORT															/*- mm 981007 -*/
			0,												/* is_dynamically_allocated */ 	/*- mm 981007 -*/
#endif /* _MSL_OS_DISK_FILE_SUPPORT */														/*- mm 981007 -*/
			0,												/* char_buffer      */
			0,												/* char_buffer_overflow */
			{0, 0},											/* ungetc_buffer    */
#if _MSL_WIDE_CHAR																		/*- mm 980204 -*/
			{0, 0},											/* ungetwc_buffer   */
#endif /* _MSL_WIDE_CHAR */															/*- mm 980204 -*/
			0,												/* position         */
			stdout_buff,									/* buffer           */
			console_buff_size,								/* buffer_size      */
			stdout_buff,									/* buffer_ptr       */
			0,												/* buffer_len       */
			0,												/* buffer_alignment */
			0,												/* saved_buffer_len */
			0,												/* buffer_pos       */
			0,												/* position_proc    */
#if _MSL_NULL_CONSOLE_ROUTINES
			0,												/* read_proc        */   /*- mm 981015 -*/
			0,												/* write_proc       */   /*- mm 981015 -*/
			0,												/* close_proc       */   /*- mm 981015 -*/
#elif _MSL_FILE_CONSOLE_ROUTINES
			__read_file,									/* read_proc        */
			__write_file,									/* write_proc       */
			__close_file,									/* close_proc       */
#else /* regular console routines */
			__read_console,									/* read_proc        */
			__write_console,								/* write_proc       */
			__close_console,								/* close_proc       */
#endif /* _MSL_NULL_CONSOLE_ROUTINES */
			0,												/* ref_con			*/
#if _MSL_OS_DISK_FILE_SUPPORT														/*- mm 981007 -*/
			&__files[2]										/* next structure   */		/*- mm 981007 -*/
#endif /* _MSL_OS_DISK_FILE_SUPPORT */													/*- mm 981007 -*/
		},



		{
			2,												/* handle */
			{												/* mode   */
				__must_exist,								/* open_mode   */
				__write,									/* io_mode   */
				_IONBF,										/* buffer_mode */ /*- Mani 970101 -*/   /*- mm 970708 -*/
#if _MSL_CONSOLE_FILE_IS_DISK_FILE
				__disk_file,
#else
				__console_file,								/* file_kind   */
#endif /* _MSL_CONSOLE_FILE_IS_DISK_FILE */
#if _MSL_WIDE_CHAR				/*- mm 980204 -*/
				__unoriented,								/* file orientation */
#endif /* _MSL_WIDE_CHAR */		/*- mm 980204 -*/
				0											/* binary_io   */
			},
			{												/* state */
				/*- __writing,*/ /*- 970313 bkoz -*/		/* io_state    */
				__neutral,
				0,											/* free_buffer */
				0,											/* eof         */
				0											/* error       */
			},
#if _MSL_OS_DISK_FILE_SUPPORT															/*- mm 981007 -*/
			0,												/* is_dynamically_allocated */ 	/*- mm 981007 -*/
#endif /* _MSL_OS_DISK_FILE_SUPPORT */														/*- mm 981007 -*/
			0,												/* char_buffer      */
			0,												/* char_buffer_overflow */
			{0, 0},											/* ungetc_buffer    */
#if _MSL_WIDE_CHAR				/*- mm 980204 -*/
			{0, 0},											/* ungetwc_buffer   */
#endif /* _MSL_WIDE_CHAR */		/*- mm 980204 -*/
			0,												/* position         */
			stderr_buff,									/* buffer           */
			console_buff_size,								/* buffer_size      */
			stderr_buff,									/* buffer_ptr       */
			0,												/* buffer_len       */
			0,												/* buffer_alignment */
			0,												/* saved_buffer_len */
			0,												/* buffer_pos       */
			0,												/* position_proc    */
#if _MSL_NULL_CONSOLE_ROUTINES
			0,												/* read_proc        */   /*- mm 981015 -*/
			0,												/* write_proc       */   /*- mm 981015 -*/
			0,												/* close_proc       */   /*- mm 981015 -*/
#elif _MSL_FILE_CONSOLE_ROUTINES
			__read_file,									/* read_proc        */
			__write_file,									/* write_proc       */
			__close_file,									/* close_proc       */
#else /* regular console routines */
			__read_console,									/* read_proc        */
			__write_console,								/* write_proc       */
			__close_console,								/* close_proc       */
#endif /* _MSL_NULL_CONSOLE_ROUTINES */
			0,												/* ref_con			*/
#if _MSL_OS_DISK_FILE_SUPPORT															/*- mm 981007 -*/
			&__files[3]										/* next structure   */			/*- mm 981007 -*/
#endif /* _MSL_OS_DISK_FILE_SUPPORT */														/*- mm 981007 -*/
		}
	};


#if _MSL_OS_DISK_FILE_SUPPORT
FILE * _MSL_CDECL __find_unopened_file(void)
{
	FILE *	p = __files[2].next_file_struct;	/*- mm 981007 -*/
	FILE *  plast;								/*- mm 981007 -*/
	
	while(p != NULL)   /*MW-mm 961203 */ 		/*- mm 981007 -*/
	{
		if (p->mode.file_kind == __closed_file)
			return(p);
		else									/*- mm 981007 -*/
		{										/*- mm 981007 -*/
			plast = p;							/*- mm 981007 -*/
			p = p->next_file_struct;			/*- mm 981007 -*/
		}										/*- mm 981007 -*/
	}
	
	if ((p = malloc(sizeof (FILE))) != NULL)	/*- mm 981007 -*/
	{											/*- mm 981007 -*/
		memset((void *)p, 0, sizeof(FILE));		/*- mm 981007 -*/
		p->is_dynamically_allocated = 1;		/*- mm 981007 -*/
		plast->next_file_struct = p;			/*- mm 981007 -*/
		return(p);								/*- mm 981007 -*/
	}
	return(NULL);
}

void _MSL_CDECL __init_file(FILE * file, __file_modes mode, char * buff, size_t size)
{
	file->handle            = 0;
	file->mode              = mode;
	file->state.io_state    = __neutral;
	file->state.free_buffer = 0;
	file->state.eof         = 0;
	file->state.error       = 0;
	file->position          = 0;
	
	if (size)
		setvbuf(file, buff, _IOFBF, size);
	else
		setvbuf(file, 0,    _IONBF, 0);
	
	file->buffer_ptr = file->buffer;
	file->buffer_len = 0;
	
	if (file->mode.file_kind  == __disk_file)							/*- mm 990816 -*/
	{
		file->position_proc    = __position_file;
		file->read_proc        = __read_file;
		file->write_proc       = __write_file;
		file->close_proc       = __close_file;
	}
	
	file->ref_con = 0;
}

void _MSL_CDECL __close_all(void)
{
	FILE *	p = &__files[0];											/*- mm 981007 -*/
	FILE *  plast;														/*- mm 981007 -*/
	
	__begin_critical_region(files_access);
	
	while(p != NULL)   													/*- mm 981007 -*/
	{
		if (p->mode.file_kind != __closed_file) 						/*- mm 981007 -*/
		{																/*- mm 040429 -*/
			__end_critical_region(files_access);						/*- mm 040429 -*/
			fclose(p);
			__begin_critical_region(files_access);						/*- mm 040429 -*/
		}																/*- mm 040429 -*/

		plast = p;														/*- mm 981007 -*/
		p     = p->next_file_struct;									/*- mm 981007 -*/
		if (plast->is_dynamically_allocated)							/*- mm 981007 -*/
			free(plast);												/*- mm 981007 -*/
		else															/*- mm 981007 -*/
		{																/*- mm 981007 -*/
			plast->mode.file_kind = __unavailable_file;					/*- mm 981007 -*/
			if ((p != NULL) && p->is_dynamically_allocated)				/*- mm 981007 -*/
				plast->next_file_struct = NULL;							/*- mm 981007 -*/
		}																/*- mm 981007 -*/
	}
	
	__end_critical_region(files_access);
}
#endif /*_MSL_OS_DISK_FILE_SUPPORT */

int _MSL_CDECL __flush_line_buffered_output_files(void)
{
	int		result = 0;
	FILE *	p = &__files[0];										/*- mm 981007 -*/

	#if !_MSL_OS_DISK_FILE_SUPPORT
		int file_index = 1;											/*- mm 981020 -*/
	#endif /* _MSL_OS_DISK_FILE_SUPPORT */
	
	while (p != NULL)												/*- mm 981007 -*/
	{
		if (p->mode.file_kind != __closed_file  &&
			(p->mode.buffer_mode & _IOLBF)  &&
			p->state.io_state == __writing) 
			if (fflush(p)) 
				result = EOF;
				
		#if _MSL_OS_DISK_FILE_SUPPORT
			p = p->next_file_struct;								/*- mm 981007 -*/
		#else														/*- mm 981020 -*/
			if (file_index < _STATIC_FILES)							/*- mm 981020 -*/
				p = &__files[file_index++];							/*- mm 981020 -*/
			else													/*- mm 981020 -*/
				p = NULL;											/*- mm 981020 -*/
		#endif /* _MSL_OS_DISK_FILE_SUPPORT */
	}
	
	return(result);
}

int _MSL_CDECL __flush_all(void)
{
	int		result = 0;
	FILE *	p = &__files[0];										/*- mm 981007 -*/

	#if !_MSL_OS_DISK_FILE_SUPPORT
		int file_index = 1;											/*- mm 981020 -*/
	#endif /* _MSL_OS_DISK_FILE_SUPPORT */
	
	while (p != NULL)												/*- mm 981007 -*/
	{
		if (p->mode.file_kind != __closed_file)
			if (fflush(p))
				result = EOF;
				
		#if _MSL_OS_DISK_FILE_SUPPORT
			p = p->next_file_struct;								/*- mm 981007 -*/
		#else														/*- mm 981020 -*/
			if (file_index < _STATIC_FILES)							/*- mm 981020 -*/
				p = &__files[file_index++];							/*- mm 981020 -*/
			else													/*- mm 981020 -*/
				p = NULL;											/*- mm 981020 -*/
		#endif /* _MSL_OS_DISK_FILE_SUPPORT */
	}
	
	return(result);
}

/* Change record:
 * JFH 950905 First code release.
 * JFH 951012 Made __files static and renamed. Added __file_to_num and
 *			  __num_to_file, intended to be used by unix functions. They
 *			  weren't used in the end, but I left them anyway.
 * JFH 951213 Added inits to zero for idle_proc.
 * JFH 960219 Added a conditional that leaves stdin, stdout, and stderr closed
 *			  so that you can omit SIOUX (and Unix routines used by SIOUX) and
 *			  still use the ANSI library.
 * JFH 960425 Modified 'files' initialization to account for multi-level 'ungetc'.
 * JFH 960426 Merged Win32 changes in.
 * mm  960927 Inserted setting of __dest_os to __mac_os when not otherwise set.
 * mm  961031 Changes for Pascal
 * mm  961203 Changed files to __files
 * Mani970101 Make stderr unbuffered output really work.
 * bkoz970318 lines ? 146, 103 changed stdin, stdout, stderr to __neutral
 * FS  970612 Add #if __dest_os == __ppc_eabi_bare for PPC 821
 * SCM 970709 Converted "#if __dest_os == __ppc_eabi_bare" to
 *			  "#ifndef __no_os" for more general-purpose embedded
 *			  use.  Also added support for unbuffered console IO.
 * MEA 970720 Changed __no_os to _No_Disk_File_OS_Support.
 * mm  970708 Inserted Be changes
 * mm  980204 Changes to file structure to support wide characters
 * mf  980512 wince changes
 * mm  980509 Removed static attribute so that it can be linked without error when _No_Console
 *			  is defined.  MW07076
 * mm  981007 Changes to make allocation of file structures dynamic
 * mm  981015 Added wrappers and things to support DSP56800
 * mm  981015 Commented out __file_to_num and __num_to_file while investigating whether they are really needed.
 * mm  981020 Corrections to dynamic file structures changes for _No_Disk_File_OS_Support
 * mea 981023 re-added the console_buff_mode.  Please see ansi_prefix.PPCEABI.bare.h
 * mm  981029 Changed __DSP568 to __m56800__
 * mf  990117 added include of stdlib.h/string.h for malloc,memset prototypes
 * ad  990128 Added No_Console wrappers
 * mm  990816 Deleted references to __string_file
 * mm  990817 Deleted include of <string_io.h>
 * mm  000406 Corrected setting of stream->handle for Mac stdin, stdout, stderr
 * mm  010115 Corrected handling of No_Console case
 * ah  010121 now uses dsp hostio define for hawk2 R1 and the dual target MSL project
 *            and hawk1 transitional MSL -- see ansi_parms.h.
 * mm  010322 Made linking possible with No_Console defined.
 * cc  010924 Moved _Unbuffered_Console defines from source file to header
 * cc  011203 Added _MSL_CDECL for new name mangling
 * JWW 020906 Use generic reference constant instead of specific idle_proc in file I/O
 * JWW 030224 Changed __NO_WIDE_CHAR flag into the new more configurable _MSL_WIDE_CHAR
 * cc  040217 Changed _No_Disk_File_OS_Support to _MSL_OS_DISK_FILE_SUPPORT
 * JWW 040405 Added new flexible console configuration flags
 * mm  040429 Changes to avoid nesting of critical regions
 */