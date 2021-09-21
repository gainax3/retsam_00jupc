/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g3d
  File:     mem.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.5 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#include <nnsys/g3d/mem.h>


/*---------------------------------------------------------------------------*
    NNS_G3dAllocRenderObj

    NNSG3dRenderObjのための領域をヒープ領域から確保します。
    イニシャライズは別途NNS_G3dRenderObjInitで行ってください。
 *---------------------------------------------------------------------------*/
NNSG3dRenderObj*
NNS_G3dAllocRenderObj(NNSFndAllocator* pAlloc)
{
    NNS_G3D_NULL_ASSERT(pAlloc);

    return (NNSG3dRenderObj*)
           NNS_FndAllocFromAllocator(pAlloc, sizeof(NNSG3dRenderObj));
}


/*---------------------------------------------------------------------------*
    NNS_G3dFreeRenderObj

    NNSG3dRenderObjの領域を解放します。
    NNSG3dRenderObjが保持しているポインタが参照する領域は解放しません。
    別途ユーザーが解放する必要があります。
 *---------------------------------------------------------------------------*/
void
NNS_G3dFreeRenderObj(NNSFndAllocator* pAlloc,
                     NNSG3dRenderObj* pRenderObj)
{
    NNS_G3D_NULL_ASSERT(pAlloc);

    NNS_FndFreeToAllocator(pAlloc, pRenderObj);
}


/*---------------------------------------------------------------------------*
    NNS_G3dAllocAnmObj

    NNSG3dAnmObjのための領域をヒープ領域から確保します。
    イニシャライズは別途NNS_G3dAnmObjInitで行ってください。
 *---------------------------------------------------------------------------*/
NNSG3dAnmObj*
NNS_G3dAllocAnmObj(NNSFndAllocator* pAlloc,
                   const void* pAnm,
                   const NNSG3dResMdl* pMdl)
{
    u32 sz;
    NNS_G3D_NULL_ASSERT(pAlloc);
    NNS_G3D_NULL_ASSERT(pAnm);
    NNS_G3D_NULL_ASSERT(pMdl);

    sz = NNS_G3dAnmObjCalcSizeRequired(pAnm, pMdl);
    return (NNSG3dAnmObj*) NNS_FndAllocFromAllocator(pAlloc, sz);
}


/*---------------------------------------------------------------------------*
    NNS_G3dFreeAnmObj

    NNSG3dAnmObjの領域を解放します。
    NNSG3dAnmObjが保持しているポインタが参照する領域は解放しません。
    別途ユーザーが解放する必要があります。
 *---------------------------------------------------------------------------*/
void
NNS_G3dFreeAnmObj(NNSFndAllocator* pAlloc,
                  NNSG3dAnmObj* pAnmObj)
{
    NNS_G3D_NULL_ASSERT(pAlloc);

    NNS_FndFreeToAllocator(pAlloc, pAnmObj);
}


/*---------------------------------------------------------------------------*
    NNS_G3dAllocRecBufferJnt

    NNSG3dRenderObjが保持することができる、ジョイントの計算結果を保持するための
    バッファ領域を確保します。領域の大きさはNNSG3dResMdlに存在するジョイントの数
    によって決まります。
 *---------------------------------------------------------------------------*/
NNSG3dJntAnmResult*
NNS_G3dAllocRecBufferJnt(NNSFndAllocator* pAlloc,
                         const NNSG3dResMdl* pResMdl)
{
    u32 numJnt;
    NNS_G3D_NULL_ASSERT(pAlloc);
    NNS_G3D_NULL_ASSERT(pResMdl);

    numJnt = pResMdl->info.numNode;

    return (NNSG3dJntAnmResult*)
            NNS_FndAllocFromAllocator(pAlloc,
                                      NNS_G3D_RENDEROBJ_JNTBUFFER_SIZE(numJnt));
}


/*---------------------------------------------------------------------------*
    NNS_G3dFreeRecBufferJnt

    ジョイントの計算結果を保持するためのバッファ領域を解放します。
 *---------------------------------------------------------------------------*/
void
NNS_G3dFreeRecBufferJnt(NNSFndAllocator* pAlloc,
                        NNSG3dJntAnmResult* pRecBuf)
{
    NNS_G3D_NULL_ASSERT(pAlloc);

    NNS_FndFreeToAllocator(pAlloc, pRecBuf);
}


/*---------------------------------------------------------------------------*
    NNS_G3dAllocRecBufferMat

    NNSG3dRenderObjが保持することができる、マテリアルの計算結果を保持するための
    バッファ領域を確保します。領域の大きさはNNSG3dResMdlに存在するマテリアルの数
    によって決まります。
 *---------------------------------------------------------------------------*/
NNSG3dMatAnmResult*
NNS_G3dAllocRecBufferMat(NNSFndAllocator* pAlloc,
                         const NNSG3dResMdl* pResMdl)
{
    u32 numMat;
    NNS_G3D_NULL_ASSERT(pAlloc);
    NNS_G3D_NULL_ASSERT(pResMdl);   

    numMat = pResMdl->info.numMat;

    return (NNSG3dMatAnmResult*)
            NNS_FndAllocFromAllocator(pAlloc,
                                      NNS_G3D_RENDEROBJ_MATBUFFER_SIZE(numMat));
}


/*---------------------------------------------------------------------------*
    NNS_G3dFreeRecBufferMat

    マテリアルの計算結果を保持するためのバッファ領域を解放します。
 *---------------------------------------------------------------------------*/
void
NNS_G3dFreeRecBufferMat(NNSFndAllocator* pAlloc,
                        NNSG3dMatAnmResult* pRecBuf)
{
    NNS_G3D_NULL_ASSERT(pAlloc);

    NNS_FndFreeToAllocator(pAlloc, pRecBuf);
}
