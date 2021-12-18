/*---------------------------------------------------------------------------*
  Project:  NitroDWC - auth - util_alloc
  File:     util_alloc.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: util_alloc.h,v $
  Revision 1.15  2006/04/13 11:35:19  hayashi
  Change prefix Ranking->Rnk

  Revision 1.14  2006/03/02 05:21:58  nakata
  DWC_Svcモジュール追加
  DWC_Util_Alloc/DWC_Util_Free関数の名前をDWC_Auth_Alloc/DWC_Auth_Free関数に名前を変更

  Revision 1.13  2005/12/22 01:39:06  nakata
  Ressurected util_alloc.h.

  Revision 1.8  2005/11/15 10:55:40  nakata
  Ressurect util_alloc.h for successful compilation.

  Revision 1.6  2005/09/01 00:19:46  nakata
  Removed util_alloc.c related codes from DWC_Http/DWC_Auth/DWC_Netcheck.

  Revision 1.5  2005/08/20 07:01:19  sasakit
  ヘッダインクルードガードの書式を統一した。
  bm/dwc_init.h -> bm/dwc_bm_init.hに変更
  ヘッダにCopyrightをつけた。
  構造体の名前空間をできるだけルールにちかづけた。
  util_wifiidtool.hとdwc_backup.hの関係を若干修正。


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_UTIL_ALLOC_H_
#define DWC_UTIL_ALLOC_H_

#include <nitro.h>

#ifdef __cplusplus
extern "C" {
#endif // [nakata] __cplusplus

//=============================================================================
// [nakata] 構造体
//=============================================================================

// [nakata] メモリ確保/解放関数のためのインタフェース型
typedef void *(*DWCAuthAlloc)(unsigned long name, long size);
typedef void (*DWCAuthFree)(unsigned long name, void *ptr, long size);

#ifdef __cplusplus
}
#endif // [nakata] __cplusplus

#endif // DWC_UTIL_ALLOC_H_
