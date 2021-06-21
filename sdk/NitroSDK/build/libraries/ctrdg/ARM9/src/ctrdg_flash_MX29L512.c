/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CTRDG - libraries - ARM9
  File:     ctrdg_flash_MX29L512.c

  Copyright 2003,2004,2006,2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ctrdg_flash_MX29L512.c,v $
  Revision 1.3  2007/02/20 00:28:11  kitase_hirotake
  indent source

  Revision 1.2  2006/04/07 03:27:28  okubata_ryoma
  small fix

  Revision 1.1  2006/04/05 10:48:30  okubata_ryoma
  AGBバックアップライブラリのSDK収録のための変更


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>


// extern data----------------------------------
extern u16 CTRDGi_PollingSR512kCOMMON(u16 phase, u8 *adr, u16 lastData);
extern u16 CTRDGi_EraseFlashChipLE(void);
extern u16 CTRDGi_EraseFlashSectorLE(u16 secNo);
extern u16 CTRDGi_ProgramFlashByteLE(u8 *src, u8 *dst);
extern void CTRDGi_EraseFlashChipAsyncLE(CTRDG_TASK_FUNC callback);
extern void CTRDGi_EraseFlashSectorAsyncLE(u16 secNo, CTRDG_TASK_FUNC callback);
extern void CTRDGi_WriteFlashSectorAsyncLE(u16 secNo, u8 *src, CTRDG_TASK_FUNC callback);
/*排他制御*/
extern u16 ctrdgi_flash_lock_id;

// function's prototype declaration-------------
u16     CTRDGi_WriteFlashSectorMX5(u16 secNo, u8 *src);
u32     CTRDGi_WriteFlashSectorCoreMX5(CTRDGTaskInfo * arg);
void    CTRDGi_WriteFlashSectorAsyncMX5(u16 secNo, u8 *src, CTRDG_TASK_FUNC callback);

// const data-----------------------------------
static const u16 PaMaxTime[] = {
    10,                                // common       10ms
    10,                                // program      10ms(exactly:50usec)    (15.256us*66)*1=1ms
    500,                               // sector erase 100ms
    500,                               // chip   erase 100ms
};

const CTRDGiFlashTypePlus MN63F805MNP = {
    CTRDGi_WriteFlashSectorMX5,
    CTRDGi_EraseFlashChipLE,
    CTRDGi_EraseFlashSectorLE,
    CTRDGi_WriteFlashSectorAsyncMX5,
    CTRDGi_EraseFlashChipAsyncLE,
    CTRDGi_EraseFlashSectorAsyncLE,
    CTRDGi_PollingSR512kCOMMON,
    PaMaxTime,
    {
/* デバッグ用 */
//#ifndef   __FLASH_DEBUG
        0x00010000,                       // ROM size
        {0x00001000, 12, 16, 0},          // sector size,shift,count,top
//#else
//      0x00002000,
//      {0x00000200,  9, 16,  0},
//#endif
        {MI_CTRDG_RAMCYCLE_10, MI_CTRDG_RAMCYCLE_6},       // agb read cycle=4, write cycle=2
        0x32,                             // maker ID
        0x1b,                             // device ID
    },
};

// program description**************************
u32 CTRDGi_WriteFlashSectorCoreMX5(CTRDGTaskInfo * arg)
{
    u8     *tgt;
    MICartridgeRamCycle ram_cycle;
    u32     result;
    CTRDGTaskInfo p = *arg;
    u16     secNo = p.sec_num;
    u8     *src = p.data;

    if (secNo >= AgbFlash->sector.count)
        return CTRDG_BACKUP_RESULT_ERROR | CTRDG_BACKUP_PHASE_PARAMETER_CHECK;

    // Erase
    result = CTRDGi_EraseFlashSectorLE(secNo);
    if (result)
    {
        return result;
    }

    /*排他制御（ロック） */
    (void)OS_LockCartridge(ctrdgi_flash_lock_id);
    /*アクセスサイクル設定 */
    ram_cycle = MI_GetCartridgeRamCycle();
    MI_SetCartridgeRamCycle(AgbFlash->agbWait[0]);
    //*(vu16 *)REG_EXMEMCNT_ADDR=(*(vu16 *)REG_EXMEMCNT_ADDR & 0xfffc)|AgbFlash->agbWait[0]; // read 3cycles wait

    ctrdg_flash_remainder = (u16)AgbFlash->sector.size;
    tgt = (u8 *)(CTRDG_AGB_FLASH_ADR + (secNo << AgbFlash->sector.shift));
    while (ctrdg_flash_remainder)
    {
        result = CTRDGi_ProgramFlashByteLE(src, tgt);
        if (result)
            break;
        ctrdg_flash_remainder--;
        src++;
        tgt++;
    }

    /*アクセスサイクル設定 */
    MI_SetCartridgeRamCycle(ram_cycle);
    /*排他制御（アンロック） */
    (void)OS_UnlockCartridge(ctrdgi_flash_lock_id);

    return result;
}

u16 CTRDGi_WriteFlashSectorMX5(u16 secNo, u8 *src)
{
    u16     result;
    CTRDGTaskInfo p;
    p.sec_num = secNo;
    p.data = src;
    result = (u16)CTRDGi_WriteFlashSectorCoreMX5(&p);

    return result;
}

void CTRDGi_WriteFlashSectorAsyncMX5(u16 secNo, u8 *src, CTRDG_TASK_FUNC callback)
{
    CTRDGTaskInfo p;

    p.sec_num = secNo;
    p.data = src;
    CTRDGi_SetTask(&p, CTRDGi_WriteFlashSectorCoreMX5, callback);
}
