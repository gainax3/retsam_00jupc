/*---------------------------------------------------------------------------*
  Project:  NitroDWC - base - memfunc
  File:     dwc_memfunc.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_memfunc.h,v $
  Revision 1.2  2006/04/25 12:08:00  nakata
  メモリ確保タイプにDWC_ALLOCTYPE_NDを追加

  Revision 1.1  2005/08/19 06:32:03  sasakit
  ファイルの追加。

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_MEMFUNC_H_
#define DWC_MEMFUNC_H_

#include <nitro/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** --------------------------------------------------------------------
  enums
  ----------------------------------------------------------------------*/
typedef enum
{
    DWC_ALLOCTYPE_AUTH,
    DWC_ALLOCTYPE_AC,
    DWC_ALLOCTYPE_BM,
    DWC_ALLOCTYPE_UTIL,
    DWC_ALLOCTYPE_BASE,
    DWC_ALLOCTYPE_GS,
	DWC_ALLOCTYPE_ND,
	DWC_ALLOCTYPE_ENC,
	DWC_ALLOCTYPE_MAIL,
    DWC_ALLOCTYPE_LAST
} DWCAllocType;

/** --------------------------------------------------------------------
  typedefs
  ----------------------------------------------------------------------*/
typedef void* (*DWCAllocEx)( DWCAllocType name, u32   size, int align );
typedef void  (*DWCFreeEx )( DWCAllocType name, void* ptr,  u32 size  );

/** --------------------------------------------------------------------
  functions
  ----------------------------------------------------------------------*/
void  DWC_SetMemFunc( DWCAllocEx alloc, DWCFreeEx free );
void* DWC_Alloc     ( DWCAllocType name, u32 size );
void* DWC_AllocEx   ( DWCAllocType name, u32 size, int align );
void  DWC_Free      ( DWCAllocType name, void* ptr, u32 size );
void* DWC_Realloc   ( DWCAllocType name, void* ptr, u32 oldsize, u32 newsize );
void* DWC_ReallocEx ( DWCAllocType name, void* ptr, u32 oldsize, u32 newsize, int align );

void* DWCi_GsMalloc  (size_t size);
void* DWCi_GsRealloc (void* ptr, size_t size);
void  DWCi_GsFree    (void* ptr);
void* DWCi_GsMemalign(size_t boundary, size_t size);


#ifdef __cplusplus
}
#endif

#endif // DWC_MEMFUNC_H_
