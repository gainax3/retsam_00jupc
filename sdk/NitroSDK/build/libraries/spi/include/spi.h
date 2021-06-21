/*---------------------------------------------------------------------------*
  Project:  NitroSDK - libraries - spi
  File:     spi.h

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: spi.h,v $
  Revision 1.20  2007/04/12 00:08:39  yasu
  著作年度修正

  Revision 1.19  2007/04/03 07:33:07  terui
  パワーダウンコマンドを使用禁止に変更

  Revision 1.18  2006/01/18 02:12:29  kitase_hirotake
  do-indent

  Revision 1.17  2005/02/28 05:26:27  yosizaki
  do-indent.

  Revision 1.16  2004/12/29 02:05:04  takano_makoto
  SetStability関数のretryパラメータを廃止

  Revision 1.15  2004/10/07 06:49:34  terui
  NVRAMについて別メーカー製の場合のコマンド追加に伴う修正。

  Revision 1.14  2004/07/31 08:08:46  terui
  不要なコードの削除

  Revision 1.13  2004/06/01 04:45:59  terui
  マイク単体サンプリングのinline関数からbuf指定を削除

  Revision 1.12  2004/05/31 08:54:50  terui
  インライン関数以外をグローバルinclude内のtype.hに移動。

  Revision 1.11  2004/05/28 08:51:44  terui
  nitro/include以下のヘッダファイルに定義を切り出し。
  inline関数をARM9側ローカル関数化できればファイル自体を削除予定。

  Revision 1.10  2004/05/25 00:58:48  terui
  SPI各デバイス用ライブラリ細分化に伴う修正

  Revision 1.8  2004/05/12 05:28:24  terui
  SPI_Initの宣言を変更

  Revision 1.7  2004/04/29 10:28:43  terui
  デバイスタイプにARM7からの排他アクセスを考慮したタイプを追加

  Revision 1.6  2004/04/27 12:05:59  terui
  マイクサンプリング最新保存アドレス取得I/Fを追加

  Revision 1.5  2004/04/23 02:27:35  takano_makoto
  Change Default Stability Range

  Revision 1.4  2004/04/15 13:55:41  takano_makoto
  Chang default stability parameter

  Revision 1.3  2004/04/14 06:26:46  terui
  SPIライブラリのソース整理に伴う更新

  Revision 1.2  2004/04/09 02:59:15  terui
  Correct misspell.

  Revision 1.1  2004/04/05 04:46:32  terui
  Initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef LIBRARIES_SPI_H_
#define LIBRARIES_SPI_H_

#include    <nitro/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================*/

/*---------------------------------------------------------------------------*
    インライン関数定義
    ARM9用に暫定的に定義。
    本来はARM9側ライブラリ内でstatic関数として定義されるべき。
 *---------------------------------------------------------------------------*/
#ifdef  SDK_ARM9

