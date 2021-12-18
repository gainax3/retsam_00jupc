/*===========================================================================*
  Project:  Pokemon Global Data Station Network Library
  File:     poke_net_common.h

  $NoKeywords: $
 *===========================================================================*/
/*! @file
	@brief	Pokemon GDS WiFi ライブラリ

*/
//===========================================================================
#ifndef ___POKE_NET_GDS_COMMON___
#define ___POKE_NET_GDS_COMMON___

//===============================================
//                時間パック
//===============================================
#define POKE_NET_TIMETOU64(Y,M,D,h,m,s)	(u64)((((u64)Y) << 40) | (((u64)M) << 32) | (((u64)D) << 24) | (((u64)h) << 16) | (((u64)m) << 8) | (((u64)s) << 0))

//===============================================
//!          ＧＤＳリクエストコード
//===============================================
enum POKE_NET_GDS_REQCODE {
	POKE_NET_GDS_REQCODE_DEBUG_START = 0 ,				// ※以下デバッグで使用するリクエスト

	// ------------------------- 以下デバッグリクエスト ----------------------
	POKE_NET_GDS_REQCODE_DEBUG_MESSAGE = 0 ,			//!< デバッグメッセージ

	POKE_NET_GDS_REQCODE_DEBUG_END   = 9999 ,			// ※9999まではデバッグで仕様するリクエスト
	POKE_NET_GDS_REQCODE_ADMIN_START = 10000 ,			// ※10000以降は管理者が使用できるリクエスト

#ifndef ___POKE_NET_BUILD_DS___
	// -------------------------- 以下管理者リクエスト -----------------------
	POKE_NET_GDS_REQCODE_ADMIN_SVR_STATUS = 10000 ,			// サーバーステータス取得
	POKE_NET_GDS_REQCODE_ADMIN_SVR_CLEARSTATUS ,			// サーバー情報クリア
	POKE_NET_GDS_REQCODE_ADMIN_SVR_COMMAND ,				// サーバーへの命令
	POKE_NET_GDS_REQCODE_ADMIN_SVR_DBINITIALIZE ,			// データベース初期化

	POKE_NET_GDS_REQCODE_ADMIN_DRESSUPSHOT_STATUS = 11000 ,	// ドレスアップショット状態取得
	POKE_NET_GDS_REQCODE_ADMIN_DRESSUPSHOT_DELETE ,			// ドレスアップショット削除
	POKE_NET_GDS_REQCODE_ADMIN_DRESSUPSHOT_CLEAR ,			// ドレスアップショットクリア

	POKE_NET_GDS_REQCODE_ADMIN_BOXSHOT_STATUS = 12000 ,		// ボックスショット状態取得
	POKE_NET_GDS_REQCODE_ADMIN_BOXSHOT_DELETE ,				// ボックスショット削除
	POKE_NET_GDS_REQCODE_ADMIN_BOXSHOT_CLEAR ,				// ボックスショットクリア

	POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_STATUS = 13000 ,	// バトルデータ状態取得
	POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_DELETE ,			// バトルデータ削除
	POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_RANKHISTORY ,		// バトルデータランキング履歴取得
	POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_EVENTFLAG ,		// バトルデータイベント登録要求
	POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_CLEAR ,			// バトルデータクリア
	POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_GETPARAM ,		// パラメータ取得
	POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_SETPARAM ,		// パラメータ設定
	POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_EXECDECISION ,	// 確定処理実行(強制)

	POKE_NET_GDS_REQCODE_ADMIN_RANKING_STATUS = 14000 ,		// ランキング状態取得
	POKE_NET_GDS_REQCODE_ADMIN_RANKING_HISTORY ,			// ランキング履歴取得
	POKE_NET_GDS_REQCODE_ADMIN_RANKING_GET ,				// ユーザーのランキング情報取得
	POKE_NET_GDS_REQCODE_ADMIN_RANKING_SET ,				// ユーザーのランキング情報設定
	POKE_NET_GDS_REQCODE_ADMIN_RANKING_CLEAR ,				// ランキング情報クリア

	POKE_NET_GDS_REQCODE_ADMIN_RANKING_GETSCHEDULE ,		// ランキングスケジュール取得
	POKE_NET_GDS_REQCODE_ADMIN_RANKING_SETSCHEDULE ,		// ランキングスケジュール設定
	POKE_NET_GDS_REQCODE_ADMIN_RANKING_EXECDECISION ,		// 確定処理実行(強制)

	POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_POKEMONTABLE_GET = 15000 ,	// 不正チェック:ポケモン毎の設定取得
	POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_POKEMONTABLE_SET ,			// 不正チェック:ポケモン毎の設定セット
	POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_NGWORD_GET ,				// 不正チェック:ＮＧワード取得
	POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_NGWORD_SET ,				// 不正チェック:ＮＧワードセット
	POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_LANGCODE_GET ,				// 不正チェック:言語コード取得
	POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_LANGCODE_SET ,				// 不正チェック:言語コードセット
	POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_ITEM_GET ,					// 不正チェック:どうぐ取得
	POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_ITEM_SET ,					// 不正チェック:どうぐセット
	POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_MEETPLACE_GET ,				// 不正チェック:出会った場所取得
	POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_MEETPLACE_SET ,				// 不正チェック:出会った場所セット
	POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_WAZATABLE_GET ,				// 不正チェック:技テーブル
	POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_WAZATABLE_SET ,				// 不正チェック:技テーブル
	POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_ACCESSORY_GET ,				// 不正チェック:ＮＧアクセサリ取得
	POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_ACCESSORY_SET ,				// 不正チェック:ＮＧアクセサリ設定
	POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_DRESSUPBGID_GET ,			// 不正チェック:ＮＧドレスアップショット背景ＩＤ
	POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_DRESSUPBGID_SET ,			// 不正チェック:ＮＧドレスアップショット背景ＩＤ
	POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_REGULATION_GET ,			// 不正チェック:有効なレギュレーション
	POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_REGULATION_SET ,			// 不正チェック:有効なレギュレーション

	POKE_NET_GDS_REQCODE_ADMIN_DHOCHECKLOG_GET ,						// 不正チェックログ:取得
	POKE_NET_GDS_REQCODE_ADMIN_DHOCHECKLOG_CLEAR ,						// 不正チェックログ:クリア

#endif
	POKE_NET_GDS_REQCODE_ADMIN_END  = 19999 ,			// ※19999までは管理者が使用できるリクエスト
	POKE_NET_GDS_REQCODE_USER_START = 20000 ,			// ※20000以降は一般ユーザーが使えるリクエスト

	// ------------------------- 以下ユーザーリクエスト ----------------------
	POKE_NET_GDS_REQCODE_DRESSUPSHOT_REGIST = 20000 ,	//!< ドレスアップショット登録
	POKE_NET_GDS_REQCODE_DRESSUPSHOT_GET ,				//!< ドレスアップショット取得
	POKE_NET_GDS_REQCODE_BOXSHOT_REGIST = 21000,		//!< ボックスショット登録
	POKE_NET_GDS_REQCODE_BOXSHOT_GET ,					//!< ボックスショット取得
	POKE_NET_GDS_REQCODE_RANKING_GETTYPE = 22000,		//!< 現在のランキングタイプ取得
	POKE_NET_GDS_REQCODE_RANKING_UPDATE ,				//!< 現在のランキング情報の更新と取得
	POKE_NET_GDS_REQCODE_BATTLEDATA_REGIST = 23000,		//!< バトルデータ登録
	POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH ,			//!< バトルデータ検索
	POKE_NET_GDS_REQCODE_BATTLEDATA_GET ,				//!< バトルデータ取得
	POKE_NET_GDS_REQCODE_BATTLEDATA_FAVORITE ,			//!< バトルデータお気に入り登録

	POKE_NET_GDS_REQCODE_LAST
};

#define	POKE_NET_GDS_REQUESTCOMMON_AUTH_SVLTOKEN_LENGTH		(304)
//===============================================
//! ポケモンWiFiライブラリ リクエスト認証情報
//===============================================
typedef struct {
	s32		PID;								//!< プロファイルＩＤ
	u8		ROMCode;							//!< バージョンコード
	u8		LangCode;							//!< 言語コード
	char	SvlToken[POKE_NET_GDS_REQUESTCOMMON_AUTH_SVLTOKEN_LENGTH];	// サービスロケータトークン
	u16		Dummy;
} POKE_NET_REQUESTCOMMON_AUTH;

