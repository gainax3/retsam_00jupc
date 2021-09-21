/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - 
  File:     gx_vramcnt.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: gx_vramcnt.c,v $
  Revision 1.44  2006/01/18 02:11:21  kitase_hirotake
  do-indent

  Revision 1.43  2005/02/28 05:26:13  yosizaki
  do-indent.

  Revision 1.42  2005/02/15 08:13:27  terui
  GX_TrySetBankFor* 関数を追加。
  コード共有化のため、GX_SetBankFor* 関数を改造。
  GX_ResetBankFor* 関数の排他確認方法を変更。
  GX_DisableBankFor* 関数の排他確認方法を変更。

  Revision 1.41  2005/02/09 09:20:31  terui
  VRAMバンク切り替え時に排他チェックを行うように改造。

  Revision 1.40  2004/12/06 07:37:26  takano_makoto
  GX_SetBankForBGExの引数にGX_VRAM_BG_NONEが指定できないように修正

  Revision 1.39  2004/11/25 11:13:43  takano_makoto
  add GX_SetBankForBGEx

  Revision 1.38  2004/10/04 13:30:42  takano_makoto
  Add GX_VRAM_CLEARDEPTH_128_* to use clear depth without clear image.

  Revision 1.37  2004/06/07 00:52:10  takano_makoto
  modify GX_VRAM_SUB_OBJEXTPLTT_01_I to GX_VRAM_SUB_OBJEXTPLTT_0_I

  Revision 1.36  2004/06/02 13:06:42  takano_makoto
  Change EXTPLTT enum name.

  Revision 1.35  2004/04/12 10:37:13  takano_makoto
  Fix a bug in GX_VRAMCNT_SetBGEXTPLTT_.

  Revision 1.34  2004/04/02 06:02:54  takano_makoto
  Fix bugs in disableBankForX_

  Revision 1.33  2004/04/02 05:21:10  takano_makoto
  Add entry of GX_VRAM_ARM7_128_C

  Revision 1.32  2004/03/08 02:29:32  takano_makoto
  Modify about GX_VRAM_OBJ_80_EG.

  Revision 1.31  2004/03/04 13:43:37  takano_makoto
  Add GX_VRAM_BG_80_EF, GX_VRAM_BG_80_EG, GX_VRAM_OBJ_80_EF, GX_VRAM_OBJ_80_EG

  Revision 1.30  2004/02/18 00:59:09  yasu
  add ifdef SDK_CW_WARNOFF_SAFESTRB

  Revision 1.29  2004/02/17 10:43:52  nishida_kenji
  small fix.

  Revision 1.28  2004/02/17 05:32:16  nishida_kenji
  Add GX_VRAM_BG_256_AC, GX_VRAM_BG_256_AD, GX_VRAM_BG_256_BD,
        GX_VRAM_BG_384_ABD, GX_VRAM_BG_384_ACD.

  Revision 1.27  2004/02/12 11:09:02  yasu
  change to new location of include files ARM9/ARM7

  Revision 1.26  2004/02/12 08:12:16  nishida_kenji
  Add GX_VRAM_BG_16_G and GX_VRAM_OBJ_16_G.

  Revision 1.25  2004/02/12 07:06:27  nishida_kenji
  Avoid generating STRB in THUMB mode.

  Revision 1.23  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.22  2004/02/03 11:56:23  nishida_kenji
  add GX_VRAM_OBJ_128_B.

  Revision 1.21  2004/02/02 12:43:43  nishida_kenji
  add GX_GetSizeOfXXXXX.

  Revision 1.20  2004/01/27 11:14:37  nishida_kenji
  adds APIs for the sub engine.

  Revision 1.19  2004/01/05 02:02:20  nishida_kenji
  add GX_GetBankForXXXX

  Revision 1.18  2003/12/25 11:00:09  nishida_kenji
  converted by GX_APIChangeFrom031212-2.pl

  Revision 1.17  2003/12/25 00:19:29  nishida_kenji
  convert INLINE to inline

  Revision 1.16  2003/12/24 08:27:56  nishida_kenji
  move WRAM control to MI domain.

  Revision 1.15  2003/12/17 09:00:20  nishida_kenji
  Totally revised APIs.
  build/buildtools/GX_APIChangeFrom031212.pl would help you change your program to some extent.

  Revision 1.14  2003/12/17 08:53:17  nishida_kenji
  revise comments

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/gx/gx_vramcnt.h>
#include <nitro/hw/ARM9/ioreg_G3X.h>
#include <nitro/hw/ARM9/ioreg_GX.h>
#include <nitro/hw/ARM9/ioreg_GXS.h>
#include "gxstate.h"


//---------------------------------------------------------------------------
// Enum values for VRAMCNT-A(internal use only)
//---------------------------------------------------------------------------
typedef enum
{
    GX_VRAMCNT_A_DISABLE = 0,
    GX_VRAMCNT_A_LCDC_0x06800000 =
        (0 << REG_GX_VRAMCNT_A_MST_SHIFT) | (0 << REG_GX_VRAMCNT_A_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_A_E_SHIFT),
    GX_VRAMCNT_A_BG_0x06000000 =
        (1 << REG_GX_VRAMCNT_A_MST_SHIFT) | (0 << REG_GX_VRAMCNT_A_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_A_E_SHIFT),
    GX_VRAMCNT_A_BG_0x06020000 =
        (1 << REG_GX_VRAMCNT_A_MST_SHIFT) | (1 << REG_GX_VRAMCNT_A_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_A_E_SHIFT),
    GX_VRAMCNT_A_BG_0x06040000 =
        (1 << REG_GX_VRAMCNT_A_MST_SHIFT) | (2 << REG_GX_VRAMCNT_A_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_A_E_SHIFT),
    GX_VRAMCNT_A_BG_0x06060000 =
        (1 << REG_GX_VRAMCNT_A_MST_SHIFT) | (3 << REG_GX_VRAMCNT_A_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_A_E_SHIFT),
    GX_VRAMCNT_A_OBJ_0x06400000 =
        (2 << REG_GX_VRAMCNT_A_MST_SHIFT) | (0 << REG_GX_VRAMCNT_A_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_A_E_SHIFT),
    GX_VRAMCNT_A_OBJ_0x06420000 =
        (2 << REG_GX_VRAMCNT_A_MST_SHIFT) | (1 << REG_GX_VRAMCNT_A_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_A_E_SHIFT),
    GX_VRAMCNT_A_TEX_0 =
        (3 << REG_GX_VRAMCNT_A_MST_SHIFT) | (0 << REG_GX_VRAMCNT_A_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_A_E_SHIFT),
    GX_VRAMCNT_A_TEX_1 =
        (3 << REG_GX_VRAMCNT_A_MST_SHIFT) | (1 << REG_GX_VRAMCNT_A_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_A_E_SHIFT),
    GX_VRAMCNT_A_TEX_2 =
        (3 << REG_GX_VRAMCNT_A_MST_SHIFT) | (2 << REG_GX_VRAMCNT_A_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_A_E_SHIFT),
    GX_VRAMCNT_A_TEX_3 =
        (3 << REG_GX_VRAMCNT_A_MST_SHIFT) | (3 << REG_GX_VRAMCNT_A_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_A_E_SHIFT)
}
GX_VRAMCNT_A;


//---------------------------------------------------------------------------
// Enum values for VRAMCNT-B(internal use only)
//---------------------------------------------------------------------------
typedef enum
{
    GX_VRAMCNT_B_DISABLE = 0,
    GX_VRAMCNT_B_LCDC_0x06820000 =
        (0 << REG_GX_VRAMCNT_B_MST_SHIFT) | (0 << REG_GX_VRAMCNT_B_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_B_E_SHIFT),
    GX_VRAMCNT_B_BG_0x06000000 =
        (1 << REG_GX_VRAMCNT_B_MST_SHIFT) | (0 << REG_GX_VRAMCNT_B_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_B_E_SHIFT),
    GX_VRAMCNT_B_BG_0x06020000 =
        (1 << REG_GX_VRAMCNT_B_MST_SHIFT) | (1 << REG_GX_VRAMCNT_B_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_B_E_SHIFT),
    GX_VRAMCNT_B_BG_0x06040000 =
        (1 << REG_GX_VRAMCNT_B_MST_SHIFT) | (2 << REG_GX_VRAMCNT_B_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_B_E_SHIFT),
    GX_VRAMCNT_B_BG_0x06060000 =
        (1 << REG_GX_VRAMCNT_B_MST_SHIFT) | (3 << REG_GX_VRAMCNT_B_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_B_E_SHIFT),
    GX_VRAMCNT_B_OBJ_0x06400000 =
        (2 << REG_GX_VRAMCNT_B_MST_SHIFT) | (0 << REG_GX_VRAMCNT_B_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_B_E_SHIFT),
    GX_VRAMCNT_B_OBJ_0x06420000 =
        (2 << REG_GX_VRAMCNT_B_MST_SHIFT) | (1 << REG_GX_VRAMCNT_B_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_B_E_SHIFT),
    GX_VRAMCNT_B_TEX_0 =
        (3 << REG_GX_VRAMCNT_B_MST_SHIFT) | (0 << REG_GX_VRAMCNT_B_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_B_E_SHIFT),
    GX_VRAMCNT_B_TEX_1 =
        (3 << REG_GX_VRAMCNT_B_MST_SHIFT) | (1 << REG_GX_VRAMCNT_B_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_B_E_SHIFT),
    GX_VRAMCNT_B_TEX_2 =
        (3 << REG_GX_VRAMCNT_B_MST_SHIFT) | (2 << REG_GX_VRAMCNT_B_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_B_E_SHIFT),
    GX_VRAMCNT_B_TEX_3 =
        (3 << REG_GX_VRAMCNT_B_MST_SHIFT) | (3 << REG_GX_VRAMCNT_B_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_B_E_SHIFT)
}
GX_VRAMCNT_B;


//---------------------------------------------------------------------------
// Enum values for VRAMCNT-C(internal use only)
//---------------------------------------------------------------------------
typedef enum
{
    GX_VRAMCNT_C_DISABLE = 0,
    GX_VRAMCNT_C_LCDC_0x06840000 =
        (0 << REG_GX_VRAMCNT_C_MST_SHIFT) | (0 << REG_GX_VRAMCNT_C_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_C_E_SHIFT),
    GX_VRAMCNT_C_BG_0x06000000 =
        (1 << REG_GX_VRAMCNT_C_MST_SHIFT) | (0 << REG_GX_VRAMCNT_C_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_C_E_SHIFT),
    GX_VRAMCNT_C_BG_0x06020000 =
        (1 << REG_GX_VRAMCNT_C_MST_SHIFT) | (1 << REG_GX_VRAMCNT_C_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_C_E_SHIFT),
    GX_VRAMCNT_C_BG_0x06040000 =
        (1 << REG_GX_VRAMCNT_C_MST_SHIFT) | (2 << REG_GX_VRAMCNT_C_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_C_E_SHIFT),
    GX_VRAMCNT_C_BG_0x06060000 =
        (1 << REG_GX_VRAMCNT_C_MST_SHIFT) | (3 << REG_GX_VRAMCNT_C_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_C_E_SHIFT),
    GX_VRAMCNT_C_ARM7_0x06000000 =
        (2 << REG_GX_VRAMCNT_C_MST_SHIFT) | (0 << REG_GX_VRAMCNT_C_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_C_E_SHIFT),
    GX_VRAMCNT_C_ARM7_0x06020000 =
        (2 << REG_GX_VRAMCNT_C_MST_SHIFT) | (1 << REG_GX_VRAMCNT_C_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_C_E_SHIFT),
    GX_VRAMCNT_C_TEX_0 =
        (3 << REG_GX_VRAMCNT_C_MST_SHIFT) | (0 << REG_GX_VRAMCNT_C_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_C_E_SHIFT),
    GX_VRAMCNT_C_TEX_1 =
        (3 << REG_GX_VRAMCNT_C_MST_SHIFT) | (1 << REG_GX_VRAMCNT_C_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_C_E_SHIFT),
    GX_VRAMCNT_C_TEX_2 =
        (3 << REG_GX_VRAMCNT_C_MST_SHIFT) | (2 << REG_GX_VRAMCNT_C_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_C_E_SHIFT),
    GX_VRAMCNT_C_TEX_3 =
        (3 << REG_GX_VRAMCNT_C_MST_SHIFT) | (3 << REG_GX_VRAMCNT_C_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_C_E_SHIFT),
    GX_VRAMCNT_C_SUBBG_0x06200000 =
        (4 << REG_GX_VRAMCNT_C_MST_SHIFT) | (1 << REG_GX_VRAMCNT_C_E_SHIFT)
}
GX_VRAMCNT_C;


//---------------------------------------------------------------------------
// Enum values for VRAMCNT-D(internal use only)
//---------------------------------------------------------------------------
typedef enum
{
    GX_VRAMCNT_D_DISABLE = 0,
    GX_VRAMCNT_D_LCDC_0x06860000 =
        (0 << REG_GX_VRAMCNT_D_MST_SHIFT) | (0 << REG_GX_VRAMCNT_D_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_D_E_SHIFT),
    GX_VRAMCNT_D_BG_0x06000000 =
        (1 << REG_GX_VRAMCNT_D_MST_SHIFT) | (0 << REG_GX_VRAMCNT_D_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_D_E_SHIFT),
    GX_VRAMCNT_D_BG_0x06020000 =
        (1 << REG_GX_VRAMCNT_D_MST_SHIFT) | (1 << REG_GX_VRAMCNT_D_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_D_E_SHIFT),
    GX_VRAMCNT_D_BG_0x06040000 =
        (1 << REG_GX_VRAMCNT_D_MST_SHIFT) | (2 << REG_GX_VRAMCNT_D_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_D_E_SHIFT),
    GX_VRAMCNT_D_BG_0x06060000 =
        (1 << REG_GX_VRAMCNT_D_MST_SHIFT) | (3 << REG_GX_VRAMCNT_D_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_D_E_SHIFT),
    GX_VRAMCNT_D_ARM7_0x06000000 =
        (2 << REG_GX_VRAMCNT_D_MST_SHIFT) | (0 << REG_GX_VRAMCNT_D_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_D_E_SHIFT),
    GX_VRAMCNT_D_ARM7_0x06020000 =
        (2 << REG_GX_VRAMCNT_D_MST_SHIFT) | (1 << REG_GX_VRAMCNT_D_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_D_E_SHIFT),
    GX_VRAMCNT_D_TEX_0 =
        (3 << REG_GX_VRAMCNT_D_MST_SHIFT) | (0 << REG_GX_VRAMCNT_D_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_D_E_SHIFT),
    GX_VRAMCNT_D_TEX_1 =
        (3 << REG_GX_VRAMCNT_D_MST_SHIFT) | (1 << REG_GX_VRAMCNT_D_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_D_E_SHIFT),
    GX_VRAMCNT_D_TEX_2 =
        (3 << REG_GX_VRAMCNT_D_MST_SHIFT) | (2 << REG_GX_VRAMCNT_D_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_D_E_SHIFT),
    GX_VRAMCNT_D_TEX_3 =
        (3 << REG_GX_VRAMCNT_D_MST_SHIFT) | (3 << REG_GX_VRAMCNT_D_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_D_E_SHIFT),
    GX_VRAMCNT_D_SUBOBJ_0x06600000 =
        (4 << REG_GX_VRAMCNT_D_MST_SHIFT) | (1 << REG_GX_VRAMCNT_D_E_SHIFT)
}
GX_VRAMCNT_D;


