//==============================================================================
/**
 * @file	ending_scene1.c
 * @brief	シーン１：メイン
 * @author	matsuda
 * @date	2008.04.10(木)
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
//	定数定義
//==============================================================================
///背景スクロール速度X(fx32)
#define MAP_SCENE1_BG_SPEED			(-0x40)

//==============================================================================
//	構造体定義
//==============================================================================
///フワンテ動作制御構造体
typedef struct{
	s32 start_wait;		///<動作開始時までのウェイト
	fx32 start_x;		///<登場座標X
	fx32 start_y;		///<登場座標Y
	fx32 add_x;			///<X加算値
	fx32 add_y;			///<Y加算値
	fx32 add_theta;		///<SINカーブ角度加算値
	fx32 furihaba;		///<SINカーブふり幅
	f32 add_scale;		///<スケール加算値(float)
	u16 add_scale_num;	///<何フレームスケールを加算したら引き返すか
	u16 padding;
}FUWANTE_MOVE_DATA;

//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static void MapScene1_FuwanteInit(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP1 *sw_map1);
static void MapScene1_FuwanteUpdate(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP1 *sw_map1);
static void MapScene1_BGScroll(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP1 *sw_map1);
static void FuwanteMove(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP1 *sw_map1, CATS_ACT_PTR cap, int no);

//==============================================================================
//	データ
//==============================================================================
///フワンテ動作制御データ
static const FUWANTE_MOVE_DATA FuwanteMoveDataTbl[] = {
	{
		ENDING_COUNTER_SCENE1_FUWANTE_0,						//動作開始時までのウェイト
		-64 * FX32_ONE, 			//登場座標X
		80 * FX32_ONE, 			//登場座標Y
		0x600,					//加算値X
		-0x80,					//加算値Y
		0xe00,					//SINカーブ加算値(fx32)
		0x8000,					//SINカーブふり幅(fx32)
		0.0030,					//アフィン加算値(float)
		60,						//何フレームアフィンを加算したら折り返すか
	},
	{
		ENDING_COUNTER_SCENE1_FUWANTE_1,						//動作開始時までのウェイト
		-64 * FX32_ONE, 			//登場座標X
		190 * FX32_ONE, 			//登場座標Y
		0x780,					//加算値X
		-0xa0,					//加算値Y
		0x1000,					//SINカーブ加算値(fx32)
		0xc000,					//SINカーブふり幅(fx32)
		-0.0040,					//アフィン加算値(float)
		50,						//何フレームアフィンを加算したら折り返すか
	},
	{
		ENDING_COUNTER_SCENE1_FUWANTE_2,						//動作開始時までのウェイト
		-64 * FX32_ONE, 		//登場座標X
		120 * FX32_ONE, 		//登場座標Y
		0x640,					//加算値X
		-0x90,					//加算値Y
		0xc00,					//SINカーブ加算値(fx32)
		0x8000,					//SINカーブふり幅(fx32)
		-0.0045,					//アフィン加算値(float)
		60,						//何フレームアフィンを加算したら折り返すか
	},
};



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
BOOL MapScene1_Main(ENDING_MAIN_WORK *emw, ENDING_SCENE_WORK *sw)
{
	enum{
		SEQ_INIT,
		SEQ_MAIN,
		SEQ_OUT,
	};
	SCENE_WORK_MAP1 *sw_map1 = &sw->sw_map1;
	
	switch(sw->seq){
	case SEQ_INIT:
		MapScene1_FuwanteInit(emw, sw_map1);
		ChangeBrightnessRequest(ENDING_FADE_SYNC_SCENE, BRIGHTNESS_NORMAL, BRIGHTNESS_BLACK,
			ENDING_FADE_PLANE, MASK_DOUBLE_DISPLAY);
		sw->seq++;
		break;
	case SEQ_MAIN:
	#ifdef DEBUG_ENDING_SCENE_SKIP
		if(sys.trg & PAD_BUTTON_Y){
			emw->main_counter = ENDING_COUNTER_SCENE1_END;
		}
	#endif
		if(emw->main_counter >= ENDING_COUNTER_SCENE1_END){
			ChangeBrightnessRequest(ENDING_FADE_SYNC_SCENE, BRIGHTNESS_BLACK, BRIGHTNESS_NORMAL,
				ENDING_FADE_PLANE, MASK_DOUBLE_DISPLAY);
			sw->seq++;
		}
		break;
	case SEQ_OUT:
		if(IsFinishedBrightnessChg(MASK_DOUBLE_DISPLAY) == TRUE){
			return TRUE;
		}
		break;
	}

	EndingTool_HeroAnimeUpdate(emw, emw->cap[ACT_INDEX_SCENE1_HERO]);
	MapScene1_FuwanteUpdate(emw, sw_map1);
	EndintTool_ModelLineScroll(emw, -1);
	MapScene1_BGScroll(emw, sw_map1);
	
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   フワンテ初期設定
 *
 * @param   emw		
 */
