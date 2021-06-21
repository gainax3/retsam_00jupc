/*---------------------------------------------------------------------------*
  Project:  NitroSDK - SPI - include
  File:     type.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: type.h,v $
  Revision 1.23  2006/01/18 02:12:38  kitase_hirotake
  do-indent

  Revision 1.22  2005/12/14 02:04:23  yada
  add SPI_PXI_COMMAND_PM_REG4VALUE for PMIC2

  Revision 1.21  2005/02/28 05:26:03  yosizaki
  do-indent.

  Revision 1.20  2005/01/11 07:54:25  takano_makoto
  fix copyright header.

  Revision 1.19  2005/01/07 12:29:33  terui
  SPI_PXI_COMMAND_MIC_AUTO_ADJUST定義を追加。

  Revision 1.18  2004/12/29 02:09:22  takano_makoto
  SetStability関数のretryパラメータを廃止

  Revision 1.17  2004/12/28 12:34:32  takano_makoto
  チャタリング対策パラメータのデフォルト値を変更

  Revision 1.16  2004/12/03 05:40:03  terui
  MICの自動サンプリング時に抜けたデータを明示的に示す機能追加に対応。

  Revision 1.15  2004/10/20 06:34:10  terui
  LCDのライン数定義を削除

  Revision 1.14  2004/10/07 06:49:34  terui
  NVRAMについて別メーカー製の場合のコマンド追加に伴う修正。

  Revision 1.13  2004/09/29 06:39:01  takano_makoto
  SPI_TP_DEFAULT_STABILITY_RANGE, SPI_TP_DEFAULT_STABILITY_RETRYの値変更

  Revision 1.12  2004/08/26 08:19:13  yada
  add definition for get LED status

  Revision 1.11  2004/08/23 08:10:03  yada
  add some SPI_PXI_COMMAND_PM_XXXX definition

  Revision 1.10  2004/08/09 13:25:12  takano_makoto
  SPI_MIC_SAMPLING_TYPE_MIX_TP_XXXを追加

  Revision 1.9  2004/08/05 06:06:37  yada
  PRE_SLEEP_START -> SYNC

  Revision 1.8  2004/08/03 08:41:18  yada
  fix a little

  Revision 1.7  2004/08/02 06:08:31  yada
  standby->sleep

  Revision 1.6  2004/08/02 00:49:35  yada
  sleep->standby

  Revision 1.5  2004/07/29 09:01:28  yada
  add pxi command about PM

  Revision 1.4  2004/07/13 09:21:21  terui
  符号付きのサンプリング種別追加に伴う変更

  Revision 1.3  2004/05/31 08:39:48  terui
  プロトコルに関する定義の追加

  Revision 1.2  2004/05/28 09:23:11  terui
  TP最大サンプリング回数を変更(16→4)

  Revision 1.1  2004/05/28 08:49:32  terui
  ライブライのローカルヘッダから定義を切り出し

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NITRO_SPI_COMMON_TYPE_H_
#define NITRO_SPI_COMMON_TYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================*/

/*---------------------------------------------------------------------------*
    定数定義
 *---------------------------------------------------------------------------*/
// PXIでの通信プロトコル関連定義
#define SPI_PXI_START_BIT                   0x02000000  // 連続パケット開始ビット
#define SPI_PXI_END_BIT                     0x01000000  // 連続パケット終了ビット
#define SPI_PXI_INDEX_MASK                  0x000f0000  // 連続パケットのインデックス部
#define SPI_PXI_INDEX_SHIFT                 16  //  を示すビットの配置を定義
#define SPI_PXI_DATA_MASK                   0x0000ffff  // 連続パケットのデータ部
#define SPI_PXI_DATA_SHIFT                  0   //  を示すビットの配置を定義
#define SPI_PXI_RESULT_COMMAND_BIT          0x00008000  // 応答を示すビット
#define SPI_PXI_RESULT_COMMAND_MASK         0x00007f00  // 応答コマンドの種別部の
#define SPI_PXI_RESULT_COMMAND_SHIFT        8   //  ビット配置を定義
#define SPI_PXI_RESULT_DATA_MASK            0x000000ff  // 応答コマンドの結果部の
#define SPI_PXI_RESULT_DATA_SHIFT           0   //  ビット配置を定義
#define SPI_PXI_CONTINUOUS_PACKET_MAX       16  // 連続パケットの最大連続回数

