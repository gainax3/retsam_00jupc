/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d - load
  File:     g2d_NSC_load.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.6 $
 *---------------------------------------------------------------------------*/
 
#ifndef NNS_G2D_NSC_LOAD_H_
#define NNS_G2D_NSC_LOAD_H_

#include <nitro.h>
#include <nnsys/g2d/g2d_config.h>
#include <nnsys/g2d/g2d_Data.h>

#ifdef __cplusplus
extern "C" {
#endif


BOOL NNS_G2dGetUnpackedScreenData( void* pNscrFile, NNSG2dScreenData** ppScrData );

#ifdef __SNC__
NNS_G2D_DEBUG_FUNC_DECL_BEGIN
void NNS_G2dPrintScreenData( const NNSG2dScreenData* p1)
NNS_G2D_DEBUG_FUNC_DECL_END
#else
NNS_G2D_DEBUG_FUNC_DECL_BEGIN
void NNS_G2dPrintScreenData( const NNSG2dScreenData* )
NNS_G2D_DEBUG_FUNC_DECL_END
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_G2D_NSC_LOAD_H_
