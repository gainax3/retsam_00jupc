/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - gfd - VramManager
  File:     gfd_BitArrayTexVramMan.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.8 $
 *---------------------------------------------------------------------------*/

#ifndef NNS_GFD_BITARRAYVRAMMAN_H_
#define NNS_GFD_BITARRAYVRAMMAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro.h>
#include <nnsys/gfd/VramManager/gfd_TexVramMan_Types.h>



//------------------------------------------------------------------------------
// ä÷êîêÈåæ
//------------------------------------------------------------------------------

void NNS_GfdInitBarTexVramManager
( 
    u32     numSlot, 
    u32     bytePerOneBlock, 
    u32*    pBitTbl, 
    u32     lengthOfBitTbl,
    BOOL	useAsDefault
);

NNSGfdTexKey    NNS_GfdAllocBarTexVram( u32 szByte, BOOL is4x4comp, u32 opt );
int             NNS_GfdFreeBarTexVram( NNSGfdTexKey memKey );
void            NNS_GfdResetBarTexVramState( void );




#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_GFD_BITARRAYVRAMMAN_H_

