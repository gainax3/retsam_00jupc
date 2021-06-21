/* Metrowerks Runtime Library
 * Copyright © 1993-2005 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2005/02/03 18:06:23 $
 * $Revision: 1.2 $
 */

#if defined(__MC68K__) && !defined(__embedded__)
	#pragma far_data off
#endif

#include <ansi_parms.h>
#include <cstdlib>
#include <CPlusLib.h>

// We do not support #pragma exception_arrayinit in the new backend . hwp
#if defined(__MC68K__) && (__BACKENDVERSION__ < 2)

/************************************************************************/
/*	Purpose..: 	Initialize a new allocated array of objects				*/
/*	Input....:	pointer to allocated memory (+8 bytes) (0L: error)		*/
/*	Input....:	pointer to default constructor function	(or 0L)			*/
/*	Input....:	pointer to destructor function (or 0L)					*/
/*	Input....:	size of one array element								*/
/*	Input....:	number of array elements								*/
/*	Return...:	pointer to first object in array						*/
/************************************************************************/
extern void *__construct_new_array(void *block,ConstructorDestructor ctor,ConstructorDestructor dtor_arg,size_t size,size_t n)
{
	char	*ptr;

	if((ptr=(char *)block)!=0L)
	{
		size_t *p = (size_t *)ptr;
		//	store number of allocated objects and size of one object at the beginnig of the allocated block
		p[0]=size;
		p[1]=n;
		ptr+=ARRAY_HEADER_SIZE;

		if(ctor)
		{	//	call constructor to initialize array
			volatile ConstructorDestructor	dtor=dtor_arg;	//	force reference to dtor
			char							*p;
			size_t							i;

			#pragma exception_arrayinit
			//	this #pragma adds partial array construction exception action (ptr,dtor,size,i)

			for(i=0,p=ptr; i<n; i++,p+=size)
			{
				CTORCALL_COMPLETE(ctor,p);
			}
		}
	}
	return ptr;	//	return pointer to first object;
}

/************************************************************************/
/*	Purpose..: 	Construct an array of objects							*/
/*	Input....:	pointer to array memory									*/
/*	Input....:	pointer to default constructor function					*/
/*	Input....:	pointer to destructor function (or 0L)					*/
/*	Input....:	size of one array element								*/
/*	Input....:	number of array elements								*/
/*	Return...:	---														*/
/************************************************************************/
extern void __construct_array(void *ptr,ConstructorDestructor ctor,ConstructorDestructor dtor_arg,size_t size,size_t n)
{
	volatile ConstructorDestructor	dtor=dtor_arg;	//	force reference to dtor
	char							*p;
	size_t							i;

	#pragma exception_arrayinit
	//	this #pragma adds partial array construction exception action (ptr,dtor,size,i)

	for(i=0,p=(char *)ptr; i<n; i++,p+=size)
	{
		CTORCALL_COMPLETE(ctor,p);
	}
}

#else

//	class __partial_array_destructor
//
//		This class is used to guarantee correct destruction of partially
//		constructed arrays if an exception is thrown from an array-element constructor.
//		We use the members of this class to keep track of the partially-constructed
//		state of the array. If the destructor for this class is called and the
//		array was not fully-constructed, we must have thrown an exception, so we
//		destroy each fully-constructed element of the array in reverse order.

class __partial_array_destructor {
private:
	void*					p;		//	pointer to start of array being constructed
	size_t					size;	//	size of each array element
	size_t					n;		//	# of elements being constructed
	ConstructorDestructor	dtor;	//	pointer to destructor for elements or 0
public:
	size_t					i;		//	current element being constructed
	
	__partial_array_destructor(void* array, size_t elementsize, size_t nelements, ConstructorDestructor destructor)
	{
		p=array; size=elementsize; n=nelements; dtor=destructor; i=n;
	}
		
	_MSL_IMP_EXP_RUNTIME ~__partial_array_destructor()
	{
		char *ptr;
		
		if(i<n && dtor)
		{
			for(ptr=(char *)p+size*i; i>0; i--)
			{
				ptr-=size;
				DTORCALL_COMPLETE(dtor,ptr);
			}
		}
	}
};

/************************************************************************/
/*	Purpose..: 	Initialize a new allocated array of objects				*/
/*	Input....:	pointer to allocated memory (+8 bytes) (0L: error)		*/
/*	Input....:	pointer to default constructor function	(or 0L)			*/
/*	Input....:	pointer to destructor function (or 0L)					*/
/*	Input....:	size of one array element								*/
/*	Input....:	number of array elements								*/
/*	Return...:	pointer to first object in array						*/
/************************************************************************/
extern void *__construct_new_array(void *block,ConstructorDestructor ctor,ConstructorDestructor dtor,size_t size,size_t n)
{
	char	*ptr;

	if((ptr=(char *)block)!=0L)
	{
		size_t *p = (size_t *)ptr;
		//	store number of allocated objects and size of one object at the beginnig of the allocated block
		p[0]=size;
		p[1]=n;
		ptr+=ARRAY_HEADER_SIZE;

		if(ctor)
		{	//	call constructor to initialize array
			__partial_array_destructor pad(ptr,size,n,dtor);
			char	*p;

			for(pad.i=0,p=(char *)ptr; pad.i<n; pad.i++,p+=size) CTORCALL_COMPLETE(ctor,p);
		}
	}
	return ptr;	//	return pointer to first object;
}

/************************************************************************/
/*	Purpose..: 	Construct an array of objects							*/
/*	Input....:	pointer to array memory									*/
/*	Input....:	pointer to default constructor function					*/
/*	Input....:	pointer to destructor function (or 0L)					*/
/*	Input....:	size of one array element								*/
/*	Input....:	number of array elements								*/
/*	Return...:	---														*/
/************************************************************************/
extern void __construct_array(void *ptr,ConstructorDestructor ctor,ConstructorDestructor dtor,size_t size,size_t n)
{
	__partial_array_destructor pad(ptr,size,n,dtor);
	char	*p;

	for(pad.i=0,p=(char *)ptr; pad.i<n; pad.i++,p+=size) CTORCALL_COMPLETE(ctor,p);
}

#endif /* __MC68K__*/

/************************************************************************/
/*	Purpose..: 	Destroy an array of objects								*/
/*	Input....:	pointer to array memory									*/
/*	Input....:	pointer to destructor function							*/
/*	Input....:	size of one object										*/
/*	Input....:	number of objects										*/
/*	Return...:	---														*/
/************************************************************************/
extern void __destroy_arr(void *block,ConstructorDestructor dtor,size_t size,size_t n)
{
	char	*p;

	for(p=(char *)block+size*n; n>0; n--)
	{
		p-=size;
		DTORCALL_COMPLETE(dtor,p);
	}
}

//  hh 971207 Added namespace support
//  hh 971215 commented out <Types.h> per John McEnerney's instructions
//  hh 980102 added exception specific to set_terminate and set_unexpected
// JWW 000418 export partial array destructor to remove dependency on the .exp file
// JWW 010329 Moved __destroy_new_array out of NMWException.cp and removed 68K support
// JWW 010531 Synchronized Mach-O and PEF runtime code
// JWW 040713 Split off construct and destruct array code from NMWException.cp