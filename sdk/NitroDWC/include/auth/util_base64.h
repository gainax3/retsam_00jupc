/*---------------------------------------------------------------------------*
  Project:  NitroDWC - auth - util_base64
  File:     util_base64.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: util_base64.h,v $
  Revision 1.6  2006/03/03 04:49:40  nakata
  NAS時間取得機能を追加した。

  Revision 1.5  2006/03/02 05:21:58  nakata
  DWC_Svcモジュール追加
  DWC_Util_Alloc/DWC_Util_Free関数の名前をDWC_Auth_Alloc/DWC_Auth_Free関数に名前を変更

  Revision 1.4  2006/03/01 02:44:08  nakata
  CAL取得機能をdwc_auth.cから分離した

  Revision 1.3  2005/08/20 07:01:19  sasakit
  ヘッダインクルードガードの書式を統一した。
  bm/dwc_init.h -> bm/dwc_bm_init.hに変更
  ヘッダにCopyrightをつけた。
  構造体の名前空間をできるだけルールにちかづけた。
  util_wifiidtool.hとdwc_backup.hの関係を若干修正。


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_UTIL_BASE64_H_
#define DWC_UTIL_BASE64_H_
/*****************************************************************************/
/*                                                                           */
/*  BASE64エンコード/デコード                                                */
/*      util_base64.h                                                        */
/*          ver. 0.5                                                         */
/*                                                                           */
/*****************************************************************************/

#include <nitro.h>

#ifdef __cplusplus
extern "C" {
#endif // [nakata] __cplusplus

//=============================================================================
// [nakata] 関数
//=============================================================================

//-----------------------------------------------------------------------------
// 機能 : 与えられたデータをカスタマイズされたBase64でエンコード
// 引数 : src		- 生データへのポインタ 
//      : srclen	- 生データの長さ
//      : dst		- 変換先バッファへのポインタ
//      : dstlen	- 変換先バッファの長さ
// 返値 : int  - -1       : 失敗
//               それ以外 : 変換結果の長さ
//-----------------------------------------------------------------------------
int DWC_Auth_Base64Encode(const char *src, const unsigned long srclen, char *dst, const unsigned long dstlen);

//-----------------------------------------------------------------------------
// 機能 : 与えられたデータをカスタマイズされたBase64でデコード
// 引数 : src		- Base64データへのポインタ 
//      : srclen	- Base64データの長さ
//      : dst		- 変換先バッファへのポインタ
//      : dstlen	- 変換先バッファの長さ
// 返値 : int  - -1       : 失敗
//               それ以外 : 変換結果の長さ
//-----------------------------------------------------------------------------
int DWC_Auth_Base64Decode(const char *src, const unsigned long srclen, char *dst, const unsigned long dstlen);

#ifdef __cplusplus
}
#endif // [nakata] __cplusplus


#endif // DWC_UTIL_BASE64_H_
