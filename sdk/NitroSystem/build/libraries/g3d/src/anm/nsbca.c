/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g3d - src - anm
  File:     nsbca.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.29 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#include <nnsys/g3d/anm/nsbca.h>
#include <nnsys/g3d/sbc.h>
#include <nnsys/g3d/binres/res_struct_accessor.h>

#ifndef NNS_G3D_NSBCA_DISABLE
/*---------------------------------------------------------------------------*
    NNSi_G3dAnmObjInitNsBca

    NNSG3dAnmObjを.nsbcaリソース用にイニシャライズします。
    NNS_G3dInitAnmObjから呼び出されます。
 *---------------------------------------------------------------------------*/
void
NNSi_G3dAnmObjInitNsBca(NNSG3dAnmObj* pAnmObj,
                        void* pResAnm,
                        const NNSG3dResMdl* pResMdl)
{
    u32 i;
    u16* ofsArray;
    NNSG3dResJntAnm* jntAnm;
    const NNSG3dResNodeInfo* jnt;

    NNS_G3D_NULL_ASSERT(pAnmObj);
    NNS_G3D_NULL_ASSERT(pResAnm);
    NNS_G3D_NULL_ASSERT(pResMdl);

    pAnmObj->resAnm = pResAnm;
    jntAnm = (NNSG3dResJntAnm*)pResAnm;
    jnt = NNS_G3dGetNodeInfo(pResMdl);
    pAnmObj->funcAnm = NNS_G3dFuncAnmJntNsBcaDefault;
    pAnmObj->numMapData = pResMdl->info.numNode;

    
    // まずはmapDataをゼロクリア
    MI_CpuClear16(&pAnmObj->mapData[0], sizeof(u16) * pAnmObj->numMapData);

    //
    // NNSG3dJntAnmSRTTag内にモデルのノードIDが格納されている。
    //
    ofsArray = (u16*)((u8*)jntAnm + sizeof(NNSG3dResJntAnm));

    // アニメーションオブジェクトで確保されたmapData配列のサイズを超えないかチェック
    NNS_G3D_ASSERT(jntAnm->numNode <= pAnmObj->numMapData);
    
    for (i = 0; i < jntAnm->numNode; ++i)
    {
        // .nsbcaの場合は、現在リソースのインデックスとモデルのノードIDが一致するようになっている.
        NNSG3dResJntAnmSRTTag* pTag =
            (NNSG3dResJntAnmSRTTag*)((u8*)jntAnm + ofsArray[i]);
        pAnmObj->mapData[i] = (u16)((pTag->tag >> NNS_G3D_JNTANM_SRTINFO_NODE_SHIFT) |
                                    NNS_G3D_ANMOBJ_MAPDATA_EXIST);
    }
}


static void
getJntSRTAnmResult_(const NNSG3dResJntAnm* pJntAnm, 
                    u32                    dataIdx, 
                    fx32                   Frame,
                    NNSG3dJntAnmResult*    pResult);

static void
getTransData_(fx32* pVal,
              fx32 Frame,
              const u32* pData,
              const NNSG3dResJntAnm* pJntAnm);

static void
getTransDataEx_(fx32* pVal,
                fx32 Frame,
                const u32* pData,
                const NNSG3dResJntAnm* pJntAnm);

static void
getScaleData_(fx32* s_invs,
              fx32 Frame,
              const u32* pData,
              const NNSG3dResJntAnm* pJntAnm);

static void
getScaleDataEx_(fx32* s_invs,
                fx32 Frame,
                const u32* pData,
                const NNSG3dResJntAnm* pJntAnm);


static void
getRotData_(MtxFx33* pRot,
            fx32 Frame,
            const u32* pData,
            const NNSG3dResJntAnm* pJntAnm);

static void
getRotDataEx_(MtxFx33* pRot,
              fx32 Frame,
              const u32* pData,
              const NNSG3dResJntAnm* pJntAnm);

static BOOL
getRotDataByIdx_(MtxFx33* pRot,
                 const void* pArrayRot3,
                 const void* pArrayRot5,
                 NNSG3dJntAnmRIdx info);


/*---------------------------------------------------------------------------*
    NNSi_G3dAnmCalcNsBca

    pResult: ジョイントアニメーションの結果を格納します。
    pAnmObj:
    dataIdx: リソース内データの格納場所を示すインデックスです
 *---------------------------------------------------------------------------*/
void NNSi_G3dAnmCalcNsBca(NNSG3dJntAnmResult* pResult,
                          const NNSG3dAnmObj* pAnmObj,
                          u32 dataIdx)
{
    NNS_G3D_NULL_ASSERT(pResult);
    NNS_G3D_NULL_ASSERT(pAnmObj);

    //
    // ジョイントアニメーションの実装方法
    // 
    // この関数自体では、
    // pResult->flag,
    // pResult->trans,
    // pResult->rot,
    // を設定することになります。
    // pResult->flagに関しては
    // NNS_G3D_JNTANM_RESULTFLAG_ROT_ZERO
    // NNS_G3D_JNTANM_RESULTFLAG_TRANS_ZERO
    // NNS_G3D_JNTANM_RESULTFLAG_SCALE_ONE
    // をそれぞれ、回転しないとき、移動しないときにセットしておいてください。
    //
    // pResult->scale, pResult->scaleEx0, pResult->scaleEx1に関しては、
    // 引数を用意して、(*NNS_G3dRS->funcJntScale)をコールすることで計算できます。
    // pResult->flagの残りのフラグもここで設定されます。
    // CGツール毎に別の計算をしなくてはならないので、こうなってます。具体的には、
    // NNS_G3dGetJointScaleBasic,
    // NNS_G3dGetJointScaleMaya,
    // NNS_G3dGetJointScaleSi3d
    // のどれかが呼ばれます。
    // 引数は、
    // void NNS_G3dGetJointScaleBasic(NNSG3dJntAnmResult* pResult,
    //                                const fx32* p,
    //                                const u8* cmd,
    //                                u32 srtflag)
    // な感じになっていて、pResultを加工するようになっています。
    // 2番目以降の引数について説明しておきます。
    // pは、ジョイントに関するスケールとスケールの逆数が入った6ワードの配列になります。
    // cmdにはNNS_G3dRS->cを入れます。
    // srtflagには、NNSG3dSRTFlag(res_struct.hで定義)に合致するような
    // フラグを構成して入れてください。
    
    {
        fx32 frame;
        NNSG3dResJntAnm* anm = (NNSG3dResJntAnm*)(pAnmObj->resAnm);

        if (pAnmObj->frame >= (anm->numFrame << FX32_SHIFT))
            frame = (anm->numFrame << FX32_SHIFT) - 1;
        else if (pAnmObj->frame < 0)
            frame = 0;
        else
            frame = pAnmObj->frame;
        
        //
        // ジョイントアニメリソースpAnmのdataIdx番目のリソースのframe目の
        // 結果をresultに入れる
        //

        getJntSRTAnmResult_(anm,
                            dataIdx,
                            frame,
                            pResult);

    }
}


/*---------------------------------------------------------------------------*
    vecCross_

    回転行列の場合、絶対値はFX32_ONE以上にならないので、
    外積もfx64にキャストする必要がない。
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>
static NNS_G3D_INLINE void
vecCross_(const VecFx32 * a, const VecFx32 * b, VecFx32 * axb)
{
    fx32 x, y, z;
    NNS_G3D_NULL_ASSERT(a);
    NNS_G3D_NULL_ASSERT(b);
    NNS_G3D_NULL_ASSERT(axb);

    x = (a->y * b->z - a->z * b->y) >> FX32_SHIFT;
    y = (a->z * b->x - a->x * b->z) >> FX32_SHIFT;
    z = (a->x * b->y - a->y * b->x) >> FX32_SHIFT;

    axb->x = x;
    axb->y = y;
    axb->z = z;
}
#include <nitro/codereset.h>


/*---------------------------------------------------------------------------*
    getMdlTrans_

    モデルのtranslationを使用する
 *---------------------------------------------------------------------------*/
