/*---------------------------------------------------------------------------*
  Project:  NitroSDK - RTC - include
  File:     api.h

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: api.h,v $
  Revision 1.12  2007/10/03 01:07:02  okubata_ryoma
  Copyright fix

  Revision 1.11  2007/07/31 02:09:37  tokunaga_eiji
  RTCTimeEx 構造体と RTC_GetDateTimeExByTick 関数の定義を追加。

  Revision 1.10  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.9  2005/02/28 05:26:01  yosizaki
  do-indent.

  Revision 1.8  2004/11/02 04:14:29  terui
  RTC_GetTimeAsyncのプロトタイプの引数名を修正。

  Revision 1.7  2004/08/19 13:43:01  terui
  HourFormat関連の定義を削除。

  Revision 1.6  2004/08/19 12:50:36  terui
  24時間表記固定化に伴う修正。

  Revision 1.5  2004/08/04 05:32:13  miya
  工場設定用に関数追加

  Revision 1.4  2004/07/01 03:17:15  yosizaki
  change around RTCHourFormat to internal use.

  Revision 1.3  2004/05/17 11:05:55  terui
  各関数の返り値をRTCResult型に統一

  Revision 1.1  2004/05/12 02:27:56  terui
  initial upload

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_RTC_ARM9_API_H_
#define NITRO_RTC_ARM9_API_H_

#ifdef  __cplusplus
extern "C" {
#endif

/*===========================================================================*/

#include    <nitro/rtc/common/type.h>
#include    <nitro/rtc/common/fifo.h>
#include    <nitro/pxi.h>


/*---------------------------------------------------------------------------*
    定数定義
 *---------------------------------------------------------------------------*/

// 曜日定義
typedef enum RTCWeek
{
    RTC_WEEK_SUNDAY = 0,               // 日曜日
    RTC_WEEK_MONDAY,                   // 月曜日
    RTC_WEEK_TUESDAY,                  // 火曜日
    RTC_WEEK_WEDNESDAY,                // 水曜日
    RTC_WEEK_THURSDAY,                 // 木曜日
    RTC_WEEK_FRIDAY,                   // 金曜日
    RTC_WEEK_SATURDAY,                 // 土曜日
    RTC_WEEK_MAX
}
RTCWeek;

// アラームチャンネル定義
typedef enum RTCAlarmChan
{
    RTC_ALARM_CHAN_1 = 0,              // 割込みチャンネル１
    RTC_ALARM_CHAN_2,                  // 割込みチャンネル２
    RTC_ALARM_CHAN_MAX
}
RTCAlarmChan;

// アラーム状態定義
typedef enum RTCAlarmStatus
{
    RTC_ALARM_STATUS_OFF = 0,          // 割込み禁止状態
    RTC_ALARM_STATUS_ON,               // 割込み許可状態
    RTC_ALARM_STATUS_MAX
}
RTCAlarmStatus;

// アラーム有効フラグ定義
#define     RTC_ALARM_ENABLE_NONE       0x0000  // 有効フラグなし
#define     RTC_ALARM_ENABLE_WEEK       0x0001  // 曜日設定を割込み判断に使用
#define     RTC_ALARM_ENABLE_HOUR       0x0002  // 時間設定を割込み判断に使用
#define     RTC_ALARM_ENABLE_MINUTE     0x0004  // 分設定を割込み判断に使用
#define     RTC_ALARM_ENABLE_ALL        ( RTC_ALARM_ENABLE_WEEK | RTC_ALARM_ENABLE_HOUR | RTC_ALARM_ENABLE_MINUTE )

// 処理結果定義
typedef enum RTCResult
{
    RTC_RESULT_SUCCESS = 0,
    RTC_RESULT_BUSY,
    RTC_RESULT_ILLEGAL_PARAMETER,
    RTC_RESULT_SEND_ERROR,
    RTC_RESULT_INVALID_COMMAND,
    RTC_RESULT_ILLEGAL_STATUS,
    RTC_RESULT_FATAL_ERROR,
    RTC_RESULT_MAX
}
RTCResult;


/*---------------------------------------------------------------------------*
    構造体定義
 *---------------------------------------------------------------------------*/
// コールバック関数型定義
typedef void (*RTCCallback) (RTCResult result, void *arg);
typedef void (*RTCInterrupt) (void);

// 日付構造体
typedef struct RTCDate
{
    u32     year;                      // 年 ( 0 ~ 99 )
    u32     month;                     // 月 ( 1 ~ 12 )
    u32     day;                       // 日 ( 1 ~ 31 )
    RTCWeek week;                      // 曜日

}
RTCDate;

