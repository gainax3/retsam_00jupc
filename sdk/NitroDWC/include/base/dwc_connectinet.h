/*---------------------------------------------------------------------------*
  Project:  NitroDWC - base - dwc_connectinet
  File:     dwc_connectinet.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_connectinet.h,v $
  Revision 1.24  2007/03/02 06:26:59  nakata
  DWC_APINFO_AREA_KORを追加。

  Revision 1.23  2006/11/17 06:30:07  nakata
  TestInet関連の定数の名称変更

  Revision 1.22  2006/11/07 04:59:30  nakata
  DWC_TestInet系の追加

  Revision 1.21  2006/06/27 06:43:56  nakata
  FriendsMatchライブラリなし用のDCF監視関数を追加

  Revision 1.20  2006/05/25 23:54:11  uemura
  DWC_CheckWiFiStation関数を追加

  Revision 1.19  2006/04/17 07:33:34  nakata
  ブラウザ用ホットスポット設定関数DWC_EnableHotSpotを追加

  Revision 1.18  2006/03/09 08:05:52  uemura
  DWC_SetConnectApTypeの引数をintからDWCApInfoTypeへ変更

  Revision 1.17  2006/03/09 07:35:13  nakata
  DWCApInfoからspotinfoを消去し、関連部分を変更

  Revision 1.16  2006/03/09 06:07:41  uemura
  DWC_DebugConnectInetAsyncの引数WEPに16進０値が入っていると正しく判定できないバグを修正
  引数にWEPMODEを渡すように変更しました

  Revision 1.15  2006/03/09 02:50:37  uemura
  コメント修正

  Revision 1.14  2006/03/09 02:27:17  uemura
  DWC_SetConnectApType,DWC_DebugConnectInetAsyncの追加
  この2関数をdwc_connect_inet_sampleへコンパイルスイッチで使用できるように追加する

  Revision 1.13  2006/02/23 08:21:47  nakata
  DWCApInfo周りの定数名等を変更。

  Revision 1.12  2006/02/23 06:58:04  nakata
  DWCApinfo.bssidの型をcharからunsigned charに変更。

  Revision 1.11  2006/02/23 02:55:40  nakata
  DWC_GetApInfo関数関連のマニュアルを追加。

  Revision 1.10  2006/02/23 00:53:54  nakata
  AP情報取得関数DWC_GetApInfoをdwc_connectinet.cに追加
  dwc_sampleの不必要なデバッグメッセージ表示を抑制

  Revision 1.9  2005/10/06 07:39:38  sasakit
  SSLの多倍長累乗計算スレッドの優先順位を下げる機能を外部から調整できるようにした。

  Revision 1.8  2005/10/01 10:57:00  sasakit
  DWC_CleanupInetAsync()中に、DWC_ProcessInet()を呼んでもDISCONNECTEDにならないようにした。

  Revision 1.7  2005/09/17 00:31:18  sasakit
  UDPパケットを落とした数を取得する関数の追加。

  Revision 1.6  2005/09/14 01:55:31  sasakit
  APからの切断処理を整理した。
  DWC_GetLinkLevel()関数の追加。

  Revision 1.5  2005/09/07 07:13:55  sasakit
  DWC_AC関数群を厳密に呼び出すようにステートマシンを入れた。

  Revision 1.4  2005/09/06 09:12:38  sasakit
  ConnectInet時のエラーの処理を追加。

  Revision 1.3  2005/08/31 04:13:43  sasakit
  サーバの切り替え機能追加。

  Revision 1.2  2005/08/20 07:01:19  sasakit
  ヘッダインクルードガードの書式を統一した。
  bm/dwc_init.h -> bm/dwc_bm_init.hに変更
  ヘッダにCopyrightをつけた。
  構造体の名前空間をできるだけルールにちかづけた。
  util_wifiidtool.hとdwc_backup.hの関係を若干修正。


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_CONNECTINET_H_
#define DWC_CONNECTINET_H_

#include <ac/dwc_ac.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DWCstInetControl
{
    volatile int ac_state;
    u16  state;
    u16  online;
    u16  dmaNo;
    u16  powerMode;

    u8            wontExecuteWDS;  // nintendo spotへの接続を試みるフェーズを中止するかどうかのフラグ [masuda]
    DWCWDSState   processWDSstate; // WDSの処理ステート
    DWCWDSData    nspotInfo;       // WDSで取得したssid,wep,wepmode,apnumを格納するための変数

#ifndef SDK_FINALROM
	int  connectType;
#endif	// SDK_FINALROM
} DWCInetControl;

typedef enum
{
    DWC_CONNECTINET_STATE_NOT_INITIALIZED = 0,
    DWC_CONNECTINET_STATE_IDLE,
    DWC_CONNECTINET_STATE_OPERATING,
    DWC_CONNECTINET_STATE_OPERATED,
    DWC_CONNECTINET_STATE_CONNECTED,
    DWC_CONNECTINET_STATE_DISCONNECTING,
    DWC_CONNECTINET_STATE_DISCONNECTED,
    DWC_CONNECTINET_STATE_ERROR,
    DWC_CONNECTINET_STATE_FATAL_ERROR,

    DWC_CONNECTINET_STATE_LAST
} DWCInetResult;

typedef enum
{
    DWC_CONNECTINET_AUTH_TEST,
    DWC_CONNECTINET_AUTH_DEVELOP,
    DWC_CONNECTINET_AUTH_RELEASE,
    DWC_CONNECTINET_AUTH_LAST
} DWCInetAuthType;

// [nakata] ここよりAP情報取得関連の拡張部分

// エリアコード
typedef enum {
	DWC_APINFO_AREA_JPN = 0,
	DWC_APINFO_AREA_USA,
	DWC_APINFO_AREA_EUR,
	DWC_APINFO_AREA_AUS,
    DWC_APINFO_AREA_KOR,
	DWC_APINFO_AREA_UNKNOWN = 0xff
} DWCApInfoArea;

// APタイプ
typedef enum {
	DWC_APINFO_TYPE_USER0 = 0,
	DWC_APINFO_TYPE_USER1,
	DWC_APINFO_TYPE_USER2,
	DWC_APINFO_TYPE_USB,
	DWC_APINFO_TYPE_SHOP,
	DWC_APINFO_TYPE_FREESPOT,
	DWC_APINFO_TYPE_WAYPORT,
	DWC_APINFO_TYPE_OTHER,          /* 旧定義 */
	DWC_APINFO_TYPE_NINTENDOWFC = DWC_APINFO_TYPE_OTHER,    /* 予約 */
	DWC_APINFO_TYPE_NINTENDOSPOT,
	DWC_APINFO_TYPE_UNKNOWN = 99    /* 不明 */
} DWCApInfoType;

