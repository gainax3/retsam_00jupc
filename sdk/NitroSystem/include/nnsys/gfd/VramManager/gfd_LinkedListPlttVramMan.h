/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - gfd - VramManager
  File:     gfd_LinkedListPlttVramMan.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.8 $
 *---------------------------------------------------------------------------*/
#ifndef NNS_GFD_LINKEDLISTPLTTVRAMMAN_H_ 
#define NNS_GFD_LINKEDLISTPLTTVRAMMAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro.h>
#include <nnsys/gfd/VramManager/gfd_VramMan.h>
#include <nnsys/gfd/VramManager/gfd_PlttVramMan_Types.h>
#include <nnsys/gfd/VramManager/gfd_LinkedListVramMan.h>



//------------------------------------------------------------------------------
// ä÷êîêÈåæ
//------------------------------------------------------------------------------
void NNS_GfdDumpLnkPlttVramManager();
void NNS_GfdDumpLnkPlttVramManagerEx( 
    NNSGfdLnkDumpCallBack   pFunc, 
    void*                   pUserData );

u32 NNS_GfdGetLnkPlttVramManagerWorkSize( u32 numMemBlk );

void NNS_GfdInitLnkPlttVramManager
( 
    u32     szByte, 
    void*   pManagementWork, 
    u32     szByteManagementWork,
    BOOL    useAsDefault
);

NNSGfdPlttKey   NNS_GfdAllocLnkPlttVram( u32 szByte, BOOL b4Pltt, u32 opt );
int             NNS_GfdFreeLnkPlttVram( NNSGfdPlttKey plttKey );
void            NNS_GfdResetLnkPlttVramState( void );


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_GFD_LINKEDLISTPLTTVRAMMAN_H_
