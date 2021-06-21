/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - 
  File:     g3x.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g3x.c,v $
  Revision 1.75  2006/01/18 02:11:21  kitase_hirotake
  do-indent

  Revision 1.74  2005/03/01 01:57:00  yosizaki
  copyright ‚Ì”N‚ðC³.

  Revision 1.73  2005/02/28 05:26:13  yosizaki
  do-indent.

  Revision 1.72  2004/11/02 15:05:21  takano_makoto
  fix G3X_InitTable() clear size

  Revision 1.71  2004/10/22 13:08:54  takano_makoto
  LineBufferOverflow -> LineBufferUnderflow‚Ö‰ü–¼

  Revision 1.70  2004/08/10 12:48:35  takano_makoto
  make available to use GX_DMA_NOT_USE.

  Revision 1.69  2004/07/28 11:27:21  takano_makoto
  make possible to change DMA No.

  Revision 1.68  2004/05/11 07:56:47  takano_makoto
  fix bug in G3X_SetFog().

  Revision 1.67  2004/04/28 00:32:05  takano_makoto
  Add G3_End() in G3X_Init()

  Revision 1.66  2004/04/28 00:17:57  takano_makoto
  make GXi_NopClearFifo128_  code32.

  Revision 1.65  2004/04/27 11:28:20  takano_makoto
  change G3X_ClearFifo() to use send 128 NOP command.

  Revision 1.64  2004/03/17 07:26:43  yasu
  do sdk_indent

  Revision 1.63  2004/03/02 09:31:27  takano_makoto
  revise G3X_InitMtxStack, G3X_ResetMtxStack

  Revision 1.62  2004/02/17 10:43:52  nishida_kenji
  small fix.

  Revision 1.61  2004/02/12 11:09:02  yasu
  change to new location of include files ARM9/ARM7

  Revision 1.60  2004/02/10 06:03:15  nishida_kenji
  Move block copy functions to MI.

  Revision 1.59  2004/02/10 02:03:13  nishida_kenji
  Include mi/memory.h.

  Revision 1.58  2004/02/09 11:09:56  nishida_kenji
  Use functions at gxasm.h.

  Revision 1.57  2004/02/06 05:06:51  nishida_kenji
  Exec G3X_InitTable in G3X_Init().

  Revision 1.56  2004/02/06 00:55:05  nishida_kenji
  Add warnings(G3X_Init, G3X_Reset, G3X_InitMtxStack, G3X_ResetMtxStack).

  Revision 1.55  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.54  2004/01/30 02:15:51  nishida_kenji
  add comments

  Revision 1.53  2004/01/29 12:20:58  nishida_kenji
  small fix

  Revision 1.52  2004/01/29 08:43:53  nishida_kenji
  Revise G3X_Init and G3X_Reset.
  Also, use MI_**** for better performance and code size.

  Revision 1.51  2004/01/15 11:49:58  nishida_kenji
  revise GXFogBlend

  Revision 1.50  2004/01/14 11:34:21  nishida_kenji
  G3X_InitTables->G3X_InitTable

  Revision 1.49  2004/01/13 00:55:25  nishida_kenji
  revise G3X_Init()

  Revision 1.48  2003/12/25 11:00:09  nishida_kenji
  converted by GX_APIChangeFrom031212-2.pl

  Revision 1.47  2003/12/25 07:08:12  nishida_kenji
  FIFO->Fifo, RAM->Ram(function name)

  Revision 1.46  2003/12/25 06:25:02  nishida_kenji
  revise a part of geometry command APIs

  Revision 1.45  2003/12/24 08:17:58  nishida_kenji
  include fx/fx_const.h

  Revision 1.44  2003/12/24 06:35:58  nishida_kenji
  integrate g3x.h and g3x_status.h

  Revision 1.43  2003/12/24 06:30:19  nishida_kenji
  minor updates

  Revision 1.42  2003/12/18 00:08:45  nishida_kenji
  remove VecFx16

  Revision 1.41  2003/12/17 09:00:20  nishida_kenji
  Totally revised APIs.
  build/buildtools/GX_APIChangeFrom031212.pl would help you change your program to some extent.

  Revision 1.40  2003/12/17 08:53:17  nishida_kenji
  revise comments

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/gx/g3x.h>
#include <nitro/gx/g3imm.h>
#include <nitro/gx/gx.h>
#include <nitro/gx/gx_bgcnt.h>
#include <nitro/hw/ARM9/ioreg_GX.h>
#include <nitro/hw/ARM9/ioreg_G3.h>
#include <nitro/hw/ARM9/ioreg_G2.h>
#include <nitro/fx/fx_const.h>
#include <nitro/mi/dma.h>
#include <nitro/mi/memory.h>
#include "gxasm.h"


