/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS
  File:     os.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os.h,v $
  Revision 1.38  2006/07/06 12:33:44  okubata_ryoma
  Copyright fix

  Revision 1.37  2006/07/05 09:15:31  yosizaki
  add systemWork header.

  Revision 1.36  2005/07/27 07:37:55  seiki_masashi
  entropy.h の追加

  Revision 1.35  2005/07/21 07:06:09  yada
  add argument.h

  Revision 1.34  2005/04/25 08:25:01  terui
  ARM9にてchina.hをインクルードするヘッダに追加

  Revision 1.33  2005/02/28 05:26:02  yosizaki
  do-indent.

  Revision 1.32  2005/02/09 09:15:36  terui
  ARM9にてvramExclusive.hをインクルードするヘッダに追加。

  Revision 1.31  2004/09/02 07:12:37  yada
  add ownerInfo.h

  Revision 1.30  2004/09/01 04:20:27  yada
  add reset.h

  Revision 1.29  2004/05/10 12:01:24  yada
  add profile.h functionCost.h

  Revision 1.28  2004/04/12 12:11:28  yada
  add callTrace.h

  Revision 1.27  2004/02/27 01:55:00  yasu
  add OS_SpinWait, move OS_Halt into os_system.c

  Revision 1.26  2004/02/25 11:29:25  yada
  systemClock→Tick に伴う変更

  Revision 1.25  2004/02/23 12:30:33  yada
  valarm.h を A9/7両方に

  Revision 1.24  2004/02/23 11:55:38  yada
  valarm.h 追加

  Revision 1.23  2004/02/12 11:41:25  yada
  alloc.h 追加

  Revision 1.22  2004/02/12 10:56:04  yasu
  new location of include files ARM9/ARM7

  Revision 1.21  2004/02/10 01:59:29  yada
  system.h を追加

  Revision 1.20  2004/02/10 01:45:39  yada
  utility.h を削除

  Revision 1.19  2004/02/10 01:28:38  yada
  utility.h を削除

  Revision 1.18  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.17  2004/02/05 01:59:45  yada
  utility.h 追加

  Revision 1.16  2004/02/03 11:18:44  yada
  alarm.h を追加

  Revision 1.15  2004/02/02 05:29:35  yada
  systemClock.h 追加

  Revision 1.14  2003/12/26 06:44:26  yada
  arena.h 追加

  Revision 1.13  2003/12/22 01:34:39  yada
  os/init.h を追加した

  Revision 1.12  2003/12/18 07:18:04  yada
  os/writeBuffer.h のインクルードを外した

  Revision 1.11  2003/12/12 01:27:14  yada
  includeの名称変更(os/system.h と hw/armArch.h) による修正

  Revision 1.10  2003/12/08 12:21:31  yada
  include するファイルを追加

  Revision 1.9  2003/11/30 04:20:42  yasu
  cpsr.h の追加

  Revision 1.8  2003/11/29 10:11:29  yada
  mutex.h を追加

  Revision 1.7  2003/11/26 01:50:58  yada
  message.h を追加

  Revision 1.6  2003/11/25 11:18:36  yasu
  emulator.h を追加

  Revision 1.5  2003/11/25 05:53:10  yada
  os回り変更

  Revision 1.4  2003/11/25 05:00:39  yasu
  thread.h/context.h の追加

  Revision 1.3  2003/11/17 00:23:22  yasu
  OS_Printf などを iris/os/printf.h へ分離

  Revision 1.2  2003/11/14 06:14:12  yasu
  OS_Warning() OS_Panic() の追加

  Revision 1.1  2003/11/13 11:59:25  yasu
  OS_Printf 関係関数作成

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_OS_H_
#define NITRO_OS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/os/common/system.h>
#include <nitro/os/common/interrupt.h>
#include <nitro/os/common/thread.h>
#include <nitro/os/common/context.h>
#include <nitro/os/common/spinLock.h>
#include <nitro/os/common/timer.h>
#include <nitro/os/common/systemCall.h>
#include <nitro/os/common/printf.h>
#include <nitro/os/common/emulator.h>
#include <nitro/os/common/message.h>
#include <nitro/os/common/mutex.h>
#include <nitro/os/common/exception.h>
#include <nitro/os/common/init.h>
#include <nitro/os/common/arena.h>
#include <nitro/os/common/alloc.h>
#include <nitro/os/common/tick.h>
#include <nitro/os/common/alarm.h>
#include <nitro/os/common/valarm.h>
#include <nitro/os/common/system.h>
#include <nitro/os/common/reset.h>
#include <nitro/os/common/ownerInfo.h>
#include <nitro/os/common/entropy.h>
#include <nitro/os/common/systemWork.h>

//---- profiler
#include <nitro/os/common/profile.h>
#include <nitro/os/common/callTrace.h>
#include <nitro/os/common/functionCost.h>

#ifdef	SDK_ARM9
#include <nitro/os/ARM9/protectionRegion.h>
#include <nitro/os/ARM9/tcm.h>
#include <nitro/os/ARM9/cache.h>
#include <nitro/os/ARM9/protectionUnit.h>
#include <nitro/os/ARM9/vramExclusive.h>
#include <nitro/os/ARM9/china.h>
#include <nitro/os/ARM9/argument.h>
#else  //SDK_ARM7
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_OS_H_ */
#endif
