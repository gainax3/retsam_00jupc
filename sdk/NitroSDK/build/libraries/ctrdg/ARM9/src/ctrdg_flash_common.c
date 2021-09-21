/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CTRDG - libraries - ARM9
  File:     ctrdg_flash_common.c

  Copyright 2003,2004,2006,2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ctrdg_flash_common.c,v $
  Revision 1.4  2007/02/20 00:28:11  kitase_hirotake
  indent source

  Revision 1.3  2006/04/10 11:23:01  okubata_ryoma
  1Mのポーリング関数の修正

  Revision 1.2  2006/04/07 03:27:28  okubata_ryoma
  small fix

  Revision 1.1  2006/04/05 10:48:30  okubata_ryoma
  AGBバックアップライブラリのSDK収録のための変更


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

// function's prototype declaration-------------

u32     CTRDGi_ReadAgbFlashCore(CTRDGTaskInfo * arg);
u32     CTRDGi_VerifyAgbFlashCore(CTRDGTaskInfo * arg);
u32     CTRDGi_WriteAndVerifyAgbFlashSectorCore(CTRDGTaskInfo * arg);
u32     CTRDGi_WriteAndVerifyAgbFlashSectorCore(CTRDGTaskInfo * arg);

u8      CTRDGi_ReadSrFunc(u8 *adr);    //Polling関数512k,1Mから個別に参照される

// global variables-----------------------------

u16     ctrdg_flash_remainder;

const CTRDGFlashType *AgbFlash;
u16     (*CTRDGi_WriteAgbFlashSector) (u16 secNo, u8 *src);
u16     (*CTRDGi_EraseAgbFlashChip) (void);
u16     (*CTRDGi_EraseAgbFlashSector) (u16 sec_num);
void    (*CTRDGi_WriteAgbFlashSectorAsync) (u16 secNo, u8 *src, CTRDG_TASK_FUNC callback);
void    (*CTRDGi_EraseAgbFlashChipAsync) (CTRDG_TASK_FUNC callback);
void    (*CTRDGi_EraseAgbFlashSectorAsync) (u16 sec_num, CTRDG_TASK_FUNC callback);
u16     (*CTRDGi_PollingSR) (u16 phase, u8 *adr, u16 lastData);
const u16 (*ctrdgi_fl_maxtime);

OSTick  start_tick, timeout_tick;

/*排他制御*/
u16     ctrdgi_flash_lock_id;
BOOL    ctrdgi_backup_irq;

vu16    ctrdgi_timeoutFlag;            //Polling関数512k,1Mから個別に参照される


/*******************************************************

    function's description

********************************************************/
void CTRDGi_SetFlashBankMx(u16 bank)
{
    *(vu8 *)CTRDG_BACKUP_COM_ADR1 = 0xaa;
    *(vu8 *)CTRDG_BACKUP_COM_ADR2 = 0x55;
    *(vu8 *)CTRDG_BACKUP_COM_ADR1 = 0xb0;
    *(vu8 *)CTRDG_AGB_FLASH_ADR = (u8)bank;
}

// flash Identify functions---------------------
u16 CTRDGi_ReadFlashID(void)
{
    u16     flashID;

    // read flashID
    ctrdgi_backup_irq = OS_DisableIrq();

    *(vu8 *)CTRDG_BACKUP_COM_ADR1 = 0xaa;
    *(vu8 *)CTRDG_BACKUP_COM_ADR2 = 0x55;
    *(vu8 *)CTRDG_BACKUP_COM_ADR1 = 0x90;

    (void)OS_RestoreIrq(ctrdgi_backup_irq);
    /*wait 20ms */
    StartFlashTimer(0);
    while (!ctrdgi_timeoutFlag)
    {
        CheckFlashTimer();
    };

    //manufacturer ID
    flashID = (u16)(CTRDGi_ReadSrFunc((u8 *)CTRDG_BACKUP_DEVICE_ID_ADR) << 8);
    flashID |= CTRDGi_ReadSrFunc((u8 *)CTRDG_BACKUP_VENDER_ID_ADR);

    // exit readID-mode(ATMEL)
    ctrdgi_backup_irq = OS_DisableIrq();

    *(vu8 *)CTRDG_BACKUP_COM_ADR1 = 0xaa;
    *(vu8 *)CTRDG_BACKUP_COM_ADR2 = 0x55;
    *(vu8 *)CTRDG_BACKUP_COM_ADR1 = 0xf0;

    // reset
    *(vu8 *)CTRDG_BACKUP_COM_ADR1 = 0xf0;

    (void)OS_RestoreIrq(ctrdgi_backup_irq);

    //wait 20ms
    StartFlashTimer(0);
    while (!ctrdgi_timeoutFlag)
    {
        CheckFlashTimer();
    };

    return flashID;
}


