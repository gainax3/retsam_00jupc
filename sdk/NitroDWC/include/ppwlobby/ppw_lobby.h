/*---------------------------------------------------------------------------*
  Project:  NitroDWC Libraries
  File:     ./include/ppwlobby/dwc_lobby.h

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
 * @brief ポケモンプラチナ Wi-Fi ロビーライブラリ ヘッダ
 * 
 * ポケモンプラチナ Wi-Fi ロビーライブラリ
 * 
 * @author Ryo Yamaguchi (yamaguchi_ryo@nintendo.co.jp)
 */

#ifndef PPW_LOBBY_H_
#define PPW_LOBBY_H_

#include <lobby/dwc_lobby.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef WIN32
#pragma pack(push, 4)
#endif

/// PPW_LobbySend*StringMessage関数で送信できる最大の文字数。(終端含む)
#define PPW_LOBBY_MAX_STRING_SIZE               400

/// PPW_LobbySend*BinaryMessage関数で送信できる最大のデータ長。
#define PPW_LOBBY_MAX_BINARY_SIZE               300

/// ::PPW_LobbyRecruitInfo構造体のparamBufメンバに指定できるデータ長。
#define PPW_LOBBY_MAX_MATCHMAKING_BUF_SIZE      32

/// ::PPW_LobbyRecruitInfo構造体のmatchMakingStringメンバに指定されるマッチメイキング指標文字列の文字数(終端含む)。
#define PPW_LOBBY_MAX_MATCHMAKING_STRING_LENGTH 20

/// ::PPW_LobbyQuestionnaireRecord構造体のquestionSentenceメンバの最大文字数(終端含む)。
#define PPW_LOBBY_MAX_QUESTION_SENTENCE_LENGTH 110

/// ::PPW_LobbyQuestionnaireRecord構造体のanswerメンバの最大文字数(終端含む)。
#define PPW_LOBBY_MAX_ANSWER_LENGTH             18

/// アンケート機能での選択肢の数
#define PPW_LOBBY_QUESTIONNAIRE_ANSWER_NUM      3

/// 任意質問の開始質問番号
#define PPW_LOBBY_FREE_QUESTION_START_NO        1000

/// 任意質問番号の個数
#define PPW_LOBBY_FREE_QUESTION_NUM             10

/// アンケートが開催されていないことを示す質問番号
#define PPW_LOBBY_INVALID_QUESTION_NO           0xffffffff

// ロビーライブラリで使用するエラーコードの基準番号
#define PPW_LOBBY_ERROR_CODE_BASE               10620

/// ライブラリがシステム定義プロフィールの送受信に使う"各プレイヤーに割り当てられる"チャンネルデータキー。(アプリケーションでは使用しないでください)
#define PPW_LOBBY_CHANNEL_KEY_SYSTEM            "b_lib_u_system"
    
/// ライブラリがユーザ定義プロフィールの送受信に使う"各プレイヤーに割り当てられる"チャンネルデータキー。(アプリケーションでは使用しないでください)
#define PPW_LOBBY_CHANNEL_KEY_USER              "b_lib_u_user"

/// ライブラリがチャンネルの管理に使う"チャンネル自体に割り当てられる"チャンネルデータキー。(アプリケーションでは使用しないでください)
#define PPW_LOBBY_CHANNEL_KEY_CHANNEL_TIME      "b_lib_c_time"

/// ライブラリがチャンネルの管理に使う"チャンネル自体に割り当てられる"チャンネルデータキー。(アプリケーションでは使用しないでください)
#define PPW_LOBBY_CHANNEL_KEY_CHANNEL_LOBBY     "b_lib_c_lobby"

/// ミニゲーム募集で使用するマッチメイク指標キー。
#define PPW_LOBBY_MATCHMAKING_KEY               "ppw_lobby"

// ライブラリ内部で使用するデータ管理サーバへのリクエストのバージョン
#define PPW_LOBBY_ENC_REQUEST_VERSION           1

#define PPW_LOBBY_MAX_PLAYER_NUM_MAIN       20                          ///< メインチャンネルに入れる最大人数。
#define PPW_LOBBY_MAX_PLAYER_NUM_FOOT       8                           ///< 足跡ボード入れる最大人数。
#define PPW_LOBBY_MAX_PLAYER_NUM_CLOCK      4                           ///< 世界時計に入れる最大人数。
#define PPW_LOBBY_MAX_PLAYER_NUM_NEWS       4                           ///< ロビーニュースに入れる最大人数。

#define PPW_LOBBY_INVALID_USER_ID           DWC_LOBBY_INVALID_USER_ID   ///< 無効なユーザID。
#define PPW_LOBBY_INVALID_TIME              DWC_LOBBY_INVALID_TIME      ///< 無効な時間。


