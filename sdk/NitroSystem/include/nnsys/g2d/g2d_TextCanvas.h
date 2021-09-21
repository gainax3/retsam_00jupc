/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d
  File:     g2d_TextCanvas.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.7 $
 *---------------------------------------------------------------------------*/
#ifndef G2D_TEXTAREA_H_
#define G2D_TEXTAREA_H_

#include <nnsys/g2d/g2d_Font.h>
#include <nnsys/g2d/g2d_CharCanvas.h>
#include <nnsys/g2d/g2di_AssertUtil.h>
#include <nnsys/g2d/g2di_SplitChar.h>
#include <nnsys/g2d/g2di_Char.h>

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------

//---------------------------------------------------------------------
// TextCanvas マクロ
//---------------------------------------------------------------------
#define NNS_G2D_TEXTCANVAS_ASSERT( pTxn )                           \
    NNS_G2D_ASSERTMSG(                                              \
        NNS_G2D_IS_VALID_POINTER(pTxn)                              \
            && NNS_G2D_IS_VALID_POINTER((pTxn)->pCanvas)            \
            && NNS_G2D_IS_VALID_POINTER((pTxn)->pFont)              \
        , "Illegal NNSG2dTextCanvas." )

#define NNS_G2D_CHARENCODING_ASSERT( enc )                      \
    NNS_G2D_ASSERTMSG(                                          \
        (0 <= (enc)) && ((enc) < NNS_G2D_NUM_OF_CHARENCODING)   \
        , "Illegal NNSG2dCharEncoding(=%d).", (enc) )           \



//---------------------------------------------------------------------
// TextCanvas 定義
//---------------------------------------------------------------------

// 垂直方向基準点配置
typedef enum NNSG2dVerticalOrigin
{
    NNS_G2D_VERTICALORIGIN_TOP      = 0x1,
    NNS_G2D_VERTICALORIGIN_MIDDLE   = 0x2,
    NNS_G2D_VERTICALORIGIN_BOTTOM   = 0x4
}
NNSG2dVerticalOrigin;

// 水平方向基準点配置
typedef enum NNSG2dHorizontalOrigin
{
    NNS_G2D_HORIZONTALORIGIN_LEFT   = 0x8,
    NNS_G2D_HORIZONTALORIGIN_CENTER = 0x10,
    NNS_G2D_HORIZONTALORIGIN_RIGHT  = 0x20
}
NNSG2dHorizontalOrigin;

// 垂直方向揃え
typedef enum NNSG2dVerticalAlign
{
    NNS_G2D_VERTICALALIGN_TOP       = 0x40,
    NNS_G2D_VERTICALALIGN_MIDDLE    = 0x80,
    NNS_G2D_VERTICALALIGN_BOTTOM    = 0x100
}
NNSG2dVerticalAlign;

// 水平方向揃え
typedef enum NNSG2dHorizontalAlign
{
    NNS_G2D_HORIZONTALALIGN_LEFT    = 0x200,
    NNS_G2D_HORIZONTALALIGN_CENTER  = 0x400,
    NNS_G2D_HORIZONTALALIGN_RIGHT   = 0x800
}
NNSG2dHorizontalAlign;



// TextCanvas
typedef struct NNSG2dTextCanvas
{
    const NNSG2dCharCanvas* pCanvas;
    const NNSG2dFont* pFont;
    int hSpace;
    int vSpace;
}
NNSG2dTextCanvas;



// NNS_G2dTextCanvasDrawTaggedText 用コールバックパラメータ
typedef struct NNSG2dTagCallbackInfo
{
    NNSG2dTextCanvas txn;   // 描画に使用しているTextCanvas。
    const NNSG2dChar* str;  // 描画中の文字列へのポインタ。
    int x;                  // 描画中の座標 x
    int y;                  // 描画中の座標 y
    int clr;                // 描画中の色番号。
    void* cbParam;          // NNS_G2dTextCanvasDrawTaggedText の第7引数。
}
NNSG2dTagCallbackInfo;

// NNS_G2dTextCanvasDrawTaggedText 用コールバック関数
typedef void (*NNSG2dTagCallback)(u16 c, NNSG2dTagCallbackInfo* pInfo);

