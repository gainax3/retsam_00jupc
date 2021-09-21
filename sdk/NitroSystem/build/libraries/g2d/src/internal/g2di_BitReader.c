/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d - src - internal
  File:     g2di_BitReader.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.4 $
 *---------------------------------------------------------------------------*/

#include <nnsys/g2d/g2d_config.h>
#include "g2di_BitReader.h"


/*---------------------------------------------------------------------------*
  Name:         BitReaderReload

  Description:  BitReader 内の現在のバッファを捨て、次のバイトを読み込みます。

  Arguments:    reader: BitReader へのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static NNS_G2D_INLINE void BitReaderReload(NNSiG2dBitReader* reader)
{
    reader->bits = *(reader->src)++;
    reader->availableBits = 8;
}



/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dBitReaderRead

  Description:  ビット列を読み取ります。

  Arguments:    reader: BitReader へのポインタ。
                nBits:  読み取るビット数。8以下でなければなりません。

  Returns:      読み取ったビット列。
 *---------------------------------------------------------------------------*/
u32 NNSi_G2dBitReaderRead(NNSiG2dBitReader* reader, int nBits)
{
    u32 val = reader->bits;
    int nAvlBits = reader->availableBits;

    SDK_ASSERT(nBits <= 8);

    if( nAvlBits < nBits )
    // バイトバッファ内のビットで不十分な場合
    {
        int lack = nBits - nAvlBits;
        val <<= lack;
        BitReaderReload(reader);
        val |= NNSi_G2dBitReaderRead(reader, lack);
    }
    else
    // バイトバッファ内のビットで十分な場合
    {
        val >>= (nAvlBits - nBits);

        reader->availableBits = (s8)(nAvlBits - nBits);
    }

    val &= 0xFF >> (8 - nBits);
    return val;
}

