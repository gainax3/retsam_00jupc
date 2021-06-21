/* MSL
 * Copyright © 1995-2006 Freescale Corporation.  All rights reserved.
 *
 * $Date: 2006/06/16 15:40:15 $
 * $Revision: 1.71.2.3 $
 */

#include <ansi_parms.h>

#if defined(__m56800__) || defined(_MSL_BAREBOARD_MALLOC)
	#if _MSL_OS_ALLOC_SUPPORT
		#undef _MSL_OS_ALLOC_SUPPORT
		#define _MSL_OS_ALLOC_SUPPORT 0
	#endif
#endif

#if !_MSL_OS_ALLOC_SUPPORT
	
	#ifndef _MSL_HEAP_EXTERN_PROTOTYPES
		#error _MSL_HEAP_EXTERN_PROTOTYPES must be defined in the platform prefix file!
	#endif
	
	#ifndef _MSL_HEAP_START
		#error _MSL_HEAP_START must be defined in the platform prefix file!
	#endif
	
	#ifndef _MSL_HEAP_SIZE
		#error _MSL_HEAP_SIZE must be defined in the platform prefix file!
	#endif
	
	_MSL_HEAP_EXTERN_PROTOTYPES
	
#endif /* _MSL_OS_ALLOC_SUPPORT */

#ifdef _MSL_OS_DIRECT_MALLOC
#pragma mark -
#pragma mark _MSL_OS_DIRECT_MALLOC
#pragma mark -

#include <pool_alloc.h>
#include <string.h>
#include <stdlib.h>

	void* _MSL_CDECL
	malloc(size_t size)
	{
		if (size == 0)
		#ifdef _MSL_MALLOC_0_RETURNS_NON_NULL
			++size;
		#else
			return 0;
		#endif
		return __sys_alloc(size);
	}

	void _MSL_CDECL
	free(void* ptr)
	{
		if (ptr != 0)
			__sys_free(ptr);
	}

	void* _MSL_CDECL
	realloc(void* ptr, size_t size)
	{
		size_t orig_size;
		void* p;
		if (ptr == 0)
			return malloc(size);
		if (size == 0)
		{
			free(ptr);
			return 0;
		}
		orig_size = __sys_pointer_size(ptr);
		p = malloc(size);
		if (p != 0)
		{
			orig_size = orig_size < size ? orig_size : size;
			memcpy(p, ptr, orig_size);
			free(ptr);
		}
		return p;
	}

	void* _MSL_CDECL
	calloc(size_t nmemb, size_t size)
	{
		void* result;

		size *= nmemb;
		result = malloc(size);
		if (result != 0)
			memset(result, 0, size);
		return result;
	}

	void _MSL_CDECL
	__pool_free_all(__mem_pool* x)
	{
		#pragma unused(x)
	}

	void _MSL_CDECL
	__malloc_free_all(void)
	{
	}

	size_t _MSL_CDECL _MSL_ALLOCATE_SIZE(void* ptr)
	{
		return __sys_pointer_size(ptr);
	}

	void* _MSL_CDECL _MSL_ALLOCATE(size_t size_requested, size_t* size_received)
	{
		void* result = 0;
		if (size_requested)
			result = malloc(size_requested);
		if (size_received)
		{
			if (result)
				*size_received = _MSL_ALLOCATE_SIZE(result);
			else
				*size_received = 0;
		}
		return result;
	}

	int _MSL_CDECL _MSL_ALLOCATE_RESIZE(void* ptr, size_t size_requested, size_t* size_received)
	{
		size_t orig_size = _MSL_ALLOCATE_SIZE(ptr);
		if (size_received)
			*size_received = orig_size;
		return orig_size == size_requested;
	}

	int _MSL_CDECL _MSL_ALLOCATE_EXPAND(void* ptr, size_t min_size, size_t preferred_size, size_t* size_received)
	{
		int result = 0;
		size_t sr_backup;
		size_t* srp = &sr_backup;
		if (size_received)
			srp = size_received;
		while (min_size <= preferred_size)
		{
			result = _MSL_ALLOCATE_RESIZE(ptr, preferred_size, srp);
			if (result)
				break;
			preferred_size = *srp;
		}
		return result;
	}

#elif defined (_MSL_CLASSIC_MALLOC) || defined (_MSL_PRO4_MALLOC)
#pragma mark -
#pragma mark _MSL_CLASSIC_MALLOC
#pragma mark -

#ifdef _MSL_PRO4_MALLOC
	#pragma ANSI_strict off
	#warning The flag _MSL_PRO4_MALLOC is being changed to _MSL_CLASSIC_MALLOC
	#warning _MSL_PRO4_MALLOC still works but please migrate to _MSL_CLASSIC_MALLOC
	#warning _MSL_CLASSIC_MALLOC now refers to what used to be called _MSL_PRO4_MALLOC
	#pragma ANSI_strict reset
#endif /* _MSL_PRO4_MALLOC */

/*
 *	alloc.c
 *	
 *	Routines
 *	--------
 *		init_alloc			Initialize allocs' memory pool to be fixed size and location
 *		GetHeapSymbols		get linker defined symbols for embedded
 *		init_default_heap	initialize the default heap
 *		malloc				Allocate memory
 *		calloc				Allocate and clear memory
 *		realloc				
 *		free				Return memory block to allocs' memory pool
 */

#include <ansi_parms.h>                 /*- mm 970904 -*/
#include "critical_regions.h"
#include <stdlib.h>
#include "pool_alloc.h"

#ifdef __ALTIVEC__
mem_pool_obj	__vec_malloc_pool;
static int		vec_initialized = 0;
#endif /* __ALTIVEC__ */
mem_pool_obj	__malloc_pool;
static int		initialized = 0;

#if !_MSL_OS_ALLOC_SUPPORT				/*- mm 981015 -*/
mem_pool_obj *  __malloc_pool_ptr = &__malloc_pool;        

static int	defaultheapinitialized = 0;

int init_alloc(void * pool_ptr, mem_size size);

/***************************************************************************
 *	init_alloc
 *		Initializes the memory pool that malloc uses. This call is designed to
 *		be used for systems that DO NOT have memory allocation functions
 *  	built into the system. The first call will define the pool and use the
 *		rest of the space as a block of memory. Subsequent calls will add the
 *		memory as another block of memory to the malloc's existing pool. Once
 *		a heap of memory is allocated to the malloc memory pool, it can not 
 *		be returned.
 *
 *	Inputs:
 *		pool_ptr		pointer to memory to be used in malloc's memory pool
 *		size				size of memory to be used in (or added to) malloc's memory pool
 *
 *	Outputs:
 *		None
 *
 *	Returns: int
 *		0		Fail
 *		1		Success: Memory initialized as malloc's memory pool
 *		2		Success: Memory added to initial heap successfully
 *
 *  NOTE: 
 *		This function must be called prior to using other alloc.c functions
 *
 ***************************************************************************/
extern int _MSL_CDECL init_alloc(void * pool_ptr, mem_size size)
{
	if (!initialized) {
 	
	/*************************************************************************** 
	* Memory not initialized, make a pool and use the rest of the room for
	*	memory blocks
	***************************************************************************/
		/* Size of pool overhead */
		size_t pool_overhead	=	sizeof(mem_pool_obj);
		
		/* Remove pool overhead from size */
		size -= pool_overhead; 
		
		/* Point malloc pool at memory address provided */
		__malloc_pool_ptr = (mem_pool_obj *)pool_ptr;
	
		/* Create pool structure at memory address provided */
		__init_pool_obj(__malloc_pool_ptr);
		
		/* Assign the rest of the space (size - pool_overhead) to a memory block  */
		__pool_preassign(__malloc_pool_ptr, (void *)((size_t)pool_ptr+pool_overhead), size);
		
		/* Mark memory as initialized */
		initialized = 1;
		
		/* Return successful pool initialization value */
		return(1);
		
	} else {
	/***************************************************************************
	 * Memory already initialized, use all memory space as a memory block
	 ***************************************************************************/
		__pool_preassign(__malloc_pool_ptr, (void *)pool_ptr, size);
		
		/* Return successful heap addition value */
		return(2);
	}
}

void init_default_heap(void);
void init_default_heap(void)
{
	defaultheapinitialized = 1;
	
	if (_MSL_HEAP_SIZE != 0)
		init_alloc((void *) _MSL_HEAP_START, (size_t) _MSL_HEAP_SIZE);
}

#endif /* _MSL_OS_ALLOC_SUPPORT */				/*- mm 981015 -*/


void * _MSL_CDECL malloc(size_t size)
{
	void *	block;
	
	__begin_critical_region(malloc_pool_access);
	
#if _MSL_OS_ALLOC_SUPPORT					/*- mm 981015 -*/
	if (!initialized)
	{
		__init_pool_obj(&__malloc_pool);
		initialized = 1;
	}
	block = __pool_alloc(&__malloc_pool, size);
	
#else
	if (!defaultheapinitialized)			/*- mm 981015 -*/
	{										/*- mm 981015 -*/
		init_default_heap();				/*- mm 981015 -*/
	}
	if (!initialized)						/*- vss 990121 -*/
	{
		return(0);
	}
	block = __pool_alloc(__malloc_pool_ptr, size);
#endif /* _MSL_OS_ALLOC_SUPPORT */
	
	__end_critical_region(malloc_pool_access);
	
	return(block);
}

void * calloc(size_t nmemb, size_t size)
{
	void *	block;
	
	__begin_critical_region(malloc_pool_access);
	
#if _MSL_OS_ALLOC_SUPPORT										/*- mm 981015 -*/
	if (!initialized)
	{
		__init_pool_obj(&__malloc_pool);
		initialized = 1;
	}
	block = __pool_alloc_clear(&__malloc_pool, nmemb*size);
	
#else															/*- mm 981015 -*/
	if (!defaultheapinitialized)								/*- mm 981015 -*/
	{															/*- mm 981015 -*/
		init_default_heap();									/*- mm 981015 -*/
	}															/*- mm 981015 -*/
	if (!initialized)											/*- mm 981015 -*/
	{															/*- mm 981015 -*/
		return(0);												/*- mm 981015 -*/
	}															/*- mm 981015 -*/
	block = __pool_alloc_clear(__malloc_pool_ptr, nmemb*size);	/*- mm 981015 -*/
#endif /* _MSL_OS_ALLOC_SUPPORT */								/*- mm 981015 -*/
	
	__end_critical_region(malloc_pool_access);
	
	return(block);
}

