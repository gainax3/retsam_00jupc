/*---------------------------------------------------------------------------*
  Project:  NitroDWC Libraries
  File:     ./build/lobby/include/dwci_nonportNitro.h

  Copyright 2005-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/
/**
 * @file
 *
 * @brief Wi-Fi ロビーライブラリ ヘッダ
 */

#ifndef DWCi_NONPORT_NITRO_H_
#define DWCi_NONPORT_NITRO_H_

#define DWCi_Np_CpuClear8(dest, size)       MI_CpuClear8(dest, size)
#define DWCi_Np_CpuCopy8(src, dest, size)   MI_CpuCopy8(src, dest, size)
#define DWC_ASSERTMSG SDK_ASSERTMSG
#undef _tprintf
#define _tprintf OS_TPrintf
#define DWCi_Np_VSNPrintf OS_VSNPrintf
#define DWC_HALT OS_Panic

inline void DWCi_Np_GetMacAddress(u8 *macAddress)
{
    OS_GetMacAddress( macAddress );
}

inline s64 DWCi_Np_GetCurrentSecondByUTC()
{
    RTCDate date;
    RTCTime time;
    DWC_GetDateTime(&date, &time);
    return RTC_ConvertDateTimeToSecond(&date, &time);
}


#endif