/*---------------------------------------------------------------------------*
  Name:         SPI_TpSamplingNow

  Description:  タッチパネルを一回サンプリングする。

  Arguments:    None.

  Returns:      BOOL - 命令をPXI経由で正常に送信できた場合TRUE、
                       失敗した場合はFALSEを返す。
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_TpSamplingNow(void)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_TOUCHPANEL,
                               SPI_PXI_START_BIT |
                               SPI_PXI_END_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) | (SPI_PXI_COMMAND_TP_SAMPLING << 8), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_TpAutoSamplingOn

  Description:  タッチパネルの自動サンプリングを開始する。

  Arguments:    vCount -    サンプリングを行うVカウント。
                            1フレームに複数回サンプリングする場合、ここを
                            起点に1フレームが時分割される。
                frequency - 1フレームに何回サンプリングするかの頻度。

  Returns:      BOOL - 命令をPXI経由で正常に送信できた場合TRUE、
                       失敗した場合はFALSEを返す。
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_TpAutoSamplingOn(u16 vCount, u8 frequency)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_TOUCHPANEL,
                               SPI_PXI_START_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_TP_AUTO_ON << 8) | (u32)frequency, 0))
    {
        return FALSE;
    }

    // Send packet [1]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_TOUCHPANEL,
                               SPI_PXI_END_BIT | (1 << SPI_PXI_INDEX_SHIFT) | (u32)vCount, 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_TpAutoSamplingOff

  Description:  タッチパネルの自動サンプリングを停止する。

  Arguments:    None.

  Returns:      BOOL - 命令をPXI経由で正常に送信できた場合TRUE、
                       失敗した場合はFALSEを返す。
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_TpAutoSamplingOff(void)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_TOUCHPANEL,
                               SPI_PXI_START_BIT |
                               SPI_PXI_END_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) | (SPI_PXI_COMMAND_TP_AUTO_OFF << 8), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_TpSetupStability

  Description:  サンプリングにおける安定判定パラメータを設定する。

  Arguments:    range - 連続したサンプリングにおいて、検出電圧が安定したとみなす誤差。
                        なお、検出値は12bitで 0 ～ 4095。

  Returns:      BOOL - 命令をPXI経由で正常に送信できた場合TRUE、
                       失敗した場合はFALSEを返す。
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_TpSetupStability(u16 range)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_TOUCHPANEL,
                               SPI_PXI_START_BIT |
                               SPI_PXI_END_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_TP_SETUP_STABILITY << 8) | (u32)range, 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_NvramWriteEnable

  Description:  NVRAMに「書き込み許可」命令を発行する。

  Arguments:    None.

  Returns:      BOOL - 命令をPXI経由で正常に送信できた場合TRUE、
                       失敗した場合はFALSEを返す。
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_NvramWriteEnable(void)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               SPI_PXI_END_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) | (SPI_PXI_COMMAND_NVRAM_WREN << 8), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_NvramWriteDisable

  Description:  NVRAMに「書き込み禁止」命令を発行する。

  Arguments:    None.

  Returns:      BOOL - 命令をPXI経由で正常に送信できた場合TRUE、
                       失敗した場合はFALSEを返す。
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_NvramWriteDisable(void)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               SPI_PXI_END_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) | (SPI_PXI_COMMAND_NVRAM_WRDI << 8), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_NvramReadStatusRegister

  Description:  NVRAMに「ステータスレジスタ読み出し」命令を発行する。

  Arguments:    pData - 読み出した値を格納する変数へのポインタ。
                        値はARM7が直接書き出すのでキャッシュに注意。

  Returns:      BOOL - 命令をPXI経由で正常に送信できた場合TRUE、
                       失敗した場合はFALSEを返す。
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_NvramReadStatusRegister(u8 *pData)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_NVRAM_RDSR << 8) | ((u32)pData >> 24), 0))
    {
        return FALSE;
    }

    // Send packet [1]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (1 << SPI_PXI_INDEX_SHIFT) | (((u32)pData >> 8) & 0x0000ffff), 0))
    {
        return FALSE;
    }

    // Send packet [2]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_END_BIT |
                               (2 << SPI_PXI_INDEX_SHIFT) | (((u32)pData << 8) & 0x0000ff00), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_NvramReadDataBytes

  Description:  NVRAMに「読み出し」命令を発行する。

  Arguments:    address - NVRAM上の読み出し開始アドレス。24bitのみ有効。
                size -    連続して読み出すバイト数。
                pData -   読み出した値を格納する配列。
                          値はARM7が直接書き出すのでキャッシュに注意。

  Returns:      BOOL - 命令をPXI経由で正常に送信できた場合TRUE、
                       失敗した場合はFALSEを返す。
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_NvramReadDataBytes(u32 address, u32 size, u8 *pData)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_NVRAM_READ << 8) |
                               ((address >> 16) & 0x000000ff), 0))
    {
        return FALSE;
    }

    // Send packet [1]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (1 << SPI_PXI_INDEX_SHIFT) | (address & 0x0000ffff), 0))
    {
        return FALSE;
    }

    // Send packet [2]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM, (2 << SPI_PXI_INDEX_SHIFT) | (size >> 16), 0))
    {
        return FALSE;
    }

    // Send packet [3]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (3 << SPI_PXI_INDEX_SHIFT) | (size & 0x0000ffff), 0))
    {
        return FALSE;
    }

    // Send packet [4]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (4 << SPI_PXI_INDEX_SHIFT) | ((u32)pData >> 16), 0))
    {
        return FALSE;
    }

    // Send packet [5]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_END_BIT |
                               (5 << SPI_PXI_INDEX_SHIFT) | ((u32)pData & 0x0000ffff), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_NvramReadHigherSpeed

  Description:  NVRAMに「高速読み出し」命令を発行する。
                「高速読み出し」は命令発行後、データ読み出し前に8bitのダミーの
                クロックを刻むことで25MHzまでのクロックで読み出し可能という命令
                だが、SPIのクロックは最高4MHzまでなので、使用する意味はない。
                ちなみに普通の「読み出し」命令は最高20MHzまでのクロックに対応。
                上記はデバイスがM45PE40の場合で、LE25FW203Tの場合は「読み出し」
                「高速読み出し」共に30MHz。

  Arguments:    address - NVRAM上の読み出し開始アドレス。24bitのみ有効。
                size -    連続して読み出すバイト数。
                pData -   読み出した値を格納する配列。
                          値はARM7が直接書き出すのでキャッシュに注意。

  Returns:      BOOL - 命令をPXI経由で正常に送信できた場合TRUE、
                       失敗した場合はFALSEを返す。
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_NvramReadHigherSpeed(u32 address, u32 size, u8 *pData)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_NVRAM_FAST_READ << 8) |
                               ((address >> 16) & 0x000000ff), 0))
    {
        return FALSE;
    }

    // Send packet [1]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (1 << SPI_PXI_INDEX_SHIFT) | (address & 0x0000ffff), 0))
    {
        return FALSE;
    }

    // Send packet [2]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM, (2 << SPI_PXI_INDEX_SHIFT) | (size >> 16), 0))
    {
        return FALSE;
    }

    // Send packet [3]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (3 << SPI_PXI_INDEX_SHIFT) | (size & 0x0000ffff), 0))
    {
        return FALSE;
    }

    // Send packet [4]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (4 << SPI_PXI_INDEX_SHIFT) | ((u32)pData >> 16), 0))
    {
        return FALSE;
    }

    // Send packet [5]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_END_BIT |
                               (5 << SPI_PXI_INDEX_SHIFT) | ((u32)pData & 0x0000ffff), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_NvramPageWrite

  Description:  NVRAMに「ページ書き込み」命令を発行する。
                NVRAMの内部では、「ページ消去」と「ページ書き込み(条件付)」が
                連続して行われる。

  Arguments:    address - NVRAM上の書き込み開始アドレス。24bitのみ有効。
                size -    連続して書き込むバイト数。
                          address + size がページ境界(256バイト)を越えると越えた
                          分のデータは無視される。
                pData -   書き込む値が格納されている配列。
                          ARM7が直接読み出すので、キャッシュからメモリ実体に
                          確実に書き出しておく必要がある。

  Returns:      BOOL - 命令をPXI経由で正常に送信できた場合TRUE、
                       失敗した場合はFALSEを返す。
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_NvramPageWrite(u32 address, u16 size, const u8 *pData)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_NVRAM_PW << 8) | ((address >> 16) & 0x000000ff), 0))
    {
        return FALSE;
    }

    // Send packet [1]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (1 << SPI_PXI_INDEX_SHIFT) | (address & 0x0000ffff), 0))
    {
        return FALSE;
    }

    // Send packet [2]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM, (2 << SPI_PXI_INDEX_SHIFT) | (u32)size, 0))
    {
        return FALSE;
    }

    // Send packet [3]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (3 << SPI_PXI_INDEX_SHIFT) | ((u32)pData >> 16), 0))
    {
        return FALSE;
    }

    // Send packet [4]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_END_BIT |
                               (4 << SPI_PXI_INDEX_SHIFT) | ((u32)pData & 0x0000ffff), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_NvramPageWrite

  Description:  NVRAMに「ページ書き込み(条件付)」命令を発行する。
                普通の「ページ書き込み」より圧倒的に早く書き込めるが、ビットを
                下げる操作しかできない。'0'のビットに'1'を書き込んでも'0'のまま。

  Arguments:    address - NVRAM上の書き込み開始アドレス。24bitのみ有効。
                size -    連続して書き込むバイト数。
                          address + size がページ境界(256バイト)を越えると越えた
                          分のデータは無視される。
                pData -   書き込む値が格納されている配列。
                          ARM7が直接読み出すので、キャッシュからメモリ実体に
                          確実に書き出しておく必要がある。

  Returns:      BOOL - 命令をPXI経由で正常に送信できた場合TRUE、
                       失敗した場合はFALSEを返す。
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_NvramPageProgram(u32 address, u16 size, const u8 *pData)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_NVRAM_PP << 8) | ((address >> 16) & 0x000000ff), 0))
    {
        return FALSE;
    }

    // Send packet [1]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (1 << SPI_PXI_INDEX_SHIFT) | (address & 0x0000ffff), 0))
    {
        return FALSE;
    }

    // Send packet [2]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM, (2 << SPI_PXI_INDEX_SHIFT) | (u32)size, 0))
    {
        return FALSE;
    }

    // Send packet [3]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (3 << SPI_PXI_INDEX_SHIFT) | ((u32)pData >> 16), 0))
    {
        return FALSE;
    }

    // Send packet [4]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_END_BIT |
                               (4 << SPI_PXI_INDEX_SHIFT) | ((u32)pData & 0x0000ffff), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_NvramPageErase

  Description:  NVRAMに「ページ消去」命令を発行する。
                消去されたページはビットが全て'1'になる。

  Arguments:    address - NVRAM上の消去するページのアドレス。24bitのみ有効。
                          このアドレスが含まれるページの256byteが全て消去される。
                          NVRAM内部ではアドレスの下位8bitがクリアされたアドレスが
                          消去対象アドレスとなる、と思われる。

  Returns:      BOOL - 命令をPXI経由で正常に送信できた場合TRUE、
                       失敗した場合はFALSEを返す。
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_NvramPageErase(u32 address)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_NVRAM_PE << 8) | ((address >> 16) & 0x000000ff), 0))
    {
        return FALSE;
    }

    // Send packet [1]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_END_BIT |
                               (1 << SPI_PXI_INDEX_SHIFT) | (address & 0x0000ffff), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_NvramSectorErase

  Description:  NVRAMに「セクタ消去」命令を発行する。
                消去されたセクタはビットが全て'1'になる。

  Arguments:    address - NVRAM上の消去するセクタのアドレス。24bitのみ有効。
                          このアドレスが含まれるセクタの64Kbyteが全て消去される。
                          NVRAM内部ではアドレスの下位16bitがクリアされたアドレスが
                          消去対象アドレスとなる、と思われる。

  Returns:      BOOL - 命令をPXI経由で正常に送信できた場合TRUE、
                       失敗した場合はFALSEを返す。
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_NvramSectorErase(u32 address)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_NVRAM_SE << 8) | ((address >> 16) & 0x000000ff), 0))
    {
        return FALSE;
    }

    // Send packet [1]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_END_BIT |
                               (1 << SPI_PXI_INDEX_SHIFT) | (address & 0x0000ffff), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/* 新型番デバイス導入に伴い使用禁止 (2007/4/3 terui@nintendo.co.jp) */