/*---------------------------------------------------------------------------*
  Static function prototype definition.
 *---------------------------------------------------------------------------*/
static asm void GXi_NopClearFifo128_( register void* pDest );


/*---------------------------------------------------------------------------*
  Name:         G3X_Init

  Description:  Initializes the registers for 3D graphics.

  Arguments:    none

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3X_Init()
{
#ifdef SDK_DEBUG
    {
        GXPower power = GX_GetPower();
        SDK_WARNING(power & GX_POWER_RE, "GX_POWER_RE is off now, registers for it not initialized.");
        SDK_WARNING(power & GX_POWER_GE, "GX_POWER_GE is off now, registers for it not initialized.");
    }
#endif

    G3X_ClearFifo();
    G3_End();
    
    while ( G3X_IsGeometryBusy() ) { }
    
    // clear registers before settings
    reg_G3X_DISP3DCNT = 0;
    reg_G3X_GXSTAT = 0;

    // Horizontal Offset
    reg_G2_BG0OFS = 0;

    // DISP3DCNT
    G3X_ResetListRamOverflow();
    G3X_ResetLineBufferUnderflow();
    G3X_SetShading(GX_SHADING_TOON);
    G3X_AntiAlias(TRUE);
    G3X_AlphaTest(FALSE, 0);

    // GXSTAT
    G3X_ResetMtxStackOverflow();

    G3X_SetFifoIntrCond(GX_FIFOINTR_COND_EMPTY);

    // matrix stack and GXSTAT
    G3X_InitMtxStack();

    // G3X_SetClearColor(GX_RGB(0, 0, 0), 0, 0x7fff, 0, FALSE);
    reg_G3X_CLEAR_COLOR = 0;
    reg_G3X_CLEAR_DEPTH = 0x7fff;

    // G3X_SetClearImageOffset(0, 0);
    reg_G3X_CLRIMAGE_OFFSET = 0;

    // G3X_SetFogColor(GX_RGB(0, 0, 0), 0);
    reg_G3X_FOG_COLOR = 0;

    // FOG_OFFSET
    reg_G3X_FOG_OFFSET = 0;

    // BG0CNT
    G2_SetBG0Priority(0);

    G3X_InitTable();

    // POLYGON_ATTR
    G3_PolygonAttr(GX_LIGHTMASK_NONE, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0, 31, GX_POLYGON_ATTR_MISC_NONE);

    // TEXIMAGE_PARAM
    G3_TexImageParam(GX_TEXFMT_NONE,
    
                     GX_TEXGEN_NONE,
                     GX_TEXSIZE_S8, GX_TEXSIZE_T8, GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, 0);

    // TEXPLTT_BASE
    G3_Direct1(G3OP_TEXPLTT_BASE, 0);
}


/*---------------------------------------------------------------------------*
  Name:         G3X_Reset

  Description:  Resets FIFO, matrix stacks, overflow flags,
                and attributes of polygons.

  Arguments:    none

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3X_Reset()
{
#ifdef SDK_DEBUG
    {
        GXPower power = GX_GetPower();
        SDK_WARNING(power & GX_POWER_RE,
                    "GX_POWER_RE is off now, registers for it not initialized.");
        SDK_WARNING(power & GX_POWER_GE,
                    "GX_POWER_GE is off now, registers for it not initialized.");
    }
#endif
    // DO NOT REMOVE THIS
    // Swapbuffers may be canceled if you remove.
    while (G3X_IsGeometryBusy())
    {
    }

    G3X_ResetMtxStackOverflow();
    G3X_ResetListRamOverflow();
    G3X_ResetLineBufferUnderflow();

    G3X_ResetMtxStack();
    // POLYGON_ATTR
    G3_PolygonAttr(GX_LIGHTMASK_NONE, GX_POLYGONMODE_MODULATE, GX_CULL_BACK, 0, 31,
                   GX_POLYGON_ATTR_MISC_NONE);

    // TEXIMAGE_PARAM
    G3_TexImageParam(GX_TEXFMT_NONE,
                     GX_TEXGEN_NONE,
                     GX_TEXSIZE_S8, GX_TEXSIZE_T8, GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE,
                     GX_TEXPLTTCOLOR0_USE, 0);

    // TEXPLTT_BASE
    G3_Direct1(G3OP_TEXPLTT_BASE, 0);
}



/*---------------------------------------------------------------------------*
  Name:         G3X_ClearFifo

  Description:  Clear FIFO

  Arguments:    none

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3X_ClearFifo(void)
{

    GXi_NopClearFifo128_((void *)&reg_G3X_GXFIFO);
//  G3_End();

    while (G3X_IsGeometryBusy())
    {
    }
}


/*---------------------------------------------------------------------------*
  Name:         G3X_InitMtxStack

  Description:  Initializes the level of matrix stacks and the current matrices.

  Arguments:    none

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3X_InitMtxStack()
{
    s32     levelPV;
    s32     levelPJ;
#ifdef SDK_DEBUG
    {
        GXPower power = GX_GetPower();
        SDK_WARNING(power & GX_POWER_GE,
                    "GX_POWER_GE is off now, registers for it not initialized.");
    }
#endif
    G3X_ResetMtxStackOverflow();

    while (G3X_GetMtxStackLevelPV(&levelPV))
        ;
    while (G3X_GetMtxStackLevelPJ(&levelPJ))
        ;

    G3_MtxMode(GX_MTXMODE_TEXTURE);
    G3_Identity();

    G3_MtxMode(GX_MTXMODE_PROJECTION);
    if (levelPJ != 0)
    {
        G3_PopMtx(levelPJ);
    }
    G3_Identity();

    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    G3_PopMtx(levelPV);
    G3_Identity();

}


/*---------------------------------------------------------------------------*
  Name:         G3X_ResetMtxStack

  Description:  Initializes the level of matrix stacks,
                and the current matrices except the current projection matrix.

  Arguments:    none

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3X_ResetMtxStack()
{
    s32     levelPV;
    s32     levelPJ;
#ifdef SDK_DEBUG
    {
        GXPower power = GX_GetPower();
        SDK_WARNING(power & GX_POWER_GE, "GX_POWER_GE is off now, registers for it not changes.");
    }
#endif
    G3X_ResetMtxStackOverflow();

    while (G3X_GetMtxStackLevelPV(&levelPV))
        ;
    while (G3X_GetMtxStackLevelPJ(&levelPJ))
        ;

    G3_MtxMode(GX_MTXMODE_TEXTURE);
    G3_Identity();

    G3_MtxMode(GX_MTXMODE_PROJECTION);
    if (levelPJ != 0)
    {
        G3_PopMtx(levelPJ);
    }
//    G3_Identity(); // Projection Matrix may not be set in a main loop

    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    G3_PopMtx(levelPV);
    G3_Identity();

}


/*---------------------------------------------------------------------------*
  Name:         G3X_SetFog

  Description:  Sets attributes for fog.

  Arguments:    enable       enables fog if not FALSE
                fogMode      fog blending manner(alpha only/color and alpha)
                fogSlope     slope of fog blending
                fogOffset    depth where fog blending starts

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3X_SetFog(BOOL enable, GXFogBlend fogMode, GXFogSlope fogSlope, int fogOffset)
{
    if (enable)
    {
        SDK_MINMAX_ASSERT(fogOffset, 0, 0x7fff);
        SDK_MINMAX_ASSERT(fogSlope, GX_FOGSLOPE_0x8000, GX_FOGSLOPE_0x0020);
        SDK_ASSERT(GX_FOGBLEND_COLOR_ALPHA == fogMode || GX_FOGBLEND_ALPHA == fogMode);

        reg_G3X_FOG_OFFSET = (u16)fogOffset;

        reg_G3X_DISP3DCNT = (u16)((reg_G3X_DISP3DCNT &
                                   ~(REG_G3X_DISP3DCNT_FOG_SHIFT_MASK
                                     | REG_G3X_DISP3DCNT_FMOD_MASK
                                     | REG_G3X_DISP3DCNT_RO_MASK
                                     | REG_G3X_DISP3DCNT_GO_MASK))
                                  | ((fogSlope << REG_G3X_DISP3DCNT_FOG_SHIFT_SHIFT)
                                     | (fogMode << REG_G3X_DISP3DCNT_FMOD_SHIFT)
                                     | (REG_G3X_DISP3DCNT_FME_MASK)));
    }
    else
    {
        reg_G3X_DISP3DCNT &= (u16)~(REG_G3X_DISP3DCNT_FME_MASK
                                    | REG_G3X_DISP3DCNT_RO_MASK | REG_G3X_DISP3DCNT_GO_MASK);
    }
}


/*---------------------------------------------------------------------------*
  Name:         G3X_GetClipMtx

  Description:  Gets a clip matrix.

  Arguments:    m            a pointer to a 4x4 matrix.

  Returns:      0 if the result is in *m,
                otherwise, the geometry engine is busy.
 *---------------------------------------------------------------------------*/
