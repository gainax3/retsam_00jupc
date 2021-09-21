/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - fnd
  File:     heapcommon.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.12 $
 *---------------------------------------------------------------------------*/

#ifndef NNS_FND_HEAPCOMMON_H_
#define NNS_FND_HEAPCOMMON_H_

#include <nitro/types.h>
#include <nnsys/fnd/list.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================
    マクロ定数
   ======================================================================== */

// 無効なヒープハンドル
#define NNS_FND_HEAP_INVALID_HANDLE     NULL

// ヒープからメモリを割り当てるときのデフォルトのアライメントサイズ
#define NNS_FND_HEAP_DEFAULT_ALIGNMENT    4

// 拡張ヒープのシグネチャ
#define NNSI_EXPHEAP_SIGNATURE    ('EXPH')

// フレームヒープのシグネチャ
#define NNSI_FRMHEAP_SIGNATURE    ('FRMH')

// ユニットヒープのシグネチャ
#define NNSI_UNTHEAP_SIGNATURE    ('UNTH')


/* ------------------------------------------------------------------------
    フィル関係
   ------------------------------------------------------------------------ */

// メモリ確保時にメモリを0でクリア
#define NNS_FND_HEAP_OPT_0_CLEAR        (1 <<0)

// ヒープ作成時・メモリ確保・解放時にメモリ充填
#define NNS_FND_HEAP_OPT_DEBUG_FILL     (1 <<1)


/* ------------------------------------------------------------------------
    ヒープチェック関係
   ------------------------------------------------------------------------ */

//  このビットが立っているとエラー出力
#define NNS_FND_HEAP_ERROR_PRINT        (1 <<0)


/* ========================================================================
    enum定数
   ======================================================================== */

enum {
    NNS_FND_HEAP_FILL_NOUSE,    // デバッグフィル未使用時
    NNS_FND_HEAP_FILL_ALLOC,    // デバッグフィル確保時
    NNS_FND_HEAP_FILL_FREE,     // デバッグフィル解放時

    NNS_FND_HEAP_FILL_MAX
};


/* =======================================================================
    型定義
   ======================================================================== */

typedef struct NNSiFndHeapHead NNSiFndHeapHead;

// ヒープ共通ヘッダ
struct NNSiFndHeapHead
{
    u32             signature;

    NNSFndLink      link;
    NNSFndList      childList;

    void*           heapStart;      // ヒープ先頭アドレス
    void*           heapEnd;        // ヒープ末尾(+1)アドレス

    u32             attribute;      // 属性
                                    // [8:オプションフラグ]
};

typedef NNSiFndHeapHead* NNSFndHeapHandle;   // ヒープのハンドルを表す型


/* ========================================================================
    マクロ関数
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetHeapStartAddress

  Description:  ヒープが利用するメモリ領域の開始アドレスを取得します。

  Arguments:    heap:  ヒープハンドル。

  Returns:      ヒープが利用するメモリ領域の開始アドレスを返します。
 *---------------------------------------------------------------------------*/
#define             NNS_FndGetHeapStartAddress(heap) \
                        ((void*)(heap))

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetHeapEndAddress

  Description:  ヒープが利用するメモリ領域の終了アドレス +1 を取得します。

  Arguments:    heap:  ヒープハンドル。

  Returns:      ヒープが利用するメモリ領域の終了アドレス +1 を返します。
 *---------------------------------------------------------------------------*/
#define             NNS_FndGetHeapEndAddress(heap) \
                        (((NNSiFndHeapHead*)(heap))->heapEnd)


/* =======================================================================
    関数プロトタイプ
   ======================================================================== */

NNSFndHeapHandle    NNS_FndFindContainHeap(
                        const void* memBlock);

#if defined(NNS_FINALROM)
    #define             NNS_FndDumpHeap(heap)				((void)0)
#else
	void                NNS_FndDumpHeap(
	                        NNSFndHeapHandle heap);
#endif

#if defined(NNS_FINALROM)
    #define             NNS_FndSetFillValForHeap(type, val) (0)
#else
    u32                 NNS_FndSetFillValForHeap(
                            int     type,
                            u32     val);
#endif

#if defined(NNS_FINALROM)
    #define             NNS_FndGetFillValForHeap(type)      (0)
#else
    u32                 NNS_FndGetFillValForHeap(
                            int     type);
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NNS_FND_HEAPCOMMON_H_ */
#endif
