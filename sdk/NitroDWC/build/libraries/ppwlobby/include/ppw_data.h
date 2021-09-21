/*---------------------------------------------------------------------------*
  Project:  NitroDWC Libraries
  File:     ./build/lobby/include/dwci_lobby.h

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
 * @brief ポケモンロビーライブラリ チャンネルデータヘッダ
 */

#ifndef PPW_DATA_H_
#define PPW_DATA_H_

#include <string>
#include <set>
#include "dwci_lobbyData.h"
#include "ppw_sender.h"
#include "ppw_innerInfo.h"
#include "ppw_profile.h"
#include "ppw_channel.h"
#include "ppw_timer.h"
#include "dwci_encsessioncpp.h"


class PPW_Lobby : public DWCi_Base
{
    static const s32 PPW_LOBBY_MAIN_CHANNEL_RETRY_COUNT_MAX = 3;
    
    PPW_LOBBY_STATE state;
    PPW_LOBBY_ERROR error;
    const PPW_LobbyCallbacks lobbyCallbacks;
    const s32 pid;
    const DWC_LOBBY_CHANNEL_PREFIX prefix;
    const BOOL isValidInitialize;
    
    // 参加しているユニークチャンネル名
    DWCi_String uniqueChannelName;
    
    BOOL createdNewMainChannel;
    BOOL recruitStarted;
    BOOL submitQuestionnaireStarted;
    s32 mainChannelRetryCount;                     // メインチャンネル入室リトライカウント
    PPW_LobbyRecruitInfo recruitInfo;   // 自分が募集している要項
    
    PPW_LobbyChannelManager channelManager;
    PPW_LobbyProfileManager profileManager;
    PPW_LobbyTimerManager timerManager;
    
    // サーバから受信したロビー設定
    PPW_LobbyInnerInfoManager lobbyInnerInfoManager;
    
public:
    PPW_Lobby(const PPW_LobbyCallbacks& _lobbyCallbacks, s32 _pid, DWC_LOBBY_CHANNEL_PREFIX _prefix, BOOL _isValidInitialize)
        : state(PPW_LOBBY_STATE_NOTINITIALIZED)
        , lobbyCallbacks(_lobbyCallbacks)
        , pid(_pid)
        , prefix(_prefix)
        , isValidInitialize(_isValidInitialize)
        , error(PPW_LOBBY_ERROR_NONE)
        , uniqueChannelName()
        , createdNewMainChannel(FALSE)
        , recruitStarted(FALSE)
        , submitQuestionnaireStarted(FALSE)
        , mainChannelRetryCount(0)
        , recruitInfo()
        , channelManager()
        , profileManager()
        , timerManager()
        , lobbyInnerInfoManager()
    {
    }
    
    // 継承禁止
    ~PPW_Lobby()
    {
    }
    
    PPW_LOBBY_STATE GetState() const
    {
        return state;
    }
    
    void SetState(PPW_LOBBY_STATE _state)
    {
        state = _state;
    }
    
    const PPW_LobbyCallbacks& GetCallbacks() const
    {
        return lobbyCallbacks;
    }
    
    s32 GetPid() const
    {
        return pid;
    }
    
    DWC_LOBBY_CHANNEL_PREFIX GetPrefix() const
    {
        return prefix;
    }
    
    DWCiEncServer GetEncServer() const
    {
        return prefix == DWC_LOBBY_CHANNEL_PREFIX_RELEASE ? DWCi_ENC_SERVER_RELEASE : DWCi_ENC_SERVER_DEBUG;
    }
    
    BOOL GetIsValidInitialize() const
    {
        return isValidInitialize;
    }
    
    PPW_LOBBY_ERROR GetError() const
    {
        return error;
    }
    
    void SetError(PPW_LOBBY_ERROR _error)
    {
        // エラーが上書きされないようにエラー無しは無視する
        if(_error == PPW_LOBBY_ERROR_NONE)
        {
            return;
        }
        
        error = _error;
        
        // ステートをエラーにしてほとんどの関数を呼べなくする。
        state = PPW_LOBBY_STATE_ERROR;
    }
    // 下位ライブラリエラーフラグを変換してエラーをセット
    void SetError(DWCi_LOBBY_ERROR _error)
    {
        // ここでコンパイルエラーが発生した場合はエラーコードの整合性がとれていないので要修正
        // PPW_LOBBY_ERROR_STATS_SESSIONの分がずれる。それまでのエラーは互換性があるのでよし
        //DWCi_STATIC_ASSERT(DWCi_LOBBY_ERROR_MAX == PPW_LOBBY_ERROR_MAX);
        
        SetError((PPW_LOBBY_ERROR)_error);
    }
    // 結果を変換して致命的エラーをセット
    void SetError(PPW_LOBBY_RESULT _error)
    {
        PPW_LOBBY_ERROR convertedError;
        switch(_error)
        {
        case PPW_LOBBY_RESULT_ERROR_ALLOC:
            convertedError = PPW_LOBBY_ERROR_ALLOC;
            break;
        case PPW_LOBBY_RESULT_ERROR_SESSION:
            convertedError = PPW_LOBBY_ERROR_SESSION;
            break;
        default:
            // それ以外は致命的エラーではない
            convertedError = PPW_LOBBY_ERROR_NONE;
            break;
        }
        
        SetError(convertedError);
    }
    
