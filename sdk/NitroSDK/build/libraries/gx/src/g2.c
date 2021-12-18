/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - 
  File:     g2.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2.c,v $
  Revision 1.31  2006/01/18 02:11:21  kitase_hirotake
  do-indent

  Revision 1.30  2005/03/01 01:57:00  yosizaki
  copyright ‚Ì”N‚ðC³.

  Revision 1.29  2005/02/28 05:26:13  yosizaki
  do-indent.

  Revision 1.28  2004/03/17 07:22:37  yasu
  do sdk_indent

  Revision 1.27  2004/02/18 07:50:31  nishida_kenji
  small fix.

  Revision 1.25  2004/02/12 07:06:27  nishida_kenji
  Avoid generating STRB in THUMB mode.

  Revision 1.24  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.23  2004/01/29 12:18:18  nishida_kenji
  Add comments.

  Revision 1.22  2004/01/22 08:41:35  nishida_kenji
  adds APIs for the 2nd display

  Revision 1.21  2003/12/17 08:53:17  nishida_kenji
  revise comments

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/gx/g2.h>
#include <nitro/fx/fx_const.h>


//----------------------------------------------------------------------------
// G2x_SetBGyAffine_:
//
// Called from G2_SetBG2Affine, G2_SetBG3Affine,
// G2S_SetBG2Affine, and G2S_SetBG3Affine. Internal use only.
//----------------------------------------------------------------------------
#include <nitro/code32.h>
// In this case, ARM binary is equal or better than THUMB binary even in size.
void G2x_SetBGyAffine_(u32 addr, const MtxFx22 *mtx, int centerX, int centerY, int x1, int y1)
{
    s32     dx, dy;
    fx32    x2, y2;

    SDK_NULL_ASSERT(mtx);
    SDK_MINMAX_ASSERT(mtx->_00, -128 * FX32_ONE, 128 * FX32_ONE - 1);
    SDK_MINMAX_ASSERT(mtx->_01, -128 * FX32_ONE, 128 * FX32_ONE - 1);
    SDK_MINMAX_ASSERT(mtx->_10, -128 * FX32_ONE, 128 * FX32_ONE - 1);
    SDK_MINMAX_ASSERT(mtx->_11, -128 * FX32_ONE, 128 * FX32_ONE - 1);

    // BGxPA, BGxPB are in s7.8 format
    *((vu32 *)addr + 0) = (u32)((u16)(s16)(mtx->_00 >> 4) | (u16)(s16)(mtx->_01 >> 4) << 16);

    // BGxPC, BGxPC are in s7.8 format
    *((vu32 *)addr + 1) = (u32)((u16)(s16)(mtx->_10 >> 4) | (u16)(s16)(mtx->_11 >> 4) << 16);

    dx = x1 - centerX;
    dy = y1 - centerY;

    // mtx * d + center
    // Note that x2, y2 are in fx32 format since dx, dy is integer.
    x2 = mtx->_00 * dx + mtx->_01 * dy + (centerX << FX32_SHIFT);
    y2 = mtx->_10 * dx + mtx->_11 * dy + (centerY << FX32_SHIFT);

    // reg_G2m_BGnXY has s19.8 format
    *((vu32 *)addr + 2) = (u32)(x2 >> 4);       // BGxX
    *((vu32 *)addr + 3) = (u32)(y2 >> 4);       // BGxY
}

#include <nitro/codereset.h>


typedef enum
{
    G2_BLENDTYPE_NONE = 0x0000,
    G2_BLENDTYPE_ALPHA = 0x0040,
    G2_BLENDTYPE_FADEIN = 0x0080,
    G2_BLENDTYPE_FADEOUT = 0x00c0
}
G2_BLENDTYPE;


//----------------------------------------------------------------------------
// G2x_SetBlendAlpha_:
//
// Called from G2_SetBlendAlpha and G2S_SetBlendAlpha.
// Internal use only.
//----------------------------------------------------------------------------
void G2x_SetBlendAlpha_(u32 addr, int plane1, int plane2, int ev1, int ev2)
{
    SDK_MINMAX_ASSERT(plane1, 0, 0x20 - 1);
    SDK_MINMAX_ASSERT(plane2, 0, 0x40 - 1);
    SDK_MINMAX_ASSERT(ev1, 0, 31);
    SDK_MINMAX_ASSERT(ev2, 0, 31);

    // BLDCNT & BLDALPHA
    *((vu32 *)addr + 0) =
        (u32)((G2_BLENDTYPE_ALPHA | plane1 | plane2 << 8) | ((ev1 | ev2 << 8) << 16));
}


