/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS
  File:     os_entropy.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_entropy.c,v $
  Revision 1.4  2006/07/05 09:11:24  yosizaki
  fix about include header.

  Revision 1.3  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.2  2005/07/27 12:15:36  seiki_masashi
  MAC アドレスも追加

  Revision 1.1  2005/07/27 07:36:41  seiki_masashi
  新規追加

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/os.h>
#include <nitro/gx.h>

//----------------------------------------------------------------------

extern vu64 OSi_TickCounter;

/*---------------------------------------------------------------------------*
  Name:         OS_GetLowEntropyData

  Description:  システム状態によって変化する低エントロピーのデータ列を取得する

  Arguments:    buffer - データ列を受け取る配列へのポインタ

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_GetLowEntropyData(u32 buffer[OS_LOW_ENTROPY_DATA_SIZE / sizeof(u32)])
{
    // System Work Memory 上の RTC のワーク領域
    const OSSystemWork *work = OS_GetSystemWork();
    const u8 *macAddress =
        (u8 *)((u32)(work->nvramUserInfo) + ((sizeof(NVRAMConfig) + 3) & ~0x00000003));

    buffer[0] = (u32)((GX_GetVCount() << 16) | OS_GetTickLo());
    buffer[1] = (u32)(*(u16 *)(macAddress + 4) << 16) ^ (u32)(OSi_TickCounter);
    buffer[2] = (u32)(OSi_TickCounter >> 32) ^ *(u32 *)macAddress ^ work->vblankCount;
#ifdef reg_G3X_GXSTAT
    buffer[2] ^= reg_G3X_GXSTAT;
#endif
    buffer[3] = *(u32 *)(&work->real_time_clock[0]);
    buffer[4] = *(u32 *)(&work->real_time_clock[4]);
    buffer[5] = (((u32)work->mic_sampling_data) << 16) ^ work->mic_last_address;
    buffer[6] = (u32)((*(u16 *)(&work->touch_panel[0]) << 16) | *(u16 *)(&work->touch_panel[2]));
    buffer[7] = (u32)((work->wm_rssi_pool << 16) | (reg_PAD_KEYINPUT | *(vu16 *)HW_BUTTON_XY_BUF));
}
