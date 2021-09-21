//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		comm_bct_command_func.c
 *	@brief		バケットミニゲーム　通信コマンド
 *	@author		tomoya takahashi
 *	@data		2007.06.20
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#include "communication/communication.h"
#include "comm_bct_command.h"

#include "comm_bct_command_func.h"

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
void CommCommandBCTInitialize( BUCKET_WK* p_wk )
{
	CommCommandInitialize( BCT_CommCommandTclGet(), 
			BCT_CommCommandTblNumGet(), p_wk );
}

int CommBCTGetZeroSize( void )
{
	return 0;
}


//----------------------------------------------------------------------------
/**
 *	@brief	ゲーム開始	受信
 *
 *	@param	netID		NETID
 *	@param	size		サイズ
 *	@param	pBuff		バッファ
 *	@param	pWork		ワーク
 */
//-----------------------------------------------------------------------------
void CommBCTGameStart( int netID, int size, void* pBuff, void* pWork )
{
	BUCKET_WK* p_wk = pWork;
	
	Bucket_StartSet( p_wk );
	TOMOYA_PRINT( "recv start\n" );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ゲーム終了　受信
 *
 *	@param	netID		NETID
 *	@param	size		サイズ
 *	@param	pBuff		バッファ
 *	@param	pWork		ワーク
 */
//-----------------------------------------------------------------------------
void CommBCTGameEnd( int netID, int size, void* pBuff, void* pWork )
{
	BUCKET_WK* p_wk = pWork;

	Bucket_EndSet( p_wk );
	TOMOYA_PRINT( "recv end\n" );
}

//----------------------------------------------------------------------------
/**
 *	@brief	木の実データ　通信
 *
 *	@param	netID		NETID
 *	@param	size		サイズ
 *	@param	pBuff		バッファ
 *	@param	pWork		ワーク
 */
//-----------------------------------------------------------------------------
void CommBCTNuts( int netID, int size, void* pBuff, void* pWork )
{
	BUCKET_WK* p_wk = pWork;

	Bucket_ClientNutsSet( p_wk, pBuff, netID );

	if( CommGetCurrentID() == COMM_PARENT_ID ){
		Bucket_SurverNutsSet( p_wk, pBuff, netID );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	スコアを受信
 *
 *	@param	netID		NETID
 *	@param	size		サイズ
 *	@param	pBuff		バッファ
 *	@param	pWork		ワーク
 */
//-----------------------------------------------------------------------------
void CommBCTScore( int netID, int size, void* pBuff, void* pWork )
{
	BUCKET_WK* p_wk = pWork;
	Bucket_SurverScoreSet( p_wk, *((u32*)pBuff), netID );

	OS_Printf( "score get netid[%d]\n", netID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	みんなのスコアを受信
 *	
 *	@param	netID		NETID
 *	@param	size		サイズ
 *	@param	pBuff		バッファ
 *	@param	pWork		ワーク
 */
//-----------------------------------------------------------------------------
void CommBCTAllScore( int netID, int size, void* pBuff, void* pWork )
{
	BUCKET_WK* p_wk = pWork;
	Bucket_ClientAllScoreSet( p_wk, pBuff, netID );

	OS_Printf( "all score get netid[%d]\n", netID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	途中のスコアを受信
 *	
 *	@param	netID		NETID
 *	@param	size		サイズ
 *	@param	pBuff		バッファ
 *	@param	pWork		ワーク
 */
//-----------------------------------------------------------------------------
void CommBCTMiddleScore( int netID, int size, void* pBuff, void* pWork )
{
	BUCKET_WK* p_wk = pWork;
	u32 score;

	score = *((u32*)pBuff);

	Bucket_ClientMiddleScoreSet( p_wk, score, netID );

//	OS_Printf( "middle score get netid[%d]\n", netID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	途中のスコアをみんなから受け取ったよ	（親ー＞子）
 *	
 *	@param	netID		NETID
 *	@param	size		サイズ
 *	@param	pBuff		バッファ
 *	@param	pWork		ワーク
 */
//-----------------------------------------------------------------------------
void CommBCTMiddleScoreOk( int netID, int size, void* pBuff, void* pWork )
{
	BUCKET_WK* p_wk = pWork;

	Bucket_ClientMiddleScoreOkSet( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ゲームデータの段階を受信	親ー＞子
 *	
 *	@param	netID		NETID
 *	@param	size		サイズ
 *	@param	pBuff		バッファ
 *	@param	pWork		ワーク
 */
//-----------------------------------------------------------------------------
void CommBCTGameDataIdx( int netID, int size, void* pBuff, void* pWork )
{
	BUCKET_WK* p_wk = pWork;
	u32 idx;

	idx = *((u32*)pBuff);
	// 親からかチェック
	if( netID == COMM_PARENT_ID ){
		Bucket_ClientGameDataIdxSet( p_wk, idx );
		OS_Printf( "game level get[%d] level[%d]\n", netID, idx );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	木の実通信データサイズ取得
 *	@return	サイズ
 */
//-----------------------------------------------------------------------------
int CommBCTGetNutsSize( void )
{
	return sizeof(BCT_NUT_COMM);
}

//----------------------------------------------------------------------------
/**
 *	@brief	みんなの得点通信データサイズ取得
 *	@return	サイズ
 */
//-----------------------------------------------------------------------------
int CommBCTGetAllScoreSize( void )
{
	return sizeof(BCT_SCORE_COMM);
}

//----------------------------------------------------------------------------
/**
 *	@brief	得点通信データサイズ取得
 *	@return	サイズ
 */
//-----------------------------------------------------------------------------
int CommBCTGetScoreSize( void )
{
	return sizeof(u32);
}

//----------------------------------------------------------------------------
/**
 *	@brief	ゲームデータインデックスサイズ取得
 *	@return	サイズ
 */
//-----------------------------------------------------------------------------
int CommBCTGetGameDataIdxSize( void )
{
	return sizeof(u32);
}
