//==============================================================================
/**
 * @file	balloon_snd_def.h
 * @brief	風船ミニゲームSE定義ファイル
 * @author	matsuda
 * @date	2008.01.08(火)
 */
//==============================================================================
#ifndef __BALLOON_SND_DEF_H__
#define __BALLOON_SND_DEF_H__

#include "system/snd_tool.h"


///ソーナンスを押す音
#define SE_SONANS_PUSH			(SEQ_SE_PL_BALLOON02)

///ポンプに空気が発生した時に鳴らす
#define SE_BALLOON_POMP_AIR		(SEQ_SE_PL_BALLOON03_2)

///ソーナンスから空気が出る音
#define SE_SONANS_AIR			(SEQ_SE_PL_BALLOON03)

///ソーナンスを押すタイミングがグレートな音
//#define SE_GREAT				(SEQ_SE_PL_BALLOON05)
#define SE_GREAT				(SEQ_SE_PL_BALLOON04)

///ソーナンスが大きくなる音
#define SE_SONANS_BIG			(SEQ_SE_PL_BALLOON01)

///風船登場音
#define SE_BALLOON_APPEAR		(SEQ_SE_DP_BOX02)

///風船の破裂音
#define SE_BALLOON_EXPLODED		(SEQ_SE_PL_BALLOON07)

///ブースターに空気がヒットした時の音
#define SE_BALLOON_BOOSTER_HIT	(SEQ_SE_PL_BALLOON05)
//#define SE_BALLOON_BOOSTER_HIT	(SEQ_SE_PL_BOOSTHIT2)

///風船がピンチになった時に鳴らす音
//#define SE_BALLOON_PINCH		(SEQ_SE_PL_ALERT3)
#define SE_BALLOON_PINCH		(SEQ_SE_PL_ALERT4)

#endif	//__BALLOON_SND_DEF_H__


