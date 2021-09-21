/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - snd
  File:     sndarc_player.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.17 $
 *---------------------------------------------------------------------------*/

#ifndef NNS_SND_SNDARC_PLAYER_H_
#define NNS_SND_SNDARC_PLAYER_H_

#include <nitro/types.h>
#include <nnsys/snd/heap.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
	structure declaration
 ******************************************************************************/

struct NNSSndHandle;

/******************************************************************************
	public function declaration
 ******************************************************************************/

BOOL NNS_SndArcPlayerSetup( NNSSndHeapHandle heap );

BOOL NNS_SndArcPlayerStartSeq( struct NNSSndHandle* handle, int seqNo );
BOOL NNS_SndArcPlayerStartSeqArc( struct NNSSndHandle* handle, int seqArcNo, int index );

BOOL NNS_SndArcPlayerStartSeqEx(
    struct NNSSndHandle* handle,
    int playerNo,
    int bankNo,
    int playerPrio,
    int seqNo
);
BOOL NNS_SndArcPlayerStartSeqArcEx(
    struct NNSSndHandle* handle,
    int playerNo,
    int bankNo,
    int playerPrio,
    int seqArcNo,
    int index
);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NNS_SND_SNDARC_PLAYER_H_ */

