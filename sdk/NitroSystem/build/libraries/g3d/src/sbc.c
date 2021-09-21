/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g3d
  File:     sbc.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.55 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#include <nnsys/g3d/sbc.h>

#include <nnsys/g3d/kernel.h>
#include <nnsys/g3d/binres/res_struct.h>
#include <nnsys/g3d/glbstate.h>
#include <nnsys/g3d/gecom.h>
#include <nnsys/g3d/binres/res_struct_accessor.h>
#include <nnsys/g3d/util.h>


static void
G3dDrawInternal_Loop_(NNSG3dRS* pRS)
{
    //
    // Loop for SBC execution
    //
    do
    {
        pRS->flag &= ~NNS_G3D_RSFLAG_SKIP;
        NNS_G3D_ASSERTMSG(NNS_G3dFuncSbcTable[NNS_G3D_GET_SBCCMD(*pRS->c)],
                          "SBC command not found / disabled.");

        (*NNS_G3dFuncSbcTable[NNS_G3D_GET_SBCCMD(*pRS->c)])
                                (pRS, NNS_G3D_GET_SBCFLG(*pRS->c));
    }
    while(!(pRS->flag & NNS_G3D_RSFLAG_RETURN));
}


static void
G3dDrawInternal_(NNSG3dRS* pRS, NNSG3dRenderObj* pRenderObj)
{
    NNS_G3D_NULL_ASSERT(pRS);
    NNS_G3D_NULL_ASSERT(pRenderObj);
  
    //
    // Init
    //
    // NOTICE:
    // isScaleCacheOneの最初のビットは立てておくこと
    // そうしないと、Si3dのclassic scale offの場合におかしくなる
    // (根ノードの親のスケールを1として計算させるためにビットを立てる)
    //
    MI_CpuClearFast(pRS, sizeof(*pRS));
    pRS->isScaleCacheOne[0] = 1; // エンディアン依存

    //
    // NNS_G3D_RSFLAG_NODE_VISIBLEはセットしておくのが無難
    // ユーザー定義のSBCでいきなりMATコマンドが実行されるような
    // 場合に備える。
    //
    pRS->flag = NNS_G3D_RSFLAG_NODE_VISIBLE;

    //
    // 解釈するSBCの選択
    // pRenderObj->ptrUserSbcにポインタを設定することで
    // 解釈するSBCを変更することができる。
    //
    if (pRenderObj->ptrUserSbc)
    {
        pRS->c = pRenderObj->ptrUserSbc;
    }
    else
    {
        pRS->c = (u8*)pRenderObj->resMdl + pRenderObj->resMdl->ofsSbc;
    }

    //
    // pRenderObjの設定
    //
    pRS->pRenderObj   = pRenderObj;

    //
    // pRenderObj->resMdlの中で繰り返し使う情報をキャッシュしておく
    //
    pRS->pResNodeInfo = NNS_G3dGetNodeInfo(pRenderObj->resMdl);
    pRS->pResMat      = NNS_G3dGetMat(pRenderObj->resMdl);
    pRS->pResShp      = NNS_G3dGetShp(pRenderObj->resMdl);
    pRS->funcJntScale = NNS_G3dGetJointScale_FuncArray[pRenderObj->resMdl->info.scalingRule];
    pRS->funcJntMtx   = NNS_G3dSendJointSRT_FuncArray[pRenderObj->resMdl->info.scalingRule];
    pRS->funcTexMtx   = NNS_G3dSendTexSRT_FuncArray[pRenderObj->resMdl->info.texMtxMode];
    pRS->posScale     = pRenderObj->resMdl->info.posScale;
    pRS->invPosScale  = pRenderObj->resMdl->info.invPosScale;

    //
    // コールバックの設定
    //
    if (pRenderObj->cbFunc && pRenderObj->cbCmd < NNS_G3D_SBC_COMMAND_NUM)
    {
        NNS_G3D_SBC_CALLBACK_TIMING_ASSERT(pRenderObj->cbTiming);
        pRS->cbVecFunc[pRenderObj->cbCmd] = pRenderObj->cbFunc;
        pRS->cbVecTiming[pRenderObj->cbCmd] = pRenderObj->cbTiming;
    }

    if (pRenderObj->flag & NNS_G3D_RENDEROBJ_FLAG_RECORD)
    {
        pRS->flag |= NNS_G3D_RSFLAG_OPT_RECORD;
    }

    if (pRenderObj->flag & NNS_G3D_RENDEROBJ_FLAG_NOGECMD)
    {
        pRS->flag |= NNS_G3D_RSFLAG_OPT_NOGECMD;
    }

    if (pRenderObj->flag & NNS_G3D_RENDEROBJ_FLAG_SKIP_SBC_DRAW)
    {
        pRS->flag |= NNS_G3D_RSFLAG_OPT_SKIP_SBCDRAW;
    }

    if (pRenderObj->flag & NNS_G3D_RENDEROBJ_FLAG_SKIP_SBC_MTXCALC)
    {
        pRS->flag |= NNS_G3D_RSFLAG_OPT_SKIP_SBCMTXCALC;
    }

    if (pRenderObj->cbInitFunc)
    {
        (*pRenderObj->cbInitFunc)(pRS);
    }

    G3dDrawInternal_Loop_(pRS);

    //
    // クリーンアップ
    //
    // Asserts for notifying 'DO NOT MODIFY THEM'
    NNS_G3D_ASSERT(pRS->pRenderObj == pRenderObj);
    NNS_G3D_ASSERT(pRS == NNS_G3dRS);

    // 使いやすくするため、レコードした場合、自動的にレコードフラグをリセットする。
    // 次に呼ぶときは再生モードになっている。
    pRenderObj->flag &= ~NNS_G3D_RENDEROBJ_FLAG_RECORD; 
}


/*---------------------------------------------------------------------------*
    updateHintVec_

    pAnmObjで登録されているリソースIDに対応するビットをONにしていく
 *---------------------------------------------------------------------------*/
static void
updateHintVec_(u32* pVec, const NNSG3dAnmObj* pAnmObj)
{
    const NNSG3dAnmObj* p = pAnmObj;
    while(p)
    {
        int i;
        for (i = 0; i < p->numMapData; ++i)
        {
            if (p->mapData[i] & NNS_G3D_ANMOBJ_MAPDATA_EXIST)
            {
                pVec[i >> 5] |= 1 << (i & 31);
            }
        }
        p = p->next;
    }
}



/*---------------------------------------------------------------------------*
    NNS_G3dDraw

    モデルを描画する。アニメーション等の設定は事前にpRenderObjを操作することに
    よって行う。
 *---------------------------------------------------------------------------*/
void
NNS_G3dDraw(NNSG3dRenderObj* pRenderObj)
{
    // NOTICE
    // NOT REENTRANT
    NNS_G3D_NULL_ASSERT(pRenderObj);
    NNS_G3D_NULL_ASSERT(pRenderObj->resMdl);
    NNS_G3D_ASSERTMSG(pRenderObj->resMdl->info.numMat <= NNS_G3D_SIZE_MAT_MAX,
                      "numMaterial exceeds NNS_G3D_SIZE_MAT_MAX");
    NNS_G3D_ASSERTMSG(pRenderObj->resMdl->info.numNode <= NNS_G3D_SIZE_JNT_MAX,
                      "numNode exceeds NNS_G3D_SIZE_JNT_MAX");
    NNS_G3D_ASSERTMSG(pRenderObj->resMdl->info.numShp <= NNS_G3D_SIZE_SHP_MAX,
                      "numShape exceeds NNS_G3D_SIZE_SHP_MAX");

    if (NNS_G3dRenderObjTestFlag(pRenderObj, NNS_G3D_RENDEROBJ_FLAG_HINT_OBSOLETE))
    {
        // hint用ビットベクトルが正しくないのでアップデートする
        MI_CpuClear32(&pRenderObj->hintMatAnmExist[0], sizeof(u32) * (NNS_G3D_SIZE_MAT_MAX / 32));
        MI_CpuClear32(&pRenderObj->hintJntAnmExist[0], sizeof(u32) * (NNS_G3D_SIZE_JNT_MAX / 32));
        MI_CpuClear32(&pRenderObj->hintVisAnmExist[0], sizeof(u32) * (NNS_G3D_SIZE_JNT_MAX / 32));

        if (pRenderObj->anmMat)
            updateHintVec_(&pRenderObj->hintMatAnmExist[0], pRenderObj->anmMat);
        if (pRenderObj->anmJnt)
            updateHintVec_(&pRenderObj->hintJntAnmExist[0], pRenderObj->anmJnt);
        if (pRenderObj->anmVis)
            updateHintVec_(&pRenderObj->hintVisAnmExist[0], pRenderObj->anmVis);

        NNS_G3dRenderObjResetFlag(pRenderObj, NNS_G3D_RENDEROBJ_FLAG_HINT_OBSOLETE);
    }

    if (NNS_G3dRS)
    {
        G3dDrawInternal_(NNS_G3dRS, pRenderObj);
    }
    else
    {
        // SBCランタイム構造体領域が確保されていない場合はローカルスタックにとる
        NNSG3dRS rs;
        NNS_G3dRS = &rs;
        G3dDrawInternal_(&rs, pRenderObj);
        NNS_G3dRS = NULL;
    }
}



//
// コールバック判定コードのコーディング関する注意喚起
// NOTICE:
// コールバック関数内部でコールバック条件がユーザーによって書き換えられることが
// あることに注意してください。つまり、コールバックがあるかどうかの判定を
// auto変数に格納して、次のコールバック判定に流用してはいけません。
//

/*---------------------------------------------------------------------------*
    NNSi_G3dFuncSbc_NOP

    mnemonic:   NNS_G3D_SBC_NOP
    operands:   none
    callbacks:  A/B/C

    動作説明
    何もしない。

    付記
    コールバックは有効であるが、タイミングの指定は無意味である。
 *---------------------------------------------------------------------------*/
void
NNSi_G3dFuncSbc_NOP(NNSG3dRS* rs, u32)
{
    NNS_G3D_NULL_ASSERT(rs);

#if !defined(NNS_G3D_SBC_CALLBACK_TIMING_A_DISABLE) || \
    !defined(NNS_G3D_SBC_CALLBACK_TIMING_B_DISABLE) || \
    !defined(NNS_G3D_SBC_CALLBACK_TIMING_C_DISABLE)
    
    if (rs->cbVecFunc[NNS_G3D_SBC_NOP])
    {
        (*rs->cbVecFunc[NNS_G3D_SBC_NOP])(rs);
    }
#endif

    rs->c++;
}


/*---------------------------------------------------------------------------*
    NNSi_G3dFuncSbc_RET

    mnemonic:   NNS_G3D_SBC_RET
    operands:   none
    callbacks:  A/B/C

    動作説明
    NNS_G3D_FUNCSBC_STATUS_RETURNを返して、Sbcの実行を終える.

    付記
    SBCの最後にあるコマンド。
    コールバックは有効であるが、タイミングの指定は無意味である。
    なお、この命令はNNS_G3dRS->cをインクリメントしない。
 *---------------------------------------------------------------------------*/
void
NNSi_G3dFuncSbc_RET(NNSG3dRS* rs, u32)
{
    NNS_G3D_NULL_ASSERT(rs);

#if !defined(NNS_G3D_SBC_CALLBACK_TIMING_A_DISABLE) || \
    !defined(NNS_G3D_SBC_CALLBACK_TIMING_B_DISABLE) || \
    !defined(NNS_G3D_SBC_CALLBACK_TIMING_C_DISABLE)

    if (rs->cbVecFunc[NNS_G3D_SBC_RET])
    {
        (*rs->cbVecFunc[NNS_G3D_SBC_RET])(rs);
    }
#endif

    rs->flag |= NNS_G3D_RSFLAG_RETURN;
}


