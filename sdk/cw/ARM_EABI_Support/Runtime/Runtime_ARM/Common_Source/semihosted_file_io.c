/* Metrowerks ARM Runtime Support Library 
 * Copyright © 1995-2003 Metrowerks Corporation. All rights reserved. 
 * 
 * $Date: 2004/12/02 15:44:38 $ 
 * $Revision: 1.2 $ 
 */ 



/*
 *	semihosted_file_io.c	-	Disk file ARM semihosting interface
 *
 */

#include "console_io.h"
#include "semihosting.h"
#include <string.h>
#include <fcntl.h>



/* The _temp_file_info struct contains enough information to identify a temporary file */
/* based on its __file_handle, and enough information to delete the temporary file when */
/* it is closed (for example, the path to the file passed to remove()) */
typedef struct __temp_file_info temp_file_info;

struct __temp_file_info
{
	__file_handle handle;
	char temp_name[L_tmpnam];
	struct __temp_file_info * next_struct;
	struct __temp_file_info * prev_struct;
};

static temp_file_info *__temp_info_anchor = NULL;

static temp_file_info *find_temp_info(__file_handle handle)
{
	temp_file_info *p = __temp_info_anchor;
	
	while (p != NULL)
	{
		if (p->handle == handle)
			return(p);
		
		p = p->next_struct;
	}
	
	return NULL;
}

/* This translates the file modes as defined in the MSL's 
 * __file_mode struct to an integer value that has meaning
 * to the semihosting protocol:
 * 0 == "r", 1 == "rb", 2 == "r+", 3 == "r+b", 4 == "w", 5 == "wb",
 * 6 == "w+", 7 == "w+b", 8 == "a", 9 == "ab", 10 == "a+" 11 == "a+b"
 * 
 */
static int translate_msl_to_semihost_mode(__file_modes modes) {

	int semihost_offset=0;
	int binary_offset=0;
	int iomode_offset=0;
	
	
	switch ( modes.open_mode ) {
		case __must_exist:
			semihost_offset = 0;
			break;
		case __create_or_truncate:
			semihost_offset = 4;
		break;
		case __create_if_necessary:
			semihost_offset = 8;
		break;
	}
	
	if  ( modes.binary_io ) {
		binary_offset = 1;
	}
	
	switch ( modes.io_mode ) {
		case __read:
		case __write:
		case (__write|__append):
			iomode_offset = 0;
		break;
		case __read_write:
		case (__read_write|__append):
			iomode_offset = 2;
		break;
	}
			
		
	
	return semihost_offset+binary_offset+iomode_offset;	
}

/*
 *	sys_open		-	open a file on the host
 *
 */

