/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - 
  File:     gxstate.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: gxstate.c,v $
  Revision 1.31  2006/01/18 02:11:21  kitase_hirotake
  do-indent

  Revision 1.30  2005/02/28 05:26:13  yosizaki
  do-indent.

  Revision 1.29  2005/02/28 04:26:31  yosizaki
  SDK_NO_MESSAGE ‚ÉŠÖ‚·‚é¬‚³‚ÈC³.

  Revision 1.28  2004/12/03 04:31:58  takano_makoto
  GX_SetBankForBGExŠÖ”‚Ì’Ç‰Á‚Ì‚½‚ßABG‚ÌRegionCheck‚ðC³

  Revision 1.27  2004/11/25 11:29:01  takano_makoto
  add GX_SetBankForBGEx

  Revision 1.26  2004/03/08 02:29:04  takano_makoto
  Modify about GX_VRAM_OBJ_80_EG.

  Revision 1.25  2004/03/04 13:43:31  takano_makoto
  Add GX_VRAM_BG_80_EF, GX_VRAM_BG_80_EG, GX_VRAM_OBJ_80_EF, GX_VRAM_OBJ_80_EG

  Revision 1.24  2004/03/03 05:51:20  kitani_toshikazu
  Fix bugs in GX_RegionCheck_OBJ_().

  Revision 1.23  2004/03/02 09:17:22  nishida_kenji
  Fix bugs in GX_RegionCheck_TexPltt_ and GX_RegionCheck_Tex_.

  Revision 1.22  2004/02/20 07:21:18  nishida_kenji
  Add region checks for Tex and TexPltt.

  Revision 1.21  2004/02/18 00:59:09  yasu
  add ifdef SDK_CW_WARNOFF_SAFESTRB

  Revision 1.20  2004/02/12 08:12:16  nishida_kenji
  Add GX_VRAM_BG_16_G and GX_VRAM_OBJ_16_G.

  Revision 1.19  2004/02/12 07:06:27  nishida_kenji
  Avoid generating STRB in THUMB mode.

  Revision 1.17  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.16  2004/02/03 01:42:06  nishida_kenji
  Convert magic numbers to macros, and add comments.

  Revision 1.15  2004/02/02 12:29:04  nishida_kenji
  small fix

  Revision 1.14  2004/02/02 05:53:25  nishida_kenji
  small bug fix

  Revision 1.13  2004/01/27 11:14:37  nishida_kenji
  adds APIs for the sub engine.

  Revision 1.12  2003/12/24 08:27:56  nishida_kenji
  move WRAM control to MI domain.

  Revision 1.11  2003/12/17 09:00:20  nishida_kenji
  Totally revised APIs.
  build/buildtools/GX_APIChangeFrom031212.pl would help you change your program to some extent.

  Revision 1.10  2003/12/17 08:53:17  nishida_kenji
  revise comments

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include "../include/gxstate.h"
#include <nitro/gx/gx_vramcnt.h>


GX_State gGXState;


/*---------------------------------------------------------------------------*
  Name:         GX_InitGXState(INTERNAL USE ONLY)

  Description:  Initializes the VRAM bank controls.

  Arguments:    none

  Returns:      none
 *---------------------------------------------------------------------------*/
#ifdef  SDK_CW_WARNOFF_SAFESTRB
#include <nitro/code32.h>
#endif
void GX_InitGXState()
{
    gGXState.vramCnt.lcdc = 0;
    gGXState.vramCnt.bg = 0;
    gGXState.vramCnt.obj = 0;
    gGXState.vramCnt.arm7 = 0;
    gGXState.vramCnt.tex = 0;
    gGXState.vramCnt.texPltt = 0;
    gGXState.vramCnt.clrImg = 0;
    gGXState.vramCnt.bgExtPltt = 0;
    gGXState.vramCnt.objExtPltt = 0;

    gGXState.vramCnt.sub_bg = 0;
    gGXState.vramCnt.sub_obj = 0;
    gGXState.vramCnt.sub_bgExtPltt = 0;
    gGXState.vramCnt.sub_objExtPltt = 0;

    reg_GX_VRAMCNT = 0;
    *((u8 *)&reg_GX_WVRAMCNT + 0) = 0;
    *((u8 *)&reg_GX_WVRAMCNT + 1) = 0;
    *((u8 *)&reg_GX_WVRAMCNT + 2) = 0;
    reg_GX_VRAM_HI_CNT = 0;
}

