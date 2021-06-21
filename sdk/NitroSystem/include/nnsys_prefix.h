/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include
  File:     nnsys_prefix.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.2 $
 *---------------------------------------------------------------------------*/

#ifndef NNSYS_PREFIX_H_
#define NNSYS_PREFIX_H_


#ifdef  SDK_DEBUG
#define NNS_DEBUG       1

#elif   SDK_RELEASE
#define NNS_RELEASE     1

#elif   SDK_FINALROM
#define NNS_FINALROM    1
#endif


#ifdef  SDK_CODE_ARM
#define NNS_CODE_ARM    1

#elif   SDK_CODE_THUMB
#define NNS_CODE_THUMB  1
#endif


#define NNS_TS          1
#define NNS_CW          1
#define NNS_ARM9        1


/* NNSYS_PREFIX_H_ */
#endif
