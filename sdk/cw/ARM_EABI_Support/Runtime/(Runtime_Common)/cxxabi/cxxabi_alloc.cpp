/* Metrowerks Runtime Library
 * Copyright © 2003-2005 Metrowerks Corporation.  All rights reserved.
 *
 * Purpose:  Implementation of the portable Itanium C++ ABI
 *           see http://www.codesourcery.com/cxx-abi/
 *
 * $Date: 2005/03/07 21:32:23 $
 * $Revision: 1.2 $
 */

#include <cxxabi.h>
#include <exception>

//	private definitions/declarations

namespace {
	void DestroyArrayRange(unsigned char *first,unsigned char *last,_CXXABI_SIZE_T element_size,void (*destructor) ( void *thisp ))
	{
		try {
			while(last>first)
			{
				last -= element_size;
				destructor(last);
			}
		}
		catch(...)
		{
			std::terminate();
		}
	}
}

//	Implementation

//	Given the number and size of elements for an array,
//	and the non-negative size of prefix padding for a cookie,
//	allocate space for the array preceded by the specified padding,
//	initialize the cookie if the padding is non-zero, and call the given
//	constructor on each element. Return the address of the array proper,
//	after the padding. If allocation throws, rethrow. If the constructor throws an exception,
//	call the given destructor for any already-constructed elements, delete the space, and rethrow the exception.
//	If the destructor throws an exception, call terminate(). The constructor may be NULL. The destructor may be NULL
//	only if the padding size is zero. If either is NULL, no action is taken when it would have been called. 
extern "C" void * __cxa_vec_new (
            _CXXABI_SIZE_T element_count,
            _CXXABI_SIZE_T element_size,
            _CXXABI_SIZE_T padding_size,
            void (*constructor) ( void *thisp ),
            void (*destructor) ( void *thisp ) )
{
	unsigned char	*ptr;

	//	allocate storage
	if( (ptr=(unsigned char *)::operator new[](element_count*element_size+padding_size))==NULL)
		return NULL;

	//	store cookie
	if(padding_size)
	{
		*(_CXXABI_SIZE_T *)(ptr+padding_size-sizeof(_CXXABI_SIZE_T)) = element_count;
#if __IA64_CPP_ABI_ARM__
		if(padding_size>=2*sizeof(_CXXABI_SIZE_T))
			*(_CXXABI_SIZE_T *)(ptr+padding_size-2*sizeof(_CXXABI_SIZE_T)) = element_size;
#endif
	}

	//	initialize array
	if(constructor)
	{
		unsigned char	*arrayptr;

		arrayptr=ptr+padding_size;

		try
		{
			for(; element_count>0; --element_count)
			{
				constructor(arrayptr);
				arrayptr += element_size;
			}
		}
		catch(...)
		{
			if(destructor)
			{
				DestroyArrayRange(ptr + padding_size,arrayptr,element_size,destructor);
			}
			::operator delete [](ptr);
			throw;
		}
	}
	return ptr+padding_size;
}

//#########################################################
//###	used by CW to initialize new allocated arrays	###
//###		not a standard ABI runtime function			###
//#########################################################
extern "C" void * __cxa_vec_newinit (
			unsigned char *ptr,
            _CXXABI_SIZE_T element_count,
            _CXXABI_SIZE_T element_size,
            _CXXABI_SIZE_T padding_size,
            void (*constructor) ( void *thisp ),
            void (*destructor) ( void *thisp ) );
