/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d
  File:     g2d_Screen.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.21 $
 *---------------------------------------------------------------------------*/

#include <nnsys/g2d/g2d_Screen.h>
#include <nnsys/g2d/g2d_config.h>
#include "g2di_Dma.h"
#include "g2di_BGManipulator.h"


#define BG_MODE_WARNING 8

// 拡張パレットスロットのサイズ
#define NNS_G2D_BGEXTPLTT_SLOTSIZE  0x2000

//---- テキストBGでの単位プレーンのサイズ（キャラクタ単位）
#define PLANE_WIDTH     32
#define PLANE_HEIGHT    32


// BG 拡張パレットスロットを表す列挙子
typedef enum NNSG2dBGExtPlttSlot
{
    NNS_G2D_BGEXTPLTTSLOT_MAIN0,
    NNS_G2D_BGEXTPLTTSLOT_MAIN1,
    NNS_G2D_BGEXTPLTTSLOT_MAIN2,
    NNS_G2D_BGEXTPLTTSLOT_MAIN3,
    NNS_G2D_BGEXTPLTTSLOT_SUB0,
    NNS_G2D_BGEXTPLTTSLOT_SUB1,
    NNS_G2D_BGEXTPLTTSLOT_SUB2,
    NNS_G2D_BGEXTPLTTSLOT_SUB3

} NNSG2dBGExtPlttSlot;


//---- スクリーンサイズ数値→レジスタ値変換用
typedef struct ScreenSizeMap
{
    u16 width;
    u16 height;
    u16 scnSize;
}
ScreenSizeMap;


// テキストBGのスクリーンサイズテーブル
static const ScreenSizeMap sTextScnSize[4] =
{
    {256, 256, GX_BG_SCRSIZE_TEXT_256x256},
    {256, 512, GX_BG_SCRSIZE_TEXT_256x512},
    {512, 256, GX_BG_SCRSIZE_TEXT_512x256},
    {512, 512, GX_BG_SCRSIZE_TEXT_512x512},
};

// アフィンBGのスクリーンサイズテーブル
static const ScreenSizeMap sAffineScnSize[4] =
{
    {128, 128, GX_BG_SCRSIZE_AFFINE_128x128},
    {256, 256, GX_BG_SCRSIZE_AFFINE_256x256},
    {512, 512, GX_BG_SCRSIZE_AFFINE_512x512},
    {1024, 1024, GX_BG_SCRSIZE_AFFINE_1024x1024},
};

// アフィン拡張BGのスクリーンサイズテーブル
static const ScreenSizeMap sAffineExtScnSize[4] =
{
    {128, 128, GX_BG_SCRSIZE_256x16PLTT_128x128},
    {256, 256, GX_BG_SCRSIZE_256x16PLTT_256x256},
    {512, 512, GX_BG_SCRSIZE_256x16PLTT_512x512},
    {1024, 1024, GX_BG_SCRSIZE_256x16PLTT_1024x1024},
};


static const u8 sBGTextModeTable[4][8] =
{
    {
        /* 0 -> */ GX_BGMODE_0,
        /* 1 -> */ GX_BGMODE_1,
        /* 2 -> */ GX_BGMODE_2,
        /* 3 -> */ GX_BGMODE_3,
        /* 4 -> */ GX_BGMODE_4,
        /* 5 -> */ GX_BGMODE_5,
        /* 6 -> */ GX_BGMODE_6,
        /* 7 -> */ BG_MODE_WARNING,                             // 設定禁止
    },
    {
        /* 0 -> */ GX_BGMODE_0,
        /* 1 -> */ GX_BGMODE_1,
        /* 2 -> */ GX_BGMODE_2,
        /* 3 -> */ GX_BGMODE_3,
        /* 4 -> */ GX_BGMODE_4,
        /* 5 -> */ GX_BGMODE_5,
        /* 6 -> */ GX_BGMODE_6,
        /* 7 -> */ BG_MODE_WARNING,                             // 設定禁止
    },
    {
        /* 0 -> */ GX_BGMODE_0,
        /* 1 -> */ GX_BGMODE_1,
        /* 2 -> */ GX_BGMODE_1,
        /* 3 -> */ GX_BGMODE_3,
        /* 4 -> */ GX_BGMODE_3,
        /* 5 -> */ GX_BGMODE_3,
        /* 6 -> */ GX_BGMODE_0,
        /* 7 -> */ (GXBGMode)(GX_BGMODE_0 + BG_MODE_WARNING),   // 設定禁止
    },
    {
        /* 0 -> */ GX_BGMODE_0,
        /* 1 -> */ GX_BGMODE_0,
        /* 2 -> */ (GXBGMode)(GX_BGMODE_0 + BG_MODE_WARNING),   // BG2:アフィン     -> テキスト
        /* 3 -> */ GX_BGMODE_0,
        /* 4 -> */ (GXBGMode)(GX_BGMODE_0 + BG_MODE_WARNING),   // BG2:アフィン     -> テキスト
        /* 5 -> */ (GXBGMode)(GX_BGMODE_0 + BG_MODE_WARNING),   // BG2:アフィン拡張 -> テキスト
        /* 6 -> */ (GXBGMode)(GX_BGMODE_0 + BG_MODE_WARNING),   // BG2:大画面BMP    -> テキスト
        /* 7 -> */ (GXBGMode)(GX_BGMODE_0 + BG_MODE_WARNING),   // 設定禁止
    }
};


