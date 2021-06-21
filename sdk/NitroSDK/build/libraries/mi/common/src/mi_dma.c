/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI
  File:     mi_dma.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mi_dma.c,v $
  Revision 1.39  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.38  2005/02/28 05:26:25  yosizaki
  do-indent.

  Revision 1.37  2005/02/18 06:41:36  yasu
  OS_DisableInterrupts 返値の型修正

  Revision 1.36  2005/01/12 11:13:37  yada
  consider about multi-DMA

  Revision 1.35  2005/01/12 11:10:15  takano_makoto
  fix copyright header.

  Revision 1.34  2005/01/12 02:47:52  yada
  consider about multi-DMA

  Revision 1.33  2004/11/02 09:54:15  yada
  just fix comment

  Revision 1.32  2004/10/21 06:54:50  yada
  MI_DmaFill*() was not interrupt-safe function. Fix that.

  Revision 1.31  2004/10/13 11:49:53  yada
  fix ITCM address in tcm check routine

  Revision 1.30  2004/09/28 04:39:38  yada
  only fix comment

  Revision 1.29  2004/09/22 01:51:29  yada
  same auto DMAs are allowed

  Revision 1.28  2004/09/02 12:48:16  yada
  fix autoDMA check

  Revision 1.27  2004/08/30 11:53:14  yada
  in ARM9, move MIi_DmaSetParams to ITCM

  Revision 1.26  2004/08/30 04:28:30  yada
  MIi_CNT_XXX -> MI_CNT_XXX

  Revision 1.25  2004/08/26 01:23:07  yada
  move function body of MIi_CheckAddressInTCM() to mi_dma.c

  Revision 1.24  2004/08/26 00:12:45  yada
  change multiple auto-DMA check method

  Revision 1.23  2004/08/25 11:56:18  yada
  add check for avoiding more than 2 auto_start_DMA run at the same time

  Revision 1.22  2004/08/25 00:49:58  yada
  separate each type of dma from mi_dma.c

  Revision 1.21  2004/08/07 02:04:54  yada
  in case of sync dma,
  insert wait while dma busy after dma  in addition to before dma

  Revision 1.20  2004/06/29 04:31:59  yada
  fix typo (if -> is)

  Revision 1.19  2004/06/25 12:03:34  yada
  fix around ITCM&DTCM

  Revision 1.18  2004/06/22 02:07:11  yada
  disable interrupt when MIi_SetParameter* and MI_StopDma

  Revision 1.17  2004/06/15 04:25:46  yada
  fix ITCM Address to 0

  Revision 1.16  2004/06/15 02:09:00  yada
  change around DMA_CLEAR_BUF.
  separate by case ARM9-TS, ARM9-TEG, ARM7.

  Revision 1.15  2004/05/14 10:30:39  takano_makoto
  Change #if SDK_ARM9 -> #ifdef SDK_ARM9

  Revision 1.14  2004/03/23 00:58:15  yada
  only fix comment

  Revision 1.13  2004/02/18 12:29:35  yada
  転送範囲が TCM　に入っていたら警告を出すように変更

  Revision 1.12  2004/02/18 10:53:53  yada
  DEBUGビルド時、src, dest アドレスが TCM　に入っていたら警告を出すようにした。

  Revision 1.11  2004/02/13 08:56:37  yasu
  fix small typo

  Revision 1.10  2004/02/13 06:31:01  yada
  ARM7側の転送サイズ制限を考慮。
  あわせてASSERT書き換え。

  Revision 1.9  2004/02/13 04:36:59  yada
  ARM9とAMR7 の場合わけ考慮

  Revision 1.8  2004/02/10 11:22:15  yada
  only arrange tab

  Revision 1.7  2004/02/05 11:39:23  yada
 *Send*() 関数の転送先ポインタをvolatileにした。

  Revision 1.6  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.5  2004/01/30 05:00:58  yada
  MI_DmaSend*, MI_CpuSend* 追加

  Revision 1.4  2004/01/29 05:09:10  yada
  callback指定がNULLの場合のAsync版はウェイトなしで進むようにした。
  また、MI_DmaCopy16Async でコールバックが呼ばれない不具合を修正。

  Revision 1.3  2004/01/29 04:22:34  yada
  callback指定がNULLの場合のAsync版はAsyncなしのもの同等にした。

  Revision 1.2  2004/01/26 12:50:27  yada
  MI_WaitDma inline ミス修正

  Revision 1.1  2004/01/26 12:42:59  yada
  firstRelease。 dma.h の inline 見直し

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include "../include/mi_dma.h"

