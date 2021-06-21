/*---------------------------------------------------------------------------*
  Project:  NitroDWC -  - 
  File:     dwc_ranking.c

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_ranking.c,v $
  Revision 1.17  2007/08/23 23:32:35  takayama
  DWC_2_2_RC1 の時点にロールバックした。

  Revision 1.14  2007/05/31 07:05:32  nakata
  DWC_RNK_GET_MODE_NEAR_HIとDWC_RNK_GET_MODE_NEAR_LOWを追加

  Revision 1.13  2006/06/21 01:04:22  hayashi
  changed to accept DWC_RnkPutScoreAsync with no user defined data.

  Revision 1.12  2006/06/16 04:51:58  hayashi
  fixed the wrong comment

  Revision 1.11  2006/05/30 14:08:50  hayashi
  added DWC_RnkResGetTotal

  Revision 1.10  2006/05/26 02:07:44  hayashi
  change the limit of number at getting frined ranking

  Revision 1.9  2006/05/23 12:49:31  hayashi
  exclude dwc_serverurl_inetrnal.h including

  Revision 1.8  2006/05/22 07:29:58  hayashi
  changed to be set the server by DWC_SetAuthServer()

  Revision 1.7  2006/05/12 08:28:54  hayashi
  NOTINITIALIZED -> NOTREADY
  RIVAL -> NEAR
  DWCAccUserData -> DWCUserData
  BOOL	DWC_RnkShutdown() -> DWCRnkError

  Revision 1.6  2006/04/26 09:25:40  hayashi
  specification is changed related to PID handling

  Revision 1.5  2006/04/24 10:12:12  hayashi
  change the limit value of parameter checking  ==0 to <=1

  Revision 1.4  2006/04/13 11:35:19  hayashi
  Change prefix Ranking->Rnk

  Revision 1.3  2006/04/13 09:01:38  hayashi
  character code converting

  Revision 1.2  2006/04/11 09:01:33  hayashi
  DWC_RnkInitializeで、接続先のサーバーを選択できるようにした

  Revision 1.1  2006/04/10 10:16:58  hayashi
  added the ranking module


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>

#include <string.h>
#include <stdlib.h>

#include <base/dwc_account.h>
#include <base/dwc_ranking.h>
#include <base/dwc_error.h>
#include <base/dwc_memfunc.h>

#include <auth/dwc_http.h>

#include <ranking/dwc_ranksession.h>

/* -------------------------------------------------------------------------
        pre-processing switch
   ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------
        define
   ------------------------------------------------------------------------- */

//
// これはdwc_serverurl_inetrnal.hでも定義されているが構造上簡単のため直書き
//
#define DWC_AUTH_NAS_URL		"https://nas.nintendowifi.net/ac"

/* -------------------------------------------------------------------------
        macro
   ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------
        struct
   ------------------------------------------------------------------------- */

// DWC_RnkInitializeに渡される初期化文字列をパースするための構造体
typedef union{

	char	data[84];

	struct{
		char	secretkey[20];	// 秘密鍵
		char	randkey_1[8];	// 内部暗号鍵１
		char	randkey_2[8];	// 内部暗号鍵２
		char	randkey_3[8];	// 内部暗号鍵３
		char	randkey_4[8];	// 内部暗号鍵４
		char	gamename[32];	// ゲーム名
	}info;

}	DWCiRankingInitData;

// 受信データを受け取るための構造体
//
typedef struct{

	u32					size;	// サイズ(bytes)
	DWCRnkGetMode		mode;	// モード

	union{

		void*				header;

		struct{								// ■ ランキングリスト取得時のヘッダ
											//
			u32	count;						//   取得した行数
											//
			u32	total;						//   総数
											//
			DWCRnkData	rankingdata;		//   ランキングデータの先頭ポインタ

		}				*listheader;

		struct{								// ■ 順位取得時のヘッダ
											//
			u32	order;						//   順位
											//
			u32	total;						//   総数

		}				*orderheader;

	};

} DWCiRankingResponse;


// 接続先のサーバーを取得するために認証サーバーのアドレスを参照
//
extern DWCHttpParam DWCauthhttpparam;

/* -------------------------------------------------------------------------
        variable
   ------------------------------------------------------------------------- */

// ランキングモジュールの状態管理構造体
struct {

	DWCRnkState	state;		// ステータス

