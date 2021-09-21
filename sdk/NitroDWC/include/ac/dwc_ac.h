/*---------------------------------------------------------------------------*
  Project:  NitroDWC - ac - dwc_ac
  File:     dwc_ac.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  Revision 1.5  2005/08/20 07:01:18  sasakit
  ヘッダインクルードガードの書式を統一した。
  bm/dwc_init.h -> bm/dwc_bm_init.hに変更
  ヘッダにCopyrightをつけた。
  構造体の名前空間をできるだけルールにちかづけた。
  util_wifiidtool.hとdwc_backup.hの関係を若干修正。


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_AC_H_
#define DWC_AC_H_

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*
 *	Project		: Wi-Fi Connection
 *	Description	: ネットワークへの自動接続ライブラリ用ヘッダ
 *	File		: dwc_ac.h
 *	Version		: 0.0
 *	Auther		: Uemura
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 *	定義
 *---------------------------------------------------------------------------*/
// 自動接続中の状態( DWC_AC_GetStatusの戻り値 )
enum{
	// -10000 以上のエラーはコメントの表示が必要になります
	// -2xxxx NetCheck中のエラーコード、主に認証系のエラー
	// -5xxxx ACとNetCheck中のエラーコード、接続失敗時のエラー（詳細は下記）
	// -5xxxxのxxxxには最もインターネット接続に近づいた接続先のタイプとエラー番号が入ります。
	// 上二桁にはエラー番号が入り、下二桁は接続先タイプを示します。
	// 上二桁  下二桁 エラー内容
	// 00      XX     通信が切断された。
	// 00      99     ＡＰのビーコンが全くみつからない。
	// 10      99     ＡＰのビーコンは見つかったが、SSIDが一致するものがない。
	// 11      XX     WEPキー間違い、モード違い
	// 12      XX     APの接続制限数をオーバーした。
	// 13      XX     ＡＰに接続できない。
	// 20      XX     DHCPエラー。
	// 21      XX     名前解決できない。DNSエラー。
	// 22      XX     インターネットに接続できない1 Test Blacklist Site 時にHTTPExcluding 200/302 or Timeout)
	// 23      XX     インターネットに接続できない2 (call Account Creates 時に HTTP Excluding 200 or Timeout or  CAinvalid)
	// 30      XX     インターネットに接続できない3 (hot spot error : Post203HTML and SSID to NAS　時に　HTTPExcluding200 or Timeout or  CAinvalid)
	// 31      XX     インターネットに接続できない4 (hot spot error : Postto Hotspot Auth Server (HTTPS) 時にTimeout  CAinvalid)
	// 32      XX     インターネットに接続できない5 (hot spot error : ２度目のTest Blacklist site 時に　HTTP302)

	// 以下はプログラム的なエラー群
	DWC_AC_STATE_ERROR_FATAL            = -10,				// エラー 無線通信に致命的なエラーが発生
	DWC_AC_STATE_ERROR_START_UP,							// エラー 接続開始（WCM_StartupAsync）失敗
	DWC_AC_STATE_ERROR_SOCKET_START,						// エラー ソケット開始（SOC_Startup）失敗
	DWC_AC_STATE_ERROR_NETCHECK_CREATE,						// エラー 接続性テスト開始（DWC_Netcheck_Create）失敗
	DWC_AC_STATE_ERROR_IRREGULAR,							// エラー WCM_GetPhaseの戻り値が「WCM_PHASE_IRREGULAR」になった場合

	// 以下はDWC_AC_Process中の状態
	DWC_AC_STATE_NULL                   = 0,				// 初期状態
	DWC_AC_STATE_SEARCH,									// 検索中
	DWC_AC_STATE_CONNECT,									// ＡＰへ接続中
	DWC_AC_STATE_TEST,										// インターネットへの接続確認中
	DWC_AC_STATE_RETRY,										// 別のＡＰへ再接続中
	DWC_AC_STATE_COMPLETE									// 接続完了
};