static void 
getMdlTrans_(NNSG3dJntAnmResult* pResult)
{
    u32 idxNode;
    const NNSG3dResNodeData* pNd;
    NNS_G3D_NULL_ASSERT(NNS_G3dRS);

    // HACK ALLERT
    // NODEDESCコマンド内なのでそこからデータをとってくる
    NNS_G3D_ASSERT(NNS_G3D_GET_SBCCMD(*NNS_G3dRS->c) == NNS_G3D_SBC_NODEDESC);
    idxNode = *(NNS_G3dRS->c + 1);
    pNd = NNS_G3dGetNodeDataByIdx(NNS_G3dRS->pResNodeInfo, idxNode);
    
    // Translation
    if (pNd->flag & NNS_G3D_SRTFLAG_TRANS_ZERO)
    {
        // pResult->transに書き込む代わりにフラグを立てるだけにして
        // メモリアクセスとその後の処理コストを抑制する。
        pResult->flag |= NNS_G3D_JNTANM_RESULTFLAG_TRANS_ZERO;
    }
    else
    {
        // NNSG3dResNodeDataはflagの値に従って末尾にデータが入っている
        // res_struct.h内のNNSG3dResNodeDataの定義を参照のこと
        const fx32* p_fx32 = (const fx32*)((const u8*)pNd + sizeof(NNSG3dResNodeData));
        
        pResult->trans.x = *(p_fx32 + 0);
        pResult->trans.y = *(p_fx32 + 1);
        pResult->trans.z = *(p_fx32 + 2);
    }
}


/*---------------------------------------------------------------------------*
    getMdlScale_

    モデルのscaleを使用する
 *---------------------------------------------------------------------------*/
static void
getMdlScale_(NNSG3dJntAnmResult* pResult)
{
    u32 idxNode;
    const NNSG3dResNodeData* pNd;
    const u8* p;
    NNS_G3D_NULL_ASSERT(NNS_G3dRS);
    NNS_G3D_NULL_ASSERT(NNS_G3dRS->funcJntScale);

    // HACK ALLERT
    // NODEDESCコマンド内なのでそこからデータをとってくる
    NNS_G3D_ASSERT(NNS_G3D_GET_SBCCMD(*NNS_G3dRS->c) == NNS_G3D_SBC_NODEDESC);
    idxNode = *(NNS_G3dRS->c + 1);

    // NNSG3dResNodeDataはflagの値に従って末尾にデータが入っている
    // res_struct.h内のNNSG3dResNodeDataの定義を参照のこと
    pNd = NNS_G3dGetNodeDataByIdx(NNS_G3dRS->pResNodeInfo, idxNode);
    p = (const u8*)pNd + sizeof(*pNd);
    
    // Translation
    if (!(pNd->flag & NNS_G3D_SRTFLAG_TRANS_ZERO))
    {
        // Tx, Ty, Tzをスキップする
        p += 3 * sizeof(fx32);
    }

    // Rotation
    if (!(pNd->flag & NNS_G3D_SRTFLAG_ROT_ZERO))
    {
        // 回転データをスキップする
        if (pNd->flag & NNS_G3D_SRTFLAG_PIVOT_EXIST)
        {
            // ピボット形式なのでA, Bをスキップする
            p += 2 * sizeof(fx16);
        }
        else
        {
            // _01, _02, ...., をスキップする
            p += 8 * sizeof(fx16);
        }
    }

    // Scale
    (*NNS_G3dRS->funcJntScale)(pResult, (const fx32*)p, NNS_G3dRS->c, pNd->flag);
}


// Rot3からデータを引いてくる
// 非0、非pivotの要素の位置が格納されている
// 配列の最初のindexがpivotの位置となる。
static const u8 pivotUtil_[9][4] =
{
    {4, 5, 7, 8},
    {3, 5, 6, 8},
    {3, 4, 6, 7},

    {1, 2, 7, 8},
    {0, 2, 6, 8},
    {0, 1, 6, 7},

    {1, 2, 4, 5},
    {0, 2, 3, 5},
    {0, 1, 3, 4}
};


/*---------------------------------------------------------------------------*
    getMdlRot_

    モデルのrotationを使用する
 *---------------------------------------------------------------------------*/
static void
getMdlRot_(NNSG3dJntAnmResult* pResult)
{
    u32 idxNode;
    const NNSG3dResNodeData* pNd;
    const u8* p;
    NNS_G3D_NULL_ASSERT(NNS_G3dRS);

    // HACK ALLERT
    // NODEDESCコマンド内なのでそこからデータをとってくる
    NNS_G3D_ASSERT(NNS_G3D_GET_SBCCMD(*NNS_G3dRS->c) == NNS_G3D_SBC_NODEDESC);
    idxNode = *(NNS_G3dRS->c + 1);

    // NNSG3dResNodeDataはflagの値に従って末尾にデータが入っている
    // res_struct.h内のNNSG3dResNodeDataの定義を参照のこと
    pNd = NNS_G3dGetNodeDataByIdx(NNS_G3dRS->pResNodeInfo, idxNode);
    p = (const u8*)pNd + sizeof(*pNd);
    
    // Translation
    if (!(pNd->flag & NNS_G3D_SRTFLAG_TRANS_ZERO))
    {
        // Tx, Ty, Tzをスキップする
        p += 3 * sizeof(fx32);
    }

    // Rotation
    if (!(pNd->flag & NNS_G3D_SRTFLAG_ROT_ZERO))
    {
        if (pNd->flag & NNS_G3D_SRTFLAG_PIVOT_EXIST)
        {
            // 圧縮されている場合(主に一軸回転)は
            // 圧縮された行列を元に戻す
            fx32 A = *(fx16*)(p + 0);
            fx32 B = *(fx16*)(p + sizeof(fx16));
            u32 idxPivot = (u32)( (pNd->flag & NNS_G3D_SRTFLAG_IDXPIVOT_MASK) >> 
                                            NNS_G3D_SRTFLAG_IDXPIVOT_SHIFT );
            // anmResult.rotをクリア
            MI_Zero36B(&pResult->rot);
            
            pResult->rot.a[idxPivot] =
                (pNd->flag & NNS_G3D_SRTFLAG_PIVOT_MINUS) ?
                    -FX32_ONE :
                    FX32_ONE;
            
            pResult->rot.a[pivotUtil_[idxPivot][0]] = A;
            pResult->rot.a[pivotUtil_[idxPivot][1]] = B;

            pResult->rot.a[pivotUtil_[idxPivot][2]] =
                (pNd->flag & NNS_G3D_SRTFLAG_SIGN_REVC) ? -B : B;

            pResult->rot.a[pivotUtil_[idxPivot][3]] =
                (pNd->flag & NNS_G3D_SRTFLAG_SIGN_REVD) ? -A : A;
        }
        else
        {
            // NOTICE:
            // メモリコピーAPIに置き換えないこと
            // fx16からfx32への暗黙のキャストを行っているから

            const fx16* pp = (const fx16*)p;

            // pResultの3x3回転行列にデータをセットしていく。
            pResult->rot.a[0] = pNd->_00;
            pResult->rot.a[1] = *(pp + 0);
            pResult->rot.a[2] = *(pp + 1);
            pResult->rot.a[3] = *(pp + 2);
            pResult->rot.a[4] = *(pp + 3);
            pResult->rot.a[5] = *(pp + 4);
            pResult->rot.a[6] = *(pp + 5);
            pResult->rot.a[7] = *(pp + 6);
            pResult->rot.a[8] = *(pp + 7);
        }
    }
    else
    {
        // pResult->rotへ書き込む代わりにフラグを立てるだけにして
        // メモリアクセスとその後の処理コストを抑制する。
        pResult->flag |= NNS_G3D_JNTANM_RESULTFLAG_ROT_ZERO;
    }
}


/*---------------------------------------------------------------------------*
    getJntSRTAnmResult_

    pResultにrotation, translation, flagをセット
    pS_invSにスケールとスケールの逆数をセット
 *---------------------------------------------------------------------------*/
