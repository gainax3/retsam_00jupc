/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI
  File:     mi_wram.c

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mi_wram.c,v $
  Revision 1.21  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.20  2005/02/28 05:26:25  yosizaki
  do-indent.

  Revision 1.19  2004/04/13 06:20:21  yada
  only fix header comment

  Revision 1.18  2004/03/30 04:50:46  yada
  ASSERTを.h から.c に移動

  Revision 1.17  2004/03/25 08:21:56  yada
  MI_GetWramBank()修正

  Revision 1.16  2004/03/25 07:51:40  yada
  MI_GetWramBank() 返り値のキャスト追加

  Revision 1.15  2004/03/25 05:22:52  yada
  ARM9とARM7でVRAMCNTの名前が異なることに対処

  Revision 1.14  2004/03/25 02:53:32  yada
  MI_GetWramBank() を ARM9 側にも公開した。

  Revision 1.13  2004/03/23 00:58:15  yada
  only fix comment

  Revision 1.12  2004/02/18 01:17:12  yasu
  force to turn byte access warning off

  Revision 1.11  2004/02/13 07:03:57  yada
  MI_GetWramBank追加

  Revision 1.10  2004/02/13 06:46:28  yada
  common に復帰

  Revision 1.9  2004/02/13 06:26:44  yada
  common から ARM9に移動

  Revision 1.8  2004/02/13 04:37:13  yada
  ARM9とARM7 の場合わけ考慮

  Revision 1.7  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.6  2004/01/18 02:27:08  yada
  (none)

  Revision 1.5  2003/12/26 05:52:44  yada
  型ルール統一による変更

  Revision 1.4  2003/12/25 10:28:18  yada
  型ルール統一による変更。MI_WRAM→ MIWram に。

  Revision 1.3  2003/12/24 08:30:36  nishida_kenji
  change include file(do not need include gxstate.h)

  Revision 1.2  2003/12/24 08:26:39  nishida_kenji
  remove needless code(small fix)

  Revision 1.1  2003/12/24 07:52:24  yada
  初版： GXからMI に WRAM関連移管


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/mi/wram.h>
#include <nitro/misc.h>


//    assert definition
#define MIi_WRAM_ASSERT( x ) \
    SDK_ASSERT( (x) == MI_WRAM_ARM9_ALL || \
                (x) == MI_WRAM_1616_1   || \
                (x) == MI_WRAM_1616_2   || \
                (x) == MI_WRAM_ARM7_ALL )


#ifdef SDK_ARM9
/*---------------------------------------------------------------------------*
  Name:         MI_SetWramBank

  Description:  set common-Wram bank status

  Arguments:    cnt      status of common-Wram.
                   MI_WRAM_ARM9_ALL :  allocate all blocks for ARM9
                   MI_WRAM_1616_1   :  allocate block1 for ARM9, block for ARM7
                   MI_WRAM_1616_2   :  allocate block0 for ARM9, block for ARM7
                   MI_WRAM_ARM7_ALL :  allocate all blocks for ARM7

  Returns:      None.

      *Notice: only ARM9 can set common-Wram status.
               ARM7 can read only.
 *---------------------------------------------------------------------------*/
#ifdef  SDK_CW_WARNOFF_SAFESTRB
#include <nitro/code32.h>
#endif
void MI_SetWramBank(MIWram cnt)
{
    MIi_WRAM_ASSERT(cnt);
    reg_GX_VRAMCNT_WRAM = (u8)cnt;     // safe byte access
}

#ifdef  SDK_CW_WARNOFF_SAFESTRB
#include <nitro/codereset.h>
#endif

#endif // SDK_ARM9


/*---------------------------------------------------------------------------*
  Name:         MI_GetWramBank

  Description:  get common-Wram bank status

  Arguments:    None.

  Returns:      status of common-Wram.
                   MI_WRAM_ARM9_ALL :  allocate all blocks for ARM9
                   MI_WRAM_1616_1   :  allocate block1 for ARM9, block for ARM7
                   MI_WRAM_1616_2   :  allocate block0 for ARM9, block for ARM7
                   MI_WRAM_ARM7_ALL :  allocate all blocks for ARM7
 *---------------------------------------------------------------------------*/
MIWram MI_GetWramBank(void)
{
// (register names are different between ARM9 and ARM7)
#ifdef SDK_ARM9
    return (MIWram)(reg_GX_VRAMCNT_WRAM & MI_WRAM_ARM9_ALL);    // safe byte access
#else
    return (MIWram)(reg_GX_WVRAMSTAT & MI_WRAM_ARM9_ALL);
#endif
}
