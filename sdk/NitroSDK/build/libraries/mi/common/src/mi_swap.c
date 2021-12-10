/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI
  File:     mi_swap.c

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mi_swap.c,v $
  Revision 1.7  2005/03/01 01:57:00  yosizaki
  copyright ÇÃîNÇèCê≥.

  Revision 1.6  2005/02/28 05:26:25  yosizaki
  do-indent.

  Revision 1.5  2004/03/23 00:58:15  yada
  only fix comment

  Revision 1.4  2004/02/10 08:20:14  yada
  rewrite comment in english

  Revision 1.3  2004/02/10 05:09:11  yada
  (none)

  Revision 1.2  2004/02/10 02:28:08  yada
  code32Ç≈àÕÇ§

  Revision 1.1  2004/02/10 01:22:22  yada
  UTL_ Ç©ÇÁÇÃà⁄çs


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/types.h>
#include <nitro/mi/swap.h>


//  MI_Swap* will be used for control common resource exclusively among processers
//  or modules. Beside, it's used for realization spin lock system.
//
//  notice: you cannot access main memory by byte unless via cache.
//  so, use MI_SwapWord generally for accessing main memory ,not MI_SwapByte.


//---- This code will be compiled in ARM-Mode
#include <nitro/code32.h>

/*---------------------------------------------------------------------------*
  Name:         MI_SwapWord

  Description:  swap data and memory

  Arguments:    setData      data to swap
                destp        memory address to swap

  Returns:      swapped memory data
 *---------------------------------------------------------------------------*/
asm u32 MI_SwapWord( register u32 setData, register vu32* destp )
{
        swp     r0, r0, [r1]
        bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         MI_SwapByte

  Description:  swap data and memory

  Arguments:    setData      data to swap
                destp        memory address to swap

  Returns:      swapped memory data
 *---------------------------------------------------------------------------*/
asm u8  MI_SwapByte( register u32 setData, register vu8* destp )
{
        swpb    r0, r0, [r1]
        bx      lr
}

//---- end limitation of ARM-Mode
#include <nitro/codereset.h>
