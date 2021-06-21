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

#include "dwci_lobbyBase.h"

std::new_handler DWCi_Base::s_currentNewHandler = 0;


void* DWCi_Base::operator new(std::size_t size) throw()
{
    void* ptr;
    if(size == 0)
    {
        size = 1;
    }

    while(TRUE)
    {
        ptr = DWC_Alloc((DWCAllocType)0, size);
        
        if(ptr)
        {
            //DWC_Printf(DWC_REPORTFLAG_INFO, "DWCi_Base::operator new: allocated %d bytes at %p\n", size, ptr);
            return ptr;
        }
        DWC_Printf(DWC_REPORTFLAG_ERROR, "DWCi_Base::operator new: Warning, failed to allocate %d bytes.\n", size, ptr);
        
        std::new_handler handler = DWCi_Base::set_new_handler(0);
        DWCi_Base::set_new_handler(handler);
        
        if(handler)
        {
            (*handler)();
        }
        else
        {
            DWC_Printf(DWC_REPORTFLAG_ERROR, "DWCi_Base::operator new: Warning, failed to allocate.");
            //throw std::bad_alloc();
            return NULL;
        }
        
    }
}
