/*---------------------------------------------------------------------------*
  Project:  NitroSDK - SPI - include
  File:     userInfo_ts_300.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: userInfo_ts_300.h,v $
  Revision 1.3  2006/10/17 05:28:26  okubata_ryoma
  韓国版IPLに対応

  Revision 1.2  2006/01/18 02:12:38  kitase_hirotake
  do-indent

  Revision 1.1  2005/04/12 10:49:57  terui
  中国語対応の為に新規追加。

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NITRO_SPI_COMMON_USERINFO_TS_300_H_
#define NITRO_SPI_COMMON_USERINFO_TS_300_H_

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================*/

/*---------------------------------------------------------------------------*
    定数定義
 *---------------------------------------------------------------------------*/
// コンフィグ設定データフォーマットバージョン
#define NVRAM_CONFIG_DATA_VERSION           5
#define NVRAM_CONFIG_DATA_EX_VERSION        1

// コンフィグ内の各種設定値
#define NVRAM_CONFIG_NICKNAME_LENGTH        10  // ニックネーム長
#define NVRAM_CONFIG_COMMENT_LENGTH         26  // コメント長
#define NVRAM_CONFIG_FAVORITE_COLOR_MAX     16  // 好きな色の最大数

// IPL2 タイプ
#define NVRAM_CONFIG_IPL2_TYPE_ADDRESS      0x001d
#define NVRAM_CONFIG_IPL2_TYPE_SIZE         1
#define NVRAM_CONFIG_IPL2_TYPE_NORMAL       0xff
#define NVRAM_CONFIG_IPL2_TYPE_EX_MASK      0x50

// 言語設定コード
#define NVRAM_CONFIG_LANG_JAPANESE          0   // 日本語
#define NVRAM_CONFIG_LANG_ENGLISH           1   // 英語
#define NVRAM_CONFIG_LANG_FRENCH            2   // フランス語
#define NVRAM_CONFIG_LANG_GERMAN            3   // ドイツ語
#define NVRAM_CONFIG_LANG_ITALIAN           4   // イタリア語
#define NVRAM_CONFIG_LANG_SPANISH           5   // スペイン語
#define NVRAM_CONFIG_LANG_CHINESE           6   // 中国語
#define NVRAM_CONFIG_LANG_HANGUL            7   // 韓国語
#define NVRAM_CONFIG_LANG_CODE_MAX          8   // 言語設定コードの種類数

// 通常版での対応言語ビットマップ
#define NVRAM_CONFIG_LANG_BITMAP_NORMAL     ( (0x0001 << NVRAM_CONFIG_LANG_JAPANESE ) \
                                            | (0x0001 << NVRAM_CONFIG_LANG_ENGLISH) \
                                            | (0x0001 << NVRAM_CONFIG_LANG_FRENCH) \
                                            | (0x0001 << NVRAM_CONFIG_LANG_GERMAN) \
                                            | (0x0001 << NVRAM_CONFIG_LANG_ITALIAN) \
                                            | (0x0001 << NVRAM_CONFIG_LANG_SPANISH) )

// 中国版での対応言語ビットマップ
#define NVRAM_CONFIG_LANG_BITMAP_CHINESE    ( (0x0001 << NVRAM_CONFIG_LANG_ENGLISH ) \
                                            | (0x0001 << NVRAM_CONFIG_LANG_FRENCH) \
                                            | (0x0001 << NVRAM_CONFIG_LANG_GERMAN) \
                                            | (0x0001 << NVRAM_CONFIG_LANG_ITALIAN) \
                                            | (0x0001 << NVRAM_CONFIG_LANG_SPANISH) \
                                            | (0x0001 << NVRAM_CONFIG_LANG_CHINESE) )


// 韓国版での対応言語ビットマップ
#define NVRAM_CONFIG_LANG_BITMAP_HANGUL     ( (0x0001 << NVRAM_CONFIG_LANG_JAPANESE ) \
                                            | (0x0001 << NVRAM_CONFIG_LANG_ENGLISH) \
                                            | (0x0001 << NVRAM_CONFIG_LANG_FRENCH) \
                                            | (0x0001 << NVRAM_CONFIG_LANG_GERMAN) \
                                            | (0x0001 << NVRAM_CONFIG_LANG_SPANISH) \
                                            | (0x0001 << NVRAM_CONFIG_LANG_HANGUL) )

/*---------------------------------------------------------------------------*
    構造体定義
 *---------------------------------------------------------------------------*/
// 誕生日データ(2byte)
typedef struct NVRAMConfigDate
{
    u8      month;                     // 月:01～12
    u8      day;                       // 日:01～31

}
NVRAMConfigDate;

// ニックネーム(22byte)
typedef struct NVRAMConfigNickname
{
    u16     str[NVRAM_CONFIG_NICKNAME_LENGTH];  // ニックネーム（Unicode(UTF16)で最大10文字、終端コードなし）
    u8      length;                    // 文字数
    u8      rsv;

}
NVRAMConfigNickname;

// コメント(54byte)
typedef struct NVRAMConfigComment
{
    u16     str[NVRAM_CONFIG_COMMENT_LENGTH];   //コメント（Unicode(UTF16)で最大26文字、終端コードなし）
    u8      length;                    // 文字数
    u8      rsv;

}
NVRAMConfigComment;

