//==============================================================================
/**
 * @file	balloon_common.h
 * @brief	風船割り：全画面共通ヘッダ
 * @author	matsuda
 * @date	2007.11.06(火)
 */
//==============================================================================
#ifndef __BALLOON_COMMON_H__
#define __BALLOON_COMMON_H__

#include "application/wifi_lobby/minigame_tool.h"


//==============================================================================
//	定数定義
//==============================================================================

///風船ミニゲームのサーバーバージョン
#define BALLOON_SERVER_VERSION			(100)

///風船割りのメインモード
enum{
	BALLOON_MODE_ENTRY,			///<エントリー画面中
	BALLOON_MODE_GAME,			///<ゲーム画面中
	BALLOON_MODE_RESULT,		///<結果発表画面中
};

///風船割りミニゲームの参加最大人数
#define BALLOON_PLAYER_MAX			(WFLBY_MINIGAME_MAX)

///Wifi負荷対策の為、送信データを1回送るごとに次の送信までのウェイトを入れる
#define BALLOON_SIO_SEND_WAIT		(13)


///サブサーフェースY(fx32)
#define BALLOON_SUB_ACTOR_DISTANCE		((192 + 160) << FX32_SHIFT)
///サブサーフェースY(int)
#define BALLOON_SUB_ACTOR_DISTANCE_INTEGER		(BALLOON_SUB_ACTOR_DISTANCE >> FX32_SHIFT)

//--------------------------------------------------------------
//	メイン画面のBGフレーム指定、プライオリティ定義
//--------------------------------------------------------------
#define BALLOON_3DBG_PRIORITY			(2)		///<3D面のBGプライオリティ
///ウィンドウ面のBGプライオリティ
#define BALLOON_BGPRI_WINDOW			(0)
///エフェクト面のBGプライオリティ
#define BALLOON_BGPRI_EFFECT			(1)
///背景面のBGプライオリティ
#define BALLOON_BGPRI_BACKGROUND		(2)

///ウィンドウ・パネル面のフレーム
#define BALLOON_FRAME_WIN				(GF_BGL_FRAME1_M)
///エフェクト面のフレーム
#define BALLOON_FRAME_EFF				(GF_BGL_FRAME2_M)
///観客(背景)面のフレーム
#define BALLOON_FRAME_BACK				(GF_BGL_FRAME3_M)

#define BALLOON_BGNO_WINDOW			(1)					///<BG番号：ウィンドウ
#define BALLOON_BGNO_EFFECT			(2)					///<BG番号：エフェクト
#define BALLOON_BGNO_BACKGROUND		(3)					///<BG番号：背景

//--------------------------------------------------------------
//	サブ画面のBGフレーム指定、プライオリティ定義
//--------------------------------------------------------------
///サブ画面：ウィンドウBG面のBGプライオリティ
#define BALLOON_SUBBG_WIN_PRI			(1)
///サブ画面：パイプBG面のBGプライオリティ
#define BALLOON_SUBBG_PIPE_PRI			(3)
///サブ画面：背景BG面のBGプライオリティ
#define BALLOON_SUBBG_BACK_PRI			(3)
///サブ画面：風船BG面のBGプライオリティ
#define BALLOON_SUBBG_BALLOON_PRI		(0)

///ウィンドウ面のフレーム
#define BALLOON_SUBFRAME_WIN			(GF_BGL_FRAME0_S)
///パイプBG面のフレーム
#define BALLOON_SUBFRAME_PIPE			(GF_BGL_FRAME1_S)
///背景面のフレーム
#define BALLOON_SUBFRAME_BACK			(GF_BGL_FRAME2_S)
///風船BGのフレーム
#define BALLOON_SUBFRAME_BALLOON		(GF_BGL_FRAME3_S)

//--------------------------------------------------------------
//	スクリーンクリアコード
//--------------------------------------------------------------
///パイプ面クリアコード
#define PIPE_BG_CLEAR_CODE		(0)
///ウィンドウ面クリアコード
#define WIN_BG_CLEAR_CODE		(512-1)

//--------------------------------------------------------------
//	風船タイプID
//--------------------------------------------------------------
///風船レベルID
enum{
	BALLOON_LEVEL_1,
	BALLOON_LEVEL_2,
	BALLOON_LEVEL_3,
	
	BALLOON_LEVEL_MAX,
};

//==============================================================================
//	構造体定義
//==============================================================================
///風船割りシステムワーク構造体定義
typedef struct{
	MNGM_ENRES_PARAM entry_param;		///<エントリー画面呼び出しパラメータ
	MNGM_RESULT_PARAM result_param;		///<結果発表画面呼び出しパラメータ

	PROC *sub_proc;						///<実行中のサブプロセス

	u8 player_netid[WFLBY_MINIGAME_MAX];	///<参加しているプレイヤーのnetID
	u8 player_max;							///<参加人数
	
	u8 mode;							///<モード(BALLOON_MODE_???)
	u16 balloon_break_num;				///<割った風船の数

	MNGM_RAREGAME_TYPE raregame_type;	///<レアゲームの種類
	
	//外側からセットされる領域
	BOOL replay;						///<再度挑戦するかどうか(結果発表画面から受け取る)

	u8 vchat;							///<ボイスチャットモード

	u8 dis_error;						///< 切断エラー検知	080527 tomoya 
	u8 dis_error_seq;					///< 切断エラー検知	080625 tomoya 

#ifdef PM_DEBUG
	int debug_offline;		///<TRUE:デバッグ用のオフラインモード
#endif
}BALLOON_SYSTEM_WORK;



//==============================================================================
//	構造体定義：ミニゲーム画面
//==============================================================================
///風船ステータス
typedef struct{
	int occ;			///<有効無効フラグ(TRUE:有効。風船が存在している)
	
	int level;			///<風船レベル
	s32 air;			///<現在の空気量
	s32 max_air;		///<最大耐久力
	s32 air_stack;		///<空気量スタックエリア
	s32 add_air;		///<スタックに貯まっている空気を1フレームで入れる量
	s32 player_air[WFLBY_MINIGAME_MAX];	///<プレイヤー毎の注入した空気量[player_pos指定]
	
	int shake_flag;
	
	u8 bg_on_req;		///<TRUE:BG表示リクエスト
	u8 bg_off_req;		///<TRUE:BG非表示リクエスト
	u8 padding[2];
}BALLOON_STATUS;


///s16のPOINT型
typedef struct{
	s16 x;
	s16 y;
}POINT_S16;

#endif	//__BALLOON_COMMON_H__

