/* Metrowerks Standard Library
 * Copyright © 1995-2004 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2004/04/16 19:31:35 $
 * $Revision: 1.1 $
 */

#include <ansi_parms.h>

#if _MSL_PTHREADS

#include <pthread.h>
#include <errno.h>

/* !!! Extremely bad implementation -- use platform specific mutex calls whenever possible !!! */
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
	if (attr != NULL)
		return EINVAL;
	
	*mutex = -1;
	
	return 0;
}

/* !!! Extremely bad implementation -- use platform specific mutex calls whenever possible !!! */
int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
	if ((*mutex != 0) || (*mutex != -1))
		return EINVAL;
	
	*mutex = -2;
	
	return 0;
}

/* !!! Extremely bad implementation -- use platform specific mutex calls whenever possible !!! */
int pthread_mutex_lock(pthread_mutex_t *mutex)
{
	volatile pthread_mutex_t *real_mutex;
	
	if ((*mutex != 0) || (*mutex != -1))
		return EINVAL;
	
	if (*mutex == 0)
	{
		real_mutex = mutex;
		
		while (*real_mutex == 0) {}
	}
	
	*mutex = 0;
	
	return 0;
}

/* !!! Extremely bad implementation -- use platform specific mutex calls whenever possible !!! */
int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
	if ((*mutex != 0) || (*mutex != -1))
		return EINVAL;
	
	if (*mutex == 0)
		return EBUSY;
	
	*mutex = 0;
	
	return 0;
}

/* !!! Extremely bad implementation -- use platform specific mutex calls whenever possible !!! */
int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
	if ((*mutex != 0) || (*mutex != -1))
		return EINVAL;
	
	if (*mutex == -1)
		return EPERM;
	
	*mutex = -1;
	
	return 0;
}

/* !!! Extremely bad implementation -- use platform specific mutex calls whenever possible !!! */
pthread_t pthread_self(void)
{
	return 0;
}

/* !!! Extremely bad implementation -- use platform specific mutex calls whenever possible !!! */
int pthread_equal(pthread_t t1, pthread_t t2)
{
	return (t1 == t2);
}

#if _MSL_LOCALDATA_AVAILABLE

/* !!! MUST USE PLATFORM SPECIFIC CALLS HERE TO IMPLEMENT THREAD LOCAL DATA AREAS !!! */
/* !!! IF THE PLATFORM CANNOT SUPPORT THREAD LOCAL DATA, TURN OFF _MSL_LOCALDATA_AVAILABLE !!! */

int pthread_key_create(pthread_key_t *key, void (*destructor)(void *))
{
#pragma unused(key, destructor)
	return 0;
}

int pthread_key_delete(pthread_key_t key)
{
#pragma unused(key)
	return 0;
}

int pthread_setspecific(pthread_key_t key, const void *value)
{
#pragma unused(key, value)
	return 0;
}

void *pthread_getspecific(pthread_key_t key)
{
#pragma unused(key)
	return NULL;
}

#endif /* _MSL_LOCALDATA_AVAILABLE */

#endif /* _MSL_PTHREADS */

/* Change record:
 * JWW 040315 Starter project - put your change log here, do not use mine!
 */