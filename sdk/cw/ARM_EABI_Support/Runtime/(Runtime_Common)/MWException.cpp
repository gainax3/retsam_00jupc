/* Metrowerks Runtime Library
 * Copyright © 1993-2005 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2005/02/03 18:06:14 $
 * $Revision: 1.2 $
 */

#if defined(__MC68K__) && !defined(__embedded__)
	#pragma far_data off
#endif

#include <ansi_parms.h>
#include <stdlib.h>
#include <exception.h>
#include <MWException.h>

#ifndef _MSL_NO_CPP_NAMESPACE
	namespace std {
#endif

#if __IA64_CPP_ABI__
	#define CONST_CHAR 'K'
#else
	#define CONST_CHAR 'C'
#endif

static void dthandler()		{ abort(); };			//	default terminate handler function
static terminate_handler	thandler = dthandler;	//	pointer to terminate handler function
static void duhandler()		{ terminate(); };		//	default unexpected handler function
static unexpected_handler	uhandler = duhandler;	//	pointer to unexpected handler function

/************************************************************************/
/*	Purpose..: 	Set a terminate handler function						*/
/*	Input....:	pointer to terminate handler function					*/
/*	Return...:	---														*/
/************************************************************************/
extern terminate_handler set_terminate(terminate_handler handler) _MSL_NO_THROW  // hh 980102 added exception specific
{
	terminate_handler old=thandler; thandler=handler; return old;
}

/************************************************************************/
/*	Purpose..: 	Terminate exception handling							*/
/*	Input....:	---														*/
/*	Return...:	--- (shall not return to caller)						*/
/************************************************************************/
extern void terminate()
{
	thandler();
}

/************************************************************************/
/*	Purpose..: 	Set an unexpected handler function						*/
/*	Input....:	pointer to unexpected handler function					*/
/*	Return...:	---														*/
/************************************************************************/
extern unexpected_handler set_unexpected(unexpected_handler handler) _MSL_NO_THROW  // hh 980102 added exception specific
{
	unexpected_handler old=uhandler; uhandler=handler; return old;
}

/************************************************************************/
/*	Purpose..: 	Handle unexpected exception								*/
/*	Input....:	---														*/
/*	Return...:	--- (shall not return to caller)						*/
/************************************************************************/
extern void unexpected()
{
	uhandler();
}

#ifndef _MSL_NO_CPP_NAMESPACE
	}
#endif

/************************************************************************/
/*	Purpose..: 	Register a global object for later destruction			*/
/*	Input....:	pointer to global object								*/
/*	Input....:	pointer to destructor function							*/
/*	Input....:	pointer to global registration structure				*/
/*	Return...:	pointer to global object (pass thru)					*/
/************************************************************************/
#ifndef __POWERPC__
extern void *__register_global_object(void *object,void *destructor,void *regmem)
{
	((DestructorChain *)regmem)->next=__global_destructor_chain;
	((DestructorChain *)regmem)->destructor=destructor;
	((DestructorChain *)regmem)->object=object;
	__global_destructor_chain=(DestructorChain *)regmem;

	return object;
}
#endif /* __POWERPC__ */

/************************************************************************/
/* Purpose..: Compare a throw and a catch type							*/
/* Input....: pointer to throw type										*/
/* Input....: pointer to catch type (0L: catch(...)						*/
/* Return...: true: can catch; false: cannot catch						*/
/************************************************************************/
extern char __throw_catch_compare(const char *throwtype,const char *catchtype,long *offset_result)
{
	const char	*cptr1,*cptr2;

	*offset_result=0;	
	if((cptr2=catchtype)==0)
	{	//	catch(...)
		return true;
	}
	cptr1=throwtype;

	if(*cptr2=='P')
	{	//	catch(cv T *)
		cptr2++;
#if __IA64_CPP_ABI__
		if(*cptr2=='V') cptr2++;
		if(*cptr2==CONST_CHAR) cptr2++;
#else
		if(*cptr2==CONST_CHAR) cptr2++;
		if(*cptr2=='V') cptr2++;
#endif
		if(*cptr2=='v')
		{	//	catch(cv void *)
			if(*cptr1=='P' || *cptr1=='*')
			{	//	throw T*;
				return true;
			}
		}
		cptr2=catchtype;
	}

	switch(*cptr1)
	{	//	class pointer/reference throw
	case '*':
	case '!':
		if(*cptr1++!=*cptr2++) return false;
		for(;;)
		{	//	class name compare loop
			if(*cptr1==*cptr2++)
			{
				if(*cptr1++=='!')
				{	//	class match found / get offset
					long offset;

					for(offset=0; *cptr1!='!';) offset=offset*10+*cptr1++-'0';
					*offset_result=offset; return true;
				}
			}
			else
			{
				while(*cptr1++!='!') ;			//	skip class name
				while(*cptr1++!='!') ;			//	skip offset
				if(*cptr1==0) return false;		//	no more class names => no match
				cptr2=catchtype+1;				//	retry with next class name
			}
		}
		return false;
	}

	while((*cptr1=='P' || *cptr1=='R') && *cptr1==*cptr2)
	{	//	pointer/reference catch => match cv-qualifiers
		cptr1++; cptr2++;
		if(*cptr2==CONST_CHAR)
		{	//	ignore 'const' in throw type
			if(*cptr1==CONST_CHAR) cptr1++;
			cptr2++;
		}
		if(*cptr1==CONST_CHAR) return false;	//	throw type is more cv-qualified

		if(*cptr2=='V')
		{	//	ignore 'volatile' in throw type
			if(*cptr1=='V') cptr1++;
			cptr2++;
		}
		if(*cptr1=='V') return false;	//	throw type is more cv-qualified
	}

	//	plain type throw catch
	for(; *cptr1==*cptr2; cptr1++,cptr2++) if(*cptr1==0) return true;
	return false;
}

// JWW 040713 Split common ABI routines from NMWException.cpp