//---------------------------------------------------------------------------
// Enum values for VRAMCNT-E(internal use only)
//---------------------------------------------------------------------------
typedef enum
{
    GX_VRAMCNT_E_DISABLE = 0,
    GX_VRAMCNT_E_LCDC_0x06880000 =
        (0 << REG_GX_VRAMCNT_E_MST_SHIFT) | (1 << REG_GX_VRAMCNT_E_E_SHIFT),
    GX_VRAMCNT_E_BG_0x06000000 =
        (1 << REG_GX_VRAMCNT_E_MST_SHIFT) | (1 << REG_GX_VRAMCNT_E_E_SHIFT),
    GX_VRAMCNT_E_OBJ_0x06400000 =
        (2 << REG_GX_VRAMCNT_E_MST_SHIFT) | (1 << REG_GX_VRAMCNT_E_E_SHIFT),
    GX_VRAMCNT_E_TEXPLTT_0123 = (3 << REG_GX_VRAMCNT_E_MST_SHIFT) | (1 << REG_GX_VRAMCNT_E_E_SHIFT),
    GX_VRAMCNT_E_BGEXTPLTT_0123 =
        (4 << REG_GX_VRAMCNT_E_MST_SHIFT) | (1 << REG_GX_VRAMCNT_E_E_SHIFT)
}
GX_VRAMCNT_E;


//---------------------------------------------------------------------------
// Enum values for VRAMCNT-F(internal use only)
//---------------------------------------------------------------------------
typedef enum
{
    GX_VRAMCNT_F_DISABLE = 0,
    GX_VRAMCNT_F_LCDC_0x06890000 =
        (0 << REG_GX_VRAMCNT_F_MST_SHIFT) | (0 << REG_GX_VRAMCNT_F_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_F_E_SHIFT),
    GX_VRAMCNT_F_BG_0x06000000 =
        (1 << REG_GX_VRAMCNT_F_MST_SHIFT) | (0 << REG_GX_VRAMCNT_F_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_F_E_SHIFT),
    GX_VRAMCNT_F_BG_0x06004000 =
        (1 << REG_GX_VRAMCNT_F_MST_SHIFT) | (1 << REG_GX_VRAMCNT_F_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_F_E_SHIFT),
    GX_VRAMCNT_F_BG_0x06010000 =
        (1 << REG_GX_VRAMCNT_F_MST_SHIFT) | (2 << REG_GX_VRAMCNT_F_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_F_E_SHIFT),
    GX_VRAMCNT_F_BG_0x06014000 =
        (1 << REG_GX_VRAMCNT_F_MST_SHIFT) | (3 << REG_GX_VRAMCNT_F_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_F_E_SHIFT),
    GX_VRAMCNT_F_OBJ_0x06400000 =
        (2 << REG_GX_VRAMCNT_F_MST_SHIFT) | (0 << REG_GX_VRAMCNT_F_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_F_E_SHIFT),
    GX_VRAMCNT_F_OBJ_0x06404000 =
        (2 << REG_GX_VRAMCNT_F_MST_SHIFT) | (1 << REG_GX_VRAMCNT_F_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_F_E_SHIFT),
    GX_VRAMCNT_F_OBJ_0x06410000 =
        (2 << REG_GX_VRAMCNT_F_MST_SHIFT) | (2 << REG_GX_VRAMCNT_F_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_F_E_SHIFT),
    GX_VRAMCNT_F_OBJ_0x06414000 =
        (2 << REG_GX_VRAMCNT_F_MST_SHIFT) | (3 << REG_GX_VRAMCNT_F_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_F_E_SHIFT),
    GX_VRAMCNT_F_TEXPLTT_0 =
        (3 << REG_GX_VRAMCNT_F_MST_SHIFT) | (0 << REG_GX_VRAMCNT_F_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_F_E_SHIFT),
    GX_VRAMCNT_F_TEXPLTT_1 =
        (3 << REG_GX_VRAMCNT_F_MST_SHIFT) | (1 << REG_GX_VRAMCNT_F_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_F_E_SHIFT),
    GX_VRAMCNT_F_TEXPLTT_4 =
        (3 << REG_GX_VRAMCNT_F_MST_SHIFT) | (2 << REG_GX_VRAMCNT_F_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_F_E_SHIFT),
    GX_VRAMCNT_F_TEXPLTT_5 =
        (3 << REG_GX_VRAMCNT_F_MST_SHIFT) | (3 << REG_GX_VRAMCNT_F_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_F_E_SHIFT),
    GX_VRAMCNT_F_BGEXTPLTT_01 =
        (4 << REG_GX_VRAMCNT_F_MST_SHIFT) | (0 << REG_GX_VRAMCNT_F_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_F_E_SHIFT),
    GX_VRAMCNT_F_BGEXTPLTT_23 =
        (4 << REG_GX_VRAMCNT_F_MST_SHIFT) | (1 << REG_GX_VRAMCNT_F_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_F_E_SHIFT),
    GX_VRAMCNT_F_OBJEXTPLTT =
        (5 << REG_GX_VRAMCNT_F_MST_SHIFT) | (0 << REG_GX_VRAMCNT_F_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_F_E_SHIFT)
}
GX_VRAMCNT_F;


//---------------------------------------------------------------------------
// Enum values for VRAMCNT-G(internal use only)
//---------------------------------------------------------------------------
typedef enum
{
    GX_VRAMCNT_G_DISABLE = 0,
    GX_VRAMCNT_G_LCDC_0x06894000 =
        (0 << REG_GX_VRAMCNT_G_MST_SHIFT) | (0 << REG_GX_VRAMCNT_G_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_G_E_SHIFT),
    GX_VRAMCNT_G_BG_0x06000000 =
        (1 << REG_GX_VRAMCNT_G_MST_SHIFT) | (0 << REG_GX_VRAMCNT_G_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_G_E_SHIFT),
    GX_VRAMCNT_G_BG_0x06004000 =
        (1 << REG_GX_VRAMCNT_G_MST_SHIFT) | (1 << REG_GX_VRAMCNT_G_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_G_E_SHIFT),
    GX_VRAMCNT_G_BG_0x06010000 =
        (1 << REG_GX_VRAMCNT_G_MST_SHIFT) | (2 << REG_GX_VRAMCNT_G_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_G_E_SHIFT),
    GX_VRAMCNT_G_BG_0x06014000 =
        (1 << REG_GX_VRAMCNT_G_MST_SHIFT) | (3 << REG_GX_VRAMCNT_G_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_G_E_SHIFT),
    GX_VRAMCNT_G_OBJ_0x06400000 =
        (2 << REG_GX_VRAMCNT_G_MST_SHIFT) | (0 << REG_GX_VRAMCNT_G_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_G_E_SHIFT),
    GX_VRAMCNT_G_OBJ_0x06404000 =
        (2 << REG_GX_VRAMCNT_G_MST_SHIFT) | (1 << REG_GX_VRAMCNT_G_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_G_E_SHIFT),
    GX_VRAMCNT_G_OBJ_0x06410000 =
        (2 << REG_GX_VRAMCNT_G_MST_SHIFT) | (2 << REG_GX_VRAMCNT_G_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_G_E_SHIFT),
    GX_VRAMCNT_G_OBJ_0x06414000 =
        (2 << REG_GX_VRAMCNT_G_MST_SHIFT) | (3 << REG_GX_VRAMCNT_G_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_G_E_SHIFT),
    GX_VRAMCNT_G_TEXPLTT_0 =
        (3 << REG_GX_VRAMCNT_G_MST_SHIFT) | (0 << REG_GX_VRAMCNT_G_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_G_E_SHIFT),
    GX_VRAMCNT_G_TEXPLTT_1 =
        (3 << REG_GX_VRAMCNT_G_MST_SHIFT) | (1 << REG_GX_VRAMCNT_G_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_G_E_SHIFT),
    GX_VRAMCNT_G_TEXPLTT_4 =
        (3 << REG_GX_VRAMCNT_G_MST_SHIFT) | (2 << REG_GX_VRAMCNT_G_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_G_E_SHIFT),
    GX_VRAMCNT_G_TEXPLTT_5 =
        (3 << REG_GX_VRAMCNT_G_MST_SHIFT) | (3 << REG_GX_VRAMCNT_G_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_G_E_SHIFT),
    GX_VRAMCNT_G_BGEXTPLTT_01 =
        (4 << REG_GX_VRAMCNT_G_MST_SHIFT) | (0 << REG_GX_VRAMCNT_G_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_G_E_SHIFT),
    GX_VRAMCNT_G_BGEXTPLTT_23 =
        (4 << REG_GX_VRAMCNT_G_MST_SHIFT) | (1 << REG_GX_VRAMCNT_G_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_G_E_SHIFT),
    GX_VRAMCNT_G_OBJEXTPLTT =
        (5 << REG_GX_VRAMCNT_G_MST_SHIFT) | (0 << REG_GX_VRAMCNT_G_OFS_SHIFT) | (1 <<
                                                                                 REG_GX_VRAMCNT_G_E_SHIFT)
}
GX_VRAMCNT_G;


//---------------------------------------------------------------------------
// Enum values for VRAMCNT-H(internal use only)
//---------------------------------------------------------------------------
typedef enum
{
    GX_VRAMCNT_H_DISABLE = 0,
    GX_VRAMCNT_H_LCDC_0x06898000 =
        (0 << REG_GX_VRAMCNT_H_MST_SHIFT) | (1 << REG_GX_VRAMCNT_H_E_SHIFT),
    GX_VRAMCNT_H_SUBBG_0x06200000 =
        (1 << REG_GX_VRAMCNT_H_MST_SHIFT) | (1 << REG_GX_VRAMCNT_H_E_SHIFT),
    GX_VRAMCNT_H_SUBBGEXTPLTT_0123 =
        (2 << REG_GX_VRAMCNT_H_MST_SHIFT) | (1 << REG_GX_VRAMCNT_H_E_SHIFT)
}
GX_VRAMCNT_H;


//---------------------------------------------------------------------------
// Enum values for VRAMCNT-I(internal use only)
//---------------------------------------------------------------------------
typedef enum
{
    GX_VRAMCNT_I_DISABLE = 0,
    GX_VRAMCNT_I_LCDC_0x068A0000 =
        (0 << REG_GX_VRAMCNT_I_MST_SHIFT) | (1 << REG_GX_VRAMCNT_I_E_SHIFT),
    GX_VRAMCNT_I_SUBBG_0x06208000 =
        (1 << REG_GX_VRAMCNT_I_MST_SHIFT) | (1 << REG_GX_VRAMCNT_I_E_SHIFT),
    GX_VRAMCNT_I_SUBOBJ_0x06600000 =
        (2 << REG_GX_VRAMCNT_I_MST_SHIFT) | (1 << REG_GX_VRAMCNT_I_E_SHIFT),
    GX_VRAMCNT_I_SUBOBJEXTPLTT = (3 << REG_GX_VRAMCNT_I_MST_SHIFT) | (1 << REG_GX_VRAMCNT_I_E_SHIFT)
}
GX_VRAMCNT_I;

#ifdef  SDK_CW_WARNOFF_SAFESTRB
#include <nitro/code32.h>
#endif
/*---------------------------------------------------------------------------*
  Name:         GX_VRAMCNT_SetLCDC_(internal use only)

  Description:  Assigns banks onto LCDC.
                DO NOT INLINE THIS.

  Arguments:    lcdc       banks assigned to LCDC.

  Returns:      none
 *---------------------------------------------------------------------------*/
static void GX_VRAMCNT_SetLCDC_(int lcdc)
{
    if (lcdc & GX_VRAM_LCDC_A)
        reg_GX_VRAMCNT_A = (u8)GX_VRAMCNT_A_LCDC_0x06800000;
    if (lcdc & GX_VRAM_LCDC_B)
        reg_GX_VRAMCNT_B = (u8)GX_VRAMCNT_B_LCDC_0x06820000;
    if (lcdc & GX_VRAM_LCDC_C)
        reg_GX_VRAMCNT_C = (u8)GX_VRAMCNT_C_LCDC_0x06840000;
    if (lcdc & GX_VRAM_LCDC_D)
        reg_GX_VRAMCNT_D = (u8)GX_VRAMCNT_D_LCDC_0x06860000;
    if (lcdc & GX_VRAM_LCDC_E)
        reg_GX_VRAMCNT_E = (u8)GX_VRAMCNT_E_LCDC_0x06880000;
    if (lcdc & GX_VRAM_LCDC_F)
        reg_GX_VRAMCNT_F = (u8)GX_VRAMCNT_F_LCDC_0x06890000;
    if (lcdc & GX_VRAM_LCDC_G)
        reg_GX_VRAMCNT_G = (u8)GX_VRAMCNT_G_LCDC_0x06894000;
    if (lcdc & GX_VRAM_LCDC_H)
        reg_GX_VRAMCNT_H = (u8)GX_VRAMCNT_H_LCDC_0x06898000;
    if (lcdc & GX_VRAM_LCDC_I)
        reg_GX_VRAMCNT_I = (u8)GX_VRAMCNT_I_LCDC_0x068A0000;
}

/*---------------------------------------------------------------------------*
  Name:         GX_VRAMCNT_SetBG_(internal use only)

  Description:  Assigns banks onto BG.
                The banks are mapped consecutively from HW_BG_VRAM(0x06000000).
                This is called only by GX_SetBankForBG.

  Arguments:    bg         banks assigned to BG

  Returns:      none
 *---------------------------------------------------------------------------*/
