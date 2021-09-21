/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - mcs
  File:     baseCommoni.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.5 $
 *---------------------------------------------------------------------------*/

#ifndef NNS_MCS_BASECOMMONI_H_
#define NNS_MCS_BASECOMMONI_H_

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================
    íËêîíËã`
   ======================================================================== */

#define NNSi_MCS_ENS_READ_RB_SIZE       (64 * 1024)
#define NNSi_MCS_ENS_READ_RB_START      0x10000000
#define NNSi_MCS_ENS_READ_RB_END        (NNSi_MCS_ENS_READ_RB_START + NNSi_MCS_ENS_READ_RB_SIZE)

#define NNSi_MCS_ENS_WRITE_RB_SIZE      NNSi_MCS_ENS_READ_RB_SIZE
#define NNSi_MCS_ENS_WRITE_RB_START     NNSi_MCS_ENS_READ_RB_END
#define NNSi_MCS_ENS_WRITE_RB_END       (NNSi_MCS_ENS_WRITE_RB_START + NNSi_MCS_ENS_WRITE_RB_SIZE)

#define NNSi_MCS_MSG_HEAD_VERSION       1

static const uint16_t   NNSi_MCS_SYSMSG_CHANNEL = 0xFFFF;


/* ========================================================================
    å^íËã`
   ======================================================================== */

typedef struct NNSiMcsEnsMsgBuf NNSiMcsEnsMsgBuf;
struct NNSiMcsEnsMsgBuf
{
    uint32_t    channel;
    uint8_t     buf[NNSi_MCS_MESSAGE_SIZE_MAX];
};


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NNS_MCS_BASECOMMONI_H_ */
#endif
