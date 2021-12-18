/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI - libraries
  File:     mi_cache.c

  Copyright 2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/


#include <nitro/math.h>
#include <nitro/mi/memory.h>
#include <nitro/mi/cache.h>


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         MI_InitCache

  Description:  メモリキャッシュを初期化.

  Arguments:    cache            初期化するMICache構造体.
                page             1ページあたりのバッファサイズ.
                                 4以上で2のべき乗である必要がある.
                buffer           ページ管理情報に使用するバッファ.
                length           bufferのサイズ.
                                 length / (sizeof(MICachePage) + page)
                                 の数だけのページリストに分割される.
                                 各ページ(N=0,1,...)のバッファ先頭アドレスは
                                 (buffer + N * page) となることが保証される.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MI_InitCache(MICache *cache, u32 page, void *buffer, u32 length)
{
    /* 現在の実装の都合上, ワードサイズより小さなページは非対応 */
    SDK_ASSERT(page >= sizeof(u32));
    /* メンバ初期化 */
    cache->pagewidth = MATH_CTZ(page);
    cache->valid_total = 0;
    cache->invalid_total = 0;
    cache->loading_total = 0;
    cache->valid = NULL;
    cache->invalid = NULL;
    cache->loading = NULL;
    /* ページ分割 */
    {
        u32             total = length / (sizeof(MICachePage) + page);
        u8             *buf = (u8*)buffer;
        MICachePage   *inf = (MICachePage*)&buf[total * page];
        cache->invalid_total += total;
        for (; total > 0; --total)
        {
            inf->offset = 0;
            inf->buffer = buf;
            inf->next = cache->invalid;
            cache->invalid = inf;
            inf += 1;
            buf += page;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WFSi_TouchCachePages

  Description:  指定したページ範囲のロード準備を要求する.
                無効リストが空であれば有効リストを終端から破棄する.

  Arguments:    cache            MICache構造体.
                head             ロード対象の先頭ページ番号.
                bitset           ロード対象ページのビットセット.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WFSi_TouchCachePages(MICache *cache, u32 head, u32 bitset)
{
    {
        PLATFORM_ENTER_CRITICALSECTION();
        MICachePage  **load;
        /* 要求リストを検索して今回との重複分があれば除外 */
        for (load = &cache->loading; *load; load = &(*load)->next)
        {
            MICachePage *p = *load;
            u32     pos = p->offset - head;
            if ((pos < 32) && ((bitset & (1 << pos)) != 0))
            {
                bitset &= ~(1UL << pos);
            }
        }
        /* 無効リストが不足する場合, 有効リストを終端から破棄 */
        {
            int     rest = MATH_CountPopulation(bitset) - cache->invalid_total;
            if (rest > 0)
            {
                int             del = cache->valid_total;
                MICachePage  **valid;
                for (valid = &cache->valid; *valid; valid = &(*valid)->next)
                {
                    if (--del < rest)
                    {
                        MICachePage **pp;
                        for (pp = &cache->invalid; *pp; pp = &(*pp)->next)
                        {
                        }
                        *pp = *valid;
                        *valid = NULL;
                        cache->valid_total -= rest;
                        cache->invalid_total += rest;
                        break;
                    }
                }
            }
        }
        /* 無効リストの先頭から要求リストの終端へ移動 */
        while (cache->invalid && bitset)
        {
            MICachePage *p = cache->invalid;
            u32     pos = MATH_CTZ(bitset);
            cache->invalid = p->next;
            p->offset = head + pos;
            p->next = NULL;
            *load = p;
    	    load = &p->next;
            --cache->invalid_total;
            --cache->loading_total;
            bitset &= ~(1UL << pos);
        }
        PLATFORM_LEAVE_CRITICALSECTION();
    }
    /* ページ数が絶対的に不足する場合は使用方法が不正なので1回だけ警告 */
    if (bitset)
    {
        static BOOL output_once = FALSE;
        if (!output_once)
        {
            output_once = TRUE;
            OS_TWarning("not enough cache-page! "
                        "MI_TouchCache() failed. "
                        "(total pages = %d, but need more %d)",
                        cache->invalid_total + cache->valid_total,
                        MATH_CountPopulation(bitset));
        }
    }
}


/*---------------------------------------------------------------------------*
  Name:         MI_ReadCache

  Description:  キャッシュからデータを読み出し.
                ヒットしたページは有効リストの先頭に移動する.

  Arguments:    cache            MICache構造体.
                buffer           転送先メモリ.
                                 NULLを指定した場合は, データを読み出さず
                                 単に該当範囲全体のキャッシュ準備のみ要求する.
                offset           転送元オフセット.
                length           転送サイズ.

  Returns:      全領域がキャッシュにヒットすればTRUE.
 *---------------------------------------------------------------------------*/
BOOL MI_ReadCache(MICache *cache, void *buffer, u32 offset, u32 length)
{
    BOOL    retval = TRUE;

    /* 32ページ単位で先頭から順に判定 */
    const u32   unit = (1UL << cache->pagewidth);
    u32     head = (offset >> cache->pagewidth);
    u32     tail = ((offset + length + unit - 1UL) >> cache->pagewidth);
    u32     pages;
    for (; (pages = MATH_MIN(tail - head, 32UL)), (pages > 0); head += pages)
    {
        /* 有効リストから該当ページを検索 */
        u32     bitset = (1UL << pages) - 1UL;
        {
            PLATFORM_ENTER_CRITICALSECTION();
            MICachePage **pp;
            for (pp = &cache->valid; *pp && bitset; pp = &(*pp)->next)
            {
                MICachePage *p = *pp;
                u32     pos = p->offset - head;
                if ((pos < pages) && ((bitset & (1UL << pos)) != 0))
                {
                    if (buffer)
                    {
                        /*
                         * TODO:
                         *     冗長なリスト検索を回避しつつ有効な寿命も保てるので
                         *     ヒットしたページを有効リストの先頭へ移動したほうが
                         *     パフォーマンスはある程度向上するかもしれない.
                         *     (変更の際はこの for ループを考慮のこと!)
                         */
                        u32     len = unit;
                        int     src = 0;
                        int     dst = (int)((p->offset << cache->pagewidth) - offset);
                        if (dst < 0)
                        {
                            len += dst;
                            src -= dst;
                            dst = 0;
                        }
                        if (dst + len > length)
                        {
                            len = length - dst;
                        }
                        MI_CpuCopy8(&p->buffer[src], &((u8*)buffer)[dst], len);
                    }
                    bitset &= ~(1UL << pos);
                }
            }
            PLATFORM_LEAVE_CRITICALSECTION();
        }
        /* ページフォルトがあった場合 */
        if (bitset)
        {
            retval = FALSE;
            WFSi_TouchCachePages(cache, head, bitset);
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         MI_LoadCache

  Description:  ロード要求リストに存在する全ページのロード処理を実行.
                ロード要求リストが空であった場合は何もせずただちに制御を返し,
                呼び出し中にロード要求リストへ追加された場合はそれも処理する.

  Note:         この関数はデバイスがブロッキングしてもよいコンテキストから
                適切なタイミングで呼び出す必要がある.
                すなわち、割り込みハンドラなどから呼び出してはならない.

  Arguments:    cache            MICache構造体.
                device           ロード対象となるデバイス.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    MI_LoadCache(MICache *cache, MIDevice *device)
{
    for (;;)
    {
        /* ロード要求リストの先頭を取得 */
        MICachePage *p = cache->loading;
        if (!p)
        {
            break;
        }
        /* デバイスからページを読み込み */
        (void)MI_ReadDevice(device, p->buffer,
                            (p->offset << cache->pagewidth),
                            (1UL << cache->pagewidth));
        /* ロード完了したページを有効リストの先頭へ挿入 */
        {
            PLATFORM_ENTER_CRITICALSECTION();
            cache->loading = p->next;
            p->next = cache->valid;
            cache->valid = p;
            ++cache->valid_total;
            ++cache->loading_total;
            PLATFORM_LEAVE_CRITICALSECTION();
        }
    }
}


/*---------------------------------------------------------------------------*
  $Log: mi_cache.c,v $
  Revision 1.1  2007/04/11 08:06:25  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
