/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - 
  File:     gx_load3d.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: gx_load3d.c,v $
  Revision 1.17  2006/01/18 02:11:21  kitase_hirotake
  do-indent

  Revision 1.16  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.15  2005/02/28 05:26:13  yosizaki
  do-indent.

  Revision 1.14  2004/10/04 13:30:47  takano_makoto
  Add GX_VRAM_CLEARDEPTH_128_* to use clear depth without clear image.

  Revision 1.13  2004/08/10 12:48:35  takano_makoto
  make available to use GX_DMA_NOT_USE.

  Revision 1.12  2004/07/28 11:27:21  takano_makoto
  make possible to change DMA No.

  Revision 1.11  2004/07/09 05:51:37  takano_makoto
  GX_LoadTexEx、GX_LoadTexで非連続なVRAMバンクが割り当てられている場合の不具合追加修正

  Revision 1.10  2004/07/05 10:09:43  takano_makoto
  GX_LoadTexEx、GX_LoadTexで非連続なVRAMバンクが割り当てられている場合の不具合修正。

  Revision 1.9  2004/02/20 07:22:03  nishida_kenji
  Add GX_LoadTexEx and GX_LoadTexPlttEx.

  Revision 1.8  2004/02/17 10:43:52  nishida_kenji
  small fix.

  Revision 1.7  2004/02/13 07:08:42  nishida_kenji
  Use tables instead of switch.

  Revision 1.6  2004/02/12 11:09:02  yasu
  change to new location of include files ARM9/ARM7

  Revision 1.5  2004/02/09 06:36:53  nishida_kenji
  Copy data asynchronously.

  Revision 1.4  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.3  2004/01/28 05:41:31  nishida_kenji
  add comments and asserts.

  Revision 1.1  2003/12/25 11:53:57  nishida_kenji
  new

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/gx/gx_load.h>
#include <nitro/gx/gx_vramcnt.h>
#include <nitro/hw/ARM9/mmap_global.h>
#include <nitro/mi/dma.h>
#include "../include/gxstate.h"
#include "../include/gxdma.h"


//----------------------------------------------------------------------------
// Internal states for GX_BeginLoadTex, GX_LoadTex, and GX_EndLoadTex
//----------------------------------------------------------------------------
static u32 sTexLCDCBlk1 = 0;
static u32 sSzTexBlk1 = 0;
static u32 sTexLCDCBlk2 = 0;
static GXVRamTex sTex = (GXVRamTex)(0);


//----------------------------------------------------------------------------
// A table of start addresses in LCDC space.
// Used by GX_BeginLoadTex and GX_LoadTexEx.
//----------------------------------------------------------------------------
static const struct
{
    u16     blk1;                      // 12 bit shift
    u16     blk2;                      // 12 bit shift
    u16     szBlk1;                    // 12 bit shift
}
sTexStartAddrTable[16] =
{
    {
    0, 0, 0}
    ,                                  // GX_VRAM_TEX_NONE
    {
    (u16)(HW_LCDC_VRAM_A >> 12), 0, 0}
    ,                                  // GX_VRAM_TEX_0_A
    {
    (u16)(HW_LCDC_VRAM_B >> 12), 0, 0}
    ,                                  // GX_VRAM_TEX_0_B
    {
    (u16)(HW_LCDC_VRAM_A >> 12), 0, 0}
    ,                                  // GX_VRAM_TEX_01_AB
    {
    (u16)(HW_LCDC_VRAM_C >> 12), 0, 0}
    ,                                  // GX_VRAM_TEX_0_C
    {
    (u16)(HW_LCDC_VRAM_A >> 12), (u16)(HW_LCDC_VRAM_C >> 12), (u16)(HW_VRAM_A_SIZE >> 12)}
    ,                                  // GX_VRAM_TEX_01_AC
    {
    (u16)(HW_LCDC_VRAM_B >> 12), 0, 0}
    ,                                  // GX_VRAM_TEX_01_BC
    {
    (u16)(HW_LCDC_VRAM_A >> 12), 0, 0}
    ,                                  // GX_VRAM_TEX_012_ABC
    {
    (u16)(HW_LCDC_VRAM_D >> 12), 0, 0}
    ,                                  // GX_VRAM_TEX_0_D
    {
    (u16)(HW_LCDC_VRAM_A >> 12), (u16)(HW_LCDC_VRAM_D >> 12), (u16)(HW_VRAM_A_SIZE >> 12)}
    ,                                  // GX_VRAM_TEX_01_AD
    {
    (u16)(HW_LCDC_VRAM_B >> 12), (u16)(HW_LCDC_VRAM_D >> 12), (u16)(HW_VRAM_B_SIZE >> 12)}
    ,                                  // GX_VRAM_TEX_01_BD
    {
    (u16)(HW_LCDC_VRAM_A >> 12),
            (u16)(HW_LCDC_VRAM_D >> 12), (u16)((HW_VRAM_A_SIZE + HW_VRAM_B_SIZE) >> 12)}
    ,                                  // GX_VRAM_TEX_012_ABD
    {
    (u16)(HW_LCDC_VRAM_C >> 12), 0, 0}
    ,                                  // GX_VRAM_TEX_01_CD
    {
    (u16)(HW_LCDC_VRAM_A >> 12), (u16)(HW_LCDC_VRAM_C >> 12), (u16)(HW_VRAM_A_SIZE >> 12)}
    ,                                  // GX_VRAM_TEX_012_ACD
    {
    (u16)(HW_LCDC_VRAM_B >> 12), 0, 0}
    ,                                  // GX_VRAM_TEX_012_BCD
    {
    (u16)(HW_LCDC_VRAM_A >> 12), 0, 0}
    ,                                  // GX_VRAM_TEX_0123_ABCD
};