// 文字列描画時の文字の配置方向
typedef struct NNSiG2dTextDirection
{
    s8 x;
    s8 y;
}
NNSiG2dTextDirection;



//---------------------------------------------------------------------
// TextCanvas 操作
//---------------------------------------------------------------------

//--------------------------------------------
// 描画

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasDrawString

  Description:  最初の改行文字もしくは終端文字までを描画します。

  Arguments:    pTxn:   描画対象の TextCanvas へのポインタ。
                x:      描画開始位置の x 座標。
                y:      描画開始位置の y 座標。
                cl:     文字色の色番号。
                str:    描画する文字列。
                pPos:   改行文字まで描画した場合は改行文字の次の文字への
                        ポインタを、終端文字まで描画した場合はNULLを格納する
                        バッファへのポインタ。
                        不要な場合はNULLを指定することができます。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void NNSi_G2dTextCanvasDrawString(
    const NNSG2dTextCanvas* pTxn,
    int x,
    int y,
    int cl,
    const void* str,
    const void** pPos
#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
    , NNSiG2dTextDirection d
#endif
);



/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dDrawTextAlign

  Description:  最初の改行文字もしくは終端文字までを描画します。

  Arguments:    pTxn:       描画対象の TextCanvas へのポインタ。
                x:          描画開始位置の x 座標。
                y:          描画開始位置の y 座標。
                cl:         文字色の色番号。
                flags:      描画位置を指定するフラグ。
                txt:        描画する文字列。
                areaWidth:  右寄せ、中寄せの際に参照される描画領域幅。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void NNSi_G2dTextCanvasDrawTextAlign(
    const NNSG2dTextCanvas* pTxn,
    int x,
    int y,
    int areaWidth,
    int cl,
    u32 flags,
    const void* txt
#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
    , NNSiG2dTextDirection d
#endif
);



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasDrawText

  Description:  複数行の文字列を描画します。
                点を基準に描画位置を指定することができます。

  Arguments:    pTxn:   描画対象の TextCanvas へのポインタ。
                x:      描画基準位置の x 座標。
                y:      描画基準位置の y 座標。
                cl:     文字色の色番号。
                flags:  描画位置を指定するフラグ。
                txt:    描画する文字列。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void NNSi_G2dTextCanvasDrawText(
    const NNSG2dTextCanvas* pTxn,
    int x,
    int y,
    int cl,
    u32 flags,
    const void* txt
#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
    , NNSiG2dTextDirection d
#endif
);



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasDrawTextRect

  Description:  複数行の文字列を描画します。
                矩形を基準に描画位置を指定することができます。
                矩形をはみ出た文字も通常どおり描画されます。

  Arguments:    pTxn:   描画対象の TextCanvas へのポインタ。
                x:      描画矩形左上の x 座標。
                y:      描画矩形左上の y 座標。
                cl:     文字色の色番号。
                flags:  描画位置を指定するフラグ。
                txt:    描画する文字列。
                w:      描画矩形の幅。
                h:      描画矩形の高さ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void NNSi_G2dTextCanvasDrawTextRect(
    const NNSG2dTextCanvas* pTxn,
    int x,
    int y,
    int w,
    int h,
    int cl,
    u32 flags,
    const void* txt
#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
    , NNSiG2dTextDirection d
#endif
);



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasDrawTaggedText

  Description:  タグが埋め込まれた複数行の文字列を描画します。
                タグによって描画を制御する事ができます。

  Arguments:    pTxn:       描画対象の TextCanvas へのポインタ。
                x:          描画基準位置の x 座標。
                y:          描画基準位置の y 座標。
                cl:         文字色の色番号。
                txt:        描画する文字列。
                cbFunc:     タグによって呼ばれるコールバック関数。
                cbParam:    コールバック関数に渡されるユーザデータ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void NNSi_G2dTextCanvasDrawTaggedText(
    const NNSG2dTextCanvas* pTxn,
    int x,
    int y,
    int cl,
    const void* txt,
    NNSG2dTagCallback cbFunc,
    void* cbParam
#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
    , NNSiG2dTextDirection d
#endif
);





