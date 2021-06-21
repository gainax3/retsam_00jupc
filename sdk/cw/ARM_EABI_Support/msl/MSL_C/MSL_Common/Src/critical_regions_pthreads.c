/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:32:23 $
 * $Revision: 1.8.2.2 $
 */

#include <ansi_parms.h>
#include <critical_regions.h>

#if _MSL_PTHREADS

#include <pthread.h>

	pthread_mutex_t __cs[num_critical_regions];
	
#if __MACH__
	static void __msl_setup_criticals(void)
	{
		__init_critical_regions();
	}
	#pragma CALL_ON_LOAD __msl_setup_criticals
	
	#ifdef __mwlinker__
		#pragma INIT_BEFORE_TERM_AFTER on
	#endif
#endif /* __MACH__ */

#endif /* _MSL_PTHREADS */

/* Change record:
 * JWW 020331 Initialize Mach-O regions on module bind and keep refcount info for locked threads
 * JWW 020423 Use new INIT_BEFORE_TERM_AFTER to setup critical regions first on Mach-O
 */