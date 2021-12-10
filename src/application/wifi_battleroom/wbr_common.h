//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wbr_common.h
 *	@brief		wifi	バトルルーム　共通ヘッダー
 *	@author		tomoya takahashi
 *	@data		2007.02.15
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WBR_COMMON_H__
#define __WBR_COMMON_H__

#include "common.h"
#include "communication/comm_def.h"
#include "communication/comm_system.h"
#include "communication/comm_info.h"
#include "communication/comm_tool.h"
#include "wbr_comm_command.h"
#include "savedata/mystatus.h"
#include "application/trainer_card.h"

#include "include/application/wifi_2dmap/wf2dmap_common.h"

#undef GLOBAL
#ifdef	__WBR_COMMON_H_GLOBAL
#define	GLOBAL	/* */
#else
#define	GLOBAL	extern
#endif

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
#define WBR_COMM_NUM		(2)	// １度につながる人数
#define WBR_COMM_OYA_AID	(0)	// 親のAID
#define WBR_COMM_MYSTATUSSIZE	(32)	// mystatusのサイズ

// グリッドデータ
#define WBR_COMM_GRID_SIZ	( 16 )	// 1グリッドサイズ


//-------------------------------------
///	行動フラグ
//=====================================
typedef enum {
	WBR_MOVE_NONE,	// 何もなし（キー入力待ち）
	WBR_MOVE_TURN,	// 振り向き
	WBR_MOVE_WALK,	// 歩き
	WBR_MOVE_TALK,	// 話しかけ
	WBR_MOVE_RUN,	// 走り
	WBR_MOVE_NUM
} WBR_MOVE_FLAG;

//-------------------------------------
///	行動フラグ
//=====================================
typedef enum {
	WBR_WAY_TOP,
	WBR_WAY_BOTTOM,
	WBR_WAY_LEFT,
	WBR_WAY_RIGHT,
	WBR_WAY_NUM
} WBR_WAY_FLAG;

//-------------------------------------
///	話しかけTALKモード
//=====================================
typedef enum {
	WBR_TALK_NONE,	// 話しかけ関係なし
	WBR_TALK_OYA,	// 話しかけ関係の親
	WBR_TALK_KO,	// 話しかけ関係の子
} WBR_TALK_FLAG;

//-------------------------------------
///	話しかけリクエストフラグ
//=====================================
typedef enum {
	WBR_TALK_REQ_START,
	WBR_TALK_REQ_END,
} WBR_TALK_REQ_FLAG;


//-------------------------------------
///	話シーケンス
//=====================================
typedef enum {
	WBR_TALK_SEQ_START,			// 話しかけ開始
	WBR_TALK_SEQ_NEXT,			// 話しかけ次は？
	WBR_TALK_SEQ_END,			// 話しかけ終了
	WBR_TALK_SEQ_EXIT,			// 話しかけから抜ける
	WBR_TALK_SEQ_TRCARD_REQ,	// トレーナーカードリクエスト
	WBR_TALK_SEQ_TRCARD_START,	// トレーナーカード開始
	WBR_TALK_SEQ_TRCARD_ENDWAIT,// トレーナーカード終了待ち
	WBR_TALK_SEQ_TRCARD_END,	// トレーナーカード終了
	WBR_TALK_SEQ_NUM	
} WBR_TALK_SEQ;



#define WBR_MOVEWAIT_TURN	(2)	// 振り向き動作シンク数
#define WBR_MOVEWAIT_WALK	(8)	// 歩き動作シンク数
#define WBR_MOVEWAIT_RUN	(4)	// 走り動作シンク数

//-------------------------------------
///	通信同期定数
//=====================================
enum {
	WBR_COMM_SYNC_START,
	WBR_COMM_SYNC_END,
} ;


//  コマンドキューのバッファ数
#define WBR_CMDQ_BUFFNUM	(16)

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	話しかけリクエスト
//=====================================
typedef struct {
	u16 flag;		// 話しかけ開始or終了	WBR_TALK_REQ_FLAG
	u16 netid;		// 自分のNETID
} WBR_TALK_REQ;


//-------------------------------------
///	話データ
//=====================================
typedef struct {
	u8	netid;			// 自分のID
	u8	talk_mode;		// 話モード（はなしかけたのか　かけられたのか）
	u8	talk_playid;	// 話しかけているプレイヤーID
	u8	talk_seq;		// 話しかけシーケンス
} WBR_TALK_DATA;

//-------------------------------------
///	オブジェクトデータ
//=====================================
typedef struct {
	WF2DMAP_ACTCMD act;
	WBR_TALK_DATA talk_data;
} WBR_OBJ_DATA;

#if 0
//-------------------------------------
///	オブジェクトデータ
//=====================================
typedef struct {
	// 座標
	s16 x;
	s16 y;
	u8 z;
	u8 playid;

	// 動作
	u8	move_flag;		// 動作
	u8	way_flag;		// 方向
	u8	talk_mode;		// 話モード（はなしかけたのか　かけられたのか）
	u8	talk_playid;	// 話しかけているプレイヤーID
	u8	talk_seq;		// 話しかけシーケンス

	u8	pad[1];
} WBR_OBJ_DATA;
#endif

//-------------------------------------
///	 子通信データ
//=====================================
typedef struct {
	WF2DMAP_REQCMD req;
} WBR_KO_COMM_GAME;

#if 0
//-------------------------------------
///	 子通信データ
//=====================================
typedef struct {
	u8	move_flag;
	u8	way_flag;
	u8	pad_0[2];
} WBR_KO_COMM_GAME;
#endif

//-------------------------------------
///	話子通信データ
//=====================================
typedef struct {
	WBR_TALK_DATA talk_data;
} WBR_KO_COMM_TALK;


//-------------------------------------
///	 親通信データ
//=====================================
typedef struct {
	WBR_OBJ_DATA	obj_data;
} WBR_OYA_COMM_GAME;

//-------------------------------------
///	通信基本情報
//	自分の基本的な情報
//	MYSTATUSなどなど
//=====================================
typedef struct {
	TR_CARD_DATA tr_card;
} WBR_COMM_COMMON;


//-------------------------------------
///	MYSTATUSデータ
//=====================================
typedef struct {
	const MYSTATUS* cp_mystatus[ WBR_COMM_NUM ];
} WBR_MYSTATUS;





//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
#undef	GLOBAL
#endif		// __WBR_COMMON_H__