/// チャンネル名に付けるプリフィックス。入室チャンネルを分けることができます。PPW_LOBBY_CHANNEL_PREFIX*も使用できますが、できるだけDWC_LOBBY_CHANNEL_PREFIX*の方をお使い下さい。
typedef DWC_LOBBY_CHANNEL_PREFIX            PPW_LOBBY_CHANNEL_PREFIX;
#define PPW_LOBBY_CHANNEL_PREFIX_RELEASE    DWC_LOBBY_CHANNEL_PREFIX_RELEASE    ///< 製品用。(->DWC_LOBBY_CHANNEL_PREFIX_RELEASE)
#define PPW_LOBBY_CHANNEL_PREFIX_DEBUG      DWC_LOBBY_CHANNEL_PREFIX_DEBUG      ///< デバッグ用。(->DWC_LOBBY_CHANNEL_PREFIX_DEBUG)
#define PPW_LOBBY_CHANNEL_PREFIX_DEBUG1     DWC_LOBBY_CHANNEL_PREFIX_DEBUG1     ///< デバッグ用。(->DWC_LOBBY_CHANNEL_PREFIX_DEBUG1)
#define PPW_LOBBY_CHANNEL_PREFIX_DEBUG2     DWC_LOBBY_CHANNEL_PREFIX_DEBUG2     ///< デバッグ用。(->DWC_LOBBY_CHANNEL_PREFIX_DEBUG2)
#define PPW_LOBBY_CHANNEL_PREFIX_DEBUG3     DWC_LOBBY_CHANNEL_PREFIX_DEBUG3     ///< デバッグ用。(->DWC_LOBBY_CHANNEL_PREFIX_DEBUG3)
#define PPW_LOBBY_CHANNEL_PREFIX_DEBUG4     DWC_LOBBY_CHANNEL_PREFIX_DEBUG4     ///< デバッグ用。(->DWC_LOBBY_CHANNEL_PREFIX_DEBUG4)
#define PPW_LOBBY_CHANNEL_PREFIX_DEBUG5     DWC_LOBBY_CHANNEL_PREFIX_DEBUG5     ///< デバッグ用。(->DWC_LOBBY_CHANNEL_PREFIX_DEBUG5)
#define PPW_LOBBY_CHANNEL_PREFIX_DEBUG6     DWC_LOBBY_CHANNEL_PREFIX_DEBUG6     ///< デバッグ用。(->DWC_LOBBY_CHANNEL_PREFIX_DEBUG6)
#define PPW_LOBBY_CHANNEL_PREFIX_DEBUG7     DWC_LOBBY_CHANNEL_PREFIX_DEBUG7     ///< デバッグ用。(->DWC_LOBBY_CHANNEL_PREFIX_DEBUG7)
#define PPW_LOBBY_CHANNEL_PREFIX_DEBUG8     DWC_LOBBY_CHANNEL_PREFIX_DEBUG8     ///< デバッグ用。(->DWC_LOBBY_CHANNEL_PREFIX_DEBUG8)
#define PPW_LOBBY_CHANNEL_PREFIX_DEBUG9     DWC_LOBBY_CHANNEL_PREFIX_DEBUG9     ///< デバッグ用。(->DWC_LOBBY_CHANNEL_PREFIX_DEBUG9)
#define PPW_LOBBY_CHANNEL_PREFIX_TEST       DWC_LOBBY_CHANNEL_PREFIX_TEST       ///< ライブラリ開発用。(指定しないでください)(->DWC_LOBBY_CHANNEL_PREFIX_TEST)
    

/// ロビーライブラリ状態フラグ。
typedef enum PPW_LOBBY_STATE
{
    PPW_LOBBY_STATE_NOTINITIALIZED,         ///< ライブラリが初期化されていない。
    PPW_LOBBY_STATE_CONNECTING,             ///< チャットサーバに接続中。
    PPW_LOBBY_STATE_CONNECTED,              ///< チャットサーバに接続完了。
    PPW_LOBBY_STATE_DOWNLOAD,               ///< サーバから設定をダウンロード開始。
    PPW_LOBBY_STATE_DOWNLOAD_WAIT,          ///< サーバから設定をダウンロード中。
    PPW_LOBBY_STATE_SEARCHINGCHANNEL,       ///< メインチャンネルを検索中。
    PPW_LOBBY_STATE_ENTERMAINCHANNEL,       ///< メインチャンネルに入室開始。
    PPW_LOBBY_STATE_PREPARINGMAINCHANNEL,   ///< メインチャンネルに入室及び準備中。
    PPW_LOBBY_STATE_READY,                  ///< メインチャンネルに入室完了。
    PPW_LOBBY_STATE_CLOSING,                ///< ライブラリ終了処理中。
    PPW_LOBBY_STATE_ERROR                   ///< ライブラリに致命的なエラーが発生中。::PPW_LobbyGetLastError関数でエラーを取得後シャットダウンしてください。
}
PPW_LOBBY_STATE;

/// チャンネル状態フラグ。
typedef enum PPW_LOBBY_CHANNEL_STATE
{
    PPW_LOBBY_CHANNEL_STATE_NONE,           ///< チャンネルに入っていない。
    PPW_LOBBY_CHANNEL_STATE_UNAVAILABLE,    ///< チャンネルに入室準備中。
    PPW_LOBBY_CHANNEL_STATE_AVAILABLE,      ///< チャンネルに入室済み。
    PPW_LOBBY_CHANNEL_STATE_ERROR           ///< チャンネル状態を取得できませんでした。
}
PPW_LOBBY_CHANNEL_STATE;

/// 処理結果フラグ。
typedef enum PPW_LOBBY_RESULT
{
    PPW_LOBBY_RESULT_SUCCESS,               ///< 成功。
    PPW_LOBBY_RESULT_ERROR_ALLOC,           ///< メモリ確保に失敗。
    PPW_LOBBY_RESULT_ERROR_SESSION,         ///< 致命的な通信エラー。
    PPW_LOBBY_RESULT_ERROR_PARAM,           ///< 指定した引数が不正。
    PPW_LOBBY_RESULT_ERROR_STATE,           ///< 呼び出してはいけない状態で関数を呼び出した。
    PPW_LOBBY_RESULT_ERROR_CHANNEL,         ///< 指定したチャンネルに入っていない。
    PPW_LOBBY_RESULT_ERROR_NODATA,          ///< 指定した情報は存在しない。
    PPW_LOBBY_RESULT_ERROR_CONDITION,       ///< 致命的エラーが発生中なのでこの関数を呼び出せない。
    PPW_LOBBY_RESULT_MAX                    ///< ライブラリ内部で使用されます。
}
PPW_LOBBY_RESULT;