void * realloc(void * ptr, size_t size)
{
	void *	block;
	
	__begin_critical_region(malloc_pool_access);
	
#if _MSL_OS_ALLOC_SUPPORT									/*- mm 981015 -*/
	if (!initialized)
	{
		__init_pool_obj(&__malloc_pool);
		initialized = 1;
	}
	
	block = __pool_realloc(&__malloc_pool, ptr, size);
	
#else														/*- mm 981015 -*/
	if (!defaultheapinitialized)							/*- mm 981015 -*/
	{														/*- mm 981015 -*/
		init_default_heap();								/*- mm 981015 -*/
	}														/*- mm 981015 -*/
	if (!initialized)										/*- mm 981015 -*/
	{														/*- mm 981015 -*/
		return(0);											/*- mm 981015 -*/
	}														/*- mm 981015 -*/
	block = __pool_realloc(__malloc_pool_ptr, ptr, size);	/*- mm 981015 -*/
#endif /* _MSL_OS_ALLOC_SUPPORT */							/*- mm 981015 -*/

	__end_critical_region(malloc_pool_access);
	
	return(block);
}

void free(void * ptr)
{
	if (!initialized)
		return;
	
	__begin_critical_region(malloc_pool_access);
	
#if _MSL_OS_ALLOC_SUPPORT			 						/*- mm 981015 -*/								
	__pool_free(&__malloc_pool, ptr);
	
#else														/*- mm 981015 -*/
	__pool_free(__malloc_pool_ptr, ptr);					/*- mm 981015 -*/
#endif /* _MSL_OS_ALLOC_SUPPORT */							/*- mm 981015 -*/
	
	__end_critical_region(malloc_pool_access);
}

size_t __msize(void * ptr)
{
	/* clear lower two bits to remove pool_alloc flags */
	return (*((size_t *) ptr - 1) & ~0x03) - 2*sizeof(unsigned long);  /*- hh 020211 -*/
}


#ifdef __ALTIVEC__
/*
 * AltiVec memory routines which deal with 16-byte aligned blocks
 *
 */
 
void* vec_malloc(size_t size);
void* vec_calloc(size_t nmemb, size_t size);
void* vec_realloc(void* ptr, size_t size);
void vec_free(void* ptr);

void * vec_malloc(size_t size)
{
	void *	block;
	
	__begin_critical_region(malloc_pool_access);
	
	if (!vec_initialized)
	{
		__init_align_pool_obj(&__malloc_pool, 16);
		vec_initialized = 1;
	}
	
	block = __pool_alloc(&__malloc_pool, size);
	
	__end_critical_region(malloc_pool_access);
	
	return(block);
}

void * vec_calloc(size_t nmemb, size_t size)
{
	void *	block;
	
	__begin_critical_region(malloc_pool_access);
	
	if (!vec_initialized)
	{
		__init_align_pool_obj(&__malloc_pool, 16);
		vec_initialized = 1;
	}
	
	block = __pool_alloc_clear(&__malloc_pool, nmemb*size);
	
	__end_critical_region(malloc_pool_access);
	
	return(block);
}

void * vec_realloc(void * ptr, size_t size)
{
	void *	block;
	
	__begin_critical_region(malloc_pool_access);
	
	if (!vec_initialized)
	{
		__init_align_pool_obj(&__malloc_pool, 16);
		vec_initialized = 1;
	}
	
	block = __pool_realloc(&__malloc_pool, ptr, size);
	
	__end_critical_region(malloc_pool_access);
	
	return(block);
}

void vec_free(void * ptr)
{
	if (!vec_initialized)
		return;
	
	__begin_critical_region(malloc_pool_access);
	
	__pool_free(&__malloc_pool, ptr);
	
	__end_critical_region(malloc_pool_access);
}
#endif /* __ALTIVEC__ */


/* Change record:
 * JFH 950320 First code release.
 * JFH 950609 Changed malloc_pool to __malloc_pool as per standard naming conventions.
 * mm  970708 Inserted Be changes
 * MEA 970720 changed __no_os to _No_Alloc_OS_Support and  __ppc_eabi_bare to __ppc_eabi.
 * mm  970904 Added include ansi_parms.h  to allow compilation without prefix
 * mm  981015 Added code for _No_Alloc_OS_Support and __DSP568
 * mm  981029 Changed __DSP568 to __m56800__
 * mf  981115 fixed mm change of 981015
 * ad  990128 added nintendo support
 * blc 990209 added __msize
 * bobc990302 added vec_malloc (etc) for AltiVec
 * vss 990624 add mcore
 * vss 990624 fixed typos in embedded platform code
 * as  990824 added SH
 * beb 990830 changed _heap_size and _heap_addr to be long vars
 * scm 991129 changed __heap_size and __heap_addr to be 'extern char []'
 * hh  000523 Added _MSL_OS_DIRECT_MALLOC implementation
 * mf  000707 ( & hh) misplaced #endif and added readability to #ifdefs
 * cc  000326 removed dest_os to be_os 
 * hh  020214 Renamed _MSL_PRO4_MALLOC to _MSL_CLASSIC_MALLOC
 * JWW 040401 Changed _No_Alloc_OS_Support to _MSL_OS_ALLOC_SUPPORT
 */

#elif defined(__m56800__) || defined(_MSL_BAREBOARD_MALLOC)
#pragma mark -
#pragma mark _MSL_BAREBOARD_MALLOC
#pragma mark -

#include <stdlib.h>
#include <limits.h>
#include <string.h>

#undef NDEBUG

#include <assert.h>

static unsigned char* __heap = (unsigned char*)_MSL_HEAP_START;

#ifndef _MSL_ALIGNMENT
	#define _MSL_ALIGNMENT sizeof(void*)
#endif

static const size_t allocate_mask = _MSL_ALIGNMENT == 1 ? (size_t)(-1) << (CHAR_BIT*sizeof(size_t)-1) : 1;

#define align_up(size) ((__typeof__(size))((size_t)size + (_MSL_ALIGNMENT-1) & ~(_MSL_ALIGNMENT-1)))
#define align_down(size) ((__typeof__(size))((size_t)size & ~(_MSL_ALIGNMENT-1)))
#define is_allocated(p) ((size_t)(p) & allocate_mask)
#define ptr(p) ((__typeof__(p))((size_t)(p) & ~allocate_mask))
#define array_size() ((size_t*)(__heap + _MSL_HEAP_SIZE) - 1)
#define array(i) ((unsigned char**)(__heap + _MSL_HEAP_SIZE) - ((i)+2))

#ifndef _MSL_MALLOC_TEST
static
#endif
int defaultheapinitialized = 0;

/*
	static void init_default_heap(void)

	This private function is called to initialize the heap, on the first call
	to __allocate.  It does a run-time check of a few underlying assumptions:

	1.  _MSL_ALIGNMENT must be a positive multiple of sizeof(void*).
	2.  The heap starting address must be properly aligned.
	3.  The heap size must be a non-negative multiple of _MSL_ALIGNMENT.
	4.  If _MSL_ALIGNMENT is 1, no pointer in the heap can have a non-zero
	    most significant bit.

	An array of pointers is created at the high end of the heap.  The length
	of this array is initialized to 2.  The array grows downward instead of
	upwards, with the 0th element at the highest address in the array.  The first
	pointer is set to the beginning of the heap, and implicitly marked free.
	The second pointer points to itself, or the next lowest address which is
	a multiple of _MSL_ALIGNMENT.  This second pointer is marked allocated.

	A file scope flag indicating the heap has been initialized is set.

	+---+---+---+---+       +---+---+---+---+---+---+
	|   |   |   |   |  ...  |   |   |   | e | p | 2 | 
	+---+---+---+---+		+---+---+---+---+---+---+
	  ^                                   ^   |
	  |                                   |   |
	  |                                       |
	  +---------------------------------------+

	If _MSL_HEAP_SIZE falls below a size which it would be impossible to set up the
	heap overhead and still have a free block, init_default_heap will do nothing.
	Clients must check for this condition by inspecting defaultheapinitialized after
	the call.
*/

#ifndef _MSL_MALLOC_TEST
static
#else
void init_default_heap(void);
#endif
void init_default_heap(void)
{
	if (_MSL_HEAP_SIZE >= align_up(3*sizeof(void*)) + align_up(sizeof(void*)))
	{
		unsigned char** p;
		assert(_MSL_ALIGNMENT >= sizeof(void*));
		assert(_MSL_ALIGNMENT % sizeof(void*) == 0);
		assert((size_t)__heap % _MSL_ALIGNMENT == 0);
		assert(_MSL_HEAP_SIZE % _MSL_ALIGNMENT == 0);
		if (_MSL_ALIGNMENT == 1)
			assert((((size_t)__heap + _MSL_HEAP_SIZE) & ((size_t)(-1) << (CHAR_BIT*sizeof(size_t)-1))) == 0);
		*array_size() = 2;
		*array(0) = __heap;
		p = array(1);
		*p = align_down((unsigned char*)p);
		*(size_t*)p |= allocate_mask;
		defaultheapinitialized = 1;
	}
}

/*
	unsigned char** insert(unsigned char** e, unsigned char* value, unsigned char** p, size_t* nblocks)

	This private function attempts to insert a new pointer just after p (at p-1), And
	give it the value value.  It moves all pointers after p down by one.  The pointer
	e marks the end of the array.  nblocks points to the size of the array which will
	be incremented if necessary.

	If the insert is successful, a pointer to the newly inserted pointer is returned,
	else null is returned.  To be successful, there must be room for the array to grow
	by one.  For this to be true, either the end of the array currently points to a
	lower address than itself (perhaps for alignment reasons), or the final block in
	the array has not been allocated, or a new free block is being inserted at the
	end of the array, and thus the array can grow into it.

	If the growing array grows into the last free block, it may either split the last
	free block, taking only _MSL_ALIGNMENT bytes, or if the last free block is only
	_MSL_ALIGNMENT big, will completely allocate this last free block for the array
	use.  In this latter case, the size of the array doesn't actually grow since the
	last free block is absorbed into the overhead (array) block.
*/

#ifndef _MSL_MALLOC_TEST
static
#else
unsigned char** insert(unsigned char** e, unsigned char* value, unsigned char** p, size_t* nblocks);
#endif
unsigned char** insert(unsigned char** e, unsigned char* value, unsigned char** p, size_t* nblocks)
{
	unsigned char** n = 0;
	if (e != (unsigned char**)ptr(*e) || !is_allocated(*(e+1)) || e == p-1)
	{
		--e;
		*e = align_down((unsigned char*)e);
		for(n = e + 1; n < p-1; ++n)
			*n = *(n+1);
		*(p-1) = value;
		if (*e == *(e+1))
			*(size_t*)(e+1) |= allocate_mask;
		else
		{
			*(size_t*)e |= allocate_mask;
			++*nblocks;
		}
	}
	return n;
}

