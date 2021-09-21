/**
 *	@file	btower_scr.h
 *	@brief	バトルタワー　スクリプト系
 *	@author	Miyuki Iwasawa
 *	@date	06.04.17
 */

#ifndef __H_B_TOWER_SCR_H__
#define __H_B_TOWER_SCR_H__

#ifdef GLOBAL
#undef GLOBAL
#endif

#ifdef __B_TOWER_SCR_H_GLOBAL
#define GLOBAL	/***/
#else
#define GLOBAL	extern
#endif

#include "b_tower_scr_def.h"
#include "tower_scr_common.h"

///スクリプトコマンドから呼び出すルーチン群
///////////////////////////////////////////////////////
/**
 *	@brief	プレイモード別のメンバー数を取得する
 */
GLOBAL u16	TowerScrTools_GetMemberNum(u16 mode);

/**
 *	@brief	バトルタワー　参加可能なポケモン数のチェック
 *
 *	@param	num			参加に必要なポケモン数
 *	@param	item_flag	アイテムチェックするかフラグ
 *
 *	@retval	true	参加可能
 *	@retval	false	参加不可
 */
//GLOBAL BOOL TowerScrTools_CheckEntryPokeNum(u16 num,SAVEDATA *savedata);
GLOBAL BOOL TowerScrTools_CheckEntryPokeNum(u16 num,SAVEDATA *savedata,u8 item_flag);

/**
 *	@brief	リセットしてタイトルに戻る
 */
GLOBAL void TowerScrTools_SystemReset(void);

/**
 *	@brief	new game時にプレイデータをクリアする
 */
GLOBAL void TowerScrTools_ClearPlaySaveData(BTLTOWER_PLAYWORK* playSave);

/**
 * @brief	セーブデータが有効かどうか返す
 */
GLOBAL BOOL TowerScrTools_IsSaveDataEnable(BTLTOWER_PLAYWORK* playSave);

/**
 *	@brief	脱出用に現在のロケーションを記憶する
 */
GLOBAL void TowerScrTools_PushNowLocation(FIELDSYS_WORK* fsys);

/**
 *	@brief	ゲートから出る時に特殊接続先を元に戻す
 */
GLOBAL void TowerScrTools_PopNowLocation(FIELDSYS_WORK* fsys);

/**
 *	@brief	入ったゲートの位置を取得する
 */
GLOBAL void TowerScrTools_GetGateLocation(FIELDSYS_WORK* fsys,u16* x,u16* y);

/**
 *	@brief	プレイモード別レコードセーブデータ連勝数を返す
 */
GLOBAL u16	TowerScrTools_GetRenshouRecord(SAVEDATA* sv,u16 play_mode);

/**
 *	@brief	スコアデータのフラグを返す
 */
GLOBAL u16 TowerScrTools_GetScoreFlags(SAVEDATA* sv,BTWR_SFLAG_ID flagid);

/**
 *	@brief	Wifi成績の未アップロードフラグを制御する
 */
GLOBAL void TowerScrTools_SetWifiUploadFlag(SAVEDATA* sv,u8 flag);

/**
 *	@brief	WiFiプレイ成績の未アップロードフラグを取得する
 */
GLOBAL u16 TowerScrTools_GetWifiUploadFlag(SAVEDATA* sv);

/**
 *	@brief	セーブせずに止めたときのエラー処理
 *
 *	@return	プレイしていたモードを返す
 */
GLOBAL u16 TowerScrTools_SetNGScore(SAVEDATA* savedata);

/**
 *	@brief	WIFIのプレイヤーDLデータがあるかどうかチェック
 */
GLOBAL u16	TowerScrTools_IsPlayerDataEnable(SAVEDATA* sv);


//*****************************************************
//=====================================================
//ワークの初期化と解放
//=====================================================
//*****************************************************
/**
 *	@brief	fsys内のバトルタワー制御ワークポインタを初期化する
 *			バトルタワー開始時に必ず呼び出す
 */
GLOBAL void TowerScr_WorkClear(BTOWER_SCRWORK** pp);

/**
 *	@brief	ワークエリアを取得して初期化する
 *
 *	@param	savedata	セーブデータへのポインタ
 *	@param	init		初期化モード BTWR_PLAY_NEW:初めから、BTWR_PLAY_CONTINE:続きから
 *	@param	playmode	プレイモード指定 BTWR_MODE_～
 *	
 *	＊かならず TowerScr_WorkRelease()で領域を開放すること
 */
GLOBAL BTOWER_SCRWORK* TowerScr_WorkInit(SAVEDATA* savedata,u16 init,u16 playmode);

/**
 *	@brief	ワークエリアを開放する
 */
GLOBAL void TowerScr_WorkRelease(BTOWER_SCRWORK* wk);


/**
 *	@brief	バトルタワー用ポケモン選択呼出し
 */
GLOBAL void TowerScr_SelectPoke(BTOWER_SCRWORK* wk,
				GMEVENT_CONTROL *ev_work,void** proc_wk);

/**
 *	@brief	選択したポケモンを取得
 *	
 *	@retval	TRUE	選択した
 *	@retval	FALSE	選択せずにやめた
 */
GLOBAL BOOL TowerScr_GetEntryPoke(BTOWER_SCRWORK* wk,void** app_work,SAVEDATA* sv);