/*---------------------------------------------------------------------------*
    NNSi_G3dFuncSbc_NODE

    mnemonic:   NNS_G3D_SBC_NODE
    operands:   nodeID
                flags(1byte)
                    visible if 0x01 is on
    callbacks:  A: before
                B: none
                C: after
    
    動作説明
    ・ TIMING_Aのコールバックを呼ぶ
    ・ ビジビリティアニメーションの実行前チェックを行う
       ビジビリティアニメーションが存在する場合：
           ビジビリティアニメーションブレンド関数の結果(返り値ではない)に従って
           NNS_G3D_RSFLAG_NODE_VISIBLEビットをセット
       ない場合：
           flagsを参照して、NNS_G3D_RSFLAG_NODE_VISIBLEビットをセット
    ・ TIMING_Bのコールバックを呼ぶ
    ・ rs->cを加算

    付記
    NNS_G3D_RSFLAG_NODE_VISIBLEビットは、この関数内でのみ変更される。
 *---------------------------------------------------------------------------*/
void
NNSi_G3dFuncSbc_NODE(NNSG3dRS* rs, u32)
{
    NNS_G3D_NULL_ASSERT(rs);

    if (!(rs->flag & NNS_G3D_RSFLAG_OPT_SKIP_SBCDRAW))
    {
        BOOL                    cbFlag;
        NNSG3dSbcCallBackTiming cbTiming;
        u32                     curNode;

        curNode = rs->currentNode = *(rs->c + 1);
        rs->flag |= NNS_G3D_RSFLAG_CURRENT_NODE_VALID;
        rs->pVisAnmResult = &rs->tmpVisAnmResult;

        // コールバックＡ
        // 内部でNNS_G3D_RSFLAG_SKIPをセットした場合、
        // rs->pVisAnmResultを操作することで
        // ビジビリティ計算を乗っ取ることができる
        cbFlag = NNSi_G3dCallBackCheck_A(rs, NNS_G3D_SBC_NODE, &cbTiming);

        if (!cbFlag)
        {
            NNS_G3D_NULL_ASSERT(rs->pRenderObj->funcBlendVis);

            if (rs->pRenderObj->anmVis                                            &&
                NNSi_G3dBitVecCheck(&rs->pRenderObj->hintVisAnmExist[0], curNode) &&
                (*rs->pRenderObj->funcBlendVis)(rs->pVisAnmResult, rs->pRenderObj->anmVis, curNode))
            {
                // AnmObjのリストが設定されていて、
                // ビジビリティアニメーションに関するヒントベクトルのビットがONになっていて
                // ビジビリティアニメーションブレンダ内で実際に該当するビジビリティアニメーションがあった場合
                ;
            }
            else
            {
                // ビジビリティアニメーションがない場合、
                // オペランドのデータから判断する。
                rs->pVisAnmResult->isVisible = *(rs->c + 2) & 1;
            }
        }
        
        // コールバックＢ
        // 計算されたビジビリティをrs->pVisAnmResultを操作することで
        // 改変することができる。
        cbFlag = NNSi_G3dCallBackCheck_B(rs, NNS_G3D_SBC_NODE, &cbTiming);

        if (!cbFlag)
        {
            if (rs->pVisAnmResult->isVisible)
            {
                rs->flag |= NNS_G3D_RSFLAG_NODE_VISIBLE;
            }
            else
            {
                rs->flag &= ~NNS_G3D_RSFLAG_NODE_VISIBLE;
            }
        }

        // コールバックＣ
        // 次の命令の前に何らかの処理を挿入することができる。
        (void)NNSi_G3dCallBackCheck_C(rs, NNS_G3D_SBC_NODE, cbTiming);
    }

    rs->c += 3;
}


/*---------------------------------------------------------------------------*
    NNSi_G3dFuncSbc_MTX

    mnemonic:   NNS_G3D_SBC_MTX
    operands:   idxMtx
    callbacks:  A: before
                B: none
                C: after

    動作説明
    ・ NNS_G3D_RSFLAG_NODE_VISIBLEがOFFならばこの命令全体をスキップ
    ・ TIMING_Aのコールバックを呼ぶ
    ・ 行列スタックからカレント行列にロード
    ・ TIMING_Bのコールバックを呼ぶ
    ・ rs->cを加算

    付記
    NNS_G3D_RSFLAG_NODE_VISIBLEがOFFの場合はコールバックも含めてスキップされる。
    開始前に、POSITION_VECTORモードでなくてはならない。
 *---------------------------------------------------------------------------*/
void
NNSi_G3dFuncSbc_MTX(NNSG3dRS* rs, u32)
{
    NNS_G3D_NULL_ASSERT(rs);

    // MTXコマンドはDrawカテゴリに入る
    // MAT,SHPの前に生成されるコマンドだから
    if (!(rs->flag & NNS_G3D_RSFLAG_OPT_SKIP_SBCDRAW) &&
        (rs->flag & NNS_G3D_RSFLAG_NODE_VISIBLE))
    {
        BOOL                    cbFlag;
        NNSG3dSbcCallBackTiming cbTiming;

        // コールバックＡ
        // 内部でNNS_G3D_RSFLAG_SKIPをセットした場合、
        // 行列のリストア動作を置き換えることができる。
        cbFlag = NNSi_G3dCallBackCheck_A(rs, NNS_G3D_SBC_MTX, &cbTiming);

        if (!cbFlag)
        {
            u32 arg = *(rs->c + 1);
            NNS_G3D_ASSERT(arg < 31);

            if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD))
            {
                NNS_G3dGeBufferOP_N(G3OP_MTX_RESTORE,
                                    &arg,
                                    G3OP_MTX_RESTORE_NPARAMS);
            }
        }

        // コールバックＣ
        // 次の命令の前に何らかの処理を挿入することができる。
        (void)NNSi_G3dCallBackCheck_C(rs, NNS_G3D_SBC_MTX, cbTiming);
    }

    rs->c += 2;
}


void
NNSi_G3dFuncSbc_MAT_InternalDefault(NNSG3dRS* rs,
                                    u32 opt,
                                    const NNSG3dResMatData* mat,
                                    u32 idxMat)
{
    static const u32 matColorMask_[8] =
    {
        0x00000000,
        0x00007fff,
        0x7fff0000,
        0x7fff7fff,
        0x00008000,
        0x0000ffff,
        0x7fff8000,
        0x7fffffff
    };
    BOOL cbFlag;
    NNSG3dSbcCallBackTiming cbTiming;

    NNS_G3D_NULL_ASSERT(rs);
    NNS_G3D_NULL_ASSERT(mat);

    rs->currentMat = (u8)idxMat;
    rs->flag |= NNS_G3D_RSFLAG_CURRENT_MAT_VALID;
    // コールバックが呼ばれる場合に使用される領域を指定しておかなくてはならない 
    rs->pMatAnmResult = &rs->tmpMatAnmResult;

    // コールバックＡ
    // 内部でNNS_G3D_RSFLAG_SKIPをセットした場合、
    // マテリアル設定コードを置き換えることができる。
    cbFlag = NNSi_G3dCallBackCheck_A(rs, NNS_G3D_SBC_MAT, &cbTiming);

    if (!cbFlag)
    {
        NNSG3dMatAnmResult* pResult;

        //
        // 計算結果のバッファを使うかどうかを決定する。
        //
        if (rs->pRenderObj->recMatAnm &&
            !(rs->flag & NNS_G3D_RSFLAG_OPT_RECORD))
        {
            // バッファが使用できる場合
            pResult = (rs->pRenderObj->recMatAnm + idxMat);
        }
        else
        {
            //
            // このifブロックでは、マテリアルデータの取得や計算を行い、
            // pResultに適切なデータを設定する。
            //
            if ((opt == NNS_G3D_SBCFLG_001 || opt == NNS_G3D_SBCFLG_010) &&
                NNSi_G3dBitVecCheck(&rs->isMatCached[0], idxMat))
            {
                // MATキャッシュ済みフラグが立っている場合
                if (rs->pRenderObj->recMatAnm)
                {
                    // 以前の計算結果がバッファに記録されているのでそこから取り出す
                    pResult = (rs->pRenderObj->recMatAnm + idxMat);
                }
                else
                {
                    // 以前の計算結果がキャッシュに記録されているのでそこから取り出す
                    pResult = &NNS_G3dRSOnGlb.matCache[idxMat];
                }
            }
            else
            {
                if (rs->pRenderObj->recMatAnm)
                {
                    // MATキャッシュ済みフラグを立てる
                    NNSi_G3dBitVecSet(&rs->isMatCached[0], idxMat);

                    // バッファがある場合は命令オプションに関わらずバッファに記録
                    pResult = (rs->pRenderObj->recMatAnm + idxMat);
                }
                else if (opt == NNS_G3D_SBCFLG_010)
                {
                    // MATキャッシュ済みフラグを立てる
                    NNSi_G3dBitVecSet(&rs->isMatCached[0], idxMat);

                    // 書き込み先をMATキャッシュに設定
                    pResult = &NNS_G3dRSOnGlb.matCache[idxMat];
                }
                else
                {
                    // NNSG3dRS上の領域を使用
                    pResult = &rs->tmpMatAnmResult;
                }

                //
                // モデルデータのマテリアル情報をセット
                // G3dGlbのデフォルトと合成する
                //
                {
                    pResult->flag           = (NNSG3dMatAnmResultFlag) 0;
                    if (NNS_G3dGetMatDataByIdx(rs->pResMat, idxMat)->flag & NNS_G3D_MATFLAG_WIREFRAME)
                    {
                        pResult->flag |= NNS_G3D_MATANM_RESULTFLAG_WIREFRAME;
                    }
                }
                {
                    // Diffuse & Ambient & VtxColor
                    u32 mask = matColorMask_[(mat->flag >> 6) & 7];
                    pResult->prmMatColor0 = (NNS_G3dGlb.prmMatColor0 & ~mask) |
                                            (mat->diffAmb & mask);
                }

                {
                    // Specular & Emission & Shininess
                    u32 mask = matColorMask_[(mat->flag >> 9) & 7];
                    pResult->prmMatColor1 = (NNS_G3dGlb.prmMatColor1 & ~mask) |
                                            (mat->specEmi & mask);
                }

                pResult->prmPolygonAttr = (NNS_G3dGlb.prmPolygonAttr & ~mat->polyAttrMask) |
                                          (mat->polyAttr & mat->polyAttrMask);

                pResult->prmTexImage = mat->texImageParam;
                pResult->prmTexPltt  = mat->texPlttBase;

                // モデルにテクスチャ行列が存在する場合は、
                // テクスチャ行列をモデルからセット
                if (mat->flag & NNS_G3D_MATFLAG_TEXMTX_USE)
                {
                    const u8* p = (const u8*)mat + sizeof(NNSG3dResMatData);

                    if (!(mat->flag & NNS_G3D_MATFLAG_TEXMTX_SCALEONE))
                    {
                        const fx32* p_fx32 = (const fx32*)p;

                        pResult->scaleS = *(p_fx32 + 0);
                        pResult->scaleT = *(p_fx32 + 1);
                        p += 2 * sizeof(fx32);
                    }
                    else
                    {
                        pResult->flag |= NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SCALEONE;
                    }

                    if (!(mat->flag & NNS_G3D_MATFLAG_TEXMTX_ROTZERO))
                    {
                        const fx16* p_fx16 = (const fx16*)p;

                        pResult->sinR = *(p_fx16 + 0);
                        pResult->cosR = *(p_fx16 + 1);
                        p += 2 * sizeof(fx16);
                    }
                    else
                    {
                        pResult->flag |= NNS_G3D_MATANM_RESULTFLAG_TEXMTX_ROTZERO;
                    }

                    if (!(mat->flag & NNS_G3D_MATFLAG_TEXMTX_TRANSZERO))
                    {
                        const fx32* p_fx32 = (const fx32*)p;

                        pResult->transS = *(p_fx32 + 0);
                        pResult->transT = *(p_fx32 + 1);
                    }
                    else
                    {
                        pResult->flag |= NNS_G3D_MATANM_RESULTFLAG_TEXMTX_TRANSZERO;
                    }

                    pResult->flag |= NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SET;
                }

                NNS_G3D_NULL_ASSERT(rs->pRenderObj->funcBlendMat);

                // マテリアルアニメーションのチェック
                if (rs->pRenderObj->anmMat &&
                    NNSi_G3dBitVecCheck(&rs->pRenderObj->hintMatAnmExist[0], idxMat))
                {
                    // マテリアルアニメーションの計算により、pResultを変更するかもしれない
                    (void)(*rs->pRenderObj->funcBlendMat)(pResult, rs->pRenderObj->anmMat, idxMat);
                }

                //
                // 元々テクスチャ行列がある場合やアニメーションで付加されている場合は
                // テクスチャの幅高情報等を追加する。
                //
                if ( pResult->flag & (NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SET |
                                      NNS_G3D_MATANM_RESULTFLAG_TEXMTX_MULT))
                {
                    pResult->origWidth  = mat->origWidth;
                    pResult->origHeight = mat->origHeight;
                    pResult->magW = mat->magW;
                    pResult->magH = mat->magH;
                }
            }
        }
        rs->pMatAnmResult = pResult;
    }

    // コールバックＢ
    // 内部でNNS_G3D_RSFLAG_SKIPをセットした場合、
    // マテリアルの送信動作を置き換えることができる。
    cbFlag = NNSi_G3dCallBackCheck_B(rs, NNS_G3D_SBC_MAT, &cbTiming);

    if (!cbFlag)
    {
        NNSG3dMatAnmResult* pResult;

        NNS_G3D_NULL_ASSERT(rs->pMatAnmResult);
        pResult = rs->pMatAnmResult;

        // 透明マテリアルか否かのチェック
        if (pResult->prmPolygonAttr & REG_G3_POLYGON_ATTR_ALPHA_MASK)
        {
            // 透明でないかワイヤーフレーム表示の場合は、
            // ジオメトリコマンドの送信
            
            if (pResult->flag & NNS_G3D_MATANM_RESULTFLAG_WIREFRAME)
            {
                // ワイヤーフレームの場合はジオメトリエンジンに送るαは0になる
                pResult->prmPolygonAttr &= ~REG_G3_POLYGON_ATTR_ALPHA_MASK;
            }

            // 透明フラグをOFF
            rs->flag &= ~NNS_G3D_RSFLAG_MAT_TRANSPARENT;

            if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD))
            {
                // スタックにプッシュして転送
                u32 cmd[7];

                cmd[0] = GX_PACK_OP(G3OP_DIF_AMB, G3OP_SPE_EMI, G3OP_POLYGON_ATTR, G3OP_NOP);
                cmd[1] = pResult->prmMatColor0;
                cmd[2] = pResult->prmMatColor1;
                cmd[3] = pResult->prmPolygonAttr;
                cmd[4] = GX_PACK_OP(G3OP_TEXIMAGE_PARAM, G3OP_TEXPLTT_BASE, G3OP_NOP, G3OP_NOP);
                cmd[5] = pResult->prmTexImage;
                cmd[6] = pResult->prmTexPltt;

                NNS_G3dGeBufferData_N(&cmd[0], 7);

                if (pResult->flag & (NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SET |
                                     NNS_G3D_MATANM_RESULTFLAG_TEXMTX_MULT))
                {

                    // テクスチャ行列の送信
                    NNS_G3D_NULL_ASSERT(rs->funcTexMtx);
                    (*rs->funcTexMtx)(pResult);
                }
            }
        }
        else
        {
            // 透明フラグをＯＮ
            rs->flag |= NNS_G3D_RSFLAG_MAT_TRANSPARENT;
        }
    }

    // コールバックＣ
    // 次の命令の前に何らかの処理を挿入することができる。
    (void) NNSi_G3dCallBackCheck_C(rs, NNS_G3D_SBC_MAT, cbTiming);
}


