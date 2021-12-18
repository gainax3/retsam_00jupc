//============================================================================================
/**
 * @file	gym.h
 * @brief	ジム関連
 * @date	2006.02.02
 *
 */
//============================================================================================
#ifndef __GYM_H__
#define __GYM_H__

#include "common.h"
#include "system/msgdata.h"	

//水ジム
extern void GYM_CheckWaterGymButton(FIELDSYS_WORK *fsys);
extern void GYM_SetupWaterGym(FIELDSYS_WORK *fsys);
extern BOOL GYM_HitCheckWaterGym(	FIELDSYS_WORK *fsys,
							const int inGridX, const int inGridZ,
							const fx32 inHeight, BOOL *outHit	);

//ゴーストジム
extern void GYM_SetupGhostGym(FIELDSYS_WORK *fsys);
extern void GYM_MoveGhostGymLift(FIELDSYS_WORK *fsys);

//鋼ジム
extern void GYM_SetupSteelGym(FIELDSYS_WORK *fsys);
extern void GYM_EndSteelGym(FIELDSYS_WORK *fsys);
extern BOOL GYM_HitCheckSteelGym(	FIELDSYS_WORK *fsys,
									const int inGridX, const int inGridZ,
									const fx32 inHeight, BOOL *outHit	);
extern BOOL GYM_CheckSteelLift(FIELDSYS_WORK *fsys);

//格闘ジム
#if 0 //DP NULL
extern void GYM_SetupCombatGym(FIELDSYS_WORK *fsys);
extern void GYM_EndCombatGym(FIELDSYS_WORK *fsys);
extern BOOL GYM_CheckCombatWall(FIELDSYS_WORK *fsys);
extern BOOL GYM_HitCheckCombatGym(	FIELDSYS_WORK *fsys,
									const int inGridX, const int inGridZ,
									const fx32 inHeight, BOOL *outHit	);
#endif

//電気ジム
extern void GYM_SetupElecGym(FIELDSYS_WORK *fsys);
extern void GYM_EndElecGym(FIELDSYS_WORK *fsys);
extern BOOL GYM_HitCheckElecGym(	FIELDSYS_WORK *fsys,
									const int inGridX, const int inGridZ,
									const fx32 inHeight, BOOL *outHit	);
extern void GYM_RotateElecGymGear(FIELDSYS_WORK *fsys, const u8 inRotate);

//PL 草ジム
extern void GYM_SetupPLGrassGym( FIELDSYS_WORK *fsys );
extern void GYM_EndPLGrassGym( FIELDSYS_WORK *fsys );
extern BOOL GYM_HitCheckPLGrassGym( FIELDSYS_WORK *fsys,
							const int inGridX, const int inGridZ,
							const fx32 inHeight, BOOL *outHit	);
extern BOOL GYM_PLGrass_TimeGainEventSet( FIELDSYS_WORK *fsys,
		GF_BGL_BMPWIN *win, MSGDATA_MANAGER *msgman, STRBUF *buf );
extern BOOL GYM_PLGrass_ExJumpPosCheck(
	FIELDSYS_WORK *fsys, int gx, int gz, int dir );

//PL 格闘ジム
extern void GYM_SetupPLFightGym( FIELDSYS_WORK *fsys );
extern void GYM_EndPLFightGym( FIELDSYS_WORK *fsys );
extern BOOL GYM_HitCheckPLFightGym( FIELDSYS_WORK *fsys,
							const int inGridX, const int inGridZ,
							const fx32 inHeight, BOOL *outHit	);
extern BOOL GYM_PLFightGymEventHitCheck( FIELDSYS_WORK *fsys );

//PL ゴーストジム
#define GYM_PLGHOST_HINT_MAX (8)

extern void GYM_SetupPLGhostGym( FIELDSYS_WORK *fsys );
extern void GYM_EndPLGhostGym( FIELDSYS_WORK *fsys );
extern BOOL GYM_EventCheckPLGhostGymLightTrainer( FIELDSYS_WORK *fsys );
extern BOOL GYM_PlGhostGymTrainerMoveCodeChange(
		FIELDSYS_WORK *fsys, void *fldobj );
BOOL GYM_PlGhostGymHintMissDoorCheck( FIELDSYS_WORK *fsys, int gx, int gz, int *dir );

//----
#ifdef PM_DEBUG

//#define DEBUG_PLGHOSTGYM_CAPTURE //定義でキャプチャー用デバッグ機能有効

#ifdef DEBUG_PLGHOSTGYM_CAPTURE 
extern void DEBUG_PlGhostGym_LightVanish( FIELDSYS_WORK *fsys );
#endif

extern u8 * DEBUG_GYM_PLGhostGym_FogRedGet( FIELDSYS_WORK *fsys );
extern u8 * DEBUG_GYM_PLGhostGym_FogGreenGet( FIELDSYS_WORK *fsys );
extern u8 * DEBUG_GYM_PLGhostGym_FogBlueGet( FIELDSYS_WORK *fsys );
extern u8 * DEBUG_GYM_PLGhostGym_FogAlphaGet( FIELDSYS_WORK *fsys );
extern char DEBUG_GYM_PLGhostGym_FogTblGet( FIELDSYS_WORK *fsys );
extern void DEBUG_GYM_PLGhostGym_FogTblSet( FIELDSYS_WORK *fsys, char param );
extern void DEBUG_GYM_PLGhostGym_FogSet( FIELDSYS_WORK *fsys );
#endif
//----

#endif //__GYM_H__
