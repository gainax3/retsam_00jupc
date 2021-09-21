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

#include "dwci_lobbyData.h"

inline void DWCi_LOBBY_ASSERT_IF_NOT_MODIFIED_CHANNEL_NAME(const DWCi_String& channelName)
{
    DWC_ASSERTMSG(channelName.find("#") == 0, "Internal error. channelName is not modified.");
    (void)channelName;
}

/** 
 * @brief 内部ロビーライブラリオブジェクトコンストラクタ
 * 
 * @param[in] 
 */
DWCi_Lobby::DWCi_Lobby(const DWCi_String& _gameName, const DWCi_String& _secretKey, DWC_LOBBY_CHANNEL_PREFIX _prefix, s32 _pid,
                       const DWCi_String& _chatNick, const DWCi_String& _chatUser, const DWCi_String& _chatName, const DWCi_LobbyGlobalCallbacks* _globalCallbacks)
    : chat(NULL)
    , state(DWCi_LOBBY_STATE_NOTINITIALIZED)
    , pid(_pid)
    , chatNick(_chatNick)
    , chatUser(_chatUser)
    , chatName(_chatName)
    , gameName(_gameName)
    , secretKey(_secretKey)
    , baseChannelName()
    , retryCount(0)
    , error(DWCi_LOBBY_ERROR_NONE)
    , connectingWaitCount(0)
    , serverDisconnectedMe(FALSE)
    , globalCallbacks(*_globalCallbacks)
    , users()
    , channels()
    , callbackManager()
{
    const char* const channelPrefix[] =
    {
        "R",    // DWC_LOBBY_CHANNEL_PREFIX_RELEASE
        "D",    // DWC_LOBBY_CHANNEL_PREFIX_DEBUG
        "D1",   // DWC_LOBBY_CHANNEL_PREFIX_DEBUG1
        "D2",   // DWC_LOBBY_CHANNEL_PREFIX_DEBUG2
        "D3",   // DWC_LOBBY_CHANNEL_PREFIX_DEBUG3
        "D4",   // DWC_LOBBY_CHANNEL_PREFIX_DEBUG4
        "D5",   // DWC_LOBBY_CHANNEL_PREFIX_DEBUG5
        "D6",   // DWC_LOBBY_CHANNEL_PREFIX_DEBUG6
        "D7",   // DWC_LOBBY_CHANNEL_PREFIX_DEBUG7
        "D8",   // DWC_LOBBY_CHANNEL_PREFIX_DEBUG8
        "D9",   // DWC_LOBBY_CHANNEL_PREFIX_DEBUG9
        "T"     // DWC_LOBBY_CHANNEL_PREFIX_TEST
    };
    
    // #GSP!gameName!T|R|D_ 形式のチャンネル名を作る
    baseChannelName = DWCi_String(DWCi_LOBBY_CHANNEL_NAME_PREFIX) + DWCi_LOBBY_CHANNEL_NAME_SEPARATOR
        + gameName + DWCi_LOBBY_CHANNEL_NAME_SEPARATOR
        + DWCi_String(channelPrefix[_prefix]) + DWCi_LOBBY_CHANNEL_NAME_SEPARATOR;
    DWC_Printf(DWC_REPORTFLAG_INFO, "Created DWCi_Lobby\n");
}

/** 
 * @brief 内部ロビーライブラリオブジェクトデストラクタ
 */
DWCi_Lobby::~DWCi_Lobby()
{
    DWC_Printf(DWC_REPORTFLAG_INFO, "Deleted DWCi_Lobby\n");
}

/** 
 * @brief 省略されたチャンネル名をフルの名前に戻します。
 * 
 * @param[in] channelNameSuffix 省略されたチャンネル名。
 * 
 * @retval 戻したチャンネル名。
 */
DWCi_String DWCi_Lobby::ModifyChannelName(const DWCi_String& channelNameSuffix) const
{
    return baseChannelName + channelNameSuffix;
}

/** 
 * @brief チャンネル名の#GSP!<gamename>!を省略したものに変換します。
 * 
 * @param[in] channelName チャンネル名。
 * 
 * @retval 変換されたチャンネル名。
 */
DWCi_String DWCi_Lobby::UnmodifyChannelName(const DWCi_String& channelName) const
{
    DWCi_LOBBY_ASSERT_IF_NOT_MODIFIED_CHANNEL_NAME(channelName);
    return channelName.substr(baseChannelName.length());
}

/** 
 * @brief 複数のチャンネル名の#GSP!<gamename>!を省略したものに変換します。
 * 
 * @param[in] numChannels チャンネル数。
 * @param[in] channelNames チャンネル名へのポインタの配列。
 * 
 * @retval 変換されたチャンネル名。
 */