//===============================================
//!  ポケモンWiFiライブラリ リクエストヘッダ
//===============================================
typedef struct {
	unsigned short ReqCode;						//!< リクエストコード(POKE_NET_GDS_REQUEST_REQCODE_xxxx)
	unsigned short Option;						//!< リクエストオプション
	POKE_NET_REQUESTCOMMON_AUTH Auth;			//!< ユーザー認証情報
	unsigned char Param[0];						//!< パラメータ(各リクエストの構造体)
} POKE_NET_REQUEST;

//===============================================
//!  ポケモンWiFiライブラリ レスポンスヘッダ
//===============================================
typedef struct {
	unsigned short ReqCode;						//!< リクエストコード(POKE_NET_GDS_REQUEST_REQCODE_xxxx)
	unsigned short Result;						//!< レスポンス結果(POKE_NET_GDS_RESPONSE_RESULT_xxxx)
	unsigned char Param[0];						//!< パラメータ(各レスポンスの構造体)
} POKE_NET_RESPONSE;

//===============================================
//       リクエスト/レスポンス構造体
//===============================================
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//      デバッグリクエスト/レスポンス
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//------------------------------------
//         デバッグメッセージ
//------------------------------------
//! ＧＤＳデバッグメッセージ結果
enum POKE_NET_GDS_RESPONSE_RESULT_DEBUG_MESSAGE {
	POKE_NET_GDS_RESPONSE_RESULT_DEBUG_MESSAGE_SUCCESS ,				//!< 登録成功
};

//! ＧＤＳデバッグメッセージリクエスト
typedef struct {
	char Message[1024];						//!< デバッグメッセージ
} POKE_NET_GDS_REQUEST_DEBUG_MESSAGE;

//! ＧＤＳデバッグメッセージレスポンス
typedef struct {
	char ResultMessage[1024];				//!< デバッグメッセージ
} POKE_NET_GDS_RESPONSE_DEBUG_MESSAGE;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//        ユーザーリクエスト/レスポンス
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//------------------------------------
//     ドレスアップショット登録
//------------------------------------
//! ＧＤＳドレスアップショット登録結果
enum POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_REGIST {
	POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_REGIST_SUCCESS ,				//!< 登録成功
	POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_REGIST_ERROR_AUTH ,			//!< ユーザー認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_REGIST_ERROR_ALREADY ,			//!< すでに登録されている
	POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_REGIST_ERROR_ILLEGAL ,			//!< 不正なデータ
	POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_REGIST_ERROR_ILLEGALPROFILE ,	//!< 不正なユーザープロフィール
	POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_REGIST_ERROR_UNKNOWN			//!< その他エラー
};

//! ＧＤＳドレスアップショット登録リクエスト
typedef struct {
	GT_GDS_DRESS_SEND Data;							//!< ドレスアップショット送信データ
} POKE_NET_GDS_REQUEST_DRESSUPSHOT_REGIST;

//! ＧＤＳドレスアップショット登録レスポンス
typedef struct {
	u64 Code;										//!< 登録されたドレスアップショットのコード
} POKE_NET_GDS_RESPONSE_DRESSUPSHOT_REGIST;

//------------------------------------
//     ドレスアップショット取得
//------------------------------------
//! ＧＤＳドレスアップショット取得結果
enum POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_GET {
	POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_GET_SUCCESS ,				//!< 取得成功
	POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_GET_ERROR_AUTH ,			//!< ユーザー認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_GET_ERROR_ILLEGALPOKEMON ,	//!< ポケモン番号エラー
	POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_GET_ERROR_UNKNOWN			//!< その他エラー
};

//! ＧＤＳドレスアップショット取得タイプ
enum POKE_NET_GDS_REQUEST_DRESSUPSHOT_GETTYPE {
	POKE_NET_GDS_REQUEST_DRESSUPSHOT_GETTYPE_POKEMON ,				//!< ポケモン番号で最新取得(SearchParamにポケモン番号)
	POKE_NET_GDS_REQUEST_DRESSUPSHOT_GETTYPE_CODE					//!< コード直接指定で取得
};

//! ＧＤＳドレスアップショット取得リクエスト
typedef struct {
	unsigned short SearchType;										//!< 検索タイプ(POKE_NET_GDS_REQUEST_DRESSUPSHOT_GETTYPE_xxxx)
	unsigned short SearchOpt;										//!< 検索オプション(現在未使用)
	u64 SearchParam;												//!< 検索パラメータ
} POKE_NET_GDS_REQUEST_DRESSUPSHOT_GET;

//! ＧＤＳドレスアップショット取得レスポンス内容
typedef struct {
	long PID;														//!< アップロード者のプロファイルＩＤ
	u64 Code;														//!< ドレスアップショットコード
	GT_GDS_DRESS_RECV Data;											//!< ドレスアップショット受信データ
} POKE_NET_GDS_RESPONSE_DRESSUPSHOT_RECVDATA;

#define	POKE_NET_GDS_RESPONSE_DRESSUPSHOT_GET_MAX	(10)			// クライアントが受け取るドレスアップショット最大数

//! ＧＤＳドレスアップショット取得レスポンス
typedef struct {
	long HitCount;													//!< ヒット件数
	POKE_NET_GDS_RESPONSE_DRESSUPSHOT_RECVDATA Data[0];				//!< ドレスアップショット受信データ
} POKE_NET_GDS_RESPONSE_DRESSUPSHOT_GET;

//------------------------------------
//        ボックスショット登録
//------------------------------------
//! ＧＤＳボックスショット登録結果
enum POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_REGIST {
	POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_REGIST_SUCCESS ,				//!< 登録成功
	POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_REGIST_ERROR_AUTH ,			//!< ユーザー認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_REGIST_ERROR_ALREADY ,			//!< すでに登録されている
	POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_REGIST_ERROR_ILLEGAL ,			//!< 不正なデータ
	POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_REGIST_ERROR_ILLEGALPROFILE ,	//!< 不正なユーザープロフィール
	POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_REGIST_ERROR_ILLEGALGROUP ,	//!< 不正なグループ番号
	POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_REGIST_ERROR_UNKNOWN			//!< その他エラー
};

//! ＧＤＳボックスショット登録リクエスト
typedef struct {
	long GroupNo;													//!< 登録先グループ番号
	GT_BOX_SHOT_SEND Data;											//!< ボックスショット送信データ
} POKE_NET_GDS_REQUEST_BOXSHOT_REGIST;

//! ＧＤＳボックスショット登録レスポンス
typedef struct {
	u64 Code;														//!< 登録されたボックスショットのコード
} POKE_NET_GDS_RESPONSE_BOXSHOT_REGIST;

//------------------------------------
//        ボックスショット取得
//------------------------------------
//! ＧＤＳボックスショット取得結果
enum POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_GET {
	POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_GET_SUCCESS ,				//!< 取得成功
	POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_GET_ERROR_AUTH ,			//!< ユーザー認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_GET_ERROR_ILLEGALGROUP ,	//!< グループ番号エラー
	POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_GET_ERROR_UNKNOWN			//!< その他エラー
};

//! ＧＤＳボックスショット取得タイプ
enum POKE_NET_GDS_REQUEST_BOXSHOT_GETTYPE {
	POKE_NET_GDS_REQUEST_BOXSHOT_GETTYPE_GROUP ,					//!< グループ番号で最新取得(SearchParamにグループ番号)
	POKE_NET_GDS_REQUEST_BOXSHOT_GETTYPE_CODE						//!< ＩＤ指定で取得(SearchParamにコード番号)
};

//! ＧＤＳボックスショット取得リクエスト
typedef struct {
	unsigned short SearchType;										//!< 検索タイプ(POKE_NET_GDS_REQUEST_BOXSHOT_GETTYPE_xxxx)
	unsigned short SearchOpt;										//!< 検索オプション(現在未使用)
	u64 SearchParam;												//!< 検索パラメータ
} POKE_NET_GDS_REQUEST_BOXSHOT_GET;

//! ＧＤＳボックスショット取得レスポンス内容
typedef struct {
	long PID;														//!< アップロード者のプロファイルＩＤ
	long GroupNo;													//!< グループ番号
	u64 Code;														//!< ボックスショットコード
	GT_BOX_SHOT_RECV Data;											//!< ボックスショット受信データ
} POKE_NET_GDS_RESPONSE_BOXSHOT_RECVDATA;