extern "C" void * __cxa_vec_newinit (
			unsigned char *ptr,
            _CXXABI_SIZE_T element_count,
            _CXXABI_SIZE_T element_size,
            _CXXABI_SIZE_T padding_size,
            void (*constructor) ( void *thisp ),
            void (*destructor) ( void *thisp ) )
{
	//	store cookie
	if(padding_size)
	{
		*(_CXXABI_SIZE_T *)(ptr+padding_size-sizeof(_CXXABI_SIZE_T)) = element_count;
#if __IA64_CPP_ABI_ARM__
		if(padding_size>=2*sizeof(_CXXABI_SIZE_T))
			*(_CXXABI_SIZE_T *)(ptr+padding_size-2*sizeof(_CXXABI_SIZE_T)) = element_size;
#endif
	}

	//	initialize array
	if(constructor)
	{
		unsigned char	*arrayptr;

		arrayptr=ptr+padding_size;
		if(destructor)
		{
			try
			{
				for(; element_count>0; --element_count)
				{
					constructor(arrayptr);
					arrayptr += element_size;
				}
			}
			catch(...)
			{
				DestroyArrayRange(ptr + padding_size,arrayptr,element_size,destructor);
				throw;
			}
		}
		else
		{
			for(; element_count>0; --element_count)
			{
				constructor(arrayptr);
				arrayptr += element_size;
			}
		}
	}
	return ptr+padding_size;
}

//	Same as __cxa_vec_new, except that the given functions are used for allocation/deallocation
//	instead of the default new/delete functions. If dealloc throws an exception, the result is undefined.
//	The alloc and dealloc pointers may not be NULL. 
extern "C" void * __cxa_vec_new2 (
            _CXXABI_SIZE_T element_count,
            _CXXABI_SIZE_T element_size,
            _CXXABI_SIZE_T padding_size,
            void (*constructor) ( void *thisp ),
            void (*destructor) ( void *thisp ),
            void* (*alloc) ( _CXXABI_SIZE_T size ),
            void (*dealloc) ( void *obj ) )
{	//	currently not used by CW / not tested
	unsigned char	*ptr;

	//	allocate storage
	if( (ptr=(unsigned char *)alloc(element_count*element_size+padding_size))==NULL)
		return NULL;

	//	store cookie
	if(padding_size)
	{
		*(_CXXABI_SIZE_T *)(ptr+padding_size-sizeof(_CXXABI_SIZE_T)) = element_count;
#if __IA64_CPP_ABI_ARM__
		if(padding_size>=2*sizeof(_CXXABI_SIZE_T))
			*(_CXXABI_SIZE_T *)(ptr+padding_size-2*sizeof(_CXXABI_SIZE_T)) = element_size;
#endif
	}

	//	initialize array
	if(constructor)
	{
		unsigned char	*arrayptr;

		arrayptr=ptr+padding_size;
		try
		{
			for(; element_count>0; --element_count)
			{
				constructor(arrayptr);
				arrayptr += element_size;
			}
		}
		catch(...)
		{
			if(destructor)
			{
				DestroyArrayRange(ptr + padding_size,arrayptr,element_size,destructor);
			}
			dealloc(ptr);
			throw;
		}
	}
	return ptr+padding_size;
}

//	Same as __cxa_vec_new, except that the given functions are used for allocation/deallocation
//	instead of the default new/delete functions. The deallocation function takes both the object address and its size.
//	If dealloc throws an exception, the result is undefined. The alloc and dealloc pointers may not be NULL. 
extern "C" void * __cxa_vec_new3 (
            _CXXABI_SIZE_T element_count,
            _CXXABI_SIZE_T element_size, 
            _CXXABI_SIZE_T padding_size,
            void (*constructor) ( void *thisp ),
            void (*destructor) ( void *thisp ),
            void* (*alloc) ( _CXXABI_SIZE_T size ),
            void (*dealloc) ( void *obj, _CXXABI_SIZE_T size ) )
{	//	currently not used by CW / not tested
	unsigned char	*ptr;
	_CXXABI_SIZE_T	size;

	//	allocate storage
	size = element_count*element_size+padding_size;
	if( (ptr=(unsigned char *)alloc(size))==NULL)
		return NULL;

	//	store cookie
	if(padding_size)
	{
		*(_CXXABI_SIZE_T *)(ptr+padding_size-sizeof(_CXXABI_SIZE_T)) = element_count;
#if __IA64_CPP_ABI_ARM__
		if(padding_size>=2*sizeof(_CXXABI_SIZE_T))
			*(_CXXABI_SIZE_T *)(ptr+padding_size-2*sizeof(_CXXABI_SIZE_T)) = element_size;
#endif
	}

	//	initialize array
	if(constructor)
	{
		unsigned char	*arrayptr;

		arrayptr=ptr+padding_size;
		try
		{
			for(; element_count>0; --element_count)
			{
				constructor(arrayptr);
				arrayptr += element_size;
			}
		}
		catch(...)
		{
			if(destructor)
			{
				DestroyArrayRange(ptr + padding_size,arrayptr,element_size,destructor);
			}
			dealloc(ptr,size);
			throw;
		}
	}
	return ptr+padding_size;
}