	s32			pid;		// GameSpy Profile ID

}	g_rankinginfo	=		// 初期データ
						{
							DWC_RNK_STATE_NOTREADY,	// ステータス
						};


/* -------------------------------------------------------------------------
        prototype
   ------------------------------------------------------------------------- */

DWCRnkError	DWCi_RankingGetResponse( DWCiRankingResponse* out );


/* -------------------------------------------------------------------------
        function - internal
   ------------------------------------------------------------------------- */

/*---------------------------------------------------------------------------*
  Name:		DWCi_RankingGetResponse

  Description:	通信で受信したデータを取得します。
				取得したバッファは、DWC_RnkShutdown()の呼び出しで解放され
				るまで有効です。

  Arguments:	out	結果を格納するDWCiRankingResponse構造体のポインタ

  Returns:		DWC_RNK_SUCCESS					成功
  				DWC_RNK_IN_ERROR				エラー発生中
  				DWC_RNK_ERROR_NOTCOMPLETED		通信未完了
				DWC_RNK_ERROR_EMPTY_RESPONSE	空のレスポンス
 *---------------------------------------------------------------------------*/

DWCRnkError	DWCi_RankingGetResponse( DWCiRankingResponse* out ){

	void* buf;
	u32 size;

	// DWCエラーが発生している場合ははじく
	if (DWCi_IsError()) return DWC_RNK_IN_ERROR;

	// ステータスチェック
	//
	if( g_rankinginfo.state != DWC_RNK_STATE_COMPLETED ){
	
		// 通信が完了していない
		return DWC_RNK_ERROR_NOTCOMPLETED;
	
	}

	// レスポンスデータ取得
	// 
	buf = DWCi_RankingSessionGetResponse( &size );

	if( size == 0 ){
	
		// レスポンスデータが空
		return DWC_RNK_ERROR_EMPTY_RESPONSE;
	
	}

	// 返り値にセットする
	//
	out->size = size;
	out->mode = (DWCRnkGetMode)((u32*)buf)[0];
	out->header = &((u32*)buf)[1];

	return DWC_RNK_SUCCESS;

}


/* -------------------------------------------------------------------------
        function - external
   ------------------------------------------------------------------------- */

/*---------------------------------------------------------------------------*
  Name:		DWC_RnkInitialize

  Description:	ランキングモジュールを初期化します
  				既に初期化済みの場合や初期化データが不正な場合は失敗します
  				通信用秘密鍵文字列はSDK運用者により発行されるもので、サーバー
  				に対してユニークです。

  Arguments:	initdata		通信用秘密鍵文字列
  				userdata		ユーザーデータ

  Returns:
	DWC_RNK_SUCCESS							成功
	DWC_RNK_ERROR_INIT_ALREADYINITIALIZED	初期化済
	DWC_RNK_ERROR_INIT_INVALID_INITDATASIZE	不正な初期化データサイズ
	DWC_RNK_ERROR_INIT_INVALID_INITDATA		不正な初期化データ
	DWC_RNK_ERROR_INIT_INVALID_USERDATA		不正なユーザーデータ

 *---------------------------------------------------------------------------*/

