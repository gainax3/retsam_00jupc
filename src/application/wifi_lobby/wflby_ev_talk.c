//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_ev_talk.c
 *	@brief		会話イベント
 *	@author		tomoya takahashi
 *	@data		2007.12.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#include "system/wordset.h"
#include "system/msgdata.h"
#include "system/fontproc.h"

#include "msgdata/msg.naix"
#include "msgdata/msg_wifi_aisatu.h"
#include "msgdata/msg_wifi_aisatu_japan.h"
#include "msgdata/msg_wifi_aisatu_english.h"
#include "msgdata/msg_wifi_aisatu_france.h"
#include "msgdata/msg_wifi_aisatu_germany.h"
#include "msgdata/msg_wifi_aisatu_italy.h"
#include "msgdata/msg_wifi_aisatu_spain.h"
#include "msgdata/msg_wifi_hiroba.h"

#include "wflby_event.h"
#include "wflby_snd.h"

#include "wflby_ev_talk.h"

//-----------------------------------------------------------------------------
/**
 *					コーディング規約
 *		●関数名
 *				１文字目は大文字それ以降は小文字にする
 *		●変数名
 *				・変数共通
 *						constには c_ を付ける
 *						staticには s_ を付ける
 *						ポインタには p_ を付ける
 *						全て合わさると csp_ となる
 *				・グローバル変数
 *						１文字目は大文字
 *				・関数内変数
 *						小文字と”＿”と数字を使用する 関数の引数もこれと同じ
*/
//-----------------------------------------------------------------------------

#ifdef PM_DEBUG
#define WFLBY_EV_TALK_DEBUG_SEL_MSG	// 順番に質問が出てくる
#endif

// 順番に質問が出てくる
#ifdef WFLBY_EV_TALK_DEBUG_SEL_MSG
static BOOL s_WFLBY_EV_TALK_DEBUG_SEL_MSG_ON		= FALSE;
static s32	s_WFLBY_EV_TALK_DEBUG_SEL_MSG_PAGE	= 0;
#endif


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
/// 話しかけがわシーケンス
//=====================================
enum {
	WFLBY_EV_TALK_A_SEQ_INIT,			// 初期化
	WFLBY_EV_TALK_A_SEQ_REQ,			// リクエスト
	WFLBY_EV_TALK_A_SEQ_ANSWAIT,		// 回答待ち
	WFLBY_EV_TALK_A_SEQ_AISATUMY,		// あいさつ　自分の時間
	WFLBY_EV_TALK_A_SEQ_AISATUYOU,		// あいさつ	 相手の時間
	WFLBY_EV_TALK_A_SEQ_SYNC_RECV,		// 会話状態同期
	WFLBY_EV_TALK_A_SEQ_AISATUFOLLOW,	// あいさつフォロー
	WFLBY_EV_TALK_A_SEQ_AISATUEND,		// あいさつの最後のメッセージ
	WFLBY_EV_TALK_A_SEQ_AISATUEND2,		// あいさつの最後のメッセージもう１回話してるとき
	WFLBY_EV_TALK_A_SEQ_SEL_GT_FIR,		// ガジェット、会話セレクト	１回目
	WFLBY_EV_TALK_A_SEQ_SEL_GT_SEC,		// ガジェット、会話セレクト	２回目以降
	WFLBY_EV_TALK_A_SEQ_SEL_GT00,		// ガジェット、会話セレクト
	WFLBY_EV_TALK_A_SEQ_SEL_GT00WAIT,	// ガジェット、会話セレクト　選択待ち
	WFLBY_EV_TALK_A_SEQ_SEND_G,			// ガジェットもらう
	WFLBY_EV_TALK_A_SEQ_SEND_G_GETCHK,	// ガジェットもらうか選択
	WFLBY_EV_TALK_A_SEQ_SEND_G_GETCHKWAIT,// ガジェットもらうか選択まち
	WFLBY_EV_TALK_A_SEQ_SEND_G_GET,		// ガジェットもらう
	WFLBY_EV_TALK_A_SEQ_SEND_G_RECV_B_SE,// ガジェットもらうBの受信確認
	WFLBY_EV_TALK_A_SEQ_SEND_G_RECV_B,	// ガジェットもらうBの受信確認
	WFLBY_EV_TALK_A_SEQ_SEND_TALK,		// 会話することを送信
	WFLBY_EV_TALK_A_SEQ_SEL_T_K,		// 会話項目選択
	WFLBY_EV_TALK_A_SEQ_SEL_T_K_WAIT,	// 会話項目選択待ち
	WFLBY_EV_TALK_A_SEQ_SEL_T_M,		// 自分のこと選択
	WFLBY_EV_TALK_A_SEQ_SEL_T_M_SEL,	// 自分のこと選択
	WFLBY_EV_TALK_A_SEQ_SEL_T_M_WAIT,	// 自分のこと選択
	WFLBY_EV_TALK_A_SEQ_SEND_T,			// 会話選択情報送信
	WFLBY_EV_TALK_A_SEQ_SEND_T00,		// 会話選択情報送信
	WFLBY_EV_TALK_A_SEQ_SEND_TWAIT,		// 会話選択情報送信
	WFLBY_EV_TALK_A_SEQ_RECV_BT,		// 会話選択情報受信
	WFLBY_EV_TALK_A_SEQ_MSG_BT,			// 応答会話情報メッセージ表示
	WFLBY_EV_TALK_A_SEQ_MSG_ABT_EQ,		// 選択項目が同じとき
	WFLBY_EV_TALK_A_SEQ_MSG_BT_END,		// 応答メッセージ最後
	WFLBY_EV_TALK_A_SEQ_ERR_ENDMSG,		// エラー終了
	WFLBY_EV_TALK_A_SEQ_ENDMSG,			// 通常終了
	WFLBY_EV_TALK_A_SEQ_ANSNGMSG,		// 応答なし終了
	WFLBY_EV_TALK_A_SEQ_SELNGMSG,		// B選択できない終了
	WFLBY_EV_TALK_A_SEQ_WAIT,			// ウエイト
	WFLBY_EV_TALK_A_SEQ_MSGWAIT,		// メッセージの終了ウエイト
	WFLBY_EV_TALK_A_SEQ_END,			// 終了
} ;

//-------------------------------------
/// 話しかけられ側シーケンス
//=====================================
enum {
	WFLBY_EV_TALK_B_SEQ_INIT,		// 初期化
	WFLBY_EV_TALK_B_SEQ_AISATUYOU,	// あいさつ　あいての時間
	WFLBY_EV_TALK_B_SEQ_AISATUMY,	// あいさつ	自分の時間
	WFLBY_EV_TALK_B_SEQ_SYNC_RECV,		// 会話状態同期
	WFLBY_EV_TALK_B_SEQ_AISATUFOLLOW,	// あいさつフォロー
	WFLBY_EV_TALK_B_SEQ_AISATUEND,		// あいさつの最後のメッセージ
	WFLBY_EV_TALK_B_SEQ_AISATUEND2,		// あいさつの最後のメッセージもう１回話してるとき
	WFLBY_EV_TALK_B_SEQ_RECV_A_FST,		// 相手の選択内容待ち	最初
	WFLBY_EV_TALK_B_SEQ_RECV_A_SEC,	// 相手の選択内容待ち ２回目以降
	WFLBY_EV_TALK_B_SEQ_RECV_A,		// 相手の選択内容待ち
	WFLBY_EV_TALK_B_SEQ_MSG_G00,		// ガジェットメッセージ
	WFLBY_EV_TALK_B_SEQ_MSG_G01,		// ガジェットメッセージ
	WFLBY_EV_TALK_B_SEQ_MSG_G02,		// ガジェットメッセージ
	WFLBY_EV_TALK_B_SEQ_MSG_G03,		// ガジェットメッセージ
	WFLBY_EV_TALK_B_SEQ_MSG_G_EQ,	// ガジェットが一緒メッセージ
	WFLBY_EV_TALK_B_SEQ_MSG_AT00,	// A会話開始メッセージ表示
	WFLBY_EV_TALK_B_SEQ_RECV_A_SEL,	// Aの会話内容を取得
	WFLBY_EV_TALK_B_SEQ_MSG_AT01,	// Aの選択事項表示
	WFLBY_EV_TALK_B_SEQ_MSG_AT02,	// Aの選択つなぎ表示
	WFLBY_EV_TALK_B_SEQ_MSG_AT03,	// Aの選択選んだこと表示
	WFLBY_EV_TALK_B_SEQ_MSG_AT04,	// Aの選択さらにつなぎ表示
	WFLBY_EV_TALK_B_SEQ_SEL_T_M,	// 自分のを選択
	WFLBY_EV_TALK_B_SEQ_SEND_T,		// 自分の選択を送信
	WFLBY_EV_TALK_B_SEQ_MSG_BT,		// 自分の選択の表示
	WFLBY_EV_TALK_B_SEQ_MSG_ABT_EQ,	// 選択項目が同じとき
	WFLBY_EV_TALK_B_SEQ_MSG_AT_END,	// 会話終了メッセージ
	WFLBY_EV_TALK_B_SEQ_ERR_ENDMSG,	// エラー終了
	WFLBY_EV_TALK_B_SEQ_ENDMSG,		// 通常終了
	WFLBY_EV_TALK_B_SEQ_NONEENDMSG,	// こたえないとき終了
	WFLBY_EV_TALK_B_SEQ_WAIT,			// ウエイト
	WFLBY_EV_TALK_B_SEQ_MSGWAIT,		// メッセージの終了ウエイト
	WFLBY_EV_TALK_B_SEQ_END,		// 終了
} ;


//-------------------------------------
///	文字バッファ数
//=====================================
#define WFLBY_EV_TALK_STRBUF_NUM	( 128 )

//-------------------------------------
///	メッセージ表示ウエイト
//=====================================
#define WFLBY_EV_TALK_MSG_WAIT		( 128 )


//-------------------------------------
///	世界の挨拶種類
//=====================================
#define WFLBY_EV_TALK_LANG_NUM	( LANG_SPAIN+1 )


//-------------------------------------
///	会話データ
//=====================================
enum {
	WFLBY_EV_TALK_TYPE_AISATSUSYNC,	// "挨拶後の同期"
	WFLBY_EV_TALK_TYPE_TALK_BEFORE,	// "会話項目受信完了	B側のダミー返答"
	WFLBY_EV_TALK_TYPE_GADGET_END,	// "会話項目受信完了	B側のダミー返答"
	WFLBY_EV_TALK_TYPE_GADGET,		// "ガジェット交換"
	WFLBY_EV_TALK_TYPE_GADGET_EQ,	// "ガジェット一緒"
	WFLBY_EV_TALK_TYPE_TALK_START,	// "会話開始"

	WFLBY_EV_TALK_TYPE_TALK_COL_00,
	WFLBY_EV_TALK_TYPE_TALK_COL_01,
	WFLBY_EV_TALK_TYPE_TALK_COL_02,
	WFLBY_EV_TALK_TYPE_TALK_COL_03,

	WFLBY_EV_TALK_TYPE_TALK_POKE_00,
	WFLBY_EV_TALK_TYPE_TALK_POKE_01,
	WFLBY_EV_TALK_TYPE_TALK_POKE_02,
	WFLBY_EV_TALK_TYPE_TALK_POKE_03,

	WFLBY_EV_TALK_TYPE_TALK_WET_00,
	WFLBY_EV_TALK_TYPE_TALK_WET_01,
	WFLBY_EV_TALK_TYPE_TALK_WET_02,
	WFLBY_EV_TALK_TYPE_TALK_WET_03,

	WFLBY_EV_TALK_TYPE_TALK_ACT_00,
	WFLBY_EV_TALK_TYPE_TALK_ACT_01,
	WFLBY_EV_TALK_TYPE_TALK_ACT_02,
	WFLBY_EV_TALK_TYPE_TALK_ACT_03,

	WFLBY_EV_TALK_TYPE_TALK_YOT_00,
	WFLBY_EV_TALK_TYPE_TALK_YOT_01,
	WFLBY_EV_TALK_TYPE_TALK_YOT_02,
	WFLBY_EV_TALK_TYPE_TALK_YOT_03,

	WFLBY_EV_TALK_TYPE_TALK_TRA_00,
	WFLBY_EV_TALK_TYPE_TALK_TRA_01,
	WFLBY_EV_TALK_TYPE_TALK_TRA_02,
	WFLBY_EV_TALK_TYPE_TALK_TRA_03,

	WFLBY_EV_TALK_TYPE_TALK_LOOK_00,
	WFLBY_EV_TALK_TYPE_TALK_LOOK_01,
	WFLBY_EV_TALK_TYPE_TALK_LOOK_02,
	WFLBY_EV_TALK_TYPE_TALK_LOOK_03,

	WFLBY_EV_TALK_TYPE_TALK_NEED_00,
	WFLBY_EV_TALK_TYPE_TALK_NEED_01,
	WFLBY_EV_TALK_TYPE_TALK_NEED_02,
	WFLBY_EV_TALK_TYPE_TALK_NEED_03,

	WFLBY_EV_TALK_TYPE_TALK_BAD_00,
	WFLBY_EV_TALK_TYPE_TALK_BAD_01,
	WFLBY_EV_TALK_TYPE_TALK_BAD_02,
	WFLBY_EV_TALK_TYPE_TALK_BAD_03,

	WFLBY_EV_TALK_TYPE_TALK_PLAY_00,
	WFLBY_EV_TALK_TYPE_TALK_PLAY_01,
	WFLBY_EV_TALK_TYPE_TALK_PLAY_02,
	WFLBY_EV_TALK_TYPE_TALK_PLAY_03,

	WFLBY_EV_TALK_TYPE_TALK_GAME_00,
	WFLBY_EV_TALK_TYPE_TALK_GAME_01,
	WFLBY_EV_TALK_TYPE_TALK_GAME_02,
	WFLBY_EV_TALK_TYPE_TALK_GAME_03,

	WFLBY_EV_TALK_TYPE_TALK_HAYARI_00,
	WFLBY_EV_TALK_TYPE_TALK_HAYARI_01,
	WFLBY_EV_TALK_TYPE_TALK_HAYARI_02,
	WFLBY_EV_TALK_TYPE_TALK_HAYARI_03,

	WFLBY_EV_TALK_TYPE_TALK_TOKUI_00,
	WFLBY_EV_TALK_TYPE_TALK_TOKUI_01,
	WFLBY_EV_TALK_TYPE_TALK_TOKUI_02,
	WFLBY_EV_TALK_TYPE_TALK_TOKUI_03,

	WFLBY_EV_TALK_TYPE_TALK_YAMERAREN_00,
	WFLBY_EV_TALK_TYPE_TALK_YAMERAREN_01,
	WFLBY_EV_TALK_TYPE_TALK_YAMERAREN_02,
	WFLBY_EV_TALK_TYPE_TALK_YAMERAREN_03,

	WFLBY_EV_TALK_TYPE_TALK_KODAWARI_00,
	WFLBY_EV_TALK_TYPE_TALK_KODAWARI_01,
	WFLBY_EV_TALK_TYPE_TALK_KODAWARI_02,
	WFLBY_EV_TALK_TYPE_TALK_KODAWARI_03,

	WFLBY_EV_TALK_TYPE_TALK_NANISURU_00,
	WFLBY_EV_TALK_TYPE_TALK_NANISURU_01,
	WFLBY_EV_TALK_TYPE_TALK_NANISURU_02,
	WFLBY_EV_TALK_TYPE_TALK_NANISURU_03,

	WFLBY_EV_TALK_TYPE_TALK_HOME_00,
	WFLBY_EV_TALK_TYPE_TALK_HOME_01,
	WFLBY_EV_TALK_TYPE_TALK_HOME_02,
	WFLBY_EV_TALK_TYPE_TALK_HOME_03,

	WFLBY_EV_TALK_TYPE_TALK_FRIPLY_00,
	WFLBY_EV_TALK_TYPE_TALK_FRIPLY_01,
	WFLBY_EV_TALK_TYPE_TALK_FRIPLY_02,
	WFLBY_EV_TALK_TYPE_TALK_FRIPLY_03,

	WFLBY_EV_TALK_TYPE_TALK_PTAISE_00,
	WFLBY_EV_TALK_TYPE_TALK_PTAISE_01,
	WFLBY_EV_TALK_TYPE_TALK_PTAISE_02,
	WFLBY_EV_TALK_TYPE_TALK_PTAISE_03,

	WFLBY_EV_TALK_TYPE_TALK_DONNAP_00,
	WFLBY_EV_TALK_TYPE_TALK_DONNAP_01,
	WFLBY_EV_TALK_TYPE_TALK_DONNAP_02,
	WFLBY_EV_TALK_TYPE_TALK_DONNAP_03,
	
	WFLBY_EV_TALK_TYPE_TALK_SIAWASE_00,
	WFLBY_EV_TALK_TYPE_TALK_SIAWASE_01,
	WFLBY_EV_TALK_TYPE_TALK_SIAWASE_02,
	WFLBY_EV_TALK_TYPE_TALK_SIAWASE_03,

	WFLBY_EV_TALK_TYPE_TALK_KOMARU_00,
	WFLBY_EV_TALK_TYPE_TALK_KOMARU_01,
	WFLBY_EV_TALK_TYPE_TALK_KOMARU_02,
	WFLBY_EV_TALK_TYPE_TALK_KOMARU_03,

	WFLBY_EV_TALK_TYPE_TALK_GANBARU_00,
	WFLBY_EV_TALK_TYPE_TALK_GANBARU_01,
	WFLBY_EV_TALK_TYPE_TALK_GANBARU_02,
	WFLBY_EV_TALK_TYPE_TALK_GANBARU_03,
	
	WFLBY_EV_TALK_TYPE_TALK_SPORTS_00,
	WFLBY_EV_TALK_TYPE_TALK_SPORTS_01,
	WFLBY_EV_TALK_TYPE_TALK_SPORTS_02,
	WFLBY_EV_TALK_TYPE_TALK_SPORTS_03,

	WFLBY_EV_TALK_TYPE_TALK_WORKS_00,
	WFLBY_EV_TALK_TYPE_TALK_WORKS_01,
	WFLBY_EV_TALK_TYPE_TALK_WORKS_02,
	WFLBY_EV_TALK_TYPE_TALK_WORKS_03,

	WFLBY_EV_TALK_TYPE_TALK_ZIMAN_00,
	WFLBY_EV_TALK_TYPE_TALK_ZIMAN_01,
	WFLBY_EV_TALK_TYPE_TALK_ZIMAN_02,
	WFLBY_EV_TALK_TYPE_TALK_ZIMAN_03,

	WFLBY_EV_TALK_TYPE_TALK_DONNA_00,
	WFLBY_EV_TALK_TYPE_TALK_DONNA_01,
	WFLBY_EV_TALK_TYPE_TALK_DONNA_02,
	WFLBY_EV_TALK_TYPE_TALK_DONNA_03,

	WFLBY_EV_TALK_TYPE_TALK_DONNAOTO_00,
	WFLBY_EV_TALK_TYPE_TALK_DONNAOTO_01,
	WFLBY_EV_TALK_TYPE_TALK_DONNAOTO_02,
	WFLBY_EV_TALK_TYPE_TALK_DONNAOTO_03,

	WFLBY_EV_TALK_TYPE_TALK_DONNANIOI_00,
	WFLBY_EV_TALK_TYPE_TALK_DONNANIOI_01,
	WFLBY_EV_TALK_TYPE_TALK_DONNANIOI_02,
	WFLBY_EV_TALK_TYPE_TALK_DONNANIOI_03,

	WFLBY_EV_TALK_TYPE_TALK_KININARU_00,
	WFLBY_EV_TALK_TYPE_TALK_KININARU_01,
	WFLBY_EV_TALK_TYPE_TALK_KININARU_02,
	WFLBY_EV_TALK_TYPE_TALK_KININARU_03,
	

	WFLBY_EV_TALK_TYPE_TALK_NONE,			//  "わからないを選択"

	WFLBY_EV_TALK_TYPE_NUM,
} ;


//-------------------------------------
///	最終データ追加メッセージ選択
//=====================================
enum {
	WFLBY_EV_TALK_LIST_END_NONE,
	WFLBY_EV_TALK_LIST_END_YAMERU,
	WFLBY_EV_TALK_LIST_END_WAKARANAI,
} ;




//-------------------------------------
///	トピック前のお姉さん
//=====================================
enum {
	WFLBY_EV_DEF_PLAYER_TOPIC_INIT,		// 初期化
	WFLBY_EV_DEF_PLAYER_TOPIC_PARADE,	// PARADEメッセージ
	WFLBY_EV_DEF_PLAYER_TOPIC_CLOSE,	// クローズ前メッセージ
	WFLBY_EV_DEF_PLAYER_TOPIC_MSG00,	// ようこそ
	WFLBY_EV_DEF_PLAYER_TOPIC_MSG01,	// どちらについて・・・
	WFLBY_EV_DEF_PLAYER_TOPIC_LISTON,	// リスト表示
	WFLBY_EV_DEF_PLAYER_TOPIC_LISTWAIT,	// リスト待ち
	WFLBY_EV_DEF_PLAYER_TOPIC_MSGGADGET,// ガジェット
	WFLBY_EV_DEF_PLAYER_TOPIC_MSGMINI,		// ミニゲーム
	WFLBY_EV_DEF_PLAYER_TOPIC_MSGMINI_SEL,	// ミニゲームさらに選択
	WFLBY_EV_DEF_PLAYER_TOPIC_MSGMINI_BS,	// タマ投げ
	WFLBY_EV_DEF_PLAYER_TOPIC_MSGMINI_BB,	// たまのり
	WFLBY_EV_DEF_PLAYER_TOPIC_MSGMINI_BL,	// ふうせんわり
	WFLBY_EV_DEF_PLAYER_TOPIC_MSGWLD,	// 世界時計
	WFLBY_EV_DEF_PLAYER_TOPIC_MSGTOP,	// ニュース
	WFLBY_EV_DEF_PLAYER_TOPIC_MSGBOAD,	// 足跡ボード
	WFLBY_EV_DEF_PLAYER_TOPIC_MSGEVE,	// イベント
	WFLBY_EV_DEF_PLAYER_TOPIC_MSGEVE_SEL,		// イベントさらに選択
	WFLBY_EV_DEF_PLAYER_TOPIC_MSGEVE_LIGHTING,	// イベント　ライティング
	WFLBY_EV_DEF_PLAYER_TOPIC_MSGEVE_FIRE,		// イベント　はなび
	WFLBY_EV_DEF_PLAYER_TOPIC_MSGEVE_PARADE,	// イベント　パレード
	WFLBY_EV_DEF_PLAYER_TOPIC_MSGANKETO,// アンケート
	WFLBY_EV_DEF_PLAYER_TOPIC_MSGEND,	// オワリ
	WFLBY_EV_DEF_PLAYER_TOPIC_MSGWAIT,	// メッセージ終了待ち
	WFLBY_EV_DEF_PLAYER_TOPIC_END,	
} ;
#define WFLBY_EV_DEF_PLAYER_TOPIC_LISTWIN_X		( 16 )
#define WFLBY_EV_DEF_PLAYER_TOPIC_LISTWIN_Y		( 1 )
#define WFLBY_EV_DEF_PLAYER_TOPIC_LISTWIN_SIZX	( 15 )

//-------------------------------------
///	トピック前のお兄さん
//=====================================
enum {
	WFLBY_EV_DEF_PLAYER_TOPIC_PLAYED_INIT,		// 初期化
	WFLBY_EV_DEF_PLAYER_TOPIC_PLAYED_MSGWAIT,	// メッセージ表示完了待ち
	WFLBY_EV_DEF_PLAYER_TOPIC_PLAYED_END,	
} ;






//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	ビットマップリスト
//=====================================
typedef struct {
	// ビットマップリスト専用
	BMPLIST_HEADER		bmplist;
	u32					bmplist_num;	// ビットマップリスト数
} WFLBY_EV_TALK_BMPLIST;


//-------------------------------------
///	相手からダミーメッセージが帰ってきたら、
//  メッセージを送信するシステム
//=====================================
typedef struct {
	u16		trans_on;
	u16		trans_data;
	u16		wait_seq;
	u16		start_seq;
	u32		recv_talk_seq;
} WFLBY_EV_TALK_RWSEND;



//-------------------------------------
///	会話イベントワーク
//=====================================
typedef struct {
	u8					sel_k;			// 選んだ項目
	u8					sel_k_data;		// 選んだデータ
	u8					recv_k_data;	// 受信した選びデータ
	u8					talk_idx;		// 会話ターゲット
	u8					my_timezone;	// 自分の時間帯
	u8					you_timezone;	// 相手の時間帯
	u8					wait;			// ウエイト
	u8					next_seq;		// ウエイト後のシーケンス
	u16					my_idx;			// 自分のインデックス
	u16					err_chk;		// エラーチェックするか
	u8					gadget_eq;		// ガジェットが一緒なのかフラグ
	u8					sel_wait_flag;	// 選択ウエイトフラグ
	s16					sel_wait;		// 選択ウエイト
	WFLBY_3DPERSON*		p_npc;			// NPC

	WFLBY_EV_TALK_BMPLIST bmplist;

	WFLBY_EV_TALK_RWSEND	rw_send;	// ダミーメッセージを受信してから送信するシステム

	// 履歴にデータをセットするためのデータ
	u16	rireki_flag;
	u16	rireki_actno;
	s32 rireki_userid;

	BOOL b_count_add;	// 会話カウンタを1足したのか


	// 退室したユーザのプロフィールを参照する可能性が出るので廃止
//	const WFLBY_USER_PROFILE* cp_mypr;
//	const WFLBY_USER_PROFILE* cp_youpr;

} WFLBY_EV_TALK_WK;




