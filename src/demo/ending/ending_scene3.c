//==============================================================================
/**
 * @file	ending_scene3.c
 * @brief	シーン３：メイン
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
#define MAP_SCENE3_BG_SPEED			(-0x80)

///EVY加算値(下位8ビット小数)
#define MAP_SCENE3_ADD_EVY			(0x0400)

///パレットアニメ切り替えウェイト
#define ENDING_SCENE3_PALANM_WAIT	0	//((ENDING_COUNTER_SCENE3_END - ENDING_COUNTER_CAP_SCENE2_END) / ENDING_SCENE3_PALANM_NUM)

///スターの半透明加算速度
#define STAR_EV_ADD			(3)
///スターの速度X
#define STAR_ADD_X			(-0x6000)	//fx32
///スターの速度Y
#define STAR_ADD_Y			(0xd000)	//fx32

//==============================================================================
//	構造体定義
//==============================================================================
///スターの動作設定構造体
typedef struct{
	s16 start_x;
	s16 start_y;
}STAR_MOVE_DATA;

///ジバコイル移動パラメータ
typedef struct{
	s16 x;				///<初期位置X
	s16 y;				///<初期位置Y
	fx32 add_x;			///<移動速度X
	fx32 add_y;			///<移動速度Y

	fx32 add_theta;		///<SINカーブ角度加算値
	fx32 furihaba;		///<SINカーブふり幅
	s32 add_rotate;		///<回転加算値
	u16 add_rotate_num;	///<何フレーム回転を加算したら引き返すか
}JIBA_MOVE_DATA;

//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static void MapScene3_JibaInit(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP3 *sw_map3);
static void MapScene3_JibaUpdate(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP3 *sw_map3);
static void MapScene3_JibaMove(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP3 *sw_map3, CATS_ACT_PTR cap, int no);
static void MapScene3_BGScroll(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP3 *sw_map3);
static void MapScene3_BGPalAnm(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP3 *sw_map3);
static void MapScene3_StarInit(ENDING_MAIN_WORK *emw);
static void MapScene3_StarUpdate(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP3 *sw_map3);
static void MapScene3_StarSet(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP3 *sw_map3, int tblno);
static void MapScene3_StarMove(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP3 *sw_map3);

//==============================================================================
//	データ
//==============================================================================
static const JIBA_MOVE_DATA JibaMoveData[] = {
	{
		256 + 48, 			///<初期位置X
		-10, 			///<初期位置Y
		-0xe00, 		///<移動速度X(fx32)
		0x600, 		///<移動速度Y(fx32)
		0x8000,			///<SINカーブ角度加算値
		0xc000,			///<SINカーブふり幅
		150,				///<回転加算値
		20,				///<何フレーム回転を加算したら引き返すか
	},
};


///スター動作データ
static const STAR_MOVE_DATA StarMoveData[] = {
	{
		200,
		0,
	},
	{
		128+16,
		0,
	},
	{
		180,
		0,
	},
};

///街頭の配置間隔
#define LIGHT_ARRANGEMENT_INTERVAL			(8)

///前列のモデル配置データ
ALIGN4 const u16 Scene3MdlSort[] = {
	ENDING_SCENE3_MDL_TREE_A,
	ENDING_SCENE3_MDL_TREE_A,
	ENDING_SCENE3_MDL_TREE_A,
	ENDING_SCENE3_MDL_TREE_A,
	ENDING_SCENE3_MDL_TREE_A,
	ENDING_SCENE3_MDL_TREE_A2,
	ENDING_SCENE3_MDL_TREE_A,
	ENDING_SCENE3_MDL_TREE_A,
	ENDING_SCENE3_MDL_TREE_A2,
	ENDING_SCENE3_MDL_TREE_A,
	ENDING_SCENE3_MDL_TREE_A,
	ENDING_SCENE3_MDL_TREE_A2,
	ENDING_SCENE3_MDL_TREE_A2,
	ENDING_SCENE3_MDL_TREE_A,
	ENDING_SCENE3_MDL_TREE_A,
	ENDING_SCENE3_MDL_TREE_A2,
	ENDING_SCENE3_MDL_TREE_A,
	ENDING_SCENE3_MDL_TREE_A2,
	ENDING_SCENE3_MDL_TREE_A,
	ENDING_SCENE3_MDL_TREE_A2,
	ENDING_SCENE3_MDL_TREE_A2,
	ENDING_SCENE3_MDL_TREE_A2,
	ENDING_SCENE3_MDL_TREE_A2,
	ENDING_SCENE3_MDL_TREE_A,
	ENDING_SCENE3_MDL_TREE_A2,
	ENDING_SCENE3_MDL_TREE_A2,
	ENDING_SCENE3_MDL_TREE_A2,
	ENDING_SCENE3_MDL_TREE_A2,
};


//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   シーン３：メイン
 *
 * @param   emw		エンディングメインワークへのポインタ
 * @param   sw		シーンワークへのポインタ
 *
 * @retval  TRUE:終了。　FALSE:継続中
 */