// BG2 をアフィンBGに変更する時のBGモード遷移テーブル
static const u8 sBGAffineModeTable[2][8] =
{
    {
        /* 0 -> */ (GXBGMode)(GX_BGMODE_2 + BG_MODE_WARNING),   // BG3:テキスト->アフィン
        /* 1 -> */ GX_BGMODE_2,
        /* 2 -> */ GX_BGMODE_2,
        /* 3 -> */ GX_BGMODE_4,
        /* 4 -> */ GX_BGMODE_4,
        /* 5 -> */ GX_BGMODE_4,
        /* 6 -> */ GX_BGMODE_4,
        /* 7 -> */ (GXBGMode)(GX_BGMODE_2 + BG_MODE_WARNING),   // 設定禁止
    },
    {
        /* 0 -> */ GX_BGMODE_1,
        /* 1 -> */ GX_BGMODE_1,
        /* 2 -> */ GX_BGMODE_2,
        /* 3 -> */ GX_BGMODE_1,
        /* 4 -> */ GX_BGMODE_2,
        /* 5 -> */ (GXBGMode)(GX_BGMODE_1 + BG_MODE_WARNING),   // BG2:アフィン拡張->テキスト
        /* 6 -> */ (GXBGMode)(GX_BGMODE_1 + BG_MODE_WARNING),   // BG2:大画面->テキスト
        /* 7 -> */ (GXBGMode)(GX_BGMODE_1 + BG_MODE_WARNING),   // 設定禁止
    }
};
static const u8 sBG256x16PlttModeTable[2][8] =
{
    {
        /* 0 -> */ (GXBGMode)(GX_BGMODE_5 + BG_MODE_WARNING),   // BG3:テキスト->アフィン拡張
        /* 1 -> */ (GXBGMode)(GX_BGMODE_5 + BG_MODE_WARNING),   // BG3:アフィン->アフィン拡張
        /* 2 -> */ (GXBGMode)(GX_BGMODE_5 + BG_MODE_WARNING),   // BG3:アフィン->アフィン拡張
        /* 3 -> */ GX_BGMODE_5,
        /* 4 -> */ GX_BGMODE_5,
        /* 5 -> */ GX_BGMODE_5,
        /* 6 -> */ GX_BGMODE_5,
        /* 7 -> */ (GXBGMode)(GX_BGMODE_5 + BG_MODE_WARNING),   // 設定禁止
    },
    {
        /* 0 -> */ GX_BGMODE_3,
        /* 1 -> */ GX_BGMODE_3,
        /* 2 -> */ GX_BGMODE_4,
        /* 3 -> */ GX_BGMODE_3,
        /* 4 -> */ GX_BGMODE_4,
        /* 5 -> */ GX_BGMODE_5,
        /* 6 -> */ (GXBGMode)(GX_BGMODE_3 + BG_MODE_WARNING),   // BG2:大画面->テキスト
        /* 7 -> */ (GXBGMode)(GX_BGMODE_3 + BG_MODE_WARNING),   // 設定禁止
    }
};



GXBGAreaOver NNSi_G2dBGAreaOver = GX_BG_AREAOVER_XLU;



//****************************************************************************
// static 関数
//****************************************************************************

//----------------------------------------------------------------------------
// インライン関数
//----------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         IsMainBGExtPlttSlot

  Description:  NNSG2dBGExtPlttSlot 型の値がメイン画面の拡張パレットスロットを
                表しているか判定します。

  Arguments:    slot:   判定対象

  Returns:      メイン画面の拡張パレットスロットを表しているなら TRUE。
 *---------------------------------------------------------------------------*/
static NNS_G2D_INLINE BOOL IsMainBGExtPlttSlot(NNSG2dBGExtPlttSlot slot)
{
    return (slot <= NNS_G2D_BGEXTPLTTSLOT_MAIN3);
}



/*---------------------------------------------------------------------------*
  Name:         CalcTextScreenOffset

  Description:  テキストBG面の指定された位置のスクリーンベースからの
                オフセットを取得します。

  Arguments:    x:  位置指定 x （キャラクタ単位）
                y:  位置指定 y （キャラクタ単位）
                w:  対象 BG 面の幅（キャラクタ単位）
                h:  対象 BG 面の高さ（キャラクタ単位）

  Returns:      指定された位置のオフセット（キャラクタ単位）
 *---------------------------------------------------------------------------*/
static NNS_G2D_INLINE int CalcTextScreenOffset(int x, int y, int w, int h)
{
    const int x_blk  = x / PLANE_WIDTH;
    const int y_blk  = y / PLANE_HEIGHT;
    const int x_char = x % PLANE_WIDTH;
    const int y_char = y % PLANE_HEIGHT;
    const int w_blk  = w / PLANE_WIDTH;
    const int h_blk  = h / PLANE_WIDTH;
    const int blk_w  = (x_blk == w_blk) ? (w % PLANE_WIDTH):  PLANE_WIDTH;
    const int blk_h  = (y_blk == h_blk) ? (h % PLANE_HEIGHT): PLANE_HEIGHT;

    return
        w * PLANE_HEIGHT * y_blk
        + PLANE_WIDTH * blk_h * x_blk
        + blk_w * y_char
        + x_char;
}



/*---------------------------------------------------------------------------*
  Name:         GetCompressedPlttOriginalIndex

  Description:  指定されたパレットの圧縮前のオフセットを取得します。

  Arguments:    pCmpInfo:   パレット圧縮情報へのポインタ。
                idx:        パレット番号。

  Returns:      パレットのオフセット（バイト単位）
 *---------------------------------------------------------------------------*/
static NNS_G2D_INLINE u32 GetCompressedPlttOriginalIndex(
    const NNSG2dPaletteCompressInfo* pCmpInfo,
    int idx
)
{
    NNS_G2D_POINTER_ASSERT( pCmpInfo );

    return ( (u16*)(pCmpInfo->pPlttIdxTbl) )[idx];
}



