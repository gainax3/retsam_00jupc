#include <nitro.h>
#include <base/dwc_base_gamespy.h>
#include <gp/gpi.h>
#include <gstats/gpersist.h>

#include <auth/util_base64.h>
#include <base/dwc_report.h>
#include <base/dwc_account.h>
#include <base/dwc_error.h>
#include <base/dwc_login.h>
#include <base/dwc_friend.h>
#include <base/dwc_match.h>
#include <base/dwc_transport.h>
#include <base/dwc_main.h>
#include <base/dwc_common.h>


//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------
// GPサーバへのキープアライブを入れる。
//#define DWC_ENABLE_KEEPALIVE_TO_GPSERVER


//----------------------------------------------------------------------------
// variable
//----------------------------------------------------------------------------
// 友達管理オブジェクトへのポインタ
static DWCFriendControl* stpFriendCnt = NULL;
static int stPersState = DWC_PERS_STATE_INIT;

// CloseStatsConnection()をPersistThink()から呼ばないように調整するフラグ
static BOOL s_bInPersistThink = FALSE;
static BOOL s_bNeedCloseStatsConnection = FALSE;

//----------------------------------------------------------------------------
// prototype
//----------------------------------------------------------------------------
static GPResult DWCi_GPProcess(void);
static void DWCi_CloseFriendProcess(void);
static void  DWCi_UpdateFriendReq(DWCFriendData friendList[], int friendListLen);
static void DWCi_EndUpdateServers(void);

static void DWCi_DeleteFriendFromList(DWCFriendData friendList[], int index, int otherIndex);
static BOOL DWCi_RefreshFriendListForth(DWCFriendData friendList[], int index, int profileID);
static int  DWCi_RefreshFriendListAll(DWCFriendData friendList[], int friendListLen, int profileID);

static GPResult DWCi_GPSendBuddyRequest(int profileID);

static BOOL DWCi_GetFriendBuddyStatus(const DWCFriendData* friendData, GPBuddyStatus* status);

static GPResult DWCi_HandleGPError(GPResult result);
static int DWCi_HandlePersError(int error);

static void DWCi_GPProfileSearchCallback(GPConnection* connection,
                                         GPProfileSearchResponseArg* arg,
                                         void* param);
static void DWCi_GPGetInfoCallback_RecvBuddyRequest(GPConnection* connection,
                                                    GPGetInfoResponseArg* arg,
                                                    void* param);
static void DWCi_GPGetInfoCallback_RecvAuthMessage(GPConnection* connection,
                                                   GPGetInfoResponseArg* arg,
                                                   void* param);

static void DWCi_StopPersLogin(DWCError error, int errorCode);
static void DWCi_SetPersistDataValuesAsync(int profileID,
                                           persisttype_t type,
                                           gsi_char* keyvalues,
                                           void* param);
static void DWCi_GetPersistDataValuesAsync(int profileID,
                                           persisttype_t type,
                                           gsi_char* keys,
                                           void* param);
static void DWCi_PersAuthCallback(int localid, int profileid,
                                  int authenticated, gsi_char* errmsg,
                                  void* instance);
static void DWCi_PersDataCallback(int localid, int profileid,
                                  persisttype_t type, int index, int success,
                                  time_t modified, char* data, int len,
                                  void* instance);
static void DWCi_PersDataSaveCallback(int localid, int profileid,
                                      persisttype_t type, int index, int success,
                                      time_t modified, void* instance);
static void DWCi_AddPersCallbackLevel(void);
static void DWCi_SubPersCallbackLevel(void);
static u32  DWCi_GetPersCallbackLevel(void);


//----------------------------------------------------------------------------
// function - external
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  友達通信状態取得関数
  引数　：friendData   友達情報へのポインタ
          statusString ゲームで定義したGameSpy status文字列格納先ポインタ。
                       NULLを渡せば文字列のコピーは行わない。
                       文字列は最大255文字＋NULL終端。
  戻り値：通信状態enum値（dwc_friend.h のDWC_STATUS_*）。
          自分がオフラインの時などに呼び出すとDWC_STATUS_OFFLINEしか返さない。
          また相手がまだGPのバディリストに登録されていない時も
          DWC_STATUS_OFFLINE返す。
  用途　：ローカルにキャッシュされた、指定の友達の通信状態を取得する。
  　　　　通信は行わない。
          友達の通信状態は、変化がある度にGPサーバから通知される。
          ただし、通信中に突然電源を切った場合などは数分間は前の状態が残る。
 *---------------------------------------------------------------------------*/
u8 DWC_GetFriendStatus(const DWCFriendData* friendData, char* statusString)
{

    return DWC_GetFriendStatusSC(friendData, NULL, NULL, statusString);
}


/*---------------------------------------------------------------------------*
  友達通信状態取得関数（サーバクライアントマッチメイクゲーム用）
  引数　：friendData   友達情報へのポインタ
          maxEntry     友達がサーバクライアントマッチメイクのサーバを立てて
                       いる場合の最大接続人数。
                       格納先ポインタ。NULLを渡せば取得しない。
          numEntry     友達がサーバクライアントマッチメイクのサーバを立てて
                       いる場合の現在接続人数。
                       格納先ポインタ。NULLを渡せば取得しない。
          statusString ゲームで定義したGameSpy status文字列格納先ポインタ。
                       NULLを渡せば文字列のコピーは行わない。
                       文字列は最大255文字＋NULL終端。
  戻り値：通信状態enum値（dwc_friend.h のDWC_STATUS_*）。
          自分がオフラインの時などに呼び出すとDWC_STATUS_OFFLINEしか返さない。
          また相手がまだGPのバディリストに登録されていない時も
          DWC_STATUS_OFFLINE返す。
  用途　：ローカルにキャッシュされた、指定の友達の通信状態を取得する。
  　　　　通信は行わない。
          友達の通信状態は、変化がある度にGPサーバから通知される。
          ただし、通信中に突然電源を切った場合などは数分間は前の状態が残る。
 *---------------------------------------------------------------------------*/
u8 DWC_GetFriendStatusSC(const DWCFriendData* friendData, u8* maxEntry, u8* numEntry, char* statusString)
{
    char valueStr[4];
    int  len;
    GPBuddyStatus status;

    // GPBudyStatusを取得する
    if (DWCi_GetFriendBuddyStatus(friendData, &status)){
        if (status.status == DWC_STATUS_MATCH_SC_SV){
            // 友達がサーバクライアントマッチメイクのサーバを立てている場合
            if (maxEntry){
                // 最大接続人数の設定を読み出す
                len = DWC_GetCommonValueString(DWC_GP_SSTR_KEY_MATCH_SC_MAX,
                                               valueStr,
                                               status.statusString, '/');
                if (len > 0) *maxEntry = (u8)strtoul(valueStr, NULL, 10);
                else *maxEntry = 0;
            }

            if (numEntry){
                // 現在接続人数の設定を読み出す
                len = DWC_GetCommonValueString(DWC_GP_SSTR_KEY_MATCH_SC_NUM,
                                               valueStr,
                                               status.statusString, '/');
                if (len > 0) *numEntry = (u8)strtoul(valueStr, NULL, 10);
                else *numEntry = 0;
            }
        }
        else {
            // 友達がサーバクライアントマッチメイクのサーバでない場合
            if (maxEntry) *maxEntry = 0;
            if (numEntry) *numEntry = 0;
        }

        if (statusString){
            // statusStringにlocationStringをセット
            strcpy(statusString, status.locationString);
        }

        return status.status;
    }
    else {
        // エラーの場合はOFFLINE扱い
        if (maxEntry) *maxEntry = 0;
        if (numEntry) *numEntry = 0;
        return DWC_STATUS_OFFLINE;
    }
}


/*---------------------------------------------------------------------------*
  友達通信状態取得関数（statusData版）
  引数　：friendData 友達情報へのポインタ
          statusData ゲームで定義したGameSpy statusデータ格納先ポインタ。
                     NULLを渡せばデータのコピーは行わない。
          size       ゲームで定義したGameSpy statusデータ長の格納先ポインタ。
                     無効なデータの場合は-1が格納される。
  戻り値：通信状態enum値（dwc_friend.h のDWC_STATUS_*）。
          自分がオフラインの時などに呼び出すとDWC_STATUS_OFFLINEしか返さない。
          また相手がまだGPのバディリストに登録されていない時も
          DWC_STATUS_OFFLINE返す。
  用途　：ローカルにキャッシュされた、指定の友達の通信状態を取得する。
  　　　　通信は行わない。
          友達の通信状態は、変化がある度にGPサーバから通知される。
          ただし、通信中に突然電源を切った場合などは数分間は前の状態が残る。
          戻り値がDWC_STATUS_OFFLINEの場合は、*size = -1 をセットする以外は
          値をセットせずに返します。
 *---------------------------------------------------------------------------*/
u8 DWC_GetFriendStatusData(const DWCFriendData* friendData, char* statusData, int* size)
{

    return DWC_GetFriendStatusDataSC(friendData, NULL, NULL, statusData, size);
}


/*---------------------------------------------------------------------------*
  友達通信状態取得関数（サーバクライアントマッチメイクゲーム用statusData版）
  引数　：friendData 友達情報へのポインタ
          maxEntry   友達がサーバクライアントマッチメイクのサーバを立てて
                     いる場合の最大接続人数。
                     格納先ポインタ。NULLを渡せば取得しない。
          numEntry   友達がサーバクライアントマッチメイクのサーバを立てて
                     いる場合の現在接続人数。
                     格納先ポインタ。NULLを渡せば取得しない。
          statusData ゲームで定義したGameSpy status文字列格納先ポインタ。
                     NULLを渡せば文字列のコピーは行わない。
          size       ゲームで定義したGameSpy statusデータ長の格納先ポインタ。
                     無効なデータの場合は-1が格納される。
  戻り値：通信状態enum値（dwc_friend.h のDWC_STATUS_*）。
          自分がオフラインの時などに呼び出すとDWC_STATUS_OFFLINEしか返さない。
          また相手がまだGPのバディリストに登録されていない時も
          DWC_STATUS_OFFLINE返す。
  用途　：ローカルにキャッシュされた、指定の友達の通信状態を取得する。
  　　　　通信は行わない。
          友達の通信状態は、変化がある度にGPサーバから通知される。
          ただし、通信中に突然電源を切った場合などは数分間は前の状態が残る。
          戻り値がDWC_STATUS_OFFLINEの場合は、*size = -1 をセットする以外は
          値をセットせずに返します。
 *---------------------------------------------------------------------------*/
u8 DWC_GetFriendStatusDataSC(const DWCFriendData* friendData, u8* maxEntry, u8* numEntry, char* statusData, int* size)
{
    u8   ret;
    char statusString[DWC_GS_STATUS_STRING_LEN];

    ret = DWC_GetFriendStatusSC(friendData, maxEntry, numEntry, statusString);

    if (ret == DWC_STATUS_OFFLINE){
        *size = -1;
        return ret;
    }

    // まずはバイナリデータのサイズを調べるだけ
    *size = DWC_Auth_Base64Decode(statusString, strlen(statusString), NULL, 0);

    if (!statusData || (*size == -1)) return ret;

    // テキストデータをバイナリデータに変換して格納する
    (void)DWC_Auth_Base64Decode(statusString, strlen(statusString), statusData, (u32)*size);

    return ret;
}


/*---------------------------------------------------------------------------*
  友達数取得関数
  引数　：friendList    友達リスト
          friendListLen 友達リストの最大長（要素数）
  戻り値：友達リストに登録されている友達の数
  用途　：ローカルの友達リストを参照して、友達登録数を返す。通信は行わない。
 *---------------------------------------------------------------------------*/
int  DWC_GetNumFriend(const DWCFriendData friendList[], int friendListLen)
{
    int count = 0;
    int i;

    if (!friendList) return 0;

    for (i = 0; i < friendListLen; i++){
        // [note]
        // この判定だと、ゲームコード違いの（場合によっては無効な）
        // 友達情報もカウントしてしまう
        if (DWCi_Acc_IsValidFriendData(&friendList[i])) count++;
    }

    return count;
}


