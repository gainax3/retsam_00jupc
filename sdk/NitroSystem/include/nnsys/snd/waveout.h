/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - snd
  File:     waveout.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.4 $
 *---------------------------------------------------------------------------*/

#ifndef NNS_SND_WAVEOUT_H_
#define NNS_SND_WAVEOUT_H_

#include <nitro/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
	macro definition
 ******************************************************************************/

#define NNS_SND_WAVEOUT_INVALID_HANDLE NULL

#define NNS_SND_WAVEOUT_VOLUME_MIN   0
#define NNS_SND_WAVEOUT_VOLUME_MAX 127

#define NNS_SND_WAVEOUT_PAN_MIN   0
#define NNS_SND_WAVEOUT_PAN_MAX 127

/******************************************************************************
	enum definition
 ******************************************************************************/

typedef enum NNSSndWaveFormat
{
    NNS_SND_WAVE_FORMAT_PCM8,
    NNS_SND_WAVE_FORMAT_PCM16
} NNSSndWaveFormat;

/******************************************************************************
	type definition
 ******************************************************************************/

struct NNSSndWaveOut;
typedef struct NNSSndWaveOut* NNSSndWaveOutHandle;

/******************************************************************************
	public function declaration
 ******************************************************************************/

NNSSndWaveOutHandle NNS_SndWaveOutAllocChannel( int chNo );
void NNS_SndWaveOutFreeChannel( NNSSndWaveOutHandle handle );

BOOL NNS_SndWaveOutStart(
    NNSSndWaveOutHandle handle,
    NNSSndWaveFormat format,
    const void* dataaddr,
    BOOL loopFlag,
    int loopStartSample,
    int samples,
    int sampleRate,
    int volume, 
    int speed,
    int pan
);
void NNS_SndWaveOutStop( NNSSndWaveOutHandle handle );

void NNS_SndWaveOutSetVolume( NNSSndWaveOutHandle handle, int volume );
void NNS_SndWaveOutSetSpeed( NNSSndWaveOutHandle handle, int speed );
void NNS_SndWaveOutSetPan( NNSSndWaveOutHandle handle, int pan );

BOOL NNS_SndWaveOutIsPlaying( NNSSndWaveOutHandle handle );

void NNS_SndWaveOutWaitForChannelStop( NNSSndWaveOutHandle handle );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NNS_SND_WAVEOUT_H_ */

