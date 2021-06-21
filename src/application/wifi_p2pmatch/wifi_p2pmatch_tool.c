//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wifi_p2pmatch_tool.c
 *	@brief		WiFiクラブ以外で任天堂認証サーバにアクセスするときに、WiFiクラブ上で表示されないステータス状態にするシステム
 *	@author		tomoya takahashi
 *	@data		2008.03.28
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#include "common.h"
#include "application/wifi_p2pmatch_tool.h"

#include "application/wifi_p2pmatch/wifi_p2pmatch_local.h"

#include  "wifi/dwc_rap.h"

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
 *	@brief	WiFiクラブ上で表示されないように状態無しをサーバに送信
 *
 *	@param	p_mystatus		自分の状態
 *	@param	heapID			ヒープID
 */
//-----------------------------------------------------------------------------
_WIFI_MACH_STATUS* WifiP2PMatch_CleanStatus_Set( const MYSTATUS* cp_mystatus, u32 heapID )
{
	_WIFI_MACH_STATUS* p_status;
	BOOL result;

	p_status = sys_AllocMemory( heapID, _WIFI_STATUS_MAX_SIZE );
	memset( p_status, 0, _WIFI_STATUS_MAX_SIZE );
	p_status->status = WIFI_STATUS_PLAY_OTHER;	// こうしておけば大丈夫

	p_status->sex			= MyStatus_GetMySex( cp_mystatus );
	p_status->trainer_view	= MyStatus_GetTrainerView( cp_mystatus );
    p_status->version		= MyStatus_GetRomCode( cp_mystatus );
    p_status->regionCode	= MyStatus_GetRegionCode( cp_mystatus );

	result = mydwc_setMyInfo( p_status, _WIFI_STATUS_MAX_SIZE );
	GF_ASSERT( result == TRUE );

	return p_status;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WiFiクラブ上で表示されないように状態なしを送信したバッファを破棄する
 *
 *	@param	p_buff	ワーク
 */
//-----------------------------------------------------------------------------
void WifiP2PMatch_CleanStatus_Delete( _WIFI_MACH_STATUS* p_buff )
{
	sys_FreeMemoryEz( p_buff );
}


