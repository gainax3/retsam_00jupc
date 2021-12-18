//==============================================================================================
/**
 * @file	factory_obj.h
 * @brief	「バトルファクトリー」OBJ(ウィンドウ)
 * @author	Satoshi Nohara
 * @date	2007.10.10
 */
//==============================================================================================
#ifndef _FACTORY_OBJ_H_
#define _FACTORY_OBJ_H_

#include "system/clact_tool.h"
#include "factory_sys.h"


//==============================================================================================
//
//	ボールOBJワークへの不完全型ポインタ
//
//==============================================================================================
typedef	struct _FACTORY_OBJ FACTORY_OBJ;


//==============================================================================================
//
//	定義
//
//==============================================================================================


//==============================================================================================
//
//	extern宣言
//
//==============================================================================================
extern FACTORY_OBJ*	FactoryObj_Create( FACTORY_CLACT* factory_clact, u32 anm_no, int x, int y, u32 heapID );
extern void*		FactoryObj_Delete( FACTORY_OBJ* wk );
extern void			FactoryObj_Vanish( FACTORY_OBJ* wk, int flag );
extern VecFx32 FactoryObj_Move( FACTORY_OBJ* wk, int x, int y );
extern void			FactoryObj_SetPos( FACTORY_OBJ* wk, int x, int y );
extern const VecFx32*	FactoryObj_GetPos( FACTORY_OBJ* wk );
extern BOOL			FactoryObj_Main( FACTORY_OBJ* wk );
extern int			FactoryObj_GetInitX( FACTORY_OBJ* wk );
extern int			FactoryObj_GetInitY( FACTORY_OBJ* wk );
extern BOOL			FactoryObj_AnmActiveCheck( FACTORY_OBJ* wk );
extern void			FactoryObj_AnmChg( FACTORY_OBJ* wk, u32 num );


#endif //_FACTORY_OBJ_H_