/// チャンネルの種類。
typedef enum PPW_LOBBY_CHANNEL_KIND
{
    PPW_LOBBY_CHANNEL_KIND_MAIN,            ///< メインチャンネル。
    PPW_LOBBY_CHANNEL_KIND_FOOT1,           ///< 足跡ボード1。
    PPW_LOBBY_CHANNEL_KIND_FOOT2,           ///< 足跡ボード2。
    PPW_LOBBY_CHANNEL_KIND_CLOCK,           ///< 世界時計。
    PPW_LOBBY_CHANNEL_KIND_NEWS,            ///< ロビーニュース。
    PPW_LOBBY_CHANNEL_KIND_INVALID          ///< 無効なチャンネル。
}
PPW_LOBBY_CHANNEL_KIND;

/// エラーフラグ。
typedef enum PPW_LOBBY_ERROR
{
    PPW_LOBBY_ERROR_NONE,                   ///< エラー無し。
    PPW_LOBBY_ERROR_UNKNOWN,                ///< 不明なエラー。
    PPW_LOBBY_ERROR_ALLOC,                  ///< メモリ確保に失敗。
    PPW_LOBBY_ERROR_SESSION,                ///< 致命的な通信エラー(チャットサーバ)。
    PPW_LOBBY_ERROR_STATS_SCHEDULE,         ///< 致命的な通信エラー(スケジュール取得時)。
    PPW_LOBBY_ERROR_STATS_VIP,              ///< 致命的な通信エラー(VIP設定取得時)。
    PPW_LOBBY_ERROR_STATS_CHECKPROFILE,     ///< 致命的な通信エラー(不正チェック時)。
    PPW_LOBBY_ERROR_STATS_QUESTIONNAIRE,    ///< 致命的な通信エラー(アンケート情報取得時)。
    PPW_LOBBY_ERROR_MAX                     ///< ライブラリ内部で使用されます。
}
PPW_LOBBY_ERROR;

/// タイムイベントの種類。::PPW_LobbyScheduleProgressCallbackコールバックで使用されます。
typedef enum PPW_LOBBY_TIME_EVENT
{
    PPW_LOBBY_TIME_EVENT_LOCK,              ///< 部屋のロック。
    PPW_LOBBY_TIME_EVENT_NEON_A0,           ///< 室内ネオン諧調0。
    PPW_LOBBY_TIME_EVENT_NEON_A1,           ///< 室内ネオン諧調1。
    PPW_LOBBY_TIME_EVENT_NEON_A2,           ///< 室内ネオン諧調2。
    PPW_LOBBY_TIME_EVENT_NEON_A3,           ///< 室内ネオン諧調3。
    PPW_LOBBY_TIME_EVENT_NEON_A4,           ///< 室内ネオン諧調4。
    PPW_LOBBY_TIME_EVENT_NEON_A5,           ///< 室内ネオン諧調5。
    PPW_LOBBY_TIME_EVENT_NEON_B0,           ///< 床ネオン諧調0。
    PPW_LOBBY_TIME_EVENT_NEON_B1,           ///< 床ネオン諧調1。
    PPW_LOBBY_TIME_EVENT_NEON_B2,           ///< 床ネオン諧調2。
    PPW_LOBBY_TIME_EVENT_NEON_B3,           ///< 床ネオン諧調3。
    PPW_LOBBY_TIME_EVENT_NEON_C0,           ///< モニュメント諧調0。
    PPW_LOBBY_TIME_EVENT_NEON_C1,           ///< モニュメント諧調1。
    PPW_LOBBY_TIME_EVENT_NEON_C2,           ///< モニュメント諧調2。
    PPW_LOBBY_TIME_EVENT_NEON_C3,           ///< モニュメント諧調3。
    PPW_LOBBY_TIME_EVENT_MINIGAME_END,      ///< ミニゲーム終了。
    PPW_LOBBY_TIME_EVENT_FIRE_WORKS_START,  ///< ファイアーワークス開始。
    PPW_LOBBY_TIME_EVENT_FIRE_WORKS_END,    ///< ファイアーワークス終了。
    PPW_LOBBY_TIME_EVENT_PARADE,            ///< パレード。
    PPW_LOBBY_TIME_EVENT_CLOSE              ///< 終了。
}
PPW_LOBBY_TIME_EVENT;

/// ::PPW_LobbySchedule構造体のroomFlagメンバに対応する部屋設定フラグ
typedef enum PPW_LOBBY_ROOM_FLAG
{
    PPW_LOBBY_ROOM_FLAG_ARCEUS  = 0x00000001    ///< アルセウスを許可するか
}
PPW_LOBBY_ROOM_FLAG;

/// スケジュールやVIP設定のダウンロード結果
typedef enum PPW_LOBBY_STATS_RESULT
{
    PPW_LOBBY_STATS_RESULT_SUCCESS,         ///< 成功。
    PPW_LOBBY_STATS_RESULT_INVALID_PARAM,   ///< 送信したパラメータが不正です。
    PPW_LOBBY_STATS_RESULT_SERVER_ERROR     ///< サーバがエラーを返しました。
}
PPW_LOBBY_STATS_RESULT;