static void
getJntSRTAnmResult_(const NNSG3dResJntAnm* pJntAnm, 
                    u32                    dataIdx, 
                    fx32                   Frame,
                    NNSG3dJntAnmResult*    pResult)
{
    NNSG3dResJntAnmSRTTag* pAnmSRTTag;
    NNSG3dJntAnmSRTTag     tag;
    const u32*             pData;
    BOOL                   IsDecimalFrame;
    fx32                   pS_invS[6]; // ポインタ引数からローカル配列に変更

    // 入力検査
    NNS_G3D_NULL_ASSERT(pJntAnm);
    NNS_G3D_NULL_ASSERT(pResult);
    NNS_G3D_ASSERT(dataIdx < pJntAnm->numNode);
    NNS_G3D_ASSERT(Frame < (pJntAnm->numFrame << FX32_SHIFT));
    NNS_G3D_ASSERT(pJntAnm->anmHeader.category0 == 'J' &&
                   pJntAnm->anmHeader.category1 == 'CA');

    {
        const u16* ofsTag = (const u16*)((u8*) pJntAnm + sizeof(NNSG3dResJntAnm));
        // アニメリソース内のdataIdx番目のデータのSRTタグを取得
        pAnmSRTTag = (NNSG3dResJntAnmSRTTag*)((u8*) pJntAnm + ofsTag[dataIdx]);

        tag = (NNSG3dJntAnmSRTTag)pAnmSRTTag->tag;
    }

    // アニメーションデータが存在しない場合をチェック
    if (tag & NNS_G3D_JNTANM_SRTINFO_IDENTITY)
    {
        pResult->flag = (NNSG3dJntAnmResultFlag)(NNS_G3D_JNTANM_RESULTFLAG_SCALE_ONE |
                                                 NNS_G3D_JNTANM_RESULTFLAG_ROT_ZERO  |
                                                 NNS_G3D_JNTANM_RESULTFLAG_TRANS_ZERO);
        goto GET_JOINTSCALE;
    }

    // pAnmSRTTagに続く領域にpAnmSRTTagの値に従った可変長のデータが格納されている
    pData = (const u32*)((const u8*)pAnmSRTTag + sizeof(NNSG3dResJntAnmSRTTag));

    IsDecimalFrame = (BOOL)((Frame & (FX32_ONE - 1)) &&
                            (pJntAnm->flag & NNS_G3D_JNTANM_OPTION_INTERPOLATION));

    // まずリセット
    pResult->flag = (NNSG3dJntAnmResultFlag) 0;

    //
    // res_struct.hのNNSG3dResJntAnmSRTについてのコメントを参照のこと
    //
    
    //
    // Translationの取得
    //
    if (!(tag & (NNS_G3D_JNTANM_SRTINFO_IDENTITY_T | NNS_G3D_JNTANM_SRTINFO_BASE_T)))
    {
        // TX
        if (!(tag & NNS_G3D_JNTANM_SRTINFO_CONST_TX))
        {
            // TXを取り出す関数呼び出し
            // データのオフセットはpAnmSRTTagからのものになる
            if (IsDecimalFrame)
            {
                getTransDataEx_(&pResult->trans.x,
                                Frame,
                                pData,
                                pJntAnm);
            }
            else
            {
                getTransData_(&pResult->trans.x,
                              Frame,
                              pData,
                              pJntAnm);
            }

            // NNSG3dJntAnmTInfoと配列へのオフセットデータの分
            pData += 2;
        }
        else
        {
            pResult->trans.x = *(fx32*)pData;

            // 定数データの分
            pData += 1;
        }

        // TY
        if (!(tag & NNS_G3D_JNTANM_SRTINFO_CONST_TY))
        {
            // TYを取り出す関数呼び出し
            // データのオフセットはpAnmSRTTagからのものになる
            if (IsDecimalFrame)
            {
                getTransDataEx_(&pResult->trans.y,
                                Frame,
                                pData,
                                pJntAnm);
            }
            else
            {
                getTransData_(&pResult->trans.y,
                              Frame,
                              pData,
                              pJntAnm);
            }

            // NNSG3dJntAnmTInfoと配列へのオフセットデータの分
            pData += 2;
        }
        else
        {
            pResult->trans.y = *(fx32*)pData;

            // 定数データの分
            pData += 1;
        }

        // TZ
        if (!(tag & NNS_G3D_JNTANM_SRTINFO_CONST_TZ))
        {
            // TZを取り出す関数呼び出し
            // データのオフセットはpAnmSRTTagからのものになる
            if (IsDecimalFrame)
            {
                getTransDataEx_(&pResult->trans.z,
                                Frame,
                                pData,
                                pJntAnm);
            }
            else
            {
                getTransData_(&pResult->trans.z,
                              Frame,
                              pData,
                              pJntAnm);
            }

            // NNSG3dJntAnmTInfoと配列へのオフセットデータの分
            pData += 2;
        }
        else
        {
            pResult->trans.z = *(fx32*)pData;

            // 定数データの分
            pData += 1;
        }
    }
    else
    {
        if (tag & NNS_G3D_JNTANM_SRTINFO_IDENTITY_T)
        {
            // Trans = (0, 0, 0)
            pResult->flag |= NNS_G3D_JNTANM_RESULTFLAG_TRANS_ZERO;

            // 後続データは存在しないのでpDataを進める必要はない
        }
        else
        {
            // モデルのTransを取得
            getMdlTrans_(pResult);
        }
    }

    //
    // Rotationの取得
    //
    if (!(tag & (NNS_G3D_JNTANM_SRTINFO_IDENTITY_R | NNS_G3D_JNTANM_SRTINFO_BASE_R)))
    {
        if (!(tag & NNS_G3D_JNTANM_SRTINFO_CONST_R))
        {
            // Rを取り出す関数呼び出し
            // データのオフセットはpJntAnm->ofsRot3,
            // またはpJntAnm->ofsRot5からのものになる
            if (IsDecimalFrame)
            {
                getRotDataEx_(&pResult->rot,
                              Frame,
                              pData,
                              pJntAnm);
            }
            else
            {
                getRotData_(&pResult->rot,
                            Frame,
                            pData,
                            pJntAnm);
            }

            // NNSG3dJntAnmRInfoと配列へのオフセットデータの分
            pData += 2;
        }
        else
        {
            // constのRを取り出すコード
            if (getRotDataByIdx_(&pResult->rot,
                                 (void*)((u8*)pJntAnm + pJntAnm->ofsRot3),
                                 (void*)((u8*)pJntAnm + pJntAnm->ofsRot5),
                                 (NNSG3dJntAnmRIdx)*pData))
            {
                vecCross_((const VecFx32*)&pResult->rot._00,
                          (const VecFx32*)&pResult->rot._10,
                          (VecFx32*)&pResult->rot._20);
            }

            // 定数データの分
            pData += 1;
        }
    }
    else
    {
        if (tag & NNS_G3D_JNTANM_SRTINFO_IDENTITY_R)
        {
            // Rot = Identity
            pResult->flag |= NNS_G3D_JNTANM_RESULTFLAG_ROT_ZERO;

            // 後続データは存在しないのでpDataを進める必要はない
        }
        else
        {
            // モデルのRotを取得
            getMdlRot_(pResult);
        }
    }

    //
    // Scaleの取得
    //
    if (!(tag & (NNS_G3D_JNTANM_SRTINFO_IDENTITY_S | NNS_G3D_JNTANM_SRTINFO_BASE_S)))
    {
        if (!(tag & NNS_G3D_JNTANM_SRTINFO_CONST_SX))
        {
            // SXを取り出す関数呼び出し
            // データのオフセットはpAnmSRTTagからのものになる。
            fx32 sx_invsx[2];
            if (IsDecimalFrame)
            {
                getScaleDataEx_(&sx_invsx[0],
                                Frame,
                                pData,
                                pJntAnm);
            }
            else
            {
                getScaleData_(&sx_invsx[0],
                              Frame,
                              pData,
                              pJntAnm);
            }
            *(pS_invS + 0) = sx_invsx[0];
            *(pS_invS + 3) = sx_invsx[1];
        }
        else
        {
            const fx32* p_fx32 = (const fx32*)pData;

            *(pS_invS + 0) = *(p_fx32 + 0);
            *(pS_invS + 3) = *(p_fx32 + 1);
        }

        // NNSG3dJntAnmSInfoと配列へのオフセットデータの分
        pData += 2;

        if (!(tag & NNS_G3D_JNTANM_SRTINFO_CONST_SY))
        {
            // SYを取り出す関数呼び出し
            // データのオフセットはpAnmSRTTagからのものになる。
            fx32 sy_invsy[2];
            if (IsDecimalFrame)
            {
                getScaleDataEx_(&sy_invsy[0],
                                Frame,
                                pData,
                                pJntAnm);
            }
            else
            {
                getScaleData_(&sy_invsy[0],
                              Frame,
                              pData,
                              pJntAnm);
            }
            *(pS_invS + 1) = sy_invsy[0];
            *(pS_invS + 4) = sy_invsy[1];
        }
        else
        {
            const fx32* p_fx32 = (const fx32*)pData;

            *(pS_invS + 1) = *(p_fx32 + 0);
            *(pS_invS + 4) = *(p_fx32 + 1);

        }

        // NNSG3dJntAnmSInfoと配列へのオフセットデータの分
        pData += 2;

        if (!(tag & NNS_G3D_JNTANM_SRTINFO_CONST_SZ))
        {
            // SZを取り出す関数呼び出し
            // データのオフセットはpAnmSRTTagからのものになる。
            fx32 sz_invsz[2];
            if (IsDecimalFrame)
            {
                getScaleDataEx_(&sz_invsz[0],
                                Frame,
                                pData,
                                pJntAnm);
            }
            else
            {
                getScaleData_(&sz_invsz[0],
                              Frame,
                              pData,
                              pJntAnm);
            }
            *(pS_invS + 2) = sz_invsz[0];
            *(pS_invS + 5) = sz_invsz[1];
        }
        else
        {
            const fx32* p_fx32 = (const fx32*)pData;

            *(pS_invS + 2) = *(p_fx32 + 0);
            *(pS_invS + 5) = *(p_fx32 + 1);
        }

        // NNSG3dJntAnmSInfoと配列へのオフセットデータの分
        pData += 2;
    }
    else
    {
        if (tag & NNS_G3D_JNTANM_SRTINFO_IDENTITY_S)
        {
            // Scale = (1, 1, 1)
            pResult->flag |= NNS_G3D_JNTANM_RESULTFLAG_SCALE_ONE;

            // 後続データは存在しないのでpDataを進める必要はない
        }
        else
        {
            // モデルのScaleを取得
            getMdlScale_(pResult);

            // getMdlScale_内部でNNSG3dJntAnmResultのスケーリング
            // 情報を設定しているのでリターンする
            return;
        }
    }
GET_JOINTSCALE:
    //
    // NNSG3dJntAnmResultのスケーリング情報を設定する
    //
    (*NNS_G3dRS->funcJntScale)(
        pResult, 
        pS_invS,
        NNS_G3dRS->c,
        ((pResult->flag & NNS_G3D_JNTANM_RESULTFLAG_SCALE_ONE) ?
                NNS_G3D_SRTFLAG_SCALE_ONE :
                0) // フラグは変換する必要がある
    );
}


