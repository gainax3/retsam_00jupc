/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g3d
  File:     mem.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.6 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#ifndef NNSG3D_MEM_H_
#define NNSG3D_MEM_H_

#include <nnsys/g3d/config.h>
#include <nnsys/g3d/kernel.h>
#include <nnsys/g3d/anm.h>

#include <nnsys/fnd/allocator.h>

//
// ヒープの確保・解放を伴うAPIの集合
// ユーティリティ的なものなので、ユーザーは特に使用を強制されない
//
// また、mem.hはmem.c以外のG3Dのソースからインクルードされてはいけない。
//

#ifdef __cplusplus
extern "C" {
#endif

NNSG3dRenderObj* NNS_G3dAllocRenderObj(NNSFndAllocator* pAlloc);
void NNS_G3dFreeRenderObj(NNSFndAllocator* pAlloc,
                          NNSG3dRenderObj* pRenderObj);



NNSG3dAnmObj* NNS_G3dAllocAnmObj(NNSFndAllocator* pAlloc,
                                 const void* pAnm,
                                 const NNSG3dResMdl* pMdl);
void NNS_G3dFreeAnmObj(NNSFndAllocator* pAlloc,
                       NNSG3dAnmObj* pAnmObj);



NNSG3dJntAnmResult* NNS_G3dAllocRecBufferJnt(NNSFndAllocator* pAlloc,
                                             const NNSG3dResMdl* pResMdl);
void NNS_G3dFreeRecBufferJnt(NNSFndAllocator* pAlloc,
                             NNSG3dJntAnmResult* pRecBuf);



NNSG3dMatAnmResult* NNS_G3dAllocRecBufferMat(NNSFndAllocator* pAlloc,
                                             const NNSG3dResMdl* pResMdl);
void NNS_G3dFreeRecBufferMat(NNSFndAllocator* pAlloc,
                             NNSG3dMatAnmResult* pRecBuf);


#ifdef __cplusplus
}/* extern "C" */
#endif

#endif

