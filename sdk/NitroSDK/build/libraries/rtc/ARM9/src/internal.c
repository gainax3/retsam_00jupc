/*---------------------------------------------------------------------------*
  Project:  NitroSDK - RTC - libraries
  File:     internal.c

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: internal.c,v $
  Revision 1.2  2005/02/28 05:26:08  yosizaki
  do-indent.

  Revision 1.1  2004/05/12 02:36:08  terui
  initial upload

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include	<nitro/rtc/ARM9/api.h>
#include	<nitro/pxi.h>


/*---------------------------------------------------------------------------*
	内部関数定義
 *---------------------------------------------------------------------------*/
static BOOL RtcSendPxiCommand(u32 command);


/*---------------------------------------------------------------------------*
  Name:         RTCi_ResetAsync

  Description:  RTCのリセットを非同期で行う。
                ARM7からの応答はPXIコールバック(tag:RTC)にて通知される。

  Arguments:    None.

  Returns:      BOOL     - PXIに対して送信が完了した場合TRUEを、
                           PXIによる送信に失敗した場合FALSEを返す。
 *---------------------------------------------------------------------------*/
BOOL RTCi_ResetAsync(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_RESET);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_SetHourFormatAsync

  Description:  時刻フォーマットを非同期で変更する。
                ARM7からの応答はPXIコールバック(tag:RTC)にて通知される。
                OS_GetSystemWork()->real_time_clockにあるstatus1で引数を設定する。

  Arguments:    None.

  Returns:      BOOL     - PXIに対して送信が完了した場合TRUEを、
                           PXIによる送信に失敗した場合FALSEを返す。
 *---------------------------------------------------------------------------*/
BOOL RTCi_SetHourFormatAsync(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_SET_HOUR_FORMAT);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_ReadRawDateTimeAsync

  Description:  未加工の日付・時刻データを非同期で取得する。
                ARM7からの応答はPXIコールバック(tag:RTC)にて通知される。
                データはOS_GetSystemWork()->real_time_clockに格納される。

  Arguments:    None.

  Returns:      BOOL     - PXIに対して送信が完了した場合TRUEを、
                           PXIによる送信に失敗した場合FALSEを返す。
 *---------------------------------------------------------------------------*/
BOOL RTCi_ReadRawDateTimeAsync(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_READ_DATETIME);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_WriteRawDateTimeAsync

  Description:  未加工の日付・時刻データを非同期でデバイスに書き込む。
                ARM7からの応答はPXIコールバック(tag:RTC)にて通知される。
                OS_GetSystemWork()->real_time_clockにあるデータが書き込まれる。

  Arguments:    None.

  Returns:      BOOL     - PXIに対して送信が完了した場合TRUEを、
                           PXIによる送信に失敗した場合FALSEを返す。
 *---------------------------------------------------------------------------*/
BOOL RTCi_WriteRawDateTimeAsync(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_WRITE_DATETIME);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_ReadRawDateAsync

  Description:  未加工の日付データを非同期で取得する。
                ARM7からの応答はPXIコールバック(tag:RTC)にて通知される。
                データはOS_GetSystemWork()->real_time_clockに格納される。

  Arguments:    None.

  Returns:      BOOL     - PXIに対して送信が完了した場合TRUEを、
                           PXIによる送信に失敗した場合FALSEを返す。
 *---------------------------------------------------------------------------*/
BOOL RTCi_ReadRawDateAsync(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_READ_DATE);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_WriteRawDateAsync

  Description:  未加工の日付データを非同期でデバイスに書き込む。
                ARM7からの応答はPXIコールバック(tag:RTC)にて通知される。
                OS_GetSystemWork()->real_time_clockにあるデータが書き込まれる。

  Arguments:    None.

  Returns:      BOOL     - PXIに対して送信が完了した場合TRUEを、
                           PXIによる送信に失敗した場合FALSEを返す。
 *---------------------------------------------------------------------------*/
BOOL RTCi_WriteRawDateAsync(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_WRITE_DATE);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_ReadRawTimeAsync

  Description:  未加工の時刻データを非同期で取得する。
                ARM7からの応答はPXIコールバック(tag:RTC)にて通知される。
                データはOS_GetSystemWork()->real_time_clockに格納される。

  Arguments:    None.

  Returns:      BOOL     - PXIに対して送信が完了した場合TRUEを、
                           PXIによる送信に失敗した場合FALSEを返す。
 *---------------------------------------------------------------------------*/
