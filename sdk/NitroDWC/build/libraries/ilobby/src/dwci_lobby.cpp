/*---------------------------------------------------------------------------*
  Project:  NitroDWC Libraries
  File:     ./src/lobby/dwci_lobby.c

  Copyright 2005-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/
/**
 * @file dwci_lobby.h
 *
 * @brief Wi-Fi ロビーライブラリ ソース
 * 
 * Wi-Fi ロビーライブラリ
 */

/**
 * @page DWCi_LOBBY Wi-Fi ロビーライブラリ
 *
 * @section Introduction
 *
 * このライブラリによって、GameSpyのIRCサーバを介して他のプレイヤーと通信することができます。
 *
 * すべての通信はサーバを中継するので、できる限り通信量が少なくなるようにしてください。\n
 * 目安としては、チャンネル全体に送信するもの(::DWCi_LobbySendChannelMessage関数や::DWCi_LobbySetChannelData関数によるブロードキャスト)は、
 * 小さいメッセージの場合は一人につき毎秒10メッセージ以下、大きいメッセージの場合は毎秒5メッセージ以下に抑えてください。\n
 * さらに、一分間で50メッセージを超えないようにしてください。
 * ライブラリ内部でのメモリ確保はDWC_Alloc関数を使用しています。\n
 * 本ライブラリはスレッドセーフではありません。\n
 * 二つ以上のスレッドで同時にライブラリの関数を呼び出さないでください。\n
 * 本ライブラリでプレイヤーを識別するために使用するユーザIDはGameSpyProfileID(pid)と一致しています。\n
 * 自分のユーザIDは初期化後に::DWCi_LobbyGetMyUserId関数を使用することにより取得できます。\n
 * これはプレイヤーごとにユニークな値となります。\n
 * ただし、デバッグ用の::DWCi_LobbyInitializePidAsync関数を使って初期化したとき、複数のプレイヤーが
 * 同じpidを指定した場合ユーザIDがユニークではなくなってしまいます。\n
 * この場合の動作は不定ですので必ず異なったpidを指定するようにしてください。\n
 * すべてのchar型のパラメータには0x5Cを除く0x22-0x7Eの範囲の文字及び終端の0x00を指定してください。\n
 * 
 * 
 * @section Usage
 *
 *<table border=1 bgcolor="mistyrose"><tr><td>
 *
 * ■ライブラリの初期化
 *
 * ::DWCi_LobbyInitializeAsync関数を使いライブラリを初期化します。\n
 * あらかじめDWC_LoginAsync関数又はDWC_NASLoginAsync関数でWi-Fiコネクションへの接続を完了している必要があります。 \n
 * ライブラリのステートが::DWCi_LOBBY_STATE_CONNECTEDになるとサーバへの接続が完了です。\n
 *
 * ■チャンネルへの参加
 * 
 * チャンネルへの参加は::DWCi_LobbyJoinChannelAsync関数で行います。一般的なIRCと同様に任意のチャンネル名を指定して行います。\n
 * もし指定したチャンネル名のチャンネルが存在した場合はそのチャンネルに入ります。もし無かった場合は新規にチャンネルを作成し入ります。\n
 * チャンネル内の全てのユーザはオペレータ権限を持っています。\n
 * 
 * ■メッセージの送受信
 *
 * ライブラリのステートが::DWCi_LOBBY_STATE_CONNECTEDのときに各種メッセージの送受信、チャンネルデータの送受信ができます。\n
 * チャンネルデータとは参加しているチャンネルのプレイヤー一人一人とチャンネル自体に関連づけられるデータです。\n
 *
 * ■ライブラリの終了
 *
 * ::DWCi_LobbyShutdownAsync関数を呼び出してライブラリを終了します。\n
 * この関数を呼び出した後、::DWCi_LobbyProcess関数が::DWCi_LOBBY_STATE_NOTINITIALIZEDを返すとシャットダウン完了です。\n
 * 
 * 
 * ■エラー処理
 *
 * 致命的なエラーが発生した場合は::DWCi_LobbyProcess関数が::DWCi_LOBBY_STATE_ERRORを返します。\n
 * この状態になるとほとんどの関数が使用不能になります。\n
 * また、内部でメモリ確保に失敗した場合、その関数での処理は正常に完了しますが、ライブラリのステートを::DWCi_LOBBY_STATE_ERRORに変更します。\n
 * そのため次の関数呼び出しで::DWCi_LOBBY_RESULT_ERROR_CONDITIONや::DWCi_LOBBY_STATE_ERRORが返ります。\n
 * この状態になった場合は::DWCi_LobbyGetLastError関数でエラー内容を確認後、::DWCi_LobbyShutdownAsync関数でシャットダウンしてください。\n
 * 致命的なエラーにはサーバへの接続失敗(::DWCi_LOBBY_ERROR_SESSION)、メモリ確保失敗(::DWCi_LOBBY_ERROR_ALLOC)などがあります。
 * また、このとき「サーバーとの通信中にエラーが発生しました。」というメッセージと共に::DWCi_LobbyToErrorCode関数を使用して得られたエラーコードを表示してください。\n
 * たとえば、::DWCi_LobbyGetLastError関数で::DWCi_LOBBY_ERROR_SESSIONを取得した場合は「42003」を表示してください。
 * その他、チャンネルへの参加に失敗した場合やチャンネルデータの取得に失敗した場合など、軽いエラーの場合はメッセージを表示する必要はありません。
 * アプリケーション側でロビー機能を続行できなくなったと判断した場合のメッセージ表示についてはアプリケーションに任されます。
 *
 *</td></tr></table>
 */

#include <lobby/dwci_lobby.h>
#include "dwci_lobbyBase.h"
#include "dwci_lobbyData.h"
#include "dwci_lobbyCallback.h"
#include "dwci_lobbyNewHandler.h"

// 定数定義
static const u32 DWCi_MAX_RETRY_COUNT = 3;
static const int DWCi_LOBBY_MESSAGE_TYPE_NORMAL_COMMAND = CHAT_UTM;

// スタティック変数宣言
static DWCi_Lobby*  s_iLobby      = NULL;

// 初期化されているかチェックする
#define RETURN_IF_NOTINITIALIZED(retval) if(!s_iLobby){return retval;}

// 致命的なエラーが起きていないかチェックする
#define RETURN_IF_ERROR_STATE(retval) \
    if(s_iLobby && s_iLobby->GetState() == DWCi_LOBBY_STATE_ERROR)\
    {\
        return retval;\
    }

// ステートがCONNECTEDであるかチェックする
#define RETURN_IF_NOTCONNECTED(retval) \
    if(s_iLobby && s_iLobby->GetState() != DWCi_LOBBY_STATE_CONNECTED)\
    {\
        return retval;\
    }

// MatchComment: new constants (id and interval)
static const u32 DWCi_LOBBY_TIMER_ID_0 = 0;

static const u32 DWCi_LOBBY_TIMER_INTERVAL_FOR_ID_0 = 30;

// 静的チェック専用ダミー関数
static void DummyStaticCheck()
{
    // 入室結果フラグが正しいかチェック
    DWCi_STATIC_ASSERT(DWCi_LOBBY_ENTER_RESULT_MAX == CHATBadChannelMask);
}

static void Raw(CHAT chat, const char* raw, void* param)
{
    RETURN_IF_NOTINITIALIZED((void)0);
    RETURN_IF_ERROR_STATE((void)0);
    
	DWC_Printf(DWC_REPORTFLAG_INFO, "RAW: %s\n", raw);
	
	(void)chat;
	(void)raw;
	(void)param;
}

static void Disconnected(CHAT chat, const char* reason, void* param)
{
    DWC_ASSERTMSG( s_iLobby, "s_iLobby: Invalid state. s_iLobby is NULL." );
    RETURN_IF_ERROR_STATE((void)0);

    // MatchComment: add this line
    s_iLobby->GetTimerManager().RemoveTimer(DWCi_LOBBY_TIMER_ID_0);

    // 正規の手順でシャットダウンしたかを確認する
    if(s_iLobby->GetState() == DWCi_LOBBY_STATE_CLOSING)
    {
	    DWC_Printf(DWC_REPORTFLAG_INFO, "Disconnected: %s\n", reason);
        DWC_SAFE_DELETE(s_iLobby);
    }
    else
    {
	    DWC_Printf(DWC_REPORTFLAG_ERROR, "Server disconnect me: %s\n", reason);
    
        // 回線が切れた(致命的エラー)
        s_iLobby->SetError(DWCi_LOBBY_ERROR_SESSION);
        
        // ここでchatDisconnect()を呼べないので(呼ぶとassertにかかる)代わりにフラグをセットする。
        s_iLobby->SetServerDisconnectedMe();
    }
	(void)chat;
	(void)reason;
	(void)param;
}

static void ChangedNickCallback(CHAT chat, CHATBool success, const char* oldNick, const char* newNick, void* param)
{
    DWC_ASSERTMSG( s_iLobby, "s_iLobby: Invalid state. s_iLobby is NULL." );
    RETURN_IF_ERROR_STATE((void)0);

	if(success)
	{
		DWC_Printf(DWC_REPORTFLAG_INFO, "Successfully changed");
	    DWC_Printf(DWC_REPORTFLAG_INFO, " nick from %s to %s\n", oldNick, newNick);
		s_iLobby->SetChatNick(newNick);
	}
	else
    {
		DWC_Printf(DWC_REPORTFLAG_ERROR, "Failed to change");
    }
	(void)chat;
	(void)oldNick;
	(void)param;
}

static void PrivateMessage(CHAT chat, const char* user, const char* message, int type, void* param)
{
    DWC_ASSERTMSG( s_iLobby, "s_iLobby: Invalid state. s_iLobby is NULL." );
    RETURN_IF_ERROR_STATE((void)0);

	DWC_Printf(DWC_REPORTFLAG_INFO, "Private message from %s: %s\n", user, message);
    
    // サーバからのメッセージか確認する
    if(DWCi_StrnicmpChar(user, "SERVER", 6) == 0)
    {
        // Excess Floodの場合警告する
        if(DWCi_StrnicmpChar(message, "Excess Flood", 12) != 0)
        {
            return;
        }
        std::vector<DWCi_String, DWCi_Allocator<DWCi_String> > result = DWCi_SplitByChars<DWCi_String>(message, " :");
        if(result.empty())
        {
            return;
        }
        u32 floodWeight = (u32)atoi(result.back().c_str());
        if(floodWeight == 0)
        {
            return;
        }
        // コールバックを呼ぶ
        s_iLobby->GetGlobalCallbacks().lobbyExcessFloodCallback(floodWeight, param);
        return;
    }

    if(type != DWCi_LOBBY_MESSAGE_TYPE_NORMAL_COMMAND)
    {
        return;
    }
    
    // userIdを取得
    s32 userId = s_iLobby->FindUser(user);
    if(userId == DWC_LOBBY_INVALID_USER_ID)
    {
        return;
    }

    // コールバックを呼ぶ
    s_iLobby->GetGlobalCallbacks().lobbyUserMassageReceivedCallback(userId, message, param);
    (void)chat;
	(void)type;
	(void)param;
}

