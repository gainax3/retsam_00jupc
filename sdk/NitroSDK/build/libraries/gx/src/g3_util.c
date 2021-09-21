/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - 
  File:     g3_util.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g3_util.c,v $
  Revision 1.29  2007/05/17 11:44:57  okubata_ryoma
  Copyright fix

  Revision 1.28  2007/04/24 12:00:16  yosizaki
  move matrix-functions' implementations to MTX.

  Revision 1.27  2006/01/18 02:11:21  kitase_hirotake
  do-indent

  Revision 1.26  2005/06/01 08:49:50  takano_makoto
  G3i_FrustumW_, G3i_PerspectiveW_, G3i_OrthoW_‚Ìnear, far‚Ì”ÍˆÍ‚ÉASSERT‚ðÝ’è

  Revision 1.25  2005/04/20 04:35:06  yosizaki
  fix about G3_FrustumW().

  Revision 1.24  2005/03/01 01:57:00  yosizaki
  copyright ‚Ì”N‚ðC³.

  Revision 1.23  2005/02/28 05:26:13  yosizaki
  do-indent.

  Revision 1.22  2004/06/21 10:27:00  takano_makoto
  integrate calcurate matrix functions,  G3_* MTX_*. G3_*W, MTX_*W.

  Revision 1.21  2004/05/06 10:47:09  takano_makoto
  Modify include file.

  Revision 1.20  2004/04/27 01:15:46  takano_makoto
  Add G3_FrustumW, G3_PerspectiveW, G3_OrthoW.

  Revision 1.19  2004/03/17 07:23:40  yasu
  do sdk_indent

  Revision 1.18  2004/02/18 01:01:16  yasu
  add comment for code32.h

  Revision 1.17  2004/02/17 10:43:52  nishida_kenji
  small fix.

  Revision 1.16  2004/02/17 05:32:26  nishida_kenji
  small fix.

  Revision 1.15  2004/02/12 11:09:02  yasu
  change to new location of include files ARM9/ARM7

  Revision 1.14  2004/02/12 07:06:27  nishida_kenji
  Avoid generating STRB in THUMB mode.

  Revision 1.13  2004/02/12 02:11:45  nishida_kenji
  small fix.

  Revision 1.12  2004/02/10 12:42:45  kitani_toshikazu
  Fix the bug in G3CS_LoadTexMtxEnv.

  Revision 1.11  2004/02/10 12:36:07  kitani_toshikazu
  Fix the bug in G3CS_LoadTexMtxTexCoord().

  Revision 1.9  2004/02/05 12:08:47  nishida_kenji
  Use MI_CpuSend32.

  Revision 1.8  2004/02/05 09:38:40  kitani_toshikazu
  fixed bugs in G3_Ortho and G3_Frustum().

  Revision 1.7  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.6  2004/01/29 12:19:23  nishida_kenji
  Add asserts and comments, and use MI_CpuCopyFast for better performance and code size.

  Revision 1.5  2004/01/18 04:39:50  nishida_kenji
  revises the interfaces for asynchronous calculation

  Revision 1.4  2004/01/16 05:01:20  nishida_kenji
  add G3xx_LoadTexMtxTexCoord and G3xx_LoadTexMtxEnv

  Revision 1.3  2004/01/10 07:45:25  nishida_kenji
  fix G3_LoadTexMtxTexCoord()

  Revision 1.2  2004/01/10 01:58:18  nishida_kenji
  add G3_LoadTexMtxTexCoord(), G3_LoadTexMtxEnv(),
  and revise G3_LoadTexMtx().

  Revision 1.1  2003/12/25 12:01:28  nishida_kenji
  renamed

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/gx/g3_util.h>
#include <nitro/gx/g3imm.h>
#include <nitro/gx/g3b.h>
#include <nitro/gx/g3c.h>
#include <nitro/gx/gx_vramcnt.h>
#include <nitro/fx/fx_cp.h>
#include <nitro/fx/fx_vec.h>
#include <nitro/hw/ARM9/mmap_global.h>
#include <nitro/mi/dma.h>
#include <nitro/mi/memory.h>



