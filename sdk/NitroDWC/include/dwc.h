/*---------------------------------------------------------------------------*
  Project:  NitroDWC - dwc - dwc
  File:     dwc.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc.h,v $
  Revision 1.8  2006/04/27 11:17:33  takayama
  NITRODWC_NOGSオプションをブラウザ向けから通常のものに修正。

  Revision 1.7  2006/04/26 08:34:32  nakata
  DWC_Ndをdwc.hさえインクルードすれば動作するよう変更

  Revision 1.6  2006/04/21 02:04:41  takayama
  NITRODWC_NOGSに対応。

  Revision 1.5  2006/04/10 10:16:57  hayashi
  added the ranking module

  Revision 1.4  2005/08/20 07:04:03  sasakit
  bm/dwc_init.h -> bm/dwc_bm_init.hに変更

  Revision 1.3  2005/08/20 07:01:18  sasakit
  ヘッダインクルードガードの書式を統一した。
  bm/dwc_init.h -> bm/dwc_bm_init.hに変更
  ヘッダにCopyrightをつけた。
  構造体の名前空間をできるだけルールにちかづけた。
  util_wifiidtool.hとdwc_backup.hの関係を若干修正。


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_H_
#define DWC_H_

//#include <auth/dwc_auth.h>
#include <util/dwc_utility.h>
#include <bm/dwc_bm_init.h>
#include <ac/dwc_ac.h>
#include <base/dwc_core.h>
#include <base/dwc_nd.h>
#ifndef NITRODWC_NOGS
#include <base/dwc_ranking.h>
#endif

#endif // DWC_H_