static void Invited(CHAT chat, const char* channel, const char* user, void* param)
{
    DWC_ASSERTMSG( s_iLobby, "s_iLobby: Invalid state. s_iLobby is NULL." );
    RETURN_IF_ERROR_STATE((void)0);
    
	DWC_Printf(DWC_REPORTFLAG_INFO, "Invited by %s to %s\n", user, channel);
	
	(void)chat;
	(void)channel;
	(void)user;
	(void)param;
}

static void ChannelMessage(CHAT chat, const char* channel, const char* user, const char* message, int type, void* param)
{
    DWC_ASSERTMSG( s_iLobby, "s_iLobby: Invalid state. s_iLobby is NULL." );
    RETURN_IF_ERROR_STATE((void)0);
    
	DWC_Printf(DWC_REPORTFLAG_INFO, "%s, in %s, said \"%s\"\n", user, channel, message);

    if(type != DWCi_LOBBY_MESSAGE_TYPE_NORMAL_COMMAND)
    {
        return;
    }
    
    // userIdを取得
    s32 userId = s_iLobby->FindUser(user);

    // コールバックを呼ぶ
    const DWCi_LobbyChannelCallbacks* callbacks = s_iLobby->GetChannelCallbacks(channel);
    DWC_ASSERTMSG(callbacks, "ChannelMessage: Internal error. No callback info. channel: %s", channel);
    callbacks->lobbyChannelMassageReceivedCallback(s_iLobby->UnmodifyChannelName(channel).c_str(), userId, message, callbacks->param);
    
    (void)chat;
	(void)type;
	(void)param;
}

static void Kicked(CHAT chat, const char* channel, const char* user, const char* reason, void* param)
{
    DWC_ASSERTMSG( s_iLobby, "s_iLobby: Invalid state. s_iLobby is NULL." );
    RETURN_IF_ERROR_STATE((void)0);
    
	DWC_Printf(DWC_REPORTFLAG_INFO, "Kicked from %s by %s: %s\n", channel, user, reason);

    // チャンネルを削除
    s_iLobby->RemoveChannel(channel);
    
	(void)chat;
	(void)user;
	(void)reason;
	(void)param;
}

static void UserJoined(CHAT chat, const char* channel, const char* user, int mode, void* param)
{
    DWC_ASSERTMSG( s_iLobby, "s_iLobby: Invalid state. s_iLobby is NULL." );
    RETURN_IF_ERROR_STATE((void)0);

    // JoinChannelLimitAsyncの時に指定したデフォルトの人数を取得
    s32 limit = s_iLobby->GetDefaultChannelLimit(channel);
    
    // 制限人数のチェック
    if(limit != 0 && chatGetChannelNumUsers(s_iLobby->GetChat(), channel) > limit)
    {
        // オーバーした人が自動的に退室するので接続コールバックは呼ばない。
        DWC_Printf(DWC_REPORTFLAG_INFO, "%s joined %s but ignored: limit over.\n", user, channel);
        return;
    }
    
    // 同じuserId(再接続前の亡霊)が存在するか確認
    // 一度PIDに直してから過去のニックネームを検索する
    DWCi_String oldNick = s_iLobby->FindUser(DWCi_Lobby::NickNameToPid(user));
    
    // 該当ニックネームがこのチャンネルにいるか探す。RemoveUserで削除するついで存在したかを確認する。
    if(s_iLobby->RemoveUser(oldNick, channel))
    {
        // 再接続ユーザだったので一端切断コールバックを呼ぶ
        const DWCi_LobbyChannelCallbacks* callbacks = s_iLobby->GetChannelCallbacks(channel);
        DWC_ASSERTMSG(callbacks, "UserJoined: Internal error. No callback info. channel: %s", channel);
        callbacks->lobbyPlayerDisconnectedCallback(s_iLobby->UnmodifyChannelName(channel).c_str(), DWCi_Lobby::NickNameToPid(user), callbacks->param);
    }
    
    // ユーザを追加
    s32 userId = s_iLobby->AddUser(user, channel);

    if(userId == DWC_LOBBY_INVALID_USER_ID)
    {
        //DWC_ASSERTMSG(FALSE, "UserJoined: s_iLobby->AddUser failed.");
        DWC_Printf(DWC_REPORTFLAG_WARNING, "UserJoined: ignored unrecognized user: %s\n", user);
        return;
    }
    
	DWC_Printf(DWC_REPORTFLAG_INFO, "%s joined %s\n", user, channel);

    if(s_iLobby->IsChannelActivated(channel))
    {
        // チャンネルが有効なもの(JoinChannelコールバックを呼んだもの)のみ接続コールバックを呼ぶ
        const DWCi_LobbyChannelCallbacks* callbacks = s_iLobby->GetChannelCallbacks(channel);
        DWC_ASSERTMSG(callbacks, "UserJoined: Internal error. No callback info. channel: %s", channel);
        callbacks->lobbyPlayerConnectedCallback(s_iLobby->UnmodifyChannelName(channel).c_str(), userId, callbacks->param);
    }
	(void)chat;
	(void)param;
	(void)mode;
}

static void UserParted(CHAT chat, const char* channel, const char* user, int why, const char* reason, const char* kicker, void* param)
{
    DWC_ASSERTMSG( s_iLobby, "s_iLobby: Invalid state. s_iLobby is NULL." );
    RETURN_IF_ERROR_STATE((void)0);
    
	switch(why)
    {
    case CHAT_LEFT:
		DWC_Printf(DWC_REPORTFLAG_INFO, "%s left %s\n", user, channel);
        break;
	case CHAT_QUIT:
		DWC_Printf(DWC_REPORTFLAG_INFO, "%s quit: %s\n", user, reason);
        break;
	case CHAT_KICKED:
		DWC_Printf(DWC_REPORTFLAG_INFO, "%s was kicked from %s by %s: %s\n", user, channel, kicker, reason);
        break;
	case CHAT_KILLED:
		DWC_Printf(DWC_REPORTFLAG_INFO, "%s was killed: %s\n", user, reason);
        break;
	default:
		DWC_Printf(DWC_REPORTFLAG_INFO, "UserParted() called with unknown part-type\n");
        break;
    }
	
    // userIdを確認
    s32 userId = s_iLobby->FindUser(user);

    // すでに該当ユーザが再接続済みで名前が変化してリストに存在しないときは切断コールバックは呼ばない
    if(userId == DWC_LOBBY_INVALID_USER_ID)
    {
		DWC_Printf(DWC_REPORTFLAG_INFO, "Disconnected reconnected user.: %s\n", user);
        return;
    }
    
    // コールバックを呼ぶ
    const DWCi_LobbyChannelCallbacks* callbacks = s_iLobby->GetChannelCallbacks(channel);
    DWC_ASSERTMSG(callbacks, "UserParted: Internal error. No callback info. channel: %s", channel);
    callbacks->lobbyPlayerDisconnectedCallback(s_iLobby->UnmodifyChannelName(channel).c_str(), userId, callbacks->param);

    // ユーザを削除
    s_iLobby->RemoveUser(user, channel);
	(void)chat;
	(void)reason;
	(void)kicker;
	(void)param;
}

static void UserChangedNick(CHAT chat, const char* channel, const char* oldNick, const char* newNick, void* param)
{
    DWC_ASSERTMSG( s_iLobby, "s_iLobby: Invalid state. s_iLobby is NULL." );
    RETURN_IF_ERROR_STATE((void)0);
    
	DWC_Printf(DWC_REPORTFLAG_INFO, "%s changed nicks to %s\n", oldNick, newNick);
	
    // nickを変更することはないが念のため対応
    
    // ユーザのIDはそのままで名前を変更
    s_iLobby->RenameUser(oldNick, newNick);
    
	(void)chat;
	(void)channel;
	(void)param;
}

static void UserModeChanged(CHAT chat, const char* channel, const char* user, int mode, void* param)
{
    DWC_ASSERTMSG( s_iLobby, "s_iLobby: Invalid state. s_iLobby is NULL." );
    RETURN_IF_ERROR_STATE((void)0);
    
    DWCi_String modeStr;
	switch(mode)
    {
    case CHAT_NORMAL:
        modeStr = "normal";
        break;
    case CHAT_VOICE:
        modeStr = "voice";
        break;
	case CHAT_OP:
        modeStr = "ops";
        break;
    case (CHAT_VOICE | CHAT_OP):
		modeStr = "voice+ops";
        break;
    default:
        modeStr = "invalid";
        break;
    }
    
	DWC_Printf(DWC_REPORTFLAG_INFO, "%s's new mode in %s is %s\n", user, channel, modeStr.c_str());;
	
	(void)chat;
	(void)channel;
	(void)user;
	(void)param;
}

static void TopicChanged(CHAT chat, const char* channel, const char* topic, void* param)
{
    DWC_ASSERTMSG( s_iLobby, "s_iLobby: Invalid state. s_iLobby is NULL." );
    RETURN_IF_ERROR_STATE((void)0);
    
	DWC_Printf(DWC_REPORTFLAG_INFO, "The topic in %s changed to %s\n", channel, topic);
	
    const DWCi_LobbyChannelCallbacks* callbacks = s_iLobby->GetChannelCallbacks(channel);
    DWC_ASSERTMSG(callbacks, "TopicChanged: Internal error. No callback info. channel: %s", channel);
    callbacks->lobbyTopicChangedCallback(s_iLobby->UnmodifyChannelName(channel).c_str(), topic, callbacks->param);
    
	(void)chat;
	(void)channel;
	(void)topic;
	(void)param;
}

static void ChannelModeChanged(CHAT chat, const char* channel, CHATChannelMode* mode, void* param)
{
    DWC_ASSERTMSG( s_iLobby, "s_iLobby: Invalid state. s_iLobby is NULL." );
    RETURN_IF_ERROR_STATE((void)0);
    
	DWC_Printf(DWC_REPORTFLAG_INFO, "The mode in %s has changed:\n", channel);
	DWC_Printf(DWC_REPORTFLAG_INFO, "   InviteOnly: %d\n", mode->InviteOnly);
	DWC_Printf(DWC_REPORTFLAG_INFO, "   Private: %d\n", mode->Private);
	DWC_Printf(DWC_REPORTFLAG_INFO, "   Secret: %d\n", mode->Secret);
	DWC_Printf(DWC_REPORTFLAG_INFO, "   Moderated: %d\n", mode->Moderated);
	DWC_Printf(DWC_REPORTFLAG_INFO, "   NoExternalMessages: %d\n", mode->NoExternalMessages);
	DWC_Printf(DWC_REPORTFLAG_INFO, "   OnlyOpsChangeTopic: %d\n", mode->OnlyOpsChangeTopic);
	DWC_Printf(DWC_REPORTFLAG_INFO, "   OpsObeyChannelLimit: %d\n", mode->OpsObeyChannelLimit);
	if(mode->Limit == 0)
    {
		DWC_Printf(DWC_REPORTFLAG_INFO, "   Limit: N/A\n");
	}
    else
    {
		DWC_Printf(DWC_REPORTFLAG_INFO, "   Limit: %d\n", mode->Limit);
	}
	(void)chat;
	(void)channel;
	(void)param;
}

