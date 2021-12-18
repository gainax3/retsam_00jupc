/*---------------------------------------------------------------------------*
  Project:  NitroDWC Libraries
  File:     ./ppwlobby/src/ppw_lobby.cpp

  Copyright 2005-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/
/**
 * @file ppw_lobby.h
 * 
 */

/**
 * @page PPW_LOBBY ポケモンプラチナ Wi-Fi ロビーライブラリ
 *
 * @section Introduction
 *
 * このライブラリによって、GameSpyのIRCサーバを介して他のプレイヤーと通信することができます。
 *
 * すべての通信はサーバを中継するので、できる限り通信量が少なくなるようにしてください。\n
 * 目安としては、チャンネル全体に送信するもの(::PPW_LobbySendChannel*Message関数や::PPW_LobbySetChannelData関数によるブロードキャスト)は、
 * 小さいメッセージの場合は一人につき毎秒10メッセージ以下、大きいメッセージの場合は毎秒5メッセージ以下に抑えてください。\n
 * ライブラリ内部でのメモリ確保はDWC_Alloc関数を使用しています。\n
 * 本ライブラリはスレッドセーフではありません。\n
 * 二つ以上のスレッドで同時にライブラリの関数を呼び出さないでください。\n
 * 本ライブラリでプレイヤーを識別するために使用するユーザIDはGameSpyProfileID(pid)と一致しています。\n
 * 自分のユーザIDは初期化後に::PPW_LobbyGetMyUserId関数を使用することにより取得できます。\n
 * これはプレイヤーごとにユニークな値となります。\n
 * ただし、デバッグ用の::PPW_LobbyInitializePidAsync関数を使って初期化したとき、複数のプレイヤーが
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
 * ::PPW_LobbyInitializeAsync関数を使いライブラリを初期化します。
 * 内部で認証サーバの時刻を参照していますのであらかじめDWC_LoginAsync関数又は
 * DWC_NASLoginAsync関数でWi-Fiコネクションへの接続を完了している必要があります。 \n
 * 初期化後、ライブラリはまだ定員に達していないチャンネルを探し、入ります。\n
 * チャンネルが見つからなかった場合は自分でチャンネルを作成し、入ります。\n
 * このチャンネルをメインチャンネルと呼びます。\n
 * メインチャンネルに入ると::PPW_LobbyJoinChannelCallbackがコールバックされ、
 * ライブラリのステートが::PPW_LOBBY_STATE_READYとなります。
 *
 *
 * ■サブチャンネル
 * 
 * メインチャンネルの他にもう一つ別のチャンネルに入ることができます。\n
 * これをサブチャンネルと呼びます。
 * サブチャンネルに入っている間もメインチャンネルに関するメッセージを受信します。\n
 * 足跡ボード、世界時計、ロビーニュースはこのサブチャンネルを使い実装してください。
 *
 * 
 * ■メッセージの送受信
 *
 * ライブラリのステートが::PPW_LOBBY_STATE_READYのときに各種メッセージの送受信、
 * チャンネルデータの送受信ができます。
 *
 * 
 * ■ミニゲームの募集・参加
 * 
 * ミニゲームの募集はAnybodyマッチを併用して行います。\n
 * ロビーで募集をかけながらマッチング処理を進めます。募集者側と参加者側に分けて処理の流れを記します。
 * 
 * - 募集者\n
 * ::PPW_LobbyStartRecruit関数を呼び、生成されたマッチング指標文字列をDWC_AddMatchKeyString関数でセットします。\n
 * セットしたマッチング指標文字列と一致するようにfilterパラメータをセットしDWC_ConnectToAnybodyAsync関数を呼び出します。\n
 * マッチメイク中は接続した人数をDWC_GetNumConnectionHost関数で参照し、
 * 現在の参加人数が変動したら::PPW_LobbyUpdateRecruitInfo関数で現在集まっている人数(currentNum)を変更します。\n
 * マッチメイクが完了したら::PPW_LobbyStopRecruit関数を呼び出してください。
 * 
 * - 参加者\n
 * ::PPW_LobbyRecruitCallbackコールバックが呼ばれた後、
 * そのミニゲームに参加するなら::PPW_LobbyRecruitCallbackコールバックに含まれているマッチング指標文字列に一致するように
 * filterパラメータをセットしたDWC_ConnectToAnybodyAsync関数を呼び出します。\n
 * マッチメイクが完了するまで待ちます。
 *
 *
 * ■ライブラリの終了
 *
 * ::PPW_LobbyShutdownAsync関数を呼び出してライブラリを終了します。\n
 * この関数を呼び出した後、::PPW_LobbyProcess関数が::PPW_LOBBY_STATE_NOTINITIALIZEDを返すとシャットダウン完了です。
 * ::PPW_LobbyShutdownAsync関数と同時にDWC_CleanupInetAsync関数を呼び出さないようにしてください。
 * ロビーライブラリの終了を確認した後にDWC_CleanupInetAsync関数を呼び出してください。
 * 
 * 
 * ■エラー処理と表示
 *
 * 致命的なエラーが発生した場合は::PPW_LobbyProcess関数が::PPW_LOBBY_STATE_ERRORを返します。\n
 * この状態になるとほとんどの関数が使用不能になります。\n
 * また、内部でメモリ確保に失敗した場合、その関数での処理は正常に完了しますが、ライブラリのステートを::PPW_LOBBY_STATE_ERRORに変更します。\n
 * そのため次の関数呼び出しで::PPW_LOBBY_RESULT_ERROR_CONDITIONや::PPW_LOBBY_STATE_ERRORが返ります。\n
 * この状態になった場合は::PPW_LobbyGetLastError関数でエラー内容を確認後、::PPW_LobbyShutdownAsync関数でシャットダウンしてください。\n
 * また、このとき「Wi-Fiひろばで通信エラーが発生しました。」というメッセージと共に::PPW_LobbyToErrorCode関数を使用して得られたエラーコードを表示してください。\n
 * "Wi-Fiひろば"という名称は適宜ロビー機能の名称に書き換えてください。\n
 * P2Pマッチング中に発生したエラーはDWCライブラリの規定に沿ってエラーメッセージを表示してください。\n
 * その他、アプリケーション側でロビー機能を続行できなくなったと判断した場合のメッセージ表示についてはアプリケーションに任されます。
 *
 *</td></tr></table>
 */

#include <lobby/dwci_lobby.h>
#include "dwci_lobbyBase.h"
#include "dwci_lobbyUtil.h"
#include "ppw_data.h"
#include "ppw_parser.h"
#include "dwci_encsessioncpp.h"
#include "WFLBY_USER_PROFILE.h"

// 定数定義
static const char* const PPW_LOBBY_CHANNEL_NAME[] =
{
    "M",    // Main
    "F1",   // Foot1
    "F2",   // Foot2
    "C",    // Clock
    "N",    // News
    "Invalid"
};

// ENC用INIT_DATA
static const char* ENC_INIT_DATA = "uLMOGEiiJogofchScpXb000244fd00006015100000005b440e7epokemondpds";


// ENCで試行する回数
static const u32 ENC_RETRY_COUNT = 3;

// サーバの結果コードを表す列挙子
typedef enum ENC_SERVER_RESPONSE_CODE
{
    ENC_SERVER_RESPONSE_CODE_OK             = 0,
    ENC_SERVER_RESPONSE_CODE_INVALID_PARAM  = 6,
    ENC_SERVER_RESPONSE_CODE_INTERNAL_ERROR = 7,
    ENC_SERVER_RESPONSE_CODE_DATABASE_ERROR = 8,
    ENC_SERVER_RESPONSE_CODE_MAINTENANCE    = 9,
    ENC_SERVER_RESPONSE_CODE_UNKNOWN_ERROR  = 10
}
ENC_SERVER_RESPONSE_CODE;


// 不正チェックをするアドレス
static const char* ENC_URL_CHECK_PROFILE = "/web/enc/lobby/checkProfile.asp";

// スケジュールを取得するアドレス
static const char* ENC_URL_GET_SCHEDULE = "/web/enc/lobby/getSchedule.asp";

// VIP情報を取得するアドレス
static const char* ENC_URL_GET_VIP = "/web/enc/lobby/getVIP.asp";

// アンケート情報を取得するアドレス
static const char* ENC_URL_GET_QUESTIONNAIRE = "/web/enc/lobby/getQuestionnaire.asp";

static const char* ENC_URL_SUBMIT_QUESTIONNAIRE = "/web/enc/lobby/submitQuestionnaire.asp";

// タイマーID
static const u32 PPW_LOBBY_TIMER_ID_SYSTEM_CHANNEL_DATA                 = 1;
static const u32 PPW_LOBBY_TIMER_ID_USER_CHANNEL_DATA                   = 2;
static const u32 PPW_LOBBY_TIMER_ID_RECRUIT                             = 3;
static const u32 PPW_LOBBY_TIMER_ID_CHECK_MAINCHANNEL_MODE              = 4;
static const u32 PPW_LOBBY_TIMER_ID_CHECK_MAINCHANNEL_SYSTEMDATA        = 5;
static const u32 PPW_LOBBY_TIMER_ID_CHECK_LOBBY_INFO                    = 6;

// タイマーインターバル(秒)
static const u32 PPW_LOBBY_TIMER_INTERVAL_SYSTEM_CHANNEL_DATA           = 3; // MatchComment: 2 -> 3
static const u32 PPW_LOBBY_TIMER_INTERVAL_USER_CHANNEL_DATA             = 4; // MatchComment: 3 -> 4
static const u32 PPW_LOBBY_TIMER_INTERVAL_RECRUIT                       = 5;
static const u32 PPW_LOBBY_TIMER_INTERVAL_CHECK_MAINCHANNEL_MODE        = 3;
static const u32 PPW_LOBBY_TIMER_INTERVAL_CHECK_MAINCHANNEL_SYSTEMDATA  = 4;
static const u32 PPW_LOBBY_TIMER_INTERVAL_CHECK_LOBBY_INFO              = 4;



// スタティック変数宣言
static PPW_Lobby* s_lobby = NULL;

// グローバル関数
PPW_Lobby* PPW_GetLobby()
{
    return s_lobby;
}

// ローカル関数プロトタイプ
static PPW_LOBBY_RESULT PPW_LobbyInitializeImplAsync(const char* gameName, const char* secretKey, DWC_LOBBY_CHANNEL_PREFIX prefix,
                                                     const PPW_LobbyCallbacks* lobbyCallbacks, s32 pid, const u8* userProfile, u32 userProfileSize,
                                                     BOOL isValidInitialize);
static PPW_LOBBY_RESULT PPW_LobbyEnterMainChannel();
static void StartDownload();
static DWCi_String PPW_LobbyCreateBaseChannelName(PPW_LOBBY_CHANNEL_KIND nameKind, const DWCi_String& uniqueName);
static DWCi_String PPW_LobbyGenerateUniqueChannelName();
static BOOL PPW_IsMainChannelReady();
static void PPW_ChannelDataHandler(BOOL success, BOOL broadcast, const char* channelName, s32 userId, const char* key, const char* value, void* param);
static BOOL PPW_SystemChannelDataHandler(BOOL success, BOOL broadcast, PPW_LOBBY_CHANNEL_KIND channelKind, s32 userId,
                                         const char* key, const u8* data, u32 dataSize, void* param);

// タイマーコールバック
static BOOL PPW_LobbyBroadcastMyUserProfile(void* param);
static BOOL PPW_LobbyBroadcastMySystemProfile(void* param);
static BOOL PPW_LobbySendRecruitInfo(void* param);
static BOOL PPW_LobbyCheckMainChannelMode(void* param);
static BOOL PPW_LobbyCheckLobbyInfo(void* param);

// ENCコールバック
static void CheckProfileCompletedCallback(BOOL success, const u8* data, u32 size, void* param);
static void GetScheduleCompletedCallback(BOOL success, const u8* data, u32 size, void* param);
static void GetVipCompletedCallback(BOOL success, const u8* data, u32 size, void* param);
static void GetQuestionnaireCompletedCallback(BOOL success, const u8* data, u32 size, void* param);
static void SubmitQuestionnaireCompletedCallback(BOOL success, const u8* data, u32 size, void* param);

// 下位ライブラリからのコールバック
static void LobbyJoinMainChannelCallback(BOOL success, DWCi_LOBBY_ENTER_RESULT result, const char* channelName, void* param);
static void LobbyJoinSubChannelCallback(BOOL success, DWCi_LOBBY_ENTER_RESULT result, const char* channelName, void* param);
static void LobbyPlayerConnectedCallback(const char* channelName, s32 userId, void* param);
static void LobbyPlayerDisconnectedCallback(const char* channelName, s32 userId, void* param);
static void LobbyChannelMassageReceivedCallback(const char* channelName, s32 userId, const char* message, void* param);
static void LobbyUserMassageReceivedCallback(s32 userId, const char* message, void* param);
static void LobbyExcessFloodCallback(u32 floodWeight, void* param);
static void LobbyBroadcastDataCallback(const char* channelName, s32 userId,
                                       const char* key, const char* value, void* param);
static void LobbyGetChannelDataCallback(BOOL success, const char* channelName, s32 userId,
                                        const char* key, const char* value, void* param);
static void LobbyChannelDataCallback(BOOL success, BOOL broadcast, const char* channelName, s32 userId,
                                        const char* key, const char* value, void* param);
static void LobbyEnumChannelsCallback(BOOL success, s32 numChannels, const char** channels, const char** topics, const s32* numUsers, void* param);
static void LobbyEnumUsersCallback(BOOL success, const char* channelName, s32 numUsers, const s32* userIds, void* param);
static void LobbyCheckMainChannelModeCallback(BOOL success, const char* channelName, const DWCi_LobbyChannelMode* mode, void* param);
static void LobbyCheckMainChannelSystemProfileCallback(BOOL success, const char* channelName, s32 userId, const char* key, const char* value, void* param);
static void LobbyCheckLobbyInfoCallback(BOOL success, const char* channelName, s32 userId, const char* key, const char* value, void* param);
static void LobbyTopicChangedCallback(const char* channelName, const char* topic, void* param);


// resultコード変換
static inline PPW_LOBBY_RESULT ToPpwResult(DWCi_LOBBY_RESULT result)
{
    // ここでコンパイルエラーが発生した場合はresultコードの整合性がとれていないので要修正
    DWCi_STATIC_ASSERT(DWCi_LOBBY_RESULT_MAX == PPW_LOBBY_RESULT_MAX);
    
    PPW_LOBBY_RESULT convertedResult = (PPW_LOBBY_RESULT)result;
    
    // 致命的なエラーが発生しているときはこちらもエラーをセットする。
    s_lobby->SetError(convertedResult);
    
    return convertedResult;
}

// 初期化されているかチェックする
#define RETURN_IF_NOTINITIALIZED(retval) if(!s_lobby){return retval;}

// 致命的なエラーが起きていないかチェックする
#define RETURN_IF_ERROR_STATE(retval) \
    if(s_lobby && s_lobby->GetState() == PPW_LOBBY_STATE_ERROR)\
    {\
        return retval;\
    }

// ステートがREADYであるかチェックする
#define RETURN_IF_NOTREADY(retval) \
    if(s_lobby && s_lobby->GetState() != PPW_LOBBY_STATE_READY)\
    {\
        return retval;\
    }

// ENCで送信する基本情報
class EncCommonRequest : public DWCi_Base
{
    s32 pid;
    u16 requestVersion;
    u8 romVersion;
    u8 language;
    u8 macAddr[6];
    u16 reserve1;
    u32 reserve2;
    
public:
    EncCommonRequest()
    {
        const PPW_LobbyInnerProfile::UserProfile& userProfile = s_lobby->GetProfileManager().GetMyProfile().GetUserProfile();
        const WFLBY_USER_PROFILE* userProfileDetail = (const WFLBY_USER_PROFILE*)DWCi_GetVectorBuffer(userProfile.data);
    
        pid = s_lobby->GetPid();
        requestVersion = PPW_LOBBY_ENC_REQUEST_VERSION;
        romVersion = userProfileDetail->rom_code;
        language = userProfileDetail->region_code;
        DWCi_Np_GetMacAddress(macAddr);
    }
    // 継承禁止
    ~EncCommonRequest(){};
};


