/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - include
  File:     mb_gameinfo.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_gameinfo.h,v $
  Revision 1.6  2006/01/18 02:11:29  kitase_hirotake
  do-indent

  Revision 1.5  2005/02/28 05:26:24  yosizaki
  do-indent.

  Revision 1.4  2005/01/18 00:20:39  takano_makoto
  fix Copyright header.

  Revision 1.3  2005/01/17 09:28:04  takano_makoto
  MB_XXXUserBeaconData を MB_XXXUserVolatDataへ変更。MB_SetSendVolatCallback関数を追加。

  Revision 1.2  2005/01/17 05:50:02  takano_makoto
  MB_SetUserBeaconData, MB_GetUserBeaconDataを追加

  Revision 1.1  2004/11/22 12:57:34  takano_makoto
  Initial update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#ifndef NITRO_MB_MB_GAMEINFO_H_
#define NITRO_MB_MB_GAMEINFO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/types.h>
#include <nitro/mb/mb.h>

#define MB_USER_VOLAT_DATA_SIZE     8


typedef void (*MBSendVolatCallbackFunc) (u32 ggid);

/*
 * 親機ゲーム情報のうち、変化しない情報.
 * (seqNoが変化したら、受信し直し。（但し、普通は固定のまま）)
 */
typedef struct MBGameInfoFixed
{
    MBIconInfo icon;                   // 544B     アイコンデータ
    MBUserInfo parent;                 //  22B     親機ユーザー情報
    u8      maxPlayerNum;              //   1B     最大プレイヤー人数
    u8      pad[1];
    u16     gameName[MB_GAME_NAME_LENGTH];      //  96B     ゲームタイトル
    u16     gameIntroduction[MB_GAME_INTRO_LENGTH];     // 192B     ゲーム内容説明
}
MBGameInfoFixed, MbGameInfoFixed;


/*
 * 親機ゲーム情報のうち、流動的に変化する情報。
 *（seqNo値が変化したら、受信し直し）
 */
typedef struct MBGameInfoVolatile
{
    u8      nowPlayerNum;              //   1B     現在のプレイヤー数
    u8      pad[1];
    u16     nowPlayerFlag;             //   2B     現在の全プレイヤーのプレイヤー番号をビットで示す。
    u16     changePlayerFlag;          //   2B     今回の更新で変更されたプレイヤー情報の番号をフラグで示す。
    MBUserInfo member[MB_MEMBER_MAX_NUM];       // 330B     メンバー情報
    u8      userVolatData[MB_USER_VOLAT_DATA_SIZE];     //   8B     ユーザがセットできるデータ
}
MBGameInfoVolatile, MbGameInfoVolatile;


/* ビーコン属性 */
typedef enum MbBeaconDataAttr
{
    MB_BEACON_DATA_ATTR_FIXED_NORMAL = 0,       /* アイコンデータありの固定データ */
    MB_BEACON_DATA_ATTR_FIXED_NO_ICON, /* アイコンデータなしの固定データ */
    MB_BEACON_DATA_ATTR_VOLAT          /* メンバー情報等の流動的なデータ */
}
MBBeaconDataAttr, MbBeaconDataAttr;

/*  dataAttr = FIXED_NORMAL の時は、MbGameInfoFixedのデータを先頭  からMB_BEACON_DATA_SIZE単位で分割して送信。
    dataAttr = FIXED_NO_ICON            〃                   MBUserInfo から　　〃                                （アイコンデータ省略）
    dataAttr = VOLATILEDの時は、「今回の送信人数×ユーザー情報」で送信
*/

/*
 * 親機ゲーム情報ビーコン
 */
typedef struct MBGameInfo
{
    MBGameInfoFixed fixed;             // 固定データ
    MBGameInfoVolatile volat;          // 流動データ
    u16     broadcastedPlayerFlag;     // 　　〃　　　　　　の配信済みプレイヤー情報をビットで示す。
    u8      dataAttr;                  // データ属性
    u8      seqNoFixed;                // 固定領域のシーケンス番号
    u8      seqNoVolat;                // 流動領域のシーケンス番号
    u8      fileNo;                    // ファイルNo.
    u8      pad[2];
    u32     ggid;                      // GGID
    struct MBGameInfo *nextp;          // 次のGameInfoへのポインタ（片方向リスト）
}
MBGameInfo, MbGameInfo;


enum
{
    MB_SEND_VOLAT_CALLBACK_TIMMING_BEFORE,
    MB_SEND_VOLAT_CALLBACK_TIMMING_AFTER
};

void    MB_SetSendVolatCallback(MBSendVolatCallbackFunc callback, u32 timming);
void    MB_SetUserVolatData(u32 ggid, const u8 *userData, u32 size);
void   *MB_GetUserVolatData(const WMGameInfo *gameInfo);



#ifdef __cplusplus
}
#endif


#endif // NITRO_MB_MB_GAMEINFO_H_
