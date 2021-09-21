/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI
  File:     mi_dma_hblank.c

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mi_dma_hblank.c,v $
  Revision 1.8  2005/02/28 05:26:25  yosizaki
  do-indent.

  Revision 1.7  2005/01/12 11:10:15  takano_makoto
  fix copyright header.

  Revision 1.6  2005/01/12 02:47:53  yada
  consider about multi-DMA

  Revision 1.5  2004/11/02 09:54:41  yada
  just fix comment

  Revision 1.4  2004/08/30 05:03:27  yada
  MIi_CNT_XXX -> MI_CNT_XXX

  Revision 1.3  2004/08/26 00:18:28  yada
  change multiple auto-DMA check method

  Revision 1.2  2004/08/25 11:56:18  yada
  add check for avoiding more than 2 auto_start_DMA run at the same time

  Revision 1.1  2004/08/25 00:49:32  yada
  initial release.
  separated from mi_dma.c

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifdef SDK_ARM9
#include <nitro.h>
#include "../include/mi_dma.h"

/*---------------------------------------------------------------------------*
  Name:         MI_HBlankDmaCopy32

  Description:  HBlank DMA copy.
                32bit.

  Arguments:    dmaNo   : DMA channel No.
                src     : source address
                dest    : destination address
                size    : transfer size (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
void MI_HBlankDmaCopy32(u32 dmaNo, const void *src, void *dest, u32 size)
{
    MIi_ASSERT_DMANO(dmaNo);
    MIi_ASSERT_SRC_ALIGN4(src);
    MIi_ASSERT_DEST_ALIGN4(dest);
    MIi_WARNING_ADDRINTCM(src, size);
    MIi_WARNING_ADDRINTCM(dest, size);

    MIi_CheckAnotherAutoDMA(dmaNo, MI_DMA_TIMING_H_BLANK);
    MIi_CheckDma0SourceAddress(dmaNo, (u32)src, size, MI_DMA_SRC_INC);
    if (size == 0)
    {
        return;
    }

    MI_WaitDma(dmaNo);
    MIi_DmaSetParams(dmaNo, (u32)src, (u32)dest, MI_CNT_HBCOPY32(size));
}

/*---------------------------------------------------------------------------*
  Name:         MI_HBlankDmaCopy16

  Description:  HBlank DMA copy.
                16bit.

  Arguments:    dmaNo   : DMA channel No.
                src     : source address
                dest    : destination address
                size    : transfer size (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
void MI_HBlankDmaCopy16(u32 dmaNo, const void *src, void *dest, u32 size)
{
    MIi_ASSERT_DMANO(dmaNo);
    MIi_ASSERT_SRC_ALIGN2(src);
    MIi_ASSERT_DEST_ALIGN2(dest);
    MIi_WARNING_ADDRINTCM(src, size);
    MIi_WARNING_ADDRINTCM(dest, size);

    MIi_CheckAnotherAutoDMA(dmaNo, MI_DMA_TIMING_H_BLANK);
    MIi_CheckDma0SourceAddress(dmaNo, (u32)src, size, MI_DMA_SRC_INC);
    if (size == 0)
    {
        return;
    }

    MI_WaitDma(dmaNo);
    MIi_DmaSetParams(dmaNo, (u32)src, (u32)dest, MI_CNT_HBCOPY16(size));
}

/*---------------------------------------------------------------------------*
  Name:         MI_HBlankDmaCopy32If

  Description:  HBlank DMA copy.
                32bit.
                occur interrupt when finish DMA.

  Arguments:    dmaNo   : DMA channel No.
                src     : source address
                dest    : destination address
                size    : transfer size (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
void MI_HBlankDmaCopy32If(u32 dmaNo, const void *src, void *dest, u32 size)
{
    MIi_ASSERT_DMANO(dmaNo);
    MIi_ASSERT_SRC_ALIGN4(src);
    MIi_ASSERT_DEST_ALIGN4(dest);
    MIi_WARNING_ADDRINTCM(src, size);
    MIi_WARNING_ADDRINTCM(dest, size);

    MIi_CheckAnotherAutoDMA(dmaNo, MI_DMA_TIMING_H_BLANK);
    MIi_CheckDma0SourceAddress(dmaNo, (u32)src, size, MI_DMA_SRC_INC);
    if (size == 0)
    {
        return;
    }

    MI_WaitDma(dmaNo);
    MIi_DmaSetParams(dmaNo, (u32)src, (u32)dest, MI_CNT_HBCOPY32_IF(size));
}

/*---------------------------------------------------------------------------*
  Name:         MI_HBlankDmaCopy16If

  Description:  HBlank DMA copy.
                16bit.
                occur interrupt when finish DMA.

  Arguments:    dmaNo   : DMA channel No.
                src     : source address
                dest    : destination address
                size    : transfer size (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
void MI_HBlankDmaCopy16If(u32 dmaNo, const void *src, void *dest, u32 size)
{
    MIi_ASSERT_DMANO(dmaNo);
    MIi_ASSERT_SRC_ALIGN2(src);
    MIi_ASSERT_DEST_ALIGN2(dest);
    MIi_WARNING_ADDRINTCM(src, size);
    MIi_WARNING_ADDRINTCM(dest, size);

    MIi_CheckAnotherAutoDMA(dmaNo, MI_DMA_TIMING_H_BLANK);
    MIi_CheckDma0SourceAddress(dmaNo, (u32)src, size, MI_DMA_SRC_INC);
    if (size == 0)
    {
        return;
    }

    MI_WaitDma(dmaNo);
    MIi_DmaSetParams(dmaNo, (u32)src, (u32)dest, MI_CNT_HBCOPY16_IF(size));
}
#endif // SDK_ARM9