/** 
 * @brief ロビーライブラリを初期化します。
 * 
 * ロビーライブラリを初期化します。\n
 * この関数を呼ぶ前にあらかじめDWC_LoginAsync関数又はDWC_NASLoginAsync関数でWi-Fiコネクションへの接続を完了している必要があります。\n
 * この関数が成功した場合は::PPW_LobbyProcess関数を毎ゲームフレーム程度の頻度で呼び出してください。
 * 
 * @param[in] gameName NULL終端されたゲーム名。
 * @param[in] secretKey NULL終端されたシークレットキー。
 * @param[in] prefix チャンネル名に付加するプリフィックス。製品用、デバッグ用、テスト用で入るチャンネルを分けることができます。\n
 *                   製品用には::DWC_LOBBY_CHANNEL_PREFIX_RELEASEを、デバッグ用には::DWC_LOBBY_CHANNEL_PREFIX_DEBUGを指定してください。\n
 *                   この値はスケジュールやVIP設定データの取得先にも影響します。::DWC_LOBBY_CHANNEL_PREFIX_RELEASEはリリースサーバから、
 *                   それ以外ではデバッグサーバからデータを取得します。
 * @param[in] lobbyCallbacks セットするコールバック。
 * @param[in] dwcUserData 有効なDWCユーザデータオブジェクト。
 * @param[in] userProfile 自動的に共有されるユーザ定義プロフィールの初期データ。
 * @param[in] userProfileSize userProfileのサイズ。1以上::PPW_LOBBY_MAX_BINARY_SIZE以下を指定してください。
 * 
 * @retval ::PPW_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::PPW_LOBBY_RESULT_ERROR_PARAM 引数が不正。
 * @retval ::PPW_LOBBY_RESULT_ERROR_ALLOC メモリ確保に失敗。
 * @retval ::PPW_LOBBY_RESULT_ERROR_SESSION 通信エラー。
 */
PPW_LOBBY_RESULT PPW_LobbyInitializeAsync(const char* gameName, const char* secretKey, DWC_LOBBY_CHANNEL_PREFIX prefix,
                                     const PPW_LobbyCallbacks* lobbyCallbacks, const DWCUserData* dwcUserData, const u8* userProfile, u32 userProfileSize)
{
    // ユーザデータの正当性を検証
    if( !DWC_CheckUserData( dwcUserData )
        || !DWC_CheckHasProfile( dwcUserData ))
    {
        // 不正なユーザデータ
        DWC_Printf(DWC_REPORTFLAG_ERROR, "PPW_LobbyInitializeAsync: Invalid dwcUserData.\n");
        return PPW_LOBBY_RESULT_ERROR_PARAM;
    }
    
    return PPW_LobbyInitializeImplAsync(gameName, secretKey, prefix, lobbyCallbacks, dwcUserData->gs_profile_id, userProfile, userProfileSize, TRUE);
}

/** 
 * @brief 任意のGameSpyProfileIDでロビーライブラリを初期化します。(デバッグ用)
 * 
 * ロビーライブラリを初期化します。(デバッグ用)\n
 * この関数を呼ぶ前にあらかじめDWC_LoginAsync関数又はDWC_NASLoginAsync関数でWi-Fiコネクションへの接続を完了している必要があります。\n
 * ::PPW_LobbyInitializeAsync関数はDWCUserData内に記録されているGameSpyProfileIDを使用するのに対し、こちらは任意のGameSpyProfileIDで初期化できます。\n
 * この関数が成功した場合は::PPW_LobbyProcess関数を毎ゲームフレーム程度の頻度で呼び出してください。
 * 
 * @param[in] gameName NULL終端されたゲーム名。
 * @param[in] secretKey NULL終端されたシークレットキー。
 * @param[in] prefix チャンネル名に付加するプリフィックス。製品用、デバッグ用、テスト用で入るチャンネルを分けることができます。\n
 *                   製品用には::DWC_LOBBY_CHANNEL_PREFIX_RELEASEを、デバッグ用には::DWC_LOBBY_CHANNEL_PREFIX_DEBUGを指定してください。
 * @param[in] lobbyCallbacks セットするコールバック。
 * @param[in] pid GameSpyProfileID。10万以上の正の数を指定してください。
 * @param[in] userProfile 自動的に共有されるユーザ定義プロフィール。
 * @param[in] userProfileSize userProfileのサイズ。
 * 
 * @retval ::PPW_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::PPW_LOBBY_RESULT_ERROR_PARAM 引数が不正。
 * @retval ::PPW_LOBBY_RESULT_ERROR_STATE すでに初期化されています。
 * @retval ::PPW_LOBBY_RESULT_ERROR_ALLOC メモリ確保に失敗。
 * @retval ::PPW_LOBBY_RESULT_ERROR_SESSION 通信エラー。
 */
PPW_LOBBY_RESULT PPW_LobbyInitializePidAsync(const char* gameName, const char* secretKey, DWC_LOBBY_CHANNEL_PREFIX prefix,
                                     const PPW_LobbyCallbacks* lobbyCallbacks, s32 pid, const u8* userProfile, u32 userProfileSize)
{
    return PPW_LobbyInitializeImplAsync(gameName, secretKey, prefix, lobbyCallbacks, pid, userProfile, userProfileSize, FALSE);
}

PPW_LOBBY_RESULT PPW_LobbyInitializeImplAsync(const char* gameName, const char* secretKey, DWC_LOBBY_CHANNEL_PREFIX prefix,
                                     const PPW_LobbyCallbacks* lobbyCallbacks, s32 pid, const u8* userProfile, u32 userProfileSize, BOOL isValidInitialize)
{
    if(s_lobby)
    {
        // すでに初期化されている
        DWC_Printf(DWC_REPORTFLAG_ERROR, "PPW_LobbyInitializeImplAsync: Already initialized.");
        return PPW_LOBBY_RESULT_ERROR_STATE;
    }
    
    DWC_ASSERTMSG(userProfile && userProfileSize != 0, "PPW_LobbyInitializeImplAsync: userProfile must be set.");
    
    DWCi_LobbyGlobalCallbacks gc;
    gc.lobbyUserMassageReceivedCallback = LobbyUserMassageReceivedCallback;
    gc.lobbyExcessFloodCallback = LobbyExcessFloodCallback;
    gc.param = (void*)PPW_LOBBY_CHANNEL_KIND_INVALID;
    
    s_lobby = new PPW_Lobby(*lobbyCallbacks, pid, prefix, isValidInitialize);
    if(!s_lobby)
    {
        return PPW_LOBBY_RESULT_ERROR_ALLOC;
    }

    PPW_LOBBY_RESULT result = ToPpwResult(DWCi_LobbyInitializePidAsync(gameName, secretKey, prefix, &gc, pid));
    if(result != PPW_LOBBY_RESULT_SUCCESS)
    {
        DWC_SAFE_DELETE(s_lobby);
        return result;
    }

    // 自分の設定。入室時刻はメインチャンネル入室時に改めてセットされる。
    s_lobby->GetProfileManager().InitializeMyProfile(pid, userProfile, userProfileSize);

    // チャットサーバ接続と同時にダウンロード開始
    StartDownload();
    
    s_lobby->SetState(PPW_LOBBY_STATE_CONNECTING);
    return PPW_LOBBY_RESULT_SUCCESS;
}

/** 
 * @brief ロビーライブラリを終了します。
 * 
 * ロビーライブラリを終了します。\n
 * ::PPW_LobbyProcess関数が::PPW_LOBBY_STATE_NOTINITIALIZEDを返すとシャットダウン完了です。
 * ::PPW_LobbyShutdownAsync関数と同時にDWC_CleanupInetAsync関数を呼び出さないようにしてください。
 * ロビーライブラリの終了を確認した後にDWC_CleanupInetAsync関数を呼び出してください。
 * それ以外に呼び出すタイミングに制限はありません。すでにロビーライブラリが終了している場合は何もしません。\n
 * @retval ::PPW_LOBBY_RESULT_SUCCESS 成功。
 */
PPW_LOBBY_RESULT PPW_LobbyShutdownAsync()
{
    RETURN_IF_NOTINITIALIZED(PPW_LOBBY_RESULT_SUCCESS);
    
    // すでにシャットダウン中のときは何もしない
    if(s_lobby->GetState() == PPW_LOBBY_STATE_CLOSING)
    {
        return PPW_LOBBY_RESULT_SUCCESS;
    }
    
    // これ以上タイマーが発動しないようにタイマー破棄
    s_lobby->GetTimerManager().ClearTimer();
    
    s_lobby->SetState(PPW_LOBBY_STATE_CLOSING);
    return ToPpwResult(DWCi_LobbyShutdownAsync());
}

/** 
 * @brief ロビーライブラリの処理を進めます。
 * 
 * ロビーライブラリの処理を進めます。\n
 * 毎ゲームフレーム程度の頻度で呼び出してください。\n
 * ::PPW_LOBBY_STATE_ERRORを返したときはPPW_LobbyGetLastError関数でエラー内容を取得後::PPW_LobbyShutdownAsync関数を呼んでシャットダウンしてください。\n
 * ::PPW_LOBBY_STATE_READYのときのみメッセージ送信、チャンネル入室などの関数を呼べます。
 * 
 * @retval ::PPW_LOBBY_STATE_NOTINITIALIZED       ライブラリが初期化されていません。
 * @retval ::PPW_LOBBY_STATE_CONNECTING           チャットサーバに接続中。
 * @retval ::PPW_LOBBY_STATE_CONNECTED            チャットサーバに接続完了。
 * @retval ::PPW_LOBBY_STATE_DOWNLOAD             サーバから設定をダウンロード開始。
 * @retval ::PPW_LOBBY_STATE_DOWNLOAD_WAIT        サーバから設定をダウンロード中。
 * @retval ::PPW_LOBBY_STATE_SEARCHINGCHANNEL     メインチャンネルを検索中。
 * @retval ::PPW_LOBBY_STATE_ENTERMAINCHANNEL     メインチャンネルに入室開始。
 * @retval ::PPW_LOBBY_STATE_PREPARINGMAINCHANNEL メインチャンネルに入室及び準備中。
 * @retval ::PPW_LOBBY_STATE_READY                メインチャンネルに入室完了。
 * @retval ::PPW_LOBBY_STATE_CLOSING              ライブラリ終了処理中。
 * @retval ::PPW_LOBBY_STATE_ERROR                ライブラリに致命的なエラーが発生中。
 */
PPW_LOBBY_STATE PPW_LobbyProcess()
{
    {
        // ダウンロード処理
        BOOL bEncSession = DWCi_EncSession::Process();
        
        if(bEncSession)
        {
            // ロビーライブラリがシャットダウン済みでもダウンロード完了まで待つ。
            RETURN_IF_NOTINITIALIZED(PPW_LOBBY_STATE_CLOSING);
        }
        else
        {
            // NOTINITIALIZEDを返すのはここのみ
            RETURN_IF_NOTINITIALIZED(PPW_LOBBY_STATE_NOTINITIALIZED);
        }
    }
    
    RETURN_IF_ERROR_STATE(PPW_LOBBY_STATE_ERROR);

    // インナーライブラリの状態確認
    switch(DWCi_LobbyProcess())
    {
    case DWCi_LOBBY_STATE_NOTINITIALIZED:
        // 下位ライブラリが終了しているのでこのライブラリも終了
        PPW_LobbyShutdownAsync();    // 念のためシャットダウンを呼ぶ
        DWC_SAFE_DELETE(s_lobby);
        return PPW_LOBBY_STATE_CLOSING;
    case DWCi_LOBBY_STATE_CONNECTING:
        break;
    case DWCi_LOBBY_STATE_CONNECTINGWAIT:
        break;
    case DWCi_LOBBY_STATE_CONNECTED:
        if(s_lobby->GetState() == PPW_LOBBY_STATE_CONNECTING)
        {
            // 最初に接続したときのみメインチャンネル入室のステップに状態遷移させる
            s_lobby->SetState(PPW_LOBBY_STATE_CONNECTED);
        }
        break;
    case DWCi_LOBBY_STATE_CLOSING:
        break;
    case DWCi_LOBBY_STATE_ERROR:
        s_lobby->SetError(DWCi_LobbyGetLastError());
        break;
    default:
        DWC_ASSERTMSG(FALSE, "PPW_LobbyProcess: Inner lib switch is invalid state");
        break;
    }

    // 管理オブジェクト処理
    s_lobby->ProcessManagers();

    // このライブラリの状態更新
    switch(s_lobby->GetState())
    {
    case PPW_LOBBY_STATE_NOTINITIALIZED:
        // ここには来ない
        DWC_ASSERTMSG(FALSE, "PPW_LobbyProcess: Internal Error.");
        return PPW_LOBBY_STATE_CLOSING;
    case PPW_LOBBY_STATE_CONNECTING:
        break;
    case PPW_LOBBY_STATE_CONNECTED:
        // 何もしない。ダウンロード開始処理はInitialize時に行っている。
        s_lobby->SetState(PPW_LOBBY_STATE_DOWNLOAD);
        break;
    case PPW_LOBBY_STATE_DOWNLOAD:
        // ダウンロードが完了したかチェック
        if(s_lobby->GetLobbyInnerInfoManager().IsDownloaded())
        {
            s_lobby->SetState(PPW_LOBBY_STATE_DOWNLOAD_WAIT);
        }
        break;
    case PPW_LOBBY_STATE_DOWNLOAD_WAIT: // stateの名前がおかしいけどそのまま
        // チャンネルサーチ開始
        DWCi_LobbyEnumChannelsAsync((PPW_LobbyCreateBaseChannelName(PPW_LOBBY_CHANNEL_KIND_MAIN, "*")).c_str(), LobbyEnumChannelsCallback, NULL);
        s_lobby->SetState(PPW_LOBBY_STATE_SEARCHINGCHANNEL);
        break;
    case PPW_LOBBY_STATE_SEARCHINGCHANNEL:
        break;
    case PPW_LOBBY_STATE_ENTERMAINCHANNEL:
        {
            // メインチャンネルに入る。もしエラーがあればセットする。
            PPW_LOBBY_RESULT ret = PPW_LobbyEnterMainChannel();
            if(ret != PPW_LOBBY_RESULT_SUCCESS)
            {
                s_lobby->SetError(ret);
            }
            else
            {
                s_lobby->SetState(PPW_LOBBY_STATE_PREPARINGMAINCHANNEL);
            }
        }
        break;
    case PPW_LOBBY_STATE_PREPARINGMAINCHANNEL:
        // メインチャンネル接続確認
        if(PPW_IsMainChannelReady())
        {
            // 全ての情報取得が終わったので準備完了を他のプレイヤーに通知する
            PPW_LobbyInnerProfile& profile = s_lobby->GetProfileManager().GetMyProfile();
            PPW_LobbyInnerProfile::SystemProfile systemProfile = profile.GetSystemProfileConfirm();
            systemProfile.ready = TRUE;
            profile.SetSystemProfile(systemProfile);
            
            DWC_Printf(DWC_REPORTFLAG_INFO, "Main channel is ready.\n");
            s_lobby->SetState(PPW_LOBBY_STATE_READY);
            s_lobby->GetCallbacks().lobbyJoinChannelCallback(TRUE, PPW_LOBBY_CHANNEL_KIND_MAIN);
        }
        break;
    case PPW_LOBBY_STATE_READY:
        break;
    case PPW_LOBBY_STATE_CLOSING:
        break;
    case PPW_LOBBY_STATE_ERROR:
        break;
    default:
        DWC_ASSERTMSG(FALSE, "PPW_LobbyProcess: Invalid state");
        break;
    }
    
    return s_lobby->GetState();
}

