/*---------------------------------------------------------------------------*
  Project:  NitroDWC Libraries
  File:     ./build/lobby/include/dwci_lobbyCallback.h

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
 * @brief Wi-Fi ロビーライブラリ チャンネルデータヘッダ
 */

#ifndef DWCi_LOBBY_CALLBACK_H_
#define DWCi_LOBBY_CALLBACK_H_

#include <set>
#include "dwci_lobbyBase.h"

class DWCi_AbstractCallback : public DWCi_Base
{
protected:
    u32 operationId;
    void* const param;
    
public:
    static const u32 INVALID_OPERATION_ID = 0xffffffff;
protected:
    DWCi_AbstractCallback(void* _param)
        : operationId()
        , param(_param)
    {
        static u32 nextOperationId = 0;
        operationId = nextOperationId++;
    }
public:
    // 検索用オブジェクト生成用
    DWCi_AbstractCallback(u32 _operationId, void*)
        : operationId(_operationId)
        , param(0)
    {}
    
    virtual ~DWCi_AbstractCallback(){}
    
    bool operator<(const DWCi_AbstractCallback& rhs) const
    {
        return operationId < rhs.operationId;
    }
    
    virtual u32 GetOperationId() const
    {
        return operationId;
    }
    
    virtual void* GetParam() const
    {
        return param;
    }
};

template<class T>
class DWCi_Callback : public DWCi_AbstractCallback
{
    T callback;
public:
    DWCi_Callback(T _callback, void* _param)
        : DWCi_AbstractCallback(_param)
        , callback(_callback)
    {}
    
    virtual ~DWCi_Callback(){}
    
    T GetCallback() const
    {
        return callback;
    }
};

class DWCi_CallbackManager : public DWCi_Base
{
    typedef std::set<DWCi_AbstractCallback*, DWCi_Base::ptr_less<DWCi_AbstractCallback>, DWCi_Allocator<DWCi_AbstractCallback*> > CallbackContainer;
    CallbackContainer callbacks;
public:
    DWCi_CallbackManager()
        : callbacks()
    {}
    
    ~DWCi_CallbackManager()
    {
        ClearCallback();
    }
    
    template<class T>
    u32 AddCallback(T callback, void* param)
    {
        DWCi_Callback<T>* callbackObj = new DWCi_Callback<T>(callback, param);
        std::pair<CallbackContainer::iterator, bool> ret;
        ret = callbacks.insert(callbackObj);
        
        DWC_ASSERTMSG(ret.second, "DWCi_CallbackManager::AddCallback failed to add.\n");
        return callbackObj->GetOperationId();
    }
    
    BOOL RemoveCallback(u32 operationId);
    
    template<class T>
    DWCi_Callback<T>* FindCallback(u32 operationId)
    {
        DWCi_AbstractCallback findObj(operationId, NULL);
        CallbackContainer::iterator ret = callbacks.find(&findObj);
        if(ret == callbacks.end())
        {
            DWC_ASSERTMSG(FALSE, "DWCi_CallbackManager::FindCallback: obj not found.");
            return NULL;
        }
        //DWC_ASSERTMSG(typeid(**ret) == typeid(DWCi_Callback<T>), "DWCi_CallbackManager::FindCallback: type mismatch. %s != %s",
        //              typeid(*ret).name(), typeid(DWCi_Callback<T>*).name());
        return (DWCi_Callback<T>*)*ret;
    }
    
    void ClearCallback();
};

#endif
