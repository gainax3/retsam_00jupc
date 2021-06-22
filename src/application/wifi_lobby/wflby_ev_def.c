//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_ev_def.c
 *	@brief		常駐イベント		各キャラクタの常駐イベント
 *	@author		tomoya takahashi
 *	@data		2007.11.26
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#include "msgdata/msg_wifi_h_info.h"
#include "msgdata/msg_wifi_hiroba.h"
#include "msgdata/msg_wifi_system.h"

#include "system/fontproc.h"

#include "wflby_ev_def.h"
#include "wflby_ev_talk.h"
#include "wflby_ev_mg.h"
#include "wflby_ev_anketo.h"
#include "wflby_3dmatrix.h"
#include "wflby_snd.h"

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
#define WFLBY_DEBUG_EV_DEF_GADGET_GET	// 選択したガジェットをもらえるようにする
//#define WFLBY_DEBUG_EV_DEF_FLOAT_RETRY	// フロート退室してももう一度戻ってくる
//#define WFLBY_DEBUG_EV_DEF_ANKETO	// アンケート判断条件をボタンに割り振る
#endif

#ifdef WFLBY_DEBUG_EV_DEF_GADGET_GET
extern BOOL D_Tomoya_WiFiLobby_DebugStart;	// デバック開始情報
#endif



//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	イベントによって起動するイベントデータ用
//=====================================
// イベントタイプ
enum {
	WFLBY_EV_DEF_EVTYPE_DEF,	// 常駐イベント
	WFLBY_EV_DEF_EVTYPE_INTR,	// 割り込みイベント
	WFLBY_EV_DEF_EVTYPE_NUM,	// イベントタイプ数
	WFLBY_EV_DEF_EVTYPE_END,	// このデータが終端データ
} ;
// 起動イベントNo最大数
#define WFLBY_EV_DEF_EVNO_MAX	( 8 )
// 起動イベントNo不正値
#define WFLBY_EV_DEF_EVNO_END	( 0xff )



//-------------------------------------
///	全共通　ガジェットイベント
//=====================================
enum {
	WFLBY_EV_DEF_ALL_GADGET_SEQ_INIT,
	WFLBY_EV_DEF_ALL_GADGET_SEQ_WAIT,
	WFLBY_EV_DEF_ALL_GADGET_SEQ_END,
} ;




//-------------------------------------
///	プレイヤー通常入場定数
//=====================================
// 通常
enum{
	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_ADD,
	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_FADEWAIT,
	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MOVESTARTWAIT,
	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MOVE00,	// 前へ
	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MOVE01,	// 前へ
	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MOVE02,	// ターン
	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MOVE03,	// 横へ
	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MOVEWAIT,	// 動作完了待ち
	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG00,		// ”ようこそ”
	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG01,		// ”１つめのタイプをえらんでね”
//	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG02,		// ”選択表示（１つめ）”
	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG03,		// ”選択（１つめ）”
//	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG04,		// ”これでよいですか？（１つめ）o”	いらない
//	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG05,		// ”YESNO表示（１つめ）”
	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG06,		// ”YESNO（１つめ）”
	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG07,		// ”２つめのタイプをえらんでね”
//	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG08,		// ”選択表示（２つめ）”
	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG09,		// ”選択（２つめ）”
	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG10,		// ”これでよいですか？（２つめ）”
//	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG11,		// ”YESNO表示（２つめ）”
	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG12,		// ”YESNO（２つめ）”
//	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG13,		// ”プロフィールをもらった”
//	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG14,		// ”タッチトイをもらった！”
	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG15,		// ”タッチトイをもらった！”
	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG16,		// ”さいごまで　たのしんでね！”
	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG17,		// ”よろしければ　タイプ選択”
	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG18,		// ”１こ選択したときの　お姉さんのお言葉”
	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG19,		// ”２こ選択したときの　お姉さんのお言葉”
	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSGWAIT,	// メッセージ終了待ち
	WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_END,
};
#define WFLBY_EV_DEF_PLAYERIN_NORMAL_MOVEWAIT			( 16 )
static const BMPLIST_HEADER sc_WFLBY_EV_DEF_WAZATYPE_HEADER = {
	NULL,
	NULL,
	NULL,
	NULL,

	WFLBY_POKEWAZA_TYPE_NUM,		//リスト項目数
	7,								//表示最大項目数
	0,								//ラベル表示Ｘ座標
	8,								//項目表示Ｘ座標
	0,								//カーソル表示Ｘ座標
	0,								//表示Ｙ座標
	FBMP_COL_BLACK,					//表示文字色
	FBMP_COL_WHITE,					//表示背景色
	FBMP_COL_BLK_SDW,				//表示文字影色
	0,								//文字間隔Ｘ
	16,								//文字間隔Ｙ
	BMPLIST_LRKEY_SKIP,				//ページスキップタイプ
	FONT_SYSTEM,					//文字指定(本来は u8 だけど、そんなに作らないと思うので)
	0,								//ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)

	NULL
};

// 指定グリッド位置に出現
enum{
	WFLBY_EV_DEF_PLAYERIN_GRID_SEQ_ADD,
	WFLBY_EV_DEF_PLAYERIN_GRID_SEQ_FADEWAIT,
	WFLBY_EV_DEF_PLAYERIN_GRID_SEQ_PLMOVE,
	WFLBY_EV_DEF_PLAYERIN_GRID_SEQ_APLFORCEEND_MSG,
	WFLBY_EV_DEF_PLAYERIN_GRID_SEQ_APLFORCEEND_MSGWAIT,
	WFLBY_EV_DEF_PLAYERIN_GRID_SEQ_END,
};
// ミニゲーム後に出現
enum{
	WFLBY_EV_DEF_PLAYERIN_MG_SEQ_ADD,
	WFLBY_EV_DEF_PLAYERIN_MG_SEQ_FADEWAIT,
	WFLBY_EV_DEF_PLAYERIN_MG_SEQ_PLMOVE,
	WFLBY_EV_DEF_PLAYERIN_MG_SEQ_MSG,
	WFLBY_EV_DEF_PLAYERIN_MG_SEQ_MSGEND,
	WFLBY_EV_DEF_PLAYERIN_MG_SEQ_END,
};
// 指定グリッド位置に出現アンケート後
enum{
	WFLBY_EV_DEF_PLAYERIN_ANKETO_SEQ_ADD,
	WFLBY_EV_DEF_PLAYERIN_ANKETO_SEQ_FADEWAIT,
	WFLBY_EV_DEF_PLAYERIN_ANKETO_SEQ_PLMOVE,
	WFLBY_EV_DEF_PLAYERIN_ANKETO_SEQ_APLFORCEEND_MSG,
	WFLBY_EV_DEF_PLAYERIN_ANKETO_SEQ_APLFORCEEND_MSGWAIT,
	WFLBY_EV_DEF_PLAYERIN_ANKETO_SEQ_END,
};

//-------------------------------------
///	プレイヤー常駐動作定数
//=====================================
enum{
	WFLBY_EV_DEF_PLAYER_SEQ_INIT,
	WFLBY_EV_DEF_PLAYER_SEQ_EVCHECK,
};

//-------------------------------------
///	プレイヤー通常退室シーケンス
//=====================================
enum {
	WFLBY_EV_DEF_PLAYER_OUTNORMAL_SEQ_MSGSTART,			// 退室チェックメッセージ開始
	WFLBY_EV_DEF_PLAYER_OUTNORMAL_SEQ_MSGWAIT,			// 退室チェックメッセージ待ち
	WFLBY_EV_DEF_PLAYER_OUTNORMAL_SEQ_MSGSTART01,		// 退室チェックメッセージ開始
	WFLBY_EV_DEF_PLAYER_OUTNORMAL_SEQ_MSGWAIT01,		// 退室チェックメッセージ待ち
	WFLBY_EV_DEF_PLAYER_OUTNORMAL_SEQ_YESNO,			// YESNOウィンドウ表示
	WFLBY_EV_DEF_PLAYER_OUTNORMAL_SEQ_YESNOWAIT,		// YESNO選択待ち
	WFLBY_EV_DEF_PLAYER_OUTNORMAL_SEQ_FLYUP,			// 退室上る
	WFLBY_EV_DEF_PLAYER_OUTNORMAL_SEQ_END,				// 退室
	WFLBY_EV_DEF_PLAYER_OUTNORMAL_SEQ_RET,				// 戻る
	WFLBY_EV_DEF_PLAYER_OUTNORMAL_SEQ_RETWAIT,			// 戻り完了まち
};

//-------------------------------------
///	世界時計へのシーケンス
//=====================================
enum {
	WFLBY_EV_DEF_PLAYER_SUBCHAN_SEQ_LOGIN,
	WFLBY_EV_DEF_PLAYER_SUBCHAN_SEQ_LOGINWAIT,
	WFLBY_EV_DEF_PLAYER_SUBCHAN_SEQ_START,
	WFLBY_EV_DEF_PLAYER_SUBCHAN_SEQ_NG,
	WFLBY_EV_DEF_PLAYER_SUBCHAN_SEQ_NGWAIT,
	WFLBY_EV_DEF_PLAYER_SUBCHAN_SEQ_CLOSE,
	WFLBY_EV_DEF_PLAYER_SUBCHAN_SEQ_CLOSEWAIT,
	WFLBY_EV_DEF_PLAYER_SUBCHAN_SEQ_NGMSG,
	WFLBY_EV_DEF_PLAYER_SUBCHAN_SEQ_NGMSGWAIT,
} ;
#define WFLBY_EV_DEF_PLAYER_SUBCHAN_MSG_WAIT	( 8 )

//-------------------------------------
///	ミニゲームへのシーケンス
//=====================================
enum {
	WFLBY_EV_DEF_PLAYER_MG_SEQ_INTR_START,
	WFLBY_EV_DEF_PLAYER_MG_SEQ_INTR_WAIT,
	WFLBY_EV_DEF_PLAYER_MG_SEQ_INTR_END,
	WFLBY_EV_DEF_PLAYER_MG_SEQ_INTR_RET,
	WFLBY_EV_DEF_PLAYER_MG_SEQ_INTR_RETWAIT,
	WFLBY_EV_DEF_PLAYER_MG_SEQ_MSG,
	WFLBY_EV_DEF_PLAYER_MG_SEQ_MSGWAIT,
	WFLBY_EV_DEF_PLAYER_MG_SEQ_RET_NORMAL,
} ;

//-------------------------------------
///	アンケートへのシーケンス
//=====================================
enum {
	WFLBY_EV_DEF_PLAYER_ANKETO_SEQ_START,
	WFLBY_EV_DEF_PLAYER_ANKETO_SEQ_START_ANKETO,
	WFLBY_EV_DEF_PLAYER_ANKETO_SEQ_RET_MOVE,
	WFLBY_EV_DEF_PLAYER_ANKETO_SEQ_RET_WAIT,
	WFLBY_EV_DEF_PLAYER_ANKETO_SEQ_RET_MSG,
	WFLBY_EV_DEF_PLAYER_ANKETO_SEQ_RET_MSGWAIT,
	WFLBY_EV_DEF_PLAYER_ANKETO_SEQ_RET_NORMAL,
} ;

//-------------------------------------
///	プレイヤーフロート
//=====================================
enum{
	WFLBY_EV_FLOAT_SEQ_INIT,
	WFLBY_EV_FLOAT_SEQ_FLOAT_CHECK,		// 予約部分
	WFLBY_EV_FLOAT_SEQ_FLOAT_INWAIT,	// 乗れるまで待つ
	WFLBY_EV_FLOAT_SEQ_FLOAT_JUMP,		// ジャンプ
	WFLBY_EV_FLOAT_SEQ_FLOAT_JUMPON,	// 着地アニメ
	WFLBY_EV_FLOAT_SEQ_FLOAT_MAIN,		// メイン	ボタン押したり
	WFLBY_EV_FLOAT_SEQ_FLOAT_END_DOWN,	// 乗れないので１歩下がる
	WFLBY_EV_FLOAT_SEQ_FLOAT_END_DOWNWAIT,// 乗れないので１歩下がる
	WFLBY_EV_FLOAT_SEQ_FLOAT_END_BEFORE,// フロート前なので乗れないメッセージ
	WFLBY_EV_FLOAT_SEQ_FLOAT_END_FULL,	// フロートがいっぱいなので乗れない
	WFLBY_EV_FLOAT_SEQ_FLOAT_END_AFTER,	// パレード後なので乗れないメッセージ
	WFLBY_EV_FLOAT_SEQ_FLOAT_MSGWAIT,	// メッセージ待ち
	WFLBY_EV_FLOAT_SEQ_FLOAT_WINOFFWAIT,// ウィンドウＯＦＦ待ち
	WFLBY_EV_FLOAT_SEQ_END,				//  終了
	WFLBY_EV_FLOAT_SEQ_ENDWAIT,			//  終了待ち
	WFLBY_EV_FLOAT_SEQ_RET,				//  終了
};
//ジャンプ
#define WFLBY_EV_FLOAT_JUMP_ANM_SYNC	( 24 )	// ジャンプシンク数
#define WFLBY_EV_FLOAT_JUMP_ANM_SYNC_ZS	( 4 )	// Zの値を設定する開始シンク数
#define WFLBY_EV_FLOAT_JUMP_ANM_SYNC_Z	( 20 )	// ジャンプシーケンスZ用シンク数
#define WFLBY_EV_FLOAT_JUMP_CHECK_X		( FX32_CONST( 16 ) )	// 飛び乗り開始フロートXオフセット
#define WFLBY_EV_FLOAT_JUMP_MOVE_Y		( FX32_CONST( 16 ) )	// 飛ぶ高さ
// 着地
#define WFLBY_EV_FLOAT_JUMPON_ANM_SYNC	( 16 )	// 着地アニメ
#define WFLBY_EV_FLOAT_JUMPON_ANM_ONE	( 8 )	// 上下する1回に使用するシンク数
#define WFLBY_EV_FLOAT_JUMPON_ANM_MOVEY	( FX32_CONST(3) )	// 動く高さ

#define WFLBY_EV_FLOAT_CAMERA_MOVE_END	( FX32_CONST( 96 ) )	// カメラスクロールの終了地点


//-------------------------------------
///	NPC入場シーケンス
//=====================================
enum{
	WFLBY_EV_DEF_NPCWAIT_SEQ_INIT,
	WFLBY_EV_DEF_NPCWAIT_SEQ_WAIT,
	WFLBY_EV_DEF_NPCWAIT_SEQ_INWAIT,
	WFLBY_EV_DEF_NPCWAIT_SEQ_STATUS_CHECK,
	WFLBY_EV_DEF_NPCWAIT_SEQ_FLYDOWN,
	WFLBY_EV_DEF_NPCWAIT_SEQ_END,
};

//-------------------------------------
///	NPC退場シーケンス
//=====================================
enum{
	WFLBY_EV_DEF_NPCEND_SEQ_OUT,
	WFLBY_EV_DEF_NPCEND_SEQ_OUTWAIT,
	WFLBY_EV_DEF_NPCEND_SEQ_END,
};

//-------------------------------------
///	NPC通常シーケンス
//=====================================
enum{
	WFLBY_EV_DEF_NPCMAIN_SEQ_INIT,
	WFLBY_EV_DEF_NPCMAIN_SEQ_MAIN,		// メイン
	WFLBY_EV_DEF_NPCMAIN_SEQ_SYNCWAIT,
};

//-------------------------------------
///	オブジェIDリスト
//=====================================
// オブジェIDの数
#define WFLBY_EV_DEF_NPC_OBJID_MINIGAME_NUM	( 4 )	// ミニゲーム
#define WFLBY_EV_DEF_NPC_OBJID_TOPIC_NUM	( 4 )	// トピック
#define WFLBY_EV_DEF_NPC_OBJID_WLDTIMER_NUM	( 4 )	// 世界時計
#define WFLBY_EV_DEF_NPC_OBJID_FOOT_NUM		( 8 )	// 足跡ボード
	
// ボール投げ
static const u32 sc_WFLBY_EV_DEF_NPC_OBJID_BS[ WFLBY_EV_DEF_NPC_OBJID_MINIGAME_NUM ]={
	WFLBY_MAPOBJID_NPCMG00MAT00,
	WFLBY_MAPOBJID_NPCMG00MAT01,
	WFLBY_MAPOBJID_NPCMG00MAT02,
	WFLBY_MAPOBJID_NPCMG00MAT03,
};
// ボールのり
static const u32 sc_WFLBY_EV_DEF_NPC_OBJID_BB[ WFLBY_EV_DEF_NPC_OBJID_MINIGAME_NUM ]={
	WFLBY_MAPOBJID_NPCMG01MAT00,
	WFLBY_MAPOBJID_NPCMG01MAT01,
	WFLBY_MAPOBJID_NPCMG01MAT02,
	WFLBY_MAPOBJID_NPCMG01MAT03,
};
// 風船割り
static const u32 sc_WFLBY_EV_DEF_NPC_OBJID_BL[ WFLBY_EV_DEF_NPC_OBJID_MINIGAME_NUM ]={
	WFLBY_MAPOBJID_NPCMG02MAT00,
	WFLBY_MAPOBJID_NPCMG02MAT01,
	WFLBY_MAPOBJID_NPCMG02MAT02,
	WFLBY_MAPOBJID_NPCMG02MAT03,
};
// トピック
static const u32 sc_WFLBY_EV_DEF_NPC_OBJID_TP[ WFLBY_EV_DEF_NPC_OBJID_TOPIC_NUM ] = {
	WFLBY_MAPOBJID_TOPIC_00,
	WFLBY_MAPOBJID_TOPIC_01,
	WFLBY_MAPOBJID_TOPIC_02,
	WFLBY_MAPOBJID_TOPIC_03,
};
// 世界時計
static const u32 sc_WFLBY_EV_DEF_NPC_OBJID_WT[ WFLBY_EV_DEF_NPC_OBJID_WLDTIMER_NUM ] = {
	WFLBY_MAPOBJID_WLDTIMER_00,
	WFLBY_MAPOBJID_WLDTIMER_01,
	WFLBY_MAPOBJID_WLDTIMER_02,
	WFLBY_MAPOBJID_WLDTIMER_03,
};
// 足跡ボード白
static const u32 sc_WFLBY_EV_DEF_NPC_OBJID_FT00[ WFLBY_EV_DEF_NPC_OBJID_FOOT_NUM ] = {
	WFLBY_MAPOBJID_FOOT_00_00,
	WFLBY_MAPOBJID_FOOT_00_01,
	WFLBY_MAPOBJID_FOOT_00_02,
	WFLBY_MAPOBJID_FOOT_00_03,
	WFLBY_MAPOBJID_FOOT_00_04,
	WFLBY_MAPOBJID_FOOT_00_05,
	WFLBY_MAPOBJID_FOOT_00_06,
	WFLBY_MAPOBJID_FOOT_00_07,
};
// 足跡ボード黒
static const u32 sc_WFLBY_EV_DEF_NPC_OBJID_FT01[ WFLBY_EV_DEF_NPC_OBJID_FOOT_NUM ] = {
	WFLBY_MAPOBJID_FOOT_01_00,
	WFLBY_MAPOBJID_FOOT_01_01,
	WFLBY_MAPOBJID_FOOT_01_02,
	WFLBY_MAPOBJID_FOOT_01_03,
	WFLBY_MAPOBJID_FOOT_01_04,
	WFLBY_MAPOBJID_FOOT_01_05,
	WFLBY_MAPOBJID_FOOT_01_06,
	WFLBY_MAPOBJID_FOOT_01_07,
};


//-------------------------------------
///	NPCジャンプシーケンス
//=====================================
enum {
	WFLBY_EV_DEF_NPCJUMP_SEQ_INIT,
	WFLBY_EV_DEF_NPCJUMP_SEQ_UP,
	WFLBY_EV_DEF_NPCJUMP_SEQ_UPWAIT,
	WFLBY_EV_DEF_NPCJUMP_SEQ_DOWN,
	WFLBY_EV_DEF_NPCJUMP_SEQ_DOWNWAIT,
	WFLBY_EV_DEF_NPCJUMP_SEQ_WAIT,
	WFLBY_EV_DEF_NPCJUMP_SEQ_RET_UP,
	WFLBY_EV_DEF_NPCJUMP_SEQ_RET_UPWAIT,
	WFLBY_EV_DEF_NPCJUMP_SEQ_RET_DOWN,
	WFLBY_EV_DEF_NPCJUMP_SEQ_RET_DOWNWAIT,
	WFLBY_EV_DEF_NPCJUMP_SEQ_END,
} ;
typedef enum{	// ジャンプ動作	タイプ
	WFLBY_EV_DEF_NPCJUMP_MOVETYPE_NORMAL,		// 通常
	WFLBY_EV_DEF_NPCJUMP_MOVETYPE_STARTDOWN,	// 落ちてくるところからはじめる
} WFLBY_EV_DEF_NPCJUMP_MOVETYPE;


//-------------------------------------
///	NPCフロートに乗って退室
//=====================================
enum {
	WFLBY_EV_DEF_NPCFLOAT_SEQ_INIT,
	WFLBY_EV_DEF_NPCFLOAT_SEQ_UPWAIT,
	WFLBY_EV_DEF_NPCFLOAT_SEQ_DOWNWAIT,
	WFLBY_EV_DEF_NPCFLOAT_SEQ_MAIN,
	WFLBY_EV_DEF_NPCFLOAT_SEQ_END,
} ;
#define WFLBY_EV_DEF_NPCFLOAT_DIF_X			( FX32_CONST( -16 ) )	// 着地点
#define WFLBY_EV_DEF_NPCFLOAT_DOWN_COUNT	( 24 )					// カウンタ


//-------------------------------------
///	汎用ドンドンアニメ
//=====================================
#define WFLBY_EV_DEF_DONDON_SYNC_ONE		( 4 )
#define WFLBY_EV_DEF_DONDON_MOVEY			( FX32_CONST( 6 ) )
#define WFLBY_EV_DEF_DONDON_NEXT_WAIT_MAX	( 4 )
enum{
	WFLBY_EV_DEF_DONDON_SEQ_END,
	WFLBY_EV_DEF_DONDON_SEQ_WAIT,
	WFLBY_EV_DEF_DONDON_SEQ_MOVE,
};


//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------


//-------------------------------------
///	汎用ドンドンアニメ
//=====================================
typedef struct {
	fx32 y;
	s8	count;			// カウンタ
	u8	seq;			// シーケンス
	u8	next_wait;		// 毎回乱数で生成される次のドンまでのウエイト
	u8	next_ok;		// 次もジャンプさせるか？
} WFLBY_EV_DEF_DONDON;


//-------------------------------------
///	主人公初回入室イベントワーク
//=====================================
typedef struct {
	u16 ret_seq;
	u16 gadget;
	s32	wait;

	// ビットマップリスト専用
	BMPLIST_HEADER		bmplist;

	// 選択した技タイプ
	u16					type1;	
	u16					type2;	
} WFLBY_EV_DEF_PLAYER_IN_WK;


//-------------------------------------
///	主人公常駐イベントワーク
//=====================================
typedef struct {
	u32 infodraw_retmvtype;
} WFLBY_EV_DEF_PLAYER_WK;

//-------------------------------------
///	サブチャンネル接続イベントワーク
//=====================================
typedef struct {
	s32 wait;
	u16	msg_idx;
	u16 msg_type;
	u32 minigame;
} WFLBY_EV_DEF_PLAYER_SUBCHAN_WK;

//-------------------------------------
///	ミニゲーム接続イベントワーク
//=====================================
typedef struct {
	WFLBY_EV_MG_PARAM mg_param;
} WFLBY_EV_DEF_PLAYER_MG_WK;

//-------------------------------------
///	アンケート接続イベント
//=====================================
typedef struct {
	u16 ret_msg;	
	u16 apl_no;
} WFLBY_EV_DEF_PLAYER_ANKETO_WK;


//-------------------------------------
///	プレイヤーフロート
//=====================================
typedef struct {
	u32				floatidx;		// 予約したフロート座席インデックス
	u16				next_seq;		// 次に進むシーケンス
	u16				station;		// 待っている駅定数
	s16				count;			// エフェクトカウンタ
	u8				msg_off_ret_seq;// メッセージ非表示戻りシーケンス
	u8				wait;
	WFLBY_3DPERSON* p_obj;
	WFLBY_EV_DEF_DONDON dondon;		// ドンドンアニメ
} WFLBY_EV_FLOAT_INSIDE;



//-------------------------------------
///	NPC非常駐イベント　
//　ある場所に人をジャンプして配置する
//=====================================
typedef struct {
	u8	gridx;
	u8	gridy;
	u8	status;
	u8	talk_flag;
	u8	movetype;
	u8	pad[3];
} WFLBY_EV_DEF_NPCJUMP_PARAM;

//-------------------------------------
///	NPC常駐イベントワーク
//=====================================
typedef struct {
	WFLBY_EV_DEF_NPCJUMP_PARAM intrparam;		// 割り込みイベント用ワーク
} WFLBY_EV_DEF_NPC_WK;

//-------------------------------------
///	NPCフロートイベントワーク
//=====================================
typedef struct {
	WFLBY_3DPERSON* p_npc;
	u32				float_idx;
	u32				float_offs;

	// 落下アニメ
	fx32			start_y;	// 開始ｙ座標
	fx32			on_x;		// 着地ｘ座標
	s32				count;

	WFLBY_EV_DEF_DONDON dondon;		// ドンドンアニメ
} WFLBY_EV_DEF_NPCFLOAT_WK;




//-------------------------------------
///	イベントによって起動するイベント
//  パラメータ生成コールバック関数
//=====================================
typedef void (*pWFLBY_EV_DEF_PFUNC)( WFLBY_EV_DEF_PLAYER_WK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno, u32 eventno );
//-------------------------------------
///	イベントによって起動するイベント
//=====================================
typedef struct {
	u8	event[ WFLBY_EV_DEF_EVNO_MAX ];		// 発動イベントナンバー　WFLBY_MAPEVID_EV_～
	u32 evtype;								// イベントタイプ	WFLBY_EV_DEF_EVTYPE_～
	WFLBY_EVENT_DATA	evdata;				// イベント関数テーブル
	pWFLBY_EV_DEF_PFUNC p_pfunc;			// パラメータ生成コールバックただし、イベントタイプがWFLBY_EV_DEF_EVTYPE_INTRのときのみ
} WFLBY_EV_DEF_EVDATA;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
// 全体で使用
//=====================================
static BOOL WFLBY_EV_DEF_WayKeyInput( WF2DMAP_WAY way );
static WFLBY_3DPERSON* WFLBY_EV_DEF_StartObjPlayer( WFLBY_ROOMWK* p_rmwk, u32 plno );
static void WFLBY_EV_DEF_StartObjPlayerEx( WFLBY_ROOMWK* p_rmwk, u32 plno, u32 gridx, u32 gridy, WF2DMAP_WAY way );


//-------------------------------------
///	共用ドンドンアニメ
//=====================================
static void WFLBY_EV_DEF_DONDON_Start( WFLBY_EV_DEF_DONDON* p_wk );
static fx32 WFLBY_EV_DEF_DONDON_Main( WFLBY_EV_DEF_DONDON* p_wk );
static void WFLBY_EV_DEF_DONDON_EndReq( WFLBY_EV_DEF_DONDON* p_wk );
static BOOL WFLBY_EV_DEF_DONDON_CheckMove( const WFLBY_EV_DEF_DONDON* cp_wk );



