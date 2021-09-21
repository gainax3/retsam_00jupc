/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CTRDG
  File:     ctrdg.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ctrdg_proc.c,v $
  Revision 1.31  2007/02/20 00:28:11  kitase_hirotake
  indent source

  Revision 1.30  2006/08/10 00:25:57  okubata_ryoma
  small fix

  Revision 1.29  2006/08/10 00:02:52  okubata_ryoma
  カートリッジの活線挿抜に関する不具合修正

  Revision 1.28  2006/05/02 02:04:47  kitase_hirotake
  不必要なデバッグ出力の削除

  Revision 1.27  2006/05/02 02:03:23  kitase_hirotake
  不必要なデバッグ出力の削除

  Revision 1.26  2006/05/01 06:04:55  yada
  avoid warning

  Revision 1.25  2006/04/28 11:04:22  yada
  add about PHI Clock setting

  Revision 1.24  2006/04/05 10:48:30  okubata_ryoma
  AGBバックアップライブラリのSDK収録のための変更

  Revision 1.23  2006/02/22 02:27:10  kitase_hirotake
  前回の修正を戻す(デフォルトで terminate を行わないように修正)

  Revision 1.22  2006/02/13 00:25:27  kitase_hirotake
  デフォルトで terminate を行わないように修正

  Revision 1.21  2006/02/10 02:53:18  yosizaki
  support memory protection of cartridge access.

  Revision 1.20  2006/02/08 00:04:25  kitase_hirotake
  Warning 修正

  Revision 1.19  2006/02/07 08:13:09  kitase_hirotake
  CTRDG_CheckPulledOut 追加

  Revision 1.18  2006/01/18 02:11:20  kitase_hirotake
  do-indent

  Revision 1.17  2005/11/01 02:57:45  okubata_ryoma
  CTRDG_SendToARM7の追加

  Revision 1.16  2005/06/21 02:46:40  yada
  insert calling cache function

  Revision 1.15  2005/02/28 05:26:03  yosizaki
  do-indent.

  Revision 1.14  2005/01/13 01:58:51  yada
  number of using DMA changed. DMA0 -> DMA1

  Revision 1.13  2005/01/12 11:09:04  takano_makoto
  fix copyright header.

  Revision 1.12  2005/01/12 00:41:38  yada
  change a little, around an auto variable

  Revision 1.11  2004/11/24 04:38:18  yada
  just fix typo

  Revision 1.10  2004/09/28 05:06:15  yada
  fix a little

  Revision 1.9  2004/09/27 13:46:42  yada
  only arrange comments

  Revision 1.8  2004/09/17 00:07:27  yada
  avoid warning by overriding pxi callback

  Revision 1.7  2004/09/16 04:15:24  yada
  fix a little

  Revision 1.6  2004/09/15 09:50:12  yada
  on default, pulling out cartridge doesn't cause terminate.

  Revision 1.5  2004/09/15 05:55:46  yada
  change include

  Revision 1.4  2004/09/14 13:07:26  yada
  fix small bug

  Revision 1.3  2004/09/14 13:01:50  yada
  CallbackForPulledOut->PulledOutCallback

  Revision 1.2  2004/09/14 10:52:30  yada
  fix address data sent to ARM7 via PXI

  Revision 1.1  2004/09/14 09:58:21  yada
  initial release

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include <../include/ctrdg_work.h>

extern void CTRDGi_InitCommon(void);
extern void CTRDGi_SendtoPxi(u32 data);

//---- pxi callbacks
static void CTRDGi_CallbackForInitModuleInfo(PXIFifoTag tag, u32 data, BOOL err);
static void CTRDGi_PulledOutCallback(PXIFifoTag tag, u32 data, BOOL err);
static void CTRDGi_CallbackForSetPhi(PXIFifoTag tag, u32 data, BOOL err);

extern CTRDGWork CTRDGi_Work;

//---- lock for pxi
static int CTRDGi_Lock = FALSE;

//---- user callback
CTRDGPulledOutCallback CTRDG_UserCallback = NULL;