/*---------------------------------------------------------------------------*
    getTransData_

    translationデータを取得し、*pValに入れる
 *---------------------------------------------------------------------------*/
static void
getTransData_(fx32* pVal,
              fx32 Frame,
              const u32* pData,
              const NNSG3dResJntAnm* pJntAnm)
{
    u32 frame = (u32)FX_Whole(Frame);
    const void* pArray = (const void*)((const u8*)pJntAnm + *(pData + 1));
    NNSG3dJntAnmTInfo info = (NNSG3dJntAnmTInfo)*pData;
    u32 last_interp;
    u32 idx;
    u32 idx_sub;

    NNS_G3D_NULL_ASSERT(pVal);
    NNS_G3D_NULL_ASSERT(pArray);

    if (!(info & NNS_G3D_JNTANM_TINFO_STEP_MASK))
    {
        // NNS_G3D_JNTANM_TINFO_STEP1が選択されている
        idx = frame;
        goto TRANS_NONINTERP;
    }

    // last_interp以降は1コマずつデータが入っている
    // last_interpは2の倍数か4の倍数である。
    last_interp = ((u32)info & NNS_G3D_JNTANM_TINFO_LAST_INTERP_MASK) >>
                                    NNS_G3D_JNTANM_TINFO_LAST_INTERP_SHIFT;

    if (info & NNS_G3D_JNTANM_TINFO_STEP_2)
    {
        // NNS_G3D_JNTANM_TINFO_STEP_2が選択されている
        if (frame & 1)
        {
            if (frame > last_interp)
            {
                // 最終フレーム以外ありえない
                idx = (last_interp >> 1) + 1;
                goto TRANS_NONINTERP;
            }
            else
            {
                // 奇数で最終フレームでないので50:50の補間処理がいる。
                idx = frame >> 1;
                goto TRANS_INTERP_2;
            }
        }
        else
        {
            // 偶数フレームなので補間処理はいらない
            idx = frame >> 1;
            goto TRANS_NONINTERP;
        }
    }
    else
    {
        // NNS_G3D_JNTANM_TINFO_STEP_4が選択されている
        if (frame & 3)
        {
            if (frame > last_interp)
            {
                idx = (last_interp >> 2) + (frame & 3);
                goto TRANS_NONINTERP;
            }

            // 補間処理あり
            if (frame & 1)
            {
                fx32 v, v_sub;
                if (frame & 2)
                {
                    // 3:1の位置で補間
                    idx_sub = (frame >> 2);
                    idx = idx_sub + 1;
                }
                else
                {
                    // 1:3の位置で補間
                    idx = (frame >> 2);
                    idx_sub = idx + 1;
                }
                
                // 1:3, 3:1の場合の補間
                if (info & NNS_G3D_JNTANM_TINFO_FX16ARRAY)
                {
                    const fx16* p_fx16 = (const fx16*)pArray;

                    v = *(p_fx16 + idx);
                    v_sub = *(p_fx16 + idx_sub);
                    *pVal = (v + v + v + v_sub) >> 2; // v:v_subを3:1でブレンド
                }
                else
                {
                    const fx32* p_fx32 = (const fx32*)pArray;

                    v = *(p_fx32 + idx);
                    v_sub = *(p_fx32 + idx_sub);
                    // v:v_subを3:1でブレンド。オーバーフローを避けるためfx64で計算
                    *pVal = (fx32)(((fx64)v + v + v + v_sub) >> 2);
                }
                return;
            }
            else
            {
                // 50:50の補間になる
                idx = frame >> 2;
                goto TRANS_INTERP_2;
            }
        }
        else
        {
            // フレームは丁度4の倍数になっている。
            idx = frame >> 2;
            goto TRANS_NONINTERP;
        }
    }
    NNS_G3D_ASSERT(0);
TRANS_INTERP_2:
    if (info & NNS_G3D_JNTANM_TINFO_FX16ARRAY)
    {
        const fx16* p_fx16 = (const fx16*)pArray;

        *pVal = (*(p_fx16 + idx) + *(p_fx16 + idx + 1)) >> 1;
    }
    else
    {
        const fx32* p_fx32 = (const fx32*)pArray;

        fx32 v1 = *(p_fx32 + idx) >> 1;
        fx32 v2 = *(p_fx32 + idx + 1) >> 1;
        *pVal = v1 + v2;
    }
    return;
TRANS_NONINTERP:
    if (info & NNS_G3D_JNTANM_TINFO_FX16ARRAY)
    {
        *pVal = *((const fx16*)pArray + idx);
    }
    else
    {
        *pVal = *((const fx32*)pArray + idx);
    }
    return;
}


/*---------------------------------------------------------------------------*
    getTransDataEx_

    translationデータを取得し、*pValに入れる。
    小数点以下の場合は補間を行う。
 *---------------------------------------------------------------------------*/
