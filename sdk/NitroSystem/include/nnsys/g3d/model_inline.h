/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g3d
  File:     model_inline.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.7 $
 *---------------------------------------------------------------------------*/

//
// AUTHOR: Kenji Nishida
//

#ifndef NNSG3D_MODEL_INLINE_
#define NNSG3D_MODEL_INLINE_

//
// DO NOT INCLUDE THIS FILE DIRECTLY
//

#ifdef __cplusplus
extern "C" {
#endif

void NNSi_G3dModifyMatFlag(NNSG3dResMdl* pMdl, BOOL isOn, NNSG3dMatFlag flag);
void NNSi_G3dModifyPolygonAttrMask(NNSG3dResMdl* pMdl, BOOL isOn, u32 mask);

//
// To Use Global data
//

/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseGlbDiff

    NNS_G3dGlbMaterialColorDiffAmbで設定したdiffuseをモデル内のマテリアルの
    diffuseとして使用します。
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseGlbDiff(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyMatFlag(pMdl,
                          FALSE,
                          NNS_G3D_MATFLAG_DIFFUSE);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseGlbAmb

    NNS_G3dGlbMaterialColorDiffAmbで設定したambientをモデル内のマテリアルの
    ambientとして使用します。
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseGlbAmb(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyMatFlag(pMdl,
                          FALSE,
                          NNS_G3D_MATFLAG_AMBIENT);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseGlbSpec

    NNS_G3dGlbMaterialColorSpecEmiで設定したspecularをモデル内のマテリアルの
    specularとして使用します。
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseGlbSpec(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyMatFlag(pMdl,
                          FALSE,
                          NNS_G3D_MATFLAG_SPECULAR);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseGlbEmi

    NNS_G3dGlbMaterialColorSpecEmiで設定したemissionをモデル内のマテリアルの
    emissionとして使用します。
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseGlbEmi(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyMatFlag(pMdl,
                          FALSE,
                          NNS_G3D_MATFLAG_EMISSION);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseGlbLightEnableFlag

    NNS_G3dGlbPolygonAttrで設定したライトイネーブルフラグをモデル内のマテリアルの
    ライトイネーブルフラグとして使用します。
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseGlbLightEnableFlag(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  FALSE,
                                  REG_G3_POLYGON_ATTR_LE_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseGlbPolygonMode

    NNS_G3dGlbPolygonAttrで設定したポリゴンモードをモデル内のマテリアルの
    ポリゴンモードとして使用します。
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseGlbPolygonMode(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  FALSE,
                                  REG_G3_POLYGON_ATTR_PM_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseGlbCullMode

    NNS_G3dGlbPolygonAttrで設定したカルモードをモデル内のマテリアルの
    カルモードとして使用します。
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseGlbCullMode(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  FALSE,
                                  REG_G3_POLYGON_ATTR_BK_MASK |
                                  REG_G3_POLYGON_ATTR_FR_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseGlbPolygonID

    NNS_G3dGlbPolygonAttrで設定したポリゴンIDをモデル内のマテリアルの
    ポリゴンIDとして使用します。
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseGlbPolygonID(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  FALSE,
                                  REG_G3_POLYGON_ATTR_ID_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseGlbAlpha

    NNS_G3dGlbPolygonAttrで設定したα値をモデル内のマテリアルの
    α値として使用します。
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseGlbAlpha(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  FALSE,
                                  REG_G3_POLYGON_ATTR_ALPHA_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseGlbFogEnableFlag

    NNS_G3dGlbPolygonAttrで設定したフォグイネーブルフラグをモデル内のマテリアルの
    フォグイネーブルフラグとして使用します。
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseGlbFogEnableFlag(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  FALSE,
                                  REG_G3_POLYGON_ATTR_FE_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseGlbDepthTestCond

    NNS_G3dGlbPolygonAttrで設定したデプステスト条件をモデル内のマテリアルの
    デプステスト条件として使用します。
    (.nsbmd形式のファイルのデフォルト設定です)
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseGlbDepthTestCond(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  FALSE,
                                  REG_G3_POLYGON_ATTR_DT_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseGlb1Dot

    NNS_G3dGlbPolygonAttrで設定した１ドットポリゴン表示指定をモデル内のマテリアルの
    １ドットポリゴン表示指定として使用します。
    (.nsbmd形式のファイルのデフォルト設定です)
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseGlb1Dot(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  FALSE,
                                  REG_G3_POLYGON_ATTR_D1_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseGlbFarClip

    NNS_G3dGlbPolygonAttrで設定したFAR面交差ポリゴン表示指定をモデル内のマテリアルの
    FAR面交差ポリゴン表示指定として使用します。
    (.nsbmd形式のファイルのデフォルト設定です)
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseGlbFarClip(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  FALSE,
                                  REG_G3_POLYGON_ATTR_FC_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseGlbXLDepthUpdate

    NNS_G3dGlbPolygonAttrで設定した半透明ポリゴンのデプス値更新イネーブルフラグを
    モデル内のマテリアルの半透明ポリゴンのデプス値更新イネーブルフラグとして使用します。
    (.nsbmd形式のファイルのデフォルト設定です)
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseGlbXLDepthUpdate(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  FALSE,
                                  REG_G3_POLYGON_ATTR_XL_MASK);
}


//
// To Use Model's Data
//

/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseMdlDiff

    モデルリソース内で設定されている個々のマテリアルのdiffuseを使用します。
    (.nsbmd形式のファイルのデフォルト設定です)
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseMdlDiff(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyMatFlag(pMdl,
                          TRUE,
                          NNS_G3D_MATFLAG_DIFFUSE);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseMdlAmb

    モデルリソース内で設定されている個々のマテリアルのambientを使用します。
    (.nsbmd形式のファイルのデフォルト設定です)
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseMdlAmb(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyMatFlag(pMdl,
                          TRUE,
                          NNS_G3D_MATFLAG_AMBIENT);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseMdlSpec

    モデルリソース内で設定されている個々のマテリアルのspecularを使用します。
    (.nsbmd形式のファイルのデフォルト設定です)
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseMdlSpec(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyMatFlag(pMdl,
                          TRUE,
                          NNS_G3D_MATFLAG_SPECULAR);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseMdlEmi

    モデルリソース内で設定されている個々のマテリアルのemissionを使用します。
    (.nsbmd形式のファイルのデフォルト設定です)
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseMdlEmi(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyMatFlag(pMdl,
                          TRUE,
                          NNS_G3D_MATFLAG_EMISSION);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseMdlLightEnableFlag

    モデルリソース内で設定されている個々のマテリアルのライトイネーブルフラグを使用します。
    (.nsbmd形式のファイルのデフォルト設定です)
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseMdlLightEnableFlag(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  TRUE,
                                  REG_G3_POLYGON_ATTR_LE_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseMdlPolygonMode

    モデルリソース内で設定されている個々のマテリアルのポリゴンモードを使用します。
    (.nsbmd形式のファイルのデフォルト設定です)
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseMdlPolygonMode(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  TRUE,
                                  REG_G3_POLYGON_ATTR_PM_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseMdlCullMode

    モデルリソース内で設定されている個々のマテリアルのカルモードを使用します。
    (.nsbmd形式のファイルのデフォルト設定です)
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseMdlCullMode(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  TRUE,
                                  REG_G3_POLYGON_ATTR_BK_MASK |
                                  REG_G3_POLYGON_ATTR_FR_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseMdlPolygonID

    モデルリソース内で設定されている個々のマテリアルのポリゴンIDを使用します。
    (.nsbmd形式のファイルのデフォルト設定です)
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseMdlPolygonID(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  TRUE,
                                  REG_G3_POLYGON_ATTR_ID_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseMdlAlpha

    モデルリソース内で設定されている個々のマテリアルのα値を使用します。
    (.nsbmd形式のファイルのデフォルト設定です)
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseMdlAlpha(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  TRUE,
                                  REG_G3_POLYGON_ATTR_ALPHA_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseMdlFogEnableFlag

    モデルリソース内で設定されている個々のマテリアルのフォグイネーブルフラグを
    使用します。
    (.nsbmd形式のファイルのデフォルト設定です)
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseMdlFogEnableFlag(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  TRUE,
                                  REG_G3_POLYGON_ATTR_FE_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseMdlDepthTestCond

    モデルリソース内で設定されている個々のデプステスト条件を使用します。
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseMdlDepthTestCond(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  TRUE,
                                  REG_G3_POLYGON_ATTR_DT_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseMdl1Dot

    モデルリソース内で設定されている個々の１ドットポリゴン表示指定を使用します。
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseMdl1Dot(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  TRUE,
                                  REG_G3_POLYGON_ATTR_D1_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseMdlFarClip

    モデルリソース内で設定されている個々のFAR面交差ポリゴン表示指定を使用します。
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseMdlFarClip(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  TRUE,
                                  REG_G3_POLYGON_ATTR_FC_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseMdlXLDepthUpdate

    モデルリソース内で設定されている個々の半透明ポリゴンのデプス値更新イネーブルフラグ
    を使用します。
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseMdlXLDepthUpdate(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  TRUE,
                                  REG_G3_POLYGON_ATTR_XL_MASK);
}





#ifdef __cplusplus
}
#endif

#endif
