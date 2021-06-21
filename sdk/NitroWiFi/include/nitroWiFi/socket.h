/*---------------------------------------------------------------------------*
  Project:  NitroWiFi - include - soc
  File:     socket.h

  Copyright 2005,2006 Nintendo. All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law. They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: socket.h,v $
  Revision 1.10  2006/03/10 09:20:22  kitase_hirotake
  INDENT SOURCE

  Revision 1.9  2005/08/05 05:06:28  seiki_masashi
  cps.h で参照されているため、#include <nitro/math.h> を追加
  NitroInet 互換のため、#include <string.h> を追加

  Revision 1.8  2005/07/18 13:16:45  yasu
  送信受信スレッドを作成する

  Revision 1.7  2005/07/15 13:32:51  yasu
  soc ライブラリのヘッダを調整

  Revision 1.6  2005/07/15 09:28:58  yasu
  ビルドを通すための一時的な対処

  Revision 1.5  2005/06/28 02:36:08  yasu
  使用していない変数をコメントアウト

  Revision 1.4  2005/06/23 12:57:06  yasu
  SDK_BIG_ENDIAN/SDK_LITTLE_ENDIAN の対応

  Revision 1.3  2005/06/22 13:16:54  yasu
  プレフィクスの変更 SO_ -> SOC_ へ

  Revision 1.2  2005/06/22 12:07:28  yasu
  ソケット API 定義ファイルを作成

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NITROWIFI_SOCKET_H_
#define NITROWIFI_SOCKET_H_

#include <string.h>
#include <nitro/types.h>
#include <nitro/os.h>
#include <nitro/math.h>
#include <nitroWiFi/cps.h>
#include <nitroWiFi/socl.h>
#include <nitroWiFi/soc.h>

#ifdef __cplusplus

extern "C" {
#endif

#ifdef __cplusplus

}
#endif

#endif // NITROWIFI_SOCKET_H_