/*---------------------------------------------------------------------------*
  Name:         GetPlttSize

  Description:  パレットデータの1パレットあたりのサイズを取得します。

  Arguments:    pPltData:   パレットデータへのポインタ。

  Returns:      パレットのサイズ（バイト単位）
 *---------------------------------------------------------------------------*/
static NNS_G2D_INLINE u32 GetPlttSize(const NNSG2dPaletteData* pPltData)
{
    NNS_G2D_POINTER_ASSERT( pPltData );

    switch( pPltData->fmt )
    {
    case GX_TEXFMT_PLTT16:     return sizeof(GXBGPltt16);
    case GX_TEXFMT_PLTT256:    return sizeof(GXBGPltt256);
    default:                    SDK_ASSERTMSG(FALSE, "invalid NNSG2dPaletteData");
    }

    return 0;
}






//----------------------------------------------------------------------------
// 通常関数
//----------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         SelectScnSize

  Description:  与えられたテーブルを参照してスクリーンサイズを変換します。
                指定されたスクリーンサイズ以上で最小のBGサイズを返します。
                指定されたスクリーンサイズ以上のBGが存在しない場合は
                最大のBGサイズを返します。

  Arguments:    tbl:    スクリーンサイズ変換テーブル
                w:      ピクセル単位のスクリーン幅
                h:      ピクセル単位のスクリーン高

  Returns:      対応するスクリーンサイズデータへのポインタ。
 *---------------------------------------------------------------------------*/
static const ScreenSizeMap* SelectScnSize(const ScreenSizeMap tbl[4], int w, int h)
{
    int i;
    SDK_NULL_ASSERT(tbl);

    for( i = 0; i < 4; i++ )
    {
        if( w <= tbl[i].width && h <= tbl[i].height )
        {
            return &tbl[i];
        }
    }
    return &tbl[3];
}



/*---------------------------------------------------------------------------*
  Name:         ChangeBGModeByTable*

  Description:  テーブルに従ってBGモードを切り替えます。

  Arguments:    modeTable:  モード遷移テーブル

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static void ChangeBGModeByTableMain(const u8 modeTable[])
{
    GXBGMode mode = (GXBGMode)modeTable[GetBGModeMain()];
    GXBG0As bg0as = IsBG03D() ? GX_BG0_AS_3D: GX_BG0_AS_2D;
    SDK_NULL_ASSERT(modeTable);

    // 他のBGに支障が出るモード変更を警告
    if( mode >= BG_MODE_WARNING )
    {
        mode -= BG_MODE_WARNING;
        OS_Warning("Dangerous Main BG mode change: %d => %d", GetBGModeMain(), mode);
    }

    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, mode, bg0as);
}

static void ChangeBGModeByTableSub(const u8 modeTable[])
{
    GXBGMode mode = (GXBGMode)modeTable[GetBGModeSub()];
    SDK_NULL_ASSERT(modeTable);

    // 他のBGに支障が出るモード変更を警告
    if( mode >= BG_MODE_WARNING )
    {
        mode -= BG_MODE_WARNING;
        OS_Warning("Dangerous Sub BG mode change: %d => %d", GetBGModeSub(), mode);
    }

    GXS_SetGraphicsMode(mode);
}



/*---------------------------------------------------------------------------*
  Name:         LoadBGPlttToExtendedPltt

  Description:  指定された拡張パレットスロットにパレットデータを読み込みます。

  Arguments:    slot:       パレットデータを読み込む拡張パレットスロット
                pPltData:   パレットデータへのポインタ

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static void LoadBGPlttToExtendedPltt(
    NNSG2dBGExtPlttSlot slot,
    const NNSG2dPaletteData* pPltData,
    const NNSG2dPaletteCompressInfo* pCmpInfo
)
{
    void (*prepairLoad)(void);
    void (*cleanupLoad)(void);
    void (*loader)(const void*, u32, u32);
    u32 offset = (u32)( slot * NNS_G2D_BGEXTPLTT_SLOTSIZE );
    SDK_NULL_ASSERT(pPltData);

    // キャッシュフラッシュ
    DC_FlushRange(pPltData->pRawData, pPltData->szByte);

    if( IsMainBGExtPlttSlot(slot) )
    {
        prepairLoad = GX_BeginLoadBGExtPltt;
        cleanupLoad = GX_EndLoadBGExtPltt;
        loader      = GX_LoadBGExtPltt;
    }
    else
    {
        offset -= NNS_G2D_BGEXTPLTT_SLOTSIZE * NNS_G2D_BGEXTPLTTSLOT_SUB0;

        prepairLoad = GXS_BeginLoadBGExtPltt;
        cleanupLoad = GXS_EndLoadBGExtPltt;
        loader      = GXS_LoadBGExtPltt;
    }

    if( pCmpInfo != NULL )
    {
        const u32 szOnePltt = GetPlttSize( pPltData );
        const int numIdx = pCmpInfo->numPalette;
        int i;

        for( i = 0; i < numIdx; i++ )
        {
        	const u32 offsetAddr = GetCompressedPlttOriginalIndex(pCmpInfo, i) * szOnePltt;
            const void* pSrc = (u8*)pPltData->pRawData + szOnePltt * i;

            prepairLoad();
            loader(pSrc, offset + offsetAddr, szOnePltt);
            cleanupLoad();
        }
    }
    else
    {
        prepairLoad();
        loader(pPltData->pRawData, offset, pPltData->szByte);
        cleanupLoad();
    }
}



/*---------------------------------------------------------------------------*
  Name:         LoadBGPlttToNormalPltt

  Description:  標準パレットにパレットデータを読み込みます。

  Arguments:    bMainDisplay:   読込先対象の BG 面。
                pPltData:       パレットデータへのポインタ

  Returns:
 *---------------------------------------------------------------------------*/
