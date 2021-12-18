/* Metrowerks Runtime Library
 * Copyright © 1993-2005 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2005/02/03 18:06:16 $
 * $Revision: 1.2 $
 */

#ifndef __MW_MWEXCEPTION_H__
#define __MW_MWEXCEPTION_H__

#include <ansi_parms.h>

////	[this section needs to be copied from CException.h]

typedef short	vbase_ctor_arg_type;
typedef char	local_cond_type;

typedef struct CatchInfo {
	void			*location;				//	pointer to complete exception object
	void			*typeinfo;				//	pointer to complete exception object's typeinfo
	void			*dtor;					//	pointer to complete exception object's destructor function (0: no destructor)
	void			*sublocation;			//	pointer to handlers subobject in complete exception object
	long			pointercopy;			//	adjusted pointer copy for pointer matches (sublocation will point to the loaction)
	void			*stacktop;				//	pointer to handlers stack top (reused for exception specification in specification handler)
}	CatchInfo;

////	[this section needs to be copied from CException.h]

typedef struct DestructorChain {			//	global destructor chain
	struct DestructorChain	*next;			//	pointer to next destructor chain element
	void					*destructor;	//	pointer to destructor function
	void					*object;		//	pointer to memory location (0L: element is marker)
}	DestructorChain;

_MSL_BEGIN_EXTERN_C

_MSL_IMP_EXP_RUNTIME extern DestructorChain	*__global_destructor_chain;
_MSL_IMP_EXP_RUNTIME extern void			*__register_global_object(void *object,void *destructor,void *regmem);
_MSL_IMP_EXP_RUNTIME extern void			__destroy_global_chain(void);

extern void									__init__catch(CatchInfo* catchinfo);
_MSL_IMP_EXP_RUNTIME extern void			__end__catch(CatchInfo* catchinfo);
_MSL_IMP_EXP_RUNTIME extern void			__throw(char *throwtype,void *location,void *dtor);
_MSL_IMP_EXP_RUNTIME extern void			__rethrow(void);
_MSL_IMP_EXP_RUNTIME extern char			__throw_catch_compare(const char *throwtype,const char *catchtype,long *offset_result);
_MSL_IMP_EXP_RUNTIME extern void			__unexpected(CatchInfo* catchinfo);
void __call_static_initializers(void);  // hh 971215 added per John McEnerney's instructions

#if defined(__PPC_EABI__)
	_MSL_IMP_EXP_RUNTIME extern int			__register_fragment(void *info, char *TOC);
	_MSL_IMP_EXP_RUNTIME extern void		__unregister_fragment(int fragmentID);
#elif defined(__POWERPC__)
	#ifdef __MACH__
		#include <mach-o/loader.h>
		
		typedef struct mach_header mach_header;
		__declspec(private_extern) int		__register_mach_sorted(char *code_start, char *code_end, char *exception_start, char *exception_end);
		__declspec(private_extern) int		__register_mach_header(const struct mach_header *header, unsigned long vmaddr_slide);
		__declspec(private_extern) void		__unregister_mach_header(int fragmentID);
	#else
		_MSL_IMP_EXP_RUNTIME extern int		__register_fragment(char *code_start,char *code_end,char *data_start,char *data_end,
												char *exception_start,char *exception_end,char *TOC);
		_MSL_IMP_EXP_RUNTIME extern void	__unregister_fragment(int fragmentID);
	#endif /* __MACH__ */
#endif /* __PPC_EABI__ */

_MSL_END_EXTERN_C

#endif /* __MW_MWEXCEPTION_H__ */

// hh  971215 added prototype per John McEnerney's instructions
// JWW 000418 export routines to remove dependency on the .exp file
// JWW 010531 Removed 68K conditional & synchronized Mach-O and PEF runtime code