DWCRnkError	DWC_RnkInitialize(	const char* initdata,
								const DWCUserData* userdata ){

	u32 randkey_1,randkey_2,randkey_3,randkey_4;
	DWCiRankingInitData* ptr;
	char secretkey[21];
	char buf[9] = "";

	// DWCエラーが発生している場合ははじく
	if (DWCi_IsError()) return DWC_RNK_IN_ERROR;

	// ステータスチェック
	if( g_rankinginfo.state != DWC_RNK_STATE_NOTREADY ){
	
		// 初期化済み
		return DWC_RNK_ERROR_INIT_ALREADYINITIALIZED;
	
	}

	// ユーザーデータの正当性を検証
	if( !DWC_CheckUserData( userdata )
		|| !DWC_CheckHasProfile( userdata )){

		// 不正なユーザーデータ
		return DWC_RNK_ERROR_INIT_INVALID_USERDATA;

	}

	// 初期化データチェック
	if( strlen( initdata ) >= sizeof( DWCiRankingInitData ) ){

		// サイズが不正
		return DWC_RNK_ERROR_INIT_INVALID_INITDATASIZE;

	}

	// ポインタにキャスト
	ptr = (DWCiRankingInitData*)initdata;

	// 秘密鍵の取得
	strncpy( secretkey, ptr->info.secretkey, 20 );
	secretkey[20] = '\0';

	// 内部暗号鍵の復元
	randkey_1 = strtoul( strncpy( buf, ptr->info.randkey_1, 8 ), NULL, 16 );
	randkey_2 = strtoul( strncpy( buf, ptr->info.randkey_2, 8 ), NULL, 16 );
	randkey_3 = strtoul( strncpy( buf, ptr->info.randkey_3, 8 ), NULL, 16 );
	randkey_4 = strtoul( strncpy( buf, ptr->info.randkey_4, 8 ), NULL, 16 );

	// 内部暗号鍵の正当性を検証
	if( !DWCi_RankingValidateKey(	ptr->info.gamename,
									secretkey,
									randkey_1,
									randkey_2,
									randkey_3,
									randkey_4) ){
	
		// 不正なキー
		return DWC_RNK_ERROR_INIT_INVALID_INITDATA;
	
	}

	// GameSpyProfileIDを保持する
	g_rankinginfo.pid = userdata->gs_profile_id;

	// 接続先のサーバーを選択する
	if( strcmp( DWCauthhttpparam.url, DWC_AUTH_NAS_URL ) == 0 ){
	
		// HTTPモジュールを初期化(本番サーバー)
		DWCi_RankingSessionInitialize( TRUE );

	}else{
	
		// HTTPモジュールを初期化(開発サーバー)
		DWCi_RankingSessionInitialize( FALSE );

	}

	// ステータスをセット
	g_rankinginfo.state = DWC_RNK_STATE_INITIALIZED;


	return DWC_RNK_SUCCESS;

}


/*---------------------------------------------------------------------------*
  Name:		DWC_RnkShutdown

  Description:	ランキングモジュールで使用したリソースを開放します。
				受信バッファが開放されるため、DWCi_RankingGetResponse()で取得
				したバッファは無効になります。

  Arguments:	なし

  Returns:		DWC_RNK_SUCCESS	成功
 *---------------------------------------------------------------------------*/

DWCRnkError	DWC_RnkShutdown(){

	// HTTPモジュールの開放
	DWCi_RankingSessionShutdown();

	// ステータスをセット
	g_rankinginfo.state = DWC_RNK_STATE_NOTREADY;

	return DWC_RNK_SUCCESS;

}


/*---------------------------------------------------------------------------*
  Name:		DWC_RnkPutScoreAsync

  Description:	スコアを登録する非同期処理を開始します。
  				

  Arguments:	category	カテゴリー
				region		リージョンコード
  				score		スコア
				data		ユーザー定義データのバッファへのポインタ
				size		dataのサイズ(DWC_RNK_DATA_MAX以下)

  Returns:		DWC_RNK_SUCCESS						成功
  				DWC_RNK_IN_ERROR					エラー発生中
  				DWC_RNK_ERROR_PUT_NOTREADY			未初期化
				DWC_RNK_ERROR_INVALID_PARAMETER		不正なパラメーター
				DWC_RNK_ERROR_PUT_INVALID_KEY		暗号化キーが不正
				DWC_RNK_ERROR_PUT_NOMEMORY			メモリー不足
 *---------------------------------------------------------------------------*/
DWCRnkError	DWC_RnkPutScoreAsync(	u32 category,
									DWCRnkRegion region,
									s32 score,
									void* data,
									u32 size ){

	DWCiRankingSessionResult res;

	// DWCエラーが発生している場合ははじく
	if (DWCi_IsError()) return DWC_RNK_IN_ERROR;

	// ステータスチェック
	//
	if( ( g_rankinginfo.state != DWC_RNK_STATE_INITIALIZED ) 
		&& (g_rankinginfo.state != DWC_RNK_STATE_COMPLETED ) ){
	
		// 初期化されていない
		return DWC_RNK_ERROR_PUT_NOTREADY;
	
	}

	// パラメーターチェック
	//
	if( (category > DWC_RNK_CATEGORY_MAX)
		|| (size > DWC_RNK_DATA_MAX)){
	
		// 不正なパラメーター
		return DWC_RNK_ERROR_INVALID_PARAMETER;
	
	}

	if( (data == NULL) && (size != 0) ){
	
		// 不正なパラメーター
		//
		// ユーザー定義データのポインタがNULLの場合はサイズも0である必要がある
		//
		return DWC_RNK_ERROR_INVALID_PARAMETER;

	}

	// リクエスト発行
	//
	res = DWCi_RankingSessionPutAsync(	category,
										g_rankinginfo.pid,
										region,
										score,
										data,
										size );

	// エラー処理
	switch( res ){

	case DWCi_RANKING_SESSION_ERROR_INVALID_KEY:
		return DWC_RNK_ERROR_PUT_INVALID_KEY;

	case DWCi_RANKING_SESSION_ERROR_NOMEMORY:
		return DWC_RNK_ERROR_PUT_NOMEMORY;

	}

	// ステータスをセット
	g_rankinginfo.state = DWC_RNK_STATE_PUT_ASYNC;

	return DWC_RNK_SUCCESS;

}


