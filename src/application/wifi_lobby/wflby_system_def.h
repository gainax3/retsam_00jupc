//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_system_def.h
 *	@brief		WiFiロビーグローバルデータ
 *	@author		tomoya takahashi
 *	@data		2007.11.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WFLBY_SYSTEM_DEF_H__
#define __WFLBY_SYSTEM_DEF_H__

#include "system/gamedata.h"

#include "lobby_news_data.h"
#include "wflby_def.h"

#include "wifi/dwc_lobbylib.h"

#include "application/wifi_p2pmatch_tool.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------


//-------------------------------------
///	ひろば　システム　エラー
//	通信エラーとは違う広場内の状態でのエラー
//=====================================
typedef enum {
	WFLBY_SYSTEM_ERR_NONE,
	WFLBY_SYSTEM_ERR_CRC,		// CRCチェックでエラーが起きた
} WFLBY_SYSTEM_ERR_TYPE;



//-------------------------------------
///	メインチャンネル通信コマンド
//=====================================
enum {

	// 挨拶用
	WFLBY_SYSTEM_MAIN_COMMCMD_TALK_REQ,		// 会話リクエスト
	WFLBY_SYSTEM_MAIN_COMMCMD_TALK_ANS,		// 会話応答
	WFLBY_SYSTEM_MAIN_COMMCMD_TALK_DATA,	// 会話データ
	WFLBY_SYSTEM_MAIN_COMMCMD_TALK_END,		// 会話終了リクエスト

	// トピック用
	WFLBY_SYSTEM_MAIN_COMMCMD_TOPIC_DATA,	// トピックデータ

	// ガジェット用
	WFLBY_SYSTEM_MAIN_COMMCMD_GADGET_DATA,	// ガジェットデータ

	// フロート用
	WFLBY_SYSTEM_MAIN_COMMCMD_FLOAT_ON,		// フロートにのった！
	WFLBY_SYSTEM_MAIN_COMMCMD_FLOAT_ANM,	// フロートアニメ

	
	// GS以降はここから追加

	// 数
	WFLBY_SYSTEM_MAIN_COMMCMD_NUM,
} ;


//-------------------------------------
///	会話シーケンス
//=====================================
typedef enum {
	WFLBY_TALK_SEQ_NONE,	// 何もなし	会話を終了させてください
	WFLBY_TALK_SEQ_B_ANS,	// かけられ側	応答
	WFLBY_TALK_SEQ_A_SEL,	// 話しかけ側	選択
	WFLBY_TALK_SEQ_B_SEL,	// かけられ側	選択
	WFLBY_TALK_SEQ_EXEND,	// 緊急終了	会話を終了させてください
} WFLBY_TALK_SEQ;


//-------------------------------------
///	タイムイベントチェック用定数
//=====================================
typedef enum {
	WFLBY_EVENT_CHG_LOCK,
	WFLBY_EVENT_CHG_NEON_ROOM,
	WFLBY_EVENT_CHG_NEON_FLOOR,
	WFLBY_EVENT_CHG_NEON_MONU,
	WFLBY_EVENT_CHG_MINIGAME,
	WFLBY_EVENT_CHG_FIRE,
	WFLBY_EVENT_CHG_PARADE,
	WFLBY_EVENT_CHG_CLOSE,
} WFLBY_EVENT_CHG_TYPE;

//-------------------------------------
///	タイムイベントチェック花火の状態
//=====================================
typedef enum {
	WFLBY_EVENT_HANABI_BEFORE,	// 花火前
	WFLBY_EVENT_HANABI_PLAY,	// 花火中
	WFLBY_EVENT_HANABI_AFTER,	// 花火後
} WFLBY_EVENT_HANABI_ST;


//-------------------------------------
///	フロートステーション
//=====================================
typedef enum {
	WFLBY_FLOAT_STATION_RIGHT,		// 右
	WFLBY_FLOAT_STATION_CENTER,		// 中
	WFLBY_FLOAT_STATION_LEFT,		// 左
	WFLBY_FLOAT_STATION_NUM,		// 駅数(予約ロックがこの値になったら誰も乗れない)
} WFLBY_FLOAT_STATION_TYPE;