/*
	static void erase(unsigned char** e, unsigned char** i, size_t* nblocks)

	This private function erases the pointer i from the array.  e marks the
	end of the array, and nblocks points to the array size.

	If the last (non-overhead) block is free, the process is quite simple:
	The pointer is simply erased by moving all of the pointers beyond i up
	by one, and decreasing the array size by one.  If the end pointer changes
	in value by moving over an alignment boundary, the last free block implicitly
	grows in size by _MSL_ALIGNMENT bytes.

	If the last (non-overhead) block is allocated, care must be taken to
	ensure the size of this allocated block does not change as the array
	shrinks.  To achieve this the erase function attempts to insert a free
	block after the last allocated block.  However this has the effect that the 
	array size does not shrink since you now need an extra pointer to point
	to the newly created free block.  But if the end pointer is currently taking
	up the space for the newly created free block, you've got a problem.

	To get around the problem of the end pointer sitting in the newly created free
	block, the erase function simply does not create the new free block, moves all
	of the pointers up by one, but has the end pointer continue to point to the
	old overhead block.  I.e. the overhead block does not shrink in size, and thus
	the last allocated block does not grow in size.

	If the end pointer already does not sit in memory big enough to create a free
	block (_MSL_ALIGNMENT bytes), then by erasing a pointer you make room for a
	new pointer to this new free block.  In this case the size of the array does
	not actually decrease.  The ith pointer is deleted, and simulateously a new
	pointer to the new last free block is created.  The end pointer doesn't move
	but is reset to point to itself, instead of into the next lowest aligned block.
	In this case, the end pointer will always be on an aligned boundary, but the
	align_down macro is used anyway just for consistency.
*/

#ifndef _MSL_MALLOC_TEST
static
#else
void erase(unsigned char** e, unsigned char** i, size_t* nblocks);
#endif
void erase(unsigned char** e, unsigned char** i, size_t* nblocks)
{
	if (!is_allocated(*(e+1)))
	{
		for (++e; i > e; --i)
			*i = *(i-1);
		*i = align_down((unsigned char*)i);
		*(size_t*)i |= allocate_mask;
		--*nblocks;
	}
	else if ((size_t)((unsigned char*)e - ptr(*e)) < _MSL_ALIGNMENT)
	{
		for (; i > e; --i)
			*i = *(i-1);
		--*nblocks;
	}
	else
	{
		for (++e; i > e; --i)
			*i = *(i-1);
		*e = ptr(*(e-1));
		--e;
		*e = align_down((unsigned char*)e);
		*(size_t*)e |= allocate_mask;
	}
}

/*
	static void erase2(unsigned char** e, unsigned char** i, size_t* nblocks)

	The erase2 function is very similar to the erase function.  It erases the
	pointers at i and at i-1.  It has the same issues as erase does concerning
	the situation when the last non-overhead block is allocated.  When it adds
	a pointer to point to a newly created last free block, erase2 only decrements
	the array size by 1.

	When _MSL_ALIGNMENT is 1, one of the possibilities is eliminated for erase2.
	Since you're always removing two elements, you always have room to insert a
	new pointer to the last free block by only removing one element.  The if
	statement for this case is cast as asking if an unsigned type is less than
	0, which of course can't happen.  It is hoped that the compiler will
	completely eliminate this branch as an optimization.
*/

#ifndef _MSL_MALLOC_TEST
static
#else
void erase2(unsigned char** e, unsigned char** i, size_t* nblocks);
#endif
void erase2(unsigned char** e, unsigned char** i, size_t* nblocks)
{
	if (!is_allocated(*(e+1)))
	{
		for (e += 2; i > e; --i)
			*i = *(i-2);
		*i = align_down((unsigned char*)i);
		*(size_t*)i |= allocate_mask;
		*nblocks -= 2;
	}
	else if ((size_t)((unsigned char*)e - ptr(*e)) < _MSL_ALIGNMENT - 1)
	{
		for (++e; i > e; --i)
			*i = *(i-2);
		*nblocks -= 2;
	}
	else
	{
		for (e += 2; i > e; --i)
			*i = *(i-2);
		*e = ptr(*(e-2));
		--e;
		*e = align_down((unsigned char*)e);
		*(size_t*)e |= allocate_mask;
		--*nblocks;
	}
}

/*
	static unsigned char** lower_bound(unsigned char** first, unsigned char** last, unsigned char* value)

	lower_bound is a binary searching function lifted from MSL C++.  Care has been taken to use > as the
	comparison instead of < since the array is stored in reverse order.  This function searches for value
	in the range [first, last) and returns an unsigned char** pointing to that value.
*/

#ifndef _MSL_MALLOC_TEST
static
#else
unsigned char** lower_bound(unsigned char** first, unsigned char** last, unsigned char* value);
#endif
unsigned char** lower_bound(unsigned char** first, unsigned char** last, unsigned char* value)
{
	size_t len = (size_t)(last - first);
	while (len > 0)
	{
		unsigned char** i = first;
		size_t len2 = len / 2;
		i += len2;
		if (ptr(*i) > value)
		{
			first = ++i;
			len -= len2 + 1;
		}
		else
			len = len2;
	}
	return first;
}

/*
	size_t _MSL_ALLOCATE_SIZE(void* vp)

	The default name for this function is __allocate_size.  This function returns the size of
	an already allocated memory block.

	Precondition: ptr is non-null and has been returned from __allocate (or malloc,
	calloc, or realloc), and has not been deallocated (by free or realloc).

	Postcondition: The returned value indicates that the client can write arbitrary
	information to the range [(char*)vp, (char*)vp + returned_value) without fear
	of corrupting the heap.

	The bounds of the array are determined, and then lower_bound is used to search
	for the client supplied pointer.  Once found the size of the block is found by
	subtracting the next highest pointer from this pointer (remember the array is
	stored in reverse).

	In debug mode, asserts check that the client supplied pointer is in the array,
	and is allocated.
*/

size_t _MSL_CDECL _MSL_ALLOCATE_SIZE(void* vp)
{
	unsigned char* pc = (unsigned char*)vp;
	size_t nblocks = *array_size();
	unsigned char** e = array(nblocks-1);
	unsigned char** i = lower_bound(e+1, array(0)+1, pc);
#ifdef _MSL_MALLOC_DEBUG
	assert(ptr(*i) == pc);
	assert(is_allocated(*i));
#endif
	return (size_t)(ptr(*(i-1)) - ptr(*i));
}

/*
	void* _MSL_ALLOCATE(size_t size_requested, size_t* size_received)

	The default name for this function is __allocate.  This function attempts
	to allocate a block of memory at least as big as size_requested. If
	size_received is non-null, it places the size of the block actually
	allocated in *size_received. This call is intended to encapsulate a call
	to malloc and a call to __allocate_size into a single function.

	Precondition: none.

	Postcondition: If size_requested is 0 then 0 is returned. Also in this
	case if size_received is not null, 0 is stored in *size_received.

	If size_requested is greater than 0 then a pointer to a memory block of
	size at least as large as size_requested bytes is returned if the system
	can manage it. And in this case if size_received is not null, then the
	actual size of the memory block will be written to *size_received. This
	value written to *size_received will be the same value which would be
	returned to the client if he immediately used the return value in a call
	to __allocate_size.

	If size_requested is greater than 0, but the system is not successful in
	returning a pointer to a block of memory of size_requested bytes, then
	null is returned. Also in this case, a best guess estimate is written to
	*size_received (if size_received is non-null) indicating what
	size_requested value may succeed in a future call to __allocate.
	Though this future call is not guaranteed to succeed. It is only a
	suggestion. Conforming implementations are allowed to always return 0 in
	*size_received upon failure.
*/

void* _MSL_CDECL _MSL_ALLOCATE(size_t size_requested, size_t* size_received)
{
	unsigned char** p;
	unsigned char** e;
	size_t nblocks;
	size_t i;
	size_t max_size_found = 0;
	void* result = 0;

	if (!defaultheapinitialized)
		init_default_heap();
	if (size_requested && size_requested < (size_t)(-1)-_MSL_ALIGNMENT && defaultheapinitialized)
	{
		size_requested = align_up(size_requested);
		nblocks = *array_size();
		e = array(nblocks-1);
		p = array(0);
		for (i = 0; i < nblocks; ++i, --p)
		{
			if (!is_allocated(*p))
			{
				size_t size = (size_t)(ptr(*(p-1)) - *p);
				if (size >= size_requested)
				{
					if (size > size_requested)
						insert(e, *p + size_requested, p, array_size());
					result = *p;
					if (size_received)
						*size_received = (size_t)(ptr(*(p-1)) - *p);
					*(size_t*)p |= allocate_mask;
					break;
				}
				else
				{
					if (max_size_found < size)
						max_size_found = size;
					/* next one is allocated, skip it */
					++i;
					--p;
				}
			}
		}
	}
	if (size_received != 0 && result == 0)
		*size_received = max_size_found;
	return result;
}

/*
	int _MSL_ALLOCATE_RESIZE(void* vp, size_t size_requested, size_t* size_received)

	The default name for this function is __allocate_resize.  This function attempts
	to expand or shrink a memory block in place to size_requested bytes. If
	successful, the return value is non-zero, otherwise zero is returned. On
	success, *size_received (if size_received is non-null) holds the actual size of
	the memory block. On failure *size_received holds a hint indicating a suggested
	size_requested that might succeed in a future call.

	Precondition: ptr is non-null and has been returned from __allocate (or malloc,
	calloc, or realloc), and has not been deallocated (by free or realloc).

	Postcondition: The memory block pointed to by ptr is still valid and has not
	been altered. If non-zero has been returned, then the current size of the memory
	block (as reflected by __allocate_size(ptr)) will be greater than or equal to
	size_requested. Additionally if size_received is non-null, then the result of
	__allocate_size(ptr) will be written to *size_received.

	If zero is returned and if size_received is non-null then a suggested
	size_requested is written to *size_received which may succeed in a future call
	to __allocate_resize.

	If size_requested is less than the size of the memory block prior to the call,
	then the size of the block after the call must be less than the size of the
	memory block before the call in order for success to be reported.  If
	size_requested is 0, the memory block is not free'd, but reduced to a minimum
	size if possible.
*/

