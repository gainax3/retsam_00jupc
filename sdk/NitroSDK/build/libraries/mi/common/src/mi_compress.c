/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI - 
  File:     mi_compress.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mi_compress.c,v $
  Revision 1.17  2007/11/02 04:29:07  takano_makoto
  LZEx圧縮の高速化処理を追加

  Revision 1.16  2007/11/02 02:19:21  takano_makoto
  LZEx圧縮の対応漏れを修正

  Revision 1.15  2007/11/02 00:49:16  takano_makoto
  LZ77拡張圧縮に対応
  ハフマン圧縮で値が16bit以上に符号化された場合の不具合を修正

  Revision 1.14  2007/02/20 00:28:10  kitase_hirotake
  indent source

  Revision 1.13  2006/07/06 13:33:02  takano_makoto
  LZ圧縮に失敗した場合にバッファの次の１バイトにオーバーアクセスする場合があった不具合を修正

  Revision 1.12  2006/05/02 08:03:05  takano_makoto
  1.9での不具合修正をMI_CompressLZ()にも適用されていなかったのを修正

  Revision 1.11  2006/04/24 00:04:47  okubata_ryoma
  CW2.0の警告レベル強化に伴った変更

  Revision 1.10  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.9  2006/01/05 04:04:46  takano_makoto
  終端データが3バイトで圧縮される場合の不具合を修正

  Revision 1.8  2005/11/29 05:06:56  takano_makoto
  MI_CompressLZFastを追加

  Revision 1.7  2005/06/29 04:24:51  takano_makoto
  Huffman圧縮で値が１種類しかない場合の不具合を修正

  Revision 1.6  2005/02/28 05:26:25  yosizaki
  do-indent.

  Revision 1.5  2005/02/18 07:09:08  yasu
  Signed/Unsigned 変換警告抑制

  Revision 1.4  2005/02/01 02:16:07  takano_makoto
  ハフマン圧縮の際に引数でワークバッファを渡すように変更

  Revision 1.3  2005/01/31 11:41:00  takano_makoto
  先頭3バイト分の判定を最初におこなうことで、処理を少し高速化する。

  Revision 1.2  2005/01/31 09:32:32  takano_makoto  Revision 1.2  2005/01/31 09:32:32  takano_makoto
  LZ77圧縮を少し高速化

  Revision 1.1  2005/01/28 13:11:28  takano_makoto
  Initial update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/types.h>
#include <nitro/mi/uncompress.h>
#include <nitro/mi/compress.h>

static u32 SearchLZ(const u8 *startp, const u8 *nextp, u32 remainSize, u16 *offset, u32 maxLength );

//===========================================================================
//  LZ符号化
//===========================================================================

/*---------------------------------------------------------------------------*
  Name:         MI_CompressLZ

  Description:  LZ77圧縮を行なう関数

  Arguments:    srcp            圧縮元データへのポインタ
                size            圧縮元データサイズ
                dstp            圧縮先データへのポインタ
                                圧縮元データよりも大きいサイズのバッファが必要です。

  Returns:      圧縮後のデータサイズ。
                圧縮後のデータが圧縮前よりも大きくなる場合には圧縮を中断し0を返します。
 *---------------------------------------------------------------------------*/
u32 MI_CompressLZImpl(const u8 *srcp, u32 size, u8 *dstp, BOOL exFormat)
{
    u32     LZDstCount;                // 圧縮データのバイト数
    u32     LZSrcCount;                // 圧縮対象データの処理済データ量(バイト単位)
    const u8 *srcStartp;
    u8      LZCompFlags;               // 圧縮の有無を示すフラグ系列
    u8     *LZCompFlagsp;              // LZCompFlags を格納するメモリ領域をポイント
    u16     lastOffset;                // 一致データまでのオフセット (その時点での最長一致データ) 
    u32     lastLength;                // 一致データ長 (その時点での最長一致データ)
    u8      i;
    u32     dstMax;
    const u32 MAX_LENGTH = (exFormat)? (0xFFFF + 0xFF + 0xF + 3U) : (0xF + 3U);
    
    SDK_ALIGN2_ASSERT(srcp);

    *(u32 *)dstp = size << 8 | MI_COMPRESSION_LZ | (exFormat? 1 : 0); // データ・ヘッダ
    dstp += 4;
    LZDstCount = 4;
    LZSrcCount = 0;
    srcStartp = srcp;
    dstMax = size;

    while (size > 0)
    {
        LZCompFlags = 0;
        LZCompFlagsp = dstp++;         // フラグ系列の格納先
        LZDstCount++;

        // フラグ系列が8ビットデータとして格納されるため、8回ループ
        for (i = 0; i < 8; i++)
        {
            LZCompFlags <<= 1;         // 初回 (i=0) は特に意味はない
            if (size <= 0)
            {
                // 終端に来た場合はフラグを最後までシフトさせてから終了
                continue;
            }

            if ((lastLength = SearchLZ(srcStartp, srcp, size, &lastOffset, MAX_LENGTH)) != 0)
            {
                u32 length;
                // 圧縮可能な場合はフラグを立てる
                LZCompFlags |= 0x1;

                if (LZDstCount + 2 >= dstMax)   // ソースよりも大きなサイズになる場合はエラー終了
                {
                    return 0;
                }
                
                if ( exFormat )
                {
                    if ( lastLength >= 0xFF + 0xF + 3 )
                    {
                        length  = (u32)( lastLength - 0xFF - 0xF - 3 );
                        *dstp++ = (u8)( 0x10 | (length >> 12) );
                        *dstp++ = (u8)( length >> 4 );
                        LZDstCount += 2;
                    }
                    else if ( lastLength >= 0xF + 2 )
                    {
                        length  = (u32)( lastLength - 0xF - 2 );
                        *dstp++ = (u8)( length >> 4 );
                        LZDstCount += 1;
                    }
                    else
                    {
                        length = (u32)( lastLength - 1 );
                    }
                }
                else
                {
                    length = (u32)( lastLength - 3 );
                }
                
                // オフセットは上位4ビットと下位8ビットに分けて格納
                *dstp++ = (u8)( length << 4 | (lastOffset - 1) >> 8 );
                *dstp++ = (u8)((lastOffset - 1) & 0xff);
                LZDstCount += 2;
                srcp += lastLength;
                size -= lastLength;
            }
            else
            {
                // 圧縮なし
                if (LZDstCount + 1 >= dstMax)   // ソースよりも大きなサイズになる場合はエラー終了
                {
                    return 0;
                }
                *dstp++ = *srcp++;
                size--;
                LZDstCount++;
            }
        }                              // 8回ループ終了
        *LZCompFlagsp = LZCompFlags;   // フラグ系列を格納
    }

    // 4バイト境界アラインメント
    //   アラインメント用データ0 はデータサイズに含めない
    i = 0;
    while ((LZDstCount + i) & 0x3)
    {
        *dstp++ = 0;
        i++;
    }

    return LZDstCount;
}

