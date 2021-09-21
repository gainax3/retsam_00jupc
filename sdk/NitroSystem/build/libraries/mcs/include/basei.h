/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - mcs
  File:     basei.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.8 $
 *---------------------------------------------------------------------------*/

#ifndef NNS_MCS_BASEI_H_
#define NNS_MCS_BASEI_H_

#include <nnsys/mcs/config.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================
    inline ä÷êî
   ======================================================================== */

NNS_MCS_INLINE void
NNSi_McsSleep(u32 milSec)
{
    if (OS_IsThreadAvailable() && OS_IsTickAvailable() && OS_IsAlarmAvailable())
    {
        OS_Sleep(milSec);
    }
    else
    {
        OS_SpinWait((u32)(1LL * HW_CPU_CLOCK * milSec /1000));
    }
}


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NNS_MCS_BASEI_H_ */
#endif
