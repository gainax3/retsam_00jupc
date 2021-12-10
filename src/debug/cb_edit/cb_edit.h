
#ifndef __CB_EDIT_H__
#define __CB_EDIT_H__

#include "include/application/cb_sys.h"


extern const PROC_DATA CBE_ProcData;

#define EDIT_CORE_MAX	( 128 )

#define EDIT_BANK		( CB_EDIT_SAVE )

#define EDIT_BTN_NUM	( 8 + 2 + 4 )
#define PANEL_BTN_NUM	( 9 + 3 )

#define EDIT_BTN_MAX	( EDIT_BTN_NUM + 8 )
#define PANEL_BTN_MAX	( PANEL_BTN_NUM + 0 )

typedef struct {
	
	CB_MAIN_WORK*	mw;	
	CB_PROC_WORK*	p_wk;

	int seq;
	int sub_seq;
	
	
	BUTTON_MAN*		btn_edit;
	BUTTON_MAN*		btn_panel;
	BUTTON_MAN*		btn_yes_no;
	BUTTON_MAN*		btn_main;
	
	int				save_or_load;
	
	RECT_HIT_TBL	hit_tbl_edit[ EDIT_BTN_MAX ];
	RECT_HIT_TBL	hit_tbl_panel[ PANEL_BTN_MAX ];
	
	///< 数字用OBJ
	CATS_ACT_PTR	cap[ 3 ];


	int				edit_no;
	int				edit_old_no;
	int				edit_keta;
	///< 保存されるデータ
 	CB_CORE			edit_core[ EDIT_CORE_MAX ];
	
} CBE_WORK;

#define CBE_SAVE_SIZE	( sizeof( CB_CORE ) * EDIT_CORE_MAX )

enum {
	
	eCBE_SEAL_0 = 0,
	eCBE_SEAL_1,
	eCBE_SEAL_2,
	eCBE_SEAL_3,
	eCBE_SEAL_4,
	eCBE_SEAL_5,
	eCBE_SEAL_6,
	eCBE_SEAL_7,
	
	eCBE_UP,
	eCBE_DOWN,
	eCBE_PANEL,
	eCBE_SHOW,
	eCBE_SAVE,
	eCBE_LOAD,
	
	eCBE_HIT_0,
	eCBE_HIT_1,
	eCBE_HIT_2,
	eCBE_HIT_3,
	eCBE_HIT_4,
	eCBE_HIT_5,
	eCBE_HIT_6,
	eCBE_HIT_7,
};

enum {
	
	eCBE_P_1 = 0,
	eCBE_P_2,
	eCBE_P_3,
	eCBE_P_4,
	eCBE_P_5,
	eCBE_P_6,
	eCBE_P_7,
	eCBE_P_8,
	eCBE_P_9,
	eCBE_P_0,
	eCBE_P_C,
	eCBE_P_E,
};

#endif