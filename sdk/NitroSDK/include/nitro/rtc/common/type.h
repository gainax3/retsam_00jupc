/*---------------------------------------------------------------------------*
  Project:  NitroSDK - RTC - include
  File:     type.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: type.h,v $
  Revision 1.3  2006/01/18 02:12:38  kitase_hirotake
  do-indent

  Revision 1.2  2005/02/28 05:26:09  yosizaki
  do-indent.

  Revision 1.1  2004/05/12 02:27:35  terui
  initial upload

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef	NITRO_RTC_COMMON_TYPE_H_
#define	NITRO_RTC_COMMON_TYPE_H_

#ifdef	__cplusplus
extern "C" {
#endif

/*===========================================================================*/

#include	<nitro/types.h>

/*---------------------------------------------------------------------------*
    定数定義
 *---------------------------------------------------------------------------*/
// RTC割込み関連定義
#define		RTC_INTERRUPT_MODE_NONE				0x0     // 割込みなし
#define		RTC_INTERRUPT_MODE_32kHz			0x8     // 32kHz定常波出力
#define		RTC_INTERRUPT_MODE_PULSE			0x1     // 選択周波数定常割込み
#define		RTC_INTERRUPT_MODE_MINUTE_EDGE		0x2     // 分単位エッジ割込み
#define		RTC_INTERRUPT_MODE_MINUTE_PULSE1	0x3     // 分単位定常割込み１(デューティー 50%)
#define		RTC_INTERRUPT_MODE_ALARM			0x4     // アラーム割込み
#define		RTC_INTERRUPT_MODE_MINUTE_PULSE2	0x7     // 分単位定常割込み２

#define		RTC_INTERRUPT_MASK_32kHz			0x8     // 32kHz定常波出力時の意味を持つビットマスク
#define		RTC_INTERRUPT_MASK_PULSE			0xb     // 選択周波数定常割込み時の意味を持つビットマスク
#define		RTC_INTERRUPT_MASK_MINUTE_EDGE		0xb     // 分単位エッジ割込み時の意味を持つビットマスク

// 周波数デューティー定義
#define		RTC_PULSE_DUTY_1HZ					0x01
#define		RTC_PULSE_DUTY_2HZ					0x02
#define		RTC_PULSE_DUTY_4HZ					0x04
#define		RTC_PULSE_DUTY_8HZ					0x08
#define		RTC_PULSE_DUTY_16HZ					0x10
/* 複数を設定した場合、設定したパルスが全てハイになったとき割込み信号線に
   ハイが出力される。どれかひとつでもローだった場合はロー出力となる。*/


/*---------------------------------------------------------------------------*
    構造体定義
 *---------------------------------------------------------------------------*/
// 日付構造体
typedef struct RTCRawDate
{
    u32     year:8;                    // 年( 00 ~ 99 )
    u32     month:5;                   // 月( 01 ~ 12 )
    u32     dummy0:3;
    u32     day:6;                     // 日( 01 ~ 31 ) 月/閏年により上限は変動
    u32     dummy1:2;
    u32     week:3;                    // 曜日( 00 ~ 06 )
    u32     dummy2:5;

}
RTCRawDate;

// 時間構造体
typedef struct RTCRawTime
{
    u32     hour:6;                    // 時間( 00 ~ 23 または 00 ~ 11 )
    u32     afternoon:1;               // 12時間表記の場合のp.m.フラグ
    u32     dummy0:1;
    u32     minute:7;                  // 分( 00 ~ 59 )
    u32     dummy1:1;
    u32     second:7;                  // 秒( 00 ~ 59 )
    u32     dummy2:9;

}
RTCRawTime;

// ステータス１構造体
typedef struct RTCRawStatus1
{
    u16     reset:1;                   // W   リセットフラグ
    u16     format:1;                  // R/W の時間表記フラグ( 0: 12時間表記、1: 24時間表記 )
    u16     dummy0:2;
    u16     intr1:1;                   // R   割込み１発生フラグ( 割込み信号線にはローが出力される )
    u16     intr2:1;                   // R   割込み２発生フラグ( 割込み信号線にはローが出力される )
    u16     bld:1;                     // R   電源電圧降下検出フラグ
    u16     poc:1;                     // R   電源投入検出フラグ
    u16     dummy1:8;

}
RTCRawStatus1;

// ステータス２構造体
typedef struct RTCRawStatus2
{
    u16     intr_mode:4;               // R/W 割込み１選択( RTC_INTERRUPT_MODE_* )
    u16     dummy0:2;
    u16     intr2_mode:1;              // R/W 割込み２許可フラグ
    u16     test:1;                    // R/W テストモードフラグ
    u16     dummy1:8;

}
RTCRawStatus2;

// アラーム構造体
typedef struct RTCRawAlarm
{
    u32     week:3;                    // 曜日( 00 ~ 06 )
    u32     dummy0:4;
    u32     we:1;                      // 曜日設定有効フラグ
    u32     hour:6;                    // 時間( 00 ~ 23 または 00 ~ 11 ) 時間表記はステータス１に依存
    u32     afternoon:1;               // 12時間表記の場合のp.m.フラグ
    u32     he:1;                      // 時間設定有効フラグ
    u32     minute:7;                  // 分( 00 ~ 59 )
    u32     me:1;                      // 分設定有効フラグ
    u32     dummy2:8;

}
RTCRawAlarm;

// パルス構造体
typedef struct RTCRawPulse
{
    u32     pulse:5;                   // 周波数デューティーフラグ( RTC_PULSE_DUTY_* )
    u32     dummy:27;

}
RTCRawPulse;

// クロック調整レジスタ構造体
typedef struct RTCRawAdjust
{
    u32     adjust:8;                  // クロック調整用レジスタ( 設定については別資料参照 )
    u32     dummy:24;

}
RTCRawAdjust;

// フリーレジスタ構造体
typedef struct RTCRawFree
{
    u32     free:8;                    // フリーレジスタ
    u32     dummy:24;

}
RTCRawFree;

// ARM9 <-> ARM7 受け渡しデータ構造体( 8バイト共用体 )
typedef union RTCRawData
{
    struct
    {
        RTCRawDate date;               // 日付
        RTCRawTime time;               // 時刻
    }
    t;

    struct
    {
        RTCRawStatus1 status1;         // ステータス１レジスタ
        RTCRawStatus2 status2;         // ステータス２レジスタ
        union
        {
            RTCRawPulse pulse;         // 周波数定常割込みレジスタ設定
            RTCRawAlarm alarm;         // アラーム( 1 or 2 )レジスタ設定
            RTCRawAdjust adjust;       // クロック調整用レジスタ設定
            RTCRawFree free;           // フリーレジスタ設定
        };
    }
    a;

    u32     words[2];                  // 4バイトアクセス用

    u16     halfs[4];                  // 2バイトアクセス用

    u8      bytes[8];                  // バイトアクセス用

}
RTCRawData;


/*===========================================================================*/

#ifdef	__cplusplus
}          /* extern "C" */
#endif

#endif /* NITRO_RTC_COMMON_TYPE_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
