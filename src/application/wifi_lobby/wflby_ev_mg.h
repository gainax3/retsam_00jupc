//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_ev_mg.h
 *	@brief		ミニゲームイベント
 *	@author		tomoya takahashi
 *	@data		2008.01.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WFLBY_EV_MG_H__
#define __WFLBY_EV_MG_H__

#include "wflby_event.h"

#include "msgdata/msg_wifi_h_info.h"
#include "msgdata/msg_wifi_hiroba.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	in_okに入っている値
//=====================================
enum{
	WFLBY_EV_MG_RET_NG_NUM,			// 人数がいっぱいで入れない
	WFLBY_EV_MG_RET_NG_PLAYING,		// ゲーム中なので入れない
	WFLBY_EV_MG_RET_NG_TIME,		// ミニゲーム台終了しているので入れない
	WFLBY_EV_MG_RET_NG_MIN,			// 必要な人数がそろわなかった
	WFLBY_EV_MG_RET_NG_DISCON,		// 誰かが切断した
	WFLBY_EV_MG_RET_NG_MY,			// 自ら断った
	WFLBY_EV_MG_RET_NG_BCAN,		// BCancelした
	WFLBY_EV_MG_RET_NG_LOCK,		// 以前親のときにBCancelしたので参加ロック中
	WFLBY_EV_MG_RET_OK,
};
//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	ミニゲーム参加パラメータ
//=====================================
typedef struct {
	u8 in_ok;			// 募集に参加できたか
	u8 mg_type;			// DWCミニゲームタイプ
	u8 wflby_mg_type;	// ロビー内ミニゲームタイプ
	u8 wflby_mg_status;	// 参加後に設定するステータス
} WFLBY_EV_MG_PARAM;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
extern BOOL WFLBY_EV_MG_Start( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno );
extern BOOL WFLBY_EV_MG_End( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno );
extern BOOL WFLBY_EV_DEF_PlayerA_MINIGAME_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno );

#endif		// __WFLBY_EV_MG_H__

