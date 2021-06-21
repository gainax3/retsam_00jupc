/*---------------------------------------------------------------------------*
  Project:  NitroDWC -  - 
  File:     dwc_nd.c

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_nd.c,v $
  Revision 1.37  2007/03/19 09:27:38  nakata
  デバッグ表示をOS_TPrintfからDWC_Printfを使用するように変更。

  Revision 1.36  2006/09/05 05:37:09  takayama
  DWC_NdCleanupAsync関数の返り値を void から BOOL に変更した。

  Revision 1.35  2006/08/25 04:31:35  nakata
  未初期化時にCleanupしてもASSERTしないように変更

  Revision 1.34  2006/08/21 07:50:01  nakata
  Ndライブラリの初期化失敗時に発生するメモリリークを修正。

  Revision 1.33  2006/06/22 09:10:20  nakata
  削除した#pragma unusedを元に戻した

  Revision 1.32  2006/06/22 02:34:19  nakata
  ファイルダウンロード時のバッファサイズがDWC_ND_RECVBUF_MINよりも小さい場合にASSERTを追加。

  Revision 1.31  2006/06/22 02:03:04  nakata
  ダウンロードライブラリのバッファ不足エラーを取り除いた。(DWCErrorの種類を増やしたくないため)

  Revision 1.30  2006/06/22 00:49:50  nakata
  ダウンロードライブラリのバッファ不足でOS_Panicしないよう変更

  Revision 1.29  2006/06/08 06:07:33  nakata
  コメント追加

  Revision 1.28  2006/06/08 06:06:54  nakata
  SVLがbase64デコードされて返されるようになったため、DWCi_Ndに渡す前にbase64エンコードするように変更

  Revision 1.27  2006/06/05 05:49:36  nakata
  ダウンロード先バッファの大きさとファイルの大きさを比較するASSERT条件式が誤っていた問題を修正

  Revision 1.26  2006/05/30 04:33:51  nakata
  SVLのエラー番号のみを残してNdからDWC_Errorを設定する変更を一部に施した

  Revision 1.25  2006/05/29 11:37:00  nakata
  SVLエラーをそのまま返した場合Ndライブラリを解放する対処が行われないため、NdライブラリによってSVLのエラーを上書きするように再修正

  Revision 1.24  2006/05/29 11:13:33  nakata
  SVLのDWCErrorをNdが上書きしないよう変更

  Revision 1.23  2006/05/26 04:09:59  nakata
  DWCError発生時にFATALエラーを上書きするのをやめる

  Revision 1.22  2006/05/26 02:42:44  nakata
  DWCError周りの追加

  Revision 1.21  2006/05/25 10:42:41  nakata
  DWC_Error周りの追加

  Revision 1.20  2006/05/25 05:08:21  nakata
  DWC_Nd/DWC_SVLにDWCエラーを返す機能を追加したことに伴う変更

  Revision 1.19  2006/05/17 09:01:11  nakata
  NHTTP/DWC_Ndライブラリを5/17版に更新したことに伴う修正

  Revision 1.18  2006/05/17 00:50:10  nakata
  dwc_error.hの変更にともなう修正

  Revision 1.17  2006/05/15 01:18:13  nakata
  5/12版のDWC_Ndライブラリに対応

  Revision 1.16  2006/05/12 11:24:48  nakata
  大木さんの指摘に対応

  Revision 1.15  2006/05/10 11:50:58  nakata
  Ndライブラリの仕様変更に伴って初期化関数を変更

  Revision 1.14  2006/05/10 04:45:34  nakata
  初期化時に設定されるDWCエラーが正しくなかった問題を修正

  Revision 1.13  2006/05/02 00:57:07  nakata
  ダミートークンを使用するように変更

  Revision 1.12  2006/05/02 00:27:10  nakata
  DWC_Ndを5月2日版に更新

  Revision 1.11  2006/04/28 13:14:03  nakata
  SVL取得失敗時のエラーをネットワークエラーに変更

  Revision 1.10  2006/04/28 12:23:27  nakata
  定数名変更に伴う修正

  Revision 1.9  2006/04/28 10:08:43  nakata
  DWC_NdInitAsyncにpassword追加
  DWC_NdGetFileAsyncの仕様書変更に伴う関数変更
  各種定数をガイドラインに沿ったものに変更するヘッダを追加

  Revision 1.8  2006/04/28 08:12:06  nakata
  SVL取得スレッドが終了しないバグを修正

  Revision 1.7  2006/04/27 12:46:24  takayama
  svc(SVC) -> svl(SVL)

  Revision 1.6  2006/04/27 05:27:54  nakata
  エラーコード関連の修正
  DWCエラー発生時には各関数の実行を不可能にした

  Revision 1.5  2006/04/26 10:52:39  nakata
  DWC_Ndをエラーコードを設定するように変更

  Revision 1.4  2006/04/26 08:35:13  nakata
  DWC_Ndをdwc.hさえインクルードすれば動作するよう変更

  Revision 1.3  2006/04/26 05:09:13  nakata
  初期化失敗時にメモリが解放されないバグを修正

  Revision 1.2  2006/04/26 04:19:23  nakata
  Nd/NHTTPを4月26日版に更新

  Revision 1.1  2006/04/25 11:46:46  nakata
  DWC_Ndラッパを追加



  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>

#include <base/dwc_report.h>
#include <base/dwc_memfunc.h>
#include <base/dwc_nasfunc.h>
#include <base/dwc_error.h>
#include <base/dwc_nd.h>
#include <nd/DWCi_Nd.h>
#include <auth/dwc_svl.h>
#include <auth/util_base64.h>

/* -------------------------------------------------------------------------
        pre-processing switch
   ------------------------------------------------------------------------- */

