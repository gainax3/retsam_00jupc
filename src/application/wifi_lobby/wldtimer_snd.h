//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wldtimer_snd.h
 *	@brief		世界時計	音
 *	@author		tomoya takahashi
 *	@data		2007.09.28
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WLDTIMER_SND_H__
#define __WLDTIMER_SND_H__

#include "system/snd_tool.h"

//#define WLDTIMER_SND_ZOMEIN			( SEQ_SE_PL_TIMER01 )		// ズームイン
#define WLDTIMER_SND_ZOMEIN				( SEQ_SE_PL_TIMER03 )		// ズームイン
//#define WLDTIMER_SND_ZOMEOUT			( SEQ_SE_PL_TIMER01 )		// ズームアウト
#define WLDTIMER_SND_ZOMEOUT			( SEQ_SE_PL_TIMER03 )		// ズームアウト

#define WLDTIMER_SND_XSELECT			( SEQ_SE_PL_TIMER04 )		// xセレクト

#define WLDTIMER_SND_ENTER				( SEQ_SE_DP_DECIDE )		// エンター

#define WLDTIMER_SND_BALLOON			( SEQ_SE_PL_TOKEI3 )		// 風船が飛んでいる音(ループ音)

#define WLDTIMER_SND_YAMERU				( SEQ_SE_DP_DECIDE )		// やめるをタッチ、Bキャンセル

#endif		// __WLDTIMER_SND_H__

