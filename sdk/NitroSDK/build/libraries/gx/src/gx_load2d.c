/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - 
  File:     gx_load2d.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: gx_load2d.c,v $
  Revision 1.19  2006/01/18 02:11:21  kitase_hirotake
  do-indent

  Revision 1.18  2005/03/01 01:57:00  yosizaki
  copyright ‚Ì”N‚ðC³.

  Revision 1.17  2005/02/28 05:26:13  yosizaki
  do-indent.

  Revision 1.16  2004/11/02 14:41:58  takano_makoto
  small fix

  Revision 1.15  2004/08/10 12:48:35  takano_makoto
  make available to use GX_DMA_NOT_USE.

  Revision 1.14  2004/07/28 11:27:21  takano_makoto
  make possible to change DMA No.

  Revision 1.13  2004/06/29 02:11:46  seiki_masashi
  #ifdef NITRO_DEBUG -> #ifdef SDK_DEBUG

  Revision 1.12  2004/06/07 00:52:10  takano_makoto
  modify GX_VRAM_SUB_OBJEXTPLTT_01_I to GX_VRAM_SUB_OBJEXTPLTT_0_I

  Revision 1.11  2004/06/02 13:06:42  takano_makoto
  Change EXTPLTT enum name.

  Revision 1.10  2004/02/24 01:20:44  nishida_kenji
  GXx_LoadOBJPltt and GXx_LoadBGPltt use MI_DmaCopy16.

  Revision 1.9  2004/02/23 23:29:48  nishida_kenji
  GXx_LoadBGnScr uses MI_DmaCopy16.

  Revision 1.8  2004/02/23 09:56:33  nishida_kenji
  Fix bugs in SDK_ASSERT(GX_LoadOBJExtPltt() and so on).

  Revision 1.7  2004/02/17 10:43:52  nishida_kenji
  small fix.

  Revision 1.6  2004/02/12 11:09:02  yasu
  change to new location of include files ARM9/ARM7

  Revision 1.5  2004/02/09 06:36:37  nishida_kenji
  Copy data asynchronously(BGExtPltt, OBJExtPltt).

  Revision 1.4  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.3  2004/02/02 05:53:25  nishida_kenji
  small bug fix

  Revision 1.2  2004/01/28 07:56:05  nishida_kenji
  add APIs for the sub 2D engine.

  Revision 1.1  2003/12/25 11:53:57  nishida_kenji
  new

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/gx/gx_load.h>
#include <nitro/gx/gx_vramcnt.h>
#include <nitro/gx/gx_bgcnt.h>
#include <nitro/hw/ARM9/mmap_global.h>
#include <nitro/mi/dma.h>

#include "../include/gxstate.h"
#include "../include/gxdma.h"

