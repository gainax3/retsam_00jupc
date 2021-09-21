/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - libraries
  File:     wm_private.h

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wm_private.h,v $
  Revision 1.27  2005/11/01 08:50:20  seiki_masashi
  WM_DEBUG_AIDBITMAP の追加

  Revision 1.26  2005/02/28 05:26:35  yosizaki
  do-indent.

  Revision 1.25  2005/02/23 12:48:14  seiki_masashi
  デバッグ表示の整理

  Revision 1.24  2005/02/22 02:14:05  seiki_masashi
  small fix

  Revision 1.23  2005/02/22 01:57:28  seiki_masashi
  small fix

  Revision 1.22  2005/02/22 01:53:47  seiki_masashi
  WM_WARNING, WM_ASSERT を追加
  DEBUG_WM_USE_PRINTF を削除

  Revision 1.21  2005/02/18 12:27:45  seiki_masashi
  デバッグ用スイッチの判定文を #if から #ifdef に変更

  Revision 1.20  2005/02/03 23:41:47  ooe
  Copyright 表記の修正。

  Revision 1.19  2005/01/31 00:53:06  seiki_masashi
  デバッグフラグの定義の追加

  Revision 1.18  2004/08/18 00:23:40  seiki_masashi
  一部定数を wm.h に移動

  Revision 1.17  2004/08/17 12:49:11  seiki_masashi
  定数の追加

  Revision 1.16  2004/08/12 14:23:26  seiki_masashi
  WM_DEBUG_INDICATES の追加

  Revision 1.15  2004/08/11 04:24:24  seiki_masashi
  デバッグフラグの追加

  Revision 1.14  2004/08/07 05:16:44  seiki_masashi
  WM_DEBUG を OFF に

  Revision 1.13  2004/08/07 00:20:29  seiki_masashi
  WM_HEADER_SEQ_NUM_MASK の追加

  Revision 1.12  2004/08/06 05:11:39  seiki_masashi
  add WM_DEBUG switch

  Revision 1.11  2004/08/05 10:02:44  seiki_masashi
  wmHeader 中の KS ビットの位置を 0x0002 から 0x4000 に変更

  Revision 1.10  2004/08/04 01:33:34  seiki_masashi
  VSYNC flag の位置の変更
  port 導入の前準備

  Revision 1.9  2004/07/28 02:44:11  terui
  不要な定義を削除。
  WM内部ステートに関する定義をwm.hに移動

  Revision 1.8  2004/07/27 07:30:30  yasu
  Add #pragma  warn_padding off

  Revision 1.7  2004/07/11 03:12:23  ooe
  ビーコン関係のCMDSTATEを追加

  Revision 1.6  2004/07/10 11:49:24  terui
  Marioneaインストール場所変更に伴う修正

  Revision 1.5  2004/07/08 09:50:22  terui
  WM7の改造開始に伴う変更

  Revision 1.8  2004/07/06 08:11:47  Ooe
  WL-1.56.00への対応

  Revision 1.7  2004/07/06 01:54:47  Ooe
  WMヘッダのビットの定義を追加

  Revision 1.6  2004/07/03 07:30:21  Ooe
  CMDSTATEを追加

  Revision 1.5  2004/06/24 10:04:23  Ooe
  WM_SetGameInfo()を追加

  Revision 1.4  2004/06/21 06:40:20  Ooe
  (none)

  Revision 1.3  2004/06/19 05:56:20  Miya
  modified header file reference

  Revision 1.4  2004/06/19 04:05:16  miya
  ver. up Marionea-1.48.00

  Revision 1.3  2004/06/14 04:18:44  terui
  Undefine OS_Printf on FINALROM build mode.

  Revision 1.2  2004/06/02 02:29:51  miya
  ver. up

  Revision 1.1  2004/05/26 08:54:40  terui
  無線組み込みに伴う追加

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef LIBRARIES_WM_PRIVATE_H_
#define LIBRARIES_WM_PRIVATE_H_

#ifdef  __cplusplus
extern "C" {
#endif

/*===========================================================================*/


#include <nitro.h>

#ifdef SDK_ARM7
#include <nitro_wl/ARM7/WlLib.h>
#endif

/*---------------------------------------------------------------------------*
    定数定義
 *---------------------------------------------------------------------------*/

//#define WM_DEBUG                // WM のデバッグフラグ(主にARM7)
//#define WM_DEBUG_DATASHARING    // DataSharing 関係のデバッグメッセージフラグ(ARM9)
//#define WM_DEBUG_CALLBACK       // ARM9 の無線用 PXI 割り込みのデバッグ表示フラグ(ARM9)
//#define WM_DEBUG_AIDBITMAP      // ARM9 の AID BITMAP 関係のデバッグ表示フラグ(ARM9)

/*---------------------------------------------------------------------------*
    マクロ定義
 *---------------------------------------------------------------------------*/

#ifdef SDK_DEBUG
#define WM_WARNING(...) \
    ( (void) ( WMi_Warning(__FILE__, __LINE__, __VA_ARGS__) ) )
#else
#define WM_WARNING(...)    ((void) 0)
#endif

#ifdef WM_DEBUG
#define WM_ASSERT(exp) \
    ( (void) ( (exp) || (WMi_Warning(__FILE__, __LINE__, "Failed assertion " #exp), 0) ) )
#define WM_ASSERTMSG(exp, ...) \
    ( (void) ( (exp) || (WMi_Warning(__FILE__, __LINE__, __VA_ARGS__), 0) ) )
#define WM_DPRINTF WMi_Printf
#else  // WM_DEBUG
#define WM_ASSERT(exp)     ((void) 0)
#define WM_ASSERTMSG(...)     ((void) 0)
#define WM_DPRINTF(...)     ((void) 0)
#endif // WM_DEBUG

#ifndef SDK_FINALROM
#define WMi_Printf      OS_TPrintf
#define WMi_Warning     OSi_TWarning
#else
#define WMi_Printf(...)    ((void) 0)
#define WMi_Warning(...)   ((void) 0)
#endif


/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C" */
#endif

#endif /* LIBRARIES_WM_PRIVATE_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
