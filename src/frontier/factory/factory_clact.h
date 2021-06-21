//==============================================================================================
/**
 * @file	factory_clact.h
 * @brief	「バトルファクトリー」セルアクター
 * @author	Satoshi Nohara
 * @date	2007.11.15
 */
//==============================================================================================
#ifndef _FACTORY_CLACT_H_
#define _FACTORY_CLACT_H_

#include "common.h"
#include "system/clact_tool.h"


//==============================================================================================
//
//	定義
//
//==============================================================================================
//CellActorに処理させるリソースマネージャの種類の数（＝マルチセル・マルチセルアニメは使用しない）
#define FACTORY_RESOURCE_NUM		(4)

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
	CLACT_U_RES_MANAGER_PTR	ResMan[FACTORY_RESOURCE_NUM];		//リソースマネージャ
	//resオブジェtbl(上下画面)
	CLACT_U_RES_OBJ_PTR 	ResObjTbl[FACTORY_RES_OBJ_MAX][FACTORY_RESOURCE_NUM];
}FACTORY_CLACT;


//==============================================================================================
//
//	extern宣言
//
//==============================================================================================
extern void					FactoryClact_InitCellActor( FACTORY_CLACT* wk );
extern CLACT_WORK_PTR FactoryClact_SetActor( FACTORY_CLACT* wk, u32 data_no, u32 anm_no, u32 pri, int bg_pri, u8 disp );
extern void					FactoryClact_DeleteCellObject( FACTORY_CLACT* wk );


#endif //_FACTORY_CLACT_H_


