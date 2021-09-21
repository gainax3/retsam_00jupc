/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:33:46 $
 * $Revision: 1.12.2.2 $
 */

#include <ansi_parms.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <unix.h>
#include <utime.h>
#include <utsname.h>
#include <unistd_api.h>
#include <stdarg.h>
#include <stdio.posix.h>  /* need this for fileno */


#ifndef _MSL_PRO6_POSIX_COMPATIBILITY
	#define _MSL_PRO6_POSIX_COMPATIBILITY 1
#endif

#if _MSL_PRO6_POSIX_COMPATIBILITY

_MSL_BEGIN_EXTERN_C

/* Items from fcntl.h */
_MSL_IMP_EXP_C int _MSL_CDECL _creat(const char *, mode_t) _MSL_CANT_THROW;
_MSL_IMP_EXP_C int _MSL_CDECL _fcntl(int, int, ...) _MSL_CANT_THROW;
_MSL_IMP_EXP_C int _MSL_CDECL _open(const char *, int, ...) _MSL_CANT_THROW;

int _MSL_CDECL _creat(const char *path, mode_t mode) _MSL_CANT_THROW
{
	return creat(path, mode);
}

int _MSL_CDECL _fcntl(int fildes, int cmd, ...) _MSL_CANT_THROW
{
	return fcntl(fildes, cmd);
}


int _MSL_CDECL _open(const char *path, int oflag, ...) _MSL_CANT_THROW
{
	return open(path, oflag);
}


/* Items from stat.h */

#if (__dest_os == __win32_os || __dest_os == __wince_os)
	#define _MSL_STAT_TYPE _stat
#else
	#define _MSL_STAT_TYPE stat
#endif

_MSL_IMP_EXP_C int _MSL_CDECL _fstat(int, struct _MSL_STAT_TYPE *) _MSL_CANT_THROW;
_MSL_IMP_EXP_C int _MSL_CDECL _mkdir(const char *, ...) _MSL_CANT_THROW;
_MSL_IMP_EXP_C int _MSL_CDECL _stat(const char *, struct _MSL_STAT_TYPE *) _MSL_CANT_THROW;

int _MSL_CDECL _fstat(int fildes, struct _MSL_STAT_TYPE *buf) _MSL_CANT_THROW
{
	return fstat(fildes, (struct stat *) buf);
}

int _MSL_CDECL _mkdir(const char *path, ...) _MSL_CANT_THROW
{
	return mkdir(path);
}

int _MSL_CDECL _stat(const char *path, struct _MSL_STAT_TYPE *buf) _MSL_CANT_THROW
{
	return stat(path, (struct stat *) buf);
}


/* Items from stdio.h */

_MSL_IMP_EXP_C int _MSL_CDECL _fileno(__std(FILE) *) _MSL_CANT_THROW;
_MSL_IMP_EXP_C __std(FILE) * _MSL_CDECL _fdopen(int, const char *) _MSL_CANT_THROW;

int _MSL_CDECL _fileno(FILE *stream) _MSL_CANT_THROW
{
	return fileno(stream);
}

FILE * _MSL_CDECL _fdopen(int fildes, const char *type) _MSL_CANT_THROW
{
	return fdopen(fildes, type);
}


/* Items from unistd.h */
_MSL_IMP_EXP_C int _MSL_CDECL _access(const char *, int) _MSL_CANT_THROW;
_MSL_IMP_EXP_C int _MSL_CDECL _chdir(const char *) _MSL_CANT_THROW;
_MSL_IMP_EXP_C int _MSL_CDECL _close(int) _MSL_CANT_THROW;
_MSL_IMP_EXP_C char * _MSL_CDECL _cuserid(char *) _MSL_CANT_THROW;
_MSL_IMP_EXP_C int _MSL_CDECL _exec(const char *, ...) _MSL_CANT_THROW;

