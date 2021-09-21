//==============================================================================
/**
 * @file	field_villaworld.h
 * @brief	フィールド　別荘
 * @author	kagaya
 * @data	05.05.13
 */
//==============================================================================
#ifndef FLD_VILLA_H_FILE
#define FLD_VILLA_H_FILE

#include "common.h"

//==============================================================================
//	define
//==============================================================================

//==============================================================================
//	typedef
//==============================================================================

//==============================================================================
//	extern
//==============================================================================
//extern void FldVilla_Init( FIELDSYS_WORK *fsys ); -> gym_init.c
extern void FldVilla_Setup( FIELDSYS_WORK *fsys );
extern void FldVilla_End( FIELDSYS_WORK *fsys );

extern BOOL FldVilla_HitCheck( FIELDSYS_WORK *fsys,
	const int inGridX, const int inGridZ,
	const fx32 inHeight, BOOL *outHit );

extern BOOL FldVilla_TalkCheck( FIELDSYS_WORK *fsys,
	const int inGridX, const int inGridZ, const int dir );

#endif //FLD_VILLA_H_FILE