int G3X_GetClipMtx(MtxFx44 *m)
{
    SDK_NULL_ASSERT(m);

    if (G3X_IsGeometryBusy())
    {
        return -1;
    }
    else
    {
#if 1
        MI_Copy64B((void *)REG_CLIPMTX_RESULT_0_ADDR, &m->_00);
#else
        m->_00 = *((fx32 *)REG_CLIPMTX_RESULT_0_ADDR + 0);
        m->_01 = *((fx32 *)REG_CLIPMTX_RESULT_0_ADDR + 1);
        m->_02 = *((fx32 *)REG_CLIPMTX_RESULT_0_ADDR + 2);
        m->_03 = *((fx32 *)REG_CLIPMTX_RESULT_0_ADDR + 3);

        m->_10 = *((fx32 *)REG_CLIPMTX_RESULT_0_ADDR + 4);
        m->_11 = *((fx32 *)REG_CLIPMTX_RESULT_0_ADDR + 5);
        m->_12 = *((fx32 *)REG_CLIPMTX_RESULT_0_ADDR + 6);
        m->_13 = *((fx32 *)REG_CLIPMTX_RESULT_0_ADDR + 7);

        m->_20 = *((fx32 *)REG_CLIPMTX_RESULT_0_ADDR + 8);
        m->_21 = *((fx32 *)REG_CLIPMTX_RESULT_0_ADDR + 9);
        m->_22 = *((fx32 *)REG_CLIPMTX_RESULT_0_ADDR + 10);
        m->_23 = *((fx32 *)REG_CLIPMTX_RESULT_0_ADDR + 11);

        m->_30 = *((fx32 *)REG_CLIPMTX_RESULT_0_ADDR + 12);
        m->_31 = *((fx32 *)REG_CLIPMTX_RESULT_0_ADDR + 13);
        m->_32 = *((fx32 *)REG_CLIPMTX_RESULT_0_ADDR + 14);
        m->_33 = *((fx32 *)REG_CLIPMTX_RESULT_0_ADDR + 15);
#endif
        return 0;
    }
}


