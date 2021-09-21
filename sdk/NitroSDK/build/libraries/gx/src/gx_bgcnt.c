/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - 
  File:     gx_bgcnt.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: gx_bgcnt.c,v $
  Revision 1.22  2006/01/18 02:11:21  kitase_hirotake
  do-indent

  Revision 1.21  2005/03/01 01:57:00  yosizaki
  copyright ‚Ì”N‚ðC³.

  Revision 1.20  2005/02/28 05:26:13  yosizaki
  do-indent.

  Revision 1.19  2004/04/05 04:18:52  takano_makoto
  Fix G2_GetBG3ScrPtr() G3_GetBG3ScrPtr()

  Revision 1.18  2004/03/01 09:19:36  nishida_kenji
  Fix G2_GetBG2ScrPtr(returns HW_BG_VRAM if GX_BGMODE_6).

  Revision 1.17  2004/02/18 08:31:37  nishida_kenji
  small fix.

  Revision 1.16  2004/02/17 10:43:52  nishida_kenji
  small fix.

  Revision 1.15  2004/02/12 07:06:27  nishida_kenji
  Avoid generating STRB in THUMB mode.

  Revision 1.14  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.13  2004/01/26 12:24:37  nishida_kenji
  adds APIs for the sub engine.

  Revision 1.12  2003/12/25 11:00:09  nishida_kenji
  converted by GX_APIChangeFrom031212-2.pl

  Revision 1.11  2003/12/25 00:19:29  nishida_kenji
  convert INLINE to inline

  Revision 1.10  2003/12/17 09:00:20  nishida_kenji
  Totally revised APIs.
  build/buildtools/GX_APIChangeFrom031212.pl would help you change your program to some extent.

  Revision 1.9  2003/12/17 08:53:17  nishida_kenji
  revise comments

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/gx/gx_bgcnt.h>
#include <nitro/gx/gx.h>

//
// Internal use
//
static inline int getBGScreenOffset_(void)
{
    return (int)(0x10000 * ((reg_GX_DISPCNT & REG_GX_DISPCNT_BGSCREENOFFSET_MASK) >>
                            REG_GX_DISPCNT_BGSCREENOFFSET_SHIFT));
}

static inline int getBGCharOffset_(void)
{
    return (int)(0x10000 * ((reg_GX_DISPCNT & REG_GX_DISPCNT_BGCHAROFFSET_MASK) >>
                            REG_GX_DISPCNT_BGCHAROFFSET_SHIFT));
}


/*---------------------------------------------------------------------------*
  Name:         G2_GetBG0ScrPtr

  Description:  Returns a pointer to BG #0 screen(MAIN engine).

  Arguments:    none

  Returns:      a pointer to BG #0 screen
 *---------------------------------------------------------------------------*/
void   *G2_GetBG0ScrPtr(void)
{
    int     baseBlock = 0x800 * ((reg_G2_BG0CNT & REG_G2_BG0CNT_SCREENBASE_MASK) >>
                                 REG_G2_BG0CNT_SCREENBASE_SHIFT);

    return (void *)(HW_BG_VRAM + getBGScreenOffset_() + baseBlock);
}


/*---------------------------------------------------------------------------*
  Name:         G2S_GetBG0ScrPtr

  Description:  Returns a pointer to BG #0 screen(SUB engine).

  Arguments:    none

  Returns:      a pointer to BG #0 screen
 *---------------------------------------------------------------------------*/
void   *G2S_GetBG0ScrPtr(void)
{
    int     baseBlock = 0x800 * ((reg_G2S_DB_BG0CNT & REG_G2S_DB_BG0CNT_SCREENBASE_MASK) >>
                                 REG_G2S_DB_BG0CNT_SCREENBASE_SHIFT);

    return (void *)(HW_DB_BG_VRAM + baseBlock);
}


/*---------------------------------------------------------------------------*
  Name:         G2_GetBG1ScrPtr

  Description:  Returns a pointer to BG #1 screen(MAIN engine).

  Arguments:    none

  Returns:      a pointer to BG #1 screen
 *---------------------------------------------------------------------------*/
