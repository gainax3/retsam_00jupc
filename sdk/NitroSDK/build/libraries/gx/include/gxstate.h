/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - 
  File:     gxstate.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: gxstate.h,v $
  Revision 1.15  2006/01/18 02:11:21  kitase_hirotake
  do-indent

  Revision 1.14  2005/03/01 01:57:00  yosizaki
  copyright ‚Ì”N‚ðC³.

  Revision 1.13  2005/02/28 05:26:13  yosizaki
  do-indent.

  Revision 1.12  2004/02/20 07:21:06  nishida_kenji
  Add region checks for Tex and TexPltt.

  Revision 1.11  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.10  2004/01/27 11:36:34  nishida_kenji
  small fix

  Revision 1.9  2004/01/27 11:14:41  nishida_kenji
  adds APIs for the sub engine.

  Revision 1.8  2004/01/17 02:28:38  nishida_kenji
  convert 'inline' to 'static inline' in header files

  Revision 1.7  2003/12/25 00:19:29  nishida_kenji
  convert INLINE to inline

  Revision 1.6  2003/12/24 08:28:01  nishida_kenji
  move WRAM control to MI domain.

  Revision 1.5  2003/12/17 06:20:20  nishida_kenji
  region checking codes

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_GXSTATE_H_
#define NITRO_GXSTATE_H_

#include <nitro/gx/gx.h>
#include <nitro/gx/gx_vramcnt.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    u16     lcdc;
    u16     bg;
    u16     obj;
    u16     arm7;
    u16     tex;
    u16     texPltt;
    u16     clrImg;
    u16     bgExtPltt;
    u16     objExtPltt;

    u16     sub_bg;
    u16     sub_obj;
    u16     sub_bgExtPltt;
    u16     sub_objExtPltt;
}
GX_VRAMCnt_;


typedef struct
{
    GX_VRAMCnt_ vramCnt;
}
GX_State;

extern GX_State gGXState;

void    GX_InitGXState();

#ifdef SDK_DEBUG
void    GX_StateCheck_VRAMCnt_();
void    GX_RegionCheck_BG_(u32 first, u32 last);
void    GX_RegionCheck_OBJ_(u32 first, u32 last);
void    GX_RegionCheck_SubBG_(u32 first, u32 last);
void    GX_RegionCheck_SubOBJ_(u32 first, u32 last);
void    GX_RegionCheck_Tex_(GXVRamTex tex, u32 first, u32 last);
void    GX_RegionCheck_TexPltt_(GXVRamTexPltt texPltt, u32 first, u32 last);
#endif

#ifdef SDK_DEBUG
static inline void GX_StateCheck_VRAMCnt()
{
    GX_StateCheck_VRAMCnt_();
}

static inline void GX_RegionCheck_BG(u32 first, u32 last)
{
    GX_RegionCheck_BG_(first, last);
}

static inline void GX_RegionCheck_OBJ(u32 first, u32 last)
{
    GX_RegionCheck_OBJ_(first, last);
}

static inline void GX_RegionCheck_SubBG(u32 first, u32 last)
{
    GX_RegionCheck_SubBG_(first, last);
}

static inline void GX_RegionCheck_SubOBJ(u32 first, u32 last)
{
    GX_RegionCheck_SubOBJ_(first, last);
}

static inline void GX_RegionCheck_Tex(GXVRamTex tex, u32 first, u32 last)
{
    GX_RegionCheck_Tex_(tex, first, last);
}

static inline void GX_RegionCheck_TexPltt(GXVRamTexPltt texPltt, u32 first, u32 last)
{
    GX_RegionCheck_TexPltt_(texPltt, first, last);
}

#else
static inline void GX_StateCheck_VRAMCnt()
{
}
static inline void GX_RegionCheck_BG(u32, u32)
{
}
static inline void GX_RegionCheck_OBJ(u32, u32)
{
}
static inline void GX_RegionCheck_SubBG(u32, u32)
{
}
static inline void GX_RegionCheck_SubOBJ(u32, u32)
{
}
static inline void GX_RegionCheck_Tex(GXVRamTex, u32, u32)
{
}
static inline void GX_RegionCheck_TexPltt(GXVRamTexPltt, u32, u32)
{
}
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
