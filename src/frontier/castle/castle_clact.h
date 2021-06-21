//==============================================================================================
/**
 * @file	castle_clact.h
 * @brief	「バトルキャッスル」セルアクター
 * @author	Satoshi Nohara
 * @date	07.07.05
 */
//==============================================================================================
#ifndef _CASTLE_CLACT_H_
#define _CASTLE_CLACT_H_

#include "common.h"
#include "poketool/poke_tool.h"
#include "system/clact_tool.h"


//==============================================================================================
//
//	定義
//
//==============================================================================================
//CellActorに処理させるリソースマネージャの種類の数（＝マルチセル・マルチセルアニメは使用しない）
#define CASTLE_RESOURCE_NUM		(4)

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
	CLACT_U_RES_MANAGER_PTR	ResMan[CASTLE_RESOURCE_NUM];		//リソースマネージャ
	CLACT_U_RES_OBJ_PTR 	ResObjTbl[RES_OBJ_MAX][CASTLE_RESOURCE_NUM];//resオブジェtbl(上下画面)
}CASTLE_CLACT;


//==============================================================================================
//
//	extern宣言
//
//==============================================================================================
//extern void		CastleClact_InitCellActor( CASTLE_CLACT* wk );
extern void			CastleClact_InitCellActor( CASTLE_CLACT* wk, POKEPARTY* party, u8 type );
extern CLACT_WORK_PTR CastleClact_SetActor( CASTLE_CLACT* wk, u32 char_no, u32 pltt_no, u32 cell_no, u32 anm_no, u32 pri, int bg_pri, u8 disp );
extern void			CastleClact_DeleteCellObject( CASTLE_CLACT* wk );

//アイテムアイコン
extern void			CastleClact_ItemIconCharChange( CASTLE_CLACT* wk, u16 item );
extern void			CastleClact_ItemIconPlttChange( CASTLE_CLACT* wk, u16 item );


#endif //_CASTLE_CLACT_H_


