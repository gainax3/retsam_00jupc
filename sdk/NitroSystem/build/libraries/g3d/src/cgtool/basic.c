/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g3d - src - cgtool
  File:     basic.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.11 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#include <nnsys/g3d/cgtool/basic.h>
#include <nnsys/g3d/gecom.h>
#include <nnsys/g3d/binres/res_struct.h>


/*---------------------------------------------------------------------------*
    NNSi_G3dSendJointSRTBasic

    ジョイント行列をジオメトリエンジンに設定する。
    通常、NNS_G3dSendJointSRT_FuncArrayにポインタが格納されていて、
    NNS_G3D_SCALINGRULE_STANDARD(<model_info>::scaling_ruleがstandardの場合)
    が指定されていた場合に呼び出されるようになっている。

    また、呼び出し時には、Position/Vectorモードであり、加工対象の行列が
    カレント行列に入っている必要がある。
 *---------------------------------------------------------------------------*/
void NNSi_G3dSendJointSRTBasic(const NNSG3dJntAnmResult* result)
{
    NNS_G3D_NULL_ASSERT(result);

    // この時点で
    // 行列モードはPosition/Vectorモードでなければならない。
    // 加工対象の行列がカレント行列になっていなければならない。
    if (!(result->flag & NNS_G3D_JNTANM_RESULTFLAG_TRANS_ZERO))
    {
        if (!(result->flag & NNS_G3D_JNTANM_RESULTFLAG_ROT_ZERO))
        {
            // HACK ALLERT
            // rotとtransは連続しているということが前提
            // つまり、コードはNNSG3dJntAnmResultのメンバの順番に依存している。
            NNS_G3dGeBufferOP_N(G3OP_MTX_MULT_4x3,
                                (u32*)&result->rot._00,
                                G3OP_MTX_MULT_4x3_NPARAMS);
        }
        else
        {
            // Position/Vectorモードは遅いが、
            // モードを切り替えるためのコマンド送信のために
            // CPUを余計に使うのは望ましくない。
            NNS_G3dGeBufferOP_N(G3OP_MTX_TRANS,
                                (u32*)&result->trans.x,
                                G3OP_MTX_TRANS_NPARAMS);
        }
    }
    else
    {
        if (!(result->flag & NNS_G3D_JNTANM_RESULTFLAG_ROT_ZERO))
        {
            NNS_G3dGeBufferOP_N(G3OP_MTX_MULT_3x3,
                                (u32*)&result->rot._00,
                                G3OP_MTX_MULT_3x3_NPARAMS);
        }
    }

    if (!(result->flag & NNS_G3D_JNTANM_RESULTFLAG_SCALE_ONE))
    {
        NNS_G3dGeBufferOP_N(G3OP_MTX_SCALE,
                            (u32*)&result->scale.x,
                            G3OP_MTX_SCALE_NPARAMS);
    }
}


/*---------------------------------------------------------------------------*
    NNSi_G3dGetJointScaleBasic

    ごく普通のスケーリングを行います。*(p + 3), *(p + 4), *(p + 5)は使用しません
 *---------------------------------------------------------------------------*/
void NNSi_G3dGetJointScaleBasic(NNSG3dJntAnmResult* pResult,
                                const fx32* p,
                                const u8*,
                                u32 srtflag)
{
    // NOTICE:
    // cmdはNULLでもよい
    // NNS_G3D_SRTFLAG_SCALE_ONEがONならpはNULLでもよい

    NNS_G3D_NULL_ASSERT(pResult);
    
    if (srtflag & NNS_G3D_SRTFLAG_SCALE_ONE)
    {
        pResult->flag |= NNS_G3D_JNTANM_RESULTFLAG_SCALE_ONE;
    }
    else
    {
        NNS_G3D_NULL_ASSERT(p);

        pResult->scale.x = *(p + 0);
        pResult->scale.y = *(p + 1);
        pResult->scale.z = *(p + 2);
    }

    // 使用しないがセットしておくのが無難(ブレンドとの兼ね合い)
    pResult->flag |= NNS_G3D_JNTANM_RESULTFLAG_SCALEEX0_ONE |
                     NNS_G3D_JNTANM_RESULTFLAG_SCALEEX1_ONE;
}

