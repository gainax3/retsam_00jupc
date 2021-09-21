/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CARD - libraries
  File:     card_rom_teg.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/

#if defined(SDK_TEG)
/*****************************************************************************/
/* code for TEG only */


#include <nitro.h>

#include "../include/card_spi.h"
#include "../include/card_common.h"


/*****************************************************************************/
/* funcion */

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadCartridge

  Description:  設定された内容に基づいてカートリッジアクセスを実行.

  Arguments:    p          アクセス内容が設定された構造体

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_ReadCartridge(CARDRomStat * p)
{
    CARDiCommon *const c = &cardi_common;
    /* 32bit DMA 使用可能かの静的判定 */
    const BOOL is_dma_static =
#if defined(SDK_ARM9)
        !CARDi_IsTcm(c->dst, c->len) &&
#endif
        (c->dma <= MI_DMA_MAX_NUM) && (((c->src ^ c->dst) & 3) == 0);
    /* ページ単位で直接転送の繰り返し */
    const u8 *src = (const u8 *)(c->src + HW_CTRDG_ROM);
    u8     *dst = (u8 *)c->dst;
    u32     len = c->len;

    (void)p;

    while (len > 0)
    {
        u32     size = (u32)(CARD_ROM_PAGE_SIZE - ((u32)dst & 31));
        if (size >= len)
            size = len;
        if (is_dma_static && !(((u32)dst | size) & 31))
        {
#if defined(SDK_ARM9)
            DC_InvalidateRange(dst, size);
            IC_InvalidateRange(dst, size);
#endif
            MI_DmaCopy32(c->dma, src, dst, size);
        }
        else
        {
            MI_CpuCopy8(src, dst, size);
        }
        src += size;
        dst += size;
        len -= size;
    }
}


#if defined(SDK_ARM9)

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadPxi

  Description:  設定された内容に基づいてPXI による CARD リード要求を実行.

  Arguments:    p          アクセス内容が設定された構造体

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_ReadPxi(CARDRomStat * p)
{
    CARDiCommon *const c = &cardi_common;
    do
    {
        /* ページ単位で PXI リード要求 */
        p->cache_page = (u8 *)CARD_ALIGN_HI_BIT(c->src);
        DC_InvalidateRange(p->cache_buf, sizeof(p->cache_buf));
        c->cmd->src = (u32)p->cache_page;
        c->cmd->dst = (u32)p->cache_buf;
        c->cmd->len = sizeof(p->cache_buf);

        if (!CARDi_Request(c, CARD_REQ_READ_ROM, 1))
            break;
        {
            /*
             * キャッシュからコピー.
             * (この部分は最長 85[us] かかって重いので
             *  割り込み駆動にするとアプリが不安定になりうる)
             */
            const u32 mod = (u32)(c->src - (u32)p->cache_page);
            u32     len = CARD_ROM_PAGE_SIZE - mod;
            if (len > c->len)
                len = c->len;
            MI_CpuCopy8(p->cache_buf + mod, (void *)c->dst, len);
            c->src += len;
            c->dst += len;
            c->len -= len;
        }
    }
    while (c->len > 0);
}

#endif /* SDK_ARM9 */

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadPxi

  Description:  カード ID の読み出し.

  Arguments:    None.

  Returns:      読み出したカード ID.
 *---------------------------------------------------------------------------*/
u32 CARDi_ReadRomID(void)
{
    CARDRomStat *const p = &rom_stat;
    CARDiCommon *const c = &cardi_common;

    u32     ret = 0;

    SDK_ASSERT(CARD_IsAvailable());
    SDK_ASSERT(CARDi_GetTargetMode() == CARD_TARGET_ROM);

    /* TEG でカートリッジなら呼び出し不可能 */
    if (CARDi_IsTrueRom())
    {
        OS_TPanic("cannot call CARD_ReadRomID() on (TEG && cartridge)!");
    }

    CARDi_WaitTask(c, NULL, NULL);

#if defined(SDK_ARM9)
    /* TEG で ARM9 ならリクエストで解決 */
    cardi_common.cur_th = OS_GetCurrentThread();
    (void)CARDi_Request(c, CARD_REQ_READ_ID, 1);
    ret = (u32)c->cmd->id;
#endif
    CARDi_ReadEnd();
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_GetRomAccessor

  Description:  動作環境ごとに異なるカードアクセス関数へのポインタを返す.

  Arguments:    None.

  Returns:      TEG 版であるこの実装においては以下のいずれか.
                CARDi_ReadCard(), CARDi_ReadPxi(), CARDi_ReadCartridge().
 *---------------------------------------------------------------------------*/
void    (*CARDi_GetRomAccessor(void)) (CARDRomStat *)
{
    /* TEG で CARD なら ARM7 からのみアクセス可能 */
    if (CARDi_IsTrueRom())
    {
#if defined(SDK_ARM9)
        /* よって ARM9 からは PXI 経由 */
        return CARDi_ReadPxi;
#else
        /* よって ARM7 からは直接アクセス */
        return CARDi_ReadCard;
#endif /* SDK_ARM9 */
    }
    else
    {
        return CARDi_ReadCartridge;
    }
}


/*****************************************************************************/
#endif /* defined(SDK_TEG) */


/*---------------------------------------------------------------------------*
  $Log: card_rom_teg.c,v $
  Revision 1.2  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.1  2005/05/12 11:19:26  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