/// システム定義プロフィール。
typedef struct PPW_LobbySystemProfile
{
    s64 enterTime;                          ///< ロビーに入った時刻。
    PPW_LOBBY_CHANNEL_KIND subChannelKind;  ///< 参加しているサブチャンネル。
}
PPW_LobbySystemProfile;

/// ミニゲーム募集要項。
typedef struct PPW_LobbyRecruitInfo
{
    s32 gameKind;                           ///< 募集するミニゲームの種類。
    u32 maxNum;                             ///< 募集する最大人数。
    u32 currentNum;                         ///< 現在集まっている人数。
    u32 paramVal;                           ///< ユーザ定義データ1。
    u8 paramBuf[PPW_LOBBY_MAX_MATCHMAKING_BUF_SIZE];    ///< ユーザ定義データ2。
    
    /// マッチング指標文字列。::PPW_LobbyStartRecruit関数内部にてセットされますので変更しないでください。
    char matchMakingString[PPW_LOBBY_MAX_MATCHMAKING_STRING_LENGTH];
}
PPW_LobbyRecruitInfo;

/// 時刻情報。
typedef struct PPW_LobbyTimeInfo
{
    s64 currentTime;                        ///< 現在の時刻。
    s64 openedTime;                         ///< 部屋を作成した時刻。
    s64 lockedTime;                         ///< 部屋をロックした時刻。
}
PPW_LobbyTimeInfo;

/// スケジュールレコード。
typedef struct PPW_LobbyScheduleRecord
{
    s32 time;                               ///< イベントが起動する時刻(メインチャンネルをロックしたときからの時間)。
    PPW_LOBBY_TIME_EVENT event;             ///< 起動するイベント。
}
PPW_LobbyScheduleRecord;

/// サーバから取得したチャンネルの設定情報です。可変長構造体です。
typedef struct PPW_LobbySchedule
{
    u32 lockTime;                           ///< メインチャンネルをオープンしてからロックするまでの時間(秒)。
    u32 random;                             ///< サーバで生成される32bit範囲のランダム値。
    u32 roomFlag;                           ///< ::PPW_LOBBY_ROOM_FLAGに対応する各種bitフラグ。
    u8 roomType;                            ///< 部屋の種類。
    u8 season;                              ///< 季節番号。
    u16 scheduleRecordNum;                  ///< スケジュールレコードの数。
    PPW_LobbyScheduleRecord scheduleRecords[1]; ///< スケジュールレコードの配列(可変長)。
}
PPW_LobbySchedule;

/// VIPレコード。
typedef struct PPW_LobbyVipRecord
{
    s32 profileId;                          ///< VIPのProfileID。
    s32 key;                                ///< 合言葉生成用キー。合言葉無しの場合は0になります。
}
PPW_LobbyVipRecord;

/// アンケート内容
typedef struct PPW_LobbyQuestionnaireRecord
{
    s32 questionSerialNo;                   ///< 質問通し番号。0からスタート。
    s32 questionNo;                         ///< 質問番号。ROM内質問:0～59 任意質問:PPW_LOBBY_FREE_QUESTION_START_NO～PPW_LOBBY_FREE_QUESTION_START_NO+PPW_LOBBY_FREE_QUESTION_NUM
    u16 questionSentence[PPW_LOBBY_MAX_QUESTION_SENTENCE_LENGTH];   ///< 任意質問。質問番号が任意質問の範囲だったときのみ格納されます。
    u16 answer[PPW_LOBBY_QUESTIONNAIRE_ANSWER_NUM][PPW_LOBBY_MAX_ANSWER_LENGTH];    ///< 任意質問の回答。質問番号が任意質問の範囲だったときのみ格納されます。
    u8 multiLanguageSummarizeFlags[12];     ///< 集計言語。0は未集計、1は集計したことを示します。インデックスには言語コードに対応します。[0]と[6]と[9]以降は常に0になります。
    BOOL isSpecialWeek;                     ///< スペシャルウィークか
}
PPW_LobbyQuestionnaireRecord;

/// アンケート情報
typedef struct PPW_LobbyQuestionnaire
{
    PPW_LobbyQuestionnaireRecord currentQuestionnaireRecord;    ///< 現在のアンケート情報
    PPW_LobbyQuestionnaireRecord lastQuestionnaireRecord;       ///< 前回のアンケート情報
    s32 lastResult[PPW_LOBBY_QUESTIONNAIRE_ANSWER_NUM];         ///< 前回の自分の言語の結果
    s32 lastMultiResult[PPW_LOBBY_QUESTIONNAIRE_ANSWER_NUM];    ///< 前回の複数集計言語での集計結果。::lastQuestionnaireRecord構造体のmultiLanguageSummarizeFlagsメンバで表される言語での集計結果です。
}
PPW_LobbyQuestionnaire;


// コールバック関数型定義

/**
 * @brief 自分がチャンネルに入ったときに呼び出されるコールバック関数型。
 * 
 * 自分がチャンネルに入ったときに呼び出されるコールバック関数型。
 * 
 * @param[in] success 入室に成功したか。入室に失敗するのはサブチャンネルに入ろうとしたときに定員オーバーしていた場合のみです。
 * @param[in] channelKind 入室したチャンネルの種類。
 */
typedef void (*PPW_LobbyJoinChannelCallback)(BOOL success, PPW_LOBBY_CHANNEL_KIND channelKind);

