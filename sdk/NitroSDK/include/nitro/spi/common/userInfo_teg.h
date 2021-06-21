/*---------------------------------------------------------------------------*
  Project:  NitroSDK - SPI - include
  File:     userInfo_teg.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: userInfo_teg.h,v $
  Revision 1.5  2006/01/18 02:12:38  kitase_hirotake
  do-indent

  Revision 1.4  2005/02/28 05:26:03  yosizaki
  do-indent.

  Revision 1.3  2004/09/03 13:03:11  takano_makoto
  スペルミス修正 NitroConfigData -> NVRAMConfigData.

  Revision 1.2  2004/09/03 04:29:15  terui
  NVRAM内ユーザー情報に関する構造体定義整理に伴う修正。

  Revision 1.1  2004/08/25 09:20:43  terui
  Initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NITRO_SPI_COMMON_USERINFO_TEG_H_
#define NITRO_SPI_COMMON_USERINFO_TEG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================*/

/*---------------------------------------------------------------------------*
    定数定義
 *---------------------------------------------------------------------------*/
// コンフィグ設定データフォーマットバージョン
#define NVRAM_CONFIG_DATA_VERSION           0

// コンフィグ内の各種設定値
#define NVRAM_CONFIG_BOOT_GAME_LOG_NUM      8   // ブートゲーム履歴の保存個数
#define NVRAM_CONFIG_NICKNAME_LENGTH        8   // ニックネーム長

// 言語設定コード
#define NVRAM_CONFIG_LANG_JAPANESE          0   // 日本語
#define NVRAM_CONFIG_LANG_ENGLISH           1   // 英語
#define NVRAM_CONFIG_LANG_FRENCH            2   // フランス語
#define NVRAM_CONFIG_LANG_GERMAN            3   // ドイツ語
#define NVRAM_CONFIG_LANG_ITALIAN           4   // イタリア語
#define NVRAM_CONFIG_LANG_SPANISH           5   // スペイン語
#define NVRAM_CONFIG_LANG_CODE_MAX          6   // 言語設定コードの種類数

/*---------------------------------------------------------------------------*
    構造体定義
 *---------------------------------------------------------------------------*/
// ニックネーム(20byte)
typedef struct NVRAMConfigNickname
{
    u16     name[NVRAM_CONFIG_NICKNAME_LENGTH]; // ニックネーム（UNICODEで最大8文字、終端コードなし）
    u16     length;                    // 文字数
    u16     padding;

}
NVRAMConfigNickname;

// 性別コード(4byte)
typedef enum NVRAMConfigSexCode
{
    NVRAM_CONFIG_SEX_MALE = 0,         // 男
    NVRAM_CONFIG_SEX_FEMALE,           // 女
    NVRAM_CONFIG_SEX_CODE_MAX
}
NVRAMConfigSexCode;

// 血液型コード(4byte)
typedef enum NVRAMConfigBloodType
{
    NVRAM_CONFIG_BLOOD_A = 0,          // A 型
    NVRAM_CONFIG_BLOOD_B,              // B 型
    NVRAM_CONFIG_BLOOD_AB,             // AB型
    NVRAM_CONFIG_BLOOD_O,              // O 型
    NVRAM_CONFIG_BLOOD_TYPE_MAX
}
NVRAMConfigBloodType;

// 日付データ(4byte)
typedef struct NVRAMConfigDate
{
    u16     year;                      //年:1800～2100
    u8      month;                     //月:01～12
    u8      day;                       //日:01～31

}
NVRAMConfigDate;

// オーナー情報(32byte)
typedef struct NVRAMConfigOwnerInfo
{
    NVRAMConfigNickname nickname;      // ニックネーム
    NVRAMConfigSexCode sex;            // 性別
    NVRAMConfigBloodType bloodType;    // 血液型
    NVRAMConfigDate birthday;          // 生年月日

}
NVRAMConfigOwnerInfo;


// 起動ゲーム履歴データ(36byte)
typedef struct NVRAMConfigBootGameLog
{
    u32     gameCode[NVRAM_CONFIG_BOOT_GAME_LOG_NUM];   // 過去に起動したゲームのゲーム略号リスト（リングバッファ）
    u16     top;                       // 最終ログのインデックス番号
    u16     num;                       // ログの個数

}
NVRAMConfigBootGameLog;                // 36byte

// タッチパネル座標補正データ(12byte)
typedef struct NVRAMConfigTpCData
{
    u16     calib_data[6];             // キャリブレーション情報

}
NVRAMConfigTpCData;


// コンフィグ各種設定データ(96byte)
typedef struct NVRAMConfigData
{
    u8      version;                   // フラッシュ格納データフォーマットのバージョン
    u8      timezone;                  // タイムゾーン
    u16     agbLcd;                    // AGBモードで起動する時にどちらのLCDで起動するか？（0:TOP,1:BOTTOM）
    u32     rtcOffset;                 // RTC設定時のオフセット値（ユーザーがRTC設定を変更する度にその値に応じて増減します。）
    u32     language;                  // 言語コード
    NVRAMConfigOwnerInfo owner;        // オーナー情報
    NVRAMConfigTpCData tp;             // タッチパネルキャリブレーションデータ
    NVRAMConfigBootGameLog bootGameLog; // 起動ゲーム履歴

}
NVRAMConfigData;

// コンフィグ各種設定データのNVRAM保存時フォーマット(100byte)
typedef struct NVRAMConfig
{
    NVRAMConfigData ncd;               // コンフィグ各種設定データ
    u16     saveCount;                 // 0x00-0x7fをループしてカウントし、カウント値が新しいデータが有効。
    u16     crc16;                     // コンフィグ各種設定データの16bitCRC

}
NVRAMConfig;


/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C" */
#endif

#endif /* NITRO_SPI_COMMON_USERINFO_TEG_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
