//==============================================================================
/**
 * @file	ending_tool.c
 * @brief	エンディング：ツール類
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
#define MODEL_LINE_START_X		(0)
#define MODEL_LINE_START_Y		(-64 * FX32_ONE)
#define MODEL_LINE_START_Z		(-50 * FX32_ONE)

#define MODEL_LINE_END_X		(128 * FX32_ONE)
#define MODEL_LINE_END_Y		(-32 * FX32_ONE)
#define MODEL_LINE_END_Z		(64 * FX32_ONE)

///STARTとENDの間にいくつモデルを置くか
#define MODEL_LINE_MDL_NUM		(10)	//ENDING_SCENE_LINE_3DOBJ_MAX-1 より小さい必要がある

//--------------------------------------------------------------
//	主人公アニメ
//--------------------------------------------------------------
///主人公が瞬きで目を閉じている時間
#define HERO_MABATAKI_CLOSE_TIME		(1)
///主人公が瞬きで目を閉じている時間
#define HERO_MABATAKI_OPEN_TIME			(40)	//(90)
///瞬きアニメの為の、キャラクタ転送アニメ位置(男)
#define HERO_MABATAKI_CHAR_POS_0		(0x1d)
#define HERO_MABATAKI_CHAR_POS_1		(0x1e)
#define HERO_MABATAKI_CHAR_POS_2		(0x25)
#define HERO_MABATAKI_CHAR_POS_3		(0x26)
///瞬きアニメの為の、キャラクタ転送アニメ位置(女)
#define HEROINE_MABATAKI_CHAR_POS_0		(0x45)
#define HEROINE_MABATAKI_CHAR_POS_1		(0x46)
#define HEROINE_MABATAKI_CHAR_POS_2		(0x4d)
#define HEROINE_MABATAKI_CHAR_POS_3		(0x4e)
///主人公の自転車こぎアニメを何回繰り返すと、漕ぎ無しアニメに移行させるか
#define HERO_RUN_LOOP_COUNT				(16)	//この回数を超えるとランダムで漕ぎ無しに移行


//==============================================================================
//	構造体定義
//==============================================================================
///キャプチャーグラフィックID参照構造体
typedef struct{
	u16 ncg_id;
	u16 nsc_id;
	u16 pal_id;
	s16 scr_x;
	s16 scr_y;
	u16 padding[3];
}CAPTURE_GRA_TBL;

//==============================================================================
//	データ
//==============================================================================
///配置物の配置データ
static const MDL_LINE_ARRANGE_DATA MdlArrangeData[][MODEL_LINE_MAX] = {
	{//マップ1
		{//MODEL_LINE_0
			14, 940,
			415744,			116736,
			-217088,		178176,
			28672,			290816,
		},
		{//MODEL_LINE_1
			14, 1560,
			634880,			116736,
			-217088,		178176,
			28672,			290816,
		},
	},
	{//マップ2
		{//MODEL_LINE_0
			14, 940 - 500,
			415744,			116736,
			-217088,		178176,
			28672,			290816,
		},
		{//MODEL_LINE_1
			0, 0,
			0,				0,
			0,				0,
			0,				0,
		},
	},
	{//マップ3
		{//MODEL_LINE_0
			ENDING_SCENE_LINE_3DOBJ_MAX, 940,
			415744,			116736,
			-217088,		178176,
			28672,			290816,
		},
		{//MODEL_LINE_1
			14, 1560,
			634880,			116736,
			-217088,		178176,
			28672,			290816,
		},
	},
};

///キャプチャーグラフィックID管理テーブル(男用)
static const CAPTURE_GRA_TBL CaptureGraphicIDTbl_Male[] = {
	{
		NARC_ending_ed_scene01m_NCGR,
		NARC_ending_ed_scene_bg_NSCR,
		NARC_ending_ed_scene01m_NCLR,
		0, 0,
	},
	{
		NARC_ending_ed_scene02m_NCGR,
		NARC_ending_ed_scene_bg_NSCR,
		NARC_ending_ed_scene02m_NCLR,
		-32, -48,
	},
	{
		NARC_ending_ed_scene03m_NCGR,
		NARC_ending_ed_scene_bg_NSCR,
		NARC_ending_ed_scene03m_NCLR,
		-16, -30,
	},
	{
		NARC_ending_ed_scene04m_NCGR,
		NARC_ending_ed_scene_bg_NSCR,
		NARC_ending_ed_scene04m_NCLR,
		-42, -2,
	},
	{
		NARC_ending_ed_scene05m_NCGR,
		NARC_ending_ed_scene_bg_NSCR,
		NARC_ending_ed_scene05m_NCLR,
		-8, -20,
	},
	{
		NARC_ending_ed_scene06m_NCGR,
		NARC_ending_ed_scene_bg_NSCR,
		NARC_ending_ed_scene06m_NCLR,
		-50, -16,
	},
	{
		NARC_ending_ed_scene07m_NCGR,
		NARC_ending_ed_scene_bg_NSCR,
		NARC_ending_ed_scene07m_NCLR,
		-40, -34,
	},
	{
		NARC_ending_ed_scene08m_NCGR,
		NARC_ending_ed_scene_bg_NSCR,
		NARC_ending_ed_scene08m_NCLR,
		-16, -1,
	},
	{
		NARC_ending_ed_scene09m_NCGR,
		NARC_ending_ed_scene_bg_NSCR,
		NARC_ending_ed_scene09m_NCLR,
		-44, -15,
	},
	{
		NARC_ending_ed_scene10m_NCGR,
		NARC_ending_ed_scene_bg_NSCR,
		NARC_ending_ed_scene10m_NCLR,
		-30, -30,
	},
};

///キャプチャーグラフィックID管理テーブル(女用)
static const CAPTURE_GRA_TBL CaptureGraphicIDTbl_Female[] = {
	{
		NARC_ending_ed_scene01f_NCGR,
		NARC_ending_ed_scene_bg_NSCR,
		NARC_ending_ed_scene01f_NCLR,
		0, 0,
	},
	{
		NARC_ending_ed_scene02f_NCGR,
		NARC_ending_ed_scene_bg_NSCR,
		NARC_ending_ed_scene02f_NCLR,
		-32, -48,
	},
	{
		NARC_ending_ed_scene03f_NCGR,
		NARC_ending_ed_scene_bg_NSCR,
		NARC_ending_ed_scene03f_NCLR,
		-16, -30,
	},
	{
		NARC_ending_ed_scene04f_NCGR,
		NARC_ending_ed_scene_bg_NSCR,
		NARC_ending_ed_scene04f_NCLR,
		-42, -2,
	},
	{
		NARC_ending_ed_scene05f_NCGR,
		NARC_ending_ed_scene_bg_NSCR,
		NARC_ending_ed_scene05f_NCLR,
		-8, -20,
	},
	{
		NARC_ending_ed_scene06f_NCGR,
		NARC_ending_ed_scene_bg_NSCR,
		NARC_ending_ed_scene06f_NCLR,
		-50, -16,
	},
	{
		NARC_ending_ed_scene07f_NCGR,
		NARC_ending_ed_scene_bg_NSCR,
		NARC_ending_ed_scene07f_NCLR,
		-40, -34,
	},
	{
		NARC_ending_ed_scene08f_NCGR,
		NARC_ending_ed_scene_bg_NSCR,
		NARC_ending_ed_scene08f_NCLR,
		-16, -1,
	},
	{
		NARC_ending_ed_scene09f_NCGR,
		NARC_ending_ed_scene_bg_NSCR,
		NARC_ending_ed_scene09f_NCLR,
		-44, -15,
	},
	{
		NARC_ending_ed_scene10f_NCGR,
		NARC_ending_ed_scene_bg_NSCR,
		NARC_ending_ed_scene10f_NCLR,
		-30, -30,
	},
};

///キャプチャーグラフィックID管理テーブル(フィールド男用)
static const CAPTURE_GRA_TBL CaptureGraphicIDTbl_FieldMale[] = {
	//最後のフィールドキャプチャ
	{
		NARC_ending_ed_sp_01m_NCGR,
		NARC_ending_ed_spscene_bg_NSCR,
		NARC_ending_ed_sp_01m_NCLR,
		0, 0,
	},
	{
		NARC_ending_ed_sp_02m_NCGR,
		NARC_ending_ed_spscene_bg_NSCR,
		NARC_ending_ed_sp_02m_NCLR,
		0, 0,
	},
	{
		NARC_ending_ed_sp_03m_NCGR,
		NARC_ending_ed_spscene_bg_NSCR,
		NARC_ending_ed_sp_03m_NCLR,
		0, 0,
	},
	{
		NARC_ending_ed_sp_04m_NCGR,
		NARC_ending_ed_spscene_bg_NSCR,
		NARC_ending_ed_sp_04m_NCLR,
		0, 0,
	},
};

///キャプチャーグラフィックID管理テーブル(フィールド女用)
static const CAPTURE_GRA_TBL CaptureGraphicIDTbl_FieldFemale[] = {
	//最後のフィールドキャプチャ
	{
		NARC_ending_ed_sp_01f_NCGR,
		NARC_ending_ed_spscene_bg_NSCR,
		NARC_ending_ed_sp_01f_NCLR,
		0, 0,
	},
	{
		NARC_ending_ed_sp_02f_NCGR,
		NARC_ending_ed_spscene_bg_NSCR,
		NARC_ending_ed_sp_02f_NCLR,
		0, 0,
	},
	{
		NARC_ending_ed_sp_03f_NCGR,
		NARC_ending_ed_spscene_bg_NSCR,
		NARC_ending_ed_sp_03f_NCLR,
		0, 0,
	},
	{
		NARC_ending_ed_sp_04f_NCGR,
		NARC_ending_ed_spscene_bg_NSCR,
		NARC_ending_ed_sp_04f_NCLR,
		0, 0,
	},
};



//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   モデル配置物データに初期値を設定する
 *
 * @param   emw		
 * @param   map_no	何番目のマップか
 */
