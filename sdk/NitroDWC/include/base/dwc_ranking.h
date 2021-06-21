/*---------------------------------------------------------------------------*
  Project:  NitroDWC -  - 
  File:     dwc_ranking.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_ranking.h,v $
  Revision 1.19  2007/08/23 23:56:15  takayama
  DWC_2_2_RC1にロールバック。

  Revision 1.15  2007/06/22 06:41:10  takayama
  定義の変更。
  DWC_RNK_CATEGORY_MAX 100 -> 1000
  DWC_RNK_GET_MAX      10  -> 30

  Revision 1.14  2007/05/31 07:05:22  nakata
  DWC_RNK_GET_MODE_NEAR_HIとDWC_RNK_GET_MODE_NEAR_LOWを追加

  Revision 1.13  2007/04/25 11:43:45  hayashi
  adding Korean region support

  Revision 1.12  2007/04/18 01:57:43  hayashi
  userdata size was decreased to 764bytes

  Revision 1.11  2006/05/30 14:08:50  hayashi
  added DWC_RnkResGetTotal

  Revision 1.10  2006/05/22 07:29:58  hayashi
  changed to be set the server by DWC_SetAuthServer()

  Revision 1.9  2006/05/12 08:28:54  hayashi
  NOTINITIALIZED -> NOTREADY
  RIVAL -> NEAR
  DWCAccUserData -> DWCUserData
  BOOL	DWC_RnkShutdown() -> DWCRnkError

  Revision 1.8  2006/04/26 09:25:41  hayashi
  specification is changed related to PID handling

  Revision 1.7  2006/04/14 05:03:50  hayashi
  do indent

  Revision 1.6  2006/04/13 11:35:19  hayashi
  Change prefix Ranking->Rnk

  Revision 1.5  2006/04/13 09:01:38  hayashi
  character code converting

  Revision 1.4  2006/04/11 09:01:33  hayashi
  DWC_RnkInitializeで、接続先のサーバーを選択できるようにした

  Revision 1.3  2006/04/11 07:04:19  hayashi
  base64のエンコードサイズの計算が間違っていたので修正

  Revision 1.2  2006/04/10 13:26:11  hayashi
  sinceパラメーターの追加

  Revision 1.1  2006/04/10 13:06:11  hayashi
  added the ranking module


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_RNK_H_
#define DWC_RNK_H_


#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------
        define
   ------------------------------------------------------------------------- */

// ユーザー定義データに使用できる最大サイズ
#define DWC_RNK_DATA_MAX		764

// カテゴリーとして指定できる最大値
#define DWC_RNK_CATEGORY_MAX	1000

// 取得するランキングリストの昇順と降順を示す定数
#define DWC_RNK_ORDER_ASC		0	// 昇順
#define DWC_RNK_ORDER_DES		1	// 降順

// 取得するランキングリストの最大数の上限値
#define DWC_RNK_GET_MAX         30

// 友達の最大数
#define DWC_RNK_FRIENDS_MAX		64


/* -------------------------------------------------------------------------
        enum
   ------------------------------------------------------------------------- */

// エラーコード
//
typedef enum{

	DWC_RNK_SUCCESS = 0,						// 成功
	DWC_RNK_IN_ERROR,							// エラー発生中

	DWC_RNK_ERROR_INVALID_PARAMETER,			// 不正なパラメーター

	// 
	// DWC_RnkInitialize のエラー値
	// 
	DWC_RNK_ERROR_INIT_ALREADYINITIALIZED,		// 初期化済
	DWC_RNK_ERROR_INIT_INVALID_INITDATASIZE,	// 不正な初期化データサイズ
	DWC_RNK_ERROR_INIT_INVALID_INITDATA,		// 不正な初期化データ
	DWC_RNK_ERROR_INIT_INVALID_USERDATA,		// ユーザー情報が不正

	//
	// DWC_RnkPutScoreAsync のエラー値
	//
	DWC_RNK_ERROR_PUT_NOTREADY,					// 初期化されていない
	DWC_RNK_ERROR_PUT_INVALID_KEY,				// 暗号化キーが不正
	DWC_RNK_ERROR_PUT_NOMEMORY,					// メモリー不足
 	
	//
	// DWC_RnkGetScoreAsync のエラー値
	//
	DWC_RNK_ERROR_GET_NOTREADY,					// 初期化されていない
	DWC_RNK_ERROR_GET_INVALID_KEY,				// 暗号化キーが不正
	DWC_RNK_ERROR_GET_NOMEMORY,					// メモリー不足
 
	//
	// DWC_RnkCancelProcess のエラー値
	//
	DWC_RNK_ERROR_CANCEL_NOTASK,				// 非同期処理中の処理なし

	//
	// DWC_RnkProcess のエラー値
	//
	DWC_RNK_PROCESS_NOTASK,				// 非同期処理中の処理なし

	//
	// DWC_RnkGetResponse のエラー値
	//
	DWC_RNK_ERROR_INVALID_MODE,					// 不正なモード
	DWC_RNK_ERROR_NOTCOMPLETED,					// 通信未完了
	DWC_RNK_ERROR_EMPTY_RESPONSE				// 空のレスポンス


} DWCRnkError;