// 時刻構造体
typedef struct RTCTime
{
    u32     hour;                      // 時 ( 0 ~ 23 )
    u32     minute;                    // 分 ( 0 ~ 59 )
    u32     second;                    // 秒 ( 0 ~ 59 )

}
RTCTime;

// ミリ秒フィールドが追加された時刻構造体
typedef struct RTCTimeEx
{
    s32     hour;                      // 時 ( 0 ~ 23 )
    s32     minute;                    // 分 ( 0 ~ 59 )
    s32     second;                    // 秒 ( 0 ~ 59 )
    s32     millisecond;               // ミリ秒 ( 0 ~ 999)
}
RTCTimeEx;

// アラームパラメータ構造体
typedef struct RTCAlarmParam
{
    RTCWeek week;                      // 曜日
    u32     hour;                      // 時 ( 0 ~ 23 )
    u32     minute;                    // 分 ( 0 ~ 59 )
    u32     enable;                    // アラーム有効フラグ( RTC_ALARM_ENABLE_* )

}
RTCAlarmParam;


/*---------------------------------------------------------------------------*
    関数定義
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         RTC_Init

  Description:  RTCライブラリを初期化する。
       Notice:  コンポーネント側の初期化によって電源投入チェックが行われる。

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    RTC_Init(void);

/*---------------------------------------------------------------------------*
  Name:         RTC_GetDate

  Description:  RTCから日付データを読み出す。

  Arguments:    date      - 日付データを格納するバッファを指定。

  Returns:      RTCResult - デバイス操作の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetDate(RTCDate *date);

/*---------------------------------------------------------------------------*
  Name:         RTC_GetTime

  Description:  RTCから時刻データを読み出す。

  Arguments:    time      - 時刻データを格納するバッファを指定。

  Returns:      RTCResult - デバイス操作の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetTime(RTCTime *time);

/*---------------------------------------------------------------------------*
  Name:         RTC_GetDateTime

  Description:  RTCから日付・時刻データを読み出す。

  Arguments:    date      - 日付データを格納するバッファを指定。
                time      - 時刻データを格納するバッファを指定。

  Returns:      RTCResult - デバイス操作の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetDateTime(RTCDate *date, RTCTime *time);

/*---------------------------------------------------------------------------*
  Name:         RTC_GetDateTimeExByTick

  Description:  CPU チックを利用してミリ秒単位までの日付・時刻データを読み出す。

  Arguments:    date      - 日付データを格納するバッファを指定。
                time      - 時刻データを格納するバッファを指定。

  Returns:      RTCResult - デバイス操作の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetDateTimeExByTick(RTCDate *date, RTCTimeEx *time);

/*---------------------------------------------------------------------------*
  Name:         RTC_SetDate

  Description:  RTCに日付データを書き込む。

  Arguments:    date      - 日付データが格納されているバッファを指定。

  Returns:      RTCResult - デバイス操作の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetDate(const RTCDate *date);

/*---------------------------------------------------------------------------*
  Name:         RTC_SetTime

  Description:  RTCに時刻データを書き込む。

  Arguments:    time      - 時刻データが格納されているバッファを指定。

  Returns:      RTCResult - デバイス操作の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetTime(const RTCTime *time);

/*---------------------------------------------------------------------------*
  Name:         RTC_SetDateTime

  Description:  RTCに日付・時刻データを書き込む。

  Arguments:    date      - 日付データが格納されているバッファを指定。
                time      - 時刻データが格納されているバッファを指定。

  Returns:      RTCResult - デバイス操作の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetDateTime(const RTCDate *date, const RTCTime *time);

/*---------------------------------------------------------------------------*
  Name:         RTC_GetAlarmStatus

  Description:  RTCからアラームのON/OFF状態を読み出す。

  Arguments:    chan      - アラームのチャンネルを指定。
                status    - アラーム状態を格納するバッファを指定。

  Returns:      RTCResult - デバイス操作の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetAlarmStatus(RTCAlarmChan chan, RTCAlarmStatus *status);

/*---------------------------------------------------------------------------*
  Name:         RTC_GetAlarmParam

  Description:  RTCからアラームの設定値を読み出す。

  Arguments:    chan      - アラームのチャンネルを指定。
                param     - アラーム設定値を格納するバッファを指定。

  Returns:      RTCResult - デバイス操作の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetAlarmParam(RTCAlarmChan chan, RTCAlarmParam *param);

/*---------------------------------------------------------------------------*
  Name:         RTC_SetAlarmStatus

  Description:  RTCにアラーム状態を書き込む。

  Arguments:    chan      - アラームのチャンネルを指定。
                status    - アラーム状態が格納されているバッファを指定。

  Returns:      RTCResult - デバイス操作の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetAlarmStatus(RTCAlarmChan chan, const RTCAlarmStatus *status);

/*---------------------------------------------------------------------------*
  Name:         RTC_SetAlarmParam

  Description:  RTCにアラーム設定値を書き込む。

  Arguments:    chan      - アラームのチャンネルを指定。
                param     - アラーム設定値が格納されているバッファを指定。

  Returns:      RTCResult - デバイス操作の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetAlarmParam(RTCAlarmChan chan, const RTCAlarmParam *param);

/*---------------------------------------------------------------------------*
  Name:         RTC_GetDateAsync

  Description:  RTCから日付データを非同期で読み出す。

  Arguments:    date      - 日付データを格納するバッファを指定。
                callback  - 非同期処理が完了した際に呼び出す関数を指定。
                arg       - コールバック関数呼び出し時の引数を指定。

  Returns:      RTCResult - 非同期デバイス操作開始の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetDateAsync(RTCDate *date, RTCCallback callback, void *arg);

/*---------------------------------------------------------------------------*
  Name:         RTC_GetTimeAsync

  Description:  RTCから時刻データを非同期で読み出す。

  Arguments:    time      - 時刻データを格納するバッファを指定。
                callback  - 非同期処理が完了した際に呼び出す関数を指定。
                arg       - コールバック関数呼び出し時の引数を指定。

  Returns:      RTCResult - 非同期デバイス操作開始の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetTimeAsync(RTCTime *time, RTCCallback callback, void *arg);

/*---------------------------------------------------------------------------*
  Name:         RTC_GetDateTimeAsync

  Description:  RTCから日付・時刻データを非同期で読み出す。

  Arguments:    date      - 日付データを格納するバッファを指定。
                time      - 時刻データを格納するバッファを指定。
                callback  - 非同期処理が完了した際に呼び出す関数を指定。
                arg       - コールバック関数呼び出し時の引数を指定。

  Returns:      RTCResult - 非同期デバイス操作開始の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetDateTimeAsync(RTCDate *date, RTCTime *time, RTCCallback callback, void *arg);

/*---------------------------------------------------------------------------*
  Name:         RTC_SetDateAsync

  Description:  RTCに日付データを非同期で書き込む。

  Arguments:    date      - 日付データが格納されているバッファを指定。
                callback  - 非同期処理が完了した際に呼び出す関数を指定。
                arg       - コールバック関数呼び出し時の引数を指定。

  Returns:      RTCResult - 非同期デバイス操作開始の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetDateAsync(const RTCDate *date, RTCCallback callback, void *arg);

/*---------------------------------------------------------------------------*
  Name:         RTC_SetTimeAsync

  Description:  RTCに時刻データを非同期で書き込む。

  Arguments:    time      - 時刻データが格納されているバッファを指定。
                callback  - 非同期処理が完了した際に呼び出す関数を指定。
                arg       - コールバック関数呼び出し時の引数を指定。

  Returns:      RTCResult - 非同期デバイス操作開始の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetTimeAsync(const RTCTime *time, RTCCallback callback, void *arg);

/*---------------------------------------------------------------------------*
  Name:         RTC_SetDateTimeAsync

  Description:  RTCに日付・時刻データを非同期で書き込む。

  Arguments:    date      - 日付データが格納されているバッファを指定。
                time      - 時刻データが格納されているバッファを指定。
                callback  - 非同期処理が完了した際に呼び出す関数を指定。
                arg       - コールバック関数呼び出し時の引数を指定。

  Returns:      RTCResult - 非同期デバイス操作開始の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetDateTimeAsync(const RTCDate *date, const RTCTime *time, RTCCallback callback,
                               void *arg);

/*---------------------------------------------------------------------------*
  Name:         RTC_GetAlarmStatusAsync

  Description:  RTCからアラームのON/OFF状態を非同期で読み出す。

  Arguments:    chan      - アラームのチャンネルを指定。
                status    - アラーム状態を格納するバッファを指定。
                callback  - 非同期処理が完了した際に呼び出す関数を指定。
                arg       - コールバック関数呼び出し時の引数を指定。

  Returns:      RTCResult - 非同期デバイス操作開始の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetAlarmStatusAsync(RTCAlarmChan chan, RTCAlarmStatus *status, RTCCallback callback,
                                  void *arg);

/*---------------------------------------------------------------------------*
  Name:         RTC_GetAlarmParamAsync

  Description:  RTCからアラームの設定値を非同期で読み出す。

  Arguments:    chan      - アラームのチャンネルを指定。
                param     - アラーム設定値を格納するバッファを指定。
                callback  - 非同期処理が完了した際に呼び出す関数を指定。
                arg       - コールバック関数呼び出し時の引数を指定。

  Returns:      RTCResult - 非同期デバイス操作開始の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetAlarmParamAsync(RTCAlarmChan chan, RTCAlarmParam *param, RTCCallback callback,
                                 void *arg);

/*---------------------------------------------------------------------------*
  Name:         RTC_SetAlarmInterrupt

  Description:  アラーム割込み発生時のコールバック関数を設定する。

  Arguments:    interrupt - コールバック関数を指定。

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    RTC_SetAlarmInterrupt(RTCInterrupt interrupt);

/*---------------------------------------------------------------------------*
  Name:         RTC_SetAlarmStatusAsync

  Description:  RTCにアラーム状態を非同期で書き込む。

  Arguments:    chan      - アラームのチャンネルを指定。
                status    - アラーム状態が格納されているバッファを指定。
                callback  - 非同期処理が完了した際に呼び出す関数を指定。
                arg       - コールバック関数呼び出し時の引数を指定。

  Returns:      RTCResult - 非同期デバイス操作開始の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetAlarmStatusAsync(RTCAlarmChan chan, const RTCAlarmStatus *status,
                                  RTCCallback callback, void *arg);

/*---------------------------------------------------------------------------*
  Name:         RTC_SetAlarmParamAsync

  Description:  RTCにアラーム設定を非同期で書き込む。
       Notice:  RTCのアラーム状態がONになっていない場合、デバイス側でライトを
                受け付けてくれないので、書き込みは失敗する。

  Arguments:    chan      - アラームのチャンネルを指定。
                param     - アラーム設定値が格納されているバッファを指定。
                callback  - 非同期処理が完了した際に呼び出す関数を指定。
                arg       - コールバック関数呼び出し時の引数を指定。

  Returns:      RTCResult - 非同期デバイス操作開始の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetAlarmParamAsync(RTCAlarmChan chan, const RTCAlarmParam *param,
                                 RTCCallback callback, void *arg);


/*---------------------------------------------------------------------------*
    非公開関数定義
 *---------------------------------------------------------------------------*/