void StartFlashTimer(u16 phase)
{
    ctrdgi_timeoutFlag = FALSE;

    timeout_tick = (u64)*(ctrdgi_fl_maxtime + phase);
    start_tick = OS_GetTick();
}

void CheckFlashTimer(void)
{
    OSTick  current_tick, process_tick, process_time;

    current_tick = OS_GetTick();
    process_tick = current_tick - start_tick;
    process_time = OS_TicksToMilliSeconds(process_tick);
    if (timeout_tick <= process_time)
        ctrdgi_timeoutFlag = TRUE;
}

// flash access functions-------------------------------

//※各ルーチンに対応する関数ポインタを使用する。

// Data Polling ----------------------------------------
u8 CTRDGi_ReadSrFunc(u8 *adr)
{
    return *adr;
}


//u8 pollflag;
u16 CTRDGi_PollingSR512kCOMMON(u16 phase, u8 *adr, u16 lastData)
{
    u16     poll;
    u16     result;

//++++++++++++++++++++++++ for debug    
//  pollflag=1;
//++++++++++++++++++++++++
    result = CTRDG_BACKUP_RESULT_OK;
    StartFlashTimer(phase);
    while (1)
    {
        poll = CTRDGi_ReadSrFunc(adr);
        if (poll == lastData)
        {
            break;
        }
        if (ctrdgi_timeoutFlag)
        {
            if (CTRDGi_ReadSrFunc(adr) == lastData)
            {
                break;
            }
            else
            {
                if (AgbFlash->makerID == 0xc2)
                {                      // Megachips makerID
                    *(vu8 *)CTRDG_BACKUP_COM_ADR1 = 0xf0;       // command reset
                }
                result = (u16)(CTRDG_BACKUP_RESULT_ERROR | CTRDG_BACKUP_RESULT_TIMEOUT | phase);
                break;
            }
        }
    }
    if (!CTRDG_IsExisting())
        result = CTRDG_BACKUP_RESULT_PULLEDOUT;
//++++++++++++++++++++++++ for debug    
//  pollflag=0;
//++++++++++++++++++++++++ for debug    
    return result;
}


//u8 pollflag;
u16 CTRDGi_PollingSR1MCOMMON(u16 phase, u8 *adr, u16 lastData)
{
    u16     poll;
    u16     result;

//++++++++++++++++++++++++ for debug    
//  pollflag=1;
//++++++++++++++++++++++++
    result = CTRDG_BACKUP_RESULT_OK;
    StartFlashTimer(phase);
    while (1)
    {
        CheckFlashTimer();
        poll = CTRDGi_ReadSrFunc(adr);

        if (poll == lastData)
        {
            break;
        }
        else
        {
            if ((poll & 0x20) != 0)
            {                          //Q5(TimeOutFlag)
                if (CTRDGi_ReadSrFunc(adr) == lastData)
                {
                    break;
                }
                else
                {
                    *(vu8 *)CTRDG_BACKUP_COM_ADR1 = 0xf0;       // command reset
                    result =
                        (u16)(CTRDG_BACKUP_RESULT_ERROR | CTRDG_BACKUP_RESULT_Q5TIMEOUT | phase);
                    break;
                }
            }
        }
        if (ctrdgi_timeoutFlag)
        {
            if (CTRDGi_ReadSrFunc(adr) == lastData)
            {
                break;
            }
            else
            {
                *(vu8 *)CTRDG_BACKUP_COM_ADR1 = 0xf0;   // command reset
                result = (u16)(CTRDG_BACKUP_RESULT_ERROR | CTRDG_BACKUP_RESULT_TIMEOUT | phase);
                break;
            }
        }
    }
    if (!CTRDG_IsExisting())
        result = CTRDG_BACKUP_RESULT_PULLEDOUT;
//++++++++++++++++++++++++ for debug    
//  pollflag=0;
//++++++++++++++++++++++++ for debug    
    return result;
}


