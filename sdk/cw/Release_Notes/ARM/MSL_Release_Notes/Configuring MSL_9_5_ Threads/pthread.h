/* Metrowerks Standard Library
 * Copyright © 1995-2004 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2004/04/16 19:31:33 $
 * $Revision: 1.1 $
 */

#ifndef _MSL_PTHREAD_STARTER_H
#define _MSL_PTHREAD_STARTER_H

#include <ansi_parms.h>

#if _MSL_PTHREADS

_MSL_BEGIN_EXTERN_C

	typedef int pthread_mutex_t;
	typedef int pthread_mutexattr_t;
	typedef int pthread_t;
	
	int pthread_mutex_init(pthread_mutex_t *, const pthread_mutexattr_t *);
	int pthread_mutex_destroy(pthread_mutex_t *);
	int pthread_mutex_lock(pthread_mutex_t *);
	int pthread_mutex_trylock(pthread_mutex_t);
	int pthread_mutex_unlock(pthread_mutex_t *);
	pthread_t pthread_self(void);
	int pthread_equal(pthread_t, pthread_t);
	
#if _MSL_LOCALDATA_AVAILABLE
	
	typedef int pthread_key_t;
	
	int pthread_key_create(pthread_key_t *, void (*)(void *));
	int pthread_key_delete(pthread_key_t);
	int pthread_setspecific(pthread_key_t, const void *);
	void *pthread_getspecific(pthread_key_t);
	
#endif /* _MSL_LOCALDATA_AVAILABLE */

_MSL_END_EXTERN_C

#endif /* _MSL_PTHREADS */

#endif /* _MSL_PTHREAD_STARTER_H */

/* Change record:
 * JWW 040315 Starter project - put your change log here, do not use mine!
 */