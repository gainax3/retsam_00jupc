/*---------------------------------------------------------------------------*
  Project:  NITRO-System - include - nnsys - g2d - load
  File:     g2d_NFT_load.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.3 $
 *---------------------------------------------------------------------------*/

#ifndef NNS_G2D_NFT_LOAD_H_
#define NNS_G2D_NFT_LOAD_H_

#include <nitro.h>
#include <nnsys/g2d/g2d_config.h>
#include <nnsys/g2d/g2d_Data.h>
#include <nnsys/g2d/g2d_Font.h>

#ifdef __cplusplus
extern "C" {
#endif



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetUnpackedFont

  Description:  NFTR ファイルを NITROフォントへと展開します。

  Arguments:    pNftrFile:  NFTR ファイルデータへのポインタ。
                ppFont:     NITROフォントへのポインタを格納するバッファへの
                            ポインタ。

  Returns:      展開に成功したら TRUE を返します。
 *---------------------------------------------------------------------------*/
BOOL NNSi_G2dGetUnpackedFont( void* pNftrFile, NNSG2dFontInformation** ppFont );



/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dUnpackNFT

  Description:  NFTR ファイルデータをライブラリで利用できる形へと展開します。
                各ファイルデータに対してライブラリでの使用の前に1回だけ
                実行する必要があります。

  Arguments:    pHeader:    NFTR ファイルのバイナリファイルヘッダへのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void NNSi_G2dUnpackNFT(NNSG2dBinaryFileHeader* pHeader);



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dPrintFont

  Description:  NITROフォントの情報をデバッグ出力に出力します。

  Arguments:    pFont:  NITROフォントへのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
#ifdef SDK_FINALROM
    NNS_G2D_INLINE void NNS_G2dPrintFont( const NNSG2dFont* /*pFont*/ )
    {
    }
#else // SDK_FINALROM
    void NNS_G2dPrintFont( const NNSG2dFont* pFont );
#endif // SDK_FINALROM



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NNS_G2D_NFT_LOAD_H_