//-------------------------------------
///	共通割り込みイベント
//=====================================
// ガジェットならしイベント
static BOOL WFLBY_EV_DEF_ALL_IntrGadGet_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno );

//-------------------------------------
// プレイヤー常駐イベント
//=====================================
// 入場メインイベント
static BOOL WFLBY_EV_DEF_PlayerInNormal_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno );
static BOOL WFLBY_EV_DEF_PlayerInGrid_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno );
static BOOL WFLBY_EV_DEF_PlayerInMinigame_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno );
static BOOL WFLBY_EV_DEF_PlayerInAnketo_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno );

// 常駐メインイベント
static void WFLBY_EV_DEF_Player_StartEventTbl( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno, const WFLBY_EV_DEF_EVDATA* cp_data, u32 eventno );
static BOOL WFLBY_EV_DEF_PlayerMain_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno );
static BOOL WFLBY_EV_DEF_PlayerMain_After( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno );
// 退場イベント
static BOOL WFLBY_EV_DEF_PlayerOutNormal_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno );
static BOOL WFLBY_EV_DEF_PlayerOutSubChan_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno );
static BOOL WFLBY_EV_DEF_PlayerOutMiniGame_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno );
static BOOL WFLBY_EV_DEF_PlayerOutAnketo( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno );
static u32 WFLBY_EV_DEF_PlayerOutSubChan_GetEvnoChannel( u32 evno );
static u32 WFLBY_EV_DEF_PlayerOutSubChan_GetEvnoRetNum( u32 evno );
static u32 WFLBY_EV_DEF_PlayerOutSubChan_GetEvnoStatus( u32 evno );
// 選択イベント
static BOOL WFLBY_EV_DEF_PlayerA_SWIN_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno );
// フロート
static BOOL WFLBY_EV_FLOAT_Inside( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno );
static void WFLBY_EV_FLOAT_SetCameraPos( WFLBY_EV_FLOAT_INSIDE* p_wk, WFLBY_CAMERA* p_camera );
static BOOL WFLBY_EV_FLOAT_CheckJumpOn( const WFLBY_EV_FLOAT_INSIDE* cp_wk, const WFLBY_3DMAPOBJ_CONT* cp_mapobjcont );
static BOOL WFLBY_EV_FLOAT_JumpMove( WFLBY_EV_FLOAT_INSIDE* p_wk, WFLBY_3DMAPOBJ_CONT* p_mapobjcont, WFLBY_CAMERA* p_camera );
static BOOL WFLBY_EV_FLOAT_JumpOnMove( WFLBY_EV_FLOAT_INSIDE* p_wk, WFLBY_3DMAPOBJ_CONT* p_mapobjcont );
static void WFLBY_EV_FLOAT_MatrixSet( WFLBY_EV_FLOAT_INSIDE* p_wk, WFLBY_3DMAPOBJ_CONT* p_mapobjcont, WFLBY_ROOMWK* p_rmwk );
static void WFLBY_EV_FLOAT_MatrixDonDonSet( WFLBY_EV_FLOAT_INSIDE* p_wk, WFLBY_3DMAPOBJ_CONT* p_mapobjcont, WFLBY_ROOMWK* p_rmwk );
// 技タイプビットマップリスト
static void WFLBY_EV_DEF_PlayerInNormal_WazaTypeBmpListInit( WFLBY_EV_DEF_PLAYER_IN_WK* p_wk, WFLBY_ROOMWK* p_rmwk );
static void WFLBY_EV_DEF_PlayerInNormal_WazaTypeBmpListDelete( WFLBY_EV_DEF_PLAYER_IN_WK* p_wk, WFLBY_ROOMWK* p_rmwk );




//-------------------------------------
// NPC常駐イベント
//=====================================
// NPC出現待ち
static BOOL WFLBY_EV_DEF_NpcWait_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno );
// NPCメイン
static BOOL WFLBY_EV_DEF_NpcMain_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno );
// NPC退室
static BOOL WFLBY_EV_DEF_NpcEnd_Brfore( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno );
// NPCフロート退室
static BOOL WFLBY_EV_DEF_NpcFloat_Brfore( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno );


//-------------------------------------
///	NPC非常中イベント
//=====================================
// NPC指定位置ジャンプ	通常状態に遷移後元に戻る
static BOOL WFLBY_EV_DEF_NpcJump_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno );



//-------------------------------------
///	パラメータ作成関数
//=====================================
// 指定位置ジャンプ
static BOOL WFLBY_EV_DEF_NpcJumpParamMake( WFLBY_EV_DEF_NPCJUMP_PARAM* p_param, WFLBY_SYSTEM* p_system, WFLBY_ROOMWK* p_rmwk, WFLBY_3DPERSON* p_npc, u8 status, WFLBY_EV_DEF_NPCJUMP_MOVETYPE movetype );
static BOOL WFLBY_EV_DEF_NpcJumpParam_GetOBJIDPos( const WFLBY_MAPCONT* cp_mapcont, const WFLBY_3DOBJCONT* cp_objcont, const u32* cp_objid, u32 num, WF2DMAP_POS* p_pos );



//-----------------------------------------------------------------------------
/**
 *				イベントデータ
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	全共通ガジェットイベント
//=====================================
static const WFLBY_EVENT_DATA	sc_WFLBY_EV_DEF_ALL_GADGET = {
	WFLBY_EV_DEF_ALL_IntrGadGet_Before,
	NULL
};

//-------------------------------------
// プレイヤー入場イベント
//=====================================
static const WFLBY_EVENT_DATA	sc_WFLBY_EV_DEF_PLAYERIN[WFLBY_EV_DEF_PLAYERIN_NUM] = {
	{
		WFLBY_EV_DEF_PlayerInNormal_Before,
		NULL,
	},
	{
		WFLBY_EV_DEF_PlayerInGrid_Before,
		NULL,
	},
	{
		WFLBY_EV_DEF_PlayerInMinigame_Before,
		NULL,
	},
	{
		WFLBY_EV_DEF_PlayerInAnketo_Before,
		NULL,
	},
};

//-------------------------------------
// プレイヤーメイン常駐イベント
//=====================================
static const WFLBY_EVENT_DATA	sc_WFLBY_EV_DEF_PLAYERMAIN = {
	WFLBY_EV_DEF_PlayerMain_Before,
	WFLBY_EV_DEF_PlayerMain_After,
};

//-------------------------------------
// プレイヤーミニゲーム参加イベント
//=====================================
// マッチング
static const WFLBY_EVENT_DATA	sc_WFLBY_EV_DEF_PLAYER_MGSTART = {
	WFLBY_EV_MG_Start,
	NULL,
};
// 切断
static const WFLBY_EVENT_DATA	sc_WFLBY_EV_DEF_PLAYER_MGEND = {
	WFLBY_EV_MG_End,
	NULL,
};

//-------------------------------------
// NPC出現待ち常駐イベント
//=====================================
static const WFLBY_EVENT_DATA	sc_WFLBY_EV_DEF_NPCWAIT = {
	WFLBY_EV_DEF_NpcWait_Before,
	NULL,
};

//-------------------------------------
// NPCメイン常駐イベント
//=====================================
static const WFLBY_EVENT_DATA	sc_WFLBY_EV_DEF_NPCMAIN = {
	WFLBY_EV_DEF_NpcMain_Before,
	NULL,
};

//-------------------------------------
// NPC退室常駐イベント
//=====================================
static const WFLBY_EVENT_DATA	sc_WFLBY_EV_DEF_NPCEND = {
	WFLBY_EV_DEF_NpcEnd_Brfore,
	NULL,
};
static const WFLBY_EVENT_DATA	sc_WFLBY_EV_DEF_NPCFLOAT = {
	WFLBY_EV_DEF_NpcFloat_Brfore,
	NULL,
};

//-------------------------------------
// NPCジャンプ非常駐イベント
//=====================================
static const WFLBY_EVENT_DATA	sc_WFLBY_EV_DEF_NPCJUMP = {
	WFLBY_EV_DEF_NpcJump_Before,
	NULL,
};


//-------------------------------------
// イベントNOで起動するイベント
//=====================================
// 上に乗ると起動するもの
static const WFLBY_EV_DEF_EVDATA sc_WFLBY_EV_DEF_EVENT_MOUNTSTART[] = {
	// 出口から退場
	{	// 起動イベントNO
		{	
			WFLBY_MAPEVID_EV_PLAYER_OUT,
			WFLBY_EV_DEF_EVNO_END,
		},
		// イベントタイプ
		WFLBY_EV_DEF_EVTYPE_DEF,
		// イベント関数ポインタ
		{
			WFLBY_EV_DEF_PlayerOutNormal_Before,
			NULL,
		},
		NULL,
	},
	// 世界時計に移行
	{	// 起動イベントNO
		{	
			WFLBY_MAPEVID_EV_WLDTIMER_00,
			WFLBY_MAPEVID_EV_WLDTIMER_01,
			WFLBY_MAPEVID_EV_WLDTIMER_02,
			WFLBY_MAPEVID_EV_WLDTIMER_03,
			WFLBY_EV_DEF_EVNO_END,
		},
		// イベントタイプ
		WFLBY_EV_DEF_EVTYPE_DEF,
		// イベント関数ポインタ
		{
			WFLBY_EV_DEF_PlayerOutSubChan_Before,
			NULL,
		},
		NULL,
	},
	// トピックに移行
	{	// 起動イベントNO
		{	
			WFLBY_MAPEVID_EV_TOPIC_00,
			WFLBY_MAPEVID_EV_TOPIC_01,
			WFLBY_MAPEVID_EV_TOPIC_02,
			WFLBY_MAPEVID_EV_TOPIC_03,
			WFLBY_EV_DEF_EVNO_END,
		},
		// イベントタイプ
		WFLBY_EV_DEF_EVTYPE_DEF,
		// イベント関数ポインタ
		{
			WFLBY_EV_DEF_PlayerOutSubChan_Before,
			NULL,
		},
		NULL,
	},

	// 足跡ボード
	{	// 起動イベントNO
		{	
			WFLBY_MAPEVID_EV_FOOT_00_00,
			WFLBY_MAPEVID_EV_FOOT_00_01,
			WFLBY_MAPEVID_EV_FOOT_00_02,
			WFLBY_MAPEVID_EV_FOOT_00_03,
			WFLBY_MAPEVID_EV_FOOT_00_04,
			WFLBY_MAPEVID_EV_FOOT_00_05,
			WFLBY_MAPEVID_EV_FOOT_00_06,
			WFLBY_MAPEVID_EV_FOOT_00_07,
		},
		// イベントタイプ
		WFLBY_EV_DEF_EVTYPE_DEF,
		// イベント関数ポインタ
		{
			WFLBY_EV_DEF_PlayerOutSubChan_Before,
			NULL,
		},
		NULL,
	},

	// 足跡ボード
	{	// 起動イベントNO
		{	
			WFLBY_MAPEVID_EV_FOOT_01_00,
			WFLBY_MAPEVID_EV_FOOT_01_01,
			WFLBY_MAPEVID_EV_FOOT_01_02,
			WFLBY_MAPEVID_EV_FOOT_01_03,
			WFLBY_MAPEVID_EV_FOOT_01_04,
			WFLBY_MAPEVID_EV_FOOT_01_05,
			WFLBY_MAPEVID_EV_FOOT_01_06,
			WFLBY_MAPEVID_EV_FOOT_01_07,
		},
		// イベントタイプ
		WFLBY_EV_DEF_EVTYPE_DEF,
		// イベント関数ポインタ
		{
			WFLBY_EV_DEF_PlayerOutSubChan_Before,
			NULL,
		},
		NULL,
	},
	
	// フロートに乗る
	{	// 起動イベントNO
		{	
			WFLBY_MAPEVID_EV_FLOAT00,
			WFLBY_MAPEVID_EV_FLOAT01,
			WFLBY_MAPEVID_EV_FLOAT02,
			WFLBY_EV_DEF_EVNO_END,
		},
		// イベントタイプ
		WFLBY_EV_DEF_EVTYPE_DEF,
		// イベント関数ポインタ
		{
			WFLBY_EV_FLOAT_Inside,
			NULL,
		},
		NULL,
	},

	// ミニゲームに移行
	{	// 起動イベントNO
		{	
			WFLBY_MAPEVID_EV_BALLSLOW,
			WFLBY_MAPEVID_EV_BALANCE,
			WFLBY_MAPEVID_EV_BALLOON,
			WFLBY_EV_DEF_EVNO_END,
		},
		// イベントタイプ
		WFLBY_EV_DEF_EVTYPE_DEF,
		// イベント関数ポインタ
		{
			WFLBY_EV_DEF_PlayerOutMiniGame_Before,
			NULL,
		},
		NULL,
	},

	// アンケートに移行
	{	// 起動イベントNO
		{	
			WFLBY_MAPEVID_EV_ANKETO_BOARD,
			WFLBY_EV_DEF_EVNO_END,
		},
		// イベントタイプ
		WFLBY_EV_DEF_EVTYPE_DEF,
		// イベント関数ポインタ
		{
			WFLBY_EV_DEF_PlayerOutAnketo,
			NULL,
		},
		NULL,
	},
	
	// 終端データ
	{
		{WFLBY_EV_DEF_EVNO_END,},WFLBY_EV_DEF_EVTYPE_END,{NULL,NULL,},NULL,
	},
};

// 目の前でAを押したとき
static const WFLBY_EV_DEF_EVDATA sc_WFLBY_EV_DEF_EVENT_ASTART[] = {

	// 入り口のお姉さん
	{	// 起動イベントNO
		{	
			WFLBY_MAPEVID_EV_SW_IN,
			WFLBY_EV_DEF_EVNO_END,
		},
		// イベントタイプ
		WFLBY_EV_DEF_EVTYPE_INTR,
		// イベント関数ポインタ
		{
			WFLBY_EV_DEF_PlayerA_SWIN_Before,
			NULL,
		},
		NULL,
	},

	// トピックのお姉さん
	{	// 起動イベントNO
		{	
			WFLBY_MAPEVID_EV_SW_TOPIC,
			WFLBY_EV_DEF_EVNO_END,
		},
		// イベントタイプ
		WFLBY_EV_DEF_EVTYPE_INTR,
		// イベント関数ポインタ
		{
			WFLBY_EV_DEF_PlayerA_SWTOPIC_Before,
			NULL,
		},
		NULL,
	},


	// トピックのお兄さん
	{	// 起動イベントNO
		{	
			WFLBY_MAPEVID_EV_SW_TOPIC2,
			WFLBY_EV_DEF_EVNO_END,
		},
		// イベントタイプ
		WFLBY_EV_DEF_EVTYPE_INTR,
		// イベント関数ポインタ
		{
#ifdef WFLBY_DEBUG_TALK_ALLWORLD
			WFLBY_EV_TALK_StartA_AllWorld,
#else
			WFLBY_EV_DEF_PlayerA_SWTOPIC_PLAYED_Before,
#endif
			NULL,
		},
		NULL,
	},

	// ミニゲーム
	{	// 起動イベントNO
		{	
			WFLBY_MAPEVID_EV_KANBAN00,
			WFLBY_MAPEVID_EV_KANBAN01,
			WFLBY_MAPEVID_EV_KANBAN02,
			WFLBY_EV_DEF_EVNO_END,
		},
		// イベントタイプ
		WFLBY_EV_DEF_EVTYPE_INTR,
		// イベント関数ポインタ
		{
			WFLBY_EV_DEF_PlayerA_MINIGAME_Before,
			NULL,
		},
		NULL,
	},

	// ピエロ
	{	// 起動イベントNO
		{	
			WFLBY_MAPEVID_EV_ANKETO_MAN,
			WFLBY_EV_DEF_EVNO_END,
		},
		// イベントタイプ
		WFLBY_EV_DEF_EVTYPE_INTR,
		// イベント関数ポインタ
		{
			WFLBY_EV_TALK_Piero,
			NULL,
		},
		NULL,
	},

	// 終端データ
	{
		{WFLBY_EV_DEF_EVNO_END,},WFLBY_EV_DEF_EVTYPE_END,{NULL,NULL,},NULL,
	},
};

// 目の前でUPを押したとき
static const WFLBY_EV_DEF_EVDATA sc_WFLBY_EV_DEF_EVENT_WAYUPSTART[] = {

	// ミニゲーム
	{	// 起動イベントNO
		{	
			WFLBY_MAPEVID_EV_KANBAN00,
			WFLBY_MAPEVID_EV_KANBAN01,
			WFLBY_MAPEVID_EV_KANBAN02,
			WFLBY_EV_DEF_EVNO_END,
		},
		// イベントタイプ
		WFLBY_EV_DEF_EVTYPE_INTR,
		// イベント関数ポインタ
		{
			WFLBY_EV_DEF_PlayerA_MINIGAME_Before,
			NULL,
		},
		NULL,
	},
	
	// 終端データ
	{
		{WFLBY_EV_DEF_EVNO_END,},WFLBY_EV_DEF_EVTYPE_END,{NULL,NULL,},NULL,
	},
};




//-------------------------------------
// 話しかけ割り込みイベント
//=====================================
static const WFLBY_EVENT_DATA	sc_WFLBY_EV_DEF_TALK_A = {
	WFLBY_EV_TALK_StartA,
	NULL,
};

//-------------------------------------
// 話しかられ割り込みイベント
//=====================================
static const WFLBY_EVENT_DATA	sc_WFLBY_EV_DEF_TALK_B = {
	WFLBY_EV_TALK_StartB,
	NULL,
};


//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤー常駐イベントを設定
 *
 *	@param	p_rmwk 
 */