static void
getTransDataEx_(fx32* pVal,
                fx32 Frame,
                const u32* pData,
                const NNSG3dResJntAnm* pJntAnm)
{
    // この関数には、
    // 0 < Frame < numFrameとなるようなFrameしか来ないようになっています。
    const void* pArray = (const void*)((const u8*)pJntAnm + *(pData + 1));
    NNSG3dJntAnmTInfo info = (NNSG3dJntAnmTInfo)*pData;

    u32 last_interp;
    u32 idx;
    fx32 remainder;
    int step;
    u32 step_shift;
    u32 frame;

    NNS_G3D_NULL_ASSERT(pVal);
    NNS_G3D_NULL_ASSERT(pArray);

    frame = (u32)FX_Whole(Frame);

    if (frame == pJntAnm->numFrame - 1)
    {
        //
        // FrameがnumFrame - 1とnumFrameの間の場合
        // ちょうど最終フレームから最初のフレームの間にある場合
        //

        //
        // 最初に最終フレームデータのインデックスを取得
        //
        if (!(info & NNS_G3D_JNTANM_TINFO_STEP_MASK))
        {
            idx = frame;
        }
        else if (info & NNS_G3D_JNTANM_TINFO_STEP_2)
        {
            idx = (frame >> 1) + (frame & 1);
        }
        else
        {
            idx = (frame >> 2) + (frame & 3);
        }

        //
        // 補外するか最終のデータを返すかはフラグで決まっている。
        //
        if (pJntAnm->flag & NNS_G3D_JNTANM_OPTION_END_TO_START_INTERPOLATION)
        {
            // 補外する場合は最終データと最初のデータで行う(ループ)
            fx32 v0, v1;
            remainder = Frame & (FX32_ONE - 1);

            if (info & NNS_G3D_JNTANM_TINFO_FX16ARRAY)
            {
                const fx16* p_fx16 = (const fx16*)pArray;

                v0 = *(p_fx16 + idx);
                v1 = *p_fx16;
            }
            else
            {
                const fx32* p_fx32 = (const fx32*)pArray;

                v0 = *(p_fx32 + idx);
                v1 = *p_fx32;
            }

            *pVal = v0 + (((v1 - v0) * remainder) >> FX32_SHIFT);
            return;
        }
        else
        {
            // 最終のデータを返す場合
            if (info & NNS_G3D_JNTANM_TINFO_FX16ARRAY)
            {
                *pVal = *((const fx16*)pArray + idx);
            }
            else
            {
                *pVal = *((const fx32*)pArray + idx);
            }
            return;
        }
    }

    if (!(info & NNS_G3D_JNTANM_TINFO_STEP_MASK))
    {
        // NNS_G3D_JNTANM_TINFO_STEP1が選択されている
        goto TRANS_EX_0;
    }

    // last_interp以降は1コマずつデータが入っている
    // last_interpは2の倍数か4の倍数である。
    last_interp = ((u32)info & NNS_G3D_JNTANM_TINFO_LAST_INTERP_MASK) >>
                                    NNS_G3D_JNTANM_TINFO_LAST_INTERP_SHIFT;
    
    if (info & NNS_G3D_JNTANM_TINFO_STEP_2)
    {
        // NNS_G3D_JNTANM_TINFO_STEP_2が選択されている
        // 等号が含まれるのは切り捨てられたデータを考慮するから
        if (frame >= last_interp)
        {
            // 最終フレーム以外ありえない
            idx = (last_interp >> 1);
            goto TRANS_EX_0_1;
        }
        else
        {
            idx = frame >> 1;
            remainder = Frame & (FX32_ONE * 2 - 1);
            step = 2;
            step_shift = 1;
            goto TRANS_EX;
        }
    }
    else
    {
        // NNS_G3D_JNTANM_TINFO_STEP_4が選択されている
        if (frame >= last_interp)
        {
            // この関数が呼び出される条件により、frameが最終データであることはない
            // ので以下のコードで境界をはみ出ることはない。
            idx = (frame >> 2) + (frame & 3);
            goto TRANS_EX_0_1;
        }
        else
        {
            idx = frame >> 2;
            remainder = Frame & (FX32_ONE * 4 - 1);
            step = 4;
            step_shift = 2;
            goto TRANS_EX;
        }
    }
    NNS_G3D_ASSERT(0);
TRANS_EX_0:
    idx = (u32)frame;
TRANS_EX_0_1:
    remainder = Frame & (FX32_ONE - 1);
    step = 1;
    step_shift = 0;
TRANS_EX:
    {
        fx32 v0, v1;
        if (info & NNS_G3D_JNTANM_TINFO_FX16ARRAY)
        {
            const fx16* p_fx16 = (const fx16*)pArray;

            v0 = *(p_fx16 + idx);
            v1 = *(p_fx16 + idx + 1);
        }
        else
        {
            const fx32* p_fx32 = (const fx32*)pArray;

            v0 = *(p_fx32 + idx);
            v1 = *(p_fx32 + idx + 1);
        }

        *pVal = ((v0 * step) + (((v1 - v0) * remainder) >> FX32_SHIFT)) >> step_shift;
    }
    return;
}


/*---------------------------------------------------------------------------*
    getScaleData_

    scaleとその逆数を取得し、s_invs[0], s_invs[1]に入れる
 *---------------------------------------------------------------------------*/
static void
getScaleData_(fx32* s_invs,
              fx32 Frame,
              const u32* pData,
              const NNSG3dResJntAnm* pJntAnm)
{
    u32 frame = (u32)FX_Whole(Frame);
    const void* pArray = (const void*)((u8*)pJntAnm + *(pData + 1));
    NNSG3dJntAnmSInfo info = (NNSG3dJntAnmSInfo)*pData;
    u32 last_interp;
    u32 idx;
    u32 idx_sub;

    NNS_G3D_NULL_ASSERT(s_invs);
    NNS_G3D_NULL_ASSERT(pArray);

    if (!(info & NNS_G3D_JNTANM_SINFO_STEP_MASK))
    {
        // NNS_G3D_JNTANM_SINFO_STEP_1が選択されている
        idx = frame;
        goto SCALE_NONINTERP;
    }

    // last_interp以降は1コマずつデータが入っている
    // last_interpは2の倍数か4の倍数である。
    last_interp = ((u32)info & NNS_G3D_JNTANM_SINFO_LAST_INTERP_MASK) >>
                                    NNS_G3D_JNTANM_SINFO_LAST_INTERP_SHIFT;

    if (info & NNS_G3D_JNTANM_SINFO_STEP_2)
    {
        // NNS_G3D_JNTANM_SINFO_STEP_2が選択されている
        if (frame & 1)
        {
            if (frame > last_interp)
            {
                // 最終フレーム以外ありえない
                idx = (last_interp >> 1) + 1;
                goto SCALE_NONINTERP;
            }
            else
            {
                // 奇数で最終フレームでないので50:50の補間処理がいる。
                idx = frame >> 1;
                goto SCALE_INTERP_2;
            }
        }
        else
        {
            // 偶数フレームなので補間処理はいらない
            idx = frame >> 1;
            goto SCALE_NONINTERP;
        }
    }
    else
    {
        // NNS_G3D_JNTANM_TINFO_STEP_4が選択されている
        if (frame & 3)
        {
            if (frame > last_interp)
            {
                // last_interp以降は補間処理なし
                idx = (last_interp >> 2) + (frame & 3);
                goto SCALE_NONINTERP;
            }

            // 補間処理あり
            if (frame & 1)
            {
                fx32 v, v_sub;
                if (frame & 2)
                {
                    // 3:1の位置で補間
                    idx_sub = (frame >> 2);
                    idx = idx_sub + 1;
                }
                else
                {
                    // 1:3の位置で補間
                    idx = (frame >> 2);
                    idx_sub = idx + 1;
                }
                
                if (info & NNS_G3D_JNTANM_SINFO_FX16ARRAY)
                {
                    const fx16* p_fx16 = (const fx16*)pArray;

                    // scaleの計算
                    v = *(p_fx16 + 2 * idx);
                    v_sub = *(p_fx16 + 2 * idx_sub);
                    s_invs[0] = (v + (v << 1) + v_sub) >> 2; // v:v_subを3:1でブレンド

                    // inverse scaleの計算
                    v = *(p_fx16 + 2 * idx + 1);
                    v_sub = *(p_fx16 + 2 * idx_sub + 1);
                    s_invs[1] = (v + (v << 1) + v_sub) >> 2; // v:v_subを3:1でブレンド
                }
                else
                {
                    const fx32* p_fx32 = (const fx32*)pArray;

                    // scaleの計算
                    v = *(p_fx32 + 2 * idx);
                    v_sub = *(p_fx32 + 2 * idx_sub);
                    // v:v_subを3:1でブレンド。オーバーフローを避けるためfx64で計算
                    s_invs[0] = (fx32)(((fx64)v + v + v + v_sub) >> 2);

                    // inverse scaleの計算
                    v = *(p_fx32 + 2 * idx + 1);
                    v_sub = *(p_fx32 + 2 * idx_sub + 1);
                    // v:v_subを3:1でブレンド。オーバーフローを避けるためfx64で計算
                    s_invs[1] = (fx32)(((fx64)v + v + v + v_sub) >> 2);
                }
                return;
            }
            else
            {
                // 50:50の補間になる
                idx = frame >> 2;
                goto SCALE_INTERP_2;
            }
        }
        else
        {
            // フレームは丁度4の倍数になっている。
            idx = frame >> 2;
            goto SCALE_NONINTERP;
        }
    }
SCALE_NONINTERP:
    if (info & NNS_G3D_JNTANM_SINFO_FX16ARRAY)
    {
        const fx16* p_fx16 = (const fx16*)pArray;

        s_invs[0] = *(p_fx16 + 2 * idx);
        s_invs[1] = *(p_fx16 + 2 * idx + 1);
    }
    else
    {
        const fx32* p_fx32 = (const fx32*)pArray;

        s_invs[0] = *(p_fx32 + 2 * idx);
        s_invs[1] = *(p_fx32 + 2 * idx + 1);
    }
    return;
SCALE_INTERP_2:
    if (info & NNS_G3D_JNTANM_SINFO_FX16ARRAY)
    {
        const fx16* p_fx16 = (const fx16*)pArray;

        s_invs[0] = (*(p_fx16 + 2 * idx) + *(p_fx16 + 2 * idx + 2)) >> 1;
        s_invs[1] = (*(p_fx16 + 2 * idx + 1) + *(p_fx16 + 2 * idx + 3)) >> 1;
    }
    else
    {
        const fx32* p_fx32 = (const fx32*)pArray;

        s_invs[0] = (*(p_fx32 + 2 * idx) + (*(p_fx32 + 2 * idx + 2))) >> 1;
        s_invs[1] = (*(p_fx32 + 2 * idx + 1) + (*(p_fx32 + 2 * idx + 3))) >> 1;
    }
    return;
}


