/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CTRDG - libraries - ARM9
  File:     ctrdg_sram.c

  Copyright 2003,2004,2006,2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ctrdg_sram.c,v $
  Revision 1.3  2007/02/20 00:28:11  kitase_hirotake
  indent source

  Revision 1.2  2006/04/07 03:27:28  okubata_ryoma
  small fix

  Revision 1.1  2006/04/05 10:48:30  okubata_ryoma
  AGBバックアップライブラリのSDK収録のための変更


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>

u32     CTRDGi_ReadAgbSramCore(CTRDGTaskInfo * arg);
u32     CTRDGi_WriteAgbSramCore(CTRDGTaskInfo * arg);
u32     CTRDGi_VerifyAgbSramCore(CTRDGTaskInfo * arg);
u32     CTRDGi_WriteAndVerifyAgbSramCore(CTRDGTaskInfo * arg);

/*排他制御*/
u16     ctrdgi_sram_lock_id;
// const data-------------------------------------------
static const u8 AgbLibSram_ver[] = "AGBSRAM_V113";

/*******************************************************

    function's description

********************************************************/
u32 CTRDGi_ReadAgbSramCore(CTRDGTaskInfo * arg)
{
    u8     *src_pt, *dst_pt;
    MICartridgeRamCycle ram_cycle;
    CTRDGTaskInfo p = *arg;
    u32     src = p.offset;
    void   *dst = p.dst;
    u32     size = p.size;
    static BOOL flag = FALSE;

    /*排他制御（ロック） */
    (void)OS_LockCartridge(ctrdgi_sram_lock_id);
    /*アクセスサイクル設定 */
    ram_cycle = MI_GetCartridgeRamCycle();
    MI_SetCartridgeRamCycle(MI_CTRDG_RAMCYCLE_18);

    src_pt = (u8 *)src;
    dst_pt = (u8 *)dst;
    while (size--)
    {
        *dst_pt++ = *src_pt++;
    }

    /*アクセスサイクル設定 */
    MI_SetCartridgeRamCycle(ram_cycle);
    /*排他制御（アンロック） */
    (void)OS_UnlockCartridge(ctrdgi_sram_lock_id);

    return 0;
}

u32 CTRDGi_WriteAgbSramCore(CTRDGTaskInfo * arg)
{
    u8     *src_pt, *dst_pt;
    MICartridgeRamCycle ram_cycle;
    CTRDGTaskInfo p = *arg;
    u32     dst = p.offset;
    const void *src = p.data;
    u32     size = p.size;

    /*排他制御（ロック） */
    (void)OS_LockCartridge(ctrdgi_sram_lock_id);
    /*アクセスサイクル設定 */
    ram_cycle = MI_GetCartridgeRamCycle();
    MI_SetCartridgeRamCycle(MI_CTRDG_RAMCYCLE_18);

    src_pt = (u8 *)src;
    dst_pt = (u8 *)dst;
    while (size--)
    {
        *dst_pt++ = *src_pt++;
    }

    /*アクセスサイクル設定 */
    MI_SetCartridgeRamCycle(ram_cycle);
    /*排他制御（アンロック） */
    (void)OS_UnlockCartridge(ctrdgi_sram_lock_id);

    return 0;
}

u32 CTRDGi_VerifyAgbSramCore(CTRDGTaskInfo * arg)
{
    u8     *tgt_pt, *src_pt;
    u32     result;
    MICartridgeRamCycle ram_cycle;
    CTRDGTaskInfo p = *arg;
    u32     tgt = p.offset;
    const void *src = p.data;
    u32     size = p.size;

    /*排他制御（ロック） */
    (void)OS_LockCartridge(ctrdgi_sram_lock_id);
    /*アクセスサイクル設定 */
    ram_cycle = MI_GetCartridgeRamCycle();
    MI_SetCartridgeRamCycle(MI_CTRDG_RAMCYCLE_18);

    result = 0;
    tgt_pt = (u8 *)tgt;
    src_pt = (u8 *)src;
    while (size--)
    {
        if (*tgt_pt++ != *src_pt++)
        {
            result = (u32)tgt_pt - 1;
            break;
        }
    }

    /*アクセスサイクル設定 */
    MI_SetCartridgeRamCycle(ram_cycle);
    /*排他制御（アンロック） */
    (void)OS_UnlockCartridge(ctrdgi_sram_lock_id);

    return result;
}


u32 CTRDGi_WriteAndVerifyAgbSramCore(CTRDGTaskInfo * arg)
{
    u8      retry;
    u32     result;
    CTRDGTaskInfo p = *arg;
    u32     dst = p.offset;
    const void *src = p.data;
    u32     size = p.size;

    retry = 0;
    while (retry < CTRDG_AGB_SRAM_RETRY_MAX)
    {
        CTRDG_WriteAgbSram(dst, src, size);
        result = CTRDG_VerifyAgbSram(dst, src, size);
        if (result == 0)
            break;
        retry++;
    }
    return result;
}


void CTRDG_ReadAgbSram(u32 src, void *dst, u32 size)
{
    CTRDGTaskInfo p;
    p.offset = src;
    p.dst = (u8 *)dst;
    p.size = size;

    (void)CTRDGi_ReadAgbSramCore(&p);
}

void CTRDG_WriteAgbSram(u32 dst, const void *src, u32 size)
{
    CTRDGTaskInfo p;
    p.data = (u8 *)src;
    p.offset = dst;
    p.size = size;

    (void)CTRDGi_WriteAgbSramCore(&p);
}

u32 CTRDG_VerifyAgbSram(u32 tgt, const void *src, u32 size)
{
    u32     result;
    CTRDGTaskInfo p;
    p.data = (u8 *)src;
    p.offset = tgt;
    p.size = size;

    result = CTRDGi_VerifyAgbSramCore(&p);

    return result;
}

u32 CTRDG_WriteAndVerifyAgbSram(u32 dst, const void *src, u32 size)
{
    u32     result;
    CTRDGTaskInfo p;
    p.data = (u8 *)src;
    p.offset = dst;
    p.size = size;

    result = CTRDGi_WriteAndVerifyAgbSramCore(&p);

    return result;
}
void CTRDG_ReadAgbSramAsync(u32 src, void *dst, u32 size, CTRDG_TASK_FUNC callback)
{
    CTRDGTaskInfo p;
    p.offset = src;
    p.dst = (u8 *)dst;
    p.size = size;

    CTRDGi_SetTask(&p, CTRDGi_ReadAgbSramCore, callback);
}

void CTRDG_WriteAgbSramAsync(u32 dst, const void *src, u32 size, CTRDG_TASK_FUNC callback)
{
    CTRDGTaskInfo p;
    p.data = (u8 *)src;
    p.offset = dst;
    p.size = size;

    CTRDGi_SetTask(&p, CTRDGi_WriteAgbSramCore, callback);
}

void CTRDG_VerifyAgbSramAsync(u32 tgt, const void *src, u32 size, CTRDG_TASK_FUNC callback)
{
    CTRDGTaskInfo p;
    p.data = (u8 *)src;
    p.offset = tgt;
    p.size = size;

    CTRDGi_SetTask(&p, CTRDGi_VerifyAgbSramCore, callback);
}

void CTRDG_WriteAndVerifyAgbSramAsync(u32 dst, const void *src, u32 size, CTRDG_TASK_FUNC callback)
{
    CTRDGTaskInfo p;
    p.data = (u8 *)src;
    p.offset = dst;
    p.size = size;

    CTRDGi_SetTask(&p, CTRDGi_WriteAndVerifyAgbSramCore, callback);
}