//-----------------------------------------------------------------------------
void WFLBY_EV_DEF_StartPlayer( WFLBY_ROOMWK* p_rmwk )
{
	WFLBY_ROOM_SAVE* p_save;
	WFLBY_EVENT* p_event;
	u32 plno;
	u32 status;

	// データ取得
	p_save	= WFLBY_ROOM_GetSaveData( p_rmwk ); 
	p_event	= WFLBY_ROOM_GetEvent( p_rmwk );
	plno	= WFLBY_ROOM_GetPlno( p_rmwk );

	// ログイン状態にする
	WFLBY_SYSTEM_SetMyStatus( WFLBY_ROOM_GetSystemData( p_rmwk ), WFLBY_STATUS_LOGIN );

	
	// プレイヤー入場イベントが不正
	GF_ASSERT( p_save->pl_inevno < WFLBY_EV_DEF_PLAYERIN_NUM );

	WFLBY_EVENT_SET_PRIVATE_EVENT( p_event, plno, &sc_WFLBY_EV_DEF_PLAYERIN[ p_save->pl_inevno ], NULL );

#if PL_T0867_080716_FIX
	// 今から移動しまくるので話しかけられても他人は会話ジャンプしない
	WFLBY_ROOM_SetPlayerEventAfterMove( p_rmwk, TRUE );
#endif

#ifdef WFLBY_DEBUG_EV_DEF_FLOAT_RETRY
	OS_TPrintf( "■フロートデバック機能	Bボタン	もう一度部屋に戻ってくる\n" );
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	NPC	常駐イベント
 *
 *	@param	p_rmwk 
 */
//-----------------------------------------------------------------------------

void WFLBY_EV_DEF_StartNpc( WFLBY_ROOMWK* p_rmwk )
{
	int i;
	u32 plno;
	WFLBY_EVENT* p_event;

	plno	= WFLBY_ROOM_GetPlno( p_rmwk );
	p_event = WFLBY_ROOM_GetEvent( p_rmwk );

	for( i=0; i<WFLBY_PLAYER_MAX; i++ ){
		if( plno != i ){	// プレイヤー以外
			WFLBY_EVENT_SET_PRIVATE_EVENT( p_event, i, &sc_WFLBY_EV_DEF_NPCWAIT, NULL );
		}
	}
}



//-----------------------------------------------------------------------------
/**
 *		プライベート関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	向いている方向のキーを入力しているかチェック
 *
 *	@param	way		向いている方向
 *
 *	@retval	TRUE	向いている
 *	@retval	FALSE	向いていない
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EV_DEF_WayKeyInput( WF2DMAP_WAY way )
{
	u32 key;
	switch( way ){
	case WF2DMAP_WAY_UP:
		key = PAD_KEY_UP;
		break;
	case WF2DMAP_WAY_DOWN:
		key = PAD_KEY_DOWN;
		break;
	case WF2DMAP_WAY_LEFT:
		key = PAD_KEY_LEFT;
		break;
	case WF2DMAP_WAY_RIGHT:
		key = PAD_KEY_RIGHT;
		break;
	}
	if( sys.cont & key ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	イベントテーブルのイベントを起動する
 *
 *	@param	p_wk		イベントワーク
 *	@param	p_rmwk		部屋ワーク
 *	@param	cp_data		イベントテーブル
 *	@param	eventno		イベントナンバー
 *
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_DEF_Player_StartEventTbl( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno, const WFLBY_EV_DEF_EVDATA* cp_data, u32 eventno )
{
	int no;
	int i;
	WFLBY_EV_DEF_PLAYER_WK* p_plwk;
	WFLBY_EVENT* p_event;

	// イベントワーク取得
	p_plwk = WFLBY_EVENTWK_GetWk( p_wk );

	// 部屋からデータ取得
	{
		p_event = WFLBY_ROOM_GetEvent( p_rmwk );
	}


	no = 0;
	while( cp_data[no].evtype != WFLBY_EV_DEF_EVTYPE_END ){

		// 起動イベントNoをチェック
		for( i=0; i<WFLBY_EV_DEF_EVNO_MAX; i++ ){

			// 終了コード
			if( cp_data[no].event[i] == WFLBY_EV_DEF_EVNO_END ){
				break;
			}

			// イベントナンバーが一緒ならそのイベントを発動
			if( cp_data[no].event[i] == eventno ){
				// イベントタイプで起動方法が変わる
				switch( cp_data[no].evtype ){
				// 常駐イベント
				case WFLBY_EV_DEF_EVTYPE_DEF:

					// 自分のワークをはき
					WFLBY_EVENTWK_DeleteWk( p_wk );

					WFLBY_EVENT_SET_PRIVATE_EVENT( p_event, plno, &cp_data[no].evdata, NULL );
					break;	
				// 割り込みイベント
				case WFLBY_EV_DEF_EVTYPE_INTR:
					// パラメータ生成
					if( cp_data[no].p_pfunc ){
						cp_data[no].p_pfunc( p_plwk, p_rmwk, plno, cp_data[no].event[i] );
					}
					WFLBY_EVENT_SET_PRIVATE_INTR_EVENT( p_event, plno, &cp_data[no].evdata, NULL );
					break;		
				}
				return ;
			}
		}

		// 次のテーブルへ
		no++;
	}

	// 設定できなかった
	GF_ASSERT(0);
	return ;
}

//----------------------------------------------------------------------------
/**
 *	@brief	主人公の登録
 */
//-----------------------------------------------------------------------------
static WFLBY_3DPERSON* WFLBY_EV_DEF_StartObjPlayer( WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	WFLBY_3DPERSON* p_player;
	WFLBY_3DOBJCONT* p_objcont;
	WFLBY_CAMERA* p_camera;
	VecFx32 vec;


	// データ取得
	p_objcont	= WFLBY_ROOM_GetObjCont( p_rmwk );
	
	p_player = WFLBY_3DOBJCONT_AddPlayer( p_objcont, plno );


	// ターゲットを設定
	p_camera = WFLBY_ROOM_GetCamera( p_rmwk );
	WFLBY_3DOBJCONT_DRAW_Get3DMatrix( p_player, &vec );
	WFLBY_CAMERA_SetTargetPerson3D( p_camera, vec.x, vec.y, vec.z );

	return p_player;
}

//----------------------------------------------------------------------------
/**
 *	@brief	主人公の登録	グリッド指定
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_DEF_StartObjPlayerEx( WFLBY_ROOMWK* p_rmwk, u32 plno, u32 gridx, u32 gridy, WF2DMAP_WAY way )
{
	WFLBY_3DPERSON* p_player;
	WFLBY_3DOBJCONT* p_objcont;
	WFLBY_CAMERA*	p_camera;

	// データ取得
	p_objcont	= WFLBY_ROOM_GetObjCont( p_rmwk );
	p_camera	= WFLBY_ROOM_GetCamera( p_rmwk );
	
	p_player = WFLBY_3DOBJCONT_AddPlayerEx( p_objcont, plno, gridx, gridy );

	// 下を向かせる
	WFLBY_3DOBJCONT_SetWkReqCmd( p_objcont, p_player, WF2DMAP_CMD_NONE, way );

	// ターゲットを設定
	WFLBY_CAMERA_SetTargetPerson( p_camera,
			p_player );
}



//----------------------------------------------------------------------------
/**
 *	@brief	ドンドンアニメ	開始
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_DEF_DONDON_Start( WFLBY_EV_DEF_DONDON* p_wk )
{
	p_wk->y			= 0;
	p_wk->count		= 0;
	p_wk->seq		= WFLBY_EV_DEF_DONDON_SEQ_WAIT;
	p_wk->next_wait	= gf_mtRand() % WFLBY_EV_DEF_DONDON_NEXT_WAIT_MAX;
	p_wk->next_ok	= TRUE;
}
	
//----------------------------------------------------------------------------
/**
 *	@brief	ドンドンアニメ	メイン
 *
 *	@param	p_wk	ワーク
 *
 *	@param	y座標
 */
//-----------------------------------------------------------------------------
static fx32 WFLBY_EV_DEF_DONDON_Main( WFLBY_EV_DEF_DONDON* p_wk )
{
	u16 rot_y;
	
	switch( p_wk->seq ){
	case WFLBY_EV_DEF_DONDON_SEQ_END:
		p_wk->y = 0;
		break;
		
	case WFLBY_EV_DEF_DONDON_SEQ_WAIT:
		if( (p_wk->next_wait - 1) > 0 ){
			p_wk->next_wait --;
		}else{
			p_wk->seq = WFLBY_EV_DEF_DONDON_SEQ_MOVE;
		}
		break;
		
	case WFLBY_EV_DEF_DONDON_SEQ_MOVE:
		p_wk->count ++;

		rot_y	= (p_wk->count * 0x7fff) / WFLBY_EV_DEF_DONDON_SYNC_ONE;
		p_wk->y	= FX_Mul( FX_SinIdx( rot_y ), WFLBY_EV_DEF_DONDON_MOVEY );

		if( p_wk->count >= WFLBY_EV_DEF_DONDON_SYNC_ONE ){
			if( p_wk->next_ok == TRUE ){
				// 待ち時間設定
				p_wk->count		= 0;
				p_wk->seq		= WFLBY_EV_DEF_DONDON_SEQ_WAIT;
				p_wk->next_wait	= gf_mtRand() % WFLBY_EV_DEF_DONDON_NEXT_WAIT_MAX;

			}else{
				// 終了
				p_wk->seq = WFLBY_EV_DEF_DONDON_SEQ_END;
			}
		}
		break;

	}

	return p_wk->y;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ドンドンアニメ	ｙ座標取得
 *
 *	@param	cp_wk	ワーク
 *
 *	@return	ｙ座標
 */
//-----------------------------------------------------------------------------
static fx32 WFLBY_EV_DEF_DONDON_GetY( const WFLBY_EV_DEF_DONDON* cp_wk )
{
	return cp_wk->y;
}

//----------------------------------------------------------------------------
/**
 *	@brief	終了リクエスト設定
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_DEF_DONDON_EndReq( WFLBY_EV_DEF_DONDON* p_wk )
{
	p_wk->next_ok = FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ドンドンアニメ中かチェック
 *
 *	@param	cp_wk	ワーク
 *
 *	@retval	TRUE	動作中
 *	@retval	FALSE	ちがう
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EV_DEF_DONDON_CheckMove( const WFLBY_EV_DEF_DONDON* cp_wk )
{
	if( cp_wk->seq == WFLBY_EV_DEF_DONDON_SEQ_END ){
		return FALSE;
	}
	return TRUE;
}




//----------------------------------------------------------------------------
/**
 *	@brief	全人物キャラクタ共通	ガジェット使用イベント
 *
 *	@param	p_wk		ワーク
 *	@param	p_rmwk		部屋ワーク
 *	@param	plno		プレイヤーIDX
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EV_DEF_ALL_IntrGadGet_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	WFLBY_3DOBJCONT*	p_objcont;
	WFLBY_3DPERSON*		p_person;
	WFLBY_SYSTEM*		p_system;

	p_objcont	= WFLBY_ROOM_GetObjCont( p_rmwk );
	p_person	= WFLBY_3DOBJCONT_GetPlIDWk( p_objcont, plno );
	p_system	= WFLBY_ROOM_GetSystemData( p_rmwk );
	
	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){
	// 初期化と、プレイヤーの動作を固定と、ガジェット開始
	case WFLBY_EV_DEF_ALL_GADGET_SEQ_INIT:
		// 下を向かせる
//		WFLBY_3DOBJCONT_SetWkActCmd( p_objcont, p_person, WF2DMAP_CMD_NONE, WF2DMAP_WAY_DOWN );

		// ガジェット開始
		WFLBY_ROOM_GadGet_Play( p_rmwk, plno );

		// ウエイト状態へ
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_ALL_GADGET_SEQ_WAIT );

		// 自分が主人公かチェック
		if( plno == WFLBY_SYSTEM_GetMyIdx( p_system ) ){
			// した画面ボタン操作開始
			WFLBY_ROOM_GadGet_SetStop( p_rmwk, FALSE );
		}
		break;
		
	// ガジェット終了待ち
	case WFLBY_EV_DEF_ALL_GADGET_SEQ_WAIT:
		if( WFLBY_ROOM_GadGet_PlayWait( p_rmwk, plno ) == TRUE ){
			// 終了処理へ
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_ALL_GADGET_SEQ_END );
		}
		break;
		
	case WFLBY_EV_DEF_ALL_GADGET_SEQ_END:
		// 動作再開
		// 自分が主人公かチェック
		if( plno == WFLBY_SYSTEM_GetMyIdx( p_system ) ){
			// 主人公動作設定
			WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_person, WFLBY_3DOBJCONT_MOVEPLAYER );

#if	PL_T0834_080709_FIX
		WFLBY_ROOM_GadGet_ClearLockWait( p_rmwk );
#else
		//
#endif
		}else{
		

			// profile表示中の場合動作を復帰させない
			// それ以外は復帰させる
			if( (WFLBY_ROOM_UNDERWIN_TrCardGetDraw( p_rmwk ) == TRUE) ){
				if( plno != WFLBY_ROOM_UNDERWIN_TrCardGetPlIdx( p_rmwk ) ){
					// NPC動作設定
					WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_person, WFLBY_3DOBJCONT_MOVENPC );
				}
			}else{
				// NPC動作設定
				WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_person, WFLBY_3DOBJCONT_MOVENPC );
			}

			// ローカル状態のBUSYを元に戻す
			// 会話応答停止
			WFLBY_ROOM_SetNpcTalkBusy( p_rmwk, plno, FALSE );

			// NPC状態表示不可能 
//			WFLBY_ROOM_SetNpcInfoDrawBusy( p_rmwk, plno, FALSE );				
		}
		
#if	PL_T0834_080709_FIX
		//
#else
		WFLBY_ROOM_GadGet_ClearLockWait( p_rmwk );
#endif
		return TRUE;
	}

	return FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤー	出現管理イベント
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EV_DEF_PlayerInNormal_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	WFLBY_EV_DEF_PLAYER_IN_WK* p_evwk;
	WFLBY_3DPERSON* p_player;
	WFLBY_3DOBJCONT* p_objcont;
	WFLBY_SYSTEM*	p_system;
	STRBUF* p_str;
	
	p_system	= WFLBY_ROOM_GetSystemData( p_rmwk );
	p_objcont	= WFLBY_ROOM_GetObjCont( p_rmwk );
	p_player	= WFLBY_3DOBJCONT_GetPlayer( p_objcont );
	p_evwk		= WFLBY_EVENTWK_GetWk( p_wk );

	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){
	// 主人公に登録
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_ADD:
		// work作成
		p_evwk = WFLBY_EVENTWK_AllocWk( p_wk, sizeof(WFLBY_EV_DEF_PLAYER_IN_WK) );

		//  技タイプリストデータを作成
		WFLBY_EV_DEF_PlayerInNormal_WazaTypeBmpListInit( p_evwk, p_rmwk );

		// 部屋に入れたのでBGMを変更
		WFLBY_SYSTEM_SetNowBGM( p_system );

		// ガジェット設定
		{

#ifdef WFLBY_DEBUG_EV_DEF_GADGET_GET
			const WFLBY_USER_PROFILE* cp_profile;

			// デバック実行したときのみ、選んだガジェットを反映
			if( D_Tomoya_WiFiLobby_DebugStart == TRUE ){
				cp_profile = WFLBY_SYSTEM_GetUserProfile( p_system, plno );
				p_evwk->gadget = WFLBY_SYSTEM_GetProfileItem( cp_profile );
			}else{
				p_evwk->gadget = WFLBY_SYSTEM_RandItemGet( p_system );
			}
#else
			p_evwk->gadget = WFLBY_SYSTEM_RandItemGet( p_system );
#endif
			WFLBY_SYSTEM_SetMyItem( p_system, p_evwk->gadget );
		}
		
		// 主人公の登録
		p_player = WFLBY_EV_DEF_StartObjPlayer( p_rmwk, plno );

		// 落とす
		WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_player, WFLBY_3DOBJCONT_MOVEFLYDOWN );

		WFLBY_EVENTWK_AddSeq( p_wk );
		break;
	// フェードイン完了と動作完了待ち
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_FADEWAIT:
		if( (WFLBY_ROOM_GetFadeFlag( p_rmwk ) == FALSE) &&
			(WFLBY_3DOBJCONT_CheckWkMoveEnd( p_player ) == TRUE) ){

			WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_player, WFLBY_3DOBJCONT_MOVENONE );

			// ターゲットを設定
			{
				WFLBY_CAMERA* p_camera;
				p_camera = WFLBY_ROOM_GetCamera( p_rmwk );
				WFLBY_CAMERA_SetTargetPerson( p_camera,
						p_player );
			}

			p_evwk->wait = WFLBY_EV_DEF_PLAYERIN_NORMAL_MOVEWAIT;


			WFLBY_EVENTWK_AddSeq( p_wk );
		}
		break;

	// 動作ウエイト
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MOVESTARTWAIT:
		p_evwk->wait --;
		if( p_evwk->wait == 0 ){

			// 通信エラーチェック	ON
			WFLBY_ROOM_SetErrorCheckFlag( p_rmwk, TRUE );

			WFLBY_EVENTWK_AddSeq( p_wk );
		}
		break;

	// 前へ
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MOVE00:	
		WFLBY_3DOBJCONT_SetWkReqCmd( p_objcont, p_player, WF2DMAP_OBJST_WALK, WF2DMAP_WAY_UP );
		p_evwk->ret_seq = WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MOVE01;
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MOVEWAIT );
		break;

	// 前へ
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MOVE01:	
		WFLBY_3DOBJCONT_SetWkReqCmd( p_objcont, p_player, WF2DMAP_OBJST_WALK, WF2DMAP_WAY_UP );
		p_evwk->ret_seq = WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MOVE02;
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MOVEWAIT );
		break;

	// ターン
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MOVE02:	
		WFLBY_3DOBJCONT_SetWkReqCmd( p_objcont, p_player, WF2DMAP_OBJST_TURN, WF2DMAP_WAY_RIGHT );
		p_evwk->ret_seq = WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MOVE03;
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MOVEWAIT );
		break;

	// 横へ
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MOVE03:	
		WFLBY_3DOBJCONT_SetWkReqCmd( p_objcont, p_player, WF2DMAP_OBJST_WALK, WF2DMAP_WAY_RIGHT );
		p_evwk->ret_seq = WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG00;
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MOVEWAIT );
		break;

	// 動作完了待ち
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MOVEWAIT:	
		if( WFLBY_3DOBJCONT_GetWkObjData( p_player, WF2DMAP_OBJPM_ST ) == WF2DMAP_OBJST_NONE ){
			WFLBY_EVENTWK_SetSeq( p_wk, p_evwk->ret_seq );
		}
		break;

	// ”ようこそ”
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG00:

#if PL_T0867_080716_FIX
		// もう移動しないので、他人が会話ジャンプしてOK
		WFLBY_ROOM_SetPlayerEventAfterMove( p_rmwk, FALSE );
#endif
		
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoa_01 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		p_evwk->ret_seq = WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG15;	// 先にタッチトイをもらう
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSGWAIT );

		break;

	// ”１つめのタイプをえらんでね”
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG01:		
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoa_t02 );
		WFLBY_ROOM_TALKWIN_PrintStrAllPut( p_rmwk, p_str );

		WFLBY_ROOM_LISTWIN_Start( p_rmwk, &p_evwk->bmplist, 0, 0 );
		WFLBY_ROOM_LISTWIN_YAZIRUSHI_SetDraw( p_rmwk, TRUE );	// 矢印表示
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG03 );
		break;
		
/*
	// ”選択表示（１つめ）”
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG02:
		WFLBY_ROOM_LISTWIN_Start( p_rmwk, &p_evwk->bmplist, 0, 0 );
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG03 );
		break;
//*/
		
	// ”選択（１つめ）”
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG03:		
		{
			u32 result;
			u32 str_no;
			BOOL delete = FALSE;
			result = WFLBY_ROOM_LISTWIN_Main( p_rmwk );

			switch( result ){
			case BMPLIST_NULL:	
				break;

			// 選ばない
			case BMPLIST_CANCEL:
				Snd_SePlay( SEQ_SE_DP_SELECT );	// CANCEL音
			case WFLBY_POKEWAZA_TYPE_NONE:
				p_evwk->type1 = WFLBY_POKEWAZA_TYPE_NONE;
				delete = TRUE;
				break;
			
			// 技タイプを選んだ
			default:
				p_evwk->type1 = result;
				delete = TRUE;
				WFLBY_ROOM_MSG_SetWazaType( p_rmwk, 0, result );
				str_no = msg_hiroba_infoa_t05;
				break;
			}

			if( delete == TRUE ){
				WFLBY_ROOM_LISTWIN_End( p_rmwk, NULL, NULL );

				if( p_evwk->type1 != WFLBY_POKEWAZA_TYPE_NONE ){

					p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, str_no );
					WFLBY_ROOM_TALKWIN_PrintStrAllPut( p_rmwk, p_str );

					WFLBY_ROOM_YESNOWIN_Start( p_rmwk );
					WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG06 );
				}else{

					WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG16 );	// オワリへ
				}
			}
		}
		break;
		
/*
	// ”これでよいですか？（１つめ）”
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG04:		
		break;
*/
		
/*
	// ”YESNO表示（１つめ）”
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG05:
		WFLBY_ROOM_YESNOWIN_Start( p_rmwk );
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG06 );
		break;
*/
		
	// ”YESNO（１つめ）”
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG06:		
		{
			WFLBY_ROOM_YESNO_RET result;

			result = WFLBY_ROOM_YESNOWIN_Main( p_rmwk );
			switch( result ){
			// はい
			case WFLBY_ROOM_YESNO_OK:
				// YESNOウィンドウを消しす
				WFLBY_ROOM_YESNOWIN_End( p_rmwk );
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG07 );

				// 選択した技タイプを設定
				WFLBY_SYSTEM_SetSelectWazaType( p_system, p_evwk->type1, p_evwk->type2 );
				WFLBY_ROOM_UNDERWIN_TrCardWazaTypeWrite( p_rmwk );

				Snd_SePlay( WFLBY_SND_TYPE_SET );
				break;
				
			// いいえ
			case WFLBY_ROOM_YESNO_NO:	
				// YESNOウィンドウを消しす
				WFLBY_ROOM_YESNOWIN_End( p_rmwk );
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG01 );
				break;

			default:
				break;
			}
		}
		break;
		
	// ”２つめのタイプをえらんでね”
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG07:		
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoa_t06 );
		WFLBY_ROOM_TALKWIN_PrintStrAllPut( p_rmwk, p_str );
		WFLBY_ROOM_LISTWIN_Start( p_rmwk, &p_evwk->bmplist, 0, 0 );
		WFLBY_ROOM_LISTWIN_YAZIRUSHI_SetDraw( p_rmwk, TRUE );	// 矢印表示
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG09 );
		break;
		
/*
	// ”選択表示（２つめ）”
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG08:
		WFLBY_ROOM_LISTWIN_Start( p_rmwk, &p_evwk->bmplist, 0, 0 );
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG09 );
		break;
//*/
		
	// ”選択（２つめ）”
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG09:		
		{
			u32 result;
			u32 str_no;
			BOOL delete = FALSE;
			result = WFLBY_ROOM_LISTWIN_Main( p_rmwk );

			switch( result ){
			case BMPLIST_NULL:	
				break;

			// 選ばない
			case BMPLIST_CANCEL:
				Snd_SePlay( SEQ_SE_DP_SELECT );	// CANCEL音
			case WFLBY_POKEWAZA_TYPE_NONE:
				p_evwk->type2 = WFLBY_POKEWAZA_TYPE_NONE;
				delete = TRUE;
				break;
			
			// 技タイプを選んだ
			default:
				p_evwk->type2 = result;
				delete = TRUE;
				WFLBY_ROOM_MSG_SetWazaType( p_rmwk, 0, result );
				str_no = msg_hiroba_infoa_t05;
				break;
			}

			if( delete == TRUE ){
				WFLBY_ROOM_LISTWIN_End( p_rmwk, NULL, NULL );

				if( p_evwk->type2 != WFLBY_POKEWAZA_TYPE_NONE ){

					p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, str_no );
					WFLBY_ROOM_TALKWIN_PrintStrAllPut( p_rmwk, p_str );

					WFLBY_ROOM_YESNOWIN_Start( p_rmwk );
					WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG12 );
				}else{

					// １つは選んでるので、お姉さんのお言葉
					WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG18 );
				}
			}
		}
		break;
		
/*
	// ”これでよいですか？（２つめ）”
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG10:		
		break;
*/
/*		
	// ”YESNO表示（２つめ）”
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG11:		
		WFLBY_ROOM_YESNOWIN_Start( p_rmwk );
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG12 );
		break;
*/
		
	// ”YESNO（２つめ）”
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG12:		
		{
			WFLBY_ROOM_YESNO_RET result;

			result = WFLBY_ROOM_YESNOWIN_Main( p_rmwk );
			switch( result ){
			// はい
			case WFLBY_ROOM_YESNO_OK:
				
				// YESNOウィンドウを消しす
				WFLBY_ROOM_YESNOWIN_End( p_rmwk );
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG19 );	//　お姉さんのお言葉

				// 選択した技タイプを設定
				WFLBY_SYSTEM_SetSelectWazaType( p_system, p_evwk->type1, p_evwk->type2 );
				WFLBY_ROOM_UNDERWIN_TrCardWazaTypeWrite( p_rmwk );

				Snd_SePlay( WFLBY_SND_TYPE_SET );
				break;
				
			// いいえ
			case WFLBY_ROOM_YESNO_NO:	
				// YESNOウィンドウを消しす
				WFLBY_ROOM_YESNOWIN_End( p_rmwk );
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG07 );
				break;

			default:
				break;
			}
		}
		break;