/*---------------------------------------------------------------------------*
  Name:         GX_LoadBGPltt

  Description:  Transfers BG palettes to the standard BG palette RAM
                for the MAIN 2D engine.
                pSrc, offset, and szByte must be 2-bytes aligned.

  Arguments:    pSrc             a pointer to the source of BG palettes
                offset           an offset to HW_BG_PLTT
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_LoadBGPltt(const void *pSrc, u32 offset, u32 szByte)
{
    SDK_NULL_ASSERT(pSrc);
    SDK_ASSERT(offset + szByte <= HW_BG_PLTT_SIZE);
    SDK_ALIGN2_ASSERT(offset);
    SDK_ALIGN2_ASSERT(szByte);

    GXi_DmaCopy16(GXi_DmaId, pSrc, (void *)(HW_BG_PLTT + offset), szByte);
}


/*---------------------------------------------------------------------------*
  Name:         GXS_LoadBGPltt

  Description:  Transfers BG palettes to the standard BG palette RAM
                for the SUB 2D engine.
                pSrc, offset, and szByte must be 2-bytes aligned.

  Arguments:    pSrc             a pointer to the source of BG palettes
                offset           an offset to HW_DB_BG_PLTT
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GXS_LoadBGPltt(const void *pSrc, u32 offset, u32 szByte)
{
    SDK_NULL_ASSERT(pSrc);
    SDK_ASSERT(offset + szByte <= HW_DB_BG_PLTT_SIZE);
    SDK_ALIGN2_ASSERT(offset);
    SDK_ALIGN2_ASSERT(szByte);

    GXi_DmaCopy16(GXi_DmaId, pSrc, (void *)(HW_DB_BG_PLTT + offset), szByte);
}


/*---------------------------------------------------------------------------*
  Name:         GX_LoadOBJPltt

  Description:  Transfers OBJ palettes to the standard OBJ palette RAM
                for the MAIN 2D engine.
                pSrc, offset, and szByte must be 2-bytes aligned.

  Arguments:    pSrc             a pointer to the source of OBJ palettes
                offset           an offset to HW_OBJ_PLTT
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_LoadOBJPltt(const void *pSrc, u32 offset, u32 szByte)
{
    SDK_NULL_ASSERT(pSrc);
    SDK_ASSERT(offset + szByte <= HW_OBJ_PLTT_SIZE);
    SDK_ALIGN2_ASSERT(offset);
    SDK_ALIGN2_ASSERT(szByte);

    GXi_DmaCopy16(GXi_DmaId, pSrc, (void *)(HW_OBJ_PLTT + offset), szByte);
}


/*---------------------------------------------------------------------------*
  Name:         GXS_LoadOBJPltt

  Description:  Transfers OBJ palettes to the standard OBJ palette RAM
                for the SUB 2D engine.
                pSrc, offset, and szByte must be 2-bytes aligned.

  Arguments:    pSrc             a pointer to the source of OBJ palettes
                offset           an offset to HW_DB_OBJ_PLTT
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GXS_LoadOBJPltt(const void *pSrc, u32 offset, u32 szByte)
{
    SDK_NULL_ASSERT(pSrc);
    SDK_ASSERT(offset + szByte <= HW_DB_OBJ_PLTT_SIZE);
    SDK_ALIGN2_ASSERT(offset);
    SDK_ALIGN2_ASSERT(szByte);

    GXi_DmaCopy16(GXi_DmaId, pSrc, (void *)(HW_DB_OBJ_PLTT + offset), szByte);
}


/*---------------------------------------------------------------------------*
  Name:         GX_LoadOAM

  Description:  Transfers object attributes to OAM for the MAIN 2D engine.
                pSrc, offset, and szByte must be 4-bytes aligned.

  Arguments:    pSrc             a pointer to the source of object attributes
                offset           an offset to HW_OAM
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_LoadOAM(const void *pSrc, u32 offset, u32 szByte)
{
    SDK_NULL_ASSERT(pSrc);
    SDK_ASSERT(offset + szByte <= HW_OAM_SIZE);
    SDK_ALIGN4_ASSERT(offset);
    SDK_ALIGN4_ASSERT(szByte);

    GXi_DmaCopy32(GXi_DmaId, pSrc, (void *)(HW_OAM + offset), szByte);
}


/*---------------------------------------------------------------------------*
  Name:         GXS_LoadOAM

  Description:  Transfers object attributes to OAM for the SUB 2D engine.
                pSrc, offset, and szByte must be 4-bytes aligned.

  Arguments:    pSrc             a pointer to the source of object attributes
                offset           an offset to HW_DB_OAM
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GXS_LoadOAM(const void *pSrc, u32 offset, u32 szByte)
{
    SDK_NULL_ASSERT(pSrc);
    SDK_ASSERT(offset + szByte <= HW_DB_OAM_SIZE);
    SDK_ALIGN4_ASSERT(offset);
    SDK_ALIGN4_ASSERT(szByte);

    GXi_DmaCopy32(GXi_DmaId, pSrc, (void *)(HW_DB_OAM + offset), szByte);
}


/*---------------------------------------------------------------------------*
  Name:         GX_LoadOBJ

  Description:  Transfers OBJ to OBJ-VRAM for the MAIN 2D engine.
                pSrc, offset, and szByte must be 4-bytes aligned.

  Arguments:    pSrc             a pointer to the source of OBJ data
                offset           an offset to HW_OBJ_VRAM
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_LoadOBJ(const void *pSrc, u32 offset, u32 szByte)
{
    u32     ptr;
    SDK_NULL_ASSERT(pSrc);
    SDK_ALIGN4_ASSERT(offset);
    SDK_ALIGN4_ASSERT(szByte);
    ptr = (u32)G2_GetOBJCharPtr();

    GX_RegionCheck_OBJ(ptr + offset, ptr + offset + szByte);

    GXi_DmaCopy32(GXi_DmaId, pSrc, (void *)(ptr + offset), szByte);
}


/*---------------------------------------------------------------------------*
  Name:         GXS_LoadOBJ

  Description:  Transfers OBJ to OBJ-VRAM for the SUB 2D engine.
                pSrc, offset, and szByte must be 4-bytes aligned.

  Arguments:    pSrc             a pointer to the source of OBJ palettes
                offset           an offset to HW_DB_OBJ_VRAM
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GXS_LoadOBJ(const void *pSrc, u32 offset, u32 szByte)
{
    u32     ptr;
    SDK_NULL_ASSERT(pSrc);
    SDK_ALIGN4_ASSERT(offset);
    SDK_ALIGN4_ASSERT(szByte);

    ptr = (u32)G2S_GetOBJCharPtr();

    GX_RegionCheck_SubOBJ(ptr + offset, ptr + offset + szByte);

    GXi_DmaCopy32(GXi_DmaId, pSrc, (void *)(ptr + offset), szByte);
}


/*---------------------------------------------------------------------------*
  Name:         GX_LoadBG0Scr

  Description:  Transfers screen data to BG #0 screen area for the MAIN 2D engine.
                pSrc, offset, and szByte must be 2-bytes aligned.

  Arguments:    pSrc             a pointer to the source of the BG screen data
                offset           an offset to the start address of BG #0 screen area
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_LoadBG0Scr(const void *pSrc, u32 offset, u32 szByte)
{
    u32     ptr;
    SDK_NULL_ASSERT(pSrc);
    SDK_ALIGN2_ASSERT(offset);
    SDK_ALIGN2_ASSERT(szByte);
    ptr = (u32)G2_GetBG0ScrPtr();

    GX_RegionCheck_BG(ptr + offset, ptr + offset + szByte);

    GXi_DmaCopy16(GXi_DmaId, pSrc, (void *)(ptr + offset), szByte);
}


/*---------------------------------------------------------------------------*
  Name:         GXS_LoadBG0Scr

  Description:  Transfers screen data to BG #0 screen area for the SUB 2D engine.
                pSrc, offset, and szByte must be 2-bytes aligned.

  Arguments:    pSrc             a pointer to the source of the BG screen data
                offset           an offset to the start address of BG #0 screen area
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GXS_LoadBG0Scr(const void *pSrc, u32 offset, u32 szByte)
{
    u32     ptr;
    SDK_NULL_ASSERT(pSrc);
    SDK_ALIGN2_ASSERT(offset);
    SDK_ALIGN2_ASSERT(szByte);
    ptr = (u32)G2S_GetBG0ScrPtr();

    GX_RegionCheck_SubBG(ptr + offset, ptr + offset + szByte);

    GXi_DmaCopy16(GXi_DmaId, pSrc, (void *)(ptr + offset), szByte);
}


/*---------------------------------------------------------------------------*
  Name:         GX_LoadBG1Scr

  Description:  Transfers screen data to BG #1 screen area for the MAIN 2D engine.
                pSrc, offset, and szByte must be 2-bytes aligned.

  Arguments:    pSrc             a pointer to the source of the BG screen data
                offset           an offset to the start address of BG #1 screen area
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_LoadBG1Scr(const void *pSrc, u32 offset, u32 szByte)
{
    u32     ptr;
    SDK_NULL_ASSERT(pSrc);
    SDK_ALIGN2_ASSERT(offset);
    SDK_ALIGN2_ASSERT(szByte);

    ptr = (u32)G2_GetBG1ScrPtr();

    GX_RegionCheck_BG(ptr + offset, ptr + offset + szByte);

    GXi_DmaCopy16(GXi_DmaId, pSrc, (void *)(ptr + offset), szByte);
}


/*---------------------------------------------------------------------------*
  Name:         GXS_LoadBG1Scr

  Description:  Transfers screen data to BG #1 screen area for the SUB 2D engine.
                pSrc, offset, and szByte must be 2-bytes aligned.

  Arguments:    pSrc             a pointer to the source of the BG screen data
                offset           an offset to the start address of BG #1 screen area
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GXS_LoadBG1Scr(const void *pSrc, u32 offset, u32 szByte)
{
    u32     ptr;
    SDK_NULL_ASSERT(pSrc);
    SDK_ALIGN2_ASSERT(offset);
    SDK_ALIGN2_ASSERT(szByte);

    ptr = (u32)G2S_GetBG1ScrPtr();

    GX_RegionCheck_SubBG(ptr + offset, ptr + offset + szByte);

    GXi_DmaCopy16(GXi_DmaId, pSrc, (void *)(ptr + offset), szByte);
}


/*---------------------------------------------------------------------------*
  Name:         GX_LoadBG2Scr

  Description:  Transfers screen data to BG #2 screen area for the MAIN 2D engine.
                pSrc, offset, and szByte must be 2-bytes aligned.

  Arguments:    pSrc             a pointer to the source of the BG screen data
                offset           an offset to the start address of BG #2 screen area
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_LoadBG2Scr(const void *pSrc, u32 offset, u32 szByte)
{
    u32     ptr;
    SDK_NULL_ASSERT(pSrc);
    SDK_ALIGN2_ASSERT(offset);
    SDK_ALIGN2_ASSERT(szByte);
    ptr = (u32)G2_GetBG2ScrPtr();

    GX_RegionCheck_BG(ptr + offset, ptr + offset + szByte);

    GXi_DmaCopy16(GXi_DmaId, pSrc, (void *)(ptr + offset), szByte);
}


/*---------------------------------------------------------------------------*
  Name:         GXS_LoadBG2Scr

  Description:  Transfers screen data to BG #2 screen area for the SUB 2D engine.
                pSrc, offset, and szByte must be 2-bytes aligned.

  Arguments:    pSrc             a pointer to the source of the BG screen data
                offset           an offset to the start address of BG #2 screen area
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GXS_LoadBG2Scr(const void *pSrc, u32 offset, u32 szByte)
{
    u32     ptr;
    SDK_NULL_ASSERT(pSrc);
    SDK_ALIGN2_ASSERT(offset);
    SDK_ALIGN2_ASSERT(szByte);

    ptr = (u32)G2S_GetBG2ScrPtr();

    GX_RegionCheck_SubBG(ptr + offset, ptr + offset + szByte);

    GXi_DmaCopy16(GXi_DmaId, pSrc, (void *)(ptr + offset), szByte);
}


/*---------------------------------------------------------------------------*
  Name:         GX_LoadBG3Scr

  Description:  Transfers screen data to BG #3 screen area for the MAIN 2D engine.
                pSrc, offset, and szByte must be 2-bytes aligned.

  Arguments:    pSrc             a pointer to the source of the BG screen data
                offset           an offset to the start address of BG #3 screen area
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_LoadBG3Scr(const void *pSrc, u32 offset, u32 szByte)
{
    u32     ptr;
    SDK_NULL_ASSERT(pSrc);
    SDK_ALIGN2_ASSERT(offset);
    SDK_ALIGN2_ASSERT(szByte);

    ptr = (u32)G2_GetBG3ScrPtr();

    GX_RegionCheck_BG(ptr + offset, ptr + offset + szByte);

    GXi_DmaCopy16(GXi_DmaId, pSrc, (void *)(ptr + offset), szByte);
}


/*---------------------------------------------------------------------------*
  Name:         GXS_LoadBG3Scr

  Description:  Transfers screen data to BG #3 screen area for the SUB 2D engine.
                pSrc, offset, and szByte must be 2-bytes aligned.

  Arguments:    pSrc             a pointer to the source of the BG screen data
                offset           an offset to the start address of BG #3 screen area
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GXS_LoadBG3Scr(const void *pSrc, u32 offset, u32 szByte)
{
    u32     ptr;
    SDK_NULL_ASSERT(pSrc);
    SDK_ALIGN2_ASSERT(offset);
    SDK_ALIGN2_ASSERT(szByte);

    ptr = (u32)G2S_GetBG3ScrPtr();

    GX_RegionCheck_SubBG(ptr + offset, ptr + offset + szByte);

    GXi_DmaCopy16(GXi_DmaId, pSrc, (void *)(ptr + offset), szByte);
}


/*---------------------------------------------------------------------------*
  Name:         GX_LoadBG0Char

  Description:  Transfers character data to BG #0 character area for the MAIN
                2D engine. pSrc, offset, and szByte must be 4-bytes aligned.

  Arguments:    pSrc             a pointer to the source of the BG character data
                offset           an offset to the start address of BG #0 character area
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_LoadBG0Char(const void *pSrc, u32 offset, u32 szByte)
{
    u32     ptr;
    SDK_NULL_ASSERT(pSrc);
    SDK_ALIGN4_ASSERT(offset);
    SDK_ALIGN4_ASSERT(szByte);

    ptr = (u32)G2_GetBG0CharPtr();

    GX_RegionCheck_BG(ptr + offset, ptr + offset + szByte);

    GXi_DmaCopy32(GXi_DmaId, pSrc, (void *)(ptr + offset), szByte);
}


/*---------------------------------------------------------------------------*
  Name:         GXS_LoadBG0Char

  Description:  Transfers character data to BG #0 character area for the SUB
                2D engine. pSrc, offset, and szByte must be 4-bytes aligned.

  Arguments:    pSrc             a pointer to the source of the BG character data
                offset           an offset to the start address of BG #0 character area
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GXS_LoadBG0Char(const void *pSrc, u32 offset, u32 szByte)
{
    u32     ptr;
    SDK_NULL_ASSERT(pSrc);
    SDK_ALIGN4_ASSERT(offset);
    SDK_ALIGN4_ASSERT(szByte);

    ptr = (u32)G2S_GetBG0CharPtr();

    GX_RegionCheck_SubBG(ptr + offset, ptr + offset + szByte);

    GXi_DmaCopy32(GXi_DmaId, pSrc, (void *)(ptr + offset), szByte);
}


/*---------------------------------------------------------------------------*
  Name:         GX_LoadBG1Char

  Description:  Transfers character data to BG #1 character area for the MAIN
                2D engine. pSrc, offset, and szByte must be 4-bytes aligned.

  Arguments:    pSrc             a pointer to the source of the BG character data
                offset           an offset to the start address of BG #1 character area
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_LoadBG1Char(const void *pSrc, u32 offset, u32 szByte)
{
    u32     ptr;
    SDK_NULL_ASSERT(pSrc);
    SDK_ALIGN4_ASSERT(offset);
    SDK_ALIGN4_ASSERT(szByte);

    ptr = (u32)G2_GetBG1CharPtr();

    GX_RegionCheck_BG(ptr + offset, ptr + offset + szByte);

    GXi_DmaCopy32(GXi_DmaId, pSrc, (void *)(ptr + offset), szByte);
}


/*---------------------------------------------------------------------------*
  Name:         GXS_LoadBG1Char

  Description:  Transfers character data to BG #1 character area for the SUB
                2D engine. pSrc, offset, and szByte must be 4-bytes aligned.

  Arguments:    pSrc             a pointer to the source of the BG character data
                offset           an offset to the start address of BG #1 character area
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GXS_LoadBG1Char(const void *pSrc, u32 offset, u32 szByte)
{
    u32     ptr;
    SDK_NULL_ASSERT(pSrc);
    SDK_ALIGN4_ASSERT(offset);
    SDK_ALIGN4_ASSERT(szByte);

    ptr = (u32)G2S_GetBG1CharPtr();

    GX_RegionCheck_SubBG(ptr + offset, ptr + offset + szByte);

    GXi_DmaCopy32(GXi_DmaId, pSrc, (void *)(ptr + offset), szByte);
}


/*---------------------------------------------------------------------------*
  Name:         GX_LoadBG2Char

  Description:  Transfers character data to BG #2 character area for the MAIN
                2D engine. pSrc, offset, and szByte must be 4-bytes aligned.

  Arguments:    pSrc             a pointer to the source of the BG character data
                offset           an offset to the start address of BG #2 character area
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_LoadBG2Char(const void *pSrc, u32 offset, u32 szByte)
{
    u32     ptr;
    SDK_NULL_ASSERT(pSrc);
    SDK_ALIGN4_ASSERT(offset);
    SDK_ALIGN4_ASSERT(szByte);

    ptr = (u32)G2_GetBG2CharPtr();

    GX_RegionCheck_BG(ptr + offset, ptr + offset + szByte);

    GXi_DmaCopy32(GXi_DmaId, pSrc, (void *)(ptr + offset), szByte);
}


/*---------------------------------------------------------------------------*
  Name:         GXS_LoadBG2Char

  Description:  Transfers character data to BG #2 character area for the SUB
                2D engine. pSrc, offset, and szByte must be 4-bytes aligned.

  Arguments:    pSrc             a pointer to the source of the BG character data
                offset           an offset to the start address of BG #2 character area
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GXS_LoadBG2Char(const void *pSrc, u32 offset, u32 szByte)
{
    u32     ptr;
    SDK_NULL_ASSERT(pSrc);
    SDK_ALIGN4_ASSERT(offset);
    SDK_ALIGN4_ASSERT(szByte);

    ptr = (u32)G2S_GetBG2CharPtr();

    GX_RegionCheck_SubBG(ptr + offset, ptr + offset + szByte);

    GXi_DmaCopy32(GXi_DmaId, pSrc, (void *)(ptr + offset), szByte);
}


/*---------------------------------------------------------------------------*
  Name:         GX_LoadBG3Char

  Description:  Transfers character data to BG #3 character area for the MAIN
                2D engine. pSrc, offset, and szByte must be 4-bytes aligned.

  Arguments:    pSrc             a pointer to the source of the BG character data
                offset           an offset to the start address of BG #3 character area
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_LoadBG3Char(const void *pSrc, u32 offset, u32 szByte)
{
    u32     ptr;
    SDK_NULL_ASSERT(pSrc);
    SDK_ALIGN4_ASSERT(offset);
    SDK_ALIGN4_ASSERT(szByte);

    ptr = (u32)G2_GetBG3CharPtr();

    GX_RegionCheck_BG(ptr + offset, ptr + offset + szByte);

    GXi_DmaCopy32(GXi_DmaId, pSrc, (void *)(ptr + offset), szByte);
}


/*---------------------------------------------------------------------------*
  Name:         GXS_LoadBG3Char

  Description:  Transfers character data to BG #3 character area for the SUB
                2D engine. pSrc, offset, and szByte must be 4-bytes aligned.

  Arguments:    pSrc             a pointer to the source of the BG character data
                offset           an offset to the start address of BG #3 character area
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GXS_LoadBG3Char(const void *pSrc, u32 offset, u32 szByte)
{
    u32     ptr;
    SDK_NULL_ASSERT(pSrc);
    SDK_ALIGN4_ASSERT(offset);
    SDK_ALIGN4_ASSERT(szByte);

    ptr = (u32)G2S_GetBG3CharPtr();

    GX_RegionCheck_SubBG(ptr + offset, ptr + offset + szByte);

    GXi_DmaCopy32(GXi_DmaId, pSrc, (void *)(ptr + offset), szByte);
}


//----------------------------------------------------------------------------
// Internal states for GX_BeginLoadBGExtPltt, GX_LoadBGExtPltt,
// and GX_EndLoadBGExtPltt
//----------------------------------------------------------------------------
static GXVRamBGExtPltt sBGExtPltt = (GXVRamBGExtPltt)(0);
static u32 sBGExtPlttLCDCBlk = 0;
static u32 sBGExtPlttLCDCOffset = 0;


/*---------------------------------------------------------------------------*
  Name:         GX_BeginLoadBGExtPltt

  Description:  Remaps the banks for MAIN 2D engine's BG extended palettes
                onto LCDC space, and prepares for palettes transfer
                by GX_LoadBGExtPltt.

  Arguments:    none

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_BeginLoadBGExtPltt()
{
    SDK_ASSERT(0 == sBGExtPltt && 0 == sBGExtPlttLCDCBlk && 0 == sBGExtPlttLCDCOffset);

    // BGExtPltt slots to LCDC
    sBGExtPltt = GX_ResetBankForBGExtPltt();

    switch (sBGExtPltt)
    {
    case GX_VRAM_BGEXTPLTT_0123_E:
        sBGExtPlttLCDCBlk = HW_LCDC_VRAM_E;
        sBGExtPlttLCDCOffset = 0;
        break;

    case GX_VRAM_BGEXTPLTT_23_G:
        sBGExtPlttLCDCBlk = HW_LCDC_VRAM_G;
        sBGExtPlttLCDCOffset = 0x4000;
        break;

    case GX_VRAM_BGEXTPLTT_0123_FG:
    case GX_VRAM_BGEXTPLTT_01_F:
        sBGExtPlttLCDCBlk = HW_LCDC_VRAM_F;
        sBGExtPlttLCDCOffset = 0;
        break;

    case GX_VRAM_BGEXTPLTT_NONE:
        break;

    default:
        SDK_INTERNAL_ERROR("unknown case 0x%x", sBGExtPltt);
        break;
    }
}


/*---------------------------------------------------------------------------*
  Name:         GX_LoadBGExtPltt

  Description:  Transfers BG extended palettes to the LCDC space which is
                remapped from BG extended palette slots. This function must be
                called between GX_BeginLoadBGExtPltt and GX_EndLoadBGExtPltt.
                pSrc, destSlotAddr, and szByte must be 4-bytes aligned.

  Arguments:    pSrc             a pointer to the source of BG extended palettes
                destSlotAddr     a destination address on the slots
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_LoadBGExtPltt(const void *pSrc, u32 destSlotAddr, u32 szByte)
{
    SDK_NULL_ASSERT(pSrc);
    SDK_ASSERT(sBGExtPltt != GX_VRAM_BGEXTPLTT_NONE);
    SDK_ASSERT(sBGExtPlttLCDCBlk != 0);
    SDK_ALIGN4_ASSERT(szByte);
    SDK_ALIGN4_ASSERT(destSlotAddr);
    SDK_ALIGN4_ASSERT(pSrc);

#ifdef SDK_DEBUG
    switch (sBGExtPltt)
    {
    case GX_VRAM_BGEXTPLTT_0123_E:
    case GX_VRAM_BGEXTPLTT_0123_FG:
        SDK_ASSERTMSG(destSlotAddr + szByte <= 0x8000,
                      "Illegal address/size specified(0x%5x->0x%5x)",
                      destSlotAddr, destSlotAddr + szByte);
        break;

    case GX_VRAM_BGEXTPLTT_23_G:
        SDK_ASSERTMSG(destSlotAddr + szByte <= 0x8000,
                      "Illegal address/size specified(0x%5x->0x%5x)",
                      destSlotAddr, destSlotAddr + szByte);
        SDK_ASSERTMSG(destSlotAddr >= 0x4000,
                      "BGExtPltt 0x0000 - 0x4000 not available(0x%5x->0x%5x)",
                      destSlotAddr, destSlotAddr + szByte);
        break;

    case GX_VRAM_BGEXTPLTT_01_F:
        SDK_ASSERTMSG(destSlotAddr + szByte <= 0x4000,
                      "BGExtPltt 0x4000 - 0x8000 not available(0x%5x->0x%5x)",
                      destSlotAddr, destSlotAddr + szByte);
        break;

    default:
        SDK_INTERNAL_ERROR("unknown case 0x%x", sBGExtPltt);
        break;
    }
#endif
    GXi_DmaCopy32Async(GXi_DmaId,
                       pSrc,
                       (void *)(sBGExtPlttLCDCBlk + destSlotAddr - sBGExtPlttLCDCOffset),
                       szByte, NULL, NULL);
}


/*---------------------------------------------------------------------------*
  Name:         GX_EndLoadBGExtPltt

  Description:  Restores the banks remapped by GX_BeginLoadBGExtPltt.
                This function is called after all the transfer is done.

  Arguments:    none

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_EndLoadBGExtPltt()
{
    GXi_WaitDma(GXi_DmaId);

    // Restore banks for BGExtPltt
    GX_SetBankForBGExtPltt(sBGExtPltt);

    sBGExtPltt = (GXVRamBGExtPltt)0;
    sBGExtPlttLCDCBlk = 0;
    sBGExtPlttLCDCOffset = 0;
}


//----------------------------------------------------------------------------
// Internal states for GX_BeginLoadOBJExtPltt, GX_LoadOBJExtPltt,
// and GX_EndLoadOBJExtPltt
//----------------------------------------------------------------------------
static GXVRamOBJExtPltt sOBJExtPltt = (GXVRamOBJExtPltt)(0);
static u32 sOBJExtPlttLCDCBlk = 0;


/*---------------------------------------------------------------------------*
  Name:         GX_BeginLoadOBJExtPltt

  Description:  Remaps the banks for MAIN 2D engine's OBJ extended palettes
                onto LCDC space, and prepares for palettes transfer
                by GX_LoadOBJExtPltt.

  Arguments:    none

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_BeginLoadOBJExtPltt()
{
    SDK_ASSERT(0 == sOBJExtPltt && 0 == sOBJExtPlttLCDCBlk);

    // OBJExtPltt slot to LCDC
    sOBJExtPltt = GX_ResetBankForOBJExtPltt();

    switch (sOBJExtPltt)
    {
    case GX_VRAM_OBJEXTPLTT_0_F:
        sOBJExtPlttLCDCBlk = HW_LCDC_VRAM_F;
        break;

    case GX_VRAM_OBJEXTPLTT_0_G:
        sOBJExtPlttLCDCBlk = HW_LCDC_VRAM_G;
        break;

    case GX_VRAM_OBJEXTPLTT_NONE:
        break;

    default:
        SDK_INTERNAL_ERROR("unknown case 0x%x", sOBJExtPltt);
        break;
    };
}


/*---------------------------------------------------------------------------*
  Name:         GX_LoadOBJExtPltt

  Description:  Transfers OBJ extended palettes to the LCDC space which is
                remapped from OBJ extended palette slots. This function must be
                called between GX_BeginLoadOBJExtPltt and GX_EndLoadOBJExtPltt.
                pSrc, destSlotAddr, and szByte must be 4-bytes aligned.

  Arguments:    pSrc             a pointer to the source of OBJ extended palettes
                destSlotAddr     a destination address on the slots
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_LoadOBJExtPltt(const void *pSrc, u32 destSlotAddr, u32 szByte)
{
    SDK_NULL_ASSERT(pSrc);
    SDK_ASSERT(GX_VRAM_OBJEXTPLTT_NONE != sOBJExtPltt);
    SDK_ASSERT(0 != sOBJExtPlttLCDCBlk);
    SDK_ALIGN4_ASSERT(szByte);
    SDK_ALIGN4_ASSERT(pSrc);
    SDK_ALIGN4_ASSERT(destSlotAddr);
    SDK_ASSERT(destSlotAddr + szByte <= 0x2000);        // size of OBJExtPltt slot 0

    GXi_DmaCopy32Async(GXi_DmaId,
                       pSrc, (void *)(sOBJExtPlttLCDCBlk + destSlotAddr), szByte, NULL, NULL);
}


/*---------------------------------------------------------------------------*
  Name:         GX_EndLoadOBJExtPltt

  Description:  Restores the banks remapped by GX_BeginLoadOBJExtPltt.
                This function is called after all the transfer is done.

  Arguments:    none

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_EndLoadOBJExtPltt()
{
    GXi_WaitDma(GXi_DmaId);

    // Restore banks for OBJExtPltt
    GX_SetBankForOBJExtPltt(sOBJExtPltt);

    sOBJExtPltt = (GXVRamOBJExtPltt)0;
    sOBJExtPlttLCDCBlk = 0;
}


//----------------------------------------------------------------------------
// Internal states for GXS_BeginLoadBGExtPltt, GXS_LoadBGExtPltt,
// and GXS_EndLoadBGExtPltt
//----------------------------------------------------------------------------
static GXVRamSubBGExtPltt sSubBGExtPltt = (GXVRamSubBGExtPltt)(0);


/*---------------------------------------------------------------------------*
  Name:         GXS_BeginLoadBGExtPltt

  Description:  Remaps the banks for SUB 2D engine's BG extended palettes
                onto LCDC space, and prepares for palettes transfer
                by GXS_LoadBGExtPltt.

  Arguments:    none

  Returns:      none
 *---------------------------------------------------------------------------*/