#define DWC_SIZE_SPOTINFO 9

typedef struct DWCstApInfo
{
	DWCApInfoType		aptype;
	DWCApInfoArea		area;
	char				spotinfo[DWC_SIZE_SPOTINFO+1];
	char				essid[WM_SIZE_SSID+1];
	u8 					bssid[WM_SIZE_BSSID];
} DWCApInfo;

BOOL DWC_GetApInfo(DWCApInfo *apinfo);
// [nakata] ここまでAP情報取得関連の拡張部分

// [nakata] ここよりブラウザ対応の拡張部分
void DWC_EnableHotspot(void);
// [nakata] ここまでブラウザ対応の拡張部分

// [uemura] 2006/05/24追加
BOOL DWC_CheckWiFiStation( const void *ssid, u16 len );
// [uemura] ここまで

typedef void* (* DWCACAlloc )( u32 name, s32 size );				// alloc関数へのポインタ
typedef void  (* DWCACFree  )( u32 name, void *ptr, s32 size );		// free関数へのポインタ

extern void  DWC_InitInetEx( DWCInetControl* inetCntl, u16 dmaNo, u16 powerMode, u16 ssl_priority );
extern void  DWC_InitInet( DWCInetControl* inetCntl );
extern void  DWC_SetAuthServer( DWCInetAuthType type );

extern void  DWC_ConnectInetAsync( void );
extern void  DWC_DebugConnectInetAsync( const void *ssid, const void *wep, int wepMode );
extern void  DWC_DebugConnectInetExAsync( const void *ssid, const void *wep, int wepMode, const char *apSpotInfo, int overrideType );

extern BOOL  DWC_CheckInet( void );

extern void  DWC_ProcessInet( void );

extern DWCInetResult DWC_GetInetStatus( void );

extern void  DWC_CleanupInet( void );
extern BOOL  DWC_CleanupInetAsync( void );

extern BOOL  DWCi_CheckDisconnected( void );
extern WMLinkLevel DWC_GetLinkLevel( void );
extern int   DWC_GetUdpPacketDropNum( void );

// [nakata] 06/06/27 dwc_main.cから移動
extern void DWC_ForceShutdown( void );
extern BOOL DWC_UpdateConnection( void );
// [nakata] ここまで

// [nakata] 06/11/06 インターネット接続確認処理をラップする関数を追加
typedef enum
{
    DWC_TESTINET_NOT_INITIALIZED = 0,
    DWC_TESTINET_OPERATING,
    DWC_TESTINET_CONNECTED,
    DWC_TESTINET_DISCONNECTED
} DWCTestInetResult;

BOOL DWC_TestInetAsync(int timeout);
DWCTestInetResult DWC_TestInetProcess(void);
// [nakata] ここまで

#ifndef SDK_FINALROM
void         DWC_SetConnectApType( DWCApInfoType type );
#else
#define      DWC_SetConnectApType( type )       ((void)0)
#endif

#ifdef __cplusplus
}
#endif


#endif // DWC_CONNECTINET_H_