//


/* -------------------------------------------------------------------------
        define
   ------------------------------------------------------------------------- */

// [nakata] Ndライブラリが使用するSVL
#define DWC_ND_SVL				"9000"
#define SVLTHREADSTACKSIZE		2048
#define SVLHOSTNAMEMAX			255
#define GAMECDMAX				32
#define PASSWDMAX				32
#define TOKEN_LENGTH			(300/3*4)
/* -------------------------------------------------------------------------
        macro
   ------------------------------------------------------------------------- */

//


/* -------------------------------------------------------------------------
        struct
   ------------------------------------------------------------------------- */

// [nakata] DWCNdの各パラメータを保存するための構造体
typedef struct {
	// [nakata] 初期化時パラメータ
	DWCNdCallback			callback;
	DWCNdCleanupCallback	cleanupcallback;
	char					gamecd[GAMECDMAX];
	char					passwd[PASSWDMAX];
	
	// [nakata] DWCi_Nd初期化フラグ
	BOOL			ndinitialized;
	
	// [nakata] SVLトークン格納領域
	DWCSvlResult	svlresult;
	
	// [nakata] SVL取得待ちスレッド
	u8				svlthreadstack[SVLTHREADSTACKSIZE];
	OSThread		svlthread;
} DWCNdCnt;

/* -------------------------------------------------------------------------
        variable
   ------------------------------------------------------------------------- */

// [nakata] Init時の各パラメータを保存するための領域
static DWCNdCnt *dwcndcnt = NULL;

/* -------------------------------------------------------------------------
        prototype
   ------------------------------------------------------------------------- */

//


/* -------------------------------------------------------------------------
        function - internal
   ------------------------------------------------------------------------- */

/*---------------------------------------------------------------------------*
  Name:			DWCi_NdAlloc

  Description:	Ndライブラリ内で使用されるメモリ管理関数

  Arguments:	size  - 確保するメモリブロックの容量
				align - 確保するメモリブロックのアラインメント
  Returns:		NULL以外 - 確保されたメモリブロックへのポインタ
				NULL     - メモリ確保失敗
 *---------------------------------------------------------------------------*/
static void *DWCi_NdAlloc(u32 size, int align)
{
	return DWC_AllocEx(DWC_ALLOCTYPE_ND, size, align);
}

/*---------------------------------------------------------------------------*
  Name:			DWCi_NdFree

  Description:	Ndライブラリ内で使用されるメモリ管理関数

  Arguments:	ptr - 解放されるメモリブロックの容量
  Returns:		なし
 *---------------------------------------------------------------------------*/
static void DWCi_NdFree(void *ptr)
{
	DWC_Free(DWC_ALLOCTYPE_ND, ptr, 0);
}