//--------------------------------------------------------------
void EndingTool_ModelLineArrangeDataInit(ENDING_MAIN_WORK *emw, int map_no)
{
	int line;
	
	for(line = 0; line < MODEL_LINE_MAX; line++){
		emw->mdl_arrange.arrange[line] = MdlArrangeData[map_no][line];
	}
}

#ifdef PM_DEBUG
//--------------------------------------------------------------
/**
 * @brief   配置物パラメータを変更する	※デバッグ用
 *
 * @param   emw			
 * @param   line		
 */
//--------------------------------------------------------------
void EndingTool_Debug_ModelLineArrangeSetting(ENDING_MAIN_WORK *emw, int line)
{
	fx32 data = 0x800;
	int sp_frame = 10;
	int flg = 0;
	MDL_LINE_ARRANGE_DATA *ad;
	
	ad = &emw->mdl_arrange.arrange[line];
	
	if(sys.cont & PAD_BUTTON_R){
		if(sys.cont & PAD_KEY_LEFT){
			ad->end_x -= data;
			flg++;
		}
		if(sys.cont & PAD_KEY_RIGHT){
			ad->end_x += data;
			flg++;
		}
		if(sys.cont & PAD_KEY_UP){
			ad->end_y += data;
			flg++;
		}
		if(sys.cont & PAD_KEY_DOWN){
			ad->end_y -= data;
			flg++;
		}
		if(sys.cont & PAD_BUTTON_X){
			ad->end_z -= data;
			flg++;
		}
		if(sys.cont & PAD_BUTTON_B){
			ad->end_z += data;
			flg++;
		}

		//配置物の数を変更
		if(sys.trg & PAD_BUTTON_Y){
			ad->mdl_num--;
			if(ad->mdl_num < 1){
				ad->mdl_num = 1;
			}
			flg++;
		}
		if(sys.trg & PAD_BUTTON_A){
			ad->mdl_num++;
			if(ad->mdl_num > ENDING_SCENE_LINE_3DOBJ_MAX){
				ad->mdl_num = ENDING_SCENE_LINE_3DOBJ_MAX;
			}
			flg++;
		}
	}
	else{
		if(sys.cont & PAD_KEY_LEFT){
			ad->space_x -= data;
			flg++;
		}
		if(sys.cont & PAD_KEY_RIGHT){
			ad->space_x += data;
			flg++;
		}
		if(sys.cont & PAD_KEY_UP){
			ad->space_y += data;
			flg++;
		}
		if(sys.cont & PAD_KEY_DOWN){
			ad->space_y -= data;
			flg++;
		}
		if(sys.cont & PAD_BUTTON_X){
			ad->space_z -= data;
			flg++;
		}
		if(sys.cont & PAD_BUTTON_B){
			ad->space_z += data;
			flg++;
		}

		//速度変更
		if(sys.trg & PAD_BUTTON_Y){
			ad->move_frame -= sp_frame;
			if(ad->move_frame < 4){
				ad->move_frame = 4;
			}
			flg++;
		}
		if(sys.trg & PAD_BUTTON_A){
			ad->move_frame += sp_frame;
			flg++;
		}
	}

	if(flg){
		EndingTool_ModelLineInit(emw);
		OS_TPrintf("arrange mdl_num = %d, move_frame = %d, end_x = %d, end_y = %d, end_z = %d, space_x = %d, space_y = %d, space_z = %d\n", ad->mdl_num, ad->move_frame, ad->end_x, ad->end_y, ad->end_z, ad->space_x, ad->space_y, ad->space_z);
	}
}
#endif	//PM_DEBUG

