/* Metrowerks Runtime Library
 * Copyright © 2003-2005 Metrowerks Corporation.  All rights reserved.
 *
 * Purpose:  Implementation of the portable Itanium C++ ABI
 *           see http://www.codesourcery.com/cxx-abi/
 *
 * $Date: 2005/09/27 20:29:56 $
 * $Revision: 1.6 $
 */

#ifndef __CXXABI_H__
#define __CXXABI_H__

#include <typeinfo>

/* Do not use stddef.h to get size_t and ptrdiff_t, instead let the compiler figure it out */
typedef __typeof__(sizeof(0)) _CXXABI_SIZE_T;
typedef __typeof__((char*)0-(char*)0) _CXXABI_PTRDIFF_T;

#ifndef _MSL_IMP_EXP_RUNTIME
	/* If not using MSL, define the import/export macro to do nothing */
	#define _MSL_IMP_EXP_RUNTIME
#endif

/*	Older versions of MSL's <typeinfo> must be updated to use this type_info for the __IA64_CPP_ABI__ !!! 

namespace std {

	class type_info {
	public:
		virtual ~type_info();
		bool operator==(const type_info &rhs) const;
		bool operator!=(const type_info &rhs) const;
		bool before(const type_info &rhs) const;
		const char* name() const {return __type_name;}
	private:
		type_info (const type_info& rhs);				//  not defined
		type_info& operator= (const type_info& rhs);	//  not defined

		const char *__type_name;
	};

}

*/

namespace __cxxabiv1 {
	class _MSL_IMP_EXP_RUNTIME __fundamental_type_info : public std::type_info {
	public:
		virtual ~__fundamental_type_info();

		//	no additional members
	};

	class _MSL_IMP_EXP_RUNTIME __array_type_info : public std::type_info {
	public:
		virtual ~__array_type_info();

		//	no additional members
	};

	class _MSL_IMP_EXP_RUNTIME __function_type_info : public std::type_info {
	public:
		virtual ~__function_type_info();

		//	no additional members
	};

	class _MSL_IMP_EXP_RUNTIME __enum_type_info : public std::type_info {
	public:
		virtual ~__enum_type_info();

		//	no additional members
	};

	class _MSL_IMP_EXP_RUNTIME __class_type_info : public std::type_info {
	public:
		virtual ~__class_type_info();

		//	no additional members
	};

	class _MSL_IMP_EXP_RUNTIME __si_class_type_info : public __class_type_info {
	public:
		virtual ~__si_class_type_info();

		const __class_type_info *__base_type;
	};

	struct _MSL_IMP_EXP_RUNTIME __base_class_type_info {
	public:
		const __class_type_info *__base_type;

#if 0
	//	32-bit ABI (no longer used)
		int				__offset;
		unsigned int	__flags;

		enum _flags_masks {
			__virtual_mask = 0x1,
			__public_mask = 0x2
		};

		inline int __get_offset() const { return __offset; }
		inline int __is_virtual() const { return __flags&__virtual_mask; }
		inline int __is_public() const { return __flags&__public_mask; }
#else
	//	64-bit ABI
		long __offset_flags;

		enum __offset_flags_masks {
			__virtual_mask = 0x1,
			__public_mask = 0x2,
			__offset_shift = 8
		};

		inline long __get_offset() const { return __offset_flags>>__offset_shift; }
		inline int __is_virtual() const { return __offset_flags&__virtual_mask; }
		inline int __is_public() const { return __offset_flags&__public_mask; }
#endif
	};

	class _MSL_IMP_EXP_RUNTIME __vmi_class_type_info : public __class_type_info {
	public:
		virtual ~__vmi_class_type_info();

		unsigned int __flags;
		unsigned int __base_count;
		__base_class_type_info __base_info[1];

		enum __flags_masks {
			__non_diamond_repeat_mask = 0x1,
			__diamond_shaped_mask = 0x2
		};
	};

	class _MSL_IMP_EXP_RUNTIME __pbase_type_info : public std::type_info {
	public:
		virtual ~__pbase_type_info();

		unsigned int __flags;
		const std::type_info *__pointee;

		enum __masks {
			__const_mask = 0x1,
			__volatile_mask = 0x2,
			__restrict_mask = 0x4,
			__incomplete_mask = 0x8,
			__incomplete_class_mask = 0x10
		};
	};

	class _MSL_IMP_EXP_RUNTIME __pointer_type_info : public __pbase_type_info {
	public:
		virtual ~__pointer_type_info();

		//	no additional members
	};

	class _MSL_IMP_EXP_RUNTIME __pointer_to_member_type_info : public __pbase_type_info {
	public:
		virtual ~__pointer_to_member_type_info();

		//	no additional members
	};
}
namespace abi = __cxxabiv1;

//	Prototypes