// PXI経由でARM9より発行される命令群(0x10のビットが1のものはARM7からの通達コマンド)
#define SPI_PXI_COMMAND_TP_SAMPLING         0x0000
#define SPI_PXI_COMMAND_TP_AUTO_ON          0x0001
#define SPI_PXI_COMMAND_TP_AUTO_OFF         0x0002
#define SPI_PXI_COMMAND_TP_SETUP_STABILITY  0x0003
#define SPI_PXI_COMMAND_TP_AUTO_SAMPLING    0x0010      // 自動サンプリングの途中経過通達用

#define SPI_PXI_COMMAND_NVRAM_WREN          0x0020
#define SPI_PXI_COMMAND_NVRAM_WRDI          0x0021
#define SPI_PXI_COMMAND_NVRAM_RDSR          0x0022
#define SPI_PXI_COMMAND_NVRAM_READ          0x0023
#define SPI_PXI_COMMAND_NVRAM_FAST_READ     0x0024
#define SPI_PXI_COMMAND_NVRAM_PW            0x0025
#define SPI_PXI_COMMAND_NVRAM_PP            0x0026
#define SPI_PXI_COMMAND_NVRAM_PE            0x0027
#define SPI_PXI_COMMAND_NVRAM_SE            0x0028
#define SPI_PXI_COMMAND_NVRAM_DP            0x0029
#define SPI_PXI_COMMAND_NVRAM_RDP           0x002a
#define SPI_PXI_COMMAND_NVRAM_CE            0x002b
#define SPI_PXI_COMMAND_NVRAM_RSI           0x002c
#define SPI_PXI_COMMAND_NVRAM_SR            0x002d

#define SPI_PXI_COMMAND_MIC_SAMPLING        0x0040
#define SPI_PXI_COMMAND_MIC_AUTO_ON         0x0041
#define SPI_PXI_COMMAND_MIC_AUTO_OFF        0x0042
#define SPI_PXI_COMMAND_MIC_AUTO_ADJUST     0x0043
#define SPI_PXI_COMMAND_MIC_BUFFER_FULL     0x0051

// for PM
#define SPI_PXI_COMMAND_PM_SYNC             0x0060
#define SPI_PXI_COMMAND_PM_SLEEP_START      0x0061
#define SPI_PXI_COMMAND_PM_SLEEP_END        0x0062
#define SPI_PXI_COMMAND_PM_UTILITY          0x0063
#define SPI_PXI_COMMAND_PM_REG_WRITE        0x0064
#define SPI_PXI_COMMAND_PM_REG_READ         0x0065
#define SPI_PXI_COMMAND_PM_SELF_BLINK       0x0066
#define SPI_PXI_COMMAND_PM_GET_BLINK        0x0067
#define SPI_PXI_COMMAND_PM_REG0VALUE        0x0070
#define SPI_PXI_COMMAND_PM_REG1VALUE        0x0071
#define SPI_PXI_COMMAND_PM_REG2VALUE        0x0072
#define SPI_PXI_COMMAND_PM_REG3VALUE        0x0073
#define SPI_PXI_COMMAND_PM_REG4VALUE        0x0074

    // *********************************************
    // ********** PMIC用定義追加場所 ***************
    // *********************************************

// PXI経由でARM7より返される応答群
#define SPI_PXI_RESULT_SUCCESS              0x0000      // 成功
#define SPI_PXI_RESULT_INVALID_COMMAND      0x0001      // 命令が異常
#define SPI_PXI_RESULT_INVALID_PARAMETER    0x0002      // パラメータが異常
#define SPI_PXI_RESULT_ILLEGAL_STATUS       0x0003      // 命令を受け付けられない状態
#define SPI_PXI_RESULT_EXCLUSIVE            0x0004      // SPIデバイスが排他されている