//--------------------------------------------------------
// LZ77圧縮でスライド窓の中から最長一致列を検索します。
//  Arguments:    startp                 データの開始位置を示すポインタ
//                nextp                  検索を開始するデータのポインタ
//                remainSize             残りデータサイズ
//                offset                 一致したオフセットを格納する領域へのポインタ
//  Return   :    一致列が見つかった場合は   TRUE
//                見つからなかった場合は     FALSE
//--------------------------------------------------------
static u32 SearchLZ(const u8 *startp, const u8 *nextp, u32 remainSize, u16 *offset, u32 maxLength )
{
    const u8 *searchp;
    const u8 *headp, *searchHeadp;
    u16     maxOffset;
    u32     currLength = 2;
    u32     tmpLength;

    if (remainSize < 3)
    {
        return 0;
    }

    // VRAMは2バイトアクセスなので (VRAMからデータを読み出す場合があるため)、
    // 検索対象データは2バイト前からのデータにしなければならない。
    // 
    // オフセットは12ビットで格納されるため、4096以下
    searchp = nextp - 4096;
    if (searchp < startp)
    {
        searchp = startp;
    }

    while (nextp - searchp >= 2)
    {
        headp = nextp;
        searchHeadp = searchp;
        tmpLength = 0;

        while ((*headp != *searchp) || (*(headp + 1) != *(searchp + 1))
               || (*(headp + 2) != *(searchp + 2)))
        {
            searchp++;
            if (nextp - searchp < 2)
            {
                goto search_end;
            }
        }
        searchHeadp = searchp + 3;
        headp += 3;
        tmpLength = 3;

        // データの終端または、異なるデータに遭遇するまで圧縮サイズをインクリメントする。
        while (((u32)(headp - nextp) < remainSize) && (*headp == *searchHeadp))
        {
            headp++;
            searchHeadp++;
            tmpLength++;

            // データ長は4ビットで格納されるため、18以下 (3の下駄をはかせる)
            if (tmpLength == maxLength)
            {
                break;
            }
        }

        if (tmpLength > currLength)
        {
            // 最大長オフセットを更新
            currLength = tmpLength;
            maxOffset = (u16)(nextp - searchp);
            if (currLength == maxLength || currLength == remainSize)
            {
                // 一致長が最大なので、検索を終了する。
                break;
            }
        }

        searchp++;
    }

  search_end:
    if (currLength < 3)
    {
        return 0;
    }
    *offset = maxOffset;
    return currLength;
}


//===========================================================================
//  LZ符号化（高速版)
//===========================================================================

// LZ高速圧縮用一時情報
typedef struct
{
    u16     windowPos;                 // 履歴窓の先頭位置
    u16     windowLen;                 // 履歴窓の長さ

    s16    *LZOffsetTable;             // 履歴窓のオフセットバッファ
    s16    *LZByteTable;               // キャラクタの最新履歴へのポインタ
    s16    *LZEndTable;                // キャラクタの最古履歴へのポインタ
}
LZCompressInfo;

static void LZInitTable(LZCompressInfo * info, u8 *work);
static void SlideByte(LZCompressInfo * info, const u8 *srcp);
static inline void LZSlide(LZCompressInfo * info, const u8 *srcp, u32 n);
static u32 SearchLZFast(LZCompressInfo * info, const u8 *nextp, u32 remainSize, u16 *offset, u32 maxLength );


//--------------------------------------------------------
// 辞書インデックスの初期化
//--------------------------------------------------------
static void LZInitTable(LZCompressInfo * info, u8 *work)
{
    u16     i;

    info->LZOffsetTable = (s16 *)work;
    info->LZByteTable = (s16 *)work + 4096;
    info->LZEndTable = (s16 *)work + 4096 + 256;

    for (i = 0; i < 256; i++)
    {
        info->LZByteTable[i] = -1;
        info->LZEndTable[i] = -1;
    }
    info->windowPos = 0;
    info->windowLen = 0;
}

//--------------------------------------------------------
// 辞書を1バイトスライド
//--------------------------------------------------------
static void SlideByte(LZCompressInfo * info, const u8 *srcp)
{
    s16     offset;
    u8      in_data = *srcp;
    u16     insert_offset;

    s16    *const LZByteTable = info->LZByteTable;
    s16    *const LZOffsetTable = info->LZOffsetTable;
    s16    *const LZEndTable = info->LZEndTable;
    const u16 windowPos = info->windowPos;
    const u16 windowLen = info->windowLen;

    if (windowLen == 4096)
    {
        u8      out_data = *(srcp - 4096);
        if ((LZByteTable[out_data] = LZOffsetTable[LZByteTable[out_data]]) == -1)
        {
            LZEndTable[out_data] = -1;
        }
        insert_offset = windowPos;
    }
    else
    {
        insert_offset = windowLen;
    }

    offset = LZEndTable[in_data];
    if (offset == -1)
    {
        LZByteTable[in_data] = (s16)insert_offset;
    }
    else
    {
        LZOffsetTable[offset] = (s16)insert_offset;
    }
    LZEndTable[in_data] = (s16)insert_offset;
    LZOffsetTable[insert_offset] = -1;

    if (windowLen == 4096)
    {
        info->windowPos = (u16)((windowPos + 1) % 0x1000);
    }
    else
    {
        info->windowLen++;
    }
}

