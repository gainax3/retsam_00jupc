//==============================================================================
/**
 * @file	ending_cap_scene1.c
 * @brief	キャプチャシーン１：メイン
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

#include "msgdata/msg.naix"

#include "demo/ending.h"
#include "graphic/ending.naix"
#include "ending_common.h"
#include "ending_setup.h"
#include "ending_tool.h"


//==============================================================================
//	定数定義
//==============================================================================
///何番目までキャプチャ画像を表示するか
#define CAP_SCENE1_CAPTURE_NUM		(7)
///キャプチャ画像を切り替えるとき、真っ暗な画面での待ち時間
#define CAP_NEXT_WAIT				(30)
///何フレーム間隔でキャプチャ画像を変更していくか
#define CAP_SCENE1_CHANGE_FRAME		((ENDING_COUNTER_CAP_SCENE1_END - ENDING_COUNTER_SCENE1_END - CAP_NEXT_WAIT * (CAP_SCENE1_CAPTURE_NUM-1)) / CAP_SCENE1_CAPTURE_NUM)


//--------------------------------------------------------------
//	シーン2用
//--------------------------------------------------------------
///何番目までキャプチャ画像を表示するか
#define CAP_SCENE2_CAPTURE_NUM		(10)
///何フレーム間隔でキャプチャ画像を変更していくか
#define CAP_SCENE2_CHANGE_FRAME		((ENDING_COUNTER_CAP_SCENE2_END - ENDING_COUNTER_SCENE2_END - CAP_NEXT_WAIT * (CAP_SCENE2_CAPTURE_NUM - CAP_SCENE1_CAPTURE_NUM-1)) / (CAP_SCENE2_CAPTURE_NUM - CAP_SCENE1_CAPTURE_NUM))



//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   シーン１：メイン
 *
 * @param   emw		エンディングメインワークへのポインタ
 * @param   sw		シーンワークへのポインタ
 *
 * @retval  TRUE:終了。　FALSE:継続中
 */
