//==============================================================================================
/**
 * @file	scratch_obj.h
 * @brief	「スクラッチ」OBJ(カーソル、アイコン)
 * @author	Satoshi Nohara
 * @date	07.12.11
 */
//==============================================================================================
#ifndef _SCRATCH_OBJ_H_
#define _SCRATCH_OBJ_H_

#include "system/clact_tool.h"
#include "scratch_sys.h"


//==============================================================================================
//
//	カーソルOBJワークへの不完全型ポインタ
//
//==============================================================================================
typedef	struct _SCRATCH_OBJ SCRATCH_OBJ;


//==============================================================================================
//
//	定義
//
//==============================================================================================
//カーソル移動タイプ
enum{
	OBJ_H_MODE = 0,
	OBJ_V_MODE = 1,
	OBJ_HV_MODE = 2,
};


//==============================================================================================
//
//	extern宣言
//
//==============================================================================================
extern SCRATCH_OBJ* ScratchObj_Create( SCRATCH_CLACT* scratch_clact, u32 data_no, u32 obj_no, u16 x, u16 y, u8 disp, u32 bg_pri, u32 pri );
extern void* ScratchObj_Delete( SCRATCH_OBJ* wk );
extern void ScratchObj_Priority( SCRATCH_OBJ* wk, u32 Priority );
extern void ScratchObj_Vanish( SCRATCH_OBJ* wk, int flag );
extern void ScratchObj_Pause( SCRATCH_OBJ* wk, int flag );
extern void ScratchObj_SetObjPos( SCRATCH_OBJ* wk, int x, int y );
extern void ScratchObj_GetObjPos( SCRATCH_OBJ* wk, int* x, int* y );
extern void ScratchObj_AnmChg( SCRATCH_OBJ* wk, u32 num );
extern void ScratchObj_IconPalChg( SCRATCH_OBJ* wk, POKEMON_PARAM* pp );
extern void ScratchObj_SetScaleAffine( SCRATCH_OBJ* wk, u32 no );
extern void ScratchObj_SetScaleAffine2( SCRATCH_OBJ* wk, u8 no );
extern void ScratchObj_SetScaleAffineTbl( SCRATCH_OBJ* wk, VecFx32* tbl );
extern void ScratchObj_PaletteNoChg( SCRATCH_OBJ* wk, u32 pltt_num );
extern u32 ScratchObj_PaletteNoGet( SCRATCH_OBJ* wk );
extern void ScratchObj_SetAnmFlag( SCRATCH_OBJ* wk, int flag );
extern void ScratchObj_MosaicSet( SCRATCH_OBJ* wk, BOOL flag );
extern BOOL ScratchObj_AnmActiveCheck( SCRATCH_OBJ* wk );


#endif //_SCRATCH_OBJ_H_