/*---------------------------------------------------------------------------*
    NNSi_G3dFuncSbc_MAT

    mnemonic:   NNS_G3D_SBC_MAT([000], [001], [010])
    operands:   idxMat
    callbacks:  A: before
                B: during
                C: after

    動作説明
    ・ NNS_G3D_RSFLAG_NODE_VISIBLEがOFFならばこの命令全体をスキップ
    ・ TIMING_Aのコールバックを呼ぶ
    ・ マテリアル情報の取得/計算を行う
       optによってMATキャッシュ関連の動作が異なる
       rs->isMatCachedのidxMat番目のビットが立っていれば、MATキャッシュに
       マテリアル情報がキャッシュされていて、利用できる。

       [000] MATキャッシュは一切参照しない
       [001] MATキャッシュへの読み書きを行う
       [010] MATキャッシュを読む

       NNS_G3D_RSFLAG_MAT_TRANSPARENTの設定を行う
       セットされたなら、rs->cの加算まで処理をスキップする。
    ・ TIMING_Bのコールバックを呼ぶ
    ・ マテリアル情報をジオメトリエンジンへ送信
    ・ TIMING_Cのコールバックを呼ぶ
    ・ rs->cを加算

    付記
    [000],[001],[010]をどう使い分けるかはコンバート時に決定される。
 *---------------------------------------------------------------------------*/
void
NNSi_G3dFuncSbc_MAT(NNSG3dRS* rs, u32 opt)
{
    NNS_G3D_NULL_ASSERT(rs);

    if (!(rs->flag & NNS_G3D_RSFLAG_OPT_SKIP_SBCDRAW))
    {
        u32 idxMat;
        idxMat = *(rs->c + 1);

        // VISIBLEでない場合か現在既に同じMatID状態である場合は命令をスキップ
        if ((rs->flag & NNS_G3D_RSFLAG_NODE_VISIBLE) ||
            !((rs->flag & NNS_G3D_RSFLAG_CURRENT_MAT_VALID) &&
            (idxMat == rs->currentMat)))
        {
            const NNSG3dResMatData* mat =
                NNS_G3dGetMatDataByIdx(rs->pResMat, idxMat);

            NNS_G3D_NULL_ASSERT(NNS_G3dFuncSbcMatTable[mat->itemTag]);
            (*NNS_G3dFuncSbcMatTable[mat->itemTag])(rs, opt, mat, idxMat);
        }
    }
    rs->c += 2;
}


void
NNSi_G3dFuncSbc_SHP_InternalDefault(NNSG3dRS* rs,
                                    u32,
                                    const NNSG3dResShpData* shp,
                                    u32)
{
    BOOL cbFlag;
    NNSG3dSbcCallBackTiming cbTiming;
    NNS_G3D_NULL_ASSERT(rs);
    NNS_G3D_NULL_ASSERT(shp);

    // コールバックＡ
    // 内部でNNS_G3D_RSFLAG_SKIPをセットした場合、
    // ディスプレイリストの送信動作を置き換えることができる
    cbFlag = NNSi_G3dCallBackCheck_A(rs, NNS_G3D_SBC_SHP, &cbTiming);

    if (!cbFlag)
    {
        if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD))
        {
            NNS_G3dGeSendDL(NNS_G3dGetShpDLPtr(shp), NNS_G3dGetShpDLSize(shp));
        }
    }

    // コールバックＢ
    (void) NNSi_G3dCallBackCheck_B(rs, NNS_G3D_SBC_SHP, &cbTiming);

    // ステートの復帰
    // 将来的に状態処理等の処理を入れるかもしれない

    // コールバックＣ
    // 次の命令の前に何らかの処理を挿入することができる。
    (void) NNSi_G3dCallBackCheck_C(rs, NNS_G3D_SBC_SHP, cbTiming);
}


/*---------------------------------------------------------------------------*
    NNSi_G3dFuncSbc_SHP

    mnemonic:   NNS_G3D_SBC_SHP
    operands:   idxShp
    callbacks:  A: before
                B: during
                C: after

    動作説明
    ・ NNS_G3D_RSFLAG_NODE_VISIBLEがOFF、又はNNS_G3D_RSFLAG_MAT_TRANSPARENTがON
       ならばこの命令全体をスキップ
    ・ TIMING_Aのコールバックを呼ぶ
    ・ ディスプレイリストをコール
    ・ TIMING_Bのコールバックを呼ぶ
    ・ ディスプレイリストが変更したステートを復帰する
    ・ TIMING_Cのコールバックを呼ぶ
    ・ rs->cの加算

    付記
 *---------------------------------------------------------------------------*/
void
NNSi_G3dFuncSbc_SHP(NNSG3dRS* rs, u32 opt)
{
    NNS_G3D_NULL_ASSERT(rs);

    if (!(rs->flag & NNS_G3D_RSFLAG_OPT_SKIP_SBCDRAW))
    {
        if ((rs->flag & NNS_G3D_RSFLAG_NODE_VISIBLE) &&
            !(rs->flag & NNS_G3D_RSFLAG_MAT_TRANSPARENT))
        {
            u32 idxShp = *(rs->c + 1);
            const NNSG3dResShpData* shp =
                NNS_G3dGetShpDataByIdx(rs->pResShp, idxShp);

            NNS_G3D_NULL_ASSERT(NNS_G3dFuncSbcShpTable[shp->itemTag]);
            (*NNS_G3dFuncSbcShpTable[shp->itemTag])(rs, opt, shp, idxShp);
        }
    }
    rs->c += 2;
}


/*---------------------------------------------------------------------------*
    NNSi_G3dFuncSbc_NODEDESC([000], [001], [010], [011])

    mnemonic:   NNS_G3D_SBC_NODEDESC
    operands:   [000]: idxNode, idxNodeParent, flags
                [001]: idxNode, idxNodeParent, flags, idxMtxDest
                [010]: idxNode, idxNodeParent, flags, idxMtxSrc
                [011]: idxNode, idxNodeParent, flags, idxMtxDest, idxMtxSrc
                0 <= idxMtxSrc, idxMtxDest <= 30
                x = 0 --> checks visibility anm and change currentNode
                x = 1 --> no
    callbacks:  A: before
                B: during
                C: after

    動作説明
    ・ [010], [011]ならば、idxMtxSrcをカレント行列にリストアする
    ・ TIMING_Aのコールバックを呼ぶ
    ・ カレント行列に対し、モデル/ジョイントアニメーションの行列を乗算する
    ・ TIMING_Bのコールバックを呼ぶ
    ・ [001], [011]ならば、idxMtxDestにカレント行列をストアする
    ・ TIMING_Cのコールバックを呼ぶ
    ・ rs->cの加算

    付記
    NNS_G3dFuncSbc_NODEのようにNODE状態を変更させることはない
 *---------------------------------------------------------------------------*/
