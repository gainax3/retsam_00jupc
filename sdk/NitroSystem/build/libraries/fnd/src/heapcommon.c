/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - fnd
  File:     heapcommon.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.15 $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include <nnsys/misc.h>
#include <nnsys/fnd/heapcommon.h>
#include <nnsys/fnd/expheap.h>
#include <nnsys/fnd/frameheap.h>
#include <nnsys/fnd/unitheap.h>
#include <nnsys/fnd/config.h>
#include "heapcommoni.h"


/* ========================================================================
    static変数
   ======================================================================== */

/* ------------------------------------------------------------------------
    リスト関連
   ------------------------------------------------------------------------ */

static NNSFndList sRootList;                // ルートのヒープリスト
static BOOL sRootListInitialized = FALSE;   // sRootListが初期化されていれば真


/* ------------------------------------------------------------------------
    フィル関連
   ------------------------------------------------------------------------ */

#if ! defined(NNS_FINALROM)

    static u32 sFillVals[NNS_FND_HEAP_FILL_MAX] =
    {
        0xC3C3C3C3, // ヒープ作成時に埋める値
        0xF3F3F3F3, // メモリブロック確保時に埋める値
        0xD3D3D3D3, // メモリブロック解放時に埋める値
    };

// #if ! defined(NNS_FINALROM)
#endif


/* ========================================================================
    static関数
   ======================================================================== */

/* ------------------------------------------------------------------------
    リスト関連
   ------------------------------------------------------------------------ */

/*---------------------------------------------------------------------------*
  Name:         FindContainHeap

  Description:  指定されたメモリブロックを含有するヒープをリストから再帰的に
                探し出します。

  Arguments:    pList:     リストへのポインタ
                memBlock:  メモリブロックへのポインタ

  Returns:      指定されたメモリブロックを確保したヒープが見つかれば、
                そのヒープへのポインタを返します。
                見つからなかった時は NULL を返します。
 *---------------------------------------------------------------------------*/