static inline void GX_VRAMCNT_SetBG_(GXVRamBG bg)
{
    switch (bg)
    {
    case GX_VRAM_BG_128_D:
        reg_GX_VRAMCNT_D = (u8)GX_VRAMCNT_D_BG_0x06000000;
        break;

    case GX_VRAM_BG_256_CD:
        reg_GX_VRAMCNT_D = (u8)GX_VRAMCNT_D_BG_0x06020000;
        // don't break
    case GX_VRAM_BG_128_C:
        reg_GX_VRAMCNT_C = (u8)GX_VRAMCNT_C_BG_0x06000000;
        break;

    case GX_VRAM_BG_384_BCD:
        reg_GX_VRAMCNT_D = (u8)GX_VRAMCNT_D_BG_0x06040000;
        // don't break
    case GX_VRAM_BG_256_BC:
        reg_GX_VRAMCNT_C = (u8)GX_VRAMCNT_C_BG_0x06020000;
        // don't break
    case GX_VRAM_BG_128_B:
        reg_GX_VRAMCNT_B = (u8)GX_VRAMCNT_B_BG_0x06000000;
        break;

    case GX_VRAM_BG_512_ABCD:
        reg_GX_VRAMCNT_D = (u8)GX_VRAMCNT_D_BG_0x06060000;
        // don't break
    case GX_VRAM_BG_384_ABC:
        reg_GX_VRAMCNT_C = (u8)GX_VRAMCNT_C_BG_0x06040000;
        // don't break
    case GX_VRAM_BG_256_AB:
        reg_GX_VRAMCNT_B = (u8)GX_VRAMCNT_B_BG_0x06020000;
        // don't break
    case GX_VRAM_BG_128_A:
        reg_GX_VRAMCNT_A = (u8)GX_VRAMCNT_A_BG_0x06000000;
        // don't break
    case GX_VRAM_BG_NONE:
        break;

    case GX_VRAM_BG_384_ABD:
        reg_GX_VRAMCNT_A = (u8)GX_VRAMCNT_A_BG_0x06000000;
        reg_GX_VRAMCNT_B = (u8)GX_VRAMCNT_B_BG_0x06020000;
        reg_GX_VRAMCNT_D = (u8)GX_VRAMCNT_D_BG_0x06040000;
        break;

    case GX_VRAM_BG_384_ACD:
        reg_GX_VRAMCNT_D = (u8)GX_VRAMCNT_D_BG_0x06040000;
        // don't break
    case GX_VRAM_BG_256_AC:
        reg_GX_VRAMCNT_A = (u8)GX_VRAMCNT_A_BG_0x06000000;
        reg_GX_VRAMCNT_C = (u8)GX_VRAMCNT_C_BG_0x06020000;
        break;

    case GX_VRAM_BG_256_AD:
        reg_GX_VRAMCNT_A = (u8)GX_VRAMCNT_A_BG_0x06000000;
        reg_GX_VRAMCNT_D = (u8)GX_VRAMCNT_D_BG_0x06020000;
        break;

    case GX_VRAM_BG_256_BD:
        reg_GX_VRAMCNT_B = (u8)GX_VRAMCNT_B_BG_0x06000000;
        reg_GX_VRAMCNT_D = (u8)GX_VRAMCNT_D_BG_0x06020000;
        break;

    case GX_VRAM_BG_96_EFG:
        reg_GX_VRAMCNT_G = (u8)GX_VRAMCNT_G_BG_0x06014000;
        // don't break;
    case GX_VRAM_BG_80_EF:
        reg_GX_VRAMCNT_F = (u8)GX_VRAMCNT_F_BG_0x06010000;
        // don't break;
    case GX_VRAM_BG_64_E:
        reg_GX_VRAMCNT_E = (u8)GX_VRAMCNT_E_BG_0x06000000;
        break;

    case GX_VRAM_BG_80_EG:
        reg_GX_VRAMCNT_G = (u8)GX_VRAMCNT_G_BG_0x06010000;
        reg_GX_VRAMCNT_E = (u8)GX_VRAMCNT_E_BG_0x06000000;
        break;

    case GX_VRAM_BG_32_FG:
        reg_GX_VRAMCNT_G = (u8)GX_VRAMCNT_G_BG_0x06004000;
        // don't break
    case GX_VRAM_BG_16_F:
        reg_GX_VRAMCNT_F = (u8)GX_VRAMCNT_F_BG_0x06000000;
        break;

    case GX_VRAM_BG_16_G:
        reg_GX_VRAMCNT_G = (u8)GX_VRAMCNT_G_BG_0x06000000;
        break;

    default:
        SDK_INTERNAL_ERROR("unknown GXVRamBG, 0x%x specified.", bg);
        break;
    }
}


/*---------------------------------------------------------------------------*
  Name:         GX_VRAMCNT_SetBGEx1_(internal use only)

  Description:  Assigns banks onto BG.
                The banks are mapped consecutively from HW_BG_VRAM(0x06000000).
                This is called only by GX_SetBankForBG.

  Arguments:    bg         banks assigned to BG

  Returns:      none
 *---------------------------------------------------------------------------*/
static inline void GX_VRAMCNT_SetBGEx1_(GXVRamBG bg)
{
    switch (bg)
    {
    case GX_VRAM_BG_96_EFG:
        reg_GX_VRAMCNT_G = (u8)GX_VRAMCNT_G_BG_0x06014000;
        // don't break;
    case GX_VRAM_BG_80_EF:
        reg_GX_VRAMCNT_F = (u8)GX_VRAMCNT_F_BG_0x06010000;
        // don't break;
    case GX_VRAM_BG_64_E:
        reg_GX_VRAMCNT_E = (u8)GX_VRAMCNT_E_BG_0x06000000;
        break;

    case GX_VRAM_BG_80_EG:
        reg_GX_VRAMCNT_G = (u8)GX_VRAMCNT_G_BG_0x06010000;
        reg_GX_VRAMCNT_E = (u8)GX_VRAMCNT_E_BG_0x06000000;
        break;

    case GX_VRAM_BG_32_FG:
        reg_GX_VRAMCNT_G = (u8)GX_VRAMCNT_G_BG_0x06004000;
        // don't break
    case GX_VRAM_BG_16_F:
        reg_GX_VRAMCNT_F = (u8)GX_VRAMCNT_F_BG_0x06000000;
        break;

    case GX_VRAM_BG_16_G:
        reg_GX_VRAMCNT_G = (u8)GX_VRAMCNT_G_BG_0x06000000;
        break;

    default:
        SDK_INTERNAL_ERROR("unknown GXVRamBG, 0x%x specified.", bg);
        break;
    }
}

/*---------------------------------------------------------------------------*
  Name:         GX_VRAMCNT_SetBGEx2_(internal use only)

  Description:  Assigns banks onto BG.
                The banks are mapped consecutively from HW_BG_VRAM(0x06000000).
                This is called only by GX_SetBankForBG.

  Arguments:    bg         banks assigned to BG

  Returns:      none
 *---------------------------------------------------------------------------*/
static inline void GX_VRAMCNT_SetBGEx2_(GXVRamBG bg)
{
    switch (bg)
    {
    case GX_VRAM_BG_128_D:
        reg_GX_VRAMCNT_D = (u8)GX_VRAMCNT_D_BG_0x06020000;
        break;

    case GX_VRAM_BG_256_CD:
        reg_GX_VRAMCNT_D = (u8)GX_VRAMCNT_D_BG_0x06040000;
        // don't break
    case GX_VRAM_BG_128_C:
        reg_GX_VRAMCNT_C = (u8)GX_VRAMCNT_C_BG_0x06020000;
        break;

    case GX_VRAM_BG_384_BCD:
        reg_GX_VRAMCNT_D = (u8)GX_VRAMCNT_D_BG_0x06060000;
        // don't break
    case GX_VRAM_BG_256_BC:
        reg_GX_VRAMCNT_C = (u8)GX_VRAMCNT_C_BG_0x06040000;
        // don't break
    case GX_VRAM_BG_128_B:
        reg_GX_VRAMCNT_B = (u8)GX_VRAMCNT_B_BG_0x06020000;
        break;

    case GX_VRAM_BG_384_ABC:
        reg_GX_VRAMCNT_C = (u8)GX_VRAMCNT_C_BG_0x06060000;
        // don't break
    case GX_VRAM_BG_256_AB:
        reg_GX_VRAMCNT_B = (u8)GX_VRAMCNT_B_BG_0x06040000;
        // don't break
    case GX_VRAM_BG_128_A:
        reg_GX_VRAMCNT_A = (u8)GX_VRAMCNT_A_BG_0x06020000;
        // don't break
    case GX_VRAM_BG_NONE:
        break;

    case GX_VRAM_BG_384_ABD:
        reg_GX_VRAMCNT_A = (u8)GX_VRAMCNT_A_BG_0x06020000;
        reg_GX_VRAMCNT_B = (u8)GX_VRAMCNT_B_BG_0x06040000;
        reg_GX_VRAMCNT_D = (u8)GX_VRAMCNT_D_BG_0x06060000;
        break;

    case GX_VRAM_BG_384_ACD:
        reg_GX_VRAMCNT_D = (u8)GX_VRAMCNT_D_BG_0x06060000;
        // don't break
    case GX_VRAM_BG_256_AC:
        reg_GX_VRAMCNT_A = (u8)GX_VRAMCNT_A_BG_0x06020000;
        reg_GX_VRAMCNT_C = (u8)GX_VRAMCNT_C_BG_0x06040000;
        break;

    case GX_VRAM_BG_256_AD:
        reg_GX_VRAMCNT_A = (u8)GX_VRAMCNT_A_BG_0x06020000;
        reg_GX_VRAMCNT_D = (u8)GX_VRAMCNT_D_BG_0x06040000;
        break;

    case GX_VRAM_BG_256_BD:
        reg_GX_VRAMCNT_B = (u8)GX_VRAMCNT_B_BG_0x06020000;
        reg_GX_VRAMCNT_D = (u8)GX_VRAMCNT_D_BG_0x06040000;
        break;

    default:
        SDK_INTERNAL_ERROR("unknown GXVRamBG, 0x%x specified.", bg);
        break;
    }
}


/*---------------------------------------------------------------------------*
  Name:         GX_VRAMCNT_SetOBJ_(internal use only)

  Description:  Assigns banks onto OBJ.
                The banks are mapped consecutively from HW_OBJ_VRAM(0x06400000).
                This is called only by GX_SetBankForOBJ.

  Arguments:    obj        banks assigned to OBJ

  Returns:      none
 *---------------------------------------------------------------------------*/
static inline void GX_VRAMCNT_SetOBJ_(GXVRamOBJ obj)
{
    switch (obj)
    {
    case GX_VRAM_OBJ_256_AB:
        reg_GX_VRAMCNT_B = (u8)GX_VRAMCNT_B_OBJ_0x06420000;
        // don't break
    case GX_VRAM_OBJ_128_A:
        reg_GX_VRAMCNT_A = (u8)GX_VRAMCNT_A_OBJ_0x06400000;
        // don't break
    case GX_VRAM_OBJ_NONE:
        break;

    case GX_VRAM_OBJ_128_B:
        reg_GX_VRAMCNT_B = (u8)GX_VRAMCNT_B_OBJ_0x06400000;
        break;

    case GX_VRAM_OBJ_96_EFG:
        reg_GX_VRAMCNT_G = (u8)GX_VRAMCNT_G_OBJ_0x06414000;
        // don't break
    case GX_VRAM_OBJ_80_EF:
        reg_GX_VRAMCNT_F = (u8)GX_VRAMCNT_F_OBJ_0x06410000;
        // don't break
    case GX_VRAM_OBJ_64_E:
        reg_GX_VRAMCNT_E = (u8)GX_VRAMCNT_E_OBJ_0x06400000;
        break;

    case GX_VRAM_OBJ_80_EG:
        reg_GX_VRAMCNT_G = (u8)GX_VRAMCNT_G_OBJ_0x06410000;
        reg_GX_VRAMCNT_E = (u8)GX_VRAMCNT_E_OBJ_0x06400000;
        break;

    case GX_VRAM_OBJ_32_FG:
        reg_GX_VRAMCNT_G = (u8)GX_VRAMCNT_G_OBJ_0x06404000;
        // don't break
    case GX_VRAM_OBJ_16_F:
        reg_GX_VRAMCNT_F = (u8)GX_VRAMCNT_F_OBJ_0x06400000;
        break;

    case GX_VRAM_OBJ_16_G:
        reg_GX_VRAMCNT_G = (u8)GX_VRAMCNT_G_OBJ_0x06400000;
        break;

    default:
        SDK_INTERNAL_ERROR("unknown GXVRamOBJ, 0x%x specified.", obj);
        break;
    }
}

/*---------------------------------------------------------------------------*
  Name:         GX_VRAMCNT_SetARM7_(internal use only)

  Description:  Assigns banks to ARM7.
                The banks are mapped consecutively from ARM7's 0x06000000.
                This is called only by GX_SetBankForARM7.

  Arguments:    arm7       banks assigned to ARM7

  Returns:      none
 *---------------------------------------------------------------------------*/
static inline void GX_VRAMCNT_SetARM7_(GXVRamARM7 arm7)
{
    switch (arm7)
    {
    case GX_VRAM_ARM7_256_CD:
        reg_GX_VRAMCNT_D = (u8)GX_VRAMCNT_D_ARM7_0x06020000;
        reg_GX_VRAMCNT_C = (u8)GX_VRAMCNT_C_ARM7_0x06000000;
        break;
    case GX_VRAM_ARM7_128_C:
        reg_GX_VRAMCNT_C = (u8)GX_VRAMCNT_C_ARM7_0x06000000;
        break;
    case GX_VRAM_ARM7_128_D:
        reg_GX_VRAMCNT_D = (u8)GX_VRAMCNT_D_ARM7_0x06000000;
        // don't break
    case GX_VRAM_ARM7_NONE:
        break;

    default:
        SDK_INTERNAL_ERROR("unknown GXVRamARM7, 0x%x specified.", arm7);
        break;
    }
}

static inline void texOn_()
{
    reg_G3X_DISP3DCNT = (u16)((reg_G3X_DISP3DCNT &
                               ~(REG_G3X_DISP3DCNT_RO_MASK | REG_G3X_DISP3DCNT_GO_MASK)) |
                              REG_G3X_DISP3DCNT_TME_MASK);
}

static inline void texOff_()
{
    reg_G3X_DISP3DCNT &= (u16)~(REG_G3X_DISP3DCNT_TME_MASK |
                                REG_G3X_DISP3DCNT_RO_MASK | REG_G3X_DISP3DCNT_GO_MASK);
}


/*---------------------------------------------------------------------------*
  Name:         GX_VRAMCNT_SetTEX_(internal use only)

  Description:  Assigns banks to texture image slots.
                The banks are mapped consecutively from the slot #0.
                This is called only by GX_SetBankForTex.

  Arguments:    tex       banks assigned to texture image slots

  Returns:      none
 *---------------------------------------------------------------------------*/