void
NNSi_G3dFuncSbc_NODEDESC(NNSG3dRS* rs, u32 opt)
{
    u32 cmdLen = 4;
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
    BOOL cbFlag;
    NNSG3dSbcCallBackTiming cbTiming;
    u32 idxNode = *(rs->c + 1);
    rs->currentNodeDesc = (u8)idxNode;
    rs->flag |= NNS_G3D_RSFLAG_CURRENT_NODEDESC_VALID;

    NNS_G3D_NULL_ASSERT(rs);

    if (rs->flag & NNS_G3D_RSFLAG_OPT_SKIP_SBCMTXCALC)
    {
        if (opt == NNS_G3D_SBCFLG_010 ||
            opt == NNS_G3D_SBCFLG_011)
        {
            ++cmdLen;
        }


        if (opt == NNS_G3D_SBCFLG_001 ||
            opt == NNS_G3D_SBCFLG_011)
        {
            ++cmdLen;
#if 1
            // 後にそのまま行列を使う可能性があるので、
            // ストア動作をリストアで置き換えておく必要がある
            if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD))
            {
                u32 idxMtxDest = *(rs->c + 4);
                NNS_G3D_ASSERT(idxMtxDest < 31);

                NNS_G3dGeBufferOP_N(G3OP_MTX_RESTORE,
                                    &idxMtxDest,
                                    G3OP_MTX_RESTORE_NPARAMS);
            }
#endif
        }
        rs->c += cmdLen;
        return;
    }

    {
        if (opt == NNS_G3D_SBCFLG_010 ||
            opt == NNS_G3D_SBCFLG_011)
        {
            u32 idxMtxSrc;

            ++cmdLen;
            if (opt == NNS_G3D_SBCFLG_010)
            {
                idxMtxSrc = *(rs->c + 4);
            }
            else
            {
                idxMtxSrc = *(rs->c + 5);
            }
            NNS_G3D_ASSERT(idxMtxSrc < 31);

            if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD))
            {
                NNS_G3dGeBufferOP_N(G3OP_MTX_RESTORE,
                                    &idxMtxSrc,
                                    G3OP_MTX_RESTORE_NPARAMS);
            }
        }
    }

    // コールバックが呼ばれる場合に使用される領域を指定しておかなくてはならない 
    rs->pJntAnmResult = &rs->tmpJntAnmResult;

    // コールバックＡ
    // 内部でNNS_G3D_RSFLAG_SKIPをセットした場合、
    // ジョイントSRTデータの取得動作を置き換えることができる。
    cbFlag = NNSi_G3dCallBackCheck_A(rs, NNS_G3D_SBC_NODEDESC, &cbTiming);

    if (!cbFlag)
    {
        NNSG3dJntAnmResult* pAnmResult;
        BOOL                isUseRecordData;
//        u32 idxNodeParent = *(rs->c + 2);
//        u32 flags         = *(rs->c + 3);

        //
        // 計算結果のバッファを使うかどうかを決定する。
        //
        if (rs->pRenderObj->recJntAnm)
        {
            // 外部バッファをポイントする
            pAnmResult = (rs->pRenderObj->recJntAnm + idxNode);
            if (!(rs->flag & NNS_G3D_RSFLAG_OPT_RECORD))
            {
                // 外部バッファのデータを再生する。
                isUseRecordData = TRUE;
            }
            else
            {
                // 外部バッファにデータを記録する。
                isUseRecordData = FALSE;
            }
        }
        else
        {
            // 関数内部のバッファをテンポラリで使用
            isUseRecordData = FALSE;
            pAnmResult = &rs->tmpJntAnmResult;
        }

        if (!isUseRecordData)
        {
            // 計算の前にNNSG3dJntAnmResult::flagだけはあらかじめクリアしておく必要がある。
            pAnmResult->flag = (NNSG3dJntAnmResultFlag) 0;

            NNS_G3D_NULL_ASSERT(rs->pRenderObj->funcBlendJnt);
            if (rs->pRenderObj->anmJnt &&
                (*rs->pRenderObj->funcBlendJnt)(pAnmResult, rs->pRenderObj->anmJnt, idxNode))
            {
                //
                // ジョイントアニメーションのリソースを使用してanmResultが得られている。
                //
                ;
            }
            else
            {
                //
                // 静止モデルのリソースを使用してanmResultを得る
                //
                const NNSG3dResNodeData* pNd =
                    NNS_G3dGetNodeDataByIdx(rs->pResNodeInfo, idxNode);
                const u8* p = (const u8*)pNd + sizeof(NNSG3dResNodeData);

                //
                // Translation
                //
                if (pNd->flag & NNS_G3D_SRTFLAG_TRANS_ZERO)
                {
                    pAnmResult->flag |= NNS_G3D_JNTANM_RESULTFLAG_TRANS_ZERO;
                }
                else
                {
                    const fx32* p_fx32 = (const fx32*)p;

                    pAnmResult->trans.x = *(p_fx32 + 0);
                    pAnmResult->trans.y = *(p_fx32 + 1);
                    pAnmResult->trans.z = *(p_fx32 + 2);
                    p += 3 * sizeof(fx32);
                }

                //
                // Rotation
                //
                if (pNd->flag & NNS_G3D_SRTFLAG_ROT_ZERO)
                {
                    pAnmResult->flag |= NNS_G3D_JNTANM_RESULTFLAG_ROT_ZERO;
                }
                else
                {
                    if (pNd->flag & NNS_G3D_SRTFLAG_PIVOT_EXIST)
                    {
                        //
                        // 圧縮されている場合(主に一軸回転)は
                        // 圧縮された行列を元に戻す
                        //
                        fx32 A = *(fx16*)(p + 0);
                        fx32 B = *(fx16*)(p + 2);
                        u32 idxPivot = (u32)( (pNd->flag & NNS_G3D_SRTFLAG_IDXPIVOT_MASK) >> 
                                                        NNS_G3D_SRTFLAG_IDXPIVOT_SHIFT );
                        // anmResult.rotをクリア
                        MI_Zero36B(&pAnmResult->rot);
                        
                        pAnmResult->rot.a[idxPivot] =
                            (pNd->flag & NNS_G3D_SRTFLAG_PIVOT_MINUS) ?
                                -FX32_ONE :
                                FX32_ONE;
                        
                        pAnmResult->rot.a[pivotUtil_[idxPivot][0]] = A;
                        pAnmResult->rot.a[pivotUtil_[idxPivot][1]] = B;

                        pAnmResult->rot.a[pivotUtil_[idxPivot][2]] =
                            (pNd->flag & NNS_G3D_SRTFLAG_SIGN_REVC) ? -B : B;

                        pAnmResult->rot.a[pivotUtil_[idxPivot][3]] =
                            (pNd->flag & NNS_G3D_SRTFLAG_SIGN_REVD) ? -A : A;

                        p += 2 * sizeof(fx16);
                    }
                    else
                    {
                        // NOTICE:
                        // メモリコピーAPIに置き換えないこと
                        // fx16からfx32への暗黙のキャストを行っているから

                        const fx16* pp = (const fx16*)p;
                        pAnmResult->rot.a[0] = pNd->_00;
                        pAnmResult->rot.a[1] = *(pp + 0);
                        pAnmResult->rot.a[2] = *(pp + 1);
                        pAnmResult->rot.a[3] = *(pp + 2);
                        pAnmResult->rot.a[4] = *(pp + 3);
                        pAnmResult->rot.a[5] = *(pp + 4);
                        pAnmResult->rot.a[6] = *(pp + 5);
                        pAnmResult->rot.a[7] = *(pp + 6);
                        pAnmResult->rot.a[8] = *(pp + 7);

                        p += 8 * sizeof(fx16);
                    }
                }

                //
                // Scale
                //

                //
                // NOTICE:
                // MayaSSCやSi3dのclassic scale offの場合には
                // NNS_G3D_SRTFLAG_SCALE_ONEのときにもフラグを立てる以外の
                // 計算が発生しうる。
                //
                NNS_G3D_NULL_ASSERT(rs->funcJntScale);
                (*rs->funcJntScale)(pAnmResult, (fx32*)p, rs->c, pNd->flag);
            }
        }
        rs->pJntAnmResult = pAnmResult;
    }

    // コールバックＢ
    // 内部でNNS_G3D_RSFLAG_SKIPをセットした場合、
    // ジオメトリエンジンへの送信動作を置き換えることができる。
    cbFlag = NNSi_G3dCallBackCheck_B(rs, NNS_G3D_SBC_NODEDESC, &cbTiming);

    if (!cbFlag)
    {
        //
        // ジオメトリエンジンへの送信
        //
        if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD))
        {
            NNS_G3D_NULL_ASSERT(rs->pJntAnmResult);
            NNS_G3D_NULL_ASSERT(rs->funcJntMtx);

            (*rs->funcJntMtx)(rs->pJntAnmResult);
        }
    }

    rs->pJntAnmResult = NULL;

    // コールバックＣ
    // 次の命令の前に何らかの処理を挿入することができる。
    // また、NNS_G3D_RSFLAG_SKIPをオンにすることによって
    // カレント行列のストア動作を置き換えることができる
    cbFlag = NNSi_G3dCallBackCheck_C(rs, NNS_G3D_SBC_NODEDESC, cbTiming);

    if (opt == NNS_G3D_SBCFLG_001 ||
        opt == NNS_G3D_SBCFLG_011)
    {
        ++cmdLen;

        if (!cbFlag)
        {
            if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD))
            {
                u32 idxMtxDest = *(rs->c + 4);
                NNS_G3D_ASSERT(idxMtxDest < 31);

                NNS_G3dGeBufferOP_N(G3OP_MTX_STORE,
                                    &idxMtxDest,
                                    G3OP_MTX_STORE_NPARAMS);
            }
        }
    }

    rs->c += cmdLen;
}


/*---------------------------------------------------------------------------*
    NNSi_G3dFuncSbc_BB([000], [001], [010], [011])

    mnemonic:   NNS_G3D_SBC_BB
    operands:   [000]: idxNode
                [001]: idxNode, idxMtxDest
                [010]: idxNode, idxMtxSrc
                [011]: idxNode, idxMtxDest, idxMtxSrc
                0 <= idxMtxSrc, idxMtxDest <= 30
    callbacks:  A: before
                B: during
                C: after

    動作説明
    ・ [010], [011]ならば、idxMtxSrcをカレント行列にリストアする
    ・ TIMING_Aのコールバックを呼ぶ
    ・ 射影行列をプッシュし、単位行列をセット。
    ・ ジオメトリエンジンが止まるまで待ち、カレント行列を取り出す。
    ・ CPUでビルボード行列になるよう加工し、カレント行列に格納
    ・ TIMING_Bのコールバックを呼ぶ
    ・ [001], [011]ならば、idxMtxDestにカレント行列をストアする
    ・ TIMING_Cのコールバックを呼ぶ
    ・ rs->cの加算

    付記
 *---------------------------------------------------------------------------*/