static void UserListUpdated(CHAT chat, const char* channel, void* param)
{
    DWC_ASSERTMSG( s_iLobby, "s_iLobby: Invalid state. s_iLobby is NULL." );
    RETURN_IF_ERROR_STATE((void)0);
    
	DWC_Printf(DWC_REPORTFLAG_INFO, "User list updated\n");
	
	(void)chat;
	(void)channel;
	(void)param;
}

static void ConnectCallback(CHAT chat, CHATBool success, int failureReason, void* param)
{
    DWC_ASSERTMSG( s_iLobby, "s_iLobby: Invalid state. s_iLobby is NULL." );
    RETURN_IF_ERROR_STATE((void)0);
    // MatchComment: add this line
    s_iLobby->GetTimerManager().RemoveTimer(DWCi_LOBBY_TIMER_ID_0);

	if (success == CHATFalse)
    {
		DWC_Printf(DWC_REPORTFLAG_ERROR, "Failed to connect (%d)\n", failureReason);
        s_iLobby->SetError(DWCi_LOBBY_ERROR_SESSION);
        // MatchComment: add this line
        s_iLobby->SetServerDisconnectedMe();
    }
	else
    {
		DWC_Printf(DWC_REPORTFLAG_INFO, "Connected\n");
        s_iLobby->SetState(DWCi_LOBBY_STATE_CONNECTINGWAIT);
    }
	(void)chat;
	(void)success;
	(void)failureReason;
	(void)param;
}

static void FillInUserCallback(CHAT chat, unsigned int IP, char user[128], void* param)
{
    DWC_ASSERTMSG( s_iLobby, "s_iLobby: Invalid state. s_iLobby is NULL." );
    RETURN_IF_ERROR_STATE((void)0);

    strncpy(user, s_iLobby->GetChatUser().c_str(), 128 - 1);

	(void)chat;
	(void)IP;
	(void)param;
}

static void NickErrorCallback(CHAT chat, int type, const char* nick, int numSuggestedNicks, const char** suggestedNicks, void* param)
{
    DWC_ASSERTMSG( s_iLobby, "s_iLobby: Invalid state. s_iLobby is NULL." );
    RETURN_IF_ERROR_STATE((void)0);

	if(type == CHAT_IN_USE)
	{
		DWC_Printf(DWC_REPORTFLAG_INFO, "The nick %s is already being used.\n", nick);
		s_iLobby->SetChatNick(DWCi_Lobby::CreateNickName(s_iLobby->GetPid()));
		chatRetryWithNick(chat, s_iLobby->GetChatNick().c_str());
	}
	else if(type == CHAT_INVALID)
	{
		DWC_Printf(DWC_REPORTFLAG_INFO, "The nick %s is invalid!\n", nick);
		// chatDisconnect(chat); THIS CRASHES
	
		// 10-14-2004: Added By Saad Nader
		// this is necessary as the function will fail if a new nick is not retries.
		////////////////////////////////////////////////////////////////////////////
		s_iLobby->SetChatNick(DWCi_Lobby::CreateNickName(s_iLobby->GetPid()));
		chatRetryWithNick(chat, s_iLobby->GetChatNick().c_str());
	}
	else if((type == CHAT_UNIQUENICK_EXPIRED) || (type == CHAT_NO_UNIQUENICK))
	{
		DWC_Printf(DWC_REPORTFLAG_INFO, "This account has no uniquenick or an expired uniquenick!\n");

		s_iLobby->SetChatNick(DWCi_Lobby::CreateNickName(s_iLobby->GetPid()));
		chatRegisterUniqueNick(chat, 2, s_iLobby->GetChatNick().c_str(), "");
	}
	else if(type == CHAT_INVALID_UNIQUENICK)
	{
		DWC_Printf(DWC_REPORTFLAG_INFO, "The uniquenick %s is invalid or in use\n", nick);
		DWC_Printf(DWC_REPORTFLAG_INFO, "There are %d suggested nicks:\n", numSuggestedNicks);

		for(int i = 0 ; i < numSuggestedNicks ; i++)
			DWC_Printf(DWC_REPORTFLAG_INFO, "   %s\n", suggestedNicks[i]);
	}

	// 10-14-2004: Added By Saad Nader
	// added for the addition of a new error code.
	////////////////////////////////////////////////////////////////////////////
	else if(type == CHAT_NICK_TOO_LONG)
	{
        DWC_ASSERTMSG( FALSE, "NickErrorCallback: Internal error. Nick name is too long." );
		s_iLobby->SetChatNick(DWCi_Lobby::CreateNickName(s_iLobby->GetPid()));
		chatRetryWithNick(chat, s_iLobby->GetChatNick().c_str());
	}
	(void)nick;
	(void)suggestedNicks;
	(void)param;
}

static void GetUserInfoCallback(CHAT chat, CHATBool success, const char* nick, const char* user, const char* name, const char* address,
                                int numChannels, const char** channels, void* param)
{
    DWC_ASSERTMSG( s_iLobby, "s_iLobby: Invalid state. s_iLobby is NULL." );
    RETURN_IF_ERROR_STATE((void)0);
    
	if(!success)
	{
		DWC_Printf(DWC_REPORTFLAG_INFO, "GetUserInfo failed\n");
		return;
	}

	DWC_Printf(DWC_REPORTFLAG_INFO, "%s's Info:\n", nick);
	DWC_Printf(DWC_REPORTFLAG_INFO, "   User: %s\n", user);
	DWC_Printf(DWC_REPORTFLAG_INFO, "   Name: %s\n", name);
	DWC_Printf(DWC_REPORTFLAG_INFO, "   Address: %s\n", address);
	DWC_Printf(DWC_REPORTFLAG_INFO, "   Channels (%d):\n", numChannels);
	for(int i = 0 ; i < numChannels ; i++)
    {
		DWC_Printf(DWC_REPORTFLAG_INFO, "      %s\n", channels[i]);
	}
    
	(void)chat;	
	(void)nick;	
	(void)user;	
	(void)name;	
	(void)address;	
	(void)numChannels;	
	(void)channels;
	(void)param;
}

static void EnumChannelsAllCallback(CHAT chat, CHATBool success, int numChannels, const char** channels, const char** topics, int* numUsers, void* param)
{
    DWC_ASSERTMSG( s_iLobby, "s_iLobby: Invalid state. s_iLobby is NULL." );
    RETURN_IF_ERROR_STATE((void)0);

    std::vector<char*, DWCi_Allocator<char*> > result = s_iLobby->UnmodifyChannelNames(numChannels, channels);

    // コールバックを呼ぶ
    DWCi_Callback<DWCi_LobbyEnumChannelsCallback>* callbackObj = s_iLobby->GetCallbackManager().FindCallback<DWCi_LobbyEnumChannelsCallback>((u32)param);
    callbackObj->GetCallback()(success == CHATTrue, numChannels, (const char**)DWCi_GetVectorBuffer(result),
                               topics, (s32*)numUsers, callbackObj->GetParam());
    s_iLobby->GetCallbackManager().RemoveCallback((u32)param);

	(void)chat;
}

static void EnumChannelsEachCallback(CHAT chat, CHATBool success, int index, const char* channel, const char* topic, int numUsers, void* param )
{
	if(!success)
	{
		DWC_Printf(DWC_REPORTFLAG_INFO, "EnumChannels failed\n");
		return;
	}

    DWC_Printf(DWC_REPORTFLAG_INFO, "ChatEnumChannelsCallbackEach called. index:%d\n", index);
    DWC_Printf(DWC_REPORTFLAG_INFO, "    Channel: %s\n", channel);
    DWC_Printf(DWC_REPORTFLAG_INFO, "    Topic: %s\n", topic);
    DWC_Printf(DWC_REPORTFLAG_INFO, "    NumUser: %d\n", numUsers);
    
	(void)param;
	(void)index;
	(void)channel;
	(void)topic;
	(void)numUsers;
	(void)chat;
}

static void EnumUsersCallback(CHAT chat, CHATBool success, const char* channel, int numUsers, const char** users, int* modes, void* param)
{
    DWC_ASSERTMSG( s_iLobby, "s_iLobby: Invalid state. s_iLobby is NULL." );
    RETURN_IF_ERROR_STATE((void)0);

    std::vector<s32, DWCi_Allocator<s32> > userIds;
    
    if(numUsers != 0)
    {
        userIds = s_iLobby->AddUsers(numUsers, users, channel);
    }
    
    

    // コールバックを呼ぶ
    DWCi_Callback<DWCi_LobbyEnumUsersCallback>* callbackObj = s_iLobby->GetCallbackManager().FindCallback<DWCi_LobbyEnumUsersCallback>((u32)param);
    callbackObj->GetCallback()(success == CHATTrue, s_iLobby->UnmodifyChannelName(channel).c_str(), (s32)userIds.size(),
                               DWCi_GetVectorBuffer(userIds), callbackObj->GetParam());
    s_iLobby->GetCallbackManager().RemoveCallback((u32)param);
    (void)chat;
    (void)modes;
}

static void EnumUsersAfterEnteredCallback(CHAT chat, CHATBool success, const char* channel, int numUsers, const char** users, int* modes, void* param)
{
    DWC_ASSERTMSG( s_iLobby, "s_iLobby: Invalid state. s_iLobby is NULL." );
    RETURN_IF_ERROR_STATE((void)0);
    
    if(success == CHATTrue)
    {
        s_iLobby->AddUsers(numUsers, users, channel);
    }

    // チャンネルを有効化
    s_iLobby->ActivateChannel(channel);

    // 入室コールバックを呼ぶ
    const DWCi_Callback<DWCi_LobbyJoinChannelCallback>* callbackObj =
        s_iLobby->GetCallbackManager().FindCallback<DWCi_LobbyJoinChannelCallback>((u32)param);
    callbackObj->GetCallback()(success == CHATTrue, DWCi_LOBBY_ENTER_RESULT_SUCCESS, s_iLobby->UnmodifyChannelName(channel).c_str(), callbackObj->GetParam());
    s_iLobby->GetCallbackManager().RemoveCallback((u32)param);

    if(success == CHATFalse)
    {
        // 失敗したので部屋を出る。
        DWCi_LobbyLeaveChannel(s_iLobby->UnmodifyChannelName(channel).c_str());
    }
    (void)chat;
    (void)modes;
}

