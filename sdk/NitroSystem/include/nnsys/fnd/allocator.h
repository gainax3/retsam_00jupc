/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - fnd
  File:     allocator.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.6 $
 *---------------------------------------------------------------------------*/

#ifndef NNS_FND_ALLOCATOR_H_
#define NNS_FND_ALLOCATOR_H_

#include <nnsys/fnd/heapcommon.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================
    型定義
   ======================================================================== */

typedef struct NNSFndAllocator NNSFndAllocator;

typedef void*   (*NNSFndFuncAllocatorAlloc)(
                    NNSFndAllocator*    pAllocator,
                    u32                 size);

typedef void    (*NNSFndFuncAllocatorFree)(
                    NNSFndAllocator*    pAllocator,
                    void*               memBlock);

typedef struct NNSFndAllocatorFunc NNSFndAllocatorFunc;

struct NNSFndAllocatorFunc
{
    NNSFndFuncAllocatorAlloc    pfAlloc;
    NNSFndFuncAllocatorFree     pfFree;
};

struct NNSFndAllocator
{
    NNSFndAllocatorFunc const * pFunc;
    void*                       pHeap;
    u32                         heapParam1;
    u32                         heapParam2;
};


/* ========================================================================
    関数プロトタイプ
   ======================================================================== */

void*           NNS_FndAllocFromAllocator(
                    NNSFndAllocator*    pAllocator,
                    u32                 size);

void            NNS_FndFreeToAllocator(
                    NNSFndAllocator*    pAllocator,
                    void*               memBlock);

void            NNS_FndInitAllocatorForExpHeap(
                    NNSFndAllocator*    pAllocator,
                    NNSFndHeapHandle    heap,
                    int                 alignment);

void            NNS_FndInitAllocatorForFrmHeap(
                    NNSFndAllocator*    pAllocator,
                    NNSFndHeapHandle    heap,
                    int                 alignment);

void            NNS_FndInitAllocatorForUnitHeap(
                    NNSFndAllocator*    pAllocator,
                    NNSFndHeapHandle    heap);

void            NNS_FndInitAllocatorForSDKHeap(
                    NNSFndAllocator*    pAllocator,
                    OSArenaId           id,
                    OSHeapHandle        heap);


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NNS_FND_ALLOCATOR_H_ */
#endif