/* カートリッジ抜けコールバックは 2 度呼ばれない */
static BOOL isCartridgePullOut = FALSE;
static BOOL skipCheck = FALSE;
static BOOL ctrdg_already_pullout = FALSE;

//================================================================================
//            INIT
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         CTRDG_Init

  Description:  initialize cartridge functions

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void CTRDG_Init(void)
{
    static BOOL isInitialized;

    if (isInitialized)
    {
        return;
    }
    isInitialized = TRUE;

    CTRDGi_InitCommon();

    /* カートリッジ抜けコールバックが 2 度以上呼ばれないように */
    ctrdg_already_pullout = FALSE;

#ifndef SDK_SMALL_BUILD
    //---- setting PXI
    PXI_Init();
    while (!PXI_IsCallbackReady(PXI_FIFO_TAG_CTRDG, PXI_PROC_ARM7))
    {
    }
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_CTRDG, CTRDGi_CallbackForInitModuleInfo);

    CTRDGi_InitModuleInfo();

    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_CTRDG, NULL);  // to avoid warning by overriding
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_CTRDG, CTRDGi_PulledOutCallback);

    //---- init user callback
    CTRDG_UserCallback = NULL;

    // AGBBackupアクセス関数の非同期版を使用するためのTaskThread作成
    {
        static CTRDGiTaskWork CTRDGTaskList;
        CTRDGi_InitTaskThread(&CTRDGTaskList);
    }

    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_CTRDG_PHI, CTRDGi_CallbackForSetPhi);

    /*
     * ユーザが明示的に enable を指定しない限りアクセス無効.
     * (オプションカートリッジでないなら書き込みアクセスも禁止)
     */
#if defined(SDK_ARM9)
    CTRDG_Enable(FALSE);
#endif
#endif // SDK_SMALL_BUILD
}