/*
	// ”プロフィールをもらった”
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG13:
		Snd_SePlay( WFLBY_SND_TOUCH_TOY_CHG );

		// 選択した技タイプを設定
		WFLBY_SYSTEM_SetSelectWazaType( p_system, p_evwk->type1, p_evwk->type2 );

		// ガジェット取得	ボタン表示　・・・↓
		// トレーナーカード表示に変更
		WFLBY_ROOM_GadGet_Get( p_rmwk );

		// 人物名設定
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, plno, 0 );
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoa_t06_01 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		p_evwk->ret_seq = WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG14;
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSGWAIT );
		break;
*/
/*
	// ”タッチトイをもらった！”
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG14:

		// 人物名設定
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoa_t07 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		p_evwk->ret_seq = WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG15;
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSGWAIT );

		break;
*/

	// ”ガジェットをもらった”
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG15:


		Snd_SePlay( WFLBY_SND_TOUCH_TOY_CHG );

		// ガジェット取得
		WFLBY_ROOM_GadGet_Get( p_rmwk );

		// 人物名設定
		WFLBY_ROOM_MSG_SetIdxPlayerName( p_rmwk, plno, 0 );
		WFLBY_ROOM_MSG_SetItem( p_rmwk, p_evwk->gadget, 1 );
		{
			BOOL vip;
			vip = WFLBY_SYSTEM_GetUserVipFlag( p_system, plno );
			if( vip == FALSE ){
				p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoa_02 );
			}else{
				p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoa_02_vip );
			}
		}
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		p_evwk->ret_seq = WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG17;	// タイプ選択へ
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSGWAIT );
		break;

	// ”たのしんでね！”
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG16:
		
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoa_03 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		p_evwk->ret_seq = WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_END;
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSGWAIT );
		break;

	// ”よろしければ　タイプ選択”
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG17:

		if( !((Snd_SePlayCheck( WFLBY_SND_TOUCH_TOY_CHG ) == FALSE) && (sys.trg & (PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL))) ){
			break;
		}

		Snd_SePlay( SEQ_SE_DP_SELECT );
		
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoa_t01 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		p_evwk->ret_seq = WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG01;
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSGWAIT );


		// 技タイプ初期化
		WFLBY_SYSTEM_SetSelectWazaType( p_system, p_evwk->type1, p_evwk->type2 );
		// トレーナカード表示
		WFLBY_ROOM_UNDERWIN_TrCardOn( p_rmwk, plno, FALSE );
		break;

	// ”１こ選択したときの　お姉さんのお言葉”
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG18:

		WFLBY_ROOM_MSG_SetWazaType( p_rmwk, 0, p_evwk->type1 );
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoa_t07 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		p_evwk->ret_seq = WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG16;
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSGWAIT );
		break;
		
	// ”２こ選択したときの　お姉さんのお言葉”
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG19:
		WFLBY_ROOM_MSG_SetWazaType( p_rmwk, 0, p_evwk->type1 );
		WFLBY_ROOM_MSG_SetWazaType( p_rmwk, 1, p_evwk->type2 );
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoa_t06_01 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
		p_evwk->ret_seq = WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSG16;
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSGWAIT );
		break;

	// メッセージ終了待ち
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_MSGWAIT:	
		if( WFLBY_ROOM_TALKWIN_WaitEnd( p_rmwk ) == TRUE ){
			WFLBY_EVENTWK_SetSeq( p_wk, p_evwk->ret_seq );
		}
		break;

	// 入場完了
	case WFLBY_EV_DEF_PLAYERIN_NORMAL_SEQ_END:
		// 基本動作の関数を設定
		WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_player, WFLBY_3DOBJCONT_MOVEPLAYER );
		

		// メッセージOFF
		WFLBY_ROOM_TALKWIN_Off( p_rmwk );

		// 技タイプリストデータ破棄
		WFLBY_EV_DEF_PlayerInNormal_WazaTypeBmpListDelete( p_evwk, p_rmwk );

		// ワーク破棄
		WFLBY_EVENTWK_DeleteWk( p_wk );

		// プレイヤーメイン常駐イベントを設定
		{
			WFLBY_EVENT* p_event;
			u32 plno;
			p_event = WFLBY_ROOM_GetEvent( p_rmwk );
			plno = WFLBY_ROOM_GetPlno( p_rmwk );
			WFLBY_EVENT_SET_PRIVATE_EVENT( p_event, plno, &sc_WFLBY_EV_DEF_PLAYERMAIN, NULL );

		}

		break;
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤー	出現管理イベント	WFLBY_ROOM_SAVEのグリッド位置に出現
 *			グリッドを指定して入場(SUBCHANからの退避)
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EV_DEF_PlayerInGrid_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	WFLBY_3DPERSON* p_player;
	WFLBY_3DOBJCONT* p_objcont;
	WFLBY_ROOM_SAVE* p_save;
	WFLBY_SYSTEM*	p_system;

	p_system	= WFLBY_ROOM_GetSystemData( p_rmwk );
	p_save		= WFLBY_ROOM_GetSaveData( p_rmwk );
	p_objcont	= WFLBY_ROOM_GetObjCont( p_rmwk );
	p_player	= WFLBY_3DOBJCONT_GetPlayer( p_objcont );


	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){
		
	// 主人公を登録
	case WFLBY_EV_DEF_PLAYERIN_GRID_SEQ_ADD:
		// 主人公の登録
		WFLBY_EV_DEF_StartObjPlayerEx( p_rmwk, plno,
				p_save->pl_gridx, p_save->pl_gridy,
				WF2DMAP_WAY_DOWN );


		// ログアウトもする
		// エラーじゃなければ
		if( WFLBY_ERR_CheckError() == FALSE ){
			DWC_LOBBY_SUBCHAN_Logout();
		}

		// BGMをフェードイン
		WFLBY_SYSTEM_SetBGMVolumeDown( p_system, FALSE );

		WFLBY_EVENTWK_AddSeq( p_wk );
		break;
	// フェードイン待ち
	case WFLBY_EV_DEF_PLAYERIN_GRID_SEQ_FADEWAIT:

		// LOGOUT待ち
		// エラーじゃなければ
		if( (WFLBY_ERR_CheckError() == FALSE) ){
			if( DWC_LOBBY_SUBCHAN_LogoutWait() == FALSE ){
				break;
			}
		}
		
		// フェードイン待ち
		p_save = WFLBY_ROOM_GetSaveData( p_rmwk );
		if( WFLBY_ROOM_GetFadeFlag( p_rmwk ) == FALSE ){
			WFLBY_EVENTWK_AddSeq( p_wk );
			// 動作リクエスト
			WFLBY_3DOBJCONT_SetWkActCmd( p_objcont, p_player, WF2DMAP_CMD_WALK, p_save->pl_way );
		}
		break;

	// プレイヤー動作処理
	case WFLBY_EV_DEF_PLAYERIN_GRID_SEQ_PLMOVE:
		{
			u32 plst;
			plst = WFLBY_3DOBJCONT_GetWkObjData( p_player, WF2DMAP_OBJPM_ST );
			if( plst == WF2DMAP_OBJST_NONE ){


#if PL_T0867_080716_FIX
				// もう移動しないので、他人が会話ジャンプしてOK
				WFLBY_ROOM_SetPlayerEventAfterMove( p_rmwk, FALSE );
#endif

				// 通信エラーチェック	ON
				WFLBY_ROOM_SetErrorCheckFlag( p_rmwk, TRUE );

				if( WFLBY_SYSTEM_APLFLAG_GetForceEnd( p_system ) == TRUE ){
					WFLBY_EVENTWK_AddSeq( p_wk );
				}else{
					WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_GRID_SEQ_END );
				}
			}
		}
		break;

	// アプリケーション強制退室メッセージの表示
	case WFLBY_EV_DEF_PLAYERIN_GRID_SEQ_APLFORCEEND_MSG:
		{
			STRBUF* p_str;
			WFLBY_ROOM_MSG_SetMinigame( p_rmwk, p_save->minigame, 0 );
			p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msg_wifi_h_info_05_02 );
			WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
			WFLBY_EVENTWK_AddSeq( p_wk );
		}
		break;
		
	// アプリケーション強制退室メッセージの表示完了待ち
	case WFLBY_EV_DEF_PLAYERIN_GRID_SEQ_APLFORCEEND_MSGWAIT:
		if( WFLBY_ROOM_TALKWIN_WaitEnd( p_rmwk ) == TRUE ){
			WFLBY_ROOM_TALKWIN_Off( p_rmwk );
			WFLBY_EVENTWK_AddSeq( p_wk );
		}
		break;
	
	// 入場完了
	case WFLBY_EV_DEF_PLAYERIN_GRID_SEQ_END:
		// 基本動作の関数を設定
		WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_player, WFLBY_3DOBJCONT_MOVEPLAYER );

		{
			WFLBY_EVENT* p_event;
			u32 plno;
			p_event = WFLBY_ROOM_GetEvent( p_rmwk );
			plno	= WFLBY_ROOM_GetPlno( p_rmwk );
			// プレイヤーメイン常駐イベントを設定
			WFLBY_EVENT_SET_PRIVATE_EVENT( p_event, plno, &sc_WFLBY_EV_DEF_PLAYERMAIN, NULL );
		}
		break;
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ミニゲーム後の復帰	WFLBY_ROOM_SAVEのグリッド位置に出現
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EV_DEF_PlayerInMinigame_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	WFLBY_3DPERSON* p_player;
	WFLBY_3DOBJCONT* p_objcont;
	WFLBY_EVENT* p_event;
	WFLBY_ROOM_SAVE* p_save;
	WFLBY_SYSTEM*	p_system;


	p_event		= WFLBY_ROOM_GetEvent( p_rmwk );
	p_objcont	= WFLBY_ROOM_GetObjCont( p_rmwk );
	p_player	= WFLBY_3DOBJCONT_GetPlayer( p_objcont );
	p_system	= WFLBY_ROOM_GetSystemData( p_rmwk );
	p_save		= WFLBY_ROOM_GetSaveData( p_rmwk );

	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){
		
	// 主人公を登録
	case WFLBY_EV_DEF_PLAYERIN_MG_SEQ_ADD:
		{
			
			// 主人公の登録
			WFLBY_EV_DEF_StartObjPlayerEx( p_rmwk, plno,
					p_save->pl_gridx, p_save->pl_gridy,
					WF2DMAP_WAY_DOWN );

			WFLBY_EVENTWK_AddSeq( p_wk );

			// 割り込みイベントで切断処理
			WFLBY_EVENT_SET_PRIVATE_INTR_EVENT( p_event, plno, 
					&sc_WFLBY_EV_DEF_PLAYER_MGEND, NULL );

			// BGM変更ロックを解除
			WFLBY_SYSTEM_SetLockBGMChange( p_system, FALSE );

			// BGMを変更
			WFLBY_SYSTEM_SetNowBGM( p_system );
			
		}
		break;
	// フェードイン待ち
	case WFLBY_EV_DEF_PLAYERIN_MG_SEQ_FADEWAIT:

		// フェードイン待ち
		{
			WFLBY_ROOM_SAVE* p_save;
			p_save = WFLBY_ROOM_GetSaveData( p_rmwk );
			if( WFLBY_ROOM_GetFadeFlag( p_rmwk ) == FALSE ){
				WFLBY_EVENTWK_AddSeq( p_wk );
				// 動作リクエスト
				WFLBY_3DOBJCONT_SetWkActCmd( p_objcont, p_player, WF2DMAP_CMD_WALK, p_save->pl_way );
			}
		}
		break;

	// プレイヤー動作処理
	case WFLBY_EV_DEF_PLAYERIN_MG_SEQ_PLMOVE:
		{
			u32 plst;
			plst = WFLBY_3DOBJCONT_GetWkObjData( p_player, WF2DMAP_OBJPM_ST );
			if( plst == WF2DMAP_OBJST_NONE ){

#if PL_T0867_080716_FIX
				// もう移動しないので、他人が会話ジャンプしてOK
				WFLBY_ROOM_SetPlayerEventAfterMove( p_rmwk, FALSE );
#endif

				// 通信エラーチェック	ON
				WFLBY_ROOM_SetErrorCheckFlag( p_rmwk, TRUE );

				if( WFLBY_SYSTEM_MG_CheckErrorEnd( p_system ) == TRUE ){
					WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_MG_SEQ_MSG );
				}else{
					WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_MG_SEQ_END );
				}
			}
		}
		break;
	
	// 歩き終わったらメッセージ表示
	case WFLBY_EV_DEF_PLAYERIN_MG_SEQ_MSG:
		{
			STRBUF* p_str;
			WFLBY_ROOM_MSG_SetMinigame( p_rmwk, p_save->minigame, 0 );
			p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msg_wifi_h_info_01_09 );
			WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
			WFLBY_EVENTWK_AddSeq( p_wk );
		}
		break;

	// 歩き終わったらメッセージ表示
	case WFLBY_EV_DEF_PLAYERIN_MG_SEQ_MSGEND:
		if( WFLBY_ROOM_TALKWIN_WaitEnd( p_rmwk ) == TRUE ){
			WFLBY_ROOM_TALKWIN_Off( p_rmwk );
			WFLBY_EVENTWK_AddSeq( p_wk );
		}
		break;
	
	// 入場完了
	case WFLBY_EV_DEF_PLAYERIN_MG_SEQ_END:
		// 基本動作の関数を設定
		WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_player, WFLBY_3DOBJCONT_MOVEPLAYER );

		{
			WFLBY_EVENT* p_event;
			u32 plno;
			p_event = WFLBY_ROOM_GetEvent( p_rmwk );
			plno	= WFLBY_ROOM_GetPlno( p_rmwk );
			// プレイヤーメイン常駐イベントを設定
			WFLBY_EVENT_SET_PRIVATE_EVENT( p_event, plno, &sc_WFLBY_EV_DEF_PLAYERMAIN, NULL );
		}
		break;
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	アンケートから戻ってくる
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EV_DEF_PlayerInAnketo_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	WFLBY_3DPERSON* p_player;
	WFLBY_3DOBJCONT* p_objcont;
	WFLBY_ROOM_SAVE* p_save;
	WFLBY_SYSTEM*	p_system;

	p_system	= WFLBY_ROOM_GetSystemData( p_rmwk );
	p_save		= WFLBY_ROOM_GetSaveData( p_rmwk );
	p_objcont	= WFLBY_ROOM_GetObjCont( p_rmwk );
	p_player	= WFLBY_3DOBJCONT_GetPlayer( p_objcont );


	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){
		
	// 主人公を登録
	case WFLBY_EV_DEF_PLAYERIN_ANKETO_SEQ_ADD:
		// 主人公の登録
		WFLBY_EV_DEF_StartObjPlayerEx( p_rmwk, plno,
				p_save->pl_gridx, p_save->pl_gridy,
				WF2DMAP_WAY_DOWN );


		// BGMをフェードイン
		WFLBY_SYSTEM_SetBGMVolumeDown( p_system, FALSE );

		WFLBY_EVENTWK_AddSeq( p_wk );
		break;
	// フェードイン待ち
	case WFLBY_EV_DEF_PLAYERIN_ANKETO_SEQ_FADEWAIT:

		// フェードイン待ち
		p_save = WFLBY_ROOM_GetSaveData( p_rmwk );
		if( WFLBY_ROOM_GetFadeFlag( p_rmwk ) == FALSE ){
			WFLBY_EVENTWK_AddSeq( p_wk );
			// 動作リクエスト
			WFLBY_3DOBJCONT_SetWkActCmd( p_objcont, p_player, WF2DMAP_CMD_WALK, p_save->pl_way );
		}
		break;

	// プレイヤー動作処理
	case WFLBY_EV_DEF_PLAYERIN_ANKETO_SEQ_PLMOVE:
		{
			u32 plst;
			plst = WFLBY_3DOBJCONT_GetWkObjData( p_player, WF2DMAP_OBJPM_ST );
			if( plst == WF2DMAP_OBJST_NONE ){

#if PL_T0867_080716_FIX
				// もう移動しないので、他人が会話ジャンプしてOK
				WFLBY_ROOM_SetPlayerEventAfterMove( p_rmwk, FALSE );
#endif

				// 通信エラーチェック	ON
				WFLBY_ROOM_SetErrorCheckFlag( p_rmwk, TRUE );

				if( WFLBY_SYSTEM_APLFLAG_GetForceEnd( p_system ) == TRUE ){
					WFLBY_EVENTWK_AddSeq( p_wk );
				}else{
					WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYERIN_ANKETO_SEQ_END );
				}
			}
		}
		break;

	// アプリケーション強制退室メッセージの表示
	case WFLBY_EV_DEF_PLAYERIN_ANKETO_SEQ_APLFORCEEND_MSG:
		{
			STRBUF* p_str;
			p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_survey_guide_a_05 );
			WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
			WFLBY_EVENTWK_AddSeq( p_wk );
		}
		break;
		
	// アプリケーション強制退室メッセージの表示完了待ち
	case WFLBY_EV_DEF_PLAYERIN_ANKETO_SEQ_APLFORCEEND_MSGWAIT:
		if( WFLBY_ROOM_TALKWIN_WaitEnd( p_rmwk ) == TRUE ){
			WFLBY_ROOM_TALKWIN_Off( p_rmwk );
			WFLBY_EVENTWK_AddSeq( p_wk );
		}
		break;
	
	// 入場完了
	case WFLBY_EV_DEF_PLAYERIN_ANKETO_SEQ_END:
		// 基本動作の関数を設定
		WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_player, WFLBY_3DOBJCONT_MOVEPLAYER );

		{
			WFLBY_EVENT* p_event;
			u32 plno;
			p_event = WFLBY_ROOM_GetEvent( p_rmwk );
			plno	= WFLBY_ROOM_GetPlno( p_rmwk );
			// プレイヤーメイン常駐イベントを設定
			WFLBY_EVENT_SET_PRIVATE_EVENT( p_event, plno, &sc_WFLBY_EV_DEF_PLAYERMAIN, NULL );
		}
		break;
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	プレイヤーメイン		処理
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EV_DEF_PlayerMain_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	WFLBY_EV_DEF_PLAYER_WK* p_plwk;

	// イベントワーク取得
	p_plwk = WFLBY_EVENTWK_GetWk( p_wk );


	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){
	// 初期化
	case WFLBY_EV_DEF_PLAYER_SEQ_INIT:
		WFLBY_EVENTWK_AllocWk( p_wk, sizeof(WFLBY_EV_DEF_PLAYER_WK) );
		WFLBY_EVENTWK_AddSeq( p_wk );
		break;
		
	// イベントチェック などなど
	case WFLBY_EV_DEF_PLAYER_SEQ_EVCHECK:
		WFLBY_ROOM_GadGet_SetStop( p_rmwk, FALSE );	// ボタン反応開始

		{
			u32 st;
			u32 way;
			WFLBY_3DPERSON* p_player;
			WF2DMAP_POS	pos, waypos;
			u32 event, wayevent;
			WFLBY_3DOBJCONT* p_objcont;
			WFLBY_3DMAPOBJ_CONT* p_mapobjcont;
			WFLBY_MAPCONT* p_mapcont;
			WFLBY_SYSTEM* p_system;
			WFLBY_EVENT*	p_event;
			WFLBY_3DPERSON* p_pierrot;


			p_event		= WFLBY_ROOM_GetEvent( p_rmwk );
			p_system	= WFLBY_ROOM_GetSystemData( p_rmwk );
			p_objcont	= WFLBY_ROOM_GetObjCont( p_rmwk );
			p_mapcont	= WFLBY_ROOM_GetMapCont( p_rmwk );
			p_mapobjcont= WFLBY_ROOM_GetMapObjCont( p_rmwk );	


			p_pierrot		= WFLBY_3DOBJCONT_GetPierrot( p_objcont );
			if( p_pierrot != NULL ){

				// ここで、ピエロを操作する
				// ピエロが飛び跳ねたりするのを人物操作処理の外部でやってしまうと、
				// 人物と離している最中にピエロがジャンピしだす可能性がある
				// ネオンイベント～PARADE終了まで飛び跳ねる
				if( (WFLBY_SYSTEM_Event_GetEndCM( p_system ) == FALSE) &&
					(WFLBY_SYSTEM_Event_GetNeon( p_system ) == TRUE) &&
					(WFLBY_SYSTEM_FLAG_GetAnketoView( p_system ) == FALSE) ){

					// とびはね動作
					if( WFLBY_3DOBJCONT_GetWkMove( p_pierrot ) != WFLBY_3DOBJCONT_MOVEJUMP ){
						WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_pierrot, WFLBY_3DOBJCONT_MOVEJUMP );
					}
				}else{

					// とまる動作
					if( WFLBY_3DOBJCONT_GetWkMove( p_pierrot ) != WFLBY_3DOBJCONT_MOVENONE ){
						WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_pierrot, WFLBY_3DOBJCONT_MOVENONE );
					}
				}
			}


			// プレイヤーを取得
			p_player = WFLBY_3DOBJCONT_GetPlayer( p_objcont );
			
			// プレイヤーステータスがNONEのときにイベントを発生チェックを行う
			st		= WFLBY_3DOBJCONT_GetWkObjData( p_player, WF2DMAP_OBJPM_ST );
			way		= WFLBY_3DOBJCONT_GetWkObjData( p_player, WF2DMAP_OBJPM_WAY );

			// 座標取得
			pos		= WFLBY_3DOBJCONT_GetWkPos( p_player );
			waypos	= WF2DMAP_OBJToolWayPosGet( pos, way );

			// イベント取得
			event		= WFLBY_MAPCONT_EventGet( p_mapcont, WF2DMAP_POS2GRID(pos.x), WF2DMAP_POS2GRID(pos.y) );
			wayevent	= WFLBY_MAPCONT_EventGet( p_mapcont, WF2DMAP_POS2GRID(waypos.x), WF2DMAP_POS2GRID(waypos.y) );

			// もしNPCの状態を表示しているときは、
			// NPCがINFODRAW_BUSY状態になったら表示をOFFにする
			{
				u32 npcid;
				npcid = WFLBY_ROOM_UNDERWIN_TrCardGetPlIdx( p_rmwk );
				if( npcid != plno ){
					// 状態描画ビジーになったらOFF
					if( WFBLY_ROOM_CheckNpcInfoDrawBusy( p_rmwk, npcid ) ){
						WFLBY_ROOM_UNDERWIN_TrCardOff( p_rmwk );
					}
				}
			}

			// 状態がNONEならイベントチェックなど
			// NONE以外ならトレーナーカードOFFチェック
			if( st != WF2DMAP_OBJST_NONE ){

				if( WFLBY_ROOM_UNDERWIN_TrCardCheckCanOff( p_rmwk ) == TRUE ){
					WFLBY_3DPERSON* p_npc;
					u32 npc_id;
					// もし歩き、振り向き、走り状態で、
					// trainerカードを出していたらボタンに戻す
					if( (st == WF2DMAP_OBJST_WALK) ||
						(st == WF2DMAP_OBJST_RUN) ||
						(st == WF2DMAP_OBJST_TURN) ){

						npc_id = WFLBY_ROOM_UNDERWIN_TrCardGetPlIdx( p_rmwk );
						if( npc_id != plno ){
							p_npc = WFLBY_3DOBJCONT_GetPlIDWk( p_objcont, npc_id );

							// 動作停止状態なら設定
							if( p_npc ){
								if( WFLBY_3DOBJCONT_GetWkMove( p_npc ) == WFLBY_3DOBJCONT_MOVENONE ){
									WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_npc, p_plwk->infodraw_retmvtype );	// NPC動作再開
								}
							}
						}
						WFLBY_ROOM_UNDERWIN_TrCardOff( p_rmwk );
					}
				}
				
			}else{

#if PL_T0867_080716_FIX
				// してないので、他人が会話ジャンプしてOK
				WFLBY_ROOM_SetPlayerEventAfterMove( p_rmwk, FALSE );
#endif

				// タッチトイ反応あり
				WFLBY_ROOM_GadGet_SetStop( p_rmwk, FALSE );

				// イベントに乗ったかチェック
				if( WFLBY_MAPCONT_Event_CheckMount( event ) == TRUE ){


					// そのイベント発動
					WFLBY_EV_DEF_Player_StartEventTbl( p_wk, p_rmwk, plno, 
							sc_WFLBY_EV_DEF_EVENT_MOUNTSTART, event );
					// イベント発動したら動作を一時停止
					WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_player, WFLBY_3DOBJCONT_MOVENONE );

					// 起動したイベントを設定
					WFLBY_ROOM_SetEvNo( p_rmwk, event );

					WFLBY_ROOM_GadGet_SetStop( p_rmwk, TRUE );	// ボタン反応なし

#if PL_T0867_080716_FIX
					// 乗ったときのイベントはすべてオート動作を行う可能性がある。
					// 話しかけられても他人は会話ジャンプしない
					WFLBY_ROOM_SetPlayerEventAfterMove( p_rmwk, TRUE );
#endif

					return FALSE;	
				}


				// ガジェットの実行
				if( WFLBY_ROOM_GadGet_GetFlag( p_rmwk ) == TRUE ){
					// 人物動作停止
					WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_player, WFLBY_3DOBJCONT_MOVENONE );
					
					// 割り込みイベント発生
					WFLBY_EVENT_SET_PRIVATE_INTR_EVENT( p_event, plno, &sc_WFLBY_EV_DEF_ALL_GADGET, NULL );
					WFLBY_ROOM_GadGet_SetStop( p_rmwk, TRUE );	// ボタン反応なし
					return FALSE;
				}


				// 話しかけられたか
				if( WFLBY_SYSTEM_CheckTalkReq( p_system ) ){
					WFLBY_EVENT_SET_PRIVATE_INTR_EVENT( p_event, plno, &sc_WFLBY_EV_DEF_TALK_B, NULL );
					// イベント発動したら動作を一時停止
					WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_player, WFLBY_3DOBJCONT_MOVENONE );
					WFLBY_ROOM_GadGet_SetStop( p_rmwk, TRUE );	// ボタン反応なし
					return FALSE;
				}


				// 選択関係
				if( sys.trg & PAD_BUTTON_DECIDE ){
					// 目の前に誰かいるかチェック
					// いたらその人と話す
					{
						WFLBY_3DPERSON* p_npc;
						u32 status;
						BOOL talk_busy;
						u32 npc_id;
						
						p_npc		= WFLBY_3DOBJCONT_GetFrontPerson( p_objcont, p_player );

						// 会話へ
						if( p_npc ){

							// すべてを始める前にその人のローカル状態が今話しかけてもよい常態かチェック
							status		= WFLBY_3DOBJCONT_GetWkObjData( p_npc, WF2DMAP_OBJPM_ST );
							npc_id		= WFLBY_3DOBJCONT_GetWkObjData( p_npc, WF2DMAP_OBJPM_PLID );

							if( npc_id != WFLBY_MAPPERSON_PIERROT_ID ){
								talk_busy	= WFBLY_ROOM_CheckNpcTalkBusy( p_rmwk, npc_id );
								// 状態がNONEじゃないと話しかけられない
								if( (status == WF2DMAP_OBJST_NONE) && (talk_busy == FALSE) ){	
									WFLBY_EVENT_SET_PRIVATE_INTR_EVENT( p_event, plno, 
											&sc_WFLBY_EV_DEF_TALK_A, NULL );
									// イベント発動したら動作を一時停止
									WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_player, WFLBY_3DOBJCONT_MOVENONE );
									WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_npc, WFLBY_3DOBJCONT_MOVENONE );

									WFLBY_ROOM_GadGet_SetStop( p_rmwk, TRUE );	// ボタン反応なし
									return FALSE;
								}
							}
						}
					}

					// 銅像チェック
					if( WFLBY_MAPCONT_Event_CheckFrontDouzou( wayevent ) == TRUE ){
						u32 way;
						// 自分が上を向いている必要がある
						way = WFLBY_3DOBJCONT_GetWkObjData( p_player, WF2DMAP_OBJPM_WAY );
						if( way == WF2DMAP_WAY_UP ){
						
							// 銅像を動かす
							WFLBY_3DMAPOBJCONT_MAP_StartDouzouAnm( p_mapobjcont, 
									WF2DMAP_POS2GRID(waypos.x),
									WF2DMAP_POS2GRID(waypos.y) );

							// 起動したイベントを設定
							WFLBY_ROOM_SetEvNo( p_rmwk, wayevent );

							WFLBY_ROOM_GadGet_SetStop( p_rmwk, TRUE );	// ボタン反応なし

							return FALSE;
						}
					}

					// 目の前にイベントがあるかチェック
					if( WFLBY_MAPCONT_Event_CheckFrontDecide( wayevent ) == TRUE ){

						// そのイベント発動
						WFLBY_EV_DEF_Player_StartEventTbl( p_wk, p_rmwk, plno, 
								sc_WFLBY_EV_DEF_EVENT_ASTART, wayevent );
						// イベント発動したら動作を一時停止
						WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_player, WFLBY_3DOBJCONT_MOVENONE );

						// 起動したイベントを設定
						WFLBY_ROOM_SetEvNo( p_rmwk, wayevent );

						WFLBY_ROOM_GadGet_SetStop( p_rmwk, TRUE );	// ボタン反応なし

						// そのイベント起動
						return FALSE;	
					}
				}

				// 向いている方向にキーを押しているか
				if( (WFLBY_EV_DEF_WayKeyInput( way ) == TRUE) ){
					
					// さらに向いている方向が上である必要がある
					if( ( way == WF2DMAP_WAY_UP ) ){ 
						if( WFLBY_MAPCONT_Event_CheckFrontKey( wayevent ) == TRUE ){

							// そのイベント発動
							WFLBY_EV_DEF_Player_StartEventTbl( p_wk, p_rmwk, plno, 
									sc_WFLBY_EV_DEF_EVENT_WAYUPSTART, wayevent );
							
							// イベント発動したら動作を一時停止
							WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_player, WFLBY_3DOBJCONT_MOVENONE );


							// 起動したイベントを設定
							WFLBY_ROOM_SetEvNo( p_rmwk, wayevent );

							WFLBY_ROOM_GadGet_SetStop( p_rmwk, TRUE );	// ボタン反応なし

							//  そのイベントを起動
							return FALSE;	
						}
					}

					// その方向が人物じゃないかチェック
					{
						WFLBY_3DPERSON* p_npc;
						u32 npc_id;
						u32 chara_id;
						
						p_npc		= WFLBY_3DOBJCONT_GetFrontPerson( p_objcont, p_player );

						// プロフィールの表示へ
						if( p_npc ){

							// それが、ピエロか他人かチェック
							npc_id		= WFLBY_3DOBJCONT_GetWkObjData( p_npc, WF2DMAP_OBJPM_PLID );
							if( npc_id != WFLBY_MAPPERSON_PIERROT_ID ){

								// 表示中ではないかチェック
								if( WFLBY_ROOM_UNDERWIN_TrCardGetDraw( p_rmwk ) == FALSE ){
									if( WFBLY_ROOM_CheckNpcInfoDrawBusy( p_rmwk, npc_id ) == FALSE ){
										WFLBY_ROOM_UNDERWIN_TrCardOn( p_rmwk, npc_id, TRUE );

										// InfoDrawがビジー状態ではない＝NPCの動作をプレイヤー側が操作しても大丈夫
										//
										// 主人公は動作するか、会話がこない限り違うイベントには飛ばない、
										// 会話のときはNPC情報は書き換わるので、
										// infodraw_retmvtypeの値は失われても大丈夫

										// その人の動作を保存しておく
										p_plwk->infodraw_retmvtype = WFLBY_3DOBJCONT_GetWkMove( p_npc );
										// 動作停止
										WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_npc, WFLBY_3DOBJCONT_MOVENONE );	

										return FALSE;
									}
								}
							}
							
						}
					}
				}


				// Xボタンを押したら自分のプロフィールを表示
				if( sys.trg & PAD_BUTTON_X ){
					// スイッチ表示
					if( WFLBY_ROOM_UNDERWIN_TrCardGetDraw( p_rmwk ) == FALSE ){
						WFLBY_ROOM_UNDERWIN_TrCardOn( p_rmwk, WFLBY_SYSTEM_GetMyIdx( p_system ), FALSE );
						Snd_SePlay( WFLBY_SND_PROFILE );
					}else{
						u32 trcard_plidx;
						// 自分かチェック
						trcard_plidx = WFLBY_ROOM_UNDERWIN_TrCardGetPlIdx( p_rmwk );
						if( trcard_plidx == plno ){
							WFLBY_ROOM_UNDERWIN_TrCardOff( p_rmwk );
							Snd_SePlay( WFLBY_SND_PROFILE );
						}
					}
					return FALSE;
				}
			}
		}
		break;

	}

	
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	主人公動作リクエスト設定後
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EV_DEF_PlayerMain_After( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	u32 st;
	WFLBY_3DPERSON* p_player;
	WFLBY_3DOBJCONT* p_objcont;

	// プレイヤーを取得
	p_objcont	= WFLBY_ROOM_GetObjCont( p_rmwk );
	p_player = WFLBY_3DOBJCONT_GetPlayer( p_objcont );
	
	// プレイヤーステータスがNONE意外になったらタッチトイ反応をOFF
	st		= WFLBY_3DOBJCONT_GetWkObjData( p_player, WF2DMAP_OBJPM_ST );

	if( st != WF2DMAP_OBJST_NONE ){
		// タッチトイ反応なし
		WFLBY_ROOM_GadGet_SetStop( p_rmwk, TRUE );

#if PL_T0867_080716_FIX
		// 移動中なので、他人が会話ジャンプしてNG
		WFLBY_ROOM_SetPlayerEventAfterMove( p_rmwk, TRUE );
#endif
	}

	return FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	出口から退場
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EV_DEF_PlayerOutNormal_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{

	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){
	// 退室チェックメッセージ開始
	case WFLBY_EV_DEF_PLAYER_OUTNORMAL_SEQ_MSGSTART:			
		// 動作停止
		{
			WFLBY_3DOBJCONT* p_objcont;
			WFLBY_3DPERSON* p_person;

			p_objcont	= WFLBY_ROOM_GetObjCont( p_rmwk );
			p_person	= WFLBY_3DOBJCONT_GetPlayer( p_objcont );
			WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_person, WFLBY_3DOBJCONT_MOVENONE );
			
		}

		// 状態変更
		// はなしかけられないように変更
		WFLBY_SYSTEM_SetMyStatus( WFLBY_ROOM_GetSystemData(p_rmwk), WFLBY_STATUS_LOGOUT );

		Snd_SePlay( WFLBY_SND_MINIGAME );

		{
			STRBUF* p_str;

			p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_wifi_hiroba_01_03 );
			WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

			WFLBY_EVENTWK_AddSeq( p_wk );
		}
		break;

	// 退室チェックメッセージ待ち
	case WFLBY_EV_DEF_PLAYER_OUTNORMAL_SEQ_MSGWAIT:	
		if( WFLBY_ROOM_TALKWIN_WaitEnd( p_rmwk ) == TRUE ){
			WFLBY_EVENTWK_AddSeq( p_wk );
		}
		break;

	// 退室チェックメッセージ開始
	case WFLBY_EV_DEF_PLAYER_OUTNORMAL_SEQ_MSGSTART01:		
		{
			STRBUF* p_str;

			p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_ERR, dwc_message_0010 );
			WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

			WFLBY_EVENTWK_AddSeq( p_wk );
		}
		break;
	
	// 退室チェックメッセージ待ち
	case WFLBY_EV_DEF_PLAYER_OUTNORMAL_SEQ_MSGWAIT01:		
		if( WFLBY_ROOM_TALKWIN_WaitEnd( p_rmwk ) == TRUE ){
			WFLBY_EVENTWK_AddSeq( p_wk );
		}
		break;
		
	// YESNOウィンドウ表示
	case WFLBY_EV_DEF_PLAYER_OUTNORMAL_SEQ_YESNO:			
		WFLBY_ROOM_YESNOWIN_StartNo( p_rmwk );
		WFLBY_EVENTWK_AddSeq( p_wk );
		break;
		
	// YESNO選択待ち
	case WFLBY_EV_DEF_PLAYER_OUTNORMAL_SEQ_YESNOWAIT:		
		{
			u32 result;
			BOOL list_end = FALSE;

			result = WFLBY_ROOM_YESNOWIN_Main( p_rmwk );
			switch( result ){
			case WFLBY_ROOM_YESNO_OK:	// はい
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_OUTNORMAL_SEQ_FLYUP );
				list_end = TRUE;
				break;
				
			case WFLBY_ROOM_YESNO_NO:	// いいえ
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_OUTNORMAL_SEQ_RET );
				list_end = TRUE;
				break;
				
			default:
			case WFLBY_ROOM_YESNO_WAIT:	// 選び中
				break;
			}

			if( list_end == TRUE ){
				WFLBY_ROOM_YESNOWIN_End( p_rmwk );
				WFLBY_ROOM_TALKWIN_Off( p_rmwk );
			}
		}
		break;

	// 退室上る
	case WFLBY_EV_DEF_PLAYER_OUTNORMAL_SEQ_FLYUP:			

		// 通信エラーチェック	OFF
		WFLBY_ROOM_SetErrorCheckFlag( p_rmwk, FALSE );
		
		{
			WFLBY_CAMERA* p_camera;
			WFLBY_3DOBJCONT* p_objcont;
			WFLBY_3DPERSON* p_person;

			p_objcont	= WFLBY_ROOM_GetObjCont( p_rmwk );
			p_person	= WFLBY_3DOBJCONT_GetPlayer( p_objcont );

			// カメラから関連付けをはぐ
			p_camera = WFLBY_ROOM_GetCamera( p_rmwk );
			WFLBY_CAMERA_ResetTargetPerson( p_camera );

			// 上に跳ぶ
			WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_person, WFLBY_3DOBJCONT_MOVEFLYUP );
		}

		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_OUTNORMAL_SEQ_END );
		break;

	// 退室
	case WFLBY_EV_DEF_PLAYER_OUTNORMAL_SEQ_END:				
		{
			WFLBY_3DOBJCONT* p_objcont;
			WFLBY_3DPERSON* p_person;

			p_objcont	= WFLBY_ROOM_GetObjCont( p_rmwk );
			p_person	= WFLBY_3DOBJCONT_GetPlayer( p_objcont );

			if( WFLBY_3DOBJCONT_CheckWkMoveEnd( p_person ) == TRUE ){
				WFLBY_ROOM_SetEndFlag( p_rmwk, TRUE );
				WFLBY_ROOM_SetEndRet( p_rmwk, WFLBY_ROOM_RET_ROOM_OUT );

				// 状態変更
				WFLBY_SYSTEM_SetMyStatus( WFLBY_ROOM_GetSystemData(p_rmwk), WFLBY_STATUS_LOGOUT );

				return TRUE;	// 終了
			}
		}
		break;
		
	// 戻る
	case WFLBY_EV_DEF_PLAYER_OUTNORMAL_SEQ_RET:
		// 上に歩かせる
		{
			WFLBY_3DOBJCONT* p_objcont;
			WFLBY_3DPERSON* p_person;

			p_objcont	= WFLBY_ROOM_GetObjCont( p_rmwk );
			p_person	= WFLBY_3DOBJCONT_GetPlayer( p_objcont );
			WFLBY_3DOBJCONT_SetWkActCmd( p_objcont, p_person, WF2DMAP_CMD_WALK, WF2DMAP_WAY_UP );
			WFLBY_EVENTWK_AddSeq( p_wk );
		}
		break;

	// 戻り完了まち
	case WFLBY_EV_DEF_PLAYER_OUTNORMAL_SEQ_RETWAIT:			
		// 動作終了を待って、通常イベントに戻る
		{
			WFLBY_3DOBJCONT* p_objcont;
			WFLBY_3DPERSON* p_person;
			WFLBY_EVENT* p_event;
			u32 status;

			// 状態変更
			// 元に戻す
			WFLBY_SYSTEM_SetMyStatus( WFLBY_ROOM_GetSystemData(p_rmwk), WFLBY_STATUS_LOGIN );

			p_objcont	= WFLBY_ROOM_GetObjCont( p_rmwk );
			p_event		= WFLBY_ROOM_GetEvent( p_rmwk );
			p_person	= WFLBY_3DOBJCONT_GetPlayer( p_objcont );
			status		= WFLBY_3DOBJCONT_GetWkObjData( p_person, WF2DMAP_OBJPM_ST );
			if( status == WF2DMAP_OBJST_NONE ){
				WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_person, WFLBY_3DOBJCONT_MOVEPLAYER );
				// イベント設定
				WFLBY_EVENT_SET_PRIVATE_EVENT( p_event, plno, &sc_WFLBY_EV_DEF_PLAYERMAIN, NULL );
				
			}
		}
		break;
	}

	return FALSE;	
}

