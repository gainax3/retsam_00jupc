//=============================================================================
/**
 * @file	wifi_note_local.h
 * @brief	WIFI手帳ローカル定義
 * @author	k.ohno
 * @date    2006.4.5
 */
//=============================================================================


#ifndef __WIFI_P2PMATCH_LOCAL_H__
#define __WIFI_P2PMATCH_LOCAL_H__

#include "savedata/wifilist.h"
#include "wifi_p2pmatchroom.h"
#include "gflib/button_man.h"
#include "system/fontoam.h"
#include "application/connect_anm_types.h"

#include "system/bmp_menu.h"
#include "system/bmp_list.h"
#include "system/wordset.h"
#include "system/procsys.h"

#include "savedata/mystatus.h"

#define WIFIP2PMATCH_MEMBER_MAX  (WIFILIST_FRIEND_MAX)
#define WIFIP2PMATCH_DISP_MEMBER_MAX  (5)

// 機能ボタン用定義
#define FUNCBUTTON_NUM	( 7 ) 		// 機能ボタンの数
#define START_WORDPANEL ( 0 )		// 最初の文字入力パネルの番号（０＝ひらがな）

// CLACTで定義しているセルが大きすぎてサブ画面に影響がでてしまうので離してみる
#define NAMEIN_SUB_ACTOR_DISTANCE 	(256*FX32_ONE)

// CellActorに処理させるリソースマネージャの種類の数（＝マルチセル・マルチセルアニメは使用しない）
#define CLACT_RESOURCE_NUM		(  4 )
#define _OAM_NUM			( 5 )

#define FRIENDCODE_MAXLEN     (12)

// マッチングルームにはいてくる人の数（自分も入れて）
#define MATCHROOM_IN_NPCNUM	(32)
#define MATCHROOM_IN_OBJNUM	(MATCHROOM_IN_NPCNUM+1)

// メッセージ表示後のWAIT
#define WIFIP2PMATCH_CORNER_MESSAGE_END_WAIT	( 60 )


