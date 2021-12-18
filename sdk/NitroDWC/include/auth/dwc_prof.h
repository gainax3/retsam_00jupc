/*---------------------------------------------------------------------------*
  Project:  NitroDWC - auth - dwc_prof
  File:     dwc_prof.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_prof.h,v $
  Revision 1.11  2007/06/29 09:13:06  takayama
  リージョン指定版の不正文字列チェック開始関数を追加。

  Revision 1.10  2007/04/20 04:20:53  takayama
  コメントの誤記修正。

  Revision 1.9  2007/04/09 02:45:59  takayama
  DWC_CheckProfanityAsync関数の引数 badnum を badwordsnum に変更したことに
  伴う変更。

  Revision 1.8  2007/04/04 07:38:05  takayama
  使用していない定義を削除。

  Revision 1.7  2007/03/28 09:41:26  nakata
  DWC_CheckProfanityAsync関数の仕様変更に伴う修正。

  Revision 1.6  2007/03/19 11:24:36  nakata
  DWCProfResultをDWCProfStateに変更した

  Revision 1.5  2007/03/16 05:35:47  nakata
  DWC_PROF_RESULT定数周りの変更

  Revision 1.4  2007/03/14 08:44:07  nakata
  DWC_PROF_RESULT_STRING_LENGTH定数を追加

  Revision 1.3  2006/11/22 12:55:27  nakata
  DWCProfResultにネットワークエラーを追加

  Revision 1.2  2006/11/17 08:29:47  nakata
  コメント修正

  Revision 1.1  2006/11/17 08:29:06  nakata
  不正文字列チェック関係ヘッダを追加


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_PROF_H
#define DWC_PROF_H

#include "dwc_http.h"

#ifdef __cplusplus
extern "C" {
#endif // [nakata] __cplusplus

//=============================================================================
// [nakata] 定義
//=============================================================================

#define DWC_PROF_RESULT_STRING_LENGTH 51

// [nakata] 不正ネームチェック中のステートを示す列挙型
typedef enum {
	DWC_PROF_STATE_NOT_INITIALIZED	= 0,// 未初期化
	DWC_PROF_STATE_OPERATING,				// HTTP通信中
	DWC_PROF_STATE_SUCCESS,				// 名前チェック成功
	DWC_PROF_STATE_FAIL					// 名前チェック失敗
} DWCProfState;

// 不正文字列チェックのリージョンを示す列挙型
typedef enum {
    DWC_PROF_REGION_SELF = 0x00,   // 製品コードに従う
    DWC_PROF_REGION_ALL  = 0xff    // 全リージョン
} DWCProfRegion;

//=============================================================================
// [nakata] 関数
//=============================================================================

//-----------------------------------------------------------------------------
// 機能 : 不正文字列チェック開始関数
// 引数 : words - チェックする文字列(UTF16-LE)のポインタ配列
//      : wordsnum - wordsの長さ
//      : reserved - ゲーム内バッドネーム辞書切り替え用の4文字のASCII文字列(通常はNULL)
//      : timeout - タイムアウト時間を秒単位で指定する
//      : result - チェック結果
//      : badwordsnum - 不正文字列の数
// 返値 : TRUE  : 取得開始
//        FALSE : 取得開始できない
//-----------------------------------------------------------------------------
BOOL DWC_CheckProfanityAsync(const u16 **words, int wordsnum, const char *reserved, int timeout, char *result, int *badwordsnum);

//-----------------------------------------------------------------------------
// 機能 : 不正文字列チェック開始関数(リージョン指定版)
// 引数 : words - チェックする文字列(UTF16-LE)のポインタ配列
//      : wordsnum - wordsの長さ
//      : reserved - ゲーム内バッドネーム辞書切り替え用の4文字のASCII文字列(通常はNULL)
//      : timeout - タイムアウト時間を秒単位で指定する
//      : result - チェック結果
//      : badwordsnum - 不正文字列の数
//      : region - 対象のリージョン
// 返値 : TRUE  : 取得開始
//        FALSE : 取得開始できない
//-----------------------------------------------------------------------------
BOOL DWC_CheckProfanityExAsync(const u16 **words, int wordsnum, const char *reserved, int timeout, char *result, int *badwordsnum, DWCProfRegion region);

//-----------------------------------------------------------------------------
// 機能 : 不正文字列チェック取得処理進行関数
// 引数 : なし
// 返値 : DWCProfState
//-----------------------------------------------------------------------------
DWCProfState DWC_CheckProfanityProcess(void);

#ifdef __cplusplus
}
#endif

#endif