/*---------------------------------------------------------------------------*
  Name:			DWCi_NdCallback

  Description:	Ndライブラリコールバックを内部的に処理しエラーコードの設定などを行う関数

  Arguments:	DWCNdCallback型を参照
  Returns:		なし
 *---------------------------------------------------------------------------*/
static void DWCi_NdCallback(DWCNdCallbackReason reason, DWCNdError error, int servererror)
{
	SDK_ASSERT(dwcndcnt);
	SDK_ASSERT(dwcndcnt->callback);
	
	// [nakata] エラーがないならばそのままコールバック関数を呼び出す
	if(error == DWC_ND_ERROR_NONE) {
		dwcndcnt->callback(reason, error, servererror);
		return;
	}
	
	// [nakata] エラーが発生している場合はDWCi_SetErrorを呼ぶ必要がある
	switch(error) {
	case DWC_ND_ERROR_ALLOC:
		DWCi_SetError(DWC_ERROR_FATAL, DWC_ECODE_SEQ_ADDINS + DWC_ECODE_FUNC_ND + DWC_ECODE_TYPE_ND_ALLOC);
		break;
	case DWC_ND_ERROR_BUSY:
		OS_Panic("You can't request more than one download process to DWC_Nd library.");
		break;
	case DWC_ND_ERROR_HTTP:
		DWCi_SetError(DWC_ERROR_ND_HTTP, DWC_ECODE_SEQ_ADDINS + DWC_ECODE_FUNC_ND + DWC_ECODE_TYPE_ND_HTTP);
		break;
	case DWC_ND_ERROR_BUFFULL:
		DWCi_SetError(DWC_ERROR_ND_ANY, DWC_ECODE_SEQ_ADDINS + DWC_ECODE_FUNC_ND + DWC_ECODE_TYPE_ND_BUFFULL);
		break;
	case DWC_ND_ERROR_DLSERVER:
		DWCi_SetError(DWC_ERROR_ND_ANY, DWC_ECODE_SEQ_ADDINS + DWC_ECODE_FUNC_ND - servererror);
		break;
	case DWC_ND_ERROR_CANCELED:
		break;
	case DWC_ND_ERROR_PARAM:
		OS_Panic("Some Parameters passed to DWC_Nd library are invalid.");
		break;
	case DWC_ND_ERROR_FATAL:
		DWCi_SetError(DWC_ERROR_FATAL, DWC_ECODE_SEQ_ADDINS + DWC_ECODE_FUNC_ND + DWC_ECODE_TYPE_ND_FATAL);
		break;
	default:
		OS_Panic("DWCi_NdCallback: Unknown error type.");
	}
	
	// [nakata] コールバック関数を呼び出す
	dwcndcnt->callback(reason, error, servererror);
}

/*---------------------------------------------------------------------------*
  Name:			DWCi_NdCleanupCallback

  Description:	Ndライブラリクリーンアップコールバックを内部的に処理しエラーコードの設定などを行う関数

  Arguments:	DWCNdCleanupCallback型を参照
  Returns:		なし
 *---------------------------------------------------------------------------*/
static void DWCi_NdCleanupCallback(void)
{
	DWCNdCleanupCallback callback;
	
	SDK_ASSERT(dwcndcnt);
	
	// [nakata] スレッドにJoin
	OS_JoinThread(&dwcndcnt->svlthread);
	
	// [nakata] ユーザー設定したコールバックを保存しておく
	callback = dwcndcnt->cleanupcallback;
	
	// [nakata] メモリ解放(ここでdwcndcntがNULLになるためdwcndcnt->ndinitializedのクリアは必要ない)
	DWCi_NdFree(dwcndcnt);
	dwcndcnt = NULL;
	
	// [nakata] ユーザー設定したコールバックを呼び出す
	if(callback != NULL)
		callback();
}

/* -------------------------------------------------------------------------
        function - external
   ------------------------------------------------------------------------- */

/*---------------------------------------------------------------------------*
  Name:			DWCi_SVLTokenWaitThread

  Description:	SVLトークン取得待ちスレッド関数

  Arguments:	なし
  Returns:		なし
 *---------------------------------------------------------------------------*/