//----------------------------------------------------------------------------
/**
 *	@brief	世界時計　ロビーニュース　足跡BOARDへ
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EV_DEF_PlayerOutSubChan_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	WFLBY_3DOBJCONT*	p_objcont;
	WFLBY_3DPERSON*		p_player;
	WFLBY_SYSTEM*		p_system;
	u32 evno;
	WFLBY_EV_DEF_PLAYER_SUBCHAN_WK* p_evwk;

	p_objcont	= WFLBY_ROOM_GetObjCont( p_rmwk );
	p_player	= WFLBY_3DOBJCONT_GetPlIDWk( p_objcont, plno );
	evno		= WFLBY_ROOM_GetEvNo( p_rmwk );
	p_evwk		= WFLBY_EVENTWK_GetWk( p_wk );
	p_system	= WFLBY_ROOM_GetSystemData( p_rmwk );

	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){
	// サブチャンネルへログイン
	case WFLBY_EV_DEF_PLAYER_SUBCHAN_SEQ_LOGIN:

		p_evwk = WFLBY_EVENTWK_AllocWk( p_wk, sizeof(WFLBY_EV_DEF_PLAYER_SUBCHAN_WK) );

		{
			u8 channelno;

			// チャンネルナンバーからミニゲームタイプを作成
			channelno = WFLBY_EV_DEF_PlayerOutSubChan_GetEvnoChannel( evno );
			switch( channelno ){
			case DWC_LOBBY_SUBCHAN_FOOT1:			// 足跡ボード１
				p_evwk->minigame = WFLBY_GAME_FOOTWHITE;
				break;
			case DWC_LOBBY_SUBCHAN_FOOT2:			// 足跡ボード２
				p_evwk->minigame = WFLBY_GAME_FOOTBLACK;
				break;
			case DWC_LOBBY_SUBCHAN_CLOCK:			// 世界時計
				p_evwk->minigame = WFLBY_GAME_WLDTIMER;
				break;
			case DWC_LOBBY_SUBCHAN_NEWS:				// ロビーニュース
				p_evwk->minigame = WFLBY_GAME_NEWS;
				break;
			default:
				GF_ASSERT(0);
				p_evwk->minigame = WFLBY_GAME_WLDTIMER;
				break;
			}
		}

		// もう入れない状態じゃないかチェック
		if( WFLBY_SYSTEM_Event_GetEndCM( p_system ) == TRUE ){
			p_evwk->msg_idx		= msg_wifi_h_info_05_03;
			p_evwk->msg_type	= WFLBY_DEFMSG_TYPE_INFO;
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_SUBCHAN_SEQ_CLOSE );
			break;
		}

		// 通信過剰状態じゃないかチェック
		if( WFLBY_SYSTEM_GetExcessFoodError( p_system ) == TRUE ){
			p_evwk->msg_idx		= msg_hiroba_end_03;
			p_evwk->msg_type	= WFLBY_DEFMSG_TYPE_HIROBA;
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_SUBCHAN_SEQ_CLOSE );
			break;
		}


		{
			BOOL result;
			u8 channel_no;

			channel_no = WFLBY_EV_DEF_PlayerOutSubChan_GetEvnoChannel( evno );

			// サブチャンネル接続人数チェック
			if( DWC_LOBBY_SUBCHAN_LoginCheck( channel_no ) == FALSE ){
				p_evwk->msg_idx = msg_wifi_h_info_05_01;
				p_evwk->msg_type	= WFLBY_DEFMSG_TYPE_INFO;
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_SUBCHAN_SEQ_NG );
				break;
			}

			
			result = DWC_LOBBY_SUBCHAN_Login( channel_no );
			GF_ASSERT( result );
			WFLBY_EVENTWK_AddSeq( p_wk );
		}
		break;
		
	//  ログイン待機
	case WFLBY_EV_DEF_PLAYER_SUBCHAN_SEQ_LOGINWAIT:
		{
			DWC_LOBBY_SUBCHAN_LOGIN_RESULT result;

			result = DWC_LOBBY_SUBCHAN_LoginWait();
			if( result == DWC_LOBBY_SUBCHAN_LOGIN_OK ){
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_SUBCHAN_SEQ_START );

				// ミニゲーム参加トピック生成
				{
					u8 subchan_num;
					u8 minigame;
					int i;
					u8 pl_no[ NEWS_TOPICNAME_NUM ] = {0};
					u32 enumplno;
					u8 count;
					WFLBY_SYSTEM*		p_system;

					// システムワーク取得
					p_system = WFLBY_ROOM_GetSystemData( p_rmwk );

					// サブチャンネルに参加している人の数
					subchan_num = WFLBY_SYSTEM_GetSubChanPlNum( p_system );


					if( (p_evwk->minigame == WFLBY_GAME_WLDTIMER) ||
						(p_evwk->minigame == WFLBY_GAME_NEWS) ){
						// 参加者リスト作成
						count = 0;
						for( i=0; i<NEWS_TOPICNAME_NUM; i++ ){
							enumplno = WFLBY_SYSTEM_GetSubChanEnum( p_system, i );
							if( enumplno != DWC_LOBBY_USERIDTBL_IDX_NONE ){
								if( count < NEWS_TOPICNAME_NUM ){
									pl_no[ count ] = enumplno;
								}
								count ++;
							}
						}
						OS_TPrintf( "check subchan_num=%d  in_num=%d\n", subchan_num, count );
					}else{
						pl_no[0] = plno;
					}

					WFLBY_SYSTEM_TOPIC_SendMiniGame( p_system, p_evwk->minigame, subchan_num, 
							pl_no[0], pl_no[1], pl_no[2], pl_no[3], FALSE );
				}
				
			// ログイン失敗
			}else if( result == DWC_LOBBY_SUBCHAN_LOGIN_NG ){
				p_evwk->msg_idx = msg_wifi_h_info_05_01;
				p_evwk->msg_type	= WFLBY_DEFMSG_TYPE_INFO;
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_SUBCHAN_SEQ_NG );
			}
		}
		break;
		
	// アプリ開始
	case WFLBY_EV_DEF_PLAYER_SUBCHAN_SEQ_START:
		{
			u32 endret;
			u32 status;
			u32 played_type;

			endret = WFLBY_EV_DEF_PlayerOutSubChan_GetEvnoRetNum( evno );
			status = WFLBY_EV_DEF_PlayerOutSubChan_GetEvnoStatus( evno );
			
			WFLBY_ROOM_SetEndFlag( p_rmwk, TRUE );
			WFLBY_ROOM_SetEndRet( p_rmwk, endret );

			// 状態変更
			WFLBY_SYSTEM_SetMyStatus( WFLBY_ROOM_GetSystemData(p_rmwk), status );

			// ボウケンノート
			WFLBY_SYSTEM_FNOTE_SetPlayMinigame( p_system, p_evwk->minigame );

			// 遊んだ情報バッファ
			switch( p_evwk->minigame ){	
			case WFLBY_GAME_FOOTWHITE:	// 足跡ボード　白
			case WFLBY_GAME_FOOTBLACK:	// 足跡ボード　黒
				played_type = WFLBY_PLAYED_FOOTBOARD;
				break;
				
			case WFLBY_GAME_WLDTIMER:	// 世界時計
				played_type = WFLBY_PLAYED_WORLDTIMER;
				break;
				
			case WFLBY_GAME_NEWS:		// ロビーニュース
				played_type = WFLBY_PLAYED_TOPIC;
				break;
			default:
				GF_ASSERT(0);	// ありえない
				break;
			}
			WFLBY_SYSTEM_PLAYED_SetPlay( p_system, played_type );

			// 出現パターン設定
			{
				WF2DMAP_POS	pos;
				WFLBY_ROOM_SAVE* p_save;
				WF2DMAP_WAY	way;	

				// プレイヤーの向いている方向の逆方向に出す
				way = WFLBY_3DOBJCONT_GetWkObjData( p_player, WF2DMAP_OBJPM_WAY );
				way = WF2DMPA_OBJToolRetWayGet( way );

				p_save		= WFLBY_ROOM_GetSaveData( p_rmwk );

				// 今の位置の１歩下の位置に出す
				pos = WFLBY_3DOBJCONT_GetWkPos( p_player );

				p_save->pl_inevno	= WFLBY_EV_DEF_PLAYERIN_GRID;
				p_save->pl_gridx	= WF2DMAP_POS2GRID( pos.x );
				p_save->pl_gridy	= WF2DMAP_POS2GRID( pos.y );
				p_save->pl_way		= way;
				p_save->minigame	= p_evwk->minigame; 
			}



			// ワーク破棄
			WFLBY_EVENTWK_DeleteWk( p_wk );

			return TRUE;	// 終了
		}
		break;
		
	// サブチャンネルに入れなかったとき,はじかれる
	case WFLBY_EV_DEF_PLAYER_SUBCHAN_SEQ_NG:
		{
			u8 moveno;
			u8 way;
			

			// はじかれ動作を入れる
			// プレイヤーの向きから方向を求める
			way = WFLBY_3DOBJCONT_GetWkObjData( p_player, WF2DMAP_OBJPM_WAY );
			switch( way ){
			case WF2DMAP_WAY_UP:
				moveno = WFLBY_3DOBJCONT_MOVEROTADOWN;
				break;
			case WF2DMAP_WAY_DOWN:
				moveno = WFLBY_3DOBJCONT_MOVEROTAUP;
				break;
			case WF2DMAP_WAY_LEFT:
				moveno = WFLBY_3DOBJCONT_MOVEROTARIGHT;
				break;

			case WF2DMAP_WAY_RIGHT:
				moveno = WFLBY_3DOBJCONT_MOVEROTALEFT;
				break;

			default:
				GF_ASSERT(0);
				break;
			}
			WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_player, moveno );
			
			// 動作完了待ちへ
			WFLBY_EVENTWK_AddSeq( p_wk );
		}
		break;

	// はじかれ動作待ち
	case WFLBY_EV_DEF_PLAYER_SUBCHAN_SEQ_NGWAIT:
		{
			BOOL				result;

			result = WFLBY_3DOBJCONT_CheckWkMoveEnd( p_player );
			if( result == TRUE ){

				// メッセージ表示までのウエイト
				p_evwk->wait = WFLBY_EV_DEF_PLAYER_SUBCHAN_MSG_WAIT;

				// メッセージ表示へ
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_SUBCHAN_SEQ_NGMSG );
			}
		}
		break;

	// 部屋が閉じたとき
	case WFLBY_EV_DEF_PLAYER_SUBCHAN_SEQ_CLOSE:
		{
			u8 way;
			

			// はじかれ動作を入れる
			// プレイヤーの向きから方向を求める
			way = WFLBY_3DOBJCONT_GetWkObjData( p_player, WF2DMAP_OBJPM_WAY );
			way = WF2DMPA_OBJToolRetWayGet( way );
			WFLBY_3DOBJCONT_SetWkActCmd( p_objcont, p_player, WF2DMAP_CMD_WALK, way );

			// 動作完了待ちへ
			WFLBY_EVENTWK_AddSeq( p_wk );
		}
		break;
		
	// 部屋が閉じたとき
	case WFLBY_EV_DEF_PLAYER_SUBCHAN_SEQ_CLOSEWAIT:
		{
			u32 st;

			st = WFLBY_3DOBJCONT_GetWkObjData( p_player, WF2DMAP_OBJPM_ST );
			if( st == WF2DMAP_OBJST_NONE ){
				// メッセージ表示までのウエイト
				p_evwk->wait = WFLBY_EV_DEF_PLAYER_SUBCHAN_MSG_WAIT;

				// メッセージ表示へ
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_SUBCHAN_SEQ_NGMSG );
			}
		}
		break;

	// メッセージ表示
	case WFLBY_EV_DEF_PLAYER_SUBCHAN_SEQ_NGMSG:
		p_evwk->wait --;
		if( p_evwk->wait <= 0 ){	
			STRBUF*				p_str;			
			// メッセージの表示
			WFLBY_ROOM_MSG_SetMinigame( p_rmwk, p_evwk->minigame, 0 );
			p_str = WFLBY_ROOM_MSG_Get( p_rmwk, p_evwk->msg_type, p_evwk->msg_idx );
			WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );
			// メッセージ完了待ち
			WFLBY_EVENTWK_AddSeq( p_wk );

#if PL_T0867_080716_FIX
			// もうオート移動しないので、他人が会話ジャンプしてOK
			WFLBY_ROOM_SetPlayerEventAfterMove( p_rmwk, FALSE );
#endif
		}
		break;
	
	// メッセージ終了待ち
	case WFLBY_EV_DEF_PLAYER_SUBCHAN_SEQ_NGMSGWAIT:
		{
			BOOL result;

			result = WFLBY_ROOM_TALKWIN_WaitEnd( p_rmwk );
			if( result ){

				// ワーク破棄
				WFLBY_EVENTWK_DeleteWk( p_wk );
				
				// もとのプレイヤーイベントに戻す
				// 基本動作の関数を設定
				WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_player, WFLBY_3DOBJCONT_MOVEPLAYER );

				// メッセージOFF
				WFLBY_ROOM_TALKWIN_Off( p_rmwk );

				// プレイヤーメイン常駐イベントを設定
				{
					WFLBY_EVENT* p_event;
					p_event = WFLBY_ROOM_GetEvent( p_rmwk );
					WFLBY_EVENT_SET_PRIVATE_EVENT( p_event, plno, &sc_WFLBY_EV_DEF_PLAYERMAIN, NULL );
				}
			}
		}
		break;
	}
	
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ミニゲームに行く
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EV_DEF_PlayerOutMiniGame_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	WFLBY_EV_DEF_PLAYER_MG_WK* p_evwk;
	WFLBY_EVENT* p_event;
	WFLBY_3DOBJCONT* p_objcont;
	WFLBY_3DPERSON* p_player;
	WFLBY_SYSTEM* p_system;

	p_evwk = WFLBY_EVENTWK_GetWk( p_wk );

	// システムワーク取得
	p_event		= WFLBY_ROOM_GetEvent( p_rmwk );
	p_objcont	= WFLBY_ROOM_GetObjCont( p_rmwk );
	p_player	= WFLBY_3DOBJCONT_GetPlayer( p_objcont );
	p_system	= WFLBY_ROOM_GetSystemData( p_rmwk );

	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){
	case WFLBY_EV_DEF_PLAYER_MG_SEQ_INTR_START:
		p_evwk = WFLBY_EVENTWK_AllocWk( p_wk, sizeof(WFLBY_EV_DEF_PLAYER_MG_WK) );

#ifdef WFLBY_DEBUG_ROOM_MINIGAME_AUTO
		WFLBY_DEBUG_ROOM_MINIGAME_AUTO_FLAG = TRUE;
#endif

		// 主人公動作停止
		WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_player, WFLBY_3DOBJCONT_MOVENONE );

		// シーケンスを進めておく
		WFLBY_EVENTWK_AddSeq( p_wk );

		// 割り込みイベント発動
		p_evwk->mg_param.in_ok		= WFLBY_EV_MG_RET_NG_NUM;
		{
			u32 evno;
			
			evno = WFLBY_ROOM_GetEvNo( p_rmwk );

			switch( evno ){
			case WFLBY_MAPEVID_EV_BALLSLOW:
				p_evwk->mg_param.mg_type		= DWC_LOBBY_MG_BALLSLOW;
				p_evwk->mg_param.wflby_mg_type	= WFLBY_GAME_BALLSLOW;
				p_evwk->mg_param.wflby_mg_status= WFLBY_STATUS_BALLSLOW;
				break;
			case WFLBY_MAPEVID_EV_BALANCE:
				p_evwk->mg_param.mg_type	= DWC_LOBBY_MG_BALANCEBALL;
				p_evwk->mg_param.wflby_mg_type	= WFLBY_GAME_BALANCEBALL;
				p_evwk->mg_param.wflby_mg_status= WFLBY_STATUS_BALANCEBALL;
				break;
			case WFLBY_MAPEVID_EV_BALLOON:
				p_evwk->mg_param.mg_type	= DWC_LOBBY_MG_BALLOON;
				p_evwk->mg_param.wflby_mg_type	= WFLBY_GAME_BALLOON;
				p_evwk->mg_param.wflby_mg_status= WFLBY_STATUS_BALLOON;
				break;
			default:
				GF_ASSERT(0);
				break;
			}

			// マッチング中に状態変更
			WFLBY_SYSTEM_SetMyStatus( WFLBY_ROOM_GetSystemData(p_rmwk), WFLBY_STATUS_MATCHING );
		}
		
		WFLBY_EVENT_SET_PRIVATE_INTR_EVENT( p_event, plno, &sc_WFLBY_EV_DEF_PLAYER_MGSTART, &p_evwk->mg_param );
		
		break;
		
	case WFLBY_EV_DEF_PLAYER_MG_SEQ_INTR_WAIT:
		// ミニゲームに参加できたかチェック
		if( p_evwk->mg_param.in_ok == WFLBY_EV_MG_RET_OK ){
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_MG_SEQ_INTR_END );
		}else{
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_MG_SEQ_INTR_RET );
		}
		break;
		
	// ミニゲームに進む
	case WFLBY_EV_DEF_PLAYER_MG_SEQ_INTR_END:
		{
			u32 endret;
			u32 status;
			u32 evno;
			u32 played_type;
			
			evno = WFLBY_ROOM_GetEvNo( p_rmwk );

			switch( evno ){
			case WFLBY_MAPEVID_EV_BALLSLOW:
				endret		= WFLBY_ROOM_RET_ROOM_BALLSLOW;
				played_type = WFLBY_PLAYED_BALLTHROW;
				break;
			case WFLBY_MAPEVID_EV_BALANCE:
				endret		= WFLBY_ROOM_RET_ROOM_BALANCE;
				played_type = WFLBY_PLAYED_BALANCEBALL;
				break;
			case WFLBY_MAPEVID_EV_BALLOON:
				endret		= WFLBY_ROOM_RET_ROOM_BALLOON;
				played_type = WFLBY_PLAYED_BALLOON;
				break;
			}

			WFLBY_ROOM_SetEndFlag( p_rmwk, TRUE );
			WFLBY_ROOM_SetEndRet( p_rmwk, endret );

			// ボウケンノート
			WFLBY_SYSTEM_FNOTE_SetPlayMinigame( p_system, p_evwk->mg_param.wflby_mg_type );

			// レコード
			WFLBY_SYSTEM_RECORD_AddMinigame( p_system );
			
			// 遊んだ情報バッファ
			WFLBY_SYSTEM_PLAYED_SetPlay( p_system, played_type );

			// 出現パターン設定
			{
				WF2DMAP_POS	pos;
				WFLBY_ROOM_SAVE* p_save;
				WF2DMAP_WAY	way;	

				// プレイヤーの向いている方向の逆方向に出す
				way = WFLBY_3DOBJCONT_GetWkObjData( p_player, WF2DMAP_OBJPM_WAY );
				way = WF2DMPA_OBJToolRetWayGet( way );

				p_save		= WFLBY_ROOM_GetSaveData( p_rmwk );

				// 今の位置の１歩下の位置に出す
				pos = WFLBY_3DOBJCONT_GetWkPos( p_player );

				p_save->pl_inevno	= WFLBY_EV_DEF_PLAYERIN_MINIGAME;
				p_save->pl_gridx	= WF2DMAP_POS2GRID( pos.x );
				p_save->pl_gridy	= WF2DMAP_POS2GRID( pos.y );
				p_save->pl_way		= way;
				p_save->minigame	= p_evwk->mg_param.wflby_mg_type; 
			}


			// BGM変更をロック
			WFLBY_SYSTEM_SetLockBGMChange( WFLBY_ROOM_GetSystemData( p_rmwk ), TRUE );


			// ワーク破棄
			WFLBY_EVENTWK_DeleteWk( p_wk );

			return TRUE;	// 終了
		}
		break;
		
	// フィールドに戻る
	case WFLBY_EV_DEF_PLAYER_MG_SEQ_INTR_RET:
		// 向いている逆方向に１グリッド歩かせる
		{
			WF2DMAP_POS	pos;
			WF2DMAP_WAY	way;	

			// プレイヤーの向いている方向の逆方向に出す
			way = WFLBY_3DOBJCONT_GetWkObjData( p_player, WF2DMAP_OBJPM_WAY );
			way = WF2DMPA_OBJToolRetWayGet( way );
			
			WFLBY_3DOBJCONT_SetWkActCmd( p_objcont, p_player, WF2DMAP_CMD_WALK, way );
		
			WFLBY_EVENTWK_AddSeq( p_wk );
		}
		break;
		
	// 歩き待ち
	case WFLBY_EV_DEF_PLAYER_MG_SEQ_INTR_RETWAIT:
		{
			u32 st;

			st = WFLBY_3DOBJCONT_GetWkObjData( p_player, WF2DMAP_OBJPM_ST );
			if( st == WF2DMAP_OBJST_NONE ){

#if PL_T0867_080716_FIX
				// もうオート移動しないので、他人が会話ジャンプしてOK
				WFLBY_ROOM_SetPlayerEventAfterMove( p_rmwk, FALSE );
#endif
				
				WFLBY_EVENTWK_AddSeq( p_wk );
			}
		}
		break;

	// NGメッセージ表示
	case WFLBY_EV_DEF_PLAYER_MG_SEQ_MSG:
		{
			STRBUF* p_str;
			u32 msgidx;
			BOOL msg_write = TRUE;

			switch( p_evwk->mg_param.in_ok ){
			case WFLBY_EV_MG_RET_NG_NUM:			// 人数がいっぱいで入れない
				// ミニゲーム名をワードセットに設定
				WFLBY_ROOM_MSG_SetMinigame( p_rmwk, p_evwk->mg_param.wflby_mg_type, 0 );
				msgidx = msg_wifi_h_info_01_04;
				break;
			case WFLBY_EV_MG_RET_NG_PLAYING:		// ゲーム中なので入れない
				msgidx = msg_wifi_h_info_01_05;
				break;
			case WFLBY_EV_MG_RET_NG_TIME:		// ミニゲーム台終了しているので入れない
				// ミニゲーム名をワードセットに設定
				WFLBY_ROOM_MSG_SetMinigame( p_rmwk, p_evwk->mg_param.wflby_mg_type, 0 );
				msgidx = msg_wifi_h_info_01_06;
				break;
			case WFLBY_EV_MG_RET_NG_MIN:			// 必要な人数がそろわなかった
				msgidx = msg_wifi_h_info_04_01;
				break;
			case WFLBY_EV_MG_RET_NG_DISCON:		// 誰かが切断した
				msgidx = msg_wifi_h_info_02_05;
				break;
			case WFLBY_EV_MG_RET_NG_BCAN:		// BCancelした
				msgidx = msg_wifi_h_info_05_10;
				break;

			case WFLBY_EV_MG_RET_NG_LOCK:		// 以前親のときにBCancelしたので参加ロック中
				msgidx = msg_wifi_h_info_01_07;
				break;
				
			default:
			case WFLBY_EV_MG_RET_NG_MY:			// 自ら断った
				msg_write = FALSE;
				break;
			}

			// 描画
			if( msg_write ){
				p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msgidx );
				WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

				WFLBY_EVENTWK_AddSeq( p_wk );
			}else{
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_MG_SEQ_RET_NORMAL );
			}
		}
		break;
		
	// メッセージ終了待ち
	case WFLBY_EV_DEF_PLAYER_MG_SEQ_MSGWAIT:
		// メッセージ終了待ち
		if( WFLBY_ROOM_TALKWIN_WaitEnd( p_rmwk ) == TRUE ){

			// メッセージOFF
			WFLBY_ROOM_TALKWIN_Off( p_rmwk );

			WFLBY_EVENTWK_AddSeq( p_wk );
		}
		break;

	// 元に戻る
	case WFLBY_EV_DEF_PLAYER_MG_SEQ_RET_NORMAL:
		// ワーク破棄
		WFLBY_EVENTWK_DeleteWk( p_wk );

		// 状態を戻す
		WFLBY_SYSTEM_SetMyStatus( WFLBY_ROOM_GetSystemData(p_rmwk), WFLBY_STATUS_LOGIN );
		
		// もとのプレイヤーイベントに戻す
		// 基本動作の関数を設定
		WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_player, WFLBY_3DOBJCONT_MOVEPLAYER );

		// プレイヤーメイン常駐イベントを設定
		{
			WFLBY_EVENT* p_event;
			p_event = WFLBY_ROOM_GetEvent( p_rmwk );
			WFLBY_EVENT_SET_PRIVATE_EVENT( p_event, plno, &sc_WFLBY_EV_DEF_PLAYERMAIN, NULL );
		}
		break;
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	アンケートに行く
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EV_DEF_PlayerOutAnketo( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	WFLBY_EV_DEF_PLAYER_ANKETO_WK* p_evwk;
	WFLBY_EVENT* p_event;
	WFLBY_3DOBJCONT* p_objcont;
	WFLBY_3DPERSON* p_player;
	WFLBY_SYSTEM* p_system;
	STRBUF* p_str;

	p_evwk = WFLBY_EVENTWK_GetWk( p_wk );

	// システムワーク取得
	p_event		= WFLBY_ROOM_GetEvent( p_rmwk );
	p_objcont	= WFLBY_ROOM_GetObjCont( p_rmwk );
	p_player	= WFLBY_3DOBJCONT_GetPlayer( p_objcont );
	p_system	= WFLBY_ROOM_GetSystemData( p_rmwk );

	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){
	// アンケートイベント開始
	// まずはアンケートの受付中なのか、発表中なのかチェック
	case WFLBY_EV_DEF_PLAYER_ANKETO_SEQ_START:

		p_evwk = WFLBY_EVENTWK_AllocWk( p_wk, sizeof(WFLBY_EV_DEF_PLAYER_SUBCHAN_WK) );


		// 状態をアンケートにする
		WFLBY_SYSTEM_SetMyStatus( WFLBY_ROOM_GetSystemData(p_rmwk), WFLBY_STATUS_ANKETO );

		// 終了してないかチェック
#ifdef WFLBY_DEBUG_EV_DEF_ANKETO
		if( (WFLBY_SYSTEM_Event_GetEndCM( p_system ) == TRUE) || (sys.cont & PAD_BUTTON_L) ){
#else
		if( (WFLBY_SYSTEM_Event_GetEndCM( p_system ) == TRUE) ){
#endif
			p_evwk->ret_msg = msg_survey_monitor_a_10;
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_ANKETO_SEQ_RET_MOVE );
			break;
		}

		// 結果表示
#ifdef WFLBY_DEBUG_EV_DEF_ANKETO
		if( (WFLBY_SYSTEM_Event_GetNeon( p_system ) == TRUE) || (sys.cont & PAD_BUTTON_SELECT) ){
#else
		if( (WFLBY_SYSTEM_Event_GetNeon( p_system ) == TRUE) ){
#endif
			// 結果表示開始
			p_evwk->apl_no = WFLBY_ROOM_RET_ROOM_ANKETO_OUTPUT;
		}else{

#ifdef WFLBY_DEBUG_EV_DEF_ANKETO
			p_evwk->apl_no = WFLBY_ROOM_RET_ROOM_ANKETO_INPUT;
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_ANKETO_SEQ_START_ANKETO );
			break;
#endif
			
			// 投票ずみ？
			if( WFLBY_SYSTEM_FLAG_GetAnketoInput( p_system ) == TRUE ){
				p_evwk->ret_msg = msg_survey_monitor_b_n03;
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_ANKETO_SEQ_RET_MOVE );
				break;
			}else{
				p_evwk->apl_no = WFLBY_ROOM_RET_ROOM_ANKETO_INPUT;
			}
		}
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_PLAYER_ANKETO_SEQ_START_ANKETO );
		break;

	// アンケートへ進む
	case WFLBY_EV_DEF_PLAYER_ANKETO_SEQ_START_ANKETO:
		{
			u32 endret;
			u32 status;
			
			if( p_evwk->apl_no == WFLBY_ROOM_RET_ROOM_ANKETO_INPUT ){
				endret		= WFLBY_ROOM_RET_ROOM_ANKETO_INPUT;
			}else{
				endret		= WFLBY_ROOM_RET_ROOM_ANKETO_OUTPUT;

				// アンケート見たフラグを設定
				WFLBY_SYSTEM_FLAG_SetAnketoView( p_system );
			}

			WFLBY_ROOM_SetEndFlag( p_rmwk, TRUE );
			WFLBY_ROOM_SetEndRet( p_rmwk, endret );

			// やったことにアンケートを追加
			// tomoya 080618
			WFLBY_SYSTEM_SetLastAction( p_system, WFLBY_LASTACTION_ANKETO, DWC_LOBBY_INVALID_USER_ID );

			// 出現パターン設定
			{
				WF2DMAP_POS	pos;
				WFLBY_ROOM_SAVE* p_save;
				WF2DMAP_WAY	way;	

				// プレイヤーの向いている方向の逆方向に出す
				way = WFLBY_3DOBJCONT_GetWkObjData( p_player, WF2DMAP_OBJPM_WAY );
				way = WF2DMPA_OBJToolRetWayGet( way );

				p_save		= WFLBY_ROOM_GetSaveData( p_rmwk );

				// 今の位置の１歩下の位置に出す
				pos = WFLBY_3DOBJCONT_GetWkPos( p_player );

				p_save->pl_inevno	= WFLBY_EV_DEF_PLAYERIN_ANKETO;
				p_save->pl_gridx	= WF2DMAP_POS2GRID( pos.x );
				p_save->pl_gridy	= WF2DMAP_POS2GRID( pos.y );
				p_save->pl_way		= way;
				p_save->minigame	= 0; 
			}

			// ワーク破棄
			WFLBY_EVENTWK_DeleteWk( p_wk );

			return TRUE;	// 終了
		}
		break;

	// 戻る
	case WFLBY_EV_DEF_PLAYER_ANKETO_SEQ_RET_MOVE:
		// 向いている逆方向に１グリッド歩かせる
		{
			WF2DMAP_POS	pos;
			WF2DMAP_WAY	way;	

			// プレイヤーの向いている方向の逆方向に出す
			way = WFLBY_3DOBJCONT_GetWkObjData( p_player, WF2DMAP_OBJPM_WAY );
			way = WF2DMPA_OBJToolRetWayGet( way );
			
			WFLBY_3DOBJCONT_SetWkActCmd( p_objcont, p_player, WF2DMAP_CMD_WALK, way );
		
			WFLBY_EVENTWK_AddSeq( p_wk );
		}
		break;
		
	// 戻りまち
	case WFLBY_EV_DEF_PLAYER_ANKETO_SEQ_RET_WAIT:
		{
			u32 st;

			st = WFLBY_3DOBJCONT_GetWkObjData( p_player, WF2DMAP_OBJPM_ST );
			if( st == WF2DMAP_OBJST_NONE ){

#if PL_T0867_080716_FIX
				// もうオート移動しないので、他人が会話ジャンプしてOK
				WFLBY_ROOM_SetPlayerEventAfterMove( p_rmwk, FALSE );
#endif

				WFLBY_EVENTWK_AddSeq( p_wk );
			}
		}
		break;

	// アンケートへは行かない
	case WFLBY_EV_DEF_PLAYER_ANKETO_SEQ_RET_MSG:
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, p_evwk->ret_msg );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		WFLBY_EVENTWK_AddSeq( p_wk );
		break;

	case WFLBY_EV_DEF_PLAYER_ANKETO_SEQ_RET_MSGWAIT:
		// メッセージ終了待ち
		if( WFLBY_ROOM_TALKWIN_WaitEnd( p_rmwk ) == TRUE ){

			// メッセージOFF
			WFLBY_ROOM_TALKWIN_Off( p_rmwk );

			WFLBY_EVENTWK_AddSeq( p_wk );
		}
		break;

	case WFLBY_EV_DEF_PLAYER_ANKETO_SEQ_RET_NORMAL:
		// ワーク破棄
		WFLBY_EVENTWK_DeleteWk( p_wk );

		// 状態を戻す
		WFLBY_SYSTEM_SetMyStatus( WFLBY_ROOM_GetSystemData(p_rmwk), WFLBY_STATUS_LOGIN );
		
		// もとのプレイヤーイベントに戻す
		// 基本動作の関数を設定
		WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_player, WFLBY_3DOBJCONT_MOVEPLAYER );

		// プレイヤーメイン常駐イベントを設定
		{
			WFLBY_EVENT* p_event;
			p_event = WFLBY_ROOM_GetEvent( p_rmwk );
			WFLBY_EVENT_SET_PRIVATE_EVENT( p_event, plno, &sc_WFLBY_EV_DEF_PLAYERMAIN, NULL );
		}
		break;

	default:
		GF_ASSERT( 0 );
		break;
	}

	return FALSE;	// 終了
}

//----------------------------------------------------------------------------
/**
 *	@brief	イベントナンバーからチャンネルナンバーを返す
 *
 *	@param	evno	イベントナンバー
 *
 *	@return	チャンネルナンバー
 */