static NNSiFndHeapHead*
FindContainHeap(
    NNSFndList* pList,
    const void* memBlock
)
{
    NNSiFndHeapHead* pHeapHd = NULL;
    while (NULL != (pHeapHd = NNS_FndGetNextListObject(pList, pHeapHd)))
    {
        if ( NNSiGetUIntPtr(pHeapHd->heapStart) <= NNSiGetUIntPtr(memBlock)
         &&  NNSiGetUIntPtr(memBlock) < NNSiGetUIntPtr(pHeapHd->heapEnd)
        )
        {
            NNSiFndHeapHead* pChildHeapHd = FindContainHeap(&pHeapHd->childList, memBlock);
            if(pChildHeapHd)
            {
                return pChildHeapHd;
            }

            return pHeapHd;
        }
    }

    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         FindListContainHeap

  Description:  ヒープを含有する親ヒープを検索し、その親ヒープのリストへの
                ポインタを返します。

  Arguments:    pHeapHd:  検索対象のヒープのヘッダへのポインタ。

  Returns:      指定したヒープを含有する親ヒープが見つかれば、
                親ヒープの子リストへのポインタを返します。
                親ヒープが見つからなければルートリストへのポインタが返ります。
 *---------------------------------------------------------------------------*/
static NNSFndList*
FindListContainHeap(NNSiFndHeapHead* pHeapHd)
{
    NNSFndList* pList = &sRootList;

    NNSiFndHeapHead* pContainHeap = FindContainHeap(&sRootList, pHeapHd);

    if(pContainHeap)
    {
        pList = &pContainHeap->childList;
    }

    return pList;
}

#if 1
    static NNS_FND_INLINE void
    DumpHeapList() {}
#else
    static void
    DumpHeapList()
    {
        NNSiFndHeapHead* pHeapHd = NULL;
        int count = 0;

        OS_Printf("Dump Heap List\n");
        while (NULL != (pHeapHd = NNS_FndGetNextListObject(&sRootList, pHeapHd)))
        {
            count++;
            OS_Printf("[%d] -> %p %08X\n", count, pHeapHd, pHeapHd->signature);
        }
    }
#endif

/* ========================================================================
    外部関数(非公開)
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNSi_FndInitHeapHead

  Description:  ヒープヘッダの初期化を行います。

  Arguments:    pHeapHd:    ヒープヘッダへのポインタ。
                signature:  シグネチャ。
                heapStart:  ヒープメモリの開始アドレス。
                heapEnd:    ヒープメモリの終了アドレス +1。
                optFlag:    ヒープオプション。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void
NNSi_FndInitHeapHead(
    NNSiFndHeapHead*    pHeapHd,
    u32                 signature,
    void*               heapStart,
    void*               heapEnd,
    u16                 optFlag
)
{
    pHeapHd->signature = signature;

    pHeapHd->heapStart = heapStart;
    pHeapHd->heapEnd   = heapEnd;

    pHeapHd->attribute = 0;
    SetOptForHeap(pHeapHd, optFlag);

    FillNoUseMemory(
        pHeapHd,
        heapStart,
        GetOffsetFromPtr(heapStart, heapEnd));

    NNS_FND_INIT_LIST(&pHeapHd->childList, NNSiFndHeapHead, link);

    // ヒープのリスト操作
    if(! sRootListInitialized)
    {
        NNS_FND_INIT_LIST(&sRootList, NNSiFndHeapHead, link);
        sRootListInitialized = TRUE;
    }

    NNS_FndAppendListObject(FindListContainHeap(pHeapHd), pHeapHd);
    DumpHeapList();

}

/*---------------------------------------------------------------------------*
  Name:         NNSi_FndFinalizeHeap

  Description:  ヒープ共通の後始末を行います。

  Arguments:    pHeapHd:  ヒープヘッダへのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void
NNSi_FndFinalizeHeap(NNSiFndHeapHead* pHeapHd)
{
    // ヒープのリスト操作
    NNS_FndRemoveListObject(FindListContainHeap(pHeapHd), pHeapHd);
    DumpHeapList();
}


/*---------------------------------------------------------------------------*
  Name:         NNSi_FndDumpHeapHead

  Description:  ヒープヘッダの情報を表示します。

  Arguments:    pHeapHd:  ヒープヘッダへのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void
NNSi_FndDumpHeapHead(NNSiFndHeapHead* pHeapHd)
{
    OS_Printf("[NNS Foundation ");

    switch(pHeapHd->signature)
    {
    case NNSI_EXPHEAP_SIGNATURE: OS_Printf("Exp");   break;
    case NNSI_FRMHEAP_SIGNATURE: OS_Printf("Frame"); break;
    case NNSI_UNTHEAP_SIGNATURE: OS_Printf("Unit");  break;
    default:
        NNS_ASSERT(FALSE);
    }

    OS_Printf(" Heap]\n");

    OS_Printf("    whole [%p - %p)\n", pHeapHd, pHeapHd->heapEnd);
}


/* ========================================================================
    外部関数(公開)
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNS_FndFindContainHeap

  Description:  メモリブロックを含有するヒープを検索します。

  Arguments:    memBlock:  検索対象のメモリブロック。

  Returns:      指定したメモリブロックを含むヒープが見つかれば、
                そのヒープのハンドルを返します。
                見つからなければ、NNS_FND_HEAP_INVALID_HANDLE が返ります。
 *---------------------------------------------------------------------------*/
NNSFndHeapHandle
NNS_FndFindContainHeap(const void* memBlock)
{
    return FindContainHeap(&sRootList, memBlock);
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndDumpHeap

  Description:  ヒープ内部の情報を表示します。
                これはデバッグ用の関数です。

  Arguments:    heap:    フレームヒープのハンドル。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
#if ! defined(NNS_FINALROM)

    void
    NNS_FndDumpHeap(NNSFndHeapHandle heap)
    {
        NNSiFndHeapHead* pHeapHd = heap;
        switch(pHeapHd->signature)
        {
        case NNSI_EXPHEAP_SIGNATURE: NNSi_FndDumpExpHeap(heap);  break;
        case NNSI_FRMHEAP_SIGNATURE: NNSi_FndDumpFrmHeap(heap);  break;
        case NNSI_UNTHEAP_SIGNATURE: NNSi_FndDumpUnitHeap(heap); break;
        default:
            OS_Printf("[NNS Foundation] dump heap : unknown heap. - %p\n", heap);
        }
    }

// #if ! defined(NNS_FINALROM)
#endif

/*---------------------------------------------------------------------------*
  Name:         NNS_FndSetFillValForHeap

  Description:  ヒープの作成時やメモリブロックの確保・解放時にメモリに
                セットする値をセットします。
                この関数はデバッグ用の関数です。
                最終ROM版(FINALROM)ライブラリでは常に0を返します。

  Arguments:    type:  取得する値の種類
                val:   セットする値

  Returns:      以前の、メモリブロックの確保時にメモリにセットする値を返します。
 *---------------------------------------------------------------------------*/

#if ! defined(NNS_FINALROM)

    u32
    NNS_FndSetFillValForHeap(
        int     type,
        u32     val
    )
    {
        NNS_ASSERT(type < NNS_FND_HEAP_FILL_MAX);

        {
            u32 oldVal = sFillVals[type];
            sFillVals[type] = val;
            return oldVal;
        }
    }

// #if ! defined(NNS_FINALROM)
#endif


/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetFillValForHeap

  Description:  ヒープの作成時やメモリブロックの確保・解放時にメモリに
                セットする値を取得します。
                この関数はデバッグ用の関数です。
                最終ROM版(FINALROM)ライブラリでは常に0を返します。

  Arguments:    type:  取得する値の種類

  Returns:      指定された種類のメモリにセットする値を返します。
 *---------------------------------------------------------------------------*/

#if ! defined(NNS_FINALROM)

    u32
    NNS_FndGetFillValForHeap(int type)
    {
        NNS_ASSERT(type < NNS_FND_HEAP_FILL_MAX);

        return sFillVals[type];
    }

// #if ! defined(NNS_FINALROM)
#endif