void GXS_BeginLoadBGExtPltt()
{
    SDK_ASSERT(0 == sSubBGExtPltt);

    // BGExtPltt slot to LCDC
    sSubBGExtPltt = GX_ResetBankForSubBGExtPltt();

    SDK_ASSERTMSG(GX_VRAM_SUB_BGEXTPLTT_0123_H == sSubBGExtPltt, "Sub BGExtPltt not allocated");
}


/*---------------------------------------------------------------------------*
  Name:         GXS_LoadBGExtPltt

  Description:  Transfers BG extended palettes to the LCDC space which is
                remapped from BG extended palette slots. This function must be
                called between GXS_BeginLoadBGExtPltt and GXS_EndLoadBGExtPltt.
                pSrc, destSlotAddr, and szByte must be 4-bytes aligned.

  Arguments:    pSrc             a pointer to the source of BG extended palettes
                destSlotAddr     a destination address on the slots
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GXS_LoadBGExtPltt(const void *pSrc, u32 destSlotAddr, u32 szByte)
{
    SDK_NULL_ASSERT(pSrc);
    SDK_ASSERT(GX_VRAM_SUB_BGEXTPLTT_0123_H == sSubBGExtPltt);
    SDK_ALIGN4_ASSERT(pSrc);
    SDK_ALIGN4_ASSERT(destSlotAddr);
    SDK_ALIGN4_ASSERT(szByte);

    SDK_ASSERTMSG(destSlotAddr + szByte <= 0x8000,
                  "Illegal address/size specified(0x%5x->0x%5x)",
                  destSlotAddr, destSlotAddr + szByte);

    GXi_DmaCopy32Async(GXi_DmaId,
                       pSrc, (void *)(HW_LCDC_VRAM_H + destSlotAddr), szByte, NULL, NULL);
}


/*---------------------------------------------------------------------------*
  Name:         GXS_EndLoadBGExtPltt

  Description:  Restores the banks remapped by GXS_BeginLoadBGExtPltt.
                This function is called after all the transfer is done.

  Arguments:    none

  Returns:      none
 *---------------------------------------------------------------------------*/