// オーナー情報(80byte)
typedef struct NVRAMConfigOwnerInfo
{
    u8      favoriteColor:4;           // 好きな色
    u8      rsv:4;                     // 予約
    NVRAMConfigDate birthday;          // 誕生日
    u8      pad;
    NVRAMConfigNickname nickname;      // ニックネーム
    NVRAMConfigComment comment;        // コメント

}
NVRAMConfigOwnerInfo;


// IPL用目覚まし時計データ(6byte)
typedef struct NVRAMConfigAlarm
{
    u8      hour;                      // アラーム時:00～23
    u8      minute;                    // アラーム分:00～59
    u8      second;                    // アラーム秒:00～59
    u8      pad;
    u16     enableWeek:7;              // どの曜日にアラームが有効かのフラグ（bit0:日 bit1:月..."1"が有効）
    u16     alarmOn:1;                 // アラームON,OFF（0:OFF, 1:ON）
    u16     rsv:8;                     // 予約

}
NVRAMConfigAlarm;

// タッチパネル座標補正データ(12byte)
typedef struct NVRAMConfigTpCalibData
{
    u16     raw_x1;                    // 第１キャリブレーション点のTP取得値X
    u16     raw_y1;                    // 第１キャリブレーション点のTP取得値Y
    u8      dx1;                       // 第１キャリブレーション点のLCD座標 X
    u8      dy1;                       // 第１キャリブレーション点のLCD座標 Y
    u16     raw_x2;                    // 第２キャリブレーション点のTP取得値X
    u16     raw_y2;                    // 第２キャリブレーション点のTP取得値Y
    u8      dx2;                       // 第２キャリブレーション点のLCD座標 X
    u8      dy2;                       // 第２キャリブレーション点のLCD座標 Y

}
NVRAMConfigTpCalibData;

// オプション情報(12byte)
typedef struct NVRAMConfigOption
{
    u16     language:3;                // 言語コード
    u16     agbLcd:1;                  // AGBモードで起動する時にどちらのLCDで起動するか？（0:TOP,1:BOTTOM）
    u16     detectPullOutCardFlag:1;   // カードが引き抜かれたことを示すフラグ。
    u16     detectPullOutCtrdgFlag:1;  // カートリッジが引き抜かれたことを示すフラグ。
    u16     autoBootFlag:1;            // 起動シーケンスで、メニュー停止なしで自動起動するかどうか？
    u16     rsv:4;                     // 予約。
    u16     input_favoriteColor:1;     // 好きな色が入力されたか？
    u16     input_tp:1;                // タッチパネルがキャリブレーションされたか？（  〃  )
    u16     input_language:1;          // 言語入力がされたか？　       (0:未設定, 1:設定済み)
    u16     input_rtc:1;               // RTC設定がされたか？          (       〃　　　　　 )
    u16     input_nickname:1;          // ニックネームが入力されたか？ (       〃　　　　　 )
    u8      timezone;                  // タイムゾーン（現在はリザーブ）
    u8      rtcClockAdjust;            // RTCクロック調整値
    s64     rtcOffset;                 // RTC設定時のオフセット値（ユーザーがRTC設定を変更する度にその値に応じて増減します。）

}
NVRAMConfigOption;


// コンフィグ各種設定データ(112byte)
typedef struct NVRAMConfigData
{
    u8      version;                   // フラッシュ格納データフォーマットのバージョン
    u8      pad;
    NVRAMConfigOwnerInfo owner;        // オーナー情報
    NVRAMConfigAlarm alarm;            // IPL用目覚まし時計データ
    NVRAMConfigTpCalibData tp;         // タッチパネルキャリブレーションデータ
    NVRAMConfigOption option;          // オプション

}
NVRAMConfigData;

// コンフィグ各種設定データのNVRAM保存時フォーマット(116byte)
typedef struct NVRAMConfig
{
    NVRAMConfigData ncd;               // コンフィグ各種設定データ
    u16     saveCount;                 // 0x00-0x7f をループしてカウントし、カウント値が新しいデータが有効。
    u16     crc16;                     // コンフィグ各種設定データの 16bit CRC

}
NVRAMConfig;

// コンフィグ拡張各種設定データ(138byte)
typedef struct NVRAMConfigDataEx
{
    u8      version;                   // 拡張コンフィグ情報の型バージョン
    u8      language;                  // 拡張言語コード( NVRAM_CONFIG_LANG_CHINESE 以降に拡張された値が格納される )
    u16     valid_language_bitmap;     // 有効な言語コードを示したビットマップ
    u8      padding[256 - sizeof(NVRAMConfigData) - 4 - 4 - 2];
    // NVRAMConfigEx のサイズが NVRAM 1ページ(256byte)になるようにパディング
}
NVRAMConfigDataEx;

// 拡張されたコンフィグ各種設定データのNVRAM保存時フォーマット(256byte)
typedef struct NVRAMConfigEx
{
    NVRAMConfigData ncd;               // コンフィグ各種設定データ
    u16     saveCount;                 // 0x00-0x7f をループしてカウントし、カウント値が新しいデータが有効。
    u16     crc16;                     // コンフィグ各種設定データの 16bit CRC
    NVRAMConfigDataEx ncd_ex;          // 拡張コンフィグ各種設定データ
    u16     crc16_ex;                  // 拡張コンフィグ各種設定データの 16bit CRC

}
NVRAMConfigEx;


/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C" */
#endif

#endif /* NITRO_SPI_COMMON_USERINFO_TS_300_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
