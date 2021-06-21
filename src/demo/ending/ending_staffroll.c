//==============================================================================
/**
 * @file	ending_staffroll.c
 * @brief	スタッフロール
 * @author	matsuda
 * @date	2008.04.14(月)
 */
//==============================================================================
#include "common.h"
#include "gflib\camera.h"
#include "system\procsys.h"
#include "system\msgdata.h"
#include "system\font_arc.h"
#include "system\brightness.h"
#include "system\clact_util.h"
#include "system\render_oam.h"
#include "system\snd_tool.h"
#include "savedata\zukanwork.h"
#include "system/brightness.h"
#include "system/d3dobj.h"

#include "msgdata\msg.naix"

#include "demo\ending.h"
#include "graphic/ending.naix"
#include "ending_common.h"
#include "ending_setup.h"
#include "ending_tool.h"


//==============================================================================
//	データ
//==============================================================================
#include "stafflist.dat"
///スタッフリストの数
#define STAFFLIST_MAX		(NELEMS(StaffListDataTable))


//==============================================================================
//	構造体定義
//==============================================================================
typedef struct{
	u32 height;
	u32 list_pos;
}STAFFROLL_WORK;


void EndingStaffroll_Init(ENDING_MAIN_WORK *emw)
{
	GF_BGL_ScrollSet(emw->bgl, FRAME_M_STAFFROLL, GF_BGL_SCROLL_X_SET, 0);
	GF_BGL_ScrollSet(emw->bgl, FRAME_M_STAFFROLL, GF_BGL_SCROLL_Y_SET, 256);
	GF_BGL_ScrollSet(emw->bgl, FRAME_S_STAFFROLL, GF_BGL_SCROLL_X_SET, 0);
	GF_BGL_ScrollSet(emw->bgl, FRAME_S_STAFFROLL, GF_BGL_SCROLL_Y_SET, 256);
}

///高さ加算値　※下位8ビット小数　※スタッフロールの流れる速度
#define HEIGHT_ADD		(0x0100)
BOOL EndingStaffroll_Main(ENDING_MAIN_WORK *emw, STAFFROLL_WORK *srw)
{
	int scroll_add;
	
	scroll_add = ((srw->height + HEIGHT_ADD) >> 8) - (srw->height >> 8);
	srw->height += HEIGHT_ADD;
	GF_BGL_ScrollReq(emw->bgl, FRAME_M_STAFFROLL, GF_BGL_SCROLL_Y_INC, scroll_add);
	GF_BGL_ScrollReq(emw->bgl, FRAME_S_STAFFROLL, GF_BGL_SCROLL_Y_INC, scroll_add);
	
	if(StaffListDataTable[srw->list_pos].height < (srw->height >> 8)){
		GF_BGL_BmpWinAdd(emw->bgl, &srw->win[srw->win_pos], FRAME_M_STAFFROLL,
			
			GF_BGL_INI * ini, GF_BGL_BMPWIN * win, u8 frmnum,
			u8 posx, u8 posy, u8 sizx, u8 sizy, u8 palnum, u16 chrofs )
		
}

void EndingStaffroll_End(ENDING_MAIN_WORK *emw)
{
}

