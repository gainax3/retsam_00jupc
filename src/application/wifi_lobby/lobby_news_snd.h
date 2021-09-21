//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		lobby_news_snd.h
 *	@brief		WiFiロビーニュース	音
 *	@author		tomoya takahashi
 *	@data		2007.10.23
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __LOBBY_NEWS_SND_H__
#define __LOBBY_NEWS_SND_H__

#include "system/snd_tool.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//#define NEWSDRAW_NEWTOPIC	( SEQ_SE_DP_DECIDE )		// 新しいトピック
//#define NEWSDRAW_NEWTOPIC	( SEQ_SE_DP_PIRORIRO )		// 新しいトピック
#define NEWSDRAW_NEWTOPIC	( SEQ_SE_DP_PIRORIRO2 )		// 新しいトピック

#define NEWSDRAW_NEWTIME	( SEQ_SE_DP_DECIDE )		// 時間更新
//#define NEWSDRAW_NEWTIME	( SEQ_SE_DP_DENSI01 )		// 時間更新
//#define NEWSDRAW_NEWTIME	( SEQ_SE_DP_DENSI01 )		// 時間更新

//#define NEWSDRAW_NEWPLAYER	( SEQ_SE_DP_DECIDE )	// 人が入った、帰った
//#define NEWSDRAW_NEWPLAYER	( SEQ_SE_DP_TARARARAN )	// 人が入った、帰った(鳴らない)
//#define NEWSDRAW_NEWPLAYER	( SEQ_SE_DP_DECIDE )		// 人が入った、帰った
//
//他の画面と統一をはかるために変更しました(08.03..07)
//#define NEWSDRAW_NEWPLAYER	( SEQ_SE_DP_BUTTON3 )		// 人が入った、帰った
#define NEWSDRAW_NEWPLAYER	( SEQ_SE_DP_BUTTON9 )		// 人が入った、帰った


#endif		// __LOBBY_NEWS_SND_H__


