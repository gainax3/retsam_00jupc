/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g3d - src - cgtool
  File:     si3d.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.19 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#include <nnsys/g3d/cgtool/si3d.h>

#ifndef NNS_G3D_SI3D_DISABLE
#include <nnsys/g3d/kernel.h>
#include <nnsys/g3d/gecom.h>
#include <nnsys/g3d/sbc.h>
#include <nnsys/g3d/util.h>

#include <nitro/code32.h>

/*---------------------------------------------------------------------------*
    NNSi_G3dSendJointSRTSi3d

    ジョイント行列をジオメトリエンジンに設定する。
    通常、NNS_G3dSendJointSRT_FuncArrayにポインタが格納されていて、
    NNS_G3D_SCALINGRULE_SI3D(<model_info>::scaling_ruleがsi3dの場合)
    が指定されていた場合に呼び出されるようになっている。

    また、呼び出し時には、Position/Vectorモードであり、加工対象の行列が
    カレント行列に入っている必要がある。
 *---------------------------------------------------------------------------*/
void NNSi_G3dSendJointSRTSi3d(const NNSG3dJntAnmResult* result)
{
    BOOL rtFlag = FALSE;
    // この時点で
    // 行列モードはPosition/Vectorモードでなければならない。
    // 加工対象の行列がカレント行列になっていなければならない。

    // 要するに以下のような計算をしている
    // a(親)->b(子)->c(孫)とすると、
    // Sc * (SbSa) * Rc * T'c * inv(SbSa) * Sb * Sa * Rb * T'b * inv(Sa) * Sa * Ra * T'a

    u32 flagScaleEx = result->flag & (u32)(NNS_G3D_JNTANM_RESULTFLAG_SCALEEX0_ONE |
                                           NNS_G3D_JNTANM_RESULTFLAG_SCALEEX1_ONE);

    //
    // ScaleEx0にはスケールを累乗していった数が、
    // ScaleEx1にはスケールの逆数を累乗していった数が入っている。
    // よってどちらか片方だけが1ということはない。
    //
    NNS_G3D_ASSERT(flagScaleEx == 0 ||
                   flagScaleEx == (NNS_G3D_JNTANM_RESULTFLAG_SCALEEX0_ONE |
                                   NNS_G3D_JNTANM_RESULTFLAG_SCALEEX1_ONE));

    if (!flagScaleEx)
    {
        // 親までの逆数スケールが1でない場合はスケーリングを行う
        NNS_G3dGeBufferOP_N(G3OP_MTX_SCALE,
                            (u32*)&result->scaleEx1.x,
                            G3OP_MTX_SCALE_NPARAMS);
    }

    if (!(result->flag & NNS_G3D_JNTANM_RESULTFLAG_TRANS_ZERO))
    {
        // Transが0でない場合
        if (!flagScaleEx)
        {
            // 親までの逆数スケールが1でない場合、translationに所定の乗算を行う
            VecFx32 tmp;
            tmp.x = (fx32)((fx64)result->trans.x * result->scaleEx0.x >> FX32_SHIFT);
            tmp.y = (fx32)((fx64)result->trans.y * result->scaleEx0.y >> FX32_SHIFT);
            tmp.z = (fx32)((fx64)result->trans.z * result->scaleEx0.z >> FX32_SHIFT);
            
            NNS_G3dGeBufferOP_N(G3OP_MTX_TRANS,
                                (u32*)&tmp.x,
                                G3OP_MTX_TRANS_NPARAMS);
        }
        else
        {
            rtFlag = TRUE;
        }
    }

    if (!(result->flag & NNS_G3D_JNTANM_RESULTFLAG_ROT_ZERO))
    {
        // 回転行列が単位行列でない場合、回転行列を乗算する。
        if (rtFlag)
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
            NNS_G3dGeBufferOP_N(G3OP_MTX_MULT_3x3,
                                (u32*)&result->rot._00,
                                G3OP_MTX_MULT_3x3_NPARAMS);
        }
    }
    else
    {
        if (rtFlag)
        {
            // 親までの逆数スケールが1の場合、そのままtranslationを送る
            NNS_G3dGeBufferOP_N(G3OP_MTX_TRANS,
                                (u32*)&result->trans.x,
                                G3OP_MTX_TRANS_NPARAMS);
        }
    }

    if (!flagScaleEx)
    {
        // 親までのスケールが1でない場合はスケーリングを行う
        NNS_G3dGeBufferOP_N(G3OP_MTX_SCALE,
                            (u32*)&result->scaleEx0.x,
                            G3OP_MTX_SCALE_NPARAMS);
    }

    if (!(result->flag & NNS_G3D_JNTANM_RESULTFLAG_SCALE_ONE))
    {
        // 自身のスケールが1でない場合はスケーリングを行う
        NNS_G3dGeBufferOP_N(G3OP_MTX_SCALE,
                            (u32*)&result->scale.x,
                            G3OP_MTX_SCALE_NPARAMS);
    }
}