// 文字パネルの遷移用
enum{
  WIFIP2PMATCH_MODE_INIT  = 0,
  WIFIP2PMATCH_NORMALCONNECT_YESNO,
  WIFIP2PMATCH_NORMALCONNECT_WAIT,
  WIFIP2PMATCH_DIFFER_MACHINE_INIT,
  WIFIP2PMATCH_DIFFER_MACHINE_NEXT,
  WIFIP2PMATCH_DIFFER_MACHINE_ONEMORE,
  WIFIP2PMATCH_FIRST_YESNO,
  WIFIP2PMATCH_POWEROFF_INIT,
  WIFIP2PMATCH_POWEROFF_YESNO,
  WIFIP2PMATCH_POWEROFF_WAIT,
  WIFIP2PMATCH_RETRY_INIT,        //10
  WIFIP2PMATCH_RETRY_YESNO,
  WIFIP2PMATCH_RETRY_WAIT,
  WIFIP2PMATCH_RETRY,
  WIFIP2PMATCH_CONNECTING_INIT,
  WIFIP2PMATCH_CONNECTING,
  WIFIP2PMATCH_FIRST_ENDMSG,
  WIFIP2PMATCH_FIRST_ENDMSG_WAIT,
  WIFIP2PMATCH_FRIENDLIST_INIT,
  WIFIP2PMATCH_MODE_FRIENDLIST,
  WIFIP2PMATCH_VCHATWIN_WAIT,           //20
  WIFIP2PMATCH_MODE_VCT_CONNECT_INIT2,
  WIFIP2PMATCH_MODE_VCT_CONNECT_INIT,
  WIFIP2PMATCH_MODE_VCT_CONNECT_WAIT,
  WIFIP2PMATCH_MODE_VCT_CONNECT,
  WIFIP2PMATCH_MODE_VCT_CONNECTEND_YESNO,
  WIFIP2PMATCH_MODE_VCT_CONNECTEND_WAIT,
  WIFIP2PMATCH_MODE_VCT_DISCONNECT,
  WIFIP2PMATCH_MODE_BATTLE_DISCONNECT,
  WIFIP2PMATCH_MODE_DISCONNECT,
  WIFIP2PMATCH_MODE_BATTLE_CONNECT_INIT,  //30
  WIFIP2PMATCH_MODE_BATTLE_CONNECT_WAIT,
  WIFIP2PMATCH_MODE_BATTLE_CONNECT,
  WIFIP2PMATCH_MODE_MAIN_MENU,
  WIFIP2PMATCH_MODE_END_WAIT,
  WIFIP2PMATCH_MODE_CHECK_AND_END,
  WIFIP2PMATCH_MODE_SELECT_INIT,
  WIFIP2PMATCH_MODE_SELECT_WAIT,
  WIFIP2PMATCH_MODE_SUBBATTLE_WAIT,
  WIFIP2PMATCH_MODE_SELECT_REL_INIT,
  WIFIP2PMATCH_MODE_SELECT_REL_YESNO,       //40
  WIFIP2PMATCH_MODE_SELECT_REL_WAIT,
  WIFIP2PMATCH_MODE_MATCH_INIT,
  WIFIP2PMATCH_MODE_MATCH_INIT2,
  WIFIP2PMATCH_MODE_MATCH_WAIT,
  WIFIP2PMATCH_MODE_MATCH_LOOP,
  WIFIP2PMATCH_MODE_BCANCEL_YESNO,
  WIFIP2PMATCH_MODE_BCANCEL_WAIT,
  WIFIP2PMATCH_MODE_CALL_INIT,
  WIFIP2PMATCH_MODE_CALL_YESNO,
  WIFIP2PMATCH_MODE_CALL_SEND,
  WIFIP2PMATCH_MODE_CALL_CHECK,             //50
  WIFIP2PMATCH_MODE_MYSTATUS_WAIT,
  WIFIP2PMATCH_MODE_CALL_WAIT,
  WIFIP2PMATCH_MODE_PERSONAL_INIT,
  WIFIP2PMATCH_MODE_PERSONAL_WAIT,
  WIFIP2PMATCH_MODE_PERSONAL_END,
  WIFIP2PMATCH_MODE_EXIT_YESNO,
  WIFIP2PMATCH_MODE_EXIT_WAIT,
  WIFIP2PMATCH_MODE_EXITING,
  WIFIP2PMATCH_MODE_EXIT_END,
  WIFIP2PMATCH_NEXTBATTLE_YESNO,         //60
  WIFIP2PMATCH_NEXTBATTLE_WAIT,
  WIFIP2PMATCH_MODE_VCHAT_NEGO,
  WIFIP2PMATCH_MODE_VCHAT_NEGO_WAIT,
  WIFIP2PMATCH_RECONECTING_WAIT,
  WIFIP2PMATCH_MODE_BCANCEL_YESNO_VCT,
  WIFIP2PMATCH_MODE_BCANCEL_WAIT_VCT,
  WIFIP2PMATCH_PARENT_RESTART,
  WIFIP2PMATCH_FIRST_SAVING,
  WIFIP2PMATCH_MODE_CANCEL_ENABLE_WAIT,
  WIFIP2PMATCH_FIRST_SAVING2,

};


enum{
	WIFIP2PMATCH_SYNCHRONIZE_END=201,
};

// 上下画面指定定義
#define BOTH_LCD	( 2 )
#define MAIN_LCD	( GF_BGL_MAIN_DISP )	// 要は０と
#define SUB_LCD		( GF_BGL_SUB_DISP )		// １なんですが。

// BMPWIN指定
enum{
	BMP_NAME1_S_BG0,
	BMP_NAME2_S_BG0,
	BMP_NAME3_S_BG0,
	BMP_NAME4_S_BG0,
	BMP_NAME5_S_BG0,
	BMP_WIFIP2PMATCH_MAX,
};


//---------------WIFISTATUSデータ

