/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CTRDG - libraries - ARM9
  File:     ctrdg_flash_MX29L010.c

  Copyright 2003,2004,2006,2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ctrdg_flash_MX29L010.c,v $
  Revision 1.3  2007/02/20 00:28:11  kitase_hirotake
  indent source

  Revision 1.2  2006/04/07 03:27:28  okubata_ryoma
  small fix

  Revision 1.1  2006/04/05 10:48:30  okubata_ryoma
  AGBバックアップライブラリのSDK収録のための変更


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>

// extern data declaration----------------------
extern u16 CTRDGi_PollingSR1MCOMMON(u16 phase, u8 *adr, u16 lastData);

// function's prototype declaration-------------
u16     CTRDGi_EraseFlashChipMX(void);
u16     CTRDGi_EraseFlashSectorMX(u16 secNo);
u16     CTRDGi_WriteFlashSectorMX(u16 secNo, u8 *src);

u32     CTRDGi_EraseFlashChipCoreMX(CTRDGTaskInfo * arg);
u32     CTRDGi_EraseFlashSectorCoreMX(CTRDGTaskInfo * arg);
u32     CTRDGi_WriteFlashSectorCoreMX(CTRDGTaskInfo * arg);

void    CTRDGi_EraseFlashChipAsyncMX(CTRDG_TASK_FUNC callback);
void    CTRDGi_EraseFlashSectorAsyncMX(u16 secNo, CTRDG_TASK_FUNC callback);
void    CTRDGi_WriteFlashSectorAsyncMX(u16 secNo, u8 *src, CTRDG_TASK_FUNC callback);

// const data-----------------------------------
static const u16 MxMaxTime[] = {
    10,                                // common       10ms
    10,                                // program      10ms(exactly:20usec)
    2000,                              // sector erase 2s
    2000,                              // chip   erase 2s
};

const CTRDGiFlashTypePlus MX29L010 = {
    CTRDGi_WriteFlashSectorMX,
    CTRDGi_EraseFlashChipMX,
    CTRDGi_EraseFlashSectorMX,
    CTRDGi_WriteFlashSectorAsyncMX,
    CTRDGi_EraseFlashChipAsyncMX,
    CTRDGi_EraseFlashSectorAsyncMX,
    CTRDGi_PollingSR1MCOMMON,
    MxMaxTime,
    {
/* デバッグ用 */
//#ifndef   __FLASH_DEBUG
        0x00020000,                       // ROM size
        {0x00001000, 12, 32, 0},          // sector size,shift,count,top
//#else
//      0x00004000,
//      {0x00000200,  9, 32,  0},
//#endif
        {MI_CTRDG_RAMCYCLE_18, MI_CTRDG_RAMCYCLE_8},       // agb read cycle=8, write cycle=3
        0xc2,                             // maker ID
        0x09,                             // device ID
    },
};

const CTRDGiFlashTypePlus defaultFlash1M = {
    CTRDGi_WriteFlashSectorMX,
    CTRDGi_EraseFlashChipMX,
    CTRDGi_EraseFlashSectorMX,
    CTRDGi_WriteFlashSectorAsyncMX,
    CTRDGi_EraseFlashChipAsyncMX,
    CTRDGi_EraseFlashSectorAsyncMX,
    CTRDGi_PollingSR1MCOMMON,
    MxMaxTime,
    {
/* デバッグ用 */
//#ifndef   __FLASH_DEBUG
        0x00020000,                       // ROM size
        {0x00001000, 12, 32, 0},          // sector size,shift,count,top
//#else
//      0x00004000,
//      {0x00000200,  9, 32,  0},
//#endif
        {MI_CTRDG_RAMCYCLE_18, MI_CTRDG_RAMCYCLE_8},       // agb read cycle=8, write cycle=3
        0x00,                             // maker ID
        0x00,                             // device ID
    },
};

/*排他制御*/
extern u16 ctrdgi_flash_lock_id;
extern BOOL ctrdgi_backup_irq;

