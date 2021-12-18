//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		scr_wifilobby.c
 *	@brief		WiFiロビーイベント
 *	@author		tomoya takahashi
 *	@data		2008.01.19
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include  "common.h"

#include "application/wifi_lobby.h"

#include "field/fieldsys.h"
#include "scr_wifilobby.h"

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
///	イベントワーク
//=====================================
typedef struct {
	u16 seq;
	u16	check_skip;
	WFLBY_PROC_PARAM param;
} EV_WIFILOBBY_WK;


// オーバーレイID宣言
FS_EXTERN_OVERLAY(wifilobby_common);

// 図鑑プロックデータ
static const PROC_DATA sc_PROC = {	
	WFLBYProc_Init,
	WFLBYProc_Main,
	WFLBYProc_Exit,
	FS_OVERLAY_ID(wifilobby_common)
};



//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
static BOOL GMEVENT_Sub_WiFiLobby(GMEVENT_CONTROL * event);

//----------------------------------------------------------------------------
/**
 *	@brief	WiFi広場プロック　イベント開始
 *
 *	@param	event		イベントワーク
 *	@param	check_skip	最初の接続チェックをスキップするか
 */
//-----------------------------------------------------------------------------
void EventCmd_WiFiLobbyProc( GMEVENT_CONTROL * event, BOOL check_skip )
{
	EV_WIFILOBBY_WK* p_evwk;

    p_evwk = sys_AllocMemoryLo( HEAPID_WORLD, sizeof(EV_WIFILOBBY_WK) );
	memset( p_evwk, 0, sizeof(EV_WIFILOBBY_WK) );
	p_evwk->check_skip = check_skip;
	FieldEvent_Call(event, GMEVENT_Sub_WiFiLobby, p_evwk);
}



//----------------------------------------------------------------------------
/**
 *	@brief	WiFi広場	プロック
 *
 *	@param	event		イベントワーク
 *
 *	@retval	TRUE	終了
 *	@retval	FALSE	途中
 */
//-----------------------------------------------------------------------------
static BOOL GMEVENT_Sub_WiFiLobby( GMEVENT_CONTROL * event )
{
	FIELDSYS_WORK *		p_fsys = FieldEvent_GetFieldSysWork(event);
	EV_WIFILOBBY_WK*	p_evwk = FieldEvent_GetSpecialWork(event);

	// 処理
	switch( p_evwk->seq ){
	case 0:
		{
			p_evwk->param.p_save			= p_fsys->savedata;
			p_evwk->param.check_skip		= p_evwk->check_skip;
			p_evwk->param.p_wflby_counter	= &p_fsys->wflby_counter;
			EventCmd_CallSubProc(event, &sc_PROC, &p_evwk->param);
			p_evwk->seq ++;
		}
		break;

	case 1:
		sys_FreeMemoryEz( p_evwk );
		return TRUE;
	}
	return FALSE;
}