/*---------------------------------------------------------------------------*
  GameSpy Status文字列セット関数
  引数　：statusString GameSpyのStatusにセットする文字列。255文字＋NULL終端まで。
  戻り値：TRUE :セット成功
          FALSE:ログイン前などでセット失敗
  用途　：自分のGameSpy Statusに文字列をセットする。
          セットされた文字列は、他のホストからもDWC_GetFriendStatus()で
          取得することができる。
          '/'と'\\'は識別文字としてライブラリが使うので、文字列中に
          使用しないで下さい。
          セットされた文字列は、DWC_ShutdownFriendsMatch()が呼ばれると
          クリアされます。
 *---------------------------------------------------------------------------*/
BOOL DWC_SetOwnStatusString(const char* statusString)
{
    
    if ((stpFriendCnt == NULL) || !DWCi_CheckLogin()) return FALSE;
    
    if (DWCi_SetGPStatus(DWC_GP_STATUS_NO_CHANGE, NULL, statusString)) return FALSE;
    else return TRUE;
}


/*---------------------------------------------------------------------------*
  GameSpy Status文字列取得関数
  引数　：statusString GameSpyのStatus文字列格納先ポインタ。255文字＋NULL終端まで。
  戻り値：TRUE :取得成功、FALSE:ログイン前などで取得失敗
  用途　：現在の自分のGameSpy Status文字列を取得する。
 *---------------------------------------------------------------------------*/
BOOL DWC_GetOwnStatusString(char* statusString)
{

    if ((stpFriendCnt == NULL) || (*stpFriendCnt->pGpObj == NULL) || !statusString) return FALSE;

    // [note]
    // gpi.hに記述されているGPIConnection構造体を使っている
    strcpy(statusString, ((GPIConnection *)*stpFriendCnt->pGpObj)->lastLocationString);

    return TRUE;
}


/*---------------------------------------------------------------------------*
  GameSpy Statusデータセット関数
  引数　：statusData GameSpyのStatusにセットするバイナリデータへのポインタ。
          size       バイナリデータのサイズ。189Byteまで。
  戻り値：TRUE :セット成功
          FALSE:ログイン前、容量オーバーなどでセット失敗
  用途　：自分のGameSpy StatusにバイナリデータをBase64でテキスト化して
          セットする。
          セットされたデータは、他のホストからDWC_GetFriendStatusData()で
          取得することができる。
          セットされたデータは、DWC_ShutdownFriendsMatch()が呼ばれると
          クリアされます。
 *---------------------------------------------------------------------------*/
BOOL DWC_SetOwnStatusData(const char* statusData, u32 size)
{
    char statusString[DWC_GS_STATUS_STRING_LEN];
    int  len;
    
    if (!stpFriendCnt || !DWCi_CheckLogin()) return FALSE;

    // テキスト変換を行う
    len = DWC_Auth_Base64Encode(statusData, size,
                                statusString, DWC_GS_STATUS_STRING_LEN-1);

    if (len == -1) return FALSE;  // 文字数が限界オーバーなら失敗

    statusString[len] = '\0';     // NULL終端を保証する
    
    if (DWCi_SetGPStatus(DWC_GP_STATUS_NO_CHANGE, NULL, statusString)) return FALSE;
    else return TRUE;
}


/*---------------------------------------------------------------------------*
  GameSpy Statusデータ取得関数
  引数　：statusData GameSpyのStatusデータ格納先ポインタ
  戻り値：-1:ログイン前、不正なデータなどで取得失敗、
          非負整数:取得したバイナリデータのサイズ
  用途　：現在の自分のGameSpy Statusデータを取得する。
          引数statusDataにNULLを渡すと、バイナリデータのサイズのみ取得する。
 *---------------------------------------------------------------------------*/
int DWC_GetOwnStatusData(char* statusData)
{
    int size;

    if (!stpFriendCnt || !DWCi_CheckLogin()) return FALSE;

    // [note]
    // gpi.hに記述されているGPIConnection構造体を使っている

    // まずはバイナリデータのサイズを調べるだけ
    size = DWC_Auth_Base64Decode(((GPIConnection *)*stpFriendCnt->pGpObj)->lastLocationString,
                                 strlen(((GPIConnection *)*stpFriendCnt->pGpObj)->lastLocationString),
                                 NULL, 0);

    if (!statusData || (size == -1)) return size;

    // テキストデータをバイナリデータに変換して格納する
    return DWC_Auth_Base64Decode(((GPIConnection *)*stpFriendCnt->pGpObj)->lastLocationString,
                                 strlen(((GPIConnection *)*stpFriendCnt->pGpObj)->lastLocationString),
                                 statusData, (u32)size);
}


/*---------------------------------------------------------------------------*
  友達リスト編集可能状態取得関数
  引数　：なし
  戻り値：TRUE:友達リストを編集して良い、FALSE:友達リストを編集してはいけない
  用途　：友達リストを編集しても良いタイミングかどうかを調べる。
          ここでの戻り値は、次にDWC_ProcessFriendsMatch()が呼ばれるまで有効。
 *---------------------------------------------------------------------------*/
BOOL DWC_CanChangeFriendList(void)
{

    if ((stpFriendCnt != NULL) &&
        ((stpFriendCnt->buddyUpdateState == DWC_BUDDY_UPDATE_STATE_CHECK) ||
         (stpFriendCnt->buddyUpdateState == DWC_BUDDY_UPDATE_STATE_PSEARCH))){
        // DWCi_UpdateFriendReq()の処理が完全に終わるまでは、
        // 友達リストのデータをずらされたりするとチェックできない友達情報が
        // 残ってしまったりするので、この間は編集禁止とする
        return FALSE;
    }
    else {
        return TRUE;
    }
}


/*---------------------------------------------------------------------------*
  友達情報完全消去関数
  引数　：friendData 友達情報へのポインタ
  戻り値：なし
  用途　：友達情報をゼロクリアし、GPサーバ上のバディ関係も消去する。
 *---------------------------------------------------------------------------*/
void DWC_DeleteBuddyFriendData(DWCFriendData* friendData)
{
    int  profileID;

    if ((stpFriendCnt != NULL)          &&
        DWCi_CheckLogin()               &&
        (DWCi_GetUserData() != NULL)    &&
        ((profileID = DWC_GetGsProfileId(DWCi_GetUserData(), friendData)) != 0) &&
        (profileID != -1) &&
        gpIsBuddy(stpFriendCnt->pGpObj, profileID)){
        // 友達情報に有効なプロファイルIDが入っており、そのプロファイルIDと
        // GP上のバディである場合
        // バディを消去
        (void)gpDeleteBuddy(stpFriendCnt->pGpObj, profileID);

        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG,
                   "DWC_DeleteBuddyFriendData : Deleted buddy.\n");
    }
    else {
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG,
                   "DWC_DeleteBuddyFriendData : Only clear data.\n");
    }

    // 友達情報をゼロクリア
    MI_CpuClear8(friendData, sizeof(DWCFriendData));
}


/*---------------------------------------------------------------------------*
  バディ成立コールバック登録関数
  引数　：callback バディ成立コールバック
          param    コールバック用パラメータ
  戻り値：TRUE :登録成功
          FALSE:FriendsMatchライブラリ非動作中で登録失敗
  用途　：ローカルの友達リストに登録された友達が、DWC_UpdateServersAsync()
          呼出し中以外のタイミングで、GPサーバ上でもバディとして
          認められた時に呼び出されるコールバックを登録する
 *---------------------------------------------------------------------------*/
BOOL DWC_SetBuddyFriendCallback(DWCBuddyFriendCallback callback, void* param)
{

    if (!stpFriendCnt) return FALSE;

    stpFriendCnt->buddyCallback = callback;
    stpFriendCnt->buddyParam    = param;

    return TRUE;
}


/*---------------------------------------------------------------------------*
  友達状態変化通知コールバック登録関数
  引数　：callback 友達状態変化通知コールバック
          param    コールバック用パラメータ。NULLを渡せば何もセットしない。
                   コールバック用パラメータのデフォルト値はNULL。
  戻り値：TRUE :登録成功
          FALSE:FriendsMatchライブラリ非動作中で登録失敗
  用途　：友達の通信状態が変化した時に通知されるコールバックを登録する。
          通常はDWC_UpdateServersAsync()で登録できるが、
          DWC_UpdateServersAsync()を使わないが友達の状態は取得したいという
          場合に本関数を用いる。
 *---------------------------------------------------------------------------*/
BOOL DWC_SetFriendStatusCallback(DWCFriendStatusCallback callback, void* param)
{

    if (!stpFriendCnt) return FALSE;

    stpFriendCnt->statusCallback  = callback;
    if (param != NULL){
        stpFriendCnt->statusParam = param;
    }

    return TRUE;
}


/*---------------------------------------------------------------------------*
  Base64エンコード関数
  引数　：src    変換元データへのポインタ
          srclen 変換元データ長
          dst    変換後データ格納用バッファへのポインタ。
                 NULLを指定すれば変換後に必要な最大バッファサイズを返すだけ。
          dstlen 変換後データ格納用バッファのサイズ（文字列長）
  戻り値：-1      :変換失敗。必要な変換後バッファサイズがdstlenを越えている。
          それ以外:変換結果の文字列長
  用途　：与えられたデータをBase64でエンコードし、char型の文字列として出力する
 *---------------------------------------------------------------------------*/
int DWC_Base64Encode(const char *src, const u32 srclen, char *dst, const u32 dstlen)
{
    return DWC_Auth_Base64Encode(src, srclen, dst, dstlen);
}


/*---------------------------------------------------------------------------*
  Base64デコード関数
  引数　：src    Base64データへのポインタ
          srclen Base64データ長（文字列長）
          dst    変換後データ格納用バッファへのポインタ。
                 NULLを指定すれば変換後に必要な最大バッファサイズを返すだけ。
          dstlen 変換後データ格納用バッファのサイズ
  戻り値：-1      :変換失敗。必要な変換後バッファサイズがdstlenを越えている。
          それ以外:変換結果のデータサイズ
  用途　：与えられたBase64データをデコードし、char型のデータとして出力する
 *---------------------------------------------------------------------------*/
int DWC_Base64Decode(const char *src, const u32 srclen, char *dst, const u32 dstlen)
{
    return DWC_Auth_Base64Decode(src, srclen, dst, dstlen);
}

/*---------------------------------------------------------------------------*
  データストレージサーバへのログイン関数
  引数　：callback ログイン完了通知コールバック
          param    コールバック用パラメータ
  戻り値：TRUE :ログイン開始。成功か失敗でコールバックが返ってくる。
          FALSE:DWC_LoginAsync()完了前、既にログイン済みなどでログインを
                開始しなかった。コールバックは返ってこない。
  用途　：GameSpyデータストレージサーバへのログインを行なう。
          ログインしないとデータストレージサーバへのセーブ・ロードはできない。
 *---------------------------------------------------------------------------*/
BOOL DWC_LoginToStorageServerAsync(DWCStorageLoginCallback callback, void* param)
{
    char response[33];
    char *authToken, *partnerChallenge;
    int  persResult;
    int  i;

    // まだGPサーバにログインしてないか、既にPersistentサーバにログイン済みなら
    // 何もせずに戻る
    if (!DWCi_GetAuthInfo(&authToken, &partnerChallenge) || IsStatsConnected())
        return FALSE;

    stpFriendCnt->persLoginCallback = callback;
    stpFriendCnt->persLoginParam    = param;

    for (i = 0; i < DWC_DNS_ERROR_RETRY_MAX; i++){
		stPersState = DWC_PERS_STATE_LOGIN;
        persResult = InitStatsConnection(0);  // stats＆persistentSDK共通初期化
    
        if (persResult == GE_NOERROR) break;
    
        // DNSエラーなら一定回数はリトライする
        if ((persResult != GE_NODNS) || (i == DWC_DNS_ERROR_RETRY_MAX-1)){
            DWCi_HandlePersError(persResult);
            return TRUE;  // コールバックでエラーを返す
        }
    }

    // Persistentサーバへの認証を行う
    (void)GenerateAuth(GetChallenge(NULL), partnerChallenge, response);
    PreAuthenticatePlayerPartner(0, authToken, response,
                                 DWCi_PersAuthCallback, NULL);

    DWCi_AddPersCallbackLevel();

    return TRUE;
}


/*---------------------------------------------------------------------------*
  データストレージサーバからのログアウト関数
  引数　：なし
  戻り値：なし
  用途　：GameSpyデータストレージサーバからログアウトし、
          制御オブジェクトを解放する。
          この関数を呼ばなくても、DWC_ShutdownFriendsMatch()を呼べば、
          ログアウト処理が行なわれる。
 *---------------------------------------------------------------------------*/