//--------------------------------------------------------------
static void MapScene1_FuwanteInit(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP1 *sw_map1)
{
	int i, tblno = 0;
	
	GF_ASSERT(NELEMS(FuwanteMoveDataTbl) == ACT_INDEX_SCENE1_FUWANTE_2-ACT_INDEX_SCENE1_FUWANTE_0+1);
	
	for(i = ACT_INDEX_SCENE1_FUWANTE_0; i <= ACT_INDEX_SCENE1_FUWANTE_2; i++, tblno++){
		CATS_ObjectPosSetCapFx32_SubSurface(emw->cap[i], 
			FuwanteMoveDataTbl[tblno].start_x, 
			FuwanteMoveDataTbl[tblno].start_y, 
			ENDING_SUB_ACTOR_DISTANCE);
		sw_map1->fuwante_work[tblno].x = FuwanteMoveDataTbl[tblno].start_x;
		sw_map1->fuwante_work[tblno].y = FuwanteMoveDataTbl[tblno].start_y;
	}
}

//--------------------------------------------------------------
/**
 * @brief   フワンテ更新処理
 *
 * @param   emw		
 */
//--------------------------------------------------------------
static void MapScene1_FuwanteUpdate(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP1 *sw_map1)
{
	int i, tblno = 0;
	
	for(i = ACT_INDEX_SCENE1_FUWANTE_0; i <= ACT_INDEX_SCENE1_FUWANTE_2; i++, tblno++){
		FuwanteMove(emw, sw_map1, emw->cap[i], tblno);
	}
}

//--------------------------------------------------------------
/**
 * @brief   フワンテ動作処理
 *
 * @param   emw			
 * @param   sw_map1		
 * @param   cap			対象のフワンテアクターへのポインタ
 * @param   no			フワンテ番号
 */
//--------------------------------------------------------------
static void FuwanteMove(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP1 *sw_map1, CATS_ACT_PTR cap, int no)
{
	FUWANTE_WORK *fuwante = &sw_map1->fuwante_work[no];
	const FUWANTE_MOVE_DATA *movedata = &FuwanteMoveDataTbl[no];
	fx32 offset_y;
	
	if(fuwante->start_wait <  movedata->start_wait){
		fuwante->start_wait++;
		return;
	}
	
	//SINカーブ
	fuwante->theta += movedata->add_theta;
	if(fuwante->theta >= (360 << FX32_SHIFT)){
		fuwante->theta -= 360 << FX32_SHIFT;
	}
	offset_y = FX_Mul(Sin360FX(fuwante->theta), movedata->furihaba);

	//座標移動
	fuwante->x += movedata->add_x;
	fuwante->y += movedata->add_y;
	CATS_ObjectPosSetCapFx32_SubSurface(
		cap, fuwante->x, fuwante->y + offset_y, ENDING_SUB_ACTOR_DISTANCE);
	
	//アフィン設定
	if(fuwante->scale_dir == 0){
		CATS_ObjectScaleAddCap(cap, movedata->add_scale, movedata->add_scale);
	}
	else{
		CATS_ObjectScaleAddCap(cap, -movedata->add_scale, -movedata->add_scale);
	}
	fuwante->scale_count++;
	if(fuwante->scale_count >= movedata->add_scale_num){
		fuwante->scale_count = 0;
		fuwante->scale_dir ^= 1;
	}
}

//--------------------------------------------------------------
/**
 * @brief   背景BGスクロール
 *
 * @param   emw			
 * @param   sw_map1		マップシーン0ワークへのポインタ
 */
//--------------------------------------------------------------
static void MapScene1_BGScroll(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP1 *sw_map1)
{
	sw_map1->bg_scr_x += MAP_SCENE1_BG_SPEED;
	GF_BGL_ScrollSet(emw->bgl, FRAME_M_BG, GF_BGL_SCROLL_X_SET, sw_map1->bg_scr_x / FX32_ONE);
	GF_BGL_ScrollSet(emw->bgl, FRAME_S_BG, GF_BGL_SCROLL_X_SET, sw_map1->bg_scr_x / FX32_ONE);
}

