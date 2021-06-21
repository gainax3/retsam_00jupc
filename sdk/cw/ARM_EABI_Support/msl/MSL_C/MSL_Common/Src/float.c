/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/02/08 20:33:05 $
 * $Revision: 1.30.2.2 $
 */

/*
 *	Modification History
 *	--------------------
 *  rewritten by matt fassiotto on 7/30/97
 *  definitions were based on which architecture when
 *  they should have been based only on "endian-ness"
 *  which is now the case.
 *  This assumes both the "long" and "float" type are 
 *  32 bits whenever  compiled
 */

#include <ansi_parms.h>
#include <math.h>
#include <float.h> 

static unsigned long	unused = 0;				/* This is an alignment thing */

_INT32 __float_nan[]    ={0x7FFFFFFF};
_INT32 __float_huge[]   ={0x7f800000};

#if _MSL_DOUBLE_SIZE == 80 || _MSL_DOUBLE_SIZE == 96
	_INT32 __double_min	 	[]	= {0x00018000,0,0};
	_INT32 __double_max	 	[]	= {0x7FFEFFFF, 0xFFFFFFFF,0xFFFF0000 };
	_INT32 __double_epsilon	[]	= {0x3FC08000,0,0 } ;
	_INT32 __double_tiny	[]	= {0,0,0x00010000} ;
	_INT32 __double_huge	[]	= {0x7FFF0000,0,0 };
	_INT32 __double_nan	 	[]	= {0x7FFFFFFF,0xFFFFFFFF,0xFFFF0000 };
  
#elif _MSL_DOUBLE_SIZE == 64 && _MSL_LITTLE_ENDIAN
 
	_INT32 __double_min	  	[] ={0,0x00100000}; 
	_INT32 __double_max	  	[] ={0xffffffff,0x7fefffff};
	_INT32 __double_epsilon [] ={0,0x3cb00000};
	_INT32 __double_tiny	[] ={1,0};
	_INT32 __double_huge	[] ={0,0x7FF00000} ;
	_INT32 __double_nan	  	[] ={0xFFFFFFFF,0x7FFFFFFF};
 
#elif _MSL_DOUBLE_SIZE == 64 && !_MSL_LITTLE_ENDIAN

	_INT32 __double_min	     [] ={0x00100000 ,0}; 
	_INT32 __double_max	     [] ={0x7fefffff,0xffffffff};
	_INT32 __double_epsilon  [] ={0x3cb00000,0};
	_INT32 __double_tiny	 [] ={0,1};
	_INT32 __double_huge	 [] ={0x7FF00000,0};
	_INT32 __double_nan	     [] ={0x7FFFFFFF,0xFFFFFFFF};
  
#elif _MSL_DOUBLE_SIZE == 32
  
	_INT32 __double_min		[]			= { 0x00800000 };
	_INT32 __double_max		[]			= { 0x7F7FFFFF };
	_INT32 __double_epsilon []			= { 0x34000000 };
	_INT32 __double_tiny	[]			= { 1 };
	_INT32 __double_huge	[]			= { 0x7f800000};
	_INT32 __double_nan		[]			= { 0x7FFFFFFF };

	_INT32 *__huge_val = __double_huge;
	_INT32 *__nan_val  = __double_nan;
  
#endif /* _MSL_DOUBLE_SIZE  */

#if _MSL_LONG_DOUBLE_SIZE == 96

  _INT32 __extended_min	   []	= {0x00010000,0x80000000, 0};
  _INT32 __extended_max	   []	= {0x7FFE0000,0xFFFFFFFF,0xFFFFFFFF};
  _INT32 __extended_epsilon[]	= {0x3FC00000,0x80000000,0x00000000};
  _INT32 __extended_tiny   []	= {0,0,1};
  _INT32 __extended_huge   []	= {0x7FFF0000,0,0};
  _INT32 __extended_nan	   []	= {0x7FFF0000,0xFFFFFFFF,0xFFFFFFFF};
  
#elif _MSL_LONG_DOUBLE_SIZE == 80

  _INT32 __extended_min	   []	= {0x00018000,0,0};
  _INT32 __extended_max	   []	= {0x7FFEFFFF, 0xFFFFFFFF,0xFFFF0000 };
  _INT32 __extended_epsilon[]	= {0x3FC08000,0,0 } ;
  _INT32 __extended_tiny   []	= {0,0,0x00010000} ;
  _INT32 __extended_huge   []	= {0x7FFF0000,0,0 };
  _INT32 __extended_nan	   []	= {0x7FFFFFFF,0xFFFFFFFF,0xFFFF0000 };
  
#elif _MSL_LONG_DOUBLE_SIZE == 64 && _MSL_LITTLE_ENDIAN

   _INT32 __extended_min    []= {0,0x00100000} ;
   _INT32 __extended_max    []= {0xffffffff,0x7fefffff} ;
   _INT32 __extended_epsilon[]= {0,0x3cb00000} ;
   _INT32 __extended_tiny   []= {1,0};
   _INT32 __extended_huge   []= {0,0x7FF00000} ;
   _INT32 __extended_nan    []= {0xFFFFFFFF,0x7FFFFFFF} ;
  
#elif _MSL_LONG_DOUBLE_SIZE == 64 && !_MSL_LITTLE_ENDIAN
  
   _INT32 __extended_min    []= {0x00100000,0} ;
   _INT32 __extended_max    []= {0x7fefffff,0xffffffff} ;
   _INT32 __extended_epsilon[]= {0x3cb00000,0} ;
   _INT32 __extended_tiny   []= {0,1};
   _INT32 __extended_huge   []= {0x7FF00000,0} ;
   _INT32 __extended_nan    []= {0x7FFFFFFF,0xFFFFFFFF} ;    
    
#elif _MSL_LONG_DOUBLE_SIZE == 32

	_INT32 __extended_min	  []			= { 0x00800000 };
	_INT32 __extended_max	  []			= { 0x7F7FFFFF };
	_INT32 __extended_epsilon []			= { 0x34000000 };
 
#endif /* _MSL_LONG_DOUBLE_SIZE  */

#if _MSL_FLOAT_SIZE == 32	/*- cc 040910 -*/

    _INT32 __float_min    [] = {0x00800000};
    _INT32 __float_max    [] = {0x7F7FFFFF};
    _INT32 __float_epsilon[] = {0x34000000};

#endif /* _MSL_FLOAT_SIZE  */

/* Change record:
 * mf  980420 no longer need float constants.
 * mf  990907 only supporting 10 byte doubles on macos 68k
 *            e68k is 8 byte double for non-FPU, 12byte for FPU.
 * cc  011016 regrouped with new size flags
 * cc  040910 Changed 3 to 32
 */