/*---------------------------------------------------------------------------*
  Name:         G3X_GetVectorMtx

  Description:  Gets a vector matrix.

  Arguments:    m            a pointer to a 3x3 matrix.

  Returns:      0 if the result is in *m,
                otherwise, the geometry engine is busy.
 *---------------------------------------------------------------------------*/
int G3X_GetVectorMtx(MtxFx33 *m)
{
    SDK_NULL_ASSERT(m);

    if (G3X_IsGeometryBusy())
    {
        return -1;
    }
    else
    {
#if 1
        MI_Copy36B((void *)(REG_VECMTX_RESULT_0_ADDR), &m->_00);
#else
        m->_00 = *((fx32 *)REG_VECMTX_RESULT_0_ADDR + 0);
        m->_01 = *((fx32 *)REG_VECMTX_RESULT_0_ADDR + 1);
        m->_02 = *((fx32 *)REG_VECMTX_RESULT_0_ADDR + 2);

        m->_10 = *((fx32 *)REG_VECMTX_RESULT_0_ADDR + 3);
        m->_11 = *((fx32 *)REG_VECMTX_RESULT_0_ADDR + 4);
        m->_12 = *((fx32 *)REG_VECMTX_RESULT_0_ADDR + 5);

        m->_20 = *((fx32 *)REG_VECMTX_RESULT_0_ADDR + 6);
        m->_21 = *((fx32 *)REG_VECMTX_RESULT_0_ADDR + 7);
        m->_22 = *((fx32 *)REG_VECMTX_RESULT_0_ADDR + 8);
#endif

        return 0;
    }
}


