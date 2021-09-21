//==============================================================================================
/**
 * @file	factory_csr.h
 * @brief	「バトルファクトリー」カーソルOBJ
 * @author	Satoshi Nohara
 * @date	2007.03.20
 */
//==============================================================================================
#ifndef _FACTORY_CSR_H_
#define _FACTORY_CSR_H_

#include "system/clact_tool.h"
#include "factory_sys.h"


//==============================================================================================
//
//	カーソルOBJワークへの不完全型ポインタ
//
//==============================================================================================
typedef	struct _FACTORY_CSR FACTORY_CSR;


//==============================================================================================
//
//	定義
//
//==============================================================================================
//カーソル移動タイプ
enum{
	CSR_H_MODE = 0,
	CSR_V_MODE = 1,
	CSR_HV_MODE = 2,
};


//==============================================================================================
//
//	extern宣言
//
//==============================================================================================
extern FACTORY_CSR* FactoryCsr_Create( FACTORY_CLACT* factory_clact, u8 sel_max, u8 sel_h_max, u8 mode, u8 csr_pos, const FACTORY_POS* pos_tbl, const u8* anm_tbl );
extern void* FactoryCsr_Delete( FACTORY_CSR* wk );
extern void FactoryCsr_Vanish( FACTORY_CSR* wk, int flag );
extern void FactoryCsr_Move( FACTORY_CSR* wk );
extern u8 FactoryCsr_GetCsrPos( FACTORY_CSR* wk );
extern BOOL FactoryCsr_Main( FACTORY_CSR* wk );
extern void FactoryCsr_Pause( FACTORY_CSR* wk, int flag );
extern void FactoryCsr_SetCsrPos( FACTORY_CSR* wk, u8 csr_pos );


#endif //_FACTORY_CSR_H_