/*---------------------------------------------------------------------------*
    getScaleDataEx_

    scaleとその逆数を取得し、s_invs[0], s_invs[1]に入れる
    小数点以下の場合は補間を行う。
 *---------------------------------------------------------------------------*/
static void
getScaleDataEx_(fx32* s_invs,
                fx32 Frame,
                const u32* pData,
                const NNSG3dResJntAnm* pJntAnm)
{
    const void* pArray = (const void*)((const u8*)pJntAnm + *(pData + 1));
    NNSG3dJntAnmSInfo info = (NNSG3dJntAnmSInfo)*pData;
    u32 last_interp;
    u32 idx0, idx1;
    fx32 remainder;
    int step;
    u32 step_shift;
    u32 frame;

    frame = (u32)FX_Whole(Frame);

    if (frame == pJntAnm->numFrame - 1)
    {
        //
        // FrameがnumFrame - 1とnumFrameの間の場合
        // ちょうど最終フレームから最初のフレームの間にある場合
        //

        //
        // 最初に最終フレームデータのインデックスを取得
        //
        if (!(info & NNS_G3D_JNTANM_SINFO_STEP_MASK))
        {
            idx0 = frame;
        }
        else if (info & NNS_G3D_JNTANM_SINFO_STEP_2)
        {
            idx0 = (frame >> 1) + (frame & 1);
        }
        else
        {
            idx0 = (frame >> 2) + (frame & 3);
        }

        //
        // 補外するか最終のデータを返すかはフラグで決まっている。
        //
        if (pJntAnm->flag & NNS_G3D_JNTANM_OPTION_END_TO_START_INTERPOLATION)
        {
            idx1 = 0;
            goto SCALE_EX_0_1;
        }
        else
        {
            if (info & NNS_G3D_JNTANM_SINFO_FX16ARRAY)
            {
                const fx16* p_fx16 = (const fx16*)pArray;

                s_invs[0] = *(p_fx16 + 2 * idx0);
                s_invs[1] = *(p_fx16 + 2 * idx0 + 1);
            }
            else
            {
                const fx32* p_fx32 = (const fx32*)pArray;

                s_invs[0] = *(p_fx32 + 2 * idx0);
                s_invs[1] = *(p_fx32 + 2 * idx0 + 1);
            }
            return;
        }
    }

    if (!(info & NNS_G3D_JNTANM_SINFO_STEP_MASK))
    {
        // NNS_G3D_JNTANM_SINFO_STEP_1が選択されている
        goto SCALE_EX_0;
    }

    last_interp = ((u32)info & NNS_G3D_JNTANM_SINFO_LAST_INTERP_MASK) >>
                                    NNS_G3D_JNTANM_SINFO_LAST_INTERP_SHIFT;

    if (info & NNS_G3D_JNTANM_SINFO_STEP_2)
    {
        // NNS_G3D_JNTANM_SINFO_STEP_2が選択されている
        // 等号が含まれるのは切り捨てられたデータを考慮するから
        if (frame >= last_interp)
        {
            // 最終フレーム以外ありえない
            idx0 = (last_interp >> 1);
            idx1 = idx0 + 1;
            goto SCALE_EX_0_1;
        }
        else
        {
            idx0 = frame >> 1;
            idx1 = idx0 + 1;
            remainder = Frame & (FX32_ONE * 2 - 1);
            step = 2;
            step_shift = 1;
            goto SCALE_EX;
        }
    }
    else
    {
        // NNS_G3D_JNTANM_SINFO_STEP_4が選択されている
        if (frame >= last_interp)
        {
            // この関数が呼び出される条件により、frameが最終データであることはない
            // ので以下のコードで境界をはみ出ることはない。
            idx0 = (frame >> 2) + (frame & 3);
            idx1 = idx0 + 1;
            goto SCALE_EX_0_1;
        }
        else
        {
            idx0 = frame >> 2;
            idx1 = idx0 + 1;
            remainder = Frame & (FX32_ONE * 4 - 1);
            step = 4;
            step_shift = 2;
            goto SCALE_EX;
        }
    }
    NNS_G3D_ASSERT(0);
SCALE_EX_0:
    idx0 = (u32)frame;
    idx1 = idx0 + 1;
SCALE_EX_0_1:
    remainder = Frame & (FX32_ONE - 1);
    step = 1;
    step_shift = 0;
SCALE_EX:
    {
        fx32 s0, s1;
        fx32 inv0, inv1;

        if (info & NNS_G3D_JNTANM_SINFO_FX16ARRAY)
        {
            const fx16* p_fx16 = (const fx16*)pArray;

            s0   = *(p_fx16 + 2 * idx0);
            inv0 = *(p_fx16 + 2 * idx0 + 1);

            s1   = *(p_fx16 + 2 * idx1);
            inv1 = *(p_fx16 + 2 * idx1 + 1);
        }
        else
        {
            const fx32* p_fx32 = (const fx32*)pArray;

            s0   = *(p_fx32 + 2 * idx0);
            inv0 = *(p_fx32 + 2 * idx0 + 1);

            s1   = *(p_fx32 + 2 * idx1);
            inv1 = *(p_fx32 + 2 * idx1 + 1);
        }

        s_invs[0] = ((s0 * step) + (((s1 - s0) * remainder) >> FX32_SHIFT)) >> step_shift;
        s_invs[1] = ((inv0 * step) + (((inv1 - inv0) * remainder) >> FX32_SHIFT)) >> step_shift;
    }

    return;
}



// フレーム間の補間処理が必要となる場合に、G3D_NORMALIZE_ROT_MTXが 
// 無効になっていると単なる行列の線形補間しか行なわないため、キーフレーム間の
// 回転角が大きい場合にはモデルの形状が歪む場合があります。
// G3D_NORMALIZE_ROT_MTXが有効である場合には、正規化処理を行なうため
// モデルの歪みを抑えることができます。
#define G3D_NORMALIZE_ROT_MTX

#ifdef G3D_NORMALIZE_ROT_MTX
#define ROT_FILTER_SHIFT    0
#else
#define ROT_FILTER_SHIFT    1
#endif

/*---------------------------------------------------------------------------*
    getRotData_

    回転行列を取得し、*pRotにセットする
 *---------------------------------------------------------------------------*/
