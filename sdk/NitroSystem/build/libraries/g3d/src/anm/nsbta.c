/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g3d - src - anm
  File:     nsbta.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.20 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#include <nnsys/g3d/anm/nsbta.h>
#include <nnsys/g3d/binres/res_struct_accessor.h>

#ifndef NNS_G3D_NSBTA_DISABLE
//---------------------------------------------------------------------
static fx32
GetTexSRTAnmVectorVal_(const NNSG3dResTexSRTAnm* pTexAnm, 
                       u32 info, 
                       u32 data, 
                       u32 frame)
{
    u32 idx, idx_sub;
    u32 last_interp;
    const void* pDataHead;

    if( info & NNS_G3D_TEXSRTANM_ELEM_CONST )
    {
        return (fx32)data;
    }

    pDataHead = (const void*)((u8*)pTexAnm + data);

    if (!(info & NNS_G3D_TEXSRTANM_ELEM_STEP_MASK))
    {
        // NNS_G3D_TEXSRTANM_ELEM_STEP_1が選択されている
        idx = frame;
        goto TEXSRT_VAL_NONINTERP;
    }

    // last_interp以降は1コマずつデータが入っている
    // last_interpは2の倍数か4の倍数である。
    last_interp = (NNS_G3D_TEXSRTANM_ELEM_LAST_INTERP_MASK & info) >>
                                NNS_G3D_TEXSRTANM_ELEM_LAST_INTERP_SHIFT;

    if (info & NNS_G3D_TEXSRTANM_ELEM_STEP_2)
    {
        // NNS_G3D_TEXSRTANM_ELEM_STEP_2が選択されている
        if (frame & 1)
        {
            if (frame > last_interp)
            {
                // 最終フレーム以外ありえない
                idx = (last_interp >> 1) + 1;
                goto TEXSRT_VAL_NONINTERP;
            }
            else
            {
                // 奇数で最終フレームでないので50:50の補間処理がいる。
                idx = frame >> 1;
                goto TEXSRT_VAL_INTERP_2;
            }
        }
        else
        {
            // 偶数フレームなので補間処理はいらない
            idx = frame >> 1;
            goto TEXSRT_VAL_NONINTERP;
        }
    }
    else
    {
        // NNS_G3D_TEXSRTANM_ELEM_STEP_4が選択されている
        if (frame & 3)
        {
            if (frame > last_interp)
            {
                // last_interp以降は補間処理なし
                idx = (last_interp >> 2) + (frame & 3);
                goto TEXSRT_VAL_NONINTERP;
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

                if (info & NNS_G3D_TEXSRTANM_ELEM_FX16)
                {
                    v = *((const fx16*)pDataHead + idx);
                    v_sub = *((const fx16*)pDataHead + idx_sub);
                }
                else
                {
                    v = *((const fx32*)pDataHead + idx);
                    v_sub = *((const fx32*)pDataHead + idx_sub);
                }
                return (v + v + v + v_sub) >> 2;
            }
            else
            {
                // 50:50の補間になる
                idx = frame >> 2;
                goto TEXSRT_VAL_INTERP_2;
            }
        }
        else
        {
            // フレームは丁度4の倍数になっている
            idx = frame >> 2;
            goto TEXSRT_VAL_NONINTERP;
        }
    }
    NNS_G3D_ASSERT(0);
TEXSRT_VAL_NONINTERP:
    if (info & NNS_G3D_TEXSRTANM_ELEM_FX16)
    {
        return *((const fx16*)pDataHead + idx);
    }
    else
    {
        return *((const fx32*)pDataHead + idx);
    }
TEXSRT_VAL_INTERP_2:
    {
        fx32 v0, v1;
        if (info & NNS_G3D_TEXSRTANM_ELEM_FX16)
        {
            v0 = *((const fx16*)pDataHead + idx);
            v1 = *((const fx16*)pDataHead + idx + 1);
        }
        else
        {
            v0 = *((const fx32*)pDataHead + idx);
            v1 = *((const fx32*)pDataHead + idx + 1);
        }
        return ((v0 + v1) >> 1);
    }
}