/*---------------------------------------------------------------------------*
  Name:         G3X_SetEdgeColorTable

  Description:  Sets a edge color table.

  Arguments:    m            a pointer to a the data to set(16 bytes length).

  Returns:      None.
 *---------------------------------------------------------------------------*/
void G3X_SetEdgeColorTable(const GXRgb *rgb_8)
{
    SDK_NULL_ASSERT(rgb_8);
#if 1
    MI_CpuCopy16(rgb_8, (void *)REG_EDGE_COLOR_0_L_ADDR, 16);
#else
    *((REGType16 *)REG_EDGE_COLOR_0_L_ADDR + 0) = (u16)(*(rgb_8 + 0));
    *((REGType16 *)REG_EDGE_COLOR_0_L_ADDR + 1) = (u16)(*(rgb_8 + 1));
    *((REGType16 *)REG_EDGE_COLOR_0_L_ADDR + 2) = (u16)(*(rgb_8 + 2));
    *((REGType16 *)REG_EDGE_COLOR_0_L_ADDR + 3) = (u16)(*(rgb_8 + 3));
    *((REGType16 *)REG_EDGE_COLOR_0_L_ADDR + 4) = (u16)(*(rgb_8 + 4));
    *((REGType16 *)REG_EDGE_COLOR_0_L_ADDR + 5) = (u16)(*(rgb_8 + 5));
    *((REGType16 *)REG_EDGE_COLOR_0_L_ADDR + 6) = (u16)(*(rgb_8 + 6));
    *((REGType16 *)REG_EDGE_COLOR_0_L_ADDR + 7) = (u16)(*(rgb_8 + 7));
#endif
}


