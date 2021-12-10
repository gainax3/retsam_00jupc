//==============================================================================
/**
 * @file	ending_copyright.c
 * @brief	エンディングコピーライト：メイン
 * @author	matsuda
 * @date	2008.04.12(土)
 */
//==============================================================================
#include "common.h"
#include "gflib/camera.h"
#include "system/procsys.h"
#include "system/msgdata.h"
#include "system/font_arc.h"
#include "system/brightness.h"
#include "system/clact_util.h"
#include "system/render_oam.h"
#include "system/snd_tool.h"
#include "savedata/zukanwork.h"
#include "system/brightness.h"
#include "system/d3dobj.h"
#include "system/wipe.h"
#include "system/wipe_def.h"

#include "msgdata/msg.naix"

#include "demo/ending.h"
#include "graphic/ending.naix"
#include "ending_common.h"
#include "ending_setup.h"
#include "ending_tool.h"



//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   シーンコピーライト：メイン
 *
 * @param   emw		エンディングメインワークへのポインタ
 * @param   sw		シーンワークへのポインタ
 *
 * @retval  TRUE:終了。　FALSE:継続中
 */
//--------------------------------------------------------------
BOOL MapCopyrightScene_Main(ENDING_MAIN_WORK *emw, ENDING_SCENE_WORK *sw)
{
	enum{
		SEQ_INIT,
		SEQ_MAIN,
		SEQ_OUT,
	};
	SCENE_WORK_COPYRIGHT *sw_cp = &sw->sw_cp;
	
	switch(sw->seq){
	case SEQ_INIT:
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
			WIPE_FADE_BLACK, 30, WIPE_DEF_SYNC, HEAPID_ENDING_DEMO );
		sw->seq++;
		break;
	case SEQ_MAIN:
		if(WIPE_SYS_EndCheck() == TRUE){
			if(emw->main_counter > ENDING_COUNTER_COPYRIGHT_END 
					|| (sys.trg & PAD_BUTTON_A) || (sys.trg & PAD_BUTTON_START)){
				WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
					WIPE_FADE_BLACK, 45, WIPE_DEF_SYNC, HEAPID_ENDING_DEMO );
				sw->seq++;
			}
		}
		break;
	case SEQ_OUT:
		if(WIPE_SYS_EndCheck() == TRUE){
			return TRUE;
		}
		break;
	}
	return FALSE;
}