//-------------------------------------
///	フロート動作状況
//=====================================
typedef enum {
	WFLBY_FLOAT_STATE_BEFORE,	// 動作前
	WFLBY_FLOAT_STATE_MOVE,		// 動作中
	WFLBY_FLOAT_STATE_AFTER,	// 動作後
} WFLBY_FLOAT_MOVE_STATE;



//-------------------------------------
///	遊んだ情報保存バッファ
//	遊びタイプ
//
//	上にいくほど、紹介する優先順位が高い
//=====================================
typedef enum {
	WFLBY_PLAYED_BALLTHROW,		// タマ投げ
	WFLBY_PLAYED_BALANCEBALL,	// Balanceボール
	WFLBY_PLAYED_BALLOON,		// ふうせんわり
	WFLBY_PLAYED_WORLDTIMER,	// 世界時計	
	WFLBY_PLAYED_TOPIC,			// 広場ニュース
	WFLBY_PLAYED_FOOTBOARD,		// 足跡BOARD
	WFLBY_PLAYED_ANKETO,		// アンケート
	WFLBY_PLAYED_TALK,			// 話しかけ
	
	WFLBY_PLAYED_NUM,			// 保存する遊びタイプの数
} WFLBY_PLAYED_TYPE;



//-------------------------------------
///	花火構成データ
//=====================================
typedef enum {
	WFLBY_FIRE_END,		// オワリ
	WFLBY_FIRE_NONE,	// 花火なし
	WFLBY_FIRE_NORMAL,	// 通常
	WFLBY_FIRE_KOUGO,	// 交互発射
	WFLBY_FIRE_NAMI,	// 波状発射
	WFLBY_FIRE_CLIMAX,	// 最後
	WFLBY_FIRE_NUM,		// 動作タイプ数
} WFLBY_FIRE_TYPE;
#define WFLBY_FIRE_END_COUNT	( 120 )		// 花火終了時の動きのカウント値






//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	WiFiロビー共通システムワーク
//=====================================
typedef struct _WFLBY_SYSTEM	WFLBY_SYSTEM;




//-------------------------------------
///	ユーザ基本情報
//=====================================
typedef struct _WFLBY_USER_PROFILE WFLBY_USER_PROFILE;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

// ひろばシステム内エラー
extern WFLBY_SYSTEM_ERR_TYPE WFLBY_SYSTEM_GetError( const WFLBY_SYSTEM* cp_wk );

//  広場通信過剰状態エラー
extern BOOL WFLBY_SYSTEM_GetExcessFoodError( const WFLBY_SYSTEM* cp_wk );


// 情報の取得
extern NEWS_DATA* WFLBY_SYSTEM_GetLobbyData( WFLBY_SYSTEM* p_wk );	// ロビーニュース
extern const WFLBY_WLDTIMER* WFLBY_SYSTEM_GetWldTimer( WFLBY_SYSTEM* p_wk );// 世界時計
extern const WFLBY_TIME* WFLBY_SYSTEM_GetWldTime( const WFLBY_SYSTEM* cp_wk );	// 時間


//  フラグ関係
extern BOOL WFLBY_SYSTEM_FLAG_GetUserIn( WFLBY_SYSTEM* p_wk, u32 idx );	// 新しいユーザが入ってきたか
extern BOOL WFLBY_SYSTEM_FLAG_GetUserOut( WFLBY_SYSTEM* p_wk, u32 idx );// ユーザが退席したか
extern BOOL WFLBY_SYSTEM_FLAG_GetEventStart( WFLBY_SYSTEM* p_wk, WFLBY_EVENT_CHG_TYPE type );// イベントが開始したか
extern BOOL WFLBY_SYSTEM_FLAG_GetUserProfileUpdate( WFLBY_SYSTEM* p_wk, u32 idx );// ユーザプロフィールを更新したか
extern SAVEDATA* WFLBY_SYSTEM_GetSaveData( WFLBY_SYSTEM* p_wk );	// セーブデータ取得
extern BOOL WFLBY_SYSTEM_FLAG_GetArceus( const WFLBY_SYSTEM* cp_wk );	// アルセウス表示許可フラグ	TRUE：許可
extern BOOL WFLBY_SYSTEM_FLAG_GetAnketoInput( const WFLBY_SYSTEM* cp_wk );	// アンケートに答えたかどうか
extern BOOL WFLBY_SYSTEM_FLAG_GetAnketoView( const WFLBY_SYSTEM* cp_wk );	// アンケートの結果を見たかどうか
extern void WFLBY_SYSTEM_FLAG_SetAnketoView( WFLBY_SYSTEM* p_wk );	// アンケートの結果を見たかどうか


