/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d
  File:     g2d_CharCanvas.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.8 $
 *---------------------------------------------------------------------------*/
#ifndef G2D_CHARCANVAS_H_
#define G2D_CHARCANVAS_H_

#include <nnsys/g2d/g2d_Font.h>
#include <nnsys/g2d/g2di_AssertUtil.h>
#include <nnsys/g2d/fmt/g2d_Cell_data.h>

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------

//---------------------------------------------------------------------
// CharCanvas マクロ
//---------------------------------------------------------------------

#define NNS_G2D_CHARCANVAS_ASSERT( pCC )                                \
    NNS_G2D_ASSERTMSG(                                                  \
        NNS_G2D_IS_VALID_POINTER(pCC)                                   \
        && (0 < (pCC)->areaWidth)                                       \
        && (0 < (pCC)->areaHeight)                                      \
        && ( ((pCC)->dstBpp == 4) || ((pCC)->dstBpp == 8) )             \
        && NNS_G2D_IS_VALID_POINTER( (pCC)->charBase )                  \
        && NNS_G2D_IS_ALIGNED((pCC)->charBase, 4)                       \
        && NNS_G2D_IS_VALID_POINTER( (pCC)->vtable )                    \
        && NNS_G2D_IS_VALID_POINTER( (pCC)->vtable->pDrawGlyph )        \
        && NNS_G2D_IS_VALID_POINTER( (pCC)->vtable->pClear )            \
        && NNS_G2D_IS_VALID_POINTER( (pCC)->vtable->pClearArea )        \
        , "Illegal NNSG2dCharCanvas.")

#define NNS_G2D_COLORMODE_ASSERT( cmode )                               \
    NNS_G2D_ASSERTMSG(                                                  \
        (cmode) == NNS_G2D_CHARA_COLORMODE_16                           \
        || (cmode) == NNS_G2D_CHARA_COLORMODE_256                       \
        , "Illegal Color Mode(=%d)", (cmode) )

#define NNS_G2D_OBJVRAMMODE_ASSERT( vmode )                             \
    NNS_G2D_ASSERTMSG(                                                  \
        (vmode) == NNS_G2D_OBJVRAMMODE_32K                              \
        || (vmode) == NNS_G2D_OBJVRAMMODE_64K                           \
        || (vmode) == NNS_G2D_OBJVRAMMODE_128K                          \
        || (vmode) == NNS_G2D_OBJVRAMMODE_256K                          \
        , "Illegal VRAM Mode(=%d)", (vmode) )

#define NNS_G2D_TEXT_BG_WIDTH_ASSERT( width )                           \
    NNS_G2D_ASSERTMSG(                                                  \
        (width) == NNS_G2D_TEXT_BG_WIDTH_256                            \
        || (width) == NNS_G2D_TEXT_BG_WIDTH_512                         \
        , "Illegal Text BG Width(=%d).", (width) )

#define NNS_G2D_AFFINE_BG_WIDTH_ASSERT( width )                         \
    NNS_G2D_ASSERTMSG(                                                  \
        (width) == NNS_G2D_AFFINE_BG_WIDTH_128                          \
        || (width) == NNS_G2D_AFFINE_BG_WIDTH_256                       \
        || (width) == NNS_G2D_AFFINE_BG_WIDTH_512                       \
        || (width) == NNS_G2D_AFFINE_BG_WIDTH_1024                      \
        , "Illegal Affine BG Width(=%d).", (width) )

#define NNS_G2D_256x16PLTT_BG_WIDTH_ASSERT( width )                     \
    NNS_G2D_ASSERTMSG(                                                  \
        (width) == NNS_G2D_256x16PLTT_BG_WIDTH_128                      \
        || (width) == NNS_G2D_256x16PLTT_BG_WIDTH_256                   \
        || (width) == NNS_G2D_256x16PLTT_BG_WIDTH_512                   \
        || (width) == NNS_G2D_256x16PLTT_BG_WIDTH_1024                  \
        , "Illegal 256x16Pltt BG Width(=%d).", (width) )




// TEXT BG の幅
typedef enum NNSG2dTextBGWidth
{
    NNS_G2D_TEXT_BG_WIDTH_256   = 32,
    NNS_G2D_TEXT_BG_WIDTH_512   = 64
}
NNSG2dTextBGWidth;

// アフィンBG の幅
typedef enum NNSG2dAffineBGWidth
{
    NNS_G2D_AFFINE_BG_WIDTH_128     = 16,
    NNS_G2D_AFFINE_BG_WIDTH_256     = 32,
    NNS_G2D_AFFINE_BG_WIDTH_512     = 64,
    NNS_G2D_AFFINE_BG_WIDTH_1024    = 128
}
NNSG2dAffineBGWidth;