std::vector<char*, DWCi_Allocator<char*> > DWCi_Lobby::UnmodifyChannelNames(int numChannels, const char** channelNames) const
{
    // channelNamesのポインタをbaseChannelName.length()ずつずらしたものをvectorにコピーする
    std::vector<char*, DWCi_Allocator<char*> > result;

    for(int i = 0; i<numChannels; i++)
    {
        result.push_back((char*)(channelNames[i] + baseChannelName.length()));
    }
    //std::transform(test.begin(), test.end(), result.begin(), std::bind1st(std::plus<char*>(), (int)baseChannelName.length()));
    
    return result;
}

/** 
 * @brief 一意となるニックネームを生成します。
 * 
 * @param[in] pid 生成元になるPID。
 * 
 * @retval 生成したニックネーム。
 */
DWCi_String DWCi_Lobby::CreateNickName(s32 pid)
{
    // ニックネームを"N_(PID(最大10桁))_(乱数(最大3桁))にする
    return DWCi_SNPrintf<DWCi_String>(DWCi_MAX_CHAT_NICK_SIZE, _T("N_%d_%u"), pid, (unsigned long)current_time() % 1000);
}

/** 
 * @brief ニックネームをPIDに変換します。
 * 
 * @param[in] nick ニックネーム
 * 
 * @retval pid 成功
 * @retval DWC_LOBBY_INVALID_USER_ID 失敗
 */
s32 DWCi_Lobby::NickNameToPid(const DWCi_String& nick)
{
    std::vector<DWCi_String, DWCi_Allocator<DWCi_String> > ret(DWCi_SplitByChars<DWCi_String>(nick, "_"));
    if(ret.size() < 2)
    {
        return DWC_LOBBY_INVALID_USER_ID;
    }
    int pid = atoi(ret[1].c_str());
    if(pid == 0)
    {
        return DWC_LOBBY_INVALID_USER_ID;
    }
    return pid;
}

/** 
 * @brief チャンネルを追加します。
 * 
 * @param[in] channelName チャンネル名
 * @param[in] channelCallbacks チャンネルに関連づけるコールバック
 * 
 * @retval TRUE 成功。
 * @retval FALSE 失敗。すでに登録済み。
 */
BOOL DWCi_Lobby::AddChannel(const DWCi_String channelName, const DWCi_LobbyChannelCallbacks& channelCallbacks, s32 limit)
{
    DWCi_LOBBY_ASSERT_IF_NOT_MODIFIED_CHANNEL_NAME(channelName);
    std::pair<ChannelContainer::iterator, bool> result;
    result = channels.insert(std::make_pair(channelName, DWCi_LobbyChannel(channelCallbacks, limit)));
    return (BOOL)result.second;
}

/** 
 * @brief チャンネルコールバックを取得します。
 * 
 * @param[in] channelName チャンネル名
 * 
 * @retval チャンネルコールバック。
 * @retval NULL 失敗。チャンネルが見つからない。
 */
const DWCi_LobbyChannelCallbacks* DWCi_Lobby::GetChannelCallbacks(const DWCi_String& channelName) const
{
    DWCi_LOBBY_ASSERT_IF_NOT_MODIFIED_CHANNEL_NAME(channelName);
    ChannelContainer::const_iterator it = channels.find(channelName);
    if(it == channels.end())
    {
        return NULL;
    }
    return &it->second.channelCallbacks;
}

/** 
 * @brief ユーザを追加します。
 * 
 * @param[in] nick ニックネーム
 * @param[in] channelName チャンネル名
 * 
 * @retval 追加した人のユーザID
 * @retval DWC_LOBBY_INVALID_USER_ID 失敗。不正なnick、もしくはチャンネルが見つからない。
 */
s32 DWCi_Lobby::AddUser(const DWCi_String& nick, const DWCi_String& channelName)
{
    DWCi_LOBBY_ASSERT_IF_NOT_MODIFIED_CHANNEL_NAME(channelName);
    s32 userId = NickNameToPid(nick);
    
    if(userId == DWC_LOBBY_INVALID_USER_ID)
    {
        return DWC_LOBBY_INVALID_USER_ID;
    }
    
    // チャンネル情報に追加する。
    ChannelContainer::iterator it = channels.find(channelName);
    if(it == channels.end())
    {
        DWC_ASSERTMSG(FALSE, "DWCi_Lobby::AddUser: no such channelName.");
        return DWC_LOBBY_INVALID_USER_ID;
    }
    it->second.userIds.insert(userId);
    
    // ユーザ情報に追加する。もし同じIDがいた場合は上書きする。(再接続したときに昔のものが残っていることがある)
    users[userId] = nick;
    return userId;
}

/** 
 * @brief ユーザを複数人追加します。
 * 
 * @param[in] numUsers ユーザ数
 * @param[in] users ニックネームの配列
 * @param[in] channelName チャンネル名
 * 
 * @retval 追加もしくは上書きした人のユーザIDのvector。追加していない人はvector内に含まれない。
 */
