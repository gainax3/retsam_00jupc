#include <nitro.h>

#include <dwc_report.h>
#include <dwc_error.h>
#include <dwc_nasfunc.h>
#include <dwc_memfunc.h>

/* =======================================================================
        define
   ======================================================================== */

/* =======================================================================
        typedef
   ======================================================================== */

/* [nakata] NALコントロール構造体 */
typedef struct {
	DWCHttp			http;		/* [nakata] HTTP構造体 */
	DWCAuthParam	authparam;	/* [nakata] DWCAuth用パラメータ */
	DWCAuthResult	result;		/* [nakata] 認証結果格納用構造体 */
} DWCNalCnt;

/* =======================================================================
        external variable
   ======================================================================== */
extern s64	DWCnastimediff;
extern BOOL	DWCnastimediffvalid;

/* =======================================================================
        static variable
   ======================================================================== */
static DWCNalCnt *nalcnt = NULL;

/* =======================================================================
        prototype
   ======================================================================== */

/* =======================================================================
        external functions(ingamesn関係)
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:			DWC_GetIngamesnCheckResult

  Description:	NASによるingamesn判定の結果を取得する

  Arguments:	なし

  Returns:		DWC_INGAMESN_NOTCHECKED	現在チェック中(ログイン処理中)
                DWC_INGAMESN_VALID		直前のログインのingamesnはNASに受け入れられた
				DWC_INGAMESN_INVALID	直前のログインのingamesnはNASに拒否された
 *---------------------------------------------------------------------------*/
DWCIngamesnCheckResult DWC_GetIngamesnCheckResult(void)
{
	return (DWCIngamesnCheckResult)DWCauthingamesncheckresult;
}

/* =======================================================================
        external function(NAS時間関連)
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:			DWC_GetIngamesnCheckResult

  Description:	NASの時間をRTC形式で取得する

  Arguments:	date	取得結果のRTCDateを書き込む領域へのポインタ
				time	取得結果のRTCTimeを書き込む領域へのポインタ

  Returns:		TRUE	取得成功
				FALSE - 取得失敗
 *---------------------------------------------------------------------------*/
extern s64     DWCnastimediffbase;

BOOL DWC_GetDateTime(RTCDate *date, RTCTime *time)
{
	s64 localsec;

	if(DWCnastimediffvalid == FALSE)
		return FALSE;
	
	// [nakata] 本体時計情報を取得
	if(RTC_GetDateTime(date, time) != 0)
		return FALSE;
	
	// [nakata] 本体時計の値を秒に変換
	localsec = RTC_ConvertDateTimeToSecond(date, time);
	if(localsec == -1)
		return FALSE;
	
	// [nakata] 秒にサーバ時間との差を足す
    if(localsec < DWCnastimediffbase) {
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "RTC overflow detected.\n");
        localsec += 3155760000;
    }
	localsec -= DWCnastimediff;
	if(localsec < 0 || localsec > 3155759999)
		return FALSE;
	
	// [nakata] Date/Timeを生成
	RTC_ConvertSecondToDateTime(date, time, localsec);
	return TRUE;
}

/* =======================================================================
        external function(SVL関連)
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:			DWC_SVLGetTokenAsync

  Description:	トークン取得関数

  Arguments:	svl		サービス種別を指定する4文字のASCII文字列
				result	取得した結果を格納する領域へのポインタ

  Returns:		TRUE	取得開始
				FALSE	取得開始できない
 *---------------------------------------------------------------------------*/