int _MSL_CDECL _MSL_ALLOCATE_RESIZE(void* vp, size_t size_requested, size_t* size_received)
{
	unsigned char* pc = (unsigned char*)vp;
	unsigned char** e = array(*array_size()-1);
	unsigned char** i = lower_bound(e+1, array(0)+1, pc);
	size_t current_size = (size_t)(ptr(*(i-1)) - ptr(*i));
	size_t aligned_request = align_up(size_requested);
	size_t sr = current_size;
	int result = 1;
#ifdef _MSL_MALLOC_DEBUG
	assert(ptr(*i) == pc);
	assert(is_allocated(*i));
#endif
	if (aligned_request == 0)
		aligned_request = align_up(1);
	if (size_requested != current_size)
	{
		if (size_requested > current_size)
		{
			if (!is_allocated(*(i-1)))
			{
				size_t next_size = (size_t)(ptr(*(i-2)) - ptr(*(i-1)));
				unsigned could_grow = 0;
				if (i-2 == e && (size_t)((unsigned char*)e-ptr(*e)) >= _MSL_ALIGNMENT)
					could_grow = 1;
				if (current_size + next_size + could_grow*_MSL_ALIGNMENT >= aligned_request)
				{
					if (current_size + next_size > aligned_request)
						*(i-1) += aligned_request - current_size;
					else
					{
						if (current_size + next_size >= aligned_request)
							*(size_t*)(i-1) |= allocate_mask;
						erase(e, i-1, array_size());
					}
					sr = aligned_request;
				}
				else
				{
					result = 0;
					sr = current_size + next_size;
				}
			}
			else
				result = 0;
		}
		else
		{
			if (aligned_request < current_size)
			{
				if (!is_allocated(*(i-1)))
				{
					*(i-1) = ptr(*i) + aligned_request;
					sr = aligned_request;
				}
				else if (insert(e, ptr(*i) + aligned_request, i, array_size()))
					sr = aligned_request;
				else
					result = 0;
			}
			else
				result = 0;
		}
	}
	if (size_received)
		*size_received = sr;
	return result;
}

/*
	int _MSL_ALLOCATE_EXPAND(void* vp, size_t min_size, size_t preferred_size, size_t* size_received)

	The default name for this function is __allocate_expand.  The intention of this function
	is to expand a block of memory in place to preferred_size, but failing to do that,
	expand to min_size. Failure is only indicated (with a zero return) if the memory block
	can not be expanded to min_size. If the size of the memory block prior to calling this
	function is already greater than min_size, a successful return is assured. If the size
	of the memory block prior to calling this function is greater than preferred_size, an
	attempt will be made to shrink the block down to preferred size, but success will be
	returned whether or not the shrink was successful. If non-null, *size_received will
	either contain the size of the memory block after a successful return, or on failure a
	suggested preferred_size that may succeed in a future call.

	Precondition: ptr is non-null and has been returned from __allocate (or malloc, calloc,
	or realloc), and has not been deallocated (by free or realloc). min_size <= preferred_size.

	Postcondition: The memory block pointed to by ptr is still valid and has not been altered.
	If non-zero has been returned, then the current size of the memory block (as reflected by
	__allocate_size(ptr)) will be greater than or equal to min_size. Additionally if
	size_received is non-null, then the result of __allocate_size(ptr) will be written to
	*size_received.

	If zero is returned and if size_received is non-null then a suggested preferred_size is
	written to *size_received which may succeed in a future call to __allocate_expand.
*/

int _MSL_CDECL _MSL_ALLOCATE_EXPAND(void* vp, size_t min_size, size_t preferred_size, size_t* size_received)
{
	int result = 0;
	size_t sr_backup;
	size_t* srp = &sr_backup;
	if (size_received)
		srp = size_received;
	while (min_size <= preferred_size)
	{
		result = _MSL_ALLOCATE_RESIZE(vp, preferred_size, srp);
		if (result)
			break;
		preferred_size = *srp;
	};
	return result;
}

#ifndef __MALLOC
#define __MALLOC malloc
#endif

void* _MSL_CDECL __MALLOC(size_t size)
{
#ifdef _MSL_MALLOC_0_RETURNS_NON_NULL
	if (size == 0)
		size = 1;
#endif /* _MSL_MALLOC_0_RETURNS_NON_NULL */
	return _MSL_ALLOCATE(size, 0);
}

#ifndef __FREE
#define __FREE free
#endif

void _MSL_CDECL __FREE(void* vp)
{
	if (vp)
	{
		unsigned char* pc = (unsigned char*)vp;
		unsigned char** e = array(*array_size()-1);
		unsigned char** i = lower_bound(e+1, array(0)+1, pc);
	#ifdef _MSL_MALLOC_DEBUG
		assert(ptr(*i) == pc);
		assert(is_allocated(*i));
	#endif
		*(size_t*)i &= ~allocate_mask;
		if (i != array(0) && !is_allocated(*(i+1)))
		{
			if (is_allocated(*(i-1)))
				erase(e, i, array_size());
			else
				erase2(e, i, array_size());
		}
		else if (!is_allocated(*(i-1)))
			erase(e, i-1, array_size());
	}
}

#ifndef __REALLOC
#define __REALLOC realloc
#endif

void* _MSL_CDECL __REALLOC(void* vp, size_t size)
{
	void* result;
	size_t orig_size;
	if (vp == 0)
		return __MALLOC(size);
	if (size == 0)
	{
		__FREE(vp);
		return 0;
	}
	orig_size = _MSL_ALLOCATE_SIZE(vp);
	if (_MSL_ALLOCATE_RESIZE(vp, size, 0) || size <= orig_size)
		return vp;
	result = __MALLOC(size);
	if (result)
	{
		memcpy(result, vp, orig_size);
		__FREE(vp);
	}
	return result;
}

#ifndef __CALLOC
#define __CALLOC calloc
#endif

void* _MSL_CDECL __CALLOC(size_t nmemb, size_t size)
{
	void* result = __MALLOC(size*nmemb);
	if (result != 0)
		memset(result, 0, size);
	return result;
}

/* For debugging */

#ifdef _MSL_MALLOC_DEBUG

#include <stdio.h>

void _MSL_CDECL __display_heap(void);
void _MSL_CDECL __display_heap(void)
{
	unsigned char** p;
	unsigned char** e;
	size_t nblocks, i;

	if (!defaultheapinitialized)
		init_default_heap();
	printf("Heap starts at %p and is %u bytes big\n", __heap, _MSL_HEAP_SIZE);
	if (defaultheapinitialized)
	{
		nblocks = *array_size();
		printf("There are %u blocks\n", nblocks);
		e = array(nblocks-1);
		--nblocks;
		for (i = 1, p = array(0); nblocks; --p, --nblocks, ++i)
		{
			printf("Block %u at address %p is ", i, (void*)ptr(*p));
			if (is_allocated(*p))
				printf("    allocated");
			else
				printf("not allocated");
			printf(" and %u bytes big\n", ptr(*(p-1)) - ptr(*p));
		}
		printf("Block %u at address %p is ", i, (void*)ptr(*p));
		if (is_allocated(*p))
			printf("    overhead ");
		else
			printf("error");
		printf(" and %u bytes big\n\n", (__heap + _MSL_HEAP_SIZE) - (__typeof__(__heap))ptr(*p));
	}
	else
	{
		printf("Heap must be at least %u bytes big in order to allocate memory\n\n",
			align_up(3*sizeof(void*)) + align_up(sizeof(void*)));
	}
}

#endif  /* _MSL_MALLOC_DEBUG */

/* End For debugging */

/* Change record:
 * hh 041005 First code release.
*/

#elif !defined(_MSL_OS_DIRECT_MALLOC) && !(defined(_MSL_PRO4_MALLOC) || defined(_MSL_CLASSIC_MALLOC))
#pragma mark -
#pragma mark CURRENT MALLOC
#pragma mark -

/*
 *	Routines
 *	--------
 *		init_alloc			Initialize allocs' memory pool to be fixed size and location
 *		init_default_heap	initialize the default heap
 *		malloc				Allocate memory
 *		calloc				Allocate and clear memory
 *		realloc				
 *		free				Return memory block to allocs' memory pool
 */


#include <pool_alloc.h>
#include <critical_regions.h>
#ifndef __MIPS__
	#include <inttypes.h>
#endif /* __MIPS__ */
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#define NDEBUG

#include <assert.h>
#ifndef NDEBUG
	#include <stdio.h>
#endif

/* These flags can be switched on/off in ansi_prefix.XXX.h
//
//	#define _MSL_MALLOC_0_RETURNS_NON_NULL
//	#define _MSL_OS_ALLOC_SUPPORT
*/
/*///////////////////////////////////////////
// Design choices and Platform configuration
////////////////////////////////////////////*/

#if defined(__MIPS__) || !_MSL_C99                             /*- mm 030625 -*/
	typedef size_t mem_size;		/* must be same size as void* */
	#define MAX_MEM_SIZE UINT_MAX
#else
	typedef uintptr_t mem_size;		/* must be same size as void* */
	#define MAX_MEM_SIZE ULONG_MAX
#endif /* __MIPS__ || !_MSL_C99 */

#define WORD_SIZE 4  /* hack!  Must manually set to sizeof(mem_size) */

/*	These two consts describe how malloc goes to the OS for memory.  The shipping
//	configuration is that malloc will ask for at least 64Kb at a time from the OS
//	and if bigger sizes are requested, they will go to the OS in increments of 4Kb.
*/
static const mem_size sys_alloc_size = 65536; /* 64Kb */

/*	All returned memory will be aligned as indicated. */

#ifdef __ALTIVEC__
	#define alignment 16   /* valid = {4, 8, 16}, must be >= sizeof(void*) */
#elif defined(__MIPS__)
	#define alignment __ALIGNMENT__
#else
	#define alignment 8   /* valid = {4, 8, 16}, must be >= sizeof(void*) */
#endif /* __ALTIVEC__ */

#define align_ratio (alignment / WORD_SIZE)  /* Not configurable */

/*	For tiny allocations, fixed sized pools help significantly speed allocation/deallocation.
//	You can reserve a pool for a small range of sizes.  The use of these pools can be disabled
//	by setting _MSL_USE_FIX_MALLOC_POOLS to 0.  Use of fixed size pools requires further
//	configuration below.  The current shipping configuration is:
//  1. Each pool will handle approx. 4000 bytes worth of requests before asking for more memory.
//	2. There are 4 pool types.  Each type is responsible for a different range of requests:
//		a.  0 - 12 bytes
//		b. 13 - 20 bytes
//		c. 21 - 36 bytes
//		d. 37 - 68 bytes
//		Requests for greater than 68 bytes go to the variable size pools.
//	The number of types of pools is configurable below.  The range of requests for each
//	type is also configurable.
*/
#ifndef _MSL_USE_FIX_MALLOC_POOLS
	#define _MSL_USE_FIX_MALLOC_POOLS 1
#endif

#if _MSL_USE_FIX_MALLOC_POOLS

	static const mem_size fix_pool_alloc_size = 4096;  /* Approx space for each pool. */
#if (defined(__ALTIVEC__) || defined(__MIPS__))
	static const mem_size max_fix_pool_size = 76;  /* Need the highest poolsize elsewhere, so kludge it up like this */
	static const mem_size fix_pool_sizes[] = {4, 12, 28, 44, 60, max_fix_pool_size};  /* Client space for each pool */
#else
	static const mem_size max_fix_pool_size = 68;  /* Need the highest poolsize elsewhere, so kludge it up like this */
	static const mem_size fix_pool_sizes[] = {4, 12, 20, 36, 52, max_fix_pool_size};  /* Client space for each pool */