//--------------------------------------------------------
// 辞書をnバイトスライド
//--------------------------------------------------------
static inline void LZSlide(LZCompressInfo * info, const u8 *srcp, u32 n)
{
    u32     i;

    for (i = 0; i < n; i++)
    {
        SlideByte(info, srcp++);
    }
}


/*---------------------------------------------------------------------------*
  Name:         MI_CompressLZFastImpl

  Description:  LZ77圧縮を行なう関数

  Arguments:    srcp            圧縮元データへのポインタ
                size            圧縮元データサイズ
                dstp            圧縮先データへのポインタ
                                圧縮元データよりも大きいサイズのバッファが必要です。
                work            圧縮用一時バッファ
                                MI_LZ_FAST_COMPRESS_WORK_SIZE 分の領域が必要です。

  Returns:      圧縮後のデータサイズ。
                圧縮後のデータが圧縮前よりも大きくなる場合には圧縮を中断し0を返します。
 *---------------------------------------------------------------------------*/
u32 MI_CompressLZFastImpl(const u8 *srcp, u32 size, u8 *dstp, u8 *work, BOOL exFormat)
{
    u32     LZDstCount;                // 圧縮データのバイト数
    u8      LZCompFlags;               // 圧縮の有無を示すフラグ系列
    u8     *LZCompFlagsp;              // LZCompFlags を格納するメモリ領域をポイント
    u16     lastOffset;                // 一致データまでのオフセット (その時点での最長一致データ) 
    u32     lastLength;                // 一致データ長 (その時点での最長一致データ)
    u8      i;
    u32     dstMax;
    LZCompressInfo info;               // LZ圧縮一時情報
    const u32 MAX_LENGTH = (exFormat)? (0xFFFF + 0xFF + 0xF + 3U) : (0xF + 3U);

    SDK_ALIGN2_ASSERT(srcp);
    SDK_NULL_ASSERT(work);

    *(u32 *)dstp = size << 8 | MI_COMPRESSION_LZ | (exFormat? 1 : 0); // データ・ヘッダ
    dstp += 4;
    LZDstCount = 4;
    dstMax = size;
    LZInitTable(&info, work);

    while (size > 0)
    {
        LZCompFlags = 0;
        LZCompFlagsp = dstp++;         // フラグ系列の格納先
        LZDstCount++;

        // フラグ系列が8ビットデータとして格納されるため、8回ループ
        for (i = 0; i < 8; i++)
        {
            LZCompFlags <<= 1;         // 初回 (i=0) は特に意味はない
            if (size <= 0)
            {
                // 終端に来た場合はフラグを最後までシフトさせてから終了
                continue;
            }

            if ( (lastLength = SearchLZFast(&info, srcp, size, &lastOffset, MAX_LENGTH)) != 0 )
            {
                u32 length;
                // 圧縮可能な場合はフラグを立てる
                LZCompFlags |= 0x1;

                if (LZDstCount + 2 >= dstMax)   // ソースよりも大きなサイズになる場合はエラー終了
                {
                    return 0;
                }
                
                if ( exFormat )
                {
                    if ( lastLength >= 0xFF + 0xF + 3 )
                    {
                        length  = (u32)( lastLength - 0xFF - 0xF - 3 );
                        *dstp++ = (u8)( 0x10 | (length >> 12) );
                        *dstp++ = (u8)( length >> 4 );
                        LZDstCount += 2;
                    }
                    else if ( lastLength >= 0xF + 2 )
                    {
                        length  = (u32)( lastLength - 0xF - 2 );
                        *dstp++ = (u8)( length >> 4 );
                        LZDstCount += 1;
                    }
                    else
                    {
                        length = (u32)( lastLength - 1 );
                    }
                }
                else
                {
                    length = (u32)( lastLength - 3 );
                }
                
                // オフセットは上位4ビットと下位8ビットに分けて格納
                *dstp++ = (u8)(length << 4 | (lastOffset - 1) >> 8);
                *dstp++ = (u8)((lastOffset - 1) & 0xff);
                LZDstCount += 2;
                LZSlide(&info, srcp, lastLength);
                srcp += lastLength;
                size -= lastLength;
            }
            else
            {
                // 圧縮なし
                if (LZDstCount + 1 >= dstMax)   // ソースよりも大きなサイズになる場合はエラー終了
                {
                    return 0;
                }
                LZSlide(&info, srcp, 1);
                *dstp++ = *srcp++;
                size--;
                LZDstCount++;
            }
        }                              // 8回ループ終了
        *LZCompFlagsp = LZCompFlags;   // フラグ系列を格納
    }

    // 4バイト境界アラインメント
    //   アラインメント用データ0 はデータサイズに含めない
    i = 0;
    while ((LZDstCount + i) & 0x3)
    {
        *dstp++ = 0;
        i++;
    }

    return LZDstCount;
}