static inline void GX_VRAMCNT_SetTEX_(GXVRamTex tex)
{
    if (tex == GX_VRAM_TEX_NONE)
    {
        texOff_();
        return;
    }
    texOn_();

    switch (tex)
    {
    case GX_VRAM_TEX_01_AC:
        reg_GX_VRAMCNT_A = (u8)GX_VRAMCNT_A_TEX_0;
        reg_GX_VRAMCNT_C = (u8)GX_VRAMCNT_C_TEX_1;
        break;

    case GX_VRAM_TEX_01_AD:
        reg_GX_VRAMCNT_A = (u8)GX_VRAMCNT_A_TEX_0;
        reg_GX_VRAMCNT_D = (u8)GX_VRAMCNT_D_TEX_1;
        break;

    case GX_VRAM_TEX_01_BD:
        reg_GX_VRAMCNT_B = (u8)GX_VRAMCNT_B_TEX_0;
        reg_GX_VRAMCNT_D = (u8)GX_VRAMCNT_D_TEX_1;
        break;

    case GX_VRAM_TEX_012_ABD:
        reg_GX_VRAMCNT_A = (u8)GX_VRAMCNT_A_TEX_0;
        reg_GX_VRAMCNT_B = (u8)GX_VRAMCNT_B_TEX_1;
        reg_GX_VRAMCNT_D = (u8)GX_VRAMCNT_D_TEX_2;
        break;

    case GX_VRAM_TEX_012_ACD:
        reg_GX_VRAMCNT_A = (u8)GX_VRAMCNT_A_TEX_0;
        reg_GX_VRAMCNT_C = (u8)GX_VRAMCNT_C_TEX_1;
        reg_GX_VRAMCNT_D = (u8)GX_VRAMCNT_D_TEX_2;
        break;

    case GX_VRAM_TEX_0_D:
        reg_GX_VRAMCNT_D = (u8)GX_VRAMCNT_D_TEX_0;
        break;

    case GX_VRAM_TEX_01_CD:
        reg_GX_VRAMCNT_D = (u8)GX_VRAMCNT_D_TEX_1;
        // don't break
    case GX_VRAM_TEX_0_C:
        reg_GX_VRAMCNT_C = (u8)GX_VRAMCNT_C_TEX_0;
        break;

    case GX_VRAM_TEX_012_BCD:
        reg_GX_VRAMCNT_D = (u8)GX_VRAMCNT_D_TEX_2;
        // don't break
    case GX_VRAM_TEX_01_BC:
        reg_GX_VRAMCNT_C = (u8)GX_VRAMCNT_C_TEX_1;
        // don't break
    case GX_VRAM_TEX_0_B:
        reg_GX_VRAMCNT_B = (u8)GX_VRAMCNT_B_TEX_0;
        break;

    case GX_VRAM_TEX_0123_ABCD:
        reg_GX_VRAMCNT_D = (u8)GX_VRAMCNT_D_TEX_3;
        // don't break;
    case GX_VRAM_TEX_012_ABC:
        reg_GX_VRAMCNT_C = (u8)GX_VRAMCNT_C_TEX_2;
        // don't break;
    case GX_VRAM_TEX_01_AB:
        reg_GX_VRAMCNT_B = (u8)GX_VRAMCNT_B_TEX_1;
        // don't break;
    case GX_VRAM_TEX_0_A:
        reg_GX_VRAMCNT_A = (u8)GX_VRAMCNT_A_TEX_0;
        break;
    default:
        SDK_INTERNAL_ERROR("unknown GXVRamTex, 0x%x specified.", tex);
        break;
    }
}

static inline void clearImageOn_()
{
    reg_G3X_DISP3DCNT |= REG_G3X_DISP3DCNT_PRI_MASK;
}

static inline void clearImageOff_()
{
    reg_G3X_DISP3DCNT &= ~REG_G3X_DISP3DCNT_PRI_MASK;
}


/*---------------------------------------------------------------------------*
  Name:         GX_VRAMCNT_SetCLRIMG_(internal use only)

  Description:  Assigns banks onto ClearImage.
                The banks are mapped onto the texture
                image slot #2, #3(clear image slot).
                This is called only by GX_SetBankForClearImage.

  Arguments:    clrImg      banks assigned to CLRIMG

  Returns:      none
 *---------------------------------------------------------------------------*/
static inline void GX_VRAMCNT_SetCLRIMG_(GXVRamClearImage clrImg)
{
    switch (clrImg)
    {
    case GX_VRAM_CLEARIMAGE_256_AB:
        reg_GX_VRAMCNT_A = (u8)GX_VRAMCNT_A_TEX_2;
        // don't break
    case GX_VRAM_CLEARDEPTH_128_B:
        reg_GX_VRAMCNT_B = (u8)GX_VRAMCNT_B_TEX_3;
        clearImageOn_();
        break;

    case GX_VRAM_CLEARIMAGE_256_CD:
        reg_GX_VRAMCNT_C = (u8)GX_VRAMCNT_C_TEX_2;
        // don't break
    case GX_VRAM_CLEARDEPTH_128_D:
        reg_GX_VRAMCNT_D = (u8)GX_VRAMCNT_D_TEX_3;
        clearImageOn_();
        break;

    case GX_VRAM_CLEARIMAGE_NONE:
        clearImageOff_();
        break;

    case GX_VRAM_CLEARDEPTH_128_A:
        reg_GX_VRAMCNT_A = (u8)GX_VRAMCNT_A_TEX_3;
        clearImageOn_();
        break;

    case GX_VRAM_CLEARDEPTH_128_C:
        reg_GX_VRAMCNT_C = (u8)GX_VRAMCNT_C_TEX_3;
        clearImageOn_();
        break;

    default:
        SDK_INTERNAL_ERROR("unknown GXVRamClearImage, 0x%x specified.", clrImg);
        break;
    }
}



/*---------------------------------------------------------------------------*
  Name:         GX_VRAMCNT_SetTEXPLTT_(internal use only)

  Description:  Assigns banks to the texture palette slots.
                The banks are mapped consecutively from the slot #0.
                This is called only by GX_SetBankForTexPltt.

  Arguments:    texPltt       banks assigned to the texture palette slots

  Returns:      none
 *---------------------------------------------------------------------------*/
static inline void GX_VRAMCNT_SetTEXPLTT_(GXVRamTexPltt texPltt)
{
    switch (texPltt)
    {
    case GX_VRAM_TEXPLTT_01_FG:
        reg_GX_VRAMCNT_G = (u8)GX_VRAMCNT_G_TEXPLTT_1;
        // don't break
    case GX_VRAM_TEXPLTT_0_F:
        reg_GX_VRAMCNT_F = (u8)GX_VRAMCNT_F_TEXPLTT_0;
        break;

    case GX_VRAM_TEXPLTT_0_G:
        reg_GX_VRAMCNT_G = (u8)GX_VRAMCNT_G_TEXPLTT_0;
        break;

    case GX_VRAM_TEXPLTT_012345_EFG:
        reg_GX_VRAMCNT_G = (u8)GX_VRAMCNT_G_TEXPLTT_5;
        // don't break
    case GX_VRAM_TEXPLTT_01234_EF:
        reg_GX_VRAMCNT_F = (u8)GX_VRAMCNT_F_TEXPLTT_4;
        // don't break
    case GX_VRAM_TEXPLTT_0123_E:
        reg_GX_VRAMCNT_E = (u8)GX_VRAMCNT_E_TEXPLTT_0123;
        break;

    case GX_VRAM_TEXPLTT_NONE:
        break;

    default:
        SDK_INTERNAL_ERROR("unknown GXVRamTexPltt, 0x%x specified.", texPltt);
        break;
    }
}

static inline void bgExtPlttOn_()
{
    reg_GX_DISPCNT |= REG_GX_DISPCNT_BG_MASK;
}

static inline void bgExtPlttOff_()
{
    reg_GX_DISPCNT &= ~REG_GX_DISPCNT_BG_MASK;
}

/*---------------------------------------------------------------------------*
  Name:         GX_VRAMCNT_SetBGEXTPLTT_(internal use only)

  Description:  Assigns banks to BG extended palettes.
                The banks are mapped consecutively from the slot #0 or #2.
                This is called only by GX_SetBankForBGExtPltt.

  Arguments:    bgExtPltt     banks assigned to the BG extended palettes

  Returns:      none
 *---------------------------------------------------------------------------*/
static inline void GX_VRAMCNT_SetBGEXTPLTT_(GXVRamBGExtPltt bgExtPltt)
{
    switch (bgExtPltt)
    {
    case GX_VRAM_BGEXTPLTT_0123_E:
        bgExtPlttOn_();
        reg_GX_VRAMCNT_E = (u8)GX_VRAMCNT_E_BGEXTPLTT_0123;
        break;

    case GX_VRAM_BGEXTPLTT_23_G:
        bgExtPlttOn_();
        reg_GX_VRAMCNT_G = (u8)GX_VRAMCNT_G_BGEXTPLTT_23;
        break;

    case GX_VRAM_BGEXTPLTT_0123_FG:
        reg_GX_VRAMCNT_G = (u8)GX_VRAMCNT_G_BGEXTPLTT_23;
        // don't break
    case GX_VRAM_BGEXTPLTT_01_F:
        reg_GX_VRAMCNT_F = (u8)GX_VRAMCNT_F_BGEXTPLTT_01;
        bgExtPlttOn_();
        break;

    case GX_VRAM_BGEXTPLTT_NONE:
        bgExtPlttOff_();
        break;

    default:
        SDK_INTERNAL_ERROR("unknown GXVRamBGExtPltt, 0x%x specified.", bgExtPltt);
        break;
    }
}

static inline void objExtPlttOn_()
{
    reg_GX_DISPCNT |= REG_GX_DISPCNT_O_MASK;
}

static inline void objExtPlttOff_()
{
    reg_GX_DISPCNT &= ~REG_GX_DISPCNT_O_MASK;
}


/*---------------------------------------------------------------------------*
  Name:         GX_VRAMCNT_SetOBJEXTPLTT_(internal use only)

  Description:  Assigns a bank to OBJ extended palettes.
                This is called only by GX_SetBankForOBJExtPltt.

  Arguments:    objExtPltt     banks assigned to the OBJ extended palettes

  Returns:      none
 *---------------------------------------------------------------------------*/
static inline void GX_VRAMCNT_SetOBJEXTPLTT_(GXVRamOBJExtPltt objExtPltt)
{
    switch (objExtPltt)
    {
    case GX_VRAM_OBJEXTPLTT_0_F:
        objExtPlttOn_();
        reg_GX_VRAMCNT_F = (u8)GX_VRAMCNT_F_OBJEXTPLTT;
        break;

    case GX_VRAM_OBJEXTPLTT_0_G:
        objExtPlttOn_();
        reg_GX_VRAMCNT_G = (u8)GX_VRAMCNT_G_OBJEXTPLTT;
        break;

    case GX_VRAM_OBJEXTPLTT_NONE:
        objExtPlttOff_();
        break;

    default:
        SDK_INTERNAL_ERROR("unknown GXVRamOBJExtPltt, 0x%x specified.", objExtPltt);
        break;
    }
}


/*---------------------------------------------------------------------------*
  Name:         GX_VRAMCNT_SetSubBG_(internal use only)

  Description:  Assigns a bank to sub 2D engine's BG.
                This is called only by GX_SetBankForSubBG.

  Arguments:    bg     banks assigned to sub 2D engine's BG

  Returns:      none
 *---------------------------------------------------------------------------*/
static inline void GX_VRAMCNT_SetSubBG_(GXVRamSubBG bg)
{
    switch (bg)
    {
    case GX_VRAM_SUB_BG_128_C:
        reg_GX_VRAMCNT_C = GX_VRAMCNT_C_SUBBG_0x06200000;
        break;

    case GX_VRAM_SUB_BG_48_HI:
        reg_GX_VRAMCNT_I = GX_VRAMCNT_I_SUBBG_0x06208000;
        // don't break
    case GX_VRAM_SUB_BG_32_H:
        reg_GX_VRAMCNT_H = GX_VRAMCNT_H_SUBBG_0x06200000;
        break;

    case GX_VRAM_SUB_BG_NONE:
        break;

    default:
        SDK_INTERNAL_ERROR("unknown GXVRamSubBG, 0x%x specified.", bg);
        break;
    }
}


/*---------------------------------------------------------------------------*
  Name:         GX_VRAMCNT_SetSubOBJ_(internal use only)

  Description:  Assigns a bank to sub 2D engine's OBJ.
                This is called only by GX_SetBankForSubOBJ.

  Arguments:    obj     banks assigned to sub 2D engine's OBJ

  Returns:      none
 *---------------------------------------------------------------------------*/
static inline void GX_VRAMCNT_SetSubOBJ_(GXVRamSubOBJ obj)
{
    switch (obj)
    {
    case GX_VRAM_SUB_OBJ_128_D:
        reg_GX_VRAMCNT_D = GX_VRAMCNT_D_SUBOBJ_0x06600000;
        break;

    case GX_VRAM_SUB_OBJ_16_I:
        reg_GX_VRAMCNT_I = GX_VRAMCNT_I_SUBOBJ_0x06600000;
        break;

    case GX_VRAM_SUB_OBJ_NONE:
        break;

    default:
        SDK_INTERNAL_ERROR("unknown GXVRamSubOBJ, 0x%x specified.", obj);
        break;
    }
}

static inline void subBGExtPlttOn_()
{
    reg_GXS_DB_DISPCNT |= REG_GXS_DB_DISPCNT_BG_MASK;
}

static inline void subBGExtPlttOff_()
{
    reg_GXS_DB_DISPCNT &= ~REG_GXS_DB_DISPCNT_BG_MASK;
}


/*---------------------------------------------------------------------------*
  Name:         GX_VRAMCNT_SetSubBGExtPltt_(internal use only)

  Description:  Assigns a bank to sub 2D engine's BGExtPltt.
                This is called only by GX_SetBankForSubBGExtPltt.

  Arguments:    bgExtPltt     banks assigned to sub 2D engine's BGExtPltt

  Returns:      none
 *---------------------------------------------------------------------------*/
static inline void GX_VRAMCNT_SetSubBGExtPltt_(GXVRamSubBGExtPltt bgExtPltt)
{
    switch (bgExtPltt)
    {
    case GX_VRAM_SUB_BGEXTPLTT_0123_H:
        subBGExtPlttOn_();
        reg_GX_VRAMCNT_H = GX_VRAMCNT_H_SUBBGEXTPLTT_0123;
        break;

    case GX_VRAM_SUB_BGEXTPLTT_NONE:
        subBGExtPlttOff_();
        break;

    default:
        SDK_INTERNAL_ERROR("unknown GXVRamSubBGExtPltt, 0x%x specified.", bgExtPltt);
        break;
    }
}

static inline void subOBJExtPlttOn_()
{
    reg_GXS_DB_DISPCNT |= REG_GXS_DB_DISPCNT_O_MASK;
}

static inline void subOBJExtPlttOff_()
{
    reg_GXS_DB_DISPCNT &= ~REG_GXS_DB_DISPCNT_O_MASK;
}


/*---------------------------------------------------------------------------*
  Name:         GX_VRAMCNT_SetSubOBJExtPltt_(internal use only)

  Description:  Assigns a bank to sub 2D engine's OBJExtPltt.
                This is called only by GX_SetBankForSubOBJExtPltt.

  Arguments:    objExtPltt     banks assigned to sub 2D engine's OBJExtPltt

  Returns:      none
 *---------------------------------------------------------------------------*/
static inline void GX_VRAMCNT_SetSubOBJExtPltt_(GXVRamSubOBJExtPltt objExtPltt)
{
    switch (objExtPltt)
    {
    case GX_VRAM_SUB_OBJEXTPLTT_0_I:
        subOBJExtPlttOn_();
        reg_GX_VRAMCNT_I = GX_VRAMCNT_I_SUBOBJEXTPLTT;
        break;

    case GX_VRAM_SUB_OBJEXTPLTT_NONE:
        subOBJExtPlttOff_();
        break;
    }
}

