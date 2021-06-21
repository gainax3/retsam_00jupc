//==============================================================================
/**
 * @file	footprint_main.h
 * @brief	足跡ボードのメインヘッダ
 * @author	matsuda
 * @date	2008.01.19(土)
 */
//==============================================================================
#ifndef __FOOTPRINT_MAIN_H__
#define __FOOTPRINT_MAIN_H__

#include "application/wifi_lobby/wflby_system_def.h"

//==============================================================================
//	定数定義
//==============================================================================
///ボードの種類
enum{
	FOOTPRINT_BOARD_TYPE_WHITE,		///<白ボード
	FOOTPRINT_BOARD_TYPE_BLACK,		///<黒ボード

	FOOTPRINT_BOARD_TYPE_MAX,
};


//==============================================================================
//	構造体定義
//==============================================================================
typedef struct{
	WFLBY_SYSTEM *wflby_sys;		///<WIFIロビーシステムへのポインタ
	u8 board_type;					///<ボードの種類(FOOTPRINT_BOARD_TYPE_???)
	
	u8 padding[3];
	
#ifdef PM_DEBUG
	SAVEDATA *debug_sv;
#endif
}FOOTPRINT_PARAM;


//==============================================================================
//	外部関数宣言
//==============================================================================
extern PROC_RESULT FootPrintProc_Init( PROC * proc, int * seq );
extern PROC_RESULT FootPrintProc_Main( PROC * proc, int * seq );
extern PROC_RESULT FootPrintProc_End( PROC * proc, int * seq );


#endif	//__FOOTPRINT_MAIN_H__