// アクセスポイントの種類( DWC_AC_GetApTypeの戻り値 )
enum{
	DWC_AC_AP_TYPE_USER1,									// ユーザー設定１
	DWC_AC_AP_TYPE_USER2,									// ユーザー設定２
	DWC_AC_AP_TYPE_USER3,									// ユーザー設定３
	DWC_AC_AP_TYPE_USB,										// ニンテンドーWi-Fi USBコネクタ
	DWC_AC_AP_TYPE_SHOP,									// ニンテンドーWi-Fi ステーション
	DWC_AC_AP_TYPE_FREESPOT,								// FreeSpot
	DWC_AC_AP_TYPE_WAYPORT,									// WayPort
	DWC_AC_AP_TYPE_NINTENDOWFC,								// NINTENDOWFC
	DWC_AC_AP_TYPE_NINTENDOSPOT,							// nintendo spot
	DWC_AC_AP_TYPE_UNKNOWN = 99,							// 不明(戻り値としては返されません)
	DWC_AC_AP_TYPE_FALSE = 0xff								// 取得失敗
};

// WDS用バッファサイズ
#define	DWC_WDS_SSID_BUF_SIZE				32
#define DWC_WDS_WEPKEY_BUF_SIZE             32

// WDS用ステート
typedef enum
{
    DWC_WDS_STATE_IDLE      = 0,
    DWC_WDS_STATE_PROCESS   = 1,
    DWC_WDS_STATE_COMPLETED = 2, // APのデータを取得できた。
    DWC_WDS_STATE_FAILED    = 3, // 取得できなかった。
    DWC_WDS_STATE_ERROR     = 4
} DWCWDSState;


/*---------------------------------------------------------------------------*
 *	構造体定義
 *---------------------------------------------------------------------------*/
typedef struct tagDWCACOption{								// AC用オプション設定
	u8          connectType;								// 接続先タイプの指定（０：指定無し　１～８：接続先指定）
	u8          skipNetCheck;								// ネットチェックのスキップ（１：チェック無　０：チェック有）
} DWCACOption;

typedef struct tagDWCACConfig{								// DWC_AC関数群用コンフィグ
	void*       (* alloc )( u32 name, s32 size );			// alloc関数へのポインタ
	void        (* free  )( u32 name, void *ptr, s32 size );// free 関数へのポインタ
	u8          dmaNo;										// ０～３
	u8          powerMode;									// １：ACTIVE　０：SAVE
	DWCACOption option;										// オプション設定
} DWCACConfig;

typedef struct tagDWCWDSData{
    u8          ssid[ DWC_WDS_SSID_BUF_SIZE ];
    u8          wep[ DWC_WDS_WEPKEY_BUF_SIZE ];
    int         wepMode;
    char        apnum[10];
} DWCWDSData;

/*---------------------------------------------------------------------------*
 *	関数定義
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * Name			: DWC_AC_Create
 * Description	: 初期化関数　メモリの確保、DWC_AC、WCMの初期化を行います
 * Arguments	: *config - DWC_AC用コンフィグデータ
 * Returns		: BOOL    - TRUE  ： 成功
 *				:           FALSE ： 失敗
 *---------------------------------------------------------------------------*/
BOOL DWC_AC_Create( DWCACConfig *config );

/*---------------------------------------------------------------------------*
 * Name			: DWC_AC_Process
 * Description	: 自動接続のメイン関数です
 *				: ０以外が戻るまで毎フレームコールしてください
 * Arguments	: None.
 * Returns		: int -  0 : 処理中
 *				:        1 : 接続完了
 *				:        2 : 接続完了、ただし同名のSSIDを複数発見(ユーザー設定APに限る)
 *				:       -1 : エラー発生
 *---------------------------------------------------------------------------*/
int  DWC_AC_Process( void );

/*---------------------------------------------------------------------------*
 * Name			: DWC_AC_GetStatus
 * Description	: 自動接続中のDWC_ACライブラリ状態を取得します
 * Arguments	: None.
 * Returns		: int - 正の値 : 現在の処理内容
 *				:       不の値 : エラー詳細
 *---------------------------------------------------------------------------*/
int  DWC_AC_GetStatus( void );

/*---------------------------------------------------------------------------*
 * Name			: DWC_AC_GetApType
 * Description	: 接続したAPのタイプを取得します
 *				: DWC_AC_Processが成功した後でなければ取得は成功しません
 * Arguments	: None.
 * Returns		: u8 - 0 ～ 7 ： 接続したAPのタイプ
 *				:        0xff ： 取得失敗
 *---------------------------------------------------------------------------*/