extern vu16 GXi_VRamLockId;
/*---------------------------------------------------------------------------*
  Name:         GxCheckExclusive

  Description:  バンクを切り替えようとするVRAMに対して排他ロックをかけます。
                既に他のライブラリ等によって排他ロックされている場合は、本来
                は排他ロックが解除されるまで待つべきですが、ここではPanicします。

  Arguments:    vramMap -   排他ロックをかけるVRAM群のバンクIDの論理和。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void GxCheckExclusive(u16 vramMap)
{
    if (!OSi_TryLockVram(vramMap, GXi_VRamLockId))
    {
        OS_Panic("VRAM bank is locked by another library.\n");
    }
}

//----------------------------------------------------------------------------
// GX_SetBankForX:
//
// 1 Error checking of arg.
// 2 Sets the state for LCDC((LCDC | X) & ~arg).
// 3 Sets the state for X(arg).
// 4 Checks conflicts with other regions.
// 5 Write VRAMCNTs for LCDC.
// 6 Write VRAMCNTs for X.
//----------------------------------------------------------------------------


/*---------------------------------------------------------------------------*/
static inline void GxSetBankForBG(GXVRamBG bg)
{
    // Check parameter
    GX_VRAM_BG_ASSERT(bg);
    // Change BG banks and LCDC ones.
    gGXState.vramCnt.lcdc = (u16)(~bg & (gGXState.vramCnt.lcdc | gGXState.vramCnt.bg));
    gGXState.vramCnt.bg = bg;
    // Check conflicts( DEBUG build ).
    GX_StateCheck_VRAMCnt();
    // Switch VRAM banks.
    GX_VRAMCNT_SetBG_(bg);
    GX_VRAMCNT_SetLCDC_(gGXState.vramCnt.lcdc);
}

/*---------------------------------------------------------------------------*
  Name:         GX_SetBankForBG

  Description:  Assigns banks onto BG.
                The specified banks must be disabled or mapped to LCDC.

  Arguments:    bg         banks to be assigned to BG

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_SetBankForBG(GXVRamBG bg)
{
#ifndef SDK_FINALROM
    GxCheckExclusive((u16)bg);
#endif
    GxSetBankForBG(bg);
}

/*---------------------------------------------------------------------------*
  Name:         GX_TrySetBankForBG

  Description:  Try to assign banks onto BG.
                The specified banks must be disabled or mapped to LCDC.

  Arguments:    bg         banks to be assigned to BG

  Returns:      BOOL       If could not lock VRAM banks , return FALSE.
 *---------------------------------------------------------------------------*/
BOOL GX_TrySetBankForBG(GXVRamBG bg)
{
    if (!OSi_TryLockVram((u16)bg, GXi_VRamLockId))
    {
        return FALSE;
    }
    GxSetBankForBG(bg);
    return TRUE;
}

/*---------------------------------------------------------------------------*/
/* 
 * inline化して少しでも速度の向上を図りたいが、この関数をinline宣言すると
 * CWコンパイラが内部の 3つの別inline関数を実体関数化してしまい、よりいっそう
 * 関数コールのオーバーヘッドが増加してしまう結果となる。
 * よって 2005/02/15 現在、当関数を実関数として定義しておく。
 */
static void GxSetBankForBGEx(GXVRamBG bg1, GXVRamBG bg2)
{
    // Check parameter validity
    GX_VRAM_BG_ASSERT_EX_1(bg1);
    GX_VRAM_BG_ASSERT_EX_2(bg2);
    // Change BG banks and LCDC ones.
    gGXState.vramCnt.lcdc = (u16)(~(bg1 | bg2) & (gGXState.vramCnt.lcdc | gGXState.vramCnt.bg));
    gGXState.vramCnt.bg = (u16)(bg1 | bg2);
    // Check conflicts( DEBUG build ).
    GX_StateCheck_VRAMCnt();
    // Switch VRAM banks
    GX_VRAMCNT_SetBGEx1_(bg1);
    GX_VRAMCNT_SetBGEx2_(bg2);
    GX_VRAMCNT_SetLCDC_(gGXState.vramCnt.lcdc);
}

/*---------------------------------------------------------------------------*
  Name:         GX_SetBankForBGEx

  Description:  Assigns banks onto BG.
                The specified banks must be disabled or mapped to LCDC.

  Arguments:    bg1         banks to be assigned to BG ( only bank EFG )
                bg2         banks to be assigned to BG ( only bank ABCD )

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_SetBankForBGEx(GXVRamBG bg1, GXVRamBG bg2)
{
#ifndef SDK_FINALROM
    GxCheckExclusive((u16)(bg1 | bg2));
#endif
    GxSetBankForBGEx(bg1, bg2);
}

/*---------------------------------------------------------------------------*
  Name:         GX_TrySetBankForBGEx

  Description:  Try to assign banks onto BG.
                The specified banks must be disabled or mapped to LCDC.

  Arguments:    bg1         banks to be assigned to BG ( only bank EFG )
                bg2         banks to be assigned to BG ( only bank ABCD )

  Returns:      BOOL        If could not lock VRAM banks , return FALSE.
 *---------------------------------------------------------------------------*/
BOOL GX_TrySetBankForBGEx(GXVRamBG bg1, GXVRamBG bg2)
{
    if (!OSi_TryLockVram((u16)(bg1 | bg2), GXi_VRamLockId))
    {
        return FALSE;
    }
    GxSetBankForBGEx(bg1, bg2);
    return TRUE;
}

/*---------------------------------------------------------------------------*/
static inline void GxSetBankForOBJ(GXVRamOBJ obj)
{
    // Check parameter validity
    GX_VRAM_OBJ_ASSERT(obj);
    // Change BG banks and LCDC ones.
    gGXState.vramCnt.lcdc = (u16)(~obj & (gGXState.vramCnt.lcdc | gGXState.vramCnt.obj));
    gGXState.vramCnt.obj = obj;
    // Check conflicts( DEBUG build ).
    GX_StateCheck_VRAMCnt();
    // Switch VRAM banks
    GX_VRAMCNT_SetOBJ_(obj);
    GX_VRAMCNT_SetLCDC_(gGXState.vramCnt.lcdc);
}

/*---------------------------------------------------------------------------*
  Name:         GX_SetBankForOBJ

  Description:  Assigns banks onto OBJ.
                The specified banks must be disabled or mapped to LCDC.

  Arguments:    obj         banks to be assigned to OBJ

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_SetBankForOBJ(GXVRamOBJ obj)
{
#ifndef SDK_FINALROM
    GxCheckExclusive((u16)obj);
#endif
    GxSetBankForOBJ(obj);
}

/*---------------------------------------------------------------------------*
  Name:         GX_TrySetBankForOBJ

  Description:  Try to assign banks onto OBJ.
                The specified banks must be disabled or mapped to LCDC.

  Arguments:    obj         banks to be assigned to OBJ

  Returns:      BOOL        If could not lock VRAM banks , return FALSE.
 *---------------------------------------------------------------------------*/
BOOL GX_TrySetBankForOBJ(GXVRamOBJ obj)
{
    if (!OSi_TryLockVram((u16)obj, GXi_VRamLockId))
    {
        return FALSE;
    }
    GxSetBankForOBJ(obj);
    return TRUE;
}

/*---------------------------------------------------------------------------*/
static inline void GxSetBankForBGExtPltt(GXVRamBGExtPltt bgExtPltt)
{
    // Check parameter validity
    GX_VRAM_BGEXTPLTT_ASSERT(bgExtPltt);
    // Change BG banks and LCDC ones.
    gGXState.vramCnt.lcdc =
        (u16)(~bgExtPltt & (gGXState.vramCnt.lcdc | gGXState.vramCnt.bgExtPltt));
    gGXState.vramCnt.bgExtPltt = bgExtPltt;
    // Check conflicts( DEBUG build ).
    GX_StateCheck_VRAMCnt();
    // Switch VRAM banks
    GX_VRAMCNT_SetBGEXTPLTT_(bgExtPltt);
    GX_VRAMCNT_SetLCDC_(gGXState.vramCnt.lcdc);
}

/*---------------------------------------------------------------------------*
  Name:         GX_SetBankForBGExtPltt

  Description:  Assigns banks onto BGExtPltt.
                The specified banks must be disabled or mapped to LCDC.

  Arguments:    bgExtPltt         banks to be assigned to bgExtPltt

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_SetBankForBGExtPltt(GXVRamBGExtPltt bgExtPltt)
{
#ifndef SDK_FINALROM
    GxCheckExclusive((u16)bgExtPltt);
#endif
    GxSetBankForBGExtPltt(bgExtPltt);
}

/*---------------------------------------------------------------------------*
  Name:         GX_TrySetBankForBGExtPltt

  Description:  Try to assign banks onto BGExtPltt.
                The specified banks must be disabled or mapped to LCDC.

  Arguments:    bgExtPltt   banks to be assigned to bgExtPltt

  Returns:      BOOL        If could not lock VRAM banks , return FALSE.
 *---------------------------------------------------------------------------*/
BOOL GX_TrySetBankForBGExtPltt(GXVRamBGExtPltt bgExtPltt)
{
    if (!OSi_TryLockVram((u16)bgExtPltt, GXi_VRamLockId))
    {
        return FALSE;
    }
    GxSetBankForBGExtPltt(bgExtPltt);
    return TRUE;
}

/*---------------------------------------------------------------------------*/
static inline void GxSetBankForOBJExtPltt(GXVRamOBJExtPltt objExtPltt)
{
    // Check parameter validity
    GX_VRAM_OBJEXTPLTT_ASSERT(objExtPltt);
    // Change BG banks and LCDC ones.
    gGXState.vramCnt.lcdc =
        (u16)(~objExtPltt & (gGXState.vramCnt.lcdc | gGXState.vramCnt.objExtPltt));
    gGXState.vramCnt.objExtPltt = objExtPltt;
    // Check conflicts( DEBUG build ).
    GX_StateCheck_VRAMCnt();
    // Switch VRAM banks
    GX_VRAMCNT_SetOBJEXTPLTT_(objExtPltt);
    GX_VRAMCNT_SetLCDC_(gGXState.vramCnt.lcdc);
}

/*---------------------------------------------------------------------------*
  Name:         GX_SetBankForOBJExtPltt

  Description:  Assigns banks onto OBJExtPltt.
                The specified banks must be disabled or mapped to LCDC.

  Arguments:    objExtPltt         banks to be assigned to objExtPltt

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_SetBankForOBJExtPltt(GXVRamOBJExtPltt objExtPltt)
{
#ifndef SDK_FINALROM
    GxCheckExclusive((u16)objExtPltt);
#endif
    GxSetBankForOBJExtPltt(objExtPltt);
}

/*---------------------------------------------------------------------------*
  Name:         GX_TrySetBankForOBJExtPltt

  Description:  Try to assign banks onto OBJExtPltt.
                The specified banks must be disabled or mapped to LCDC.

  Arguments:    objExtPltt  banks to be assigned to objExtPltt

  Returns:      BOOL        If could not lock VRAM banks , return FALSE.
 *---------------------------------------------------------------------------*/
BOOL GX_TrySetBankForOBJExtPltt(GXVRamOBJExtPltt objExtPltt)
{
    if (!OSi_TryLockVram((u16)objExtPltt, GXi_VRamLockId))
    {
        return FALSE;
    }
    GxSetBankForOBJExtPltt(objExtPltt);
    return TRUE;
}

/*---------------------------------------------------------------------------*/
static inline void GxSetBankForTex(GXVRamTex tex)
{
    // Check parameter validity
    GX_VRAM_TEX_ASSERT(tex);
    // Change BG banks and LCDC ones.
    gGXState.vramCnt.lcdc = (u16)(~tex & (gGXState.vramCnt.lcdc | gGXState.vramCnt.tex));
    gGXState.vramCnt.tex = tex;
    // Check conflicts( DEBUG build ).
    GX_StateCheck_VRAMCnt();
    // Switch VRAM banks
    GX_VRAMCNT_SetTEX_(tex);
    GX_VRAMCNT_SetLCDC_(gGXState.vramCnt.lcdc);
}

/*---------------------------------------------------------------------------*
  Name:         GX_SetBankForTex

  Description:  Assigns banks onto texture image slots.
                The specified banks must be disabled or mapped to LCDC.

  Arguments:    tex         banks to be assigned to texture image slots

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_SetBankForTex(GXVRamTex tex)
{
#ifndef SDK_FINALROM
    GxCheckExclusive((u16)tex);
#endif
    GxSetBankForTex(tex);
}

/*---------------------------------------------------------------------------*
  Name:         GX_TrySetBankForTex

  Description:  Try to assign banks onto texture image slots.
                The specified banks must be disabled or mapped to LCDC.

  Arguments:    tex         banks to be assigned to texture image slots

  Returns:      BOOL        If could not lock VRAM banks , return FALSE.
 *---------------------------------------------------------------------------*/
BOOL GX_TrySetBankForTex(GXVRamTex tex)
{
    if (!OSi_TryLockVram((u16)tex, GXi_VRamLockId))
    {
        return FALSE;
    }
    GxSetBankForTex(tex);
    return TRUE;
}

/*---------------------------------------------------------------------------*/
static inline void GxSetBankForTexPltt(GXVRamTexPltt texPltt)
{
    // Check parameter validity
    GX_VRAM_TEXPLTT_ASSERT(texPltt);
    // Change BG banks and LCDC ones.
    gGXState.vramCnt.lcdc = (u16)(~texPltt & (gGXState.vramCnt.lcdc | gGXState.vramCnt.texPltt));
    gGXState.vramCnt.texPltt = texPltt;
    // Check conflicts( DEBUG build ).
    GX_StateCheck_VRAMCnt();
    // Switch VRAM banks
    GX_VRAMCNT_SetTEXPLTT_(texPltt);
    GX_VRAMCNT_SetLCDC_(gGXState.vramCnt.lcdc);
}

/*---------------------------------------------------------------------------*
  Name:         GX_SetBankForTexPltt

  Description:  Assigns banks onto texture palette slots.
                The specified banks must be disabled or mapped to LCDC.

  Arguments:    texPltt     banks to be assigned to texture palette slots

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_SetBankForTexPltt(GXVRamTexPltt texPltt)
{
#ifndef SDK_FINALROM
    GxCheckExclusive((u16)texPltt);
#endif
    GxSetBankForTexPltt(texPltt);
}

/*---------------------------------------------------------------------------*
  Name:         GX_TrySetBankForTexPltt

  Description:  Try to assign banks onto texture palette slots.
                The specified banks must be disabled or mapped to LCDC.

  Arguments:    texPltt     banks to be assigned to texture palette slots

  Returns:      BOOL        If could not lock VRAM banks , return FALSE.
 *---------------------------------------------------------------------------*/
BOOL GX_TrySetBankForTexPltt(GXVRamTexPltt texPltt)
{
    if (!OSi_TryLockVram((u16)texPltt, GXi_VRamLockId))
    {
        return FALSE;
    }
    GxSetBankForTexPltt(texPltt);
    return TRUE;
}

/*---------------------------------------------------------------------------*/
static inline void GxSetBankForClearImage(GXVRamClearImage clrImg)
{
    // Check parameter validity
    GX_VRAM_CLRIMG_ASSERT(clrImg);
    // Change BG banks and LCDC ones.
    gGXState.vramCnt.lcdc = (u16)(~clrImg & (gGXState.vramCnt.lcdc | gGXState.vramCnt.clrImg));
    gGXState.vramCnt.clrImg = clrImg;
    // Check conflicts( DEBUG build ).
    GX_StateCheck_VRAMCnt();
    // Switch VRAM banks
    GX_VRAMCNT_SetCLRIMG_(clrImg);
    GX_VRAMCNT_SetLCDC_(gGXState.vramCnt.lcdc);
}

