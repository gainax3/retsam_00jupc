/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - 
  File:     gxasm.c

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: gxasm.c,v $
  Revision 1.7  2005/03/01 01:57:00  yosizaki
  copyright ÇÃîNÇèCê≥.

  Revision 1.6  2005/02/28 05:26:13  yosizaki
  do-indent.

  Revision 1.5  2004/04/08 01:56:46  takano_makoto
  In GX_SendFifo64B and 128B, divide processing by TEG or TS.

  Revision 1.4  2004/02/18 07:40:37  nishida_kenji
  small fix.

  Revision 1.2  2004/02/10 06:03:15  nishida_kenji
  Move block copy functions to MI.

  Revision 1.1  2004/02/09 11:07:27  nishida_kenji
  Assembly code for GX API.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/code32.h>
#include "../include/gxasm.h"

asm void GX_SendFifo48B(register const void* pSrc, register void* pDest)
{
    ldmia   r0!, {r2,r3,r12}           // r0-r3, r12 need not be saved.
    stmia   r1,  {r2,r3,r12}
    ldmia   r0!, {r2,r3,r12}
    stmia   r1,  {r2,r3,r12}
    ldmia   r0!, {r2,r3,r12}
    stmia   r1,  {r2,r3,r12}
    ldmia   r0!, {r2,r3,r12}
    stmia   r1,  {r2,r3,r12}

    bx      lr
}

asm void GX_SendFifo64B(register const void* pSrc, register void* pDest)
{
#ifdef SDK_TEG
    ldmia   r0!, {r2,r3,r12}           // r0-r3, r12 need not be saved.
    stmia   r1,  {r2,r3,r12}
    ldmia   r0!, {r2,r3,r12}
    stmia   r1,  {r2,r3,r12}
    ldmia   r0!, {r2,r3,r12}
    stmia   r1,  {r2,r3,r12}
    ldmia   r0!, {r2,r3,r12}
    stmia   r1,  {r2,r3,r12}

    ldmia   r0!, {r2,r3,r12}
    stmia   r1,  {r2,r3,r12}
    ldmia   r0!, {r2}
    stmia   r1,  {r2}
    
    bx      lr
#else
    stmfd   sp!, {r4-r8}               // r0-r3, r12 need not be saved.

    ldmia   r0!, {r2-r8, r12}
    stmia   r1,  {r2-r8, r12}
    ldmia   r0!, {r2-r8, r12}
    stmia   r1,  {r2-r8, r12}

    ldmfd   sp!, {r4-r8}
    bx      lr
#endif
}

asm void GX_SendFifo128B(register const void* pSrc, register void* pDest)
{
#ifdef SDK_TEG
    ldmia   r0!, {r2,r3,r12}           // r0-r3, r12 need not be saved.
    stmia   r1,  {r2,r3,r12}
    ldmia   r0!, {r2,r3,r12}
    stmia   r1,  {r2,r3,r12}
    ldmia   r0!, {r2,r3,r12}
    stmia   r1,  {r2,r3,r12}
    ldmia   r0!, {r2,r3,r12}
    stmia   r1,  {r2,r3,r12}

    ldmia   r0!, {r2,r3,r12}
    stmia   r1,  {r2,r3,r12}
    ldmia   r0!, {r2,r3,r12}
    stmia   r1,  {r2,r3,r12}
    ldmia   r0!, {r2,r3,r12}
    stmia   r1,  {r2,r3,r12}
    ldmia   r0!, {r2,r3,r12}
    stmia   r1,  {r2,r3,r12}
    
    ldmia   r0!, {r2,r3,r12}
    stmia   r1,  {r2,r3,r12}
    ldmia   r0!, {r2,r3,r12}
    stmia   r1,  {r2,r3,r12}
    ldmia   r0!, {r2,r3}
    stmia   r1,  {r2,r3}
    
    bx      lr
    
#else
    stmfd   sp!, {r4-r8}               // r0-r3, r12 need not be saved.

    ldmia   r0!, {r2-r8, r12}
    stmia   r1,  {r2-r8, r12}
    ldmia   r0!, {r2-r8, r12}
    stmia   r1,  {r2-r8, r12}
    ldmia   r0!, {r2-r8, r12}
    stmia   r1,  {r2-r8, r12}
    ldmia   r0!, {r2-r8, r12}
    stmia   r1,  {r2-r8, r12}

    ldmfd   sp!, {r4-r8}
    bx      lr
#endif
}
