/*---------------------------------------------------------------------------*
  Project:  NitroDWC Libraries
  File:     ./build/lobby/include/ppw_timer.h

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
 * @brief ポケモンロビーライブラリ 内部公開ヘッダ
 */

#ifndef PPW_TIMER_H_
#define PPW_TIMER_H_

// MatchComment: change to lobbyBase to prevent weird header includes
// (dwci_lobbyData.h needs PPW_LobbyTimer)
#include "dwci_lobbyBase.h"
class PPW_Lobby;

class PPW_LobbyTimer : public DWCi_Base
{
public:
    typedef BOOL (*Callback)(void* param);
private:
    s64 lastInvokedTime;
    s64 interval;
    void* param;
    Callback callback;
public:
    PPW_LobbyTimer(){};
    PPW_LobbyTimer(s64 _lastInvokedTime, s64 _interval, Callback _callback, void* _param, BOOL callImediate)
        : lastInvokedTime(_lastInvokedTime)
        , interval(_interval)
        , callback(_callback)
        , param(_param)
    {
        if(callImediate)
        {
            lastInvokedTime -= interval;
        }
    }
    
    // FALSEを返すとタイマーを削除する
    BOOL Process(s64 currentTime)
    {
        if(currentTime - lastInvokedTime > interval)
        {
            lastInvokedTime = currentTime;
            if(!callback(param))
            {
                return FALSE;
            }
        }
        return TRUE;
    }
};

class PPW_LobbyTimerManager : public DWCi_Base
{
    typedef std::map<u32, PPW_LobbyTimer, std::less<u32>, DWCi_Allocator<std::pair<u32, PPW_LobbyTimer> > > TimerContainer;
    TimerContainer timers;
public:
    PPW_LobbyTimerManager()
        : timers()
    {}
    
    ~PPW_LobbyTimerManager()
    {
        ClearTimer();
    }
    
    void AddTimer(u32 id, s64 interval, PPW_LobbyTimer::Callback callback, void* param, BOOL callImediate=FALSE)
    {
        timers.insert(std::make_pair(id, PPW_LobbyTimer(DWCi_Np_GetCurrentSecondByUTC(), interval, callback, param, callImediate)));
    }
    
    void Process()
    {
        s64 currentTime = DWCi_Np_GetCurrentSecondByUTC();
        for(TimerContainer::iterator it = timers.begin(); it != timers.end();)
        {
            if(!it->second.Process(currentTime))
            {
                // コールバック関数がFALSEを返したときは削除する
                timers.erase(it++);
                continue;
            }
            it++;
        }
    }
    
    void RemoveTimer(u32 id)
    {
        timers.erase(id);
    }
    
    void ClearTimer()
    {
        timers.clear();
    }
};

#endif