std::vector<s32, DWCi_Allocator<s32> > DWCi_Lobby::AddUsers(int numUsers, const char** users, const DWCi_String& channelName)
{
    DWCi_LOBBY_ASSERT_IF_NOT_MODIFIED_CHANNEL_NAME(channelName);
    std::vector<s32, DWCi_Allocator<s32> > userIds;
    userIds.reserve((std::size_t)numUsers);
	for(int i = 0; i < numUsers; i++)
    {
        // 過去の自分は登録しない。ユーザIDが同じでニックが違うものが過去の自分
        if(NickNameToPid(users[i]) == GetPid() && GetChatNick() != users[i])
        {
            continue;
        }
        s32 userId = AddUser(users[i], channelName);
        if(userId != DWC_LOBBY_INVALID_USER_ID)
        {
            userIds.push_back(userId);
        }
    }
    return userIds;
}

/** 
 * @brief もし全てのチャンネルに指定したユーザがいなかったらユーザ情報からユーザを削除します。
 * 
 * @param[in] userId 削除するユーザID。
 * 
 * @retval TRUE 削除した。
 * @retval FALSE まだチャンネルに残っていたので削除しなかった。
 */
BOOL DWCi_Lobby::RemoveIfNoUserInChannels(s32 userId)
{
    // もし全てのチャンネル情報にこのユーザがいなければユーザ情報からも削除
    ChannelContainer::const_iterator it;
    it = std::find_if(channels.begin(), channels.end(), ExistUserInChannelContainer(userId));
    if(it == channels.end())
    {
        users.erase(userId);
        return TRUE;
    }
    return FALSE;
}

/** 
 * @brief 指定したユーザをチャンネルから削除します。
 * 
 * @param[in] nick 削除するユーザID。
 * @param[in] channelName 削除するチャンネル名。
 * 
 * @retval TRUE 削除した。
 * @retval FALSE 削除しなかった。ニックネームが不正、もしくはチャンネルが見つからない(internal error)、もしくはチャンネル内にそのユーザがいない。
 */
BOOL DWCi_Lobby::RemoveUser(const DWCi_String& nick, const DWCi_String& channelName)
{
    DWCi_LOBBY_ASSERT_IF_NOT_MODIFIED_CHANNEL_NAME(channelName);
    s32 userId = NickNameToPid(nick);
    
    if(userId == DWC_LOBBY_INVALID_USER_ID)
    {
        return FALSE;
    }
    
    // チャンネル情報から削除
    ChannelContainer::iterator it = channels.find(channelName);
    if(it == channels.end())
    {
        DWC_ASSERTMSG(FALSE, "DWCi_Lobby::RemoveUser: no such channelName.");
        return FALSE;
    }
    BOOL result = (BOOL)it->second.userIds.erase(userId);
    
    if(!result)
    {
        return FALSE;
    }
    
    // もし全てのチャンネル情報にこのユーザがいなければユーザ情報からも削除
    RemoveIfNoUserInChannels(userId);
    return TRUE;
}

/** 
 * @brief 指定したチャンネルにいるユーザとチャンネルの情報を削除します。
 * 
 * n^2のオーダーなので遅い
 * 
 * @param[in] channelName 削除するチャンネル名。
 * 
 * @retval TRUE 削除した。
 * @retval FALSE 指定したチャンネルが見つからない。
 */
BOOL DWCi_Lobby::RemoveChannel(const DWCi_String& channelName)
{
    DWCi_LOBBY_ASSERT_IF_NOT_MODIFIED_CHANNEL_NAME(channelName);
    ChannelContainer::iterator it = channels.find(channelName);
    if(it == channels.end())
    {
        return FALSE;
    }
    
    // 一時バッファにIDを保存
    std::vector<s32, DWCi_Allocator<s32> > tempBuf;
    tempBuf.assign(it->second.userIds.begin(), it->second.userIds.end());
    
    channels.erase(channelName);
    
    // もし全てのチャンネル情報にこのユーザがいなければユーザ情報からも削除
    std::for_each(tempBuf.begin(), tempBuf.end(), std::bind1st(std::mem_fun(&DWCi_Lobby::RemoveIfNoUserInChannels), this));
    return TRUE;
}

/** 
 * @brief 指定したニックネームのユーザを捜します(ニックネームの一致も確認する)
 * 
 * @param[in] nick ニックネーム。
 * 
 * @retval ユーザID
 * @retval DWC_LOBBY_INVALID_USER_ID 見つからなかった。
 */
