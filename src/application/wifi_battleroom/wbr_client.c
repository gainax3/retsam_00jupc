//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wbr_client.c
 *	@brief		クライアントシステム
 *	@author		tomoya takahashi
 *	@data		2007.02.22
 *
 *	歩き回り描画PROCや
 *	その他PROCに分岐する処理を管理
 * 
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "wbr_client_local.h"
#include "system/procsys.h"
#include "wbr_client_cont.h"
#include "field/fieldsys.h"
#include "field/tr_card_setup.h"
#include "field/field_subproc.h"

#define __WBR_CLIENT_H_GLOBAL
#include "wbr_client.h"

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


//-------------------------------------
///	クライアントワーク
//=====================================
typedef struct _WBR_CLIENT{
	u32 seq;
	WBR_CLIENT_COMMON common_data;

	// クライアント共通データ
	WBR_COMM_COMMON comm_data;
}WBR_CLIENT;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
static void WBR_ClientCommDataInit( WBR_CLIENT* p_sys, WBR_COMM_COMMON* p_common_data );


//----------------------------------------------------------------------------
/**
 *	@brief	クライアントシステム初期化
 *	
 *	@param	p_init		初期化データ
 *	@param	heapID		ヒープ
 *
 *	@return	クライアントワーク
 */
//-----------------------------------------------------------------------------
WBR_CLIENT* WBR_ClientInit( const WBR_CLIENT_INIT* p_init, u32 heapID )
{
	WBR_CLIENT* p_sys;

	// ワーク作成
	p_sys = sys_AllocMemory( heapID, sizeof( WBR_CLIENT ) );
	memset( p_sys, 0, sizeof(WBR_CLIENT) );
	
	// 初期化データ設定
	p_sys->common_data.netid = p_init->netid;
	p_sys->common_data.cp_mystatus = p_init->cp_mystatus;
	p_sys->common_data.p_fsys = p_init->p_fsys;
	p_sys->common_data.heapID = heapID;

	return p_sys;
} 

//----------------------------------------------------------------------------
/**
 *	@brief	クライアントシステム破棄
 *
 *	@param	p_sys	システムワーク
 */
