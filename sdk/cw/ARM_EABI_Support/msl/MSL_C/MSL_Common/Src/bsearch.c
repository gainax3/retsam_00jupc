/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:32:22 $
 * $Revision: 1.20.2.2 $
 */

/*
 *	Routines
 *	--------
 *		bsearch
 *		bsearch_s
 */

#define __STDC_WANT_SECURE_LIB__ 1
#include <stdint.h>
#include <stdlib.h>
#include <msl_secure_error.h>

#define table_ptr(i)	(((char *) table_base) + (member_size * (i)))

void * _MSL_CDECL bsearch(const void * key,
							 const void * table_base, size_t num_members, size_t member_size,
							 _compare_function compare_members)                   /*- mm 961031 -*/
{
	size_t	l, r, m;
	int			c;
	char *	mp;
	
	if (!key || !table_base || !num_members || !member_size || !compare_members)
		return(NULL);
	
	mp = table_ptr(0);
	
	c = compare_members(key, mp);							/* We have to make sure 'key' doesn't compare   */
															/* less than the first element in the table.    */
	if (c == 0)												/* As long as we're comparing, if it happens to */
		return(mp);											/* come out equal we'll forego discovering that */
	else if (c < 0)											/* all over again via the binary search.        */
		return(NULL);
	
	l = 1;
	r = num_members - 1;
	
	while (l <= r) {
		
		m = (l + r) / 2;
		
		mp = table_ptr(m);
		
		c = compare_members(key, mp);

		if (c == 0)
			return(mp);
		else if (c < 0)
			r = m - 1;
		else
			l = m + 1;
		
	}
	
	return(NULL);
}

void * _MSL_CDECL bsearch_s(const void * key,
							 const void * table_base, rsize_t num_members, rsize_t member_size,
							 _compare_function_s compare_members, void * context)
{
	rsize_t	l, r, m;
	int			c;
	char *	mp;
	
	if ((num_members > RSIZE_MAX) || (member_size > RSIZE_MAX) || ((num_members > 0) &&
		(key == NULL) || (table_base == NULL) || (compare_members == NULL)))
	{
		__msl_undefined_behavior_s();
		return(NULL);
	}
	
	if (!key || !table_base || !num_members || !member_size || !compare_members)
		return(NULL);
	
	mp = table_ptr(0);
	
	c = compare_members(key, mp, context);					/* We have to make sure 'key' doesn't compare   */
															/* less than the first element in the table.    */
	if (c == 0)												/* As long as we're comparing, if it happens to */
		return(mp);											/* come out equal we'll forego discovering that */
	else if (c < 0)											/* all over again via the binary search.        */
		return(NULL);
	
	l = 1;
	r = num_members - 1;
	
	while (l <= r) {
		
		m = (l + r) / 2;
		
		mp = table_ptr(m);
		
		c = compare_members(key, mp, context);

		if (c == 0)
			return(mp);
		else if (c < 0)
			r = m - 1;
		else
			l = m + 1;
		
	}
	
	return(NULL);
}

/* Change record:
 * JFH 950719 First code release.
 * JFH 960129 Fixed bug where the sense of the compare_members arguments was reversed.
 *			  Also made some small performance improvements (courtesy of Fabrizio Oddone).
 * JFH 960220 Fixed small bug in "performance improvement" where 'r' (which was *unsigned*)
 *			  could wrap from zero to ULONG_MAX. Rather than take the performance hit
 *			  back, num_members is constrained to fit in a *signed* long. If it doesn't,
 *			  we return NULL immediately.
 * JFH 960227 Fabrizio convinced me to undo the previous change and fix the bug in a
 *			  different way. We basically preflight to make sure the fatal condition
 *			  doesn't occur.
 * mm  961031 Improved legibility of function declaration
 * cc  011203 Added _MSL_CDECL for new name mangling 
 * JWW 031030 Added the __STDC_WANT_SECURE_LIB__ secure library extensions
 */