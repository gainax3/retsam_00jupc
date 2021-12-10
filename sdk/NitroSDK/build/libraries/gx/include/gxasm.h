/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - 
  File:     gxasm.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: gxasm.h,v $
  Revision 1.5  2006/01/18 02:11:21  kitase_hirotake
  do-indent

  Revision 1.4  2005/03/01 01:57:00  yosizaki
  copyright ÇÃîNÇèCê≥.

  Revision 1.3  2005/02/28 05:26:13  yosizaki
  do-indent.

  Revision 1.2  2004/02/10 06:03:15  nishida_kenji
  Move block copy functions to MI.

  Revision 1.1  2004/02/09 11:07:37  nishida_kenji
  Assembly code for GX API.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_GXASM_H_
#define NITRO_GXASM_H_

#include <nitro/mi/memory.h>

#ifdef __cplusplus
extern "C" {
#endif

void    GX_SendFifo36B(register const void *pSrc, register void *pDest);
void    GX_SendFifo48B(register const void *pSrc, register void *pDest);
void    GX_SendFifo64B(register const void *pSrc, register void *pDest);
void    GX_SendFifo128B(register const void *pSrc, register void *pDest);



static inline void GX_SendFifo36B(register const void *pSrc, register void *pDest)
{
    MI_Copy36B(pSrc, pDest);
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
