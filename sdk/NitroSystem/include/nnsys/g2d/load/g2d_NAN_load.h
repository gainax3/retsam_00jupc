/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d - load
  File:     g2d_NAN_load.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.13 $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_NAN_LOAD_H_
#define NNS_G2D_NAN_LOAD_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <nitro.h>
#include <nnsys/g2d/g2d_config.h>
#include <nnsys/g2d/g2d_Data.h>

BOOL NNS_G2dGetUnpackedAnimBank( void* pNanrFile, NNSG2dAnimBankData** ppAnimBank );
BOOL NNS_G2dGetUnpackedMCAnimBank( void* pNanrFile, NNSG2dAnimBankData** ppAnimBank );

void NNS_G2dUnpackNAN( NNSG2dAnimBankData* pData );

//------------------------------------------------------------------------------
NNS_G2D_INLINE u16 NNS_G2dGetNumAnimSequence( const NNSG2dAnimBankData* pAnimBank )
{
    NNS_G2D_NULL_ASSERT( pAnimBank );
    return pAnimBank->numSequences;
}

const NNSG2dAnimSequenceData* NNS_G2dGetAnimSequenceByIdx( const NNSG2dAnimBankData* pAnimBank, u16 idx );


//------------------------------------------------------------------------------
const char* NNS_G2dGetAnimSequenceTypeStr  ( NNSG2dAnimationType );


//------------------------------------------------------------------------------
//#include "nnsys/g2d/load/g2di_Begin_LoadDebugFuncDecl.h"

#ifdef __SNC__
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void        NNS_G2dPrintAnimContents       ( const void* p1, NNSG2dAnimationElement p2) NNS_G2D_DEBUG_FUNC_DECL_END
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void        NNS_G2dPrintAnimFrame          ( const NNSG2dAnimFrameData* p1, NNSG2dAnimationElement p2) NNS_G2D_DEBUG_FUNC_DECL_END
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void        NNS_G2dPrintAnimSequence       ( const NNSG2dAnimSequenceData* p1) NNS_G2D_DEBUG_FUNC_DECL_END
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void        NNS_G2dPrintAnimBank           ( const NNSG2dAnimBankData* p1) NNS_G2D_DEBUG_FUNC_DECL_END
#else
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void        NNS_G2dPrintAnimContents       ( const void* , NNSG2dAnimationElement ) NNS_G2D_DEBUG_FUNC_DECL_END
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void        NNS_G2dPrintAnimFrame          ( const NNSG2dAnimFrameData* , NNSG2dAnimationElement ) NNS_G2D_DEBUG_FUNC_DECL_END
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void        NNS_G2dPrintAnimSequence       ( const NNSG2dAnimSequenceData* ) NNS_G2D_DEBUG_FUNC_DECL_END
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void        NNS_G2dPrintAnimBank           ( const NNSG2dAnimBankData* ) NNS_G2D_DEBUG_FUNC_DECL_END
#endif

//#include "nnsys/g2d/load/g2di_End_LoadDebugFuncDecl.h"
//------------------------------------------------------------------------------

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_G2D_NAN_LOAD_H_