/*- ejs 011120 -*/
#if __dest_os == __win32_os
	_MSL_IMP_EXP_C  int _MSL_CDECL _execl(const char *,...) _MSL_CANT_THROW;
	_MSL_IMP_EXP_C  int _MSL_CDECL _execv(const char *,const char *const*) _MSL_CANT_THROW;
	_MSL_IMP_EXP_C  int _MSL_CDECL _execle(const char *,...) _MSL_CANT_THROW;
	_MSL_IMP_EXP_C  int _MSL_CDECL _execve(const char *,const char *const*, const char *const*) _MSL_CANT_THROW;
	_MSL_IMP_EXP_C  int _MSL_CDECL _execlp(const char *,...) _MSL_CANT_THROW;
	_MSL_IMP_EXP_C  int _MSL_CDECL _execvp(const char *,const char *const *) _MSL_CANT_THROW;
	_MSL_IMP_EXP_C  int _MSL_CDECL _execlpe(const char *,...) _MSL_CANT_THROW;
	_MSL_IMP_EXP_C  int _MSL_CDECL _execvpe(const char *,const char *const *, const char *const*) _MSL_CANT_THROW;
	_MSL_IMP_EXP_C  int _MSL_CDECL _spawnl(int,const char *,...) _MSL_CANT_THROW;
	_MSL_IMP_EXP_C  int _MSL_CDECL _spawnv(int,const char *,const char *const*) _MSL_CANT_THROW;
	_MSL_IMP_EXP_C  int _MSL_CDECL _spawnle(int,const char *,...) _MSL_CANT_THROW;
	_MSL_IMP_EXP_C  int _MSL_CDECL _spawnve(int,const char *,const char *const*, const char *const*) _MSL_CANT_THROW;
	_MSL_IMP_EXP_C  int _MSL_CDECL _spawnlp(int,const char *,...) _MSL_CANT_THROW;
	_MSL_IMP_EXP_C  int _MSL_CDECL _spawnvp(int,const char *,const char *const *) _MSL_CANT_THROW;
	_MSL_IMP_EXP_C  int _MSL_CDECL _spawnlpe(int,const char *,...) _MSL_CANT_THROW;
	_MSL_IMP_EXP_C  int _MSL_CDECL _spawnvpe(int,const char *,const char *const *, const char *const*) _MSL_CANT_THROW;
	_MSL_IMP_EXP_C 	int _MSL_CDECL _cwait(int *,int,int) _MSL_CANT_THROW;
	_MSL_IMP_EXP_C  int _MSL_CDECL _dup(int _a ) _MSL_CANT_THROW;
	_MSL_IMP_EXP_C  int _MSL_CDECL _dup2(int _a, int _b ) _MSL_CANT_THROW;
#else /* __dest_os != __win32_os */
	_MSL_IMP_EXP_C int _MSL_CDECL _execl(const char *, ...) _MSL_CANT_THROW;
	_MSL_IMP_EXP_C int _MSL_CDECL _execle(const char *, ...) _MSL_CANT_THROW;
	_MSL_IMP_EXP_C int _MSL_CDECL _execlp(const char *, ...) _MSL_CANT_THROW;
	_MSL_IMP_EXP_C int _MSL_CDECL _execv(const char *, ...) _MSL_CANT_THROW;
	_MSL_IMP_EXP_C int _MSL_CDECL _execve(const char *, ...) _MSL_CANT_THROW;
	_MSL_IMP_EXP_C int _MSL_CDECL _execvp(const char *, ...) _MSL_CANT_THROW;
#endif

_MSL_IMP_EXP_C char * _MSL_CDECL _getcwd(char *, int) _MSL_CANT_THROW;
_MSL_IMP_EXP_C char * _MSL_CDECL _getlogin(void) _MSL_CANT_THROW;
_MSL_IMP_EXP_C int _MSL_CDECL _getpid(void) _MSL_CANT_THROW;
_MSL_IMP_EXP_C int _MSL_CDECL _isatty(int) _MSL_CANT_THROW;
_MSL_IMP_EXP_C long _MSL_CDECL _lseek(int, long, int) _MSL_CANT_THROW;
_MSL_IMP_EXP_C int _MSL_CDECL _read(int , void*, __std(size_t)) _MSL_CANT_THROW;
_MSL_IMP_EXP_C int _MSL_CDECL _rmdir(const char*) _MSL_CANT_THROW;
_MSL_IMP_EXP_C unsigned int _MSL_CDECL _sleep(unsigned int) _MSL_CANT_THROW;
_MSL_IMP_EXP_C char * _MSL_CDECL _ttyname(int ) _MSL_CANT_THROW;
_MSL_IMP_EXP_C int _MSL_CDECL _unlink(const char*) _MSL_CANT_THROW;
_MSL_IMP_EXP_C int _MSL_CDECL _write(int, const void*, __std(size_t)) _MSL_CANT_THROW;

int _MSL_CDECL _access(const char *path, int mode) _MSL_CANT_THROW
{
	return access(path, mode);
}

int _MSL_CDECL _chdir(const char *path) _MSL_CANT_THROW
{
	return chdir(path);
}

int _MSL_CDECL _close(int fildes) _MSL_CANT_THROW
{
	return close(fildes);
}

