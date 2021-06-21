/*---------------------------------------------------------------------------*
  Project:  NitroSDK - include - MI
  File:     mi.h

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mi.h,v $
  Revision 1.13  2007/05/14 00:26:20  yosizaki
  add cache.h and device.h

  Revision 1.12  2006/08/15 07:00:48  yosizaki
  fix comments.

  Revision 1.11  2006/04/05 07:44:43  yosizaki
  add mi/endian.h

  Revision 1.10  2005/02/28 05:26:02  yosizaki
  do-indent.

  Revision 1.9  2005/02/10 07:47:21  yada
  add mi/init.h

  Revision 1.8  2005/01/28 13:13:39  takano_makoto
  mi/compress.hを追加

  Revision 1.7  2004/11/30 10:53:53  takano_makoto
  uncomp_stream.hを追加

  Revision 1.6  2004/07/20 07:58:42  yada
  add stream.h

  Revision 1.5  2004/04/26 11:04:25  yosizaki
  add mi/card.h

  Revision 1.4  2004/03/30 11:00:30  yada
  バイトアクセス関数をARM7専用から共通に

  Revision 1.3  2004/03/23 00:54:05  yada
  ARM7 の場合に byteAccess.hを追加

  Revision 1.2  2004/02/12 13:37:44  yasu
  change include "" -> include <>

  Revision 1.1  2004/02/10 01:21:10  yada
  mi/ のヘッダファイルが増えてきたので作成


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_MI_H_
#define NITRO_MI_H_

#include <nitro/mi/dma.h>
#include <nitro/mi/exMemory.h>
#include <nitro/mi/wram.h>
#include <nitro/mi/memory.h>
#include <nitro/mi/swap.h>
#include <nitro/mi/uncompress.h>
#include <nitro/mi/byteAccess.h>
#include <nitro/mi/card.h>
#include <nitro/mi/stream.h>
#include <nitro/mi/uncomp_stream.h>
#include <nitro/mi/compress.h>
#include <nitro/mi/init.h>
#include <nitro/mi/endian.h>
#include <nitro/mi/cache.h>
#include <nitro/mi/device.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_MI_H_ */
#endif
