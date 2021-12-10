//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_event.c
 *	@brief		イベント管理システム
 *	@author		tomoya takahashi
 *	@data		2007.11.26
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#include "wflby_def.h"
#include "wflby_event.h"

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


#ifdef PM_DEBUG
#define WFLBY_DEBUG_EVENT_PRINT		// イベントプリント
#endif

// デバックプリント設定
#ifdef WFLBY_DEBUG_EVENT_PRINT
#define WFLBY_EVENT_DEBUG_PRINT(...)		OS_TPrintf(__VA_ARGS__)
#else
#define WFLBY_EVENT_DEBUG_PRINT(...)		((void)0);
#endif


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	イベントタイミング定数
//=====================================
enum {
	WFLBY_EVENT_FUNC_BEFORE,
	WFLBY_EVENT_FUNC_AFTER,
} ;


//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	イベントデータ
//=====================================
typedef struct {
	const WFLBY_EVENT_DATA*	cp_data;	// イベントデータ
	void*					p_wk;		// ローカルワーク
	void*					p_param;	// パラメータ
	u32						seq;		// シーケンス
} WFLBY_EVENTWK_LOCAL;

//-------------------------------------
///	イベントワーク
//=====================================
typedef struct _WFLBY_EVENTWK{
	// ヒープID
	u16				heapID;

	// イベント動作フラグ
	u8				evmove;

	// イベントNO
	u8				evno;

	// 実行データ
	WFLBY_EVENTWK_LOCAL data;

	// 対比先
	WFLBY_EVENTWK_LOCAL tmp;
} WFLBY_EVENTWK;



//-------------------------------------
///	イベント管理システム
//=====================================
typedef struct _WFLBY_EVENT {
	// ヒープID
	u32				heapID;

	// ロビーデータ
	WFLBY_ROOMWK*	p_rmwk;

	// 個別イベントデータ
	WFLBY_EVENTWK	private[ WFLBY_PLAYER_MAX ];

	// 全体イベントデータ
	WFLBY_EVENTWK	global;
}WFLBY_EVENT;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	システム管理関数
//=====================================
static void WFLBY_EVENT_MainLocal( WFLBY_EVENT* p_sys, u32 timing );



//-------------------------------------
///	イベント管理システム関数
//=====================================
static void WFLBY_EVENTWK_Init( WFLBY_EVENTWK* p_wk, u32 evno, u32 heapID );
static void WFLBY_EVENTWK_Exit( WFLBY_EVENTWK* p_wk );
static void WFLBY_EVENTWK_Main( WFLBY_EVENTWK* p_wk, WFLBY_EVENT* p_sys, u32 timing, u32 plno );
static void WFLBY_EVENTWK_Set( WFLBY_EVENTWK* p_wk, const WFLBY_EVENT_DATA* cp_data, void* p_param );
static void WFLBY_EVENTWK_SetMove( WFLBY_EVENTWK* p_wk, BOOL move );
static void WFLBY_EVENTWK_Push( WFLBY_EVENTWK* p_wk, const WFLBY_EVENT_DATA* cp_data, void* p_param );
static void WFLBY_EVENTWK_Pop( WFLBY_EVENTWK* p_wk );
static BOOL WFLBY_EVENTWK_CheckIntrEnd( const WFLBY_EVENTWK* cp_wk );
static BOOL WFLBY_EVENTWK_CheckEventDo( const WFLBY_EVENTWK* cp_wk );
static void WFLBY_EVENTWKCL_SetData( WFLBY_EVENTWK_LOCAL* p_wk, const WFLBY_EVENT_DATA* cp_data, void* p_param, u32 seq, void* p_evwk );
static void WFLBY_EVENTWKCL_Clean( WFLBY_EVENTWK_LOCAL* p_wk );








//-------------------------------------
// イベントシステム管理
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	イベントシステム	初期化	
 *
 *	@param	p_rmwk		ROOMワーク
 *	@param	heapID		ヒープID
 *
 *	@return	イベントシステム
 */