/**
 * @brief 自分自身もしくは他のプレイヤーがメインチャンネルに入室したときに呼び出されるコールバック関数型。
 * 
 * 自分自身もしくは他のプレイヤーがメインチャンネルに入室したときに呼び出されるコールバック関数型。\n
 * 自分がメインチャンネルに入るとき、自分自身と自分が入室する以前からいたプレイヤーに対するコールバックは\n
 * ステートが::PPW_LOBBY_STATE_PREPARINGMAINCHANNELの時に発生します。\n
 * 自分自身に対するコールバックは必ず一番最初に行われます。
 * 
 * @param[in] userId 入室したユーザID。
 * @param[in] systemProfile 入室したプレイヤーのシステム定義プロフィール。
 * @param[in] userProfile 入室したのユーザ定義プロフィール。
 * @param[in] userProfileSize userProfileのサイズ。
 */
typedef void (*PPW_LobbyPlayerConnectedCallback)(s32 userId, const PPW_LobbySystemProfile* systemProfile, const u8* userProfile, u32 userProfileSize);

/**
 * @brief 他のプレイヤーがメインチャンネルから退室したときに呼び出されるコールバック関数型。
 * 
 * 他のプレイヤーがメインチャンネルから退室したときに呼び出されるコールバック関数型。
 * 
 * @param[in] userId 退室したユーザID。
 */
typedef void (*PPW_LobbyPlayerDisconnectedCallback)(s32 userId);

/**
 * @brief 文字列メッセージを受信したときに呼び出されるコールバック関数型。
 * 
 * 文字列メッセージを受信したときに呼び出されるコールバック関数型。
 * 
 * @param[in] userId 送信元のユーザID。
 * @param[in] channelKind 送信されたチャンネルの種類。PPW_LobbySendUser*Message関数で送信されたときは::PPW_LOBBY_INVALID_USER_IDが指定されます。
 * @param[in] type 送信者が指定したメッセージのタイプ。
 * @param[in] message 受信したNULL終端文字列。
 */
typedef void (*PPW_LobbyStringMassageReceivedCallback)(s32 userId, PPW_LOBBY_CHANNEL_KIND channelKind, s32 type, const char* message);

/**
 * @brief バイナリメッセージを受信したときに呼び出されるコールバック関数型。
 * 
 * バイナリメッセージを受信したときに呼び出されるコールバック関数型。
 * 
 * @param[in] userId 送信元のユーザID。
 * @param[in] channelKind 送信されたチャンネルの種類。PPW_LobbySendUser*Message関数で送信されたときは::PPW_LOBBY_INVALID_USER_IDとなります。
 * @param[in] type 送信者が指定したメッセージのタイプ。
 * @param[in] data 受信したバイナリデータ。
 * @param[in] dataSize dataのサイズ。
 */
typedef void (*PPW_LobbyBinaryMassageReceivedCallback)(s32 userId, PPW_LOBBY_CHANNEL_KIND channelKind, s32 type, const u8* data, u32 dataSize);

/**
 * @brief チャンネルデータを取得したときに呼び出されるコールバック関数型。
 * 
 * チャンネルデータを取得したときに呼び出されるコールバック関数型。
 * ::PPW_LobbyGetChannelDataAsync関数の結果または他のプレイヤーが::PPW_LobbySetChannelData関数でブロードキャストキー("b_"で始まるキー)を指定し、
 * それを受信したときに呼び出されます。
 * 
 * @param[in] success 成功したか。ブロードキャストを受信した場合は必ずTRUEとなります。
 * @param[in] broadcast ::PPW_LobbyGetChannelDataAsync関数で取得したものならFALSE、
 *                      他のプレイヤーが::PPW_LobbySetChannelData関数でブロードキャストされたものならTRUE。
 * @param[in] channelKind このチャンネルデータに関連づけられているチャンネルの種類。
 * @param[in] userId このチャンネルデータを持つユーザID。チャンネル自体のデータを取得したときは::PPW_LOBBY_INVALID_USER_IDとなります。
 * @param[in] key 受信したキー文字列。
 * @param[in] data keyに関連づけられたデータ。
 * @param[in] dataSize dataのサイズ。
 * @param[in] param ::PPW_LobbyGetChannelDataAsync関数に指定したparam。ブロードキャストを受信した場合はNULLになります。
 */
typedef void (*PPW_LobbyGetChannelDataCallback)(BOOL success, BOOL broadcast, PPW_LOBBY_CHANNEL_KIND channelKind, s32 userId,
                                                const char* key, const u8* data, u32 dataSize, void* param);

/**
 * @brief 他のプレイヤーのシステム定義プロフィール(入室サブチャンネルなど)が変更されたときに呼び出されるコールバック関数型。
 * 
 * 他のプレイヤーのシステム定義プロフィール(入室サブチャンネルなど)が変更されたときに呼び出されるコールバック関数型。\n
 * 現在はプレイヤーがサブチャンネルを変更したときのみ呼び出されます。
 * 
 * @param[in] userId システム定義プロフィールを変更したユーザID。
 * @param[in] systemProfile 変更されたシステム定義プロフィール。
 */
typedef void (*PPW_LobbySystemProfileUpdatedCallback)(s32 userId, const PPW_LobbySystemProfile* systemProfile);

/**
 * @brief 他のプレイヤーのユーザ定義プロフィールが変更されたときに呼び出されるコールバック関数型。
 * 
 * 他のプレイヤーのユーザ定義プロフィールが変更されたときに呼び出されるコールバック関数型。
 * 
 * @param[in] userId ユーザ定義プロフィールを変更したユーザID。
 * @param[in] userProfile 変更されたユーザ定義プロフィール。
 * @param[in] userProfileSize userProfileのサイズ。
 */