//--------------------------------------------------------
// LZ77圧縮でスライド窓の中から最長一致列を検索します。
//  Arguments:    startp                 データの開始位置を示すポインタ
//                nextp                  検索を開始するデータのポインタ
//                remainSize             残りデータサイズ
//                offset                 一致したオフセットを格納する領域へのポインタ
//  Return   :    一致列が見つかった場合は   TRUE
//                見つからなかった場合は     FALSE
//--------------------------------------------------------
static u32 SearchLZFast(LZCompressInfo * info, const u8 *nextp, u32 remainSize, u16 *offset, u32 maxLength )
{
    const u8 *searchp;
    const u8 *headp, *searchHeadp;
    u16     maxOffset;
    u32     currLength = 2;
    u32     tmpLength;
    s32     w_offset;
    s16    *const LZOffsetTable = info->LZOffsetTable;
    const u16 windowPos = info->windowPos;
    const u16 windowLen = info->windowLen;

    if (remainSize < 3)
    {
        return 0;
    }

    w_offset = info->LZByteTable[*nextp];

    while (w_offset != -1)
    {
        if (w_offset < windowPos)
        {
            searchp = nextp - windowPos + w_offset;
        }
        else
        {
            searchp = nextp - windowLen - windowPos + w_offset;
        }

        /* 無くても良いが、僅かに高速化が見込める */
        if (*(searchp + 1) != *(nextp + 1) || *(searchp + 2) != *(nextp + 2))
        {
            w_offset = LZOffsetTable[w_offset];
            continue;
        }

        if (nextp - searchp < 2)
        {
            // VRAMは2バイトアクセスなので (VRAMからデータを読み出す場合があるため)、
            // 検索対象データは2バイト前からのデータにしなければならない。
            // 
            // オフセットは12ビットで格納されるため、4096以下
            break;
        }
        tmpLength = 3;
        searchHeadp = searchp + 3;
        headp = nextp + 3;

        // データの終端または、異なるデータに遭遇するまで圧縮サイズをインクリメントする。
        while (((u32)(headp - nextp) < remainSize) && (*headp == *searchHeadp))
        {
            headp++;
            searchHeadp++;
            tmpLength++;

            // データ長は4ビットで格納されるため、18以下 (3の下駄をはかせる)
            if (tmpLength == maxLength)
            {
                break;
            }
        }

        if (tmpLength > currLength)
        {
            // 最大長オフセットを更新
            currLength = tmpLength;
            maxOffset = (u16)(nextp - searchp);
            if (currLength == maxLength || currLength == remainSize)
            {
                // 一致長が最大なので、検索を終了する。
                break;
            }
        }
        w_offset = LZOffsetTable[w_offset];
    }

    if (currLength < 3)
    {
        return 0;
    }
    *offset = maxOffset;
    return currLength;
}


//===========================================================================
//  ランレングス符号化
//===========================================================================

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
u32 MI_CompressRL(const u8 *srcp, u32 size, u8 *dstp)
{
    u32     RLDstCount;                // 圧縮データのバイト数
    u32     RLSrcCount;                // 圧縮対象データの処理済データ量(バイト単位)
    u8      RLCompFlag;                // ランレングス符号化を行う場合１
    u8      runLength;                 // ランレングス
    u8      rawDataLength;             // ランになっていないデータのレングス
    u32     i;

    const u8 *startp;                  // 一回の処理ループにおける、圧縮対象データの先頭をポイント

    //  データヘッダ        (サイズは展開後)
    *(u32 *)dstp = size << 8 | MI_COMPRESSION_RL;       // データ・ヘッダ
    RLDstCount = 4;

    RLSrcCount = 0;
    rawDataLength = 0;
    RLCompFlag = 0;

    while (RLSrcCount < size)
    {
        startp = &srcp[RLSrcCount];    // 圧縮対象データの設定

        for (i = 0; i < 128; i++)      // 7ビットで表現できるデータ量が 0~127
        {
            // 圧縮対象データの末尾に到達
            if (RLSrcCount + rawDataLength >= size)
            {
                rawDataLength = (u8)(size - RLSrcCount);
                break;
            }

            if (RLSrcCount + rawDataLength + 2 < size)
            {
                if (startp[i] == startp[i + 1] && startp[i] == startp[i + 2])
                {
                    RLCompFlag = 1;
                    break;
                }
            }
            rawDataLength++;
        }

        // 符号化しないデータを格納
        // データ長格納バイトの8ビット目が0なら、符号化しないデータ系列
        // データ長は -1 した数になるので、0-127 が 1-128 となる
        if (rawDataLength)
        {
            if (RLDstCount + rawDataLength + 1 >= size) // ソースよりも大きなサイズになる場合はエラー終了
            {
                return 0;
            }
            dstp[RLDstCount++] = (u8)(rawDataLength - 1);       // "データ長-1" 格納(7ビット)
            for (i = 0; i < rawDataLength; i++)
            {
                dstp[RLDstCount++] = srcp[RLSrcCount++];
            }
            rawDataLength = 0;
        }

        // ランレングス符号化
        if (RLCompFlag)
        {
            runLength = 3;
            for (i = 3; i < 128 + 2; i++)
            {
                // 圧縮用データの末尾に到達
                if (RLSrcCount + runLength >= size)
                {
                    runLength = (u8)(size - RLSrcCount);
                    break;
                }

                // ランが途切れた場合
                if (srcp[RLSrcCount] != srcp[RLSrcCount + runLength])
                {
                    break;
                }
                // ラン継続中
                runLength++;
            }

            // データ長格納バイトの8ビット目が1なら、符号化したデータ系列
            if (RLDstCount + 2 >= size) // ソースよりも大きなサイズになる場合はエラー終了
            {
                return 0;
            }
            dstp[RLDstCount++] = (u8)(0x80 | (runLength - 3));  // ３の下駄をはかせて、3~130を格納
            dstp[RLDstCount++] = srcp[RLSrcCount];
            RLSrcCount += runLength;
            RLCompFlag = 0;
        }
    }

    // 4バイト境界アラインメント
    //   アラインメント用データ0 はデータサイズに含めない
    i = 0;
    while ((RLDstCount + i) & 0x3)
    {
        dstp[RLDstCount + i] = 0;
        i++;
    }
    return RLDstCount;
}


//===========================================================================
//  ハフマン符号化
//===========================================================================
#define HUFF_END_L  0x80
#define HUFF_END_R  0x40

static void HuffUpdateParentDepth(u16 leftNo, u16 rightNo);
static void HuffMakeCode(u16 nodeNo, u32 paHuffCode);
static u8 HuffCountHWord(u16 nodeNo);
static void HuffMakeHuffTree(u16 rootNo);
static void HuffMakeSubsetHuffTree(u16 huffTreeNo, u8 rightNodeFlag);
static u8 HuffRemainingNodeCanSetOffset(u8 costHWord);
static void HuffSetOneNodeOffset(u16 huffTreeNo, u8 rightNodeFlag);