/*---------------------------------------------------------------------------*
  Name:		DWC_RnkGetScoreAsync

  Description:	ランキングデータを取得する非同期処理を開始します。

  Arguments:	mode		取得モード
  				category	カテゴリー
				region		リージョンコード（ORで複数指定可能）
				param		パラメーター


  Returns:		DWC_RNK_SUCCESS						成功
  				DWC_RNK_IN_ERROR					エラー発生中
  				DWC_RNK_ERROR_GET_NOTREADY			未初期化
				DWC_RNK_ERROR_INVALID_PARAMETER		不正なパラメーター
				DWC_RNK_ERROR_GET_INVALID_KEY		暗号化キーが不正
				DWC_RNK_ERROR_GET_NOMEMORY			メモリー不足
  *---------------------------------------------------------------------------*/

DWCRnkError	DWC_RnkGetScoreAsync(	DWCRnkGetMode mode,
									u32 category,
									DWCRnkRegion region,
									DWCRnkGetParam* param ){

	DWCiRankingSessionResult res;

	// DWCエラーが発生している場合ははじく
	if (DWCi_IsError()) return DWC_RNK_IN_ERROR;

	// ステータスチェック
	//
	if( ( g_rankinginfo.state != DWC_RNK_STATE_INITIALIZED ) 
		&& (g_rankinginfo.state != DWC_RNK_STATE_COMPLETED ) ){
	
		// 初期化されていない
		return DWC_RNK_ERROR_GET_NOTREADY;
	
	}

	// パラメーターチェック
	//
	if( (category > DWC_RNK_CATEGORY_MAX)
		|| (param == NULL) ){
	
		// 不正なパラメーター
		return DWC_RNK_ERROR_INVALID_PARAMETER;
	
	}

	switch( mode ){

	// 順位
	//
	case DWC_RNK_GET_MODE_ORDER:

		// サイズチェック
		if( param->size != sizeof(param->order) ){
			// 不正なパラメーター
			return DWC_RNK_ERROR_INVALID_PARAMETER;
		}

		// パラメーターチェック
		switch( param->order.sort ){

		case DWC_RNK_ORDER_ASC:	// 昇順
		case DWC_RNK_ORDER_DES:	// 降順
			// 正常
			break;

		default:
			// 不正なパラメーター
			return DWC_RNK_ERROR_INVALID_PARAMETER;
		}

		break;

	// ハイスコア
	//
	case DWC_RNK_GET_MODE_TOPLIST:

		// サイズチェック
		if( param->size != sizeof(param->toplist) ){
			// 不正なパラメーター
			return DWC_RNK_ERROR_INVALID_PARAMETER;
		}

		// パラメーターチェック
		switch( param->toplist.sort ){

		case DWC_RNK_ORDER_ASC:	// 昇順
		case DWC_RNK_ORDER_DES:	// 降順
			// 正常
			break;

		default:
			// 不正なパラメーター
			return DWC_RNK_ERROR_INVALID_PARAMETER;
		}

		if( param->toplist.limit > DWC_RNK_GET_MAX ||
			param->toplist.limit == 0 ){
			// 不正なパラメーター
			return DWC_RNK_ERROR_INVALID_PARAMETER;
		}

		break;

	case DWC_RNK_GET_MODE_NEAR:	// 周辺スコア
	case DWC_RNK_GET_MODE_NEAR_HI:
	case DWC_RNK_GET_MODE_NEAR_LOW:

		// サイズチェック
		if( param->size != sizeof(param->near) ){
			// 不正なパラメーター
			return DWC_RNK_ERROR_INVALID_PARAMETER;
		}

		// パラメーターチェック
		switch( param->near.sort ){

		case DWC_RNK_ORDER_ASC:	// 昇順
		case DWC_RNK_ORDER_DES:	// 降順
			// 正常
			break;

		default:
			// 不正なパラメーター
			return DWC_RNK_ERROR_INVALID_PARAMETER;
		}

		if( param->near.limit > DWC_RNK_GET_MAX ||
			param->near.limit <= 1 ){
			// 不正なパラメーター
			return DWC_RNK_ERROR_INVALID_PARAMETER;
		}

		break;

	case DWC_RNK_GET_MODE_FRIENDS:	// 友達指定ランキング


		// サイズチェック
		if( param->size != sizeof(param->friends) ){
			// 不正なパラメーター
			return DWC_RNK_ERROR_INVALID_PARAMETER;
		}

		// パラメーターチェック
		switch( param->friends.sort ){

		case DWC_RNK_ORDER_ASC:	// 昇順
		case DWC_RNK_ORDER_DES:	// 降順
			// 正常
			break;

		default:
			// 不正なパラメーター
			return DWC_RNK_ERROR_INVALID_PARAMETER;
		}

		if( param->friends.limit > (DWC_RNK_FRIENDS_MAX+1) ||
			param->friends.limit <= 1 ){
			// 不正なパラメーター
			return DWC_RNK_ERROR_INVALID_PARAMETER;
		}

		break;

	}

	// リクエスト発行
	//
	res = DWCi_RankingSessionGetAsync(	mode,
										category,
										g_rankinginfo.pid,
										region,
										param );

	// エラー処理
	switch( res ){

	case DWCi_RANKING_SESSION_ERROR_INVALID_KEY:
		return DWC_RNK_ERROR_GET_INVALID_KEY;

	case DWCi_RANKING_SESSION_ERROR_NOMEMORY:
		return DWC_RNK_ERROR_GET_NOMEMORY;

	}

	// ステータスをセット
	g_rankinginfo.state = DWC_RNK_STATE_GET_ASYNC;

	return DWC_RNK_SUCCESS;

}