/*******************************************************

    function's description

********************************************************/
u32 CTRDGi_EraseFlashChipCoreMX(CTRDGTaskInfo * arg)
{
/* デバッグ用 */
//#ifdef    __FLASH_DEBUG
//  u32 i;
//#endif
    MICartridgeRamCycle ram_cycle;
    u32     result;
    (void)arg;

    /*排他制御（ロック） */
    (void)OS_LockCartridge(ctrdgi_flash_lock_id);
    /*アクセスサイクル設定 */
    ram_cycle = MI_GetCartridgeRamCycle();
    MI_SetCartridgeRamCycle(AgbFlash->agbWait[0]);

//    *(vu16 *)REG_EXMEMCNT_ADDR=(*(vu16 *)REG_EXMEMCNT_ADDR & 0xfffc)|AgbFlash->agbWait[0]; // read 3cycles wait
    ctrdgi_backup_irq = OS_DisableIrq();

    *(vu8 *)CTRDG_BACKUP_COM_ADR1 = 0xaa;
    *(vu8 *)CTRDG_BACKUP_COM_ADR2 = 0x55;
    *(vu8 *)CTRDG_BACKUP_COM_ADR1 = 0x80;
    *(vu8 *)CTRDG_BACKUP_COM_ADR1 = 0xaa;
    *(vu8 *)CTRDG_BACKUP_COM_ADR2 = 0x55;
    *(vu8 *)CTRDG_BACKUP_COM_ADR1 = 0x10;
/* デバッグ用 */
//#ifdef    __FLASH_DEBUG
//  adr=(u8 *)CTRDG_AGB_FLASH_ADR;
//  for(i=0;i<AgbFlash->romSize;i++)
//      *adr++=0xff;
//#endif
    (void)OS_RestoreIrq(ctrdgi_backup_irq);

    result = CTRDGi_PollingSR(CTRDG_BACKUP_PHASE_CHIP_ERASE, (u8 *)CTRDG_AGB_FLASH_ADR, 0xff);
//  *(vu16 *)REG_EXMEMCNT_ADDR=(*(vu16 *)REG_EXMEMCNT_ADDR & 0xfffc)| 1;

    /*アクセスサイクル設定 */
    MI_SetCartridgeRamCycle(ram_cycle);
    /*排他制御（アンロック） */
    (void)OS_UnlockCartridge(ctrdgi_flash_lock_id);

    return result;
}

u32 CTRDGi_EraseFlashSectorCoreMX(CTRDGTaskInfo * arg)
{
    u8     *adr;
    u16     retry;
    MICartridgeRamCycle ram_cycle;
    u32     result;
    CTRDGTaskInfo p = *arg;
    u16     secNo = p.sec_num;

    if (secNo >= AgbFlash->sector.count)
        return CTRDG_BACKUP_RESULT_ERROR | CTRDG_BACKUP_PHASE_PARAMETER_CHECK;

    /*排他制御（ロック） */
    (void)OS_LockCartridge(ctrdgi_flash_lock_id);
    /*アクセスサイクル設定 */
    ram_cycle = MI_GetCartridgeRamCycle();
    MI_SetCartridgeRamCycle(AgbFlash->agbWait[0]);

    // バンクのセット
    CTRDGi_SetFlashBankMx((u16)(secNo >> 4));
    secNo &= 0x0f;

    retry = 0;

  erase_again:
//    *(vu16 *)REG_EXMEMCNT_ADDR=(*(vu16 *)REG_EXMEMCNT_ADDR & 0xfffc)|AgbFlash->agbWait[0]; // read 3cycles wait
    adr = (u8 *)(CTRDG_AGB_FLASH_ADR + (secNo << AgbFlash->sector.shift));
    ctrdgi_backup_irq = OS_DisableIrq();

    *(vu8 *)CTRDG_BACKUP_COM_ADR1 = 0xaa;
    *(vu8 *)CTRDG_BACKUP_COM_ADR2 = 0x55;
    *(vu8 *)CTRDG_BACKUP_COM_ADR1 = 0x80;
    *(vu8 *)CTRDG_BACKUP_COM_ADR1 = 0xaa;
    *(vu8 *)CTRDG_BACKUP_COM_ADR2 = 0x55;
    *(vu8 *)adr = 0x30;
/* デバッグ用 */
//#ifdef    __FLASH_DEBUG
//  for(i=0;i<AgbFlash->sector.size;i++)
//      *adr++=0xff;
//  adr=(u8 *)(CTRDG_AGB_FLASH_ADR+(secNo<<AgbFlash->sector.shift));
//#endif
    (void)OS_RestoreIrq(ctrdgi_backup_irq);

    result = CTRDGi_PollingSR(CTRDG_BACKUP_PHASE_SECTOR_ERASE, adr, 0xff);
    if (((result & (CTRDG_BACKUP_RESULT_ERROR | CTRDG_BACKUP_RESULT_Q5TIMEOUT)) != 0) &&
        (retry == 0))
    {
        retry++;
        goto erase_again;
    }

    /*アクセスサイクル設定 */
    MI_SetCartridgeRamCycle(ram_cycle);
    /*排他制御（アンロック） */
    (void)OS_UnlockCartridge(ctrdgi_flash_lock_id);

//  *(vu16 *)REG_EXMEMCNT_ADDR=(*(vu16 *)REG_EXMEMCNT_ADDR & 0xfffc)|1;
    return result;
}

