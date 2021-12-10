/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - mcs
  File:     baseCommon.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.2 $
 *---------------------------------------------------------------------------*/

#ifndef NNS_MCS_BASECOMMON_H_
#define NNS_MCS_BASECOMMON_H_


#if defined(_MSC_VER)

    #include "mcsStdInt.h"

    typedef uint8_t     u8;
    typedef uint16_t    u16;
    typedef uint32_t    u32;

#endif // #if defined(_MSC_VER)

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
    íËêîíËã`
   ======================================================================== */

#define NNSi_MCS_MESSAGE_SIZE_MAX       (1024 * 32)

#define NNSi_MCS_MSG_HEAD_SIZE          sizeof(NNSiMcsMsgHead)

#define NNSi_MCS_MSG_DATA_SIZE_MAX      (NNSi_MCS_MESSAGE_SIZE_MAX - NNSi_MCS_MSG_HEAD_SIZE)
#define NNSi_MCS_MSG_DATA_SIZE_MIN      (1024 * 8 - NNSi_MCS_MSG_HEAD_SIZE)


/* ========================================================================
    å^íËã`
   ======================================================================== */

typedef struct NNSiMcsMsgHead NNSiMcsMsgHead;
struct NNSiMcsMsgHead
{
    u16         version;
    u16         reserved;
    u32         offset;
    u32         totalLen;
};

typedef struct NNSiMcsMsg NNSiMcsMsg;
struct NNSiMcsMsg
{
    NNSiMcsMsgHead  head;
    u8              data[NNSi_MCS_MSG_DATA_SIZE_MAX];
};


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NNS_MCS_BASECOMMON_H_ */
#endif