/*---------------------------------------------------------------------------*
  Name:         CTRDGi_InitModuleInfo

  Description:  initialize the information of peripheral device around cartridge

                Because try lock cartridge, in case another processor had locked,
                wait till its was released.

                To avoid other bus master interrupts,
                DMA1 is used for accessing cartridge.
                This is a compatibility in future.

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
static CTRDGHeader headerBuf ATTRIBUTE_ALIGN(32);

void CTRDGi_InitModuleInfo(void)
{
#ifndef SDK_SMALL_BUILD

    static BOOL isInitialized;
    CTRDGLockByProc lockInfo;
    OSIrqMask lastIE;
    BOOL    lastIME;

    if (isInitialized)
    {
        return;
    }
    isInitialized = TRUE;

    if (!(reg_OS_PAUSE & REG_OS_PAUSE_CHK_MASK))
    {
        return;
    }

    lastIE = OS_SetIrqMask(OS_IE_SPFIFO_RECV);
    lastIME = OS_EnableIrq();

    //---- get privilege for accessing cartridge
    CTRDGi_LockByProcessor(CTRDGi_Work.lockID, &lockInfo);

    //---- read information of peripheral devices
    {
        MIProcessor proc = MI_GetMainMemoryPriority();
        CTRDGRomCycle rc;

        // set the latest access cycle
        CTRDGi_ChangeLatestAccessCycle(&rc);

        // set main memory priority for avoiding DMA stole by ARM7 access
        MI_SetMainMemoryPriority(MI_PROCESSOR_ARM9);

        // 16bit access in order to have much enough access interval
        DC_InvalidateRange(&((u8 *)&headerBuf)[0x80], sizeof(headerBuf) - 0x80);
        MI_DmaCopy16(1, (void *)(HW_CTRDG_ROM + 0x80),
                     &((u8 *)&headerBuf)[0x80], sizeof(headerBuf) - 0x80);

        // restore main memory priority
        MI_SetMainMemoryPriority(proc);

        // restore access cycle
        CTRDGi_RestoreAccessCycle(&rc);
    }

    //---- release privilege for accessing cartridge
    CTRDGi_UnlockByProcessor(CTRDGi_Work.lockID, &lockInfo);

    // 前回チェックでカートリッジが挿入されていた場合、または
    // ソフトリセットが一度も実行されていない場合のみカートリッジデータを更新する
    if ((*(u8 *)HW_IS_CTRDG_EXIST) || !(*(u8 *)HW_SET_CTRDG_MODULE_INFO_ONCE))
    {
        //---- copy the information of peripheral devices to system area
        int     i;
        CTRDGHeader *chb = &headerBuf;
        CTRDGModuleInfo *cip = CTRDGi_GetModuleInfoAddr();

        cip->moduleID.raw = chb->moduleID;
        for (i = 0; i < 3; i++)
        {
            cip->exLsiID[i] = chb->exLsiID[i];
        }
        cip->makerCode = chb->makerCode;
        cip->gameCode = chb->gameCode;

        // カートリッジが挿入されているのかチェック
        *(u8 *)HW_IS_CTRDG_EXIST = (u8)((CTRDG_IsExisting())? 1 : 0);
        // カートリッジの情報が一回でも更新されればTRUE
        (*(u8 *)HW_SET_CTRDG_MODULE_INFO_ONCE) = TRUE;
    }

    //---- copy NINTENDO logo data in the ARM9 system ROM to main memory
    MI_CpuCopy32((void *)CTRDG_SYSROM9_NINLOGO_ADR, &headerBuf.nintendoLogo,
                 sizeof(headerBuf.nintendoLogo));
    DC_FlushAll();

    //---- send message to ARM7
    CTRDGi_SendtoPxi(CTRDG_PXI_COMMAND_INIT_MODULE_INFO |
                     (((u32)&headerBuf - HW_MAIN_MEM) >> 5) << CTRDG_PXI_COMMAND_PARAM_SHIFT);

    //---- wait till ARM7 finish initialize
    while (CTRDGi_Work.subpInitialized != TRUE)
    {
        SVC_WaitByLoop(1);
    }

    (void)OS_RestoreIrq(lastIME);
    (void)OS_SetIrqMask(lastIE);

#endif // SDK_SMALL_BUILD
}

/*---------------------------------------------------------------------------*
  Name:         CTRDGi_CallbackForInitModuleInfo

  Description:  callback to receive data from PXI

  Arguments:    tag  : tag from PXI (unused)
                data : data from PXI
                err  : error bit (unused)

  Returns:      None
 *---------------------------------------------------------------------------*/
static void CTRDGi_CallbackForInitModuleInfo(PXIFifoTag tag, u32 data, BOOL err)
{
#pragma unused( tag, err )

    if ((data & CTRDG_PXI_COMMAND_MASK) == CTRDG_PXI_COMMAND_INIT_MODULE_INFO)
    {
        CTRDGi_Work.subpInitialized = TRUE;
    }
    else
    {
#ifndef SDK_FINALROM
        OS_Panic("illegal Cartridge pxi command.");
#else
        OS_Panic("");
#endif
    }
}

//================================================================================
//            PULLOUT PROCS
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         CTRDGi_PulledOutCallback

  Description:  callback to receive data from PXI

  Arguments:    tag  : tag from PXI (unused)
                data : data from PXI
                err  : error bit (unused)

  Returns:      None
 *---------------------------------------------------------------------------*/
static void CTRDGi_PulledOutCallback(PXIFifoTag tag, u32 data, BOOL err)
{
#pragma unused( tag, err )

    //---- receive message 'pulled out'
    if ((data & CTRDG_PXI_COMMAND_MASK) == CTRDG_PXI_COMMAND_PULLED_OUT)
    {
        if (ctrdg_already_pullout == FALSE)
        {
            BOOL    isTerminateImm = FALSE;

            //---- call user callback
            if (CTRDG_UserCallback)
            {
                isTerminateImm = CTRDG_UserCallback();
            }

            //---- terminate
            if (isTerminateImm)
            {
                CTRDG_TerminateForPulledOut();
            }
            /* 一度呼ばれたら、もう呼ばれない */
            ctrdg_already_pullout = TRUE;
        }
    }
    else
    {
        OS_Printf(">>>tag[%x] data[%x] err[%x]\n", tag, data, err);
#ifndef SDK_FINALROM
        OS_Panic("illegal Cartridge pxi command.");
#else
        OS_Panic("");
#endif
    }
}

