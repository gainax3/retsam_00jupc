/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CTRDG - libraries - ARM9
  File:     ctrdg_flash_LE39FW512.c

  Copyright 2003,2004,2006,2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ctrdg_flash_LE39FW512.c,v $
  Revision 1.3  2007/02/20 00:28:11  kitase_hirotake
  indent source

  Revision 1.2  2006/04/07 03:27:28  okubata_ryoma
  small fix

  Revision 1.1  2006/04/05 10:48:30  okubata_ryoma
  AGBバックアップライブラリのSDK収録のための変更


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>

//#define   __LE_DEBUG
/* デバッグ用 */
//#ifdef    __LE_DEBUG
//#include "mylib.h"
//#endif

// definition data------------------------------
/* デバッグ用 */
//#ifndef   __FLASH_DEBUG
#define CTRDG_BACKUP_COM_ADR1   (CTRDG_AGB_FLASH_ADR+0x00005555)
#define CTRDG_BACKUP_COM_ADR2   (CTRDG_AGB_FLASH_ADR+0x00002aaa)
//#else
//#define   COM_ADR1    0x02003ffe
//#define COM_ADR2  0x02003fff
//#endif

#define FLASH_LOG_SECTOR_COUNT          16

#define ERASE_RETRY_MAX                 0x50

// extern data----------------------------------
extern const CTRDGFlashType *AgbFlash;
extern u16 CTRDGi_PollingSR512kCOMMON(u16 phase, u8 *adr, u16 lastData);
/*排他制御*/
extern u16 ctrdgi_flash_lock_id;
extern BOOL ctrdgi_backup_irq;

// function's prototype declaration-------------
u16     CTRDGi_EraseFlashChipLE(void);
u16     CTRDGi_EraseFlashSectorLE(u16 secNo);
u16     CTRDGi_ProgramFlashByteLE(u8 *src, u8 *dst);
u16     CTRDGi_WriteFlashSectorLE(u16 secNo, u8 *src);
u32     CTRDGi_VerifyFlashCoreFF(u8 *adr);
u32     CTRDGi_VerifyFlashErase(u8 *tgt, u32 (*func_p) (u8 *));
u32     CTRDGi_EraseFlashChipCoreLE(CTRDGTaskInfo * arg);
u32     CTRDGi_EraseFlashSectorCoreLE(CTRDGTaskInfo * arg);
u32     CTRDGi_WriteFlashSectorCoreLE(CTRDGTaskInfo * arg);

void    CTRDGi_EraseFlashChipAsyncLE(CTRDG_TASK_FUNC callback);
void    CTRDGi_EraseFlashSectorAsyncLE(u16 secNo, CTRDG_TASK_FUNC callback);
void    CTRDGi_WriteFlashSectorAsyncLE(u16 secNo, u8 *src, CTRDG_TASK_FUNC callback);

// const data-----------------------------------
static const u16 leMaxTime[] = {
    10,                                // common       10ms
    10,                                // program      10ms(exactly:20usec)
    40,                                // sector erase 25ms
    200,                               // chip   erase 100ms
};

const CTRDGiFlashTypePlus defaultFlash512 = {
    CTRDGi_WriteFlashSectorLE,
    CTRDGi_EraseFlashChipLE,
    CTRDGi_EraseFlashSectorLE,
    CTRDGi_WriteFlashSectorAsyncLE,
    CTRDGi_EraseFlashChipAsyncLE,
    CTRDGi_EraseFlashSectorAsyncLE,
    CTRDGi_PollingSR512kCOMMON,
    leMaxTime,
    {
/* デバッグ用 */
//#ifndef   __FLASH_DEBUG
        0x00010000,                       // ROM size
        {0x00001000, 12, 16, 0},          // sectorSize,Shift,Count
//#else
//      0x00002000,
//      {0x00000200,  9, 16,  0},
//#endif
        {MI_CTRDG_RAMCYCLE_8, MI_CTRDG_RAMCYCLE_6},        // agb read cycle=3, write cycle=2
         0x00,                             // maker ID
         0x00,                             // device ID
    },
};

const CTRDGiFlashTypePlus LE39FW512 = {
    CTRDGi_WriteFlashSectorLE,
    CTRDGi_EraseFlashChipLE,
    CTRDGi_EraseFlashSectorLE,
    CTRDGi_WriteFlashSectorAsyncLE,
    CTRDGi_EraseFlashChipAsyncLE,
    CTRDGi_EraseFlashSectorAsyncLE,
    CTRDGi_PollingSR512kCOMMON,
    leMaxTime,
    {
/* デバッグ用 */
//#ifndef   __FLASH_DEBUG
        0x00010000,                       // ROM size
        {0x00001000, 12, 16, 0},          // sector size,shift,count,top
//#else
//      0x00002000,
//      {0x00000200,  9, 16,  0},
//#endif
        {MI_CTRDG_RAMCYCLE_8, MI_CTRDG_RAMCYCLE_6},        // agb read cycle=3, write cycle=2
        0xbf,                             // maker ID
        0xd4,                             // device ID
    },
};