//-----------------------------------------------------------------------------
static u32 WFLBY_EV_DEF_PlayerOutSubChan_GetEvnoChannel( u32 evno )
{
	u32 channel_no;

	switch( evno ){
	case WFLBY_MAPEVID_EV_WLDTIMER_00:
	case WFLBY_MAPEVID_EV_WLDTIMER_01:
	case WFLBY_MAPEVID_EV_WLDTIMER_02:
	case WFLBY_MAPEVID_EV_WLDTIMER_03:
		channel_no = DWC_LOBBY_SUBCHAN_CLOCK;
		break;

	case WFLBY_MAPEVID_EV_TOPIC_00:
	case WFLBY_MAPEVID_EV_TOPIC_01:
	case WFLBY_MAPEVID_EV_TOPIC_02:
	case WFLBY_MAPEVID_EV_TOPIC_03:
		channel_no = DWC_LOBBY_SUBCHAN_NEWS;
		break;

	case WFLBY_MAPEVID_EV_FOOT_00_00:
	case WFLBY_MAPEVID_EV_FOOT_00_01:
	case WFLBY_MAPEVID_EV_FOOT_00_02:
	case WFLBY_MAPEVID_EV_FOOT_00_03:
	case WFLBY_MAPEVID_EV_FOOT_00_04:
	case WFLBY_MAPEVID_EV_FOOT_00_05:
	case WFLBY_MAPEVID_EV_FOOT_00_06:
	case WFLBY_MAPEVID_EV_FOOT_00_07:
		channel_no = DWC_LOBBY_SUBCHAN_FOOT1;
		break;

	case WFLBY_MAPEVID_EV_FOOT_01_00:
	case WFLBY_MAPEVID_EV_FOOT_01_01:
	case WFLBY_MAPEVID_EV_FOOT_01_02:
	case WFLBY_MAPEVID_EV_FOOT_01_03:
	case WFLBY_MAPEVID_EV_FOOT_01_04:
	case WFLBY_MAPEVID_EV_FOOT_01_05:
	case WFLBY_MAPEVID_EV_FOOT_01_06:
	case WFLBY_MAPEVID_EV_FOOT_01_07:
		channel_no = DWC_LOBBY_SUBCHAN_FOOT2;
		break;

	default:
		GF_ASSERT(0);
		channel_no = 0;
		break;
	}

	return channel_no;
}

//----------------------------------------------------------------------------
/**
 *	@brief	次に何をさせるのか上に返す値を取得する
 *
 *	@param	evno	イベントナンバー
 *
 *	@return	返す値
 */
//-----------------------------------------------------------------------------
static u32 WFLBY_EV_DEF_PlayerOutSubChan_GetEvnoRetNum( u32 evno )
{
	u32 ret_num;

	switch( evno ){
	case WFLBY_MAPEVID_EV_WLDTIMER_00:
	case WFLBY_MAPEVID_EV_WLDTIMER_01:
	case WFLBY_MAPEVID_EV_WLDTIMER_02:
	case WFLBY_MAPEVID_EV_WLDTIMER_03:
		ret_num = WFLBY_ROOM_RET_ROOM_WORLDTIMER;
		break;

	case WFLBY_MAPEVID_EV_TOPIC_00:
	case WFLBY_MAPEVID_EV_TOPIC_01:
	case WFLBY_MAPEVID_EV_TOPIC_02:
	case WFLBY_MAPEVID_EV_TOPIC_03:
		ret_num = WFLBY_ROOM_RET_ROOM_TOPIC;
		break;

	case WFLBY_MAPEVID_EV_FOOT_00_00:
	case WFLBY_MAPEVID_EV_FOOT_00_01:
	case WFLBY_MAPEVID_EV_FOOT_00_02:
	case WFLBY_MAPEVID_EV_FOOT_00_03:
	case WFLBY_MAPEVID_EV_FOOT_00_04:
	case WFLBY_MAPEVID_EV_FOOT_00_05:
	case WFLBY_MAPEVID_EV_FOOT_00_06:
	case WFLBY_MAPEVID_EV_FOOT_00_07:
		ret_num = WFLBY_ROOM_RET_ROOM_FOOT1;
		break;

	case WFLBY_MAPEVID_EV_FOOT_01_00:
	case WFLBY_MAPEVID_EV_FOOT_01_01:
	case WFLBY_MAPEVID_EV_FOOT_01_02:
	case WFLBY_MAPEVID_EV_FOOT_01_03:
	case WFLBY_MAPEVID_EV_FOOT_01_04:
	case WFLBY_MAPEVID_EV_FOOT_01_05:
	case WFLBY_MAPEVID_EV_FOOT_01_06:
	case WFLBY_MAPEVID_EV_FOOT_01_07:
		ret_num = WFLBY_ROOM_RET_ROOM_FOOT2;
		break;

	default:
		GF_ASSERT(0);
		ret_num = 0;
		break;
	}

	return ret_num;
}

//----------------------------------------------------------------------------
/**
 *	@brief	イベントナンバーから状態定数を取得する
 *
 *	@param	evno	イベントナンバー
 *		
 *	@return	状態定数
 */
//-----------------------------------------------------------------------------
static u32 WFLBY_EV_DEF_PlayerOutSubChan_GetEvnoStatus( u32 evno )
{
	u32 ret_num;

	switch( evno ){
	case WFLBY_MAPEVID_EV_WLDTIMER_00:
	case WFLBY_MAPEVID_EV_WLDTIMER_01:
	case WFLBY_MAPEVID_EV_WLDTIMER_02:
	case WFLBY_MAPEVID_EV_WLDTIMER_03:
		ret_num = WFLBY_STATUS_WORLDTIMER;
		break;

	case WFLBY_MAPEVID_EV_TOPIC_00:
	case WFLBY_MAPEVID_EV_TOPIC_01:
	case WFLBY_MAPEVID_EV_TOPIC_02:
	case WFLBY_MAPEVID_EV_TOPIC_03:
		ret_num = WFLBY_STATUS_TOPIC;
		break;

	case WFLBY_MAPEVID_EV_FOOT_00_00:
	case WFLBY_MAPEVID_EV_FOOT_00_01:
	case WFLBY_MAPEVID_EV_FOOT_00_02:
	case WFLBY_MAPEVID_EV_FOOT_00_03:
	case WFLBY_MAPEVID_EV_FOOT_00_04:
	case WFLBY_MAPEVID_EV_FOOT_00_05:
	case WFLBY_MAPEVID_EV_FOOT_00_06:
	case WFLBY_MAPEVID_EV_FOOT_00_07:
		ret_num = WFLBY_STATUS_FOOTBOAD00;
		break;

	case WFLBY_MAPEVID_EV_FOOT_01_00:
	case WFLBY_MAPEVID_EV_FOOT_01_01:
	case WFLBY_MAPEVID_EV_FOOT_01_02:
	case WFLBY_MAPEVID_EV_FOOT_01_03:
	case WFLBY_MAPEVID_EV_FOOT_01_04:
	case WFLBY_MAPEVID_EV_FOOT_01_05:
	case WFLBY_MAPEVID_EV_FOOT_01_06:
	case WFLBY_MAPEVID_EV_FOOT_01_07:
		ret_num = WFLBY_STATUS_FOOTBOAD01;
		break;

	default:
		GF_ASSERT(0);
		ret_num = 0;
		break;
	}

	return ret_num;
}


