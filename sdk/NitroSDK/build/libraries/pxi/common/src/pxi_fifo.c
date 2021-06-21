/*---------------------------------------------------------------------------*
  Project:  NitroSDK - PXI - libraries
  File:     pxi_fifo.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: pxi_fifo.c,v $
  Revision 1.26  2006/05/22 07:00:50  okubata_ryoma
  割り込み禁止中でもPMの同期関数が動作するように修正

  Revision 1.25  2006/01/18 02:12:28  kitase_hirotake
  do-indent

  Revision 1.24  2005/02/28 05:26:29  yosizaki
  do-indent.

  Revision 1.23  2005/02/18 07:09:31  yasu
  Signed/Unsigned 変換警告抑制

  Revision 1.22  2005/02/17 00:04:08  yasu
  PXI_SetFifoRecvCallback の WARNING 出力条件を緩和

  Revision 1.21  2005/01/13 00:18:59  yada
  fix copyright header

  Revision 1.20  2005/01/12 11:41:34  yada
  clear PXI interrupt check flag before setting

  Revision 1.19  2005/01/12 11:18:28  yada
  exchange irq setting and PXI clear

  Revision 1.18  2004/09/15 09:11:55  terui
  PXI_SetFifoRecvCallbackにNULLを指定することで指定TAGの初期化前の状態になるように改造。

  Revision 1.17  2004/07/13 08:27:23  yada
  move PXI_IsArm7CallbackReady() and PXI_IsArm9CallbackReady() to pxi_fifo.h as inline.

  Revision 1.16  2004/05/20 07:20:41  yasu
  small fix

  Revision 1.15  2004/05/20 06:35:37  yasu
  (ARM7 only) Yield main bus to ARM9 when PXI_Init sync

  Revision 1.14  2004/05/17 08:34:00  yosizaki
  fix around priority. (== and &)

  Revision 1.13  2004/05/14 09:34:00  yasu
  add ARM9/ARM7 sync in PXI_Init

  Revision 1.12  2004/04/22 06:56:33  yasu
  workaround for CW blxeq/blxne problem

  Revision 1.11  2004/04/13 12:24:42  takano_makoto
  Down optimization level in PXIi_HandlerRecvFifoNotEmpty() because of CW probrem.

  Revision 1.10  2004/04/05 10:46:32  takano_makoto
  Modify error check code in PXIi_GetFromFifo() and PXIi_SetToFifo().

  Revision 1.9  2004/03/30 12:19:11  yasu
  change parameter of PXI_SetSendCallback

  Revision 1.8  2004/03/30 10:46:41  miya
  add send FIFO callback functions

  Revision 1.7  2004/03/10 03:07:33  yasu
  send signal to let ensata know PXI initialized

  Revision 1.6  2004/03/09 07:25:41  yada
  PXI_InitFIFO() の sync動作部分をコメントアウト

  Revision 1.5  2004/03/08 12:41:37  yasu
  workaround for ensata

  Revision 1.4  2004/03/05 09:40:10  yasu
  fix comments

  Revision 1.3  2004/03/05 04:19:55  yasu
  change comments

  Revision 1.2  2004/03/04 12:39:57  yasu
  reformat

  Revision 1.1  2004/03/04 09:06:41  miya
  add pxi_fifo.c

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include  <nitro.h>

static u16 FifoCtrlInit = 0;
static PXIFifoCallback FifoRecvCallbackTable[PXI_MAX_FIFO_TAG];

/*********** function prototypes ******************/
static inline PXIFifoStatus PXIi_GetFromFifo(u32 *data_buf);
static inline PXIFifoStatus PXIi_SetToFifo(u32 data);


