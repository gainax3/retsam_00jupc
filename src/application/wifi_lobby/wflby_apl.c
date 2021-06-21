//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_apl.c
 *	@brief		WiFiロビーアプリケーション管理システム
 *	@author		tomoya takahashi
 *	@data		2007.09.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#include "system/procsys.h"
#include "system/pm_overlay.h"

#include "application/bucket.h"
#include "application/balloon.h"
#include "application/balance_ball.h"
#include "application/footprint_main.h"
#include "application/wifi_lobby/wflby_anketo.h"

#include "wflby_apl.h"

// 動作させるプロック
#include "wflby_room.h"
#include "worldtimer.h"
#include "lobby_news.h"
#include "wflby_connect.h"

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
//-------------------------------------
///	アプリで動かすシステム
//=====================================
enum {
	WFLBY_APL_ROOM,
	WFLBY_APL_WORLDTIMER,
	WFLBY_APL_TOPIC,
	WFLBY_APL_FOOT1,
	WFLBY_APL_FOOT2,
	WFLBY_APL_LOGIN,
	WFLBY_APL_BALLSLOW,
	WFLBY_APL_BALANCEBALL,
	WFLBY_APL_BALLOON,
	WFLBY_APL_LOGOUT,
	WFLBY_APL_TIMEOUTLOGOUT,
	WFLBY_APL_ANKETO_INPUT,
	WFLBY_APL_ANKETO_OUTPUT,
	WFLBY_APL_NUM,
} ;



//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	1アプリデータ
//=====================================
typedef struct {
	void* p_param;
} WFLBY_APLDATA;



//-------------------------------------
///	WiFiロビー　アプリ管理システム
//=====================================
typedef struct _WFLBY_APL{
	PROC*			p_proc;					// アプリプロック
	WFLBY_APLDATA	apldata[WFLBY_APL_NUM];	// 動作アプリ分のパラメータ
	u8				aplno;					// 実行中のアプリナンバー
	u8				check_skip;				// 接続画面接続確認をスキップするか
	u16				heapID;					// ヒープID

	// データ関連
	WFLBY_SYSTEM*		p_system;
	SAVEDATA*			p_savedata;
	WFLBY_COUNTER_TIME* p_wflby_counter;
}WFLBY_APL;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
static void WFLBY_APL_PROC_Start( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data, const PROC_DATA* cp_pdata );

// アプリデータ関連
static void WFLBY_APLDATA_Init( WFLBY_APL* p_sys, u32 idx,  u32 heapID );
static void WFLBY_APLDATA_Exit( WFLBY_APL* p_sys, u32 idx );
static void WFLBY_APLDATA_Start( WFLBY_APL* p_sys, u32 idx );
static WFLBY_APL_RET WFLBY_APLDATA_End( WFLBY_APL* p_sys, u32 idx );


// ミニゲーム共通ワーク作成
static void WFLBY_APL_MINIGAME_AllocWk( WFLBY_MINIGAME_WK* p_wk, u32 heapID );
static void WFLBY_APL_MINIGAME_FreeWk( WFLBY_MINIGAME_WK* p_wk );
static void WFLBY_APL_MINIGAME_InitWk( WFLBY_MINIGAME_WK* p_wk, WFLBY_SYSTEM* p_system, u32 heapID );

// 各アプリの関数
// 部屋
static void WFLBY_APLDATA_ROOM_Init( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data,  u32 heapID );
static void WFLBY_APLDATA_ROOM_Exit( WFLBY_APLDATA* p_data );
static void WFLBY_APLDATA_ROOM_Start( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data );
static WFLBY_APL_RET WFLBY_APLDATA_ROOM_End( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data );

// 時計
static void WFLBY_APLDATA_WORLDTIMER_Init( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data,  u32 heapID );
static void WFLBY_APLDATA_WORLDTIMER_Exit( WFLBY_APLDATA* p_data );
static void WFLBY_APLDATA_WORLDTIMER_Start( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data );
static WFLBY_APL_RET WFLBY_APLDATA_WORLDTIMER_End( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data );

// トピック
static void WFLBY_APLDATA_TOPIC_Init( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data,  u32 heapID );
static void WFLBY_APLDATA_TOPIC_Exit( WFLBY_APLDATA* p_data );
static void WFLBY_APLDATA_TOPIC_Start( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data );
static WFLBY_APL_RET WFLBY_APLDATA_TOPIC_End( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data );

// ログイン
static void WFLBY_APLDATA_LOGIN_Init( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data,  u32 heapID );
static void WFLBY_APLDATA_LOGIN_Exit( WFLBY_APLDATA* p_data );
static void WFLBY_APLDATA_LOGIN_Start( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data );
static WFLBY_APL_RET WFLBY_APLDATA_LOGIN_End( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data );

// 玉投げ
static void WFLBY_APLDATA_BS_Init( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data,  u32 heapID );
static void WFLBY_APLDATA_BS_Exit( WFLBY_APLDATA* p_data );
static void WFLBY_APLDATA_BS_Start( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data );
static WFLBY_APL_RET WFLBY_APLDATA_BS_End( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data );

// 玉乗り
static void WFLBY_APLDATA_BB_Init( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data,  u32 heapID );
static void WFLBY_APLDATA_BB_Exit( WFLBY_APLDATA* p_data );
static void WFLBY_APLDATA_BB_Start( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data );
static WFLBY_APL_RET WFLBY_APLDATA_BB_End( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data );

// 風千割
static void WFLBY_APLDATA_BL_Init( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data,  u32 heapID );
static void WFLBY_APLDATA_BL_Exit( WFLBY_APLDATA* p_data );
static void WFLBY_APLDATA_BL_Start( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data );
static WFLBY_APL_RET WFLBY_APLDATA_BL_End( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data );

// 足跡ボード
static void WFLBY_APLDATA_FOOT_Init( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data,  u32 heapID );
static void WFLBY_APLDATA_FOOT_Exit( WFLBY_APLDATA* p_data );
static WFLBY_APL_RET WFLBY_APLDATA_FOOT_End( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data );
static void WFLBY_APLDATA_FOOT1_Start( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data );
static void WFLBY_APLDATA_FOOT2_Start( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data );