void DWC_LogoutFromStorageServer(void)
{
    if(s_bInPersistThink == TRUE)
    {
        s_bNeedCloseStatsConnection = TRUE;
    }
    else
    {
        CloseStatsConnection();  // Persistentオブジェクトの開放
    }

    stPersState = DWC_PERS_STATE_INIT;

    if (stpFriendCnt != NULL) stpFriendCnt->persCallbackLevel = 0;
}


/*---------------------------------------------------------------------------*
  データストレージサーバ データセーブ・ロード完了通知コールバック登録関数
  引数　：saveCallback セーブ完了通知コールバック
          loadCallback ロード完了通知コールバック
  戻り値：TRUE :登録成功
          FALSE:FriendsMatchライブラリ非動作中で登録失敗
  用途　：データストレージへのセーブ・ロードが完了した時に呼び出される
          コールバックを登録する
 *---------------------------------------------------------------------------*/
BOOL DWC_SetStorageServerCallback(DWCSaveToServerCallback saveCallback,
                                  DWCLoadFromServerCallback loadCallback)
{

    if (!stpFriendCnt) return FALSE;

    stpFriendCnt->saveCallback = saveCallback;
    stpFriendCnt->loadCallback = loadCallback;

    return TRUE;
}


/*---------------------------------------------------------------------------*
  データストレージサーバPublicデータセーブ関数
  引数　：keyvalues データベース文字列（key/value値）へのポインタ
          param     コールバック用パラメータ
  戻り値：TRUE:セーブ開始、FALSE：オフライン状態か通信エラー中でセーブできない
  用途　：誰でも参照可能なデータをデータストレージサーバにセーブする。
          セーブするデータはNULL終端の文字列で、key/valueの組である必要がある。
          （"\\name\\mario\\stage\\3"のように'\\'で区切って記述する。
          この例のように複数のkey/value組を一度にセーブすることができる）
          データのセットが完了したらコールバックが呼ばれる。
 *---------------------------------------------------------------------------*/
BOOL DWC_SavePublicDataAsync(char* keyvalues, void* param)
{

    if ((stPersState != DWC_PERS_STATE_CONNECTED) || DWCi_IsError() || !stpFriendCnt) return FALSE;

    DWCi_SetPersistDataValuesAsync(stpFriendCnt->profileID, pd_public_rw,
                                   keyvalues, param);

    return TRUE;
}


/*---------------------------------------------------------------------------*
  データストレージサーバPrivateデータセーブ関数
  引数　：keyvalues セーブしたいデータベース文字列（key/value値）へのポインタ
          param     コールバック用パラメータ
  戻り値：TRUE:セーブ開始、FALSE：オフライン状態か通信エラー中でセーブできない
  用途　：本人のみ参照可能なデータをデータストレージサーバにセーブする。
          セーブするデータはNULL終端の文字列で、key/valueの組である必要がある。
          （"\\name\\mario\\stage\\3"のように'\\'で区切って記述する。
          この例のように複数のkey/value組を一度にセーブすることができる）
          データのセットが完了したらコールバックが呼ばれる。
 *---------------------------------------------------------------------------*/
BOOL DWC_SavePrivateDataAsync(char* keyvalues, void* param)
{

    if ((stPersState != DWC_PERS_STATE_CONNECTED) || DWCi_IsError() || !stpFriendCnt) return FALSE;

    DWCi_SetPersistDataValuesAsync(stpFriendCnt->profileID, pd_private_rw,
                                   keyvalues, param);

    return TRUE;
}


/*---------------------------------------------------------------------------*
  データストレージサーバ 自分のPublicデータロード関数
  引数　：keys  ロードしたいデータのkey文字列へのポインタ
          param コールバック用パラメータ
  戻り値：TRUE:ロード開始、FALSE：オフライン状態か通信エラー中でロードできない
  用途　：データストレージサーバからPublicデータをロードする。
          ロードするデータはkeyで指定し、NULL終端の文字列である必要がある。
          （"\\name\\stage"のように'\\'で区切って記述する。
          この例のように複数のkeyを一度にロードすることができる）
          データのロードが完了したらコールバックが呼ばれる。
 *---------------------------------------------------------------------------*/
BOOL DWC_LoadOwnPublicDataAsync(char* keys, void* param)
{

    if ((stPersState != DWC_PERS_STATE_CONNECTED) || DWCi_IsError() || !stpFriendCnt) return FALSE;

    DWCi_GetPersistDataValuesAsync(stpFriendCnt->profileID, pd_public_rw, keys, param);

    return TRUE;
}


/*---------------------------------------------------------------------------*
  データストレージサーバ 自分のPrivateデータロード関数
  引数　：keys  ロードしたいデータのkey文字列へのポインタ
          param コールバック用パラメータ
  戻り値：TRUE:ロード開始、FALSE：オフライン状態か通信エラー中でロードできない
  用途　：データストレージサーバからPrivateデータをロードする。
          ロードするデータはkeyで指定し、NULL終端の文字列である必要がある。
          （"\\name\\stage"のように'\\'で区切って記述する。
          この例のように複数のkeyを一度にロードすることができる）
          データのロードが完了したらコールバックが呼ばれる。
 *---------------------------------------------------------------------------*/
BOOL DWC_LoadOwnPrivateDataAsync(char* keys, void* param)
{

    if ((stPersState != DWC_PERS_STATE_CONNECTED) || DWCi_IsError() || !stpFriendCnt) return FALSE;

    DWCi_GetPersistDataValuesAsync(stpFriendCnt->profileID, pd_private_rw, keys, param);

    return TRUE;
}


/*---------------------------------------------------------------------------*
  データストレージサーバ 他人のPublicデータロード関数
  引数　：keys  ロードしたいデータのkey文字列へのポインタ
          index ロードしたいユーザの友達リストインデックス
          param コールバック用パラメータ
  戻り値：TRUE:ロード開始、
          FALSE：自分がオフライン状態か相手が友達でない、通信エラー中でロードできない
  用途　：データストレージサーバからPublicデータをロードする。
          ロードするデータはkeyで指定し、NULL終端の文字列である必要がある。
          （"\\name\\stage"のように'\\'で区切って記述する。
          この例のように複数のkeyを一度にロードすることができる）
          データのロードが完了したらコールバックが呼ばれる。
 *---------------------------------------------------------------------------*/
BOOL DWC_LoadOthersDataAsync(char* keys, int index, void* param)
{

    if ((stPersState != DWC_PERS_STATE_CONNECTED) || DWCi_IsError() || !stpFriendCnt ||
        !DWCi_GetProfileIDFromList(index))
        return FALSE;

    DWCi_GetPersistDataValuesAsync(DWCi_GetProfileIDFromList(index), pd_public_rw, keys, param);

    return TRUE;
}


//----------------------------------------------------------------------------
// function - internal
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  友達管理構造体初期化関数
  引数　：friendcnt     友達管理オブジェクトへのポインタ
          pGpObj        gpコネクション構造体ポインタへのポインタ
          playerName    他のユーザも参照する自分のプレイヤー名へのポインタ
          friendList    友達リストへのポインタ
          friendListLen 友達リストの最大長（要素数）
  戻り値：なし
  用途　：友達管理構造体を初期化する
 *---------------------------------------------------------------------------*/
void DWCi_FriendInit(DWCFriendControl* friendcnt,
                     GPConnection* pGpObj,
                     const u16* playerName,
                     DWCFriendData* friendList,
                     int friendListLen)
{

    SDK_ASSERT(friendcnt);

    stpFriendCnt = friendcnt;

    stpFriendCnt->state              = DWC_FRIEND_STATE_INIT;
    stpFriendCnt->pGpObj             = pGpObj;
    stpFriendCnt->gpProcessCount     = 0;
    stpFriendCnt->lastGpProcess      = 0;
    stpFriendCnt->friendListLen      = friendListLen;
    stpFriendCnt->friendList         = friendList;
    stpFriendCnt->buddyUpdateIdx     = 0;
    stpFriendCnt->friendListChanged  = 0;
    stpFriendCnt->buddyUpdateState   = DWC_BUDDY_UPDATE_STATE_WAIT;
    stpFriendCnt->svUpdateComplete   = 0;
    stpFriendCnt->persCallbackLevel  = 0;
    stpFriendCnt->profileID          = 0;
    stpFriendCnt->playerName         = playerName;
    stpFriendCnt->updateCallback     = NULL;
    stpFriendCnt->updateParam        = NULL;
    stpFriendCnt->statusCallback     = NULL;
    stpFriendCnt->statusParam        = NULL;
    stpFriendCnt->deleteCallback     = NULL;
    stpFriendCnt->deleteParam        = NULL;
    stpFriendCnt->buddyCallback      = NULL;
    stpFriendCnt->buddyParam         = NULL;
    stpFriendCnt->persLoginCallback  = NULL;
    stpFriendCnt->persLoginParam     = NULL;
    stpFriendCnt->saveCallback       = NULL;
    stpFriendCnt->loadCallback       = NULL;
}


/*---------------------------------------------------------------------------*
  友達管理更新関数
  引数　：なし
  戻り値：なし
  用途　：毎ゲームフレーム呼び出し、通信処理を更新する
 *---------------------------------------------------------------------------*/
void DWCi_FriendProcess(void)
{
    GPResult gpResult;

    if (!stpFriendCnt || DWCi_IsError())
        return;
    else if (!stpFriendCnt->friendList)
    {
        // GP 通信処理更新
        if (stpFriendCnt->pGpObj && *stpFriendCnt->pGpObj)
        {
            gpResult = DWCi_GPProcess();
            // GPエラーの処理はgpProcess()から呼ばれるエラーコールバック内で
            // 行われる
        }
        return;
    }

    if (DWCi_GetPersCallbackLevel() || IsStatsConnected()){
        // Persistentサーバへの接続中、もしくはPersistentのコールバック待ち中
        // ならPersistent の通信処理更新

        s_bInPersistThink = TRUE; // PersistThink()呼び出し中であることを記録
        s_bNeedCloseStatsConnection = FALSE;

        if (!PersistThink()){
            s_bInPersistThink = FALSE;
            DWC_Printf(DWC_REPORTFLAG_WARNING,
                       "Connection to the stats server was lost\n");

            // [note]
            // ここの戻り値処理は、ServerBrowsing SDK と同様で必要ない
#if 0
            DWCi_StopFriendProcess(DWC_ERROR_NETWORK,

DWC_ECODE_SEQ_FRIEND+DWC_ECODE_GS_PERS+DWC_ECODE_TYPE_NETWORK);
            return;
#endif
        }

        s_bInPersistThink = FALSE;

        if(s_bNeedCloseStatsConnection == TRUE)
        {
            s_bNeedCloseStatsConnection = FALSE;
            CloseStatsConnection();  // Persistentオブジェクトの開放
        }
    }

    // GP 通信処理更新
    if (stpFriendCnt->pGpObj && *stpFriendCnt->pGpObj){
        gpResult = DWCi_GPProcess();

        // GPエラーの処理はgpProcess()から呼ばれるエラーコールバック内で
        // 行われるため、ここではエラーを検出したら何もせずに抜けるだけで良い
        if (gpResult != GP_NO_ERROR) return;
		
		// 初期状態に戻っているのにGP処理を続行するとFATALエラーになるため回避コードを追加
        if(stpFriendCnt->state == DWC_FRIEND_STATE_INIT) return; 
		
        // GPサーバログイン後、一度gpProcess()が呼ばれないとローカルの
        // バディリストは更新されないので、ここで友達リストの
        // 登録・消去処理を呼び出す。

		// 以下のコードはDWCi_UpdateServersAsyncが呼ばれ、ステートが所定のものにならない場合は実行されない
        if (stpFriendCnt->friendList &&
            (stpFriendCnt->buddyUpdateState != DWC_BUDDY_UPDATE_STATE_COMPLETE) &&
            (stpFriendCnt->gpProcessCount > DWC_FRIEND_UPDATE_WAIT_COUNT)){
            if ((stpFriendCnt->buddyUpdateState == DWC_BUDDY_UPDATE_STATE_WAIT) ||
                (stpFriendCnt->buddyUpdateState == DWC_BUDDY_UPDATE_STATE_CHECK)){
                // バディリストとローカル友達リストの同期処理
                DWCi_UpdateFriendReq(stpFriendCnt->friendList,
                                     stpFriendCnt->friendListLen);
            }

            if (stpFriendCnt->buddyUpdateIdx >= stpFriendCnt->friendListLen){
                // アップデート完了
                stpFriendCnt->buddyUpdateState = DWC_BUDDY_UPDATE_STATE_COMPLETE;
                stpFriendCnt->svUpdateComplete++;
            }
        }
    }

    if (stpFriendCnt->svUpdateComplete >= 2){
        // 友達リスト同期処理が完了したら、終了処理を呼び出す
        stpFriendCnt->svUpdateComplete = 0;
        DWCi_EndUpdateServers();
    }
}