/*------------------------------------------------------------------*/
/*          データ読み出し                                          */
/*------------------------------------------------------------------*/
u32 CTRDGi_ReadAgbFlashCore(CTRDGTaskInfo * arg)
{
    u8     *src;
    MICartridgeRamCycle ram_cycle;
    CTRDGTaskInfo p = *arg;
    u16     sec_num = p.sec_num;
    u32     offset = p.offset;
    u8     *dst = p.dst;
    u32     size = p.size;

    /*排他制御（ロック） */
    (void)OS_LockCartridge(ctrdgi_flash_lock_id);
    /*アクセスサイクル設定 */
    ram_cycle = MI_GetCartridgeRamCycle();
    MI_SetCartridgeRamCycle(MI_CTRDG_RAMCYCLE_18);

    // 1MbitFlashなら、バンクのセット
    if (AgbFlash->romSize == 0x00020000)
    {
        CTRDGi_SetFlashBankMx((u16)(sec_num >> 4));
        sec_num &= 0x0f;
    }

    src = (u8 *)(CTRDG_AGB_FLASH_ADR + (sec_num << AgbFlash->sector.shift) + offset);

    while (size--)
        *dst++ = *src++;

    /*アクセスサイクル設定 */
    MI_SetCartridgeRamCycle(ram_cycle);
    /*排他制御（アンロック） */
    (void)OS_UnlockCartridge(ctrdgi_flash_lock_id);

    return 0;
}

static u32 CTRDGi_VerifyFlashSectorCore(u8 *src, u8 *tgt, u32 size)
{
    while (size--)
    {
        if (*tgt++ != *src++)
            return (u32)tgt - 1;
    }
    return 0;
}

/*------------------------------------------------------------------*/
/*          バイト単位データベリファイ                              */
/*------------------------------------------------------------------*/
u32 CTRDGi_VerifyAgbFlashCore(CTRDGTaskInfo * arg)
{
    u8     *tgt;
    u32     result;
    MICartridgeRamCycle ram_cycle;
    CTRDGTaskInfo p = *arg;
    u16     sec_num = p.sec_num;
    u8     *src = p.data;
    u32     size = p.size;

    /*排他制御（ロック） */
    (void)OS_LockCartridge(ctrdgi_flash_lock_id);
    /*アクセスサイクル設定 */
    ram_cycle = MI_GetCartridgeRamCycle();
    MI_SetCartridgeRamCycle(MI_CTRDG_RAMCYCLE_18);

    // 1MbitFlashなら、バンクのセット
    if (AgbFlash->romSize == 0x00020000)
    {
        CTRDGi_SetFlashBankMx((u16)(sec_num >> 4));
        sec_num &= 0x0f;
    }

    tgt = (u8 *)(CTRDG_AGB_FLASH_ADR + (sec_num << AgbFlash->sector.shift));
    result = CTRDGi_VerifyFlashSectorCore(src, tgt, size);

    /*アクセスサイクル設定 */
    MI_SetCartridgeRamCycle(ram_cycle);
    /*排他制御（アンロック） */
    (void)OS_UnlockCartridge(ctrdgi_flash_lock_id);
    return result;
}