typedef void (*PPW_LobbyUserProfileUpdatedCallback)(s32 userId, const u8* userProfile, u32 userProfileSize);

/**
 * @brief 他のプレイヤーがミニゲームを募集している時に呼び出されるコールバック関数型。
 * 
 * 他のプレイヤーがミニゲームを募集している時に呼び出されるコールバック関数型。
 * 
 * @param[in] userId 募集しているユーザID。
 * @param[in] recruitInfo 募集要項。
 */
typedef void (*PPW_LobbyRecruitCallback)(s32 userId, const PPW_LobbyRecruitInfo* recruitInfo);

/**
 * @brief 他のプレイヤーがミニゲームの募集を終了した時に呼び出されるコールバック関数型。
 * 
 * 他のプレイヤーがミニゲームの募集を終了した時に呼び出されるコールバック関数型。
 * 
 * @param[in] userId 募集していたユーザID。
 * @param[in] recruitInfo 最後に使用していた募集要項。
 */
typedef void (*PPW_LobbyRecruitStoppedCallback)(s32 userId, const PPW_LobbyRecruitInfo* recruitInfo);

/**
 * @brief イベントが発生したときに呼び出されるコールバック関数型。
 * 
 * イベントが発生したときに呼び出されるコールバック関数型。
 * 
 * @param[in] event 発生したイベントの種類。
 */
typedef void (*PPW_LobbyScheduleProgressCallback)(PPW_LOBBY_TIME_EVENT event);

/**
 * @brief 不正チェックの結果を通知するコールバック関数型。
 * 
 * 不正チェックの結果を通知するコールバック関数型。ROMバージョンなどにおける致命的な不正だった場合はresultが::PPW_LOBBY_STATS_RESULT_INVALID_PARAMになり、
 * 置換可能なパラメータが不正だった場合は置換されたユーザ定義プロフィールが通知されます。
 * 
 * @param[in] result 通信結果。::PPW_LOBBY_STATS_RESULT_SUCCESS以外だったときはライブラリはエラー状態になります。
 *                   このときは::PPW_LobbyShutdownAsync関数でライブラリを終了してください。
 *                   サーバに送信したユーザ定義プロフィールが不正チェックに引っかかった場合は::PPW_LOBBY_STATS_RESULT_INVALID_PARAMになります。
 * @param[in] userProfile (もし置換箇所があれば)置換されたユーザ定義プロフィール。
 *                        すでにこのプロフィールを使用するようにセットされていますので::PPW_LobbyUpdateMyProfile関数を用いて再セットする必要はありません。
 *                        resultが::PPW_LOBBY_STATS_RESULT_SUCCESS以外のときはNULLになります。
 * @param[in] userProfileSize userProfileのサイズ。
 */
typedef void (*PPW_LobbyCheckProfileCallback)(PPW_LOBBY_STATS_RESULT result, const u8* userProfile, u32 userProfileSize);

/**
 * @brief サーバからスケジュールを取得したときに呼び出されるコールバック関数型。
 * 
 * サーバからスケジュールを取得したときに呼び出されるコールバック関数型。
 * このコールバックで渡されたスケジュールが実際にWi-Fiひろばのスケジュールに採用されるかは確定していません。
 * メインチャンネルに一番最初に入った人のスケジュールが使用されることになります。
 * ::PPW_LobbyJoinChannelCallbackコールバックが発生しメインチャンネルに入室したときにスケジュールが確定しますので
 * このときに::PPW_LobbyGetSchedule関数で取得してください。
 * 返り値でどのデータを採用候補にするかを選択してください。
 * 
 * @param[in] result 通信結果。::PPW_LOBBY_STATS_RESULT_SUCCESS以外だったときはライブラリはエラー状態になります。
 *                   このときは::PPW_LobbyShutdownAsync関数でライブラリを終了してください。
 * @param[in] schedule 取得したスケジュール。resultが::PPW_LOBBY_STATS_RESULT_SUCCESS以外のときはNULLになります。
 * 
 * @retval TRUE ダウンロードしてきたこのスケジュールを使用する。
 * @retval FALSE デバッグ用にダウンロードしたこのスケジュールは使わず、
 *               事前にまたは本コールバック内で::PPW_LobbySetSchedule関数によりセットしたスケジュールを使用するか、デフォルトのスケジュールを使用する。
 */
typedef BOOL (*PPW_LobbyNotifyScheduleCallback)(PPW_LOBBY_STATS_RESULT result, const PPW_LobbySchedule* schedule);

/**
 * @brief サーバからVIP情報を取得したときに呼び出されるコールバック関数型。
 * 
 * サーバからVIP情報を取得したときに呼び出されるコールバック関数型。このVIP情報は確定であり、今後変化することはありません。
 * 
 * @param[in] result 通信結果。::PPW_LOBBY_STATS_RESULT_SUCCESS以外だったときはライブラリはエラー状態になります。
 *                   このときは::PPW_LobbyShutdownAsync関数でライブラリを終了してください。
 * @param[in] vipRecords 取得したVIP情報の配列。resultが::PPW_LOBBY_STATS_RESULT_SUCCESS以外のときはNULLになります。
 * @param[in] num 取得したVIP情報の数。0個の場合もあります。resultが::PPW_LOBBY_STATS_RESULT_SUCCESS以外のときは0になります。
 */