// イベント状態取得
extern BOOL WFLBY_SYSTEM_Event_GetLock( const WFLBY_SYSTEM* cp_wk );
extern BOOL WFLBY_SYSTEM_Event_GetEndCM( const WFLBY_SYSTEM* cp_wk );
extern BOOL WFLBY_SYSTEM_Event_GetEnd( const WFLBY_SYSTEM* cp_wk );
extern WFLBY_EVENT_HANABI_ST WFLBY_SYSTEM_Event_GetHanabi( const WFLBY_SYSTEM* cp_wk );
extern BOOL WFLBY_SYSTEM_Event_GetParade( const WFLBY_SYSTEM* cp_wk );
extern BOOL WFLBY_SYSTEM_Event_GetMiniGameStop( const WFLBY_SYSTEM* cp_wk );
extern BOOL WFLBY_SYSTEM_Event_GetNeon( const WFLBY_SYSTEM* cp_wk );
extern PPW_LOBBY_TIME_EVENT WFLBY_SYSTEM_Event_GetNeonRoom( const WFLBY_SYSTEM* cp_wk );
extern PPW_LOBBY_TIME_EVENT WFLBY_SYSTEM_Event_GetNeonFloor( const WFLBY_SYSTEM* cp_wk );
extern PPW_LOBBY_TIME_EVENT WFLBY_SYSTEM_Event_GetNeonMonu( const WFLBY_SYSTEM* cp_wk );


// ミニゲームフラグ
extern void WFLBY_SYSTEM_MGFLAG_SetLock( WFLBY_SYSTEM* p_wk );
extern BOOL WFLBY_SYSTEM_MGFLAG_GetLock( const WFLBY_SYSTEM* cp_wk );


extern BOOL WFLBY_SYSTEM_MG_CheckErrorEnd( const WFLBY_SYSTEM* cp_wk );
extern void WFLBY_SYSTEM_MG_SetErrorEnd( WFLBY_SYSTEM* p_wk, BOOL flag );


// ミニゲーム通信相手のプレイヤーIDX
extern void WFLBY_SYSTEM_PLIDX_Clear( WFLBY_SYSTEM* p_wk );
extern void WFLBY_SYSTEM_PLIDX_Set( WFLBY_SYSTEM* p_wk, s32 userid, u32 netid );
extern void WFLBY_SYSTEM_PLIDX_SetMyData( WFLBY_SYSTEM* p_wk, u32 netid );
extern void WFLBY_SYSTEM_PLIDX_Get( const WFLBY_SYSTEM* cp_wk, WFLBY_MINIGAME_PLIDX* p_buff );


// アプリ状態フラグ
// アプリを強制終了させたのかを設定する
extern void WFLBY_SYSTEM_APLFLAG_SetForceEnd( WFLBY_SYSTEM* p_wk );
extern BOOL WFLBY_SYSTEM_APLFLAG_GetForceEnd( const WFLBY_SYSTEM* cp_wk );


// BGMを再生
extern void WFLBY_SYSTEM_SetLockBGMChange( WFLBY_SYSTEM* p_wk, BOOL flag );
extern void WFLBY_SYSTEM_SetNowBGM( const WFLBY_SYSTEM* cp_wk );
extern void WFLBY_SYSTEM_SetBGMVolumeDown( WFLBY_SYSTEM* p_wk, BOOL flag );