BOOL RTCi_ReadRawTimeAsync(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_READ_TIME);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_WriteRawTimeAsync

  Description:  未加工の時刻データを非同期でデバイスに書き込む。
                ARM7からの応答はPXIコールバック(tag:RTC)にて通知される。
                OS_GetSystemWork()->real_time_clockにあるデータが書き込まれる。

  Arguments:    None.

  Returns:      BOOL     - PXIに対して送信が完了した場合TRUEを、
                           PXIによる送信に失敗した場合FALSEを返す。
 *---------------------------------------------------------------------------*/
BOOL RTCi_WriteRawTimeAsync(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_WRITE_TIME);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_ReadRawPulseAsync

  Description:  未加工の周波数定常割込み設定値を非同期で取得する。
                ARM7からの応答はPXIコールバック(tag:RTC)にて通知される。
                データはOS_GetSystemWork()->real_time_clockに格納される。

  Arguments:    None.

  Returns:      BOOL     - PXIに対して送信が完了した場合TRUEを、
                           PXIによる送信に失敗した場合FALSEを返す。
 *---------------------------------------------------------------------------*/
BOOL RTCi_ReadRawPulseAsync(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_READ_PULSE);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_WriteRawPulseAsync

  Description:  未加工の周波数定常割込み設定値を非同期でデバイスに書き込む。
                ARM7からの応答はPXIコールバック(tag:RTC)にて通知される。
                OS_GetSystemWork()->real_time_clockにあるデータが書き込まれる。

  Arguments:    None.

  Returns:      BOOL     - PXIに対して送信が完了した場合TRUEを、
                           PXIによる送信に失敗した場合FALSEを返す。
 *---------------------------------------------------------------------------*/
BOOL RTCi_WriteRawPulseAsync(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_WRITE_PULSE);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_ReadRawAlarm1Async

  Description:  未加工のアラーム１割込み設定値を非同期で取得する。
                ARM7からの応答はPXIコールバック(tag:RTC)にて通知される。
                データはOS_GetSystemWork()->real_time_clockに格納される。

  Arguments:    None.

  Returns:      BOOL     - PXIに対して送信が完了した場合TRUEを、
                           PXIによる送信に失敗した場合FALSEを返す。
 *---------------------------------------------------------------------------*/
BOOL RTCi_ReadRawAlarm1Async(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_READ_ALARM1);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_WriteRawAlarm1Async

  Description:  未加工のアラーム１割込み設定値を非同期でデバイスに書き込む。
                ARM7からの応答はPXIコールバック(tag:RTC)にて通知される。
                OS_GetSystemWork()->real_time_clockにあるデータが書き込まれる。

  Arguments:    None.

  Returns:      BOOL     - PXIに対して送信が完了した場合TRUEを、
                           PXIによる送信に失敗した場合FALSEを返す。
 *---------------------------------------------------------------------------*/
BOOL RTCi_WriteRawAlarm1Async(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_WRITE_ALARM1);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_ReadRawAlarm2Async

  Description:  未加工のアラーム２割込み設定値を非同期で取得する。
                ARM7からの応答はPXIコールバック(tag:RTC)にて通知される。
                データはOS_GetSystemWork()->real_time_clockに格納される。

  Arguments:    None.

  Returns:      BOOL     - PXIに対して送信が完了した場合TRUEを、
                           PXIによる送信に失敗した場合FALSEを返す。
 *---------------------------------------------------------------------------*/
BOOL RTCi_ReadRawAlarm2Async(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_READ_ALARM2);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_WriteRawAlarm2Async

  Description:  未加工のアラーム２割込み設定値を非同期でデバイスに書き込む。
                ARM7からの応答はPXIコールバック(tag:RTC)にて通知される。
                OS_GetSystemWork()->real_time_clockにあるデータが書き込まれる。

  Arguments:    None.

  Returns:      BOOL     - PXIに対して送信が完了した場合TRUEを、
                           PXIによる送信に失敗した場合FALSEを返す。
 *---------------------------------------------------------------------------*/
BOOL RTCi_WriteRawAlarm2Async(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_WRITE_ALARM2);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_ReadRawStatus1Async

  Description:  未加工のステータス１レジスタ設定値を非同期で取得する。
                ARM7からの応答はPXIコールバック(tag:RTC)にて通知される。
                データはOS_GetSystemWork()->real_time_clockに格納される。

  Arguments:    None.

  Returns:      BOOL     - PXIに対して送信が完了した場合TRUEを、
                           PXIによる送信に失敗した場合FALSEを返す。
 *---------------------------------------------------------------------------*/