static void EnterChannelCallback(CHAT chat, CHATBool success, CHATEnterResult result, const char* channel, void* param)
{
    DWC_ASSERTMSG( s_iLobby, "s_iLobby: Invalid state. s_iLobby is NULL." );
    RETURN_IF_ERROR_STATE((void)0);
    
    if(success == CHATTrue)
    {
        // JoinChannelLimitAsyncの時に指定したデフォルトの人数を取得
        s32 limit = s_iLobby->GetDefaultChannelLimit(channel);
    
        // DWCi_LobbyJoinChannelLimitAsyncでJoinしたかの確認
        if(limit != 0)
        {
            // 現在の人数を取得
            s32 currentNum = chatGetChannelNumUsers(s_iLobby->GetChat(), channel);
    
            // 現在の人数が制限値をオーバーしていたら退室する。入室前にLeaveしたとき(limit==-1)と、
            // 入室できていないとき(currentNum==-1)も念のため退室(DWCi_LobbyLeaveChannel)する)
            if(limit == -1)
            {
                success = CHATFalse;
                result = CHATEnterTimedOut; // 該当する理由がないのでタイムアウトにする。
            }
            else if(currentNum > limit || currentNum == -1)
            {
                success = CHATFalse;
                result = CHATChannelIsFull;
            }
            else
            {
                // オーバーしていなかったら人数制限をかける
                DWC_Printf(DWC_REPORTFLAG_INFO, "SetChannelLimit channel: %s, limit: %d\n", channel, limit);
                chatSetChannelLimit(s_iLobby->GetChat(), channel, limit);
            }
            DWC_Printf(DWC_REPORTFLAG_INFO, "CHATEnterResult: %d, currentNum %d\n", result, currentNum);
        }
    
    }
    
    // このライブラリ内で入室失敗させる場合もあるので再度成功判定をする
	if (success == CHATTrue)
    {
        // 自分自身をユーザに追加
        s32 userId = s_iLobby->AddUser(s_iLobby->GetChatNick(), channel);
    
        // 全てのユーザIDを取得(常に全員分のユーザIDを取得しておく必要がある)
        chatEnumUsers(chat, channel, EnumUsersAfterEnteredCallback, param, CHATFalse);
    }
    else
    {
        // チャンネル情報を削除(参加していないチャンネルに対してLeaveしても何も起こらないので、本当に入室失敗した場合でも呼んで良い)
        // 本当に入室失敗した場合に下位ライブラリからその他のコールバックが呼ばれることがあるのでそれも防止。
        DWCi_LobbyLeaveChannel(s_iLobby->UnmodifyChannelName(channel).c_str());
        
        // 入室失敗コールバックを呼ぶ
        const DWCi_Callback<DWCi_LobbyJoinChannelCallback>* callbackObj =
            s_iLobby->GetCallbackManager().FindCallback<DWCi_LobbyJoinChannelCallback>((u32)param);
        callbackObj->GetCallback()(FALSE, (DWCi_LOBBY_ENTER_RESULT)result, s_iLobby->UnmodifyChannelName(channel).c_str(), callbackObj->GetParam());
        s_iLobby->GetCallbackManager().RemoveCallback((u32)param);
    }
	(void)chat;
	(void)result;
	(void)channel;
}

static void GetChannelKeysCallback(CHAT chat, CHATBool success, const char* channel, const char* user, int num, const char** keys, const char** values, void* param)
{
    DWC_ASSERTMSG( s_iLobby, "s_iLobby: Invalid state. s_iLobby is NULL." );
    RETURN_IF_ERROR_STATE((void)0);
    
    // channelはsuccess == CHATFalseのときにNULLになる場合がある。
    if(!channel)
    {
        //DWC_ASSERTMSG( channel, "GetChannelKeysCallback: channel is NULL. Please check excess flood.");
        DWC_Printf( DWC_REPORTFLAG_WARNING, "GetChannelKeysCallback: channel is NULL. success: %d, num: %d.", success, num);
        DWC_Printf( DWC_REPORTFLAG_WARNING, "                      : channel: %s, user: %s.", channel ? channel : "NULL", user ? user : "NULL");
        return;
    }
    
    DWC_Printf(DWC_REPORTFLAG_INFO, "GetChannelKeysCallback called\n");
    
    s32 userId;

    if(user)
    {
        DWC_Printf(DWC_REPORTFLAG_INFO, "Channel user key changed. User:%s\n", user);
        // userIdを確認
        userId = s_iLobby->FindUser(user);
    }
    else
    {
        DWC_Printf(DWC_REPORTFLAG_INFO, "Channel key changed\n");
        userId = DWC_LOBBY_INVALID_USER_ID;
    }

    // コールバックを取り出す
    DWCi_Callback<DWCi_LobbyGetChannelDataCallback>* callbackObj = s_iLobby->GetCallbackManager().FindCallback<DWCi_LobbyGetChannelDataCallback>((u32)param);
    
    // コールバックを呼ぶ
    char* key = NULL;
    char* value = NULL;
    if(success == CHATTrue && num > 0)
    {
        key = (char*)keys[0];
        value = (char*)values[0];
    }
    callbackObj->GetCallback()(success == CHATTrue, s_iLobby->UnmodifyChannelName(channel).c_str(),
                               userId, key, value, callbackObj->GetParam());
    
    // コールバックを削除する
    s_iLobby->GetCallbackManager().RemoveCallback((u32)param);

    for(int i=0; i<num; i++)
    {
        DWC_Printf(DWC_REPORTFLAG_INFO, "index:%d, Key:%s, Value:%s\n", i, keys[i], values[i]);
    }
    (void)chat;
}

static void BroadcastKeyChanged(CHAT chat, const char* channel, const char* user, const char* key, const char* value, void* param )
{
    DWC_ASSERTMSG( s_iLobby, "s_iLobby: Invalid state. s_iLobby is NULL." );
    RETURN_IF_ERROR_STATE((void)0);
    
    DWC_Printf(DWC_REPORTFLAG_INFO, "BroadcastKeyChanged called\n");
    
    s32 userId;

    if(user)
    {
        DWC_Printf(DWC_REPORTFLAG_INFO, "Channel user key changed. User:%s\n", user);
        // userIdを確認
        userId = s_iLobby->FindUser(user);
    }
    else
    {
        DWC_Printf(DWC_REPORTFLAG_INFO, "Channel key changed\n", user);
        userId = DWC_LOBBY_INVALID_USER_ID;
    }
    DWC_Printf(DWC_REPORTFLAG_INFO, "Key:%s, Value:%s\n", key, value);
    
    const DWCi_LobbyChannelCallbacks* callbacks = s_iLobby->GetChannelCallbacks(channel);
    DWC_ASSERTMSG(callbacks, "BroadcastKeyChanged: Internal error. No callback info. channel: %s", channel);
    callbacks->lobbyBroadcastDataCallback(s_iLobby->UnmodifyChannelName(channel).c_str(), userId, key, value, callbacks->param);
    
    (void)chat;
    (void)param;
}

static void GetUserModeCallback(CHAT chat, CHATBool success, const char* channel, const char* user, int mode, void* param)
{
    DWC_ASSERTMSG( s_iLobby, "s_iLobby: Invalid state. s_iLobby is NULL." );
    RETURN_IF_ERROR_STATE((void)0);

	DWC_Printf(DWC_REPORTFLAG_INFO, "chatGetUserModeCallback called\n");

	if(success == CHATTrue)
	{
		DWC_Printf(DWC_REPORTFLAG_INFO, "name:%s, mode:%d\n", user, mode);
	}
	else
	{
		DWC_Printf(DWC_REPORTFLAG_ERROR, "chatGetUserModeCallback failed\n");
	}

	(void)param;
	(void)channel;
	(void)user;
	(void)mode;
	(void)chat;
}

static void GetGlobalKeysCallback(CHAT chat, CHATBool success, const char* user, int num, const char** keys, const char** values, void* param )
{
    DWC_ASSERTMSG( s_iLobby, "s_iLobby: Invalid state. s_iLobby is NULL." );
    RETURN_IF_ERROR_STATE((void)0);
    
	DWC_Printf(DWC_REPORTFLAG_INFO, "chatGetGlobalKeysCallback called\n");

	if(!success)
	{
		DWC_Printf(DWC_REPORTFLAG_INFO, "GetGlobalKeys failed\n");
		return;
	}

    if(user)
    {
        DWC_Printf(DWC_REPORTFLAG_INFO, "Global user key changed. User:%s\n", user);
    }
    else
    {
        DWC_Printf(DWC_REPORTFLAG_INFO, "Global key changed\n", user);
    }
    
    for(int i=0; i<num; i++)
    {
        DWC_Printf(DWC_REPORTFLAG_INFO, "index:%d, Key:%s, Value:%s\n", i, keys[i], values[i]);
    }
    
	(void)chat;
	(void)param;
	(void)user;
	(void)keys;
	(void)values;
}

static void GetChannelModeCallback(CHAT chat, CHATBool success, const char* channel, CHATChannelMode* mode, void* param)
{
    DWC_ASSERTMSG( s_iLobby, "s_iLobby: Invalid state. s_iLobby is NULL." );
    RETURN_IF_ERROR_STATE((void)0);
    
	if(success)
	{
	    ChannelModeChanged(chat, channel, mode, param);
    }
    else
    {
		DWC_Printf(DWC_REPORTFLAG_ERROR, "GetChannelModeCallback failed\n");
		return;
	}
    
    // コールバックを呼ぶ
    DWCi_Callback<DWCi_LobbyGetChannelModeCallback>* callbackObj = s_iLobby->GetCallbackManager().FindCallback<DWCi_LobbyGetChannelModeCallback>((u32)param);
    callbackObj->GetCallback()(success == CHATTrue, s_iLobby->UnmodifyChannelName(channel).c_str(),
                               (DWCi_LobbyChannelMode*)mode, callbackObj->GetParam());
    s_iLobby->GetCallbackManager().RemoveCallback((u32)param);

    (void)chat;
}

static void GetChannelBasicUserInfoCallback(CHAT chat, CHATBool success, const char* channel, const char* nick, const char* user, const char* address, void* param)
{
    DWC_ASSERTMSG( s_iLobby, "s_iLobby: Invalid state. s_iLobby is NULL." );
    RETURN_IF_ERROR_STATE((void)0);
    
	DWC_Printf(DWC_REPORTFLAG_INFO, "GetChannelBasicUserInfo called\n");

	if(success == CHATFalse)
	{
		DWC_Printf(DWC_REPORTFLAG_ERROR, "GetChannelBasicUserInfo failed\n");
		return;
	}

	DWC_Printf(DWC_REPORTFLAG_INFO, "Basic info Nick:%s, User:%s, Addr:%s\n", nick, user, address);
    (void)chat;
    (void)channel;
    (void)nick;
    (void)user;
    (void)address;
    (void)param;
}

BOOL ov66_2244758(void* param)
{
    s_iLobby->SetError(DWCi_LOBBY_ERROR_SESSION);

    (void)param;

    return FALSE;
}

    // 07 Dec 2021 pikalaxalt
    // 私は明らかに機械翻訳を使用して、この行がここにあることを表現しています。
inline void ProcessTimers() {
    s_iLobby->GetTimerManager().Process();
}