//---------------------------------------------------------------------
static u32
GetTexSRTAnmSinCosVal_(const NNSG3dResTexSRTAnm* pTexAnm, 
                       u32 info,
                       u32 data,
                       u32 frame)
{
    u32 idx, idx_sub;
    u32 last_interp;
    const void* pDataHead;

    NNS_G3D_ASSERT(!(info & NNS_G3D_TEXSRTANM_ELEM_FX16));

    if(info & NNS_G3D_TEXSRTANM_ELEM_CONST)
    {
        return data;
    }

    pDataHead = (const void*)((u8*)pTexAnm + data);

    if (!(info & NNS_G3D_TEXSRTANM_ELEM_STEP_MASK))
    {
        // NNS_G3D_TEXSRTANM_ELEM_STEP_1が選択されている
        idx = frame;
        goto TEXSRT_SINCOS_NONINTERP;
    }

    // last_interp以降は1コマずつデータが入っている
    // last_interpは2の倍数か4の倍数である。
    last_interp = (NNS_G3D_TEXSRTANM_ELEM_LAST_INTERP_MASK & info) >>
                                NNS_G3D_TEXSRTANM_ELEM_LAST_INTERP_SHIFT;

    if (info & NNS_G3D_TEXSRTANM_ELEM_STEP_2)
    {
        // NNS_G3D_TEXSRTANM_ELEM_STEP_2が選択されている
        if (frame & 1)
        {
            if (frame > last_interp)
            {
                // 最終フレーム以外ありえない
                idx = (last_interp >> 1) + 1;
                goto TEXSRT_SINCOS_NONINTERP;
            }
            else
            {
                // 奇数で最終フレームでないので50:50の補間処理がいる。
                idx = frame >> 1;
                goto TEXSRT_SINCOS_INTERP_2;
            }
        }
        else
        {
            // 偶数フレームなので補間処理はいらない
            idx = frame >> 1;
            goto TEXSRT_SINCOS_NONINTERP;
        }
    }
    else
    {
        // NNS_G3D_TEXSRTANM_ELEM_STEP_4が選択されている
        if (frame & 3)
        {
            if (frame > last_interp)
            {
                // last_interp以降は補間処理なし
                idx = (last_interp >> 2) + (frame & 3);
                goto TEXSRT_SINCOS_NONINTERP;
            }

            // 補間処理あり
            if (frame & 1)
            {
                fx32 s, s_sub;
                fx32 c, c_sub;

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

                s = *((const fx16*)pDataHead + 2 * idx);
                c = *((const fx16*)pDataHead + 2 * idx + 1);
                s_sub = *((const fx16*)pDataHead + 2 * idx_sub);
                c_sub = *((const fx16*)pDataHead + 2 * idx_sub + 1);

                s = (s + s + s + s_sub) >> 2;
                c = (c + c + c + c_sub) >> 2;
                return (u32)((s & 0xffff) | (c << 16));
            }
            else
            {
                // 50:50の補間になる
                idx = frame >> 2;
                goto TEXSRT_SINCOS_INTERP_2;
            }
        }
        else
        {
            // フレームは丁度4の倍数になっている
            idx = frame >> 2;
            goto TEXSRT_SINCOS_NONINTERP;
        }
    }
    NNS_G3D_ASSERT(0);
TEXSRT_SINCOS_NONINTERP:
    return *((const u32*)pDataHead + idx);
TEXSRT_SINCOS_INTERP_2:
    {
        fx32 s0, s1;
        fx32 c0, c1;
        s0 = *((const fx16*)pDataHead + 2 * idx);
        c0 = *((const fx16*)pDataHead + 2 * idx + 1);

        s1 = *((const fx16*)pDataHead + 2 * idx + 2);
        c1 = *((const fx16*)pDataHead + 2 * idx + 3);

        return (u32)((((s0 + s1) >> 1) & 0xffff) | (((c0 + c1) >> 1) << 16));
    }
}


static void 
GetTexSRTAnm_(const NNSG3dResTexSRTAnm* pTexAnm, 
              u16                       idx, 
              u32                       frame, 
              NNSG3dMatAnmResult*       pResult)
{
    NNS_G3D_ASSERT( idx < pTexAnm->dict.numEntry );
    NNS_G3D_ASSERT( frame < pTexAnm->numFrame );
    NNS_G3D_ASSERT( (pTexAnm->anmHeader.category0 == 'M') && (pTexAnm->anmHeader.category1 == 'TA') );
    
    {
        const NNSG3dResDictTexSRTAnmData* pAnmData = 
            (const NNSG3dResDictTexSRTAnmData*)NNS_G3dGetResDataByIdx( &pTexAnm->dict, idx );
        NNSG3dMatAnmResultFlag flag = pResult->flag;
        
        //
        // Trans
        //
        {
            fx32 transS, transT;
            
            transS = GetTexSRTAnmVectorVal_(pTexAnm,
                                            pAnmData->transS,
                                            pAnmData->transSEx,
                                            frame);
            transT = GetTexSRTAnmVectorVal_(pTexAnm,
                                            pAnmData->transT,
                                            pAnmData->transTEx,
                                            frame);
            //
            // データの特性を記述している、フラグ値を設定する。
            // この設定によって、後に行う、テクスチャ行列計算のコストを削減することが可能となる。
            // 
            if (transS == 0 && transT == 0 )
            {
                flag |=  NNS_G3D_MATANM_RESULTFLAG_TEXMTX_TRANSZERO;
            }
            else
            {
                flag &= ~NNS_G3D_MATANM_RESULTFLAG_TEXMTX_TRANSZERO;
                pResult->transS = transS;
                pResult->transT = transT;
            }
        }
        
        //
        // Rotation
        //
        {
            
            u32 data= GetTexSRTAnmSinCosVal_(pTexAnm, 
                                             pAnmData->rot,    
                                             pAnmData->rotEx,    
                                             frame); 

            //
            // データの特性を記述している、フラグ値を設定する。
            //
            if (data == ((FX32_ONE << 16) | 0)) // sin = 0, cos = 1
            {
                flag |=  NNS_G3D_MATANM_RESULTFLAG_TEXMTX_ROTZERO;
            }
            else
            {
                pResult->sinR = (fx16)(data & 0x0000FFFF);
                pResult->cosR = (fx16)(data >> 16);
                flag &= ~NNS_G3D_MATANM_RESULTFLAG_TEXMTX_ROTZERO;
            }
        }
        
        //
        // Scale
        //
        {        
            fx32 scaleS, scaleT;

            scaleS = GetTexSRTAnmVectorVal_(pTexAnm,
                                            pAnmData->scaleS,
                                            pAnmData->scaleSEx,
                                            frame);
            scaleT = GetTexSRTAnmVectorVal_(pTexAnm,
                                            pAnmData->scaleT,
                                            pAnmData->scaleTEx,
                                            frame);    
           
            //
            // データの特性を記述している、フラグ値を設定する。
            //
            if (scaleS == FX32_ONE && scaleT == FX32_ONE )
            {
                flag |=  NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SCALEONE;
            }
            else
            {
                flag &= ~NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SCALEONE;
                pResult->scaleS = scaleS;
                pResult->scaleT = scaleT;
            }
        }

        pResult->flag = flag;
    }
}


