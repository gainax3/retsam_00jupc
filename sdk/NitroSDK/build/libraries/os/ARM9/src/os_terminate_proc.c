/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS
  File:     os_terminate_proc.c

  Copyright 2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_terminate_proc.c,v $
  Revision 1.4  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.3  2006/01/10 07:35:09  okubata_ryoma
  small fix

  Revision 1.2  2006/01/10 06:04:28  okubata_ryoma
  ‚“mall fix

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/os/common/system.h>

//============================================================================
//          TERMINATE and HALT
//============================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_Terminate

  Description:  Halt CPU and loop
  
  Arguments:    None

  Returns:      --  (Never return)
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL void OS_Terminate(void)
{
    while (1)
    {
        (void)OS_DisableInterrupts();
        OS_Halt();
    }
}

/*---------------------------------------------------------------------------*
  Name:         OS_Halt

  Description:  Halt CPU
  
  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>
SDK_WEAK_SYMBOL asm void OS_Halt( void )
{
        mov     r0, #0
        mcr     p15, 0, r0, c7, c0, 4
        bx      lr
}
#include <nitro/codereset.h>

/*---------------------------------------------------------------------------*
  Name:         OS_Exit

  Description:  Display exit string and Terminate.
                This is useful for 'loadrun' tool command.                 
  
  Arguments:    status : exit status

  Returns:      --  (Never return)
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL void OS_Exit(int status)
{
#ifdef SDK_FINALROM
#pragma unused( status )
#endif
    (void)OS_DisableInterrupts();
    OS_Printf("\n" OS_EXIT_STRING, status);
    OS_Terminate();
}
