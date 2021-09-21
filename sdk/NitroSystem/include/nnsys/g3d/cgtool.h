/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g3d
  File:     cgtool.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.9 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#ifndef NNSG3D_CGTOOL_H_
#define NNSG3D_CGTOOL_H_

#include <nnsys/g3d/config.h>
#include <nnsys/g3d/anm.h>

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*
    NNSG3dGetJointScale

    ジョイントのスケール計算を行う関数へのポインタ
    CGツール毎に違いがあるため分離しておく。
*---------------------------------------------------------------------------*/
typedef void (*NNSG3dGetJointScale)(NNSG3dJntAnmResult* pResult,
                                    const fx32* p,
                                    const u8* cmd,
                                    u32 srtflag);

/*---------------------------------------------------------------------------*
    NNSG3dSendJointSRT

    ジョイント行列をジオメトリエンジンに設定する。
    呼ばれる時点での行列モードはPosition/Vectorモードになっている必要がある。
    また、加工対象の行列はカレント行列に入っている必要がある。
 *---------------------------------------------------------------------------*/
typedef void (*NNSG3dSendJointSRT)(const NNSG3dJntAnmResult*);


/*---------------------------------------------------------------------------*
    NNSG3dSendTexSRT

    テクスチャSRT行列をジオメトリエンジンに設定する。
    実行後は行列モードがPosition/Vectorモードになっている。
 *---------------------------------------------------------------------------*/
typedef void (*NNSG3dSendTexSRT)(const NNSG3dMatAnmResult*);


/*---------------------------------------------------------------------------*
    NNS_G3dSendJointSRT_FuncArray

    NNSG3dScalingRule(<model_info>::scaling_rule)の値に対応する
    関数ポインタの配列。ジョイント行列を送信する場合、G3Dからは
    この関数ポインタベクタを通してアクセスしなければならない。
 *---------------------------------------------------------------------------*/
extern NNSG3dGetJointScale NNS_G3dGetJointScale_FuncArray[NNS_G3D_FUNC_SENDJOINTSRT_MAX];
extern NNSG3dSendJointSRT NNS_G3dSendJointSRT_FuncArray[NNS_G3D_FUNC_SENDJOINTSRT_MAX];


/*---------------------------------------------------------------------------*
    NNS_G3dSendTexSRT_FuncArray

    NNSG3dTexMtxMode(<model_info>::tex_matrix_mode)の値に対応する
    関数ポインタの配列。テクスチャ行列を送信する場合、G3Dからは
    この関数ポインタベクタを通してアクセスしなければならない。
 *---------------------------------------------------------------------------*/
extern NNSG3dSendTexSRT   NNS_G3dSendTexSRT_FuncArray[NNS_G3D_FUNC_SENDTEXSRT_MAX];


#ifdef __cplusplus
}/* extern "C" */
#endif

#endif
