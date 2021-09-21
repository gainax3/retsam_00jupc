/*---------------------------------------------------------------------------*
  Project:  NitroDWC - auth - dwc_svl
  File:     dwc_svl.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_svl.h,v $
  Revision 1.4  2006/05/29 11:21:50  nakata
  SVLのエラーコードを24000-27000から24000-25000に変更

  Revision 1.3  2006/05/25 05:17:16  nakata
  DWC_SVLにDWCエラーを返す機能を追加したことに伴う変更

  Revision 1.2  2006/04/27 12:46:24  takayama
  svc(SVC) -> svl(SVL)

  Revision 1.1  2006/04/27 12:31:19  takayama
  dwc_svc.h -> dwc_svl.h

  Revision 1.5  2006/03/23 00:27:01  nakata
  ステートDWC_SVL_STATE_DIRTYを追加

  Revision 1.4  2006/03/20 05:00:24  nakata
  *** empty log message ***

  Revision 1.1  2006/03/09 08:54:14  nakata
  svc関連のものがパッケージに入らないよう変更

  Revision 1.2  2006/03/07 01:16:22  nakata
  改行コード修正

  Revision 1.1  2006/03/02 05:21:58  nakata
  DWC_Svcモジュール追加
  DWC_Util_Alloc/DWC_Util_Free関数の名前をDWC_Auth_Alloc/DWC_Auth_Free関数に名前を変更



  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_SVL_H
#define DWC_SVL_H

#include "util_alloc.h"

#ifdef __cplusplus
extern "C" {
#endif // [nakata] __cplusplus

//=============================================================================
// [nakata] 定義
//=============================================================================

#define DWC_SVL_ERROR_BASENUM		(-24000)
#define DWC_SVL_ERROR_HTTPBASENUM	(-25000)

#define DWC_SVL_ERROR_FATAL			(-100)
#define DWC_SVL_ERROR_BADDATA		(-101)

#define DWC_SVL_HOST_LENGTH		64
#define DWC_SVL_TOKEN_LENGTH	300
#define DWC_SVL_STATUSDATA_LENGTH	1

// [nakata] SVC取得中のステートを示す列挙型
typedef enum {
	DWC_SVL_STATE_DIRTY	= 0,// 未初期化
	DWC_SVL_STATE_IDLE,		// 処理実行中ではない
	DWC_SVL_STATE_HTTP,		// HTTP通信中
	DWC_SVL_STATE_SUCCESS,	// トークン取得成功
	DWC_SVL_STATE_ERROR,	// トークン取得失敗
	DWC_SVL_STATE_CANCELED,	// トークン取得キャンセル
	DWC_SVL_STATE_MAX
} DWCSvlState;

// [nakata] SVC取得結果を格納する構造体
typedef struct {
	BOOL	status;
	char svlhost[DWC_SVL_HOST_LENGTH+1];
	char svltoken[DWC_SVL_TOKEN_LENGTH+1];
} DWCSvlResult;

//=============================================================================
// [nakata] 関数
//=============================================================================

//-----------------------------------------------------------------------------
// 機能 : DWC_Svcで使用する領域の確保と初期化
// 引数 : alloc             - 内部で使用するメモリ確保関数へのポインタ
//      : free              - 内部で使用するメモリ解放関数へのポインタ
// 返値 : TRUE  : 成功
//        FALSE : 失敗
//-----------------------------------------------------------------------------
BOOL		DWC_Svl_Init(DWCAuthAlloc alloc, DWCAuthFree free);

//-----------------------------------------------------------------------------
// 機能 : DWC_Svcで使用した領域の解放
// 引数 : なし
// 返値 : なし
//-----------------------------------------------------------------------------
void		DWC_Svl_Cleanup(void);

//-----------------------------------------------------------------------------
// 機能 : トークン取得関数
// 引数 : svc    - サービス種別を指定する4文字のASCII文字列
//      : result - 取得した結果を格納する領域へのポインタ
// 返値 : TRUE  : 取得開始
//        FALSE : 取得開始できない
//-----------------------------------------------------------------------------
BOOL		DWC_Svl_GetTokenAsync(char *svc, DWCSvlResult *result);

//-----------------------------------------------------------------------------
// 機能 : トークン取得処理進行関数
// 引数 : なし
// 返値 : DWCSvcState
//-----------------------------------------------------------------------------
DWCSvlState	DWC_Svl_Process(void);

//-----------------------------------------------------------------------------
// 機能 : トークン取得処理中断関数
// 引数 : なし
// 返値 : なし
//-----------------------------------------------------------------------------
void		DWC_Svl_Abort(void);

#ifdef __cplusplus
}
#endif

#endif