//----------------------------------------------------------------------------
/**
 *	@brief		入り口お姉さん
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EV_DEF_PlayerA_SWIN_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	
	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){
	case 0:
		{
			STRBUF* p_str;
			WFLBY_SYSTEM* p_system;

			p_system = WFLBY_ROOM_GetSystemData( p_rmwk );

			if( WFLBY_SYSTEM_Event_GetEndCM( p_system ) == TRUE ){
				p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoa_04 );
			}else{
				// PARADE中かチェック
				if( WFLBY_SYSTEM_Event_GetParade( p_system ) == TRUE ){
					p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoc_06 );
				}else{
					p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoa_03 );
				}
			}
			WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

			// 話しかけられた音
			Snd_SePlay( WFLBY_SND_STAFF );

			
			WFLBY_EVENTWK_AddSeq( p_wk );
		}
		break;

	case 1:
		if( WFLBY_ROOM_TALKWIN_WaitEnd( p_rmwk ) ){
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
		break;
	}

	return FALSE;
}






//----------------------------------------------------------------------------
/**
 *	@brief	フロート乗り込みイベント
 *
 *	@param	p_wk		ワーク
 *	@param	p_rmwk		広場ワーク
 *	@param	plno		プレイヤーNO
 *
 *	@retval	TRUE	イベント終了
 *	@retval	FALSE	イベント途中
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EV_FLOAT_Inside( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	WFLBY_EV_FLOAT_INSIDE*	p_evwk;
	WFLBY_3DMAPOBJ_CONT*	p_mapobjcont;
	WFLBY_3DOBJCONT*		p_objcont;
	WFLBY_CAMERA*			p_camera;
	WFLBY_SYSTEM*			p_system;
	BOOL					result;
	STRBUF*					p_str;

	p_objcont		= WFLBY_ROOM_GetObjCont( p_rmwk );
	p_mapobjcont	= WFLBY_ROOM_GetMapObjCont( p_rmwk );
	p_camera		= WFLBY_ROOM_GetCamera( p_rmwk );
	p_system		= WFLBY_ROOM_GetSystemData( p_rmwk );
		
	// ワーク取得
	p_evwk = WFLBY_EVENTWK_GetWk( p_wk );
	
	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){
	// 初期化
	case WFLBY_EV_FLOAT_SEQ_INIT:
		{
			u32	evno;

			p_evwk = WFLBY_EVENTWK_AllocWk( p_wk, sizeof(WFLBY_EV_FLOAT_INSIDE) );

			// 乗った人取得
			p_evwk->p_obj = WFLBY_3DOBJCONT_GetPlIDWk( p_objcont, plno );

			evno	= WFLBY_ROOM_GetEvNo( p_rmwk );
			switch( evno ){
			case WFLBY_MAPEVID_EV_FLOAT00:
				p_evwk->station = WFLBY_FLOAT_STATION_LEFT;
				break;
				
			case WFLBY_MAPEVID_EV_FLOAT01:
				p_evwk->station = WFLBY_FLOAT_STATION_CENTER;
				break;
				
			case WFLBY_MAPEVID_EV_FLOAT02:
				p_evwk->station = WFLBY_FLOAT_STATION_RIGHT;
				break;

			default:
				GF_ASSERT( 0 );
				break;
			}
		}

		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_FLOAT_SEQ_FLOAT_CHECK );
		break;

	// 予約部分
	case WFLBY_EV_FLOAT_SEQ_FLOAT_CHECK:		
		// フロートが始まってる？
		if( WFLBY_SYSTEM_Event_GetParade( p_system ) == FALSE ){
			// まだはじまってない 1歩さがってメッセージ
			p_evwk->next_seq = WFLBY_EV_FLOAT_SEQ_FLOAT_END_BEFORE;	
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_FLOAT_SEQ_FLOAT_END_DOWN );
		}else{

			// フロートが終わってないかチェック
			if( WFLBY_SYSTEM_Event_GetEndCM( p_system ) == TRUE ){
				// フロート終了
				p_evwk->next_seq = WFLBY_EV_FLOAT_SEQ_FLOAT_END_AFTER;	
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_FLOAT_SEQ_FLOAT_END_DOWN );
			}else{

				// 予約できるかチェック
				p_evwk->floatidx = WFLBY_SYSTEM_SetFloatReserve( p_system, plno, p_evwk->station );

				if( p_evwk->floatidx != DWC_LOBBY_USERIDTBL_IDX_NONE ){
					// 予約できた！
					WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_FLOAT_SEQ_FLOAT_INWAIT );

					// メッセージ表示＆タイムウエイト設定
					p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msg_wifi_h_info_06_02 );
					WFLBY_ROOM_TALKWIN_PrintStrAllPut( p_rmwk, p_str );
					WFLBY_ROOM_TALKWIN_StartTimeWait( p_rmwk );
				}else{
					// できない
					p_evwk->next_seq = WFLBY_EV_FLOAT_SEQ_FLOAT_END_FULL;	
					WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_FLOAT_SEQ_FLOAT_END_DOWN );
				}
			}
		}
		break;

	// 乗れるまで待つ
	case WFLBY_EV_FLOAT_SEQ_FLOAT_INWAIT:	

		// BボタンとKEYDOWNでCancel処理
		if( (sys.trg & PAD_BUTTON_CANCEL) || (sys.trg & PAD_KEY_DOWN) ){
			// 予約Cancel
			WFLBY_SYSTEM_ResetFloatReserve( p_system, plno );

			// メッセージOFF
			WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );
			p_evwk->msg_off_ret_seq = WFLBY_EV_FLOAT_SEQ_FLOAT_END_DOWN;

			// 1歩下がって通常イベントに戻る
			p_evwk->next_seq = WFLBY_EV_FLOAT_SEQ_RET;

			// WINOFF->END_DOWN->RET
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_FLOAT_SEQ_FLOAT_WINOFFWAIT );

		}else{
			
			// 目的の座席が目の前にきたら飛び乗る
			if( WFLBY_EV_FLOAT_CheckJumpOn( p_evwk, p_mapobjcont ) == TRUE ){
				// メッセージOFF
				WFLBY_ROOM_TALKWIN_StopTimeWait( p_rmwk );

				// フロートに乗ったことを送信
				WFLBY_SYSTEM_SendFloatOn( p_system );
				p_evwk->count = 0;	// ジャンプ開始

				//　ステータスをフロートニする
				WFLBY_SYSTEM_SetMyStatus( WFLBY_ROOM_GetSystemData( p_rmwk ), WFLBY_STATUS_FLOAT );

				// これ以降はタイムアウト処理はなし
				WFLBY_ROOM_OffTimeOutCheck( p_rmwk );

				//  カメラを主人公の座標をそのまま追尾するように変更する。
				WFLBY_CAMERA_ResetTargetPerson( p_camera );

				// 主人公の描画を更新しないように変更
				WFLBY_3DOBJCONT_DRAW_SetUpdata( p_evwk->p_obj, FALSE );

				// ジャンプアニメ設定
				WFLBY_3DOBJCONT_DRAW_SetAnmJump( p_evwk->p_obj, TRUE );
				
				// ジャンプ音
				Snd_SePlay( WFLBY_SND_JUMP );
				
				// した画面フロートに変更
				WFLBY_ROOM_UNDERWIN_FloatBttnOn( p_rmwk );

				// ジャンプ処理へ
				// WINOFF->JUMP
				p_evwk->msg_off_ret_seq = WFLBY_EV_FLOAT_SEQ_FLOAT_JUMP;
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_FLOAT_SEQ_FLOAT_WINOFFWAIT );

				// ボウケンノート
				WFLBY_SYSTEM_FNOTE_SetParade( p_system );
			}
		}
		break;
		
	// ジャンプ
	case WFLBY_EV_FLOAT_SEQ_FLOAT_JUMP:
		// 目の前で、シートの位置までジャンプ
		// ジャンプ中のカメラ動作も管理
		result = WFLBY_EV_FLOAT_JumpMove( p_evwk, p_mapobjcont, p_camera );
		if( result == TRUE ){

			// ジャンプアニメ停止
			WFLBY_3DOBJCONT_DRAW_SetAnmJump( p_evwk->p_obj, FALSE );

			// 前を向かせる
			WFLBY_3DOBJCONT_DRAW_SetWay( p_evwk->p_obj, WF2DMAP_WAY_LEFT );

			// 光らせる
			WFLBY_3DOBJCONT_DRAW_SetLight( p_evwk->p_obj, 1 << WFLBY_DRAW_LIGHT_FLOAT );
			
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_FLOAT_SEQ_FLOAT_JUMPON );

			p_evwk->count = 0;
		}
		break;


	// 着地アニメ
	case WFLBY_EV_FLOAT_SEQ_FLOAT_JUMPON:
		// まずは座標をあわせる
		WFLBY_EV_FLOAT_MatrixSet( p_evwk, p_mapobjcont, p_rmwk );
		// その座標をカメラに設定
		WFLBY_EV_FLOAT_SetCameraPos( p_evwk, p_camera );

		// 着地座標をさらに設定
		result = WFLBY_EV_FLOAT_JumpOnMove( p_evwk, p_mapobjcont );
		if( result == TRUE ){
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_FLOAT_SEQ_FLOAT_MAIN );

			// した画面ボタン操作開始
			WFLBY_ROOM_GadGet_SetStop( p_rmwk, FALSE );
		}
		break;
		
	// メイン	ボタン押したり
	case WFLBY_EV_FLOAT_SEQ_FLOAT_MAIN:		
		// オブジェの位置を設定
		{
			VecFx32 vec;
			
			//
			WFLBY_EV_FLOAT_MatrixSet( p_evwk, p_mapobjcont, p_rmwk );

			// カメラ座標にもする
			WFLBY_EV_FLOAT_SetCameraPos( p_evwk, p_camera );

			// 表示座標を取得
			WFLBY_3DOBJCONT_DRAW_Get3DMatrix( p_evwk->p_obj, &vec );

			// ぐらぐらエフェクトの値を設定
			WFLBY_EV_FLOAT_MatrixDonDonSet( p_evwk, p_mapobjcont, p_rmwk );

#ifdef WFLBY_DEBUG_EV_DEF_FLOAT_RETRY
			if( sys.trg & PAD_BUTTON_B ){
				WFLBY_ROOM_SAVE* p_save;

				// 予約リセット
				WFLBY_SYSTEM_ResetFloatReserve( p_system, plno );

				p_save				= WFLBY_ROOM_GetSaveData( p_rmwk );
				p_save->pl_inevno	= WFLBY_EV_DEF_PLAYERIN_MINIGAME;
				p_save->pl_gridx	= 23;
				p_save->pl_gridy	= 8;
				p_save->pl_way		= WF2DMAP_WAY_DOWN;
				
				WFLBY_ROOM_SetEndFlag( p_rmwk, TRUE );
				WFLBY_ROOM_SetEndRet( p_rmwk, WFLBY_ROOM_RET_ROOM_RETURN );
				WFLBY_ROOM_TALKWIN_Off( p_rmwk );
				// 状態変更
				WFLBY_SYSTEM_SetMyStatus( WFLBY_ROOM_GetSystemData(p_rmwk), WFLBY_STATUS_LOGIN );
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_FLOAT_SEQ_ENDWAIT );
				break;
			}
#endif
			

			// 移動のリミットにきたらカメラのスクロールが止まり、そのままフェードアウト
			if( vec.x < WFLBY_EV_FLOAT_CAMERA_MOVE_END ){

				// 終了へ
				p_evwk->wait = 0;
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_FLOAT_SEQ_END );
			}
		}
		break;

	// 乗れないので１歩下がる
	case WFLBY_EV_FLOAT_SEQ_FLOAT_END_DOWN:	
		WFLBY_3DOBJCONT_SetWkActCmd( p_objcont, p_evwk->p_obj, WF2DMAP_CMD_WALK, WF2DMAP_WAY_DOWN );
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_FLOAT_SEQ_FLOAT_END_DOWNWAIT );
		break;

	// 乗れないので１歩下がる
	case WFLBY_EV_FLOAT_SEQ_FLOAT_END_DOWNWAIT:
		if( WFLBY_3DOBJCONT_GetWkObjData( p_evwk->p_obj, WF2DMAP_OBJPM_ST ) == WF2DMAP_OBJST_NONE ){


#if PL_T0867_080716_FIX
			// もうオート移動しないので、他人が会話ジャンプしてOK
			WFLBY_ROOM_SetPlayerEventAfterMove( p_rmwk, FALSE );
#endif
			WFLBY_EVENTWK_SetSeq( p_wk, p_evwk->next_seq );
		}
		break;

	// フロート前なので乗れないメッセージ
	case WFLBY_EV_FLOAT_SEQ_FLOAT_END_BEFORE:

		// メッセージ表示
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msg_wifi_h_info_06_01 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// 通常のイベントに戻る
		p_evwk->next_seq = WFLBY_EV_FLOAT_SEQ_RET;

		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_FLOAT_SEQ_FLOAT_MSGWAIT );
		break;
	
	// フロートがいっぱいなので乗れない
	case WFLBY_EV_FLOAT_SEQ_FLOAT_END_FULL:
		// メッセージ表示
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msg_wifi_h_info_06_03 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// 通常のイベントに戻る
		p_evwk->next_seq = WFLBY_EV_FLOAT_SEQ_RET;

#if PL_T0867_080716_FIX
		// もうオート移動しないので、他人が会話ジャンプしてOK
		WFLBY_ROOM_SetPlayerEventAfterMove( p_rmwk, FALSE );
#endif

		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_FLOAT_SEQ_FLOAT_MSGWAIT );
		break;

	// パレード後なので乗れないメッセージ
	case WFLBY_EV_FLOAT_SEQ_FLOAT_END_AFTER:	
		// メッセージ表示
		p_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_INFO, msg_wifi_h_info_06_04 );
		WFLBY_ROOM_TALKWIN_PrintStr( p_rmwk, p_str );

		// 通常のイベントに戻る
		p_evwk->next_seq = WFLBY_EV_FLOAT_SEQ_RET;

#if PL_T0867_080716_FIX
		// もうオート移動しないので、他人が会話ジャンプしてOK
		WFLBY_ROOM_SetPlayerEventAfterMove( p_rmwk, FALSE );
#endif

		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_FLOAT_SEQ_FLOAT_MSGWAIT );
		break;

	// メッセージ待ち
	case WFLBY_EV_FLOAT_SEQ_FLOAT_MSGWAIT:
		if( WFLBY_ROOM_TALKWIN_WaitEnd( p_rmwk ) == TRUE ){
			WFLBY_EVENTWK_SetSeq( p_wk, p_evwk->next_seq );
		}
		break;

	// ウィンドウＯＦＦ待ち(TimeWaitをＯＦＦにするのが完了してからＭＳＧをＯＦＦするため)
	case WFLBY_EV_FLOAT_SEQ_FLOAT_WINOFFWAIT:
		WFLBY_ROOM_TALKWIN_Off( p_rmwk );
		WFLBY_EVENTWK_SetSeq( p_wk, p_evwk->msg_off_ret_seq );
		break;

	// 通常終了
	case WFLBY_EV_FLOAT_SEQ_END:				//  終了

		WFLBY_EV_FLOAT_MatrixSet( p_evwk, p_mapobjcont, p_rmwk );


		// ぐらぐらエフェクトの値を設定
		WFLBY_EV_FLOAT_MatrixDonDonSet( p_evwk, p_mapobjcont, p_rmwk );


		
		p_evwk->wait ++;
		if( p_evwk->wait > 120 ){
		
			WFLBY_ROOM_SetEndFlag( p_rmwk, TRUE );
			WFLBY_ROOM_SetEndRet( p_rmwk, WFLBY_ROOM_RET_ROOM_OUT );
			
			WFLBY_ROOM_TALKWIN_Off( p_rmwk );

			// 状態変更
			WFLBY_SYSTEM_SetMyStatus( WFLBY_ROOM_GetSystemData(p_rmwk), WFLBY_STATUS_LOGOUT );

			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_FLOAT_SEQ_ENDWAIT );
		}	
		break;

	//  終了待ち
	case WFLBY_EV_FLOAT_SEQ_ENDWAIT:			

		WFLBY_EV_FLOAT_MatrixSet( p_evwk, p_mapobjcont, p_rmwk );


		// ぐらぐらエフェクトの値を設定
		WFLBY_EV_FLOAT_MatrixDonDonSet( p_evwk, p_mapobjcont, p_rmwk );

		break;

	//  通常のイベントに戻る
	case WFLBY_EV_FLOAT_SEQ_RET:				
		{
			WFLBY_EVENT* p_event;

			p_event = WFLBY_ROOM_GetEvent( p_rmwk );

			WFLBY_ROOM_TALKWIN_Off( p_rmwk );

			WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_evwk->p_obj, WFLBY_3DOBJCONT_MOVEPLAYER );
			// ワーク破棄
			WFLBY_EVENTWK_DeleteWk( p_wk );

			// イベント設定
			WFLBY_EVENT_SET_PRIVATE_EVENT( p_event, plno, &sc_WFLBY_EV_DEF_PLAYERMAIN, NULL );
		}
		break;

	}

	return FALSE;
}




//-----------------------------------------------------------------------------
/**
 *			プライベート関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	カメラ座標を設定する
 *
 *	@param	p_wk		ワーク
 *	@param	p_camera	カメラ
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_FLOAT_SetCameraPos( WFLBY_EV_FLOAT_INSIDE* p_wk, WFLBY_CAMERA* p_camera )
{
	VecFx32 vec;
	WFLBY_3DOBJCONT_DRAW_Get3DMatrix( p_wk->p_obj, &vec );

	// カメラ座標にもする
	WFLBY_CAMERA_SetTarget3D( p_camera, vec.x, vec.y, vec.z );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ジャンプ開始するかチェック
 *
 *	@param	cp_wk			ワーク
 *	@param	cp_mapobjcont	マップオブジェ管理システム
 *
 *	@retval	TRUE	ジャンプ開始
 *	@retval	FALSE	ジャンプしない
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EV_FLOAT_CheckJumpOn( const WFLBY_EV_FLOAT_INSIDE* cp_wk, const WFLBY_3DMAPOBJ_CONT* cp_mapobjcont )
{
	VecFx32 obj_mat;
	VecFx32 sheet_mat;
	u32 float_idx, float_ofs;

	WFLBY_SYSTEM_GetFloatIdxOfs( cp_wk->floatidx, &float_idx, &float_ofs );

	// シート座標
	WFLBY_3DMAPOBJCONT_FLOAT_GetSheetPos( cp_mapobjcont, float_idx, float_ofs, &sheet_mat );

	// 自分の座標
	WFLBY_3DOBJCONT_DRAW_Get3DMatrix( cp_wk->p_obj, &obj_mat );	

	// 位置を比較
	if( (obj_mat.x + WFLBY_EV_FLOAT_JUMP_CHECK_X) >= sheet_mat.x ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	オブジェジャンプ処理
 *
 *	@param	p_wk			ワーク
 *	@param	p_mapobjcont	マップオブジェ管理システム 
 *	@paran	p_camera		カメラ
 *
 *	@retval	TRUE	動作完了
 *	@retval	FALSE	動作途中
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EV_FLOAT_JumpMove( WFLBY_EV_FLOAT_INSIDE* p_wk, WFLBY_3DMAPOBJ_CONT* p_mapobjcont, WFLBY_CAMERA* p_camera )
{
	VecFx32 obj_mat;
	VecFx32 sheet_mat;
	u32 float_idx, float_ofs;
	WF2DMAP_POS	obj_pos;
	VecFx32 set_mat;
	u16 rot_y;
	BOOL ret;
	

	// カウント
	p_wk->count ++;
	if( p_wk->count < WFLBY_EV_FLOAT_JUMP_ANM_SYNC ){
		ret = FALSE;
	}else{
		p_wk->count = WFLBY_EV_FLOAT_JUMP_ANM_SYNC;
		ret = TRUE;
	}
	

	WFLBY_SYSTEM_GetFloatIdxOfs( p_wk->floatidx, &float_idx, &float_ofs );

	// シート座標
	WFLBY_3DMAPOBJCONT_FLOAT_GetSheetPos( p_mapobjcont, float_idx, float_ofs, &sheet_mat );

	// 自分の座標
	obj_pos = WFLBY_3DOBJCONT_GetWkPos( p_wk->p_obj );
	WFLBY_3DMATRIX_GetPosVec( &obj_pos, &obj_mat );	
	
	// シート位置と実データ位置の間をcountのカウンタ分移動する
	set_mat.x = 0;
	set_mat.y = FX_Div( FX_Mul( FX32_CONST(p_wk->count), (sheet_mat.y - obj_mat.y) ), FX32_CONST( WFLBY_EV_FLOAT_JUMP_ANM_SYNC ) );
	if( p_wk->count >= WFLBY_EV_FLOAT_JUMP_ANM_SYNC_ZS ){
		set_mat.z = FX_Div( FX_Mul( FX32_CONST(p_wk->count-WFLBY_EV_FLOAT_JUMP_ANM_SYNC_ZS), (sheet_mat.z - obj_mat.z) ), FX32_CONST( WFLBY_EV_FLOAT_JUMP_ANM_SYNC_Z ) );
	}else{
		set_mat.z = 0;
	}

	// ここがカメラの座標
	VEC_Add( &set_mat, &obj_mat, &set_mat );
	WFLBY_CAMERA_SetTarget3D( p_camera, set_mat.x, set_mat.y, set_mat.z );

	// ジャンプなのでＹにサインカーブを入れる
	rot_y = (p_wk->count * 0x7fff) / WFLBY_EV_FLOAT_JUMP_ANM_SYNC;
	set_mat.y += FX_Mul( FX_SinIdx( rot_y ), WFLBY_EV_FLOAT_JUMP_MOVE_Y );

	// 座標を設定
	WFLBY_3DOBJCONT_DRAW_Set3DMatrix( p_wk->p_obj, &set_mat );

	return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	着地アニメ
 *
 *	@param	p_wk			ワーク
 *	@param	p_mapobjcont	マップオブジェ
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EV_FLOAT_JumpOnMove( WFLBY_EV_FLOAT_INSIDE* p_wk, WFLBY_3DMAPOBJ_CONT* p_mapobjcont )
{
	s32 local_count;
	BOOL ret;
	u16 rota_y;
	fx32 move_y;
	VecFx32 matrix;
	
	// カウント
	p_wk->count ++;
	if( p_wk->count < WFLBY_EV_FLOAT_JUMPON_ANM_SYNC ){
		ret = FALSE;
	}else{
		p_wk->count = WFLBY_EV_FLOAT_JUMPON_ANM_SYNC;
		ret = TRUE;
	}

	// ローカルカウント値
	local_count = p_wk->count % WFLBY_EV_FLOAT_JUMPON_ANM_ONE;
	rota_y = (local_count * 0x7fff) / WFLBY_EV_FLOAT_JUMPON_ANM_ONE;
	move_y = FX_Mul( FX_SinIdx( rota_y ), WFLBY_EV_FLOAT_JUMPON_ANM_MOVEY );

	// 今設定されている座標に足しこむ
	WFLBY_3DOBJCONT_DRAW_Get3DMatrix( p_wk->p_obj, &matrix );
	matrix.y += move_y;
	WFLBY_3DOBJCONT_DRAW_Set3DMatrix( p_wk->p_obj, &matrix );

	return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	座標を設定
 *
 *	@param	p_wk			ワーク
 *	@param	p_mapobjcont	マップオブジェ管理システム
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_FLOAT_MatrixSet( WFLBY_EV_FLOAT_INSIDE* p_wk, WFLBY_3DMAPOBJ_CONT* p_mapobjcont, WFLBY_ROOMWK* p_rmwk )
{
	VecFx32 vec;
	u32 float_offs;
	u32 float_idx;

	WFLBY_SYSTEM_GetFloatIdxOfs( p_wk->floatidx, &float_idx, &float_offs );

	WFLBY_3DMAPOBJCONT_FLOAT_GetSheetPos( p_mapobjcont, float_idx, float_offs, &vec );
	WFLBY_3DOBJCONT_DRAW_Set3DMatrix( p_wk->p_obj, &vec );

}

//----------------------------------------------------------------------------
/**
 *	@brief	ドンドンアニメの結果を座標に反映
 *
 *	@param	p_wk			ワーク
 *	@param	p_mapobjcont	マップオブジェ管理システム
 *	@param	p_rmwk			部屋ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_FLOAT_MatrixDonDonSet( WFLBY_EV_FLOAT_INSIDE* p_wk, WFLBY_3DMAPOBJ_CONT* p_mapobjcont, WFLBY_ROOMWK* p_rmwk )
{
	VecFx32 vec;
	fx32 y;
	u32 float_idx, float_ofs;

	WFLBY_SYSTEM_GetFloatIdxOfs( p_wk->floatidx, &float_idx, &float_ofs );

	// フロートシェイクアニメ中はオブジェをぐらぐら
	if( WFLBY_ROOM_FLOAT_CheckShakeAnm( p_rmwk, float_idx ) == TRUE ){
		if( WFLBY_EV_DEF_DONDON_CheckMove( &p_wk->dondon ) == FALSE ){
			WFLBY_EV_DEF_DONDON_Start( &p_wk->dondon );
		}
	}else{
		if( WFLBY_EV_DEF_DONDON_CheckMove( &p_wk->dondon ) == TRUE ){
			WFLBY_EV_DEF_DONDON_EndReq( &p_wk->dondon );
		}
	}
	//  ドンドンアニメ
	y = WFLBY_EV_DEF_DONDON_Main( &p_wk->dondon );

	// 表示座標を取得
	WFLBY_3DOBJCONT_DRAW_Get3DMatrix( p_wk->p_obj, &vec );

	// ぐらぐらエフェクトの値を設定
	vec.y += y;
	WFLBY_3DOBJCONT_DRAW_Set3DMatrix( p_wk->p_obj, &vec );
}


// 技タイプビットマップリスト
//----------------------------------------------------------------------------
/**
 *	@brief	技タイプビットマップリストデータを作成
 *
 *	@param	p_wk		ワーク
 *	@param	p_rmwk		部屋ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_DEF_PlayerInNormal_WazaTypeBmpListInit( WFLBY_EV_DEF_PLAYER_IN_WK* p_wk, WFLBY_ROOMWK* p_rmwk )
{
	int i;
	const STRBUF* cp_str;

	WFLBY_ROOM_LISTWIN_CreateBmpList( p_rmwk, WFLBY_POKEWAZA_TYPE_NUM );

	// 技タイプを設定
	for( i=WFLBY_POKEWAZA_TYPE_NORMAL; i<WFLBY_POKEWAZA_TYPE_NUM; i++ ){

		// 選択なしの項目以外はワードセットで設定
		WFLBY_ROOM_MSG_SetWazaType( p_rmwk, 0, i );
		cp_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoa_t03 );

		WFLBY_ROOM_LISTWIN_SetBmpListStr( p_rmwk, cp_str, i );
	}
	// 最後に”えらばない”
	cp_str = WFLBY_ROOM_MSG_Get( p_rmwk, WFLBY_DEFMSG_TYPE_HIROBA, msg_hiroba_infoa_t04 );
	WFLBY_ROOM_LISTWIN_SetBmpListStr( p_rmwk, cp_str, WFLBY_POKEWAZA_TYPE_NONE );

	p_wk->bmplist		= sc_WFLBY_EV_DEF_WAZATYPE_HEADER;
	p_wk->bmplist.list	= WFLBY_ROOM_LISTWIN_GetBmpList( p_rmwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	技タイプビットマップリストデータを破棄
 *
 *	@param	p_wk		ワーク
 *	@param	p_rmwk		部屋ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_EV_DEF_PlayerInNormal_WazaTypeBmpListDelete( WFLBY_EV_DEF_PLAYER_IN_WK* p_wk, WFLBY_ROOMWK* p_rmwk )
{
	WFLBY_ROOM_LISTWIN_DeleteBmpList( p_rmwk );
}





//----------------------------------------------------------------------------
/**
 *	@brief	NPC出現待ちイベント
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EV_DEF_NpcWait_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	WFLBY_EV_DEF_NPC_WK*	p_evwk;
	WFLBY_3DPERSON*			p_npc;
	WFLBY_SYSTEM*			p_system;
	WFLBY_3DOBJCONT*		p_objcont;
	WFLBY_EVENT*			p_event;


	p_system	= WFLBY_ROOM_GetSystemData( p_rmwk );
	p_objcont	= WFLBY_ROOM_GetObjCont( p_rmwk );
	p_npc		= WFLBY_3DOBJCONT_GetPlIDWk( p_objcont, plno );
	p_evwk		= WFLBY_EVENTWK_GetWk( p_wk );
	p_event		= WFLBY_ROOM_GetEvent( p_rmwk );
	
	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){
	// 初期化
	case WFLBY_EV_DEF_NPCWAIT_SEQ_INIT:
		p_evwk = WFLBY_EVENTWK_AllocWk( p_wk, sizeof(WFLBY_EV_DEF_NPC_WK) );
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_NPCWAIT_SEQ_WAIT );
		
	// 出現待ち
	case WFLBY_EV_DEF_NPCWAIT_SEQ_WAIT:
		{
			BOOL in_flag, in_tbl;
			
			// 自分が追加されたかチェック	追加されてなくても、存在したら中に入る
			in_flag = WFLBY_SYSTEM_FLAG_GetUserIn( p_system, plno );
			in_tbl	= WFLBY_SYSTEM_GetUserRoomIn( p_system, plno );
			if( (in_flag == TRUE) || (in_tbl == TRUE) ){

				// 落ちるまで落とし続ける
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_NPCWAIT_SEQ_INWAIT );
			}
		}
		break;

	// 登録できるまで登録し続ける
	case WFLBY_EV_DEF_NPCWAIT_SEQ_INWAIT:
		{
			const WFLBY_USER_PROFILE* cp_profile;
			u32 tr_type;

			// プロフィール取得
			cp_profile = WFLBY_SYSTEM_GetUserProfile( p_system, plno );
			if( cp_profile == NULL ){
				// いなくなってしまったので、待機に戻る
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_NPCWAIT_SEQ_WAIT );
				break;
			}

			// その人がガジェットをもらったかチェック
			if( WFLBY_SYSTEM_CheckGetItem( cp_profile ) == TRUE ){

				p_npc = WFLBY_3DOBJCONT_AddNpc( p_objcont, plno,
						WFLBY_SYSTEM_GetProfileTrType( cp_profile ) );
				if( p_npc ){	// 登録できたら次へ

					// 非表示
					WFLBY_3DOBJCONT_DRAW_SetDraw( p_npc, FALSE );

					WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_NPCWAIT_SEQ_STATUS_CHECK );
				}
			}
		}
		break;

	// 状態をチェックして、ミニゲームなどを行っている場合はJUMP割り込みイベントへ
	case WFLBY_EV_DEF_NPCWAIT_SEQ_STATUS_CHECK:
		{
			u32 status;
			const WFLBY_USER_PROFILE* cp_profile;
			BOOL result;
			
			cp_profile	= WFLBY_SYSTEM_GetUserProfile( p_system, plno );

			// いなくなってないかチェック
			if( cp_profile == NULL ){
				// Waitに戻る
				if( p_npc != NULL ){
					WFLBY_3DOBJCONT_Delete( p_npc );
				}
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_NPCWAIT_SEQ_WAIT );
				break;
			}
			
			status		= WFLBY_SYSTEM_GetProfileStatus( cp_profile );

			// 変更した内容により、動作を変更する
			// ある場所に飛ばす場合
			// 位置と方向を指定して飛ばす
			result = WFLBY_EV_DEF_NpcJumpParamMake( &p_evwk->intrparam, p_system, p_rmwk, p_npc, status, WFLBY_EV_DEF_NPCJUMP_MOVETYPE_STARTDOWN );
			if( result ){

				// 帰ってきたら終了へ
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_NPCWAIT_SEQ_END );

				// NPCの動作停止
				WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_npc, WFLBY_3DOBJCONT_MOVENONE );

				// NPC状態表示不可能 
				WFLBY_ROOM_SetNpcInfoDrawBusy( p_rmwk, plno, TRUE );				
				// ローカル状態をBUSYにする
				WFLBY_ROOM_SetNpcTalkBusy( p_rmwk, plno, TRUE );

				// 描画更新停止
				WFLBY_3DOBJCONT_DRAW_SetUpdata( p_npc, FALSE );

				// 割り込みイベント発生
				// いきなりジャンプインしてくるところからはじめる
				WFLBY_EVENT_SET_PRIVATE_INTR_EVENT( p_event, plno, &sc_WFLBY_EV_DEF_NPCJUMP, &p_evwk->intrparam );
			}else{
				
				// ステータス的にジャンプしていく必要がないなら
				// 普通に落とす
				WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_npc, WFLBY_3DOBJCONT_MOVEFLYDOWN );

				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_NPCWAIT_SEQ_FLYDOWN );

				// ローカル状態をBUSYにする
				WFLBY_ROOM_SetNpcTalkBusy( p_rmwk, plno, TRUE );
				// NPC状態表示不可能 
				WFLBY_ROOM_SetNpcInfoDrawBusy( p_rmwk, plno, TRUE );				

				// 表示
				WFLBY_3DOBJCONT_DRAW_SetDraw( p_npc, TRUE );
			}
		}
		break;
		
	// 落ちてくる処理
	case WFLBY_EV_DEF_NPCWAIT_SEQ_FLYDOWN:
		{
			BOOL result;
			result  = WFLBY_3DOBJCONT_CheckWkMoveEnd( p_npc );
			if( result == TRUE ){
				WFLBY_EVENTWK_AddSeq( p_wk );
			}
		}
		break;
		
	case WFLBY_EV_DEF_NPCWAIT_SEQ_END:

		// ワーク破棄
		WFLBY_EVENTWK_DeleteWk( p_wk );
		
		// メイン動作へ
		WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_npc, WFLBY_3DOBJCONT_MOVENPC );
		WFLBY_EVENT_SET_PRIVATE_EVENT( p_event, plno, &sc_WFLBY_EV_DEF_NPCMAIN, NULL );

		// ローカル状態をBUSYを戻す
		WFLBY_ROOM_SetNpcTalkBusy( p_rmwk, plno, FALSE );
		// NPC状態表示可能 
		WFLBY_ROOM_SetNpcInfoDrawBusy( p_rmwk, plno, FALSE );				
		break;
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	NPCメインイベント
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EV_DEF_NpcMain_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	WFLBY_EV_DEF_NPC_WK*	p_evwk;		// イベントワーク
	WFLBY_3DPERSON*			p_npc;
	WFLBY_3DPERSON*			p_player;
	WFLBY_SYSTEM*			p_system;
	WFLBY_3DOBJCONT*		p_objcont;
	WFLBY_EVENT*			p_event;

	p_system	= WFLBY_ROOM_GetSystemData( p_rmwk );
	p_objcont	= WFLBY_ROOM_GetObjCont( p_rmwk );
	p_event		= WFLBY_ROOM_GetEvent( p_rmwk );
	p_npc		= WFLBY_3DOBJCONT_GetPlIDWk( p_objcont, plno );
	p_player	= WFLBY_3DOBJCONT_GetPlayer( p_objcont );

	p_evwk		= WFLBY_EVENTWK_GetWk( p_wk );
			

	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){
	// 初期化
	case WFLBY_EV_DEF_NPCMAIN_SEQ_INIT:
		p_evwk = WFLBY_EVENTWK_AllocWk( p_wk, sizeof(WFLBY_EV_DEF_NPC_WK) );
		WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_NPCMAIN_SEQ_MAIN );

	// メイン
	case WFLBY_EV_DEF_NPCMAIN_SEQ_MAIN:
		// 退室チェック
		if( WFLBY_SYSTEM_FLAG_GetUserOut(p_system, plno) ){
			// 退室処理へ
			// ワーク破棄
			WFLBY_EVENTWK_DeleteWk( p_wk );

			WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_npc, WFLBY_3DOBJCONT_MOVENONE );	// 動作停止
			// NPC状態表示不可能 
			WFLBY_ROOM_SetNpcInfoDrawBusy( p_rmwk, plno, TRUE );				

			// ローカル状態をBUSYにする
			WFLBY_ROOM_SetNpcTalkBusy( p_rmwk, plno, TRUE );

			WFLBY_EVENT_SET_PRIVATE_EVENT( p_event, plno, &sc_WFLBY_EV_DEF_NPCEND, NULL );

			break;
		}

		// プロフィール変更処理チェック
		if( WFLBY_SYSTEM_FLAG_GetUserProfileUpdate(p_system, plno) ){
		}

		// フロートチェック
		if( WFLBY_SYSTEM_Event_GetParade( p_system ) == TRUE ){
			u32 idx;

			// 乗ってないかチェック
			idx = WFLBY_SYSTEM_GetFloatPlIdxReserve( p_system, plno );
			if( idx != DWC_LOBBY_USERIDTBL_IDX_NONE ){
				// 乗ってるのでフロートイベントへ
				// NPCの動作停止
				WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_npc, WFLBY_3DOBJCONT_MOVENONE );

				// NPC状態表示不可能 
				WFLBY_ROOM_SetNpcInfoDrawBusy( p_rmwk, plno, TRUE );				
				// ローカル状態をBUSYにする
				WFLBY_ROOM_SetNpcTalkBusy( p_rmwk, plno, TRUE );

				// 描画更新停止
				WFLBY_3DOBJCONT_DRAW_SetUpdata( p_npc, FALSE );

				// ワーク破棄
				WFLBY_EVENTWK_DeleteWk( p_wk );

				WFLBY_EVENT_SET_PRIVATE_EVENT( p_event, plno, &sc_WFLBY_EV_DEF_NPCFLOAT, &p_evwk->intrparam );
				break;
			}
		}

		// 位置を飛ばすかチェック
		{
			u32 status;
			const WFLBY_USER_PROFILE* cp_profile;
			BOOL result;
			
			cp_profile = WFLBY_SYSTEM_GetUserProfile( p_system, plno );
			status = WFLBY_SYSTEM_GetProfileStatus( cp_profile );

			// 変更した内容により、動作を変更する
			// ある場所に飛ばす場合
			// 位置と方向を指定して飛ばす
			result = WFLBY_EV_DEF_NpcJumpParamMake( &p_evwk->intrparam, p_system, p_rmwk, p_npc, status, WFLBY_EV_DEF_NPCJUMP_MOVETYPE_NORMAL );
			if( result ){

				// NPCの動作停止
				WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_npc, WFLBY_3DOBJCONT_MOVENONE );

				// NPC状態表示不可能 
				WFLBY_ROOM_SetNpcInfoDrawBusy( p_rmwk, plno, TRUE );				
				// ローカル状態をBUSYにする
				WFLBY_ROOM_SetNpcTalkBusy( p_rmwk, plno, TRUE );
				// 描画更新停止
				WFLBY_3DOBJCONT_DRAW_SetUpdata( p_npc, FALSE );

				// 割り込みイベント発生
				WFLBY_EVENT_SET_PRIVATE_INTR_EVENT( p_event, plno, &sc_WFLBY_EV_DEF_NPCJUMP, &p_evwk->intrparam );
				break;
			}


			// ガジェットの実行
			if( WFLBY_SYSTEM_GetGadGetFlag( p_system, plno ) == TRUE ){
				// 人物動作停止
				WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_npc, WFLBY_3DOBJCONT_MOVENONE );

				// NPC状態表示不可能 
//				WFLBY_ROOM_SetNpcInfoDrawBusy( p_rmwk, plno, TRUE );				タッチトイはOK
				// ローカル状態をBUSYにする
				WFLBY_ROOM_SetNpcTalkBusy( p_rmwk, plno, TRUE );
				
				// 割り込みイベント発生
				WFLBY_EVENT_SET_PRIVATE_INTR_EVENT( p_event, plno, &sc_WFLBY_EV_DEF_ALL_GADGET, NULL );
				break;
			}
		}
		
		break;
		
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	退室処理
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EV_DEF_NpcEnd_Brfore( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	WFLBY_3DPERSON*		p_npc;
	WFLBY_3DOBJCONT*	p_objcont;

	p_objcont = WFLBY_ROOM_GetObjCont( p_rmwk );

	p_npc = WFLBY_3DOBJCONT_GetPlIDWk( p_objcont, plno );
	
	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){
	// 退室
	case WFLBY_EV_DEF_NPCEND_SEQ_OUT:
		WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_npc, WFLBY_3DOBJCONT_MOVEFLYUP );	// 上に跳んでく
		WFLBY_EVENTWK_AddSeq( p_wk );
		break;

	// 退室完了待ち
	case WFLBY_EV_DEF_NPCEND_SEQ_OUTWAIT:
		{
			BOOL result;

			result = WFLBY_3DOBJCONT_CheckWkMoveEnd( p_npc );
			if( result ){
				WFLBY_EVENTWK_AddSeq( p_wk );
			}
		}
		break;

	// 終了
	case WFLBY_EV_DEF_NPCEND_SEQ_END:
		// 破棄
		WFLBY_3DOBJCONT_Delete( p_npc );

		// 入室待ちへ
		{
			WFLBY_EVENT* p_event;
			p_event = WFLBY_ROOM_GetEvent( p_rmwk );
			WFLBY_EVENT_SET_PRIVATE_EVENT( p_event, plno, &sc_WFLBY_EV_DEF_NPCWAIT, NULL );
		}

		break;

	}

	return FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	フロートに乗って退室
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EV_DEF_NpcFloat_Brfore( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	WFLBY_EV_DEF_NPCFLOAT_WK* p_evwk;
	WFLBY_3DOBJCONT*		p_objcont;
	WFLBY_3DMAPOBJ_CONT*	p_mapobjcont;
	WFLBY_SYSTEM*			p_system;

	p_system	= WFLBY_ROOM_GetSystemData( p_rmwk );
	p_objcont	= WFLBY_ROOM_GetObjCont( p_rmwk );
	p_mapobjcont= WFLBY_ROOM_GetMapObjCont( p_rmwk );
	p_evwk		= WFLBY_EVENTWK_GetWk( p_wk );

	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){
	// 初期化
	case WFLBY_EV_DEF_NPCFLOAT_SEQ_INIT:
		{
			p_evwk = WFLBY_EVENTWK_AllocWk( p_wk, sizeof(WFLBY_EV_DEF_NPCFLOAT_WK) );
			p_evwk->p_npc		= WFLBY_3DOBJCONT_GetPlIDWk( p_objcont, plno );
			p_evwk->float_idx	= WFLBY_SYSTEM_GetFloatPlIdxReserve( p_system, plno );
			GF_ASSERT( p_evwk->float_idx != DWC_LOBBY_USERIDTBL_IDX_NONE );

			WFLBY_SYSTEM_GetFloatIdxOfs( p_evwk->float_idx, &p_evwk->float_idx, &p_evwk->float_offs );

			OS_TPrintf( "npc=%d float_idx %d float_offs %d \n", plno, p_evwk->float_idx, p_evwk->float_offs );
			
			
			// 上に飛び上がる
			WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_evwk->p_npc, WFLBY_3DOBJCONT_MOVEFLYUP );
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_NPCFLOAT_SEQ_UPWAIT );
		}
		break;

	// 飛び上がり
	case WFLBY_EV_DEF_NPCFLOAT_SEQ_UPWAIT:
		if( WFLBY_3DOBJCONT_CheckWkMoveEnd( p_evwk->p_npc ) ){

			// 着地点を計算
			{
				VecFx32 matrix;

				WFLBY_3DMAPOBJCONT_FLOAT_GetSheetPos( p_mapobjcont, p_evwk->float_idx,
						p_evwk->float_offs, &matrix );
				p_evwk->on_x	= matrix.x + WFLBY_EV_DEF_NPCFLOAT_DIF_X;
				WFLBY_3DOBJCONT_DRAW_Get3DMatrix( p_evwk->p_npc, &matrix );
				p_evwk->start_y	= matrix.y;
				p_evwk->count	= 0;
			}

			
			// もう当たり判定関係ないので、０，０の位置に移動
			{
				WF2DMAP_POS pos = {0};
				WFLBY_3DOBJCONT_SetWkPos( p_evwk->p_npc, pos );
			}

			// 落ちてくる音
			Snd_SePlay( WFLBY_SND_PLOUT );

			// 回転開始
			WFLBY_3DOBJCONT_DRAW_SetAnmRota( p_evwk->p_npc, TRUE );
			
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_NPCFLOAT_SEQ_DOWNWAIT );
		}
		break;

	// 着地
	case WFLBY_EV_DEF_NPCFLOAT_SEQ_DOWNWAIT:
		{
			VecFx32 matrix;
			VecFx32 set_mat;
			BOOL end = FALSE;

			//  カウント処理
			p_evwk->count ++;
			if( p_evwk->count >= WFLBY_EV_DEF_NPCFLOAT_DOWN_COUNT ){
				p_evwk->count = WFLBY_EV_DEF_NPCFLOAT_DOWN_COUNT;
				end = TRUE;
			}

			// シート位置取得
			WFLBY_3DMAPOBJCONT_FLOAT_GetSheetPos( p_mapobjcont, p_evwk->float_idx,
					p_evwk->float_offs, &matrix );

			set_mat.z = matrix.z;
			set_mat.x = p_evwk->on_x;
			set_mat.y = FX_Div( FX_Mul( FX32_CONST(p_evwk->count), (matrix.y - p_evwk->start_y) ), FX32_CONST( WFLBY_EV_DEF_NPCFLOAT_DOWN_COUNT ) );
			set_mat.y += p_evwk->start_y;

			WFLBY_3DOBJCONT_DRAW_Set3DMatrix( p_evwk->p_npc, &set_mat );

			// カウンタが終了したら着地成功
			if( end == TRUE ){

				// 回転オワリ
				WFLBY_3DOBJCONT_DRAW_SetAnmRota( p_evwk->p_npc, FALSE );
				
				// 明るくする
				WFLBY_3DOBJCONT_DRAW_SetLight( p_evwk->p_npc, 1<<WFLBY_DRAW_LIGHT_FLOAT  );

				// 左を向ける
				WFLBY_3DOBJCONT_DRAW_SetWay( p_evwk->p_npc, WF2DMAP_WAY_LEFT );
				
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_NPCFLOAT_SEQ_MAIN );
			}
		}
		break;

	// メイン
	case WFLBY_EV_DEF_NPCFLOAT_SEQ_MAIN:
		{
			VecFx32 matrix;
			fx32 y;

			// フロートシェイクアニメ中はオブジェをぐらぐら
			if( WFLBY_ROOM_FLOAT_CheckShakeAnm( p_rmwk, p_evwk->float_idx ) == TRUE ){
				if( WFLBY_EV_DEF_DONDON_CheckMove( &p_evwk->dondon ) == FALSE ){
					WFLBY_EV_DEF_DONDON_Start( &p_evwk->dondon );
				}
			}else{
				if( WFLBY_EV_DEF_DONDON_CheckMove( &p_evwk->dondon ) == TRUE ){
					WFLBY_EV_DEF_DONDON_EndReq( &p_evwk->dondon );
				}
			}
			//  ドンドンアニメ
			y = WFLBY_EV_DEF_DONDON_Main( &p_evwk->dondon );

			// シート位置取得
			WFLBY_3DMAPOBJCONT_FLOAT_GetSheetPos( p_mapobjcont, p_evwk->float_idx,
					p_evwk->float_offs, &matrix );

			matrix.y += y;

			// シート位置にずっと出す
			WFLBY_3DOBJCONT_DRAW_Set3DMatrix( p_evwk->p_npc, &matrix );

			// 乗っているフロートが非表示に動作停止したらオワリ
			if( WFLBY_SYSTEM_GetFloatMove( p_system, p_evwk->float_idx ) == WFLBY_FLOAT_STATE_AFTER ){
				WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_NPCFLOAT_SEQ_END );
			}
		}
		break;

	// 終了
	case WFLBY_EV_DEF_NPCFLOAT_SEQ_END:
		// 破棄
		WFLBY_3DOBJCONT_Delete( p_evwk->p_npc );

		// ワーク破棄
		WFLBY_EVENTWK_DeleteWk( p_wk );

		// 入室待ちへ
		{
			WFLBY_EVENT* p_event;
			p_event = WFLBY_ROOM_GetEvent( p_rmwk );
			WFLBY_EVENT_SET_PRIVATE_EVENT( p_event, plno, &sc_WFLBY_EV_DEF_NPCWAIT, NULL );
		}
		break;
	}

	return FALSE;
}




//-------------------------------------
///	NPC非常中イベント
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	ある場所にNPCをジャンプさせておく処理
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EV_DEF_NpcJump_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno )
{
	WFLBY_EV_DEF_NPCJUMP_PARAM* p_param;
	WFLBY_3DOBJCONT*			p_objcont;
	WFLBY_3DPERSON*				p_npc;
	WFLBY_3DPERSON*				p_player;
	WFLBY_SYSTEM*				p_system;
	
	// パラメータ取得
	p_param = WFLBY_EVENTWK_GetParam( p_wk );

	// システムデータ
	p_system = WFLBY_ROOM_GetSystemData( p_rmwk );
	
	// NPCのデータ取得
	p_objcont	= WFLBY_ROOM_GetObjCont( p_rmwk );
	p_npc		= WFLBY_3DOBJCONT_GetPlIDWk( p_objcont, plno );
	p_player	= WFLBY_3DOBJCONT_GetPlayer( p_objcont );

	switch( WFLBY_EVENTWK_GetSeq( p_wk ) ){
	// 初期化処理
	case WFLBY_EV_DEF_NPCJUMP_SEQ_INIT:
		if( p_param->movetype == WFLBY_EV_DEF_NPCJUMP_MOVETYPE_NORMAL ){
			WFLBY_EVENTWK_AddSeq( p_wk );
		}else{
			WFLBY_EVENTWK_SetSeq( p_wk, WFLBY_EV_DEF_NPCJUMP_SEQ_DOWN );
		}
		break;

	// 上る
	case WFLBY_EV_DEF_NPCJUMP_SEQ_UP:

		// その人の動作を変更
		WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_npc, WFLBY_3DOBJCONT_MOVEFLYUP );
		WFLBY_EVENTWK_AddSeq( p_wk );
		break;

	// のぼり待ち
	case WFLBY_EV_DEF_NPCJUMP_SEQ_UPWAIT:
	// 戻りの上り待ち
	case WFLBY_EV_DEF_NPCJUMP_SEQ_RET_UPWAIT:
	// 戻りのくだりまち
	case WFLBY_EV_DEF_NPCJUMP_SEQ_RET_DOWNWAIT:
		{
			BOOL result;
			result  = WFLBY_3DOBJCONT_CheckWkMoveEnd( p_npc );
			if( result == TRUE ){
				WFLBY_EVENTWK_AddSeq( p_wk );
			}
		}
		break;

	// 新しい場所に下る
	case WFLBY_EV_DEF_NPCJUMP_SEQ_DOWN:

		// 落ちる場所の設定
		{
			WF2DMAP_POS pos;
			pos = WFLBY_3DOBJCONT_GetWkPos( p_npc );
			WFLBY_3DOBJCONT_DRAW_SetMatrix( p_npc, &pos );
		}

		//  表示がOFFならONにする
		if( WFLBY_3DOBJCONT_DRAW_GetDraw( p_npc ) == FALSE ){
			// 表示
			WFLBY_3DOBJCONT_DRAW_SetDraw( p_npc, TRUE );
		}

		// 落とす
		WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_npc, WFLBY_3DOBJCONT_MOVEFLYDOWN );
		WFLBY_EVENTWK_AddSeq( p_wk );
		break;

	// 下り待ち
	case WFLBY_EV_DEF_NPCJUMP_SEQ_DOWNWAIT:
		{
			BOOL result;
			result  = WFLBY_3DOBJCONT_CheckWkMoveEnd( p_npc );
			if( result == TRUE ){
				WFLBY_EVENTWK_AddSeq( p_wk );

				// 動作を停止
				WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_npc, WFLBY_3DOBJCONT_MOVENONE );

				// 会話ビジー状態を解除
				WFLBY_ROOM_SetNpcTalkBusy( p_rmwk, plno, FALSE );

				// NPC状態表示可能 
				WFLBY_ROOM_SetNpcInfoDrawBusy( p_rmwk, plno, FALSE );
			}
		}
		break;

	// 状態遷移待ち
	case WFLBY_EV_DEF_NPCJUMP_SEQ_WAIT:
		{
			const WFLBY_USER_PROFILE* cp_profile;
			const WFLBY_USER_PROFILE* cp_profile_player;
			BOOL add_seq = FALSE;

			// 落ちきったので、会話状態のときなのかチェック
			if( (p_param->talk_flag == TRUE) && 
				(WFLBY_SYSTEM_CheckTalkBStart( p_system ) == FALSE) ){
				u32 retway;
				u32 way;
				u32 status;

				cp_profile_player	= WFLBY_SYSTEM_GetMyProfileLocal( p_system );
				status				= WFLBY_SYSTEM_GetProfileStatus( cp_profile_player );
				
				// 主人公状態が会話になったら振り向く
				if( status == WFLBY_STATUS_TALK ){
					// 主人公の会話を開始させる
					WFLBY_SYSTEM_SetTalkBStart( p_system );

					// 主人公を自分の方に向かせる
					way		= WFLBY_3DOBJCONT_GetWkObjData( p_npc, WF2DMAP_OBJPM_WAY );
					retway	= WF2DMPA_OBJToolRetWayGet( way );
					WFLBY_3DOBJCONT_SetWkActCmd( p_objcont, p_player, WF2DMAP_CMD_NONE, retway );
				}
			}

			cp_profile	= WFLBY_SYSTEM_GetUserProfile( p_system, plno );

			do{
				// 退室チェック
				if( cp_profile == NULL ){	// すでに退室している
					add_seq = TRUE;
					break;
				}


				// 会話状態なら、会話が終了したところで１回戻らせる
				// 落ちきったので、会話状態のときなのかチェック
				if( (p_param->talk_flag == TRUE) ){
					// 主人公の会話が終わったらNPC自身も戻る
					if( WFLBY_SYSTEM_CheckTalk( p_system ) == FALSE ){
						add_seq = TRUE;
						break;
					}
				}
				
				// 指定の状態が変わったら元の位置に戻る
				if( WFLBY_SYSTEM_FLAG_GetUserProfileUpdate(p_system, plno) ){
					u32 status;

					status		= WFLBY_SYSTEM_GetProfileStatus( cp_profile );

					// ステータスが変わったら元のの位置に戻る
					if( status != p_param->status ){
						add_seq = TRUE;
						break;
					}
				}
			}while(0);


			// シーケンスを進めるかチェック
			if( add_seq == TRUE ){
				WFLBY_EVENTWK_AddSeq( p_wk );

				// 会話ビジー状態を設定
				WFLBY_ROOM_SetNpcTalkBusy( p_rmwk, plno, TRUE );

				// NPC状態表示不可能 
				WFLBY_ROOM_SetNpcInfoDrawBusy( p_rmwk, plno, TRUE );
			}
		}
		break;

	// 戻りの上り
	case WFLBY_EV_DEF_NPCJUMP_SEQ_RET_UP:
		// その人の動作を変更
		WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_npc, WFLBY_3DOBJCONT_MOVEFLYUP );
		WFLBY_EVENTWK_AddSeq( p_wk );
		break;

	// 戻りのくだり
	case WFLBY_EV_DEF_NPCJUMP_SEQ_RET_DOWN:
		// 元の位置に戻れるかチェック
		// その場所に主人公がいないかチェック
		{
			WFLBY_3DPERSON* p_player;
			WF2DMAP_POS		pos;

			p_player = WFLBY_3DOBJCONT_GetPlayer( p_objcont );
			if( WFLBY_3DOBJCONT_CheckGridHit( p_player, 
						p_param->gridx, 
						p_param->gridy ) == FALSE ){

				pos.x = WF2DMAP_GRID2POS( p_param->gridx );
				pos.y = WF2DMAP_GRID2POS( p_param->gridy );

				// 座標を設定
				// 落ちる場所の設定
				WFLBY_3DOBJCONT_SetWkPos( p_npc, pos );
				WFLBY_3DOBJCONT_DRAW_SetMatrix( p_npc, &pos );

				// 落とす
				WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_npc, WFLBY_3DOBJCONT_MOVEFLYDOWN );
				WFLBY_EVENTWK_AddSeq( p_wk );
			}
		}
		break;

	// 終了
	case WFLBY_EV_DEF_NPCJUMP_SEQ_END:
		// NPC動作復帰
		WFLBY_3DOBJCONT_SetWkMove( p_objcont, p_npc, WFLBY_3DOBJCONT_MOVENPC );

		// ローカル状態をBUSYを元に戻す
		WFLBY_ROOM_SetNpcTalkBusy( p_rmwk, plno, FALSE );

		// NPC状態表示可能 
		WFLBY_ROOM_SetNpcInfoDrawBusy( p_rmwk, plno, FALSE );

		// 非常駐イベント終了
		return TRUE;

	}

	return FALSE;
}


//-------------------------------------
///	パラメータ作成関数
//=====================================

//----------------------------------------------------------------------------
/**
 *	@brief	ジャンプ位置移動パラメータ作成＆ジャンプ位置移動イベントを発動するかチェック
 *
 *	@param	p_param		パラメータ格納先
 *	@param	p_system	システムワーク
 *	@param	p_rmwk		ルームワーク
 *	@param	p_npc		NPCワーク
 *	@param	status		状態
 *	@param	movetype	動作タイプ
 *
 *	@retval	TRUE		イベント発動
 *	@retval	FALSE		イベント発動させない
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EV_DEF_NpcJumpParamMake( WFLBY_EV_DEF_NPCJUMP_PARAM* p_param, WFLBY_SYSTEM* p_system, WFLBY_ROOMWK* p_rmwk, WFLBY_3DPERSON* p_npc, u8 status, WFLBY_EV_DEF_NPCJUMP_MOVETYPE movetype )
{
	u32 plno;
	u32 way;
	WF2DMAP_POS pos;
	WFLBY_3DOBJCONT* p_objcont;
	WFLBY_3DPERSON* p_player;
	WFLBY_MAPCONT*	p_mapcont;
	BOOL talk_flag = FALSE;
	BOOL result;

	// オブジェ管理システム取得
	p_objcont	= WFLBY_ROOM_GetObjCont( p_rmwk );
	p_player	= WFLBY_3DOBJCONT_GetPlayer( p_objcont );
	
	// マップ管理システム取得
	p_mapcont	= WFLBY_ROOM_GetMapCont( p_rmwk );


	plno = WFLBY_3DOBJCONT_GetWkObjData( p_npc, WF2DMAP_OBJPM_PLID );

	// 方向の初期値は下
	way = WF2DMAP_WAY_DOWN;
	
	// status的にジャンプするものかチェック
	switch( status ){

	// 絶対にジャンプする
	case WFLBY_STATUS_BALLSLOW:		// 玉いれ
		result = WFLBY_EV_DEF_NpcJumpParam_GetOBJIDPos( p_mapcont, p_objcont, 
					sc_WFLBY_EV_DEF_NPC_OBJID_BS, WFLBY_EV_DEF_NPC_OBJID_MINIGAME_NUM, &pos );
		if( result == FALSE ){
			return FALSE;
		}
		break;
		
	case WFLBY_STATUS_BALANCEBALL:	// 玉乗り
		result = WFLBY_EV_DEF_NpcJumpParam_GetOBJIDPos( p_mapcont, p_objcont, 
					sc_WFLBY_EV_DEF_NPC_OBJID_BB, WFLBY_EV_DEF_NPC_OBJID_MINIGAME_NUM, &pos );
		if( result == FALSE ){
			return FALSE;
		}
		break;
		
	case WFLBY_STATUS_BALLOON:		// 風船わり
		result = WFLBY_EV_DEF_NpcJumpParam_GetOBJIDPos( p_mapcont, p_objcont, 
					sc_WFLBY_EV_DEF_NPC_OBJID_BL, WFLBY_EV_DEF_NPC_OBJID_MINIGAME_NUM, &pos );
		if( result == FALSE ){
			return FALSE;
		}
		break;
		
	case WFLBY_STATUS_FOOTBOAD00:	// 足跡ボード
		way = WF2DMAP_WAY_UP;
		result = WFLBY_EV_DEF_NpcJumpParam_GetOBJIDPos( p_mapcont, p_objcont, 
					sc_WFLBY_EV_DEF_NPC_OBJID_FT00, WFLBY_EV_DEF_NPC_OBJID_FOOT_NUM, &pos );
		if( result == FALSE ){
			return FALSE;
		}
		break;
		
	case WFLBY_STATUS_FOOTBOAD01:	// 足跡ボード
		way = WF2DMAP_WAY_UP;
		result = WFLBY_EV_DEF_NpcJumpParam_GetOBJIDPos( p_mapcont, p_objcont, 
					sc_WFLBY_EV_DEF_NPC_OBJID_FT01, WFLBY_EV_DEF_NPC_OBJID_FOOT_NUM, &pos );
		if( result == FALSE ){
			return FALSE;
		}
		break;
		
	case WFLBY_STATUS_WORLDTIMER:	// 世界時計
		way = WF2DMAP_WAY_RIGHT;
		result = WFLBY_EV_DEF_NpcJumpParam_GetOBJIDPos( p_mapcont, p_objcont, 
					sc_WFLBY_EV_DEF_NPC_OBJID_WT, WFLBY_EV_DEF_NPC_OBJID_WLDTIMER_NUM, &pos );
		if( result == FALSE ){
			return FALSE;
		}
		break;
		
	case WFLBY_STATUS_TOPIC:		// ニュース
		way = WF2DMAP_WAY_UP;
		result = WFLBY_EV_DEF_NpcJumpParam_GetOBJIDPos( p_mapcont, p_objcont, 
					sc_WFLBY_EV_DEF_NPC_OBJID_TP, WFLBY_EV_DEF_NPC_OBJID_TOPIC_NUM, &pos );
		if( result == FALSE ){
			return FALSE;
		}
		break;
		
	case WFLBY_STATUS_FLOAT:		// フロート
		// おあずけ
		return FALSE;

	// 会話状態によっては
	case WFLBY_STATUS_TALK:			// 会話
		// 主人公が会話中？
		if( WFLBY_SYSTEM_CheckTalk( p_system ) == FALSE ){
			return FALSE;
		}

		// 主人公は話しかけられた側
		if( WFLBY_SYSTEM_CheckTalkReq( p_system ) == FALSE ){
			return FALSE;
		}

		// 主人公が話しかけられた人？
		if( WFLBY_SYSTEM_GetTalkIdx( p_system ) != plno ){
			return FALSE;
		}

#if PL_T0867_080716_FIX
		// 主人公が会話可能状態になったかチェック
		// 無理なので、ジャンプしない
		if( WFLBY_ROOM_CheckPlayerEventAfterMove( p_rmwk ) == TRUE ){
			return FALSE;
		}
#endif

		// 主人公の近くに飛んでこさせる
		// 主人公の周り４グリッドで空いているところを探す。
		result = WFLBY_3DOBJCONT_GetOpenGird4Way( p_objcont, p_player, &way, &pos );
		GF_ASSERT( result == TRUE );

		// wayは主人公の向くべき方向
		way = WF2DMPA_OBJToolRetWayGet( way );	// 自分は逆向き
		talk_flag = TRUE;
		break;
	

	case WFLBY_STATUS_NONE:
	case WFLBY_STATUS_LOGIN:			// ログイン状態
	case WFLBY_STATUS_LOGOUT:		// 退室
	case WFLBY_STATUS_UNKNOWN:		// 不明
	case WFLBY_STATUS_MATCHING:		// ミニゲームのマッチング中
	case WFLBY_STATUS_ANKETO:		// アンケート中
	defalut:
		return FALSE;
	}

	// 予約&戻る場所を設定
	{
		WF2DMAP_POS retpos;

		// 戻る場所取得
		retpos = WFLBY_3DOBJCONT_GetWkPos( p_npc );

		// 位置を予約
		WFLBY_3DOBJCONT_SetWkPosAndWay( p_npc, pos, way );

		// パラメータ設定
		p_param->gridx		= WF2DMAP_POS2GRID( retpos.x );
		p_param->gridy		= WF2DMAP_POS2GRID( retpos.y );
		p_param->status		= status;
		p_param->talk_flag	= talk_flag;
		p_param->movetype	= movetype;
	}



	return TRUE;
	
}

//----------------------------------------------------------------------------
/**
 *	@brief	オブジェリストにあるオブジェIDのマップ位置に落ちていける場所があるかチェックする
 *
 *	@param	cp_mapcont		マップ管理システム
 *	@param	cp_objcont		オブジェ管理システム
 *	@param	cp_objid		オブジェIDリスト
 *	@param	num				リスト数
 *	@param	p_pos			落ちていける位置
 *
 *	@retval	TRUE	落ちる場所があった
 *	@retval	FALSE	落ちる場所がない
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EV_DEF_NpcJumpParam_GetOBJIDPos( const WFLBY_MAPCONT* cp_mapcont, const WFLBY_3DOBJCONT* cp_objcont, const u32* cp_objid, u32 num, WF2DMAP_POS* p_pos )
{
	u16 gridx, gridy;
	int i;
	BOOL result;

	for( i=0; i<num; i++ ){
		result = WFLBY_MAPCONT_SarchObjID( cp_mapcont, cp_objid[i], &gridx, &gridy, 0 );
		if( result == TRUE ){	// あったらその位置に人がいないかチェック
			if( WFLBY_3DOBJCONT_CheckSysGridHit( cp_objcont, gridx, gridy ) == NULL ){
				
				// その位置が空いている
				p_pos->x = WF2DMAP_GRID2POS( gridx );
				p_pos->y = WF2DMAP_GRID2POS( gridy );
				return TRUE;
			}
		}
	}

	return FALSE;
}

