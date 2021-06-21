/*---------------------------------------------------------------------------*
  Project:  NitroDWC Libraries
  File:     ./src/lobby/dwci_lobbyNewHandler.cpp

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
 * @brief newハンドラ ソース
 */

#include <lobby/dwci_lobby.h>
#include "dwci_lobbyData.h"
#include "dwci_lobbyNewHandler.h"

static const std::size_t DWCi_LOBBY_FAIL_SAFE_BUF_SIZE = 1024;

static DWCi_Lobby*  s_iLobby        = NULL;
static void*        s_failSafeBuf   = NULL;


static void EmergencyNewHandler()
{
    DWC_ASSERTMSG(FALSE, "EmergencyNewHandler: Fatal error, close Chat SDK.");
    chatDisconnect(s_iLobby->GetChat());
    DWCi_Base::set_new_handler(0);
}

static void FreeFailSafeBufNewHandler()
{
    DWC_Printf(DWC_REPORTFLAG_ERROR, "FreeFailSafeBufNewHandler: Low memory. Please close lobby lib.");
    DWC_SAFE_FREE(s_failSafeBuf);
    s_iLobby->SetError(DWCi_LOBBY_ERROR_ALLOC);
    DWCi_Base::set_new_handler(EmergencyNewHandler);
}

BOOL DWCi_LobbyInitializeNewHandler(DWCi_Lobby* iLobby)
{
    s_iLobby = iLobby;
    DWC_SAFE_FREE(s_failSafeBuf);
    s_failSafeBuf = DWC_Alloc((DWCAllocType)0, DWCi_LOBBY_FAIL_SAFE_BUF_SIZE);
    if(!s_failSafeBuf)
    {
        return FALSE;
    }
    DWCi_Base::set_new_handler(FreeFailSafeBufNewHandler);
    return TRUE;
}

void DWCi_LobbyDestroyNewHandler()
{
    DWC_SAFE_FREE(s_failSafeBuf);
    s_failSafeBuf = NULL;
}