static void LoadBGPlttToNormalPltt(
    NNSG2dBGSelect bg,
    const NNSG2dPaletteData* pPltData,
    const NNSG2dPaletteCompressInfo* pCmpInfo
)
{
    u8* pDst;
    SDK_NULL_ASSERT(pPltData);

    // キャッシュフラッシュ
    DC_FlushRange(pPltData->pRawData, pPltData->szByte);

    pDst = (u8*)( IsMainBG(bg) ? HW_BG_PLTT: HW_DB_BG_PLTT );

    if( pCmpInfo != NULL )
    {
        const u32 szOnePltt = GetPlttSize( pPltData );
        const int numIdx = pCmpInfo->numPalette;
        int i;

        for( i = 0; i < numIdx; i++ )
        {
        	const u32 offsetAddr = GetCompressedPlttOriginalIndex(pCmpInfo, i) * szOnePltt;
            const void* pSrc = (u8*)pPltData->pRawData + szOnePltt * i;

            NNSi_G2dDmaCopy16(GXi_DmaId, pSrc, pDst + offsetAddr, szOnePltt);
        }
    }
    else
    {
        NNSi_G2dDmaCopy16(GXi_DmaId, pPltData->pRawData, pDst, pPltData->szByte);
    }
}



/*---------------------------------------------------------------------------*
  Name:         GetBGExtPlttSlot

  Description:  指定された BG が使用する拡張パレットスロットを取得します。

  Arguments:    bg: 使用する拡張パレットスロット番号を取得したい BG

  Returns:      bg が使用する拡張パレットスロット番号
 *---------------------------------------------------------------------------*/
static NNSG2dBGExtPlttSlot GetBGExtPlttSlot(NNSG2dBGSelect bg)
{
    // NNSG2dBGSelect => BGxCNTレジスタオフセット の変換テーブル
    static const u16 addrTable[] =
    {
        REG_BG0CNT_OFFSET,
        REG_BG1CNT_OFFSET,
        0,                      // 使用するスロットは固定
        0,                      // 使用するスロットは固定
        REG_DB_BG0CNT_OFFSET,
        REG_DB_BG1CNT_OFFSET,
        0,                      // 使用するスロットは固定
        0                       // 使用するスロットは固定
    };
    u32 addr;
    NNSG2dBGExtPlttSlot slot = (NNSG2dBGExtPlttSlot)bg;
        // 基本は (int)slot == (int)bg

    NNS_G2D_BG_ASSERT(bg);
    addr = addrTable[bg];

    if( addr != 0 )
    {
        addr += HW_REG_BASE;

        if( (*(u16*)addr & REG_G2_BG0CNT_BGPLTTSLOT_MASK) != 0 )
        {
            // BGPLTTSLOTビットがたっている場合は
            // (int)slot == (int)bg + 2
            slot += 2;
        }
    }

    return slot;
}