// モジュールステータス
//
typedef enum{

	DWC_RNK_STATE_NOTREADY	= 0,	// 未初期化状態

	DWC_RNK_STATE_INITIALIZED,		// 初期化完了
	DWC_RNK_STATE_PUT_ASYNC,		// PUT非同期処理中
	DWC_RNK_STATE_GET_ASYNC,		// GET非同期処理中
	DWC_RNK_STATE_COMPLETED,		// 完了

	DWC_RNK_STATE_ERROR				// エラー発生

} DWCRnkState;


// リージョンコード
//
typedef enum{

	DWC_RNK_REGION_JP	= 0x01,	// Japan
	DWC_RNK_REGION_US	= 0x02,	// United States
	DWC_RNK_REGION_EU	= 0x04,	// Europe
	DWC_RNK_REGION_KR	= 0x08,	// Korea

	DWC_RNK_REGION_ALL	= 0xff	// World

} DWCRnkRegion;


// DWC_RnkGetScoreAsync 取得モード
//
typedef enum{

	DWC_RNK_GET_MODE_ORDER,		// 順位
	DWC_RNK_GET_MODE_TOPLIST,	// ハイスコア
	DWC_RNK_GET_MODE_NEAR,		// 周辺スコア
	DWC_RNK_GET_MODE_FRIENDS,	// 友達指定ランキング
	DWC_RNK_GET_MODE_NEAR_HI,
	DWC_RNK_GET_MODE_NEAR_LOW

} DWCRnkGetMode;


/* -------------------------------------------------------------------------
        typedef - function
   ------------------------------------------------------------------------- */


/* -------------------------------------------------------------------------
        struct
   ------------------------------------------------------------------------- */

// スコア情報格納用構造体
//
typedef struct{

	u32					order;		// 順位
	s32					pid;		// GameSpy Profile ID
	s32					score;		// スコア
	DWCRnkRegion		region;		// リージョンコード
	u32					lastupdate;	// 前回の更新からの経過時間(分)

	u32					size;		// userdataのサイズ(bytes)
	void*				userdata;	// ユーザー定義データのバッファ

} DWCRnkData;


// DWC_RnkGetScoreAsyncで指定するパラメータ
//
typedef struct{

	u32				size;	// サイズ(bytes)

	union{

		//
		// モード = DWC_RNK_GET_MODE_ORDER
		//
		struct{

			u32	sort;							// DWC_RNK_ORDER_ASC:昇順
												// DWC_RNK_ORDER_DES:降順
			u32	since;							// 何秒前からか

		}			order;

		//
		// モード = DWC_RNK_GET_MODE_TOPLIST
		//
		struct{

			u32	sort;							// DWC_RNK_ORDER_ASC:昇順
												// DWC_RNK_ORDER_DES:降順
			u32	limit;							// 取得する最大数
			u32	since;							// 何秒前からか

		}			toplist;

		//
		// モード = DWC_RNK_GET_MODE_NEAR
		//
		struct{

			u32	sort;							// DWC_RNK_ORDER_ASC:昇順
												// DWC_RNK_ORDER_DES:降順
			u32	limit;							// 取得する最大数
			u32	since;							// 何秒前からか

		}			near;

		//
		// モード = DWC_RNK_GET_MODE_FRIENDS
		//
		struct{


			u32	sort;							// DWC_RNK_ORDER_ASC:昇順
												// DWC_RNK_ORDER_DES:降順
			u32	limit;							// 取得する最大数
			u32	since;							// 何秒前からか
			s32	friends[DWC_RNK_FRIENDS_MAX];	// 友達のPIDリスト

		}			friends;
	
		u32			data;

	};

} DWCRnkGetParam;


/* -------------------------------------------------------------------------
        function - external
   ------------------------------------------------------------------------- */

extern DWCRnkError	DWC_RnkInitialize(		const char* initdata,
											const DWCUserData* userdata);

extern DWCRnkError	DWC_RnkShutdown();

extern DWCRnkError	DWC_RnkPutScoreAsync(	u32 category,
											DWCRnkRegion region,
											s32 score,
											void* data,
											u32 size );

extern DWCRnkError	DWC_RnkGetScoreAsync(	DWCRnkGetMode mode,
											u32 category,
											DWCRnkRegion region,
											DWCRnkGetParam* param );

extern DWCRnkError	DWC_RnkCancelProcess();

extern DWCRnkError	DWC_RnkProcess();

extern DWCRnkState	DWC_RnkGetState();

//
// レスポンス取得用
//
extern DWCRnkError	DWC_RnkResGetRow(		DWCRnkData* out,
											u32 index );

extern DWCRnkError	DWC_RnkResGetRowCount(	u32* count );

extern DWCRnkError	DWC_RnkResGetOrder(		u32* order );

extern DWCRnkError	DWC_RnkResGetTotal(		u32* total );


#ifdef __cplusplus
}
#endif

#endif // DWC_RNK_H_
