
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
    PPW_LOBBY_ERROR_STATS_SESSION,          ///< 致命的な通信エラー(ルーム設定サーバ)。
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


typedef enum LANGUAGE
{
	LANGUAGE_NONE,
	LANGUAGE_JAPANESE,
	LANGUAGE_US,
	LANGUAGE_FR,
	LANGUAGE_IT,
	LANGUAGE_DE,
	LANGUAGE_SP,
	LANGUAGE_RESERVE,
	LANGUAGE_KR,
	LANGUAGE_MAX
}
LANGUAGE;

/// 質問条件
typedef struct PPW_LobbyQuestionCondition
{
    s32 questionNo;
    u32 multiLanguageSummarizeId;
}
PPW_LobbyQuestionCondition;

/// スペシャルウィークレコード
typedef struct PPW_LobbySpecialWeekRecord
{
    s32 questionSerialNo;
    PPW_LobbyQuestionCondition questionConditions[LANGUAGE_MAX];
}
PPW_LobbySpecialWeekRecord;

/// 任意質問
typedef struct PPW_LobbyFreeQuestion
{
    s32 language;                       ///< 登録する言語
    s32 questionNo;                         ///< スロット番号(質問番号-1000)
    u16 questionSentence[PPW_LOBBY_MAX_QUESTION_SENTENCE_LENGTH];   ///< 任意質問。
    u16 answer[PPW_LOBBY_QUESTIONNAIRE_ANSWER_NUM][PPW_LOBBY_MAX_ANSWER_LENGTH];    ///< 任意質問の回答。
}
PPW_LobbyFreeQuestion;