/*---------------------------------------------------------------------------*
  友達リスト同期処理関数
  引数　：authToken          認証に使ったトークンへのポインタ
          partnerChallenge   認証に使ったチャレンジ文字列へのポインタ
          updateCallback     友達同期処理完了コールバック
          updateParam        上記コールバック用パラメータ
          statusCallback     友達状態変化通知コールバック
          statusParam        上記コールバック用パラメータ
          deleteCallback     友達リスト削除コールバック
          deleteParam        上記コールバック用パラメータ
  戻り値：なし
  用途　：ログイン完了後、GPサーバ上の友達リスト（バディリスト）と
          ローカルの友達リストの同期処理を行う
 *---------------------------------------------------------------------------*/
void DWCi_UpdateServersAsync(const char* authToken,
                             const char* partnerChallenge,
                             DWCUpdateServersCallback updateCallback,
                             void* updateParam,
                             DWCFriendStatusCallback statusCallback,
                             void* statusParam,
                             DWCDeleteFriendListCallback deleteCallback,
                             void* deleteParam)
{
#if 0  // 051007 Persistentサーバには必ずアクセスしないようにしておく
    char response[33];
    int  persResult;
    int  i;
#else
#pragma unused(authToken)
#pragma unused(partnerChallenge)
#endif // 051007 Persistentサーバには必ずアクセスしないようにしておく

    SDK_ASSERT(updateCallback);

    stpFriendCnt->updateCallback = updateCallback;
    stpFriendCnt->updateParam    = updateParam;
    stpFriendCnt->statusCallback = statusCallback;
    stpFriendCnt->statusParam    = statusParam;
    stpFriendCnt->deleteCallback = deleteCallback;
    stpFriendCnt->deleteParam    = deleteParam;

    stpFriendCnt->friendListChanged = 0;  // 友達リストの変更フラグを初期化
    // 次のgpProcess()呼出し後に、友達リスト登録・消去要求を処理する
    stpFriendCnt->buddyUpdateState  = DWC_BUDDY_UPDATE_STATE_WAIT;
    stpFriendCnt->svUpdateComplete  = 0;  // 友達リスト同期処理完了フラグクリア
    stpFriendCnt->buddyUpdateIdx    = 0;
    
    // Persistentサーバログイン状態に移行
    stpFriendCnt->state = DWC_FRIEND_STATE_PERS_LOGIN;

    // [arakit] main 051020
    if (stpFriendCnt->friendList == NULL){
        // 友達リストを使わない場合は友達リストの同期処理が終了したことにする
        stpFriendCnt->svUpdateComplete++; 
    }
    // [arakit] main 051020

#if 0  // 051007 Persistentサーバには必ずアクセスしないようにしておく
    if (stpFriendCnt->playerName[0] != '\0'){
        // プレイヤー名がセットされていたらPersistentサーバにセットする
        for (i = 0; i < DWC_DNS_ERROR_RETRY_MAX; i++){
            persResult = InitStatsConnection(0);  // stats＆persistentSDK共通初期化
    
            if (!persResult) break;
    
            // DNSエラーなら一定回数はリトライする
            if ((persResult != GE_NODNS) || (i == DWC_DNS_ERROR_RETRY_MAX-1)){
                DWCi_HandlePersError(persResult);
                return;  // コールバックでエラーを返す
            }
        }

        // Persistentサーバへの認証を行う
        (void)GenerateAuth(GetChallenge(NULL), partnerChallenge, response);
        PreAuthenticatePlayerPartner(0, authToken, response,
                                     DWCi_PersAuthCallback, NULL);

        DWCi_AddPersCallbackLevel();
    }
    else {
#endif // 051007 Persistentサーバには必ずアクセスしないようにしておく
        // プレイヤー名がセットされていなければPersistentの処理が
        // 終了したことを記録してすぐに終わる
        stpFriendCnt->svUpdateComplete++;
#if 0  // 051007 Persistentサーバには必ずアクセスしないようにしておく
    }
#endif // 051007 Persistentサーバには必ずアクセスしないようにしておく
}


/*---------------------------------------------------------------------------*
  友達管理処理エラー終了関数
  引数　：error     DWCエラー種別
          errorCode エラーコード
  戻り値：なし
  用途　：友達管理処理中にエラーが発生した場合に、処理を中断する
 *---------------------------------------------------------------------------*/
void DWCi_StopFriendProcess(DWCError error, int errorCode)
{

    if (!stpFriendCnt || (error == DWC_ERROR_NONE)) return;

    DWCi_SetError(error, errorCode);

    if ((stpFriendCnt->state != DWC_FRIEND_STATE_INIT) &&
        (stpFriendCnt->state != DWC_FRIEND_STATE_LOGON)){
        // 友達リスト同期処理中ならコールバック関数をエラーで呼び出す
        stpFriendCnt->updateCallback(error, stpFriendCnt->friendListChanged,
                                     stpFriendCnt->updateParam);
    }

    DWCi_CloseFriendProcess();
}


/*---------------------------------------------------------------------------*
  GPバディリクエスト受信コールバック関数
  引数　：connection GPコネクションオブジェクトへのポインタ
          arg        GPRecvBuddyRequestArg型オブジェクトへのポインタ
          param      コールバック用パラメータ
  戻り値：なし
  用途　：GPバディリクエスト受信コールバック。
          DWC_GetUserData()を使うので、DWC_InitFriendsMatch()より前に呼ぶように
          変更してはならない。
 *---------------------------------------------------------------------------*/
void DWCi_GPRecvBuddyRequestCallback(GPConnection* connection,
                                     GPRecvBuddyRequestArg* arg,
                                     void* param)
{
#pragma unused(param)

    DWC_Printf(DWC_REPORTFLAG_UPDATE_SV, "Received buddy request from %u\n",
               arg->profile);

    if (!stpFriendCnt->friendList) return;

    DWC_Printf(DWC_REPORTFLAG_UPDATE_SV, "Begin to search gpInfo.\n");

    // 既にプロファイルIDを持っている友達であったとしても、
    // 必ずgpGetInfoでプロファイルIDからlastnameを取得する
    gpGetInfo(connection, arg->profile, GP_DONT_CHECK_CACHE,
              GP_NON_BLOCKING, (GPCallback)DWCi_GPGetInfoCallback_RecvBuddyRequest, NULL );
}

/*---------------------------------------------------------------------------*
  GPバディレスポンス受信コールバック関数
  引数　：connection GPコネクションオブジェクトへのポインタ
          arg        GPRecvBuddyMessageArg型オブジェクトへのポインタ
          param      コールバック用パラメータ
  戻り値：GPバディリクエストの返答ならTRUE
  用途　：GPバディレスポンス受信コールバック。DWCi_GPRecvBuddyMessageCallbackより
          呼び出さされる。
 *---------------------------------------------------------------------------*/
BOOL DWCi_GPRecvBuddyAuthCallback( GPConnection* connection,
                                   GPRecvBuddyMessageArg* arg,
                                   void* param )
{
#pragma unused(param)
    char* message = arg->message;

    if (!strcmp(message, "I have authorized your request to add me to your list"))
    {
        // 既にプロファイルIDを持っている友達であったとしても、
        // 必ずgpGetInfoでプロファイルIDからlastnameを取得する
        DWC_Printf(DWC_REPORTFLAG_UPDATE_SV,
                   "Received buddy authenticated message from %u.\n",
                   arg->profile);
        DWC_Printf(DWC_REPORTFLAG_UPDATE_SV, "Begin to search gpInfo.\n");
            
        gpGetInfo(connection, arg->profile, GP_DONT_CHECK_CACHE,
                  GP_NON_BLOCKING, (GPCallback)DWCi_GPGetInfoCallback_RecvAuthMessage, NULL );
        return TRUE;
    }

    return FALSE;
}


/*---------------------------------------------------------------------------*
  GPバディ状態変化受信コールバック関数
  引数　：connection GPコネクションオブジェクトへのポインタ
          arg        GPRecvBuddyStatusArg型オブジェクトへのポインタ
          param      コールバック用パラメータ
  戻り値：なし
  用途　：GPバディ状態変化受信コールバック
          DWC_GetUserData()を使うので、DWC_InitFriendsMatch()より前に呼ぶように
          変更してはならない。
 *---------------------------------------------------------------------------*/
void DWCi_GPRecvBuddyStatusCallback(GPConnection* connection,
                                    GPRecvBuddyStatusArg* arg,
                                    void* param)
{
#pragma unused(param)
    int friendIdx;
    GPBuddyStatus status;

    DWC_Printf(DWC_REPORTFLAG_UPDATE_SV, "RECV update friend status. p:%d\n", arg->profile);

    if (stpFriendCnt->statusCallback){
        friendIdx = DWCi_GetFriendListIndex(arg->profile);

        if (friendIdx == -1){
            // 万が一微妙なタイミングでローカルの友達リストが消去されていた場合
            return;
        }

        // エラーはないはず
        (void)gpGetBuddyStatus(connection, arg->index, &status);

        // ユーザが登録したコールバックを呼び出す
        stpFriendCnt->statusCallback(friendIdx,
                                     (u8)status.status,
                                     status.locationString,
                                     stpFriendCnt->statusParam);
    }
}


/*---------------------------------------------------------------------------*
  友達リスト取得関数
  引数　：なし
  戻り値：友達リストへのポインタ。友達管理オブジェクトがなければNULL。
  用途　：友達管理オブジェクトが保持している友達リストへのポインタを取得する
 *---------------------------------------------------------------------------*/
DWCFriendData* DWCi_GetFriendList(void)
{

    if (stpFriendCnt){
        return stpFriendCnt->friendList;
    }
    else {
        return NULL;
    }
}


/*---------------------------------------------------------------------------*
  友達リスト長取得関数
  引数　：なし
  戻り値：友達リスト長。友達管理オブジェクトがなければ0。
  用途　：友達管理オブジェクトが保持している友達リスト長を取得する
 *---------------------------------------------------------------------------*/
int DWCi_GetFriendListLen(void)
{

    if (stpFriendCnt){
        return stpFriendCnt->friendListLen;
    }
    else {
        return 0;
    }
}


/*---------------------------------------------------------------------------*
  友達リストからプロファイルIDを取得する関数
  引数　：index プロファイルIDを取得したい友達の友達リストインデックス
  戻り値：0:リストに無効なデータ（空き含む）か、ログインIDしか入っていない、
  　　　　　もしくは友達リストがない
          非0:友達のプロファイルID
  用途　：ローカルの友達リストから、指定したインデックスの友達情報の
          プロファイルIDを取得する。
          この関数はリストが空きなのか、ゲームコード違いなのか、ログインID
          なのかを区別しないので、とりあえずプロファイルIDがあるなら
          取得したいという時に使う。
          DWC_GetUserData()を使うので、DWC_InitFriendsMatch()より前に呼ぶように
          変更してはならない。
 *---------------------------------------------------------------------------*/
int  DWCi_GetProfileIDFromList(int index)
{
    int profileID;

    if (stpFriendCnt->friendList == NULL) return 0;

    profileID = DWC_GetGsProfileId(DWCi_GetUserData(), &stpFriendCnt->friendList[index]);

    if (!profileID || (profileID == -1)) return 0;
    else return profileID;
}


/*---------------------------------------------------------------------------*
  友達リスト内のインデックス取得関数
  引数　：profileID プロファイルID
  戻り値：該当するプロファイルIDの友達リスト内インデックス。ない場合は-1を返す
  用途　：プロファイルIDから、それが存在する友達リスト内のインデックスを返す。
          DWC_GetUserData()を使うので、DWC_InitFriendsMatch()より前に呼ぶように
          変更してはならない。
 *---------------------------------------------------------------------------*/