/*---------------------------------------------------------------------------*
  Name:         PXI_InitFifo

  Description:  initialize FIFO system

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void PXI_InitFifo(void)
{
    int     i;
    OSIntrMode enabled;
    OSSystemWork *p = OS_GetSystemWork();

    enabled = OS_DisableInterrupts();

    if (!FifoCtrlInit)
    {
        FifoCtrlInit = TRUE;

        p->pxiHandleChecker[PXI_PROC_ARM] = 0UL;

        for (i = 0; i < PXI_MAX_FIFO_TAG; i++)
        {
            FifoRecvCallbackTable[i] = NULL;
        }

        reg_PXI_FIFO_CNT =
            (REG_PXI_FIFO_CNT_SEND_CL_MASK |
             REG_PXI_FIFO_CNT_RECV_RI_MASK | REG_PXI_FIFO_CNT_E_MASK | REG_PXI_FIFO_CNT_ERR_MASK);

        (void)OS_ResetRequestIrqMask(OS_IE_FIFO_RECV);
        (void)OS_SetIrqFunction(OS_IE_FIFO_RECV, PXIi_HandlerRecvFifoNotEmpty);
        (void)OS_EnableIrqMask(OS_IE_FIFO_RECV);

#ifndef SDK_FINALROM
        //
        // Send signal to let ensata know the time of PXI initialized
        //              Requested by ensata dev team 04/03/10
        //
        if (OS_IsRunOnEmulator())
        {
            reg_PXI_INTF = 0x100;
        }
        else
#endif
#ifdef  SDK_ARM7
        {
            //-------------------------------
            // PXI_INTF
            //   d14     RW  IE
            //   d13     W   IREQ
            //   d11-d08 RW  OUT Status
            //   d03-d00 R   IN  Status
            //-------------------------------
            for (i = 8; i >= 0; i--)
            {
                reg_PXI_INTF = (u16)(i << 8);
                OS_SpinWait(1000);     // Yield main bus to ARM9

                if ((reg_PXI_INTF & 15) != i)
                {
                    i = 8;
                }
            }
        }
#else
        {
            int     timeout;
            s32     c;

            for (i = 0;; i++)
            {
                c = reg_PXI_INTF & 15;
                reg_PXI_INTF = (u16)(c << 8);

                if (c == 0 && i > 4)
                {
                    break;
                }

                for (timeout = 1000; (reg_PXI_INTF & 15) == c; timeout--)
                {
                    if (timeout == 0)
                    {
                        i = 0;
                        break;
                    }
                }
            }
        }
#endif
    }
    (void)OS_RestoreInterrupts(enabled);
}


/*---------------------------------------------------------------------------*
  Name:         PXI_SetFifoRecvCallback

  Description:  set callback function when data arrive via FIFO

  Arguments:    fifotag    fifo tag NO (0-31)
                callback   callback function to be called

  Returns:      None.
 *---------------------------------------------------------------------------*/
void PXI_SetFifoRecvCallback(int fifotag, PXIFifoCallback callback)
{
    OSIntrMode enabled;
    OSSystemWork *p = OS_GetSystemWork();

    SDK_WARNING(callback == NULL ||
                FifoRecvCallbackTable[fifotag] == NULL ||
                FifoRecvCallbackTable[fifotag] == callback,
                "Fifo Callback overridden [fifotag=%d]\n", fifotag);

    enabled = OS_DisableInterrupts();

    FifoRecvCallbackTable[fifotag] = callback;
    if (callback)
    {
        p->pxiHandleChecker[PXI_PROC_ARM] |= (1UL << fifotag);
    }
    else
    {
        p->pxiHandleChecker[PXI_PROC_ARM] &= ~(1UL << fifotag);
    }
    (void)OS_RestoreInterrupts(enabled);
}

/*---------------------------------------------------------------------------*
  Name:         PXI_IsCallbackReady

  Description:  check if callback is ready

  Arguments:    fifotag    fifo tag NO (0-31)
                proc       processor name PXI_PROC_ARM9 or PXI_PROC_ARM7

  Returns:      True if callback is ready
 *---------------------------------------------------------------------------*/
