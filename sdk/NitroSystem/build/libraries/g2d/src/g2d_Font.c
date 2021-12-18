/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d
  File:     g2d_Font.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.10 $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nnsys/g2d/g2d_Font.h>
#include <nnsys/g2d/load/g2d_NFT_load.h>
#include <nnsys/g2d/fmt/g2d_Font_data.h>
#include <nnsys/g2d/g2d_config.h>



//****************************************************************************
// Local Functions
//****************************************************************************

/*---------------------------------------------------------------------------*
  Name:         GetGlyphIndex

  Description:  文字コードマップブロックから c に対応するグリフインデックスを
                取得します。

  Arguments:    pMap:   フォントの文字コードマップブロック。
                c:      文字コード。

  Returns:      c に対応するグリフインデックス。
                存在しない場合は NNS_G2D_GLYPH_INDEX_NOT_FOUND。
 *---------------------------------------------------------------------------*/
static u16 GetGlyphIndex(const NNSG2dFontCodeMap* pMap, u16 c)
{
    u16 index = NNS_G2D_GLYPH_INDEX_NOT_FOUND;

    NNS_G2D_POINTER_ASSERT( pMap );
    NNS_G2D_ASSERT(pMap->ccodeBegin <= c && c <= pMap->ccodeEnd);

    switch( pMap->mappingMethod )
    {
    //-----------------------------------------------------------
    // インデックス = 文字コード - オフセット
    case NNS_G2D_MAPMETHOD_DIRECT:
        {
            u16 offset = pMap->mapInfo[0];
            index = (u16)(c - pMap->ccodeBegin + offset);
        }
        break;

    //-----------------------------------------------------------
    // インデックス = table[文字コード - 文字コードオフセット]
    case NNS_G2D_MAPMETHOD_TABLE:
        {
            const int table_index = c - pMap->ccodeBegin;

            index = pMap->mapInfo[table_index];
        }
        break;

    //-----------------------------------------------------------
    // インデックス = 二分探索(文字コード)
    case NNS_G2D_MAPMETHOD_SCAN:
        {
            const NNSG2dCMapInfoScan* const ws = (NNSG2dCMapInfoScan*)(pMap->mapInfo);
            const NNSG2dCMapScanEntry* st = &(ws->entries[0]);
            const NNSG2dCMapScanEntry* ed = &(ws->entries[ws->num - 1]);

            while( st <= ed )
            {
                const NNSG2dCMapScanEntry* md = st + (ed - st) / 2;

                if( md->ccode < c )
                {
                    st = md + 1;
                }
                else if( c < md->ccode )
                {
                    ed = md - 1;
                }
                else
                {
                    index = md->index;
                    break;
                }
            }
        }
        break;

    //-----------------------------------------------------------
    // unknown
    default:
        NNS_G2D_ASSERTMSG(FALSE, "unknwon MAPMETHOD");
    }

    return index;
}



/*---------------------------------------------------------------------------*
  Name:         GetCharWidthsFromIndex

  Description:  文字幅ブロックからグリフインデックスに対応する文字幅情報を
                取得します。

  Arguments:    pWidth: フォントの文字幅ブロックへのポインタ。
                idx:    グリフインデックス。

  Returns:      idx に対応する文字幅情報。
                idx が pWidth に含まれないインデックスである場合の動作は不定です。
 *---------------------------------------------------------------------------*/
static NNS_G2D_INLINE const NNSG2dCharWidths* GetCharWidthsFromIndex( const NNSG2dFontWidth* const pWidth, int idx )
{
    NNS_G2D_POINTER_ASSERT( pWidth );
    NNS_G2D_ASSERT(pWidth->indexBegin <= idx && idx <= pWidth->indexEnd);

    return (NNSG2dCharWidths*)(pWidth->widthTable) + (idx - pWidth->indexBegin);
}





//*******************************************************************************
// Global Functions
//*******************************************************************************

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontInit*

  Description:  フォントリソースからフォントを構築します。

  Arguments:    pFont:      フォントへのポインタ。
                pNftrFile:  フォントリソースへのポインタ。

  Returns:      フォントの構築に成功した場合はFALSE以外を返します。
 *---------------------------------------------------------------------------*/

