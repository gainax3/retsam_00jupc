#include "Profiler.h"

typedef unsigned long long *timeIntervalPtr;

asm void getTime( timeIntervalPtr time )
{
  mov    r2, r0
  mov    r0, #0x10
  mov    r1, #0
  swi    0x123456
#ifdef BIG_ENDIAN  
  str    r0, [ r2, #4 ]
  mov    r0, #0
  str    r0, [ r2, #0 ]
  bx     lr
#else
  str    r0, [ r2, #0 ]
  mov    r0, #0
  str    r0, [ r2, #4 ]
  bx     lr
#endif
}

long getTimerResolution()
{
	return 100;
}
