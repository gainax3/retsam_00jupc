/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d
  File:     g2di_AssertUtil.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.2 $
 *---------------------------------------------------------------------------*/
#ifndef ASSERT_UTIL_H_
#define ASSERT_UTIL_H_


#ifdef __cplusplus
extern "C" {
#endif


// アライメントチェック
#define NNS_G2D_IS_ALIGNED(addr, align) (((u32)(addr) & ((align)-1)) == 0)

#define NNS_G2D_ALIGN_ASSERT(addr, align)                   \
    NNS_G2D_ASSERTMSG(                                      \
        NNS_G2D_IS_ALIGNED((addr), (align)),                \
        "'%s'=%p must be %d byte aligned.", #addr, (addr) )

// 有効なメモリアドレスチェック
#define NNS_G2D_IS_VALID_POINTER(p) ((HW_ITCM_IMAGE <= ((u32)(p))) && (((u32)(p)) <= HW_CTRDG_RAM_END))

#define NNS_G2D_POINTER_ASSERT(p)                                           \
    NNS_G2D_ASSERTMSG(                                                      \
        NNS_G2D_IS_VALID_POINTER(p),                                        \
        "'%s'=%p is invalid memory address.", #p, (p) );

#ifdef __cplusplus
}/* extern "C" */
#endif

#endif // ASSERT_UTIL_H_

