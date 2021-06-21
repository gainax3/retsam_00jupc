/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - fnd
  File:     unitheap.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.8 $
 *---------------------------------------------------------------------------*/

#ifndef NNS_FND_UNITHEAP_H_
#define NNS_FND_UNITHEAP_H_

#include <nnsys/fnd/heapcommon.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================
    型定義
   ======================================================================== */

typedef struct NNSiFndUntHeapMBlockHead NNSiFndUntHeapMBlockHead;

// メモリブロックのヘッダ情報
struct NNSiFndUntHeapMBlockHead
{
    NNSiFndUntHeapMBlockHead*  pMBlkHdNext;    // 次ブロック
};


typedef struct NNSiFndUntMBlockList NNSiFndUntMBlockList;

// メモリブロックのリスト
struct NNSiFndUntMBlockList
{
    NNSiFndUntHeapMBlockHead*  head;           // 先頭に繋がれているメモリブロックへのポインタ
};


typedef struct NNSiFndUntHeapHead NNSiFndUntHeapHead;

// ユニットヒープのヘッダ情報
struct NNSiFndUntHeapHead
{
    NNSiFndUntMBlockList    mbFreeList;     // フリーリスト
    u32                     mBlkSize;       // メモリブロックサイズ
};


/* ========================================================================
    マクロ関数
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNS_FndCreateUnitHeap

  Description:  ユニットヒープを作成します。

  Arguments:    startAddress:  ヒープ領域の先頭アドレス。
                heapSize:      ヒープ領域のサイズ。
                memBlockSize:  メモリブロックのサイズ。

  Returns:      関数が成功した場合、作成されたユニットヒープのハンドルが返ります。
                関数が失敗すると、NNS_FND_INVALID_HEAP_HANDLE が返ります。
 *---------------------------------------------------------------------------*/
#define                 NNS_FndCreateUnitHeap(startAddress, heapSize, memBlockSize) \
                            NNS_FndCreateUnitHeapEx(startAddress, heapSize, memBlockSize, NNS_FND_HEAP_DEFAULT_ALIGNMENT, 0)


/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetMemBlockSizeForUnitHeap

  Description:  ユニットヒープのメモリブロックサイズを取得します。

  Arguments:    heap:  ユニットヒープのハンドル。

  Returns:      ユニットヒープのメモリブロックサイズを返します。
 *---------------------------------------------------------------------------*/
#define                 NNS_FndGetMemBlockSizeForUnitHeap(heap) \
                            (((const NNSiFndUntHeapHead*)((const u8*)((const void*)(heap)) + sizeof(NNSiFndHeapHead)))->mBlkSize)


/* ========================================================================
    関数プロトタイプ
   ======================================================================== */

#if ! defined(NNS_FINALROM)

    void                    NNSi_FndDumpUnitHeap(
                                NNSFndHeapHandle    heap);

// #if ! defined(NNS_FINALROM)
#endif

NNSFndHeapHandle        NNS_FndCreateUnitHeapEx(
                            void*   startAddress,
                            u32     heapSize,
                            u32     memBlockSize,
                            int     alignment,
                            u16     optFlag);

void                    NNS_FndDestroyUnitHeap(
                            NNSFndHeapHandle    heap);

void*                   NNS_FndAllocFromUnitHeap(
                        	NNSFndHeapHandle    heap);

void                    NNS_FndFreeToUnitHeap(
                            NNSFndHeapHandle    heap,
                            void*               memBlock);

u32                     NNS_FndCountFreeBlockForUnitHeap(
                            NNSFndHeapHandle    heap);


u32                     NNS_FndCalcHeapSizeForUnitHeap(
                            u32     memBlockSize,
                            u32     memBlockNum,
                            int     alignment);


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NNS_FND_UNITHEAP_H_ */
#endif
