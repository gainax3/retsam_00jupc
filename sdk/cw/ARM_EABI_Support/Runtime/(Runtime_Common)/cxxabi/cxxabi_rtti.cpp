/* Metrowerks Runtime Library
 * Copyright © 2003-2005 Metrowerks Corporation.  All rights reserved.
 *
 * Purpose:  Implementation of the portable Itanium C++ ABI
 *           see http://www.codesourcery.com/cxx-abi/
 *
 * $Date: 2005/03/07 21:33:04 $
 * $Revision: 1.4 $
 */

#include <cxxabi.h>
#include <cstring>

#if !defined(__MSL__) && !defined(__std)
	#define __std(ref) ref
#endif

#ifndef __CXXABI_SAFE_DYNAMICCAST
#define __CXXABI_SAFE_DYNAMICCAST			1	//	1: use slower dynamic_cast compare that can handle multiple class typeinfo instances
#endif
#ifndef __CXXABI_SAFE_TYPEINFO_COMPARE
#define __CXXABI_SAFE_TYPEINFO_COMPARE		1	//	1: use slower typeinfo compares that can handle multiple typeinfo instances
#endif

namespace std {

	//	std::type_info member function definitions
	type_info::~type_info()
	{	//	dummy destructor function (triggers vtable generation in this file)
	}
	bool type_info::operator==(const type_info &rhs) const
	{
#if __CXXABI_SAFE_TYPEINFO_COMPARE
		return this==&rhs || strcmp(__type_name, rhs.__type_name) == 0;
#else
		return this==&rhs;
#endif
	}
	bool type_info::operator!=(const type_info &rhs) const
	{
#if __CXXABI_SAFE_TYPEINFO_COMPARE
		return this!=&rhs && strcmp(__type_name, rhs.__type_name) != 0;
#else
		return this!=&rhs;
#endif
	}
	bool type_info::before(const type_info &rhs) const
	{
#if __CXXABI_SAFE_TYPEINFO_COMPARE
		return strcmp(__type_name, rhs.__type_name) < 0;
#else
		return this<&rhs;
#endif
	}
	//	type_info::type_info (const type_info& rhs);				//  not defined
	//	type_info& type_info::operator= (const type_info& rhs);		//  not defined

}

namespace abi {

	//	dummy destructor functions (triggers vtable generation in this file)
	__fundamental_type_info::~__fundamental_type_info() {}
	__array_type_info::~__array_type_info() {}
	__function_type_info::~__function_type_info() {}
	__enum_type_info::~__enum_type_info() {}
	__class_type_info::~__class_type_info() {}
	__si_class_type_info::~__si_class_type_info() {}
	__vmi_class_type_info::~__vmi_class_type_info() {}
	__pbase_type_info::~__pbase_type_info() {}
	__pointer_type_info::~__pointer_type_info() {}
	__pointer_to_member_type_info::~__pointer_to_member_type_info() {}

}

/*

Basic type information (e.g. for "int", "bool", etc.) will be kept in the run-time support
library. Specifically, the run-time support library should contain type_info objects for
the types X, X* and X const*, for every X in: void, bool, wchar_t, char, unsigned char,
signed char, short, unsigned short, int, unsigned int, long, unsigned long, long long,
unsigned long long, float, double, long double. (Note that various other type_info objects
for class types may reside in the run-time support library by virtue of the preceding
rules, e.g. that of std::bad_alloc.) 

*/

namespace {

	//	POD version of __fundamental_type_info class
	struct fundamentalPOD {
		const void	*vtablepointer;
		const char	*__type_name;
	};

	//	POD version of __pointer_type_info class
	struct pointerPOD {
		const void	*vtablepointer;
		const char	*__type_name;
		unsigned int __flags;
		const void	*__pointee;
	};

}

//	dummy vtable declarations
#define ABI_NS_NAME(name)			_ZTVN10__cxxabiv1 ## name ## E
#define CLASS_TYPEINFO_NAME			ABI_NS_NAME(17__class_type_info)
#define POINTER_TYPEINFO_NAME		ABI_NS_NAME(19__pointer_type_info)
#define SI_CLASS_TYPEINFO_NAME		ABI_NS_NAME(20__si_class_type_info)
#define VMI_CLASS_TYPEINFO_NAME		ABI_NS_NAME(21__vmi_class_type_info)
#define FUNDAMENTAL_TYPEINFO_NAME	ABI_NS_NAME(23__fundamental_type_info)

