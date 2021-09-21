/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - include
  File:     wm.h

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wm.h,v $
  Revision 1.7  2005/02/28 05:26:02  yosizaki
  do-indent.

  Revision 1.6  2004/08/10 09:11:27  terui
  ARM9専用ヘッダwm_api.hをinclude

  Revision 1.5  2004/07/20 10:50:36  terui
  wm_libとwmの別ライブラリ化に伴う変更

  Revision 1.4  2004/07/20 07:51:40  terui
  wm_lib.hを暫定でインクルード

  Revision 1.3  2004/07/20 01:17:57  terui
  ARM9側上位ライブラリのインクルードを一旦削除

  Revision 1.2  2004/07/08 10:03:47  terui
  WM7改造開始に伴う変更

  Revision 1.1  2004/05/26 08:46:55  terui
  無線組み込みに伴う追加

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NITRO_WM_H_
#define NITRO_WM_H_

#ifdef  __cplusplus
extern "C" {
#endif

/*===========================================================================*/

#include    <nitro/wm/common/wm.h>

#ifdef  SDK_ARM7

#include    <nitro/wm/ARM7/wm_sp.h>

#else  // SDK_ARM9

#include    <nitro/wm/ARM9/wm_api.h>

#endif

/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C" */
#endif

#endif /* NITRO_WM_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
