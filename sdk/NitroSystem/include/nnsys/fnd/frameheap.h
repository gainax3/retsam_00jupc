/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - fnd
  File:     frameheap.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.10 $
 *---------------------------------------------------------------------------*/

#ifndef NNS_FND_FRAMEHEAP_H_
#define NNS_FND_FRAMEHEAP_H_

#include <nnsys/fnd/heapcommon.h>

#ifdef __cplusplus
extern "C" {
#endif


/* =======================================================================
    定数定義
   ======================================================================== */

#define NNS_FND_FRMHEAP_FREE_HEAD (1 <<0)
#define NNS_FND_FRMHEAP_FREE_TAIL (1 <<1)
#define NNS_FND_FRMHEAP_FREE_ALL  (NNS_FND_FRMHEAP_FREE_HEAD | NNS_FND_FRMHEAP_FREE_TAIL)


/* =======================================================================
    型定義
   ======================================================================== */

typedef struct NNSiFndFrmHeapState NNSiFndFrmHeapState;

// 状態保存用構造体
struct NNSiFndFrmHeapState
{
	u32						tagName;        // タグ名
	void*					headAllocator;  // フレームヒープの先頭位置
	void*					tailAllocator;  // フレームヒープの末尾位置
	NNSiFndFrmHeapState*    pPrevState;     // 1つ前の状態保存へのポインタ
};

typedef struct NNSiFndFrmHeapHead NNSiFndFrmHeapHead;

// フレームヒープのヘッダ情報
struct NNSiFndFrmHeapHead
{
	void*				    headAllocator;  // 先頭メモリ確保ポインタ
	void*				    tailAllocator;  // 末尾メモリ確保ポインタ

	NNSiFndFrmHeapState*	pState;         // 状態保存パラメータ
};


/* =======================================================================
    マクロ関数
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNS_FndCreateFrmHeap

  Description:  フレームヒープを作成します。

  Arguments:    startAddress: ヒープ領域の先頭アドレス。
                size:         ヒープ領域のサイズ。

  Returns:      関数が成功した場合、作成されたフレームヒープのハンドルが返ります。
                関数が失敗すると、NNS_FND_INVALID_HEAP_HANDLE が返ります。
 *---------------------------------------------------------------------------*/
#define             NNS_FndCreateFrmHeap(startAddress, size) \
                        NNS_FndCreateFrmHeapEx(startAddress, size, 0)

/*---------------------------------------------------------------------------*
  Name:         NNS_FndAllocFromFrmHeap

  Description:  フレームヒープからメモリブロックを確保します。
                メモリブロックのアライメントは4バイト固定です。

  Arguments:    heap:   フレームヒープのハンドル。
                size:   確保するメモリブロックのサイズ(バイト単位)。

  Returns:      メモリブロックの確保が成功した場合、確保したメモリブロックへの
                ポインタが返ります。
                失敗した場合、NULLが返ります。
 *---------------------------------------------------------------------------*/
#define             NNS_FndAllocFromFrmHeap(heap, size) \
                        NNS_FndAllocFromFrmHeapEx(heap, size, NNS_FND_HEAP_DEFAULT_ALIGNMENT)

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetAllocatableSizeForFrmHeap

  Description:  フレームヒープ内の割り当て可能な最大サイズを取得します。
                メモリブロックのアライメントは4バイト固定です。

  Arguments:    heap:      フレームヒープのハンドル。

  Returns:      フレームヒープ内の割り当て可能な最大サイズを返します(バイト単位)。
 *---------------------------------------------------------------------------*/
#define             NNS_FndGetAllocatableSizeForFrmHeap(heap) \
                        NNS_FndGetAllocatableSizeForFrmHeapEx(heap, NNS_FND_HEAP_DEFAULT_ALIGNMENT)


/* =======================================================================
    関数プロトタイプ
   ======================================================================== */

void*               NNSi_FndGetFreeStartForFrmHeap(
                        NNSFndHeapHandle    heap);

void*               NNSi_FndGetFreeEndForFrmHeap(
                        NNSFndHeapHandle    heap);

#if ! defined(NNS_FINALROM)

    void                NNSi_FndDumpFrmHeap(
                            NNSFndHeapHandle    heap);

// #if ! defined(NNS_FINALROM)
#endif

NNSFndHeapHandle    NNS_FndCreateFrmHeapEx(
                        void*   startAddress,
                        u32     size,
                        u16     optFlag);

void                NNS_FndDestroyFrmHeap(
                        NNSFndHeapHandle    heap);

void*               NNS_FndAllocFromFrmHeapEx(
                        NNSFndHeapHandle    heap,
                        u32                 size,
                        int                 alignment);

void                NNS_FndFreeToFrmHeap(
                        NNSFndHeapHandle    heap,
                        int                 mode);

u32                 NNS_FndGetAllocatableSizeForFrmHeapEx(
                        NNSFndHeapHandle    heap,
                        int                 alignment);

BOOL                NNS_FndRecordStateForFrmHeap(
                        NNSFndHeapHandle    heap,
                        u32                 tagName);

BOOL                NNS_FndFreeByStateToFrmHeap(
                        NNSFndHeapHandle    heap,
                        u32                 tagName);

u32                 NNS_FndAdjustFrmHeap(
                        NNSFndHeapHandle    heap);

u32                 NNS_FndResizeForMBlockFrmHeap(
                        NNSFndHeapHandle    heap,
                        void*               memBlock,
                        u32                 newSize);


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NNS_FND_FRAMEHEAP_H_ */
#endif