/*---------------------------------------------------------------------------*
  Name:		DWC_RnkCancelProcess

  Description:	非同期処理を中断します。
  				中断するとDWC_RNK_STATE_ERRORに遷移するため、再通信を行う
  				場合には終了、初期化処理を経る必要があります。

  Arguments:	なし

  Returns:		DWC_RNK_SUCCESS					成功
  				DWC_RNK_IN_ERROR				エラー発生中
  				DWC_RNK_ERROR_CANCEL_NOTASK		非同期タスクがない
 *---------------------------------------------------------------------------*/

DWCRnkError	DWC_RnkCancelProcess(){

	// DWCエラーが発生している場合ははじく
	if (DWCi_IsError())return DWC_RNK_IN_ERROR;

	// ステータスチェック
	//
	if( (g_rankinginfo.state != DWC_RNK_STATE_PUT_ASYNC) &&
		(g_rankinginfo.state != DWC_RNK_STATE_GET_ASYNC) ){
	
		// 非同期処理中のタスクがない
		return DWC_RNK_ERROR_CANCEL_NOTASK;
	
	}

	// キャンセルする
	//
	DWCi_RankingSessionCancel();

	// ステータスをセット
	//
	g_rankinginfo.state = DWC_RNK_STATE_ERROR;


	return DWC_RNK_SUCCESS;

}


/*---------------------------------------------------------------------------*
  Name:		DWC_RnkProcess

  Description:	非同期処理を更新します

  Arguments:	なし

  Returns:		DWC_RNK_SUCCESS					成功
  				DWC_RNK_IN_ERROR				エラー発生中
				DWC_RNK_PROCESS_NOTASK	非同期タスクがない
 *---------------------------------------------------------------------------*/

