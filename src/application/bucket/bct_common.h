//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		bct_common.h
 *	@brief		共通ヘッダ
 *	@author		tomoya takahashi
 *	@data		2007.06.19
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __BCT_COMMON_H__
#define __BCT_COMMON_H__

#include "savedata/mystatus.h"

// デバック定数
#ifdef PM_DEBUG
//#define BCT_DEBUG_AUTOSLOW		// 自動発射
//#define BCT_DEBUG_NOT_TIMECOUNT	// 時間制限なし
//
//#define BCT_DEBUG			// OS_PRINT	木の実数
//#define BCT_DEBUG_HITZONE	// 当たり判定ゾーンの表示
//#define BCT_DEBUG_TIMESELECT // 時間選択mode
#endif

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
#define BCT_PLAYER_NUM	(4)		// プレイヤー数
#define BCT_SCORE_MAX	(99999)	// スコア最大値
#define BCT_MIDDLE_SCORE_SEND_TIMING	( 30 )	// ～シンクに１回送信する
#define BCT_SPECIAL_BALL_COUNT	(5)	// スペシャルボールになるカウント
#define BCT_NUTS_COUNT_START_BONUS_NUM	(3)	// 木の実カウンタを出す数

// レアゲーム情報
#define BCT_GAMETYPE_PAR_MAX		(100)
#define BCT_GAMETYPE_NORAL_PAR		(75)		// 通常
#define BCT_GAMETYPE_ROT_RET_PAR	(15)		// 逆回転パーセンテージ
#define BCT_GAMETYPE_SCA_RET_PAR	(10)		// スケールパーセンテージ

// 通信同期用
enum{
	BCT_SYNCID_NONE,
	BCT_SYNCID_GAMESTART,
	BCT_SYNCID_CLIENT_TOUCHPEN_END,
	BCT_SYNCID_END,		// 終了同期
	BCT_SYNCID_ERR_END,	// エラーチェック終了同期
};

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	ゲーム前に送信するゲームの情報
//=====================================
typedef struct {
	u8 scale_rev;	// 拡大率の反転
	u8 rota_rev;	// 回転方向の反転
	u8 pad0;
	u8 pad1;
} BCT_GAME_TYPE_WK;


//-------------------------------------
/// 木の実通信
//=====================================
typedef struct{
	u16 pl_no;		// プレイヤーNO
	u16	in_flag;	// 木の実がマルノームに入ったか	4
	VecFx16 way;	// 方位							
	VecFx16 mat;	// 座標
	fx16	power;	// パワー						8
	s8 bonus;		//	ボーナス					12
	u8 special;		// スペシャルボール
} BCT_NUT_COMM;


//-------------------------------------
///	みんなのスコア
//=====================================
typedef struct {
	u32 score[ BCT_PLAYER_NUM ];
} BCT_SCORE_COMM;



//-------------------------------------
/// ゲーム構成データ1テーブルデータ
//=====================================
typedef struct {
	s32	nuts_num;		// このテーブルを参照する範囲の木の実の数
	s32	rota_speed;		// 回転スピード
	s32 camera_ta_y;	// カメラターゲットY座標
	s32	camera_an_x;	// カメラアングルｘ
	s32 pos_z;			// ｚ位置
	s32 size;			// サイズ
} BCT_GAMEDATA_ONE;

//-------------------------------------
///	ゲーム構成データ	
//=====================================
typedef struct {
	u32 tblnum;
	BCT_GAMEDATA_ONE* p_tbl;
	const MYSTATUS* cp_status[ BCT_PLAYER_NUM ];
	u32				vip[ BCT_PLAYER_NUM ];
} BCT_GAMEDATA;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
#endif		// __BCT_COMMON_H__