int DWCi_GetFriendListIndex(int profileID)
{
    int i;

    if (!stpFriendCnt || !profileID) return -1;

    for (i = 0; i < stpFriendCnt->friendListLen; i++){
        if (DWCi_GetProfileIDFromList(i) == profileID){
            return i;
        }
    }

    return -1;
}


/*---------------------------------------------------------------------------*
  gpProcess()呼び出し回数カウンタ初期化関数
  引数　：なし
  戻り値：なし
  用途　：gpProcess()呼び出し回数カウンタを初期化する
 *---------------------------------------------------------------------------*/
void DWCi_InitGPProcessCount(void)
{

    if (stpFriendCnt){
        stpFriendCnt->gpProcessCount = 0;
        stpFriendCnt->lastGpProcess  = OS_GetTick();
    }
}


/*---------------------------------------------------------------------------*
  gpStatusセット関数
  引数　：status         gpStatusのstatusにセットする文字列。
                         現在の値を変更したくなければDWC_GP_STATUS_NO_CHANGEを渡す。
          statusString   gpStatusのstatusStringにセットする文字列。
                         現在の文字列を変更したくなければNULLを渡す。
          locationString gpStatusのlocationStringにセットする文字列
                         現在の文字列を変更したくなければNULLを渡す。
                         ここはゲーム定義の文字列として使うので、ライブラリで
                         上書きすることは通常はない。
  戻り値：GPの処理結果型。ログイン前に呼ばれたらGP_NO_ERRORを返すが、
          処理は何もしない。
  用途　：自分のgpStatusをセットする
 *---------------------------------------------------------------------------*/
GPResult DWCi_SetGPStatus(int status, const char* statusString, const char* locationString)
{

    // statusString, locationStringは、文字数が多すぎても、gpSetStatus内で
    // エラーなしで最大文字数までのみコピーされる

#if 1
    // [note]
    // 本当はこう書くべきなのに、elseのように書かないと、
    // ログインコールバック内でDWC_SetOwnStatusData()を呼び出した場合に
    // しばらくしてからなぜかGPエラーになる。エラーコードはGP_PARSE。
    // どちらの場合もこの判定は抜けてgpSetStatus()まで実行される。
    // →解決した
    if ((stpFriendCnt == NULL) || !DWCi_CheckLogin()) return GP_NO_ERROR;
#else
    if ((stpFriendCnt == NULL) || (stpFriendCnt->pGpObj == NULL)) return GP_NO_ERROR;
#endif

    // [note]
    // gpi.hに記述されているGPIConnection構造体を使っている
    if (status == DWC_GP_STATUS_NO_CHANGE){
        status = ((GPIConnection *)*stpFriendCnt->pGpObj)->lastStatus;
    }
    else {
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Change GP status->status %d\n", status);
    }

    if (!statusString){
        statusString = ((GPIConnection *)*stpFriendCnt->pGpObj)->lastStatusString;
    }
    else {
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Change GP status->statusString %s\n", statusString);
    }
        
    if (!locationString){
        locationString = ((GPIConnection *)*stpFriendCnt->pGpObj)->lastLocationString;
    }
    else {
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Change GP status->locationString %s\n", locationString);
    }

    return gpSetStatus(stpFriendCnt->pGpObj, (GPEnum)status, statusString, locationString);
}


/*---------------------------------------------------------------------------*
  gpStatus取得関数
  引数　：status         gpStatusのstatus
          statusString   gpStatusのstatusString文字列
          locationString gpStatusのlocationString文字列
  戻り値：TRUE :取得成功、FALSE:ログイン前などで取得失敗
  用途　：現在の自分のgpStatusを取得する
 *---------------------------------------------------------------------------*/
BOOL DWCi_GetGPStatus(int* status, char* statusString, char* locationString)
{

    if ((stpFriendCnt == NULL) || (*stpFriendCnt->pGpObj == NULL)) return FALSE;

    // [note]
    // gpi.hに記述されているGPIConnection構造体を使っている
    *status = ((GPIConnection *)*stpFriendCnt->pGpObj)->lastStatus;
    strcpy(statusString, ((GPIConnection *)*stpFriendCnt->pGpObj)->lastStatusString);
    strcpy(locationString, ((GPIConnection *)*stpFriendCnt->pGpObj)->lastLocationString);

    return TRUE;
}


/*---------------------------------------------------------------------------*
  バディ成立コールバック呼び出し関数
  引数　：index バディ関係が成立した友達の友達リストインデックス
  戻り値：なし
  用途　：友達リスト同期処理中以外なら、バディ成立コールバック関数を呼び出す
 *---------------------------------------------------------------------------*/
void DWCi_CallBuddyFriendCallback(int index)
{

    if (stpFriendCnt->buddyCallback &&
        (stpFriendCnt->state != DWC_FRIEND_STATE_PERS_LOGIN)){
        stpFriendCnt->buddyCallback(index, stpFriendCnt->buddyParam);
    }

    // バディ成立時は明示的にステータス更新コールバックを呼び出す。
    if (stpFriendCnt->statusCallback){
        u8 st;
        GPBuddyStatus status;

        st = DWC_GetFriendStatus( &stpFriendCnt->friendList[index], status.locationString );
        
        // ユーザが登録したコールバックを呼び出す
        stpFriendCnt->statusCallback(index,
                                     st,
                                     status.locationString,
                                     stpFriendCnt->statusParam);
    }
}


/*---------------------------------------------------------------------------*
  友達管理オブジェクト解放関数
  引数　：なし
  戻り値：なし
  用途　：友達管理オブジェクトへのポインタをNULLクリアする
 *---------------------------------------------------------------------------*/
void DWCi_ShutdownFriend(void)
{
    stpFriendCnt = NULL;
}


//----------------------------------------------------------------------------
// function - static
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  gpProcess()呼び出し関数
  引数　：なし
  戻り値：GPの処理結果型
  用途　：gpProcess()を呼び出す。gpProcessの呼び出し回数もカウントしている。
 *---------------------------------------------------------------------------*/
static GPResult DWCi_GPProcess(void)
{
    GPResult result = GP_NO_ERROR;

    if (OS_TicksToMilliSeconds(OS_GetTick()-stpFriendCnt->lastGpProcess) >= DWC_GP_PROCESS_INTERVAL)
    {
        stpFriendCnt->gpProcessCount++; // 40年ぐらいはループしないので、対策なし。

#if defined(DWC_ENABLE_KEEPALIVE_TO_GPSERVER)
        if ((stpFriendCnt->gpProcessCount % DWC_GP_KEEP_ALIVE_COUNT) == 0)
        {
            // 一定時間ごとにGPのセッションが切られないように
            // キープアライブ代わりの関数を呼び出す
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "GP:keep alive\n");
                
            (void)gpSetInfoi(stpFriendCnt->pGpObj, GP_INTERESTS1, 0);
        }
#endif
        result = gpProcess(stpFriendCnt->pGpObj);

        stpFriendCnt->lastGpProcess = OS_GetTick();
    }

    return result;
}


/*---------------------------------------------------------------------------*
  友達管理処理終了関数
  引数　：なし
  戻り値：なし
  用途　：友達管理オブジェクトを初期化して、友達管理処理を終了する
 *---------------------------------------------------------------------------*/
static void DWCi_CloseFriendProcess(void)
{

    if (!stpFriendCnt) return;

#if 0
    CloseStatsConnection();
#endif
    
    // 友達管理オブジェクト初期化
    stpFriendCnt->state = DWC_FRIEND_STATE_INIT;

    // [arakit] main 051027
    stpFriendCnt->buddyUpdateState = DWC_BUDDY_UPDATE_STATE_WAIT;
    stpFriendCnt->svUpdateComplete = 0;
    // [arakit] main 051027
}


/*---------------------------------------------------------------------------*
  友達登録更新関数
  引数　：friendList    友達リスト
          friendListLen 更新を行うfriendListの要素数
  戻り値：なし
  用途　：友達リストを参照し、Buddy登録・消去を行う。
          DWC_GetUserData()を使うので、DWC_InitFriendsMatch()より前に呼ぶように
          変更してはならない。
 *---------------------------------------------------------------------------*/
static void DWCi_UpdateFriendReq(DWCFriendData friendList[], int friendListLen)
{
    char userName[21];
    int numBuddy;
    int profileID;
    int i, j;
    GPBuddyStatus status;
    GPResult gpResult;

    // [todo]
    // ここのエラー処理はどうしたらよいか
    // バディリストを見るだけの関数はパラメータエラーしかない

    if (stpFriendCnt->buddyUpdateState == DWC_BUDDY_UPDATE_STATE_WAIT){
        // 主にGPバディリストの友達を消す処理で、これは１回だけ行う
        // バディの数を取得
        gpResult = gpGetNumBuddies(stpFriendCnt->pGpObj, &numBuddy);
        (void)DWCi_HandleGPError(gpResult);

        DWC_Printf(DWC_REPORTFLAG_UPDATE_SV, "gpGetNumBuddies -> %d\n", numBuddy);
    
        // 友達の消去を行う
        for (i = 0; i < numBuddy; i++){
            gpResult = gpGetBuddyStatus(stpFriendCnt->pGpObj, i, &status);
            (void)DWCi_HandleGPError(gpResult);
    
            for (j = 0; j < friendListLen; j++){
                if (DWCi_GetProfileIDFromList(j) == status.profile){
                    // 両方のリストに同じプロファイルIDがあれば元から友達
                    if (!DWC_IsBuddyFriendData(&friendList[j])){
                        // バディ成立フラグが立っていなかったら、前回フラグが
                        // セーブされる前に電源を切られたと考えられるので
                        // フラグを立てておく
                        DWC_SetGsProfileId(&friendList[j], status.profile);
                        DWCi_SetBuddyFriendData(&friendList[j]);

                        stpFriendCnt->friendListChanged = 1;  // 友達リスト変更あり
                    }
                    break;
                }
            }
    
            if (j == friendListLen){
                // バディリストにあって友達リストにないプロファイルIDとは
                // バディ関係を解消する
                DWC_Printf(DWC_REPORTFLAG_UPDATE_SV, "Deleted buddy %u\n", status.profile);
                           
                gpResult = gpDeleteBuddy(stpFriendCnt->pGpObj, status.profile);
                (void)DWCi_HandleGPError(gpResult);
    
                numBuddy--;
                i--;
            }
        }

        stpFriendCnt->buddyUpdateState = DWC_BUDDY_UPDATE_STATE_CHECK;
    }

    
    // 友達の登録を行う
    for ( ; stpFriendCnt->buddyUpdateIdx < friendListLen; stpFriendCnt->buddyUpdateIdx++){
        profileID = DWCi_GetProfileIDFromList(stpFriendCnt->buddyUpdateIdx);
        if (profileID){
            // 友達リストのプロファイルIDが有効な場合

            // 既に同じプロファイルIDが存在して消去された場合は何もしない
            if (DWCi_RefreshFriendListForth(friendList, stpFriendCnt->buddyUpdateIdx, profileID))
                continue;
            
            // 既にバディ成立フラグが立っている場合は、実際にバディなら問題ないし、
            // GPバディリストにない場合はバディリストのダウンロードが完了して
            // いないだけと考えられるので何もしない
            //if (DWC_IsBuddyFriendData(&friendList[stpFriendCnt->buddyUpdateIdx]))
            //    continue;
            
            gpResult = gpGetBuddyIndex(stpFriendCnt->pGpObj, profileID, &i);
            (void)DWCi_HandleGPError(gpResult);
        
            if (i == -1){
                // プロファイルIDがまだバディリストに登録されていなければ
                // バディ登録要求を行う
                (void)DWCi_GPSendBuddyRequest(profileID);

#ifndef SDK_FINALROM    
                if (DWC_IsBuddyFriendData(&friendList[stpFriendCnt->buddyUpdateIdx])){
                    DWC_Printf(DWC_REPORTFLAG_DEBUG, "Local only friend\n");
                }
#endif
            }
            // 既にバディリストが登録されていてフレンドコードのままの場合は、
            // 最初の友達消去ループの方で処理されている
        }
        else if (DWC_GetGsProfileId(DWCi_GetUserData(), &friendList[stpFriendCnt->buddyUpdateIdx]) == -1){
            // 友達リストにログインIDがセットされている場合
            // GPに問い合わせてプロファイルIDを取得してくる必要があるため
            // 必要なデータをparamに渡して非同期関数を呼び出し、
            // 続きの処理はコールバック関数内で行う
            // ログインIDからGPプロファイルのラストネームを取得
            DWC_LoginIdToUserName(DWCi_GetUserData(),
                                  &friendList[stpFriendCnt->buddyUpdateIdx],
                                  userName);
            // ラストネームから友達を検索
            gpProfileSearch(stpFriendCnt->pGpObj,
                            NULL,
                            NULL,
                            NULL,
                            NULL,
                            userName,
                            NULL,
                            GP_NON_BLOCKING,
                            (GPCallback)DWCi_GPProfileSearchCallback,
                            (void *)stpFriendCnt->buddyUpdateIdx);

            DWC_Printf(DWC_REPORTFLAG_UPDATE_SV, "Called gpProfileSearch().\n");

            stpFriendCnt->buddyUpdateState = DWC_BUDDY_UPDATE_STATE_PSEARCH;
            break;
        }
    }

    return;
}


