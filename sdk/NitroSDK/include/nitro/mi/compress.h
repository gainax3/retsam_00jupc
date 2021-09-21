/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI - include
  File:     compress.h

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: compress.h,v $
  Revision 1.7  2007/11/12 09:46:58  seiki_masashi
  fix copyright header.

  Revision 1.6  2007/11/02 00:46:00  takano_makoto
  LZ77拡張圧縮に対応

  Revision 1.5  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.4  2005/11/29 05:07:22  takano_makoto
  MI_CompressLZFastを追加

  Revision 1.3  2005/02/28 05:26:02  yosizaki
  do-indent.

  Revision 1.2  2005/02/01 02:17:18  takano_makoto
  ハフマン圧縮の際に引数でワークバッファを渡すように変更

  Revision 1.1  2005/01/28 13:12:43  takano_makoto
  Initial update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_MI_COMPRESS_H_
#define NITRO_MI_COMPRESS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/misc.h>
#include <nitro/types.h>


/*---------------------------------------------------------------------------*
  Name:         MI_CompressLZ
                MI_CompressLZFast

  Description:  LZ77圧縮を行なう関数
                高速版のMI_CompressLZFastではMI_LZ_FAST_COMPRESS_WORK_SIZEバイトの
                ワークバッファが必要となります。

  Arguments:    srcp            圧縮元データへのポインタ
                size            圧縮元データサイズ
                dstp            圧縮先データへのポインタ
                                圧縮元データよりも大きいサイズのバッファが必要です。

  Returns:      圧縮後のデータサイズ。
                圧縮後のデータが圧縮前よりも大きくなる場合には圧縮を中断し0を返します。
 *---------------------------------------------------------------------------*/
u32     MI_CompressLZImpl(const u8 *srcp, u32 size, u8 *dstp, BOOL exFormat);
u32     MI_CompressLZFastImpl(const u8 *srcp, u32 size, u8 *dstp, u8 *work, BOOL exFormat);

inline u32 MI_CompressLZ(const u8 *srcp, u32 size, u8 *dstp)
{
    return MI_CompressLZImpl( srcp, size, dstp, FALSE );
}
inline u32 MI_CompressLZEx(const u8 *srcp, u32 size, u8 *dstp)
{
    return MI_CompressLZImpl( srcp, size, dstp, TRUE );
}
inline u32 MI_CompressLZFast(const u8 *srcp, u32 size, u8 *dstp, u8 *work)
{
    return MI_CompressLZFastImpl( srcp, size, dstp, work, FALSE );
}
inline u32 MI_CompressLZExFast(const u8 *srcp, u32 size, u8 *dstp, u8 *work)
{
    return MI_CompressLZFastImpl( srcp, size, dstp, work, TRUE );
}


#define MI_LZ_FAST_COMPRESS_WORK_SIZE   ( (4096 + 256 + 256) * sizeof(s16) )



/*---------------------------------------------------------------------------*
  Name:         MI_CompressRL

  Description:  ランレングス圧縮を行なう関数

  Arguments:    srcp            圧縮元データへのポインタ
                size            圧縮元データサイズ
                dstp            圧縮先データへのポインタ
                                圧縮元データよりも大きいサイズのバッファが必要です。

  Returns:      圧縮後のデータサイズ。
                圧縮後のデータが圧縮前よりも大きくなる場合には圧縮を中断し0を返します。
 *---------------------------------------------------------------------------*/
u32     MI_CompressRL(const u8 *srcp, u32 size, u8 *dstp);


#define MI_HUFFMAN_COMPRESS_WORK_SIZE   ( 12288 + 512 + 1536 )

/*---------------------------------------------------------------------------*
  Name:         MI_CompressHuffman

  Description:  ハフマン圧縮を行なう関数

  Arguments:    srcp            圧縮元データへのポインタ
                size            圧縮元データサイズ
                dstp            圧縮先データへのポインタ
                                圧縮元データよりも大きいサイズのバッファが必要です。
                huffBitSize     符号化ビット数
                work            圧縮用ワークバッファ MI_HUFFMAN_COMPRESS_WORK_SIZE 分のサイズが必要

  Returns:      圧縮後のデータサイズ。
                圧縮後のデータが圧縮前よりも大きくなる場合には圧縮を中断し0を返します。
 *---------------------------------------------------------------------------*/
u32     MI_CompressHuffman(const u8 *srcp, u32 size, u8 *dstp, u8 huffBitSize, u8 *work);


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_MI_COMPRESS_H_ */
#endif