typedef void (*PPW_LobbyGetVipCallback)(PPW_LOBBY_STATS_RESULT result, const PPW_LobbyVipRecord* vipRecords, u32 num);

/**
 * @brief サーバからアンケート情報を取得したときに呼び出されるコールバック関数型。
 * 
 * サーバからアンケート情報を取得したときに呼び出されるコールバック関数型。
 * 
 * @param[in] result 通信結果。::PPW_LOBBY_STATS_RESULT_SUCCESS以外だったときはライブラリはエラー状態になります。
 *                   このときは::PPW_LobbyShutdownAsync関数でライブラリを終了してください。
 * @param[in] questionnaire 取得したアンケート情報。resultが::PPW_LOBBY_STATS_RESULT_SUCCESS以外のときはNULLになります。
 */
typedef void (*PPW_LobbyGetQuestionnaireCallback)(PPW_LOBBY_STATS_RESULT result, const PPW_LobbyQuestionnaire* questionnaire);

/**
 * @brief アンケートの提出に完了したときに呼び出されるコールバック関数型。
 * 
 * アンケートの提出に完了したときに呼び出されるコールバック関数型。
 * 
 * @param[in] result 通信結果。提出に失敗すると::PPW_LOBBY_STATS_RESULT_SUCCESS以外になりますが、ライブラリ自体はエラー状態にならずそのまま続行が可能です。
 */
typedef void (*PPW_LobbySubmitQuestionnaireCallback)(PPW_LOBBY_STATS_RESULT result);

/**
 * @brief サーバとの通信を過剰に行ったときに呼び出されるコールバック関数型。
 * 
 * サーバとの通信を過剰に行ったときに呼び出されるコールバック関数型。
 * floodWeightは通信を発生させるロビーライブラリの関数の呼び出しによって上昇し、
 * 300を超えると::PPW_LobbyGetChannelDataAsync関数が無視されるようになり、600を超えるとサーバから切断されます。
 * floodWeightは1秒に5減少します。
 * ただしPPW_LobbySend*Message関数の呼び出しはfloodWeightを上昇させません。
 * このコールバックが発生したときはチャンネルデータセットや取得、プロフィールの更新、チャンネルの出入りを控えるようにしてください。
 * もしくはこのコールバックが発生しない程度に通信する頻度を下げてください。
 * 
 * @param[in] floodWeight 通信過多の割合を示す重み。
 */
typedef void (*PPW_LobbyExcessFloodCallback)(u32 floodWeight);

/// コールバック登録用構造体
typedef struct PPW_LobbyCallbacks
{
    /// 自分がチャンネルに入ったときに呼び出されるコールバック関数。
    PPW_LobbyJoinChannelCallback lobbyJoinChannelCallback;
    /// 自分自身もしくは他のプレイヤーがメインチャンネルに入室したときに呼び出されるコールバック関数。
    PPW_LobbyPlayerConnectedCallback lobbyPlayerConnectedCallback;
    /// 他のプレイヤーがメインチャンネルから退室したときに呼び出されるコールバック関数。
    PPW_LobbyPlayerDisconnectedCallback lobbyPlayerDisconnectedCallback;
    /// 文字列メッセージを受信したときに呼び出されるコールバック関数。
    PPW_LobbyStringMassageReceivedCallback lobbyStringMassageReceivedCallback;
    /// バイナリメッセージを受信したときに呼び出されるコールバック関数。
    PPW_LobbyBinaryMassageReceivedCallback lobbyBinaryMassageReceivedCallback;
    /// チャンネルデータを取得したときに呼び出されるコールバック関数。
    PPW_LobbyGetChannelDataCallback lobbyGetChannelDataCallback;
    /// 他のプレイヤーのシステム定義プロフィールが変更されたときに呼び出されるコールバック関数。
    PPW_LobbySystemProfileUpdatedCallback lobbySystemProfileUpdatedCallback;
    /// 他のプレイヤーのユーザ定義プロフィールが変更されたときに呼び出されるコールバック関数。
    PPW_LobbyUserProfileUpdatedCallback lobbyUserProfileUpdatedCallback;
    /// 他のプレイヤーがミニゲームを募集している時に呼び出されるコールバック関数。
    PPW_LobbyRecruitCallback lobbyRecruitCallback;
    /// 他のプレイヤーがミニゲームの募集を終了した時に呼び出されるコールバック関数。
    PPW_LobbyRecruitStoppedCallback lobbyRecruitStoppedCallback;
    /// イベントが発生したときに呼び出されるコールバック関数。
    PPW_LobbyScheduleProgressCallback lobbyScheduleProgressCallback;
    /// 不正チェックの結果を通知するコールバック関数。
    PPW_LobbyCheckProfileCallback lobbyCheckProfileCallback;
    /// スケジュールを受信したときに呼び出されるコールバック関数。
    PPW_LobbyNotifyScheduleCallback lobbyNotifyScheduleCallback;
    /// VIP情報を受信したときに呼び出されるコールバック関数。
    PPW_LobbyGetVipCallback lobbyGetVipCallback;
    /// アンケート情報を受信したときに呼び出されるコールバック関数。
    PPW_LobbyGetQuestionnaireCallback lobbyGetQuestionnaireCallback;
    /// アンケートの提出に完了したときに呼び出されるコールバック関数。
    PPW_LobbySubmitQuestionnaireCallback lobbySubmitQuestionnaireCallback;
    /// サーバとの通信を過剰に行ったときに呼び出されるコールバック関数。
    PPW_LobbyExcessFloodCallback lobbyExcessFloodCallback;
}
PPW_LobbyCallbacks;