/*---------------------------------------------------------------------------*
  Name:         G3i_FrustumW_

  Description:  Computes a 4x4 perspective projection matrix from a specified
                view volume, and Load it to the current projection matrix if draw flag is TRUE.

  Arguments:    t            top coordinate of view volume at the near clipping plane
                b            bottom coordinate of view volume at the near clipping plane
                l            left coordinate of view volume at near clipping plane
                r            right coordinate of view volume at near clipping plane
                n            positive distance from camera to near clipping plane
                f            positive distance from camera to far clipping plane
                draw         if TRUE load to projection matrix
                mtx          4x4 matrix to be set if not NULL

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3i_FrustumW_(fx32 t, fx32 b, fx32 l, fx32 r, fx32 n, fx32 f, fx32 scaleW, BOOL draw,
                   MtxFx44 *mtx)
{
    MtxFx44 tmp[1];
    if (mtx == NULL)
    {
        mtx = tmp;
    }
    MTX_FrustumW(t, b, l, r, n, f, scaleW, mtx);
    if (draw)
    {
        G3_MtxMode(GX_MTXMODE_PROJECTION);
        G3_LoadMtx44(mtx);
    }
}


/*---------------------------------------------------------------------------*
  Name:         G3i_PerspectiveW_

  Description:  Computes a 4x4 perspective projection matrix from field of
                view and aspect ratio, and Load it to the current projection
                matrix if draw flag is TRUE.

  Arguments:    fovySin      sine value of fovy
                fovyCos      cosine value of fovy
                aspect       ratio of view window width:height (X / Y)
                n            positive distance from camera to near clipping plane
                f            positive distance from camera to far clipping plane
                scaleW       W scale parameter that adjust precision of view volume.
                draw         if TRUE load to projection matrix
                mtx          4x4 matrix to be set if not NULL

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3i_PerspectiveW_(fx32 fovySin, fx32 fovyCos, fx32 aspect, fx32 n, fx32 f, fx32 scaleW,
                       BOOL draw, MtxFx44 *mtx)
{
    MtxFx44 tmp[1];
    if (mtx == NULL)
    {
        mtx = tmp;
    }
    MTX_PerspectiveW(fovySin, fovyCos, aspect, n, f, scaleW, mtx);
    if (draw)
    {
        G3_MtxMode(GX_MTXMODE_PROJECTION);
        G3_LoadMtx44(mtx);
    }
}


/*---------------------------------------------------------------------------*
  Name:         G3i_OrthoW_

  Description:  Computes a 4x4 orthographic projection matrix, and Load it
                to the current projection matrix if draw flag is TRUE.

  Arguments:    t            top coordinate of parallel view volume
                b            bottom coordinate of parallel view volume
                l            left coordinate of parallel view volume
                r            right coordinate of parallel view volume
                n            positive distance from camera to near clipping plane
                f            positive distance from camera to far clipping plane
                scaleW       W scale parameter that adjust precision of view volume.
                draw         if TRUE load to projection matrix
                mtx          4x4 matrix to be set if not NULL

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3i_OrthoW_(fx32 t, fx32 b, fx32 l, fx32 r, fx32 n, fx32 f, fx32 scaleW, BOOL draw,
                 MtxFx44 *mtx)
{
    MtxFx44 tmp[1];
    if (mtx == NULL)
    {
        mtx = tmp;
    }
    MTX_OrthoW(t, b, l, r, n, f, scaleW, mtx);
    if (draw)
    {
        G3_MtxMode(GX_MTXMODE_PROJECTION);
        G3_LoadMtx44(mtx);
    }
}



/*---------------------------------------------------------------------------*
  Name:         G3i_LookAt_

  Description:  Computes a matrix to transform points to camera coordinates,
                and load it to the current position/vector matrix if draw flag is TRUE.

  Arguments:    camPos       camera position
                camUp        camera 'up' direction
                target       camera aim point
                draw         if TRUE load to position vector matrix
                mtx          a 4x3 matrix to be set if not NULL

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3i_LookAt_(const VecFx32 *camPos, const VecFx32 *camUp, const VecFx32 *target, BOOL draw,
                 MtxFx43 *mtx)
{
    MtxFx43 tmp[1];
    if (mtx == NULL)
    {
        mtx = tmp;
    }
    MTX_LookAt(camPos, camUp, target, mtx);
    if (draw)
    {
        G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
        G3_LoadMtx43(mtx);
    }
}


/*---------------------------------------------------------------------------*
  Name:         G3_RotX

  Description:  Multiplies the current matrix by a rotation matrix about X axis.

  Arguments:    s            sine of rotation angle
                c            cosine of rotation angle

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3_RotX(fx32 s, fx32 c)
{
    vs32   *p = (vs32 *)&reg_G3_MTX_MULT_3x3;

    SDK_MINMAX_ASSERT(s, -FX32_ONE, FX32_ONE);
    SDK_MINMAX_ASSERT(c, -FX32_ONE, FX32_ONE);

    *p = FX32_ONE;                     // _00
    *p = 0;                            // _01
    *p = 0;                            // _02
    *p = 0;                            // _10
    *p = c;                            // _11
    *p = s;                            // _12
    *p = 0;                            // _20
    *p = -s;                           // _21
    *p = c;                            // _22
}


/*---------------------------------------------------------------------------*
  Name:         G3_RotY

  Description:  Multiplies the current matrix by a rotation matrix about Y axis.

  Arguments:    s            sine of rotation angle
                c            cosine of rotation angle

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3_RotY(fx32 s, fx32 c)
{
    vs32   *p = (vs32 *)&reg_G3_MTX_MULT_3x3;

    SDK_MINMAX_ASSERT(s, -FX32_ONE, FX32_ONE);
    SDK_MINMAX_ASSERT(c, -FX32_ONE, FX32_ONE);

    *p = c;                            // _00
    *p = 0;                            // _01
    *p = -s;                           // _02
    *p = 0;                            // _10
    *p = FX32_ONE;                     // _11
    *p = 0;                            // _12
    *p = s;                            // _20
    *p = 0;                            // _21
    *p = c;                            // _22
}


/*---------------------------------------------------------------------------*
  Name:         G3_RotZ

  Description:  Multiplies the current matrix by a rotation matrix about Z axis.

  Arguments:    s            sine of rotation matrix
                c            cosine of rotation matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3_RotZ(fx32 s, fx32 c)
{
    vs32   *p = (vs32 *)&reg_G3_MTX_MULT_3x3;

    SDK_MINMAX_ASSERT(s, -FX32_ONE, FX32_ONE);
    SDK_MINMAX_ASSERT(c, -FX32_ONE, FX32_ONE);

    *p = c;                            // _00
    *p = s;                            // _01
    *p = 0;                            // _02
    *p = -s;                           // _10
    *p = c;                            // _11
    *p = 0;                            // _12
    *p = 0;                            // _20
    *p = 0;                            // _21
    *p = FX32_ONE;                     // _22
}


/*---------------------------------------------------------------------------*
  Name:         G3_LoadTexMtxTexCoord

  Description:  Loads a texture matrix to the current one on the geometry engine,
                adjusting the elements for TexCoord source.

  Arguments:    mtx          a pointer to a 4x4 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3_LoadTexMtxTexCoord(const MtxFx44 *mtx)
{
    vs32   *p = (vs32 *)&reg_G3_MTX_LOAD_4x4;
    SDK_NULL_ASSERT(mtx);
    G3_MtxMode(GX_MTXMODE_TEXTURE);

    *p = mtx->_00;
    *p = mtx->_01;
    *p = mtx->_02;
    *p = mtx->_03;
    *p = mtx->_10;
    *p = mtx->_11;
    *p = mtx->_12;
    *p = mtx->_13;
    *p = mtx->_20 << 4;
    *p = mtx->_21 << 4;
    *p = mtx->_22 << 4;
    *p = mtx->_23 << 4;
    *p = mtx->_30 << 4;
    *p = mtx->_31 << 4;
    *p = mtx->_32 << 4;
    *p = mtx->_33 << 4;
}


static void G3xx_LoadTexMtxTexCoord_(u32 *param, const MtxFx44 *mtx)
{
#if 1
    MI_Copy32B(&mtx->_00, (void *)(param + 2));
#else
    *(param + 2) = (u32)(mtx->_00);
    *(param + 3) = (u32)(mtx->_01);
    *(param + 4) = (u32)(mtx->_02);
    *(param + 5) = (u32)(mtx->_03);
    *(param + 6) = (u32)(mtx->_10);
    *(param + 7) = (u32)(mtx->_11);
    *(param + 8) = (u32)(mtx->_12);
    *(param + 9) = (u32)(mtx->_13);
#endif
    *(param + 10) = (u32)(mtx->_20 << 4);
    *(param + 11) = (u32)(mtx->_21 << 4);
    *(param + 12) = (u32)(mtx->_22 << 4);
    *(param + 13) = (u32)(mtx->_23 << 4);
    *(param + 14) = (u32)(mtx->_30 << 4);
    *(param + 15) = (u32)(mtx->_31 << 4);
    *(param + 16) = (u32)(mtx->_32 << 4);
    *(param + 17) = (u32)(mtx->_33 << 4);
}


/*---------------------------------------------------------------------------*
  Name:         G3BS_LoadTexMtxTexCoord

  Description:  Generates a display list code without command packing,
                to load a texture matrix to the current one on the geometry
                engine, adjusting the elements for TexCoord source.

  Arguments:    info         a pointer to display list info
                mtx          a pointer to a 4x4 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3BS_LoadTexMtxTexCoord(GXDLInfo *info, const MtxFx44 *mtx)
{
    SDK_NULL_ASSERT(mtx);
    SDK_NULL_ASSERT(info);

    *(u32 *)info->curr_cmd = (u32)G3OP_MTX_MODE;
    *info->curr_param = (u32)(GX_MTXMODE_TEXTURE << REG_G3_MTX_MODE_M_SHIFT);
    *(info->curr_param + 1) = (u32)G3OP_MTX_LOAD_4x4;

    G3xx_LoadTexMtxTexCoord_(info->curr_param, mtx);
}


/*---------------------------------------------------------------------------*
  Name:         G3CS_LoadTexMtxTexCoord

  Description:  Generates a display list code with command packing,
                to load a texture matrix to the current one on the geometry
                engine, adjusting the elements for TexCoord source.
  
  Arguments:    info         a pointer to display list info
                mtx          a pointer to a 4x4 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>              // avoid byte access problems
void G3CS_LoadTexMtxTexCoord(GXDLInfo *info, const MtxFx44 *mtx)
{
    SDK_NULL_ASSERT(info);
    SDK_NULL_ASSERT(mtx);

    // padding(notice that info->curr_cmd and info->curr_param change)
    switch ((u32)info->curr_cmd & 3)
    {
        // padding
    case 0:
        // DO NOTHING
        break;
    case 1:
        *info->curr_cmd++ = 0;         // byte access
    case 2:
        *info->curr_cmd++ = 0;
    case 3:
        *info->curr_cmd++ = 0;
        ++info->curr_param;
    };

    *(u32 *)info->curr_cmd = (G3OP_MTX_LOAD_4x4 << 8) | G3OP_MTX_MODE;
    *info->curr_param = (u32)(GX_MTXMODE_TEXTURE << REG_G3_MTX_MODE_M_SHIFT);
    *(info->curr_param + 1) = (u32)G3OP_MTX_LOAD_4x4;

    G3xx_LoadTexMtxTexCoord_(info->curr_param, mtx);
}

#include <nitro/codereset.h>

/*---------------------------------------------------------------------------*
  Name:         G3_LoadTexMtxEnv

  Description:  Loads a texture matrix to the current one on the geometry engine,
                adjusting the elements for Normal/Vertex source.

  Arguments:    mtx          a pointer to a 4x4 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3_LoadTexMtxEnv(const MtxFx44 *mtx)
{
    vs32   *p = (vs32 *)&reg_G3_MTX_LOAD_4x4;
    SDK_NULL_ASSERT(mtx);
    G3_MtxMode(GX_MTXMODE_TEXTURE);

    // The input must be (x16, x16, x16) scaled when you set a texture matrix
    *p = mtx->_00 << 4;
    *p = mtx->_01 << 4;
    *p = mtx->_02 << 4;
    *p = mtx->_03 << 4;
    *p = mtx->_10 << 4;
    *p = mtx->_11 << 4;
    *p = mtx->_12 << 4;
    *p = mtx->_13 << 4;
    *p = mtx->_20 << 4;
    *p = mtx->_21 << 4;
    *p = mtx->_22 << 4;
    *p = mtx->_23 << 4;
    *p = mtx->_30;
    *p = mtx->_31;
    *p = mtx->_32;
    *p = mtx->_33;
}


/*---------------------------------------------------------------------------*
  Name:         G3BS_LoadTexMtxEnv

  Description:  Generates a display list code without command packing,
                to load a texture matrix to the current one on the geometry
                engine, adjusting the elements for Normal/Vertex source.

  Arguments:    info         a pointer to display list info
                mtx          a pointer to a 4x4 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3BS_LoadTexMtxEnv(GXDLInfo *info, const MtxFx44 *mtx)
{
    SDK_NULL_ASSERT(info);
    SDK_NULL_ASSERT(mtx);

    *(u32 *)info->curr_cmd = (u32)G3OP_MTX_MODE;
    *info->curr_param = (u32)(GX_MTXMODE_TEXTURE << REG_G3_MTX_MODE_M_SHIFT);
    *(info->curr_param + 1) = (u32)G3OP_MTX_LOAD_4x4;

    *(info->curr_param + 2) = (u32)(mtx->_00 << 4);
    *(info->curr_param + 3) = (u32)(mtx->_01 << 4);
    *(info->curr_param + 4) = (u32)(mtx->_02 << 4);
    *(info->curr_param + 5) = (u32)(mtx->_03 << 4);
    *(info->curr_param + 6) = (u32)(mtx->_10 << 4);
    *(info->curr_param + 7) = (u32)(mtx->_11 << 4);
    *(info->curr_param + 8) = (u32)(mtx->_12 << 4);
    *(info->curr_param + 9) = (u32)(mtx->_13 << 4);
    *(info->curr_param + 10) = (u32)(mtx->_20 << 4);
    *(info->curr_param + 11) = (u32)(mtx->_21 << 4);
    *(info->curr_param + 12) = (u32)(mtx->_22 << 4);
    *(info->curr_param + 13) = (u32)(mtx->_23 << 4);
    *(info->curr_param + 14) = (u32)mtx->_30;
    *(info->curr_param + 15) = (u32)mtx->_31;
    *(info->curr_param + 16) = (u32)mtx->_32;
    *(info->curr_param + 17) = (u32)mtx->_33;
}


/*---------------------------------------------------------------------------*
  Name:         G3CS_LoadTexMtxEnv

  Description:  Generates a display list code with command packing,
                to load a texture matrix to the current one on the geometry
                engine, adjusting the elements for Normal/Vertex source.

  Arguments:    info         a pointer to display list info
                mtx          a pointer to a 4x4 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>              // avoid byte access problems
void G3CS_LoadTexMtxEnv(GXDLInfo *info, const MtxFx44 *mtx)
{
    SDK_NULL_ASSERT(info);
    SDK_NULL_ASSERT(mtx);

    // padding(notice that info->curr_cmd and info->curr_param change)
    switch ((u32)info->curr_cmd & 3)
    {
        // padding
    case 0:
        // DO NOTHING
        break;
    case 1:
        *info->curr_cmd++ = 0;         // byte access
    case 2:
        *info->curr_cmd++ = 0;
    case 3:
        *info->curr_cmd++ = 0;
        ++info->curr_param;
    };

    *(u32 *)info->curr_cmd = (G3OP_MTX_LOAD_4x4 << 8) | G3OP_MTX_MODE;
    *info->curr_param = (u32)(GX_MTXMODE_TEXTURE << REG_G3_MTX_MODE_M_SHIFT);
    *(info->curr_param + 1) = (u32)G3OP_MTX_LOAD_4x4;

    *(info->curr_param + 2) = (u32)(mtx->_00 << 4);
    *(info->curr_param + 3) = (u32)(mtx->_01 << 4);
    *(info->curr_param + 4) = (u32)(mtx->_02 << 4);
    *(info->curr_param + 5) = (u32)(mtx->_03 << 4);
    *(info->curr_param + 6) = (u32)(mtx->_10 << 4);
    *(info->curr_param + 7) = (u32)(mtx->_11 << 4);
    *(info->curr_param + 8) = (u32)(mtx->_12 << 4);
    *(info->curr_param + 9) = (u32)(mtx->_13 << 4);
    *(info->curr_param + 10) = (u32)(mtx->_20 << 4);
    *(info->curr_param + 11) = (u32)(mtx->_21 << 4);
    *(info->curr_param + 12) = (u32)(mtx->_22 << 4);
    *(info->curr_param + 13) = (u32)(mtx->_23 << 4);
    *(info->curr_param + 14) = (u32)(mtx->_30);
    *(info->curr_param + 15) = (u32)(mtx->_31);
    *(info->curr_param + 16) = (u32)(mtx->_32);
    *(info->curr_param + 17) = (u32)(mtx->_33);
}

#include <nitro/codereset.h>

/*---------------------------------------------------------------------------*
  Name:         G3B_LoadTexMtxTexCoord

  Description:  Increments the pointer to the display list buffer
                after it executed G3BS_LoadTexMtxTexCoord.

  Arguments:    info         a pointer to display list info
                mtx          a pointer to a 4x4 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3B_LoadTexMtxTexCoord(GXDLInfo *info, const MtxFx44 *mtx)
{
    G3BS_LoadTexMtxTexCoord(info, mtx);
    G3B_UpdateGXDLInfo(info, G3OP_MTX_MODE_NPARAMS + G3OP_MTX_LOAD_4x4_NPARAMS + 1);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_LoadTexMtxTexCoord

  Description:  Increments the pointer to the display list buffer
                after it executed G3CS_LoadTexMtxTexCoord.

  Arguments:    info         a pointer to display list info
                mtx          a pointer to a 4x4 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_LoadTexMtxTexCoord(GXDLInfo *info, const MtxFx44 *mtx)
{
    G3CS_LoadTexMtxTexCoord(info, mtx);
    info->curr_param += G3OP_MTX_MODE_NPARAMS + G3OP_MTX_LOAD_4x4_NPARAMS + 1;
    info->curr_cmd += 2;
}


/*---------------------------------------------------------------------------*
  Name:         G3B_LoadTexMtxEnv

  Description:  Increments the pointer to the display list buffer
                after it executed G3BS_LoadTexMtxEnv.

  Arguments:    info         a pointer to display list info
                mtx          a pointer to a 4x4 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3B_LoadTexMtxEnv(GXDLInfo *info, const MtxFx44 *mtx)
{
    G3BS_LoadTexMtxEnv(info, mtx);
    G3B_UpdateGXDLInfo(info, G3OP_MTX_MODE_NPARAMS + G3OP_MTX_LOAD_4x4_NPARAMS + 1);
}


/*---------------------------------------------------------------------------*
  Name:         G3C_LoadTexMtxEnv

  Description:  Increments the pointer to the display list buffer
                after it executed G3CS_LoadTexMtxEnv.

  Arguments:    info         a pointer to display list info
                mtx          a pointer to a 4x4 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
void G3C_LoadTexMtxEnv(GXDLInfo *info, const MtxFx44 *mtx)
{
    G3CS_LoadTexMtxEnv(info, mtx);
    info->curr_param += G3OP_MTX_MODE_NPARAMS + G3OP_MTX_LOAD_4x4_NPARAMS + 1;
    info->curr_cmd += 2;
}
