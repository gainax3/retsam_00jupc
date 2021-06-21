/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - snd
  File:     heap.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.10 $
 *---------------------------------------------------------------------------*/

#ifndef NNS_SND_HEAP_H_
#define NNS_SND_HEAP_H_

#include <nitro/types.h>
#include <nitro/snd.h>
#include <nnsys/fnd.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
	macro definition
 ******************************************************************************/

#define NNS_SND_HEAP_INVALID_HANDLE NNS_FND_HEAP_INVALID_HANDLE

/******************************************************************************
	structure declarations
 ******************************************************************************/

struct NNSSndHeap;

/******************************************************************************
	type definition
 ******************************************************************************/

typedef void (*NNSSndHeapDisposeCallback)( void* mem, u32 size, u32 data1, u32 data2 );
typedef struct NNSSndHeap* NNSSndHeapHandle;

/******************************************************************************
	public function declarations
 ******************************************************************************/

NNSSndHeapHandle NNS_SndHeapCreate( void* startAddress, u32 size );
void NNS_SndHeapDestroy( NNSSndHeapHandle heap );

void* NNS_SndHeapAlloc( NNSSndHeapHandle heap, u32 size, NNSSndHeapDisposeCallback callback, u32 data1, u32 data2 );
void NNS_SndHeapClear( NNSSndHeapHandle heap );

int NNS_SndHeapSaveState( NNSSndHeapHandle heap );
void NNS_SndHeapLoadState( NNSSndHeapHandle heap, int level );
int NNS_SndHeapGetCurrentLevel( NNSSndHeapHandle heap );

u32 NNS_SndHeapGetSize( NNSSndHeapHandle heap );
u32 NNS_SndHeapGetFreeSize( NNSSndHeapHandle heap );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NNS_SND_HEAP_H_ */

