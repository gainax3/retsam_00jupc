//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		comm_command_wfp2pmf_func.c
 *	@brief		２～４人専用待合室	通信コマンド
 *	@author		tomoya takahashi
 *	@data		2007.05.24
 *
 *	このソースは
 *	wifi_p2pmatch.c	wifi_p2pmatchfour.c	どちらからも呼ばれるので常駐におきます。
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#include "communication/communication.h"
#include "comm_command_wfp2pmf.h"

#define __COMM_COMMAND_WFP2PMF_FUNC_H_GLOBAL
#include "comm_command_wfp2pmf_func.h"

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


//----------------------------------------------------------------------------
/**
 *	@brief	通信コマンド設定
 *	
 *	@param	p_wk			ワーク
 */
//-----------------------------------------------------------------------------
void CommCommandWFP2PMFInitialize( WFP2P_WK* p_wk )
{
	CommCommandInitialize( WFP2PMF_CommCommandTclGet(), 
			WFP2PMF_CommCommandTblNumGet(), p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	通信コマンド設定	wifi_p2pmatch.c内でテーブル設定用
 */
//-----------------------------------------------------------------------------
void CommCommandWFP2PMF_MatchStartInitialize( void )
{
	CommCommandInitialize( WFP2PMF_CommCommandTclGet(), 
			WFP2PMF_CommCommandTblNumGet(), NULL );
}

int CommWFP2PMFGetZeroSize( void )
{
	return 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	結果データサイズ
 *	@return	データサイズ
 */
//-----------------------------------------------------------------------------
int CommWFP2PMFGetWFP2PMF_COMM_RESULTSize( void )
{
	return sizeof(WFP2PMF_COMM_RESULT);
}

//----------------------------------------------------------------------------
/**
 *	@brief	VCHATデータサイズ
 *	@return	データサイズ
 */
//-----------------------------------------------------------------------------
int CommWFP2PMFGetWFP2PMF_COMM_VCHATSize( void )
{
	return sizeof(WFP2PMF_COMM_VCHAT);
}

//----------------------------------------------------------------------------
/**
 *	@brief	親からの通信OK通知
 *
 *	@param	netID		ネットID
 *	@param	size		サイズ
 *	@param	pBuff		バッファ
 *	@param	pWork		ワーク
 */
//-----------------------------------------------------------------------------
void CommWFP2PMFGameResult( int netID, int size, void* pBuff, void* pWork )
{
	WFP2P_WK* p_wk = pWork;
	WFP2PMF_COMM_RESULT* p_data = pBuff;

	if( p_wk == NULL ){
		return ;
	}
	
	WFP2PMF_CommResultRecv( p_wk, p_data );
}

//----------------------------------------------------------------------------
/**
 *	@brief	親からのゲーム開始を通知
 *
 *	@param	netID		ネットID
 *	@param	size		サイズ
 *	@param	pBuff		バッファ
 *	@param	pWork		ワーク
 */
//-----------------------------------------------------------------------------
void CommWFP2PMFGameStart( int netID, int size, void* pBuff, void* pWork )
{
	WFP2P_WK* p_wk = pWork;

	if( p_wk == NULL ){
		return ;
	}
	
	// ゲーム開始
	WFP2PMF_CommStartRecv( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	親からVCHATフラグ取得
 *
 *	@param	netID		ネットID
 *	@param	size		サイズ
 *	@param	pBuff		バッファ
 *	@param	pWork		ワーク
 */
//-----------------------------------------------------------------------------
void CommWFP2PMFGameVchat( int netID, int size, void* pBuff, void* pWork )
{
	WFP2P_WK* p_wk = pWork;

	if( p_wk == NULL ){
		return ;
	}
	
	// VCHATフラグ取得
	WFP2PMF_CommVchatRecv( p_wk, pBuff );
}