char * _MSL_CDECL _cuserid(char *string) _MSL_CANT_THROW
{
	return cuserid(string);
}

int _MSL_CDECL _exec(const char *path, ...) _MSL_CANT_THROW
{
	return exec(path);
}

#if __dest_os == __win32_os
#pragma mark -

int _MSL_CDECL _spawnl(int mode, const char *prog, ...) _MSL_CANT_THROW
{
	va_list va;
	int ret;

	va_start(va, prog);
	ret = __msl_vspawnl(mode, prog, va);
	va_end(va);
	return ret;
}

int	_MSL_CDECL _spawnle(int mode, const char *prog, ...) _MSL_CANT_THROW
{
	va_list va;
	int ret;

	va_start(va, prog);
	ret = __msl_vspawnle(mode, prog, va);
	va_end(va);
	return ret;
}

int	_MSL_CDECL _spawnlp(int mode, const char *prog, ...) _MSL_CANT_THROW
{
	va_list va;
	int ret;

	va_start(va, prog);
	ret = __msl_vspawnlp(mode, prog, va);
	va_end(va);
	return ret;
}

int	_MSL_CDECL _spawnlpe(int mode, const char *prog, ...) _MSL_CANT_THROW
{
	va_list va;
	int ret;

	va_start(va, prog);
	ret = __msl_vspawnlpe(mode, prog, va);
	va_end(va);
	return ret;
}

int _MSL_CDECL _execl(const char *prog, ...) _MSL_CANT_THROW
{
	va_list va;
	int ret;

	va_start(va, prog);
	ret = __msl_vspawnl(_P_OVERLAY, prog, va);
	va_end(va);
	return ret;
}

int	_MSL_CDECL _execle(const char *prog, ...) _MSL_CANT_THROW
{
	va_list va;
	int ret;

	va_start(va, prog);
	ret = __msl_vspawnle(_P_OVERLAY, prog, va);
	va_end(va);
	return ret;
}

int	_MSL_CDECL _execlp(const char *prog, ...) _MSL_CANT_THROW
{
	va_list va;
	int ret;

	va_start(va, prog);
	ret = __msl_vspawnlp(_P_OVERLAY, prog, va);
	va_end(va);
	return ret;
}

int	_MSL_CDECL _execlpe(const char *prog, ...) _MSL_CANT_THROW
{
	va_list va;
	int ret;

	va_start(va, prog);
	ret = __msl_vspawnlpe(_P_OVERLAY, prog, va);
	va_end(va);
	return ret;
}

int	_MSL_CDECL _execv(const char *prog,const char *const* args) _MSL_CANT_THROW
{
	return execv(prog, args);
}

int	_MSL_CDECL _execve(const char *prog, const char *const* args, const char *const* envp) _MSL_CANT_THROW
{
	return execve(prog, args, envp);
}

int	_MSL_CDECL _execvp(const char *prog,const char *const *args) _MSL_CANT_THROW
{
	return execvp(prog, args);
}

int	_MSL_CDECL _execvpe(const char *prog, const char *const *args, const char *const* envp) _MSL_CANT_THROW
{
	return execvpe(prog, args, envp);
}

int	_MSL_CDECL _spawnv(int mode, const char *prog,const char *const* args) _MSL_CANT_THROW
{
	return spawnv(mode, prog, args);
}

int	_MSL_CDECL _spawnve(int mode, const char *prog, const char *const* args, const char *const* envp) _MSL_CANT_THROW
{
	return spawnve(mode, prog, args, envp);
}

int	_MSL_CDECL _spawnvp(int mode, const char *prog,const char *const *args) _MSL_CANT_THROW
{
	return spawnvp(mode, prog, args);
}

int	_MSL_CDECL _spawnvpe(int mode, const char *prog, const char *const *args, const char *const* envp) _MSL_CANT_THROW
{
	return spawnvpe(mode, prog, args, envp);
}

int _MSL_CDECL _cwait(int *termstat, int pid, int action) _MSL_CANT_THROW
{
	return cwait(termstat, pid, action);
}

int _MSL_CDECL _dup(int _a ) _MSL_CANT_THROW
{ 
	return dup(_a);
} 

int _MSL_CDECL _dup2(int _a, int _b ) _MSL_CANT_THROW
{ 
	return dup2(_a, _b);
}		


#else	/* __dest_os != __win32_os */
#pragma mark -

/*	These stubs only make sense for classic MacOS -- feel free to copy
	the above versions for POSIX-capable systems */
	
