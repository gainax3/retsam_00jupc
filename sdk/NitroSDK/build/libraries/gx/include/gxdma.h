/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - 
  File:     gxdma.h

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: gxdma.h,v $
  Revision 1.4  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.3  2005/02/28 05:26:13  yosizaki
  do-indent.

  Revision 1.2  2004/12/02 11:26:12  takano_makoto
  データサイズが小さい場合にCPUコピーするように修正

  Revision 1.1  2004/08/10 12:48:57  takano_makoto
  Initial Update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_GXDMA_H_
#define NITRO_GXDMA_H_

#include <nitro/gx/gx.h>
#include <nitro/mi.h>

#ifdef __cplusplus
extern "C" {
#endif

static void GXi_DmaCopy32(u32 dmaNo, const void *src, void *dest, u32 size);
static void GXi_DmaCopy16(u32 dmaNo, const void *src, void *dest, u32 size);
static void GXi_DmaCopy32Async(u32 dmaNo, const void *src, void *dest, u32 size,
                               MIDmaCallback callback, void *arg);
static void GXi_WaitDma(u32 dmaNo);

#define GX_CPU_FASTER32_SIZE        48
#define GX_CPU_FASTER16_SIZE        28

//---------------------------------------------------------------------------
//   inline functions.
//---------------------------------------------------------------------------

static inline void GXi_DmaCopy32(u32 dmaNo, const void *src, void *dest, u32 size)
{
    if (dmaNo != GX_DMA_NOT_USE && size > GX_CPU_FASTER32_SIZE)
    {
        MI_DmaCopy32(dmaNo, src, dest, size);
    }
    else
    {
        MI_CpuCopy32(src, dest, size);
    }
}

static inline void GXi_DmaCopy16(u32 dmaNo, const void *src, void *dest, u32 size)
{
    if (dmaNo != GX_DMA_NOT_USE && size > GX_CPU_FASTER16_SIZE)
    {
        MI_DmaCopy16(dmaNo, src, dest, size);
    }
    else
    {
        MI_CpuCopy16(src, dest, size);
    }
}

static inline void GXi_DmaCopy32Async(u32 dmaNo, const void *src, void *dest, u32 size,
                                      MIDmaCallback callback, void *arg)
{
    if (dmaNo != GX_DMA_NOT_USE)
    {
        MI_DmaCopy32Async(dmaNo, src, dest, size, callback, arg);
    }
    else
    {
        MI_CpuCopy32(src, dest, size);
    }
}

static inline void GXi_WaitDma(u32 dmaNo)
{
    if (dmaNo != GX_DMA_NOT_USE)
    {
        MI_WaitDma(dmaNo);
    }
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NITRO_GXDMA_H_