#define	POKE_NET_GDS_RESPONSE_BOXSHOT_GET_MAXNUM	(20)			// クライアントが受け取るボックスショット最大数

//! ＧＤＳボックスショット取得レスポンス
typedef struct {
	long HitCount;													//!< ヒット件数
	POKE_NET_GDS_RESPONSE_BOXSHOT_RECVDATA Data[0];					//!< ボックスショット受信データ
} POKE_NET_GDS_RESPONSE_BOXSHOT_GET;


//------------------------------------
//      ランキングタイプ取得
//------------------------------------
/*
//! ＧＤＳランキングタイプ取得リクエスト
typedef struct {
} POKE_NET_GDS_REQUEST_RANKING_GETTYPE;
*/

//! ＧＤＳランキングタイプ取得結果
enum POKE_NET_GDS_RESPONSE_RESULT_RANKING_GETTYPE {
	POKE_NET_GDS_RESPONSE_RESULT_RANKING_GETTYPE_SUCCESS ,				//!< 取得成功
	POKE_NET_GDS_RESPONSE_RESULT_RANKING_GETTYPE_ERROR_AUTH ,			//!< ユーザー認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_RANKING_GETTYPE_ERROR_UNKNOWN			//!< その他エラー
};

//! ＧＤＳランキング取得レスポンス内容
typedef struct {
	GT_RANKING_TYPE_RECV Data;					//!< ランキングタイプデータ
} POKE_NET_GDS_RESPONSE_RANKING_GETTYPE;

//------------------------------------
//        ランキング更新
//------------------------------------
//! ＧＤＳランキング更新結果
enum POKE_NET_GDS_RESPONSE_RESULT_RANKING_UPDATE {
	POKE_NET_GDS_RESPONSE_RESULT_RANKING_UPDATE_SUCCESS ,				//!< 更新成功
	POKE_NET_GDS_RESPONSE_RESULT_RANKING_UPDATE_ERROR_AUTH ,			//!< ユーザー認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_RANKING_UPDATE_ERROR_ILLEGALTYPE	 ,	//!< ランキングタイプエラー
	POKE_NET_GDS_RESPONSE_RESULT_RANKING_UPDATE_ERROR_ILLEGALDATA	 ,	//!< 不正なデータエラー
	POKE_NET_GDS_RESPONSE_RESULT_RANKING_UPDATE_ERROR_ILLEGALPROFILE ,	//!< 不正なプロフィール
	POKE_NET_GDS_RESPONSE_RESULT_RANKING_UPDATE_ERROR_UNKNOWN			//!< その他エラー
};

//! ＧＤＳランキング更新リクエスト
typedef struct {
	GT_RANKING_MYDATA_SEND Data;					//!< ランキング更新データ
} POKE_NET_GDS_REQUEST_RANKING_UPDATE;

//! ＧＤＳランキング更新レスポンス
typedef struct {
	GT_LAST_WEEK_RANKING_ALL_RECV LastWeek;			//!< 先週のランキングデータ
	GT_THIS_WEEK_RANKING_DATA_ALL_RECV ThisWeek;	//!< 今週のランキングデータ
} POKE_NET_GDS_RESPONSE_RANKING_UPDATE;

//------------------------------------
//       バトルデータ登録
//------------------------------------
//! ＧＤＳバトルデータ登録結果
enum POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST {
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_SUCCESS ,				//!< 登録成功
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_AUTH ,				//!< ユーザー認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ALREADY ,			//!< すでに登録されている
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ILLEGAL ,			//!< 不正なデータ
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ILLEGALPROFILE ,	//!< 不正なユーザープロフィール
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_UNKNOWN ,			//!< その他エラー
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ILLEGALPOKEMON_MINE ,	//!< 自分のポケモンでエラー
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ILLEGALPOKEMON_OTHER ,	//!< 相手のポケモンでエラ
};

//! ＧＤＳバトルデータ登録リクエスト
typedef struct {
	GT_BATTLE_REC_SEND Data;												//!< バトルデータ送信データ
} POKE_NET_GDS_REQUEST_BATTLEDATA_REGIST;

//! ＧＤＳバトルデータ登録レスポンス
typedef struct {
	u64 Code;																//!< 登録されたバトルデータのコード
} POKE_NET_GDS_RESPONSE_BATTLEDATA_REGIST;


//------------------------------------
//      バトルデータリスト取得
//------------------------------------
//! ＧＤＳバトルデータリスト取得結果
enum POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH {
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_SUCCESS ,			//!< 取得成功
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_ERROR_AUTH ,			//!< ユーザー認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_ERROR_ILLEGALPARAM ,	//!< 検索パラメータエラー
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_ERROR_UNKNOWN		//!< その他エラー
};

//! ＧＤＳバトルデータリスト取得タイプ
enum POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE {
	POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_CONDITION ,				//!< 条件指定で取得
	POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_RANKING ,				//!< 今週の通常ランキング
	POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_EXRANKING ,				//!< 今週の拡張ランキング
	POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_PID ,					// PIDによる取得(管理用)
};

//! ＧＤＳバトルデータリスト取得リクエスト
typedef struct {
	unsigned short SearchType;											//!< 検索タイプ(POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_xxxx)
	unsigned short SearchOpt;											//!< 検索オプション(現在未使用)
	GT_BATTLE_REC_SEARCH_SEND ConditionParam;							//!< 複合検索パラメータ
	u64 SearchParam;													//!< 検索パラメータ
} POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCH;

#define	POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH_MAXNUM	(30)			// クライアントが受け取るバトルデータヘッダ最大数

//! ＧＤＳバトルデータリスト取得レスポンス内容
typedef struct {
	long PID;															//!< アップロード者のプロファイルＩＤ
	u64 Code;															//!< バトルデータコード
	GT_BATTLE_REC_OUTLINE_RECV Data;									//!< バトルデータヘッダ
} POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH_RECVDATA;

//! ＧＤＳバトルデータリスト取得レスポンス
typedef struct {
	long HitCount;														//!< ヒット件数
	POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH_RECVDATA Data[0];			//!< バトルデータリスト
} POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH;

//------------------------------------
//      バトルデータ取得
//------------------------------------
//! ＧＤＳバトルデータ取得結果
enum POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET {
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_SUCCESS ,				//!< 取得成功
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_ERROR_AUTH ,			//!< ユーザー認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_ERROR_ILLEGALCODE ,		//!< コードエラー
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_ERROR_UNKNOWN			//!< その他エラー
};

//! ＧＤＳバトルデータ取得リクエスト
typedef struct {
	u64 Code;															//!< バトルデータコード
	u32 ServerVersion;													//!< 取得対象バトルデータのサーバーバージョン
} POKE_NET_GDS_REQUEST_BATTLEDATA_GET;

//! ＧＤＳバトルデータ取得レスポンス
typedef struct {
	long PID;															//!< アップロード者のプロファイルＩＤ
	u64 Code;															//!< バトルデータコード
	GT_BATTLE_REC_RECV Data;											//!< バトルデータ
} POKE_NET_GDS_RESPONSE_BATTLEDATA_GET;

//------------------------------------
//      バトルデータお気に入り登録
//------------------------------------
//! ＧＤＳバトルデータお気に入り登録結果
enum POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_FAVORITE {
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_FAVORITE_SUCCESS ,				//!< お気に入り登録成功
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_FAVORITE_ERROR_AUTH ,			//!< ユーザー認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_FAVORITE_ERROR_ILLEGALCODE ,	//!< コードエラー
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_FAVORITE_ERROR_UNKNOWN			//!< その他エラー
};

//! ＧＤＳバトルデータお気に入り登録リクエスト
typedef struct {
	u64 Code;															//!< バトルデータコード
} POKE_NET_GDS_REQUEST_BATTLEDATA_FAVORITE;

/*
//! ＧＤＳバトルデータお気に入り登録レスポンス
typedef struct {
} POKE_NET_GDS_RESPONSE_BATTLEDATA_FAVORITE;
*/

#ifndef ___POKE_NET_BUILD_DS___
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//        管理者リクエスト/レスポンス
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//------------------------------------
//        サーバー状態取得
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_STATUS {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_STATUS_SUCCESS ,				// 取得成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_STATUS_ERROR_AUTH ,			// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_STATUS_ERROR_UNKNOWN			// その他エラー
};

