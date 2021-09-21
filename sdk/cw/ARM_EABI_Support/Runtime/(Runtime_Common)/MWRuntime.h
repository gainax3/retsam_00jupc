/* Metrowerks Runtime Library
 * Copyright © 1993-2005 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2005/08/22 17:01:06 $
 * $Revision: 1.4 $
 */

#ifndef __MW_RUNTIME_H__
#define __MW_RUNTIME_H__

#if defined(__POWERPC__) && (__MWERKS__ >= 0x2400)
	#define ARRAY_HEADER_SIZE 16
#else
	#include <stddef.h>
	#define ARRAY_HEADER_SIZE (2 * sizeof(size_t))
#endif

//
//	#define CABI_ZEROOFFSETVTABLE	1
//		
//	forces vtable pointer to point to the first virtual function pointer,
//	the RTTI header will be at offset -sizeof(RTTIVTableHeader)
//	This flag must be set in CABI.c and in MWRTTI.cp
//

#if defined(__INTEL__)
	#define CABI_ZEROOFFSETVTABLE	1
#elif defined(__IA64_CPP_ABI__) && __IA64_CPP_ABI__
	#define CABI_ZEROOFFSETVTABLE	1
#else
	#define CABI_ZEROOFFSETVTABLE	0
#endif

//
//	invisible constructor/destructor argument handling
//

#ifdef __INTEL__

#define CTORARG_TYPE			int
#define CTORARG_PARTIAL			(0)				//	construct non-virtual bases
#define CTORARG_COMPLETE		(1)				//	construct all bases	

#define CTORCALL_COMPLETE(ctor,objptr)\
	(((void (*)(void *,CTORARG_TYPE))ctor)(objptr,CTORARG_COMPLETE))

#define DTORARG_TYPE			int
#define DTORARG_DELETEFLAG		0x0001			//	delete after destruction
#define DTORARG_VECTORFLAG		0x0002			//	array destruction

#define DTORCALL_COMPLETE(dtor,objptr)\
	(((void (*)(void *,DTORARG_TYPE))dtor)(objptr,0))

#define DTORCALL_PARTIAL(dtor,objptr)\
	(((void (*)(void *))dtor)(objptr))

#else

#define CTORARG_TYPE			short
#define CTORARG_PARTIAL			(0)				//	construct non-virtual bases
#define CTORARG_COMPLETE		(1)				//	construct all bases	

#if __COLDFIRE__ && __REGABI__
#define CTORCALL_COMPLETE(ctor,objptr)\
    { asm {move.l  objptr, -(a7) }\
	  (((void (*)(CTORARG_TYPE))ctor)(CTORARG_COMPLETE));\
      asm {addq.l  #4, a7 }\
    }
#else
#define CTORCALL_COMPLETE(ctor,objptr)\
	(((void (*)(void *,CTORARG_TYPE))ctor)(objptr,CTORARG_COMPLETE))
#endif

#define DTORARG_TYPE			short
#define DTORARG_PARTIAL			(0)				//	destroy non-virtual bases	
#define DTORARG_COMPLETE		(-1)			//	destroy all bases	
#define DTORARG_DELETE			(1)				//	destroy all bases and delete object

#if __COLDFIRE__ && __REGABI__
static inline asm void dtor_call(void *dtor:__A0, void *objptr:__A1, long value:__D0)
    { move.l d0,-(a7)
      pea    (a1)
	  jsr    (a0)
      addq   #8, a7
    }
#define DTORCALL_COMPLETE(dtor,objptr) dtor_call(dtor, objptr,DTORARG_COMPLETE)
#define DTORCALL_PARTIAL(dtor,objptr)  dtor_call(dtor, objptr,DTORARG_PARTIAL)
#else
#define DTORCALL_COMPLETE(dtor,objptr)\
	(((void (*)(void *,DTORARG_TYPE))dtor)(objptr,DTORARG_COMPLETE))

#define DTORCALL_PARTIAL(dtor,objptr)\
	(((void (*)(void *,DTORARG_TYPE))dtor)(objptr,DTORARG_PARTIAL))

#define DTORCALL_VTTOBJECT(dtor,objptr,vttptr)\
	(((void (*)(void *,void*))dtor)(objptr,vttptr))
#endif

#endif /* __INTEL__ */

typedef void *ConstructorDestructor;	//	constructor/destructor function	pointer

#endif /* __MW_RUNTIME_H__ */

// JWW 040713 Pulled common runtime items out from CPlusLib.h