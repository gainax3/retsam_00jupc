//==============================================================================
/**
 * @file	gds_rap.h
 * @brief	GDSライブラリをラッパーしたもの
 * @author	matsuda
 * @date	2008.01.09(水)
 */
//==============================================================================
#ifndef __GDS_RAP_H__
#define __GDS_RAP_H__

#include <dwc.h>
#include "libgds/poke_net_gds.h"
#include "libgds/poke_net_common.h"


//==============================================================================
//	定数定義
//==============================================================================
///GDSエラータイプ
enum{
	GDS_ERROR_TYPE_LIB,			///<ライブラリエラー(POKE_NET_GDS_LASTERROR)
	GDS_ERROR_TYPE_STATUS,		///<ステータスエラー(POKE_NET_GDS_STATUS)
	GDS_ERROR_TYPE_APP,			///<各アプリ毎のエラー
};

//==============================================================================
//	構造体定義
//==============================================================================

///送信パラメータ構造体
typedef struct{
	//送信データ
	union{
		GT_GDS_DRESS_SEND gt_dress_send;	///<ドレスアップ送信データ
		GT_BOX_SHOT_SEND gt_box_send;		///<ボックスショット送信データ
		GT_RANKING_MYDATA_SEND gt_ranking_mydata_send;	///<自分のランキング送信データ
		GT_BATTLE_REC_SEND *gt_battle_rec_send_ptr;	///<送信データへのポインタ
		GT_BATTLE_REC_SEARCH_SEND gt_battle_rec_search;	///<ビデオ検索送信データ
		GT_BATTLE_REC_RANKING_SEARCH_SEND gt_battle_rec_ranking_search;	///<ビデオランキング検索送信データ
	};

	//送信データに付随するオプションパラメータ
	union{
		//ボックスのサブパラメータ
		struct{
			u8 category_no;		///<登録カテゴリー番号
			u8 tray_number;		///<ボックス番号
			u8 box_padding[2];
		}box;
		//ドレスアップのサブパラメータ
		struct{
			u16 recv_monsno;	///<取得するカテゴリー番号(ポケモン番号)
			u16 dressup_padding;
		}dressup;
		//バトルビデオのサブパラメータ
		struct{
			u64 data_number;	///<データナンバー
		}rec;
	}sub_para;
}GDS_RAP_SEND_WORK;

///GDSラッパーワークのサブワーク
typedef struct _GDS_RAP_SUB_PROCCESS_WORK{
	int local_seq;
	int wait;
	int work;
}GDS_RAP_SUB_PROCCESS_WORK;

///GDSライブラリのエラー情報管理構造体
typedef struct _GDS_RAP_ERROR_INFO{
	BOOL occ;				///<TRUE:エラー情報あり。　FALSE:エラー情報無し
	int type;				///<エラータイプ(GDS_ERROR_TYPE_???)
	int req_code;			///<リクエストコード(POKE_NET_RESPONSE.ReqCode)
	int result;				///<結果(POKE_NET_RESPONSE.Result)
}GDS_RAP_ERROR_INFO;

///データ受信時のコールバック関数の型
typedef void (*GDSRAP_RESPONSE_FUNC)(void *, const GDS_RAP_ERROR_INFO *);
///通信エラーメッセージ表示のコールバック関数の型
typedef void (*GDSRAP_ERROR_WIDE_MSG_FUNC)(void *, STRBUF *);

///データ受信時のコールバック関数などをまとめた構造体
typedef struct{
	void *callback_work;	///<データ受信時のコールバック関数に引数として渡すポインタ

	///データ受信時のコールバック関数へのポインタ
	///ドレスアップショット登録(POKE_NET_GDS_REQCODE_DRESSUPSHOT_REGIST)
	GDSRAP_RESPONSE_FUNC func_dressupshot_regist;
	
	///データ受信時のコールバック関数へのポインタ
	///ドレスアップショット取得(POKE_NET_GDS_REQCODE_DRESSUPSHOT_GET)
	GDSRAP_RESPONSE_FUNC func_dressupshot_get;
	
	///データ受信時のコールバック関数へのポインタ
	///ボックスショット登録(POKE_NET_GDS_REQCODE_BOXSHOT_REGIST)
	GDSRAP_RESPONSE_FUNC func_boxshot_regist;
	
	///データ受信時のコールバック関数へのポインタ
	///ボックスショット取得(POKE_NET_GDS_REQCODE_BOXSHOT_GET)
	GDSRAP_RESPONSE_FUNC func_boxshot_get;

	///データ受信時のコールバック関数へのポインタ
	///開催中のランキングタイプ取得(POKE_NET_GDS_REQCODE_RANKING_GETTYPE)
	GDSRAP_RESPONSE_FUNC func_ranking_type_get;
	
	///データ受信時のコールバック関数へのポインタ
	///ランキング更新(自分のデータ取得＆結果取得)(POKE_NET_GDS_REQCODE_RANKING_UPDATE)
	GDSRAP_RESPONSE_FUNC func_ranking_update_get;

	///データ受信時のコールバック関数へのポインタ
	///バトルビデオ登録(POKE_NET_GDS_REQCODE_BATTLEDATA_REGIST)
	GDSRAP_RESPONSE_FUNC func_battle_video_regist;

	///データ受信時のコールバック関数へのポインタ
	///バトルビデオ検索(詳細検索、最新、お気に入りランキング)(POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH)
	GDSRAP_RESPONSE_FUNC func_battle_video_search_get;

	///データ受信時のコールバック関数へのポインタ
	///バトルビデオデータ取得(POKE_NET_GDS_REQCODE_BATTLEDATA_GET)
	GDSRAP_RESPONSE_FUNC func_battle_video_data_get;

	///データ受信時のコールバック関数へのポインタ
	///バトルビデオお気に入り登録(POKE_NET_GDS_REQCODE_BATTLEDATA_FAVORITE)
	GDSRAP_RESPONSE_FUNC func_battle_video_favorite_regist;
}GDS_RAP_RESPONSE_CALLBACK;

