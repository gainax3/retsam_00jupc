/*---------------------------------------------------------------------------*
  Project:  NitroDWC Libraries
  File:     ./src/lobby/dwci_lobbyBase.cpp

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
 * @brief アロケータソース
 */

#include "dwci_lobbyCallback.h"


/** 
 * @brief 指定したオペレーションIDのコールバックを削除します。
 * 
 * @param[in] operationId オペレーションID。
 * 
 * @retval TRUE 成功。
 * @retval FALSE 見つからなかった。
 */
BOOL DWCi_CallbackManager::RemoveCallback(u32 operationId)
{
    DWCi_AbstractCallback findObj(operationId, NULL);
    CallbackContainer::iterator ret = callbacks.find(&findObj);
    if(ret == callbacks.end())
    {
        return FALSE;
    }
    delete *ret;
    callbacks.erase(ret);
    return TRUE;
}

/** 
 * @brief コールバックを全て削除します。
 */
void DWCi_CallbackManager::ClearCallback()
{
    for(CallbackContainer::iterator it = callbacks.begin(); it != callbacks.end();)
    {
        delete *it;
        callbacks.erase(it++);
    }
}