extern "C" const unsigned char CLASS_TYPEINFO_NAME[];
extern "C" const unsigned char SI_CLASS_TYPEINFO_NAME[];
extern "C" const unsigned char VMI_CLASS_TYPEINFO_NAME[];
extern "C" const unsigned char FUNDAMENTAL_TYPEINFO_NAME[];
extern "C" const unsigned char POINTER_TYPEINFO_NAME[];

#define FUNDAMENTAL_TYPE_INIT(mangledtypename) 						\
	extern const char _ZTS ## mangledtypename[] = #mangledtypename;		\
	extern const fundamentalPOD _ZTI ## mangledtypename = { FUNDAMENTAL_TYPEINFO_NAME+sizeof(void*)*2, _ZTS ## mangledtypename }

#define POINTER_TYPE_INIT(mangledtypename) 							\
	extern const char _ZTSP ## mangledtypename[] = "P" #mangledtypename;	\
	extern const pointerPOD _ZTIP ## mangledtypename = { POINTER_TYPEINFO_NAME+sizeof(void*)*2, _ZTSP ## mangledtypename, 0, &_ZTI ## mangledtypename }

#define POINTER_CONST_TYPE_INIT(mangledtypename) 					\
	extern const char _ZTSPK ## mangledtypename[] = "PK" #mangledtypename;	\
	extern const pointerPOD _ZTIPK ## mangledtypename = { POINTER_TYPEINFO_NAME+sizeof(void*)*2, _ZTSPK ## mangledtypename, abi::__pbase_type_info::__const_mask, &_ZTI ## mangledtypename }

#define BASIC_TYPE_INIT(mangledtypename)							\
	FUNDAMENTAL_TYPE_INIT(mangledtypename);							\
	POINTER_TYPE_INIT(mangledtypename);								\
	POINTER_CONST_TYPE_INIT(mangledtypename)

extern "C" {

	BASIC_TYPE_INIT(v);		//	void
	BASIC_TYPE_INIT(b);		//	bool
	BASIC_TYPE_INIT(u7__bool8);		//	bool
	BASIC_TYPE_INIT(u8__bool32);	//	bool
	BASIC_TYPE_INIT(w);		//	wchar_t
	BASIC_TYPE_INIT(c);		//	char
	BASIC_TYPE_INIT(a);		//	signed char
	BASIC_TYPE_INIT(h);		//	unsigned char
	BASIC_TYPE_INIT(s);		//	short
	BASIC_TYPE_INIT(t);		//	unsigned short
	BASIC_TYPE_INIT(i);		//	int
	BASIC_TYPE_INIT(j);		//	unsigned int
	BASIC_TYPE_INIT(l);		//	long
	BASIC_TYPE_INIT(m);		//	unsigned long
	BASIC_TYPE_INIT(x);		//	long long
	BASIC_TYPE_INIT(y);		//	unsigned long long
	BASIC_TYPE_INIT(f);		//	float
	BASIC_TYPE_INIT(d);		//	double
	BASIC_TYPE_INIT(e);		//	long double
}

namespace {

	struct RTTIVTableHeader {									//	the RTTI data header in a vtable
		_CXXABI_PTRDIFF_T				md_offset;				//	offset of most derived class
		abi::__class_type_info			*md_type_info;			//	pointer to most derived class type_info struct
	};

	struct ClassTreeWalkResult {
		//	static information
		const unsigned char 			*sub;					//	pointer to subobject object
		const abi::__class_type_info	*src;					//	pointer to source typeinfo
		const abi::__class_type_info	*dst;					//	pointer to destination typeinfo

		//	most derived path information
		const unsigned char 			*mdst;					//	pointer to last found dst object in most derived
		bool							mdst_is_public;			//	true: last found dst is a public base class of most derived
		bool							mdst_is_ambig;			//	true: found more than one mdst in most derived
		bool							msrc_is_public;			//	true: src is a public base class of most derived

		//	dst derived path information
		const unsigned char 			*pdst;					//	pointer to last found dst object in most derived
		bool							psrc_is_public;			//	true: scr is a public base class of pdst
		bool							pdst_is_ambig;			//	true: found more than one pdst class