/*------------------------------------------------------------------*/
/*          セクタ単位データ書き込み＆バイト単位ベリファイ          */
/*------------------------------------------------------------------*/
u32 CTRDGi_WriteAndVerifyAgbFlashSectorCore(CTRDGTaskInfo * arg)
{
    u16     retry;
    u32     result;
    CTRDGTaskInfo p = *arg;
    u16     sec_num = p.sec_num;
    u32     offset = p.offset;
    u8     *src = p.data;
    u32     verifysize = p.size;

    retry = 0;
    while (retry < CTRDG_AGB_FLASH_RETRY_MAX)
    {
        result = (u32)CTRDG_WriteAgbFlashSector(sec_num, src);
        if (result == 0)
        {
            result = CTRDG_VerifyAgbFlash(sec_num, src, verifysize);
            if (result == 0)
                break;
        }
        retry++;
    }
    return result;
}

void CTRDG_ReadAgbFlash(u16 sec_num, u32 offset, u8 *dst, u32 size)
{
    CTRDGTaskInfo p;
    p.sec_num = sec_num;
    p.offset = offset;
    p.dst = dst;
    p.size = size;

    (void)CTRDGi_ReadAgbFlashCore(&p);
}

u16 CTRDG_EraseAgbFlashChip(void)
{
    u16     result = (*CTRDGi_EraseAgbFlashChip) ();

    return result;
}

u16 CTRDG_EraseAgbFlashSector(u16 secNo)
{
    u16     result = (*CTRDGi_EraseAgbFlashSector) (secNo);

    return result;
}

u16 CTRDG_WriteAgbFlashSector(u16 secNo, u8 *src)
{
    u16     result = (*CTRDGi_WriteAgbFlashSector) (secNo, src);

    return result;
}

u32 CTRDG_VerifyAgbFlash(u16 sec_num, u8 *src, u32 size)
{
    u32     result;
    CTRDGTaskInfo p;
    p.sec_num = sec_num;
    p.data = src;
    p.size = size;
    result = CTRDGi_VerifyAgbFlashCore(&p);

    return result;
}

u32 CTRDG_WriteAndVerifyAgbFlash(u16 sec_num, u8 *src, u32 verifysize)
{
    u32     result;
    CTRDGTaskInfo p;
    p.sec_num = sec_num;
    p.data = src;
    p.size = verifysize;
    result = CTRDGi_WriteAndVerifyAgbFlashSectorCore(&p);

    return result;
}

void CTRDG_ReadAgbFlashAsync(u16 sec_num, u32 offset, u8 *dst, u32 size, CTRDG_TASK_FUNC callback)
{
    CTRDGTaskInfo p;
    p.sec_num = sec_num;
    p.offset = offset;
    p.dst = dst;
    p.size = size;

    CTRDGi_SetTask(&p, CTRDGi_ReadAgbFlashCore, callback);
}

void CTRDG_EraseAgbFlashChipAsync(CTRDG_TASK_FUNC callback)
{
    (*CTRDGi_EraseAgbFlashChipAsync) (callback);
}

void CTRDG_EraseAgbFlashSectorAsync(u16 secNo, CTRDG_TASK_FUNC callback)
{
    (*CTRDGi_EraseAgbFlashSectorAsync) (secNo, callback);
}

void CTRDG_WriteAgbFlashSectorAsync(u16 secNo, u8 *src, CTRDG_TASK_FUNC callback)
{
    (*CTRDGi_WriteAgbFlashSectorAsync) (secNo, src, callback);
}

void CTRDG_VerifyAgbFlashAsync(u16 sec_num, u8 *src, u32 size, CTRDG_TASK_FUNC callback)
{
    CTRDGTaskInfo p;
    p.sec_num = sec_num;
    p.data = src;
    p.size = size;
    CTRDGi_SetTask(&p, CTRDGi_VerifyAgbFlashCore, callback);
}

void CTRDG_WriteAndVerifyAgbFlashAsync(u16 sec_num, u8 *src, u32 verifysize,
                                       CTRDG_TASK_FUNC callback)
{
    CTRDGTaskInfo p;
    p.sec_num = sec_num;
    p.data = src;
    p.size = verifysize;
    CTRDGi_SetTask(&p, CTRDGi_WriteAndVerifyAgbFlashSectorCore, callback);
}
