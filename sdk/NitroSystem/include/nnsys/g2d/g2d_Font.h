/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d
  File:     g2d_Font.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.9 $
 *---------------------------------------------------------------------------*/
#ifndef G2D_FONT_H_
#define G2D_FONT_H_

#include <nnsys/g2d/g2d_config.h>
#include <nnsys/g2d/g2di_AssertUtil.h>
#include <nnsys/g2d/fmt/g2d_Font_data.h>
#include <nnsys/g2d/g2di_SplitChar.h>
#include <nnsys/g2d/g2di_Char.h>

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------

//*****************************************************************************
// Font マクロ
//*****************************************************************************

#define NNS_G2D_FONT_ASSERT( pFont )                                            \
    NNS_G2D_ASSERTMSG(                                                          \
        ((pFont) != NULL)                                                       \
            && (*(((u32*)((pFont)->pRes)) - 2) == NNS_G2D_BINBLK_SIG_FINFDATA)  \
            && ((pFont)->pRes->pGlyph != NULL)                                  \
            && NNS_G2D_IS_VALID_POINTER((pFont)->cbCharSpliter)                 \
        , "invalid NNSG2dFont data" )

#define NNS_G2D_GLYPH_ASSERT( pGlyph )                                          \
    NNS_G2D_ASSERTMSG(                                                          \
        ((pGlyph) != NULL)                                                      \
            && ((pGlyph)->image != NULL)                                        \
        , "invalid NNSG2dGlyph data")                                           \

#define NNS_G2D_FONT_MAX_GLYPH_INDEX( pFont )                                   \
    ((*((u32*)(pFont)->pRes->pGlyph - 1) - sizeof(*((pFont)->pRes->pGlyph)))    \
        / (pFont)->pRes->pGlyph->cellSize)                                      \


#define NNS_G2D_GLYPH_INDEX_NOT_FOUND   0xFFFF





//*****************************************************************************
// Font 定義
//*****************************************************************************

// フォント
typedef struct NNSG2dFont
{
    NNSG2dFontInformation*   pRes;          // 展開済みフォントリソースへのポインタ
    NNSiG2dSplitCharCallback cbCharSpliter; // 文字列エンコーディング処理コールバックへのポインタ
}
NNSG2dFont;



// グリフ
typedef struct NNSG2dGlyph
{
    const NNSG2dCharWidths* pWidths;    // 幅情報へのポインタ
    const u8* image;                    // グリフイメージへのポインタ
}
NNSG2dGlyph;



// 文字列矩形
typedef struct NNSG2dTextRect
{
    int width;      // 矩形幅
    int height;     // 矩形高さ
}
NNSG2dTextRect;





//*****************************************************************************
// Font 構築
//*****************************************************************************

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontInit*

  Description:  フォントリソースからフォントを構築します。

  Arguments:    pFont:      フォントへのポインタ。
                pNftrFile:  フォントリソースへのポインタ。

  Returns:      フォントの構築に成功した場合はFALSE以外を返します。
 *---------------------------------------------------------------------------*/
void NNS_G2dFontInitAuto(NNSG2dFont* pFont, void* pNftrFile);
void NNS_G2dFontInitUTF8(NNSG2dFont* pFont, void* pNftrFile);
void NNS_G2dFontInitUTF16(NNSG2dFont* pFont, void* pNftrFile);
void NNS_G2dFontInitShiftJIS(NNSG2dFont* pFont, void* pNftrFile);
void NNS_G2dFontInitCP1252(NNSG2dFont* pFont, void* pNftrFile);





//*****************************************************************************
// Font 操作
//*****************************************************************************

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontFindGlyphIndex

  Description:  文字コードから対応するグリフのインデックスを取得します。

  Arguments:    pFont:  フォントへのポインタ。
                c:      グリフインデックスを取得する文字コード。

  Returns:      c に対応するグリフが存在する場合はそのインデックス。
                存在しない場合は NNS_G2D_GLYPH_INDEX_NOT_FOUND。
 *---------------------------------------------------------------------------*/
u16 NNS_G2dFontFindGlyphIndex( const NNSG2dFont* pFont, u16 c );



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetCharWidthsFromIndex

  Description:  グリフインデックスから文字幅情報を取得します。

  Arguments:    pFont:  フォントへのポインタ。
                idx:    グリフインデックス。

  Returns:      文字幅情報へのポインタ。
 *---------------------------------------------------------------------------*/
const NNSG2dCharWidths* NNS_G2dFontGetCharWidthsFromIndex(
                                        const NNSG2dFont* pFont, u16 idx );