//--------------------------------------------------------------
BOOL MapCapScene1_Main(ENDING_MAIN_WORK *emw, ENDING_SCENE_WORK *sw)
{
	enum{
		SEQ_INIT,
		SEQ_MAIN,
		SEQ_CHANGE_WAIT,
		SEQ_NEXT_CAP,
		SEQ_OUT,
	};
	SCENE_WORK_CAP1 *sw_cap1 = &sw->sw_cap1;
	
	switch(sw->seq){
	case SEQ_INIT:
		ChangeBrightnessRequest(ENDING_FADE_SYNC_SCENE, BRIGHTNESS_NORMAL, BRIGHTNESS_BLACK,
			ENDING_FADE_PLANE, MASK_DOUBLE_DISPLAY);
		sw->seq++;
		break;
	case SEQ_MAIN:
	#ifdef DEBUG_ENDING_SCENE_SKIP
		if(sys.trg & PAD_BUTTON_Y){
			sw_cap1->change_frame = CAP_SCENE1_CHANGE_FRAME + 1;
			emw->cap_no = CAP_SCENE1_CAPTURE_NUM;
			emw->main_counter = ENDING_COUNTER_CAP_SCENE1_END;
		}
	#endif
		if(sw_cap1->change_frame > CAP_SCENE1_CHANGE_FRAME){
			ChangeBrightnessRequest(ENDING_FADE_SYNC_CAPTURE, BRIGHTNESS_BLACK, BRIGHTNESS_NORMAL,
				ENDING_FADE_PLANE, MASK_DOUBLE_DISPLAY);
			sw_cap1->change_frame = 0;
			sw->seq++;
		}
		break;
	case SEQ_CHANGE_WAIT:
		if(IsFinishedBrightnessChg(MASK_DOUBLE_DISPLAY) == TRUE){
			if(emw->cap_no < CAP_SCENE1_CAPTURE_NUM){
				sw->seq = SEQ_NEXT_CAP;
			}
			else{
				sw->seq = SEQ_OUT;
			}
		}
		break;
	case SEQ_NEXT_CAP:
		sw->wait++;
		if(sw->wait > CAP_NEXT_WAIT){
			sw->wait = 0;
			EndingTool_CaptureGraphicTrans(
				emw, emw->cap_no, FRAME_M_CAPTURE, CAPTURE_TBL_TYPE_PICTURE);
			emw->cap_no++;
			ChangeBrightnessRequest(ENDING_FADE_SYNC_CAPTURE, BRIGHTNESS_NORMAL, BRIGHTNESS_BLACK,
				ENDING_FADE_PLANE, MASK_DOUBLE_DISPLAY);
			sw->seq = SEQ_MAIN;
		}
		break;
	case SEQ_OUT:
		if(emw->main_counter >= ENDING_COUNTER_CAP_SCENE1_END){
			return TRUE;
		}
		break;
	}
	
	sw_cap1->change_frame++;
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   シーン２：メイン
 *
 * @param   emw		エンディングメインワークへのポインタ
 * @param   sw		シーンワークへのポインタ
 *
 * @retval  TRUE:終了。　FALSE:継続中
 *
 * シーン１と違うのは参照するCAP_SCENE1_CHANGE_FRAME, CAP_SCENE1_CAPTURE_NUMだけ
 */
//--------------------------------------------------------------
BOOL MapCapScene2_Main(ENDING_MAIN_WORK *emw, ENDING_SCENE_WORK *sw)
{
	enum{
		SEQ_INIT,
		SEQ_MAIN,
		SEQ_CHANGE_WAIT,
		SEQ_NEXT_CAP,
		SEQ_OUT,
	};
	SCENE_WORK_CAP1 *sw_cap1 = &sw->sw_cap1;
	
	switch(sw->seq){
	case SEQ_INIT:
		ChangeBrightnessRequest(ENDING_FADE_SYNC_SCENE, BRIGHTNESS_NORMAL, BRIGHTNESS_BLACK,
			ENDING_FADE_PLANE, MASK_DOUBLE_DISPLAY);
		sw->seq++;
		break;
	case SEQ_MAIN:
	#ifdef DEBUG_ENDING_SCENE_SKIP
		if(sys.trg & PAD_BUTTON_Y){
			sw_cap1->change_frame = CAP_SCENE2_CHANGE_FRAME + 1;
			emw->cap_no = CAP_SCENE2_CAPTURE_NUM;
			emw->main_counter = ENDING_COUNTER_CAP_SCENE2_END;
		}
	#endif
		if(sw_cap1->change_frame > CAP_SCENE2_CHANGE_FRAME){
			ChangeBrightnessRequest(ENDING_FADE_SYNC_CAPTURE, BRIGHTNESS_BLACK, BRIGHTNESS_NORMAL,
				ENDING_FADE_PLANE, MASK_DOUBLE_DISPLAY);
			sw_cap1->change_frame = 0;
			sw->seq++;
		}
		break;
	case SEQ_CHANGE_WAIT:
		if(IsFinishedBrightnessChg(MASK_DOUBLE_DISPLAY) == TRUE){
			if(emw->cap_no < CAP_SCENE2_CAPTURE_NUM){
				sw->seq = SEQ_NEXT_CAP;
			}
			else{
				sw->seq = SEQ_OUT;
			}
		}
		break;
	case SEQ_NEXT_CAP:
		sw->wait++;
		if(sw->wait > CAP_NEXT_WAIT){
			sw->wait = 0;
			EndingTool_CaptureGraphicTrans(emw, emw->cap_no, FRAME_M_CAPTURE, 
				CAPTURE_TBL_TYPE_PICTURE);
			emw->cap_no++;
			ChangeBrightnessRequest(ENDING_FADE_SYNC_CAPTURE, BRIGHTNESS_NORMAL, BRIGHTNESS_BLACK,
				ENDING_FADE_PLANE, MASK_DOUBLE_DISPLAY);
			sw->seq = SEQ_MAIN;
		}
		break;
	case SEQ_OUT:
		if(emw->main_counter >= ENDING_COUNTER_CAP_SCENE1_END){
			return TRUE;
		}
		break;
	}
	
	sw_cap1->change_frame++;
	return FALSE;
}

