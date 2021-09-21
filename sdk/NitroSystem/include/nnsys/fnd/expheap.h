/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - fnd
  File:     expheap.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.9 $
 *---------------------------------------------------------------------------*/

#ifndef NNS_FND_EXPHEAP_H_
#define NNS_FND_EXPHEAP_H_

#include <nnsys/fnd/heapcommon.h>

#ifdef __cplusplus
extern "C" {
#endif


/* =======================================================================
    定数定義
   ======================================================================== */

// メモリ確保方向
enum
{
    NNS_FND_EXPHEAP_ALLOC_DIR_FRONT,    // 前方より確保
    NNS_FND_EXPHEAP_ALLOC_DIR_REAR      // 後方より確保
};

// メモリ確保モード
enum
{
    /*
        この属性値がセットされていると、確保しようとしている
        メモリブロックのサイズ以上の大きさを持つ、
        最初に見つかった空き領域からメモリブロックを確保します。
    */
    NNS_FND_EXPHEAP_ALLOC_MODE_FIRST,

    /*
        この属性値がセットされていると、確保しようとしている
        メモリブロックのサイズに一番近いサイズの空き領域を探し、
        その空き領域からメモリブロックを確保します。
    */
    NNS_FND_EXPHEAP_ALLOC_MODE_NEAR
};


/* =======================================================================
    型定義
   ======================================================================== */

typedef struct NNSiFndExpHeapMBlockHead NNSiFndExpHeapMBlockHead;

// メモリブロックのヘッダ情報
struct NNSiFndExpHeapMBlockHead
{
    u16                         signature;      // シグネチャ
    u16                         attribute;      // 属性
                                                // [8:グループID]
                                                // [7:アラインメント]
                                                // [1:テンポラリフラグ]

    u32                         blockSize;      // ブロックサイズ(データ領域のみ)

    NNSiFndExpHeapMBlockHead*   pMBHeadPrev;    // 前ブロック
    NNSiFndExpHeapMBlockHead*   pMBHeadNext;    // 次ブロック
};

typedef struct NNSiFndExpMBlockList NNSiFndExpMBlockList;

// メモリブロックのリスト
struct NNSiFndExpMBlockList
{
    NNSiFndExpHeapMBlockHead*   head;   // 先頭に繋がれているメモリブロックへのポインタ
    NNSiFndExpHeapMBlockHead*   tail;   // 後尾に繋がれているメモリブロックへのポインタ
};

typedef struct NNSiFndExpHeapHead NNSiFndExpHeapHead;

// 拡張ヒープのヘッダ情報
struct NNSiFndExpHeapHead
{
    NNSiFndExpMBlockList        mbFreeList;     // フリーリスト
    NNSiFndExpMBlockList        mbUsedList;     // 使用リスト

    u16                         groupID;        // カレントグループID (下位8bitのみ)
    u16                         feature;        // 属性
};

// メモリブロック毎に呼び出されるコールバック関数の型
typedef void        (*NNSFndHeapVisitor)(
                        void*               memBlock,
                        NNSFndHeapHandle    heap,
                        u32                 userParam);

/* =======================================================================
    マクロ関数
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNS_FndCreateExpHeap

  Description:  拡張ヒープを作成します。

  Arguments:    startAddress: ヒープ領域の先頭アドレス。
                size:         ヒープ領域のサイズ。

  Returns:      関数が成功した場合、作成された拡張ヒープのハンドルが返ります。
                関数が失敗すると、NNS_FND_HEAP_INVALID_HANDLE が返ります。
 *---------------------------------------------------------------------------*/
#define             NNS_FndCreateExpHeap(startAddress, size) \
                        NNS_FndCreateExpHeapEx(startAddress, size, 0)

/*---------------------------------------------------------------------------*
  Name:         NNS_FndAllocFromExpHeap

  Description:  拡張ヒープからメモリブロックを確保します。
                メモリブロックのアライメントは4バイト固定です。

  Arguments:    heap:   拡張ヒープのハンドル。
                size:   確保するメモリブロックのサイズ(バイト単位)。

  Returns:      メモリブロックの確保が成功した場合、確保したメモリブロックへの
                ポインタが返ります。
                失敗した場合、NULLが返ります。
 *---------------------------------------------------------------------------*/
#define             NNS_FndAllocFromExpHeap(heap, size) \
                        NNS_FndAllocFromExpHeapEx(heap, size, NNS_FND_HEAP_DEFAULT_ALIGNMENT)

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetAllocatableSizeForExpHeap

  Description:  拡張ヒープ内の割り当て可能な最大サイズを取得します。
                メモリブロックのアライメントは4バイト固定です。

  Arguments:    heap:     拡張ヒープのハンドル。

  Returns:      拡張ヒープ内の割り当て可能な最大サイズを返します(バイト単位)。
 *---------------------------------------------------------------------------*/
#define             NNS_FndGetAllocatableSizeForExpHeap(heap) \
                        NNS_FndGetAllocatableSizeForExpHeapEx(heap, NNS_FND_HEAP_DEFAULT_ALIGNMENT)


/* =======================================================================
    関数プロトタイプ
   ======================================================================== */

#if ! defined(NNS_FINALROM)

    void                NNSi_FndDumpExpHeap(
                            NNSFndHeapHandle    heap);

// #if ! defined(NNS_FINALROM)
#endif

NNSFndHeapHandle    NNS_FndCreateExpHeapEx(
                        void*   startAddress,
                        u32     size,
                        u16     optFlag);

void                NNS_FndDestroyExpHeap(
                        NNSFndHeapHandle    heap);

void*               NNS_FndAllocFromExpHeapEx(
                        NNSFndHeapHandle    heap,
                        u32                 size,
                        int                 alignment);

u32                 NNS_FndResizeForMBlockExpHeap(
                        NNSFndHeapHandle    heap,
                        void*               memBlock,
                        u32                 size);

void                NNS_FndFreeToExpHeap(
                        NNSFndHeapHandle    heap,
                        void*               memBlock);

u32                 NNS_FndGetTotalFreeSizeForExpHeap(
                        NNSFndHeapHandle    heap);

u32                 NNS_FndGetAllocatableSizeForExpHeapEx(
                        NNSFndHeapHandle    heap,
                        int                 alignment);

u16                 NNS_FndSetAllocModeForExpHeap(
                        NNSFndHeapHandle    heap,
                        u16                 mode);

u16                 NNS_FndGetAllocModeForExpHeap(
                        NNSFndHeapHandle    heap);

u16                 NNS_FndSetGroupIDForExpHeap(
                        NNSFndHeapHandle    heap,
                        u16                 groupID);

u16                 NNS_FndGetGroupIDForExpHeap(
                        NNSFndHeapHandle    heap);

void                NNS_FndVisitAllocatedForExpHeap(
                        NNSFndHeapHandle    heap,
                        NNSFndHeapVisitor   visitor,
                        u32                 userParam);

u32                 NNS_FndGetSizeForMBlockExpHeap(
                        const void*         memBlock);

u16                 NNS_FndGetGroupIDForMBlockExpHeap(
                        const void*         memBlock);

u16                 NNS_FndGetAllocDirForMBlockExpHeap(
                        const void*         memBlock);

#if defined(NNS_FINALROM)

    #define             NNS_FndCheckExpHeap(heap, optFlag)                      (TRUE)

    #define             NNS_FndCheckForMBlockExpHeap(memBlock, heap, optFlag)   (TRUE)

// #if defined(NNS_FINALROM)
#else

    BOOL                NNS_FndCheckExpHeap(
                            NNSFndHeapHandle    heap,
                            u32                 optFlag);

    BOOL                NNS_FndCheckForMBlockExpHeap(
                            const void*         memBlock,
                            NNSFndHeapHandle    heap,
                            u32                 optFlag);

// #if defined(NNS_FINALROM)
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NNS_FND_EXPHEAP_H_ */
#endif