#ifdef  SDK_CW_WARNOFF_SAFESTRB
#include <nitro/codereset.h>
#endif

#define SDK_CONFLICT_ASSERT(a, b, name1, name2) \
    SDK_ASSERTMSG(!(a & b),                        \
                  "VRAM Bank conflicts between "#name1" and "#name2" (0x%02x, 0x%02x)\n", \
                  a, b)

#define SDK_CONFLICT_CHECK(val, bank) \
    if (val & bank) goto SDK_VRAMCNT_ERROR; else val |= bank


#ifdef SDK_DEBUG
/*---------------------------------------------------------------------------*
  Name:         GX_StateCheck_VRAMCnt_(INTERNAL USE ONLY)

  Description:  Checks VRAM bank conflicts.
                This is invoked by GX_SetBankForXXXXX only if SDK_DEBUG is
                defined.

  Arguments:    none

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_StateCheck_VRAMCnt_()
{
    GX_VRAMCnt_ *p = &gGXState.vramCnt;
    u16     tmp = p->lcdc;
    SDK_CONFLICT_CHECK(tmp, p->bg);
    SDK_CONFLICT_CHECK(tmp, p->obj);
    SDK_CONFLICT_CHECK(tmp, p->arm7);
    SDK_CONFLICT_CHECK(tmp, p->tex);
    SDK_CONFLICT_CHECK(tmp, p->texPltt);
    SDK_CONFLICT_CHECK(tmp, p->clrImg);
    SDK_CONFLICT_CHECK(tmp, p->bgExtPltt);
    SDK_CONFLICT_CHECK(tmp, p->objExtPltt);
    SDK_CONFLICT_CHECK(tmp, p->sub_bg);
    SDK_CONFLICT_CHECK(tmp, p->sub_obj);
    SDK_CONFLICT_CHECK(tmp, p->sub_bgExtPltt);
    SDK_CONFLICT_CHECK(tmp, p->sub_objExtPltt);
    return;

  SDK_VRAMCNT_ERROR:
    SDK_CONFLICT_ASSERT(p->lcdc, p->bg, LCDC, BG);
    SDK_CONFLICT_ASSERT(p->lcdc, p->obj, LCDC, OBJ);
    SDK_CONFLICT_ASSERT(p->lcdc, p->arm7, LCDC, ARM7);
    SDK_CONFLICT_ASSERT(p->lcdc, p->tex, LCDC, Tex);
    SDK_CONFLICT_ASSERT(p->lcdc, p->texPltt, LCDC, TEXPltt);
    SDK_CONFLICT_ASSERT(p->lcdc, p->clrImg, LCDC, ClearImage);
    SDK_CONFLICT_ASSERT(p->lcdc, p->bgExtPltt, LCDC, BGExtPltt);
    SDK_CONFLICT_ASSERT(p->lcdc, p->objExtPltt, LCDC, OBJExtPltt);
    SDK_CONFLICT_ASSERT(p->lcdc, p->sub_bg, LCDC, SubBG);
    SDK_CONFLICT_ASSERT(p->lcdc, p->sub_obj, LCDC, SubOBJ);
    SDK_CONFLICT_ASSERT(p->lcdc, p->sub_bgExtPltt, LCDC, SubBGExtPltt);
    SDK_CONFLICT_ASSERT(p->lcdc, p->sub_objExtPltt, LCDC, SubOBJExtPltt);

    SDK_CONFLICT_ASSERT(p->bg, p->obj, BG, OBJ);
    SDK_CONFLICT_ASSERT(p->bg, p->arm7, BG, ARM7);
    SDK_CONFLICT_ASSERT(p->bg, p->tex, BG, Tex);
    SDK_CONFLICT_ASSERT(p->bg, p->texPltt, BG, TexPltt);
    SDK_CONFLICT_ASSERT(p->bg, p->clrImg, BG, ClearImage);
    SDK_CONFLICT_ASSERT(p->bg, p->bgExtPltt, BG, BGExtPltt);
    SDK_CONFLICT_ASSERT(p->bg, p->objExtPltt, BG, OBJExtPltt);
    SDK_CONFLICT_ASSERT(p->bg, p->sub_bg, BG, SubBG);
    SDK_CONFLICT_ASSERT(p->bg, p->sub_obj, BG, SubOBJ);
    SDK_CONFLICT_ASSERT(p->bg, p->sub_bgExtPltt, BG, SubBGExtPltt);
    SDK_CONFLICT_ASSERT(p->bg, p->sub_objExtPltt, BG, SubOBJExtPltt);

    SDK_CONFLICT_ASSERT(p->obj, p->arm7, OBJ, ARM7);
    SDK_CONFLICT_ASSERT(p->obj, p->tex, OBJ, Tex);
    SDK_CONFLICT_ASSERT(p->obj, p->texPltt, OBJ, TexPltt);
    SDK_CONFLICT_ASSERT(p->obj, p->clrImg, OBJ, ClearImage);
    SDK_CONFLICT_ASSERT(p->obj, p->bgExtPltt, OBJ, BGExtPltt);
    SDK_CONFLICT_ASSERT(p->obj, p->objExtPltt, OBJ, OBJExtPltt);
    SDK_CONFLICT_ASSERT(p->obj, p->sub_bg, OBJ, SubBG);
    SDK_CONFLICT_ASSERT(p->obj, p->sub_obj, OBJ, SubOBJ);
    SDK_CONFLICT_ASSERT(p->obj, p->sub_bgExtPltt, OBJ, SubBGExtPltt);
    SDK_CONFLICT_ASSERT(p->obj, p->sub_objExtPltt, OBJ, SubOBJExtPltt);

    SDK_CONFLICT_ASSERT(p->arm7, p->tex, ARM7, Tex);
    SDK_CONFLICT_ASSERT(p->arm7, p->texPltt, ARM7, TexPltt);
    SDK_CONFLICT_ASSERT(p->arm7, p->clrImg, ARM7, ClearImage);
    SDK_CONFLICT_ASSERT(p->arm7, p->bgExtPltt, ARM7, BGExtPltt);
    SDK_CONFLICT_ASSERT(p->arm7, p->objExtPltt, ARM7, OBJExtPltt);
    SDK_CONFLICT_ASSERT(p->arm7, p->sub_bg, ARM7, SubBG);
    SDK_CONFLICT_ASSERT(p->arm7, p->sub_obj, ARM7, SubOBJ);
    SDK_CONFLICT_ASSERT(p->arm7, p->sub_bgExtPltt, ARM7, SubBGExtPltt);
    SDK_CONFLICT_ASSERT(p->arm7, p->sub_objExtPltt, ARM7, SubOBJExtPltt);

    SDK_CONFLICT_ASSERT(p->tex, p->texPltt, Tex, TexPltt);
    SDK_CONFLICT_ASSERT(p->tex, p->clrImg, Tex, ClearImage);
    SDK_CONFLICT_ASSERT(p->tex, p->bgExtPltt, Tex, BGExtPltt);
    SDK_CONFLICT_ASSERT(p->tex, p->objExtPltt, Tex, OBJExtPltt);
    SDK_CONFLICT_ASSERT(p->tex, p->sub_bg, Tex, SubBG);
    SDK_CONFLICT_ASSERT(p->tex, p->sub_obj, Tex, SubOBJ);
    SDK_CONFLICT_ASSERT(p->tex, p->sub_bgExtPltt, Tex, SubBGExtPltt);
    SDK_CONFLICT_ASSERT(p->tex, p->sub_objExtPltt, Tex, SubOBJExtPltt);

    SDK_CONFLICT_ASSERT(p->texPltt, p->clrImg, TexPltt, ClearImage);
    SDK_CONFLICT_ASSERT(p->texPltt, p->bgExtPltt, TexPltt, BGExtPltt);
    SDK_CONFLICT_ASSERT(p->texPltt, p->objExtPltt, TexPltt, OBJExtPltt);
    SDK_CONFLICT_ASSERT(p->texPltt, p->sub_bg, TexPltt, SubBG);
    SDK_CONFLICT_ASSERT(p->texPltt, p->sub_obj, TexPltt, SubOBJ);
    SDK_CONFLICT_ASSERT(p->texPltt, p->sub_bgExtPltt, TexPltt, SubBGExtPltt);
    SDK_CONFLICT_ASSERT(p->texPltt, p->sub_objExtPltt, TexPltt, SubOBJExtPltt);

    SDK_CONFLICT_ASSERT(p->clrImg, p->bgExtPltt, ClearImage, BGExtPltt);
    SDK_CONFLICT_ASSERT(p->clrImg, p->objExtPltt, ClearImage, OBJExtPltt);
    SDK_CONFLICT_ASSERT(p->clrImg, p->sub_bg, ClearImage, SubBG);
    SDK_CONFLICT_ASSERT(p->clrImg, p->sub_obj, ClearImage, SubOBJ);
    SDK_CONFLICT_ASSERT(p->clrImg, p->sub_bgExtPltt, ClearImage, SubBGExtPltt);
    SDK_CONFLICT_ASSERT(p->clrImg, p->sub_objExtPltt, ClearImage, SubOBJExtPltt);

    SDK_CONFLICT_ASSERT(p->bgExtPltt, p->objExtPltt, BGExtPltt, OBJExtPltt);
    SDK_CONFLICT_ASSERT(p->bgExtPltt, p->sub_bg, BGExtPltt, SubBG);
    SDK_CONFLICT_ASSERT(p->bgExtPltt, p->sub_obj, BGExtPltt, SubOBJ);
    SDK_CONFLICT_ASSERT(p->bgExtPltt, p->sub_bgExtPltt, BGExtPltt, SubBGExtPltt);
    SDK_CONFLICT_ASSERT(p->bgExtPltt, p->sub_objExtPltt, BGExtPltt, SubOBJExtPltt);

    SDK_CONFLICT_ASSERT(p->objExtPltt, p->sub_bg, OBJExtPltt, SubBG);
    SDK_CONFLICT_ASSERT(p->objExtPltt, p->sub_obj, OBJExtPltt, SubOBJ);
    SDK_CONFLICT_ASSERT(p->objExtPltt, p->sub_bgExtPltt, OBJExtPltt, SubBGExtPltt);
    SDK_CONFLICT_ASSERT(p->objExtPltt, p->sub_objExtPltt, OBJExtPltt, SubOBJExtPltt);

    SDK_CONFLICT_ASSERT(p->sub_bg, p->sub_obj, SubBG, SubOBJ);
    SDK_CONFLICT_ASSERT(p->sub_bg, p->sub_bgExtPltt, SubBG, SubBGExtPltt);
    SDK_CONFLICT_ASSERT(p->sub_bg, p->sub_objExtPltt, SubBG, SubOBJExtPltt);

    SDK_CONFLICT_ASSERT(p->sub_obj, p->sub_bgExtPltt, SubOBJ, SubBGExtPltt);
    SDK_CONFLICT_ASSERT(p->sub_obj, p->sub_objExtPltt, SubOBJ, SubOBJExtPltt);

    SDK_CONFLICT_ASSERT(p->sub_bgExtPltt, p->sub_objExtPltt, SubBGExtPltt, SubOBJExtPltt);
}

#define SDK_REGION_ASSERT(name, r1, r2, d1, d2)     \
    SDK_ASSERTMSG(((r1) <= (d1)) && ((r2) >= (d2)), \
                  "Region allocated to "#name" is (0x%08x -> 0x%08x),\n"\
                  "access occured (0x%08x -> 0x%08x).", r1, r2, d1, d2)

#define SDK_REGION_ASSERT_EX(name, r1, r2, r3, r4, d1, d2)     \
    SDK_ASSERTMSG(( ((r1) <= (d1)) && ((r2) >= (d2)) ) || ( ((r3) <= (d1)) && ((r4) >= (d2)) ), \
                  "Region allocated to "#name" is (0x%08x -> 0x%08x, 0x%08x -> 0x%08x),\n"\
                  "access occured (0x%08x -> 0x%08x).", r1, r2, r3, r4, d1, d2)


/*---------------------------------------------------------------------------*
  Name:         GX_RegionCheck_OBJ_(INTERNAL USE ONLY)

  Description:  Checks if memory exists at the destination address.
                This is invoked by GX_LoadOBJ only if SDK_DEBUG is defined.

  Arguments:    first        a start address
                last         an end address
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_RegionCheck_OBJ_(u32 first, u32 last)
{
    switch (gGXState.vramCnt.obj)
    {
    case GX_VRAM_OBJ_NONE:
        SDK_REGION_ASSERT(OBJ, HW_OBJ_VRAM, HW_OBJ_VRAM, first, last);
        break;
        // HW_VRAM_F_SIZE and HW_VRAM_G_SIZE Must be same size.
    case GX_VRAM_OBJ_16_F:
    case GX_VRAM_OBJ_16_G:
        SDK_REGION_ASSERT(OBJ, HW_OBJ_VRAM, HW_OBJ_VRAM + HW_VRAM_F_SIZE, first, last);
        break;
    case GX_VRAM_OBJ_32_FG:
        SDK_REGION_ASSERT(OBJ, HW_OBJ_VRAM,
                          HW_OBJ_VRAM + HW_VRAM_F_SIZE + HW_VRAM_G_SIZE, first, last);
        break;
    case GX_VRAM_OBJ_64_E:
        SDK_REGION_ASSERT(OBJ, HW_OBJ_VRAM, HW_OBJ_VRAM + HW_VRAM_E_SIZE, first, last);
        break;
    case GX_VRAM_OBJ_80_EF:
        SDK_REGION_ASSERT(OBJ, HW_OBJ_VRAM,
                          HW_OBJ_VRAM + HW_VRAM_E_SIZE + HW_VRAM_F_SIZE, first, last);
        break;
    case GX_VRAM_OBJ_80_EG:
        SDK_REGION_ASSERT(OBJ, HW_OBJ_VRAM,
                          HW_OBJ_VRAM + HW_VRAM_E_SIZE + HW_VRAM_G_SIZE, first, last);
        break;
    case GX_VRAM_OBJ_96_EFG:
        SDK_REGION_ASSERT(OBJ, HW_OBJ_VRAM,
                          HW_OBJ_VRAM + HW_VRAM_E_SIZE + HW_VRAM_F_SIZE + HW_VRAM_G_SIZE,
                          first, last);
        break;
        // HW_VRAM_A_SIZE and HW_VRAM_B_SIZE Must be same size.
    case GX_VRAM_OBJ_128_A:
    case GX_VRAM_OBJ_128_B:
        SDK_REGION_ASSERT(OBJ, HW_OBJ_VRAM, HW_OBJ_VRAM + HW_VRAM_A_SIZE, first, last);
        break;
    case GX_VRAM_OBJ_256_AB:
        SDK_REGION_ASSERT(OBJ, HW_OBJ_VRAM,
                          HW_OBJ_VRAM + HW_VRAM_A_SIZE + HW_VRAM_B_SIZE, first, last);
        break;
    default:
        SDK_INTERNAL_ERROR("unknown case 0x%x", gGXState.vramCnt.obj);
        break;
    }
}


/*---------------------------------------------------------------------------*
  Name:         GX_RegionCheck_BG_(INTERNAL USE ONLY)

  Description:  Checks if memory exists at the destination address.
                This is invoked by GX_LoadBGXXXXX only if SDK_DEBUG is
                defined.

  Arguments:    first        a start address
                last         an end address
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_RegionCheck_BG_(u32 first, u32 last)
{
    u16     vram_abcd;
    u16     vram_efg;
    u32     abcd_size, efg_size;

    SDK_ASSERT(HW_VRAM_A_SIZE == HW_VRAM_B_SIZE &&
               HW_VRAM_A_SIZE == HW_VRAM_C_SIZE && HW_VRAM_A_SIZE == HW_VRAM_D_SIZE);

    if (gGXState.vramCnt.bg & (GX_VRAM_H | GX_VRAM_I))
    {
        SDK_INTERNAL_ERROR("unknown case 0x%x", gGXState.vramCnt.bg);
        return;
    }

    vram_abcd = (u16)(gGXState.vramCnt.bg & GX_VRAM_BG_512_ABCD);
    vram_efg = (u16)(gGXState.vramCnt.bg & GX_VRAM_BG_96_EFG);

    switch (vram_efg)
    {
    case GX_VRAM_BG_NONE:
        efg_size = 0;
        break;
    case GX_VRAM_BG_16_F:
    case GX_VRAM_BG_16_G:
        efg_size = HW_VRAM_F_SIZE;
        break;
    case GX_VRAM_BG_32_FG:
        efg_size = HW_VRAM_F_SIZE + HW_VRAM_G_SIZE;
        break;
    case GX_VRAM_BG_64_E:
        efg_size = HW_VRAM_E_SIZE;
        break;
    case GX_VRAM_BG_80_EF:
    case GX_VRAM_BG_80_EG:
        efg_size = HW_VRAM_E_SIZE + HW_VRAM_F_SIZE;
        break;
    case GX_VRAM_BG_96_EFG:
        efg_size = HW_VRAM_E_SIZE + HW_VRAM_F_SIZE + HW_VRAM_G_SIZE;
        break;
    }

    switch (vram_abcd)
    {
    case GX_VRAM_BG_NONE:
        abcd_size = 0;
        break;
    case GX_VRAM_BG_128_A:
    case GX_VRAM_BG_128_B:
    case GX_VRAM_BG_128_C:
    case GX_VRAM_BG_128_D:
        abcd_size = HW_VRAM_A_SIZE;
        break;
    case GX_VRAM_BG_256_AB:
    case GX_VRAM_BG_256_BC:
    case GX_VRAM_BG_256_CD:
    case GX_VRAM_BG_256_AC:
    case GX_VRAM_BG_256_AD:
    case GX_VRAM_BG_256_BD:
        abcd_size = HW_VRAM_A_SIZE + HW_VRAM_B_SIZE;
        break;
    case GX_VRAM_BG_384_ABC:
    case GX_VRAM_BG_384_BCD:
    case GX_VRAM_BG_384_ABD:
    case GX_VRAM_BG_384_ACD:
        abcd_size = HW_VRAM_A_SIZE + HW_VRAM_B_SIZE + HW_VRAM_C_SIZE;
        break;
    case GX_VRAM_BG_512_ABCD:
        abcd_size = HW_VRAM_A_SIZE + HW_VRAM_B_SIZE + HW_VRAM_C_SIZE + HW_VRAM_D_SIZE;
        break;
    }

    if (vram_abcd && vram_efg)
    {
        if (vram_abcd == GX_VRAM_BG_512_ABCD)
        {
            SDK_INTERNAL_ERROR("unknown case 0x%x", gGXState.vramCnt.bg);
            return;
        }

        SDK_REGION_ASSERT_EX(BG, HW_BG_VRAM, HW_BG_VRAM + efg_size,
                             HW_BG_VRAM + HW_VRAM_A_SIZE, HW_BG_VRAM + HW_VRAM_A_SIZE + abcd_size,
                             first, last);
    }
    else if (vram_abcd)
    {
        SDK_REGION_ASSERT(BG, HW_BG_VRAM, HW_BG_VRAM + abcd_size, first, last);
    }
    else
    {
        SDK_REGION_ASSERT(BG, HW_BG_VRAM, HW_BG_VRAM + efg_size, first, last);
    }
}


/*---------------------------------------------------------------------------*
  Name:         GX_RegionCheck_SubOBJ_(INTERNAL USE ONLY)

  Description:  Checks if memory exists at the destination address.
                This is invoked by GXS_LoadOBJ only if SDK_DEBUG is defined.

  Arguments:    first        a start address
                last         an end address
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_RegionCheck_SubOBJ_(u32 first, u32 last)
{
    switch (gGXState.vramCnt.sub_obj)
    {
    case GX_VRAM_SUB_OBJ_NONE:
        SDK_REGION_ASSERT(SubOBJ, HW_DB_OBJ_VRAM, HW_DB_OBJ_VRAM, first, last);
        break;
    case GX_VRAM_SUB_OBJ_128_D:
        SDK_REGION_ASSERT(SubOBJ, HW_DB_OBJ_VRAM, HW_DB_OBJ_VRAM + HW_VRAM_D_SIZE, first, last);
        break;
    case GX_VRAM_SUB_OBJ_16_I:
        SDK_REGION_ASSERT(SubOBJ, HW_DB_OBJ_VRAM, HW_DB_OBJ_VRAM + HW_VRAM_I_SIZE, first, last);
        break;
    }
}


/*---------------------------------------------------------------------------*
  Name:         GX_RegionCheck_SubBG_(INTERNAL USE ONLY)

  Description:  Checks if memory exists at the destination address.
                This is invoked by GXS_LoadBGXXXXX only if SDK_DEBUG is defined.

  Arguments:    first        a start address
                last         an end address
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_RegionCheck_SubBG_(u32 first, u32 last)
{
    switch (gGXState.vramCnt.sub_bg)
    {
    case GX_VRAM_SUB_BG_NONE:
        SDK_REGION_ASSERT(SubBG, HW_DB_BG_VRAM, HW_DB_BG_VRAM, first, last);
        break;
    case GX_VRAM_SUB_BG_128_C:
        SDK_REGION_ASSERT(SubBG, HW_DB_BG_VRAM, HW_DB_BG_VRAM + HW_VRAM_C_SIZE, first, last);
        break;
    case GX_VRAM_SUB_BG_32_H:
        SDK_REGION_ASSERT(SubBG, HW_DB_BG_VRAM, HW_DB_BG_VRAM + HW_VRAM_H_SIZE, first, last);
        break;
    case GX_VRAM_SUB_BG_48_HI:
        SDK_REGION_ASSERT(SubBG, HW_DB_BG_VRAM,
                          HW_DB_BG_VRAM + HW_VRAM_H_SIZE + HW_VRAM_I_SIZE, first, last);
        break;
    default:
        SDK_INTERNAL_ERROR("unknown case 0x%x", gGXState.vramCnt.sub_bg);
        break;
    }
}


void GX_RegionCheck_TexPltt_(GXVRamTexPltt texPltt, u32 first, u32 last)
{
#if	defined(SDK_NO_MESSAGE)
#pragma	unused(first)
#endif
    switch (texPltt)
    {
    case GX_VRAM_TEXPLTT_0_F:
    case GX_VRAM_TEXPLTT_0_G:
        SDK_ASSERTMSG(last <= 0x04000,
                      "Texture pallete 0x04000-0x18000 not available(0x%5x->0x%5x)", first, last);
        break;

    case GX_VRAM_TEXPLTT_01_FG:
        SDK_ASSERTMSG(last <= 0x08000,
                      "Texture pallete 0x08000-0x18000 not available(0x%5x->0x%5x)", first, last);
        break;

    case GX_VRAM_TEXPLTT_0123_E:
        SDK_ASSERTMSG(last <= 0x10000,
                      "Texture pallete 0x10000-0x18000 not available(0x%5x->0x%5x)", first, last);
        break;

    case GX_VRAM_TEXPLTT_01234_EF:
        SDK_ASSERTMSG(last <= 0x14000,
                      "Texture pallete 0x14000-0x18000 not available(0x%5x->0x%5x)", first, last);
        break;

    case GX_VRAM_TEXPLTT_012345_EFG:
        SDK_ASSERTMSG(last <= 0x18000, "Illegal Address/size specified(0x%5x->0x%5x)", first, last);
        break;

    default:
        SDK_INTERNAL_ERROR("unknown case 0x%x", texPltt);
        break;
    }
}


void GX_RegionCheck_Tex_(GXVRamTex tex, u32 first, u32 last)
{
#if	defined(SDK_NO_MESSAGE)
#pragma	unused(first)
#endif
    switch (tex)
    {
    case GX_VRAM_TEX_0_A:
    case GX_VRAM_TEX_0_B:
    case GX_VRAM_TEX_0_C:
    case GX_VRAM_TEX_0_D:
        SDK_ASSERTMSG(last <= 0x20000,
                      "Texture slot 0x20000-0x80000 not available(0x%5x->0x%5x)", first, last);
        break;

    case GX_VRAM_TEX_01_AB:
    case GX_VRAM_TEX_01_BC:
    case GX_VRAM_TEX_01_CD:
    case GX_VRAM_TEX_01_AC:
    case GX_VRAM_TEX_01_AD:
    case GX_VRAM_TEX_01_BD:
        SDK_ASSERTMSG(last <= 0x40000,
                      "Texture slot 0x40000-0x80000 not available(0x%5x->0x%5x)", first, last);
        break;

    case GX_VRAM_TEX_012_ABC:
    case GX_VRAM_TEX_012_BCD:
    case GX_VRAM_TEX_012_ABD:
    case GX_VRAM_TEX_012_ACD:
        SDK_ASSERTMSG(last <= 0x60000,
                      "Texture slot 0x60000-0x80000 not available(0x%5x->0x%5x)", first, last);
        break;

    case GX_VRAM_TEX_0123_ABCD:
        SDK_ASSERTMSG(last <= 0x80000, "Illegal Address/size specified(0x%5x->0x%5x)", first, last);
        break;

    default:
        SDK_INTERNAL_ERROR("unknown case 0x%x", tex);
        break;
    };
}



#endif
