//==============================================================================
/**
 * @file	frontier_actor.h
 * @brief	フロンティア2Dマップで使用するアクターのヘッダ
 * @author	matsuda
 * @date	2007.05.30(水)
 */
//==============================================================================
#ifndef __FRONTIER_ACTOR_H__
#define __FRONTIER_ACTOR_H__


//==============================================================================
//	外部関数宣言
//==============================================================================
extern void FAct_ResourceLoad(CATS_SYS_PTR csp, CATS_RES_PTR crp, ARCHANDLE* hdl, 
	PALETTE_FADE_PTR pfd, u16 act_id);
extern void FAct_ResourceFree(CATS_RES_PTR crp, u16 act_id);
extern CATS_ACT_PTR FAct_ActorSet(CATS_SYS_PTR csp, CATS_RES_PTR crp, u16 act_id);
extern void FAct_ActorDel(CATS_ACT_PTR cap);


#endif	//__FRONTIER_ACTOR_H__