// アフィン拡張 256x16パレットタイプ BG の幅
typedef enum NNSG2d256x16PlttBGWidth
{
    NNS_G2D_256x16PLTT_BG_WIDTH_128     = 16,
    NNS_G2D_256x16PLTT_BG_WIDTH_256     = 32,
    NNS_G2D_256x16PLTT_BG_WIDTH_512     = 64,
    NNS_G2D_256x16PLTT_BG_WIDTH_1024    = 128
}
NNSG2d256x16PlttBGWidth;

// キャラクタのカラーモード
typedef enum NNSG2dCharaColorMode
{
    NNS_G2D_CHARA_COLORMODE_16 = 4,
    NNS_G2D_CHARA_COLORMODE_256 = 8
}
NNSG2dCharaColorMode;

// OBJ キャラクタ領域参照可能サイズ
typedef enum NNSG2dOBJVramMode
{
    NNS_G2D_OBJVRAMMODE_32K     = 0,
    NNS_G2D_OBJVRAMMODE_64K     = 1,
    NNS_G2D_OBJVRAMMODE_128K    = 2,
    NNS_G2D_OBJVRAMMODE_256K    = 3
}
NNSG2dOBJVramMode;

//---------------------------------------------------------------------
// CharCanvas 定義
//---------------------------------------------------------------------


struct NNSG2dCharCanvas;

// CharCanvas グリフ描画関数定義
typedef void (*NNSiG2dDrawGlyphFunc)(
    const struct NNSG2dCharCanvas* pCC,
    const NNSG2dFont* pFont,
    int x,
    int y,
    int cl,
    const NNSG2dGlyph* pGlyph
);

// CharCanvas クリア関数定義
typedef void (*NNSiG2dClearFunc)(
    const struct NNSG2dCharCanvas* pCC,
    int cl
);

// CharCanvas 部分クリア関数定義
typedef void (*NNSiG2dClearAreaFunc)(
    const struct NNSG2dCharCanvas* pCC,
    int cl,
    int x,
    int y,
    int w,
    int h
);

typedef struct NNSiG2dCharCanvasVTable
{
    NNSiG2dDrawGlyphFunc    pDrawGlyph;
    NNSiG2dClearFunc        pClear;
    NNSiG2dClearAreaFunc    pClearArea;
}
NNSiG2dCharCanvasVTable;

// CharCanvas構造体
typedef struct NNSG2dCharCanvas
{
    u8*     charBase;
    int     areaWidth;
    int     areaHeight;
    u8      dstBpp;
    u8      reserved[3];
    u32     param;
    const NNSiG2dCharCanvasVTable* vtable;
}
NNSG2dCharCanvas;





//****************************************************************************
// BG スクリーン構成関数
//****************************************************************************

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dMapScrToCharText

  Description:  CharCanvas 用にスクリーンからキャラクタへ1対1に対応させます。
                テキストBG用です。

  Arguments:    scnBase:    CharCanvas が利用している BG面の
                            スクリーンベースへのポインタ。
                areaWidth:  CharCanvas のキャラクタ単位での幅
                areaHeight: CharCanvas のキャラクタ単位での高さ
                areaLeft:   BG面の左上を(0,0)とした時の CharCanvas 左上の
                            x 座標。
                areaTop:    BG面の左上を(0,0)とした時の CharCanvas 左上の
                            y 座標。
                scnWidth:   BG面の幅
                charNo:     CharCanvas に割り当てられているキャラクタ列の
                            先頭のキャラクタへのポインタ。
                cplt:       スクリーンに指定するカラーパレット番号。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void NNS_G2dMapScrToCharText(
    void* scnBase,
    int areaWidth,
    int areaHeight,
    int areaLeft,
    int areaTop,
    NNSG2dTextBGWidth scnWidth,
    int charNo,
    int cplt
);



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dMapScrToCharAffine

  Description:  CharCanvas 用にスクリーンからキャラクタへ1対1に対応させます。
                アフィンBG用です。

  Arguments:    areaBase:   CharCanvas 左上にあたるスクリーンへのポインタ。
                areaWidth:  CharCanvas のキャラクタ単位での幅
                areaHeight: CharCanvas のキャラクタ単位での高さ
                scnWidth:   BG面の幅
                charNo:     CharCanvas に割り当てられているキャラクタ列の
                            先頭のキャラクタへのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void NNS_G2dMapScrToCharAffine(
    void* areaBase,
    int areaWidth,
    int areaHeight,
    NNSG2dAffineBGWidth scnWidth,
    int charNo
);



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dMapScrToChar256x16Pltt

  Description:  CharCanvas 用にスクリーンからキャラクタへ1対1に対応させます。
                アフィン拡張BGの256x16パレットタイプ用です。

  Arguments:    areaBase:   CharCanvas 左上にあたるスクリーンへのポインタ。
                areaWidth:  CharCanvas のキャラクタ単位での幅
                areaHeight: CharCanvas のキャラクタ単位での高さ
                scnWidth:   BG面の幅
                charNo:     CharCanvas に割り当てられているキャラクタ列の
                            先頭のキャラクタへのポインタ。
                cplt:       スクリーンに指定するカラーパレット番号。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void NNS_G2dMapScrToChar256x16Pltt(
    void* areaBase,
    int areaWidth,
    int areaHeight,
    NNSG2d256x16PlttBGWidth scnWidth,
    int charNo,
    int cplt
);