/*---------------------------------------------------------------------------*
  友達リスト同期処理終了関数
  引数　：なし
  戻り値：なし
  用途　：友達リスト同期処理終了時の処理を行う
 *---------------------------------------------------------------------------*/
static void DWCi_EndUpdateServers(void)
{
    
    // コールバックを呼び出す
    stpFriendCnt->updateCallback(DWC_ERROR_NONE,
                                 stpFriendCnt->friendListChanged,
                                 stpFriendCnt->updateParam);

    stpFriendCnt->state = DWC_FRIEND_STATE_LOGON;
}


/*---------------------------------------------------------------------------*
  友達リスト上の友達情報消去関数（コールバック呼び出し付き）
  引数　：friendList  友達リスト
          deleteIndex 消去したい友達情報のインデックス
          otherIndex  他にもコールバックに渡したい友達リストインデックス
  戻り値：なし
  用途　：ローカルの友達リストから友達情報を消去し、ゲームから登録された
          削除コールバック関数を呼び出す。
          ゲーム側で意図しない削除については必ずこの関数を呼び出す。
 *---------------------------------------------------------------------------*/
static void DWCi_DeleteFriendFromList(DWCFriendData friendData[], int deleteIndex, int otherIndex)
{

    if (stpFriendCnt){
        // 友達情報クリア
        MI_CpuClear8(&friendData[deleteIndex], sizeof(DWCFriendData));
        
        if (stpFriendCnt->deleteCallback){
            // ゲーム側で登録された削除コールバックを呼び出す
            stpFriendCnt->deleteCallback(deleteIndex, otherIndex,
                                         stpFriendCnt->deleteParam);
        }
    }
}


/*---------------------------------------------------------------------------*
  友達リスト整理関数
  引数　：friendList 友達リストへのポインタ
          index      重複を確認したい友達の友達リストインデックス
          profileID  重複を確認したい友達のプロファイルID
  戻り値：TRUE:友達リストの内容を変更した、FALSE:友達リストの内容を変更しなかった
  用途　：指定した友達リストインデックスとプロファイルIDの友達について、
          指定のインデックスより前のインデックスに、重複している
          プロファイルIDがないかを調べ、あったら自らの友達情報を消去する。
          ただし、指定の友達情報がバディ成立済みで、前のインデックスの
          友達情報がバディ成立済みでない場合のみ、前のインデックスの
          友達情報の方を消去する。
 *---------------------------------------------------------------------------*/
static BOOL DWCi_RefreshFriendListForth(DWCFriendData friendList[], int index, int profileID)
{
    int pid_i;
    int i;

    for (i = 0; i < index; i++){
        pid_i = DWCi_GetProfileIDFromList(i);
        if (pid_i && (pid_i == profileID)){
            // 重複を見つけたら削除する
            DWC_Printf(DWC_REPORTFLAG_UPDATE_SV,
                       "Found same friend in the list [%d] & [%d], %d\n",
                       i, index, DWC_GetFriendDataType(&friendList[i]));

            if (DWC_IsBuddyFriendData(&friendList[index]) &&
                !DWC_IsBuddyFriendData(&friendList[i])){
                // 消そうと思っていた友達情報でバディ成立済みフラグが立っており、
                // 元データと思っていた友達情報でバディ成立済みフラグが
                // 立っていない場合のみ、元データの方を消去する
                DWCi_DeleteFriendFromList(friendList, i, index);
            }
            else {
                // 通常は元データを残し、指定のインデックスのデータを消去する
                DWCi_DeleteFriendFromList(friendList, index, i);
            }

            stpFriendCnt->friendListChanged = 1;  // 友達リスト変更あり
            return TRUE;
        }
    }

    return FALSE;
}


/*---------------------------------------------------------------------------*
  友達リスト整理関数（全体調査版）
  引数　：friendList    友達リストへのポインタ
          friendListLen 友達リストの最大長（要素数）
          profileID     インデックスを調べたいプロファイルID
  戻り値：引数profileIDのインデックス。友達リストにない場合は-1を返す。
  用途　：友達リストを全て調べ、重複しているプロファイルIDがあったら消去する。
          最終的に引数profileIDが何番のインデックスとして残ったかを返す。
 *---------------------------------------------------------------------------*/
static int DWCi_RefreshFriendListAll(DWCFriendData friendList[], int friendListLen, int profileID)
{
    int  index = -1;
    int  pid_i;
    int  i, j;

    for (i = 0; i < friendListLen; i++){
        pid_i = DWCi_GetProfileIDFromList(i);
        if (pid_i == 0) continue;

        // 最後まで残るprofileIDのインデックスを退避
        if (pid_i == profileID) index = i;

        for (j = i+1; j < friendListLen; j++){
            if (pid_i == DWCi_GetProfileIDFromList(j)){
                // プロファイルIDの重複を見つけた場合
                if ((DWC_GetFriendDataType(&friendList[i]) == DWC_FRIENDDATA_FRIEND_KEY) &&
                    (DWC_GetFriendDataType(&friendList[j]) == DWC_FRIENDDATA_GS_PROFILE_ID)){
                    // iが友達登録鍵でjがプロファイルIDの場合、
                    // まずはiの友達情報もプロファイルIDの状態にする
                    DWC_SetGsProfileId(&friendList[i], pid_i);
                }

                if (DWC_IsBuddyFriendData(&friendList[j])){
                    // jがバディ成立済みの場合、iの友達情報もバディ成立済みにする
                    DWCi_SetBuddyFriendData(&friendList[i]);
                }

                DWC_Printf(DWC_REPORTFLAG_UPDATE_SV,
                           "Found same friend in the list [%d] & [%d], %d\n",
                           i, j, DWC_GetFriendDataType(&friendList[i]));

                // jの友達情報を削除する
                DWCi_DeleteFriendFromList(friendList, j, i);

                stpFriendCnt->friendListChanged = 1;  // 友達リスト変更あり
            }
        }
    }

    return index;
}


/*---------------------------------------------------------------------------*
  バディリクエスト送信関数
  引数　：profileID バディリクエスト送信先プロファイルID
  戻り値：GPの処理結果型
  用途　：バディリクエストを送信する。エラー処理は中で行う。
 *---------------------------------------------------------------------------*/
static GPResult DWCi_GPSendBuddyRequest(int profileID)
{
    GPResult gpResult;
    
    gpResult = gpSendBuddyRequest(stpFriendCnt->pGpObj,
                                  profileID,
                                  ""/*(gsi_char *)stpFriendCnt->playerName*/);
    (void)DWCi_HandleGPError(gpResult);

    DWC_Printf(DWC_REPORTFLAG_UPDATE_SV,
               "Send buddy request to %u\n",
               profileID);

    return gpResult;
}


/*---------------------------------------------------------------------------*
  友達BuddyStatus取得関数
  引数　：friendData 友達情報へのポインタ
          status     BuddyStatus格納先ポインタ
  戻り値：TRUE:バディステータス取得成功、FALSE:バディステータス取得失敗
  用途　：友達リストのインデックスを指定して、その友達のBuddyStatusを取得する
 *---------------------------------------------------------------------------*/
static BOOL DWCi_GetFriendBuddyStatus(const DWCFriendData* friendData, GPBuddyStatus* status)
{
    u8  ret;
    int buddyIdx = 0;
    int profileid;

    SDK_ASSERT(friendData);
    SDK_ASSERT(status);

    if (!stpFriendCnt || !DWCi_CheckLogin()) return FALSE;

    profileid = DWC_GetGsProfileId(DWCi_GetUserData(), friendData);
    if (profileid > 0 && gpGetBuddyIndex(stpFriendCnt->pGpObj,
                                         profileid,
                                         &buddyIdx)){
        // エラーはパラメータエラーしかないのでオフライン扱いで充分
        ret = FALSE;
    }
    else if (profileid <= 0 || buddyIdx == -1){
        // まだバディがバディリストに登録されていない場合もオフライン扱い。
        // ログインコールバック内で呼ばれた場合など、まだバディリストが
        // 全て取得できていない場合もここに来る。
        ret = FALSE;
    }
    else {
        // バディ情報を取得
        if (gpGetBuddyStatus(stpFriendCnt->pGpObj, buddyIdx, status)){
            // エラーはパラメータエラーしかないのでオフライン扱いで充分
            ret = FALSE;
        }
        else {
            ret = TRUE;
        }
    }

    return ret;
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

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Friend, GP error %d\n", result);

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
    errorCode += DWC_ECODE_SEQ_FRIEND+DWC_ECODE_GS_GP;

    // 友達管理処理中断関数を呼び出す
    DWCi_StopFriendProcess(dwcError, errorCode);

    return result;
}


/*---------------------------------------------------------------------------*
  Persistentエラー処理関数
  引数　：error Persistentのエラー型
  戻り値：Persistentのエラー型（引数をそのまま返す）
  用途　：Persistenエラー値に対応するエラー表示（もしくは停止）を行いつつ、
          対応するDWCエラーを渡して、対応するコールバックを呼び出す
 *---------------------------------------------------------------------------*/
static int DWCi_HandlePersError(int error)
{
    int errorCode;
    DWCError dwcError;

    if (error == GE_NOERROR) return GE_NOERROR;

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Friend, persistent error %d\n", error);

    switch (error){
    case GE_NOSOCKET:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_SOCKET;
        break;
    case GE_NODNS:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_DNS;
        break;
    case GE_NOCONNECT:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_SERVER;
        break;
    case GE_DATAERROR:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_DATA;
        break;
    case GE_TIMEDOUT:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_TIMEOUT;
        break;
    }
    // InitStatsAsync()を使わないのでGE_CONNECTINGは来ないはず

#if 0
    // ベースエラーコードを追加
    errorCode += DWC_ECODE_SEQ_FRIEND+DWC_ECODE_GS_PERS;

    // 友達管理処理中断関数を呼び出す
    DWCi_StopFriendProcess(dwcError, errorCode);

#else
    // ベースエラーコードを追加
    errorCode += DWC_ECODE_SEQ_ETC+DWC_ECODE_GS_PERS;

    if (stPersState == DWC_PERS_STATE_LOGIN){
        DWCi_StopPersLogin(dwcError, errorCode);
    }
#endif
        
    return error;
}


/*---------------------------------------------------------------------------*
  GPプロファイルサーチ完了コールバック関数
  引数　：connection GPコネクションオブジェクトへのポインタ
          arg        GPProfileSearchResponseArg型オブジェクトへのポインタ
          param      コールバック用パラメータ
  戻り値：なし
  用途　：GPプロファイルサーチ完了コールバック
 *---------------------------------------------------------------------------*/