// ユーザプロフィール
// ユーザプロフィールのポインタをローカルワークに保存しないこと、
// プロフィールの中身を参照するときは、参照するごとにWFLBY_SYSTEM_GetUserProfile関数などで
// プロフィールポインタを取得してください。
// そうしないともう退室した人のメモリをクリアされたプロフィールを参照する可能性が出るためです。
extern u32 WFLBY_SYSTEM_GetMyIdx( const WFLBY_SYSTEM* cp_wk );	// 自分のバッファインデックス
extern s32 WFLBY_SYSTEM_GetMyUserID( const WFLBY_SYSTEM* cp_wk );
extern BOOL WFLBY_SYSTEM_GetUserRoomIn( const WFLBY_SYSTEM* cp_wk, u32 idx );	// 存在しているかチェック
extern const WFLBY_USER_PROFILE* WFLBY_SYSTEM_GetUserProfile( const WFLBY_SYSTEM* cp_wk, u32 idx );
extern const WFLBY_USER_PROFILE* WFLBY_SYSTEM_GetMyProfile( const WFLBY_SYSTEM* cp_wk );		// DWCLobbyライブラリ内から直接取得(ライブラリから更新されるもの)
extern WFLBY_USER_PROFILE* WFLBY_SYSTEM_GetMyProfileLocal( WFLBY_SYSTEM* p_wk );				// WFLBY_SYSLTEM内にとってある物を取得（最新）
extern void WFLBY_SYSTEM_SetMyProfile( WFLBY_SYSTEM* p_wk, const WFLBY_USER_PROFILE* cp_data );
extern void WFLBY_SYSTEM_SetMyStatus( WFLBY_SYSTEM* p_wk, WFLBY_STATUS_TYPE status );
extern void WFLBY_SYSTEM_SetLastAction( WFLBY_SYSTEM* p_wk, WFLBY_LASTACTION_TYPE action, s32 userid );
extern void WFLBY_SYSTEM_SetMyItem( WFLBY_SYSTEM* p_wk, WFLBY_ITEMTYPE item );
extern void WFLBY_SYSTEM_SetSelectWazaType( WFLBY_SYSTEM* p_wk, WFLBY_POKEWAZA_TYPE type1, WFLBY_POKEWAZA_TYPE type2 );
extern void WFLBY_SYSTEM_SetAnketoData( WFLBY_SYSTEM* p_wk, u32 anketo_no, u32 select );
extern void WFLBY_SYSTEM_GetProfileMyStatus( const WFLBY_USER_PROFILE* cp_profile, MYSTATUS* p_buff, u32 heapID );
extern u32 WFLBY_SYSTEM_GetProfileStatus( const WFLBY_USER_PROFILE* cp_profile );
extern WFLBY_LASTACTION_TYPE WFLBY_SYSTEM_GetProfileLastAction( const WFLBY_USER_PROFILE* cp_profile, u32 idx );
extern s32 WFLBY_SYSTEM_GetProfileLastActionUserID( const WFLBY_USER_PROFILE* cp_profile, u32 idx );
extern s32 WFLBY_SYSTEM_GetProfileUserID( const WFLBY_USER_PROFILE* cp_profile );
extern u32 WFLBY_SYSTEM_GetProfileTrID( const WFLBY_USER_PROFILE* cp_profile );
extern WFLBY_TIME WFLBY_SYSTEM_GetProfileInTime( const WFLBY_USER_PROFILE* cp_profile );
extern WFLBY_TIME WFLBY_SYSTEM_GetProfileWldTime( const WFLBY_USER_PROFILE* cp_profile );
extern u32 WFLBY_SYSTEM_GetProfileSex( const WFLBY_USER_PROFILE* cp_profile );
extern u32 WFLBY_SYSTEM_GetProfileRagionCode( const WFLBY_USER_PROFILE* cp_profile );
extern BOOL WFLBY_SYSTEM_CheckProfileRagionCode( const WFLBY_USER_PROFILE* cp_profile );
extern u32 WFLBY_SYSTEM_GetProfileRagionCodeOrg( const WFLBY_USER_PROFILE* cp_profile );	// 使用時注意！！！！　メンバ値をそのまま取得
 
