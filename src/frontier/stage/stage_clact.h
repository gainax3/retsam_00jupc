//==============================================================================================
/**
 * @file	stage_clact.h
 * @brief	「バトルステージ」セルアクター
 * @author	Satoshi Nohara
 * @date	07.06.08
 */
//==============================================================================================
#ifndef _STAGE_CLACT_H_
#define _STAGE_CLACT_H_

#include "common.h"
#include "poketool/poke_tool.h"
#include "system/clact_tool.h"


//==============================================================================================
//
//	定義
//
//==============================================================================================
//CellActorに処理させるリソースマネージャの種類の数（＝マルチセル・マルチセルアニメは使用しない）
#define STAGE_RESOURCE_NUM		(4)

enum{
	DISP_MAIN	= 0,		//上画面
	DISP_SUB	= 1,		//下画面
};


//==============================================================================================
//
//	構造体宣言
//
//==============================================================================================
typedef struct{
	CLACT_SET_PTR 			ClactSet;							//セルアクターセット
	CLACT_U_EASYRENDER_DATA	RendData;							//簡易レンダーデータ
	CLACT_U_RES_MANAGER_PTR	ResMan[STAGE_RESOURCE_NUM];		//リソースマネージャ
	//resオブジェtbl(上下画面)
	CLACT_U_RES_OBJ_PTR 	ResObjTbl[STAGE_RES_OBJ_MAX][STAGE_RESOURCE_NUM];
}STAGE_CLACT;


//==============================================================================================
//
//	extern宣言
//
//==============================================================================================
//extern void					StageClact_InitCellActor( STAGE_CLACT* wk );
extern void					StageClact_InitCellActor( STAGE_CLACT* wk, POKEMON_PARAM* pp );
extern CLACT_WORK_PTR StageClact_SetActor( STAGE_CLACT* wk, u32 data_no, u32 anm_no, u32 pri, u8 disp );
extern void					StageClact_DeleteCellObject( STAGE_CLACT* wk );


#endif //_STAGE_CLACT_H_


