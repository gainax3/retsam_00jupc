/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI
  File:     mi_dma_gxcommand.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mi_dma_gxcommand.c,v $
  Revision 1.18  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.17  2005/02/28 05:26:25  yosizaki
  do-indent.

  Revision 1.16  2005/02/18 06:41:11  yasu
  OS_DisableInterrupts •Ô’l‚ÌŒ^C³

  Revision 1.15  2005/01/12 02:47:53  yada
  consider about multi-DMA

  Revision 1.14  2005/01/11 07:41:41  takano_makoto
  fix copyright header.

  Revision 1.13  2005/01/05 05:59:56  yada
  fix around volatile variable 'isBusy'

  Revision 1.12  2004/09/28 04:35:30  yada
  fix MI_SendGXCommand(), add MI_SendGXCommandFast()

  Revision 1.11  2004/09/27 00:33:24  yada
  change MIi_DMAFastCallback to set busy flag be clear at first

  Revision 1.10  2004/09/09 01:43:11  yada
  0x118 -> 118

  Revision 1.9  2004/09/04 06:54:59  yada
  fix a little

  Revision 1.8  2004/09/03 12:20:20  yada
  clear IF bit in MIi_FIFOCallback().
  Underhalf interrupt is level trigger, not edge trigger.

  Revision 1.7  2004/09/01 09:12:07  yada
  change MI_SendGXCommandAsync() callback

  Revision 1.6  2004/08/30 11:51:43  yada
  change forwaring words at one time

  Revision 1.5  2004/08/30 05:03:27  yada
  MIi_CNT_XXX -> MI_CNT_XXX

  Revision 1.4  2004/08/26 00:18:28  yada
  change multiple auto-DMA check method

  Revision 1.3  2004/08/25 11:56:18  yada
  add check for avoiding more than 2 auto_start_DMA run at the same time

  Revision 1.2  2004/08/25 06:43:43  yada
  fix MI_SendGXCommandAsync(). transfer when FIFO under half
  with interrupt handler, not use auto DMA start.

  Revision 1.1  2004/08/25 00:49:32  yada
  initial release.
  separated from mi_dma.c

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifdef SDK_ARM9
#include <nitro.h>
#include "../include/mi_dma.h"

//---- flagmental DMA size
#define MIi_GX_LENGTH_ONCE  (118 * sizeof(u32)) //(byte)

//---- fmagment DMA parameters
typedef struct
{
    volatile BOOL isBusy;

    u32     dmaNo;
    u32     src;
    u32     length;
    MIDmaCallback callback;
    void   *arg;

    GXFifoIntrCond fifoCond;
    void    (*fifoFunc) (void);
}
MIiGXDmaParams;

static MIiGXDmaParams MIi_GXDmaParams = { FALSE };

static void MIi_FIFOCallback(void);
static void MIi_DMACallback(void *);
static void MIi_DMAFastCallback(void *);