//	Given the (data) address of an array, not including any cookie padding, and the number and size of its elements,
//	call the given constructor on each element. If the constructor throws an exception,
//	call the given destructor for any already-constructed elements, and rethrow the exception.
//	If the destructor throws an exception, call terminate(). The constructor and/or destructor pointers may be NULL.
//	If either is NULL, no action is taken when it would have been called. 
extern "C" void __cxa_vec_ctor (
	void	*array_address,
	_CXXABI_SIZE_T	element_count,
	_CXXABI_SIZE_T	element_size,
	void	(*constructor) ( void *thisp ),
	void	(*destructor) ( void *thisp ) )
{
	unsigned char *ptr;

	if(constructor)
	{
		if(destructor)
		{
			try
			{
				for(ptr=(unsigned char *)array_address; element_count>0; --element_count)
				{
					constructor(ptr);
					ptr += element_size;
				}
			}
			catch(...)
			{
				DestroyArrayRange((unsigned char *)array_address,ptr,element_size,destructor);
				throw;
			}
		}
		else
		{
			for(ptr=(unsigned char *)array_address; element_count>0; --element_count)
			{
				constructor(ptr);
				ptr += element_size;
			}
		}
	}
}

//	Given the (data) address of an array, the number of elements, and the size of its elements,
//	call the given destructor on each element. If the destructor throws an exception,
//	rethrow after destroying the remaining elements if possible. If the destructor throws a
//	second exception, call terminate. The destructor pointer may be NULL,
//	in which case this routine does nothing. 
extern "C" void __cxa_vec_dtor (
            void *array_address,
            _CXXABI_SIZE_T element_count,
            _CXXABI_SIZE_T element_size,
            void (*destructor) ( void *thisp ) )
{
	unsigned char *ptr;

	if(destructor==NULL) return;

	ptr=(unsigned char *)array_address + element_count*element_size;
	try
	{
		for(; element_count>0; --element_count)
		{
			ptr -= element_size;
			destructor(ptr);
		}
	}
	catch(...)
	{
		try
		{
			for(--element_count; element_count>0; --element_count)
			{
				ptr -= element_size;
				destructor(ptr);
			}
		}
		catch(...)
		{
			std::terminate();
		}
		throw;
	}
}


//	Given the (data) address of an array, the number of elements, and the size of its elements,
//	call the given destructor on each element. If the destructor throws an exception, call terminate.
//	The destructor pointer may be NULL, in which case this routine does nothing. 
extern "C" void __cxa_vec_cleanup (
            void *array_address,
            _CXXABI_SIZE_T element_count,
            _CXXABI_SIZE_T element_size,
            void (*destructor) ( void *thisp ) )
{
	unsigned char *ptr;

	if(destructor==NULL) return;

	try
	{
		ptr=(unsigned char *)array_address + element_count*element_size;
		for(; element_count>0; --element_count)
		{
			ptr -= element_size;
			destructor(ptr);
		}
	}
	catch(...)
	{
		std::terminate();
	}
}

//	Given the (data) address of an array, the non-negative size of prefix padding for the cookie,
//	and the size of its elements, call the given destructor on each element,
//	using the cookie to determine the number of elements, and then delete the space.
//	If the destructor throws an exception, rethrow after destroying the remaining elements if possible.
//	If the destructor throws a second exception, call terminate. If padding_size is 0,
//	the destructor pointer must be NULL. If the destructor pointer is NULL,
//	no destructor call is to be made. 
extern "C" void __cxa_vec_delete (
            void *array_address,
            _CXXABI_SIZE_T element_size,
            _CXXABI_SIZE_T padding_size,
            void (*destructor) ( void *thisp ) )
{
	if(array_address!=NULL)
	{
		if(destructor!=NULL)
		{
			_CXXABI_SIZE_T element_count;

			//	get element count
			element_count = *(_CXXABI_SIZE_T *)((unsigned char *)array_address-sizeof(_CXXABI_SIZE_T));

			//	destroy the array using __cxa_vec_dtor (shouldn't this be __cxa_vec_cleanup?)
			__cxa_vec_dtor(array_address,element_count,element_size,destructor);
		}

		//	release storage
		::operator delete[]((unsigned char *)array_address-padding_size);
	}
}

