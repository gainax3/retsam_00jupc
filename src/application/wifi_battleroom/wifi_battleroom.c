//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifi_battleroom.c
 *	@brief		wifi	バトルルーム
 *	@author		tomoya takahashi
 *	@data		2007.02.15
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "wifi_battleroom_local.h"
#include "system/pm_overlay.h"

#define __WIFI_BATTLEROOM_H_GLOBAL
#include "application/wifi_battleroom.h"

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
enum{
	WBRSYS_STATUS_START_SYNC,
	WBRSYS_STATUS_START_SYNCWAIT,
	WBRSYS_STATUS_MAIN,
	WBRSYS_STATUS_END_SYNC,
	WBRSYS_STATUS_END_SYNCWAIT,
	WBRSYS_STATUS_END,
	WBRSYS_STATUS_NUM,	// 状態の数
};

#define WBRSYS_TCB_PRI	(32)	// TCB優先順位


// WIFI2DMAPシステムオーバーレイ
FS_EXTERN_OVERLAY(wifi_2dmapsys);

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
static void WBRSys_ContTcb( TCB_PTR tcb, void* p_work );	// 管理TCB


//----------------------------------------------------------------------------
/**
 *	@brief	wifiバトルルーム初期化
 *
 *	@param	cp_init		初期化データ
 *	@param	heapID		ヒープID
 *
 *	@return 管理システムワーク
 */
//-----------------------------------------------------------------------------
WIFI_BATTLEROOM* WBRSys_Init( const WBR_INIT* cp_init, u32 heapID )
{
	WIFI_BATTLEROOM* p_wk;
	int i;
	WBR_CLIENT_INIT client_init;


	// wifi_2dmapオーバーレイ読込み
	Overlay_Load( FS_OVERLAY_ID(wifi_2dmapsys), OVERLAY_LOAD_NOT_SYNCHRONIZE);
	
	// ヒープ作成
	sys_CreateHeap( HEAPID_BASE_APP, HEAPID_WIFI_BATTLEROOM, 0x40000 );
	
	// ワーク作成
	p_wk = sys_AllocMemory( HEAPID_WIFI_BATTLEROOM, sizeof(WIFI_BATTLEROOM) );
	memset( p_wk, 0, sizeof(WIFI_BATTLEROOM) );

	// FIELDSYS_WORKを設定
	p_wk->p_fsys = cp_init->p_fsys;

	// 通信初期化
	CommCommandInitialize( Wbr_CommCommandTclGet(), CNM_COMMAND_MAX, p_wk );

	// 全員分のSAVEDATAを取得
	for( i=0; i<WBR_COMM_NUM; i++ ){
		p_wk->mystatus.cp_mystatus[ i ] = CommInfoGetMyStatus( i );
	}
	
	// 親なら親データ初期化
	p_wk->netid = CommGetCurrentID();
	if( p_wk->netid == COMM_PARENT_ID ){
		p_wk->p_surver = WBR_SurverInit( heapID );
	}
	
	// 子データ初期化
	client_init.netid = p_wk->netid;
	client_init.cp_mystatus = &p_wk->mystatus;
	client_init.p_fsys = cp_init->p_fsys;
	p_wk->p_client = WBR_ClientInit( &client_init, HEAPID_WIFI_BATTLEROOM );
	
	// 管理TCB登録
	p_wk->tcb = TCB_Add( WBRSys_ContTcb, p_wk, WBRSYS_TCB_PRI );

	return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	wifiバトルルーム破棄
 *		
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
void WBRSys_Exit( WIFI_BATTLEROOM* p_sys )
{
	GF_ASSERT( p_sys->status == WBRSYS_STATUS_END );

	// タスク破棄
	TCB_Delete( p_sys->tcb );

	// 子データ破棄
	WBR_ClientExit( p_sys->p_client );
	
	// 親データ破棄
	if( p_sys->p_surver ){
		WBR_SurverExit( p_sys->p_surver );
		p_sys->p_surver = NULL;
	}

	// ワーク破棄
	sys_FreeMemoryEz( p_sys );

	// ヒープ破棄
	sys_DeleteHeap( HEAPID_WIFI_BATTLEROOM );


	// オーバーレイ破棄
	Overlay_UnloadID( FS_OVERLAY_ID(wifi_2dmapsys) );
	
}

//----------------------------------------------------------------------------
/**
 *	@brief	wifiバトルルーム終了チェック
 *
 *	@param	cp_sys		システムワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
BOOL WBRSys_EndCheck( const WIFI_BATTLEROOM* cp_sys )
{
	if( cp_sys->status == WBRSYS_STATUS_END ){
		return TRUE;
	}
	return FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	バトルルーム管理TCB
 *
 *	@param	tcb			TCBポインタ
 *	@param	p_work		ワーク
 */
//-----------------------------------------------------------------------------
static void WBRSys_ContTcb( TCB_PTR tcb, void* p_work )
{
	WIFI_BATTLEROOM* p_wk = p_work;
	BOOL result;

	switch( p_wk->status ){
	case WBRSYS_STATUS_START_SYNC:
		CommTimingSyncStart(WBR_COMM_SYNC_START);
		p_wk->status = WBRSYS_STATUS_START_SYNCWAIT;
		break;
		
	case WBRSYS_STATUS_START_SYNCWAIT:
		if(!CommIsTimingSync(WBR_COMM_SYNC_START)){
			break;
		}
		p_wk->status = WBRSYS_STATUS_MAIN;
		break;
		
	case WBRSYS_STATUS_MAIN:
		if(p_wk->p_surver){
			// サーバメイン
			WBR_SurverMain( p_wk->p_surver );
		}

		// クライアントメイン
		result = WBR_ClientMain( p_wk->p_client );

		if( result == TRUE ){
			p_wk->status = WBRSYS_STATUS_END_SYNC;
		}
		break;
		
	case WBRSYS_STATUS_END_SYNC:
		CommTimingSyncStart(WBR_COMM_SYNC_END);
		p_wk->status = WBRSYS_STATUS_END_SYNCWAIT;
		break;
		
	case WBRSYS_STATUS_END_SYNCWAIT:
		if(!CommIsTimingSync(WBR_COMM_SYNC_END)){
			break;
		}
		p_wk->status = WBRSYS_STATUS_END;
		break;
		
	case WBRSYS_STATUS_END:
		break;
	}
}