/*---------------------------------------------------------------------------*
  Name:         CTRDG_SetPulledOutCallback

  Description:  set user callback for being pulled out cartridge

  Arguments:    callback : callback

  Returns:      None
 *---------------------------------------------------------------------------*/
void CTRDG_SetPulledOutCallback(CTRDGPulledOutCallback callback)
{
    CTRDG_UserCallback = callback;
}

/*---------------------------------------------------------------------------*
  Name:         CTRDG_TerminateForPulledOut

  Description:  terminate for pulling out cartridge.
                send message to do termination to ARM7

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void CTRDG_TerminateForPulledOut(void)
{
    //---- send 'TERMINATE' command to ARM7, and terminate itself immediately
    CTRDGi_SendtoPxi(CTRDG_PXI_COMMAND_TERMINATE);

    OS_Terminate();
}

/*---------------------------------------------------------------------------*
  Name:         CTRDG_SendToARM7

  Description:  send data to ARM7

  Arguments:    arg  : data to send

  Returns:      None
 *---------------------------------------------------------------------------*/
void CTRDG_SendToARM7(void *arg)
{
    (void)PXI_SendWordByFifo(PXI_FIFO_TAG_CTRDG_Ex, (u32)arg, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CTRDG_CheckPulledOut

  Description:  cartridge is pulled out

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void CTRDG_CheckPulledOut(void)
{
    //---- if skip flag  or  cartridge already pulled out.
    if (skipCheck || isCartridgePullOut)
    {
        return;
    }

    //---------------- check cartridge pulled out
    //---- check cartridge
    isCartridgePullOut = CTRDG_IsPulledOut();

    //---- if no cartridge...
    if (!CTRDG_IsExisting())
    {
        if (!isCartridgePullOut)
        {
            skipCheck = TRUE;
            return;
        }
    }

    //---------------- if cartridge pulled out, tell that to ARM9
    if (isCartridgePullOut)
    {
        /* ARM7 から PXI で呼んでいた関数を同じく呼ぶ */
        /* 引数は処理がされる場合の引数に強制的にした */
        CTRDGi_PulledOutCallback(PXI_FIFO_TAG_CTRDG, CTRDG_PXI_COMMAND_PULLED_OUT, NULL);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CTRDG_SetPhiClock

  Description:  set ARM9 and ARM7 PHI output clock

  Arguments:    clock :    PHI clock to set. (MIPhiClock value)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CTRDG_SetPhiClock(CTRDGPhiClock clock)
{
    u32     data = ((u32)clock << CTRDG_PXI_COMMAND_PARAM_SHIFT) | CTRDG_PXI_COMMAND_SET_PHI;

    //---- check parameter range
    SDK_ASSERT((u32)clock <= CTRDG_PHI_CLOCK_16MHZ);

    //---- set ARM9 PHI output clock
    MIi_SetPhiClock((MIiPhiClock) clock);

    //---- send command to set ARM7 phi clock
    CTRDGi_Lock = TRUE;
    while (PXI_SendWordByFifo(PXI_FIFO_TAG_CTRDG_PHI, data, FALSE) != PXI_FIFO_SUCCESS)
    {
        SVC_WaitByLoop(1);
    }

    //---- wait response
    while (CTRDGi_Lock)
    {
        SVC_WaitByLoop(1);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CTRDG_CallbackForSetPhi

  Description:  callback from ARM7 to receive PHI clock setting

  Arguments:    tag  : tag from PXI (unused)
                data : data from PXI
                err  : error bit (unused)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CTRDGi_CallbackForSetPhi(PXIFifoTag tag, u32 data, BOOL err)
{
#pragma unused(tag, data, err)
    CTRDGi_Lock = FALSE;
}
