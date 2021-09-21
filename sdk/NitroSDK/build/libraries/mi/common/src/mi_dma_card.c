/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI
  File:     mi_dma_card.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mi_dma_card.c,v $
  Revision 1.6  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.5  2005/02/28 05:26:25  yosizaki
  do-indent.

  Revision 1.4  2005/01/12 11:10:15  takano_makoto
  fix copyright header.

  Revision 1.3  2005/01/12 02:47:53  yada
  consider about multi-DMA

  Revision 1.2  2004/10/12 02:03:46  yosizaki
  change ~0 to MIi_DMA_TIMING_ANY.

  Revision 1.1  2004/10/11 23:49:01  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include "../include/mi_dma.h"


/*---------------------------------------------------------------------------*
  Name:         MIi_CardDmaCopy32

  Description:  CARD DMA copy.
                32bit, sync version.
                (this sets only DMA-control. CARD register must be set after)

  Arguments:    dmaNo   : DMA channel No.
                src     : source address
                dest    : destination address
                size    : transfer size (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
void MIi_CardDmaCopy32(u32 dmaNo, const void *src, void *dest, u32 size)
{
    vu32   *dmaCntp;

    MIi_ASSERT_DMANO(dmaNo);
    MIi_ASSERT_DEST_ALIGN4(dest);
    /* サイズはこの関数内では使用しないが判定のために指定 */
    MIi_ASSERT_SRC_ALIGN512(size);
    MIi_WARNING_ADDRINTCM(dest, size);
    (void)size;

#ifdef SDK_ARM9
    /*
     * CARD は他の DMA と違って「同種が並列に」ということが有りえない.
     * 呼び出し側である CARD ライブラリで一元管理はしているが,
     * せっかくのチェック機能なので同種も検出するよう MIi_DMA_TIMING_ANY を設定.
     */
    MIi_CheckAnotherAutoDMA(dmaNo, MIi_DMA_TIMING_ANY);
#endif
    //---- check DMA0 source address
    MIi_CheckDma0SourceAddress(dmaNo, (u32)src, size, MI_DMA_SRC_FIX);

    if (size == 0)
    {
        return;
    }

    MIi_Wait_BeforeDMA(dmaCntp, dmaNo);
    MIi_DmaSetParams(dmaNo, (u32)src, (u32)dest,
                     (u32)(MI_CNT_CARDRECV32(4) | MI_DMA_CONTINUOUS_ON));
    /*
     * ここでは自動起動が ON になっただけ.
     * CARD レジスタへコマンドを設定して初めて起動する.
     */
}
