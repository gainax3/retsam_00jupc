//==============================================================================
/**
 * @file	fldeff_lgrass.h
 * @brief	フィールドOBJ草
 * @author	kagaya
 * @data	05.07.13
 */
//==============================================================================
#ifndef FLDEFF_LGRASS_H_FILE
#define FLDEFF_LGRASS_H_FILE

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
extern void * FE_LGrass_Init( FE_SYS *fes );
extern void FE_LGrass_Delete( void *work );

extern void FE_FldOBJLGrass_Add( FIELD_OBJ_PTR fldobj, int shake );

#endif //FLDEFF_LGRASS_H_FILE