static void DWCi_SVLTokenWaitThread(void *arg)
{
#pragma unused(arg)
	DWCSvlState	svlstate;
	char		url[SVLHOSTNAMEMAX+1];
	char		token[TOKEN_LENGTH];
	int			errorcode;

	SDK_ASSERT(dwcndcnt);
	SDK_ASSERT(dwcndcnt->callback);
	
	for(;;) {
		svlstate = DWC_SVLProcess();
		if(svlstate == DWC_SVL_STATE_SUCCESS) {
			// [nakata] SVLトークン取得成功、Ndライブラリ初期化開始
			if(strlen(dwcndcnt->svlresult.svlhost)+strlen("https:///download") > SVLHOSTNAMEMAX) {
				DWC_Printf(DWC_REPORTFLAG_ERROR, "DWCi_SVLTokenWaitThread: DWCi_NdStartup too long svlhost.\n");
				DWCi_NdCallback(DWC_ND_CBREASON_INITIALIZE, DWC_ND_ERROR_FATAL, -1);
				return;
			}
			OS_SNPrintf(url, SVLHOSTNAMEMAX+1, "https://%s/download", dwcndcnt->svlresult.svlhost);
			
			// [nakata] SVLがトークンをbase64デコードして返すように変更されたため、DWCi_Ndに渡す前にbase64エンコードする
			if(DWC_Auth_Base64Encode(dwcndcnt->svlresult.svltoken, strlen(dwcndcnt->svlresult.svltoken), token, TOKEN_LENGTH) < 0) {
				DWC_Printf(DWC_REPORTFLAG_ERROR, "DWCi_SVLTokenWaitThread: Can't encode SVL token.\n");
				DWCi_NdCallback(DWC_ND_CBREASON_INITIALIZE, DWC_ND_ERROR_FATAL, -1);
				return;
			}
			
			if(DWCi_NdStartup(DWCi_NdAlloc, DWCi_NdFree, url, token, dwcndcnt->gamecd, dwcndcnt->passwd, DWCi_NdCallback) == FALSE) {
				// [nakata] Ndライブラリ初期化失敗
				DWC_Printf(DWC_REPORTFLAG_ERROR, "DWCi_SVLTokenWaitThread: DWCi_NdStartup failed.\n");
				DWCi_NdCallback(DWC_ND_CBREASON_INITIALIZE, DWC_ND_ERROR_FATAL, -1);
				return;
			}
			
			// [nakata] ndライブラリ初期化成功
			DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWCi_SVLTokenWaitThread: DWCi_NdStartup successed.\n");
			dwcndcnt->ndinitialized = TRUE;
			DWCi_NdCallback(DWC_ND_CBREASON_INITIALIZE, DWC_ND_ERROR_NONE, -1);
			return;
		}
		else if(svlstate == DWC_SVL_STATE_ERROR) {
			// [nakata] SVLトークン取得失敗
			// [nakata] DWCエラーは設定済み
			DWC_Printf(DWC_REPORTFLAG_ERROR, "DWCi_SVLTokenWaitThread: Can't get SVL token.\n");
			
			// [nakata] コールバック関数を呼び出す
			(void)DWC_GetLastError(&errorcode);
			DWCi_SetError(DWC_ERROR_ND_ANY, errorcode);
			dwcndcnt->callback(DWC_ND_CBREASON_INITIALIZE, DWC_ND_ERROR_HTTP, -1);
			
			// [nakata] DWCエラーは設定済みだが、NdライブラリはSVLと違って対処として解放しなければならないため
			//          エラーを上書き
			//DWCi_NdCallback(DWC_ND_CBREASON_INITIALIZE, DWC_ND_ERROR_HTTP, -1);
			return;
		}
		else if(svlstate == DWC_SVL_STATE_CANCELED) {
			// [nakata] SVLトークン取得キャンセル
			//DWCi_NdCallback(DWC_ND_CBREASON_INITIALIZE, DWC_ND_ERROR_HTTP, -1);
			
			// [nakata] DWCエラーは設定済みだが、Ndライブラリキャンセル扱いとしてエラーを上書き
			DWCi_NdCallback(DWC_ND_CBREASON_INITIALIZE, DWC_ND_ERROR_CANCELED, -1);
		}
		OS_Sleep(100);
	}
	return;
}