/*---------------------------------------------------------------------------*
    NNSi_G3dAnmObjInitNsBta

    NNSG3dAnmObjを.nsbtaリソース用にイニシャライズします。
    NNS_G3dInitAnmObjから呼び出されます。
 *---------------------------------------------------------------------------*/
void
NNSi_G3dAnmObjInitNsBta(NNSG3dAnmObj* pAnmObj,
                        void* pResAnm,
                        const NNSG3dResMdl* pResMdl)
{
    u32 i;
    NNSG3dResTexSRTAnm* srtAnm;
    const NNSG3dResMat* mat;

    NNS_G3D_NULL_ASSERT(pAnmObj);
    NNS_G3D_NULL_ASSERT(pResAnm);
    NNS_G3D_NULL_ASSERT(pResMdl);

    srtAnm = (NNSG3dResTexSRTAnm*)pResAnm;
    mat = NNS_G3dGetMat(pResMdl);
    pAnmObj->funcAnm = (void*) NNS_G3dFuncAnmMatNsBtaDefault;
    pAnmObj->numMapData = pResMdl->info.numMat;

    // まずはmapDataをゼロクリア
    MI_CpuClear16(&pAnmObj->mapData[0], sizeof(u16) * pAnmObj->numMapData);
    
    for (i = 0; i < srtAnm->dict.numEntry; ++i)
    {
        const NNSG3dResName* name = NNS_G3dGetResNameByIdx(&srtAnm->dict, i);
        int idx = NNS_G3dGetMatIdxByName(mat, name);
        if (!(idx < 0))
        {
            // リソースID iに対応するリソースが存在する場合は、
            // マテリアルID idxにリソースID iを関連付ける。
            pAnmObj->mapData[idx] = (u16)(i | NNS_G3D_ANMOBJ_MAPDATA_EXIST);
        }
    }
}


/*---------------------------------------------------------------------------*
    NNSi_G3dAnmCalcNsBta

    pResult: マテリアルアニメーションの結果を格納します。
    pAnmObj: アニメーションオブジェクトへのポインタ
    dataIdx: リソース内データの格納場所を示すインデックスです
 *---------------------------------------------------------------------------*/
void NNSi_G3dAnmCalcNsBta(NNSG3dMatAnmResult* pResult,
                          const NNSG3dAnmObj* pAnmObj,
                          u32 dataIdx)
{
    NNS_G3D_NULL_ASSERT(pResult);
    NNS_G3D_NULL_ASSERT(pAnmObj);

    {
        const NNSG3dResTexSRTAnm* pTexAnm 
                = (const NNSG3dResTexSRTAnm*)pAnmObj->resAnm;
                        
        GetTexSRTAnm_( pTexAnm,
                       (u16)dataIdx, 
                       (u32)FX_Whole( pAnmObj->frame ),
                       pResult );
        //
        // Texture SRT 使用時には強制的にテクスチャ座標生成モードを
        // GX_TEXGEN_TEXCOORD に設定します。
        //
        pResult->prmTexImage &= ~REG_G3_TEXIMAGE_PARAM_TGEN_MASK;
        pResult->prmTexImage |= GX_TEXGEN_TEXCOORD << REG_G3_TEXIMAGE_PARAM_TGEN_SHIFT;
        
        //
        // 本フラグを設定しないと、Texture行列がグラフィックスエンジンに送信されない。
        //
        pResult->flag |= NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SET;        
        
    }
}

#endif // NNS_G3D_NSBTA_DISABLE
