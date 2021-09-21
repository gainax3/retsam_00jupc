/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - mcs
  File:     printi.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.4 $
 *---------------------------------------------------------------------------*/

#ifndef NNS_MCS_PRINTI_H_
#define NNS_MCS_PRINTI_H_


#if defined(_MSC_VER)
    #include "mcsStdInt.h"
#else
    #include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================
    íËêîíËã`
   ======================================================================== */

#define NNSi_MCS_PRINT_CHANNEL  (uint16_t)('PR' +0x8000)


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NNS_MCS_PRINTI_H_ */
#endif
