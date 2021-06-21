//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wbr_comm_command_func.c
 *	@brief		wifi バトルルーム　受信コールバック関数
 *	@author		tomoya takahashi
 *	@data		2007.02.15
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "communication/communication.h"

#include "wifi_battleroom_local.h"
#include "wbr_surver.h"
#include "wbr_client.h"

#define	__WBR_COMM_COMMAND_FUNC_H_GLOBAL
#include "wbr_comm_command_func.h"

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
 *	@brief	wifi_battleroom　タスク実行開始を子受信
 *
 *	@param	netID		ネットID
 *	@param	size		サイズ
 *	@param	pBuff		バッファ
 *	@param	p_work		ワーク
 */
//-----------------------------------------------------------------------------
void WBR_CNM_GameSysStart( int netID, int size, void* pBuff, void* p_work )
{
	WIFI_BATTLEROOM* p_sys = p_work;
	WBR_ClientSeqSet( p_sys->p_client, WBR_CLIENT_SEQ_COMMDATA_SEND );

	OS_Printf( "共通データの送信開始\n\n" );
}

//----------------------------------------------------------------------------
/**
 *	@brief	親ー＞子　ゲーム処理の開始
 *
 *	@param	netID	
 *	@param	size
 *	@param	pBuff
 *	@param	p_work 
 */