extern u32 WFLBY_SYSTEM_GetProfileTrType( const WFLBY_USER_PROFILE* cp_profile );
extern WFLBY_LASTACTION_TYPE WFLBY_SYSTEM_GetProfileTrTypetoLastActionNo( const WFLBY_USER_PROFILE* cp_profile );
extern u32 WFLBY_SYSTEM_GetProfileNation( const WFLBY_USER_PROFILE* cp_profile );
extern u32 WFLBY_SYSTEM_GetProfileArea( const WFLBY_USER_PROFILE* cp_profile );
extern BOOL WFLBY_SYSTEM_GetProfileZukanZenkoku( const WFLBY_USER_PROFILE* cp_profile );
extern BOOL WFLBY_SYSTEM_GetProfileGameClear( const WFLBY_USER_PROFILE* cp_profile );
extern BOOL WFLBY_SYSTEM_CheckGetItem( const WFLBY_USER_PROFILE* cp_profile );
extern WFLBY_ITEMTYPE WFLBY_SYSTEM_GetProfileItem( const WFLBY_USER_PROFILE* cp_profile );
extern s64 WFLBY_SYSTEM_GetProfileGameStartTime( const WFLBY_USER_PROFILE* cp_profile );
extern BOOL WFLBY_SYSTEM_CheckProfileGameStartTime( const WFLBY_USER_PROFILE* cp_profile );
extern WFLBY_POKEWAZA_TYPE WFLBY_SYSTEM_GetProfileWazaType( const WFLBY_USER_PROFILE* cp_profile, u32 idx );
extern void WFLBY_SYSTEM_GetProfileAnketoData( const WFLBY_USER_PROFILE* cp_profile, WFLBY_ANKETO* p_data );


// ミニゲーム専用MYSTATUSデータ
extern MYSTATUS* WFLBY_SYSTEM_GetMgMyStatus( const WFLBY_SYSTEM* cp_wk );


// VIPフラグの取得
extern BOOL WFLBY_SYSTEM_GetUserVipFlag( const WFLBY_SYSTEM* cp_wk, u32 idx );
extern void WFLBY_SYSTEM_GetVipFlagWk( const WFLBY_SYSTEM* cp_wk, WFLBY_VIPFLAG* p_buff );

// あいことばフラグの取得
extern BOOL WFLBY_SYSTEM_GetUserAikotobaFlag( const WFLBY_SYSTEM* cp_wk, u32 idx );

// あいことばメッセージの取得
extern const WFLBY_AIKOTOBA_DATA* WFLBY_SYSTEM_GetUserAikotobaStr( const WFLBY_SYSTEM* cp_wk, u32 idx );


// サブチャンネルへの参加情報作成
extern u32 WFLBY_SYSTEM_GetSubChanPlNum( const WFLBY_SYSTEM* cp_wk );
extern u32 WFLBY_SYSTEM_GetSubChanEnum( const WFLBY_SYSTEM* cp_wk, u32 idx );


// トピック送信
extern void WFLBY_SYSTEM_TOPIC_SendConnect( WFLBY_SYSTEM* p_wk, u32 user_a, u32 user_b );
extern void WFLBY_SYSTEM_TOPIC_SendItem( WFLBY_SYSTEM* p_wk, u32 user_a, u32 user_b, WFLBY_ITEMTYPE item );
extern void WFLBY_SYSTEM_TOPIC_SendMiniGame( WFLBY_SYSTEM* p_wk, WFLBY_GAMETYPE minigame, u32 num, u32 user_0, u32 user_1, u32 user_2, u32 user_3, BOOL play );
extern void WFLBY_SYSTEM_TOPIC_SendMiniGameTopResult( WFLBY_SYSTEM* p_wk, WFLBY_GAMETYPE minigame, u32 num, u32 user_0, u32 user_1, u32 user_2, u32 user_3 );


// ゲームデータ
extern u32 WFLBY_SYSTEM_GetSeason( const WFLBY_SYSTEM* cp_wk );
extern u32 WFLBY_SYSTEM_GetRoomType( const WFLBY_SYSTEM* cp_wk );
extern u32 WFLBY_SYSTEM_GetLockTime( const WFLBY_SYSTEM* cp_wk );	// ロック時間
extern u32 WFLBY_SYSTEM_GetCloseTime( const WFLBY_SYSTEM* cp_wk );	// ロックがかかってからクローズまでの時間


// ガジェット
extern BOOL WFLBY_SYSTEM_GetGadGetFlag( const WFLBY_SYSTEM* cp_wk, u32 idx ); 
extern void WFLBY_SYSTEM_SendGadGetData( WFLBY_SYSTEM* p_wk, WFLBY_ITEMTYPE gadget );


