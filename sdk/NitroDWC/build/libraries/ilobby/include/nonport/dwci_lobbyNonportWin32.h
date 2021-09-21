/*---------------------------------------------------------------------------*
  Project:  NitroDWC Libraries
  File:     ./build/lobby/include/dwci_nonportWin32.h

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

#ifndef DWCi_NONPORT_WIN32_H_
#define DWCi_NONPORT_WIN32_H_

#pragma pack(4)

// 例外指定の警告をなくす
#pragma warning(disable: 4290)

#define DWCi_Np_CpuClear8(dest, size)       memset(dest, 0, size)
#define DWCi_Np_CpuCopy8(src, dest, size)   memcpy(dest, src, size)
//#define DWC_ASSERTMSG SDK_ASSERTMSG
#define _tprintf printf
#define DWCi_Np_VSNPrintf vsnprintf

#include <auth/dwci_auth_interface.h>
#define OSTIME_UNIXTIME_DIFF_SEC    946684800LL

inline s64 DWCi_Np_GetCurrentSecondByUTC()
{
    DWCUtcTime diff;
    DWCUtcTime now;
    BOOL result;
    
    // ライブラリが初期化されていなければアサート
    DWC_ASSERT_IF_NOT_INITIALIZED()
    
    now = DWCi_Np_GetTimeInSeconds();
    result = DWCi_Auth_GetNasTimeDiff(&diff);
    if (result)
    {
        now += diff;
    }
    
    // 1970年からの時間なので2000年からに直す
    now -= OSTIME_UNIXTIME_DIFF_SEC;
    
    return (s64)now;
}


#endif