/**
 *	@brief	参加指定した手持ちポケモンの条件チェック
 *
 *	@retval	0	参加OK
 *	@retval	1	同じポケモンがいる
 *	@retval 2	同じアイテムを持っている
 */
GLOBAL int TowerScr_CheckEntryPoke(BTOWER_SCRWORK* wk,SAVEDATA* savedata);

/**
 *	@brief　対戦トレーナーNo抽選
 */
GLOBAL void TowerScr_BtlTrainerNoSet(BTOWER_SCRWORK* wk,SAVEDATA* sv);

/**
 *	@brief	現在のラウンド数をスクリプトワークに取得する
 */
GLOBAL u16	TowerScr_GetNowRound(BTOWER_SCRWORK* wk);

/**
 *	@brief	ラウンド数をインクリメント
 */
GLOBAL u16	TowerScr_IncRound(BTOWER_SCRWORK* wk);

/**
 *	@brief	7連勝しているかどうかチェック
 */
GLOBAL BOOL TowerScr_IsClear(BTOWER_SCRWORK* wk);

/**
 *	@brief	現在の連勝数を取得する
 */
GLOBAL u16 TowerScr_GetRenshouCount(BTOWER_SCRWORK* wk);

/**
 *	@brief	敗戦処理	
 */
GLOBAL void TowerScr_SetLoseScore(BTOWER_SCRWORK* wk,SAVEDATA* savedata);

/**
 *	@brief	クリア処理
 */
GLOBAL void TowerScr_SetClearScore(BTOWER_SCRWORK* wk,SAVEDATA* savedata, FNOTE_DATA* fnote);


/**
 *	@brief	休むときに現在のプレイ状況をセーブに書き出す
 */
GLOBAL void TowerScr_SaveRestPlayData(BTOWER_SCRWORK* wk);


/**
 *	@brief	対戦トレーナー抽選
 */
GLOBAL void TowerScr_ChoiceBtlPartner(BTOWER_SCRWORK* wk,SAVEDATA* sv);
/**
 *	@brief	AIマルチペアポケモン抽選
 */
GLOBAL void TowerScr_ChoiceBtlSeven(BTOWER_SCRWORK* wk);

/**
 *	@brief	対戦トレーナーOBJコード取得
 */
GLOBAL u16 TowerScr_GetEnemyObj(BTOWER_SCRWORK* wk,u16 idx);

/**
 *	@brief	戦闘呼び出し
 */
GLOBAL void TowerScr_LocalBattleCall(GMEVENT_CONTROL* event,BTOWER_SCRWORK* wk,BOOL* win_flag);

/**
 *	@brief	現在のプレイモードを返す
 */
GLOBAL u16	TowerScr_GetPlayMode(BTOWER_SCRWORK* wk);

/**
 *	@brief	リーダークリアフラグを立てる
 */
GLOBAL void TowerScr_SetLeaderClearFlag(BTOWER_SCRWORK* wk,u16 mode);

/**
 *	@brief	リーダークリアフラグを取得する
 */
GLOBAL	u16	TowerScr_GetLeaderClearFlag(BTOWER_SCRWORK* wk);

/**
 *	@brief	タワークリア時にバトルポイントを加算する
 *
 *	@return	新たに取得したバトルポイント
 */
GLOBAL u16	TowerScr_AddBtlPoint(BTOWER_SCRWORK* wk);

/**
 *	@brief	シングルで連勝した時のご褒美のトロフィーをあげるフラグを立てておく
 */
GLOBAL	u16	TowerScr_GoodsFlagSet(BTOWER_SCRWORK* wk,SAVEDATA* sv);

/**
 *	@brief	現在のWIFIランクを操作して返す
 */
GLOBAL u16	TowerScr_SetWifiRank(BTOWER_SCRWORK* wk,SAVEDATA* sv,u8 mode);


/**
 *	@brief	リーダーを倒したご褒美リボンをあげる
 */
GLOBAL u16 TowerScr_LeaderRibbonSet(BTOWER_SCRWORK* wk,SAVEDATA* sv);

/**
 *	@brief	連勝のご褒美リボンをあげる	
 */
GLOBAL u16 TowerScr_RenshouRibbonSet(BTOWER_SCRWORK* wk,SAVEDATA* sv);

/**
 *	@biref	プレイランダムシードを更新する
 */
GLOBAL u16 TowerScr_PlayRandUpdate(BTOWER_SCRWORK* wk,SAVEDATA* sv);


//=============================================================================================
//
//	フロンティアとフィールドで共通で使用するものを移動
//
//=============================================================================================

//---------------------------------------------------------------------------------------------
/**
 *	バトルタワートレーナーの持ちポケモンのパワー乱数を決定する
 *
 * @param	tr_no	トレーナーナンバー
 *
 * @return	パワー乱数
 *
 * b_tower_fld.c→b_tower_scrに移動
 */
//---------------------------------------------------------------------------------------------
GLOBAL u8	BattleTowerPowRndGet(u16 tr_no);

//---------------------------------------------------------------------------------------------
/**
 * @brief	タワープレイ更新ランダムシード取得 ラウンド更新時に呼ぶ
 *
 * b_tower_fld.c→b_tower_scrに移動
 */
//---------------------------------------------------------------------------------------------
GLOBAL u16	btower_rand(BTOWER_SCRWORK* wk);


#endif	//__H_BTOWER_SCR_H__