/*---------------------------------------------------------------------------*
  Name:         GX_SetBankForClearImage

  Description:  Assigns banks onto clear image slots(texture image slot #2,#3).
                The specified banks must be disabled or mapped to LCDC.

  Arguments:    clrImg       banks to be assigned to clear image slots

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_SetBankForClearImage(GXVRamClearImage clrImg)
{
#ifndef SDK_FINALROM
    GxCheckExclusive((u16)clrImg);
#endif
    GxSetBankForClearImage(clrImg);
}

/*---------------------------------------------------------------------------*
  Name:         GX_TrySetBankForClearImage

  Description:  Try to assign banks onto clear image slots(texture image slot #2,#3).
                The specified banks must be disabled or mapped to LCDC.

  Arguments:    clrImg      banks to be assigned to clear image slots

  Returns:      BOOL        If could not lock VRAM banks , return FALSE.
 *---------------------------------------------------------------------------*/
BOOL GX_TrySetBankForClearImage(GXVRamClearImage clrImg)
{
    if (!OSi_TryLockVram((u16)clrImg, GXi_VRamLockId))
    {
        return FALSE;
    }
    GxSetBankForClearImage(clrImg);
    return TRUE;
}

/*---------------------------------------------------------------------------*/
static inline void GxSetBankForARM7(GXVRamARM7 arm7)
{
    // Check parameter validity
    GX_VRAM_ARM7_ASSERT(arm7);
    // Change BG banks and LCDC ones.
    gGXState.vramCnt.lcdc = (u16)(~arm7 & (gGXState.vramCnt.lcdc | gGXState.vramCnt.arm7));
    gGXState.vramCnt.arm7 = arm7;
    // Check conflicts( DEBUG build ).
    GX_StateCheck_VRAMCnt();
    // Switch VRAM banks
    GX_VRAMCNT_SetARM7_(arm7);
    GX_VRAMCNT_SetLCDC_(gGXState.vramCnt.lcdc);
}

/*---------------------------------------------------------------------------*
  Name:         GX_SetBankForARM7

  Description:  Assigns banks onto ARM7 memory space.
                The specified banks must be disabled or mapped to LCDC.

  Arguments:    arm7     banks to be assigned to ARM7 memory space

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_SetBankForARM7(GXVRamARM7 arm7)
{
#ifndef SDK_FINALROM
    GxCheckExclusive((u16)arm7);
#endif
    GxSetBankForARM7(arm7);
}

/*---------------------------------------------------------------------------*
  Name:         GX_TrySetBankForARM7

  Description:  Try to assign banks onto ARM7 memory space.
                The specified banks must be disabled or mapped to LCDC.

  Arguments:    arm7        banks to be assigned to ARM7 memory space

  Returns:      BOOL        If could not lock VRAM banks , return FALSE.
 *---------------------------------------------------------------------------*/
BOOL GX_TrySetBankForARM7(GXVRamARM7 arm7)
{
    if (!OSi_TryLockVram((u16)arm7, GXi_VRamLockId))
    {
        return FALSE;
    }
    GxSetBankForARM7(arm7);
    return TRUE;
}

/*---------------------------------------------------------------------------*/
static inline void GxSetBankForLCDC(int lcdc)
{
    // Check parameter validity
    GX_VRAM_LCDC_ASSERT(lcdc);
    // Change BG banks and LCDC ones.
    gGXState.vramCnt.lcdc |= lcdc;
    // Check conflicts( DEBUG build ).
    GX_StateCheck_VRAMCnt();
    // Switch VRAM banks
    GX_VRAMCNT_SetLCDC_(lcdc);
}

/*---------------------------------------------------------------------------*
  Name:         GX_SetBankForLCDC

  Description:  Assigns banks onto LCDC memory space.
                The specified banks must be disabled or mapped to LCDC.

  Arguments:    lcdc     banks to be assigned to LCDC memory space

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_SetBankForLCDC(int lcdc)
{
#ifndef SDK_FINALROM
    GxCheckExclusive((u16)lcdc);
#endif
    GxSetBankForLCDC(lcdc);
}

/*---------------------------------------------------------------------------*
  Name:         GX_TrySetBankForLCDC

  Description:  Try to assign banks onto LCDC memory space.
                The specified banks must be disabled or mapped to LCDC.

  Arguments:    lcdc        banks to be assigned to LCDC memory space

  Returns:      BOOL        If could not lock VRAM banks , return FALSE.
 *---------------------------------------------------------------------------*/
BOOL GX_TrySetBankForLCDC(int lcdc)
{
    if (!OSi_TryLockVram((u16)lcdc, GXi_VRamLockId))
    {
        return FALSE;
    }
    GxSetBankForLCDC(lcdc);
    return TRUE;
}

/*---------------------------------------------------------------------------*/
static inline void GxSetBankForSubBG(GXVRamSubBG sub_bg)
{
    // Check parameter validity
    GX_VRAM_SUB_BG_ASSERT(sub_bg);
    // Change BG banks and LCDC ones.
    gGXState.vramCnt.lcdc = (u16)(~sub_bg & (gGXState.vramCnt.lcdc | gGXState.vramCnt.sub_bg));
    gGXState.vramCnt.sub_bg = sub_bg;
    // Check conflicts( DEBUG build ).
    GX_StateCheck_VRAMCnt();
    // Switch VRAM banks
    GX_VRAMCNT_SetSubBG_(sub_bg);
    GX_VRAMCNT_SetLCDC_(gGXState.vramCnt.lcdc);
}

/*---------------------------------------------------------------------------*
  Name:         GX_SetBankForSubBG

  Description:  Assigns banks onto sub engine's BG.
                The specified banks must be disabled or mapped to LCDC.

  Arguments:    sub_bg     banks to be assigned to sub engine's BG

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_SetBankForSubBG(GXVRamSubBG sub_bg)
{
#ifndef SDK_FINALROM
    GxCheckExclusive((u16)sub_bg);
#endif
    GxSetBankForSubBG(sub_bg);
}

/*---------------------------------------------------------------------------*
  Name:         GX_TrySetBankForSubBG

  Description:  Try to assign banks onto sub engine's BG.
                The specified banks must be disabled or mapped to LCDC.

  Arguments:    sub_bg      banks to be assigned to sub engine's BG

  Returns:      BOOL        If could not lock VRAM banks , return FALSE.
 *---------------------------------------------------------------------------*/
BOOL GX_TrySetBankForSubBG(GXVRamSubBG sub_bg)
{
    if (!OSi_TryLockVram((u16)sub_bg, GXi_VRamLockId))
    {
        return FALSE;
    }
    GxSetBankForSubBG(sub_bg);
    return TRUE;
}

/*---------------------------------------------------------------------------*/
static inline void GxSetBankForSubOBJ(GXVRamSubOBJ sub_obj)
{
    // Check parameter validity
    GX_VRAM_SUB_OBJ_ASSERT(sub_obj);
    // Change BG banks and LCDC ones.
    gGXState.vramCnt.lcdc = (u16)(~sub_obj & (gGXState.vramCnt.lcdc | gGXState.vramCnt.sub_obj));
    gGXState.vramCnt.sub_obj = sub_obj;
    // Check conflicts( DEBUG build ).
    GX_StateCheck_VRAMCnt();
    // Switch VRAM banks
    GX_VRAMCNT_SetSubOBJ_(sub_obj);
    GX_VRAMCNT_SetLCDC_(gGXState.vramCnt.lcdc);
}

/*---------------------------------------------------------------------------*
  Name:         GX_SetBankForSubOBJ

  Description:  Assigns banks onto sub engine's OBJ.
                The specified banks must be disabled or mapped to LCDC.

  Arguments:    sub_obj     banks to be assigned to sub engine's OBJ

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_SetBankForSubOBJ(GXVRamSubOBJ sub_obj)
{
#ifndef SDK_FINALROM
    GxCheckExclusive((u16)sub_obj);
#endif
    GxSetBankForSubOBJ(sub_obj);
}

/*---------------------------------------------------------------------------*
  Name:         GX_TrySetBankForSubOBJ

  Description:  Try to assign banks onto sub engine's OBJ.
                The specified banks must be disabled or mapped to LCDC.

  Arguments:    sub_obj     banks to be assigned to sub engine's OBJ

  Returns:      BOOL        If could not lock VRAM banks , return FALSE.
 *---------------------------------------------------------------------------*/
BOOL GX_TrySetBankForSubOBJ(GXVRamSubOBJ sub_obj)
{
    if (!OSi_TryLockVram((u16)sub_obj, GXi_VRamLockId))
    {
        return FALSE;
    }
    GxSetBankForSubOBJ(sub_obj);
    return TRUE;
}

/*---------------------------------------------------------------------------*/
static inline void GxSetBankForSubBGExtPltt(GXVRamSubBGExtPltt sub_bgExtPltt)
{
    // Check parameter validity
    GX_VRAM_SUB_BGEXTPLTT_ASSERT(sub_bgExtPltt);
    // Change BG banks and LCDC ones.
    gGXState.vramCnt.lcdc =
        (u16)(~sub_bgExtPltt & (gGXState.vramCnt.lcdc | gGXState.vramCnt.sub_bgExtPltt));
    gGXState.vramCnt.sub_bgExtPltt = sub_bgExtPltt;
    // Check conflicts( DEBUG build ).
    GX_StateCheck_VRAMCnt();
    // Switch VRAM banks
    GX_VRAMCNT_SetSubBGExtPltt_(sub_bgExtPltt);
    GX_VRAMCNT_SetLCDC_(gGXState.vramCnt.lcdc);
}

/*---------------------------------------------------------------------------*
  Name:         GX_SetBankForSubBGExtPltt

  Description:  Assigns banks onto sub engine's BGExtPltt.
                The specified banks must be disabled or mapped to LCDC.

  Arguments:    sub_bgExtPltt     banks to be assigned to sub engine's BGExtPltt

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_SetBankForSubBGExtPltt(GXVRamSubBGExtPltt sub_bgExtPltt)
{
#ifndef SDK_FINALROM
    GxCheckExclusive((u16)sub_bgExtPltt);
#endif
    GxSetBankForSubBGExtPltt(sub_bgExtPltt);
}

/*---------------------------------------------------------------------------*
  Name:         GX_TrySetBankForSubBGExtPltt

  Description:  Try to assign banks onto sub engine's BGExtPltt.
                The specified banks must be disabled or mapped to LCDC.

  Arguments:    sub_bgExtPltt   banks to be assigned to sub engine's BGExtPltt

  Returns:      BOOL        If could not lock VRAM banks , return FALSE.
 *---------------------------------------------------------------------------*/
BOOL GX_TrySetBankForSubBGExtPltt(GXVRamSubBGExtPltt sub_bgExtPltt)
{
    if (!OSi_TryLockVram((u16)sub_bgExtPltt, GXi_VRamLockId))
    {
        return FALSE;
    }
    GxSetBankForSubBGExtPltt(sub_bgExtPltt);
    return TRUE;
}

/*---------------------------------------------------------------------------*/
static inline void GxSetBankForSubOBJExtPltt(GXVRamSubOBJExtPltt sub_objExtPltt)
{
    // Check parameter validity
    GX_VRAM_SUB_OBJEXTPLTT_ASSERT(sub_objExtPltt);
    // Change BG banks and LCDC ones.
    gGXState.vramCnt.lcdc =
        (u16)(~sub_objExtPltt & (gGXState.vramCnt.lcdc | gGXState.vramCnt.sub_objExtPltt));
    gGXState.vramCnt.sub_objExtPltt = sub_objExtPltt;
    // Check conflicts( DEBUG build ).
    GX_StateCheck_VRAMCnt();
    // Switch VRAM banks
    GX_VRAMCNT_SetSubOBJExtPltt_(sub_objExtPltt);
    GX_VRAMCNT_SetLCDC_(gGXState.vramCnt.lcdc);
}

/*---------------------------------------------------------------------------*
  Name:         GX_SetBankForSubOBJExtPltt

  Description:  Assigns banks onto sub engine's OBJExtPltt.
                The specified banks must be disabled or mapped to LCDC.

  Arguments:    sub_objExtPltt     banks to be assigned to sub engine's OBJExtPltt

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_SetBankForSubOBJExtPltt(GXVRamSubOBJExtPltt sub_objExtPltt)
{
#ifndef SDK_FINALROM
    GxCheckExclusive((u16)sub_objExtPltt);
#endif
    GxSetBankForSubOBJExtPltt(sub_objExtPltt);
}

/*---------------------------------------------------------------------------*
  Name:         GX_TrySetBankForSubOBJExtPltt

  Description:  Try to assign banks onto sub engine's OBJExtPltt.
                The specified banks must be disabled or mapped to LCDC.

  Arguments:    sub_objExtPltt    banks to be assigned to sub engine's OBJExtPltt

  Returns:      BOOL        If could not lock VRAM banks , return FALSE.
 *---------------------------------------------------------------------------*/
BOOL GX_TrySetBankForSubOBJExtPltt(GXVRamSubOBJExtPltt sub_objExtPltt)
{
    if (!OSi_TryLockVram((u16)sub_objExtPltt, GXi_VRamLockId))
    {
        return FALSE;
    }
    GxSetBankForSubOBJExtPltt(sub_objExtPltt);
    return TRUE;
}

#ifdef  SDK_CW_WARNOFF_SAFESTRB
#include <nitro/codereset.h>
#endif

/*---------------------------------------------------------------------------*
  Name:         resetBankForX_

  Description:  Assigns the argument banks onto LCDC memory space.

  Arguments:    g3bit    a pointer to bank information

  Returns:      none
 *---------------------------------------------------------------------------*/
static int resetBankForX_(u16 *g3bit)
{
    int     rval = *g3bit;
    *g3bit = 0;                        // GX_VRAMCNT_xxxxxx_NONE

    // maps to LCDC
    gGXState.vramCnt.lcdc |= rval;
    GX_VRAMCNT_SetLCDC_(rval);

    return rval;
}


/*---------------------------------------------------------------------------*
  Name:         GX_ResetBankForBG

  Description:  Switches BG banks onto LCDC memory space.

  Arguments:    none

  Returns:      the banks that were assigned to BG
 *---------------------------------------------------------------------------*/
GXVRamBG GX_ResetBankForBG()
{
#ifndef SDK_FINALROM
    if (!OSi_TryLockVram((u16)gGXState.vramCnt.bg, GXi_VRamLockId))
    {
        return GX_VRAM_BG_NONE;
    }
#endif
    return (GXVRamBG)resetBankForX_(&gGXState.vramCnt.bg);
}


/*---------------------------------------------------------------------------*
  Name:         GX_ResetBankForOBJ

  Description:  Switches OBJ banks onto LCDC memory space.

  Arguments:    none

  Returns:      the banks that were assigned to OBJ
 *---------------------------------------------------------------------------*/
