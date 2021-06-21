/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - fnd
  File:     unitheap.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.17 $
 *---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <nitro.h>
#include <nnsys/misc.h>
#include <nnsys/fnd/unitheap.h>
#include <nnsys/fnd/config.h>
#include "heapcommoni.h"


/* ========================================================================
    マクロ定数
   ======================================================================== */

// アライメントの最小値
#define MIN_ALIGNMENT           4


/* ========================================================================
    static関数
   ======================================================================== */

/* ------------------------------------------------------------------------
    メモリブロックリスト操作
   ------------------------------------------------------------------------ */

static NNSiFndUntHeapMBlockHead*
PopMBlock(NNSiFndUntMBlockList* list)
{
    NNSiFndUntHeapMBlockHead* block = list->head;
    if (block)
    {
        list->head = block->pMBlkHdNext;
    }

    return block;
}

/*---------------------------------------------------------------------------*
  Name:         PushMBlock

  Description:  メモリブロックをリストの先頭に追加します。

  Arguments:    link:   追加するリスト
                block:  追加するメモリブロック

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static NNS_FND_INLINE void
PushMBlock(
    NNSiFndUntMBlockList*       list,
    NNSiFndUntHeapMBlockHead*   block
)
{
    block->pMBlkHdNext = list->head;
    list->head = block;
}


/*---------------------------------------------------------------------------*
  Name:         GetUnitHeapHeadPtrFromHeapHead

  Description:  ヒープヘッダへのポインタから、ユニットヒープヘッダへのポインタを取得します。

  Arguments:    pHeapHd:  ヒープヘッダへのポインタ。

  Returns:      ユニットヒープヘッダへのポインタを返します。
 *---------------------------------------------------------------------------*/
static NNS_FND_INLINE NNSiFndUntHeapHead*
GetUnitHeapHeadPtrFromHeapHead(NNSiFndHeapHead* pHeapHd)
{
    return AddU32ToPtr(pHeapHd, sizeof(NNSiFndHeapHead));
}

static NNS_FND_INLINE BOOL
IsValidUnitHeapHandle(NNSFndHeapHandle handle)
{
    if(handle == NNS_FND_HEAP_INVALID_HANDLE)
    {
        return FALSE;
    }

    {
        NNSiFndHeapHead* pHeapHd = handle;
        return pHeapHd->signature == NNSI_UNTHEAP_SIGNATURE;
    }
}


/* ========================================================================
    外部関数(非公開)
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNSi_FndDumpUnitHeap

  Description:  ユニットヒープ内部の情報を表示します。
                これはデバッグ用の関数です。

  Arguments:    heap:    ユニットヒープのハンドル。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
#if ! defined(NNS_FINALROM)

    void
    NNSi_FndDumpUnitHeap(NNSFndHeapHandle heap)
    {
        NNS_ASSERT(IsValidUnitHeapHandle(heap));

        {
            NNSiFndHeapHead *const pHeapHd = heap;
            NNSiFndUntHeapHead *const pUnitHeapHd = GetUnitHeapHeadPtrFromHeapHead(pHeapHd);
            const u32 heapSize = GetOffsetFromPtr(pHeapHd->heapStart, pHeapHd->heapEnd);

        	const u32 freeSize = NNS_FndCountFreeBlockForUnitHeap(heap) * pUnitHeapHd->mBlkSize;
        	const u32 usedSize = heapSize - freeSize;

            NNSi_FndDumpHeapHead(pHeapHd);

        	OS_Printf( "    %d / %d bytes (%6.2f%%) used\n",
        											usedSize, heapSize, 100.0f * usedSize / heapSize);
        }
    }

// #if ! defined(NNS_FINALROM)
#endif


/* ========================================================================
    外部関数(公開)
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNS_FndCreateUnitHeapEx

  Description:  ユニットヒープを作成します。

  Arguments:    startAddress:  ヒープ領域の先頭アドレス。
                heapSize:      ヒープ領域のサイズ。
                memBlockSize:  メモリブロックのサイズ。
                alignment:     メモリブロックのアライメント。
                               4,8,16,32のいずれかの値が指定できます。
                optFlag:       オプションフラグ。

  Returns:      関数が成功した場合、作成されたユニットヒープのハンドルが返ります。
                関数が失敗すると、NNS_FND_INVALID_HEAP_HANDLE が返ります。
 *---------------------------------------------------------------------------*/