#endif /* __ALTIVEC__ || __MIPS__ */
	/* Sizes must be compatible with align_ratio (there is no check).
	// Allowed Size = (N * align_ratio - 1) * sizeof(void*)
	// Assuming sizeof(void*) == 4, then Allowed Sizes include:
	// align_ratio == 1 : {4,  8, 12, 16, 20, 24, ...}
	// align_ratio == 2 : {4, 12, 20, 28, 36, 44, ...}
	// align_ratio == 4 : {4, 12, 28, 44, 60, 76, ...}
	// Assuming sizeof(void*) == 8, then Allowed Sizes include:
	// align_ratio == 1 : {8, 16, 24, 32, 40, 48, ...}
	// align_ratio == 2 : {8, 24, 40, 56, 72, 88, ...}
	*/
	static const mem_size num_fix_pools = sizeof(fix_pool_sizes) / sizeof(mem_size);  /* Not configurable */

#endif /* _MSL_USE_FIX_MALLOC_POOLS */

/*//////////////////////////////////////////////
// End Design choices and Platform configuration
///////////////////////////////////////////////*/

typedef struct Block
{
	struct Block* prev_;
	struct Block* next_;
#if align_ratio == 4
	mem_size reserve1_;
	mem_size reserve2_;
#endif /* align_ratio == 4 */
	mem_size max_size_;
	mem_size size_;
/*	...                 // Client space here only
//	mem_size size_;     // Block trailer starts here
//	SubBlock* start_;
*/
} Block;

typedef struct SubBlock
{
	mem_size size_;
	Block* bp_;
	struct SubBlock* prev_;  /* Client space starts here */
	struct SubBlock* next_;
/*	...
//	mem_size size_;   // SubBlock Trailer, only used when SubBlock is not allocated
                      // Client space end here */
} SubBlock;

#if _MSL_USE_FIX_MALLOC_POOLS

	struct FixSubBlock;

	typedef struct FixBlock
	{
		struct FixBlock* prev_;
		struct FixBlock* next_;
		mem_size client_size_;
		struct FixSubBlock* start_;
		mem_size n_allocated_;
	#if align_ratio == 4
		mem_size reserve1_;
		mem_size reserve2_;
	#endif /* align_ratio == 4 */
	} FixBlock;

	typedef struct FixSubBlock
	{
		FixBlock* block_;
		struct FixSubBlock* next_;  /* Client space starts here */
	} FixSubBlock;

	typedef struct FixStart
	{
		FixBlock* tail_;
		FixBlock* head_;
	} FixStart;
	
#endif /* _MSL_USE_FIX_MALLOC_POOLS */

typedef struct __mem_pool_obj
{
	Block* start_;
#if _MSL_USE_FIX_MALLOC_POOLS
	FixStart fix_start[num_fix_pools];
#endif /* _MSL_USE_FIX_MALLOC_POOLS */
} __mem_pool_obj;

	static const mem_size Block_min_size = sys_alloc_size;
#if align_ratio == 4
	static const mem_size Block_overhead = 8*sizeof(mem_size);
	static const mem_size Block_header_size = 6*sizeof(mem_size);
#else
	static const mem_size Block_overhead = 6*sizeof(mem_size);
	static const mem_size Block_header_size = 4*sizeof(mem_size);
#endif /* align_ratio == 4 */
	static const mem_size Block_trailer_size = 2*sizeof(mem_size);

	static void Block_construct(Block* ths, mem_size size);
	static SubBlock* Block_subBlock(Block* ths, mem_size size, size_t*);
	static void Block_link(Block* ths, SubBlock* sb);
	static void Block_unlink(Block* ths, SubBlock* sb);
#ifndef NDEBUG
	static void Block_report(Block* ths, int verbose);
#endif

	static const mem_size SubBlock_overhead = 2*sizeof(mem_size);
#if _MSL_USE_FIX_MALLOC_POOLS
	static const mem_size SubBlock_min_size = SubBlock_overhead + max_fix_pool_size + 1 + (alignment-1) & ~(alignment-1);
#else
	static const mem_size SubBlock_min_size = SubBlock_overhead + 3*sizeof(mem_size) + (alignment-1) & ~(alignment-1);
#endif /* _MSL_USE_FIX_MALLOC_POOLS */
	static const mem_size SubBlock_header_size = SubBlock_overhead;

	static void SubBlock_construct(SubBlock* ths, mem_size size, Block* bp, int prev_alloc, int this_alloc);
	static SubBlock* SubBlock_split(SubBlock* ths, mem_size size);
	static SubBlock* SubBlock_merge_prev(SubBlock* ths, SubBlock** start);
	static void SubBlock_merge_next(SubBlock* ths, SubBlock** start);
#ifndef NDEBUG
	static void SubBlock_report(SubBlock* ths, int verbose);
#endif

/* Inline Utilities */

/* Note, there is a bit of information currently unused when alignment == 16 (0x80) */

#if WORD_SIZE == 4
	#if alignment == 4
		#define size_flag 0x7FFFFFFC
	#elif alignment == 8
		#define size_flag 0xFFFFFFF8
	#elif alignment == 16
		#define size_flag 0xFFFFFFF0
	#else
		#error Unsupported Alignment
	#endif
#elif WORD_SIZE == 8
	#if alignment == 8
		#define size_flag 0xFFFFFFFFFFFFFFF8
	#elif alignment == 16
		#define size_flag 0xFFFFFFFFFFFFFFF0
	#else
		#error Unsupported Alignment
	#endif
#else
	#error Unsupported word size in alloc.c
#endif

#define fix_var_flag 0x01

#define this_alloc_flag 0x02

#if alignment == 4
	#define prev_alloc_flag 0x80000000
#else
	#define prev_alloc_flag 0x04
#endif

#define align(size, algn) (size + (algn-1) & ~(algn-1))

#if _MSL_USE_FIX_MALLOC_POOLS

	/* return true for SubBlock, false for FixSubBlock */
	#define classify(ptr) (*(mem_size*)((char*)(ptr) - sizeof(mem_size)) & fix_var_flag)

#endif /* _MSL_USE_FIX_MALLOC_POOLS */

/* SubBlock inlines */

#define SubBlock_size(ths) ((ths)->size_ & size_flag)

#define SubBlock_block(ths) ((Block*)((mem_size)((ths)->bp_) & ~fix_var_flag))

#define SubBlock_client_space(ths) ((char*)(ths) + SubBlock_header_size)

#define SubBlock_set_free(ths)                                               \
	mem_size this_size = SubBlock_size((ths));                               \
	(ths)->size_ &= ~this_alloc_flag;                                        \
	*(mem_size*)((char*)(ths) + this_size) &= ~prev_alloc_flag;              \
	*(mem_size*)((char*)(ths) + this_size - sizeof(mem_size)) = this_size

#define SubBlock_set_not_free(ths)                              \
	mem_size this_size = SubBlock_size((ths));                  \
	(ths)->size_ |= this_alloc_flag;                            \
	*(mem_size*)((char*)(ths) + this_size) |= prev_alloc_flag

#define SubBlock_is_free(ths) !((ths)->size_ & this_alloc_flag)

#define SubBlock_set_size(ths, sz)                                 \
	(ths)->size_ &= ~size_flag;                                    \
	(ths)->size_ |= (sz) & size_flag;                              \
	if (SubBlock_is_free((ths)))                                   \
		*(mem_size*)((char*)(ths) + (sz) - sizeof(mem_size)) = (sz)

#define SubBlock_from_pointer(ptr) ((SubBlock*)((char*)(ptr) - SubBlock_header_size))

/* Block inlines */

#define Block_max_possible_size(ths) ((ths)->max_size_)

#define Block_size(ths) ((ths)->size_ & size_flag)

#define Block_start(ths) (*(SubBlock**)((char*)(ths) + Block_size((ths)) - sizeof(mem_size)))

#define Block_empty(ths)                                                      \
	(_sb = (SubBlock*)((char*)(ths) + Block_header_size)),                    \
	SubBlock_is_free(_sb) && SubBlock_size(_sb) == Block_size((ths)) - Block_overhead

/* Block Implementation */

static
void
Block_construct(Block* ths, mem_size size)
{
	SubBlock* sb;

	ths->size_ = size | this_alloc_flag | fix_var_flag;
	*(mem_size*)((char*)ths + size - Block_trailer_size) = ths->size_;
	sb = (SubBlock*)((char*)ths + Block_header_size);
	SubBlock_construct(sb, size - Block_overhead, ths, 0, 0);
	ths->max_size_ = size - Block_overhead;
	Block_start(ths) = 0;
	Block_link(ths, sb);
}

static
SubBlock*
Block_subBlock(Block* ths, mem_size size, size_t* size_received)
{
	SubBlock* st;
	SubBlock* sb;
	mem_size sb_size;
	mem_size max_found;

	st = Block_start(ths);
	if (st == 0)
	{
		ths->max_size_ = 0;
		return 0;
	}
	sb = st;
	sb_size = SubBlock_size(sb);
	max_found = sb_size;
	while (sb_size < size)
	{
		sb = sb->next_;
		sb_size = SubBlock_size(sb);
		if (max_found < sb_size)
			max_found = sb_size;
		if (sb == st)
		{
			ths->max_size_ = max_found;
			if (size_received)
				*size_received = max_found - SubBlock_overhead;
			return 0;
		}
	}
	if (sb_size - size >= SubBlock_min_size)
		SubBlock_split(sb, size);
	Block_start(ths) = sb->next_;
	Block_unlink(ths, sb);
	if (size_received)
		*size_received = SubBlock_size(sb) - SubBlock_overhead;
	return sb;
}

static
void
Block_link(Block* ths, SubBlock* sb)
{
	SubBlock** st;

	SubBlock_set_free(sb);
	st = &Block_start(ths);
	if (*st != 0)
	{
		sb->prev_ = (*st)->prev_;
		sb->prev_->next_ = sb;
		sb->next_ = *st;
		(*st)->prev_ = sb;
		*st = sb;
		*st = SubBlock_merge_prev(*st, st);
		SubBlock_merge_next(*st, st);
	}
	else
	{
		*st = sb;
		sb->prev_ = sb;
		sb->next_ = sb;
	}
	if (ths->max_size_ < SubBlock_size(*st))
		ths->max_size_ = SubBlock_size(*st);
}

static
void
Block_unlink(Block* ths, SubBlock* sb)
{
	SubBlock** st;

	SubBlock_set_not_free(sb);
	st = &Block_start(ths);
	if (*st == sb)
		*st = sb->next_;
	if (*st == sb)
	{
		*st = 0;
		ths->max_size_ = 0;
	}
	else
	{
		sb->next_->prev_ = sb->prev_;
		sb->prev_->next_ = sb->next_;
	}
}