//--------------------------------------------
// 構築

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasInit*

  Description:  TextCanvas を初期化します。

  Arguments:    pTxn:   初期化する TextCanvas へのポインタ。
                pCC:    描画先の CharCanvas へのポインタ。
                pFont:  描画に用いるフォントへのポインタ。
                hSpace: 文字間隔。
                vSpace: フォントの持つ行間隔の補正差分。
                encode: 文字切り出しコールバック関数へのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dTextCanvasInit(
    NNSG2dTextCanvas* pTxn,
    const NNSG2dCharCanvas* pCC,
    const NNSG2dFont* pFont,
    int hSpace,
    int vSpace
)
{
    NNS_G2D_POINTER_ASSERT( pTxn );
    NNS_G2D_CHARCANVAS_ASSERT( pCC );
    NNS_G2D_FONT_ASSERT( pFont );

    pTxn->pCanvas       = pCC;
    pTxn->pFont         = pFont;
    pTxn->hSpace        = hSpace;
    pTxn->vSpace        = vSpace;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasInit*

  Description:  TextCanvas を初期化します。

  Arguments:    pTxn:       初期化する TextCanvas へのポインタ。
                pCC:        描画先の CharCanvas へのポインタ。
                pFont:      描画に用いるフォントへのポインタ。
                hSpace:     文字間隔。
                vSpace:     フォントの持つ行間隔の補正差分。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
#define NNS_G2dTextCanvasInit1Byte      NNS_G2dTextCanvasInit
#define NNS_G2dTextCanvasInitUTF8       NNS_G2dTextCanvasInit
#define NNS_G2dTextCanvasInitUTF16      NNS_G2dTextCanvasInit
#define NNS_G2dTextCanvasInitShiftJIS   NNS_G2dTextCanvasInit


//--------
// getter

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasGetCharCanvas

  Description:  TextCanvas の描画対象の CharCanvas を設定します。

  Arguments:    pTxn:   フォントを設定する TextCanvas へのポインタ。
                pCC:    新しく設定する CharCanvas へのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE const NNSG2dCharCanvas* NNS_G2dTextCanvasGetCharCanvas(
    const NNSG2dTextCanvas* pTxn
)
{
    NNS_G2D_TEXTCANVAS_ASSERT( pTxn );
    return pTxn->pCanvas;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasGetFont

  Description:  TextCanvas にフォントを設定します。

  Arguments:    pTxn:   フォントを設定する TextCanvas へのポインタ。
                pFont:  新しく設定するフォントへのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE const NNSG2dFont* NNS_G2dTextCanvasGetFont( const NNSG2dTextCanvas* pTxn )
{
    NNS_G2D_TEXTCANVAS_ASSERT( pTxn );
    return pTxn->pFont;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasGetHSpace

  Description:  TextCanvas が文字列の描画時に用いる間隔を設定します。

  Arguments:    pTxn:   フォントを設定する TextCanvas へのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE int NNS_G2dTextCanvasGetHSpace( const NNSG2dTextCanvas* pTxn )
{
    NNS_G2D_TEXTCANVAS_ASSERT( pTxn );
    return pTxn->hSpace;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasGetVSpace

  Description:  TextCanvas が文字列の描画時に用いる間隔を設定します。

  Arguments:    pTxn:   フォントを設定する TextCanvas へのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE int NNS_G2dTextCanvasGetVSpace( const NNSG2dTextCanvas* pTxn )
{
    NNS_G2D_TEXTCANVAS_ASSERT( pTxn );
    return pTxn->vSpace;
}



//--------
// setter

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasSetCharCanvas

  Description:  TextCanvas の描画対象の CharCanvas を設定します。

  Arguments:    pTxn:   フォントを設定する TextCanvas へのポインタ。
                pCC:    新しく設定する CharCanvas へのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dTextCanvasSetCharCanvas(
    NNSG2dTextCanvas* pTxn,
    const NNSG2dCharCanvas* pCC
)
{
    NNS_G2D_TEXTCANVAS_ASSERT( pTxn );
    NNS_G2D_CHARCANVAS_ASSERT( pCC );
    pTxn->pCanvas = pCC;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasSetFont

  Description:  TextCanvas にフォントを設定します。

  Arguments:    pTxn:   フォントを設定する TextCanvas へのポインタ。
                pFont:  新しく設定するフォントへのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dTextCanvasSetFont( NNSG2dTextCanvas* pTxn, const NNSG2dFont* pFont )
{
    NNS_G2D_TEXTCANVAS_ASSERT( pTxn );
    NNS_G2D_FONT_ASSERT( pFont );
    pTxn->pFont = pFont;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasSetHSpace

  Description:  TextCanvas が文字列の描画時に用いる間隔を設定します。

  Arguments:    pTxn:   フォントを設定する TextCanvas へのポインタ。
                hSpace: 新しく設定する文字間。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dTextCanvasSetHSpace( NNSG2dTextCanvas* pTxn, int hSpace )
{
    NNS_G2D_TEXTCANVAS_ASSERT( pTxn );
    pTxn->hSpace = hSpace;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasSetVSpace

  Description:  TextCanvas が文字列の描画時に用いる間隔を設定します。

  Arguments:    pTxn:   フォントを設定する TextCanvas へのポインタ。
                vSpace: 新しく設定する行間。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dTextCanvasSetVSpace( NNSG2dTextCanvas* pTxn, int vSpace )
{
    NNS_G2D_TEXTCANVAS_ASSERT( pTxn );
    pTxn->vSpace = vSpace;
}



//--------------------------------------------
// 情報取得

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasGetStringWidth

  Description:  最初の改行文字もしくは終端文字までを描画した場合の幅を求めます。

  Arguments:    pTxn:   計算の基準となる TextCanvas へのポインタ。
                str:    文字列。
                pPos:   改行文字で処理を停止した場合は改行文字の次の文字への
                        ポインタを、終端文字で停止した場合はNULLを格納する
                        バッファへのポインタ。
                        不要な場合はNULLを指定することができます。


  Returns:      文字列を描画した場合の幅。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE int NNS_G2dTextCanvasGetStringWidth(
    const NNSG2dTextCanvas* pTxn,
    const NNSG2dChar* str,
    const NNSG2dChar** pPos
)
{
    return NNS_G2dFontGetStringWidth(
                pTxn->pFont,
                pTxn->hSpace,
                str,
                pPos );
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasGetTextHeight

  Description:  文字列を描画した場合の高さを求めます。

  Arguments:    pTxn:   計算の基準となる TextCanvas へのポインタ。
                txt:    文字列。

  Returns:      文字列を描画した場合の高さ。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE int NNS_G2dTextCanvasGetTextHeight(
    const NNSG2dTextCanvas* pTxn,
    const NNSG2dChar* txt
)
{
    return NNS_G2dFontGetTextHeight(
                pTxn->pFont,
                pTxn->vSpace,
                txt );
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasGetTextWidth

  Description:  文字列を描画した場合の幅を求めます。
                これは各行の幅の最大値になります。

  Arguments:    pTxn:   計算の基準となる TextCanvas へのポインタ。
                txt:    文字列。

  Returns:      文字列を描画した場合の幅。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE int NNS_G2dTextCanvasGetTextWidth(
    const NNSG2dTextCanvas* pTxn,
    const NNSG2dChar* txt
)
{
    return NNS_G2dFontGetTextWidth(
                pTxn->pFont,
                pTxn->hSpace,
                txt );
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasGetTextRect

  Description:  文字列を描画した場合の高さと幅を求めます。

  Arguments:    pTxn:   計算の基準となる TextCanvas へのポインタ。
                txt:    文字列。

  Returns:      文字列を描画した場合の高さと幅。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE NNSG2dTextRect NNS_G2dTextCanvasGetTextRect(
    const NNSG2dTextCanvas* pTxn,
    const NNSG2dChar* txt
)
{
    return NNS_G2dFontGetTextRect(
                pTxn->pFont,
                pTxn->hSpace,
                pTxn->vSpace,
                txt );
}



//---------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dGetTextDirection

  Description:  フォントに応じた文字列配置方向を取得します。

  Arguments:    pFont:  文字列配置方向取得元のフォント

  Returns:      フォントに応じた文字列配置方向を返します。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE NNSiG2dTextDirection NNSi_G2dGetTextDirection(const NNSG2dFont* pFont)
{
    NNSiG2dTextDirection d = { 0, 0 };
    switch( NNS_G2dFontGetFlags(pFont) )
    {
    case (NNS_G2D_FONT_FLAG_ROT_0):
    case (NNS_G2D_FONT_FLAG_ROT_270|NNS_G2D_FONT_FLAG_TBRL): d.x = 1; break;
    case (NNS_G2D_FONT_FLAG_ROT_90):
    case (NNS_G2D_FONT_FLAG_ROT_0|NNS_G2D_FONT_FLAG_TBRL):   d.y = 1; break;
    case (NNS_G2D_FONT_FLAG_ROT_180):
    case (NNS_G2D_FONT_FLAG_ROT_90|NNS_G2D_FONT_FLAG_TBRL):  d.x = -1; break;
    case (NNS_G2D_FONT_FLAG_ROT_270):
    case (NNS_G2D_FONT_FLAG_ROT_180|NNS_G2D_FONT_FLAG_TBRL): d.y = -1; break;
    }
    return d;
}

#ifdef NNS_G2D_FONT_ENABLE_DIRECTION_SUPPORT
NNS_G2D_INLINE void NNS_G2dTextCanvasDrawString(const NNSG2dTextCanvas* pTxn, int x, int y, int cl, const NNSG2dChar* str, const NNSG2dChar** pPos)
    { NNSi_G2dTextCanvasDrawString(pTxn, x, y, cl, str, (const void**)pPos,
            NNSi_G2dGetTextDirection(NNS_G2dTextCanvasGetFont(pTxn))); }
NNS_G2D_INLINE void NNS_G2dTextCanvasDrawText(const NNSG2dTextCanvas* pTxn, int x, int y, int cl, u32 flags, const NNSG2dChar* txt)
    { NNSi_G2dTextCanvasDrawText(pTxn, x, y, cl, flags, txt,
            NNSi_G2dGetTextDirection(NNS_G2dTextCanvasGetFont(pTxn))); }
NNS_G2D_INLINE void NNS_G2dTextCanvasDrawTextRect(const NNSG2dTextCanvas* pTxn, int x, int y, int w, int h, int cl, u32 flags, const NNSG2dChar* txt)
    { NNSi_G2dTextCanvasDrawTextRect(pTxn, x, y, w, h, cl, flags, txt,
            NNSi_G2dGetTextDirection(NNS_G2dTextCanvasGetFont(pTxn))); }
NNS_G2D_INLINE void NNS_G2dTextCanvasDrawTaggedText(const NNSG2dTextCanvas* pTxn, int x, int y, int cl, const NNSG2dChar* txt, NNSG2dTagCallback cbFunc, void* cbParam)
    { NNSi_G2dTextCanvasDrawTaggedText(pTxn, x, y, cl, txt, cbFunc, cbParam,
            NNSi_G2dGetTextDirection(NNS_G2dTextCanvasGetFont(pTxn))); }
#else
NNS_G2D_INLINE void NNS_G2dTextCanvasDrawString(const NNSG2dTextCanvas* pTxn, int x, int y, int cl, const NNSG2dChar* str, const NNSG2dChar** pPos)
    { NNSi_G2dTextCanvasDrawString(pTxn, x, y, cl, str, (const void**)pPos); }
NNS_G2D_INLINE void NNS_G2dTextCanvasDrawText(const NNSG2dTextCanvas* pTxn, int x, int y, int cl, u32 flags, const NNSG2dChar* txt)
    { NNSi_G2dTextCanvasDrawText(pTxn, x, y, cl, flags, txt); }
NNS_G2D_INLINE void NNS_G2dTextCanvasDrawTextRect(const NNSG2dTextCanvas* pTxn, int x, int y, int w, int h, int cl, u32 flags, const NNSG2dChar* txt)
    { NNSi_G2dTextCanvasDrawTextRect(pTxn, x, y, w, h, cl, flags, txt); }
NNS_G2D_INLINE void NNS_G2dTextCanvasDrawTaggedText(const NNSG2dTextCanvas* pTxn, int x, int y, int cl, const NNSG2dChar* txt, NNSG2dTagCallback cbFunc, void* cbParam)
    { NNSi_G2dTextCanvasDrawTaggedText(pTxn, x, y, cl, txt, cbFunc, cbParam); }
#endif

//---------------------------------------------------------------------

#ifdef __cplusplus
}/* extern "C" */
#endif

#endif // G2D_TEXTAREA_H_

