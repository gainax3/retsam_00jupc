//==============================================================================
/**
 * @file	ending_field.c
 * @brief	エンディングフィールド：メイン
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
#define CAP_FIELD_CAPTURE_NUM		(4)
///キャプチャ画像を切り替えるとき、真っ暗な画面での待ち時間
#define CAP_NEXT_WAIT				(CAP_FIELD_CAPTURE_NUM * 16)	//(30)
///何フレーム間隔でキャプチャ画像を変更していくか
#define CAP_FIELD_CHANGE_FRAME		((ENDING_COUNTER_FIELD_END - ENDING_COUNTER_SCENE3_END - CAP_NEXT_WAIT * (CAP_FIELD_CAPTURE_NUM-1)) / CAP_FIELD_CAPTURE_NUM)


#define ALPHA_FADE_A_TO_B_PLANE1		(GX_BLEND_PLANEMASK_BG2)
#define ALPHA_FADE_A_TO_B_PLANE2		(GX_BLEND_PLANEMASK_BG3|GX_BLEND_PLANEMASK_BD)
#define ALPHA_FADE_B_TO_A_PLANE1		(GX_BLEND_PLANEMASK_BG3)
#define ALPHA_FADE_B_TO_A_PLANE2		(GX_BLEND_PLANEMASK_BG2|GX_BLEND_PLANEMASK_BD)

#define ALPHA_FADE_SP				(1)


//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static void AlphaFadeInit(SCENE_WORK_FIELD1 *sw_field1, u32 plane1);
static BOOL AlphaFadeMain(SCENE_WORK_FIELD1 *sw_field1);


//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   シーンフィールド：メイン
 *
 * @param   emw		エンディングメインワークへのポインタ
 * @param   sw		シーンワークへのポインタ
 *
 * @retval  TRUE:終了。　FALSE:継続中
 */
//--------------------------------------------------------------
BOOL MapFieldScene1_Main(ENDING_MAIN_WORK *emw, ENDING_SCENE_WORK *sw)
{
	enum{
		SEQ_INIT,
		SEQ_MAIN,
		SEQ_CHANGE,
		SEQ_ALPHA,
		SEQ_OUT,
	};
	SCENE_WORK_FIELD1 *sw_field1 = &sw->sw_field1;
	
	switch(sw->seq){
	case SEQ_INIT:
		ChangeBrightnessRequest(ENDING_FADE_SYNC_SCENE, BRIGHTNESS_NORMAL, BRIGHTNESS_BLACK,
			ENDING_FADE_PLANE, MASK_DOUBLE_DISPLAY);
		sw->seq++;
		break;
	case SEQ_MAIN:
		sw_field1->change_frame++;
		if(sw_field1->change_frame > CAP_FIELD_CHANGE_FRAME){
			sw_field1->change_frame = 0;
			sw->seq++;
		}
		break;
	case SEQ_CHANGE:
		if(sw_field1->change_num >= CAP_FIELD_CAPTURE_NUM - 1){
			ChangeBrightnessRequest(ENDING_FADE_SYNC_CAPTURE, BRIGHTNESS_BLACK, BRIGHTNESS_NORMAL,
				ENDING_FADE_PLANE, MASK_DOUBLE_DISPLAY);
			sw->seq = SEQ_OUT;
		}
		else{
			if(sw_field1->change_num & 1){
				EndingTool_CaptureGraphicTrans(
					emw, emw->cap_no, FRAME_FIN_M_CAP_A, CAPTURE_TBL_TYPE_FIELD);
				AlphaFadeInit(sw_field1, ALPHA_FADE_B_TO_A_PLANE1);
			}
			else{
				EndingTool_CaptureGraphicTrans(
					emw, emw->cap_no, FRAME_FIN_M_CAP_B, CAPTURE_TBL_TYPE_FIELD);
				AlphaFadeInit(sw_field1, ALPHA_FADE_A_TO_B_PLANE1);
			}
			emw->cap_no++;
			sw_field1->change_num++;
			sw->seq = SEQ_ALPHA;
		}
		break;
	case SEQ_ALPHA:
		if(AlphaFadeMain(sw_field1) == TRUE){
			sw->seq = SEQ_MAIN;
		}
		break;
	case SEQ_OUT:
		if(IsFinishedBrightnessChg(MASK_DOUBLE_DISPLAY) == TRUE){
			return TRUE;
		}
		break;
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   半透明フェードでキャプチャBG面切り替え：初期設定
 *
 * @param   sw_field1		
 * @param   plane1			
 */
//--------------------------------------------------------------
static void AlphaFadeInit(SCENE_WORK_FIELD1 *sw_field1, u32 plane1)
{
	sw_field1->plane1 = plane1;
	sw_field1->ev1 = 16;//31;
	sw_field1->ev2 = 0;
	if(plane1 == ALPHA_FADE_A_TO_B_PLANE1){
		G2_SetBlendAlpha(ALPHA_FADE_A_TO_B_PLANE1, ALPHA_FADE_A_TO_B_PLANE2, 
			sw_field1->ev1, sw_field1->ev2);
		GF_BGL_VisibleSet(FRAME_FIN_M_CAP_B, VISIBLE_ON);
	}
	else{
		G2_SetBlendAlpha(ALPHA_FADE_B_TO_A_PLANE1, ALPHA_FADE_B_TO_A_PLANE2, 
			sw_field1->ev1, sw_field1->ev2);
		GF_BGL_VisibleSet(FRAME_FIN_M_CAP_A, VISIBLE_ON);
	}
}

//--------------------------------------------------------------
/**
 * @brief   半透明フェードでキャプチャBG面切り替え：メイン
 *
 * @param   sw_field1		
 * 
 * @retval  TRUE:切り替え終了
 */
//--------------------------------------------------------------
static BOOL AlphaFadeMain(SCENE_WORK_FIELD1 *sw_field1)
{
	sw_field1->ev1 -= ALPHA_FADE_SP;
	if(sw_field1->ev1 < 16){
		sw_field1->ev2 += ALPHA_FADE_SP;
	}

	if(sw_field1->ev1 < 0){
		sw_field1->ev1 = 0;
	}
	if(sw_field1->ev2 > 16){//31){
		sw_field1->ev2 = 16;//31;
	}

	G2_ChangeBlendAlpha(sw_field1->ev1, sw_field1->ev2);
	if(sw_field1->ev1 == 0 && sw_field1->ev2 == 16){//31){
		if(sw_field1->plane1 == ALPHA_FADE_A_TO_B_PLANE1){
			GF_BGL_VisibleSet(FRAME_FIN_M_CAP_A, VISIBLE_OFF);
			GF_BGL_PrioritySet(FRAME_FIN_M_CAP_A, FRAME_BGPRI_FIN_M_CAPTURE_BOTTOM);
			GF_BGL_PrioritySet(FRAME_FIN_M_CAP_B, FRAME_BGPRI_FIN_M_CAPTURE_TOP);
		}
		else{
			GF_BGL_VisibleSet(FRAME_FIN_M_CAP_B, VISIBLE_OFF);
			GF_BGL_PrioritySet(FRAME_FIN_M_CAP_A, FRAME_BGPRI_FIN_M_CAPTURE_TOP);
			GF_BGL_PrioritySet(FRAME_FIN_M_CAP_B, FRAME_BGPRI_FIN_M_CAPTURE_BOTTOM);
		}
		return TRUE;
	}
	return FALSE;
}