void   *G2_GetBG1ScrPtr(void)
{
    int     baseBlock = 0x800 * ((reg_G2_BG1CNT & REG_G2_BG1CNT_SCREENBASE_MASK) >>
                                 REG_G2_BG1CNT_SCREENBASE_SHIFT);

    return (void *)(HW_BG_VRAM + getBGScreenOffset_() + baseBlock);
}


/*---------------------------------------------------------------------------*
  Name:         G2S_GetBG1ScrPtr

  Description:  Returns a pointer to BG #1 screen(SUB engine).

  Arguments:    none

  Returns:      a pointer to BG #1 screen
 *---------------------------------------------------------------------------*/
void   *G2S_GetBG1ScrPtr(void)
{
    int     baseBlock = 0x800 * ((reg_G2S_DB_BG1CNT & REG_G2S_DB_BG1CNT_SCREENBASE_MASK)
                                 >> REG_G2S_DB_BG1CNT_SCREENBASE_SHIFT);

    return (void *)(HW_DB_BG_VRAM + baseBlock);
}


/*---------------------------------------------------------------------------*
  Name:         G2_GetBG2ScrPtr

  Description:  Returns a pointer to BG #2 screen(MAIN engine).

  Arguments:    none

  Returns:      a pointer to BG #2 screen,
                NULL if BGMODE is GX_BGMODE_6.
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>              // ARM binary is even better in size.
void   *G2_GetBG2ScrPtr(void)
{
    GXBGMode bgMode = (GXBGMode)((reg_GX_DISPCNT & REG_GX_DISPCNT_BGMODE_MASK) >>
                                 REG_GX_DISPCNT_BGMODE_SHIFT);
    u32     bg = reg_G2_BG2CNT;
    int     offset = getBGScreenOffset_();
    u32     blockID = (bg & REG_G2_BG2CNT_SCREENBASE_MASK) >> REG_G2_BG2CNT_SCREENBASE_SHIFT;

    switch (bgMode)
    {
    case GX_BGMODE_0:
    case GX_BGMODE_1:
    case GX_BGMODE_2:
    case GX_BGMODE_3:
    case GX_BGMODE_4:
        // Text or SR BG
        return (void *)(HW_BG_VRAM + offset + 0x800 * blockID);
        break;
    case GX_BGMODE_5:
        // Extended SR BG
        if (bg & REG_G2_BG2CNT_COLORMODE_MASK)
        {
            // Bitmap BG
            return (void *)(HW_BG_VRAM + 0x4000 * blockID);
        }
        else
        {
            // Char BG
            return (void *)(HW_BG_VRAM + offset + 0x800 * blockID);
        }
        break;
    case GX_BGMODE_6:
        // 256 colors large bitmap screen
        return (void *)HW_BG_VRAM;
        break;
    default:
        SDK_INTERNAL_ERROR("unknown BGMODE 0x%x", bgMode);
        return NULL;
        break;
    }
}

#include <nitro/codereset.h>


/*---------------------------------------------------------------------------*
  Name:         G2S_GetBG2ScrPtr

  Description:  Returns a pointer to BG #2 screen(SUB engine).

  Arguments:    none

  Returns:      a pointer to BG #2 screen,
                NULL if BGMODE is GX_BGMODE_6.
 *---------------------------------------------------------------------------*/