enum {
	POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUSDATA_STATUS_STOPPED ,			// 停止状態
	POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUSDATA_STATUS_ACTIVE ,			// 稼動状態
	POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUSDATA_STATUS_SHUTDOWN ,		// シャットダウン
	POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUSDATA_STATUS_UNKNOWN ,			// 不明状態(応答なし)
};

#define POKE_NET_GDS_RESPONSE_ADMIN_SVR_MAXNUM	(256)			// 最大サーバー数

enum {
	POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUSDATA_SVRTYPE_ADMIN ,	// 管理サーバー
	POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUSDATA_SVRTYPE_SLAVE ,	// 通常サーバー
};

typedef struct {
	long Type;												// サーバー種(POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUSDATA_SVRTYPE_xxxx)
	long Status;											// ステータス(POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUSDATA_STATUS_xxxxx)
	long MaxConnectCount;									// 最大コネクション可能数
	long ConnectCount;										// コネクション数
	u64 LastUpdateTime;										// 最終ステータス更新時間(0xYYYYMMDDHHMMSS)
	unsigned long IP;										// サーバーIPアドレス
	long Request;											// サーバーへ送っているリクエストコード
} POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUSDATA;

typedef struct {
	long ServerCount;										// サーバー数
	POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUSDATA Status[0];	// 各サーバーのステータス
} POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUS;

//------------------------------------
//        サーバー情報クリア
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_CLEARSTATUS {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_CLEARSTATUS_SUCCESS ,		// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_CLEARSTATUS_ERROR_AUTH ,		// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_CLEARSTATUS_ERROR_UNKNOWN	// その他エラー
};

//------------------------------------
//        サーバーへの命令
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_COMMAND {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_COMMAND_SUCCESS ,				// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_COMMAND_ERROR_AUTH ,				// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_COMMAND_ERROR_ILLEGALIP ,		// 不正なサーバーＩＰアドレス
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_COMMAND_ERROR_ILLEGALTYPE ,		// 通常サーバー以外には命令できない
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_COMMAND_ERROR_ILLEGALSTATUS ,	// 命令できないステータス状態
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_COMMAND_ERROR_ALREADYREQUEST ,	// すでに要求が送られているサーバー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_COMMAND_ERROR_UNKNOWN			// その他エラー
};

enum POKE_NET_GDS_REQUEST_ADMIN_SVR_COMMAND_REQCODE {
	POKE_NET_GDS_REQUEST_ADMIN_SVR_COMMAND_NOREQ ,						// 要求なし
	POKE_NET_GDS_REQUEST_ADMIN_SVR_COMMAND_START ,						// 開始要求
	POKE_NET_GDS_REQUEST_ADMIN_SVR_COMMAND_STOP ,						// 停止要求
	POKE_NET_GDS_REQUEST_ADMIN_SVR_COMMAND_SHUTDOWN ,					// シャットダウン要求
};

typedef struct {
	short Command;														// コマンド番号(POKE_NET_GDS_REQUEST_ADMIN_SVR_COMMAND_xxx)
	short Option;														// オプション(現在未使用)
	unsigned long IP;													// 開始するサーバーのIPアドレス
} POKE_NET_GDS_REQUEST_ADMIN_SVR_COMMAND;

//------------------------------------
// 初期ＤＢ設定
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_DBINITIALIZE {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_DBINITIALIZE_SUCCESS ,		// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_DBINITIALIZE_ERROR_AUTH ,	// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_DBINITIALIZE_ERROR_UNKNOWN	// その他エラー
};

//------------------------------------
//  ドレスアップショット：全体情報
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_DRESSUPSHOT_STATUS {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_DRESSUPSHOT_STATUS_SUCCESS ,				// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_DRESSUPSHOT_STATUS_ERROR_AUTH ,			// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_DRESSUPSHOT_STATUS_ERROR_UNKNOWN			// その他エラー
};

/* リクエストはコードのみ
typedef struct {
} POKE_NET_GDS_REQUEST_ADMIN_DRESSUPSHOT_STATUS;
*/

typedef struct {
	long TotalCount;										// 登録総数
	// 今後追加予定
} POKE_NET_GDS_RESPONSE_ADMIN_DRESSUPSHOT_STATUS;

//------------------------------------
//  ドレスアップショット：削除
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_DRESSUPSHOT_DELETE {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_DRESSUPSHOT_DELETE_SUCCESS ,				// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_DRESSUPSHOT_DELETE_ERROR_ILLEGALCODE ,	// コードエラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_DRESSUPSHOT_DELETE_ERROR_AUTH ,			// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_DRESSUPSHOT_DELETE_ERROR_UNKNOWN			// その他エラー
};

typedef struct {
	u64 Code;												// 削除したいコード
} POKE_NET_GDS_REQUEST_ADMIN_DRESSUPSHOT_DELETE;

/* レスポンスはコードのみ
typedef struct {
} POKE_NET_GDS_RESPONSE_ADMIN_DRESSUPSHOT_DELETE;
*/

//------------------------------------
//  ドレスアップショット：クリア
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_DRESSUPSHOT_CLEAR {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_DRESSUPSHOT_CLEAR_SUCCESS ,				// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_DRESSUPSHOT_CLEAR_ERROR_PERMISSION ,		// 認められていない
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_DRESSUPSHOT_CLEAR_ERROR_AUTH ,			// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_DRESSUPSHOT_CLEAR_ERROR_UNKNOWN			// その他エラー
};

/* リクエストはコードのみ
typedef struct {
} POKE_NET_GDS_REQUEST_ADMIN_DRESSUPSHOT_CLEAR;
*/

/* レスポンスはコードのみ
typedef struct {
} POKE_NET_GDS_RESPONSE_ADMIN_DRESSUPSHOT_CLEAR;
*/

//------------------------------------
//  ボックスショット：全体情報
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BOXSHOT_STATUS {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BOXSHOT_STATUS_SUCCESS ,				// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BOXSHOT_STATUS_ERROR_AUTH ,			// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BOXSHOT_STATUS_ERROR_UNKNOWN			// その他エラー
};

/* リクエストはコードのみ
typedef struct {
} POKE_NET_GDS_REQUEST_ADMIN_BOXSHOT_STATUS;
*/

typedef struct {
	long TotalCount;										// 登録総数
	// 今後追加予定
} POKE_NET_GDS_RESPONSE_ADMIN_BOXSHOT_STATUS;

//------------------------------------
//  ボックスショット：削除
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BOXSHOT_DELETE {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BOXSHOT_DELETE_SUCCESS ,				// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BOXSHOT_DELETE_ERROR_ILLEGALCODE ,	// コードエラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BOXSHOT_DELETE_ERROR_AUTH ,			// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BOXSHOT_DELETE_ERROR_UNKNOWN			// その他エラー
};

typedef struct {
	u64 Code;												// 削除したいコード
} POKE_NET_GDS_REQUEST_ADMIN_BOXSHOT_DELETE;

/* レスポンスはコードのみ
typedef struct {
} POKE_NET_GDS_RESPONSE_ADMIN_BOXSHOT_DELETE;
*/

//------------------------------------
//  ボックスショット：クリア
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BOXSHOT_CLEAR {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BOXSHOT_CLEAR_SUCCESS ,				// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BOXSHOT_CLEAR_ERROR_PERMISSION ,		// 認められていない
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BOXSHOT_CLEAR_ERROR_AUTH ,			// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BOXSHOT_CLEAR_ERROR_UNKNOWN			// その他エラー
};

/* リクエストはコードのみ
typedef struct {
} POKE_NET_GDS_REQUEST_ADMIN_BOXSHOT_CLEAR;
*/

/* レスポンスはコードのみ
typedef struct {
} POKE_NET_GDS_RESPONSE_ADMIN_BOXSHOT_CLEAR;
*/

//------------------------------------
//  バトルデータ：全体情報
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_STATUS {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_STATUS_SUCCESS ,				// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_STATUS_ERROR_AUTH ,			// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_STATUS_ERROR_UNKNOWN			// その他エラー
};

/* リクエストはコードのみ
typedef struct {
} POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_STATUS;
*/

typedef struct {
	long TotalRankingCount;									// 現在までのランキング確定回数
	long TotalRegistCount;									// 登録総数
	long TotalEventCount;									// イベント登録総数
	// 今後追加予定
} POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_STATUS;

//------------------------------------
//  バトルデータ：削除
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_DELETE {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_DELETE_SUCCESS ,				// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_DELETE_ERROR_ILLEGALCODE ,	// コードエラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_DELETE_ERROR_AUTH ,			// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_DELETE_ERROR_UNKNOWN			// その他エラー
};

