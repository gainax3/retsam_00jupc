//==============================================================================
/**
 * @file	fldeff_gym05eff.h
 * @brief	ゴーストジムエフェクト
 * @author	kagaya
 * @data	05.05.13
 */
//==============================================================================
#ifndef FLDEFF_GYM05EFF_H_FILE
#define FLDEFF_GYM05EFF_H_FILE

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
extern void * FE_Gym05Eff_Init( FE_SYS *fes );
extern void FE_Gym05Eff_Delete( void *work );

extern EOA_PTR FE_Gym05Eff_LightAdd(
		FE_SYS *fes, CONST_FIELD_OBJ_PTR fldobj, int range, int tcb_offs );
extern void FE_Gym05Eff_LightRangeSet( EOA_PTR eoa, int range );

extern EOA_PTR FE_Gym05Eff_HintPanelAdd( FE_SYS *fes, int gx, int gz, int no );

#ifdef PM_DEBUG
#include "gym.h"
#ifdef DEBUG_PLGHOSTGYM_CAPTURE
extern void DEBUG_FE_Gym05Eff_LightVanish( EOA_PTR eoa );
#endif
#endif

#endif //FLDEFF_GYM05EFF_H_FILE
