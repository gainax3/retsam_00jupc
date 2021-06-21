/*---------------------------------------------------------------------------*
  Project:  NitroDWC Libraries
  File:     ./build/lobby/include/ppw_channel.h

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

#ifndef PPW_CHANNEL_H_
#define PPW_CHANNEL_H_

#include "dwci_lobbyData.h"
class PPW_Lobby;

class PPW_Channel : public DWCi_Base
{
    PPW_LOBBY_CHANNEL_KIND kind;
    DWCi_String name;
    BOOL isUserListEnumerated;  // ユーザリストを列挙済みか
    BOOL isModeSet;             // 初期チャンネルモードをセット済みか
    
public:
    PPW_Channel()
        : kind(PPW_LOBBY_CHANNEL_KIND_INVALID)
        , name()
    {
    }
    PPW_Channel(PPW_LOBBY_CHANNEL_KIND _kind, const DWCi_String& _name)
        : kind(_kind)
        , name(_name)
        , isUserListEnumerated(FALSE)
        , isModeSet(FALSE)
    {
    }
    
    // チャンネルに入っているもしくは入室中ならTRUE
    BOOL Exist() const
    {
        if(kind != PPW_LOBBY_CHANNEL_KIND_INVALID)
        {
            return TRUE;
        }
        return FALSE;
    }
    
    PPW_LOBBY_CHANNEL_KIND GetKind() const
    {
        return kind;
    }
    
    DWCi_String GetName() const
    {
        return name;
    }
    
    void SetUserListIsEnumerated()
    {
        isUserListEnumerated = TRUE;
    }
    
    BOOL GetUserListIsEnumerated() const
    {
        return isUserListEnumerated;
    }
    
    void SetModeIsSet()
    {
        isModeSet = TRUE;
    }
    
    BOOL GetModeIsSet() const
    {
        return isModeSet;
    }
    
    void Initialize(PPW_LOBBY_CHANNEL_KIND _kind, const DWCi_String& _name)
    {
        kind = _kind;
        name = _name;
        isUserListEnumerated = FALSE;
        isModeSet = FALSE;
    }
    
    void Clear()
    {
        Initialize(PPW_LOBBY_CHANNEL_KIND_INVALID, DWCi_String());
    }
};

class PPW_LobbyChannelManager : public DWCi_Base
{
public:
    enum TYPE
    {
        TYPE_MAIN,
        TYPE_SUB
    };
private:
    PPW_Channel mainChannel;
    PPW_Channel subChannel;
public:
    PPW_LobbyChannelManager()
        : mainChannel()
        , subChannel()
    {}
    
    PPW_Channel& GetChannel(TYPE type)
    {
        switch(type)
        {
        case TYPE_MAIN:
            return mainChannel;
        case TYPE_SUB:
            return subChannel;
        default:
            DWC_ASSERTMSG(FALSE, "PPW_LobbyChannelManager::GetChannel: Invalid type.");
            return mainChannel;
        }
    }
    
    void SetChannel(TYPE type, const PPW_Channel& channel)
    {
        switch(type)
        {
        case TYPE_MAIN:
            mainChannel = channel;
            break;
        case TYPE_SUB:
            subChannel = channel;
            break;
        default:
            DWC_ASSERTMSG(FALSE, "PPW_LobbyChannelManager::SetChannel: Invalid type.");
            break;
        }
    }
    
    DWCi_String GetChannelName(PPW_LOBBY_CHANNEL_KIND channelKind) const
    {
        if(channelKind == PPW_LOBBY_CHANNEL_KIND_MAIN)
        {
            return mainChannel.GetName();
        }
        else
        {
            return subChannel.GetName();
        }
    }
    
    PPW_LOBBY_CHANNEL_KIND GetChannelKind(const DWCi_String& channelName) const
    {
        if(mainChannel.GetName() == channelName)
        {
            return mainChannel.GetKind();
        }
        
        if(subChannel.GetName() == channelName)
        {
            return subChannel.GetKind();
        }
        return PPW_LOBBY_CHANNEL_KIND_INVALID;
    }
    
    void Process()
    {}
};

#endif
