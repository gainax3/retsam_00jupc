/*---------------------------------------------------------------------------*
  Project:  NitroSDK - SPI - include
  File:     config.h

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: config.h,v $
  Revision 1.18  2005/04/12 10:49:15  terui
  中国語対応の為に改造

  Revision 1.17  2005/02/28 05:26:03  yosizaki
  do-indent.

  Revision 1.16  2004/10/01 02:54:29  terui
  NVRAM内の無線使用可能チャンネルへのオフセット定義を追加。

  Revision 1.15  2004/09/13 05:49:56  yasu
  merged with NitroSDK_2_00pr6_branch

  Revision 1.14.2.1  2004/09/13 05:43:02  yasu
  Support SDK_NVRAM_FORMAT

  Revision 1.14  2004/09/03 11:48:54  yasu
  fix typo

  Revision 1.13  2004/09/03 04:29:15  terui
  NVRAM内ユーザー情報に関する構造体定義整理に伴う修正。

  Revision 1.12  2004/09/02 02:24:36  terui
  無線MACアドレス格納位置を定義に追加。

  Revision 1.11  2004/08/31 09:46:16  yosiokat
  SDK_SMALL_BUILDをSDK_NCD_LOCAL_USEに変更。

  Revision 1.10  2004/08/27 12:39:04  yosiokat
  IPL_BUILD_をSDK_SMALL_BUILDに変更。

  Revision 1.9  2004/08/25 09:21:50  terui
  userInfo定義を別ファイルに切り出し、TS_VERSIONにて切り替えるよう変更。

  Revision 1.8  2004/07/27 11:04:17  yasu
  Add #pragma  warn_padding off for SDK_TEG

  Revision 1.7  2004/07/27 07:00:10  yasu
  Add padding explicitly

  Revision 1.6  2004/06/30 10:36:18  terui
  TSにおけるユーザー情報フォーマットの定義を変更

  Revision 1.5  2004/06/30 09:12:11  terui
  TSの場合、NVRAM内のユーザー情報を探すためにオフセットを用いるよう定義を変更。

  Revision 1.4  2004/05/21 07:57:30  terui
  ユーザー情報へのアドレスを固定値にできるように変更。
  タッチパネルcalibrationデータフィールドを12バイトに拡張。

  Revision 1.3  2004/05/21 05:52:30  yosiokat
  IPl2のビルド時にこのファイルがインクルードされるのを防ぐよう修正。

  Revision 1.2  2004/05/20 10:31:33  takano_makoto
  c++からのコンパイルが通らないバグを修正

  Revision 1.1  2004/05/20 09:41:06  terui
  ユーザー情報へのオフセットのアドレス定義などを追加。

  Revision 1.2  2004/05/19 08:25:28  yosiokat
  更新ログの追加。

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NITRO_SPI_COMMON_CONFIG_H_
#define NITRO_SPI_COMMON_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================*/

#include <nitro/types.h>
#include <nitro/memorymap.h>
#include <nitro/spec.h>

// userInfo定義
#ifdef  SDK_TS
#if ( SDK_TS_VERSION >= 200 || SDK_NVRAM_FORMAT >= 100 )
#if ( SDK_NVRAM_FORMAT >= 300 )
#include <nitro/spi/common/userInfo_ts_300.h>
#else
#include <nitro/spi/common/userInfo_ts_200.h>
#endif
#else
#include <nitro/spi/common/userInfo_ts_0.h>
#endif
#else
#include <nitro/spi/common/userInfo_teg.h>
#endif

// NVRAM内のuserInfo格納位置
#ifdef  SDK_TS
#undef  NVRAM_CONFIG_CONST_ADDRESS     // ユーザー情報の位置はヘッダのオフセットから取得する
#else
#define NVRAM_CONFIG_CONST_ADDRESS     // ユーザー情報の位置を固定アドレスに
#endif

#ifdef  NVRAM_CONFIG_CONST_ADDRESS
#define NVRAM_CONFIG_DATA_ADDRESS_DUMMY     0x3fe00
#else
#define NVRAM_CONFIG_DATA_OFFSET_ADDRESS    0x0020
#define NVRAM_CONFIG_DATA_OFFSET_SIZE       2
#define NVRAM_CONFIG_DATA_OFFSET_SHIFT      3
#endif

#define NVRAM_CONFIG_SAVE_COUNT_MAX         0x0080      // NitroConfigData.saveCountの最大値
#define NVRAM_CONFIG_SAVE_COUNT_MASK        0x007f      // NitroConfigData.saveCountの値の範囲をマスクする。(0x00-0x7f）

// NVRAM内のMACアドレス格納位置
#define NVRAM_CONFIG_MACADDRESS_ADDRESS     0x00036
#define NVRAM_CONFIG_ENABLECHANNEL_ADDRESS  0x0003c

/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C" */
#endif

#endif /* NITRO_SPI_COMMON_CONFIG_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
