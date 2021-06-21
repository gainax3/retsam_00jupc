/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - fnd
  File:     frameheap.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.20 $
 *---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <nitro.h>
#include <nnsys/misc.h>
#include <nnsys/fnd/frameheap.h>
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

static NNS_FND_INLINE BOOL
IsValidFrmHeapHandle(NNSFndHeapHandle handle)
{
    if(handle == NNS_FND_HEAP_INVALID_HANDLE)
    {
        return FALSE;
    }

    {
        NNSiFndHeapHead* pHeapHd = handle;
        return pHeapHd->signature == NNSI_FRMHEAP_SIGNATURE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         GetFrmHeapHeadPtrFromHeapHead

  Description:  ヒープヘッダへのポインタから、フレームヒープヘッダへのポインタを取得します。

  Arguments:    pHHead:  ヒープヘッダへのポインタ。

  Returns:      フレームヒープヘッダへのポインタを返します。
 *---------------------------------------------------------------------------*/
static NNS_FND_INLINE NNSiFndFrmHeapHead*
GetFrmHeapHeadPtrFromHeapHead(NNSiFndHeapHead* pHHead)
{
    return AddU32ToPtr(pHHead, sizeof(NNSiFndHeapHead));
}

/*---------------------------------------------------------------------------*
  Name:         GetHeapHeadPtrFromFrmHeapHead

  Description:  フレームヒープヘッダへのポインタから、ヒープヘッダへのポインタを取得します。

  Arguments:    pFrmHeapHd:  フレームヒープヘッダへのポインタ。

  Returns:      ヒープヘッダへのポインタを返します。
 *---------------------------------------------------------------------------*/
static NNS_FND_INLINE NNSiFndHeapHead*
GetHeapHeadPtrFromFrmHeapHead(NNSiFndFrmHeapHead* pFrmHeapHd)
{
    return SubU32ToPtr(pFrmHeapHd, sizeof(NNSiFndHeapHead));
}

/*---------------------------------------------------------------------------*
  Name:         InitFrameHeap

  Description:  フレームヒープの初期化を行います。

  Arguments:    startAddress:  フレームヒープとするメモリの開始アドレス。
                endAddress:    フレームヒープとするメモリの終了アドレス +1。
                optFlag:       オプションフラグ。

  Returns:      ヒープヘッダへのポインタを返します。
 *---------------------------------------------------------------------------*/
static NNSiFndHeapHead*
InitFrameHeap(
    void*   startAddress,
    void*   endAddress,
    u16     optFlag
)
{
    NNSiFndHeapHead* pHeapHd = startAddress;
    NNSiFndFrmHeapHead* pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(pHeapHd);

    NNSi_FndInitHeapHead(       // ヒープ共通初期化
        pHeapHd,
        NNSI_FRMHEAP_SIGNATURE,
        AddU32ToPtr(pFrmHeapHd, sizeof(NNSiFndFrmHeapHead)),    // heapStart
        endAddress,                                             // heapEnd
        optFlag);

    pFrmHeapHd->headAllocator = pHeapHd->heapStart;
    pFrmHeapHd->tailAllocator = pHeapHd->heapEnd;

    pFrmHeapHd->pState = NULL;   // 状態保存ステート位置

    return pHeapHd;
}


/*---------------------------------------------------------------------------*
  Name:         AllocFromHead

  Description:  ヒープの先頭からメモリブロックを確保します。
                アラインメントの指定があります。

  Arguments:    pHHead:  ヒープヘッダへのポインタ。
                size:    確保するメモリブロックのサイズ。
                alignment:  アライメント値。

  Returns:      メモリブロックの確保が成功した場合、確保したメモリブロックへの
                ポインタが返ります。
                失敗した場合、NULLが返ります。
 *---------------------------------------------------------------------------*/
static void*
AllocFromHead(
    NNSiFndFrmHeapHead* pFrmHeapHd,
    u32                 size,
    int                 alignment
)
{
    void* newBlock = NNSi_FndRoundUpPtr(pFrmHeapHd->headAllocator, alignment);
    void* endAddress = AddU32ToPtr(newBlock, size);

    if(NNSiGetUIntPtr(endAddress) > NNSiGetUIntPtr(pFrmHeapHd->tailAllocator))
    {
        return NULL;
    }

    FillAllocMemory(  // メモリ充填
        GetHeapHeadPtrFromFrmHeapHead(pFrmHeapHd),
        pFrmHeapHd->headAllocator,
        GetOffsetFromPtr(pFrmHeapHd->headAllocator, endAddress));

    pFrmHeapHd->headAllocator = endAddress;

    return newBlock;
}

/*---------------------------------------------------------------------------*
  Name:         AllocFromTail

  Description:  ヒープの末尾からメモリブロックを確保します。
                アラインメントの指定があります。

  Arguments:    pHHead:     ヒープヘッダへのポインタ。
                size:       確保するメモリブロックのサイズ。
                alignment:  アライメント値。

  Returns:      メモリブロックの確保が成功した場合、確保したメモリブロックへの
                ポインタが返ります。
                失敗した場合、NULLが返ります。
 *---------------------------------------------------------------------------*/
static void*
AllocFromTail(
    NNSiFndFrmHeapHead* pFrmHeapHd,
    u32                 size,
    int                 alignment
)
{
    void* newBlock = NNSi_FndRoundDownPtr(SubU32ToPtr(pFrmHeapHd->tailAllocator, size), alignment);

    if(NNSiGetUIntPtr(newBlock) < NNSiGetUIntPtr(pFrmHeapHd->headAllocator))
    {
        return NULL;
    }

    FillAllocMemory(  // メモリ充填
        GetHeapHeadPtrFromFrmHeapHead(pFrmHeapHd),
        newBlock,
        GetOffsetFromPtr(newBlock, pFrmHeapHd->tailAllocator));

    pFrmHeapHd->tailAllocator = newBlock;

    return newBlock;
}

/*---------------------------------------------------------------------------*
  Name:         FreeHead

  Description:  ヒープ領域の先頭から確保したメモリブロックを一括して開放します。

  Arguments:    pHeapHd:  ヒープのヘッダへのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static void
FreeHead(NNSiFndHeapHead* pHeapHd)
{
    NNSiFndFrmHeapHead* pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(pHeapHd);

    FillFreeMemory(
        pHeapHd,
        pHeapHd->heapStart,
        GetOffsetFromPtr(pHeapHd->heapStart, pFrmHeapHd->headAllocator));

    pFrmHeapHd->headAllocator = pHeapHd->heapStart;
    pFrmHeapHd->pState = NULL;
}

/*---------------------------------------------------------------------------*
  Name:         FreeTail

  Description:  ヒープから確保した全てのメモリブロックを一括して開放します。

  Arguments:    pHeapHd:  ヒープのヘッダへのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static void
FreeTail(NNSiFndHeapHead* pHeapHd)
{
    NNSiFndFrmHeapHead* pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(pHeapHd);

    FillFreeMemory(
        pHeapHd,
        pFrmHeapHd->tailAllocator,
        GetOffsetFromPtr(pFrmHeapHd->tailAllocator, pHeapHd->heapEnd));

    /*
        ヒープの割り当て状態の復帰により解放したメモリブロックが復活してしまわ
        ないように、保存情報の後尾割り当てポインタを再設定しておく。
     */
    {
        NNSiFndFrmHeapState* pState;
        for (pState = pFrmHeapHd->pState; pState; pState = pState->pPrevState)
        {
            pState->tailAllocator = pHeapHd->heapEnd;
        }
    }

    pFrmHeapHd->tailAllocator = pHeapHd->heapEnd;
}

/*---------------------------------------------------------------------------*
  Name:         PrintSize

  Description:  サイズとパーセンテージを出力します。

  Arguments:    size:       対象となるサイズ。
                wholeSize:  全体のサイズ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
#if ! defined(NNS_FINALROM)

    static void
    PrintSize(
        u32     size,
        u32     wholeSize
    )
    {
        OS_Printf("%9d (%6.2f%%)", size, 100.0 * size / wholeSize);
    }

// #if ! defined(NNS_FINALROM)
#endif


/* ========================================================================
    外部関数(非公開)
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNSi_FndGetFreeStartForFrmHeap

  Description:  フレームヒープのフリーエリアの先頭アドレスを取得します。

  Arguments:    heap: フレームヒープのハンドル。

  Returns:      フレームヒープのフリーエリアの先頭アドレスを返します。
 *---------------------------------------------------------------------------*/
void*
NNSi_FndGetFreeStartForFrmHeap(NNSFndHeapHandle heap)
{
    NNS_ASSERT(IsValidFrmHeapHandle(heap));

    return GetFrmHeapHeadPtrFromHeapHead(heap)->headAllocator;
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_FndGetFreeEndForFrmHeap

  Description:  フレームヒープのフリーエリアの末尾アドレスを取得します。

  Arguments:    heap: フレームヒープのハンドル。

  Returns:      フレームヒープのフリーエリアの末尾アドレス +1 を返します。
 *---------------------------------------------------------------------------*/
void*
NNSi_FndGetFreeEndForFrmHeap(NNSFndHeapHandle heap)
{
    NNS_ASSERT(IsValidFrmHeapHandle(heap));

    return GetFrmHeapHeadPtrFromHeapHead(heap)->tailAllocator;
}


/*---------------------------------------------------------------------------*
  Name:         NNSi_FndDumpFrmHeap

  Description:  フレームヒープ内部の情報を表示します。
                これはデバッグ用の関数です。

  Arguments:    heap:    フレームヒープのハンドル。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
#if ! defined(NNS_FINALROM)

    void
    NNSi_FndDumpFrmHeap(NNSFndHeapHandle heap)
    {
        NNS_ASSERT(IsValidFrmHeapHandle(heap));

        {
            NNSiFndHeapHead *const pHeapHd = heap;
            NNSiFndFrmHeapHead *const pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(pHeapHd);
            const u32 heapSize = GetOffsetFromPtr(pHeapHd->heapStart, pHeapHd->heapEnd);

            NNSi_FndDumpHeapHead(pHeapHd);

            OS_Printf(  "     head [%p - %p) ", pHeapHd->heapStart, pFrmHeapHd->headAllocator);
            PrintSize(GetOffsetFromPtr(pHeapHd->heapStart, pFrmHeapHd->headAllocator), heapSize);
            OS_Printf("\n     free                           ");
            PrintSize(GetOffsetFromPtr(pFrmHeapHd->headAllocator, pFrmHeapHd->tailAllocator), heapSize);
            OS_Printf("\n     tail [%p - %p) ", pFrmHeapHd->tailAllocator, pHeapHd->heapEnd);
            PrintSize(GetOffsetFromPtr(pFrmHeapHd->tailAllocator, pHeapHd->heapEnd), heapSize);
            OS_Printf("\n");

            if (pFrmHeapHd->pState)
            {
                NNSiFndFrmHeapState* pState;

                OS_Printf("    state : [tag]   [head]      [tail]\n");

                for (pState = pFrmHeapHd->pState; pState; pState = pState->pPrevState)
                {
                    OS_Printf("            '%c%c%c%c' : %p %p\n", pState->tagName >>24, (pState->tagName >>16) & 0xFF, (pState->tagName >>8) & 0xFF, pState->tagName & 0xFF,
                                            pState->headAllocator, pState->tailAllocator);
                }
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
  Name:         NNS_FndCreateFrmHeapEx

  Description:  フレームヒープを作成します。

  Arguments:    startAddress: ヒープ領域の先頭アドレス。
                size:         ヒープ領域のサイズ。
                optFlag:      オプションフラグ。

  Returns:      関数が成功した場合、作成されたフレームヒープのハンドルが返ります。
                関数が失敗すると、NNS_FND_INVALID_HEAP_HANDLE が返ります。

  Memo:         基本はスレッドセーフではない。
                スレッドセーフにする場合、ヒープの属性を指定する引数を追加するようにするか、
                あるいは、属性をセットする関数で制御してもらうか。
 *---------------------------------------------------------------------------*/
NNSFndHeapHandle
NNS_FndCreateFrmHeapEx(
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
     ||  GetOffsetFromPtr(startAddress, endAddress) < sizeof(NNSiFndHeapHead) + sizeof(NNSiFndFrmHeapHead)
    )
    {
        return NNS_FND_HEAP_INVALID_HANDLE;
    }

    {   // Frame ヒープ向け初期化
        NNSiFndHeapHead* pHHead = InitFrameHeap(startAddress, endAddress, optFlag);
        return pHHead;  // ヒープヘッダへのポインタがそのままハンドル値とする
    }
}


/*---------------------------------------------------------------------------*
  Name:         NNS_FndDestroyFrmHeap

  Description:  フレームヒープを破棄します。

  Arguments:    heap: フレームヒープのハンドル。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void
NNS_FndDestroyFrmHeap(NNSFndHeapHandle heap)
{
    NNS_ASSERT(IsValidFrmHeapHandle(heap));

    NNSi_FndFinalizeHeap(heap);
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndAllocFromFrmHeapEx

  Description:  フレームヒープからメモリブロックを確保します。
                メモリブロックのアライメントを指定できます。
                アライメント値を負の値で指定すると、ヒープの空き領域を後方から探します。

  Arguments:    heap:      フレームヒープのハンドル。
                size:      確保するメモリブロックのサイズ(バイト単位)。
                alignment: 確保するメモリブロックのアライメント。
                           4,8,16,32,-4,-8,-16,-32のいずれかの値が指定できます。

  Returns:      メモリブロックの確保が成功した場合、確保したメモリブロックへの
                ポインタが返ります。
                失敗した場合、NULLが返ります。
 *---------------------------------------------------------------------------*/
void*
NNS_FndAllocFromFrmHeapEx(
    NNSFndHeapHandle    heap,
    u32                 size,
    int                 alignment
)
{
    void* memory = NULL;
    NNSiFndFrmHeapHead* pFrmHeapHd;

    NNS_ASSERT(IsValidFrmHeapHandle(heap));

    // alignment のチェック
    NNS_ASSERT(alignment % MIN_ALIGNMENT == 0);
    NNS_ASSERT(MIN_ALIGNMENT <= abs(alignment) && abs(alignment) <= 32);

    pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(heap);

    if (size == 0)
    {
        size = 1;
    }

    size = NNSi_FndRoundUp(size, MIN_ALIGNMENT);

    if (alignment >= 0)   // ヒープ前から確保
    {
        memory = AllocFromHead(pFrmHeapHd, size, alignment);
    }
    else                    // ヒープ後ろから確保
    {
        memory = AllocFromTail(pFrmHeapHd, size, -alignment);
    }

    return memory;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndFreeToFrmHeap

  Description:  フレームヒープへメモリブロックを返却します。

  Arguments:    heap: フレームヒープのハンドル。
                mode: メモリブロックの返却方法。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void
NNS_FndFreeToFrmHeap(
    NNSFndHeapHandle    heap,
    int                 mode
)
{
    NNS_ASSERT(IsValidFrmHeapHandle(heap));

    if (mode & NNS_FND_FRMHEAP_FREE_HEAD)
    {
        FreeHead(heap);
    }

    if (mode & NNS_FND_FRMHEAP_FREE_TAIL)
    {
        FreeTail(heap);
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetAllocatableSizeForFrmHeapEx

  Description:  フレームヒープ内の割り当て可能な最大サイズを取得します。
                メモリブロックのアライメントを指定できます。

  Arguments:    heap:      フレームヒープのハンドル。
                alignment: 確保するメモリブロックのアライメント。
                           4,8,16,32のいずれかの値が指定できます。

  Returns:      フレームヒープ内の割り当て可能な最大サイズを返します(バイト単位)。
 *---------------------------------------------------------------------------*/
u32
NNS_FndGetAllocatableSizeForFrmHeapEx(
    NNSFndHeapHandle    heap,
    int                 alignment
)
{
    NNS_ASSERT(IsValidFrmHeapHandle(heap));

    // alignment のチェック
    NNS_ASSERT(alignment % MIN_ALIGNMENT == 0);
    NNS_ASSERT(MIN_ALIGNMENT <= abs(alignment) && abs(alignment) <= 32);

    alignment = abs(alignment); // 念のため正数化

    {
        const NNSiFndFrmHeapHead* pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(heap);
        const void* block = NNSi_FndRoundUpPtr(pFrmHeapHd->headAllocator, alignment);

        if (NNSiGetUIntPtr(block) > NNSiGetUIntPtr(pFrmHeapHd->tailAllocator))
        {
            return 0;
        }

        return GetOffsetFromPtr(block, pFrmHeapHd->tailAllocator);
    }
}


/*---------------------------------------------------------------------------*
  Name:         NNS_FndRecordStateForFrmHeap

  Description:  フレームヒープの現在のメモリ使用状態を記録します。
                後で記録したメモリ使用状況に戻すことができます。
                状態を記録するのに20バイト使用します。

  Arguments:    heap:     フレームヒープのハンドル。
                tagName:  状態記録に付けるタグ名。

  Returns:      関数が成功した場合、TRUEが返ります。
                失敗したら、FALSEが返ります。
 *---------------------------------------------------------------------------*/
BOOL
NNS_FndRecordStateForFrmHeap(
    NNSFndHeapHandle    heap,
    u32                 tagName
)
{
    NNS_ASSERT(IsValidFrmHeapHandle(heap));

    {
        NNSiFndFrmHeapHead* pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(heap);
        void* oldHeadAllocator = pFrmHeapHd->headAllocator;

        // 情報保存領域確保
        NNSiFndFrmHeapState* pState = AllocFromHead(pFrmHeapHd, sizeof(NNSiFndFrmHeapState), MIN_ALIGNMENT);
        if (! pState)
        {
            return FALSE;
        }

        // 現在の状態を格納
        pState->tagName       = tagName;
        pState->headAllocator = oldHeadAllocator;
        pState->tailAllocator = pFrmHeapHd->tailAllocator;
        pState->pPrevState    = pFrmHeapHd->pState;

        pFrmHeapHd->pState = pState;

        return TRUE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndFreeByStateToFrmHeap

  Description:  フレームヒープのメモリブロックを記録された状態に従って返却します。
                指定したタグ名を用いてNNS_FndRecordStateForFrmHeap()を呼び出す直前
                のメモリの使用状況に戻ります。
                タグ名に0を指定すると最後にNNS_FndRecordStateForFrmHeap()を呼び出す
                直前の状態になります。

                タグ名を指定して返却した場合、それ以後に呼び出された
                NNS_FndRecordStateForFrmHeap()によって記録された情報は
                無くなります。

  Arguments:    heap:     フレームヒープのハンドル。
                tagName:  状態記録に付けるタグ名。

  Returns:      関数が成功した場合、TRUEが返ります。
                失敗したら、FALSEが返ります。
 *---------------------------------------------------------------------------*/
BOOL
NNS_FndFreeByStateToFrmHeap(
    NNSFndHeapHandle    heap,
    u32                 tagName
)
{
    NNS_ASSERT(IsValidFrmHeapHandle(heap));

    {
        NNSiFndFrmHeapHead* pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(heap);
        NNSiFndFrmHeapState* pState = pFrmHeapHd->pState;

        if (tagName != 0)   // タグ名の指定あり
        {
            for(; pState; pState = pState->pPrevState)
            {
                if(pState->tagName == tagName)
                    break;
            }
        }

        if (! pState)
        {
            return FALSE;
        }

        pFrmHeapHd->headAllocator = pState->headAllocator;
        pFrmHeapHd->tailAllocator = pState->tailAllocator;

        pFrmHeapHd->pState = pState->pPrevState;

        return TRUE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndAdjustFrmHeap

  Description:  フレームヒープの空き領域をヒープ領域から開放し、その分ヒープ領域を
                縮小します。
                ヒープメモリの後ろから確保されたメモリブロックが存在していては
                いけません。

  Arguments:    heap:     フレームヒープのハンドル。

  Returns:      関数が成功した場合、縮小後のフレームヒープのサイズを返します
                (バイト単位)。
                失敗した場合、0を返します。
 *---------------------------------------------------------------------------*/
u32
NNS_FndAdjustFrmHeap(NNSFndHeapHandle heap)
{
    NNS_ASSERT(IsValidFrmHeapHandle(heap));

    {
        NNSiFndHeapHead* pHeapHd = heap;
        NNSiFndFrmHeapHead* pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(pHeapHd);

        // 後方に確保されたメモリブロックが存在する場合は失敗する
        if(0 < GetOffsetFromPtr(pFrmHeapHd->tailAllocator, pHeapHd->heapEnd))
        {
            return 0;
        }

        pFrmHeapHd->tailAllocator = pHeapHd->heapEnd = pFrmHeapHd->headAllocator;

        return GetOffsetFromPtr(heap, pHeapHd->heapEnd);
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndResizeForMBlockFrmHeap

  Description:  フレームヒープから確保されたメモリブロックのサイズを変更します。

                サイズを変更するメモリブロックは、ヒープの空き領域の前方から確保された
                末尾のメモリブロックである必要があります。

  Arguments:    heap:      フレームヒープのハンドル。
                memBlock:  サイズを変更するメモリブロックへのポインタ。
                newSize:   新しく割り当てるサイズ(バイト単位)。
                           4未満の値を指定された場合は、4が指定されたものとして処理します。

  Returns:      関数が成功した場合、変更されたメモリブロックのサイズを返します(バイト単位)。
                関数が失敗した場合、0 が返ります。
 *---------------------------------------------------------------------------*/
u32
NNS_FndResizeForMBlockFrmHeap(
    NNSFndHeapHandle    heap,
    void*               memBlock,
    u32                 newSize
)
{
    NNSiFndHeapHead* pHeapHd = NULL;
    NNSiFndFrmHeapHead* pFrmHeapHd = NULL;

    NNS_ASSERT(IsValidFrmHeapHandle(heap));
    NNS_ASSERT(memBlock == NNSi_FndRoundDownPtr(memBlock, MIN_ALIGNMENT));  // 最低限、最小アライメントの境界にあるかチェック

    pHeapHd = heap;
    pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(pHeapHd);

    NNS_ASSERT(
            ComparePtr(pHeapHd->heapStart, memBlock) <= 0
        &&  ComparePtr(pFrmHeapHd->headAllocator, memBlock) > 0);           // メモリブロックは前方に存在すること
    NNS_ASSERT(
            pFrmHeapHd->pState == NULL
        ||  ComparePtr(pFrmHeapHd->pState, memBlock) < 0);                  // 状態保存がメモリブロックの後方に無いこと

    /*
        newSizeを0することは認めないようにしている。
        0にしてしまうと、memBlockが指すメモリブロックが存在しなくなるため。
    */
    if (newSize == 0)
    {
        newSize = 1;
    }
    newSize = NNSi_FndRoundUp(newSize, MIN_ALIGNMENT);

    {
        const u32 oldSize = GetOffsetFromPtr(memBlock, pFrmHeapHd->headAllocator);
        void* endAddress = AddU32ToPtr(memBlock, newSize);

        if (newSize == oldSize)  // ブロックサイズ変更なしの場合
        {
            return newSize;
        }

        if (newSize > oldSize)  // 拡大するとき
        {
            if (ComparePtr(endAddress, pFrmHeapHd->tailAllocator) > 0)  // サイズが足りない場合
            {
                return 0;
            }

            FillAllocMemory(heap, pFrmHeapHd->headAllocator, newSize - oldSize);
        }
        else                    // 縮小するとき
        {
            FillFreeMemory(heap, endAddress, oldSize - newSize);
        }

        pFrmHeapHd->headAllocator = endAddress;

        return newSize;
    }
}
