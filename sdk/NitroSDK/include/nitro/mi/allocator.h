/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI - include
  File:     allocator.h

  Copyright 2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/
#ifndef	NITRO_MI_ALLOCATOR_H_
#define	NITRO_MI_ALLOCATOR_H_


#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/platform.h>


#ifdef __cplusplus
extern  "C"
{
#endif


/*---------------------------------------------------------------------------*/
/* declarations */

/* メモリアロケータ関数プロトタイプ */
typedef void* (*MIAllocatorAllocFunction)(void *userdata, u32 length, u32 alignment);
typedef void  (*MIAllocatorFreeFunction)(void *userdata, void *buffer);

/* メモリアロケータ構造体 */
typedef struct MIAllocator
{
    void                       *userdata;
    MIAllocatorAllocFunction    Alloc;
    MIAllocatorFreeFunction     Free;
}
MIAllocator;


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         MI_InitAllocator

  Description:  アロケータを初期化.

  Arguments:    allocator        初期化するMIAllocator構造体.
                userdata         任意のユーザ定義引数.
                alloc            メモリ確保関数のポインタ.
                free             メモリ解放関数のポインタ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
void MI_InitAllocator(MIAllocator *allocator, void *userdata,
                      MIAllocatorAllocFunction alloc,
                      MIAllocatorFreeFunction free)
{
    allocator->userdata = userdata;
    allocator->Alloc = alloc;
    allocator->Free = free;
}

/*---------------------------------------------------------------------------*
  Name:         MI_CallAlloc

  Description:  アロケータからメモリ確保.

  Arguments:    allocator        初期化されたMIAllocator構造体.
                length           確保するサイズ.
                alignment        必要な境界整合 (2のべき乗である必要がある)

  Returns:      確保されたメモリまたはNULL.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
void* MI_CallAlloc(MIAllocator *allocator, u32 length, u32 alignment)
{
    return allocator->Alloc(allocator->userdata, length, alignment);
}

/*---------------------------------------------------------------------------*
  Name:         MI_CallFree

  Description:  アロケータへメモリ解放.

  Arguments:    allocator        初期化されたMIAllocator構造体.
                buffer           解放するメモリ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
void MI_CallFree(MIAllocator *allocator, void *buffer)
{
    allocator->Free(allocator->userdata, buffer);
}


/*---------------------------------------------------------------------------*/


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* NITRO_MI_ALLOCATOR_H_ */
/*---------------------------------------------------------------------------*
  $Log: allocator.h,v $
  Revision 1.1  2007/04/11 08:04:55  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