int _MSL_CDECL _execl(const char *path, ...) _MSL_CANT_THROW
{
	return exec(path);
}

int _MSL_CDECL _execle(const char *path, ...) _MSL_CANT_THROW
{
	return exec(path);
}

int _MSL_CDECL _execlp(const char *path, ...) _MSL_CANT_THROW
{
	return exec(path);
}

int _MSL_CDECL _execv(const char *path, ...) _MSL_CANT_THROW
{
	return exec(path);
}

int _MSL_CDECL _execve(const char *path, ...) _MSL_CANT_THROW
{
	return exec(path);
}

int _MSL_CDECL _execvp(const char *path, ...) _MSL_CANT_THROW
{
	return exec(path);
}

#endif	/* __dest_os == __win32_os */
#pragma mark -

char * _MSL_CDECL _getcwd(char *buf, int size) _MSL_CANT_THROW
{
	return getcwd(buf, size);
}

char * _MSL_CDECL _getlogin(void) _MSL_CANT_THROW
{
	return getlogin();
}

int _MSL_CDECL _getpid(void) _MSL_CANT_THROW
{
	return getpid();
}

int _MSL_CDECL _isatty(int fildes) _MSL_CANT_THROW
{
	return isatty(fildes);
}

long _MSL_CDECL _lseek(int fildes, long offset, int whence) _MSL_CANT_THROW
{
	return lseek(fildes, offset, whence);
}

int _MSL_CDECL _read(int fildes, void *buf, size_t count) _MSL_CANT_THROW
{
	return read(fildes, buf, count);
}

int _MSL_CDECL _rmdir(const char *path) _MSL_CANT_THROW
{
	return rmdir(path);
}

unsigned int _MSL_CDECL _sleep(unsigned int seconds) _MSL_CANT_THROW
{
	return sleep(seconds);
}

char * _MSL_CDECL _ttyname(int fildes) _MSL_CANT_THROW
{
	return ttyname(fildes);
}

int _MSL_CDECL _unlink(const char *path) _MSL_CANT_THROW
{
	return unlink(path);
}

int _MSL_CDECL _write(int fildes, const void *buf, size_t count) _MSL_CANT_THROW
{
	return write(fildes, buf, count);
}

/* Items from unix.h */

_MSL_IMP_EXP_C int _MSL_CDECL _chmod(const char *, mode_t) _MSL_CANT_THROW; 						
_MSL_IMP_EXP_C _MSL_POSIX_PARAMS _MSL_CDECL _umask(_MSL_POSIX_PARAMS) _MSL_CANT_THROW;  						/*- cc 000210 -*/		

int _MSL_CDECL _chmod(const char *path, mode_t mode) _MSL_CANT_THROW
{
	return chmod(path, mode);
}

_MSL_POSIX_PARAMS _MSL_CDECL _umask(_MSL_POSIX_PARAMS __pmode) _MSL_CANT_THROW
{
	return umask(__pmode);
}

/* Items from utime.h */

#if (__dest_os == __win32_os || __dest_os == __wince_os)
	#define _MSL_STRUCT_UTIMEBUF_TYPE struct
#else
	#define _MSL_STRUCT_UTIMEBUF_TYPE const struct
#endif

_MSL_IMP_EXP_C int _MSL_CDECL _utime(const char *, _MSL_STRUCT_UTIMEBUF_TYPE utimbuf *) _MSL_CANT_THROW;
_MSL_IMP_EXP_C int _MSL_CDECL _utimes(const char *path, struct timeval buf[2]) _MSL_CANT_THROW;

int _MSL_CDECL _utime(const char *path, _MSL_STRUCT_UTIMEBUF_TYPE utimbuf *buf) _MSL_CANT_THROW
{
	return utime(path, buf);
}

int _MSL_CDECL _utimes(const char *path, struct timeval buf[2]) _MSL_CANT_THROW
{
	return utimes(path, buf);
}


/* Items from utsname.h */

_MSL_IMP_EXP_C int _MSL_CDECL _uname(struct utsname*) _MSL_CANT_THROW;

int _MSL_CDECL _uname(struct utsname *name) _MSL_CANT_THROW
{
	return uname(name);
}

_MSL_END_EXTERN_C

#endif

/* Change record:
 * JWW 010614 Added Pro 6 compatibility routines
 * cc  010622 Added _chmod and _umask
 * cc  011203 Added _MSL_CDECL for new name mangling 
 * cc  020221 Added #include
 * ejs 040108 Use <sys/stat.h>, not <stat.h>
 */