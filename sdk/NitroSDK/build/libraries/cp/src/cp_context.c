/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CP - src
  File:     context.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: cp_context.c,v $
  Revision 1.7  2006/08/10 01:37:31  okubata_ryoma
  Copyright fix

  Revision 1.6  2006/08/09 06:38:50  takano_makoto
  stm, ldmÇégópÇµÇƒçÇë¨âª

  Revision 1.5  2005/02/28 05:26:03  yosizaki
  do-indent.

  Revision 1.4  2005/01/12 11:08:24  takano_makoto
  fix copyright header.

  Revision 1.3  2005/01/12 00:28:41  yada
  consider SDK_TCM_APPLY

  Revision 1.2  2004/05/06 10:46:43  takano_makoto
  Modify include file.

  Revision 1.1  2004/05/06 02:52:40  takano_makoto
  Initial Update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/cp/context.h>

#if defined(SDK_TCM_APPLY) && defined(SDK_ARM9)
#include    <nitro/itcm_begin.h>
#endif
#include <nitro/code32.h>

/*---------------------------------------------------------------------------*
  Name:         CP_SaveContext

  Description:  Save current context into specified memory

  Arguments:    context   pointer to the memory to be stored the current context

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void CP_SaveContext(register CPContext *context)
{
    ldr     r1,  =REG_DIV_NUMER_ADDR
    stmfd   sp!, {r4}   // avoid stall
    ldmia   r1,  {r2-r4,r12}
    stmia   r0!, {r2-r4,r12}
    ldrh    r12, [r1, #REG_DIVCNT_OFFSET - REG_DIV_NUMER_OFFSET]
    
    add     r1,  r1, #REG_SQRT_PARAM_OFFSET - REG_DIV_NUMER_OFFSET
    ldmia   r1,  {r2-r3}
    stmia   r0!, {r2-r3}
    
    and     r12, r12, #REG_CP_DIVCNT_MODE_MASK
    ldrh    r2,  [r1, #REG_SQRTCNT_OFFSET - REG_SQRT_PARAM_OFFSET]
    strh    r12, [r0]
    and     r2,  r2, #REG_CP_SQRTCNT_MODE_MASK
    strh    r2,  [r0, #2]
    ldmfd   sp!, {r4}
    
    bx lr
}


/*---------------------------------------------------------------------------*
  Name:         CP_RestoreContext

  Description:  Reload specified context as current context

  Arguments:    context   pointer to the memory to switch to the context

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void CPi_RestoreContext(register const CPContext *context)
{
    stmfd   sp!, {r4}
    ldr     r1,  =REG_DIV_NUMER_ADDR
    ldmia   r0,  {r2-r4,r12}
    stmia   r1,  {r2-r4,r12}
    ldrh    r2,  [r0, #CPContext.div_mode]
    ldrh    r3,  [r0, #CPContext.sqrt_mode]
    
    strh    r2,  [r1, #REG_DIVCNT_OFFSET  - REG_DIV_NUMER_OFFSET]
    strh    r3,  [r1, #REG_SQRTCNT_OFFSET - REG_DIV_NUMER_OFFSET]
    
    add     r0,  r0, #CPContext.sqrt
    add     r1,  r1, #REG_SQRT_PARAM_OFFSET - REG_DIV_NUMER_OFFSET
    
    ldmia   r0,  {r2-r3}
    stmia   r1,  {r2-r3}
    
    ldmfd   sp!, {r4}
    
    bx lr
}

#include <nitro/codereset.h>

#if defined(SDK_TCM_APPLY) && defined(SDK_ARM9)
#include    <nitro/itcm_end.h>
#endif