void
NNSi_G3dFuncSbc_BB(NNSG3dRS* rs, u32 opt)
{
    //
    // 「カメラの射影平面」に平行なビルボードの表示
    //
    u32 cmdLen = 2;

    static u32 bbcmd1[] =
    {
        GX_PACK_OP(G3OP_MTX_POP, G3OP_MTX_MODE, G3OP_MTX_LOAD_4x3, G3OP_MTX_SCALE),
        1,
        GX_MTXMODE_POSITION_VECTOR,
        FX32_ONE, 0, 0, 
        0, FX32_ONE, 0,
        0, 0, FX32_ONE,
        0, 0, 0,   // ここは可変(Trans)
        0, 0, 0    // ここは可変(Scale)
    };
    
    VecFx32* trans = (VecFx32*)&bbcmd1[12];
    VecFx32* scale = (VecFx32*)&bbcmd1[15];
    MtxFx44 m;
    BOOL cbFlag;
    NNSG3dSbcCallBackTiming cbTiming;

    NNS_G3D_NULL_ASSERT(rs);

    if (rs->flag & NNS_G3D_RSFLAG_OPT_SKIP_SBCDRAW)
    {
        // ビルボードはDrawカテゴリに入る
        if (opt == NNS_G3D_SBCFLG_010 ||
            opt == NNS_G3D_SBCFLG_011)
        {
            ++cmdLen;
        }
        if (opt == NNS_G3D_SBCFLG_001 ||
            opt == NNS_G3D_SBCFLG_011)
        {
            ++cmdLen;
        }
        rs->c += cmdLen;
        return;
    }

    {
        if (opt == NNS_G3D_SBCFLG_010 ||
            opt == NNS_G3D_SBCFLG_011)
        {
            ++cmdLen;

            if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD))
            {
                u32 idxMtxSrc;
                if (opt == NNS_G3D_SBCFLG_010)
                {
                    idxMtxSrc = *(rs->c + 2);
                }
                else
                {
                    idxMtxSrc = *(rs->c + 3);
                }
                NNS_G3D_ASSERT(idxMtxSrc < 31);

                NNS_G3dGeBufferOP_N(G3OP_MTX_RESTORE,
                                    &idxMtxSrc,
                                    1);
            }
        }
    }

    // コールバックＡ
    // 内部でNNS_G3D_RSFLAG_SKIPをセットした場合、
    // ビルボード行列の設定動作を置き換えることができる。
    cbFlag = NNSi_G3dCallBackCheck_A(rs, NNS_G3D_SBC_BB, &cbTiming);

    if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD) &&
        (!cbFlag))
    {
        // バッファのフラッシュ
        NNS_G3dGeFlushBuffer();

        //
        // 次のコールバックポイントまでイミディエイト送信可能.
        //

        // コマンド転送:
        // PROJモードに変更
        // 射影行列を退避
        // 単位行列をセット
        reg_G3X_GXFIFO = GX_PACK_OP(G3OP_MTX_MODE, G3OP_MTX_PUSH, G3OP_MTX_IDENTITY, G3OP_NOP);
        reg_G3X_GXFIFO = (u32)GX_MTXMODE_PROJECTION;
        reg_G3X_GXFIFO = 0; // 2004/08/26 geometry fifo glitch

        // 更にジオメトリエンジンの停止を待つ
        // カレント行列の取得
        while (G3X_GetClipMtx(&m))
            ;

        if (NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_FLUSH_WVP)
        {
            // Projection行列にカメラ行列がかかっている場合は、
            // 改めてSRT付カメラ行列を後ろからかけなければならない。
            const MtxFx43* cam = NNS_G3dGlbGetSrtCameraMtx();
            MtxFx44 tmp;

            MTX_Copy43To44(cam, &tmp);
            MTX_Concat44(&m, &tmp, &m);
        }
        else if (NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_FLUSH_VP)
        {
            const MtxFx43* cam = NNS_G3dGlbGetCameraMtx();
            MtxFx44 tmp;

            MTX_Copy43To44(cam, &tmp);
            MTX_Concat44(&m, &tmp, &m);
        }

        // ビルボード行列の計算
        trans->x = m._30;
        trans->y = m._31;
        trans->z = m._32;

        scale->x = VEC_Mag((VecFx32*)&m._00);
        scale->y = VEC_Mag((VecFx32*)&m._10);
        scale->z = VEC_Mag((VecFx32*)&m._20);

        if (NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_FLUSH_WVP)
        {
            // 射影行列のPOP
            // POS_VECに復帰
            // SRT付カメラの逆行列をセット
            reg_G3X_GXFIFO = GX_PACK_OP(G3OP_MTX_POP, G3OP_MTX_MODE, G3OP_MTX_LOAD_4x3, G3OP_NOP);
            MI_CpuSend32(&bbcmd1[1],
                         &reg_G3X_GXFIFO,
                         2 * sizeof(u32));
            MI_CpuSend32(NNS_G3dGlbGetInvSrtCameraMtx(),
                         &reg_G3X_GXFIFO,
                         G3OP_MTX_LOAD_4x3_NPARAMS * sizeof(u32));

            // カレント行列にかける
            // 計算したスケールをかける
            reg_G3X_GXFIFO = GX_PACK_OP(G3OP_MTX_MULT_4x3, G3OP_MTX_SCALE, G3OP_NOP, G3OP_NOP);
            MI_CpuSend32(&bbcmd1[3],
                         &reg_G3X_GXFIFO,
                         sizeof(MtxFx43) + sizeof(VecFx32));
        }
        else if (NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_FLUSH_VP)
        {
            // 射影行列のPOP
            // POS_VECに復帰
            // カメラの逆行列をセット
            reg_G3X_GXFIFO = GX_PACK_OP(G3OP_MTX_POP, G3OP_MTX_MODE, G3OP_MTX_LOAD_4x3, G3OP_NOP);
            MI_CpuSend32(&bbcmd1[1],
                         &reg_G3X_GXFIFO,
                         2 * sizeof(u32));
            MI_CpuSend32(NNS_G3dGlbGetInvCameraMtx(),
                         &reg_G3X_GXFIFO,
                         G3OP_MTX_LOAD_4x3_NPARAMS * sizeof(u32));

            // カレント行列にかける
            // 計算したスケールをかける
            reg_G3X_GXFIFO = GX_PACK_OP(G3OP_MTX_MULT_4x3, G3OP_MTX_SCALE, G3OP_NOP, G3OP_NOP);
            MI_CpuSend32(&bbcmd1[3],
                         &reg_G3X_GXFIFO,
                         sizeof(MtxFx43) + sizeof(VecFx32));
        }
        else
        {
            // 射影行列のPOP
            // POS_VECに復帰
            // カレント行列に格納
            // 計算したスケールをかける
            MI_CpuSend32(&bbcmd1[0],
                         &reg_G3X_GXFIFO,
                         18 * sizeof(u32));
        }
    }

    // コールバックＣ
    // 次の命令の前に何らかの処理を挿入することができる。
    // また、NNS_G3D_RSFLAG_SKIPをオンにすることによって
    // カレント行列のストア動作を置き換えることができる
    cbFlag = NNSi_G3dCallBackCheck_C(rs, NNS_G3D_SBC_BB, cbTiming);

    if (opt == NNS_G3D_SBCFLG_001 ||
        opt == NNS_G3D_SBCFLG_011)
    {
        ++cmdLen;

        if (!cbFlag)
        {
            if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD))
            {
                u32 idxMtxDest;
                idxMtxDest = *(rs->c + 2);

                NNS_G3D_ASSERT(idxMtxDest < 31);

                // ここではイミディエイト送信可能ではないかもしれない
                NNS_G3dGeBufferOP_N(G3OP_MTX_STORE,
                                    &idxMtxDest,
                                    G3OP_MTX_STORE_NPARAMS);
            }
        }
    }

    rs->c += cmdLen;
}


/*---------------------------------------------------------------------------*
    NNSi_G3dFuncSbc_BBY([000], [001], [010], [011])

    mnemonic:   NNS_G3D_SBC_BBY
    operands:   [000]: idxNode
                [001]: idxNode, idxMtxDest
                [010]: idxNode, idxMtxSrc
                [011]: idxNode, idxMtxDest, idxMtxSrc
                0 <= idxMtxSrc, idxMtxDest <= 30
    callbacks:  A: before
                B: during
                C: after

    動作説明
    ・ [010], [011]ならば、idxMtxSrcをカレント行列にリストアする
    ・ TIMING_Aのコールバックを呼ぶ
    ・ 射影行列をプッシュし、単位行列をセット。
    ・ ジオメトリエンジンが止まるまで待ち、カレント行列を取り出す。
    ・ CPUでY軸ビルボード行列になるよう加工し、カレント行列に格納
    ・ TIMING_Bのコールバックを呼ぶ
    ・ [001], [011]ならば、idxMtxDestにカレント行列をストアする
    ・ TIMING_Cのコールバックを呼ぶ
    ・ rs->cの加算

    付記
 *---------------------------------------------------------------------------*/
void
NNSi_G3dFuncSbc_BBY(NNSG3dRS* rs, u32 opt)
{
    u32 cmdLen = 2;
    MtxFx44 m;

    static u32 bbcmd1[] =
    {
        GX_PACK_OP(G3OP_MTX_POP, G3OP_MTX_MODE, G3OP_MTX_LOAD_4x3, G3OP_MTX_SCALE),
        1,
        GX_MTXMODE_POSITION_VECTOR,
        FX32_ONE, 0, 0, // ここは可変(4x3Mtx)
        0, FX32_ONE, 0,
        0, 0, FX32_ONE,
        0, 0, 0,   
        0, 0, 0    // ここは可変(Scale)
    };
    VecFx32* trans = (VecFx32*)&bbcmd1[12];
    VecFx32* scale = (VecFx32*)&bbcmd1[15];
    MtxFx43* mtx   = (MtxFx43*)&bbcmd1[3];
    BOOL cbFlag;
    NNSG3dSbcCallBackTiming cbTiming;

    NNS_G3D_NULL_ASSERT(rs);

    if (rs->flag & NNS_G3D_RSFLAG_OPT_SKIP_SBCDRAW)
    {
        // ビルボードはDrawカテゴリに入る
        if (opt == NNS_G3D_SBCFLG_010 ||
            opt == NNS_G3D_SBCFLG_011)
        {
            ++cmdLen;
        }
        if (opt == NNS_G3D_SBCFLG_001 ||
            opt == NNS_G3D_SBCFLG_011)
        {
            ++cmdLen;
        }
        rs->c += cmdLen;
        return;
    }

    {
        if (opt == NNS_G3D_SBCFLG_010 ||
            opt == NNS_G3D_SBCFLG_011)
        {
            ++cmdLen;
            if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD))
            {
                u32 idxMtxSrc;
                if (opt == NNS_G3D_SBCFLG_010)
                {
                    idxMtxSrc = *(rs->c + 2);
                }
                else
                {
                    idxMtxSrc = *(rs->c + 3);
                }
                NNS_G3D_ASSERT(idxMtxSrc < 31);
                NNS_G3dGeBufferOP_N(G3OP_MTX_RESTORE,
                                    &idxMtxSrc,
                                    1);
            }
        }
    }

    // コールバックＡ
    // 内部でNNS_G3D_RSFLAG_SKIPをセットした場合、
    // ビルボード行列の設定動作を置き換えることができる
    cbFlag = NNSi_G3dCallBackCheck_A(rs, NNS_G3D_SBC_BBY, &cbTiming);

    if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD) &&
        (!cbFlag))
    {
        // バッファのフラッシュ
        NNS_G3dGeFlushBuffer();

        //
        // 次のコールバックポイントまでイミディエイト送信可能.
        //

        // コマンド転送:
        // PROJモードに変更
        // 射影行列を退避
        // 単位行列をセット
        reg_G3X_GXFIFO = GX_PACK_OP(G3OP_MTX_MODE, G3OP_MTX_PUSH, G3OP_MTX_IDENTITY, G3OP_NOP);
        reg_G3X_GXFIFO = (u32)GX_MTXMODE_PROJECTION;
        reg_G3X_GXFIFO = 0; // 2004/08/26 geometry fifo glitch

        // 更にジオメトリエンジンの停止を待つ
        // カレント行列(クリップ行列)の取得
        while (G3X_GetClipMtx(&m))
            ;

        if (NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_FLUSH_WVP)
        {
            // Projection行列にカメラ行列がかかっている場合は、
            // 改めてSRT付カメラ行列を後ろからかけなければならない。
            const MtxFx43* cam = NNS_G3dGlbGetSrtCameraMtx();
            MtxFx44 tmp;

            MTX_Copy43To44(cam, &tmp);
            MTX_Concat44(&m, &tmp, &m);
        }
        else if (NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_FLUSH_VP)
        {
            // Projection行列にカメラ行列がかかっている場合は、
            // 改めてカメラ行列を後ろからかけなければならない。
            const MtxFx43* cam = NNS_G3dGlbGetCameraMtx();
            MtxFx44 tmp;

            MTX_Copy43To44(cam, &tmp);
            MTX_Concat44(&m, &tmp, &m);
        }

        // ビルボード行列の計算

        // 1: translationは流用
        trans->x = m._30;
        trans->y = m._31;
        trans->z = m._32;

        // 2: scaleは各行のベクトルの大きさで近似
        scale->x = VEC_Mag((VecFx32*)&m._00);
        scale->y = VEC_Mag((VecFx32*)&m._10);
        scale->z = VEC_Mag((VecFx32*)&m._20);

        // 3: 回転行列は結果としてX軸に対する回転行列になるので
        //    それらしい値を設定する
        if (m._11 != 0 || m._12 != 0)
        {
            VEC_Normalize((VecFx32*)&m._10, (VecFx32*)&mtx->_10);

            mtx->_21 = -mtx->_12;
            mtx->_22 = mtx->_11;
        }
        else
        {
            VEC_Normalize((VecFx32*)&m._20, (VecFx32*)&mtx->_20);
            
            mtx->_12 = -mtx->_21;
            mtx->_11 = mtx->_22;
        }

        if (NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_FLUSH_WVP)
        {
            // 射影行列のPOP
            // POS_VECに復帰
            // SRTつきカメラの逆行列をセット
            reg_G3X_GXFIFO = GX_PACK_OP(G3OP_MTX_POP, G3OP_MTX_MODE, G3OP_MTX_LOAD_4x3, G3OP_NOP);
            MI_CpuSend32(&bbcmd1[1],
                         &reg_G3X_GXFIFO,
                         2 * sizeof(u32));
            MI_CpuSend32(NNS_G3dGlbGetInvSrtCameraMtx(),
                         &reg_G3X_GXFIFO,
                         G3OP_MTX_LOAD_4x3_NPARAMS * sizeof(u32));
            
            // カレント行列にかける
            // 計算したスケールをかける        
            reg_G3X_GXFIFO = GX_PACK_OP(G3OP_MTX_MULT_4x3, G3OP_MTX_SCALE, G3OP_NOP, G3OP_NOP);
            MI_CpuSend32(&bbcmd1[3],
                         &reg_G3X_GXFIFO,
                         sizeof(MtxFx43) + sizeof(VecFx32));
        }
        else if (NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_FLUSH_VP)
        {
            // 射影行列のPOP
            // POS_VECに復帰
            // カメラの逆行列をセット
            reg_G3X_GXFIFO = GX_PACK_OP(G3OP_MTX_POP, G3OP_MTX_MODE, G3OP_MTX_LOAD_4x3, G3OP_NOP);
            MI_CpuSend32(&bbcmd1[1],
                         &reg_G3X_GXFIFO,
                         2 * sizeof(u32));
            MI_CpuSend32(NNS_G3dGlbGetInvCameraMtx(),
                         &reg_G3X_GXFIFO,
                         G3OP_MTX_LOAD_4x3_NPARAMS * sizeof(u32));
            
            // カレント行列にかける
            // 計算したスケールをかける        
            reg_G3X_GXFIFO = GX_PACK_OP(G3OP_MTX_MULT_4x3, G3OP_MTX_SCALE, G3OP_NOP, G3OP_NOP);
            MI_CpuSend32(&bbcmd1[3],
                         &reg_G3X_GXFIFO,
                         sizeof(MtxFx43) + sizeof(VecFx32));
        }
        else
        {
            // 射影行列のPOP
            // POS_VECに復帰
            // カレント行列に格納
            // 計算したスケールをかける
            MI_CpuSend32(&bbcmd1[0], &reg_G3X_GXFIFO, 18 * sizeof(u32));
        }
    }

    // コールバックＣ
    // 次の命令の前に何らかの処理を挿入することができる。
    // また、NNS_G3D_RSFLAG_SKIPをオンにすることによって
    // カレント行列のストア動作を置き換えることができる
    cbFlag = NNSi_G3dCallBackCheck_C(rs, NNS_G3D_SBC_BBY, cbTiming);

    if (opt == NNS_G3D_SBCFLG_001 ||
        opt == NNS_G3D_SBCFLG_011)
    {
        ++cmdLen;

        if (!cbFlag)
        {
            if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD))
            {
                u32 idxMtxDest;
                idxMtxDest = *(rs->c + 2);

                NNS_G3D_ASSERT(idxMtxDest < 31);
                // ここではイミディエイト送信可能ではないかもしれない
                NNS_G3dGeBufferOP_N(G3OP_MTX_STORE,
                                    &idxMtxDest,
                                    G3OP_MTX_STORE_NPARAMS);
            }
        }
    }

    rs->c += cmdLen;
}


