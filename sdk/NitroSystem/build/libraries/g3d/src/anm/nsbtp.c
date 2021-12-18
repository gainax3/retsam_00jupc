/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g3d - src - anm
  File:     nsbtp.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.21 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#include <nnsys/g3d/anm/nsbtp.h>
#include <nnsys/g3d/binres/res_struct_accessor.h>

#ifndef NNS_G3D_NSBTP_DISABLE
/*---------------------------------------------------------------------------*
    NNSi_G3dAnmObjInitNsBtp

    NNSG3dAnmObjを.nsbtpリソース用にイニシャライズします。
    NNS_G3dInitAnmObjから呼び出されます。
 *---------------------------------------------------------------------------*/
void
NNSi_G3dAnmObjInitNsBtp(NNSG3dAnmObj*       pAnmObj,
                        void*               pResAnm,
                        const NNSG3dResMdl* pResMdl)
{
    u32 i;
    NNSG3dResTexPatAnm* tpAnm = (NNSG3dResTexPatAnm*)pResAnm;
    const NNSG3dResMat* mat = NNS_G3dGetMat(pResMdl);

    NNS_G3D_NULL_ASSERT(pAnmObj->resTex);

    pAnmObj->funcAnm    = (void*) NNS_G3dFuncAnmMatNsBtpDefault;
    pAnmObj->numMapData = pResMdl->info.numMat;
    pAnmObj->resAnm     = tpAnm;

    // まずはmapDataをゼロクリア
    MI_CpuClear16(&pAnmObj->mapData[0], sizeof(u16) * pAnmObj->numMapData);

    for (i = 0; i < tpAnm->dict.numEntry; ++i)
    {
        const NNSG3dResName* name = NNS_G3dGetResNameByIdx(&tpAnm->dict, i);
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
    SetTexParamaters_

    pResult の テクスチャ関連部分を設定します
 *---------------------------------------------------------------------------*/
static void SetTexParamaters_
( 
    const NNSG3dResTex*     pTex, 
    const NNSG3dResName*    pTexName, 
    NNSG3dMatAnmResult*     pResult 
)
{
    NNS_G3D_NULL_ASSERT( pTex );
    NNS_G3D_NULL_ASSERT( pTexName );
    NNS_G3D_NULL_ASSERT( pResult );
    NNS_G3D_ASSERTMSG((pTex->texInfo.vramKey != 0 || pTex->texInfo.sizeTex == 0),
                      "No texture key assigned");
    NNS_G3D_ASSERTMSG((pTex->tex4x4Info.vramKey != 0 || pTex->tex4x4Info.sizeTex == 0),
                      "No texture(4x4) key assigned");
    {
        //
        // 名前からテクスチャデータを辞書引きします
        //
        const NNSG3dResDictTexData* pData = NNS_G3dGetTexDataByName( pTex, pTexName );        
        NNS_G3D_NULL_ASSERT( pData );
        
        {
            const u32 vramOffset = (( pData->texImageParam & REG_G3_TEXIMAGE_PARAM_TEXFMT_MASK) !=
                                                (GX_TEXFMT_COMP4x4 << REG_G3_TEXIMAGE_PARAM_TEXFMT_SHIFT)) ?
                                    NNS_GfdGetTexKeyAddr(pTex->texInfo.vramKey) >> NNS_GFD_TEXKEY_ADDR_SHIFT :    // 4x4以外のテクスチャ
                                    NNS_GfdGetTexKeyAddr(pTex->tex4x4Info.vramKey) >> NNS_GFD_TEXKEY_ADDR_SHIFT;  // 4x4テクスチャ

            // flipとrepeatとtexgenを残しリセットしてから合成する。
            // マテリアルが持っているデータは残しておかなくてはならない。
            pResult->prmTexImage &= REG_G3_TEXIMAGE_PARAM_TGEN_MASK |
                                    REG_G3_TEXIMAGE_PARAM_FT_MASK | REG_G3_TEXIMAGE_PARAM_FS_MASK |
                                    REG_G3_TEXIMAGE_PARAM_RT_MASK | REG_G3_TEXIMAGE_PARAM_RS_MASK;
            pResult->prmTexImage |= pData->texImageParam + vramOffset;
            
            pResult->origWidth  = (u16)(( pData->extraParam & NNS_G3D_TEXIMAGE_PARAMEX_ORIGW_MASK ) >> 
                                    NNS_G3D_TEXIMAGE_PARAMEX_ORIGW_SHIFT);
            pResult->origHeight = (u16)(( pData->extraParam & NNS_G3D_TEXIMAGE_PARAMEX_ORIGH_MASK ) >> 
                                    NNS_G3D_TEXIMAGE_PARAMEX_ORIGH_SHIFT);
            
            //
            // テクスチャ行列の計算に必要な縦横の倍率を算出します。
            //
            {
                const s32 w = (s32)(((pData->extraParam) & NNS_G3D_TEXIMAGE_PARAMEX_ORIGW_MASK) >> NNS_G3D_TEXIMAGE_PARAMEX_ORIGW_SHIFT);
                const s32 h = (s32)(((pData->extraParam) & NNS_G3D_TEXIMAGE_PARAMEX_ORIGH_MASK) >> NNS_G3D_TEXIMAGE_PARAMEX_ORIGH_SHIFT);
                                
                pResult->magW = ( w != pResult->origWidth ) ?
                                FX_Div( w << FX32_SHIFT, pResult->origWidth << FX32_SHIFT) :
                                FX32_ONE;
                pResult->magH = ( h != pResult->origHeight ) ?
                                FX_Div( h << FX32_SHIFT, pResult->origHeight << FX32_SHIFT) :
                                FX32_ONE;
            }
        }
    }
}


/*---------------------------------------------------------------------------*
    SetPlttParamaters_

    pResult の パレット関連部分を設定します
 *---------------------------------------------------------------------------*/
static void SetPlttParamaters_
( 
    const NNSG3dResTex*     pTex, 
    const NNSG3dResName*    pPlttName, 
    NNSG3dMatAnmResult*     pResult 
)
{
    NNS_G3D_NULL_ASSERT(pTex);
    NNS_G3D_NULL_ASSERT(pPlttName);
    NNS_G3D_NULL_ASSERT(pResult);
    NNS_G3D_ASSERTMSG((pTex->plttInfo.vramKey != 0 || pTex->plttInfo.sizePltt == 0),
         "No palette key assigned");
    
    {
        // テクスチャブロックからパレット名に対応するデータフィールドを得る
        const NNSG3dResDictPlttData* pPlttData = NNS_G3dGetPlttDataByName(pTex, pPlttName);
        u16 plttBase    = pPlttData->offset;
        u16 vramOffset  = (u16)(NNS_GfdGetTexKeyAddr(pTex->plttInfo.vramKey) >> NNS_GFD_TEXKEY_ADDR_SHIFT);
        
        NNS_G3D_NULL_ASSERT(pPlttData);    
        
        
        // 4colorsならビットが立っている
        if (!(pPlttData->flag & 1))
        {
            // 4colors以外の場合は4bitシフトになる。
            // 4colorsの場合は3bitシフトなのでそのまま
            plttBase >>= 1;
            vramOffset >>= 1;
        }
        
        //
        // アドレスを設定する
        //
        pResult->prmTexPltt = (u32)(plttBase + vramOffset);
    }
}


/*---------------------------------------------------------------------------*
    NNSi_G3dAnmCalcNsBtp

    pResult: マテリアルアニメーションの結果を格納します。
    pAnmObj:
    dataIdx: リソース内データの格納場所を示すインデックスです
 *---------------------------------------------------------------------------*/
void NNSi_G3dAnmCalcNsBtp(NNSG3dMatAnmResult* pResult,
                          const NNSG3dAnmObj* pAnmObj,
                          u32 dataIdx)
{
    NNS_G3D_NULL_ASSERT(pResult);
    NNS_G3D_NULL_ASSERT(pAnmObj);
    NNS_G3D_NULL_ASSERT(pAnmObj->resTex);
    
    {
        //
        // 現在のフレームに対応するテクスチャパタンーンFVを取得します
        //
        const NNSG3dResTexPatAnm*     pPatAnm 
            = (const NNSG3dResTexPatAnm*)pAnmObj->resAnm;
            
        const NNSG3dResTexPatAnmFV*   pTexFV 
            = NNSi_G3dGetTexPatAnmFV( pPatAnm, 
                                     (u16)dataIdx, 
                                     (u16)FX_Whole( pAnmObj->frame ) );
        NNS_G3D_NULL_ASSERT(pTexFV);
        
        //
        // pResult の テクスチャ関連部分を設定します
        //
        SetTexParamaters_( pAnmObj->resTex, 
                           NNSi_G3dGetTexPatAnmTexNameByIdx( pPatAnm, pTexFV->idTex ),
                           pResult ); 
        //                   
        // pResult の パレット関連部分を設定します
        //
        
        //
        // 現在のコンバータの実装ではダイレクトテクスチャフォーマット参照時には
        // pTexFV->idPltt メンバに 255 が代入されます。pTexFV->idPlttに255が設定された場合ライブラリは
        // パレット設定処理をスキップする必要があります。
        //
        if( pTexFV->idPltt != 255 )
        {
            SetPlttParamaters_( pAnmObj->resTex, 
                                NNSi_G3dGetTexPatAnmPlttNameByIdx( pPatAnm, pTexFV->idPltt ),
                                pResult ); 
        }                                     
    }
}

#endif // NNS_G3D_NSBTP_DISABLE
