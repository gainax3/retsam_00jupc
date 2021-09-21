//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_ev_talk.h
 *	@brief		会話イベント
 *	@author		tomoya takahashi
 *	@data		2007.12.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WFLBY_EV_TALK_H__
#define __WFLBY_EV_TALK_H__

#include "wflby_event.h"

#ifdef PM_DEBUG
//#define WFLBY_DEBUG_TALK_ALLWORLD	// トピック前の兄ちゃんが世界の挨拶を流す
#endif

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


// 会話はなしかけた
extern BOOL WFLBY_EV_TALK_StartA( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno );

// 会話はなしかけられた
extern BOOL WFLBY_EV_TALK_StartB( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno );

// トピック会話
extern BOOL WFLBY_EV_DEF_PlayerA_SWTOPIC_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno );

// 遊びを推薦する人
extern BOOL WFLBY_EV_DEF_PlayerA_SWTOPIC_PLAYED_Before( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno );



#ifdef WFLBY_DEBUG_TALK_ALLWORLD
extern BOOL WFLBY_EV_TALK_StartA_AllWorld( WFLBY_EVENTWK* p_wk, WFLBY_ROOMWK* p_rmwk, u32 plno );
#endif

#endif		// __WFLBY_EV_TALK_H__

