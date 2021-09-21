/*---------------------------------------------------------------------------*
  Project:  NitroDWC - base - dwc_core
  File:     dwc_core.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_core.h,v $
  Revision 1.16  2006/06/30 03:03:31  takayama
  includeの順番に依存性があったため、dwc_main.hを最後にした。

  Revision 1.15  2006/06/29 09:50:18  takayama
  dwc_commonをNITRODWC_NOGSオプション時から外した。

  Revision 1.14  2006/04/27 11:17:33  takayama
  NITRODWC_NOGSオプションをブラウザ向けから通常のものに修正。

  Revision 1.13  2006/04/21 01:33:43  takayama
  NITORDWC_NOGSでのビルド環境の調整。

  Revision 1.12  2006/04/20 13:21:12  takayama
  NITRODWC_NOGSオプションに対応。

  Revision 1.11  2006/03/07 01:18:16  nakata
  NASに関連する機能追加をdwc_nasfunc.c/dwc_nasfunc.hに集約する際に必要な変更を行った。

  Revision 1.10  2005/11/02 02:49:33  arakit
  ファイルのインクルード関係を調整した。

  Revision 1.9  2005/09/05 05:17:22  onozawa
  dwc_ghttp.hの追加

  Revision 1.8  2005/08/20 07:01:19  sasakit
  ヘッダインクルードガードの書式を統一した。
  bm/dwc_init.h -> bm/dwc_bm_init.hに変更
  ヘッダにCopyrightをつけた。
  構造体の名前空間をできるだけルールにちかづけた。
  util_wifiidtool.hとdwc_backup.hの関係を若干修正。


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_CORE_H_
#define DWC_CORE_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <base/dwc_connectinet.h>
#include <base/dwc_error.h>
#include <base/dwc_init.h>
#include <base/dwc_memfunc.h>
#include <base/dwc_nasfunc.h>
#include <base/dwc_report.h>

#ifndef NITRODWC_NOGS
#include <base/dwc_account.h>
#include <base/dwc_base_gamespy.h>
#include <base/dwc_common.h>
#include <base/dwc_friend.h>
#include <base/dwc_ghttp.h>
#include <base/dwc_login.h>
#include <base/dwc_match.h>
#include <base/dwc_transport.h>
#include <base/dwc_main.h>
#endif


#ifdef __cplusplus
}
#endif


#endif // DWC_CORE_H_
