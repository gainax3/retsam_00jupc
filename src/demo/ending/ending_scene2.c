//==============================================================================
/**
 * @file	ending_scene2.c
 * @brief	シーン２：メイン
 * @author	matsuda
 * @date	2008.04.10(木)
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
///背景スクロール速度X(fx32)
#define MAP_SCENE2_BG_SPEED			(0x60)

///EVY加算値(下位8ビット小数)
#define MAP_SCENE2_ADD_EVY			(0x0200)

///パレットアニメ転送位置
#define MAP_SCENE2_PALANM_NO		(2*16)

//==============================================================================
//	構造体定義
//==============================================================================
///キャモメ動作制御構造体
typedef struct{
	s32 start_wait;		///<登場までのウェイト
	fx32 start_furihaba_x;		///<登場時のふり幅X
	fx32 start_furihaba_y;		///<登場時のふり幅Y
	fx32 start_add_theta;		///<登場時の速度
	fx32 start_theta;			///<登場時のtheta初期値
	fx32 start_sub_theta_time;	///<thetaがこの時間を越えたらadd_thetaを減速し始める
	fx32 start_end_theta;		///<thetaがこの値を超えたら停止
	fx32 start_sub_theta;		///<登場時の減速力
	fx32 start_low_theta;		///<登場時の最低速度
	
	fx32 move_furihaba_x;		///<停滞動作のふり幅X
	fx32 move_furihaba_y;		///<停滞動作のふり幅Y
	fx32 move_add_theta_x;		///<停滞動作のtheta速度X
	fx32 move_add_theta_y;		///<停滞動作のtheta速度Y

	fx32 escape_furihaba_x;		///<逃げる時のふり幅X
	fx32 escape_furihaba_y;		///<逃げる時のふり幅Y
	fx32 escape_add_theta;		///<逃げる時の速度
	
	fx32 end_x;			///<到着座標X
	fx32 end_y;			///<到着座標Y
	
	fx32 add_up;				///<毎フレーム少しずつ上に移動させていく
	
	s32 add_rotate;		///<回転加算値
	u16 add_rotate_num;	///<何フレーム回転を加算したら引き返すか
	
	u16 padding;
}KYAMOME_MOVE_DATA;

//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static void MapScene2_KyamomeInit(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP2 *sw_map2);
static void MapScene2_KyamomeUpdate(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP2 *sw_map2);
static void MapScene2_BGScroll(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP2 *sw_map2);
static void MapScene2_BGPalAnm(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP2 *sw_map2);
static void KyamomeMove(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP2 *sw_map2, CATS_ACT_PTR cap, int no);

//==============================================================================
//	データ
//==============================================================================
///キャモメ動作データ
static const KYAMOME_MOVE_DATA KyamomeMoveData[] = {
	{
		90,							//登場までのウェイト
		160 * FX32_ONE,						///<登場時のふり幅X
		32 * FX32_ONE,							///<登場時のふり幅Y
		0x2700,						///<登場時の速度
		0*FX32_ONE,				///<登場時のtheta初期値
		70*FX32_ONE,				///<thetaがこの時間を越えたらadd_thetaを減速し始める
		90*FX32_ONE,				///<thetaがこの値を超えたら停止
		0x200,						///<登場時の減速力
		0x1800,						///<登場時の最低速度
		-24*FX32_ONE,				///<停滞動作のふり幅X
		7*FX32_ONE,				///<停滞動作のふり幅Y
		0x1b00,						///<停滞動作のtheta速度X
		0x7800,						///<停滞動作のtheta速度Y
		160 * FX32_ONE,				///<逃げる時のふり幅X
		60 * FX32_ONE,				///<逃げる時のふり幅Y
		0x3000,						///<逃げる時の速度

		(128-54) * FX32_ONE,		//到着座標X
		(96+16) * FX32_ONE,			//到着座標Y
		0x0080,						///<毎フレーム少しずつ上に移動させていく
		80,				///<回転加算値
		30,				///<何フレーム回転を加算したら引き返すか
	},
	{
		100,							//登場までのウェイト
		-160 * FX32_ONE,						///<登場時のふり幅X
		-48 * FX32_ONE,							///<登場時のふり幅Y
		0x2400,						///<登場時の速度
		0*FX32_ONE,				///<登場時のtheta初期値
		80*FX32_ONE,				///<thetaがこの時間を越えたらadd_thetaを減速し始める
		90*FX32_ONE,				///<thetaがこの値を超えたら停止
		0x40,						///<登場時の減速力
		0x1800,						///<登場時の最低速度
		24*FX32_ONE,				///<停滞動作のふり幅X
		6*FX32_ONE,				///<停滞動作のふり幅Y
		0x1d00,						///<停滞動作のtheta速度X
		0x7000,						///<停滞動作のtheta速度Y
		-160 * FX32_ONE,			///<逃げる時のふり幅X
		-52 * FX32_ONE,				///<逃げる時のふり幅Y
		0x2800,						///<逃げる時の速度

		(128+62) * FX32_ONE,		//到着座標X
		(96+10) * FX32_ONE,			//到着座標Y
		0x0080,						///<毎フレーム少しずつ上に移動させていく
		70,				///<回転加算値
		35,				///<何フレーム回転を加算したら引き返すか
	},
};

//--------------------------------------------------------------
/**
 * @brief   シーン２：メイン
 *
 * @param   emw		エンディングメインワークへのポインタ
 * @param   sw		シーンワークへのポインタ
 *
 * @retval  TRUE:終了。　FALSE:継続中
 */