//----------------------------------------------------------------------------
// G2x_SetBlendBrightness_:
//
// Called from G2_SetBlendBrightness and G2S_SetBlendBrightness.
// Internal use only.
//----------------------------------------------------------------------------
void G2x_SetBlendBrightness_(u32 addr, int plane, int brightness)
{
    SDK_MINMAX_ASSERT(brightness, -31, 31);
    SDK_MINMAX_ASSERT(plane, 0, 0x40 - 1);

    if (brightness < 0)
    {
        // BLDCNT
        *((vu16 *)addr + 0) = (u16)(G2_BLENDTYPE_FADEOUT | plane);
        // BLDY
        *((vu16 *)addr + 2) = (u16)-brightness;
    }
    else
    {
        // BLDCNT
        *((vu16 *)addr + 0) = (u16)(G2_BLENDTYPE_FADEIN | plane);
        // BLDY
        *((vu16 *)addr + 2) = (u16)brightness;
    }
}


//----------------------------------------------------------------------------
// G2x_SetBlendBrightnessExt_:
//
// Called from G2_SetBlendBrightnessExt and G2S_SetBlendBrightnessExt.
// Internal use only.
//----------------------------------------------------------------------------
void G2x_SetBlendBrightnessExt_(u32 addr, int plane1, int plane2, int ev1, int ev2, int brightness)
{
    SDK_MINMAX_ASSERT(brightness, -31, 31);
    SDK_MINMAX_ASSERT(plane1, 0, 0x40 - 1);
    SDK_MINMAX_ASSERT(plane2, 0, 0x40 - 1);
    SDK_MINMAX_ASSERT(ev1, 0, 31);
    SDK_MINMAX_ASSERT(ev2, 0, 31);

    // BLDALPHA
    *((vu16 *)addr + 1) = (u16)(ev1 | (ev2 << 8));

    if (brightness < 0)
    {
        // BLDCNT
        *((vu16 *)addr + 0) = (u16)(G2_BLENDTYPE_FADEOUT | plane1 | (plane2 << 8));

        // BLDY
        *((vu16 *)addr + 2) = (u16)-brightness;
    }
    else
    {
        // BLDCNT
        *((vu16 *)addr + 0) = (u16)(G2_BLENDTYPE_FADEIN | plane1 | (plane2 << 8));

        // BLDY
        *((vu16 *)addr + 2) = (u16)brightness;
    }
}


//----------------------------------------------------------------------------
// G2x_ChangeBlendBrightness_:
//
// Called from G2_ChangeBlendBrightness and G2S_ChangeBlendBrightness.
// Internal use only.
//----------------------------------------------------------------------------
void G2x_ChangeBlendBrightness_(u32 addr, int brightness)
{
    u16     tmp;
    SDK_MINMAX_ASSERT(brightness, -31, 31);

    // read BLDCNT
    tmp = *((vu16 *)addr + 0);

    if (brightness < 0)
    {
        if (G2_BLENDTYPE_FADEIN == (tmp & REG_G2_BLDCNT_EFFECT_MASK))
        {
            // BLDCNT
            *((vu16 *)addr + 0) = (u16)((tmp & ~REG_G2_BLDCNT_EFFECT_MASK) | G2_BLENDTYPE_FADEOUT);
        }
        // BLDY
        *((vu16 *)addr + 2) = (u16)-brightness;
    }
    else
    {
        if (G2_BLENDTYPE_FADEOUT == (tmp & REG_G2_BLDCNT_EFFECT_MASK))
        {
            // BLDCNT
            *((vu16 *)addr + 0) = (u16)((tmp & ~REG_G2_BLDCNT_EFFECT_MASK) | G2_BLENDTYPE_FADEIN);
        }
        // BLDY
        *((vu16 *)addr + 2) = (u16)brightness;
    }
}
