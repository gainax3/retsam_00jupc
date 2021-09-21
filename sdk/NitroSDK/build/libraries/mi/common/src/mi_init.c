/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI
  File:     mi_init.c

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mi_init.c,v $
  Revision 1.3  2005/02/28 05:26:25  yosizaki
  do-indent.

  Revision 1.2  2005/02/10 07:44:48  yada
  fix include

  Revision 1.1  2005/02/09 05:10:02  yada
  initial release

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>

/*---------------------------------------------------------------------------*
  Name:         MI_Init

  Description:  initialize MI functions

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void MI_Init(void)
{
#ifdef SDK_ARM9
    //---- Init Work RAM (allocate both wram (block0/1) to ARM7)
    MI_SetWramBank(MI_WRAM_ARM7_ALL);
#endif

    //---- dummy DMA
    MI_StopDma(0);
}
