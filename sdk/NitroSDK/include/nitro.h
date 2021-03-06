/*---------------------------------------------------------------------------*
  Project:  NitroSDK
  File:     nitro.h

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: nitro.h,v $
  Revision 1.90  2007/06/14 08:25:11  okubata_ryoma
  Copyright fix

  Revision 1.89  2007/06/11 10:57:41  yosizaki
  add wfs.h

  Revision 1.88  2006/06/16 05:29:21  yosizaki
  add wbt.h

  Revision 1.87  2006/03/02 10:20:34  okubata_ryoma
  vib.hの追加

  Revision 1.86  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.85  2005/09/05 01:18:58  yada
  initial release

  Revision 1.84  2005/08/19 11:01:52  yada
  add std.h

  Revision 1.83  2005/07/05 10:55:55  yasu
  utest.h のインクルード位置を元に戻す

  Revision 1.82  2005/07/05 09:36:29  yasu
  utest.h の misc.h への移動

  Revision 1.81  2005/06/24 02:20:13  yasu
  utest 追加

  Revision 1.80  2005/06/23 12:16:01  yasu
  SDK_LITTLE_ENDIAN 定義の追加

  Revision 1.79  2005/02/24 08:29:20  yasu
  フォーマット修正

  Revision 1.78  2005/02/21 00:33:02  yasu
  厳格な型チェックに対応

  Revision 1.77  2005/02/18 08:42:35  yasu
  存在しないマクロ変数に対する警告への対処

  Revision 1.76  2005/02/16 23:51:41  terui
  Update copyright.

  Revision 1.75  2005/02/16 10:33:41  terui
  wvr.h を追加。

  Revision 1.74  2004/12/14 13:13:12  takano_makoto
  fx_rand.hをmath/rand.hへ移動

  Revision 1.73  2004/12/14 10:51:38  seiki_masashi
  MATH ジャンルを追加

  Revision 1.72  2004/12/13 07:45:25  takano_makoto
  fx/fx_rand.hを追加

  Revision 1.71  2004/11/10 13:14:18  takano_makoto
  mb.hを追加

  Revision 1.70  2004/10/20 06:18:04  terui
  Add include <nitro/hw/common/lcd.h>

  Revision 1.69  2004/09/13 10:53:32  yada
  add ctrdg.h

  Revision 1.68  2004/09/06 13:05:10  terui
  Add #include <nitro/wm.h>

  Revision 1.67  2004/08/27 02:05:39  yosizaki
  delete wbt.h

  Revision 1.66  2004/08/23 09:43:28  yosizaki
  add WBT.

  Revision 1.65  2004/08/09 11:29:54  yasu
  Add spec.h to switch platform settings

  Revision 1.64  2004/05/14 11:56:21  takano_makoto
  Move gx include description from nitro.h to nitro/gx.h.

  Revision 1.63  2004/05/12 05:45:08  terui
  Add /nitro/rtc.h

  Revision 1.62  2004/05/11 09:35:19  yosizaki
  add /nitro/fs.h

  Revision 1.61  2004/05/06 02:57:28  takano_makoto
  Add cp_context.h

  Revision 1.60  2004/04/21 12:11:09  takano_makoto
  move spi.h to common.

  Revision 1.59  2004/04/15 03:26:15  yasu
  add SDK_ASM

  Revision 1.58  2004/03/10 05:38:14  yada
  ARM7に spi.h を追加

  Revision 1.57  2004/02/18 07:05:36  nishida_kenji
  Add gx_sp.h.

  Revision 1.56  2004/02/18 01:23:52  yada
  pad 移動, exi 新設

  Revision 1.55  2004/02/17 09:22:39  yada
  pad.h を ARM7/9 両方がインクルードするようにした

  Revision 1.54  2004/02/14 06:43:55  yasu
  add pxi.h

  Revision 1.53  2004/02/13 09:14:27  nishida_kenji
  Add nitro/gx/struct_2d.h .

  Revision 1.52  2004/02/13 00:17:18  ida
  snd ARM7 -> common

  Revision 1.51  2004/02/12 13:12:13  yasu
  merge nitro.h and nitro_sp.h

  Revision 1.50  2004/02/12 10:56:04  yasu
  new location of include files ARM9/ARM7

  Revision 1.49  2004/02/10 01:21:33  yada
  mi/*.h が増えてきたので mi.h に纏める

  Revision 1.48  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.47  2004/02/05 00:59:34  kitani_toshikazu
  added ext header.

  Revision 1.46  2003/12/25 12:13:58  nishida_kenji
  g2oam.h to g2_oam.h

  Revision 1.45  2003/12/25 12:02:58  nishida_kenji
  g3util.h -> g3_util.h
  add gx_load.h
  remove g2util.h

  Revision 1.44  2003/12/24 06:35:50  nishida_kenji
  integrate g3x.h and g3x_status.h

  Revision 1.43  2003/12/12 01:27:14  yada
  includeの名称変更(os/system.h と hw/armArch.h) による修正

  Revision 1.42  2003/12/10 00:16:12  nishida_kenji
  fx_sincos.h -> fx_trig.h

  Revision 1.41  2003/12/09 08:35:44  yasu
  メモリマップ周りを刷新　memorymap.h の追加

  Revision 1.40  2003/12/08 12:17:50  yada
  インクルードするファイルをMemoryMap.hから mmap_global.hに変更

  Revision 1.39  2003/11/29 08:52:43  nishida_kenji
  g2util.h

  Revision 1.38  2003/11/28 01:55:51  yada
  REDSDKの03-11-27反映

  Revision 1.37  2003/11/25 07:04:43  nishida_kenji
  gx_capture.hの追加

  Revision 1.36  2003/11/25 05:52:58  yada
  os.h 回り変更

  Revision 1.35  2003/11/25 02:22:58  nishida_kenji
  gx_bgcnt.hを追加

  Revision 1.34  2003/11/25 00:58:50  nishida_kenji
  gx_vramcnt.hを追加

  Revision 1.33  2003/11/21 09:54:10  nishida_kenji
  g3util.hの追加

  Revision 1.32  2003/11/21 08:14:50  nishida_kenji
  g3x_status.hの追加

  Revision 1.31  2003/11/18 11:17:59  nishida_kenji
  fx_cp.h, fx_vec.h fx_mtx.h

  Revision 1.30  2003/11/17 00:35:17  nishida_kenji
  fx追加

  Revision 1.29  2003/11/14 12:05:05  nishida_kenji
  fx_const.h追加

  Revision 1.28  2003/11/14 10:54:14  nishida_kenji
  fx.hの追加

  Revision 1.27  2003/11/14 06:05:41  nishida_kenji
  G3Rの廃止

  Revision 1.26  2003/11/13 11:25:47  nishida_kenji
  gxsincos.h追加

  Revision 1.25  2003/11/12 12:25:25  yasu
  base.h -> crt0.h への変更

  Revision 1.24  2003/11/12 08:56:13  yasu
  typo 修正

  Revision 1.23  2003/11/11 12:48:00  yada
  systemCall.h をインクルードするようにした

  Revision 1.22  2003/11/11 06:16:38  yada
  misc.h を追加

  Revision 1.21  2003/11/11 06:08:06  nishida_kenji
  fixed.hとvec.hの除去

  Revision 1.20  2003/11/10 09:02:02  yada
  base/base.h をインクルードするようにした

  Revision 1.19  2003/11/10 05:53:23  nishida_kenji
  g2oam.h追加

  Revision 1.18  2003/11/05 07:20:41  yasu
  hw/ -> iris/hw に変更

  Revision 1.17  2003/11/05 01:44:14  yada
  spinlock→spinLock

  Revision 1.16  2003/11/05 01:29:00  Nishida_Kenji
  スペルミス修正

  Revision 1.15  2003/11/05 01:07:20  yada
  spinlock→spinLock

  Revision 1.14  2003/11/04 10:39:10  Nishida_Kenji
  ioreg追加

  Revision 1.13  2003/11/04 09:26:26  yada
  (none)

  Revision 1.12  2003/11/04 09:25:15  yada
  hw のインクルードを追加

  Revision 1.11  2003/11/04 09:23:22  yada
  cp,mi,os,pad のインクルード情報を追加。

  Revision 1.10  2003/10/31 10:57:36  Nishida_Kenji
  iris/gx/gx.h

  Revision 1.9  2003/10/31 04:48:47  Nishida_Kenji
  iris/gx/g2.h

  Revision 1.8  2003/10/30 12:36:50  Nishida_Kenji
  g3r.h

  Revision 1.7  2003/10/30 10:45:12  Nishida_Kenji
  gxの移動

  Revision 1.6  2003/10/30 09:53:02  Nishida_Kenji
  g3x.h

  Revision 1.5  2003/10/30 05:07:47  Nishida_Kenji
  g3imm.hの追加

  Revision 1.4  2003/10/30 02:12:04  Nishida_Kenji
  iris/g3b.h iris/g3c.h

  Revision 1.3  2003/10/30 01:24:47  Nishida_Kenji
  インクルードするファイルを追加

  Revision 1.2  2003/10/28 11:36:51  Nishida_Kenji
  changed the header of this file

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NITRO_H_
#define NITRO_H_

#ifdef  SDK_ASM
#include <nitro/spec.h>
#include <nitro/types.h>
#include <nitro/memorymap.h>
#include <nitro/hw/common/armArch.h>
#else  //SDK_ASM

//
// Workaround for no macro warnings on CW:
//  - __embedded_cplusplus in MSL_C\MSL_ARM\Include\ansi_prefix.ARM.h
//  - __VEC__              in MSL_C\MSL_Common\Include\stdlib.h
//  - _MSL_USE_INLINE      in MSL_C\MSL_ARM\Include\math.ARM.h, 
//
#ifdef        __MWERKS__
#  ifdef      __cplusplus
#    ifndef   __embedded_cplusplus
#      define __embedded_cplusplus 0
#    endif
#    ifndef   __VEC__
#      define __VEC__			0
#    endif
#  endif
#  if         !defined(_MSL_USE_INLINE) && __option(dont_inline)
#  define     _MSL_USE_INLINE		0
#  endif
#endif

#include <nitro/spec.h>
#include <nitro/misc.h>
#include <nitro/init/crt0.h>

#include <nitro/types.h>
#include <nitro/memorymap.h>
#include <nitro/hw/common/armArch.h>
#include <nitro/hw/common/lcd.h>

#include <nitro/os.h>
#include <nitro/mi.h>
#include <nitro/pxi.h>
#include <nitro/snd.h>
#include <nitro/pad.h>
#include <nitro/spi.h>
#include <nitro/card.h>
#include <nitro/fs.h>
#include <nitro/rtc.h>
#include <nitro/gx.h>
#include <nitro/wm.h>
#include <nitro/wvr.h>
#include <nitro/ctrdg.h>
#include <nitro/math.h>
#include <nitro/utest.h>
#include <nitro/std.h>

#ifdef  SDK_ARM9

#include <nitro/fx/fx.h>
#include <nitro/fx/fx_const.h>
#include <nitro/fx/fx_trig.h>
#include <nitro/fx/fx_cp.h>
#include <nitro/fx/fx_vec.h>
#include <nitro/fx/fx_mtx.h>
#include <nitro/cp.h>
#include <nitro/ext.h>
#include <nitro/mb.h>
#include <nitro/wbt.h>
#include <nitro/wfs.h>
#include <nitro/env.h>
#include <nitro/vib.h>

#else  //SDK_ARM7

#include <nitro/exi.h>

#endif //SDK_ARM7
#endif //SDK_ASM

/* NITRO_H_ */
#endif

/*-----------------------------------------*
   NITRO-SDK Development Staff
       1st period (Sep03-Feb04)
         - Yoshitaka Yasumoto
         - Hiroto Yada
         - Kenji Nishida
         - Toshikazu Kitani(Jan04-Feb04)
       2nd period (Feb04- )
         - Yoshitaka Yasumoto
         - Hiroto Yada
         - Akio Terui
         - Koji Yoshizaki
         - Makoto Takano
         - Masashi Seiki(Jun04-)
 *-----------------------------------------*/