/** 
 * @brief 足跡ボード、世界時計、ロビーニュース(サブチャンネル)に参加します。
 * 
 * 足跡ボード、世界時計、ロビーニュースに参加します。入室が完了または失敗すると::PPW_LobbyJoinChannelCallbackコールバックが呼ばれます。
 * 
 * @param[in] channelKind 参加するチャンネルの種類。
 * 
 * @retval ::PPW_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::PPW_LOBBY_RESULT_ERROR_STATE ステートがREADYではないか、すでにいずれかのサブチャンネルに入っています。
 * @retval ::PPW_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
PPW_LOBBY_RESULT PPW_LobbyJoinSubChannelAsync(PPW_LOBBY_CHANNEL_KIND channelKind)
{
    RETURN_IF_NOTINITIALIZED(PPW_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(PPW_LOBBY_RESULT_ERROR_CONDITION);
    RETURN_IF_NOTREADY(PPW_LOBBY_RESULT_ERROR_STATE);
    
    if(s_lobby && s_lobby->GetSubChannel().Exist())
    {
        // すでにサブチャンネルに参加している
        return PPW_LOBBY_RESULT_ERROR_STATE;
    }

    DWCi_LobbyChannelCallbacks cc;
    cc.lobbyPlayerConnectedCallback = LobbyPlayerConnectedCallback;
    cc.lobbyPlayerDisconnectedCallback = LobbyPlayerDisconnectedCallback;
    cc.lobbyChannelMassageReceivedCallback = LobbyChannelMassageReceivedCallback;
    cc.lobbyBroadcastDataCallback = LobbyBroadcastDataCallback;
    cc.lobbyTopicChangedCallback = LobbyTopicChangedCallback;
    cc.param = (void*)channelKind;
    
    // サブチャンネルに入る。
    DWCi_String channelName = PPW_LobbyCreateBaseChannelName(channelKind, s_lobby->GetUniqueChannelName());
    const DWCi_LobbyChannelMode* mode = PPW_LobbyGetChannelMode(channelKind);
    PPW_LOBBY_RESULT result = ToPpwResult(DWCi_LobbyJoinChannelLimitAsync(channelName.c_str(), NULL, mode->limit,
                                                                          &cc, LobbyJoinSubChannelCallback, (void*)channelKind));
    if(result != PPW_LOBBY_RESULT_SUCCESS)
    {
        return result;
    }
    s_lobby->SetSubChannel(PPW_Channel(channelKind, channelName));
    return PPW_LOBBY_RESULT_SUCCESS;
}

/** 
 * @brief 参加中の足跡ボード、世界時計、ロビーニュース(サブチャンネル)から出ます。
 * 
 * 参加中の足跡ボード、世界時計、ロビーニュース(サブチャンネル)から出ます。
 * 
 * @retval ::PPW_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::PPW_LOBBY_RESULT_ERROR_STATE ステートがREADYではないか、サブチャンネルに入っていません。
 * @retval ::PPW_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
PPW_LOBBY_RESULT PPW_LobbyLeaveSubChannel()
{
    RETURN_IF_NOTINITIALIZED(PPW_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(PPW_LOBBY_RESULT_ERROR_CONDITION);
    RETURN_IF_NOTREADY(PPW_LOBBY_RESULT_ERROR_STATE);
    
    if(!(s_lobby && s_lobby->GetSubChannel().Exist()))
    {
        // サブチャンネルに参加していない
        return PPW_LOBBY_RESULT_ERROR_STATE;
    }

    PPW_LOBBY_RESULT result = ToPpwResult(DWCi_LobbyLeaveChannel(s_lobby->GetSubChannel().GetName().c_str()));
    if(result != PPW_LOBBY_RESULT_SUCCESS)
    {
        DWC_ASSERTMSG(FALSE, "PPW_LobbyLeaveSubChannel: Internal error.");
    }
    
    s_lobby->GetSubChannel().Clear();
    
    // システムデータを更新
    PPW_LobbyInnerProfile& profile = s_lobby->GetProfileManager().GetMyProfile();
    PPW_LobbyInnerProfile::SystemProfile systemProfile = profile.GetSystemProfileConfirm();
    systemProfile.subChannelKind = PPW_LOBBY_CHANNEL_KIND_INVALID;
    profile.SetSystemProfile(systemProfile);
    
    return PPW_LOBBY_RESULT_SUCCESS;
}

/**
 * @brief 時刻情報を取得します。
 * 
 * 時刻情報を取得します。
 * 
 * @param[out] timeInfo 時刻情報を格納する領域を指定してください。部屋がロックされていない場合、lockedTimeメンバは::PPW_LOBBY_INVALID_TIMEとなります。
 * 
 * @retval ::PPW_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::PPW_LOBBY_RESULT_ERROR_STATE ステートがREADYではありません。
 * @retval ::PPW_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
PPW_LOBBY_RESULT PPW_LobbyGetTimeInfo(PPW_LobbyTimeInfo* timeInfo)
{
    RETURN_IF_NOTINITIALIZED(PPW_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(PPW_LOBBY_RESULT_ERROR_CONDITION);
    RETURN_IF_NOTREADY(PPW_LOBBY_RESULT_ERROR_STATE);
    
    s_lobby->GetLobbyInnerInfoManager().GetTimeInfo(*timeInfo);
    
    return PPW_LOBBY_RESULT_SUCCESS;
}

/**
 * @brief スケジュールを強制的にセットします。(デバッグ用)
 * 
 * スケジュールを強制的にセットします。(デバッグ用)\n
 * ::PPW_LobbyInitializeAsync関数または::PPW_LobbyInitializePidAsync関数で初期化した直後に呼び出してください。
 * 
 * @param[in] schedule セットするスケジュール。イベントとして::PPW_LOBBY_TIME_EVENT_LOCKを指定することはできません。
 * @param[in] scheduleSize scheduleに確保したメモリ量(byte)。
 * 
 * @retval ::PPW_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::PPW_LOBBY_RESULT_ERROR_PARAM scheduleのscheduleRecordNumメンバから計算した構造体容量がsizeを上回っています。
 * @retval ::PPW_LOBBY_RESULT_ERROR_STATE 初期化されていません。
 * @retval ::PPW_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
PPW_LOBBY_RESULT PPW_LobbySetSchedule(const PPW_LobbySchedule* schedule, u32 scheduleSize)
{
    RETURN_IF_NOTINITIALIZED(PPW_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(PPW_LOBBY_RESULT_ERROR_CONDITION);

    return s_lobby->GetLobbyInnerInfoManager().SetLobbyInfo((u8*)schedule, scheduleSize) ? PPW_LOBBY_RESULT_SUCCESS : PPW_LOBBY_RESULT_ERROR_PARAM;
}

/**
 * @brief スケジュールを取得します。
 * 
 * スケジュールを取得します。\n
 * ::PPW_LobbyJoinChannelCallbackコールバックが発生し、メインチャンネルに入室したときにスケジュールが確定しますのでそれ以降に使用することができます。
 * それ以前に呼び出した場合は初期設定のスケジュールが取得されるか、::PPW_LobbySetSchedule関数でセットしてあればそのスケジュールが取得されます。\n
 * スケジュールを保存する領域を引数に渡してください。\n
 * scheduleにNULL、もしくは必要なサイズがscheduleSizeを下回っていた場合はscheduleSizeに必要なサイズが格納されます。
 * 
 * @param[in, out] schedule scheduleSizeの領域を確保した、スケジュールを格納するポインタ。必要なサイズを満たしている場合のみスケジュールが格納されます。
 * @param[in, out] scheduleSize 確保したscheduleのサイズ(byte)を指定してください。関数は内部で実際のscheduleのサイズを代入します。
 * 
 * @retval ::PPW_LOBBY_RESULT_SUCCESS 成功。scheduleにスケジュールが格納され、scheduleSizeにそのサイズが格納されます。
 *                                  ただしscheduleSizeが0だった場合はscheduleは変化しません。
 * @retval ::PPW_LOBBY_RESULT_ERROR_PARAM scheduleがNULLもしくはsizeが必要サイズを下回っています。scheduleSize以上のメモリを確保して再試行してください。
 * @retval ::PPW_LOBBY_RESULT_ERROR_STATE ステートがREADYではありません。
 * @retval ::PPW_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
PPW_LOBBY_RESULT PPW_LobbyGetSchedule(PPW_LobbySchedule* schedule, u32* scheduleSize)
{
    RETURN_IF_NOTINITIALIZED(PPW_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(PPW_LOBBY_RESULT_ERROR_CONDITION);
    RETURN_IF_NOTREADY(PPW_LOBBY_RESULT_ERROR_STATE);
    
    return s_lobby->GetLobbyInnerInfoManager().GetLobbyInfo(schedule, *scheduleSize) ? PPW_LOBBY_RESULT_SUCCESS : PPW_LOBBY_RESULT_ERROR_PARAM;
}

/**
 * @brief VIP情報を取得します。
 * 
 * VIP情報を取得します。\n
 * ::PPW_LobbyGetVipCallbackコールバックが呼び出された後から使用することができます。それ以前に呼び出した場合はrecordNumが必ず0になります。
 * VIP情報を保存する領域を引数に渡してください。\n
 * recordsにNULL、もしくは必要な個数がrecordNumを下回っていた場合はrecordNumに必要な個数が格納されます。
 * 
 * @param[in, out] records recordNum*sizeof(::PPW_LobbyVipRecord)の領域を確保した、VIP情報を格納するポインタ。
 *                         必要なサイズを満たしている場合のみVIP情報が格納されます。
 * @param[in, out] recordNum 確保したrecordsの個数を指定してください。関数は実際のrecordNumの個数を代入します。
 * 
 * @retval ::PPW_LOBBY_RESULT_SUCCESS 成功。recordsにVIP情報が格納され、recordNumにそのサイズが格納されます。
 *                                  ただしrecordNumが0だった場合はrecordsは変化しません。
 * @retval ::PPW_LOBBY_RESULT_ERROR_PARAM recordsがNULLもしくはrecordNumが必要サイズを下回っています。
 *                                        recordNum*sizeof(::PPW_LobbyVipRecord)以上のメモリを確保して再試行してください。
 * @retval ::PPW_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
PPW_LOBBY_RESULT PPW_LobbyGetVip(PPW_LobbyVipRecord* records, u32* recordNum)
{
    RETURN_IF_NOTINITIALIZED(PPW_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(PPW_LOBBY_RESULT_ERROR_CONDITION);
    
    return s_lobby->GetLobbyInnerInfoManager().GetVipRecords(records, *recordNum) ? PPW_LOBBY_RESULT_SUCCESS : PPW_LOBBY_RESULT_ERROR_PARAM;
}

/**
 * @brief 参加中の足跡ボード、世界時計、ロビーニュース(サブチャンネル)の状態を取得します。
 * 
 * 参加中の足跡ボード、世界時計、ロビーニュース(サブチャンネル)の状態を取得します。
 * 
 * @retval ::PPW_LOBBY_CHANNEL_STATE_NONE サブチャンネルに参加していません。
 * @retval ::PPW_LOBBY_CHANNEL_STATE_UNAVAILABLE サブチャンネルに入室準備中です。
 * @retval ::PPW_LOBBY_CHANNEL_STATE_AVAILABLE サブチャンネルに入室済みです。
 * @retval ::PPW_LOBBY_CHANNEL_STATE_ERROR 致命的なエラーにより状態を取得できませんでした。またはステートがREADYではありません。
 */
PPW_LOBBY_CHANNEL_STATE PPW_LobbyGetSubChannelState()
{
    RETURN_IF_NOTINITIALIZED(PPW_LOBBY_CHANNEL_STATE_ERROR);
    RETURN_IF_ERROR_STATE(PPW_LOBBY_CHANNEL_STATE_ERROR);
    RETURN_IF_NOTREADY(PPW_LOBBY_CHANNEL_STATE_ERROR);
    
    if(!s_lobby->GetSubChannel().Exist())
    {
        // サブチャンネルに参加していない
        return PPW_LOBBY_CHANNEL_STATE_NONE;
    }

    BOOL state = DWCi_LobbyInChannel(s_lobby->GetSubChannel().GetName().c_str());
    return state ? PPW_LOBBY_CHANNEL_STATE_AVAILABLE : PPW_LOBBY_CHANNEL_STATE_UNAVAILABLE;
}