GXVRamOBJ GX_ResetBankForOBJ()
{
#ifndef SDK_FINALROM
    if (!OSi_TryLockVram((u16)gGXState.vramCnt.obj, GXi_VRamLockId))
    {
        return GX_VRAM_OBJ_NONE;
    }
#endif
    return (GXVRamOBJ)resetBankForX_(&gGXState.vramCnt.obj);
}


/*---------------------------------------------------------------------------*
  Name:         GX_ResetBankForBGExtPltt

  Description:  Switches BGExtPltt banks onto LCDC memory space.

  Arguments:    none

  Returns:      the banks that were assigned to BGExtPltt
 *---------------------------------------------------------------------------*/
GXVRamBGExtPltt GX_ResetBankForBGExtPltt()
{
#ifndef SDK_FINALROM
    if (!OSi_TryLockVram((u16)gGXState.vramCnt.bgExtPltt, GXi_VRamLockId))
    {
        return GX_VRAM_BGEXTPLTT_NONE;
    }
#endif
    bgExtPlttOff_();
    return (GXVRamBGExtPltt)resetBankForX_(&gGXState.vramCnt.bgExtPltt);
}


/*---------------------------------------------------------------------------*
  Name:         GX_ResetBankForOBJExtPltt

  Description:  Switches OBJExtPltt banks onto LCDC memory space.

  Arguments:    none

  Returns:      the banks that were assigned to OBJExtPltt
 *---------------------------------------------------------------------------*/
GXVRamOBJExtPltt GX_ResetBankForOBJExtPltt()
{
#ifndef SDK_FINALROM
    if (!OSi_TryLockVram((u16)gGXState.vramCnt.objExtPltt, GXi_VRamLockId))
    {
        return GX_VRAM_OBJEXTPLTT_NONE;
    }
#endif
    objExtPlttOff_();
    return (GXVRamOBJExtPltt)resetBankForX_(&gGXState.vramCnt.objExtPltt);
}


/*---------------------------------------------------------------------------*
  Name:         GX_ResetBankForTex

  Description:  Switches TexImage banks onto LCDC memory space.

  Arguments:    none

  Returns:      the banks that were assigned to TexImage
 *---------------------------------------------------------------------------*/
GXVRamTex GX_ResetBankForTex()
{
#ifndef SDK_FINALROM
    if (!OSi_TryLockVram((u16)gGXState.vramCnt.tex, GXi_VRamLockId))
    {
        return GX_VRAM_TEX_NONE;
    }
#endif
    return (GXVRamTex)resetBankForX_(&gGXState.vramCnt.tex);
}


/*---------------------------------------------------------------------------*
  Name:         GX_ResetBankForTexPltt

  Description:  Switches TexPltt banks onto LCDC memory space.

  Arguments:    none

  Returns:      the banks that were assigned to TexPltt
 *---------------------------------------------------------------------------*/
GXVRamTexPltt GX_ResetBankForTexPltt()
{
#ifndef SDK_FINALROM
    if (!OSi_TryLockVram((u16)gGXState.vramCnt.texPltt, GXi_VRamLockId))
    {
        return GX_VRAM_TEXPLTT_NONE;
    }
#endif
    return (GXVRamTexPltt)resetBankForX_(&gGXState.vramCnt.texPltt);
}


/*---------------------------------------------------------------------------*
  Name:         GX_ResetBankForClearImage

  Description:  Switches ClearImage banks onto LCDC memory space.

  Arguments:    none

  Returns:      the banks that were assigned to ClearImage
 *---------------------------------------------------------------------------*/
GXVRamClearImage GX_ResetBankForClearImage()
{
#ifndef SDK_FINALROM
    if (!OSi_TryLockVram((u16)gGXState.vramCnt.clrImg, GXi_VRamLockId))
    {
        return GX_VRAM_CLEARIMAGE_NONE;
    }
#endif
    return (GXVRamClearImage)resetBankForX_(&gGXState.vramCnt.clrImg);
}


/*---------------------------------------------------------------------------*
  Name:         GX_ResetBankForARM7

  Description:  Switches ARM7 banks onto LCDC memory space.

  Arguments:    none

  Returns:      the banks that were assigned to ARM7
 *---------------------------------------------------------------------------*/
GXVRamARM7 GX_ResetBankForARM7()
{
#ifndef SDK_FINALROM
    if (!OSi_TryLockVram((u16)gGXState.vramCnt.arm7, GXi_VRamLockId))
    {
        return GX_VRAM_ARM7_NONE;
    }
#endif
    return (GXVRamARM7)resetBankForX_(&gGXState.vramCnt.arm7);
}


/*---------------------------------------------------------------------------*
  Name:         GX_ResetBankForSubBG

  Description:  Switches sub engine's BG banks onto LCDC memory space.

  Arguments:    none

  Returns:      the banks that were assigned to sub engine's BG
 *---------------------------------------------------------------------------*/
GXVRamSubBG GX_ResetBankForSubBG()
{
#ifndef SDK_FINALROM
    if (!OSi_TryLockVram((u16)gGXState.vramCnt.sub_bg, GXi_VRamLockId))
    {
        return GX_VRAM_SUB_BG_NONE;
    }
#endif
    return (GXVRamSubBG)resetBankForX_(&gGXState.vramCnt.sub_bg);
}


/*---------------------------------------------------------------------------*
  Name:         GX_ResetBankForSubOBJ

  Description:  Switches sub engine's OBJ banks onto LCDC memory space.

  Arguments:    none

  Returns:      the banks that were assigned to sub engine's OBJ
 *---------------------------------------------------------------------------*/
GXVRamSubOBJ GX_ResetBankForSubOBJ()
{
#ifndef SDK_FINALROM
    if (!OSi_TryLockVram((u16)gGXState.vramCnt.sub_obj, GXi_VRamLockId))
    {
        return GX_VRAM_SUB_OBJ_NONE;
    }
#endif
    return (GXVRamSubOBJ)resetBankForX_(&gGXState.vramCnt.sub_obj);
}


/*---------------------------------------------------------------------------*
  Name:         GX_ResetBankForSubBGExtPltt

  Description:  Switches sub engine's BGExtPltt banks onto LCDC memory space.

  Arguments:    none

  Returns:      the banks that were assigned to sub engine's BGExtPltt
 *---------------------------------------------------------------------------*/
GXVRamSubBGExtPltt GX_ResetBankForSubBGExtPltt()
{
#ifndef SDK_FINALROM
    if (!OSi_TryLockVram((u16)gGXState.vramCnt.sub_bgExtPltt, GXi_VRamLockId))
    {
        return GX_VRAM_SUB_BGEXTPLTT_NONE;
    }
#endif
    subBGExtPlttOff_();
    return (GXVRamSubBGExtPltt)resetBankForX_(&gGXState.vramCnt.sub_bgExtPltt);
}


/*---------------------------------------------------------------------------*
  Name:         GX_ResetBankForSubOBJExtPltt

  Description:  Switches sub engine's OBJExtPltt banks onto LCDC memory space.

  Arguments:    none

  Returns:      the banks that were assigned to sub engine's OBJExtPltt
 *---------------------------------------------------------------------------*/
GXVRamSubOBJExtPltt GX_ResetBankForSubOBJExtPltt()
{
#ifndef SDK_FINALROM
    if (!OSi_TryLockVram((u16)gGXState.vramCnt.sub_objExtPltt, GXi_VRamLockId))
    {
        return GX_VRAM_SUB_OBJEXTPLTT_NONE;
    }
#endif
    subOBJExtPlttOff_();
    return (GXVRamSubOBJExtPltt)resetBankForX_(&gGXState.vramCnt.sub_objExtPltt);
}


/*---------------------------------------------------------------------------*
  Name:         disableBankForX_

  Description:  Disables the argument banks.

  Arguments:    g3bit    a pointer to bank information

  Returns:      none
 *---------------------------------------------------------------------------*/
#ifdef  SDK_CW_WARNOFF_SAFESTRB
#include <nitro/code32.h>
#endif
static int disableBankForX_(u16 *g3bit)
{
    int     rval = *g3bit;
    *g3bit = 0;                        // GX_VRAMCNT_xxxxxx_NONE

    if (rval & GX_VRAM_LCDC_A)
        reg_GX_VRAMCNT_A = 0;
    if (rval & GX_VRAM_LCDC_B)
        reg_GX_VRAMCNT_B = 0;
    if (rval & GX_VRAM_LCDC_C)
        reg_GX_VRAMCNT_C = 0;
    if (rval & GX_VRAM_LCDC_D)
        reg_GX_VRAMCNT_D = 0;
    if (rval & GX_VRAM_LCDC_E)
        reg_GX_VRAMCNT_E = 0;
    if (rval & GX_VRAM_LCDC_F)
        reg_GX_VRAMCNT_F = 0;
    if (rval & GX_VRAM_LCDC_G)
        reg_GX_VRAMCNT_G = 0;
    if (rval & GX_VRAM_LCDC_H)
        reg_GX_VRAMCNT_H = 0;
    if (rval & GX_VRAM_LCDC_I)
        reg_GX_VRAMCNT_I = 0;

    OSi_UnlockVram((u16)rval, GXi_VRamLockId);

    return rval;
}

#ifdef  SDK_CW_WARNOFF_SAFESTRB
#include <nitro/codereset.h>
#endif

/*---------------------------------------------------------------------------*
  Name:         GX_DisableBankForBG

  Description:  Disables the banks assigned to BG.

  Arguments:    none

  Returns:      the banks that were assigned to BG
 *---------------------------------------------------------------------------*/
GXVRamBG GX_DisableBankForBG()
{
#ifndef SDK_FINALROM
    if (!OSi_TryLockVram((u16)gGXState.vramCnt.bg, GXi_VRamLockId))
    {
        return GX_VRAM_BG_NONE;
    }
#endif
    return (GXVRamBG)disableBankForX_(&gGXState.vramCnt.bg);
}


/*---------------------------------------------------------------------------*
  Name:         GX_DisableBankForOBJ

  Description:  Disables the banks assigned to OBJ.

  Arguments:    none

  Returns:      the banks that were assigned to OBJ
 *---------------------------------------------------------------------------*/
GXVRamOBJ GX_DisableBankForOBJ()
{
#ifndef SDK_FINALROM
    if (!OSi_TryLockVram((u16)gGXState.vramCnt.obj, GXi_VRamLockId))
    {
        return GX_VRAM_OBJ_NONE;
    }
#endif
    return (GXVRamOBJ)disableBankForX_(&gGXState.vramCnt.obj);
}


/*---------------------------------------------------------------------------*
  Name:         GX_DisableBankForBGExtPltt

  Description:  Disables the banks assigned to BGExtPltt.

  Arguments:    none

  Returns:      the banks that were assigned to BGExtPltt
 *---------------------------------------------------------------------------*/
GXVRamBGExtPltt GX_DisableBankForBGExtPltt()
{
#ifndef SDK_FINALROM
    if (!OSi_TryLockVram((u16)gGXState.vramCnt.bgExtPltt, GXi_VRamLockId))
    {
        return GX_VRAM_BGEXTPLTT_NONE;
    }
#endif
    bgExtPlttOff_();
    return (GXVRamBGExtPltt)disableBankForX_(&gGXState.vramCnt.bgExtPltt);
}


/*---------------------------------------------------------------------------*
  Name:         GX_DisableBankForOBJExtPltt

  Description:  Disables the banks assigned to OBJExtPltt.

  Arguments:    none

  Returns:      the banks that were assigned to OBJExtPltt
 *---------------------------------------------------------------------------*/
GXVRamOBJExtPltt GX_DisableBankForOBJExtPltt()
{
#ifndef SDK_FINALROM
    if (!OSi_TryLockVram((u16)gGXState.vramCnt.objExtPltt, GXi_VRamLockId))
    {
        return GX_VRAM_OBJEXTPLTT_NONE;
    }
#endif
    objExtPlttOff_();
    return (GXVRamOBJExtPltt)disableBankForX_(&gGXState.vramCnt.objExtPltt);
}


/*---------------------------------------------------------------------------*
  Name:         GX_DisableBankForTex

  Description:  Disables the banks assigned to Tex.

  Arguments:    none

  Returns:      the banks that were assigned to Tex
 *---------------------------------------------------------------------------*/
GXVRamTex GX_DisableBankForTex()
{
#ifndef SDK_FINALROM
    if (!OSi_TryLockVram((u16)gGXState.vramCnt.tex, GXi_VRamLockId))
    {
        return GX_VRAM_TEX_NONE;
    }
#endif
    return (GXVRamTex)disableBankForX_(&gGXState.vramCnt.tex);
}


/*---------------------------------------------------------------------------*
  Name:         GX_DisableBankForTexPltt

  Description:  Disables the banks assigned to TexPltt.

  Arguments:    none

  Returns:      the banks that were assigned to TexPltt
 *---------------------------------------------------------------------------*/
GXVRamTexPltt GX_DisableBankForTexPltt()
{
#ifndef SDK_FINALROM
    if (!OSi_TryLockVram((u16)gGXState.vramCnt.texPltt, GXi_VRamLockId))
    {
        return GX_VRAM_TEXPLTT_NONE;
    }
#endif
    return (GXVRamTexPltt)disableBankForX_(&gGXState.vramCnt.texPltt);
}


/*---------------------------------------------------------------------------*
  Name:         GX_DisableBankForClearImage

  Description:  Disables the banks assigned to ClearImage.

  Arguments:    none

  Returns:      the banks that were assigned to ClearImage
 *---------------------------------------------------------------------------*/
GXVRamClearImage GX_DisableBankForClearImage()
{
#ifndef SDK_FINALROM
    if (!OSi_TryLockVram((u16)gGXState.vramCnt.clrImg, GXi_VRamLockId))
    {
        return GX_VRAM_CLEARIMAGE_NONE;
    }
#endif
    return (GXVRamClearImage)disableBankForX_(&gGXState.vramCnt.clrImg);
}


/*---------------------------------------------------------------------------*
  Name:         GX_DisableBankForARM7

  Description:  Disables the banks assigned to ARM7.

  Arguments:    none

  Returns:      the banks that were assigned to ARM7
 *---------------------------------------------------------------------------*/
GXVRamARM7 GX_DisableBankForARM7()
{
#ifndef SDK_FINALROM
    if (!OSi_TryLockVram((u16)gGXState.vramCnt.arm7, GXi_VRamLockId))
    {
        return GX_VRAM_ARM7_NONE;
    }
#endif
    return (GXVRamARM7)disableBankForX_(&gGXState.vramCnt.arm7);
}


/*---------------------------------------------------------------------------*
  Name:         GX_DisableBankForLCDC

  Description:  Disables the banks assigned to LCDC.

  Arguments:    none

  Returns:      the banks that were assigned to LCDC
 *---------------------------------------------------------------------------*/
GXVRamLCDC GX_DisableBankForLCDC()
{
#ifndef SDK_FINALROM
    if (!OSi_TryLockVram((u16)gGXState.vramCnt.lcdc, GXi_VRamLockId))
    {
        return GX_VRAM_LCDC_NONE;
    }
#endif
    return (GXVRamLCDC)disableBankForX_(&gGXState.vramCnt.lcdc);
}


