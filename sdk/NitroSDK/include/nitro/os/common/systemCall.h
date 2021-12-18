/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - include
  File:     systemCall.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: systemCall.h,v $
  Revision 1.24  2006/03/13 05:59:37  okubata_ryoma
  マクロの微修正

  Revision 1.23  2006/01/18 02:12:28  kitase_hirotake
  do-indent

  Revision 1.22  2005/11/21 02:25:10  terui
  Reconstruct SVC_WaitVBlankIntr definition.

  Revision 1.21  2005/10/24 00:49:42  adachi_hiroaki
  SVC_WaitVBlankIntr()のSDK_WEAK_SYMBOL指定を外した

  Revision 1.20  2005/10/21 09:00:02  yada
  let SVC_WaitVBlankIntr be weak symbol.

  Revision 1.19  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.18  2005/02/28 05:26:01  yosizaki
  do-indent.

  Revision 1.17  2004/11/02 09:48:48  yada
  just fix comment

  Revision 1.16  2004/07/20 07:27:11  yada
  system call functions were renewed

  Revision 1.15  2004/05/26 11:10:02  terui
  change comment of SVC_WaitByLoop

  Revision 1.14  2004/04/15 11:52:27  yada
  delete SVC_ReadCard

  Revision 1.13  2004/04/07 02:03:17  yada
  fix header comment

  Revision 1.12  2004/03/26 13:11:31  yasu
  #if SDK_ARM7 -> #ifdef SDK_ARM7

  Revision 1.11  2004/03/25 09:43:20  yada
  ARM7の SVC_Halt(), SVC_WaitByLoop() 追加

  Revision 1.10  2004/03/17 10:36:50  yasu
  fix SVC_CpuCopyFast

  Revision 1.9  2004/02/10 11:34:05  yada
  SystemCall() を削除

  Revision 1.8  2004/02/05 07:27:19  yada
  履歴文字列のIRISだったものを NITRO から IRISに戻した。

  Revision 1.7  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.6  2004/02/05 00:19:53  yada
  #define で、{ } のみで囲われているものを do{ }while(0) にした。

  Revision 1.5  2004/01/19 06:14:09  yada
  SVC_CpuSet() 内部の定数にMI_ をつけていない所があったのを修正

  Revision 1.4  2003/12/11 00:35:20  yasu
  IRIS_TS -> SDK_TS などの修正

  Revision 1.3  2003/11/28 01:54:54  yada
  REDSDKの03-11-27反映

  Revision 1.2  2003/11/11 12:27:28  yada
  システムコール部分をいくつか interrupt.h から systemCall.h に移した。

  Revision 1.1  2003/11/07 07:55:16  yada
  暫定版です。きっと大きく変更になります。


  $NoKeywords: $

 *---------------------------------------------------------------------------*/

#ifndef NITRO_OS_SYSTEMCALL_H_
#define NITRO_OS_SYSTEMCALL_H_

#include <nitro/mi/stream.h>
#include <nitro/mi/uncompress.h>
#include <nitro/os/common/system.h>