static void DWCi_GPProfileSearchCallback(GPConnection* connection,
                                         GPProfileSearchResponseArg* arg,
                                         void* param)
{
    int idx;
    GPResult gpResult;
    int friendIdx = (int)param;

    DWC_Printf( DWC_REPORTFLAG_DEBUG, "ProfileSearch: num:%d m:%x\n", arg->numMatches, arg->more );

    if ((arg->result == GP_NO_ERROR) && arg->numMatches &&
        (DWC_GetFriendDataType(&stpFriendCnt->friendList[friendIdx]) != DWC_FRIENDDATA_NODATA)){
        int match_index;
        // プロファイルが見つかった
        if (arg->numMatches > 1){
            // 万が一２つ以上マッチするプロファイルが見つかってしまった場合
            // 非常に稀なケースなので、特に対策はしない
            DWC_Printf(DWC_REPORTFLAG_UPDATE_SV, "Happned to find %d profiles.\n",
                       arg->numMatches);
        }

        if (stpFriendCnt->state == DWC_FRIEND_STATE_PERS_LOGIN){
            // すべての見つかったプロファイルについて、チェックする。
            for ( match_index = 0 ; match_index < arg->numMatches ; ++match_index )
            {
                // 既にあるプロファイルIDが見つかった場合は、すぐ戻る。
                // プロファイルIDの重複をチェックする
                if (DWCi_RefreshFriendListForth(stpFriendCnt->friendList,
                                                friendIdx,
                                                arg->matches[match_index].profile)){
                    // 新たに取得したプロファイルIDは既に登録済みだったので
                    // 消去した場合。
                    // 処理中の友達リストインデックスを進め、同期処理を続ける
                    stpFriendCnt->buddyUpdateIdx++;
                    stpFriendCnt->buddyUpdateState = DWC_BUDDY_UPDATE_STATE_CHECK;
                    arg->more = GP_DONE; // 終了。
                    return;
                }
            }

            // すべての見つかったプロファイルについて、チェックする。
            for ( match_index = 0 ; match_index < arg->numMatches ; ++match_index )
            {
                gpResult = gpGetBuddyIndex(connection, arg->matches[match_index].profile, &idx);
                (void)DWCi_HandleGPError(gpResult);

                if (idx == -1){
                    // プロファイルIDがまだバディリストに登録されて
                    // いなければバディリクエストを行う
                    (void)DWCi_GPSendBuddyRequest(arg->matches[match_index].profile);
                }
                else {
                    // プロファイルIDが既にバディリストに登録されていた場合は
                    // 前回ログインIDが登録された後、バディ関係が成立したのに、
                    // セーブせずに電源を切られた場合と考えられるので、
                    // プロファイルIDを登録し、バディ成立フラグも立てておく
                    DWC_SetGsProfileId(&stpFriendCnt->friendList[friendIdx],
                                       arg->matches->profile);
                    DWCi_SetBuddyFriendData(&stpFriendCnt->friendList[friendIdx]);
                    // バディ成立コールバックを呼び出す
                    DWCi_CallBuddyFriendCallback(friendIdx);

                    stpFriendCnt->buddyUpdateIdx++;
                    stpFriendCnt->buddyUpdateState = DWC_BUDDY_UPDATE_STATE_CHECK;
                    arg->more = GP_DONE; // 終了。
		            stpFriendCnt->friendListChanged = 1;  // 友達リスト変更あり
                    return;
                }
            }

            if ( arg->more != GP_MORE )
            {
                // 友達リストを受信終了したので、続きを受信するため、一度戻る。
                // 処理中の友達リストインデックスを進め、同期処理を続ける
                stpFriendCnt->buddyUpdateIdx++;
                stpFriendCnt->buddyUpdateState = DWC_BUDDY_UPDATE_STATE_CHECK;
            }
            else
            {
                DWC_Printf(DWC_REPORTFLAG_DEBUG, "more profiles will come...\n");
            }
        }
    }
    else if (arg->result){
        // エラー発生
        if (DWCi_HandleGPError(arg->result)) return;
    }
    else if ((stpFriendCnt->state == DWC_FRIEND_STATE_PERS_LOGIN) ||
             (DWC_GetFriendDataType(&stpFriendCnt->friendList[friendIdx]) == DWC_FRIENDDATA_NODATA)){
        // 該当プロファイルが見つからなかった場合も
        // 処理中の友達リストインデックスを進め、同期処理を続ける。
        // ProfileSearchCallbackが来る前に、GetInfoコールバック内で
        // 友達情報が削除されていた場合もここに来る
        stpFriendCnt->buddyUpdateIdx++;
        stpFriendCnt->buddyUpdateState = DWC_BUDDY_UPDATE_STATE_CHECK;
    }
}


/*---------------------------------------------------------------------------*
  GP GPInfo受信コールバック関数（バディリクエスト受信時用）
  引数　：connection GPコネクションオブジェクトへのポインタ
          arg        GPGetInfoResponseArg型オブジェクトへのポインタ
          param      コールバック用パラメータ
  戻り値：なし
  用途　：バディリクエスト受信時用、GPInfo受信コールバック
          DWC_GetUserData()を使うので、DWC_InitFriendsMatch()より前に呼ぶように
          変更してはならない。
 *---------------------------------------------------------------------------*/
static void DWCi_GPGetInfoCallback_RecvBuddyRequest(GPConnection* connection,
                                                    GPGetInfoResponseArg* arg,
                                                    void* param)
{
#pragma unused(param)
    char userName[21];
	char gamecode_str[5];
	unsigned int gamecode;
    GPResult gpResult;
    int i;
    BOOL confirm = FALSE;

    // [arakit] main 051013
    if (arg->result != GP_NO_ERROR)
    {
        DWC_Printf( DWC_REPORTFLAG_LOGIN, " ERROR: GetInfo Req. why??? : %d\n", arg->result );
        return;
    }
    // [arakit] main 051013

    // [arakit] main 051012
    DWC_Printf(DWC_REPORTFLAG_DEBUG,
               "GetInfo Req: profileID %u, lastname '%s'.\n",
               arg->profile, arg->lastname);

    for (i = 0; i < stpFriendCnt->friendListLen; i++)
    {
        if ( DWC_GetFriendDataType( &stpFriendCnt->friendList[i] ) == DWC_FRIENDDATA_LOGIN_ID )
        {
            DWC_LoginIdToUserName(DWCi_GetUserData(),
                                  &stpFriendCnt->friendList[i],
                                  userName);

            if (strcmp(userName, arg->lastname) == 0)
            {
                // バディリクエストを承認する
                gpResult = gpAuthBuddyRequest(connection, arg->profile);
                // 友達情報をログインIDからプロファイルIDに置き換える
                DWC_SetGsProfileId(&stpFriendCnt->friendList[i], arg->profile);

                confirm = TRUE;

                DWC_Printf(DWC_REPORTFLAG_UPDATE_SV,
                           "Auth buddy request from %u, friend[%d]mp.\n",
                           arg->profile, i);
            }
        }
        else if ( DWC_GetFriendDataType( &stpFriendCnt->friendList[i] ) == DWC_FRIENDDATA_GS_PROFILE_ID ||
                  DWC_GetFriendDataType( &stpFriendCnt->friendList[i] ) == DWC_FRIENDDATA_FRIEND_KEY )
        {
			// ゲームコードの確認
			gamecode = DWCi_GetUserData()->gamecode;
			OS_SNPrintf( gamecode_str, 5, "%c%c%c%c", (u8)(gamecode >> 24), (u8)(gamecode >> 16), (u8)(gamecode >> 8), (u8)gamecode);
            // プロファイルIDベースのとき
            if ( DWC_GetGsProfileId( DWCi_GetUserData(), &stpFriendCnt->friendList[i] ) == arg->profile &&
			     memcmp(gamecode_str, arg->lastname+DWC_ACC_USERNAME_GSBRCD_OFFSET, 4) == 0)
            {
                // バディリクエストを承認する
                gpResult = gpAuthBuddyRequest(connection, arg->profile);

                confirm = TRUE;

                DWC_Printf(DWC_REPORTFLAG_UPDATE_SV,
                           "Auth buddy request from %u, friend[%d]gs.\n",
                           arg->profile, i);
            }
        }
    }

    if ( confirm )
    {
        // こちらからもバディリクエストを送信する。
        // オンライン中に相手にバディリクエストが届かないと困るので
        // 過剰になっても良いから必ず送信する。
        (void)DWCi_GPSendBuddyRequest(arg->profile);
    }
    else {
        // 友達リストにない友達からのバディリクエストだった場合は要求を拒否する
        gpDenyBuddyRequest(connection, arg->profile);

        DWC_Printf(DWC_REPORTFLAG_UPDATE_SV, "Denied buddy request from %u.\n",
                   arg->profile);
    }
}


/*---------------------------------------------------------------------------*
  GP GPInfo受信コールバック関数（バディ承認コールバック受信時用）
  引数　：connection GPコネクションオブジェクトへのポインタ
          arg        GPGetInfoResponseArg型オブジェクトへのポインタ
          param      コールバック用パラメータ
  戻り値：なし
  用途　：バディ承認コールバック受信時用、GPInfo受信コールバック
          DWC_GetUserData()を使うので、DWC_InitFriendsMatch()より前に呼ぶように
          変更してはならない。
 *---------------------------------------------------------------------------*/
static void DWCi_GPGetInfoCallback_RecvAuthMessage(GPConnection* connection,
                                                   GPGetInfoResponseArg* arg,
                                                   void* param)
{
#pragma unused(connection)
#pragma unused(param)
    char userName[21];
    int index;
    int i;
    BOOL listChanged  = FALSE;
    BOOL alreadyBuddy = FALSE;

    // [arakit] main 051013
    if (arg->result != GP_NO_ERROR)
    {
        DWC_Printf( DWC_REPORTFLAG_LOGIN, " ERROR: GetInfo Auth. why??? : %d\n", arg->result );
        return;
    }
    // [arakit] main 051013

    // [arakit] main 051012
    DWC_Printf(DWC_REPORTFLAG_DEBUG,
               "GetInfo Auth: profileID %u, lastname '%s'.\n",
               arg->profile, arg->lastname);

    // 友達リストを検索して仮ログインIDなら比較する
    for (i = 0; i < stpFriendCnt->friendListLen; i++)
    {
        if ( DWC_GetFriendDataType( &stpFriendCnt->friendList[i] ) == DWC_FRIENDDATA_LOGIN_ID )
        {
            // 仮IDのとき
            DWC_LoginIdToUserName(DWCi_GetUserData(),
                                  &stpFriendCnt->friendList[i],
                                  userName);

            if (strcmp(userName, arg->lastname) == 0)
            {
                // 友達リストをログインIDからプロファイルIDに置き換えて友達関係が成立
                DWC_SetGsProfileId(&stpFriendCnt->friendList[i], arg->profile);
                DWCi_SetBuddyFriendData(&stpFriendCnt->friendList[i]);
                listChanged = TRUE;

                DWC_Printf(DWC_REPORTFLAG_UPDATE_SV,
                           "Established buddy with %u, friend[%d]mp.\n",
                           arg->profile, i);

            }
        }
        else if ( DWC_GetFriendDataType( &stpFriendCnt->friendList[i] ) == DWC_FRIENDDATA_GS_PROFILE_ID ||
                  DWC_GetFriendDataType( &stpFriendCnt->friendList[i] ) == DWC_FRIENDDATA_FRIEND_KEY )
        {
            // プロファイルIDか友達登録鍵のとき
            if ( DWC_GetGsProfileId( DWCi_GetUserData(), &stpFriendCnt->friendList[i] ) == arg->profile )
            {
                if( DWC_IsBuddyFriendData( &stpFriendCnt->friendList[i] ) == TRUE )
                {
                    DWC_Printf(DWC_REPORTFLAG_UPDATE_SV, 
                               "This profile is already my friend[%d].\n", i);
                    alreadyBuddy = TRUE;
                }
                else
                {
                    // 友達リストを友達登録鍵からプロファイルIDに置き換えて友達関係が成立
                    DWC_SetGsProfileId(&stpFriendCnt->friendList[i], arg->profile);
                    DWCi_SetBuddyFriendData(&stpFriendCnt->friendList[i]);
                    listChanged = TRUE;

                    DWC_Printf(DWC_REPORTFLAG_UPDATE_SV,
                               "Established buddy with %u, friend[%d]gs.\n",
                               arg->profile, i);
                }
            }
        }
    }

    // 友達リストの変更があった場合はリストの整理と、必要に応じてバディ成立コールバックを呼び出す
    if ( listChanged )
    {
        // プロファイルIDの重複を見つけて消去する
        index = DWCi_RefreshFriendListAll(stpFriendCnt->friendList,
                                          stpFriendCnt->friendListLen,
                                          arg->profile);

        // バディ成立コールバックを呼び出す
        if ( !alreadyBuddy )
        {
            DWCi_CallBuddyFriendCallback(index);
        }

        stpFriendCnt->friendListChanged = 1;  // 友達リスト変更あり
    }
    else
    {
        if ( !alreadyBuddy )
        {
            DWC_Printf(DWC_REPORTFLAG_UPDATE_SV, "Not Established buddy with %u.\n",
                       arg->profile);
        }
    }
}


