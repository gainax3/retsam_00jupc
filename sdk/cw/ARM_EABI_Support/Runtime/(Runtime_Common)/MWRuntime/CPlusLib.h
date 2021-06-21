/* Metrowerks Runtime Library
 * Copyright © 1993-2005 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2005/02/03 18:05:42 $
 * $Revision: 1.2 $
 */

#ifndef __MW_CPLUSLIB_H__
#define __MW_CPLUSLIB_H__

#include <ansi_parms.h>
#include <stddef.h>
#include <MWRuntime.h>

typedef struct PTMF {
	long	this_delta;					//	delta to this pointer
	long	vtbl_offset;				//	offset of virtual function pointer in vtable (<0: non-virtual function address)
	union {
		void	*func_addr;				//	non-virtual function address
		long	ventry_offset;			//	offset of vtable pointer in class
	}	func_data;
}	PTMF;

_MSL_BEGIN_EXTERN_C

extern long							__ptmf_test(const PTMF *ptmf);
extern long							__ptmf_cmpr(const PTMF *ptmf1,const PTMF *ptmf2);
extern void							__ptmf_call(...);
extern void							__ptmf_call4(...);
extern void							__ptmf_scall(...);
extern void							__ptmf_scall4(...);

extern PTMF							*__ptmf_cast(long offset,const PTMF *ptmfrom,PTMF *ptmto);

_MSL_IMP_EXP_RUNTIME extern void	*__copy(char *to,char *from,size_t size);
_MSL_IMP_EXP_RUNTIME extern void	*__clear(char *mem,size_t size);
_MSL_IMP_EXP_RUNTIME extern void	*__init_arr(void *memptr,ConstructorDestructor constructor,size_t object_size,size_t nobjects);
_MSL_IMP_EXP_RUNTIME extern void	*__new_arr(ConstructorDestructor constructor,size_t object_size,size_t objects);
_MSL_IMP_EXP_RUNTIME extern void	__del_arr(void *memptr,ConstructorDestructor destructor);
_MSL_IMP_EXP_RUNTIME extern void	__dc_arr(void *mem,ConstructorDestructor con_des,short object_size,short objects);

_MSL_IMP_EXP_RUNTIME extern void	__construct_array(void *block,ConstructorDestructor ctor,ConstructorDestructor dtor,size_t size,size_t n);
_MSL_IMP_EXP_RUNTIME extern void	__destroy_arr(void *block,ConstructorDestructor dtor,size_t size,size_t n);
_MSL_IMP_EXP_RUNTIME extern void	*__construct_new_array(void *block,ConstructorDestructor ctor,ConstructorDestructor dtor,size_t size,size_t n);

extern void	__destroy_new_array(void *block,ConstructorDestructor dtor);
extern void	*__destroy_new_array2(void *block,ConstructorDestructor dtor);
extern void	__destroy_new_array3(void *block,ConstructorDestructor dtor,void *dealloc_func,short has_size_t_param);

_MSL_END_EXTERN_C

#endif /* __MW_CPLUSLIB_H__ */

// JWW 000418 export routines to remove dependency on the .exp file
// AH  001002 added __clear() prototype
// JWW 010329 Removed 68K support and added ARRAY_HEADER_SIZE (previously in NMWException.cp)
// JWW 010402 Made __destroy_new_array variants not _MSL_IMP_EXP_RUNTIME since they're static now
// JWW 010531 Synchronized Mach-O and PEF runtime code
// JWW 040713 Moved common ABI routines to MWRuntime.h