//==============================================================================
/**
 * @file	fldeff_drill.h
 * @brief	地下  ドリル演出
 * @author	k.ohno
 * @data	06.05.05
 */
//==============================================================================
#ifndef FLDEFF_DRILL_H_FILE
#define FLDEFF_DRILL_H_FILE

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
extern void * FE_Drill_Init( FE_SYS *fes );
extern void FE_Drill_Delete( void *work );

extern void FE_FldOBJDrillSet( FIELD_OBJ_PTR fldobj, int gx, int gy, int gz );

#endif //FLDEFF_DRILL_H_FILE
