/*---------------------------------------------------------------------------*
  Project:  NitroSDK - RTC - include
  File:     convert.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: convert.h,v $
  Revision 1.4  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.3  2005/09/30 00:25:46  yada
  add  RTC_GetDayOfWeek()

  Revision 1.2  2005/05/18 02:53:17  seiki_masashi
  RTC_ConvertDateToDay, DayToDate を公開

  Revision 1.1  2005/05/17 13:01:45  seiki_masashi
  変換関数の追加

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_RTC_ARM9_CONVERT_H_
#define NITRO_RTC_ARM9_CONVERT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================*/

#include <nitro/types.h>

/*---------------------------------------------------------------------------*
    定数定義
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
    構造体定義
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
    関数宣言
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         RTC_ConvertDateToDay

  Description:  RTCDate 型の日付データを2000年からの通算日数に変換する

  Arguments:    date      - 日付データへのポインタ.

  Returns:      2000年1月1日からの通算日数.
 *---------------------------------------------------------------------------*/
s32     RTC_ConvertDateToDay(const RTCDate *date);

/*---------------------------------------------------------------------------*
  Name:         RTCi_ConvertTimeToSecond

  Description:  RTCTime 型の時刻データを0時からの通算秒数に変換する

  Arguments:    time      - 時刻データへのポインタ.

  Returns:      0時0分からの通算秒数.
 *---------------------------------------------------------------------------*/
s32     RTCi_ConvertTimeToSecond(const RTCTime *time);

/*---------------------------------------------------------------------------*
  Name:         RTC_ConvertDateTimeToSecond

  Description:  RTCDate 型と RTCTime 型の日時データを通算秒数に変換する

  Arguments:    date      - 日付データへのポインタ.
                time      - 時刻データへのポインタ.

  Returns:      ローカル時間における2000年1月1日0時0分からの通算秒数.
 *---------------------------------------------------------------------------*/
s64     RTC_ConvertDateTimeToSecond(const RTCDate *date, const RTCTime *time);

/*---------------------------------------------------------------------------*
  Name:         RTC_ConvertDayToDate

  Description:  2000年からの通算日数を RTCDate 型の日付データに変換する

  Arguments:    date      - 保存先の日付データのポインタ.
                day       - 2000年1月1日からの通算日数.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    RTC_ConvertDayToDate(RTCDate *date, s32 day);

/*---------------------------------------------------------------------------*
  Name:         RTCi_ConvertSecondToTime

  Description:  0時からの通算秒数を RTCTime 型の時刻データに変換する

  Arguments:    time      - 保存先の日付データのポインタ.
                sec       - 0時0分からの通算秒数.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    RTCi_ConvertSecondToTime(RTCTime *time, s32 sec);

/*---------------------------------------------------------------------------*
  Name:         RTC_ConvertDateTimeToSecond

  Description:  通算秒数を RTCDate 型と RTCTime 型の日時データを変換する

  Arguments:    date      - 保存先の日付データへのポインタ.
                time      - 保存先の時刻データへのポインタ.
                sec       - 2000年1月1日0時0分からの通算秒数.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    RTC_ConvertSecondToDateTime(RTCDate *date, RTCTime *time, s64 sec);

/*---------------------------------------------------------------------------*
  Name:         RTC_GetDayOfWeek

  Description:  RTCDate 型データから、曜日を計算して返す。
                (RTCDate の中の曜日を使用せずに計算によって求める)

  Arguments:    date      - 日付データへのポインタ.

  Returns:      曜日      RTC_WEEK_xxxx
 *---------------------------------------------------------------------------*/
RTCWeek RTC_GetDayOfWeek(RTCDate *date);

/*===========================================================================*/

#ifdef __cplusplus
}          /* extern "C" */
#endif

#endif /* NITRO_RTC_ARM9_CONVERT_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