/*---------------------------------------------------------------------------*
    NNSi_G3dFuncSbc_NODEMIX

    mnemonic:   NNS_G3D_SBC_NODEMIX
    operands:   idxMtxDest, numMtx, idxStack1, idxMtx1, ratio1, ... ,
                                    idxStackN, idxMtxN, ratioN
    callbacks:  none

    動作説明
    ・sum(ratio * InvM(InvN) * AnmM)を計算して、行列スタックのidxMtxDestに格納します。
      つまり、頂点を逆行列(InvM, InvN)によって各ジョイントのローカル座標に戻し
      アニメーション行列をかけるという操作を行う行列をブレンドしています。
    ・モデリング変換行列の逆行列(InvM, InvN)はモデルリソースに格納されています。
      g3dcvtrによるコンバートの際に計算されています。
    ・行列の乗算はジオメトリエンジンを利用して行っています。
    ・ウェイテッドエンベロープがかかっている頂点はグローバル座標で格納されています。

    付記
 *---------------------------------------------------------------------------*/
void
NNSi_G3dFuncSbc_NODEMIX(NNSG3dRS* rs, u32)
{
    fx64 w = 0;
    const NNSG3dResEvpMtx* evpMtx =
        (const NNSG3dResEvpMtx*)((u8*)rs->pRenderObj->resMdl +
                                    rs->pRenderObj->resMdl->ofsEvpMtx);
    u32 numMtx = *(rs->c + 2);
    u8* p = rs->c + 3;

    NNS_G3D_ASSERT(numMtx >= 2);

#if (NNS_G3D_USE_EVPCACHE)
    {
        u32 i;
        struct
        {
            MtxFx43 M;
            MtxFx33 N;
        } sum;
        MtxFx44* pX;
        MtxFx33* pY;

        MI_CpuClearFast(&sum, sizeof(sum));
        NNS_G3dGeFlushBuffer();

        G3_MtxMode(GX_MTXMODE_PROJECTION);
        G3_StoreMtx(1);
        G3_Identity();
        G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);

        for (i = 0; i < numMtx; ++i)
        {
            u32 idxJnt = *(p + 1);
            BOOL evpCached = NNSi_G3dBitVecCheck(&rs->isEvpCached[0], idxJnt);

            pX = &NNS_G3dRSOnGlb.evpCache[idxJnt].M;
            if (!evpCached)
            {
                NNSi_G3dBitVecSet(&rs->isEvpCached[0], idxJnt);

                G3_RestoreMtx(*p);
                G3_MtxMode(GX_MTXMODE_POSITION);
                G3_MultMtx43(&evpMtx[idxJnt].invM);
            }

            if (i != 0)
            {
                sum.N.m[0][0] += (w * pY->m[0][0]) >> FX32_SHIFT;
                sum.N.m[0][1] += (w * pY->m[0][1]) >> FX32_SHIFT;
                sum.N.m[0][2] += (w * pY->m[0][2]) >> FX32_SHIFT;

                sum.N.m[1][0] += (w * pY->m[1][0]) >> FX32_SHIFT;
                sum.N.m[1][1] += (w * pY->m[1][1]) >> FX32_SHIFT;
                sum.N.m[1][2] += (w * pY->m[1][2]) >> FX32_SHIFT;

                sum.N.m[2][0] += (w * pY->m[2][0]) >> FX32_SHIFT;
                sum.N.m[2][1] += (w * pY->m[2][1]) >> FX32_SHIFT;
                sum.N.m[2][2] += (w * pY->m[2][2]) >> FX32_SHIFT;
            }

            if (!evpCached)
            {
                while (G3X_GetClipMtx(pX))
                    ;
                G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
                G3_MultMtx33(&evpMtx[idxJnt].invN);
            }

            w = *(p + 2) << 4;

            sum.M.m[0][0] += (w * pX->m[0][0]) >> FX32_SHIFT;
            sum.M.m[0][1] += (w * pX->m[0][1]) >> FX32_SHIFT;
            sum.M.m[0][2] += (w * pX->m[0][2]) >> FX32_SHIFT;

            sum.M.m[1][0] += (w * pX->m[1][0]) >> FX32_SHIFT;
            sum.M.m[1][1] += (w * pX->m[1][1]) >> FX32_SHIFT;
            sum.M.m[1][2] += (w * pX->m[1][2]) >> FX32_SHIFT;

            sum.M.m[2][0] += (w * pX->m[2][0]) >> FX32_SHIFT;
            sum.M.m[2][1] += (w * pX->m[2][1]) >> FX32_SHIFT;
            sum.M.m[2][2] += (w * pX->m[2][2]) >> FX32_SHIFT;

            sum.M.m[3][0] += (w * pX->m[3][0]) >> FX32_SHIFT;
            sum.M.m[3][1] += (w * pX->m[3][1]) >> FX32_SHIFT;
            sum.M.m[3][2] += (w * pX->m[3][2]) >> FX32_SHIFT;

            p += 3;
            pY = &NNS_G3dRSOnGlb.evpCache[idxJnt].N;

            if (!evpCached)
            {
                while (G3X_GetVectorMtx(pY))
                    ;
            }
        }
        sum.N.m[0][0] += (w * pY->m[0][0]) >> FX32_SHIFT;
        sum.N.m[0][1] += (w * pY->m[0][1]) >> FX32_SHIFT;
        sum.N.m[0][2] += (w * pY->m[0][2]) >> FX32_SHIFT;

        sum.N.m[1][0] += (w * pY->m[1][0]) >> FX32_SHIFT;
        sum.N.m[1][1] += (w * pY->m[1][1]) >> FX32_SHIFT;
        sum.N.m[1][2] += (w * pY->m[1][2]) >> FX32_SHIFT;

        sum.N.m[2][0] += (w * pY->m[2][0]) >> FX32_SHIFT;
        sum.N.m[2][1] += (w * pY->m[2][1]) >> FX32_SHIFT;
        sum.N.m[2][2] += (w * pY->m[2][2]) >> FX32_SHIFT;

        G3_LoadMtx43((const MtxFx43*)&sum.N); // 上書きされるのでゴミデータが混じってもよい
        G3_MtxMode(GX_MTXMODE_POSITION);
        G3_LoadMtx43(&sum.M);
        G3_MtxMode(GX_MTXMODE_PROJECTION);
        G3_RestoreMtx(1);
        G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    }
#else
    {
        u32 i;
        struct
        {
            MtxFx43 M;
            MtxFx33 N;
        } sum;
        MtxFx44 X;
        MtxFx33 Y;

        MI_CpuClearFast(&sum, sizeof(sum));
        NNS_G3dGeFlushBuffer();

        G3_MtxMode(GX_MTXMODE_PROJECTION);
        G3_StoreMtx(1);
        G3_Identity();
        G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);

        for (i = 0; i < numMtx; ++i)
        {
            u32 idxJnt = *(p + 1);
            
            G3_RestoreMtx(*p);
            G3_MtxMode(GX_MTXMODE_POSITION);
            G3_MultMtx43(&evpMtx[idxJnt].invM);

            if (i != 0)
            {
                sum.N.m[0][0] += (w * Y.m[0][0]) >> FX32_SHIFT;
                sum.N.m[0][1] += (w * Y.m[0][1]) >> FX32_SHIFT;
                sum.N.m[0][2] += (w * Y.m[0][2]) >> FX32_SHIFT;

                sum.N.m[1][0] += (w * Y.m[1][0]) >> FX32_SHIFT;
                sum.N.m[1][1] += (w * Y.m[1][1]) >> FX32_SHIFT;
                sum.N.m[1][2] += (w * Y.m[1][2]) >> FX32_SHIFT;

                sum.N.m[2][0] += (w * Y.m[2][0]) >> FX32_SHIFT;
                sum.N.m[2][1] += (w * Y.m[2][1]) >> FX32_SHIFT;
                sum.N.m[2][2] += (w * Y.m[2][2]) >> FX32_SHIFT;
            }

            while (G3X_GetClipMtx(&X))
                ;
            G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
            G3_MultMtx33(&evpMtx[idxJnt].invN);

            w = *(p + 2) << 4;
            sum.M.m[0][0] += (w * X.m[0][0]) >> FX32_SHIFT;
            sum.M.m[0][1] += (w * X.m[0][1]) >> FX32_SHIFT;
            sum.M.m[0][2] += (w * X.m[0][2]) >> FX32_SHIFT;

            sum.M.m[1][0] += (w * X.m[1][0]) >> FX32_SHIFT;
            sum.M.m[1][1] += (w * X.m[1][1]) >> FX32_SHIFT;
            sum.M.m[1][2] += (w * X.m[1][2]) >> FX32_SHIFT;

            sum.M.m[2][0] += (w * X.m[2][0]) >> FX32_SHIFT;
            sum.M.m[2][1] += (w * X.m[2][1]) >> FX32_SHIFT;
            sum.M.m[2][2] += (w * X.m[2][2]) >> FX32_SHIFT;

            sum.M.m[3][0] += (w * X.m[3][0]) >> FX32_SHIFT;
            sum.M.m[3][1] += (w * X.m[3][1]) >> FX32_SHIFT;
            sum.M.m[3][2] += (w * X.m[3][2]) >> FX32_SHIFT;
            p += 3;

            while (G3X_GetVectorMtx(&Y))
                ;
        }
        sum.N.m[0][0] += (w * Y.m[0][0]) >> FX32_SHIFT;
        sum.N.m[0][1] += (w * Y.m[0][1]) >> FX32_SHIFT;
        sum.N.m[0][2] += (w * Y.m[0][2]) >> FX32_SHIFT;

        sum.N.m[1][0] += (w * Y.m[1][0]) >> FX32_SHIFT;
        sum.N.m[1][1] += (w * Y.m[1][1]) >> FX32_SHIFT;
        sum.N.m[1][2] += (w * Y.m[1][2]) >> FX32_SHIFT;

        sum.N.m[2][0] += (w * Y.m[2][0]) >> FX32_SHIFT;
        sum.N.m[2][1] += (w * Y.m[2][1]) >> FX32_SHIFT;
        sum.N.m[2][2] += (w * Y.m[2][2]) >> FX32_SHIFT;

        G3_LoadMtx43((const MtxFx43*)&sum.N); // 上書きされるのでゴミデータが混じってもよい
        G3_MtxMode(GX_MTXMODE_POSITION);
        G3_LoadMtx43(&sum.M);
        G3_MtxMode(GX_MTXMODE_PROJECTION);
        G3_RestoreMtx(1);
        G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    }