/*---------------------------------------------------------------------------*
  Name:			DWC_NdInitAsync

  Description:	Ndライブラリ初期化関数

  Arguments:	callback - Ndライブラリが呼び出す通知コールバック関数へのポインタ
                password - ダウンロードサーバへのアクセスに必要なパスワード
  Returns:		TRUE  - 初期化成功
				FALSE - 初期化失敗
 *---------------------------------------------------------------------------*/
BOOL DWC_NdInitAsync(DWCNdCallback callback, char *gamecd, char *passwd)
{
	SDK_ASSERT(callback);
	SDK_ASSERT(dwcndcnt == NULL);
	SDK_ASSERT(gamecd);
	SDK_ASSERT(passwd);
	
	// [nakata] DWCエラーが発生している場合ははじく
	if(DWCi_IsError()) {
		return FALSE;
	}
	
	// [nakata] コントロール構造体初期化
	dwcndcnt = DWCi_NdAlloc(sizeof(DWCNdCnt), 32);
	SDK_ASSERT(dwcndcnt);
	dwcndcnt->ndinitialized = FALSE;
	
	MI_CpuClear8(dwcndcnt, sizeof(DWCNdCnt));
	dwcndcnt->callback	= callback;
	strncpy(dwcndcnt->gamecd, gamecd, GAMECDMAX-1);
	strncpy(dwcndcnt->passwd, passwd, PASSWDMAX-1);
	
	// [nakata] SVLトークン取得開始
	if(DWC_SVLGetTokenAsync(DWC_ND_SVL, &dwcndcnt->svlresult) == FALSE) {
		// [nakata] SVLトークン取得処理開始失敗
		// [nakata] DWCエラーはSVL内部で設定済み
		DWC_Printf(DWC_REPORTFLAG_ERROR, "DWC_NdInitAsync: DWC_SVLGetTokenAsync failed.\n");
		
		// [nakata] メモリ解放
		DWCi_NdFree(dwcndcnt);
		dwcndcnt = NULL;
		return FALSE;
	}
	DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWC_NdInitAsync: DWC_SVLGetTokenAsync started.\n");
	
	// [nakata] SVLトークン受信待ちスレッド起動
	OS_CreateThread(&dwcndcnt->svlthread, DWCi_SVLTokenWaitThread, NULL, dwcndcnt->svlthreadstack+SVLTHREADSTACKSIZE, SVLTHREADSTACKSIZE, 16);
	OS_WakeupThreadDirect(&dwcndcnt->svlthread);
	
	return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:			DWC_NdCleanupAsync

  Description:	Ndライブラリ解放関数

  Arguments:	callback - Ndライブラリが呼び出す解放完了通知コールバック関数へのポインタ
  Returns:		なし
 *---------------------------------------------------------------------------*/
BOOL DWC_NdCleanupAsync(DWCNdCleanupCallback callback)
{
	if(dwcndcnt == NULL)
		return FALSE;
	
	dwcndcnt->cleanupcallback = callback;

	if(dwcndcnt->ndinitialized == FALSE)
		DWCi_NdCleanupCallback();
	else
		DWCi_NdCleanupAsync(DWCi_NdCleanupCallback);
	return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:			DWC_NdSetAttr

  Description:	ダウンロード時属性設定関数

  Arguments:	attr1 - 属性文字列1
				attr2 - 属性文字列2
				attr3 - 属性文字列3
  Returns:		TRUE  - 属性設定成功
				FALSE - 属性設定失敗
 *---------------------------------------------------------------------------*/
BOOL DWC_NdSetAttr(const char* attr1, const char* attr2, const char* attr3)
{
	SDK_ASSERT(dwcndcnt);
	
	// [nakata] DWCエラーが発生している場合ははじく
	if(DWCi_IsError()) {
		return FALSE;
	}
	
	// [nakata] 返り値によってエラーコードを設定する
	return DWCi_NdSetAttr((char *)attr1, (char *)attr2, (char *)attr3);
}

/*---------------------------------------------------------------------------*
  Name:			DWC_NdGetFileListNumAsync

  Description:	ファイルリスト総エントリ数取得処理開始関数

  Arguments:	entrynum - 通信完了時にファイルリスト総エントリ数が格納される領域へのポインタ
  Returns:		TRUE  - 通信が開始され処理完了待ち状態
				FALSE - 処理失敗
 *---------------------------------------------------------------------------*/
BOOL DWC_NdGetFileListNumAsync(int* entrynum)
{
	SDK_ASSERT(dwcndcnt);
	
	// [nakata] DWCエラーが発生している場合ははじく
	if(DWCi_IsError()) {
		return FALSE;
	}

	return DWCi_NdGetFileListNumAsync(entrynum);
}

/*---------------------------------------------------------------------------*
  Name:			DWC_NdFileListAsync

  Description:	ファイルリスト取得処理開始関数

  Arguments:	filelist - 取得されたファイルリストが格納される配列へのポインタ
				offset   - 取得するファイルリストの開始位置のオフセット
				num      - 取得するファイルリストの最大長
  Returns:		TRUE  - 通信が開始され処理完了待ち状態
				FALSE - 処理失敗
 *---------------------------------------------------------------------------*/
BOOL DWC_NdGetFileListAsync(DWCNdFileInfo* filelist, unsigned  offset, unsigned int num)
{
	SDK_ASSERT(dwcndcnt);
	
	// [nakata] DWCエラーが発生している場合ははじく
	if(DWCi_IsError()) {
		return FALSE;
	}
	
	// [nakata] ファイルリスト領域を0クリア
	MI_CpuClear8(filelist, sizeof(DWCNdFileInfo)*num);
	
	return DWCi_NdGetFileListAsync(filelist, (int)offset, (int)num);
}

/*---------------------------------------------------------------------------*
  Name:			DWC_NdGetFileAsync

  Description:	ファイル取得処理開始関数

  Arguments:	filename - 取得するファイルの名前
				buf      - 取得されたファイルのデータが格納される領域へのポインタ
				size     - 取得されたファイルのデータが格納される領域のサイズ
  Returns:		TRUE  - 通信が開始され処理完了待ち状態
				FALSE - 処理失敗
 *---------------------------------------------------------------------------*/
BOOL DWC_NdGetFileAsync(DWCNdFileInfo *fileinfo, char* buf, unsigned int size)
{
#pragma unused(size)
	SDK_ASSERT(dwcndcnt);
	SDK_ASSERT(fileinfo);
	SDK_ASSERT(buf);
	SDK_ASSERT(size >= DWC_ND_RECVBUF_MIN);
	SDK_ASSERT(size >= fileinfo->size);
	
	// [nakata] DWCエラーが発生している場合ははじく
	if(DWCi_IsError()) {
		return FALSE;
	}
	
	return DWCi_NdGetFileAsync(fileinfo, buf, size);
}

/*---------------------------------------------------------------------------*
  Name:			DWC_NdCancelAsync

  Description:	実行中処理の中断通知関数

  Arguments:	なし
  Returns:		TRUE  - 処理中断に成功したかあるいは処理を実行していない
				FALSE - 処理失敗
 *---------------------------------------------------------------------------*/
BOOL DWC_NdCancelAsync(void)
{
	SDK_ASSERT(dwcndcnt);
	
	// [nakata] DWCエラーが発生している場合ははじく
	if(DWCi_IsError()) {
		return FALSE;
	}
	
	return DWCi_NdCancelAsync();
}

/*---------------------------------------------------------------------------*
  Name:			DWC_NdGetProgress

  Description:	実行中処理の進行度取得関数

  Arguments:	received   - 受信済みデータのバイト数を格納する領域へのポインタ
				contentlen - 受信中のデータのContent-Lengthを格納する領域へのポインタ
  Returns:		TRUE  - 進行度取得成功
				FALSE - 進行度取得失敗、あるいは処理を実行していない
 *---------------------------------------------------------------------------*/
BOOL DWC_NdGetProgress(u32* received, u32* contentlen)
{
	SDK_ASSERT(dwcndcnt);
	
	// [nakata] DWCエラーが発生している場合ははじく
	if(DWCi_IsError()) {
		return FALSE;
	}

	return DWCi_NdGetProgress(received, contentlen);
}