/*---------------------------------------------------------------------------*
  Name:         GX_DisableBankForSubBG

  Description:  Disables the banks assigned to sub engine's BG.

  Arguments:    none

  Returns:      the banks that were assigned to sub engine's BG
 *---------------------------------------------------------------------------*/
GXVRamSubBG GX_DisableBankForSubBG()
{
#ifndef SDK_FINALROM
    if (!OSi_TryLockVram((u16)gGXState.vramCnt.sub_bg, GXi_VRamLockId))
    {
        return GX_VRAM_SUB_BG_NONE;
    }
#endif
    return (GXVRamSubBG)disableBankForX_(&gGXState.vramCnt.sub_bg);
}


/*---------------------------------------------------------------------------*
  Name:         GX_DisableBankForSubOBJ

  Description:  Disables the banks assigned to sub engine's OBJ.

  Arguments:    none

  Returns:      the banks that were assigned to sub engine's OBJ
 *---------------------------------------------------------------------------*/
GXVRamSubOBJ GX_DisableBankForSubOBJ()
{
#ifndef SDK_FINALROM
    if (!OSi_TryLockVram((u16)gGXState.vramCnt.sub_obj, GXi_VRamLockId))
    {
        return GX_VRAM_SUB_OBJ_NONE;
    }
#endif
    return (GXVRamSubOBJ)disableBankForX_(&gGXState.vramCnt.sub_obj);
}


/*---------------------------------------------------------------------------*
  Name:         GX_DisableBankForSubBGExtPltt

  Description:  Disables the banks assigned to sub engine's BGExtPltt.

  Arguments:    none

  Returns:      the banks that were assigned to sub engine's BGExtPltt
 *---------------------------------------------------------------------------*/
GXVRamSubBGExtPltt GX_DisableBankForSubBGExtPltt()
{
#ifndef SDK_FINALROM
    if (!OSi_TryLockVram((u16)gGXState.vramCnt.sub_bgExtPltt, GXi_VRamLockId))
    {
        return GX_VRAM_SUB_BGEXTPLTT_NONE;
    }
#endif
    subBGExtPlttOff_();
    return (GXVRamSubBGExtPltt)disableBankForX_(&gGXState.vramCnt.sub_bgExtPltt);
}


/*---------------------------------------------------------------------------*
  Name:         GX_DisableBankForSubOBJExtPltt

  Description:  Disables the banks assigned to sub engine's OBJExtPltt.

  Arguments:    none

  Returns:      the banks that were assigned to sub engine's OBJExtPltt
 *---------------------------------------------------------------------------*/
GXVRamSubOBJExtPltt GX_DisableBankForSubOBJExtPltt()
{
#ifndef SDK_FINALROM
    if (!OSi_TryLockVram((u16)gGXState.vramCnt.sub_objExtPltt, GXi_VRamLockId))
    {
        return GX_VRAM_SUB_OBJEXTPLTT_NONE;
    }
#endif
    subOBJExtPlttOff_();
    return (GXVRamSubOBJExtPltt)disableBankForX_(&gGXState.vramCnt.sub_objExtPltt);
}





/*---------------------------------------------------------------------------*
  Name:         GX_GetBankForBG

  Description:  Returns the bank assigned to BG.

  Arguments:    none

  Returns:      the banks that are assigned to BG
 *---------------------------------------------------------------------------*/
GXVRamBG GX_GetBankForBG()
{
    return (GXVRamBG)gGXState.vramCnt.bg;
}


/*---------------------------------------------------------------------------*
  Name:         GX_GetBankForOBJ

  Description:  Returns the bank assigned to OBJ.

  Arguments:    none

  Returns:      the banks that are assigned to OBJ
 *---------------------------------------------------------------------------*/
GXVRamOBJ GX_GetBankForOBJ()
{
    return (GXVRamOBJ)gGXState.vramCnt.obj;
}


/*---------------------------------------------------------------------------*
  Name:         GX_GetBankForBGExtPltt

  Description:  Returns the bank assigned to BGExtPltt.

  Arguments:    none

  Returns:      the banks that are assigned to BGExtPltt
 *---------------------------------------------------------------------------*/
GXVRamBGExtPltt GX_GetBankForBGExtPltt()
{
    return (GXVRamBGExtPltt)gGXState.vramCnt.bgExtPltt;
}


/*---------------------------------------------------------------------------*
  Name:         GX_GetBankForOBJExtPltt

  Description:  Returns the bank assigned to OBJExtPltt.

  Arguments:    none

  Returns:      the banks that are assigned to OBJExtPltt
 *---------------------------------------------------------------------------*/
GXVRamOBJExtPltt GX_GetBankForOBJExtPltt()
{
    return (GXVRamOBJExtPltt)gGXState.vramCnt.objExtPltt;
}


/*---------------------------------------------------------------------------*
  Name:         GX_GetBankForTex

  Description:  Returns the bank assigned to Tex.

  Arguments:    none

  Returns:      the banks that are assigned to Tex
 *---------------------------------------------------------------------------*/
GXVRamTex GX_GetBankForTex()
{
    return (GXVRamTex)gGXState.vramCnt.tex;
}


/*---------------------------------------------------------------------------*
  Name:         GX_GetBankForTexPltt

  Description:  Returns the bank assigned to TexPltt.

  Arguments:    none

  Returns:      the banks that are assigned to TexPltt
 *---------------------------------------------------------------------------*/
GXVRamTexPltt GX_GetBankForTexPltt()
{
    return (GXVRamTexPltt)gGXState.vramCnt.texPltt;
}


/*---------------------------------------------------------------------------*
  Name:         GX_GetBankForClearImage

  Description:  Returns the bank assigned to ClearImage.

  Arguments:    none

  Returns:      the banks that are assigned to ClearImage
 *---------------------------------------------------------------------------*/
GXVRamClearImage GX_GetBankForClearImage()
{
    return (GXVRamClearImage)gGXState.vramCnt.clrImg;
}


/*---------------------------------------------------------------------------*
  Name:         GX_GetBankForARM7

  Description:  Returns the bank assigned to ARM7.

  Arguments:    none

  Returns:      the banks that are assigned to ARM7
 *---------------------------------------------------------------------------*/
GXVRamARM7 GX_GetBankForARM7()
{
    return (GXVRamARM7)gGXState.vramCnt.arm7;
}


/*---------------------------------------------------------------------------*
  Name:         GX_GetBankForLCDC

  Description:  Returns the bank assigned to LCDC.

  Arguments:    none

  Returns:      the banks that are assigned to LCDC
 *---------------------------------------------------------------------------*/
GXVRamLCDC GX_GetBankForLCDC()
{
    return (GXVRamLCDC)gGXState.vramCnt.lcdc;
}


/*---------------------------------------------------------------------------*
  Name:         GX_GetBankForSubBG

  Description:  Returns the bank assigned to sub engine's BG.

  Arguments:    none

  Returns:      the banks that are assigned to sub engine's BG
 *---------------------------------------------------------------------------*/
GXVRamSubBG GX_GetBankForSubBG()
{
    return (GXVRamSubBG)gGXState.vramCnt.sub_bg;
}


/*---------------------------------------------------------------------------*
  Name:         GX_GetBankForSubOBJ

  Description:  Returns the bank assigned to sub engine's OBJ.

  Arguments:    none

  Returns:      the banks that are assigned to sub engine's OBJ
 *---------------------------------------------------------------------------*/
GXVRamSubOBJ GX_GetBankForSubOBJ()
{
    return (GXVRamSubOBJ)gGXState.vramCnt.sub_obj;
}


/*---------------------------------------------------------------------------*
  Name:         GX_GetBankForSubBGExtPltt

  Description:  Returns the bank assigned to sub engine's BGExtPltt.

  Arguments:    none

  Returns:      the banks that are assigned to sub engine's BGExtPltt
 *---------------------------------------------------------------------------*/
GXVRamSubBGExtPltt GX_GetBankForSubBGExtPltt()
{
    return (GXVRamSubBGExtPltt)gGXState.vramCnt.sub_bgExtPltt;
}


/*---------------------------------------------------------------------------*
  Name:         GX_GetBankForSubOBJExtPltt

  Description:  Returns the bank assigned to sub engine's OBJExtPltt.

  Arguments:    none

  Returns:      the banks that are assigned to sub engine's OBJExtPltt
 *---------------------------------------------------------------------------*/
GXVRamSubOBJExtPltt GX_GetBankForSubOBJExtPltt()
{
    return (GXVRamSubOBJExtPltt)gGXState.vramCnt.sub_objExtPltt;
}


/*---------------------------------------------------------------------------*
  Name:         GX_GetSizeOfX_(internal use only)

  Description:  Returns the size of VRAM allocated to BG/OBJ/Tex etc.

  Arguments:    bit          the value of GXVRamXXXXX

  Returns:      the size of resouce in bytes
 *---------------------------------------------------------------------------*/
static u32 GX_GetSizeOfX_(u32 bit /* gGXState.vramCnt.????? */ )
{
    u32     size = 0;
    if (bit & GX_VRAM_LCDC_A)
        size += HW_VRAM_A_SIZE;
    if (bit & GX_VRAM_LCDC_B)
        size += HW_VRAM_B_SIZE;
    if (bit & GX_VRAM_LCDC_C)
        size += HW_VRAM_C_SIZE;
    if (bit & GX_VRAM_LCDC_D)
        size += HW_VRAM_D_SIZE;
    if (bit & GX_VRAM_LCDC_E)
        size += HW_VRAM_E_SIZE;
    if (bit & GX_VRAM_LCDC_F)
        size += HW_VRAM_F_SIZE;
    if (bit & GX_VRAM_LCDC_G)
        size += HW_VRAM_G_SIZE;
    if (bit & GX_VRAM_LCDC_H)
        size += HW_VRAM_H_SIZE;
    if (bit & GX_VRAM_LCDC_I)
        size += HW_VRAM_I_SIZE;
    return size;
}


/*---------------------------------------------------------------------------*
  Name:         GX_GetSizeOfBG

  Description:  Returns the size of the memory allocated to the main engine's
                BG.

  Arguments:    none

  Returns:      the size of the main engine's BG
 *---------------------------------------------------------------------------*/
u32 GX_GetSizeOfBG(void)
{
    return GX_GetSizeOfX_(gGXState.vramCnt.bg);
}


/*---------------------------------------------------------------------------*
  Name:         GX_GetSizeOfOBJ

  Description:  Returns the size of the memory allocated to the main engine's OBJ.

  Arguments:    none

  Returns:      the size of the main engine's OBJ
 *---------------------------------------------------------------------------*/
u32 GX_GetSizeOfOBJ(void)
{
    return GX_GetSizeOfX_(gGXState.vramCnt.obj);
}


/*---------------------------------------------------------------------------*
  Name:         GX_GetSizeOfBGExtPltt

  Description:  Returns the size of the memory allocated to the main engine's
                BGExtPltt.

  Arguments:    none

  Returns:      the size of the main engine's BGExtPltt
 *---------------------------------------------------------------------------*/
u32 GX_GetSizeOfBGExtPltt(void)
{
    return GX_GetSizeOfX_(gGXState.vramCnt.bgExtPltt);
}


/*---------------------------------------------------------------------------*
  Name:         GX_GetSizeOfOBJExtPltt

  Description:  Returns the size of the memory allocated to the main engine's
                OBJExtPltt.

  Arguments:    none

  Returns:      the size of the main engine's OBJExtPltt
 *---------------------------------------------------------------------------*/
u32 GX_GetSizeOfOBJExtPltt(void)
{
    return GX_GetSizeOfX_(gGXState.vramCnt.objExtPltt);
}


/*---------------------------------------------------------------------------*
  Name:         GX_GetSizeOfTex

  Description:  Returns the size of the memory allocated to texture image slots.

  Arguments:    none

  Returns:      the size of the texture image memory
 *---------------------------------------------------------------------------*/
u32 GX_GetSizeOfTex(void)
{
    return GX_GetSizeOfX_(gGXState.vramCnt.tex);
}


/*---------------------------------------------------------------------------*
  Name:         GX_GetSizeOfTexPltt

  Description:  Returns the size of the memory allocated to texture palette
                slots.

  Arguments:    none

  Returns:      the size of the texture palette memory
 *---------------------------------------------------------------------------*/
u32 GX_GetSizeOfTexPltt(void)
{
    return GX_GetSizeOfX_(gGXState.vramCnt.texPltt);
}


/*---------------------------------------------------------------------------*
  Name:         GX_GetSizeOfClearImage

  Description:  Returns the size of the memory allocated to clear image slots.

  Arguments:    none

  Returns:      the size of the clear image memory
 *---------------------------------------------------------------------------*/
u32 GX_GetSizeOfClearImage(void)
{
    return GX_GetSizeOfX_(gGXState.vramCnt.clrImg);
}


/*---------------------------------------------------------------------------*
  Name:         GX_GetSizeOfSubBG

  Description:  Returns the size of the memory allocated to the sub engine's BG.

  Arguments:    none

  Returns:      the size of the sub engine's BG
 *---------------------------------------------------------------------------*/
u32 GX_GetSizeOfSubBG(void)
{
    return GX_GetSizeOfX_(gGXState.vramCnt.sub_bg);
}


/*---------------------------------------------------------------------------*
  Name:         GX_GetSizeOfSubOBJ

  Description:  Returns the size of the memory allocated to the sub engine's OBJ.

  Arguments:    none

  Returns:      the size of the sub engine's OBJ
 *---------------------------------------------------------------------------*/
u32 GX_GetSizeOfSubOBJ(void)
{
    return GX_GetSizeOfX_(gGXState.vramCnt.sub_obj);
}


/*---------------------------------------------------------------------------*
  Name:         GX_GetSizeOfSubBGExtPltt

  Description:  Returns the size of the memory allocated to the sub engine's
                BGExtPltt.

  Arguments:    none

  Returns:      the size of the sub engine's BGExtPltt
 *---------------------------------------------------------------------------*/
u32 GX_GetSizeOfSubBGExtPltt(void)
{
    return GX_GetSizeOfX_(gGXState.vramCnt.sub_bgExtPltt);
}


/*---------------------------------------------------------------------------*
  Name:         GX_GetSizeOfSubOBJExtPltt

  Description:  Returns the size of the memory allocated to the sub engine's
                OBJExtPltt.

  Arguments:    none

  Returns:      the size of the sub engine's OBJExtPltt
 *---------------------------------------------------------------------------*/
u32 GX_GetSizeOfSubOBJExtPltt(void)
{
    return GX_GetSizeOfX_(gGXState.vramCnt.sub_objExtPltt);
}


/*---------------------------------------------------------------------------*
  Name:         GX_GetSizeOfARM7

  Description:  Returns the size of VRAM allocated to ARM7 memory space.

  Arguments:    none

  Returns:      the size of VRAM allocated to ARM7 memory space
 *---------------------------------------------------------------------------*/
u32 GX_GetSizeOfARM7(void)
{
    return GX_GetSizeOfX_(gGXState.vramCnt.arm7);
}
