/*---------------------------------------------------------------------------*
  Project:  NitroDWC - base - dwc_nasfunc
  File:     dwc_nasfunc.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_nasfunc.h,v $
  Revision 1.12  2006/11/17 06:57:56  nakata
  不正文字列チェックのヘッダを追加

  Revision 1.11  2006/05/25 09:07:18  nakata
  DWC_NASLOGIN_STATE_CANCELEDを追加

  Revision 1.10  2006/04/27 12:46:24  takayama
  svc(SVC) -> svl(SVL)

  Revision 1.9  2006/04/05 02:15:22  nakata
  大木さんに指摘された問題点を修正。

  Revision 1.8  2006/03/27 05:49:47  nakata
  プレフィックスNALをNasLoginに変更。

  Revision 1.7  2006/03/23 05:16:01  nakata
  DWC_NAL関係のコードを追加。

  Revision 1.6  2006/03/20 07:47:24  nakata
  SVLの仕様修正に対応した変更。

  Revision 1.5  2006/03/20 05:02:33  nakata
  *** empty log message ***

  Revision 1.4  2006/03/17 07:58:39  nakata
  コメント形式を変更。

  Revision 1.3  2006/03/09 04:11:44  nakata
  DWC_GetDateTime関数を追加

  Revision 1.2  2006/03/07 01:57:40  nakata
  DWC_GetNasTime関数のリファレンスをマニュアルに追加。

  Revision 1.1  2006/03/07 01:18:16  nakata
  NASに関連する機能追加をdwc_nasfunc.c/dwc_nasfunc.hに集約する際に必要な変更を行った。


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_NASFUNC_H_
#define DWC_NASFUNC_H_

#include "auth/dwc_auth.h"
#include "auth/dwc_svl.h"
#include "auth/dwc_prof.h"

#ifdef __cplusplus
extern "C" {
#endif

/* =======================================================================
        enum
   ======================================================================== */

/* [nakata] ingamesnチェック結果列挙子 */
typedef enum {
	DWC_INGAMESN_NOT_CHECKED	= DWC_AUTH_INGAMESN_NOT_CHECKED,
	DWC_INGAMESN_VALID			= DWC_AUTH_INGAMESN_VALID,
	DWC_INGAMESN_INVALID		= DWC_AUTH_INGAMESN_INVALID
} DWCIngamesnCheckResult;

// [nakata] DWC_Nalのステートを示す列挙型
typedef enum {
	DWC_NASLOGIN_STATE_DIRTY	= 0,	// 初期化されていない
	DWC_NASLOGIN_STATE_IDLE,			// 処理実行中ではない
	DWC_NASLOGIN_STATE_HTTP,			// HTTP通信中
	DWC_NASLOGIN_STATE_SUCCESS,		// ログイン成功
	DWC_NASLOGIN_STATE_ERROR,		// ログイン失敗
	DWC_NASLOGIN_STATE_CANCELED,	// ログインキャンセル
	DWC_NASLOGIN_STATE_MAX
} DWCNasLoginState;

/* =======================================================================
        external function(ingamesn関連)
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:			DWC_GetIngamesnCheckResult

  Description:	NASによるingamesn判定の結果を取得する

  Arguments:	なし

  Returns:		DWC_INGAMESN_NOTCHECKED	現在チェック中(ログイン処理中)
                DWC_INGAMESN_VALID		直前のログインのingamesnはNASに受け入れられた
				DWC_INGAMESN_INVALID	直前のログインのingamesnはNASに拒否された
 *---------------------------------------------------------------------------*/
DWCIngamesnCheckResult DWC_GetIngamesnCheckResult(void);

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
BOOL DWC_GetDateTime(RTCDate *date, RTCTime *time);

/* =======================================================================
        external function(SVL関連)
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:			DWC_SVLGetToken

  Description:	トークン取得関数

  Arguments:	svl		サービス種別を指定する4文字のASCII文字列
				result	取得した結果を格納する領域へのポインタ

  Returns:		TRUE	取得開始
				FALSE	取得開始できない
 *---------------------------------------------------------------------------*/
BOOL DWC_SVLGetTokenAsync(char *svl, DWCSvlResult *result);

/*---------------------------------------------------------------------------*
  Name:			DWC_SVLProcess

  Description:	SVLトークン取得処理進行関数

  Arguments:	なし

  Returns:		DWCSvlState
 *---------------------------------------------------------------------------*/
DWCSvlState DWC_SVLProcess(void);

/*---------------------------------------------------------------------------*
  Name:			DWC_SVLAbort

  Description:	SVLトークン取得処理進行関数

  Arguments:	なし

  Returns:		なし
 *---------------------------------------------------------------------------*/
void DWC_SVLAbort(void);

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
BOOL DWC_NASLoginAsync(void);

/*---------------------------------------------------------------------------*
  Name:			DWC_NASLoginProcess

  Description:	NASログイン処理進行

  Arguments:	なし

  Returns:		DWCNasLoginState
 *---------------------------------------------------------------------------*/
DWCNasLoginState DWC_NASLoginProcess(void);

/*---------------------------------------------------------------------------*
  Name:			DWC_NASLoginAbort

  Description:	任天堂認証サーバログイン処理中断関数

  Arguments:	なし

  Returns:		なし
 *---------------------------------------------------------------------------*/
void DWC_NASLoginAbort(void);

#ifdef __cplusplus
}
#endif

#endif // DWC_LOGIN_H_