/*---------------------------------------------------------------------------*
  Name:         GX_LoadTexEx

  Description:  Transfers texture images to the LCDC space which are to be
                mapped to texture image slots. Note that banks specified
                by 'tex' must be mapped to LCDC.
                pSrc, destSlotAddr, and szByte must be 4-bytes aligned.
                Also, notice that DMA transfer is asynchronous.

  Arguments:    tex              a set of VRAM banks to be mapped to texture
                pSrc             a pointer to the source of texture images
                destSlotAddr     a destination address on the slots
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_LoadTexEx(GXVRamTex tex, const void *pSrc, u32 destSlotAddr, u32 szByte)
{
    u32     base1, base2, szBlk1;
    void   *pLCDC;

    SDK_ALIGN4_ASSERT(szByte);
    SDK_ALIGN4_ASSERT(destSlotAddr);
    SDK_ALIGN4_ASSERT(pSrc);
    SDK_ASSERTMSG((GX_GetBankForLCDC() & tex) == tex,
                  "Banks specified by tex must be on LCDC space.");
    GX_VRAM_TEX_ASSERT(tex);

    base1 = (u32)(sTexStartAddrTable[tex].blk1 << 12);
    base2 = (u32)(sTexStartAddrTable[tex].blk2 << 12);
    szBlk1 = (u32)(sTexStartAddrTable[tex].szBlk1 << 12);

    SDK_ASSERT(0 != base1);

    // This works only if NITRO_DEBUG is true.
    // It is declared in gxstate.h
    GX_RegionCheck_Tex(tex, destSlotAddr, destSlotAddr + szByte);

    if (0 == base2)
    {
        // continuous on LCDC address space
        pLCDC = (void *)(base1 + destSlotAddr);
    }
    else
    {
        // two discontinuous blocks on LCDC address space
        if (destSlotAddr + szByte < szBlk1)
        {
            pLCDC = (void *)(base1 + destSlotAddr);
        }
        else if (destSlotAddr >= szBlk1)
        {
            pLCDC = (void *)(base2 + destSlotAddr - szBlk1);
        }
        else
        {
            // cross the boundary
            void   *pLCDC2 = (void *)base2;
            u32     sz = szBlk1 - destSlotAddr;
            pLCDC = (void *)(base1 + destSlotAddr);

            GXi_DmaCopy32(GXi_DmaId, pSrc, pLCDC, sz);
            GXi_DmaCopy32Async(GXi_DmaId,
                               (void *)((u8 *)pSrc + sz), pLCDC2, szByte - sz, NULL, NULL);
            return;
        }
    }
    // DMA Transfer
    GXi_DmaCopy32Async(GXi_DmaId, pSrc, pLCDC, szByte, NULL, NULL);
}


/*---------------------------------------------------------------------------*
  Name:         GX_BeginLoadTex

  Description:  Remaps the banks for texture image slots onto LCDC space,
                and prepares for texture image transfer by GX_LoadTex.

  Arguments:    none

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_BeginLoadTex()
{
#if 1
    SDK_ASSERT(0 == sTex && 0 == sTexLCDCBlk1 && 0 == sSzTexBlk1 && 0 == sTexLCDCBlk2);

    // Texture slots to LCDC
    sTex = GX_ResetBankForTex();
    // check
    GX_VRAM_TEX_ASSERT(sTex);

    sTexLCDCBlk1 = (u32)(sTexStartAddrTable[sTex].blk1 << 12);
    sTexLCDCBlk2 = (u32)(sTexStartAddrTable[sTex].blk2 << 12);
    sSzTexBlk1 = (u32)(sTexStartAddrTable[sTex].szBlk1 << 12);

#else
    SDK_ASSERT(sTex == 0 && sTexLCDCBlk1 == 0 && sSzTexBlk1 == 0 && sTexLCDCBlk2 == 0);

    // Texture slots to LCDC
    sTex = GX_ResetBankForTex();

    switch (sTex)
    {
    case GX_VRAM_TEX_01_AC:
    case GX_VRAM_TEX_012_ACD:
        sTexLCDCBlk2 = HW_LCDC_VRAM_C;
        sSzTexBlk1 = HW_VRAM_A_SIZE;
        // do not break
    case GX_VRAM_TEX_0_A:
    case GX_VRAM_TEX_01_AB:
    case GX_VRAM_TEX_012_ABC:
    case GX_VRAM_TEX_0123_ABCD:
        sTexLCDCBlk1 = HW_LCDC_VRAM_A;
        break;

    case GX_VRAM_TEX_01_BD:
        sTexLCDCBlk2 = HW_LCDC_VRAM_D;
        sSzTexBlk1 = HW_VRAM_B_SIZE;
        // do not break
    case GX_VRAM_TEX_0_B:
    case GX_VRAM_TEX_01_BC:
    case GX_VRAM_TEX_012_BCD:
        sTexLCDCBlk1 = HW_LCDC_VRAM_B;
        break;

    case GX_VRAM_TEX_0_C:
    case GX_VRAM_TEX_01_CD:
        sTexLCDCBlk1 = HW_LCDC_VRAM_C;
        break;

    case GX_VRAM_TEX_0_D:
        sTexLCDCBlk1 = HW_LCDC_VRAM_D;
        break;

    case GX_VRAM_TEX_01_AD:
        sTexLCDCBlk1 = HW_LCDC_VRAM_A;
        sTexLCDCBlk2 = HW_LCDC_VRAM_D;
        sSzTexBlk1 = HW_VRAM_A_SIZE;
        break;

    case GX_VRAM_TEX_012_ABD:
        sTexLCDCBlk1 = HW_LCDC_VRAM_A;
        sTexLCDCBlk2 = HW_LCDC_VRAM_D;
        sSzTexBlk1 = HW_VRAM_A_SIZE + HW_VRAM_B_SIZE;
        break;

    case GX_VRAM_TEX_NONE:
        break;

    default:
        SDK_INTERNAL_ERROR("unknown case 0x%x", sTex);
        break;
    };
#endif
}


/*---------------------------------------------------------------------------*
  Name:         GX_LoadTex

  Description:  Transfers texture images to the LCDC space which is remapped
                from texture image slots. Two DMA transfers may be issued
                if there are discontinuous banks on LCDC space. This function
                must be called between GX_BeginLoadTex and GX_EndLoadTex.
                pSrc, destSlotAddr, and szByte must be 4-bytes aligned.

  Arguments:    pSrc             a pointer to the source of texture images
                destSlotAddr     a destination address on the slots
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_LoadTex(const void *pSrc, u32 destSlotAddr, u32 szByte)
{
    void   *pLCDC;
    SDK_NULL_ASSERT(pSrc);

    SDK_ASSERT(GX_VRAM_TEX_NONE != sTex);
    SDK_ASSERT(0 != sTexLCDCBlk1);
    SDK_ALIGN4_ASSERT(szByte);
    SDK_ALIGN4_ASSERT(destSlotAddr);
    SDK_ALIGN4_ASSERT(pSrc);

    // This works only if NITRO_DEBUG is true.
    // It is declared in gxstate.h
    GX_RegionCheck_Tex(sTex, destSlotAddr, destSlotAddr + szByte);

    if (0 == sTexLCDCBlk2)
    {
        // continuous on LCDC address space
        pLCDC = (void *)(sTexLCDCBlk1 + destSlotAddr);
    }
    else
    {
        // two discontinuous blocks on LCDC address space
        if (destSlotAddr + szByte < sSzTexBlk1)
        {
            pLCDC = (void *)(sTexLCDCBlk1 + destSlotAddr);
        }
        else if (destSlotAddr >= sSzTexBlk1)
        {
            pLCDC = (void *)(sTexLCDCBlk2 + destSlotAddr - sSzTexBlk1);
        }
        else
        {
            // cross the boundary
            void   *pLCDC2 = (void *)sTexLCDCBlk2;
            u32     sz = sSzTexBlk1 - destSlotAddr;
            pLCDC = (void *)(sTexLCDCBlk1 + destSlotAddr);

            GXi_DmaCopy32(GXi_DmaId, pSrc, pLCDC, sz);
            GXi_DmaCopy32Async(GXi_DmaId,
                               (void *)((u8 *)pSrc + sz), pLCDC2, szByte - sz, NULL, NULL);
            return;
        }
    }
    // DMA Transfer
    GXi_DmaCopy32Async(GXi_DmaId, pSrc, pLCDC, szByte, NULL, NULL);
}


/*---------------------------------------------------------------------------*
  Name:         GX_EndLoadTex

  Description:  Restores the banks remapped by GX_BeginLoadTex.
                This function is called after all the texture transfer is done.

  Arguments:    none

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_EndLoadTex()
{
    GXi_WaitDma(GXi_DmaId);

    // Restore banks for textures
    GX_SetBankForTex(sTex);

    sTexLCDCBlk1 = sTexLCDCBlk2 = sSzTexBlk1 = 0;
    sTex = (GXVRamTex)0;
}


//----------------------------------------------------------------------------
// Internal states for GX_BeginLoadTexPltt, GX_LoadTexPltt,
// and GX_EndLoadTexPltt
//----------------------------------------------------------------------------
static GXVRamTexPltt sTexPltt = (GXVRamTexPltt)(0);
static u32 sTexPlttLCDCBlk = 0;


//----------------------------------------------------------------------------
// A table of start addresses in LCDC space.
// Used by GX_BeginLoadTexPltt and GX_LoadTexPlttEx.
//----------------------------------------------------------------------------
static const u16 sTexPlttStartAddrTable[8] = {
    0,                                 // GX_VRAM_TEXPLTT_NONE
    (u16)(HW_LCDC_VRAM_E >> 12),       // GX_VRAM_TEXPLTT_0123_E
    (u16)(HW_LCDC_VRAM_F >> 12),       // GX_VRAM_TEXPLTT_0_F
    (u16)(HW_LCDC_VRAM_E >> 12),       // GX_VRAM_TEXPLTT_01234_EF
    (u16)(HW_LCDC_VRAM_G >> 12),       // GX_VRAM_TEXPLTT_0_G
    0,                                 // forbidden(GX_VRAM_TEXPLTT_0_EG)
    (u16)(HW_LCDC_VRAM_F >> 12),       // GX_VRAM_TEXPLTT_01_FG
    (u16)(HW_LCDC_VRAM_E >> 12)        // GX_VRAM_TEXPLTT_012345_EFG
};


/*---------------------------------------------------------------------------*
  Name:         GX_LoadTexPlttEx

  Description:  Transfers texture palettes to the LCDC space which are to be
                mapped to texture palette slots. Note that banks specified
                by 'texPltt' must be mapped to LCDC. 
                pSrc, destSlotAddr, and szByte must be 4-bytes aligned.
                Also, notice that DMA transfer is asynchronous.

  Arguments:    texPltt          a set of VRAM banks to be mapped to texture
                                 palette slots
                pSrc             a pointer to the source of texture palettes
                destSlotAddr     a destination address on the slots
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_LoadTexPlttEx(GXVRamTexPltt texPltt, const void *pSrc, u32 destSlotAddr, u32 szByte)
{
    u32     base;
    SDK_ASSERTMSG((GX_GetBankForLCDC() & texPltt) == texPltt,
                  "Banks specified by texPltt must be on LCDC space.");

    GX_VRAM_TEXPLTT_ASSERT(texPltt);
    base = (u32)(sTexPlttStartAddrTable[texPltt >> 4] << 12);

    SDK_NULL_ASSERT(pSrc);
    SDK_ASSERT(0 != base);
    SDK_ALIGN4_ASSERT(destSlotAddr);
    SDK_ALIGN4_ASSERT(szByte);
    SDK_ALIGN4_ASSERT(pSrc);

    // This works only if NITRO_DEBUG is true.
    // It is declared in gxstate.h
    GX_RegionCheck_TexPltt(texPltt, destSlotAddr, destSlotAddr + szByte);

    // DMA Transfer
    GXi_DmaCopy32Async(GXi_DmaId, pSrc, (void *)(base + destSlotAddr), szByte, NULL, NULL);
}


/*---------------------------------------------------------------------------*
  Name:         GX_BeginLoadTexPltt

  Description:  Remaps the banks for texture palette slots onto LCDC space,
                and prepares for texture palettes transfer by GX_LoadTexPltt.

  Arguments:    none

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_BeginLoadTexPltt()
{
#if 1
    SDK_ASSERT(0 == sTexPltt && 0 == sTexPlttLCDCBlk);

    // Texture pallet slot to LCDC
    sTexPltt = GX_ResetBankForTexPltt();

    GX_VRAM_TEXPLTT_ASSERT(sTexPltt);
    sTexPlttLCDCBlk = (u32)(sTexPlttStartAddrTable[sTexPltt >> 4] << 12);
#else
    SDK_ASSERT(sTexPltt == 0 && sTexPlttLCDCBlk == 0);

    // Texture pallet slot to LCDC
    sTexPltt = GX_ResetBankForTexPltt();

    switch (sTexPltt)
    {
    case GX_VRAM_TEXPLTT_0_F:
    case GX_VRAM_TEXPLTT_01_FG:
        sTexPlttLCDCBlk = HW_LCDC_VRAM_F;
        break;
    case GX_VRAM_TEXPLTT_0_G:
        sTexPlttLCDCBlk = HW_LCDC_VRAM_G;
        break;
    case GX_VRAM_TEXPLTT_0123_E:
    case GX_VRAM_TEXPLTT_01234_EF:
    case GX_VRAM_TEXPLTT_012345_EFG:
        sTexPlttLCDCBlk = HW_LCDC_VRAM_E;
        break;
    case GX_VRAM_TEXPLTT_NONE:
        break;
    default:
        SDK_INTERNAL_ERROR("unknown case 0x%x", sTexPltt);
        break;
    }
#endif
}


/*---------------------------------------------------------------------------*
  Name:         GX_LoadTexPltt

  Description:  Transfers texture palettes to the LCDC space which is remapped
                from texture palette slots. This function must be called
                between GX_BeginLoadTexPltt and GX_EndLoadTexPltt.
                pSrc, destSlotAddr, and szByte must be 4-bytes aligned.

  Arguments:    pSrc             a pointer to the source of texture palettes
                destSlotAddr     a destination address on the slots
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_LoadTexPltt(const void *pSrc, u32 destSlotAddr, u32 szByte)
{
    SDK_NULL_ASSERT(pSrc);
    SDK_ASSERT(GX_VRAM_TEXPLTT_NONE != sTexPltt);
    SDK_ASSERT(0 != sTexPlttLCDCBlk);
    SDK_ALIGN4_ASSERT(destSlotAddr);
    SDK_ALIGN4_ASSERT(szByte);
    SDK_ALIGN4_ASSERT(pSrc);

    // This works only if NITRO_DEBUG is true.
    // It is declared in gxstate.h
    GX_RegionCheck_TexPltt(sTexPltt, destSlotAddr, destSlotAddr + szByte);

    // DMA Transfer
    GXi_DmaCopy32Async(GXi_DmaId,
                       pSrc, (void *)(sTexPlttLCDCBlk + destSlotAddr), szByte, NULL, NULL);
}


/*---------------------------------------------------------------------------*
  Name:         GX_EndLoadTexPltt

  Description:  Restores the banks remapped by GX_BeginLoadTexPltt.
                This function is called after all the texture palette
                transfer is done.

  Arguments:    none

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_EndLoadTexPltt()
{
    GXi_WaitDma(GXi_DmaId);

    // Restore banks for texture pallets
    GX_SetBankForTexPltt(sTexPltt);

    sTexPltt = (GXVRamTexPltt)0;
    sTexPlttLCDCBlk = 0;
}


//----------------------------------------------------------------------------
// Internal states for GX_BeginLoadClearImage, GX_LoadClearImageColor,
// GX_LoadClearImageDepth, and GX_EndLoadClearImage.
//----------------------------------------------------------------------------
static GXVRamClearImage sClrImg = (GXVRamClearImage)(0);
static u32 sClrImgLCDCBlk = 0;


/*---------------------------------------------------------------------------*
  Name:         GX_BeginLoadClearImage

  Description:  Remaps the banks for a clear image onto LCDC space,
                and prepares for clear image transfer
                by GX_LoadClearImageColor and GX_LoadCleraImageDepth.

  Arguments:    none

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_BeginLoadClearImage()
{
    SDK_ASSERT(0 == sClrImg && 0 == sClrImgLCDCBlk);

    // Texture slot 2&3(Clear Image slots) to LCDC
    sClrImg = GX_ResetBankForClearImage();

    switch (sClrImg)
    {
    case GX_VRAM_CLEARIMAGE_256_AB:
    case GX_VRAM_CLEARDEPTH_128_B:
        sClrImgLCDCBlk = HW_LCDC_VRAM_A;
        break;

    case GX_VRAM_CLEARIMAGE_256_CD:
    case GX_VRAM_CLEARDEPTH_128_D:
        sClrImgLCDCBlk = HW_LCDC_VRAM_C;
        break;

    case GX_VRAM_CLEARDEPTH_128_A:
        sClrImgLCDCBlk = HW_LCDC_VRAM_A - 0x20000;
        break;

    case GX_VRAM_CLEARDEPTH_128_C:
        sClrImgLCDCBlk = HW_LCDC_VRAM_C - 0x20000;
        break;

    case GX_VRAM_CLEARIMAGE_NONE:
        break;

    default:
        SDK_INTERNAL_ERROR("unknown case 0x%x", sClrImg);
        break;
    }
}


/*---------------------------------------------------------------------------*
  Name:         GX_LoadClearImageColor

  Description:  Transfers a clear image to the LCDC space which is remapped
                from clear image slots. This function must be called
                between GX_BeginLoadClearImage and GX_EndLoadClearImage.
                pSrc and szByte must be 4-bytes aligned.

  Arguments:    pSrc             a pointer to the source of a clear color image
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_LoadClearImageColor(const void *pSrc, u32 szByte)
{
    SDK_NULL_ASSERT(pSrc);
    SDK_ASSERT(GX_VRAM_CLEARIMAGE_256_AB == sClrImg || GX_VRAM_CLEARIMAGE_256_CD == sClrImg);
    SDK_ASSERT(0 != sClrImgLCDCBlk);
    SDK_ALIGN4_ASSERT(szByte);
    SDK_ALIGN4_ASSERT(pSrc);
    SDK_ASSERT(szByte <= 0x20000 /* HW_VRAM_A_SIZE or HW_VRAM_C_SIZE */ );

    GXi_DmaCopy32Async(GXi_DmaId, pSrc, (void *)(sClrImgLCDCBlk), szByte, NULL, NULL);
}


