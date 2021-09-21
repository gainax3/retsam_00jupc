/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:34:36 $
 * $Revision: 1.24.2.2 $
 */

/*
 *	Routines
 *	--------
 *	
 *	towctrans
 *	wctrans
 *
 */

#pragma ANSI_strict off

#include <ansi_parms.h>

#if _MSL_WIDE_CHAR
#if _MSL_C99									/*- mm 030305 -*/
#pragma ANSI_strict reset

#include <wctrans.h>
#include <string.h>
#include <wctype.h>

static const struct wctable 
{
	const char *string;
	wctype_t	value;
} 

wtable[] = 
{
	{"tolower", 0},
	{"toupper", 1},
	{(const char *)0, 0}
};

wint_t _MSL_CDECL towctrans(wint_t c, wctrans_t value)
{
	return (value == 1 ? towupper(c) : towlower(c));
}

wctrans_t _MSL_CDECL wctrans(const char *name)
{
	int	i;
	
	for(i=0; wtable[i].string != 0; ++i)
	{
		if(strcmp(wtable[i].string, name) == 0)
		{
			return (wtable[i].value);
		}
	
	}

	return 0;
}

#endif /* _MSL_C99 */					/*- mm 030305 -*/
#endif /* #if _MSL_WIDE_CHAR */        /*- mm 981030 -*/

/* Change record:
 * JCM 980126 JCM  First code release.
 * mm  981030 Added #if _MSL_WIDE_CHAR wrappers
 * mm  990809 Changed includes 
 * cc  011203 Added _MSL_CDECL for new name mangling 
 * JWW 030224 Changed __NO_WIDE_CHAR flag into the new more configurable _MSL_WIDE_CHAR
 * mm  030305 Added _MSL_C99 wrapper
 */