/**
 * @brief ロビーライブラリの処理を進めます。
 * 
 * ロビーライブラリの処理を進めます。\n
 * 毎ゲームフレーム程度の頻度で呼び出してください。\n
 * ::DWCi_LOBBY_STATE_ERRORを返したときは::DWCi_LobbyGetLastError関数でエラー内容を取得後::DWCi_LobbyShutdownAsync関数を呼んでシャットダウンしてください。\n
 * ::DWCi_LOBBY_STATE_CONNECTEDのときのみメッセージ送信、チャンネル入室などの関数を呼べます。
 * 
 * @retval ::DWCi_LOBBY_STATE_NOTINITIALIZED ロビーライブラリが初期化されていません。
 * @retval ::DWCi_LOBBY_STATE_CONNECTING サーバに接続を開始しています。
 * @retval ::DWCi_LOBBY_STATE_CONNECTINGWAIT サーバに接続中です。
 * @retval ::DWCi_LOBBY_STATE_CONNECTED サーバに接続しました。各種関数が呼べます。
 * @retval ::DWCi_LOBBY_STATE_CLOSING ロビーライブラリを終了中です。
 * @retval ::DWCi_LOBBY_STATE_ERROR ロビーライブラリが初期化されていません。
 */
DWCi_LOBBY_STATE DWCi_LobbyProcess()
{
    RETURN_IF_NOTINITIALIZED(DWCi_LOBBY_STATE_NOTINITIALIZED);
    RETURN_IF_ERROR_STATE(DWCi_LOBBY_STATE_ERROR);
    
    if(s_iLobby->GetChat())
    {
        // ここで各種コールバックが呼ばれる
        chatThink(s_iLobby->GetChat());
    }

    ProcessTimers();
    // コールバック中に致命的なエラーが発生していないかチェック
    RETURN_IF_ERROR_STATE(DWCi_LOBBY_STATE_ERROR);
    
    // リトライカウントがオーバーしているときはセッションエラーを返す
    if(s_iLobby->GetRetryCount() >= DWCi_MAX_RETRY_COUNT)
    {
        s_iLobby->ResetRetryCount();
        s_iLobby->SetError(DWCi_LOBBY_ERROR_SESSION);
        return s_iLobby->GetState();
    }
    
    // 処理を進める
    switch(s_iLobby->GetState())
    {
    case DWCi_LOBBY_STATE_NOTINITIALIZED:
        DWC_ASSERTMSG(FALSE, "DWCi_LobbyProcess: Invalid state: NOTINITIALIZED");
        break;
    case DWCi_LOBBY_STATE_CONNECTING:
        break;
    case DWCi_LOBBY_STATE_CONNECTINGWAIT:
        s_iLobby->WaitAfterConnect();
        break;
    case DWCi_LOBBY_STATE_CONNECTED:
        break;
    case DWCi_LOBBY_STATE_CLOSING:
        break;
    default:
        DWC_ASSERTMSG(FALSE, "DWCi_LobbyProcess: Invalid state");
        break;
    }
    
    return s_iLobby->GetState();
}

/**
 * @brief 発生している致命的なエラーの情報を取得します。
 * 
 * 発生している致命的なエラーの情報を取得します。
 * 
 * @retval ::DWCi_LOBBY_ERROR_NONE エラー無し。
 * @retval ::DWCi_LOBBY_ERROR_UNKNOWN 不明なエラー。
 * @retval ::DWCi_LOBBY_ERROR_ALLOC メモリ確保に失敗。
 * @retval ::DWCi_LOBBY_ERROR_SESSION 通信エラー。
 */
DWCi_LOBBY_ERROR DWCi_LobbyGetLastError()
{
    RETURN_IF_NOTINITIALIZED(DWCi_LOBBY_ERROR_NONE);
    
    return s_iLobby->GetError();
}

/**
 * @brief 発生したエラー(::DWCi_LOBBY_ERROR)から表示すべきエラーコードを得ます。
 * 
 * ::DWCi_LOBBY_ERRORから表示すべきエラーコードに変換します。
 * 
 * @param err 発生したエラー。
 * 
 * @retval 表示すべきエラーコード。
 */
s32 DWCi_LobbyToErrorCode(DWCi_LOBBY_ERROR err)
{
    return err + DWCi_LOBBY_ERROR_CODE_BASE;
}

/**
 * @brief ロビーライブラリで使用している自分のユーザIDを取得します。
 * 
 * ロビーライブラリで使用している自分のユーザIDを取得します。
 * 
 * @retval 自分のユーザID。
 * @retval ::DWC_LOBBY_INVALID_USER_ID 初期化していません。
 */
s32 DWCi_LobbyGetMyUserId()
{
    RETURN_IF_NOTINITIALIZED(DWC_LOBBY_INVALID_USER_ID);
    
    return s_iLobby->GetPid();
}

/**
 * @brief 指定したチャンネルに参加しているかを調べます。
 * 
 * 指定したチャンネルに参加しているかを調べます。
 * 
 * @param[in] channelName 調べるチャンネル名
 * 
 * @retval TRUE 参加している。
 * @retval FALSE 参加していない。
 */
BOOL DWCi_LobbyInChannel(const char* channelName)
{
    RETURN_IF_NOTINITIALIZED(FALSE);
    
    //s_iLobby->IsChannelActivated(s_iLobby->ModifyChannelName(channelName).c_str());
    return chatInChannel(s_iLobby->GetChat(), s_iLobby->ModifyChannelName(channelName).c_str()) == CHATTrue;
}

/** 
 * @brief ロビーライブラリを初期化します。
 * 
 * ロビーライブラリを初期化します。\n
 * この関数を呼ぶ前にあらかじめDWC_LoginAsync関数又はDWC_NASLoginAsync関数でWi-Fiコネクションへの接続を完了している必要があります。\n
 * この関数が成功した場合は::DWCi_LobbyProcess関数を毎ゲームフレーム程度の頻度で呼び出してください。
 * 
 * @param[in] gameName NULL終端されたゲーム名。
 * @param[in] secretKey NULL終端されたシークレットキー。
 * @param[in] prefix チャンネル名に付加するプリフィックス。製品用、デバッグ用、テスト用で入るチャンネルを分けることができます。\n
 *                   製品用には::DWC_LOBBY_CHANNEL_PREFIX_RELEASEを、デバッグ用には::DWC_LOBBY_CHANNEL_PREFIX_DEBUGを指定してください。
 * @param[in] globalCallbacks チャンネルに関わらないコールバック関数をセットします。
 * @param[in] dwcUserData 有効なDWCユーザデータオブジェクト。
 * 
 * @retval ::DWCi_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_PARAM 引数が不正。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_STATE すでに初期化されています。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_ALLOC メモリ確保に失敗。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_SESSION 通信エラー。
 */
DWCi_LOBBY_RESULT DWCi_LobbyInitializeAsync(const char* gameName, const char* secretKey, DWC_LOBBY_CHANNEL_PREFIX prefix,
                                            const DWCi_LobbyGlobalCallbacks* globalCallbacks, const DWCUserData* dwcUserData)
{
	// ユーザデータの正当性を検証
	if( !DWC_CheckUserData( dwcUserData )
		|| !DWC_CheckHasProfile( dwcUserData ))
    {
		// 不正なユーザデータ
        DWC_Printf(DWC_REPORTFLAG_ERROR, "DWCi_LobbyInitializeAsync: Invalid dwcUserData.\n");
		return DWCi_LOBBY_RESULT_ERROR_PARAM;
	}
    return DWCi_LobbyInitializePidAsync(gameName, secretKey, prefix, globalCallbacks, dwcUserData->gs_profile_id);
}

/** 
 * @brief ロビーライブラリを初期化します。(デバッグ用)
 * 
 * ロビーライブラリを初期化します。(デバッグ用)\n
 * この関数を呼ぶ前にあらかじめDWC_LoginAsync関数又はDWC_NASLoginAsync関数でWi-Fiコネクションへの接続を完了している必要があります。\n
 * ::DWCi_LobbyInitializeAsync関数はDWCUserData内に記録されているGameSpyProfileIDを使用するのに対し、こちらは任意のGameSpyProfileIDで初期化できます。\n
 * この関数が成功した場合は::DWCi_LobbyProcess関数を毎ゲームフレーム程度の頻度で呼び出してください。
 * 
 * @param[in] gameName NULL終端されたゲーム名。
 * @param[in] secretKey NULL終端されたシークレットキー。
 * @param[in] prefix チャンネル名に付加するプリフィックス。製品用、デバッグ用、テスト用で入るチャンネルを分けることができます。\n
 *                   製品用には::DWC_LOBBY_CHANNEL_PREFIX_RELEASEを、デバッグ用には::DWC_LOBBY_CHANNEL_PREFIX_DEBUGを指定してください。
 * @param[in] globalCallbacks チャンネルに関わらないコールバック関数をセットします。
 * @param[in] pid GameSpyProfileID。10万以上の正の数を指定してください。
 * 
 * @retval ::DWCi_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_PARAM 引数が不正。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_STATE すでに初期化されています。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_ALLOC メモリ確保に失敗。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_SESSION 通信エラー。
 */
DWCi_LOBBY_RESULT DWCi_LobbyInitializePidAsync(const char* gameName, const char* secretKey, DWC_LOBBY_CHANNEL_PREFIX prefix,
                                               const DWCi_LobbyGlobalCallbacks* globalCallbacks, s32 pid)
{
    // 状態チェック
    if(s_iLobby)
    {
        DWC_Printf(DWC_REPORTFLAG_ERROR, "DWCi_LobbyInitializePidAsync: Already initialized.\n");
        return DWCi_LOBBY_RESULT_ERROR_STATE;
    }
    
	DWCi_String chatNick(DWCi_Lobby::CreateNickName(pid));
	DWCi_String chatUser("DWCUser");
	DWCi_String chatName("DWCName");
    CHAT chat = NULL;

    // 内部ロビーデータ初期化
    s_iLobby = new DWCi_Lobby(gameName, secretKey, prefix, pid, chatNick, chatUser, chatName, globalCallbacks);
    if(!s_iLobby)
    {
        return DWCi_LOBBY_RESULT_ERROR_ALLOC;
    }
    
    
    DWCi_LOBBY_RESULT result = DWCi_LOBBY_RESULT_SUCCESS;

    // newハンドラ初期化
    if(!DWCi_LobbyInitializeNewHandler(s_iLobby))
    {
        result = DWCi_LOBBY_RESULT_ERROR_ALLOC;
        goto ERROR_ALLOC;
    }
    
	chatGlobalCallbacks gsGlobalCallbacks;
	DWCi_Np_CpuClear8(&gsGlobalCallbacks, sizeof(chatGlobalCallbacks));
	gsGlobalCallbacks.raw = Raw;
	gsGlobalCallbacks.disconnected = Disconnected;
	gsGlobalCallbacks.privateMessage = PrivateMessage;
	gsGlobalCallbacks.invited = Invited;
	gsGlobalCallbacks.param = globalCallbacks->param;

    DWC_Printf(DWC_REPORTFLAG_INFO, "chatConnectSecure start\n");
	/*
    // 古いバージョンは引数の仕様が違う
    chat = chatConnectSecure(serverAddress[0]?serverAddress:NULL, port, chatNick, chatName,
                                  gamename, secretKey, &gsGlobalCallbacks, NickErrorCallback, FillInUserCallback, ConnectCallback, NULL, CHATFalse);
    */
	chat = chatConnectSecure((s_iLobby->GetGameName() + "." + DWCi_LOBBY_SERVER_DOMAIN).c_str(), 6667, s_iLobby->GetChatNick().c_str(), s_iLobby->GetChatName().c_str(),
                                     s_iLobby->GetGameName().c_str(), s_iLobby->GetSecretKey().c_str(), &gsGlobalCallbacks,
                                     NickErrorCallback, FillInUserCallback, ConnectCallback, globalCallbacks->param, CHATFalse);
	if(!chat)
	{
		DWC_Printf(DWC_REPORTFLAG_ERROR, "chatConnectSecure failed\n");
		result = DWCi_LOBBY_RESULT_ERROR_SESSION;
        goto ERROR_ALLOC;
	}
    s_iLobby->SetChat(chat);
    s_iLobby->SetState(DWCi_LOBBY_STATE_CONNECTING);
    // MatchComment: add this line
    s_iLobby->GetTimerManager().AddTimer(DWCi_LOBBY_TIMER_ID_0, DWCi_LOBBY_TIMER_INTERVAL_FOR_ID_0,
                                            ov66_2244758, s_iLobby, FALSE);

    return DWCi_LOBBY_RESULT_SUCCESS;
    
ERROR_ALLOC:
    DWCi_LobbyDestroyNewHandler();
    DWC_SAFE_DELETE(s_iLobby);
    return result;
}