typedef struct
{
    u32     Freq;                      // 出現頻度
    u16     No;                        // データNo
    s16     PaNo;                      // 親No 
    s16     ChNo[2];                   // 子No (0: 左側， 1: 右側)
    u16     PaDepth;                   // 親ノードの深さ
    u16     LeafDepth;                 // 葉までの深さ
    u32     HuffCode;                  // ハフマン符号
    u8      Bit;                       // ノードのビットデータ
    u8      _padding;
    u16     HWord;                     // 各中間節点において、その節点をルートとする部分木を HuffTree 格納に必要なメモリ量
}
HuffData;                              // 計 24 Byte

static HuffData *HuffTable;            // [512] 12288B
static const HuffData HuffTableInitData = { 0, 0, 0, {-1, -1}, 0, 0, 0, 0, 0 };

static u8 HuffTreeTop;                 // HuffTreeTop の番号
static u8 *HuffTree;                   // [256][2] 512B

typedef struct
{
    u8      leftOffsetNeed;            // 左の子節点へのオフセットが必要なら1
    u8      rightOffsetNeed;           // 右の子節点へのオフセットが必要なら1
    u16     leftNodeNo;                // 左の子節点No
    u16     rightNodeNo;               // 右の子節点No
}
HuffTreeCtrlData;                      // 計 6 Byte
static HuffTreeCtrlData *HuffTreeCtrl; // 1536 Byte  [ 256 ]
static const HuffTreeCtrlData HuffTreeCtrlInitData = { 1, 1, 0, 0 };

static u16 sHuffDataNum;               // 符号化されるデータの種類 4ビット符号化なら16 8ビットなら256

// ハフマンワークバッファ構成
typedef struct
{
    HuffData HuffTable[512];           //    12288B
    u8      HuffTree[256 * 2];         //      512B
    HuffTreeCtrlData HuffTreeCtrl[256]; //     1536B
}
HuffWork;                              // 計 14336B

/*---------------------------------------------------------------------------*
  Name:         MI_CompressHuffman

  Description:  ハフマン圧縮を行なう関数

  Arguments:    srcp            圧縮元データへのポインタ
                size            圧縮元データサイズ
                dstp            圧縮先データへのポインタ
                                圧縮元データよりも大きいサイズのバッファが必要です。
                huffBitSize     符号化ビット数
                work            ハフマン圧縮用のワークバッファ

  Returns:      圧縮後のデータサイズ。
                圧縮後のデータが圧縮前よりも大きくなる場合には圧縮を中断し0を返します。
 *---------------------------------------------------------------------------*/
