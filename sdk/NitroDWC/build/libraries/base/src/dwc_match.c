#include <nitro.h>
#include <base/dwc_base_gamespy.h>
#include <natneg/natneg.h>
#include <serverbrowsing/sb_internal.h>
#include <base/dwc_report.h>
#include <base/dwc_account.h>
#include <base/dwc_error.h>
#include <base/dwc_memfunc.h>
#include <base/dwc_login.h>
#include <base/dwc_friend.h>
#include <base/dwc_match.h>
#include <base/dwc_transport.h>
#include <base/dwc_main.h>
#include <base/dwc_common.h>

//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------
// マッチメイクコマンド追加送信データ最大サイズと最大要素数
#define DWC_MATCH_COMMAND_ADD_MESSAGE_MAX 512
#define DWC_MATCH_COMMAND_ADD_MESSAGE_NUM (DWC_MATCH_COMMAND_ADD_MESSAGE_MAX >> 2)

// [arakit] main 051024
// マッチメイク予約拒否理由
#define DWC_MATCH_RESV_DENY_REASON_SV_FULL 0x10  // サーバが定員オーバー


//----------------------------------------------------------------------------
// enum
//----------------------------------------------------------------------------
// マッチメイクキャンセル状態列挙子
enum {
    DWC_MATCH_CANCEL_STATE_INIT = 0,  // 初期状態
    DWC_MATCH_CANCEL_STATE_EXEC,      // キャンセル処理実行中（キャンセル側）
    DWC_MATCH_CANCEL_STATE_NUM
};

// マッチメイククローズ状態列挙子
enum {
    DWC_MATCH_CLOSE_STATE_INIT = 0,          // 初期状態
    DWC_MATCH_CLOSE_STATE_CHANGE_TO_CLIENT,  // クライアントへの転向のためクローズ
    DWC_MATCH_CLOSE_STATE_TIMEOUT,           // 応答無しホストを検知したためクローズ
    DWC_MATCH_CLOSE_STATE_FORCED,            // 他ホストからクローズされたため残りホストをクローズ
    DWC_MATCH_CLOSE_STATE_NUM
};

// DWCi_PostProcessConnection()に渡す処理の種類
typedef enum {
    DWC_PP_CONNECTION_SV_CONNECT = 0,     // サーバ自身のgt2Connect完了、またはクライアント同士のコネクション完了
    DWC_PP_CONNECTION_CL_GT2_CONNECT,     // 接続済みクライアントのgt2Connect完了
    DWC_PP_CONNECTION_CL_GT2_ACCEPT,      // 新規接続クライアントのgt2Accept完了
    DWC_PP_CONNECTION_CL_FINISH_CONNECT,  // 新規接続クライアントの接続済みクライアントへの接続全て完了
    DWC_PP_CONNECTION_SYN_FINISH,         // マッチメイク完了同期調整終了
    DWC_PP_CONNECTION_NUM
} DWCMatchPpConnectionType;

// マッチメイク制御パラメータのリセットレベル。DWCi_ResetMatchParam()に渡す
typedef enum {
    DWC_MATCH_RESET_ALL = 0,   // マッチメイクを初期化する
    DWC_MATCH_RESET_RESTART,   // 最初からマッチメイクをやり直す
    DWC_MATCH_RESET_CONTINUE,  // 引き続き同じ条件でマッチメイク継続
    DWC_MATCH_RESET_NUM
} DWCMatchResetLevel;


//----------------------------------------------------------------------------
// variable
//----------------------------------------------------------------------------
// マッチメイク制御オブジェクトへのポインタ
// ソースの下へ static DWCMatchControl* stpMatchCnt = NULL;

// SBサーバアップデート用フィルタへのポインタ
static char* stpAddFilter = NULL;

// 指定人数以下でもマッチメイクを完了するオプションの制御構造体へのポインタ
static DWCMatchOptMinCompleteIn* stpOptMinComp = NULL;

// サーバクライアントマッチメイクで、１台マッチメイク完了後に他の接続を
// 受け付けなくするオプションの制御構造体
static DWCMatchOptSCBlock stOptSCBlock;

// ゲーム定義のQR2用キーデータ配列
// [todo]
// ゲームで必要な分だけメモリを確保してもらうように変更する
static DWCGameMatchKeyData stGameMatchKeys[DWC_QR2_GAME_RESERVED_KEYS];

// サーバ評価を行なう場合の各サーバの選ばれやすさの割合
static const stEvalRate[DWC_SB_UPDATE_MAX_SERVERS] = { 3, 3, 2, 2, 1, 1, };
// サーバーブラウザーオブジェクト解放遅延のための対処
static int s_sbCallbackLevel = 0;
static BOOL s_needSbFree = FALSE;

// NNオブジェクト解放遅延のための対処
static BOOL s_bInNNThink = FALSE;
static BOOL s_bNNFreeNegotiateList = FALSE;

void DWCi_NNFreeNegotiateList()
{
    if(s_bInNNThink == FALSE)
    {
        NNFreeNegotiateList();
    }
    else
    {
        s_bNNFreeNegotiateList = TRUE;
    }
}

#if 0
// for test
int DWCi_connect_errorcount = 0;
#endif

//----------------------------------------------------------------------------
// prototype
//----------------------------------------------------------------------------
static void DWCi_ResetMatchParam(DWCMatchResetLevel level);
static void DWCi_SetMatchCommonParam(u8  matchType,
                                     u8  numEntry,
                                     DWCMatchedSCCallback callback,
                                     void* param);

static void DWCi_CloseMatching(void);

static SBError DWCi_SBUpdateAsync(int profileID);
static int DWCi_GetDefaultMatchFilter(char* filter, int profileID, u8 numEntry, u8 matchType);

static NegotiateError DWCi_NNStartupAsync(int isQR2, int cookie, SBServer server);
static NegotiateError DWCi_DoNatNegotiationAsync(DWCNNInfo* nnInfo);

static int  DWCi_SendMatchCommand(u8 command, int profileID, u32 ip, u16 port, const u32 data[], int len);
static SBError DWCi_SendSBMsgCommand(u8 command, u32 ip, u16 port, const u32 data[], int len);
static GPResult DWCi_SendGPBuddyMsgCommand(GPConnection* connection, u8 command, int profileID, const char* message);
static int  DWCi_GetGPBuddyAdditionalMsg(char* dstMsg, const char* srcMsg, int index);
static void DWCi_StopResendingMatchCommand(void);

static BOOL DWCi_ProcessRecvMatchCommand(u8  command,
                                         int srcPid,
                                         u32 srcIP,
                                         u16 srcPort,
                                         const u32 data[],
                                         int len);
static u8   DWCi_CheckResvCommand(int profileID, u32 qr2IP, u16 qr2Port, u32 matchType, BOOL priorFlag);
static int  DWCi_ProcessResvOK(int profileID, u32 ip, u16 port);
static void DWCi_MakeBackupServerData(int profileID, const u32 data[]);

static int  DWCi_HandleMatchCommandError(int error);
static int  DWCi_SendResvCommand(int profileID, BOOL delay);
static int  DWCi_SendResvCommandToFriend(BOOL delay, BOOL init, int resendPid);
static BOOL DWCi_RetryReserving(int resendPid);

static int  DWCi_CancelReservation(int profileID);
static BOOL DWCi_CancelPreConnectedServerProcess(int clientPid);
static BOOL DWCi_CancelPreConnectedClientProcess(int serverPid);

static int  DWCi_ChangeToClient(void);

static void DWCi_PostProcessConnection(DWCMatchPpConnectionType type);

static BOOL DWCi_AreAllBuddies(const u32 pidList[], u32 pidListLen);

static void DWCi_DoCancelMatching(void);
static void DWCi_FinishCancelMatching(void);
static int  DWCi_InvalidateReservation(void);
static void DWCi_RestartFromCancel(DWCMatchResetLevel level);
static void DWCi_RestartFromTimeout(void);
static int  DWCi_ResumeMatching(void);
static BOOL DWCi_CloseCancelHostAsync(int profileID);
static void DWCi_CloseAllConnectionsByTimeout(void);
static BOOL DWCi_CloseShutdownClientSC(u32 bitmap);

static void DWCi_SendMatchSynPacket(u8 aid, u16 type);
static BOOL DWCi_ProcessMatchSynTimeout(void);

static BOOL DWCi_SendCancelMatchSynCommand(int profileID, u8 command);
static BOOL DWCi_ProcessCancelMatchSynCommand(int profileID, u8 command, u32 data);
static BOOL DWCi_ProcessCancelMatchSynTimeout(void);

static u8   DWCi_GetNewMatchKey(void);
static void DWCi_ClearGameMatchKeys(void);

static u8   DWCi_GetAIDFromList(void);
static BOOL DWCi_IsFriendByIdxList(int profileID);
static int  DWCi_CheckDWCServer(SBServer server);

static int  DWCi_GetPidListIndex(int profileID, BOOL idx0);
static u8   DWCi_GetAIDFromProfileID(int profileID, BOOL idx0);
static u32  DWCi_GetAIDBitmask(BOOL valid);

static void DWCi_InitClWaitTimeout(void);

static void DWCi_InitOptMinCompParam(BOOL reset);
static void DWCi_ProcessOptMinComp(void);

static GPResult DWCi_HandleGPError(GPResult result);
static SBError DWCi_HandleSBError(SBError error);
static qr2_error_t DWCi_HandleQR2Error(qr2_error_t error);
static NegotiateError DWCi_HandleNNError(NegotiateError error);
static NegotiateResult DWCi_HandleNNResult(NegotiateResult result);
static GT2Result DWCi_HandleGT2Error(GT2Result result);

static void DWCi_SBCallback(ServerBrowser sb, SBCallbackReason reason, SBServer server, void *instance);
static void DWCi_SBPrintServerData(SBServer server);
static BOOL DWCi_EvaluateServers(BOOL sort);
static void DWCi_RandomizeServers(void);

static void DWCi_QR2ServerKeyCallback(int keyid, qr2_buffer_t outbuf, void *userdata);
static void DWCi_QR2PlayerKeyCallback(int keyid, int index, qr2_buffer_t outbuf, void *userdata);
static void DWCi_QR2TeamKeyCallback(int keyid, int index, qr2_buffer_t outbuf, void *userdata);
static void DWCi_QR2KeyListCallback(qr2_key_type keytype, qr2_keybuffer_t keybuffer, void *userdata);
static int DWCi_QR2CountCallback(qr2_key_type keytype, void *userdata);
static void DWCi_QR2AddErrorCallback(qr2_error_t error, gsi_char *errmsg, void *userdata);
static void DWCi_QR2PublicAddrCallback(unsigned int ip, unsigned short port, void* userdata);
static void DWCi_QR2NatnegCallback(int cookie, void *userdata);
static void DWCi_QR2ClientMsgCallback(gsi_char* data, int len, void* userdata);

static void DWCi_NNProgressCallback(NegotiateState state, void* userdata);
static void DWCi_NNCompletedCallback(NegotiateResult result, SOCKET gamesocket, struct sockaddr_in* remoteaddr, void* userdata);
static BOOL DWCi_ProcessNNFailure(BOOL ignoreError);

static void DWCi_SetMatchCnt(DWCMatchControl *pCnt);
static DWCMatchControl *DWCi_GetMatchCnt(void);
static void DWCi_SetMatchStatus(DWCMatchState state);

//#define NITRODWC_DEBUG

#ifdef NITRODWC_DEBUG
static void disp_time(void);
static void disp_match_state(DWCMatchState old, DWCMatchState now);
static const char *get_dwccommandstr(u8 command);
#endif
//----------------------------------------------------------------------------
// function - external
//----------------------------------------------------------------------------
#ifdef DWC_QR2_ALIVE_DURING_MATCHING
/*---------------------------------------------------------------------------*
  マッチメイクステータスのマスターサーバ登録関数（友達指定、無指定ピアマッチメイク用）
  引数　：なし
  戻り値：TRUE :マスターサーバにマッチメイクステータスが登録された、
          FALSE:ログイン前、もしくはエラーのため登録されなかった
  用途　：自ホストのマッチメイクステータスをマスターサーバに登録する。
          この関数が呼ばれてから最短15秒程で、他ホストから自ホストの状態が
          見えるようになり、マッチメイクを進められるようになる。
          サーバクライアントマッチメイク以外では、マッチメイクが終わった時点で
          マスターサーバからこのマッチメイクステータスが消去される。
          マッチメイク開始関数を呼べば、ライブラリ内でこの関数が呼ばれるが、
          マッチメイクの終了を早める為に、予めマッチメイクステータスを
          マスターサーバに登録しておきたい場合は、この関数を呼び出せばよい。
          また、マッチメイクステータスはDWC_ShutdownFriendsMatch()を呼んだ時にも
          消去される。
 *---------------------------------------------------------------------------*/
BOOL DWC_RegisterMatchingStatus(void)
{

    // [arakit] main 051013
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "!!DWC_RegisterMatchingStatus() was called!!\n");

    if ((DWCi_GetMatchCnt() == NULL) || (DWCi_GetMatchCnt()->profileID == 0) || DWCi_IsError()){
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "But ignored.\n");
        return FALSE;
    }

    if (DWCi_QR2Startup(DWCi_GetMatchCnt()->profileID)) return FALSE;
    return TRUE;
}
#endif


/*---------------------------------------------------------------------------*
  マッチメイク処理キャンセル関数
  引数　：なし
  戻り値：TRUE:キャンセル処理を実行した、FALSE:マッチメイク中でない
  用途　：進行中のマッチメイク処理をキャンセルする。
          キャンセル処理は本関数内で完了し、マッチメイク完了コールバックが
          呼び出される。
 *---------------------------------------------------------------------------*/
BOOL DWC_CancelMatching(void)
{

    if (DWC_IsValidCancelMatching()){
        // キャンセル処理を行う
        DWCi_DoCancelMatching();
        return TRUE;
    }
    else {
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Now unable to cancel.\n");
        return FALSE;
    }
}


/*---------------------------------------------------------------------------*
  マッチメイク処理キャンセル関数 非同期版 (obsolete function)
  引数　：なし
  戻り値：TRUE:キャンセル処理の実行を開始した、FALSE:マッチメイク中でない
  用途　：進行中のマッチメイク処理をキャンセルする。
          必要なキャンセル処理を行った後、マッチメイク完了コールバックが
          呼び出される。
          →DWC_CancelMatching()を使って下さい
 *---------------------------------------------------------------------------*/
BOOL DWC_CancelMatchingAsync(void)
{

    if (DWC_IsValidCancelMatching()){
        // キャンセル処理の予約状態に進む
        DWCi_DoCancelMatching();
        return TRUE;
    }
    else {
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Now unable to cancel.\n");
        return FALSE;
    }
}


/*---------------------------------------------------------------------------*
  マッチメイクキャンセル有効確認関数
  引数　：なし
  戻り値：TRUE:キャンセル可能、FALSE:マッチメイク中でなかったり、中断できない
          状況でキャンセル不可能。
  用途　：進行中のマッチメイク処理をキャンセルできるかどうかを調べる
 *---------------------------------------------------------------------------*/
BOOL DWC_IsValidCancelMatching(void)
{

    if (DWCi_IsError()) return FALSE;

    if (!DWCi_GetMatchCnt() ||
        (DWC_GetState() != DWC_STATE_MATCHING) ||
        (DWCi_GetMatchCnt()->cancelState != DWC_MATCH_CANCEL_STATE_INIT)){
        return FALSE;
    }
    else {
        return TRUE;
    }
}


#ifdef DWC_STOP_SC_SERVER
/*---------------------------------------------------------------------------*
  サーバクライアントマッチメイク サーバ締め切り関数
  引数　：callback 締め切り完了コールバック
          param    コールバック用パラメータ
  戻り値：TRUE :マッチメイクの締め切り開始。完了したらコールバックが返ってくる。
          FALSE:エラー発生、FriendsMatchライブラリ非動作中などの理由で締め切り失敗。
  用途　：サーバクライアントマッチメイクのマッチメイクを締め切り、以後新規の
          接続を受け付けないようにする。
 *---------------------------------------------------------------------------*/
BOOL DWC_StopSCMatchingAsync(DWCStopSCCallback callback, void* param)
{

    if (DWCi_IsError() || !DWCi_GetMatchCnt() ||
        (DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_SC_SV) ||
        ((DWC_GetState() != DWC_STATE_MATCHING) && (DWC_GetState() != DWC_STATE_CONNECTED)))
        return FALSE;

    if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_WAITING){
        // マッチメイク中でなければ最大接続人数を現在接続人数に合わせるだけで
        // 以後の接続を拒否できる
        DWCi_GetMatchCnt()->qr2NumEntry = DWCi_GetMatchCnt()->gt2NumConnection;
        (void)DWCi_GPSetServerStatus();

        if (callback) callback(param);  // コールバック呼び出し
    }
    else {
        DWCi_GetMatchCnt()->stopSCCallback = callback;
        DWCi_GetMatchCnt()->stopSCParam    = param;
        DWCi_GetMatchCnt()->stopSCFlag     = 1;
    }

    return TRUE;
}
#endif


/*---------------------------------------------------------------------------*
  マッチメイク指標キー追加関数 int版
  引数　：keyID        キーIDを指定してセットしたい場合はここにキーIDを渡す。
                       新規にkeyIDを取得したければ DWC_QR2_GAME_KEY_START
                       以下の値（0など）をセットすれば良い。
                       既に使用中の追加キーIDを指定した場合は、使用中のものと
                       同じkeyStringを渡せばvalueSrcをセットし直すが、
                       違うkeyStringを渡した場合は何もしない。
          keyString    キー識別のための文字列へのポインタ。
                       文字列はコピーしてライブラリ内で保持します。
          valueSrc     キーに対応する値へのポインタで、マスターサーバからの
                       ポーリングがあると、このポインタから値を参照して
                       マスターサーバに値をアップする。
  戻り値：追加したキーのキーID。パラメータエラー、もしくはキーの設定上限を
          越えたため追加できなかった場合は0を返す。
          DWC_InitFriendsMatch()が呼ばれるより前に呼び出しても0を返す。
  用途　：マッチメイクの指標となるキー（int型）を追加する。
 *---------------------------------------------------------------------------*/
u8  DWC_AddMatchKeyInt(u8 keyID,
                       const char* keyString,
                       const int* valueSrc)
{
    int index;

    if (!keyString || !valueSrc) return 0;

    if ((keyID >= DWC_QR2_GAME_KEY_START) &&
        stGameMatchKeys[keyID-DWC_QR2_GAME_KEY_START].keyID){
        // 既に定義されているゲーム定義キーIDが指定された場合
        if ((stGameMatchKeys[keyID-DWC_QR2_GAME_KEY_START].keyStr != NULL) &&
            (strcmp(stGameMatchKeys[keyID-DWC_QR2_GAME_KEY_START].keyStr, keyString) != 0)){
            // 既に定義されているものとキー名が違う場合は何もしない
            return 0;
        }
        // 既に定義されているものとキー名が同じ場合は、処理を進めて
        // ポインタの差し替えを行なう
    }
    else {
        keyID = DWCi_GetNewMatchKey();  // 空きキーIDを取得
        if (!keyID) return 0;
    }

    // 新規キーデータを登録する
    index = keyID-DWC_QR2_GAME_KEY_START;
    stGameMatchKeys[index].keyID  = keyID;
    stGameMatchKeys[index].isStr  = 0;
    stGameMatchKeys[index].pad    = 0;

    // 万が一前のキー名が残っていたら解放する
    if (stGameMatchKeys[index].keyStr)
        DWC_Free(DWC_ALLOCTYPE_BASE, stGameMatchKeys[index].keyStr, 0);

    // キー名のための領域をALLOCして文字列をセットする
    stGameMatchKeys[index].keyStr = DWC_Alloc(DWC_ALLOCTYPE_BASE, strlen(keyString)+1);
    if (!stGameMatchKeys[index].keyStr){
        DWCi_StopMatching(DWC_ERROR_FATAL, DWC_ECODE_SEQ_MATCH+DWC_ECODE_TYPE_ALLOC);
        return 0;
    }

    strcpy(stGameMatchKeys[index].keyStr, keyString);

    DWC_Printf(DWC_REPORTFLAG_DEBUG,
               "DWC_AddMatchKeyInt: key='%s', value=%d\n",
               keyString, *valueSrc);
    
    stGameMatchKeys[index].value  = valueSrc;

    // カスタムキーを設定する
    qr2_register_key(keyID, stGameMatchKeys[index].keyStr);

    return keyID;
}


/*---------------------------------------------------------------------------*
  マッチメイク指標キー追加関数 string版
  引数　：keyID        キーIDを指定してセットしたい場合はここにキーIDを渡す。
                       新規にkeyIDを取得したければ DWC_QR2_GAME_KEY_START
                       以下の値（0など）をセットすれば良い。
                       既に使用中の追加キーIDを指定した場合は、使用中のものと
                       同じkeyStringを渡せばvalueSrcをセットし直すが、
                       違うkeyStringを渡した場合は何もしない。
          keyString    キー識別のための文字列へのポインタ。
                       文字列はコピーしてライブラリ内で保持します。
          valueSrc     キーに対応する文字列へのポインタで、マスターサーバからの
                       ポーリングがあると、このポインタから文字列を参照して
                       マスターサーバに文字列をアップする。
  戻り値：追加したキーのキーID。パラメータエラー、もしくはキーの設定上限を
          越えたため追加できなかった場合は0を返す。
          DWC_InitFriendsMatch()が呼ばれるより前に呼び出しても0を返す。
  用途　：マッチメイクの指標となるキー（char*型）を追加する。
 *---------------------------------------------------------------------------*/
u8  DWC_AddMatchKeyString(u8 keyID,
                          const char* keyString,
                          const char* valueSrc)
{
    int index;

    if (!keyString || !valueSrc) return 0;

    if ((keyID >= DWC_QR2_GAME_KEY_START) &&
        stGameMatchKeys[keyID-DWC_QR2_GAME_KEY_START].keyID){
        // 既に定義されているゲーム定義キーIDが指定された場合
        if ((stGameMatchKeys[keyID-DWC_QR2_GAME_KEY_START].keyStr != NULL) &&
            (strcmp(stGameMatchKeys[keyID-DWC_QR2_GAME_KEY_START].keyStr, keyString) != 0)){
            // 既に定義されているものとキー名が違う場合は何もしない
            return 0;
        }
        // 既に定義されているものとキー名が同じ場合は、処理を進めて
        // ポインタの差し替えを行なう
    }
    else {
        keyID = DWCi_GetNewMatchKey();  // 空きキーIDを取得
        if (!keyID) return 0;
    }

    // 新規キーデータを登録する
    index = keyID-DWC_QR2_GAME_KEY_START;
    stGameMatchKeys[index].keyID      = keyID;
    stGameMatchKeys[index].isStr      = 1;
    stGameMatchKeys[index].pad        = 0;

    // 万が一前のキー名が残っていたら解放する
    if (stGameMatchKeys[index].keyStr)
        DWC_Free(DWC_ALLOCTYPE_BASE, stGameMatchKeys[index].keyStr, 0);

    // キー名のための領域をALLOCして文字列をセットする
    stGameMatchKeys[index].keyStr = DWC_Alloc(DWC_ALLOCTYPE_BASE, strlen(keyString)+1);
    if (!stGameMatchKeys[index].keyStr){
        DWCi_StopMatching(DWC_ERROR_FATAL, DWC_ECODE_SEQ_MATCH+DWC_ECODE_TYPE_ALLOC);
        return 0;
    }

    strcpy(stGameMatchKeys[index].keyStr, keyString);

    DWC_Printf(DWC_REPORTFLAG_DEBUG,
               "DWC_AddMatchKeyString: key='%s' value='%s'\n",

               keyString, valueSrc);
    
    stGameMatchKeys[index].value      = valueSrc;

    // カスタムキーを設定する
    qr2_register_key(keyID, stGameMatchKeys[index].keyStr);

    return keyID;
}


/*---------------------------------------------------------------------------*
  マッチメイク指標キー取得関数 int版
  引数　：index     マッチメイク候補プレイヤーのインデックス。評価コールバックの
                    引数 index をそのまま渡す。
          keyString キー識別のための文字列へのポインタ
          idefault  指定したキーを持っていないプレイヤーだった場合の
                    デフォルト値
  戻り値：マッチメイク候補プレイヤーが持つ、指定したキーに対応する値。
          キーがない場合は引数 idefault に渡した値が返ってくる。
  用途　：マッチメイク指標キーに対応する値を取得する。
          プレイヤー評価コールバック内での使用のみ保証しており、
          それ以外の場所で呼び出した場合の戻り値は不定。
 *---------------------------------------------------------------------------*/
int DWC_GetMatchIntValue(int index, const char* keyString, int idefault)
{
    SBServer server;

    if (!DWCi_GetMatchCnt() || DWCi_IsError()) return idefault;

    server = ServerBrowserGetServer(DWCi_GetMatchCnt()->sbObj, index);

    if (!server) return idefault;
    
    return SBServerGetIntValue(server, keyString, idefault);
}


/*---------------------------------------------------------------------------*
  マッチメイク指標キー取得関数 string版
  引数　：index     マッチメイク候補プレイヤーのインデックス。評価コールバックの
                    引数 index をそのまま渡す。
          keyString キー識別のための文字列へのポインタ
          sdefault  指定したキーを持っていないプレイヤーだった場合の
                    デフォルト文字列
  戻り値：マッチメイク候補プレイヤーが持つ、指定したキーに対応する文字列。
          キーがない場合は引数 sdefault に渡した文字列が返ってくる。
  用途　：マッチメイク指標キーに対応する文字列を取得する。
          プレイヤー評価コールバック内での使用のみ保証しており、
          それ以外の場所で呼び出した場合の戻り値は不定。
 *---------------------------------------------------------------------------*/
const char* DWC_GetMatchStringValue(int index, const char* keyString, const char* sdefault)
{
    SBServer server;

    if (!DWCi_GetMatchCnt() || DWCi_IsError()) return sdefault;

    server = ServerBrowserGetServer(DWCi_GetMatchCnt()->sbObj, index);

    if (!server) return sdefault;
    
    return SBServerGetStringValue(server, keyString, sdefault);
}


/*---------------------------------------------------------------------------*
  マッチメイクタイプ取得関数
  引数　：なし
  戻り値：最後に行ったマッチメイク種別。DWC_MATCH_TYPE_*で定義される。
          オフライン状態なら-1を返す。
  用途　：最後に行ったマッチメイク種別を取得する。
 *---------------------------------------------------------------------------*/
int DWC_GetLastMatchingType(void)
{

    if (DWCi_GetMatchCnt()) return DWCi_GetMatchCnt()->qr2MatchType;
    else return -1;
}


/*---------------------------------------------------------------------------*
  マッチメイクオプションセット関数
  引数　：opttype マッチメイクオプションの型
          optval  マッチメイクオプションの値へのポインタ。
                  opttypeに対応した型のポインタを渡す。
          optlen  型が配列の場合のみ、その配列のサイズを渡す必要がある。
  戻り値：DWC_SET_MATCH_OPT_RESULT_*型の列挙子
  用途　：マッチメイクについての様々なオプションを設定する。
 *---------------------------------------------------------------------------*/
int DWC_SetMatchingOption(DWCMatchOptType opttype, const void* optval, int optlen)
{
#pragma unused(optlen)
    DWCMatchOptMinComplete* pMinComp;

    // [arakit] main 051013
    DWC_Printf(DWC_REPORTFLAG_DEBUG,
               "!!DWC_SetMatchingOption() was called!! type %d\n",
               opttype);

    if (!DWCi_GetMatchCnt()) return DWC_SET_MATCH_OPT_RESULT_E_BAD_STATE;
    if (!optval) return DWC_SET_MATCH_OPT_RESULT_E_PARAM;
    
    switch (opttype){
    case DWC_MATCH_OPTION_MIN_COMPLETE:
        // 指定人数が集まらなくても一定時間でマッチメイク完了にするオプション。
        // optvalは DWCMatchOptMinComplete構造体で受け取る。
        if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_POLL_TIMEOUT){
            // 丁度タイムアウトポーリングの処理中に呼んでしまった場合
            return DWC_SET_MATCH_OPT_RESULT_E_BAD_STATE;
        }
        
        pMinComp = (DWCMatchOptMinComplete *)optval;
        // 最低人数1以下、タイムアウト0はパラメータエラー
        if (pMinComp->valid && (pMinComp->minEntry <= 1))
            return DWC_SET_MATCH_OPT_RESULT_E_PARAM;

        if (!stpOptMinComp){
            // 構造体の実体がなければAlloc
            stpOptMinComp =
                DWC_Alloc(DWC_ALLOCTYPE_BASE, sizeof(DWCMatchOptMinCompleteIn));
            if (!stpOptMinComp) return DWC_SET_MATCH_OPT_RESULT_E_ALLOC;
        }

        stpOptMinComp->valid        = pMinComp->valid;
        stpOptMinComp->minEntry     = pMinComp->minEntry;
        stpOptMinComp->retry        = 0;
        stpOptMinComp->pad          = 0;
        stpOptMinComp->timeout      = pMinComp->timeout;
        stpOptMinComp->recvBit      = 0;
        stpOptMinComp->timeoutBit   = 0;
        stpOptMinComp->startTime    = OS_GetTick();
        stpOptMinComp->lastPollTime = OS_GetTick();
        return DWC_SET_MATCH_OPT_RESULT_SUCCESS;

    case DWC_MATCH_OPTION_SC_CONNECT_BLOCK:
        // サーバクライアントマッチメイクで、１台マッチメイク完了後に
        // 他の接続を受け付けなくするオプション。
        // optvalはBOOLで受け取る。
        // マッチメイク開始前に設定する必要があるので、サーバクライアント
        // マッチメイクでないと設定できない、という判定は行わない。
        if (*(BOOL *)optval) stOptSCBlock.valid = 1;
        else                 stOptSCBlock.valid = 0;
        stOptSCBlock.lock  = 0;
        return DWC_SET_MATCH_OPT_RESULT_SUCCESS;

    default:
        return DWC_SET_MATCH_OPT_RESULT_E_INVALID;
    }
}


/*---------------------------------------------------------------------------*
  マッチメイクオプション取得関数
  引数　：opttype マッチメイクオプションの型
          optval  マッチメイクオプションの値格納先ポインタ。
                  opttypeに対応した型のポインタを渡す。
          optlen  取得したオプションの値のサイズ格納先
  戻り値：DWC_SET_MATCH_OPT_RESULT_*型の列挙子
  用途　：マッチメイクについての設定中のオプションを取得する。
          指定したオプションが設定されていない場合は、何もせずに、
          引数optlenに0を格納して返す。
 *---------------------------------------------------------------------------*/
int DWC_GetMatchingOption(DWCMatchOptType opttype, void* optval, int* optlen)
{

    if (!DWCi_GetMatchCnt()) return DWC_SET_MATCH_OPT_RESULT_E_BAD_STATE;
    if (!optval) return DWC_SET_MATCH_OPT_RESULT_E_PARAM;
    
    switch (opttype){
    case DWC_MATCH_OPTION_MIN_COMPLETE:
        // 指定人数が集まらなくても一定時間でマッチメイク完了にするオプション
        if (!stpOptMinComp){
            if (optlen) *optlen = 0;
        }
        else {
            DWCMatchOptMinComplete* pVal = optval;
            pVal->valid    = stpOptMinComp->valid;
            pVal->minEntry = stpOptMinComp->minEntry;
            pVal->timeout  = stpOptMinComp->timeout;
            pVal->pad[0] = pVal->pad[1] = 0;
            if (optlen) *optlen = sizeof(DWCMatchOptMinComplete);
        }
        return DWC_SET_MATCH_OPT_RESULT_SUCCESS;

    case DWC_MATCH_OPTION_SC_CONNECT_BLOCK:
        // サーバクライアントマッチメイクで、１台マッチメイク完了後に
        // 他の接続を受け付けなくするオプション。
        // オプションが有効かどうかのみを返す。
        if (stOptSCBlock.valid == 1) *(BOOL *)optval = TRUE;
        else                         *(BOOL *)optval = FALSE;
        if (optlen) *optlen = sizeof(BOOL);
        return DWC_SET_MATCH_OPT_RESULT_SUCCESS;

    default:
        return DWC_SET_MATCH_OPT_RESULT_E_INVALID;
    }
}


/*---------------------------------------------------------------------------*
  指定人数以下マッチメイク完了オプション状態取得関数
  引数　：time マッチメイク開始からの経過時間（単位：msec）格納先ポインタ。
               時間が必要なければNULLを渡せば良い。
  戻り値：0 :タイムアウト時間内
          1 :タイムアウト時間を越えており、指定人数以下でマッチメイク完了する可能性あり
          -1:このマッチメイクオプションが設定されていない、もしくはOFFになっている
  用途　：指定人数以下マッチメイク完了オプションの時間経過状況を取得する
 *---------------------------------------------------------------------------*/
int DWC_GetMOMinCompState(u64* time)
{
    u64 passTime;

    if (!stpOptMinComp || !stpOptMinComp->valid) return -1;

    passTime = OS_TicksToMilliSeconds(OS_GetTick()-stpOptMinComp->startTime);
    if (time) *time = passTime;  // 経過時間をセット

    if (passTime >= stpOptMinComp->timeout) return 1;
    else return 0;
}


/*---------------------------------------------------------------------------*
  マッチメイクブロックオプション状態取得関数
  引数　：なし
  戻り値：TRUE :新規接続クライアントをブロック中、
          FALSE:ブロック中でないか、オプションが設定されていない
  用途　：マッチメイクブロックオプションのブロック状態を取得する
 *---------------------------------------------------------------------------*/
BOOL DWC_GetMOSCConnectBlockState(void)
{

    if ((stOptSCBlock.valid == 0) || (stOptSCBlock.lock == 0)){
        // オプションが設定されていないか、ブロック中でなければFALSEを返す
        return FALSE;
    }
    else {
        return TRUE;
    }
}


/*---------------------------------------------------------------------------*
  マッチメイクブロック解除関数
  引数　：なし
  戻り値：なし
  用途　：マッチメイクブロックオプションのブロック状態を解除する
 *---------------------------------------------------------------------------*/
void DWC_ClearMOSCConnectBlock(void)
{

    // 新規接続のブロックを解除する
    stOptSCBlock.lock = 0;

    DWC_Printf(DWC_REPORTFLAG_DEBUG, "[OPT_SC_BLOCK] ClearMOSCConnectBlock\n");
}


/*---------------------------------------------------------------------------*
  マッチメイク内部状態取得関数
  引数　：なし
  戻り値：現在のマッチメイク内部状態
  用途　：マッチメイク中の内部状態を取得する。デバッグ中にマッチメイクの内部状態を
          知りたい場合にのみ使用する。
 *---------------------------------------------------------------------------*/
DWCMatchState DWC_GetMatchingState(void)
{

    if (DWCi_GetMatchCnt()){
        return DWCi_GetMatchCnt()->state;
    }
    else {
        return DWC_MATCH_STATE_INIT;
    }
}


//----------------------------------------------------------------------------
// function - internal
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  マッチメイク制御構造体初期化関数
  引数　：matchcnt      マッチメイク制御オブジェクトへのポインタ
          pGpObj        GPコネクション構造体へのポインタ
          pGt2Socket    gt2ソケット構造体へのポインタ
          gt2Callbacks  GT2コールバック集へのポインタ
          gameName      GameSpyから与えられるゲーム名
          secretKey     GameSpyから与えられるシークレットキー
          friendList    友達リスト（ゲームで使用しない場合はNULLでも良い）
          friendListLen 友達リスト長
  戻り値：なし
  用途　：マッチメイク制御構造体を初期化する
 *---------------------------------------------------------------------------*/
void DWCi_MatchInit(DWCMatchControl* matchcnt,
                    GPConnection pGpObj,
                    GT2Socket* pGt2Socket,
                    GT2ConnectionCallbacks* gt2Callbacks,
                    const char* gameName,
                    const char* secretKey,
                    const DWCFriendData friendList[],
                    int  friendListLen)
{

    SDK_ASSERT(matchcnt);

	DWCi_SetMatchCnt(matchcnt); //stpMatchCnt = matchcnt;

    DWCi_GetMatchCnt()->pGpObj            = pGpObj;
    DWCi_GetMatchCnt()->pGt2Socket        = pGt2Socket;
    DWCi_GetMatchCnt()->gt2Callbacks      = gt2Callbacks;
    DWCi_GetMatchCnt()->qr2Obj            = NULL;
    DWCi_GetMatchCnt()->qr2IP             = 0;
    DWCi_GetMatchCnt()->qr2Port           = 0;
    DWCi_GetMatchCnt()->sbObj             = NULL;
    
    DWCi_SetMatchStatus(DWC_MATCH_STATE_INIT);
    DWCi_GetMatchCnt()->pad1              = 0;
    DWCi_GetMatchCnt()->pad2              = 0;
#ifdef DWC_STOP_SC_SERVER
    DWCi_GetMatchCnt()->pad3              = 0;
    DWCi_GetMatchCnt()->pad4              = 0;
#endif
#ifdef DWC_LIMIT_FRIENDS_MATCH_VALID
    DWCi_GetMatchCnt()->friendAcceptBit   = 0;
#endif
    DWCi_GetMatchCnt()->profileID         = 0;
    DWCi_GetMatchCnt()->gameName          = gameName;
    DWCi_GetMatchCnt()->secretKey         = secretKey;

    DWCi_GetMatchCnt()->friendList        = friendList;
    DWCi_GetMatchCnt()->friendListLen     = friendListLen;
    MI_CpuClear8(DWCi_GetMatchCnt()->friendIdxList, DWC_MAX_MATCH_IDX_LIST);
    DWCi_GetMatchCnt()->friendIdxListLen  = 0;

    DWCi_GetMatchCnt()->matchedCallback   = NULL;
    DWCi_GetMatchCnt()->matchedParam      = NULL;
    DWCi_GetMatchCnt()->evalCallback      = NULL;
    DWCi_GetMatchCnt()->evalParam         = NULL;
#ifdef DWC_STOP_SC_SERVER
    DWCi_GetMatchCnt()->stopSCCallback    = NULL;
    DWCi_GetMatchCnt()->stopSCParam       = NULL;
#endif

    // [nakata] ServerBrowserLimitUpdate呼び出しTickを初期化
    DWCi_GetMatchCnt()->sbUpdateRequestTick = 0;
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "ServerBrowserLimitUpdate timeout reset.(%s)\n", __FUNCTION__);

    // ゲーム定義のQR2用キーデータ配列を初期化
    DWCi_ClearGameMatchKeys();

    stOptSCBlock.valid = 0;
    stOptSCBlock.lock  = 0;
    stOptSCBlock.pad   = 0;

    // マッチメイクごとに初期化が必要なパラメータはこちらでまとめて初期化
    DWCi_ResetMatchParam(DWC_MATCH_RESET_ALL);
}


/*---------------------------------------------------------------------------*
  QR2起動関数
  引数　：profileID 自分のプロファイルID
  戻り値：QR2エラー種別
  用途　：QR2を起動する。一度失敗してもリトライする。
 *---------------------------------------------------------------------------*/
qr2_error_t DWCi_QR2Startup(int profileID)
{
    int i;
    qr2_error_t qr2Error = e_qrnoerror;

    if (DWCi_GetMatchCnt()->qr2Obj){
        // QR2が既に起動されているのにここに来た場合は何もしない
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "QR2 is already set up.\n");
        return e_qrnoerror;
    }

    DWCi_GetMatchCnt()->profileID = profileID;

    for (i = 0; i < DWC_DNS_ERROR_RETRY_MAX; i++){
        // QR2初期化
        qr2Error = qr2_init_socket(&DWCi_GetMatchCnt()->qr2Obj,
                                   gt2GetSocketSOCKET(*DWCi_GetMatchCnt()->pGt2Socket),
                                   gt2GetLocalPort(*DWCi_GetMatchCnt()->pGt2Socket),
                                   DWCi_GetMatchCnt()->gameName, DWCi_GetMatchCnt()->secretKey, 1, 1,
                                   DWCi_QR2ServerKeyCallback,
                                   DWCi_QR2PlayerKeyCallback,
                                   DWCi_QR2TeamKeyCallback,
                                   DWCi_QR2KeyListCallback,
                                   DWCi_QR2CountCallback,
                                   DWCi_QR2AddErrorCallback,
                                   NULL);
        if (!qr2Error) break;
        else {
            qr2_shutdown(DWCi_GetMatchCnt()->qr2Obj);
            DWCi_GetMatchCnt()->qr2Obj = NULL;
        }

        // DNSエラーなら一定回数はリトライする
        if ((qr2Error != e_qrdnserror) || (i == DWC_DNS_ERROR_RETRY_MAX-1)){
            DWCi_HandleQR2Error(qr2Error);
            return qr2Error;
        }
    }

#ifdef DWC_QR2_ALIVE_DURING_MATCHING
    // 自分のQR2用パブリックIPとポート番号を初期化
    DWCi_GetMatchCnt()->qr2IP   = 0;
    DWCi_GetMatchCnt()->qr2Port = 0;
#endif

    // パブリックアドレス通知コールバック関数登録
    qr2_register_publicaddress_callback(DWCi_GetMatchCnt()->qr2Obj, DWCi_QR2PublicAddrCallback);
        
    // NATネゴシエーションコールバック関数登録
    qr2_register_natneg_callback(DWCi_GetMatchCnt()->qr2Obj, DWCi_QR2NatnegCallback);

    // クライアントメッセージ受信コールバック関数登録
    qr2_register_clientmessage_callback(DWCi_GetMatchCnt()->qr2Obj, DWCi_QR2ClientMsgCallback);

    // 状態の変化をマスターサーバに通知する
    qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);

    return qr2Error;
}


/*---------------------------------------------------------------------------*
  友達無指定ピアマッチメイク開始関数
  引数　：numEntry        接続要求人数（自分は含まない）
          addFilter       ゲームで追加したいSBサーバアップデート用フィルタ
          matchedCallback マッチメイク完了コールバック
          matehedParam    上記コールバック用パラメータ
          evalCallback    プレイヤー評価コールバック
          evalParam       上記コールバック用パラメータ
  戻り値：なし
  用途　：友達指定ピアマッチメイクを開始する
 *---------------------------------------------------------------------------*/
void DWCi_ConnectToAnybodyAsync(u8  numEntry,
                                const char* addFilter,
                                DWCMatchedSCCallback matchedCallback,
                                void* matchedParam,
                                DWCEvalPlayerCallback evalCallback,
                                void* evalParam)
{
    char filter[MAX_FILTER_LEN];
    u32  addFilterLenMax;
    SBError sbError;

    // 万が一前のフィルタが残っていたら解放する
    if (stpAddFilter){
        DWC_Free(DWC_ALLOCTYPE_BASE, stpAddFilter, 0);
        stpAddFilter = NULL;
    }

    // 追加フィルター長が制限文字数を越えていないかを調べる
    if (addFilter){
        // 追加フィルタの最大許容文字数（NULL終端含む）を算出
        addFilterLenMax = MAX_FILTER_LEN-DWCi_GetDefaultMatchFilter(filter, 0xffffffff, DWC_MAX_CONNECTIONS, DWC_MATCH_TYPE_SC_CL)-strlen(" and ()");

        SDK_ASSERT(strlen(addFilter) < addFilterLenMax);

        stpAddFilter = DWC_Alloc(DWC_ALLOCTYPE_BASE, addFilterLenMax);
        if (!stpAddFilter){
            DWCi_StopMatching(DWC_ERROR_FATAL, DWC_ECODE_SEQ_MATCH+DWC_ECODE_TYPE_ALLOC);
            return;
        }

        MI_CpuCopy8(addFilter, stpAddFilter, addFilterLenMax);
        stpAddFilter[addFilterLenMax-1] = '\0';  // NULL終端を保証
    }

    // マッチメイク用共通パラメータセット
    DWCi_SetMatchCommonParam(DWC_MATCH_TYPE_ANYBODY,
                             numEntry, matchedCallback, matchedParam);

    DWCi_GetMatchCnt()->evalCallback = evalCallback;
    DWCi_GetMatchCnt()->evalParam    = evalParam;

    DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SEARCH_OWN);

    // SB初期化
    if ( !DWCi_GetMatchCnt()->sbObj )
            DWCi_GetMatchCnt()->sbObj = ServerBrowserNew(DWCi_GetMatchCnt()->gameName,
                                                  DWCi_GetMatchCnt()->gameName,
                                                  DWCi_GetMatchCnt()->secretKey, 0, 20,
                                                  QVERSION_QR2, SBFalse,
                                                  DWCi_SBCallback, NULL);
    if (!DWCi_GetMatchCnt()->sbObj){
        // メモリ確保エラーと思われる
        if (DWCi_HandleSBError(sbe_allocerror)) return;
    }

    // GPステータスを友達無指定ピアマッチメイク中にセットする（エラーは無視する）
    (void)DWCi_SetGPStatus(DWC_STATUS_MATCH_ANYBODY, "", NULL);

    // SBサーバアップデートを行い、自分のデータの反映を確認する
    sbError = DWCi_SBUpdateAsync(DWCi_GetMatchCnt()->profileID);
    if (DWCi_HandleSBError(sbError)) return;

#ifdef DWC_QR2_ALIVE_DURING_MATCHING
    if (DWCi_GetMatchCnt()->qr2Obj == NULL){
        // QR2が起動されていなければここで起動する
        DWCi_QR2Startup(DWCi_GetMatchCnt()->profileID);
    }

#else
    // 状態の変化をマスターサーバに通知する
    qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
#endif

    // 指定人数以下でのマッチメイク完了オプション用初期化
    DWCi_InitOptMinCompParam(FALSE);
}


/*---------------------------------------------------------------------------*
  友達指定ピアマッチメイク開始関数
  引数　：friendIdxList    接続要求友達インデックスリストへのポインタ
          friendIdxListLen 接続要求友達インデックスリスト長
          numEntry         接続要求人数（自分は含まない）
          distantFriend    TRUE:友達の友達との接続を許す、FALSE:許さない
          matchedCallback  マッチメイク完了コールバック
          matchedParam     上記コールバック用パラメータ
          evalCallback     プレイヤー評価コールバック
          evalParam        上記コールバック用パラメータ
  戻り値：なし
  用途　：友達指定ピアマッチメイクを開始する
 *---------------------------------------------------------------------------*/
void DWCi_ConnectToFriendsAsync(const u8 friendIdxList[],
                                int friendIdxListLen,
                                u8  numEntry,
                                BOOL distantFriend,
                                DWCMatchedSCCallback matchedCallback,
                                void* matchedParam,
                                DWCEvalPlayerCallback evalCallback,
                                void* evalParam)
{
    char valueStr[12], keyValueStr[32];
#ifndef DWC_MATCH_ACCEPT_NO_FRIEND
    int  checkpid;
    int  friendCount;
    int  i;
#endif
    int  result;
    GPResult gpResult;

    // マッチメイク用共通パラメータセット
    DWCi_SetMatchCommonParam(DWC_MATCH_TYPE_FRIEND,
                             numEntry, matchedCallback, matchedParam);

    DWCi_GetMatchCnt()->distantFriend = (u8)(distantFriend ? 1 : 0);
    DWCi_GetMatchCnt()->evalCallback  = evalCallback;
    DWCi_GetMatchCnt()->evalParam     = evalParam;

    // 友達インデックスリストをコピー
#ifdef DWC_MATCH_ACCEPT_NO_FRIEND
    MI_CpuCopy8(friendIdxList, DWCi_GetMatchCnt()->friendIdxList, (u32)friendIdxListLen);
    DWCi_GetMatchCnt()->friendIdxListLen = friendIdxListLen;

    if (!friendIdxListLen ||
        ((friendIdxListLen < numEntry) &&
         !distantFriend &&
         (!stpOptMinComp || !stpOptMinComp->valid || (friendIdxListLen < stpOptMinComp->minEntry-1)))){
        // 要求する接続人数より友達の人数が少ない場合、
        // 友達の友達を許さない設定で、かつ指定人数以下マッチメイク
        // 完了オプションの最低人数よりも友達の人数が少なければ
        // 友達人数不足エラー。有効友達人数0でもエラー。
        DWCi_StopMatching(DWC_ERROR_FRIENDS_SHORTAGE, 0);
        return;
    }

#else
    for (i = 0, friendCount = 0; i < friendIdxListLen; i++){
        if (!(checkpid = DWCi_GetProfileIDFromList(friendIdxList[i])) ||
            !gpIsBuddy(DWCi_GetMatchCnt()->pGpObj, checkpid)){
            // 指定されたインデックスがプロファイルIDを持っていない、
            // もしくは相手がバディでなかった場合はリストから外す
            continue;
        }

        DWCi_GetMatchCnt()->friendIdxList[friendCount] = friendIdxList[i];
        friendCount++;
    }

    if (!friendCount ||
        ((friendCount < numEntry) &&
         !distantFriend &&
         (!stpOptMinComp || !stpOptMinComp->valid || (friendCount < stpOptMinComp->minEntry-1)))){
        // 要求する接続人数より友達の人数が少ない場合、
        // 友達の友達を許さない設定で、かつ指定人数以下マッチメイク
        // 完了オプションの最低人数よりも友達の人数が少なければ
        // 友達人数不足エラー。有効友達人数0でもエラー。
        // [todo]
        // マッチメイク中に成立したバディをインデックスリストに加えるように
        // したら、ここのエラーはやめる
        // →やめた
        DWCi_StopMatching(DWC_ERROR_FRIENDS_SHORTAGE, 0);
        return;
    }

    DWCi_GetMatchCnt()->friendIdxListLen = friendCount;
#endif
    
    DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAIT_RESV);

    // SB初期化
    if ( !DWCi_GetMatchCnt()->sbObj )
            DWCi_GetMatchCnt()->sbObj = ServerBrowserNew(DWCi_GetMatchCnt()->gameName,
                                                  DWCi_GetMatchCnt()->gameName,
                                                  DWCi_GetMatchCnt()->secretKey, 0, 20,
                                                  QVERSION_QR2, SBFalse,
                                                  DWCi_SBCallback, NULL);
    if (!DWCi_GetMatchCnt()->sbObj){
        // メモリ確保エラーと思われる
        if (DWCi_HandleSBError(sbe_allocerror)) return;
    }

    // 希望接続人数と友達の友達を許すかをGPステータス文字列として作成する
    (void)OS_SNPrintf(valueStr, sizeof(valueStr), "%u", numEntry);
    (void)DWC_SetCommonKeyValueString(DWC_GP_SSTR_KEY_MATCH_FRIEND_NUM,
                                      valueStr, keyValueStr, '/');
    (void)DWC_AddCommonKeyValueString(DWC_GP_SSTR_KEY_DISTANT_FRIEND,
                                      distantFriend ? "Y" : "N",
                                      keyValueStr, '/');

    // マッチメイクバージョンをセットする
    (void)OS_SNPrintf(valueStr, sizeof(valueStr), "%u", DWC_MATCHING_VERSION);
    (void)DWC_AddCommonKeyValueString(DWC_GP_SSTR_KEY_MATCH_VERSION,
                                      valueStr, keyValueStr, '/');

    // GPステータスを友達指定ピアマッチメイク中にし、作成したGPステータス文字列も
    // セットする
    gpResult = DWCi_SetGPStatus(DWC_STATUS_MATCH_FRIEND, keyValueStr, NULL);
    if (DWCi_HandleGPError(gpResult)) return;

#ifdef DWC_QR2_ALIVE_DURING_MATCHING
    if (DWCi_GetMatchCnt()->qr2Obj == NULL){
        // QR2が起動されていなければここで起動する。
        // 予約コマンドを送る前にqr2IPとqr2Portを初期化する必要がある。
        if (DWCi_QR2Startup(DWCi_GetMatchCnt()->profileID)) return;
    }

#else
    // 状態の変化をマスターサーバに通知する
    qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
#endif

    // GPメッセージで最初のサーバにNATネゴシエーションの予約を行う。
    // プロファイルIDが必ずバディであることはここでは保証されている。
    result = DWCi_SendResvCommandToFriend(FALSE, TRUE, 0);
    if (DWCi_HandleMatchCommandError(result)) return;

    // 指定人数以下でのマッチメイク完了オプション用初期化
    DWCi_InitOptMinCompParam(FALSE);
}


/*---------------------------------------------------------------------------*
  サーバクライアントマッチメイク・サーバ開始関数
  引数　：maxEntry          最大接続人数（自分は含まない）
          matchedCallback   マッチメイク完了コールバック
          matchedParam      上記コールバック用パラメータ
          newClientCallback 新規接続クライアント通知コールバック
          newClientParam    上記コールバック用パラメータ
  戻り値：なし
  用途　：サーバクライアントマッチメイクのサーバ側としてマッチメイクを開始する
 *---------------------------------------------------------------------------*/
// [arakit] main 051027
void DWCi_SetupGameServer(u8   maxEntry,
                          DWCMatchedSCCallback matchedCallback,
                          void* matchedParam,
                          DWCNewClientCallback newClientCallback,
                          void* newClientParam)
{
    GPResult gpResult;

    // マッチメイク用共通パラメータセット
    DWCi_SetMatchCommonParam(DWC_MATCH_TYPE_SC_SV,
                             maxEntry, matchedCallback, matchedParam);

    // 新規接続クライアント通知コールバックをセット
    DWCi_GetMatchCnt()->newClientCallback = newClientCallback;
    DWCi_GetMatchCnt()->newClientParam    = newClientParam;

    // プロファイルIDリストの最初には自分のプロファイルIDを入れる
    DWCi_GetMatchCnt()->sbPidList[0] = DWCi_GetMatchCnt()->profileID;

    // 自分のAIDは必ず最初から有効
    DWCi_GetMatchCnt()->validAidBitmap  = 1;
    DWCi_GetMatchCnt()->gt2NumValidConn = 0;

    // マッチメイク開始時には必ずマッチメイクオプションの新規接続ブロックは解除する
    stOptSCBlock.lock  = 0;

    DWCi_SetMatchStatus(DWC_MATCH_STATE_SV_WAITING);

    // GPステータスをセットする
    gpResult = DWCi_GPSetServerStatus();
    if (DWCi_HandleGPError(gpResult)) return;

    // [arakit] main 051008
#ifdef DWC_QR2_ALIVE_DURING_MATCHING
    if (DWCi_GetMatchCnt()->qr2Obj == NULL){
        // QR2が起動されていなければここで起動する
        DWCi_QR2Startup(DWCi_GetMatchCnt()->profileID);
    }

#else
    // 状態の変化をマスターサーバに通知する
    qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
#endif
    // [arakit] main 051008
}


/*---------------------------------------------------------------------------*
  サーバクライアントマッチメイク・サーバ接続開始関数
  引数　：serverPid         サーバのプロファイルID
          matchedCallback   マッチメイク完了コールバック
          matchedParam      上記コールバック用パラメータ
          newClientCallback 新規接続クライアント通知コールバック
          newClientParam    上記コールバック用パラメータ
  戻り値：なし
  用途　：サーバクライアントマッチメイクのクライアント側として
          マッチメイクを開始する
 *---------------------------------------------------------------------------*/
void DWCi_ConnectToGameServerAsync(int serverPid,
                                   DWCMatchedSCCallback matchedCallback,
                                   void* matchedParam,
                                   DWCNewClientCallback newClientCallback,
                                   void* newClientParam)
{
    int result;
    GPResult gpResult;

    // マッチメイク用共通パラメータセット
    DWCi_SetMatchCommonParam(DWC_MATCH_TYPE_SC_CL,
                             0, matchedCallback, matchedParam);

    // 新規接続クライアント通知コールバックをセット
    DWCi_GetMatchCnt()->newClientCallback = newClientCallback;
    DWCi_GetMatchCnt()->newClientParam    = newClientParam;

    // クライアントであることを記録する
    DWCi_GetMatchCnt()->qr2IsReserved  = 1;
    DWCi_GetMatchCnt()->qr2Reservation = DWCi_GetMatchCnt()->profileID;

    // サーバのプロファイルIDをリストにセットする
    DWCi_GetMatchCnt()->sbPidList[0] = serverPid;

    DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAIT_RESV);

    // SB初期化
    if ( !DWCi_GetMatchCnt()->sbObj )
            DWCi_GetMatchCnt()->sbObj = ServerBrowserNew(DWCi_GetMatchCnt()->gameName,
                                                  DWCi_GetMatchCnt()->gameName,
                                                  DWCi_GetMatchCnt()->secretKey,
                                                  0, 20,
                                                  QVERSION_QR2, SBFalse,
                                                  DWCi_SBCallback, NULL);
    if (!DWCi_GetMatchCnt()->sbObj){
        // メモリ確保エラーと思われる
        if (DWCi_HandleSBError(sbe_allocerror)) return;
    }

    // GPステータスをサーバクライアントマッチメイクのクライアントにセットする
    gpResult = DWCi_SetGPStatus(DWC_STATUS_MATCH_SC_CL, "", NULL);
    if (DWCi_HandleGPError(gpResult)) return;

    // [arakit] main 051008
#ifdef DWC_QR2_ALIVE_DURING_MATCHING
    if (DWCi_GetMatchCnt()->qr2Obj == NULL){
        // QR2が起動されていなければここで起動する。
        // 予約コマンドを送る前にqr2IPとqr2Portを初期化する必要がある。
        if (DWCi_QR2Startup(DWCi_GetMatchCnt()->profileID)) return;
    }

#else
    // 状態の変化をマスターサーバに通知する
    qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
#endif
    // [arakit] main 051008

    // GPメッセージで目標のサーバにNATネゴシエーションの予約を行う。
    // プロファイルIDが必ずバディであることはここでは保証されている。
    // また相手が必ずサーバクライアントマッチメイクのサーバを起動していることも
    // 保証されている。
    result = DWCi_SendResvCommand(DWCi_GetMatchCnt()->sbPidList[0], FALSE);
    if (DWCi_HandleMatchCommandError(result)) return;
}

/*---------------------------------------------------------------------------*
  マッチメイク処理更新関数の下請け関数。必要に応じてqr2_send_statechanged を呼ぶ
  引数　：qr2obj 
  戻り値：なし
  用途　：QR2データ反映のために状態の変化をマスターサーバに通知する
 *---------------------------------------------------------------------------*/
static void DWCi_SendStateChanged(qr2_t qr2Obj)
{
  if(qr2Obj == NULL){ return; }

  qr2_think(qr2Obj);
  
  if(qr2Obj->userstatechangerequested){ return; }
  
  // クライアントが検索を行うタイミングでQR2データ反映を
  // 早くするために状態の変化をマスターサーバに通知する
  // （このタイミングだと実際にはフラグがセットされるだけ）
  switch(DWCi_GetMatchCnt()->qr2MatchType){
  case DWC_MATCH_TYPE_ANYBODY:
  case DWC_MATCH_TYPE_FRIEND:
	switch(DWCi_GetMatchCnt()->state){
	case DWC_MATCH_STATE_CL_WAITING:
	case DWC_MATCH_STATE_CL_SEARCH_OWN:
	case DWC_MATCH_STATE_CL_SEARCH_HOST:
	case DWC_MATCH_STATE_CL_WAIT_RESV:
	case DWC_MATCH_STATE_CL_NN:
	case DWC_MATCH_STATE_SV_OWN_NN:
	  qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
	  break;
	}
	break;
  case DWC_MATCH_TYPE_SC_SV:
	if(DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_OWN_NN){
	  qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
	}
	break;
  }
}

/*---------------------------------------------------------------------------*
  マッチメイク処理更新関数
  引数　：fullSpec TRUE :マッチメイクの為の処理を全て行う、
                   FALSE:GT2とQR2の更新のみ行う
  戻り値：なし
  用途　：毎ゲームフレーム呼び出し、マッチメイク通信処理を更新する
 *---------------------------------------------------------------------------*/
void DWCi_MatchProcess(BOOL fullSpec)
{
  u32  resendInterval;
  u32  sbInterval;
  int  result;
  SBError sbError;
    
  if(!DWCi_GetMatchCnt() || DWCi_IsError()){ return; }
  
  if(!fullSpec){ // GT2とQR2の更新のみ行う
	if(DWCi_GetMatchCnt()->qr2Obj){ qr2_think(DWCi_GetMatchCnt()->qr2Obj); }
	if (DWCi_GetMatchCnt()->pGt2Socket){ gt2Think(*DWCi_GetMatchCnt()->pGt2Socket); }
	return;
  }

  if(DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_INIT){	return; } // 初期状態で来ても何もしない

    /*** ↓タイムアウト処理 ***/
    switch (DWCi_GetMatchCnt()->state){
    case DWC_MATCH_STATE_CL_WAIT_RESV:  // NN予約後返答待ち状態
	  //        if (DWCi_GetMatchCnt()->cmdTimeoutCount){
	  if (DWCi_GetMatchCnt()->cmdTimeoutTime > 0 ){
		// NN予約コマンドの応答待ち
		//            DWCi_GetMatchCnt()->cmdTimeoutCount++;
		//            if (DWCi_GetMatchCnt()->cmdTimeoutCount >= DWC_MATCH_CMD_RESV_TIMEOUT){
		if (OS_TicksToMilliSeconds( OS_GetTick() - DWCi_GetMatchCnt()->cmdTimeoutStartTick ) > DWCi_GetMatchCnt()->cmdTimeoutTime){
		  // サーバ側が切断して応答がなくなったと考えられる
		  //                DWCi_GetMatchCnt()->cmdTimeoutCount = 0;
		  DWCi_GetMatchCnt()->cmdTimeoutTime = 0;
		  
		  if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL){
			// サーバクライアントマッチメイクの場合
			DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
					   "Timeout: wait server response %d/%d.\n",
					   DWCi_GetMatchCnt()->scResvRetryCount,
					   DWC_MATCH_CMD_RETRY_MAX );
			
			DWCi_GetMatchCnt()->scResvRetryCount++;
			if (DWCi_GetMatchCnt()->scResvRetryCount > DWC_MATCH_CMD_RETRY_MAX){
			  // 一定回数失敗したらマッチメイクをエラー終了する
			  DWCi_StopMatching(DWC_ERROR_NETWORK,
								DWC_ECODE_SEQ_MATCH+DWC_ECODE_TYPE_SC_CL_FAIL);
			  return;
			}
			else {
			  // 予約コマンドを再送する
			  result = DWCi_SendResvCommand(DWCi_GetMatchCnt()->sbPidList[0], FALSE);
			  if (DWCi_HandleMatchCommandError(result)) return;
			}
		  }
		  else {
			// 友達指定・無指定ピアマッチメイクの場合は次のサーバを探す
			// エラー処理は関数内で行っている
			DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "NN resv(with %u) timed out. Try next server.\n", DWCi_GetMatchCnt()->sbPidList[0]);
			if (!DWCi_RetryReserving(0)) return;
		  }
		}
	  }
	  
	  //        if (DWCi_GetMatchCnt()->cmdResendCount){
	  if (DWCi_GetMatchCnt()->cmdResendFlag){
		// NN予約コマンドの再送待ち
		// 既にクライアントが繋がっている場合は、再検索開始を遅らせ、
		// 他のホストの予約を長めに待つ
		resendInterval =
		  (u32)DWC_MATCH_CMD_RESEND_INTERVAL_MSEC+DWC_MATCH_CMD_RESEND_INTERVAL_ADD_MSEC*DWCi_GetMatchCnt()->gt2NumConnection;
		
		//            DWCi_GetMatchCnt()->cmdResendCount++;
		//            if (DWCi_GetMatchCnt()->cmdResendCount >= resendInterval){
		if (OS_TicksToMilliSeconds( OS_GetTick() - DWCi_GetMatchCnt()->cmdResendTick ) >= resendInterval){
		  // 再びサーバにNN予約コマンドを送る
		  result = DWCi_SendResvCommand(DWCi_GetMatchCnt()->sbPidList[0], FALSE);
		  if (DWCi_HandleMatchCommandError(result)) return;
		}
	  }
	  break;

    case DWC_MATCH_STATE_CL_SEARCH_OWN:      // 自ホスト情報検索中
    case DWC_MATCH_STATE_CL_SEARCH_HOST:     // 空きホスト検索中（友達無指定ピアマッチメイク時）
    case DWC_MATCH_STATE_CL_SEARCH_NN_HOST:  // NN予約完了ホスト検索中
	  if(DWCi_GetMatchCnt()->sbUpdateFlag <= 0){ break; }

	  if(DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_SEARCH_HOST){
		// 友達無指定ピアマッチメイクのサーバで、既にクライアントが
		// 繋がっている場合は、再検索開始を遅らせ、他のホストの予約を
		// 長めに待つ
		sbInterval = (u32)DWC_SB_UPDATE_INTERVAL_MSEC+DWC_SB_UPDATE_INTERVAL_ADD_MSEC*DWCi_GetMatchCnt()->gt2NumConnection;
	  }else{
		if (DWCi_GetMatchCnt()->sbUpdateFlag == DWC_SB_UPDATE_INTERVAL_SHORT){
		  sbInterval = DWC_SB_UPDATE_INTERVAL_SHORT_MSEC;
		}else{
		  sbInterval = DWC_SB_UPDATE_INTERVAL_MSEC;
		}
	  }
	  // DWCi_GetMatchCnt()->sbUpdateCount++;
	  if (OS_TicksToMilliSeconds( OS_GetTick() - DWCi_GetMatchCnt()->sbUpdateTick ) > sbInterval){
		// SBサーバアップデートを行う
		sbError = DWCi_SBUpdateAsync(DWCi_GetMatchCnt()->reqProfileID);
		if(DWCi_HandleSBError(sbError)){ return; }
		DWCi_GetMatchCnt()->sbUpdateFlag = 0;
	  }
	  break;
    case DWC_MATCH_STATE_CL_GT2:  // GT2コネクション確立中
        if (DWCi_GetMatchCnt()->nnFinishTime){
            // NN開始側でしか(nnFinishTime > 0)にならない
            if (OS_TicksToMilliSeconds(OS_GetTick()-DWCi_GetMatchCnt()->nnFinishTime) > DWC_WAIT_GT2_CONNECT_TIMEOUT){
                // NAT外の相手とNATネゴシエーションを完了した後、
                // 相手からgt2Connectがなかなか返ってこない場合
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Timeout: wait gt2Connect().\n");

                DWCi_GetMatchCnt()->nnFinishTime = 0;
                // 全てのクライアント処理を終了してマッチメイクを再開する。
                // サーバクライアントマッチメイクの場合はエラー終了する。
                if (!DWCi_CancelPreConnectedClientProcess(DWCi_GetMatchCnt()->sbPidList[0]))
                    return;
            }
        }
        else if ((DWCi_GetMatchCnt()->cmdCnt.command == DWC_MATCH_COMMAND_TELL_ADDR) &&
                 (OS_TicksToMilliSeconds(OS_GetTick()-DWCi_GetMatchCnt()->cmdCnt.sendTime) > DWC_MATCH_CMD_RTT_TIMEOUT)){
                DWC_Printf(DWC_REPORTFLAG_DEBUG,
                           "RTT Timeout with DWC_MATCH_STATE_CL_GT2.\n" );
            // 同一NAT内の相手からgt2Connectがなかなか返ってこない場合
            DWCi_GetMatchCnt()->cmdCnt.count++;
            if (DWCi_GetMatchCnt()->cmdCnt.count > DWC_MATCH_CMD_RETRY_MAX){
                // 再送回数オーバーの場合
                DWCi_StopResendingMatchCommand();  // コマンド再送停止

                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                           "Stop resending command %d.\n",
                           DWC_MATCH_COMMAND_TELL_ADDR);
                    
                // 全てのクライアント処理を終了してマッチメイクを再開する。
                // サーバクライアントマッチメイクの場合はエラー終了する。
                if (!DWCi_CancelPreConnectedClientProcess(DWCi_GetMatchCnt()->sbPidList[0]))
                    return;
                break;
            }

            // アドレス通知コマンドの再送を行う
            result =
                DWCi_SendMatchCommand(DWC_MATCH_COMMAND_TELL_ADDR,
                                      DWCi_GetMatchCnt()->cmdCnt.profileID,
                                      DWCi_GetMatchCnt()->cmdCnt.ip,
                                      DWCi_GetMatchCnt()->cmdCnt.port,
                                      DWCi_GetMatchCnt()->cmdCnt.data,
                                      DWCi_GetMatchCnt()->cmdCnt.len);
            if (DWCi_HandleMatchCommandError(result)) return;
        }
        break;

    case DWC_MATCH_STATE_SV_OWN_NN:  // NATネゴシエーション中
        if ((DWCi_GetMatchCnt()->cmdCnt.command == DWC_MATCH_COMMAND_RESV_OK) &&
            (((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY) &&
              (OS_TicksToMilliSeconds(OS_GetTick()-DWCi_GetMatchCnt()->cmdCnt.sendTime) > DWC_MATCH_RESV_KEEP_TIME_ANYBODY)) ||
             ((DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_ANYBODY) &&
              (OS_TicksToMilliSeconds(OS_GetTick()-DWCi_GetMatchCnt()->cmdCnt.sendTime) > DWC_MATCH_RESV_KEEP_TIME_FRIEND)))){
                // 予約を承認したクライアントが、いつまで経ってもNN接続に
                // 来ない場合、予約をクリアして初期状態に戻る。
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                           "Reservation timeout. Cancel reservation.\n");

                DWCi_StopResendingMatchCommand();  // コマンド再送停止
                
                // 新規接続の受け付け処理を終了してマッチメイクを再開する。
                // サーバクライアントマッチメイクの場合は、新規接続クライアントの
                // 接続キャンセルとして処理する。
                if (!DWCi_CancelPreConnectedServerProcess(DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->gt2NumConnection+1]))
                    return;
        }
        break;

    case DWC_MATCH_STATE_SV_WAIT_CL_LINK:  // クライアント同士の接続完了待ち
        if ((DWCi_GetMatchCnt()->cmdCnt.command == DWC_MATCH_COMMAND_LINK_CLS_REQ) &&
            (OS_TicksToMilliSeconds(OS_GetTick()-DWCi_GetMatchCnt()->cmdCnt.sendTime) > DWC_MATCH_LINK_CLS_TIMEOUT)){
            // クライアント同士の接続がなかなか終わらない時
            DWCi_GetMatchCnt()->cmdCnt.count++;
            if (DWCi_GetMatchCnt()->cmdCnt.count > 0){ // [sasakit] retryの意味があまりないので、やり直さない。
                // 再送回数オーバーの場合。
                DWCi_StopResendingMatchCommand();  // コマンド再送停止

                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                           "Wait clients connecting timeout.\n");

                if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV){
                    // サーバクライアントマッチメイクのサーバの場合は、
                    // 接続状態を継続する必要があるので、新規接続クライアント
                    // のみをクローズして続ける
                    if (!DWCi_CancelPreConnectedServerProcess(DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->gt2NumConnection]))
                        return;
                }
                else {
                    DWCi_RestartFromTimeout();  // マッチメイクを最初からやり直し
                }
                break;
            }

            // クライアント同士の接続要求コマンドが落ちた為かもしれないので、
            // コマンドの再送を行う
            result =
                DWCi_SendMatchCommand(DWC_MATCH_COMMAND_LINK_CLS_REQ,
                                      DWCi_GetMatchCnt()->cmdCnt.profileID,
                                      DWCi_GetMatchCnt()->cmdCnt.ip,
                                      DWCi_GetMatchCnt()->cmdCnt.port,
                                      DWCi_GetMatchCnt()->cmdCnt.data,
                                      DWCi_GetMatchCnt()->cmdCnt.len);
            if (DWCi_HandleMatchCommandError(result)) return;
        }
        break;

    case DWC_MATCH_STATE_CL_WAITING:
        if ((DWC_GetState() == DWC_STATE_MATCHING) &&
            (OS_TicksToMilliSeconds(OS_GetTick()-DWCi_GetMatchCnt()->clWaitTime) > DWC_MATCH_CL_WAIT_TIMEOUT)){
            // クライアントが待ち状態の時に一定時間が経過した場合、
            // サーバにポーリングパケットを送る
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "No data from server %d/%d.\n",
                       DWCi_GetMatchCnt()->clWaitTimeoutCount, DWC_MATCH_CMD_RETRY_MAX);
                
            if (DWCi_GetMatchCnt()->clWaitTimeoutCount >= DWC_MATCH_CMD_RETRY_MAX){
                // 5回送ってもサーバから返答がなかった場合はマッチメイクをやり直す。
                // 全てのクライアント処理を終了してマッチメイクを再開する。
                // サーバクライアントマッチメイクの場合はエラー終了する。
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                           "Timeout: Connection to server was shut down.\n");
                    
                if (!DWCi_CancelPreConnectedClientProcess(DWCi_GetMatchCnt()->sbPidList[0]))
                    return;
            }
            else {
                result =
                    DWCi_SendMatchCommand(DWC_MATCH_COMMAND_CL_WAIT_POLL,
                                          DWCi_GetMatchCnt()->sbPidList[0],
                                          DWCi_GetMatchCnt()->qr2IPList[0],
                                          DWCi_GetMatchCnt()->qr2PortList[0],
                                          NULL, 0);
                if (DWCi_HandleMatchCommandError(result)) return;

                DWCi_GetMatchCnt()->clWaitTimeoutCount++;
                // コマンドの往復タイムアウト時間経過後に次のタイムアウトが
                // 発生するよう、タイムアウト計測開始時間を調整する
                DWCi_GetMatchCnt()->clWaitTime =
                    OS_GetTick()-OS_MilliSecondsToTicks(DWC_MATCH_CL_WAIT_TIMEOUT-DWC_MATCH_CMD_RTT_TIMEOUT);
            }
        }
        break;

    default:
        break;
    }

    if (((DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_OWN_NN) ||
         (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_NN)) &&
        DWCi_GetMatchCnt()->nnFailedTime &&
        (OS_TicksToMilliSeconds(OS_GetTick()-DWCi_GetMatchCnt()->nnFailedTime) > DWC_WAIT_NN_RETRY_TIMEOUT)){
        // NN失敗後、いつまでもリトライが来ず、タイムアウトになった場合
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Timeout : wait NN retry.\n");

        // NATネゴシエーション完了コールバックを、相手からの応答無しエラーで
        // 呼び出す
        DWCi_NNCompletedCallback(nr_deadbeatpartner, NULL, NULL, &DWCi_GetMatchCnt()->nnInfo);
    }
    /*** ↑タイムアウト処理 ***/


    /*** ↓GameSpy 各SDK更新処理 ***/
    if (DWCi_GetMatchCnt()->sbObj){
#if 1
        // 長時間マスターサーバと通信しないとsbe_connecterrorのエラーを
        // 返すが、ここでは無視する。
        // マスターサーバとの通信を行う時に切断されていたら自動的に再接続される。
        s_sbCallbackLevel = 0;
        s_needSbFree = FALSE;
        
        (void)ServerBrowserThink(DWCi_GetMatchCnt()->sbObj);
       
        if(s_needSbFree)
        {
            ServerBrowserFree(DWCi_GetMatchCnt()->sbObj);
            DWCi_GetMatchCnt()->sbObj = NULL;
        }

#else
        sbError = ServerBrowserThink(DWCi_GetMatchCnt()->sbObj);
        if (DWCi_HandleSBError(sbError)) return;
#endif
		// SBが接続状態でかつLimitUpdate中でかつ長い時間応答がない場合にはマッチングをエラー終了させる
        if(DWCi_GetMatchCnt()->sbObj) { 
            if(ServerBrowserState(DWCi_GetMatchCnt()->sbObj) != sb_disconnected &&
               DWCi_GetMatchCnt()->sbUpdateRequestTick != 0 &&
               OS_GetTick() > DWCi_GetMatchCnt()->sbUpdateRequestTick) {
                DWCi_StopMatching(DWC_ERROR_NETWORK, DWC_ECODE_SEQ_MATCH+DWC_ECODE_GS_SB+DWC_ECODE_TYPE_NETWORK);
                DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "ServerBrowserLimitUpdate timeout.\n" );
            }
        }
    }

	DWCi_SendStateChanged(DWCi_GetMatchCnt()->qr2Obj); // ステートチェンジを一定時間ごとに送る

    // NNCancel後、キャンセルされたcookieに対応するNATNegotiatorは
    // NNThink()内でRemoveされるので、それまでにgt2Think()内の
    // UnrecognizedCallbackからNNProcessData()が呼び出されるのは好ましくない。
    // そのため、gt2Think()よりもNNThink()の方を先に呼び出す。
    s_bNNFreeNegotiateList = FALSE;
    s_bInNNThink = TRUE;
    NNThink();
    s_bInNNThink = FALSE;

    if(s_bNNFreeNegotiateList == TRUE)
    {
        s_bNNFreeNegotiateList = FALSE;
        NNFreeNegotiateList();
    }

    if (DWCi_GetMatchCnt()->pGt2Socket){
        gt2Think(*DWCi_GetMatchCnt()->pGt2Socket);
    }
    /*** ↑GameSpy 各SDK更新処理 ***/


    /*** その他毎ゲームフレーム処理（各SDK更新後に呼び出す必要があるもの） ***/
    if ((DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_WAIT_CLOSE) &&
        (OS_TicksToMilliSeconds(OS_GetTick()-DWCi_GetMatchCnt()->closedTime) > (DWC_MATCH_CMD_RTT_TIMEOUT >> 1))){
        // 他ホストからクローズされた場合の優先予約先サーバ通知待ちが
        // タイムアウトした場合、最初からマッチメイクをやり直す
        DWC_Printf(DWC_REPORTFLAG_DEBUG,
                   "RTT Timeout with DWCi_MatchProcess.\n" );
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Timeout : Wait prior profileID.\n");
        
        if (DWCi_ResumeMatching()) return;
    }

    // マッチメイク完了同期調整処理
    if (!DWCi_ProcessMatchSynTimeout()) return;

    // サーバクライアントマッチメイクキャンセル同期調整処理
    if (!DWCi_ProcessCancelMatchSynTimeout()) return;

    // 指定人数以下でのマッチメイク完了オプション処理
    DWCi_ProcessOptMinComp();

#ifdef DWC_STOP_SC_SERVER
    // サーバクライアントマッチメイクのサーバ側で、マッチメイク締め切りフラグが
    // 立っていたら、マッチメイクが完了次第、最大接続人数を現在接続人数に
    // 合わせて以後の接続を拒否できる
    if (DWCi_GetMatchCnt()->stopSCFlag && (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_WAITING)){
        DWCi_GetMatchCnt()->qr2NumEntry = DWCi_GetMatchCnt()->gt2NumConnection;
        (void)DWCi_GPSetServerStatus();
        
        DWCi_GetMatchCnt()->stopSCFlag = 0;

        // コールバック呼び出し
        if (DWCi_GetMatchCnt()->stopSCCallback)
            DWCi_GetMatchCnt()->stopSCCallback(DWCi_GetMatchCnt()->stopSCParam);
    }
#endif
}


/*---------------------------------------------------------------------------*
  GT2認識不能メッセージ受信コールバック関数
  引数　：socket  GT2ソケットへのポインタ
          ip      相手ホストのIP
          port    相手ホストのポート番号
          message 受信データ
          len     受信データサイズ
  戻り値：GT2のメッセージでなければGT2Falseを返す
  用途　：GT2認識不能メッセージ受信コールバック
 *---------------------------------------------------------------------------*/
GT2Bool DWCi_GT2UnrecognizedMessageCallback(GT2Socket socket,
                                            unsigned int ip,
                                            unsigned short port,
                                            GT2Byte* message,
                                            int len)
{
#pragma unused(socket)
	struct sockaddr_in saddr;

	if (!len || !message) return GT2False;

    //(void)memset(&saddr, 0, sizeof(saddr));
    MI_CpuClear32(&saddr, sizeof(saddr));
    saddr.sin_family = AF_INET;
   	saddr.sin_addr.s_addr = ip;
    saddr.sin_port = htons(port);

    if (((message[0] == QR_MAGIC_1) && (message[1] == QR_MAGIC_2))
        || (message[0] == '\\')){
        // QR2メッセージ受信の場合
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "GT2 Unrecognized : Received QR2 data.\n");
        
        // 必ずNULL終端にしたい
        // [note]
        // 定数GSI_UNICODEが定義されていなければNULL終端確認は必要ないが、
        // 定義された場合は、qr2_parse_query()の先で終端のNULL文字を探す
        // 処理があるので、バグの原因となる
        if (DWCi_GetMatchCnt()->qr2Obj){
         	qr2_parse_query(DWCi_GetMatchCnt()->qr2Obj, (gsi_char *)message, len, (struct sockaddr *)&saddr);
        }
        else {
            DWC_Printf( DWC_REPORTFLAG_MATCH_NN, "  ignore qr2 message.\n" );
        }
    }
    else if (!memcmp(message, NNMagicData, NATNEG_MAGIC_LEN)){
        // NNメッセージ受信の場合
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "GT2 Unrecognized : Received NN data.\n");
        NNProcessData((char *)message, len, &saddr);
    }
    else if ((message[0] == 0xfe) && (message[1] == 0xfe))
    {
        // GT2ConnectionのないGT2Packet
        DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "GT2 Unrecognized : Not Connected gt2 data.\n");

        // ここでGT2Falseを返さないと、ConnectAttemptCallbackの呼び出しに
        // 進んでくれないので注意！！
        return GT2False;
    }
    else {
        // 認識不能メッセージを受信した場合
        // またコネクションのない相手からデータを受信した場合は、
        // GT2パケット用のマジックナンバーが先頭にあってもここに来る
        DWC_Printf(DWC_REPORTFLAG_WARNING, "GT2 Unrecognized : Received unrecognized data.\n");

        return GT2False;
    }

    return GT2True;
}


/*---------------------------------------------------------------------------*
  GT2接続要求受信コールバック関数
  引数　：socket     GT2ソケットへのポインタ
          connection 受け付けたコネクションの構造体ポインタ
          ip         相手ホストのIP
          port       相手ホストのポート番号
          latency    相手ホストとのレイテンシ（往復時間）
          message    受信データ
          len        受信データサイズ
  戻り値：なし
  用途　：GT2接続要求受信コールバック
 *---------------------------------------------------------------------------*/
void DWCi_GT2ConnectAttemptCallback(GT2Socket socket,
                                    GT2Connection connection,
                                    unsigned int ip,
                                    unsigned short port,
                                    int latency,
                                    GT2Byte* message,
                                    int len)
{
#pragma unused(socket)
#pragma unused(message)
#pragma unused(len)
    int index;
    GT2Bool result;
    GT2Connection* pGt2Con;
    DWCConnectionInfo* pConInfo;

    if (!DWCi_GetMatchCnt() || (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_GT2) ||
        (DWCi_GetMatchCnt()->cancelState != DWC_MATCH_CANCEL_STATE_INIT)){
        // キャンセル直後などにGT2Connectされた場合はRejectする
        gt2Reject(connection, (const GT2Byte *)"Init state", -1);
        DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "gt2Reject was called : Init state\n");
        return;
    }

    // コネクションリストの空きを検索
    index = DWCi_GT2GetConnectionListIdx();
	if (index == -1){
        // 既にコネクションがいっぱいの場合はRejectする
        gt2Reject(connection, (const GT2Byte *)"Server full", -1);
        DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "gt2Reject was called : Server full\n");

        // マッチメイクをエラー終了する
        DWCi_StopMatching(DWC_ERROR_NETWORK,
                          DWC_ECODE_SEQ_MATCH+DWC_ECODE_GS_GT2+DWC_ECODE_TYPE_CONN_OVER);
    }
    else {
        if ((ip != DWCi_GetMatchCnt()->ipList[DWCi_GetMatchCnt()->gt2NumConnection]) ||
            (port != DWCi_GetMatchCnt()->portList[DWCi_GetMatchCnt()->gt2NumConnection])){
            if (message[0] &&
                (strtoul((const char *)message, NULL, 10) == DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->gt2NumConnection])){
                // まだNATネゴシエーションが完了していないのに、
                // 先にgt2Connect()を受信した場合
                DWC_Printf(DWC_REPORTFLAG_WARNING,
                           "gt2Connect() came before NN complete.\n");

                // IPとポートはここでリストに設定する
                DWCi_GetMatchCnt()->ipList[DWCi_GetMatchCnt()->gt2NumConnection]   = ip;
                DWCi_GetMatchCnt()->portList[DWCi_GetMatchCnt()->gt2NumConnection] = port;
            }
            else {
                // 全然関係ないホストからのgt2Connect()だった場合は何もしない
                gt2Reject(connection, (const GT2Byte *)"Unknown connect attempt", -1);
                DWC_Printf(DWC_REPORTFLAG_WARNING,
                           "gt2Reject was called : Unknown connect attempt from %s\n",
                           gt2AddressToString(ip, port, NULL));
                return;
            }
        }

        
        // NN終了時の時刻をクリアする
        DWCi_GetMatchCnt()->nnFinishTime = 0;

        // コネクション受け入れ
	    result = gt2Accept(connection, DWCi_GetMatchCnt()->gt2Callbacks);
        if (result == GT2False){
            // [test]
            // このライブラリにおいては、Connect受け付け後、即Acceptするので
            // resultがGT2Falseになることは、スレッドに割り込みでもされない
            // 限りはありえない
            DWC_Printf(DWC_REPORTFLAG_WARNING, "Unexpected failure to gt2Accept.\n");
                
            // 既に切断されていた場合、エラーでマッチメイク処理を止める
            // [todo]
            // サーバクライアントマッチメイク以外の場合は、
            // マッチメイクをやり直すように変更する必要がある
            DWCi_StopMatching(DWC_ERROR_NETWORK,
                              DWC_ECODE_SEQ_MATCH+DWC_ECODE_TYPE_OTHER);
            return;
        }
        
	    DWC_Printf(DWC_REPORTFLAG_MATCH_GT2,
                   "Accepted connection from %s (latency %d)\n",
                   gt2AddressToString(ip, port, NULL), latency);

        // TELL_ADDRコマンドをこれ以上再送しないように再送コマンドをクリアする
        DWCi_StopResendingMatchCommand();

        if (DWCi_GetMatchCnt()->gt2NumConnection == 0){
            // 後でサーバに伝える為に、サーバとの間の片道レイテンシを記録する
            DWCi_GetMatchCnt()->baseLatency =
                (u16)(((latency >> 1) < 0xffff) ? (latency >> 1) : 0xffff);
        }

        // GT2コネクションリストとコネクション情報リストの要素へのポインタ取得
        pGt2Con  = DWCi_GetGT2ConnectionByIdx(index);
        pConInfo = DWCi_GetConnectionInfoByIdx(index);

        // 受け付けたコネクションをリストに追加
        *pGt2Con = connection;
        DWCi_GetMatchCnt()->gt2NumConnection++;

        // コネクションデータをセットする
        pConInfo->index   = (u8)index;
        pConInfo->aid     = DWCi_GetMatchCnt()->aidList[DWCi_GetMatchCnt()->gt2NumConnection-1];
        pConInfo->reserve = 0;
        pConInfo->param   = NULL;

        // GT2Connection構造体のメンバdata（ユーザデータ登録用）に、
        // このコネクションの情報をセットしておく
    	gt2SetConnectionData(connection, pConInfo);

        // Connectされる側は常に新規接続クライアント
        DWCi_PostProcessConnection(DWC_PP_CONNECTION_CL_GT2_ACCEPT);
    }
}


/*---------------------------------------------------------------------------*
  GT2接続完了コールバック関数
  引数　：connection 完了したコネクションの構造体ポインタ
          result     GT2結果型
          message    受信データ
          len        受信データサイズ
  戻り値：なし
  用途　：GT2接続完了コールバック
 *---------------------------------------------------------------------------*/
void DWCi_GT2ConnectedCallback(GT2Connection connection,
                               GT2Result result,
                               GT2Byte* message,
                               int len)
{
#pragma unused(len)
    char pidStr[12];
    int  index;
    GT2Connection* pGt2Con;
    GT2Result gt2Result;
    DWCConnectionInfo* pConInfo;

    if (!DWCi_GetMatchCnt() ||
        ((DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_GT2) &&
         (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_SV_OWN_GT2))){
        // キャンセル直後などに来ても無視する
        DWC_Printf(DWC_REPORTFLAG_MATCH_GT2,
                   "gt2ConnectedCallback: Already cancel\n");
        return;
    }

	if (result != GT2Success){
		if (!message) message = (GT2Byte *)"";
		DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "GT2 connect failed %d: %s\n", result, message);

        if (result == GT2DuplicateAddress){
            // result = GT2DuplicateAddressの時は、connect完了の通知が届く前に
            // タイムアウトでconnectリトライしてしまった場合と考えられるので
            // 何もしない
            return;
        }
        else if (result == GT2TimedOut){
            // タイムアウトの場合はリトライする
            DWCi_GetMatchCnt()->gt2ConnectCount++;
            if (DWCi_GetMatchCnt()->gt2ConnectCount > DWC_MATCH_CMD_RETRY_MAX){
                // 再送回数オーバーの場合
                DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "gt2Connect() retry over.\n");
                DWCi_GetMatchCnt()->gt2ConnectCount = 0;

                // 新規接続の受け付け処理を終了してマッチメイクを再開する。
                // サーバクライアントマッチメイクの場合は、新規接続クライアントの
                // 接続キャンセルとして処理する。
                DWCi_CancelPreConnectedServerProcess(DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->qr2NNFinishCount]);
                return;
            }
                
            DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "Retry to gt2Connect.\n");

            (void)OS_SNPrintf(pidStr, sizeof(pidStr), "%u", DWCi_GetMatchCnt()->profileID);
            gt2Result =
                gt2Connect(*DWCi_GetMatchCnt()->pGt2Socket,
                           NULL,
                           gt2AddressToString(
                               DWCi_GetMatchCnt()->ipList[DWCi_GetMatchCnt()->qr2NNFinishCount],
                               DWCi_GetMatchCnt()->portList[DWCi_GetMatchCnt()->qr2NNFinishCount],
                               NULL),
                           (GT2Byte *)pidStr,
                           -1,
                           DWC_GT2_CONNECT_TIMEOUT,
                           DWCi_GetMatchCnt()->gt2Callbacks,
                           GT2False);
            if ( gt2Result == GT2OutOfMemory )
            {
                DWCi_HandleGT2Error(gt2Result);
                return;
            }
            else if ( gt2Result == GT2Success )
            {
                // 成功のときは、コールバックを待つ。
            }
            else if (!DWCi_CancelPreConnectedServerProcess(DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->qr2NNFinishCount]))
            {
                // gt2Connectにいきなり失敗したときは、リセットして次の接続を待つ。
                return;
            }
        }
        else {
            // Rejectされた場合は、新規接続の受け付け処理を終了して
            // マッチメイクを再開する。それ以外のエラーのときも同様。
            // （ここで無視しても、それ以外のSocketでエラーが
            // 出るはずなので、そこに期待する）
            // サーバクライアントマッチメイクの場合は、新規接続クライアントの
            // 接続キャンセルとして処理する。
            if (!DWCi_CancelPreConnectedServerProcess(DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->gt2NumConnection+1]))
                return;
        }
	}
    else {
    	DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "GT2 connected.\n");

        // コネクションリストの空きを検索
        index = DWCi_GT2GetConnectionListIdx();

        if (index == -1){
            // ゲームでコネクションをクローズせずにマッチメイクを繰り返した場合
            // コネクションリストがいっぱいになることはあり得る。
            DWC_Printf(DWC_REPORTFLAG_WARNING,
                       "Don't continue matching without closing connections!!\n");

            // マッチメイクをエラー終了する
            DWCi_StopMatching(DWC_ERROR_NETWORK,
                              DWC_ECODE_SEQ_MATCH+DWC_ECODE_GS_GT2+DWC_ECODE_TYPE_CONN_OVER);
            return;
        }

        // GT2コネクションリストとコネクション情報リストの要素へのポインタ取得
        pGt2Con  = DWCi_GetGT2ConnectionByIdx(index);
        pConInfo = DWCi_GetConnectionInfoByIdx(index);

        // 受け付けられたコネクションをリストに追加
        *pGt2Con = connection;
        DWCi_GetMatchCnt()->gt2NumConnection++;

        // コネクションデータをセットする
        pConInfo->index   = (u8)index;
        pConInfo->reserve = 0;
        pConInfo->param   = NULL;
        pConInfo->aid     = DWCi_GetMatchCnt()->aidList[DWCi_GetMatchCnt()->gt2NumConnection];

        // GT2Connection構造体のメンバdata（ユーザデータ登録用）に、
        // このコネクションの情報をセットしておく
	    gt2SetConnectionData(connection, pConInfo);

        if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_OWN_GT2){
            // サーバ側の場合
            DWCi_PostProcessConnection(DWC_PP_CONNECTION_SV_CONNECT);
        }
        else {
            // 接続済みクライアントの場合
            DWCi_PostProcessConnection(DWC_PP_CONNECTION_CL_GT2_CONNECT);
        }
    }
}


/*---------------------------------------------------------------------------*
  GPバディメッセージ受信コールバック関数
  引数　：pconnection GPコネクションオブジェクトへのポインタ
          profileID   メッセージ送信元のプロファイルID
          message     受信したメッセージ本文
  戻り値：なし
  用途　：GPバディメッセージを受信したらそのコールバック内で呼び出され、
          マッチメイク処理中に固有の処理を行う
 *---------------------------------------------------------------------------*/
void DWCi_MatchGPRecvBuddyMsgCallback(GPConnection* pconnection,
                                      int profileID,
                                      char* message)
{
#pragma unused(pconnection)
    char tmpStr[16];
    // [arakit] main 051011
    u32 data[DWC_MATCH_COMMAND_ADD_MESSAGE_NUM];
    int i;

    DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
               "<GP> RECV-0x%02x <- [--------:-----] [pid=%u]\n",
               *message, profileID);

    // 文字列で格納されている追加メッセージをu32に変換して配列に格納する
    for (i = 0; i < DWC_MATCH_COMMAND_ADD_MESSAGE_NUM; i++){ 
        if (DWCi_GetGPBuddyAdditionalMsg(tmpStr, message+1, i) == -1) break;
        data[i] = strtoul(tmpStr, NULL, 10);
    }

    // 受信したコマンドを処理する。
    // エラー処理は関数内で行っている
    (void)DWCi_ProcessRecvMatchCommand((u8)*message, profileID, 0, 0, data, i);
}


/*---------------------------------------------------------------------------*
  マッチメイクコマンド再送停止関数
  引数　：なし
  戻り値：なし
  用途　：マッチメイクコマンドの送信制御構造体の一部メンバを初期化し、
          コマンドの再送を止める
 *---------------------------------------------------------------------------*/
static void DWCi_StopResendingMatchCommand(void)
{

    DWCi_GetMatchCnt()->cmdCnt.command = DWC_MATCH_COMMAND_DUMMY;
    DWCi_GetMatchCnt()->cmdCnt.count   = 0;
}


/*---------------------------------------------------------------------------*
  マッチメイクエラー終了関数
  引数　：error     DWCエラー種別
          errorCode エラーコード
  戻り値：なし
  用途　：マッチメイク処理中にエラーが発生した場合に、処理を中断する
 *---------------------------------------------------------------------------*/
void DWCi_StopMatching(DWCError error, int errorCode)
{

    if (!DWCi_GetMatchCnt() || (error == DWC_ERROR_NONE)) return;

    // コネクションは全てクローズする
    DWCi_CloseAllConnectionsByTimeout();

    DWCi_SetError(error, errorCode);

    // GPステータスをオンライン状態にセットする
    // [arakit] main 051007
    (void)DWCi_SetGPStatus(DWC_STATUS_ONLINE, "", NULL);

    // マッチメイク完了コールバック関数呼び出し
    DWCi_GetMatchCnt()->matchedCallback(error,
                                 FALSE,
                                 DWCi_GetMatchCnt()->cbEventPid ? FALSE : TRUE,
                                 (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV) ? TRUE : FALSE, 
                                 DWCi_GetFriendListIndex(DWCi_GetMatchCnt()->cbEventPid),
                                 DWCi_GetMatchCnt()->matchedParam);

    DWCi_CloseMatching();
}


/*---------------------------------------------------------------------------*
  QR2用キー初期化関数
  引数　：なし
  戻り値：なし
  用途　：マッチメイク完了後、通常のオンライン状態に戻ってからSBサーバアップデートに
          よる検索に引っ掛からないようにするため、QR2キーを初期化する
 *---------------------------------------------------------------------------*/
void DWCi_ClearQR2Key(void)
{

    // サーバクライアントマッチメイクのサーバが、まだマッチメイクが続くのに
    // ここに来るように作ってはいけない！！

    if (DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_SC_SV){
        DWCi_GetMatchCnt()->qr2NNFinishCount = 0;  // 通常はここに来る時には0になっている
        DWCi_GetMatchCnt()->qr2NumEntry      = 0;
        //DWCi_GetMatchCnt()->qr2Reservation   = 0;

        // 状態の変化をマスターサーバに通知する
        qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
    }
}


/*---------------------------------------------------------------------------*
  マッチメイク完了同期調整パケット受信処理関数
  引数　：aid  送信元ホストのAID
          type DWC Transportパケットタイプ。DWC_SEND_TYPE_*で定義される
          data ヘッダに付随したデータの先頭へのポインタ。
  戻り値：なし
  用途　：マッチメイク完了同期調整のパケット受信時の処理を行う
 *---------------------------------------------------------------------------*/
void DWCi_ProcessMatchSynPacket(u8 aid, u16 type, u8* data)
{
    u8  aidIdx;
    u32 bitmask;
    int i;

    DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "Received SYN %d packet from aid %d.\n",
               type-DWC_SEND_TYPE_MATCH_SYN, aid);

    switch (type){
    case DWC_SEND_TYPE_MATCH_SYN:
        if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_WAITING){
            // SYNパケットを始めて受信した時
            if (data[0] == 1){
                // 自分が新規接続クライアントの場合。
                // 全てのクライアントとのコネクションが確立された。
                // コールバック用のプロファイルIDは0。
                DWCi_GetMatchCnt()->cbEventPid = 0;
            }

            aidIdx = data[1];
            // 自分のAIDを登録する
            DWCi_GetMatchCnt()->aidList[aidIdx] = data[2];
            // 自分のプロファイルIDも登録しておく
            DWCi_GetMatchCnt()->sbPidList[aidIdx] = DWCi_GetMatchCnt()->profileID;

            // 指定人数以下でのマッチメイク完了オプションのために、
            // 接続希望人数を実際の接続数に合わせる
            if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY) ||
                (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND)){
                DWCi_GetMatchCnt()->qr2NumEntry = (u8)DWCi_GetMatchCnt()->gt2NumConnection;
            }

            // クライアントのタイムアウト発生回数カウンタを初期化する
            DWCi_GetMatchCnt()->clWaitTimeoutCount = 0;

            // マッチメイク完了同期状態に進む
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SYN);
        }

        // サーバにSYN-ACKパケットを送信する
        DWCi_SendMatchSynPacket(aid, DWC_SEND_TYPE_MATCH_SYN_ACK);
        break;

    case DWC_SEND_TYPE_MATCH_SYN_ACK:
        if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_SYN){
            // SYN-ACK待ち状態の場合
            // SYN-ACK受信AIDビットマップセット
            DWCi_GetMatchCnt()->synAckBit |= 1 << aid;

            // クライアントとのレイテンシが最大なら更新
            if ((data[0] | (data[1] << 8)) > DWCi_GetMatchCnt()->baseLatency){
                DWCi_GetMatchCnt()->baseLatency = (u16)(data[0] | (data[1] << 8));
            }

            bitmask = DWCi_GetAIDBitmask(FALSE);  // 判定用AIDビットマスクを取得
            if (DWCi_GetMatchCnt()->synAckBit == bitmask){
                // 全クライアントからSYN-ACKが返ってきたら、全クライアントに
                // ACKを送信する
                for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
                    DWCi_SendMatchSynPacket(DWCi_GetMatchCnt()->aidList[i],
                                            DWC_SEND_TYPE_MATCH_ACK);
                }

                // マッチメイク完了同期待ち状態に進む
                DWCi_SetMatchStatus(DWC_MATCH_STATE_SV_SYN_WAIT);

                DWC_Printf(DWC_REPORTFLAG_MATCH_GT2,
                           "Wait max latency %d msec.\n",
                           DWCi_GetMatchCnt()->baseLatency);
            }
        }
        else {
            // ACKを送信した後、個別に遅れてきたSYN-ACKに対応する場合
            // すぐにACKを返信する
            DWCi_SendMatchSynPacket(aid, DWC_SEND_TYPE_MATCH_ACK);
        }
        break;

    case DWC_SEND_TYPE_MATCH_ACK:
        if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_SYN){
            // マッチメイク完了処理を呼び出す
            DWCi_PostProcessConnection(DWC_PP_CONNECTION_SYN_FINISH);
        }
        break;
    }
}


/*---------------------------------------------------------------------------*
  マッチメイク処理用コネクションクローズ処理関数
  引数　：error     クローズ処理のエラーをDWCError型に変換した値
          errorCode エラーコード
          profileID クローズホストのプロファイルID
  戻り値：TRUE :マッチメイク中のキャンセルやタイムアウト処理上のクローズだった、
          FALSE:また別のクローズだった
  用途　：マッチメイク中にコネクションクローズが発生した場合に
          gt2ConnectionCloseコールバック内から呼び出し、クライアントとしての
          マッチメイク処理を再開する。
          または、マッチメイクがキャンセルされた場合にもクローズが行われる。
 *---------------------------------------------------------------------------*/
BOOL DWCi_ProcessMatchClosing(DWCError error, int errorCode, int profileID)
{
#pragma unused(profileID)

    if (DWC_GetState() != DWC_STATE_MATCHING) return FALSE;

    // [test]
    // gt2Connectionが確立されてからconnectedコールバックが
    // 呼び出されるまでの僅かな時間に万が一ここに来れるなら
    // 何もしない、という対策が必要
    if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV){
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWCi_ProcessMatchClosing: SC_SV.\n");
        return TRUE;
    }

    if (error){
        // マッチメイク終了処理呼び出し
        DWCi_StopMatching(error, errorCode+DWC_ECODE_SEQ_MATCH);
        return TRUE;
    }

    // dwc_main.cのDWCi_GT2ClosedCallback()内で呼び出される
    // DWCi_DeleteAID()の特性上、aidListがずれて残っているので、
    // ここでサーバのaidを初期化する
    DWCi_GetMatchCnt()->aidList[0] = 0;

    if ((DWCi_GetMatchCnt()->cancelState == DWC_MATCH_CANCEL_STATE_EXEC) ||
        (DWCi_GetMatchCnt()->closeState == DWC_MATCH_CLOSE_STATE_CHANGE_TO_CLIENT) ||
        (DWCi_GetMatchCnt()->closeState == DWC_MATCH_CLOSE_STATE_TIMEOUT))
        // 自分でキャンセルした場合、もしくはクローズした場合は何もしない
        return TRUE;

    
    if (DWCi_GetMatchCnt()->nnInfo.cookie){
        // NATネゴシエーション中ならキャンセル
        NNCancel(DWCi_GetMatchCnt()->nnInfo.cookie);
        DWCi_GetMatchCnt()->nnInfo.cookie = 0;
    }

    if (DWCi_GetMatchCnt()->gt2NumConnection){
        // まだコネクションが残っている場合
        if (DWCi_GetMatchCnt()->closeState == DWC_MATCH_CLOSE_STATE_INIT){
            // 他ホストから初めてクローズされた場合、他ホストからの
            // クローズによる、残りホストのクローズ中であることを記録し、
            // 全てのコネクションをクローズする
            DWCi_GetMatchCnt()->closeState = DWC_MATCH_CLOSE_STATE_FORCED;
            gt2CloseAllConnectionsHard(*DWCi_GetMatchCnt()->pGt2Socket);
        }
    }
    else {
        if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL){
            // サーバクライアントマッチメイクのクライアントは
            // クローズされたらマッチメイクをエラーで終了する
            // [todo]
            // 新規接続クライアントがサーバ以外と接続中に、サーバ以外の
            // クライアントと切断、もしくは反応がなくなっても、
            // マッチメイクを最初からやり直すようにした方が良い
            DWCi_StopMatching(DWC_ERROR_NETWORK,
                              DWC_ECODE_SEQ_MATCH+DWC_ECODE_TYPE_SC_CL_FAIL);
        }
        else if (DWCi_GetMatchCnt()->priorProfileID){
            // 優先予約先サーバのプロファイルIDがセットされている場合
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Reserve NN to %u.\n",
                       DWCi_GetMatchCnt()->priorProfileID);
            
            (void)DWCi_ResumeMatching();
        }
        else if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_WAITING){
            // 既にクライアントとして接続し、待ち状態にあったホストの場合、
            // サーバから優先予約先が送られて来るかもしれないので、
            // しばらく待つ。
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Wait prior profileID.\n");

            // stateを変更しないとNN要求などを受けてしまうので専用の
            // stateに移行する
            DWCi_SetMatchStatus(DWC_MATCH_STATE_WAIT_CLOSE);
            DWCi_GetMatchCnt()->closedTime = OS_GetTick();
        }
        else {
            // タイムアウトを検知した他ホストからのクローズか、
            // マッチメイクをキャンセルされたホストの場合はここに来るので、
            // マッチメイクを最初からやり直す
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Restart matching immediately.\n");
            
            DWCi_RestartFromCancel(DWC_MATCH_RESET_RESTART);
        }
    }

    return TRUE;
}


/*---------------------------------------------------------------------------*
  サーバクライアントマッチメイク用新規接続クライアントクローズ処理関数
  引数　：clientPid クローズしたクライアントのプロファイルID
  戻り値：なし
  用途　：サーバクライアントマッチメイクの新規接続クライアント接続中に、
          新規接続クライアントがクローズした場合に、gt2ConnectionClose
          コールバック内から呼び出し、接続済みクライアントにキャンセルを通知する。
          この関数を呼び出すのは必ずサーバ。
 *---------------------------------------------------------------------------*/
void DWCi_ProcessMatchSCClosing(int clientPid)
{

    if (DWCi_GetMatchCnt()->closeState != DWC_MATCH_CLOSE_STATE_TIMEOUT){
        // 自分でクローズしたのでなければ接続済みクライアントに
        // キャンセルコマンドを送信し、サーバ処理をキャンセルする
        DWCi_CancelPreConnectedServerProcess(clientPid);
    }
}


/*---------------------------------------------------------------------------*
  ホストデータリスト削除関数（プロファイルID指定版）
  引数　：profileID 削除したいホストのプロファイルID
          numHost   接続ホスト数
  戻り値：TRUE:削除実行した、FALSE:指定のプロファイルIDがリストになかった
  用途　：各種ホストデータリストから、プロファイルIDで指定されたホストの
          データを削除する
 *---------------------------------------------------------------------------*/
BOOL DWCi_DeleteHostByProfileID(int profileID, int numHost)
{
    int i;

    if (!DWCi_GetMatchCnt()) return FALSE;

    for (i = 0; i < numHost; i++){
        if (DWCi_GetMatchCnt()->sbPidList[i] == profileID){
            DWCi_DeleteHostByIndex(i, numHost);
            return TRUE;
        }
    }

    return FALSE;
}


/*---------------------------------------------------------------------------*
  ホストデータリスト削除関数（インデックス指定版）
  引数　：index   削除したいホストのインデックス
          numHost 接続ホスト数
  戻り値：削除したホストのプロファイルID。シャットダウン後に呼ばれたら0を返す。
  用途　：各種ホストデータリストから、インデックスで指定されたホストの
          データを削除する
 *---------------------------------------------------------------------------*/
int DWCi_DeleteHostByIndex(int index, int numHost)
{
    int profileID;

    if (!DWCi_GetMatchCnt()) return 0;

    profileID = DWCi_GetMatchCnt()->sbPidList[index];

    // 有効なAIDビットマップのビットを下ろす
    DWCi_GetMatchCnt()->validAidBitmap &= ~(1 << DWCi_GetMatchCnt()->aidList[index]);
    // 有効コネクション数もセットで更新
    DWCi_SetNumValidConnection();

    if (index < numHost-1){
        // 1つ前にずらす
        int i;
        for ( i = 0 ; i < numHost - index - 1 ; ++i )
        {
            DWCi_GetMatchCnt()->qr2IPList  [index+i] = DWCi_GetMatchCnt()->qr2IPList  [index+i+1];
            DWCi_GetMatchCnt()->qr2PortList[index+i] = DWCi_GetMatchCnt()->qr2PortList[index+i+1];
            DWCi_GetMatchCnt()->sbPidList  [index+i] = DWCi_GetMatchCnt()->sbPidList  [index+i+1];
            DWCi_GetMatchCnt()->ipList     [index+i] = DWCi_GetMatchCnt()->ipList     [index+i+1];
            DWCi_GetMatchCnt()->portList   [index+i] = DWCi_GetMatchCnt()->portList   [index+i+1];
            DWCi_GetMatchCnt()->aidList    [index+i] = DWCi_GetMatchCnt()->aidList    [index+i+1];
        }
    }

    if ( numHost > 0 )
    {
        DWCi_GetMatchCnt()->qr2IPList[numHost-1]   = 0;
        DWCi_GetMatchCnt()->qr2PortList[numHost-1] = 0;
        DWCi_GetMatchCnt()->sbPidList[numHost-1]   = 0;
        DWCi_GetMatchCnt()->ipList[numHost-1]      = 0;
        DWCi_GetMatchCnt()->portList[numHost-1]    = 0;
        DWCi_GetMatchCnt()->aidList[numHost-1]     = 0;
    }

    return profileID;
}


/*---------------------------------------------------------------------------*
  全てのコネクション数取得関数
  引数　：なし
  戻り値：自分に接続しているホスト数（自分を含まない）。
          オフライン時は0を返す。
  用途　：自分と接続している、ホストの数を取得する。
          マッチメイク進行中も現在の接続数をそのまま返す。
 *---------------------------------------------------------------------------*/
int DWCi_GetNumAllConnection(void)
{

    if (!DWCi_GetMatchCnt()) return 0;
    return DWCi_GetMatchCnt()->gt2NumConnection;
}


/*---------------------------------------------------------------------------*
  有効コネクション数取得関数
  引数　：なし
  戻り値：自分に接続しているホスト数（自分を含まない）。
          オフライン時は0を返す。
  用途　：自分と接続している、有効なホスト（全ホストと相互に繋がっているホスト）
          の数を取得する。
          主に、ゲームの通信を行いながら、同時にマッチメイクが進行する場合に、
          マッチメイク完了前に部分的に接続されているホストを除いた数を
          取得したい場合に使う。
 *---------------------------------------------------------------------------*/
int DWCi_GetNumValidConnection(void)
{
    
    if (!DWCi_GetMatchCnt()) return 0;
    return DWCi_GetMatchCnt()->gt2NumValidConn;
}


/*---------------------------------------------------------------------------*
  有効コネクション数セット関数
  引数　：なし
  戻り値：なし
  用途　：自分と接続している、有効なホスト（全ホストと相互に繋がっているホスト）
          の数を制御構造体のメンバに記録する。
          validAidBitmapが変更された時は必ずセットする必要がある。
 *---------------------------------------------------------------------------*/
void DWCi_SetNumValidConnection(void)
{
    int count = -1;
    int i;

    for (i = 0; i < DWC_MAX_CONNECTIONS; i++){
        if (DWCi_GetMatchCnt()->validAidBitmap & (1 << i))
            count++;
    }

    if (count == -1) DWCi_GetMatchCnt()->gt2NumValidConn = 0;
    else DWCi_GetMatchCnt()->gt2NumValidConn = (u8)count;
}


/*---------------------------------------------------------------------------*
  全ての接続中ホストのAIDリスト取得関数
  引数　：aidList AIDリストへのポインタ
  戻り値：AIDリスト長（自分を含む）
  用途　：AIDリストへのポインタを取得する（コピーはしない）。
          AIDリストは、現在接続中のAIDが前から順に詰められたリストで、
          その要素数は自分も含めた接続中ホスト数になる。
          マッチメイク進行中も現在の接続AIDと接続数をそのまま返す。
 *---------------------------------------------------------------------------*/
int DWCi_GetAllAIDList(u8** aidList)
{

    if (!DWCi_GetMatchCnt()) return 0;

    *aidList = DWCi_GetMatchCnt()->aidList;
    return DWCi_GetMatchCnt()->gt2NumConnection+1;
}


/*---------------------------------------------------------------------------*
  有効な接続中ホストのAIDリスト取得関数
  引数　：aidList AIDリストへのポインタ
  戻り値：AIDリスト長（自分を含む）
  用途　：AIDリストへのポインタを取得する（コピーはしない）。
          AIDリストは、現在有効なAIDが前から順に詰められたリストで、
          その要素数は自分も含めた接続中ホスト数になる。
          主に、ゲームの通信を行いながら、同時にマッチメイクが進行する場合に、
          マッチメイク完了前に部分的に接続されているホストを除いたAIDのリストを
          取得したい場合に使う。
 *---------------------------------------------------------------------------*/
int DWCi_GetValidAIDList(u8** aidList)
{
    static u8 validAidList[DWC_MAX_CONNECTIONS];
    int i;

    if (!DWCi_GetMatchCnt()) return 0;

    // 参照用有効AIDリストクリア
    MI_CpuClear8(validAidList, sizeof(validAidList));
    
    for (i = 0; i <= DWCi_GetMatchCnt()->gt2NumValidConn; i++){
        if (DWCi_GetMatchCnt()->validAidBitmap & (1 << DWCi_GetMatchCnt()->aidList[i])){
            // 有効なAIDならリストにセット
            validAidList[i] = DWCi_GetMatchCnt()->aidList[i];
        }
        else break;
        // 前詰めなので、無効なAIDが1つあればそれ以降に有効なAIDはない
    }

    *aidList = validAidList;

    return DWCi_GetMatchCnt()->gt2NumValidConn+1;
}


/*---------------------------------------------------------------------------*
  サーバクライアントマッチメイク・サーバGPstatusセット関数
  引数　：なし
  戻り値：GP処理結果型
  用途　：サーバクライアントマッチメイク時のサーバ用gp Statusをセットする
 *---------------------------------------------------------------------------*/
GPResult DWCi_GPSetServerStatus(void)
{
    char valueStr[12], keyValueStr[32];

    if (DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_SC_SV) return GP_NO_ERROR;

    // 希望接続人数と現在の接続人数をGPステータス文字列として作成する
    (void)OS_SNPrintf(valueStr, sizeof(valueStr), "%u", DWCi_GetMatchCnt()->qr2NumEntry+1);
    (void)DWC_SetCommonKeyValueString(DWC_GP_SSTR_KEY_MATCH_SC_MAX,
                                      valueStr, keyValueStr, '/');
    (void)OS_SNPrintf(valueStr, sizeof(valueStr), "%u", DWCi_GetMatchCnt()->gt2NumConnection+1);
    (void)DWC_AddCommonKeyValueString(DWC_GP_SSTR_KEY_MATCH_SC_NUM,
                                      valueStr, keyValueStr, '/');

    // マッチメイクバージョンをセットする。
    // サーバクライアントマッチメイクの場合は無差別にマッチメイクしないので、
    // このステータスは参照しないが、確認用にセットだけはしておく。
    (void)OS_SNPrintf(valueStr, sizeof(valueStr), "%u", DWC_MATCHING_VERSION);
    (void)DWC_AddCommonKeyValueString(DWC_GP_SSTR_KEY_MATCH_VERSION,
                                      valueStr, keyValueStr, '/');

    // GPステータスをサーバクライアントマッチメイクのサーバ状態にし、
    // 作成したGPステータス文字列もセットする
    return DWCi_SetGPStatus(DWC_STATUS_MATCH_SC_SV, keyValueStr, NULL);
}


/*---------------------------------------------------------------------------*
  マッチメイク制御オブジェクト解放関数
  引数　：なし
  戻り値：なし
  用途　：マッチメイク制御オブジェクトへのポインタをNULLクリアする
 *---------------------------------------------------------------------------*/
void DWCi_ShutdownMatch(void)
{

  DWCi_SetMatchCnt(NULL); //stpMatchCnt = NULL;

    // フィルタ文字列を解放
    if (stpAddFilter){
        DWC_Free(DWC_ALLOCTYPE_BASE, stpAddFilter, 0);
        stpAddFilter = NULL;
    }

    // ゲーム定義のQR2用キーデータ配列を初期化
    DWCi_ClearGameMatchKeys();

    // マッチメイクオプション制御構造体を解放
    if (stpOptMinComp != NULL){
        DWC_Free(DWC_ALLOCTYPE_BASE, stpOptMinComp, 0);
        stpOptMinComp = NULL;
    }

    stOptSCBlock.valid = 0;
    stOptSCBlock.lock  = 0;
}


/*---------------------------------------------------------------------------*
  マッチメイクシャットダウンチェック関数
  引数　：なし
  戻り値：TRUE:マッチメイクがシャットダウンされている、FALSE:正常
  用途　：DWCi_ShutdownMatch()が呼ばれて、マッチメイクがシャットダウンされた
          状態かどうかを調べる
 *---------------------------------------------------------------------------*/
BOOL DWCi_IsShutdownMatch(void)
{

    if (!DWCi_GetMatchCnt()) return TRUE;
    else return FALSE;
}


//----------------------------------------------------------------------------
// function - static
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  マッチメイクごと制御構造体初期化関数
  引数　：なし
  戻り値：なし
  用途　：マッチメイクごとに初期化が必要な制御構造体メンバを初期化する
 *---------------------------------------------------------------------------*/
static void DWCi_ResetMatchParam(DWCMatchResetLevel level)
{

    DWCi_GetMatchCnt()->gt2ConnectCount      = 0;
    DWCi_GetMatchCnt()->nnRecvCount          = 0;
    DWCi_GetMatchCnt()->nnCookieRand         = (u16)DWCi_GetMathRand32(0x10000);
    DWCi_GetMatchCnt()->nnLastCookie         = 0;
    DWCi_GetMatchCnt()->nnFailedTime         = 0;
    DWCi_GetMatchCnt()->nnFinishTime         = 0;
    DWCi_GetMatchCnt()->clLinkProgress       = 0;
    DWCi_GetMatchCnt()->cancelState          = DWC_MATCH_CANCEL_STATE_INIT;
    DWCi_GetMatchCnt()->scResvRetryCount     = 0;
    DWCi_GetMatchCnt()->synResendCount       = 0;
    DWCi_GetMatchCnt()->cancelSynResendCount = 0;
    DWCi_GetMatchCnt()->resvWaitCount        = 0;
    DWCi_GetMatchCnt()->closeState           = DWC_MATCH_CLOSE_STATE_INIT;
    DWCi_GetMatchCnt()->cancelBaseLatency    = 0;
    DWCi_GetMatchCnt()->searchPort           = 0;
    DWCi_GetMatchCnt()->searchIP             = 0;
    DWCi_GetMatchCnt()->lastSynSent          = 0;
    DWCi_GetMatchCnt()->closedTime           = 0;
    MI_CpuClear32(&DWCi_GetMatchCnt()->cmdCnt, sizeof(DWCi_GetMatchCnt()->cmdCnt));

    if (level == DWC_MATCH_RESET_CONTINUE){
        // 引き続きマッチメイクを行う場合はNNだけ終了したホストのカウントを
        // クリアする
        DWCi_GetMatchCnt()->qr2NNFinishCount = (u8)DWCi_GetMatchCnt()->gt2NumConnection;

        // サーバクライアントマッチメイクの時はstateもセットする
        if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL){
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAITING);
        }
        else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV){
            DWCi_SetMatchStatus(DWC_MATCH_STATE_SV_WAITING);
        }
    }
    else {
        DWCi_GetMatchCnt()->gt2NumConnection = 0;
        DWCi_GetMatchCnt()->gt2NumValidConn  = 0;
        
        DWCi_GetMatchCnt()->qr2NNFinishCount = 0;
        DWCi_GetMatchCnt()->qr2IsReserved    = 0;
        DWCi_GetMatchCnt()->qr2Reservation   = 0;
        
//        DWCi_GetMatchCnt()->sbUpdateCount    = 0;
        DWCi_GetMatchCnt()->sbUpdateFlag     = FALSE;

        DWCi_GetMatchCnt()->friendCount      = 0;
        DWCi_GetMatchCnt()->baseLatency      = 0;
//        DWCi_GetMatchCnt()->cmdResendCount   = 0;
        DWCi_GetMatchCnt()->cmdResendFlag    = FALSE;
        DWCi_GetMatchCnt()->cmdResendTick    = 0;
//        DWCi_GetMatchCnt()->cmdTimeoutCount  = 0;
        DWCi_GetMatchCnt()->cmdTimeoutTime = 0;
        DWCi_GetMatchCnt()->cmdTimeoutStartTick = 0;
        DWCi_GetMatchCnt()->synAckBit        = 0;
        DWCi_GetMatchCnt()->reqProfileID     = 0;
        DWCi_GetMatchCnt()->priorProfileID   = 0;
        DWCi_GetMatchCnt()->validAidBitmap   = 0;

        MI_CpuClear32(DWCi_GetMatchCnt()->qr2IPList, sizeof(DWCi_GetMatchCnt()->qr2IPList));
        MI_CpuClear16(DWCi_GetMatchCnt()->qr2PortList, sizeof(DWCi_GetMatchCnt()->qr2PortList));
        MI_CpuClear32(DWCi_GetMatchCnt()->sbPidList, sizeof(DWCi_GetMatchCnt()->sbPidList));
        MI_CpuClear32(&DWCi_GetMatchCnt()->nnInfo, sizeof(DWCNNInfo));
        MI_CpuClear32(DWCi_GetMatchCnt()->ipList, sizeof(DWCi_GetMatchCnt()->ipList));
        MI_CpuClear16(DWCi_GetMatchCnt()->portList, sizeof(DWCi_GetMatchCnt()->portList));
        MI_CpuClear8(DWCi_GetMatchCnt()->aidList, sizeof(DWCi_GetMatchCnt()->aidList));
        MI_CpuClear32(DWCi_GetMatchCnt()->svDataBak, sizeof(DWCi_GetMatchCnt()->svDataBak));

        if (level == DWC_MATCH_RESET_RESTART){
            // 最初からマッチメイクをやり直す場合はstateも初期状態に戻す
            if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY){
                DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SEARCH_HOST);
            }
            else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND){
                DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAIT_RESV);
            }
            // サーバクライアントマッチメイクはこのlevelにはならない
        }
        else {
            // 以下のパラメータは DWC_MATCH_RESET_ALL の時しかクリアしない
            DWCi_GetMatchCnt()->qr2MatchType       = 0;
            DWCi_GetMatchCnt()->qr2NumEntry        = 0;
#ifdef DWC_QR2_ALIVE_DURING_MATCHING
            DWCi_GetMatchCnt()->qr2ShutdownFlag    = 0;
#endif
            DWCi_GetMatchCnt()->cbEventPid         = 0;
            DWCi_GetMatchCnt()->distantFriend      = 0;
#ifdef DWC_STOP_SC_SERVER
            DWCi_GetMatchCnt()->stopSCFlag         = 0;
#endif
            DWCi_GetMatchCnt()->clWaitTimeoutCount = 0;
            DWCi_GetMatchCnt()->clWaitTime         = 0;
            DWCi_GetMatchCnt()->newClientCallback  = NULL;
            DWCi_GetMatchCnt()->newClientParam     = NULL;
        }
    }
}


/*---------------------------------------------------------------------------*
  マッチメイク共通パラメータ初期化関数
  引数　：matchType マッチメイクタイプ。DWC_MATCH_TYPE_*列挙子
          numEntry  接続要求人数（自分は含まない）
          callback  マッチメイク完了コールバック
          param     マッチメイク完了コールバック用パラメータ
  戻り値：なし
  用途　：マッチメイク開始時の共通パラメータ設定を行う
 *---------------------------------------------------------------------------*/
static void DWCi_SetMatchCommonParam(u8  matchType,
                                     u8  numEntry,
                                     DWCMatchedSCCallback callback,
                                     void* param)
{

    // マッチメイクごとに初期化が必要なパラメータを初期化
    DWCi_ResetMatchParam(DWC_MATCH_RESET_ALL);

    DWCi_GetMatchCnt()->qr2MatchType    = matchType;
    DWCi_GetMatchCnt()->qr2NumEntry     = numEntry;
    DWCi_GetMatchCnt()->matchedCallback = callback;
    DWCi_GetMatchCnt()->matchedParam    = param;
    DWCi_GetMatchCnt()->nnFailureCount  = 0;

    // サーバのAIDは必ず0
    DWCi_GetMatchCnt()->aidList[0] = 0;

    // カスタムキーを設定する
    qr2_register_key(DWC_QR2_PID_KEY, DWC_QR2_PID_KEY_STR);
    qr2_register_key(DWC_QR2_MATCH_TYPE_KEY, DWC_QR2_MATCH_TYPE_KEY_STR);
    qr2_register_key(DWC_QR2_MATCH_RESV_KEY, DWC_QR2_MATCH_RESV_KEY_STR);
    qr2_register_key(DWC_QR2_MATCH_VER_KEY, DWC_QR2_MATCH_VER_KEY_STR);
    qr2_register_key(DWC_QR2_MATCH_EVAL_KEY, DWC_QR2_MATCH_EVAL_KEY_STR);
}


/*---------------------------------------------------------------------------*
  マッチメイク終了関数
  引数　：なし
  戻り値：なし
  用途　：マッチメイク制御オブジェクトを初期化し、GameSpy SDKが確保している
          オブジェクトを開放する
 *---------------------------------------------------------------------------*/
static void DWCi_CloseMatching(void)
{
    
    DWC_Printf( DWC_REPORTFLAG_MATCH_NN, " Close Matching....\n" );

    if (!DWCi_GetMatchCnt()) return;

#ifndef DWC_BUG_WIFI_DELAY_SEND
    // SBオブジェクト開放
    if (DWCi_GetMatchCnt()->sbObj)
    {
        if(s_sbCallbackLevel == 0) 
        {
            ServerBrowserFree(DWCi_GetMatchCnt()->sbObj);
            DWCi_GetMatchCnt()->sbObj = NULL;
        }
        else
        {
            s_needSbFree = TRUE;
        }
    }
#endif
    // NN使用領域も開放する
    DWCi_NNFreeNegotiateList();

    // 状態を初期化する
    DWCi_SetMatchStatus(DWC_MATCH_STATE_INIT);

    // SBサーバアップデート用フィルタがセットさていたら解放する
    if (stpAddFilter){
        DWC_Free(DWC_ALLOCTYPE_BASE, stpAddFilter, 0);
        stpAddFilter = NULL;
    }

    // ゲーム定義のQR2用キーデータ配列を初期化
    DWCi_ClearGameMatchKeys();

#ifdef DWC_QR2_ALIVE_DURING_MATCHING
    // QR2をシャットダウンするためのフラグを立てる。
    // QR2コールバックを抜けてしまうまではシャットダウンしない。
    // サーバクライアントマッチメイクではここに来ないのでシャットダウンされない
    DWCi_GetMatchCnt()->qr2ShutdownFlag = 1;
#endif
}


#define DWC_NUM_QR2_MATCH_KEY 7  // ライブラリが使用するQR2キーの数
/*---------------------------------------------------------------------------*
  SBサーバアップデート関数
  引数　：profileID プロファイルIDを指定して検索する場合のプロファイルID。
                    指定しないなら0を渡す。
  戻り値：SBエラー種別
  用途　：ServerBrowserLimitUpdate関数を、stateに応じたやり方で呼び出す
 *---------------------------------------------------------------------------*/
static SBError DWCi_SBUpdateAsync(int profileID)
{
    char filter[MAX_FILTER_LEN];
    char* pFilter = filter;
    u8 basicFields[DWC_NUM_QR2_MATCH_KEY+DWC_QR2_GAME_RESERVED_KEYS];
    int len;
    int numBasicFields = DWC_NUM_QR2_MATCH_KEY;
    int i;
    SBError sbError = sbe_noerror;

    // ライブラリが使用するQR2キーをセット
    basicFields[0] = NUMPLAYERS_KEY;
    basicFields[1] = MAXPLAYERS_KEY;
    basicFields[2] = DWC_QR2_PID_KEY;
    basicFields[3] = DWC_QR2_MATCH_TYPE_KEY;
    basicFields[4] = DWC_QR2_MATCH_RESV_KEY;
    basicFields[5] = DWC_QR2_MATCH_VER_KEY;
    basicFields[6] = DWC_QR2_MATCH_EVAL_KEY;

    // ゲーム定義のキーを追加
    if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY) ||
        (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND)){
        for (i = 0; i < DWC_QR2_GAME_RESERVED_KEYS; i++){
            if (stGameMatchKeys[i].keyID){
                basicFields[numBasicFields] = stGameMatchKeys[i].keyID;
                numBasicFields++;
            }
        }
    }

    switch (DWCi_GetMatchCnt()->state){
    case DWC_MATCH_STATE_CL_SEARCH_HOST:
        if (!DWCi_GetMatchCnt()->priorProfileID){
            len = DWCi_GetDefaultMatchFilter(filter, DWCi_GetMatchCnt()->profileID,
                                             DWCi_GetMatchCnt()->qr2NumEntry,
                                             DWCi_GetMatchCnt()->qr2MatchType);

            // ゲームで追加したいフィルタが設定されていれば追加する
            if (stpAddFilter){
                (void)OS_SNPrintf(&filter[len], sizeof(filter) - len,
                                  " and (%s)",
                                  stpAddFilter);
            }
            break;
        }
        else {
            // 優先予約プロファイルIDが設定されている場合は、
            // そのプロファイルIDで検索を行う
            profileID = DWCi_GetMatchCnt()->priorProfileID;
        }

    case DWC_MATCH_STATE_CL_SEARCH_OWN:
    case DWC_MATCH_STATE_CL_WAIT_RESV:
    case DWC_MATCH_STATE_CL_SEARCH_NN_HOST:
        // プロファイルIDのみで検索する
        (void)OS_SNPrintf(filter, sizeof(filter),
                          "%s = %u",
                          DWC_QR2_PID_KEY_STR, profileID);

        // 友達無指定ピアマッチメイク、サーバクライアントマッチメイクの場合は
        // reqProfileID を使わないので、再送用にここでセットする必要がある
        DWCi_GetMatchCnt()->reqProfileID = profileID;
        break;

    default:
        // [test]
        // 有り得ない
        DWC_Printf(DWC_REPORTFLAG_ERROR,
                   "---DWCi_SBUpdateAsync() illegal state %d.\n",
                   DWCi_GetMatchCnt()->state);
    }

    DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "ServerBrowserFilter : %s\n", filter);

#if 1
    // 一度マスターサーバとdisconnectしないとSBUpdateできないので
    // サーバリストクリア
    ServerBrowserClear(DWCi_GetMatchCnt()->sbObj);
#else
    if (ServerBrowserState(DWCi_GetMatchCnt()->sbObj) != sb_disconnected){
        // 一度マスターサーバとdisconnectしないとSBUpdateできないので
        // サーバリストクリア
        ServerBrowserClear(DWCi_GetMatchCnt()->sbObj);
    }
#endif

    // 最大接続希望ホスト数までのサーバリストアップデートを行う。
    for (i = 0; i < DWC_DNS_ERROR_RETRY_MAX; i++){
        sbError = ServerBrowserLimitUpdate(DWCi_GetMatchCnt()->sbObj, SBTrue, SBFalse,
                                           basicFields, numBasicFields,
                                           filter, DWC_SB_UPDATE_MAX_SERVERS);
        if (!sbError) break;
        else if (sbError != sbe_dnserror) break;
        // DNSエラーなら一定回数はリトライする
    }

    // ServerBrowserLimitUpdate呼び出し時間を記録する
    if (!sbError){
        DWCi_GetMatchCnt()->sbUpdateRequestTick = OS_GetTick()+OS_MilliSecondsToTicks(DWC_SB_UPDATE_TIMEOUT);
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "ServerBrowserLimitUpdate timeout set.(%s)\n", __FUNCTION__);
    }

    return sbError;
}


/*---------------------------------------------------------------------------*
  友達無指定ピアマッチメイクのSBサーバアップデート用デフォルトフィルタ文字列取得関数
  引数　：filter    文字列セット先ポインタ
          profileID 自分のプロファイルID
          numEntry  接続要求人数（自分は含まない）
          matchType マッチメイクタイプ
  戻り値：文字列長
  用途　：友達無指定ピアマッチメイクのSBサーバアップデート用デフォルトフィルタ
          文字列を作成・取得する
 *---------------------------------------------------------------------------*/
static int DWCi_GetDefaultMatchFilter(char* filter, int profileID, u8 numEntry, u8 matchType)
{

    // マッチメイク条件が同じで定員割れのQR2ホストを検索する。
    // 古い自分のホストデータに反応しないように pid も確認する。
    return OS_SNPrintf(filter, MAX_FILTER_LEN,
                       "%s = %d and %s != %u and maxplayers = %d and numplayers < %d and %s = %d and %s != %s",
                       DWC_QR2_MATCH_VER_KEY_STR,
                       DWC_MATCHING_VERSION,
                       DWC_QR2_PID_KEY_STR,
                       profileID,
                       numEntry,
                       numEntry,
                       DWC_QR2_MATCH_TYPE_KEY_STR,
                       matchType,
                       DWC_QR2_MATCH_RESV_KEY_STR,
                       DWC_QR2_PID_KEY_STR);
}


/*---------------------------------------------------------------------------*
  NATネゴシエーション開始関数
  引数　：isQR2    1:QR2側、0：SB側
          cookie   QR2側の場合に必要なクッキー値。SB側は関数内で生成する
          server   SBServerオブジェクト。スター型NATネゴシエーション時のSB側で必要
  戻り値：NNエラー種別
  用途　：NATネゴシエーションを、stateに応じたやり方で開始する
 *---------------------------------------------------------------------------*/
static NegotiateError DWCi_NNStartupAsync(int isQR2, int cookie, SBServer server)
{
    u32 ip;
    u32 senddata[2];
    int index = DWCi_GetMatchCnt()->qr2NNFinishCount;
    int result;
    BOOL nnValid;
    NegotiateError nnError = ne_noerror;

    if (!isQR2){
        // NN開始側の場合
        // クッキー値生成
        cookie = (DWCi_GetMatchCnt()->profileID & 0x0ffff) | (DWCi_GetMatchCnt()->nnCookieRand << 16);

        if (SBServerHasPrivateAddress(server)){
            // サーバがNAT内にある場合
            if (SBServerGetPublicInetAddress(server) ==
                ServerBrowserGetMyPublicIPAddr(DWCi_GetMatchCnt()->sbObj)){
                // サーバが同一NAT内にある場合
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                           "Server[%d] is behind same NAT as me.\n", index);

                // 実際に使用するIP、ポートはプライベート値になる
                DWCi_GetMatchCnt()->ipList[index]   = SBServerGetPrivateInetAddress(server);
                DWCi_GetMatchCnt()->portList[index] = SBServerGetPrivateQueryPort(server);

                nnValid = FALSE;  // NATネゴシエーションは必要ない
            }
            else {
                // サーバが別のNAT内にある場合
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                           "Server[%d] is behind NAT.\n", index);
                
                // 実際に使用するIP、ポートはNATネゴシエーションで決まる
                nnValid = TRUE;  // NATネゴシエーションが必要
            }
        }
        else {
            // サーバがNAT外にある場合
            ip = (u16)SO_GetHostID();
            if (((ip & 0xffff) == 0xa8c0) ||
                (((ip & 0xff) == 0xac) && ((ip & 0xff00) >= 0x1000) && ((ip & 0xff00) <= 0x1f00)) ||
                ((ip & 0xff) == 0x0a)){
                // 相手がNAT外でも自分がNAT内ならNATネゴシエーションが必要
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                           "Server[%d] is not behind NAT. But I'm behind NAT.\n",
                           index);

                // 実際に使用するIP、ポートはNATネゴシエーションで決まる
                nnValid = TRUE;  // NATネゴシエーションが必要
            }
            else {
                // お互いNAT外の場合
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                           "Both I and Server[%d] are not behind NAT.\n",
                           index);

                // 実際に使用するIP、ポートはパブリック値になる

                DWCi_GetMatchCnt()->ipList[index]   = SBServerGetPublicInetAddress(server);
                DWCi_GetMatchCnt()->portList[index] = SBServerGetPublicQueryPort(server);
                    
                nnValid = FALSE;  // NATネゴシエーションは必要ない
            }
        }

        if (nnValid){
            // 実際にNATネゴシエーション要求を行う時のみ生成
            DWCi_GetMatchCnt()->nnCookieRand  = (u16)DWCi_GetMathRand32(0x10000);

            // クッキーは実際にNATネゴシエーションを行う場合のみセット
            DWCi_GetMatchCnt()->nnInfo.cookie = cookie;
        }
        else {
            // NATネゴシエーション不要の場合は、相手にIPとポート番号を教えて
            // NATネゴシエーション完了状態にしてもらう
            senddata[0] = (u32)SO_GetHostID();
            senddata[1] = gt2GetLocalPort(*DWCi_GetMatchCnt()->pGt2Socket);

            // コマンド送信
            result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_TELL_ADDR,
                                           DWCi_GetMatchCnt()->sbPidList[index],
                                           SBServerGetPublicInetAddress(server),
                                           SBServerGetPublicQueryPort(server),
                                           senddata, 2);

            DWCi_GetMatchCnt()->cmdCnt.count = 0;  // コマンド再送カウント開始
                                               
            // [todo]
            // ここのエラーはどう処理する？
            if (result) return ne_socketerror;

            // 実際にNATネゴシエーションを行わない場合はクッキーをセットしない
            DWCi_GetMatchCnt()->nnInfo.cookie = 0;
        }

        // NNパラメータは必ず設定する。NATネゴシエーション不要の場合でも
        // NN完了コールバック内でメンバisQR2のみ参照する。
        DWCi_GetMatchCnt()->nnInfo.isQR2      = 0;
        DWCi_GetMatchCnt()->nnInfo.retryCount = 0;
        DWCi_GetMatchCnt()->nnInfo.port       = SBServerGetPublicQueryPort(server);
        DWCi_GetMatchCnt()->nnInfo.ip         = SBServerGetPublicInetAddress(server);
    }
    else {
        // QR2側はここでNNパラメータ設定
        DWCi_GetMatchCnt()->nnInfo.isQR2      = 1;
        DWCi_GetMatchCnt()->nnInfo.retryCount = 0;
        DWCi_GetMatchCnt()->nnInfo.port       = 0;
        DWCi_GetMatchCnt()->nnInfo.ip         = 0;
        DWCi_GetMatchCnt()->nnInfo.cookie     = cookie;

        nnValid = TRUE;  // NATネゴシエーションが必要
    }

    
    if (nnValid){
        // NATネゴシエーション処理実行
        // エラー処理は中で行っている
        nnError = DWCi_DoNatNegotiationAsync(&DWCi_GetMatchCnt()->nnInfo);
    }
    else {
        // NATネゴシエーション不要の場合は、NATネゴシエーション完了
        // コールバックを呼び出す。
        // remoteaddr = userdata = NULL として目印にする。
        DWCi_NNCompletedCallback(nr_success,
                                 gt2GetSocketSOCKET(*DWCi_GetMatchCnt()->pGt2Socket),
                                 NULL, &DWCi_GetMatchCnt()->nnInfo);

        // 実際にNATネゴシエーションを行わなかった場合は、NN終了時の時刻を
        // クリアする
        DWCi_GetMatchCnt()->nnFinishTime = 0;
    }

    return nnError;
}


/*---------------------------------------------------------------------------*
  NATネゴシエーション開始関数
  引数　：nnInfo NATネゴシエーションパラメータ構造体へのポインタ
  戻り値：NNエラー種別
  用途　：NATネゴシエーションパラメータに基づいて、実際にネゴシエーションを開始する
 *---------------------------------------------------------------------------*/
static NegotiateError DWCi_DoNatNegotiationAsync(DWCNNInfo* nnInfo)
{
    SBError sbError;
    NegotiateError nnError;
    int i;

    if (!nnInfo->isQR2){
        // QR2側でなければクッキーをサーバに送信
        sbError =
            ServerBrowserSendNatNegotiateCookieToServer(
                DWCi_GetMatchCnt()->sbObj,
                gt2AddressToString(nnInfo->ip, 0, NULL),
                nnInfo->port,
                nnInfo->cookie);
        // [todo]
        // ここのエラーはどう処理する？
        if (DWCi_HandleSBError(sbError)) return ne_socketerror;

        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Send NN cookie = %x.\n",
                   nnInfo->cookie);
    }

    // NATネゴシエーション開始
    for ( i = 0 ; i < DWC_DNS_ERROR_RETRY_MAX ; ++i )
    {
        nnError =
                NNBeginNegotiationWithSocket(gt2GetSocketSOCKET(*DWCi_GetMatchCnt()->pGt2Socket),
                                             nnInfo->cookie, nnInfo->isQR2,
                                             DWCi_NNProgressCallback,
                                             DWCi_NNCompletedCallback,
                                             nnInfo);
        if ( nnError == ne_noerror || nnError != ne_dnserror) break;

        DWC_Printf(DWC_REPORTFLAG_DEBUG, " dns error occurs when NatNegotiation begin... retry\n");
    }

    return nnError;
}


/*---------------------------------------------------------------------------*
  マッチメイクコマンド送信関数
  引数　：command   送信したいコマンド定数
          profileID 送信先ホストのプロファイルID
          ip        友達無指定ピアマッチメイクの場合の送信先ホストのQR2用IP
          port      友達無指定ピアマッチメイクの場合の送信先ホストのQR2用ポート番号
          data      コマンドに追加して送りたいデータ（u32型配列へのポインタ）。
                    なければNULLを渡す
          len       配列dataの要素数。data=NULLなら何を渡しても無視される。
  戻り値：SBError型かGPResult型をint型に変換した値
  用途　：マッチメイクコマンドを、SBメッセージかGPバディメッセージかを
          選択して送信する
 *---------------------------------------------------------------------------*/
static int  DWCi_SendMatchCommand(u8 command, int profileID, u32 ip, u16 port, const u32 data[], int len)
{
    char message[DWC_MATCH_COMMAND_ADD_MESSAGE_MAX];
    char tmpStr[16];
    int msgLen = 0, tmpLen;
    int result;
    int i;

    if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY) ||
        (((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL) ||
          DWCi_GetMatchCnt()->distantFriend) &&
         (command == DWC_MATCH_COMMAND_TELL_ADDR))){
        // SBメッセージコマンド送信
        // サーバクライアントマッチメイクでも、クライアント同士は
        // バディでない可能性があるのでSBメッセージを使う。
        // 同様に、友達の友達許可（友達指定ピアマッチメイクの場合のみ）の
        // 場合も上記コマンドに限りSBメッセージを使う。
        result = (int)DWCi_SendSBMsgCommand(command, ip, port, data, len);
    }
    else {
        if (data && len){
            // u32の配列データを文字列に変換する
            msgLen = OS_SNPrintf(message, sizeof(message), "%u", data[0]);

            for (i = 1; i < len; i++){
                tmpLen = OS_SNPrintf(tmpStr, sizeof(tmpStr), "/%u", data[i]);

                // メッセージ文字列追加(バッファ長に達したら追加を止める)
                if ( msgLen + tmpLen + 1 > sizeof(message) )
                {
                    SDK_ASSERTMSG( 0, "Match command is too long." );
                    break;
                }
                MI_CpuCopy8(tmpStr, &message[msgLen], (u32)tmpLen);
                msgLen += tmpLen;
            }
        }
        message[msgLen] = '\0';

        // GPバディメッセージコマンド送信
        result =
            (int)DWCi_SendGPBuddyMsgCommand(DWCi_GetMatchCnt()->pGpObj,
                                            command,
                                            profileID,
                                            message);
    }

    // 再送に関わる送信コマンドのデータのみストックしておく
    if ((command == DWC_MATCH_COMMAND_RESV_OK)      ||
        (command == DWC_MATCH_COMMAND_TELL_ADDR)    ||
        (command == DWC_MATCH_COMMAND_LINK_CLS_REQ) ||
        (command == DWC_MATCH_COMMAND_LINK_CLS_SUC)){
        DWCi_GetMatchCnt()->cmdCnt.command   = command;
        DWCi_GetMatchCnt()->cmdCnt.port      = port;
        DWCi_GetMatchCnt()->cmdCnt.ip        = ip;
        DWCi_GetMatchCnt()->cmdCnt.profileID = profileID;
        DWCi_GetMatchCnt()->cmdCnt.len       = len;
        DWCi_GetMatchCnt()->cmdCnt.sendTime  = OS_GetTick();
        if (data && len){
            MI_CpuCopy32(data, DWCi_GetMatchCnt()->cmdCnt.data, (u32)len*4);
        }
    }

    return result;
}


/*---------------------------------------------------------------------------*
  マッチメイクSBメッセージコマンド送信関数
  引数　：command 送信したいコマンド定数
          ip      送信先ホストのQR2用IP
          port    送信先ホストのQR2用ポート番号
          data    コマンドに追加して送りたいデータ（u32型配列へのポインタ）。
                  なければNULLを渡す
          len     配列dataの要素数。data=NULLなら何を渡しても無視される。
  戻り値：SBError型
  用途　：SBメッセージでマッチメイクのための制御コマンドを送信する。
 *---------------------------------------------------------------------------*/
static SBError DWCi_SendSBMsgCommand(u8 command, u32 ip, u16 port, const u32 data[], int len)
{
    int i;
    DWCSBMessage senddata;
    SBError sbError;

    // 自分がデータを作りすぎた場合のASSERT
    if ( len > sizeof(senddata.data) / sizeof(u32) )
    {
        SDK_ASSERTMSG( 0, "Match command is too long." );
        len = sizeof(senddata.data) / sizeof(u32);    // バッファに入る長さに丸める
    }

    // 送信データ作成
    if (data && len){
        MI_CpuCopy32(data, senddata.data, sizeof(u32)*len);
    }
    else {
        // data = NULL の時はlenに与えられた値を無視する
        len = 0;
    }

    strcpy(senddata.header.identifier, DWC_SB_COMMAND_STRING);
    senddata.header.version   = DWC_MATCHING_VERSION;
    senddata.header.command   = command;
    senddata.header.size      = (u8)(sizeof(u32)*len);
    senddata.header.qr2Port   = DWCi_GetMatchCnt()->qr2Port;
    senddata.header.qr2IP     = DWCi_GetMatchCnt()->qr2IP;
    senddata.header.profileID = DWCi_GetMatchCnt()->profileID;

#ifdef NITRODWC_DEBUG
    DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
               "<SB> SEND-%s -> [%08x:%d] [pid=--------]\n",
               get_dwccommandstr(command), ip, port);
#else
    DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
               "<SB> SEND-0x%02x -> [%08x:%d] [pid=--------]\n",
               command, ip, port);
#endif
    // SBメッセージ送信
    for (i = 0; i < DWC_DNS_ERROR_RETRY_MAX; i++){
        sbError =
            ServerBrowserSendMessageToServer(DWCi_GetMatchCnt()->sbObj,
                                             gt2AddressToString(ip, 0, NULL),
                                             port,
                                             (gsi_char *)&senddata,
                                             (int)(sizeof(DWCSBMessageHeader)+senddata.header.size));

        if (!sbError) break;
        else if (sbError != sbe_dnserror) break;
        // DNSエラーなら一定回数はリトライする
    }

    return sbError;
}


/*---------------------------------------------------------------------------*
  マッチメイクGPバディメッセージコマンド送信関数
  引数　：connection GPコネクションオブジェクトへのポインタ
          profileID  送信先バディのプロファイルID
          command    送信したいコマンド定数
          message    コマンドに追加して送りたいメッセージ。必要なければNULLを渡す
                     数値も必ず文字列として渡すこと（0が終端とみなされるので）
  戻り値：GP結果型
  用途　：GPバディメッセージでマッチメイクのための制御コマンドを送信する。
 *---------------------------------------------------------------------------*/
static GPResult DWCi_SendGPBuddyMsgCommand(GPConnection* connection, u8 command, int profileID, const char* message)
{
    char tmpStr[DWC_MATCH_COMMAND_ADD_MESSAGE_MAX];
    u32  len, len2;
    GPResult result;

    len = (u32)OS_SNPrintf(tmpStr, sizeof(tmpStr),
                           "%s%dv%s",
                           DWC_GP_COMMAND_STRING,
                           DWC_MATCHING_VERSION,
                           DWC_GP_COMMAND_MATCH_STRING);
    tmpStr[len]   = (char)command;
    tmpStr[len+1] = '\0';

    if (message){
        len2 = strlen(message);

        // [test]
        // 長すぎる追加コマンドメッセージを作っていたら自分への警告
        if ( (len + 1) + len2 + 1 > sizeof(tmpStr) )
        {
            SDK_ASSERTMSG( 0, "Match command is too long." );
            len2 = sizeof(tmpStr) - (len + 1) - 1;    // バッファに入る長さに丸める
        }

        // 追加コマンドメッセージを送信コマンドに追加
        MI_CpuCopy8(message, &tmpStr[len+1], len2);
        tmpStr[len+1+len2] = '\0';
    }

    // バディメッセージ送信
    result = gpSendBuddyMessage(connection, profileID, tmpStr);

    DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
               "<GP> SEND-0x%02x -> [--------:-----] [pid=%u]\n",
               command, profileID);

    return result;
}


/*---------------------------------------------------------------------------*
  GPバディメッセージコマンド追加メッセージ取得関数
  引数　：dstMsg 抜き出した文字列をコピーする配列へのポインタ
          srcMsg GPバディメッセージコマンド追加メッセージ
          index  '/'で区切られた文字列の何個目を取得したいか
  戻り値：文字列長。存在しないインデックスを指定した場合は-1を返す
  用途　：GPバディメッセージコマンドの追加メッセージから、'/'で区切られた
          コマンドのうちのいくつ目かを指定して文字列を取得する
 *---------------------------------------------------------------------------*/
static int  DWCi_GetGPBuddyAdditionalMsg(char* dstMsg, const char* srcMsg, int index)
{
    const char* pSrcBegin = srcMsg;
    char* pSrcNext = NULL;
    char* pSrcEnd;
    int len;
    int i;

    pSrcEnd = strchr(pSrcBegin, '\0');  // 終端のポインタを取得

    // pSrcBeginに指定のインデックスの文字列の先頭へのポインタをセットする
    for (i = 0; i < index; i++){
        pSrcNext = strchr(pSrcBegin, '/');
        if (!pSrcNext){
            // 不正なインデックスの指定
            return -1;
        }
        pSrcBegin = pSrcNext+1;
    }

    // 最後のインデックスの場合も文字列の終端へのポインタをセットする
    //if (!pSrcNext) pSrcNext = pSrcEnd;
    pSrcNext = strchr(pSrcBegin, '/');
    if (!pSrcNext){
        // '/'が見つからなかった場合
        pSrcNext = pSrcEnd;
    }

    // 追加メッセージがないのに呼び出された場合は不正なインデックス指定
    if (pSrcBegin == pSrcNext) return -1;

    // 指定の文字列をコピー
    len = pSrcNext-pSrcBegin;
    MI_CpuCopy8(pSrcBegin, dstMsg, (u32)len);
    dstMsg[len] = '\0';

    return len;
}


/*---------------------------------------------------------------------------*
  マッチメイクコマンド処理関数
  引数　：command 受信したマッチメイクコマンド
          srcPid  コマンド送信元のプロファイルID
          srcIP   SBメッセージで受信した場合のコマンド送信元QR2用パブリックIP
          srcPort SBメッセージで受信した場合のコマンド送信元QR2用パブリックポート番号
          data    受信データ配列へのポインタ
          len     配列dataの要素数
  戻り値：TRUE:正常終了、FALSE:エラー発生
  用途　：受信したコマンドとデータを参照して処理を行う
 *---------------------------------------------------------------------------*/
static BOOL DWCi_ProcessRecvMatchCommand(u8  command,
                                         int srcPid,
                                         u32 srcIP,
                                         u16 srcPort,
                                         const u32 data[],
                                         int len)
{
#pragma unused(len)
    u8  sendCommand;
    u8  aid;
    u16 port = 0;
    u32 ip = 0;
    u32 aidIdx;
    u32 senddata[DWC_MAX_MATCH_IDX_LIST+1];
    int profileID;
    int sendlen = 0;
    int result;
    int i;
    SBError sbError;
    NegotiateError nnError;
    struct sockaddr_in remoteaddr;

    if (!DWCi_GetMatchCnt() || (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_INIT)){
        // マッチメイクキャンセル直後などに受信した場合は何も返さない
        // [todo]
        // この条件だけだと、キャンセル後すぐに別のマッチメイク条件で
        // やり直した時に受信コマンドを処理してしまう。
        // そうなっても暴走しないように設計しないといけない。
        return TRUE;
    }

    if ((DWC_GetState() == DWC_STATE_MATCHING) &&
        (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_WAITING) &&
        (srcPid == DWCi_GetMatchCnt()->sbPidList[0])){
        // 接続済みクライアントの場合は、サーバからコマンドを受信したら
        // タイムアウトカウンタを初期化する
        DWCi_InitClWaitTimeout();
    }

    switch (command){
    case DWC_MATCH_COMMAND_RESERVATION:  // NN予約要求
    case DWC_MATCH_COMMAND_RESV_PRIOR:   // NN優先予約要求
        if (DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_ANYBODY){
            // 友達指定、サーバクライアントマッチメイクの場合は
            // 追加受信データ内にクライアントのQR2用パブリックIP、ポート番号が
            // 入っている
            srcIP   = data[1];
            srcPort = (u16)data[2];
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "friend IP:%x, port:%d\n",
                       srcIP, srcPort);
        }
        
        // 予約コマンドの送信者情報を元に、送り返すコマンドを取得する
        sendCommand =
            DWCi_CheckResvCommand(srcPid, srcIP, srcPort, data[0],
                                  command == DWC_MATCH_COMMAND_RESV_PRIOR ? TRUE : FALSE);

        if (sendCommand == DWC_MATCH_COMMAND_RESV_OK){
            // 予約承認の場合
#ifdef DWC_LIMIT_FRIENDS_MATCH_VALID
            // 新規接続友達許可コマンド受信ビットマップをクリアする
            DWCi_GetMatchCnt()->friendAcceptBit = 0;
#endif
                
            // 予約承認の際の共通設定と、接続済みのクライアントへの
            // 新規接続ホスト通知処理を行う
            result = DWCi_ProcessResvOK(srcPid, srcIP, srcPort);
            if (DWCi_HandleMatchCommandError(result)) return FALSE;

            if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV){
                // サーバクライアントマッチメイクのサーバの場合は
                // 新規接続クライアント通知コールバックを呼び出す
                if (DWCi_GetMatchCnt()->newClientCallback){
                    DWCi_GetMatchCnt()->newClientCallback(DWCi_GetFriendListIndex(srcPid),
                                                   DWCi_GetMatchCnt()->newClientParam);
                }
            }

            senddata[0] = DWCi_GetMatchCnt()->qr2NNFinishCount;

            // 接続済みクライアント数と各クライアントのプロファイルIDを送信する。
            // （現在は、受信側では友達指定・無指定ピアマッチメイクの場合しか
            // このデータを参照していない）
            for (sendlen = 1; sendlen <= DWCi_GetMatchCnt()->qr2NNFinishCount; sendlen++){
                senddata[sendlen] = (u32)DWCi_GetMatchCnt()->sbPidList[sendlen];
            }

            // 自分の最新のQR2用パブリックIPとポート番号をクライアントに教える
            senddata[sendlen++] = DWCi_GetMatchCnt()->qr2IP;
            senddata[sendlen++] = DWCi_GetMatchCnt()->qr2Port;

            // 状態をNATネゴシエーション中に進める
            DWCi_SetMatchStatus(DWC_MATCH_STATE_SV_OWN_NN);
        }
        // [arakit] main 051024
        else if (sendCommand == DWC_MATCH_COMMAND_RESV_DENY){
            // RESV_DENYの場合は拒否理由を送信する
            // [note]
            // qr2NumEntry > 0 が必要である理由は？
            // これがあると、誰とも接続していない時、予約受信直前に
            // DWC_StopSCMatching()を呼び出すとqr2NumEntry = 0 になるので、
            // クライアントは80410のエラーになる
            if ((DWCi_GetMatchCnt()->qr2NumEntry > 0) &&
                (DWCi_GetMatchCnt()->qr2NNFinishCount == DWCi_GetMatchCnt()->qr2NumEntry)){
                // サーバが定員オーバーの場合
                if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV){
                    // 今のところサーバクライアントマッチメイクの場合しか
                    // 拒否理由は必要ない
                    senddata[0] = DWC_MATCH_RESV_DENY_REASON_SV_FULL;
                    sendlen     = 1;
                }
            }
        }
        // [arakit] main 051024

        if (sendCommand != DWC_MATCH_COMMAND_DUMMY){
            // 予約コマンド送信元ホストにコマンドを送信する
            result =
                DWCi_SendMatchCommand(sendCommand, srcPid, srcIP, srcPort,
                                      senddata, sendlen);
            if (DWCi_HandleMatchCommandError(result)) return FALSE;
        }
        break;

    case DWC_MATCH_COMMAND_RESV_OK:  // NN予約完了
        // 既に予約を承認したのに遅れてこのコマンドが到着した場合は無視する
        if (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_WAIT_RESV) break;
        
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Succeeded NN reservation.\n");

        if (srcPid != DWCi_GetMatchCnt()->reqProfileID){
            // 予約要求を送っていないサーバから予約完了通知が返って来ても無視する
            break;
        }

        DWCi_GetMatchCnt()->priorProfileID = 0;  // 優先予約クリア

        DWCi_GetMatchCnt()->resvWaitCount  = 0;  // WAIT受信による再送カウンタクリア

//        DWCi_GetMatchCnt()->cmdTimeoutCount = 0;
        DWCi_GetMatchCnt()->cmdTimeoutTime = 0;

//        DWCi_GetMatchCnt()->cmdResendCount  = 0;
        DWCi_GetMatchCnt()->cmdResendFlag  = FALSE;

        // サーバの最新のQR2用パブリックIPとポート番号を記録する
        DWCi_GetMatchCnt()->qr2IPList[0]   = data[data[0]+1];
        DWCi_GetMatchCnt()->qr2PortList[0] = (u16)data[data[0]+2];
        DWCi_GetMatchCnt()->searchIP       = data[data[0]+1];
        DWCi_GetMatchCnt()->searchPort     = (u16)data[data[0]+2];
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Server IP:%x, port:%d\n",
                   DWCi_GetMatchCnt()->searchIP, DWCi_GetMatchCnt()->searchPort);

        if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND){
            // 友達指定ピアマッチメイクの場合
            if (DWCi_AreAllBuddies(&data[1], data[0])){
                // サーバ側の接続済みクライアントが全て自分の友達だった場合
                if (DWCi_GetMatchCnt()->gt2NumConnection){
                    // 自分に接続済みクライアントがあったら、そこにも
                    // dataを転送する為にバックアップを取っておく
                    DWCi_MakeBackupServerData(srcPid, data);
                }
                // あとはまず相手ホストの情報を探す
            }
            else {
                // サーバ側に友達でないクライアントが接続されていた場合
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                           "But some clients are not friends.\n");
                
                // 予約キャンセルを行う
                result = DWCi_CancelReservation(srcPid);
                if (DWCi_HandleMatchCommandError(result)) return FALSE;

                // 次の友達にNN予約要求を送信する
                result = DWCi_SendResvCommandToFriend(FALSE, FALSE, srcPid);
                if (DWCi_HandleMatchCommandError(result)) return FALSE;
                break;  // 以下の処理は飛ばす
            }
        }

        if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY){
            // 友達無指定ピアマッチメイクの場合
            if (DWCi_GetMatchCnt()->gt2NumConnection){
                // 自分に接続済みのクライアントがあったら、全てのコネクションを
                // クローズして完全な１クライアントに戻る
                DWCi_MakeBackupServerData(srcPid, data);

                result = DWCi_ChangeToClient();
                if (DWCi_HandleMatchCommandError(result)) return FALSE;
            }

            // NATネゴシエーション状態をセット
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_NN);
               
            // サーバに対してNATネゴシエーションを開始する
            nnError = DWCi_NNStartupAsync(0, 0, ServerBrowserGetServer(DWCi_GetMatchCnt()->sbObj, 0));
            if (DWCi_HandleNNError(nnError)) return FALSE;
        }
        else {
            // 予約ホストの検索状態に進む
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SEARCH_NN_HOST);

            // SBサーバアップデートで相手ホストの情報を探す
            sbError = DWCi_SBUpdateAsync(srcPid);
            if (DWCi_HandleSBError(sbError)) return FALSE;
        }
        break;
            
    case DWC_MATCH_COMMAND_RESV_DENY:  // NN予約拒否
        // サーバからの応答なしでタイムアウトになった時は
        // srcIP, srcPortの値は0になる
        // →この仕様はなくなった
            
        // 既に他のクライアントからの予約を承認したのに、
        // 遅れてこのコマンドが到着した場合は無視する
        if ((DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_WAIT_RESV) ||
            (srcPid != DWCi_GetMatchCnt()->reqProfileID))
            break;

        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Reservation was denied by %u.\n", srcPid);

        // [arakit] main 051024
        if (len > 0){
            // 拒否理由がある場合
            if (data[0] == DWC_MATCH_RESV_DENY_REASON_SV_FULL){
                // サーバクライアントマッチメイクのサーバが定員オーバー
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                           "Game server is fully occupied.\n");

                DWCi_StopMatching(DWC_ERROR_SERVER_FULL, 0);
                return FALSE;
            }
        }
        // [arakit] main 051024

        // 次の予約先を探す
        return DWCi_RetryReserving(DWCi_GetMatchCnt()->reqProfileID);
        break;

    case DWC_MATCH_COMMAND_RESV_WAIT:  // NN予約待ち
        // 既に予約を承認したのに遅れてこのコマンドが到着した場合は無視する
        if (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_WAIT_RESV) break;
        
        // サーバかそれに繋がるクライアント同士が
        // コネクション確立中なので、それが終わるのを待つ
        if (srcPid == DWCi_GetMatchCnt()->reqProfileID){
            // タイムアウト計測開始時間再セット
            DWCi_GetMatchCnt()->cmdTimeoutStartTick = OS_GetTick();
            
            if ((DWCi_GetMatchCnt()->priorProfileID &&
                 (DWCi_GetMatchCnt()->resvWaitCount < DWC_RESV_COMMAND_RETRY_MAX)) ||
                (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL)){
                // 優先予約先が決まっている場合は一定時間後に
                // 予約コマンドを再送する。
                // サーバクライアントマッチメイクでも、他のサーバに繋ぎに行く
                // 必要がないので再送する。
//                DWCi_GetMatchCnt()->cmdResendCount = 1;
                DWCi_GetMatchCnt()->cmdResendFlag = TRUE;
                DWCi_GetMatchCnt()->cmdResendTick = OS_GetTick();
                
                if (DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_SC_CL){
                    DWCi_GetMatchCnt()->resvWaitCount++;
                }
                // サーバクライアントマッチメイクの場合、指定のサーバ以外には
                // 予約コマンドを送らないので、WAITが返ってくる間は
                // 再送を続けて構わない
                break;  // 以後の処理は飛ばす
            }

            DWCi_GetMatchCnt()->priorProfileID = 0;  // 優先予約はクリアする
            DWCi_GetMatchCnt()->resvWaitCount  = 0;  // WAIT受信予約送信カウンタクリア

            // 優先予約先が決まっている場合でなければ、すぐに諦めて
            // 次の予約先を探す　
            if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY){
                // 空きホストの検索状態に戻る
                DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SEARCH_HOST);
//                DWCi_GetMatchCnt()->sbUpdateCount = 1;  // 一定時間後に検索
                DWCi_GetMatchCnt()->sbUpdateFlag = DWC_SB_UPDATE_INTERVAL_SHORT;  // 一定時間後に検索
                DWCi_GetMatchCnt()->sbUpdateTick = OS_GetTick();
            }
            else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND){
                // 次の友達にNN予約要求を送信する
                (void)DWCi_SendResvCommandToFriend(TRUE, FALSE, 0);
            }
            // サーバクライアントマッチメイクの場合はここまで来ない
        }
        // 予約送信した相手以外からのコマンド、もしくは自分が既に予約承認、
        // 予約キャンセルした場合の予約待ちコマンドは無視する
        break;

    case DWC_MATCH_COMMAND_RESV_CANCEL:  // NN予約解除要求
        if (DWCi_GetMatchCnt()->qr2IsReserved &&
            (srcPid == DWCi_GetMatchCnt()->qr2Reservation)){ 
            // 予約中の相手からの予約解除要求なら応じる
            if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV) &&
                (DWCi_GetMatchCnt()->gt2NumConnection == 1) &&
                (DWCi_GetMatchCnt()->sbPidList[1] == srcPid)){
                // サーバクライアントマッチメイクで、
                // 新規接続クライアントが直接キャンセルしてきた場合で、
                // gt2Connectが成立した直後であれば、コネクションを
                // クローズする
                gt2CloseAllConnectionsHard(*DWCi_GetMatchCnt()->pGt2Socket);
            }
                
            // 新規接続の受け付け処理を終了してマッチメイクを再開する
            if (!DWCi_CancelPreConnectedServerProcess(srcPid)) return FALSE;
        }
        break;

    case DWC_MATCH_COMMAND_TELL_ADDR:  // NN開始側アドレス通知
        // 自ホストがNN開始側と同一のNAT内にあり、NATネゴシエーションが
        // 必要ない場合に、NATネゴシエーション終了状態にするためのコマンド
        ip   = data[0];
        port = (u16)data[1];

        DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                   "NN parent is behind same NAT as me. Received IP %x & port %d\n",
                   ip, port);

        if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_WAITING){
            // 接続済みクライアントの場合は便宜上、状態を変更する
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_NN);
        }
        else if (((DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_NN) &&
                  (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_SV_OWN_NN)) ||
                 (srcPid != DWCi_GetMatchCnt()->qr2Reservation)){
            // なかなかクライアントが予約に来ないので予約をキャンセルした後に
            // NN要求が来た場合は無視する
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                       "But already canceled reservation.\n");
            break;
        }

        // 予約保持タイムアウト計測終了
        DWCi_GetMatchCnt()->cmdCnt.command = DWC_MATCH_COMMAND_DUMMY;

        // もし新規接続クライアント通知コマンドを受信できなくて、
        // まだ相手のプロファイルIDを知らなかった場合は、ここでセットする。
        // NATネゴシエーションを実際に行った場合は、このようなプロファイルIDの
        // リカバーを行う機会が全くない。
        if (DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->gt2NumConnection+1] != srcPid){
            DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->gt2NumConnection+1] = srcPid;
        }

        // NATネゴシエーション完了コールバックに渡すアドレスデータを作成
        remoteaddr.sin_addr.s_addr = ip;
        // ポート番号はNNコールバック関数内で変換されるので、
        // ここでは逆変換しておく
        remoteaddr.sin_port = SO_HtoNs(port);

        // NATネゴシエーション完了コールバックを呼び出す
        DWCi_GetMatchCnt()->nnInfo.isQR2 = 1;
        DWCi_NNCompletedCallback(nr_success,
                                 gt2GetSocketSOCKET(*DWCi_GetMatchCnt()->pGt2Socket),
                                 &remoteaddr, &DWCi_GetMatchCnt()->nnInfo);

        // 実際にNATネゴシエーションを行わなかった場合は、NN終了時の時刻を
        // クリアする
        DWCi_GetMatchCnt()->nnFinishTime = 0;
        break;

    case DWC_MATCH_COMMAND_NEW_PID_AID:  // 新規接続クライアント情報
        // [arakit] main 051010
        // 適正な状態で正しいサーバから受信した場合以外は何もしない
        if ((DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_WAITING) ||
            (srcPid != DWCi_GetMatchCnt()->sbPidList[0])){
            DWC_Printf(DWC_REPORTFLAG_DEBUG,
                       "Ignore delayed NEW_PID_AID command.\n");
            break;
        }
        // [arakit] main 051010
            
        profileID = (int)data[0];
        aid       = (u8)data[1];

#ifdef DWC_LIMIT_FRIENDS_MATCH_VALID
        if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND) &&
            !DWCi_GetMatchCnt()->distantFriend){
            // 友達指定ピアマッチメイクで友達の友達との接続を許さない場合は、
            // 新規接続クライアントが自分の友達インデックスリストで指定された
            // 友達かどうか調べて、結果をサーバに送信する
            senddata[0] = (u32)DWCi_IsFriendByIdxList(profileID);

            DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                       "profileID %d is acceptable? - %d.\n",
                       profileID, senddata[0]);
            
            result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_FRIEND_ACCEPT,
                                           srcPid, srcIP, srcPort,
                                           senddata, 1);
            if (DWCi_HandleMatchCommandError(result)) return FALSE;
        }
#endif

        // プロファイルIDとAIDをリストにセットする
        DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->qr2NNFinishCount+1] = profileID;
        DWCi_GetMatchCnt()->aidList[DWCi_GetMatchCnt()->qr2NNFinishCount+1]   = aid;

        // 近いうちに新規接続クライアントが自分のQR2データを検索するので、
        // アップデートを行う
        qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);

        if (DWCi_GetMatchCnt()->newClientCallback){
            // 新規接続クライアント通知コールバックを呼び出す
            DWCi_GetMatchCnt()->newClientCallback(DWCi_GetFriendListIndex(profileID),
                                           DWCi_GetMatchCnt()->newClientParam);
        }

        DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                   "Received new client's profileID = %u & aid = %d.\n",
                   profileID, aid);
        break;

    case DWC_MATCH_COMMAND_LINK_CLS_REQ:  // クライアント同士の接続要求
        // [arakit] main 051010
        // 適正な状態で正しいサーバから受信した場合以外は何もしない
        if ((DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_WAITING) ||
            (srcPid != DWCi_GetMatchCnt()->sbPidList[0])){
            DWC_Printf(DWC_REPORTFLAG_DEBUG,
                       "Ignore delayed LINK_CLS_REQ command.\n");
            break;
        }
        // [arakit] main 051010
        
        profileID = (int)data[0];
        if (!profileID){
            // 全てのクライアントとのコネクションが確立された
            aidIdx = data[1];
            aid    = (u8)data[2];

            // 自分のAIDを登録する
            DWCi_GetMatchCnt()->aidList[aidIdx] = aid;
            // 自分のプロファイルIDも登録しておく
            DWCi_GetMatchCnt()->sbPidList[aidIdx] = DWCi_GetMatchCnt()->profileID;
            
            DWCi_PostProcessConnection(DWC_PP_CONNECTION_CL_FINISH_CONNECT);
        }
        else {
            // 指定のプロファイルIDに対してNATネゴシエーションを要求する。
            // この場合は予約なしで良い。
            // 次に接続を行うクライアントのプロファイルIDとAIDを格納する
            aidIdx = data[1];
            aid    = (u8)data[2];

            if ((DWCi_GetMatchCnt()->sbPidList[aidIdx] == profileID) &&
                (aidIdx == DWCi_GetMatchCnt()->gt2NumConnection-1)){
                // 既に接続完了して接続成功のコマンドを送信したのに、
                // 入れ違いでまた接続要求コマンドを受信した場合、
                // 接続成功コマンドを再送する
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                           "Resend command %d for delayed command %d.\n",
                           DWC_MATCH_COMMAND_LINK_CLS_SUC,
                           DWC_MATCH_COMMAND_LINK_CLS_REQ);
                
                result =
                    DWCi_SendMatchCommand(DWC_MATCH_COMMAND_LINK_CLS_SUC,
                                          srcPid,
                                          DWCi_GetMatchCnt()->qr2IPList[0],
                                          DWCi_GetMatchCnt()->qr2PortList[0],
                                          (u32 *)&profileID, 1);
                if (DWCi_HandleMatchCommandError(result)) return FALSE;
                break;
            }
            
            DWCi_GetMatchCnt()->sbPidList[aidIdx] = profileID;
            DWCi_GetMatchCnt()->aidList[aidIdx]   = aid;

            // QR2用パブリックIPとポート番号を受信しているので、
            // 次に接続するクライアントのQR2データを正確に検索するため記録
            DWCi_GetMatchCnt()->qr2IPList[aidIdx]   = data[3];
            DWCi_GetMatchCnt()->qr2PortList[aidIdx] = (u16)data[4];
            DWCi_GetMatchCnt()->searchIP            = data[3];
            DWCi_GetMatchCnt()->searchPort          = (u16)data[4];
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "Client IP:%x, port:%d\n",
                       DWCi_GetMatchCnt()->searchIP, DWCi_GetMatchCnt()->searchPort);
                    
            // サーバ情報取得のための状態に移行
            // [note]
            // 友達無指定ピアマッチメイクの場合、プライベートアドレスとポートの
            // リストも持つようにすれば、ここでSBUpdateでサーバを検索しなくても
            // いきなりNATネゴシエーションを始められるので、一件につき
            // 0.5秒程度は短縮できる余地が残っていると思われる
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SEARCH_NN_HOST);

            DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                       "Next, try to connect to %u.\n", profileID);

            // NATネゴシエーションする相手をSBサーバアップデートで見つける
            sbError = DWCi_SBUpdateAsync(profileID);
            if (DWCi_HandleSBError(sbError)) return FALSE;

//            DWCi_GetMatchCnt()->cmdTimeoutCount = 0;
            DWCi_GetMatchCnt()->cmdTimeoutTime = 0;
//            DWCi_GetMatchCnt()->cmdResendCount  = 0;
            DWCi_GetMatchCnt()->cmdResendFlag = FALSE;
        }
        break;

    case DWC_MATCH_COMMAND_LINK_CLS_SUC:  // クライアント同士の接続成功
        if ((DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_WAIT_CL_LINK) &&
            (DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->clLinkProgress+1] == data[0])){
            // data[0]はクライアントが接続完了した相手のプロファイルID。
            // 既に接続要求を出していた相手との接続を完了した場合のみ
            // 次の接続要求を送信する。
            DWCi_GetMatchCnt()->clLinkProgress++;

            // 次のコネクション確立を開始する
            DWCi_PostProcessConnection(DWC_PP_CONNECTION_SV_CONNECT);
        }
        else {
            // 既に確立したコネクションの成功コマンドが入れ違いで届いた場合は
            // 何もしない
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Ignore delayed command %d.\n",
                       DWC_MATCH_COMMAND_LINK_CLS_SUC);
        }
        break;

    case DWC_MATCH_COMMAND_CLOSE_LINK:  // コネクションクローズ命令
        // [arakit] main 051010
        // 適正な状態で受信した場合以外は何もしない
        if ((DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_WAITING) &&
            (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_WAIT_CLOSE)){
            DWC_Printf(DWC_REPORTFLAG_DEBUG,
                       "Ignore delayed CLOSE_LINK command.\n");
            break;
        }
        // [arakit] main 051010
            
        if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY) ||
            DWCi_AreAllBuddies(&data[1], data[0])){
            // クローズ命令送信元の新規接続先サーバの接続済みクライアントが、
            // 全て自分の友達だった場合、自分も次にそこに接続を試みる。
            // 友達無指定ピアマッチメイクの場合は友達かどうかは関係ない。
            DWCi_GetMatchCnt()->priorProfileID = (int)data[1];  // 次回接続先記録

            // WAIT受信による再送の為の準備
            DWCi_GetMatchCnt()->resvWaitCount = 0;

            DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                       "Received close command. Next try to %u.\n",
                       data[1]);
        }
        else {
            // クローズ命令送信元の新規接続先サーバの接続済みクライアントに、
            // 自分の友達でないクライアントがいた場合は次回はまた別の接続先を探す
            DWCi_GetMatchCnt()->priorProfileID = 0;

            DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                       "Received close command. Server %u or its clients are not friends.\n",
                       data[1]);
        }
        
        if (DWCi_GetMatchCnt()->gt2NumConnection){
            // まだコネクションのクローズが完了していないなら自分で
            // クローズする。
            // 続きの処理はコールバック内で行われる。
            gt2CloseAllConnectionsHard(*DWCi_GetMatchCnt()->pGt2Socket);
        }
        else {
            // 既にコネクションのクローズが終わっていた場合は、
            // すぐに優先予約先ホストへの接続を試みる
            if (DWCi_ResumeMatching()) return FALSE;
        }
        break;

    case DWC_MATCH_COMMAND_CANCEL:  // マッチメイクキャンセル
        // このコマンドを受信するのは、サーバへの予約をしたところから
        // gt2Connection確立直前までのクライアントのみ

        // [arakit] main 051010
        // 適正な状態で正しいサーバから受信した場合以外は何もしない
        if (srcPid != DWCi_GetMatchCnt()->sbPidList[0]){
            DWC_Printf(DWC_REPORTFLAG_DEBUG,
                       "Ignore delayed CANCEL command.\n");
            break;
        }
        // [arakit] main 051010
            
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                   "Received cancel command from %u data[0] = %d.\n",
                   srcPid, data[0]);
        DWC_Printf(DWC_REPORTFLAG_DEBUG,
                   "numHost nn=%d gt2=%d, state %d\n",
                   DWCi_GetMatchCnt()->qr2NNFinishCount,
                   DWCi_GetMatchCnt()->gt2NumConnection, DWCi_GetMatchCnt()->state);

        if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY) ||
            (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND)){
            // 全てのクライアント処理を終了してマッチメイクを再開する。
            if (!DWCi_CancelPreConnectedClientProcess(srcPid)) return FALSE;
        }
        else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL){
            // 新規接続クライアントで、サーバがキャンセルした場合、
            // マッチメイクを終了する
            // キャンセルホストのプロファイルIDを記録
            DWCi_GetMatchCnt()->cbEventPid = srcPid;

            // コネクションが残っていたら全てクローズする
            DWCi_CloseAllConnectionsByTimeout();
            DWCi_RestartFromCancel(DWC_MATCH_RESET_ALL);
        }
        break;

    case DWC_MATCH_COMMAND_CANCEL_SYN:
    case DWC_MATCH_COMMAND_CANCEL_SYN_ACK:
    case DWC_MATCH_COMMAND_CANCEL_ACK:
        // サーバクライアントマッチメイクキャンセル同期調整コマンドを処理する
        if (!DWCi_ProcessCancelMatchSynCommand(srcPid, command, data[0]))
            return FALSE;
        break;

    case DWC_MATCH_COMMAND_SC_CLOSE_CL:   // 接続済みクライアントの切断通知
        // 万が一サーバ以外からこのコマンドを受信しても何もしない
        if (srcPid != DWCi_GetMatchCnt()->sbPidList[0]) return TRUE;

        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Close shutdown client.\n");

        // 切断と判定されたクライアントをクローズする
        for (i = 0; i < len; i++){
            aid = DWCi_GetAIDFromProfileID((int)data[i], FALSE);
            if (aid != 0xff){
                DWC_CloseConnectionHard(aid);
            }
        }
        break;

    case DWC_MATCH_COMMAND_POLL_TIMEOUT:  // OPTION_MIN_COMPLETEのタイムアウトポーリング
        if (stpOptMinComp && stpOptMinComp->valid &&
            (OS_TicksToMilliSeconds(OS_GetTick()-stpOptMinComp->startTime) >= stpOptMinComp->timeout)){
            // 自分もOPTION_MIN_COMPLETEのオプションを設定していて、
            // タイムアウト時間を越えていたら1を返す
            senddata[0] = 1;

            DWC_Printf(DWC_REPORTFLAG_MATCH_GT2,
                       "[OPT_MIN_COMP] time is %lu.\n",
                       OS_TicksToMilliSeconds(OS_GetTick()-stpOptMinComp->startTime));
        }
        else {
            senddata[0] = 0;
        }

        result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_POLL_TO_ACK,
                                       srcPid, srcIP, srcPort,
                                       senddata, 1);
        if (DWCi_HandleMatchCommandError(result)) return FALSE;
        break;

    case DWC_MATCH_COMMAND_POLL_TO_ACK:  // OPTION_MIN_COMPLETEのタイムアウトポーリングACK
        if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_POLL_TIMEOUT){
            // 送信元クライアントからの受信ビットを立てる（AIDビットマップ）
            aid = DWCi_GetAIDFromProfileID(srcPid, FALSE);
            if (aid != 0xff){
                stpOptMinComp->recvBit |= 1 << aid;
            
                if (data[0]){
                    // クライアントもタイムアウトならビットを立てる
                    stpOptMinComp->timeoutBit |= 1 << aid;
                }
            }
        }
        // 違う状態の時に遅れてきたACKを受信しても何もしない
        break;
 
    case DWC_MATCH_COMMAND_SC_CONN_BLOCK:  // OPTION_SC_CONNECT_BLOCKのブロックによる予約拒否
        // マッチメイクをエラー終了する
        // [arakit] main 051011
        DWCi_StopMatching(DWC_ERROR_MO_SC_CONNECT_BLOCK, 0);
        return FALSE;

#ifdef DWC_LIMIT_FRIENDS_MATCH_VALID
    case DWC_MATCH_COMMAND_FRIEND_ACCEPT:  // クライアントからの新規接続友達許可
        if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND) &&
            !DWCi_GetMatchCnt()->distantFriend){
            // 友達指定ピアマッチメイクで友達の友達との接続不許可の場合のみ
            // 以下の処理に進む
            // [todo]
            // プロファイルIDから自分の接続済みクライアントかどうか調べる関数を作る
            for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
                if (DWCi_GetMatchCnt()->sbPidList[i] == srcPid){
                    DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                               "New client was accepted? - %d.\n", data[0]);
                    if (!data[0]){
                        // 接続済みクライアントにとって、新規接続クライアントが
                        // 友達インデックスリスト記載の友達でなかった場合は
                        // マッチメイクをやり直す
                        if ((DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_OWN_NN) ||
                            (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_OWN_GT2)){

                            // 新規接続クライアントとの接続中なら、
                            // マッチメイクキャンセルコマンドを送信
                            result = DWCi_InvalidateReservation();
                            if (DWCi_HandleMatchCommandError(result)) return FALSE;
                        }

                        if (DWCi_GetMatchCnt()->nnInfo.cookie){
                            // NATネゴシエーション中ならキャンセル
                            NNCancel(DWCi_GetMatchCnt()->nnInfo.cookie);
                            DWCi_GetMatchCnt()->nnInfo.cookie = 0;
                        }

                        // マッチメイクやり直し
                        DWCi_RestartFromTimeout();
                    }
                    else {
                        // 新規接続友達許可を受信したことを記録する
                        DWCi_GetMatchCnt()->friendAcceptBit |=
                            1 << DWCi_GetAIDFromProfileID(srcPid, FALSE);
                    }
                    break;
                }
            }
        }
        break;
#endif

    case DWC_MATCH_COMMAND_CL_WAIT_POLL:  // クライアントからのキープアライブ要求
        for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
            // 自分に接続しているクライアントから受信したのであれば、
            // キープアライブのコマンドを返信する
            if (DWCi_GetMatchCnt()->sbPidList[i] == srcPid){
                result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_SV_KA_TO_CL,
                                               srcPid, srcIP, srcPort,
                                               NULL, 0);
                if (DWCi_HandleMatchCommandError(result)) return FALSE;
                else break;
            }
        }
        break;

    case DWC_MATCH_COMMAND_SV_KA_TO_CL:  // サーバからのキープアライブ
        // 本関数の最初でタイムアウトカウンタの初期化は行っているので、
        // ここでは特に何もする必要はない
        break;

    default:
        // [test]
        // 開発中のみ表示
        DWC_Printf(DWC_REPORTFLAG_ERROR,
                   "Received unexpected matching command 0x%02x.\n",
                   command);
        break;
    }

    return TRUE;
}


/*---------------------------------------------------------------------------*
  マッチメイク予約コマンドチェック関数
  引数　：profileID コマンド送信元ホストのプロファイルID
          qr2IP     コマンド送信元ホストのQR2用パブリックIP
          qr2Port   コマンド送信元ホストのQR2用パブリックポート番号
          matchType コマンド送信元ホストのマッチメイク型
          priorFlag 優先予約フラグ
  戻り値：次に送り返すべきコマンド。DWC_MATCH_COMMAND_RESV_* 列挙子。
  用途　：NATネゴシエーション予約コマンドを受信した時に、次に送り返すべき
          コマンドを判定する
 *---------------------------------------------------------------------------*/
static u8  DWCi_CheckResvCommand(int profileID, u32 qr2IP, u16 qr2Port, u32 matchType, BOOL priorFlag)
{
    u8  sendCommand;
    int result;

    switch (DWCi_GetMatchCnt()->qr2MatchType){
    case DWC_MATCH_TYPE_FRIEND:   // 友達指定ピアマッチメイク
        if (!gpIsBuddy(DWCi_GetMatchCnt()->pGpObj, profileID)){
            // 片方向のみの友達からの予約は無視する。
            // 相手のQR2が上がっているかどうかも分からないので、
            // SBメッセージで返すこともしない。
            sendCommand = DWC_MATCH_COMMAND_DUMMY;  // ダミーコマンド
            break;
        }
#ifdef DWC_LIMIT_FRIENDS_MATCH_VALID
        else if (!DWCi_IsFriendByIdxList(profileID)){
            // 友達インデックスリストにない友達からの予約は拒否する
            sendCommand = DWC_MATCH_COMMAND_RESV_DENY;  // 予約拒否コマンド
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                       "This friend doesn't exist in friendIdxList.\n");
            break;
        }
#endif
        // 引き続き友達無指定ピアマッチメイクと共通の処理に進む
        
    case DWC_MATCH_TYPE_ANYBODY:  // 友達無指定ピアマッチメイク
        if ((matchType != DWCi_GetMatchCnt()->qr2MatchType)                    ||
            (DWCi_GetMatchCnt()->cancelState != DWC_MATCH_CANCEL_STATE_INIT)   ||
            (DWCi_GetMatchCnt()->qr2NNFinishCount == DWCi_GetMatchCnt()->qr2NumEntry) ||
            (DWCi_GetMatchCnt()->qr2IsReserved &&
             (DWCi_GetMatchCnt()->qr2Reservation == DWCi_GetMatchCnt()->profileID))){
            // 自分が既に接続されているクライアントの場合や、
            // 既に接続数がいっぱいの場合は予約を拒否する。
            sendCommand = DWC_MATCH_COMMAND_RESV_DENY;  // 予約拒否コマンド

            if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY) &&
                !DWCi_GetMatchCnt()->qr2Obj->userstatechangerequested &&
                DWCi_GetMatchCnt()->qr2IsReserved &&
                (DWCi_GetMatchCnt()->qr2Reservation == DWCi_GetMatchCnt()->profileID)){
                // 友達無指定ピアマッチメイクで既に接続済みクライアントなのに
                // 予約コマンドを受信した場合は、早くサーバに接続済み情報が
                // 反映されるようにする
                qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
            }
        }
        else if (((DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_SEARCH_HOST) &&
                  (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_WAIT_RESV))     ||
                 ((DWCi_GetMatchCnt()->qr2IP == 0) && (DWCi_GetMatchCnt()->qr2Port == 0)) ||
                 ((qr2IP == 0) && (qr2Port == 0))){
            // 自分のマッチメイク処理が進行中ならそれが終わるまで待ってもらう。
            // まだ自分か相手のQR2用パブリックIPとポートが決まっていない場合も同様
            sendCommand = DWC_MATCH_COMMAND_RESV_WAIT;  // 予約待機コマンド
        }
        else {
            if (DWCi_GetMatchCnt()->reqProfileID){
                // 誰かにNN要求を送って待っている状態
                if (DWCi_GetMatchCnt()->reqProfileID == profileID){
                    // 自分がNN要求を送った相手からNN要求が来た場合
                    if (priorFlag ||
                        ((DWCi_GetMatchCnt()->profileID < profileID) &&
                        (profileID != DWCi_GetMatchCnt()->priorProfileID))){
                        // 自分のプロファイルIDが相手より小さい、もしくは
                        // 優先予約フラグがONの場合は予約承認。
                        // ただし、上記を満たしていても、自分が優先予約要求を
                        // 送った相手からの要求は承認しない
                        sendCommand = DWC_MATCH_COMMAND_RESV_OK;  // 予約承認コマンド
                    }
                    else {
                        // 相手からNN要求への返答があるはずなので
                        // コマンドを送信しない為にダミーコマンドを設定する
                        sendCommand = DWC_MATCH_COMMAND_DUMMY;  // ダミーコマンド
                    }
                }
                else {
                    // 自分がNN要求を送っていない相手からNN要求が来た場合
                    if (priorFlag ||
                        ((DWCi_GetMatchCnt()->profileID < profileID) &&
                        !DWCi_GetMatchCnt()->priorProfileID)){
                        // 自分のプロファイルIDが相手より小さく、優先予約要求を
                        // 送信済みでないか、もしくは優先予約フラグがONの
                        // 場合は、自分のNN要求は諦め受信したNN要求を承認する。
                        // 予約キャンセルを行う。
                        result = DWCi_CancelReservation(DWCi_GetMatchCnt()->reqProfileID);
                        if (DWCi_HandleMatchCommandError(result)){
                            return DWC_MATCH_COMMAND_DUMMY;  // ダミーコマンド
                        }
                        else {
                            sendCommand = DWC_MATCH_COMMAND_RESV_OK;  // 予約承認コマンド
                        }
                    }
                    else {
                        sendCommand = DWC_MATCH_COMMAND_RESV_DENY;  // 予約拒否コマンド
                    }
                }
            }
            else {
                sendCommand = DWC_MATCH_COMMAND_RESV_OK;    // 予約承認コマンド
            }
        }
        break;

    case DWC_MATCH_TYPE_SC_SV:  // サーバクライアントマッチメイクサーバ側
        if (!gpIsBuddy(DWCi_GetMatchCnt()->pGpObj, profileID)){
            // 片方向のみの友達からの予約は無視する。
            // 相手のQR2が上がっているかどうかも分からないので、
            // SBメッセージで返すこともしない。
            sendCommand = DWC_MATCH_COMMAND_DUMMY;  // ダミーコマンド
        }
        else if ((matchType != DWC_MATCH_TYPE_SC_CL) ||
                 (DWCi_GetMatchCnt()->qr2NNFinishCount == DWCi_GetMatchCnt()->qr2NumEntry)){
            // 既にいっぱいの場合は予約を拒否する。
            // マッチメイクをキャンセルした直後にNN要求を受信してしまった
            // 場合も拒否する。
            sendCommand = DWC_MATCH_COMMAND_RESV_DENY;  // 予約拒否コマンド
        }
        else if ((stOptSCBlock.valid == 1) && (stOptSCBlock.lock == 1)){
            // OPTION_SC_CONNECT_BLOCKのオプションが設定されていて、
            // 新規接続がブロックされている場合は、特別な拒否コマンドを送る
            sendCommand = DWC_MATCH_COMMAND_SC_CONN_BLOCK;
        }
        else if ((DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_SV_WAITING)         ||
                 ((DWCi_GetMatchCnt()->qr2IP == 0) && (DWCi_GetMatchCnt()->qr2Port == 0)) ||
                 ((qr2IP == 0) && (qr2Port == 0))){
            // 他の人とのコネクション確立処理が進行中なら、
            // それが終わるまで待ってもらう。
            // まだ自分か相手のQR2用パブリックIPとポートが決まっていない場合も同様。
            sendCommand = DWC_MATCH_COMMAND_RESV_WAIT;  // 予約待機コマンド
        }
        else {
            sendCommand = DWC_MATCH_COMMAND_RESV_OK;    // 予約承認コマンド
        }
        break;
    }

    return sendCommand;
}


/*---------------------------------------------------------------------------*
  マッチメイク予約OK返信前処理関数
  引数　：profileID 予約コマンド送信元ホストのプロファイルID
          ip        友達無指定ピアマッチメイクの場合の相手のQR2用パブリックIP
          port      友達無指定ピアマッチメイクの場合の相手のQR2用パブリックポート番号
  戻り値：SBError型もしくはGPResult型をint型に変換した値
  用途　：NATネゴシエーション予約にOKを返す前に、いくつかの設定と
          接続済みのクライアントへの新規接続クライアントの通知を行う
 *---------------------------------------------------------------------------*/
static int DWCi_ProcessResvOK(int profileID, u32 ip, u16 port)
{
    u32  senddata[2];
    int  result;
    int  i;

    if (DWCi_GetMatchCnt()->qr2IsReserved && (DWCi_GetMatchCnt()->qr2Reservation == profileID)){

        // 既に予約できているのにここに来た場合は何もしない
        return 0;
    }

    // クライアントによる予約を成立させる
    DWCi_GetMatchCnt()->qr2IsReserved   = 1;
    DWCi_GetMatchCnt()->qr2Reservation  = profileID;
//    DWCi_GetMatchCnt()->cmdResendCount  = 0;  // コマンド再送カウンタクリア
    DWCi_GetMatchCnt()->cmdResendFlag   = FALSE;  // コマンド再送カウンタクリア
//    DWCi_GetMatchCnt()->cmdTimeoutCount = 0;  // コマンドレスポンスタイムアウトカウントクリア
    DWCi_GetMatchCnt()->cmdTimeoutTime = 0;  // コマンドレスポンスタイムアウトカウントクリア

    // 状態の変化をマスターサーバに通知する
    qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);

    // 友達指定ピアマッチメイクの場合のために、NN要求送信先プロファイルIDをクリア
    DWCi_GetMatchCnt()->reqProfileID = 0;

    // 相手のプロファイルIDをリストに格納する
    DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->qr2NNFinishCount+1] = profileID;

    // 相手のQR2用パブリックIPとポート番号をセット
    DWCi_GetMatchCnt()->qr2IPList[DWCi_GetMatchCnt()->qr2NNFinishCount+1]    = ip;
    DWCi_GetMatchCnt()->qr2PortList[DWCi_GetMatchCnt()->qr2NNFinishCount+1] = port;
    DWCi_GetMatchCnt()->searchIP   = ip;
    DWCi_GetMatchCnt()->searchPort = port;

    // AIDリストにAIDを登録
    DWCi_GetMatchCnt()->aidList[DWCi_GetMatchCnt()->qr2NNFinishCount+1] = DWCi_GetAIDFromList();

    // 接続済みのクライアントに新規接続クライアントのプロファイルIDと
    // AIDを送信する
    senddata[0] = (u32)profileID;
    senddata[1] = DWCi_GetMatchCnt()->aidList[DWCi_GetMatchCnt()->qr2NNFinishCount+1];

    for (i = 1; i <= DWCi_GetMatchCnt()->qr2NNFinishCount; i++){
        result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_NEW_PID_AID,
                                       DWCi_GetMatchCnt()->sbPidList[i],
                                       DWCi_GetMatchCnt()->qr2IPList[i],
                                       DWCi_GetMatchCnt()->qr2PortList[i],
                                       senddata, 2);
        if (result) return result;
    }

    // サーバになることが決定した場合、指定人数以下マッチメイク完了オプションが
    // 設定されていれば、そのパラメータを初期化する
    DWCi_InitOptMinCompParam(TRUE);

    return 0;
}


/*---------------------------------------------------------------------------*
  サーバの接続済みホストリスト退避関数
  引数　：profileID サーバのプロファイルID
          data      サーバから受信した接続済みホストリスト
  戻り値：なし
  用途　：友達指定ピアマッチメイクで、サーバから予約OKのコマンドを受信した時に、
          サーバの接続済みホストデータのバックアップを取っておく
 *---------------------------------------------------------------------------*/
static void DWCi_MakeBackupServerData(int profileID, const u32 data[])
{
    u32 len;

    // svDataBak[1]はサーバのプロファイルIDとし、その後ろにpidListを
    // コピーする。svDataBak[0]はプロファイルIDリスト長
    len = data[0]+2;
    if (len > 2){
        MI_CpuCopy32(&data[1], &DWCi_GetMatchCnt()->svDataBak[2], sizeof(u32)*(len-2));
    }

    DWCi_GetMatchCnt()->svDataBak[0] = len-1;
    DWCi_GetMatchCnt()->svDataBak[1] = (u32)profileID;
}


/*---------------------------------------------------------------------------*
  マッチメイクコマンド処理時エラー処理関数
  引数　：int SBError型もしくはGPResult型
  戻り値：引数に受け取ったものをint型にして返す
  用途　：マッチメイクコマンドはGPバディメッセージとSBメッセージのどちらかを
          使うので、両方のエラー型をハンドリングして１つのエラーとして返す
 *---------------------------------------------------------------------------*/
static int DWCi_HandleMatchCommandError(int error)
{

    // [todo]
    // 友達無指定ピアマッチメイクでなくてもSBメッセージでコマンドを送ることが
    // あるのでこの場合分けだと正確ではない
    if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY){
        return (int)DWCi_HandleSBError((SBError)error);
    }
    else {
        return (int)DWCi_HandleGPError((GPResult)error);
    }
}


/*---------------------------------------------------------------------------*
  NN予約コマンド送信関数
  引数　：profileID 予約したいサーバのプロファイルID。
                    友達指定、サーバクライアントマッチメイクで使用。
          delay     TRUE :一定フレーム後にコマンド送信する、
                    FALSE:即コマンド送信する
  戻り値：SBError型もしくはGPResult型をint型に変換した値
  用途　：指定のサーバの情報をセットし、NN予約コマンドを送信する
 *---------------------------------------------------------------------------*/
static int DWCi_SendResvCommand(int profileID, BOOL delay)
{
    u8  command;
    u32 senddata[3];
    int sendlen;
    int result;
    SBServer server;

    if (delay || ((DWCi_GetMatchCnt()->qr2IP == 0) && (DWCi_GetMatchCnt()->qr2Port == 0))){
        // 実際の送信を遅らせたい場合は再送カウンタをセットし、
        // プロファイルIDリスト[0]に次の送信先プロファイルIDをセットしておけば
        // DWCi_MatchProcess()内で再送という形でNN予約要求が送信される。
        // QR2用パブリックIP、ポート番号が確定していない場合は、
        // サーバに通知できないので、送信を遅らせる。
//        DWCi_GetMatchCnt()->cmdResendCount = 1;
        DWCi_GetMatchCnt()->cmdResendFlag = TRUE;
        DWCi_GetMatchCnt()->cmdResendTick = OS_GetTick();
        DWCi_GetMatchCnt()->sbPidList[0] = profileID;

        if (!delay){
            DWC_Printf(DWC_REPORTFLAG_DEBUG,
                       "Delay ResvCommand - qr2IP & qr2Port = 0.\n");
        }
        return 0;
    }

    if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY){
        // 友達無指定ピアマッチメイクの場合は、サーバ候補のプロファイルID、
        // QR2用パブリックIP、ポート番号をリストにセット
        server = ServerBrowserGetServer(DWCi_GetMatchCnt()->sbObj, 0); 
        DWCi_GetMatchCnt()->sbPidList[0]   =
            SBServerGetIntValue(server, DWC_QR2_PID_KEY_STR, 0);
        DWCi_GetMatchCnt()->qr2IPList[0]   = SBServerGetPublicInetAddress(server);
        DWCi_GetMatchCnt()->qr2PortList[0] = SBServerGetPublicQueryPort(server);

        // NN要求送信先プロファイルIDを記録
        DWCi_GetMatchCnt()->reqProfileID = DWCi_GetMatchCnt()->sbPidList[0];
        sendlen = 1;
    }
    else {
        if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND){
            // プロファイルIDリストの最初にサーバ候補のプロファイルIDをセット
            DWCi_GetMatchCnt()->sbPidList[0] = profileID;
        }

        // NN要求送信先プロファイルIDを記録
        DWCi_GetMatchCnt()->reqProfileID = profileID;

        // 友達指定、サーバクライアントマッチメイクの場合は
        // ここでQR2用パブリックIP、ポート番号を送信する。
        // 友達無指定ピアマッチメイクの場合は、SBメッセージヘッダに格納されている。
        senddata[1] = DWCi_GetMatchCnt()->qr2IP;
        senddata[2] = DWCi_GetMatchCnt()->qr2Port;
        sendlen = 3;
    }
    
//    DWCi_GetMatchCnt()->cmdTimeoutCount = 1;  // タイムアウトカウント開始
    DWCi_GetMatchCnt()->cmdTimeoutTime = DWC_MATCH_CMD_RESV_TIMEOUT_MSEC;
    DWCi_GetMatchCnt()->cmdTimeoutStartTick = OS_GetTick();
//    DWCi_GetMatchCnt()->cmdResendCount  = 0;
    DWCi_GetMatchCnt()->cmdResendFlag  = FALSE;
        
    if (DWCi_GetMatchCnt()->priorProfileID){
        // サーバから切断され、切断元サーバから次の接続先サーバを
        // 斡旋された場合は、優先予約コマンドを送信する
        command = DWC_MATCH_COMMAND_RESV_PRIOR;
    }
    else {
        // 通常の予約コマンド
        command = DWC_MATCH_COMMAND_RESERVATION;
    }

    // 送信データに自分のマッチメイクタイプをセットする
    senddata[0] = DWCi_GetMatchCnt()->qr2MatchType;

    // NN予約コマンド送信
    // 予約コマンドの送信は必ずサーバに対して行うのでインデックスは[0]
    result = DWCi_SendMatchCommand(command,
                                   profileID,
                                   DWCi_GetMatchCnt()->qr2IPList[0],
                                   DWCi_GetMatchCnt()->qr2PortList[0],
                                   senddata, sendlen);

    return result;
}


/*---------------------------------------------------------------------------*
  友達指定ピアマッチメイク用NN予約コマンド送信関数
  引数　：delay     TRUE:一定フレーム後にコマンド送信する、
                    FALSE:即コマンド送信する
          init      TRUE:初回呼び出し。friendCountを+1したくない場合、
                    FALSE:それ以外
          resendPid 友達指定ピアマッチメイクで、delay = FALSE であっても、
                    前回と同じプロファイルIDに対して連続して予約コマンドを
                    送信する場合に間隔を空けたい場合は、前回送信先の
                    プロファイルIDを指定する。その必要がなければ0を指定する。
  戻り値：SBError型もしくはGPResult型をint型に変換した値。
          引数 delay = TRUE の時は必ず0
  用途　：友達インデックスリストから次に接続を試みるホストを選択し、
          そのホストの情報をセットして、NN予約コマンドを送信する。
          引数 delay = TRUE の時は、一定時間後にNN予約コマンドを送信する。
 *---------------------------------------------------------------------------*/
static int DWCi_SendResvCommandToFriend(BOOL delay, BOOL init, int resendPid)
{
    char version[12];
    char numEntry[4];
    char distantFriend[2];
    int  profileID;
    int  startIdx = init ? DWCi_GetMatchCnt()->friendCount :
                           (DWCi_GetMatchCnt()->friendCount < DWCi_GetMatchCnt()->friendIdxListLen-1) ? DWCi_GetMatchCnt()->friendCount+1 : 0;
    int  buddyIdx;
    int  initFinished = 0;
    int  versionLen, numEntryLen, distantFriendLen;
    int  i;
    GPBuddyStatus status;
    GPResult gpResult;

    while (1){
        if (!init || initFinished){
            DWCi_GetMatchCnt()->friendCount++;
            if (DWCi_GetMatchCnt()->friendCount >= DWCi_GetMatchCnt()->friendIdxListLen){
                // 最後まで調べたら最初に戻る
                DWCi_GetMatchCnt()->friendCount = 0;
            }
        }

        if (initFinished){
            if (DWCi_GetMatchCnt()->friendCount == startIdx){
                // 1ループしても誰もいなければタイムアウトでの再送を待つ
                // ただ、２人が同時にマッチメイクを開始した場合は、
                // 少し待てばすぐにGPStatusが更新され、検知できるので、
                // タイムアウトまでの時間を短くしておく
//                DWCi_GetMatchCnt()->cmdTimeoutCount =
//                    DWC_MATCH_CMD_RESV_TIMEOUT-DWC_MATCH_CMD_RESEND_INTERVAL;
                DWCi_GetMatchCnt()->cmdTimeoutTime = DWC_MATCH_CMD_RESEND_INTERVAL_MSEC;
                DWCi_GetMatchCnt()->cmdTimeoutStartTick = OS_GetTick();

//                DWCi_GetMatchCnt()->cmdResendCount  = 0;
                DWCi_GetMatchCnt()->cmdResendFlag  = FALSE;
                return 0;
            }
        }
        initFinished = 1;

        profileID = DWC_GetGsProfileId(DWCi_GetUserData(), &DWCi_GetMatchCnt()->friendList[DWCi_GetMatchCnt()->friendIdxList[DWCi_GetMatchCnt()->friendCount]]);

#ifdef DWC_MATCH_ACCEPT_NO_FRIEND
        // まだプロファイルIDができていなければ何もしない
        if ((profileID == 0) || (profileID == -1)) continue;

        // 成立済みバディでない場合も何もしない
        if (!DWCi_Acc_IsValidFriendData(&DWCi_GetMatchCnt()->friendList[DWCi_GetMatchCnt()->friendIdxList[DWCi_GetMatchCnt()->friendCount]])) continue;
#endif

        // 次のサーバ候補が既に接続済みでないか調べる
        for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
            if (DWCi_GetMatchCnt()->sbPidList[i] == profileID) break;
        }

        if (i > DWCi_GetMatchCnt()->gt2NumConnection){
            // 相手が友達指定ピアマッチメイク中であるかどうかをチェックする
            gpResult  = gpGetBuddyIndex(DWCi_GetMatchCnt()->pGpObj, profileID, &buddyIdx);
            gpResult |= gpGetBuddyStatus(DWCi_GetMatchCnt()->pGpObj, buddyIdx, &status);

            if (!gpResult && (status.status == DWC_STATUS_MATCH_FRIEND)){
                // 相手も友達指定ピアマッチメイク中なので、バージョンが自分と
                // 合っているか、接続要求人数が自分と合っているか、
                // 友達の友達許可属性が自分と同じか調べる
                versionLen =
                    DWC_GetCommonValueString(DWC_GP_SSTR_KEY_MATCH_VERSION,
                                             version, 
                                             status.statusString,
                                             '/');
                numEntryLen = 
                    DWC_GetCommonValueString(DWC_GP_SSTR_KEY_MATCH_FRIEND_NUM,
                                             numEntry, 
                                             status.statusString,
                                             '/');
                distantFriendLen = 
                    DWC_GetCommonValueString(DWC_GP_SSTR_KEY_DISTANT_FRIEND,
                                             distantFriend,
                                             status.statusString,
                                             '/');

                if ((versionLen > 0) && (numEntryLen > 0) && (distantFriendLen > 0) &&
                    (strtoul(version, NULL, 10) == DWC_MATCHING_VERSION) &&
                    (strtoul(numEntry, NULL, 10) == DWCi_GetMatchCnt()->qr2NumEntry)){
                    // 予約送信先サーバとして決定
                    break;
                }
            }
            // gpResultのエラーは無視
        }
    }

    // 友達が一人しかいない場合などに、前回と同じプロファイルIDに間隔を空けずに
    // 連続送信してしまうことを防ぐために、前回送信先プロファイルID resendPidが
    // 設定されている場合は、今回も送信先が同じなら一定フレーム後に送信する
    if (profileID == resendPid) delay = TRUE;

    // 次の友達にNN予約要求を送信する
    return DWCi_SendResvCommand(profileID, delay);
}


/*---------------------------------------------------------------------------*
  予約コマンド再送リトライ関数
  引数　：resendPid 友達指定ピアマッチメイクで、delay = FALSE であっても、
                    前回と同じプロファイルIDに対して連続して予約コマンドを
                    送信する場合に間隔を空けたい場合は、前回送信先の
                    プロファイルIDを指定する。その必要がなければ0を指定する。
  戻り値：SBError型もしくはGPResult型をint型に変換した値
  用途　：RESV_DENYコマンド受信時や、コマンド応答待ちタイムアウト後などに、
          予約コマンド送信処理を行う。
          サーバクライアントマッチメイクのクライアントの場合はリトライの必要は
          ない。エラー処理は関数内で行っている。
 *---------------------------------------------------------------------------*/
static BOOL DWCi_RetryReserving(int resendPid)
{
    SBError sbError;
    int result;

    DWCi_GetMatchCnt()->priorProfileID = 0;  // 優先予約クリア
    DWCi_GetMatchCnt()->reqProfileID   = 0;  // 予約クリア

    DWCi_GetMatchCnt()->resvWaitCount  = 0;  // WAIT受信による再送カウンタクリア
    // タイムアウト計測開始時間再セット
    DWCi_GetMatchCnt()->cmdTimeoutStartTick = OS_GetTick();

    if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY){
        // 空きホストの検索状態に戻る
        DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SEARCH_HOST);

        // SBサーバアップデートを行う
        // [todo]
        // これだと既にクライアントが繋がっているサーバの場合も、間を置かずに
        // SBアップデートを行ってしまうので良くない
        sbError = DWCi_SBUpdateAsync(0);
        if (DWCi_HandleSBError(sbError)) return FALSE;
    }
    else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND){
        // 次の友達にNN予約要求を送信する
        result = DWCi_SendResvCommandToFriend(FALSE, FALSE, resendPid);
        if (DWCi_HandleMatchCommandError(result)) return FALSE;
    }
    else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL){
        // サーバが微妙なタイミングで定員オーバーになったか、
        // マッチメイクキャンセルしたのでマッチメイクを中断する
        DWCi_StopMatching(DWC_ERROR_NETWORK,
                          DWC_ECODE_SEQ_MATCH+DWC_ECODE_TYPE_OTHER);
        return FALSE;
    }

    return TRUE;
}


/*---------------------------------------------------------------------------*
  NN予約キャンセル関数
  引数　：profileID 予約をキャンセルしたいサーバのプロファイルID。
  戻り値：SBError型もしくはGPResult型をint型に変換した値
  用途　：NN予約キャンセルコマンドを送信する。
 *---------------------------------------------------------------------------*/
static int  DWCi_CancelReservation(int profileID)
{
    int result;

    result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_RESV_CANCEL,
                                   profileID,
                                   DWCi_GetMatchCnt()->qr2IPList[0],
                                   DWCi_GetMatchCnt()->qr2PortList[0],
                                   NULL, 0);

    // NN要求送信先プロファイルIDをクリア
    DWCi_GetMatchCnt()->reqProfileID = 0;

    return result;
}


/*---------------------------------------------------------------------------*
  接続確立前サーバの処理キャンセル関数
  引数　：clientPid 予約済みクライアントのプロファイルID
  戻り値：TRUE:成功、FALSE:失敗
  用途　：接続確立前のやり取り中のサーバにおいて、全ての処理を

          キャンセルし、今あるコネクションはそのままでマッチメイクを再開する。
          新規接続クライアントとのNN失敗、GT2コネクト失敗といった場合は、
          接続済みクライアントでもこの関数に来ることがある。
          この関数は、新規接続クライアントとのgt2Connectが完了していない
          状態で呼ばれることを前提としている。
 *---------------------------------------------------------------------------*/
static BOOL DWCi_CancelPreConnectedServerProcess(int clientPid)
{
    int  i;
    BOOL isServer;

    if (DWCi_GetMatchCnt()->qr2IsReserved && (DWCi_GetMatchCnt()->qr2Reservation == DWCi_GetMatchCnt()->profileID)){
        DWC_Printf(DWC_REPORTFLAG_DEBUG,
                   "DWCi_CancelPreConnectedServerProcess : client\n");
        isServer = FALSE;
    }
    else {
        DWC_Printf(DWC_REPORTFLAG_DEBUG,
                   "DWCi_CancelPreConnectedServerProcess : server\n");
        isServer = TRUE;
    }

    if (isServer){
        // サーバなら予約をクリア。
        // クライアントの場合、通常はこの先のDWCi_RestartFromTimeout()内で
        // 予約をクリアするが、サーバクライアントマッチメイクの接続済み
        // クライアントはマッチメイクを継続するので、自分のプロファイルIDでの
        // 予約をクリアしない。
        DWCi_GetMatchCnt()->qr2IsReserved   = 0;
        DWCi_GetMatchCnt()->qr2Reservation  = 0;
    
        // 状態の変化をマスターサーバに通知する
        qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
    }

    // 予約クライアントのプロファイルIDをリストから消去。
    // gt2NumConnectionに新規接続クライアントの分が入っていないことが前提。
    if (DWCi_GetMatchCnt()->gt2NumConnection < DWC_MAX_CONNECTIONS-1){
        DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->gt2NumConnection+1] = 0;
    }

    // 予約保持タイムアウト計測終了
    DWCi_GetMatchCnt()->cmdCnt.command = DWC_MATCH_COMMAND_DUMMY;

    if (DWCi_GetMatchCnt()->nnInfo.cookie){
        // NATネゴシエーション中ならキャンセル
        NNCancel(DWCi_GetMatchCnt()->nnInfo.cookie);
        DWCi_GetMatchCnt()->nnInfo.cookie = 0;
    }

    // サーバ検索再開のためにパラメータを検索前状態に戻す
    DWCi_GetMatchCnt()->qr2NNFinishCount = (u8)DWCi_GetMatchCnt()->gt2NumConnection;
    DWCi_GetMatchCnt()->reqProfileID     = 0;

    if (!isServer){
        if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL){
            // サーバクライアントマッチメイクで、接続済みクライアントが
            // 新規接続クライアントの応答無しを検出した場合は、
            // サーバがクライアント同士の接続タイムアウトを検出して、
            // キャンセルコマンドを送ってくれるのを待つ。
            // [todo]
            // キャンセルコマンド受信までのタイムアウト時間を設定して、
            // オーバーしたらサーバがシャットダウンしたと判断する必要がある
        }
        else {
            // その他のマッチメイクタイプでは、コネクションを全てクローズして
            // マッチメイクをやり直す
            DWCi_RestartFromTimeout();
        }
    }
    else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY){
        // 友達無指定ピアマッチメイクの場合、空きホスト検索状態に戻る
        DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SEARCH_HOST);
        
        // 一定フレーム後に空きホストの検索を行う
//        DWCi_GetMatchCnt()->sbUpdateCount = 1;
        DWCi_GetMatchCnt()->sbUpdateFlag = DWC_SB_UPDATE_INTERVAL_LONG;
        DWCi_GetMatchCnt()->sbUpdateTick = OS_GetTick();
    }
    else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND){
        // 友達指定ピアマッチメイクの場合、最初の状態に戻る
        DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAIT_RESV);

        // 一定フレーム後にNN予約要求送信を行う
        (void)DWCi_SendResvCommandToFriend(TRUE, FALSE, 0);
    }
    else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV){
        // サーバクライアントマッチメイクのサーバの場合は、
        // 接続済みクライアント全員にキャンセルを伝える。
        // キャンセルSYN-ACK受信待ち状態に進む
        DWCi_SetMatchStatus(DWC_MATCH_STATE_SV_CANCEL_SYN);

        DWCi_GetMatchCnt()->cancelSynAckBit   = 0;  // キャンセルSYN-ACK受信Bitクリア
        DWCi_GetMatchCnt()->cancelBaseLatency = 0;  // キャンセル同期用レイテンシクリア

        // 新規接続クライアントをクローズする
        DWCi_CloseCancelHostAsync(clientPid);

        // 接続済みクライアントにSYNパケットを送信する。
        for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
            if (!DWCi_SendCancelMatchSynCommand(DWCi_GetMatchCnt()->sbPidList[i],
                                                DWC_MATCH_COMMAND_CANCEL_SYN))
                return FALSE;
        }

        if (DWCi_GetMatchCnt()->gt2NumConnection == 0){
            // 接続ホストがなければ上のSYNパケットもクローズも発生しないので
            // 引き続きマッチメイク状態にする
            DWCi_RestartFromCancel(DWC_MATCH_RESET_CONTINUE);
        }
    }

    return TRUE;
}


/*---------------------------------------------------------------------------*
  接続確立前クライアントの処理キャンセル関数
  引数　：serverPid 予約先サーバのプロファイルID
  戻り値：TRUE:成功、FALSE:失敗
  用途　：接続確立前のやり取り中のクライアントにおいて、全ての処理を
          キャンセルする
 *---------------------------------------------------------------------------*/
static BOOL DWCi_CancelPreConnectedClientProcess(int serverPid)
{
#pragma unused(serverPid)
    BOOL result = TRUE;

    if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL){
        // サーバクライアントマッチメイクの新規接続クライアントなら
        // 接続処理をエラー終了する
        if (DWCi_GetMatchCnt()->gt2NumConnection){
            // 全てのコネクションをクローズする
            DWCi_CloseAllConnectionsByTimeout();
        }

        // マッチメイクエラー終了
        DWCi_StopMatching(DWC_ERROR_NETWORK,
                          DWC_ECODE_SEQ_MATCH+DWC_ECODE_TYPE_SC_CL_FAIL);
        return FALSE;
    }

    // サーバ検索再開のためにパラメータを検索前状態に戻す
    DWCi_GetMatchCnt()->qr2NNFinishCount = (u8)DWCi_GetMatchCnt()->gt2NumConnection;
    DWCi_GetMatchCnt()->priorProfileID   = 0;

    if (DWCi_GetMatchCnt()->nnInfo.cookie){
        // NATネゴシエーション中ならキャンセル
        NNCancel(DWCi_GetMatchCnt()->nnInfo.cookie);
        DWCi_GetMatchCnt()->nnInfo.cookie = 0;
    }

    if (DWCi_GetMatchCnt()->gt2NumConnection){
        // サーバに繋がっているクライアントと接続中の場合、
        // もしくはサーバと接続中の場合でも、既にgt2Connect()をAcceptして
        // しまっていたら、コネクションを全てクローズしてマッチメイクをやり直す
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                   "Close all connection and restart matching.\n");
            
        DWCi_RestartFromTimeout();
    }
    else {
        // まだコネクションがない状態なら、状態を元に戻して
        // クライアントのマッチメイク処理を再開する
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                   "Cancel and restart client process.\n");
        
        // 状態を戻す
        DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAIT_RESV);

        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Cancel and retry to reserve.\n");

        // RESV_DENYコマンドを受信した時と同じ処理で、次のサーバを探す
        result = DWCi_RetryReserving(0);
    }

    return result;
}


/*---------------------------------------------------------------------------*
  クライアント転向関数
  引数　：なし
  戻り値：SBError型もしくはGPResult型をint型に変換した値
  用途　：完全にクライアントに戻って、新規サーバへ接続しに行く
 *---------------------------------------------------------------------------*/
static int  DWCi_ChangeToClient(void)
{
    int result;
    int i;

    // 接続済みのクライアントにクローズ命令を送信する
    for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
        result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_CLOSE_LINK,
                                       DWCi_GetMatchCnt()->sbPidList[i],
                                       DWCi_GetMatchCnt()->qr2IPList[i],
                                       DWCi_GetMatchCnt()->qr2PortList[i],
                                       DWCi_GetMatchCnt()->svDataBak,
                                       (int)(DWCi_GetMatchCnt()->svDataBak[0]+1));
        if (result) return result;
    }

    DWCi_GetMatchCnt()->qr2IsReserved  = 0;
    DWCi_GetMatchCnt()->qr2Reservation = 0;

    // クローズ状態をクライアントへの転向処理中にする
    DWCi_GetMatchCnt()->closeState = DWC_MATCH_CLOSE_STATE_CHANGE_TO_CLIENT;
    
    // 全てのコネクションを切断する
    gt2CloseAllConnectionsHard(*DWCi_GetMatchCnt()->pGt2Socket);

    // クローズ状態初期化
    DWCi_GetMatchCnt()->closeState = DWC_MATCH_CLOSE_STATE_INIT;

    DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
               "Closed all connections. Begin NN to %u\n",
               DWCi_GetMatchCnt()->reqProfileID);

    return 0;
}


/*---------------------------------------------------------------------------*
  コネクション確立後処理関数
  引数　：type 処理種別。DWCMatchPpConnectionType型。
  戻り値：なし
  用途　：１つのコネクション確立が終わった後に次のコネクション確立開始、
          もしくは接続完了処理を行う。
 *---------------------------------------------------------------------------*/
static void DWCi_PostProcessConnection(DWCMatchPpConnectionType type)
{
    u32 senddata[5];
    int sendlen = 3;
    int result;
    int i;
    BOOL sbClear = FALSE;
    GPResult gpResult;

    switch (type){
    case DWC_PP_CONNECTION_SV_CONNECT:
        // サーバで一件接続完了
        if (DWCi_GetMatchCnt()->clLinkProgress < DWCi_GetMatchCnt()->gt2NumConnection-1){
            // 新規接続クライアントの、接続済みクライアントへの接続が
            // 全て終わっていない場合
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                       "Send client-client link request.\n");

            // クライアント同士のコネクション確立待ち状態に移行する
            DWCi_SetMatchStatus(DWC_MATCH_STATE_SV_WAIT_CL_LINK);

            // クライアントに接続しに行ってほしい他クライアントの
            // プロファイルIDとAIDインデックス、AIDを通知する
            senddata[0] = (u32)DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->clLinkProgress+1];
            senddata[1] = (u32)(DWCi_GetMatchCnt()->clLinkProgress+1);
            senddata[2] = DWCi_GetMatchCnt()->aidList[DWCi_GetMatchCnt()->clLinkProgress+1];

            // qr2用パブリックIPとポート番号も送信する
            senddata[3] = DWCi_GetMatchCnt()->qr2IPList[DWCi_GetMatchCnt()->clLinkProgress+1];
            senddata[4] = DWCi_GetMatchCnt()->qr2PortList[DWCi_GetMatchCnt()->clLinkProgress+1];
            sendlen     = 5;
        }
        else {
            // 初めてのクライアントとの自分のGT2接続が完了した場合か、
            // 全ての接続クライアント同士の接続が終わった場合
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                       "Tell new client completion of matching.\n");
            // 予約を解除
            DWCi_GetMatchCnt()->qr2IsReserved  = 0;
            DWCi_GetMatchCnt()->qr2Reservation = 0;

            // 状態の変化をマスターサーバに通知する
            qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);

            if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY){
                // 友達無指定ピアマッチメイクの場合は、空きホストの検索状態に戻る
                DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SEARCH_HOST);
            }
            else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND){
                // 友達指定ピアマッチメイクの場合はクライアント初期状態に戻る
                DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAIT_RESV);
            }
            else {
                // サーバクライアントマッチメイクのサーバの場合は待ち状態に戻る
                DWCi_SetMatchStatus(DWC_MATCH_STATE_SV_WAITING);
            }
            
            DWCi_GetMatchCnt()->clLinkProgress = 0;  // クライアント間接続数クリア

            if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV) ||
                (DWCi_GetMatchCnt()->gt2NumConnection == DWCi_GetMatchCnt()->qr2NumEntry)){
                // 規定人数のネットワーク完成（サーバクライアントマッチメイクの
                // 場合は１台接続ごと）。
                // コールバックに渡すプロファイルIDを退避
                if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV){
                    DWCi_GetMatchCnt()->cbEventPid =
                        DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->gt2NumConnection];
                }
                else {
                    DWCi_GetMatchCnt()->cbEventPid = 0;

                    // サーバからクライアントに転向したホストのために、
                    // ここで改めて自分のプロファイルIDをリストにセットする
                    // （既定接続数のマッチメイクが完了していた場合に必要）
                    DWCi_GetMatchCnt()->sbPidList[0] = DWCi_GetMatchCnt()->profileID;
                }

                // SYN-ACK受信待ち状態に進む
                DWCi_SetMatchStatus(DWC_MATCH_STATE_SV_SYN);

                // SYN-ACK受信Bitクリア
                DWCi_GetMatchCnt()->synAckBit = 0;

                // 接続済みクライアントにSYNパケットを送信する。
                for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
                    DWCi_SendMatchSynPacket(DWCi_GetMatchCnt()->aidList[i],
                                            DWC_SEND_TYPE_MATCH_SYN);
                }
            }
            else {
                // 友達指定・無指定ピアマッチメイクで接続ホストが規定数に満たない場合
                // 間を置いて新たな接続先を探す
                senddata[0] = 0;  // プロファイルID=0で終了を通知
                // 新規接続クライアントのAIDインデックスとAIDを教える
                senddata[1] = (u8)DWCi_GetMatchCnt()->gt2NumConnection;
                senddata[2] = DWCi_GetMatchCnt()->aidList[DWCi_GetMatchCnt()->gt2NumConnection];

                if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY){
                    // 友達無指定ピアマッチメイクの場合は、空きホストの検索状態に戻る
//                    DWCi_GetMatchCnt()->sbUpdateCount = 1;
                    DWCi_GetMatchCnt()->sbUpdateFlag = DWC_SB_UPDATE_INTERVAL_LONG;
                    DWCi_GetMatchCnt()->sbUpdateTick = OS_GetTick();
                }
                else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND){
                    // 友達指定ピアマッチメイクの場合は、一定フレーム後に
                    // NN予約要求を送信する
                    (void)DWCi_SendResvCommandToFriend(TRUE, FALSE, 0);

#ifdef DWC_LIMIT_FRIENDS_MATCH_VALID
                    if (!DWCi_GetMatchCnt()->distantFriend &&
                        (DWCi_GetMatchCnt()->gt2NumConnection >= 2)){
                        // 友達の友達との接続を許可せず、２人目以降の
                        // クライアントとの接続を終えた場合は、
                        // 接続済みクライアントからの新規接続友達許可コマンドを
                        // 全て受信できているか調べる
                        if (DWCi_GetMatchCnt()->friendAcceptBit !=
                            (DWCi_GetAIDBitmask(FALSE) & ~(1 << DWCi_GetMatchCnt()->aidList[DWCi_GetMatchCnt()->gt2NumConnection]))){
                            // 受信できていなければマッチメイクをやり直す
                            // [todo]
                            // 遅延が発生した場合のことを考えると、もう少し
                            // 待った方が良い
                            DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                                       "FRIEND_ACCEPT command droped.\n");
                            DWCi_RestartFromTimeout();
                            break;
                        }
                    }
#endif
                }
            }

            if (DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_SC_SV){
                // サーバクライアントマッチメイクでなければ
                // サーバブラウジングをクリア
                sbClear = TRUE;
            }
        }

        if (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_SV_SYN){
            // クライアント同士の接続要求通知を行う（全て終了なら終了通知になる）
            result =
                DWCi_SendMatchCommand(DWC_MATCH_COMMAND_LINK_CLS_REQ,
                                      DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->gt2NumConnection],
                                      DWCi_GetMatchCnt()->qr2IPList[DWCi_GetMatchCnt()->gt2NumConnection],
                                      DWCi_GetMatchCnt()->qr2PortList[DWCi_GetMatchCnt()->gt2NumConnection],
                                      senddata, sendlen);
            if (DWCi_HandleMatchCommandError(result)) return;

            DWCi_GetMatchCnt()->cmdCnt.count = 0;  // コマンド再送カウント開始
        }
        break;
    
    case DWC_PP_CONNECTION_CL_GT2_CONNECT:
        // 待ち状態に戻る
        DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAITING);

        // サーバクライアントマッチメイクのクライアントの場合は
        // ここでコールバックに渡すプロファイルIDを退避
        if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL){
            DWCi_GetMatchCnt()->cbEventPid = DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->gt2NumConnection];
        }

        // サーバブラウジングをクリア
        sbClear = TRUE;
        break;

    case DWC_PP_CONNECTION_CL_GT2_ACCEPT:
        // 待ち状態になる
        DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAITING);

        if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY) ||
            (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND)){
            // 友達指定・無指定ピアマッチメイクの場合は、ここで初めて自分の
            // プロファイルIDで予約状態にして、他のホストが接続してくるのを防ぐ
            DWCi_GetMatchCnt()->qr2IsReserved  = 1;
            DWCi_GetMatchCnt()->qr2Reservation = DWCi_GetMatchCnt()->profileID;
        }

        // 次にサーバからDWC_MATCH_COMMAND_LINK_CLS_REQコマンドを受信するまでは
        // 待ち状態なので、タイムアウトカウンタを初期化する
        DWCi_InitClWaitTimeout();
        
        if (DWCi_GetMatchCnt()->gt2NumConnection > 1){
            // 新規接続クライアントが、接続済みのクライアントとの
            // コネクションを確立した場合は、サーバに完了を伝える。
            // 今コネクションを確立したクライアントのプロファイルIDも伝える。
            result =
                DWCi_SendMatchCommand(DWC_MATCH_COMMAND_LINK_CLS_SUC,
                                      DWCi_GetMatchCnt()->sbPidList[0],
                                      DWCi_GetMatchCnt()->qr2IPList[0],
                                      DWCi_GetMatchCnt()->qr2PortList[0],
                                      (u32 *)&DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->gt2NumConnection-1],
                                      1);
            if (DWCi_HandleMatchCommandError(result)) return;
        }
        // サーバからの接続完了か接続済みのクライアントへの接続かの指示を待つ
        break;
        
    case DWC_PP_CONNECTION_CL_FINISH_CONNECT:
        // CLS_REQコマンドが落ちても大丈夫なように、SYNパケットで必ず
        // aidを送信するようにしているのでリカバーできるが、
        // サーバブラウジングのクリアができないので、なかなかマッチメイクが
        // 終了しなければSBタイムアウトエラーになってしまう
            
        // 待ち状態になる
        DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAITING);

        // マッチメイクが完了するまでは待ち状態になるので、
        // タイムアウトカウンタを初期化する
        DWCi_InitClWaitTimeout();

        // コールバックに渡すプロファイルIDは必要ない
        DWCi_GetMatchCnt()->cbEventPid = 0;

        // サーバブラウジングをクリア
        sbClear = TRUE;
        break;

    case DWC_PP_CONNECTION_SYN_FINISH:
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Completed matching!\n");

        if (DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_SC_SV){
            // サーバクライアントマッチメイクのサーバでなければ
            // GPステータスをゲームプレイ状態にセットする
            (void)DWCi_SetGPStatus(DWC_STATUS_PLAYING, "", NULL);
        }

        // dwc_main.cのコールバックを呼び出す
        DWCi_GetMatchCnt()->matchedCallback(DWC_ERROR_NONE,
                                     FALSE,
                                     DWCi_GetMatchCnt()->cbEventPid ? FALSE : TRUE,
                                     FALSE,
                                     DWCi_GetFriendListIndex(DWCi_GetMatchCnt()->cbEventPid),
                                     DWCi_GetMatchCnt()->matchedParam);

        if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY) ||
            (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND)){
            // 友達指定・無指定ピアマッチメイクは途中参加なしなので
            // ここでGameSpyの制御構造体を開放する
            DWCi_CloseMatching();
        }
        else {
#ifndef DWC_BUG_WIFI_DELAY_SEND
            // サーバクライアントマッチメイクではSBオブジェクトのみ開放する
            if (DWCi_GetMatchCnt()->sbObj){
                ServerBrowserFree(DWCi_GetMatchCnt()->sbObj);
                DWCi_GetMatchCnt()->sbObj = NULL;  // 自分でNULLクリアしなければならない！
            }
#endif

            // NN使用領域も開放する
            DWCi_NNFreeNegotiateList();

            if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV){
                // サーバクライアントマッチメイクのサーバの場合は、
                // 接続人数の増加をGPステータスにセットする
                gpResult = DWCi_GPSetServerStatus();
                if (DWCi_HandleGPError(gpResult)) return;

                // マッチメイクが１件完了したら新規接続をブロックするオプションが
                // 有効なら、ここでブロックする
                if (stOptSCBlock.valid == 1){
                    stOptSCBlock.lock = 1;
                    DWC_Printf(DWC_REPORTFLAG_DEBUG,
                               "[OPT_SC_BLOCK] Connect block start!\n");
                }

                // 状態をサーバ待ち状態に戻す
                DWCi_SetMatchStatus(DWC_MATCH_STATE_SV_WAITING);
            }
            else {
                // 状態をクライアント待ち状態に戻す
                DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAITING);
            }

            // コールバックに渡す為のプロファイルIDを初期化する
            DWCi_GetMatchCnt()->cbEventPid = 0;
        }

        // キャンセルが無視された場合の為にキャンセル状態を初期化する
        DWCi_GetMatchCnt()->cancelState = DWC_MATCH_CANCEL_STATE_INIT;
        break;
    }


#ifndef DWC_BUG_WIFI_DELAY_SEND
#if 1
    if (sbClear &&
        (DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_SC_CL)){
        // 友達指定、無指定ピアマッチメイクの場合、しばらくマスターサーバに
        // アクセスしない場合は、disconnectしないと通信エラーになるので
        // サーバリストクリア
        ServerBrowserClear(DWCi_GetMatchCnt()->sbObj);
	}
#else
    if (sbClear &&
        (DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_SC_CL) &&
        (ServerBrowserState(DWCi_GetMatchCnt()->sbObj) != sb_disconnected)){
        // 友達指定、無指定ピマッチメイクの場合、しばらくマスターサーバに
        // アクセスしない場合は、disconnectしないと通信エラーになるので
        // サーバリストクリア
        ServerBrowserClear(DWCi_GetMatchCnt()->sbObj);
    }
#endif
#endif
}


/*---------------------------------------------------------------------------*
  複数バディ確認関数
  引数　：pidList    バディかどうか確認したいプロファイルIDの配列へのポインタ
          pidListLen 上記配列の要素数
  戻り値：TRUE:全てのプロファイルIDがバディ、FALSE:バディでないプロファイルIDがある
  用途　：プロファイルIDのリストを調べて、全てバディかどうかを返す
 *---------------------------------------------------------------------------*/
static BOOL DWCi_AreAllBuddies(const u32 pidList[], u32 pidListLen)
{
    int i;

    // 友達の友達を許す場合、新規サーバを見つけたクライアントは必ず
    // そのサーバへの接続を許可する
    if (DWCi_GetMatchCnt()->distantFriend &&
        (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_WAIT_RESV))
        return TRUE;

    for (i = 0; i < pidListLen; i++){
#ifdef DWC_LIMIT_FRIENDS_MATCH_VALID
        if (!DWCi_IsFriendByIdxList((int)pidList[i])) return FALSE;
#else
        if (!gpIsBuddy(DWCi_GetMatchCnt()->pGpObj, (int)pidList[i])) return FALSE;
#endif

        if (DWCi_GetMatchCnt()->distantFriend &&
            (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_WAITING)){
            // 友達の友達を許す場合、現在のサーバからクローズされた
            // クライアントは、新規サーバ候補（pidList[0]）のみ
            // 自分の友達なら、そのサーバへの接続を許可する
            return TRUE;
        }
    }

    return TRUE;
}


/*---------------------------------------------------------------------------*
  マッチメイクキャンセル実行関数
  引数　：なし
  戻り値：なし
  用途　：マッチメイクのキャンセル処理を実行する
 *---------------------------------------------------------------------------*/
static void DWCi_DoCancelMatching(void)
{
    u32 senddata;
    int result;

    DWC_Printf(DWC_REPORTFLAG_DEBUG, "CANCEL! state %d, numHost nn=%d gt2=%d.\n",
               DWCi_GetMatchCnt()->state, DWCi_GetMatchCnt()->qr2NNFinishCount,
               DWCi_GetMatchCnt()->gt2NumConnection);

    // 自分でキャンセルした場合はキャンセルホストのプロファイルIDは0
    DWCi_GetMatchCnt()->cbEventPid = 0;
    
    if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_SEARCH_OWN){
        // 未接続なので即キャンセル処理を実行できる
        DWCi_FinishCancelMatching();
        return;
    }

    // キャンセル実行状態をセットする
    DWCi_GetMatchCnt()->cancelState = DWC_MATCH_CANCEL_STATE_EXEC;

    if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL){
        // サーバクライアントマッチメイクの新規接続クライアントがキャンセル
        senddata = 0;

        // キャンセル実行状態をセット
        DWCi_GetMatchCnt()->cancelState = DWC_MATCH_CANCEL_STATE_EXEC;

        if (DWCi_GetMatchCnt()->gt2NumConnection){
            // 全てのコネクションを切断する
            gt2CloseAllConnectionsHard(*DWCi_GetMatchCnt()->pGt2Socket);
        }
        else {
            // コネクションがまだない場合は予約のキャンセルのみ行う。
            // このコマンドが落ちても、サーバ側でタイムアウトで気付くので
            // 再送の必要はない。
            result = DWCi_CancelReservation(DWCi_GetMatchCnt()->sbPidList[0]);
            if (DWCi_HandleMatchCommandError(result)) return;
        }

        // キャンセル処理終了
        DWCi_FinishCancelMatching();
        return;
    }

    // gt2コネクションがまだできていないが、接続処理中のホストに対する処理
    if ((DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_WAIT_RESV) ||
        (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_SEARCH_NN_HOST) ||
        (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_NN) ||
        (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_GT2) ||
        (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_OWN_NN) ||
        (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_OWN_GT2)){
        if (DWCi_GetMatchCnt()->reqProfileID){
            // 誰かに予約コマンドを送った後の状態の場合
            // 予約コマンドを送った相手に対して予約をキャンセルする
            result = DWCi_CancelReservation(DWCi_GetMatchCnt()->sbPidList[0]);
            if (DWCi_HandleMatchCommandError(result)) return;
        }

        // 自分が誰かに予約されていれば、マッチメイクキャンセルコマンドを
        // クライアントに送信
        result = DWCi_InvalidateReservation();
        if (DWCi_HandleMatchCommandError(result)) return;
    }

    if (DWCi_GetMatchCnt()->nnInfo.cookie){
        // NATネゴシエーション中ならキャンセル
        NNCancel(DWCi_GetMatchCnt()->nnInfo.cookie);
        DWCi_GetMatchCnt()->nnInfo.cookie = 0;
    }

    if (DWCi_GetMatchCnt()->gt2NumConnection){
        // キャンセル実行状態に移行する
        DWCi_GetMatchCnt()->cancelState = DWC_MATCH_CANCEL_STATE_EXEC;

        // 全てのコネクションを切断する
        gt2CloseAllConnectionsHard(*DWCi_GetMatchCnt()->pGt2Socket);
    }

    // キャンセル処理終了
    DWCi_FinishCancelMatching();
}


/*---------------------------------------------------------------------------*
  マッチメイクキャンセル終了関数
  引数　：なし
  戻り値：なし
  用途　：マッチメイクキャンセルした側が、キャンセルが完了した時に
          終了処理を行う
 *---------------------------------------------------------------------------*/
static void DWCi_FinishCancelMatching(void)
{
    GPResult gpResult;

    // GPステータスをオンラインにする
    gpResult = DWCi_SetGPStatus(DWC_STATUS_ONLINE, "", NULL);
    if (DWCi_HandleGPError(gpResult)) return;

    // マッチメイク処理をクローズする
    DWCi_CloseMatching();

    // マッチメイク完了コールバック呼び出し
    DWCi_GetMatchCnt()->matchedCallback(DWC_ERROR_NONE,
                                 TRUE,
                                 DWCi_GetMatchCnt()->cbEventPid ? FALSE : TRUE,
                                 DWCi_GetMatchCnt()->cbEventPid ? TRUE : ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV) ? TRUE : FALSE),
                                 DWCi_GetFriendListIndex(DWCi_GetMatchCnt()->cbEventPid),
                                 DWCi_GetMatchCnt()->matchedParam);

    // キャンセル状態初期化
    DWCi_GetMatchCnt()->cancelState = DWC_MATCH_CANCEL_STATE_INIT;
}


/*---------------------------------------------------------------------------*
  クライアント予約無効化関数
  引数　：なし
  戻り値：SBError型もしくはGPResult型をint型に変換した値
  用途　：自分がクライアントに予約されている場合、その予約を取り消し、
          予約元クライアントにその旨を通知する
 *---------------------------------------------------------------------------*/
static int  DWCi_InvalidateReservation(void)
{
    u32 senddata = 0;
    int result   = 0;

    if (DWCi_GetMatchCnt()->qr2IsReserved && DWCi_GetMatchCnt()->qr2Reservation &&
        (DWCi_GetMatchCnt()->qr2Reservation != DWCi_GetMatchCnt()->profileID)){
        result =
            DWCi_SendMatchCommand(DWC_MATCH_COMMAND_CANCEL,
                                  DWCi_GetMatchCnt()->qr2Reservation,
                                  DWCi_GetMatchCnt()->qr2IPList[DWCi_GetMatchCnt()->gt2NumConnection+1],
                                  DWCi_GetMatchCnt()->qr2PortList[DWCi_GetMatchCnt()->gt2NumConnection+1],
                                  &senddata, 1);
        
        DWCi_GetMatchCnt()->qr2IsReserved  = 0;
        DWCi_GetMatchCnt()->qr2Reservation = 0;

        // send_statechanged必要ない？
    }

    return result;
}


/*---------------------------------------------------------------------------*
  マッチメイクキャンセル後再スタート関数
  引数　：level マッチメイク再開レベル。DWCMatchResetLevel列挙子で定義される。
  戻り値：なし
  用途　：マッチメイクをキャンセルされた側が、キャンセル処理が終わった後に
          マッチメイクの再開（場合によっては終了）処理を行う
 *---------------------------------------------------------------------------*/
static void DWCi_RestartFromCancel(DWCMatchResetLevel level)
{
    SBError sbError;

    if (level == DWC_MATCH_RESET_ALL){        
        // マッチメイクを終了する
        DWCi_FinishCancelMatching();
    }
    else {
        // マッチメイク制御パラメータ初期化
        DWCi_ResetMatchParam(level);

        if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV) ||
            (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL)){
            // キャンセルでマッチメイクコールバックを呼び出す。
            // levelがどちらの値でもコールバックはセットで、
            // この後はリセットされたパラメータで待ち状態になる。
            DWCi_GetMatchCnt()->matchedCallback(DWC_ERROR_NONE,
                                         TRUE,
                                         DWCi_GetMatchCnt()->cbEventPid ? FALSE : TRUE,
                                         FALSE,
                                         DWCi_GetFriendListIndex(DWCi_GetMatchCnt()->cbEventPid),
                                         DWCi_GetMatchCnt()->matchedParam);
        }
        else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY){
            if (level == DWC_MATCH_RESET_RESTART){
                // SBサーバアップデートを行う
                sbError = DWCi_SBUpdateAsync(0);
                if (DWCi_HandleSBError(sbError)) return;
            }
        }
        else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND){
            if (level == DWC_MATCH_RESET_RESTART){
                // GPメッセージで最初のサーバにNATネゴシエーションの予約を行う
                // 引数 init を TRUE にすると無限ループに陥る可能性がある
                // [todo]
                // なぜ無限ループなのか調べて、できることならinit=TRUEで
                (void)DWCi_SendResvCommandToFriend(FALSE/*TRUE*/, FALSE, 0);
            }
        }
        else {
            // [test]
            // 来ることはないはず
            DWC_Printf(DWC_REPORTFLAG_ERROR,
                       "ERROR - DWCi_RestartFromCancel : matchType %d, level %d\n",
                       DWCi_GetMatchCnt()->qr2MatchType, level);
        }
    }
}


/*---------------------------------------------------------------------------*
  タイムアウト検出後再スタート関数
  引数　：なし
  戻り値：なし
  用途　：タイムアウトホストを見つけたホストが、全コネクションをクローズし、
          最初からマッチメイクをやり直す。
          サーバクライアントマッチメイクの場合はこの関数を呼んではいけない！
 *---------------------------------------------------------------------------*/
static void DWCi_RestartFromTimeout(void)
{

    // [test]
    // サーバクライアントマッチメイクの場合はこの関数を呼んではいけない！
    if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV) ||
        (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL)){
        DWC_Printf(DWC_REPORTFLAG_WARNING,
                   "DWCi_RestartFromTimeout() shouldn't be called.\n");
        return;
    }

    // 全てのコネクションをクローズする
    DWCi_CloseAllConnectionsByTimeout();

    DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
               "Closed all connections and restart matching.\n");

    // 最初からマッチメイクをやり直す
    DWCi_RestartFromCancel(DWC_MATCH_RESET_RESTART);
}


/*---------------------------------------------------------------------------*
  コネクションクローズ後マッチメイク再開関数
  引数　：なし
  戻り値：SBError型かGPResult型をint型に変換した値
  用途　：一旦サーバに接続し待機中となっていたクライアントにおいて、
          マッチメイクキャンセルなどで、コネクションが他ホストからクローズ
          された後、マッチメイクを再開する。
          サーバクライアントマッチメイクの場合はこの関数を呼んではいけない！
 *---------------------------------------------------------------------------*/
static int DWCi_ResumeMatching(void)
{
    SBError sbError;
    int result;

    // 予約を解除 
    DWCi_GetMatchCnt()->qr2IsReserved  = 0;
    DWCi_GetMatchCnt()->qr2Reservation = 0;

    // クローズ状態を初期化する
    DWCi_GetMatchCnt()->closeState     = DWC_MATCH_CLOSE_STATE_INIT;

    if (DWCi_GetMatchCnt()->priorProfileID){
        // クローズ命令送信元サーバから斡旋された次の接続サーバが有効な場合。
        // サーバクライアントマッチメイクではpriorProfileIDはセットされない
        if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY){
            // 友達無指定の場合は、元サーバから紹介された
            // 新サーバを探す状態に戻る
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SEARCH_HOST);

            sbError = DWCi_SBUpdateAsync(0);
            if (DWCi_HandleSBError(sbError)) return sbError;
        }
        else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND){
            // 友達指定の場合、すぐにNN予約を行う
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAIT_RESV);
            
            result = DWCi_SendResvCommand(DWCi_GetMatchCnt()->priorProfileID, FALSE);
            if (DWCi_HandleMatchCommandError(result)) return result;
        }
    }
    else {
        // クローズ命令送信元サーバから斡旋された次の接続サーバ、
        // もしくはそこの接続ホストが自分の友達で無かった場合。
        DWCi_RestartFromCancel(DWC_MATCH_RESET_RESTART);
    }

    return 0;
}


/*---------------------------------------------------------------------------*
  キャンセルホストクローズ関数
  引数　：profileID クローズしたいホストのプロファイルID
  戻り値：TRUE:クローズ処理が発生、FALSE:指定ホストとは元々コネクションがなく
          クローズ処理は行っていない
  用途　：マッチメイクキャンセル時に、キャンセル元ホストとのコネクションを
          クローズする。
 *---------------------------------------------------------------------------*/
static BOOL DWCi_CloseCancelHostAsync(int profileID)
{
    BOOL ret;
    GT2Connection* connection;

    // キャンセルクライアントのプロファイルIDを記録
    DWCi_GetMatchCnt()->cbEventPid = profileID;

    // 指定のプロファイルIDのホストとのgt2コネクションを取得
    connection =
        DWCi_GetGT2ConnectionByProfileID(profileID, DWCi_GetMatchCnt()->gt2NumConnection+1);

    if (connection){
        // クローズ状態をタイムアウトによるクローズにする
        DWCi_GetMatchCnt()->closeState = DWC_MATCH_CLOSE_STATE_TIMEOUT;
        
        // コネクションがある場合はコネクションのクローズを行う。
        gt2CloseConnectionHard(*connection);

        // クローズ状態を元に戻す
        DWCi_GetMatchCnt()->closeState = DWC_MATCH_CLOSE_STATE_INIT;
        ret = TRUE;
    }
    else {
        // コネクションがない場合はリストから情報を消すだけ
        (void)DWCi_DeleteHostByProfileID(profileID, DWCi_GetMatchCnt()->gt2NumConnection+1);
        
        ret = FALSE;
    }

    return ret;
}


/*---------------------------------------------------------------------------*
  タイムアウトホスト発見時のコネクションクローズ関数
  引数　：なし
  戻り値：なし
  用途　：タイムアウトホストを発見した時に、全てのホストとのコネクションを
          クローズする。
          コネクションクローズコールバック内で何もせずにコネクションをクローズ
          したい場合も、このクローズ関数を使う。
 *---------------------------------------------------------------------------*/
static void DWCi_CloseAllConnectionsByTimeout(void) 
{

    // クローズ状態をタイムアウト検出によるクローズにする
    DWCi_GetMatchCnt()->closeState = DWC_MATCH_CLOSE_STATE_TIMEOUT;
    
    // 全てのコネクションを切断する
    gt2CloseAllConnectionsHard(*DWCi_GetMatchCnt()->pGt2Socket);

    // クローズ状態初期化
    DWCi_GetMatchCnt()->closeState = DWC_MATCH_CLOSE_STATE_INIT;
}


/*---------------------------------------------------------------------------*
  サーバクライアントマッチメイク時タイムアウトクライアントクローズ＆通知関数
  引数　：bitmap 応答のあるホストのAIDビットマップ（サーバは含まない）
  戻り値：TRUE:成功、FALSE:エラー発生
  用途　：サーバクライアントマッチメイクのサーバにおいて、
          タイムアウトクライアントを発見した時に、それらのクライアントを
          クローズし、接続済みクライアントにも通知する
 *---------------------------------------------------------------------------*/
static BOOL DWCi_CloseShutdownClientSC(u32 bitmap)
{
    u8  aid;
    int senddata[DWC_MAX_CONNECTIONS];
    int senddataLen = 0;
    int validPid[DWC_MAX_CONNECTIONS];
    int validPidLen = 0;
    int result;
    int i;

    // [note]
    // このやり方だと、サーバ側はクローズコールバックが呼ばれ、その後
    // 同期調整が完了してそのコールバックが返ってくるが、クライアント側は
    // マッチメイク完了同期調整の場合は、GT2のパケットの方が速く届くので、
    // 同期調整完了のコールバックが返ってきてから、クローズコールバックが
    // 呼ばれてしまう
    
    // まずは無応答ホストのプロファイルIDリストを作成する
    for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
        if (bitmap & (1 << DWCi_GetMatchCnt()->aidList[i])){
            validPid[validPidLen] = DWCi_GetMatchCnt()->sbPidList[i];
            validPidLen++;
        }
        else {
            senddata[senddataLen] = DWCi_GetMatchCnt()->sbPidList[i];
            senddataLen++;
        }
    }

    // 応答のあったクライアントには無応答クライアントクローズのコマンドを送信する
    for (i = 0; i < validPidLen; i++){
        result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_SC_CLOSE_CL,
                                       validPid[i], 0, 0,
                                       (u32 *)senddata, senddataLen);
        if (DWCi_HandleMatchCommandError(result)) return FALSE;
    }

    // マッチメイク中のクライアントをクローズしても何もしないように
    // クローズ状態をセットする
    DWCi_GetMatchCnt()->closeState = DWC_MATCH_CLOSE_STATE_TIMEOUT;

    // 無応答ホストをクローズする
    for (i = 0; i < senddataLen; i++){
        aid = DWCi_GetAIDFromProfileID(senddata[i], FALSE);
        if (aid != 0xff){
            // コネクションをクローズする
            DWC_CloseConnectionHard(aid);
        }
    }

    // クローズ状態をクリアする
    DWCi_GetMatchCnt()->closeState = DWC_MATCH_CLOSE_STATE_INIT;

    return TRUE;
}

/*---------------------------------------------------------------------------*
  マッチメイク完了同期調整パケット送信関数
  引数　：aid  送信先ホストのAID
          type DWC Transportパケットタイプ。DWC_SEND_TYPE_*で定義される
  戻り値：なし
  用途　：マッチメイク完了同期調整のパケットを送信する
 *---------------------------------------------------------------------------*/
static void DWCi_SendMatchSynPacket(u8 aid, u16 type)
{
    u8  i;
    u8  senddata[4];

    DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "Sent SYN %d packet to aid %d.\n",
               type-DWC_SEND_TYPE_MATCH_SYN, aid);

    switch (type){
    case DWC_SEND_TYPE_MATCH_SYN:
        if (aid == DWCi_GetMatchCnt()->aidList[DWCi_GetMatchCnt()->gt2NumConnection]){
            // 新規接続クライアントに送る時はフラグを立てる
            senddata[0] = 1;
        }
        else {
            senddata[0] = 0;
        }

        for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
            if (aid == DWCi_GetMatchCnt()->aidList[i]){
                // コマンドCLS_REQが落ちた時の為に、必ずAIDインデックスと
                // AIDも伝える。
                // 現在の仕様では必ずAIDインデックス＝AID。
                senddata[1] = i;
                senddata[2] = aid;
                break;
            }
        }
        break;

    case DWC_SEND_TYPE_MATCH_SYN_ACK:
        // サーバとのレイテンシを付加して送信する
        senddata[0] = (u8)(DWCi_GetMatchCnt()->baseLatency & 0xff);
        senddata[1] = (u8)((DWCi_GetMatchCnt()->baseLatency >> 8) & 0xff);
        break;
    }
    // DWC_SEND_TYPE_MATCH_ACKの時は付加データはなし

    DWCi_SendReliable(type, aid, senddata, DWC_MATCH_SYN_DATA_BODY_SIZE);

    // 送信時間を記録
    DWCi_GetMatchCnt()->lastSynSent = OS_GetTick();
}


/*---------------------------------------------------------------------------*
  マッチメイク完了同期調整タイムアウト処理関数
  引数　：なし
  戻り値：TRUE:正常動作、FALSE:エラー発生
  用途　：マッチメイク完了同期調整のタイムアウトをチェックし、
          タイムアウトに達していたら再送処理を行う
 *---------------------------------------------------------------------------*/
static BOOL DWCi_ProcessMatchSynTimeout(void)
{
    u64 passTime;
    int i;

    if ((DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_SYN) ||
        (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_SYN) ||
        (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_SYN_WAIT))
        passTime = OS_TicksToMilliSeconds(OS_GetTick()-DWCi_GetMatchCnt()->lastSynSent);
    else return TRUE;

    switch (DWCi_GetMatchCnt()->state){
    case DWC_MATCH_STATE_CL_SYN:
        if (passTime > DWC_MATCH_SYN_ACK_WAIT_TIME){
            // クライアントがACK待ち状態の時に一定時間が経過した場合、
            // サーバにSYN-ACKを再送する
            DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "[SYN] No ACK from server %d/%d.\n",
                       DWCi_GetMatchCnt()->clWaitTimeoutCount, DWC_MATCH_CMD_RETRY_MAX);
            if ((DWC_GetState() == DWC_STATE_MATCHING) &&
                (DWCi_GetMatchCnt()->clWaitTimeoutCount >= DWC_MATCH_CMD_RETRY_MAX)){
                // 5回送ってもサーバから返答がなかった場合はマッチメイクをやり直す。
                // 全てのクライアント処理を終了してマッチメイクを再開する。
                // サーバクライアントマッチメイクの新規接続クライアントの場合は
                // エラー終了する。
                // サーバクライアントマッチメイクの接続済みクライアントの場合は
                // 接続完了後のタイムアウト処理はゲーム側に委ねられる。
                DWC_Printf(DWC_REPORTFLAG_MATCH_GT2,
                           "Timeout: [SYN] Connection to server was shut down.\n");
                    
                if (!DWCi_CancelPreConnectedClientProcess(DWCi_GetMatchCnt()->sbPidList[0]))
                    return FALSE;
            }
            else {
                DWCi_GetMatchCnt()->clWaitTimeoutCount++;

                // サーバにSYN-ACKを再送する
                DWCi_SendMatchSynPacket(DWCi_GetMatchCnt()->aidList[0],
                                        DWC_SEND_TYPE_MATCH_SYN_ACK);

            }
        }
        break;

    case DWC_MATCH_STATE_SV_SYN:
        if (passTime > DWC_MATCH_SYN_ACK_WAIT_TIME){
            // SYN-ACK受信待ちタイムアウト時間経過
            DWCi_GetMatchCnt()->synResendCount++;
            if (DWCi_GetMatchCnt()->synResendCount > DWC_MATCH_CMD_RETRY_MAX){
                // 一定回数再送しても返事がなかった場合は、そのクライアントは
                // 通信を切断したと判断する
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                           "Timeout: wait SYN-ACK (aidbitmap 0x%x). Restart matching.\n",
                           DWCi_GetMatchCnt()->synAckBit);
                    
                if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY) ||
                    (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND)){
                    // 友達指定、無指定ピアマッチメイクでは、全てのコネクションを
                    // 切断し、マッチメイクを最初からやり直す
                    DWCi_CloseAllConnectionsByTimeout();
                    DWCi_RestartFromCancel(DWC_MATCH_RESET_RESTART);
                }
                else {
                    // サーバクライアントマッチメイクでは、応答無しの
                    // クライアントのみを切断し、他のクライアントにもそれを
                    // 伝えてそのクライアントを切断させる。
                    if (!DWCi_CloseShutdownClientSC(DWCi_GetMatchCnt()->synAckBit))
                        return FALSE;

                    if (DWCi_GetMatchCnt()->gt2NumConnection != 0){
                        // まだ接続しているクライアントが残っている場合は
                        // リトライカウントとSYNパケット送信時間を初期化し、
                        // 再び同期調整処理を続ける
                        DWCi_GetMatchCnt()->synResendCount = 0;
                        DWCi_GetMatchCnt()->lastSynSent    = OS_GetTick();
                    }
                    else {
                        // 接続済みクライアント全員がいなくなった場合は
                        // 全てのサーバ処理を終了してマッチメイク開始状態に戻る
                        if (!DWCi_CancelPreConnectedServerProcess(DWCi_GetMatchCnt()->cbEventPid))
                            return FALSE;
                    }
                }
            }
            else {
                // SYN-ACKを返していないクライアントにのみSYNパケットを再送する
                for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
                    if (!(DWCi_GetMatchCnt()->synAckBit & (1 << DWCi_GetMatchCnt()->aidList[i]))){
                        DWCi_SendMatchSynPacket(DWCi_GetMatchCnt()->aidList[i],
                                                DWC_SEND_TYPE_MATCH_SYN);
                    }
                }
            }
        }
        break;

    case DWC_MATCH_STATE_SV_SYN_WAIT:
        if (passTime > DWCi_GetMatchCnt()->baseLatency){
            // ACK送信から最大レイテンシ時間経過したらマッチメイク完了処理呼び出し
            DWCi_PostProcessConnection(DWC_PP_CONNECTION_SYN_FINISH);
        }
        break;
    }

    return TRUE;
}


/*---------------------------------------------------------------------------*
  サーバクライアントマッチメイクキャンセル同期調整コマンド送信関数
  引数　：profileID 送信先ホストのプロファイルID
          command   送信する同期調整コマンド
  戻り値：TRUE:送信成功、FALSE:エラー発生
  用途　：サーバクライアントマッチメイクキャンセル同期調整コマンドを送信する。
          エラー処理は内部で行っている。
 *---------------------------------------------------------------------------*/
static BOOL DWCi_SendCancelMatchSynCommand(int profileID, u8 command)
{
    u32 senddata;
    int len;
    int result;

    DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "Sent CANCEL SYN %d command to %u.\n",
               command-DWC_MATCH_COMMAND_CANCEL_SYN, profileID);

    if (command == DWC_MATCH_COMMAND_CANCEL_SYN){
        // キャンセルSYN送信時にキャンセルクライアントのプロファイルIDを送る
        senddata = (u32)DWCi_GetMatchCnt()->cbEventPid;
        len      = 1;
    }
    else {
        len      = 0;
    }

    result = DWCi_SendMatchCommand(command, profileID, 0, 0, &senddata, len);
    if (DWCi_HandleMatchCommandError(result)) return FALSE;

    // 送信時間を記録
    DWCi_GetMatchCnt()->lastCancelSynSent = OS_GetTick();

    return TRUE;
}


/*---------------------------------------------------------------------------*
  サーバクライアントマッチメイクキャンセル同期調整コマンド受信処理関数
  引数　：profileID 送信元ホストのプロファイルID
          command   受信した同期調整コマンド
          data      コマンドに付随したu32データ
  戻り値：TRUE:正常終了、FALSE:エラー発生
  用途　：サーバクライアントマッチメイクキャンセル同期調整コマンド受信時の
          処理を行う
 *---------------------------------------------------------------------------*/
static BOOL DWCi_ProcessCancelMatchSynCommand(int profileID, u8 command, u32 data)
{
    u8  aid;
    u32 bitmask;
    u64 baseTime, latency;
    int i;

    DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "Received CANCEL SYN %d command from %u.\n",
               command-DWC_MATCH_COMMAND_CANCEL_SYN, profileID);

    if (DWC_GetState() != DWC_STATE_CONNECTED){
        // 既にクローズした後に受信した場合は無視する
        DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "Ignore delayed CANCEL SYN.\n");
        return TRUE;
    }

    switch (command){
    case DWC_MATCH_COMMAND_CANCEL_SYN:
        if (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_CANCEL_SYN){
            // 最初の受信時ならマッチメイクキャンセル同期状態に進む
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_CANCEL_SYN);

            // 新規接続クライアントをクローズする。
            // dataに新規接続クライアントのプロファイルIDが入っている。
            DWCi_CloseCancelHostAsync((int)data);
        }

        // サーバにSYN-ACKパケットを送信する
        if (!DWCi_SendCancelMatchSynCommand(profileID, DWC_MATCH_COMMAND_CANCEL_SYN_ACK))
            return FALSE;
        break;

    case DWC_MATCH_COMMAND_CANCEL_SYN_ACK:
        if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_CANCEL_SYN){
            // SYN-ACK待ち状態の場合。
            // SYNを送ってから返ってくるまでの時間からおおよその片道の
            // レイテンシを求め、最大であれば記録する。
            // 再送直後に前のSYN-ACKが返ってきた場合は短めになるが、
            // 短くなる分には構わない。
            baseTime = OS_GetTick();
            if ((OS_TicksToMilliSeconds(baseTime-DWCi_GetMatchCnt()->lastCancelSynSent) >> 1) > DWC_GP_PROCESS_INTERVAL){
                // クライアント側でのコマンド受信からコマンド送信までの
                // 最大想定時間を引いておく
                latency = (OS_TicksToMilliSeconds(baseTime-DWCi_GetMatchCnt()->lastCancelSynSent) >> 1)-DWC_GP_PROCESS_INTERVAL;
                if (latency > DWCi_GetMatchCnt()->cancelBaseLatency){
                    DWCi_GetMatchCnt()->cancelBaseLatency = (u16)latency;
                }
            }
                
            // SYN-ACK受信Bitセット
            // AIDビットマップで記録する
            aid = DWCi_GetAIDFromProfileID(profileID, FALSE);
            if (aid != 0xff){
                DWCi_GetMatchCnt()->cancelSynAckBit |= 1 << aid;
            }

            bitmask = DWCi_GetAIDBitmask(TRUE);  // 判定用AIDビットマスクを取得
            if (DWCi_GetMatchCnt()->cancelSynAckBit == bitmask){
                // 全クライアントからSYN-ACKが返ってきたら、全クライアントに
                // ACKを送信する
                for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
                    if (!DWCi_SendCancelMatchSynCommand(DWCi_GetMatchCnt()->sbPidList[i],
                                                        DWC_MATCH_COMMAND_CANCEL_ACK))
                        return FALSE;
                }

                // マッチメイクキャンセル同期完了待ち状態に進む
                DWCi_SetMatchStatus(DWC_MATCH_STATE_SV_CANCEL_SYN_WAIT);

                DWC_Printf(DWC_REPORTFLAG_MATCH_GT2,
                           "Wait max latency %d msec.\n",
                           DWCi_GetMatchCnt()->cancelBaseLatency);
            }
        }
        else {
            // ACKを送信した後、個別に遅れてきたSYN-ACKに対応する場合
            // すぐにACKを返信する
            if (!DWCi_SendCancelMatchSynCommand(profileID, DWC_MATCH_COMMAND_CANCEL_ACK))
                return FALSE;
        }
        break;

    case DWC_MATCH_COMMAND_CANCEL_ACK:
        if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_CANCEL_SYN){            
            // クライアントマッチメイク再開
            DWCi_RestartFromCancel(DWC_MATCH_RESET_CONTINUE);
        }
        break;
    }

    return TRUE;
}


/*---------------------------------------------------------------------------*
  サーバクライアントマッチメイクキャンセル同期調整タイムアウト処理関数
  引数　：なし
  戻り値：TRUE:正常終了、FALSE:エラー発生
  用途　：サーバクライアントマッチメイクキャンセル同期調整のタイムアウトを
          チェックし、タイムアウトに達していたら再送処理を行う
 *---------------------------------------------------------------------------*/
static BOOL DWCi_ProcessCancelMatchSynTimeout(void)
{
    u64 passTime;
    int i;

    if ((DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_CANCEL_SYN) ||
        (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_CANCEL_SYN) ||
        (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_CANCEL_SYN_WAIT))
        passTime = OS_TicksToMilliSeconds(OS_GetTick()-DWCi_GetMatchCnt()->lastCancelSynSent);
    else return TRUE;

    switch (DWCi_GetMatchCnt()->state){
    case DWC_MATCH_STATE_CL_CANCEL_SYN:
        if (passTime > DWC_MATCH_CANCEL_SYN_ACK_WAIT_TIME){
            // サーバにキャンセルSYN-ACKを再送する
            if (!DWCi_SendCancelMatchSynCommand(DWCi_GetMatchCnt()->sbPidList[0],
                                                DWC_MATCH_COMMAND_CANCEL_SYN_ACK))
                return FALSE;
        }
        break;

    case DWC_MATCH_STATE_SV_CANCEL_SYN:
        if (passTime > DWC_MATCH_CANCEL_SYN_ACK_WAIT_TIME){
            // SYN-ACK受信待ちタイムアウト時間経過
            DWCi_GetMatchCnt()->cancelSynResendCount++;
            if (DWCi_GetMatchCnt()->cancelSynResendCount > DWC_MATCH_CMD_RETRY_MAX){
                // 一定回数再送しても返事がなかった場合は、そのクライアントは
                // 通信を切断したと判断する
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                           "Timeout: wait cancel SYN-ACK (aidbitmap 0x%x).\n",
                           DWCi_GetMatchCnt()->cancelSynAckBit);

                // サーバクライアントマッチメイクでは、応答無しの
                // クライアントのみを切断し、他のクライアントにもそれを
                // 伝えてそのクライアントを切断させる。
                if (!DWCi_CloseShutdownClientSC(DWCi_GetMatchCnt()->cancelSynAckBit))
                    return FALSE;

                if (DWCi_GetMatchCnt()->gt2NumConnection != 0){
                    // まだ接続しているクライアントが残っている場合は
                    // リトライカウントとSYNパケット送信時間を初期化し、
                    // 再び同期調整処理を続ける
                    DWCi_GetMatchCnt()->cancelSynResendCount = 0;
                    DWCi_GetMatchCnt()->lastCancelSynSent    = OS_GetTick();
                }
                else {
                    // 接続済みクライアント全員がいなくなった場合は
                    // 引き続きマッチメイク状態にする
                    DWCi_RestartFromCancel(DWC_MATCH_RESET_CONTINUE);
                }
            }
            else {
                // SYN-ACKを返していないクライアントにのみSYNパケットを再送する
                for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
                    if (!(DWCi_GetMatchCnt()->cancelSynAckBit & (1 << DWCi_GetMatchCnt()->aidList[i]))){
                        if (!DWCi_SendCancelMatchSynCommand(DWCi_GetMatchCnt()->sbPidList[i],
                                                            DWC_MATCH_COMMAND_CANCEL_SYN))
                            return FALSE;
                    }
                }
            }
        }
        break;

    case DWC_MATCH_STATE_SV_CANCEL_SYN_WAIT:
        if (passTime > DWCi_GetMatchCnt()->cancelBaseLatency){
            // キャンセルACK送信から最大レイテンシ時間経過したらマッチメイク再開
            DWCi_RestartFromCancel(DWC_MATCH_RESET_CONTINUE);
        }
        break;
    }

    return TRUE;
}


/*---------------------------------------------------------------------------*
  空きQR2キーID取得関数
  引数　：なし
  戻り値：利用可能なゲーム定義QR2キーID（DWC_QR2_GAME_KEY_START以降の値）
  用途　：QR2用キーデータ配列を検索し、利用可能なQR2キーIDを取得する
 *---------------------------------------------------------------------------*/
static u8  DWCi_GetNewMatchKey(void)
{
    u8 i;

    // 空いているキー番号を探す
    for (i = 0; i < DWC_QR2_GAME_RESERVED_KEYS; i++){
        if (!stGameMatchKeys[i].keyID) return (u8)(DWC_QR2_GAME_KEY_START+i);
    }

    // 既に登録キー数がいっぱい
    return 0;
}


/*---------------------------------------------------------------------------*
  ゲーム用QR2キークリア関数
  引数　：なし
  戻り値：なし
  用途　：QR2用キーデータ配列をクリアする
 *---------------------------------------------------------------------------*/
static void DWCi_ClearGameMatchKeys(void)
{
    int i;

    // まず全てのキーを調べてALLOC済み状態のものを解放する
    for (i = 0; i < DWC_QR2_GAME_RESERVED_KEYS; i++){
        if (stGameMatchKeys[i].keyStr)
            DWC_Free(DWC_ALLOCTYPE_BASE, stGameMatchKeys[i].keyStr, 0);
    }

    // 全てゼロクリアする
    MI_CpuClear32(stGameMatchKeys, sizeof(DWCGameMatchKeyData)*DWC_QR2_GAME_RESERVED_KEYS);
}

/*---------------------------------------------------------------------------*
  空きAID取得関数
  引数　：なし
  戻り値：利用可能な最小数値のAID
  用途　：AIDリストを検索し、利用可能なAIDの最小値を取得する
 *---------------------------------------------------------------------------*/
static u8   DWCi_GetAIDFromList(void)
{
    u8  i;
    int j;

    for (i = 0; i < DWC_MAX_CONNECTIONS; i++){
        for (j = 0; j <= DWCi_GetMatchCnt()->qr2NNFinishCount; j++){
            if (DWCi_GetMatchCnt()->aidList[j] == i) break;
        }
        if (j > DWCi_GetMatchCnt()->qr2NNFinishCount) break;
    }

    return i;
}


/*---------------------------------------------------------------------------*
  友達判定関数（友達インデックスリスト使用版）
  引数　：profileID プロファイルID
  戻り値：TRUE:インデックスリストにある友達である、FALSE:友達ではない
  用途　：ゲームから登録された友達インデックスリストと友達リストを参照し、
          指定のプロファイルIDが友達インデックスリストに登録された
          プロファイルIDかを調べる
 *---------------------------------------------------------------------------*/
static BOOL DWCi_IsFriendByIdxList(int profileID)
{
    int listProfileID;
    int i;

    if (DWCi_GetMatchCnt()->friendList == NULL) return FALSE;

    for (i = 0; i < DWCi_GetMatchCnt()->friendIdxListLen; i++){
        if (((listProfileID = DWCi_GetProfileIDFromList(DWCi_GetMatchCnt()->friendIdxList[i])) > 0) &&
            (listProfileID == profileID)){
            // 友達リストにプロファイルIDが存在していれば友達と判定する。
            // この関数の使われ方からして、gpIsBuddy()で調べる必要はない。
            return TRUE;
        }
    }

    return FALSE;
}


/*---------------------------------------------------------------------------*
  DWCライブラリ使用サーバチェック関数
  引数　：server SBServer型
  戻り値：serverが持つ dwc_pid キーの値（プロファイルID）を返す。
          DWCライブラリ使用サーバでなければ0を返す
  用途　：SBServerオブジェクトを調べ、DWCライブラリが設定するキーがあるか
          どうか判定し、あれば dwc_pid キーの値（プロファイルID）を返し、
          無ければ0を返す。
 *---------------------------------------------------------------------------*/
static int  DWCi_CheckDWCServer(SBServer server)
{

    if (SBServerGetIntValue(server, "numplayers", -1) == -1) return 0;
    if (SBServerGetIntValue(server, "maxplayers", -1) == -1) return 0;
    if (SBServerGetIntValue(server, DWC_QR2_MATCH_TYPE_KEY_STR, -1) == -1) return 0;
    if (SBServerGetIntValue(server, DWC_QR2_MATCH_RESV_KEY_STR, -1) == -1){
        if (SBServerGetIntValue(server, DWC_QR2_MATCH_RESV_KEY_STR, 0) == 0) return 0;
    }
    if (SBServerGetIntValue(server, DWC_QR2_MATCH_VER_KEY_STR, -1) == -1) return 0;
    return SBServerGetIntValue(server, DWC_QR2_PID_KEY_STR, 0);
}


/*---------------------------------------------------------------------------*
  sbPidListインデックス取得関数
  引数　：profileID インデックスを知りたいホストのプロファイルID
          idx0      TRUE:検索するインデックスに0を含める、FALSE:0は不定なので含めない
  戻り値：sbPidList等リストのインデックス。リストにないプロファイルIDであれば
          -1を返す
  用途　：指定プロファイルIDのホストの、sbPidListインデックスを取得する
 *---------------------------------------------------------------------------*/
static int  DWCi_GetPidListIndex(int profileID, BOOL idx0)
{
    int i;

    for (i = (idx0 ? 0 : 1); i <= DWCi_GetMatchCnt()->qr2NNFinishCount; i++){
        if (DWCi_GetMatchCnt()->sbPidList[i] == profileID) return i;
    }

    return -1;
}


/*---------------------------------------------------------------------------*
  プロファイルIDからAID取得関数
  引数　：profileID AIDを知りたいホストのプロファイルID
          idx0      TRUE:検索するインデックスに0を含める、FALSE:0は不定なので含めない
  戻り値：AID。対応するAIDがない場合は0xffを返す。
  用途　：指定プロファイルIDのホストの、AIDを取得する
 *---------------------------------------------------------------------------*/
static u8   DWCi_GetAIDFromProfileID(int profileID, BOOL idx0)
{
    int i;

    for (i = (idx0 ? 0 : 1); i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
        if (DWCi_GetMatchCnt()->sbPidList[i] == profileID)
            return DWCi_GetMatchCnt()->aidList[i];
    }

    return 0xff;
}


/*---------------------------------------------------------------------------*
  同期調整時判定用AIDビットマップ取得関数
  引数　：valid TRUE: 接続完了済みのホストのみのビットマップ
                     （サーバクライアントマッチメイクキャンセル同期で使用）
                FALSE:マッチメイク中も含めた接続ホストのビットマップ
                      （マッチメイク完了同期に使用）
  戻り値：AIDビットマップ
  用途　：各種同期調整時に、SYN-ACK受信ホストを調べる為のビットマップを取得する
 *---------------------------------------------------------------------------*/
static u32  DWCi_GetAIDBitmask(BOOL valid)
{
    u32 bitmask = 0;
    int i;

    if (valid){
        // 既に接続完了済みのAIDから、サーバのaid（=0）を消したビットマスク
        return (DWCi_GetMatchCnt()->validAidBitmap & ~1);
    }
    else {
        // マッチメイク中のホストも含めた接続済みのAIDから、サーバのaid（=0）を
        // 消したビットマスク
        for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
            bitmask |= 1 << DWCi_GetMatchCnt()->aidList[i];
        }
        return bitmask;
    }
}


/*---------------------------------------------------------------------------*
  接続済みクライアントタイムアウトカウンタ初期化関数
  引数　：timeout タイムアウトと判定するサーバとの無通信時間
  戻り値：なし
  用途　：接続済みクライアントが、サーバからのデータ受信がない時間を計測する
          ための最終データ受信時間と、タイムアウト発生回数カウンタを初期化する
 *---------------------------------------------------------------------------*/
static void DWCi_InitClWaitTimeout(void)
{

    DWCi_GetMatchCnt()->clWaitTimeoutCount = 0;
    DWCi_GetMatchCnt()->clWaitTime         = OS_GetTick();
}


/*---------------------------------------------------------------------------*
  マッチメイクオプション　指定人数以下マッチメイク完了カウンタ初期化関数
  引数　：reset TRUE:サーバになった時のリセット、FALSE:完全初期化
  戻り値：なし
  用途　：指定人数以下マッチメイク完了のマッチメイクオプションの、
          タイムアウト計測用カウンタ初期化関数
 *---------------------------------------------------------------------------*/
static void DWCi_InitOptMinCompParam(BOOL reset)
{

    if (stpOptMinComp && stpOptMinComp->valid){
        stpOptMinComp->recvBit      = 0;
        stpOptMinComp->timeoutBit   = 0;
        stpOptMinComp->retry        = 0;
        stpOptMinComp->lastPollTime = OS_GetTick();
        if (!reset) stpOptMinComp->startTime = OS_GetTick();
    }
}


/*---------------------------------------------------------------------------*
  マッチメイクオプション　指定人数以下マッチメイク完了毎フレーム処理関数
  引数　：なし
  戻り値：なし
  用途　：指定人数以下マッチメイク完了のマッチメイクオプションの、タイムアウト
          チェック、及びクライアントへのポーリング処理を行う
 *---------------------------------------------------------------------------*/
static void DWCi_ProcessOptMinComp(void)
{
    u32 bitmask;
    int result;
    int i;

    if (!stpOptMinComp || !stpOptMinComp->valid ||
        (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV) ||
        (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL))
        return;

    if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_POLL_TIMEOUT){
        bitmask = DWCi_GetAIDBitmask(FALSE);  // 判定用AIDビットマスクを取得
        if (stpOptMinComp->recvBit == bitmask){
            // 全員から返信があった場合
            if (stpOptMinComp->timeoutBit == bitmask){
                // クライアントも全員タイムアウト時間を越えて待っていた場合
                // マッチメイク完了処理を行う。
                DWC_Printf(DWC_REPORTFLAG_MATCH_GT2,
                           "[OPT_MIN_COMP] Timeout occured in all hosts.\n");
                    
                // 元々の接続希望人数を、現在の接続人数に合わせて変更する。
                DWCi_GetMatchCnt()->qr2NumEntry = (u8)DWCi_GetMatchCnt()->gt2NumConnection;
                // 終了処理に進む為のパラメータ調整
                DWCi_GetMatchCnt()->clLinkProgress = (u8)(DWCi_GetMatchCnt()->gt2NumConnection-1);

                // マッチメイク終了処理呼び出し
                DWCi_PostProcessConnection(DWC_PP_CONNECTION_SV_CONNECT);
            }
            else {
                // まだタイムアウトに達していないクライアントがいた場合
                DWC_Printf(DWC_REPORTFLAG_MATCH_GT2,
                           "[OPT_MIN_COMP] Some clients is in time.\n");

                // 再送間隔の計測を開始する
                stpOptMinComp->lastPollTime = OS_GetTick();
                stpOptMinComp->recvBit      = 0;  // 受信ビットクリア

                if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY){
                    // 友達無指定ピアマッチメイクの場合は、空きホストの検索状態に戻る
                    DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SEARCH_HOST);
//                    DWCi_GetMatchCnt()->sbUpdateCount = 1;
                    DWCi_GetMatchCnt()->sbUpdateFlag = DWC_SB_UPDATE_INTERVAL_LONG;
                    DWCi_GetMatchCnt()->sbUpdateTick = OS_GetTick();
                }
                else {
                    // 友達指定ピアマッチメイクの場合は、一定フレーム後に
                    // NN予約要求送信を行う
                    DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAIT_RESV);
                    (void)DWCi_SendResvCommandToFriend(TRUE, FALSE, 0);
                }
            }
        }
        else if (OS_TicksToMilliSeconds(OS_GetTick()-stpOptMinComp->lastPollTime) >= DWC_MATCH_CMD_RTT_TIMEOUT*stpOptMinComp->retry){
            // タイムアウト時間が過ぎてもクライアントからの返信が揃わない場合
            DWC_Printf(DWC_REPORTFLAG_DEBUG,
                       "[OPT_MIN_COMP] Timeout: wait poll-ACK %d/%d.\n",
                       stpOptMinComp->retry-1, DWC_MATCH_OPT_MIN_COMP_POLL_RETRY_MAX);
            
            if (stpOptMinComp->retry > DWC_MATCH_OPT_MIN_COMP_POLL_RETRY_MAX){
                // 最大リトライ回数ポーリングしてもクライアントからの反応がない
                // 場合は、そのクライアントは切断されたものと判断し、
                // マッチメイクをやり直す
                DWC_Printf(DWC_REPORTFLAG_DEBUG, 
                           "[OPT_MIN_COMP] Timeout: aidbitmap 0x%x. Restart matching.\n",
                           stpOptMinComp->recvBit);
                
                // 制御パラメータ初期化
                DWCi_InitOptMinCompParam(TRUE);

                // 全てのコネクションを切断し、マッチメイクを最初からやり直す
                DWCi_CloseAllConnectionsByTimeout();
                DWCi_RestartFromCancel(DWC_MATCH_RESET_RESTART);
            }
            else {
                // まだ返信していないクライアントにコマンドを再送する
                for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
                    if (!(stpOptMinComp->recvBit & (1 << DWCi_GetMatchCnt()->aidList[i]))){
                        result =
                            DWCi_SendMatchCommand(DWC_MATCH_COMMAND_POLL_TIMEOUT,
                                                  DWCi_GetMatchCnt()->sbPidList[i],
                                                  DWCi_GetMatchCnt()->qr2IPList[i],
                                                  DWCi_GetMatchCnt()->qr2PortList[i],
                                                  NULL, 0);
                        if (DWCi_HandleMatchCommandError(result)) return;
                    }
                }
                stpOptMinComp->retry++;
            }
        }
    }
    else if (((DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_WAIT_RESV) ||
              (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_SEARCH_HOST)) &&
             (DWCi_GetMatchCnt()->gt2NumConnection >= stpOptMinComp->minEntry-1) &&
             ((!stpOptMinComp->retry &&
               (OS_TicksToMilliSeconds(OS_GetTick()-stpOptMinComp->startTime) >= stpOptMinComp->timeout)) ||
              (stpOptMinComp->retry &&
               (OS_TicksToMilliSeconds(OS_GetTick()-stpOptMinComp->lastPollTime) >= stpOptMinComp->timeout >> 2)))){
        // サーバがタイムアウト時間を越えており、最低マッチメイク
        // 完了人数が接続されていたら、クライアントもタイムアウト時間を
        // 越えていないかをポーリングする。
        // ２度目以降のポーリングは、タイムアウト時間の1/4の時間間隔で
        // 定期的に行う。

        // まずは予約送信済みなら予約をキャンセルする
        if (DWCi_GetMatchCnt()->reqProfileID){
            result = DWCi_CancelReservation(DWCi_GetMatchCnt()->reqProfileID);
            if (DWCi_HandleMatchCommandError(result)) return;
        }

        // ポーリング状態に進む
        DWCi_SetMatchStatus(DWC_MATCH_STATE_SV_POLL_TIMEOUT);

        DWC_Printf(DWC_REPORTFLAG_MATCH_GT2,
                   "[OPT_MIN_COMP] Poll timeout (my time is %lu).\n",
                   OS_TicksToMilliSeconds(OS_GetTick()-stpOptMinComp->startTime));

        // ポーリングパケットを送信する
        for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
            result =
                DWCi_SendMatchCommand(DWC_MATCH_COMMAND_POLL_TIMEOUT,
                                      DWCi_GetMatchCnt()->sbPidList[i],
                                      DWCi_GetMatchCnt()->qr2IPList[i],
                                      DWCi_GetMatchCnt()->qr2PortList[i],
                                      NULL, 0);
            if (DWCi_HandleMatchCommandError(result)) return;
        }

        // 全てのクライアントからのデータが帰ってくるまでの時間を
        // カウントするためにlastPollTimeを現在時間で上書きする
        stpOptMinComp->lastPollTime = OS_GetTick();
        stpOptMinComp->retry        = 1;  // １回送信したことを記録
    }
}


/*---------------------------------------------------------------------------*
  GPResult対応エラー処理関数
  引数　：result GPの処理結果型
  戻り値：GPの処理結果型（引数をそのまま返す）
  用途　：GPResultの値に対応するエラー表示（もしくは停止）を行いつつ、
          対応するDWCエラーを渡して、対応するコールバックを呼び出す
 *---------------------------------------------------------------------------*/
static GPResult DWCi_HandleGPError(GPResult result)
{
    int errorCode;
    DWCError dwcError;

    if (result == GP_NO_ERROR) return GP_NO_ERROR;

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Match, GP error %d\n", result);

    switch (result){
    case GP_MEMORY_ERROR:
        dwcError  = DWC_ERROR_FATAL;
        errorCode = DWC_ECODE_TYPE_ALLOC;
        break;
    case GP_PARAMETER_ERROR:
        dwcError  = DWC_ERROR_FATAL;
        errorCode = DWC_ECODE_TYPE_PARAM;
        break;
    case GP_NETWORK_ERROR:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_NETWORK;
        break;
    case GP_SERVER_ERROR:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_SERVER;
        break;
    }

    // ベースエラーコードを追加
    errorCode += DWC_ECODE_SEQ_MATCH+DWC_ECODE_GS_GP;

    // マッチメイク中断関数を呼び出す
    DWCi_StopMatching(dwcError, errorCode);

    return result;
}


/*---------------------------------------------------------------------------*
  SBError対応エラー処理関数
  引数　：error SBのエラー型
  戻り値：SBのエラー型（引数をそのまま返す）
  用途　：SBErrorの値に対応するエラー表示（もしくは停止）を行いつつ、
          対応するDWCエラーを渡してマッチメイク手続きを終了する
 *---------------------------------------------------------------------------*/
static SBError DWCi_HandleSBError(SBError error)
{
    int errorCode;
    DWCError dwcError;

    if (error == sbe_noerror) return sbe_noerror;

#if 0
    // [sasakit] ignore connecterror
    if (error == sbe_connecterror)
    {
        DWCi_connect_errorcount++;
        DWC_Printf(DWC_REPORTFLAG_ERROR, "********************************%d\n", DWCi_connect_errorcount);
        return sbe_noerror;
    }
#endif

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Match, SB error %d\n", error);

    switch (error){
    case sbe_socketerror:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_SOCKET;
        break;
    case sbe_dnserror:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_DNS;
        break;
    case sbe_connecterror:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_SERVER;
        break;
    case sbe_dataerror:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_DATA;
        break;
    case sbe_allocerror:
        dwcError  = DWC_ERROR_FATAL;
        errorCode = DWC_ECODE_TYPE_ALLOC;
        break;
    case sbe_paramerror:
        dwcError  = DWC_ERROR_FATAL;
        errorCode = DWC_ECODE_TYPE_PARAM;
        break;
    }

    // ベースエラーコードを追加
    errorCode += DWC_ECODE_SEQ_MATCH+DWC_ECODE_GS_SB;

    DWCi_StopMatching(dwcError, errorCode);
    
    return error;
}


/*---------------------------------------------------------------------------*
  qr2_error_t対応エラー処理関数
  引数　：error QR2のエラー型
  戻り値：QR2のエラー型（引数をそのまま返す）
  用途　：qr2_error_tの値に対応するエラー表示（もしくは停止）を行いつつ、
          対応するDWCエラーを渡してマッチメイク手続きを終了する
 *---------------------------------------------------------------------------*/
static qr2_error_t DWCi_HandleQR2Error(qr2_error_t error)
{
    int errorCode;
    DWCError dwcError;

    if (error == e_qrnoerror) return e_qrnoerror;

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Match, QR2 error %d\n", error);

    switch (error){
    case e_qrwsockerror:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_SOCKET;
        break;
    case e_qrbinderror:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_BIND;
        break;
    case e_qrdnserror:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_DNS;
        break;
    case e_qrconnerror:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_PEER;
        break;
    case e_qrnochallengeerror:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_SERVER;
        break;
    }

    // ベースエラーコードを追加して各種処理中の固有エラー処理を呼び出す
    switch (DWC_GetState()){
    case DWC_STATE_LOGIN:
        errorCode += DWC_ECODE_SEQ_LOGIN+DWC_ECODE_GS_QR2;
        DWCi_StopLogin(dwcError, errorCode);     // ログイン処理終了
        break;
    case DWC_STATE_UPDATE_SERVERS:
        errorCode += DWC_ECODE_SEQ_FRIEND+DWC_ECODE_GS_QR2;
        DWCi_StopFriendProcess(dwcError, errorCode);  // 友達管理処理終了
        break;
    case DWC_STATE_MATCHING:
        // マッチメイク時にエラーになった場合
        // サーバクライアントマッチメイクで既にSTATE_CONNECTEDになっている
        // 状態でのエラー時は、別にマッチメイクコールバックを返さなくても良い
        errorCode += DWC_ECODE_SEQ_MATCH+DWC_ECODE_GS_QR2;
        DWCi_StopMatching(dwcError, errorCode);  // マッチメイク処理終了
        break;
    default:
        errorCode += DWC_ECODE_SEQ_ETC+DWC_ECODE_GS_QR2;
        DWCi_SetError(dwcError, errorCode);
        break;
    }
    
    return error;
}


/*---------------------------------------------------------------------------*
  NegotiateError対応エラー処理関数
  引数　：error NNのエラー型
  戻り値：NNのエラー型（引数をそのまま返す）
  用途　：NegotiateErrorの値に対応するエラー表示（もしくは停止）を行いつつ、
          対応するDWCエラーを渡してマッチメイク手続きを終了する
 *---------------------------------------------------------------------------*/
static NegotiateError DWCi_HandleNNError(NegotiateError error)
{
    int errorCode;
    DWCError dwcError;

    if (error == ne_noerror) return ne_noerror;

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Match, NN error %d\n", error);

    switch (error){
    case ne_allocerror:
        dwcError  = DWC_ERROR_FATAL;
        errorCode = DWC_ECODE_TYPE_ALLOC;
        break;
    case ne_socketerror:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_SOCKET;
        break;
    case ne_dnserror:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_DNS;
        break;
    }

    // ベースエラーコードを追加
    errorCode += DWC_ECODE_SEQ_MATCH+DWC_ECODE_GS_NN;

    DWCi_StopMatching(dwcError, errorCode);
    
    return error;
}


/*---------------------------------------------------------------------------*
  NegotiateResult対応エラー処理関数
  引数　：result NNの結果型
  戻り値：NNの結果型（引数をそのまま返す）
  用途　：NegotiateResultの値に対応するエラー表示（もしくは停止）を行いつつ、
          対応するDWCエラーを渡してマッチメイク手続きを終了する
 *---------------------------------------------------------------------------*/
static NegotiateResult DWCi_HandleNNResult(NegotiateResult result)
{
    int errorCode;
    DWCError dwcError;

    if (result == nr_success) return nr_success;

    DWC_Printf(DWC_REPORTFLAG_WARNING, "Match, NN result %d\n", result);

    switch (result){
    case nr_deadbeatpartner:
        return nr_deadbeatpartner;
        break;
    case nr_inittimeout:
        return nr_inittimeout;
        break;
    default:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_UNEXPECTED;
        break;
    }

    if (dwcError){
        // ベースエラーコードを追加
        errorCode += DWC_ECODE_SEQ_MATCH+DWC_ECODE_GS_NN;
        DWCi_StopMatching(dwcError, errorCode);
    }
    
    return result;
}


/*---------------------------------------------------------------------------*
  GT2Result対応エラー処理関数
  引数　：result GT2の処理結果型
  戻り値：GT2の処理結果型（引数をそのまま返す）
  用途　：GT2Resultの値に対応するエラー表示（もしくは停止）を行いつつ、
          対応するDWCエラーを渡して、対応するコールバックを呼び出す
 *---------------------------------------------------------------------------*/
// [note]
// gt2Connectでしか使われていない
static GT2Result DWCi_HandleGT2Error(GT2Result result)
{
    int errorCode;
    DWCError dwcError;

    if (result == GT2Success) return GT2Success;

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Match, GT2 error %d\n", result);

    switch (result){
    case GT2OutOfMemory:
        dwcError  = DWC_ERROR_FATAL;
        errorCode = DWC_ECODE_TYPE_ALLOC;
        break;
    case GT2Rejected:
    case GT2DuplicateAddress:
        dwcError  = DWC_ERROR_NONE;
        errorCode = 0;
        result    = GT2Success;
        break;
    case GT2NetworkError:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_NETWORK;
        break;
    case GT2AddressError:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_DNS;
        break;
    case GT2TimedOut:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_TIMEOUT;
        break;
    case GT2NegotiationError:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_PEER;
        break;
    }

    if (dwcError){
        // ベースエラーコードを追加
        errorCode += DWC_ECODE_SEQ_MATCH+DWC_ECODE_GS_GT2;
        DWCi_StopMatching(dwcError, errorCode);
    }

    return result;
}


/*---------------------------------------------------------------------------*
  SBコールバック関数
  引数　：sb       SBオブジェクト
          reason   コールバック呼び出し理由
          server   更新されたサーバ構造体
          instance コールバック用パラメータ
  戻り値：なし
  用途　：SBコールバック
 *---------------------------------------------------------------------------*/
static void DWCi_SBCallback(ServerBrowser sb, SBCallbackReason reason, SBServer server, void *instance)
{
#pragma unused(instance)
    int profileID;
    int result;
    int i;
    NegotiateError nnError;

    DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
               "SBCallback : reason %d (state = %d)\n",
               reason, DWCi_GetMatchCnt()->state);

    // コールバックの呼び出しレベルをカウント
    s_sbCallbackLevel++;

    // ServerBrowserLimitUpdateのタイムアウトの更新
    if(DWCi_GetMatchCnt()->sbUpdateRequestTick != 0)
    {
        if(reason == sbc_updatecomplete || reason == sbc_queryerror)
        {
            // 終了
            DWCi_GetMatchCnt()->sbUpdateRequestTick = 0;
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "ServerBrowserLimitUpdate timeout reset.(%s)\n", __FUNCTION__);
        }
        else
        {
            // 継続のためにタイムアウトを延長
            DWCi_GetMatchCnt()->sbUpdateRequestTick = OS_GetTick() + OS_MilliSecondsToTicks(DWC_SB_UPDATE_TIMEOUT);
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "ServerBrowserLimitUpdate timeout update.(%s)\n", __FUNCTION__);
        }
    }

    switch (reason){
    case sbc_serveradded:     // サーバ情報追加
        DWCi_SBPrintServerData(server);  // サーバデータテスト表示
        break;
        
    case sbc_updatecomplete:  // サーバ情報全てアップデート完了
        // サーバリストを調べて成りすましサーバをリストから消しておく。
        // これ以降、正しいDWCサーバのデータだけが存在するものとして
        // 処理を行う。完全な成りすましサーバがあった場合は他のエラーに
        // 引っ掛かることを期待する。
        for (i = 0; i < ServerBrowserCount(sb); i++){
            server = ServerBrowserGetServer(sb, i);

            // serverがDWCライブラリを使用しているかをチェックして
            // プロファイルIDを取得
            if (!DWCi_CheckDWCServer(server)){
                // 有効なプロファイルIDを持たないサーバはリストから削除する
                ServerBrowserRemoveServer(sb, server);

                DWC_Printf(DWC_REPORTFLAG_SB_UPDATE,
                           "Deleted server [%d].\n", i);
                i--;
            }
        }

        switch (DWCi_GetMatchCnt()->state){
        case DWC_MATCH_STATE_CL_SEARCH_OWN:
            // 友達無指定ピアマッチメイクの自ホスト検索時
            for (i = 0; i < ServerBrowserCount(sb); i++){
                server = ServerBrowserGetServer(sb, i);

                if (DWCi_GetMatchCnt()->qr2IP &&
                    (DWCi_GetMatchCnt()->qr2IP == SBServerGetPublicInetAddress(server)) &&
                    DWCi_GetMatchCnt()->qr2Port &&
                    (DWCi_GetMatchCnt()->qr2Port == SBServerGetPublicQueryPort(server)))
                    break;
            }

            if (i < ServerBrowserCount(sb)){
                SBError sbError;
                // 既にQR2アドレスコールバックで自分の現在のIP、ポート番号が
                // 分かっており、かつそれがマスターサーバにアップされている場合
                DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SEARCH_HOST);
                DWCi_GetMatchCnt()->reqProfileID = 0;  // 予約送信先クリア

                // 次の検索を開始。
                sbError = DWCi_SBUpdateAsync(DWCi_GetMatchCnt()->reqProfileID);
                if (DWCi_HandleSBError(sbError)) goto on_exit;
            }
            else
            {
                // まだ自分の情報がマスターサーバにアップされていない場合は
                // 同じ条件で再びSBサーバアップデートを予約しておく

//            DWCi_GetMatchCnt()->sbUpdateCount = 1;  // SBサーバアップデート予約
                DWCi_GetMatchCnt()->sbUpdateFlag = DWC_SB_UPDATE_INTERVAL_LONG;
                DWCi_GetMatchCnt()->sbUpdateTick = OS_GetTick();
            }
            break;
            
        case DWC_MATCH_STATE_CL_SEARCH_HOST:
            // 友達無指定ピアマッチメイクの空きホスト検索時。
            // サーバから切断され、新しいサーバを紹介されてプロファイルIDで
            // 検索した時もここに来る。
            (void)DWCi_EvaluateServers(TRUE);  // 各サーバを評価する
            DWCi_RandomizeServers();           // 更にランダム要素を加える

            if (ServerBrowserCount(sb)){
                // 定員割れホストが見つかったら、予約要求コマンド送信
                // 指定のサーバの情報をセットし、そこに予約コマンドを送信する
                result = DWCi_SendResvCommand(0, FALSE); 
                if (DWCi_HandleMatchCommandError(result)) goto on_exit; 

                // 予約確認状態へ移行
                DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAIT_RESV);

//                DWCi_GetMatchCnt()->sbUpdateCount = 0;
                DWCi_GetMatchCnt()->sbUpdateFlag = 0;
            }
            else {
                // 1つもサーバが見つからなければ再びSBサーバアップデートを予約する
//                DWCi_GetMatchCnt()->sbUpdateCount = 1;
                DWCi_GetMatchCnt()->sbUpdateFlag = DWC_SB_UPDATE_INTERVAL_LONG;
                DWCi_GetMatchCnt()->sbUpdateTick = OS_GetTick();
            }
            break;
            
        case DWC_MATCH_STATE_CL_SEARCH_NN_HOST:
            // NATネゴシエーションの相手ホスト検索時
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "searchIP: %x, searchPort: %d\n",
                       DWCi_GetMatchCnt()->searchIP, DWCi_GetMatchCnt()->searchPort);

            // 最新の情報でなければサーバリストから消す
            while (ServerBrowserCount(sb)){
                server = ServerBrowserGetServer(sb, 0);
                if ((SBServerGetPublicInetAddress(server) == DWCi_GetMatchCnt()->searchIP) &&
                    (SBServerGetPublicQueryPort(server) == DWCi_GetMatchCnt()->searchPort)){
                    break;
                }
                else {
                    ServerBrowserRemoveServer(sb, server);
                }
            }
                
            if (ServerBrowserCount(sb)){
                // サーバが見つかった場合
                profileID =
                    SBServerGetIntValue(ServerBrowserGetServer(sb, 0),
                                        DWC_QR2_PID_KEY_STR, 0);
                if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND) &&
                    (profileID == DWCi_GetMatchCnt()->sbPidList[0])){
                    // 友達指定ピアマッチメイクでは、サーバ候補を見つけた
                    // 時のみ、そのサーバの評価を行う
                    if (DWCi_EvaluateServers(FALSE)){
                        if (DWCi_GetMatchCnt()->gt2NumConnection){
                            // 自分に接続済みホストがあったら全てコネクション
                            // クローズして完全な１クライアントに戻る
                            result = DWCi_ChangeToClient();
                            if (DWCi_HandleMatchCommandError(result)) goto on_exit;

                            // コネクションのクローズを待つため、以下の処理は飛ばす
                        }
                    }
                    else {
                        // 評価の結果サーバが削除されたら予約をキャンセルする
                        result = DWCi_CancelReservation(DWCi_GetMatchCnt()->sbPidList[0]);
                        if (DWCi_HandleMatchCommandError(result)) goto on_exit;

                        // 状態を予約待ちに戻す
                        DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAIT_RESV);

                        // 次の友達にNN予約要求を送信する
                        result = DWCi_SendResvCommandToFriend(FALSE, FALSE, DWCi_GetMatchCnt()->sbPidList[0]);
                        if (DWCi_HandleMatchCommandError(result)) goto on_exit;
                        break;  // 以下の処理は飛ばす
                    }
                }
                
                // NATネゴシエーション状態をセット
                DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_NN);
                
                // サーバに対してNATネゴシエーションを開始する
                server = ServerBrowserGetServer(sb, 0);
                nnError = DWCi_NNStartupAsync(0, 0, server);
                if (DWCi_HandleNNError(nnError)) goto on_exit;
            }
            else {
                // サーバが見つからなかった場合、マスターサーバに
                // データがアップされるまでの待ち時間だと思われるので
                // 再びSBサーバアップデートを行う
//                DWCi_GetMatchCnt()->sbUpdateCount = 1;  // SBサーバアップデートカウント開始
                DWCi_GetMatchCnt()->sbUpdateFlag = DWC_SB_UPDATE_INTERVAL_LONG;
                DWCi_GetMatchCnt()->sbUpdateTick = OS_GetTick();
            }
            break;

        default:
            // 友達無指定ピアマッチメイク時に、空きホスト検索中に他ホストからの
            // 予約を受け付けた場合は、state = DWC_MATCH_STATE_SV_OWN_NNの
            // 状態でここに来ることがあるが、特に何もしない。
            break;
        }
        break;
        
    case sbc_queryerror:      // クエリ文字列エラー
        break;
        
    default:
        // アップデート１回ごとにサーバリストをクリアするので、
        // reason が sbc_serverupdated, sbc_serverupdatefailedになることは
        // ないはず。sbc_serverdeleted はある
        break;
    }

on_exit:

    // コールバックの呼び出しレベルをカウント
    s_sbCallbackLevel--;

    return;

}


/*---------------------------------------------------------------------------*
  SBサーバデータ表示テスト関数
  引数　：server 更新されたサーバ構造体
  戻り値：なし
  用途　：SBサーバデータをコンソールに表示する
 *---------------------------------------------------------------------------*/
static void DWCi_SBPrintServerData(SBServer server)
{
#if defined(SDK_FINALROM)
#pragma unused(server)
#endif

    int i;

    // サーバの情報を表示する
    DWC_Printf(DWC_REPORTFLAG_SB_UPDATE,
               "SBServerGetPrivateAddress     = %s\n",
               SBServerGetPrivateAddress(server));
  	DWC_Printf(DWC_REPORTFLAG_SB_UPDATE,
               "SBServerGetPrivateInetAddress = %x\n",
               SBServerGetPrivateInetAddress(server));
  	DWC_Printf(DWC_REPORTFLAG_SB_UPDATE,
               "SBServerGetPrivateQueryPort   = %d\n",
               SBServerGetPrivateQueryPort(server));
  	DWC_Printf(DWC_REPORTFLAG_SB_UPDATE,
               "SBServerGetPublicAddress      = %s\n",
               SBServerGetPublicAddress(server));
   	DWC_Printf(DWC_REPORTFLAG_SB_UPDATE,
               "SBServerGetPublicInetAddres   = %x\n",
               SBServerGetPublicInetAddress(server));
  	DWC_Printf(DWC_REPORTFLAG_SB_UPDATE,
               "SBServerGetPublicQueryPort    = %d\n",
               SBServerGetPublicQueryPort(server));
   	DWC_Printf(DWC_REPORTFLAG_SB_UPDATE,
               "SBServerHasPrivateAddress     = %d\n",
               SBServerHasPrivateAddress(server));

    // DWCで使用しているキーデータを表示する
    DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "numplayers  = %d\n",
               SBServerGetIntValue(server, "numplayers", -1));
    DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "maxplayers  = %d\n",
               SBServerGetIntValue(server, "maxplayers", -1));
    DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "%s     = %u\n",
               DWC_QR2_PID_KEY_STR,
               SBServerGetIntValue(server, DWC_QR2_PID_KEY_STR, 0));
    DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "%s   = %u\n",
               DWC_QR2_MATCH_RESV_KEY_STR,
               SBServerGetIntValue(server, DWC_QR2_MATCH_RESV_KEY_STR, -1));
    DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "%s   = %d\n",
               DWC_QR2_MATCH_TYPE_KEY_STR,
               SBServerGetIntValue(server, DWC_QR2_MATCH_TYPE_KEY_STR, -1));
    DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "%s   = %d\n",
               DWC_QR2_MATCH_VER_KEY_STR,
               SBServerGetIntValue(server, DWC_QR2_MATCH_VER_KEY_STR, -1));

    // ゲーム定義のキーデータを表示する
    for (i = 0; i < DWC_QR2_GAME_RESERVED_KEYS; i++){
        if (stGameMatchKeys[i].keyID){
            if (stGameMatchKeys[i].isStr){
                DWC_Printf(DWC_REPORTFLAG_SB_UPDATE,
                           "%s  = %s\n",
                           stGameMatchKeys[i].keyStr,
                           SBServerGetStringValue(server,
                                                  stGameMatchKeys[i].keyStr,
                                                  "NONE"));
            }
            else {
                DWC_Printf(DWC_REPORTFLAG_SB_UPDATE,
                           "%s  = %d\n",
                           stGameMatchKeys[i].keyStr,
                           SBServerGetIntValue(server,
                                               stGameMatchKeys[i].keyStr,
                                               -1));
            }
        }
    }
}


/*---------------------------------------------------------------------------*
  サーバ評価関数
  引数　：sort TRUE:サーバリストを評価値順にソートする、FALSE:ソートしない
  戻り値：TRUE:通常、FALSE:サーバデータを削除してサーバリストが空になった
  用途　：サーバ評価関数
 *---------------------------------------------------------------------------*/
static BOOL DWCi_EvaluateServers(BOOL sort)
{
    int eval;
    int deleteFlag = 0;
    int localDelete;
    int profileID;
    int i, j;
    SBServer server;

    // 各サーバの評価を行う
    for (i = 0; i < ServerBrowserCount(DWCi_GetMatchCnt()->sbObj); i++){
        server = ServerBrowserGetServer(DWCi_GetMatchCnt()->sbObj, i);

        // 友達無指定ピアマッチメイクの場合は、既に自分に繋がったクライアントの
        // 古いサーバデータを参照しないようにする
        if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY){
            profileID   = SBServerGetIntValue(server, DWC_QR2_PID_KEY_STR, 0);
            localDelete = 0;
            for (j = 1; j <= DWCi_GetMatchCnt()->gt2NumConnection; j++){
                if (profileID == DWCi_GetMatchCnt()->sbPidList[j]){
                    // 既に接続済みのサーバなら消去
                    ServerBrowserRemoveServer(DWCi_GetMatchCnt()->sbObj, server);

                    i--;
                    localDelete = 1;
                    break;
                }
            }

            if (localDelete) continue;
        }

        if (DWCi_GetMatchCnt()->evalCallback){
            eval = DWCi_GetMatchCnt()->evalCallback(i, DWCi_GetMatchCnt()->evalParam);
            
            if (eval > 0){
                // 評価値が0より大きければ評価値をサーバリストに書き込む。
                // 同じ値でも必ず差を付けるためにシフトして下位にランダム値を
                // セットする。
                if ( eval > 0x007fffff ) eval = 0x007fffff;
                SBServerAddIntKeyValue(server,
                                       DWC_QR2_MATCH_EVAL_KEY_STR,
                                       (int)((eval << 8) | DWCi_GetMathRand32(256)));
            }
            else {
                // 評価値が0以下ならマッチメイクの対象から外す
                ServerBrowserRemoveServer(DWCi_GetMatchCnt()->sbObj, server);

                DWC_Printf(DWC_REPORTFLAG_SB_UPDATE,
                           "Deleted server [%d] (eval point is %d).\n",
                           i, eval);

                i--;
                deleteFlag = 1;
            }
        }
        else {
            // サーバ評価が行われない場合も検索の度にサーバの並びを
            // 変えたいので、ランダムな評価値をセットする
            // （この時は評価はしていないので、評価値は0でも良い）
            SBServerAddIntKeyValue(server, DWC_QR2_MATCH_EVAL_KEY_STR,
                                   (int)DWCi_GetMathRand32(DWC_MAX_CONNECTIONS << 2));
        }
    }

    //if (!DWCi_GetMatchCnt()->evalCallback) return TRUE;

    if (sort && ServerBrowserCount(DWCi_GetMatchCnt()->sbObj)){
        // サーバリストのソートが必要ならソートする。
        // 評価コールバックが設定されていなくても予約先をランダムにしたいので
        // ソートは必要。
        ServerBrowserSort(DWCi_GetMatchCnt()->sbObj, SBFalse, DWC_QR2_MATCH_EVAL_KEY_STR, sbcm_int);
    }

    if (deleteFlag && !ServerBrowserCount(DWCi_GetMatchCnt()->sbObj)) return FALSE;
    else return TRUE;
}


/*---------------------------------------------------------------------------*
  サーバリスト順序ランダム化関数
  引数　：なし
  戻り値：なし
  用途　：評価済みサーバリストを、評価値の大きさを考慮しつつランダム要素を
          加えて再ソートする。
          DWCi_EvaluateServers()の呼び出し直後に呼び出す為だけの関数。
          [note]
          選んだサーバ以外のデータが必要なけばソートはしなくて良い
 *---------------------------------------------------------------------------*/
static void DWCi_RandomizeServers(void)
{
    u32 rand;
    int tmpEval, maxEval = 0;
    int sum = 0;
    int i;
    int eval[DWC_SB_UPDATE_MAX_SERVERS];
    SBServer server;

    if (ServerBrowserCount(DWCi_GetMatchCnt()->sbObj) <= 1) return;

    // 全サーバを調べて評価値の最大値と、評価レートの合計を求める
    for (i = 0; i < ServerBrowserCount(DWCi_GetMatchCnt()->sbObj); i++){
        server  = ServerBrowserGetServer(DWCi_GetMatchCnt()->sbObj, i);
        tmpEval = SBServerGetIntValue(server, DWC_QR2_MATCH_EVAL_KEY_STR, -1);
        if (tmpEval > maxEval) maxEval = tmpEval;
        
        sum += stEvalRate[i];
    }

    // 0～99の乱数値を求める
    rand = DWCi_GetMathRand32(100);

    // 各サーバの評価レート値の割合（％）を求め、乱数値からサーバを選ぶ
    for (i = 0; i < ServerBrowserCount(DWCi_GetMatchCnt()->sbObj); i++){
        if (i == ServerBrowserCount(DWCi_GetMatchCnt()->sbObj)-1){
            // 除算誤差のため合計必ず100％にはならないので、最後までいったら
            // 必ずそのサーバを選ぶ
            eval[i] = 100;
            break;
        }
        else {
            eval[i] = (100*stEvalRate[i])/sum+(i > 0 ? eval[i-1] : 0);
            if (rand < eval[i]) break;
        }
    }

    // ここで表示するインデックス[]の値は、評価値の大きい順にサーバリストを
    // ソートした上でのインデックス値であることに注意する
    DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
               "Server[%d] is selected (%d/100: rand %d)\n",
               i, eval[i], rand);

    // 選ばれたサーバに最大の評価値を与えてから、ソートし直す
    if (maxEval < 0x7fffffff) maxEval++;
    SBServerAddIntKeyValue(ServerBrowserGetServer(DWCi_GetMatchCnt()->sbObj, i),
                           DWC_QR2_MATCH_EVAL_KEY_STR,
                           maxEval);

    ServerBrowserSort(DWCi_GetMatchCnt()->sbObj, SBFalse, DWC_QR2_MATCH_EVAL_KEY_STR, sbcm_int);
}


/*---------------------------------------------------------------------------*
  QR2サーバキー要求受信コールバック関数
  引数　：keyid    キーID
          outbuf   送信用バッファ
          userdata コールバックパラメータ
  戻り値：なし
  用途　：QR2サーバキー要求受信コールバック
 *---------------------------------------------------------------------------*/
static void DWCi_QR2ServerKeyCallback(int keyid, qr2_buffer_t outbuf, void *userdata)
{
#pragma unused(userdata)
    int index;

    switch (keyid){
    case NUMPLAYERS_KEY:
        qr2_buffer_add_int(outbuf, DWCi_GetMatchCnt()->qr2NNFinishCount);
        DWC_Printf(DWC_REPORTFLAG_QR2_REQ, "QR2, Received ServerKeyReq : keyID %d - %d\n", keyid, DWCi_GetMatchCnt()->qr2NNFinishCount);
        break;
    case MAXPLAYERS_KEY:
        qr2_buffer_add_int(outbuf, DWCi_GetMatchCnt()->qr2NumEntry);
        DWC_Printf(DWC_REPORTFLAG_QR2_REQ, "QR2, Received ServerKeyReq : keyID %d - %d\n", keyid, DWCi_GetMatchCnt()->qr2NumEntry );
        break;
    case DWC_QR2_PID_KEY:
        qr2_buffer_add_int(outbuf, DWCi_GetMatchCnt()->profileID);
        DWC_Printf(DWC_REPORTFLAG_QR2_REQ, "QR2, Received ServerKeyReq : keyID %d - %d\n", keyid, DWCi_GetMatchCnt()->profileID );
        break;
    case DWC_QR2_MATCH_TYPE_KEY:
        qr2_buffer_add_int(outbuf, DWCi_GetMatchCnt()->qr2MatchType);
        DWC_Printf(DWC_REPORTFLAG_QR2_REQ, "QR2, Received ServerKeyReq : keyID %d - %d\n", keyid, DWCi_GetMatchCnt()->qr2MatchType );
        break;
    case DWC_QR2_MATCH_RESV_KEY:
        qr2_buffer_add_int(outbuf, DWCi_GetMatchCnt()->qr2Reservation);
        DWC_Printf(DWC_REPORTFLAG_QR2_REQ, "QR2, Received ServerKeyReq : keyID %d - %d\n", keyid, DWCi_GetMatchCnt()->qr2Reservation );
        break;
    case DWC_QR2_MATCH_VER_KEY:
        qr2_buffer_add_int(outbuf, DWC_MATCHING_VERSION);
        DWC_Printf(DWC_REPORTFLAG_QR2_REQ, "QR2, Received ServerKeyReq : keyID %d - %d\n", keyid, DWC_MATCHING_VERSION);
        break;
    case DWC_QR2_MATCH_EVAL_KEY:
        qr2_buffer_add_int(outbuf, 1);
        DWC_Printf(DWC_REPORTFLAG_QR2_REQ, "QR2, Received ServerKeyReq : keyID %d - %d\n", keyid, 1 );
        break;
    default:
        // ゲーム定義のキーデータが存在するならアップする
        index = keyid-DWC_QR2_GAME_KEY_START;
        if ( ( index >= 0 && index < DWC_QR2_GAME_RESERVED_KEYS )
             && stGameMatchKeys[index].keyID)
        {
            if (stGameMatchKeys[index].isStr){
                qr2_buffer_add(outbuf, (char *)stGameMatchKeys[index].value);
            }
            else {
                qr2_buffer_add_int(outbuf, *(int *)stGameMatchKeys[index].value);
            }
        }
        break;
    }
}


/*---------------------------------------------------------------------------*
  QR2プレイヤーキー要求受信コールバック関数
  引数　：keyid    キーID
          index    プレイヤーインデックス
          outbuf   送信用バッファ
          userdata コールバックパラメータ
  戻り値：なし
  用途　：QR2プレイヤーキー要求受信コールバック
 *---------------------------------------------------------------------------*/
static void DWCi_QR2PlayerKeyCallback(int keyid, int index, qr2_buffer_t outbuf, void *userdata)
{
#pragma unused(keyid)
#pragma unused(index)
#pragma unused(outbuf)
#pragma unused(userdata)
}


/*---------------------------------------------------------------------------*
  QR2チームキー要求受信コールバック関数
  引数　：keyid    キーID
          index    チームインデックス
          outbuf   送信用バッファ
          userdata コールバックパラメータ
  戻り値：なし
  用途　：QR2チームキー要求受信コールバック
 *---------------------------------------------------------------------------*/
static void DWCi_QR2TeamKeyCallback(int keyid, int index, qr2_buffer_t outbuf, void *userdata)
{
#pragma unused(keyid)
#pragma unused(index)
#pragma unused(outbuf)
#pragma unused(userdata)
}


/*---------------------------------------------------------------------------*
  QR2キーリスト要求受信コールバック関数
  引数　：keytype   キー種別
          keybuffer キー送信用バッファ
          userdata  コールバックパラメータ
  戻り値：なし
  用途　：QR2キーリスト要求受信コールバック
 *---------------------------------------------------------------------------*/
static void DWCi_QR2KeyListCallback(qr2_key_type keytype, qr2_keybuffer_t keybuffer, void *userdata)
{
#pragma unused(userdata)
    int i;

    // 必要なキーのリストを送信するためにバッファに登録する
    switch (keytype){
	case key_server:
        qr2_keybuffer_add(keybuffer, NUMPLAYERS_KEY);
        qr2_keybuffer_add(keybuffer, MAXPLAYERS_KEY);
        qr2_keybuffer_add(keybuffer, DWC_QR2_PID_KEY);
        qr2_keybuffer_add(keybuffer, DWC_QR2_MATCH_TYPE_KEY);
        qr2_keybuffer_add(keybuffer, DWC_QR2_MATCH_RESV_KEY);
        qr2_keybuffer_add(keybuffer, DWC_QR2_MATCH_VER_KEY);
        qr2_keybuffer_add(keybuffer, DWC_QR2_MATCH_EVAL_KEY);

        // ゲーム定義のキーがあるならそれも送信する
        for (i = 0; i < DWC_QR2_GAME_RESERVED_KEYS; i++){
            if (stGameMatchKeys[i].keyID){
                qr2_keybuffer_add(keybuffer, stGameMatchKeys[i].keyID);
            }
        }
        break;
    case key_player:
        break;
    case key_team:
        break;
    }

    DWC_Printf(DWC_REPORTFLAG_QR2_REQ, "QR2, Received KeyListReq : keytype %d\n", keytype);
}


/*---------------------------------------------------------------------------*
  QR2登録プレイヤー数もしくはチーム数取得コールバック関数
  引数　：keytype  キー種別
          userdata コールバックパラメータ
  戻り値：プレイヤー数、もしくはチーム数
  用途　：QR2登録プレイヤー数もしくはチーム数取得コールバック
 *---------------------------------------------------------------------------*/
static int DWCi_QR2CountCallback(qr2_key_type keytype, void *userdata)
{
#pragma unused(keytype)
#pragma unused(userdata)

    return 0;
}


/*---------------------------------------------------------------------------*
  QR2マスターサーバへの登録失敗コールバック関数
  引数　：error    QR2エラー種別
          userdata コールバックパラメータ
  戻り値：なし
  用途　：QR2マスターサーバへの登録失敗コールバック
 *---------------------------------------------------------------------------*/
static void DWCi_QR2AddErrorCallback(qr2_error_t error, gsi_char *errmsg, void *userdata)
{
#pragma unused(userdata)
#if defined(SDK_FINALROM)
#pragma unused(errmsg)
#endif

    DWC_Printf(DWC_REPORTFLAG_ERROR, "QR2 Failed query addition to master server %d\n", error);
    DWC_Printf(DWC_REPORTFLAG_ERROR, "%s\n", errmsg);

    (void)DWCi_HandleQR2Error(error);
}


/*---------------------------------------------------------------------------*
  QR2パブリックアドレス通知コールバック関数
  引数　：ip       QR2が使用するパブリックIP
          port     QR2が使用するパブリックポート番号
          userdata コールバックパラメータ
  戻り値：なし
  用途　：QR2パブリックアドレス通知コールバック
 *---------------------------------------------------------------------------*/
static void DWCi_QR2PublicAddrCallback(unsigned int ip, unsigned short port, void* userdata)
{
#pragma unused(userdata)

    DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
               "Got my query IP %08x & port %d.\n", ip, port);

    DWCi_GetMatchCnt()->qr2IP   = ip;
    DWCi_GetMatchCnt()->qr2Port = port;
}


/*---------------------------------------------------------------------------*
  QR2 NATネゴシエーション要求受信コールバック関数
  引数　：cookie   受信したクッキー値
          userdata コールバックパラメータ
  戻り値：なし
  用途　：QR2 NATネゴシエーション要求受信コールバック
 *---------------------------------------------------------------------------*/
static void DWCi_QR2NatnegCallback(int cookie, void *userdata)
{
#pragma unused(userdata)
    NegotiateError nnError;

    // NATネゴシエーション要求は必ず受け入れる
    DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Got NN request, cookie = %x.\n", cookie);

    if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_WAITING){
        // 新規接続クライアントからの要求を受け付けた場合は状態を進める
        DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_NN);
    }
    else if ((DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_NN) &&
             (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_SV_OWN_NN)){
        // なかなかクライアントが予約に来ないので予約をキャンセルした後に
        // NN要求が来た場合は無視する
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                   "But already canceled reservation.\n");
        return;
    }

    if (DWCi_GetMatchCnt()->nnLastCookie == cookie){
        // 前回と同じクッキーを受信したらNNリトライと判断してカウントアップ
        DWCi_GetMatchCnt()->nnRecvCount++;
    }
    else {
        // 前回と違うクッキーを受信したら新たなNNなのでカウンタクリア
        DWCi_GetMatchCnt()->nnRecvCount  = 0;
        DWCi_GetMatchCnt()->nnLastCookie = cookie;
    }

    // NATネゴシエーションのクッキーを受信したら失敗時時刻クリア
    DWCi_GetMatchCnt()->nnFailedTime = 0;

    // NATネゴシエーション開始
    nnError = DWCi_NNStartupAsync(1, cookie, NULL);
    if (DWCi_HandleNNError(nnError)) return;

    // 予約保持タイムアウト計測終了
    DWCi_GetMatchCnt()->cmdCnt.command = DWC_MATCH_COMMAND_DUMMY;
}


/*---------------------------------------------------------------------------*
  QR2クライアントメッセージ受信コールバック関数
  引数　：data     受信したデータ
          len      受信したデータ長
          userdata コールバックパラメータ
  戻り値：なし
  用途　：QR2クライアントメッセージ受信コールバック
 *---------------------------------------------------------------------------*/
static void DWCi_QR2ClientMsgCallback(gsi_char* data, int len, void* userdata)
{
#pragma unused(userdata)
    int offset = 0;
    BOOL boolResult;
    DWCSBMessage sbMsg;

    // [arakit] main 051010
    // マッチメイク中でなければコマンドを受け付けない
    if ((DWC_GetState() != DWC_STATE_MATCHING) &&
        ((DWC_GetState() != DWC_STATE_CONNECTED) ||
         ((DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_SC_SV) &&
          (DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_SC_CL)))){
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Ignore delayed SB matching command.\n");
        return;
    }
    // [arakit] main 051010

    //if (len < sizeof(DWCSBMessageHeader)){
        // 定義されていないコマンドのメッセージを受信した場合
    //    DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "QR2, Got undefined SBcommand\n");
    //    return;
    //}

    // 複数コマンドが連続したパケットにも対応できるようにしているが、
    // そのようなパケットを送信することはない
    while (offset+sizeof(DWCSBMessageHeader) <= len){
        // SBメッセージをコピーする
        MI_CpuCopy8(data, &sbMsg, sizeof(DWCSBMessageHeader));

        if (strncmp(sbMsg.header.identifier, DWC_SB_COMMAND_STRING, 4)){
            // 識別子の違うコマンドを受信した
            DWC_Printf(DWC_REPORTFLAG_WARNING, "Got undefined SBcommand.\n");
            return;
        }
        if (sbMsg.header.version != DWC_MATCHING_VERSION){
            // マッチメイクプロトコルのバージョン違いコマンドを受信した
            DWC_Printf(DWC_REPORTFLAG_WARNING, "Got different version SBcommand.\n");
            return;
        }
        
        MI_CpuCopy8(data+sizeof(DWCSBMessageHeader), sbMsg.data, sbMsg.header.size);

#ifdef NITRODWC_DEBUG
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                   "<SB> RECV-%s <- [%08x:%d] [pid=%u]\n",
                   get_dwccommandstr(sbMsg.header.command),
                   sbMsg.header.qr2IP, sbMsg.header.qr2Port,
                   sbMsg.header.profileID);
#else
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                   "<SB> RECV-0x%02x <- [%08x:%d] [pid=%u]\n",
                   sbMsg.header.command,
                   sbMsg.header.qr2IP, sbMsg.header.qr2Port,
                   sbMsg.header.profileID);
#endif		
        // 受信したコマンドを処理する
        // エラー処理は関数内で行っている
        boolResult = DWCi_ProcessRecvMatchCommand(sbMsg.header.command,
                                                  sbMsg.header.profileID,
                                                  sbMsg.header.qr2IP,
                                                  sbMsg.header.qr2Port,
                                                  sbMsg.data,
                                                  sbMsg.header.size >> 2);
        if (!boolResult) return;  // エラーの場合は即終了

        offset += sizeof(DWCSBMessageHeader)+sbMsg.header.size;
    }
}


/*---------------------------------------------------------------------------*
  NNネゴシエーション処理コールバック関数
  引数　：state    NATネゴシエーション処理状態
          userdata コールバックパラメータ
  戻り値：なし
  用途　：NATネゴシエーション処理コールバック
 *---------------------------------------------------------------------------*/
static void DWCi_NNProgressCallback(NegotiateState state, void* userdata)
{
#pragma unused(userdata)
#if defined(SDK_FINALROM)
#pragma unused(state)
#endif

	DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "NN, Got state update: %d\n", state);
}


/*---------------------------------------------------------------------------*
  NNネゴシエーション完了コールバック関数
  引数　：result     NATネゴシエーション結果型
          gamesocket 通信ソケット記述子
          remoteaddr アドレス構造体へのポインタ
          userdata   コールバックパラメータ
  戻り値：なし
  用途　：NATネゴシエーション処理コールバック
 *---------------------------------------------------------------------------*/
static void DWCi_NNCompletedCallback(NegotiateResult result, SOCKET gamesocket, struct sockaddr_in* remoteaddr, void* userdata)
{
#pragma unused(gamesocket)
    char pidStr[12];
    int  index;
    NegotiateError nnError;
    NegotiateResult nnResult;
    GT2Result gt2Result;
    DWCNNInfo* nnInfo = (DWCNNInfo *)userdata;

    DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "NN, Complete NAT Negotiation. result : %d\n", result);
    if (nnInfo)
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "NN cookie = %x.\n", nnInfo->cookie);

    if (((DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_NN) &&
         (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_SV_OWN_NN)) ||
        !nnInfo){
        // マッチメイクキャンセル中か、キャンセル完了後に来ても無視する。
        // NNクッキーを受信できず、NATネゴシエーション失敗コールバックだけ
        // 受信した場合（!nnInfoの場合）も無視する。
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Ignore delayed NN after cancel.\n");
        return;
    }

    if (result == nr_success){
        if (remoteaddr){
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "NN, remote address : %s\n",
                       gt2AddressToString(remoteaddr->sin_addr.s_addr, SO_NtoHs(remoteaddr->sin_port), NULL));
        }

        nnInfo->cookie = 0;  // NN終了を記録

        DWCi_GetMatchCnt()->qr2NNFinishCount++;        // NN完了カウントアップ
        index = DWCi_GetMatchCnt()->qr2NNFinishCount;  // インデックス退避

        if (nnInfo->isQR2){
            // NN受信側の場合
            // IP、ポートリストを更新
            DWCi_GetMatchCnt()->ipList[index]   = remoteaddr->sin_addr.s_addr;
            DWCi_GetMatchCnt()->portList[index] = SO_NtoHs(remoteaddr->sin_port);

            DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                       "NN child finished Nat Negotiation.\n");

            // NN受信情報クリア
            DWCi_GetMatchCnt()->nnRecvCount  = 0;
            DWCi_GetMatchCnt()->nnLastCookie = 0;
            DWCi_GetMatchCnt()->nnFailedTime = 0;

            // GT2接続状態に進む
            if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_OWN_NN){
                DWCi_SetMatchStatus(DWC_MATCH_STATE_SV_OWN_GT2);
            }
            else {
                DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_GT2);
            }

            // GT2Connectリトライカウント初期化
            DWCi_GetMatchCnt()->gt2ConnectCount = 0;

            // 必ずこちらからgt2Connect()を行う
            DWC_Printf(DWC_REPORTFLAG_MATCH_GT2,
                       "gt2Connect() to pidList[%d] (%s)\n",
                       index,
                       gt2AddressToString(DWCi_GetMatchCnt()->ipList[index], DWCi_GetMatchCnt()->portList[index], NULL));

            // コネクション構造体はConnectedCallback内で取得したいので
            // ここではNULLを渡しておく。
            // messageとして自分のプロファイルIDを送信する。
            (void)OS_SNPrintf(pidStr, sizeof(pidStr), "%u", DWCi_GetMatchCnt()->profileID);
            gt2Result =
                gt2Connect(*DWCi_GetMatchCnt()->pGt2Socket,
                           NULL,
                           gt2AddressToString(DWCi_GetMatchCnt()->ipList[index], DWCi_GetMatchCnt()->portList[index], NULL),
                           (GT2Byte *)pidStr,
                           -1,
                           DWC_GT2_CONNECT_TIMEOUT,
                           DWCi_GetMatchCnt()->gt2Callbacks,
                           GT2False);
            if ( gt2Result == GT2OutOfMemory )
            {
                DWCi_HandleGT2Error(gt2Result);
                return;
            }
            else if ( gt2Result == GT2Success )
            {
                // 成功のときは、コールバックを待つ。
            }
            else if (!DWCi_CancelPreConnectedServerProcess(DWCi_GetMatchCnt()->sbPidList[index]))
            {
                // gt2Connectにいきなり失敗したときは、リセットして次の接続を待つ。
                return;
            }
        }
        else {
            // NN開始側の場合
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                       "NN parent finished Nat Negotiation.\n");

            if (remoteaddr){
                // 実際にNATネゴシエーションを行った場合は
                // IP、ポートリストを更新
                DWCi_GetMatchCnt()->ipList[index-1]   = remoteaddr->sin_addr.s_addr;
                DWCi_GetMatchCnt()->portList[index-1] = SO_NtoHs(remoteaddr->sin_port);
            }

            // gt2Connect()を受信するまでの時間を計測するため、
            // 現在時刻を記録する
            DWCi_GetMatchCnt()->nnFinishTime = OS_GetTick();

            // 続けてGT2コネクションを確立する（gt2Connectされるのを待つ）
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_GT2);
        }
    }
    else {
        // NATネゴシエーション失敗の場合
        if (!nnInfo->cookie){
            // マッチメイクキャンセル完了後に来た場合は無視する
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "Ignore delayed NN error after cancel.\n");
            return;
        }
            
        // nr_inittimeout, nr_deadbeatpartnerではエラー処理はしないようにしている
        nnResult = DWCi_HandleNNResult(result);

        if ((nnResult != nr_inittimeout) && (nnResult != nr_deadbeatpartner)) return;

        if (!nnInfo->isQR2){
            // SB側（NATネゴシエーション開始側）
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Failed %d/%d NN send.\n",
                       nnInfo->retryCount, DWC_MAX_MATCH_NN_RETRY);
                
            if ((nnResult == nr_deadbeatpartner) ||
                ((nnResult == nr_inittimeout) &&
                 (nnInfo->retryCount >= DWC_MAX_MATCH_NN_RETRY))){
                // 相手からの応答がなくなった、もしくはリトライ回数オーバの場合
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Abort NN.\n");

                nnInfo->cookie = 0;  // NN終了を記録

                // NN失敗回数をカウントして処理する
                if (!DWCi_ProcessNNFailure(FALSE)) return;

                // 全てのクライアント処理を終了してマッチメイクを再開する。
                // サーバクライアントマッチメイクの場合はエラー終了する。
                if (!DWCi_CancelPreConnectedClientProcess(DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->gt2NumConnection]))
                    return;
            }
            else {
                nnInfo->retryCount++;  // タイムアウト回数記録
                
                // NATネゴシエーション開始
                nnError = DWCi_DoNatNegotiationAsync(nnInfo);
                if (DWCi_HandleNNError(nnError)) return;
            }
        }
        else {
            // QR2側（NATネゴシエーション受信側）
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Failed %d/%d NN recv.\n",
                       DWCi_GetMatchCnt()->nnRecvCount, DWC_MAX_MATCH_NN_RETRY);

            // NATネゴシエーションに失敗した時間を記録
            DWCi_GetMatchCnt()->nnFailedTime = OS_GetTick();
            
            if ((nnResult == nr_deadbeatpartner) ||
                ((nnResult == nr_inittimeout) &&
                 (DWCi_GetMatchCnt()->nnRecvCount >= DWC_MAX_MATCH_NN_RETRY))){
                // 相手からの応答がなくなった、もしくはリトライ回数オーバの場合
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Abort NN.\n");

                nnInfo->cookie = 0;  // NN終了を記録

                // NN失敗回数をカウントして処理する
                if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL) ||
                    (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV)){
                    if (!DWCi_ProcessNNFailure(TRUE)) return;
                }
                else {
                    if (!DWCi_ProcessNNFailure(FALSE)) return;
                }

                // NN受信情報クリア
                DWCi_GetMatchCnt()->nnRecvCount  = 0;
                DWCi_GetMatchCnt()->nnLastCookie = 0;
                DWCi_GetMatchCnt()->nnFailedTime = 0;
                    
                // 新規接続の受け付け処理を終了してマッチメイクを再開する。
                // サーバクライアントマッチメイクの場合は、新規接続クライアントの
                // 接続キャンセルとして処理する。
                if (!DWCi_CancelPreConnectedServerProcess(DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->gt2NumConnection+1]))
                    return;
            }
        }
    }
}


/*---------------------------------------------------------------------------*
  NNネゴシエーション失敗回数関連処理関数
  引数　：ignoreError TRUE:失敗をカウントしない、FALSE:失敗をカウントする
  戻り値：TRUE:まだ失敗できる、FALSE:規定回数失敗でマッチメイクエラー終了
  用途　：NATネゴシエーション処理コールバック
 *---------------------------------------------------------------------------*/
static BOOL DWCi_ProcessNNFailure(BOOL ignoreError)
{

    // サーバクライアントマッチメイクのサーバと接続済みクライアントは、
    // NNをキャンセルしてプレイを続行する為、NN失敗回数のカウントは行わない
    if (ignoreError) return TRUE;
    
    if (DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_SC_CL){
        // NN失敗回数をカウント
        DWCi_GetMatchCnt()->nnFailureCount++;

        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "NN failure %d/%d.\n",
                   DWCi_GetMatchCnt()->nnFailureCount, DWC_MATCH_NN_FAILURE_MAX);
    }

    if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL) ||
        (DWCi_GetMatchCnt()->nnFailureCount >= DWC_MATCH_NN_FAILURE_MAX)){
        // 規定回数以上失敗したらマッチメイクをエラー終了する。
        // サーバクライアントマッチメイクのクライアントでは必ずエラー終了する
        DWCi_StopMatching(DWC_ERROR_NETWORK,
                          DWC_ECODE_SEQ_MATCH+DWC_ECODE_GS_NN+DWC_ECODE_TYPE_MUCH_FAILURE);
        return FALSE;
    }
    else {
        return TRUE;
    }
}
//----------------------------------------------------------------------------
// カプセル化風の処理をするために下にもってきている変数
//----------------------------------------------------------------------------
// マッチメイク制御オブジェクトへのポインタ
static DWCMatchControl* stpMatchCnt = NULL;
#ifdef NITRODWC_DEBUG
static OSTick sMatchStateTick;
#endif
/*---------------------------------------------------------------------------*
  アクセス関数
 *---------------------------------------------------------------------------*/
static void DWCi_SetMatchCnt(DWCMatchControl *pCnt){ 
  stpMatchCnt = pCnt; 
#ifdef NITRODWC_DEBUG
  sMatchStateTick = OS_GetTick();
#endif
}
static DWCMatchControl *DWCi_GetMatchCnt(void){ return (stpMatchCnt); }
static void DWCi_SetMatchStatus(DWCMatchState state)
{
  SDK_ASSERT(stpMatchCnt);
#ifdef NITRODWC_DEBUG
  if(stpMatchCnt->state != state){
	disp_match_state(stpMatchCnt->state, state);
  }
#endif
  stpMatchCnt->state = state;
}

#ifdef NITRODWC_DEBUG
static char *match_st_string[DWC_MATCH_STATE_NUM] = 
{
  "DWC_MATCH_STATE_INIT",           // 初期状態
  // クライアント側の状態
  "DWC_MATCH_STATE_CL_WAITING",         // 待ち状態
  "DWC_MATCH_STATE_CL_SEARCH_OWN",      // 自ホスト情報検索中
  "DWC_MATCH_STATE_CL_SEARCH_HOST",     // 空きホスト検索状態（友達無指定の時のみ）
  "DWC_MATCH_STATE_CL_WAIT_RESV",       // 予約に対するサーバからの返答待ち
  "DWC_MATCH_STATE_CL_SEARCH_NN_HOST",  // 予約が完了した相手ホストを検索中
  "DWC_MATCH_STATE_CL_NN",              // NATネゴシエーション中
  "DWC_MATCH_STATE_CL_GT2",             // GT2コネクション確立中
  "DWC_MATCH_STATE_CL_CANCEL_SYN",      // サーバクライアントでマッチメイクキャンセル同期調整中
  "DWC_MATCH_STATE_CL_SYN",             // マッチメイク完了同期調整中
        
  // サーバ側の状態
  "DWC_MATCH_STATE_SV_WAITING",         // 待ち状態
  "DWC_MATCH_STATE_SV_OWN_NN",          // クライアントとのNATネゴシエーション中
  "DWC_MATCH_STATE_SV_OWN_GT2",         // クライアントとのGT2コネクション確立中
  "DWC_MATCH_STATE_SV_WAIT_CL_LINK",    // クライアント同士の接続完了待ち
  "DWC_MATCH_STATE_SV_CANCEL_SYN",      // サーバクライアントでマッチメイクキャンセル同期調整SYN-ACK待ち
  "DWC_MATCH_STATE_SV_CANCEL_SYN_WAIT", // サーバクライアントでマッチメイクキャンセル同期調整終了待ち
  "DWC_MATCH_STATE_SV_SYN",             // マッチメイク終了同期調整SYN-ACK待ち
  "DWC_MATCH_STATE_SV_SYN_WAIT",        // マッチメイク終了同期調整終了待ち
  
  // 共通状態
  "DWC_MATCH_STATE_WAIT_CLOSE",         // コネクションクローズ完了待ち
  
  // マッチメイクオプション使用時のみの状態
  "DWC_MATCH_STATE_SV_POLL_TIMEOUT",    // サーバがOPTION_MIN_COMPLETEのタイムアウトをポーリング中
};

static void disp_time(void)
{
  int m_time, sec;

  m_time = (int)OS_TicksToMilliSeconds(OS_GetTick() - sMatchStateTick);
  sec = m_time/1000;
  m_time = (m_time-sec+50)/100;
  OS_TPrintf("経過時間 %d.%d 秒\n", sec, m_time);
}
static void disp_match_state(DWCMatchState old, DWCMatchState now)
{
  int m_time, sec;
  OSTick tick;
  tick = sMatchStateTick; sMatchStateTick = OS_GetTick(); tick = sMatchStateTick - tick;
  m_time = (int)OS_TicksToMilliSeconds(tick);
  sec = m_time/1000;
  m_time = (m_time-sec+50)/100;
  OS_TPrintf("ステータス変更 %s(%d.%d 秒) -> %s\n", match_st_string[old], sec, m_time, match_st_string[now]);
}
static const char *get_dwccommandstr(u8 command)
{
	switch(command) {
	case DWC_MATCH_COMMAND_RESERVATION:
		return "DWC_MATCH_COMMAND_RESERVATION";
	case DWC_MATCH_COMMAND_RESV_OK:
		return "DWC_MATCH_COMMAND_RESV_OK";
	case DWC_MATCH_COMMAND_RESV_DENY:
		return "DWC_MATCH_COMMAND_RESV_DENY";
	case DWC_MATCH_COMMAND_RESV_WAIT:
		return "DWC_MATCH_COMMAND_RESV_WAIT";
	case DWC_MATCH_COMMAND_RESV_CANCEL:
		return "DWC_MATCH_COMMAND_RESV_CANCEL";
	case DWC_MATCH_COMMAND_TELL_ADDR:
		return "DWC_MATCH_COMMAND_TELL_ADDR";
	case DWC_MATCH_COMMAND_NEW_PID_AID:
		return "DWC_MATCH_COMMAND_NEW_PID_AID";
	case DWC_MATCH_COMMAND_LINK_CLS_REQ:
		return "DWC_MATCH_COMMAND_LINK_CLS_REQ";
	case DWC_MATCH_COMMAND_LINK_CLS_SUC:
		return "DWC_MATCH_COMMAND_LINK_CLS_SUC";
	case DWC_MATCH_COMMAND_CLOSE_LINK:
		return "DWC_MATCH_COMMAND_CLOSE_LINK";
	case DWC_MATCH_COMMAND_RESV_PRIOR:
		return "DWC_MATCH_COMMAND_RESV_PRIOR";
	case DWC_MATCH_COMMAND_CANCEL:
		return "DWC_MATCH_COMMAND_CANCEL";
	case DWC_MATCH_COMMAND_CANCEL_SYN:
		return "DWC_MATCH_COMMAND_CANCEL_SYN";
	case DWC_MATCH_COMMAND_CANCEL_SYN_ACK:
		return "DWC_MATCH_COMMAND_CANCEL_SYN_ACK";
	case DWC_MATCH_COMMAND_CANCEL_ACK:
		return "DWC_MATCH_COMMAND_CANCEL_ACK";
	case DWC_MATCH_COMMAND_SC_CLOSE_CL:
		return "DWC_MATCH_COMMAND_SC_CLOSE_CL";
	case DWC_MATCH_COMMAND_POLL_TIMEOUT:
		return "DWC_MATCH_COMMAND_POLL_TIMEOUT";
	case DWC_MATCH_COMMAND_POLL_TO_ACK:
		return "DWC_MATCH_COMMAND_POLL_TO_ACK";
	case DWC_MATCH_COMMAND_SC_CONN_BLOCK:
		return "DWC_MATCH_COMMAND_SC_CONN_BLOCK";
	case DWC_MATCH_COMMAND_FRIEND_ACCEPT:
		return "DWC_MATCH_COMMAND_FRIEND_ACCEPT";
	case DWC_MATCH_COMMAND_CL_WAIT_POLL:
		return "DWC_MATCH_COMMAND_CL_WAIT_POLL";
	case DWC_MATCH_COMMAND_SV_KA_TO_CL:
		return "DWC_MATCH_COMMAND_SV_KA_TO_CL";
	case DWC_MATCH_COMMAND_DUMMY:
		return "DWC_MATCH_COMMAND_DUMMY";
	default:
		return "DWC_MATCH_COMMAND_UNDEFINED";
	}
}
#endif