typedef enum{
  WIFI_STATUS_NONE,   // 何も無い	NONEのときは出現もしません
  WIFI_STATUS_VCT,      // VCT中
  WIFI_STATUS_SBATTLE50,      // シングル対戦中
  WIFI_STATUS_SBATTLE100,      // シングル対戦中
  WIFI_STATUS_SBATTLE_FREE,      // シングル対戦中
  WIFI_STATUS_DBATTLE50,      // ダブル対戦中
  WIFI_STATUS_DBATTLE100,      // ダブル対戦中
  WIFI_STATUS_DBATTLE_FREE,      // ダブル対戦中
  WIFI_STATUS_TRADE,          // 交換中
  WIFI_STATUS_SBATTLE50_WAIT,   // シングルLv50対戦募集中
  WIFI_STATUS_SBATTLE100_WAIT,   // シングルLv100対戦募集中
  WIFI_STATUS_SBATTLE_FREE_WAIT,   // シングルLv100対戦募集中
  WIFI_STATUS_DBATTLE50_WAIT,   // ダブルLv50対戦募集中
  WIFI_STATUS_DBATTLE100_WAIT,   // ダブルLv100対戦募集中
  WIFI_STATUS_DBATTLE_FREE_WAIT,   // ダブルLv100対戦募集中
  WIFI_STATUS_TRADE_WAIT,    // 交換募集中
  WIFI_STATUS_LOGIN_WAIT,    // 待機中　ログイン直後はこれ
  
  WIFI_STATUS_DP_UNK,        // DPのUNKNOWN
  
  // プラチナで追加
  WIFI_STATUS_POFIN,          // ポフィン料理中
  WIFI_STATUS_POFIN_WAIT,    // ポフィン募集中
  WIFI_STATUS_FRONTIER,          // フロンティア中
  WIFI_STATUS_FRONTIER_WAIT,    // フロンティア募集中

  WIFI_STATUS_BUCKET,				// バケットゲーム
  WIFI_STATUS_BUCKET_WAIT,			// バケットゲーム募集中
  WIFI_STATUS_BALANCEBALL,		    // 玉乗りゲーム
  WIFI_STATUS_BALANCEBALL_WAIT,		// 玉乗りゲーム募集中
  WIFI_STATUS_BALLOON,				// ばるーんゲーム
  WIFI_STATUS_BALLOON_WAIT,			// ばるーんーム募集中

#ifdef WFP2P_DEBUG_EXON
  WIFI_STATUS_BATTLEROOM,     // バトルルーム中
  WIFI_STATUS_BATTLEROOM_WAIT,// バトルルーム募集中
  WIFI_STATUS_MBATTLE_FREE,     // マルチバトル中
  WIFI_STATUS_MBATTLE_FREE_WAIT,// マルチバトル募集中
#endif

  WIFI_STATUS_PLAY_OTHER,	// WiFiクラブに以外で遊び中
  WIFI_STATUS_UNKNOWN,   // 新たに作ったらこの番号以上になる
};

typedef enum{
  _REGULATION_BATTLE_TOWER     // バトルタワー対戦方式
};


//トータル189バイト送信できるがとりあえず下位プログラムは範囲分だけ送る
#define _POKEMON_NUM   (6)

#define _CANCELENABLE_TIMER (60*30)   // キャンセルになる為のタイマー60min


enum{
	WF_VIEW_STATUS,
	WF_VIEW_VCHAT,
	WF_VIEW_STATUS_NUM,
};

enum{
	WF_USERDISPTYPE_NRML,	// 通常
	WF_USERDISPTYPE_MINI,	// ミニゲーム
	WF_USERDISPTYPE_BLTW,	// バトルタワー
	WF_USERDISPTYPE_BLFT,	// バトルフロンティア
	WF_USERDISPTYPE_BLKS,	// バトルキャッスル
	WF_USERDISPTYPE_BTST,	// バトルステージ
	WF_USERDISPTYPE_BTRT,	// バトルルーレット
	WF_USERDISPTYPE_NUM,
};

// ユーザ表示ボタン数
enum{
	MCV_USERD_BTTN_LEFT = 0,
	MCV_USERD_BTTN_BACK,
	MCV_USERD_BTTN_RIGHT,
	MCV_USERD_BTTN_NUM,
};

typedef struct _WIFI_MACH_STATUS_tag{
  u16 pokemonType[_POKEMON_NUM];
  u16 hasItemType[_POKEMON_NUM];
  u8 version;
  u8 regionCode;
  u8 pokemonZukan;
  u8 status;
  u8 regulation;
  u8 trainer_view;
  u8 sex;
  u8 nation;
  u8 area;
  u8 vchat;
  u8 vchat_org;
} _WIFI_MACH_STATUS;

#define _WIFI_STATUS_MAX_SIZE  (sizeof(_WIFI_MACH_STATUS))		// ここを変えるとDPとの互換性がなくなる

//============================================================================================
//	構造体定義
//============================================================================================