u32 MI_CompressHuffman(const u8 *srcp, u32 size, u8 *dstp, u8 huffBitSize, u8 *work)
{
    u32     HuffDstCount;              // 圧縮データのバイト数
    u8      tmp;
    u16     nodeNum;                   // 有効ノード数
    u16     tableTop;                  // テーブル作成時の、テーブルトップNo
    s32     leftNo, rightNo;           // 2分木作成時のノードNo
    s32     i, ii, iii;
    u8      srcTmp;
    u32     bitStream = 0;
    u32     streamLength = 0;
    u16     rootNo;                    // 二分木のルートNo


    SDK_NULL_ASSERT(srcp);
    SDK_NULL_ASSERT(dstp);
    SDK_ASSERT(huffBitSize == 4 || huffBitSize == 8);
    SDK_NULL_ASSERT(work);
    SDK_ALIGN4_ASSERT(work);

    HuffTable = ((HuffWork *) work)->HuffTable;
    HuffTree = ((HuffWork *) work)->HuffTree;
    HuffTreeCtrl = ((HuffWork *) work)->HuffTreeCtrl;

    sHuffDataNum = (u16)(1 << huffBitSize);     // 8->256, 4->16
    tableTop = sHuffDataNum;

    // テーブル初期化
    //  添え字 ：   0  ~ 15(255)    : 符号化対象データの情報 (8ビット符号化)
    //              16 ~ 31(511)    : 二分木作成用の情報 
    for (i = 0; i < (u16)(sHuffDataNum * 2); i++)
    {
        HuffTable[i] = HuffTableInitData;
        HuffTable[i].No = (u16)i;
    }

    // 出現頻度チェック
    if (huffBitSize == 8)
    {
        for (i = 0; i < size; i++)
        {
            HuffTable[srcp[i]].Freq++; // 8ビット符号化
        }
    }
    else
    {
        for (i = 0; i < size; i++)
        {                              // 4ビット符号化        
            tmp = (u8)((srcp[i] & 0xf0) >> 4);  // 上位4ビットから先に格納
            HuffTable[tmp].Freq++;     // 問題は符号化のとこ
            tmp = (u8)(srcp[i] & 0x0f);
            HuffTable[tmp].Freq++;
        }
    }

    // ツリーテーブル作成
    leftNo = rightNo = -1;
    while (1)
    {
        // Freqの小さい部分木頂点を2つ探す  1つは必ず見つかるはず
        // 子頂点(左)の探索
        for (i = 0; i < tableTop; i++)
        {
            if ((HuffTable[i].Freq != 0) && (HuffTable[i].PaNo == 0))
            {
                leftNo = i;
                break;
            }
        }
        for (i = (u16)leftNo; i < tableTop; i++)
        {
            if ((HuffTable[i].Freq != 0) &&
                (HuffTable[i].PaNo == 0) && (HuffTable[i].Freq < HuffTable[leftNo].Freq))
            {
                leftNo = i;
            }
        }
        // 子頂点(右)の探索
        for (i = 0; i < tableTop; i++)
        {
            if ((HuffTable[i].Freq != 0) && (HuffTable[i].PaNo == 0) && (i != leftNo))
            {
                rightNo = i;
                break;
            }
        }
        for (i = (u16)rightNo; i < tableTop; i++)
        {
            if ((HuffTable[i].Freq != 0) &&
                (HuffTable[i].PaNo == 0) &&
                (HuffTable[i].Freq < HuffTable[rightNo].Freq) && (i != leftNo))
            {
                rightNo = i;
            }
        }
        // 1つしかなかったら、テーブル作成終了
        if (rightNo < 0)
        {
            if (tableTop == sHuffDataNum)
                // 値が一種類しかない存在しない場合には01どちらも同じ値となるノードを１つ作成する
            {
                HuffTable[tableTop].Freq = HuffTable[leftNo].Freq;
                HuffTable[tableTop].ChNo[0] = (s16)leftNo;
                HuffTable[tableTop].ChNo[1] = (s16)leftNo;
                HuffTable[tableTop].LeafDepth = 1;
                HuffTable[leftNo].PaNo = (s16)tableTop;
                HuffTable[leftNo].Bit = 0;
                HuffTable[leftNo].PaDepth = 1;
            }
            else
            {
                tableTop--;
            }
            rootNo = tableTop;
            nodeNum = (u16)((rootNo - sHuffDataNum + 1) * 2 + 1);
            break;
        }

        // 左部分木と右部分木を統合する頂点作成
        HuffTable[tableTop].Freq = HuffTable[leftNo].Freq + HuffTable[rightNo].Freq;
        HuffTable[tableTop].ChNo[0] = (s16)leftNo;
        HuffTable[tableTop].ChNo[1] = (s16)rightNo;
        if (HuffTable[leftNo].LeafDepth > HuffTable[rightNo].LeafDepth)
        {
            HuffTable[tableTop].LeafDepth = (u16)(HuffTable[leftNo].LeafDepth + 1);
        }
        else
        {
            HuffTable[tableTop].LeafDepth = (u16)(HuffTable[rightNo].LeafDepth + 1);
        }

        HuffTable[leftNo].PaNo = HuffTable[rightNo].PaNo = (s16)tableTop;
        HuffTable[leftNo].Bit = 0;
        HuffTable[rightNo].Bit = 1;

        HuffUpdateParentDepth((u16)leftNo, (u16)rightNo);

        tableTop++;
        leftNo = rightNo = -1;
    }

    // ハフマンコード生成 (HuffTable[i].HuffCode に)
    HuffMakeCode(rootNo, 0x00);        // PaDepthのビット数だけ、HuffCode の下位ビットをマスクしたものがハフマンコード

    // 各中間節点において、その節点をルートとする部分木を HuffTree 格納に必要なメモリ量の計算
    (void)HuffCountHWord(rootNo);

    // HuffTree 作成
    HuffMakeHuffTree(rootNo);
    HuffTree[0] = --HuffTreeTop;

    // データ・ヘッダ
    *(u32 *)dstp = size << 8 | MI_COMPRESSION_HUFFMAN | huffBitSize;
    HuffDstCount = 4;

    if (HuffDstCount + (HuffTreeTop + 1) * 2 >= size)   // ソースよりも大きなサイズになる場合はエラー終了
    {
        return 0;
    }

    for (i = 0; i < (u16)((HuffTreeTop + 1) * 2); i++)  // ツリーテーブル
    {
        dstp[HuffDstCount++] = ((u8 *)HuffTree)[i];
    }

    // 4バイト境界アラインメント
    //   アラインメント用データ0 はデータサイズに含める (デコーダのアルゴリズムによる)
    while (HuffDstCount & 0x3)
    {
        if (HuffDstCount & 0x1)
        {
            HuffTreeTop++;
            dstp[4]++;
        }
        dstp[HuffDstCount++] = 0;
    }

    // ハフマン符号化
    for (i = 0; i < size; i++)         // データ圧縮
    {
        u8 val = srcp[i];
        if (huffBitSize == 8)          // 8ビットハフマン
        {
            bitStream = (bitStream << HuffTable[ val ].PaDepth) | HuffTable[ val ].HuffCode;
            streamLength += HuffTable[ val ].PaDepth;

            if (HuffDstCount + (streamLength / 8) >= size)
            {
                // ソースよりも大きなサイズになる場合はエラー終了
                return 0;
            }
            for (ii = 0; ii < streamLength / 8; ii++)
            {
                dstp[HuffDstCount++] = (u8)(bitStream >> (streamLength - (ii + 1) * 8));
            }
            streamLength %= 8;
        }
        else                           // 4ビットハフマン
        {
            for (ii = 0; ii < 2; ii++)
            {
                if (ii)
                {
                    srcTmp = (u8)( val >> 4 );        // 上位4ビット
                }
                else
                {
                    srcTmp = (u8)( val & 0x0F );      // 下位4ビット
                }
                bitStream = (bitStream << HuffTable[srcTmp].PaDepth) | HuffTable[srcTmp].HuffCode;
                streamLength += HuffTable[srcTmp].PaDepth;
                if (HuffDstCount + (streamLength / 8) >= size)
                {
                    // ソースよりも大きなサイズになる場合はエラー終了
                    return 0;
                }
                for (iii = 0; iii < streamLength / 8; iii++)
                {
                    dstp[HuffDstCount++] = (u8)(bitStream >> (streamLength - (iii + 1) * 8));
                }
                streamLength %= 8;
            }
        }
    }

    if (streamLength != 0)
    {
        if (HuffDstCount + 1 >= size)
        {
            // ソースよりも大きなサイズになる場合はエラー終了
            return 0;
        }
        dstp[HuffDstCount++] = (u8)(bitStream << (8 - streamLength));
    }

    // 4バイト境界アラインメント
    //   アラインメント用データ0 はデータサイズに含め「る」 
    //   ハフマン符号化だけ特別!　リトルエンディアン変換するため、アラインメント境界データより後にデータが格納される
    while (HuffDstCount & 0x3)
    {
        dstp[HuffDstCount++] = 0;
    }

    for (i = 1 + (HuffTreeTop + 1) * 2 / 4; i < (HuffDstCount / 4) + 1; i++)    // リトルエンディアン変換
    {
        tmp = dstp[i * 4 + 0];
        dstp[i * 4 + 0] = dstp[i * 4 + 3];
        dstp[i * 4 + 3] = tmp;         // スワップ
        tmp = dstp[i * 4 + 1];
        dstp[i * 4 + 1] = dstp[i * 4 + 2];
        dstp[i * 4 + 2] = tmp;         // スワップ
    }

    return HuffDstCount;
}



