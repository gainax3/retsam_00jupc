//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		d_taya2.c
 *	@brief		田谷さん　デバックメニュー	常駐部分
 *	@author		tomoya takahashi
 *	@data		2007.04.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#include "common.h"
#include "system/lib_pack.h"
#include "system/bmp_list.h"
#include "system/wipe.h"
#include "fieldmap.h"
#include "fieldsys.h"
#include "system/fontproc.h"
#include "fld_bmp.h"
#include "system/pm_str.h"
#include "system/pm_overlay.h"
#include "field_event.h"
#include "system/palanm.h"
#include "application/pms_input.h"
#include "system/pm_rtc.h"

#include "msgdata/msg.naix"
#include "msgdata/msg_debug_taya.h"

#define __D_TAYA2_H_GLOBAL
#include "d_taya2.h"

//-----------------------------------------------------------------------------
/**
 *					コーディング規約
 *		●関数名
 *				１文字目は大文字それ以降は小文字にする
 *		●変数名
 *				・変数共通
 *						constには c_ を付ける
 *						staticには s_ を付ける
 *						ポインタには p_ を付ける
 *						全て合わさると csp_ となる
 *				・グローバル変数
 *						１文字目は大文字
 *				・関数内変数
 *						小文字と”＿”と数字を使用する 関数の引数もこれと同じ
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------


BOOL GMEVENT_ChangeScheneDebug(GMEVENT_CONTROL * event)
{
	FIELDSYS_WORK* fsys;
	DEBUG_CHANGESCHENE_WORK* wk;

//	if( PaletteFadeCheck() != 0 ){
//		return FALSE;
//	}

	fsys = FieldEvent_GetFieldSysWork(event);
	wk = FieldEvent_GetSpecialWork(event);

	switch( wk->seq ){
	case 0:
		FieldFadeWipeSet( FLD_DISP_BRIGHT_BLACKOUT );
		wk->seq++;
		/* fallthru */
	case 1:
		if( WIPE_SYS_EndCheck() )
		{
			wk->start_func( fsys, wk->gen_wk );
			wk->seq++;
		}
		break;
	case 2:
		if (FieldEvent_Cmd_WaitSubProcEnd(fsys) == FALSE) {
			FieldEvent_Cmd_SetMapProc(fsys);
			wk->seq++;
		}
		break;
	case 3:
		if (FieldEvent_Cmd_WaitMapProcStart(fsys) == FALSE) {
			FieldFadeWipeSet( FLD_DISP_BRIGHT_BLACKIN );
			wk->seq++;
		}
		break;
	case 4:
		if( WIPE_SYS_EndCheck() )
		{
			if( wk->end_func )
			{
				wk->end_func( fsys, wk->gen_wk );
			}
			sys_FreeMemoryEz(wk);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