NNSFndHeapHandle
NNS_FndCreateUnitHeapEx(
    void*   startAddress,
    u32     heapSize,
    u32     memBlockSize,
    int     alignment,
    u16     optFlag
)
{
    NNSiFndHeapHead* pHeapHd;
    void* heapEnd;

    SDK_NULL_ASSERT(startAddress);

    // alignment のチェック
    NNS_ASSERT(alignment % MIN_ALIGNMENT == 0);
    NNS_ASSERT(MIN_ALIGNMENT <= alignment && alignment <= 32);

    pHeapHd = NNSi_FndRoundUpPtr(startAddress, MIN_ALIGNMENT);
    heapEnd = NNSi_FndRoundDownPtr(AddU32ToPtr(startAddress, heapSize), MIN_ALIGNMENT);

    if (ComparePtr(pHeapHd, heapEnd) > 0)
    {
        return NNS_FND_HEAP_INVALID_HANDLE;
    }

    memBlockSize = NNSi_FndRoundUp(memBlockSize, alignment);    // 実質のブロックサイズ

    {
        NNSiFndUntHeapHead* pUntHeapHd = GetUnitHeapHeadPtrFromHeapHead(pHeapHd);
        void* heapStart = NNSi_FndRoundUpPtr(AddU32ToPtr(pUntHeapHd, sizeof(NNSiFndUntHeapHead)), alignment);
        u32 elementNum;

        if (ComparePtr(heapStart, heapEnd) > 0)
        {
            return NNS_FND_HEAP_INVALID_HANDLE;
        }

        elementNum = GetOffsetFromPtr(heapStart, heapEnd) / memBlockSize;
        if (elementNum == 0)
        {
            return NNS_FND_HEAP_INVALID_HANDLE;
        }

        heapEnd = AddU32ToPtr(heapStart, elementNum * memBlockSize);

        NNSi_FndInitHeapHead(           // ヒープ共通初期化
            pHeapHd,
            NNSI_UNTHEAP_SIGNATURE,
            heapStart,
            heapEnd,
            optFlag);

        pUntHeapHd->mbFreeList.head = heapStart;
        pUntHeapHd->mBlkSize = memBlockSize;

        {
            NNSiFndUntHeapMBlockHead* pMBlkHd = pUntHeapHd->mbFreeList.head;
            int i;

            for (i = 0; i < elementNum - 1; ++i, pMBlkHd = pMBlkHd->pMBlkHdNext)
            {
                pMBlkHd->pMBlkHdNext = AddU32ToPtr(pMBlkHd, memBlockSize);
            }

            pMBlkHd->pMBlkHdNext = NULL;
        }

        return pHeapHd;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndDestroyUnitHeap

  Description:  ユニットヒープを破棄します。

  Arguments:    heap: ユニットヒープのハンドル。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void
NNS_FndDestroyUnitHeap(NNSFndHeapHandle heap)
{
    NNS_ASSERT(IsValidUnitHeapHandle(heap));

    NNSi_FndFinalizeHeap(heap);
}


/*---------------------------------------------------------------------------*
  Name:         NNS_FndAllocFromUnitHeap

  Description:  ユニットヒープからメモリブロックを確保します。

  Arguments:    heap:   ユニットヒープのハンドル。

  Returns:      メモリブロックの確保が成功した場合、確保したメモリブロックへの
                ポインタが返ります。
                失敗した場合、NULLが返ります。
 *---------------------------------------------------------------------------*/
void*
NNS_FndAllocFromUnitHeap(NNSFndHeapHandle heap)
{
    NNS_ASSERT(IsValidUnitHeapHandle(heap));

    {
        NNSiFndUntHeapHead* pUntHeapHd = GetUnitHeapHeadPtrFromHeapHead(heap);
        NNSiFndUntHeapMBlockHead* pMBlkHd = PopMBlock(&pUntHeapHd->mbFreeList);

        if (pMBlkHd)
        {
            FillAllocMemory(heap, pMBlkHd, pUntHeapHd->mBlkSize);
        }

        return pMBlkHd;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndFreeToUnitHeap

  Description:  ユニットヒープへメモリブロックを返却します。

  Arguments:    heap:     ユニットヒープのハンドル。
                memBlock: 返却するメモリブロックへのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void
NNS_FndFreeToUnitHeap(
    NNSFndHeapHandle    heap,
    void*               memBlock
)
{
    NNS_ASSERT(IsValidUnitHeapHandle(heap));

    {
        NNSiFndUntHeapHead* pUntHeapHd = GetUnitHeapHeadPtrFromHeapHead(heap);

        FillFreeMemory(heap, memBlock, pUntHeapHd->mBlkSize);

        PushMBlock(&pUntHeapHd->mbFreeList, memBlock);
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndCountFreeBlockForUnitHeap

  Description:  ユニットヒープの空きメモリブロック数を取得します。

  Arguments:    heap:     ユニットヒープのハンドル。

  Returns:      ユニットヒープの空きメモリブロック数を返します。
 *---------------------------------------------------------------------------*/
u32
NNS_FndCountFreeBlockForUnitHeap(NNSFndHeapHandle heap)
{
    NNS_ASSERT(IsValidUnitHeapHandle(heap));

    {
        NNSiFndUntHeapHead* pUntHeapHd = GetUnitHeapHeadPtrFromHeapHead(heap);
        NNSiFndUntHeapMBlockHead* pMBlkHd = pUntHeapHd->mbFreeList.head;
        u32 cnt = 0;

        for (; pMBlkHd; pMBlkHd = pMBlkHd->pMBlkHdNext)
        {
            ++cnt;
        }

        return cnt;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndCalcHeapSizeForUnitHeap

  Description:  メモリブロックのサイズと個数から必要なヒープのサイズを取得します。

  Arguments:    memBlockSize:  メモリブロックのサイズ(バイト値)。
                memBlockNum:   確保するメモリブロックの総数。
                alignment:     メモリブロックのアライメント。

  Returns:      必要なヒープのサイズを返します。
 *---------------------------------------------------------------------------*/
u32
NNS_FndCalcHeapSizeForUnitHeap(
    u32     memBlockSize,
    u32     memBlockNum,
    int     alignment
)
{
    return
          // ヒープが内部で使用するサイズ
          sizeof(NNSiFndHeapHead) + sizeof(NNSiFndUntHeapHead)

          // アライメントの調整に必要なサイズの最大
        + (alignment - 4)

          // 全ユニットが必要とするサイズ
        + memBlockNum * NNSi_FndRoundUp(memBlockSize, alignment);
}