///データ受信後に行うサブプロセス用のコールバック関数の型
typedef BOOL (*GDSRAP_RECV_SUB_PROCCESS_FUNC)(void *, void *);

///データ受信後に行うサブプロセス用ワーク
typedef struct _GDS_RECV_SUB_PROCCESS_WORK{
	GDSRAP_RESPONSE_FUNC user_callback_func;	///<データ受信後に呼び出すのコールバック関数
	u16 recv_save_seq0;
	u16 recv_save_seq1;
	u8 recv_sub_seq;
	GDSRAP_RECV_SUB_PROCCESS_FUNC recv_sub_proccess;
}GDS_RECV_SUB_PROCCESS_WORK;

///GDSライブラリ、NitroDWCに近い関係のワークの構造体
typedef struct _GDS_RAP_WORK{
	SAVEDATA *savedata;
	
	//アプリから渡されるワークポインタ
	///エラーメッセージ描画用のコールバック関数
	GDSRAP_ERROR_WIDE_MSG_FUNC callback_error_msg_wide;
	void *callback_work;	///<コールバック関数呼び出し時、引数として渡す
	
	//アプリから渡される重要データ
	POKE_NET_REQUESTCOMMON_AUTH pokenet_auth;
	int heap_id;
	
	POKE_NET_GDS_STATUS stat;
	POKE_NET_GDS_STATUS laststat;
	
	GDS_RAP_RESPONSE_CALLBACK response_callback;	///データ受信時のコールバック関数
	GDS_RAP_ERROR_INFO error_info;			///<エラー情報格納用ワーク
	
	//※check　暫定
	void *areanaLo;
	
	//送信データ
	GDS_RAP_SEND_WORK send_buf;				///<送信データ
	//受信データ
	void *response;		///<POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_REGIST_xxxxが受信されます
	
	//内部使用ワーク
	int comm_initialize_ok;		///<TRUE:通信ライブラリ初期化済み
	int proccess_tblno;		///<実行するサブプロセスのテーブル番号
	int proccess_seqno;		///<実行中のサブプロセステーブル内のシーケンス番号
	BOOL gdslib_initialize;			///<TRUE:GDSライブラリ初期化済み
	BOOL connect_success;					///<TRUE:ネット接続中
	GDS_RAP_SUB_PROCCESS_WORK sub_work;		///<サブプロセス制御用ワーク

	int ConnectErrorNo;						///< DWC・またはサーバーからのエラー
	int ErrorRet;
	int ErrorCode;
	int ErrorType;
	
	int send_req;	///<POKE_NET_GDS_REQCODE_???(リクエスト無しの場合はPOKE_NET_GDS_REQCODE_LAST)
	int recv_wait_req;///<send_reqの結果受信データ待ち(POKE_NET_GDS_REQCODE_???)
	int send_req_option;	///<POKE_NET_GDS_REQCODE_???でさらに検索を分ける場合のオプション

	MSGDATA_MANAGER *msgman_wifisys;		///<メッセージデータマネージャのポインタ
	WORDSET *wordset;						///<Allocしたメッセージ用単語バッファへのポインタ
	STRBUF *ErrorString;					///<文字列展開バッファ

	GDS_RECV_SUB_PROCCESS_WORK recv_sub_work;	///<データ受信後のサブプロセス用ワーク
	
	u8 div_save_seq;			///<分割セーブ実行シーケンス
	u8 send_before_wait;		///<TRUE:送信前にメッセージを見せる為の一定ウェイト
}GDS_RAP_WORK;


///GDSRAP初期化時に引き渡す引数
typedef struct{
	int gs_profile_id;		///<GSプロファイルID
	
	int heap_id;
	SAVEDATA *savedata;
	
	GDS_RAP_RESPONSE_CALLBACK response_callback;	///データ受信時のコールバック関数
	
	///エラーメッセージ描画用のコールバック関数
	GDSRAP_ERROR_WIDE_MSG_FUNC callback_error_msg_wide;
	void *callback_work;	///<コールバック関数呼び出し時、引数として渡す
}GDSRAP_INIT_DATA;

