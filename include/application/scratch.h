//==============================================================================================
/**
 * @file	scratch.h
 * @brief	「スクラッチ」呼び出し
 * @author	Satoshi Nohara
 * @date	2007.12.11
 */
//==============================================================================================
#ifndef _SCRATCH_H_
#define _SCRATCH_H_

#include "system/window.h"
#include "../../src/application/scratch/scratch_sys.h"


//==============================================================================================
//
//	定義
//
//==============================================================================================


//==============================================================================================
//
//	構造体
//
//==============================================================================================
typedef struct{
	SAVEDATA* sv;
	u8	type;							//シングル、ダブル、マルチ、WIFI
	u8	fr_no;							//施設ナンバー
	u16 dummy2;

	u16 item_no[SCRATCH_CARD_NUM];		//アイテムナンバー
	u16 item_num[SCRATCH_CARD_NUM];		//アイテム数

	u16 ret_work;						//戻り値格納ワーク
	u16	dummy3;
}SCRATCH_CALL_WORK;


//==============================================================================================
//
//	extern宣言
//
//==============================================================================================
//extern const PROC_DATA FrontierProcData;				//プロセス定義データ
extern PROC_RESULT ScratchProc_Init( PROC * proc, int * seq );
extern PROC_RESULT ScratchProc_Main( PROC * proc, int * seq );
extern PROC_RESULT ScratchProc_End( PROC * proc, int * seq );


#endif //_SCRATCH_H_


