/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - gfd - VramManager
  File:     gfd_BitArrayPlttVramMan.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.4 $
 *---------------------------------------------------------------------------*/

#ifndef NNS_GFD_BITARRAYPLTTMAN_H_
#define NNS_GFD_BITARRAYPLTTMAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro.h>
#include <nnsys/gfd/VramManager/gfd_PlttVramMan_Types.h>



//------------------------------------------------------------------------------
// パレットマネージャ
//------------------------------------------------------------------------------


void NNS_GfdInitBarPlttVramManager
( 
    u32     szByte, 
    u32     bytePerOneBlock, 
    u32*    pBitTbl, 
    u32     lengthOfBitTbl,
    BOOL    useAsDefault 
);

NNSGfdPlttKey NNS_GfdAllocBarPlttVram
( 
    u32         szByte, 
    BOOL        b4Pltt, 
    u32         bAllocFromLo
);

int NNS_GfdFreeBarPlttVram( NNSGfdPlttKey plttKey );

void NNS_GfdResetBarPlttVramState ( void );


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_GFD_BITARRAYPLTTMAN_H_
