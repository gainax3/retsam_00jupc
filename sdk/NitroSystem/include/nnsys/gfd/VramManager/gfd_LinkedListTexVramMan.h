/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - gfd - VramManager
  File:     gfd_LinkedListTexVramMan.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.8 $
 *---------------------------------------------------------------------------*/
#ifndef NNS_GFD_LINKEDLISTTEXVRAMMAN_H_ 
#define NNS_GFD_LINKEDLISTTEXVRAMMAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro.h>
#include <nnsys/gfd/VramManager/gfd_VramMan.h>
#include <nnsys/gfd/VramManager/gfd_TexVramMan_Types.h>
#include <nnsys/gfd/VramManager/gfd_LinkedListVramMan.h>


//------------------------------------------------------------------------------
// ä÷êîêÈåæ
//------------------------------------------------------------------------------
void NNS_GfdDumpLnkTexVramManager();
void NNS_GfdDumpLnkTexVramManagerEx( 
    NNSGfdLnkDumpCallBack           pFuncForNrm, 
    NNSGfdLnkDumpCallBack           pFuncFor4x4, 
    void*                           pUserData );

u32 NNS_GfdGetLnkTexVramManagerWorkSize( u32 numMemBlk );

void NNS_GfdInitLnkTexVramManager
( 
    u32     szByte, 
    u32     szByteFor4x4,
    void*   pManagementWork, 
    u32     szByteManagementWork,
    BOOL    useAsDefault
);

NNSGfdTexKey    NNS_GfdAllocLnkTexVram( u32 szByte, BOOL is4x4comp, u32 opt );
int             NNS_GfdFreeLnkTexVram( NNSGfdTexKey memKey );
void            NNS_GfdResetLnkTexVramState( void );


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_GFD_LINKEDLISTTEXVRAMMAN_H_
