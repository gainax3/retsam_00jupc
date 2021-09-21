/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - fnd
  File:     expheap.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.28 $
 *---------------------------------------------------------------------------*/
#include <stdlib.h>
#include <nitro.h>
#include <nnsys/misc.h>
#include <nnsys/fnd/expheap.h>
#include <nnsys/fnd/config.h>
#include "heapcommoni.h"


/* ========================================================================
    マクロ定数
   ======================================================================== */

// フリーメモリブロックのシグネチャ
#define MBLOCK_FREE_SIGNATURE   ('FR')

// 使用メモリブロックのシグネチャ
#define MBLOCK_USED_SIGNATURE   ('UD')

// グループIDの最大値
#define MAX_GROUPID             0xff

// グループIDのデフォルト値
#define DEFAULT_GROUPID         0

// アライメントの最小値
#define MIN_ALIGNMENT           4

// デフォルトのメモリ確保モード
#define DEFAULT_ALLOC_MODE      NNS_FND_EXPHEAP_ALLOC_MODE_FIRST

// フリーブロックとして登録する最小サイズ (ヘッダは含まないサイズ)
#define MIN_FREE_BLOCK_SIZE     4
// #define MIN_FREE_BLOCK_SIZE 16


/* ========================================================================
    構造体定義
   ======================================================================== */
typedef struct NNSiMemRegion NNSiMemRegion;

struct NNSiMemRegion
{
    void*       start;
    void*       end;
};


/* ========================================================================
    マクロ関数
   ======================================================================== */

#if ! defined(NNS_FINALROM)

    // ヒープの検査時の警告出力用
    #define HEAP_WARNING(exp, ...) \
        (void) ((exp) && (OS_Printf(__VA_ARGS__), 0))

// #if ! defined(NNS_FINALROM)
#endif


/* ========================================================================
    static関数
   ======================================================================== */

/* ------------------------------------------------------------------------
    ポインタ操作
   ------------------------------------------------------------------------ */

static NNS_FND_INLINE void*
MaxPtr(void* a, void* b)
{
    return NNSiGetUIntPtr(a) > NNSiGetUIntPtr(b) ? a: b;
}