//-------------------------------------
///	主人公初回入室イベントワーク
//=====================================
typedef struct {
	u32						ret_seq;
	WFLBY_EV_TALK_BMPLIST	bmplist;
} WFLBY_EV_DEF_PLAYER_WFTOPIC_WK;

//-----------------------------------------------------------------------------
/**
 *			リストデータ
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	基本ヘッダー
//=====================================
static const BMPLIST_HEADER sc_WFLBY_EV_TALK_HEADER = {
	NULL,
	NULL,
	NULL,
	NULL,

	0,	//リスト項目数
	6,	//表示最大項目数
	0,								//ラベル表示Ｘ座標
	8,								//項目表示Ｘ座標
	0,								//カーソル表示Ｘ座標
	0,								//表示Ｙ座標
	FBMP_COL_BLACK,					//表示文字色
	FBMP_COL_WHITE,					//表示背景色
	FBMP_COL_BLK_SDW,				//表示文字影色
	0,								//文字間隔Ｘ
	16,								//文字間隔Ｙ
	BMPLIST_NO_SKIP,				//ページスキップタイプ
	FONT_SYSTEM,					//文字指定(本来は u8 だけど、そんなに作らないと思うので)
	0,								//ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)

	NULL
};




//-------------------------------------
///	ガジェットか会話か選択
//=====================================
enum{	
	WFLBY_EV_TALK_LIST_SEL_GT_GAGET,
	WFLBY_EV_TALK_LIST_SEL_GT_TALK,
	WFLBY_EV_TALK_LIST_SEL_GT_END,
	WFLBY_EV_TALK_LIST_SEL_GT_NUM,
};

//-------------------------------------
///	ガジェットもらうか選択
//=====================================
enum{	
	WFLBY_EV_TALK_LIST_SEL_G_GET,
	WFLBY_EV_TALK_LIST_SEL_G_END,
	WFLBY_EV_TALK_LIST_SEL_G_NUM,
};

//-------------------------------------
///	会話項目選択
//=====================================
enum{	
	WFLBY_EV_TALK_LIST_SEL_TK_COL,		// 色
	WFLBY_EV_TALK_LIST_SEL_TK_POKE,		// ポケ
	WFLBY_EV_TALK_LIST_SEL_TK_WEA,		// 天気
	WFLBY_EV_TALK_LIST_SEL_TK_ACT,		// なにしてた
	WFLBY_EV_TALK_LIST_SEL_TK_YOTE,		// 予定
	WFLBY_EV_TALK_LIST_SEL_TK_TRA,		// りょこう
	WFLBY_EV_TALK_LIST_SEL_TK_LOOK,		// にてる
	WFLBY_EV_TALK_LIST_SEL_TK_NEED,		// ほしいもの
	WFLBY_EV_TALK_LIST_SEL_TK_BAD,		// にがてなもの
	WFLBY_EV_TALK_LIST_SEL_TK_PLAY,		// あそびにいくなら
	WFLBY_EV_TALK_LIST_SEL_TK_GAME,		// とくいなゲーム
	WFLBY_EV_TALK_LIST_SEL_TK_HAYARI,	// はやってることは
	WFLBY_EV_TALK_LIST_SEL_TK_TOKUI,	// とくいなことは
	WFLBY_EV_TALK_LIST_SEL_TK_YEMERAREN,// やめれないこと
	WFLBY_EV_TALK_LIST_SEL_TK_KODAWARI,	// こだわってること
	WFLBY_EV_TALK_LIST_SEL_TK_NANISURU,	// このあとなにする
	WFLBY_EV_TALK_LIST_SEL_TK_HOME,		// 家の周りになにがある
	WFLBY_EV_TALK_LIST_SEL_TK_FRIPLY,	// 友達となにしてあそぶ
	WFLBY_EV_TALK_LIST_SEL_TK_PTAISE,	// たいせつなひとはいる
	WFLBY_EV_TALK_LIST_SEL_TK_DONNAP,	// どんなひとっていわれる
	WFLBY_EV_TALK_LIST_SEL_TK_SIAWASE,	// しあわせってどんなとき
	WFLBY_EV_TALK_LIST_SEL_TK_KOMARU,	// こまってることはある
	WFLBY_EV_TALK_LIST_SEL_TK_GANBARU,	// がんばってることはある
	WFLBY_EV_TALK_LIST_SEL_TK_SPORTS,	// とくいなスポーツ
	WFLBY_EV_TALK_LIST_SEL_TK_WORKS,	// 憧れの職業
	WFLBY_EV_TALK_LIST_SEL_TK_ZIMAN,	// すんでるところの自慢
	WFLBY_EV_TALK_LIST_SEL_TK_DONNA,	// すんでるところはどんなとこ
	WFLBY_EV_TALK_LIST_SEL_TK_DONNAOTO,	// どんなおとしてる
	WFLBY_EV_TALK_LIST_SEL_TK_DONNANIOI,// どんなにおいしてる
	WFLBY_EV_TALK_LIST_SEL_TK_KININARU,	// 今きになってることある
	
//	WFLBY_EV_TALK_LIST_SEL_TK_END,		// やめる
	WFLBY_EV_TALK_LIST_SEL_TK_NUM,
};
#define WFLBY_EV_TALK_TK_SEL_NUM			( 4 )
#define WFLBY_EV_TALK_TK_LISTWIN_X			( 8 )
#define WFLBY_EV_TALK_TK_LISTWIN_Y			( 3 )
#define WFLBY_EV_TALK_TK_LISTWIN_SIZX			( 23 )

//-------------------------------------
///	会話内容
//=====================================
enum{	
	WFLBY_EV_TALK_LIST_SEL_TK_S_00,	
	WFLBY_EV_TALK_LIST_SEL_TK_S_01,	
	WFLBY_EV_TALK_LIST_SEL_TK_S_02,	
	WFLBY_EV_TALK_LIST_SEL_TK_S_04,	
	WFLBY_EV_TALK_LIST_SEL_TK_S_MAX,			// A[項目数]	B[わからない]
	WFLBY_EV_TALK_LIST_SEL_TK_S_B_MAX,			// 項目数
};


//-------------------------------------
///	トピックお姉さんリスト
//=====================================
enum {
	WFLBY_EV_TOPIC_LIST_SEL_GADGET,
	WFLBY_EV_TOPIC_LIST_SEL_MINI,
	WFLBY_EV_TOPIC_LIST_SEL_WLD,
	WFLBY_EV_TOPIC_LIST_SEL_TOP,
	WFLBY_EV_TOPIC_LIST_SEL_FOOT,
	WFLBY_EV_TOPIC_LIST_SEL_ANKETO,
	WFLBY_EV_TOPIC_LIST_SEL_TIME,	
	WFLBY_EV_TOPIC_LIST_SEL_END,	
	WFLBY_EV_TOPIC_LIST_SEL_NUM,	
} ;
// イベントリスト
enum {
	WFLBY_EV_TOPIC_LIST_SEL_EVE_LIGHTING,
	WFLBY_EV_TOPIC_LIST_SEL_EVE_FIRE,
	WFLBY_EV_TOPIC_LIST_SEL_EVE_PARADE,
	WFLBY_EV_TOPIC_LIST_SEL_EVE_END,
	WFLBY_EV_TOPIC_LIST_SEL_EVE_NUM,	
} ;
// ミニゲームリスト
enum {
	WFLBY_EV_TOPIC_LIST_SEL_MG_BALLSLOW,
	WFLBY_EV_TOPIC_LIST_SEL_MG_BALANCE,
	WFLBY_EV_TOPIC_LIST_SEL_MG_BALLOON,
	WFLBY_EV_TOPIC_LIST_SEL_MG_END,
	WFLBY_EV_TOPIC_LIST_SEL_MG_NUM,	
} ;




//-----------------------------------------------------------------------------
/**
 *		VIPメッセージデータテーブル
 */
//-----------------------------------------------------------------------------
typedef struct{
	u16 normal_msg;
	u16 vip_msg;
} WFLBY_VIP_MSGDATA;
static const WFLBY_VIP_MSGDATA sc_WFLBY_VIP_MSGTBL[] = {
	{ msg_hiroba_aisatu_03_01, msg_vip_aisatu_03_01 },
	{ msg_hiroba_aisatu_03_02, msg_vip_aisatu_03_02 },
	{ msg_hiroba_aisatu_04_02, msg_vip_aisatu_04_02 },
	{ msg_hiroba_aisatu_04_03, msg_vip_aisatu_04_03 },
	{ msg_hiroba_osyaberi_00_01, msg_vip_osyaberi_00_01 },
	{ msg_hiroba_osyaberi_00_02, msg_vip_osyaberi_00_02 },
	{ msg_hiroba_osyaberi_00_03, msg_vip_osyaberi_00_03 },
	{ msg_hiroba_osyaberi_00_04, msg_vip_osyaberi_00_04 },
	{ msg_hiroba_osyaberi_00_05, msg_vip_osyaberi_00_05 },
	{ msg_hiroba_osyaberi_00_06, msg_vip_osyaberi_00_06 },
	{ msg_hiroba_osyaberi_00_07, msg_vip_osyaberi_00_07 },
	{ msg_hiroba_osyaberi_00_08, msg_vip_osyaberi_00_08 },
	{ msg_hiroba_osyaberi_00_09, msg_vip_osyaberi_00_09 },
	{ msg_hiroba_osyaberi_01_01, msg_vip_osyaberi_01_01 },
	{ msg_hiroba_osyaberi_01_02, msg_vip_osyaberi_01_02 },
	{ msg_hiroba_osyaberi_01_03, msg_vip_osyaberi_01_03 },
	{ msg_hiroba_osyaberi_01_04, msg_vip_osyaberi_01_04 },
	{ msg_hiroba_osyaberi_01_05, msg_vip_osyaberi_01_05 },
	{ msg_hiroba_osyaberi_01_06, msg_vip_osyaberi_01_06 },
	{ msg_hiroba_osyaberi_01_07, msg_vip_osyaberi_01_07 },
	{ msg_hiroba_osyaberi_01_08, msg_vip_osyaberi_01_08 },
	{ msg_hiroba_osyaberi_01_09, msg_vip_osyaberi_01_09 },
	{ msg_hiroba_osyaberi_01_10, msg_vip_osyaberi_01_10 },
	{ msg_hiroba_osyaberi_02_01, msg_vip_osyaberi_02_01 },
	{ msg_hiroba_osyaberi_02_02, msg_vip_osyaberi_02_02 },
	{ msg_hiroba_osyaberi_02_03, msg_vip_osyaberi_02_03 },
	{ msg_hiroba_osyaberi_02_04, msg_vip_osyaberi_02_04 },
	{ msg_hiroba_osyaberi_02_05, msg_vip_osyaberi_02_05 },
	{ msg_hiroba_osyaberi_02_06, msg_vip_osyaberi_02_06 },
	{ msg_hiroba_osyaberi_02_07, msg_vip_osyaberi_02_07 },
	{ msg_hiroba_osyaberi_02_08, msg_vip_osyaberi_02_08 },
	{ msg_hiroba_osyaberi_02_09, msg_vip_osyaberi_02_09 },
	{ msg_hiroba_osyaberi_02_10, msg_vip_osyaberi_02_10 },
	{ msg_hiroba_osyaberi_04_01, msg_vip_osyaberi_04_01 },
	{ msg_hiroba_osyaberi_04_02, msg_vip_osyaberi_04_02 },
	{ msg_hiroba_osyaberi_04_03, msg_vip_osyaberi_04_03 },
	{ msg_hiroba_osyaberi_04_04, msg_vip_osyaberi_04_04 },
	{ msg_hiroba_osyaberi_04_05, msg_vip_osyaberi_04_05 },
	{ msg_hiroba_osyaberi_04_06, msg_vip_osyaberi_04_06 },
	{ msg_hiroba_osyaberi_04_07, msg_vip_osyaberi_04_07 },
	{ msg_hiroba_osyaberi_04_08, msg_vip_osyaberi_04_08 },
	{ msg_hiroba_osyaberi_04_09, msg_vip_osyaberi_04_09 },
	{ msg_hiroba_osyaberi_04_10, msg_vip_osyaberi_04_10 },
	{ msg_hiroba_osyaberi_04_11, msg_vip_osyaberi_04_11 },
	{ msg_hiroba_osyaberi_04_12, msg_vip_osyaberi_04_12 },
	{ msg_hiroba_osyaberi_04_13, msg_vip_osyaberi_04_13 },
	{ msg_hiroba_osyaberi_04_14, msg_vip_osyaberi_04_14 },
	{ msg_hiroba_osyaberi_04_15, msg_vip_osyaberi_04_15 },
	{ msg_hiroba_osyaberi_04_16, msg_vip_osyaberi_04_16 },
	{ msg_hiroba_osyaberi_04_17, msg_vip_osyaberi_04_17 },
	{ msg_hiroba_osyaberi_04_18, msg_vip_osyaberi_04_18 },
	{ msg_hiroba_osyaberi_04_19, msg_vip_osyaberi_04_19 },
	{ msg_hiroba_osyaberi_04_20, msg_vip_osyaberi_04_20 },
	{ msg_hiroba_osyaberi_04_21, msg_vip_osyaberi_04_21 },
	{ msg_hiroba_osyaberi_04_22, msg_vip_osyaberi_04_22 },
	{ msg_hiroba_osyaberi_04_23, msg_vip_osyaberi_04_23 },
	{ msg_hiroba_osyaberi_04_24, msg_vip_osyaberi_04_24 },
	{ msg_hiroba_osyaberi_04_25, msg_vip_osyaberi_04_25 },
	{ msg_hiroba_osyaberi_04_26, msg_vip_osyaberi_04_26 },
	{ msg_hiroba_osyaberi_04_27, msg_vip_osyaberi_04_27 },
	{ msg_hiroba_osyaberi_04_28, msg_vip_osyaberi_04_28 },
	{ msg_hiroba_osyaberi_04_29, msg_vip_osyaberi_04_29 },
	{ msg_hiroba_osyaberi_04_30, msg_vip_osyaberi_04_30 },
	{ msg_hiroba_osyaberi_04_31, msg_vip_osyaberi_04_31 },
	{ msg_hiroba_osyaberi_04_32, msg_vip_osyaberi_04_32 },
	{ msg_hiroba_osyaberi_04_33, msg_vip_osyaberi_04_33 },
	{ msg_hiroba_osyaberi_04_34, msg_vip_osyaberi_04_34 },
	{ msg_hiroba_osyaberi_04_35, msg_vip_osyaberi_04_35 },
	{ msg_hiroba_osyaberi_04_36, msg_vip_osyaberi_04_36 },
	{ msg_hiroba_osyaberi_04_37, msg_vip_osyaberi_04_37 },
	{ msg_hiroba_osyaberi_04_38, msg_vip_osyaberi_04_38 },
	{ msg_hiroba_osyaberi_04_39, msg_vip_osyaberi_04_39 },
	{ msg_hiroba_osyaberi_04_40, msg_vip_osyaberi_04_40 },
	{ msg_hiroba_osyaberi_04_41, msg_vip_osyaberi_04_41 },
	{ msg_hiroba_osyaberi_05_01, msg_vip_osyaberi_05_01 },
	{ msg_hiroba_osyaberi_05_02, msg_vip_osyaberi_05_02 },
	{ msg_hiroba_osyaberi_05_03, msg_vip_osyaberi_05_03 },
	{ msg_hiroba_osyaberi_05_04, msg_vip_osyaberi_05_04 },
	{ msg_hiroba_osyaberi_05_05, msg_vip_osyaberi_05_05 },
	{ msg_hiroba_osyaberi_05_06, msg_vip_osyaberi_05_06 },
	{ msg_hiroba_osyaberi_05_07, msg_vip_osyaberi_05_07 },
	{ msg_hiroba_osyaberi_05_08, msg_vip_osyaberi_05_08 },
	{ msg_hiroba_osyaberi_05_09, msg_vip_osyaberi_05_09 },
	{ msg_hiroba_osyaberi_05_10, msg_vip_osyaberi_05_10 },
	{ msg_hiroba_osyaberi_05_11, msg_vip_osyaberi_05_11 },
	{ msg_hiroba_osyaberi_05_12, msg_vip_osyaberi_05_12 },
	{ msg_hiroba_osyaberi_05_13, msg_vip_osyaberi_05_13 },
	{ msg_hiroba_osyaberi_05_14, msg_vip_osyaberi_05_14 },
	{ msg_hiroba_osyaberi_05_15, msg_vip_osyaberi_05_15 },
	{ msg_hiroba_osyaberi_05_16, msg_vip_osyaberi_05_16 },
	{ msg_hiroba_osyaberi_05_17, msg_vip_osyaberi_05_17 },
	{ msg_hiroba_osyaberi_05_18, msg_vip_osyaberi_05_18 },
	{ msg_hiroba_osyaberi_05_19, msg_vip_osyaberi_05_19 },
	{ msg_hiroba_osyaberi_05_20, msg_vip_osyaberi_05_20 },
	{ msg_hiroba_osyaberi_05_21, msg_vip_osyaberi_05_21 },
	{ msg_hiroba_osyaberi_05_22, msg_vip_osyaberi_05_22 },
	{ msg_hiroba_osyaberi_05_23, msg_vip_osyaberi_05_23 },
	{ msg_hiroba_osyaberi_05_24, msg_vip_osyaberi_05_24 },
	{ msg_hiroba_osyaberi_05_25, msg_vip_osyaberi_05_25 },
	{ msg_hiroba_osyaberi_05_26, msg_vip_osyaberi_05_26 },
	{ msg_hiroba_osyaberi_05_27, msg_vip_osyaberi_05_27 },
	{ msg_hiroba_osyaberi_05_28, msg_vip_osyaberi_05_28 },
	{ msg_hiroba_osyaberi_05_29, msg_vip_osyaberi_05_29 },
	{ msg_hiroba_osyaberi_05_30, msg_vip_osyaberi_05_30 },
	{ msg_hiroba_osyaberi_05_31, msg_vip_osyaberi_05_31 },
	{ msg_hiroba_osyaberi_05_32, msg_vip_osyaberi_05_32 },
	{ msg_hiroba_osyaberi_05_33, msg_vip_osyaberi_05_33 },
	{ msg_hiroba_osyaberi_05_34, msg_vip_osyaberi_05_34 },
	{ msg_hiroba_osyaberi_05_35, msg_vip_osyaberi_05_35 },
	{ msg_hiroba_osyaberi_05_36, msg_vip_osyaberi_05_36 },
	{ msg_hiroba_osyaberi_05_37, msg_vip_osyaberi_05_37 },
	{ msg_hiroba_osyaberi_05_38, msg_vip_osyaberi_05_38 },
	{ msg_hiroba_osyaberi_05_39, msg_vip_osyaberi_05_39 },
	{ msg_hiroba_osyaberi_05_40, msg_vip_osyaberi_05_40 },
	{ msg_hiroba_osyaberi_06_01, msg_vip_osyaberi_06_01 },
	{ msg_hiroba_osyaberi_06_02, msg_vip_osyaberi_06_02 },
	{ msg_hiroba_osyaberi_06_03, msg_vip_osyaberi_06_03 },
	{ msg_hiroba_osyaberi_06_04, msg_vip_osyaberi_06_04 },
	{ msg_hiroba_osyaberi_06_05, msg_vip_osyaberi_06_05 },
	{ msg_hiroba_osyaberi_06_06, msg_vip_osyaberi_06_06 },
	{ msg_hiroba_osyaberi_06_07, msg_vip_osyaberi_06_07 },
	{ msg_hiroba_osyaberi_06_08, msg_vip_osyaberi_06_08 },
	{ msg_hiroba_osyaberi_06_09, msg_vip_osyaberi_06_09 },
	{ msg_hiroba_osyaberi_06_10, msg_vip_osyaberi_06_10 },
	{ msg_hiroba_item_00_01, msg_vip_item_00_01 },
	{ msg_hiroba_item_00_02, msg_vip_item_00_02 },
	{ msg_hiroba_item_00_03, msg_vip_item_00_03 },
	{ msg_hiroba_item_00_04, msg_vip_item_00_04 },
	{ msg_hiroba_item_00_05, msg_vip_item_00_05 },
	{ msg_hiroba_item_00_06, msg_vip_item_00_06 },
	{ msg_hiroba_item_00_07, msg_vip_item_00_07 },
	{ msg_hiroba_item_00_08, msg_vip_item_00_08 },
	{ msg_hiroba_osyaberi_07_01, msg_vip_osyaberi_07_01 }, 
	{ msg_hiroba_osyaberi_07_02, msg_vip_osyaberi_07_02 }, 
	{ msg_hiroba_osyaberi_08_01, msg_vip_osyaberi_08_01 }, 
	{ msg_hiroba_osyaberi_01_11, msg_vip_osyaberi_01_11 },
	{ msg_hiroba_osyaberi_01_12, msg_vip_osyaberi_01_12 },
	{ msg_hiroba_osyaberi_01_13, msg_vip_osyaberi_01_13 },
	{ msg_hiroba_osyaberi_01_14, msg_vip_osyaberi_01_14 },
	{ msg_hiroba_osyaberi_01_15, msg_vip_osyaberi_01_15 },
	{ msg_hiroba_osyaberi_01_16, msg_vip_osyaberi_01_16 },
	{ msg_hiroba_osyaberi_01_17, msg_vip_osyaberi_01_17 },
	{ msg_hiroba_osyaberi_01_18, msg_vip_osyaberi_01_18 },
	{ msg_hiroba_osyaberi_01_19, msg_vip_osyaberi_01_19 },
	{ msg_hiroba_osyaberi_01_20, msg_vip_osyaberi_01_20 },
	{ msg_hiroba_osyaberi_01_21, msg_vip_osyaberi_01_21 },
	{ msg_hiroba_osyaberi_01_22, msg_vip_osyaberi_01_22 },
	{ msg_hiroba_osyaberi_01_23, msg_vip_osyaberi_01_23 },
	{ msg_hiroba_osyaberi_01_24, msg_vip_osyaberi_01_24 },
	{ msg_hiroba_osyaberi_01_25, msg_vip_osyaberi_01_25 },
	{ msg_hiroba_osyaberi_01_26, msg_vip_osyaberi_01_26 },
	{ msg_hiroba_osyaberi_01_27, msg_vip_osyaberi_01_27 },
	{ msg_hiroba_osyaberi_01_28, msg_vip_osyaberi_01_28 },
	{ msg_hiroba_osyaberi_01_29, msg_vip_osyaberi_01_29 },
	{ msg_hiroba_osyaberi_01_30, msg_vip_osyaberi_01_30 },
	{ msg_hiroba_osyaberi_02_11, msg_vip_osyaberi_02_11 },
	{ msg_hiroba_osyaberi_02_12, msg_vip_osyaberi_02_12 },
	{ msg_hiroba_osyaberi_02_13, msg_vip_osyaberi_02_13 },
	{ msg_hiroba_osyaberi_02_14, msg_vip_osyaberi_02_14 },
	{ msg_hiroba_osyaberi_02_15, msg_vip_osyaberi_02_15 },
	{ msg_hiroba_osyaberi_02_16, msg_vip_osyaberi_02_16 },
	{ msg_hiroba_osyaberi_02_17, msg_vip_osyaberi_02_17 },
	{ msg_hiroba_osyaberi_02_18, msg_vip_osyaberi_02_18 },
	{ msg_hiroba_osyaberi_02_19, msg_vip_osyaberi_02_19 },
	{ msg_hiroba_osyaberi_02_20, msg_vip_osyaberi_02_20 },
	{ msg_hiroba_osyaberi_02_21, msg_vip_osyaberi_02_21 },
	{ msg_hiroba_osyaberi_02_22, msg_vip_osyaberi_02_22 },
	{ msg_hiroba_osyaberi_02_23, msg_vip_osyaberi_02_23 },
	{ msg_hiroba_osyaberi_02_24, msg_vip_osyaberi_02_24 },
	{ msg_hiroba_osyaberi_02_25, msg_vip_osyaberi_02_25 },
	{ msg_hiroba_osyaberi_02_26, msg_vip_osyaberi_02_26 },
	{ msg_hiroba_osyaberi_02_27, msg_vip_osyaberi_02_27 },
	{ msg_hiroba_osyaberi_02_28, msg_vip_osyaberi_02_28 },
	{ msg_hiroba_osyaberi_02_29, msg_vip_osyaberi_02_29 },
	{ msg_hiroba_osyaberi_02_30, msg_vip_osyaberi_02_30 },
	{ msg_hiroba_osyaberi_04_42, msg_vip_osyaberi_04_42 },
	{ msg_hiroba_osyaberi_04_43, msg_vip_osyaberi_04_43 },
	{ msg_hiroba_osyaberi_04_44, msg_vip_osyaberi_04_44 },
	{ msg_hiroba_osyaberi_04_45, msg_vip_osyaberi_04_45 },
	{ msg_hiroba_osyaberi_04_46, msg_vip_osyaberi_04_46 },
	{ msg_hiroba_osyaberi_04_47, msg_vip_osyaberi_04_47 },
	{ msg_hiroba_osyaberi_04_48, msg_vip_osyaberi_04_48 },
	{ msg_hiroba_osyaberi_04_49, msg_vip_osyaberi_04_49 },
	{ msg_hiroba_osyaberi_04_50, msg_vip_osyaberi_04_50 },
	{ msg_hiroba_osyaberi_04_51, msg_vip_osyaberi_04_51 },
	{ msg_hiroba_osyaberi_04_52, msg_vip_osyaberi_04_52 },
	{ msg_hiroba_osyaberi_04_53, msg_vip_osyaberi_04_53 },
	{ msg_hiroba_osyaberi_04_54, msg_vip_osyaberi_04_54 },
	{ msg_hiroba_osyaberi_04_55, msg_vip_osyaberi_04_55 },
	{ msg_hiroba_osyaberi_04_56, msg_vip_osyaberi_04_56 },
	{ msg_hiroba_osyaberi_04_57, msg_vip_osyaberi_04_57 },
	{ msg_hiroba_osyaberi_04_58, msg_vip_osyaberi_04_58 },
	{ msg_hiroba_osyaberi_04_59, msg_vip_osyaberi_04_59 },
	{ msg_hiroba_osyaberi_04_60, msg_vip_osyaberi_04_60 },
	{ msg_hiroba_osyaberi_04_61, msg_vip_osyaberi_04_61 },
	{ msg_hiroba_osyaberi_04_62, msg_vip_osyaberi_04_62 },
	{ msg_hiroba_osyaberi_04_63, msg_vip_osyaberi_04_63 },
	{ msg_hiroba_osyaberi_04_64, msg_vip_osyaberi_04_64 },
	{ msg_hiroba_osyaberi_04_65, msg_vip_osyaberi_04_65 },
	{ msg_hiroba_osyaberi_04_66, msg_vip_osyaberi_04_66 },
	{ msg_hiroba_osyaberi_04_67, msg_vip_osyaberi_04_67 },
	{ msg_hiroba_osyaberi_04_68, msg_vip_osyaberi_04_68 },
	{ msg_hiroba_osyaberi_04_69, msg_vip_osyaberi_04_69 },
	{ msg_hiroba_osyaberi_04_70, msg_vip_osyaberi_04_70 },
	{ msg_hiroba_osyaberi_04_71, msg_vip_osyaberi_04_71 },
	{ msg_hiroba_osyaberi_04_72, msg_vip_osyaberi_04_72 },
	{ msg_hiroba_osyaberi_04_73, msg_vip_osyaberi_04_73 },
	{ msg_hiroba_osyaberi_04_74, msg_vip_osyaberi_04_74 },
	{ msg_hiroba_osyaberi_04_75, msg_vip_osyaberi_04_75 },
	{ msg_hiroba_osyaberi_04_76, msg_vip_osyaberi_04_76 },
	{ msg_hiroba_osyaberi_04_77, msg_vip_osyaberi_04_77 },
	{ msg_hiroba_osyaberi_04_78, msg_vip_osyaberi_04_78 },
	{ msg_hiroba_osyaberi_04_79, msg_vip_osyaberi_04_79 },
	{ msg_hiroba_osyaberi_04_80, msg_vip_osyaberi_04_80 },
	{ msg_hiroba_osyaberi_04_81, msg_vip_osyaberi_04_81 },
	{ msg_hiroba_osyaberi_04_82, msg_vip_osyaberi_04_82 },
	{ msg_hiroba_osyaberi_04_83, msg_vip_osyaberi_04_83 },
	{ msg_hiroba_osyaberi_04_84, msg_vip_osyaberi_04_84 },
	{ msg_hiroba_osyaberi_04_85, msg_vip_osyaberi_04_85 },
	{ msg_hiroba_osyaberi_04_86, msg_vip_osyaberi_04_86 },
	{ msg_hiroba_osyaberi_04_87, msg_vip_osyaberi_04_87 },
	{ msg_hiroba_osyaberi_04_88, msg_vip_osyaberi_04_88 },
	{ msg_hiroba_osyaberi_04_89, msg_vip_osyaberi_04_89 },
	{ msg_hiroba_osyaberi_04_90, msg_vip_osyaberi_04_90 },
	{ msg_hiroba_osyaberi_04_91, msg_vip_osyaberi_04_91 },
	{ msg_hiroba_osyaberi_04_92, msg_vip_osyaberi_04_92 },
	{ msg_hiroba_osyaberi_04_93, msg_vip_osyaberi_04_93 },
	{ msg_hiroba_osyaberi_04_94, msg_vip_osyaberi_04_94 },
	{ msg_hiroba_osyaberi_04_95, msg_vip_osyaberi_04_95 },
	{ msg_hiroba_osyaberi_04_96, msg_vip_osyaberi_04_96 },
	{ msg_hiroba_osyaberi_04_97, msg_vip_osyaberi_04_97 },
	{ msg_hiroba_osyaberi_04_98, msg_vip_osyaberi_04_98 },
	{ msg_hiroba_osyaberi_04_99, msg_vip_osyaberi_04_99 },
	{ msg_hiroba_osyaberi_04_100, msg_vip_osyaberi_04_100 },
	{ msg_hiroba_osyaberi_04_101, msg_vip_osyaberi_04_101 },
	{ msg_hiroba_osyaberi_04_102, msg_vip_osyaberi_04_102 },
	{ msg_hiroba_osyaberi_04_103, msg_vip_osyaberi_04_103 },
	{ msg_hiroba_osyaberi_04_104, msg_vip_osyaberi_04_104 },
	{ msg_hiroba_osyaberi_04_105, msg_vip_osyaberi_04_105 },
	{ msg_hiroba_osyaberi_04_106, msg_vip_osyaberi_04_106 },
	{ msg_hiroba_osyaberi_04_107, msg_vip_osyaberi_04_107 },
	{ msg_hiroba_osyaberi_04_108, msg_vip_osyaberi_04_108 },
	{ msg_hiroba_osyaberi_04_109, msg_vip_osyaberi_04_109 },
	{ msg_hiroba_osyaberi_04_110, msg_vip_osyaberi_04_110 },
	{ msg_hiroba_osyaberi_04_111, msg_vip_osyaberi_04_111 },
	{ msg_hiroba_osyaberi_04_112, msg_vip_osyaberi_04_112 },
	{ msg_hiroba_osyaberi_04_113, msg_vip_osyaberi_04_113 },
	{ msg_hiroba_osyaberi_04_114, msg_vip_osyaberi_04_114 },
	{ msg_hiroba_osyaberi_04_115, msg_vip_osyaberi_04_115 },
	{ msg_hiroba_osyaberi_04_116, msg_vip_osyaberi_04_116 },
	{ msg_hiroba_osyaberi_04_117, msg_vip_osyaberi_04_117 },
	{ msg_hiroba_osyaberi_04_118, msg_vip_osyaberi_04_118 },
	{ msg_hiroba_osyaberi_04_119, msg_vip_osyaberi_04_119 },
	{ msg_hiroba_osyaberi_04_120, msg_vip_osyaberi_04_120 },
	{ msg_hiroba_osyaberi_04_121, msg_vip_osyaberi_04_121 },
	{ msg_hiroba_osyaberi_05_41, msg_vip_osyaberi_05_41 },
	{ msg_hiroba_osyaberi_05_42, msg_vip_osyaberi_05_42 },
	{ msg_hiroba_osyaberi_05_43, msg_vip_osyaberi_05_43 },
	{ msg_hiroba_osyaberi_05_44, msg_vip_osyaberi_05_44 },
	{ msg_hiroba_osyaberi_05_45, msg_vip_osyaberi_05_45 },
	{ msg_hiroba_osyaberi_05_46, msg_vip_osyaberi_05_46 },
	{ msg_hiroba_osyaberi_05_47, msg_vip_osyaberi_05_47 },
	{ msg_hiroba_osyaberi_05_48, msg_vip_osyaberi_05_48 },
	{ msg_hiroba_osyaberi_05_49, msg_vip_osyaberi_05_49 },
	{ msg_hiroba_osyaberi_05_50, msg_vip_osyaberi_05_50 },
	{ msg_hiroba_osyaberi_05_51, msg_vip_osyaberi_05_51 },
	{ msg_hiroba_osyaberi_05_52, msg_vip_osyaberi_05_52 },
	{ msg_hiroba_osyaberi_05_53, msg_vip_osyaberi_05_53 },
	{ msg_hiroba_osyaberi_05_54, msg_vip_osyaberi_05_54 },
	{ msg_hiroba_osyaberi_05_55, msg_vip_osyaberi_05_55 },
	{ msg_hiroba_osyaberi_05_56, msg_vip_osyaberi_05_56 },
	{ msg_hiroba_osyaberi_05_57, msg_vip_osyaberi_05_57 },
	{ msg_hiroba_osyaberi_05_58, msg_vip_osyaberi_05_58 },
	{ msg_hiroba_osyaberi_05_59, msg_vip_osyaberi_05_59 },
	{ msg_hiroba_osyaberi_05_60, msg_vip_osyaberi_05_60 },
	{ msg_hiroba_osyaberi_05_61, msg_vip_osyaberi_05_61 },
	{ msg_hiroba_osyaberi_05_62, msg_vip_osyaberi_05_62 },
	{ msg_hiroba_osyaberi_05_63, msg_vip_osyaberi_05_63 },
	{ msg_hiroba_osyaberi_05_64, msg_vip_osyaberi_05_64 },
	{ msg_hiroba_osyaberi_05_65, msg_vip_osyaberi_05_65 },
	{ msg_hiroba_osyaberi_05_66, msg_vip_osyaberi_05_66 },
	{ msg_hiroba_osyaberi_05_67, msg_vip_osyaberi_05_67 },
	{ msg_hiroba_osyaberi_05_68, msg_vip_osyaberi_05_68 },
	{ msg_hiroba_osyaberi_05_69, msg_vip_osyaberi_05_69 },
	{ msg_hiroba_osyaberi_05_70, msg_vip_osyaberi_05_70 },
	{ msg_hiroba_osyaberi_05_71, msg_vip_osyaberi_05_71 },
	{ msg_hiroba_osyaberi_05_72, msg_vip_osyaberi_05_72 },
	{ msg_hiroba_osyaberi_05_73, msg_vip_osyaberi_05_73 },
	{ msg_hiroba_osyaberi_05_74, msg_vip_osyaberi_05_74 },
	{ msg_hiroba_osyaberi_05_75, msg_vip_osyaberi_05_75 },
	{ msg_hiroba_osyaberi_05_76, msg_vip_osyaberi_05_76 },
	{ msg_hiroba_osyaberi_05_77, msg_vip_osyaberi_05_77 },
	{ msg_hiroba_osyaberi_05_78, msg_vip_osyaberi_05_78 },
	{ msg_hiroba_osyaberi_05_79, msg_vip_osyaberi_05_79 },
	{ msg_hiroba_osyaberi_05_80, msg_vip_osyaberi_05_80 },
	{ msg_hiroba_osyaberi_05_81, msg_vip_osyaberi_05_81 },
	{ msg_hiroba_osyaberi_05_82, msg_vip_osyaberi_05_82 },
	{ msg_hiroba_osyaberi_05_83, msg_vip_osyaberi_05_83 },
	{ msg_hiroba_osyaberi_05_84, msg_vip_osyaberi_05_84 },
	{ msg_hiroba_osyaberi_05_85, msg_vip_osyaberi_05_85 },
	{ msg_hiroba_osyaberi_05_86, msg_vip_osyaberi_05_86 },
	{ msg_hiroba_osyaberi_05_87, msg_vip_osyaberi_05_87 },
	{ msg_hiroba_osyaberi_05_88, msg_vip_osyaberi_05_88 },
	{ msg_hiroba_osyaberi_05_89, msg_vip_osyaberi_05_89 },
	{ msg_hiroba_osyaberi_05_90, msg_vip_osyaberi_05_90 },
	{ msg_hiroba_osyaberi_05_91, msg_vip_osyaberi_05_91 },
	{ msg_hiroba_osyaberi_05_92, msg_vip_osyaberi_05_92 },
	{ msg_hiroba_osyaberi_05_93, msg_vip_osyaberi_05_93 },
	{ msg_hiroba_osyaberi_05_94, msg_vip_osyaberi_05_94 },
	{ msg_hiroba_osyaberi_05_95, msg_vip_osyaberi_05_95 },
	{ msg_hiroba_osyaberi_05_96, msg_vip_osyaberi_05_96 },
	{ msg_hiroba_osyaberi_05_97, msg_vip_osyaberi_05_97 },
	{ msg_hiroba_osyaberi_05_98, msg_vip_osyaberi_05_98 },
	{ msg_hiroba_osyaberi_05_99, msg_vip_osyaberi_05_99 },
	{ msg_hiroba_osyaberi_05_100, msg_vip_osyaberi_05_100 },
	{ msg_hiroba_osyaberi_05_101, msg_vip_osyaberi_05_101 },
	{ msg_hiroba_osyaberi_05_102, msg_vip_osyaberi_05_102 },
	{ msg_hiroba_osyaberi_05_103, msg_vip_osyaberi_05_103 },
	{ msg_hiroba_osyaberi_05_104, msg_vip_osyaberi_05_104 },
	{ msg_hiroba_osyaberi_05_105, msg_vip_osyaberi_05_105 },
	{ msg_hiroba_osyaberi_05_106, msg_vip_osyaberi_05_106 },
	{ msg_hiroba_osyaberi_05_107, msg_vip_osyaberi_05_107 },
	{ msg_hiroba_osyaberi_05_108, msg_vip_osyaberi_05_108 },
	{ msg_hiroba_osyaberi_05_109, msg_vip_osyaberi_05_109 },
	{ msg_hiroba_osyaberi_05_110, msg_vip_osyaberi_05_110 },
	{ msg_hiroba_osyaberi_05_111, msg_vip_osyaberi_05_111 },
	{ msg_hiroba_osyaberi_05_112, msg_vip_osyaberi_05_112 },
	{ msg_hiroba_osyaberi_05_113, msg_vip_osyaberi_05_113 },
	{ msg_hiroba_osyaberi_05_114, msg_vip_osyaberi_05_114 },
	{ msg_hiroba_osyaberi_05_115, msg_vip_osyaberi_05_115 },
	{ msg_hiroba_osyaberi_05_116, msg_vip_osyaberi_05_116 },
	{ msg_hiroba_osyaberi_05_117, msg_vip_osyaberi_05_117 },
	{ msg_hiroba_osyaberi_05_118, msg_vip_osyaberi_05_118 },
	{ msg_hiroba_osyaberi_05_119, msg_vip_osyaberi_05_119 },
	{ msg_hiroba_osyaberi_05_120, msg_vip_osyaberi_05_120 },
	{ msg_hiroba_osyaberi_06_11, msg_vip_osyaberi_06_11 },
	{ msg_hiroba_osyaberi_06_12, msg_vip_osyaberi_06_12 },
	{ msg_hiroba_osyaberi_06_13, msg_vip_osyaberi_06_13 },
	{ msg_hiroba_osyaberi_06_14, msg_vip_osyaberi_06_14 },
	{ msg_hiroba_osyaberi_06_15, msg_vip_osyaberi_06_15 },
	{ msg_hiroba_osyaberi_06_16, msg_vip_osyaberi_06_16 },
	{ msg_hiroba_osyaberi_06_17, msg_vip_osyaberi_06_17 },
	{ msg_hiroba_osyaberi_06_18, msg_vip_osyaberi_06_18 },
	{ msg_hiroba_osyaberi_06_19, msg_vip_osyaberi_06_19 },
	{ msg_hiroba_osyaberi_06_20, msg_vip_osyaberi_06_20 },
	{ msg_hiroba_osyaberi_06_21, msg_vip_osyaberi_06_21 },
	{ msg_hiroba_osyaberi_06_22, msg_vip_osyaberi_06_22 },
	{ msg_hiroba_osyaberi_06_23, msg_vip_osyaberi_06_23 },
	{ msg_hiroba_osyaberi_06_24, msg_vip_osyaberi_06_24 },
	{ msg_hiroba_osyaberi_06_25, msg_vip_osyaberi_06_25 },
	{ msg_hiroba_osyaberi_06_26, msg_vip_osyaberi_06_26 },
	{ msg_hiroba_osyaberi_06_27, msg_vip_osyaberi_06_27 },
	{ msg_hiroba_osyaberi_06_28, msg_vip_osyaberi_06_28 },
	{ msg_hiroba_osyaberi_06_29, msg_vip_osyaberi_06_29 },
	{ msg_hiroba_osyaberi_06_30, msg_vip_osyaberi_06_30 },
};


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