//--------------------------------------------------------------
BOOL MapScene2_Main(ENDING_MAIN_WORK *emw, ENDING_SCENE_WORK *sw)
{
	enum{
		SEQ_INIT,
		SEQ_MAIN,
		SEQ_OUT,
	};
	SCENE_WORK_MAP2 *sw_map2 = &sw->sw_map2;
	
	switch(sw->seq){
	case SEQ_INIT:
		MapScene2_KyamomeInit(emw, sw_map2);
		ChangeBrightnessRequest(ENDING_FADE_SYNC_SCENE, BRIGHTNESS_NORMAL, BRIGHTNESS_BLACK,
			ENDING_FADE_PLANE, MASK_DOUBLE_DISPLAY);
		sw->seq++;
		break;
	case SEQ_MAIN:
	#ifdef DEBUG_ENDING_SCENE_SKIP
		if(sys.trg & PAD_BUTTON_Y){
			emw->main_counter = ENDING_COUNTER_SCENE2_END;
		}
	#endif
		if(emw->main_counter >= ENDING_COUNTER_SCENE2_END){
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

	EndingTool_HeroAnimeUpdate(emw, emw->cap[ACT_INDEX_SCENE2_HERO]);
	MapScene2_KyamomeUpdate(emw, sw_map2);
	EndintTool_ModelLineScroll(emw, -1);
	MapScene2_BGScroll(emw, sw_map2);
	MapScene2_BGPalAnm(emw, sw_map2);
	
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   キャモメ初期設定
 *
 * @param   emw		
 */
//--------------------------------------------------------------
static void MapScene2_KyamomeInit(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP2 *sw_map2)
{
	int i, tblno = 0;
	
	GF_ASSERT(NELEMS(KyamomeMoveData) == ACT_INDEX_SCENE2_KYAMOME_1-ACT_INDEX_SCENE2_KYAMOME_0+1);
	
	for(i = ACT_INDEX_SCENE2_KYAMOME_0; i <= ACT_INDEX_SCENE2_KYAMOME_1; i++, tblno++){
		sw_map2->kyamome_work[tblno].x = KyamomeMoveData[tblno].end_x - KyamomeMoveData[tblno].start_furihaba_x;
		sw_map2->kyamome_work[tblno].y = KyamomeMoveData[tblno].end_y;
		sw_map2->kyamome_work[tblno].theta = KyamomeMoveData[tblno].start_theta;
		sw_map2->kyamome_work[tblno].add_theta = KyamomeMoveData[tblno].start_add_theta;
		
		sw_map2->kyamome_work[tblno].offset_x 
			= FX_Mul(Sin360FX(KyamomeMoveData[tblno].start_theta), 
			KyamomeMoveData[tblno].start_furihaba_x);
		sw_map2->kyamome_work[tblno].offset_y 
			= FX_Mul(Cos360FX(KyamomeMoveData[tblno].start_theta), 
			KyamomeMoveData[tblno].start_furihaba_y);

		sw_map2->kyamome_work[tblno].wait = KyamomeMoveData[tblno].start_wait;

		CATS_ObjectPosSetCapFx32_SubSurface(emw->cap[i], 
			sw_map2->kyamome_work[tblno].x + sw_map2->kyamome_work[tblno].offset_x, 
			sw_map2->kyamome_work[tblno].y + sw_map2->kyamome_work[tblno].offset_y,
			ENDING_SUB_ACTOR_DISTANCE);
		CATS_ObjectAffineSetCap(emw->cap[i], CLACT_AFFINE_DOUBLE);	//倍角アフィン
	}
}

//--------------------------------------------------------------
/**
 * @brief   キャモメ更新処理
 *
 * @param   emw		
 */
//--------------------------------------------------------------
static void MapScene2_KyamomeUpdate(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP2 *sw_map2)
{
	int i, tblno = 0;
	
	for(i = ACT_INDEX_SCENE2_KYAMOME_0; i <= ACT_INDEX_SCENE2_KYAMOME_1; i++, tblno++){
		KyamomeMove(emw, sw_map2, emw->cap[i], tblno);
	}
}

//--------------------------------------------------------------
/**
 * @brief   キャモメ動作処理
 *
 * @param   emw			
 * @param   sw_map2		
 * @param   cap			対象のフワンテアクターへのポインタ
 * @param   no			フワンテ番号
 */
//--------------------------------------------------------------
static void KyamomeMove(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP2 *sw_map2, CATS_ACT_PTR cap, int no)
{
	KYAMOME_WORK *kyamome = &sw_map2->kyamome_work[no];
	const KYAMOME_MOVE_DATA *movedata = &KyamomeMoveData[no];
	fx32 offset_y = 0, offset_x = 0;
	
	if(kyamome->wait > 0){
		kyamome->wait--;
		return;
	}
	
	switch(kyamome->seq){
	case 0:		//登場
		kyamome->seq++;
		//break;
	case 1:		//登場動作
		if(kyamome->theta > movedata->start_sub_theta_time){	//減速
			kyamome->add_theta -= movedata->start_sub_theta;
			if(kyamome->add_theta < movedata->start_low_theta){
				kyamome->add_theta = movedata->start_low_theta;
			}
		}
		kyamome->theta += kyamome->add_theta;
		if(kyamome->theta >= movedata->start_end_theta){
			kyamome->theta = movedata->start_end_theta;
			kyamome->seq++;
		}
		kyamome->offset_x = FX_Mul(Sin360FX(kyamome->theta), movedata->start_furihaba_x);
		kyamome->offset_y = FX_Mul(Cos360FX(kyamome->theta), movedata->start_furihaba_y);
		break;
	case 2:		//ジグザグ
		kyamome->x += kyamome->offset_x;
		kyamome->y += kyamome->offset_y;
		kyamome->offset_x = 0;
		kyamome->offset_y = 0;
		kyamome->add_theta_x = movedata->move_add_theta_x;
		kyamome->add_theta_y = movedata->move_add_theta_y;
		kyamome->seq++;
		//break;
	case 3:		//ジグザグ動作
		kyamome->theta_x += kyamome->add_theta_x;
		kyamome->theta_y += kyamome->add_theta_y;
		if(kyamome->theta_x >= (360 << FX32_SHIFT)){
			kyamome->theta_x -= 360 << FX32_SHIFT;
		}
		if(kyamome->theta_y >= (360 << FX32_SHIFT)){
			kyamome->theta_y -= 360 << FX32_SHIFT;
		}
		kyamome->offset_x = FX_Mul(Sin360FX(kyamome->theta_x), movedata->move_furihaba_x);
		kyamome->offset_y = FX_Mul(Sin360FX(kyamome->theta_y), movedata->move_furihaba_y);
		if(emw->main_counter >= ENDING_COUNTER_SCENE2_KYAMOME_ESCAPE){
			//逃げる前に今のジグザグ移動を減速させる
			kyamome->add_theta_x -= movedata->move_add_theta_x / 20;
			kyamome->add_theta_y -= movedata->move_add_theta_y / 20;
			if(kyamome->add_theta_x <= movedata->move_add_theta_x / 20){
				kyamome->seq++;
			}
		}
		break;
	case 4:		//逃げる
		kyamome->x = (kyamome->x + kyamome->offset_x) - movedata->escape_furihaba_x;
		kyamome->y = kyamome->y + kyamome->offset_y;
		kyamome->theta = 0;
		kyamome->add_theta = movedata->escape_add_theta;
		
		kyamome->offset_x = FX_Mul(Sin360FX(0), movedata->escape_furihaba_x);
		kyamome->offset_y = FX_Mul(Cos360FX(0), movedata->escape_furihaba_y);

		kyamome->seq++;
		//break;
	case 5:		//逃げる動作
		kyamome->theta += kyamome->add_theta;
		kyamome->offset_x = FX_Mul(Cos360FX(kyamome->theta), movedata->escape_furihaba_x);
		kyamome->offset_y = FX_Mul(Sin360FX(kyamome->theta), movedata->escape_furihaba_y);
		if(kyamome->theta >= (180<<FX32_SHIFT)){
			kyamome->seq++;
		}
		break;
	default:
		break;
	}

	//アフィン設定
	if(kyamome->rotate_dir == 0){
		CATS_ObjectRotationAddCap(cap, movedata->add_rotate);
	}
	else{
		CATS_ObjectRotationAddCap(cap, -movedata->add_rotate);
	}
	kyamome->rotate_count++;
	if(kyamome->rotate_count >= movedata->add_rotate_num){
		kyamome->rotate_count = 0;
		kyamome->rotate_dir ^= 1;
	}
	
	kyamome->y -= movedata->add_up;
	
	CATS_ObjectPosSetCapFx32_SubSurface(
		cap, kyamome->x + kyamome->offset_x, kyamome->y + kyamome->offset_y, 
		ENDING_SUB_ACTOR_DISTANCE);
}

//--------------------------------------------------------------
/**
 * @brief   背景BGスクロール
 *
 * @param   emw			
 * @param   sw_map2		マップシーン0ワークへのポインタ
 */
//--------------------------------------------------------------
static void MapScene2_BGScroll(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP2 *sw_map2)
{
	sw_map2->bg_scr_y += MAP_SCENE2_BG_SPEED;
	GF_BGL_ScrollSet(emw->bgl, FRAME_M_BG, GF_BGL_SCROLL_Y_SET, sw_map2->bg_scr_y / FX32_ONE);
	GF_BGL_ScrollSet(emw->bgl, FRAME_S_BG, GF_BGL_SCROLL_Y_SET, sw_map2->bg_scr_y / FX32_ONE);
}

//--------------------------------------------------------------
/**
 * @brief   背景パレットアニメ
 *
 * @param   emw		
 * @param   sw_map2		
 */
//--------------------------------------------------------------
static void MapScene2_BGPalAnm(ENDING_MAIN_WORK *emw, SCENE_WORK_MAP2 *sw_map2)
{
	int i, s;
	int anm_pos, evy, next_color_pos;
	u16 *trans_buf;
	
	anm_pos = sw_map2->anm_pos;
	sw_map2->evy += MAP_SCENE2_ADD_EVY;
	
	if(sw_map2->evy >= (16 << 8)){
		evy = 16;
		sw_map2->evy = 0;
		if(sw_map2->evy_dir == 0){
			sw_map2->anm_pos++;
			if(sw_map2->anm_pos >= ENDING_SCENE2_PALANM_NUM){
				sw_map2->anm_pos = ENDING_SCENE2_PALANM_NUM - 2;
				sw_map2->evy_dir ^= 1;
			}
		}
		else{
			sw_map2->anm_pos--;
			if(sw_map2->anm_pos < 0){
				sw_map2->anm_pos = 1;
				sw_map2->evy_dir ^= 1;
			}
		}
	}
	else{
		evy = sw_map2->evy >> 8;
	}

	if(sw_map2->evy_dir == 0){
		next_color_pos = anm_pos + 1;
		if(next_color_pos >= ENDING_SCENE2_PALANM_NUM){
			next_color_pos = anm_pos - 1;
		}
	}
	else{
		next_color_pos = anm_pos - 1;
		if(next_color_pos < 0){
			next_color_pos = 1;
		}
	}
	
	trans_buf = PaletteWorkTransWorkGet( emw->pfd, FADE_SUB_BG );
	for(s = 0; s < 16; s++){
		SoftFade(&sw_map2->palanm_buf[anm_pos][s], &trans_buf[MAP_SCENE2_PALANM_NO + s],
			1, evy, sw_map2->palanm_buf[next_color_pos][s]);
	}
}

