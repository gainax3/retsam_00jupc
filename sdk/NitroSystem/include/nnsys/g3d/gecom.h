/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g3d
  File:     gecom.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.14 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#ifndef NNSG3D_GECOM_H_
#define NNSG3D_GECOM_H_

#include <nnsys/g3d/config.h>

#ifdef __cplusplus
extern "C" {
#endif

//
// ジオメトリコマンド/ディスプレイリスト転送ラッパ
// コマンドのキャッシングを行う場合に備えて、
// ジオメトリコマンドはここの関数を経由して送信すること。
//

////////////////////////////////////////////////////////////////////////////////
//
// 構造体定義及びtypedef
//

/*---------------------------------------------------------------------------*
    NNSG3dGeBuffer

    以前のコマンドが転送中のときにキャッシングするためのデータ構造。
    通常はスタック領域(DTCM)に確保する。
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dGeBuffer_
{
    u32 idx;

    // ジオメトリコマンドリストの転送中(NNS_G3dGeSendDL)に
    // ジオメトリコマンドを送信したい場合は、ここにバッファリングされる。
    u32 data[NNS_G3D_SIZE_COMBUFFER];
}
NNSG3dGeBuffer;


////////////////////////////////////////////////////////////////////////////////
//
// 関数の宣言
//

//
// inline functions
//
NNS_G3D_INLINE void NNS_G3dGeBufferData_N(const u32* args, u32 num);
NNS_G3D_INLINE void NNS_G3dGeMtxMode(GXMtxMode mode);
NNS_G3D_INLINE void NNS_G3dGePushMtx(void);
NNS_G3D_INLINE void NNS_G3dGePopMtx(int num);
NNS_G3D_INLINE void NNS_G3dGeStoreMtx(int num);
NNS_G3D_INLINE void NNS_G3dGeRestoreMtx(int num);
NNS_G3D_INLINE void NNS_G3dGeIdentity(void);
NNS_G3D_INLINE void NNS_G3dGeScaleVec(const VecFx32* vec);
NNS_G3D_INLINE void NNS_G3dGeScale(fx32 x, fx32 y, fx32 z);
NNS_G3D_INLINE void NNS_G3dGeTranslateVec(const VecFx32* vec);
NNS_G3D_INLINE void NNS_G3dGeTranslate(fx32 x, fx32 y, fx32 z);
NNS_G3D_INLINE void NNS_G3dGeColor(GXRgb rgb);
NNS_G3D_INLINE void NNS_G3dGeNormal(fx16 x, fx16 y, fx16 z);
NNS_G3D_INLINE void NNS_G3dGeTexCoord(fx32 s, fx32 t);
NNS_G3D_INLINE void NNS_G3dGeVtx(fx16 x, fx16 y, fx16 z);
NNS_G3D_INLINE void NNS_G3dGeVtx10(fx16 x, fx16 y, fx16 z);
NNS_G3D_INLINE void NNS_G3dGeVtxXY(fx16 x, fx16 y);
NNS_G3D_INLINE void NNS_G3dGeVtxXZ(fx16 x, fx16 z);
NNS_G3D_INLINE void NNS_G3dGeVtxYZ(fx16 y, fx16 z);
NNS_G3D_INLINE void NNS_G3dGeVtxDiff(fx16 x, fx16 y, fx16 z);
NNS_G3D_INLINE void NNS_G3dGePolygonAttr(
                        int light,
                        GXPolygonMode polyMode,
                        GXCull cullMode,
                        int polygonID,
                        int alpha,
                        int misc // GXPolygonAttrMisc
                        );
NNS_G3D_INLINE void NNS_G3dGeTexImageParam(
                        GXTexFmt texFmt,
                        GXTexGen texGen,
                        GXTexSizeS s,
                        GXTexSizeT t,
                        GXTexRepeat repeat,
                        GXTexFlip flip,
                        GXTexPlttColor0 pltt0,
                        u32 addr
                        );
NNS_G3D_INLINE void NNS_G3dGeTexPlttBase(u32 addr, GXTexFmt texfmt);
NNS_G3D_INLINE void NNS_G3dGeMaterialColorDiffAmb(
                        GXRgb diffuse,
                        GXRgb ambient,
                        BOOL IsSetVtxColor);
NNS_G3D_INLINE void NNS_G3dGeMaterialColorSpecEmi(
                        GXRgb specular,
                        GXRgb emission,
                        BOOL IsShininess);
NNS_G3D_INLINE void NNS_G3dGeLightVector(
                        GXLightId lightID,
                        fx16 x,
                        fx16 y,
                        fx16 z);
NNS_G3D_INLINE void NNS_G3dGeLightColor(GXLightId lightID, GXRgb rgb);
NNS_G3D_INLINE void NNS_G3dGeBegin(GXBegin primitive);
NNS_G3D_INLINE void NNS_G3dGeEnd(void);
NNS_G3D_INLINE void NNS_G3dGeViewPort(int x1, int y1, int x2, int y2);
NNS_G3D_INLINE void NNS_G3dGeBoxTest(const GXBoxTestParam * box);
NNS_G3D_INLINE void NNS_G3dGePositionTest(fx16 x, fx16 y, fx16 z);
NNS_G3D_INLINE void NNS_G3dGeVectorTest(fx16 x, fx16 y, fx16 z);

NNS_G3D_INLINE void NNS_G3dGeLoadMtx44(const MtxFx44* m);
NNS_G3D_INLINE void NNS_G3dGeLoadMtx43(const MtxFx43* m);
NNS_G3D_INLINE void NNS_G3dGeMultMtx44(const MtxFx44 * m);
NNS_G3D_INLINE void NNS_G3dGeMultMtx43(const MtxFx43 * m);
NNS_G3D_INLINE void NNS_G3dGeMultMtx33(const MtxFx33 * m);
NNS_G3D_INLINE void NNS_G3dGeShininess(const u32 * table);

//
// non-inline functions
//
BOOL NNS_G3dGeIsSendDLBusy(void);
BOOL NNS_G3dGeIsBufferExist(void);
BOOL NNS_G3dGeIsImmOK(void);
BOOL NNS_G3dGeIsBufferOK(u32 numWord);
void NNS_G3dGeWaitSendDL(void);
void NNS_G3dGeSetBuffer(NNSG3dGeBuffer* p);
NNSG3dGeBuffer* NNS_G3dGeReleaseBuffer(void);
void NNS_G3dGeFlushBuffer(void);
void NNS_G3dGeSendDL(const void* src, u32 szByte);
void NNS_G3dGeBufferOP_N(u32 op, const u32* args, u32 num);
void NNS_G3dGeUseFastDma(BOOL cond); // Undocumented

////////////////////////////////////////////////////////////////////////////////
//
// グローバル変数
//

#ifdef __cplusplus
}/* extern "C" */
#endif

#include <nnsys/g3d/gecom_inline.h>

#endif
