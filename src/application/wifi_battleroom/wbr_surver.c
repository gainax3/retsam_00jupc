//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wbr_surver.c
 *	@brief		サーバーシステム
 *	@author		tomoya takahashi
 *	@data		2007.02.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#include "wbr_surver_cont.h"
#define __WBR_SURVER_H_GLOBAL
#include "wbr_surver.h"

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
#define WBR_SURVER_KO_COMMON_RECV_OK_MSK	(0x3)	// 通信完了マスク

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	サーバーワーク
//=====================================
typedef struct _WBR_SURVER{
	u32 seq;	// シーケンス
	
	// 受信用ワーク
	WBR_KO_COMM_GAME	game_ko[ WBR_COMM_NUM ];

	// 共通処理受信マスク
	u32	common_data_msk;

	// オブジェクト管理システム
	WBR_SURVERCONT*	p_survercont;
}WBR_SURVER;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
static void WBR_SurverDataSendExe( WBR_SURVER* p_sys );

//----------------------------------------------------------------------------
/**
 *	@brief	サーバーシステム初期化
 *
 *	@param	heap	ヒープ
 *
 *	@return	システムワーク
 */
//-----------------------------------------------------------------------------
WBR_SURVER* WBR_SurverInit( u32 heap )
{
	WBR_SURVER* p_sys;

	// システムワーク作成
	p_sys = sys_AllocMemory( heap, sizeof(WBR_SURVER) );
	memset( p_sys, 0, sizeof(WBR_SURVER) );
	
	// オブジェクト管理システム作成
	{
		p_sys->p_survercont = WBR_SurverContInit( WBR_COMM_NUM, heap );
	}

	return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	サーバーシステム破棄
 *
 *	@param	p_sys	サーバーシステム
 */
//-----------------------------------------------------------------------------
void WBR_SurverExit( WBR_SURVER* p_sys )
{
	WBR_SurverContExit( p_sys->p_survercont );
	sys_FreeMemoryEz( p_sys );
} 

//----------------------------------------------------------------------------
/**
 *	@brief	サーバーメイン
 *
 *	@param	p_sys	サーバーシステム
 */
//-----------------------------------------------------------------------------
void WBR_SurverMain( WBR_SURVER* p_sys )
{
	BOOL result;
	
	switch( p_sys->seq ){
	case WBR_SURVER_SEQ_START_SEND:
		// 通信開始を送信
		CommSendData( CNM_WBR_GAME_SYS_START, NULL, 0 );
		
		p_sys->seq ++;
		break;
		
	case WBR_SURVER_SEQ_GETCOMMON_DATA:
		// みんなに共通データがいきわたるのを待つ
		break;

	case WBR_SURVER_SEQ_MAIN_START:	// 開始準備
		CommSendData( CNM_WBR_GAME_SYS_GAMESTART, NULL, 0 );
		p_sys->seq++;
		break;
		
	case WBR_SURVER_SEQ_MAIN:
		// オブジェクト管理
		result = WBR_SurverContMain( p_sys->p_survercont );
		WBR_SurverDataSendExe( p_sys );	// データ送信
		if( result == TRUE ){
			p_sys->seq ++;
		}
		break;

	case WBR_SURVER_SEQ_END_SEND:
		// 通信開始を送信
		CommSendData( CNM_WBR_GAME_SYS_END, NULL, 0 );
		p_sys->seq ++;
		break;

	case WBR_SURVER_SEQ_END_WAIT:
		break;

	default:
		GF_ASSERT(0);
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	シーケンス設定
 *	
 *	@param	p_sys	ワーク
 *	@param	seq		シーケンス
 */
//-----------------------------------------------------------------------------
void WBR_SurverSeqSet( WBR_SURVER* p_sys, u32 seq )
{
	p_sys->seq = seq;
}

//----------------------------------------------------------------------------
/**
 *	@brief	シーケンス取得
 *
 *	@param	cp_sys	ワーク
 *
 *	@return
 */
//-----------------------------------------------------------------------------
u32 WBR_SurverSeqGet( const WBR_SURVER* cp_sys )
{
	return cp_sys->seq;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ゲーム内の子データを設定
 *
 *	@param	p_sys		ワーク
 *	@param	cp_ko		子データ
 *	@param	id			ID
 */
//-----------------------------------------------------------------------------
void WBR_SurverKoGameDataSet( WBR_SURVER* p_sys, const WBR_KO_COMM_GAME* cp_ko, u32 id )
{
	WBR_SurverContReqCmdSet( p_sys->p_survercont, &cp_ko->req );
}

//----------------------------------------------------------------------------
/**
 *	@brief	話しかけデータを取得
 *
 *	@param	p_sys		ワーク
 *	@param	cp_ko		子データ
 *	@param	id			ID
 */
//-----------------------------------------------------------------------------
void WBR_SurverKoTalkDataSet( WBR_SURVER* p_sys, const WBR_KO_COMM_TALK* cp_ko, u32 id )
{
	WBR_SurverContTalkDataSet( p_sys->p_survercont, id, &cp_ko->talk_data );
}

//----------------------------------------------------------------------------
/**
 *	@brief	話しかけリクエストを取得
 *
 *	@param	p_sys		ワーク
 *	@param	cp_ko		子データ
 *	@param	id			ID
 */
//-----------------------------------------------------------------------------
void WBR_SurverKoTalkReqSet( WBR_SURVER* p_sys, const WBR_TALK_REQ* cp_ko, u32 id )
{
	WBR_SurverContTalkReqDataSet( p_sys->p_survercont, cp_ko );
}

//----------------------------------------------------------------------------
/**
 *	@brief	子からの共通データ受信処理
 *
 *	@param	p_sys		ワーク
 *	@param	id			ID
 *
 *	@retval	TRUE	全員から受信した
 *	@retval	FALSE	全員からまだ受信中
 */
//-----------------------------------------------------------------------------
BOOL WBR_SurverKoCommDataRecv( WBR_SURVER* p_sys, u32 id )
{
	p_sys->common_data_msk |= 1<<id;

	if( p_sys->common_data_msk == WBR_SURVER_KO_COMMON_RECV_OK_MSK ){
		return TRUE;
	}
	return FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	サーバーの状態を転送
 *
 *	@param	cp_sys	ワーク
 */
//-----------------------------------------------------------------------------
void WBR_SurverDataSend( WBR_SURVER* p_sys )
{
	WBR_SurverContNowStatusActCmdSet( p_sys->p_survercont );
}


//-----------------------------------------------------------------------------
/**
 *		プライベート関数
 */
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/**
 *	@brief	おやデータ送信
 *
 *	@param	p_sys		ワーク
 */
//-----------------------------------------------------------------------------
static void WBR_SurverDataSendExe( WBR_SURVER* p_sys )
{
	WBR_OBJ_DATA oya_data;

	while( WBR_SurverContSendDataPop( p_sys->p_survercont, &oya_data ) == TRUE ){
		CommSendData( CNM_WBR_OYA_GAMEDATA, &oya_data, sizeof(WBR_OBJ_DATA) );
	}
}