//*****************************************************************************
// Font アクセサ
//*****************************************************************************

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetType

  Description:  フォントのタイプを取得します。

  Arguments:    pFont:  フォントへのポインタ。

  Returns:      フォントのタイプ。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE NNSG2dFontType NNS_G2dFontGetType( const NNSG2dFont* pFont )
{
    NNS_G2D_FONT_ASSERT(pFont);
    return (NNSG2dFontType)pFont->pRes->fontType;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetLineFeed

  Description:  フォントの改行幅を取得します。

  Arguments:    pFont:  フォントへのポインタ。

  Returns:      フォントの改行幅。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE s8 NNS_G2dFontGetLineFeed( const NNSG2dFont* pFont )
{
    NNS_G2D_FONT_ASSERT(pFont);
    return pFont->pRes->linefeed;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetAlternateGlyphIndex

  Description:  フォントの代替文字グリフインデックスを取得します。

  Arguments:    pFont:  フォントへのポインタ。

  Returns:      フォントの代替文字グリフインデックス。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE u16 NNS_G2dFontGetAlternateGlyphIndex( const NNSG2dFont* pFont )
{
    NNS_G2D_FONT_ASSERT(pFont);
    return pFont->pRes->alterCharIndex;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetDefaultCharWidths

  Description:  デフォルトの文字幅情報を取得します。
                デフォルトの文字幅情報は文字が固有の文字幅情報を持たない場合に
                使用されます。

  Arguments:    pFont:  フォントへのポインタ。

  Returns:      デフォルトの文字幅情報へのポインタ。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE NNSG2dCharWidths* NNS_G2dFontGetDefaultCharWidths( const NNSG2dFont* pFont )
{
    NNS_G2D_FONT_ASSERT(pFont);
    return &pFont->pRes->defaultWidth;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetHeight

  Description:  フォントの高さを取得します。
                通常、グリフイメージの高さがフォントの高さになります。

  Arguments:    pFont:  フォントへのポインタ。

  Returns:      フォントの高さ。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE u8 NNS_G2dFontGetHeight( const NNSG2dFont* pFont )
{
    NNS_G2D_FONT_ASSERT(pFont);
    return pFont->pRes->pGlyph->cellHeight;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetCellHeight

  Description:  グリフイメージの高さを取得します。
                グリフイメージの大きさはフォント内の全ての文字で共通です。

  Arguments:    pFont:  フォントへのポインタ。

  Returns:      グリフイメージの高さ。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE u8 NNS_G2dFontGetCellHeight( const NNSG2dFont* pFont )
{
    NNS_G2D_FONT_ASSERT(pFont);
    return pFont->pRes->pGlyph->cellHeight;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetCellWidth

  Description:  グリフイメージの幅を取得します。
                グリフイメージの大きさはフォント内の全ての文字で共通です。

  Arguments:    pFont:  フォントへのポインタ。

  Returns:      グリフイメージの幅。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE u8 NNS_G2dFontGetCellWidth( const NNSG2dFont* pFont )
{
    NNS_G2D_FONT_ASSERT(pFont);
    return pFont->pRes->pGlyph->cellWidth;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetBaselinePos

  Description:  グリフイメージの上端からみたベースラインの位置を取得します。

  Arguments:    pFont:  フォントへのポインタ。

  Returns:      グリフイメージの上端を 0 としたベースラインの位置
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE int NNS_G2dFontGetBaselinePos( const NNSG2dFont* pFont )
{
    NNS_G2D_FONT_ASSERT( pFont );
    return pFont->pRes->pGlyph->baselinePos;
}



/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dFontGetGlyphDataSize

  Description:  グリフイメージの1文字あたりのバイトサイズを取得します。
                グリフイメージの大きさはフォント内の全ての文字で共通です。

                (CellWidth * CellHeight * bpp + 7) / 8 と等しくなります。

  Arguments:    pFont:  フォントへのポインタ。

  Returns:      グリフイメージの1文字あたりのバイトサイズ。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE int NNSi_G2dFontGetGlyphDataSize( const NNSG2dFont* pFont )
{
    NNS_G2D_FONT_ASSERT(pFont);
    return pFont->pRes->pGlyph->cellSize;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetMaxCharWidth

  Description:  フォント中の最大文字幅を取得します。

  Arguments:    pFont:  フォントへのポインタ。

  Returns:      フォント中の最大文字幅。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE u8 NNS_G2dFontGetMaxCharWidth( const NNSG2dFont* pFont )
{
    NNS_G2D_FONT_ASSERT(pFont);
    return pFont->pRes->pGlyph->maxCharWidth;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetBpp

  Description:  グリフイメージの1ドットあたりのビット数を取得します。

  Arguments:    pFont:  フォントへのポインタ。

  Returns:      グリフイメージの1ドットあたりのビット数。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE u8 NNS_G2dFontGetBpp( const NNSG2dFont* pFont )
{
    NNS_G2D_FONT_ASSERT(pFont);
    return pFont->pRes->pGlyph->bpp;
}



/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dFontGetEncoding

  Description:  フォントが対応する文字列エンコーディングを取得します。

  Arguments:    pFont:  フォントへのポインタ。

  Returns:      フォントが対応する文字列エンコーディング。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE NNSG2dFontEncoding NNSi_G2dFontGetEncoding( const NNSG2dFont* pFont )
{
    NNS_G2D_FONT_ASSERT(pFont);
    return (NNSG2dFontEncoding)pFont->pRes->encoding;
}



/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dFontGetSpliter

  Description:  文字列エンコーディング処理コールバック関数へのポインタを
                取得します。

  Arguments:    pFont:  フォントへのポインタ。

  Returns:      文字列エンコーディング処理コールバック関数へのポインタ。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE NNSiG2dSplitCharCallback NNSi_G2dFontGetSpliter( const NNSG2dFont* pFont )
{
    NNS_G2D_FONT_ASSERT(pFont);
    return pFont->cbCharSpliter;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetGlyphImageFromIndex

  Description:  グリフインデックスからグリフイメージを取得します。

  Arguments:    pFont:  フォントへのポインタ。
                idx:    グリフインデックス。

  Returns:      グリフイメージへのポインタ。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE const u8* NNS_G2dFontGetGlyphImageFromIndex(
                                        const NNSG2dFont* pFont, u16 idx )
{
    NNS_G2D_FONT_ASSERT(pFont);
    NNS_G2D_ASSERT( idx < NNS_G2D_FONT_MAX_GLYPH_INDEX(pFont) );
    return pFont->pRes->pGlyph->glyphTable + idx * NNSi_G2dFontGetGlyphDataSize(pFont);
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetFlags

  Description:  グリフの特徴フラグを取得します。

  Arguments:    pFont:  フォントへのポインタ。

  Returns:      グリフの特徴フラグを返します。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE u8 NNS_G2dFontGetFlags(const NNSG2dFont* pFont)
{
    NNS_G2D_FONT_ASSERT(pFont);
    return pFont->pRes->pGlyph->flags;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontSetLineFeed

  Description:  フォントの改行幅を設定します。

  Arguments:    pFont:      フォントへのポインタ。
                linefeed:   新しい改行幅

  Returns:      フォントの改行幅。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dFontSetLineFeed( NNSG2dFont* pFont, s8 linefeed )
{
    NNS_G2D_FONT_ASSERT(pFont);
    pFont->pRes->linefeed = linefeed;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontSetDefaultCharWidths

  Description:  デフォルトの文字幅情報を設定します。

  Arguments:    pFont:  フォントへのポインタ。
                gw:     新しいデフォルト文字幅情報。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dFontSetDefaultCharWidths(
                                NNSG2dFont* pFont, NNSG2dCharWidths cw)
{
    NNS_G2D_FONT_ASSERT(pFont);
    pFont->pRes->defaultWidth = cw;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontSetAlternateGlyphIndex

  Description:  フォントの代替文字グリフインデックスを設定します。

  Arguments:    pFont:  フォントへのポインタ。
                idx:    新しい代替文字グリフインデックス。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dFontSetAlternateGlyphIndex( NNSG2dFont* pFont, u16 idx )
{
    NNS_G2D_FONT_ASSERT(pFont);
    NNS_G2D_ASSERT( idx < NNS_G2D_FONT_MAX_GLYPH_INDEX(pFont) );
    pFont->pRes->alterCharIndex = idx;
}





//*****************************************************************************
// inline function
//*****************************************************************************

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetGlyphIndex

  Description:  文字コードからグリフインデックスを取得します。

  Arguments:    pFont:  フォントへのポインタ。
                c:      文字コード。

  Returns:      グリフインデックス。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE u16 NNS_G2dFontGetGlyphIndex( const NNSG2dFont* pFont, u16 c )
{
    const u16 idx = NNS_G2dFontFindGlyphIndex(pFont, c);

    return (idx != NNS_G2D_GLYPH_INDEX_NOT_FOUND) ?
                idx :
                pFont->pRes->alterCharIndex ;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetGlyphFromIndex

  Description:  グリフインデックスからグリフを取得します。

  Arguments:    pGlyph: グリフを格納するバッファへのポインタ。
                pFont:  フォントへのポインタ。
                idx:    取得するグリフのグリフインデックス。

  Returns:      グリフへのポインタ。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dFontGetGlyphFromIndex(
                        NNSG2dGlyph* pGlyph, const NNSG2dFont* pFont, u16 idx )
{
    NNS_G2D_FONT_ASSERT( pFont );
    NNS_G2D_POINTER_ASSERT( pGlyph );
    pGlyph->pWidths = NNS_G2dFontGetCharWidthsFromIndex(pFont, idx);
    pGlyph->image = NNS_G2dFontGetGlyphImageFromIndex(pFont, idx);
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetCharWidths

  Description:  文字コードから文字幅情報を取得します。

  Arguments:    pFont:  フォントへのポインタ。
                c:      文字幅情報を取得する文字の文字コード

  Returns:      文字幅情報。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE const NNSG2dCharWidths* NNS_G2dFontGetCharWidths(
                                            const NNSG2dFont* pFont, u16 c )
{
    u16 iGlyph;

    NNS_G2D_FONT_ASSERT( pFont );

    iGlyph = NNS_G2dFontGetGlyphIndex( pFont, c );
    return NNS_G2dFontGetCharWidthsFromIndex( pFont, iGlyph );
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetCharWidth

  Description:  文字コードから文字幅を取得します。

  Arguments:    pFont:  フォントへのポインタ。
                c:      文字幅を取得する文字の文字コード

  Returns:      ピクセル単位の文字幅。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE int NNS_G2dFontGetCharWidth( const NNSG2dFont* pFont, u16 c )
{
    const NNSG2dCharWidths* pWidths;

    NNS_G2D_FONT_ASSERT( pFont );

    pWidths = NNS_G2dFontGetCharWidths(pFont, c);
    return pWidths->charWidth;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetCharWidthFromIndex

  Description:  グリフインデックスから文字幅を取得します。

  Arguments:    pFont:  フォントへのポインタ。
                idx:    文字幅を取得する文字のグリフインデックス

  Returns:      ピクセル単位の文字幅。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE int NNS_G2dFontGetCharWidthFromIndex( const NNSG2dFont* pFont, u16 idx )
{
    const NNSG2dCharWidths* pWidths;

    NNS_G2D_FONT_ASSERT( pFont );

    pWidths = NNS_G2dFontGetCharWidthsFromIndex(pFont, idx);
    return pWidths->charWidth;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetGlyphImage

  Description:  文字コードからグリフイメージを取得します。

  Arguments:    pFont:  フォントへのポインタ。

  Returns:      グリフイメージへのポインタ。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE const u8* NNS_G2dFontGetGlyphImage( const NNSG2dFont* pFont, u16 c )
{
    u16 iGlyph;

    NNS_G2D_FONT_ASSERT( pFont );

    iGlyph = NNS_G2dFontGetGlyphIndex( pFont, c );
    return NNS_G2dFontGetGlyphImageFromIndex( pFont, iGlyph );
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetGlyph

  Description:  文字コードからグリフを取得します。

  Arguments:    pGlyph: グリフを格納するバッファへのポインタ。
                pFont:  フォントへのポインタ。
                ccode:  グリフを取得する文字の文字コード

  Returns:      グリフへのポインタ。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dFontGetGlyph(
                    NNSG2dGlyph* pGlyph, const NNSG2dFont* pFont, u16 ccode )
{
    u16 iGlyph;

    NNS_G2D_FONT_ASSERT( pFont );
    NNS_G2D_POINTER_ASSERT( pGlyph );

    iGlyph = NNS_G2dFontGetGlyphIndex(pFont, ccode);
    NNS_G2dFontGetGlyphFromIndex(pGlyph, pFont, iGlyph);
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontSetAlternateChar

  Description:  代替文字を指定した文字コードの文字に置き換えます。

  Arguments:    pFont:  フォントへのポインタ。
                c:      新しい代替文字の文字コード。

  Returns:      代替文字の置き換えに成功すれば TRUE。
                フォント内に c に対応する文字が存在しない場合に失敗します。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE BOOL NNS_G2dFontSetAlternateChar( NNSG2dFont* pFont, u16 c )
{
    u16 iGlyph;

    NNS_G2D_FONT_ASSERT(pFont);

    iGlyph = NNS_G2dFontFindGlyphIndex(pFont, c);

    if( iGlyph == NNS_G2D_GLYPH_INDEX_NOT_FOUND )
    {
        return FALSE;
    }

    pFont->pRes->alterCharIndex = iGlyph;
    return TRUE;
}






/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetCharWidthFromIndex

  Description:  代替文字を指定した文字コードの文字に置き換えます。

  Arguments:    pFont:  フォントへのポインタ。
                c:      新しい代替文字の文字コード。

  Returns:      代替文字の置き換えに成功すれば TRUE。
                フォント内に c に対応する文字が存在しない場合に失敗します。
 *---------------------------------------------------------------------------*/





//*****************************************************************************
// inline function
//*****************************************************************************

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dFontGetStringWidth

  Description:  指定された条件で文字列を1行描画した場合の幅を求めます。
                同時に行の終端位置のポインタを取得します。

  Arguments:    pFont:      文字列幅の計算に用いるフォントへのポインタ。
                hSpace:     ピクセル単位の文字と文字の間隔。
                str:        文字列幅を求める文字列へのポインタ。
                pPos:       行終端位置へのポインタを受け取るバッファへのポインタ。
                            NULL指定可。

  Returns:      str から1行分の文字列を描画した場合のピクセル単位での幅。
 *---------------------------------------------------------------------------*/
int NNSi_G2dFontGetStringWidth(
    const NNSG2dFont* pFont,
    int hSpace,
    const void* str,
    const void** pPos
);



/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dFontGetTextHeight

  Description:  指定された条件で文字列を描画した場合の高さを求めます。

  Arguments:    pFont:  文字列幅の計算に用いるフォントへのポインタ。
                vSpace: ピクセル単位の行と行の間隔。
                txt:    文字列幅を求める文字列へのポインタ。

  Returns:      文字列を描画した場合のピクセル単位での高さ。
 *---------------------------------------------------------------------------*/
int NNSi_G2dFontGetTextHeight(
    const NNSG2dFont* pFont,
    int vSpace,
    const void* txt
);



/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dFontGetTextWidth

  Description:  指定された条件で文字列を描画した場合の幅を求めます。

  Arguments:    pFont:  文字列幅の計算に用いるフォントへのポインタ。
                hSpace: ピクセル単位の文字と文字の間隔。
                txt:    文字列幅を求める文字列へのポインタ。

  Returns:      文字列を描画した場合のピクセル単位での幅。
                これは各行の幅のうち最大のものを意味します。
 *---------------------------------------------------------------------------*/
int NNSi_G2dFontGetTextWidth(
    const NNSG2dFont* pFont,
    int hSpace,
    const void* txt
);



/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dFontGetTextRect

  Description:  指定された条件で文字列を描画した場合の幅と高さを求めます。

  Arguments:    pFont:  文字列幅の計算に用いるフォントへのポインタ。
                hSpace: ピクセル単位の文字と文字の間隔。
                vSpace: ピクセル単位の行と行の間隔。
                txt:    文字列幅を求める文字列へのポインタ。

  Returns:      文字列を描画した場合のピクセル単位での幅と高さを格納した
                構造体。
 *---------------------------------------------------------------------------*/
NNSG2dTextRect NNSi_G2dFontGetTextRect(
    const NNSG2dFont* pFont,
    int hSpace,
    int vSpace,
    const void* txt
);



//---------------------------------------------------------------------

NNS_G2D_INLINE int NNS_G2dFontGetStringWidth( const NNSG2dFont* pFont, int hSpace, const NNSG2dChar* str, const NNSG2dChar** pPos )
    { return NNSi_G2dFontGetStringWidth(pFont, hSpace, str, (const void**)pPos); }
NNS_G2D_INLINE int NNS_G2dFontGetTextHeight( const NNSG2dFont* pFont, int vSpace, const NNSG2dChar* txt )
    { return NNSi_G2dFontGetTextHeight(pFont, vSpace, txt); }
NNS_G2D_INLINE int NNS_G2dFontGetTextWidth( const NNSG2dFont* pFont, int hSpace, const NNSG2dChar* txt )
    { return NNSi_G2dFontGetTextWidth(pFont, hSpace, txt); }
NNS_G2D_INLINE NNSG2dTextRect NNS_G2dFontGetTextRect( const NNSG2dFont* pFont, int hSpace, int vSpace, const NNSG2dChar* txt )
    { return NNSi_G2dFontGetTextRect(pFont, hSpace, vSpace, txt); }

//---------------------------------------------------------------------




//---------------------------------------------------------------------

#ifdef __cplusplus
}/* extern "C" */
#endif

#endif // G2D_FONT_H_