enum POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_DELETE_AREA {
	POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_DELETE_AREA_ALL ,					// 全体
	POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_DELETE_AREA_REGIST ,				// 登録エリア
	POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_DELETE_AREA_TODAYRANKING ,		// 今日のランキング
	POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_DELETE_AREA_YESTERDAYRANKING ,	// 確定された先日ランキングエリア
	POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_DELETE_AREA_TODAYEXRANKING ,		// 今日の拡張ランキング
	POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_DELETE_AREA_YESTERDAYEXRANKING ,	// 確定された先日拡張ランキングエリア
	POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_DELETE_AREA_EVENT ,				// イベントエリア
};

typedef struct {
	long TargetArea;										// ターゲットエリア(POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_DELETE_AREA_xxxx)
	u64 Code;												// 削除したいコード
} POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_DELETE;

/* レスポンスはコードのみ
typedef struct {
} POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_DELETE;
*/

//------------------------------------
//  バトルデータ：クリア
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_CLEAR {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_CLEAR_SUCCESS ,				// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_CLEAR_ERROR_PERMISSION ,		// 認められていない
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_CLEAR_ERROR_AUTH ,			// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_CLEAR_ERROR_UNKNOWN			// その他エラー
};

/*リクエストはコードのみ
typedef struct {
} POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_CLEAR;
*/

/* レスポンスはコードのみ
typedef struct {
} POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_CLEAR;
*/


//------------------------------------
//  バトルデータ：ランキング履歴取得
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_RANKHISTORY {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_RANKHISTORY_SUCCESS ,				// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_RANKHISTORY_ERROR_AUTH ,			// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_RANKHISTORY_ERROR_UNKNOWN			// その他エラー
};

enum POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_RANKHISTORY_TYPE {
	POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_RANKHISTORY_TYPE_RANKING ,			// 通常ランキング
	POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_RANKHISTORY_TYPE_EXRANKING ,			// 拡張ランキング
	POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_RANKHISTORY_TYPE_NEW ,				// 最新
};

typedef struct {											// - ランキング履歴リクエスト -
	long Type;												// タイプ(POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_RANKHISTORY_TYPE_xxxx)
	long StartBefore;										// 取得開始過去日数(現在からStartBefore日前のランキングから取得)
	long Count;												// 取得するランキング日数
} POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_RANKHISTORY;

typedef struct {											// - ランキング内データ情報 -
	s32 PID;												// プロファイルＩＤ
	u64 Code;												// バトルデータコード
	u64 Point;												// 獲得ポイント
	u64 RegistTime;											// 登録日時(0xYYYYMMDDHHMMSS)
	u32 FavoriteCount;										// お気に入り登録数
	GT_GDS_PROFILE Profile;									// プロフィール
	GT_BATTLE_REC_OUTLINE_RECV Data;						// バトルデータヘッダ
} POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_RANKHISTORY_PARAM;

typedef struct {														// - ランキング情報 -
	long Count;															// ヒット数
	POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_RANKHISTORY_PARAM List[0];	// 各データ
} POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_RANKHISTORY_RANKING;

typedef struct {											// - ランキング履歴レスポンス -
	long HitCount;											// ヒットしたランキング数
	char RankingList[0];									// ランキングリスト
															// (sizeof(POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_RANKHISTORY_RANKING) +
															//	sizeof(POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_RANKHISTORY_PARAM) * それぞれのランキング内のCount) * HitCount
} POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_RANKHISTORY;

//------------------------------------
//  バトルデータ：イベント登録要求
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_EVENTFLAG {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_EVENTFLAG_SUCCESS ,				// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_EVENTFLAG_ERROR_ILLEGALCODE ,		// コードエラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_EVENTFLAG_ERROR_AUTH ,			// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_EVENTFLAG_ERROR_UNKNOWN			// その他エラー
};

typedef struct {
	u64 Code;												// イベント領域へ移動させたいデータのコード
} POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_EVENTFLAG;

/*
typedef struct {
} POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_EVENTFLAG;
*/

//------------------------------------
//  バトルデータ：管理パラメータ取得
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_GETPARAM {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_GETPARAM_SUCCESS ,				// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_GETPARAM_ERROR_AUTH ,				// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_GETPARAM_ERROR_UNKNOWN ,			// エラー
};

#define	POKE_NET_GDS_BATTLEDATA_GETPARAM_WEIGHT_MAXNUM	(30+2)			// 順位によるパラメータ数(順位数+その他/最新)
#define	POKE_NET_GDS_BATTLEDATA_GETPARAM_SCALING_MAXNUM	(10+1)			// 順位によるパラメータ数

/*
typedef struct {
} POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_GETPARAM;
*/

typedef struct {
	long Weight[POKE_NET_GDS_BATTLEDATA_GETPARAM_WEIGHT_MAXNUM];		// ポイント重みづけ
	long Scaling[POKE_NET_GDS_BATTLEDATA_GETPARAM_SCALING_MAXNUM];		// 順位によるポイント倍率
	s64 InitPoint;														// 初期ポイント差分値
	s64 Bias;															// 平坦化閾値
} POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_GETPARAM;

//------------------------------------
//  バトルデータ：管理パラメータ設定
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_SETPARAM {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_SETPARAM_SUCCESS ,				// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_SETPARAM_ERROR_AUTH ,				// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_SETPARAM_ERROR_PARAMS ,			// パラメータエラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_SETPARAM_ERROR_UNKNOWN ,			// エラー
};

#define	POKE_NET_GDS_BATTLEDATA_SETPARAM_WEIGHT_MAXNUM	(30+2)			// 順位によるパラメータ数(順位数+その他/最新)
#define	POKE_NET_GDS_BATTLEDATA_SETPARAM_SCALING_MAXNUM	(10+1)			// 順位によるパラメータ数

typedef struct {
	long Weight[POKE_NET_GDS_BATTLEDATA_SETPARAM_WEIGHT_MAXNUM];		// ポイント重みづけ
	long Scaling[POKE_NET_GDS_BATTLEDATA_SETPARAM_SCALING_MAXNUM];		// 順位によるポイント倍率
	s64 InitPoint;														// 初期ポイント差分値
	s64 Bias;															// 平坦化閾値
} POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_SETPARAM;

/*
typedef struct {
} POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_SETPARAM;
*/

//------------------------------------
//  バトルデータ：強制確定処理
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_EXECDECISION {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_EXECDECISION_SUCCESS ,				// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_EXECDECISION_ERROR_AUTH ,				// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_EXECDECISION_ERROR_UNKNOWN ,			// エラー
};

/*
typedef struct {
} POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_EXECDECISION;
*/

/*
typedef struct {
} POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_EXECDECISION;
*/

//------------------------------------
//  ランキング：全体情報
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_STATUS {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_STATUS_SUCCESS ,				// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_STATUS_ERROR_AUTH ,			// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_STATUS_ERROR_UNKNOWN			// その他エラー
};

/* リクエストはコードのみ
typedef struct {
} POKE_NET_GDS_REQUEST_ADMIN_RANKING_STATUS;
*/

typedef struct {
	long TotalRankingCount;									// 現在までのランキング回数
	long ThisWeekRankingNo;									// 現在のランキングの開催番号
	// 今後追加予定
} POKE_NET_GDS_RESPONSE_ADMIN_RANKING_STATUS;


//------------------------------------
//  ランキング：ランキング履歴取得
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_HISTORY {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_HISTORY_SUCCESS ,				// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_HISTORY_ERROR_AUTH ,				// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_HISTORY_ERROR_UNKNOWN			// その他エラー
};

typedef struct {											// - ランキング履歴リクエスト -
	long Type;												// ランキングタイプ(-1:指定なし)
	long StartBefore;										// 取得開始過去開催回数(現在からStartBefore回前のランキングから取得)
	long Count;												// 取得するランキング回数
} POKE_NET_GDS_REQUEST_ADMIN_RANKING_HISTORY;


typedef struct {											// - ランキング履歴レスポンス -
	long Type;												// 取得したランキングタイプ
	long HitCount;											// ヒットしたランキング数
	GT_LAST_WEEK_RANKING_DATA Ranking[0];					// ランキングデータ(Type:-1の時はGT_RANKING_WEEK_NUM個で１ランキング)
} POKE_NET_GDS_RESPONSE_ADMIN_RANKING_HISTORY;


