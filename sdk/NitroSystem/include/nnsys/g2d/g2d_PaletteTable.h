/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d
  File:     g2d_PaletteTable.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.4 $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_PALETTETABLE_H_
#define NNS_G2D_PALETTETABLE_H_ 

#include <nitro.h>
#include <nnsys/g2d/g2d_config.h>

#ifdef __cplusplus
extern "C" {
#endif


//------------------------------------------------------------------------------
#define NNS_G2D_NUM_COLOR_PALETTE   16




//------------------------------------------------------------------------------
typedef struct NNSG2dPaletteSwapTable
{
    u16        paletteIndex[NNS_G2D_NUM_COLOR_PALETTE];

}NNSG2dPaletteSwapTable;






//------------------------------------------------------------------------------
void NNS_G2dInitializePaletteTable  ( NNSG2dPaletteSwapTable* pPlttTbl );
void NNS_G2dSetPaletteTableValue    ( NNSG2dPaletteSwapTable* pPlttTbl, u16 beforeIdx, u16 afterIdx );
u16 NNS_G2dGetPaletteTableValue     ( const NNSG2dPaletteSwapTable* pPlttTbl, u16 beforeIdx );








#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_G2D_PALETTETABLE_H_