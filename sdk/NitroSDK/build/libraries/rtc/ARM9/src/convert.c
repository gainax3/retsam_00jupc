/*---------------------------------------------------------------------------*
  Project:  NitroSDK - RTC - libraries
  File:     convert.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: convert.c,v $
  Revision 1.6  2006/01/18 02:11:20  kitase_hirotake
  do-indent

  Revision 1.5  2005/10/04 00:29:01  yada
  suppressed warning

  Revision 1.4  2005/09/30 00:26:08  yada
  add  RTC_GetDayOfWeek()

  Revision 1.3  2005/05/18 02:01:37  seiki_masashi
  RTC_ConvertDateToDay, DayToDate を公開関数に変更

  Revision 1.2  2005/05/18 00:40:07  seiki_masashi
  警告に対応

  Revision 1.1  2005/05/17 12:53:54  seiki_masashi
  変換関数の追加

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include    <nitro/types.h>
#include    <nitro/rtc.h>


/*---------------------------------------------------------------------------*
    静的変数定義
 *---------------------------------------------------------------------------*/
static s32 sDayOfYear[12] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };


/*---------------------------------------------------------------------------*
    内部関数宣言
 *---------------------------------------------------------------------------*/

// 簡易うるう年の判定 (うるう年なら TRUE）
// ※ RTC のとりうる範囲の 2000 ～ 2099 年に限定する。
static inline BOOL RTCi_IsLeapYear(u32 year)
{
    // うるう年は、「4で割り切れ かつ 100で割り切れない年」または「400で割り切れる年」
    return !((year & 0x03));
}

/*---------------------------------------------------------------------------*
    関数定義
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         RTC_ConvertDateToDay

  Description:  RTCDate 型の日付データを2000年からの通算日数に変換する

  Arguments:    date      - 日付データへのポインタ.

  Returns:      2000年1月1日からの通算日数.
 *---------------------------------------------------------------------------*/