/*---------------------------------------------------------------------------*
  Name:         GX_LoadClearImageDepth

  Description:  Transfers a clear depth to the LCDC space which is remapped
                from clear image slots. This function must be called
                between GX_BeginLoadClearImage and GX_EndLoadClearImage.
                pSrc and szByte must be 4-bytes aligned.

  Arguments:    pSrc             a pointer to the source of a clear depth image
                szByte           the size of the source

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_LoadClearImageDepth(const void *pSrc, u32 szByte)
{
    SDK_NULL_ASSERT(pSrc);
    SDK_ASSERT(GX_VRAM_CLEARIMAGE_NONE != sClrImg);
    SDK_ASSERT(0 != sClrImgLCDCBlk);
    SDK_ALIGN4_ASSERT(szByte);
    SDK_ALIGN4_ASSERT(pSrc);
    SDK_ASSERT(szByte <= 0x20000 /* HW_VRAM_A_SIZE or HW_VRAM_C_SIZE */ );

    GXi_DmaCopy32Async(GXi_DmaId,
                       pSrc,
                       (void *)(sClrImgLCDCBlk + 0x20000 /* HW_VRAM_A_SIZE or HW_VRAM_C_SIZE */ ),
                       szByte, NULL, NULL);
}


/*---------------------------------------------------------------------------*
  Name:         GX_EndLoadClearImage

  Description:  Restores the banks remapped by GX_BeginLoadTexPltt.
                This function is called after all the clear image transfer
                is done.

  Arguments:    none

  Returns:      none
 *---------------------------------------------------------------------------*/
void GX_EndLoadClearImage()
{
    GXi_WaitDma(GXi_DmaId);

    // Restore banks for texture pallets
    GX_SetBankForClearImage(sClrImg);

    sClrImg = (GXVRamClearImage)0;
    sClrImgLCDCBlk = 0;
}