// ログアウト
static void WFLBY_APLDATA_LOGOUT_Init( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data,  u32 heapID );
static void WFLBY_APLDATA_LOGOUT_Exit( WFLBY_APLDATA* p_data );
static WFLBY_APL_RET WFLBY_APLDATA_LOGOUT_End( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data );
static void WFLBY_APLDATA_LOGOUT_Start( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data );
static void WFLBY_APLDATA_TIMEOUTLOGOUT_Start( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data );
static void WFLBY_APLDATA_Logout_StartDef( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data, BOOL timeout );

// アンケート
static void WFLBY_APLDATA_ANKETO_InitInput( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data,  u32 heapID );
static void WFLBY_APLDATA_ANKETO_InitOutput( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data,  u32 heapID );
static void WFLBY_APLDATA_ANKETO_Exit( WFLBY_APLDATA* p_data );
static void WFLBY_APLDATA_ANKETO_Start( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data );
static WFLBY_APL_RET WFLBY_APLDATA_ANKETO_End( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data );

//----------------------------------------------------------------------------
/**
 *	@brief	WiFiロビーアプリ管理システム作成
 *
 *	@param	check_skip		接続確認をスキップするか
 *	@param	p_save			セーブデータ
 *	@param	p_wflby_counter	WiFiひろば入室時間格納先
 *	@param	p_system		システムワーク
 *	@param	heapID			ヒープID
 *
 *	@return	システムワーク
 */