//-----------------------------------------------------------------------------
void WBR_ClientExit( WBR_CLIENT* p_sys )
{
	GF_ASSERT( p_sys->seq == WBR_CLIENT_SEQ_END );	// 終わり状態になっているかチェック
	sys_FreeMemoryEz( p_sys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	メイン関数
 *
 *	@param	p_sys	システムワーク
 *	
 *	@retval	TRUE	終了
 *	@retval	FALSE	通常
 */
//-----------------------------------------------------------------------------
BOOL WBR_ClientMain( WBR_CLIENT* p_sys )
{
	switch( p_sys->seq ){
	case WBR_CLIENT_SEQ_START_WAIT:		// 親からの開始命令待ち
		break;

	case WBR_CLIENT_SEQ_COMMDATA_SEND:	// 共通データ送信

		// 共通データ作成
		WBR_ClientCommDataInit( p_sys, &p_sys->comm_data );
		CommSendHugeData( CNM_WBR_KO_COMMON, &p_sys->comm_data, sizeof(WBR_COMM_COMMON) );
		p_sys->seq ++;
		break;

	case WBR_CLIENT_SEQ_COMMDATA_WAIT:	// 共通データ受信待ち
		break;
			
	case WBR_CLIENT_SEQ_WALK_START:
		{
			static const PROC_DATA proc_data = {
				WBR_ClientCont_Init,
				WBR_ClientCont_Main,
				WBR_ClientCont_End,
				NO_OVERLAY_ID
			};
			// プロセス開始
			GameSystem_StartSubProc( p_sys->common_data.p_fsys, &proc_data, &p_sys->common_data );

			p_sys->seq ++;
		}
		break;
		
	case WBR_CLIENT_SEQ_WALK_ENDWAIT:
		if( GameSystem_CheckSubProcExists( p_sys->common_data.p_fsys ) == FALSE ){

			// 次何をするのかチェック
			switch( p_sys->common_data.next_seq ){
			case WBR_CLIENTCONT_RET_END:
				// 終了へ
				p_sys->seq = WBR_CLIENT_SEQ_END;	
				break;

			case WBR_CLIENTCONT_RET_TRCARD:
				// トレーナーカードへ
				p_sys->seq = WBR_CLIENT_SEQ_TRCARD_START;
				break;
			default:
				GF_ASSERT( 0 );
				break;
			}
		}
		break;

	case WBR_CLIENT_SEQ_TRCARD_START:
		FieldTrCard_SetProc( p_sys->common_data.p_fsys, 
				&p_sys->common_data.comm_data[ p_sys->common_data.link_netid ].tr_card );
		p_sys->seq++;
		break;
		
	case WBR_CLIENT_SEQ_TRCARD_ENDWAIT:
		if( GameSystem_CheckSubProcExists( p_sys->common_data.p_fsys ) == FALSE ){
			// 歩き回り表示に戻る
			p_sys->seq = WBR_CLIENT_SEQ_WALK_START;
		}
		break;
		
	case WBR_CLIENT_SEQ_END:
		return TRUE;
	
	default:
		GF_ASSERT(0);
		break;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	シーケンスを設定する
 *
 *	@param	p_sys	システムワーク
 *	@param	seq		シーケンス
 */
//-----------------------------------------------------------------------------
void WBR_ClientSeqSet( WBR_CLIENT* p_sys, u32 seq )
{
	p_sys->seq = seq;
}

//----------------------------------------------------------------------------
/**
 *	@brief	共通データ設定
 *
 *	@param	p_sys		システムワーク
 *	@param	cp_data		共通データ
 */
//-----------------------------------------------------------------------------
void WBR_ClientCommDataSet( WBR_CLIENT* p_sys, const WBR_COMM_COMMON* cp_data )
{
	memcpy( p_sys->common_data.comm_data, cp_data, sizeof(WBR_COMM_COMMON)*WBR_COMM_NUM );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ゲームデータ設定
 *		
 *	@param	p_sys		システムワーク
 *	@param	cp_data		共通データ
 */
//-----------------------------------------------------------------------------
void WBR_ClientGameDataSet( WBR_CLIENT* p_sys, const WBR_OYA_COMM_GAME* cp_data )
{
	WBR_ClientCont_SetOyaGameData( p_sys->common_data.p_procw, cp_data );
}	

//----------------------------------------------------------------------------
/**
 *	@brief	話しかけデータ取得
 *
 *	@param	p_sys		システムワーク
 *	@param	cp_data		話しかけデータ
 */
//-----------------------------------------------------------------------------
void WBR_ClientTalkDataSet( WBR_CLIENT* p_sys, const WBR_KO_COMM_TALK* cp_data )
{
	if( p_sys->common_data.p_procw != NULL ){
		WBR_ClientCont_SetTalkData( p_sys->common_data.p_procw, cp_data );
	}
}	

//----------------------------------------------------------------------------
/**
 *	@brief	ゲーム終了データ設定
 *
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
void WBR_ClientGameEndSet( WBR_CLIENT* p_sys )
{
	WBR_ClientCont_SetEndFlag( p_sys->common_data.p_procw );
}	

//----------------------------------------------------------------------------
/**
 *	@brief	共通データ格納先バッファを返す
 *
 *	@param	p_sys		システムワーク
 *	@param	netid		ネットID
 *
 *	@return	バッファポインタ
 */
//-----------------------------------------------------------------------------
u8* WBR_ClientCommDataBuffPtrGet( WBR_CLIENT* p_sys, u32 netid )
{
	return (u8*)&p_sys->common_data.comm_data[ netid ];
}



//----------------------------------------------------------------------------
/**
 *	@brief	クライアントデータ作成
 *
 *	@param	p_sys			システムワーク
 *	@param	p_common_data	共通データ格納先
 */
//-----------------------------------------------------------------------------
static void WBR_ClientCommDataInit( WBR_CLIENT* p_sys, WBR_COMM_COMMON* p_common_data )
{
	TRCSET_MakeTrainerInfo( BADGE_BRUSH_OFF, TIME_UPDATE_OFF, GYM_READER_MASK_NONE, UNION_TR_NONE,
							p_sys->common_data.p_fsys,
							&p_common_data->tr_card );

}