static asm int sys_open(register void *op)
{
#ifndef __thumb // ARM version

		// save return address on stack
		str		lr,[sp,#-4]!

		mov		r1,op
		mov		r0,#SYS_OPEN
		swi		SEMIHOSTING_SWI
		
		// r0 will be non-zero handle if successful, -1 otherwise
		// restore return address from stack and return
		ldr		pc,[sp],#4

#else // Thumb version

		// save return address on stack
		push	{lr}

		mov		r1,op
		mov		r0,#SYS_OPEN
		swi		SEMIHOSTING_SWI
		
		// r0 will be non-zero handle if successful, -1 otherwise
		// restore return address from stack and return
		pop		{pc}

#endif 
}

/*
 *	__open_file	-	Set up for host file open
 *
 */

int	__open_file (char * name, __file_modes modes, __file_handle* handle)
{
	semihost_open_parms open_parms; 
	int semihost_mode = translate_msl_to_semihost_mode(modes);
	int rv_open;
	
	open_parms.name = name;
	open_parms.semihost_mode = semihost_mode;
	open_parms.name_len = strlen(name);
	rv_open = sys_open((void *)&open_parms);
	
	if ( rv_open <= 0 )
		return (__io_error);
	
	// success, set the file handle;
	*handle = (__file_handle)rv_open;

	return(__no_io_error);
}

/*
 * open - POSIX open
 *
 */
int open( const char *path, int access, ... )
{
    unsigned long handle;
    __file_modes modes = { 0 };

    if ( access & O_RDONLY )
        modes.io_mode |= __read;
    if ( access & O_WRONLY )
        modes.io_mode |= __write;
    if ( access & O_RDWR )
        modes.io_mode |= __read_write;
    if ( access & O_APPEND )
        modes.io_mode |= __append;

    if ( access & O_CREAT ) {
        modes.open_mode = __create_if_necessary;
        if ( access & O_TRUNC )
            modes.open_mode = __create_or_truncate;
    }

    if ( access & O_TRUNC )
        modes.open_mode = __create_or_truncate;
    
    if ( access & O_BINARY )
        modes.binary_io = 1;
    
    if ( __open_file((char *)path, modes, &handle) == __no_io_error) {
        return (handle);
    }
    else {
        return -1;
    }
}


/*
 *	sys_close		-	close a file on the host
 *
 */

static asm int sys_close(register unsigned long *fh)
{
#ifndef __thumb // ARM version

		// save return address on stack
		str		lr,[sp,#-4]!

		mov		r1,fh
		mov		r0,#SYS_CLOSE
		swi		SEMIHOSTING_SWI
		
		// r0 will be non-zero handle if successful, -1 otherwise
		// restore return address from stack and return
		ldr		pc,[sp],#4

#else // Thumb version

		// save return address on stack
		push	{lr}

		mov		r1,fh
		mov		r0,#SYS_CLOSE
		swi		SEMIHOSTING_SWI
		
		// r0 will be non-zero handle if successful, -1 otherwise
		// restore return address from stack and return
		pop		{pc}

#endif 
}

/*
 *	__close_file	-	Set up for host file close
 *
 */
int	__close_file ( __file_handle handle)
{

	int rv;
	int ioResult;
	temp_file_info *info;
	temp_file_info *p;
	
	/* Find out if the file to close is a temporary file */
	info = find_temp_info(handle);
	
	rv = sys_close(&handle);
	
	if ( rv < 0 )
		return (__io_error);
	
	ioResult = __no_io_error;
	
	/* If the file was a temporary file, delete it and remove the tracking information */
	if (info != NULL)
	{
		/* Delete the file only if the previous close operation succeeded */
		ioResult = __delete_file(info->temp_name);
		
		/* Unlink the tracking information from the master list */
		if (__temp_info_anchor == info)
		{
			__temp_info_anchor = info->next_struct;
			if (__temp_info_anchor != NULL)
				__temp_info_anchor->prev_struct = NULL;
		}
		else
		{
			p = info->next_struct;
			if (p != NULL)
				p->prev_struct = info->prev_struct;
			if (info->prev_struct != NULL)
				info->prev_struct->next_struct = info->next_struct;
		}
		
		/* Dispose of the temporary file tracking information space */
		free(info);
	}


	return(ioResult);
}

/*
 * close - POSIX close
 *
 */

int close (int h)
{
    return __close_file(h);
}

/*
 * fstat - POSIX fstat (not supported)
 *
 */

int fstat(int h, struct stat *buf)
{
    return -1;
}


/*
 *	sys_write		-	write a file on the host
 *
 */

static asm int sys_write(register void * wp) 
{
#ifndef __thumb // ARM version

		// save return address on stack
		str		lr,[sp,#-4]!

		mov		r1,wp
		mov		r0,#SYS_WRITE
		swi		SEMIHOSTING_SWI
		
		// r0 will contain number of bytes not written if error, 0 otherwise
		// restore return address from stack and return
		ldr		pc,[sp],#4

#else // Thumb version

		// save return address on stack
		push	{lr}

		mov		r1,wp
		mov		r0,#SYS_WRITE
		swi		SEMIHOSTING_SWI
		
		// r0 will contain number of bytes not written if error, 0 otherwise
		// restore return address from stack and return
		pop		{pc}

#endif 
}

/*
 *	__write_file	-	Set up for host file write
 *
 */
int	__write_file ( __file_handle handle, unsigned char *buf, size_t *cnt, __ref_con con )
{
	#pragma unused (con)
	semihost_readwrite_parms write_parms;
	int rv;
	
	write_parms.handle = handle;
	write_parms.data = buf;
	write_parms.count = *cnt;
	

	rv = sys_write((void *)&write_parms);
	
	if ( rv > 0 )
		return (__io_error);

	return(__no_io_error);
}

/*
 * write - POSIX write
 *
 */
int write ( int h, void *buf, unsigned len)
{
    if ( __write_file(h, (unsigned char *)buf, (unsigned long *)&len, 0) == __no_io_error) {
        return (len);
    }
    else {
        return -1;
    }
}


/*
 *	sys_read		-	read a file on the host
 *
 */

static asm int sys_read(register void * rp) 
{
#ifndef __thumb // ARM version

		// save return address on stack
		str		lr,[sp,#-4]!

		mov		r1,rp
		mov		r0,#SYS_READ
		swi		SEMIHOSTING_SWI
		
		// r0 will contain number of bytes not written if error, 0 otherwise
		// restore return address from stack and return
		ldr		pc,[sp],#4

#else // Thumb version

		// save return address on stack
		push	{lr}

		mov		r1,rp
		mov		r0,#SYS_READ
		swi		SEMIHOSTING_SWI
		
		// r0 will contain number of bytes not written if error, 0 otherwise
		// restore return address from stack and return
		pop		{pc}

#endif 
}

/*
 *	__read_file	-	Set up for host file read
 *
 */
int	__read_file ( __file_handle handle, unsigned char *buf, size_t *cnt, __ref_con con )
{
	#pragma unused (con)
	semihost_readwrite_parms read_parms;
	int rv;
	
	read_parms.handle = handle;
	read_parms.data = buf;
	read_parms.count = *cnt;
	

	rv = sys_read((void *)&read_parms);
	
	// EOF if rv == *cnt as per ARM protocol
	if ( rv == *cnt ) {
		*cnt = 0;
		return (__io_EOF);
	} // for successful call, rv is amount remaining in buffer
	  // and *cnt is number of bytes actually read.
	else if ( rv < BUFSIZ ) {
		*cnt = BUFSIZ - rv;
	}
	else { // rv > BUFSIZ
		// unclear what to do here.
	}
		
	

	return(__no_io_error);
}

/*
 * read - POSIX read
 *
 */
int read ( int h, void *buf, unsigned len)
{
    if ( __read_file(h, (unsigned char *)buf, (unsigned long *)&len, 0) == __no_io_error ) {
        return (len);
    }
    else
        return -1;
}

            
/*
 *	sys_seek		-	seek a file on the host
 *
 */
static asm int sys_seek(register void * p) 
{
#ifndef __thumb // ARM version

		// save return address on stack
		str		lr,[sp,#-4]!

		mov		r1,p
		mov		r0,#SYS_SEEK
		swi		SEMIHOSTING_SWI
		
		// r0 will contain number of bytes not written if error, 0 otherwise
		// restore return address from stack and return
		ldr		pc,[sp],#4

#else // Thumb version

		// save return address on stack
		push	{lr}

		mov		r1,p
		mov		r0,#SYS_SEEK
		swi		SEMIHOSTING_SWI
		
		// r0 will contain number of bytes not written if error, 0 otherwise
		// restore return address from stack and return
		pop		{pc}

#endif 
}

/*
 *	sys_flen		- get host file length	
 *
 */
static asm int sys_flen(register int * p) 
{
#ifndef __thumb // ARM version

		// save return address on stack
		str		lr,[sp,#-4]!

		mov		r1,p
		mov		r0,#SYS_FLEN
		swi		SEMIHOSTING_SWI
		
		// r0 will contain number of bytes not written if error, 0 otherwise
		// restore return address from stack and return
		ldr		pc,[sp],#4

#else // Thumb version

		// save return address on stack
		push	{lr}

		mov		r1,p
		mov		r0,#SYS_FLEN
		swi		SEMIHOSTING_SWI
		
		// r0 will contain number of bytes not written if error, 0 otherwise
		// restore return address from stack and return
		pop		{pc}

#endif 
}
/*
 *	__position_file	-	Set up for host file position
 *
 */
int	__position_file ( __file_handle handle, unsigned long  *offset, int mode, __ref_con con )
{
	#pragma unused (con)
	semihost_seek_parms seek_parms;
	int rv;
	int pos_adjust = 0;
	
	if (mode == SEEK_END) {
		pos_adjust = sys_flen((int *)&handle);
		if ( pos_adjust < 0 )
			return __io_error;
	}
	
	seek_parms.handle = handle;
	seek_parms.pos = *offset + pos_adjust;

	

	rv = sys_seek((void *)&seek_parms);
	
	if ( rv < 0 )
		return (__io_error);
	

	return(__no_io_error);
}

/*
 * lseek - POSIX lseek
 *
 */
long lseek (int h, long offset, int whence)
{
    return __position_file(h, (unsigned long *)&offset, whence, 0);
}


/*
 *	sys_tmpnam		-	return a temp file name from host
 *
 */
static asm int sys_tmpnam(register void * tnp) 
{
#ifndef __thumb // ARM version

		// save return address on stack
		str		lr,[sp,#-4]!

		mov		r1,tnp
		mov		r0,#SYS_TMPNAM
		swi		SEMIHOSTING_SWI
		
		// r0 will contain number of bytes not written if error, 0 otherwise
		// restore return address from stack and return
		ldr		pc,[sp],#4

#else // Thumb version

		// save return address on stack
		push	{lr}

		mov		r1,tnp
		mov		r0,#SYS_TMPNAM
		swi		SEMIHOSTING_SWI
		
		// r0 will contain number of bytes not written if error, 0 otherwise
		// restore return address from stack and return
		pop		{pc}

#endif 
}

/*
 *	__temp_file_name	-	Set up for host file temp name (tmpnam)
 *
 */
int	__temp_file_name ( char * name, void * notused )
{
	#pragma unused (notused)
	static int tmpnam_targetid;
	semihost_tmpnam_parms tmpnam_parms;
	int rv;
	
	tmpnam_parms.name = name;
	tmpnam_parms.targetid = (0xFF & tmpnam_targetid++);
	tmpnam_parms.len = L_tmpnam;

	

	rv = sys_tmpnam((void *)&tmpnam_parms);
	
	if ( rv < 0 )
		return (__io_error);
	

	return(__no_io_error);
}
	
/*
 *	sys_remove		-	remove a file the from host
 *
 */
static asm int sys_remove(register void * p) 
{
#ifndef __thumb // ARM version

		// save return address on stack
		str		lr,[sp,#-4]!

		mov		r1,p
		mov		r0,#SYS_REMOVE
		swi		SEMIHOSTING_SWI
		
		// restore return address from stack and return
		ldr		pc,[sp],#4

#else // Thumb version

		// save return address on stack
		push	{lr}

		mov		r1,p
		mov		r0,#SYS_REMOVE
		swi		SEMIHOSTING_SWI
		
		// restore return address from stack and return
		pop		{pc}

#endif 
}

/*
 *	__delete_file	-	Set up for host file delete
 *
 */
int	__delete_file ( const char * name)
{
	semihost_remove_parms remove_parms;
	int rv;
	
	remove_parms.name 	= (char*)name;
	remove_parms.len	= strlen(name);
	rv = sys_remove((void *)&remove_parms);
	
	if ( rv < 0 )
		return (__io_error);
	

	return(__no_io_error);
}

/*
 *	sys_rename		-	return a file on the host
 *
 */
static asm int sys_rename(register void * p) 
{
#ifndef __thumb // ARM version

		// save return address on stack
		str		lr,[sp,#-4]!

		mov		r1,p
		mov		r0,#SYS_RENAME
		swi		SEMIHOSTING_SWI
		
		// r0 will contain number of bytes not written if error, 0 otherwise
		// restore return address from stack and return
		ldr		pc,[sp],#4

#else // Thumb version

		// save return address on stack
		push	{lr}

		mov		r1,p
		mov		r0,#SYS_RENAME
		swi		SEMIHOSTING_SWI
		
		// r0 will contain number of bytes not written if error, 0 otherwise
		// restore return address from stack and return
		pop		{pc}

#endif 
}

/*
 *	__rename_file	-	Set up for host file rename
 *
 */
int	__rename_file ( char * oldname, char *newname )
{

	semihost_rename_parms rename_parms;
	int rv;
	
	rename_parms.oldname = oldname;
	rename_parms.olen = strlen(oldname);
	rename_parms.newname = newname;
	rename_parms.nlen = strlen(newname);

	
	rv = sys_rename((void *)&rename_parms);
	
	if ( rv )
		return (__io_error);
	

	return(__no_io_error);
}
	

/*
 *	__open_temp_file	-	Set up for open temp file
 *
 */
int	__open_temp_file ( __file_handle *handle )
{

	int rv;
	char tname[L_tmpnam];
	temp_file_info *info;
	semihost_open_parms open_parms; 
	__file_handle rv_open;

	if (__temp_file_name(tname,NULL) == __io_error) {
		return __io_error;
	}
	
	open_parms.name = tname;
	open_parms.semihost_mode = 7; // "w+b"
	open_parms.name_len = strlen(tname);
	rv_open = sys_open((void *)&open_parms);
	
	if ( rv_open < 0 )
		return (__io_error);
	
	// success, set the file handle;
	*handle = rv_open;
	
	/* Get space to keep track of the new temporary file */
	info = (temp_file_info *)malloc(sizeof(temp_file_info));
	
	if (info == NULL)
	{
		/* Space unavailable, so close and delete the file */
		__close_file(*handle);
		__delete_file(tname);
		return(__io_error);
	}
	
	/* Fill in the tracking information for the temporary file */
	info->handle = *handle;
	strcpy(info->temp_name, tname);
	info->next_struct = __temp_info_anchor;
	info->prev_struct = NULL;
	
	/* Link the new tracking information in to the master list */
	if (__temp_info_anchor != NULL)
		__temp_info_anchor->prev_struct = info;
	__temp_info_anchor = info;

	
	return(__no_io_error);	
	
}