DWCRnkError DWC_RnkProcess(){

	// DWCエラーが発生している場合ははじく
	if (DWCi_IsError()) return DWC_RNK_IN_ERROR;

	// ステータスチェック
	//
	if( (g_rankinginfo.state != DWC_RNK_STATE_PUT_ASYNC) &&
		(g_rankinginfo.state != DWC_RNK_STATE_GET_ASYNC) ){
	
		// 非同期処理中のタスクがない
		return DWC_RNK_PROCESS_NOTASK;
	
	}

	// 処理を進める
	//
	switch( DWCi_RankingSessionProcess() ){

	case DWCi_RANKING_SESSION_STATE_ERROR:

		// エラー発生
		g_rankinginfo.state = DWC_RNK_STATE_ERROR;

		break;
	
	case DWCi_RANKING_SESSION_STATE_CANCELED:
		
		//!< キャンセル
		g_rankinginfo.state = DWC_RNK_STATE_ERROR;

		break;
	
	case DWCi_RANKING_SESSION_STATE_INITIAL:
		
		//!< 初期状態
		break;

	case DWCi_RANKING_SESSION_STATE_REQUEST:		//!< リクエスト発行
	case DWCi_RANKING_SESSION_STATE_GETTING_TOKEN:	//!< トークン取得中
	case DWCi_RANKING_SESSION_STATE_GOT_TOKEN:		//!< トークン取得完了
	case DWCi_RANKING_SESSION_STATE_SENDING_DATA:	//!< データ送信中

		// 処理中
		break;

	case DWCi_RANKING_SESSION_STATE_COMPLETED:

		// 完了
		g_rankinginfo.state = DWC_RNK_STATE_COMPLETED;

		break;
	
	}


	return DWC_RNK_SUCCESS;

}


/*---------------------------------------------------------------------------*
  Name:		DWC_RnkGetState

  Description:	現在の処理状態を取得します

  Arguments:	なし

  Returns:		DWCRnkState
 *---------------------------------------------------------------------------*/

DWCRnkState	DWC_RnkGetState(){

	return g_rankinginfo.state;

}


/*---------------------------------------------------------------------------*
  Name:		DWC_RnkResGetRow

  Description:	通信で受信したランキングリストから任意の行を抽出して取得します

  Arguments:	out		結果を格納するDWCRnkData構造体のポインタ
				index	取得するリストのインデックス

  Returns:		DWC_RNK_SUCCESS					成功
  				DWC_RNK_IN_ERROR				エラー発生中
				DWC_RNK_ERROR_INVALID_MODE		不正なモード
  				DWC_RNK_ERROR_INVALID_PARAMETER	不正なパラメーター
  				DWC_RNK_ERROR_NOTCOMPLETED		通信未完了
				DWC_RNK_ERROR_EMPTY_RESPONSE	空のレスポンス
 *---------------------------------------------------------------------------*/

DWCRnkError	DWC_RnkResGetRow(	DWCRnkData* out,
										u32 index ){

	DWCRnkData* ptr;
	DWCiRankingResponse res;
	DWCRnkError err;

	// DWCエラーが発生している場合ははじく
	if (DWCi_IsError()) return DWC_RNK_IN_ERROR;

	// レスポンスのポインタを得る
	// 
	err = DWCi_RankingGetResponse( &res );
	if( err != DWC_RNK_SUCCESS )
		return err;

	// レスポンスが合致するかをチェック
	// 
	switch( res.mode ){

	case DWC_RNK_GET_MODE_ORDER:

		// 順位取得リクエストではランキングリストはないのでエラー
		return DWC_RNK_ERROR_INVALID_MODE;

	case DWC_RNK_GET_MODE_TOPLIST:
	case DWC_RNK_GET_MODE_NEAR:
	case DWC_RNK_GET_MODE_FRIENDS:
	case DWC_RNK_GET_MODE_NEAR_HI:
	case DWC_RNK_GET_MODE_NEAR_LOW:

		break;

	}

	// 不正パラメーターチェック
	//
	if( out == NULL )
		return DWC_RNK_ERROR_INVALID_PARAMETER;

	if( index >= res.listheader->count )
		return DWC_RNK_ERROR_INVALID_PARAMETER;

	// シークする
	//
	ptr = &res.listheader->rankingdata;

	while( index-- > 0 ){

		ptr = (DWCRnkData*)( ((u8*)&ptr->userdata) + ptr->size );

	}

	// メモリーアクセスオーバーの検知
	// 
	if( (u32)&ptr->userdata + ptr->size > (u32)res.header + res.size ){

		return DWC_RNK_ERROR_INVALID_PARAMETER;

	}

	// 返り値のセット
	*out = *ptr;
	out->userdata = &ptr->userdata;


	return DWC_RNK_SUCCESS;

}


/*---------------------------------------------------------------------------*
  Name:		DWC_RnkResGetRowCount

  Description:	通信で受信したランキングリストから任意の行を抽出して取得します

  Arguments:	count	行数を取得する変数のポインタ

  Returns:		DWC_RNK_SUCCESS					成功
  				DWC_RNK_IN_ERROR				エラー発生中
				DWC_RNK_ERROR_INVALID_MODE		不正なモード
  				DWC_RNK_ERROR_INVALID_PARAMETER	不正なパラメーター
  				DWC_RNK_ERROR_NOTCOMPLETED		通信未完了
				DWC_RNK_ERROR_EMPTY_RESPONSE	空のレスポンス
 *---------------------------------------------------------------------------*/