#ifndef NDEBUG

	static
	void
	Block_report(Block* ths, int verbose)
	{
		SubBlock* st;
		SubBlock* end;
		int i;
		int prev_free;
		SubBlock* sb;

		st = (SubBlock*)((char*)ths + Block_header_size);
		end = (SubBlock*)((char*)ths + Block_size(ths) - Block_trailer_size);
		i = 1;
		if (verbose)
		printf("\tsize_ = %d, max_size = %d\n", Block_size(ths), Block_max_possible_size(ths));
		if (Block_size(ths) > 1042*1024)
		{
			printf("\t**ERROR** Block size suspiciously large %d\n", Block_size(ths));
			exit(1);
		}
		prev_free = 0;
		do
		{
			if (SubBlock_is_free(st))
			{
				if (verbose)
					printf("\tSubBlock %d at %p\n", i, st);
				if (prev_free)
				{
					printf("\t**ERROR** SubBlock failed to merge with previous\n");
					exit(1);
				}
				SubBlock_report(st, verbose);
				prev_free = 1;
			}
			else
			{
				SubBlock_report(st, verbose);
				prev_free = 0;
			}
			st = (SubBlock*)((char*)st + SubBlock_size(st));
			++i;
		} while (st != end);
		sb = Block_start(ths);
		if (sb != 0)
		{
			do
			{
				if (SubBlock_size(sb) > 1042*1024)
				{
					printf("\t**ERROR** SubBlock size suspiciously large %d\n", SubBlock_size(sb));
					exit(1);
				}
				sb = sb->next_;
			} while (sb != Block_start(ths));
			sb = Block_start(ths);
			do
			{
				if (SubBlock_size(sb) > 1042*1024)
				{
					printf("\t**ERROR** SubBlock size suspiciously large %d\n", SubBlock_size(sb));
					exit(1);
				}
				sb = sb->prev_;
			} while (sb != Block_start(ths));
		}
	}

#endif /* NDEBUG */

/* SubBlock Implementation */

static
void
SubBlock_construct(SubBlock* ths, mem_size size, Block* bp, int prev_alloc, int this_alloc)
{
	ths->bp_ = (Block*)((mem_size)bp | fix_var_flag);
	ths->size_ = size;
	if (prev_alloc)
		ths->size_ |= prev_alloc_flag;
	if (this_alloc)
	{
		ths->size_ |= this_alloc_flag;
		*(mem_size*)((char*)ths + size) |= prev_alloc_flag;
	}
	else
		*(mem_size*)((char*)ths + size - sizeof(mem_size)) = size;
}

static
SubBlock*
SubBlock_split(SubBlock* ths, mem_size sz)
{
	mem_size origsize;
	int isfree;
	int isprevalloc;
	SubBlock* np;
	Block* bp;

	origsize = SubBlock_size(ths);
	isfree = SubBlock_is_free(ths);
	isprevalloc = (int)(ths->size_ & prev_alloc_flag);
	np = (SubBlock*)((char*)ths + sz);
	bp = SubBlock_block(ths);
	
	SubBlock_construct(ths, sz, bp, isprevalloc, !isfree);
	SubBlock_construct(np, origsize - sz, bp, !isfree, !isfree);
	if (isfree)
	{
		np->next_ = ths->next_;
		np->next_->prev_ = np;
		np->prev_ = ths;
		ths->next_ = np;
	}
	return np;
}

static
SubBlock*
SubBlock_merge_prev(SubBlock* ths, SubBlock** start)
{
	mem_size prevsz;
	SubBlock* p;

	if (!(ths->size_ & prev_alloc_flag))
	{
		prevsz = *(mem_size*)((char*)ths - sizeof(mem_size));
		if (prevsz & this_alloc_flag)  /* Is it the block header? */
			return ths;
		p = (SubBlock*)((char*)ths - prevsz);
		SubBlock_set_size(p, prevsz + SubBlock_size(ths));
		/* unlink ths */
		if (*start == ths)
			*start = (*start)->next_;
		ths->next_->prev_ = ths->prev_;
		ths->next_->prev_->next_ = ths->next_;
		return p;
	}
	return ths;
}

static
void
SubBlock_merge_next(SubBlock* ths, SubBlock** start)
{
	SubBlock* next_sub;
	mem_size this_size;

	next_sub = (SubBlock*)((char*)ths + SubBlock_size(ths));
	if (SubBlock_is_free(next_sub))
	{
		this_size = SubBlock_size(ths) + SubBlock_size(next_sub);
		SubBlock_set_size(ths, this_size);
		if (SubBlock_is_free(ths))
			*(mem_size*)((char*)ths + this_size) &= ~prev_alloc_flag;
		else
			*(mem_size*)((char*)ths + this_size) |= prev_alloc_flag;
		/* unlink next_sub */
		if (*start == next_sub)
			*start = (*start)->next_;
		if (*start == next_sub)
			*start = 0;
		next_sub->next_->prev_ = next_sub->prev_;
		next_sub->prev_->next_ = next_sub->next_;
	}
}

#ifndef NDEBUG

	static
	void
	SubBlock_report(SubBlock* ths, int verbose)
	{
		if (verbose)
			printf("\t\tsize_ = %d, bp_ = %p\n", SubBlock_size(ths), SubBlock_block(ths));
		if (SubBlock_size(ths) > 1042*1024)
		{
			printf("\t**ERROR** SubBlock size suspiciously large %d\n", SubBlock_size(ths));
			exit(1);
		}
	}

#endif /* NDEBUG */

static
void
link(__mem_pool_obj* pool_obj, Block* bp)
{
	if (pool_obj->start_ != 0)
	{
		bp->prev_ = pool_obj->start_->prev_;
		bp->prev_->next_ = bp;
		bp->next_ = pool_obj->start_;
		pool_obj->start_->prev_ = bp;
		pool_obj->start_ = bp;
	}
	else
	{
		pool_obj->start_ = bp;
		bp->prev_ = bp;
		bp->next_ = bp;
	}
}

static
Block*
__unlink(__mem_pool_obj* pool_obj, Block* bp)
{
	Block* result = bp->next_;
	if (result == bp)
		result = 0;
	if (pool_obj->start_ == bp)
		pool_obj->start_ = result;
	if (result != 0)
	{
		result->prev_ = bp->prev_;
		result->prev_->next_ = result;
	}
	bp->next_ = 0;
	bp->prev_ = 0;
	return result;
}

static
Block*
link_new_block(__mem_pool_obj* pool_obj, mem_size size)
{
	Block* bp;

	size += Block_overhead;
	size = align(size, alignment);
	if (size < Block_min_size)
		size = Block_min_size;
	bp = (Block*)__sys_alloc(size);
	if (bp == 0)
		return 0;
	Block_construct(bp, size);
	link(pool_obj, bp);
	return bp;
}

static
void*
allocate_from_var_pools(__mem_pool_obj* pool_obj, mem_size size, size_t* size_received)
{
	Block* bp;
	SubBlock* ptr;

	if (size_received)
		*size_received = 0;
	size += SubBlock_overhead;
	size = align(size, alignment);
	if (size < SubBlock_min_size)
		size = SubBlock_min_size;
	bp = pool_obj->start_ != 0 ? pool_obj->start_ : link_new_block(pool_obj, size);
	if (bp == 0)
		return 0;
	do
	{
		if (size <= Block_max_possible_size(bp))
		{
			ptr = Block_subBlock(bp, size, size_received);
			if (ptr != 0)
			{
				pool_obj->start_ = bp;
				break;
			}
		}
		bp = bp->next_;
		if (bp == pool_obj->start_)
		{
			bp = link_new_block(pool_obj, size);
			if (bp == 0)
				return 0;
			ptr = Block_subBlock(bp, size, size_received);
			break;
		}
	} while (1);
	return SubBlock_client_space(ptr);
}

static
void*
soft_allocate_from_var_pools(__mem_pool_obj* pool_obj, mem_size size, mem_size* max_size)
{
	Block* bp;
	SubBlock* ptr;

	size += SubBlock_overhead;
	size = align(size, alignment);
	if (size < SubBlock_min_size)
		size = SubBlock_min_size;
	*max_size = 0;
	bp = pool_obj->start_;
	if (bp == 0)
		return 0;
	do
	{
		if (size <= Block_max_possible_size(bp))
		{
			ptr = Block_subBlock(bp, size, 0);
			if (ptr != 0)
			{
				pool_obj->start_ = bp;
				break;
			}
		}
		if (Block_max_possible_size(bp) > SubBlock_overhead &&
		    *max_size < Block_max_possible_size(bp) - SubBlock_overhead)
			*max_size = Block_max_possible_size(bp) - SubBlock_overhead;
		bp = bp->next_;
		if (bp == pool_obj->start_)
			return 0;
	} while (1);
	return SubBlock_client_space(ptr);
}

static
void
deallocate_from_var_pools(__mem_pool_obj* pool_obj, void* ptr)
{
	SubBlock* sb = SubBlock_from_pointer(ptr);
#if _MSL_OS_ALLOC_SUPPORT
	SubBlock* _sb;
#endif /* _MSL_OS_ALLOC_SUPPORT */
	Block* bp = SubBlock_block(sb);
	Block_link(bp, sb);
#if _MSL_OS_ALLOC_SUPPORT
	if (Block_empty(bp))
	{
		__unlink(pool_obj, bp);
		__sys_free(bp);
	}
#else
	(void)pool_obj;
#endif /* _MSL_OS_ALLOC_SUPPORT */
}

#if _MSL_USE_FIX_MALLOC_POOLS

	static const mem_size FixBlock_overhead = sizeof(FixBlock);
	static const mem_size FixBlock_header_size = FixBlock_overhead;

	static void FixBlock_construct(FixBlock* ths, FixBlock* prev, FixBlock* next, mem_size index,
		FixSubBlock* chunk, mem_size chunk_size);

	static const mem_size FixSubBlock_overhead = sizeof(mem_size);
	static const mem_size FixSubBlock_header_size = FixSubBlock_overhead;

	/* FixBlock Implementation */
	
	#define FixBlock_client_size(ths) ((ths)->client_size_)

	/* FixSubBlock Implementation */
	
	#define FixSubBlock_construct(ths, block, next) (((FixSubBlock*)(ths))->block_ = block, ((FixSubBlock*)(ths))->next_ = next)

	#define FixSubBlock_client_space(ths) ((char*)(ths) + FixSubBlock_header_size)

	#define FixSubBlock_size(ths) (FixBlock_client_size((ths)->block_))

	#define FixSubBlock_from_pointer(ptr) ((FixSubBlock*)((char*)(ptr) - FixSubBlock_header_size))

	/* FixBlock Implementation */
	
	static
	void
	FixBlock_construct(FixBlock* ths, FixBlock* prev, FixBlock* next, mem_size index,
		FixSubBlock* chunk, mem_size chunk_size)
	{
		mem_size fixSubBlock_size;
		mem_size n;
		char* p;
		mem_size i;
		char* np;

		ths->prev_ = prev;
		ths->next_ = next;
		prev->next_ = ths;
		next->prev_ = ths;
		ths->client_size_ = fix_pool_sizes[index];
		fixSubBlock_size = fix_pool_sizes[index] + FixSubBlock_overhead;
		n = chunk_size / fixSubBlock_size;
		p = (char*)chunk;
		for (i = 0; i < n-1; ++i)
		{
			np = p + fixSubBlock_size;
			FixSubBlock_construct(p, ths, (FixSubBlock*)np);
			p = np;
		}
		FixSubBlock_construct(p, ths, 0);
		ths->start_ = chunk;
		ths->n_allocated_ = 0;
	}

