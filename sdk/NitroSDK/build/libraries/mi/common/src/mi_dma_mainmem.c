/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI
  File:     mi_dma_mainmem.c

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mi_dma_mainmem.c,v $
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

#ifdef SDK_ARM9
#include <nitro.h>
#include "../include/mi_dma.h"

/*---------------------------------------------------------------------------*
  Name:         MI_DispMemDmaCopy

  Description:  main memory display DMA.

  Arguments:    dmaNo   : DMA channel No.
                src     : source address

  Returns:      None
 *---------------------------------------------------------------------------*/
void MI_DispMemDmaCopy(u32 dmaNo, const void *src)
{
    MIi_ASSERT_DMANO(dmaNo);
    MIi_ASSERT_SRC_ALIGN4(src);
    MIi_WARNING_ADDRINTCM(src, 0x18000);        // 0x18000=256*192*2(byte)

    MIi_CheckAnotherAutoDMA(dmaNo, MI_DMA_TIMING_DISP_MMEM);
    //---- check DMA0 source address
    MIi_CheckDma0SourceAddress(dmaNo, (u32)src, 0x18000, MI_DMA_SRC_INC);

    MIi_DmaSetParams(dmaNo, (u32)src, (u32)REG_DISP_MMEM_FIFO_ADDR, MI_CNT_MMCOPY(0));
}
#endif // SDK_ARM9
