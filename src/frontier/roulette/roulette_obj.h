//==============================================================================================
/**
 * @file	roulette_obj.h
 * @brief	「バトルルーレット」OBJ(カーソル、アイコン)
 * @author	Satoshi Nohara
 * @date	07.09.05
 */
//==============================================================================================
#ifndef _ROULETTE_OBJ_H_
#define _ROULETTE_OBJ_H_

#include "system/clact_tool.h"
#include "roulette_sys.h"


//==============================================================================================
//
//	カーソルOBJワークへの不完全型ポインタ
//
//==============================================================================================
typedef	struct _ROULETTE_OBJ ROULETTE_OBJ;


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

//BGプライオリティ
enum{
	ROULETTE_BG_PRI_LOW = 2,
	ROULETTE_BG_PRI_HIGH = 0,
};


//==============================================================================================
//
//	extern宣言
//
//==============================================================================================
extern ROULETTE_OBJ* RouletteObj_Create( ROULETTE_CLACT* roulette_clact, u32 char_no, u32 pltt_no, u32 cell_no, u32 anm_no, u16 x, u16 y, u32 pri, int bg_pri, u8 disp );
extern void* RouletteObj_Delete( ROULETTE_OBJ* wk );
extern void RouletteObj_Vanish( ROULETTE_OBJ* wk, int flag );
extern void RouletteObj_Pause( ROULETTE_OBJ* wk, int flag );
extern u16 RouletteObj_GetObjX( ROULETTE_OBJ* wk );
extern u16 RouletteObj_GetObjY( ROULETTE_OBJ* wk );
extern void RouletteObj_SetObjPos( ROULETTE_OBJ* wk, u16 x, u16 y );
extern void RouletteObj_AnmChg( ROULETTE_OBJ* wk, u32 num );
extern void RouletteObj_IconPalChg( ROULETTE_OBJ* wk, POKEMON_PARAM* pp );
extern u16 RouletteObj_GetAnmFrame( ROULETTE_OBJ* wk );
extern void RouletteObj_SetInitPosOffset( ROULETTE_OBJ* wk, int offset_x, int offset_y );
extern void RouletteObj_SetScaleAffine( ROULETTE_OBJ* wk, u32 no );
extern void RouletteObj_SetAnmFlag( ROULETTE_OBJ* wk, u8 flag );


#endif //_ROULETTE_OBJ_H_