#endif /* _MSL_USE_FIX_MALLOC_POOLS */

void _MSL_CDECL __init_pool_obj(__mem_pool* pool_obj)
{
	assert(sizeof(__mem_pool) >= sizeof(__mem_pool_obj));
	memset(pool_obj, 0, sizeof(__mem_pool_obj));
}

static
__mem_pool*
get_malloc_pool(void)
{
	static __mem_pool protopool;
	static unsigned char init = 0;
	if (!init)
	{
		__init_pool_obj(&protopool);
		init = 1;
	}
	return &protopool;
}

#if _MSL_USE_FIX_MALLOC_POOLS
	#define __msize_inline(ptr) (!classify(ptr) ? FixSubBlock_size(FixSubBlock_from_pointer(ptr)) : SubBlock_size(SubBlock_from_pointer(ptr)) - SubBlock_overhead)
#else
	#define __msize_inline(ptr) (SubBlock_size(SubBlock_from_pointer(ptr)) - SubBlock_overhead)
#endif /* _MSL_USE_FIX_MALLOC_POOLS */

#if _MSL_USE_FIX_MALLOC_POOLS

	static
	void*
	allocate_from_fixed_pools(__mem_pool_obj* pool_obj, mem_size size, size_t* client_received)
	{
		mem_size i = 0;
		FixSubBlock* p;
		FixStart* fs;

		while (size > fix_pool_sizes[i])
			++i;
		fs = &pool_obj->fix_start[i];
		if (fs->head_ == 0 || fs->head_->start_ == 0)
		{
			mem_size size_requested = fix_pool_alloc_size;
			char* newblock;
			mem_size size_received;
			mem_size n, nsave, size_has;

			n = (size_requested - FixBlock_overhead) / (fix_pool_sizes[i] + FixSubBlock_overhead);
			if (n > 256)
				n = 256;
			nsave = n;
			while (n >= 10)
			{
				size_requested = n * (fix_pool_sizes[i] + FixSubBlock_overhead) + FixBlock_overhead;
				newblock = (char*)soft_allocate_from_var_pools(pool_obj, size_requested, &size_has);
				if (newblock != 0)
					break;
				if (size_has > FixBlock_overhead)
					n = (size_has - FixBlock_overhead) / (fix_pool_sizes[i] + FixSubBlock_overhead);
				else
					n = 0;
			}
			if (newblock == 0)
			{
				size_t cr_backup;
				n = nsave;
				while (1)
				{
					size_requested = n * (fix_pool_sizes[i] + FixSubBlock_overhead) + FixBlock_overhead;
					newblock = (char*)allocate_from_var_pools(pool_obj, size_requested, &cr_backup);
					if (newblock != 0)
						break;
					n = cr_backup / ((fix_pool_sizes[i] + FixSubBlock_overhead) + FixBlock_overhead);
					if (n < 10)
					{
						if (client_received)
							*client_received = 0;
						return 0;
					}
				}
			}
			size_received = __msize_inline(newblock);
			if (fs->head_ == 0)
			{
				fs->head_ = (FixBlock*)newblock;
				fs->tail_ = (FixBlock*)newblock;
			}
			FixBlock_construct((FixBlock*)newblock, fs->tail_, fs->head_, i,
				(FixSubBlock*)(newblock + FixBlock_overhead), size_received - FixBlock_overhead);
			fs->head_ = (FixBlock*)newblock;
		}
		p = fs->head_->start_;
		fs->head_->start_ = p->next_;
		++fs->head_->n_allocated_;
		if (fs->head_->start_ == 0)
		{
			fs->head_ = fs->head_->next_;
			fs->tail_ = fs->tail_->next_;
		}
		if (client_received)
			*client_received = fix_pool_sizes[i];
		return FixSubBlock_client_space(p);
	}

	void deallocate_from_fixed_pools(__mem_pool_obj*, void* , mem_size);
	void
	deallocate_from_fixed_pools(__mem_pool_obj* pool_obj, void* ptr, mem_size size)
	{
		mem_size i = 0;
		FixSubBlock* p;
		FixBlock* b;
		FixStart* fs;

		while (size > fix_pool_sizes[i])
			++i;
		fs = &pool_obj->fix_start[i];
		p = FixSubBlock_from_pointer(ptr);
		b = p->block_;
		if (b->start_ == 0 && fs->head_ != b)
		{
			if (fs->tail_ == b)
			{
				fs->head_ = fs->head_->prev_;
				fs->tail_ = fs->tail_->prev_;
			}
			else
			{
				b->prev_->next_ = b->next_;
				b->next_->prev_ = b->prev_;
				b->next_ = fs->head_;
				b->prev_ = b->next_->prev_;
				b->prev_->next_ = b;
				b->next_->prev_ = b;
				fs->head_ = b;
			}
		}
		p->next_ = b->start_;
		b->start_ = p;
		if (--b->n_allocated_ == 0)
		{
			if (fs->head_ == b)
				fs->head_ = b->next_;
			if (fs->tail_ == b)
				fs->tail_ = b->prev_;
			b->prev_->next_ = b->next_;
			b->next_->prev_ = b->prev_;
			if (fs->head_ == b)
				fs->head_ = 0;
			if (fs->tail_ == b)
				fs->tail_ = 0;
			deallocate_from_var_pools(pool_obj, b);
		}
	}

#endif /* _MSL_USE_FIX_MALLOC_POOLS */

#ifndef NDEBUG

	void __report_on_pool_heap(__mem_pool_obj* pool_obj, int verbose);
	void
	__report_on_pool_heap(__mem_pool_obj* pool_obj, int verbose)
	{
		Block* bp;
		int i;

		if (verbose)
			printf("\n-------------------------\n");
		bp = pool_obj->start_;
		if (bp == 0)
		{
			if (verbose)
				printf("\nNothing to report\n");
			return;
		}
		i = 1;
		do
		{
			if (verbose)
				printf("\nBlock %d at %p\n", i, bp);
			Block_report(bp, verbose);
			bp = bp->next_;
			++i;
		} while (bp != pool_obj->start_);
	}

	void __report_on_heap(int verbose);
	void __report_on_heap(int verbose)
	{
		__report_on_pool_heap((__mem_pool_obj*)get_malloc_pool(), verbose);
	}

#endif /* NDEBUG */

static int _MSL_CDECL
__pool_allocate_resize(__mem_pool* pool, void* ptr, size_t size_requested, size_t* size_received)
{
	__mem_pool_obj* pool_obj;
	mem_size sz, this_size, orig_size;
	SubBlock* sb;
	SubBlock* next_sub;

	pool_obj = (__mem_pool_obj*)pool;
	orig_size = __msize_inline(ptr);
	if (size_requested == orig_size)
	{
		if (size_received)
			*size_received = orig_size;
		return 1;
	}
#if _MSL_USE_FIX_MALLOC_POOLS
	if (classify(ptr)) /* is var_block */
	{
#endif /* _MSL_USE_FIX_MALLOC_POOLS */
		if (size_requested > MAX_MEM_SIZE - (alignment + SubBlock_overhead + Block_overhead + alignment + SubBlock_min_size))
			size_requested = MAX_MEM_SIZE - (alignment + SubBlock_overhead + Block_overhead + alignment + SubBlock_min_size);
		sz = size_requested + SubBlock_overhead;
		sz = align(sz, alignment);
		if (sz < SubBlock_min_size)
			sz = SubBlock_min_size;
		sb = SubBlock_from_pointer(ptr);
		this_size = orig_size + SubBlock_overhead;
		next_sub = (SubBlock*)((char*)sb + this_size);
		if (SubBlock_is_free(next_sub) && sz <= this_size + SubBlock_size(next_sub))
		{
			SubBlock_merge_next(sb, &Block_start(SubBlock_block(sb)));
			this_size = SubBlock_size(sb);
		}
		if (this_size >= sz + SubBlock_min_size)
		{
			Block_link(SubBlock_block(sb), SubBlock_split(sb, sz));
			this_size = SubBlock_size(sb);
		}
		this_size -= SubBlock_overhead;
		if (size_received)
			*size_received = this_size;
		if (size_requested > orig_size)
		{
			if (size_received && size_requested > this_size && SubBlock_is_free(next_sub))
				*size_received += SubBlock_size(next_sub);
			return size_requested <= this_size;
		}
		return this_size < orig_size;
#if _MSL_USE_FIX_MALLOC_POOLS
	}
	/* is fix_block*/
	if (size_received)
		*size_received = orig_size;
	return 0;
#endif /* _MSL_USE_FIX_MALLOC_POOLS */
}

size_t _MSL_CDECL
__msize(void* ptr)
{
#if _MSL_USE_FIX_MALLOC_POOLS
	if (!classify(ptr))
		return FixSubBlock_size(FixSubBlock_from_pointer(ptr));
#endif /* _MSL_USE_FIX_MALLOC_POOLS */
	return SubBlock_size(SubBlock_from_pointer(ptr)) - SubBlock_overhead;
}

#ifndef __MALLOC
#define __MALLOC malloc
#endif

#ifndef __REALLOC
#define __REALLOC realloc
#endif

#ifndef __CALLOC
#define __CALLOC calloc
#endif

#ifndef __FREE
#define __FREE free
#endif

#if !_MSL_OS_ALLOC_SUPPORT
	static int defaultheapinitialized = 0;
	void init_default_heap(void);
#endif /* !_MSL_OS_ALLOC_SUPPORT */

static void* _MSL_CDECL
__pool_allocate(__mem_pool* pool, size_t size_requested, size_t* size_received)
{
	void* result;
	__mem_pool_obj* pool_obj;

	assert(sizeof(mem_size) == sizeof(void*));
	assert(align_ratio >= 1);
	assert(sizeof(__mem_pool) >= sizeof(__mem_pool_obj));

	if (size_requested > MAX_MEM_SIZE - (alignment + SubBlock_overhead + Block_overhead + alignment))
	{
		if (size_received)
			*size_received = MAX_MEM_SIZE - (alignment + SubBlock_overhead + Block_overhead + alignment);
		return 0;
	}
#if !_MSL_OS_ALLOC_SUPPORT
	if (!defaultheapinitialized)
		init_default_heap();
#endif /* !_MSL_OS_ALLOC_SUPPORT */
	pool_obj = (__mem_pool_obj*)pool;
#if _MSL_USE_FIX_MALLOC_POOLS
	if (size_requested <= max_fix_pool_size)
		result = allocate_from_fixed_pools(pool_obj, size_requested, size_received);
	else
#endif /* _MSL_USE_FIX_MALLOC_POOLS */
		result = allocate_from_var_pools(pool_obj, size_requested, size_received);
	return result;
}