void   *G2S_GetBG2ScrPtr(void)
{
    GXBGMode bgMode = (GXBGMode)((reg_GXS_DB_DISPCNT & REG_GXS_DB_DISPCNT_BGMODE_MASK) >>
                                 REG_GXS_DB_DISPCNT_BGMODE_SHIFT);
    u32     bg = reg_G2S_DB_BG2CNT;
    u32     blockID =
        (bg & REG_G2S_DB_BG2CNT_SCREENBASE_MASK) >> REG_G2S_DB_BG2CNT_SCREENBASE_SHIFT;

    switch (bgMode)
    {
    case GX_BGMODE_0:
    case GX_BGMODE_1:
    case GX_BGMODE_2:
    case GX_BGMODE_3:
    case GX_BGMODE_4:
        // Text or SR BG
        return (void *)(HW_DB_BG_VRAM + 0x800 * blockID);
        break;
    case GX_BGMODE_5:
        // Extended SR BG
        if (bg & REG_G2S_DB_BG2CNT_COLORMODE_MASK)
        {
            // Bitmap BG
            return (void *)(HW_DB_BG_VRAM + 0x4000 * blockID);
        }
        else
        {
            // Char BG
            return (void *)(HW_DB_BG_VRAM + 0x800 * blockID);
        }
        break;
    case GX_BGMODE_6:
        return NULL;
        break;
    default:
        SDK_INTERNAL_ERROR("unknown BGMODE 0x%x", bgMode);
        return NULL;
        break;
    }
}


/*---------------------------------------------------------------------------*
  Name:         G2_GetBG3ScrPtr

  Description:  Returns a pointer to BG #3 screen(MAIN engine).

  Arguments:    none

  Returns:      a pointer to BG #3 screen,
                NULL if BGMODE is GX_BGMODE_6.
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>              // ARM binary is even better in size.
void   *G2_GetBG3ScrPtr(void)
{
    GXBGMode bgMode = (GXBGMode)((reg_GX_DISPCNT & REG_GX_DISPCNT_BGMODE_MASK) >>
                                 REG_GX_DISPCNT_BGMODE_SHIFT);
    u32     bg = reg_G2_BG3CNT;
    int     offset = getBGScreenOffset_();
    u32     blockID = (bg & REG_G2_BG3CNT_SCREENBASE_MASK) >> REG_G2_BG3CNT_SCREENBASE_SHIFT;

    switch (bgMode)
    {
    case GX_BGMODE_0:
    case GX_BGMODE_1:
    case GX_BGMODE_2:
        // Text or affine BG
        return (void *)(HW_BG_VRAM + offset + 0x800 * blockID);
        break;
    case GX_BGMODE_3:
    case GX_BGMODE_4:
    case GX_BGMODE_5:
        // Extended affine BG
        if (bg & REG_G2_BG3CNT_COLORMODE_MASK)
        {
            // Bitmap BG
            return (void *)(HW_BG_VRAM + 0x4000 * blockID);
        }
        else
        {
            // Char BG
            return (void *)(HW_BG_VRAM + offset + 0x800 * blockID);
        }
        break;
    case GX_BGMODE_6:
        return NULL;
        break;
    default:
        SDK_INTERNAL_ERROR("unknown BGMODE 0x%x", bgMode);
        return NULL;
        break;
    }
}

#include <nitro/codereset.h>


/*---------------------------------------------------------------------------*
  Name:         G2S_GetBG3ScrPtr

  Description:  Returns a pointer to BG #3 screen(SUB engine).

  Arguments:    none

  Returns:      a pointer to BG #3 screen,
                NULL if BGMODE is GX_BGMODE_6.
 *---------------------------------------------------------------------------*/
void   *G2S_GetBG3ScrPtr(void)
{
    GXBGMode bgMode = (GXBGMode)((reg_GXS_DB_DISPCNT & REG_GXS_DB_DISPCNT_BGMODE_MASK) >>
                                 REG_GXS_DB_DISPCNT_BGMODE_SHIFT);
    u32     bg = reg_G2S_DB_BG3CNT;
    u32     blockID =
        (bg & REG_G2S_DB_BG3CNT_SCREENBASE_MASK) >> REG_G2S_DB_BG3CNT_SCREENBASE_SHIFT;

    switch (bgMode)
    {
    case GX_BGMODE_0:
    case GX_BGMODE_1:
    case GX_BGMODE_2:
        // Text or affine BG
        return (void *)(HW_DB_BG_VRAM + 0x800 * blockID);
        break;
    case GX_BGMODE_3:
    case GX_BGMODE_4:
    case GX_BGMODE_5:
        // Extended affine BG
        if (bg & REG_G2S_DB_BG3CNT_COLORMODE_MASK)
        {
            // Bitmap BG
            return (void *)(HW_DB_BG_VRAM + 0x4000 * blockID);
        }
        else
        {
            // Char BG
            return (void *)(HW_DB_BG_VRAM + 0x800 * blockID);
        }
        break;
    case GX_BGMODE_6:
        return NULL;
        break;
    default:
        SDK_INTERNAL_ERROR("unknown BGMODE 0x%x", bgMode);
        return NULL;
        break;
    }
}


