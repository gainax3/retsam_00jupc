/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g3d
  File:     1mat1shp.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.10 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#include <nnsys/g3d/1mat1shp.h>
#include <nnsys/g3d/binres/res_struct_accessor.h>
#include <nnsys/g3d/gecom.h>
#include <nnsys/g3d/cgtool.h>


/*---------------------------------------------------------------------------*
    NNS_G3dDraw1Mat1Shp

    matIDとshpIDを指定して描画します。単純なモデルを描画するのための関数です。
    sendMatをFALSEにするとマテリアルの送信を省略します。
    使用方法については、1mat1shpサンプルを参照してください。
 *---------------------------------------------------------------------------*/
void
NNS_G3dDraw1Mat1Shp(const NNSG3dResMdl* pResMdl,
                    u32 matID,
                    u32 shpID,
                    BOOL sendMat)
{
    NNS_G3D_NULL_ASSERT(pResMdl);

    if (pResMdl->info.posScale != FX32_ONE)
    {
        // pos_scaleによるスケール補正が必要な場合
        NNS_G3dGeScale(pResMdl->info.posScale,
                       pResMdl->info.posScale,
                       pResMdl->info.posScale);
    }

    if (sendMat)
    {
        NNS_G3D_ASSERT(matID < pResMdl->info.numMat);
        if (matID < pResMdl->info.numMat)
        {
            //
            // マテリアルを送信する場合:
            // ・マテリアルアニメーションの再生は不可能です（モデルデータに格納されているマテリアルデータを
            //   書き換えれば同様の効果は出せます）。
            // ・NNS_G3dGlb構造体に格納されている、グローバルのマテリアル情報は反映されません。モデルデータに
            //   格納されているマテリアルデータが反映されます。
            //
            const NNSG3dResMatData* mat;
            u32 cmd[7];
            
            {
                // NNSG3dResMatDataへのポインタを取得
                const NNSG3dResMat* p = NNS_G3dGetMat(pResMdl);
                NNS_G3D_NULL_ASSERT(p);
                mat = NNS_G3dGetMatDataByIdx(p, matID);
                NNS_G3D_NULL_ASSERT(mat);
            }

            // 透明なら描画せず戻る。
            if (!(mat->polyAttr & REG_G3_POLYGON_ATTR_ALPHA_MASK))
                return;

            // G3_MaterialColorDiffAmb(mat->diffAmb);
            // G3_MaterialColorSpecEmi(mat->specEmi);
            // G3_PolygonAttr(...);
            cmd[0] = GX_PACK_OP(G3OP_DIF_AMB, G3OP_SPE_EMI, G3OP_POLYGON_ATTR, G3OP_NOP);
            cmd[1] = mat->diffAmb;
            cmd[2] = mat->specEmi;
            cmd[3] = mat->polyAttr;

            if (mat->flag & NNS_G3D_MATFLAG_WIREFRAME)
            {
                // ワイヤーフレーム表示の場合はジオメトリエンジンに送るαは0になる
                // ALPHAのビットを0にしておく。
                cmd[3] &= ~REG_G3_POLYGON_ATTR_ALPHA_MASK;
            }

            // G3_TexImageParam(...);
            // G3_TexPlttBase(...);
            cmd[4] = GX_PACK_OP(G3OP_TEXIMAGE_PARAM, G3OP_TEXPLTT_BASE, G3OP_NOP, G3OP_NOP);
            cmd[5] = mat->texImageParam;
            cmd[6] = mat->texPlttBase;

            // まとめてFIFOに送信
            NNS_G3dGeBufferData_N(&cmd[0], 7);

            if (mat->flag & NNS_G3D_MATFLAG_TEXMTX_USE)
            {
                // テクスチャ行列をセット
                NNSG3dSendTexSRT func;
                NNSG3dMatAnmResult dummy;

                // NNSG3dResMatDataの後ろにSRTデータがmat->flagの値によって入っている
                const u8* p = (const u8*)mat + sizeof(NNSG3dResMatData);
                
                dummy.flag = NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SET;
                dummy.origWidth = mat->origWidth;
                dummy.origHeight = mat->origHeight;
                dummy.magW = mat->magW;
                dummy.magH = mat->magH;

                //
                // 以下NNSG3dResMatData後部のデータ(SRT)をフラグの値を見ながら取得していく
                //

                // テクスチャ行列のScale設定
                if (!(mat->flag & NNS_G3D_MATFLAG_TEXMTX_SCALEONE))
                {
                    const fx32* p_fx32 = (const fx32*)p;

                    dummy.scaleS = *(p_fx32 + 0);
                    dummy.scaleT = *(p_fx32 + 1);
                    p += 2 * sizeof(fx32);
                }
                else
                {
                    dummy.flag |= NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SCALEONE;
                }

                // テクスチャ行列のRotation設定
                if (!(mat->flag & NNS_G3D_MATFLAG_TEXMTX_ROTZERO))
                {
                    const fx16* p_fx16 = (const fx16*)p;

                    dummy.sinR = *(p_fx16 + 0);
                    dummy.cosR = *(p_fx16 + 1);
                    p += 2 * sizeof(fx16);
                }
                else
                {
                    dummy.flag |= NNS_G3D_MATANM_RESULTFLAG_TEXMTX_ROTZERO;
                }

                // テクスチャ行列のTranslation設定
                if (!(mat->flag & NNS_G3D_MATFLAG_TEXMTX_TRANSZERO))
                {
                    const fx32* p_fx32 = (const fx32*)p;

                    dummy.transS = *(p_fx32 + 0);
                    dummy.transT = *(p_fx32 + 1);
                }
                else
                {
                    dummy.flag |= NNS_G3D_MATANM_RESULTFLAG_TEXMTX_TRANSZERO;
                }

                func = NNS_G3dSendTexSRT_FuncArray[pResMdl->info.texMtxMode];
                NNS_G3D_NULL_ASSERT(func);
                if (func)
                {
                    // テクスチャ行列をジオメトリエンジンに設定
                    // モデル作成に利用されたCGツール毎の設定を行う
                    (*func)(&dummy);
                }
            }
        }
    }

    NNS_G3D_ASSERT(shpID < pResMdl->info.numShp);
    if (shpID < pResMdl->info.numShp)
    {
        // Shpを送信
        const NNSG3dResShp* p;
        const NNSG3dResShpData* shp;
        
        p = NNS_G3dGetShp(pResMdl);
        NNS_G3D_NULL_ASSERT(p);
        shp = NNS_G3dGetShpDataByIdx(p, shpID);
        NNS_G3D_NULL_ASSERT(shp);

        NNS_G3dGeSendDL(NNS_G3dGetShpDLPtr(shp), NNS_G3dGetShpDLSize(shp));
    }

    if (pResMdl->info.invPosScale != FX32_ONE)
    {
        // pos_scaleによるスケール補正がかかっている場合は元に戻す
        NNS_G3dGeScale(pResMdl->info.invPosScale,
                       pResMdl->info.invPosScale,
                       pResMdl->info.invPosScale);
    }
}