s32 RTC_ConvertDateToDay(const RTCDate *date)
{
    s32     dayNum;

    if (date->year >= 100
        || (date->month < 1) || (date->month > 12)
        || (date->day < 1) || (date->day > 31)
        || (date->week >= RTC_WEEK_MAX) || (date->month < 1) || (date->month > 12))
    {
        return -1;
    }

    // 月、日を日数に換算
    dayNum = (s32)(date->day - 1);
    dayNum += sDayOfYear[date->month - 1];
    if ((date->month >= 3) && RTCi_IsLeapYear(date->year))
    {
        // 閏年では、3月以降を1日増やす
        dayNum++;
    }

    // 年を日数に換算
    dayNum += date->year * 365;
    // 指定年-1までのうるう年の個数を算出して、その日数を加算。
    // date->year は 0 ～ 99 までなので、閏年の計算は簡易版
    dayNum += (date->year + 3) / 4;

    return dayNum;
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_ConvertTimeToSecond

  Description:  RTCTime 型の時刻データを0時からの通算秒数に変換する

  Arguments:    time      - 時刻データへのポインタ.

  Returns:      0時0分からの通算秒数.
 *---------------------------------------------------------------------------*/
s32 RTCi_ConvertTimeToSecond(const RTCTime *time)
{
    return (s32)((time->hour * 60 + time->minute) * 60 + time->second);
}

/*---------------------------------------------------------------------------*
  Name:         RTC_ConvertDateTimeToSecond

  Description:  RTCDate 型と RTCTime 型の日時データを通算秒数に変換する

  Arguments:    date      - 日付データへのポインタ.
                time      - 時刻データへのポインタ.

  Returns:      ローカル時間における2000年1月1日0時0分からの通算秒数.
 *---------------------------------------------------------------------------*/
s64 RTC_ConvertDateTimeToSecond(const RTCDate *date, const RTCTime *time)
{
    s32     day, sec;
    day = RTC_ConvertDateToDay(date);
    if (day == -1)
    {
        return -1;
    }
    sec = RTCi_ConvertTimeToSecond(time);
    if (sec == -1)
    {
        return -1;
    }
    return ((s64)day) * (60 * 60 * 24) + sec;
}

/*---------------------------------------------------------------------------*
  Name:         RTC_ConvertDayToDate

  Description:  2000年からの通算日数を RTCDate 型の日付データに変換する

  Arguments:    date      - 保存先の日付データのポインタ.
                day       - 2000年1月1日からの通算日数.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void RTC_ConvertDayToDate(RTCDate *date, s32 day)
{
    u32     year;
    s32     month;

    if (day < 0)
    {
        day = 0;
    }
    if (day > 36524)
    {
        day = 36524;
    }

    // 2000年1月1日は土曜日
    date->week = (RTCWeek)((day + 6) % 7);

    for (year = 0; year < 99; year++)
    {
        s32     prev = day;
        day -= (RTCi_IsLeapYear(year)) ? 366 : 365;
        if (day < 0)
        {
            day = prev;
            break;
        }
    }
    if (day > 365)
    {
        day = 365;
    }

    date->year = year;

    if (RTCi_IsLeapYear(year))
    {
        if (day < 31 + 29)
        {
            if (day < 31)
            {
                month = 1;
            }
            else
            {
                month = 2;
                day -= 31;
            }
            date->month = (u32)month;
            date->day = (u32)(day + 1);
            return;
        }
        else
        {
            day--;
        }
    }

    for (month = 11; month >= 0; month--)
    {
        if (day >= sDayOfYear[month])
        {
            date->month = (u32)(month + 1);
            date->day = (u32)(day - sDayOfYear[month] + 1);
            return;
        }
    }

    SDK_ASSERT("Internal Error.");
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_ConvertSecondToTime

  Description:  0時からの通算秒数を RTCTime 型の時刻データに変換する

  Arguments:    time      - 保存先の日付データのポインタ.
                sec       - 0時0分からの通算秒数.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void RTCi_ConvertSecondToTime(RTCTime *time, s32 sec)
{
    if (sec < 0)
    {
        sec = 0;
    }
    if (sec > 86399)
    {
        sec = 86399;
    }

    time->second = (u32)(sec % 60);
    sec /= 60;
    time->minute = (u32)(sec % 60);
    sec /= 60;
    time->hour = (u32)sec;
}

/*---------------------------------------------------------------------------*
  Name:         RTC_ConvertDateTimeToSecond

  Description:  通算秒数を RTCDate 型と RTCTime 型の日時データを変換する

  Arguments:    date      - 保存先の日付データへのポインタ.
                time      - 保存先の時刻データへのポインタ.
                sec       - 2000年1月1日0時0分からの通算秒数.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void RTC_ConvertSecondToDateTime(RTCDate *date, RTCTime *time, s64 sec)
{
    if (sec < 0)
    {
        sec = 0;
    }
    else if (sec > 3155759999)
    {
        sec = 3155759999;
    }
    RTCi_ConvertSecondToTime(time, (s32)(sec % 86400));
    RTC_ConvertDayToDate(date, (s32)(sec / 86400));
}

/*---------------------------------------------------------------------------*
  Name:         RTC_GetDayOfWeek

  Description:  RTCDate 型データから、曜日を計算して返す。
                (RTCDate の中の曜日を使用せずに計算によって求める)

  Arguments:    date      - 日付データへのポインタ.

  Returns:      曜日      RTC_WEEK_xxxx
 *---------------------------------------------------------------------------*/
RTCWeek RTC_GetDayOfWeek(RTCDate *date)
{
    int     cent;
    int     year = (int)(2000 + date->year);
    int     month = (int)date->month;
    int     day = (int)date->day;

    month -= 2;
    if (month < 1)
    {
        month += 12;
        --year;
    }

    cent = year / 100;
    year %= 100;
    return (RTCWeek)(((26 * month - 2) / 10 + day + year + year / 4 + cent / 4 + 5 * cent) % 7);
    // RTC_WEEK_xxxx は日、月、…土が0～6で定義されていることを利用
}

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