// フロートデータ
// idxはフォロート座席のインデックス
// plidxはプレイヤーインデックス
// floatはプレイヤーインデックス
extern WFLBY_FLOAT_MOVE_STATE WFLBY_SYSTEM_GetFloatMove( const WFLBY_SYSTEM* cp_wk, u32 floatidx );
extern u32 WFLBY_SYSTEM_GetFloatCount( const WFLBY_SYSTEM* cp_wk, u32 floatidx );
extern u32 WFLBY_SYSTEM_GetFloatCountMax( const WFLBY_SYSTEM* cp_wk );
extern u32 WFLBY_SYSTEM_GetFloatReserve( const WFLBY_SYSTEM* cp_wk, u32 idx );
extern u32 WFLBY_SYSTEM_GetFloatPlIdxReserve( const WFLBY_SYSTEM* cp_wk, u32 plidx );
extern u32 WFLBY_SYSTEM_SetFloatReserve( WFLBY_SYSTEM* p_wk, u32 plidx, WFLBY_FLOAT_STATION_TYPE station );
extern void WFLBY_SYSTEM_ResetFloatReserve( WFLBY_SYSTEM* p_wk, u32 plidx );
extern void WFLBY_SYSTEM_SetFloatAnm( WFLBY_SYSTEM* p_wk, u32 plidx );
extern BOOL WFLBY_SYSTEM_GetFloatAnm( const WFLBY_SYSTEM* cp_wk, u32 idx );
extern void WFLBY_SYSTEM_CleanFloatAnm( WFLBY_SYSTEM* p_wk, u32 idx );
extern void WFLBY_SYSTEM_GetFloatIdxOfs( u32 idx, u32* p_floatidx, u32* p_floatofs );
extern void WFLBY_SYSTEM_SetFloatReserveLock( WFLBY_SYSTEM* p_wk, u32 idx, WFLBY_FLOAT_STATION_TYPE station );
extern WFLBY_FLOAT_STATION_TYPE WFLBY_SYSTEM_GetFloatReserveLock( const WFLBY_SYSTEM* cp_wk, u32 idx );
extern void WFLBY_SYSTEM_SendFloatOn( WFLBY_SYSTEM* p_wk );
extern void WFLBY_SYSTEM_SendFloatAnm( WFLBY_SYSTEM* p_wk );

// 会話データ
// 完全にクリーンな状態にする
extern void WFLBY_SYSTEM_EndTalk( WFLBY_SYSTEM* p_wk );	// 
// 送信
extern BOOL WFLBY_SYSTEM_SendTalkReq( WFLBY_SYSTEM* p_wk, u32 idx );			// 会話リクエスト
extern void WFLBY_SYSTEM_SendTalkData( WFLBY_SYSTEM* p_wk, u16 data );	// 会話データを送る
extern void WFLBY_SYSTEM_SendTalkEnd( WFLBY_SYSTEM* p_wk );			// 会話終了
extern void WFLBY_SYSTEM_SendTalkExEnd( WFLBY_SYSTEM* p_wk );		// 強制終了したことを教える

