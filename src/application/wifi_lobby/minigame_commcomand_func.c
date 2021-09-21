//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		minigame_commcomand_func.c
 *	@brief		ミニゲームツール通信コマンド
 *	@author		tomoya takahashi
 *	@data		2007.11.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"
#include "minigame_commcomand.h"
#include "minigame_commcomand_func.h"

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
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------


// 通信初期化関数
void CommCommandMNGMInitialize( MNGM_RESULTWK* p_wk )
{
	CommCommandInitialize( MNGM_CommCommandTclGet(), 
			MNGM_CommCommandTblNumGet(), p_wk );
}
void CommCommandMNGMEntryInitialize( MNGM_ENTRYWK* p_wk )
{
	CommCommandInitialize( MNGM_CommCommandTclGet(), 
			MNGM_CommCommandTblNumGet(), p_wk );
}

// 共通サイズ取得関数
int CommMNGMGetZeroSize( void )
{
	return 0;
}
int CommMNGMGetu32Size( void )
{
	return sizeof(u32);
}

// 親ー＞子　Rareゲームタイプ
void CommMNGMRareGame( int netID, int size, void* pBuff, void* pWork )
{
	MNGM_ENTRY_SetRareGame( pWork, *((u32*)pBuff) );
}


// 子ー>親	もういちど
void CommMNGMRetryYes( int netID, int size, void* pBuff, void* pWork )
{
	MNGM_RESULT_SetKoRetry( pWork, netID, TRUE );
}

// 子ー>親	もうやらない
void CommMNGMRetryNo( int netID, int size, void* pBuff, void* pWork )
{
	MNGM_RESULT_SetKoRetry( pWork, netID, FALSE );
}


// 親ー>子	もういちど
void CommMNGMRetryOk( int netID, int size, void* pBuff, void* pWork )
{
	MNGM_RESULT_SetOyaRetry( pWork, TRUE );
}

// 親ー>子	もうやらない
void CommMNGMRetryNg( int netID, int size, void* pBuff, void* pWork )
{
	MNGM_RESULT_SetOyaRetry( pWork, FALSE );
}