/*---------------------------------------------------------------------------*
  Name:         G3X_SetFogTable

  Description:  Sets a fog table.

  Arguments:    fogTable     a pointer to the data to set(32 bytes length)

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3X_SetFogTable(const u32 *fogTable)
{
    SDK_NULL_ASSERT(fogTable);
#if 1
    MI_Copy32B(&fogTable[0], (void *)REG_FOG_TABLE_0_ADDR);
#else
    *((REGType32 *)REG_FOG_TABLE_0_ADDR + 0) = *(fogTable + 0);
    *((REGType32 *)REG_FOG_TABLE_0_ADDR + 1) = *(fogTable + 1);
    *((REGType32 *)REG_FOG_TABLE_0_ADDR + 2) = *(fogTable + 2);
    *((REGType32 *)REG_FOG_TABLE_0_ADDR + 3) = *(fogTable + 3);
    *((REGType32 *)REG_FOG_TABLE_0_ADDR + 4) = *(fogTable + 4);
    *((REGType32 *)REG_FOG_TABLE_0_ADDR + 5) = *(fogTable + 5);
    *((REGType32 *)REG_FOG_TABLE_0_ADDR + 6) = *(fogTable + 6);
    *((REGType32 *)REG_FOG_TABLE_0_ADDR + 7) = *(fogTable + 7);
#endif
}


/*---------------------------------------------------------------------------*
  Name:         G3X_SetToonTable

  Description:  Sets a toon table.

  Arguments:    rgb_32       a pointer to the data to set(64 bytes length)

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3X_SetToonTable(const GXRgb *rgb_32)
{
#if 1
    SDK_NULL_ASSERT(rgb_32);
    MI_CpuCopy16(rgb_32, (void *)REG_TOON_TABLE_0_L_ADDR, 64);
#else
    int     i;
    SDK_NULL_ASSERT(rgb_32);
    for (i = 0; i < 32; ++i)
    {
        *((REGType16 *)REG_TOON_TABLE_0_L_ADDR + i) = (u16)(*(rgb_32 + i));
    }
#endif
}


/*---------------------------------------------------------------------------*
  Name:         G3X_SetClearColor

  Description:  Sets a clear color, alpha, depth, polygonID

  Arguments:    rgb          clear color on the color buffer
                alpha        clear alpha on the color buffer
                depth        clear depth(15 bits) on the depth buffer
                polygonID    clear polygon ID(opaque) on the attribute buffer
                fog          fog enable flag on the attribute buffer
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void G3X_SetClearColor(GXRgb rgb, int alpha, int depth, int polygonID, BOOL fog)
{
    u32     val;
    GX_POLYGONID_ASSERT(polygonID);
    GX_ALPHA_ASSERT(alpha);
    GXRGB_ASSERT(rgb);
    GX_DEPTH_ASSERT(depth);

    val =
        (u32)(rgb | (alpha << REG_G3X_CLEAR_COLOR_ALPHA_SHIFT) |
              (polygonID << REG_G3X_CLEAR_COLOR_POLYGONID_SHIFT));
    if (fog)
        val |= REG_G3X_CLEAR_COLOR_F_MASK;

    reg_G3X_CLEAR_COLOR = val;
    reg_G3X_CLEAR_DEPTH = (u16)depth;
}


/*---------------------------------------------------------------------------*
  Name:         G3X_InitTable

  Description:  Initializes the edge color table, the fog table,
                the toon table, and the shininess table.

  Arguments:    none

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3X_InitTable()
{
    int     i;
#if 1
    if (GXi_DmaId != GX_DMA_NOT_USE)
    {
        MI_DmaFill32Async(GXi_DmaId, (void *)REG_EDGE_COLOR_0_L_ADDR, 0, 16, NULL, NULL);
        MI_DmaFill32(GXi_DmaId, (void *)REG_FOG_TABLE_0_ADDR, 0, 96);
    }
    else
    {
        MI_CpuFill32((void *)REG_EDGE_COLOR_0_L_ADDR, 0, 16);
        MI_CpuFill32((void *)REG_FOG_TABLE_0_ADDR, 0, 96);
    }
#else
    *((REGType16 *)REG_EDGE_COLOR_0_L_ADDR + 0) = 0;
    *((REGType16 *)REG_EDGE_COLOR_0_L_ADDR + 1) = 0;
    *((REGType16 *)REG_EDGE_COLOR_0_L_ADDR + 2) = 0;
    *((REGType16 *)REG_EDGE_COLOR_0_L_ADDR + 3) = 0;
    *((REGType16 *)REG_EDGE_COLOR_0_L_ADDR + 4) = 0;
    *((REGType16 *)REG_EDGE_COLOR_0_L_ADDR + 5) = 0;
    *((REGType16 *)REG_EDGE_COLOR_0_L_ADDR + 6) = 0;
    *((REGType16 *)REG_EDGE_COLOR_0_L_ADDR + 7) = 0;

    *((REGType32 *)REG_FOG_TABLE_0_ADDR + 0) = 0;
    *((REGType32 *)REG_FOG_TABLE_0_ADDR + 1) = 0;
    *((REGType32 *)REG_FOG_TABLE_0_ADDR + 2) = 0;
    *((REGType32 *)REG_FOG_TABLE_0_ADDR + 3) = 0;

    for (i = 0; i < 32; ++i)
    {
        *((REGType16 *)REG_TOON_TABLE_0_L_ADDR + i) = 0;
    }
#endif
    for (i = 0; i < 32; ++i)
    {
        reg_G3_SHININESS = 0;
    }
}


/*---------------------------------------------------------------------------*
  Name:         G3X_GetMtxStackLevelPV

  Description:  Get the current level of the Position/Vector matrix stack.

  Arguments:    level      a pointer to the current Position/Vector matrix
                           stack level

  Returns:      if 0, *level has the result(0 to 31),
                otherwise, the matrix stack is busy.
 *---------------------------------------------------------------------------*/