// program description**************************
u32 CTRDGi_EraseFlashChipCoreLE(CTRDGTaskInfo * arg)
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
    //*(vu16 *)REG_EXMEMCNT_ADDR=(*(vu16 *)REG_EXMEMCNT_ADDR & 0xfffc)|AgbFlash->agbWait[0]; // read 3cycles wait
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

    /*アクセスサイクル設定 */
    MI_SetCartridgeRamCycle(ram_cycle);
    /*排他制御（アンロック） */
    (void)OS_UnlockCartridge(ctrdgi_flash_lock_id);
    return result;
}

u32 CTRDGi_EraseFlashSectorCoreLE(CTRDGTaskInfo * arg)
{
    u8     *adr;
    //u16 readFuncBuff[32];
    MICartridgeRamCycle ram_cycle;
    u32     result;
    CTRDGTaskInfo p = *arg;
    u16     secNo = p.sec_num;

    if (secNo >= FLASH_LOG_SECTOR_COUNT)
        return CTRDG_BACKUP_RESULT_ERROR | CTRDG_BACKUP_PHASE_PARAMETER_CHECK;

    /*排他制御（ロック） */
    (void)OS_LockCartridge(ctrdgi_flash_lock_id);
    /*アクセスサイクル設定 */
    ram_cycle = MI_GetCartridgeRamCycle();
    MI_SetCartridgeRamCycle(AgbFlash->agbWait[0]);
    //*(vu16 *)REG_EXMEMCNT_ADDR=(*(vu16 *)REG_EXMEMCNT_ADDR & 0xfffc)|AgbFlash->agbWait[0]; // read 3cycles wait

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

    /*アクセスサイクル設定 */
    MI_SetCartridgeRamCycle(ram_cycle);
    /*排他制御（アンロック） */
    (void)OS_UnlockCartridge(ctrdgi_flash_lock_id);

    return result;
}

u16 CTRDGi_ProgramFlashByteLE(u8 *src, u8 *dst)
{
    u16     result;
    *(vu8 *)CTRDG_BACKUP_COM_ADR1 = 0xaa;
    *(vu8 *)CTRDG_BACKUP_COM_ADR2 = 0x55;
    *(vu8 *)CTRDG_BACKUP_COM_ADR1 = 0xa0;
    *dst = *src;

    result = CTRDGi_PollingSR(CTRDG_BACKUP_PHASE_PROGRAM, dst, *src);
    return result;
}