//-----------------------------------------------------------------------------
void WBR_CNM_GameSysGameStart( int netID, int size, void* pBuff, void* p_work )
{
	WIFI_BATTLEROOM* p_sys = p_work;

	// クライアントシステムは実際の通信処理へ
	WBR_ClientSeqSet( p_sys->p_client, WBR_CLIENT_SEQ_WALK_START );

	OS_Printf( "クライアント　処理開始\n" );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ゲーム終了を子受信
 *
 *	@param	netID		ネットID
 *	@param	size		サイズ
 *	@param	pBuff		バッファ
 *	@param	p_work		ワーク
 */
//-----------------------------------------------------------------------------
void WBR_CNM_GameSysEnd( int netID, int size, void* pBuff, void* p_work )
{
	WIFI_BATTLEROOM* p_sys = p_work;
	WBR_ClientGameEndSet( p_sys->p_client );

	OS_Printf( "ゲーム終了\n" );
}


//----------------------------------------------------------------------------
/**
 *	@brief	子機からの共通データ受信
 *
 *	@param	netID	ネットID
 *	@param	size	サイズ
 *	@param	pBuff	バッファ
 *	@param	p_work	ワーク
 */
//-----------------------------------------------------------------------------
void WBR_CNM_KoCommonRecv( int netID, int size, void* pBuff, void* p_work )
{
	WIFI_BATTLEROOM* p_sys = p_work;
	BOOL result;

	if( p_sys->p_surver ){
		result = WBR_SurverKoCommDataRecv( p_sys->p_surver, netID );
		if( result == TRUE ){
			WBR_SurverSeqSet( p_sys->p_surver, WBR_SURVER_SEQ_MAIN_START );
			OS_Printf( "子　共通データ受信完了\n" );
		}
	}

}

//----------------------------------------------------------------------------
/**
 *	@brief	子からゲームコマンドデータを取得する
 *
 *	@param	netID		ネットID
 *	@param	size		サイズ
 *	@param	pBuff		バッファ
 *	@param	p_work		ワーク
 */
//-----------------------------------------------------------------------------
void WBR_CNM_KoGameDataRecv( int netID, int size, void* pBuff, void* p_work )
{
	WIFI_BATTLEROOM* p_sys = p_work;

	if( p_sys->netid == COMM_PARENT_ID ){
		WBR_SurverKoGameDataSet( p_sys->p_surver, pBuff, netID );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	親からのゲーム情報データを取得する
 *
 *	@param	netID		ネットID
 *	@param	size		サイズ
 *	@param	pBuff		バッファ
 *	@param	p_work		ワーク
 */
//-----------------------------------------------------------------------------
void WBR_CNM_OyaGameDataRecv( int netID, int size, void* pBuff, void* p_work )
{
	WIFI_BATTLEROOM* p_sys = p_work;
	WBR_ClientGameDataSet( p_sys->p_client, pBuff );
}

//----------------------------------------------------------------------------
/**
 *	@brief	子からの親ゲームデータ強制受信要請
 *
 *	@param	netID		ネットID
 *	@param	size		サイズ
 *	@param	pBuff		バッファ
 *	@param	p_work		ワーク
 */
//-----------------------------------------------------------------------------
void WBR_CNM_KoOyaDataGetRecv( int netID, int size, void* pBuff, void* p_work )
{
	WIFI_BATTLEROOM* p_sys = p_work;
	
	if( p_sys->netid == COMM_PARENT_ID ){
		WBR_SurverDataSend( p_sys->p_surver );
	}

}

//----------------------------------------------------------------------------
/**
 *	@brief	話しかけデータ受信
 *
 *	@param	netID		ネットID
 *	@param	size		サイズ
 *	@param	pBuff		バッファ
 *	@param	p_work		ワーク
 */
//-----------------------------------------------------------------------------
void WBR_CNM_KoTalkDataRecv( int netID, int size, void* pBuff, void* p_work )
{
	WIFI_BATTLEROOM* p_sys = p_work;
	
	if( p_sys->netid == COMM_PARENT_ID ){
		WBR_SurverKoTalkDataSet( p_sys->p_surver, pBuff, netID );
	}

	// クライアントにも登録
	WBR_ClientTalkDataSet( p_sys->p_client, pBuff );
}

//----------------------------------------------------------------------------
/**
 *	@brief	話しかけ開始データ受信
 *
 *	@param	netID		ネットID
 *	@param	size		サイズ
 *	@param	pBuff		バッファ
 *	@param	p_work		ワーク
 */
//-----------------------------------------------------------------------------
void WBR_CNM_KoTalkReqRecv( int netID, int size, void* pBuff, void* p_work )
{
	WIFI_BATTLEROOM* p_sys = p_work;
	
	if( p_sys->netid == COMM_PARENT_ID ){
		WBR_SurverKoTalkReqSet( p_sys->p_surver, pBuff, netID );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	子の共通データ取得先バッファポインタを返す
 *
 *	@param	netID		ネットID
 *	@param	p_work		ワーク
 *	@param	size		サイズ
 *
 *	@return	バッファポインタ
 */
//-----------------------------------------------------------------------------
u8* WBR_CNM_KoCommRecvBuffGet( int netID, void* p_work, int size )
{
	WIFI_BATTLEROOM* p_sys = p_work;
	return WBR_ClientCommDataBuffPtrGet( p_sys->p_client, netID );
}
 

//----------------------------------------------------------------------------
/**
 *	@brief	サイズ0を返す
 */
//-----------------------------------------------------------------------------
int WBR_CNM_ZeroSizeGet( void )
{
	return 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	子機からの共通データサイズ
 */
//-----------------------------------------------------------------------------
int WBR_CNM_KoCommonSizeGet( void )
{
	return sizeof(WBR_COMM_COMMON);
}

//----------------------------------------------------------------------------
/**
 *	@brief	親機からの共通データサイズ
 */
//-----------------------------------------------------------------------------
int WBR_CNM_OyaCommonSizeGet( void )
{
	return sizeof(WBR_COMM_COMMON) * WBR_COMM_NUM;
}

//----------------------------------------------------------------------------
/**
 *	@brief	子機からのゲームデータサイズ
 */
//-----------------------------------------------------------------------------
int WBR_CNM_KoGameDataSizeGet( void )
{
	return sizeof( WBR_KO_COMM_GAME );
}

//----------------------------------------------------------------------------
/**
 *	@brief	親機からのゲームデータサイズ
 */
//-----------------------------------------------------------------------------
int WBR_CNM_OyaGameDataSizeGet( void )
{
	return sizeof( WBR_OYA_COMM_GAME );
}

//----------------------------------------------------------------------------
/**
 *	@brief	子機からの話しかけデータサイズ
 */
//-----------------------------------------------------------------------------
int WBR_CNM_KoTalkDataSizeGet( void )
{
	return sizeof( WBR_KO_COMM_TALK );
}

//----------------------------------------------------------------------------
/**
 *	@brief	話しかけリクエストデータサイズ取得
 */
//-----------------------------------------------------------------------------
int WBR_CNM_KoTalkReqSizeGet( void )
{
	return sizeof( WBR_TALK_REQ );
}
