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

#include <lobby/dwci_lobby.h>
#include "dwci_lobbyBase.h"
#include "dwci_lobbyUtil.h"
#include "ppw_data.h"
#include "ppw_parser.h"
#include "ppw_profile.h"


/** 
 * @brief 定期処理。状態を遷移させます。
 */
void PPW_LobbyInnerProfile::Process()
{
    switch(state)
    {
    case STATE_PREPARING:
        state = (systemProfileInitialized && userProfileInitialized) ? STATE_DATARECVED : STATE_PREPARING;
        if(state == STATE_PREPARING)
        {
            break;
        }
        // STATE_DATARECVEDになった場合はreadyのチェックも行うのでbreakしない。ステートの遷移が遅れ、先に自分がjoinするのを防ぐ。
    case STATE_DATARECVED:
        if(systemProfile.Get().ready)
        {
            state = STATE_READY;
        }
        break;
    case STATE_READY:
        if(activated)
        {
            state = STATE_ACTIVATED;
        }
        break;
    case STATE_ACTIVATED:
        break;
    default:
        DWC_ASSERTMSG(FALSE, "PPW_LobbyInnerProfile::Process: Invalid state.");
        break;
    }
}



/** 
 * @brief すでに登録されていなければユーザを登録します。
 * 
 * @param id ユーザID
 * 
 * @retval TRUE 登録した。
 * @retval FALSE 登録済み。
 */
BOOL PPW_LobbyProfileManager::AddProfile(s32 id)
{
    std::pair<PPW_ProfileContainer::iterator, bool> ret;
    PPW_LobbyInnerProfile* newProfile = new PPW_LobbyInnerProfile(id);

    ret = profiles.insert(newProfile);
    if(!ret.second)
    {
        // 登録済み
        delete newProfile;
        return TRUE;
    }
    return TRUE;
}

/** 
 * @brief ユーザを削除します。
 * 
 * @param id ユーザID
 * 
 * @retval TRUE 削除した。
 * @retval FALSE 存在しなかった。
 */
BOOL PPW_LobbyProfileManager::RemoveProfile(s32 id)
{
    for(PPW_ProfileContainer::iterator it = profiles.begin(); it != profiles.end(); it++)
    {
        if((*it)->GetPid() == id)
        {
            delete *it;
            profiles.erase(it);
            return TRUE;
        }
    }
    return FALSE;
}

/** 
 * @brief ユーザを探します。
 * 
 * @param id ユーザID
 * 
 * @retval プロフィール 見つかった。
 * @retval NULL 見つからなかった。
 */
PPW_LobbyInnerProfile* PPW_LobbyProfileManager::FindProfile(s32 id)
{
    if(id == myProfile.GetPid())
    {
        return &myProfile;
    }
    PPW_ProfileContainer::const_iterator it = std::find_if(profiles.begin(), profiles.end(), PPW_LobbyInnerProfile::Equal(id));
    return it == profiles.end() ? NULL : *it;
}

/** 
 * @brief ユーザ情報を全て削除します。
 */
void PPW_LobbyProfileManager::ClearProfile()
{
    for(PPW_ProfileContainer::iterator it = profiles.begin(); it != profiles.end();)
    {
        delete *it;
        profiles.erase(it++);
    }
}

/** 
 * @brief Activate済み参加者が満員であるか調べます。
 * 
 * @retval TRUE 満員。
 * @retval FALSE まだ。
 */
BOOL PPW_LobbyProfileManager::IsFull() const
{
    // 負荷を軽くするためにまずvectorの大きさで比較し、次にactivate済みのサイズを確認する
    // 自分の分を1プラスして数える
    return profiles.size()+1 == PPW_LOBBY_MAX_PLAYER_NUM_MAIN && EnumId().size()+1 == PPW_LOBBY_MAX_PLAYER_NUM_MAIN;
}

/** 
 * @brief Activate済み参加者全員のpidを列挙します。
 * 
 * @retval ユーザIDのvector。
 */
std::vector<s32, DWCi_Allocator<s32> > PPW_LobbyProfileManager::EnumId() const
{
    std::vector<s32, DWCi_Allocator<s32> > userIds;
    userIds.reserve(profiles.size());
    for(PPW_ProfileContainer::const_iterator it = profiles.begin(); it != profiles.end(); it++)
    {
        if((*it)->IsActivated())
        {
            userIds.push_back((*it)->GetPid());
        }
    }
    return userIds;
}

/** 
 * @brief 自分のプロフィールを初期化します。
 * 
 * @param id ユーザID。
 * @param userProfile ユーザ定義プロフィール。
 * @param userProfileSize ユーザ定義プロフィールサイズ。
 */
void PPW_LobbyProfileManager::InitializeMyProfile(s32 id, const u8* userProfile, u32 userProfileSize)
{
    myProfile = PPW_LobbyInnerProfile(id);
    myProfile.SetSystemProfile(PPW_LobbyInnerProfile::SystemProfile());
    myProfile.SetUserProfile(userProfile, userProfileSize, FALSE);
    myProfile.Activate();
}

/** 
 * @brief 参加者全員のプロフィールを受信したか調べます。
 * 
 * @retval TRUE 受信した。
 * @retval FALSE まだ。
 */
BOOL PPW_LobbyProfileManager::CheckAllProfilesRecved() const
{
    // プロフィールを受信していないものを探す
    PPW_ProfileContainer::const_iterator it = std::find_if(profiles.begin(), profiles.end(), std::not1(std::mem_fun(&PPW_LobbyInnerProfile::IsDataRecved)));
    return it == profiles.end();
}

// 定期実行
// 必要に応じてユーザ接続コールバックを呼ぶ
void PPW_LobbyProfileManager::Process()
{
    // 接続した人のプロフィール登録確認
    for(PPW_ProfileContainer::iterator it = profiles.begin(); it != profiles.end(); it++)
    {
        (*it)->Process();
        switch((*it)->GetState())
        {
        case PPW_LobbyInnerProfile::STATE_PREPARING:
            break;
        case PPW_LobbyInnerProfile::STATE_DATARECVED:
            break;
        case PPW_LobbyInnerProfile::STATE_READY:
        {
            const PPW_LobbyInnerProfile::SystemProfile& systemProfile = (*it)->GetSystemProfileConfirm();
            const PPW_LobbyInnerProfile::UserProfile& userProfile = (*it)->GetUserProfileConfirm();
            
            PPW_LobbySystemProfile publicSystemProfile = systemProfile.ToPublicSystemProfile();
            
            // ユーザ接続完了コールバックを呼ぶ
            PPW_GetLobby()->GetCallbacks().lobbyPlayerConnectedCallback((*it)->GetPid(), &publicSystemProfile, DWCi_GetVectorBuffer(userProfile.data),
                                                                            userProfile.data.size());
            (*it)->Activate();
            break;
        }
        case PPW_LobbyInnerProfile::STATE_ACTIVATED:
            break;
        default:
            DWC_ASSERTMSG(FALSE, "PPW_LobbyProfileManager::Process: Invalid state.");
        }
    }
}
