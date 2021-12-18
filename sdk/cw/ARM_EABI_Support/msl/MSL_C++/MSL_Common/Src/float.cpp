/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/09 17:02:18 $
 * $Revision: 1.14.2.2 $
 */

// float.cpp

#include <msl_t.h>

#if !_MSL_USING_MW_C_HEADERS

#ifdef __cplusplus
	extern "C" {
#endif

_INT32 __double_huge[]  = {0x7FF00000, 0};
_INT32 __float_nan[]    = {0x7FFFFFFF};
_INT32 __float_huge[]   = {0x7f800000};

#ifdef __cplusplus
	}
#endif

#endif  // !_MSL_USING_MW_C_HEADERS
