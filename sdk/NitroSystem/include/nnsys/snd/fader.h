/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - snd
  File:     fader.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.2 $
 *---------------------------------------------------------------------------*/

#ifndef NNS_SND_FADER_H_
#define NNS_SND_FADER_H_

#include <nitro/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
	structure definition
 ******************************************************************************/

typedef struct NNSSndFader
{
    int origin;
    int target;
    int counter;
    int frame;
} NNSSndFader;

/******************************************************************************
	private function declaration
 ******************************************************************************/

void NNSi_SndFaderInit( NNSSndFader* fader );
void NNSi_SndFaderSet( NNSSndFader* fader, int target, int frame );
int  NNSi_SndFaderGet( const NNSSndFader* fader );
void NNSi_SndFaderUpdate( NNSSndFader* fader );
BOOL NNSi_SndFaderIsFinished( const NNSSndFader* fader );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NNS_SND_FADER_H_ */