static u32 WFLBY_EV_TALK_GetTimeZone( const WFLBY_USER_PROFILE* cp_pr );
static STRBUF* WFLBY_EV_TALK_GetStrBuf( WFLBY_ROOMWK* p_rmwk, u32 plidx, u32 msgidx );
static STRBUF* WFLBY_EV_TALK_GetStrBuf_Aisatu03_02( WFLBY_ROOMWK* p_rmwk, u32 aidx, u32 bidx );


static BOOL WFLBY_EV_TALK_WK_Init( WFLBY_EV_TALK_WK* p_evwk, WFLBY_SYSTEM* p_system, u32 idx );
static void WFLBY_EV_TALK_WK_Exit( WFLBY_EV_TALK_WK* p_evwk );
static void WFLBY_EV_TALK_WK_Wait( WFLBY_EV_TALK_WK* p_evwk, u8 wait, u8 ret_seq, WFLBY_EVENTWK* p_wk, u8 wait_seq );
static void WFLBY_EV_TALK_WK_WaitMain( WFLBY_EV_TALK_WK* p_evwk, WFLBY_EVENTWK* p_wk );
static void WFLBY_EV_TALK_WK_MsgEndWaitMain( WFLBY_EV_TALK_WK* p_evwk, WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk );
static void WFLBY_EV_TALK_WK_AisatuDraw( WFLBY_EV_TALK_WK* p_evwk, WFLBY_SYSTEM* p_system, WFLBY_ROOMWK* p_rmwk, u32 timezone, const WFLBY_USER_PROFILE* cp_mypr, const WFLBY_USER_PROFILE* cp_youpr );
static void WFLBY_EV_TALK_WK_Aisatu_SetWordSet( WFLBY_ROOMWK* p_rmwk, u32 buffno, u32 timezone, const WFLBY_USER_PROFILE* cp_pr );
static BOOL WFLBY_EV_TALK_WK_AisatuFollowDraw( WFLBY_EV_TALK_WK* p_evwk, WFLBY_SYSTEM* p_system, WFLBY_ROOMWK* p_rmwk );
static void WFLBY_EV_TALK_WK_WordsetSetItem( WFLBY_EV_TALK_WK* p_evwk, WFLBY_SYSTEM* p_system, WFLBY_ROOMWK* p_rmwk, u32 bufid, BOOL my );
static void WFLBY_EV_TALK_WK_PrintTimeWaitMsg( WFLBY_EV_TALK_WK* p_evwk, WFLBY_ROOMWK* p_rmwk );
static void WFLBY_EV_TALK_WK_StartSelectWait( WFLBY_EV_TALK_WK* p_evwk );
static void WFLBY_EV_TALK_WK_StopSelectWait( WFLBY_EV_TALK_WK* p_evwk );
static void WFLBY_EV_TALK_WK_CheckAndPrintSelectWait( WFLBY_EV_TALK_WK* p_evwk, WFLBY_ROOMWK* p_rmwk );
static void WFLBY_EV_TALK_WK_MainSelectWait( WFLBY_EV_TALK_WK* p_evwk );
static void WFLBY_EV_TALK_WK_SetRirekiData( WFLBY_EV_TALK_WK* p_evwk, const WFLBY_USER_PROFILE* cp_youpr );


static void WFLBY_EV_TALK_WK_InitBmpList( WFLBY_EV_TALK_BMPLIST* p_wk, WFLBY_ROOMWK* p_rmwk, u32 num, u32 msg_start, u32 last_sel_b );
static void WFLBY_EV_TALK_WK_InitBmpListRandom( WFLBY_EV_TALK_BMPLIST* p_wk, WFLBY_ROOMWK* p_rmwk, u32 allnum, u32 sel_num, u32 msg_start, u32 last_sel_b );
static void WFLBY_EV_TOPIC_WK_InitBmpList( WFLBY_EV_TALK_BMPLIST* p_wk, WFLBY_ROOMWK* p_rmwk );
static void WFLBY_EV_TOPIC_WK_InitEventBmpList( WFLBY_EV_TALK_BMPLIST* p_wk, WFLBY_ROOMWK* p_rmwk );
static void WFLBY_EV_TOPIC_WK_InitMinigameBmpList( WFLBY_EV_TALK_BMPLIST* p_wk, WFLBY_ROOMWK* p_rmwk );
static void WFLBY_EV_TALK_WK_ExitBmpList( WFLBY_EV_TALK_BMPLIST* p_wk, WFLBY_ROOMWK* p_rmwk );


static void WFLBY_EV_TALK_RWSEND_Init( WFLBY_EV_TALK_RWSEND* p_wk );
static void WFLBY_EV_TALK_RWSEND_Start( WFLBY_EV_TALK_RWSEND* p_wk, const WFLBY_SYSTEM* cp_system, u16 send_data, u16 wait_seq );
static void WFLBY_EV_TALK_RWSEND_Main( WFLBY_EV_TALK_RWSEND* p_wk, WFLBY_SYSTEM* p_system );
static BOOL WFLBY_EV_TALK_RWSEND_CheckEnd( const WFLBY_EV_TALK_RWSEND* cp_wk );
static u16 WFLBY_EV_TALK_RWSEND_GetTalkSeq( const WFLBY_EV_TALK_RWSEND* cp_wk );


