//==============================================================================
/**
 * @file	frontier_act_id.h
 * @brief	フロンティア2Dマップで使用するアクターのID定義場所
 * @author	matsuda
 * @date	2007.05.30(水)
 */
//==============================================================================
#ifndef __FRONTIER_ACT_ID_H__
#define __FRONTIER_ACT_ID_H__


//==============================================================================
//	※FrontierArticleActorHeadTblと並びを同じにしておくこと！
//		アセンブラで使用するのでenum禁止
//==============================================================================
#define ACTID_TEST_BALL				(0)
#define ACTID_TOWER_DOOR			(ACTID_TEST_BALL + 1)
#define ACTID_ROULETTE_RAMP			(ACTID_TEST_BALL + 2)
#define ACTID_ROULETTE_POKEPANEL	(ACTID_TEST_BALL + 3)


#endif	//__FRONTIER_ACT_ID_H__
