/*---------------------------------------------------------------------------*
  Project:  NitroSDK - RTC - include
  File:     fifo.h

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: fifo.h,v $
  Revision 1.6  2005/02/28 05:26:08  yosizaki
  do-indent.

  Revision 1.5  2004/09/14 01:59:57  yasu
  Fix backslashs at end of line.

  Revision 1.4  2004/05/31 08:42:49  terui
  アラーム割込み用RESULTを削除し、COMMANDに追加。

  Revision 1.3  2004/05/17 11:02:51  terui
  処理結果にFATAL_ERRORを追加

  Revision 1.2  2004/05/17 08:43:34  terui
  処理結果にBUSYを追加

  Revision 1.1  2004/05/12 02:27:41  terui
  initial upload

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_RTC_COMMON_FIFO_H_
#define NITRO_RTC_COMMON_FIFO_H_

#ifdef  __cplusplus
extern "C" {
#endif

/*===========================================================================*/

/*---------------------------------------------------------------------------*
    定数定義
 *---------------------------------------------------------------------------*/
// プロトコル関連定義
#define RTC_PXI_COMMAND_MASK                0x00007f00  // コマンド格納部分のマスク
#define RTC_PXI_COMMAND_SHIFT               8   // コマンド格納部分の位置
#define RTC_PXI_RESULT_BIT_MASK             0x00008000  // PXIの応答を示す部分のマスク
#define RTC_PXI_RESULT_MASK                 0x000000ff  // 処理結果格納部分のマスク
#define RTC_PXI_RESULT_SHIFT                0   // 処理結果格納部分の位置

// PXIコマンド定義
#define RTC_PXI_COMMAND_RESET               0x00        // リセット
#define RTC_PXI_COMMAND_SET_HOUR_FORMAT     0x01        // 時間表記フォーマット設定
#define RTC_PXI_COMMAND_READ_DATETIME       0x10        // 日付・時刻読み出し
#define RTC_PXI_COMMAND_READ_DATE           0x11        // 日付読み出し
#define RTC_PXI_COMMAND_READ_TIME           0x12        // 時刻読み出し
#define RTC_PXI_COMMAND_READ_PULSE          0x13        // パルス出力設定値読み出し
#define RTC_PXI_COMMAND_READ_ALARM1         0x14        // アラーム１設定値読み出し
#define RTC_PXI_COMMAND_READ_ALARM2         0x15        // アラーム２設定値読み出し
#define RTC_PXI_COMMAND_READ_STATUS1        0x16        // ステータス１レジスタ読み出し
#define RTC_PXI_COMMAND_READ_STATUS2        0x17        // ステータス２レジスタ読み出し
#define RTC_PXI_COMMAND_READ_ADJUST         0x18        // 周波数定常割込みレジスタ読み出し
#define RTC_PXI_COMMAND_READ_FREE           0x19        // フリーレジスタ読み出し
#define RTC_PXI_COMMAND_WRITE_DATETIME      0x20        // 日付・時刻書き込み
#define RTC_PXI_COMMAND_WRITE_DATE          0x21        // 日付書き込み
#define RTC_PXI_COMMAND_WRITE_TIME          0x22        // 時刻書き込み
#define RTC_PXI_COMMAND_WRITE_PULSE         0x23        // パルス出力設定値書き込み
#define RTC_PXI_COMMAND_WRITE_ALARM1        0x24        // アラーム１設定値書き込み
#define RTC_PXI_COMMAND_WRITE_ALARM2        0x25        // アラーム２設定値書き込み
#define RTC_PXI_COMMAND_WRITE_STATUS1       0x26        // ステータス１レジスタ書き込み
#define RTC_PXI_COMMAND_WRITE_STATUS2       0x27        // ステータス２レジスタ書き込み
#define RTC_PXI_COMMAND_WRITE_ADJUST        0x28        // 周波数定常割込みレジスタ書き込み
#define RTC_PXI_COMMAND_WRITE_FREE          0x29        // フリーレジスタ書き込み
#define RTC_PXI_COMMAND_INTERRUPT           0x30        // アラーム割込み発生通達用

// 応答定義
typedef enum RTCPxiResult
{
    RTC_PXI_RESULT_SUCCESS = 0,        // 処理成功.
    RTC_PXI_RESULT_INVALID_COMMAND,    // 不正なPXIコマンド.
    RTC_PXI_RESULT_ILLEGAL_STATUS,     // RTCの状態により処理を実行不能.
    RTC_PXI_RESULT_BUSY,               // 他のリクエストを実行中.
    RTC_PXI_RESULT_FATAL_ERROR,        // その他何らかの原因で処理に失敗.
    RTC_PXI_RESULT_MAX
}
RTCPxiResult;


/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C" */
#endif

#endif /* NITRO_RTC_COMMON_FIFO_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