//--------------------------------------------------------------
BOOL MapScene3_Main(ENDING_MAIN_WORK *emw, ENDING_SCENE_WORK *sw)
{
	enum{
		SEQ_INIT,
		SEQ_INIT_WAIT,
		SEQ_MAIN,
		SEQ_OUT,
	};
	SCENE_WORK_MAP3 *sw_map3 = &sw->sw_map3;
	s32 mdl_index_no;
	
	switch(sw->seq){
	case SEQ_INIT:
		{
			int i;
			for(i = 0; i < ENDING_SCENE_LINE_3DOBJ_MAX; i++){
				MI_CpuClear8(&emw->obj[MODEL_LINE_0][i].render, sizeof(NNSG3dRenderObj));
				if(i % LIGHT_ARRANGEMENT_INTERVAL == 0){
					mdl_index_no = ENDING_SCENE3_MDL_LIGHT;
				}
				else if(i >= NELEMS(Scene3MdlSort)){
					mdl_index_no = ENDING_SCENE3_MDL_TREE_A2;
				}
				else{
					mdl_index_no = Scene3MdlSort[i];
				}
				NNS_G3dRenderObjInit( 
					&emw->obj[MODEL_LINE_0][i].render, emw->mdl[mdl_index_no].pModel );
			}
			sw_map3->mdl_sort_tblno = ENDING_SCENE_LINE_3DOBJ_MAX;
		}
		MapScene3_JibaInit(emw, sw_map3);
		MapScene3_StarInit(emw);
		ChangeBrightnessRequest(ENDING_FADE_SYNC_SCENE, BRIGHTNESS_NORMAL, BRIGHTNESS_BLACK,
			ENDING_FADE_PLANE, MASK_DOUBLE_DISPLAY);
		sw->seq++;
		break;
	case SEQ_INIT_WAIT:
		if(IsFinishedBrightnessChg(MASK_DOUBLE_DISPLAY) == TRUE){
			//街頭のライトのBGとの半透明を有効にする為、PLANE2を設定
			G2_SetBlendAlpha(0, 
				GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2
				| GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_BD | GX_BLEND_PLANEMASK_OBJ, 
				31, 0);
			sw->seq++;
		}
		break;
	case SEQ_MAIN:
	#ifdef DEBUG_ENDING_SCENE_SKIP
		if(sys.trg & PAD_BUTTON_Y){
			emw->main_counter = ENDING_COUNTER_SCENE3_END;
		}
	#endif
		if(emw->main_counter >= ENDING_COUNTER_SCENE3_END){
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

	if(sw_map3->mdl_sort_tblno % LIGHT_ARRANGEMENT_INTERVAL == 0){
		mdl_index_no = ENDING_SCENE3_MDL_LIGHT;
	}
	else if(sw_map3->mdl_sort_tblno >= NELEMS(Scene3MdlSort)){
		mdl_index_no = ENDING_SCENE3_MDL_TREE_A2;
	}
	else{
		mdl_index_no = Scene3MdlSort[sw_map3->mdl_sort_tblno];
	}
	if(EndintTool_ModelLineScroll(emw, mdl_index_no) == TRUE){
		sw_map3->mdl_sort_tblno++;
	}

	EndingTool_HeroAnimeUpdate(emw, emw->cap[ACT_INDEX_SCENE3_HERO]);
	MapScene3_JibaUpdate(emw, sw_map3);
	MapScene3_StarUpdate(emw, sw_map3);
	//MapScene3_BGScroll(emw, sw_map3);
	MapScene3_BGPalAnm(emw, sw_map3);
	
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   ジバコイル初期設定
 *
 * @param   emw		
 */
//--------------------------------------------------------------
static void MapScene3_JibaInit(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP3 *sw_map3)
{
	int i, tblno = 0;
	
	GF_ASSERT(NELEMS(JibaMoveData) == ACT_INDEX_SCENE3_JIBA_0-ACT_INDEX_SCENE3_JIBA_0+1);
	
	for(i = ACT_INDEX_SCENE3_JIBA_0; i <= ACT_INDEX_SCENE3_JIBA_0; i++, tblno++){
		CATS_ObjectAffineSetCap(emw->cap[i], CLACT_AFFINE_DOUBLE);
		CATS_ObjectPosSetCap_SubSurface(emw->cap[i], 
			JibaMoveData[tblno].x, JibaMoveData[tblno].y, ENDING_SUB_ACTOR_DISTANCE);
		sw_map3->jiba_work[tblno].x = JibaMoveData[tblno].x * FX32_ONE;
		sw_map3->jiba_work[tblno].y = JibaMoveData[tblno].y * FX32_ONE;
	}
}

//--------------------------------------------------------------
/**
 * @brief   ジバコイル更新処理
 *
 * @param   emw		
 */
//--------------------------------------------------------------
static void MapScene3_JibaUpdate(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP3 *sw_map3)
{
	int i, tblno = 0;
	
	if(emw->main_counter < ENDING_COUNTER_SCENE3_JIBA){
		return;
	}
	
	for(i = ACT_INDEX_SCENE3_JIBA_0; i <= ACT_INDEX_SCENE3_JIBA_0; i++, tblno++){
		MapScene3_JibaMove(emw, sw_map3, emw->cap[i], tblno);
	}
}

//--------------------------------------------------------------
/**
 * @brief   ジバコイル動作処理
 *
 * @param   emw		
 * @param   sw_map3		
 * @param   cap		
 * @param   no		
 */
//--------------------------------------------------------------
static void MapScene3_JibaMove(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP3 *sw_map3, CATS_ACT_PTR cap, int no)
{
	JIBA_WORK *jiba = &sw_map3->jiba_work[no];
	const JIBA_MOVE_DATA *movedata = &JibaMoveData[no];
	fx32 offset_y;
	
	if(jiba->x < -64*FX32_ONE){
		return;
	}
	
	//SINカーブ
	jiba->theta += movedata->add_theta;
	if(jiba->theta >= (360 << FX32_SHIFT)){
		jiba->theta -= 360 << FX32_SHIFT;
	}
	offset_y = FX_Mul(Sin360FX(jiba->theta), movedata->furihaba);

	//座標移動
	jiba->x += movedata->add_x;
	jiba->y += movedata->add_y;
	CATS_ObjectPosSetCapFx32_SubSurface(
		cap, jiba->x, jiba->y + offset_y, ENDING_SUB_ACTOR_DISTANCE);
	
	//アフィン設定
	if(jiba->rotate_dir == 0){
		CATS_ObjectRotationAddCap(cap, movedata->add_rotate);
	}
	else{
		CATS_ObjectRotationAddCap(cap, -movedata->add_rotate);
	}
	jiba->rotate_count++;
	if(jiba->rotate_count >= movedata->add_rotate_num){
		jiba->rotate_count = 0;
		jiba->rotate_dir ^= 1;
	}
}

//--------------------------------------------------------------
/**
 * @brief   スター初期設定
 *
 * @param   emw		
 */
//--------------------------------------------------------------
static void MapScene3_StarInit(ENDING_MAIN_WORK *emw)
{
	CATS_ObjectObjModeSetCap(emw->cap[ACT_INDEX_SCENE3_STAR_MAIN], GX_OAM_MODE_XLU);	//半透明ON
	CATS_ObjectObjModeSetCap(emw->cap[ACT_INDEX_SCENE3_STAR_SUB], GX_OAM_MODE_XLU);
}

//--------------------------------------------------------------
/**
 * @brief   スター更新処理
 *
 * @param   emw		
 * @param   sw_map3		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
static void MapScene3_StarUpdate(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP3 *sw_map3)
{
	if(emw->main_counter == ENDING_COUNTER_SCENE3_STAR_0){
		MapScene3_StarSet(emw, sw_map3, 0);
	}
	if(emw->main_counter == ENDING_COUNTER_SCENE3_STAR_1){
		MapScene3_StarSet(emw, sw_map3, 1);
	}
	if(emw->main_counter == ENDING_COUNTER_SCENE3_STAR_2){
		MapScene3_StarSet(emw, sw_map3, 2);
	}
	
	MapScene3_StarMove(emw, sw_map3);
}

//--------------------------------------------------------------
/**
 * @brief   スターの動作開始設定
 *
 * @param   emw			
 * @param   sw_map3		
 * @param   tblno		
 */
//--------------------------------------------------------------
static void MapScene3_StarSet(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP3 *sw_map3, int tblno)
{
	GF_ASSERT(sw_map3->star_move == FALSE);		//他のスター動作中は出せない(半透明が空いてない)
	GF_ASSERT(NELEMS(StarMoveData) > tblno);
	
	sw_map3->star_move = TRUE;
	sw_map3->star_ud = 0;

	sw_map3->ev1 = 0;
	sw_map3->ev2 = 31;
	
	CATS_ObjectPosSetCap_SubSurface(emw->cap[ACT_INDEX_SCENE3_STAR_SUB], 
		StarMoveData[tblno].start_x, StarMoveData[tblno].start_y, ENDING_SUB_ACTOR_DISTANCE);
	CATS_ObjectEnableCap(emw->cap[ACT_INDEX_SCENE3_STAR_SUB], CATS_ENABLE_TRUE);	//表示
	CATS_ObjectEnableCap(emw->cap[ACT_INDEX_SCENE3_STAR_MAIN], CATS_ENABLE_FALSE);	//非表示
	
	G2S_SetBlendAlpha(0, 
		GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2
		| GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_BD | GX_BLEND_PLANEMASK_OBJ, 
		sw_map3->ev1, sw_map3->ev2);
	G2_SetBlendAlpha(0, 
		GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2
		| GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_BD | GX_BLEND_PLANEMASK_OBJ, 
		sw_map3->ev2, sw_map3->ev1);
}

//--------------------------------------------------------------
/**
 * @brief   スター動作
 *
 * @param   emw		
 * @param   sw_map3		
 */
//--------------------------------------------------------------
static void MapScene3_StarMove(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP3 *sw_map3)
{
	s16 x, y;
	
	if(sw_map3->star_move == FALSE){
		return;
	}
	
	if(sw_map3->star_ud == 0){	//上画面
		sw_map3->ev1 += STAR_EV_ADD;
		sw_map3->ev2 -= STAR_EV_ADD;
		if(sw_map3->ev1 > 31){
			sw_map3->ev1 = 31;
		}
		if(sw_map3->ev2 < 0){
			sw_map3->ev2 = 0;
		}
		G2S_ChangeBlendAlpha(sw_map3->ev1, sw_map3->ev2);
		
		CATS_ObjectPosMoveCapFx32(emw->cap[ACT_INDEX_SCENE3_STAR_SUB], STAR_ADD_X, STAR_ADD_Y);
		CATS_ObjectPosGetCap_SubSurface(emw->cap[ACT_INDEX_SCENE3_STAR_SUB], 
			&x, &y, ENDING_SUB_ACTOR_DISTANCE);
		if(y > 192+32){
			CATS_ObjectEnableCap(emw->cap[ACT_INDEX_SCENE3_STAR_SUB], CATS_ENABLE_FALSE);
			CATS_ObjectEnableCap(emw->cap[ACT_INDEX_SCENE3_STAR_MAIN], CATS_ENABLE_TRUE);
			CATS_ObjectPosSetCap_SubSurface(emw->cap[ACT_INDEX_SCENE3_STAR_MAIN], 
				x, -32, ENDING_SUB_ACTOR_DISTANCE);
			sw_map3->star_ud = 1;
		}
	}
	else{	//下画面
		sw_map3->ev2 += STAR_EV_ADD;
		sw_map3->ev1 -= STAR_EV_ADD;
		if(sw_map3->ev2 > 31){
			sw_map3->ev2 = 31;
		}
		if(sw_map3->ev1 < 0){
			sw_map3->ev1 = 0;
		}
		G2_ChangeBlendAlpha(sw_map3->ev1, sw_map3->ev2);
		
		CATS_ObjectPosMoveCapFx32(emw->cap[ACT_INDEX_SCENE3_STAR_MAIN], STAR_ADD_X, STAR_ADD_Y);
		CATS_ObjectPosGetCap_SubSurface(emw->cap[ACT_INDEX_SCENE3_STAR_MAIN], 
			&x, &y, ENDING_SUB_ACTOR_DISTANCE);
		if(y > 192+32){
			CATS_ObjectEnableCap(emw->cap[ACT_INDEX_SCENE3_STAR_MAIN], CATS_ENABLE_FALSE);
			sw_map3->star_ud = 0;
			sw_map3->star_move = FALSE;
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   背景BGスクロール
 *
 * @param   emw			
 * @param   sw_map3		マップシーン0ワークへのポインタ
 */
//--------------------------------------------------------------
static void MapScene3_BGScroll(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP3 *sw_map3)
{
	sw_map3->bg_scr_y += MAP_SCENE3_BG_SPEED;
	GF_BGL_ScrollSet(emw->bgl, FRAME_M_BG, GF_BGL_SCROLL_Y_SET, sw_map3->bg_scr_y / FX32_ONE);
	GF_BGL_ScrollSet(emw->bgl, FRAME_S_BG, GF_BGL_SCROLL_Y_SET, sw_map3->bg_scr_y / FX32_ONE);
}

//--------------------------------------------------------------
/**
 * @brief   背景パレットアニメ
 *
 * @param   emw		
 * @param   sw_map3		
 */
//--------------------------------------------------------------
static void MapScene3_BGPalAnm(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP3 *sw_map3)
{
	int i, s;
	int anm_pos, evy, next_color_pos;
	u16 *trans_buf_m, *trans_buf_s;
	
	switch(sw_map3->anm_seq){
	case 0:
		if(emw->main_counter < ENDING_COUNTER_SCENE3_PALANM_START){
			return;
		}
		if(sw_map3->anm_pos >= ENDING_SCENE3_PALANM_NUM){
			return;
		}
		
		if(sw_map3->anm_frame > ENDING_SCENE3_PALANM_WAIT * (sw_map3->anm_pos + 1)){
			sw_map3->anm_seq++;
		}
		break;
	case 1:
		anm_pos = sw_map3->anm_pos;
		sw_map3->evy += MAP_SCENE3_ADD_EVY;
		
		if(sw_map3->evy >= (16 << 8)){
			evy = 16;
			sw_map3->evy = 0;
			sw_map3->anm_pos++;
			sw_map3->anm_seq = 0;
		}
		else{
			evy = sw_map3->evy >> 8;
		}

		next_color_pos = anm_pos + 1;
		if(next_color_pos >= ENDING_SCENE3_PALANM_NUM){
			next_color_pos = ENDING_SCENE3_PALANM_NUM - 1;//anm_pos - 1;
		}
		
		trans_buf_m = PaletteWorkTransWorkGet( emw->pfd, FADE_MAIN_BG );
		trans_buf_s = PaletteWorkTransWorkGet( emw->pfd, FADE_SUB_BG );
		for(s = 0; s < 16; s++){
			SoftFade(&sw_map3->palanm_buf[anm_pos][s], 
				&trans_buf_m[s + 16*ENDING_SCENE3_PALANM_PAL_POS],
				1, evy, sw_map3->palanm_buf[next_color_pos][s]);
			SoftFade(&sw_map3->palanm_buf[anm_pos][s], 
				&trans_buf_s[s + 16*ENDING_SCENE3_PALANM_PAL_POS],
				1, evy, sw_map3->palanm_buf[next_color_pos][s]);
		}
		break;
	}
	
	sw_map3->anm_frame++;
}

