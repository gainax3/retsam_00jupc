/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d
  File:     g2di_Dma.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.3 $
 *---------------------------------------------------------------------------*/
#ifndef G2DI_DMA_H_
#define G2DI_DMA_H_

#include <nitro.h>

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dDmaCopy16

  Description:  Dmaコピーを行います。

  Arguments:    dmaNo:  Dma番号
                        ( GX_DMA_NOT_USEが指定された場合は
                          Dmaコピーの代わりにCpuコピーが使用されます。)
                src:    転送元アドレス
                dest:   転送先アドレス
                size:   転送サイズ

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNSi_G2dDmaCopy16( u32 dmaNo, const void* src, void* dest, u32 size )
{
    // dmaNo に GX_DMA_NOT_USE が指定された場合は、CpuCopyを使用します。
    if( dmaNo != GX_DMA_NOT_USE )
    {
        MI_DmaCopy16( dmaNo, src, dest, size );
    }else{
        MI_CpuCopy16( src, dest, size );
    }
}
                      

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dDmaFill32

  Description:  メモリを指定のデータで埋めます。

  Arguments:    dmaNo:  Dma番号
                        ( GX_DMA_NOT_USEが指定された場合は
                          DmaFillの代わりにCpuFillが使用されます。)
                dest:   転送先アドレス
                data:   転送データ
                size:   転送サイズ

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNSi_G2dDmaFill32( u32 dmaNo, void* dest, u32 data, u32 size )
{
    // dmaNo に GX_DMA_NOT_USE が指定された場合は、CpuFillを使用します。
    if( dmaNo != GX_DMA_NOT_USE )
    {
        MI_DmaFill32( dmaNo, dest, data, size );
    }else{
        MI_CpuFill32( dest, data, size );
    }
}

#ifdef __cplusplus
}/* extern "C" */
#endif

#endif // G2DI_DMA_H_

