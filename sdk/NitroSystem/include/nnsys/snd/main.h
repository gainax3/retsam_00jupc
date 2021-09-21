/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - snd
  File:     main.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.19 $
 *---------------------------------------------------------------------------*/

#ifndef NNS_SND_MAIN_H_
#define NNS_SND_MAIN_H_

#include <nitro/snd.h>
#include <nnsys/snd/config.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
	public function declaration
 ******************************************************************************/

void NNS_SndInit( void );
void NNS_SndMain( void );

void NNS_SndStopSoundAll( void );
void NNS_SndStopChannelAll( void );
void NNS_SndSetMasterVolume( int volume );
void NNS_SndSetMonoFlag( BOOL flag );

BOOL NNS_SndUpdateDriverInfo( void );
BOOL NNS_SndReadDriverChannelInfo( int chNo, SNDChannelInfo* info );

/******************************************************************************
	private function declaration
 ******************************************************************************/
BOOL NNSi_SndReadDriverPlayerInfo( int playerNo, SNDPlayerInfo* info );
BOOL NNSi_SndReadDriverTrackInfo( int playerNo, int trackNo, SNDTrackInfo* info );

/******************************************************************************
	ghost functions
 ******************************************************************************/

NNS_SND_INLINE void NNS_SndBeginSleep( void ) {}
NNS_SND_INLINE void NNS_SndEndSleep( void ) {}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NNS_SND_MAIN_H_ */