#ifdef SDK_ARM9
#include <nitro/itcm_begin.h>
//================================================================================
//            start DMA
//================================================================================
//---------------- set parameters
void MIi_DmaSetParams(u32 dmaNo, u32 src, u32 dest, u32 ctrl)
{
    OSIntrMode enabled = OS_DisableInterrupts();
    vu32   *p = (vu32 *)((u32)REG_DMA0SAD_ADDR + dmaNo * 12);
    *p = (vu32)src;
    *(p + 1) = (vu32)dest;
    *(p + 2) = (vu32)ctrl;
    (void)OS_RestoreInterrupts(enabled);
}

void MIi_DmaSetParams_wait(u32 dmaNo, u32 src, u32 dest, u32 ctrl)
{
    OSIntrMode enabled = OS_DisableInterrupts();
    vu32   *p = (vu32 *)((u32)REG_DMA0SAD_ADDR + dmaNo * 12);
    *p = (vu32)src;
    *(p + 1) = (vu32)dest;
    *(p + 2) = (vu32)ctrl;

    //---- ARM9 must wait 2 cycle (load is 1/2 cycle)
    {
        u32     dummy = reg_MI_DMA0SAD;
    }
    {
        u32     dummy = reg_MI_DMA0SAD;
    }

    //---- for multiple DMA problem in DMA0
    if (dmaNo == MIi_DUMMY_DMA_NO)
    {
        *p = (vu32)MIi_DUMMY_SRC;
        *(p + 1) = (vu32)MIi_DUMMY_DEST;
        *(p + 2) = (vu32)MIi_DUMMY_CNT;
    }

    (void)OS_RestoreInterrupts(enabled);
}

void MIi_DmaSetParams_noInt(u32 dmaNo, u32 src, u32 dest, u32 ctrl)
{
    vu32   *p = (vu32 *)((u32)REG_DMA0SAD_ADDR + dmaNo * 12);
    *p = (vu32)src;
    *(p + 1) = (vu32)dest;
    *(p + 2) = (vu32)ctrl;
}

void MIi_DmaSetParams_wait_noInt(u32 dmaNo, u32 src, u32 dest, u32 ctrl)
{
    vu32   *p = (vu32 *)((u32)REG_DMA0SAD_ADDR + dmaNo * 12);
    *p = (vu32)src;
    *(p + 1) = (vu32)dest;
    *(p + 2) = (vu32)ctrl;

    //---- ARM9 must wait 2 cycle (load is 1/2 cycle)
    {
        u32     dummy = reg_MI_DMA0SAD;
    }
    {
        u32     dummy = reg_MI_DMA0SAD;
    }

    //---- for multiple DMA problem in DMA0
    if (dmaNo == MIi_DUMMY_DMA_NO)
    {
        *p = (vu32)MIi_DUMMY_SRC;
        *(p + 1) = (vu32)MIi_DUMMY_DEST;
        *(p + 2) = (vu32)MIi_DUMMY_CNT;
    }

    //---- ARM9 must wait 2 cycle (load is 1/2 cycle)
    {
        u32     dummy = reg_MI_DMA0SAD;
    }
    {
        u32     dummy = reg_MI_DMA0SAD;
    }
}

#include <nitro/itcm_end.h>
#endif // ifdef SDK_ARM9