typedef struct {
  _WIFI_MACH_STATUS myMatchStatus;   // 自分のマッチング状態データ
  _WIFI_MACH_STATUS friendMatchStatus[WIFIP2PMATCH_MEMBER_MAX]; // 相手のマッチング状態データ
} TEST_MATCH_WORK;

//-------------------------------------
///	Iconグラフィック
//=====================================
typedef struct {
	void* p_buff;
	NNSG2dScreenData* p_scrn;

	void* p_charbuff;
	NNSG2dCharacterData* p_char;
} WIFIP2PMATCH_ICON;


//-------------------------------------
///	データビューアーデータ
//=====================================
typedef struct {

	// ワードセット
	WORDSET*	p_wordset;			// メッセージ展開用ワークマネージャー
	
	// ボタングラフィック
	void* p_bttnbuff;
	NNSG2dScreenData* p_bttnscrn;

	void* p_userbuff[WF_USERDISPTYPE_NUM];
	NNSG2dScreenData* p_userscrn[WF_USERDISPTYPE_NUM];

	void* p_useretcbuff;
	NNSG2dScreenData* p_useretcscrn;
	
	BUTTON_MAN* p_bttnman;	// ボタン管理システム
	u8 bttnfriendNo[ MATCHROOM_IN_NPCNUM ];
	u8 frame_no;	// 今表示している床のナンバー
	u8 touch_friendNo;	// 触れている友達番号+1
	u8 touch_frame;	// フレーム数
	u8 user_disp;		// ユーザーデータ表示ONOFF
	s8 user_dispno;	// ユーザーDISP内容
	u8 button_on;		// ボタン表示更新
	u8 button_count;	// ボタンフレームカウンタ
	u8 bttn_chg_friendNo;	// ボタンを更新してほしい友達番号
	BOOL bttn_allchg;	// ボタンすべてを更新するか 

	// 表示ビットマップ面
	GF_BGL_BMPWIN	  nameWin[ WCR_MAPDATA_1BLOCKOBJNUM ];
	GF_BGL_BMPWIN	  statusWin[ WCR_MAPDATA_1BLOCKOBJNUM ][ WF_VIEW_STATUS_NUM ];
	GF_BGL_BMPWIN	  userWin;

	// ボタン
	CLACT_U_RES_OBJ_PTR	button_res[ 4 ];
	CLACT_WORK_PTR button_act[MCV_USERD_BTTN_NUM];
	CHAR_MANAGER_ALLOCDATA back_fontoam_cg;
	FONTOAM_OBJ_PTR back_fontoam;	// もどる用FONTOAM
	u32 buttonact_on;			// ボタン動作モード
	u32 touch_button;
	u32 touch_button_event;
	BUTTON_MAN* p_oambttnman;	// ボタン管理システム
} WIFIP2PMATCH_VIEW;



struct _WIFIP2PMATCH_WORK{
  WIFI_LIST* pList;				// セーブデータ内のユーザーとフレンドデータ
  TEST_MATCH_WORK* pMatch;		// サーバーと送受信する自分と友達の状態
  int friendMatchReadCount;		// ビーコンを受信した友達数
  u8 index2No[WIFIP2PMATCH_MEMBER_MAX];	// フレンドナンバー配列
  u8 index2NoBackUp[WIFIP2PMATCH_MEMBER_MAX];	// フレンドナンバー配列バックアップ
  int matchStatusBackup[WIFIP2PMATCH_MEMBER_MAX];// 友達ビーコンデータが変わったかﾁｪｯｸ用
  int matchVchatBackup[WIFIP2PMATCH_MEMBER_MAX];// 友達ビーコンデータが変わったかﾁｪｯｸ用
//  NAMEIN_PARAM*		nameinParam;
  
  // メインリスト用ワーク
  BMPLIST_DATA*   menulist;
  BMPLIST_WORK* lw;		// BMPメニューワーク
  
  BMPLIST_DATA*   submenulist;
  BMPLIST_WORK* sublw;		// BMPメニューワーク
  GF_BGL_INI		*bgl;									// GF_BGL_INI
  SAVEDATA*  pSaveData;
  WORDSET			*WordSet;								// メッセージ展開用ワークマネージャー
  MSGDATA_MANAGER *MsgManager;							// 名前入力メッセージデータマネージャー
  MSGDATA_MANAGER *SysMsgManager;  //
 // STRBUF			*TrainerName[WIFIP2PMATCH_MEMBER_MAX];		// 名前
//  STRBUF			*MsgString;								// メッセージ
  STRBUF*         pExpStrBuf;
  STRBUF			*TalkString;							// 会話メッセージ用
  STRBUF			*TitleString;							// タイトルメッセージ用
//  STRBUF			*MenuString[4];							// メニューメッセージ用
  STRBUF*         pTemp;        // 入力登録時の一時バッファ
  