u32 CTRDGi_WriteFlashSectorCoreLE(CTRDGTaskInfo * arg)
{
    //u16 funcBuff[48],*func_src,*func_dst
    u8     *tgt;
    u16     retry, add_erase, j;
    MICartridgeRamCycle ram_cycle;
    u32     result;
    CTRDGTaskInfo p = *arg;
    u16     secNo = p.sec_num;
    u8     *src = p.data;

    /*パラメータチェック */
    if (secNo >= FLASH_LOG_SECTOR_COUNT)
        return CTRDG_BACKUP_RESULT_ERROR | CTRDG_BACKUP_PHASE_PARAMETER_CHECK;

    tgt = (u8 *)(CTRDG_AGB_FLASH_ADR + (secNo << AgbFlash->sector.shift));

    // erase verify routine transmit
//  func_src=(u16 *)((u32)VerifyFlashCoreFF & 0xFFFFFFFE);
//  func_dst=funcBuff;
//  for(i=(u32)VerifyFlashErase-(u32)VerifyFlashCoreFF;i>0;i-=2)
//      *func_dst++=*func_src++;
    retry = 0;
    // Erase
/* デバッグ用 */
    //++++++++++++++++++++
//#ifdef    __LE_DEBUG
//  if(secNo<8)
//      pos=0x0194+(secNo<<5);
//  else
//      pos=0x0199+((secNo-8)<<5);
//  DrawHexOnBgx(0,pos,BG_PLTT_WHITE,&retry,2);
//#endif
    //++++++++++++++++++++
    while (1)
    {
        result = CTRDGi_EraseFlashSectorLE(secNo);
        if (result == 0)
        {
            result = (u16)CTRDGi_VerifyFlashErase(tgt, CTRDGi_VerifyFlashCoreFF);
            if (result == 0)
                break;
        }
        if (retry++ == ERASE_RETRY_MAX)
            return result;
/* デバッグ用 */
        //++++++++++++++++++++
//#ifdef    __LE_DEBUG
//      DrawHexOnBgx(0,pos,BG_PLTT_YELLOW,&retry,2);
//#endif
        //++++++++++++++++++++
    }

    // add Erase
    add_erase = 1;
    if (retry > 0)
        add_erase = 6;
    for (j = 1; j <= add_erase; j++)
    {
/* デバッグ用 */
        //++++++++++++++++++++
//#ifdef    __LE_DEBUG
//      DrawHexOnBgx(0,pos+3,BG_PLTT_GREEN,&j,1);
//#endif
        //++++++++++++++++++++
        (void)CTRDGi_EraseFlashSectorLE(secNo);
    }

    /*排他制御（ロック） */
    (void)OS_LockCartridge(ctrdgi_flash_lock_id);
    /*アクセスサイクル設定 */
    ram_cycle = MI_GetCartridgeRamCycle();
    MI_SetCartridgeRamCycle(AgbFlash->agbWait[0]);
    //*(vu16 *)REG_EXMEMCNT_ADDR=(*(vu16 *)REG_EXMEMCNT_ADDR & 0xfffc)|AgbFlash->agbWait[0]; // read 3cycles wait

    // Program

    ctrdg_flash_remainder = (u16)AgbFlash->sector.size;
    ctrdgi_backup_irq = OS_DisableIrq();
    while (ctrdg_flash_remainder)
    {
        result = CTRDGi_ProgramFlashByteLE(src, tgt);
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

u32 CTRDGi_VerifyFlashCoreFF(u8 *adr)
{
    u32     count;
    for (count = AgbFlash->sector.size; count > 0; count--)
    {
        if (*adr++ != 0xff)
        {
            break;
        }
    }
    return count;
}

u32 CTRDGi_VerifyFlashErase(u8 *tgt, u32 (*func_p) (u8 *))
{
    u32     result;
    MICartridgeRamCycle ram_cycle;

    /*排他制御（ロック） */
    (void)OS_LockCartridge(ctrdgi_flash_lock_id);
    /*アクセスサイクル設定 */
    ram_cycle = MI_GetCartridgeRamCycle();
    MI_SetCartridgeRamCycle(AgbFlash->agbWait[0]);

    result = 0;
    if (func_p(tgt))
    {
        result = (CTRDG_BACKUP_RESULT_ERROR | CTRDG_BACKUP_PHASE_VERIFY_ERASE);
    }

    /*アクセスサイクル設定 */
    MI_SetCartridgeRamCycle(ram_cycle);
    /*排他制御（アンロック） */
    (void)OS_UnlockCartridge(ctrdgi_flash_lock_id);

    return result;
}


u16 CTRDGi_EraseFlashChipLE(void)
{
    u16     result;
    CTRDGTaskInfo p;
    result = (u16)CTRDGi_EraseFlashChipCoreLE(&p);

    return result;
}

u16 CTRDGi_EraseFlashSectorLE(u16 secNo)
{
    u16     result;
    CTRDGTaskInfo p;
    p.sec_num = secNo;
    result = (u16)CTRDGi_EraseFlashSectorCoreLE(&p);

    return result;
}

u16 CTRDGi_WriteFlashSectorLE(u16 secNo, u8 *src)
{
    u16     result;
    CTRDGTaskInfo p;
    p.sec_num = secNo;
    p.data = src;
    result = (u16)CTRDGi_WriteFlashSectorCoreLE(&p);

    return result;
}

void CTRDGi_EraseFlashChipAsyncLE(CTRDG_TASK_FUNC callback)
{
    CTRDGTaskInfo p;

    CTRDGi_SetTask(&p, CTRDGi_EraseFlashChipCoreLE, callback);
}

void CTRDGi_EraseFlashSectorAsyncLE(u16 secNo, CTRDG_TASK_FUNC callback)
{
    CTRDGTaskInfo p;

    p.sec_num = secNo;
    CTRDGi_SetTask(&p, CTRDGi_EraseFlashSectorCoreLE, callback);
}

void CTRDGi_WriteFlashSectorAsyncLE(u16 secNo, u8 *src, CTRDG_TASK_FUNC callback)
{
    CTRDGTaskInfo p;

    p.sec_num = secNo;
    p.data = src;
    CTRDGi_SetTask(&p, CTRDGi_WriteFlashSectorCoreLE, callback);
}