//-----------------------------------------------------------------------------
WFLBY_EVENT* WFLBY_EVENT_Init( WFLBY_ROOMWK* p_rmwk, u32 heapID )
{
	WFLBY_EVENT* p_sys;

	p_sys = sys_AllocMemory( heapID, sizeof(WFLBY_EVENT) );
	memset( p_sys, 0, sizeof(WFLBY_EVENT) );

	// ヒープ保存
	p_sys->heapID = heapID;

	// 部屋データを保存
	p_sys->p_rmwk = p_rmwk;

	// 個別イベント初期設定
	{
		int i;
		for( i=0; i<WFLBY_PLAYER_MAX; i++ ){
			WFLBY_EVENTWK_Init( &p_sys->private[i], i, heapID );
		}
	}

	// 全体イベント初期設定
	WFLBY_EVENTWK_Init( &p_sys->global, 0, heapID );

	return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	イベントシステム	破棄
 *
 *	@param	p_sys	システムワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_EVENT_Exit( WFLBY_EVENT* p_sys )
{
	// 個別イベント破棄
	{
		int i;
		for( i=0; i<WFLBY_PLAYER_MAX; i++ ){
			WFLBY_EVENTWK_Exit( &p_sys->private[i] );
		}
	}

	// 全体イベントデータ破棄
	WFLBY_EVENTWK_Exit( &p_sys->global );


	// システム破棄
	sys_FreeMemoryEz( p_sys );
}

//-------------------------------------
// 各実行関数
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	動作オブジェ動作後実行関数
 *
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_EVENT_MainBefore( WFLBY_EVENT* p_sys )
{
	WFLBY_EVENT_MainLocal( p_sys, WFLBY_EVENT_FUNC_BEFORE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	動作オブジェリクエスト設定後実行関数
 *
 *	@param	p_sys		システムワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_EVENT_MainAfter( WFLBY_EVENT* p_sys )
{
	WFLBY_EVENT_MainLocal( p_sys, WFLBY_EVENT_FUNC_AFTER );
}


//----------------------------------------------------------------------------
/**
 *	@brief	イベントパラメータを取得する
 *
 *	@param	p_wk	ワーク
 *
 *	@return	パラメータ
 */
//-----------------------------------------------------------------------------
void* WFLBY_EVENTWK_GetParam( WFLBY_EVENTWK* p_wk )
{
	return p_wk->data.p_param;
}

//----------------------------------------------------------------------------
/**
 *	@brief	イベント作業領域を作成する
 *
 *	@param	p_wk		ワーク
 *	@param	size		サイズ
 */
//-----------------------------------------------------------------------------
void* WFLBY_EVENTWK_AllocWk( WFLBY_EVENTWK* p_wk, u32 size )
{
	GF_ASSERT( p_wk->data.p_wk == NULL );
	p_wk->data.p_wk = sys_AllocMemory( p_wk->heapID, size );
	memset( p_wk->data.p_wk, 0, size );
	return p_wk->data.p_wk;
}


//----------------------------------------------------------------------------
/**
 *	@brief	イベント作業領域を破棄する
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_EVENTWK_DeleteWk( WFLBY_EVENTWK* p_wk )
{
	GF_ASSERT( p_wk->data.p_wk != NULL );
	sys_FreeMemoryEz( p_wk->data.p_wk );
	p_wk->data.p_wk = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	イベント作業領域を取得する
 *
 *	@param	p_wk		ワーク
 *
 *	@return	作業領域
 */
//-----------------------------------------------------------------------------
void* WFLBY_EVENTWK_GetWk( WFLBY_EVENTWK* p_wk )
{
	return p_wk->data.p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	シーケンスを取得する
 *
 *	@param	cp_wk	ワーク
 *
 *	@return	シーケンス
 */
//-----------------------------------------------------------------------------
u32 WFLBY_EVENTWK_GetSeq( const WFLBY_EVENTWK* cp_wk )
{
	return  cp_wk->data.seq;
}

//----------------------------------------------------------------------------
/**
 *	@brief	シーケンスを設定する
 *
 *	@param	p_wk		ワーク
 *	@param	seq			シーケンス
 */
//-----------------------------------------------------------------------------
void WFLBY_EVENTWK_SetSeq( WFLBY_EVENTWK* p_wk, u32 seq )
{
	
	p_wk->data.seq = seq;
	WFLBY_EVENT_DEBUG_PRINT( "event no = %d seq = %d\n", p_wk->evno, seq );
}

//----------------------------------------------------------------------------
/**
 *	@brief	シー件数を１進める
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_EVENTWK_AddSeq( WFLBY_EVENTWK* p_wk )
{
	p_wk->data.seq ++;
	WFLBY_EVENT_DEBUG_PRINT( "event no = %d seq = %d\n", p_wk->evno, p_wk->data.seq );
}

//-------------------------------------
// 個別イベント管理
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	常駐イベントを設定する
 *
 *	@param	p_sys		システムワーク
 *	@param	plno		プレイヤーID
 *	@param	cp_data		イベントデータ
 *	@param	p_param	パラメータ
 */
//-----------------------------------------------------------------------------
void WFLBY_EVENT_SetPrivateEvent( WFLBY_EVENT* p_sys, u32 plno, const WFLBY_EVENT_DATA* cp_data, void* p_param )
{
	GF_ASSERT( p_sys );
	GF_ASSERT( plno < WFLBY_PLAYER_MAX );

	// 常駐イベントデータ設定
	WFLBY_EVENTWK_Set( &p_sys->private[ plno ], cp_data, p_param );
}

//----------------------------------------------------------------------------
/**
 *	@brief	割り込みイベントデータ設定
 *
 *	@param	p_sys		システムワーク
 *	@param	plno		プレイヤーNO
 *	@param	cp_data		データ
 *	@param	p_param	パラメータ
 */
//-----------------------------------------------------------------------------
void WFLBY_EVENT_SetPrivateIntrEvent( WFLBY_EVENT* p_sys, u32 plno, const WFLBY_EVENT_DATA* cp_data, void* p_param )
{
	GF_ASSERT( p_sys );
	GF_ASSERT( plno < WFLBY_PLAYER_MAX );

	// 割り込みイベント設定
	WFLBY_EVENTWK_Push( &p_sys->private[ plno ], cp_data, p_param );
}

//----------------------------------------------------------------------------
/**
 *	@brief	割り込みイベント終了チェック
 *
 *	@param	cp_sys		システムワーク
 *	@param	plno		プレイヤーNO
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	割り込み中
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_EVENT_WaitPrivateIntrEvent( const WFLBY_EVENT* cp_sys, u32 plno )
{
	GF_ASSERT( cp_sys );
	GF_ASSERT( plno < WFLBY_PLAYER_MAX );

	return WFLBY_EVENTWK_CheckIntrEnd( &cp_sys->private[ plno ] );
}


#ifdef PM_DEBUG
// デバック
void WFLBY_EVENT_SetPrivateEvent_DEBUG( WFLBY_EVENT* p_sys, u32 plno, const WFLBY_EVENT_DATA* cp_data, void* p_param, u32 line )
{
	WFLBY_EVENT_DEBUG_PRINT( "event set plno=%d line=%d\n", plno, line );
	WFLBY_EVENT_SetPrivateEvent( p_sys, plno, cp_data, p_param );
}

void WFLBY_EVENT_SetPrivateIntrEvent_DEBUG( WFLBY_EVENT* p_sys, u32 plno, const WFLBY_EVENT_DATA* cp_data, void* p_param, u32 line )
{
	WFLBY_EVENT_DEBUG_PRINT( "event set intr plno=%d line=%d\n", plno, line );
	WFLBY_EVENT_SetPrivateIntrEvent( p_sys, plno, cp_data, p_param );
}

#endif


//----------------------------------------------------------------------------
/**
 *	@brief	イベントの動作設定
 *
 *	@param	p_sys		システム
 *	@param	plno		操作するイベントのPLNO
 *	@param	move		動作フラグ	TRUE：動作		FALSE：非動作
 *
 */
//-----------------------------------------------------------------------------
void WFLBY_EVENT_SetPrivateEventMove( WFLBY_EVENT* p_sys, u32 plno, BOOL move )
{
	GF_ASSERT( p_sys );
	GF_ASSERT( plno < WFLBY_PLAYER_MAX );

	WFLBY_EVENTWK_SetMove( &p_sys->private[ plno ], move );
}



//----------------------------------------------------------------------------
/**
 *	@brief	全体イベントを開始
 *
 *	@param	p_sys		システムワーク
 *	@param	cp_data		イベントデータ 
 *	@param	p_param	パラメータ
 */
//-----------------------------------------------------------------------------
void WFLBY_EVENT_SetGlobalEvent( WFLBY_EVENT* p_sys, const WFLBY_EVENT_DATA* cp_data, void* p_param )
{
	GF_ASSERT( p_sys );

	// 割り込みイベント設定
	WFLBY_EVENTWK_Push( &p_sys->global, cp_data, p_param );
}

//----------------------------------------------------------------------------
/**
 *	@brief	グローバルイベントが終了したかチェック
 *
 *	@param	cp_sys	システムワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
BOOL WFLBY_EVENT_WaitGlobalEvent( const WFLBY_EVENT* cp_sys )
{
	GF_ASSERT( cp_sys );
	return WFLBY_EVENTWK_CheckIntrEnd( &cp_sys->global );
}




//-----------------------------------------------------------------------------
/**
 *			プライベート関数
 */
//-----------------------------------------------------------------------------
static void WFLBY_EVENT_MainLocal( WFLBY_EVENT* p_sys, u32 timing )
{

	// 全体イベント実行が最優先
	if( WFLBY_EVENTWK_CheckEventDo( &p_sys->global ) == TRUE ){
		WFLBY_EVENTWK_Main( &p_sys->global, p_sys, timing, 0 );
		return ;
	}

	// プライベートイベント実行
	{
		int i;

		for( i=0; i<WFLBY_PLAYER_MAX; i++ ){
			if( WFLBY_EVENTWK_CheckEventDo( &p_sys->private[i] ) == TRUE ){
				WFLBY_EVENTWK_Main( &p_sys->private[i], p_sys, timing, i );
			}
		}
	}
}




//----------------------------------------------------------------------------
/**
 *	@brief	イベントワーク初期化
 *
 *	@param	p_wk		ワーク
 *	@param	evno		イベントナンバー
 *	@param	heapID		ヒープ
 */
//-----------------------------------------------------------------------------
static void WFLBY_EVENTWK_Init( WFLBY_EVENTWK* p_wk, u32 evno, u32 heapID )
{
	p_wk->heapID = heapID;
	p_wk->evmove = TRUE;
	p_wk->evno	 = evno;
}

//----------------------------------------------------------------------------
/**
 *	@brief	イベントワーク破棄
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_EVENTWK_Exit( WFLBY_EVENTWK* p_wk )
{
	//  実行データ破棄
	if( p_wk->data.p_wk != NULL ){
		sys_FreeMemoryEz( p_wk->data.p_wk );
	}
	if( p_wk->tmp.p_wk != NULL ){
		sys_FreeMemoryEz( p_wk->tmp.p_wk );
	}
	memset( p_wk, 0, sizeof(WFLBY_EVENTWK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	メイン実行
 *
 *	@param	p_wk		ワーク
 *	@param	p_sys		システム
 *	@param	timing		実行タイミング
 *	@param	plno		プレイヤーNO
 */
//-----------------------------------------------------------------------------
static void WFLBY_EVENTWK_Main( WFLBY_EVENTWK* p_wk, WFLBY_EVENT* p_sys, u32 timing, u32 plno )
{
	BOOL result;
	pEVENT_FUNC p_func;

	// イベント動作チェック
	if( p_wk->evmove == FALSE ){
		return ;
	}

	// タイミングによって読み替える
	switch( timing ){
	case WFLBY_EVENT_FUNC_BEFORE:
		p_func = p_wk->data.cp_data->p_before;
		break;
	case WFLBY_EVENT_FUNC_AFTER:
		p_func = p_wk->data.cp_data->p_after;
		break;
	default:
		GF_ASSERT(0);	// ありえない
		break;
	}
	if( p_func != NULL ){
		result = p_func( p_wk,
				p_sys->p_rmwk, plno );

		// 終了が帰ってきたら
		if( result == TRUE ){
			// 退避データを常駐に戻す
			WFLBY_EVENTWK_Pop( p_wk );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	イベントデータを設定する
 *
 *	@param	p_wk		ワーク
 *	@param	cp_data		データ
 *	@param	p_param	パラメータ
 */
//-----------------------------------------------------------------------------
static void WFLBY_EVENTWK_Set( WFLBY_EVENTWK* p_wk, const WFLBY_EVENT_DATA* cp_data, void* p_param )
{
	// 割り込み中でないかチェック
	GF_ASSERT_MSG( WFLBY_EVENTWK_CheckIntrEnd( p_wk ) == TRUE, "intr event\n" );

	// データが残ってないかチェック
	GF_ASSERT_MSG( p_wk->data.p_wk == NULL, "rest wk\n" );

	WFLBY_EVENTWKCL_SetData( &p_wk->data, cp_data, p_param, 0, NULL );
}

//----------------------------------------------------------------------------
/**
 *	@brief	動作フラグを設定する
 *
 *	@param	p_wk		ワーク
 *	@param	move		動作フラグ
 */
//-----------------------------------------------------------------------------
static void WFLBY_EVENTWK_SetMove( WFLBY_EVENTWK* p_wk, BOOL move )
{
	p_wk->evmove = move;
}

//----------------------------------------------------------------------------
/**
 *	@brief	イベントワーク	割り込みイベントを設定	常駐イベントを退避
 *
 *	@param	p_wk		ワーク
 *	@param	cp_data		設定する
 *	@param	p_param	パラメータ
 */
//-----------------------------------------------------------------------------
static void WFLBY_EVENTWK_Push( WFLBY_EVENTWK* p_wk, const WFLBY_EVENT_DATA* cp_data, void* p_param )
{
	// 割り込み起動中に割り込み設定できない
	GF_ASSERT_MSG( WFLBY_EVENTWK_CheckIntrEnd( p_wk ) == TRUE, "event tmp over\n" );

	// 退避
	p_wk->tmp	= p_wk->data;

	// 設定
	WFLBY_EVENTWKCL_SetData( &p_wk->data, cp_data, p_param, 0, NULL );
}

//----------------------------------------------------------------------------
/**
 *	@brief	イベントワーク	常駐イベントを元に戻す
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static void WFLBY_EVENTWK_Pop( WFLBY_EVENTWK* p_wk )
{
	// 退避先があるかチェック
//	GF_ASSERT( p_wk->cp_tmp != NULL, "event tmp none\n" );	// やっぱり退避先がなくてもよい
	// データが残ってないかチェック
	GF_ASSERT( p_wk->data.p_wk == NULL );

	// 退避先を設定
	p_wk->data		= p_wk->tmp;

	// 退避先のクリア
	WFLBY_EVENTWKCL_Clean( &p_wk->tmp );
}

//----------------------------------------------------------------------------
/**
 *	@brief	割り込みイベントが終了したのかチェックする
 *
 *	@param	cp_wk	イベントワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EVENTWK_CheckIntrEnd( const WFLBY_EVENTWK* cp_wk )
{
	if( cp_wk->tmp.cp_data == NULL ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	実行するイベントがあるのかチェック
 *
 *	@param	cp_wk	 イベントワーク
 *
 *	@retval	TRUE	実行する
 *	@retval	FALSE	実行しない
 */
//-----------------------------------------------------------------------------
static BOOL WFLBY_EVENTWK_CheckEventDo( const WFLBY_EVENTWK* cp_wk )
{
	if( cp_wk->data.cp_data == NULL ){
		return FALSE;
	}
	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	イベントデータにデータを設定する
 *
 *	@param	p_wk		ワーク
 *	@param	cp_data		データ
 *	@param	p_param		パラメータ
 *	@param	seq			シーケンス
 *	@param	p_evwk		イベントローカルワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_EVENTWKCL_SetData( WFLBY_EVENTWK_LOCAL* p_wk, const WFLBY_EVENT_DATA* cp_data, void* p_param, u32 seq, void* p_evwk )
{
	p_wk->cp_data	= cp_data;
	p_wk->seq		= seq;
	p_wk->p_wk		= p_evwk;
	p_wk->p_param	= p_param;
}

//----------------------------------------------------------------------------
/**
 *	@brief	イベントデータのデータをクリーンする
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_EVENTWKCL_Clean( WFLBY_EVENTWK_LOCAL* p_wk )
{
	p_wk->cp_data	= NULL;
	p_wk->seq		= 0;
	p_wk->p_wk		= NULL;
	p_wk->p_param	= NULL;
}