BOOL RTCi_ReadRawStatus1Async(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_READ_STATUS1);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_WriteRawStatus1Async

  Description:  未加工のステータス１レジスタ設定値を非同期でデバイスに書き込む。
                ARM7からの応答はPXIコールバック(tag:RTC)にて通知される。
                OS_GetSystemWork()->real_time_clockにあるデータが書き込まれる。

  Arguments:    None.

  Returns:      BOOL     - PXIに対して送信が完了した場合TRUEを、
                           PXIによる送信に失敗した場合FALSEを返す。
 *---------------------------------------------------------------------------*/
BOOL RTCi_WriteRawStatus1Async(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_WRITE_STATUS1);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_ReadRawStatus2Async

  Description:  未加工のステータス２レジスタ設定値を非同期で取得する。
                ARM7からの応答はPXIコールバック(tag:RTC)にて通知される。
                データはOS_GetSystemWork()->real_time_clockに格納される。

  Arguments:    None.

  Returns:      BOOL     - PXIに対して送信が完了した場合TRUEを、
                           PXIによる送信に失敗した場合FALSEを返す。
 *---------------------------------------------------------------------------*/
BOOL RTCi_ReadRawStatus2Async(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_READ_STATUS2);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_WriteRawStatus2Async

  Description:  未加工のステータス２レジスタ設定値を非同期でデバイスに書き込む。
                ARM7からの応答はPXIコールバック(tag:RTC)にて通知される。
                OS_GetSystemWork()->real_time_clockにあるデータが書き込まれる。

  Arguments:    None.

  Returns:      BOOL     - PXIに対して送信が完了した場合TRUEを、
                           PXIによる送信に失敗した場合FALSEを返す。
 *---------------------------------------------------------------------------*/
BOOL RTCi_WriteRawStatus2Async(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_WRITE_STATUS2);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_ReadRawAdjustAsync

  Description:  未加工のクロック調整用レジスタ設定値を非同期で取得する。
                ARM7からの応答はPXIコールバック(tag:RTC)にて通知される。
                データはOS_GetSystemWork()->real_time_clockに格納される。

  Arguments:    None.

  Returns:      BOOL     - PXIに対して送信が完了した場合TRUEを、
                           PXIによる送信に失敗した場合FALSEを返す。
 *---------------------------------------------------------------------------*/
BOOL RTCi_ReadRawAdjustAsync(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_READ_ADJUST);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_WriteRawAdjustAsync

  Description:  未加工のクロック調整用レジスタ設定値を非同期でデバイスに書き込む。
                ARM7からの応答はPXIコールバック(tag:RTC)にて通知される。
                OS_GetSystemWork()->real_time_clockにあるデータが書き込まれる。

  Arguments:    None.

  Returns:      BOOL     - PXIに対して送信が完了した場合TRUEを、
                           PXIによる送信に失敗した場合FALSEを返す。
 *---------------------------------------------------------------------------*/
BOOL RTCi_WriteRawAdjustAsync(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_WRITE_ADJUST);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_ReadRawFreeAsync

  Description:  未加工のフリーレジスタ設定値を非同期で取得する。
                ARM7からの応答はPXIコールバック(tag:RTC)にて通知される。
                データはOS_GetSystemWork()->real_time_clockに格納される。

  Arguments:    None.

  Returns:      BOOL     - PXIに対して送信が完了した場合TRUEを、
                           PXIによる送信に失敗した場合FALSEを返す。
 *---------------------------------------------------------------------------*/
BOOL RTCi_ReadRawFreeAsync(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_READ_FREE);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_WriteRawFreeAsync

  Description:  未加工のフリーレジスタ設定値を非同期でデバイスに書き込む。
                ARM7からの応答はPXIコールバック(tag:RTC)にて通知される。
                OS_GetSystemWork()->real_time_clockにあるデータが書き込まれる。

  Arguments:    None.

  Returns:      BOOL     - PXIに対して送信が完了した場合TRUEを、
                           PXIによる送信に失敗した場合FALSEを返す。
 *---------------------------------------------------------------------------*/
BOOL RTCi_WriteRawFreeAsync(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_WRITE_FREE);
}

/*---------------------------------------------------------------------------*
  Name:         RtcSendPxiCommand

  Description:  指定コマンドをPXI経由でARM7に送信する。

  Arguments:    None.

  Returns:      BOOL     - PXIに対して送信が完了した場合TRUEを、
                           PXIによる送信に失敗した場合FALSEを返す。
 *---------------------------------------------------------------------------*/
static BOOL RtcSendPxiCommand(u32 command)
{
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_RTC,
                               ((command << RTC_PXI_COMMAND_SHIFT) & RTC_PXI_COMMAND_MASK), 0))
    {
        return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