/*---------------------------------------------------------------------------*
    NNSi_G3dGetJointScaleSi3d

    Si3dのClassic Scale Offを考慮したスケーリングを行います。
    pResult->scaleEx0には親ノードまでの累積スケール値ベクトル,
    pResult->scaleEx1には親ノードまでの累積逆数スケール値ベクトルが入ります。
    NNS_G3dRSOnGlb.scaleCache内には自身の累積スケール値ベクトル・
    累積逆数スケール値が入ります。
 *---------------------------------------------------------------------------*/
void
NNSi_G3dGetJointScaleSi3d(NNSG3dJntAnmResult* pResult,
                          const fx32* p,
                          const u8* cmd,
                          u32 srtflag)
{
    //
    // srtflagはNNSG3dResNodeData::flagである。
    // cmdは現在処理しているSBCを指す(NODEDESCコマンド)
    //

    //
    // ScaleEx0, ScaleEx1には親までの累積スケール値が入る
    //
    u32 nodeID;
    u32 parentID;
    NNS_G3D_NULL_ASSERT(pResult);
    NNS_G3D_NULL_ASSERT(cmd);
    NNS_G3D_NULL_ASSERT(p);
    NNS_G3D_NULL_ASSERT(NNS_G3dRS);
    
    nodeID = *(cmd + 1);
    parentID = *(cmd + 2);

    if (srtflag & NNS_G3D_SRTFLAG_SCALE_ONE)
    {
        // 自身のスケール値は1なのでフラグセット
        pResult->flag |= NNS_G3D_JNTANM_RESULTFLAG_SCALE_ONE;

        if (NNSi_G3dBitVecCheck(&NNS_G3dRS->isScaleCacheOne[0], parentID))
        {
            // 累積スケール値は1のまま
            NNSi_G3dBitVecSet(&NNS_G3dRS->isScaleCacheOne[0], nodeID);

            pResult->flag |= (NNS_G3D_JNTANM_RESULTFLAG_SCALEEX0_ONE |
                              NNS_G3D_JNTANM_RESULTFLAG_SCALEEX1_ONE);
        }
        else
        {
            // 親の累積スケール値を持ち越す
            MI_CpuCopy32(&NNS_G3dRSOnGlb.scaleCache[parentID],
                         &NNS_G3dRSOnGlb.scaleCache[nodeID],
                         sizeof(VecFx32) + sizeof(VecFx32));

            MI_CpuCopy32(&NNS_G3dRSOnGlb.scaleCache[parentID],
                         &pResult->scaleEx0,
                         sizeof(VecFx32) + sizeof(VecFx32));
        }
    }
    else
    {
        // 自身のスケールをセット
        pResult->scale.x = *(p + 0);
        pResult->scale.y = *(p + 1);
        pResult->scale.z = *(p + 2);

        if (NNSi_G3dBitVecCheck(&NNS_G3dRS->isScaleCacheOne[0], parentID))
        {
            // 自身のスケール値が累積スケール値になる
            // NNSG3dResNodeDataのSx,Sy,Sz,InvSx,InvSy,InvSzをコピーしている。
            MI_CpuCopy32(p,
                         &NNS_G3dRSOnGlb.scaleCache[nodeID],
                         sizeof(VecFx32) + sizeof(VecFx32));

            // NOTICE
            // nodeID=0のscaleOneフラグは事前にセットされています。
            // これは、根ノードの親ノードがscaleOneであることを示すためのもので、
            // 根ノードがscaleOneでない場合はリセットしなくてはいけません。
            // なお、このコードをifの外に出すのは間違いです。
            NNSi_G3dBitVecReset(&NNS_G3dRS->isScaleCacheOne[0], nodeID);

            // 親までの累積スケール値は1なのでフラグをセット
            pResult->flag |= (NNS_G3D_JNTANM_RESULTFLAG_SCALEEX0_ONE |
                              NNS_G3D_JNTANM_RESULTFLAG_SCALEEX1_ONE);
        }
        else
        {
            // NOTICE
            // nodeID=0のscaleOneフラグは事前にセットされています。
            // これは、根ノードの親ノードがscaleOneであることを示すためのもので、
            // 根ノードがscaleOneでない場合はリセットしなくてはいけません。
            // なお、コードをifの外に出すのは間違いです。
            NNSi_G3dBitVecReset(&NNS_G3dRS->isScaleCacheOne[0], nodeID);

            // 親の累積スケール値に自身のスケール値をかけて自身の累積スケール値を得る
            NNS_G3dRSOnGlb.scaleCache[nodeID].s.x =
                (fx32)((fx64)(*(p + 0)) * NNS_G3dRSOnGlb.scaleCache[parentID].s.x >> FX32_SHIFT);
            NNS_G3dRSOnGlb.scaleCache[nodeID].s.y =
                (fx32)((fx64)(*(p + 1)) * NNS_G3dRSOnGlb.scaleCache[parentID].s.y >> FX32_SHIFT);
            NNS_G3dRSOnGlb.scaleCache[nodeID].s.z =
                (fx32)((fx64)(*(p + 2)) * NNS_G3dRSOnGlb.scaleCache[parentID].s.z >> FX32_SHIFT);
            
            NNS_G3dRSOnGlb.scaleCache[nodeID].inv.x =
                (fx32)((fx64)(*(p + 3)) * NNS_G3dRSOnGlb.scaleCache[parentID].inv.x >> FX32_SHIFT);
            NNS_G3dRSOnGlb.scaleCache[nodeID].inv.y =
                (fx32)((fx64)(*(p + 4)) * NNS_G3dRSOnGlb.scaleCache[parentID].inv.y >> FX32_SHIFT);
            NNS_G3dRSOnGlb.scaleCache[nodeID].inv.z =
                (fx32)((fx64)(*(p + 5)) * NNS_G3dRSOnGlb.scaleCache[parentID].inv.z >> FX32_SHIFT);

            // 親までの累積スケール値をセット
            MI_CpuCopy32(&NNS_G3dRSOnGlb.scaleCache[parentID],
                         &pResult->scaleEx0,
                         sizeof(VecFx32) + sizeof(VecFx32));
        }
    }
}