/** 
 * @brief ロビーライブラリを終了します。
 * 
 * ロビーライブラリを終了します。\n
 * 任意のタイミングで呼ぶことができます。すでに終了している場合は何もしません。\n
 * ::DWCi_LobbyProcess関数が::DWCi_LOBBY_STATE_NOTINITIALIZEDを返すとシャットダウン完了です。
 * 
 * @retval ::DWCi_LOBBY_RESULT_SUCCESS 成功。
 */
DWCi_LOBBY_RESULT DWCi_LobbyShutdownAsync()
{
    RETURN_IF_NOTINITIALIZED(DWCi_LOBBY_RESULT_SUCCESS);
    
    // すでに能動的に終了中。Diconnectコールバック待ち
    if(s_iLobby->GetState() == DWCi_LOBBY_STATE_CLOSING)
    {
        return DWCi_LOBBY_RESULT_SUCCESS;
    }
    
    // newハンドラ終了
    DWCi_LobbyDestroyNewHandler();
    
    if(s_iLobby->GetServerDisconnectedMe())
    {
        // サーバ側から切断されてDisconnectedコールバックが呼ばれたときの場合
        // GSチャットライブラリを終了する。このときはDisconnectedコールバックは呼ばれない。
        chatDisconnect(s_iLobby->GetChat());
        DWC_SAFE_DELETE(s_iLobby);
    }
    else
    {
        // 能動的に終了する場合
        // MatchComment: SetState -> SetStateNoErrorCheck
        s_iLobby->SetStateNoErrorCheck(DWCi_LOBBY_STATE_CLOSING);
        chatDisconnect(s_iLobby->GetChat());
        // Disconnectedコールバックでs_iLobbyを削除
    }
    return DWCi_LOBBY_RESULT_SUCCESS;
}