//----------------------------------------------------------------------------
/**
 *	@brief	話しかけ開始
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_EV_TALK_StartA( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	WFLBY_EV_TALK_WK* p_evwk;
	WFLBY_SYSTEM* p_system;
	WFLBY_EVENT*  p_event;
	u32 talk_seq;
	u32 recv_data;
	STRBUF* p_str;
	const WFLBY_USER_PROFILE* cp_mypr;
	const WFLBY_USER_PROFILE* cp_youpr;

	p_system	= WFLBY_ROOM_GetSystemData( p_rmwk );		// システムデータ取得
	p_event		= WFLBY_ROOM_GetEvent( p_rmwk );			// イベントワーク取得
	p_evwk		= WFLBY_EVENTWK_GetWk( p_wk );				// イベントデータ取得
	talk_seq	= WFLBY_SYSTEM_GetTalkSeq( p_system );		// 会話シーケンス取得
	recv_data	= WFLBY_SYSTEM_GetTalkRecv( p_system );		// 受信データ

	// えらーが発生したらEX終了
	if( WFLBY_SYSTEM_CheckTalkError( p_system ) ){
		if( p_evwk ){
			if( p_evwk->err_chk ){
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_ERR_ENDMSG );
				WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );
				p_evwk->err_chk = FALSE;
			}
		}
	}

	if( p_evwk ){
		cp_mypr		= WFLBY_SYSTEM_GetMyProfileLocal( p_system );
#if PL_T0866_080715_FIX
		if( p_evwk->p_npc ){
			cp_youpr	= WFLBY_SYSTEM_GetUserProfile( p_system, p_evwk->talk_idx );
		}else{
			cp_youpr	= NULL;
		}
#else
		cp_youpr	= WFLBY_SYSTEM_GetUserProfile( p_system, p_evwk->talk_idx );
#endif
	}

	// 初期化完了後自分と相手のユーザプロフィールを取得する
	if( WFLBY_EVENTWK_GetSeq( p_wk ) > WFLBY_EV_TALK_A_SEQ_INIT ){
		if( p_evwk->err_chk ){

			// 相手がいなくなったら強制終了
			if( cp_youpr == NULL ){	
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_ERR_ENDMSG );
				WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );
				p_evwk->err_chk = FALSE;
			}
		}
	}
	

	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){
	// 初期化
	case WFLBY_EV_TALK_A_SEQ_INIT:

#ifdef WFLBY_EV_TALK_DEBUG_SEL_MSG
		// 順番に質問が出てくるデバックON
		if( sys.cont & PAD_BUTTON_R ){
			s_WFLBY_EV_TALK_DEBUG_SEL_MSG_ON = TRUE;
		}else{
			s_WFLBY_EV_TALK_DEBUG_SEL_MSG_ON = FALSE;
		}
#endif


		p_evwk = WFLBY_EVENTWK_AllocWk( p_wk, sizeof(WFLBY_EV_TALK_WK) );

		// 履歴データ初期化
		p_evwk->rireki_flag		= FALSE;
		p_evwk->rireki_actno	= 0;
		p_evwk->rireki_userid	= 0;

		// 自動メッセージ送信システムを初期化
		WFLBY_EV_TALK_RWSEND_Init( &p_evwk->rw_send );
		
		//  会話ターゲットを取得する
		{
			WFLBY_3DPERSON* p_npc;
			WFLBY_3DOBJCONT* p_objcont;
			WFLBY_3DPERSON* p_player;
			u32 idx;
			u32 way;
			BOOL result;

			p_objcont	= WFLBY_ROOM_GetObjCont( p_rmwk );
			p_player	= WFLBY_3DOBJCONT_GetPlayer( p_objcont );
			p_npc		= WFLBY_3DOBJCONT_GetFrontPerson( p_objcont, p_player );
#if	PL_T0866_080715_FIX
			if( p_npc == NULL ){
				// 会話エラー終了へ
				p_evwk->err_chk = FALSE;// (一応行う)
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_END );
				break;
			}
#endif
			idx			= WFLBY_3DOBJCONT_GetWkObjData( p_npc, WF2DMAP_OBJPM_PLID );
			way			= WFLBY_3DOBJCONT_GetWkObjData( p_player,  WF2DMAP_OBJPM_WAY );
			way			= WF2DMPA_OBJToolRetWayGet( way );

			// 080630	会話状態のワークをクリア
			WFLBY_SYSTEM_EndTalk( p_system );

			// ワーク初期化
			// 戻り値で話し相手が在籍中か調べる
			result = WFLBY_EV_TALK_WK_Init( p_evwk, p_system, idx );
		
			// NPCもとめる＆こっち向ける
			p_evwk->p_npc	= p_npc;
			WFLBY_3DOBJCONT_DRAW_SetUpdata( p_npc, FALSE );
			WFLBY_3DOBJCONT_DRAW_SetWay( p_npc, way );
			// イベントもとめる
			WFLBY_EVENT_SetPrivateEventMove( p_event, idx, FALSE );

			// 話しかけ音
			Snd_SePlay( WFLBY_SND_TALK );

			// 会話シーケンスへ
			WFLBY_SYSTEM_SetMyStatus( p_system, WFLBY_STATUS_TALK );

			if( result == TRUE ){
				// NPCのプロフィールを表示する
				WFLBY_ROOM_UNDERWIN_TrCardOn( p_rmwk, idx, FALSE );

				// 会話リクエストへ
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_REQ );

			}else{

				// 会話リクエストへ
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_ANSNGMSG );
				break;
			}

		}
		break;

	// リクエスト
	case WFLBY_EV_TALK_A_SEQ_REQ:
		{
			BOOL result;

			result = WFLBY_SYSTEM_SendTalkReq( p_system, p_evwk->talk_idx );
			if( result == TRUE ){
				// 応答待ちへ
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_ANSWAIT );
			}else{
				// 応答失敗
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_ANSNGMSG );
			}
		}
		break;
		
	// 回答待ち
	case WFLBY_EV_TALK_A_SEQ_ANSWAIT:
		
		// 返事が帰ってきたか？
		switch( talk_seq ){
		// 返事の結果OKO
		case WFLBY_TALK_SEQ_A_SEL:	// 話しかけ側	選択

			// その人と話したと、マスク設定
			WFLBY_SYSTEM_SetTalkMsk( p_system, p_evwk->talk_idx );

			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_AISATUMY );
			break;

		// 返事の結果NG
		case WFLBY_TALK_SEQ_NONE:	// 何もなし	会話を終了させてください
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_ANSNGMSG );
			break;

		// 待ち状態
		case WFLBY_TALK_SEQ_B_ANS:	// かけられ側	応答
			break;

		// その他にはならないはず
		// EXENDあつかい
		case WFLBY_TALK_SEQ_B_SEL:	// かけられ側	選択
		case WFLBY_TALK_SEQ_EXEND:	// 緊急終了	会話を終了させてください
		default:
			// 切断終了
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_ERR_ENDMSG );
			break;
		}
		break;

	// あいさつ	自分の時間
	case WFLBY_EV_TALK_A_SEQ_AISATUMY:
		// 冒険ノート
		WFLBY_SYSTEM_FNOTE_SetTalk( p_system, p_evwk->talk_idx );

		// レコード
		WFLBY_SYSTEM_RECORD_AddTalk( p_system );

		// 遊んだ情報
		WFLBY_SYSTEM_PLAYED_SetPlay( p_system, WFLBY_PLAYED_TALK );
		
		WFLBY_EV_TALK_WK_AisatuDraw( p_evwk, p_system, p_rmwk, p_evwk->my_timezone, cp_mypr, cp_youpr );
		// ウエイトのあと挨拶フォローへ
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_A_SEQ_AISATUYOU, p_wk, WFLBY_EV_TALK_A_SEQ_MSGWAIT );
		break;

	// あいさつ	相手の時間
	case WFLBY_EV_TALK_A_SEQ_AISATUYOU:
		WFLBY_EV_TALK_WK_AisatuDraw( p_evwk, p_system, p_rmwk, p_evwk->you_timezone, cp_youpr, cp_mypr );

		// ウエイトの後同期処理へ
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_A_SEQ_SYNC_RECV, p_wk, WFLBY_EV_TALK_A_SEQ_MSGWAIT );

		// 同期開始	自分のデータ送信
		WFLBY_SYSTEM_SendTalkData( p_system, WFLBY_EV_TALK_TYPE_AISATSUSYNC );
		break;

	// 会話状態同期
	case WFLBY_EV_TALK_A_SEQ_SYNC_RECV:		
		// 返事が帰ってきたか？
		switch( talk_seq ){
		case WFLBY_TALK_SEQ_A_SEL:	// 話しかけ側	選択
			WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );


			// その人との会話が初めてかで分岐？
			if( WFLBY_SYSTEM_GetTalkFirst( p_system ) ){
				// 初めてのとき
				// 挨拶フォローへ
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_AISATUFOLLOW );
			}else{
				// もうはなしたことがある
				// 挨拶終了２へ
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_AISATUEND2 );
			}
			break;

		// 待ち状態
		case WFLBY_TALK_SEQ_B_SEL:	// かけられ側	選択
			if( WFLBY_ROOM_TALKWIN_CheckTimeWait( p_rmwk ) == FALSE ){
				WFLBY_ROOM_TALKWIN_StartTimeWait( p_rmwk );
			}

			// 待ち時間がWFLBY_TALK_RECV_HALF_WAITになったらメッセージを表示
			WFLBY_EV_TALK_WK_PrintTimeWaitMsg( p_evwk, p_rmwk );
			break;

		// その他にはならないはず
		// EXENDあつかい
		case WFLBY_TALK_SEQ_NONE:	// 何もなし	会話を終了させてください
		case WFLBY_TALK_SEQ_EXEND:	// 緊急終了	会話を終了させてください
		case WFLBY_TALK_SEQ_B_ANS:	// かけられ側	応答
		default:
			// 切断終了
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_ERR_ENDMSG );
			WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );
			break;
		}
		break;

	// あいさつフォロー
	case WFLBY_EV_TALK_A_SEQ_AISATUFOLLOW:
		{
			BOOL result;
			// フォローが必要？
			result = WFLBY_EV_TALK_WK_AisatuFollowDraw( p_evwk, p_system, p_rmwk );
			if( result == TRUE ){
				// ウエイトのあと挨拶最後へ
				WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
						WFLBY_EV_TALK_A_SEQ_AISATUEND, p_wk, WFLBY_EV_TALK_A_SEQ_MSGWAIT );
			}else{
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_AISATUEND );
			}
		}
		break;
		
	// あいさつの最後のメッセージ
	case WFLBY_EV_TALK_A_SEQ_AISATUEND:
		// 文字列作成
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_aisatu_03_01 );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_aisatu_03_01 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// ウエイトの後、自分の選択へ
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_A_SEQ_SEL_GT_FIR, p_wk, WFLBY_EV_TALK_A_SEQ_MSGWAIT );
		break;

	// 挨拶の最後のメッセージ	もう１回はなしているとき
	case WFLBY_EV_TALK_A_SEQ_AISATUEND2:

		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->my_idx, 1 );
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_aisatu_03_02 );
		p_str = WFLBY_EV_TALK_GetStrBuf_Aisatu03_02( p_rmwk, p_evwk->talk_idx, p_evwk->my_idx );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// ウエイトの後、自分の選択へ
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_A_SEQ_SEL_GT_FIR, p_wk, WFLBY_EV_TALK_A_SEQ_MSGWAIT );
		break;

	// ガジェット、会話セレクト	１回目
	case WFLBY_EV_TALK_A_SEQ_SEL_GT_FIR:
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_osyaberi_00_01 );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_osyaberi_00_01 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// ウエイトの後、リスト表示
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_A_SEQ_SEL_GT00, p_wk, WFLBY_EV_TALK_A_SEQ_MSGWAIT );
		break;
		
	// ガジェット、会話セレクト	２回目以降
	case WFLBY_EV_TALK_A_SEQ_SEL_GT_SEC:		
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_osyaberi_00_09 );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_osyaberi_00_09 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// ウエイトの後、リスト表示
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_A_SEQ_SEL_GT00, p_wk, WFLBY_EV_TALK_A_SEQ_MSGWAIT );
		break;

	// ガジェット、会話セレクト リスト表示
	case WFLBY_EV_TALK_A_SEQ_SEL_GT00:
		WFLBY_EV_TALK_WK_InitBmpList( &p_evwk->bmplist, p_rmwk, WFLBY_EV_TALK_LIST_SEL_GT_NUM, msg_hiroba_menu_00_01, WFLBY_EV_TALK_LIST_END_NONE );
		WFLBY_ROOM_LISTWIN_Start( p_rmwk, &p_evwk->bmplist.bmplist, 0, 0 );
		WFLBY_EV_TALK_WK_StartSelectWait( p_evwk );	// 選択待ち開始
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_SEL_GT00WAIT );
		break;

	// ガジェット、会話セレクト　選択待ち
	case WFLBY_EV_TALK_A_SEQ_SEL_GT00WAIT:
		{
			u32 result;
			BOOL del;
			result = WFLBY_ROOM_LISTWIN_Main( p_rmwk );
			switch( result ){
			case WFLBY_EV_TALK_LIST_SEL_GT_GAGET:
				//  ガジェットメッセージへ
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_SEND_G );
				del = TRUE;
				break;
				
			case WFLBY_EV_TALK_LIST_SEL_GT_TALK:
				//  会話選択へ
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_SEND_TALK );

				// 会話トピック作成
				{
					u32 npc_plno;
					npc_plno = WFLBY_3DOBJCONT_GetWkObjData( p_evwk->p_npc, WF2DMAP_OBJPM_PLID );
					WFLBY_SYSTEM_TOPIC_SendConnect( p_system, plno, npc_plno );
				}
				
				del = TRUE;
				break;
				
			case WFLBY_EV_TALK_LIST_SEL_GT_END:

				// 終了を送信
				WFLBY_SYSTEM_SendTalkEnd( p_system );
				
				//  終了へ
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_ENDMSG );
				del = TRUE;
				break;

			default:
				WFLBY_EV_TALK_WK_CheckAndPrintSelectWait( p_evwk, p_rmwk );
				del = FALSE;
				break;
			}

			// リスト破棄
			if( del == TRUE ){
				OS_TPrintf( "SEL_GT00WAIT result = %d\n", result );
				WFLBY_ROOM_LISTWIN_End( p_rmwk, NULL, NULL );
				WFLBY_EV_TALK_WK_ExitBmpList( &p_evwk->bmplist, p_rmwk );
				WFLBY_EV_TALK_WK_StopSelectWait( p_evwk );
			}
		}
		break;

	// ガジェットもらう処理開始
	case WFLBY_EV_TALK_A_SEQ_SEND_G:

		// ガジェットが一緒かチェック
		if( WFLBY_SYSTEM_GetProfileItem( cp_mypr ) == WFLBY_SYSTEM_GetProfileItem( cp_youpr ) ){

			// ガジェット一緒	
			p_evwk->gadget_eq = TRUE;
			
			// 文字列作成
			WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
			WFLBY_EV_TALK_WK_WordsetSetItem( p_evwk, p_system, p_rmwk, 1, FALSE );
		//	p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_item_00_01 );
			p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_item_00_01 );
			WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

			// Bの返答待ちに戻る
			WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
					WFLBY_EV_TALK_A_SEQ_SEND_G_RECV_B, p_wk, WFLBY_EV_TALK_A_SEQ_MSGWAIT );

			// ガジェットが一緒だと送信
			WFLBY_SYSTEM_SendTalkData( p_system, WFLBY_EV_TALK_TYPE_GADGET_EQ );
		}else{

			// ガジェット違う
			p_evwk->gadget_eq = FALSE;

			// 違う
			// 文字列作成
			WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
			WFLBY_EV_TALK_WK_WordsetSetItem( p_evwk, p_system, p_rmwk, 1, TRUE );
			WFLBY_EV_TALK_WK_WordsetSetItem( p_evwk, p_system, p_rmwk, 2, FALSE );
//			p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_item_00_02 );
			p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_item_00_02 );
			WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

			// ウエイトの後、もらうか選択へ
			WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
					WFLBY_EV_TALK_A_SEQ_SEND_G_GETCHK, p_wk, WFLBY_EV_TALK_A_SEQ_MSGWAIT );

		}
		break;
		
	// ガジェットもらうか選択
	case WFLBY_EV_TALK_A_SEQ_SEND_G_GETCHK:
		WFLBY_EV_TALK_WK_InitBmpList( &p_evwk->bmplist, p_rmwk, WFLBY_EV_TALK_LIST_SEL_G_NUM, msg_hiroba_menu_00_04, WFLBY_EV_TALK_LIST_END_YAMERU );
		WFLBY_ROOM_LISTWIN_Start( p_rmwk, &p_evwk->bmplist.bmplist, 0, 0 );

		WFLBY_EV_TALK_WK_StartSelectWait( p_evwk );
		
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_SEND_G_GETCHKWAIT );
		break;

	// ガジェットもらうか選択まち
	case WFLBY_EV_TALK_A_SEQ_SEND_G_GETCHKWAIT:
		{
			u32 result;
			BOOL del;
			result = WFLBY_ROOM_LISTWIN_Main( p_rmwk );
			switch( result ){
			case WFLBY_EV_TALK_LIST_SEL_G_GET:

				// ガジェットをもらうのでもらうことを送信
				WFLBY_SYSTEM_SendTalkData( p_system, WFLBY_EV_TALK_TYPE_GADGET );
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_SEND_G_GET );

				// 冒険ノート
				WFLBY_SYSTEM_FNOTE_SetGetGadget( p_system, p_evwk->talk_idx );


				// ガジェット交換トピック作成
				{
					u32 npc_plno;
					npc_plno = WFLBY_3DOBJCONT_GetWkObjData( p_evwk->p_npc, WF2DMAP_OBJPM_PLID );
					WFLBY_SYSTEM_TOPIC_SendItem( p_system, plno, npc_plno, WFLBY_SYSTEM_GetProfileItem( cp_youpr ) );
				}
				del = TRUE;
				break;
				
			case WFLBY_EV_TALK_LIST_SEL_G_END:
				//  会話選択へ
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_SEL_GT_SEC );
				del = TRUE;
				break;
				
			default:
				WFLBY_EV_TALK_WK_CheckAndPrintSelectWait( p_evwk, p_rmwk );
				del = FALSE;
				break;
			}

			// リスト破棄
			if( del == TRUE ){
				WFLBY_ROOM_LISTWIN_End( p_rmwk, NULL, NULL );
				WFLBY_EV_TALK_WK_ExitBmpList( &p_evwk->bmplist, p_rmwk );

				WFLBY_EV_TALK_WK_StopSelectWait( p_evwk );
			}
		}
		break;

	// ガジェットもらう
	case WFLBY_EV_TALK_A_SEQ_SEND_G_GET:
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
		WFLBY_EV_TALK_WK_WordsetSetItem( p_evwk, p_system, p_rmwk, 1, FALSE );
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_item_00_04 );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_item_00_04 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// ウエイトの後、項目選択へ
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_A_SEQ_SEND_G_RECV_B_SE, p_wk, WFLBY_EV_TALK_A_SEQ_MSGWAIT );

		// ガジェットを設定
		{
			u32 item;
			item = WFLBY_SYSTEM_GetProfileItem( cp_youpr );
			WFLBY_SYSTEM_SetMyItem( p_system, item );
		}
		break;

	case WFLBY_EV_TALK_A_SEQ_SEND_G_RECV_B_SE:
		// ガジェットもらうBの受信確認
		// SE再生
		Snd_SePlay( WFLBY_SND_TOUCH_TOY_CHG );
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_SEND_G_RECV_B );
		break;

	// ガジェットもらうBの受信確認
	case WFLBY_EV_TALK_A_SEQ_SEND_G_RECV_B:	

		// 返事が帰ってきたか？
		switch( talk_seq ){
		case WFLBY_TALK_SEQ_A_SEL:	// 話しかけ側	選択

			// 最後にしたことにガジェット交換を設定
			WFLBY_EV_TALK_WK_SetRirekiData( p_evwk, cp_youpr );
			
			if( p_evwk->gadget_eq == TRUE ){
				// 会話終了へ
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_ENDMSG );
			}else{
				// 再項目選択へ
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_SEL_GT_SEC );
			}

			WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );
			break;

		// 待ち状態
		case WFLBY_TALK_SEQ_B_SEL:	// かけられ側	選択
			if( WFLBY_ROOM_TALKWIN_CheckTimeWait( p_rmwk ) == FALSE ){
				WFLBY_ROOM_TALKWIN_StartTimeWait( p_rmwk );
			}

			// 待ち時間がWFLBY_TALK_RECV_HALF_WAITになったらメッセージを表示
			WFLBY_EV_TALK_WK_PrintTimeWaitMsg( p_evwk, p_rmwk );
			break;

		// その他にはならないはず
		// EXENDあつかい
		case WFLBY_TALK_SEQ_NONE:	// 何もなし	会話を終了させてください
		case WFLBY_TALK_SEQ_EXEND:	// 緊急終了	会話を終了させてください
		case WFLBY_TALK_SEQ_B_ANS:	// かけられ側	応答
		default:
			// 切断終了
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_ERR_ENDMSG );
			WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );
			break;
		}
		break;

	// 会話することを送信
	case WFLBY_EV_TALK_A_SEQ_SEND_TALK:
		WFLBY_SYSTEM_SendTalkData( p_system, WFLBY_EV_TALK_TYPE_TALK_START );
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_SEL_T_K );
		break;
		
	// 会話項目選択
	case WFLBY_EV_TALK_A_SEQ_SEL_T_K:
		WFLBY_EV_TALK_WK_InitBmpListRandom( &p_evwk->bmplist, p_rmwk, WFLBY_EV_TALK_LIST_SEL_TK_NUM, WFLBY_EV_TALK_TK_SEL_NUM, msg_hiroba_menu_01_01, WFLBY_EV_TALK_LIST_END_NONE );
		WFLBY_ROOM_LISTWIN_Start_Ex( p_rmwk, &p_evwk->bmplist.bmplist, 0, 0, WFLBY_EV_TALK_TK_LISTWIN_X, WFLBY_EV_TALK_TK_LISTWIN_Y, WFLBY_EV_TALK_TK_LISTWIN_SIZX );
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_SEL_T_K_WAIT );


		// 項目選択待ちー＞自分のものを選択　まで１つのくくりとして待つ
		WFLBY_EV_TALK_WK_StartSelectWait( p_evwk );
		break;

	// 会話項目選択待ち
	case WFLBY_EV_TALK_A_SEQ_SEL_T_K_WAIT:	
		{
			u32 result;

#ifdef WFLBY_EV_TALK_DEBUG_SEL_MSG
			// 順番に質問が出てくるデバック
			if( s_WFLBY_EV_TALK_DEBUG_SEL_MSG_ON == TRUE ){
				if( (sys.trg & PAD_KEY_RIGHT) || (sys.trg & PAD_KEY_LEFT) ){

					if( sys.trg & PAD_KEY_RIGHT ){
						s_WFLBY_EV_TALK_DEBUG_SEL_MSG_PAGE = (s_WFLBY_EV_TALK_DEBUG_SEL_MSG_PAGE+1) % 8;
					}else{
						s_WFLBY_EV_TALK_DEBUG_SEL_MSG_PAGE --;
						if( s_WFLBY_EV_TALK_DEBUG_SEL_MSG_PAGE < 0 ){
							s_WFLBY_EV_TALK_DEBUG_SEL_MSG_PAGE += 8;
						}
					}

					WFLBY_ROOM_LISTWIN_End( p_rmwk, NULL, NULL );
					WFLBY_EV_TALK_WK_ExitBmpList( &p_evwk->bmplist, p_rmwk );

					// リスト再描画
					WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_SEL_T_K );
				}
			}
#endif

			
			result = WFLBY_ROOM_LISTWIN_Main( p_rmwk );
			switch( result ){
/*
			case WFLBY_EV_TALK_LIST_SEL_TK_END:	// やめる
				WFLBY_ROOM_LISTWIN_End( p_rmwk, NULL, NULL );
				WFLBY_EV_TALK_WK_ExitBmpList( &p_evwk->bmplist, p_rmwk );
				WFLBY_EV_TALK_WK_StopSelectWait( p_evwk );
				// 選択へ
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_SEL_GT_SEC );
				break;
//*/

			case BMPLIST_NULL:
			case BMPLIST_CANCEL:
				WFLBY_EV_TALK_WK_CheckAndPrintSelectWait( p_evwk, p_rmwk );
				break;

			// なんか選んだ
			default:
				WFLBY_ROOM_LISTWIN_End( p_rmwk, NULL, NULL );
				WFLBY_EV_TALK_WK_ExitBmpList( &p_evwk->bmplist, p_rmwk );
//				WFLBY_EV_TALK_WK_StopSelectWait( p_evwk );	// 次の会話内容を選ぶものも続けて待つ

				// 会話項目数設定
				p_evwk->sel_k = result;

				// 詳細項目チェックへ
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_SEL_T_M );
				break;

			}
		}
		break;
		
	// 自分のこと選択
	case WFLBY_EV_TALK_A_SEQ_SEL_T_M:
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
		
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_osyaberi_02_01+p_evwk->sel_k );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_osyaberi_06_01+p_evwk->sel_k );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// ウエイトの後、リスト表示
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_A_SEQ_SEL_T_M_SEL, p_wk, WFLBY_EV_TALK_A_SEQ_MSGWAIT );
		break;

	// 自分のこと選択
	case WFLBY_EV_TALK_A_SEQ_SEL_T_M_SEL:	
		WFLBY_EV_TALK_WK_InitBmpList( &p_evwk->bmplist, p_rmwk, WFLBY_EV_TALK_LIST_SEL_TK_S_MAX, 
				msg_hiroba_osyaberi_03_01 + (p_evwk->sel_k*WFLBY_EV_TALK_LIST_SEL_TK_S_MAX), WFLBY_EV_TALK_LIST_END_NONE );
		WFLBY_ROOM_LISTWIN_Start( p_rmwk, &p_evwk->bmplist.bmplist, 0, 0 );
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_SEL_T_M_WAIT );
		break;

	// 自分のこと選択
	case WFLBY_EV_TALK_A_SEQ_SEL_T_M_WAIT:
		{
			u32 result;
			result = WFLBY_ROOM_LISTWIN_Main( p_rmwk );
			switch( result ){
			case WFLBY_EV_TALK_LIST_SEL_TK_S_00:	
			case WFLBY_EV_TALK_LIST_SEL_TK_S_01:	
			case WFLBY_EV_TALK_LIST_SEL_TK_S_02:	
			case WFLBY_EV_TALK_LIST_SEL_TK_S_04:	
				WFLBY_ROOM_LISTWIN_End( p_rmwk, NULL, NULL );
				WFLBY_EV_TALK_WK_ExitBmpList( &p_evwk->bmplist, p_rmwk );
				WFLBY_EV_TALK_WK_StopSelectWait( p_evwk );

				// 会話項目設定
				p_evwk->sel_k_data = (p_evwk->sel_k*WFLBY_EV_TALK_LIST_SEL_TK_S_MAX) + result;

				// 詳細項目チェックへ
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_SEND_T );
				break;

			default:
				WFLBY_EV_TALK_WK_CheckAndPrintSelectWait( p_evwk, p_rmwk );
				break;
			}
		}
		break;
	// 会話選択情報送信
	case WFLBY_EV_TALK_A_SEQ_SEND_T:
		// 自動送信開始
		WFLBY_EV_TALK_RWSEND_Start( &p_evwk->rw_send, p_system, WFLBY_EV_TALK_TYPE_TALK_COL_00 + p_evwk->sel_k_data, WFLBY_TALK_SEQ_A_SEL );

		// リアクションメッセージ表示
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_osyaberi_04_01+p_evwk->sel_k_data );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_osyaberi_05_01+p_evwk->sel_k_data );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_A_SEQ_SEND_T00, p_wk, WFLBY_EV_TALK_A_SEQ_MSGWAIT );

		break;

	// 会話リアクション
	case WFLBY_EV_TALK_A_SEQ_SEND_T00:
		// リアクションメッセージ2表示
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_osyaberi_06_01+p_evwk->sel_k );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_osyaberi_02_01+p_evwk->sel_k );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_A_SEQ_SEND_TWAIT, p_wk, WFLBY_EV_TALK_A_SEQ_MSGWAIT );
		break;

	// 会話選択情報送信
	case  WFLBY_EV_TALK_A_SEQ_SEND_TWAIT:
		// 自動送信が終わったかチェック
		{
			BOOL result;
			u16 recv_talk_seq;
			result = WFLBY_EV_TALK_RWSEND_CheckEnd( &p_evwk->rw_send );
			if( result == TRUE ){
				recv_talk_seq = WFLBY_EV_TALK_RWSEND_GetTalkSeq( &p_evwk->rw_send );

				// 成功
				if( recv_talk_seq == WFLBY_TALK_SEQ_A_SEL ){

					// 受信待ちへ
					WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_RECV_BT );
				}else{

					// 切断終了
					WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_ERR_ENDMSG );
					WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );
				}
			}else{

				// 終わってないので待機

				// タイムウエイトマーク出す
				if( WFLBY_ROOM_TALKWIN_CheckTimeWait( p_rmwk ) == FALSE ){
					WFLBY_ROOM_TALKWIN_StartTimeWait( p_rmwk );
				}

			}
		}
		break;

	// 会話選択情報受信
	case WFLBY_EV_TALK_A_SEQ_RECV_BT:

		
		// 会話データ受信待ち
		switch( talk_seq ){
		// 会話選択あり
		case WFLBY_TALK_SEQ_A_SEL:	// 話しかけ側	選択

			// 履歴保存
			WFLBY_EV_TALK_WK_SetRirekiData( p_evwk, cp_youpr );
			
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_MSG_BT );
			WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );
			break;


		// 待ち状態
		case WFLBY_TALK_SEQ_B_SEL:	// かけられ側	選択
			if( WFLBY_ROOM_TALKWIN_CheckTimeWait( p_rmwk ) == FALSE ){
				WFLBY_ROOM_TALKWIN_StartTimeWait( p_rmwk );
			}

			// 待ち時間がWFLBY_TALK_RECV_HALF_WAITになったらメッセージを表示
			WFLBY_EV_TALK_WK_PrintTimeWaitMsg( p_evwk, p_rmwk );
			break;

		// その他にはならないはず
		// EXENDあつかい
		case WFLBY_TALK_SEQ_NONE:	// 会話を終了させてください
		case WFLBY_TALK_SEQ_B_ANS:	// かけられ側	応答
		case WFLBY_TALK_SEQ_EXEND:	// 緊急終了	会話を終了させてください
		default:
			// 切断終了
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_ERR_ENDMSG );
			WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );
			break;
		}
		break;

	// 応答会話情報メッセージ表示
	case WFLBY_EV_TALK_A_SEQ_MSG_BT:
		{
			u16 recv_k_data = (recv_data-WFLBY_EV_TALK_TYPE_TALK_COL_00);

			// わからないといっていたら「わからないメッセージ終了」
			// わからない異常の数字が帰ってきてもわからないにする
			if( (recv_data >= WFLBY_EV_TALK_TYPE_TALK_NONE) || 
				(recv_data < WFLBY_EV_TALK_TYPE_TALK_COL_00 ) ){
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_A_SEQ_SELNGMSG );
				break;
			}
			
			// リアクションメッセージ表示
			WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