s32 DWCi_Lobby::FindUser(const DWCi_String& nick) const
{
    s32 userId = NickNameToPid(nick);
    
    if(userId == DWC_LOBBY_INVALID_USER_ID)
    {
        return DWC_LOBBY_INVALID_USER_ID;
    }
    
    // 存在しているか確認する
    UserContainer::const_iterator it = users.find(userId);
    if(it == users.end())
    {
        return DWC_LOBBY_INVALID_USER_ID;
    }
    // ユーザ名も確認する。(再接続したユーザはnickが変化するのでその確認)
    if(it->second != nick)
    {
        return DWC_LOBBY_INVALID_USER_ID;
    }
    return userId;
}

/** 
 * @brief 指定したユーザIDのユーザを捜します。
 * 
 * @param[in] userId ユーザID。
 * 
 * @retval ニックネーム。
 * @retval "" 見つからなかった。
 */
DWCi_String DWCi_Lobby::FindUser(s32 userId) const
{
    UserContainer::const_iterator it = users.find(userId);
    if(it == users.end())
    {
        return "";
    }
    return it->second;
}

/** 
 * @brief ニックネームを変更します。
 * 
 * @param[in] oldNick 古いニックネーム。 
 * @param[in] newNick 新しいニックネーム。
 * 
 * @retval 変更されたユーザID。
 * @retval DWC_LOBBY_INVALID_USER_ID InternalError。
 */
s32 DWCi_Lobby::RenameUser(const DWCi_String& oldNick, const DWCi_String& newNick)
{
    s32 userId = FindUser(oldNick);
    users.erase(userId);
    std::pair<UserContainer::iterator, bool> ret;

    ret = users.insert(std::make_pair(userId, newNick));
    if(!ret.second)
    {
        DWC_ASSERTMSG(FALSE, "DWCi_Lobby::RenameUser: Internal error. There is same user.");
        return DWC_LOBBY_INVALID_USER_ID;
    }
    return userId;
}

/** 
 * @brief 指定したチャンネルに参加している全てのユーザIDを返します。
 * 
 * @param[in] channelName チャンネル名。
 * 
 * @retval ユーザIDのvector。
 * @retval 空のvector 指定したチャンネルに参加していない。
 */
std::vector<s32, DWCi_Allocator<s32> > DWCi_Lobby::EnumUsers(const DWCi_String& channelName) const
{
    DWCi_LOBBY_ASSERT_IF_NOT_MODIFIED_CHANNEL_NAME(channelName);
    ChannelContainer::const_iterator it = channels.find(channelName);
    if(it == channels.end())
    {
        // 見つからないときはFALSE
        return std::vector<s32, DWCi_Allocator<s32> >();
    }
    
    std::vector<s32, DWCi_Allocator<s32> > userIds;
    userIds.assign(it->second.userIds.begin(), it->second.userIds.end());
    return userIds;
}

/** 
 * @brief チャンネルが有効化されているか調べます。
 * 
 * @param[in] channelName 修飾されたチャンネル名。 
 * 
 * @retval TRUE 有効化されている。
 * @retval FALSE 有効化されていないかチャンネルが見つからない。
 */
BOOL DWCi_Lobby::IsChannelActivated(const DWCi_String& channelName) const
{
    DWCi_LOBBY_ASSERT_IF_NOT_MODIFIED_CHANNEL_NAME(channelName);
    ChannelContainer::const_iterator it = channels.find(channelName);
    if(it == channels.end())
    {
        // 見つからないときはFALSE
        return FALSE;
    }
    return it->second.activated;
}

/** 
 * @brief チャンネルが有効化します。
 * 
 * @param[in] channelName チャンネル名。 
 * 
 * @retval TRUE 成功。
 * @retval FALSE チャンネルが見つからない。
 */
BOOL DWCi_Lobby::ActivateChannel(const DWCi_String& channelName)
{
    DWCi_LOBBY_ASSERT_IF_NOT_MODIFIED_CHANNEL_NAME(channelName);
    ChannelContainer::iterator it = channels.find(channelName);
    if(it == channels.end())
    {
        // 見つからないときはFALSE
        return FALSE;
    }
    it->second.activated = TRUE;
    return TRUE;
}

/** 
 * @brief チャンネルのデフォルト制限人数(::DWCi_LobbyJoinChannelLimitAsync関数で指定したもの)を取得します。
 * 
 * @param[in] channelName チャンネル名。 
 * 
 * @retval 0以上 成功。制限人数。
 * @retval -1 チャンネルが見つからない。
 */
s32 DWCi_Lobby::GetDefaultChannelLimit(const DWCi_String& channelName)
{
    DWCi_LOBBY_ASSERT_IF_NOT_MODIFIED_CHANNEL_NAME(channelName);
    ChannelContainer::iterator it = channels.find(channelName);
    if(it == channels.end())
    {
        DWC_Printf(DWC_REPORTFLAG_WARNING, "DWCi_Lobby::GetDefaultChannelLimit: No such channel: %s\n", channelName.c_str());
        return -1;
    }
    return it->second.limit;
}
