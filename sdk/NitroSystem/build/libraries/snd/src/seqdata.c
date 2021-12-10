/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - snd
  File:     seqdata.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.9 $
 *---------------------------------------------------------------------------*/
#include <nnsys/snd/seqdata.h>

/* if include from Other Environment for exsample VC or BCB, */
/* please define NNS_FROM_TOOL                               */
#ifndef NNS_FROM_TOOL

#include <nnsys/misc.h>

#else

#define NNS_NULL_ASSERT(exp)           ((void) 0)

#ifdef _MSC_VER
#pragma warning( disable : 4018 ) // warning: signed/unsigned mismatch
#pragma warning( disable : 4311 ) // warning: pointer truncation from 'type' to 'type'
#pragma warning( disable : 4312 ) // warning: conversion from 'type' to 'type' of greater size
#endif

#endif // NNS_FROM_TOOL

/******************************************************************************
	private functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndSeqArcGetSeqCount

  Description:  シーケンスアーカイブ中のシーケンスの数を取得

  Arguments:    seqArc - シーケンスアーカイブ

  Returns:      シーケンスの数
 *---------------------------------------------------------------------------*/
u32 NNSi_SndSeqArcGetSeqCount( const NNSSndSeqArc* seqArc )
{
    NNS_NULL_ASSERT( seqArc );
    return seqArc->count;
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndSeqArcGetSeqInfo

  Description:  シーケンスアーカイブ中のシーケンス情報の取得

  Arguments:    seqArc - シーケンスアーカイブ
                seqNo  - インデックス番号

  Returns:      シーケンス情報構造体
 *---------------------------------------------------------------------------*/
const NNSSndSeqArcSeqInfo* NNSi_SndSeqArcGetSeqInfo(
    const NNSSndSeqArc* seqArc,
    int index
)
{
    NNS_NULL_ASSERT( seqArc );
    
	if ( index < 0 ) return NULL;
	if ( index >= seqArc->count ) return NULL;
    if ( seqArc->info[ index ].offset == NNS_SND_SEQ_ARC_INVALID_OFFSET ) return NULL;
	
	return & seqArc->info[ index ];
}

/*====== End of seqarc.c ======*/