//--------------------------------------------------------------
/**
 * @brief   モデルデータの座標やスケールを初期設定する
 *
 * @param   emw		
 */
//--------------------------------------------------------------
void EndingTool_ModelLineInit(ENDING_MAIN_WORK *emw)
{
	int i, line;
	MDL_LINE_ARRANGE_DATA *ad;
	
	for(line = 0; line < emw->mdl_line_num; line++){
		ad = &emw->mdl_arrange.arrange[line];
		GF_ASSERT(ad->mdl_num <= ENDING_SCENE_LINE_3DOBJ_MAX);
		for(i = 0; i < ad->mdl_num; i++){
			D3DOBJ_SetMatrix(&emw->obj[line][i], 
				ad->end_x - ad->space_x * i,
				ad->end_y - ad->space_y * i,
				ad->end_z - ad->space_z * i);
			D3DOBJ_SetScale(&emw->obj[line][i], FX32_ONE, FX32_ONE, FX32_ONE);
			D3DOBJ_SetDraw(&emw->obj[line][i], TRUE );
		}
		for( ; i < ENDING_SCENE_LINE_3DOBJ_MAX; i++){
			D3DOBJ_SetMatrix(&emw->obj[line][i], 
				MODEL_LINE_START_X, 
				MODEL_LINE_START_Y, 
				MODEL_LINE_START_Z);
			D3DOBJ_SetScale(&emw->obj[line][i], FX32_ONE, FX32_ONE, FX32_ONE);
			D3DOBJ_SetDraw(&emw->obj[line][i], FALSE );
		}
	}
	
	//邪魔なので一時的に消す
//	for(i = 0; i < ENDING_SCENE_LINE_3DOBJ_MAX; i++){
//		D3DOBJ_SetDraw(&emw->obj[MODEL_LINE_1][i], FALSE );
//	}
}

