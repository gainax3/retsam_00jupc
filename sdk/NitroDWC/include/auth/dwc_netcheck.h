/*---------------------------------------------------------------------------*
  Project:  NitroDWC - auth - dwc_netcheck
  File:     dwc_netcheck.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_netcheck.h,v $
  Revision 1.19  2006/11/07 04:54:35  nakata
  DWC_SimpleNetcheck系の追加

  Revision 1.18  2006/04/06 09:21:33  nakata
  302エラーを無視するモードを設定する関数を追加。

  Revision 1.17  2006/04/04 07:47:35  nakata
  改行コード修正。

  Revision 1.16  2006/03/02 05:21:58  nakata
  DWC_Svcモジュール追加
  DWC_Util_Alloc/DWC_Util_Free関数の名前をDWC_Auth_Alloc/DWC_Auth_Free関数に名前を変更

  Revision 1.15  2005/12/21 12:02:19  nakata
  Rollback Auth related code to versiont "2005-11-21 08:00:00"
  Fixed retry times count bug in DWC_Auth.

  Revision 1.12  2005/09/12 06:19:29  nakata
  Added DWC_Netcheck_SetCustomBlacklist function.
  Refined DWC_Http timeout/receive finish detection.
  Modified some Makefile for successful compilation.

  Revision 1.11  2005/08/20 10:28:07  sasakit
  行末に\来ている行の対策。

  Revision 1.10  2005/08/20 07:01:18  sasakit
  ヘッダインクルードガードの書式を統一した。
  bm/dwc_init.h -> bm/dwc_bm_init.hに変更
  ヘッダにCopyrightをつけた。
  構造体の名前空間をできるだけルールにちかづけた。
  util_wifiidtool.hとdwc_backup.hの関係を若干修正。


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_NETCHECK_H_
#define DWC_NETCHECK_H_
/*****************************************************************************/
/*                                                                           */
/*  接続性検証ライブラリ DWC_Netcheck ヘッダファイル                         */
/*      dwc_netcheck.h                                                       */
/*          ver. 0.5                                                         */
/*                                                                           */
/*****************************************************************************/

#include "dwc_auth.h"