//			p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_osyaberi_05_01+recv_k_data );
			p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_osyaberi_04_01+recv_k_data );
			WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

			// 項目が一緒かチェック
			if( recv_k_data == p_evwk->sel_k_data ){
				WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
						WFLBY_EV_TALK_A_SEQ_MSG_ABT_EQ, p_wk, WFLBY_EV_TALK_A_SEQ_MSGWAIT );
			}else{
				WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
						WFLBY_EV_TALK_A_SEQ_MSG_BT_END, p_wk, WFLBY_EV_TALK_A_SEQ_MSGWAIT );
			}
		}
		break;
		
	// 選択項目が同じとき
	case WFLBY_EV_TALK_A_SEQ_MSG_ABT_EQ:	
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_osyaberi_00_06 );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_osyaberi_00_06 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_A_SEQ_SEL_GT_SEC, p_wk, WFLBY_EV_TALK_A_SEQ_MSGWAIT );

		break;

	// 応答メッセージ最後
	case WFLBY_EV_TALK_A_SEQ_MSG_BT_END:		
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
		// なるほどなぁ
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_osyaberi_00_05 );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_osyaberi_00_05 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_A_SEQ_SEL_GT_SEC, p_wk, WFLBY_EV_TALK_A_SEQ_MSGWAIT );

		break;

	// エラー終了
	case WFLBY_EV_TALK_A_SEQ_ERR_ENDMSG:	

		// リストが出ているかもしれないので消す
		WFLBY_ROOM_LISTWIN_End( p_rmwk, NULL, NULL );
		WFLBY_EV_TALK_WK_ExitBmpList( &p_evwk->bmplist, p_rmwk );
		
		// 選択中メッセージ表示
		// 文字列作成
//		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_aisatu_04_04 );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_aisatu_04_04 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// もうエラーチェック必要なし
		p_evwk->err_chk = FALSE;

		// ウエイトの後、リスト表示
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_A_SEQ_END, p_wk, WFLBY_EV_TALK_A_SEQ_WAIT );
//				WFLBY_EV_TALK_A_SEQ_END, p_wk, WFLBY_EV_TALK_A_SEQ_MSGWAIT );

		// 強制終了したことを教える
		WFLBY_SYSTEM_SendTalkExEnd( p_system );
		break;
	// 通常終了
	case WFLBY_EV_TALK_A_SEQ_ENDMSG:		
		// 選択中メッセージ表示
		// 文字列作成
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_aisatu_04_02 );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_aisatu_04_02 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// もうエラーチェック必要なし
		p_evwk->err_chk = FALSE;

		// ウエイトの後、リスト表示
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_A_SEQ_END, p_wk, WFLBY_EV_TALK_A_SEQ_WAIT );
//				WFLBY_EV_TALK_A_SEQ_END, p_wk, WFLBY_EV_TALK_A_SEQ_MSGWAIT );

		break;
		
	// 応答なし終了
	case WFLBY_EV_TALK_A_SEQ_ANSNGMSG:
		// 選択中メッセージ表示
		// 文字列作成
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_aisatu_04_01 );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_aisatu_04_01 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// もうエラーチェック必要なし
		p_evwk->err_chk = FALSE;

		// ウエイトの後、リスト表示
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_A_SEQ_END, p_wk, WFLBY_EV_TALK_A_SEQ_WAIT );
//				WFLBY_EV_TALK_A_SEQ_END, p_wk, WFLBY_EV_TALK_A_SEQ_MSGWAIT );
		break;

	// B選択NGメッセージ
	case WFLBY_EV_TALK_A_SEQ_SELNGMSG:		// B選択できない終了
		// 選択中メッセージ表示
		// 文字列作成
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_osyaberi_04_41 );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_osyaberi_04_41 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// もうエラーチェック必要なし
		p_evwk->err_chk = FALSE;

		// ウエイトの後、リスト表示
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_A_SEQ_END, p_wk, WFLBY_EV_TALK_A_SEQ_WAIT );
//				WFLBY_EV_TALK_A_SEQ_END, p_wk, WFLBY_EV_TALK_A_SEQ_MSGWAIT );
		break;

	// 待機処理
	case WFLBY_EV_TALK_A_SEQ_WAIT:
		WFLBY_EV_TALK_WK_WaitMain( p_evwk, p_wk );
		break;

	// メッセージの終了ウエイト
	case WFLBY_EV_TALK_A_SEQ_MSGWAIT:
		WFLBY_EV_TALK_WK_MsgEndWaitMain( p_evwk, p_wk, p_rmwk );
		break;

	// 終了
	case WFLBY_EV_TALK_A_SEQ_END:		

		// 最後にしたことに会話を設定
		if( p_evwk->rireki_flag == TRUE ){
			WFLBY_SYSTEM_SetLastAction( p_system, p_evwk->rireki_actno, p_evwk->rireki_userid );
		}

		// 会話終了にする
		WFLBY_SYSTEM_EndTalk( p_system );

		// メッセージ終了
		WFLBY_ROOM_TALKWIN_Off( p_rmwk );
		
#if	PL_T0866_080715_FIX 
#else
		// ワーク内の破棄
		WFLBY_EV_TALK_WK_Exit( p_evwk );

		// イベントワーク破棄
		WFLBY_EVENTWK_DeleteWk( p_wk );
#endif

		// 通常シーケンスへ
		WFLBY_SYSTEM_SetMyStatus( p_system, WFLBY_STATUS_LOGIN );
		{
			WFLBY_3DPERSON* p_player;
			WFLBY_3DOBJCONT* p_objcont;
			u32 idx;

			// プレイヤーを取得
			p_objcont = WFLBY_ROOM_GetObjCont( p_rmwk );
			p_player = WFLBY_3DOBJCONT_GetPlayer( p_objcont );
			WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_player, WFLBY_3DOBJCONT_MOVEPLAYER );

#if PL_T0866_080715_FIX
			if( p_evwk->p_npc != NULL ){
				// NPCも動かす
				WFLBY_3DOBJCONT_DRAW_SetUpdata( p_evwk->p_npc, TRUE );

				// イベントも動かす
				idx	= WFLBY_3DOBJCONT_GetWkObjData( p_evwk->p_npc, WF2DMAP_OBJPM_PLID );
				WFLBY_EVENT_SetPrivateEventMove( p_event, idx, TRUE );

				// プロフィール表示をやめる
				WFLBY_ROOM_UNDERWIN_TrCardOff( p_rmwk );
			}
#else

			// NPCも動かす
			WFLBY_3DOBJCONT_DRAW_SetUpdata( p_evwk->p_npc, TRUE );

			// イベントも動かす
			idx	= WFLBY_3DOBJCONT_GetWkObjData( p_evwk->p_npc, WF2DMAP_OBJPM_PLID );
			WFLBY_EVENT_SetPrivateEventMove( p_event, idx, TRUE );

			// プロフィール表示をやめる
			WFLBY_ROOM_UNDERWIN_TrCardOff( p_rmwk );
#endif
		}

#if	PL_T0866_080715_FIX 
		// ワーク内の破棄
		WFLBY_EV_TALK_WK_Exit( p_evwk );

		// イベントワーク破棄
		WFLBY_EVENTWK_DeleteWk( p_wk );
#else
#endif
		return TRUE;	// 割り込みイベント終了
	}

	WFLBY_EV_TALK_WK_MainSelectWait( p_evwk );


	// 自動送信システムメイン
	WFLBY_EV_TALK_RWSEND_Main( &p_evwk->rw_send, p_system );
	

	
	return FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	話しかけられ開始
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_EV_TALK_StartB( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	WFLBY_EV_TALK_WK* p_evwk;
	WFLBY_SYSTEM* p_system;
	u32 talk_seq;
	u32 recv_data;
	STRBUF* p_str;
	const WFLBY_USER_PROFILE* cp_mypr;
	const WFLBY_USER_PROFILE* cp_youpr;

	p_system	= WFLBY_ROOM_GetSystemData( p_rmwk );		// システムデータ取得
	p_evwk		= WFLBY_EVENTWK_GetWk( p_wk );				// イベントデータ取得
	talk_seq	= WFLBY_SYSTEM_GetTalkSeq( p_system );		// 会話シーケンス取得
	recv_data	= WFLBY_SYSTEM_GetTalkRecv( p_system );		// 受信データ

	// えらーが発生したらEX終了
	if( WFLBY_SYSTEM_CheckTalkError( p_system ) ){
		if( p_evwk ){
			if( p_evwk->err_chk == TRUE ){
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_B_SEQ_ERR_ENDMSG );
				WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );
				p_evwk->err_chk = FALSE;
			}
		}
	}

	if( p_evwk ){
		cp_mypr		= WFLBY_SYSTEM_GetMyProfileLocal( p_system );
		cp_youpr	= WFLBY_SYSTEM_GetUserProfile( p_system, p_evwk->talk_idx );
	}

	// 初期化完了後自分と相手のユーザプロフィールを取得する
	if( WFLBY_EVENTWK_GetSeq( p_wk ) > WFLBY_EV_TALK_B_SEQ_INIT ){
		if( p_evwk->err_chk == TRUE ){
			// 相手がいなくなったら強制終了
			if( cp_youpr == NULL ){	
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_B_SEQ_ERR_ENDMSG );
				WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );
				p_evwk->err_chk = FALSE;
			}
		}
	}
	

	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){
	// 初期化
	case WFLBY_EV_TALK_B_SEQ_INIT:
		p_evwk = WFLBY_EVENTWK_AllocWk( p_wk, sizeof(WFLBY_EV_TALK_WK) );

		p_evwk->rireki_flag		= FALSE;
		p_evwk->rireki_actno	= 0;
		p_evwk->rireki_userid	= 0;
		
		{
			u32 talk_idx;
			BOOL result;

			talk_idx = WFLBY_SYSTEM_GetTalkIdx( p_system );

			// ワーク初期化
			// 戻り値で話し相手が在籍中か調べる
			result = WFLBY_EV_TALK_WK_Init( p_evwk, p_system, talk_idx );

			// 話しかけ音
			Snd_SePlay( WFLBY_SND_TALK );

			// 会話シーケンスへ
			WFLBY_SYSTEM_SetMyStatus( p_system, WFLBY_STATUS_TALK );

			// 話し相手がいたらそのまま進む
			if( result == TRUE ){
				// NPCのプロフィールを表示する
				WFLBY_ROOM_UNDERWIN_TrCardOn( p_rmwk, talk_idx, FALSE );

				// その人と話したと、マスク設定
				WFLBY_SYSTEM_SetTalkMsk( p_system, talk_idx );

				// 会話リクエストへ
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_B_SEQ_AISATUYOU );
			}else{
				// 話し相手がいなかったら終わる
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_B_SEQ_ERR_ENDMSG );
				break;
			}

		}
		break;

	// あいさつ	あいての時間
	case WFLBY_EV_TALK_B_SEQ_AISATUYOU:

		// 挨拶相手が目の前に来るまで待つ
		if( WFLBY_SYSTEM_CheckTalkBStart( p_system ) == FALSE ){
			break;
		}
		
		WFLBY_EV_TALK_WK_AisatuDraw( p_evwk, p_system, p_rmwk, p_evwk->you_timezone, cp_youpr, cp_mypr );

		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_B_SEQ_AISATUMY, p_wk, WFLBY_EV_TALK_B_SEQ_MSGWAIT );
		break;

	// あいさつ	自分の時間
	case WFLBY_EV_TALK_B_SEQ_AISATUMY:

		// 冒険ノート
		WFLBY_SYSTEM_FNOTE_SetTalk( p_system, p_evwk->talk_idx );

		// レコード
		WFLBY_SYSTEM_RECORD_AddTalk( p_system );

		// 遊んだ情報
		WFLBY_SYSTEM_PLAYED_SetPlay( p_system, WFLBY_PLAYED_TALK );



		WFLBY_EV_TALK_WK_AisatuDraw( p_evwk, p_system, p_rmwk, p_evwk->my_timezone, cp_mypr, cp_youpr );

		// ウエイトの後通信同期
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_B_SEQ_SYNC_RECV, p_wk, WFLBY_EV_TALK_B_SEQ_MSGWAIT );

		break;

	// 会話状態同期
	case WFLBY_EV_TALK_B_SEQ_SYNC_RECV:
		// 会話データ受信待ち
		switch( talk_seq ){
		// 同期確認データ受信
		case WFLBY_TALK_SEQ_B_SEL:
			WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );

			// すぐにダミー応答
			WFLBY_SYSTEM_SendTalkData( p_system,  WFLBY_EV_TALK_TYPE_AISATSUSYNC );

			// その人との会話が初めてか？
			if( WFLBY_SYSTEM_GetTalkFirst( p_system ) ){
				// 初めてのとき
				// 挨拶フォローへ
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_B_SEQ_AISATUFOLLOW );
			}else{
				// もうはなしたことがある
				// ウエイトのあと挨拶終了２へ
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_B_SEQ_AISATUEND2 );
			}
			break;

		// 待ち状態
		case WFLBY_TALK_SEQ_A_SEL:	// かけられ側	選択
			if( WFLBY_ROOM_TALKWIN_CheckTimeWait( p_rmwk ) == FALSE ){
				WFLBY_ROOM_TALKWIN_StartTimeWait( p_rmwk );
			}

			// 待ち時間がWFLBY_TALK_RECV_HALF_WAITになったらメッセージを表示
			WFLBY_EV_TALK_WK_PrintTimeWaitMsg( p_evwk, p_rmwk );
			break;


		// その他にはならないはず
		// EXENDあつかい
		case WFLBY_TALK_SEQ_B_ANS:	// かけられ側	応答
		case WFLBY_TALK_SEQ_EXEND:	// 緊急終了	会話を終了させてください
		case WFLBY_TALK_SEQ_NONE:	// 会話を終了させてください
		default:
			// 切断終了
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_B_SEQ_ERR_ENDMSG );
			WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );
			break;
		}
		break;

	// あいさつフォロー
	case WFLBY_EV_TALK_B_SEQ_AISATUFOLLOW:
		{
			BOOL result;
			// フォローが必要？
			result = WFLBY_EV_TALK_WK_AisatuFollowDraw( p_evwk, p_system, p_rmwk );
			if( result == TRUE ){
				// ウエイトのあと挨拶最後へ
				WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
						WFLBY_EV_TALK_B_SEQ_AISATUEND, p_wk, WFLBY_EV_TALK_B_SEQ_MSGWAIT );
			}else{
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_B_SEQ_AISATUEND );
			}
		}
		break;
		
	// あいさつの最後のメッセージ
	case WFLBY_EV_TALK_B_SEQ_AISATUEND:
		// 文字列作成
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_aisatu_03_01 );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_aisatu_03_01 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// ウエイトの後、話し相手の選択まち
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_B_SEQ_RECV_A_FST, p_wk, WFLBY_EV_TALK_B_SEQ_MSGWAIT );
		break;

	// 挨拶の最後のメッセージ	もう１回はなしているとき
	case WFLBY_EV_TALK_B_SEQ_AISATUEND2:

		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->my_idx, 1 );
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_aisatu_03_02 );
		p_str = WFLBY_EV_TALK_GetStrBuf_Aisatu03_02( p_rmwk, p_evwk->talk_idx, p_evwk->my_idx );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// ウエイトの後、自分の選択へ
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_B_SEQ_RECV_A_FST, p_wk, WFLBY_EV_TALK_B_SEQ_MSGWAIT );
		break;

	// 相手の選択内容待ち	最初
	case WFLBY_EV_TALK_B_SEQ_RECV_A_FST:
		// 文字列作成
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_osyaberi_00_02 );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_osyaberi_00_02 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// ウエイトの後、話し相手の選択まち
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_B_SEQ_RECV_A, p_wk, WFLBY_EV_TALK_B_SEQ_MSGWAIT );
		break;

	// 相手の選択内容待ち ２回目以降
	case WFLBY_EV_TALK_B_SEQ_RECV_A_SEC:	
		// 文字列作成
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_osyaberi_00_08 );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_osyaberi_00_08 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// ウエイトの後、話し相手の選択まち
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_B_SEQ_RECV_A, p_wk, WFLBY_EV_TALK_B_SEQ_MSGWAIT );
		break;

	// 相手の選択内容待ち
	case WFLBY_EV_TALK_B_SEQ_RECV_A:
		// 会話データ受信待ち
		switch( talk_seq ){
		// 会話かガジェットの選択データ到着
		case WFLBY_TALK_SEQ_B_SEL:	// 話しかけられ側	選択
			switch( recv_data ){
			// ガジェットもらった
			case WFLBY_EV_TALK_TYPE_GADGET:
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_B_SEQ_MSG_G00 );

				// 会話成立!!!
				WFLBY_SYSTEM_AddTalkCount( p_system );
				p_evwk->b_count_add = TRUE;

				// ガジェット交換を履歴に設定
				WFLBY_EV_TALK_WK_SetRirekiData( p_evwk, cp_youpr );
				break;
			// ガジェットが一緒
			case WFLBY_EV_TALK_TYPE_GADGET_EQ:
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_B_SEQ_MSG_G_EQ );

				// 会話成立!!!
				WFLBY_SYSTEM_AddTalkCount( p_system );
				p_evwk->b_count_add = TRUE;

				// ガジェット交換を履歴に設定
				WFLBY_EV_TALK_WK_SetRirekiData( p_evwk, cp_youpr );
				break;

			// 挨拶
			case WFLBY_EV_TALK_TYPE_TALK_START:
				// すぐにダミー応答
				WFLBY_SYSTEM_SendTalkData( p_system,  WFLBY_EV_TALK_TYPE_TALK_BEFORE );
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_B_SEQ_MSG_AT00 );
				break;

			// その他はありえない
			default:
				// 切断終了
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_B_SEQ_ERR_ENDMSG );
				WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );
				break;
			}
			WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );
			break;

		// 待ち状態
		case WFLBY_TALK_SEQ_A_SEL:	// かけられ側	選択
			if( WFLBY_ROOM_TALKWIN_CheckTimeWait( p_rmwk ) == FALSE ){
				WFLBY_ROOM_TALKWIN_StartTimeWait( p_rmwk );
			}

			// 待ち時間がWFLBY_TALK_RECV_HALF_WAITになったらメッセージを表示
			WFLBY_EV_TALK_WK_PrintTimeWaitMsg( p_evwk, p_rmwk );
			break;

		// 終了を選択した
		case WFLBY_TALK_SEQ_NONE:	// 会話を終了させてください
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_B_SEQ_ENDMSG );
			WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );
			break;

		// その他にはならないはず
		// EXENDあつかい
		case WFLBY_TALK_SEQ_B_ANS:	// かけられ側	応答
		case WFLBY_TALK_SEQ_EXEND:	// 緊急終了	会話を終了させてください
		default:
			// 切断終了
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_B_SEQ_ERR_ENDMSG );
			WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );
			break;
		}
		break;

	// ガジェットメッセージ
	case WFLBY_EV_TALK_B_SEQ_MSG_G00:		// ガジェットメッセージ
		// 文字列作成
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
		WFLBY_EV_TALK_WK_WordsetSetItem( p_evwk, p_system, p_rmwk, 1, TRUE );
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_item_00_05 );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_item_00_05 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// ウエイトの後、話し相手の選択まち
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_B_SEQ_MSG_G01, p_wk, WFLBY_EV_TALK_B_SEQ_MSGWAIT );
		break;
		
	// ガジェットメッセージ
	case WFLBY_EV_TALK_B_SEQ_MSG_G01:		
		// 文字列作成
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
		WFLBY_EV_TALK_WK_WordsetSetItem( p_evwk, p_system, p_rmwk, 1, TRUE );
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_item_00_06 );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_item_00_06 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// ウエイトの後
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_B_SEQ_MSG_G02, p_wk, WFLBY_EV_TALK_B_SEQ_MSGWAIT );
		break;

	// ガジェットメッセージ
	case WFLBY_EV_TALK_B_SEQ_MSG_G02:		
		// 文字列作成
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
		WFLBY_EV_TALK_WK_WordsetSetItem( p_evwk, p_system, p_rmwk, 1, TRUE );
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_item_00_07 );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_item_00_07 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// ウエイトの後、”だいじにするね”
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_B_SEQ_MSG_G03, p_wk, WFLBY_EV_TALK_B_SEQ_MSGWAIT );
		break;

	// ガジェットメッセージ
	case WFLBY_EV_TALK_B_SEQ_MSG_G03:

		// わけてあげたSE
		Snd_SePlay( WFLBY_SND_TOUCH_TOY_CHG );

		// 文字列作成
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_item_00_08 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// ウエイトの後、話し相手の選択まち
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_B_SEQ_RECV_A_SEC, p_wk, WFLBY_EV_TALK_B_SEQ_MSGWAIT );

		// ダミー応答
		WFLBY_SYSTEM_SendTalkData( p_system,  WFLBY_EV_TALK_TYPE_GADGET_END );

		// サブ画面の表示も更新
		{
			u32  item;

			item = WFLBY_SYSTEM_GetProfileItem( WFLBY_SYSTEM_GetMyProfileLocal( p_system ) );
			WFLBY_ROOM_UNDERWIN_TrCardGadGetChange( p_rmwk, item );
		}
		break;

	// ガジェットが一緒メッセージ
	case WFLBY_EV_TALK_B_SEQ_MSG_G_EQ:		
		// 文字列作成
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
		WFLBY_EV_TALK_WK_WordsetSetItem( p_evwk, p_system, p_rmwk, 1, TRUE );
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_item_00_01 );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_item_00_01 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// ウエイトの後、話し相手の選択まち
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_B_SEQ_ENDMSG, p_wk, WFLBY_EV_TALK_B_SEQ_MSGWAIT );

		// ダミー応答
		WFLBY_SYSTEM_SendTalkData( p_system,  WFLBY_EV_TALK_TYPE_GADGET_END );
		break;

	// A会話開始メッセージ表示
	case WFLBY_EV_TALK_B_SEQ_MSG_AT00:
		// ちょっとおしゃべりしようよ！
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_osyaberi_08_01 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// ウエイトの後、話し相手の選択まち
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_B_SEQ_RECV_A_SEL, p_wk, WFLBY_EV_TALK_B_SEQ_MSGWAIT );
		break;
		
	// Aの会話内容を取得
	case WFLBY_EV_TALK_B_SEQ_RECV_A_SEL:	
		// 会話データ受信待ち
		switch( talk_seq ){
		// 会話の選択データ到着
		case WFLBY_TALK_SEQ_B_SEL:	// 話しかけられ側	選択
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_B_SEQ_MSG_AT01 );
			WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );

			// 会話成立!!!
			WFLBY_SYSTEM_AddTalkCount( p_system );
			p_evwk->b_count_add = TRUE;
			break;

		// 待ち状態
		case WFLBY_TALK_SEQ_A_SEL:	// かけられ側	選択
			if( WFLBY_ROOM_TALKWIN_CheckTimeWait( p_rmwk ) == FALSE ){
				WFLBY_ROOM_TALKWIN_StartTimeWait( p_rmwk );
			}

			// 待ち時間がWFLBY_TALK_RECV_HALF_WAITになったらメッセージを表示
			WFLBY_EV_TALK_WK_PrintTimeWaitMsg( p_evwk, p_rmwk );
			break;


		// その他にはならないはず
		// EXENDあつかい
		case WFLBY_TALK_SEQ_B_ANS:	// かけられ側	応答
		case WFLBY_TALK_SEQ_EXEND:	// 緊急終了	会話を終了させてください
		case WFLBY_TALK_SEQ_NONE:	// 会話を終了させてください
		default:
			// 切断終了
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_B_SEQ_ERR_ENDMSG );
			WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );
			break;
		}
		break;
		
	// Aの選択事項表示
	case WFLBY_EV_TALK_B_SEQ_MSG_AT01:	// Aの選択事項表示
		// 不正な受信データチェック
		if( (recv_data < WFLBY_EV_TALK_TYPE_TALK_COL_00) || (recv_data >= WFLBY_EV_TALK_TYPE_TALK_NONE) ){
			// 切断終了
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_B_SEQ_ERR_ENDMSG );
			break;
		}
		
		// 受信データから項目とデータナンバーを求める
		p_evwk->recv_k_data		= recv_data - WFLBY_EV_TALK_TYPE_TALK_COL_00;
		p_evwk->sel_k			= p_evwk->recv_k_data / WFLBY_EV_TALK_LIST_SEL_TK_S_MAX;

		// 項目のことをいう
		// 文字列作成
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_osyaberi_01_01+p_evwk->sel_k );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_osyaberi_01_01+p_evwk->sel_k );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// ウエイトの後、話し相手の選択まち
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_B_SEQ_MSG_AT02, p_wk, WFLBY_EV_TALK_B_SEQ_MSGWAIT );
		break;

	// Aの選択つなぎ表示
	case WFLBY_EV_TALK_B_SEQ_MSG_AT02:

		// 項目のことをいう
		// 文字列作成
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_osyaberi_02_01+p_evwk->sel_k );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_osyaberi_02_01+p_evwk->sel_k );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// ウエイトの後、話し相手の選択まち
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_B_SEQ_MSG_AT03, p_wk, WFLBY_EV_TALK_B_SEQ_MSGWAIT );
		break;
		
	// Aの選択選んだこと表示
	case WFLBY_EV_TALK_B_SEQ_MSG_AT03:
		// 文字列作成
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_osyaberi_04_01+p_evwk->recv_k_data );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_osyaberi_04_01+p_evwk->recv_k_data );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// ウエイトの後、話し相手の選択まち
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_B_SEQ_MSG_AT04, p_wk, WFLBY_EV_TALK_B_SEQ_MSGWAIT );
		break;

	// Aの選択さらにつなぎ表示
	case WFLBY_EV_TALK_B_SEQ_MSG_AT04:	
		// 文字列作成
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_osyaberi_06_01+p_evwk->sel_k );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_osyaberi_06_01+p_evwk->sel_k );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// ウエイトの後、話し相手の選択まち
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_B_SEQ_SEL_T_M, p_wk, WFLBY_EV_TALK_B_SEQ_MSGWAIT );
		break;

	// 自分のを選択
	case WFLBY_EV_TALK_B_SEQ_SEL_T_M:	
		WFLBY_EV_TALK_WK_InitBmpList( &p_evwk->bmplist, p_rmwk, WFLBY_EV_TALK_LIST_SEL_TK_S_B_MAX, 
				msg_hiroba_osyaberi_03_01 + (p_evwk->sel_k*WFLBY_EV_TALK_LIST_SEL_TK_S_MAX), 
				WFLBY_EV_TALK_LIST_END_WAKARANAI );
		WFLBY_ROOM_LISTWIN_Start( p_rmwk, &p_evwk->bmplist.bmplist, 0, 0 );
		WFLBY_EV_TALK_WK_StartSelectWait( p_evwk );
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_B_SEQ_SEND_T );
		break;
		
	// 自分の選択を送信
	case WFLBY_EV_TALK_B_SEQ_SEND_T:		
		{
			u32 result;
			BOOL del;

			result = WFLBY_ROOM_LISTWIN_Main( p_rmwk );
			switch( result ){
			case WFLBY_EV_TALK_LIST_SEL_TK_S_00:	
			case WFLBY_EV_TALK_LIST_SEL_TK_S_01:	
			case WFLBY_EV_TALK_LIST_SEL_TK_S_02:	
			case WFLBY_EV_TALK_LIST_SEL_TK_S_04:	

				// 会話項目設定
				p_evwk->sel_k_data = (p_evwk->sel_k*WFLBY_EV_TALK_LIST_SEL_TK_S_MAX) + result;
				WFLBY_SYSTEM_SendTalkData( p_system, p_evwk->sel_k_data+WFLBY_EV_TALK_TYPE_TALK_COL_00 );

				// 自分の選択を表示チェックへ
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_B_SEQ_MSG_BT );

				del = TRUE;
				break;

			case WFLBY_EV_TALK_LIST_SEL_TK_S_MAX:			// 	B[わからない]
				// 会話項目設定
				p_evwk->sel_k_data = WFLBY_EV_TALK_TYPE_TALK_NONE;
				WFLBY_SYSTEM_SendTalkData( p_system, p_evwk->sel_k_data );

				// 答えられないときの終了へ
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_TALK_B_SEQ_NONEENDMSG );

				del = TRUE;
				break;

			default:
				WFLBY_EV_TALK_WK_CheckAndPrintSelectWait( p_evwk, p_rmwk );
				del = FALSE;
				break;
			}

			if( del ){
				WFLBY_ROOM_LISTWIN_End( p_rmwk, NULL, NULL );
				WFLBY_EV_TALK_WK_ExitBmpList( &p_evwk->bmplist, p_rmwk );
				WFLBY_EV_TALK_WK_StopSelectWait( p_evwk );

				// 自分も選択したので会話履歴に設定
				WFLBY_EV_TALK_WK_SetRirekiData( p_evwk, cp_youpr );
			}
		}
		break;

	// 自分の選択の表示
	case WFLBY_EV_TALK_B_SEQ_MSG_BT:
		// 文字列作成
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_osyaberi_04_01+p_evwk->sel_k_data );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_osyaberi_05_01+p_evwk->sel_k_data );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// 選択項目が一緒かチェック
		if( p_evwk->recv_k_data == p_evwk->sel_k_data ){

			// いっしょだねへ
			WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
					WFLBY_EV_TALK_B_SEQ_MSG_ABT_EQ, p_wk, WFLBY_EV_TALK_B_SEQ_MSGWAIT );
		}else{
			// ありがとうへ
			WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
					WFLBY_EV_TALK_B_SEQ_MSG_AT_END, p_wk, WFLBY_EV_TALK_B_SEQ_MSGWAIT );
		}
		break;

	// 選択項目が同じとき
	case WFLBY_EV_TALK_B_SEQ_MSG_ABT_EQ:	
		// 文字列作成
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_osyaberi_00_06 );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_osyaberi_00_06 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// ありがとうへ
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_B_SEQ_MSG_AT_END, p_wk, WFLBY_EV_TALK_B_SEQ_MSGWAIT );
		break;

	// 会話終了メッセージ
	case WFLBY_EV_TALK_B_SEQ_MSG_AT_END:	
		// 文字列作成
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_osyaberi_00_07 );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_osyaberi_00_07 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// ありがとうへ
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_B_SEQ_RECV_A_SEC, p_wk, WFLBY_EV_TALK_B_SEQ_MSGWAIT );
		
		break;

	// エラー終了
	case WFLBY_EV_TALK_B_SEQ_ERR_ENDMSG:	

		// リストが出ているかもしれないので消す
		WFLBY_ROOM_LISTWIN_End( p_rmwk, NULL, NULL );
		WFLBY_EV_TALK_WK_ExitBmpList( &p_evwk->bmplist, p_rmwk );

		// 選択中メッセージ表示
		// 文字列作成
