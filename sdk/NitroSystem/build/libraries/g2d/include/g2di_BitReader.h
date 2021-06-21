/*---------------------------------------------------------------------------*
  Project:  NITRO-System - libraries - g2d
  File:     g2di_BitReader.h

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.4 $
 *---------------------------------------------------------------------------*/
#ifndef G2DI_BITREADER_H_
#define G2DI_BITREADER_H_

#include <nitro.h>
#include <nnsys/g2d/g2d_config.h>
#ifdef __cplusplus
extern "C" {
#endif


typedef struct NNSiG2dBitReader
{
    const u8* src;          // 読み取り位置へのポインタ
    s8 availableBits;       // bits中の未読み取りビット数
    u8 bits;                // バイトキャッシュ
    u8 padding_[2];         //
}
NNSiG2dBitReader;

//----------------------------------

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dBitReaderInit

  Description:  BitReader を初期化します。

  Arguments:    reader: 初期化する BitReader へのポインタ。
                src:    ビットを読み取るバイト列へのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNSi_G2dBitReaderInit(NNSiG2dBitReader* reader, const void* src)
{
    reader->availableBits   = 0;
    reader->src             = (const u8*)src;
    reader->bits            = 0;
}



/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dBitReaderRead

  Description:  ビット列を読み取ります。

  Arguments:    reader: BitReader へのポインタ。
                nBits:  読み取るビット数。8以下でなければなりません。

  Returns:      読み取ったビット列。
 *---------------------------------------------------------------------------*/
u32 NNSi_G2dBitReaderRead(NNSiG2dBitReader* reader, int nBits);



/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dBitReaderAlignByte

  Description:  バイトバッファ内の余りのビットを捨てます。
                これにより次に読み込まれるビットは読み込み元の
                各バイト先頭になります。

  Arguments:    reader: BitReader へのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNSi_G2dBitReaderAlignByte(NNSiG2dBitReader* reader)
{
    // availableBits が 8 でなければ 0 にする
    // availableBits が 8 以下であることを仮定
    reader->availableBits &= 8;
}



#ifdef __cplusplus
}/* extern "C" */
#endif

#endif // G2DI_BITREADER_H_