void GXS_EndLoadBGExtPltt()
{
    GXi_WaitDma(GXi_DmaId);

    // Restore banks for SubBGExtPltt
    GX_SetBankForSubBGExtPltt(sSubBGExtPltt);

    sSubBGExtPltt = (GXVRamSubBGExtPltt)0;
}


//----------------------------------------------------------------------------
// Internal states for GXS_BeginLoadOBJExtPltt, GXS_LoadOBJExtPltt,
// and GXS_EndLoadOBJExtPltt
//----------------------------------------------------------------------------
static GXVRamSubOBJExtPltt sSubOBJExtPltt = (GXVRamSubOBJExtPltt)(0);


/*---------------------------------------------------------------------------*
  Name:         GXS_BeginLoadOBJExtPltt

  Description:  Remaps the banks for SUB 2D engine's OBJ extended palettes
                onto LCDC space, and prepares for palettes transfer
                by GXS_LoadOBJExtPltt.

  Arguments:    none

  Returns:      none
 *---------------------------------------------------------------------------*/
void GXS_BeginLoadOBJExtPltt()
{
    SDK_ASSERT(0 == sSubOBJExtPltt);

    // BGExtPltt slot to LCDC
    sSubOBJExtPltt = GX_ResetBankForSubOBJExtPltt();

    SDK_ASSERTMSG(GX_VRAM_SUB_OBJEXTPLTT_0_I == sSubOBJExtPltt, "Sub OBJExtPltt not allocated");
}