//		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_aisatu_04_04 );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_aisatu_04_04 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// もうエラーチェック必要なし
		p_evwk->err_chk = FALSE;

		// ウエイトの後、リスト表示
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_B_SEQ_END, p_wk, WFLBY_EV_TALK_B_SEQ_WAIT );
//				WFLBY_EV_TALK_B_SEQ_END, p_wk, WFLBY_EV_TALK_B_SEQ_MSGWAIT );

		// 強制終了したことを教える
		WFLBY_SYSTEM_SendTalkExEnd( p_system );
		break;

	// 通常終了
	case WFLBY_EV_TALK_B_SEQ_ENDMSG:		
		// 選択中メッセージ表示
		// 文字列作成
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_aisatu_04_02 );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_aisatu_04_02 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// もうエラーチェック必要なし
		p_evwk->err_chk = FALSE;

		// ウエイトの後、リスト表示
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_B_SEQ_END, p_wk, WFLBY_EV_TALK_B_SEQ_WAIT );
//				WFLBY_EV_TALK_B_SEQ_END, p_wk, WFLBY_EV_TALK_B_SEQ_MSGWAIT );
		break;

	// こたえないとき終了
	case WFLBY_EV_TALK_B_SEQ_NONEENDMSG:	
		// 文字列作成
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
//		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_osyaberi_00_04 );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_osyaberi_00_04 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// ウエイトの後、リスト表示
		WFLBY_EV_TALK_WK_Wait( p_evwk, WFLBY_EV_TALK_MSG_WAIT, 
				WFLBY_EV_TALK_B_SEQ_ENDMSG, p_wk, WFLBY_EV_TALK_B_SEQ_MSGWAIT );
		break;
		
	// 待機処理
	case WFLBY_EV_TALK_B_SEQ_WAIT:
		WFLBY_EV_TALK_WK_WaitMain( p_evwk, p_wk );
		break;

	// メッセージの終了ウエイト
	case WFLBY_EV_TALK_B_SEQ_MSGWAIT:
		WFLBY_EV_TALK_WK_MsgEndWaitMain( p_evwk, p_wk, p_rmwk );
		break;

	// 終了
	case WFLBY_EV_TALK_B_SEQ_END:		

		// 最後にしたことに会話を設定
		if( p_evwk->rireki_flag == TRUE ){
			WFLBY_SYSTEM_SetLastAction( p_system, p_evwk->rireki_actno, p_evwk->rireki_userid );
		}

		// 会話など何もせずに終わるにきたら
		// 会話カウントを足す
		if( p_evwk->b_count_add == FALSE ){
			// 会話成立!!!
			WFLBY_SYSTEM_AddTalkCountEx( p_system, p_evwk->talk_idx );
		}

		// 会話終了にする
		WFLBY_SYSTEM_EndTalk( p_system );
		
		// メッセージ終了
		WFLBY_ROOM_TALKWIN_Off( p_rmwk );

		// ワーク内の破棄
		WFLBY_EV_TALK_WK_Exit( p_evwk );
		
		// イベントワーク破棄
		WFLBY_EVENTWK_DeleteWk( p_wk );

		// 通常シーケンスへ
		WFLBY_SYSTEM_SetMyStatus( p_system, WFLBY_STATUS_LOGIN );
		{
			WFLBY_3DPERSON* p_player;
			WFLBY_3DOBJCONT* p_objcont;

			// プレイヤーを取得
			p_objcont = WFLBY_ROOM_GetObjCont( p_rmwk );
			p_player = WFLBY_3DOBJCONT_GetPlayer( p_objcont );
			WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_player, WFLBY_3DOBJCONT_MOVEPLAYER );

			// プロフィール表示をやめる
			WFLBY_ROOM_UNDERWIN_TrCardOff( p_rmwk );
		}
		return TRUE;	// 割り込みイベント終了
	}

	WFLBY_EV_TALK_WK_MainSelectWait( p_evwk );

	return FALSE;
}



//----------------------------------------------------------------------------
/**
 *	@brief	トピックお姉さん会話
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_EV_DEF_PlayerA_SWTOPIC_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	WFLBY_EV_DEF_PLAYER_WFTOPIC_WK* p_evwk;
	STRBUF* p_str;
	WFLBY_SYSTEM* p_system;

	p_evwk = WFLBY_EVENTWK_GetWk( p_wk );
	p_system = WFLBY_ROOM_GetSystemData( p_rmwk );
	
	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){
	// 初期化
	case WFLBY_EV_DEF_PLAYER_TOPIC_INIT:		
		p_evwk = WFLBY_EVENTWK_AllocWk( p_wk, sizeof(WFLBY_EV_DEF_PLAYER_WFTOPIC_WK) );

		// 話しかけられた音
		Snd_SePlay( WFLBY_SND_STAFF );

		// クローズ前かあとかで変更
		if( WFLBY_SYSTEM_Event_GetEndCM( p_system ) == TRUE ){

			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_CLOSE );
		}else{

			// PARADE中かチェック
			if( WFLBY_SYSTEM_Event_GetParade( p_system ) == TRUE ){
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_PARADE );
			}else{
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSG00 );
			}
		}
		break;

	// PARADEメッセージ
	case WFLBY_EV_DEF_PLAYER_TOPIC_PARADE:	
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoc_06 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		p_evwk->ret_seq = WFLBY_EV_DEF_PLAYER_TOPIC_END;
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGWAIT );
		break;

	// クローズ前メッセージ
	case WFLBY_EV_DEF_PLAYER_TOPIC_CLOSE:	
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoa_04 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		p_evwk->ret_seq = WFLBY_EV_DEF_PLAYER_TOPIC_END;
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGWAIT );
		break;

	// ようこそ
	case WFLBY_EV_DEF_PLAYER_TOPIC_MSG00:	
		
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infob_01 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		p_evwk->ret_seq = WFLBY_EV_DEF_PLAYER_TOPIC_MSG01;
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGWAIT );
		break;

	// どちらについて・・・
	case WFLBY_EV_DEF_PLAYER_TOPIC_MSG01:	
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infob_02 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		p_evwk->ret_seq = WFLBY_EV_DEF_PLAYER_TOPIC_LISTON;
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGWAIT );
		break;
		
	// リスト表示
	case WFLBY_EV_DEF_PLAYER_TOPIC_LISTON:	
		WFLBY_EV_TOPIC_WK_InitBmpList( &p_evwk->bmplist, p_rmwk );
		WFLBY_ROOM_LISTWIN_Start_Ex( p_rmwk, &p_evwk->bmplist.bmplist, 0, 0, 
				WFLBY_EV_DEF_PLAYER_TOPIC_LISTWIN_X, WFLBY_EV_DEF_PLAYER_TOPIC_LISTWIN_Y, 
				WFLBY_EV_DEF_PLAYER_TOPIC_LISTWIN_SIZX );
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_LISTWAIT );
		break;
		
	// リスト待ち
	case WFLBY_EV_DEF_PLAYER_TOPIC_LISTWAIT:	
		{
			u32 result;
			BOOL delete = FALSE;
			result = WFLBY_ROOM_LISTWIN_Main( p_rmwk );

			switch( result ){
			case WFLBY_EV_TOPIC_LIST_SEL_GADGET:
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGGADGET );
				delete = TRUE;
				break;
			case WFLBY_EV_TOPIC_LIST_SEL_MINI:
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGMINI );
				delete = TRUE;
				break;
			case WFLBY_EV_TOPIC_LIST_SEL_WLD:
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGWLD );
				delete = TRUE;
				break;
			case WFLBY_EV_TOPIC_LIST_SEL_TOP:
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGTOP );
				delete = TRUE;
				break;
			case WFLBY_EV_TOPIC_LIST_SEL_FOOT:
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGBOAD );
				delete = TRUE;
				break;
			case WFLBY_EV_TOPIC_LIST_SEL_ANKETO:
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGANKETO );
				delete = TRUE;
				break;
			case WFLBY_EV_TOPIC_LIST_SEL_TIME:	
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGEVE );
				delete = TRUE;
				break;
			case BMPLIST_CANCEL:
				Snd_SePlay( SEQ_SE_DP_SELECT );	// CANCEL音
			case WFLBY_EV_TOPIC_LIST_SEL_END:	
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGEND );
				delete = TRUE;
				break;
			
			default:
				break;
			}

			if( delete == TRUE ){
				WFLBY_ROOM_LISTWIN_End( p_rmwk, NULL, NULL );
				WFLBY_EV_TALK_WK_ExitBmpList( &p_evwk->bmplist, p_rmwk );
			}
		}
		break;

	// ガジェット
	case WFLBY_EV_DEF_PLAYER_TOPIC_MSGGADGET:
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infob_09 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		p_evwk->ret_seq = WFLBY_EV_DEF_PLAYER_TOPIC_MSG01;
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGWAIT );
		break;
		
	// ミニゲーム
	case WFLBY_EV_DEF_PLAYER_TOPIC_MSGMINI:	
		WFLBY_EV_TOPIC_WK_InitMinigameBmpList( &p_evwk->bmplist, p_rmwk );
		WFLBY_ROOM_LISTWIN_Start( p_rmwk, &p_evwk->bmplist.bmplist, 0, 0 );
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGMINI_SEL );
		break;

	// ミニゲームさらに選択
	case WFLBY_EV_DEF_PLAYER_TOPIC_MSGMINI_SEL:	
		{
			u32 result;
			BOOL delete = FALSE;
			result = WFLBY_ROOM_LISTWIN_Main( p_rmwk );

			switch( result ){
			case WFLBY_EV_TOPIC_LIST_SEL_MG_BALLSLOW:
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGMINI_BS );
				delete = TRUE;
				break;
				
			case WFLBY_EV_TOPIC_LIST_SEL_MG_BALANCE:
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGMINI_BB );
				delete = TRUE;
				break;
				
			case WFLBY_EV_TOPIC_LIST_SEL_MG_BALLOON:
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGMINI_BL );
				delete = TRUE;
				break;

			case BMPLIST_CANCEL:
				Snd_SePlay( SEQ_SE_DP_SELECT );	// CANCEL音
			case WFLBY_EV_TOPIC_LIST_SEL_MG_END:
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSG01 );
				delete = TRUE;
				break;

			
			default:
				break;
			}

			if( delete == TRUE ){
				WFLBY_ROOM_LISTWIN_End( p_rmwk, NULL, NULL );
				WFLBY_EV_TALK_WK_ExitBmpList( &p_evwk->bmplist, p_rmwk );
			}
		}
		break;
	
	// タマ投げ
	case WFLBY_EV_DEF_PLAYER_TOPIC_MSGMINI_BS:	
		WFLBY_ROOM_MSG_SetMinigame( p_rmwk, WFLBY_GAME_BALLSLOW, 0 );
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infob_10 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		p_evwk->ret_seq = WFLBY_EV_DEF_PLAYER_TOPIC_MSG01;
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGWAIT );
		break;
	
	// たまのり
	case WFLBY_EV_DEF_PLAYER_TOPIC_MSGMINI_BB:	
		WFLBY_ROOM_MSG_SetMinigame( p_rmwk, WFLBY_GAME_BALANCEBALL, 0 );
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infob_11 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		p_evwk->ret_seq = WFLBY_EV_DEF_PLAYER_TOPIC_MSG01;
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGWAIT );
		break;

	// ふうせんわり
	case WFLBY_EV_DEF_PLAYER_TOPIC_MSGMINI_BL:	
		WFLBY_ROOM_MSG_SetMinigame( p_rmwk, WFLBY_GAME_BALLOON, 0 );
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infob_12 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		p_evwk->ret_seq = WFLBY_EV_DEF_PLAYER_TOPIC_MSG01;
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGWAIT );
		break;
		
	// 世界時計
	case WFLBY_EV_DEF_PLAYER_TOPIC_MSGWLD:	
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infob_04 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		p_evwk->ret_seq = WFLBY_EV_DEF_PLAYER_TOPIC_MSG01;
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGWAIT );
		break;
		
	// ニュース
	case WFLBY_EV_DEF_PLAYER_TOPIC_MSGTOP:	
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infob_05 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		p_evwk->ret_seq = WFLBY_EV_DEF_PLAYER_TOPIC_MSG01;
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGWAIT );
		break;
		
	// 足跡ボード
	case WFLBY_EV_DEF_PLAYER_TOPIC_MSGBOAD:	
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infob_06 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		p_evwk->ret_seq = WFLBY_EV_DEF_PLAYER_TOPIC_MSG01;
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGWAIT );
		break;
		
	// イベント
	case WFLBY_EV_DEF_PLAYER_TOPIC_MSGEVE:	

		WFLBY_EV_TOPIC_WK_InitEventBmpList( &p_evwk->bmplist, p_rmwk );
		WFLBY_ROOM_LISTWIN_Start( p_rmwk, &p_evwk->bmplist.bmplist, 0, 0 );
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGEVE_SEL );
		break;

	// イベントさらに選択
	case WFLBY_EV_DEF_PLAYER_TOPIC_MSGEVE_SEL:
		{
			u32 result;
			BOOL delete = FALSE;
			result = WFLBY_ROOM_LISTWIN_Main( p_rmwk );

			switch( result ){
			case WFLBY_EV_TOPIC_LIST_SEL_EVE_LIGHTING:
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGEVE_LIGHTING );
				delete = TRUE;
				break;
				
			case WFLBY_EV_TOPIC_LIST_SEL_EVE_FIRE:
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGEVE_FIRE );
				delete = TRUE;
				break;
				
			case WFLBY_EV_TOPIC_LIST_SEL_EVE_PARADE:
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGEVE_PARADE );
				delete = TRUE;
				break;

			case BMPLIST_CANCEL:
				Snd_SePlay( SEQ_SE_DP_SELECT );	// CANCEL音
			case WFLBY_EV_TOPIC_LIST_SEL_EVE_END:
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSG01 );
				delete = TRUE;
				break;

			default:
				break;
			}

			if( delete == TRUE ){
				WFLBY_ROOM_LISTWIN_End( p_rmwk, NULL, NULL );
				WFLBY_EV_TALK_WK_ExitBmpList( &p_evwk->bmplist, p_rmwk );
			}
		}
		break;

	// イベント　ライティング
	case WFLBY_EV_DEF_PLAYER_TOPIC_MSGEVE_LIGHTING:	
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infob_13 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		p_evwk->ret_seq = WFLBY_EV_DEF_PLAYER_TOPIC_MSG01;
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGWAIT );
		break;

	// イベント　はなび
	case WFLBY_EV_DEF_PLAYER_TOPIC_MSGEVE_FIRE:		
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infob_14 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		p_evwk->ret_seq = WFLBY_EV_DEF_PLAYER_TOPIC_MSG01;
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGWAIT );
		break;

	// イベント　パレード
	case WFLBY_EV_DEF_PLAYER_TOPIC_MSGEVE_PARADE:	
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infob_15 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		p_evwk->ret_seq = WFLBY_EV_DEF_PLAYER_TOPIC_MSG01;
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGWAIT );
		break;

	// アンケート
	case WFLBY_EV_DEF_PLAYER_TOPIC_MSGANKETO:
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infob_16 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		p_evwk->ret_seq = WFLBY_EV_DEF_PLAYER_TOPIC_MSG01;
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGWAIT );
		break;
		
	// やめる
	case WFLBY_EV_DEF_PLAYER_TOPIC_MSGEND:	
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infob_08 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		p_evwk->ret_seq = WFLBY_EV_DEF_PLAYER_TOPIC_END;
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_MSGWAIT );
		break;
		
	// メッセージ終了待ち
	case WFLBY_EV_DEF_PLAYER_TOPIC_MSGWAIT:	
		if( WFLBY_ROOM_TALKWIN_WaitEnd( p_rmwk ) ){
			WFLBY_EVENTWK_SetSeq( p_wk, p_evwk->ret_seq );
		}
		break;

	// 終了
	case WFLBY_EV_DEF_PLAYER_TOPIC_END:	
		WFLBY_EVENTWK_DeleteWk( p_wk );
		WFLBY_ROOM_TALKWIN_Off( p_rmwk );

		{
			WFLBY_3DOBJCONT* p_objcont;
			WFLBY_3DPERSON* p_person;

			p_objcont	= WFLBY_ROOM_GetObjCont( p_rmwk );
			p_person	= WFLBY_3DOBJCONT_GetPlayer( p_objcont );
			WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_person, WFLBY_3DOBJCONT_MOVEPLAYER );
		}
		return TRUE;
	}

	return FALSE;
}



//----------------------------------------------------------------------------
/**
 *	@brief	トピックお兄さん会話		遊びを勧める人
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_EV_DEF_PlayerA_SWTOPIC_PLAYED_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	WFLBY_EV_DEF_PLAYER_WFTOPIC_WK* p_evwk;
	WFLBY_SYSTEM* p_system;
	STRBUF* p_str;

	p_evwk = WFLBY_EVENTWK_GetWk( p_wk );
	p_system = WFLBY_ROOM_GetSystemData( p_rmwk );
	
	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){
	case WFLBY_EV_DEF_PLAYER_TOPIC_PLAYED_INIT:		// 初期化

		// 話しかけられた音
		Snd_SePlay( WFLBY_SND_STAFF );

		// PARADE中はPARADEのことをいう
		if( WFLBY_SYSTEM_Event_GetEndCM( p_system ) == FALSE ){
			// PARADE中かチェック
			if( WFLBY_SYSTEM_Event_GetParade( p_system ) == TRUE ){
				p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoc_06 );
				// メッセージ描画
				WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_PLAYED_MSGWAIT );
				break;
			}
		}else{
			// 終了時は終了時のことをいう
			p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoa_04 );
			// メッセージ描画
			WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_PLAYED_MSGWAIT );
			break;
		}
		
		{
			u32 played_type;



			played_type = WFLBY_SYSTEM_PLAYED_GetOsusumePlayType( p_system );
			// 何を出すかチェック
			switch(played_type){
			// タマ投げ
			case WFLBY_PLAYED_BALLTHROW:	
				WFLBY_ROOM_MSG_SetMinigame( p_rmwk, WFLBY_GAME_BALLSLOW, 0 );
				p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoc_01 );
				break;
			// Balanceボール
			case WFLBY_PLAYED_BALANCEBALL:
				WFLBY_ROOM_MSG_SetMinigame( p_rmwk, WFLBY_GAME_BALANCEBALL, 0 );
				p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoc_01 );
				break;
			// ふうせんわり
			case WFLBY_PLAYED_BALLOON:		
				WFLBY_ROOM_MSG_SetMinigame( p_rmwk, WFLBY_GAME_BALLOON, 0 );
				p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoc_01 );
				break;
			// 世界時計	
			case WFLBY_PLAYED_WORLDTIMER:	
				WFLBY_ROOM_MSG_SetMinigame( p_rmwk, WFLBY_GAME_WLDTIMER, 0 );
				p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoc_05 );
				break;
			// 広場ニュース
			case WFLBY_PLAYED_TOPIC:			
				WFLBY_ROOM_MSG_SetMinigame( p_rmwk, WFLBY_GAME_NEWS, 0 );
				p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoc_05 );
				break;
			// 足跡BOARD
			case WFLBY_PLAYED_FOOTBOARD:		
				p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoc_02 );
				break;
			// アンケート
			case WFLBY_PLAYED_ANKETO:
				p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoc_07 );
				break;
			// 話しかけ
			case WFLBY_PLAYED_TALK:			
				p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoc_03 );
				break;
			// もう全部遊んだ
			default:
			case WFLBY_PLAYED_NUM:			
				p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoc_04 );
				break;
			}


			// メッセージ描画
			WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		}

		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_PLAYED_MSGWAIT );
		break;
		
	case WFLBY_EV_DEF_PLAYER_TOPIC_PLAYED_MSGWAIT:	// メッセージ表示完了待ち
		if( WFLBY_ROOM_TALKWIN_WaitEnd( p_rmwk ) ){
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_TOPIC_PLAYED_END );
		}
		break;
		
	case WFLBY_EV_DEF_PLAYER_TOPIC_PLAYED_END:	
		WFLBY_ROOM_TALKWIN_Off( p_rmwk );

		{
			WFLBY_3DOBJCONT* p_objcont;
			WFLBY_3DPERSON* p_person;

			p_objcont	= WFLBY_ROOM_GetObjCont( p_rmwk );
			p_person	= WFLBY_3DOBJCONT_GetPlayer( p_objcont );
			WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_person, WFLBY_3DOBJCONT_MOVEPLAYER );
		}
		return TRUE;
	}

	return FALSE;
}




#ifdef WFLBY_DEBUG_TALK_ALLWORLD
static u8 s_WFLBY_DEBUG_TALK_ALLWORLD_COUNT_MY = 0;
static u8 s_WFLBY_DEBUG_TALK_ALLWORLD_COUNT_YOU = 0;
static u8 s_WFLBY_DEBUG_TALK_ALLWORLD_COUNT_TIMEZONE = 0;
BOOL WFLBY_EV_TALK_StartA_AllWorld( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	WFLBY_EV_TALK_WK* p_evwk;
	WFLBY_SYSTEM* p_system;
	STRBUF* p_str;
	const WFLBY_USER_PROFILE* cp_mypr;
	const WFLBY_USER_PROFILE* cp_youpr;

	p_system	= WFLBY_ROOM_GetSystemData( p_rmwk );		// システムデータ取得
	p_evwk		= WFLBY_EVENTWK_GetWk( p_wk );				// イベントデータ取得

	cp_mypr		= WFLBY_SYSTEM_GetMyProfile( p_system );
	cp_youpr	= WFLBY_SYSTEM_GetUserProfile( p_system, 1 );	// 適当に


	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){
	// 表示
	case 0:
		WFLBY_SYSTEM_DEBUG_SetLangCode( (WFLBY_USER_PROFILE*)cp_mypr, s_WFLBY_DEBUG_TALK_ALLWORLD_COUNT_MY );
		WFLBY_SYSTEM_DEBUG_SetLangCode( (WFLBY_USER_PROFILE*)cp_youpr, s_WFLBY_DEBUG_TALK_ALLWORLD_COUNT_YOU );

		WFLBY_EV_TALK_WK_AisatuDraw( NULL, p_system, p_rmwk, s_WFLBY_DEBUG_TALK_ALLWORLD_COUNT_TIMEZONE, cp_mypr, cp_youpr );

		WFLBY_EVENTWK_SetSeq( p_wk, 1 );
		break;

	// ループ
	case 1:

		// Aがおされるまでまつ
		if( (sys.trg & PAD_BUTTON_A) == 0 ){
			break;
		}

		if( (s_WFLBY_DEBUG_TALK_ALLWORLD_COUNT_TIMEZONE+1) < 3 ){
			s_WFLBY_DEBUG_TALK_ALLWORLD_COUNT_TIMEZONE++;
		}else{
			s_WFLBY_DEBUG_TALK_ALLWORLD_COUNT_TIMEZONE = 0;

			if( (s_WFLBY_DEBUG_TALK_ALLWORLD_COUNT_YOU+1) < WFLBY_EV_TALK_LANG_NUM ){
				s_WFLBY_DEBUG_TALK_ALLWORLD_COUNT_YOU ++;
			}else{
				s_WFLBY_DEBUG_TALK_ALLWORLD_COUNT_YOU = 0;

				if( (s_WFLBY_DEBUG_TALK_ALLWORLD_COUNT_MY+1) < WFLBY_EV_TALK_LANG_NUM ){
					s_WFLBY_DEBUG_TALK_ALLWORLD_COUNT_MY ++;
				}else{

					// 終了へ
					WFLBY_EVENTWK_SetSeq( p_wk, 2 );
					break;
				}
			}
		}

		// 再表示へ
		WFLBY_EVENTWK_SetSeq( p_wk, 0 );
		break;


	// 終了
	case 2:
		// メッセージ終了
		WFLBY_ROOM_TALKWIN_Off( p_rmwk );

		// ワーク内の破棄
		WFLBY_EV_TALK_WK_Exit( p_evwk );
		
		// イベントワーク破棄
		WFLBY_EVENTWK_DeleteWk( p_wk );

		// 通常シーケンスへ
		WFLBY_SYSTEM_SetMyStatus( p_system, WFLBY_STATUS_LOGIN );
		{
			WFLBY_3DPERSON* p_player;
			WFLBY_3DOBJCONT* p_objcont;

			// プレイヤーを取得
			p_objcont = WFLBY_ROOM_GetObjCont( p_rmwk );
			p_player = WFLBY_3DOBJCONT_GetPlayer( p_objcont );
			WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_player, WFLBY_3DOBJCONT_MOVEPLAYER );
		}
		return TRUE;	// 割り込みイベント終了
		break;
	}


	return FALSE;
}
#endif



//-----------------------------------------------------------------------------
/**
 *			プライベート関数
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief	挨拶時間ゾーンを取得する
 *
 *	@param	cp_pr		プロフィール
 *
 *	@return	時間ゾーン
 */