DWCRnkError	DWC_RnkResGetRowCount( u32* count ){

	DWCiRankingResponse res;
	DWCRnkError err;

	// DWCエラーが発生している場合ははじく
	if (DWCi_IsError()) return DWC_RNK_IN_ERROR;

	// レスポンスのポインタを得る
	// 
	err = DWCi_RankingGetResponse( &res );
	if( err != DWC_RNK_SUCCESS )
		return err;

	// 不正パラメーターチェック
	//
	if( count == NULL )
		return DWC_RNK_ERROR_INVALID_PARAMETER;

	// レスポンスが合致するかをチェック
	// 
	switch( res.mode ){

	case DWC_RNK_GET_MODE_ORDER:

		// 順位取得リクエストではランキングリストはないのでエラー
		return DWC_RNK_ERROR_INVALID_MODE;

	case DWC_RNK_GET_MODE_TOPLIST:
	case DWC_RNK_GET_MODE_NEAR:
	case DWC_RNK_GET_MODE_FRIENDS:
	case DWC_RNK_GET_MODE_NEAR_HI:
	case DWC_RNK_GET_MODE_NEAR_LOW:
	
		break;

	}

	// 返り値をセット
	//
	*count = res.listheader->count;


	return DWC_RNK_SUCCESS;

}


/*---------------------------------------------------------------------------*
  Name:		DWC_RnkResGetOrder

  Description:	順位を取得します

  Arguments:	order*	順位を取得する変数のポインタ

  Returns:		DWC_RNK_SUCCESS					成功
  				DWC_RNK_IN_ERROR				エラー発生中
				DWC_RNK_ERROR_INVALID_MODE		不正なモード
  				DWC_RNK_ERROR_INVALID_PARAMETER	不正なパラメーター
  				DWC_RNK_ERROR_NOTCOMPLETED		通信未完了
				DWC_RNK_ERROR_EMPTY_RESPONSE	空のレスポンス
 *---------------------------------------------------------------------------*/

DWCRnkError	DWC_RnkResGetOrder( u32* order ){

	DWCiRankingResponse res;
	DWCRnkError err;

	// DWCエラーが発生している場合ははじく
	if (DWCi_IsError()) return DWC_RNK_IN_ERROR;

	// レスポンスのポインタを得る
	// 
	err = DWCi_RankingGetResponse( &res );
	if( err != DWC_RNK_SUCCESS )
		return err;

	// 不正パラメーターチェック
	//
	if( order == NULL )
		return DWC_RNK_ERROR_INVALID_PARAMETER;

	// レスポンスが合致するかをチェック
	// 
	if( res.mode != DWC_RNK_GET_MODE_ORDER )
		return DWC_RNK_ERROR_INVALID_MODE;

	// 返り値をセット
	//
	*order = res.orderheader->order;


	return DWC_RNK_SUCCESS;

}


/*---------------------------------------------------------------------------*
  Name:		DWC_RnkResGetTotal

  Description:	総数を取得します

  Arguments:	total*	順位を取得する変数のポインタ

  Returns:		DWC_RNK_SUCCESS					成功
  				DWC_RNK_IN_ERROR				エラー発生中
  				DWC_RNK_ERROR_INVALID_PARAMETER	不正なパラメーター
  				DWC_RNK_ERROR_NOTCOMPLETED		通信未完了
				DWC_RNK_ERROR_EMPTY_RESPONSE	空のレスポンス
 *---------------------------------------------------------------------------*/

DWCRnkError	DWC_RnkResGetTotal( u32* total ){

	DWCiRankingResponse res;
	DWCRnkError err;

	// DWCエラーが発生している場合ははじく
	if (DWCi_IsError()) return DWC_RNK_IN_ERROR;

	// レスポンスのポインタを得る
	// 
	err = DWCi_RankingGetResponse( &res );
	if( err != DWC_RNK_SUCCESS )
		return err;

	// 不正パラメーターチェック
	//
	if( total == NULL )
		return DWC_RNK_ERROR_INVALID_PARAMETER;

	// 返り値をセット
	//
	*total = res.orderheader->total;


	return DWC_RNK_SUCCESS;

}