//================================================================================
//            memory oparation using DMA (sync)
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         MI_DmaFill32

  Description:  fill memory with specified data.
                sync 32bit version

  Arguments:    dmaNo : DMA channel No.
                dest  : destination address
                data  : fill data
                size  : size (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
void MI_DmaFill32(u32 dmaNo, void *dest, u32 data, u32 size)
{
    vu32   *dmaCntp;

    MIi_ASSERT_DMANO(dmaNo);
    MIi_ASSERT_MUL4(size);
    MIi_ASSERT_SIZE(dmaNo, size / 4);
    MIi_ASSERT_DEST_ALIGN4(dest);
    MIi_WARNING_ADDRINTCM(dest, size);

    if (size == 0)
    {
        return;
    }

    MIi_Wait_BeforeDMA(dmaCntp, dmaNo);
    MIi_DmaSetParams_wait_src32(dmaNo, data, (u32)dest, MI_CNT_CLEAR32(size));
    MIi_Wait_AfterDMA(dmaCntp);
}

/*---------------------------------------------------------------------------*
  Name:         MI_DmaCopy32

  Description:  copy memory with DMA
                sync 32bit version

  Arguments:    dmaNo : DMA channel No.
                src   : source address
                dest  : destination address
                size  : size (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
void MI_DmaCopy32(u32 dmaNo, const void *src, void *dest, u32 size)
{
    vu32   *dmaCntp;

    MIi_ASSERT_DMANO(dmaNo);
    MIi_ASSERT_MUL4(size);
    MIi_ASSERT_SIZE(dmaNo, size / 4);
    MIi_ASSERT_SRC_ALIGN4(src);
    MIi_ASSERT_DEST_ALIGN4(dest);
    MIi_WARNING_ADDRINTCM(src, size);
    MIi_WARNING_ADDRINTCM(dest, size);

    //---- check DMA0 source address
    MIi_CheckDma0SourceAddress(dmaNo, (u32)src, size, MI_DMA_SRC_INC);

    if (size == 0)
    {
        return;
    }

    MIi_Wait_BeforeDMA(dmaCntp, dmaNo);
    MIi_DmaSetParams_wait(dmaNo, (u32)src, (u32)dest, MI_CNT_COPY32(size));
    MIi_Wait_AfterDMA(dmaCntp);
}

/*---------------------------------------------------------------------------*
  Name:         MI_DmaSend32

  Description:  send u32 data to fixed address
                sync 32bit version

  Arguments:    dmaNo : DMA channel No.
                src   : data stream to send
                dest  : destination address. not incremented
                size  : size (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
void MI_DmaSend32(u32 dmaNo, const void *src, volatile void *dest, u32 size)
{
    vu32   *dmaCntp;

    MIi_ASSERT_DMANO(dmaNo);
    MIi_ASSERT_MUL4(size);
    MIi_ASSERT_SIZE(dmaNo, size / 4);
    MIi_ASSERT_SRC_ALIGN4(src);
    MIi_ASSERT_DEST_ALIGN4(dest);
    MIi_WARNING_ADDRINTCM(src, size);
    MIi_WARNING_ADDRINTCM(dest, size);

    //---- check DMA0 source address
    MIi_CheckDma0SourceAddress(dmaNo, (u32)src, size, MI_DMA_SRC_INC);

    if (size == 0)
    {
        return;
    }

    MIi_Wait_BeforeDMA(dmaCntp, dmaNo);
    MIi_DmaSetParams_wait(dmaNo, (u32)src, (u32)dest, MI_CNT_SEND32(size));
    MIi_Wait_AfterDMA(dmaCntp);
}

/*---------------------------------------------------------------------------*
  Name:         MI_DmaFill16

  Description:  fill memory with specified data.
                sync 16bit version

  Arguments:    dmaNo : DMA channel No.
                dest  : destination address
                data  : fill data
                size  : size (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
void MI_DmaFill16(u32 dmaNo, void *dest, u16 data, u32 size)
{
    vu32   *dmaCntp;

    MIi_ASSERT_DMANO(dmaNo);
    MIi_ASSERT_MUL2(size);
    MIi_ASSERT_SIZE(dmaNo, size / 2);
    MIi_ASSERT_DEST_ALIGN2(dest);
    MIi_WARNING_ADDRINTCM(dest, size);

    if (size == 0)
    {
        return;
    }

    MIi_Wait_BeforeDMA(dmaCntp, dmaNo);
    MIi_DmaSetParams_wait_src32(dmaNo, data, (u32)dest, MI_CNT_CLEAR16(size));
    MIi_Wait_AfterDMA(dmaCntp);
}

/*---------------------------------------------------------------------------*
  Name:         MI_DmaCopy16

  Description:  copy memory with DMA
                sync 16bit version

  Arguments:    dmaNo : DMA channel No.
                src   : source address
                dest  : destination address
                size  : size (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
void MI_DmaCopy16(u32 dmaNo, const void *src, void *dest, u32 size)
{
    vu32   *dmaCntp;

    MIi_ASSERT_DMANO(dmaNo);
    MIi_ASSERT_MUL2(size);
    MIi_ASSERT_SIZE(dmaNo, size / 2);
    MIi_ASSERT_SRC_ALIGN2(src);
    MIi_ASSERT_DEST_ALIGN2(dest);
    MIi_WARNING_ADDRINTCM(src, size);
    MIi_WARNING_ADDRINTCM(dest, size);

    if (size == 0)
    {
        return;
    }

    //---- check DMA0 source address
    MIi_CheckDma0SourceAddress(dmaNo, (u32)src, size, MI_DMA_SRC_INC);

    MIi_Wait_BeforeDMA(dmaCntp, dmaNo);
    MIi_DmaSetParams_wait(dmaNo, (u32)src, (u32)dest, MI_CNT_COPY16(size));
    MIi_Wait_AfterDMA(dmaCntp);
}

/*---------------------------------------------------------------------------*
  Name:         MI_DmaSend16

  Description:  send u16 data to fixed address
                sync 16bit version

  Arguments:    dmaNo : DMA channel No.
                src   : data stream to send
                dest  : destination address. not incremented
                size  : size (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
void MI_DmaSend16(u32 dmaNo, const void *src, volatile void *dest, u32 size)
{
    vu32   *dmaCntp;

    MIi_ASSERT_DMANO(dmaNo);
    MIi_ASSERT_MUL2(size);
    MIi_ASSERT_SIZE(dmaNo, size / 2);
    MIi_ASSERT_SRC_ALIGN2(src);
    MIi_ASSERT_DEST_ALIGN2(dest);
    MIi_WARNING_ADDRINTCM(src, size);
    MIi_WARNING_ADDRINTCM(dest, size);

    if (size == 0)
    {
        return;
    }

    //---- check DMA0 source address
    MIi_CheckDma0SourceAddress(dmaNo, (u32)src, size, MI_DMA_SRC_INC);

    MIi_Wait_BeforeDMA(dmaCntp, dmaNo);
    MIi_DmaSetParams_wait(dmaNo, (u32)src, (u32)dest, MI_CNT_SEND16(size));
    MIi_Wait_AfterDMA(dmaCntp);
}

//================================================================================
//            memory oparation using DMA (async)
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         MI_DmaFill32Async

  Description:  fill memory with specified data.
                async 32bit version

  Arguments:    dmaNo : DMA channel No.
                dest  : destination address
                data  : fill data
                size  : size (byte)
                callback : callback function called finish DMA
                arg      : callback argument

  Returns:      None
 *---------------------------------------------------------------------------*/
void MI_DmaFill32Async(u32 dmaNo, void *dest, u32 data, u32 size, MIDmaCallback callback, void *arg)
{
    MIi_ASSERT_DMANO(dmaNo);
    MIi_ASSERT_MUL4(size);
    MIi_ASSERT_SIZE(dmaNo, size / 4);
    MIi_ASSERT_DEST_ALIGN4(dest);
    MIi_WARNING_ADDRINTCM(dest, size);

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
            MIi_DmaSetParams_src32(dmaNo, data, (u32)dest, MI_CNT_CLEAR32_IF(size));
        }
        else
        {
            MIi_DmaSetParams_src32(dmaNo, data, (u32)dest, MI_CNT_CLEAR32(size));
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         MI_DmaCopy32Async

  Description:  copy memory with DMA
                async 32bit version

  Arguments:    dmaNo : DMA channel No.
                src   : source address
                dest  : destination address
                size  : size (byte)
                callback : callback function called finish DMA
                arg      : callback argument

  Returns:      None
 *---------------------------------------------------------------------------*/
void MI_DmaCopy32Async(u32 dmaNo, const void *src, void *dest, u32 size, MIDmaCallback callback,
                       void *arg)
{
    MIi_ASSERT_DMANO(dmaNo);
    MIi_ASSERT_MUL4(size);
    MIi_ASSERT_SIZE(dmaNo, size / 4);
    MIi_ASSERT_SRC_ALIGN4(src);
    MIi_ASSERT_DEST_ALIGN4(dest);
    MIi_WARNING_ADDRINTCM(src, size);
    MIi_WARNING_ADDRINTCM(dest, size);

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
            MIi_DmaSetParams(dmaNo, (u32)src, (u32)dest, MI_CNT_COPY32_IF(size));
        }
        else
        {
            MIi_DmaSetParams(dmaNo, (u32)src, (u32)dest, MI_CNT_COPY32(size));
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         MI_DmaSend32Async

  Description:  send u32 data to fixed address
                async 32bit version

  Arguments:    dmaNo : DMA channel No.
                src   : data stream to send
                dest  : destination address. not incremented
                size  : size (byte)
                callback : callback function called finish DMA
                arg      : callback argument

  Returns:      None
 *---------------------------------------------------------------------------*/
void MI_DmaSend32Async(u32 dmaNo, const void *src, volatile void *dest, u32 size,
                       MIDmaCallback callback, void *arg)
{
    MIi_ASSERT_DMANO(dmaNo);
    MIi_ASSERT_MUL4(size);
    MIi_ASSERT_SIZE(dmaNo, size / 4);
    MIi_ASSERT_SRC_ALIGN4(src);
    MIi_ASSERT_DEST_ALIGN4(dest);
    MIi_WARNING_ADDRINTCM(src, size);
    MIi_WARNING_ADDRINTCM(dest, size);

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
            MIi_DmaSetParams(dmaNo, (u32)src, (u32)dest, MI_CNT_SEND32_IF(size));
        }
        else
        {
            MIi_DmaSetParams(dmaNo, (u32)src, (u32)dest, MI_CNT_SEND32(size));
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         MI_DmaFill16Async

  Description:  fill memory with specified data.
                async 16bit version

  Arguments:    dmaNo : DMA channel No.
                dest  : destination address
                data  : fill data
                size  : size (byte)
                callback : callback function called finish DMA
                arg      : callback argument

  Returns:      None
 *---------------------------------------------------------------------------*/
void MI_DmaFill16Async(u32 dmaNo, void *dest, u16 data, u32 size, MIDmaCallback callback, void *arg)
{
    MIi_ASSERT_DMANO(dmaNo);
    MIi_ASSERT_MUL2(size);
    MIi_ASSERT_SIZE(dmaNo, size / 2);
    MIi_ASSERT_DEST_ALIGN2(dest);
    MIi_WARNING_ADDRINTCM(dest, size);

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
            MIi_DmaSetParams_src32(dmaNo, data, (u32)dest, MI_CNT_CLEAR16_IF(size));
        }
        else
        {
            MIi_DmaSetParams_src32(dmaNo, data, (u32)dest, MI_CNT_CLEAR16(size));
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         MI_DmaCopy16Async

  Description:  copy memory with DMA
                async 16bit version

  Arguments:    dmaNo : DMA channel No.
                src   : source address
                dest  : destination address
                size  : size (byte)
                callback : callback function called finish DMA
                arg      : callback argument

  Returns:      None
 *---------------------------------------------------------------------------*/
void MI_DmaCopy16Async(u32 dmaNo, const void *src, void *dest, u32 size, MIDmaCallback callback,
                       void *arg)
{
    MIi_ASSERT_DMANO(dmaNo);
    MIi_ASSERT_MUL2(size);
    MIi_ASSERT_SIZE(dmaNo, size / 2);
    MIi_ASSERT_SRC_ALIGN2(src);
    MIi_ASSERT_DEST_ALIGN2(dest);
    MIi_WARNING_ADDRINTCM(src, size);
    MIi_WARNING_ADDRINTCM(dest, size);

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
            MIi_DmaSetParams(dmaNo, (u32)src, (u32)dest, MI_CNT_COPY16_IF(size));
        }
        else
        {
            MIi_DmaSetParams(dmaNo, (u32)src, (u32)dest, MI_CNT_COPY16(size));
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         MI_DmaSend16Async

  Description:  send u16 data to fixed address
                async 16bit version

  Arguments:    dmaNo : DMA channel No.
                src   : data stream to send
                dest  : destination address. not incremented
                size  : size (byte)
                callback : callback function called finish DMA
                arg      : callback argument

  Returns:      None
 *---------------------------------------------------------------------------*/
void MI_DmaSend16Async(u32 dmaNo, const void *src, volatile void *dest, u32 size,
                       MIDmaCallback callback, void *arg)
{
    MIi_ASSERT_DMANO(dmaNo);
    MIi_ASSERT_MUL2(size);
    MIi_ASSERT_SIZE(dmaNo, size / 2);
    MIi_ASSERT_SRC_ALIGN2(src);
    MIi_ASSERT_DEST_ALIGN2(dest);
    MIi_WARNING_ADDRINTCM(src, size);
    MIi_WARNING_ADDRINTCM(dest, size);

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
            MIi_DmaSetParams(dmaNo, (u32)src, (u32)dest, MI_CNT_SEND16_IF(size));
        }
        else
        {
            MIi_DmaSetParams(dmaNo, (u32)src, (u32)dest, MI_CNT_SEND16(size));
        }
    }
}


//================================================================================
//       DMA WAIT/STOP
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         MI_IsDmaBusy

  Description:  check whether DMA is busy or not

  Arguments:    dmaNo : DMA channel No.

  Returns:      TRUE if DMA is busy, FALSE if not
 *---------------------------------------------------------------------------*/
BOOL MI_IsDmaBusy(u32 dmaNo)
{
    vu32   *dmaCntp = &((vu32 *)REG_DMA0SAD_ADDR)[dmaNo * 3 + 2];

    MIi_ASSERT_DMANO(dmaNo);
    return (BOOL)((*(vu32 *)dmaCntp & REG_MI_DMA0CNT_E_MASK) >> REG_MI_DMA0CNT_E_SHIFT);
}

/*---------------------------------------------------------------------------*
  Name:         MI_WaitDma

  Description:  wait while DMA is busy

  Arguments:    dmaNo : DMA channel No.

  Returns:      None
 *---------------------------------------------------------------------------*/
void MI_WaitDma(u32 dmaNo)
{
    OSIntrMode enabled = OS_DisableInterrupts();
    vu32   *dmaCntp = &((vu32 *)REG_DMA0SAD_ADDR)[dmaNo * 3 + 2];

    MIi_ASSERT_DMANO(dmaNo);
    while (*dmaCntp & REG_MI_DMA0CNT_E_MASK)
    {
    }

    //---- for multiple DMA problem in DMA0
    if (dmaNo == MIi_DUMMY_DMA_NO)
    {
        vu32   *p = (vu32 *)((u32)REG_DMA0SAD_ADDR + dmaNo * 12);
        *p = (vu32)MIi_DUMMY_SRC;
        *(p + 1) = (vu32)MIi_DUMMY_DEST;
        *(p + 2) = (vu32)MIi_DUMMY_CNT;
    }

    (void)OS_RestoreInterrupts(enabled);
}

/*---------------------------------------------------------------------------*
  Name:         MI_StopDma

  Description:  stop DMA

  Arguments:    dmaNo : DMA channel No.

  Returns:      None
 *---------------------------------------------------------------------------*/
void MI_StopDma(u32 dmaNo)
{
    OSIntrMode enabled = OS_DisableInterrupts();
    vu16   *dmaCntp = &((vu16 *)REG_DMA0SAD_ADDR)[dmaNo * 6 + 5];

    MIi_ASSERT_DMANO(dmaNo);

    *dmaCntp &= ~((MI_DMA_TIMING_MASK | MI_DMA_CONTINUOUS_ON) >> 16);
    *dmaCntp &= ~(MI_DMA_ENABLE >> 16);

    //---- ARM9 must wait 2 cycle (load is 1/2 cycle)
    {
        s32     dummy = dmaCntp[0];
    }
    {
        s32     dummy = dmaCntp[0];
    }

    //---- for multiple DMA problem in DMA0
    if (dmaNo == MIi_DUMMY_DMA_NO)
    {
        vu32   *p = (vu32 *)((u32)REG_DMA0SAD_ADDR + dmaNo * 12);
        *p = (vu32)MIi_DUMMY_SRC;
        *(p + 1) = (vu32)MIi_DUMMY_DEST;
        *(p + 2) = (vu32)MIi_DUMMY_CNT;
    }

    (void)OS_RestoreInterrupts(enabled);
}


//================================================================================
//           for avoid multiple auto start DMA
//================================================================================
#ifdef SDK_ARM9
/*---------------------------------------------------------------------------*
  Name:         MIi_CheckAnotherAutoDMA

  Description:  check whether other auto DMA is running.
                (however, HBlankDMA and VBlankDMA are permit to
                 run at the same time.)

  Arguments:    dmaNo   : DMA channel No.
                dmaType : DMA type
                src     : source address
                size    : DMA size
                dir     : source direction (MI_DMA_SRC_INC/DEC/FIX)

  Returns:      None
 *---------------------------------------------------------------------------*/
void MIi_CheckAnotherAutoDMA(u32 dmaNo, u32 dmaType)
{
    int     n;
    u32     dmaCnt;
    u32     timing;

    for (n = 0; n < MI_DMA_MAX_NUM; n++)
    {
        if (n == dmaNo)
        {
            continue;
        }

        dmaCnt = *(REGType32v *)(REG_DMA0CNT_ADDR + n * 12);

        //---- not used
        if ((dmaCnt & MI_DMA_ENABLE) == 0)
        {
            continue;
        }

        timing = dmaCnt & MI_DMA_TIMING_MASK;

        //---- if two DMAs are same, or HDMA and VDMA, it is allowed
        if (timing == dmaType
            || (timing == MI_DMA_TIMING_V_BLANK && dmaType == MI_DMA_TIMING_H_BLANK)
            || (timing == MI_DMA_TIMING_H_BLANK && dmaType == MI_DMA_TIMING_V_BLANK))
        {
            continue;
        }

        //---- check other auto DMA running
        if (timing == MI_DMA_TIMING_DISP
            || timing == MI_DMA_TIMING_DISP_MMEM
            || timing == MI_DMA_TIMING_CARD
            || timing == MI_DMA_TIMING_CARTRIDGE
            || timing == MI_DMA_TIMING_GXFIFO
            || timing == MI_DMA_TIMING_V_BLANK || timing == MI_DMA_TIMING_H_BLANK)
        {
            OS_Panic("cannot start auto DMA at the same time.");
        }
    }
}

#endif // ifdef SDK_ARM9

/*---------------------------------------------------------------------------*
  Name:         MIi_CheckDma0SourceAddress

  Description:  IN case of using DMA0, check source address.
                Source address which is in I/O register or cartridge bus
                is not available.

  Arguments:    dmaNo   : DMA channel No.
                src     : source address
                size    : DMA size
                dir     : source direction (MI_DMA_SRC_INC/DEC/FIX)

  Returns:      None
 *---------------------------------------------------------------------------*/
void MIi_CheckDma0SourceAddress(u32 dmaNo, u32 src, u32 size, u32 dir)
{
    //---- only in case of DMA0
    if (dmaNo == 0)
    {
        u32     addStart;
        u32     addEnd;

        //---- start address of souce
        addStart = src & 0xff000000;

        //---- end address of source
        switch (dir)
        {
        case MI_DMA_SRC_INC:
            addEnd = src + size;
            break;
        case MI_DMA_SRC_DEC:
            addEnd = src - size;
            break;
        default:
            addEnd = src;
            break;
        }
        addEnd &= 0xff000000;

        //---- check start and end address of source
        if (addStart == 0x04000000 || addStart >= 0x08000000 ||
            addEnd == 0x04000000 || addEnd >= 0x08000000)
        {
            OS_Panic("illegal DMA0 source address.");
        }
    }
}

//================================================================================
//           check if area is in TCM
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         MIi_CheckAddressInTCM

  Description:  Check if specified area (from address to address+size)
                is in ITCM/DTCM.

  Arguments:    addr  : start address
                size  : size

  Returns:      None
 *---------------------------------------------------------------------------*/
#if defined( SDK_ARM9 ) && defined( SDK_DEBUG )
void MIi_CheckAddressInTCM(u32 addr, u32 size)
{
    u32     itcm = HW_ITCM;            // ITCM fixed to HW_ITCM (=0x01ff8000)
    u32     dtcm = OS_GetDTCMAddress();

    SDK_WARNING(itcm >= addr + size
                || addr >= itcm + HW_ITCM_SIZE, "intend to do DMA in ITCM area (%x)", addr);
    SDK_WARNING(dtcm >= addr + size
                || addr >= dtcm + HW_DTCM_SIZE, "intend to do DMA in DTCM area (%x)", addr);
}
#endif
