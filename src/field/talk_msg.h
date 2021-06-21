//============================================================================================
/**
 * @file	talk_msg.h
 * @brief	メッセージ表示処理
 * @author	Hiroyuki Nakamura
 * @date	2004.10.28
 */
//============================================================================================
#ifndef TALK_MSG_H
#define TALK_MSG_H

#include "savedata/config.h"

#undef GLOBAL
#ifdef TALK_MSG_H_GLOBAL
#define GLOBAL	/*	*/
#else
#define GLOBAL	extern
#endif


//============================================================================================
//	シンボル定義
//============================================================================================
// メッセージプリント初期化フラグ
enum {
	MSG_PRINT_NO_INIT = 0,	// 初期化なし
	MSG_PRINT_INIT_ON		// 初期化あり
};


//============================================================================================
//	グローバル変数
//============================================================================================


//============================================================================================
//	プロトタイプ宣言
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * メッセージ表示システム初期化
 *
 * @param	type	パレット転送タイプ
 * @param	init	メッセージプリント初期化フラグ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void FieldMsgPrintInit( u32 type, u32 init );

//--------------------------------------------------------------------------------------------
/**
 * フィールド会話用BMP取得
 *
 * @param	ini		BGLデータ
 * @param	win		BMPデータ格納場所
 * @param	frmnum	BGフレーム
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void FldTalkBmpAdd( GF_BGL_INI * ini, GF_BGL_BMPWIN * win, u32 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * フィールド会話ウィンドウ表示
 *
 * @param	win		BMPデータ
 * @param	cfg		コンフィグデータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void FieldTalkWinPut( GF_BGL_BMPWIN * win, const CONFIG * cfg );


//--------------------------------------------------------------------------------------------
/**
 * フィールド会話ウィンドウ表示領域クリア
 *
 * @param	win		BMPデータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void FieldTalkWinClear( GF_BGL_BMPWIN* win );

//--------------------------------------------------------------------------------------------
/**
 * フィールド会話スタート
 *
 * @param	win		BMPデータ
 * @param	msg		メッセージ
 * @param	cfg		コンフィグデータ
 * @param	skip	スキップ許可フラグ
 *
 * @return	メッセージインデックス
 */
//--------------------------------------------------------------------------------------------
GLOBAL u8 FieldTalkMsgStart( GF_BGL_BMPWIN * win, STRBUF * msg, const CONFIG * cfg, u8 skip );

//------------------------------------------------------------------
/**
 * フィールド会話スタート(拡張版)
 *
 * @param   win			BitmapWindow
 * @param   msg			メッセージデータ
 * @param   font		フォント
 * @param   msg_speed	メッセージ速度
 * @param   skip		スキップ許可フラグ
 * @param   auto_flag	自動送りフラグ
 *
 * @retval  u8			メッセージプリントタスクのインデックス
 */
//------------------------------------------------------------------
GLOBAL u8 FieldTalkMsgStartEx( GF_BGL_BMPWIN * win, STRBUF * msg, int font, int msg_speed, u8 skip, int auto_flag );

//--------------------------------------------------------------------------------------------
/**
 * フィールド会話終了待ち
 *
 * @param	msg_index	メッセージインデックス
 *
 * @retval	"0 = 会話中"
 * @retval	"1 = 終了"
 */
//--------------------------------------------------------------------------------------------
GLOBAL u8 FldTalkMsgEndCheck( u8 msg_index );


//--------------------------------------------------------------------------------------------
/**
 * フィールド看板用用BMP取得
 *
 * @param	ini		BGLデータ
 * @param	win		BMPデータ格納場所
 * @param	type	看板タイプ
 * @param	frmnum	BGフレーム
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void FldBoardBmpAdd( GF_BGL_INI * ini, GF_BGL_BMPWIN * win, u16 type, u16 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * フィールド看板ウィンドウ表示
 *
 * @param	win		BMPデータ
 * @param	type	看板タイプ
 * @param	map		マップ番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void FldBoardWinPut( GF_BGL_BMPWIN * win, u16 type, u16 map );


#undef GLOBAL
#endif	/* TALK_MSG_H */
