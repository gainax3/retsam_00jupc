/*---------------------------------------------------------------------------*
  Project:  NitroDWC Libraries
  File:     ./build/lobby/include/ppw_innerInfo.h

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

#ifndef PPW_INNERINFO_H_
#define PPW_INNERINFO_H_

#ifdef max  // gccのdequeでコンパイルエラーになるのでundef
    #undef max
#endif
#ifdef min  // gccのdequeでコンパイルエラーになるのでundef
    #undef min
#endif

#include <deque>
#include "dwci_lobbyData.h"
class PPW_Lobby;

class PPW_LobbyInnerInfoManager : public DWCi_Base
{
public:
    // チャンネルデータで共有するデータ
    struct TimeData
    {
        s64 openedTime;
        
        TimeData()
            : openedTime(PPW_LOBBY_INVALID_TIME)
        {}
        
        void Initialize()
        {
            openedTime = DWCi_Np_GetCurrentSecondByUTC();
        }
    };
private:
    enum STATE
    {
        NOTINITIALIZED,
        DOWNLOADING,
        DOWNLOADED,
        SHARING,
        SHARED,
        LOCKED
    };
    
    struct LessScheduleRecord : std::binary_function<const PPW_LobbyScheduleRecord&, const PPW_LobbyScheduleRecord&, bool>
    {
        bool operator()(const PPW_LobbyScheduleRecord& lhs, const PPW_LobbyScheduleRecord& rhs) const
        {
            return lhs.time < rhs.time;
        }
    };
    
    STATE state;
    BOOL isTimeDataSet;       // 初期システムデータをセット済みか
    
    TimeData timeData;
    s64 lockedTime;
    BOOL bLockReserved;
    
    // PPW_LobbyScheduleと同内容
    u32 lockTime;
    u32 random;
    u32 roomFlag;
    u8 roomType;
    u8 season;
    typedef std::vector<PPW_LobbyScheduleRecord, DWCi_Allocator<PPW_LobbyScheduleRecord> > ScheduleRecordContainer;
    ScheduleRecordContainer scheduleRecords;
    
    typedef std::deque<PPW_LobbyScheduleRecord, DWCi_Allocator<PPW_LobbyScheduleRecord> > ScheduleRecordRemainContainer;
    ScheduleRecordRemainContainer scheduleRecordsRemain;
    
    typedef std::vector<PPW_LobbyVipRecord, DWCi_Allocator<PPW_LobbyVipRecord> > VipRecordContainer;
    VipRecordContainer vipRecords;
    
    PPW_LobbyQuestionnaire questionnaire;
    BOOL bSetQuestionnaire;
public:
    PPW_LobbyInnerInfoManager();
    
    BOOL GetLobbyInfo(PPW_LobbySchedule* info, u32& size) const;
    BOOL SetLobbyInfo(const u8* buf, u32 size);
    BOOL CheckLobbyInfo(const u8* buf, u32 size);
    BOOL GetVipRecords(PPW_LobbyVipRecord* records, u32& num) const;
    BOOL SetVipRecord(const u8* buf, u32 size);
    BOOL CheckVipRecord(const u8* buf, u32 size);
    BOOL GetQuestionnaire(PPW_LobbyQuestionnaire* questionnaire);
    void SetQuestionnaire(const PPW_LobbyQuestionnaire* questionnaire);
    
    std::vector<u8, DWCi_Allocator<u8> > Serialize() const;
    void Process();
    BOOL NeedLock() const;
    BOOL Lock();
    
    // スケジュール及びVIPデータがダウンロードされた
    BOOL Downloaded()
    {
        state = DOWNLOADED;
        return TRUE;
    }
    
    BOOL IsDownloaded()
    {
        return state >= DOWNLOADED;
    }
    
    // 情報が共有された
    BOOL Shared()
    {
        state = SHARED;
        return TRUE;
    }
    
    BOOL IsShared() const
    {
        return state >= SHARED;
    }
    
    BOOL GetTimeDataIsSet() const
    {
        return isTimeDataSet;
    }
    
    void SetTimeData(TimeData _systemProfile)
    {
        isTimeDataSet = TRUE;
        timeData = _systemProfile;
    }
    
    void GetTimeInfo(PPW_LobbyTimeInfo& info) const
    {
        info.currentTime = DWCi_Np_GetCurrentSecondByUTC();
        info.openedTime = timeData.openedTime;
        info.lockedTime = lockedTime;
    }
    
    TimeData InitializeTimeData()
    {
        timeData.Initialize();
        return timeData;
    }
};

#endif
