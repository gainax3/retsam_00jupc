/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS
  File:     os_protectionUnit.c

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_protectionUnit.c,v $
  Revision 1.8  2005/02/28 05:26:29  yosizaki
  do-indent.

  Revision 1.7  2004/09/02 06:30:50  yada
  only change comment

  Revision 1.6  2004/03/24 00:36:00  yada
  SDK_BB îpé~Ç…Ç¬Ç´èCê≥

  Revision 1.5  2004/02/09 11:17:22  yasu
  include code32.h

  Revision 1.4  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.3  2004/01/18 02:25:37  yada
  ÉCÉìÉfÉìÉgìôÇÃêÆå`

  Revision 1.2  2004/01/16 01:18:05  yasu
  Support SDK_BB

  Revision 1.1  2004/01/14 12:03:05  yada
  èâî≈

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/os.h>
#include <nitro/code32.h>

//======================================================================
//          protection unit
//======================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_EnableProtectionUnit

  Description:  enable protection unit

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void OS_EnableProtectionUnit( void )
{
    mrc     p15, 0, r0, c1, c0, 0
    orr     r0, r0, #HW_C1_PROTECT_UNIT_ENABLE
    mcr     p15, 0, r0, c1, c0, 0
    bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         OS_DisableProtectionUnit

  Description:  disable protection unit

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void OS_DisableProtectionUnit( void )
{
    mrc     p15, 0, r0, c1, c0, 0
    bic     r0, r0, #HW_C1_PROTECT_UNIT_ENABLE
    mcr     p15, 0, r0, c1, c0, 0
    bx      lr
}

#include <nitro/codereset.h>