// タッチパネル関連定義
#define SPI_TP_SAMPLING_FREQUENCY_MAX       4   // 自動サンプリング頻度の最大値
#define SPI_TP_DEFAULT_STABILITY_RANGE      20  // 安定判定時に許容される出力範囲差(初期値)
#define SPI_TP_VALARM_TAG                   0x54505641  // "TPVA" - TP用Vアラームのタグ

// NVRAM(フラッシュROM)関連定義
#define SPI_NVRAM_PAGE_SIZE                 256
#define SPI_NVRAM_SECTOR_SIZE               ( SPI_NVRAM_PAGE_SIZE * 256 )
#define SPI_NVRAM_ALL_SIZE                  ( SPI_NVRAM_SECTOR_SIZE * 8 )

// マイク関連定義
#define SPI_MIC_SAMPLING_TYPE_8BIT          0x0000
#define SPI_MIC_SAMPLING_TYPE_12BIT         0x0001
#define SPI_MIC_SAMPLING_TYPE_S8BIT         0x0002
#define SPI_MIC_SAMPLING_TYPE_S12BIT        0x0003
#define SPI_MIC_SAMPLING_TYPE_FILTER_ON     0x0000
#define SPI_MIC_SAMPLING_TYPE_FILTER_OFF    0x0004
#define SPI_MIC_SAMPLING_TYPE_ADMODE_MASK   0x0007
#define SPI_MIC_SAMPLING_TYPE_BIT_MASK      0x0001
#define SPI_MIC_SAMPLING_TYPE_SIGNED_MASK   0x0002
#define SPI_MIC_SAMPLING_TYPE_FILTER_MASK   0x0004

#define SPI_MIC_SAMPLING_TYPE_LOOP_OFF      0x0000
#define SPI_MIC_SAMPLING_TYPE_LOOP_ON       0x0010
#define SPI_MIC_SAMPLING_TYPE_LOOP_MASK     0x0010

#define SPI_MIC_SAMPLING_TYPE_CORRECT_OFF   0x0000
#define SPI_MIC_SAMPLING_TYPE_CORRECT_ON    0x0020
#define SPI_MIC_SAMPLING_TYPE_CORRECT_MASK  0x0020

#define SPI_MIC_SAMPLING_TYPE_MIX_TP_OFF    0x0000
#define SPI_MIC_SAMPLING_TYPE_MIX_TP_ON     0x0040
#define SPI_MIC_SAMPLING_TYPE_MIX_TP_MASK   0x0040


/*---------------------------------------------------------------------------*
    構造体定義
 *---------------------------------------------------------------------------*/
// SPI関連デバイス種別
typedef enum SPIDeviceType
{
    SPI_DEVICE_TYPE_TP = 0,            // タッチパネル
    SPI_DEVICE_TYPE_NVRAM,             // NVRAM(フラッシュROM)
    SPI_DEVICE_TYPE_MIC,               // マイク
    SPI_DEVICE_TYPE_PM,                // パワーマネジメントIC
    SPI_DEVICE_TYPE_ARM7,
    SPI_DEVICE_TYPE_MAX
}
SPIDeviceType;

// タッチパネルの接触正否
typedef enum SPITpTouch
{
    SPI_TP_TOUCH_OFF = 0,
    SPI_TP_TOUCH_ON
}
SPITpTouch;

// タッチパネルのデータ有効性正否
typedef enum SPITpValidity
{
    SPI_TP_VALIDITY_VALID = 0,
    SPI_TP_VALIDITY_INVALID_X,
    SPI_TP_VALIDITY_INVALID_Y,
    SPI_TP_VALIDITY_INVALID_XY
}
SPITpValidity;

// タッチパネル入力構造体
typedef union SPITpData
{
    struct
    {
        u32     x:12;
        u32     y:12;
        u32     touch:1;
        u32     validity:2;
        u32     dummy:5;

    }
    e;
    u32     raw;
    u8      bytes[4];
    u16     halfs[2];

}
SPITpData;


/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C" */
#endif

#endif /* NITRO_SPI_COMMON_TYPE_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