#endif
    G3_StoreMtx(*(rs->c + 1));
    rs->c += 3 + *(rs->c + 2) * 3;
}


/*---------------------------------------------------------------------------*
    NNSi_G3dFuncSbc_CALLDL

    mnemonic:   NNS_G3D_SBC_CALLDL
    operands:   rel_addr(1 word), size(1 word)
    callbacks:  A: before
                B: none
                C: after
    動作説明
    ・ TIMING_Aのコールバックを呼ぶ
    ・ ディスプレイリストの転送を行う
    ・ TIMING_Cのコールバックを呼ぶ
    ・ rs->cの加算

    付記
    最初にNODEDESCで一通り行列を設定して、CALLDLを呼ぶことで単純なモデルを高速に
    描画するようなコンバート方法を実装した場合に、恐らくこの命令が必要になる。
    ユーザーによるハックのための手段の１つでもある。
 *---------------------------------------------------------------------------*/
void
NNSi_G3dFuncSbc_CALLDL(NNSG3dRS* rs, u32)
{
    BOOL cbFlag;
    NNSG3dSbcCallBackTiming cbTiming;

    NNS_G3D_NULL_ASSERT(rs);

    // コールバックＡ
    // 内部でNNS_G3D_RSFLAG_SKIPをセットした場合、
    // ディスプレイリストの送信動作を置き換えることができる。
    cbFlag = NNSi_G3dCallBackCheck_A(rs, NNS_G3D_SBC_CALLDL, &cbTiming);

    if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD) &&
        (!cbFlag))
    {
        u32 rel_addr;
        u32 size;
        rel_addr = (u32)( (*(rs->c + 1) << 0 ) |
                          (*(rs->c + 2) << 8 ) |
                          (*(rs->c + 3) << 16) |
                          (*(rs->c + 4) << 24) );

        size = (u32)( (*(rs->c + 5) << 0 ) |
                      (*(rs->c + 6) << 8 ) |
                      (*(rs->c + 7) << 16) |
                      (*(rs->c + 8) << 24) );

        NNS_G3dGeSendDL(rs->c + rel_addr, size);
    }

    // コールバックＣ
    // 次の命令の前に何らかの処理を挿入することができる。
    (void) NNSi_G3dCallBackCheck_C(rs, NNS_G3D_SBC_CALLDL, cbTiming);

    rs->c += 1 + sizeof(u32) + sizeof(u32);
}


/*---------------------------------------------------------------------------*
    NNSi_G3dFuncSbc_POSSCALE([000], [001])

    mnemonic:   NNS_G3D_SBC_POSSCALE
    operands:   none
    callbacks:  A: none
                B: none
                C: none

    動作説明
    ・ カレント行列にスケーリング行列をかける
       [000] モデルデータ内のposScaleを要素とするスケーリング行列をかける
       [001] モデルデータ内のinvPosScaleを要素とするスケーリング行列をかける
    ・ rs->cの加算

    付記
    imdファイルのpos_scaleが0でない場合、このコマンドが出力されている。
    描画の直前に配置される必要がある。
    ユーザーから意識できる必要はないと思われるのでコールバックを置かない。
 *---------------------------------------------------------------------------*/
void
NNSi_G3dFuncSbc_POSSCALE(NNSG3dRS* rs, u32 opt)
{
    VecFx32 s;
    NNS_G3D_NULL_ASSERT(rs);
    NNS_G3D_ASSERT(opt == NNS_G3D_SBCFLG_000 ||
                   opt == NNS_G3D_SBCFLG_001);

    if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD) &&
        !(rs->flag & NNS_G3D_RSFLAG_OPT_SKIP_SBCDRAW))
    {
        if (opt == NNS_G3D_SBCFLG_000)
        {
            s.x = s.y = s.z = rs->posScale;
        }
        else
        {
            // NNS_G3D_SBCFLG_001
            s.x = s.y = s.z = rs->invPosScale;
        }
        NNS_G3dGeBufferOP_N(G3OP_MTX_SCALE,
                            (u32*)&s.x,
                            G3OP_MTX_SCALE_NPARAMS);
    }

    rs->c += 1;
}


/*---------------------------------------------------------------------------*
    NNSi_G3dFuncSbc_ENVMAP

    mnemonic:   NNS_G3D_SBC_ENVMAP
    operands:   idxMat, flags(予約分)
    callbacks:  A: 法線->テクスチャ座標マッピング行列のカスタマイズ
                B: effect_mtx設定のカスタマイズ
                C: 法線変換のカスタマイズ

    動作説明
    nrm * (C: 法線の変換行列) * (B: .imdに設定されたエフェクト行列) * 
          (A: マッピング行列) * (materialで設定されたテクスチャ行列)
    という計算を行います。A, B, Cの値は、コールバックを設定することにより、
    カスタマイズすることができます。デフォルトの状態では、表面と裏面にそれぞれ
    １枚分のテクスチャイメージを貼り付けるようなマッピングを行います。
 *---------------------------------------------------------------------------*/
void
NNSi_G3dFuncSbc_ENVMAP(NNSG3dRS* rs, u32)
{
    NNS_G3D_NULL_ASSERT(rs);

    // VISIBLEでない場合は命令をスキップ
    // 同じMatID状態でもノードが違えば計算しなおす必要がある
    if (!(rs->flag & NNS_G3D_RSFLAG_OPT_SKIP_SBCDRAW) &&
        (rs->flag & NNS_G3D_RSFLAG_NODE_VISIBLE))
    {
        BOOL cbFlag;
        NNSG3dSbcCallBackTiming cbTiming;

        // テクスチャSRTアニメを付けている場合、
        // TEXGENがGX_TEXGEN_TEXCOORDになってしまうので、
        // GX_TEXGEN_NORMALに設定しなおす。
        if ((rs->pMatAnmResult->prmTexImage & REG_G3_TEXIMAGE_PARAM_TGEN_MASK) !=
                (GX_TEXGEN_NORMAL << REG_G3_TEXIMAGE_PARAM_TGEN_SHIFT))
        {
            static u32 cmd[] =
            {
                G3OP_TEXIMAGE_PARAM,
                0
            };
            rs->pMatAnmResult->prmTexImage &= ~REG_G3_TEXIMAGE_PARAM_TGEN_MASK;
            rs->pMatAnmResult->prmTexImage |= GX_TEXGEN_NORMAL << REG_G3_TEXIMAGE_PARAM_TGEN_SHIFT;

            cmd[1] = rs->pMatAnmResult->prmTexImage;
            NNS_G3dGeBufferData_N(&cmd[0], 2);
        }

        // テクスチャ行列を操作対象にする。
        NNS_G3dGeMtxMode(GX_MTXMODE_TEXTURE);

        // コールバックＡ
        // 内部でNNS_G3D_RSFLAG_SKIPをセットした場合、
        // qMtxの設定を置き換えることができる。
        cbFlag = NNSi_G3dCallBackCheck_A(rs, NNS_G3D_SBC_ENVMAP, &cbTiming);
        if (!cbFlag)
        {
            // callback Aでカスタマイズできる処理
            s32 width, height;
            width = (s32)rs->pMatAnmResult->origWidth;
            height =(s32)rs->pMatAnmResult->origHeight;

            // NOTICE:
            // 変換後のS,T座標がqで割られないためParaboloid Mappingを実装することはできない。
            {
                //     0.5   0   0   0
                // m =  0  -0.5  0   0
                //      0    0   1   0
                //     0.5  0.5  0   1

                // 表面と裏面に１枚ずつテクスチャをはるようなマッピング
                NNS_G3dGeScale(width << (FX32_SHIFT + 3), -height << (FX32_SHIFT + 3), FX32_ONE << 4);
                NNS_G3dGeTexCoord(width << (FX32_SHIFT - 1), height << (FX32_SHIFT - 1));
            }
        }

        // コールバックＢ
        // 内部でNNS_G3D_RSFLAG_SKIPをセットした場合、
        // effect_mtxの設定を置き換えることができる。
        cbFlag = NNSi_G3dCallBackCheck_B(rs, NNS_G3D_SBC_ENVMAP, &cbTiming);
        if (!cbFlag)
        {
            u32 idxMat = *(rs->c + 1);
            const NNSG3dResMatData* mat =
                NNS_G3dGetMatDataByIdx(rs->pResMat, idxMat);

            // NITRO中間ファイル内にeffect_mtxが存在している場合は、
            // effect_mtxをかける
            if (mat->flag & NNS_G3D_MATFLAG_EFFECTMTX)
            {
                // NITRO中間ファイル内にeffect_mtxが存在している場合
                const MtxFx44* effect_mtx;
                const u8* p = (const u8*)mat + sizeof(NNSG3dResMatData);

                if (!(mat->flag & NNS_G3D_MATFLAG_TEXMTX_SCALEONE))
                {
                    p += sizeof(fx32) + sizeof(fx32); // scaleS, scaleT
                }

                if (!(mat->flag & NNS_G3D_MATFLAG_TEXMTX_ROTZERO))
                {
                    p += sizeof(fx16) + sizeof(fx16); // rotSin, rotCos
                }

                if (!(mat->flag & NNS_G3D_MATFLAG_TEXMTX_TRANSZERO))
                {
                    p += sizeof(fx32) + sizeof(fx32); // transS, transT
                }

                effect_mtx = (const MtxFx44*)p;
                NNS_G3dGeMultMtx44(effect_mtx);
            }
        }

        // コールバックＣ
        // 内部でNNS_G3D_RSFLAG_SKIPをセットした場合、
        // 入力法線ベクトルの変換行列を置き換えることができる。
        cbFlag = NNSi_G3dCallBackCheck_C(rs, NNS_G3D_SBC_ENVMAP, cbTiming);
        if (!cbFlag)
        {
            MtxFx33 n;
            NNS_G3dGeMtxMode(GX_MTXMODE_POSITION_VECTOR);
            NNS_G3dGetCurrentMtx(NULL, &n);
            NNS_G3dGeMtxMode(GX_MTXMODE_TEXTURE);

            //
            // 入力法線をワールド座標系の方向に変換する。
            // なお、入力される法線ベクトルは、ジオメトリコマンドの方向と同じ(ジョイント座標系)。
            //
            if (NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_FLUSH_WVP)
            {
                // NNS_G3dGlbFlushWVPのときは、
                // ジョイント座標系->ローカル座標系->ビュー座標系
                // の変換を行う。
                NNS_G3dGeMultMtx33((const MtxFx33*)NNS_G3dGlbGetCameraMtx()); // ワールド座標系 -> ビュー座標系
                NNS_G3dGeMultMtx33(NNS_G3dGlbGetBaseRot());                   // ローカル座標系 -> ワールド座標系
                NNS_G3dGeMultMtx33(&n);                                       // ジョイント座標系 -> ローカル座標系

            }
            else if (NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_FLUSH_VP)
            {
                // NNS_G3dGlbFlushVPのときは
                // そのままでワールド座標系の方向ベクトルになる。
                NNS_G3dGeMultMtx33((const MtxFx33*)NNS_G3dGlbGetCameraMtx()); // ワールド座標系 -> ビュー座標系
                NNS_G3dGeMultMtx33(&n);                                       // ジョイント座標系 -> ワールド座標系
            }
            else
            {
                NNS_G3dGeMultMtx33(&n);                            // ジョイント座標系 -> ビュー座標系
            }
        }

        // 操作対象行列を元に戻す
        NNS_G3dGeMtxMode(GX_MTXMODE_POSITION_VECTOR);
    }
    rs->c += 3;
}


