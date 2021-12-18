/*---------------------------------------------------------------------------*
  Project:  NitroDWC Libraries
  File:     ./src/lobby/lobby.c

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
 * @brief ロビーライブラリのラッパーユーティリティー ソース
 */

#include "dwci_encsessioncpp.h"

DWCi_EncSession* DWCi_EncSession::session = NULL;
DWCi_EncSession* DWCi_EncSession::endSession = NULL;
