/* Metrowerks Runtime Library
 * Copyright © 1993-2005 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2005/02/03 18:05:47 $
 * $Revision: 1.2 $
 */

#include <CPlusLib.h>

/************************************************************************/
/*	Purpose..: 	Delete an array of objects								*/
/*	Input....:	pointer to first object	(can be NULL)					*/
/*	Input....:	pointer to destructor function							*/
/*	Return...:	---														*/
/************************************************************************/
extern void __destroy_new_array(void *block,ConstructorDestructor dtor)
{
	if(block)
	{
		if(dtor)
		{
			size_t	i,objects,objectsize;
			char	*p;

			objectsize=*(size_t *)((char *)block-ARRAY_HEADER_SIZE);
			objects=((size_t *)((char *)block-ARRAY_HEADER_SIZE))[1];
			p=(char *)block+objectsize*objects;
			for(i=0; i<objects; i++)
			{
				p-=objectsize;
				DTORCALL_COMPLETE(dtor,p);
			}
		}
		#if __MWERKS__>=0x2020
		::operator delete[] ((char *)block-ARRAY_HEADER_SIZE);
		#else
		::operator delete ((char *)block-ARRAY_HEADER_SIZE);
		#endif
	}
}

/************************************************************************/
/*	Purpose..: 	Destroy a new allocated array of objects				*/
/*	Input....:	pointer to first object	(can be NULL)					*/
/*	Input....:	pointer to destructor function (not NULL)				*/
/*	Return...:	pointer to complete array block							*/
/************************************************************************/
extern void *__destroy_new_array2(void *block,ConstructorDestructor dtor)
{
	size_t	i,objects,objectsize;
	char	*p;

	if(block==NULL) return NULL;

	if(dtor)
	{	//	destroy array members
		objectsize=*(size_t *)((char *)block-ARRAY_HEADER_SIZE);
		objects=((size_t *)((char *)block-ARRAY_HEADER_SIZE))[1];
		p=(char *)block+objectsize*objects;
		for(i=0; i<objects; i++)
		{
			p-=objectsize;
			DTORCALL_COMPLETE(dtor,p);
		}
	}

	//	return pointer to complete array block
	return (char *)block-ARRAY_HEADER_SIZE;
}

/************************************************************************/
/*	Purpose..: 	Destroy/delete a new allocated array of objects			*/
/*	Input....:	pointer to first object	(or NULL)						*/
/*	Input....:	pointer to destructor function (or NULL)				*/
/*	Input....:	pointer to deallocation function (or NULL)				*/
/*	Input....:	1: deallocation has size_t parameter					*/
/*	Return...:	pointer to complete array block							*/
/************************************************************************/
extern void __destroy_new_array3(void *block,ConstructorDestructor dtor,void *dealloc_func,short has_size_t_param)
{
	size_t	i,elements,elementsize;
	char	*p;

	if(block)
	{
		elements	= ((size_t *)((char *)block-ARRAY_HEADER_SIZE))[1];
		elementsize	= *(size_t *)((char *)block-ARRAY_HEADER_SIZE);

		if(dtor!=NULL)
		{	//	destroy array elements
			p=(char *)block+elements*elementsize;
			for(i=0; i<elements; i++)
			{
				p-=elementsize;
				DTORCALL_COMPLETE(dtor,p);
			}
		}

		if(dealloc_func!=NULL)
		{	//	call deallocation functiuon
			p=(char *)block-ARRAY_HEADER_SIZE;
			if(has_size_t_param)
			{
				((void (*)(void *,size_t))dealloc_func)(p,elements*elementsize+ARRAY_HEADER_SIZE);
			}
			else ((void (*)(void *))dealloc_func)(p);
		}
	}
}

// JWW 010329 Moved __destroy_new_array out of NMWException.cp
// JWW 010531 Synchronized Mach-O and PEF runtime code