//--------------------------------------------------------------
/**
 * @brief   モデルデータのスクロール
 *
 * @param   emw		
 * @param   mdl_index_no	回り込んだモデルに割り当てるモデルIndex(未使用の場合は-1)
 * 
 * @retval  TRUE:回り込みが発生した
 */
//--------------------------------------------------------------
BOOL EndintTool_ModelLineScroll(ENDING_MAIN_WORK *emw, s32 mdl_index_no)
{
	fx32 add_x, add_y, add_z;
	fx32 pos_x, pos_y, pos_z;
	int line, i;
	MDL_LINE_ARRANGE_DATA *ad;
	BOOL rotate_ret = FALSE;
	
#ifdef DEBUG_ENDING_CAMERA
	if(sys.cont & PAD_BUTTON_L){
		return FALSE;
	}
#endif

	for(line = 0; line < emw->mdl_line_num; line++){
		ad = &emw->mdl_arrange.arrange[line];
		add_x = ad->space_x * ad->mdl_num / ad->move_frame;
		add_y = ad->space_y * ad->mdl_num / ad->move_frame;
		add_z = ad->space_z * ad->mdl_num / ad->move_frame;

		for(i = 0; i < ad->mdl_num; i++){
			D3DOBJ_GetMatrix( &emw->obj[line][i], &pos_x, &pos_y, &pos_z);
			if(pos_x + add_x >= ad->end_x){
				D3DOBJ_SetMatrix(&emw->obj[line][i], 
					pos_x + add_x - ad->space_x * ad->mdl_num, 
					pos_y + add_y - ad->space_y * ad->mdl_num, 
					pos_z + add_z - ad->space_z * ad->mdl_num);
				//シーン３専用の処理
				if(mdl_index_no != -1 && line == MODEL_LINE_0){
					//D3DOBJ_Init( &emw->obj[line][i], &emw->mdl[mdl_index_no] );
					MI_CpuClear8(&emw->obj[line][i].render, sizeof(NNSG3dRenderObj));
					NNS_G3dRenderObjInit( 
						&emw->obj[line][i].render, emw->mdl[mdl_index_no].pModel );
				}
				rotate_ret = TRUE;
			}
			else{
				D3DOBJ_SetMatrix(&emw->obj[line][i], 
					pos_x + add_x, pos_y + add_y, pos_z + add_z);
			}
			D3DOBJ_SetDraw(&emw->obj[line][i], TRUE );
		}
	}

	//邪魔なので一時的に消す
//	for(i = 0; i < ENDING_SCENE_LINE_3DOBJ_MAX; i++){
//		D3DOBJ_SetDraw(&emw->obj[MODEL_LINE_1][i], FALSE );
//	}

	return rotate_ret;
}