/*---------------------------------------------------------------------------*
  Name:         GXS_LoadOBJExtPltt

  Description:  Transfers OBJ extended palettes to the LCDC space which is
                remapped from OBJ extended palette slots. This function must be
                called between GXS_BeginLoadOBJExtPltt and GXS_EndLoadOBJExtPltt.
                pSrc, destSlotAddr, and szByte must be 4-bytes aligned.

  Arguments:    pSrc             a pointer to the source of OBJ extended palettes
                destSlotAddr     a destination address on the slots
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GXS_LoadOBJExtPltt(const void *pSrc, u32 destSlotAddr, u32 szByte)
{
    SDK_NULL_ASSERT(pSrc);
    SDK_ASSERT(GX_VRAM_SUB_OBJEXTPLTT_0_I == sSubOBJExtPltt);
    SDK_ALIGN4_ASSERT(pSrc);
    SDK_ALIGN4_ASSERT(destSlotAddr);
    SDK_ALIGN4_ASSERT(szByte);

    SDK_ASSERTMSG(destSlotAddr + szByte <= 0x2000,
                  "Illegal address/size specified(0x%5x->0x%5x)",
                  destSlotAddr, destSlotAddr + szByte);

    GXi_DmaCopy32Async(GXi_DmaId,
                       pSrc, (void *)(HW_LCDC_VRAM_I + destSlotAddr), szByte, NULL, NULL);
}


/*---------------------------------------------------------------------------*
  Name:         GXS_EndLoadOBJExtPltt

  Description:  Restores the banks remapped by GXS_BeginLoadOBJExtPltt.
                This function is called after all the transfer is done.

  Arguments:    none

  Returns:      none
 *---------------------------------------------------------------------------*/
void GXS_EndLoadOBJExtPltt()
{
    GXi_WaitDma(GXi_DmaId);

    // Restore banks for OBJExtPltt
    GX_SetBankForSubOBJExtPltt(sSubOBJExtPltt);

    sSubOBJExtPltt = (GXVRamSubOBJExtPltt)0;
}