//-----------------------------------------------------------------------
// ハフマンコード表作成
//-----------------------------------------------------------------------
static void HuffMakeHuffTree(u16 rootNo)
{
    s16     i;
    u16     tmp;
    s16     costHWord, tmpCostHWord;   // 部分木のコード表を作成しなかった時のコスト 最大値の節点の部分木コード表を作る
    s16     costOffsetNeed, tmpCostOffsetNeed;
    s16     costMaxKey, costMaxRightFlag;       // コスト最小の節点を HuffTree から特定するための情報
    u8      offsetNeedNum;
    u8      tmpKey, tmpRightFlag;

    // HuffTreeCtrl 初期化
    for (i = 0; i < 256; i++)
    {
        HuffTree[i * 2] = HuffTree[i * 2 + 1] = 0;
    }
    for (i = 0; i < 256; i++)
    {
        HuffTreeCtrl[i] = HuffTreeCtrlInitData;
    }
    HuffTreeTop = 1;
    costOffsetNeed = 0;

    HuffTreeCtrl[0].leftOffsetNeed = 0; // 使用しない (テーブルサイズとして使用)
    HuffTreeCtrl[0].rightNodeNo = rootNo;

    while (1)                          // return するまで 
    {
        // オフセットを設定する必要のあるノード数の計算
        offsetNeedNum = 0;
        for (i = 0; i < HuffTreeTop; i++)
        {
            if (HuffTreeCtrl[i].leftOffsetNeed)
            {
                offsetNeedNum++;
            }
            if (HuffTreeCtrl[i].rightOffsetNeed)
            {
                offsetNeedNum++;
            }
        }

        // 最大コストの節点を検索
        costHWord = -1;
        costMaxKey = -1;
        tmpKey = 0;
        tmpRightFlag = 0;

        for (i = 0; i < HuffTreeTop; i++)
        {

            tmpCostOffsetNeed = (u8)(HuffTreeTop - i);

            // 左の子節点のコスト評価
            if (HuffTreeCtrl[i].leftOffsetNeed)
            {
                tmpCostHWord = (s16)HuffTable[HuffTreeCtrl[i].leftNodeNo].HWord;

                if ((tmpCostHWord + offsetNeedNum) > 64)
                {
                    goto leftCostEvaluationEnd;
                }
                if (!HuffRemainingNodeCanSetOffset((u8)tmpCostHWord))
                {
                    goto leftCostEvaluationEnd;
                }
                if (tmpCostHWord > costHWord)
                {
                    costMaxKey = i;
                    costMaxRightFlag = 0;
                }
                else if ((tmpCostHWord == costHWord) && (tmpCostOffsetNeed > costOffsetNeed))
                {
                    costMaxKey = i;
                    costMaxRightFlag = 0;
                }
            }
          leftCostEvaluationEnd:{
            }

            // 右の子節点のコスト評価
            if (HuffTreeCtrl[i].rightOffsetNeed)
            {
                tmpCostHWord = (s16)HuffTable[HuffTreeCtrl[i].rightNodeNo].HWord;

                if ((tmpCostHWord + offsetNeedNum) > 64)
                {
                    goto rightCostEvaluationEnd;
                }
                if (!(HuffRemainingNodeCanSetOffset((u8)tmpCostHWord)))
                {
                    goto rightCostEvaluationEnd;
                }
                if (tmpCostHWord > costHWord)
                {
                    costMaxKey = i;
                    costMaxRightFlag = 1;
                }
                else if ((tmpCostHWord == costHWord) && (tmpCostOffsetNeed > costOffsetNeed))
                {
                    costMaxKey = i;
                    costMaxRightFlag = 1;
                }
            }
          rightCostEvaluationEnd:{
            }
        }

        // 部分木をまるまる HuffTree に格納
        if (costMaxKey >= 0)
        {
            HuffMakeSubsetHuffTree((u8)costMaxKey, (u8)costMaxRightFlag);
            goto nextTreeMaking;
        }
        else
        {
            // 必要オフセット最大のノードを検索
            for (i = 0; i < HuffTreeTop; i++)
            {
                tmp = 0;
                tmpRightFlag = 0;
                if (HuffTreeCtrl[i].leftOffsetNeed)
                {
                    tmp = HuffTable[HuffTreeCtrl[i].leftNodeNo].HWord;
                }
                if (HuffTreeCtrl[i].rightOffsetNeed)
                {
                    if (HuffTable[HuffTreeCtrl[i].rightNodeNo].HWord > tmp)
                    {
                        tmpRightFlag = 1;
                    }
                }
                if ((tmp != 0) || (tmpRightFlag))
                {
                    HuffSetOneNodeOffset((u8)i, tmpRightFlag);
                    goto nextTreeMaking;
                }
            }
        }
        return;
      nextTreeMaking:{
        }
    }
}

//-----------------------------------------------------------------------
// 部分木をまるまる HuffTree に格納
//-----------------------------------------------------------------------
static void HuffMakeSubsetHuffTree(u16 huffTreeNo, u8 rightNodeFlag)
{
    u8      i;

    i = HuffTreeTop;
    HuffSetOneNodeOffset(huffTreeNo, rightNodeFlag);

    if (rightNodeFlag)
    {
        HuffTreeCtrl[huffTreeNo].rightOffsetNeed = 0;
    }
    else
    {
        HuffTreeCtrl[huffTreeNo].leftOffsetNeed = 0;
    }

    while (i < HuffTreeTop)
    {
        if (HuffTreeCtrl[i].leftOffsetNeed)
        {
            HuffSetOneNodeOffset(i, 0);
            HuffTreeCtrl[i].leftOffsetNeed = 0;
        }
        if (HuffTreeCtrl[i].rightOffsetNeed)
        {
            HuffSetOneNodeOffset(i, 1);
            HuffTreeCtrl[i].rightOffsetNeed = 0;
        }
        i++;
    }
}

