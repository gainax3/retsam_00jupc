/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CTRDG - include
  File:     ctrdg_work.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ctrdg_work.h,v $
  Revision 1.3  2006/01/18 02:11:20  kitase_hirotake
  do-indent

  Revision 1.2  2005/02/28 05:26:03  yosizaki
  do-indent.

  Revision 1.1  2004/09/14 09:58:21  yada
  initial release

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_LIBRARY_CTRDG_WORK_H_
#define NITRO_LIBRARY_CTRDG_WORK_H_


#include <nitro/types.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------
// for ctrdg common work
//
typedef struct CTRDGWork
{
    vu16    subpInitialized;
    u16     lockID;

}
CTRDGWork;


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_LIBRARY_CTRDG_WORK_H_ */
#endif