static NNS_FND_INLINE BOOL
IsValidExpHeapHandle(NNSFndHeapHandle handle)
{
    if (handle == NNS_FND_HEAP_INVALID_HANDLE)
    {
        return FALSE;
    }

    {
        NNSiFndHeapHead* pHeapHd = handle;
        return pHeapHd->signature == NNSI_EXPHEAP_SIGNATURE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         GetExpHeapHeadPtrFromHeapHead

  Description:  ヒープヘッダへのポインタから、拡張ヒープヘッダへのポインタを取得します。

  Arguments:    pHHead:  ヒープヘッダへのポインタ。

  Returns:      拡張ヒープヘッダへのポインタを返します。
 *---------------------------------------------------------------------------*/
static NNS_FND_INLINE NNSiFndExpHeapHead*
GetExpHeapHeadPtrFromHeapHead(NNSiFndHeapHead* pHHead)
{
    return AddU32ToPtr(pHHead, sizeof(NNSiFndHeapHead));
}

/*---------------------------------------------------------------------------*
  Name:         GetHeapHeadPtrFromExpHeapHead

  Description:  拡張ヒープヘッダへのポインタから、ヒープヘッダへのポインタを取得します。

  Arguments:    pEHHead:  拡張ヒープヘッダへのポインタ。

  Returns:      ヒープヘッダへのポインタを返します。
 *---------------------------------------------------------------------------*/
static NNS_FND_INLINE NNSiFndHeapHead*
GetHeapHeadPtrFromExpHeapHead(NNSiFndExpHeapHead* pEHHead)
{
    return SubU32ToPtr(pEHHead, sizeof(NNSiFndHeapHead));
}

/*---------------------------------------------------------------------------*
  Name:         GetExpHeapHeadPtrFromHandle

  Description:  拡張ヒープハンドルから、拡張ヒープヘッダへのポインタを取得します。

  Arguments:    heap:  拡張ヒープハンドル。

  Returns:      拡張ヒープヘッダへのポインタを返します。
 *---------------------------------------------------------------------------*/
static NNS_FND_INLINE NNSiFndExpHeapHead*
GetExpHeapHeadPtrFromHandle(NNSFndHeapHandle heap)
{
    return GetExpHeapHeadPtrFromHeapHead(heap);
}

/*---------------------------------------------------------------------------*
  Name:         GetMemPtrForMBlock

  Description:  NNSiFndExpHeapMBlockHead構造体へのポインタから、
                メモリブロックへのポインタを取得します。

  Arguments:    pMBlkHd:  NNSiFndExpHeapMBlockHead構造体へのポインタ。

  Returns:      メモリブロックへのポインタを返します。
 *---------------------------------------------------------------------------*/
static NNS_FND_INLINE void*
GetMemPtrForMBlock(NNSiFndExpHeapMBlockHead* pMBlkHd)
{
    return AddU32ToPtr(pMBlkHd, sizeof(NNSiFndExpHeapMBlockHead));
}

static NNS_FND_INLINE const void*
GetMemCPtrForMBlock(const NNSiFndExpHeapMBlockHead* pMBlkHd)
{
    return AddU32ToCPtr(pMBlkHd, sizeof(NNSiFndExpHeapMBlockHead));
}

/*---------------------------------------------------------------------------*
  Name:         GetMBlockHeadPtr

  Description:  メモリブロックへのポインタから、
                NNSiFndExpHeapMBlockHead構造体へのポインタを取得します。
                メモリブロックへのポインタを取得します。

  Arguments:    memBlock:  メモリブロックへのポインタ。

  Returns:      NNSiFndExpHeapMBlockHead構造体へのポインタを返します。
 *---------------------------------------------------------------------------*/
static NNS_FND_INLINE NNSiFndExpHeapMBlockHead*
GetMBlockHeadPtr(void* memBlock)
{
    return SubU32ToPtr(memBlock, sizeof(NNSiFndExpHeapMBlockHead));
}

static NNS_FND_INLINE const NNSiFndExpHeapMBlockHead*
GetMBlockHeadCPtr(const void* memBlock)
{
    return SubU32ToCPtr(memBlock, sizeof(NNSiFndExpHeapMBlockHead));
}

static NNS_FND_INLINE void*
GetMBlockEndAddr(NNSiFndExpHeapMBlockHead* pMBHead)
{
    return AddU32ToPtr(GetMemPtrForMBlock(pMBHead), pMBHead->blockSize);
}

/* ------------------------------------------------------------------------
    NNSiFndExpHeapMBlockHead 構造体アクセス関数
   ------------------------------------------------------------------------ */

/*---------------------------------------------------------------------------*
  Name:         GetAlignmentForMBlock

  Description:  NNSiFndExpHeapMBlockHead構造体のアライメント値を取得します。

  Arguments:    pMBlkHd:  NNSiFndExpHeapMBlockHead構造体へのポインタ。

  Returns:      NNSiFndExpHeapMBlockHead構造体のアライメント値を返します。
 *---------------------------------------------------------------------------*/
static NNS_FND_INLINE u16
GetAlignmentForMBlock(const NNSiFndExpHeapMBlockHead* pMBlkHd)
{
    return (u16)NNSi_FndGetBitValue(pMBlkHd->attribute, 8, 7);
}

/*---------------------------------------------------------------------------*
  Name:         SetAlignmentForMBlock

  Description:  NNSiFndExpHeapMBlockHead構造体のアライメント値をセットします。

  Arguments:    pMBlkHd:    NNSiFndExpHeapMBlockHead構造体へのポインタ。
                alignment:  セットするアライメント値

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static NNS_FND_INLINE void
SetAlignmentForMBlock(
    NNSiFndExpHeapMBlockHead*   pMBlkHd,
    u16                         alignment
)
{
    NNSi_FndSetBitValue(pMBlkHd->attribute, 8, 7, alignment);
}

static NNS_FND_INLINE u16
GetGroupIDForMBlock(const NNSiFndExpHeapMBlockHead* pMBHead)
{
    return (u16)NNSi_FndGetBitValue(pMBHead->attribute, 0, 8);
}

static NNS_FND_INLINE void
SetGroupIDForMBlock(
    NNSiFndExpHeapMBlockHead*   pMBHead,
    u16                         id
)
{
    NNSi_FndSetBitValue(pMBHead->attribute, 0, 8, id);
}

static NNS_FND_INLINE u16
GetAllocDirForMBlock(const NNSiFndExpHeapMBlockHead* pMBHead)
{
    return (u16)NNSi_FndGetBitValue(pMBHead->attribute, 15, 1);
}

static NNS_FND_INLINE void
SetAllocDirForMBlock(
    NNSiFndExpHeapMBlockHead*   pMBHead,
    u16                         mode
)
{
    NNSi_FndSetBitValue(pMBHead->attribute, 15, 1, mode);
}


/* ------------------------------------------------------------------------
    NNSiFndExpHeapHead 構造体アクセス関数
   ------------------------------------------------------------------------ */

/*---------------------------------------------------------------------------*
  Name:         GetAllocMode

  Description:  拡張ヒープのメモリ確保モードを取得します。

  Arguments:    pEHHead:  拡張ヒープヘッダへのポインタ。

  Returns:      拡張ヒープのメモリ確保モードを返します。
 *---------------------------------------------------------------------------*/
static NNS_FND_INLINE u16
GetAllocMode(NNSiFndExpHeapHead* pEHHead)
{
    return (u16)NNSi_FndGetBitValue(pEHHead->feature, 0, 1);
}

/*---------------------------------------------------------------------------*
  Name:         SetAllocMode

  Description:  拡張ヒープのメモリ確保モードをセットします。

  Arguments:    pEHHead:  拡張ヒープヘッダへのポインタ。
                mode:     メモリ確保モード。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static NNS_FND_INLINE void
SetAllocMode(
    NNSiFndExpHeapHead* pEHHead,
    u16                 mode
)
{
    NNSi_FndSetBitValue(pEHHead->feature, 0, 1, mode);
}

static void
GetRegionOfMBlock(
    NNSiMemRegion*              region,
    NNSiFndExpHeapMBlockHead*   block
)
{
    region->start = SubU32ToPtr(block, GetAlignmentForMBlock(block));
    region->end   = GetMBlockEndAddr(block);
}


/* ------------------------------------------------------------------------
    メモリブロックリスト操作
   ------------------------------------------------------------------------ */

static NNSiFndExpHeapMBlockHead*
RemoveMBlock(
    NNSiFndExpMBlockList*       list,
    NNSiFndExpHeapMBlockHead*   block
)
{
    NNSiFndExpHeapMBlockHead *const prev = block->pMBHeadPrev;
    NNSiFndExpHeapMBlockHead *const next = block->pMBHeadNext;

    // 前参照リンク
    if (prev)
    {
        prev->pMBHeadNext = next;
    }
    else
    {
        list->head = next;
    }

    // 次参照リンク
    if (next)
    {
        next->pMBHeadPrev = prev;
    }
    else
    {
        list->tail = prev;
    }

    return prev;
}

static NNSiFndExpHeapMBlockHead*
InsertMBlock(
    NNSiFndExpMBlockList*       list,
    NNSiFndExpHeapMBlockHead*   target,
    NNSiFndExpHeapMBlockHead*   prev
)
{
    NNSiFndExpHeapMBlockHead* next;

    // 前参照リンク
    target->pMBHeadPrev = prev;
    if (prev)
    {
        next = prev->pMBHeadNext;
        prev->pMBHeadNext = target;
    }
    else
    {
        next = list->head;
        list->head = target;
    }

    // 次参照リンク
    target->pMBHeadNext = next;
    if (next)
    {
        next->pMBHeadPrev = target;
    }
    else
    {
        list->tail = target;
    }

    return target;
}

/*---------------------------------------------------------------------------*
  Name:         AppendMBlock

  Description:  メモリブロックをリストの最後に追加します。

  Arguments:    link:   追加するリスト
                block:  追加するメモリブロック

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static NNS_FND_INLINE void
AppendMBlock(
    NNSiFndExpMBlockList*       list,
    NNSiFndExpHeapMBlockHead*   block
)
{
    (void)InsertMBlock(list, block, list->tail);
}

/*---------------------------------------------------------------------------*
  Name:         InitMBlock

  Description:  メモリブロックの初期化を行います。

  Arguments:    pRegion:    メモリブロックとするリージョンを表す構造体へのポインタ。
                signature:  メモリブロックのシグネチャ。

  Returns:      初期化したメモリブロックへのポインタを返します。
 *---------------------------------------------------------------------------*/
static NNSiFndExpHeapMBlockHead*
InitMBlock(
    const NNSiMemRegion*    pRegion,
    u16                     signature
)
{
    NNSiFndExpHeapMBlockHead* block = pRegion->start;

    block->signature = signature;
    block->attribute = 0;
    block->blockSize = GetOffsetFromPtr(GetMemPtrForMBlock(block), pRegion->end);
    block->pMBHeadPrev = NULL;
    block->pMBHeadNext = NULL;

    return block;
}

/*---------------------------------------------------------------------------*
  Name:         InitFreeMBlock

  Description:  メモリブロックをフリーブロック用に初期化を行います。

  Arguments:    pRegion:    メモリブロックとするリージョンを表す構造体へのポインタ。

  Returns:      初期化したメモリブロックへのポインタを返します。
 *---------------------------------------------------------------------------*/
static NNS_FND_INLINE NNSiFndExpHeapMBlockHead*
InitFreeMBlock(
    const NNSiMemRegion*    pRegion
)
{
    return InitMBlock(pRegion, MBLOCK_FREE_SIGNATURE);
}

/*---------------------------------------------------------------------------*
  Name:         InitExpHeap

  Description:  拡張ヒープの初期化を行います。

  Arguments:    startAddress:  拡張ヒープとするメモリの開始アドレス。
                endAddress:    拡張ヒープとするメモリの終了アドレス +1。
                optFlag:       オプションフラグ。

  Returns:      ヒープヘッダへのポインタを返します。
 *---------------------------------------------------------------------------*/
static NNSiFndHeapHead*
InitExpHeap(
    void*   startAddress,
    void*   endAddress,
    u16     optFlag
)
{
    NNSiFndHeapHead* pHeapHd = startAddress;
    NNSiFndExpHeapHead* pExpHeapHd = GetExpHeapHeadPtrFromHeapHead(pHeapHd);

    NNSi_FndInitHeapHead(           // ヒープ共通初期化
        pHeapHd,
        NNSI_EXPHEAP_SIGNATURE,
        AddU32ToPtr(pExpHeapHd, sizeof(NNSiFndExpHeapHead)),    // heapStart
        endAddress,                                             // heapEnd
        optFlag);

    pExpHeapHd->groupID = DEFAULT_GROUPID;      // グループID
    pExpHeapHd->feature = 0;
    SetAllocMode(pExpHeapHd, DEFAULT_ALLOC_MODE);

    // フリーのブロックを作る
    {
        NNSiFndExpHeapMBlockHead* pMBHead;
        NNSiMemRegion region;
        region.start = pHeapHd->heapStart;
        region.end   = pHeapHd->heapEnd;
        pMBHead = InitFreeMBlock(&region);

        // ブロックリスト
        pExpHeapHd->mbFreeList.head = pMBHead;
        pExpHeapHd->mbFreeList.tail = pMBHead;
        pExpHeapHd->mbUsedList.head = NULL;
        pExpHeapHd->mbUsedList.tail = NULL;

        return pHeapHd;
    }
}

/*---------------------------------------------------------------------------*
  Name:         AllocUsedBlockFromFreeBlock

  Description:  フリーブロックの中から新しいメモリブロックを確保します。

  Arguments:    pEHHead:      拡張ヒープヘッダへのポインタ。
                pMBHeadFree:  フリーブロックヘッダへのポインタ。
                mblock:       確保するメモリブロックのアドレス。
                size:         確保するメモリブロックのサイズ。
                direction:    確保方向。

  Returns:      確保したメモリブロックの先頭のポインタを返します。
 *---------------------------------------------------------------------------*/
static void*
AllocUsedBlockFromFreeBlock(
    NNSiFndExpHeapHead*         pEHHead,
    NNSiFndExpHeapMBlockHead*   pMBHeadFree,
    void*                       mblock,
    u32                         size,
    u16                         direction
)
{
    NNSiMemRegion freeRgnT;
    NNSiMemRegion freeRgnB;
    NNSiFndExpHeapMBlockHead* pMBHeadFreePrev;

    GetRegionOfMBlock(&freeRgnT, pMBHeadFree);
    freeRgnB.end   = freeRgnT.end;
    freeRgnB.start = AddU32ToPtr(mblock, size);
    freeRgnT.end   = SubU32ToPtr(mblock, sizeof(NNSiFndExpHeapMBlockHead));

    pMBHeadFreePrev = RemoveMBlock(&pEHHead->mbFreeList, pMBHeadFree);  // 一旦フリーブロックを削除

    // フリーブロック作る余裕が無い場合
    if (GetOffsetFromPtr(freeRgnT.start, freeRgnT.end) < sizeof(NNSiFndExpHeapMBlockHead) + MIN_FREE_BLOCK_SIZE)
    {
        freeRgnT.end = freeRgnT.start;  // 使用ブロックのアライメント値に含める
    }
    else
    {
        pMBHeadFreePrev = InsertMBlock(&pEHHead->mbFreeList, InitFreeMBlock(&freeRgnT), pMBHeadFreePrev);
    }

    // フリーブロック作る余裕が無い場合
    if (GetOffsetFromPtr(freeRgnB.start, freeRgnB.end) < sizeof(NNSiFndExpHeapMBlockHead) + MIN_FREE_BLOCK_SIZE)
    {
        freeRgnB.start= freeRgnB.end;   // 使用ブロックに含める
    }
    else
    {
        (void)InsertMBlock(&pEHHead->mbFreeList, InitFreeMBlock(&freeRgnB), pMBHeadFreePrev);
    }

    // デバグ用メモリ充填
    FillAllocMemory(GetHeapHeadPtrFromExpHeapHead(pEHHead), freeRgnT.end, GetOffsetFromPtr(freeRgnT.end, freeRgnB.start));

    // 新規ブロック作成
    {
        NNSiFndExpHeapMBlockHead* pMBHeadNewUsed;
        NNSiMemRegion region;

        region.start = SubU32ToPtr(mblock, sizeof(NNSiFndExpHeapMBlockHead));
        region.end   = freeRgnB.start;

        pMBHeadNewUsed = InitMBlock(&region, MBLOCK_USED_SIGNATURE);
        SetAllocDirForMBlock(pMBHeadNewUsed, direction);
        SetAlignmentForMBlock(pMBHeadNewUsed, (u16)GetOffsetFromPtr(freeRgnT.end, pMBHeadNewUsed));
        SetGroupIDForMBlock(pMBHeadNewUsed, pEHHead->groupID);
        AppendMBlock(&pEHHead->mbUsedList, pMBHeadNewUsed);
    }

    return mblock;
}

/*---------------------------------------------------------------------------*
  Name:         AllocFromHead

  Description:  ヒープの先頭からメモリブロックを確保します。

  Arguments:    pHeapHd:    ヒープヘッダへのポインタ。
                size:       確保するメモリブロックのサイズ。
                alignment:  アライメント値。

  Returns:      メモリブロックの確保が成功した場合、確保したメモリブロックへの
                ポインタが返ります。
                失敗した場合、NULLが返ります。
 *---------------------------------------------------------------------------*/
static void*
AllocFromHead(
    NNSiFndHeapHead*    pHeapHd,
    u32                 size,
    int                 alignment
)
{
    NNSiFndExpHeapHead* pExpHeapHd = GetExpHeapHeadPtrFromHeapHead(pHeapHd);

    // 最初にみつかったものを割り当てるか
    const BOOL bAllocFirst = GetAllocMode(pExpHeapHd) == NNS_FND_EXPHEAP_ALLOC_MODE_FIRST;

    NNSiFndExpHeapMBlockHead* pMBlkHd      = NULL;
    NNSiFndExpHeapMBlockHead* pMBlkHdFound = NULL;
    u32 foundSize = 0xffffffff;
    void* foundMBlock = NULL;

    // フリーブロック検索
    for (pMBlkHd = pExpHeapHd->mbFreeList.head; pMBlkHd; pMBlkHd = pMBlkHd->pMBHeadNext)
    {
        void *const mblock    = GetMemPtrForMBlock(pMBlkHd);
        void *const reqMBlock = NNSi_FndRoundUpPtr(mblock, alignment);
        const u32 offset      = GetOffsetFromPtr(mblock, reqMBlock);    // 発生したずれ

        if ( pMBlkHd->blockSize >= size + offset
         &&  foundSize > pMBlkHd->blockSize )
        {
            pMBlkHdFound  = pMBlkHd;
            foundSize     = pMBlkHd->blockSize;
            foundMBlock   = reqMBlock;

            if (bAllocFirst || foundSize == size)
            {
                break;
            }
        }
    }

    if (! pMBlkHdFound) // 条件に合うブロックが見つからない
    {
        return NULL;
    }

    return AllocUsedBlockFromFreeBlock( // 発見したフリーブロックから領域確保
            pExpHeapHd,
            pMBlkHdFound,
            foundMBlock,
            size,
            NNS_FND_EXPHEAP_ALLOC_DIR_FRONT);
}

/*---------------------------------------------------------------------------*
  Name:         AllocFromTail

  Description:  ヒープの末尾からメモリブロックを確保します。

  Arguments:    pHeapHd:    ヒープヘッダへのポインタ。
                size:       確保するメモリブロックのサイズ。
                alignment:  アライメント値。

  Returns:      メモリブロックの確保が成功した場合、確保したメモリブロックへの
                ポインタが返ります。
                失敗した場合、NULLが返ります。
 *---------------------------------------------------------------------------*/
static void*
AllocFromTail(
    NNSiFndHeapHead*    pHeapHd,
    u32                 size,
    int                 alignment
)
{
    NNSiFndExpHeapHead* pExpHeapHd = GetExpHeapHeadPtrFromHeapHead(pHeapHd);

    // 最初にみつかったものを割り当てるか
    const BOOL bAllocFirst = GetAllocMode(pExpHeapHd) == NNS_FND_EXPHEAP_ALLOC_MODE_FIRST;

    NNSiFndExpHeapMBlockHead* pMBlkHd      = NULL;
    NNSiFndExpHeapMBlockHead* pMBlkHdFound = NULL;
    u32 foundSize = 0xffffffff;
    void* foundMBlock = NULL;

    // フリーブロック検索
    for (pMBlkHd = pExpHeapHd->mbFreeList.tail; pMBlkHd; pMBlkHd = pMBlkHd->pMBHeadPrev)
    {
        void *const mblock    = GetMemPtrForMBlock(pMBlkHd);
        void *const mblockEnd = AddU32ToPtr(mblock, pMBlkHd->blockSize);
        void *const reqMBlock = NNSi_FndRoundDownPtr(SubU32ToPtr(mblockEnd, size), alignment);  // アライン済みのアドレス

        if ( ComparePtr(reqMBlock, mblock) >= 0
         &&  foundSize > pMBlkHd->blockSize )
        {
            pMBlkHdFound = pMBlkHd;
            foundSize    = pMBlkHd->blockSize;
            foundMBlock  = reqMBlock;

            if (bAllocFirst || foundSize == size)
            {
                break;
            }
        }
    }

    if (! pMBlkHdFound) // 条件に合うブロックが見つからない
    {
        return NULL;
    }

    return AllocUsedBlockFromFreeBlock( // 発見したフリーブロックから領域確保
            pExpHeapHd,
            pMBlkHdFound,
            foundMBlock,
            size,
            NNS_FND_EXPHEAP_ALLOC_DIR_REAR);
}

/*---------------------------------------------------------------------------*
  Name:         RecycleRegion

  Description:  空きリージョンをフリーメモリブロックへ組み入れます。
                フリーブロックと隣接している場合は、フリーブロックを拡張します。
                フリーブロックと隣接しておらず、かつフリーブロックとするほどの
                サイズが無い場合は、後方に隣接する使用済みブロックのアライメント値とします。
                後方に隣接する使用済みブロックが無い場合は、関数は失敗します。

  Arguments:    pEHHead:  拡張ヒープヘッダへのポインタ。
                pRegion:  空きリージョンへのポインタ。

  Returns:      関数が成功すれば TRUE を返します。
                失敗すれば FALSE を返します。
 *---------------------------------------------------------------------------*/
static BOOL
RecycleRegion(
    NNSiFndExpHeapHead*     pEHHead,
    const NNSiMemRegion*    pRegion
)
{
    NNSiFndExpHeapMBlockHead* pBlkPrFree  = NULL;   // 直前フリーブロック
    NNSiMemRegion freeRgn = *pRegion;

    // 指定エリアに隣接したフリーエリアを検索
    {
        NNSiFndExpHeapMBlockHead* pBlk;

        for (pBlk = pEHHead->mbFreeList.head; pBlk; pBlk = pBlk->pMBHeadNext)
        {
            if (pBlk < pRegion->start)
            {
                pBlkPrFree = pBlk;
                continue;
            }

            if (pBlk == pRegion->end)   // 後方に隣接するブロックか?
            {
                // 空きリージョンを結合
                freeRgn.end = GetMBlockEndAddr(pBlk);
                (void)RemoveMBlock(&pEHHead->mbFreeList, pBlk);

                // ヘッダ部をNoUseで埋める
                FillNoUseMemory(GetHeapHeadPtrFromExpHeapHead(pEHHead), pBlk, sizeof(NNSiFndExpHeapMBlockHead));
            }
            break;
        }
    }

    // 直前のフリーブロックが前方に隣接するブロックか?
    if (pBlkPrFree && GetMBlockEndAddr(pBlkPrFree) == pRegion->start)
    {
        // 空きリージョンを結合
        freeRgn.start = pBlkPrFree;
        pBlkPrFree = RemoveMBlock(&pEHHead->mbFreeList, pBlkPrFree);
    }

    if (GetOffsetFromPtr(freeRgn.start, freeRgn.end) < sizeof(NNSiFndExpHeapMBlockHead)) // ブロックになれない大きさ
    {
        return FALSE;   // NNS_FndResizeForMBlockExpHeap()で小さいサイズを縮小しようとしていて、
                        // かつ後ろにフリーブロックが無い場合にここに到達
    }

    // デバグ用メモリ充填
    FillFreeMemory(GetHeapHeadPtrFromExpHeapHead(pEHHead), pRegion->start, GetOffsetFromPtr(pRegion->start, pRegion->end));

    (void)InsertMBlock(&pEHHead->mbFreeList,
        InitFreeMBlock(&freeRgn),   // フリーブロック作成
        pBlkPrFree);

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         CheckMBlock

  Description:  メモリブロックのヘッダの内容が妥当であるかチェックします。

  Arguments:    pMBHead:    チェックするメモリブロックのヘッダへのポインタ。
                pHeapHd:    拡張ヒープのヘッダへのポインタ。
                signature:  メモリブロックのシグネチャ。
                heapType:   メモリブロックのタイプ(使用 or フリー)
                flag:       フラグ。

  Returns:      メモリブロックのヘッダの内容が妥当なら TRUE、
                そうでないなら FALSE を返します。
 *---------------------------------------------------------------------------*/
#if ! defined(NNS_FINALROM)

    static BOOL
    CheckMBlock(
        const NNSiFndExpHeapMBlockHead* pMBHead,
        NNSiFndHeapHead*                pHeapHd,
        u16                             signature,
        const char*                     heapType,
        u32                             flag
    )
    {
        const BOOL bPrint = 0 != (flag & NNS_FND_HEAP_ERROR_PRINT);
        const void *const memBlock = GetMemCPtrForMBlock(pMBHead);

        if (pHeapHd)
        {
            if ( NNSiGetUIntPtr(pMBHead) < NNSiGetUIntPtr(pHeapHd->heapStart)
              || NNSiGetUIntPtr(memBlock) > NNSiGetUIntPtr(pHeapHd->heapEnd)
            )
            {
                HEAP_WARNING(bPrint, "[NNS Foundation " "Exp" " Heap]" " Bad %s memory block address. - address %08X, heap area [%08X - %08X)\n",
                    heapType, memBlock, pHeapHd->heapStart, pHeapHd->heapEnd);
                return FALSE;
            }
        }
        else
        {
            if (NNSiGetUIntPtr(pMBHead) >= 0x11000000)
            {
                HEAP_WARNING(bPrint, "[NNS Foundation " "Exp" " Heap]" " Bad %s memory block address. - address %08X\n",
                    heapType, memBlock);
                return FALSE;
            }
        }

        if (pMBHead->signature != signature)    // シグネチャが異なる?
        {
            HEAP_WARNING(bPrint, "[NNS Foundation " "Exp" " Heap]" " Bad %s memory block signature. - address %08X, signature %04X\n",
                heapType, memBlock, pMBHead->signature);
            return FALSE;
        }

        if (pMBHead->blockSize >= 0x01000000)   // サイズが大きすぎる?
        {
            HEAP_WARNING(bPrint, "[NNS Foundation " "Exp" " Heap]" " Too large %s memory block. - address %08X, block size %08X\n",
                heapType, memBlock, pMBHead->blockSize);
            return FALSE;
        }

        if (pHeapHd)
        {
            if (NNSiGetUIntPtr(memBlock) + pMBHead->blockSize > NNSiGetUIntPtr(pHeapHd->heapEnd))
            {
                HEAP_WARNING(bPrint, "[NNS Foundation " "Exp" " Heap]" " wrong size %s memory block. - address %08X, block size %08X\n",
                    heapType, memBlock, pMBHead->blockSize);
                return FALSE;
            }
        }

        return TRUE;
    }

// #if ! defined(NNS_FINALROM)
#endif

/*---------------------------------------------------------------------------*
  Name:         CheckUsedMBlock

  Description:  使用メモリブロックのヘッダの内容が妥当であるかチェックします。

  Arguments:    pMBHead:  チェックするメモリブロックのヘッダへのポインタ。
                pHeapHd:  拡張ヒープのヘッダへのポインタ。
                flag:     フラグ。

  Returns:      メモリブロックのヘッダの内容が妥当なら TRUE、
                そうでないなら FALSE を返します。
 *---------------------------------------------------------------------------*/
#if ! defined(NNS_FINALROM)

    static NNS_FND_INLINE BOOL
    CheckUsedMBlock(
        const NNSiFndExpHeapMBlockHead* pMBHead,
        NNSiFndHeapHead*                pHeapHd,
        u32                             flag
    )
    {
        return CheckMBlock(pMBHead, pHeapHd, MBLOCK_USED_SIGNATURE, "used", flag);
    }

// #if ! defined(NNS_FINALROM)
#endif

/*---------------------------------------------------------------------------*
  Name:         CheckFreeMBlock

  Description:  フリーメモリブロックのヘッダの内容が妥当であるかチェックします。

  Arguments:    pMBHead:  チェックするメモリブロックのヘッダへのポインタ。
                pHeapHd:  拡張ヒープのヘッダへのポインタ。
                flag:     フラグ。

  Returns:      メモリブロックのヘッダの内容が妥当なら TRUE、
                そうでないなら FALSE を返します。
 *---------------------------------------------------------------------------*/
#if ! defined(NNS_FINALROM)

    static NNS_FND_INLINE BOOL
    CheckFreeMBlock(
        const NNSiFndExpHeapMBlockHead* pMBHead,
        NNSiFndHeapHead*                pHeapHd,
        u32                             flag
    )
    {
        return CheckMBlock(pMBHead, pHeapHd, MBLOCK_FREE_SIGNATURE, "free", flag);
    }

// #if ! defined(NNS_FINALROM)
#endif

/*---------------------------------------------------------------------------*
  Name:         CheckMBlockPrevPtr

  Description:  メモリブロックの前へのリンクが正しいかチェックします。

  Arguments:    pMBHead:      チェックするメモリブロックのヘッダへのポインタ。
                pMBHeadPrev:  チェックするメモリブロックの前のメモリブロックのヘッダへのポインタ。
                flag:         フラグ。

  Returns:      メモリブロックの前へのリンクが正しいなら TRUE、
                そうでないなら FALSE を返します。
 *---------------------------------------------------------------------------*/
#if ! defined(NNS_FINALROM)

    static BOOL
    CheckMBlockPrevPtr(
        const NNSiFndExpHeapMBlockHead* pMBHead,
        const NNSiFndExpHeapMBlockHead* pMBHeadPrev,
        u32                             flag
    )
    {
        const BOOL bPrint = 0 != (flag & NNS_FND_HEAP_ERROR_PRINT);

        if (pMBHead->pMBHeadPrev != pMBHeadPrev)
        {
            HEAP_WARNING(bPrint, "[NNS Foundation " "Exp" " Heap]" " Wrong link memory block. - address %08X, previous address %08X != %08X\n",
                GetMemCPtrForMBlock(pMBHead), pMBHead->pMBHeadPrev, pMBHeadPrev);
            return FALSE;
        }

        return TRUE;
    }

// #if ! defined(NNS_FINALROM)
#endif

/*---------------------------------------------------------------------------*
  Name:         CheckMBlockNextPtr

  Description:  メモリブロックの次へのリンクが正しいかチェックします。

  Arguments:    pMBHead:      チェックするメモリブロックのヘッダへのポインタ。
                pMBHeadNext:  チェックするメモリブロックの次のメモリブロックのヘッダへのポインタ。
                flag:         フラグ。

  Returns:      メモリブロックの次へのリンクが正しいなら TRUE、
                そうでないなら FALSE を返します。
 *---------------------------------------------------------------------------*/
#if ! defined(NNS_FINALROM)

    static BOOL
    CheckMBlockNextPtr(
        const NNSiFndExpHeapMBlockHead* pMBHead,
        const NNSiFndExpHeapMBlockHead* pMBHeadNext,
        u32                             flag
    )
    {
        const BOOL bPrint = 0 != (flag & NNS_FND_HEAP_ERROR_PRINT);

        if (pMBHead->pMBHeadNext != pMBHeadNext)
        {
            HEAP_WARNING(bPrint, "[NNS Foundation " "Exp" " Heap]" " Wrong link memory block. - address %08X, next address %08X != %08X\n",
                GetMemCPtrForMBlock(pMBHead), pMBHead->pMBHeadNext, pMBHeadNext);
            return FALSE;
        }

        return TRUE;
    }

// #if ! defined(NNS_FINALROM)
#endif

/*---------------------------------------------------------------------------*
  Name:         CheckMBlockLinkTail

  Description:  メモリブロックのポインタがメモリブロックリストの先頭/末尾であるかチェックします。

  Arguments:    pMBHead:      チェックするメモリブロックのヘッダへのポインタ。
                pMBHeadTail:  メモリブロックリストの先頭/末尾メモリブロックのヘッダへのポインタ。
                headType:     先頭か末尾を表す文字列。
                flag:         フラグ。

  Returns:      メモリブロックのポインタがメモリブロックリストの先頭/末尾であるなら TRUE、
                そうでないなら FALSE を返します。
 *---------------------------------------------------------------------------*/
#if ! defined(NNS_FINALROM)

    static BOOL
    CheckMBlockLinkTail(
        const NNSiFndExpHeapMBlockHead* pMBHead,
        const NNSiFndExpHeapMBlockHead* pMBHeadTail,
        const char*                     heapType,
        u32                             flag
    )
    {
        const BOOL bPrint = 0 != (flag & NNS_FND_HEAP_ERROR_PRINT);

        if (pMBHead != pMBHeadTail)
        {
            HEAP_WARNING(bPrint, "[NNS Foundation " "Exp" " Heap]" " Wrong memory brock list %s pointer. - address %08X, %s address %08X != %08X\n",
                heapType, GetMemCPtrForMBlock(pMBHead), heapType, pMBHead, pMBHeadTail);
            return FALSE;
        }

        return TRUE;
    }

// #if ! defined(NNS_FINALROM)
#endif

/*---------------------------------------------------------------------------*
  Name:         IsValidUsedMBlock

  Description:  使用メモリブロックの妥当性をチェックします。

  Arguments:    memBlock:  チェックするメモリブロック。
                heap:      メモリブロックを含有する拡張ヒープのハンドル。
                           NULLを指定すると、メモリブロックがヒープに含まれているかの
                           チェックは行いません。

  Returns:      指定したメモリブロックが問題なければ TRUE を返します。
                問題があれば FALSE を返します。
 *---------------------------------------------------------------------------*/
#if ! defined(NNS_FINALROM)

    static BOOL
    IsValidUsedMBlock(
        const void*         memBlock,
        NNSFndHeapHandle    heap
    )
    {
        NNSiFndHeapHead* pHeapHd = heap;

        if (! memBlock)
        {
            return FALSE;
        }

        return CheckUsedMBlock(GetMBlockHeadCPtr(memBlock), pHeapHd, 0);
    }

// #if ! defined(NNS_FINALROM)
#endif

/* ========================================================================
    外部関数(非公開)
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNSi_FndDumpExpHeap

  Description:  拡張ヒープ内部の情報を表示します。
                これはデバッグ用の関数です。

  Arguments:    heap:    拡張ヒープのハンドル。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
#if ! defined(NNS_FINALROM)

    void
    NNSi_FndDumpExpHeap(NNSFndHeapHandle heap)
    {
        NNS_ASSERT(IsValidExpHeapHandle(heap));

        {
            u32  usedSize = 0;
            u32  usedCnt = 0;
            u32  freeSize = 0;
            u32  freeCnt = 0;

            NNSiFndHeapHead* pHeapHd = heap;
            NNSiFndExpHeapHead* pExpHeapHd = GetExpHeapHeadPtrFromHandle(pHeapHd);

            NNSi_FndDumpHeapHead(pHeapHd);

            OS_Printf("     attr  address:   size    gid aln   prev_ptr next_ptr\n");   // ヘッダー行

            // ---------------- UsedBlock のダンプ ----------------
            OS_Printf("    (Used Blocks)\n" );

            if (pExpHeapHd->mbUsedList.head == NULL)
            {
                OS_Printf("     NONE\n");
            }
            else
            {
                NNSiFndExpHeapMBlockHead* pMBHead;

                for (pMBHead = pExpHeapHd->mbUsedList.head; pMBHead; pMBHead = pMBHead->pMBHeadNext)
                {
                    if (pMBHead->signature != MBLOCK_USED_SIGNATURE)
                    {
                        OS_Printf("    xxxxx %08x: --------  --- ---  (-------- --------)\nabort\n", pMBHead);
                        break;
                    }

                    OS_Printf("    %s %08x: %8d  %3d %3d  (%08x %08x)\n",
                        GetAllocDirForMBlock(pMBHead) == NNS_FND_EXPHEAP_ALLOC_DIR_REAR ? " rear" : "front",
                        GetMemPtrForMBlock(pMBHead),
                        pMBHead->blockSize,
                        GetGroupIDForMBlock( pMBHead ),
                        GetAlignmentForMBlock( pMBHead ),
                        pMBHead->pMBHeadPrev ? GetMemPtrForMBlock(pMBHead->pMBHeadPrev): NULL,
                        pMBHead->pMBHeadNext ? GetMemPtrForMBlock(pMBHead->pMBHeadNext): NULL
                    );

                    // ---- 使用量
                    usedSize += sizeof(NNSiFndExpHeapMBlockHead) + pMBHead->blockSize + GetAlignmentForMBlock(pMBHead);

                    usedCnt ++;
                }
            }

            // ---------------- FreeBlock のダンプ ----------------
            OS_Printf("    (Free Blocks)\n" );

            if (pExpHeapHd->mbFreeList.head == NULL)
            {
                OS_Printf("     NONE\n" );
            }
            else
            {
                NNSiFndExpHeapMBlockHead* pMBHead;

                for (pMBHead = pExpHeapHd->mbFreeList.head; pMBHead; pMBHead = pMBHead->pMBHeadNext)
                {
                    if (pMBHead->signature != MBLOCK_FREE_SIGNATURE)
                    {
                        OS_Printf("    xxxxx %08x: --------  --- ---  (-------- --------)\nabort\n", pMBHead);
                        break;
                    }

                    OS_Printf("    %s %08x: %8d  %3d %3d  (%08x %08x)\n",
                        " free",
                        GetMemPtrForMBlock(pMBHead),
                        pMBHead->blockSize,
                        GetGroupIDForMBlock( pMBHead ),
                        GetAlignmentForMBlock( pMBHead ),
                        pMBHead->pMBHeadPrev ? GetMemPtrForMBlock(pMBHead->pMBHeadPrev): NULL,
                        pMBHead->pMBHeadNext ? GetMemPtrForMBlock(pMBHead->pMBHeadNext): NULL
                    );

                    freeCnt ++;
                }
            }

            OS_Printf("\n");

            {
                u32 heapSize  = GetOffsetFromPtr(pHeapHd->heapStart, pHeapHd->heapEnd); // ヒープサイズ(データ領域のサイズ)
                OS_Printf("    %d / %d bytes (%6.2f%%) used (U:%d F:%d)\n",
                                   usedSize, heapSize, 100.0 * usedSize / heapSize, usedCnt, freeCnt);
            }

            OS_Printf("\n");
        }
    }

// #if ! defined(NNS_FINALROM)
#endif


/* ========================================================================
    外部関数(公開)
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNS_FndCreateExpHeapEx

  Description:  拡張ヒープを作成します。

  Arguments:    startAddress: ヒープ領域の先頭アドレス。
                size:         ヒープ領域のサイズ。
                optFlag:      オプションフラグ。

  Returns:      関数が成功した場合、作成された拡張ヒープのハンドルが返ります。
                関数が失敗すると、NNS_FND_HEAP_INVALID_HANDLE が返ります。
 *---------------------------------------------------------------------------*/
NNSFndHeapHandle
NNS_FndCreateExpHeapEx(
    void*   startAddress,
    u32     size,
    u16     optFlag
)
{
    void* endAddress;

    SDK_NULL_ASSERT(startAddress);

    endAddress   = NNSi_FndRoundDownPtr(AddU32ToPtr(startAddress, size), MIN_ALIGNMENT);
    startAddress = NNSi_FndRoundUpPtr(startAddress, MIN_ALIGNMENT);

    if ( NNSiGetUIntPtr(startAddress) > NNSiGetUIntPtr(endAddress)
     ||  GetOffsetFromPtr(startAddress, endAddress) < sizeof(NNSiFndHeapHead) + sizeof(NNSiFndExpHeapHead)
                                                        + sizeof(NNSiFndExpHeapMBlockHead) + MIN_ALIGNMENT
    )
    {
        return NNS_FND_HEAP_INVALID_HANDLE;
    }

    {   // Expヒープ向け初期化
        NNSiFndHeapHead* pHeapHd = InitExpHeap(startAddress, endAddress, optFlag);
        return pHeapHd;  // ヒープヘッダへのポインタがそのままハンドル値とする
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndDestroyExpHeap

  Description:  拡張ヒープを破棄します。

  Arguments:    heap: 拡張ヒープのハンドル。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void
NNS_FndDestroyExpHeap(NNSFndHeapHandle heap)
{
    NNS_ASSERT(IsValidExpHeapHandle(heap));

    NNSi_FndFinalizeHeap(heap);
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndAllocFromExpHeapEx

  Description:  拡張ヒープからメモリブロックを確保します。
                メモリブロックのアライメントを指定できます。
                アライメント値を負の値で指定すると、ヒープの空き領域を後方から探します。

  Arguments:    heap:      拡張ヒープのハンドル。
                size:      確保するメモリブロックのサイズ(バイト単位)。
                alignment: 確保するメモリブロックのアライメント。
                           4,8,16,32,-4,-8,-16,-32のいずれかの値が指定できます。

  Returns:      メモリブロックの確保が成功した場合、確保したメモリブロックへの
                ポインタが返ります。
                失敗した場合、NULLが返ります。
 *---------------------------------------------------------------------------*/
void*
NNS_FndAllocFromExpHeapEx(
    NNSFndHeapHandle    heap,
    u32                 size,
    int                 alignment)
{
    void* memory = NULL;

    NNS_ASSERT(IsValidExpHeapHandle(heap));

    // alignment のチェック
    NNS_ASSERT(alignment % MIN_ALIGNMENT == 0);
    NNS_ASSERT(MIN_ALIGNMENT <= abs(alignment) && abs(alignment) <= 32);

    if (size == 0)
    {
        size = 1;
    }

    size = NNSi_FndRoundUp(size, MIN_ALIGNMENT);    // 実際に確保するサイズ

    if (alignment >= 0)     // 前から確保
    {
        memory = AllocFromHead(heap, size, alignment);
    }
    else                    // 後ろから確保
    {
        memory = AllocFromTail(heap, size, -alignment);
    }

    return memory;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndResizeForMBlockExpHeap

  Description:  拡張ヒープから確保されたメモリブロックのサイズを変更します。

  Arguments:    heap:     拡張ヒープのハンドル。
                memBlock: サイズを変更するメモリブロックへのポインタ。
                size:     新しく割り当てるサイズ(バイト単位)。

  Returns:      関数が成功した場合、変更されたメモリブロックのサイズを返します(バイト単位)。
                関数が失敗した場合、0 が返ります。
 *---------------------------------------------------------------------------*/
u32
NNS_FndResizeForMBlockExpHeap(
    NNSFndHeapHandle    heap,
    void*               memBlock,
    u32                 size
)
{
    NNSiFndExpHeapHead* pEHHead;
    NNSiFndExpHeapMBlockHead* pMBHead;

    NNS_ASSERT(IsValidExpHeapHandle(heap));
    NNS_ASSERT(IsValidUsedMBlock(memBlock, heap));

    pEHHead = GetExpHeapHeadPtrFromHandle(heap);
    pMBHead = GetMBlockHeadPtr(memBlock);

    size = NNSi_FndRoundUp(size, MIN_ALIGNMENT);
    if (size == pMBHead->blockSize)  // ブロックサイズ変更なしの場合
    {
        return size;
    }

    // 新しいエリアが拡大するとき
    if (size > pMBHead->blockSize)
    {
        void* crUsedEnd = GetMBlockEndAddr(pMBHead);   // 現使用ブロックのend address
        NNSiFndExpHeapMBlockHead* block;

        // 次のフリーブロックを探す
        for (block = pEHHead->mbFreeList.head; block; block = block->pMBHeadNext)
        {
            if (block == crUsedEnd)
            {
                break;
            }
        }

        // 次のフリーブロックがないか、サイズが足りない場合
        if (! block || size > pMBHead->blockSize + sizeof(NNSiFndExpHeapMBlockHead) + block->blockSize)
        {
            return 0;
        }

        {
            NNSiMemRegion rgnNewFree;
            void* oldFreeStart;
            NNSiFndExpHeapMBlockHead* nextBlockPrev;

            // フリーブロックのリージョンを取得し、フリーブロックを一旦外す
            GetRegionOfMBlock(&rgnNewFree, block);
            nextBlockPrev = RemoveMBlock(&pEHHead->mbFreeList, block);

            oldFreeStart = rgnNewFree.start;
            rgnNewFree.start = AddU32ToPtr(memBlock, size); // 新しくフリーとなるリージョン位置

            // 余りがメモリブロックサイズ未満なら
            if (GetOffsetFromPtr(rgnNewFree.start, rgnNewFree.end) < sizeof(NNSiFndExpHeapMBlockHead))
            {
                rgnNewFree.start = rgnNewFree.end;  // 使用ブロックに吸収
            }

            pMBHead->blockSize = GetOffsetFromPtr(memBlock, rgnNewFree.start);  // 対象ブロックサイズ変更

            // 余りがメモリブロックサイズ以上なら
            if (GetOffsetFromPtr(rgnNewFree.start, rgnNewFree.end) >= sizeof(NNSiFndExpHeapMBlockHead))
            {
                (void)InsertMBlock(&pEHHead->mbFreeList, InitFreeMBlock(&rgnNewFree), nextBlockPrev);   // 新しくフリーブロックを作る
            }

            FillAllocMemory(  // 拡張した部分フィル
                heap,
                oldFreeStart,
                GetOffsetFromPtr(oldFreeStart, rgnNewFree.start));
        }
    }
    // 新しいエリアが縮小するとき
    else
    {
        NNSiMemRegion rgnNewFree;
        const u32 oldBlockSize = pMBHead->blockSize;

        rgnNewFree.start = AddU32ToPtr(memBlock, size); // 新しくフリーとなるリージョン位置
        rgnNewFree.end   = GetMBlockEndAddr(pMBHead);   // 現使用ブロックのend address

        pMBHead->blockSize = size;  // 対象ブロックサイズ変更

        if (! RecycleRegion(pEHHead, &rgnNewFree))    // フリーリストに返してみる
        {
            pMBHead->blockSize = oldBlockSize;  // 失敗したら、元に戻す
        }
    }

    return pMBHead->blockSize;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndFreeToExpHeap

  Description:  拡張ヒープへメモリブロックを返却します。

  Arguments:    heap:     拡張ヒープのハンドル。
                memBlock: 返却するメモリブロックへのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void
NNS_FndFreeToExpHeap(
    NNSFndHeapHandle    heap,
    void*               memBlock
)
{
    NNS_ASSERT(IsValidExpHeapHandle(heap));

    {
        NNSiFndHeapHead* pHeapHd = heap;
        NNSiFndExpHeapHead* pExpHeapHd = GetExpHeapHeadPtrFromHandle(pHeapHd);
        NNSiFndExpHeapMBlockHead* pMBHead = GetMBlockHeadPtr(memBlock);
        NNSiMemRegion region;

        // このヒープの中に入っているか
        NNS_ASSERT(pHeapHd->heapStart <= memBlock && memBlock < pHeapHd->heapEnd);

        GetRegionOfMBlock(&region, pMBHead);
        (void)RemoveMBlock(&pExpHeapHd->mbUsedList, pMBHead);   // 使用リストからはずす
        (void)RecycleRegion(pExpHeapHd, &region);   // 指定アドレスから指定サイズをフリーに加える
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetTotalFreeSizeForExpHeap

  Description:  拡張ヒープの空き領域のサイズの合計を取得します。

  Arguments:    heap:     拡張ヒープのハンドル。

  Returns:      拡張ヒープの空き領域のサイズの合計を返します(バイト単位)。
 *---------------------------------------------------------------------------*/
u32
NNS_FndGetTotalFreeSizeForExpHeap(NNSFndHeapHandle heap)
{
    NNS_ASSERT(IsValidExpHeapHandle(heap));

    {
        u32 sumSize = 0;
        NNSiFndExpHeapHead* pEHHead = GetExpHeapHeadPtrFromHandle(heap);
        NNSiFndExpHeapMBlockHead* pMBHead;

        for(pMBHead = pEHHead->mbFreeList.head; pMBHead; pMBHead = pMBHead->pMBHeadNext)
        {
            sumSize += pMBHead->blockSize;
        }

        return sumSize;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetAllocatableSizeForExpHeapEx

  Description:  拡張ヒープ内の割り当て可能な最大サイズを取得します。
                メモリブロックのアライメントを指定できます。

  Arguments:    heap:      拡張ヒープのハンドル。
                alignment: 確保するメモリブロックのアライメント。
                           4,8,16,32のいずれかの値が指定できます。

  Returns:      拡張ヒープ内の割り当て可能な最大サイズを返します(バイト単位)。
 *---------------------------------------------------------------------------*/
u32
NNS_FndGetAllocatableSizeForExpHeapEx(
    NNSFndHeapHandle    heap,
    int                 alignment
)
{
    NNS_ASSERT(IsValidExpHeapHandle(heap));

    // alignment のチェック
    NNS_ASSERT(alignment % MIN_ALIGNMENT == 0);
    NNS_ASSERT(MIN_ALIGNMENT <= abs(alignment) && abs(alignment) <= 32);

    alignment = abs(alignment); // 念のため正数化

    {
        NNSiFndExpHeapHead* pEHHead = GetExpHeapHeadPtrFromHandle(heap);
        u32 maxSize = 0;
        u32 offsetMin = 0xFFFFFFFF;
        NNSiFndExpHeapMBlockHead* pMBlkHd;

        for (pMBlkHd = pEHHead->mbFreeList.head; pMBlkHd; pMBlkHd = pMBlkHd->pMBHeadNext)
        {
            // アライメントを考慮したメモリブロック位置
            void* baseAddress = NNSi_FndRoundUpPtr(GetMemPtrForMBlock(pMBlkHd), alignment);

            if (NNSiGetUIntPtr(baseAddress) < NNSiGetUIntPtr(GetMBlockEndAddr(pMBlkHd)))
            {
                const u32 blockSize = GetOffsetFromPtr(baseAddress, GetMBlockEndAddr(pMBlkHd));
                // アライメントによる空きエリア
                const u32 offset = GetOffsetFromPtr(GetMemPtrForMBlock(pMBlkHd), baseAddress);

                /*
                    サイズが大きい場合、あるいはサイズが同じでも無駄な空間がより小さい場合は、
                    メモリブロックを更新
                 */
                if ( maxSize < blockSize
                 ||  (maxSize == blockSize && offsetMin > offset)
                )
                {
                    maxSize = blockSize;
                    offsetMin= offset;
                }
            }
        }

        return maxSize;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndSetAllocModeForExpHeap

  Description:  拡張ヒープのメモリ確保モードをセットします。

  Arguments:    heap:  拡張ヒープのハンドル。
                mode:  メモリ確保モード。

  Returns:      以前の拡張ヒープのメモリ確保モードを返します。
 *---------------------------------------------------------------------------*/
u16
NNS_FndSetAllocModeForExpHeap(
    NNSFndHeapHandle    heap,
    u16                 mode
)
{
    NNS_ASSERT(IsValidExpHeapHandle(heap));

    {
        NNSiFndExpHeapHead *const pEHHead = GetExpHeapHeadPtrFromHandle(heap);
        u16 oldMode = GetAllocMode(pEHHead);
        SetAllocMode(pEHHead, mode);

        return oldMode;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetAllocModeForExpHeap

  Description:  拡張ヒープのメモリ確保モードを取得します。

  Arguments:    heap:    拡張ヒープのハンドル。

  Returns:      拡張ヒープのメモリ確保モードを返します。
 *---------------------------------------------------------------------------*/
u16
NNS_FndGetAllocModeForExpHeap(NNSFndHeapHandle heap)
{
    NNS_ASSERT(IsValidExpHeapHandle(heap));
    return GetAllocMode(GetExpHeapHeadPtrFromHandle(heap));
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndSetGroupIDForExpHeap

  Description:  拡張ヒープのグループIDをセットします。

  Arguments:    heap:    拡張ヒープのハンドル。
                groupID: セットするグループID値。

  Returns:      以前のグループID値が返ります。
 *---------------------------------------------------------------------------*/
u16
NNS_FndSetGroupIDForExpHeap(
    NNSFndHeapHandle    heap,
    u16                 groupID)
{
    NNS_ASSERT(IsValidExpHeapHandle(heap));
    NNS_ASSERT(groupID <= MAX_GROUPID);

    {
        NNSiFndExpHeapHead* pEHHead = GetExpHeapHeadPtrFromHandle(heap);
        u16 oldGroupID = pEHHead->groupID;
        pEHHead->groupID = groupID;

        return oldGroupID;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetGroupIDForExpHeap

  Description:  拡張ヒープのグループIDを取得します。

  Arguments:    heap:    拡張ヒープのハンドル。

  Returns:      グループID値が返ります。
 *---------------------------------------------------------------------------*/
u16
NNS_FndGetGroupIDForExpHeap(NNSFndHeapHandle heap)
{
    NNS_ASSERT(IsValidExpHeapHandle(heap));

    return GetExpHeapHeadPtrFromHandle(heap)->groupID;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndVisitAllocatedForExpHeap

  Description:  拡張ヒープから割り当てられたメモリブロック全てに対して、
                ユーザが指定した関数を呼ばせます。
                visitor関数で呼ばれるメモリブロックの順番は、確保した順番になります。

                visitor の型 NNSFndHeapVisitor は次のように定義されています。

                    typedef void (*NNSFndHeapVisitor)(
                                    void*               memBlock,
                                    NNSFndHeapHandle    heap,
                                    u32                 userParam);

                                        memBlock:   メモリブロックへのポインタ。
                                        heap:       メモリブロックを含有するヒープ。
                                        userParam:  ユーザー用パラメータ。

  Arguments:    heap:       拡張ヒープのハンドル。
                visitor:    各メモリブロックに対して呼ばせる関数。
                userParam:  visitor関数に渡すユーザ指定のパラメータ

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void
NNS_FndVisitAllocatedForExpHeap(
    NNSFndHeapHandle    heap,
    NNSFndHeapVisitor   visitor,
    u32                 userParam
)
{
    NNS_ASSERT(IsValidExpHeapHandle(heap));
    SDK_NULL_ASSERT(visitor);

    {
        NNSiFndExpHeapMBlockHead* pMBHead = GetExpHeapHeadPtrFromHandle(heap)->mbUsedList.head;

        while (pMBHead)
        {
            NNSiFndExpHeapMBlockHead* pMBHeadNext = pMBHead->pMBHeadNext;
            (*visitor)(GetMemPtrForMBlock(pMBHead), heap, userParam);
            pMBHead = pMBHeadNext;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetSizeForMBlockExpHeap

  Description:  拡張ヒープから確保されたメモリブロックのサイズを取得します。

  Arguments:    memBlock:  サイズを取得するメモリブロックへのポインタ。

  Returns:      指定したメモリブロックのサイズを返します(バイト単位)。
 *---------------------------------------------------------------------------*/
u32
NNS_FndGetSizeForMBlockExpHeap(const void* memBlock)
{
    NNS_ASSERT(IsValidUsedMBlock(memBlock, NULL));

    return GetMBlockHeadCPtr(memBlock)->blockSize;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetGroupIDForMBlockExpHeap

  Description:  拡張ヒープから確保されたメモリブロックのグループIDを取得します。

  Arguments:    memBlock:  グループIDを取得するメモリブロックへのポインタ。

  Returns:      指定したメモリブロックのグループIDが返ります。
 *---------------------------------------------------------------------------*/
u16
NNS_FndGetGroupIDForMBlockExpHeap(const void* memBlock)
{
    NNS_ASSERT(IsValidUsedMBlock(memBlock, NULL));

    return GetGroupIDForMBlock(GetMBlockHeadCPtr(memBlock));
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetAllocDirForMBlockExpHeap

  Description:  拡張ヒープから確保されたメモリブロックの確保方向を取得します。

  Arguments:    memBlock:  メモリブロックへのポインタ。

  Returns:      指定したメモリブロックの確保方向が返ります。
 *---------------------------------------------------------------------------*/
u16
NNS_FndGetAllocDirForMBlockExpHeap(const void* memBlock)
{
    NNS_ASSERT(IsValidUsedMBlock(memBlock, NULL));

    return GetAllocDirForMBlock(GetMBlockHeadCPtr(memBlock));
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndCheckExpHeap

  Description:  拡張ヒープが破壊されていないかどうかをチェックします。

  Arguments:    heap:     拡張ヒープのハンドル。
                optFlag:  フラグ。

  Returns:      ヒープが正常だった場合 TRUE を返します。
                ヒープにエラーがあった場合、FALSE を返します。
 *---------------------------------------------------------------------------*/
#if ! defined(NNS_FINALROM)

    BOOL
    NNS_FndCheckExpHeap(
        NNSFndHeapHandle    heap,
        u32                 optFlag
    )
    {
        const BOOL bPrint = 0 != (optFlag & NNS_FND_HEAP_ERROR_PRINT);
        u32  totalBytes  = 0;

        if (! IsValidExpHeapHandle(heap))
        {
            HEAP_WARNING(bPrint, "[NNS Foundation " "Exp" " Heap]" " Invalid heap handle. - %08X\n", heap);
            return FALSE;
        }

        {
            NNSiFndHeapHead *const pHeapHd = heap;
            NNSiFndExpHeapHead *const pExpHeapHd = GetExpHeapHeadPtrFromHeapHead(pHeapHd);
            NNSiFndExpHeapMBlockHead* pMBHead = NULL;
            NNSiFndExpHeapMBlockHead* pMBHeadPrev = NULL;

            // 使用ブロック
            for (pMBHead = pExpHeapHd->mbUsedList.head; pMBHead; pMBHeadPrev = pMBHead, pMBHead = pMBHead->pMBHeadNext)
            {
                if ( ! CheckUsedMBlock(pMBHead, pHeapHd, optFlag)
                  || ! CheckMBlockPrevPtr(pMBHead, pMBHeadPrev, optFlag)   // 前ブロックへのポインタが1ループ前のメモリブロックのポインタと同じでない?
                )
                {
                    return FALSE;
                }

                // 占有サイズを積算
                totalBytes += sizeof(NNSiFndExpHeapMBlockHead) + pMBHead->blockSize + GetAlignmentForMBlock(pMBHead);
            }

            if (! CheckMBlockLinkTail(pMBHeadPrev, pExpHeapHd->mbUsedList.tail, "tail", optFlag))  // 末尾ブロックが最後のブロックへのポインタを指しているか?
            {
                return FALSE;
            }

            // フリーブロック
            pMBHead = NULL;
            pMBHeadPrev = NULL;
            for (pMBHead = pExpHeapHd->mbFreeList.head; pMBHead; pMBHeadPrev = pMBHead, pMBHead = pMBHead->pMBHeadNext)
            {
                if ( ! CheckFreeMBlock(pMBHead, pHeapHd, optFlag)
                  || ! CheckMBlockPrevPtr(pMBHead, pMBHeadPrev, optFlag)   // 前ブロックへのポインタが1ループ前のメモリブロックのポインタと同じでない?
                )
                {
                    return FALSE;
                }

                // 占有サイズを積算
                totalBytes += sizeof(NNSiFndExpHeapMBlockHead) + pMBHead->blockSize;
            }

            if (! CheckMBlockLinkTail(pMBHeadPrev, pExpHeapHd->mbFreeList.tail, "tail", optFlag))  // 末尾ブロックが最後のブロックへのポインタを指しているか?
            {
                return FALSE;
            }

            // 全体の結果表示
            if (totalBytes != GetOffsetFromPtr(pHeapHd->heapStart, pHeapHd->heapEnd))
            {
                HEAP_WARNING(bPrint, "[NNS Foundation " "Exp" " Heap]" " Incorrect total memory block size. - heap size %08X, sum size %08X\n",
                    GetOffsetFromPtr(pHeapHd->heapStart, pHeapHd->heapEnd), totalBytes);
                return FALSE;
            }

            return TRUE;
        }
    }

// #if ! defined(NNS_FINALROM)
#endif

/*---------------------------------------------------------------------------*
  Name:         NNS_FndCheckForMBlockExpHeap

  Description:  拡張ヒープのメモリブロックが破壊されていないかどうかをチェックします。

  Arguments:    memBlock:  チェックするメモリブロックへのポインタ。
                heap:      拡張ヒープのハンドル。
                optFlag:   フラグ。

  Returns:      メモリブロックが正常だった場合 TRUE を返します。
                メモリブロックにエラーがあった場合、FALSE を返します。
 *---------------------------------------------------------------------------*/
#if ! defined(NNS_FINALROM)

    BOOL
    NNS_FndCheckForMBlockExpHeap(
        const void*         memBlock,
        NNSFndHeapHandle    heap,
        u32                 optFlag
    )
    {
        const NNSiFndExpHeapMBlockHead* pMBHead = NULL;
        NNSiFndHeapHead *const pHeapHd = heap;

        if (! memBlock)
        {
            return FALSE;
        }

        pMBHead = GetMBlockHeadCPtr(memBlock);

        if (! CheckUsedMBlock(pMBHead, pHeapHd, optFlag))
        {
            return FALSE;
        }

        if (pMBHead->pMBHeadPrev)
        {
            if ( ! CheckUsedMBlock(pMBHead->pMBHeadPrev, pHeapHd, optFlag)     // 前ブロックのシグネチャとサイズのチェック
              || ! CheckMBlockNextPtr(pMBHead->pMBHeadPrev, pMBHead, optFlag)  // 前ブロックの次ブロックへのポインタが自分を指しているか?
            )
            {
                return FALSE;
            }
        }
        else
        {
            if (pHeapHd)
            {
                // prevが NULL のときは、リストのheadポインタは自分を指しているはず
                if (! CheckMBlockLinkTail(pMBHead, GetExpHeapHeadPtrFromHeapHead(pHeapHd)->mbUsedList.head, "head", optFlag))
                {
                    return FALSE;
                }
            }
        }

        if (pMBHead->pMBHeadNext)
        {
            if ( ! CheckUsedMBlock(pMBHead->pMBHeadNext, pHeapHd, optFlag)     // 次ブロックのシグネチャとサイズのチェック
              || ! CheckMBlockPrevPtr(pMBHead->pMBHeadNext, pMBHead, optFlag)  // 次ブロックの前ブロックへのポインタが自分を指しているか?
            )
            {
                return FALSE;
            }
        }
        else
        {
            if (pHeapHd)
            {
                // nextが NULL のときは、リストのtailポインタは自分を指しているはず
                if (! CheckMBlockLinkTail(pMBHead, GetExpHeapHeadPtrFromHeapHead(pHeapHd)->mbUsedList.tail, "tail", optFlag))
                {
                    return FALSE;
                }
            }
        }

        return TRUE;
    }

// #if ! defined(NNS_FINALROM)
#endif