/*---------------------------------------------------------------------------*
  Persistentサーバログイン終了関数
  引数　：error     DWCエラー種別
          errorCode エラーコード
  戻り値：なし
  用途　：Persistentサーバログイン処理中にエラーが発生した場合に、処理を中断する
 *---------------------------------------------------------------------------*/
static void DWCi_StopPersLogin(DWCError error, int errorCode)
{

    if (!stpFriendCnt || (error == DWC_ERROR_NONE)) return;

    DWCi_SetError(error, errorCode);

    // Persistentサーバからログアウトする
    if(IsStatsConnected())
		DWC_LogoutFromStorageServer();

    if (stpFriendCnt->persLoginCallback != NULL){
        stpFriendCnt->persLoginCallback(error, stpFriendCnt->persLoginParam);
    }
}


/*---------------------------------------------------------------------------*
  Persistentサーバセーブ関数
  引数　：profileID セーブ先のプロファイルID
          type      データアクセスタイプ
          keyvalues データベース文字列（key/value値）へのポインタ
          param     コールバック用パラメータ
  戻り値：なし
  用途　：Persistentサーバにデータをセーブする。
 *---------------------------------------------------------------------------*/
static void DWCi_SetPersistDataValuesAsync(int profileID,
                                           persisttype_t type,
                                           gsi_char* keyvalues,
                                           void* param)
{

    DWCi_AddPersCallbackLevel();

    SetPersistDataValues(0, profileID, type, 0, keyvalues,
                         DWCi_PersDataSaveCallback, param);
}


/*---------------------------------------------------------------------------*
  Persistentサーバロード関数
  引数　：profileID ロード先のプロファイルID
          type      データアクセスタイプ
          keys      データベース文字列（key値）へのポインタ
          param     コールバック用パラメータ
  戻り値：なし
  用途　：Persistentサーバからデータをロードする。
 *---------------------------------------------------------------------------*/
static void DWCi_GetPersistDataValuesAsync(int profileID,
                                           persisttype_t type,
                                           gsi_char* keys,
                                           void* param)
{

    DWCi_AddPersCallbackLevel();

    GetPersistDataValues(0, profileID, type, 0, keys,
                         DWCi_PersDataCallback, param);
}


/*---------------------------------------------------------------------------*
  Persistentサーバ認証コールバック関数
  引数　：localid       認証を行ったユーザのローカルID
          profileid     認証を行ったユーザのプロファイルID
          authenticated TRUE:認証成功、FALSE:認証失敗
          errmsg        認証失敗の場合のエラーメッセージ
          instance      コールバック用パラメータ
  戻り値：なし
  用途　：Persistentサーバ認証コールバック
 *---------------------------------------------------------------------------*/
static void DWCi_PersAuthCallback(int localid, int profileid,
                                  int authenticated, gsi_char* errmsg,
                                  void* instance)
{
#ifdef SDK_FINALROM
#pragma unused(errmsg)
#endif
#pragma unused(instance)
#if 0
    char tmpStr[DWC_MAX_PLAYER_NAME*2];
#endif

    DWCi_SubPersCallbackLevel();
    
    if (!authenticated || (localid != 0)){
        // 認証失敗
        DWC_Printf(DWC_REPORTFLAG_ERROR, "Stats server authentication failed.\n");
        DWC_Printf(DWC_REPORTFLAG_ERROR, "%s\n", errmsg);

#if 0
        // 友達管理処理中断関数を呼び出す
        DWCi_StopFriendProcess(DWC_ERROR_NETWORK,
                               DWC_ECODE_SEQ_FRIEND+DWC_ECODE_GS_PERS+DWC_ECODE_TYPE_STATS_AUTH);
#else
        DWCi_StopPersLogin(DWC_ERROR_NETWORK,
                           DWC_ECODE_SEQ_ETC+DWC_ECODE_GS_PERS+DWC_ECODE_TYPE_STATS_AUTH);
#endif
        return;
    }
    else {
        // 認証成功
        DWC_Printf(DWC_REPORTFLAG_UPDATE_SV, "Stats server authentication succeeded.\n");

        stpFriendCnt->profileID = profileid;

#if 0
        // プレイヤー名のキーを作成
        (void)OS_SNPrintf(tmpStr, sizeof(tmpStr),
                          "\\dwc_name\\%s",
                          stpFriendCnt->playerName);
        // プレイヤー名をPersistentサーバにセーブする
        DWCi_SetPersistDataValuesAsync(stpFriendCnt->profileID, pd_public_rw,
                                       tmpStr,
                                       (void *)DWC_PERS_KEY_ID_PLAYER_NAME);

        // プレイヤー名のキー登録を終えるまではPersistentサーバ
        // ログイン状態のままにしておく

#else
        stPersState = DWC_PERS_STATE_CONNECTED;

        // コールバックを呼び出す
        if (stpFriendCnt->persLoginCallback != NULL){
            stpFriendCnt->persLoginCallback(DWC_ERROR_NONE, stpFriendCnt->persLoginParam);
        }
#endif
    }
}


/*---------------------------------------------------------------------------*
  Persistentデータ取得コールバック関数
  引数　：localid   データを持つユーザのローカルID
          profileid データを持つユーザのプロファイルID
          type      アクセスタイプ
          index     キー内インデックス
          success   TRUE:取得成功、FALSE:取得失敗
          modified  データが修正された時間
          data      取得したデータ
          len       取得データ長
          instance  コールバック用パラメータ
  戻り値：なし
  用途　：Persistentデータ取得コールバック
 *---------------------------------------------------------------------------*/
static void DWCi_PersDataCallback(int localid, int profileid,
                                  persisttype_t type, int index, int success,
                                  time_t modified, char* data, int len,
                                  void* instance)
{
#pragma unused(localid)
#pragma unused(type)
#pragma unused(index)
#pragma unused(modified)
    BOOL result;

    DWCi_SubPersCallbackLevel();

    if (success){
        if (len == 0){
            // キーが存在しなかった場合。
            // もしくは他人のPrivateキーをロードしようとした場合。
            DWC_Printf(DWC_REPORTFLAG_WARNING,
                       "Persitent, Specified key is not exist.\n");
            // 失敗のコールバックを返す
            result = FALSE;
        }
        else {
            // キーが存在した場合
            DWC_Printf(DWC_REPORTFLAG_DEBUG,
                       "Loaded data from persistent server. -> %s\n", data);
            result = TRUE;
        }

        if (stpFriendCnt->loadCallback){
            // ゲームから登録されたコールバックを呼び出す
            stpFriendCnt->loadCallback(result, DWCi_GetFriendListIndex(profileid), data, len, instance);
        }
    }
    else {
        // エラー発生
        DWC_Printf(DWC_REPORTFLAG_ERROR, "Failed to load persistent data.\n");

#if 0
        // 友達管理処理中断関数を呼び出す
        DWCi_StopFriendProcess(DWC_ERROR_NETWORK,
                               DWC_ECODE_SEQ_FRIEND+DWC_ECODE_GS_PERS+DWC_ECODE_TYPE_STATS_LOAD);
        return;
#endif

        // ゲームから登録されたコールバックを呼び出す
        if (stpFriendCnt->loadCallback){
            stpFriendCnt->loadCallback(FALSE, DWCi_GetFriendListIndex(profileid), data, len, instance);
        }
    }
}


/*---------------------------------------------------------------------------*
  Persistentデータセーブコールバック関数
  引数　：localid   データをセーブしたユーザのローカルID
          profileid データをセーブしたユーザのプロファイルID
          type      アクセスタイプ
          index     キー内インデックス
          success   TRUE:セーブ成功、FALSE:セーブ失敗
          modified  データがセーブされた時間
          instance  コールバック用パラメータ
  戻り値：なし
  用途　：Persistentデータセーブコールバック
 *---------------------------------------------------------------------------*/
static void DWCi_PersDataSaveCallback(int localid, int profileid,
                                      persisttype_t type, int index, int success,
                                      time_t modified, void* instance)
{
#pragma unused(localid)
#pragma unused(profileid)
#pragma unused(index)
#pragma unused(modified)
    u32  flag = (u32)instance;
    BOOL isPublic;

    DWCi_SubPersCallbackLevel();

#if 0
    if (flag == DWC_PERS_KEY_ID_PLAYER_NAME){
        if (success){
            // 本当にdwc_nameのセーブコールバックか確認する
            if (stpFriendCnt->state == DWC_FRIEND_STATE_PERS_LOGIN){
                // プレイヤー名をセーブした
                DWC_Printf(DWC_REPORTFLAG_UPDATE_SV,
                           "Saved player name to persistent server.\n");

                // Persistentの処理が終了したことを記録する
                stpFriendCnt->svUpdateComplete++;
            }
            else {
                // たまたまパラメータが一緒になっただけだった
                flag = 0;
            }
        }
        else {
            // エラー発生
            DWC_Printf(DWC_REPORTFLAG_ERROR, "Failed to save persistent data.\n");

            // 友達管理処理中断関数を呼び出す
            DWCi_StopFriendProcess(DWC_ERROR_NETWORK,
                                   DWC_ECODE_SEQ_FRIEND+DWC_ECODE_GS_PERS+DWC_ECODE_TYPE_STATS_SAVE);
        }
    }

    // DWC制御下のデータセーブ時はここまでで終了
    if (flag & DWC_PERS_KEY_ID_MASK) return;
#endif

    // 以下ゲームでのデータセーブの場合
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "Saved data to persistent server.\n");

    if ((type == pd_public_ro) || (type == pd_public_rw)){
        isPublic = TRUE;
    }
    else {
        isPublic = FALSE;
    }
        
    if (success){
        if (stpFriendCnt->saveCallback){
            // ゲームから登録されたコールバックを呼び出す
            stpFriendCnt->saveCallback(TRUE, isPublic, instance);
        }
    }
    else {
        // エラー発生
        DWC_Printf(DWC_REPORTFLAG_ERROR, "Failed to save persistent data.\n");

#if 0
        // 友達管理処理中断関数を呼び出す
        DWCi_StopFriendProcess(DWC_ERROR_NETWORK,
                               DWC_ECODE_SEQ_FRIEND+DWC_ECODE_GS_PERS+DWC_ECODE_TYPE_STATS_SAVE);
#endif

        // ゲームから登録されたコールバックを呼び出す
        if (stpFriendCnt->saveCallback){
            stpFriendCnt->saveCallback(FALSE, isPublic, instance);
        }
    }
}


/*---------------------------------------------------------------------------*
  Persistentコールバックレベル増加関数
  引数　：なし
  戻り値：なし
  用途　：Persistentの非同期関数呼び出し時にコールバックレベルを1増やす
 *---------------------------------------------------------------------------*/
static void DWCi_AddPersCallbackLevel(void)
{

    if (stpFriendCnt->persCallbackLevel == 0xffffffff){
        DWCi_StopFriendProcess(DWC_ERROR_FATAL,
                               DWC_ECODE_SEQ_FRIEND+DWC_ECODE_TYPE_UNEXPECTED);
        return;
    }
    stpFriendCnt->persCallbackLevel++;
}


/*---------------------------------------------------------------------------*
  Persistentコールバックレベル減少関数
  引数　：なし
  戻り値：なし
  用途　：Persistentの非同期関数完了コールバック呼び出し時に
          コールバックレベルを1減らす
 *---------------------------------------------------------------------------*/
static void DWCi_SubPersCallbackLevel(void)
{

    if (stpFriendCnt->persCallbackLevel == 0){
        DWCi_StopFriendProcess(DWC_ERROR_FATAL,
                               DWC_ECODE_SEQ_FRIEND+DWC_ECODE_TYPE_UNEXPECTED);
        return;
    }
    stpFriendCnt->persCallbackLevel--;
}


/*---------------------------------------------------------------------------*
  Persistentコールバックレベル取得関数
  引数　：なし
  戻り値：Persistentコールバックレベル
  用途　：Persistentのコールバック呼び出し待ち件数を取得する
 *---------------------------------------------------------------------------*/
static u32 DWCi_GetPersCallbackLevel(void)
{

    return stpFriendCnt->persCallbackLevel;
}