static void
getRotData_(MtxFx33* pRot,
            fx32 Frame,
            const u32* pData,
            const NNSG3dResJntAnm* pJntAnm)
{
    u32 frame = (u32)FX_Whole(Frame);
    const void* pArray = (const void*)((const u8*)pJntAnm + *(pData + 1));
    const void* pArrayRot3 = (const void*)((const u8*)pJntAnm + pJntAnm->ofsRot3);
    const void* pArrayRot5 = (const void*)((const u8*)pJntAnm + pJntAnm->ofsRot5);
    NNSG3dJntAnmRInfo info = (NNSG3dJntAnmRInfo)*pData;
    const u16* p = (const u16*)pArray;

    u32 last_interp;
    u32 idx;
    u32 idx_sub;

    if (!(info & NNS_G3D_JNTANM_RINFO_STEP_MASK))
    {
        // NNS_G3D_JNTANM_RINFO_STEP_1が選択されている
        idx = frame;
        goto ROT_NONINTERP;
    }

    last_interp = ((u32)info & NNS_G3D_JNTANM_RINFO_LAST_INTERP_MASK) >>
                                    NNS_G3D_JNTANM_RINFO_LAST_INTERP_SHIFT;

    if (info & NNS_G3D_JNTANM_RINFO_STEP_2)
    {
        // NNS_G3D_JNTANM_RINFO_STEP_2が選択されている
        if (frame & 1)
        {
            if (frame > last_interp)
            {
                // 最終フレーム以外ありえない
                idx = (last_interp >> 1) + 1;
                goto ROT_NONINTERP;
            }
            else
            {
                // 奇数で最終フレームでないので50:50の補間処理がいる。
                idx = frame >> 1;
                goto ROT_INTERP_2;
            }
        }
        else
        {
            // 偶数フレームなので補間処理はいらない
            idx = frame >> 1;
            goto ROT_NONINTERP;
        }
    }
    else
    {
        // NNS_G3D_JNTANM_RINFO_STEP_4が選択されている
        if (frame & 3)
        {
            if (frame > last_interp)
            {
                idx = (last_interp >> 2) + (frame & 3);
                goto ROT_NONINTERP;
            }

            // 補間処理あり
            if (frame & 1)
            {
                MtxFx33 tmp;
                BOOL    doCross = FALSE;
                if (frame & 2)
                {
                    // 3:1の位置で補間
                    idx_sub = (frame >> 2);
                    idx = idx_sub + 1;
                }
                else
                {
                    // 1:3の位置で補間
                    idx = (frame >> 2);
                    idx_sub = idx + 1;
                }
                doCross |= getRotDataByIdx_(pRot,
                                            pArrayRot3,
                                            pArrayRot5,
                                            (NNSG3dJntAnmRIdx)p[idx]);
                doCross |= getRotDataByIdx_(&tmp,
                                            pArrayRot3,
                                            pArrayRot5,
                                            (NNSG3dJntAnmRIdx)p[idx_sub]);
                pRot->_00 = (pRot->_00 * 3 + tmp._00) >> (2 * ROT_FILTER_SHIFT);
                pRot->_01 = (pRot->_01 * 3 + tmp._01) >> (2 * ROT_FILTER_SHIFT);
                pRot->_02 = (pRot->_02 * 3 + tmp._02) >> (2 * ROT_FILTER_SHIFT);
                pRot->_10 = (pRot->_10 * 3 + tmp._10) >> (2 * ROT_FILTER_SHIFT);
                pRot->_11 = (pRot->_11 * 3 + tmp._11) >> (2 * ROT_FILTER_SHIFT);
                pRot->_12 = (pRot->_12 * 3 + tmp._12) >> (2 * ROT_FILTER_SHIFT);
                
#ifdef G3D_NORMALIZE_ROT_MTX
                VEC_Normalize( (VecFx32*)(&pRot->_00), (VecFx32*)(&pRot->_00) );
                VEC_Normalize( (VecFx32*)(&pRot->_10), (VecFx32*)(&pRot->_10) );
#endif
                
                if (!doCross)
                {
                    // 両方ともRot3形式のデータだった場合は
                    // 外積の計算を避けることにする
                    pRot->_20 = (pRot->_20 * 3 + tmp._20) >> (2 * ROT_FILTER_SHIFT);
                    pRot->_21 = (pRot->_21 * 3 + tmp._21) >> (2 * ROT_FILTER_SHIFT);
                    pRot->_22 = (pRot->_22 * 3 + tmp._22) >> (2 * ROT_FILTER_SHIFT);

#ifdef G3D_NORMALIZE_ROT_MTX
                    VEC_Normalize( (VecFx32*)(&pRot->_20), (VecFx32*)(&pRot->_20) );
#endif
                }
                else
                {
                    vecCross_((const VecFx32*)&pRot->_00,
                              (const VecFx32*)&pRot->_10,
                              (VecFx32*)&pRot->_20);
                }
                return;
            }
            else
            {
                // 50:50の補間になる
                idx = frame >> 2;
                goto ROT_INTERP_2;
            }
        }
        else
        {
            idx = frame >> 2;
            goto ROT_NONINTERP;
        }
    }
ROT_INTERP_2:
    {
        MtxFx33 tmp;
        BOOL    doCross = FALSE;
        doCross |= getRotDataByIdx_(pRot,
                                    pArrayRot3,
                                    pArrayRot5,
                                    (NNSG3dJntAnmRIdx)p[idx]);
        doCross |= getRotDataByIdx_(&tmp,
                                    pArrayRot3,
                                    pArrayRot5,
                                    (NNSG3dJntAnmRIdx)p[idx + 1]);
        pRot->_00 = (pRot->_00 + tmp._00) >> ROT_FILTER_SHIFT;
        pRot->_01 = (pRot->_01 + tmp._01) >> ROT_FILTER_SHIFT;
        pRot->_02 = (pRot->_02 + tmp._02) >> ROT_FILTER_SHIFT;
        pRot->_10 = (pRot->_10 + tmp._10) >> ROT_FILTER_SHIFT;
        pRot->_11 = (pRot->_11 + tmp._11) >> ROT_FILTER_SHIFT;
        pRot->_12 = (pRot->_12 + tmp._12) >> ROT_FILTER_SHIFT;
        
#ifdef G3D_NORMALIZE_ROT_MTX
        VEC_Normalize( (VecFx32*)(&pRot->_00), (VecFx32*)(&pRot->_00) );
        VEC_Normalize( (VecFx32*)(&pRot->_10), (VecFx32*)(&pRot->_10) );
#endif
        
        if (!doCross)
        {
            pRot->_20 = (pRot->_20 + tmp._20) >> ROT_FILTER_SHIFT;
            pRot->_21 = (pRot->_21 + tmp._21) >> ROT_FILTER_SHIFT;
            pRot->_22 = (pRot->_22 + tmp._22) >> ROT_FILTER_SHIFT;
            
#ifdef G3D_NORMALIZE_ROT_MTX
            VEC_Normalize( (VecFx32*)(&pRot->_20), (VecFx32*)(&pRot->_20) );
#endif
        }
        else
        {
            vecCross_((const VecFx32*)&pRot->_00,
                      (const VecFx32*)&pRot->_10,
                      (VecFx32*)&pRot->_20);
        }
        return;
    }
ROT_NONINTERP:
    if (getRotDataByIdx_(pRot,
                         pArrayRot3,
                         pArrayRot5,
                         (NNSG3dJntAnmRIdx)p[idx]))
    {
        vecCross_((const VecFx32*)&pRot->_00,
                  (const VecFx32*)&pRot->_10,
                  (VecFx32*)&pRot->_20);
    }
    else
    {
#ifdef G3D_NORMALIZE_ROT_MTX
        VEC_Normalize( (VecFx32*)(&pRot->_20), (VecFx32*)(&pRot->_20) );
#endif
    }
    return;
}


/*---------------------------------------------------------------------------*
    getRotData_

    回転行列を取得し、*pRotにセットする。
    小数点以下の場合は補間を行う。
 *---------------------------------------------------------------------------*/