/**
 * @brief チャンネルデータをセットします。
 * 
 * チャンネルデータをセットします。\n
 * チャンネルデータは参加しているチャンネルのプレイヤー一人一人とチャンネル自体にセットすることができます。\n
 * ライブラリで使用しているものと合わせてそれぞれに最大20個のチャンネルデータをセットすることができます。\n
 * keyに"b_"から始まる文字列を指定すると、サーバにセットすると同時に他のプレイヤーにそれを通知します。(ブロードキャスト)\n
 * このブロードキャストは自分にもループバックされます。\n
 * 通知は::PPW_LobbyGetChannelDataCallbackコールバックにより行われます。このとき第二引数(broadcast)がTRUEとなります。
 * 
 * @param[in] userId チャンネルデータをセットするユーザID。::PPW_LOBBY_INVALID_USER_IDを指定するとチャンネル自体にセットします。
 * @param[in] channelKind チャンネルデータをセットするチャンネルの種類。
 * @param[in] key チャンネルデータにアクセスするためのキー。
 * @param[in] data keyに結びつけるデータ。
 * @param[in] dataSize dataのサイズ。1以上::PPW_LOBBY_MAX_BINARY_SIZE以下の値を指定してください。
 * 
 * @retval ::PPW_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::PPW_LOBBY_RESULT_ERROR_PARAM 引数が不正。
 * @retval ::PPW_LOBBY_RESULT_ERROR_STATE ステートがREADYではありません。
 * @retval ::PPW_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
PPW_LOBBY_RESULT PPW_LobbySetChannelData(s32 userId, PPW_LOBBY_CHANNEL_KIND channelKind, const char* key, const u8* data, u32 dataSize)
{
    // TODO: channelKindではなく、メインサブを指定するようにする
    RETURN_IF_NOTINITIALIZED(PPW_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(PPW_LOBBY_RESULT_ERROR_CONDITION);
    RETURN_IF_NOTREADY(PPW_LOBBY_RESULT_ERROR_STATE);

    return PPW_LobbySetChannelDataImpl(userId, channelKind, key, data, dataSize);
}

/**
 * @brief チャンネルデータを取得します。
 * 
 * チャンネルデータを取得します。\n
 * 完了すると::PPW_LobbyGetChannelDataCallbackコールバックが呼ばれます。
 * 
 * @param[in] userId チャンネルデータを取得するユーザID。::PPW_LOBBY_INVALID_USER_IDを指定するとチャンネル自体から取得します。
 * @param[in] channelKind チャンネルデータを取得するチャンネルの種類。
 * @param[in] key チャンネルデータにアクセスするためのキー。
 * @param[in] param このリクエストに関連づける任意の値。::PPW_LobbyGetChannelDataCallbackコールバックが呼ばれたときにparamに代入されます。
 * 
 * @retval ::PPW_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::PPW_LOBBY_RESULT_ERROR_PARAM 引数が不正。
 * @retval ::PPW_LOBBY_RESULT_ERROR_STATE ステートがREADYではありません。
 * @retval ::PPW_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
PPW_LOBBY_RESULT PPW_LobbyGetChannelDataAsync(s32 userId, PPW_LOBBY_CHANNEL_KIND channelKind, const char* key, void* param)
{
    RETURN_IF_NOTINITIALIZED(PPW_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(PPW_LOBBY_RESULT_ERROR_CONDITION);
    RETURN_IF_NOTREADY(PPW_LOBBY_RESULT_ERROR_STATE);
    
    DWCi_String channelName = s_lobby->GetChannelManager().GetChannelName(channelKind);
    if(channelName == "")
    {
        return PPW_LOBBY_RESULT_ERROR_CHANNEL;
    }

    return ToPpwResult(DWCi_LobbyGetChannelDataAsync(channelName.c_str(), userId, key, LobbyGetChannelDataCallback, param));
}

/**
 * @brief Ascii文字列のメッセージを指定したチャンネルに参加している全てのプレイヤーに送信します。
 * 
 * Ascii文字列のメッセージを指定したチャンネルに参加している全てのプレイヤーに送信します。\n
 * メッセージは自分にもループバックされます。
 * 
 * @param[in] channelKind 送信先のチャンネル。
 * @param[in] type このメッセージを表す任意の値。
 * @param[in] message 送信するNULL終端されたAscii文字列。終端込みで::PPW_LOBBY_MAX_STRING_SIZE以下の長さである必要があります。
 * 
 * @retval ::PPW_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::PPW_LOBBY_RESULT_ERROR_PARAM 引数が不正。
 * @retval ::PPW_LOBBY_RESULT_ERROR_STATE ステートがREADYではありません。
 * @retval ::PPW_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
PPW_LOBBY_RESULT PPW_LobbySendChannelStringMessage(PPW_LOBBY_CHANNEL_KIND channelKind, s32 type, const char* message)
{
    RETURN_IF_NOTINITIALIZED(PPW_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(PPW_LOBBY_RESULT_ERROR_CONDITION);
    RETURN_IF_NOTREADY(PPW_LOBBY_RESULT_ERROR_STATE);
    
    return PPW_LobbySendMessage(PPW_LOBBY_INVALID_USER_ID, channelKind, PPW_LobbyMessage::FORMAT_STRING, PPW_LobbyMessage::TARGET_APPLICATION,
                                type, (u8*)message, strlen(message) + 1);
}

/**
 * @brief Ascii文字列のメッセージを一人のプレイヤーに送信します。
 * 
 * Ascii文字列のメッセージを一人のプレイヤーに送信します。
 * 
 * @param[in] userId 送信先のユーザID。
 * @param[in] type このメッセージを表す任意の値。
 * @param[in] message 送信するNULL終端されたAscii文字列。終端込みで::PPW_LOBBY_MAX_STRING_SIZE以下の長さである必要があります。
 * 
 * @retval ::PPW_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::PPW_LOBBY_RESULT_ERROR_PARAM 引数が不正。
 * @retval ::PPW_LOBBY_RESULT_ERROR_STATE ステートがREADYではありません。
 * @retval ::PPW_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
PPW_LOBBY_RESULT PPW_LobbySendPlayerStringMessage(s32 userId, s32 type, const char* message)
{
    RETURN_IF_NOTINITIALIZED(PPW_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(PPW_LOBBY_RESULT_ERROR_CONDITION);
    RETURN_IF_NOTREADY(PPW_LOBBY_RESULT_ERROR_STATE);
    
    return PPW_LobbySendMessage(userId, PPW_LOBBY_CHANNEL_KIND_INVALID, PPW_LobbyMessage::FORMAT_STRING, PPW_LobbyMessage::TARGET_APPLICATION,
                                type, (u8*)message, strlen(message) + 1);
}

/**
 * @brief バイナリデータを指定したチャンネルに参加している全てのプレイヤーに送信します。
 * 
 * バイナリデータを指定したチャンネルに参加している全てのプレイヤーに送信します。\n
 * メッセージは自分にもループバックされます。
 * 
 * @param[in] channelKind 送信先のチャンネル。
 * @param[in] type このメッセージを表す任意の値。
 * @param[in] data 送信するバイナリデータ。
 * @param[in] dataSize dataのサイズ。1以上::PPW_LOBBY_MAX_BINARY_SIZE以下を指定してください。
 * 
 * @retval ::PPW_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::PPW_LOBBY_RESULT_ERROR_PARAM 引数が不正。
 * @retval ::PPW_LOBBY_RESULT_ERROR_STATE ステートがREADYではありません。
 * @retval ::PPW_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
PPW_LOBBY_RESULT PPW_LobbySendChannelBinaryMessage(PPW_LOBBY_CHANNEL_KIND channelKind, s32 type, const u8* data, u32 dataSize)
{
    RETURN_IF_NOTINITIALIZED(PPW_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(PPW_LOBBY_RESULT_ERROR_CONDITION);
    RETURN_IF_NOTREADY(PPW_LOBBY_RESULT_ERROR_STATE);
    
    return PPW_LobbySendMessage(PPW_LOBBY_INVALID_USER_ID, channelKind, PPW_LobbyMessage::FORMAT_BASE64, PPW_LobbyMessage::TARGET_APPLICATION,
                                type, data, dataSize);
}

/**
 * @brief バイナリデータを一人のプレイヤーに送信します。
 * 
 * バイナリデータを一人のプレイヤーに送信します。
 * 
 * @param[in] userId 送信先のユーザID。
 * @param[in] type このメッセージを表す任意の値。
 * @param[in] data 送信するバイナリデータ。
 * @param[in] dataSize dataのサイズ。1以上::PPW_LOBBY_MAX_BINARY_SIZE以下を指定してください。
 * 
 * @retval ::PPW_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::PPW_LOBBY_RESULT_ERROR_PARAM 引数が不正。
 * @retval ::PPW_LOBBY_RESULT_ERROR_STATE ステートがREADYではありません。
 * @retval ::PPW_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
PPW_LOBBY_RESULT PPW_LobbySendPlayerBinaryMessage(s32 userId, s32 type, const u8* data, u32 dataSize)
{
    RETURN_IF_NOTINITIALIZED(PPW_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(PPW_LOBBY_RESULT_ERROR_CONDITION);
    RETURN_IF_NOTREADY(PPW_LOBBY_RESULT_ERROR_STATE);
    
    return PPW_LobbySendMessage(userId, PPW_LOBBY_CHANNEL_KIND_INVALID, PPW_LobbyMessage::FORMAT_BASE64, PPW_LobbyMessage::TARGET_APPLICATION,
                                type, data, dataSize);
}

/**
 * @brief ロビーライブラリで使用している自分のユーザIDを取得します。
 * 
 * ロビーライブラリで使用している自分のユーザIDを取得します。
 * 
 * @retval 自分のユーザID。
 * @retval ::PPW_LOBBY_INVALID_USER_ID 初期化していません。
 */
s32 PPW_LobbyGetMyUserId()
{
    RETURN_IF_NOTINITIALIZED(PPW_LOBBY_INVALID_USER_ID);
    return s_lobby->GetPid();
}

/**
 * @brief 現在メインチャンネルにいる自分以外のユーザIDを列挙します。
 * 
 * 現在メインチャンネルにいる人のユーザIDを列挙します。\n
 * ユーザIDのリストを保存する領域を引数に渡してください。\n
 * userIdsにNULL、もしくは必要なサイズがnumを下回っていた場合はnumに必要なサイズが格納されます。
 * 
 * @param[in, out] userIds ユーザIDのリストを保存する領域を渡してください。NULLを指定するとnumにユーザIDの個数が格納されます。
 * @param[in, out] num userIdsに確保したs32の個数を指定してください。バイト数ではありません。\n
 *                 本関数が::PPW_LOBBY_RESULT_SUCCESSもしくは::PPW_LOBBY_RESULT_ERROR_PARAMを返すとユーザIDの個数が格納されます。
 * 
 * @retval ::PPW_LOBBY_RESULT_SUCCESS 成功。userIdsにユーザIDのリストが格納され、numにその個数が格納されます。\n
 *                                  ただしnumが0だった場合はuserIdsは変化しません。
 * @retval ::PPW_LOBBY_RESULT_ERROR_PARAM userIdsがNULLもしくはnumが必要サイズを下回っています。sizeof(s32)*num以上のメモリを確保して再試行してください。
 * @retval ::PPW_LOBBY_RESULT_ERROR_STATE ステートがREADYではありません。
 * @retval ::PPW_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
PPW_LOBBY_RESULT PPW_LobbyEnumUserId(s32* userIds, u32* num)
{
    RETURN_IF_NOTINITIALIZED(PPW_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(PPW_LOBBY_RESULT_ERROR_CONDITION);
    RETURN_IF_NOTREADY(PPW_LOBBY_RESULT_ERROR_STATE);
    
    u32 orgNum = *num;

    std::vector<s32, DWCi_Allocator<s32> > result = s_lobby->GetProfileManager().EnumId();
    *num = result.size();
    if(*num == 0)
    {
        return PPW_LOBBY_RESULT_SUCCESS;
    }

    if(orgNum < *num || userIds == NULL)
    {
        return PPW_LOBBY_RESULT_ERROR_PARAM;
    }

    DWCi_Np_CpuCopy8(DWCi_GetVectorBuffer(result), userIds, result.size() * sizeof(result[0]));
    return PPW_LOBBY_RESULT_SUCCESS;
}

/**
 * @brief ローカルにキャッシュしているユーザ定義プロフィールを参照します。
 * 
 * ローカルにキャッシュしているユーザ定義プロフィールを参照します。\n
 * ユーザ定義プロフィールを保存する領域を引数に渡してください。\n
 * userProfileにNULL、もしくは必要なサイズがuserProfileSizeを下回っていた場合はuserProfileSizeに必要なサイズが格納されます。
 * 
 * @param[in] userId 取得する相手のユーザID。::PPW_LOBBY_INVALID_USER_IDを指定すると自分のプロフィールが参照できます
 * @param[in, out] userProfile userProfileSizeの領域を確保した、ユーザ定義プロフィールを格納するポインタ。
 *                 必要なサイズを満たしている場合のみユーザ定義プロフィールが格納されます。
 * @param[in, out] userProfileSize 確保したuserProfileのサイズを指定してください。関数は実際のuserProfileのサイズを代入します。
 * 
 * @retval ::PPW_LOBBY_RESULT_SUCCESS 成功。userProfileにユーザ定義プロフィールが格納され、userProfileSizeにそのサイズが格納されます。
 *                                  ただしuserProfileSizeが0だった場合はuserProfileは変化しません。
 * @retval ::PPW_LOBBY_RESULT_ERROR_PARAM userProfileがNULLもしくはuserProfileSizeが必要サイズを下回っています。
 *                                      userProfileSize以上のメモリを確保して再試行してください。
 * @retval ::PPW_LOBBY_RESULT_ERROR_STATE ステートがREADYではありません。
 * @retval ::PPW_LOBBY_RESULT_ERROR_NODATA ユーザ定義プロフィールが見つかりません。
 * @retval ::PPW_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
PPW_LOBBY_RESULT PPW_LobbyGetProfile(s32 userId, u8* userProfile, u32* userProfileSize)
{
    RETURN_IF_NOTINITIALIZED(PPW_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(PPW_LOBBY_RESULT_ERROR_CONDITION);
    RETURN_IF_NOTREADY(PPW_LOBBY_RESULT_ERROR_STATE);

    if(userId == PPW_LOBBY_INVALID_USER_ID)
    {
        userId = s_lobby->GetPid();
    }

    PPW_LobbyInnerProfile* profile = s_lobby->GetProfileManager().FindProfile(userId);
    if(!profile)
    {
        return PPW_LOBBY_RESULT_ERROR_NODATA;
    }

    return profile->GetUserProfile(userProfile, userProfileSize) ? PPW_LOBBY_RESULT_SUCCESS : PPW_LOBBY_RESULT_ERROR_PARAM;
}

/**
 * @brief 自分のユーザ定義プロフィールを更新します。
 * 
 * 自分のユーザ定義プロフィールを更新します。更新したデータは自動的に他のプレイヤーに通知されます。\n
 * 実際に通知されるには若干の時間がかかります。
 * 
 * @param[in] userProfile セットするユーザ定義プロフィール。
 * @param[in] userProfileSize userProfileのサイズ。
 * @param[in] force FALSEを指定するとデータが前回から内容が変更されていない場合は情報を送信しません。\n
 *                  TRUEを指定すると変更されていなくても強制的に情報を送信します。
 * 
 * @retval ::PPW_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::PPW_LOBBY_RESULT_ERROR_PARAM 引数が不正。
 * @retval ::PPW_LOBBY_RESULT_ERROR_STATE ステートがREADYではありません。
 * @retval ::PPW_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
PPW_LOBBY_RESULT PPW_LobbyUpdateMyProfile(const u8* userProfile, u32 userProfileSize, BOOL force)
{
    RETURN_IF_NOTINITIALIZED(PPW_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(PPW_LOBBY_RESULT_ERROR_CONDITION);
    RETURN_IF_NOTREADY(PPW_LOBBY_RESULT_ERROR_STATE);
    
    PPW_LobbyInnerProfile& myProfile = s_lobby->GetProfileManager().GetMyProfile();

    myProfile.SetUserProfile(userProfile, userProfileSize, force);
    
    return PPW_LOBBY_RESULT_SUCCESS;
}

/**
 * @brief ミニゲームを募集します。
 * 
 * ミニゲームを募集します。\n
 * 関数が成功するとrecruitInfoのmatchMakingStringメンバにマッチメイク指標文字列が格納されますので\n
 * DWC_AddMatchKeyString関数のkeyStringに::PPW_LOBBY_MATCHMAKING_KEY、valueSrcにmatchMakingStringを指定し呼び出した後、\n
 * DWC_ConnectToAnybodyAsync関数のaddFilterに「::PPW_LOBBY_MATCHMAKING_KEY = 'matchMakingString'」となる文字列をセットし呼び出してください。\n
 * 本関数呼び出し後は自動的に一定間隔でrecruitInfoが他のプレイヤーに送信され、::PPW_LobbyRecruitCallback関数がコールバックされます。\n
 * 募集を行うにはロビーライブラリを::PPW_LobbyInitializeAsync関数で初期化されている必要があります。
 * ::PPW_LobbyInitializePidAsync関数で初期化されていた場合はAssertします。
 * 
 * @param[in, out] recruitInfo 他のプレイヤーに通知する募集要項です。募集するゲームの種類、最大人数、現在集まっている人数(初めは0を指定してください)、\n
 *                             その他通知したい任意の値をセットしたものを指定してください。\n
 *                             matchMakingStringは本関数内でセットされますので初期化する必要はありません。
 * 
 * @retval ::PPW_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::PPW_LOBBY_RESULT_ERROR_PARAM 引数が不正。
 * @retval ::PPW_LOBBY_RESULT_ERROR_STATE ステートがREADYではありません。
 * @retval ::PPW_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
PPW_LOBBY_RESULT PPW_LobbyStartRecruit(PPW_LobbyRecruitInfo* recruitInfo)
{
    RETURN_IF_NOTINITIALIZED(PPW_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(PPW_LOBBY_RESULT_ERROR_CONDITION);
    RETURN_IF_NOTREADY(PPW_LOBBY_RESULT_ERROR_STATE);
    
    DWC_ASSERTMSG(s_lobby->GetIsValidInitialize(), "PPW_LobbyStartRecruit: You must initialize by PPW_LobbyInitializeAsync() to recruit.");
    
    DWCi_String tempMatchMakingString;
    if(!s_lobby->StartRecruit(*recruitInfo, tempMatchMakingString))
    {
        // すでに募集中
        return PPW_LOBBY_RESULT_ERROR_STATE;
    }

    strncpy(recruitInfo->matchMakingString, tempMatchMakingString.c_str(), PPW_LOBBY_MAX_MATCHMAKING_STRING_LENGTH-1);
    
    // 募集状況送信タイマー起動
    s_lobby->GetTimerManager().AddTimer(PPW_LOBBY_TIMER_ID_RECRUIT, PPW_LOBBY_TIMER_INTERVAL_RECRUIT,
                                        PPW_LobbySendRecruitInfo, NULL, TRUE);
    
    return PPW_LOBBY_RESULT_SUCCESS;
}

/**
 * @brief ミニゲーム募集要項を更新します。
 * 
 * 一定間隔で他のプレイヤーに送信しているミニゲーム募集要項を変更します。\n
 * 例えば現在集まっている人数が変化したときなどに使用してください。
 * 
 * @param[in] recruitInfo 更新するデータ。
 * 
 * @retval ::PPW_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::PPW_LOBBY_RESULT_ERROR_PARAM 引数が不正。
 * @retval ::PPW_LOBBY_RESULT_ERROR_STATE ステートがREADYではないか、募集をしていません。
 * @retval ::PPW_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
PPW_LOBBY_RESULT PPW_LobbyUpdateRecruitInfo(const PPW_LobbyRecruitInfo* recruitInfo)
{
    RETURN_IF_NOTINITIALIZED(PPW_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(PPW_LOBBY_RESULT_ERROR_CONDITION);
    RETURN_IF_NOTREADY(PPW_LOBBY_RESULT_ERROR_STATE);
    
    if(!s_lobby->UpdateRecruitInfo(*recruitInfo))
    {
        return PPW_LOBBY_RESULT_ERROR_STATE;
    }
    return PPW_LOBBY_RESULT_SUCCESS;
}

/**
 * @brief ミニゲームの募集を終了します。
 * 
 * ミニゲームの募集を終了します。
 * 
 * @retval ::PPW_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::PPW_LOBBY_RESULT_ERROR_STATE ステートがREADYではないか、募集をしていません。
 * @retval ::PPW_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
PPW_LOBBY_RESULT PPW_LobbyStopRecruit()
{
    RETURN_IF_NOTINITIALIZED(PPW_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(PPW_LOBBY_RESULT_ERROR_CONDITION);
    RETURN_IF_NOTREADY(PPW_LOBBY_RESULT_ERROR_STATE);
    
    if(!s_lobby->StopRecruit())
    {
        // 募集していない
        return PPW_LOBBY_RESULT_ERROR_STATE;
    }
    
    // 募集終了を通知
    PPW_LobbySendMessage(s_lobby->GetPid(), PPW_LOBBY_CHANNEL_KIND_MAIN, PPW_LobbyMessage::FORMAT_BASE64, PPW_LobbyMessage::TARGET_SYSTEM,
                         PPW_LOBBY_MESSAGE_TYPE_RECRUIT_STOPPED, (u8*)&s_lobby->GetRecruitInfo(), sizeof(PPW_LobbyRecruitInfo));
    
    // 募集要項送信タイマーを停止
    s_lobby->GetTimerManager().RemoveTimer(PPW_LOBBY_TIMER_ID_RECRUIT);
    return PPW_LOBBY_RESULT_SUCCESS;
}

/**
 * @brief アンケートの答えを提出します。
 * 
 * アンケートの答えを提出します。同一のアンケート期間に何回でも提出できますが、前回提出した回答は無効になります。
 * 提出処理中に再度この関数を呼ぶと::PPW_LOBBY_RESULT_ERROR_STATEが返ります。
 * 
 * @param answerNo 回答番号[0-2]
 * 
 * @retval ::PPW_LOBBY_RESULT_SUCCESS 成功。
 * @retval ::PPW_LOBBY_RESULT_ERROR_PARAM 回答番号が[0-2]ではありません。
 * @retval ::PPW_LOBBY_RESULT_ERROR_STATE まだアンケート情報を取得していません。もしくは現在提出処理中です。
 * @retval ::PPW_LOBBY_RESULT_ERROR_CONDITION 致命的なエラーが発生しています。
 */