/*---------------------------------------------------------------------------*
  Name:         SetBGnControlTo*

  Description:  BGxCNT レジスタの設定と BG モードの変更を行います。

  Arguments:    n:          対象の BG 面。
                size:       スクリーンサイズ。
                cmode:      カラーモード。
                areaOver:   エリアオーバー処理。
                scnBase:    BG のスクリーンベースブロック。
                chrBase:    BG のキャラクタベースブロック。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static void SetBGnControlToText(
    NNSG2dBGSelect n,
    GXBGScrSizeText size,
    GXBGColorMode cmode,
    GXBGScrBase scnBase,
    GXBGCharBase chrBase
)
{
    const int bgNo = GetBGNo(n);
    GXBGExtPltt extPltt = GX_BG_EXTPLTT_01;
    if( IsMainBG(n) )
    {
        if( ! IsMainBGExtPltt01Available() ) extPltt = GX_BG_EXTPLTT_23;
        ChangeBGModeByTableMain(sBGTextModeTable[bgNo]);
    }
    else
    {
        ChangeBGModeByTableSub(sBGTextModeTable[bgNo]);
    }
    SetBGnControlText(n, size, cmode, scnBase, chrBase, extPltt);
}
static void SetBGnControlToAffine(
    NNSG2dBGSelect n,
    GXBGScrSizeAffine size,
    GXBGAreaOver areaOver,
    GXBGScrBase scnBase,
    GXBGCharBase chrBase
)
{
    if( IsMainBG(n) )
    {
        ChangeBGModeByTableMain(sBGAffineModeTable[n-2]);
    }
    else
    {
        ChangeBGModeByTableSub(sBGAffineModeTable[n-6]);
    }
    SetBGnControlAffine(n, size, areaOver, scnBase, chrBase);
}
static void SetBGnControlTo256x16Pltt(
    NNSG2dBGSelect n,
    GXBGScrSize256x16Pltt size,
    GXBGAreaOver areaOver,
    GXBGScrBase scnBase,
    GXBGCharBase chrBase
)
{
    if( IsMainBG(n) )
    {
        ChangeBGModeByTableMain(sBG256x16PlttModeTable[n-2]);
    }
    else
    {
        ChangeBGModeByTableSub(sBG256x16PlttModeTable[n-6]);
    }
    SetBGnControl256x16Pltt(n, size, areaOver, scnBase, chrBase);
}



/*---------------------------------------------------------------------------*
  Name:         BGAutoControlText

  Description:  スクリーンデータに従って BGxCNT レジスタをテキストBGとして
                設定します。同時に BG モードの変更も行います。

  Arguments:    bg:         コントロール対象の BG。
                pScnData:   スクリーンデータへのポインタ。
                scnBase:    BG のスクリーンベース
                chrBase:    BG のキャラクタベース

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static void SetBGControlText(
    NNSG2dBGSelect bg,
    GXBGColorMode colorMode,
    int screenWidth,
    int screenHeight,
    GXBGScrBase scnBase,
    GXBGCharBase chrBase
)
{
    const ScreenSizeMap* pSizeMap;

    NNS_G2D_BG_ASSERT(bg);

    pSizeMap = SelectScnSize(sTextScnSize, screenWidth, screenHeight);
    SDK_ASSERTMSG( pSizeMap != NULL,
        "Unsupported screen size(%dx%d) in input screen data",
        screenWidth,
        screenHeight
    );

    SetBGnControlToText(bg, (GXBGScrSizeText)pSizeMap->scnSize, colorMode, scnBase, chrBase);
}



/*---------------------------------------------------------------------------*
  Name:         BGAutoControlAffine

  Description:  スクリーンデータに従って BGxCNT レジスタをアフィンBGとして
                設定します。同時に BG モードの変更も行います。

  Arguments:    bg:         コントロール対象の BG。
                pScnData:   スクリーンデータへのポインタ。
                scnBase:    BG のスクリーンベース
                chrBase:    BG のキャラクタベース

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static void SetBGControlAffine(
    NNSG2dBGSelect bg,
    int screenWidth,
    int screenHeight,
    GXBGScrBase scnBase,
    GXBGCharBase chrBase
)
{
    const ScreenSizeMap* pSizeMap;

    NNS_G2D_BG_ASSERT(bg);
    SDK_ASSERTMSG( (bg % 4) >= 2,
        "You can not show affine BG on %s BG %d",
        (IsMainBG(bg) ? "Main": "Sub"),
        (IsMainBG(bg) ? bg: bg - NNS_G2D_BGSELECT_SUB0) );

    pSizeMap = SelectScnSize(sAffineScnSize, screenWidth, screenHeight);
    SDK_ASSERTMSG( pSizeMap != NULL,
        "Unsupported screen size(%dx%d) in input screen data",
        screenWidth,
        screenHeight
    );

    SetBGnControlToAffine(bg, (GXBGScrSizeAffine)pSizeMap->scnSize, NNSi_G2dBGAreaOver, scnBase, chrBase);
}



/*---------------------------------------------------------------------------*
  Name:         BGAutoControlAffineExt

  Description:  スクリーンデータに従って BGxCNT レジスタをアフィン拡張BGとして
                設定します。同時に BG モードの変更も行います。

  Arguments:    bg:         コントロール対象の BG。
                pScnData:   スクリーンデータへのポインタ。
                scnBase:    BG のスクリーンベース
                chrBase:    BG のキャラクタベース

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static void SetBGControl256x16Pltt(
    NNSG2dBGSelect bg,
    int screenWidth,
    int screenHeight,
    GXBGScrBase scnBase,
    GXBGCharBase chrBase
)
{
    const ScreenSizeMap* pSizeMap;

    NNS_G2D_BG_ASSERT(bg);
    SDK_ASSERTMSG( (bg % 4) >= 2,
        "You can not show affine BG on %s BG %d",
        (IsMainBG(bg) ? "Main": "Sub"),
        (IsMainBG(bg) ? bg: bg - NNS_G2D_BGSELECT_SUB0) );

    pSizeMap = SelectScnSize(sAffineExtScnSize, screenWidth, screenHeight);
    SDK_ASSERTMSG( pSizeMap != NULL,
        "Unsupported screen size(%dx%d) in input screen data",
        screenWidth,
        screenHeight
    );

    SetBGnControlTo256x16Pltt(bg, (GXBGScrSize256x16Pltt)pSizeMap->scnSize, NNSi_G2dBGAreaOver, scnBase, chrBase);
}



/*---------------------------------------------------------------------------*
  Name:         IsBGExtPlttSlotAssigned

  Description:  指定された拡張パレットスロットに VRAM が割り当てられているか
                判定します。

  Arguments:    slot:   判定対象の拡張パレットスロット

  Returns:      VRAM が割り当てられているなら TRUE。
 *---------------------------------------------------------------------------*/
static BOOL IsBGExtPlttSlotAssigned(NNSG2dBGExtPlttSlot slot)
{
    if( IsMainBGExtPlttSlot(slot) )
    {
        if( slot <= NNS_G2D_BGEXTPLTTSLOT_MAIN1 )
        {
            return IsMainBGExtPltt01Available();
        }
        else
        {
            return IsMainBGExtPltt23Available();
        }
    }
    else
    {
        return IsSubBGExtPlttAvailable();
    }
}



