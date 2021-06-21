/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - snd
  File:     output_effect.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.4 $
 *---------------------------------------------------------------------------*/

#ifndef NNS_SND_OUTPUT_EFFECT_H_
#define NNS_SND_OUTPUT_EFFECT_H_

#include <nitro/types.h>
#include <nnsys/snd/capture.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
	enum declaration
 ******************************************************************************/

typedef enum {
    NNS_SND_CAPTURE_OUTPUT_EFFECT_NORMAL,
    NNS_SND_CAPTURE_OUTPUT_EFFECT_SURROUND,
    NNS_SND_CAPTURE_OUTPUT_EFFECT_HEADPHONE,
    NNS_SND_CAPTURE_OUTPUT_EFFECT_MONO
} NNSSndCaptureOutputEffectType;

/******************************************************************************
	public function declaration
 ******************************************************************************/

BOOL NNS_SndCaptureStartOutputEffect( void* buffer_p, u32 bufSize, NNSSndCaptureOutputEffectType type );
void NNS_SndCaptureStopOutputEffect( void );
void NNS_SndCaptureChangeOutputEffect( NNSSndCaptureOutputEffectType type );
void NNS_SndCaptureSetOutputEffectCallback( NNSSndCaptureCallback func, void* arg );
void NNS_SndCaptureSetPostOutputEffectCallback( NNSSndCaptureCallback func, void* arg );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NNS_SND_OUTPUT_EFFECT_H_ */