#ifdef __cplusplus
extern "C" {
#endif


//================================================================================
/*---------------------------------------------------------------------------*
  Name:         SVC_WaitVBlankIntr

  Description:  wait for VBlank

              ・Vブランク割り込みが発生するまでホールト状態で待ち続けます。
              ・割り込み処理にてINTR_CHECK_BUF (DTCM_END - 0x8) へ該当
                するフラグをセットして下さい。
              ・複数の割り込みを併用した時、SVC_Halt()が繰り返し呼び出される
                場合と比べてシステムコール呼び出しのオーバーヘッドを軽減する
                ことができます。
              ・スレッド使用時にホールトへ入りますと、割り込みが発生するまで
                他のスレッドも止まってしまうことになりますので、アイドルスレッド
                以外はOS_WaitIrq(1, OS_IE_V_BLANK)の使用を推奨します。

  Arguments:    None

  Returns:      None

 *---------------------------------------------------------------------------*/
#ifdef  SDK_SVC_WAITVBLANK_COMPATIBLE
void    SVC_WaitVBlankIntr(void);
#else
#define SVC_WaitVBlankIntr  OS_WaitVBlankIntr
#endif

/*---------------------------------------------------------------------------*
  Name:         SVC_WaitByLoop

  Description:  loop specified times

              ・システムROM上で指定回数ループ処理を行います。
              ・1回のループに4サイクルかかります。
              ・通常時のサブプロセッサがメインメモリへの優先権を持っている状態においては、
                サブプロセッサをシステムROM上でプログラムを動作させることによって
                メインプロセッサがストールするのを軽減することができます。
              ・メインメモリ表示モードなどメインプロセッサへ優先権を与える
                必要がある場合には、メインプロセッサ側で呼び出すことが
                有効な状況があるかもしれませんが、ほとんどの期間がキャッシュや
                TCM上で動作していればメインプロセッサから呼び出す必要はありません。

              ・引数：
                  count  :     ループ回数

  Arguments:    count : times to loop 

  Returns:      None

 *---------------------------------------------------------------------------*/
void    SVC_WaitByLoop(s32 count);

/*---------------------------------------------------------------------------*
  Name:         SVC_CpuSet

  Description:  clear or copy memory by cpu

               ・DmaSetマクロ互換のパラメータでRAMクリアまたはコピーします。
               ・32bit転送では強制的に4Byte境界にてアクセスされますが、
                 16bit転送では引数を2Byte境界へ合わせて渡す必要があります。

               ・引数：
                   srcp          ソースアドレス
                   destp         デスティネーションアドレス
                   dmaCntData    MI_DMA_SRC_FIX／MI_DMA_32BIT_BUS／MI_DMA_COUNT_MASKのみ有効

                         MI_DMA_SRC_FIX(  0, 1) = (ソースアドレス・インクリメント, ソースアドレス固定)
                         MI_DMA_32BIT_BUS(0, 1) = (16bit転送, 32bit転送)
                         MI_DMA_COUNT_MASK & dmaCntData = 転送回数

               ・上位マクロ：
                   SVC_CpuClear, SVC_CpuClearArray, SVC_CpuCopy, SVC_CpuCopyArray

  Arguments:    srcp :       source address
                destp :      destination addrss
                dmaCntData : dma control data
                            (only MI_DMA_SRC_FIX/MI_DMA_32BIT_BUS/MI_DMA_COUNT_MASK
                             is available)

  Returns:      None

 *---------------------------------------------------------------------------*/
void    SVC_CpuSet(const void *srcp, void *destp, u32 dmaCntData);

/*---------------------------------------------------------------------------*
  Name:         SVC_CpuSetFast

  Description:  clear or copy memory by cpu quickly

              ・DmaSetマクロ互換のパラメータで高速にRAMクリアまたはコピーします。
              ・32Byte単位でアクセス可能な時には複数ロード／ストア命令が使用され、
                端数は4Byte単位でアクセスされます。
              ・4Byte境界以外で引数を与えても強制的に4Byte境界でのアクセスになります。

              ・引数：
                  srcp          ソースアドレス
                  destp         デスティネーションアドレス
                  dmaCntData     MI_DMA_SRC_FIX／MI_DMA_COUNT_MASKのみ有効

                        MI_DMA_SRC_FIX(0, 1) = (ソースアドレス・インクリメント, ソースアドレス固定)
                        MI_DMA_COUNT_MASK & dmaCntData = 転送回数

              ・上位マクロ：
                  SVC_CpuClearFast, SVC_CpuClearArrayFast, SVC_CpuCopyFast, SVC_CpuCopyArrayFast

  Arguments:    srcp :       source address
                destp :      destination addrss
                dmaCntData : dma control data
                            (only MI_DMA_SRC_FIX/MI_DMA_COUNT_MASK
                             is available)

  Returns:      None

 *---------------------------------------------------------------------------*/
void    SVC_CpuSetFast(const void *srcp, void *destp, u32 dmaCntData);

/*---------------------------------------------------------------------------*
  Name:         SVC_CpuClear

  Description:  clear memory by SVC_CpuSet

              ・CPUでRAMクリアするシステムコールを呼び出します。
              ・クリアデータはスタックに置かれ、それをデスティネーションへコピーします。

              ・引数：
                  data        クリアデータ
                  destp       デスティネーションアドレス
                  size        クリアバイト数
                  bit         転送ビット幅（16|32）

  Arguments:    data  : clear data
                destp : destination address
                size  : clear size ( by byte )
                bit   : bit width ( 16 or 32 )

  Returns:      None

 *---------------------------------------------------------------------------*/
#define SVC_CpuClear( data, destp, size, bit )                  \
do{                                                             \
    vu##bit tmp = (vu##bit )(data);                             \
    SVC_CpuSet((u8 *)&(tmp), (u8 *)(destp), (                   \
        MI_DMA_SRC_FIX        |                                 \
        MI_DMA_##bit##BIT_BUS | ((size)/((bit)/8) & 0x1fffff)));  \
} while(0)

/*---------------------------------------------------------------------------*
  Name:         SVC_CpuClearArray

  Description:  clear memory by SVC_CpuSet

              ・CPUでRAMクリアするシステムコールを呼び出します。
              ・クリアデータはスタックに置かれ、それをデスティネーションへコピーします。
              ・SVC_CpuClearArrayはデスティネーション配列全体をクリアします。

              ・引数：
                  data        クリアデータ
                  destp       デスティネーションアドレス
                  bit         転送ビット幅（16|32）

  Arguments:    data  : clear data
                destp : destination address
                bit   : bit width ( 16 or 32 )

  Returns:      None

 *---------------------------------------------------------------------------*/
#define SVC_CpuClearArray( data, destp, bit )                   \
        SVC_CpuClear( data, destp, sizeof(destp), bit )

/*---------------------------------------------------------------------------*
  Name:         SVC_CpuCopy

  Description:  copy memory by SVC_CpuSet

              ・CPUでコピーするシステムコールを呼び出します。
              ・SVC_CpuCopyArrayはソース配列全体をコピーします。

              ・引数：
                  srcp      :  ソースアドレス
                  destp     :  デスティネーションアドレス
                  size      :  転送バイト数
                  bit       :  転送ビット幅（16|32）

  Arguments:    srcp      : source address
                destp     : destination address
                size      : size to copy ( by byte )
                bit       : bit width ( 16 or 32 )

  Returns:      None

 *---------------------------------------------------------------------------*/
#define SVC_CpuCopy( srcp, destp, size, bit )                   \
                                                                \
    SVC_CpuSet((u8 *)(srcp), (u8 *)(destp),  (                  \
        MI_DMA_SRC_INC        |                                 \
        MI_DMA_##bit##BIT_BUS | ((size)/((bit)/8) & 0x1fffff)))

/*---------------------------------------------------------------------------*
  Name:         SVC_CpuCopyArray

  Description:  copy memory by SVC_CpuSet

              ・CPUでコピーするシステムコールを呼び出します。
              ・SVC_CpuCopyArrayはソース配列全体をコピーします。

              ・引数：
                  srcp      :  ソースアドレス
                  destp     :  デスティネーションアドレス
                  bit       :  転送ビット幅（16|32）

  Arguments:    srcp      : source address
                destp     : destination address
                bit       : bit width ( 16 or 32 )

  Returns:      None

 *---------------------------------------------------------------------------*/
#define SVC_CpuCopyArray( srcp, destp, bit  )                  \
        SVC_CpuCopy( srcp, destp, sizeof(srcp), bit )

/*---------------------------------------------------------------------------*
  Name:         SVC_CpuClearFast

  Description:  clear memory by SVC_CpuSetFast quickly

              ・CPUで高速にRAMクリアするシステムコールを呼び出します。
              ・クリアデータはスタックに置かれ、それをデスティネーションへコピーします。
              ・32Byte単位でアクセス可能な時には32Byte単位の複数ストア命令が使用され、
                端数は4Byte単位でアクセスされます。

              ・引数：
                  data        クリアデータ
                  destp       デスティネーションアドレス
                  size        クリアバイト数

  Arguments:    data  : clear data
                destp : destination address
                size  : clear size ( by byte )

  Returns:      None

 *---------------------------------------------------------------------------*/
#define SVC_CpuClearFast( data, destp, size )                   \
do{                                                             \
    vu32 tmp = (vu32 )(data);                                   \
    SVC_CpuSetFast((u8 *)&(tmp), (u8 *)(destp), (               \
        MI_DMA_SRC_FIX | ((size)/(32/8) & 0x1fffff)));   \
} while(0)

/*---------------------------------------------------------------------------*
  Name:         SVC_CpuClearArrayFast

  Description:  clear memory by SVC_CpuSetFast quickly

              ・CPUで高速にRAMクリアするシステムコールを呼び出します。
              ・クリアデータはスタックに置かれ、それをデスティネーションへコピーします。
              ・32Byte単位でアクセス可能な時には複数ストア命令が使用され、
                端数は4Byte単位でアクセスされます。
              ・SVC_CpuClearArrayFastはデスティネーション配列全体をクリアします。

              ・引数：
                  data        クリアデータ
                  destp       デスティネーションアドレス

  Arguments:    data  : clear data
                size  : size to clear ( by byte )

  Returns:      None
 *---------------------------------------------------------------------------*/
#define SVC_CpuClearArrayFast( data, destp )                    \
        SVC_CpuClearFast( data, destp, sizeof(destp) )

/*---------------------------------------------------------------------------*
  Name:         SVC_CpuCopyFast

  Description:  clear memory by SVC_CpuSetFast quickly

              ・CPUで高速にコピーするシステムコールを呼び出します。
              ・32Byte単位でアクセス可能な時には複数ロード／ストア命令が使用され、
                端数は4Byte単位でアクセスされます。

              ・引数：
                  srcp        ソースアドレス
                  destp       デスティネーションアドレス
                  size        転送バイト数

  Arguments:    srcp  : source address
                destp : destination address
                size  : size to copy ( by byte )

  Returns:      None

 *---------------------------------------------------------------------------*/
#define SVC_CpuCopyFast( srcp, destp, size )                    \
                                                                \
    SVC_CpuSetFast((u8 *)(srcp), (u8 *)(destp),  (              \
        MI_DMA_SRC_INC | ((size)/(32/8) & 0x1fffff)))

/*---------------------------------------------------------------------------*
  Name:         SVC_CpuCopyArrayFast

  Description:  clear memory by SVC_CpuSetFast quickly

              ・CPUで高速にコピーするシステムコールを呼び出します。
              ・32Byte単位でアクセス可能な時には複数ロード／ストア命令が使用され、
                端数は4Byte単位でアクセスされます。
              ・SVC_CpuCopyArrayFastはソース配列全体をコピーします。

              ・引数：
                  srcp        ソースアドレス
                  destp       デスティネーションアドレス

  Arguments:    srcp  : source address
                destp : destination address

  Returns:      None

 *---------------------------------------------------------------------------*/
#define SVC_CpuCopyArrayFast( srcp, destp )                     \
        SVC_CpuCopyFast( srcp, destp, sizeof(srcp) )

/*---------------------------------------------------------------------------*
  Name:         SVC_UnpackBits

  Description:  unpack bits

              ・0固定のbitを詰めたデータを展開します。
              ・デスティネーションアドレスは4Byte境界に合わせて下さい。

              ・引数：
                   srcp          ソースアドレス
                   destp         デスティネーションアドレス
                   paramp        MIUnpackBitsParam構造体のアドレス

              ・MIUnpackBitsParam構造体
                    u16 srcNum              ソースデータ・バイト数
                    u8  srcBitNum           １ソースデータ・ビット数
                    u8  destBitNum          １デスティネーションデータ・ビット数
                    u32 destOffset:31       ソースデータに加算するオフセット数
                        destOffset0_On:1    ０のデータにオフセットを加算するか否かのフラグ

  Arguments:    srcp   : source address
                destp  : destination address
                paramp : parameter structure address

  Returns:      None

 *---------------------------------------------------------------------------*/
void    SVC_UnpackBits(const void *srcp, void *destp, const MIUnpackBitsParam *paramp);

/*---------------------------------------------------------------------------*
  Name:         SVC_UncompressLZ8

  Description:  uncompress LZ77

              ・LZ77圧縮データを展開し、8bit単位で書き込みます。
              ・バイトアクセスできないVRAM等に直接展開することはできません。
              ・圧縮データのサイズが4の倍数にならなかった場合は
                出来るだけ0で詰めて調整して下さい。
              ・ソースアドレスは4Byte境界に合わせて下さい。

              ・引数：
                   srcp          ソースアドレス
                   destp         デスティネーションアドレス

               ・データヘッダ
                   u32 :4                  予約
                        compType:4          圧縮タイプ（ = 1）
                        destSize:24         展開後のデータサイズ

               ・フラグデータフォーマット
                    u8  flags               圧縮／無圧縮フラグ
                                            （0, 1） = （無圧縮データ, 圧縮データ）
               ・コードデータフォーマット（Big Endian）
                    u16 length:4            展開データ長 - 3（一致長3Byte以上時のみ圧縮）
                        offset:12           一致データオフセット - 1

  Arguments:    srcp  : source address
                destp : destination address

  Returns:      None

 *---------------------------------------------------------------------------*/
void    SVC_UncompressLZ8(const void *srcp, void *destp);

/*---------------------------------------------------------------------------*
  Name:         SVC_UncompressRL8

  Description:  uncompress run length

              ・ランレングス圧縮データを展開し、8bit単位で書き込みます。
              ・バイトアクセスできないVRAM等に直接展開することはできません。
              ・圧縮データのサイズが4の倍数にならなかった場合は
                出来るだけ0で詰めて調整して下さい。
              ・ソースアドレスは4Byte境界に合わせて下さい。

              ・引数：
                   srcp          ソースアドレス
                   destp         デスティネーションアドレス

              ・データヘッダ
                   u32 :4                  予約
                       compType:4          圧縮タイプ（ = 3）
                       destSize:24         展開後のデータサイズ

              ・フラグデータフォーマット
                   u8  length:7            展開データ長 - 1（無圧縮時）
                                           展開データ長 - 3（連続長3Byte以上時のみ圧縮）
                       flag:1              （0, 1） = （無圧縮データ, 圧縮データ）

  Arguments:    srcp  : source address
                destp : destination address

  Returns:      None

 *---------------------------------------------------------------------------*/
void    SVC_UncompressRL8(const void *srcp, void *destp);

#ifdef SDK_TS
/*---------------------------------------------------------------------------*
  Name:         SVC_UncompressLZ16FromDevice

  Description:  uncompress LZ77 from device

              ・LZ77圧縮データを展開し、16bit単位で書き込みます。
              ・メモリマッピングされていないデバイス上の圧縮データを
                テンポラリバッファを使わずに直接展開することができます。
              ・バイトアクセスできないRAMにも展開できますが、
                SVC_UncompressLZ8()より低速です。
              ・圧縮データは一致文字列を2Byte以前より検索したものにして下さい。
              ・圧縮データのサイズが4の倍数にならなかった場合は
                出来るだけ0で詰めて調整して下さい。
              ・ソースアドレスは4Byte境界に合わせて下さい。
              ・成功時は展開後のサイズを返し、失敗時は負の値を返します。
                initStream/terminateStreamコールバック関数内でエラーを
                検出した場合は負の値を返して下さい。

              ・引数：
                   srcp          ソースアドレス
                   destp         デスティネーションアドレス
                   paramp        MIReadStreamCallbacks構造体のinitStream関数へ渡すパラメータのアドレス
                   callbacks     MIReadStreamCallbacks構造体のアドレス

               ・データヘッダ
                   u32 :4                  予約
                       compType:4          圧縮タイプ（ = 1）
                       destSize:23         展開後のデータサイズ
                       :1                  使用不可

               ・フラグデータフォーマット
                    u8  flags               圧縮／無圧縮フラグ
                                            （0, 1） = （無圧縮データ, 圧縮データ）
               ・コードデータフォーマット（Big Endian）
                    u16 length:4            展開データ長 - 3（一致長3Byte以上時のみ圧縮）
                        offset:12           一致データオフセット - 1

  Arguments:    srcp      : source address
                destp     : destination address
                callbacks : address of stream callbacks structure

  Returns:      uncompressed size >= 0
                error < 0

 *---------------------------------------------------------------------------*/
s32     SVC_UncompressLZ16FromDevice(const void *srcp, void *destp, const void *paramp,
                                     const MIReadStreamCallbacks *callbacks);

/*---------------------------------------------------------------------------*
  Name:         SVC_UncompressRL16FromDevice

  Description:  uncompress run length from device

              ・ランレングス圧縮データを展開し、16bit単位で書き込みます。
              ・メモリマッピングされていないデバイス上の圧縮データを
                テンポラリバッファを使わずに直接展開することができます。
              ・バイトアクセスできないRAMにも展開できますが、
                SVC_UncompressRL8()より低速です。
              ・圧縮データのサイズが4の倍数にならなかった場合は
                出来るだけ0で詰めて調整して下さい。
              ・ソースアドレスは4Byte境界に合わせて下さい。
              ・成功時は展開後のサイズを返し、失敗時は負の値を返します。
                initStream/terminateStreamコールバック関数内でエラーを
                検出した場合は負の値を返して下さい。

              ・引数：
                   srcp          ソースアドレス
                   destp         デスティネーションアドレス
                   paramp        MIReadStreamCallbacks構造体のinitStream関数へ渡すパラメータのアドレス
                   callbacks     MIReadStreamCallbacks構造体のアドレス

              ・データヘッダ
                   u32 :4                  予約
                       compType:4          圧縮タイプ（ = 3）
                       destSize:23         展開後のデータサイズ
                       :1                  使用不可

              ・フラグデータフォーマット
                   u8  length:7            展開データ長 - 1（無圧縮時）
                                           展開データ長 - 3（連続長3Byte以上時のみ圧縮）
                       flag:1              （0, 1） = （無圧縮データ, 圧縮データ）

  Arguments:    srcp      : source address
                destp     : destination address
                callbacks : address of stream callbacks structure

  Returns:      uncompressed size >= 0
                error < 0

 *---------------------------------------------------------------------------*/
s32     SVC_UncompressRL16FromDevice(const void *srcp, void *destp, const void *paramp,
                                     const MIReadStreamCallbacks *callbacks);

/*---------------------------------------------------------------------------*
  Name:         SVC_UncompressHuffmanFromDevice

  Description:  uncompress huffman from device

              ・ハフマン圧縮データを展開し、32bit単位で書き込みます。
              ・メモリマッピングされていないデバイス上の圧縮データを
                テンポラリバッファを使わずに直接展開することができます。
              ・圧縮データのサイズが4の倍数にならなかった場合は
                出来るだけ0で詰めて調整して下さい。
              ・ソースアドレスは4Byte境界に合わせて下さい。
              ・成功時は展開後のサイズを返し、失敗時は負の値を返します。
                initStream/terminateStreamコールバック関数内でエラーを
                検出した場合は負の値を返して下さい。

              ・引数：
                   srcp          ソースアドレス
                   destp         デスティネーションアドレス
                   tableBufp     ツリーテーブル格納バッファ（最大512Byte）
                                 MIReadStreamCallbacks構造体のinitStream関数へパラメータを渡したい場合、
                                 このバッファ経由して渡すことができます。
                                 ただし、initStream関数呼び出し後はツリーテーブルで上書きされます。
                   callbacks     MIReadStreamCallbacks構造体のアドレス

              ・データヘッダ
                   u32 bitSize:4           １データ・ビットサイズ（通常 4|8）
                       compType:4          圧縮タイプ（ = 2）
                       destSize:23         展開後のデータサイズ
                       :1                  使用不可

              ・ツリーテーブル
                   u8           treeSize        ツリーテーブルサイズ/2 - 1
                   TreeNodeData nodeRoot        ルートノード

                   TreeNodeData nodeLeft        ルート左ノード
                   TreeNodeData nodeRight       ルート右ノード

                   TreeNodeData nodeLeftLeft    左左ノード
                   TreeNodeData nodeLeftRight   左右ノード

                   TreeNodeData nodeRightLeft   右左ノード
                   TreeNodeData nodeRightRight  右右ノード

                           ・
                           ・

                この後に圧縮データ本体

              ・TreeNodeData構造体
                  u8  nodeNextOffset:6    次ノードデータへのオフセット - 1（2Byte単位）
                      rightEndFlag:1      右ノード終了フラグ
                      leftEndflag:1       左ノード終了フラグ
                                          終了フラグがセットされている場合
                                          次ノードにデータがある

  Arguments:    srcp      : source address
                destp     : destination address
                callbacks : address of stream callbacks structure

  Returns:      uncompressed size >= 0
                error < 0

 *---------------------------------------------------------------------------*/
s32     SVC_UncompressHuffmanFromDevice(const void *srcp, void *destp, u8 *tableBufp,
                                        const MIReadStreamCallbacks *callbacks);

/*---------------------------------------------------------------------------*
  Name:         SVC_GetCRC16

  Description:  calculate CRC-16

              ・CRC-16を算出します。
              ・データアドレスとサイズは2Byte境界に合わせて下さい。

              ・引数：
                   start         初期値
                   datap         データアドレス
                   size          サイズ（バイト数）

  Arguments:    start  : start value
                datap  : data address
                size   : data size (by byte)

  Returns:      CRC-16

 *---------------------------------------------------------------------------*/
u16     SVC_GetCRC16(u32 start, const void *datap, u32 size);

/*---------------------------------------------------------------------------*
  Name:         SVC_IsMmemExpanded

  Description:  check if main memory is expanded

              ・メインメモリが8Mバイトへ拡張されているかどうかを調べます。
              ・ARM9側では0x023FFFF8番地と0x027FFFF8番地をキャッシュ無効の設定に
                しておく必要があります。

  Arguments:    None

  Returns:      TRUE if main memory is expanded

 *---------------------------------------------------------------------------*/
BOOL    SVC_IsMmemExpanded(void);

#endif // SDK_TS

#if defined(SDK_ARM7) || (defined(SDK_ARM9) && defined(SDK_TS))
/*---------------------------------------------------------------------------*
  Name:         SVC_Div

  Description:  quotient of division

              ・numer/denomを算出します。
              ・レジスタの値は、r0=numer/denom, r1=number%denom, 
                r3=|numer/denom|で復帰します。
              ・コードサイズを抑えているため、あまり高速ではありません。

              ・引数：
                   number        分子
                   denom         分母

  Arguments:    numer  : 
                denom  : 

  Returns:      quotient

 *---------------------------------------------------------------------------*/
s32     SVC_Div(s32 number, s32 denom);

/*---------------------------------------------------------------------------*
  Name:         SVC_DivRem

  Description:  remainder of division

              ・numer%denomを算出します。
              ・レジスタの値は、r0=number%denom, r1=number%denom, 
                r3=|numer/denom|で復帰します。
              ・コードサイズを抑えているため、あまり高速ではありません。

              ・引数：
                   number        分子
                   denom         分母

  Arguments:    numer  : 
                denom  : 

  Returns:      remainder

 *---------------------------------------------------------------------------*/
s32     SVC_DivRem(s32 number, s32 denom);

/*---------------------------------------------------------------------------*
  Name:         SVC_Sqrt

  Description:  square root

              ・平方根を算出します。
              ・精度を良くするために引数を2の倍数だけ左シフトして渡し、
                戻り値もシフトして桁合わせを行なって下さい。
              ・コードサイズを抑えているため、あまり高速ではありません。

  Arguments:    src  : 

  Returns:      square root

 *---------------------------------------------------------------------------*/
u16     SVC_Sqrt(u32 src);

/*---------------------------------------------------------------------------*
  Name:         SVC_Halt

  Description:  halt

                ・CPUコアのみ停止させます。
                ・該当する割り込みが許可（IEにセット）されている
                  割り込み要求（IFセット）にて復帰します。
                ・CPSRのIRQディセーブルフラグがセットされている場合
                  （OS_DisableInterrupts）には、ホールトから復帰しますが
                  割り込みは発生しません。
                ・IMEがクリアされた状態でホールトに入りますと（OS_DisableIrq）
                  復帰できなくなります。

  Arguments:    None

  Returns:      None

 *---------------------------------------------------------------------------*/
void    SVC_Halt(void);

#endif // SDK_ARM7 || (SDK_ARM9 && SDK_TS)

#ifdef SDK_ARM7
/*---------------------------------------------------------------------------*
  Name:         SVC_Sleep

  Description:  sleep

              ・原発振を停止します。
              ・RTC／キー／カード／カートリッジ／本体オープンのいずれかの
                割り込みが許可（IEにセット）されている場合、該当する割り込み要求条件
                の発生によって復帰します。
              ・原発振が停止していますので復帰直後はIFフラグはセットされませんが、
                CPU再起動時まで端子へ割り込み要求信号が入ったままになっている場合には
                その時点でIFフラグがセットされます。
              ・あらかじめ両プロセッサともにPOWCNTレジスタを0クリアして
                全ブロックを停止させ、サウンドアンプや無線モジュールも停止、
                ARM9はホールト状態にしてから呼び出して下さい。
              ・POWCNTレジスタのLCDイネーブルフラグは、この関数を呼び出す
                100ms以上前に0へ落として下さい。守られていない場合は
                本体がシャットダウンしてしまう可能性があります。

  Arguments:    None

  Returns:      None

 *---------------------------------------------------------------------------*/
void    SVC_Sleep(void);

/*---------------------------------------------------------------------------*
  Name:         SVC_SetSoundBias

  Description:  set sound bias

              ・サウンドBIASを0から中間値（0x200）へ移行します。

              ・引数：
                   stepLoops     サウンドバイアス変更１ステップ間のループ数（４サイクル／ループ）。
                                 値が大きいほどサウンドバイアスを緩やかに変化させます。

  Arguments:    stepLoops : 

  Returns:      None

 *---------------------------------------------------------------------------*/
void    SVC_SetSoundBias(s32 stepLoops);

/*---------------------------------------------------------------------------*
  Name:         SVC_ResetSoundBias

  Description:  set sound bias

              ・サウンドBIASを中間値（0x200）から0へ移行します。

              ・引数：
                   stepLoops     サウンドバイアス変更１ステップ間のループ数（４サイクル／ループ）。
                                 値が大きいほどサウンドバイアスを緩やかに変化させます。

  Arguments:    stepLoops : 

  Returns:      None

 *---------------------------------------------------------------------------*/
void    SVC_ResetSoundBias(s32 stepLoops);

#ifdef SDK_TS
/*---------------------------------------------------------------------------*
  Name:         SVC_GetSinTable
                SVC_GetPitchTable
                SVC_GetVolumeTable

  Description:  get sound table data

              ・サウンド関連のテーブルを参照して値を返します。

              ・引数：
                   index         インデックス

  Arguments:    index : 

  Returns:      sound table data

 *---------------------------------------------------------------------------*/
s16     SVC_GetSinTable(int index);
u16     SVC_GetPitchTable(int index);
u8      SVC_GetVolumeTable(int index);

#endif // SDK_TS

#endif // SDK_ARM7


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_OS_SYSTEMCALL_H_ */
#endif