PPW_LOBBY_RESULT PPW_LobbySubmitQuestionnaire(s32 answerNo)
{
    RETURN_IF_NOTINITIALIZED(PPW_LOBBY_RESULT_ERROR_STATE);
    RETURN_IF_ERROR_STATE(PPW_LOBBY_RESULT_ERROR_CONDITION);
    
    if(answerNo<0 || answerNo>2)
    {
        return PPW_LOBBY_RESULT_ERROR_PARAM;
    }
    
    PPW_LobbyQuestionnaire questionnaire;
    if(!s_lobby->GetLobbyInnerInfoManager().GetQuestionnaire(&questionnaire)
      || s_lobby->GetSubmitQuestionnaireStarted())
    {
        // まだアンケート情報を取得していない、または提出中ならエラー
        return PPW_LOBBY_RESULT_ERROR_STATE;
    }
    
    // リクエスト作成準備
    const PPW_LobbyInnerProfile::UserProfile& userProfile = s_lobby->GetProfileManager().GetMyProfile().GetUserProfile();
    const WFLBY_USER_PROFILE* userProfileDetail = (const WFLBY_USER_PROFILE*)DWCi_GetVectorBuffer(userProfile.data);
    
    // リクエスト作成
    struct Request
    {
        EncCommonRequest commonRequest;
        s32 questionSerialNo;
        s32 questionNo;
        s32 answerNo;
        u32 trainerId;
        s32 sex;
        u16 country;
        u16 region;
    }
    request = { EncCommonRequest(),
                questionnaire.currentQuestionnaireRecord.questionSerialNo,
                questionnaire.currentQuestionnaireRecord.questionNo,
                answerNo,
                userProfileDetail->trainerid,
                userProfileDetail->sex,
                userProfileDetail->nation,
                userProfileDetail->area
              };
    
    DWCi_EncSession::StartSessionAsync(s_lobby->GetEncServer(), ENC_INIT_DATA, ENC_URL_SUBMIT_QUESTIONNAIRE, s_lobby->GetPid(),
                                       (u8*)&request, sizeof(request), ENC_RETRY_COUNT, SubmitQuestionnaireCompletedCallback, NULL);
    
    s_lobby->SetSubmitQuestionnaireStarted(TRUE);
    return PPW_LOBBY_RESULT_SUCCESS;
}

/**
 * @brief 発生している致命的なエラーの情報を取得します。
 * 
 * 発生している致命的なエラーの情報を取得します。
 * 
 * @retval ::PPW_LOBBY_ERROR_NONE エラー無し。
 * @retval ::PPW_LOBBY_ERROR_UNKNOWN 不明なエラー。
 * @retval ::PPW_LOBBY_ERROR_ALLOC メモリ確保に失敗。
 * @retval ::PPW_LOBBY_ERROR_SESSION 通信エラー。
 */
PPW_LOBBY_ERROR PPW_LobbyGetLastError()
{
    RETURN_IF_NOTINITIALIZED(PPW_LOBBY_ERROR_NONE);
    
    return s_lobby->GetError();
}

/**
 * @brief 発生したエラー(::PPW_LOBBY_ERROR)から表示すべきエラーコードを得ます。
 * 
 * ::PPW_LOBBY_ERRORから表示すべきエラーコードに変換します。
 * 
 * @param err 発生したエラー。
 * 
 * @retval 表示すべきエラーコード。
 */
s32 PPW_LobbyToErrorCode(PPW_LOBBY_ERROR err)
{
    if(err >= DWCi_LOBBY_ERROR_MAX)
    {
        // ポケモン独自コード
        return err + PPW_LOBBY_ERROR_CODE_BASE;
    }
    else
    {
        // 共通ロビーライブラリ用コード
        return DWCi_LobbyToErrorCode((DWCi_LOBBY_ERROR)err);
    }
}

// ??????
PPW_LOBBY_ERROR ov66_2237134(PPW_LOBBY_RESULT result)
{
    switch (result)
    {
    case PPW_LOBBY_RESULT_SUCCESS:
        return PPW_LOBBY_ERROR_NONE;
    case PPW_LOBBY_RESULT_ERROR_ALLOC:
        return PPW_LOBBY_ERROR_ALLOC;
    case PPW_LOBBY_RESULT_ERROR_SESSION:
        return PPW_LOBBY_ERROR_SESSION;
    default:
        return PPW_LOBBY_ERROR_UNKNOWN;
    }
}

/**
 * @brief FriendsMatch中に、指定したプレイヤーと接続しているかを調べます。
 * 
 * FriendsMatch中に、指定したプレイヤーと接続しているかを調べます。(デバッグ用)\n
 * FriendsMatchライブラリ非稼働中、もしくはロビーライブラリが初期化されていないときに本関数を呼び出した場合はFALSEを返します。\n
 * FriendsMatchライブラリ稼働中に自分のユーザIDを指定した場合はTRUEを返します。\n
 * ミニゲームを募集している人と本当に接続しているかを確認するために使用してください。\n
 * この関数を使用するにはロビーライブラリを::PPW_LobbyInitializeAsync関数で初期化されている必要があります。
 * ::PPW_LobbyInitializePidAsync関数で初期化されていた場合はAssertします。
 * Nitro以外では使用できません。
 * 
 * @param[in] dwcFriendMatchCtrl フレンドマッチ制御オブジェクト。
 * @param[in] userId 接続中であるかを判定するユーザID。
 * 
 * @retval TRUE 接続している。
 * @retval FALSE 接続していない。
 */
BOOL PPW_IsConnectedUserId(const DWCFriendsMatchControl* dwcFriendMatchCtrl, s32 userId)
{
    RETURN_IF_NOTINITIALIZED(FALSE);    // ロビーライブラリ非稼働
    
#ifndef _NITRO  // NITRO以外は適当にTRUEを返す
    DWC_ASSERTMSG(FALSE, "PPW_IsConnectedUserId is only for NITRO.");
    (void)dwcFriendMatchCtrl;
    (void)userId;
    return TRUE;
#else
    DWC_ASSERTMSG(s_lobby->GetIsValidInitialize() && s_lobby->GetPid() == dwcFriendMatchCtrl->profileID,
                  "PPW_IsConnectedUserId: You must initialize by PPW_LobbyInitializeAsync() to join mini game.");
    
    int currentHostNum = DWC_GetNumConnectionHost();
    if(currentHostNum == 0 || !dwcFriendMatchCtrl)
    {
        // FriendsMatch非稼働
        return FALSE;
    }
    
    if(userId == s_lobby->GetPid())
    {
        return TRUE;
    }
    
    // 接続済みpidリストに含まれているか調べる
    const int* sbPidList = dwcFriendMatchCtrl->matchcnt.sbPidList;
    const int* ret = std::find(sbPidList, sbPidList + currentHostNum, userId);
    
    return ret != sbPidList + currentHostNum;
#endif
}

/*
 * @brief デバッグ用に任意のEncInitDataを指定します。
 * 
 * デバッグ用に任意のEncInitDataを指定します。初期化前に呼んでください。
 * 
 * @param[in] initData ENC用のInitData。指定した文字列は静的に保持してください。
 */
void PPW_LobbySetEncInitData(const char* initData)
{
    ENC_INIT_DATA = initData;
}

//
// 内部公開関数定義
//

// メッセージを送信
PPW_LOBBY_RESULT PPW_LobbySendMessage(s32 userId, PPW_LOBBY_CHANNEL_KIND channelKind, PPW_LobbyMessage::FORMAT format,
                                             PPW_LobbyMessage::TARGET target, s32 type, const u8* data, u32 dataSize)
{
    if(userId == PPW_LOBBY_INVALID_USER_ID && channelKind == PPW_LOBBY_CHANNEL_KIND_INVALID)
    {
        return PPW_LOBBY_RESULT_ERROR_PARAM;
    }
    
    DWCi_String sendString;
    if(!PPW_LobbyMessage::Build(format, target, type, (const char*)data, dataSize, sendString))
    {
        return PPW_LOBBY_RESULT_ERROR_PARAM;
    }

    if(channelKind == PPW_LOBBY_CHANNEL_KIND_INVALID)
    {
        return ToPpwResult(DWCi_LobbySendUserMessage(userId, sendString.c_str()));
    }
    else
    {
        DWCi_String channelName = s_lobby->GetChannelManager().GetChannelName(channelKind);
        if(channelName == "")
        {
            return PPW_LOBBY_RESULT_ERROR_CHANNEL;
        }
        return ToPpwResult(DWCi_LobbySendChannelMessage(channelName.c_str(), sendString.c_str()));
    }
}

// チャンネルデータを送信
PPW_LOBBY_RESULT PPW_LobbySetChannelDataImpl(s32 userId, PPW_LOBBY_CHANNEL_KIND channelKind, const char* key, const u8* data, u32 dataSize)
{
    DWCi_String channelName = s_lobby->GetChannelManager().GetChannelName(channelKind);
    if(channelName == "")
    {
        return PPW_LOBBY_RESULT_ERROR_CHANNEL;
    }

    if(dataSize > PPW_LOBBY_MAX_BINARY_SIZE)
    {
        return PPW_LOBBY_RESULT_ERROR_PARAM;
    }

    std::vector<char, DWCi_Allocator<char> > tempBuf(PPW_LobbyToBase64Size(dataSize) + 1);
    int writtenSize = DWC_Base64Encode((char*)data, dataSize, DWCi_GetVectorBuffer(tempBuf), tempBuf.size());
    DWC_ASSERTMSG(writtenSize != -1, "PPW_LobbySetChannelDataImpl: Internal error.");
    tempBuf[(std::size_t)writtenSize] = '\0';

    return ToPpwResult(DWCi_LobbySetChannelData(channelName.c_str(), userId, key, DWCi_GetVectorBuffer(tempBuf)));
}

//
// ローカル関数定義
//

// 部屋設定ダウンロード開始
static void StartDownload()
{
    // リクエスト作成準備
    const PPW_LobbyInnerProfile::UserProfile& userProfile = s_lobby->GetProfileManager().GetMyProfile().GetUserProfile();
    const WFLBY_USER_PROFILE* userProfileDetail = (const WFLBY_USER_PROFILE*)DWCi_GetVectorBuffer(userProfile.data);
    
    struct Request
    {
        EncCommonRequest commonRequest;
        WFLBY_USER_PROFILE profile;
    }
    request;
    request.profile = *userProfileDetail;
    
    // CheckProfileCompletedCallback関数内でスケジュール受信に移行
    DWCi_EncSession::StartSessionAsync(s_lobby->GetEncServer(), ENC_INIT_DATA, ENC_URL_CHECK_PROFILE, s_lobby->GetPid(),
                                  (u8*)&request, sizeof(request), ENC_RETRY_COUNT, CheckProfileCompletedCallback, NULL);
}

// kind!uniqueName の形式のチャンネル名を作る
static DWCi_String PPW_LobbyCreateBaseChannelName(PPW_LOBBY_CHANNEL_KIND nameKind, const DWCi_String& uniqueName)
{
    DWC_ASSERTMSG(nameKind < PPW_LOBBY_CHANNEL_KIND_INVALID, "PPW_LobbyCreateBaseChannelName: Invalid channel kind.");
    return DWCi_String(PPW_LOBBY_CHANNEL_NAME[nameKind]) + DWCi_LOBBY_CHANNEL_NAME_SEPARATOR + uniqueName;
}

static DWCi_String PPW_LobbyGenerateUniqueChannelName()
{
    return DWCi_SPrintf<DWCi_String>("C%d_%u", s_lobby->GetPid(), (unsigned long)current_time() % 1000);
}

// メインチャンネルに入る。
static PPW_LOBBY_RESULT PPW_LobbyEnterMainChannel()
{
    DWCi_LobbyChannelCallbacks cc;
    cc.lobbyPlayerConnectedCallback = LobbyPlayerConnectedCallback;
    cc.lobbyPlayerDisconnectedCallback = LobbyPlayerDisconnectedCallback;
    cc.lobbyChannelMassageReceivedCallback = LobbyChannelMassageReceivedCallback;
    cc.lobbyBroadcastDataCallback = LobbyBroadcastDataCallback;
    cc.lobbyTopicChangedCallback = LobbyTopicChangedCallback;
    cc.param = (void*)PPW_LOBBY_CHANNEL_KIND_MAIN;
    
    DWCi_String channelName = PPW_LobbyCreateBaseChannelName(PPW_LOBBY_CHANNEL_KIND_MAIN, s_lobby->GetUniqueChannelName());
    PPW_LOBBY_RESULT result = ToPpwResult(DWCi_LobbyJoinChannelLimitAsync(channelName.c_str(), NULL, PPW_LOBBY_MAX_PLAYER_NUM_MAIN,
                                                                          &cc, LobbyJoinMainChannelCallback, (void*)PPW_LOBBY_CHANNEL_KIND_MAIN));
    if(result != PPW_LOBBY_RESULT_SUCCESS)
    {
        return result;
    }
    s_lobby->SetMainChannel(PPW_Channel(PPW_LOBBY_CHANNEL_KIND_MAIN, channelName));
    return result;
}