//-----------------------------------------------------------------------------
static u32 WFLBY_EV_TALK_GetTimeZone( const WFLBY_USER_PROFILE* cp_pr )
{
	WFLBY_TIME time;
	time = WFLBY_SYSTEM_GetProfileWldTime( cp_pr );
	
	switch( time.hour ){
	// 朝
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
		return WFLBY_AISATSU_MORNING;

	// 昼
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
		return WFLBY_AISATSU_NOON;

	// 夜
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
	case 24:
	case 0:
	case 1:
	case 2:
	case 3:
		return WFLBY_AISATSU_NIGHT;
	}

	GF_ASSERT( 0 );	// ありえない
	return WFLBY_AISATSU_NOON;
}



//----------------------------------------------------------------------------
/**
 *	@brief	文字列を取得する
 *
 *	@param	p_rmwk		ワーク
 *	@param	plidx		その文字を発言するプレイヤーインデックス
 *	@param	msgidx		メッセージインデックス
 *
 *	@return	文字列
 */
//-----------------------------------------------------------------------------
static STRBUF* WFLBY_EV_TALK_GetStrBuf( WFLBY_ROOMWK* p_rmwk, u32 plidx, u32 msgidx )
{
	BOOL vip;
	WFLBY_SYSTEM* p_system;
	int i;

	p_system = WFLBY_ROOM_GetSystemData( p_rmwk );
	
	// plidxの人がVIPじゃないかチェック
	vip = WFLBY_SYSTEM_GetUserVipFlag( p_system, plidx );
	if( vip == TRUE ){
		
		// msgidxにVIP用メッセージがあるかチェック
		for( i=0; i<NELEMS(sc_WFLBY_VIP_MSGTBL); i++ ){
			if( msgidx == sc_WFLBY_VIP_MSGTBL[i].normal_msg ){
				// VIP用メッセージに書き換える
				msgidx = sc_WFLBY_VIP_MSGTBL[i].vip_msg;	
				break;
			}
		}
	}
	
	return WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msgidx );
}

//----------------------------------------------------------------------------
/**
 *	@brief	会話２回目にあったときの文字列を取得する
 *
 *	@param	p_rmwk		ワーク
 *	@param	aidx		話しかけた側のIDX
 *	@param	bidx		話しかけられた側のIDX
 *
 *	@return	文字列
 */
//-----------------------------------------------------------------------------
static STRBUF* WFLBY_EV_TALK_GetStrBuf_Aisatu03_02( WFLBY_ROOMWK* p_rmwk, u32 aidx, u32 bidx )
{
	BOOL a_vip, b_vip;
	WFLBY_SYSTEM* p_system;
	u32 msgidx;

	p_system = WFLBY_ROOM_GetSystemData( p_rmwk );
	
	// plidxの人がVIPじゃないかチェック
	a_vip	= WFLBY_SYSTEM_GetUserVipFlag( p_system, aidx );
	b_vip	= WFLBY_SYSTEM_GetUserVipFlag( p_system, bidx );
	
	msgidx = msg_hiroba_aisatu_03_02;
	if( (a_vip == TRUE) && (b_vip == TRUE) ){
		msgidx = msg_vip_aisatu_03_02_01;
	}
	else if( (a_vip == TRUE) && (b_vip == FALSE) ){
		msgidx = msg_vip_aisatu_03_02;
	}
	else if( (a_vip == FALSE) && (b_vip == TRUE) ){
		msgidx = msg_vip_aisatu_03_02_00;
	}
	
	return WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msgidx );
}