BOOL    RTCi_ResetAsync(void);
BOOL    RTCi_SetHourFormatAsync(void);
BOOL    RTCi_ReadRawDateTimeAsync(void);
BOOL    RTCi_WriteRawDateTimeAsync(void);
BOOL    RTCi_ReadRawDateAsync(void);
BOOL    RTCi_WriteRawDateAsync(void);
BOOL    RTCi_ReadRawTimeAsync(void);
BOOL    RTCi_WriteRawTimeAsync(void);
BOOL    RTCi_ReadRawPulseAsync(void);
BOOL    RTCi_WriteRawPulseAsync(void);
BOOL    RTCi_ReadRawAlarm1Async(void);
BOOL    RTCi_WriteRawAlarm1Async(void);
BOOL    RTCi_ReadRawAlarm2Async(void);
BOOL    RTCi_WriteRawAlarm2Async(void);
BOOL    RTCi_ReadRawStatus1Async(void);
BOOL    RTCi_WriteRawStatus1Async(void);
BOOL    RTCi_ReadRawStatus2Async(void);
BOOL    RTCi_WriteRawStatus2Async(void);
BOOL    RTCi_ReadRawAdjustAsync(void);
BOOL    RTCi_WriteRawAdjustAsync(void);
BOOL    RTCi_ReadRawFreeAsync(void);
BOOL    RTCi_WriteRawFreeAsync(void);
RTCResult RTCi_SetRegStatus2Async(const RTCRawStatus2 *status2, RTCCallback callback, void *arg);
RTCResult RTCi_SetRegAdjustAsync(const RTCRawAdjust *adjust, RTCCallback callback, void *arg);
RTCResult RTCi_SetRegAdjust(const RTCRawAdjust *Adjust);
RTCResult RTCi_SetRegStatus2(const RTCRawStatus2 *status2);


/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C" */
#endif

#endif /* NITRO_RTC_ARM9_API_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
