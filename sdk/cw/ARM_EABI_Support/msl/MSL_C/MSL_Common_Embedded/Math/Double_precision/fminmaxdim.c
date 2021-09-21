/* $Id: fminmaxdim.c,v 1.6 2004/12/02 12:38:50 cgalvan Exp $ */

#include "fdlibm.h"
#if _MSL_FLOATING_POINT
#if _MSL_C99

#include <math.h>
_MSL_IMP_EXP_C double _MSL_MATH_CDECL fmax ( double x, double y ) _MSL_CANT_THROW
 {
 if(x >= y) return x;
 else if(x < y) return y;
 else
 { /* since x is neither ge or l y they are unordered */
  if(isnan(x)) return y;
  else return x;
  }
 }
_MSL_IMP_EXP_C double _MSL_MATH_CDECL fmin ( double x, double y ) _MSL_CANT_THROW
 {
 if(x >= y) return y;
 else if(x < y) return x;
 else
 { /* since x is neither ge or l y they are unordered */
  if(isnan(x)) return y;
  else return x;
  }
 }
_MSL_IMP_EXP_C double _MSL_MATH_CDECL fdim ( double x, double y ) _MSL_CANT_THROW
{
 if(x >= y) return (x-y);
 else if(x < y) return 0.0;
 else
 {
 if(isnan(x)) return x;
 else return y;
 }
}
#endif /* _MSL_C99 */
#endif /* _MSL_FLOATING_POINT */
