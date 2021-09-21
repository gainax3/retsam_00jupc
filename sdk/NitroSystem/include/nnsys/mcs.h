/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys
  File:     mcs.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.8 $
 *---------------------------------------------------------------------------*/

#ifndef NNS_MCS_H_
#define NNS_MCS_H_

#if defined(_WIN32)

#include <nnsys/mcs/base_win32.h>

/* _WIN32 */
#else

#include <nnsys/mcs/baseCommon.h>
#include <nnsys/mcs/base.h>
#include <nnsys/mcs/fileIOcommon.h>
#include <nnsys/mcs/fileIObase.h>
#include <nnsys/mcs/print.h>

/* _WIN32 */
#endif

/* NNS_MCS_H_ */
#endif
