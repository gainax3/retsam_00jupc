//==============================================================================================
/**
 * @file	scratch_clact.h
 * @brief	「スクラッチ」セルアクター
 * @author	Satoshi Nohara
 * @date	07.12.11
 */
//==============================================================================================
#ifndef _SCRATCH_CLACT_H_
#define _SCRATCH_CLACT_H_

#include "common.h"
#include "poketool/poke_tool.h"
#include "system/clact_tool.h"


//==============================================================================================
//
//	定義
//
//==============================================================================================
//CellActorに処理させるリソースマネージャの種類の数（＝マルチセル・マルチセルアニメは使用しない）
#define SCRATCH_RESOURCE_NUM		(4)

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
	CLACT_U_RES_MANAGER_PTR	ResMan[SCRATCH_RESOURCE_NUM];		//リソースマネージャ
	//resオブジェtbl(上下画面)
	CLACT_U_RES_OBJ_PTR 	ResObjTbl[SCRATCH_RES_OBJ_MAX][SCRATCH_RESOURCE_NUM];
}SCRATCH_CLACT;


//==============================================================================================
//
//	extern宣言
//
//==============================================================================================
extern void					ScratchClact_InitCellActor( SCRATCH_CLACT* wk );
extern CLACT_WORK_PTR ScratchClact_SetActor( SCRATCH_CLACT* wk, u32 data_no, u32 anm_no, u32 bg_pri, u32 pri, u8 disp );
extern void					ScratchClact_DeleteCellObject( SCRATCH_CLACT* wk );
extern void					ScratchClact_ResButtonChg( SCRATCH_CLACT* wk );
extern void					ScratchClact_ButtonResourcePalChg( SCRATCH_CLACT* wk, int no );


#endif //_SCRATCH_CLACT_H_


