/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CTRDG
  File:     ctrdg.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ctrdg.c,v $
  Revision 1.24  2007/02/20 00:28:11  kitase_hirotake
  indent source

  Revision 1.23  2006/04/24 00:05:09  okubata_ryoma
  CW2.0ÇÃåxçêÉåÉxÉãã≠âªÇ…î∫Ç¡ÇΩïœçX

  Revision 1.22  2006/02/22 04:06:44  kitase_hirotake
  ñ≥êßå¿Ç…ÉçÉbÉNë“ÇøÇµÇ»Ç¢ÇÊÇ§Ç…èCê≥

  Revision 1.21  2006/02/10 02:53:18  yosizaki
  support memory protection of cartridge access.

  Revision 1.20  2006/01/18 02:11:20  kitase_hirotake
  do-indent

  Revision 1.19  2005/11/30 12:22:42  yosizaki
  add CTRDG_IsEnabled(), CTRDG_Enable(), CTRDG_CheckEnabled().

  Revision 1.18  2005/04/12 06:22:51  yosizaki
  add pulled-out checkers.

  Revision 1.17  2005/02/28 05:26:03  yosizaki
  do-indent.

  Revision 1.16  2005/02/18 14:19:10  seiki_masashi
  Copyright ï\ãLÇÃèCê≥

  Revision 1.15  2005/02/18 13:17:08  seiki_masashi
  warning ëŒçÙ

  Revision 1.14  2004/09/29 02:32:59  yada
  consider for exchanging a option cartridge and an AGB cartridge in sleeping

  Revision 1.13  2004/09/27 13:46:04  yada
  only arrange spacing

  Revision 1.12  2004/09/21 01:31:23  yada
  in CTRDG_CommonInit(), call SVC_CpuClear instead of MI_CpuClear.

  Revision 1.11  2004/09/17 12:25:18  yada
  add data forwarding functions

  Revision 1.10  2004/09/17 06:43:50  yada
  add line in CTRDG_CpuCopy8()

  Revision 1.9  2004/09/17 05:11:50  yada
  add CTRDG_IsOptionCartridge()

  Revision 1.8  2004/09/16 09:22:20  yada
  fix a little

  Revision 1.7  2004/09/16 04:11:48  yada
  OS_IsExisting() skip process after detect pulled out

  Revision 1.6  2004/09/14 11:56:49  yada
  only write comment about CTRDGi_InitCommon

  Revision 1.5  2004/09/14 09:59:17  yada
  move ARM9 code to ctrdg_proc.c

  Revision 1.4  2004/09/14 08:52:43  yada
  separate callback to 2 function

  Revision 1.3  2004/09/14 07:23:45  yada
  fix a little

  Revision 1.2  2004/09/14 06:09:44  yada
  check cartridge pulled out in ARM7

  Revision 1.1  2004/09/13 10:45:14  yada
  initial release.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/ctrdg.h>
#include <../include/ctrdg_work.h>
#include <nitro/os/ARM9/cache.h>

//----------------------------------------------------------------------------

CTRDGWork CTRDGi_Work;

/* cartridge permission */
static BOOL CTRDGi_EnableFlag = FALSE;


/*---------------------------------------------------------------------------*
  Name:         CTRDGi_InitCommon

  Description:  common routine called from CTRDG_Init.
                keep lockID for cartridge functions.

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void CTRDGi_InitCommon(void)
{
    SVC_CpuClear(0, &CTRDGi_Work, sizeof(CTRDGi_Work), 32);     // use SVC_* intentionally.

    CTRDGi_Work.lockID = (u16)OS_GetLockID();
}

/*---------------------------------------------------------------------------*
  Name:         CTRDG_IsBitID

  Description:  return whether peripheral device which is specified by bitID exists
                in cartridge

  Arguments:    bitID  bit ID

  Returns:      TRUE if existed
 *---------------------------------------------------------------------------*/

BOOL CTRDG_IsBitID(u8 bitID)
{
    return (CTRDG_IsExisting() && CTRDGi_IsBitIDAtInit(bitID));
}

/*---------------------------------------------------------------------------*
  Name:         CTRDGi_IsBitIDAtInit

  Description:  return whether peripheral device which is specified by bitID existed
                in cartridge at boot time

  Arguments:    bitID  bit ID

  Returns:      TRUE if existed
 *---------------------------------------------------------------------------*/