int G3X_GetMtxStackLevelPV(s32 *level)
{
    SDK_NULL_ASSERT(level);
    if (reg_G3X_GXSTAT & REG_G3X_GXSTAT_SB_MASK)
    {
        return -1;
    }
    else
    {
        *level = (s32)((reg_G3X_GXSTAT & REG_G3X_GXSTAT_PV_MASK) >> REG_G3X_GXSTAT_PV_SHIFT);
        return 0;
    }
}


/*---------------------------------------------------------------------------*
  Name:         G3X_GetMtxStackLevelPJ

  Description:  Get the current level of the Projection matrix stack.

  Arguments:    level      a pointer to the current Projection matrix
                           stack level

  Returns:      if 0, *level has the result(0 or 1),
                otherwise, the matrix stack is busy.
 *---------------------------------------------------------------------------*/
int G3X_GetMtxStackLevelPJ(s32 *level)
{
    SDK_NULL_ASSERT(level);
    if (reg_G3X_GXSTAT & REG_G3X_GXSTAT_SB_MASK)
    {
        return -1;
    }
    else
    {
        *level = (s32)((reg_G3X_GXSTAT & REG_G3X_GXSTAT_PJ_MASK) >> REG_G3X_GXSTAT_PJ_SHIFT);
        return 0;
    }
}


/*---------------------------------------------------------------------------*
  Name:         G3X_SetDisp1DotDepth

  Description:  If GX_POLYGON_ATTR_MISC_DISP_1DOT is set in G3*_PolygonAttr,
                the depth(w) value set by this function is the boundary
                to display or not to display the polygons which are mapped
                to less than one dot square.

  Arguments:    w          w-value(even if z-buffering mode) in fx32 format

  Returns:      None
 *---------------------------------------------------------------------------*/
void G3X_SetDisp1DotDepth(fx32 w)
{
    SDK_MINMAX_ASSERT(w, 0, FX32_ONE * 0x1000 - 1);
    reg_G3X_DISP_1DOT_DEPTH = (u16)((w >> 9) & 0x7fff);
}


/*---------------------------------------------------------------------------*
  Name:         G3X_GetBoxTestResult

  Description:  Obtain the result of boxtest.
                If *in is 0, the box is out the frustum.
                otherwise, the box is in/across the frustum.

  Arguments:    in         a pointer to the result
  
  Returns:      if 0, *in has the result.
                otherwise, the test is on the way.
 *---------------------------------------------------------------------------*/