/*---------------------------------------------------------------------------*
    NNSi_G3dFuncSbc_PRJMAP

    mnemonic:   NNS_G3D_SBC_PRJMAP([000], [001], [010])
    operands:   idxMat
    callbacks:  A: 頂点座標->テクスチャ座標マッピング行列のカスタマイズ
                B: effect_mtx設定のカスタマイズ
                C: 頂点座標変換のカスタマイズ

    動作説明
    計算したテクスチャ行列から値を取り出して、TexCoordに設定する必要があるので、
    取り出すのが可能な位置座標行列を用いて計算を行います。
    その後取り出して、テクスチャ行列とTexCoordに設定します。
    なお、MATコマンドで設定したテクスチャ行列は上書きされるので無効になります。
 *---------------------------------------------------------------------------*/
void
NNSi_G3dFuncSbc_PRJMAP(NNSG3dRS* rs, u32 )
{
    //
    // テクスチャ行列の一部をTexCoordを設定する必要があるので、
    // NNS_G3D_MTXSTACK_SYSを使用して計算してから取り出す。
    //

    NNS_G3D_NULL_ASSERT(rs);

    // VISIBLEでない場合は命令をスキップ
    // 同じMatID状態でもノードが違えば計算しなおす必要がある
    if (!(rs->flag & NNS_G3D_RSFLAG_OPT_SKIP_SBCDRAW) &&
        (rs->flag & NNS_G3D_RSFLAG_NODE_VISIBLE))
    {
        BOOL cbFlag;
        NNSG3dSbcCallBackTiming cbTiming;

        // 後で使用するカレント位置座標行列を取り出す。
        MtxFx43 m;
        NNS_G3dGetCurrentMtx(&m, NULL);
        NNS_G3dGeStoreMtx(NNS_G3D_MTXSTACK_SYS);

        // テクスチャSRTアニメを付けている場合、
        // TEXGENがGX_TEXGEN_TEXCOORDになってしまうので、
        // GX_TEXGEN_VERTEXに設定しなおす。
        if ((rs->pMatAnmResult->prmTexImage & REG_G3_TEXIMAGE_PARAM_TGEN_MASK) !=
                (GX_TEXGEN_VERTEX << REG_G3_TEXIMAGE_PARAM_TGEN_SHIFT))
        {
            static u32 cmd[] =
            {
                G3OP_TEXIMAGE_PARAM,
                0
            };
            rs->pMatAnmResult->prmTexImage &= ~REG_G3_TEXIMAGE_PARAM_TGEN_MASK;
            rs->pMatAnmResult->prmTexImage |= GX_TEXGEN_VERTEX << REG_G3_TEXIMAGE_PARAM_TGEN_SHIFT;

            cmd[1] = rs->pMatAnmResult->prmTexImage;
            NNS_G3dGeBufferData_N(&cmd[0], 2);
        }

        // コールバックＡ
        // 内部でNNS_G3D_RSFLAG_SKIPをセットした場合、
        // qMtxの設定を置き換えることができる。
        cbFlag = NNSi_G3dCallBackCheck_A(rs, NNS_G3D_SBC_PRJMAP, &cbTiming);
        if (!cbFlag)
        {
            // callback Aでカスタマイズできる処理
            s32 width, height;
            width = (s32)rs->pMatAnmResult->origWidth;
            height = (s32)rs->pMatAnmResult->origHeight;

            {
                static MtxFx44 mtx = {
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, FX32_ONE << 4, 0,
                    0, 0, 0, FX32_ONE << 4
                };

                mtx._00 = width << (FX32_SHIFT + 3);
                mtx._11 = -height << (FX32_SHIFT + 3);
                mtx._30 = width << (FX32_SHIFT + 3);
                mtx._31 = height << (FX32_SHIFT + 3);

                //        0.5   0   0   0
                // mtx =   0  -0.5  0   0
                //         0    0   1   0
                //        0.5  0.5  0   1
                NNS_G3dGeLoadMtx44(&mtx);
            }
        }

        // コールバックＢ
        // 内部でNNS_G3D_RSFLAG_SKIPをセットした場合、
        // effect_mtxの設定を置き換えることができる。
        cbFlag = NNSi_G3dCallBackCheck_B(rs, NNS_G3D_SBC_PRJMAP, &cbTiming);
        if (!cbFlag)
        {
            u32 idxMat = *(rs->c + 1);
            const NNSG3dResMatData* mat =
                NNS_G3dGetMatDataByIdx(rs->pResMat, idxMat);

            // NITRO中間ファイル内にeffect_mtxが存在している場合は、
            // effect_mtxをかける
            if (mat->flag & NNS_G3D_MATFLAG_EFFECTMTX)
            {
                // NITRO中間ファイル内にeffect_mtxが存在している場合
                const MtxFx44* effect_mtx;
                const u8* p = (const u8*)mat + sizeof(NNSG3dResMatData);

                if (!(mat->flag & NNS_G3D_MATFLAG_TEXMTX_SCALEONE))
                {
                    p += sizeof(fx32) + sizeof(fx32); // scaleS, scaleT
                }

                if (!(mat->flag & NNS_G3D_MATFLAG_TEXMTX_ROTZERO))
                {
                    p += sizeof(fx16) + sizeof(fx16); // rotSin, rotCos
                }

                if (!(mat->flag & NNS_G3D_MATFLAG_TEXMTX_TRANSZERO))
                {
                    p += sizeof(fx32) + sizeof(fx32); // transS, transT
                }

                effect_mtx = (const MtxFx44*)p;
                NNS_G3dGeMultMtx44(effect_mtx);
            }
        }

        // コールバックＣ
        // 内部でNNS_G3D_RSFLAG_SKIPをセットした場合、
        // 入力法線ベクトルの変換行列を置き換えることができる。
        cbFlag = NNSi_G3dCallBackCheck_C(rs, NNS_G3D_SBC_PRJMAP, cbTiming);
        if (!cbFlag)
        {
            MtxFx44 tex_mtx;

            //
            // 入力座標をワールド座標系の座標に変換する。
            // なお、入力される法線ベクトルは、ジオメトリコマンドの方向と同じ(ジョイント座標系)。
            //
            if (NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_FLUSH_WVP)
            {
                // NNS_G3dGlbFlushWVPのときは、
                // ジョイント座標系->ローカル座標系->ワールド座標系
                // の変換を行う。
                NNS_G3dGeTranslateVec(NNS_G3dGlbGetBaseTrans());
                NNS_G3dGeMultMtx33(NNS_G3dGlbGetBaseRot());  // ローカル座標系 -> ワールド座標系
                NNS_G3dGeMultMtx43(&m);                      // ジョイント座標系 -> ローカル座標系

            }
            else if (NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_FLUSH_VP)
            {
                // NNS_G3dGlbFlushVPのときは
                // そのままでワールド座標系の方向ベクトルになる。
                NNS_G3dGeMultMtx43(&m);                      // ジョイント座標系 -> ワールド座標系
            }
            else
            {
                // NNS_G3dGlbFlushPのときは、
                // ジョイント座標系->カメラ座標系->ワールド座標系
                // の変換を行う。
                NNS_G3dGeMultMtx43(NNS_G3dGlbGetInvV()); // カメラ座標系 -> ワールド座標系
                NNS_G3dGeMultMtx43(&m);                            // ジョイント座標系 -> カメラ座標系
            }

            //
            // 位置座標行列スタックから計算したテクスチャ行列を読み戻す
            //
            {
                NNS_G3dGeFlushBuffer();

                G3_MtxMode(GX_MTXMODE_PROJECTION);
                G3_PushMtx();
                G3_Identity();

                while(G3X_GetClipMtx(&tex_mtx))
                    ;

                G3_PopMtx(1);
                G3_MtxMode(GX_MTXMODE_TEXTURE);
            }

            NNS_G3dGeLoadMtx44(&tex_mtx);
            NNS_G3dGeTexCoord(tex_mtx._30 >> 4, tex_mtx._31 >> 4);
        }

        // 操作対象行列を元に戻す
        NNS_G3dGeMtxMode(GX_MTXMODE_POSITION_VECTOR);
        NNS_G3dGeRestoreMtx(NNS_G3D_MTXSTACK_SYS);
    }
    rs->c += 3;
}




////////////////////////////////////////////////////////////////////////////////
//
// グローバル変数
//

/*---------------------------------------------------------------------------*
    NNS_G3dRS

    レンダリング中にプログラムが保持する状態(ステートマシンのステート)を保持する
    構造体へのポインタ。構造体の領域は、NNS_G3dDrawにおいてスタック領域(DTCM)に
    確保される。NNS_G3dRSは、NNS_G3dDraw終了時にNULLにクリーンアップされる。
 *---------------------------------------------------------------------------*/
NNSG3dRS* NNS_G3dRS = NULL;


/*---------------------------------------------------------------------------*
    NNS_G3dRSOnGlb

    レンダリング中にプログラムが保持する状態(ステートマシンのステート)で、
    サイズが大きいためにスタック領域に置かないもの。
 *---------------------------------------------------------------------------*/
NNSG3dRSOnGlb NNS_G3dRSOnGlb;


/*---------------------------------------------------------------------------*
    NNS_G3dFuncSbcTable

    SBCコードのハンドラが登録されている。
    constではないのはユーザーの書き換えを可能にするためである。
 *---------------------------------------------------------------------------*/
NNSG3dFuncSbc NNS_G3dFuncSbcTable[NNS_G3D_SBC_COMMAND_NUM] =
{
    &NNSi_G3dFuncSbc_NOP,
    &NNSi_G3dFuncSbc_RET,
    &NNSi_G3dFuncSbc_NODE,
    &NNSi_G3dFuncSbc_MTX,
    &NNSi_G3dFuncSbc_MAT,
    &NNSi_G3dFuncSbc_SHP,
    &NNSi_G3dFuncSbc_NODEDESC,
    &NNSi_G3dFuncSbc_BB,

    &NNSi_G3dFuncSbc_BBY,
    &NNSi_G3dFuncSbc_NODEMIX,
    &NNSi_G3dFuncSbc_CALLDL,
    &NNSi_G3dFuncSbc_POSSCALE,
    &NNSi_G3dFuncSbc_ENVMAP,
    &NNSi_G3dFuncSbc_PRJMAP
};


/*---------------------------------------------------------------------------*
    NNS_G3dFuncSbcShpTable

    個々のシェイプデータの先頭にあるタグはこのテーブルへのインデックスになっている。
    テーブルにシェイプデータの種類別のハンドラを登録しておく。
 *---------------------------------------------------------------------------*/
NNSG3dFuncSbc_ShpInternal NNS_G3dFuncSbcShpTable[NNS_G3D_SIZE_SHP_VTBL_NUM] =
{
    &NNSi_G3dFuncSbc_SHP_InternalDefault
};


/*---------------------------------------------------------------------------*
    NNS_G3dFuncSbcMatTable

    個々のマテリアルデータの先頭にあるタグはこのテーブルへのインデックスになっている。
    テーブルにマテリアルデータの種類別のハンドラを登録しておく。
 *---------------------------------------------------------------------------*/
NNSG3dFuncSbc_MatInternal NNS_G3dFuncSbcMatTable[NNS_G3D_SIZE_MAT_VTBL_NUM] =
{
    &NNSi_G3dFuncSbc_MAT_InternalDefault
};