    BOOL GetCreatedNewMainChannel() const
    {
        return createdNewMainChannel;
    }
    
    void SetCreatedNewMainChannel(BOOL _createdNewMainChannel)
    {
        createdNewMainChannel = _createdNewMainChannel;
    }
    
    DWCi_String GetUniqueChannelName() const
    {
        return uniqueChannelName;
    }
    
    // リトライカウントを増加させる。オーバーしていたらFALSEを返す。
    BOOL IncrementMainChannelRetryCount()
    {
        if(mainChannelRetryCount >= PPW_LOBBY_MAIN_CHANNEL_RETRY_COUNT_MAX)
        {
            return FALSE;
        }
        mainChannelRetryCount++;
        return TRUE;
    }
    
    void ClearMainChannelRetryCount()
    {
        mainChannelRetryCount = 0;
    }
    
    void SetUniqueChannelName(const DWCi_String& _uniqueChannelName)
    {
        uniqueChannelName = _uniqueChannelName;
    }
    
    BOOL StartRecruit(const PPW_LobbyRecruitInfo& _recruitInfo, DWCi_String& _matchMakingString)
    {
        if(recruitStarted)
        {
            return FALSE;
        }
        recruitStarted = TRUE;
        recruitInfo = _recruitInfo;
        _matchMakingString =
            DWCi_SNPrintf<DWCi_String>(PPW_LOBBY_MAX_MATCHMAKING_STRING_LENGTH, "M%d_%u", pid, (unsigned long)current_time() % 1000);
        strncpy(recruitInfo.matchMakingString, _matchMakingString.c_str(), PPW_LOBBY_MAX_MATCHMAKING_STRING_LENGTH-1);
        return TRUE;
    }
    
    BOOL IsRecruiting() const
    {
        return recruitStarted;
    }
    
    const PPW_LobbyRecruitInfo& GetRecruitInfo() const
    {
        return recruitInfo;
    }
    
    BOOL UpdateRecruitInfo(const PPW_LobbyRecruitInfo& _recruitInfo)
    {
        if(!recruitStarted)
        {
            return FALSE;
        }
        recruitInfo = _recruitInfo;
        return TRUE;
    }
    
    BOOL StopRecruit()
    {
        if(!recruitStarted)
        {
            return FALSE;
        }
        recruitStarted = FALSE;
        return TRUE;
    }
    
    void SetSubmitQuestionnaireStarted(BOOL submitQuestionnaireStarted_)
    {
        submitQuestionnaireStarted = submitQuestionnaireStarted_;
    }
    
    BOOL GetSubmitQuestionnaireStarted() const
    {
        return submitQuestionnaireStarted;
    }
    
    void ProcessManagers()
    {
        timerManager.Process();
        profileManager.Process();
        channelManager.Process();
        lobbyInnerInfoManager.Process();
    }
    
    // 取得したときは参照を保持しないこと。
    PPW_LobbyChannelManager& GetChannelManager()
    {
        return channelManager;
    }
    
    // ショートカット関数
    PPW_Channel& GetMainChannel()
    {
        return channelManager.GetChannel(PPW_LobbyChannelManager::TYPE_MAIN);
    }
    
    PPW_Channel& GetSubChannel()
    {
        return channelManager.GetChannel(PPW_LobbyChannelManager::TYPE_SUB);
    }
    
    void SetMainChannel(const PPW_Channel& channel)
    {
        channelManager.SetChannel(PPW_LobbyChannelManager::TYPE_MAIN, channel);
    }
    
    void SetSubChannel(const PPW_Channel& channel)
    {
        channelManager.SetChannel(PPW_LobbyChannelManager::TYPE_SUB, channel);
    }
    
    // 取得したときは参照を保持しないこと。
    PPW_LobbyProfileManager& GetProfileManager()
    {
        return profileManager;
    }
    
    // 取得したときは参照を保持しないこと。
    PPW_LobbyTimerManager& GetTimerManager()
    {
        return timerManager;
    }
    
    // 取得したときは参照を保持しないこと。
    PPW_LobbyInnerInfoManager& GetLobbyInnerInfoManager()
    {
        return lobbyInnerInfoManager;
    }
};

PPW_Lobby* PPW_GetLobby();

#endif
