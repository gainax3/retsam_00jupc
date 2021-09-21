//=============================================================================
/**
 * @file	opening_demo_obj.h
 * @brief	ÉIÅ[ÉvÉjÉìÉOÉfÉÇÇnÇaÇi
 * @author	Nozomu Saito
 * @date    2006.06.03
 */
//=============================================================================
#ifndef __OPENING_DEMO_OBJ_H__
#define __OPENING_DEMO_OBJ_H__

#include "system/clact_tool.h"

typedef enum {
	ACT_WORK_HERO,
	ACT_WORK_HERO_SUB,
	ACT_WORK_HEROINE,
	ACT_WORK_RIVAL,
	ACT_WORK_BIRD_1,
	ACT_WORK_BIRD_2,
	ACT_WORK_BIRD_3,
	ACT_WORK_BIRD_4,
	ACT_WORK_BIRD_5,
	ACT_WORK_BIRD_6,
	ACT_WORK_HAKASE,
	ACT_WORK_BALL_MAIN,
	ACT_WORK_BALL_SUB,
	ACT_WORK_STREAM_MAIN_0,
	ACT_WORK_STREAM_MAIN_1,
	ACT_WORK_STREAM_SUB_0,
	ACT_WORK_STREAM_SUB_1,

	ACT_WORK_MAX,
}ACT_WORK_NO;

typedef enum{
	CHG_MODE_BLACK,
	CHG_MODE_NORMAL,
}COL_CHG_MODE;

typedef struct OP_DEMO_OBJ_WORK_tag * OPD_OBJ_PTR;

extern const int OPD_Obj_GetWorkSize( void );
extern void OPD_Obj_InitCellActor( OPD_OBJ_PTR wk );
extern void OPD_Obj_EndCellActor( OPD_OBJ_PTR wk );
extern void OPD_Obj_SetActor( OPD_OBJ_PTR wk );
extern void OPD_Obj_SetKiraActor( OPD_OBJ_PTR wk,
								  OPD_KIRA_DATLST_PTR pKiraDataListPtr );
extern void OPD_Obj_KiraActorDrawOff( OPD_OBJ_PTR wk, OPD_KIRA_DATLST_PTR pKiraDataListPtr );
extern void OPD_Obj_DrawActor( OPD_OBJ_PTR wk );
extern void OPD_Obj_DispOnOff( OPD_OBJ_PTR wk, const u8 inActWorkNo, const u8 inFlg );
extern void OPD_Obj_ChangePos( OPD_OBJ_PTR wk, const u8 inActWorkNo, const int inX, const int inY );
extern void OPD_Obj_ChangeHeroinePos( OPD_OBJ_PTR wk );
extern void OPD_Obj_ChangeHeroHeroineAnime( OPD_OBJ_PTR wk, const u8 inNo );
extern void OPD_Obj_OnOffHeroHeroineAnime( OPD_OBJ_PTR wk, const u8 inAnime );
extern void OPD_Obj_HeroHeroineMove(OPD_OBJ_PTR wk);
extern BOOL OPD_Obj_SlideInRival(OPD_OBJ_PTR wk, int inCounter);
extern BOOL OPD_Obj_SlideInHakase(OPD_OBJ_PTR wk, int inCounter);
extern void OPD_Obj_StreamScroll(OPD_OBJ_PTR wk, fx32 speed);
extern void OPD_Obj_StreamDispReset(OPD_OBJ_PTR wk, int final_in);
extern void OPD_Obj_BallRotate(OPD_OBJ_PTR wk, int inCounter);
extern void OPD_Obj_ChangeCollor(OPD_OBJ_PTR wk, const int inActWorkNo, const u8 inColChgMode);
extern void  OPD_Obj_SlideInBird(OPD_OBJ_PTR wk, const int inCounter);
#endif	//__OPENING_DEMO_OBJ_H__