		//	current state information
		const unsigned char 			*dst_parent;			//	pointer to parent dst object (NULL: no dst parent)
		bool				 			on_public_pdst_path;	//	on a public parent dst object path
		bool							on_public_md_path;		//	true: on a public most-derived path
	};

	/****************************************************************/
	/* Purpose..: Compare two vtable pointers 						*/
	/* Input....: pointer to vtable pointer							*/
	/* Input....: pointer to vtable pointer							*/
	/* Returns..: true: vtables pointers for identical class object	*/
	/****************************************************************/
	inline bool ClassRTTIVTablePointerCompare(const unsigned char *vtbl,const unsigned char *tvtbl)
	{
		if(vtbl==tvtbl) return true;
#if __CXXABI_SAFE_DYNAMICCAST
		//	slow / safe version
		if(vtbl!=NULL)
		{	//	get type_info pointers from vtable header
			vtbl=(const unsigned char *)((RTTIVTableHeader *)vtbl)->md_type_info;
			tvtbl=(const unsigned char *)((RTTIVTableHeader *)tvtbl)->md_type_info;
			if(vtbl!=NULL)
			{
				return __std(strcmp(((fundamentalPOD *)vtbl)->__type_name, ((fundamentalPOD *)tvtbl)->__type_name)) == 0;
			}
		}
#endif
		return false;
	}

	/****************************************************************/
	/* Purpose..: Recursive class tree walk							*/
	/* Input....: pointer to class object							*/
	/* Input....: pointer to class object's __class_type_info		*/
	/* Input....: pointer to ClassTreeWalkResult struct				*/
	/* Returns..: ---												*/
	/****************************************************************/
	static void ClassTreeWalk(const unsigned char *obj,const abi::__class_type_info *typeinfo,ClassTreeWalkResult *result)
	{
		const unsigned char *vtblptr;
		const unsigned char	*psave;
		bool				bsave1,bsave2;

		if(result->sub==obj && *result->src==*typeinfo)
		{	//	we have found the base class sub object
			if(result->on_public_md_path) result->msrc_is_public=true;

			if(result->dst_parent!=NULL)
			{	//	src is a base class of dst
				if(result->pdst==NULL)
				{	//	first time
					result->pdst	= result->dst_parent;
				}
				else
				{	//	already found
					if(result->pdst!=result->dst_parent)
					{	//	found more than one dst in most derived
						result->pdst_is_ambig	= true;
					}
				}
				if(result->on_public_pdst_path) result->psrc_is_public=true;
			}
			return;
		}

		//	save current state information
		psave	= result->dst_parent;
		bsave1	= result->on_public_pdst_path;
		bsave2	= result->on_public_md_path;

		if(*result->dst==*typeinfo)
		{	//	we have found a class object with the destination type
			if(result->mdst==NULL)
			{	//	first time
				result->mdst	= obj;
			}
			else
			{	//	already found
				if(result->mdst!=obj)
				{	//	found more than one dst in most derived
					result->mdst_is_ambig	= true;
				}
			}
			if(result->on_public_md_path) result->mdst_is_public=true;
			result->dst_parent 			= obj;
			result->on_public_pdst_path = true;
		}

		//	use type's vtable pointer to detect the proper class type (poor man's RTTI ;-) )
		//	this could probably be done using dynamic_cast, but this will be much faster
		vtblptr	= *(const unsigned char **)typeinfo - (sizeof(void *)+sizeof(_CXXABI_PTRDIFF_T));
		if(ClassRTTIVTablePointerCompare(vtblptr,VMI_CLASS_TYPEINFO_NAME))
		{	//	n base classes
			const abi::__vmi_class_type_info	*info;
			const abi::__base_class_type_info	*base;
			const unsigned char					*baseobj;
			int									i,n;
			bool								nbsave1,nbsave2;

nbases:		info 	= reinterpret_cast<const abi::__vmi_class_type_info *>(typeinfo);
			nbsave1	= result->on_public_pdst_path;
			nbsave2	= result->on_public_md_path;
			for(i=0,n=info->__base_count; i<n; i++)
			{
				base=&info->__base_info[i];
				if(!base->__is_public())
				{	//	base class not public
					result->on_public_pdst_path = false;
					result->on_public_md_path	= false;
				}

				//	compute the address of the base class
				if(base->__is_virtual())
				{
					baseobj=obj + *(_CXXABI_PTRDIFF_T *)(*(const unsigned char **)obj+base->__get_offset());
					//	this code is for the old vtable pointer implementation
					//	baseobj=*(const unsigned char **)(obj+base->__get_offset());
				}
				else baseobj=obj+base->__get_offset();

				//	recursively visit base class
				ClassTreeWalk(baseobj,base->__base_type,result);
				result->on_public_pdst_path = nbsave1;
				result->on_public_md_path	= nbsave2;
			}
		}
		else if(ClassRTTIVTablePointerCompare(vtblptr,SI_CLASS_TYPEINFO_NAME))
		{	//	a single non-virtual public base class
			const abi::__si_class_type_info	*info;

onebase:	info = reinterpret_cast<const abi::__si_class_type_info *>(typeinfo);
			ClassTreeWalk(obj,info->__base_type,result);
		}
#if 0
		else if(ClassRTTIVTablePointerCompare(vtblptr,CLASS_TYPEINFO_NAME))
		{
			//	no base classes, nothing to do
		}
		else
		{
//			ClassRTTIVTablePointerCompare() already does a slow/safe check, so this is redundant
//			if(ClassRTTIVTablePointerCompare(vtblptr,VMI_CLASS_TYPEINFO_NAME)) goto nbases;
//			if(ClassRTTIVTablePointerCompare(vtblptr,SI_CLASS_TYPEINFO_NAME)) goto onebase;
		}
#endif

		//	restore current state information
		result->dst_parent			= psave;
		result->on_public_pdst_path	= bsave1;
		result->on_public_md_path	= bsave2;
	}
}