static u16 CTRDGi_ProgramFlashByteMX(u8 *src, u8 *dst)
{
    u16     result;

    /*排他制御（ロック） */
//  OS_LockCartridge(ctrdgi_flash_lock_id);

    *(vu8 *)CTRDG_BACKUP_COM_ADR1 = 0xaa;
    *(vu8 *)CTRDG_BACKUP_COM_ADR2 = 0x55;
    *(vu8 *)CTRDG_BACKUP_COM_ADR1 = 0xa0;
    *dst = *src;

    result = CTRDGi_PollingSR(CTRDG_BACKUP_PHASE_PROGRAM, dst, *src);

    /*排他制御（アンロック） */
//  OS_UnlockCartridge(ctrdgi_flash_lock_id);

    return result;
}

u32 CTRDGi_WriteFlashSectorCoreMX(CTRDGTaskInfo * arg)
{
    u8     *tgt;
    //,readFuncBuff[32];
    MICartridgeRamCycle ram_cycle;
    u32     result;
    CTRDGTaskInfo p = *arg;
    u16     secNo = p.sec_num;
    u8     *src = p.data;

    if (secNo >= AgbFlash->sector.count)
        return CTRDG_BACKUP_RESULT_ERROR | CTRDG_BACKUP_PHASE_PARAMETER_CHECK;

    // Erase
    result = CTRDGi_EraseFlashSectorMX(secNo);
    if (result)
    {
        return result;
    }

    /*排他制御（ロック） */
    (void)OS_LockCartridge(ctrdgi_flash_lock_id);
    /*アクセスサイクル設定 */
    ram_cycle = MI_GetCartridgeRamCycle();
    MI_SetCartridgeRamCycle(AgbFlash->agbWait[0]);

    // Program

    // バンクのセット
    CTRDGi_SetFlashBankMx((u16)(secNo >> 4));
    secNo &= 0x0f;


//  *(vu16 *)REG_EXMEMCNT_ADDR=(*(vu16 *)REG_EXMEMCNT_ADDR & 0xfffc)|AgbFlash->agbWait[0]; // read 3cycles wait
    ctrdg_flash_remainder = (u16)AgbFlash->sector.size;
    tgt = (u8 *)(CTRDG_AGB_FLASH_ADR + (secNo << AgbFlash->sector.shift));
    ctrdgi_backup_irq = OS_DisableIrq();
    while (ctrdg_flash_remainder)
    {
        result = CTRDGi_ProgramFlashByteMX(src, tgt);
        if (result)
            break;
        ctrdg_flash_remainder--;
        src++;
        tgt++;
    }
    (void)OS_RestoreIrq(ctrdgi_backup_irq);

    /*アクセスサイクル設定 */
    MI_SetCartridgeRamCycle(ram_cycle);
    /*排他制御（アンロック） */
    (void)OS_UnlockCartridge(ctrdgi_flash_lock_id);

    return result;
}

u16 CTRDGi_EraseFlashChipMX(void)
{
    u16     result;
    CTRDGTaskInfo p;
    result = (u16)CTRDGi_EraseFlashChipCoreMX(&p);

    return result;
}

u16 CTRDGi_EraseFlashSectorMX(u16 secNo)
{
    u16     result;
    CTRDGTaskInfo p;
    p.sec_num = secNo;
    result = (u16)CTRDGi_EraseFlashSectorCoreMX(&p);

    return result;
}

u16 CTRDGi_WriteFlashSectorMX(u16 secNo, u8 *src)
{
    u16     result;
    CTRDGTaskInfo p;
    p.sec_num = secNo;
    p.data = src;
    result = (u16)CTRDGi_WriteFlashSectorCoreMX(&p);

    return result;
}

void CTRDGi_EraseFlashChipAsyncMX(CTRDG_TASK_FUNC callback)
{
    CTRDGTaskInfo p;

    CTRDGi_SetTask(&p, CTRDGi_EraseFlashChipCoreMX, callback);
}

void CTRDGi_EraseFlashSectorAsyncMX(u16 secNo, CTRDG_TASK_FUNC callback)
{
    CTRDGTaskInfo p;

    p.sec_num = secNo;
    CTRDGi_SetTask(&p, CTRDGi_EraseFlashSectorCoreMX, callback);
}

void CTRDGi_WriteFlashSectorAsyncMX(u16 secNo, u8 *src, CTRDG_TASK_FUNC callback)
{
    CTRDGTaskInfo p;

    p.sec_num = secNo;
    p.data = src;
    CTRDGi_SetTask(&p, CTRDGi_WriteFlashSectorCoreMX, callback);
}