// チャンネルデータ処理
static void PPW_ChannelDataHandler(BOOL success, BOOL broadcast, const char* channelName, s32 userId,
                                        const char* key, const char* value, void* param)
{
    // チャンネルを確認
    PPW_LOBBY_CHANNEL_KIND kind = s_lobby->GetChannelManager().GetChannelKind(channelName);
    if(kind == PPW_LOBBY_CHANNEL_KIND_INVALID)
    {
        DWC_Printf(DWC_REPORTFLAG_WARNING, "PPW_ChannelDataHandler: No such channel.");
        return;
    }
    
    // Base64デコード
    std::vector<u8, DWCi_Allocator<u8> > tempBuf;
    if(!DWCi_Base64Decode(value, tempBuf))
    {
        DWC_Printf(DWC_REPORTFLAG_WARNING, "PPW_ChannelDataHandler: ignored invalid data.\n");
        return;
    }
    
    // サイズ0は無効
    if(tempBuf.empty())
    {
        DWC_Printf(DWC_REPORTFLAG_WARNING, "PPW_ChannelDataHandler: ignored empty data.\n");
        return;
    }
    
    // ライブラリ側サポートのユーザ定義プロフィールを処理
    if(PPW_SystemChannelDataHandler(success, broadcast, kind, userId, key, DWCi_GetVectorBuffer(tempBuf), tempBuf.size(), param))
    {
        return;
    }
    
    // チャンネル自体のデータ以外の場合、アクティベートしていないプレイヤーからのメッセージは無視
    if(userId != PPW_LOBBY_INVALID_USER_ID && !s_lobby->GetProfileManager().IsActivatedProfile(userId))
    {
        DWC_Printf(DWC_REPORTFLAG_INFO, "PPW_ChannelDataHandler: Ignored not activated user's data.\n");
        return;
    }
    
    // ユーザ側で処理
    s_lobby->GetCallbacks().lobbyGetChannelDataCallback(success, broadcast, kind, userId, key, DWCi_GetVectorBuffer(tempBuf), tempBuf.size(), param);
}

// システム定義のチャンネルデータ処理
// ライブラリ内で処理したらTRUEを返す
static BOOL PPW_SystemChannelDataHandler(BOOL success, BOOL broadcast, PPW_LOBBY_CHANNEL_KIND channelKind, s32 userId,
                                        const char* key, const u8* data, u32 dataSize, void* param)
{
    // ここで処理しているのはいずれもメインチャンネル
    if(channelKind != PPW_LOBBY_CHANNEL_KIND_MAIN)
    {
        return FALSE;
    }
    
    if(strcmp(key, PPW_LOBBY_CHANNEL_KEY_USER) == 0)
    {
        //
        // ユーザ定義プロフィールが更新された
        //
        if(userId == s_lobby->GetPid())
        {
            // 自分のデータは無視
            return TRUE;
        }
        
        PPW_LobbyInnerProfile* profile = s_lobby->GetProfileManager().FindProfile(userId);
        if(!profile)
        {
            DWC_Printf(DWC_REPORTFLAG_ERROR, "PPW_SystemChannelDataHandler: profile is NULL. userId: %d\n", userId);
            return TRUE;
        }
    
        profile->SetUserProfile(data, (u32)dataSize);
        
        // 接続完了しているときのみコールバックを呼ぶ
        if(profile->IsActivated())
        {
            s_lobby->GetCallbacks().lobbyUserProfileUpdatedCallback(userId, data, dataSize);
        }
        return TRUE;
    }
    else if(strcmp(key, PPW_LOBBY_CHANNEL_KEY_SYSTEM) == 0)
    {
        //
        // システム定義プロフィールが更新された
        //
        if(userId == s_lobby->GetPid())
        {
            // 自分のデータは無視
            DWC_Printf(DWC_REPORTFLAG_INFO, "PPW_SystemChannelDataHandler: Ignored my data. userId: %d\n", userId);
            return TRUE;
        }
        
        if(dataSize != sizeof(PPW_LobbyInnerProfile::SystemProfile))   // TODO: データの出し入れをシリアライズ関数とデシリアライズ関数にする
        {
            DWC_Printf(DWC_REPORTFLAG_ERROR, "PPW_SystemChannelDataHandler: Invalid system data size. size: %d\n", sizeof(PPW_LobbyInnerProfile::SystemProfile));
            // 不正なデータは無視する
            return TRUE;
        }
        PPW_LobbyInnerProfile* profile = s_lobby->GetProfileManager().FindProfile(userId);
        if(!profile)
        {
            DWC_Printf(DWC_REPORTFLAG_ERROR, "PPW_SystemChannelDataHandler: profile is NULL. userId: %d\n", userId);
            return TRUE;
        }
        profile->SetSystemProfile(*(PPW_LobbyInnerProfile::SystemProfile*)data);
        
        // 接続完了しているときと変更があったときのみコールバックを呼ぶ
        if(profile->IsActivated() && profile->IsSystemProfileUpdated())
        {
            const PPW_LobbyInnerProfile::SystemProfile& systemProfile = profile->GetSystemProfileConfirm();  // 整合性を保つため保存したデータから取ってくる
            PPW_LobbySystemProfile publicSystemProfile = systemProfile.ToPublicSystemProfile();
            s_lobby->GetCallbacks().lobbySystemProfileUpdatedCallback(userId, &publicSystemProfile);
        }
        return TRUE;
    }
    else if(strcmp(key, PPW_LOBBY_CHANNEL_KEY_CHANNEL_TIME) == 0)
    {
        //
        // チャンネルシステムデータが更新された(LobbyCheckMainChannelSystemProfileCallback()でもセットしてるのでこちらはなくてもよいが、処理の速度が上がる)
        //
        if(dataSize != sizeof(PPW_LobbyInnerInfoManager::TimeData))
        {
            // 不正なデータは無視する
            return TRUE;
        }
        PPW_LobbyInnerInfoManager::TimeData* systemProfile = (PPW_LobbyInnerInfoManager::TimeData*)data;
        DWC_Printf(DWC_REPORTFLAG_INFO, "PPW_SystemChannelDataHandler: openedTime is %lld\n", systemProfile->openedTime);
        s_lobby->GetLobbyInnerInfoManager().SetTimeData(*systemProfile);
        return TRUE;
    }
    else if(strcmp(key, PPW_LOBBY_CHANNEL_KEY_CHANNEL_LOBBY) == 0)
    {
        //
        // ロビーデータが更新された(LobbyCheckLobbyInfoCallback()でもセットしてるのでこちらはなくてもよいが、処理の速度が上がる)
        //
        if(!s_lobby->GetLobbyInnerInfoManager().SetLobbyInfo(data, dataSize))
        {
            // 不正なデータは無視する
            return TRUE;
        }
        s_lobby->GetLobbyInnerInfoManager().Shared();
        return TRUE;
    }
    
    (void)success;
    (void)broadcast;
    (void)param;
    
    return FALSE;
}

// システム定義のメッセージ処理。必ずTRUEを返す
static BOOL PPW_SystemMessageHandler(const PPW_LobbyMessage::Result& result, s32 userId, PPW_LOBBY_CHANNEL_KIND channelKind, u8* data, u32 dataSize)
{
    // ここで処理しているのはいずれもメインチャンネル
    if(channelKind != PPW_LOBBY_CHANNEL_KIND_MAIN)
    {
        return TRUE;
    }
    
    switch(result.type)
    {
    case PPW_LOBBY_MESSAGE_TYPE_RECRUIT:
        // 募集はループバックさせない
        if(s_lobby->GetPid() == userId)
        {
            break;
        }
        
        // アクティベートしていないユーザからのメッセージは無視
        if(!s_lobby->GetProfileManager().IsActivatedProfile(userId))
        {
            DWC_Printf( DWC_REPORTFLAG_INFO, "PPW_SystemMessageHandler: Ignored not activated user's message.\n" );
            break;
        }
        
        if(result.format == PPW_LobbyMessage::FORMAT_BASE64 && dataSize == sizeof(PPW_LobbyRecruitInfo))
        {
            s_lobby->GetCallbacks().lobbyRecruitCallback(userId, (PPW_LobbyRecruitInfo*)data);
        }
        break;
    case PPW_LOBBY_MESSAGE_TYPE_RECRUIT_STOPPED:
        // 募集終了もループバックさせない
        if(s_lobby->GetPid() == userId)
        {
            break;
        }
        
        // アクティベートしていないユーザからのメッセージは無視
        if(!s_lobby->GetProfileManager().IsActivatedProfile(userId))
        {
            DWC_Printf( DWC_REPORTFLAG_INFO, "PPW_SystemMessageHandler: Ignored not activated user's message.\n" );
            break;
        }
        
        if(result.format == PPW_LobbyMessage::FORMAT_BASE64 && dataSize == sizeof(PPW_LobbyRecruitInfo))
        {
            s_lobby->GetCallbacks().lobbyRecruitStoppedCallback(userId, (PPW_LobbyRecruitInfo*)data);
        }
        break;
    case PPW_LOBBY_MESSAGE_TYPE_LOCK:
        if(result.format == PPW_LobbyMessage::FORMAT_BASE64 && dataSize == sizeof(PPW_LobbyDummyData))
        {
            s_lobby->GetLobbyInnerInfoManager().Lock();
        }
        break;
    default:
        // 不明なタイプは無視
        DWC_Printf( DWC_REPORTFLAG_INFO, "PPW_SystemMessageHandler: Ignored unknown type.\n" );
        break;
    }
    return TRUE;
}

// メインチャンネルの準備ができているかを確認
BOOL PPW_IsMainChannelReady()
{
    // メインチャンネルに入室済みか
    if(!DWCi_LobbyInChannel(s_lobby->GetMainChannel().GetName().c_str()))
    {
        return FALSE;
    }
    
    // 参加者を列挙したか
    if(!s_lobby->GetMainChannel().GetUserListIsEnumerated())
    {
        return FALSE;
    }
    
    // チャンネルモードがセットされているか
    if(!s_lobby->GetMainChannel().GetModeIsSet())
    {
        return FALSE;
    }
    
    // チャンネル管理データがセットされているか
    if(!s_lobby->GetLobbyInnerInfoManager().GetTimeDataIsSet())
    {
        return FALSE;
    }
    
    // スケジュールがセットされているか
    if(!s_lobby->GetLobbyInnerInfoManager().IsShared())
    {
        return FALSE;
    }
    
    // 参加者全てのユーザ定義プロフィールを受け取ったか
    if(!s_lobby->GetProfileManager().CheckAllProfilesRecved())
    {
        return FALSE;
    }
    return TRUE;
}

//
// タイマーコールバック
//

// 自分のユーザ定義プロフィールを送信
static BOOL PPW_LobbyBroadcastMyUserProfile(void* param)
{
    PPW_LobbyInnerProfile& myProfile = s_lobby->GetProfileManager().GetMyProfile();
    
    if(!myProfile.IsUserProfileUpdated())
    {
        // 更新されていなければ送らない
        return TRUE;
    }
    
    const PPW_LobbyInnerProfile::UserProfile& userProfile = myProfile.GetUserProfileConfirm();
    
    PPW_LobbySetChannelDataImpl(s_lobby->GetPid(), PPW_LOBBY_CHANNEL_KIND_MAIN, PPW_LOBBY_CHANNEL_KEY_USER, (u8*)DWCi_GetVectorBuffer(userProfile.data),
                                userProfile.data.size());
    
    (void)param;
    return TRUE;
}

// 自分のシステム定義プロフィールを送信
static BOOL PPW_LobbyBroadcastMySystemProfile(void* param)
{
    PPW_LobbyInnerProfile& myProfile = s_lobby->GetProfileManager().GetMyProfile();
    
    if(!myProfile.IsSystemProfileUpdated())
    {
        // 更新されていなければ送らない
        return TRUE;
    }
    
    const PPW_LobbyInnerProfile::SystemProfile& systemProfile = myProfile.GetSystemProfileConfirm();
    
    PPW_LobbySetChannelDataImpl(s_lobby->GetPid(), PPW_LOBBY_CHANNEL_KIND_MAIN, PPW_LOBBY_CHANNEL_KEY_SYSTEM, (u8*)&systemProfile, sizeof(systemProfile));
    
    (void)param;
    return TRUE;
}

// 募集情報を送信
static BOOL PPW_LobbySendRecruitInfo(void* param)
{
    // 念のため募集しているか確認
    if(!s_lobby->IsRecruiting())
    {
        DWC_ASSERTMSG(FALSE, "PPW_LobbySendRecruitInfo: Not recruiting?");
        return FALSE;   // FALSEを返すとこのタイマーを削除できる
    }
    
    PPW_LobbySendMessage(s_lobby->GetPid(), PPW_LOBBY_CHANNEL_KIND_MAIN, PPW_LobbyMessage::FORMAT_BASE64, PPW_LobbyMessage::TARGET_SYSTEM,
                         PPW_LOBBY_MESSAGE_TYPE_RECRUIT, (u8*)&s_lobby->GetRecruitInfo(), sizeof(PPW_LobbyRecruitInfo));
    
    (void)param;
    return TRUE;
}

// メインチャンネルモードをチェック
static BOOL PPW_LobbyCheckMainChannelMode(void* param)
{
    DWCi_LobbyGetChannelModeAsync(s_lobby->GetMainChannel().GetName().c_str(), LobbyCheckMainChannelModeCallback, NULL);
    
    (void)param;
    return TRUE;
}

// メインチャンネルシステムデータチェック
static BOOL PPW_LobbyCheckMainChannelSystemProfile(void* param)
{
    // すでにセットされていたら中断する
    if(s_lobby->GetLobbyInnerInfoManager().GetTimeDataIsSet())
    {
        // FALSEを返すとタイマー削除
        return FALSE;
    }
    DWCi_LobbyGetChannelDataAsync(s_lobby->GetMainChannel().GetName().c_str(), PPW_LOBBY_INVALID_USER_ID,
                                  PPW_LOBBY_CHANNEL_KEY_CHANNEL_TIME, LobbyCheckMainChannelSystemProfileCallback, NULL);
    
    (void)param;
    return TRUE;
}


// ロビーデータチェック
static BOOL PPW_LobbyCheckLobbyInfo(void* param)
{
    // すでにセットされていたら中断する
    if(s_lobby->GetLobbyInnerInfoManager().IsShared())
    {
        // FALSEを返すとタイマー削除
        return FALSE;
    }
    DWCi_LobbyGetChannelDataAsync(s_lobby->GetMainChannel().GetName().c_str(), PPW_LOBBY_INVALID_USER_ID,
                             PPW_LOBBY_CHANNEL_KEY_CHANNEL_LOBBY, LobbyCheckLobbyInfoCallback, NULL);
    
    (void)param;
    return TRUE;
}

static PPW_LOBBY_STATS_RESULT EncResultCodeToStatsResult(u32 code)
{
    switch(code)
    {
    case ENC_SERVER_RESPONSE_CODE_OK:
        return PPW_LOBBY_STATS_RESULT_SUCCESS;
    case ENC_SERVER_RESPONSE_CODE_INVALID_PARAM:
        return PPW_LOBBY_STATS_RESULT_INVALID_PARAM;
    case ENC_SERVER_RESPONSE_CODE_INTERNAL_ERROR:
    case ENC_SERVER_RESPONSE_CODE_DATABASE_ERROR:
    case ENC_SERVER_RESPONSE_CODE_MAINTENANCE:
    case ENC_SERVER_RESPONSE_CODE_UNKNOWN_ERROR:
        return PPW_LOBBY_STATS_RESULT_SERVER_ERROR;
    default:
        DWC_ASSERTMSG(FALSE, "EncResultCodeToStatsResult: unknown code.");
        return PPW_LOBBY_STATS_RESULT_SERVER_ERROR;
    }
}