/* sub: source address to be adjusted; nonnull, and since the
 *      source object is polymorphic, *(void**)sub is a virtual pointer.
 * src: static type of the source object.
 * dst: destination type (the "T" in "dynamic_cast<T>(v)").
 * src2dst_offset: a static hint about the location of the
 *    source subobject with respect to the complete object;
 *    special negative values are:
 *       -1: no hint
 *       -2: src is not a public base of dst
 *       -3: src is a multiple public base type but never a
 *           virtual base type
 *    otherwise, the src type is a unique public nonvirtual
 *    base type of dst at offset src2dst_offset from the
 *    origin of dst.
 */
extern "C" void* __dynamic_cast (
	const void *sub,
	const abi::__class_type_info *src,
	const abi::__class_type_info *dst,
	_CXXABI_PTRDIFF_T 	/* src2dst_offset currently ignored */
)
{
	RTTIVTableHeader	*vthead;
	ClassTreeWalkResult	result;

//	if(sub==NULL) return NULL;		//	should not be necessary	

	//	get a pointer to the vtable's RTTI data
	vthead=*(RTTIVTableHeader **)((const char *)sub)-1;
	if(vthead->md_type_info==NULL)
	{	//	error: the class was compiled without the RTTI option
		return NULL;
	}

	//	walk the class tree starting from the most-derived class object to find a valid dynamic cast dst object
	result.sub					= (const unsigned char *)sub;
	result.src					= src;
	result.dst					= dst;

	result.mdst					= NULL;
	result.mdst_is_public		= false;
	result.mdst_is_ambig		= false;
	result.msrc_is_public		= false;

	result.pdst					= NULL;
	result.psrc_is_public		= false;
	result.pdst_is_ambig		= false;

	result.dst_parent			= NULL;
	result.on_public_pdst_path	= false;
	result.on_public_md_path	= true;

	ClassTreeWalk((const unsigned char *)sub+vthead->md_offset,vthead->md_type_info,&result);

	//	If, in the most derived object pointed (referred) to by v, v points (refers) to a public base class
	//	sub-object of a T object, and if only one object of type T is derived from the sub-object pointed (referred)
	//	to by v, the result is a pointer (an lvalue referring) to that T object. 
	if(result.pdst!=NULL && result.psrc_is_public && !result.pdst_is_ambig)
	{
		return (void *)result.pdst;
	}

	//	Otherwise, if v points (refers) to a public base class sub-object of the most derived object,
	//	and the type of the most derived object has a base class, of type T, that is unambiguous and public,
	//	the result is a pointer (an lvalue referring) to the T sub-object of the most derived object. 
	if(result.mdst!=NULL && result.mdst_is_public && result.msrc_is_public && !result.mdst_is_ambig)
	{
		return (void *)result.mdst;
	}

	//	Otherwise, the run-time check fails. 
	return NULL;
}