#ifdef __cplusplus
extern "C" {
#endif // [nakata] __cplusplus

//=============================================================================
// [nakata] 定義
//=============================================================================

// [nakata] DWCNetcheckのエラー状態を示す列挙型
typedef enum {
	DWCNETCHECK_E_NOERR,			// [nakata] 初期状態/処理中

	DWCNETCHECK_E_HTTPINITERR,		// [nakata] DWCHttpが初期化できない
	DWCNETCHECK_E_HTTPPARSEERR,		// [nakata] 受信したHTTPパケットが不正でパースできない
	DWCNETCHECK_E_HTTPERR,			// [nakata] DWCHttpに関する未分類のエラーが発生している
	DWCNETCHECK_E_MEMERR,			// [nakata] メモリ確保失敗
	DWCNETCHECK_E_AUTHINITERR,		// [nakata] DWCAuthが初期化できない
	DWCNETCHECK_E_AUTHERR,			// [nakata] DWCAuthに関する未分類のエラーが発生している
	DWCNETCHECK_E_302TWICE,			// [nakata] Wayport認証を行ったのに302エラーが出た
	DWCNETCHECK_E_CANTADDPOSTITEM,	// [nakata] HTTPパケットにPOSTアイテムを追加できない
	DWCNETCHECK_E_NASPARSEERR,		// [nakata] NASパケットとして正常にパースできない
	
	DWCNETCHECK_E_NETUNAVAIL,		// [nakata] (DWCNetcheck結果)ネットワーク接続不可能 
	DWCNETCHECK_E_NETAVAIL,			// [nakata] (DWCNetcheck結果)ネットワーク接続可能 

	DWCNETCHECK_E_MAX
} DWCNetcheckError;

// [nakata] DWCNetcheckの初期化パラメータ
typedef struct
{
	DWCAuthAlloc	alloc;	// [nakata] メモリ確保関数
	DWCAuthFree	free;	// [nakata] メモリ解放関数
	
	void *bmworkarea;		// [nakata] BM用領域(32bitアラインメントされた静的なメモリ領域)
} DWCNetcheckParam;

// [nakata] SIMPLE_CHECK用
typedef enum
{
	DWC_SIMPLENETCHECK_PROCESSING = 0,
	DWC_SIMPLENETCHECK_SUCCESS,
	DWC_SIMPLENETCHECK_FAIL,
	DWC_SIMPLENETCHECK_SYSFAIL,
	DWC_SIMPLENETCHECK_NOTINITIALIZED
} DWCSimpleNetcheckResult;

//=============================================================================
// [nakata] 関数
//=============================================================================

//-----------------------------------------------------------------------------
// 機能 : DWCNetcheckで使用する構造体の初期化/メモリ確保/スレッド実行
// 引数 : param - 初期化パラメータ
// 返値 : DWCNetcheckError
//-----------------------------------------------------------------------------
DWCNetcheckError DWC_Netcheck_Create(DWCNetcheckParam *param);

//-----------------------------------------------------------------------------
// 機能 : DWCNetcheckで使用したメモリの解放/ネットワーク切断
// 引数 : なし
// 返値 : なし
//-----------------------------------------------------------------------------
void DWC_Netcheck_Destroy(void);

//-----------------------------------------------------------------------------
// 機能 : DWCNetcheckスレッドアボート
// 引数 : なし
// 返値 : なし
//-----------------------------------------------------------------------------
void DWC_Netcheck_Abort(void);

//-----------------------------------------------------------------------------
// 機能 : DWCNetcheckスレッドが終了するのをOS_JoinThreadして待つ
// 引数 : なし
// 返値 : なし
//-----------------------------------------------------------------------------
void DWC_Netcheck_Join(void);

//-----------------------------------------------------------------------------
// 機能 : DWC_Netcheckの動作をポーリングするための関数
// 引数 : なし
// 返値 : BOOL - DWCNETCHECK_NOERR : まだ処理が終わっていない
//             - それ以外          : Netcheck処理が終わった
//-----------------------------------------------------------------------------
DWCNetcheckError DWC_Netcheck_GetError(void);

//-----------------------------------------------------------------------------
// 機能 : Mutexを使用せずにDWCNetcheckの結果を取得する
//        (Authスレッドが終了してから使うこと、DWC_Netcheck_Destroy前に使うこと)
// 引数 : なし
// 返値 : int
//-----------------------------------------------------------------------------
int DWC_Netcheck_GetReturnCode(void);

//-----------------------------------------------------------------------------
// 機能 : DWC_NetcheckのBlacklistサイトを設定する
// 引数 : const char * - BlacklistサイトのURL
// 返値 : なし
//-----------------------------------------------------------------------------
void DWC_Netcheck_SetCustomBlacklist(const char *url);

//-----------------------------------------------------------------------------
// 機能 : DWC_Netcheckが302エラーをネットワーク接続済みと認識するかどうかのフラグを設定する
// 引数 : allow302 - TRUE  - DWC_Netcheckは302エラーを接続扱いとする
//                   FALSE - DWC_Netcheckは302エラーを接続扱いとしない
// 返値 : なし
//-----------------------------------------------------------------------------
void DWC_Netcheck_SetAllow302(BOOL allow302);

//-----------------------------------------------------------------------------
// 機能 : SIMPLE_CHECKを初期化
// 引数 : タイムアウト時間(ミリ秒)
// 返値 : TRUE - SIMPLE_CHECKを開始した
//      : FALSE - SIMPLE_CHECKを開始できなかった
//-----------------------------------------------------------------------------
BOOL DWC_SimpleNetcheckAsync(int timeout);

//-----------------------------------------------------------------------------
// 機能 : SIMPLE_CHECKをprocessする
// 引数 : なし
// 返値 : DWCSimpleNetcheckResult
//-----------------------------------------------------------------------------
DWCSimpleNetcheckResult DWC_SimpleNetcheckProcess(void);

#ifdef __cplusplus
}
#endif // [nakata] __cplusplus

#endif // DWC_NETCHECK_H_
