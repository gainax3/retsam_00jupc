//==============================================================================
/**
 * @file	d_matsu.h
 * @brief	松田デバッグ用ファイルのヘッダ
 * @author	matsuda
 * @date	2007.04.18(水)
 */
//==============================================================================
#ifndef __D_MATSU_H__
#define __D_MATSU_H__

//==============================================================================
//	定数定義
//==============================================================================
enum{
	DMATSU_PROC_FLAG_BALLOON,
	DMATSU_PROC_FLAG_GDS,
	DMATSU_PROC_FLAG_GDS_RECORDER,
	DMATSU_PROC_FLAG_FOOTPRINT,
};

///コンテストデバッグ機能呼び出しの有効･無効	0=無効、1=有効
#define DEBUG_MATSU_CONTEST		(0)

//==============================================================================
//	構造体定義
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   松田デバッグ用構造体
 */
//--------------------------------------------------------------
typedef struct{
	FIELDSYS_WORK *fsys;
	BMPLIST_WORK *lw;
	BMP_MENULIST_DATA *listmenu;
	GF_BGL_BMPWIN bmpwin;
	s16 wait;
	u8 main_seq;
	u8 proc_flag;
	u8 proc_debug_mode;
}DMATSU_WORK;

typedef struct{
	int seq;
	int debug_mode;
	u16 rank;
	u16 type;
}EV_ACTING_WORK;

typedef struct{
	CONTEST_SYSTEM *consys;		///<※check　正式な入り口・出口が決まるまでの暫定
	EV_ACTING_WORK *eaw;
	int seq;
	int local_seq;
	int debug_mode;
	u16 rank;
	u16 type;
}EV_SUB_ACTING_WORK;

#endif	//__D_MATSU_H__
