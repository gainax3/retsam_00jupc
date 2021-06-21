//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wbr_comm_command.c
 *	@brief		wifi	バトルルーム通信コマンド
 *	@author		tomoya takahashi
 *	@data		2007.02.15
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"
#include "communication/communication.h"
#include "wbr_comm_command_func.h"

#define __WBR_COMM_COMMAND_H_GLOBAL
#include "wbr_comm_command.h"

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
//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *			通信データ
 */
//-----------------------------------------------------------------------------
static const CommPacketTbl _CommWbrPacketTbl[] = {
	//-------------------------------------
	//　ゲーム用
	//=====================================
	// 同期用	
	{ WBR_CNM_GameSysStart, WBR_CNM_ZeroSizeGet, NULL },		///< OYA->KO 準備完了
	{ WBR_CNM_GameSysGameStart, WBR_CNM_ZeroSizeGet, NULL },		///< OYA->KO ゲーム開始
	{ WBR_CNM_GameSysEnd,	WBR_CNM_ZeroSizeGet, NULL },		///< OYA->KO 終了 	
	// データ受信用
	{ WBR_CNM_KoCommonRecv,		WBR_CNM_KoCommonSizeGet, WBR_CNM_KoCommRecvBuffGet },		///< KO->OYA 共通データ
	{ WBR_CNM_KoGameDataRecv,	WBR_CNM_KoGameDataSizeGet, NULL },		///< KO->OYA ゲームコマンド
	{ WBR_CNM_OyaGameDataRecv,	WBR_CNM_OyaGameDataSizeGet, NULL },		///< OYA->KO ゲーム状態データ
	{ WBR_CNM_KoOyaDataGetRecv,	WBR_CNM_ZeroSizeGet, NULL },		///< KO->OYA 現在のゲーム状態強制取得
	{ WBR_CNM_KoTalkDataRecv,	WBR_CNM_KoTalkDataSizeGet, NULL },		///< KO->OYA 話しかけデータ
	{ WBR_CNM_KoTalkReqRecv,	WBR_CNM_KoTalkReqSizeGet, NULL },		///< KO->OYA 話しかけデータ
};


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief		コマンドテーブルを取得
 */
//-----------------------------------------------------------------------------
const CommPacketTbl* Wbr_CommCommandTclGet( void )
{
	return _CommWbrPacketTbl;
}

//----------------------------------------------------------------------------
/**
 *	@brief	コマンドテーブル要素数を取得
 */
//-----------------------------------------------------------------------------
int Wbr_CommCommandTblNumGet( void )
{
    return sizeof(_CommWbrPacketTbl)/sizeof(CommPacketTbl);
}

