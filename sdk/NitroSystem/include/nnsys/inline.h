/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys
  File:     inline.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.2 $
 *---------------------------------------------------------------------------*/

#ifndef NNS_INLINE_H_
#define NNS_INLINE_H_

#if defined(_MSC_VER) || defined(NNS_FROM_TOOL)
#define NNS_INLINE __inline
#else
#define NNS_INLINE inline
#endif

/* NNS_INLINE_H_ */
#endif
