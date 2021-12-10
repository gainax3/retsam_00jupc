//============================================================================================
/**
 * @file	scr_tool.h
 * @bfief	スクリプトで使用するプログラム
 * @author	Satoshi Nohara
 * @date	05.11.16
 */
//============================================================================================
#ifndef __SCR_TOOL_H__
#define __SCR_TOOL_H__

#include "fieldobj.h"

//============================================================================================
//
//	extern宣言
//
//============================================================================================
#ifdef PM_DEBUG
extern u8 debug_frontier_key_set;
#endif

//============================================================================================
//
//	その他
//
//============================================================================================

//--------------------------------------------------------------
/**
 * 渡された値の桁数を取得
 *
 * @param   num			値
 *
 * @retval  "桁数"
 */
//--------------------------------------------------------------
extern u16 GetNumKeta(u32 num);

//--------------------------------------------------------------
/**
 * 技マシンのアイテムナンバーかチェック
 *
 * @param   itemno		アイテムナンバー
 *
 * @retval  TRUE		技マシン
 * @retval  FALSE		それ以外
 */
//--------------------------------------------------------------
extern u16 WazaMachineItemNoCheck( u16 itemno );


//============================================================================================
//
//	図鑑評価
//
//============================================================================================

//--------------------------------------------------------------
/**
 * シンオウ図鑑評価メッセージID取得
 *
 * @param   num		隠しを除いた見た数
 * @param	c04_arrive	c04ハクタイシティの到着フラグ
 *
 * @retval  "メッセージID"
 */
 //--------------------------------------------------------------
extern u16 GetShinouZukanHyoukaMsgID( u16 num ,u16 c04_arrive);

//--------------------------------------------------------------
/**
 * 評価メッセージID取得
 *
 * @param   num		隠しを除いた捕まえた数
 * @param	sex		主人公の性別
 *
 * @retval  "メッセージID"
 */
//--------------------------------------------------------------
extern u16 GetZenkokuZukanHyoukaMsgID( u16 num ,u16 sex);


//==============================================================================================
//
//	全滅関連
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	ゲームオーバー画面呼び出し
 *
 * @param	fsys	FIELDSYS_WORK型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
extern void GameOverCall( FIELDSYS_WORK* fsys, GMEVENT_CONTROL* event );



//--------------------------------------------------------------
/**
 * 先頭ポケモン算出
 *
 * @param   sv		ポケモンセーブ情報構造体
 *
 * @retval  番号
 */
//--------------------------------------------------------------
extern u16 GetFrontPokemon(SAVEDATA * sv);
//--------------------------------------------------------------
/**
 *	@brief	レジキングイベント　キング解放できるかチェック
 *
 *	手持ちにアイス・ロック・スチルがいるかどうか？
 */
//--------------------------------------------------------------
extern BOOL EventCheck_IsReziUnseal(SAVEDATA* sv);

//--------------------------------------------------------------
/**
 * @brief	フィールドオブジェを揺らすイベント 
 * @param	ev	GMEVENT_CONTROL *
 * @param	obj	FIELD_OBJ_PTR	フィールドオブジェへのポインタ
 * @param	time	揺らす回数
 * @param	spd		揺らすスピード(360を割り切れる数字がよい)
 * @param	ofsx	揺らす幅X	
 * @param	ofsz	揺らす幅Z	
 * @retval	BOOL	TRUE=イベント終了
 */
//--------------------------------------------------------------
extern void EventCmd_ObjShakeAnm( GMEVENT_CONTROL *event,FIELD_OBJ_PTR obj,
		u16 time,u16 spd,u16 ofsx,u16 ofsz);

//--------------------------------------------------------------
/**
 * @brief	フィールドオブジェをBlinkさせるアニメイベント 
 * @param	ev		GMEVENT_CONTROL *
 * @param	obj		FIELD_OBJ_PTR	フィールドオブジェへのポインタ
 * @param	count	Blinkさせる回数
 * @param	time	onoffを切り替えるインターバル
 */
//--------------------------------------------------------------
extern void EventCmd_ObjBlinkAnm(GMEVENT_CONTROL *event,FIELD_OBJ_PTR obj,u16 count,u16 time);


//============================================================================================
//
//	ファクトリーで使用(frontier,fieldで使用される)
//
//============================================================================================
extern int FactoryScr_GetWinRecordID( u8 level, u8 type );
extern int FactoryScr_GetMaxWinRecordID( u8 level, u8 type );
extern int FactoryScr_GetTradeRecordID( u8 level, u8 type );
extern int FactoryScr_GetMaxTradeRecordID( u8 level, u8 type );


//============================================================================================
//
//	バトルステージで使用(frontier,fieldで使用される)
//
//============================================================================================
extern int StageScr_GetWinRecordID( u8 type );
extern int StageScr_GetMaxWinRecordID( u8 type );
extern int StageScr_GetMonsNoRecordID( u8 type );
extern int StageScr_GetExMaxWinRecordID( u8 type );
extern int StageScr_GetTypeLevelRecordID( u8 type, u8 csr_pos );

//タイプレベルのレコードを取得する
extern u16 StageScr_TypeLevelRecordGet( SAVEDATA* sv, u8 type, u8 csr_pos, u16* l_num, u16* h_num );

//タイプレベルのレコードをセットする
extern void StageScr_TypeLevelRecordSet( SAVEDATA* sv, u8 type, u8 csr_pos, u8 num );


//============================================================================================
//
//	バトルキャッスルで使用(frontier,fieldで使用される)
//
//============================================================================================
extern int CastleScr_GetRankRecordID( u8 type, u8 id );
extern int CastleScr_GetWinRecordID( u8 type );
extern int CastleScr_GetMaxWinRecordID( u8 type );
extern int CastleScr_GetCPRecordID( u8 type );
extern int CastleScr_GetUsedCPRecordID( u8 type );
extern int CastleScr_GetRemainderCPRecordID( u8 type );


//============================================================================================
//
//	バトルタワーで使用(frontier,fieldで使用される)
//
//============================================================================================
//extern int TowerScr_CheckEntryPokeSub( u8* pos_tbl, u8 max, SAVEDATA* savedata );


//============================================================================================
//
//	バトルルーレットで使用(frontier,fieldで使用される)
//
//============================================================================================
extern int RouletteScr_GetWinRecordID( u8 type );
extern int RouletteScr_GetMaxWinRecordID( u8 type );


//============================================================================================
//
//	タワーで使用(frontier,fieldで使用される)
//
//============================================================================================
extern int TowerScr_GetWinRecordID( u8 type );
extern int TowerScr_GetMaxWinRecordID( u8 type );


//
extern int Frontier_GetFriendIndex( u32 record_no );
extern u8 Frontier_GetPairRomCode( void );
extern u8 Frontier_CheckDPRomCode( SAVEDATA* sv );

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
extern u16 FrontierScrTools_CheckEntryPokeNum( u16 num, SAVEDATA* savedata );


#endif