// 取得
extern BOOL WFLBY_SYSTEM_CheckTalk( const WFLBY_SYSTEM* cp_wk );
extern BOOL WFLBY_SYSTEM_CheckTalkReq( const WFLBY_SYSTEM* cp_wk );			// 話しかけられたか
extern BOOL WFLBY_SYSTEM_GetTalkMsk( const WFLBY_SYSTEM* cp_wk, u32 idx );	// 話しかけたことがあるかチェック
extern u32 WFLBY_SYSTEM_GetTalkIdx( const WFLBY_SYSTEM* cp_wk );			// 会話相手
extern BOOL WFLBY_SYSTEM_GetTalkFirst( const WFLBY_SYSTEM* cp_wk );
extern WFLBY_TALK_SEQ WFLBY_SYSTEM_GetTalkSeq( const WFLBY_SYSTEM* cp_wk );	// 会話シーケンス
extern u16 WFLBY_SYSTEM_GetTalkRecv( const WFLBY_SYSTEM* cp_wk );			// 会話選択情報取得
extern BOOL WFLBY_SYSTEM_CheckTalkCount( const WFLBY_SYSTEM* cp_wk, u32 idx );	// 会話できるか取得
extern BOOL WFLBY_SYSTEM_CheckTalkError( const WFLBY_SYSTEM* cp_wk );	// エラー状態チェック	エラーになったら会話終了処理へ
extern s16	WFLBY_SYSTEM_GetRecvWaitTime( const WFLBY_SYSTEM* cp_wk );	// 受信待ち時間を取得する
extern void WFLBY_SYSTEM_SetTalkBStart( WFLBY_SYSTEM* p_wk );			// 話しかけた側がそばにきたので掛けられた側は会話開始
extern BOOL WFLBY_SYSTEM_CheckTalkBStart( const WFLBY_SYSTEM* cp_wk );	// 話しかけられた場合、話しかけ相手が自分のそばに来たか取得
//  会話回数のカウント
extern void WFLBY_SYSTEM_AddTalkCount( WFLBY_SYSTEM* p_wk );	// 会話成立した回数をカウント
extern void WFLBY_SYSTEM_AddTalkCountEx( WFLBY_SYSTEM* p_wk, u32 idx );	// 会話成立した回数をカウント
// 会話したことあるかマスク
extern void WFLBY_SYSTEM_SetTalkMsk( WFLBY_SYSTEM* p_wk, u32 idx );	// 会話したことあるの設定をする

// 冒険ノートデータの設定
extern void WFLBY_SYSTEM_FNOTE_SetTalk( WFLBY_SYSTEM* p_wk, u32 plidx );
extern void WFLBY_SYSTEM_FNOTE_SetGetGadget( WFLBY_SYSTEM* p_wk, u32 plidx );
extern void WFLBY_SYSTEM_FNOTE_SetPlayMinigame( WFLBY_SYSTEM* p_wk, WFLBY_GAMETYPE minigame );
extern void WFLBY_SYSTEM_FNOTE_SetParade( WFLBY_SYSTEM* p_wk );

// レコードデータの設定
extern void WFLBY_SYSTEM_RECORD_AddTalk( WFLBY_SYSTEM* p_wk );
extern void WFLBY_SYSTEM_RECORD_AddMinigame( WFLBY_SYSTEM* p_wk );


// 遊んだデータの保存と取得
extern void WFLBY_SYSTEM_PLAYED_SetPlay( WFLBY_SYSTEM* p_wk, WFLBY_PLAYED_TYPE type );
extern WFLBY_PLAYED_TYPE WFLBY_SYSTEM_PLAYED_GetOsusumePlayType( const WFLBY_SYSTEM* cp_wk );

// ガジェット取得
extern WFLBY_ITEMTYPE WFLBY_SYSTEM_RandItemGet( const WFLBY_SYSTEM* cp_wk );


// 花火情報
extern WFLBY_FIRE_TYPE WFLBY_SYSTEM_FIRE_GetType( const WFLBY_SYSTEM* cp_wk );
extern u32 WFLBY_SYSTEM_FIRE_GetTypeMoveCount( const WFLBY_SYSTEM* cp_wk );

// 広場内のアンケート結果を保存する
extern void WFLBY_SYSTEM_SetAnketoResult( WFLBY_SYSTEM* p_wk, const ANKETO_QUESTION_RESULT* cp_data );
extern BOOL WFLBY_SYSTEM_GetAnketoResult( const WFLBY_SYSTEM* cp_wk, ANKETO_QUESTION_RESULT* p_data );



// Wi-Fiクラブの状態をNONEにするためのバッファ
extern void WFLBY_SYSTEM_WiFiClubBuff_Init( WFLBY_SYSTEM* p_wk );
extern void WFLBY_SYSTEM_WiFiClubBuff_Exit( WFLBY_SYSTEM* p_wk );

#ifdef PM_DEBUG
extern void WFLBY_SYSTEM_DEBUG_SetItem( WFLBY_SYSTEM* p_wk, u32 item );
extern void WFLBY_SYSTEM_DEBUG_SetLangCode( WFLBY_USER_PROFILE* p_profile, u32 code );
#endif

#endif		// __WFLBY_SYSTEM_DEF_H__