/** 
 * @brief チャンネルに参加します。
 * 
 * チャンネルに参加します。参加が完了または失敗すると::DWCi_LobbyJoinChannelCallbackコールバックが呼ばれます。
 * 
 * @param[in] channelName 参加するチャンネルの名前。終端込みで::DWC_LOBBY_MAX_CHANNEL_NAME_SIZE以下の長さである必要があります。\n
 *                        実際に参加するチャンネル名は「\#GSP!(gameName)!(::DWC_LOBBY_CHANNEL_PREFIX)!(channelName)」となります。\n
 *                        例：ゲーム名が"dwctest"、::DWC_LOBBY_CHANNEL_PREFIXが::DWC_LOBBY_CHANNEL_PREFIX_DEBUG、チャンネル名が"TestChannel"の場合、
 *                        「\#GSP!dwctest!D!TestChannel」となります。
 * @param[in] password チャンネルのパスワード。::DWCi_LobbySetChannelPassword関数でセットされたパスワードを指定してください。
 *                     パスワードが設定されていない場合は無視されます。
 * @param[in] channelCallbacks チャンネルに関するコールバック関数をセットします。
 * @param[in] callback チャンネルに参加または失敗したときに呼ばれるコールバック。
 * @param[in] param callbackのparamパラメータにそのまま渡されるユーザ定義情報。
 * 
 * @retval ::DWCi_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_STATE ステートがCONNECTEDではありません。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
DWCi_LOBBY_RESULT DWCi_LobbyJoinChannelAsync(const char* channelName, const char* password, const DWCi_LobbyChannelCallbacks* channelCallbacks,
                                             DWCi_LobbyJoinChannelCallback callback, void* param)
{
    return DWCi_LobbyJoinChannelLimitAsync(channelName, password, 0, channelCallbacks, callback, param);
}

/** 
 * @brief 制限人数を指定してチャンネルに参加します。
 * 
 * チャンネルに参加します。参加が完了または失敗すると::DWCi_LobbyJoinChannelCallbackコールバックが呼ばれます。
 * 
 * @param[in] channelName 参加するチャンネルの名前。終端込みで::DWC_LOBBY_MAX_CHANNEL_NAME_SIZE以下の長さである必要があります。\n
 *                        実際に参加するチャンネル名は「\#GSP!(gameName)!(::DWC_LOBBY_CHANNEL_PREFIX)!(channelName)」となります。\n
 *                        例：ゲーム名が"dwctest"、::DWC_LOBBY_CHANNEL_PREFIXが::DWC_LOBBY_CHANNEL_PREFIX_DEBUG、チャンネル名が"TestChannel"の場合、
 *                        「\#GSP!dwctest!D!TestChannel」となります。
 * @param[in] password チャンネルのパスワード。::DWCi_LobbySetChannelPassword関数でセットされたパスワードを指定してください。
 *                     パスワードが設定されていない場合は無視されます。
 * @param[in] limit チャンネルに参加できる人数を指定します。既存のチャンネルに参加する場合も指定した人数に制限が変更される点にご注意下さい。
 *                  0を指定すると制限しません。(::DWCi_LobbyJoinChannelAsync関数と同じ動作になります。)
 * @param[in] channelCallbacks チャンネルに関するコールバック関数をセットします。
 * @param[in] callback チャンネルに参加または失敗したときに呼ばれるコールバック。
 * @param[in] param callbackのparamパラメータにそのまま渡されるユーザ定義情報。
 * 
 * @retval ::DWCi_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_STATE ステートがCONNECTEDではありません。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
DWCi_LOBBY_RESULT DWCi_LobbyJoinChannelLimitAsync(const char* channelName, const char* password, s32 limit, const DWCi_LobbyChannelCallbacks* channelCallbacks,
                                             DWCi_LobbyJoinChannelCallback callback, void* param)
{
    RETURN_IF_NOTINITIALIZED(DWCi_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(DWCi_LOBBY_RESULT_ERROR_CONDITION);
    RETURN_IF_NOTCONNECTED(DWCi_LOBBY_RESULT_ERROR_STATE);
    
    if(strlen(channelName) + 1 > DWC_LOBBY_MAX_CHANNEL_NAME_SIZE)
    {
        DWC_Printf(DWC_REPORTFLAG_INFO, "DWCi_LobbyJoinChannelAsync: channelName is too long.\n");
    }
    
    // 入室コールバック情報を保存
    u32 joinChannelOperationId = s_iLobby->GetCallbackManager().AddCallback(callback, param);
    
    // チャンネル情報を追加
    s_iLobby->AddChannel(s_iLobby->ModifyChannelName(channelName).c_str(), *channelCallbacks, limit);
    
	chatChannelCallbacks gsChannelCallbacks;
	DWCi_Np_CpuClear8(&gsChannelCallbacks, sizeof(chatChannelCallbacks));
	gsChannelCallbacks.channelMessage = ChannelMessage;
	gsChannelCallbacks.channelModeChanged = ChannelModeChanged;
	gsChannelCallbacks.kicked = Kicked;
	gsChannelCallbacks.topicChanged = TopicChanged;
	gsChannelCallbacks.userParted = UserParted;
	gsChannelCallbacks.userJoined = UserJoined;
	gsChannelCallbacks.userListUpdated = UserListUpdated;
	gsChannelCallbacks.userModeChanged = UserModeChanged;
	gsChannelCallbacks.userChangedNick = UserChangedNick;
    gsChannelCallbacks.broadcastKeyChanged = BroadcastKeyChanged;
	gsChannelCallbacks.param = (void*)channelCallbacks->param;
    
    DWC_Printf(DWC_REPORTFLAG_INFO, "DWCi_LobbyJoinChannelAsync start to: %s\n", s_iLobby->ModifyChannelName(channelName).c_str());
	chatEnterChannel(s_iLobby->GetChat(), s_iLobby->ModifyChannelName(channelName).c_str(), password,
                     &gsChannelCallbacks, EnterChannelCallback, (void*)joinChannelOperationId, CHATFalse);
    
    return DWCi_LOBBY_RESULT_SUCCESS;
}

/** 
 * @brief チャンネルから出ます。
 * 
 * チャンネルから出ます。
 * 
 * @param[in] channelName 退室したいチャンネル名。
 * 
 * @retval ::DWCi_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_STATE ステートがCONNECTEDではありません。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
DWCi_LOBBY_RESULT DWCi_LobbyLeaveChannel(const char* channelName)
{
    RETURN_IF_NOTINITIALIZED(DWCi_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(DWCi_LOBBY_RESULT_ERROR_CONDITION);
    RETURN_IF_NOTCONNECTED(DWCi_LOBBY_RESULT_ERROR_STATE);
    
    DWC_Printf(DWC_REPORTFLAG_INFO, "DWCi_LobbyLeaveChannel start\n");
	chatLeaveChannel(s_iLobby->GetChat(), s_iLobby->ModifyChannelName(channelName).c_str(), NULL);
    
    // チャンネル情報を削除
    s_iLobby->RemoveChannel(s_iLobby->ModifyChannelName(channelName));
    
    return DWCi_LOBBY_RESULT_SUCCESS;
}

/**
 * @brief Ascii文字列のメッセージを指定したチャンネルに参加している全てのプレイヤーに送信します。
 * 
 * Ascii文字列のメッセージを指定したチャンネルに参加している全てのプレイヤーに送信します。\n
 * メッセージは自分にもループバックされます。
 * 
 * @param[in] channelName 送信先のチャンネル名。
 * @param[in] message 送信するNULL終端されたAscii文字列。終端込みで::DWC_LOBBY_MAX_STRING_SIZE以下の長さである必要があります。
 * 
 * @retval ::DWCi_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_PARAM messageが長すぎます。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_STATE ステートがCONNECTEDではありません。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
DWCi_LOBBY_RESULT DWCi_LobbySendChannelMessage(const char* channelName, const char* message)
{
    RETURN_IF_NOTINITIALIZED(DWCi_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(DWCi_LOBBY_RESULT_ERROR_CONDITION);
    RETURN_IF_NOTCONNECTED(DWCi_LOBBY_RESULT_ERROR_STATE);
    
    if(strlen(message) + 1 > DWC_LOBBY_MAX_STRING_SIZE)
    {
        DWC_Printf(DWC_REPORTFLAG_INFO, "DWCi_LobbySendChannelMessage: message is too long.\n");
        return DWCi_LOBBY_RESULT_ERROR_PARAM;
    }
    
	chatSendChannelMessage(s_iLobby->GetChat(), s_iLobby->ModifyChannelName(channelName).c_str(), message, DWCi_LOBBY_MESSAGE_TYPE_NORMAL_COMMAND);
    return DWCi_LOBBY_RESULT_SUCCESS;
}

/**
 * @brief Ascii文字列のメッセージを一人のプレイヤーに送信します。
 * 
 * Ascii文字列のメッセージを一人のプレイヤーに送信します。自分が参加しているチャンネルにいる人以外には送信できません。
 * 
 * @param[in] userId 送信先のユーザID。
 * @param[in] message 送信するNULL終端されたAscii文字列。NULL終端込みで::DWC_LOBBY_MAX_STRING_SIZE以下の長さである必要があります。
 * 
 * @retval ::DWCi_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_PARAM 指定したユーザがいません、もしくはmessageが長すぎます。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_STATE ステートがCONNECTEDではありません。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
DWCi_LOBBY_RESULT DWCi_LobbySendUserMessage(s32 userId, const char* message)
{
    RETURN_IF_NOTINITIALIZED(DWCi_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(DWCi_LOBBY_RESULT_ERROR_CONDITION);
    RETURN_IF_NOTCONNECTED(DWCi_LOBBY_RESULT_ERROR_STATE);
    
    if(strlen(message) + 1 > DWC_LOBBY_MAX_STRING_SIZE)
    {
        return DWCi_LOBBY_RESULT_ERROR_PARAM;
    }
    
    DWCi_String userName = s_iLobby->FindUser(userId);
    if(userName == "")
    {
        DWC_Printf(DWC_REPORTFLAG_INFO, "DWCi_LobbySendUserMessage: There is no such userId: %d\n", userId);
        return DWCi_LOBBY_RESULT_ERROR_PARAM;
    }
    
	chatSendUserMessage(s_iLobby->GetChat(), userName.c_str(), message, DWCi_LOBBY_MESSAGE_TYPE_NORMAL_COMMAND);
    return DWCi_LOBBY_RESULT_SUCCESS;
}

/**
 * @brief チャンネルデータをセットします。
 * 
 * チャンネルデータをセットします。\n
 * チャンネルデータは参加しているチャンネルのプレイヤー一人一人とチャンネル自体にセットすることができます。\n
 * それぞれに最大20個のチャンネルデータをセットすることができます。\n
 * keyに"b_"から始まる文字列を指定すると、サーバにセットすると同時に他のプレイヤーにそれを通知します。(ブロードキャスト)\n
 * このブロードキャストは自分にもループバックされます。\n
 * 通知は::DWCi_LobbyGetChannelDataCallbackコールバックにより行われます。このとき第二引数(broadcast)がTRUEとなります。
 * 
 * @param[in] channelName チャンネルデータをセットするチャンネル名。
 * @param[in] userId チャンネルデータをセットするユーザID。::DWC_LOBBY_INVALID_USER_IDを指定するとチャンネル自体にセットします。
 * @param[in] key チャンネルデータにアクセスするためのNULL終端された文字列。NULL終端込みで::DWC_LOBBY_MAX_CHANNEL_KEY_SIZEまでの長さである必要があります。
 * @param[in] value keyに結びつけるNULL終端された文字列。NULL終端込みで::DWC_LOBBY_MAX_STRING_SIZEまでの長さである必要があります。
 * 
 * @retval ::DWCi_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_PARAM 指定したユーザがいません、もしくはkeyまたはvalueが長すぎます。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_STATE ステートがCONNECTEDではありません。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
DWCi_LOBBY_RESULT DWCi_LobbySetChannelData(const char* channelName, s32 userId, const char* key, const char* value)
{
    RETURN_IF_NOTINITIALIZED(DWCi_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(DWCi_LOBBY_RESULT_ERROR_CONDITION);
    RETURN_IF_NOTCONNECTED(DWCi_LOBBY_RESULT_ERROR_STATE);
    
    if(strlen(value) + 1 > DWC_LOBBY_MAX_STRING_SIZE || strlen(key) + 1 > DWC_LOBBY_MAX_CHANNEL_KEY_SIZE)
    {
        DWC_Printf(DWC_REPORTFLAG_INFO, "DWCi_LobbySetChannelData: key or value is too long.\n");
        return DWCi_LOBBY_RESULT_ERROR_PARAM;
    }
    
    DWCi_String userName = "";
    if(userId == DWC_LOBBY_INVALID_USER_ID)
    {
        // チャンネル自体のデータをセット
        userName = "";
    }
    else
    {
        // 特定のユーザのデータをセット
        userName = s_iLobby->FindUser(userId);
        if(userName == "")
        {
            DWC_Printf(DWC_REPORTFLAG_INFO, "DWCi_LobbySetChannelData: There is no such userId: %d\n", userId);
            return DWCi_LOBBY_RESULT_ERROR_PARAM;
        }
    }
    DWC_Printf(DWC_REPORTFLAG_INFO, "DWCi_LobbySetChannelData start. key: %s, value: %s\n", key, value);
    chatSetChannelKeys(s_iLobby->GetChat(), s_iLobby->ModifyChannelName(channelName).c_str(), userName.c_str(), 1, &key, &value);
    return DWCi_LOBBY_RESULT_SUCCESS;
}

/**
 * @brief チャンネルデータを取得します。
 * 
 * チャンネルデータを取得します。\n
 * 完了すると::DWCi_LobbyGetChannelDataCallbackコールバックが呼ばれます。
 * 
 * @param[in] channelName チャンネルデータを取得するチャンネル名。
 * @param[in] userId チャンネルデータを取得するユーザID。::DWC_LOBBY_INVALID_USER_IDを指定するとチャンネル自体から取得します。
 * @param[in] key チャンネルデータにアクセスするためのキー。NULL終端込みで::DWC_LOBBY_MAX_CHANNEL_KEY_SIZEまでの長さである必要があります。
 * @param[in] callback 取得が完了、失敗したときに呼び出されるコールバック。
 * @param[in] param callbackのparamパラメータにそのまま渡されるユーザ定義情報。
 * 
 * @retval ::DWCi_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_PARAM 指定したユーザがいません、もしくはkeyが長すぎます。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_STATE ステートがCONNECTEDではありません。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
DWCi_LOBBY_RESULT DWCi_LobbyGetChannelDataAsync(const char* channelName, s32 userId, const char* key, DWCi_LobbyGetChannelDataCallback callback, void* param)
{
    RETURN_IF_NOTINITIALIZED(DWCi_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(DWCi_LOBBY_RESULT_ERROR_CONDITION);
    RETURN_IF_NOTCONNECTED(DWCi_LOBBY_RESULT_ERROR_STATE);
    
    if(strlen(key) + 1 > DWC_LOBBY_MAX_CHANNEL_KEY_SIZE)
    {
        DWC_Printf(DWC_REPORTFLAG_INFO, "DWCi_LobbyGetChannelDataAsync: key is too long.\n");
        return DWCi_LOBBY_RESULT_ERROR_PARAM;
    }
    
    DWCi_String userName = "";
    if(userId == DWC_LOBBY_INVALID_USER_ID)
    {
        // チャンネル自体のデータを取得
        userName = "";
    }
    else
    {
        // 特定のユーザのデータを取得
        userName = s_iLobby->FindUser(userId);
        if(userName == "")
        {
            DWC_Printf(DWC_REPORTFLAG_ERROR, "DWCi_LobbyGetChannelDataAsync: There is no such userId: %d\n", userId);
            return DWCi_LOBBY_RESULT_ERROR_PARAM;
        }
    }
    
    // コールバック情報を保存
    u32 operationId = s_iLobby->GetCallbackManager().AddCallback(callback, param);
    
    DWC_Printf(DWC_REPORTFLAG_INFO, "DWCi_LobbyGetChannelDataAsync start: channel: %s, key: %s\n", channelName, key);
    chatGetChannelKeys(s_iLobby->GetChat(), s_iLobby->ModifyChannelName(channelName).c_str(), userName.c_str(),
                       1, &key, GetChannelKeysCallback, (void*)operationId, CHATFalse);
    return DWCi_LOBBY_RESULT_SUCCESS;
}

/**
 * @brief サーバ上のチャンネルを検索します。
 * 
 * サーバ上のチャンネルを検索します。\n
 * 完了すると::DWCi_LobbyEnumChannelsCallbackコールバックが呼ばれます。
 * 
 * @param[in] filter 検索文字列。ワイルドカード"*"が使用できます。例えばこのアプリケーションで使用している全てのチャンネルを列挙する場合は"*"を指定します。
 *                   testから始まるチャンネル名を検索する場合は"test*"とします。
 * @param[in] callback 検索が完了、失敗したときに呼び出されるコールバック。
 * @param[in] param callbackのparamパラメータにそのまま渡されるユーザ定義情報。
 * 
 * @retval ::DWCi_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_STATE ステートがCONNECTEDではありません。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
DWCi_LOBBY_RESULT DWCi_LobbyEnumChannelsAsync(const char* filter, DWCi_LobbyEnumChannelsCallback callback, void* param)
{
    RETURN_IF_NOTINITIALIZED(DWCi_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(DWCi_LOBBY_RESULT_ERROR_CONDITION);
    RETURN_IF_NOTCONNECTED(DWCi_LOBBY_RESULT_ERROR_STATE);
    
    // コールバック情報を保存
    u32 operationId = s_iLobby->GetCallbackManager().AddCallback(callback, param);
    
    DWC_Printf(DWC_REPORTFLAG_INFO, "DWCi_LobbyEnumChannelsAsync start. searching:%s\n", s_iLobby->ModifyChannelName(filter).c_str());
    chatEnumChannels(s_iLobby->GetChat(), s_iLobby->ModifyChannelName(filter).c_str(), EnumChannelsEachCallback,    // バグ?によりEachも設定しなければならない。
                     EnumChannelsAllCallback, reinterpret_cast<void*>(operationId), CHATFalse);
    return DWCi_LOBBY_RESULT_SUCCESS;
}

/**
 * @brief チャンネルに参加している人を列挙します。
 * 
 * @deprecated ::DWCi_LobbyEnumUsers関数を使用してください。
 * 
 * チャンネルに参加している人を列挙します。\n
 * 自分が参加しているチャンネルのみ列挙できます。\n
 * 完了すると::DWCi_LobbyEnumUsersCallbackコールバックが呼ばれます。
 * 
 * @param[in] channelName 列挙するチャンネル名。
 * @param[in] callback 列挙が完了、失敗したときに呼び出されるコールバック。
 * @param[in] param callbackのparamパラメータにそのまま渡されるユーザ定義情報。
 * 
 * @retval ::DWCi_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_CHANNEL 指定したチャンネルに参加していません。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_STATE ステートがCONNECTEDではありません。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
DWCi_LOBBY_RESULT DWCi_LobbyEnumUsersAsync(const char* channelName, DWCi_LobbyEnumUsersCallback callback, void* param)
{
    RETURN_IF_NOTINITIALIZED(DWCi_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(DWCi_LOBBY_RESULT_ERROR_CONDITION);
    RETURN_IF_NOTCONNECTED(DWCi_LOBBY_RESULT_ERROR_STATE);
    
    // 自分が参加していないチャンネルは列挙不能
    //if(!DWCi_LobbyInChannel(channelName))   // なぜかこちらだとGCC3.2.2で関数呼び出しがすっとばされる
    if(!chatInChannel(s_iLobby->GetChat(), s_iLobby->ModifyChannelName(channelName).c_str()))
    {
        DWC_Printf(DWC_REPORTFLAG_INFO, "DWCi_LobbyEnumUsersAsync called2 %s\n", channelName);
        return DWCi_LOBBY_RESULT_ERROR_CHANNEL;
    }
    
    // コールバック情報を保存
    u32 operationId = s_iLobby->GetCallbackManager().AddCallback(callback, param);
    
    DWC_Printf(DWC_REPORTFLAG_INFO, "DWCi_LobbyEnumUsersAsync start\n");
    chatEnumUsers(s_iLobby->GetChat(), s_iLobby->ModifyChannelName(channelName).c_str(),
                  EnumUsersCallback, reinterpret_cast<void*>(operationId), CHATFalse);
    return DWCi_LOBBY_RESULT_SUCCESS;
}

/**
 * @brief チャンネルに参加している人を列挙します。
 * 
 * チャンネルに参加している人を列挙します。\n
 * 自分が参加しているチャンネルのみ列挙できます。\n
 * userIdsにNULLもしくはuserIdNumに実際に必要な個数より少ない数を渡すと::DWCi_LOBBY_RESULT_ERROR_PARAMが返り、
 * userIdNumに実際に必要な個数(つまりチャンネルに参加しているユーザの数)が格納されます。
 * 
 * @param[in] channelName 列挙するチャンネル名。
 * @param[in,out] userIds ユーザIDの配列を格納するバッファ。
 * @param[in,out] userIdNum userIdsに用意した配列のサイズ。byte単位ではありません。
 * 
 * @retval ::DWCi_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_CHANNEL 指定したチャンネルに参加していません。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_STATE ステートがCONNECTEDではありません。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_PARAM userIdNumが必要な個数を下回っているか、userIdsがNULLです。userIdNumに必要な個数を格納しました。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
DWCi_LOBBY_RESULT DWCi_LobbyEnumUsers(const char* channelName, s32* userIds, u32* userIdNum)
{
    RETURN_IF_NOTINITIALIZED(DWCi_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(DWCi_LOBBY_RESULT_ERROR_CONDITION);
    RETURN_IF_NOTCONNECTED(DWCi_LOBBY_RESULT_ERROR_STATE);
    
    u32 inputSize = *userIdNum;
    
    std::vector<s32, DWCi_Allocator<s32> > resultIds = s_iLobby->EnumUsers(s_iLobby->ModifyChannelName(channelName));
    if(resultIds.empty())
    {
        return DWCi_LOBBY_RESULT_ERROR_CHANNEL;
    }
    
    *userIdNum = resultIds.size();
    if(*userIdNum > inputSize || userIds == NULL)
    {
        return DWCi_LOBBY_RESULT_ERROR_PARAM;
    }
    
    std::copy(resultIds.begin(), resultIds.end(), userIds);
    return DWCi_LOBBY_RESULT_SUCCESS;
}

/**
 * @brief チャンネルモードをセットします。
 * 
 * チャンネルモードをセットします。
 * 
 * @param[in] channelName セットするチャンネル名。
 * @param[in] mode セットするチャンネルモード。
 * 
 * @retval ::DWCi_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_STATE ステートがCONNECTEDではありません。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
DWCi_LOBBY_RESULT DWCi_LobbySetChannelMode(const char* channelName, const DWCi_LobbyChannelMode* mode)
{
    RETURN_IF_NOTINITIALIZED(DWCi_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(DWCi_LOBBY_RESULT_ERROR_CONDITION);
    RETURN_IF_NOTCONNECTED(DWCi_LOBBY_RESULT_ERROR_STATE);
    
    DWC_Printf(DWC_REPORTFLAG_INFO, "DWCi_LobbySetChannelMode start\n");
    chatSetChannelMode(s_iLobby->GetChat(), s_iLobby->ModifyChannelName(channelName).c_str(), (CHATChannelMode*)mode);
    return DWCi_LOBBY_RESULT_SUCCESS;
}

/**
 * @brief チャンネルモードを取得します。
 * 
 * チャンネルモードを取得します。
 * 
 * @param[in] channelName 取得するチャンネル名。
 * @param[in] callback 取得が完了、失敗したときに呼び出されるコールバック。
 * @param[in] param callbackのparamパラメータにそのまま渡されるユーザ定義情報。
 * 
 * @retval ::DWCi_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_STATE ステートがCONNECTEDではありません。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
DWCi_LOBBY_RESULT DWCi_LobbyGetChannelModeAsync(const char* channelName, DWCi_LobbyGetChannelModeCallback callback, void* param)
{
    RETURN_IF_NOTINITIALIZED(DWCi_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(DWCi_LOBBY_RESULT_ERROR_CONDITION);
    RETURN_IF_NOTCONNECTED(DWCi_LOBBY_RESULT_ERROR_STATE);
    
    // コールバック情報を保存
    u32 operationId = s_iLobby->GetCallbackManager().AddCallback(callback, param);
    
    DWC_Printf(DWC_REPORTFLAG_INFO, "DWCi_LobbyGetChannelModeAsync start\n");
	chatGetChannelMode(s_iLobby->GetChat(), s_iLobby->ModifyChannelName(channelName).c_str(),
                       GetChannelModeCallback, reinterpret_cast<void*>(operationId), CHATFalse);
    return DWCi_LOBBY_RESULT_SUCCESS;
}

/**
 * @brief チャンネルトピックをセットします。
 * 
 * チャンネルトピックをセットします。このトピックは::DWCi_LobbyEnumChannelsAsync関数で参照することができます。\n
 * 指定したチャンネルに入室していない場合はセットできません。
 * 
 * @param[in] channelName トピックをセットするチャンネル名。
 * @param[in] topic 設定するトピック。NULL終端を含み::DWC_LOBBY_MAX_CHANNEL_TOPIC_SIZE以下の長さまで指定できます。
 * 
 * @retval ::DWCi_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_STATE ステートがCONNECTEDではありません。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_PARAM 指定したtopicが長すぎます。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_CHANNEL 指定したチャンネルに入っていません。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
DWCi_LOBBY_RESULT DWCi_LobbySetChannelTopic(const char* channelName, const char* topic)
{
    RETURN_IF_NOTINITIALIZED(DWCi_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(DWCi_LOBBY_RESULT_ERROR_CONDITION);
    RETURN_IF_NOTCONNECTED(DWCi_LOBBY_RESULT_ERROR_STATE);
    
    if(strlen(topic) + 1 > DWC_LOBBY_MAX_CHANNEL_TOPIC_SIZE)
    {
        DWC_Printf(DWC_REPORTFLAG_INFO, "DWCi_LobbySetChannelTopic: topic length is too long.\n");
        return DWCi_LOBBY_RESULT_ERROR_PARAM;
    }
    
    if(!DWCi_LobbyInChannel(channelName))
    {
        DWC_Printf(DWC_REPORTFLAG_INFO, "DWCi_LobbySetChannelTopic: You are not in the channel.\n");
        return DWCi_LOBBY_RESULT_ERROR_CHANNEL;
    }
    
    DWC_Printf(DWC_REPORTFLAG_INFO, "DWCi_LobbySetChannelTopic start\n");
    chatSetChannelTopic(s_iLobby->GetChat(), s_iLobby->ModifyChannelName(channelName).c_str(), topic);
    return DWCi_LOBBY_RESULT_SUCCESS;
}

/**
 * @brief チャンネルパスワードをセットまたは解除します。
 * 
 * チャンネルにパスワードをセットまたは解除します。パスワードがセットされた場合、::DWCi_LobbyJoinChannelAsync関数でパスワードを指定しないと入室できません。。\n
 * 指定したチャンネルに入室していない場合はセットできません。
 * 
 * @param[in] channelName パスワードをセットするチャンネル名。
 * @param[in] enable パスワードをセットする場合はTRUE、解除する場合はFALSEを指定してください。
 * @param[in] password 設定または解除するパスワード。NULL終端を含み::DWC_LOBBY_MAX_CHANNEL_PASSWORD_SIZE以下の長さまで指定できます。
 *                     解除する場合は現在のパスワードを指定してください。
 * 
 * @retval ::DWCi_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_STATE ステートがCONNECTEDではありません。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_PARAM 指定したpasswordが長すぎます。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_CHANNEL 指定したチャンネルに入っていません。
 * @retval ::DWCi_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
DWCi_LOBBY_RESULT DWCi_LobbySetChannelPassword(const char* channelName, BOOL enable, const char* password)
{
    RETURN_IF_NOTINITIALIZED(DWCi_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(DWCi_LOBBY_RESULT_ERROR_CONDITION);
    RETURN_IF_NOTCONNECTED(DWCi_LOBBY_RESULT_ERROR_STATE);
    
    if(strlen(password) + 1 > DWC_LOBBY_MAX_CHANNEL_PASSWORD_SIZE)
    {
        DWC_Printf(DWC_REPORTFLAG_INFO, "DWCi_LobbySetChannelTopic: topic length is too long.\n");
        return DWCi_LOBBY_RESULT_ERROR_PARAM;
    }
    
    if(!DWCi_LobbyInChannel(channelName))
    {
        DWC_Printf(DWC_REPORTFLAG_INFO, "DWCi_LobbySetChannelTopic: You are not in the channel.\n");
        return DWCi_LOBBY_RESULT_ERROR_CHANNEL;
    }
    
    DWC_Printf(DWC_REPORTFLAG_INFO, "DWCi_LobbySetChannelPassword start\n");
    chatSetChannelPassword(s_iLobby->GetChat(), s_iLobby->ModifyChannelName(channelName).c_str(), enable ? CHATTrue : CHATFalse, password);
    return DWCi_LOBBY_RESULT_SUCCESS;
}
