/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - snd
  File:     resource_mgr.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.7 $
 *---------------------------------------------------------------------------*/

#ifndef NNS_SND_RESOURCE_MGR_H_
#define NNS_SND_RESOURCE_MGR_H_

#include <nitro/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
	public function declaration
 ******************************************************************************/

BOOL NNS_SndLockChannel( u32 chBitFlag );
void NNS_SndUnlockChannel( u32 chBitFlag );

BOOL NNS_SndLockCapture( u32 capBitFlag );
void NNS_SndUnlockCapture( u32 capBitFlag );

int NNS_SndAllocAlarm( void );
void NNS_SndFreeAlarm( int alarmNo );

/******************************************************************************
	private function declaration
 ******************************************************************************/

void NNSi_SndInitResourceMgr( void );
u32 NNSi_GetLockedChannel( void );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NNS_SND_RESOURCE_MGR_H_ */

