/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - gfd
  File:     gfd_common.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.5 $
 *---------------------------------------------------------------------------*/
 
#ifndef NNS_GFD_COMMON_H_
#define NNS_GFD_COMMON_H_

#include <nitro.h>
#include <nnsys/inline.h>

#ifdef __cplusplus
extern "C" {
#endif


//------------------------------------------------------------------------------
#define NNS_GFD_NON_ZERO_ASSERT( val )  SDK_ASSERTMSG( ( val != 0 ), "Non-Zero value is expected in this context." );

#define NNS_GFD_ASSERT           SDK_ASSERT
#define NNS_GFD_ASSERTMSG        SDK_ASSERTMSG
#define NNS_GFD_NULL_ASSERT      SDK_NULL_ASSERT
#define NNS_GFD_MINMAX_ASSERT    SDK_MINMAX_ASSERT

#define NNS_GFD_INLINE           NNS_INLINE


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_GFD_COMMON_H_