//-----------------------------------------------------------------------
// 与えられたデータ量の部分木を展開しても HuffTree 構築に支障がないか調べる
//-----------------------------------------------------------------------
static u8 HuffRemainingNodeCanSetOffset(u8 costHWord)
{
    u8      i;
    s16     capacity;

    capacity = (s16)(64 - costHWord);

    // オフセット数は i が小さいほど大きいので、ソートせず、i=0 -> HuffTreeTop で計算すればよい
    for (i = 0; i < HuffTreeTop; i++)
    {
        if (HuffTreeCtrl[i].leftOffsetNeed)
        {
            if ((HuffTreeTop - i) <= capacity)
            {
                capacity--;
            }
            else
            {
                return 0;
            }
        }
        if (HuffTreeCtrl[i].rightOffsetNeed)
        {
            if ((HuffTreeTop - i) <= capacity)
            {
                capacity--;
            }
            else
            {
                return 0;
            }
        }
    }

    return 1;
}

//-----------------------------------------------------------------------
// 1節点分、ハフマンコード表を作成
//-----------------------------------------------------------------------
static void HuffSetOneNodeOffset(u16 huffTreeNo, u8 rightNodeFlag)
{
    u16     nodeNo;
    u8      offsetData = 0;

    if (rightNodeFlag)
    {
        nodeNo = HuffTreeCtrl[huffTreeNo].rightNodeNo;
        HuffTreeCtrl[huffTreeNo].rightOffsetNeed = 0;
    }
    else
    {
        nodeNo = HuffTreeCtrl[huffTreeNo].leftNodeNo;
        HuffTreeCtrl[huffTreeNo].leftOffsetNeed = 0;
    }

    // 左の子節点
    if (HuffTable[HuffTable[nodeNo].ChNo[0]].LeafDepth == 0)
    {
        offsetData |= 0x80;
        HuffTree[HuffTreeTop * 2 + 0] = (u8)HuffTable[nodeNo].ChNo[0];
        HuffTreeCtrl[HuffTreeTop].leftNodeNo = (u8)HuffTable[nodeNo].ChNo[0];
        HuffTreeCtrl[HuffTreeTop].leftOffsetNeed = 0;   // オフセットは必要なくなる
    }
    else
    {
        HuffTreeCtrl[HuffTreeTop].leftNodeNo = (u16)HuffTable[nodeNo].ChNo[0];  // オフセットは必要
    }

    // 右の子節点
    if (HuffTable[HuffTable[nodeNo].ChNo[1]].LeafDepth == 0)
    {
        offsetData |= 0x40;
        HuffTree[HuffTreeTop * 2 + 1] = (u8)HuffTable[nodeNo].ChNo[1];
        HuffTreeCtrl[HuffTreeTop].rightNodeNo = (u8)HuffTable[nodeNo].ChNo[1];
        HuffTreeCtrl[HuffTreeTop].rightOffsetNeed = 0;  // オフセットは必要なくなる
    }
    else
    {
        HuffTreeCtrl[HuffTreeTop].rightNodeNo = (u16)HuffTable[nodeNo].ChNo[1]; // オフセットは必要
    }

    offsetData |= (u8)(HuffTreeTop - huffTreeNo - 1);
    HuffTree[huffTreeNo * 2 + rightNodeFlag] = offsetData;

    HuffTreeTop++;
}


//-----------------------------------------------------------------------
// 2文木作成時に、部分木を統合したときに、部分木の各構成ノードの深さを＋1する
//-----------------------------------------------------------------------
static void HuffUpdateParentDepth(u16 leftNo, u16 rightNo)
{
    HuffTable[leftNo].PaDepth++;
    HuffTable[rightNo].PaDepth++;

    if (HuffTable[leftNo].LeafDepth != 0)
    {
        HuffUpdateParentDepth((u16)HuffTable[leftNo].ChNo[0], (u16)HuffTable[leftNo].ChNo[1]);
    }
    if (HuffTable[rightNo].LeafDepth != 0)
    {
        HuffUpdateParentDepth((u16)HuffTable[rightNo].ChNo[0], (u16)HuffTable[rightNo].ChNo[1]);
    }
}

//-----------------------------------------------------------------------
// ハフマンコード生成
//-----------------------------------------------------------------------
static void HuffMakeCode(u16 nodeNo, u32 paHuffCode)
{
    HuffTable[nodeNo].HuffCode = (paHuffCode << 1) | HuffTable[nodeNo].Bit;

    if (HuffTable[nodeNo].LeafDepth != 0)
    {
        HuffMakeCode((u16)HuffTable[nodeNo].ChNo[0], HuffTable[nodeNo].HuffCode);
        HuffMakeCode((u16)HuffTable[nodeNo].ChNo[1], HuffTable[nodeNo].HuffCode);
    }
}

//-----------------------------------------------------------------------
// 中間ノードが HuffTree 作成に必要とするデータ量
//-----------------------------------------------------------------------
static u8 HuffCountHWord(u16 nodeNo)
{
    u8      leftHWord, rightHWord;

    switch (HuffTable[nodeNo].LeafDepth)
    {
    case 0:
        return 0;
    case 1:
        leftHWord = rightHWord = 0;
        break;
    default:
        leftHWord = HuffCountHWord((u16)HuffTable[nodeNo].ChNo[0]);
        rightHWord = HuffCountHWord((u16)HuffTable[nodeNo].ChNo[1]);
        break;
    }

    HuffTable[nodeNo].HWord = (u16)(leftHWord + rightHWord + 1);
    return (u8)(leftHWord + rightHWord + 1);
}
