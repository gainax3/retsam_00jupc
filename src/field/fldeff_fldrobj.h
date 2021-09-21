//==============================================================================
/**
 * @file	fldeff_fldrobj.h
 * @brief	フィールドOBJ　レンダリングOBJ
 * @author	kagaya
 * @data	05.07.13
 */
//==============================================================================
#ifndef FLDEFF_FLDROBJ_H_FILE
#define FLDEFF_FLDROBJ_H_FILE

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
extern void * FE_FldROBJ_Init( FE_SYS *fes );
extern void FE_FldROBJ_Delete( void *work );

extern void FE_FldROBJ_MdlWorkInit( FE_SYS *fes, u32 max );
extern void FE_FldROBJ_MdlAdd( FE_SYS *fes, u32 code, u32 arc_idx );
extern FRO_MDL * FE_FldROBJ_FROMdlSearch( FE_SYS *fes, u32 code );
extern FRO_OBJ * FE_FldROBJ_FROObjSearch( FE_SYS *fes, u32 code );

extern EOA_PTR FE_FldROBJ_Add( CONST_FIELD_OBJ_PTR fldobj, const VecFx32 *offs );

extern void FE_FieldRenderOBJInit( FE_SYS *fes );

#endif //FLDEFF_FLDROBJ_H_FILE
