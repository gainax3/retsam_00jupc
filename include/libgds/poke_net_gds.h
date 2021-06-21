/*===========================================================================*
  Project:  Pokemon Global Data Station Network Library
  File:     poke_net_gds.h

  $NoKeywords: $
 *===========================================================================*/
/*! @file
	@brief	Pokemon GDS WiFi ライブラリ
	@date	2007/12/18
*/
//===========================================================================
/**
		@mainpage	Pokemon GDS WiFi ライブラリ
			<b>・概要</b><br>
			　本ライブラリは、NintendoDS上でポケモンＧＤＳ関連の通信を行う為のものです。<br>
			<b>・説明</b><br>
			　本ヘルプファイルはリファレンスマニュアルです。<br>
			　説明は別途 poke_net_inst.pdf を参照してください。<br>
			　<br>
		@version	0.04	レギュレーション、サーバーバージョン対応
		@version	0.03	最大最少定義など対応、バトルデータ検索対応
		@version	0.02	バトルデータ/ランキング機構追加<br>未実装部あり
		@version	0.01	初回リリース ドレスアップ/ボックスショット

*/

#ifndef ___POKE_NET_GDS___
#define ___POKE_NET_GDS___

#include "application/battle_recorder/gds_min_max.h"				// 最大最小定義
#include "poke_net_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------------*
					型・定数宣言
 *-----------------------------------------------------------------------*/

//! ポケモン WiFi GDS ライブラリの現在の状態
typedef enum {
	POKE_NET_GDS_STATUS_INACTIVE ,					//!< 非稼動中
	POKE_NET_GDS_STATUS_INITIALIZED ,				//!< 初期化済
	POKE_NET_GDS_STATUS_REQUEST ,					//!< リクエスト発行済
	POKE_NET_GDS_STATUS_NETSETTING ,				//!< ネットワーク設定中
	POKE_NET_GDS_STATUS_CONNECTING ,				//!< 接続中
	POKE_NET_GDS_STATUS_SENDING ,					//!< 送信中
	POKE_NET_GDS_STATUS_RECEIVING ,					//!< 受信中
	POKE_NET_GDS_STATUS_ABORTED ,					//!< 中断終了
	POKE_NET_GDS_STATUS_FINISHED ,					//!< 正常終了
	POKE_NET_GDS_STATUS_ERROR ,						//!< エラー終了
	POKE_NET_GDS_STATUS_COUNT
} POKE_NET_GDS_STATUS;

//! ポケモン WiFi GDS ライブラリがエラー終了した際の詳細
typedef enum {
	POKE_NET_GDS_LASTERROR_NONE ,					//!< 特になし
	POKE_NET_GDS_LASTERROR_NOTINITIALIZED ,			//!< 初期化されていない
	POKE_NET_GDS_LASTERROR_ILLEGALREQUEST ,			//!< リクエストが異常
	POKE_NET_GDS_LASTERROR_CREATESOCKET ,			//!< ソケット生成に失敗
	POKE_NET_GDS_LASTERROR_IOCTRLSOCKET ,			//!< ソケットのノンブロッキング化に失敗
	POKE_NET_GDS_LASTERROR_NETWORKSETTING ,			//!< ネットワーク設定失敗
	POKE_NET_GDS_LASTERROR_CREATETHREAD ,			//!< スレッド生成失敗
	POKE_NET_GDS_LASTERROR_CONNECT ,				//!< 接続失敗
	POKE_NET_GDS_LASTERROR_SENDREQUEST ,			//!< リクエストエラー
	POKE_NET_GDS_LASTERROR_RECVRESPONSE ,			//!< レスポンスエラー
	POKE_NET_GDS_LASTERROR_CONNECTTIMEOUT ,			//!< 接続タイムアウト
	POKE_NET_GDS_LASTERROR_SENDTIMEOUT ,			//!< 送信タイムアウト
	POKE_NET_GDS_LASTERROR_RECVTIMEOUT ,			//!< 受信タイムアウト
	POKE_NET_GDS_LASTERROR_ABORTED ,				//!< 中断終了
	POKE_NET_GDS_LASTERROR_GETSVL ,					//!< SVL取得エラー
	POKE_NET_GDS_LASTERROR_COUNT
} POKE_NET_GDS_LASTERROR;

