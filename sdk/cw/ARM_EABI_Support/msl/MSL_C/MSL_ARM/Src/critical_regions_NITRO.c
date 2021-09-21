/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/09 16:57:36 $
 * $Revision: 1.3.2.2 $
 */

#include <critical_regions.h>

#include <nitro/os/common/thread.h>
#include <nitro/os/common/mutex.h>

#if _MSL_THREADSAFE

	OSMutex __cs[num_critical_regions];
	int __cs_id[num_critical_regions];
	int __cs_ref[num_critical_regions];
	
	static void __msl_setup_criticals(void)
	{
		__init_critical_regions();
	}
	
	#ifdef __mwlinker__
		#pragma INIT_BEFORE_TERM_AFTER on
	#endif

#endif /* _MSL_THREADSAFE */

/* Change record:
 * JWW 020331 Initialize Mach-O regions on module bind and keep refcount info for locked threads
 * JWW 020423 Use new INIT_BEFORE_TERM_AFTER to setup critical regions first on Mach-O
 * jr  050118 remove CALL_ON_MODULE_BIND pragma
 */