int G3X_GetBoxTestResult(s32 *in)
{
    SDK_NULL_ASSERT(in);
    if (reg_G3X_GXSTAT & REG_G3X_GXSTAT_TB_MASK)
        return -1;
    else
    {
        *in = (s32)(reg_G3X_GXSTAT & REG_G3X_GXSTAT_TR_MASK);
        return 0;
    }
}


/*---------------------------------------------------------------------------*
  Name:         G3X_GetPositionTestResult

  Description:  Obtain the result of positiontest.

  Arguments:    vec, w     a pointer to the result

  Returns:      if 0, *vec, *w have the result.
                otherwise, the test is on the way.
 *---------------------------------------------------------------------------*/
int G3X_GetPositionTestResult(VecFx32 *vec, fx32 *w)
{
    SDK_NULL_ASSERT(vec);
    SDK_NULL_ASSERT(w);

    if (reg_G3X_GXSTAT & REG_G3X_GXSTAT_TB_MASK)
    {
        return -1;
    }
    else
    {
        vec->x = (fx32)reg_G3X_POS_RESULT_X;
        vec->y = (fx32)reg_G3X_POS_RESULT_Y;
        vec->z = (fx32)reg_G3X_POS_RESULT_Z;
        *w = (fx32)reg_G3X_POS_RESULT_W;
        return 0;
    }
}


/*---------------------------------------------------------------------------*
  Name:         G3X_GetVectorTestResult

  Description:  Obtain the result of vectortest.

  Arguments:    vec       a pointer to the result

  Returns:      if 0, *vec has the result.
                otherwise, the test is on the way.
 *---------------------------------------------------------------------------*/
int G3X_GetVectorTestResult(fx16 *vec)
{
    SDK_NULL_ASSERT(vec);
    if (reg_G3X_GXSTAT & REG_G3X_GXSTAT_TB_MASK)
    {
        return -1;
    }
    else
    {
        *(vec + 0) = (fx16)reg_G3X_VEC_RESULT_X;
        *(vec + 1) = (fx16)reg_G3X_VEC_RESULT_Y;
        *(vec + 2) = (fx16)reg_G3X_VEC_RESULT_Z;
        return 0;
    }
}


/*---------------------------------------------------------------------------*
  Name:         G3X_SetHOffset

  Description:  Sets a horizontal offset of 3D plane.

  Arguments:    hOffset      horizontal offset of 3D plane

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3X_SetHOffset(int hOffset)
{
    SDK_WARNING((reg_GX_DISPCNT & REG_GX_DISPCNT_BG02D3D_MASK) != 0,
                "BG0 is in 2D mode now. call \'G2_SetBG0Offset\'\n");
    SDK_ASSERT(hOffset >= -256 && hOffset <= 255);

    reg_G2_BG0OFS = (u32)hOffset;
}


#include <nitro/code32.h>

/*---------------------------------------------------------------------------*
  Name:         G3Xi_NopClearFifo_

  Description:  Set Nop for clear FIFO

  Arguments:    pDest  - pointer to Fifo command register.

  Returns:      none
 *---------------------------------------------------------------------------*/
static asm void GXi_NopClearFifo128_( register void* pDest )
{
    mov     r1,  #0         // r0-r3, r12 need not be saved.
    mov     r2,  #0
    mov     r3,  #0
    mov     r12, #0
    
    // set 128 NOP command for 4 PACKED Shineness command. (32 * 4 NOP)
    
    stmia   r0, {r1-r3, r12}    // 0 - 15
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}

    stmia   r0, {r1-r3, r12}    // 16 - 31
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}

    stmia   r0, {r1-r3, r12}    // 32 - 47
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}

    stmia   r0, {r1-r3, r12}    // 48 - 63
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}

    stmia   r0, {r1-r3, r12}    // 64 - 79
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}

    stmia   r0, {r1-r3, r12}    // 80 - 95
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}

    stmia   r0, {r1-r3, r12}    // 96 - 111
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}

    stmia   r0, {r1-r3, r12}    // 112 - 127
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}
    stmia   r0, {r1-r3, r12}
    
    bx      lr
}

#include <nitro/codereset.h>