//--------------------------------------------------------------
/**
 * @brief   キャプチャ画像をVRAM転送する
 *
 * @param   emw			エンディングメインワークへのポインタ
 * @param   cap_no		キャプチャー番号
 * @param   frameno		BGフレーム番号
 * @param   tbl_type	CAPTURE_TBL_TYPE_???
 */
//--------------------------------------------------------------
void EndingTool_CaptureGraphicTrans(ENDING_MAIN_WORK *emw, int cap_no, int frame_no, int tbl_type)
{
	GF_BGL_INI *bgl = emw->bgl;
	NNSG2dPaletteData *palData;
	void *p_work;
	const CAPTURE_GRA_TBL *gratbl;
	
	GF_ASSERT(NELEMS(CaptureGraphicIDTbl_Male) == NELEMS(CaptureGraphicIDTbl_Female));
	GF_ASSERT(NELEMS(CaptureGraphicIDTbl_FieldMale) == NELEMS(CaptureGraphicIDTbl_FieldFemale));
	
	switch(tbl_type){
	case CAPTURE_TBL_TYPE_PICTURE:
		GF_ASSERT(NELEMS(CaptureGraphicIDTbl_Male) > cap_no);
		if(emw->parent_work->playerSex == PM_MALE){
			gratbl = &CaptureGraphicIDTbl_Male[cap_no];
		}
		else{
			gratbl = &CaptureGraphicIDTbl_Female[cap_no];
		}
		break;
	case CAPTURE_TBL_TYPE_FIELD:
		GF_ASSERT(NELEMS(CaptureGraphicIDTbl_Female) > cap_no);
		if(emw->parent_work->playerSex == PM_MALE){
			gratbl = &CaptureGraphicIDTbl_FieldMale[cap_no];
		}
		else{
			gratbl = &CaptureGraphicIDTbl_FieldFemale[cap_no];
		}
		break;
	}
	
	// パレットデータを拡張パレットVRAMに転送
	p_work = ArcUtil_HDL_PalDataGet(emw->ending_hdl, 
		gratbl->pal_id, &palData, HEAPID_ENDING_DEMO);
	DC_FlushRange(palData->pRawData, palData->szByte);
	GX_BeginLoadBGExtPltt();	   // パレットデータの転送準備
	if(frame_no == GF_BGL_FRAME3_M){
		GX_LoadBGExtPltt(palData->pRawData, 0x6000, 0x2000);
	}
	else{
		GX_LoadBGExtPltt(palData->pRawData, 0x4000, 0x2000);
	}
    GX_EndLoadBGExtPltt();         // パレットデータの転送完了
    sys_FreeMemoryEz(p_work);

	//画面外を真っ黒にする為、先頭のパレットに強制で黒をセット
	PaletteWork_Clear(emw->pfd, FADE_MAIN_BG, FADEBUF_ALL, 0x0000, 0, 1);
	PaletteWork_Clear(emw->pfd, FADE_SUB_BG, FADEBUF_ALL, 0x0000, 0, 1);

	ArcUtil_HDL_BgCharSet(emw->ending_hdl, gratbl->ncg_id, emw->bgl, 
		frame_no, 0, 0, 0, HEAPID_ENDING_DEMO);

	ArcUtil_HDL_ScrnSet(emw->ending_hdl, gratbl->nsc_id,
		emw->bgl, frame_no, 0, 0, 0, HEAPID_ENDING_DEMO);
	
	GF_BGL_ScrollSet(bgl, frame_no, GF_BGL_SCROLL_X_SET, gratbl->scr_x);
	GF_BGL_ScrollSet(bgl, frame_no, GF_BGL_SCROLL_Y_SET, gratbl->scr_y);
}