//================================================================================
//         not use auto-DMA
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         MI_SendGXCommand

  Description:  send GX command with geometry FIFO DMA.
                sync version.
                send data with small fragmental DMA, so DMA autostart may not occur.

  Arguments:    dmaNo          : DMA channel No.
                src            : source address
                commandLength  : GX command length. (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
#include <nitro/itcm_begin.h>
void MI_SendGXCommand(u32 dmaNo, const void *src, u32 commandLength)
{
    vu32   *dmaCntp;
    u32     leftLength = commandLength;
    u32     currentSrc = (u32)src;

    MIi_ASSERT_DMANO(dmaNo);
    MIi_ASSERT_SRC_ALIGN4(src);
    MIi_WARNING_ADDRINTCM(src, commandLength);

    if (leftLength == 0)
    {
        return;
    }

    //---- check DMA0 source address
    MIi_CheckDma0SourceAddress(dmaNo, (u32)src, commandLength, MI_DMA_SRC_INC);

    MIi_Wait_BeforeDMA(dmaCntp, dmaNo);
    while (leftLength > 0)
    {
        u32     length = (leftLength > MIi_GX_LENGTH_ONCE) ? MIi_GX_LENGTH_ONCE : leftLength;
        MIi_DmaSetParams(dmaNo, currentSrc, (u32)REG_GXFIFO_ADDR, MI_CNT_SEND32(length));
        leftLength -= length;
        currentSrc += length;
    }
    MIi_Wait_AfterDMA(dmaCntp);
}

#include <nitro/itcm_end.h>

/*---------------------------------------------------------------------------*
  Name:         MI_SendGXCommandAsync

  Description:  send GX command with geometry FIFO DMA.
                async version.
                send data with small fragmental DMA, so DMA autostart may not occur.

  Arguments:    dmaNo          : DMA channel No.
                src            : source address
                commandLength  : GX command length. (byte)
                callback       : callback function
                arg            : callback argument

  Returns:      None
 *---------------------------------------------------------------------------*/
void MI_SendGXCommandAsync(u32 dmaNo, const void *src, u32 commandLength, MIDmaCallback callback,
                           void *arg)
{
    MIi_ASSERT_DMANO(dmaNo);
    MIi_ASSERT_SRC_ALIGN4(src);
    MIi_WARNING_ADDRINTCM(src, commandLength);

    //---- if size==0, call callback immediately
    if (commandLength == 0)
    {
        MIi_CallCallback(callback, arg);
        return;
    }

    //---- wait till other task end
    while (MIi_GXDmaParams.isBusy /*volatile valiable */ )
    {
    }

    //---- wait till geometryEngine FIFO is underhalf
    while (!(G3X_GetCommandFifoStatus() & GX_FIFOSTAT_UNDERHALF))
    {
    }

    //---- fragmental DMA params
    MIi_GXDmaParams.isBusy = TRUE;
    MIi_GXDmaParams.dmaNo = dmaNo;
    MIi_GXDmaParams.src = (u32)src;
    MIi_GXDmaParams.length = commandLength;
    MIi_GXDmaParams.callback = callback;
    MIi_GXDmaParams.arg = arg;

    //---- check DMA0 source address
    MIi_CheckDma0SourceAddress(dmaNo, (u32)src, commandLength, MI_DMA_SRC_INC);

    MI_WaitDma(dmaNo);
    {
        OSIntrMode enabled = OS_DisableInterrupts();

        //---- remember FIFO interrupt setting
        MIi_GXDmaParams.fifoCond =
            (GXFifoIntrCond)((reg_G3X_GXSTAT & REG_G3X_GXSTAT_FI_MASK) >> REG_G3X_GXSTAT_FI_SHIFT);
        MIi_GXDmaParams.fifoFunc = OS_GetIrqFunction(OS_IE_GXFIFO);

        //---- set FIFO interrupt
        G3X_SetFifoIntrCond(GX_FIFOINTR_COND_UNDERHALF);
        OS_SetIrqFunction(OS_IE_GXFIFO, MIi_FIFOCallback);
        (void)OS_EnableIrqMask(OS_IE_GXFIFO);

        MIi_FIFOCallback();

        (void)OS_RestoreInterrupts(enabled);
    }
}

/*---------------------------------------------------------------------------*
  Name:         MIi_FIFOCallback

  Description:  callback for GX FIFO
                (used for MI_SendGXCommandAsync)
  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
static void MIi_FIFOCallback(void)
{
    u32     length;
    u32     src;

    if (MIi_GXDmaParams.length == 0)
    {
        return;
    }

    //---- parameters for this fragment
    length =
        (MIi_GXDmaParams.length >=
         MIi_GX_LENGTH_ONCE) ? MIi_GX_LENGTH_ONCE : MIi_GXDmaParams.length;
    src = MIi_GXDmaParams.src;

    //---- arrange fragmental DMA params
    MIi_GXDmaParams.length -= length;
    MIi_GXDmaParams.src += length;

    //---- start fragmental DMA. if last, set DMA callback
    if (MIi_GXDmaParams.length == 0)
    {
        OSi_EnterDmaCallback(MIi_GXDmaParams.dmaNo, MIi_DMACallback, NULL);
        MIi_DmaSetParams(MIi_GXDmaParams.dmaNo, src, (u32)REG_GXFIFO_ADDR,
                         MI_CNT_SEND32_IF(length));
        (void)OS_ResetRequestIrqMask(OS_IE_GXFIFO);
    }
    else
    {
        MIi_DmaSetParams(MIi_GXDmaParams.dmaNo, src, (u32)REG_GXFIFO_ADDR, MI_CNT_SEND32(length));
        (void)OS_ResetRequestIrqMask(OS_IE_GXFIFO);
    }
}

/*---------------------------------------------------------------------------*
  Name:         MIi_DMACallback

  Description:  callback for DMA
                (used for MI_SendGXCommandAsync)

  Arguments:    not use

  Returns:      None
 *---------------------------------------------------------------------------*/
static void MIi_DMACallback(void *)
{
    (void)OS_DisableIrqMask(OS_IE_GXFIFO);

    //---- restore FIFO interrupt setting
    G3X_SetFifoIntrCond(MIi_GXDmaParams.fifoCond);
    OS_SetIrqFunction(OS_IE_GXFIFO, MIi_GXDmaParams.fifoFunc);

    MIi_GXDmaParams.isBusy = FALSE;

    MIi_CallCallback(MIi_GXDmaParams.callback, MIi_GXDmaParams.arg);
}

//================================================================================
//         use auto-DMA
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         MI_SendGXCommandFast

  Description:  send GX command with geometry FIFO DMA.
                sync version.
                send data at once, so DMA autostart may occur.

  Arguments:    dmaNo          : DMA channel No.
                src            : source address
                commandLength  : GX command length. (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
#include <nitro/itcm_begin.h>
void MI_SendGXCommandFast(u32 dmaNo, const void *src, u32 commandLength)
{
    vu32   *dmaCntp;

    MIi_ASSERT_DMANO(dmaNo);
    MIi_ASSERT_SRC_ALIGN4(src);
    MIi_WARNING_ADDRINTCM(src, commandLength);

    if (commandLength == 0)
    {
        return;
    }

    MIi_Wait_BeforeDMA(dmaCntp, dmaNo);
    MIi_DmaSetParams(dmaNo, (u32)src, (u32)REG_GXFIFO_ADDR, MI_CNT_GXCOPY(commandLength));
    MIi_Wait_AfterDMA(dmaCntp);
}

#include <nitro/itcm_end.h>

/*---------------------------------------------------------------------------*
  Name:         MI_SendGXCommandAsyncFast

  Description:  send GX command with geometry FIFO DMA.
                async version.
                send data at once, so DMA autostart may occur.

  Arguments:    dmaNo          : DMA channel No.
                src            : source address
                commandLength  : GX command length. (byte)
                callback       : callback function
                arg            : callback argument

  Returns:      None
 *---------------------------------------------------------------------------*/
void MI_SendGXCommandAsyncFast(u32 dmaNo, const void *src, u32 commandLength,
                               MIDmaCallback callback, void *arg)
{
    MIi_ASSERT_DMANO(dmaNo);
    MIi_ASSERT_SRC_ALIGN4(src);
    MIi_WARNING_ADDRINTCM(src, commandLength);

    if (commandLength == 0)
    {
        MIi_CallCallback(callback, arg);
        return;
    }

    //---- wait till other task end
    while (MIi_GXDmaParams.isBusy /*volatile valiable */ )
    {
    }

    //---- DMA params
    MIi_GXDmaParams.isBusy = TRUE;
    MIi_GXDmaParams.dmaNo = dmaNo;
    MIi_GXDmaParams.callback = callback;
    MIi_GXDmaParams.arg = arg;

    MIi_CheckAnotherAutoDMA(dmaNo, MI_DMA_TIMING_GXFIFO);
    //---- check DMA0 source address
    MIi_CheckDma0SourceAddress(dmaNo, (u32)src, commandLength, MI_DMA_SRC_INC);

    MI_WaitDma(dmaNo);

    OSi_EnterDmaCallback(dmaNo, MIi_DMAFastCallback, NULL);
    MIi_DmaSetParams(dmaNo, (u32)src, (u32)REG_GXFIFO_ADDR, MI_CNT_GXCOPY_IF(commandLength));
}

/*---------------------------------------------------------------------------*
  Name:         MIi_DMAFastCallback

  Description:  callback for DMA
                (used for MI_SendGXCommandAsyncFast)

  Arguments:    not use

  Returns:      None
 *---------------------------------------------------------------------------*/
static void MIi_DMAFastCallback(void *)
{
    MIi_GXDmaParams.isBusy = FALSE;

    MIi_CallCallback(MIi_GXDmaParams.callback, MIi_GXDmaParams.arg);
}

#endif // SDK_ARM9