// Encレスポンスチェック
static PPW_LOBBY_STATS_RESULT CheckResponseHeader(BOOL success,
                                                  const u8* data,
                                                  u32 size,
                                                  const u8** responseData,  // out
                                                  u32* responseSize)        // out
{
    PPW_LOBBY_STATS_RESULT result = PPW_LOBBY_STATS_RESULT_SUCCESS;
    *responseData = NULL;
    *responseSize = 0;
    
    if(!success)
    {
        return PPW_LOBBY_STATS_RESULT_SERVER_ERROR;
    }

    // 結果コードが含まれているかチェック
    if(size < sizeof(u32))
    {
        return PPW_LOBBY_STATS_RESULT_SERVER_ERROR;
    }

    // 先頭の結果コードを確認
    u32 code = (u32)*data;
    result = EncResultCodeToStatsResult(code);
    if(result == PPW_LOBBY_STATS_RESULT_SERVER_ERROR)
    {
        DWC_Printf(DWC_REPORTFLAG_ERROR, "CheckResponseHeader: server returned error: %u\n", code);
        return result;
    }
    else if(result == PPW_LOBBY_STATS_RESULT_INVALID_PARAM)
    {
        DWC_Printf(DWC_REPORTFLAG_ERROR, "CheckResponseHeader: You specified invalid parameter.\n");
        return result;
    }

    *responseData = data + sizeof(u32);
    *responseSize = size - sizeof(u32);
    
    return result;
}

// ENC完了コールバック
// 不正チェック完了コールバック
static void CheckProfileCompletedCallback(BOOL success, const u8* data, u32 size, void* param)
{
    RETURN_IF_NOTINITIALIZED((void)0);
    if(s_lobby->GetState() == PPW_LOBBY_STATE_CLOSING)
    {
        return;
    }
    
    const u8* responseData = NULL;
    u32 responseSize = 0;
    PPW_LOBBY_STATS_RESULT result = CheckResponseHeader(success, data, size, &responseData, &responseSize);

    if(result == PPW_LOBBY_STATS_RESULT_SUCCESS)
    {
        // コールバックを呼ぶ前にデータをチェック
        if(responseSize != sizeof(WFLBY_USER_PROFILE))
        {
            // ダウンロードしたデータがおかしい
            DWC_Printf(DWC_REPORTFLAG_ERROR, "CheckProfileCompletedCallback: returned profile is invalid.\n");
            result = PPW_LOBBY_STATS_RESULT_SERVER_ERROR;
        }
    }
    
    if(result == PPW_LOBBY_STATS_RESULT_SUCCESS)
    {
        // 置換されたユーザ定義プロフィールセット
        PPW_LobbyUpdateMyProfile(responseData, responseSize, FALSE);
    }
    
    // コールバック呼び出し
    s_lobby->GetCallbacks().lobbyCheckProfileCallback(result, responseData, responseSize);
    
    if(result != PPW_LOBBY_STATS_RESULT_SUCCESS)
    {
        // 失敗時(不正チェックに引っかかったときやサーバエラーのとき)はエラー状態にして終了
        s_lobby->SetError(PPW_LOBBY_ERROR_STATS_CHECKPROFILE);
        return;
    }
    
    // リクエスト作成
    EncCommonRequest request;
    
    // スケジュールダウンロード開始
    DWCi_EncSession::StartSessionAsync(s_lobby->GetEncServer(), ENC_INIT_DATA, ENC_URL_GET_SCHEDULE, s_lobby->GetPid(),
                                       (u8*)&request, sizeof(request), ENC_RETRY_COUNT, GetScheduleCompletedCallback, NULL, TRUE);
    (void)param;
}

// スケジュールダウンロード完了コールバック
static void GetScheduleCompletedCallback(BOOL success, const u8* data, u32 size, void* param)
{
    RETURN_IF_NOTINITIALIZED((void)0);
    if(s_lobby->GetState() == PPW_LOBBY_STATE_CLOSING)
    {
        return;
    }
    
    const u8* responseData = NULL;
    u32 responseSize = 0;
    PPW_LOBBY_STATS_RESULT result = CheckResponseHeader(success, data, size, &responseData, &responseSize);

    if(result == PPW_LOBBY_STATS_RESULT_SUCCESS)
    {
        // コールバックを呼ぶ前にデータをチェック
        if(!s_lobby->GetLobbyInnerInfoManager().CheckLobbyInfo(responseData, responseSize))
        {
            // ダウンロードしたデータがおかしい
            DWC_Printf(DWC_REPORTFLAG_ERROR, "GetScheduleCompletedCallback: room setting data is invalid.\n");
            result = PPW_LOBBY_STATS_RESULT_SERVER_ERROR;
        }
    }
    
    // コールバック呼び出し
    BOOL useDownloadedData = s_lobby->GetCallbacks().lobbyNotifyScheduleCallback(result, (PPW_LobbySchedule*)responseData);
    
    if(result != PPW_LOBBY_STATS_RESULT_SUCCESS)
    {
        s_lobby->SetError(PPW_LOBBY_ERROR_STATS_SCHEDULE);
        return;
    }
    
    if(useDownloadedData)
    {
        // スケジュールセット。一度CheckLobbyInfoしているので必ず成功する。
        s_lobby->GetLobbyInnerInfoManager().SetLobbyInfo(responseData, responseSize);
    }
    
    // リクエスト作成準備
    const PPW_LobbyInnerProfile::UserProfile& userProfile = s_lobby->GetProfileManager().GetMyProfile().GetUserProfile();
    const WFLBY_USER_PROFILE* userProfileDetail = (const WFLBY_USER_PROFILE*)DWCi_GetVectorBuffer(userProfile.data);
    
    // リクエスト作成
    struct Request
    {
        EncCommonRequest commonRequest;
        u32 trainerId;
    }
    request =
    {
        EncCommonRequest(),
        userProfileDetail->trainerid
    };
    
    // VIP情報ダウンロード開始
    DWCi_EncSession::StartSessionAsync(s_lobby->GetEncServer(), ENC_INIT_DATA, ENC_URL_GET_VIP, s_lobby->GetPid(),
                                       (u8*)&request, sizeof(request), ENC_RETRY_COUNT, GetVipCompletedCallback, NULL, TRUE);
    (void)param;
}

// VIP情報ダウンロード完了コールバック
static void GetVipCompletedCallback(BOOL success, const u8* data, u32 size, void* param)
{
    RETURN_IF_NOTINITIALIZED((void)0);
    if(s_lobby->GetState() == PPW_LOBBY_STATE_CLOSING)
    {
        return;
    }
    
    const u8* responseData = NULL;
    u32 responseSize = 0;
    PPW_LOBBY_STATS_RESULT result = CheckResponseHeader(success, data, size, &responseData, &responseSize);

    if(!s_lobby->GetLobbyInnerInfoManager().CheckVipRecord(responseData, responseSize))
    {
        // ダウンロードしたデータがおかしい
        DWC_Printf(DWC_REPORTFLAG_ERROR, "GetVipCompletedCallback: vip data is invalid.\n");
        result = PPW_LOBBY_STATS_RESULT_SERVER_ERROR;
    }
    
    if(result != PPW_LOBBY_STATS_RESULT_SUCCESS)
    {
        s_lobby->SetError(PPW_LOBBY_ERROR_STATS_VIP);
        return;
    }
    
    // VIP情報セット
    s_lobby->GetLobbyInnerInfoManager().SetVipRecord(responseData, responseSize);
    
    // コールバック
    s_lobby->GetCallbacks().lobbyGetVipCallback(result, (PPW_LobbyVipRecord*)responseData, responseSize / sizeof(PPW_LobbyVipRecord));
    
    // リクエスト作成
    EncCommonRequest request;
    
    // アンケート情報ダウンロード開始
    DWCi_EncSession::StartSessionAsync(s_lobby->GetEncServer(), ENC_INIT_DATA, ENC_URL_GET_QUESTIONNAIRE, s_lobby->GetPid(),
                                       (u8*)&request, sizeof(request), ENC_RETRY_COUNT, GetQuestionnaireCompletedCallback, NULL, TRUE);
    (void)param;
}

// アンケート情報ダウンロード完了コールバック
static void GetQuestionnaireCompletedCallback(BOOL success, const u8* data, u32 size, void* param)
{
    RETURN_IF_NOTINITIALIZED((void)0);
    if(s_lobby->GetState() == PPW_LOBBY_STATE_CLOSING)
    {
        return;
    }
    
    const u8* responseData = NULL;
    u32 responseSize = 0;
    PPW_LOBBY_STATS_RESULT result = CheckResponseHeader(success, data, size, &responseData, &responseSize);

    if(responseSize != sizeof(PPW_LobbyQuestionnaire))
    {
        // ダウンロードしたデータがおかしい
        DWC_Printf(DWC_REPORTFLAG_ERROR, "GetQuestionnaireCompletedCallback: questionnaire data is invalid.\n");
        result = PPW_LOBBY_STATS_RESULT_SERVER_ERROR;
    }
    
    if(result != PPW_LOBBY_STATS_RESULT_SUCCESS)
    {
        s_lobby->SetError(PPW_LOBBY_ERROR_STATS_QUESTIONNAIRE);
        return;
    }
    
    // アンケート情報保存
    s_lobby->GetLobbyInnerInfoManager().SetQuestionnaire((PPW_LobbyQuestionnaire*)responseData);
    
    // コールバック
    s_lobby->GetCallbacks().lobbyGetQuestionnaireCallback(result, (PPW_LobbyQuestionnaire*)responseData);
    
    s_lobby->GetLobbyInnerInfoManager().Downloaded();
    (void)param;
}

// アンケート提出完了コールバック
static void SubmitQuestionnaireCompletedCallback(BOOL success, const u8* data, u32 size, void* param)
{
    RETURN_IF_NOTINITIALIZED((void)0);
    if(s_lobby->GetState() == PPW_LOBBY_STATE_CLOSING)
    {
        return;
    }
    
    s_lobby->SetSubmitQuestionnaireStarted(FALSE);
    s_lobby->GetCallbacks().lobbySubmitQuestionnaireCallback(success ? PPW_LOBBY_STATS_RESULT_SUCCESS : PPW_LOBBY_STATS_RESULT_SERVER_ERROR);
    (void)data;
    (void)size;
    (void)param;
}


//
// 下位ライブラリからのコールバック定義
//

// メインチャンネル入室コールバック
static void LobbyJoinMainChannelCallback(BOOL success, DWCi_LOBBY_ENTER_RESULT result, const char* channelName, void* param)
{
    // VS2005だと一度(u32)にキャストする必要がある
    PPW_LOBBY_CHANNEL_KIND kind = (PPW_LOBBY_CHANNEL_KIND)(u32)param;
    if(kind == PPW_LOBBY_CHANNEL_KIND_INVALID)
    {
        DWC_ASSERTMSG(FALSE, "LobbyJoinChannelCallback: Internal error. No such channel.");
        return;
    }
    
    if(success)
    {
        s_lobby->ClearMainChannelRetryCount();
        
        // 自分のシステムデータ(入室時刻など)を初期化する
        s_lobby->GetProfileManager().GetMyProfile().SetSystemProfile(PPW_LobbyInnerProfile::SystemProfile());
        
        // メンバーの確認をする
        DWCi_LobbyEnumUsersAsync(channelName, LobbyEnumUsersCallback, (void*)kind);
        
        // 自動データ送信用タイマーセット
        s_lobby->GetTimerManager().AddTimer(PPW_LOBBY_TIMER_ID_SYSTEM_CHANNEL_DATA, PPW_LOBBY_TIMER_INTERVAL_SYSTEM_CHANNEL_DATA,
                                            PPW_LobbyBroadcastMySystemProfile, NULL, TRUE);
        s_lobby->GetTimerManager().AddTimer(PPW_LOBBY_TIMER_ID_USER_CHANNEL_DATA, PPW_LOBBY_TIMER_INTERVAL_USER_CHANNEL_DATA,
                                            PPW_LobbyBroadcastMyUserProfile, NULL, TRUE);
        
        // チャンネル管理データ確認用タイマーセット
        s_lobby->GetTimerManager().AddTimer(PPW_LOBBY_TIMER_ID_CHECK_MAINCHANNEL_SYSTEMDATA, PPW_LOBBY_TIMER_INTERVAL_CHECK_MAINCHANNEL_SYSTEMDATA,
                                            PPW_LobbyCheckMainChannelSystemProfile, NULL, TRUE);
        
        // チャンネルモード確認用タイマーセット
        s_lobby->GetTimerManager().AddTimer(PPW_LOBBY_TIMER_ID_CHECK_MAINCHANNEL_MODE, PPW_LOBBY_TIMER_INTERVAL_CHECK_MAINCHANNEL_MODE,
                                            PPW_LobbyCheckMainChannelMode, NULL, TRUE);
        
        // ロビーデータ確認用タイマーセット
        s_lobby->GetTimerManager().AddTimer(PPW_LOBBY_TIMER_ID_CHECK_LOBBY_INFO, PPW_LOBBY_TIMER_INTERVAL_CHECK_LOBBY_INFO,
                                          PPW_LobbyCheckLobbyInfo, NULL, TRUE);
        
        // 自分に対するユーザ接続完了コールバックを呼ぶ
        const PPW_LobbyInnerProfile& myProfile = s_lobby->GetProfileManager().GetMyProfile();
        const PPW_LobbyInnerProfile::SystemProfile& systemProfile = myProfile.GetSystemProfile();
        const PPW_LobbyInnerProfile::UserProfile& userProfile = myProfile.GetUserProfile();
        PPW_LobbySystemProfile publicSystemProfile = systemProfile.ToPublicSystemProfile();
        s_lobby->GetCallbacks().lobbyPlayerConnectedCallback(myProfile.GetPid(), &publicSystemProfile, DWCi_GetVectorBuffer(userProfile.data),
                                                             userProfile.data.size());
    }
    else
    {
        s_lobby->GetMainChannel().Clear();
        
        if(s_lobby->IncrementMainChannelRetryCount())
        {
            // 人数がいっぱいだったかもしれないのでリトライする。
            // チャンネルサーチからやり直し
            s_lobby->SetState(PPW_LOBBY_STATE_DOWNLOAD_WAIT);
        }
        else
        {
            // メインチャンネル入室エラーは致命的エラー
            s_lobby->SetError(PPW_LOBBY_ERROR_SESSION);
        }
    }
    
    (void)result;
}

// サブチャンネル入室コールバック
static void LobbyJoinSubChannelCallback(BOOL success, DWCi_LOBBY_ENTER_RESULT result, const char* channelName, void* param)
{
    // VS2005だと一度(u32)にキャストする必要がある
    PPW_LOBBY_CHANNEL_KIND kind = (PPW_LOBBY_CHANNEL_KIND)(u32)param;
    if(kind == PPW_LOBBY_CHANNEL_KIND_INVALID)
    {
        DWC_ASSERTMSG(FALSE, "LobbyJoinChannelCallback: Internal error. No such channel.");
        return;
    }
    
    if(success)
    {
        // メンバーの確認をする
        //DWCi_LobbyEnumUsersAsync(channelName, LobbyEnumUsersCallback, kind);
        
        // チャンネルモードの変更をする
        const DWCi_LobbyChannelMode* mode = PPW_LobbyGetChannelMode(kind);
        if(!mode)
        {
            s_lobby->SetError(PPW_LOBBY_ERROR_UNKNOWN);
        }
        DWCi_LobbySetChannelMode(channelName, mode);
        
        // システムデータ更新
        PPW_LobbyInnerProfile& profile = s_lobby->GetProfileManager().GetMyProfile();
        PPW_LobbyInnerProfile::SystemProfile systemProfile = profile.GetSystemProfileConfirm();
        systemProfile.subChannelKind = kind;
        profile.SetSystemProfile(systemProfile);
    }
    else
    {
        s_lobby->GetSubChannel().Clear();
    }
    // VS2005だと一度(u32)にキャストする必要がある
    s_lobby->GetCallbacks().lobbyJoinChannelCallback(success, (PPW_LOBBY_CHANNEL_KIND)(u32)param);
    
    (void)result;
}