//------------------------------------
//  ランキング：ユーザーパラメータ取得
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_GET {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_GET_SUCCESS ,		// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_GET_ERROR_AUTH ,		// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_GET_ERROR_UNKNOWN	// その他エラー
};

#define POKE_NET_GDS_ADMIN_RANKING_USER_STATUS_INEFFECTIVE (0)				// 無効
#define POKE_NET_GDS_ADMIN_RANKING_USER_STATUS_EFFECTIVE   (1)			// 有効

#define POKE_NET_GDS_ADMIN_RANKING_USER_MAXCOUNT    (50)									// パラメータが返ってくる最大プレイヤー数
#define POKE_NET_GDS_ADMIN_RANKING_USER_PARAMCOUNT  (GT_RANKING_MYDATA_RANKING_TYPE_MAX)	// パラメータが返ってくる最大プレイヤー数

typedef struct {													// - ランキング パラメータ取得リクエスト -
	long PID;														// ユーザーのプロファイルＩＤ(0:全無効ユーザー)
	long PageNo;													// ページ番号(POKE_NET_GDS_ADMIN_RANKING_USER_MAXCOUNT = １ページ)
} POKE_NET_GDS_REQUEST_ADMIN_RANKING_GET;

typedef struct {													// - ランキング ユーザーパラメータ取得 -
	long PID;														// PID
	long Status;													// 現在のステータス(POKE_NET_GDS_ADMIN_RANKING_USER_STATUS_xxxx)
	u64 Params[POKE_NET_GDS_ADMIN_RANKING_USER_PARAMCOUNT];			// 各パラメータ(サーバーに保持しているユーザー毎の得点)
} POKE_NET_GDS_RESPONSE_ADMIN_RANKING_GETPARAM;

typedef struct {													// - ランキング ユーザーパラメータ取得 -
	long Count;														// ユーザー数
	POKE_NET_GDS_RESPONSE_ADMIN_RANKING_GETPARAM List[0];			// ユーザーパラメータ
																	// 最大で * POKE_NET_GDS_ADMIN_RANKING_USER_MAXCOUNT
} POKE_NET_GDS_RESPONSE_ADMIN_RANKING_GET;


//------------------------------------
//  ランキング：ユーザーパラメータ設定
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_SET {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_SET_SUCCESS ,		// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_SET_ERROR_AUTH ,		// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_SET_ERROR_UNKNOWN	// その他エラー
};

typedef struct {													// - ランキング ユーザーパラメータ設定 -
	long PID;														// PID(0:無効)
	long Status;													// 現在のステータス(POKE_NET_GDS_ADMIN_RANKING_USER_STATUS_xxxx)
	u64 Params[POKE_NET_GDS_ADMIN_RANKING_USER_PARAMCOUNT];			// 各パラメータ(サーバーに保持しているユーザー毎の得点)
} POKE_NET_GDS_REQUEST_ADMIN_RANKING_SETPARAM;

typedef struct {																						// - ランキング ユーザーパラメータ設定 -
	long Count;																							// ユーザー数
	POKE_NET_GDS_REQUEST_ADMIN_RANKING_SETPARAM List[POKE_NET_GDS_ADMIN_RANKING_USER_MAXCOUNT];			// ユーザーパラメータ
} POKE_NET_GDS_REQUEST_ADMIN_RANKING_SET;

/* レスポンスはコードのみ
typedef struct {													// - ユーザーパラメータ設定レスポンス -
} POKE_NET_GDS_RESPONSE_ADMIN_RANKING_SET;
*/

//------------------------------------
//  ランキング：クリア
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_CLEAR {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_CLEAR_SUCCESS ,				// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_CLEAR_ERROR_PERMISSION ,		// 認められていない
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_CLEAR_ERROR_AUTH ,			// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_CLEAR_ERROR_UNKNOWN			// その他エラー
};

/* リクエストはコードのみ
typedef struct {
} POKE_NET_GDS_REQUEST_ADMIN_RANKING_CLEAR;
*/

/* レスポンスはコードのみ
typedef struct {
} POKE_NET_GDS_RESPONSE_ADMIN_RANKING_CLEAR;
*/

//------------------------------------
//  ランキング：スケジュール取得
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_GETSCHEDULE {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_GETSCHEDULE_SUCCESS ,				// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_GETSCHEDULE_ERROR_AUTH ,				// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_GETSCHEDULE_ERROR_UNKNOWN			// その他エラー
};

/* リクエストはコードのみ
typedef struct {
} POKE_NET_GDS_REQUEST_ADMIN_RANKING_GETSCHEDULE;
*/

#define	POKE_NET_GDS_RESPONSE_ADMIN_RANKING_SCHEDULE_CATEGORYCOUNT	(3)
#define	POKE_NET_GDS_RESPONSE_ADMIN_RANKING_SCHEDULE_ITEMCOUNT		(256)

// スケジュールリスト
typedef struct {
	long CurrentWeekNo;
	long CurrentNo[POKE_NET_GDS_RESPONSE_ADMIN_RANKING_SCHEDULE_CATEGORYCOUNT];
	unsigned char Kind[POKE_NET_GDS_RESPONSE_ADMIN_RANKING_SCHEDULE_CATEGORYCOUNT][POKE_NET_GDS_RESPONSE_ADMIN_RANKING_SCHEDULE_ITEMCOUNT];
} POKE_NET_GDS_RESPONSE_ADMIN_RANKING_GETSCHEDULE;

//------------------------------------
//  ランキング：スケジュール設定
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_SETSCHEDULE {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_SETSCHEDULE_SUCCESS ,				// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_SETSCHEDULE_ERROR_AUTH ,				// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_SETSCHEDULE_ERROR_UNKNOWN			// その他エラー
};

#define	POKE_NET_GDS_RESPONSE_ADMIN_RANKING_SCHEDULE_CATEGORYCOUNT	(3)
#define	POKE_NET_GDS_RESPONSE_ADMIN_RANKING_SCHEDULE_ITEMCOUNT		(256)

// スケジュールリスト
typedef struct {
	unsigned char Kind[POKE_NET_GDS_RESPONSE_ADMIN_RANKING_SCHEDULE_CATEGORYCOUNT][POKE_NET_GDS_RESPONSE_ADMIN_RANKING_SCHEDULE_ITEMCOUNT];
} POKE_NET_GDS_REQUEST_ADMIN_RANKING_SETSCHEDULE;

/* レスポンスはコードのみ
typedef struct {
} POKE_NET_GDS_RESPONSE_ADMIN_RANKING_SETSCHEDULE;
*/

//------------------------------------
//  ランキング：強制確定処理
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_EXECDECISION {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_EXECDECISION_SUCCESS ,				// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_EXECDECISION_ERROR_AUTH ,			// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_RANKING_EXECDECISION_ERROR_UNKNOWN ,			// エラー
};

/*
typedef struct {
} POKE_NET_GDS_REQUEST_ADMIN_RANKING_EXECDECISION;
*/

/*
typedef struct {
} POKE_NET_GDS_RESPONSE_ADMIN_RANKING_EXECDECISION;
*/

//------------------------------------
// 不正チェック:ポケモン毎の設定取得
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_POKEMONTABLE_GET {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_POKEMONTABLE_GET_SUCCESS ,		// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_POKEMONTABLE_GET_ERROR_AUTH ,	// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_POKEMONTABLE_GET_ERROR_UNKNOWN	// その他エラー
};

/*
typedef struct {													// - ポケモン毎設定取得リクエスト -
} POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_POKEMONTABLE_GET;
*/

#define POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_MONSCOUNT	(507)	// ポケモン毎表はフォルム枠ポケモンも含む500体

