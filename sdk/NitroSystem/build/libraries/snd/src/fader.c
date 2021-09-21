/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - snd
  File:     fader.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.5 $
 *---------------------------------------------------------------------------*/
#include <nnsys/snd/fader.h>

#include <nnsys/misc.h>

/******************************************************************************
	private function
 ******************************************************************************/

void NNSi_SndFaderInit( NNSSndFader* fader )
{    
    NNS_NULL_ASSERT( fader );
    
    fader->origin = fader->target = 0;
    fader->counter = fader->frame = 0;
}

void NNSi_SndFaderSet( NNSSndFader* fader, int target, int frame )
{
    NNS_NULL_ASSERT( fader );
    
    fader->origin = NNSi_SndFaderGet( fader );
    fader->target = target;
    fader->frame  = frame;
    fader->counter = 0;
    
}

int NNSi_SndFaderGet( const NNSSndFader* fader )
{
    s64 value;
    
    NNS_NULL_ASSERT( fader );
    
    if ( fader->counter >= fader->frame ) {
        return fader->target;
    }
    
    value = ( fader->target - fader->origin )
            * fader->counter / fader->frame
            + fader->origin;
    
    return (int)value;
}

void NNSi_SndFaderUpdate( NNSSndFader* fader )
{
    NNS_NULL_ASSERT( fader );
    
    if ( fader->counter < fader->frame ) {
        fader->counter ++;
    }
}

BOOL NNSi_SndFaderIsFinished( const NNSSndFader* fader )
{
    NNS_NULL_ASSERT( fader );
    
    return fader->counter >= fader->frame ? TRUE : FALSE;
}

/*====== End of fader.c ======*/

