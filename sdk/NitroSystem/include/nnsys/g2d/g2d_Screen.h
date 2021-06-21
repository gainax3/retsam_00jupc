/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d
  File:     g2d_Screen.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.8 $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_SCREEN_H_
#define NNS_G2D_SCREEN_H_

#include <nitro.h>
#include <nnsys/g2d.h>
#include <nnsys/g2d/fmt/g2d_Screen_data.h>

#ifdef __cplusplus
extern "C" {
#endif



//-------------------------------------------------------------------------
// 列挙子
//-------------------------------------------------------------------------

// 処理対象のBG指定用
typedef enum NNSG2dBGSelect
{
    NNS_G2D_BGSELECT_MAIN0,
    NNS_G2D_BGSELECT_MAIN1,
    NNS_G2D_BGSELECT_MAIN2,
    NNS_G2D_BGSELECT_MAIN3,
    NNS_G2D_BGSELECT_SUB0,
    NNS_G2D_BGSELECT_SUB1,
    NNS_G2D_BGSELECT_SUB2,
    NNS_G2D_BGSELECT_SUB3,
    NNS_G2D_BGSELECT_NUM
}
NNSG2dBGSelect;

#define NNS_G2D_BG_ASSERT(bg) SDK_MINMAX_ASSERT( bg, NNS_G2D_BGSELECT_MAIN0, NNS_G2D_BGSELECT_NUM-1 )


//-------------------------------------------------------------------------
// 関数宣言
//-------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dBGSetupEx

  Description:  BGの設定を行い、BGデータをVRAMに読み込みます。

  Arguments:    bg:         対象とする BG
                pScnData:   BG で使用するスクリーンデータへのポインタ。
                            NULL であってはなりません。
                pChrData:   BG で使用するキャラクタデータへのポインタ。
                            NULL の場合はVRAMに読み込みません。
                pPltData:   BG で使用するパレットデータへのポインタ。
                            NULL の場合はVRAMに読み込みません。
                pPosInfo:   キャラクタ抽出領域情報へのポインタ。
                pCmpInfo:   パレット圧縮情報へのポインタ。
                scnBase:    BG のスクリーンベース。
                chrBase:    BG のキャラクタベース。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void NNS_G2dBGSetupEx(
    NNSG2dBGSelect bg,
    const NNSG2dScreenData* pScnData,
    const NNSG2dCharacterData* pChrData,
    const NNSG2dPaletteData* pPltData,
    const NNSG2dCharacterPosInfo* pPosInfo,
    const NNSG2dPaletteCompressInfo* pCmpInfo,
    GXBGScrBase scnBase,
    GXBGCharBase chrBase
);

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dBGLoadElements

  Description:  BG で用いるグラフィックスデータを適切なVRAMに読み込みます。
                対象 BG の BGControl が適切に設定されている必要があります。
                BG0-1の拡張パレットの読込先は拡張パレットへの VRAM の
                割り当て状況から自動的に判断します。
                Text BG の 256x1 パレットは拡張パレットが有効かどうかで
                ロード先を切り替えます。

  Arguments:    bg:         ロードするデータを使用する BG
                pScnData:   VRAM にロードするスクリーンデータへのポインタ。
                            NULL でも構いませんが、pPltData が NULL でない
                            場合は NULL であってはなりません。
                pChrData:   VRAM にロードするキャラクタデータへのポインタ。
                            NULL の場合はVRAMに読み込みません。
                pPltData:   VRAM にロードするパレットデータへのポインタ。
                            NULL の場合はVRAMに読み込みません。
                pPosInfo:   キャラクタ抽出領域情報へのポインタ。
                pCmpInfo:   パレット圧縮情報へのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void NNS_G2dBGLoadElementsEx(
    NNSG2dBGSelect bg,
    const NNSG2dScreenData* pScnData,
    const NNSG2dCharacterData* pChrData,
    const NNSG2dPaletteData* pPltData,
    const NNSG2dCharacterPosInfo* pPosInfo,
    const NNSG2dPaletteCompressInfo* pCmpInfo
);

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dBGLoadScreenRect

  Description:  スクリーンデータ中の指定された矩形をバッファの指定された位置に
                コピーします。

  Arguments:    pScreenDst: 転送先基準点へのポインタ。
                pScnData:   転送下となるスクリーンデータへのポインタ。
                srcX:       転送元左上隅のX座標。（キャラクタ単位）
                srcY:       転送元左上隅のY座標。（キャラクタ単位）
                dstX:       転送先左上隅のX座標。（キャラクタ単位）
                dstY:       転送先左上隅のY座標。（キャラクタ単位）
                dstW:       転送先領域の幅。（キャラクタ単位）
                dstH:       転送先領域の高さ。（キャラクタ単位）
                width:      転送する領域の幅。（キャラクタ単位）
                height:     転送する領域の高さ。（キャラクタ単位）

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void NNS_G2dBGLoadScreenRect(
    void* pScreenDst,
    const NNSG2dScreenData* pScnData,
    int srcX,
    int srcY,
    int dstX,
    int dstY,
    int dstW,
    int dstH,
    int width,
    int height
);


//-------------------------------------------------------------------------
// 大域変数(非公開)
//-------------------------------------------------------------------------

extern GXBGAreaOver NNSi_G2dBGAreaOver;




//-------------------------------------------------------------------------
// inline 関数
//-------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dBGGetScreenColorMode

  Description:  スクリーンデータのカラーモードを取得します。

  Arguments:    pScnData:   スクリーンデータへのポインタ

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE GXBGColorMode NNSi_G2dBGGetScreenColorMode(const NNSG2dScreenData* pScnData)
{
    NNS_G2D_POINTER_ASSERT( pScnData );
    return (pScnData->colorMode == NNS_G2D_SCREENCOLORMODE_16x16) ?
                GX_BG_COLORMODE_16: GX_BG_COLORMODE_256;
}



/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dBGGetScreenFormat

  Description:  スクリーンデータのスクリーンフォーマットを取得します。

  Arguments:    pScnData:   スクリーンデータへのポインタ

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE NNSG2dScreenFormat NNSi_G2dBGGetScreenFormat(const NNSG2dScreenData* pScnData)
{
    NNS_G2D_POINTER_ASSERT( pScnData );
    return (NNSG2dScreenFormat)pScnData->screenFormat;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetBGAreaOver

  Description:  NNS_G2dLoadBGScreenSet() で Affine (拡張) BG の
                エリアオーバー処理指定として用いられるを指定します。

  Arguments:    areaOver:   以降の NNS_G2dLoadBGScreenSet() で用いられる
                            エリアオーバー処理を指定します。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dSetBGAreaOver( GXBGAreaOver areaOver )
{
    SDK_MINMAX_ASSERT( areaOver, GX_BG_AREAOVER_XLU, GX_BG_AREAOVER_REPEAT );
    NNSi_G2dBGAreaOver = areaOver;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetBGAreaOver

  Description:  NNS_G2dLoadBGScreenSet() で用いられるエリアオーバー処理を
                取得します。

  Arguments:    なし。

  Returns:      NNS_G2dLoadBGScreenSet() で用いられるエリアオーバー処理。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE GXBGAreaOver NNS_G2dGetBGAreaOver( void )
{
    return NNSi_G2dBGAreaOver;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dBGSetup

  Description:  BGの設定を行い、BGデータをVRAMに読み込みます。

  Arguments:    bg:         対象とする BG
                pScnData:   BG で使用するスクリーンデータへのポインタ。
                            NULL であってはなりません。
                pChrData:   BG で使用するキャラクタデータへのポインタ。
                            NULL の場合はVRAMに読み込みません。
                pPltData:   BG で使用するパレットデータへのポインタ。
                            NULL の場合はVRAMに読み込みません。
                scnBase:    BG のスクリーンベース。
                chrBase:    BG のキャラクタベース。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dBGSetup(
    NNSG2dBGSelect bg,
    const NNSG2dScreenData* pScnData,
    const NNSG2dCharacterData* pChrData,
    const NNSG2dPaletteData* pPltData,
    GXBGScrBase scnBase,
    GXBGCharBase chrBase
)
{
    NNS_G2dBGSetupEx(bg, pScnData, pChrData, pPltData, NULL, NULL, scnBase, chrBase);
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dBGLoadElements

  Description:  BG で用いるグラフィックスデータを適切なVRAMに読み込みます。
                対象 BG の BGControl が適切に設定されている必要があります。
                BG0-1の拡張パレットの読込先は拡張パレットへの VRAM の
                割り当て状況から自動的に判断します。
                Text BG の 256x1 パレットは拡張パレットが有効かどうかで
                ロード先を切り替えます。

  Arguments:    bg:         ロードするデータを使用する BG
                pScnData:   VRAM にロードするスクリーンデータへのポインタ。
                            NULL でも構いませんが、pPltData が NULL でない
                            場合は NULL であってはなりません。
                pChrData:   VRAM にロードするキャラクタデータへのポインタ。
                            NULL の場合はVRAMに読み込みません。
                pPltData:   VRAM にロードするパレットデータへのポインタ。
                            NULL の場合はVRAMに読み込みません。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dBGLoadElements(
    NNSG2dBGSelect bg,
    const NNSG2dScreenData* pScnData,
    const NNSG2dCharacterData* pChrData,
    const NNSG2dPaletteData* pPltData
)
{
    NNS_G2dBGLoadElementsEx(bg, pScnData, pChrData, pPltData, NULL, NULL);
}


//-------------------------------------------------------------------------





#ifdef __cplusplus
}/* extern "C" */
#endif

#endif // NNS_G2D_SCREEN_H_