BOOL DWC_SVLGetTokenAsync(char *svl, DWCSvlResult *result)
{
	BOOL flag;
	
	SDK_ASSERT(svl);
	SDK_ASSERT(result);

	/* [nakata] 初期化処理 */
	flag = DWC_Svl_Init((DWCAuthAlloc)DWC_Alloc, (DWCAuthFree)DWC_Free);
	if(flag == FALSE) {
		/* [nakata] 内部でDWCエラー設定済み */
		return FALSE;
	}
	
	/* [nakata] トークン取得処理開始 */
	if(strlen(svl) == 0)
		flag = DWC_Svl_GetTokenAsync("0000", result);
	else
		flag = DWC_Svl_GetTokenAsync(svl, result);
	
	if(flag == FALSE) {
		/* [nakata] DWC_SVLのワークエリアを開放 */
		DWC_Svl_Cleanup();
		/* [nakata] 内部でDWCエラー設定済み */
		return FALSE;
	}
	return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:			DWC_SVLProcess

  Description:	SVLトークン取得処理進行関数

  Arguments:	なし

  Returns:		DWCSvcState
 *---------------------------------------------------------------------------*/
DWCSvlState DWC_SVLProcess(void)
{
	DWCSvlState state;
	state = DWC_Svl_Process();
	if(state == DWC_SVL_STATE_ERROR || state == DWC_SVL_STATE_SUCCESS || state == DWC_SVL_STATE_CANCELED)
		/* [nakata] DWC_SVLのワークエリアを開放 */
		/* [nakata] 内部でDWCエラー設定済み */
		DWC_Svl_Cleanup();
	return state;
}

/*---------------------------------------------------------------------------*
  Name:			DWC_SVLAbort

  Description:	SVLトークン取得処理進行関数

  Arguments:	なし

  Returns:		なし
 *---------------------------------------------------------------------------*/
void DWC_SVLAbort(void)
{
	DWC_Svl_Abort();
}

/* =======================================================================
        external function(NAS認証関連)
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:			DWC_NASLoginAsync

  Description:	NASログイン開始

  Arguments:	なし

  Returns:		TRUE  - ログイン処理開始成功
				FALSE - ログイン処理開始失敗
 *---------------------------------------------------------------------------*/
BOOL DWC_NASLoginAsync(void)
{
	if(nalcnt != NULL)
		OS_Panic("You can't request more than one NAS login request at once.\n");
	
	/* [nakata] 内部ワークエリアを確保/初期化 */
	nalcnt = DWC_Alloc(DWC_ALLOCTYPE_BASE, sizeof(DWCNalCnt));
	if(nalcnt == NULL) {
		DWCi_SetError(DWC_ERROR_FATAL, -20100);
		return FALSE;
	}
	MI_CpuClear8(nalcnt, sizeof(DWCNalCnt));
	
	/* [nakata] Authパラメータ初期化 */
	/* [nakata] nalcnt->authparam.ingamesnは空文字列 */
	/* [nakata] nalcnt->authparam.gsbrcdは空文字列 */
	MI_CpuCopy8("", nalcnt->authparam.gsbrcd, 9);
	nalcnt->authparam.alloc	= (DWCAuthAlloc)DWC_Alloc;
	nalcnt->authparam.free	= (DWCAuthFree)DWC_Free;
	
	/* [nakata] Auth開始 */
#if 1
	DWC_Auth_Create(&nalcnt->authparam, &nalcnt->http);
#else
	if(DWC_Auth_Create(&nalcnt->authparam, &nalcnt->http) != DWCAUTH_E_NOERR) {
		DWC_Free(DWC_ALLOCTYPE_BASE, nalcnt, sizeof(DWCNalCnt));
		nalcnt = NULL;
		DWCi_SetError(DWC_ERROR_FATAL, -20100);
		return FALSE;
	}
#endif
	return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:			DWC_NASLoginProcess

  Description:	NASログイン処理進行

  Arguments:	なし

  Returns:		DWCNasLoginState
 *---------------------------------------------------------------------------*/
DWCNasLoginState DWC_NASLoginProcess(void)
{
	DWCAuthResult result;
	
	if(nalcnt == NULL)
		return DWC_NASLOGIN_STATE_DIRTY;
	
	/* [nakata] HTTP通信状態確認 */
	// [nakata] この部分の処理は必要なく、またバグを発生させていたので削除した(DWCのlogin処理に準ずる処理になった)
	//if(nalcnt->http.thread.id == 0)
	//	return DWC_NASLOGIN_STATE_IDLE;
	
	// [nakata] スレッドIDが存在しているため、動作によって分岐する
	switch(DWC_Auth_GetError()) {
	case DWCAUTH_E_NOERR:
		/* [nakata] HTTP通信中 */
		return DWC_NASLOGIN_STATE_HTTP;
	case DWCAUTH_E_FINISH:
		DWC_Auth_Destroy();
		DWC_Free(DWC_ALLOCTYPE_BASE, nalcnt, sizeof(DWCNalCnt));
		nalcnt = NULL;
		return DWC_NASLOGIN_STATE_SUCCESS;
	case DWCAUTH_E_ABORT:
		/* [nakata] 中断の場合にはエラーをセットしない */
		DWC_Auth_Destroy();
		DWC_Free(DWC_ALLOCTYPE_BASE, nalcnt, sizeof(DWCNalCnt));
		nalcnt = NULL;
		return DWC_NASLOGIN_STATE_CANCELED;
	default:
		DWC_Auth_GetResult(&result);
		DWC_Auth_Destroy();
		DWC_Free(DWC_ALLOCTYPE_BASE, nalcnt, sizeof(DWCNalCnt));
		nalcnt = NULL;
		DWCi_SetError(DWC_ERROR_AUTH_ANY, result.returncode);
		return DWC_NASLOGIN_STATE_ERROR;
	}
}

/*---------------------------------------------------------------------------*
  Name:			DWC_NASLoginAbort

  Description:	任天堂認証サーバログイン処理中断関数

  Arguments:	なし

  Returns:		なし
 *---------------------------------------------------------------------------*/
void DWC_NASLoginAbort(void)
{
	DWC_Auth_Abort();
}