/*---------------------------------------------------------------------------*
    NNSi_G3dSendTexSRTSi3d

    SoftImage3Dで作成されたモデルデータのテクスチャ行列を設定します。
 *---------------------------------------------------------------------------*/
void NNSi_G3dSendTexSRTSi3d(const NNSG3dMatAnmResult* anm)
{
    struct
    {
        u32     cmd;
        u32     mode_tex;
        MtxFx43 m;
        u32     mode_pos_vec;
    }
    data;

    NNS_G3D_NULL_ASSERT(anm);
    NNS_G3D_ASSERT(anm->flag & (NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SET |
                                NNS_G3D_MATANM_RESULTFLAG_TEXMTX_MULT));

    if (anm->flag & NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SET)
    {
        data.cmd = GX_PACK_OP(G3OP_MTX_MODE, G3OP_MTX_LOAD_4x3, G3OP_MTX_MODE, G3OP_NOP);
    }
    else
    {
        data.cmd = GX_PACK_OP(G3OP_MTX_MODE, G3OP_MTX_MULT_4x3, G3OP_MTX_MODE, G3OP_NOP);
    }

    data.mode_tex     = GX_MTXMODE_TEXTURE;
    data.mode_pos_vec = GX_MTXMODE_POSITION_VECTOR;

    data.m._01 = data.m._02 = data.m._10 = data.m._12 = data.m._20 =
    data.m._21 = data.m._22 = data.m._32 = 0;

    if (anm->flag & NNS_G3D_MATANM_RESULTFLAG_TEXMTX_TRANSZERO)
    {
        data.m._30 = 0;
        data.m._31 = 0;

        if (anm->flag & NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SCALEONE)
        {
            data.m._00 = FX32_ONE;
            data.m._11 = FX32_ONE;
        }
        else
        {
            data.m._00 = anm->scaleS;
            data.m._11 = anm->scaleT;
        }
    }
    else if (anm->flag & NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SCALEONE)
    {
        data.m._30 = -(anm->transS << 4) * anm->origWidth;
        data.m._31 = -(anm->transT << 4) * anm->origHeight;
        data.m._00 = FX32_ONE;
        data.m._11 = FX32_ONE;
    }
    else
    {
        data.m._30 = -(fx32)((fx64)anm->scaleS * anm->transS >> (FX32_SHIFT - 4)) * anm->origWidth;
        data.m._31 = -(fx32)((fx64)anm->scaleT * anm->transT >> (FX32_SHIFT - 4)) * anm->origHeight;
        data.m._00 = anm->scaleS;
        data.m._11 = anm->scaleT;
    }

    if (anm->magW != FX32_ONE)
    {
        data.m._00 = (fx32)((fx64)anm->magW * data.m._00 >> FX32_SHIFT);
        data.m._30 = (fx32)((fx64)anm->magW * data.m._30 >> FX32_SHIFT);

    }

    if (anm->magH != FX32_ONE)
    {
        data.m._11 = (fx32)((fx64)anm->magH * data.m._11 >> FX32_SHIFT);
        data.m._31 = (fx32)((fx64)anm->magH * data.m._31 >> FX32_SHIFT);
    }

    NNS_G3dGeBufferData_N((u32*)&data.cmd, sizeof(data) / 4);
}

#endif // NNS_G3D_SI3D_DISABLE


