/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d - fmt
  File:     g2d_Font_data.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.10 $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_FONT_DATA_H_
#define NNS_G2D_FONT_DATA_H_

//#include <nitro/types.h>
#include <nnsys/g2d/fmt/g2d_Common_data.h>

#ifdef _MSC_VER // for VC
    #pragma warning( disable: 4200 )
	#pragma warning( disable: 4201 )
#endif  //_MSC_VER

#ifndef SDK_ARM9
    #include <nitro_win32.h>
#endif //!SDK_ARM9

#ifdef __cplusplus
extern "C" {
#endif



//------------------------------------------------------------------------------
// シグネチャ定義
//------------------------------------------------------------------------------

// Binary file signature
#define NNS_G2D_BINFILE_SIG_FONTDATA          (u32)'NFTR'

// Binary block signature
#define NNS_G2D_BINBLK_SIG_FINFDATA           (u32)'FINF'
#define NNS_G2D_BINBLK_SIG_CGLPDATA           (u32)'CGLP'
#define NNS_G2D_BINBLK_SIG_TGLPDATA           (u32)'TGLP'
#define NNS_G2D_BINBLK_SIG_TCLPDATA           (u32)'TCLP'
#define NNS_G2D_BINBLK_SIG_CWDHDATA           (u32)'CWDH'
#define NNS_G2D_BINBLK_SIG_CMAPDATA           (u32)'CMAP'


#define NNS_G2D_BINFILE_EXT_FONTDATA          "NFTR"





//------------------------------------------------------------------------------
// リソースバージョン定義
//------------------------------------------------------------------------------

//
// バージョン情報
// Ver         変更点
// -------------------------------------
// 1.0         初版
// 1.1         回転/縦書きの情報を追加
//
#define NNS_G2D_NFTR_MAJOR_VER              (u8)1
#define NNS_G2D_NFTR_MINOR_VER              (u8)1

#define NNS_G2D_NFTR_VER        (u16)((NNS_G2D_NFTR_MAJOR_VER << 8) | NNS_G2D_NFTR_MINOR_VER)




//------------------------------------------------------------------------------
// enum 定義
//------------------------------------------------------------------------------

// フォントタイプ
typedef enum NNSG2dFontType
{
    NNS_G2D_FONTTYPE_GLYPH,     // BMP
    NNS_G2D_NUM_OF_FONTTYPE
}
NNSG2dFontType;

// 文字列エンコーディングタイプ
typedef enum NNSG2dFontEncoding
{
    NNS_G2D_FONT_ENCODING_UTF8,			// UTF-8
    NNS_G2D_FONT_ENCODING_UTF16,		// UTF-16
    NNS_G2D_FONT_ENCODING_SJIS,			// ShiftJIS
	NNS_G2D_FONT_ENCODING_CP1252,	    // CP1252
    NNS_G2D_NUM_OF_ENCODING
}
NNSG2dFontEncoding;

// 文字コードマップタイプ
typedef enum NNSG2dFontMappingMethod
{
    NNS_G2D_MAPMETHOD_DIRECT,       // インデックス = 文字コード - オフセット
    NNS_G2D_MAPMETHOD_TABLE,        // インデックス = mapInfo[文字コード - オフセット]
    NNS_G2D_MAPMETHOD_SCAN,         // インデックス = search(mapInfo, 文字コード)
    NNS_G2D_NUM_OF_MAPMETHOD
}
NNSG2dFontMappingMethod;

// 縦書きと回転に関するフラグ
typedef enum NNSG2dFontGlyphFlag
{
    NNS_G2D_FONT_FLAG_TBRL      = (1 << 0),     // 縦書き用のフォント
    NNS_G2D_FONT_FLAG_ROT_0     = (0 << 1),     // 時計回り   0 度回転のフォント
    NNS_G2D_FONT_FLAG_ROT_90    = (1 << 1),     // 時計回り  90 度回転のフォント
    NNS_G2D_FONT_FLAG_ROT_180   = (2 << 1),     // 時計回り 180 度回転のフォント
    NNS_G2D_FONT_FLAG_ROT_270   = (3 << 1),     // 時計回り 270 度回転のフォント
    NNS_G2D_FONT_FLAG_ROT_MASK  = (3 << 1)      // 回転マスク
}
NNSG2dFontGlyphFlag;


//------------------------------------------------------------------------------
// 構造体定義
//------------------------------------------------------------------------------
// 文字幅情報
typedef struct NNSG2dCharWidths
{
    s8 left;                            // 文字の左スペースの幅
    u8 glyphWidth;                      // 文字のグリフ幅
    s8 charWidth;                       // 文字の幅 = 左スペース幅 + グリフ幅 + 右スペース幅
}
NNSG2dCharWidths;

// 文字コードとグリフインデックスペア
typedef struct NNSG2dCMapScanEntry
{
    u16 ccode;                          // 文字コード
    u16 index;                          // グリフインデックス
}
NNSG2dCMapScanEntry;

// NNS_G2D_MAPMETHOD_SCAN の場合の NNSG2dFontCodeMap.mapInfo
typedef struct NNSG2dCMapInfoScan
{
    u16 num;                            // entries の要素数
    NNSG2dCMapScanEntry entries[];      // 文字コードからグリフインデックスへのマッピングリスト
}
NNSG2dCMapInfoScan;





//------------------------------------------------------------------------------
// フォントリソース
//------------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         NNSG2dFontGlyph

  Description:  フォントのグリフBMPを格納します。
                全てのグリフBMPは幅と高さが等しく、ゆえにデータサイズも
                等しいため配列としてアクセスできます。
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dFontGlyph
{
    u8  cellWidth;          // グリフイメージ1ラインのピクセル数
    u8  cellHeight;         // グリフイメージのライン数
    u16 cellSize;           // 1グリフあたりのグリフイメージデータサイズ
    s8  baselinePos;        // (*)グリフイメージ上端からベースラインまでの距離
    u8  maxCharWidth;       // (*)最大文字幅
    u8  bpp;                // グリフイメージ1ピクセルあたりのbit数
    u8  flags;              // NNSG2dFontGlyphFlag

    u8  glyphTable[];       // グリフイメージの配列
}
NNSG2dFontGlyph;



/*---------------------------------------------------------------------------*
  Name:         NNSG2dFontWidth

  Description:  各文字の文字幅情報を格納します。
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dFontWidth
{
    u16                     indexBegin;     // widthTable の最初のエントリが対応するグリフインデックス
    u16                     indexEnd;       // widthTable の最後のエントリが対応するグリフインデックス
    struct NNSG2dFontWidth* pNext;          // 次の NNSG2dFontWidth へのポインタ
    NNSG2dCharWidths        widthTable[];   // 幅情報の配列
}
NNSG2dFontWidth;



/*---------------------------------------------------------------------------*
  Name:         NNSG2dFontCodeMap

  Description:  文字コードから、文字コードに対応するグリフの
                グリフイメージ配列中のインデックスへのマッピングを規定します。
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dFontCodeMap
{
    u16                         ccodeBegin;     // このブロックが担当する文字コード範囲の最初の文字コード
    u16                         ccodeEnd;       // このブロックが担当する文字コード範囲の最後の文字コード
    u16                         mappingMethod;  // マッピング方法 (NNSG2dFontMappingMethod型)
    u16                         reserved;       // 予約
    struct NNSG2dFontCodeMap*   pNext;          // 次の NNSG2dFontCodeMap へのポインタ
    u16                         mapInfo[];      // 文字コードマッピング情報 具体的な内容は mappingMethod に依る
}
NNSG2dFontCodeMap;



/*---------------------------------------------------------------------------*
  Name:         NNSG2dFontInformation

  Description:  フォント全体に渡る情報を格納します。
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dFontInformation
{
    u8                  fontType;       // グリフデータタイプ (NNSG2dFontType型)
    s8                  linefeed;       // (*)== leading
    u16                 alterCharIndex; // (*)フォントに含まれない文字用のグリフのグリフインデックス
    NNSG2dCharWidths    defaultWidth;   // (*)文字幅情報を持たないグリフ用の文字幅情報
    u8                  encoding;       // 対応する文字列エンコーディング (NNSG2dFontEncoding型)
    NNSG2dFontGlyph*    pGlyph;         // 唯一の NNSG2dFontGlyph へのポインタ
    NNSG2dFontWidth*    pWidth;         // 最初の NNSG2dFontWidth へのポインタ
    NNSG2dFontCodeMap*  pMap;           // 最初の NNSG2dFontCodeMap へのポインタ
}
NNSG2dFontInformation;




//------------------------------------------------------------------------------
// NITRO-System バイナリファイル ブロック
//------------------------------------------------------------------------------

typedef struct NNSG2dFontInformationBlock
{
    NNSG2dBinaryBlockHeader     blockHeader;
    NNSG2dFontInformation       blockBody;
}
NNSG2dFontInformationBlock;

typedef struct NNSG2dFontGlyphBlock
{
    NNSG2dBinaryBlockHeader     blockHeader;
    NNSG2dFontGlyph             blockBody;
}
NNSG2dFontGlyphBlock;

typedef struct NNSG2dFontWidthBlock
{
    NNSG2dBinaryBlockHeader     blockHeader;
    NNSG2dFontWidth             blockBody;
}
NNSG2dFontWidthBlock;

typedef struct NNSG2dFontCodeMapBlock
{
    NNSG2dBinaryBlockHeader     blockHeader;
    NNSG2dFontCodeMap           blockBody;
}
NNSG2dFontCodeMapBlock;

//------------------------------------------------------------------------------


#ifdef NNS_G2D_FONT_USE_OLD_RESOURCE

#define NNS_G2D_NFTR_PREV_MAJOR_VER         (u8)1
#define NNS_G2D_NFTR_PREV_MINOR_VER         (u8)0

#define NNS_G2D_NFTR_PREV_VER   (u16)((NNS_G2D_NFTR_PREV_MAJOR_VER << 8) | NNS_G2D_NFTR_PREV_MINOR_VER)

#endif  // NNS_G2D_FONT_USE_OLD_RESOURCE




#ifdef __cplusplus
} /* extern "C" */
#endif

#ifdef _MSC_VER
    #pragma warning( default: 4200 )
    #pragma warning( default: 4201 )
#endif  //_MSC_VER

#endif // NNS_G2D_FONT_DATA_H_