void NNS_G2dFontInitAuto(NNSG2dFont* pFont, void* pNftrFile)
{
    const static NNSiG2dSplitCharCallback spliterTable[] =
    {
        NNSi_G2dSplitCharUTF8,
        NNSi_G2dSplitCharUTF16,
        NNSi_G2dSplitCharShiftJIS,
        NNSi_G2dSplitChar1Byte
    };
    NNSG2dFontEncoding encoding;
    BOOL result;

    NNS_G2D_POINTER_ASSERT( pFont );
    NNS_G2D_POINTER_ASSERT( pNftrFile );

    result = NNSi_G2dGetUnpackedFont(pNftrFile, &(pFont->pRes));
    NNS_G2D_ASSERTMSG(result, "Faild to unpack font.");
    encoding = (NNSG2dFontEncoding)pFont->pRes->encoding;
    NNS_G2D_MINMAX_ASSERT( encoding, 0, NNS_G2D_NUM_OF_ENCODING - 1 );

    pFont->cbCharSpliter = spliterTable[encoding];
}


#define NNS_G2D_DEFINE_FONT_INIT(name, enc, spliter)                        \
    void NNS_G2dFontInit##name(NNSG2dFont* pFont, void* pNftrFile)          \
    {                                                                       \
        BOOL result;                                                        \
        NNS_G2D_POINTER_ASSERT( pFont );                                    \
        NNS_G2D_POINTER_ASSERT( pNftrFile );                                \
                                                                            \
        result = NNSi_G2dGetUnpackedFont(pNftrFile, &(pFont->pRes));        \
        NNS_G2D_ASSERTMSG(result, "Faild to unpack font.");                 \
        NNS_G2D_ASSERT(  (NNSG2dFontEncoding)pFont->pRes->encoding          \
                                    == NNS_G2D_FONT_ENCODING_##enc );       \
                                                                            \
        pFont->cbCharSpliter = NNSi_G2dSplitChar##spliter;                  \
    }

NNS_G2D_DEFINE_FONT_INIT(UTF8, UTF8, UTF8)
NNS_G2D_DEFINE_FONT_INIT(UTF16, UTF16, UTF16)
NNS_G2D_DEFINE_FONT_INIT(ShiftJIS, SJIS, ShiftJIS)
NNS_G2D_DEFINE_FONT_INIT(CP1252, CP1252, 1Byte)



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontFindGlyphIndex

  Description:  文字コードから対応するグリフのインデックスを取得します。

  Arguments:    pFont:  フォントへのポインタ。
                c:      グリフインデックスを取得する文字コード。

  Returns:      c に対応するグリフが存在する場合はそのインデックス。
                存在しない場合は NNS_G2D_GLYPH_INDEX_NOT_FOUND。
 *---------------------------------------------------------------------------*/
u16 NNS_G2dFontFindGlyphIndex( const NNSG2dFont* pFont, u16 c )
{
    const NNSG2dFontCodeMap* pMap;

    NNS_G2D_FONT_ASSERT(pFont);

    pMap = pFont->pRes->pMap;

    // CMAPブロックのリストを線形探索
    while( pMap != NULL )
    {
        if( (pMap->ccodeBegin <= c) && (c <= pMap->ccodeEnd) )
        {
            return GetGlyphIndex(pMap, c);
        }

        pMap = pMap->pNext;
    }

    // 見つからなかった
    return NNS_G2D_GLYPH_INDEX_NOT_FOUND;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetCharWidthsFromIndex

  Description:  グリフインデックスから文字幅情報を得ます。

  Arguments:    pFont:  フォントへのポインタ。
                idx:    グリフインデックス。

  Returns:      idx に対応する文字幅情報が存在する場合は
                フォント内に格納されている文字幅情報へのポインタ。
                存在しない場合はフォントのデフォルト幅情報へのポインタ。
 *---------------------------------------------------------------------------*/
const NNSG2dCharWidths* NNS_G2dFontGetCharWidthsFromIndex( const NNSG2dFont* pFont, u16 idx )
{
    const NNSG2dFontWidth* pWidth;

    NNS_G2D_FONT_ASSERT(pFont);

    pWidth = pFont->pRes->pWidth;

    // 幅情報ブロックのリストを線形探索
    while( pWidth != NULL )
    {
        if( (pWidth->indexBegin <= idx) && (idx <= pWidth->indexEnd) )
        {
            return GetCharWidthsFromIndex(pWidth, idx);

        }

        pWidth = pWidth->pNext;
    }

    // 見つからなければデフォルトを返す。
    return &(pFont->pRes->defaultWidth);
}















//----------------------------------------------------------------------------
// サイズ取得
//----------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetStringWidth

  Description:  文字列を1行分描画した場合の幅を求めます。
                i.e. txt から改行文字もしくは終端文字までを描画した場合の
                文字列幅を求めます。

  Arguments:    pTxn:   テキストスクリーン構造体へのポインタ。
                txt:    NUL終端文字列へのポインタ。
                pPos:   txt が改行文字を含む場合に最初の改行文字の次の文字への
                        ポインタを格納するバッファへのポインタ。
                        txt が改行文字を含まない場合は NULL が格納されます。
                        不要な場合は NULL を指定することができます。

  Returns:      txt の幅。
 *---------------------------------------------------------------------------*/
int NNSi_G2dFontGetStringWidth(
    const NNSG2dFont* pFont,
    int hSpace,
    const void* str,
    const void** pPos
)
{
    int width = 0;
    const void* pos = str;
    u16 c;
    NNSiG2dSplitCharCallback getNextChar;

    NNS_G2D_FONT_ASSERT( pFont );
    NNS_G2D_POINTER_ASSERT( str );

    getNextChar = NNSi_G2dFontGetSpliter(pFont);

    while( (c = getNextChar((const void**)&pos)) != 0 )
    {
        if( c == '\n' )
        {
            break;
        }

        width += NNS_G2dFontGetCharWidth(pFont, c) + hSpace;
    }

    if( pPos != NULL )
    {
        *pPos = (c == '\n') ? pos: NULL;
    }
    if( width > 0 )
    {
        width -= hSpace;
    }
    return width;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetTextHeight

  Description:  改行を含む文字列を描画した場合の高さを求めます。

  Arguments:    pTxn:   テキストスクリーン構造体へのポインタ。
                txt:    NUL終端文字列へのポインタ。

  Returns:      txt の高さ。
 *---------------------------------------------------------------------------*/
int NNSi_G2dFontGetTextHeight(
    const NNSG2dFont* pFont,
    int vSpace,
    const void* txt
)
{
    const void* pos = txt;
    int lines = 1;
    NNSG2dTextRect rect = {0, 0};
    u16 c;
    NNSiG2dSplitCharCallback getNextChar;

    NNS_G2D_FONT_ASSERT( pFont );
    NNS_G2D_POINTER_ASSERT( txt );

    getNextChar = NNSi_G2dFontGetSpliter(pFont);

    while( (c = getNextChar((const void**)&pos)) != 0 )
    {
        if( c == '\n' )
        {
            lines++;
        }
    }

    return lines * (NNS_G2dFontGetLineFeed(pFont) + vSpace) - vSpace;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetTextWidth

  Description:  文字列を描画した場合の幅を求めます。
                これは各行の幅の最大値になります。

  Arguments:    pTxn:   計算の基準となる Font へのポインタ。
                txt:    文字列。

  Returns:      文字列を描画した場合の幅。
 *---------------------------------------------------------------------------*/
int NNSi_G2dFontGetTextWidth(
    const NNSG2dFont* pFont,
    int hSpace,
    const void* txt
)
{
    int width = 0;

    NNS_G2D_FONT_ASSERT( pFont );
    NNS_G2D_POINTER_ASSERT( txt );

    while( txt != NULL )
    {
        const int line_width = NNSi_G2dFontGetStringWidth(pFont, hSpace, txt, &txt);
        if( line_width > width )
        {
            width = line_width;
        }
    }

    return width;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetTextRect

  Description:  改行を含む文字列を描画した場合の最大幅と高さを求めます。

  Arguments:    pTxn:   テキストスクリーン構造体へのポインタ。
                txt:    NUL終端文字列へのポインタ。

  Returns:      txt を内包する最小の矩形の幅と高さ。
 *---------------------------------------------------------------------------*/
NNSG2dTextRect NNSi_G2dFontGetTextRect(
    const NNSG2dFont* pFont,
    int hSpace,
    int vSpace,
    const void* txt
)
{
    int lines = 1;
    NNSG2dTextRect rect = {0, 0};

    NNS_G2D_FONT_ASSERT( pFont );
    NNS_G2D_POINTER_ASSERT( txt );

    while( txt != NULL )
    {
        const int width = NNSi_G2dFontGetStringWidth(pFont, hSpace, txt, &txt);
        if( width > rect.width )
        {
            rect.width = width;
        }
        lines++;
    }

    // 高さ = 行数 x (行高 + 行間) - 行間
    rect.height = ((lines - 1) * (NNS_G2dFontGetLineFeed(pFont) + vSpace) - vSpace);

    return rect;
}

