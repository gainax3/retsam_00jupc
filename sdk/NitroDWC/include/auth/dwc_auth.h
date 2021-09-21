/*---------------------------------------------------------------------------*
  Project:  NitroDWC - auth - dwc_auth
  File:     dwc_auth.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_auth.h,v $
  Revision 1.42  2006/06/13 00:00:22  nakata
  ブラウザ向けのエラー項目追加

  Revision 1.41  2006/04/27 07:06:09  takayama
  ingamesnを10文字から25文字に拡張。

  Revision 1.40  2006/03/09 04:06:36  nakata
  一部改行コードが正常でなかった部分を修正

  Revision 1.39  2006/03/02 05:21:58  nakata
  DWC_Svcモジュール追加
  DWC_Util_Alloc/DWC_Util_Free関数の名前をDWC_Auth_Alloc/DWC_Auth_Free関数に名前を変更

  Revision 1.38  2006/03/01 02:43:53  nakata
  CAL取得機能をdwc_auth.cから分離した

  Revision 1.37  2006/02/24 05:31:08  nakata
  必要ないコメントを削除した。

  Revision 1.36  2006/01/17 07:15:38  nakata
  Fixed the bug related to determining ingamesn length in DWCi_RemoteLoginProcess().

  Revision 1.35  2005/12/26 06:57:43  nakata
  Changed DWC_AUTH_INGAMESN_NOTCHECKED to DWC_AUTH_INGAMESN_NOT_CHECKED.
  Changed DWC_INGAMESN_NOTCHECKED to DWC_INGAMESN_NOT_CHECKED.

  Revision 1.34  2005/12/26 06:43:19  nakata
  Moved ingamesn check functions from dwc_common.c to dwc_login.c.

  Revision 1.33  2005/12/26 04:23:17  nakata
  Added NAS return code 40 related functions to Auth.
  Added DWC_GetIngamesnCheckResult() to dwc_common.c/dwc_common.h.
  Fixed the bug that incorrect ingamesn is sent when auth is called in netcheck.

  Revision 1.32  2005/12/21 12:02:19  nakata
  Rollback Auth related code to versiont "2005-11-21 08:00:00"
  Fixed retry times count bug in DWC_Auth.

  Revision 1.21  2005/11/17 07:52:34  nakata
  Now DWC_Auth/DWC_Netcheck/DWC_Http don't use DWC_Alloc/DWC_Free.
  DWC_Netcheck sends whole HTTP packet including HTTP header to NAS
  when receives 302 message from conntest.

  Revision 1.20  2005/10/07 13:10:53  nakata
  Added SSL priority setting code to DWC_Http. But disabled by DWC_HTTP_SETSSLPRIO switch.
  Fixed ingamesn length detection part. It used strlen and now it uses wcslen for ingamesn.

  Revision 1.19  2005/09/07 08:01:02  nakata
  Modified DWC_Auth_GetError and DWC_Auth_GetResult to work when DWC_Auth
  is not initialized

  Revision 1.18  2005/08/22 11:48:02  nakata
  Add runtime custom nas setting function to DWC_Auth.

  Revision 1.17  2005/08/20 07:01:18  sasakit
  ヘッダインクルードガードの書式を統一した。
  bm/dwc_init.h -> bm/dwc_bm_init.hに変更
  ヘッダにCopyrightをつけた。
  構造体の名前空間をできるだけルールにちかづけた。
  util_wifiidtool.hとdwc_backup.hの関係を若干修正。


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_AUTH_H_
#define DWC_AUTH_H_

#include <nitro.h>

#include "dwc_http.h"

#include "ac/dwc_ac.h"

#ifdef __cplusplus
extern "C" {
#endif // [nakata] __cplusplus

//=============================================================================
// [nakata] 定義
//=============================================================================

// [nakata] 定数定義
#define DWCAUTH_INGAMESN_LEN	(25+1)
#define DWCAUTH_GSBRCDBYTES		(11+1)

// [nakata] DWCAuthの結果を格納する構造体

// [nakata] NASエラーコード一覧
// 0または正の値	成功
//
// -20100	DS側で発生したエラー(メモリ不足/ライブラリ初期化失敗など)により認証を完了できない
// -20101	サーバ側で以下に定義したもの以外のエラーが発生し認証を完了できない
// -20102	送信したMACアドレスがNASのBANリストに入っている
// -20103	ログイン/新規ユーザー作成に使用したユーザー名から抽出したMACアドレスがDSのものでない
// -20104	IDがすでに使用されており、IDを変化させてリトライをしても新規作成に失敗した
// -20105	IDが存在しないためログインできない
// -20106	このDSから作ることができるID数の上限を超えてIDを作ろうとしている
// -20107	送信したゲームコードがNASに登録されていない
// -20108	IDの使用期限が切れている
// -20109	DSから必要な情報が送られてきていない、不正アクセス疑い
// -20110	送信したゲームコードではWiFiコネクションサービスが終了している
//
// -20200	NAS内部プロセスエラー
// -20301	NAS機能未実装
//
// -20403	閲覧権限なし(HTTPサーバエラー)
// -20404	閲覧対象ファイルなし(HTTPサーバエラー)
// -20500	サーバインターナルエラー(HTTPサーバエラー)
// -20503	HTTPサーバ処理エラー/RADIUSエラー(HTTPサーバエラー)

typedef struct
{
	int returncode;			// [nakata] NASエラーコード
	
	// [nakata] 認証の結果得られた各種情報(文字列)
	char retcode[3+1];		// [nakata] HTTPリザルト
	char datetime[14+1];	// [nakata] YYYYMMDDHHMMSS形式のサーバ時間
	char locator[50+1];		// [nakata] GameSpyサーバのドメイン名
	char token[300+1];		// [nakata] トークン文字列
	char challenge[8+1];	// [nakata] チャレンジコード
	char cookie[64+1];		// [nakata] クッキー
} DWCAuthResult;

// [nakata] DWCAuthのエラー状態を示す列挙型
typedef enum
{
	DWCAUTH_E_NOERR,				// [nakata] 初期状態/処理中
	DWCAUTH_E_BMINITERR,			// [nakata] DSのFlashアクセス用ライブラリ初期化失敗
	DWCAUTH_E_MEMERR,				// [nakata] メモリ確保失敗
	DWCAUTH_E_ACERR,				// [nakata] 自動接続ライブラリから接続状況を取得できない
	DWCAUTH_E_HTTPINITERR,			// [nakata] DWCHttpが初期化できない
	DWCAUTH_E_CALERR,				// [nakata] CAL情報取得に失敗した
	DWCAUTH_E_INVALIDPARAM,			// [nakata] 不正なパラメータが与えられている
	DWCAUTH_E_CANTADDHTTPHEADER,	// [nakata] HTTPパケットにヘッダアイテムを追加できない
	DWCAUTH_E_CANTADDPOSTITEM,		// [nakata] HTTPパケットにPOSTアイテムを追加できない
	DWCAUTH_E_DNSERR,				// [nakata] DNSエラー
	DWCAUTH_E_CANTSTARTHTTPTHREAD,	// [nakata] HTTPスレッドを開始できない
	DWCAUTH_E_HTTPCONERR,			// [nakata] TCP接続を確立できない
	DWCAUTH_E_HTTPPARSEERR,			// [nakata] 受信したHTTPパケットが不正でパースできない
	DWCAUTH_E_HTTPERR,				// [nakata] DWCHttpに関する未分類のエラーが発生している
	DWCAUTH_E_NASPARSEERR,			// [nakata] 受信したNASの応答が不正でパースできない
	DWCAUTH_E_BMERR,				// [nakata] DSのFlash操作に失敗した
	DWCAUTH_E_NASALREADYEXIST,		// [nakata] NAS上に作ろうとしたアカウントが既に存在していた
	DWCAUTH_E_NASACCTEXPIRE,		// [nakata] ログインしようとしたアカウントはNAS上で期限切れだった
	DWCAUTH_E_NASERR,				// [nakata] NASからエラーが帰ってきている
	DWCAUTH_E_TOOMANYRETRY,			// [nakata] リトライが多すぎるのでエラー
	DWCAUTH_E_ABORT,
	DWCAUTH_E_FINISH,				// [nakata] 認証に成功した
	DWCAUTH_E_NOTINITIALIZED,		// [nakata] 初期化されていない
	DWCAUTH_E_MAX
} DWCAuthError;

// [nakata] DWC_Authを初期化するために必要な情報を格納する構造体
typedef struct
{
	// [nakata] これらのパラメータは全てNULL終端している必要がある
	unsigned short ingamesn[DWCAUTH_INGAMESN_LEN];	// [nakata] ゲーム内スクリーン名
	char           gsbrcd[DWCAUTH_GSBRCDBYTES];		// [nakata] Gamespy ID Branch Code
	
	// [nakata] メモリ管理関数
	DWCAuthAlloc	alloc;
	DWCAuthFree 	free;
} DWCAuthParam;

// [nakata] ingamesnチェック結果列挙子
typedef enum
{
	DWC_AUTH_INGAMESN_NOT_CHECKED	= 0,
	DWC_AUTH_INGAMESN_VALID			= 1,
	DWC_AUTH_INGAMESN_INVALID		= 2
} DWCAuthIngamesnCheckResult;

extern DWCAuthIngamesnCheckResult DWCauthingamesncheckresult;

//=============================================================================
// [nakata] 関数
//=============================================================================

//-----------------------------------------------------------------------------
// 機能 : DWC_Authで使用するカスタムNASアドレスを設定する
// 引数 : nasaddr - 使用するNASのURL文字列
// 返値 : なし
//-----------------------------------------------------------------------------
void DWC_Auth_SetCustomNas(const char *nasaddr);

//-----------------------------------------------------------------------------
// 機能 : DWCAuthで使用する構造体の初期化/メモリ確保
// 引数 : param - 初期化パラメータ
//      : http  - DWCAuthが使用するDWCHttp構造体へのポインタ
// 返値 : DWCAuthError
//-----------------------------------------------------------------------------
DWCAuthError DWC_Auth_Create(DWCAuthParam *param, void *http);
DWCAuthError DWC_Auth_Create_WFCUtil(DWCAuthParam *param, void *http);

//-----------------------------------------------------------------------------
// 機能 : DWCAuthアボート
//        
// 引数 : なし
// 返値 : なし
//-----------------------------------------------------------------------------
void DWC_Auth_Abort(void);

//-----------------------------------------------------------------------------
// 機能 : DWCAuthで使用したメモリの解放/ネットワーク切断
// 引数 : なし
// 返値 : なし
//-----------------------------------------------------------------------------
void DWC_Auth_Destroy(void);

//-----------------------------------------------------------------------------
// 機能 : DWCAuthスレッドが終了するのをOS_JoinThreadして待つ
// 引数 : なし
// 返値 : なし
//-----------------------------------------------------------------------------
void DWC_Auth_Join(void);

//-----------------------------------------------------------------------------
// 機能 : 排他的にDWCAuthのエラーコードを取得する
//        (DWCAuthの状態をポーリングする場合にも使える)
// 引数 : なし
// 返値 : DWCAuthError
//-----------------------------------------------------------------------------
DWCAuthError DWC_Auth_GetError(void);

//-----------------------------------------------------------------------------
// 機能 : Mutexを使用せずにDWCAuthの結果を取得する
//        (Authスレッドが終了してから使うこと、DWC_Auth_Destroy前に使うこと)
// 引数 : なし
// 返値 : DWCAuthResult
//-----------------------------------------------------------------------------
void DWC_Auth_GetResult(DWCAuthResult *result);

#ifdef __cplusplus
}
#endif // [nakata] __cplusplus


#endif // DWC_AUTH_H_
