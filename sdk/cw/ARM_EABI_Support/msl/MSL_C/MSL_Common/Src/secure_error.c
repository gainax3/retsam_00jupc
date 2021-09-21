/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:34:03 $
 * $Revision: 1.2.2.2 $
 */

#define __STDC_WANT_SECURE_LIB__ 1
#include <ansi_parms.h>
#include <stdlib.h>
#include <msl_secure_error.h>

static __msl_secure_user_fn_ptr __user_fn_ptr = NULL;

void _MSL_CDECL __msl_undefined_behavior_s(void)
{
	if (__user_fn_ptr)
		(*__user_fn_ptr)();
}

void _MSL_CDECL __msl_set_secure_undefined(__msl_secure_user_fn_ptr new_user_fn_ptr)
{
	__user_fn_ptr = new_user_fn_ptr;
}

__msl_secure_user_fn_ptr _MSL_CDECL __msl_get_secure_undefined(void)
{
	return __user_fn_ptr;
}

/* Change record:
 * JWW 050111 New file to handle diagnosed undefined behavior in secure library routines
 */