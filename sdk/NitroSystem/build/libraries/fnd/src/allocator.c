/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - fnd
  File:     allocator.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.10 $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nnsys/misc.h>
#include <nnsys/fnd/expheap.h>
#include <nnsys/fnd/frameheap.h>
#include <nnsys/fnd/unitheap.h>
#include <nnsys/fnd/allocator.h>

/* ========================================================================
    static関数
   ======================================================================== */

/* ------------------------------------------------------------------------
    Exp Heap 用
   ------------------------------------------------------------------------ */

static void*
AllocatorAllocForExpHeap(
    NNSFndAllocator*    pAllocator,
    u32                 size
)
{
    NNSFndHeapHandle const heap = pAllocator->pHeap;
    int const alignment = (int)pAllocator->heapParam1;
    return NNS_FndAllocFromExpHeapEx(heap, size, alignment);
}

static void
AllocatorFreeForExpHeap(
    NNSFndAllocator*    pAllocator,
    void*               memBlock
)
{
    NNSFndHeapHandle const heap = pAllocator->pHeap;
    NNS_FndFreeToExpHeap(heap, memBlock);
}

/* ------------------------------------------------------------------------
    Frame Heap 用
   ------------------------------------------------------------------------ */

static void*
AllocatorAllocForFrmHeap(
    NNSFndAllocator*    pAllocator,
    u32                 size
)
{
    NNSFndHeapHandle const heap = pAllocator->pHeap;
    int const alignment = (int)pAllocator->heapParam1;
    return NNS_FndAllocFromFrmHeapEx(heap, size, alignment);
}

/*
    フレームヒープではメモリブロック単位のメモリ解放ができないため、
    この実装では何もしないことにする。
*/
static void
AllocatorFreeForFrmHeap(
    NNSFndAllocator*    /*pAllocator*/,
    void*               /*memBlock*/
)
{
}


/* ------------------------------------------------------------------------
    Unit Heap 用
   ------------------------------------------------------------------------ */

/*
    ユニットヒープのメモリブロックサイズを超えるサイズの割り当てはできないので
    NULLを返す。
*/
static void*
AllocatorAllocForUnitHeap(
    NNSFndAllocator*    pAllocator,
    u32                 size
)
{
    NNSFndHeapHandle const heap = pAllocator->pHeap;

    if (size > NNS_FndGetMemBlockSizeForUnitHeap(heap))
    {
        return NULL;
    }

    return NNS_FndAllocFromUnitHeap(heap);
}

static void
AllocatorFreeForUnitHeap(
    NNSFndAllocator*    pAllocator,
    void*               memBlock
)
{
    NNSFndHeapHandle const heap = pAllocator->pHeap;
    NNS_FndFreeToUnitHeap(heap, memBlock);
}


/* ------------------------------------------------------------------------
    SDK heap 用
   ------------------------------------------------------------------------ */

static void*
AllocatorAllocForSDKHeap(
    NNSFndAllocator*    pAllocator,
    u32                 size
)
{
    OSHeapHandle const heap = (int)pAllocator->pHeap;
    OSArenaId const id = (OSArenaId)pAllocator->heapParam1;
    return OS_AllocFromHeap(id, heap, size);
}

static void
AllocatorFreeForSDKHeap(
    NNSFndAllocator*    pAllocator,
    void*               memBlock
)
{
    OSHeapHandle const heap = (int)pAllocator->pHeap;
    OSArenaId const id = (OSArenaId)pAllocator->heapParam1;
    OS_FreeToHeap(id, heap, memBlock);
}



/* ========================================================================
    外部関数
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNS_FndAllocFromAllocator

  Description:  アロケータからメモリブロック確保します。

                実際には、どのように確保されるかどうかはアロケータと
                それに関連付けられているメモリマネージャの実装によります。

  Arguments:    pAllocator:  NNSFndAllocator構造体のアドレス。
                size:        メモリブロックのサイズ(バイト)。

  Returns:      メモリブロックを確保できた場合、そのメモリブロックの先頭アドレスを返します。
                確保できなかった場合、NULL を返します。
 *---------------------------------------------------------------------------*/
