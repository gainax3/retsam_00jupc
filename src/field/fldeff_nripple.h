//==============================================================================
/**
 * @file	fldeff_ripple.h
 * @brief	フィールドOBJ沼波紋
 * @author	kagaya
 * @data	05.07.13
 */
//==============================================================================
#ifndef FLDEFF_NRIPPLE_H_FILE
#define FLDEFF_NRIPPLE_H_FILE

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
extern void * FE_NRipple_Init( FE_SYS *fes );
extern void FE_NRipple_Delete( void *work );

extern void FE_FldOBJNRippleSet( FIELD_OBJ_PTR fldobj, int gx, int gy, int gz );

#endif //FLDEFF_NRIPPLE_H_FILE