/*---------------------------------------------------------------------------*
  Name:         LoadBGPaletteSelect

  Description:  パレットデータを VRAM に読み込みます。
                パレットを使用する BG と、拡張パレットを使うかどうかで
                読み込み先を指定します。

  Arguments:    bg:         読み込ませるパレットデータを使用する BG
                bToExtPltt: TRUE なら拡張パレットへ、FALSE なら標準パレットへ
                            読み込みます。
                pPltData:   パレットデータへのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static void LoadBGPaletteSelect(
    NNSG2dBGSelect bg,
    BOOL bToExtPltt,
    const NNSG2dPaletteData* pPltData,
    const NNSG2dPaletteCompressInfo* pCmpInfo
)
{
    NNS_G2D_BG_ASSERT(bg);
    SDK_NULL_ASSERT(pPltData);

    if( bToExtPltt )
    {
        NNSG2dBGExtPlttSlot slot = GetBGExtPlttSlot(bg);

        SDK_ASSERT( IsBGExtPlttSlotAssigned(slot) );
        LoadBGPlttToExtendedPltt(slot, pPltData, pCmpInfo);
    }
    else
    {
        LoadBGPlttToNormalPltt(bg, pPltData, pCmpInfo);
    }
}



/*---------------------------------------------------------------------------*
  Name:         LoadBGPalette

  Description:  パレットデータを VRAM に読み込みます。
                パレットを使用する BG と G2Dスクリーンデータから
                読み込み先を自動判定します。

  Arguments:    bg:         読み込ませるパレットデータを使用する BG
                pPltData:   パレットデータへのポインタ。
                pScnData:   読み込ませるパレットデータを使用する
                            スクリーンデータへのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static void LoadBGPalette(
    NNSG2dBGSelect bg,
    const NNSG2dPaletteData* pPltData,
    const NNSG2dScreenData* pScnData,
    const NNSG2dPaletteCompressInfo* pCmpInfo
)
{
/*
    拡張パレット    |有効           無効
      対応スロット  |有効   無効    -
    -------------------------------------
    text    16x16   |標準   標準    標準
            256x1   |拡張   (拡張)  標準
            256x16  |拡張   (拡張)  [標準]
    affine  256x1   |標準   標準    標準
    extend  256x16  |拡張   (拡張)  [標準]

    () 付は透過色以外がすべて黒になる
    [] 付はパレット指定が無視される
    共に ASSERT で停止
*/
    // TODO: 拡張パレット使用時でもバックドロップ色には標準パレットが使われる
    //*(GXRgb*)(HW_BG_PLTT) = pPltData->pRawData[0];

    NNS_G2D_BG_ASSERT(bg);
    SDK_NULL_ASSERT(pPltData);
    SDK_NULL_ASSERT(pScnData);

    if( (pScnData->screenFormat == NNS_G2D_SCREENFORMAT_TEXT)
        && (pScnData->colorMode == NNS_G2D_SCREENCOLORMODE_256x1) )
    {
        LoadBGPaletteSelect(bg, IsBGUseExtPltt(bg), pPltData, pCmpInfo);
    }
    else
    {
        BOOL bExtPlttData = ! (
            (pPltData->fmt == GX_TEXFMT_PLTT16)
            || (pScnData->screenFormat == NNS_G2D_SCREENFORMAT_AFFINE)
        );

        SDK_ASSERTMSG( ! bExtPlttData || IsBGUseExtPltt(bg),
            "Input screen data requires extended BG palette, but unavailable");
        LoadBGPaletteSelect(bg, bExtPlttData, pPltData, pCmpInfo);
    }
}