// CharPtr
/*---------------------------------------------------------------------------*
  Name:         G2_GetBG0CharPtr

  Description:  Returns a pointer to BG #0 character(MAIN engine).

  Arguments:    none

  Returns:      a pointer to BG #0 character
 *---------------------------------------------------------------------------*/
void   *G2_GetBG0CharPtr(void)
{
    int     baseBlock = 0x4000 * ((reg_G2_BG0CNT & REG_G2_BG0CNT_CHARBASE_MASK) >>
                                  REG_G2_BG0CNT_CHARBASE_SHIFT);

    return (void *)(HW_BG_VRAM + getBGCharOffset_() + baseBlock);
}


/*---------------------------------------------------------------------------*
  Name:         G2S_GetBG0CharPtr

  Description:  Returns a pointer to BG #0 character(SUB engine).

  Arguments:    none

  Returns:      a pointer to BG #0 character
 *---------------------------------------------------------------------------*/
void   *G2S_GetBG0CharPtr(void)
{
    int     baseBlock = 0x4000 * ((reg_G2S_DB_BG0CNT & REG_G2S_DB_BG0CNT_CHARBASE_MASK) >>
                                  REG_G2S_DB_BG0CNT_CHARBASE_SHIFT);

    return (void *)(HW_DB_BG_VRAM + baseBlock);
}


/*---------------------------------------------------------------------------*
  Name:         G2_GetBG1CharPtr

  Description:  Returns a pointer to BG #1 character(MAIN engine).

  Arguments:    none

  Returns:      a pointer to BG #1 character
 *---------------------------------------------------------------------------*/
void   *G2_GetBG1CharPtr(void)
{
    int     baseBlock = 0x4000 * ((reg_G2_BG1CNT & REG_G2_BG1CNT_CHARBASE_MASK) >>
                                  REG_G2_BG1CNT_CHARBASE_SHIFT);

    return (void *)(HW_BG_VRAM + getBGCharOffset_() + baseBlock);
}


/*---------------------------------------------------------------------------*
  Name:         G2S_GetBG1CharPtr

  Description:  Returns a pointer to BG #1 character(SUB engine).

  Arguments:    none

  Returns:      a pointer to BG #1 character
 *---------------------------------------------------------------------------*/
void   *G2S_GetBG1CharPtr(void)
{
    int     baseBlock = 0x4000 * ((reg_G2S_DB_BG1CNT & REG_G2S_DB_BG1CNT_CHARBASE_MASK) >>
                                  REG_G2S_DB_BG1CNT_CHARBASE_SHIFT);

    return (void *)(HW_DB_BG_VRAM + baseBlock);
}


/*---------------------------------------------------------------------------*
  Name:         G2_GetBG2CharPtr

  Description:  Returns a pointer to BG #2 character(MAIN engine).

  Arguments:    none

  Returns:      BG #2 is character BG:     a pointer to BG #2 character
                otherwise:                 NULL
 *---------------------------------------------------------------------------*/
void   *G2_GetBG2CharPtr(void)
{
    GXBGMode bgMode = (GXBGMode)((reg_GX_DISPCNT & REG_GX_DISPCNT_BGMODE_MASK) >>
                                 REG_GX_DISPCNT_BGMODE_SHIFT);
    u32     bg = reg_G2_BG2CNT;

    if (bgMode < 5 || !(bg & REG_G2_BG2CNT_COLORMODE_MASK))
    {
        // Text BG, affine BG, or extended affine character BG
        int     offset = getBGCharOffset_();
        u32     blockID = (bg & REG_G2_BG2CNT_CHARBASE_MASK) >> REG_G2_BG2CNT_CHARBASE_SHIFT;

        return (void *)(HW_BG_VRAM + offset + 0x4000 * blockID);
    }
    else
    {
        // otherwise
        return NULL;
    }
}