//	cxxabi_alloc.cpp
extern "C" void * __cxa_vec_new (
            _CXXABI_SIZE_T element_count,
            _CXXABI_SIZE_T element_size,
            _CXXABI_SIZE_T padding_size,
            void (*constructor) ( void *thisp ),
            void (*destructor) ( void *thisp ) );

extern "C" void * __cxa_vec_new2 (
            _CXXABI_SIZE_T element_count,
            _CXXABI_SIZE_T element_size,
            _CXXABI_SIZE_T padding_size,
            void (*constructor) ( void *thisp ),
            void (*destructor) ( void *thisp ),
            void* (*alloc) ( _CXXABI_SIZE_T size ),
            void (*dealloc) ( void *obj ) );

extern "C" void * __cxa_vec_new3 (
            _CXXABI_SIZE_T element_count,
            _CXXABI_SIZE_T element_size,
            _CXXABI_SIZE_T padding_size,
            void (*constructor) ( void *thisp ),
            void (*destructor) ( void *thisp ),
            void* (*alloc) ( _CXXABI_SIZE_T size ),
            void (*dealloc) ( void *obj, _CXXABI_SIZE_T size ) );

extern "C" void __cxa_vec_ctor (
            void *array_address,
            _CXXABI_SIZE_T element_count,
            _CXXABI_SIZE_T element_size,
            void (*constructor) ( void *thisp ),
            void (*destructor) ( void *thisp ) );

extern "C" void __cxa_vec_dtor (
            void *array_address,
            _CXXABI_SIZE_T element_count,
            _CXXABI_SIZE_T element_size,
            void (*destructor) ( void *thisp ) );

extern "C" void __cxa_vec_cleanup (
            void *array_address,
            _CXXABI_SIZE_T element_count,
            _CXXABI_SIZE_T element_size,
            void (*destructor) ( void *thisp ) );

extern "C" void __cxa_vec_delete (
            void *array_address,
            _CXXABI_SIZE_T element_size,
            _CXXABI_SIZE_T padding_size,
            void (*destructor) ( void *thisp ) );

extern "C" void __cxa_vec_delete2 (
            void *array_address,
            _CXXABI_SIZE_T element_size,
            _CXXABI_SIZE_T padding_size,
            void (*destructor) ( void *thisp ),
            void (*dealloc) ( void *obj ) );

extern "C" void __cxa_vec_delete3 (
            void *array_address,
            _CXXABI_SIZE_T element_size,
            _CXXABI_SIZE_T padding_size,
            void (*destructor) ( void *thisp ),
            void (*dealloc) ( void *obj, _CXXABI_SIZE_T size ) );

extern "C" void __cxa_vec_cctor (
            void *dest_array,
            void *src_array,
            _CXXABI_SIZE_T element_count,
            _CXXABI_SIZE_T element_size,
            void (*constructor) (void *, void *),
            void (*destructor) (void *));

//	cxxabi_badeh.cpp
extern "C" _MSL_IMP_EXP_RUNTIME void __cxa_bad_cast ();
extern "C" _MSL_IMP_EXP_RUNTIME void __cxa_bad_typeid ();

//	cxxabi_onetimeinit.cpp
#if __IA64_CPP_ABI__ && __POWERPC__
extern "C" _MSL_IMP_EXP_RUNTIME __declspec(weak) int __cxa_guard_acquire ( long long *guard_object );
extern "C" _MSL_IMP_EXP_RUNTIME __declspec(weak) void __cxa_guard_release ( long long *guard_object );
extern "C" _MSL_IMP_EXP_RUNTIME __declspec(weak) void __cxa_guard_abort ( long long *guard_object );
#elif __IA64_CPP_ABI_ARM__
extern "C" _MSL_IMP_EXP_RUNTIME __declspec(weak) int __cxa_guard_acquire ( int *guard_object );
extern "C" _MSL_IMP_EXP_RUNTIME __declspec(weak) void __cxa_guard_release ( int *guard_object );
extern "C" _MSL_IMP_EXP_RUNTIME __declspec(weak) void __cxa_guard_abort ( int *guard_object );
#else
extern "C" _MSL_IMP_EXP_RUNTIME __declspec(weak) int __cxa_guard_acquire ( unsigned char *guard_object );
extern "C" _MSL_IMP_EXP_RUNTIME __declspec(weak) void __cxa_guard_release ( unsigned char *guard_object );
extern "C" _MSL_IMP_EXP_RUNTIME __declspec(weak) void __cxa_guard_abort ( unsigned char *guard_object );
#endif

//	cxxabi_rtti.cpp
extern "C" _MSL_IMP_EXP_RUNTIME void* __dynamic_cast (
	const void *sub,
	const abi::__class_type_info *src,
	const abi::__class_type_info *dst,
	_CXXABI_PTRDIFF_T src2dst_offset
);

#endif