//----------------------------------------------------------------------------
/**
 *	@brief	会話データ初期化
 *
 *	@param	p_evwk		イベントワーク
 *	@param	p_system	システムデータ
 *	@param	idx			会話相手インデックス
 *
 *	@retval	TRUE	会話相手在籍
 *	@retval	FALSE	会話相手がいない
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EV_TALK_WK_Init( WFLBY_EV_TALK_WK* p_evwk, WFLBY_SYSTEM* p_system, u32 idx )
{
	GF_ASSERT( idx != DWC_LOBBY_INVALID_USER_ID );
	
	// 会話相手
	p_evwk->talk_idx = idx;

	//自分
	p_evwk->my_idx = WFLBY_SYSTEM_GetMyIdx( p_system );

	// エラーチェックON
	p_evwk->err_chk = TRUE;
	
	// プロフィールデータを抜き出す
	{
		const WFLBY_USER_PROFILE* cp_mypr;
		const WFLBY_USER_PROFILE* cp_youpr;
		cp_mypr		= WFLBY_SYSTEM_GetMyProfileLocal( p_system );
		cp_youpr	= WFLBY_SYSTEM_GetUserProfile( p_system, p_evwk->talk_idx );
		if( cp_youpr == NULL ){
			return FALSE;
		}

		// タイムゾーン取得
		p_evwk->my_timezone		= WFLBY_EV_TALK_GetTimeZone( cp_mypr );
		p_evwk->you_timezone	= WFLBY_EV_TALK_GetTimeZone( cp_youpr );

	}

	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ワークの破棄処理
 *
 *	@param	p_evwk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_TALK_WK_Exit( WFLBY_EV_TALK_WK* p_evwk )
{
}



//----------------------------------------------------------------------------
/**
 *	@brief	ウエイト	開始
 *
 *	@param	p_evwk		ワーク
 *	@param	wait		ウエイト値（メッセージ終了待ちのときはどんな値でもＯＫ）
 *	@param	ret_seq		戻ってくるシーケンス
 *	@param	p_wk		イベントワーク
 *	@param	wait_seq	待機シーケンス
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_TALK_WK_Wait( WFLBY_EV_TALK_WK* p_evwk, u8 wait, u8 ret_seq, WFLBY_EVENTWK* p_wk, u8 wait_seq )
{
	p_evwk->wait		= wait;
	p_evwk->next_seq	= ret_seq;

	// 待機シーケンスへ
	WFLBY_EVENTWK_SetSeq( p_wk, wait_seq );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ウエイトメイン
 *
 *	@param	p_evwk	ワーク
 *	@param	p_wk	イベントワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_TALK_WK_WaitMain( WFLBY_EV_TALK_WK* p_evwk, WFLBY_EVENTWK* p_wk )
{
	if( p_evwk->wait > 0 ){
		p_evwk->wait --;
	}else{

		// シーケンスを戻す
		WFLBY_EVENTWK_SetSeq( p_wk, p_evwk->next_seq );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージ終了待ち
 *
 *	@param	p_evwk	ワーク
 *	@param	p_wk	イベントワーク
 *	@param	p_rmwk	部屋ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_TALK_WK_MsgEndWaitMain( WFLBY_EV_TALK_WK* p_evwk, WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk )
{
	if( WFLBY_ROOM_TALKWIN_WaitEnd( p_rmwk ) ){
		// シーケンスを戻す
		WFLBY_EVENTWK_SetSeq( p_wk, p_evwk->next_seq );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	挨拶の表示
 *
 *	@param	p_evwk		ワーク
 *	@param	p_system	システムワーク
 *	@param	p_rmwk		ルームワーク
 *	@param	timezone	表示する時間ゾーン
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_TALK_WK_AisatuDraw( WFLBY_EV_TALK_WK* p_evwk, WFLBY_SYSTEM* p_system, WFLBY_ROOMWK* p_rmwk, u32 timezone, const WFLBY_USER_PROFILE* cp_mypr, const WFLBY_USER_PROFILE* cp_youpr )
{
	u32 data_id;
	u32 my_lang, you_lang;
	static const u16 sc_WFLBY_EV_TALK_AISATSU_MSGIDX[WFLBY_EV_TALK_LANG_NUM][WFLBY_EV_TALK_LANG_NUM] = {
		// NONE
		{
			msg_hiroba_aisatu_01_32,		// NONE
			msg_hiroba_aisatu_01_32,		// LANG_JAPAN
			msg_hiroba_aisatu_01_32,		// LANG_ENGLISH
			msg_hiroba_aisatu_01_32,		// LANG_FRANCE
			msg_hiroba_aisatu_01_32,		// LANG_ITALY
			msg_hiroba_aisatu_01_32,		// LANG_GERMANY
			msg_hiroba_aisatu_01_32,		// NONE
			msg_hiroba_aisatu_01_32,		// LANG_SPAIN
		},

		// LANG_JAPAN
		{
			msg_hiroba_aisatu_01_02,		// NONE
			msg_hiroba_aisatu_01_01,		// LANG_JAPAN
			msg_hiroba_aisatu_01_02,		// LANG_ENGLISH
			msg_hiroba_aisatu_01_04,		// LANG_FRANCE
			msg_hiroba_aisatu_01_06,		// LANG_ITALY
			msg_hiroba_aisatu_01_08,		// LANG_GERMANY
			msg_hiroba_aisatu_01_02,		// NONE
			msg_hiroba_aisatu_01_10,		// LANG_SPAIN
		},

		// LANG_ENGLISH
		{
			msg_hiroba_aisatu_01_33,		// NONE
			msg_hiroba_aisatu_01_03,		// LANG_JAPAN
			msg_hiroba_aisatu_01_33,		// LANG_ENGLISH
			msg_hiroba_aisatu_01_12,		// LANG_FRANCE
			msg_hiroba_aisatu_01_14,		// LANG_ITALY
			msg_hiroba_aisatu_01_16,		// LANG_GERMANY
			msg_hiroba_aisatu_01_33,		// NONE
			msg_hiroba_aisatu_01_18,		// LANG_SPAIN
		},

		// LANG_FRANCE
		{
			msg_hiroba_aisatu_01_13,		// NONE
			msg_hiroba_aisatu_01_05,		// LANG_JAPAN
			msg_hiroba_aisatu_01_13,		// LANG_ENGLISH
			msg_hiroba_aisatu_01_34,		// LANG_FRANCE
			msg_hiroba_aisatu_01_20,		// LANG_ITALY
			msg_hiroba_aisatu_01_22,		// LANG_GERMANY
			msg_hiroba_aisatu_01_13,		// NONE
			msg_hiroba_aisatu_01_24,		// LANG_SPAIN
		},

		// LANG_ITALY
		{
			msg_hiroba_aisatu_01_15,		// NONE
			msg_hiroba_aisatu_01_07,		// LANG_JAPAN
			msg_hiroba_aisatu_01_15,		// LANG_ENGLISH
			msg_hiroba_aisatu_01_21,		// LANG_FRANCE
			msg_hiroba_aisatu_01_35,		// LANG_ITALY
			msg_hiroba_aisatu_01_26,		// LANG_GERMANY
			msg_hiroba_aisatu_01_15,		// NONE
			msg_hiroba_aisatu_01_28,		// LANG_SPAIN
		},

		// LANG_GERMANY
		{
			msg_hiroba_aisatu_01_17,		// NONE
			msg_hiroba_aisatu_01_09,		// LANG_JAPAN
			msg_hiroba_aisatu_01_17,		// LANG_ENGLISH
			msg_hiroba_aisatu_01_23,		// LANG_FRANCE
			msg_hiroba_aisatu_01_27,		// LANG_ITALY
			msg_hiroba_aisatu_01_36,		// LANG_GERMANY
			msg_hiroba_aisatu_01_17,		// NONE
			msg_hiroba_aisatu_01_30,		// LANG_SPAIN
		},

		// NONE
		{
			msg_hiroba_aisatu_01_32,		// NONE
			msg_hiroba_aisatu_01_32,		// LANG_JAPAN
			msg_hiroba_aisatu_01_32,		// LANG_ENGLISH
			msg_hiroba_aisatu_01_32,		// LANG_FRANCE
			msg_hiroba_aisatu_01_32,		// LANG_ITALY
			msg_hiroba_aisatu_01_32,		// LANG_GERMANY
			msg_hiroba_aisatu_01_32,		// NONE
			msg_hiroba_aisatu_01_32,		// LANG_SPAIN
		},

		// LANG_SPAIN
		{
			msg_hiroba_aisatu_01_19,		// NONE
			msg_hiroba_aisatu_01_11,		// LANG_JAPAN
			msg_hiroba_aisatu_01_19,		// LANG_ENGLISH
			msg_hiroba_aisatu_01_25,		// LANG_FRANCE
			msg_hiroba_aisatu_01_29,		// LANG_ITALY
			msg_hiroba_aisatu_01_31,		// LANG_GERMANY
			msg_hiroba_aisatu_01_19,		// NONE
			msg_hiroba_aisatu_01_37,		// LANG_SPAIN
		},
	};

	// 自分の挨拶をワードセットに設定
	WFLBY_EV_TALK_WK_Aisatu_SetWordSet( p_rmwk, 0, timezone, cp_mypr );
	// 相手の挨拶をワードセットに設定
	WFLBY_EV_TALK_WK_Aisatu_SetWordSet( p_rmwk, 1, timezone, cp_youpr );

	// 自分と相手の国コード取得
	// 不明な国コードなら英語にする
	my_lang = WFLBY_SYSTEM_GetProfileRagionCode( cp_mypr );
	you_lang = WFLBY_SYSTEM_GetProfileRagionCode( cp_youpr );

	data_id = sc_WFLBY_EV_TALK_AISATSU_MSGIDX[my_lang][you_lang];
	
	
/*	

	// 相手と自分の国から表示する文字データを取得
	switch( WFLBY_SYSTEM_GetProfileRagionCode( cp_mypr ) ){
	// 自分が日本
	case LANG_JAPAN:
		WFLBY_ROOM_MSG_SetAisatsuJapan( p_rmwk, 0, timezone );
		switch( WFLBY_SYSTEM_GetProfileRagionCode( cp_youpr ) ){
		case LANG_JAPAN:	
			data_id = msg_hiroba_aisatu_01_01;
			WFLBY_ROOM_MSG_SetAisatsuJapan( p_rmwk, 1, timezone );
			break;
		case LANG_ENGLISH:	
			data_id = msg_hiroba_aisatu_01_02; 
			WFLBY_ROOM_MSG_SetAisatsuEnglish( p_rmwk, 1, timezone );
			break;
		case LANG_FRANCE:	
			data_id = msg_hiroba_aisatu_01_04; 
			WFLBY_ROOM_MSG_SetAisatsuFrance( p_rmwk, 1, timezone );
			break;
		case LANG_ITALY:	
			data_id = msg_hiroba_aisatu_01_06; 
			WFLBY_ROOM_MSG_SetAisatsuItaly( p_rmwk, 1, timezone );
			break;
		case LANG_GERMANY:	
			data_id = msg_hiroba_aisatu_01_08; 
			WFLBY_ROOM_MSG_SetAisatsuGerMany( p_rmwk, 1, timezone );
			break;
		case LANG_SPAIN:	
			data_id = msg_hiroba_aisatu_01_10; 
			WFLBY_ROOM_MSG_SetAisatsuSpain( p_rmwk, 1, timezone );
			break;
		default:			
			data_id = msg_hiroba_aisatu_01_02; 
			WFLBY_ROOM_MSG_SetAisatsuEnglish( p_rmwk, 1, timezone );
			break;
		}
		break;

	// 自分がフランス
	case LANG_FRANCE:	
		WFLBY_ROOM_MSG_SetAisatsuFrance( p_rmwk, 0, timezone );
		switch( WFLBY_SYSTEM_GetProfileRagionCode( cp_youpr ) ){
		case LANG_JAPAN:	
			data_id = msg_hiroba_aisatu_01_05; 
			WFLBY_ROOM_MSG_SetAisatsuJapan( p_rmwk, 1, timezone );
			break;
		case LANG_ENGLISH:	
			data_id = msg_hiroba_aisatu_01_13; 
			WFLBY_ROOM_MSG_SetAisatsuEnglish( p_rmwk, 1, timezone );
			break;
		case LANG_FRANCE:	
			data_id = msg_hiroba_aisatu_01_33; 
			WFLBY_ROOM_MSG_SetAisatsuFrance( p_rmwk, 1, timezone );
			break;
		case LANG_ITALY:	
			data_id = msg_hiroba_aisatu_01_19; 
			WFLBY_ROOM_MSG_SetAisatsuItaly( p_rmwk, 1, timezone );
			break;
		case LANG_GERMANY:	
			data_id = msg_hiroba_aisatu_01_21; 
			WFLBY_ROOM_MSG_SetAisatsuGerMany( p_rmwk, 1, timezone );
			break;
		case LANG_SPAIN:	
			data_id = msg_hiroba_aisatu_01_22; 
			WFLBY_ROOM_MSG_SetAisatsuSpain( p_rmwk, 1, timezone );
			break;
		default:			
			data_id = msg_hiroba_aisatu_01_13; 
			WFLBY_ROOM_MSG_SetAisatsuEnglish( p_rmwk, 1, timezone );
			break;
		}
		break;

	// 自分がイタリア
	case LANG_ITALY:
		WFLBY_ROOM_MSG_SetAisatsuItaly( p_rmwk, 0, timezone );
		switch( WFLBY_SYSTEM_GetProfileRagionCode( cp_youpr ) ){
		case LANG_JAPAN:	
			data_id = msg_hiroba_aisatu_01_07; 
			WFLBY_ROOM_MSG_SetAisatsuJapan( p_rmwk, 1, timezone );
			break;
		case LANG_ENGLISH:	
			data_id = msg_hiroba_aisatu_01_14; 
			WFLBY_ROOM_MSG_SetAisatsuEnglish( p_rmwk, 1, timezone );
			break;
		case LANG_FRANCE:	
			data_id = msg_hiroba_aisatu_01_20; 
			WFLBY_ROOM_MSG_SetAisatsuFrance( p_rmwk, 1, timezone );
			break;
		case LANG_ITALY:	
			data_id = msg_hiroba_aisatu_01_34; 
			WFLBY_ROOM_MSG_SetAisatsuItaly( p_rmwk, 1, timezone );
			break;
		case LANG_GERMANY:	
			data_id = msg_hiroba_aisatu_01_08; 
			WFLBY_ROOM_MSG_SetAisatsuGerMany( p_rmwk, 1, timezone );
			break;
		case LANG_SPAIN:	
			data_id = msg_hiroba_aisatu_01_10; 
			WFLBY_ROOM_MSG_SetAisatsuSpain( p_rmwk, 1, timezone );
			break;
		default:			
			data_id = msg_hiroba_aisatu_01_14; 
			WFLBY_ROOM_MSG_SetAisatsuEnglish( p_rmwk, 1, timezone );
			break;
		}
		break;

	// 自分がドイツ
	case LANG_GERMANY:
		WFLBY_ROOM_MSG_SetAisatsuGerMany( p_rmwk, 0, timezone );
		switch( WFLBY_SYSTEM_GetProfileRagionCode( cp_youpr ) ){
		case LANG_JAPAN:	
			data_id = msg_hiroba_aisatu_01_09; 
			WFLBY_ROOM_MSG_SetAisatsuJapan( p_rmwk, 1, timezone );
			break;
		case LANG_ENGLISH:	
			data_id = msg_hiroba_aisatu_01_16; 
			WFLBY_ROOM_MSG_SetAisatsuEnglish( p_rmwk, 1, timezone );
			break;
		case LANG_FRANCE:	
			data_id = msg_hiroba_aisatu_01_23; 
			WFLBY_ROOM_MSG_SetAisatsuFrance( p_rmwk, 1, timezone );
			break;
		case LANG_ITALY:	
			data_id = msg_hiroba_aisatu_01_28; 
			WFLBY_ROOM_MSG_SetAisatsuItaly( p_rmwk, 1, timezone );
			break;
		case LANG_GERMANY:	
			data_id = msg_hiroba_aisatu_01_35; 
			WFLBY_ROOM_MSG_SetAisatsuGerMany( p_rmwk, 1, timezone );
			break;
		case LANG_SPAIN:	
			data_id = msg_hiroba_aisatu_01_31; 
			WFLBY_ROOM_MSG_SetAisatsuSpain( p_rmwk, 1, timezone );
			break;
		default:			
			data_id = msg_hiroba_aisatu_01_16; 
			WFLBY_ROOM_MSG_SetAisatsuEnglish( p_rmwk, 1, timezone );
			break;
		}
		break;

	// 自分がスペイン
	case LANG_SPAIN:	
		WFLBY_ROOM_MSG_SetAisatsuSpain( p_rmwk, 0, timezone );
		switch( WFLBY_SYSTEM_GetProfileRagionCode( cp_youpr ) ){
		case LANG_JAPAN:	
			data_id = msg_hiroba_aisatu_01_11; 
			WFLBY_ROOM_MSG_SetAisatsuJapan( p_rmwk, 1, timezone );
			break;
		case LANG_ENGLISH:	
			data_id = msg_hiroba_aisatu_01_18; 
			WFLBY_ROOM_MSG_SetAisatsuEnglish( p_rmwk, 1, timezone );
			break;
		case LANG_FRANCE:	
			data_id = msg_hiroba_aisatu_01_26; 
			WFLBY_ROOM_MSG_SetAisatsuFrance( p_rmwk, 1, timezone );
			break;
		case LANG_ITALY:	
			data_id = msg_hiroba_aisatu_01_30; 
			WFLBY_ROOM_MSG_SetAisatsuItaly( p_rmwk, 1, timezone );
			break;
		case LANG_GERMANY:	
			data_id = msg_hiroba_aisatu_01_31; 
			WFLBY_ROOM_MSG_SetAisatsuGerMany( p_rmwk, 1, timezone );
			break;
		case LANG_SPAIN:	
			data_id = msg_hiroba_aisatu_01_36; 
			WFLBY_ROOM_MSG_SetAisatsuSpain( p_rmwk, 1, timezone );
			break;
		default:			
			data_id = msg_hiroba_aisatu_01_18; 
			WFLBY_ROOM_MSG_SetAisatsuEnglish( p_rmwk, 1, timezone );
			break;
		}
		break;
	
	// 自分が英国
	case LANG_ENGLISH:
	// 自分が不明なときは英語で会話
	default:
		WFLBY_ROOM_MSG_SetAisatsuEnglish( p_rmwk, 0, timezone );
		switch( WFLBY_SYSTEM_GetProfileRagionCode( cp_youpr ) ){
		case LANG_JAPAN:	
			data_id = msg_hiroba_aisatu_01_03; 
			WFLBY_ROOM_MSG_SetAisatsuJapan( p_rmwk, 1, timezone );
			break;
		case LANG_ENGLISH:	
			data_id = msg_hiroba_aisatu_01_32; 
			WFLBY_ROOM_MSG_SetAisatsuEnglish( p_rmwk, 1, timezone );
			break;
		case LANG_FRANCE:	
			data_id = msg_hiroba_aisatu_01_13; 
			WFLBY_ROOM_MSG_SetAisatsuFrance( p_rmwk, 1, timezone );
			break;
		case LANG_ITALY:	
			data_id = msg_hiroba_aisatu_01_14; 
			WFLBY_ROOM_MSG_SetAisatsuItaly( p_rmwk, 1, timezone );
			break;
		case LANG_GERMANY:	
			data_id = msg_hiroba_aisatu_01_15; 
			WFLBY_ROOM_MSG_SetAisatsuGerMany( p_rmwk, 1, timezone );
			break;
		case LANG_SPAIN:	
			data_id = msg_hiroba_aisatu_01_17; 
			WFLBY_ROOM_MSG_SetAisatsuSpain( p_rmwk, 1, timezone );
			break;
		default:			
			data_id = msg_hiroba_aisatu_01_32; 
			WFLBY_ROOM_MSG_SetAisatsuEnglish( p_rmwk, 1, timezone );
			break;
		}
		break;

	}
//*/


	WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, data_id ) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイムゾーンの挨拶をワードセットに設定
 *
 *	@param	p_rmwk			部屋ワーク
 *	@param	buffno			ワードセットバッファナンバー
 *	@param	timezone		タイムゾーン
 *	@param	cp_pr			プロフィール
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_TALK_WK_Aisatu_SetWordSet( WFLBY_ROOMWK* p_rmwk, u32 buffno, u32 timezone, const WFLBY_USER_PROFILE* cp_pr )
{
	switch( WFLBY_SYSTEM_GetProfileRagionCode( cp_pr ) ){
	case LANG_JAPAN:	
		WFLBY_ROOM_MSG_SetAisatsuJapan( p_rmwk, buffno, timezone );
		break;
	case LANG_ENGLISH:	
		WFLBY_ROOM_MSG_SetAisatsuEnglish( p_rmwk, buffno, timezone );
		break;
	case LANG_FRANCE:	
		WFLBY_ROOM_MSG_SetAisatsuFrance( p_rmwk, buffno, timezone );
		break;
	case LANG_ITALY:	
		WFLBY_ROOM_MSG_SetAisatsuItaly( p_rmwk, buffno, timezone );
		break;
	case LANG_GERMANY:	
		WFLBY_ROOM_MSG_SetAisatsuGerMany( p_rmwk, buffno, timezone );
		break;
	case LANG_SPAIN:	
		WFLBY_ROOM_MSG_SetAisatsuSpain( p_rmwk, buffno, timezone );
		break;
	default:			
		WFLBY_ROOM_MSG_SetAisatsuEnglish( p_rmwk, buffno, timezone );
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	挨拶フォローの表示
 *	
 *	@param	p_evwk		ワーク
 *	@param	p_system	システムワーク
 *	@param	p_rmwk		部屋ワーク
 *
 *	@retval	TRUE	フォローを表示した
 *	@retval	FALSE	フォローいらなかった
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EV_TALK_WK_AisatuFollowDraw( WFLBY_EV_TALK_WK* p_evwk, WFLBY_SYSTEM* p_system, WFLBY_ROOMWK* p_rmwk )
{
	u32 data_id;
	const WFLBY_USER_PROFILE* cp_mypr;
	const WFLBY_USER_PROFILE* cp_youpr;

	cp_mypr		= WFLBY_SYSTEM_GetMyProfileLocal( p_system );
	cp_youpr	= WFLBY_SYSTEM_GetUserProfile( p_system, p_evwk->talk_idx );
	
	// 自分か相手の言語が不明でないか
	if( WFLBY_SYSTEM_CheckProfileRagionCode( cp_mypr ) == FALSE ){
		return FALSE;
	}
	if( WFLBY_SYSTEM_CheckProfileRagionCode( cp_youpr ) == FALSE ){
		return FALSE;
	}
	
	// 時間帯が違うかチェック
	if( p_evwk->my_timezone != p_evwk->you_timezone ){

		// 何を表示するか
		switch( p_evwk->you_timezone ){
		case WFLBY_AISATSU_MORNING:
			data_id = msg_hiroba_aisatu_02_03;
			break;
			
		case WFLBY_AISATSU_NOON:
			data_id = msg_hiroba_aisatu_02_01;
			break;
			
		case WFLBY_AISATSU_NIGHT:
			data_id = msg_hiroba_aisatu_02_02;
			break;
		}
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, data_id ) );
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ワードセットにアイテム名を追加
 *
 *	@param	p_evwk		ワーク
 *	@param	p_system	システムワーク
 *	@param	p_rmwk		部屋ワーク
 *	@param	bufid		バッファID
 *	@param	my			自分のデータ？
 *	
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_TALK_WK_WordsetSetItem( WFLBY_EV_TALK_WK* p_evwk, WFLBY_SYSTEM* p_system, WFLBY_ROOMWK* p_rmwk, u32 bufid, BOOL my )
{
	u32 item;
	const WFLBY_USER_PROFILE* cp_mypr;
	const WFLBY_USER_PROFILE* cp_youpr;

	cp_mypr		= WFLBY_SYSTEM_GetMyProfileLocal( p_system );
	cp_youpr	= WFLBY_SYSTEM_GetUserProfile( p_system, p_evwk->talk_idx );

	
	if( my==FALSE ){
		item = WFLBY_SYSTEM_GetProfileItem( cp_youpr );
	}else{
		item = WFLBY_SYSTEM_GetProfileItem( cp_mypr );
	}

	WFLBY_ROOM_MSG_SetItem( p_rmwk, item, bufid );
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイムウエイト　の中間まで時間がきたら、中間メッセージを表示する
 *
 *	@param	p_evwk		イベントワーク
 *	@param	p_rmwk		部屋ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_TALK_WK_PrintTimeWaitMsg( WFLBY_EV_TALK_WK* p_evwk, WFLBY_ROOMWK* p_rmwk )
{
	WFLBY_SYSTEM* p_system;
	STRBUF* p_str;

	p_system = WFLBY_ROOM_GetSystemData( p_rmwk );
	
	if( WFLBY_SYSTEM_GetRecvWaitTime( p_system ) == WFLBY_TALK_RECV_HALF_WAIT ){
		WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
		p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_osyaberi_07_02 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		WFLBY_ROOM_TALKWIN_StartTimeWait( p_rmwk );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	選択待ち時間カウンタ　開始
 *
 *	@param	p_evwk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_TALK_WK_StartSelectWait( WFLBY_EV_TALK_WK* p_evwk )
{
	p_evwk->sel_wait		= 0;
	p_evwk->sel_wait_flag	= TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	選択待ち時間カウンタ　停止
 *
 *	@param	p_evwk 
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_TALK_WK_StopSelectWait( WFLBY_EV_TALK_WK* p_evwk )
{
	p_evwk->sel_wait		= 0;
	p_evwk->sel_wait_flag	= FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	選択待ちカウンタウエイト処理
 *
 *	@param	p_evwk	イベントワーク
 *	@param	p_rmwk	部屋ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_TALK_WK_CheckAndPrintSelectWait( WFLBY_EV_TALK_WK* p_evwk, WFLBY_ROOMWK* p_rmwk )
{
	STRBUF* p_str;

	// 選択待ち状態で、時間がWFLBY_TALK_RECV_HALF_WAITにいったらメッセージを流す
	if( p_evwk->sel_wait_flag == TRUE ){

		if( p_evwk->sel_wait == WFLBY_TALK_RECV_HALF_WAIT ){
			WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, p_evwk->talk_idx, 0 );
			p_str = WFLBY_EV_TALK_GetStrBuf( p_rmwk, p_evwk->talk_idx, msg_hiroba_osyaberi_07_01 );
			WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	選択待ちカウンタウエイト処理	メイン
 *
 *	@param	p_evwk		イベントワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_TALK_WK_MainSelectWait( WFLBY_EV_TALK_WK* p_evwk )
{
	if( p_evwk->sel_wait_flag == TRUE ){
		if( p_evwk->sel_wait <= WFLBY_TALK_RECV_WAIT ){
			p_evwk->sel_wait ++;
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	履歴データ設定
 *
 *	@param	p_evwk		ワーク
 *	@param	cp_youpr	相手のプロフィール
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_TALK_WK_SetRirekiData( WFLBY_EV_TALK_WK* p_evwk, const WFLBY_USER_PROFILE* cp_youpr )
{
	u32 lastact;
	lastact = WFLBY_SYSTEM_GetProfileTrTypetoLastActionNo( cp_youpr );
	p_evwk->rireki_flag		= TRUE;
	p_evwk->rireki_actno	= lastact;
	p_evwk->rireki_userid	= WFLBY_SYSTEM_GetProfileUserID( cp_youpr );
}




//----------------------------------------------------------------------------
/**
 *	@brief	ビットマップリストデータ作成
 *
 *	@param	p_evwk		ワーク
 *	@param	p_rmwk		部屋ワーク
 *	@param	num			リスト数
 *	@param	msg_start	メッセージ開始位置
 *	@param	last_sel_b	選択B最終行を追加するか？
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_TALK_WK_InitBmpList( WFLBY_EV_TALK_BMPLIST* p_wk, WFLBY_ROOMWK* p_rmwk, u32 num, u32 msg_start, u32 last_sel_b )
{
	int i;
	const STRBUF* cp_str;

	// バッファ作成
	WFLBY_ROOM_LISTWIN_CreateBmpList( p_rmwk, num );
	p_wk->bmplist_num = num;

	// データ作成
	for( i=0; i<num; i++ ){
		// 最終行チェック
		if( (last_sel_b != WFLBY_EV_TALK_LIST_END_NONE) &&
			(i == (num-1)) ){
			if( last_sel_b == WFLBY_EV_TALK_LIST_END_YAMERU ){
				cp_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_menu_00_03 );
			}else{
				cp_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_osyaberi_03_41 );
			}
		}else{
			cp_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_start+i );
		}
		WFLBY_ROOM_LISTWIN_SetBmpListStr( p_rmwk, cp_str, i );
	}

	// ビットマップリストヘッダーに設定
	p_wk->bmplist			= sc_WFLBY_EV_TALK_HEADER;
	p_wk->bmplist.count	= p_wk->bmplist_num;
	if( p_wk->bmplist.line > p_wk->bmplist_num ){
		p_wk->bmplist.line = p_wk->bmplist_num;
	}
	p_wk->bmplist.list	= WFLBY_ROOM_LISTWIN_GetBmpList( p_rmwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	p_evwk		ワーク
 *	@param	p_rmwk		部屋ワーク
 *	@param	allnum		メッセージすべての数
 *	@param	sel_num		選択する数(last_sel_bがNONEじゃないなら追加する項目も含めた値)
 *	@param	msg_start	メッセージ開始位置
 *	@param	last_sel_b	選択B最終行を追加するか？
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_TALK_WK_InitBmpListRandom( WFLBY_EV_TALK_BMPLIST* p_wk, WFLBY_ROOMWK* p_rmwk, u32 allnum, u32 sel_num, u32 msg_start, u32 last_sel_b )
{
	int i;
	u32 sel_msg;
	u32 msg_param;
	const STRBUF* cp_str;
	BOOL result;
	

	// バッファ作成
    // MatchComment: WFLBY_ROOM_LISTWIN_CreateBmpList -> WFLBY_ROOM_LISTWIN_CreateBmpList_WithThirdArg. use allnum as third arg
	WFLBY_ROOM_LISTWIN_CreateBmpList_WithThirdArg( p_rmwk, sel_num, allnum );
	p_wk->bmplist_num = sel_num;

	// データ作成
	for( i=0; i<sel_num; i++ ){
		// 最終行チェック
		if( (last_sel_b != WFLBY_EV_TALK_LIST_END_NONE) &&
			(i == (sel_num-1)) ){

			msg_param = allnum;
			if( last_sel_b == WFLBY_EV_TALK_LIST_END_YAMERU ){
				cp_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_menu_00_03 );
			}else{
				cp_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_hiroba_osyaberi_03_41 );
			}
		}else{


#ifdef WFLBY_EV_TALK_DEBUG_SEL_MSG
			// デバック時
			if( s_WFLBY_EV_TALK_DEBUG_SEL_MSG_ON == TRUE ){
				sel_msg		= i+(4*s_WFLBY_EV_TALK_DEBUG_SEL_MSG_PAGE);
				sel_msg		= sel_msg % allnum;
				msg_param	= sel_msg;

				cp_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_start+sel_msg );

			}else{

				// 通常
				// 
				// 重複しない項目がでるまで
				do{
					sel_msg		= gf_mtRand() % allnum;
					msg_param	= sel_msg;
					result		= WFLBY_ROOM_LISTWIN_CheckBmpListParam( p_rmwk, msg_param );
				}while( result == TRUE );
				cp_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_start+sel_msg );

			}
#else
			// 通常
			// 
			// 重複しない項目がでるまで
			do{
				sel_msg		= gf_mtRand() % allnum;
				msg_param	= sel_msg;
				result		= WFLBY_ROOM_LISTWIN_CheckBmpListParam( p_rmwk, msg_param );
			}while( result == TRUE );
			cp_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_AISATSU, msg_start+sel_msg );
#endif

			
		}
		WFLBY_ROOM_LISTWIN_SetBmpListStr( p_rmwk, cp_str, msg_param );
	}

	// ビットマップリストヘッダーに設定
	p_wk->bmplist			= sc_WFLBY_EV_TALK_HEADER;
	p_wk->bmplist.count	= p_wk->bmplist_num;
	if( p_wk->bmplist.line > p_wk->bmplist_num ){
		p_wk->bmplist.line = p_wk->bmplist_num;
	}
	p_wk->bmplist.list	= WFLBY_ROOM_LISTWIN_GetBmpList( p_rmwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	トピック前お姉さん用ビットマップリスト
 *
 *	@param	p_wk		ワーク
 */	
//-----------------------------------------------------------------------------
static void WFLBY_EV_TOPIC_WK_InitBmpList( WFLBY_EV_TALK_BMPLIST* p_wk, WFLBY_ROOMWK* p_rmwk )
{
	int i;
	STRBUF* p_str;
	static const u8 sc_LISTMSG[ WFLBY_EV_TOPIC_LIST_SEL_NUM ] = {
		msg_hiroba_infowin_01,
		msg_hiroba_infowin_02,
		msg_hiroba_infowin_03,
		msg_hiroba_infowin_04,
		msg_hiroba_infowin_05,
		msg_hiroba_infowin_10,
		msg_hiroba_infowin_06,
		msg_hiroba_infowin_07,
	};

	// バッファ作成
	WFLBY_ROOM_LISTWIN_CreateBmpList( p_rmwk, WFLBY_EV_TOPIC_LIST_SEL_NUM );
	p_wk->bmplist_num = WFLBY_EV_TOPIC_LIST_SEL_NUM;

	// データ作成
	for( i=0; i<WFLBY_EV_TOPIC_LIST_SEL_NUM; i++ ){
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, sc_LISTMSG[i] );
		WFLBY_ROOM_LISTWIN_SetBmpListStr( p_rmwk, p_str, i );
	}

	// ビットマップリストヘッダーに設定
	p_wk->bmplist			= sc_WFLBY_EV_TALK_HEADER;
	p_wk->bmplist.count		= p_wk->bmplist_num;
	p_wk->bmplist.line		= p_wk->bmplist_num;
	p_wk->bmplist.list		= WFLBY_ROOM_LISTWIN_GetBmpList( p_rmwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	トピック前お姉さん用イベント選択リスト
 *
 *	@param	p_wk	ワーク
 *	@param	p_rmwk	部屋ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_TOPIC_WK_InitEventBmpList( WFLBY_EV_TALK_BMPLIST* p_wk, WFLBY_ROOMWK* p_rmwk )
{
	int i;
	STRBUF* p_str;

	// バッファ作成
	WFLBY_ROOM_LISTWIN_CreateBmpList( p_rmwk, WFLBY_EV_TOPIC_LIST_SEL_EVE_NUM );
	p_wk->bmplist_num = WFLBY_EV_TOPIC_LIST_SEL_EVE_NUM;

	// データ作成
	for( i=0; i<WFLBY_EV_TOPIC_LIST_SEL_EVE_NUM-1; i++ ){
		WFLBY_ROOM_MSG_SetTimeEvent( p_rmwk, WFLBY_EVENT_GMM_NEON+i, 0 );
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infowin_09 );
		WFLBY_ROOM_LISTWIN_SetBmpListStr( p_rmwk, p_str, i );
	}

	// やめるを追加
	p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infowin_07 );
	WFLBY_ROOM_LISTWIN_SetBmpListStr( p_rmwk, p_str, WFLBY_EV_TOPIC_LIST_SEL_EVE_NUM-1 );
	

	// ビットマップリストヘッダーに設定
	p_wk->bmplist			= sc_WFLBY_EV_TALK_HEADER;
	p_wk->bmplist.count		= p_wk->bmplist_num;
	p_wk->bmplist.line		= p_wk->bmplist_num;
	p_wk->bmplist.list		= WFLBY_ROOM_LISTWIN_GetBmpList( p_rmwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	トピック前お姉さん用イベント選択リスト
 *
 *	@param	p_wk	ワーク
 *	@param	p_rmwk	部屋ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_TOPIC_WK_InitMinigameBmpList( WFLBY_EV_TALK_BMPLIST* p_wk, WFLBY_ROOMWK* p_rmwk )
{
	int i;
	STRBUF* p_str;

	// バッファ作成
	WFLBY_ROOM_LISTWIN_CreateBmpList( p_rmwk, WFLBY_EV_TOPIC_LIST_SEL_MG_NUM );
	p_wk->bmplist_num = WFLBY_EV_TOPIC_LIST_SEL_MG_NUM;

	// データ作成
	for( i=0; i<WFLBY_EV_TOPIC_LIST_SEL_MG_NUM-1; i++ ){
		WFLBY_ROOM_MSG_SetMinigame( p_rmwk, WFLBY_GAME_BALLSLOW+i, 0 );
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infowin_08 );
		WFLBY_ROOM_LISTWIN_SetBmpListStr( p_rmwk, p_str, i );
	}

	// やめるを追加
	p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infowin_07 );
	WFLBY_ROOM_LISTWIN_SetBmpListStr( p_rmwk, p_str, WFLBY_EV_TOPIC_LIST_SEL_MG_NUM-1 );

	// ビットマップリストヘッダーに設定
	p_wk->bmplist			= sc_WFLBY_EV_TALK_HEADER;
	p_wk->bmplist.count		= p_wk->bmplist_num;
	p_wk->bmplist.line		= p_wk->bmplist_num;
	p_wk->bmplist.list		= WFLBY_ROOM_LISTWIN_GetBmpList( p_rmwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ビットマップリストデータを破棄
 *
 *	@param	p_evwk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_TALK_WK_ExitBmpList( WFLBY_EV_TALK_BMPLIST* p_wk, WFLBY_ROOMWK* p_rmwk )
{
	WFLBY_ROOM_LISTWIN_DeleteBmpList( p_rmwk ); // TODO__fix_me
}



//----------------------------------------------------------------------------
/**
 *	@brief	ダミーメッセージ受信後　データ送信するシステム	初期化
 *	
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_TALK_RWSEND_Init( WFLBY_EV_TALK_RWSEND* p_wk )
{
	p_wk->trans_on = FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ダミーメッセージ受信後　データ送信するシステム	開始
 *
 *	@param	p_wk			ワーク
 *	@param	cp_system		システムワーク
 *	@param	send_data		送信データ
 *	@param	wait_seq		受信待ちしてる会話シーケンス
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_TALK_RWSEND_Start( WFLBY_EV_TALK_RWSEND* p_wk, const WFLBY_SYSTEM* cp_system, u16 send_data, u16 wait_seq )
{
	GF_ASSERT( p_wk->trans_on == FALSE );
	p_wk->trans_on		= TRUE;
	p_wk->trans_data	= send_data;
	p_wk->wait_seq		= wait_seq;
	p_wk->start_seq		= WFLBY_SYSTEM_GetTalkSeq( cp_system );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ダミーメッセージ受信後　データ送信するシステム	メイン関数
 *
 *	@param	p_wk		ワーク
 *	@param	p_system	ロビーシステムワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_TALK_RWSEND_Main( WFLBY_EV_TALK_RWSEND* p_wk, WFLBY_SYSTEM* p_system )
{
	u32 talk_seq;

	// 動作中かチェック
	if( p_wk->trans_on == FALSE ){
		return;
	}

	// 会話シーケンス取得
	talk_seq	= WFLBY_SYSTEM_GetTalkSeq( p_system );		// 会話シーケンス取得

	// 会話シーケンス保存
	p_wk->recv_talk_seq = talk_seq;

	// 待っているシーケンスになったらデータ送信
	if( talk_seq == p_wk->wait_seq ){
		// データ転送
		WFLBY_SYSTEM_SendTalkData( p_system, p_wk->trans_data );
		p_wk->trans_on = FALSE;
	}
	// シーケンスが変わったらおかしい、送信せずに終了
	else if( talk_seq != p_wk->start_seq ){
		p_wk->trans_on = FALSE;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ダミーメッセージ受信後　データ送信するシステム	終了チェック
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EV_TALK_RWSEND_CheckEnd( const WFLBY_EV_TALK_RWSEND* cp_wk )
{
	if( cp_wk->trans_on == TRUE ){
		return FALSE;
	}
	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ダミーメッセージ受信後　データ送信するシステム	受信した会話シーケンスを返す
 *
 *	@param	cp_wk	ワーク
 *
 *	@return	会話シーケンス
 */
//-----------------------------------------------------------------------------
static u16 WFLBY_EV_TALK_RWSEND_GetTalkSeq( const WFLBY_EV_TALK_RWSEND* cp_wk )
{
	return cp_wk->recv_talk_seq;
}






