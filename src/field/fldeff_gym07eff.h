//==============================================================================
/**
 * @file	fldeff_gym07eff.h
 * @brief	格闘ジムエフェクト
 * @author	kagaya
 * @data	05.07.13
 */
//==============================================================================
#ifndef FLDEFF_GYM07EFF_H_FILE
#define FLDEFF_GYM07EFF_H_FILE

#include "eoa.h"
#include "fieldobj.h"
#include "field_effect.h"

//==============================================================================
//	define
//==============================================================================

//==============================================================================
//	typedef
//==============================================================================

//==============================================================================
//	外部参照
//==============================================================================
extern void * FE_Gym07Eff_Init( FE_SYS *fes );
extern void FE_Gym07Eff_Delete( void *work );
extern void FE_Gym07Eff_BoxSandbagMaxSet(
		FE_SYS *fes, int box_max, int bag_max );

extern EOA_PTR FE_Gym07Eff_BoxAdd(
		FIELDSYS_WORK *fsys, int gx, int gz, fx32 oy );
extern void FE_Gym07Eff_BoxBreakAnmSet( EOA_PTR eoa );
extern BOOL FE_Gym07Eff_BoxBreakAnmCheck( EOA_PTR eoa );

extern EOA_PTR FE_Gym07Eff_SandbagAdd(
		FIELDSYS_WORK *fsys, int gx, int gz, fx32 oy );
extern void FE_Gym07Eff_SandbagShakeSet( EOA_PTR eoa, int dir, BOOL big );
extern BOOL FE_Gym07Eff_SandbagAnmEndCheck( EOA_PTR eoa );
extern void FE_Gym07Eff_SandbagAnmSpeedSet( EOA_PTR eoa, fx32 speed );

#endif //FLDEFF_GYM07EFF_H_FILE