//==============================================================================
//	定数定義
//==============================================================================
//--------------------------------------------------------------
//	処理実行リクエスト
//--------------------------------------------------------------
typedef enum{
	GDSRAP_PROCESS_REQ_NULL,				///<リクエスト無し
	
//	GDSRAP_PROCESS_REQ_INTERNET_CONNECT,	///<インターネット接続
//	GDSRAP_PROCESS_REQ_INTERNET_CLEANUP,	///<インターネット切断
	
	//以下、システム内部でのみ呼び出し
	GDSRAP_PROCESS_REQ_WIFI_ERROR,			///<WIFI接続エラーが発生した時の強制切断処理
	GDSRAP_PROCESS_REQ_SERVER_ERROR,		///<サーバーサービスエラーが発生した時の強制切断処理
}GDSRAP_PROCESS_REQ;


//==============================================================================
//	外部関数宣言
//==============================================================================
//--------------------------------------------------------------
//	メイン
//--------------------------------------------------------------
extern int GDSRAP_Init(GDS_RAP_WORK *gdsrap, const GDSRAP_INIT_DATA *init_data);
extern int GDSRAP_Main(GDS_RAP_WORK *gdsrap);
extern void GDSRAP_Exit(GDS_RAP_WORK *gdsrap);

//--------------------------------------------------------------
//	送信系
//--------------------------------------------------------------
extern int GDSRAP_Tool_Send_DressupUpload(GDS_RAP_WORK *gdsrap, 
	GDS_PROFILE_PTR gpp, IMC_TELEVISION_SAVEDATA * dress);
extern int GDSRAP_Tool_Send_DressupDownload(GDS_RAP_WORK *gdsrap, int monsno);
extern int GDSRAP_Tool_Send_BoxshotUpload(GDS_RAP_WORK *gdsrap, int category_no, 
	GDS_PROFILE_PTR gpp, const BOX_DATA *boxdata, int tray_number);
extern int GDSRAP_Tool_Send_BoxshotDownload(GDS_RAP_WORK *gdsrap, int category_no);
extern int GDSRAP_Tool_Send_RankingTypeDownload(GDS_RAP_WORK *gdsrap);
extern int GDSRAP_Tool_Send_RankingUpdate(GDS_RAP_WORK *gdsrap, GDS_PROFILE_PTR gpp, 
	GT_RANKING_MYDATA ranking_mydata[]);
extern int GDSRAP_Tool_Send_BattleVideoUpload(GDS_RAP_WORK *gdsrap, GDS_PROFILE_PTR gpp);
extern int GDSRAP_Tool_Send_BattleVideoSearchDownload(GDS_RAP_WORK *gdsrap, 
	u16 monsno, u8 battle_mode, u8 country_code, u8 local_code);
extern int GDSRAP_Tool_Send_BattleVideoNewDownload_ColosseumOnly(GDS_RAP_WORK *gdsrap);
extern int GDSRAP_Tool_Send_BattleVideoNewDownload(GDS_RAP_WORK *gdsrap);
extern int GDSRAP_Tool_Send_BattleVideoFavoriteDownload_Colosseum(GDS_RAP_WORK *gdsrap);
extern int GDSRAP_Tool_Send_BattleVideoFavoriteDownload_Frontier(GDS_RAP_WORK *gdsrap);
extern int GDSRAP_Tool_Send_BattleVideo_DataDownload(GDS_RAP_WORK *gdsrap, u64 data_number);
extern int GDSRAP_Tool_Send_BattleVideo_FavoriteUpload(GDS_RAP_WORK *gdsrap, u64 data_number);

//--------------------------------------------------------------
//	処理系
//--------------------------------------------------------------
extern BOOL GDSRAP_MoveStatusAllCheck(GDS_RAP_WORK *gdsrap);
extern BOOL GDSRAP_ProccessReq(GDS_RAP_WORK *gdsrap, GDSRAP_PROCESS_REQ proccess_req);

//--------------------------------------------------------------
//	エラー
//--------------------------------------------------------------
extern BOOL GDSRAP_ErrorInfoGet(GDS_RAP_WORK *gdsrap, GDS_RAP_ERROR_INFO **error_info);
extern void GDSRAP_ErrorInfoClear(GDS_RAP_WORK *gdsrap);

//--------------------------------------------------------------
//	デバッグ用
//--------------------------------------------------------------
extern void DEBUG_GDSRAP_SaveFlagReset(GDS_RAP_WORK *gdsrap);
extern void DEBUG_GDSRAP_SendVideoProfileFreeWordSet(GDS_RAP_WORK *gdsrap, u16 *set_code);

#endif	//__GDS_RAP_H__
