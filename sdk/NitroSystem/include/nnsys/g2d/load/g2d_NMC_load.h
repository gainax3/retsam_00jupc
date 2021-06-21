/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d - load
  File:     g2d_NMC_load.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.9 $
 *---------------------------------------------------------------------------*/
 
#ifndef NNS_G2D_NMC_LOAD_H_
#define NNS_G2D_NMC_LOAD_H_

#include <nitro.h>
#include <nnsys/g2d/g2d_config.h>
#include <nnsys/g2d/g2d_Data.h>

#ifdef __cplusplus
extern "C" {
#endif

BOOL NNS_G2dGetUnpackedMultiCellBank
( 
    void*                       pNmcrFile, 
    NNSG2dMultiCellDataBank**   ppMCBank 
);
void NNS_G2dUnpackNMC( NNSG2dMultiCellDataBank* pMCellData );
const NNSG2dMultiCellData* NNS_G2dGetMultiCellDataByIdx( const NNSG2dMultiCellDataBank* pDataBank, u16 idx );


#ifdef __SNC__
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void NNS_G2dPrintNodeHierData   ( const NNSG2dMultiCellHierarchyData* p1) NNS_G2D_DEBUG_FUNC_DECL_END
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void NNS_G2dPrintMultiCellInfo  ( const NNSG2dMultiCellData* p1) NNS_G2D_DEBUG_FUNC_DECL_END
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void NNS_G2dPrintMultiCellBank  ( const NNSG2dMultiCellDataBank* p1) NNS_G2D_DEBUG_FUNC_DECL_END
#else
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void NNS_G2dPrintNodeHierData   ( const NNSG2dMultiCellHierarchyData* ) NNS_G2D_DEBUG_FUNC_DECL_END
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void NNS_G2dPrintMultiCellInfo  ( const NNSG2dMultiCellData* ) NNS_G2D_DEBUG_FUNC_DECL_END
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void NNS_G2dPrintMultiCellBank  ( const NNSG2dMultiCellDataBank* ) NNS_G2D_DEBUG_FUNC_DECL_END
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_G2D_NMC_LOAD_H_