/*
// = 社内単体公開サーバー =
#define POKE_NET_GDS_URL		"192.168.1.219"				// サーバーURL
#define POKE_NET_GDS_PORTNO		23457						// サーバーポート番号
*/
/*
// = 社内複数サーバー：管理 =
#define POKE_NET_GDS_URL		"192.168.1.105"				// サーバーURL
#define POKE_NET_GDS_PORTNO		13570						// サーバーポート番号
*/
/*
// = 社内複数サーバー:スレーブ =
#define POKE_NET_GDS_URL		"192.168.1.105"				// サーバーURL
#define POKE_NET_GDS_PORTNO		13571						// サーバーポート番号
*/
/*
#define POKE_NET_GDS_URL		"pmsv.denyu-sha.to"
#define POKE_NET_GDS_PORTNO		12400
*/

#define POKE_NET_GDS_URL		"pkgdsprod.nintendo.co.jp"	// サーバーURL
#define POKE_NET_GDS_PORTNO		12400						// サーバーポート番号


/*-----------------------------------------------------------------------*
					グローバル変数定義
 *-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*
					関数外部宣言
 *-----------------------------------------------------------------------*/
extern BOOL POKE_NET_GDS_Initialize(POKE_NET_REQUESTCOMMON_AUTH *_auth);
extern void POKE_NET_GDS_Release();
extern void POKE_NET_GDS_SetThreadLevel(unsigned long _level);
extern POKE_NET_GDS_STATUS POKE_NET_GDS_GetStatus();
extern POKE_NET_GDS_LASTERROR POKE_NET_GDS_GetLastErrorCode();
extern BOOL POKE_NET_GDS_Abort();

extern BOOL POKE_NET_GDS_DebugMessageRequest(char *_message ,void *_response);

extern BOOL POKE_NET_GDS_DressupShotRegist(GT_GDS_DRESS_SEND *_data ,void *_response);
extern BOOL POKE_NET_GDS_DressupShotGet(long _pokemonno ,void *_response);

extern BOOL POKE_NET_GDS_BoxShotRegist(long _groupno ,GT_BOX_SHOT_SEND *_data ,void *_response);
extern BOOL POKE_NET_GDS_BoxShotGet(long _groupno ,void *_response);

extern BOOL POKE_NET_GDS_RankingGetType(void *_response);
extern BOOL POKE_NET_GDS_RankingUpdate(GT_RANKING_MYDATA_SEND *_data ,void *_response);

extern void *POKE_NET_GDS_GetResponse();
extern long POKE_NET_GDS_GetResponseSize();
extern long POKE_NET_GDS_GetResponseMaxSize(long _reqno);

extern BOOL POKE_NET_GDS_BattleDataRegist(GT_BATTLE_REC_SEND *_data ,void *_response);
extern BOOL POKE_NET_GDS_BattleDataSearchCondition(GT_BATTLE_REC_SEARCH_SEND *_condition ,void *_response);
extern BOOL POKE_NET_GDS_BattleDataSearchRanking(GT_BATTLE_REC_RANKING_SEARCH_SEND *_condition ,void *_response);
extern BOOL POKE_NET_GDS_BattleDataSearchExRanking(GT_BATTLE_REC_RANKING_SEARCH_SEND *_condition ,void *_response);
extern BOOL POKE_NET_GDS_BattleDataGet(u64 _code ,u32 _sver ,void *_response);
extern BOOL POKE_NET_GDS_BattleDataFavorite(u64 _code ,void *_response);

extern void POKE_NET_GDS_DEBUG_PrintResponse(POKE_NET_RESPONSE *_response ,long _size);

#ifndef SDK_FINALROM

#else

#endif


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // ___POKE_NET_GDS___