enum {
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_EFFECTIVE ,	// 有効/無効
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_RAREFLAG ,		// レアフラグ(立っていたらレア有効)
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_DISTRIBUTE ,	// 配布(立っていたら配布フラグをチェックする)
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_LVMIN ,		// 最低レベル
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_FORMMAX ,		// フォルム番号最大
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_CFORM ,		// 計算済みフォルム値
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_ABSEVO ,		// 強制的に進化前として扱うか否か
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_LEGEND ,		// 伝説ポケモンか否か
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_ABILITY0 ,		// とくせい0
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_ABILITY1 ,		// とくせい1
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_HP ,			// HP
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_ATTACK ,		// こうげき
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_DEFENSE ,		// ぼうぎょ
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_AGILITY ,		// すばやさ
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_SATTACK ,		// とくこう
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_SDEFENSE ,		// とくぼう
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_TYPE1 ,		// タイプ1
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_TYPE2 ,		// タイプ2
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_CAPTUREPER ,	// 捕獲率
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_EXP ,			// 経験値
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_EHP ,			// 努力値HP
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_EATTACK ,		// 努力値こうげき
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_EDEFENSE ,		// 努力値ぼうぎょ
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_EAGILITY ,		// 努力値すばやさ
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_ESATTACK ,		// 努力値とくこう
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_ESDEFENSE ,	// 努力値とくぼう
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_ITEM0 ,		// どうぐ0
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_ITEM1 ,		// どうぐ1
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_SEX ,			// 性別ベクトル
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_BORNWALK ,		// 孵化歩数
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_ATTACH ,		// 初期なつき度
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_GROWTH ,		// 成長曲線
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_EGG0 ,			// タマゴグループ0
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_EGG1 ,			// タマゴグループ2
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_ESCAPEPER ,	// 逃げ率
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_COLOR ,		// 色
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_REV ,			// 反転不可
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_WAZAFLAG0 ,	// わざマシンフラグ0
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_WAZAFLAG1 ,	// わざマシンフラグ1
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_WAZAFLAG2 ,	// わざマシンフラグ2
	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_WAZAFLAG3 ,	// わざマシンフラグ3

	POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_COUNT			// 個数
};

typedef struct {													// - ポケモン毎設定取得レスポンス -
	unsigned long Table[POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_MONSCOUNT][POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_COUNT];
} POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_POKEMONTABLE_GET;

//------------------------------------
// 不正チェック:ポケモン毎の設定セット
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_POKEMONTABLE_SET {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_POKEMONTABLE_SET_SUCCESS ,		// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_POKEMONTABLE_SET_ERROR_AUTH ,	// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_POKEMONTABLE_SET_ERROR_UNKNOWN	// その他エラー
};

typedef struct {													// - ポケモン毎設定リクエスト -
	unsigned long Table[POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_MONSCOUNT][POKE_NET_GDS_ADMIN_ILLEGALCHECK_POKEMONTABLE_COUNT];
} POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_POKEMONTABLE_SET;

/*
typedef struct {													// - ポケモン毎設定レスポンス -
} POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_POKEMONTABLE_SET;
*/

//------------------------------------
// 不正チェック:ＮＧワードの設定取得
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_NGWORD_GET {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_NGWORD_GET_SUCCESS ,		// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_NGWORD_GET_ERROR_AUTH ,		// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_NGWORD_GET_ERROR_UNKNOWN	// その他エラー
};

/*
typedef struct {													// - ＮＧワード設定取得リクエスト -
} POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_NGWORD_GET;
*/

#define POKE_NET_GDS_ADMIN_ILLEGALCHECK_NGWORD_MAXNUM (100)

typedef struct {													// - ＮＧワード設定取得レスポンス -
	long Count;														// ＮＧワードコード数
	long Codes[0];													// ＮＧワードコード
} POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_NGWORD_GET;

//------------------------------------
// 不正チェック:ＮＧワードの設定セット
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_NGWORD_SET {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_NGWORD_SET_SUCCESS ,		// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_NGWORD_SET_ERROR_AUTH ,		// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_NGWORD_SET_ERROR_UNKNOWN	// その他エラー
};

typedef struct {													// - ＮＧワード設定リクエスト -
	long Count;														// ＮＧワードコード数
	long Codes[POKE_NET_GDS_ADMIN_ILLEGALCHECK_NGWORD_MAXNUM];		// ＮＧワードコード
} POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_NGWORD_SET;

/*
typedef struct {													// - ＮＧワード設定レスポンス -
} POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_NGWORD_SET;
*/

//------------------------------------
// 不正チェック:言語コードの設定取得
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_LANGCODE_GET {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_LANGCODE_GET_SUCCESS ,			// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_LANGCODE_GET_ERROR_AUTH ,		// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_LANGCODE_GET_ERROR_UNKNOWN		// その他エラー
};

/*
typedef struct {													// - 言語コード設定取得リクエスト -
} POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_LANGCODE_GET;
*/

#define POKE_NET_GDS_ADMIN_ILLEGALCHECK_LANGCODE_MAXNUM (100)

typedef struct {													// - 言語コード設定取得レスポンス -
	long Count;														// 言語コードコード数
	long Codes[0];													// 言語コードコード
} POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_LANGCODE_GET;

//------------------------------------
// 不正チェック:言語コードの設定セット
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_LANGCODE_SET {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_LANGCODE_SET_SUCCESS ,			// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_LANGCODE_SET_ERROR_AUTH ,		// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_LANGCODE_SET_ERROR_UNKNOWN		// その他エラー
};

typedef struct {													// - 言語コード設定リクエスト -
	long Count;														// 言語コード数
	long Codes[POKE_NET_GDS_ADMIN_ILLEGALCHECK_LANGCODE_MAXNUM];	// 言語コード
} POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_LANGCODE_SET;

/*
typedef struct {													// - 言語コード設定レスポンス -
} POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_LANGCODE_SET;
*/

//------------------------------------
// 不正チェック:どうぐの設定取得
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_ITEM_GET {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_ITEM_GET_SUCCESS ,			// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_ITEM_GET_ERROR_AUTH ,		// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_ITEM_GET_ERROR_UNKNOWN		// その他エラー
};


/*
typedef struct {													// - どうぐ設定取得リクエスト -
} POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_ITEM_GET;
*/

#define POKE_NET_GDS_ADMIN_ILLEGALCHECK_ITEM_MAXNUM (100)

typedef struct {													// - どうぐ設定取得レスポンス -
	long Count;														// どうぐ数
	long Codes[0];													// どうぐコード
} POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_ITEM_GET;

//------------------------------------
// 不正チェック:どうぐの設定セット
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_ITEM_SET {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_ITEM_SET_SUCCESS ,			// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_ITEM_SET_ERROR_AUTH ,		// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_ITEM_SET_ERROR_UNKNOWN		// その他エラー
};

typedef struct {													// - どうぐ設定リクエスト -
	long Count;														// どうぐ数
	long Codes[POKE_NET_GDS_ADMIN_ILLEGALCHECK_ITEM_MAXNUM];		// どうぐコード
} POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_ITEM_SET;

/*
typedef struct {													// - どうぐ設定レスポンス -
} POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_ITEM_SET;
*/

//------------------------------------
// 不正チェック:出会った場所の設定取得
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MEETPLACE_GET {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MEETPLACE_GET_SUCCESS ,		// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MEETPLACE_GET_ERROR_AUTH ,	// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MEETPLACE_GET_ERROR_UNKNOWN	// その他エラー
};

/*
typedef struct {													// - 出会った場所設定取得リクエスト -
} POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_MEETPLACE_GET;
*/

#define POKE_NET_GDS_ADMIN_ILLEGALCHECK_MEETPLACE_MAXPLACE		(1+32)
#define POKE_NET_GDS_ADMIN_ILLEGALCHECK_MEETPLACE_MAXPOKEMON	(GT_BOXSHOT_MONSNO_MAX)

typedef struct {													// - 出会った場所設定取得レスポンス -
	long Count;														// 設定されているポケモン数
	s16 Table[0];													// sizeof(s16)*POKE_NET_GDS_ADMIN_ILLEGALCHECK_MEETPLACE_MAXPLACE * HitCount
																	// [0]はポケモン番号
} POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_MEETPLACE_GET;

//------------------------------------
// 不正チェック:出会った場所の設定セット
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MEETPLACE_SET {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MEETPLACE_SET_SUCCESS ,		// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MEETPLACE_SET_ERROR_AUTH ,	// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MEETPLACE_SET_ERROR_UNKNOWN	// その他エラー
};

typedef struct {													// - 出会った場所設定リクエスト -
	long Count;
	s16 Table[POKE_NET_GDS_ADMIN_ILLEGALCHECK_MEETPLACE_MAXPOKEMON*POKE_NET_GDS_ADMIN_ILLEGALCHECK_MEETPLACE_MAXPLACE];	// sizeof(s16)*POKE_NET_GDS_ADMIN_ILLEGALCHECK_MEETPLACE_MAXPLACE * HitCount
																														// [0]はポケモン番号
} POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_MEETPLACE_SET;