static void
getRotDataEx_(MtxFx33* pRot,
              fx32 Frame,
              const u32* pData,
              const NNSG3dResJntAnm* pJntAnm)
{
    // この関数には、
    // 0 < Frame < numFrameとなるようなFrameしか来ないようになっています。

    const void* pArray = (const void*)((const u8*)pJntAnm + *(pData + 1));
    const void* pArrayRot3 = (const void*)((const u8*)pJntAnm + pJntAnm->ofsRot3);
    const void* pArrayRot5 = (const void*)((const u8*)pJntAnm + pJntAnm->ofsRot5);
    NNSG3dJntAnmRInfo info = (NNSG3dJntAnmRInfo)*pData;

    u32 last_interp;
    u32 idx0, idx1;
    fx32 remainder;
    int step;
    u32 step_shift;
    u32 frame;
    const u16* p = (const u16*)pArray;

    frame = (u32)FX_Whole(Frame);

    if (frame == pJntAnm->numFrame - 1)
    {
        //
        // FrameがnumFrame - 1とnumFrameの間の場合
        // ちょうど最終フレームから最初のフレームの間にある場合
        //

        //
        // 最初に最終フレームデータのインデックスを取得
        //
        if (!(info & NNS_G3D_JNTANM_RINFO_STEP_MASK))
        {
            idx0 = frame;
        }
        else if (info & NNS_G3D_JNTANM_RINFO_STEP_2)
        {
            idx0 = (frame >> 1) + (frame & 1);
        }
        else
        {
            idx0 = (frame >> 2) + (frame & 3);
        }

        //
        // 補外するか最終のデータを返すかはフラグで決まっている。
        //
        if (pJntAnm->flag & NNS_G3D_JNTANM_OPTION_END_TO_START_INTERPOLATION)
        {
            // 補外する場合は最終データと最初のデータで行う(ループ)
            idx1 = 0;
            goto ROT_EX_0_1;
        }
        else
        {
            // 最終のデータを返す場合
            if (getRotDataByIdx_(pRot,
                                 pArrayRot3,
                                 pArrayRot5,
                                 (NNSG3dJntAnmRIdx)p[idx0]))
            {
                vecCross_((const VecFx32*)&pRot->_00,
                          (const VecFx32*)&pRot->_10,
                          (VecFx32*)&pRot->_20);
            }
            else
            {
#ifdef G3D_NORMALIZE_ROT_MTX
                VEC_Normalize( (VecFx32*)(&pRot->_20), (VecFx32*)(&pRot->_20) );
#endif
            }
            return;
        }
    }

    if (!(info & NNS_G3D_JNTANM_RINFO_STEP_MASK))
    {
        // NNS_G3D_JNTANM_RINFO_STEP_1が選択されている
        goto ROT_EX_0;
    }

    last_interp = ((u32)info & NNS_G3D_JNTANM_RINFO_LAST_INTERP_MASK) >>
                                    NNS_G3D_JNTANM_RINFO_LAST_INTERP_SHIFT;

    if (info & NNS_G3D_JNTANM_RINFO_STEP_2)
    {
        // NNS_G3D_JNTANM_RINFO_STEP_2が選択されている
        // 等号が含まれるのは切り捨てられたデータを考慮するから
        if (frame >= last_interp)
        {
            // 最終フレーム以外ありえない
            idx0 = (last_interp >> 1);
            idx1 = idx0 + 1;
            goto ROT_EX_0_1;
        }
        else
        {
            idx0 = frame >> 1;
            idx1 = idx0 + 1;
            remainder = Frame & (FX32_ONE * 2 - 1);
            step = 2;
            step_shift = 1;
            goto ROT_EX;
        }
    }
    else
    {
        // NNS_G3D_JNTANM_RINFO_STEP_4が選択されている
        if (frame >= last_interp)
        {
            // この関数が呼び出される条件により、frameが最終データであることはない
            // ので以下のコードで境界をはみ出ることはない。
            idx0 = (frame >> 2) + (frame & 3);
            idx1 = idx0 + 1;
            goto ROT_EX_0_1;
        }
        else
        {
            idx0 = frame >> 2;
            idx1 = idx0 + 1;
            remainder = Frame & (FX32_ONE * 4 - 1);
            step = 4;
            step_shift = 2;
            goto ROT_EX;
        }
    }
    NNS_G3D_ASSERT(0);
ROT_EX_0:
    idx0 = (u32)frame;
    idx1 = idx0 + 1;
ROT_EX_0_1:
    remainder = Frame & (FX32_ONE - 1);
    step = 1;
    step_shift = 0;
ROT_EX:
    {
        MtxFx33 r0, r1;
        BOOL doCross = FALSE;
        doCross |= getRotDataByIdx_(&r0,
                                    pArrayRot3,
                                    pArrayRot5,
                                    (NNSG3dJntAnmRIdx)p[idx0]);
        doCross |= getRotDataByIdx_(&r1,
                                    pArrayRot3,
                                    pArrayRot5,
                                    (NNSG3dJntAnmRIdx)p[idx1]);

        pRot->_00 = ((r0._00 * step) + (((r1._00 - r0._00) * remainder) >> FX32_SHIFT)) >> (step_shift * ROT_FILTER_SHIFT);
        pRot->_01 = ((r0._01 * step) + (((r1._01 - r0._01) * remainder) >> FX32_SHIFT)) >> (step_shift * ROT_FILTER_SHIFT);
        pRot->_02 = ((r0._02 * step) + (((r1._02 - r0._02) * remainder) >> FX32_SHIFT)) >> (step_shift * ROT_FILTER_SHIFT);
        pRot->_10 = ((r0._10 * step) + (((r1._10 - r0._10) * remainder) >> FX32_SHIFT)) >> (step_shift * ROT_FILTER_SHIFT);
        pRot->_11 = ((r0._11 * step) + (((r1._11 - r0._11) * remainder) >> FX32_SHIFT)) >> (step_shift * ROT_FILTER_SHIFT);
        pRot->_12 = ((r0._12 * step) + (((r1._12 - r0._12) * remainder) >> FX32_SHIFT)) >> (step_shift * ROT_FILTER_SHIFT);

#ifdef G3D_NORMALIZE_ROT_MTX
        VEC_Normalize( (VecFx32*)(&pRot->_00), (VecFx32*)(&pRot->_00) );
        VEC_Normalize( (VecFx32*)(&pRot->_10), (VecFx32*)(&pRot->_10) );
#endif

        if (!doCross)
        {
            // 両方ともRot3形式のデータだった場合は
            // 外積の計算を避けることにする
            pRot->_20 = ((r0._20 * step) + (((r1._20 - r0._20) * remainder) >> FX32_SHIFT)) >> (step_shift * ROT_FILTER_SHIFT);
            pRot->_21 = ((r0._21 * step) + (((r1._21 - r0._21) * remainder) >> FX32_SHIFT)) >> (step_shift * ROT_FILTER_SHIFT);
            pRot->_22 = ((r0._22 * step) + (((r1._22 - r0._22) * remainder) >> FX32_SHIFT)) >> (step_shift * ROT_FILTER_SHIFT);

#ifdef G3D_NORMALIZE_ROT_MTX
            VEC_Normalize( (VecFx32*)(&pRot->_20), (VecFx32*)(&pRot->_20) );
#endif
        }
        else
        {
            vecCross_((const VecFx32*)&pRot->_00,
                      (const VecFx32*)&pRot->_10,
                      (VecFx32*)&pRot->_20);
        }
        return;
    }
}


/*---------------------------------------------------------------------------*
    getRotDataByIdx_

    インデックスからRot3,Rot5いずれかのデータを取り出し、pRotに格納する。
 *---------------------------------------------------------------------------*/
static BOOL
getRotDataByIdx_(MtxFx33* pRot,
                 const void* pArrayRot3,
                 const void* pArrayRot5,
                 NNSG3dJntAnmRIdx info)
{
    if (info & NNS_G3D_JNTANM_RIDX_PIVOT)
    {
        const fx16* data;
        fx32 A, B;
        u32 idxPivot;

        pRot->_00 = pRot->_01 = pRot->_02 =
        pRot->_10 = pRot->_11 = pRot->_12 =
        pRot->_20 = pRot->_21 = pRot->_22 = 0;

        // data[0] NNSG3dJntAnmPivotInfoのフラグデータ
        // data[1], data[2] 行列の非0要素
        data = (const fx16*)pArrayRot3 + ((info & NNS_G3D_JNTANM_RIDX_IDXDATA_MASK) * 3);
        A = *(data + 1);
        B = *(data + 2);
        idxPivot = (u32)( (*data & NNS_G3D_JNTANM_PIVOTINFO_IDXPIVOT_MASK) >>
                                 NNS_G3D_JNTANM_PIVOT_INFO_IDXPIVOT_SHIFT );

        pRot->a[idxPivot] =
            (*data & NNS_G3D_JNTANM_PIVOTINFO_MINUS) ? -FX32_ONE : FX32_ONE;

        pRot->a[pivotUtil_[idxPivot][0]] = A;
        pRot->a[pivotUtil_[idxPivot][1]] = B;

        pRot->a[pivotUtil_[idxPivot][2]] =
            (*data & NNS_G3D_JNTANM_PIVOTINFO_SIGN_REVC) ? -B : B;
        pRot->a[pivotUtil_[idxPivot][3]] =
            (*data & NNS_G3D_JNTANM_PIVOTINFO_SIGN_REVD) ? -A : A;

        // 3行目まで埋まってる
        return FALSE;
    }
    else
    {
        // Rot5からデータを引いてくる
        const fx16* data = (fx16*)pArrayRot5 +
                           ((info & NNS_G3D_JNTANM_RIDX_IDXDATA_MASK) * 5);
        fx16 _12;

        _12 = (fx16)(data[4] & 7);
        pRot->_11 = (data[4] >> 3);

        _12 = (fx16)((_12 << 3) | (data[0] & 7));
        pRot->_00 = (data[0] >> 3);

        _12 = (fx16)((_12 << 3) | (data[1] & 7));
        pRot->_01 = (data[1] >> 3);

        _12 = (fx16)((_12 << 3) | (data[2] & 7));
        pRot->_02 = (data[2] >> 3);

        _12 = (fx16)((_12 << 3) | (data[3] & 7));
        pRot->_10 = (data[3] >> 3);

        // 符号拡張されるようにシフトした値をfx16にキャストしてから
        // fx32に暗黙キャストする
        pRot->_12 = ((fx16)(_12 << 3) >> 3);

        // 3行目は外積で求めなければならない
        return TRUE;
    }
}

#endif // NNS_G3D_NSBCA_DISABLE