/*---------------------------------------------------------------------------*
  Name:         G2S_GetBG2CharPtr

  Description:  Returns a pointer to BG #2 character(SUB engine).

  Arguments:    none

  Returns:      BG #2 is character BG:     a pointer to BG #2 character
                otherwise:                 NULL
 *---------------------------------------------------------------------------*/
void   *G2S_GetBG2CharPtr(void)
{
    GXBGMode bgMode = (GXBGMode)((reg_GXS_DB_DISPCNT & REG_GXS_DB_DISPCNT_BGMODE_MASK) >>
                                 REG_GXS_DB_DISPCNT_BGMODE_SHIFT);
    u32     bg = reg_G2S_DB_BG2CNT;
    if (bgMode < 5 || !(bg & REG_G2S_DB_BG2CNT_COLORMODE_MASK))
    {
        // Text BG, affine BG, or extended affine character BG
        u32     blockID =
            (bg & REG_G2S_DB_BG2CNT_CHARBASE_MASK) >> REG_G2S_DB_BG2CNT_CHARBASE_SHIFT;

        return (void *)(HW_DB_BG_VRAM + 0x4000 * blockID);
    }
    else
    {
        // otherwise
        return NULL;
    }
}


/*---------------------------------------------------------------------------*
  Name:         G2_GetBG3CharPtr

  Description:  Returns a pointer to BG #3 character(MAIN engine).

  Arguments:    none

  Returns:      BG #3 is character BG:     a pointer to BG #3 character
                otherwise:                 NULL
 *---------------------------------------------------------------------------*/
void   *G2_GetBG3CharPtr(void)
{
    GXBGMode bgMode = (GXBGMode)((reg_GX_DISPCNT & REG_GX_DISPCNT_BGMODE_MASK) >>
                                 REG_GX_DISPCNT_BGMODE_SHIFT);
    u32     bg = reg_G2_BG3CNT;
    if (bgMode < 3 || (bgMode < 6 && !(bg & REG_G2_BG2CNT_COLORMODE_MASK)))
    {
        // Text BG, affine BG, or extended affine character BG
        u32     blockID = (bg & REG_G2_BG3CNT_CHARBASE_MASK) >> REG_G2_BG3CNT_CHARBASE_SHIFT;
        int     offset = getBGCharOffset_();

        return (void *)(HW_BG_VRAM + offset + 0x4000 * blockID);
    }
    else
    {
        // otherwise
        return NULL;
    }
}


/*---------------------------------------------------------------------------*
  Name:         G2S_GetBG3CharPtr

  Description:  Returns a pointer to BG #3 character(SUB engine).

  Arguments:    none

  Returns:      BG #3 is character BG:     a pointer to BG #3 character
                otherwise:                 NULL
 *---------------------------------------------------------------------------*/
void   *G2S_GetBG3CharPtr(void)
{
    GXBGMode bgMode = (GXBGMode)((reg_GXS_DB_DISPCNT & REG_GXS_DB_DISPCNT_BGMODE_MASK) >>
                                 REG_GXS_DB_DISPCNT_BGMODE_SHIFT);
    u32     bg = reg_G2S_DB_BG3CNT;
    if (bgMode < 3 || (bgMode < 6 && !(bg & REG_G2S_DB_BG2CNT_COLORMODE_MASK)))
    {
        // Text BG, affine BG, or extended affine character BG
        u32     blockID =
            (bg & REG_G2S_DB_BG3CNT_CHARBASE_MASK) >> REG_G2S_DB_BG3CNT_CHARBASE_SHIFT;

        return (void *)(HW_DB_BG_VRAM + 0x4000 * blockID);
    }
    else
    {
        // otherwise
        return NULL;
    }
}
