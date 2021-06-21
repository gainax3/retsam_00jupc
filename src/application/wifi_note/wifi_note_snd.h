//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wifi_note_snd.h
 *	@brief		友達手帳　サウンド
 *	@author		tomoya takahshi
 *	@data		2007.07.24
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WIFI_NOTE_SND_H__
#define __WIFI_NOTE_SND_H__

#include "system/snd_tool.h"



// SE用定義
//#define WIFINOTE_MOVE_SE		(SEQ_SE_DP_SELECT)
#define WIFINOTE_MOVE_SE		(SEQ_SE_DP_BUTTON3)

#define WIFINOTE_MENU_MOVE_SE	(SEQ_SE_DP_SELECT)	// メニューの移動音

//#define WIFINOTE_DECIDE_SE	(SEQ_SE_DP_SELECT)
//#define WIFINOTE_DECIDE_SE	(SEQ_SE_DP_BUTTON3)
#define WIFINOTE_DECIDE_SE		(SEQ_SE_DP_DECIDE)

#define WIFINOTE_MENU_DECIDE_SE	(SEQ_SE_DP_SELECT)	// メニューの決定

//#define WIFINOTE_BS_SE		(SEQ_SE_DP_SELECT)
//#define WIFINOTE_BS_SE		(SEQ_SE_DP_BUTTON3)
#define WIFINOTE_BS_SE			(SEQ_SE_DP_DECIDE)

#define WIFINOTE_MENU_BS_SE		(SEQ_SE_DP_SELECT)	// メニューのキャンセル

#define WIFINOTE_CLEAN_SE		(SEQ_SE_DP_UG_025)	// 消しゴムで消す音

//#define WIFINOTE_SCRLL_SE		(SEQ_SE_DP_BOX02)	// スクロール音
#define WIFINOTE_SCRLL_SE		(SEQ_SE_DP_SELECT5)	// スクロール音

#endif		// __WIFI_NOTE_SND_H__

