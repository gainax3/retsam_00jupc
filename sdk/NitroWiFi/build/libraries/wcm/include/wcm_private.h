/*---------------------------------------------------------------------------*
  Project:  NitroWiFi - WCM - include
  File:     wcm_private.h

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wcm_private.h,v $
  Revision 1.9  2006/03/10 09:22:19  kitase_hirotake
  INDENT SOURCE

  Revision 1.8  2005/10/05 13:44:40  yasu
  OS_SecondsToTicks() のバグの対策

  Revision 1.7  2005/10/05 09:40:38  terui
  キープアライブ用Nullパケット送信間隔を60秒→70秒に変更

  Revision 1.6  2005/10/05 08:53:32  terui
  電波強度蓄積、及び取得関数のプロトタイプ宣言を追加

  Revision 1.5  2005/09/12 10:01:13  terui
  WCM_CONNECT_STATUSCODE_ILLEGAL_RATESET定数を追加

  Revision 1.4  2005/09/01 13:11:03  terui
  Keep Alive パケット送信間隔を定数定義
  アラーム用変数をワーク構造体に追加
  コネクト失敗時の wlStatus を保持する変数をワーク構造体に追加

  Revision 1.3  2005/08/09 07:58:30  terui
  WEPキーを退避する内部バッファを32バイトアラインした位置に調整

  Revision 1.2  2005/07/11 12:05:31  terui
  WCMWork 構造体にキャッシュラインを合わせる為のダミーメンバを追加。

  Revision 1.1  2005/07/07 10:45:37  terui
  新規追加

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NITROWIFI_WCM_PRIVATE_H_
#define NITROWIFI_WCM_PRIVATE_H_

#ifdef __cplusplus

extern "C" {
#endif

/*===========================================================================*/
#include <nitroWiFi/wcm.h>
#include <nitro/os.h>

/*---------------------------------------------------------------------------*
    定数 定義
 *---------------------------------------------------------------------------*/

// リセット重複呼び出し管理フラグの状態
#define WCM_RESETTING_OFF   0
#define WCM_RESETTING_ON    1

// AP 情報保持リスト内の各ブロックの使用状態
#define WCM_APLIST_BLOCK_EMPTY  0
#define WCM_APLIST_BLOCK_OCCUPY 1

// Keep Alive 用 NULL パケット送信間隔(秒)
#define WCM_KEEP_ALIVE_SPAN ((OSTick) 70)

// デバッグ表示有無設定
#ifdef SDK_FINALROM
#define WCM_DEBUG   0
#else
#define WCM_DEBUG   1
#endif

// 接続時の失敗コード定義
#define WCM_CONNECT_STATUSCODE_ILLEGAL_RATESET  18

#if WCM_DEBUG
#define WCMi_Printf     OS_TPrintf("[WCM] %s(%d) Report: ", __FILE__, __LINE__); \
    OS_TPrintf
#define WCMi_Warning    OS_TPrintf("[WCM] %s(%d) Warning: ", __FILE__, __LINE__); \
    OS_TPrintf
#else
#define WCMi_Printf(...)    ((void)0)
#define WCMi_Warning(...)   ((void)0)
#endif

/*---------------------------------------------------------------------------*
    構造体 定義
 *---------------------------------------------------------------------------*/

// WCM 内部ワーク変数群をまとめた構造体
typedef struct WCMWork
{
    u8          wmWork[WCM_ROUNDUP32( WM_SYSTEM_BUF_SIZE ) ];
    u8          sendBuf[WCM_DCF_SEND_BUF_SIZE];
    u8          recvBuf[WCM_DCF_RECV_BUF_SIZE * 2];
    WMBssDesc   bssDesc;
    struct
    {
        u8  key[WM_SIZE_WEPKEY];
        u8  mode;
        u8  keyId;
        u8  reserved[WCM_ROUNDUP32( WM_SIZE_WEPKEY + 2 ) - ( WM_SIZE_WEPKEY + 2 ) ];
    } wepDesc;
    u32         phase;
    u32         option;
    u16         maxScanTime;
    u8          apListLock;
    u8          resetting;
    WCMConfig   config;
    WCMNotify   notify;
    s16         notifyId;
    u16         authId;
    u32         scanCount;
    WMScanExParam   scanExParam;
    OSAlarm         keepAliveAlarm;
    u16 wlStatusOnConnectFail;
    u8  reserved[6];

} WCMWork;

// AP 情報保持リスト内のブロック構造体
typedef struct WCMApList
{
    u8      state;
    u8      reserved;
    u16     linkLevel;
    u32     index;
    void*   previous;
    void*   next;
    u32     data[WCM_ROUNDUP4( WCM_APLIST_SIZE ) / sizeof(u32) ];
} WCMApList;

// AP 情報保持リストの管理用ヘッダ構造体
typedef struct WCMApListHeader
{
    u32         count;
    WCMApList*  head;
    WCMApList*  tail;

} WCMApListHeader;

/*---------------------------------------------------------------------------*
    関数 定義
 *---------------------------------------------------------------------------*/

// WCM ライブラリの内部管理構造体へのポインタを取得
WCMWork*    WCMi_GetSystemWork(void);

// AP 情報発見時に呼び出される 保持リストへのエントリー追加(更新)要求
void        WCMi_EntryApList(WMBssDesc* bssDesc, u16 linkLevel);

// WCM_Init 関数から呼び出される CPS インターフェース初期化関数
void        WCMi_InitCpsif(void);

// DCF データ受信時に呼び出される CPS インターフェースへのコールバック関数
void        WCMi_CpsifRecvCallback(WMDcfRecvBuf* recv);

// Keep Alive 用 NULL パケットを送信する関数
void        WCMi_CpsifSendNullPacket(void);

// Keep Alive 用アラームをリセットする関数
void        WCMi_ResetKeepAliveAlarm(void);

// DCF データ受信時に受信電波強度を一旦退避する関数
void        WCMi_ShelterRssi(u8 rssi);
u8          WCMi_GetRssiAverage(void);

/*===========================================================================*/
#ifdef __cplusplus

}       /* extern "C" */
#endif

#endif /* NITROWIFI_WCM_PRIVATE_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
