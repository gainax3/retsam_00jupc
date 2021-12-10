//==============================================================================
/**
 * @file	fldeff_kemuri.h
 * @brief	フィールドOBJ土煙
 * @author	kagaya
 * @data	05.07.13
 */
//==============================================================================
#ifndef FLDEFF_KEMURI_H_FILE
#define FLDEFF_KEMURI_H_FILE

#include "eoa.h"
#include "fieldobj.h"
#include "field_effect.h"

//==============================================================================
//	typedef
//==============================================================================
//--------------------------------------------------------------
///	煙向き
//--------------------------------------------------------------
typedef enum
{
	FE_KEMURI_TYPE_FLAT = 0,	//平面
	FE_KEMURI_TYPE_LEFTG,		//左地面
	FE_KEMURI_TYPE_RIGHTG,		//右地面
	FE_KEMURI_TYPE_ROOFG,		//天井面
	FE_KEMURI_TYPE_MAX,
}FE_KEMURI_TYPE;

//==============================================================================
//	外部参照
//==============================================================================
extern void * FE_Kemuri_Init( FE_SYS *fes );
extern void FE_Kemuri_Delete( void *work );
extern void FE_Kemuri_TornWorldON( FE_SYS *fes );

extern void FE_FldOBJKemuri_Add( FIELD_OBJ_PTR fldobj );
extern void FE_FldOBJKemuriTornWorld_Add( FIELD_OBJ_PTR fldobj, FE_KEMURI_TYPE type );

#endif //FLDEFF_KEMURI_H_FILE
