/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI
  File:     mi_exMemory.c

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mi_exMemory.c,v $
  Revision 1.4  2005/02/28 05:26:25  yosizaki
  do-indent.

  Revision 1.3  2005/02/10 07:45:11  yada
  just fix spacing

  Revision 1.2  2005/02/09 07:22:02  yada
  change function name CTRDG_SetAgbFastestRomCycle to MI_SetAgbCartridgeFastestRomCycle()

  Revision 1.1  2005/02/04 12:40:31  yada
  new release

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>

/*---------------------------------------------------------------------------*
  Name:         MI_SetAgbCartridgeFastestRomCycle

  Description:  set Cartridge ROM access cycles for AGB. (8-4 cycle)

  Arguments:    prev1st : pointer to be store the previous 1st ROM access cycle.
                          if NULL, no effect.
                prev2nd : pointer to be store the previous 2nd ROM access cycle.
                          if NULL, no effect.

  Returns:      None
 *---------------------------------------------------------------------------*/
void MI_SetAgbCartridgeFastestRomCycle(MICartridgeRomCycle1st *prev1st,
                                       MICartridgeRomCycle2nd *prev2nd)
{
    //---- store previous value
    if (prev1st)
    {
        *prev1st = MI_GetCartridgeRomCycle1st();
    }
    if (prev2nd)
    {
        *prev2nd = MI_GetCartridgeRomCycle2nd();
    }

    //---- set cycles for AGB cartridge setting
    MI_SetCartridgeRomCycle1st(MI_CTRDG_ROMCYCLE1_8);
    MI_SetCartridgeRomCycle2nd(MI_CTRDG_ROMCYCLE2_4);
}