u8   DWC_AC_GetApType( void );

/*---------------------------------------------------------------------------*
 * Name			: DWC_AC_GetApSpotInfo
 * Description	: 接続したAPの場所情報を取得します
 *				: DWC_AC_Processが成功した後でなければ取得は成功しません
 * Arguments	: *apSpotInfo - 情報を格納するバッファ(10バイト必要です)
 * Returns		: BOOL    - TRUE  ： 成功
 *				:           FALSE ： 失敗
 *---------------------------------------------------------------------------*/
BOOL DWC_AC_GetApSpotInfo( u8 *apSpotInfo );

/*---------------------------------------------------------------------------*
 * Name			: DWC_AC_Destroy
 * Description	: 終了関数、切断処理、バッファの開放を行います
 *				: TRUEになるまで毎フレームコールしてください
 * Arguments	: None.
 * Returns		: BOOL - TRUE  ： 成功
 *				:        FALSE ： 失敗
 *---------------------------------------------------------------------------*/
BOOL DWC_AC_Destroy( void );

/*---------------------------------------------------------------------------*
 * Name			: DWC_AC_SetSpecifyAp
 * Description	: 指定されたAPへの接続設定を行う
 * Arguments	: *ssid   - 接続するAPのSSID
 *              : *wep    - 接続するAPのWepキー
 *              : wepMode - Wepキーのモード
 * Returns		: None.
 *---------------------------------------------------------------------------*/
void DWC_AC_SetSpecifyAp( const void *ssid, const void *wep, int wepMode );

/*---------------------------------------------------------------------------*
 * Name			: DWC_AC_SetSpecifyApEx
 * Description	: 指定されたAPへの接続設定を行う(場所情報付)
 * Arguments	: *ssid        - 接続するAPのSSID
 *              : *wep         - 接続するAPのWepキー
 *              : wepMode      - Wepキーのモード
 *              : apSpotInfo   - 10文字のAP場所情報(不要ならNULLを指定)
 *              :                10バイト固定で読み出します
 *              : overrideType - 上書きする接続先タイプ
 * Returns		: None.
 *---------------------------------------------------------------------------*/
void DWC_AC_SetSpecifyApEx( const void *ssid, const void *wep, int wepMode, const char *apSpotInfo, int overrideType );

/*---------------------------------------------------------------------------*
 * Name			: DWC_AC_CheckWiFiStation
 * Description	: APのSSIDからWi-Fiステーションかどうかを確認する
 * Arguments	: *ssid - 調査するAPのSSID
 *              : len   - SSIDの長さ
 * Returns		: BOOL  - TRUE  ： 成功
 *				:         FALSE ： 失敗
 *---------------------------------------------------------------------------*/
BOOL DWC_AC_CheckWiFiStation( const void *ssid, u16 len );

/*---------------------------------------------------------------------------*
 * Name			: DWC_AC_StartupGetWDSInfo
 * Description	: nintendo spotの接続するための情報を取得する　初期化
 * Arguments	: DWCWDSData
 *              : 
 * Returns		: BOOL  - TRUE  ： 成功
 *				:         FALSE ： 失敗
 *---------------------------------------------------------------------------*/
BOOL DWC_AC_StartupGetWDSInfo( DWCWDSData* nspotInfo );

/*---------------------------------------------------------------------------*
 * Name			: DWC_AC_ProcessGetWDSInfo
 * Description	: nintendo spotへの接続するための情報を取得する　処理を進める
 * Arguments	: 
 *              : 
 * Returns		: DWCWDSResult
 *---------------------------------------------------------------------------*/
DWCWDSState DWC_AC_ProcessGetWDSInfo( void );

/*---------------------------------------------------------------------------*
 * Name			: DWC_AC_CleanupGetWDSInfo
 * Description	: nintendo spotへの接続するための情報を取得する　開放
 * Arguments	: 
 *              : 
 * Returns		: None.
 *---------------------------------------------------------------------------*/
void DWC_AC_CleanupGetWDSInfo( void );



#ifdef __cplusplus
}
#endif


#endif // DWC_AC_H_