  int				MsgIndex;								// 終了検出用ワーク
  BMPMENU_WORK* pYesNoWork;
  void* timeWaitWork;			// タイムウエイトアイコンワーク
  CLACT_SET_PTR 			clactSet;								// セルアクターセット
  CLACT_U_EASYRENDER_DATA	renddata;								// 簡易レンダーデータ
  CLACT_U_RES_MANAGER_PTR	resMan[CLACT_RESOURCE_NUM];				// リソースマネージャ
  FONTOAM_SYS_PTR			fontoam;								// フォントOAMシステム
/*
  CLACT_U_RES_OBJ_PTR 	resObjTbl[BOTH_LCD][CLACT_RESOURCE_NUM];// リソースオブジェテーブル
  CLACT_HEADER			clActHeader_m;							// セルアクターヘッダー
  CLACT_HEADER			clActHeader_s;							// セルアクターヘッダー
  CLACT_WORK_PTR			MainActWork[_OAM_NUM];				// セルアクターワークポインタ配列
	//CLACT_WORK_PTR			SubActWork[_OAM_NUM];				// セルアクターワークポインタ配列
//*/

  GF_BGL_BMPWIN			MsgWin;									// 会話ウインドウ
  //   GF_BGL_BMPWIN           MenuWin[4];
  GF_BGL_BMPWIN			MyInfoWin;								// 自分の状態表示
  GF_BGL_BMPWIN			MyInfoWinBack;								// タイトル
  GF_BGL_BMPWIN			SysMsgWin;								// システムウィンドウで描画するもの	えらーや、DWCルールのメッセージ
  GF_BGL_BMPWIN			ListWin;									// フレンドリスト
  GF_BGL_BMPWIN			SubListWin;									// 募集するタイプなどを描画するリスト
  GF_BGL_BMPWIN			MyWin;									// 友達の個人情報表示

   int cancelEnableTimer;   // キャンセル許可になる為のタイマー
  int localTime;
  int seq;									// 現在の文字入力状態（入力OK/アニメ中）など
  int       endSeq;
  int						nextseq;
  int initSeq;
  int						mode;									// 現在最前面の文字パネル
  int						timer;			// 待ち時間や、フラグなどに使用
  MYSTATUS                *pMyStatus;		// 自分のステータス
  u8     mainCur;
  u8     subCur;
  BOOL bInitMessage;		// 初回接続か
  PROC*		subProc;
  int preConnect;			// 新しく来た友達(-1=なし)
  u16 battleCur;			// バトルタイプ選択メニューカーソル
  u16 singleCur[3];			// バトルの詳細部分のメニューカーソル
  u16 bSingle;				// SINGLEバトル　ダブルバトルスイッチ
  u16 keepStatus;			// 接続しようとしたら相手のステータスが変わったときのﾁｪｯｸ用
  u16 keepVChat;			// 接続しようとしたらボイスチャット状態が変わったときのﾁｪｯｸ用
  u16 friendNo;			// 今つながっている友達ナンバー
  BOOL bRetryBattle;
   int vctEnc;
	WIFI_MATCHROOM matchroom;	// マッチングルームワーク
	MCR_MOVEOBJ*	p_matchMoveObj[ MATCHROOM_IN_OBJNUM ];
	WIFIP2PMATCH_ICON icon;	// アイコングラフィック
	WIFIP2PMATCH_VIEW view;	// 友達データビューアー

	BOOL friend_request_wait;	// 友達募集中にTRUEになるフラグ
	
	CONNECT_BG_PALANM cbp;		// Wifi接続画面のBGパレットアニメ制御構造体

	u16 brightness_reset;	// _retryでマスター輝度をVBlankでリセットするため
	u16 friend_num;			// P2Pmatch画面初期化タイミングの友達数
};




#endif  //__WIFI_P2PMATCH_LOCAL_H__