//****************************************************************************
// OBJ 配列関数
//****************************************************************************

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dCalcRequiredOBJ
                NNS_G2dCalcRequiredOBJ1D
                NNS_G2dCalcRequiredOBJ2DRect

  Description:  NNS_G2dArrangeOBJ* で必要なOBJ数を計算します。
                CharCanvas の大きさが同じであれば NNS_G2dArrangeOBJ1D と
                NNS_G2dArrangeOBJ2DRect は同じようにOBJを並べます。
                そのため NNS_G2dCalcRequireOBJ1D と
                NNS_G2dCalcRequireOBJ2DRect も内部的には同じものです。

  Arguments:    areaWidth:  OBJ数を計算する CharCanvas のキャラクタ単位での幅。
                areaHeight: OBJ数を計算する CharCanvas のキャラクタ単位での高さ。

  Returns:      必要なOBJ数。
 *---------------------------------------------------------------------------*/
int NNSi_G2dCalcRequiredOBJ(
    int areaWidth,
    int areaHeight
);

NNS_G2D_INLINE int NNS_G2dCalcRequiredOBJ1D(
    int areaWidth,
    int areaHeight
)
{
    return NNSi_G2dCalcRequiredOBJ(areaWidth, areaHeight);
}

NNS_G2D_INLINE int NNS_G2dCalcRequiredOBJ2DRect(
    int areaWidth,
    int areaHeight
)
{
    return NNSi_G2dCalcRequiredOBJ(areaWidth, areaHeight);
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dArrangeOBJ1D

  Description:  NNS_G2dCharCanvasInitForOBJ1D で描画したOBJを適切に
                表示できるようにOBJを整列させます。
                oam を始点とする NNS_G2dCalcRequireOBJ1D(areaWidth, areaHeight)
                個の OBJ を使用します。

  Arguments:    oam:        使用するOAM列の始点へのポインタ。
                areaWidth:  CharCanvas のキャラクタ単位での幅。
                areaHeight: CharCanvas のキャラクタ単位での高さ。
                x:          CharCanvas 左上隅の表示位置。
                y:          CharCanvas 左上隅の表示位置。
                color:      CharCanvas のカラーモード。
                charName:   OBJ列が表示に用いるキャラクタ列先頭のキャラクタネーム。
                vramMode:   OBJ VRAM容量

  Returns:
 *---------------------------------------------------------------------------*/
int NNS_G2dArrangeOBJ1D(
    GXOamAttr * oam,
    int areaWidth,
    int areaHeight,
    int x,
    int y,
    GXOamColorMode color,
    int charName,
    NNSG2dOBJVramMode vramMode
);



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dArrangeOBJ2DRect

  Description:  NNS_G2dCharCanvasInitForOBJ2DRect で描画したOBJを適切に
                表示できるようにOBJを整列させます。
                oam を始点とする NNS_G2dCalcRequireOBJ2DRect(areaWidth, areaHeight)
                個の OBJ を使用します。

  Arguments:    oam:        使用するOAM列の始点へのポインタ。
                areaWidth:  CharCanvas のキャラクタ単位での幅。
                areaHeight: CharCanvas のキャラクタ単位での高さ。
                x:          CharCanvas 左上隅の表示位置。
                y:          CharCanvas 左上隅の表示位置。
                color:      CharCanvas のカラーモード。
                charName:   OBJ列が表示に用いるキャラクタ列先頭のキャラクタネーム。

  Returns:
 *---------------------------------------------------------------------------*/
int NNS_G2dArrangeOBJ2DRect(
    GXOamAttr * oam,
    int areaWidth,
    int areaHeight,
    int x,
    int y,
    GXOamColorMode color,
    int charName
);





//****************************************************************************
// CharCanvas 操作
//****************************************************************************

// 描画

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dCharCanvasDrawChar

  Description:  CharCanvas に文字コード指定で1文字描画します。

  Arguments:    pCC:    CharCanvas へのポインタ。
                pFont:  描画に用いるフォントへのポインタ。
                x:      文字の左上の座標
                y:      文字の左上の座標
                cl:     文字色の色番号。
                pGlyph: 描画する文字の文字コード。

  Returns:      文字の描画幅。
 *---------------------------------------------------------------------------*/
int NNS_G2dCharCanvasDrawChar(
    const NNSG2dCharCanvas* pCC,
    const NNSG2dFont* pFont,
    int x,
    int y,
    int cl,
    u16 ccode
);



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dCharCanvasDrawGlyph

  Description:  CharCanvas に指定したグリフを描画します。

  Arguments:    pCC:    CharCanvas へのポインタ。
                pFont:  描画に用いるフォントへのポインタ。
                x:      文字の左上の座標。
                y:      文字の左上の座標。
                cl:     文字色の色番号。
                pGlyph: 描画するグリフへのポインタ。。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dCharCanvasDrawGlyph(
    const NNSG2dCharCanvas* pCC,
    const NNSG2dFont* pFont,
    int x,
    int y,
    int cl,
    const NNSG2dGlyph* pGlyph
)
{
    NNS_G2D_CHARCANVAS_ASSERT( pCC );
    pCC->vtable->pDrawGlyph(pCC, pFont, x, y, cl, pGlyph);
}


// 消去

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dCharCanvasClear

  Description:  CharCanvas に属しているキャラクタ全体を
                指定された色で塗りつぶします。

  Arguments:    pCC:    CharCanvas へのポインタ。
                cl:     塗りつぶし色の色番号。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dCharCanvasClear(
    const NNSG2dCharCanvas* pCC,
    int cl
)
{
    NNS_G2D_CHARCANVAS_ASSERT( pCC );
    pCC->vtable->pClear(pCC, cl);
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dCharCanvasClearArea

  Description:  CharCanvas に属しているキャラクタの指定された領域を
                指定された色で塗りつぶします。

  Arguments:    pCC:    CharCanvas へのポインタ。
                cl:     塗りつぶし色の色番号。
                x:      塗りつぶす領域の左上の x 座標。
                y:      塗りつぶす領域の左上の y 座標。
                w:      塗りつぶす領域の幅。
                h:      塗りつぶす領域の高さ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dCharCanvasClearArea(
    const NNSG2dCharCanvas* pCC,
    int cl,
    int x,
    int y,
    int w,
    int h
)
{
    NNS_G2D_CHARCANVAS_ASSERT( pCC );
    pCC->vtable->pClearArea(pCC, cl, x, y, w, h);
}



//----------------------------------------------------------------------------
// 構築
//----------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dCharCanvasInitForBG

  Description:  CharCanvas を初期化します。
                BG用にパラメータを直接指定して初期化します。

  Arguments:    pCC:        CharCanvas へのポインタ。
                charBase:   CharCanvas に割り当てるキャラクタ列の
                            先頭のキャラクタへのポインタ。
                areaWidth:  CharCanvas のキャラクタ単位での幅。
                areaHeight: CharCanvas のキャラクタ単位での高さ。
                colorMode:  描画対象キャラクタのカラーモード。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void NNS_G2dCharCanvasInitForBG(
    NNSG2dCharCanvas* pCC,
    void* charBase,
    int areaWidth,
    int areaHeight,
    NNSG2dCharaColorMode colorMode
);



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dCharCanvasInitForOBJ1D

  Description:  CharCanvas を1DマッピングOBJで使用するために初期化します。

  Arguments:    pCC:        初期化する CharCanvas へのポインタ。
                charBase:   CharCanvasに割り当てるキャラクタの始点へのポインタ。
                areaWidth:  CharCanvasのキャラクタ単位での幅。
                areaHeight: CharCanvasのキャラクタ単位での高さ。
                colorMode:  描画対象キャラクタのカラーモード。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void NNS_G2dCharCanvasInitForOBJ1D(
    NNSG2dCharCanvas* pCC,
    void* charBase,
    int areaWidth,
    int areaHeight,
    NNSG2dCharaColorMode colorMode
);



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dCharCanvasInitForOBJ2DRect

  Description:  CharCanvas を初期化します。
                2次元マッピングのOBJでキャラクターメモリ中の矩形を
                CharCanvas とする時に用います。

  Arguments:    pCC:        CharCanvas へのポインタ。
                charBase:   CharCanvas の左上のキャラクタへのポインタ。
                areaWidth:  CharCanvas のキャラクタ単位での幅。
                areaHeight: CharCanvas のキャラクタ単位での高さ。
                colorMode:  描画対象キャラクタのカラーモード。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void NNS_G2dCharCanvasInitForOBJ2DRect(
    NNSG2dCharCanvas* pCC,
    void* charBase,
    int areaWidth,
    int areaHeight,
    NNSG2dCharaColorMode colorMode
);



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dCharCanvasMakeCell1D

  Description:  NNS_G2dCharCanvasInitForOBJ1D() で初期化した CharCanvas を
                表示するためのセルを作成します。

  Arguments:    pCell:      生成したセルデータを格納するバッファ
                pCC:        CharCanvas へのポインタ
                x:          セルの中心座標(CharCanvas座標系)
                y:          セルの中心座標(CharCanvas座標系)
                priority:   セルのプライオリティ
                mode:       セルのモード
                mosaic:     セルのモザイク
                effect:     セルのエフェクト
                color:      CharCanvas のカラーモード
                charName:   先頭キャラクタネーム
                cParam:     カラーパレット番号
                vramMode:   OBJ VRAM容量の設定を指定します。
                makeBR:     境界矩形情報を付加するかどうかを指定します。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void NNS_G2dCharCanvasMakeCell1D(
    NNSG2dCellData* pCell,
    const NNSG2dCharCanvas* pCC,
    int x,
    int y,
    int priority,
    GXOamMode mode,
    BOOL mosaic,
    GXOamEffect effect,
    GXOamColorMode color,
    int charName,
    int cParam,
    NNSG2dOBJVramMode vramMode,
    BOOL makeBR
);



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dCharCanvasMakeCell2DRect

  Description:  NNS_G2dCharCanvasInitForOBJ2DRect() で初期化した CharCanvas を
                表示するためのセルを作成します。

  Arguments:    pCell:      生成したセルデータを格納するバッファ
                pCC:        CharCanvas へのポインタ
                x:          セルの中心座標(CharCanvas座標系)
                y:          セルの中心座標(CharCanvas座標系)
                priority:   セルのプライオリティ
                mode:       セルのモード
                mosaic:     セルのモザイク
                effect:     セルのエフェクト
                color:      CharCanvas のカラーモード
                charName:   先頭キャラクタネーム
                cParam:     カラーパレット番号
                makeBR:     境界矩形情報を付加するかどうかを指定します。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void NNS_G2dCharCanvasMakeCell2DRect(
    NNSG2dCellData* pCell,
    const NNSG2dCharCanvas* pCC,
    int x,
    int y,
    int priority,
    GXOamMode mode,
    BOOL mosaic,
    GXOamEffect effect,
    GXOamColorMode color,
    int charName,
    int cParam,
    BOOL makeBR
);



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dCharCanvasCalcCellDataSize*

  Description:  NNS_G2dCharCanvasMakeCell*() の第一引数に渡すべき
                バッファのサイズを計算します。

  Arguments:    pCC:        CharCanvas へのポインタ
                makeBR:     境界矩形情報を付加するかどうかを指定します。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE size_t NNSi_G2dCharCanvasCalcCellDataSize(
    const NNSG2dCharCanvas* pCC,
    BOOL makeBR
)
{
    const int numObj        = NNSi_G2dCalcRequiredOBJ(pCC->areaWidth, pCC->areaHeight);
    const size_t oamSize    = sizeof(NNSG2dCellOAMAttrData) * numObj;
    const size_t brSize     = makeBR ? sizeof(NNSG2dCellBoundingRectS16): 0;

    return sizeof(NNSG2dCellData) + brSize + oamSize;
}

NNS_G2D_INLINE size_t NNS_G2dCharCanvasCalcCellDataSize1D(
    const NNSG2dCharCanvas* pCC,
    BOOL makeBR
)
{
    return NNSi_G2dCharCanvasCalcCellDataSize(pCC, makeBR);
}

NNS_G2D_INLINE size_t NNS_G2dCharCanvasCalcCellDataSize2DRect(
    const NNSG2dCharCanvas* pCC,
    BOOL makeBR
)
{
    return NNSi_G2dCharCanvasCalcCellDataSize(pCC, makeBR);
}






//---------------------------------------------------------------------

#ifdef __cplusplus
}/* extern "C" */
#endif

#endif // G2D_CHARCANVAS_H_