//--------------------------------------------------------------
/**
 * @brief   主人公アニメ更新処理
 *
 * @param   emw		
 * @param   cap		主人公アクターへのポインタ
 *
 * @retval  
 */
//--------------------------------------------------------------
void EndingTool_HeroAnimeUpdate(ENDING_MAIN_WORK *emw, CATS_ACT_PTR cap)
{
	void *obj_vram;
	u32 anmseq;
	NNSG2dImageProxy *image;
	
	if(emw->hero_eye_charbuf == NULL){
		return;
	}
	
	//まばたき
	if(emw->hero_anm_work.run_anm_count == -1){	//漕いでいないときだけ瞬き
		if(emw->hero_anm_work.mabataki_frame > 0){
			emw->hero_anm_work.mabataki_frame--;
		}
		else{
			obj_vram = G2_GetOBJCharPtr();
			image = CLACT_ImageProxyGet(cap->act);
			//出来上がったものをVramへ書き戻す
			if(emw->parent_work->playerSex == PM_MALE){
				MI_CpuCopy32(
					&emw->hero_eye_charbuf[emw->hero_anm_work.mabataki_anmno*(HERO_EYE_CHARBUF_SIZE/2)], 
					(void*)((u32)obj_vram + HERO_MABATAKI_CHAR_POS_0*0x20 
					+ image->vramLocation.baseAddrOfVram[NNS_G2D_VRAM_TYPE_2DMAIN]), 
					0x20);
				MI_CpuCopy32(
					&emw->hero_eye_charbuf[emw->hero_anm_work.mabataki_anmno*(HERO_EYE_CHARBUF_SIZE/2)+0x20], 
					(void*)((u32)obj_vram + HERO_MABATAKI_CHAR_POS_1*0x20 
					+ image->vramLocation.baseAddrOfVram[NNS_G2D_VRAM_TYPE_2DMAIN]), 
					0x20);
				MI_CpuCopy32(
					&emw->hero_eye_charbuf[emw->hero_anm_work.mabataki_anmno*(HERO_EYE_CHARBUF_SIZE/2)+0x40], 
					(void*)((u32)obj_vram + HERO_MABATAKI_CHAR_POS_2*0x20 
					+ image->vramLocation.baseAddrOfVram[NNS_G2D_VRAM_TYPE_2DMAIN]), 
					0x20);
				MI_CpuCopy32(
					&emw->hero_eye_charbuf[emw->hero_anm_work.mabataki_anmno*(HERO_EYE_CHARBUF_SIZE/2)+0x60], 
					(void*)((u32)obj_vram + HERO_MABATAKI_CHAR_POS_3*0x20 
					+ image->vramLocation.baseAddrOfVram[NNS_G2D_VRAM_TYPE_2DMAIN]), 
					0x20);
			}
			else{
				MI_CpuCopy32(
					&emw->hero_eye_charbuf[emw->hero_anm_work.mabataki_anmno*(HERO_EYE_CHARBUF_SIZE/2)], 
					(void*)((u32)obj_vram + HEROINE_MABATAKI_CHAR_POS_0*0x20 
					+ image->vramLocation.baseAddrOfVram[NNS_G2D_VRAM_TYPE_2DMAIN]), 
					0x20);
				MI_CpuCopy32(
					&emw->hero_eye_charbuf[emw->hero_anm_work.mabataki_anmno*(HERO_EYE_CHARBUF_SIZE/2)+0x20], 
					(void*)((u32)obj_vram + HEROINE_MABATAKI_CHAR_POS_1*0x20 
					+ image->vramLocation.baseAddrOfVram[NNS_G2D_VRAM_TYPE_2DMAIN]), 
					0x20);
				MI_CpuCopy32(
					&emw->hero_eye_charbuf[emw->hero_anm_work.mabataki_anmno*(HERO_EYE_CHARBUF_SIZE/2)+0x40], 
					(void*)((u32)obj_vram + HEROINE_MABATAKI_CHAR_POS_2*0x20 
					+ image->vramLocation.baseAddrOfVram[NNS_G2D_VRAM_TYPE_2DMAIN]), 
					0x20);
				MI_CpuCopy32(
					&emw->hero_eye_charbuf[emw->hero_anm_work.mabataki_anmno*(HERO_EYE_CHARBUF_SIZE/2)+0x60], 
					(void*)((u32)obj_vram + HEROINE_MABATAKI_CHAR_POS_3*0x20 
					+ image->vramLocation.baseAddrOfVram[NNS_G2D_VRAM_TYPE_2DMAIN]), 
					0x20);
			}

			if(emw->hero_anm_work.mabataki_anmno == 0){
				if(emw->hero_anm_work.mabataki_count % 3 == 0){
					if(gf_rand() & 1){
						emw->hero_anm_work.mabataki_frame = gf_rand() % 10;	//あけている時間
						emw->hero_anm_work.mabataki_count++;
					}
					else{
						emw->hero_anm_work.mabataki_frame = HERO_MABATAKI_OPEN_TIME;
					}
				}
				else{
					emw->hero_anm_work.mabataki_frame = HERO_MABATAKI_OPEN_TIME;//あけている時間
					emw->hero_anm_work.mabataki_count++;
				}
			}
			else{
				emw->hero_anm_work.mabataki_frame = HERO_MABATAKI_CLOSE_TIME;	//瞬きを閉じている時間
			}
			emw->hero_anm_work.mabataki_anmno ^= 1;
		}
	}
	
	//走りアニメ
	if(CATS_ObjectAnimeActiveCheckCap(cap) == FALSE){
		if(emw->hero_anm_work.run_anm_count != -1){
			emw->hero_anm_work.run_anm_count++;
			if(emw->hero_anm_work.run_anm_count > HERO_RUN_LOOP_COUNT && (gf_rand() % 5 == 0)){
				anmseq = 1;
				emw->hero_anm_work.run_anm_count = -1;
			}
			else{
				anmseq = 0;
			}
			CATS_ObjectAnimeSeqSetCap(cap, anmseq);
		}
		else{
			emw->hero_anm_work.run_anm_count = 0;
			CATS_ObjectAnimeSeqSetCap(cap, 0);
		}
	}
}