/*
typedef struct {													// - 出会った場所設定レスポンス -
} POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_MEETPLACE_SET;
*/


//------------------------------------
// 不正チェック:技表の設定取得
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_WAZATABLE_GET {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_WAZATABLE_GET_SUCCESS ,		// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_WAZATABLE_GET_ERROR_AUTH ,	// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_WAZATABLE_GET_ERROR_UNKNOWN	// その他エラー
};

#define POKE_NET_GDS_ADMIN_ILLEGALCHECK_WAZATABLE_MONSCOUNT	(507)	// 技表はフォルム枠ポケモンも含む507体

/*
typedef struct {													// - 技表設定取得リクエスト -
} POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_WAZATABLE_GET;
*/

typedef struct {													// - 技表設定取得レスポンス -
	long Size;														// サイズ
	char Binary[0];													// バイナリデータ
} POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_WAZATABLE_GET;

//------------------------------------
// 不正チェック:技表の設定セット
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_WAZATABLE_SET {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_WAZATABLE_SET_SUCCESS ,		// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_WAZATABLE_SET_ERROR_AUTH ,	// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_WAZATABLE_SET_ERROR_UNKNOWN	// その他エラー
};

typedef struct {													// - 技表設定リクエスト -
	long Size;														// サイズ
	char Binary[0];													// バイナリデータ
} POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_WAZATABLE_SET;

/*
typedef struct {													// - 技表設定レスポンス -
} POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_WAZATABLE_SET;
*/


//------------------------------------
// 不正チェック:アクセサリの設定取得
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_ACCESSORY_GET {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_ACCESSORY_GET_SUCCESS ,		// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_ACCESSORY_GET_ERROR_AUTH ,	// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_ACCESSORY_GET_ERROR_UNKNOWN	// その他エラー
};

/*
typedef struct {													// - アクセサリ設定取得リクエスト -
} POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_ACCESSORY_GET;
*/

#define POKE_NET_GDS_ADMIN_ILLEGALCHECK_ACCESSORY_MAXNUM (100)

typedef struct {													// - アクセサリ設定取得レスポンス -
	long Count;														// アクセサリコード数
	long Codes[0];													// アクセサリコード
} POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_ACCESSORY_GET;

//------------------------------------
// 不正チェック:アクセサリの設定セット
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_ACCESSORY_SET {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_ACCESSORY_SET_SUCCESS ,		// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_ACCESSORY_SET_ERROR_AUTH ,	// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_ACCESSORY_SET_ERROR_UNKNOWN	// その他エラー
};

typedef struct {														// - アクセサリ設定リクエスト -
	long Count;															// アクセサリコード数
	long Codes[POKE_NET_GDS_ADMIN_ILLEGALCHECK_ACCESSORY_MAXNUM];		// アクセサリコード
} POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_ACCESSORY_SET;

/*
typedef struct {													// - アクセサリ設定レスポンス -
} POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_ACCESSORY_SET;
*/

//------------------------------------
// 不正チェック:ドレスアップBGIDの設定取得
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_DRESSUPBGID_GET {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_DRESSUPBGID_GET_SUCCESS ,		// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_DRESSUPBGID_GET_ERROR_AUTH ,	// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_DRESSUPBGID_GET_ERROR_UNKNOWN	// その他エラー
};

/*
typedef struct {													// - ドレスアップBGID設定取得リクエスト -
} POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_DRESSUPBGID_GET;
*/

#define POKE_NET_GDS_ADMIN_ILLEGALCHECK_DRESSUPBGID_MAXNUM (18)

typedef struct {													// - ドレスアップBGID設定取得レスポンス -
	long Count;														// ドレスアップBGIDコード数
	long Codes[0];													// ドレスアップBGIDコード
} POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_DRESSUPBGID_GET;

//------------------------------------
// 不正チェック:ドレスアップBGIDの設定セット
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_DRESSUPBGID_SET {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_DRESSUPBGID_SET_SUCCESS ,		// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_DRESSUPBGID_SET_ERROR_AUTH ,	// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_DRESSUPBGID_SET_ERROR_UNKNOWN	// その他エラー
};

typedef struct {														// - ドレスアップBGID設定リクエスト -
	long Count;															// ドレスアップBGIDコード数
	long Codes[POKE_NET_GDS_ADMIN_ILLEGALCHECK_DRESSUPBGID_MAXNUM];		// ドレスアップBGIDコード
} POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_DRESSUPBGID_SET;

/*
typedef struct {													// - ドレスアップBGID設定レスポンス -
} POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_DRESSUPBGID_SET;
*/


//------------------------------------
// 不正チェック:レギュレーションの設定取得
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_REGULATION_GET {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_REGULATION_GET_SUCCESS ,		// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_REGULATION_GET_ERROR_AUTH ,		// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_REGULATION_GET_ERROR_UNKNOWN	// その他エラー
};

/*
typedef struct {																	// - レギュレーション設定取得リクエスト -
} POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_REGULATION_GET;
*/

#define POKE_NET_GDS_ADMIN_ILLEGALCHECK_REGULATION_MAXNUM (100)

typedef struct {																	// - レギュレーション設定取得レスポンス -
	long Count;																		// レギュレーション数
	GT_REGULATION Regulation[0];													// レギュレーション
} POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_REGULATION_GET;

//------------------------------------
// 不正チェック:レギュレーションの設定セット
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_REGULATION_SET {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_REGULATION_SET_SUCCESS ,		// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_REGULATION_SET_ERROR_AUTH ,		// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_REGULATION_SET_ERROR_UNKNOWN	// その他エラー
};

typedef struct {																	// - レギュレーション設定リクエスト -
	long Count;																		// レギュレーションコード数
	GT_REGULATION Regulation[POKE_NET_GDS_ADMIN_ILLEGALCHECK_REGULATION_MAXNUM];	// レギュレーション数
} POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_REGULATION_SET;

/*
typedef struct {																	// - レギュレーション設定レスポンス -
} POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_REGULATION_SET;
*/

//------------------------------------
// 不正チェックログ:取得
//------------------------------------
#define POKE_NET_GDS_RESPONSE_ADMIN_DHOCHECKLOG_GET_TIMECAPA	(20)	// 時刻文字列の大きさ
#define POKE_NET_GDS_RESPONSE_ADMIN_DHOCHECKLOG_GET_CONTENTCAPA	(256)	// 内容文字列の大きさ
#define POKE_NET_GDS_RESPONSE_ADMIN_DHOCHECKLOG_GET_MAXRESPONSE	(30)	// 最大何件までのデータを返答するか

enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_DHOCHECKLOG_GET {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_DHOCHECKLOG_GET_SUCCESS ,
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_DHOCHECKLOG_GET_ERROR_UNKNOWN
};

typedef struct {
	long	PageNo;		// ページ番号
} POKE_NET_GDS_REQUEST_ADMIN_DHOCHECKLOG_GET;

typedef struct {
	char	Time[POKE_NET_GDS_RESPONSE_ADMIN_DHOCHECKLOG_GET_TIMECAPA];
	char	Content[POKE_NET_GDS_RESPONSE_ADMIN_DHOCHECKLOG_GET_CONTENTCAPA];
} POKE_NET_GDS_RESPONSE_ADMIN_DHOCHECKLOG_GET_PARAM;

typedef struct {
	long	HitCount;
	POKE_NET_GDS_RESPONSE_ADMIN_DHOCHECKLOG_GET_PARAM	List[0];
} POKE_NET_GDS_RESPONSE_ADMIN_DHOCHECKLOG_GET;

//------------------------------------
// 不正チェックログ:クリア
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_DHOCHECKLOG_CLEAR {
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_DHOCHECKLOG_CLEAR_SUCCESS ,			// 成功
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_DHOCHECKLOG_CLEAR_ERROR_PERMISSION ,	// 認められていない
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_DHOCHECKLOG_CLEAR_ERROR_AUTH ,		// 認証エラー
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_DHOCHECKLOG_CLEAR_ERROR_UNKNOWN		// その他エラー
};

/*	 リクエストはコードのみ
typedef struct {
} POKE_NET_GDS_REQUEST_ADMIN_DHOCHECKLOG_CLEAR;
*/

/*	レスポンスはコードのみ
typedef struct {
} POKE_NET_GDS_RESPONSE_ADMIN_DHOCHECKLOG_CLEAR;
*/

#endif

#endif //___POKE_NET_GDS_COMMON___
