//==============================================================================================
/**
 * @file	factory.h
 * @brief	「バトルファクトリー」ヘッダー
 * @author	Satoshi Nohara
 * @date	2007.03.15
 */
//==============================================================================================
#ifndef _FACTORY_H_
#define _FACTORY_H_

#include "../../src/frontier/factory_def.h"


//==============================================================================================
//
//	定義
//
//==============================================================================================


//==============================================================================================
//
//	ファクトリー画面呼び出し　構造体宣言
//
//==============================================================================================
typedef struct{
	SAVEDATA* sv;

	u8	type; 
	u8	level;
	u8	mode; 
	u8	dummy;

	POKEPARTY* p_m_party;							//味方
	POKEPARTY* p_e_party;							//敵
	u16 ret_work[FACTORY_RET_WORK_MAX];				//戻り値格納ワーク
	void* p_work;									//FACTORY_SCRWORK(bf_scr_wk保存)

	u32 dummy_work;
}FACTORY_CALL_WORK;


//==============================================================================================
//
//	extern宣言
//
//==============================================================================================
//extern const PROC_DATA FactoryProcData;				//プロセス定義データ
extern PROC_RESULT FactoryProc_Init( PROC * proc, int * seq );
extern PROC_RESULT FactoryProc_Main( PROC * proc, int * seq );
extern PROC_RESULT FactoryProc_End( PROC * proc, int * seq );


#endif //_FACTORY_H_