void*
NNS_FndAllocFromAllocator(
    NNSFndAllocator*    pAllocator,
    u32                 size
)
{
    NNS_ASSERT(pAllocator);
    return (*pAllocator->pFunc->pfAlloc)(pAllocator, size);
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndFreeToAllocator

  Description:  メモリブロックを解放しアロケータへ返却します。

                実際には、どのように返却されるかどうかはアロケータと
                それに関連付けられているメモリマネージャの実装によります。

  Arguments:    pAllocator:  NNSFndAllocator構造体のアドレス。
                memBlock:    解放するメモリブロックへのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void
NNS_FndFreeToAllocator(
    NNSFndAllocator*    pAllocator,
    void*               memBlock
)
{
    NNS_ASSERT(pAllocator && memBlock);
    (*pAllocator->pFunc->pfFree)(pAllocator, memBlock);
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndInitAllocatorForExpHeap

  Description:  拡張ヒープからメモリの確保と解放を行うようにアロケータを初期化します。
                アロケータを通じて確保される全てのメモリブロックのアライメント値は
                引数alignmentで指定した値になります。

  Arguments:    pAllocator:  NNSFndAllocator構造体のアドレス。
                heap:        拡張ヒープのハンドル。
                alignment:   確保する各メモリブロックに適用するアライメント値

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void
NNS_FndInitAllocatorForExpHeap(
    NNSFndAllocator*    pAllocator,
    NNSFndHeapHandle    heap,
    int                 alignment
)
{
    static const NNSFndAllocatorFunc sAllocatorFunc =
    {
        AllocatorAllocForExpHeap,
        AllocatorFreeForExpHeap,
    };

    pAllocator->pFunc = &sAllocatorFunc;
    pAllocator->pHeap = heap;
    pAllocator->heapParam1 = (u32)alignment;
    pAllocator->heapParam2 = 0; // no use
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndInitAllocatorForFrmHeap

  Description:  フレームヒープからメモリの確保と解放を行うようにアロケータを初期化します。
                アロケータを通じて確保される全てのメモリブロックのアライメント値は
                引数alignmentで指定した値になります。

  Arguments:    pAllocator:  NNSFndAllocator構造体のアドレス。
                heap:        フレームヒープのハンドル。
                alignment:   確保する各メモリブロックに適用するアライメント値

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void
NNS_FndInitAllocatorForFrmHeap(
    NNSFndAllocator*    pAllocator,
    NNSFndHeapHandle    heap,
    int                 alignment
)
{
    static const NNSFndAllocatorFunc sAllocatorFunc =
    {
        AllocatorAllocForFrmHeap,
        AllocatorFreeForFrmHeap,
    };

    pAllocator->pFunc = &sAllocatorFunc;
    pAllocator->pHeap = heap;
    pAllocator->heapParam1 = (u32)alignment;
    pAllocator->heapParam2 = 0; // no use
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndInitAllocatorForUnitHeap

  Description:  ユニットヒープからメモリの確保と解放を行うようにアロケータを初期化します。
                ユニットヒープのメモリブロックサイズより大きなメモリブロックを確保することは出来ません。
                その場合、関数NNS_FndAllocFromAllocator() はNULLを返します。

  Arguments:    pAllocator:  NNSFndAllocator構造体のアドレス。
                heap:        ユニットヒープのハンドル。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void
NNS_FndInitAllocatorForUnitHeap(
    NNSFndAllocator*    pAllocator,
    NNSFndHeapHandle    heap
)
{
    static const NNSFndAllocatorFunc sAllocatorFunc =
    {
        AllocatorAllocForUnitHeap,
        AllocatorFreeForUnitHeap,
    };

    pAllocator->pFunc = &sAllocatorFunc;
    pAllocator->pHeap = heap;
    pAllocator->heapParam1 = 0; // no use
    pAllocator->heapParam2 = 0; // no use
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndInitAllocatorForSDKHeap

  Description:  NITRO-SDKのOS_CreateHeap()関数で作成されるヒープから
                メモリの確保と解放を行うようにアロケータを初期化します。

  Arguments:    pAllocator:  NNSFndAllocator構造体のアドレス。
                id:          ヒープのあるアリーナのアリーナID。
                heap:        ヒープのハンドル。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void
NNS_FndInitAllocatorForSDKHeap(
    NNSFndAllocator*    pAllocator,
    OSArenaId           id,
    OSHeapHandle        heap
)
{
    static const NNSFndAllocatorFunc sAllocatorFunc =
    {
        AllocatorAllocForSDKHeap,
        AllocatorFreeForSDKHeap,
    };

    pAllocator->pFunc = &sAllocatorFunc;
    pAllocator->pHeap = (void*)heap;
    pAllocator->heapParam1 = id;
    pAllocator->heapParam2 = 0; // no use
}

