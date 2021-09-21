/* Metrowerks Standard Library
 * Copyright © 1995-2003 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2003/05/28 16:11:00 $
 * $Revision: 1.19 $
 */

/*
 *	Routines
 *	--------
 *		__init_critical_regions
 *		__kill_critical_regions
 *		__begin_critical_region
 *		__end_critical_region
 */

#ifndef _MSL_CRITICAL_REGIONS_MACOS_H
#define _MSL_CRITICAL_REGIONS_MACOS_H

#if __MACH__

#include <msl_thread_local_data.h>
#include <pthread.h>

_MSL_BEGIN_EXTERN_C

	_MSL_IMP_EXP_C extern pthread_mutex_t __cs[num_critical_regions];
	extern pthread_t __cs_id[num_critical_regions];
	extern int __cs_ref[num_critical_regions];
	
	_MSL_INLINE void __init_critical_regions(void) _MSL_CANT_THROW
	{
		int i;
		
		for (i = 0; i < num_critical_regions; i++)
			pthread_mutex_init(&__cs[i], 0);
		
		__msl_InitializeMainThreadData();
	}
	
	_MSL_INLINE void __kill_critical_regions(void) _MSL_CANT_THROW
	{
		int i;
		
		__msl_DisposeAllThreadData();
		
		for (i = 0; i < num_critical_regions; i++)
			pthread_mutex_destroy(&__cs[i]);
	}
	
	_MSL_INLINE void __begin_critical_region(int region) _MSL_CANT_THROW
	{
		/* Attempt to get the lock, but do not block if lock cannot be acquired */
		if (pthread_mutex_trylock(&__cs[region]) == 0)
		{
			/* Lock was acquired */
			__cs_id[region] = pthread_self();
			__cs_ref[region] = 1;
		}
		else
		{
			/* Lock was not acquired, somebody else already has the mutex locked */
			if (pthread_equal(pthread_self(), __cs_id[region]))
				/* ...but the lock was already acquired by the current thread */
				__cs_ref[region]++;
			else
			{
				/* The lock is in another thread, so block until it is acquired */
				pthread_mutex_lock(&__cs[region]);
				__cs_id[region] = pthread_self();
				__cs_ref[region] = 1;
			}
		}
	}
	
	_MSL_INLINE void __end_critical_region(int region) _MSL_CANT_THROW
	{
		/* Don't unlock until reference count goes to zero */
		if (--__cs_ref[region] == 0)
			pthread_mutex_unlock(&__cs[region]);
	}

_MSL_END_EXTERN_C

#else

#include <Multiprocessing.h>

_MSL_BEGIN_EXTERN_C	/*- cc 010410 -*/

	_MSL_IMP_EXP_C extern MPCriticalRegionID __cs[num_critical_regions];

	_MSL_INLINE void __init_critical_regions(void) _MSL_CANT_THROW
	{
		int i;
		if ((__cs[0] == NULL) && MPLibraryIsLoaded())
		{
			for (i = 0; i < num_critical_regions; i++)
				MPCreateCriticalRegion(&__cs[i]);
		}
	}

	_MSL_INLINE void __kill_critical_regions(void) _MSL_CANT_THROW
	{
		int i;
		for (i = 0; i < num_critical_regions; i++)
			MPDeleteCriticalRegion(__cs[i]);
	}

	_MSL_INLINE void __begin_critical_region(int region) _MSL_CANT_THROW
	{
		MPEnterCriticalRegion(__cs[region], 0x7FFFFFFF);	
	}

	_MSL_INLINE void __end_critical_region(int region) _MSL_CANT_THROW
	{
		MPExitCriticalRegion(__cs[region]);
	}

_MSL_END_EXTERN_C	/*- cc 010410 -*/

#endif /* __MACH__ */

#endif /* _MSL_CRITICAL_REGIONS_MACOS_H */

/* Change record:
 * JFH 951016 First code release.
 * hh  990804 Filled out for #if _MWMT
 * hh  990831 Added undefs for __init_critical_regions, __kill_critical_regions
 * cc  010410 updated to new namespace macros
 * JWW 011012 Added MPLibraryIsLoaded() call to __init_critical_regions to make sure MP is ready
 * JWW 011128 Added Mach-O support
 * JWW 020331 Use refcount for locked Mach-O threads to keep same-thread relocking from deadlocking
 * hh  020603 Added no throw spec to functions
 * JWW 021031 Take thread local data structures into account for Mach-O
 * JWW 030528 Use _MSL_INLINE instead of __inline to get inlining without declaring functions static
 */