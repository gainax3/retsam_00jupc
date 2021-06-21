//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wifi_lobby.c
 *	@brief		WiFiロビーメインシステム
 *	@author		tomoya takahashi
 *	@data		2007.09.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"

#include "system/pm_overlay.h"

#include "savedata/savedata_def.h"

#include "application/wifi_lobby.h"

#include "communication/communication.h"

#include "wifi/dwc_overlay.h"

#include "wflby_system.h"
#include "wflby_apl.h"

#include <ppwlobby/ppw_lobby.h>

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
///	WiFiロビーメインワーク
//=====================================
typedef struct {
	SAVEDATA*		p_save;		// セーブデータ
	WFLBY_SYSTEM*	p_commsys;	// WiFiロビー共通処理システム
	WFLBY_APL*		p_apl;		// WiFiロビーアプリ管理システム
	TCB_PTR p_vtcb;				// VBLANKTCB
} WFLBY_WK;



//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
static void WFLBY_VBlankFunc( TCB_PTR p_tcb, void* p_work );


//----------------------------------------------------------------------------
/**
 *	@brief	初期化処理
 *
 *	@param	p_proc		ワーク
 *	@param	p_seq		シーケンス
 *
 *	@retval	PROC_RES_CONTINUE = 0,		///<動作継続中
 *	@retval	PROC_RES_FINISH,			///<動作終了
 */
//-----------------------------------------------------------------------------
PROC_RESULT WFLBYProc_Init( PROC* p_proc, int* p_seq )
{
	WFLBY_WK* p_wk;
	WFLBY_PROC_PARAM* p_param;

	{
		// オーバーレイ
		FS_EXTERN_OVERLAY(wifi_2dmapsys);
		Overlay_Load( FS_OVERLAY_ID(wifi_2dmapsys), OVERLAY_LOAD_NOT_SYNCHRONIZE);

		// WiFiオーバーレイ開始
		DwcOverlayStart();

		// 世界交換（ghttpライブラリのため）
		DpwCommonOverlayStart();
	}
	
	// ヒープ作成
	sys_CreateHeap( HEAPID_BASE_APP, HEAPID_WFLOBBY, 0x5000 );

	// ワーク作成
	p_wk = PROC_AllocWork( p_proc, sizeof(WFLBY_WK), HEAPID_WFLOBBY );
	memset( p_wk, 0, sizeof(WFLBY_WK) );

	// パラメータワーク取得
	p_param = PROC_GetParentWork( p_proc );

	// セーブデータ取得
	p_wk->p_save = p_param->p_save;

	// 共通処理システム作成
	p_wk->p_commsys = WFLBY_SYSTEM_Init( p_wk->p_save, HEAPID_WFLOBBY );
	p_wk->p_apl = WFLBY_APL_Init( p_param->check_skip, p_param->p_save, 
			p_param->p_wflby_counter, p_wk->p_commsys, HEAPID_WFLOBBY );

	// VブランクHブランク関数設定
	sys_VBlankFuncChange( NULL, NULL );	// VBlankセット
	sys_HBlankIntrStop();	//HBlank割り込み停止


	// 常時動作VBLANKタスク生成
	p_wk->p_vtcb = VWaitTCB_Add( WFLBY_VBlankFunc, p_wk, 0 );

	// プロック開始
	WFLBY_APL_Start( p_wk->p_apl );
	
	return PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	メイン処理
 *
 *	@param	p_proc		ワーク
 *	@param	p_seq		シーケンス
 *
 *	@retval	PROC_RES_CONTINUE = 0,		///<動作継続中
 *	@retval	PROC_RES_FINISH,			///<動作終了
 */
//-----------------------------------------------------------------------------
PROC_RESULT WFLBYProc_Main( PROC* p_proc, int* p_seq )
{
	WFLBY_WK* p_wk;
	WFLBY_APL_RET apl_ret;

	p_wk = PROC_GetWork( p_proc );

	// アプリメイン
	apl_ret = WFLBY_APL_Main( p_wk->p_apl );

	// 共通処理メイン
	// 位置はアプリのあと固定！！！！
	WFLBY_SYSTEM_Main( p_wk->p_commsys );

	// 終了チェック
	if( apl_ret == WFLBY_APL_RET_END ){
		return PROC_RES_FINISH;
	}
	return PROC_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	破棄処理
 *
 *	@param	p_proc		ワーク
 *	@param	p_seq		シーケンス
 *
 *	@retval	PROC_RES_CONTINUE = 0,		///<動作継続中
 *	@retval	PROC_RES_FINISH,			///<動作終了
 */
//-----------------------------------------------------------------------------
PROC_RESULT WFLBYProc_Exit( PROC* p_proc, int* p_seq )
{
	WFLBY_WK* p_wk;

	p_wk = PROC_GetWork( p_proc );

	// 常時動作VブランクTCB破棄
	TCB_Delete( p_wk->p_vtcb );
	
	// VブランクHブランク関数設定
	sys_VBlankFuncChange( NULL, NULL );	// VBlankセット
	sys_HBlankIntrStop();	//HBlank割り込み停止

	// アプリシステム破棄
	WFLBY_APL_Exit( p_wk->p_apl );

	// 共通処理システム破棄
	WFLBY_SYSTEM_Exit( p_wk->p_commsys );

	// ワーク破棄
	PROC_FreeWork( p_proc );

	// ヒープ破棄
	sys_DeleteHeap( HEAPID_WFLOBBY );

	// ２Dマップシステムをオーバーレイを破棄
	{
		FS_EXTERN_OVERLAY(wifi_2dmapsys);
		Overlay_UnloadID( FS_OVERLAY_ID(wifi_2dmapsys) );

		// 世界交換（ghttpライブラリのため）
		DpwCommonOverlayEnd();

		// WiFiオーバーレイ終了
		DwcOverlayEnd();
	}

	return PROC_RES_FINISH;
}





//-----------------------------------------------------------------------------
/**
 *				プライベート関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	VBlank関数
 *
 *	@param	p_tcb		TCBポインタ
 *	@param	p_work		ワーク
 */
//-----------------------------------------------------------------------------
static void WFLBY_VBlankFunc( TCB_PTR p_tcb, void* p_work )
{
	WFLBY_WK* p_wk;
	p_wk = p_work;

	WFLBY_APL_VBlank( p_wk->p_apl );
	WFLBY_SYSTEM_VBlank( p_wk->p_commsys );
}