BOOL PXI_IsCallbackReady(int fifotag, PXIProc proc)
{
    OSSystemWork *p = OS_GetSystemWork();

    return (p->pxiHandleChecker[proc] & (1UL << fifotag)) ? TRUE : FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         PXI_SetFifoSendCallback

  Description:  set callback function when FIFO empty

  Arguments:    callback   callback function to be called

  Returns:      None.
 *---------------------------------------------------------------------------*/
void PXI_SetFifoSendCallback(PXIFifoEmtpyCallback callback)
{
    OSIntrMode enabled;

    enabled = OS_DisableInterrupts();

    (void)OS_SetIrqFunction(OS_IE_FIFO_SEND, callback);
    (void)OS_EnableIrqMask(OS_IE_FIFO_SEND);

    reg_PXI_FIFO_CNT |= REG_PXI_FIFO_CNT_SEND_TI_MASK;

    (void)OS_RestoreInterrupts(enabled);
}


/*---------------------------------------------------------------------------*
  Name:         PXI_SendWordByFifo

  Description:  Send 32bit-word to anothre CPU via FIFO

  Arguments:    fifotag  fifo tag NO (0-31)
                data     data(26-bit) whichi is sent
                err      1 on error

  Returns:      if error occured, returns minus value
 *---------------------------------------------------------------------------*/
int PXI_SendWordByFifo(int fifotag, u32 data, BOOL err)
{
    PXIFifoMessage fifomsg;

    SDK_ASSERTMSG(0 <= fifotag && fifotag < PXI_MAX_FIFO_TAG, "[FifoTag] out of range");
    SDK_ASSERTMSG(data < (1UL << PXI_FIFOMESSAGE_BITSZ_DATA), "[data] out of range");

    fifomsg.e.tag = (PXIFifoTag)fifotag;
    fifomsg.e.err = (u32)err;
    fifomsg.e.data = data;

    return PXIi_SetToFifo(fifomsg.raw);
}


//======================================================================
//                      Write Send-FIFO reg.
//======================================================================
static inline PXIFifoStatus PXIi_SetToFifo(u32 data)
{
    OSIntrMode enabled;

    if (reg_PXI_FIFO_CNT & REG_PXI_FIFO_CNT_ERR_MASK)
    {
        reg_PXI_FIFO_CNT |= (REG_PXI_FIFO_CNT_E_MASK | REG_PXI_FIFO_CNT_ERR_MASK);
        return PXI_FIFO_FAIL_SEND_ERR;
    }

    enabled = OS_DisableInterrupts();
    if (reg_PXI_FIFO_CNT & REG_PXI_FIFO_CNT_SEND_FULL_MASK)
    {
        (void)OS_RestoreInterrupts(enabled);
        return PXI_FIFO_FAIL_SEND_FULL;
    }

    reg_PXI_SEND_FIFO = data;
    (void)OS_RestoreInterrupts(enabled);
    return PXI_FIFO_SUCCESS;
}


//======================================================================
//                      Read Send-FIFO reg.
//======================================================================
static inline PXIFifoStatus PXIi_GetFromFifo(u32 *data_buf)
{

    OSIntrMode enabled;

    if (reg_PXI_FIFO_CNT & REG_PXI_FIFO_CNT_ERR_MASK)
    {
        reg_PXI_FIFO_CNT |= (REG_PXI_FIFO_CNT_E_MASK | REG_PXI_FIFO_CNT_ERR_MASK);
        return PXI_FIFO_FAIL_RECV_ERR;
    }

    enabled = OS_DisableInterrupts();
    if (reg_PXI_FIFO_CNT & REG_PXI_FIFO_CNT_RECV_EMP_MASK)
    {
        (void)OS_RestoreInterrupts(enabled);
        return PXI_FIFO_FAIL_RECV_EMPTY;
    }

    *data_buf = reg_PXI_RECV_FIFO;
    (void)OS_RestoreInterrupts(enabled);

    return PXI_FIFO_SUCCESS;
}


//======================================================================
//      Interrupt handler called when SEND FIFO empty
//======================================================================
static void PXIi_HandlerSendFifoEmpty(void)
{
}

//======================================================================
//              Interrupt handler called when RECV FIFO not empty
//======================================================================
#ifdef  SDK_CW_WA_OPT_BLX
#pragma     optimization_level  1      // for BLX problem
#endif

void PXIi_HandlerRecvFifoNotEmpty(void)
{
    PXIFifoMessage fifomsg;
    PXIFifoStatus ret_code;
    PXIFifoTag tag;

    while (1)
    {
        ret_code = PXIi_GetFromFifo(&fifomsg.raw);

        if (ret_code == PXI_FIFO_FAIL_RECV_EMPTY)
            break;
        if (ret_code == PXI_FIFO_FAIL_RECV_ERR)
            continue;

        tag = (PXIFifoTag)fifomsg.e.tag;

        if (tag)
        {
            if (FifoRecvCallbackTable[tag])
            {
                (FifoRecvCallbackTable[tag]) (tag, fifomsg.e.data, (BOOL)fifomsg.e.err);
            }
            else
            {
                if (fifomsg.e.err)     /* reject if ping-pong message */
                {
                }
                else
                {
                    fifomsg.e.err = TRUE;       /* Set error & block send */
                    (void)PXIi_SetToFifo(fifomsg.raw);
                }
            }
        }
        else
        {
            /* Extended FIFO command */
        }
    }
}

#ifdef  SDK_CW_WA_OPT_BLX
#pragma     optimization_level  4
#endif
