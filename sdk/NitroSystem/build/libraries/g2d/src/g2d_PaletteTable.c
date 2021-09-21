/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d
  File:     g2d_PaletteTable.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.5 $
 *---------------------------------------------------------------------------*/

#include <nnsys/g2d/g2d_PaletteTable.h>

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitializePaletteTable

  Description:  パレット変換テーブルを初期化します
                
  Arguments:    pPlttTbl:      [OUT] パレット実体
                
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dInitializePaletteTable( NNSG2dPaletteSwapTable* pPlttTbl )
{
    u16     i;
    NNS_G2D_NULL_ASSERT( pPlttTbl );
    
    
    // 入れ替え なし に設定
    for( i = 0; i < NNS_G2D_NUM_COLOR_PALETTE; i++ )
    {
        pPlttTbl->paletteIndex[i] = i;
    }    
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitializePaletteTable

  Description:  パレット変換テーブルを設定します
                
  Arguments:    pPlttTbl:       [OUT] パレット実体
                beforeIdx:      [IN]  変換前のパレット番号
                afterIdx:       [IN]  変換後のパレット番号
                
  Returns:      なし
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetPaletteTableValue
( 
    NNSG2dPaletteSwapTable* pPlttTbl, 
    u16                     beforeIdx, 
    u16                     afterIdx 
)
{
    NNS_G2D_NULL_ASSERT( pPlttTbl );
    NNS_G2D_MINMAX_ASSERT( beforeIdx, 0, NNS_G2D_NUM_COLOR_PALETTE );
    NNS_G2D_MINMAX_ASSERT( afterIdx, 0, NNS_G2D_NUM_COLOR_PALETTE );
    
    pPlttTbl->paletteIndex[beforeIdx] = afterIdx;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitializePaletteTable

  Description:  パレット変換テーブルの設定を取得します
                
  Arguments:    pPlttTbl:      [IN]  パレット実体
                beforeIdx:     [IN]  変換前のパレット番号
                
  Returns:      変換後のパレット番号
  
 *---------------------------------------------------------------------------*/
u16 NNS_G2dGetPaletteTableValue( const NNSG2dPaletteSwapTable* pPlttTbl, u16 beforeIdx )
{
    NNS_G2D_NULL_ASSERT( pPlttTbl );
    NNS_G2D_MINMAX_ASSERT( beforeIdx, 0, NNS_G2D_NUM_COLOR_PALETTE );
    
    {
        const u16       afterIdx = pPlttTbl->paletteIndex[beforeIdx];
        NNS_G2D_MINMAX_ASSERT( afterIdx, 0, NNS_G2D_NUM_COLOR_PALETTE );
        return afterIdx;
    }
}

