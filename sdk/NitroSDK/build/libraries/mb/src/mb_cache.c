/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - libraries
  File:     mb_cache.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_cache.c,v $
  Revision 1.6  2006/01/18 02:11:29  kitase_hirotake
  do-indent

  Revision 1.5  2005/12/21 14:25:56  yosizaki
  fix about page-cache.

  Revision 1.4  2005/02/28 05:26:24  yosizaki
  do-indent.

  Revision 1.3  2005/02/18 11:56:03  yosizaki
  fix around hidden warnings.

  Revision 1.2  2004/11/11 11:51:13  yosizaki
  change argument of MBi_ReadFromCache. (const_cast)
  add member 'is_hit' to avoid endless cache-missing.

  Revision 1.1  2004/11/10 13:14:58  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>

#include "mb_cache.h"


/*---------------------------------------------------------------------------*
  Name:         MBi_InitCache

  Description:  キャッシュリストを初期化する.
                

  Arguments:    pl         キャッシュリスト

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_InitCache(MBiCacheList * pl)
{
    MI_CpuClear8(pl, sizeof(*pl));
}

/*---------------------------------------------------------------------------*
  Name:         MBi_AttachCacheBuffer

  Description:  キャッシュリストへバッファを割り当てる.
                
  Arguments:    pl         キャッシュリスト
                ptr        割り当てるバッファ
                src        ptr のソースアドレス
                len        ptr のバイトサイズ
                state      指定する初期状態
                           (MB_CACHE_STATE_READY か MB_CACHE_STATE_LOCKED)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_AttachCacheBuffer(MBiCacheList * pl, u32 src, u32 len, void *ptr, u32 state)
{
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    {
        /* 未登録のページを検索 */
        MBiCacheInfo *pi = pl->list;
        for (;; ++pi)
        {
            if (pi >= &pl->list[MB_CACHE_INFO_MAX])
            {
                OS_TPanic("MBi_AttachCacheBuffer() failed! (over maximum count)");
            }
            if (pi->state == MB_CACHE_STATE_EMPTY)
            {
                pi->src = src;
                pi->len = len;
                pi->ptr = (u8 *)ptr;
                pi->state = state;
                break;
            }
        }
    }
    (void)OS_RestoreInterrupts(bak_cpsr);
}

/*---------------------------------------------------------------------------*
  Name:         MBi_ReadFromCache

  Description:  指定したアドレスの内容をキャッシュからリードする.

  Arguments:    pl         キャッシュリスト
                src        リード元アドレス
                len        リードサイズ (BYTE)
                dst        リード先アドレス.

  Returns:      キャッシュにヒットすればリードして TRUE, そうでなければ FALSE.
 *---------------------------------------------------------------------------*/
BOOL MBi_ReadFromCache(MBiCacheList * pl, u32 src, void *dst, u32 len)
{
    BOOL    ret = FALSE;
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    {
        /* 使用可能なページのみを検索 */
        const MBiCacheInfo *pi = pl->list;
        for (; pi < &pl->list[MB_CACHE_INFO_MAX]; ++pi)
        {
            if (pi->state >= MB_CACHE_STATE_READY)
            {
                /* 対象アドレスが範囲内か判定 */
                const int ofs = (int)(src - pi->src);
                if ((ofs >= 0) && (ofs + len <= pi->len))
                {
                    /* キャッシュヒットしたのでリード */
                    MI_CpuCopy8(pi->ptr + ofs, dst, len);
                    pl->lifetime = 0;
                    ret = TRUE;
                    break;
                }
            }
        }
    }
    (void)OS_RestoreInterrupts(bak_cpsr);
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         MBi_TryLoadCache

  Description:  指定したアドレスの内容をキャッシュへロードする.
                最も番地の小さい READY のページキャッシュが破棄される.

  Arguments:    pl         キャッシュリスト
                src        リロード元アドレス
                len        リロードサイズ (BYTE)

  Returns:      リロードが開始できれば TRUE, そうでなければ FALSE.
                (リロード処理のエンジンがシステム内で単一の場合,
                 前回の処理を未了ならこの関数は FALSE を返すであろう)
 *---------------------------------------------------------------------------*/
BOOL MBi_TryLoadCache(MBiCacheList * pl, u32 src, u32 len)
{
    BOOL    ret = FALSE;
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    {
        /* リロード可能なページのみを検索 */
        MBiCacheInfo *trg = NULL;
        MBiCacheInfo *pi = pl->list;
        for (; pi < &pl->list[MB_CACHE_INFO_MAX]; ++pi)
        {
            if (pi->state == MB_CACHE_STATE_READY)
            {
                /* 最も番地が小さければ候補に残す */
                if (!trg || (trg->src > pi->src))
                {
                    trg = pi;
                }
            }
        }
        /* リロード対象のページが見つかれば処理要求 */
        if (trg)
        {
            /* ここにタスクスレッドへの処理要求を記述 */
            (void)src;
            (void)len;
        }
        OS_TPanic("reload-system is not yet!");
    }
    (void)OS_RestoreInterrupts(bak_cpsr);
    return ret;
}