void* _MSL_CDECL
__pool_alloc(__mem_pool* pool, size_t size)
{
	return __pool_allocate(pool, size, 0);
}

size_t _MSL_CDECL _MSL_ALLOCATE_SIZE(void* ptr)
{
#if _MSL_USE_FIX_MALLOC_POOLS
	if (!classify(ptr))
		return FixSubBlock_size(FixSubBlock_from_pointer(ptr));
#endif /* _MSL_USE_FIX_MALLOC_POOLS */
	return SubBlock_size(SubBlock_from_pointer(ptr)) - SubBlock_overhead;
}

void* _MSL_CDECL _MSL_ALLOCATE(size_t size_requested, size_t* size_received)
{
	void* result;
	if (size_requested)
	{
		__begin_critical_region(malloc_pool_access);
		result = __pool_allocate(get_malloc_pool(), size_requested, size_received);
		__end_critical_region(malloc_pool_access);
	}
	else
	{
		result = 0;
		if (size_received)
			*size_received = 0;
	}
	return result;
}

int _MSL_CDECL _MSL_ALLOCATE_RESIZE(void* ptr, size_t size_requested, size_t* size_received)
{
	int result;
	__begin_critical_region(malloc_pool_access);
	result = __pool_allocate_resize(get_malloc_pool(), ptr, size_requested, size_received);
	__end_critical_region(malloc_pool_access);
	return result;
}

int _MSL_CDECL _MSL_ALLOCATE_EXPAND(void* ptr, size_t min_size, size_t preferred_size, size_t* size_received)
{
	int result = 0;
	size_t sr_backup;
	size_t* srp = &sr_backup;
	if (size_received)
		srp = size_received;
	__begin_critical_region(malloc_pool_access);
	while (min_size <= preferred_size)
	{
		result = __pool_allocate_resize(get_malloc_pool(), ptr, preferred_size, srp);
		if (result)
			break;
		preferred_size = *srp;
	}
	__end_critical_region(malloc_pool_access);
	return result;
}

void _MSL_CDECL
__pool_free(__mem_pool* pool, void* ptr)
{
	__mem_pool_obj* pool_obj;
#if _MSL_USE_FIX_MALLOC_POOLS
	mem_size size;
#endif /* _MSL_USE_FIX_MALLOC_POOLS */

	if (ptr == 0)
		return;
	pool_obj = (__mem_pool_obj*)pool;
#if _MSL_USE_FIX_MALLOC_POOLS
	size = __msize_inline(ptr);
	if (size <= max_fix_pool_size)
		deallocate_from_fixed_pools(pool_obj, ptr, size);
	else
#endif /* _MSL_USE_FIX_MALLOC_POOLS */
		deallocate_from_var_pools(pool_obj, ptr);
}

void* _MSL_CDECL
__pool_realloc(__mem_pool* pool, void* ptr, size_t size)
{
	__mem_pool_obj* pool_obj;
	mem_size current_size;
	void* newptr;

	pool_obj = (__mem_pool_obj*)pool;
	if (ptr == 0)
	#ifdef _MSL_MALLOC_0_RETURNS_NON_NULL
		return __pool_allocate(pool, size ? size : 1, 0);
	#else
		return __pool_allocate(pool, size, 0);
	#endif /* _MSL_MALLOC_0_RETURNS_NON_NULL */
	if (size == 0)
	{
		__pool_free(pool, ptr);
		return 0;
	}
	current_size = __msize_inline(ptr);
	if (__pool_allocate_resize(pool, ptr, size, 0) || size <= current_size)
		return ptr;
	newptr = __pool_allocate(pool, size, 0);
	if (newptr)
	{
		memcpy(newptr, ptr, current_size);
		__pool_free(pool, ptr);
	}
	return newptr;
}

void* _MSL_CDECL __pool_alloc_clear(__mem_pool* pool, size_t size)
{
	void* result;
#ifdef _MSL_MALLOC_0_RETURNS_NON_NULL
	if (size == 0)
		size = 1;
#endif /* _MSL_MALLOC_0_RETURNS_NON_NULL */
	result = __pool_allocate(pool, size, 0);
	if (result != 0)
		memset(result, 0, size);
	return result;
}

void* _MSL_CDECL __MALLOC(size_t size)
{
#ifdef _MSL_MALLOC_0_RETURNS_NON_NULL
	if (size == 0)
		size = 1;
#endif /* _MSL_MALLOC_0_RETURNS_NON_NULL */
	return _MSL_ALLOCATE(size, 0);
}

void _MSL_CDECL __FREE(void* ptr)
{
	__begin_critical_region(malloc_pool_access);
	__pool_free(get_malloc_pool(), ptr);
	__end_critical_region(malloc_pool_access);
}

void* _MSL_CDECL __REALLOC(void* ptr, size_t size)
{
	void* result;
	__begin_critical_region(malloc_pool_access);
	result = __pool_realloc(get_malloc_pool(), ptr, size);
	__end_critical_region(malloc_pool_access);
	return result;
}

void* _MSL_CDECL __CALLOC(size_t nmemb, size_t size)
{
	void* result;
	__begin_critical_region(malloc_pool_access);
	result = __pool_alloc_clear(get_malloc_pool(), size*nmemb);
	__end_critical_region(malloc_pool_access);
	return result;
}

#if _MSL_OS_ALLOC_SUPPORT

	void _MSL_CDECL
	__pool_free_all(__mem_pool* pool)
	{
		__mem_pool_obj* pool_obj = (__mem_pool_obj*)pool;
		Block* bp = pool_obj->start_;
		Block* bpn;

		if (bp == 0)
			return;
		do
		{
			bpn = bp->next_;
			__sys_free(bp);
			bp = bpn;
		} while (bp != pool_obj->start_);
		__init_pool_obj(pool);
	}

	void _MSL_CDECL
	__malloc_free_all(void)
	{
		__pool_free_all(get_malloc_pool());
	}

#endif /* _MSL_OS_ALLOC_SUPPORT */

#if !_MSL_OS_ALLOC_SUPPORT

	/***************************************************************************
	 *	init_alloc
	 *		Initializes the memory pool that malloc uses. This call is designed to
	 *		be used for systems that DO NOT have memory allocation functions
	 *  	built into the system. The first call will define the pool and use the
	 *		rest of the space as a block of memory. Subsequent calls will add the
	 *		memory as another block of memory to the malloc's existing pool. Once
	 *		a heap of memory is allocated to the malloc memory pool, it can not 
	 *		be returned.
	 *
	 *	Inputs:
	 *		pool_ptr		pointer to memory to be used in malloc's memory pool
	 *		size				size of memory to be used in (or added to) malloc's memory pool
	 *
	 *	Outputs:
	 *		None
	 *
	 *	Returns: int
	 *		0		Fail
	 *		1		Success: Memory initialized as malloc's memory pool
	 *		2		Success: Memory added to initial heap successfully
	 *
	 *  NOTE: 
	 *		This function must be called prior to using other alloc.c functions
	 *
	 ***************************************************************************/
	int
	init_alloc(void* heap_ptr, size_t heap_size)
	{
		mem_size size;
		Block* bp;
		int result;
		__mem_pool_obj* pool_obj;

		assert((mem_size)heap_ptr % alignment == 0);

		if (heap_ptr == 0)
			return 0;
		size = align(heap_size, alignment);
		if (size > heap_size)
			size -= alignment;
		if (size < Block_overhead + SubBlock_min_size)
			return 0;
		__begin_critical_region(malloc_pool_access);
		pool_obj = (__mem_pool_obj*)get_malloc_pool();
		result = pool_obj->start_ == 0 ? 1 : 2;
		bp = (Block*)heap_ptr;
		Block_construct(bp, size);
		link(pool_obj, bp);
		__end_critical_region(malloc_pool_access);
		return result;
	}

	void init_default_heap(void)
	{
		defaultheapinitialized = 1;
		
		if (_MSL_HEAP_SIZE != 0)
			init_alloc((void *) _MSL_HEAP_START, (size_t) _MSL_HEAP_SIZE);
	}

	void*
	__sys_alloc(size_t x)
	{
	#pragma unused(x)
		return 0;
	}

#endif /* _MSL_OS_ALLOC_SUPPORT */

#ifdef __ALTIVEC__

	void* vec_malloc(size_t size);
	void* vec_calloc(size_t nmemb, size_t size);
	void* vec_realloc(void* ptr, size_t size);
	void vec_free(void* ptr);

	void*
	vec_malloc(size_t size)
	{
		return malloc(size);
	}

	void*
	vec_calloc(size_t nmemb, size_t size)
	{
		return calloc(nmemb, size);
	}

	void*
	vec_realloc(void* ptr, size_t size)
	{
		return realloc(ptr, size);
	}

	void
	vec_free(void* ptr)
	{
		free(ptr);
	}

#endif /* __ALTIVEC__ */


/* Change record:
 * JFH 950320 First code release.
 * JFH 950609 Changed malloc_pool to __malloc_pool as per standard naming conventions.
 * mm  970708 Inserted Be changes
 * MEA 970720 Changed __no_os to _No_Alloc_OS_Support and  __ppc_eabi_bare to __ppc_eabi.
 * mm  970904 Added include ansi_parms.h  to allow compilation without prefix
 * mm  981015 Added code for _No_Alloc_OS_Support and __DSP568
 * mm  981029 Changed __DSP568 to __m56800__
 * mf  981115 Fixed mm change of 981015
 * blc 990209 Added __msize
 * hh  990227 Rewrote in an effort to make faster.
 * hh  990504 Added configuration for MIPS in several places
 * hh  010424 Modified malloc system to handle multiple pools
 * cc  010822 Added __dest_os ==__dolphin_os
 * hh  011109 set ths->max_size_ to 0 in Block_subBlock when st == 0
 * cc  011203 Added _MSL_CDECL for new name mangling
 * cc  020529 Added rsj changes from the mainline
 * cc  021031 Added bethc's changes to support arm
 * hh  030521 Fixed bug in prev_alloc_flag definition when alignment == 4
 * mm  030625 Added _MSL_C99 wrapper
 * hh  030907 Added ARM / __allocate interface
 * JWW 040401 Changed _No_Alloc_OS_Support to _MSL_OS_ALLOC_SUPPORT
 */

#elif 0

}  //  make the ide #pragma mark happy

#endif