BOOL CTRDGi_IsBitIDAtInit(u8 bitID)
{
    BOOL    retval = FALSE;

    CTRDGModuleInfo *cip = CTRDGi_GetModuleInfoAddr();

    if (cip->moduleID.raw != 0xffff && cip->moduleID.raw != 0x0000 && ~cip->moduleID.bitID & bitID)
    {
        retval = TRUE;
    }

    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         CTRDG_IsNumberID

  Description:  return whether peripheral device which is specified by numberID exists
                in cartridge

  Arguments:    numberID  number ID

  Returns:      TRUE if existed
 *---------------------------------------------------------------------------*/
BOOL CTRDG_IsNumberID(u8 numberID)
{
    return (CTRDG_IsExisting() && CTRDGi_IsNumberIDAtInit(numberID));
}

/*---------------------------------------------------------------------------*
  Name:         CTRDGi_IsNumberIDAtInit

  Description:  return whether peripheral device which is specified by numberID existed
                in cartridge at boot time

  Arguments:    numberID  number ID

  Returns:      TRUE if existed
 *---------------------------------------------------------------------------*/
BOOL CTRDGi_IsNumberIDAtInit(u8 numberID)
{
    BOOL    retval = FALSE;

    CTRDGModuleInfo *cip = CTRDGi_GetModuleInfoAddr();

    if (cip->moduleID.raw != 0xffff && cip->moduleID.raw != 0x0000)
    {
        if (cip->moduleID.numberID == numberID)
        {
            retval = TRUE;
        }
        else if (!cip->moduleID.disableExLsiID)
        {
            if (cip->exLsiID[0] == numberID
                || cip->exLsiID[1] == numberID || cip->exLsiID[2] == numberID)
            {
                retval = TRUE;
            }
        }
    }

    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         CTRDG_IsAgbCartridge

  Description:  return whether AGB cartridge exists

  Arguments:    None

  Returns:      TRUE if existed
 *---------------------------------------------------------------------------*/
BOOL CTRDG_IsAgbCartridge(void)
{
    return (CTRDG_IsExisting() && CTRDGi_IsAgbCartridgeAtInit());
}

/*---------------------------------------------------------------------------*
  Name:         CTRDG_IsOptionCartridge

  Description:  return whether option cartridge exists

  Arguments:    None

  Returns:      TRUE if existed
 *---------------------------------------------------------------------------*/
BOOL CTRDG_IsOptionCartridge(void)
{
    return (CTRDG_IsExisting() && !CTRDGi_IsAgbCartridgeAtInit());
}

/*---------------------------------------------------------------------------*
  Name:         CTRDGi_IsAgbCartridgeAtInit

  Description:  return whether AGB cartridge existed at boot time

  Arguments:    None

  Returns:      TRUE if existed
 *---------------------------------------------------------------------------*/
BOOL CTRDGi_IsAgbCartridgeAtInit(void)
{
    CTRDGModuleInfo *cip = CTRDGi_GetModuleInfoAddr();

    return cip->isAgbCartridge;
}

/*---------------------------------------------------------------------------*
  Name:         CTRDG_GetAgbGameCode

  Description:  get game code in AGB cartridge

  Arguments:    None

  Returns:      Game code if exist, FALSE if cartridge not exist
 *---------------------------------------------------------------------------*/
u32 CTRDG_GetAgbGameCode(void)
{
    u32     retval = FALSE;

    if (CTRDG_IsExisting())
    {
        retval = CTRDGi_GetAgbGameCodeAtInit();
    }

    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         CTRDGi_GetAgbGameCodeAtInit

  Description:  get game code in AGB cartridge read at boot time

  Arguments:    None

  Returns:      Game code if exist, FALSE if cartridge did not exist
 *---------------------------------------------------------------------------*/
u32 CTRDGi_GetAgbGameCodeAtInit(void)
{
    CTRDGModuleInfo *cip = CTRDGi_GetModuleInfoAddr();
    u32     retval = FALSE;

    if (CTRDGi_IsAgbCartridgeAtInit())
    {
        retval = cip->gameCode;
    }

    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         CTRDG_GetAgbMakerCode

  Description:  get maker code in AGB cartridge

  Arguments:    None

  Returns:      Maker code if exist, FALSE if cartridge not exist
 *---------------------------------------------------------------------------*/
u16 CTRDG_GetAgbMakerCode(void)
{
    u16     retval = FALSE;

    if (CTRDG_IsExisting())
    {
        retval = CTRDGi_GetAgbMakerCodeAtInit();
    }

    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         CTRDGi_GetAgbMakerCodeAtInit

  Description:  get maker code in AGB cartridge read at boot time

  Arguments:    None

  Returns:      Maker code if exist, FALSE if not exist
 *---------------------------------------------------------------------------*/
u16 CTRDGi_GetAgbMakerCodeAtInit(void)
{
    CTRDGModuleInfo *cip = CTRDGi_GetModuleInfoAddr();
    u16     retval = FALSE;

    if (CTRDGi_IsAgbCartridgeAtInit())
    {
        retval = cip->makerCode;
    }

    return retval;
}


/*---------------------------------------------------------------------------*
  Name:         CTRDG_IsPulledOut

  Description:  get whether system has detected pulled out cartridge

  Arguments:    None

  Returns:      TRUE if detect pull out
 *---------------------------------------------------------------------------*/
BOOL CTRDG_IsPulledOut(void)
{
    CTRDGModuleInfo *cip = CTRDGi_GetModuleInfoAddr();

    //---- cartridge not exist at boot time
    if (cip->moduleID.raw == 0xffff)
    {
        return FALSE;
    }

    //---- check existing when not detect pulled out
    if (!cip->detectPullOut)
    {
        (void)CTRDG_IsExisting();
    }

    return cip->detectPullOut;
}

/*---------------------------------------------------------------------------*
  Name:         CTRDG_IsAgbCartridgePulledOut

  Description:  get whether system has detected pulled out AGB cartridge

  Arguments:    None

  Returns:      TRUE if detect pull out
 *---------------------------------------------------------------------------*/
BOOL CTRDG_IsAgbCartridgePulledOut(void)
{
    return (CTRDG_IsPulledOut() && CTRDGi_IsAgbCartridgeAtInit());
}

/*---------------------------------------------------------------------------*
  Name:         CTRDG_IsOptionCartridgePulledOut

  Description:  get whether system has detected pulled out option cartridge

  Arguments:    None

  Returns:      TRUE if detect pull out
 *---------------------------------------------------------------------------*/
BOOL CTRDG_IsOptionCartridgePulledOut(void)
{
    return (CTRDG_IsPulledOut() && !CTRDGi_IsAgbCartridgeAtInit());
}

/*---------------------------------------------------------------------------*
  Name:         CTRDG_IsExisting

  Description:  get whether cartridge exists

  Arguments:    None

  Returns:      TRUE if existing
 *---------------------------------------------------------------------------*/
BOOL CTRDG_IsExisting(void)
{
    BOOL    retval = TRUE;
    CTRDGLockByProc lockInfo;

    CTRDGHeader *chp = CTRDGi_GetHeaderAddr();
    CTRDGModuleInfo *cip = CTRDGi_GetModuleInfoAddr();

    //---- if cartridge not exist at boot time.
    if (cip->moduleID.raw == 0xffff)
    {
        return FALSE;
    }

    //---- if detect cartirdge pulled out
    if (cip->detectPullOut == TRUE)
    {
        return FALSE;
    }
#if defined(SDK_ARM7)
    //---- get privilege for accessing cartridge
    if (CTRDGi_LockByProcessor(CTRDGi_Work.lockID, &lockInfo) == FALSE)
    {
        (void)OS_RestoreInterrupts(lockInfo.irq);
        return TRUE;
    }
#else
    //---- get privilege for accessing cartridge
    CTRDGi_LockByProcessor(CTRDGi_Work.lockID, &lockInfo);
#endif

    //---- check if cartridge exists
    {
        CTRDGRomCycle rc;
        u8      isRomCode;

        // set the lastest access cycle
        CTRDGi_ChangeLatestAccessCycle(&rc);
        isRomCode = chp->isRomCode;

        // ( please observe comparison order to the following )
        if ((isRomCode == CTRDG_IS_ROM_CODE && cip->moduleID.raw != chp->moduleID)      // memory loaded
            || (isRomCode != CTRDG_IS_ROM_CODE && cip->moduleID.raw != *CTRDGi_GetModuleIDImageAddr())  // memory not loaded
            || ((cip->gameCode != chp->gameCode) && cip->isAgbCartridge))       // AGB cartridge comparison
        {
            cip->detectPullOut = TRUE;
            retval = FALSE;
        }

        //---- restore access cycle
        CTRDGi_RestoreAccessCycle(&rc);
    }

    //---- release privilege for accessing cartridge
    CTRDGi_UnlockByProcessor(CTRDGi_Work.lockID, &lockInfo);

    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         CTRDGi_ChangeLatestAccessCycle

  Description:  set access cycle to cartridge to latest setting

  Arguments:    r :  Cartridge ROM access cycle

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CTRDGi_ChangeLatestAccessCycle(CTRDGRomCycle *r)
{
    r->c1 = MI_GetCartridgeRomCycle1st();
    r->c2 = MI_GetCartridgeRomCycle2nd();

    MI_SetCartridgeRomCycle1st(MI_CTRDG_ROMCYCLE1_18);
    MI_SetCartridgeRomCycle2nd(MI_CTRDG_ROMCYCLE2_6);
}

/*---------------------------------------------------------------------------*
  Name:         CTRDGi_RestoreAccessCycle

  Description:  set access cycle to cartridge to the original setting

  Arguments:    r :  Cartridge ROM access cycle

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CTRDGi_RestoreAccessCycle(CTRDGRomCycle *r)
{
    MI_SetCartridgeRomCycle1st(r->c1);
    MI_SetCartridgeRomCycle2nd(r->c2);
}


/*---------------------------------------------------------------------------*
  Name:         CTRDGi_LockByProcessor

  Description:  get privilege for accessing cartridge to specified processor

                Because try lock cartridge, in case another processor had locked,
                wait till its was released.

                Status of interrupt in return is disable.

  Arguments:    lockID : lock ID for cartridge
                info   : info for lock by my processor

  Returns:      None.
 *---------------------------------------------------------------------------*/
#if defined(SDK_ARM7)
BOOL CTRDGi_LockByProcessor(u16 lockID, CTRDGLockByProc *info)
#else
void CTRDGi_LockByProcessor(u16 lockID, CTRDGLockByProc *info)
#endif
{
    while (1)
    {
        info->irq = OS_DisableInterrupts();

        if (((info->locked = OS_ReadOwnerOfLockCartridge() & CTRDG_LOCKED_BY_MYPROC_FLAG) != 0)
            || (OS_TryLockCartridge(lockID) == OS_LOCK_SUCCESS))
        {
#if defined(SDK_ARM7)
            return TRUE;
#else
            break;
#endif
        }
#if defined(SDK_ARM7)
        return FALSE;
#endif
        (void)OS_RestoreInterrupts(info->irq);

        SVC_WaitByLoop(1);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CTRDGi_UnlockByProcessor

  Description:  release privilege for accessing cartirige 

  Arguments:    lockID : lock ID for cartridge
                info   : info for lock by my processor
                         it must be the value got in CTRDGi_LockCartridgeByProccesser.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CTRDGi_UnlockByProcessor(u16 lockID, CTRDGLockByProc *info)
{
    if (!info->locked)
    {
        (void)OS_UnLockCartridge(lockID);
    }

    (void)OS_RestoreInterrupts(info->irq);
}

/*---------------------------------------------------------------------------*
  Name:         CTRDGi_SendtoPxi

  Description:  send data via PXI

  Arguments:    data : data to send

  Returns:      None
 *---------------------------------------------------------------------------*/
void CTRDGi_SendtoPxi(u32 data)
{
    while (PXI_SendWordByFifo(PXI_FIFO_TAG_CTRDG, data, FALSE) != PXI_FIFO_SUCCESS)
    {
        SVC_WaitByLoop(1);
    }
}

//================================================================================
//       READ DATA FROM CARTRIDGE
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         CTRDG_DmaCopy16 / 32

  Description:  read cartridge data via DMA

  Arguments:    dmaNo : DMA No.
                src   : source address (in cartridge)
                dest  : destination address (in memory)
                size  : forwarding size

  Returns:      TRUE if success.
                FALSE if fail. ( no cartridge )
 *---------------------------------------------------------------------------*/
BOOL CTRDG_DmaCopy16(u32 dmaNo, const void *src, void *dest, u32 size)
{
    return CTRDGi_CopyCommon(dmaNo, src, dest, size, CTRDGi_FORWARD_DMA16);
}
BOOL CTRDG_DmaCopy32(u32 dmaNo, const void *src, void *dest, u32 size)
{
    return CTRDGi_CopyCommon(dmaNo, src, dest, size, CTRDGi_FORWARD_DMA32);
}

/*---------------------------------------------------------------------------*
  Name:         CTRDG_CpuCopy8 / 16 / 32

  Description:  read cartridge data by CPU access

  Arguments:    src   : source address (in cartridge)
                dest  : destination address (in memory)
                size  : forwarding size

  Returns:      TRUE if success.
                FALSE if fail. ( no cartridge )
 *---------------------------------------------------------------------------*/
BOOL CTRDG_CpuCopy8(const void *src, void *dest, u32 size)
{
    if (HW_CTRDG_ROM <= (u32)dest && (u32)dest < HW_CTRDG_RAM_END)
    {
        return CTRDGi_CopyCommon(0 /*dummy */ , (const void *)dest, (void *)src, size,
                                 CTRDGi_FORWARD_CPU8);
    }
    else
    {
        return CTRDGi_CopyCommon(0 /*dummy */ , src, dest, size, CTRDGi_FORWARD_CPU8);
    }
}
BOOL CTRDG_CpuCopy16(const void *src, void *dest, u32 size)
{
    return CTRDGi_CopyCommon(0 /*dummy */ , src, dest, size, CTRDGi_FORWARD_CPU16);
}
BOOL CTRDG_CpuCopy32(const void *src, void *dest, u32 size)
{
    return CTRDGi_CopyCommon(0 /*dummy */ , src, dest, size, CTRDGi_FORWARD_CPU32);
}

/*---------------------------------------------------------------------------*
  Name:         CTRDGi_CopyCommon

  Description:  read/write cartridge data.
                subroutine of CTRDG_CpuCopy*(), CTRDG_DmaCopy*() and CTRDG_WriteStream8().

  Arguments:    dmaNo       : DMA No.
                src         : source address
                dest        : destination address
                size        : forwarding size
                forwardType : action flag

  Returns:      TRUE if success.
                FALSE if fail. ( no cartridge )
 *---------------------------------------------------------------------------*/
BOOL CTRDGi_CopyCommon(u32 dmaNo, const void *src, void *dest, u32 size, u32 forwardType)
{
    //---- check cartridge existence
    if (!CTRDG_IsExisting())
    {
        return FALSE;
    }

    /* confirm whether application has certainly judged AGB-cartridge */
    CTRDG_CheckEnabled();

    (void)OS_LockCartridge(CTRDGi_Work.lockID);

    if ((forwardType & CTRDGi_FORWARD_TYPE_MASK) == CTRDGi_FORWARD_TYPE_DMA)
    {
        MI_StopDma(dmaNo);
        DC_FlushRange(dest, size);
    }

    switch (forwardType)
    {
    case CTRDGi_FORWARD_DMA16:
        MI_DmaCopy16(dmaNo, src, dest, size);
        break;
    case CTRDGi_FORWARD_DMA32:
        MI_DmaCopy32(dmaNo, src, dest, size);
        break;
    case CTRDGi_FORWARD_CPU16:
        MI_CpuCopy16(src, dest, size);
        break;
    case CTRDGi_FORWARD_CPU32:
        MI_CpuCopy32(src, dest, size);
        break;

    case CTRDGi_FORWARD_CPU8:
        {
            int     n;
            u8     *dest8 = (u8 *)dest;
            u8     *src8 = (u8 *)src;
            for (n = 0; n < size; n++)
            {
                *dest8++ = *src8++;
            }
        }
        break;
    }

    (void)OS_UnLockCartridge(CTRDGi_Work.lockID);

    //---- check to remove cartridge in the middle of reading
    if (!CTRDG_IsExisting())
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         CTRDG_Read8 / 16 / 32

  Description:  read cartridge data by CPU access

  Arguments:    address  : source address (in cartridge)
                rdata    : address to store read data

  Returns:      TRUE if success.
                FALSE if fail. ( no cartridge )
 *---------------------------------------------------------------------------*/
BOOL CTRDG_Read8(const u8 *address, u8 *rdata)
{
    return CTRDGi_AccessCommon((void *)address, 0 /*dummy */ , rdata, CTRDGi_ACCESS_READ8);
}
BOOL CTRDG_Read16(const u16 *address, u16 *rdata)
{
    return CTRDGi_AccessCommon((void *)address, 0 /*dummy */ , rdata, CTRDGi_ACCESS_READ16);
}
BOOL CTRDG_Read32(const u32 *address, u32 *rdata)
{
    return CTRDGi_AccessCommon((void *)address, 0 /*dummy */ , rdata, CTRDGi_ACCESS_READ32);
}

/*---------------------------------------------------------------------------*
  Name:         CTRDG_Write8 / 16 / 32

  Description:  write data to cartridge

  Arguments:    address  : destination address (in cartridge)
                data     : data to write

  Returns:      TRUE if success.
                FALSE if fail. ( no cartridge )
 *---------------------------------------------------------------------------*/
BOOL CTRDG_Write8(u8 *address, u8 data)
{
    return CTRDGi_AccessCommon(address, data, 0 /*dummy */ , CTRDGi_ACCESS_WRITE8);
}
BOOL CTRDG_Write16(u16 *address, u16 data)
{
    return CTRDGi_AccessCommon(address, data, 0 /*dummy */ , CTRDGi_ACCESS_WRITE16);
}
BOOL CTRDG_Write32(u32 *address, u32 data)
{
    return CTRDGi_AccessCommon(address, data, 0 /*dummy */ , CTRDGi_ACCESS_WRITE32);
}

/*---------------------------------------------------------------------------*
  Name:         CTRDGi_AccessCommon

  Description:  read/write cartridge data.
                subroutine of CTRDG_Read*() and CTRDG_Write*().

  Arguments:    address    : address to access
                data       : data to write (in write mode)
                rdata      : address to store read data (in read mode)
                accessType : action flag

  Returns:      TRUE if success.
                FALSE if fail. ( no cartridge )
 *---------------------------------------------------------------------------*/
BOOL CTRDGi_AccessCommon(void *address, u32 data, void *rdata, u32 accessType)
{
    //---- check cartridge existence
    if (!CTRDG_IsExisting())
    {
        return FALSE;
    }

    /* confirm whether application has certainly judged AGB-cartridge */
    CTRDG_CheckEnabled();

    (void)OS_LockCartridge(CTRDGi_Work.lockID);

    switch (accessType)
    {
    case CTRDGi_ACCESS_READ8:
        if (rdata)
        {
            *(u8 *)rdata = *(u8 *)address;
        }
        break;
    case CTRDGi_ACCESS_READ16:
        if (rdata)
        {
            *(u16 *)rdata = *(u16 *)address;
        }
        break;
    case CTRDGi_ACCESS_READ32:
        if (rdata)
        {
            *(u32 *)rdata = *(u32 *)address;
        }
        break;
    case CTRDGi_ACCESS_WRITE8:
        *(u8 *)address = (u8)data;
        break;
    case CTRDGi_ACCESS_WRITE16:
        *(u16 *)address = (u16)data;
        break;
    case CTRDGi_ACCESS_WRITE32:
        *(u32 *)address = (u32)data;
        break;
    }

    (void)OS_UnLockCartridge(CTRDGi_Work.lockID);

    //---- check to remove cartridge in the middle of reading
    if (!CTRDG_IsExisting())
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         CTRDG_IsEnabled

  Description:  check if CTRDG is accessable

  Arguments:    None.

  Returns:      Return cartridge access permission.
 *---------------------------------------------------------------------------*/
BOOL CTRDG_IsEnabled(void)
{
    return CTRDGi_EnableFlag;
}

/*---------------------------------------------------------------------------*
  Name:         CTRDG_Enable

  Description:  Set cartridge access permission mode.

  Arguments:    enable       permission mode to be set.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CTRDG_Enable(BOOL enable)
{
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    CTRDGi_EnableFlag = enable;
#if defined(SDK_ARM9)
    if (!CTRDG_IsOptionCartridge())
    {
        u32     acc = (u32)(enable ? OS_PR3_ACCESS_RW : OS_PR3_ACCESS_RO);
        (void)OS_SetDPermissionsForProtectionRegion(OS_PR3_ACCESS_MASK, acc);
    }
#endif /* defined(SDK_ARM9) */
    (void)OS_RestoreInterrupts(bak_cpsr);
}

/*---------------------------------------------------------------------------*
  Name:         CTRDG_CheckEnabled

  Description:  Terminate program if CTRDG is not accessable

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CTRDG_CheckEnabled(void)
{
#ifdef SDK_ARM9
    if (!CTRDG_IsOptionCartridge() && !CTRDG_IsEnabled())
    {
        OS_TPanic
            ("cartridge permission denied. (you must call CTRDG_Enable() under the guideline.)");
    }
#endif
}