//	Same as __cxa_vec_delete, except that the given function is used for deallocation
//	instead of the default delete function. If dealloc throws an exception,
//	the result is undefined. The dealloc pointer may not be NULL. 
extern "C" void __cxa_vec_delete2 (
            void *array_address,
            _CXXABI_SIZE_T element_size,
            _CXXABI_SIZE_T padding_size,
            void (*destructor) ( void *thisp ),
            void (*dealloc) ( void *obj ) )
{
	if(array_address!=NULL)
	{
		if(destructor!=NULL)
		{
			_CXXABI_SIZE_T element_count;

			//	get element count
			element_count = *(_CXXABI_SIZE_T *)((unsigned char *)array_address-sizeof(_CXXABI_SIZE_T));

			//	destroy the array using __cxa_vec_dtor (shouldn't this be __cxa_vec_cleanup?)
			__cxa_vec_dtor(array_address,element_count,element_size,destructor);
		}

		//	release storage
		dealloc((unsigned char *)array_address-padding_size);
	}
}


//	Same as __cxa_vec_delete, except that the given function is used for deallocation
//	instead of the default delete function. The deallocation function takes both the
//	object address and its size. If dealloc throws an exception, the result is undefined.
//	The dealloc pointer may not be NULL. 
extern "C" void __cxa_vec_delete3 (
            void *array_address,
            _CXXABI_SIZE_T element_size,
            _CXXABI_SIZE_T padding_size,
            void (*destructor) ( void *thisp ),
            void (*dealloc) ( void *obj, _CXXABI_SIZE_T size ) )
{
	if(array_address!=NULL)
	{
		_CXXABI_SIZE_T element_count;

		//	get element count
		element_count = *(_CXXABI_SIZE_T *)((unsigned char *)array_address-sizeof(_CXXABI_SIZE_T));

		if(destructor!=NULL)
		{
			//	destroy the array using __cxa_vec_dtor (shouldn't this be __cxa_vec_cleanup?)
			__cxa_vec_dtor(array_address,element_count,element_size,destructor);
		}

		//	release storage
		dealloc((unsigned char *)array_address-padding_size,element_count*element_size+padding_size);
	}
}

//	Given the (data) addresses of a destination and a source array, an element count
//	and an element size, call the given copy constructor to copy each element from the
//	source array to the destination array. If an exception occurs, call the given
//	destructor on each (if non-NULL) and rethrow. If the destructor throws an exception,
//	call terminate(). The constructor and or destructor pointers may be NULL.
//	If either is NULL, no action is taken when it would have been called. 
extern "C" void __cxa_vec_cctor (
            void *dest_array,
            void *src_array,
            _CXXABI_SIZE_T element_count,
            _CXXABI_SIZE_T element_size,
            void (*constructor) (void *, void *),
            void (*destructor) (void *))
{	//	currently not used by CW / not tested
	unsigned char *dptr,*sptr;

	if(constructor)
	{
		dptr=(unsigned char *)dest_array;
		sptr=(unsigned char *)src_array;
		if(destructor)
		{
			try
			{
				for(; element_count>0; --element_count)
				{
					constructor(dptr,sptr);
					dptr += element_size;
					sptr += element_size;
				}
			}
			catch(...)
			{
				DestroyArrayRange((unsigned char *)dest_array,dptr,element_size,destructor);
				throw;
			}
		}
		else
		{
			for(; element_count>0; --element_count)
			{
				constructor(dptr,sptr);
				dptr += element_size;
				sptr += element_size;
			}
		}
	}
}
