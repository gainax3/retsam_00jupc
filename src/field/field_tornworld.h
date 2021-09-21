//==============================================================================
/**
 * @file	field_tornworld.h
 * @brief	フィールド　やぶれたせかい
 * @author	kagaya
 * @data	05.05.13
 */
//==============================================================================
#ifndef FLD_TORNWORLD_H_FILE
#define FLD_TORNWORLD_H_FILE

#include "common.h"
#include "field_tornworld_def.h"

//==============================================================================
//	define
//==============================================================================
//--------------------------------------------------------------
//	debug
//--------------------------------------------------------------
#ifdef PM_DEBUG
//#define DEBUG_TWORLD_CAPTURE	//定義でキャプチャー用デバッグ機能有効
#endif

//==============================================================================
//	typedef
//==============================================================================
typedef struct _EV_FALLROCK_WORK EV_FALLROCK_WORK;

//==============================================================================
//	extern
//==============================================================================
//extern void FldTornWorld_Init( FIELDSYS_WORK *fsys ); ->gym_init.c
extern void FldTornWorld_Setup( FIELDSYS_WORK *fsys );
extern void FldTornWorld_End( FIELDSYS_WORK *fsys );

extern BOOL FldTornWorld_HitCheck( FIELDSYS_WORK *fsys,
	const int inGridX, const int inGridZ,
	const fx32 inHeight, BOOL *outHit );

extern void FldTornWorld_CameraUpdate( FIELDSYS_WORK *fsys );
extern void FldTornWorld_CameraDefaultSet( FIELDSYS_WORK *fsys );

extern int FldTornWorld_OBJRotateGet( FIELDSYS_WORK *fsys, FIELD_OBJ_PTR fldobj );
extern void FldTornWorld_BlActRotateAdd(
	FIELDSYS_WORK *fsys, BLACT_WORK_PTR blact, int rot );
extern void FldTornWorld_BlActRotateDel(
		FIELDSYS_WORK *fsys, CONST_BLACT_WORK_PTR blact );

extern BOOL FldTornWorld_PlayerMoveEventCheck( FIELDSYS_WORK *fsys, int dir );
extern BOOL FldTornWorld_MoveEventCheck( FIELDSYS_WORK *fsys );
extern BOOL FldTornWorld_StepEventCheck( FIELDSYS_WORK *fsys, int dir );
extern BOOL FldTornWorld_PushEventCheck( FIELDSYS_WORK *fsys, int dir );

extern void FldTornWorld_Draw( FIELDSYS_WORK *fsys );
extern void FldTornWorld_MapUpdate( FIELDSYS_WORK *fsys );

extern void FldTornWorld_GirasSet( FIELDSYS_WORK *fsys, u16 no );
extern void FldTornWorld_GirasDelete( FIELDSYS_WORK *fsys );

extern void FldTornWorld_EvFldOBJAdd( FIELDSYS_WORK *fsys, u16 id );
extern void FldTornWorld_EvFldOBJDelete( FIELDSYS_WORK *fsys, u16 id );

extern BOOL FldTornWorld_FallRockDirCheck( CONST_FIELD_OBJ_PTR fldobj, int dir );
extern BOOL FldTornWorld_FallRockCheck( CONST_FIELD_OBJ_PTR fldobj );
extern EV_FALLROCK_WORK * FldTornWorld_FallRockEventInit(
		FIELDSYS_WORK *fsys, GMEVENT_CONTROL *ev, FIELD_OBJ_PTR fldobj );
extern BOOL FldTornWorld_FallRockEvent( EV_FALLROCK_WORK *work );

extern BOOL FldTornWorld_TornWorldCheck( FIELDSYS_WORK *fsys );
extern BOOL FldTornWorld_WallCheck(
		FIELDSYS_WORK *fsys, int gx, int gy, int gz );
extern BOOL FldTornWorld_SpMapDataInsideCheck(
	FIELDSYS_WORK *fsys, int gx, int gy, int gz );
extern BOOL FldTornWorld_SpMapDataExistCheck(
	FIELDSYS_WORK *fsys, int gx, int gy, int gz );
extern void FldTornWorld_SpMapDataPosExistLoad(
	FIELDSYS_WORK *fsys, int gx, int gy, int gz );
extern BOOL FldTornWorld_SpAttrGet(
	FIELDSYS_WORK *fsys, int gx, int gy, int gz, u32 *attr );
extern void FldTornWorld_ArcDataMapOriginGridPosGet(
			int zone_id, int *x, int *y, int *z );
extern BOOL FldTornWorld_NotJumpAttrCheck(
	FIELDSYS_WORK *fsys, int gx, int gz, int dir );

extern void FldTornWorld_GiratinaPlttFade( FIELDSYS_WORK *fsys );

#ifdef DEBUG_TWORLD_CAPTURE
extern void DEBUG_FldTornWorld_StepVanish( void );
extern void DEBUG_FldTornWorld_CloudStop( void );
extern void DEBUG_FldTornWorld_BGVanish( FIELDSYS_WORK *fsys );
extern CAMERA_ANGLE * DEBUG_FldTornWorld_CameraAngleOffsetGet( FIELDSYS_WORK *fsys );
extern void DEBUG_FldTornWorld_CameraReset( FIELDSYS_WORK *fsys );
#endif

#endif //FLD_TORNWORLD_H_FILE