/*---------------------------------------------------------------------------*
  Name:         LoadBGCharacter

  Description:  キャラクタデータを対象のBG用にロードします。

  Arguments:    bg:         対象の BG 面。
                pChrData:   キャラクタデータへのポインタ。
                pPosInfo:   キャラクタ位置情報へのポインタ。
                            pChrData が部分キャラクタでない場合は NULL を指定。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static void LoadBGCharacter(
    NNSG2dBGSelect bg,
    const NNSG2dCharacterData* pChrData,
    const NNSG2dCharacterPosInfo* pPosInfo
)
{
    u32 offset = 0;

    NNS_G2D_BG_ASSERT(bg);
    SDK_NULL_ASSERT(pChrData);
    SDK_ASSERT( pPosInfo == NULL || pPosInfo->srcPosX == 0 );

    //---- 部分キャラクタの場合はオフセット算出
    if( pPosInfo != NULL )
    {
        int offsetChars = pPosInfo->srcPosY * pPosInfo->srcW;
        u32 szChar = (pChrData->pixelFmt == GX_TEXFMT_PLTT256) ?
                        sizeof(GXCharFmt256): sizeof(GXCharFmt16);

        offset = offsetChars * szChar;
    }

    DC_FlushRange(pChrData->pRawData, pChrData->szByte);
    LoadBGnChar(bg, pChrData->pRawData, offset, pChrData->szByte);
}



/*---------------------------------------------------------------------------*
  Name:         LoadBGScreen

  Description:  スクリーンデータを対象のBG用にロードします。

  Arguments:    bg:         対象の BG 面。
                pScnData:   ロードするスクリーンデータ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static void LoadBGScreen(
    NNSG2dBGSelect bg,
    const NNSG2dScreenData* pScnData
)
{
    GXScrFmtText* pDstBase;
    int plane_cwidth;
    int plane_cheight;
    int load_cwidth;
    int load_cheight;

    NNS_G2D_BG_ASSERT(bg);
    SDK_NULL_ASSERT(pScnData);

    //---- スクリーンベース取得
    pDstBase = (GXScrFmtText*)GetBGnScrPtr(bg);

    //---- 転送可能なサイズを算出
    {
        const int scn_cwidth  = pScnData->screenWidth / 8;
        const int scn_cheight = pScnData->screenHeight / 8;

        NNSi_G2dBGGetCharSize(&plane_cwidth, &plane_cheight, bg);
        load_cwidth = (plane_cwidth > scn_cwidth) ? scn_cwidth: plane_cwidth;
        load_cheight = (plane_cheight > scn_cheight) ? scn_cheight: plane_cheight;
    }


    //---- 転送
    DC_FlushRange( (void*)pScnData->rawData, pScnData->szByte );
    NNS_G2dBGLoadScreenRect(
        pDstBase,
        pScnData,
        0, 0,
        0, 0,
        plane_cwidth, plane_cheight,
        load_cwidth, load_cheight
    );
}






/*---------------------------------------------------------------------------*
  Name:         SetBGControlAuto

  Description:  対象の BG 面の BG コントロールを行います。

  Arguments:    bg:             対象の BG 面。
                screenFormat:   BGタイプ。
                colorMode:      カラーモード
                screenWidth:    スクリーン幅（ピクセル単位）
                screenHeight:   スクリーン高（ピクセル単位）
                schBase:        スクリーンベースブロック。
                chrBase:        キャラクタベースブロック。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static void SetBGControlAuto(
    NNSG2dBGSelect bg,
    NNSG2dScreenFormat screenFormat,
    GXBGColorMode colorMode,
    int screenWidth,
    int screenHeight,
    GXBGScrBase scnBase,
    GXBGCharBase chrBase
)
{
    // BG Control
    switch( screenFormat )
    {
    case NNS_G2D_SCREENFORMAT_TEXT:
        SetBGControlText(bg, colorMode, screenWidth, screenHeight, scnBase, chrBase);
        break;

    case NNS_G2D_SCREENFORMAT_AFFINE:
        SetBGControlAffine(bg, screenWidth, screenHeight, scnBase, chrBase);
        break;

    case NNS_G2D_SCREENFORMAT_AFFINEEXT:
        SetBGControl256x16Pltt(bg, screenWidth, screenHeight, scnBase, chrBase);
        break;

    default:
        SDK_ASSERTMSG(FALSE, "TEXT, AFFINE, and 256x16 format support only");
        break;
    }
}



/*---------------------------------------------------------------------------*
  Name:         LoadScreenPartText

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
static void LoadScreenPartText(
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
)
{
    NNS_G2D_POINTER_ASSERT( pScreenDst );
    NNS_G2D_POINTER_ASSERT( pScnData );

    //---- 1キャラ毎にマッピング
    {
        const int src_x_end             = srcX + width;
        const int src_y_end             = srcY + height;
        const int src_next_offset       = pScnData->screenWidth - width;
        const int dst_next_offset       = dstW - width;
        const u32 szLine                = sizeof(GXScrFmtText) * width;
        const int srcW                  = pScnData->screenWidth / 8;
        const int srcH                  = pScnData->screenHeight / 8;
        const GXScrFmtText* pSrcBase    = (const GXScrFmtText*)pScnData->rawData;
        GXScrFmtText* pDstBase          = (GXScrFmtText*)pScreenDst;
        int sx, sy;
        int dx, dy;

        for( sy = srcY, dy = dstY; sy < src_y_end; sy++, dy++ )
        {
            for( sx = srcX, dx = dstX; sx < src_x_end; sx++, dx++ )
            {
                const GXScrFmtText* pSrc = pSrcBase + CalcTextScreenOffset(sx, sy, srcW, srcH);
                GXScrFmtText* pDst = pDstBase + CalcTextScreenOffset(dx, dy, dstW, dstH);

                *pDst = *pSrc;
            }
        }
    }
}




/*---------------------------------------------------------------------------*
  Name:         LoadScreenPartAffine

  Description:  スクリーンデータ中の指定された矩形をバッファの指定された位置に
                コピーします。

  Arguments:    pScreenDst: 転送先基準点へのポインタ。
                pScnData:   転送下となるスクリーンデータへのポインタ。
                srcX:       転送元左上隅のX座標。（キャラクタ単位）
                srcY:       転送元左上隅のY座標。（キャラクタ単位）
                dstX:       転送先左上隅のX座標。（キャラクタ単位）
                dstY:       転送先左上隅のY座標。（キャラクタ単位）
                dstW:       転送先領域の幅。（キャラクタ単位）
                width:      転送する領域の幅。（キャラクタ単位）
                height:     転送する領域の高さ。（キャラクタ単位）

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static void LoadScreenPartAffine(
    void* pScreenDst,
    const NNSG2dScreenData* pScnData,
    int srcX,
    int srcY,
    int dstX,
    int dstY,
    int dstW,
    int width,
    int height
)
{
    NNS_G2D_POINTER_ASSERT( pScreenDst );
    NNS_G2D_POINTER_ASSERT( pScnData );

    //---- 単純に矩形を転送
    {
        const int src_y_end        = srcY + height;
        const int srcW             = pScnData->screenWidth / 8;
        const u32 szLine           = sizeof(GXScrFmtAffine) * width;
        const GXScrFmtAffine* pSrc = (const GXScrFmtAffine*)pScnData->rawData;
        GXScrFmtAffine*       pDst = (GXScrFmtAffine*)pScreenDst;
        int y;

        pSrc += srcY * srcW + srcX;
        pDst += dstY * dstW + dstX;

        for( y = srcY; y < src_y_end; y++ )
        {
            MI_CpuCopy8(pSrc, pDst, szLine);
            pSrc += srcW;
            pDst += dstW;
        }
    }
}



/*---------------------------------------------------------------------------*
  Name:         LoadScreenPart256x16Pltt

  Description:  スクリーンデータ中の指定された矩形をバッファの指定された位置に
                コピーします。

  Arguments:    pScreenDst: 転送先基準点へのポインタ。
                pScnData:   転送下となるスクリーンデータへのポインタ。
                srcX:       転送元左上隅のX座標。（キャラクタ単位）
                srcY:       転送元左上隅のY座標。（キャラクタ単位）
                dstX:       転送先左上隅のX座標。（キャラクタ単位）
                dstY:       転送先左上隅のY座標。（キャラクタ単位）
                dstW:       転送先領域の幅。（キャラクタ単位）
                width:      転送する領域の幅。（キャラクタ単位）
                height:     転送する領域の高さ。（キャラクタ単位）

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static void LoadScreenPart256x16Pltt(
    void* pScreenDst,
    const NNSG2dScreenData* pScnData,
    int srcX,
    int srcY,
    int dstX,
    int dstY,
    int dstW,
    int width,
    int height
)
{
    NNS_G2D_POINTER_ASSERT( pScreenDst );
    NNS_G2D_POINTER_ASSERT( pScnData );

    //---- 単純に矩形を転送
    {
        const int src_y_end      = srcY + height;
        const int srcW           = pScnData->screenWidth / 8;
        const u32 szLine         = sizeof(GXScrFmtText) * width;
        const GXScrFmtText* pSrc = (const GXScrFmtText*)pScnData->rawData;
        GXScrFmtText*       pDst = (GXScrFmtText*)pScreenDst;
        int y;


        pSrc += srcY * srcW + srcX;
        pDst += dstY * dstW + dstX;

        for( y = srcY; y < src_y_end; y++ )
        {
            MI_CpuCopy16(pSrc, pDst, szLine);
            pSrc += srcW;
            pDst += dstW;
        }
    }
}












//****************************************************************************
// 公開関数
//****************************************************************************

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
)
{
    NNS_G2D_BG_ASSERT(bg);
    NNS_G2D_ASSERT( pPltData == NULL || pScnData != NULL );
    NNS_G2D_ASSERT( pPosInfo == NULL || pChrData != NULL );
    NNS_G2D_ASSERT( pCmpInfo == NULL || pPltData != NULL );

    NNS_G2D_ASSERTMSG( pChrData == NULL || pScnData == NULL
                || ( (pChrData->pixelFmt == GX_TEXFMT_PLTT16)
                    == (pScnData->colorMode == NNS_G2D_SCREENCOLORMODE_16x16) ),
                "Color mode mismatch between character data and screen data" );
    NNS_G2D_ASSERTMSG( pPltData == NULL
                || ( (pPltData->fmt == GX_TEXFMT_PLTT16)
                    == (pScnData->colorMode == NNS_G2D_SCREENCOLORMODE_16x16) ),
                "Color mode mismatch between palette data and screen data" );
    NNS_G2D_ASSERT( pChrData == NULL
                || ( NNSi_G2dGetCharacterFmtType( pChrData->characterFmt )
                    == NNS_G2D_CHARACTER_FMT_CHAR ) );

    if( pPltData != NULL && pScnData != NULL )
    {
        LoadBGPalette(bg, pPltData, pScnData, pCmpInfo);
    }
    if( pChrData != NULL )
    {
        LoadBGCharacter(bg, pChrData, pPosInfo);
    }
    if( pScnData != NULL )
    {
        LoadBGScreen(bg, pScnData);
    }
}



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
)
{
    NNS_G2D_BG_ASSERT(bg);
    NNS_G2D_NULL_ASSERT( pScnData );
    NNS_G2D_ASSERT( pPosInfo == NULL || pChrData != NULL );
    NNS_G2D_ASSERT( pCmpInfo == NULL || pPltData != NULL );

    NNS_G2D_ASSERTMSG( pChrData == NULL
                || ( (pChrData->pixelFmt == GX_TEXFMT_PLTT16)
                    == (pScnData->colorMode == NNS_G2D_SCREENCOLORMODE_16x16) ),
                "Color mode mismatch between character data and screen data" );
    NNS_G2D_ASSERTMSG( pPltData == NULL
                || ( (pPltData->fmt == GX_TEXFMT_PLTT16)
                    == (pScnData->colorMode == NNS_G2D_SCREENCOLORMODE_16x16) ),
                "Color mode mismatch between palette data and screen data" );
    NNS_G2D_ASSERT( pChrData == NULL
                || ( NNSi_G2dGetCharacterFmtType( pChrData->characterFmt )
                    == NNS_G2D_CHARACTER_FMT_CHAR ) );

    SetBGControlAuto(
        bg,
        NNSi_G2dBGGetScreenFormat(pScnData),
        NNSi_G2dBGGetScreenColorMode(pScnData),
        pScnData->screenWidth,
        pScnData->screenHeight,
        scnBase,
        chrBase );
    NNS_G2dBGLoadElementsEx(bg, pScnData, pChrData, pPltData, pPosInfo, pCmpInfo);
}




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
)
{
    NNS_G2D_POINTER_ASSERT( pScreenDst );
    NNS_G2D_POINTER_ASSERT( pScnData );

    // 入力元/出力先の範囲を超えた領域は何もしないようにパラメータを調節する
    if( dstX < 0 )
    {
        const int adj = - dstX;
        srcX  += adj;
        width -= adj;
        dstX   = 0;
    }
    if( dstY < 0 )
    {
        const int adj = - dstY;
        srcY   += adj;
        height -= adj;
        dstY    = 0;
    }
    if( dstX + width > dstW )
    {
        const int adj = (dstX + width) - dstW;
        width -= adj;
    }
    if( dstY + height > dstH )
    {
        const int adj = (dstY + height) - dstH;
        height -= adj;
    }
    if( srcX < 0 )
    {
        const int adj = - srcX;
        dstX  += adj;
        width -= adj;
        srcX   = 0;
    }
    if( srcY < 0 )
    {
        const int adj = - srcY;
        dstY   += adj;
        height -= adj;
        srcY    = 0;
    }
    if( srcX + width > pScnData->screenWidth / 8 )
    {
        const int adj = (srcX + width) - (pScnData->screenWidth / 8);
        width -= adj;
    }
    if( srcY + height > pScnData->screenHeight / 8 )
    {
        const int adj = (srcY + height) - (pScnData->screenHeight / 8);
        height -= adj;
    }

    if( width <= 0 || height <= 0 )
    {
        return;
    }

    switch( pScnData->screenFormat )
    {
    case NNS_G2D_SCREENFORMAT_TEXT:
        LoadScreenPartText(pScreenDst, pScnData, srcX, srcY, dstX, dstY, dstW, dstH, width, height);
        break;

    case NNS_G2D_SCREENFORMAT_AFFINE:
        LoadScreenPartAffine(pScreenDst, pScnData, srcX, srcY, dstX, dstY, dstW, width, height);
        break;

    case NNS_G2D_SCREENFORMAT_AFFINEEXT:
        LoadScreenPart256x16Pltt(pScreenDst, pScnData, srcX, srcY, dstX, dstY, dstW, width, height);
        break;

    default:
        NNS_G2D_ASSERTMSG(FALSE, "Unknown screen format(=%d)", pScnData->screenFormat );
    }
}


