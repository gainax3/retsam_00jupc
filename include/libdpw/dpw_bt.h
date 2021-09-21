/*---------------------------------------------------------------------------*
  Project:  DP WiFi Library
  File:     dpw_bt.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

/*! @file
	@brief	DP WiFi Battle Tower ライブラリ
	
	@author	kitayama(kitayama_shigetoshi@nintendo.co.jp)
	@author	Yamaguchi Ryo(yamaguchi_ryo@nintendo.co.jp)
	
*/

#ifndef DPW_BT_H_
#define DPW_BT_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _NITRO
	#include <dwc.h>
#else
	#include <nitro.h>
	#include <dwc.h>
#endif

#include "include/libdpw/dpw_common.h"
/*-----------------------------------------------------------------------*
					型・定数宣言
 *-----------------------------------------------------------------------*/

//! ランクの数
#define DPW_BT_RANK_NUM			10

//! ルーム数の最大値
#define DPW_BT_ROOM_NUM_MAX		100

//! Dpw_Bt_Callback の結果がエラーとなる場合の種別
typedef enum {
	DPW_BT_ERROR_SERVER_FULL = -1,		//!< サーバー容量オーバー
	DPW_BT_ERROR_SERVER_TIMEOUT = -2,	//!< サーバーからの応答がない
	DPW_BT_ERROR_ILLIGAL_REQUEST = -3,	//!< サーバへの不正な要求。不正な部屋番号を指定しています
	DPW_BT_ERROR_CANCEL	= -4,			//!< 処理が Dpw_Bt_CancelAsync() によってキャンセルされた
	DPW_BT_ERROR_FATAL = -5,			//!< 通信致命的エラー。電源の再投入が必要です
	DPW_BT_ERROR_DISCONNECTED = -6,		//!< 通信不能エラー。ライブラリの初期化が必要です。
	DPW_BT_ERROR_FAILURE = -7			//!< 通信失敗エラー。リトライしてください
} DpwBtError;

//! サーバーステータス
typedef enum {
	DPW_BT_STATUS_SERVER_OK,				//!< 正常
	DPW_BT_STATUS_SERVER_STOP_SERVICE,		//!< サービス一時停止中
	DPW_BT_STATUS_SERVER_FULL				//!< サーバー容量オーバー
} DpwBtServerStatus;

//! ポケモンのデータ。データの内容は見知しない
typedef struct {
	u8 data[56];		//!< ポケモンのデータ
} Dpw_Bt_PokemonData;

//! リーダーデータ構造体
typedef struct {
	u16 playerName[8];		//!< プレイヤー名
	u8 versionCode;			//!< バージョンコード
	u8 langCode;			//!< 言語コード
	u8 countryCode;			//!< 住んでいる国コード
	u8 localCode;			//!< 住んでいる地方コード
	u8 playerId[4];			//!< プレイヤーID
	s8 leaderMessage[8];	//!< リーダーメッセージ
	union{
		struct{
			u8 ngname_f	:1;	//!< NGネームフラグ
			u8 gender	:1;	//!< プレイヤーの性別フラグ
			u8			:6;	// あまり6bit
		};
		u8	  flags;
	};
	u8 padding;				// パディング
} Dpw_Bt_Leader;

//! プレイヤーデータ構造体
typedef struct {
	Dpw_Bt_PokemonData pokemon[3];	//!< ポケモンデータ
	u16 playerName[8];		//!< プレイヤー名
	u8 versionCode;			//!< バージョンコード
	u8 langCode;			//!< 言語コード
	u8 countryCode;			//!< 住んでいる国コード
	u8 localCode;			//!< 住んでいる地方コード
	u8 playerId[4];			//!< プレイヤーID
	s8 leaderMessage[8];	//!< リーダーメッセージ
	union{
		struct{
			u8 ngname_f	:1;	//!< NGネームフラグ
			u8 gender	:1;	//!< プレイヤーの性別フラグ
			u8			:6;	// あまり6bit
		};
		u8	  flags;
	};
	u8 trainerType;			//!< トレーナータイプ
	s8 message[24];			//!< メッセージ
	u16 result;				//!< 成績
} Dpw_Bt_Player;

//! ルームデータ構造体
typedef struct {
	Dpw_Bt_Player player[7];	//!< プレイヤーデータ
	Dpw_Bt_Leader leader[30];	//!< リーダーデータ
} Dpw_Bt_Room;


/*-----------------------------------------------------------------------*
					グローバル変数定義
 *-----------------------------------------------------------------------*/



/*-----------------------------------------------------------------------*
					関数外部宣言
 *-----------------------------------------------------------------------*/

extern void Dpw_Bt_Init(s32 pid, u64 friend_key);
extern void Dpw_Bt_Main(void);
extern void Dpw_Bt_End(void);
extern BOOL Dpw_Bt_IsAsyncEnd(void);
extern s32 Dpw_Bt_GetAsyncResult(void);
extern void Dpw_Bt_GetRoomNumAsync(s32 rank);
extern void Dpw_Bt_DownloadRoomAsync(s32 rank, s32 roomNo, Dpw_Bt_Room* roomData);
extern void Dpw_Bt_UploadPlayerAsync(s32 rank, s32 roomNo, s32 win, const Dpw_Bt_Player* player);
extern void Dpw_Bt_CancelAsync(void);
extern void Dpw_Bt_GetServerStateAsync(void);
extern void Dpw_Bt_SetProfileAsync(const Dpw_Common_Profile* data, Dpw_Common_ProfileResult* result);

BOOL Dpw_Bt_Db_InitServer(void);
BOOL Dpw_Bt_Db_UpdateServer(void);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // DPW_BT_H_