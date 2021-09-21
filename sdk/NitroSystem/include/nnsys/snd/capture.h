/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - snd
  File:     capture.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.22 $
 *---------------------------------------------------------------------------*/

#ifndef NNS_SND_CAPTURE_H_
#define NNS_SND_CAPTURE_H_

#include <nitro/types.h>
#include <nitro/snd.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
	enum definition
 ******************************************************************************/

typedef enum
{
    NNS_SND_CAPTURE_FORMAT_PCM16,
    NNS_SND_CAPTURE_FORMAT_PCM8
} NNSSndCaptureFormat;

typedef enum {
    NNS_SND_CAPTURE_TYPE_REVERB,
    NNS_SND_CAPTURE_TYPE_EFFECT,
    NNS_SND_CAPTURE_TYPE_SAMPLING
} NNSSndCaptureType;

/******************************************************************************
	type definition
 ******************************************************************************/

typedef void (*NNSSndCaptureCallback)(
    void* bufferL,
    void* bufferR,
    u32 len,
    NNSSndCaptureFormat format,
    void* arg
);
typedef NNSSndCaptureCallback NNSSndCaptureEffectCallback;

/******************************************************************************
	public function declaration
 ******************************************************************************/

void NNS_SndCaptureCreateThread( u32 threadPrio );
void NNS_SndCaptureDestroyThread( void );

BOOL NNS_SndCaptureStartReverb(    
    void* buffer_p,
    u32 bufSize,
    NNSSndCaptureFormat format,
    int sampleRate,
    int volume
);
void NNS_SndCaptureStopReverb( int frames );
void NNS_SndCaptureSetReverbVolume( int volume, int frames );

BOOL NNS_SndCaptureStartEffect(    
    void* buffer_p,
    u32 bufSize,
    NNSSndCaptureFormat format,
    int sampleRate,
    int interval,
    NNSSndCaptureEffectCallback callback,
    void* arg
);
void NNS_SndCaptureStopEffect( void );

BOOL NNS_SndCaptureStartSampling(    
    void* buffer_p,
    u32 bufSize,
    NNSSndCaptureFormat format,
    int sampleRate,
    int interval,
    NNSSndCaptureCallback callback,
    void* arg
);
void NNS_SndCaptureStopSampling( void );

BOOL NNS_SndCaptureIsActive( void );
NNSSndCaptureType NNS_SndCaptureGetCaptureType( void );

/******************************************************************************
	private function declaration
 ******************************************************************************/

void NNSi_SndCaptureInit( void );
void NNSi_SndCaptureMain( void );

BOOL NNSi_SndCaptureStart(
    NNSSndCaptureType type,
    void* buffer0,
    void* buffer1,
    u32 bufLen,
    NNSSndCaptureFormat format,
    SNDCaptureIn input,
    SNDCaptureOut output,
    BOOL loopFlag,
    int sampleRate,
    int volume,
    int pan0,
    int pan1,
    int interval,
    NNSSndCaptureCallback callback,
    void* arg    
);
void NNSi_SndCaptureStop( void );

void NNSi_SndCaptureBeginSleep( void );
void NNSi_SndCaptureEndSleep( void );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NNS_SND_CAPTURE_H_ */

