/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:33:26 $
 * $Revision: 1.63.2.2 $
 */

/* The math.c file has been deprecated. */
/* Please remove and replace with math_double.c, math_float.c and math_longdouble.c. */

#include "math_double.c"
#include "math_float.c"
#include "math_longdouble.c"


/* Change record: 
 * cc  020124 Needed for __fpclassifyf and other _MSL_INLINE items
 * JWW 020212 Removed the unnecessary __cdecl stuff
 * JWW 020308 Use new _MSL_DO_NOT_INLINE macro for bringing inlined functions into the library
 * mm  021108 Added erf, nan and gamma functions
 * mm  021120 Added nexttowardl()
 * cc  021121 Removed redefined of nexttowardl.  It lives as an inline in math_XXX.h.
 * cc  021216 Added cbrt and family for Michael
 * cc  030110 Fixed so it can be added to mac os and not affect math lib
 * mm  030128 Revised cbrt() and cbrtf() based on Howard's version
 * cc  030221 Added _MSL_USES_SUN_MATH_LIB flag
 * mm  030520 Added C99 wrappers
 * JWW 030321 Added fma, fmaf, and assortied helper functions (thanks to Howard)
 * mm  030522 Added C99 wrappers
 * mm  030529 Added modf() and modff()
 * mm  030605 Added ilogb() and ilogbl()
 * ejs 030613 Standardize the __builtin_xxx types
 * cc  031019 Added Michael's round, roundf,and roundl for sun math
 * cc  030924 Added Michael's lround, lroundf, lroundl, llround, llroundf, 
 *            and llroundl for sun math
 * cc  031003 Added Michael's lrint,lrint and lrintl for sun math
 * cc  031029 Added Michael's nearbyintl, nearbyintf, nearbyint, remquo,
 *            remquol, and remquof for sun math
 * cc  031029 Changed return types of nearbyint, nearbyintf, and nearbyintl
 * cc  040204 Changed float fracpart to long double fracpart in long double versions of 
 *			  nearbyint, lrint, lround, and llround. MTWX10358
 * hh  040410 Added several more intrinsics, changed implementation of __msl_generic_count_leading_zero32
 *            and __msl_generic_count_trailing_zero32
 * cc  040504 Added _MSL_FLOATING_POINT flag check
 * cc  040518 File has been split
 */