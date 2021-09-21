/*---------------------------------------------------------------------------*
  Project:  NitroWiFi - Top - include
  File:     nitroWiFi.h

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: nitroWiFi.h,v $
  Revision 1.5  2006/03/10 09:20:22  kitase_hirotake
  INDENT SOURCE

  Revision 1.4  2005/07/15 08:35:20  yasu
  ssl 関係ヘッダファイル追加

  Revision 1.3  2005/07/07 10:43:50  terui
  WCM ライブラリのサポートを追加
  IW_ -> WCM_ 置き換えのサポートを追加

  Revision 1.2  2005/06/22 13:18:06  yasu
  SO_ -> SOC_ の置き換えのサポート

  Revision 1.1  2005/06/21 13:04:43  yasu
  ヘッダファイルの登録テンプレート作成
  
  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NITROWIFI_H_
#define NITROWIFI_H_

#ifdef SDK_ARM9

#include <nitroWiFi/socket.h>
#include <nitroWiFi/wcm.h>

#include <nitroWiFi/ssl.h>

#ifndef SDKWIFI_NO_SO_SYMBOLS
#include <nitroWiFi/so2soc.h>
#include <nitroWiFi/iw2wcm.h>
#endif

#else //SDK_ARM7
#endif

#endif //NITROWIFI_H_
