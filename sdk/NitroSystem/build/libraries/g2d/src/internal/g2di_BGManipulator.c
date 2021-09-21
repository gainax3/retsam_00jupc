/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d - src - internal
  File:     g2di_BGManipulator.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.2 $
 *---------------------------------------------------------------------------*/

#include "g2di_BGManipulator.h"



//---- BGnCNT レジスタテーブル
REGType16v* const NNSiG2dBGCNTTable[] =
{
    &reg_G2_BG0CNT, &reg_G2_BG1CNT, &reg_G2_BG2CNT, &reg_G2_BG3CNT,
    &reg_G2S_DB_BG0CNT, &reg_G2S_DB_BG1CNT, &reg_G2S_DB_BG2CNT, &reg_G2S_DB_BG3CNT
};


/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dBGGetCharSize

  Description:  対象 BG 面のキャラクタ単位でのサイズを取得します。

  Arguments:    pWidth:     BG 面の幅を格納するバッファへのポインタ。
                pHeight:    BG 面の高さを格納するバッファへのポインタ。
                n:          対象とする BG 面。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void NNSi_G2dBGGetCharSize(int* pWidth, int* pHeight, NNSG2dBGSelect n)
{
    const int bgNo = GetBGNo(n);
    const int scnSize = (*GetBGnCNT(n) & REG_G2_BG0CNT_SCREENSIZE_MASK)
                            >> REG_G2_BG0CNT_SCREENSIZE_SHIFT;
    const int bgMode = IsMainBG(n) ? GetBGModeMain(): GetBGModeSub();
    const BOOL bAffine = ( ((bgNo == 2) && ((bgMode == 2) || (bgMode >= 4)))
                            ||  ((bgNo == 3) && (bgMode >= 1)) );

    if( bAffine )
    {
        const int size = (16 << scnSize);
        *pWidth  = size;
        *pHeight = size;
    }
    else
    {
        *pWidth  = (scnSize & 1) ? 64: 32;
        *pHeight = (scnSize & 2) ? 64: 32;
    }
}
