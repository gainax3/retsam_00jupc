/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI
  File:     mi_dma_vblank.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mi_dma_vblank.c,v $
  Revision 1.8  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.7  2005/02/28 05:26:25  yosizaki
  do-indent.

  Revision 1.6  2005/01/12 11:10:15  takano_makoto
  fix copyright header.

  Revision 1.5  2005/01/12 02:47:53  yada
  consider about multi-DMA

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

#include <nitro.h>
#include "../include/mi_dma.h"

/*---------------------------------------------------------------------------*
  Name:         MI_VBlankDmaCopy32

  Description:  VBlank DMA copy.
                32bit, sync version.

  Arguments:    dmaNo   : DMA channel No.
                src     : source address
                dest    : destination address
                size    : transfer size (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
void MI_VBlankDmaCopy32(u32 dmaNo, const void *src, void *dest, u32 size)
{
    vu32   *dmaCntp;

    MIi_ASSERT_DMANO(dmaNo);
    MIi_ASSERT_SRC_ALIGN4(src);
    MIi_ASSERT_DEST_ALIGN4(dest);
    MIi_WARNING_ADDRINTCM(src, size);
    MIi_WARNING_ADDRINTCM(dest, size);

#ifdef SDK_ARM9
    MIi_CheckAnotherAutoDMA(dmaNo, MI_DMA_TIMING_V_BLANK);
#endif
    //---- check DMA0 source address
    MIi_CheckDma0SourceAddress(dmaNo, (u32)src, size, MI_DMA_SRC_INC);

    if (size == 0)
    {
        return;
    }

    MIi_Wait_BeforeDMA(dmaCntp, dmaNo);
    MIi_DmaSetParams(dmaNo, (u32)src, (u32)dest, MI_CNT_VBCOPY32(size));
    MIi_Wait_AfterDMA(dmaCntp);
}

/*---------------------------------------------------------------------------*
  Name:         MI_VBlankDmaCopy16

  Description:  VBlank DMA copy.
                16bit, sync version.

  Arguments:    dmaNo   : DMA channel No.
                src     : source address
                dest    : destination address
                size    : transfer size (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
void MI_VBlankDmaCopy16(u32 dmaNo, const void *src, void *dest, u32 size)
{
    vu32   *dmaCntp;

    MIi_ASSERT_DMANO(dmaNo);
    MIi_ASSERT_SRC_ALIGN2(src);
    MIi_ASSERT_DEST_ALIGN2(dest);
    MIi_WARNING_ADDRINTCM(src, size);
    MIi_WARNING_ADDRINTCM(dest, size);

#ifdef SDK_ARM9
    MIi_CheckAnotherAutoDMA(dmaNo, MI_DMA_TIMING_V_BLANK);
#endif
    //---- check DMA0 source address
    MIi_CheckDma0SourceAddress(dmaNo, (u32)src, size, MI_DMA_SRC_INC);

    if (size == 0)
    {
        return;
    }

    MIi_Wait_BeforeDMA(dmaCntp, dmaNo);
    MIi_DmaSetParams_wait(dmaNo, (u32)src, (u32)dest, MI_CNT_VBCOPY16(size));
    MIi_Wait_AfterDMA(dmaCntp);
}

/*---------------------------------------------------------------------------*
  Name:         MI_VBlankDmaCopy32Async

  Description:  VBlank DMA copy.
                32bit, async version

  Arguments:    dmaNo   : DMA channel No.
                src     : source address
                dest    : destination address
                size    : transfer size (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
void MI_VBlankDmaCopy32Async(u32 dmaNo, const void *src, void *dest, u32 size,
                             MIDmaCallback callback, void *arg)
{
    MIi_ASSERT_DMANO(dmaNo);
    MIi_ASSERT_SRC_ALIGN4(src);
    MIi_ASSERT_DEST_ALIGN4(dest);
    MIi_WARNING_ADDRINTCM(src, size);
    MIi_WARNING_ADDRINTCM(dest, size);

#ifdef SDK_ARM9
    MIi_CheckAnotherAutoDMA(dmaNo, MI_DMA_TIMING_V_BLANK);
#endif
    //---- check DMA0 source address
    MIi_CheckDma0SourceAddress(dmaNo, (u32)src, size, MI_DMA_SRC_INC);

    if (size == 0)
    {
        MIi_CallCallback(callback, arg);
    }
    else
    {
        MI_WaitDma(dmaNo);
        if (callback)
        {
            OSi_EnterDmaCallback(dmaNo, callback, arg);
            MIi_DmaSetParams(dmaNo, (u32)src, (u32)dest, MI_CNT_VBCOPY32_IF(size));
        }
        else
        {
            MIi_DmaSetParams(dmaNo, (u32)src, (u32)dest, MI_CNT_VBCOPY32(size));
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         MI_VBlankDmaCopy16Async

  Description:  VBlank DMA copy.
                16bit, async version

  Arguments:    dmaNo   : DMA channel No.
                src     : source address
                dest    : destination address
                size    : transfer size (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
void MI_VBlankDmaCopy16Async(u32 dmaNo, const void *src, void *dest, u32 size,
                             MIDmaCallback callback, void *arg)
{
    MIi_ASSERT_DMANO(dmaNo);
    MIi_ASSERT_SRC_ALIGN2(src);
    MIi_ASSERT_DEST_ALIGN2(dest);
    MIi_WARNING_ADDRINTCM(src, size);
    MIi_WARNING_ADDRINTCM(dest, size);

#ifdef SDK_ARM9
    MIi_CheckAnotherAutoDMA(dmaNo, MI_DMA_TIMING_V_BLANK);
#endif
    //---- check DMA0 source address
    MIi_CheckDma0SourceAddress(dmaNo, (u32)src, size, MI_DMA_SRC_INC);

    if (size == 0)
    {
        MIi_CallCallback(callback, arg);
    }
    else
    {
        MI_WaitDma(dmaNo);
        if (callback)
        {
            OSi_EnterDmaCallback(dmaNo, callback, arg);
            MIi_DmaSetParams(dmaNo, (u32)src, (u32)dest, MI_CNT_VBCOPY16_IF(size));
        }
        else
        {
            MIi_DmaSetParams(dmaNo, (u32)src, (u32)dest, MI_CNT_VBCOPY16(size));
        }
    }
}