// プレイヤー接続コールバック
static void LobbyPlayerConnectedCallback(const char* channelName, s32 userId, void* param)
{
    // paramはPPW_LOBBY_CHANNEL_KIND_INVALID固定
    
    PPW_LOBBY_CHANNEL_KIND kind = s_lobby->GetChannelManager().GetChannelKind(channelName);
    if(kind == PPW_LOBBY_CHANNEL_KIND_INVALID)
    {
        DWC_ASSERTMSG(FALSE, "LobbyPlayerConnectedCallback: Internal error. No such channel.");
        return;
    }
    
    if(kind == PPW_LOBBY_CHANNEL_KIND_MAIN)
    {
        // メインチャンネルに入ってきた人はコンテナに入れる
        s_lobby->GetProfileManager().AddProfile(userId);
        
        // システム・ユーザ定義プロフィールが送信された後、接続コールバックが呼ばれる
    }
    else
    {
        // 自分が入っているサブチャンネルに出入りしたときのみシステムデータを先行して更新し通知する
        PPW_LobbyInnerProfile* profile = s_lobby->GetProfileManager().FindProfile(userId);
        if(!profile)
        {
            // すでにコンテナに入っているはず
            DWC_ASSERTMSG(FALSE, "LobbyPlayerConnectedCallback: no profile.");
            s_lobby->GetProfileManager().AddProfile(userId);
            profile = s_lobby->GetProfileManager().FindProfile(userId);
        }
        PPW_LobbyInnerProfile::SystemProfile systemProfile = profile->GetSystemProfileConfirm();
        systemProfile.subChannelKind = kind;
        profile->SetSystemProfile(systemProfile);
        // 変更を確定するためにもう一度GetSystemProfileConfirm()を呼ぶ
        const PPW_LobbyInnerProfile::SystemProfile& confirmedSystemProfile = profile->GetSystemProfileConfirm();
        PPW_LobbySystemProfile publicSystemProfile = confirmedSystemProfile.ToPublicSystemProfile();
        
        if(profile->IsActivated())
        {
            // サブチャンネル変更を通知
            s_lobby->GetCallbacks().lobbySystemProfileUpdatedCallback(userId, &publicSystemProfile);
        }
    }
    
    (void)param;
}

// プレイヤー切断コールバック
static void LobbyPlayerDisconnectedCallback(const char* channelName, s32 userId, void* param)
{
    // paramはPPW_LOBBY_CHANNEL_KIND_INVALID固定
    
    PPW_LOBBY_CHANNEL_KIND kind = s_lobby->GetChannelManager().GetChannelKind(channelName);
    if(kind == PPW_LOBBY_CHANNEL_KIND_INVALID)
    {
        DWC_ASSERTMSG(FALSE, "LobbyPlayerDisconnectedCallback: Internal error. No such channel.");
        return;
    }
    
    if(kind == PPW_LOBBY_CHANNEL_KIND_MAIN)
    {
        PPW_LobbyInnerProfile* profile = s_lobby->GetProfileManager().FindProfile(userId);
        if(!profile || !profile->IsActivated())
        {
            // 接続コールバックが呼ばれる前に退室した場合はなにもしない
            DWC_Printf(DWC_REPORTFLAG_INFO, "LobbyPlayerDisconnectedCallback: user disconnected but did not call connect callback.\n");
            return;
        }
        
        // コールバックを呼ぶ
        s_lobby->GetCallbacks().lobbyPlayerDisconnectedCallback(userId);
        
        // メインチャンネルから出た人はコンテナから削除
        s_lobby->GetProfileManager().RemoveProfile(userId);
    }
    else
    {
        // 自分が入っているサブチャンネルに出入りしたときのみシステムデータを先行して更新し通知する
        PPW_LobbyInnerProfile* profile = s_lobby->GetProfileManager().FindProfile(userId);
        if(!profile)
        {
            // すでにコンテナに入っているはず
#ifdef _LINUX  // linuxで発生する場合がある。
            DWC_Printf(DWC_REPORTFLAG_ERROR, "LobbyPlayerDisconnectedCallback: no profile.");
#else
            DWC_ASSERTMSG(FALSE, "LobbyPlayerDisconnectedCallback: no profile.");
#endif
            return;
        }
        PPW_LobbyInnerProfile::SystemProfile systemProfile = profile->GetSystemProfileConfirm();
        systemProfile.subChannelKind = PPW_LOBBY_CHANNEL_KIND_INVALID;
        profile->SetSystemProfile(systemProfile);
        // 変更を確定するためにもう一度GetSystemProfileConfirm()を呼ぶ
        const PPW_LobbyInnerProfile::SystemProfile& confirmedSystemProfile = profile->GetSystemProfileConfirm();
        PPW_LobbySystemProfile publicSystemProfile = confirmedSystemProfile.ToPublicSystemProfile();
        
        if(profile->IsActivated())
        {
            // サブチャンネル変更を通知
            s_lobby->GetCallbacks().lobbySystemProfileUpdatedCallback(userId, &publicSystemProfile);
        }
    }
    (void)param;
}

// プライベートメッセージ受信コールバック
static void LobbyUserMassageReceivedCallback(s32 userId, const char* message, void* param)
{
    LobbyChannelMassageReceivedCallback(NULL, userId, message, param);
}

// メッセージ受信コールバック
// channelNameがNULLのときはプライベートメッセージ
static void LobbyChannelMassageReceivedCallback(const char* channelName, s32 userId, const char* message, void* param)
{
    PPW_LOBBY_CHANNEL_KIND kind = PPW_LOBBY_CHANNEL_KIND_INVALID;
    if(channelName)
    {
        kind = s_lobby->GetChannelManager().GetChannelKind(channelName);
    }
    
    if(channelName != NULL && kind == PPW_LOBBY_CHANNEL_KIND_INVALID)
    {
        DWC_ASSERTMSG(FALSE, "LobbyChannelMassageReceivedCallback: Internal error. No such channel.");
        return;
    }
    
    
    // メッセージをパース
    PPW_LobbyMessage::Result result;
    if(!PPW_LobbyMessage::Parse(message, result))
    {
        DWC_Printf(DWC_REPORTFLAG_WARNING, "LobbyChannelMassageReceivedCallback: parse failed\n");
        return;
    }
    
    switch(result.target)
    {
    // ユーザ定義メッセージならコールバックを呼ぶ
    case PPW_LobbyMessage::TARGET_APPLICATION:
    {
        // アクティベートしていないプレイヤーからのメッセージは無視
        if(!s_lobby->GetProfileManager().IsActivatedProfile(userId))
        {
            DWC_Printf(DWC_REPORTFLAG_INFO, "LobbyChannelMassageReceivedCallback: Ignored not activated user's message.\n");
            return;
        }
        switch(result.format)
        {
        case PPW_LobbyMessage::FORMAT_BASE64:
            s_lobby->GetCallbacks().lobbyBinaryMassageReceivedCallback(userId, kind, result.type,
                                                                       (u8*)DWCi_GetVectorBuffer(result.data), result.data.size());
            break;
        case PPW_LobbyMessage::FORMAT_STRING:
            s_lobby->GetCallbacks().lobbyStringMassageReceivedCallback(userId, kind, result.type, (char*)DWCi_GetVectorBuffer(result.data));
            break;
        default:
            DWC_ASSERTMSG( FALSE, "LobbyChannelMassageReceivedCallback: Unknown message format." );
            break;
        }
        break;
    }
    // ライブラリ側で処理
    case PPW_LobbyMessage::TARGET_SYSTEM:
        PPW_SystemMessageHandler(result, userId, kind, (u8*)DWCi_GetVectorBuffer(result.data), result.data.size());
        break;
    default:
        DWC_ASSERTMSG( FALSE, "LobbyChannelMassageReceivedCallback: Unknown target." );
    }
    (void)param;
}

// ExcessFlood受信コールバック
static void LobbyExcessFloodCallback(u32 floodWeight, void* param)
{
    s_lobby->GetCallbacks().lobbyExcessFloodCallback(floodWeight);
    (void)param;
}

// チャンネルデータブロードキャスト受信コールバック
static void LobbyBroadcastDataCallback(const char* channelName, s32 userId, const char* key, const char* value, void* param)
{
    // ブロードキャストはparamをNULLにする。
    PPW_ChannelDataHandler(TRUE, TRUE, channelName, userId, key, value, NULL);
    (void)param;
}

// チャンネルデータ受信完了コールバック
static void LobbyGetChannelDataCallback(BOOL success, const char* channelName, s32 userId, const char* key, const char* value, void* param)
{
    PPW_ChannelDataHandler(success, FALSE, channelName, userId, key, value, param);
}

// チャンネル列挙コールバック
static void LobbyEnumChannelsCallback(BOOL success, s32 numChannels, const char** channels, const char** topics, const s32* numUsers, void* param)
{
    if(!success)
    {
        DWC_Printf(DWC_REPORTFLAG_WARNING, "LobbyEnumChannelsCallback: failed\n");
        return;
    }
    
    DWC_Printf(DWC_REPORTFLAG_INFO, "LobbyEnumChannelsCallback: found %d channels\n", numChannels);
    
    for(s32 i=0; i<numChannels; i++)
    {
        DWC_Printf(DWC_REPORTFLAG_INFO, "channel:%s, topic:%s, numUser:%u\n", channels[i], topics[i], numUsers[i]);
        if(numUsers[i] < PPW_LOBBY_MAX_PLAYER_NUM_MAIN)
        {
            // 最初のM!を除去し、ユニーク名のみを取り出す
            std::vector<DWCi_String, DWCi_Allocator<DWCi_String> > result = DWCi_SplitByStr<DWCi_String>(channels[i], DWCi_LOBBY_CHANNEL_NAME_SEPARATOR);
            if(result.size() < 2)
            {
                continue;
            }
            
            s_lobby->SetUniqueChannelName(result[1].c_str());
            s_lobby->SetCreatedNewMainChannel(FALSE);
            s_lobby->SetState(PPW_LOBBY_STATE_ENTERMAINCHANNEL);
            return;
        }
    }
    
    // 適当なチャンネルが見つからなかったので自分でチャンネルを新規に作る
    s_lobby->SetUniqueChannelName(PPW_LobbyGenerateUniqueChannelName());
    s_lobby->SetCreatedNewMainChannel(TRUE);
    s_lobby->SetState(PPW_LOBBY_STATE_ENTERMAINCHANNEL);
    
    (void)param;
    (void)topics;
    return;
}

// ユーザ列挙コールバック
static void LobbyEnumUsersCallback(BOOL success, const char* channelName, s32 numUsers, const s32* userIds, void* param)
{
    if(!success)
    {
        DWC_Printf(DWC_REPORTFLAG_WARNING, "LobbyEnumUsersCallback: failed\n");
        return;
    }
    
    DWC_Printf(DWC_REPORTFLAG_INFO, "LobbyEnumUsersCallback: exist %u users\n", numUsers);
    
    PPW_LOBBY_CHANNEL_KIND kind = s_lobby->GetChannelManager().GetChannelKind(channelName);
    if(kind == PPW_LOBBY_CHANNEL_KIND_INVALID)
    {
        DWC_ASSERTMSG(FALSE, "LobbyEnumUsersCallback: Internal error. No such channel.");
        return;
    }
    
    if(kind == PPW_LOBBY_CHANNEL_KIND_MAIN)
    {
        // メインチャンネルにいる人はコンテナに入れ、詳細情報を取得する
        for(s32 i=0; i<numUsers; i++)
        {
            if(userIds[i] == s_lobby->GetPid())
            {
                // 自分自身の情報はスキップする
                continue;
            }
            s_lobby->GetProfileManager().AddProfile(userIds[i]);
            DWCi_LobbyGetChannelDataAsync(channelName, userIds[i], PPW_LOBBY_CHANNEL_KEY_USER, LobbyGetChannelDataCallback, NULL);
            DWCi_LobbyGetChannelDataAsync(channelName, userIds[i], PPW_LOBBY_CHANNEL_KEY_SYSTEM, LobbyGetChannelDataCallback, NULL);
        }
        
        s_lobby->GetMainChannel().SetUserListIsEnumerated();
    }
    else
    {
    }
    (void)param;
}

// メインチャンネルのチャンネルモードを確認する
static void LobbyCheckMainChannelModeCallback(BOOL success, const char* channelName, const DWCi_LobbyChannelMode* mode, void* param)
{
    if(!success)
    {
        // TODO: リトライカウントする
        return;
    }
    
    if(mode->limit == PPW_LOBBY_MAX_PLAYER_NUM_MAIN)
    {
        // 初期設定済み
        s_lobby->GetMainChannel().SetModeIsSet();
        s_lobby->GetTimerManager().RemoveTimer(PPW_LOBBY_TIMER_ID_CHECK_MAINCHANNEL_MODE);
    }
    else
    {
        // 初期設定されていない
        
        // チャンネルの初期設定をする
        DWCi_LobbySetChannelMode(channelName, &PPW_LOBBY_CHANNEL_MODE_MAIN_OPEN);
    }
    (void)param;
}

// メインチャンネルのシステムデータを確認する
static void LobbyCheckMainChannelSystemProfileCallback(BOOL success, const char* channelName, s32 userId, const char* key, const char* value, void* param)
{
    if(!success)
    {
        // TODO: リトライカウントする
        return;
    }
    
    // Base64デコード
    std::vector<u8, DWCi_Allocator<u8> > tempBuf;
    BOOL result = DWCi_Base64Decode(value, tempBuf);
    if(!result || tempBuf.size() != sizeof(PPW_LobbyInnerInfoManager::TimeData))
    {
        // 初期設定されていないので自分のデータをストア
        PPW_LobbyInnerInfoManager::TimeData systemProfile = s_lobby->GetLobbyInnerInfoManager().InitializeTimeData();
        PPW_LobbySetChannelDataImpl(PPW_LOBBY_INVALID_USER_ID, PPW_LOBBY_CHANNEL_KIND_MAIN, PPW_LOBBY_CHANNEL_KEY_CHANNEL_TIME,
                                    (u8*)&systemProfile, sizeof(systemProfile));
        return;
    }
    else
    {
        // 初期設定済み
        PPW_LobbyInnerInfoManager::TimeData* systemProfile = (PPW_LobbyInnerInfoManager::TimeData*)DWCi_GetVectorBuffer(tempBuf);
        DWC_Printf(DWC_REPORTFLAG_INFO, "LobbyCheckMainChannelSystemProfileCallback: openedTime is %lld\n", systemProfile->openedTime);
        s_lobby->GetLobbyInnerInfoManager().SetTimeData(*systemProfile);
        s_lobby->GetTimerManager().RemoveTimer(PPW_LOBBY_TIMER_ID_CHECK_MAINCHANNEL_SYSTEMDATA);
    }
    (void)channelName;
    (void)userId;
    (void)key;
    (void)param;
}

// メインチャンネルのシステムデータ(ロビーデータ)を確認する
static void LobbyCheckLobbyInfoCallback(BOOL success, const char* channelName, s32 userId, const char* key, const char* value, void* param)
{
    if(!success)
    {
        // TODO: リトライカウントする
        return;
    }
    
    // Base64デコード
    std::vector<u8, DWCi_Allocator<u8> > tempBuf;
    BOOL result = DWCi_Base64Decode(value, tempBuf);

    // サイズチェックと代入を含む
    if(!result || tempBuf.empty()
       || !s_lobby->GetLobbyInnerInfoManager().SetLobbyInfo(DWCi_GetVectorBuffer(tempBuf), tempBuf.size()))
    {
        // 初期設定されていないので自分のデータをストア
        std::vector<u8, DWCi_Allocator<u8> > buf = s_lobby->GetLobbyInnerInfoManager().Serialize();
        PPW_LobbySetChannelDataImpl(PPW_LOBBY_INVALID_USER_ID, PPW_LOBBY_CHANNEL_KIND_MAIN, PPW_LOBBY_CHANNEL_KEY_CHANNEL_LOBBY,
                                    DWCi_GetVectorBuffer(buf), buf.size());
        return;
    }
    else
    {
        // 設定済み
        s_lobby->GetLobbyInnerInfoManager().Shared();
        s_lobby->GetTimerManager().RemoveTimer(PPW_LOBBY_TIMER_ID_CHECK_LOBBY_INFO);
    }
    (void)channelName;
    (void)userId;
    (void)key;
    (void)param;
}

// トピック変更コールバック
static void LobbyTopicChangedCallback(const char* channelName, const char* topic, void* param)
{
    (void)channelName;
    (void)topic;
    (void)param;
}
