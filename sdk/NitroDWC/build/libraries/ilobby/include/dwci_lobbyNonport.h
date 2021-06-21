/*---------------------------------------------------------------------------*
  Project:  NitroDWC Libraries
  File:     ./build/lobby/include/dwci_nonport.h

  Copyright 2005-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/
/**
 * @file
 *
 * @brief Wi-Fi ロビーライブラリ ヘッダ
 */

#ifndef DWCi_LOBBY_NONPORT_H_
#define DWCi_LOBBY_NONPORT_H_

#include <dwc.h>

#ifdef _NITRO
    #include <gs/chat/chat.h>
#else
    #include <chat/chat.h>
#endif

#ifdef _NITRO
    #include "nonport/dwci_lobbyNonportNitro.h"
#elif defined (_RVL) || defined (RVL)
    #include "nonport/dwci_lobbyNonportRvl.h"
#elif defined (WIN32)
    #include "nonport/dwci_lobbyNonportWin32.h"
#elif defined (_LINUX)
    #include "nonport/dwci_lobbyNonportLinux.h"
#else
    #error not supported
#endif

s64 DWCi_Np_GetCurrentSecondByUTC();

#endif