#if 0
/*---------------------------------------------------------------------------*
  Name:         SPI_NvramPowerDown

  Description:  NVRAMに「省電力」命令を発行する。
                「省電力から復帰」命令以外を受け付けなくなる。

  Arguments:    None.

  Returns:      BOOL - 命令をPXI経由で正常に送信できた場合TRUE、
                       失敗した場合はFALSEを返す。
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_NvramPowerDown(void)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               SPI_PXI_END_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) | (SPI_PXI_COMMAND_NVRAM_DP << 8), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_NvramReleasePowerDown

  Description:  NVRAMに「省電力から復帰」命令を発行する。
                省電力モードになっていない時は無視される。

  Arguments:    None.

  Returns:      BOOL - 命令をPXI経由で正常に送信できた場合TRUE、
                       失敗した場合はFALSEを返す。
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_NvramReleasePowerDown(void)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               SPI_PXI_END_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) | (SPI_PXI_COMMAND_NVRAM_RDP << 8), 0))
    {
        return FALSE;
    }

    return TRUE;
}
#endif

/*---------------------------------------------------------------------------*
  Name:         SPI_NvramChipErase

  Description:  NVRAMに「チップ消去」命令を発行する。
                デバイスがLE25FW203Tの場合に有効。
                デバイスがM45PE40の場合は無視される。

  Arguments:    None.

  Returns:      BOOL - 命令をPXI経由で正常に送信できた場合TRUE、
                       失敗した場合はFALSEを返す。
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_NvramChipErase(void)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               SPI_PXI_END_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) | (SPI_PXI_COMMAND_NVRAM_CE << 8), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_NvramReadSiliconId

  Description:  NVRAMに「シリコンID読み出し」命令を発行する。
                デバイスがLE25FW203Tの場合に有効。
                デバイスがM45PE40の場合は無視される。

  Arguments:    pData - 読み出したシリコンIDを格納する2バイトのバッファを指定
                        する。値はARM7が直接書き出すのでキャッシュに注意。

  Returns:      BOOL  - 命令をPXI経由で正常に送信できた場合TRUE、
                        失敗した場合はFALSEを返す。
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_NvramReadSiliconId(u8 *pData)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_NVRAM_RSI << 8) | ((u32)pData >> 24), 0))
    {
        return FALSE;
    }

    // Send packet [1]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (1 << SPI_PXI_INDEX_SHIFT) | (((u32)pData >> 8) & 0x0000ffff), 0))
    {
        return FALSE;
    }

    // Send packet [2]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_END_BIT |
                               (2 << SPI_PXI_INDEX_SHIFT) | (((u32)pData << 8) & 0x0000ff00), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_NvramSoftwareReset

  Description:  NVRAMに「ソフトウェアリセット」命令を発行する。
                デバイスがLE25FW203Tの場合に有効。
                デバイスがM45PE40の場合は無視される。

  Arguments:    None.

  Returns:      BOOL - 命令をPXI経由で正常に送信できた場合TRUE、
                       失敗した場合はFALSEを返す。
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_NvramSoftwareReset(void)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               SPI_PXI_END_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) | (SPI_PXI_COMMAND_NVRAM_SR << 8), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_MicSamplingNow

  Description:  マイクを一回サンプリングする。

  Arguments:    type  - サンプリングタイプ( 0: 8ビット , 1: 12ビット )
                pData - サンプリング結果を格納するメモリのアドレス。
                   -> 削除

  Returns:      BOOL - 命令をPXI経由で正常に送信できた場合TRUE、
                       失敗した場合はFALSEを返す。
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_MicSamplingNow(u8 type)
{
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               SPI_PXI_START_BIT |
                               SPI_PXI_END_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_MIC_SAMPLING << 8) | (u32)type, 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_MicAutoSamplingOn

  Description:  マイクの自動サンプリングを開始する。

  Arguments:    pData    - サンプリングしたデータを格納するバッファのアドレス。
                size     - バッファのサイズ。バイト単位で指定。
                span     - サンプリング間隔(ARM7のCPUクロックで指定)。
                           タイマーの性質上、16ビット×1or64or256or1024 の数字
                           しか正確には設定できない。端数ビットは切り捨てられる。
                middle   - 途中応答を返すサンプリング回数。
                           -> 削除
                adMode   - AD変換のビット幅( 8 or 12 )を指定。
                loopMode - 自動サンプリングのループ制御可否。
                correct  - 自動サンプリング時のTick補正可否。

  Returns:      BOOL - 命令をPXI経由で正常に送信できた場合TRUE、
                       失敗した場合はFALSEを返す。
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_MicAutoSamplingOn(void *pData, u32 size, u32 span,
//    u32     middle ,
                                         u8 adMode, BOOL loopMode, BOOL correct)
{
    u8      temp;

    // フラグ類を"type"変数にまとめる
    switch (adMode)                    // AD変換ビット幅指定
    {
    case SPI_MIC_SAMPLING_TYPE_8BIT:
    case SPI_MIC_SAMPLING_TYPE_12BIT:
        temp = adMode;
        break;
    default:
        return FALSE;
    }
    if (loopMode)                      // 連続サンプリングのループ指定
    {
        temp |= (u8)SPI_MIC_SAMPLING_TYPE_LOOP_ON;
    }
    if (correct)                       // Tick誤差による補正ロジック適用指定
    {
        temp |= (u8)SPI_MIC_SAMPLING_TYPE_CORRECT_ON;
    }

    // Send packet[0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               SPI_PXI_START_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_MIC_AUTO_ON << 8) | (u32)temp, 0))
    {
        return FALSE;
    }

    // Send packet[1]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               (1 << SPI_PXI_INDEX_SHIFT) | ((u32)pData >> 16), 0))
    {
        return FALSE;
    }

    // Send packet[2]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               (2 << SPI_PXI_INDEX_SHIFT) | ((u32)pData & 0x0000ffff), 0))
    {
        return FALSE;
    }

    // Send packet[3]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC, (3 << SPI_PXI_INDEX_SHIFT) | (size >> 16), 0))
    {
        return FALSE;
    }

    // Send packet[4]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               (4 << SPI_PXI_INDEX_SHIFT) | (size & 0x0000ffff), 0))
    {
        return FALSE;
    }

    // Send packet[5]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC, (5 << SPI_PXI_INDEX_SHIFT) | (span >> 16), 0))
    {
        return FALSE;
    }

    // Send packet[6]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               SPI_PXI_END_BIT |
                               (6 << SPI_PXI_INDEX_SHIFT) | (span & 0x0000ffff), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_MicAutoSamplingOff

  Description:  マイクの自動サンプリングを停止する。
                開始時にループ指定しなかった場合はバッファが埋まった時点で
                自動的に停止する。

  Arguments:    None.

  Returns:      BOOL - 命令をPXI経由で正常に送信できた場合TRUE、
                       失敗した場合はFALSEを返す。
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_MicAutoSamplingOff(void)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               SPI_PXI_START_BIT |
                               SPI_PXI_END_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) | (SPI_PXI_COMMAND_MIC_AUTO_OFF << 8), 0))
    {
        return FALSE;
    }

    return TRUE;
}

#endif /* SDK_ARM9 */

/*===========================================================================*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LIBRARIES_SPI_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
