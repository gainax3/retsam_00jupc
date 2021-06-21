/*---------------------------------------------------------------------------*
  Project:  NitroDWC Libraries
  File:     ./build/libraries/lobby/include/dwci_lobby.h

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
 * @brief Wi-Fi ロビーライブラリ ヘッダ
 * 
 * Wi-Fi ロビーライブラリ
 */

#ifndef DWCi_LOBBY_H_
#define DWCi_LOBBY_H_

#include <lobby/dwc_lobby.h>


#ifdef __cplusplus
extern "C"
{
#endif

#ifdef WIN32
#pragma pack(push, 4)
#endif

// 無効なチャンネルID
#define DWCi_LOBBY_INVALID_CHANNEL_ID       0xffffffff

// 最大ニックネームサイズ
#define DWCi_MAX_CHAT_NICK_SIZE             20

// 接続後に待つフレーム。
#define DWCi_LOBBY_CONNECTINGWAIT_TIME      60

// チャンネル名の区切り文字
#define DWCi_LOBBY_CHANNEL_NAME_SEPARATOR   "!"

// ロビーライブラリで使用するエラーコードの基準番号
#define DWCi_LOBBY_ERROR_CODE_BASE          42000

// ロビーライブラリで使用するチャットサーバドメイン
#define DWCi_LOBBY_SERVER_DOMAIN            "peerchat.gs.nintendowifi.net"

/// 状態フラグ
typedef enum DWCi_LOBBY_STATE
{
    DWCi_LOBBY_STATE_NOTINITIALIZED,        ///< ライブラリが初期化されていない。
    DWCi_LOBBY_STATE_CONNECTING,            ///< サーバに接続中
    DWCi_LOBBY_STATE_CONNECTINGWAIT,        ///< サーバに接続後処理
    DWCi_LOBBY_STATE_CONNECTED,             ///< サーバに接続完了
    DWCi_LOBBY_STATE_CLOSING,               ///< ライブラリ終了処理中。
    DWCi_LOBBY_STATE_ERROR                  ///< ライブラリに致命的なエラーが発生中。::DWCi_LobbyGetLastError関数でエラーを取得後シャットダウンしてください。
}
DWCi_LOBBY_STATE;

/// 処理結果フラグ。
typedef enum DWCi_LOBBY_RESULT
{
    DWCi_LOBBY_RESULT_SUCCESS,              ///< 成功。
    DWCi_LOBBY_RESULT_ERROR_ALLOC,          ///< メモリ確保に失敗。
    DWCi_LOBBY_RESULT_ERROR_SESSION,        ///< 通信エラー。
    DWCi_LOBBY_RESULT_ERROR_PARAM,          ///< 指定した引数が不正。
    DWCi_LOBBY_RESULT_ERROR_STATE,          ///< 呼び出してはいけない状態で関数を呼び出した。
    DWCi_LOBBY_RESULT_ERROR_CHANNEL,        ///< 指定したチャンネルに入っていない。
    DWCi_LOBBY_RESULT_ERROR_NODATA,         ///< 指定した情報は存在しない。
    DWCi_LOBBY_RESULT_ERROR_CONDITION,      ///< 致命的エラーが発生中なのでこの関数を呼び出せない。
    DWCi_LOBBY_RESULT_MAX                   ///< ライブラリ内部で使用されます。
}
DWCi_LOBBY_RESULT;

/// エラーフラグ
typedef enum DWCi_LOBBY_ERROR
{
    DWCi_LOBBY_ERROR_NONE,                  ///< エラー無し。
    DWCi_LOBBY_ERROR_UNKNOWN,               ///< 不明なエラー。
    DWCi_LOBBY_ERROR_ALLOC,                 ///< メモリ確保に失敗。
    DWCi_LOBBY_ERROR_SESSION,               ///< 致命的な通信エラー。
    DWCi_LOBBY_ERROR_MAX                    ///< ライブラリ内部で使用されます。
}
DWCi_LOBBY_ERROR;

/// チャンネル入室結果
typedef enum
{
	DWCi_LOBBY_ENTER_RESULT_SUCCESS,            ///< 成功。
	DWCi_LOBBY_ENTER_RESULT_BAD_CHANNEL_NAME,   ///< チャンネル名が不正です。
	DWCi_LOBBY_ENTER_RESULT_FULL,               ///< 満員。
	DWCi_LOBBY_ENTER_RESULT_INVITE_ONLY,        ///< このチャンネルは招待でのみ入れます。
	DWCi_LOBBY_ENTER_RESULT_BANNED,             ///< このチャンネルからBanされています。
	DWCi_LOBBY_ENTER_RESULT_BAD_PASSWORD,       ///< このチャンネルにはパスワードがかけられています。もしくはパスワードが違います。
	DWCi_LOBBY_ENTER_RESULT_TOO_MANY_CHANNELS,  ///< これ以上の多くのチャンネルに入れません。
	DWCi_LOBBY_ENTER_RESULT_TIME_OUT,           ///< タイムアウト。
	DWCi_LOBBY_ENTER_RESULT_MAX                 ///< ライブラリ内部で使用されます。
}
DWCi_LOBBY_ENTER_RESULT;

/// チャンネルモード
typedef struct DWCi_LobbyChannelMode
{
    BOOL inviteOnly;                        ///< チャンネルへの参加を招待のみにする。FALSEを指定してください。
    BOOL privateMode;                       ///< チャンネル検索に対してチャンネル名を伏せる。secretModeと併用しないでください。
    BOOL secretMode;                        ///< チャンネル検索に対してチャンネルを完全に隠す。privateModeと併用しないでください。
    BOOL moderated;                         ///< チャンネルオペレータしか発言できなくする。
    BOOL noExternalMessages;                ///< チャンネルに入っていない人の発言を拒否する。
    BOOL onlyOpsChangeTopic;                ///< チャンネルオペレータしかトピックを変更できなくする。TRUEを指定してください。
    BOOL opsObeyChannelLimit;               ///< チャンネルオペレータにも人数制限を適用する。TRUEを指定してください。
    s32 limit;                              ///< チャンネルに入れる人数を制限する。0以下の場合は制限しません。
}
DWCi_LobbyChannelMode;


// コールバック型宣言

/**
 * @brief 自分がチャンネルに入ったときに呼び出されるコールバック関数型。
 * 
 * 自分がチャンネルに入ったときに呼び出されるコールバック関数型。
 * 
 * @param[in] success 入室に成功したか。
 * @param[in] result 入室結果フラグ。
 * @param[in] channelName 入室したチャンネルの名前。
 * @param[in] param ::DWCi_LobbyJoinChannel*Async関数のparamパラメータに指定した値。
 */
typedef void (*DWCi_LobbyJoinChannelCallback)(BOOL success, DWCi_LOBBY_ENTER_RESULT result, const char* channelName, void* param);

/**
 * @brief 他の人がチャンネルに入ったときに呼び出されるコールバック関数型。
 * 
 * 他の人がチャンネルに入ったときに呼び出されるコールバック関数型。
 * 
 * @param[in] channelName 入室したチャンネルの名前。
 * @param[in] userId 入室した人のユーザID。
 * @param[in] param ::DWCi_LobbyChannelCallbacks構造体のparamメンバに指定した値。
 */
typedef void (*DWCi_LobbyPlayerConnectedCallback)(const char* channelName, s32 userId, void* param);

/**
 * @brief 他の人がチャンネルから退室したときに呼び出されるコールバック関数型。
 * 
 * 他の人がチャンネルから退室したときに呼び出されるコールバック関数型。
 * 
 * @param[in] channelName 退室したチャンネルの名前。
 * @param[in] userId 退室した人のユーザID。
 * @param[in] param ::DWCi_LobbyChannelCallbacks構造体のparamメンバに指定した値。
 */
typedef void (*DWCi_LobbyPlayerDisconnectedCallback)(const char* channelName, s32 userId, void* param);

/**
 * @brief チャンネルへのメッセージを受信したときに呼び出されるコールバック関数型。
 * 
 * チャンネルへのメッセージを受信したときに呼び出されるコールバック関数型。
 * 
 * @param[in] channelName 受信したチャンネルの名前。
 * @param[in] userId 送信した人のユーザID。
 * @param[in] message 受信したメッセージ。
 * @param[in] param ::DWCi_LobbyChannelCallbacks構造体のparamメンバに指定した値。
 */
typedef void (*DWCi_LobbyChannelMassageReceivedCallback)(const char* channelName, s32 userId, const char* message, void* param);

/**
 * @brief 個人宛のメッセージを受信したときに呼び出されるコールバック関数型。
 * 
 * 個人宛のメッセージを受信したときに呼び出されるコールバック関数型。
 * 
 * @param[in] userId 送信した人のユーザID。
 * @param[in] message 受信したメッセージ。
 * @param[in] param ::DWCi_LobbyGlobalCallbacks構造体のparamメンバに指定した値。
 */
typedef void (*DWCi_LobbyUserMassageReceivedCallback)(s32 userId, const char* message, void* param);

/**
 * @brief チャンネルデータを受信したときに呼び出されるコールバック関数型。
 * 
 * チャンネルデータを受信したときに呼び出されるコールバック関数型。
 * ::DWCi_LobbyGetChannelDataAsync関数の結果が通知されます。
 * 
 * @param[in] success 成功したか。
 * @param[in] channelName 受信したチャンネルデータに関連づけられているチャンネル名。
 * @param[in] userId 受信したチャンネルデータに関連づけられているユーザID。
 * @param[in] key 受信したキー文字列。
 * @param[in] value keyに関連づけられたデータ。
 * @param[in] param ::DWCi_LobbyGetChannelDataAsync関数のparamパラメータに指定した値。
 */
typedef void (*DWCi_LobbyGetChannelDataCallback)(BOOL success, const char* channelName, s32 userId,
                                                 const char* key, const char* value, void* param);

/**
 * @brief ブロードキャストされたチャンネルデータを受信したときに呼び出されるコールバック関数型。
 * 
 * ブロードキャストされたチャンネルデータを受信したときに呼び出されるコールバック関数型。
 * 
 * @param[in] channelName 受信したチャンネルデータに関連づけられているチャンネル名。
 * @param[in] userId ブロードキャストを行ったユーザID。
 * @param[in] key 受信したキー文字列。
 * @param[in] value keyに関連づけられたデータ。
 * @param[in] param ::DWCi_LobbyChannelCallbacks構造体のparamメンバに指定した値。
 */
typedef void (*DWCi_LobbyBroadcastDataCallback)(const char* channelName, s32 userId,
                                                 const char* key, const char* value, void* param);

/**
 * @brief チャンネルの列挙が完了したときに呼び出されるコールバック関数型。
 * 
 * チャンネルの列挙が完了したときに呼び出されるコールバック関数型。
 * ::DWCi_LobbyEnumChannelsAsync関数の結果が通知されます。
 * 
 * @param[in] success 成功したか。
 * @param[in] numChannels 列挙したチャンネル数。
 * @param[in] channels numChannel分のチャンネル名の配列。
 * @param[in] topics channelsに対応するトピックの配列。
 * @param[in] numUsers channelsに対応するチャンネルに参加している人の人数。
 * @param[in] param ::DWCi_LobbyEnumChannelsAsync関数のparamパラメータに指定した値。
 */
typedef void (*DWCi_LobbyEnumChannelsCallback)(BOOL success, s32 numChannels, const char** channels, const char** topics, const s32* numUsers, void* param);

/**
 * @brief チャンネルに参加している人の列挙が完了したときに呼び出されるコールバック関数型。
 * 
 * チャンネルに参加している人の列挙が完了したときに呼び出されるコールバック関数型。
 * ::DWCi_LobbyEnumUsersAsync関数の結果が通知されます。
 * 
 * @param[in] success 成功したか。
 * @param[in] channelName 列挙した対象のチャンネル名。
 * @param[in] numUsers チャンネルに参加している人数。
 * @param[in] userIds チャンネルに参加している人のユーザIDの配列。
 * @param[in] param ::DWCi_LobbyEnumUsersAsync関数のparamパラメータに指定した値。
 */
typedef void (*DWCi_LobbyEnumUsersCallback)(BOOL success, const char* channelName, s32 numUsers, const s32* userIds, void* param);

/**
 * @brief チャンネルモードを取得したときに呼び出されるコールバック関数型。
 * 
 * チャンネルモードを取得したときに呼び出されるコールバック関数型。
 * ::DWCi_LobbyGetChannelModeAsync関数の結果が通知されます。\n
 * ::DWCi_LobbySetChannelMode関数を呼んだ直後に本関数によってチャンネルモードを取得した場合、
 * 実際にモードがセットされるまでに若干のタイムラグがあるため、変更前のチャンネルモードが取得される場合があります。
 * 
 * @param[in] success 成功したか。
 * @param[in] channelName 取得した対象のチャンネル名。
 * @param[in] mode チャンネルモード。
 * @param[in] param ::DWCi_LobbyGetChannelModeAsync関数のparamパラメータに指定した値。
 */
typedef void (*DWCi_LobbyGetChannelModeCallback)(BOOL success, const char* channelName, const DWCi_LobbyChannelMode* mode, void* param);

/**
 * @brief チャンネルトピックが変更されたときに呼び出されるコールバック関数型。
 * 
 * チャンネルトピックが変更されたときに呼び出されるコールバック関数型。
 * 
 * @param[in] channelName 取得した対象のチャンネル名。
 * @param[in] topic 変更後のトピック。
 * @param[in] param ::DWCi_LobbyChannelCallbacks構造体のparamメンバに指定した値。
 */
typedef void (*DWCi_LobbyTopicChangedCallback)(const char* channelName, const char* topic, void* param);

/**
 * @brief サーバとの通信を過剰に行ったときに呼び出されるコールバック関数型。
 * 
 * サーバとの通信を過剰に行ったときに呼び出されるコールバック関数型。
 * floodWeightは通信を発生させるロビーライブラリの関数の呼び出しによって上昇し、
 * 300を超えると::DWCi_LobbyGetChannelDataAsync関数が無視されるようになり、600を超えるとサーバから切断されます。
 * floodWeightは1秒に5減少します。
 * ただしDWCi_LobbySend*Message関数の呼び出しはfloodWeightを上昇させません。
 * このコールバックが発生したときはチャンネルデータセットや取得を控えるようにしてください。
 * 
 * @param[in] floodWeight 通信過多の割合を示す重み。
 * @param[in] param ::DWCi_LobbyGlobalCallbacks構造体のparamメンバに指定した値。
 */
typedef void (*DWCi_LobbyExcessFloodCallback)(u32 floodWeight, void* param);

/// グローバルコールバック登録用構造体
typedef struct DWCi_LobbyGlobalCallbacks
{
    /// 個人宛のメッセージを受信したときに呼び出されるコールバック関数。
    DWCi_LobbyUserMassageReceivedCallback lobbyUserMassageReceivedCallback;
    /// サーバとの通信を過剰に行ったときに呼び出されるコールバック関数。
    DWCi_LobbyExcessFloodCallback lobbyExcessFloodCallback;
    /// コールバック関数のparamパラメータに渡される値。
    void* param;
}
DWCi_LobbyGlobalCallbacks;

/// チャンネルコールバック登録用構造体
typedef struct DWCi_LobbyChannelCallbacks
{
    /// 他の人がチャンネルに入ったときに呼び出されるコールバック関数。
    DWCi_LobbyPlayerConnectedCallback lobbyPlayerConnectedCallback;
    /// 他の人がチャンネルから退室したときに呼び出されるコールバック関数。
    DWCi_LobbyPlayerDisconnectedCallback lobbyPlayerDisconnectedCallback;
    /// チャンネルへのメッセージを受信したときに呼び出されるコールバック関数。
    DWCi_LobbyChannelMassageReceivedCallback lobbyChannelMassageReceivedCallback;
    /// ブロードキャストされたチャンネルデータを受信したときに呼び出されるコールバック関数。
    DWCi_LobbyBroadcastDataCallback lobbyBroadcastDataCallback;
    /// チャンネルトピックが変更されたときに呼び出されるコールバック関数。
    DWCi_LobbyTopicChangedCallback lobbyTopicChangedCallback;
    /// コールバック関数のparamパラメータに渡される値。
    void* param;
}
DWCi_LobbyChannelCallbacks;

// ロビーライブラリを初期化。
DWCi_LOBBY_RESULT   DWCi_LobbyInitializeAsync(const char* gameName, const char* secretKey, DWC_LOBBY_CHANNEL_PREFIX prefix,
                                         const DWCi_LobbyGlobalCallbacks* globalCallbacks, const DWCUserData* userdata);

// ロビーライブラリを指定したPIDで初期化。
DWCi_LOBBY_RESULT   DWCi_LobbyInitializePidAsync(const char* gameName, const char* secretKey, DWC_LOBBY_CHANNEL_PREFIX prefix,
                                         const DWCi_LobbyGlobalCallbacks* globalCallbacks, s32 pid);
// ロビーライブラリを終了。
DWCi_LOBBY_RESULT   DWCi_LobbyShutdownAsync();

// プロセス関数。
DWCi_LOBBY_STATE    DWCi_LobbyProcess();

// エラー情報取得。
DWCi_LOBBY_ERROR    DWCi_LobbyGetLastError();

// 発生したエラー(::DWCi_LOBBY_ERROR)から表示すべきエラーコードを得る。
s32                 DWCi_LobbyToErrorCode(DWCi_LOBBY_ERROR err);

// 自分のユーザIDを取得。
s32                 DWCi_LobbyGetMyUserId();

// 指定したチャンネルに入っているか調べる。
BOOL                DWCi_LobbyInChannel(const char* channelName);

// チャンネルに入る。
DWCi_LOBBY_RESULT   DWCi_LobbyJoinChannelAsync(const char* channelName, const char* password, const DWCi_LobbyChannelCallbacks* channelCallbacks,
                                               DWCi_LobbyJoinChannelCallback callback, void* param);

// 最大人数を指定してチャンネルに入る。
DWCi_LOBBY_RESULT   DWCi_LobbyJoinChannelLimitAsync(const char* channelName, const char* password, s32 limit, const DWCi_LobbyChannelCallbacks* channelCallbacks,
                                                  DWCi_LobbyJoinChannelCallback callback, void* param);
    
// チャンネルから出る。
DWCi_LOBBY_RESULT   DWCi_LobbyLeaveChannel(const char* channelName);

// チャンネルに参加している人にメッセージを送信する。
DWCi_LOBBY_RESULT   DWCi_LobbySendChannelMessage(const char* channelName, const char* message);

// 指定したユーザへメッセージを送信する。
DWCi_LOBBY_RESULT   DWCi_LobbySendUserMessage(s32 userId, const char* message);

// チャンネルデータをセットする。
DWCi_LOBBY_RESULT   DWCi_LobbySetChannelData(const char* channelName, s32 userId, const char* key, const char* value);

// チャンネルデータを取得する。
DWCi_LOBBY_RESULT   DWCi_LobbyGetChannelDataAsync(const char* channelName, s32 userId, const char* key, DWCi_LobbyGetChannelDataCallback callback, void* param);

// チャンネルを列挙する。
DWCi_LOBBY_RESULT   DWCi_LobbyEnumChannelsAsync(const char* filter, DWCi_LobbyEnumChannelsCallback callback, void* param);

// チャンネルに参加している人を列挙する。
DWCi_LOBBY_RESULT   DWCi_LobbyEnumUsersAsync(const char* channelName, DWCi_LobbyEnumUsersCallback callback, void* param);

// チャンネルに参加している人を列挙する。
DWCi_LOBBY_RESULT   DWCi_LobbyEnumUsers(const char* channelName, s32* userIds, u32* userIdNum);

// チャンネルモードをセットする。
DWCi_LOBBY_RESULT   DWCi_LobbySetChannelMode(const char* channelName, const DWCi_LobbyChannelMode* mode);

// チャンネルモードを取得する。
DWCi_LOBBY_RESULT   DWCi_LobbyGetChannelModeAsync(const char* channelName, DWCi_LobbyGetChannelModeCallback callback, void* param);

// チャンネルトピックをセットする。
DWCi_LOBBY_RESULT   DWCi_LobbySetChannelTopic(const char* channelName, const char* topic);

// チャンネルパスワードをセットする。
DWCi_LOBBY_RESULT DWCi_LobbySetChannelPassword(const char* channelName, BOOL enable, const char* password);

#ifdef WIN32
#pragma pack(pop)
#endif

#ifdef __cplusplus
}
#endif

#endif