// ライブラリ初期化
PPW_LOBBY_RESULT PPW_LobbyInitializeAsync(const char* gameName, const char* secretKey, DWC_LOBBY_CHANNEL_PREFIX prefix,
                                          const PPW_LobbyCallbacks* lobbyCallbacks, const DWCUserData* dwcUserdata, const u8* userProfile, u32 userProfileSize);
// ライブラリ初期化(デバッグ用)
PPW_LOBBY_RESULT PPW_LobbyInitializePidAsync(const char* gameName, const char* secretKey, DWC_LOBBY_CHANNEL_PREFIX prefix,
                                        const PPW_LobbyCallbacks* lobbyCallbacks, s32 pid, const u8* userProfile, u32 userProfileSize);

// ライブラリ終了
PPW_LOBBY_RESULT PPW_LobbyShutdownAsync();

// プロセス関数
PPW_LOBBY_STATE PPW_LobbyProcess();

// エラー情報取得
PPW_LOBBY_ERROR PPW_LobbyGetLastError();

// 発生したエラー(::PPW_LOBBY_ERROR)から表示すべきエラーコードを得る。
s32 PPW_LobbyToErrorCode(PPW_LOBBY_ERROR err);

// 足跡ボード、世界時計、ロビーニュースに参加する
PPW_LOBBY_RESULT PPW_LobbyJoinSubChannelAsync(PPW_LOBBY_CHANNEL_KIND channelKind);

// 足跡ボード、世界時計、ロビーニュースから出る
PPW_LOBBY_RESULT PPW_LobbyLeaveSubChannel();

// 時刻情報を取得
PPW_LOBBY_RESULT PPW_LobbyGetTimeInfo(PPW_LobbyTimeInfo* timeInfo);

// スケジュールをセットする(デバッグ用)
PPW_LOBBY_RESULT PPW_LobbySetSchedule(const PPW_LobbySchedule* schedule, u32 scheduleSize);

// スケジュールを取得する
PPW_LOBBY_RESULT PPW_LobbyGetSchedule(PPW_LobbySchedule* schedule, u32* scheduleSize);

// VIP情報を取得する
PPW_LOBBY_RESULT PPW_LobbyGetVip(PPW_LobbyVipRecord* records, u32* recordNum);

// サブチャンネルの状態を取得
PPW_LOBBY_CHANNEL_STATE PPW_LobbyGetSubChannelState();
    
// チャンネルデータ保存
PPW_LOBBY_RESULT PPW_LobbySetChannelData(s32 userId, PPW_LOBBY_CHANNEL_KIND channelKind, const char* key, const u8* data, u32 dataSize);

// チャンネルデータ取得
PPW_LOBBY_RESULT PPW_LobbyGetChannelDataAsync(s32 userId, PPW_LOBBY_CHANNEL_KIND channelKind, const char* key, void* param);

// Ascii文字列のメッセージをチャンネルに送信
PPW_LOBBY_RESULT PPW_LobbySendChannelStringMessage(PPW_LOBBY_CHANNEL_KIND channelKind, s32 type, const char* message);

// Ascii文字列のメッセージを一人のプレイヤーに送信
PPW_LOBBY_RESULT PPW_LobbySendPlayerStringMessage(s32 userId, s32 type, const char* message);

// バイナリメッセージをチャンネルに送信
PPW_LOBBY_RESULT PPW_LobbySendChannelBinaryMessage(PPW_LOBBY_CHANNEL_KIND channelKind, s32 type, const u8* data, u32 dataSize);

// バイナリデータを一人のプレイヤーに送信
PPW_LOBBY_RESULT PPW_LobbySendPlayerBinaryMessage(s32 userId, s32 type, const u8* data, u32 dataSize);

// ロビーライブラリで使用している自分のユーザIDを取得
s32 PPW_LobbyGetMyUserId();

// 現在メインチャンネルにいる自分以外のユーザIDを列挙
PPW_LOBBY_RESULT PPW_LobbyEnumUserId(s32* userIds, u32* num);

// ユーザ定義プロフィール参照。userIdに自分のGameSpy Profile IDもしくはPPW_LOBBY_INVALID_USER_IDを渡すと自分のユーザ定義プロフィールを参照できる
PPW_LOBBY_RESULT PPW_LobbyGetProfile(s32 userId, u8* userProfile, u32* userProfileSize);

// 自分のユーザ定義プロフィールを更新
PPW_LOBBY_RESULT PPW_LobbyUpdateMyProfile(const u8* userProfile, u32 userProfileSize, BOOL force);

// ミニゲーム募集
PPW_LOBBY_RESULT PPW_LobbyStartRecruit(PPW_LobbyRecruitInfo* recruitInfo);

// ミニゲーム募集情報更新
PPW_LOBBY_RESULT PPW_LobbyUpdateRecruitInfo(const PPW_LobbyRecruitInfo* recruitInfo);

// ミニゲーム募集終了
PPW_LOBBY_RESULT PPW_LobbyStopRecruit();

// アンケートを提出する
PPW_LOBBY_RESULT PPW_LobbySubmitQuestionnaire(s32 answerNo);

// DWCの内部状態取得
// 指定した人と接続しているか
BOOL PPW_IsConnectedUserId(const DWCFriendsMatchControl* dwcFriendMatchCtrl, s32 userId);

// デバッグ用に任意のEncInitDataを指定
void PPW_LobbySetEncInitData(const char* initData);

#ifdef WIN32
#pragma pack(pop)
#endif

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // PPW_LOBBY_H_
