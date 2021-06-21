/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d
  File:     g2di_BGManipulator.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.6 $
 *---------------------------------------------------------------------------*/
#ifndef G2DI_BGMANIPULATOR_H_
#define G2DI_BGMANIPULATOR_H_

#include <nitro.h>
#include <nnsys/g2d/g2d_Screen.h>
#include "g2di_Dma.h"

#ifdef __cplusplus
extern "C" {
#endif




/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dBGGetCharSize

  Description:  対象 BG 面のキャラクタ単位でのサイズを取得します。

  Arguments:    pWidth:     BG 面の幅を格納するバッファへのポインタ。
                pHeight:    BG 面の高さを格納するバッファへのポインタ。
                n:          対象とする BG 面。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void NNSi_G2dBGGetCharSize(int* pWidth, int* pHeight, NNSG2dBGSelect n);



/*---------------------------------------------------------------------------*
  Name:         GetBGNo

  Description:  対象BG面の BG 番号を取得します。

  Arguments:    n:          対象の BG 面。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE int GetBGNo(NNSG2dBGSelect n)
{
    NNS_G2D_BG_ASSERT( n );
    return n & 3;
}

/*---------------------------------------------------------------------------*
  Name:         GetBGnCNT

  Description:  対象の BG 面を制御する BGnCNT レジスタへのポインタを取得します。

  Arguments:    n:          対象とする BG 面。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE REGType16v* GetBGnCNT(NNSG2dBGSelect n)
{
    extern REGType16v* const NNSiG2dBGCNTTable[];

    NNS_G2D_BG_ASSERT( n );
    return NNSiG2dBGCNTTable[n];
}

/*---------------------------------------------------------------------------*
  Name:         IsBG03D

  Description:  メイン画面 BG0 が3Dに割り当てられているかどうか調べます。

  Arguments:    なし。

  Returns:      3Dに割り当てられているならTRUE。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE BOOL IsBG03D( void )
{
    return (reg_GX_DISPCNT & REG_GX_DISPCNT_BG02D3D_MASK) != 0;
}

/*---------------------------------------------------------------------------*
  Name:         IsMainBG

  Description:  NNSG2dBGSelect 型の値が メイン画面 BG を表しているか
                判定します。

  Arguments:    bg: 判定対象

  Returns:      メイン画面 BG を表しているなら TRUE。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE BOOL IsMainBG(NNSG2dBGSelect bg)
{
    NNS_G2D_BG_ASSERT( bg );
    return ( bg <= NNS_G2D_BGSELECT_MAIN3 );
}

/*---------------------------------------------------------------------------*
  Name:         GetBGMode*

  Description:  現在のBGモードを取得します。

  Arguments:    なし。

  Returns:      現在のBGモード。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE GXBGMode GetBGModeMain( void )
{
    return (GXBGMode)((reg_GX_DISPCNT & REG_GX_DISPCNT_BGMODE_MASK) >> REG_GX_DISPCNT_BGMODE_SHIFT);
}

NNS_G2D_INLINE GXBGMode GetBGModeSub( void )
{
    return (GXBGMode)((reg_GXS_DB_DISPCNT & REG_GXS_DB_DISPCNT_BGMODE_MASK) >> REG_GXS_DB_DISPCNT_BGMODE_SHIFT);
}

/*---------------------------------------------------------------------------*
  Name:         IsBGUseExtPltt*

  Description:  対象の BG 面を担当するグラフィックエンジンが
                拡張パレットを使用するかどうか判定します。

  Arguments:    bg: 判定対象の BG 面。

  Returns:      拡張パレットが有効なら TRUE。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE BOOL IsBGUseExtPlttMain( void )
{
    return (reg_GX_DISPCNT & REG_GX_DISPCNT_BG_MASK) != 0;
}

NNS_G2D_INLINE BOOL IsBGUseExtPlttSub( void )
{
    return (reg_GXS_DB_DISPCNT & REG_GXS_DB_DISPCNT_BG_MASK) != 0;
}

NNS_G2D_INLINE BOOL IsBGUseExtPltt(NNSG2dBGSelect bg)
{
    return IsMainBG(bg) ? IsBGUseExtPlttMain(): IsBGUseExtPlttSub();
}

/*---------------------------------------------------------------------------*
  Name:         IsSubBGExtPlttAvailable

  Description:  サブ画面の拡張パレットに VRAM が割り当てられているか
                判定します。
                サブ画面では0123全スロットに割り当てるか、全く割り当てないかの
                どちらかしか VRAM の割り当てが行えません。

  Arguments:    なし。

  Returns:      VRAM が割り当てられているなら TRUE。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE BOOL IsSubBGExtPlttAvailable( void )
{
    return GX_GetBankForSubBGExtPltt() != GX_VRAM_SUB_BGEXTPLTT_NONE;
}

/*---------------------------------------------------------------------------*
  Name:         IsMainBGExtPltt01Available

  Description:  メイン画面の拡張パレットスロット01に VRAM が割り当てられて
                いるか判定します。

  Arguments:    なし。

  Returns:      VRAM が割り当てられているなら TRUE。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE BOOL IsMainBGExtPltt01Available( void )
{
    GXVRamBGExtPltt pltt = GX_GetBankForBGExtPltt();

    return (pltt == GX_VRAM_BGEXTPLTT_01_F)
            || (pltt == GX_VRAM_BGEXTPLTT_0123_E)
            || (pltt == GX_VRAM_BGEXTPLTT_0123_FG);
}

/*---------------------------------------------------------------------------*
  Name:         IsMainBGExtPltt23Available

  Description:  メイン画面の拡張パレットスロット23に VRAM が割り当てられて
                いるか判定します。

  Arguments:    なし。

  Returns:      VRAM が割り当てられているなら TRUE。
 *---------------------------------------------------------------------------*/
static BOOL IsMainBGExtPltt23Available( void )
{
    GXVRamBGExtPltt pltt = GX_GetBankForBGExtPltt();

    return (pltt == GX_VRAM_BGEXTPLTT_23_G)
            || (pltt == GX_VRAM_BGEXTPLTT_0123_E)
            || (pltt == GX_VRAM_BGEXTPLTT_0123_FG);
}

/*---------------------------------------------------------------------------*
  Name:         MakeBGnCNTVal*

  Description:  BGnCNT レジスタに設定するための値を作成します。

  Arguments:    screenSize: BG面のサイズ
                areaOver:   エリアオーバー処理
                colorMode:  カラーモード
                screenBase: スクリーンベースオフセット
                charBase:   キャラクタベースオフセット
                bgExtPltt:  拡張パレットスロット

  Returns:     作成した値。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE u16 MakeBGnCNTValText(
    GXBGScrSizeText screenSize,
    GXBGColorMode colorMode,
    GXBGScrBase screenBase,
    GXBGCharBase charBase,
    GXBGExtPltt bgExtPltt
)
{
    GX_BG_SCRSIZE_TEXT_ASSERT(screenSize);
    GX_BG_COLORMODE_ASSERT(colorMode);
    GX_BG_SCRBASE_ASSERT(screenBase);
    GX_BG_CHARBASE_ASSERT(charBase);
    GX_BG_EXTPLTT_ASSERT(bgExtPltt);

    return (u16)(
        (screenSize   << REG_G2_BG0CNT_SCREENSIZE_SHIFT)
        | (screenBase << REG_G2_BG0CNT_SCREENBASE_SHIFT)
        | (charBase   << REG_G2_BG0CNT_CHARBASE_SHIFT)
        | (bgExtPltt  << REG_G2_BG0CNT_BGPLTTSLOT_SHIFT)
        | (colorMode  << REG_G2_BG0CNT_COLORMODE_SHIFT)
    );
}
NNS_G2D_INLINE u16 MakeBGnCNTValAffine(
    GXBGScrSizeAffine screenSize,
    GXBGAreaOver areaOver,
    GXBGScrBase screenBase,
    GXBGCharBase charBase
)
{
    GX_BG_SCRSIZE_AFFINE_ASSERT(screenSize);
    GX_BG_AREAOVER_ASSERT(areaOver);
    GX_BG_SCRBASE_ASSERT(screenBase);
    GX_BG_CHARBASE_ASSERT(charBase);

    return (u16)(
        (screenSize   << REG_G2_BG2CNT_SCREENSIZE_SHIFT)
        | (screenBase << REG_G2_BG2CNT_SCREENBASE_SHIFT)
        | (charBase   << REG_G2_BG2CNT_CHARBASE_SHIFT)
        | (areaOver   << REG_G2_BG2CNT_AREAOVER_SHIFT)
    );
}
NNS_G2D_INLINE u16 MakeBGnCNTVal256x16Pltt(
    GXBGScrSize256x16Pltt screenSize,
    GXBGAreaOver areaOver,
    GXBGScrBase screenBase,
    GXBGCharBase charBase
)
{
    GX_BG_SCRSIZE_256x16PLTT_ASSERT(screenSize);
    GX_BG_AREAOVER_ASSERT(areaOver);
    GX_BG_SCRBASE_ASSERT(screenBase);
    GX_BG_CHARBASE_ASSERT(charBase);

    return (u16)(
        (screenSize   << REG_G2_BG2CNT_SCREENSIZE_SHIFT)
        | (screenBase << REG_G2_BG2CNT_SCREENBASE_SHIFT)
        | (charBase   << REG_G2_BG2CNT_CHARBASE_SHIFT)
        | (areaOver   << REG_G2_BG2CNT_AREAOVER_SHIFT)
        | GX_BG_EXTMODE_256x16PLTT
    );
}

/*---------------------------------------------------------------------------*
  Name:         GetBG*Offset

  Description:  メイン画面のスクリーンorキャラクタベースオフセットを取得します。

  Arguments:    なし。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE int GetBGCharOffset(void)
{
    return (int)(0x10000 * ((reg_GX_DISPCNT & REG_GX_DISPCNT_BGCHAROFFSET_MASK) >>
                            REG_GX_DISPCNT_BGCHAROFFSET_SHIFT));
}

NNS_G2D_INLINE int GetBGScrOffset(void)
{
    return (int)(0x10000 * ((reg_GX_DISPCNT & REG_GX_DISPCNT_BGSCREENOFFSET_MASK) >>
                            REG_GX_DISPCNT_BGSCREENOFFSET_SHIFT));
}

/*---------------------------------------------------------------------------*
  Name:         GetBGn*Ptr

  Description:  対象 BG 面のスクリーンorキャラクタベースへのポインタを
                取得します。

  Arguments:    n:      対象となる BG 面。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void* GetBGnCharPtr(NNSG2dBGSelect n)
{
    const int baseBlock = 0x4000 * ((*GetBGnCNT(n) & REG_G2_BG0CNT_CHARBASE_MASK) >>
                             REG_G2_BG0CNT_CHARBASE_SHIFT);

    return (void *)((IsMainBG(n) ? (HW_BG_VRAM + GetBGCharOffset()): HW_DB_BG_VRAM) + baseBlock);
}

NNS_G2D_INLINE void* GetBGnScrPtr(NNSG2dBGSelect n)
{
    const int baseBlock = 0x800 * ((*GetBGnCNT(n) & REG_G2_BG0CNT_SCREENBASE_MASK) >>
                             REG_G2_BG0CNT_SCREENBASE_SHIFT);

    return (void *)((IsMainBG(n) ? (HW_BG_VRAM + GetBGScrOffset()): HW_DB_BG_VRAM) + baseBlock);
}

/*---------------------------------------------------------------------------*
  Name:         LoadBGnChar

  Description:  対象 BG 面のキャラクタデータをロードします。

  Arguments:    n:      対象となる BG 面。
                pSrc:   ロードするデータへのポインタ。
                offset: ロード先のキャラクタベースからのオフセット
                szByte: ロードするデータのサイズ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void LoadBGnChar(NNSG2dBGSelect n, const void *pSrc, u32 offset, u32 szByte)
{
    u32 ptr;

    NNS_G2D_POINTER_ASSERT( pSrc );
    ptr = (u32)GetBGnCharPtr(n);

    NNSi_G2dDmaCopy16(GXi_DmaId, pSrc, (void *)(ptr + offset), szByte);
}

/*---------------------------------------------------------------------------*
  Name:         SetBGnControl*

  Description:  対象 BG 面の BGControl を行います。

  Arguments:    n:          対象の BG 面指定。
                screenSize: スクリーンサイズ。
                colorMode:  カラーモード。
                areaOver:   エリアオーバー処理。
                screenBase: スクリーンベースブロック。
                charBase:   キャラクタベースブロック。
                bgExtPltt:  拡張パレットスロット選択。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void SetBGnControlText(
    NNSG2dBGSelect n,
    GXBGScrSizeText screenSize,
    GXBGColorMode colorMode,
    GXBGScrBase screenBase,
    GXBGCharBase charBase,
    GXBGExtPltt bgExtPltt
)
{
    *GetBGnCNT(n) = (u16)(
        (*GetBGnCNT(n) & (REG_G2_BG0CNT_PRIORITY_MASK | REG_G2_BG0CNT_MOSAIC_MASK))
        | MakeBGnCNTValText(screenSize, colorMode, screenBase, charBase, bgExtPltt)
    );
}
NNS_G2D_INLINE void SetBGnControlAffine(
    NNSG2dBGSelect n,
    GXBGScrSizeAffine screenSize,
    GXBGAreaOver areaOver,
    GXBGScrBase screenBase,
    GXBGCharBase charBase
)
{
    *GetBGnCNT(n) = (u16)(
        (*GetBGnCNT(n) & (REG_G2_BG2CNT_PRIORITY_MASK | REG_G2_BG2CNT_MOSAIC_MASK))
        | MakeBGnCNTValAffine(screenSize, areaOver, screenBase, charBase)
    );
}
NNS_G2D_INLINE void SetBGnControl256x16Pltt(
    NNSG2dBGSelect n,
    GXBGScrSize256x16Pltt screenSize,
    GXBGAreaOver areaOver,
    GXBGScrBase screenBase,
    GXBGCharBase charBase
)
{
    *GetBGnCNT(n) = (u16)(
        (*GetBGnCNT(n) & (REG_G2_BG2CNT_PRIORITY_MASK | REG_G2_BG2CNT_MOSAIC_MASK))
        | MakeBGnCNTVal256x16Pltt(screenSize, areaOver, screenBase, charBase)
    );
}








#ifdef __cplusplus
}/* extern "C" */
#endif

#endif // G2DI_BGMANIPULATOR_H_

