/*---------------------------------------------------------------------------*
  Project:  NitroSDK - PXI - libraries
  File:     pxi_init.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: pxi_init.c,v $
  Revision 1.4  2006/01/18 02:12:28  kitase_hirotake
  do-indent

  Revision 1.3  2005/08/22 02:48:45  kitase_hirotake
  .versionセクションに、ビルドがSDK_FINALROMでなければ無条件に文字列を埋め込む

  Revision 1.2  2005/02/28 05:26:29  yosizaki
  do-indent.

  Revision 1.1  2004/03/08 01:55:18  yasu
  add pxi_init.c

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include  <nitro/pxi.h>

#include  <nitro/misc.h>               // .version埋め込み用

/*
 * SDK_FINALROMでなければ.versionに文字列を埋め込む
 */
#ifndef SDK_FINALROM
#ifdef SDK_ARM9
#include <nitro/version_begin.h>
SDK_DEFINE_MIDDLEWARE(checkString, "NINTENDO", "DEBUG");
#include <nitro/version_end.h>
#endif
#endif

/*---------------------------------------------------------------------------*
  Name:         PXI_Init

  Description:  initialize PXI system

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void PXI_Init(void)
{
#ifndef SDK_FINALROM
#ifdef SDK_ARM9
    SDK_USING_MIDDLEWARE(checkString);
#endif
#endif
    PXI_InitFifo();
}