//-----------------------------------------------------------------------------
WFLBY_APL* WFLBY_APL_Init( BOOL check_skip, SAVEDATA* p_save, WFLBY_COUNTER_TIME* p_wflby_counter, WFLBY_SYSTEM* p_system, u32 heapID )
{
	WFLBY_APL* p_wk;

	p_wk = sys_AllocMemory( heapID, sizeof(WFLBY_APL) );
	memset( p_wk, 0, sizeof(WFLBY_APL) );

	// データ保存
	p_wk->p_system			= p_system;
	p_wk->p_savedata		= p_save;
	p_wk->p_wflby_counter	= p_wflby_counter;

	// 接続確認フラグ
	p_wk->check_skip	= check_skip;

	// ヒープID保存
	p_wk->heapID		= heapID;

	// アプリデータ初期化
	{
		int i;
		for( i=0; i<WFLBY_APL_NUM; i++ ){
			WFLBY_APLDATA_Init( p_wk, i, heapID );
		}
	}
	

	return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WiFiロビーアプリ管理システム破棄
 *
 *	@param	p_wk	システムワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_APL_Exit( WFLBY_APL* p_wk )
{
	// アプリデータ破棄
	{
		int i;
		for( i=0; i<WFLBY_APL_NUM; i++ ){
			WFLBY_APLDATA_Exit( p_wk, i );
		}
	}
	
	sys_FreeMemoryEz( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	アプリケーションの開始
 *
 *	@param	p_wk	システムワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_APL_Start( WFLBY_APL* p_wk )
{
	WFLBY_APLDATA_Start( p_wk, WFLBY_APL_LOGIN );
}

//----------------------------------------------------------------------------
/**
 *	@brief	WiFiロビーアプリ管理システム	メイン
 *
 *	@param	p_wk	システムワーク
 *
 *	@return	処理状態
 */
//-----------------------------------------------------------------------------
WFLBY_APL_RET WFLBY_APL_Main( WFLBY_APL* p_wk )
{
	BOOL result;
	u32 ret = WFLBY_APL_RET_NONE;
	
	if( p_wk->p_proc ){
		result = ProcMain( p_wk->p_proc );
		if( result ){
			PROC_Delete( p_wk->p_proc );
			p_wk->p_proc = NULL;
			
			//  そのアプリの終了処理
			ret = WFLBY_APLDATA_End( p_wk, p_wk->aplno );
		}
	}
	return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WiFiロビーアプリ管理システム	VBlank処理
 *
 *	@param	p_wk	システムワーク
 */
//-----------------------------------------------------------------------------
void WFLBY_APL_VBlank( WFLBY_APL* p_wk )
{
}


//-----------------------------------------------------------------------------
/**
 *		プライベート関数
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief	プロック開始共通関数
 *
 *	@param	p_sys		システム
 *	@param	p_data		アプリデータ
 *	@param	cp_pdata	プロックデータ
 */
//-----------------------------------------------------------------------------
static void WFLBY_APL_PROC_Start( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data, const PROC_DATA* cp_pdata )
{
	GF_ASSERT( p_sys->p_proc == NULL );
	p_sys->p_proc = PROC_Create( cp_pdata, p_data->p_param, p_sys->heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	アプリワークの初期化
 *
 *	@param	p_sys	アプリシステム
 *	@param	idx		インデックス
 *	@param	heapID	ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_Init( WFLBY_APL* p_sys, u32 idx,  u32 heapID )
{
	static void (*pFunc[])( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data, u32 heapID ) = {
		WFLBY_APLDATA_ROOM_Init,
		WFLBY_APLDATA_WORLDTIMER_Init,
		WFLBY_APLDATA_TOPIC_Init,
		WFLBY_APLDATA_FOOT_Init,
		WFLBY_APLDATA_FOOT_Init,
		WFLBY_APLDATA_LOGIN_Init,
		WFLBY_APLDATA_BS_Init,
		WFLBY_APLDATA_BB_Init,
		WFLBY_APLDATA_BL_Init,
		WFLBY_APLDATA_LOGOUT_Init,
		WFLBY_APLDATA_LOGOUT_Init,
		WFLBY_APLDATA_ANKETO_InitInput,
		WFLBY_APLDATA_ANKETO_InitOutput,
	};
	GF_ASSERT( idx < WFLBY_APL_NUM );
	pFunc[ idx ]( p_sys, &p_sys->apldata[idx], heapID );
} 

//----------------------------------------------------------------------------
/**
 *	@brief	アプリワークの破棄
 *
 *	@param	p_sys	アプリシステム
 *	@param	idx		インデックス
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_Exit( WFLBY_APL* p_sys, u32 idx )
{
	static void (*pFunc[])( WFLBY_APLDATA* p_data ) = {
		WFLBY_APLDATA_ROOM_Exit,
		WFLBY_APLDATA_WORLDTIMER_Exit,
		WFLBY_APLDATA_TOPIC_Exit,
		WFLBY_APLDATA_FOOT_Exit,
		WFLBY_APLDATA_FOOT_Exit,
		WFLBY_APLDATA_LOGIN_Exit,
		WFLBY_APLDATA_BS_Exit,
		WFLBY_APLDATA_BB_Exit,
		WFLBY_APLDATA_BL_Exit,
		WFLBY_APLDATA_LOGOUT_Exit,
		WFLBY_APLDATA_LOGOUT_Exit,
		WFLBY_APLDATA_ANKETO_Exit,
		WFLBY_APLDATA_ANKETO_Exit,
	};
	GF_ASSERT( idx < WFLBY_APL_NUM );
	pFunc[ idx ]( &p_sys->apldata[idx] );
}

//----------------------------------------------------------------------------
/**
 *	@brief	アプリ開始
 *
 *	@param	p_sys	アプリシステム
 *	@param	idx		インデックス
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_Start( WFLBY_APL* p_sys, u32 idx )
{
	static void (*pFunc[])( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data ) = {
		WFLBY_APLDATA_ROOM_Start,
		WFLBY_APLDATA_WORLDTIMER_Start,
		WFLBY_APLDATA_TOPIC_Start,
		WFLBY_APLDATA_FOOT1_Start,
		WFLBY_APLDATA_FOOT2_Start,
		WFLBY_APLDATA_LOGIN_Start,
		WFLBY_APLDATA_BS_Start,
		WFLBY_APLDATA_BB_Start,
		WFLBY_APLDATA_BL_Start,
		WFLBY_APLDATA_LOGOUT_Start,
		WFLBY_APLDATA_TIMEOUTLOGOUT_Start,
		WFLBY_APLDATA_ANKETO_Start,
		WFLBY_APLDATA_ANKETO_Start,
	};
	GF_ASSERT( idx < WFLBY_APL_NUM );
	pFunc[ idx ]( p_sys, &p_sys->apldata[idx] );

	// 開始アプリナンバー保存
	p_sys->aplno = idx;
}

//----------------------------------------------------------------------------
/**
 *	@brief	アプリ終了
 *
 *	@param	p_sys	アプリシステム
 *	@param	idx		インデックス
 *
 *	@return	アプリ戻り値
 */
//-----------------------------------------------------------------------------
static WFLBY_APL_RET WFLBY_APLDATA_End( WFLBY_APL* p_sys, u32 idx )
{
	static WFLBY_APL_RET (*pFunc[])( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data ) = {
		WFLBY_APLDATA_ROOM_End,
		WFLBY_APLDATA_WORLDTIMER_End,
		WFLBY_APLDATA_TOPIC_End,
		WFLBY_APLDATA_FOOT_End,
		WFLBY_APLDATA_FOOT_End,
		WFLBY_APLDATA_LOGIN_End,
		WFLBY_APLDATA_BS_End,
		WFLBY_APLDATA_BB_End,
		WFLBY_APLDATA_BL_End,
		WFLBY_APLDATA_LOGOUT_End,
		WFLBY_APLDATA_LOGOUT_End,
		WFLBY_APLDATA_ANKETO_End,
		WFLBY_APLDATA_ANKETO_End,
	};
	GF_ASSERT( idx < WFLBY_APL_NUM );
	return pFunc[ idx ]( p_sys, &p_sys->apldata[idx] );
}





//----------------------------------------------------------------------------
/**
 *	@brief	ミニゲーム共通ワーク	メモリ確保
 *
 *	@param	p_wk		ワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_APL_MINIGAME_AllocWk( WFLBY_MINIGAME_WK* p_wk, u32 heapID )
{
	int i;
	
	for( i=0; i<WFLBY_MINIGAME_MAX; i++ ){
		p_wk->p_mystate[i] = MyStatus_AllocWork( heapID );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ミニゲーム共通ワーク	メモリ破棄
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_APL_MINIGAME_FreeWk( WFLBY_MINIGAME_WK* p_wk )
{
	int i;
	
	for( i=0; i<WFLBY_MINIGAME_MAX; i++ ){
		sys_FreeMemoryEz( p_wk->p_mystate[i] );
		p_wk->p_mystate[i] = NULL;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ミニゲーム共通ワークの初期化
 *
 *	@param	p_wk		ワーク
 *	@param	p_system	システムワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_APL_MINIGAME_InitWk( WFLBY_MINIGAME_WK* p_wk, WFLBY_SYSTEM* p_system, u32 heapID )
{
	const WFLBY_USER_PROFILE* cp_profile;
	const WFLBY_USER_PROFILE* cp_status_profile;
	int i;

	// ミニゲーム共通ワークを作成
	cp_profile		= WFLBY_SYSTEM_GetMyProfileLocal( p_system );
	p_wk->gadget	= WFLBY_SYSTEM_GetProfileItem( cp_profile );
	WFLBY_SYSTEM_GetVipFlagWk( p_system, &p_wk->vipflag );
	WFLBY_SYSTEM_PLIDX_Get( p_system, &p_wk->plidx );
	p_wk->p_wflbysystem	= p_system;
	p_wk->error_end	= FALSE;

	// それぞれの人のMYSTATUSワーク取得
	for( i=0; i<WFLBY_MINIGAME_MAX; i++ ){
		cp_status_profile = WFLBY_SYSTEM_GetUserProfile( p_system, p_wk->plidx.plidx[i] );
		if( cp_status_profile != NULL ){
			WFLBY_SYSTEM_GetProfileMyStatus( cp_status_profile, p_wk->p_mystate[i], heapID );
		}
	}
}



//----------------------------------------------------------------------------
/**
 *	@brief	部屋パラメータ初期化
 *
 *	@param	p_sys		システム
 *	@param	p_data		データワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_ROOM_Init( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data,  u32 heapID )
{
	WFLBY_ROOM_PARAM* p_param;
	
	p_data->p_param = sys_AllocMemory( heapID, sizeof(WFLBY_ROOM_PARAM) );
	memset( p_data->p_param, 0, sizeof(WFLBY_ROOM_PARAM) );

	p_param = p_data->p_param;

	p_param->p_save		= p_sys->p_savedata;
	p_param->p_system	= p_sys->p_system;
}

//----------------------------------------------------------------------------
/**
 *	@brief	部屋パラメータ破棄
 *
 *	@param	p_data	システム
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_ROOM_Exit( WFLBY_APLDATA* p_data )
{
	GF_ASSERT(p_data->p_param);
	sys_FreeMemoryEz( p_data->p_param );
	p_data->p_param = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	部屋プロック開始
 *
 *	@param	p_sys		システム
 *	@param	p_data		データ
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_ROOM_Start( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data )
{
	WFLBY_ROOM_PARAM* p_param;

	// オーバーレイID宣言
	FS_EXTERN_OVERLAY(wifilobby_room);

	// 図鑑プロックデータ
	static const PROC_DATA Proc = {	
		WFLBY_ROOM_Init,
		WFLBY_ROOM_Main,
		WFLBY_ROOM_Exit,
		FS_OVERLAY_ID(wifilobby_room)
	};
	p_param				= p_data->p_param;
	p_param->season		= WFLBY_SYSTEM_GetSeason( p_sys->p_system );
	p_param->room		= WFLBY_SYSTEM_GetRoomType( p_sys->p_system );
	WFLBY_APL_PROC_Start( p_sys, p_data, &Proc );
}

//----------------------------------------------------------------------------
/**
 *	@brief	部屋プロック終了処理
 *
 *	@param	p_sys	システム
 *	@param	p_data	アプリデータ
 *
 *	@return	アプリシステムの戻り値
 */
//-----------------------------------------------------------------------------
static WFLBY_APL_RET WFLBY_APLDATA_ROOM_End( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data )
{
	WFLBY_ROOM_PARAM* p_param;
	p_param = p_data->p_param;
	
	switch( p_param->ret ){
	// 出口から出たので終了
	case WFLBY_ROOM_RET_ROOM_OUT:			
		WFLBY_APLDATA_Start( p_sys, WFLBY_APL_LOGOUT );
		break;
	// 世界時計へ
	case WFLBY_ROOM_RET_ROOM_WORLDTIMER:
		WFLBY_APLDATA_Start( p_sys, WFLBY_APL_WORLDTIMER );
		break;
	// トピックへ
	case WFLBY_ROOM_RET_ROOM_TOPIC:	
		WFLBY_APLDATA_Start( p_sys, WFLBY_APL_TOPIC );
		break;
	// 足跡ボードへ
	case WFLBY_ROOM_RET_ROOM_FOOT1:	
		WFLBY_APLDATA_Start( p_sys, WFLBY_APL_FOOT1 );
		break;
	// 足跡ボードへ
	case WFLBY_ROOM_RET_ROOM_FOOT2:	
		WFLBY_APLDATA_Start( p_sys, WFLBY_APL_FOOT2 );
		break;
	// 玉投げ
	case WFLBY_ROOM_RET_ROOM_BALLSLOW:		
		WFLBY_APLDATA_Start( p_sys, WFLBY_APL_BALLSLOW );
		break;
	// 玉乗り
	case WFLBY_ROOM_RET_ROOM_BALANCE:		
		WFLBY_APLDATA_Start( p_sys, WFLBY_APL_BALANCEBALL );
		break;
	// 風船割り
	case WFLBY_ROOM_RET_ROOM_BALLOON:		
		WFLBY_APLDATA_Start( p_sys, WFLBY_APL_BALLOON );
		break;
	// タイムアウト終了
	case WFLBY_ROOM_RET_ROOM_TIMEOUT:
		WFLBY_APLDATA_Start( p_sys, WFLBY_APL_TIMEOUTLOGOUT );
		break;

	// アンケート　入力
	case WFLBY_ROOM_RET_ROOM_ANKETO_INPUT:	
		WFLBY_APLDATA_Start( p_sys, WFLBY_APL_ANKETO_INPUT );
		break;

	// アンケート　出力	
	case WFLBY_ROOM_RET_ROOM_ANKETO_OUTPUT:	
		WFLBY_APLDATA_Start( p_sys, WFLBY_APL_ANKETO_OUTPUT );
		break;

#ifdef PM_DEBUG
	case WFLBY_ROOM_RET_ROOM_RETURN:			// 部屋にもう一度戻る
		WFLBY_APLDATA_Start( p_sys, WFLBY_APL_ROOM);
		break;
#endif

	default:
		GF_ASSERT(0);
		break;
	}
	return WFLBY_APL_RET_NONE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	世界時計システム	初期化
 *
 *	@param	p_sys		システムワーク
 *	@param	p_data		アプリデータ
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_WORLDTIMER_Init( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data,  u32 heapID )
{
	WLDTIMER_PARAM* p_param;

	p_data->p_param = sys_AllocMemory( heapID, sizeof(WLDTIMER_PARAM) );
	memset( p_data->p_param, 0, sizeof(WLDTIMER_PARAM) );

	p_param = p_data->p_param;

	// データ設定
	p_param->p_save		= p_sys->p_savedata;
	p_param->p_system	= p_sys->p_system;
}

//----------------------------------------------------------------------------
/**
 *	@brief	世界時計システム	破棄
 *
 *	@param	p_data		アプリデータ
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_WORLDTIMER_Exit( WFLBY_APLDATA* p_data )
{
	sys_FreeMemoryEz( p_data->p_param );
}

//----------------------------------------------------------------------------
/**
 *	@brief	世界時計開始
 *
 *	@param	p_sys	アプリシステム
 *	@param	p_data	アプリデータ
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_WORLDTIMER_Start( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data )
{
	WLDTIMER_PARAM* p_param;

	// オーバーレイID宣言
	FS_EXTERN_OVERLAY(worldtimer);

	// 図鑑プロックデータ
	static const PROC_DATA Proc = {	
		WLDTIMER_Init,
		WLDTIMER_Main,
		WLDTIMER_Exit,
		FS_OVERLAY_ID(worldtimer)
	};

	p_param = p_data->p_param;

	p_param->cp_data	= WFLBY_SYSTEM_GetWldTimer( p_sys->p_system );
	p_param->worldtime	= *(WFLBY_SYSTEM_GetWldTime( p_sys->p_system ));

	WFLBY_APL_PROC_Start( p_sys, p_data, &Proc );
}

//----------------------------------------------------------------------------
/**
 *	@brief	世界時計終了
 *	
 *	@param	p_sys	アプリシステム
 *	@param	p_data	アプリデータ
 *
 *	@return	つねにNONE
 */
//-----------------------------------------------------------------------------
static WFLBY_APL_RET WFLBY_APLDATA_WORLDTIMER_End( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data )
{
	// 部屋が終わっていないかチェック
	if( WFLBY_SYSTEM_Event_GetEnd( p_sys->p_system ) ){
		// タイムアウト終了
		WFLBY_APLDATA_Start( p_sys, WFLBY_APL_TIMEOUTLOGOUT );
	}else{
		// 部屋に戻る
		WFLBY_APLDATA_Start( p_sys, WFLBY_APL_ROOM );
	}
	
	return WFLBY_APL_RET_NONE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	トピックパラメータ初期化
 *
 *	@param	p_sys		アプリシステム
 *	@param	p_data		アプリデータ
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_TOPIC_Init( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data,  u32 heapID )
{
	NEWS_DRAW_DATA* p_param;

	p_data->p_param = sys_AllocMemory( heapID, sizeof(NEWS_DRAW_DATA) );
	memset( p_data->p_param, 0, sizeof(NEWS_DRAW_DATA) );
	
	p_param = p_data->p_param;

	// データ設定
	p_param->p_data			= WFLBY_SYSTEM_GetLobbyData( p_sys->p_system );
	p_param->cp_nowtime		= WFLBY_SYSTEM_GetWldTime( p_sys->p_system );
	p_param->p_system		= p_sys->p_system;
}

//----------------------------------------------------------------------------
/**
 *	@brief	トピックパラメータ破棄
 *
 *	@param	p_data		アプリデータ
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_TOPIC_Exit( WFLBY_APLDATA* p_data )
{
	sys_FreeMemoryEz( p_data->p_param );
}

//----------------------------------------------------------------------------
/**
 *	@brief	トピック開始
 *
 *	@param	p_sys		アプリシステム
 *	@param	p_data		アプリデータ
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_TOPIC_Start( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data )
{
	// オーバーレイID宣言
	FS_EXTERN_OVERLAY(lobbynews);

	// 図鑑プロックデータ
	static const PROC_DATA Proc = {	
		NEWS_DRAW_Init,
		NEWS_DRAW_Main,
		NEWS_DRAW_Exit,
		FS_OVERLAY_ID(lobbynews)
	};

	WFLBY_APL_PROC_Start( p_sys, p_data, &Proc );
}

//----------------------------------------------------------------------------
/**
 *	@brief	トピックプロック終了
 *
 *	@param	p_sys		アプリシステム
 *	@param	p_data		アプリデータ
 *
 *	@return	常にNONE
 */
//-----------------------------------------------------------------------------
static WFLBY_APL_RET WFLBY_APLDATA_TOPIC_End( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data )
{
	// 部屋が終わっていないかチェック
	if( WFLBY_SYSTEM_Event_GetEnd( p_sys->p_system ) ){
		// タイムアウト終了
		WFLBY_APLDATA_Start( p_sys, WFLBY_APL_TIMEOUTLOGOUT );
	}else{
		// 部屋に戻る
		WFLBY_APLDATA_Start( p_sys, WFLBY_APL_ROOM );
	}
	return WFLBY_APL_RET_NONE;
}


//----------------------------------------------------------------------------
/**
 *	@brief		ログインプロック開始
 *
 *	@param	p_sys		アプリシステム
 *	@param	p_data		アプリデータ
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_LOGIN_Init( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data,  u32 heapID )
{
	WFLBY_CONNECT_PARAM* p_param;

	p_data->p_param = sys_AllocMemory( heapID, sizeof(WFLBY_CONNECT_PARAM) );
	memset( p_data->p_param, 0, sizeof(WFLBY_CONNECT_PARAM) );
	
	p_param = p_data->p_param;

	// データ設定
	p_param->p_save				= p_sys->p_savedata;
	p_param->p_system			= p_sys->p_system;
	p_param->check_skip			= p_sys->check_skip;
	p_param->p_wflby_counter	= p_sys->p_wflby_counter;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ログインプロック終了
 *
 *	@param	p_data	アプリデータ
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_LOGIN_Exit( WFLBY_APLDATA* p_data )
{
	WFLBY_CONNECT_PARAM* p_param;
	p_param = p_data->p_param;
	sys_FreeMemoryEz( p_data->p_param );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ログインプロック	開始
 *
 *	@param	p_sys	アプリシステム
 *	@param	p_data	アプリデータ
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_LOGIN_Start( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data )
{
	// オーバーレイID宣言
	FS_EXTERN_OVERLAY(wifilobby_connect);

	// 図鑑プロックデータ
	static const PROC_DATA Proc = {	
		WFLBY_CONNECT_Init,
		WFLBY_CONNECT_Main,
		WFLBY_CONNECT_Exit,
		FS_OVERLAY_ID(wifilobby_connect)
	};

	WFLBY_APL_PROC_Start( p_sys, p_data, &Proc );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ログイン処理	終了
 *
 *	@param	p_sys	アプリシステム
 *	@param	p_data	アプリデータ
 *
 *	@return	常にNONE
 */
//-----------------------------------------------------------------------------
static WFLBY_APL_RET WFLBY_APLDATA_LOGIN_End( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data )
{
	WFLBY_CONNECT_PARAM* p_param;

	
	p_param = p_data->p_param;


	// 入れたかチェック
	if( p_param->enter == FALSE ){
		// おわり
		return WFLBY_APL_RET_END;
	}

	// 部屋に入る
	WFLBY_APLDATA_Start( p_sys, WFLBY_APL_ROOM );
	return WFLBY_APL_RET_NONE;
}



//----------------------------------------------------------------------------
/**
 *	@brief	ボール投げ	パラメータ初期化
 *
 *	@param	p_sys		システム
 *	@param	p_data		データワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_BS_Init( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data,  u32 heapID )
{
	BUCKET_PROC_WORK* p_param;

	p_data->p_param = sys_AllocMemory( heapID, sizeof(BUCKET_PROC_WORK) );
	memset( p_data->p_param, 0, sizeof(BUCKET_PROC_WORK) );
	
	p_param = p_data->p_param;

	p_param->vchat		= FALSE;
	p_param->wifi_lobby	= TRUE;
	p_param->p_save		= p_sys->p_savedata;

	// ミニゲーム共通ワークメモリ確保
	WFLBY_APL_MINIGAME_AllocWk( &p_param->lobby_wk, heapID );

}

//----------------------------------------------------------------------------
/**
 *	@brief	ボール投げパラメータ破棄
 *
 *	@param	p_data		データワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_BS_Exit( WFLBY_APLDATA* p_data )
{
	BUCKET_PROC_WORK* p_param;


	// ミニゲーム共通ワークメモリ破棄
	p_param = p_data->p_param;
	WFLBY_APL_MINIGAME_FreeWk( &p_param->lobby_wk );
	
	sys_FreeMemoryEz( p_data->p_param );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ボール投げアプリ開始
 *
 *	@param	p_sys		システム
 *	@param	p_data		データワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_BS_Start( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data )
{
	FS_EXTERN_OVERLAY(bucket);
	FS_EXTERN_OVERLAY(minigame_common);
	// プロセス定義データ
	static const PROC_DATA Proc = {
		BucketProc_Init,
		BucketProc_Main,
		BucketProc_End,
		FS_OVERLAY_ID(bucket),
	};

	// ミニゲーム共通オーバーレイを読み込む
	Overlay_Load( FS_OVERLAY_ID(minigame_common), OVERLAY_LOAD_NOT_SYNCHRONIZE);

	{
		BUCKET_PROC_WORK* p_param;

		p_param = p_data->p_param;
		WFLBY_APL_MINIGAME_InitWk( &p_param->lobby_wk, p_sys->p_system, p_sys->heapID );
	}
	
	WFLBY_APL_PROC_Start( p_sys, p_data, &Proc );
}

//----------------------------------------------------------------------------
/**
 *	@brief	玉投げ	プロック終了
 *
 *	@param	p_sys		システム
 *	@param	p_data		アプリデータ
 */
//-----------------------------------------------------------------------------
static WFLBY_APL_RET WFLBY_APLDATA_BS_End( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data )
{

	// ミニゲーム共通オーバーレイ破棄
	FS_EXTERN_OVERLAY(minigame_common);
	Overlay_UnloadID( FS_OVERLAY_ID(minigame_common) );


	// ガジェットがアップデートされている可能性があるのでチェック
	// エラー終了したかも設定
	{
		BUCKET_PROC_WORK* p_param;
		p_param = p_data->p_param;
		WFLBY_SYSTEM_MG_SetErrorEnd( p_sys->p_system, p_param->lobby_wk.error_end );
		WFLBY_SYSTEM_SetMyItem( p_sys->p_system, p_param->lobby_wk.gadget );
	}
	
	// 部屋に戻る
	WFLBY_APLDATA_Start( p_sys, WFLBY_APL_ROOM );
	
	return WFLBY_APL_RET_NONE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	BALANCEボールパラメータ初期化
 *
 *	@param	p_sys		アプリシステム
 *	@param	p_data		アプリデータ
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_BB_Init( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data,  u32 heapID )
{
	BB_PROC_WORK* p_param;
	
	p_data->p_param = sys_AllocMemory( heapID, sizeof(BB_PROC_WORK) );
	memset( p_data->p_param, 0, sizeof(BB_PROC_WORK) );

	p_param = p_data->p_param;
	
	p_param->vchat		= FALSE;
	p_param->wifi_lobby	= TRUE;
	p_param->p_save		= p_sys->p_savedata;

	// ミニゲーム共通ワークメモリ確保
	WFLBY_APL_MINIGAME_AllocWk( &p_param->lobby_wk, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	BALANCEボールパラメータ破棄
 *
 *	@param	p_data		データワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_BB_Exit( WFLBY_APLDATA* p_data )
{
	BB_PROC_WORK* p_param;

	// ミニゲーム共通ワークメモリ破棄
	p_param = p_data->p_param;
	WFLBY_APL_MINIGAME_FreeWk( &p_param->lobby_wk );

	sys_FreeMemoryEz( p_data->p_param );
}

//----------------------------------------------------------------------------
/**
 *	@brief	BALANCEボールアプリ開始
 *
 *	@param	p_sys		システム
 *	@param	p_data		データワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_BB_Start( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data )
{
	FS_EXTERN_OVERLAY(balance_ball);
	FS_EXTERN_OVERLAY(minigame_common);
	// プロセス定義データ
	static const PROC_DATA Proc = {
		BalanceBallProc_Init,
		BalanceBallProc_Main,
		BalanceBallProc_Exit,
		FS_OVERLAY_ID(balance_ball),
	};

	// ミニゲーム共通オーバーレイを読み込む
	Overlay_Load( FS_OVERLAY_ID(minigame_common), OVERLAY_LOAD_NOT_SYNCHRONIZE);

	{
		BB_PROC_WORK* p_param;
		p_param = p_data->p_param;
		WFLBY_APL_MINIGAME_InitWk( &p_param->lobby_wk, p_sys->p_system, p_sys->heapID );
	}
	
	WFLBY_APL_PROC_Start( p_sys, p_data, &Proc );
}

//----------------------------------------------------------------------------
/**
 *	@brief	BALANCEボール	プロック終了
 *
 *	@param	p_sys		システム
 *	@param	p_data		アプリデータ
 */
//-----------------------------------------------------------------------------
static WFLBY_APL_RET WFLBY_APLDATA_BB_End( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data )
{
	// ミニゲーム共通オーバーレイ破棄
	FS_EXTERN_OVERLAY(minigame_common);
	Overlay_UnloadID( FS_OVERLAY_ID(minigame_common) );

	// ガジェットがアップデートされている可能性があるのでチェック
	{
		BB_PROC_WORK* p_param;
		p_param = p_data->p_param;
		WFLBY_SYSTEM_MG_SetErrorEnd( p_sys->p_system, p_param->lobby_wk.error_end );
		WFLBY_SYSTEM_SetMyItem( p_sys->p_system, p_param->lobby_wk.gadget );
	}
	
	// 部屋に戻る
	WFLBY_APLDATA_Start( p_sys, WFLBY_APL_ROOM );
	
	return WFLBY_APL_RET_NONE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	風千割	パラメータ初期化
 *
 *	@param	p_sys		システム
 *	@param	p_data		データワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_BL_Init( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data,  u32 heapID )
{
	BALLOON_PROC_WORK* p_param;

	p_data->p_param = sys_AllocMemory( heapID, sizeof(BALLOON_PROC_WORK) );
	memset( p_data->p_param, 0, sizeof(BALLOON_PROC_WORK) );
	
	p_param = p_data->p_param;

	p_param->vchat		= FALSE;
	p_param->wifi_lobby	= TRUE;
	p_param->p_save		= p_sys->p_savedata;

	// ミニゲーム共通ワークメモリ確保
	WFLBY_APL_MINIGAME_AllocWk( &p_param->lobby_wk, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	風千割パラメータ破棄
 *
 *	@param	p_data		データワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_BL_Exit( WFLBY_APLDATA* p_data )
{
	BALLOON_PROC_WORK* p_param;
	
	// ミニゲーム共通ワークメモリ破棄
	p_param = p_data->p_param;
	WFLBY_APL_MINIGAME_FreeWk( &p_param->lobby_wk );
	
	sys_FreeMemoryEz( p_data->p_param );
}

//----------------------------------------------------------------------------
/**
 *	@brief	風千割	アプリ開始
 *
 *	@param	p_sys		システム
 *	@param	p_data		データワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_BL_Start( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data )
{
	FS_EXTERN_OVERLAY(balloon);
	FS_EXTERN_OVERLAY(minigame_common);
	// プロセス定義データ
	static const PROC_DATA Proc = {
		BalloonProc_Init,
		BalloonProc_Main,
		BalloonProc_End,
		FS_OVERLAY_ID(balloon),
	};

	// ミニゲーム共通オーバーレイを読み込む
	Overlay_Load( FS_OVERLAY_ID(minigame_common), OVERLAY_LOAD_NOT_SYNCHRONIZE);

	{
		BALLOON_PROC_WORK* p_param;
		p_param = p_data->p_param;
		WFLBY_APL_MINIGAME_InitWk( &p_param->lobby_wk, p_sys->p_system, p_sys->heapID );
		
	}
	
	WFLBY_APL_PROC_Start( p_sys, p_data, &Proc );
}

//----------------------------------------------------------------------------
/**
 *	@brief	BALANCEボール	プロック終了
 *
 *	@param	p_sys		システム
 *	@param	p_data		アプリデータ
 */
//-----------------------------------------------------------------------------
static WFLBY_APL_RET WFLBY_APLDATA_BL_End( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data )
{
	// ミニゲーム共通オーバーレイ破棄
	FS_EXTERN_OVERLAY(minigame_common);
	Overlay_UnloadID( FS_OVERLAY_ID(minigame_common) );

	// ガジェットがアップデートされている可能性があるのでチェック
	{
		BALLOON_PROC_WORK* p_param;
		p_param = p_data->p_param;
		WFLBY_SYSTEM_MG_SetErrorEnd( p_sys->p_system, p_param->lobby_wk.error_end );
		WFLBY_SYSTEM_SetMyItem( p_sys->p_system, p_param->lobby_wk.gadget );
	}
	
	
	// 部屋に戻る
	WFLBY_APLDATA_Start( p_sys, WFLBY_APL_ROOM );
	
	return WFLBY_APL_RET_NONE;
}



//----------------------------------------------------------------------------
/**
 *	@brief	足跡ボード　パラメータ初期化
 *
 *	@param	p_sys		システム
 *	@param	p_data		データワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_FOOT_Init( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data,  u32 heapID )
{
	FOOTPRINT_PARAM* p_param;
	
	p_data->p_param = sys_AllocMemory( heapID, sizeof(FOOTPRINT_PARAM) );
	memset( p_data->p_param, 0, sizeof(FOOTPRINT_PARAM) );

	p_param = p_data->p_param;

	p_param->wflby_sys		= p_sys->p_system;
}

//----------------------------------------------------------------------------
/**
 *	@brief	足跡ボード	パラメータ破棄
 *
 *	@param	p_data		データワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_FOOT_Exit( WFLBY_APLDATA* p_data )
{
	sys_FreeMemoryEz( p_data->p_param );
}

//----------------------------------------------------------------------------
/**
 *	@brief	足跡ボード終了
 *
 *	@param	p_sys		システムワーク
 *	@param	p_data		データワーク
 */
//-----------------------------------------------------------------------------
static WFLBY_APL_RET WFLBY_APLDATA_FOOT_End( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data )
{
	// 部屋が終わっていないかチェック
	if( WFLBY_SYSTEM_Event_GetEnd( p_sys->p_system ) ){
		// タイムアウト終了
		WFLBY_APLDATA_Start( p_sys, WFLBY_APL_TIMEOUTLOGOUT );
	}else{
		// 部屋に戻る
		WFLBY_APLDATA_Start( p_sys, WFLBY_APL_ROOM );
	}
	
	return WFLBY_APL_RET_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	足跡ボード白開始
 *
 *	@param	p_sys		システムワーク
 *	@param	p_data		データワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_FOOT1_Start( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data )
{
	FOOTPRINT_PARAM* p_param;

	FS_EXTERN_OVERLAY(footprint_board);
	// プロセス定義データ
	static const PROC_DATA Proc = {
		FootPrintProc_Init,
		FootPrintProc_Main,
		FootPrintProc_End,
		FS_OVERLAY_ID(footprint_board),
	};

	p_param = p_data->p_param;
	p_param->board_type		= FOOTPRINT_BOARD_TYPE_WHITE;

	WFLBY_APL_PROC_Start( p_sys, p_data, &Proc );
}

//----------------------------------------------------------------------------
/**
 *	@brief	足跡ボード黒開始
 *
 *	@param	p_sys		システムワーク
 *	@param	p_data		データワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_FOOT2_Start( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data )
{
	FOOTPRINT_PARAM* p_param;

	FS_EXTERN_OVERLAY(footprint_board);
	// プロセス定義データ
	static const PROC_DATA Proc = {
		FootPrintProc_Init,
		FootPrintProc_Main,
		FootPrintProc_End,
		FS_OVERLAY_ID(footprint_board),
	};

	p_param = p_data->p_param;
	p_param->board_type		= FOOTPRINT_BOARD_TYPE_BLACK;

	WFLBY_APL_PROC_Start( p_sys, p_data, &Proc );
}
	

//----------------------------------------------------------------------------
/**
 *	@brief	ログアウト処理パラメータ初期化作成
 *
 *	@param	p_sys		アプリシステム
 *	@param	p_data		アプリデータ
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_LOGOUT_Init( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data,  u32 heapID )
{
	WFLBY_DISCONNECT_PARAM* p_param;

	p_data->p_param = sys_AllocMemory( heapID, sizeof(WFLBY_DISCONNECT_PARAM) );
	memset( p_data->p_param, 0, sizeof(WFLBY_DISCONNECT_PARAM) );

	p_param = p_data->p_param;

	// データ設定
	p_param->p_save			= p_sys->p_savedata;
	p_param->p_system		= p_sys->p_system;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ログアウト処理パラメータ破棄
 *
 *	@param	p_data		データワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_LOGOUT_Exit( WFLBY_APLDATA* p_data )
{
	sys_FreeMemoryEz( p_data->p_param );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ログアウトアプリ開始
 *
 *	@param	p_sys		システム
 *	@param	p_data		データワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_LOGOUT_Start( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data )
{
	WFLBY_APLDATA_Logout_StartDef( p_sys, p_data, FALSE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイムアウトログアウトアプリ開始
 *
 *	@param	p_sys		システム
 *	@param	p_data		データワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_TIMEOUTLOGOUT_Start( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data )
{
	WFLBY_APLDATA_Logout_StartDef( p_sys, p_data, TRUE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ログアウト開始	共通部分
 *
 *	@param	p_sys		システム
 *	@param	p_data		データワーク
 *	@param	timeout		タイムアウト
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_Logout_StartDef( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data, BOOL timeout )
{
	// オーバーレイID宣言
	FS_EXTERN_OVERLAY(wifilobby_connect);

	// 図鑑プロックデータ
	static const PROC_DATA Proc = {	
		WFLBY_DISCONNECT_Init,
		WFLBY_DISCONNECT_Main,
		WFLBY_DISCONNECT_Exit,
		FS_OVERLAY_ID(wifilobby_connect)
	};

	{
		WFLBY_DISCONNECT_PARAM* p_param;
		p_param = p_data->p_param;
		p_param->timeout = timeout;
	}

	WFLBY_APL_PROC_Start( p_sys, p_data, &Proc );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ログアウトアプリ終了
 *	
 *	@param	p_sys		システム
 *	@param	p_data		データワーク
 */
//-----------------------------------------------------------------------------
static WFLBY_APL_RET WFLBY_APLDATA_LOGOUT_End( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data )
{
	// 終わる
	return WFLBY_APL_RET_END;
}


//----------------------------------------------------------------------------
/**
 *	@brief	アンケート	初期化
 *
 *	@param	p_sys		システムワーク
 *	@param	p_data		データワーク
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_ANKETO_InitInput( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data,  u32 heapID )
{
	ANKETO_PARAM* p_param;

	p_data->p_param = sys_AllocMemory( heapID, sizeof(ANKETO_PARAM) );
	memset( p_data->p_param, 0, sizeof(ANKETO_PARAM) );
	
	p_param = p_data->p_param;

	// データ設定
	p_param->p_save			= p_sys->p_savedata;
	p_param->p_system		= p_sys->p_system;
	p_param->move_type		= ANKETO_MOVE_TYPE_INPUT;
}
static void WFLBY_APLDATA_ANKETO_InitOutput( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data,  u32 heapID )
{
	ANKETO_PARAM* p_param;

	p_data->p_param = sys_AllocMemory( heapID, sizeof(ANKETO_PARAM) );
	memset( p_data->p_param, 0, sizeof(ANKETO_PARAM) );
	
	p_param = p_data->p_param;

	// データ設定
	p_param->p_save			= p_sys->p_savedata;
	p_param->p_system		= p_sys->p_system;
	p_param->move_type		= ANKETO_MOVE_TYPE_VIEW;
}

//----------------------------------------------------------------------------
/**
 *	@brief	アンケート	破棄
 *
 *	@param	p_data		データワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_ANKETO_Exit( WFLBY_APLDATA* p_data )
{
	sys_FreeMemoryEz( p_data->p_param );
}

//----------------------------------------------------------------------------
/**
 *	@brief	アンケート	開始
 *	
 *	@param	p_sys		システムワーク
 *	@param	p_data		データワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_APLDATA_ANKETO_Start( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data )
{
	// オーバーレイID宣言
	FS_EXTERN_OVERLAY(wifilobby_anketo);

	// 図鑑プロックデータ
	static const PROC_DATA Proc = {	
		ANKETO_Init,
		ANKETO_Main,
		ANKETO_Exit,
		FS_OVERLAY_ID(wifilobby_anketo)
	};

	WFLBY_APL_PROC_Start( p_sys, p_data, &Proc );
}

//----------------------------------------------------------------------------
/**
 *	@brief	アンケート終了
 *
 *	@param	p_sys		ワーク
 *	@param	p_data		データ
 */
//-----------------------------------------------------------------------------
static WFLBY_APL_RET WFLBY_APLDATA_ANKETO_End( WFLBY_APL* p_sys, WFLBY_APLDATA* p_data )
{
	// 部屋が終わっていないかチェック
	if( WFLBY_SYSTEM_Event_GetEnd( p_sys->p_system ) ){
		// タイムアウト終了
		WFLBY_APLDATA_Start( p_sys, WFLBY_APL_TIMEOUTLOGOUT );
	}else{
		// 部屋に戻る
		WFLBY_APLDATA_Start( p_sys, WFLBY_APL_ROOM );
	}
	return WFLBY_APL_RET_NONE;
}

