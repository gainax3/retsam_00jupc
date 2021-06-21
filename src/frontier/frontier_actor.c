//==============================================================================
/**
 * @file	frontier_actor.c
 * @brief	フロンティア2Dマップで使用するアクター
 * @author	matsuda
 * @date	2007.05.30(水)
 */
//==============================================================================
#include "common.h"
#include "system/arc_tool.h"
#include "system/clact_tool.h"

#include "frontier_act_pri.h"
#include "graphic/frontier_obj_def.h"
#include "frontier_act_id.h"
#include "frontier_actor.h"


//==============================================================================
//	アクターヘッダ
//==============================================================================
//==============================================================================
//
//	アクターヘッダ
//			リソースIDをそのままマネージャの管理IDとしても使用しています
//
//	※frontier_act_id.hにFrontierArticleActorHeadTblの並びで定義を書くこと！
//
//==============================================================================
static const TCATS_OBJECT_ADD_PARAM_S FrontierArticleActorHeadTbl[] = {
	//ACTID_TEST_BALL
	{
		0, 0, 0,						//x, y, z
		0, ACT_SOFTPRI_TEST, 0,			//アニメ番号、優先順位、パレット番号
		NNS_G2D_VRAM_TYPE_2DMAIN,		//描画エリア
		{	//使用リソースIDテーブル
#if 0
			TEST_BALL32K_NCGR_BIN,		//キャラ
			TEST_BALL32K_NCLR,				//パレット
			TEST_BALL32K_NCER_BIN,		//セル
			TEST_BALL32K_NANR_BIN,		//セルアニメ
#else
			BT_OBJ_DOOR_NCGR_BIN,		//キャラ
			BT_OBJ_DOOR_NCLR,			//パレット
			BT_OBJ_DOOR_NCER_BIN,		//セル
			BT_OBJ_DOOR_NANR_BIN,		//セルアニメ
#endif
			CLACT_U_HEADER_DATA_NONE,	//マルチセル
			CLACT_U_HEADER_DATA_NONE,	//マルチセルアニメ
		},
		ACT_BGPRI_TEST,					//BGプライオリティ
		0,								//VRAM転送フラグ
	},
/*	//ACTID_TEST_BALL2
	{
		0, 0, 0,						//x, y, z
		0, ACT_SOFTPRI_TEST, 0,			//アニメ番号、優先順位、パレット番号
		NNS_G2D_VRAM_TYPE_2DMAIN,		//描画エリア
		{	//使用リソースIDテーブル
			TEST_BALL32K_NCGR_BIN,		//キャラ
			TEST_BALL32K_NCLR,				//パレット
			TEST_BALL32K_NCER_BIN,		//セル
			TEST_BALL32K_NANR_BIN,		//セルアニメ
			CLACT_U_HEADER_DATA_NONE,	//マルチセル
			CLACT_U_HEADER_DATA_NONE,	//マルチセルアニメ
		},
		ACT_BGPRI_TEST,					//BGプライオリティ
		0,								//VRAM転送フラグ
	},
*/
	//ACTID_TOWER_DOOR
	{
		0, 0, 0,						//x, y, z
		0, ACT_SOFTPRI_TEST, 0,			//アニメ番号、優先順位、パレット番号
		NNS_G2D_VRAM_TYPE_2DMAIN,		//描画エリア
		{	//使用リソースIDテーブル
			BT_OBJ_DOOR_NCGR_BIN,		//キャラ
			BT_OBJ_DOOR_NCLR,			//パレット
			BT_OBJ_DOOR_NCER_BIN,		//セル
			BT_OBJ_DOOR_NANR_BIN,		//セルアニメ
			CLACT_U_HEADER_DATA_NONE,	//マルチセル
			CLACT_U_HEADER_DATA_NONE,	//マルチセルアニメ
		},
		ACT_BGPRI_TEST,					//BGプライオリティ
		0,								//VRAM転送フラグ
	},

	//ACTID_ROULETTE_RAMP
	{
		0, 0, 0,						//x, y, z
		0, ACT_SOFTPRI_TEST, 0,			//アニメ番号、優先順位、パレット番号
		NNS_G2D_VRAM_TYPE_2DMAIN,		//描画エリア
		{	//使用リソースIDテーブル
			BR_RAMP_NCGR_BIN,			//キャラ
			BR_RAMP_NCLR,				//パレット
			BR_RAMP_NCER_BIN,			//セル
			BR_RAMP_NANR_BIN,			//セルアニメ
			CLACT_U_HEADER_DATA_NONE,	//マルチセル
			CLACT_U_HEADER_DATA_NONE,	//マルチセルアニメ
		},
		ACT_BGPRI_TEST,					//BGプライオリティ
		0,								//VRAM転送フラグ
	},

	//ACTID_ROULETTE_POKEPANEL
	{
		0, 0, 0,						//x, y, z
		0, ACT_SOFTPRI_TEST, 0,			//アニメ番号、優先順位、パレット番号
		NNS_G2D_VRAM_TYPE_2DMAIN,		//描画エリア
		{	//使用リソースIDテーブル
			BR_POKEPANEL_NCGR_BIN,		//キャラ
			BR_POKEPANEL_NCLR,			//パレット
			BR_POKEPANEL_NCER_BIN,		//セル
			BR_POKEPANEL_NANR_BIN,		//セルアニメ
			CLACT_U_HEADER_DATA_NONE,	//マルチセル
			CLACT_U_HEADER_DATA_NONE,	//マルチセルアニメ
		},
		ACT_BGPRI_TEST,					//BGプライオリティ
		0,								//VRAM転送フラグ
	},

};



//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   
 *
 * @param   csp		
 * @param   crp		
 * @param   hdl		
 * @param   pfd		
 * @param   act_id		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
void FAct_ResourceLoad(CATS_SYS_PTR csp, CATS_RES_PTR crp, ARCHANDLE* hdl, 
	PALETTE_FADE_PTR pfd, u16 act_id)
{
	const TCATS_OBJECT_ADD_PARAM_S *obj_head;
	
	GF_ASSERT(act_id < NELEMS(FrontierArticleActorHeadTbl));
	obj_head = &FrontierArticleActorHeadTbl[act_id];
	
	CATS_LoadResourceCharArcH(csp, crp, hdl, obj_head->id[CLACT_U_CHAR_RES], 1, 
		NNS_G2D_VRAM_TYPE_2DMAIN, obj_head->id[CLACT_U_CHAR_RES]);
	CATS_LoadResourcePlttWorkArcH(pfd, FADE_MAIN_OBJ, csp, crp, 
		hdl, obj_head->id[CLACT_U_PLTT_RES], 0, 
		1, NNS_G2D_VRAM_TYPE_2DMAIN, obj_head->id[CLACT_U_PLTT_RES]);
	CATS_LoadResourceCellArcH(csp, crp, hdl, obj_head->id[CLACT_U_CELL_RES], 1, 
		obj_head->id[CLACT_U_CELL_RES]);
	CATS_LoadResourceCellAnmArcH(csp, crp, hdl, 
		obj_head->id[CLACT_U_CELLANM_RES], 1, obj_head->id[CLACT_U_CELLANM_RES]);
}

//--------------------------------------------------------------
/**
 * @brief   ゲージのリソースを解放する
 * @param   gauge		ゲージワークへのポインタ
 */
//--------------------------------------------------------------
void FAct_ResourceFree(CATS_RES_PTR crp, u16 act_id)
{
	const TCATS_OBJECT_ADD_PARAM_S *obj_head;

	GF_ASSERT(act_id < NELEMS(FrontierArticleActorHeadTbl));
	obj_head = &FrontierArticleActorHeadTbl[act_id];

	//リソース解放
	CATS_FreeResourceChar(crp, obj_head->id[CLACT_U_CHAR_RES]);
	CATS_FreeResourcePltt(crp, obj_head->id[CLACT_U_PLTT_RES]);
	CATS_FreeResourceCell(crp, obj_head->id[CLACT_U_CELL_RES]);
	CATS_FreeResourceCellAnm(crp, obj_head->id[CLACT_U_CELLANM_RES]);
}

//--------------------------------------------------------------
/**
 * @brief   
 *
 * @param   csp		
 * @param   crp		
 * @param   act_id		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
CATS_ACT_PTR FAct_ActorSet(CATS_SYS_PTR csp, CATS_RES_PTR crp, u16 act_id)
{
	CATS_ACT_PTR cap;
	
	GF_ASSERT(act_id < NELEMS(FrontierArticleActorHeadTbl));
	
	cap = CATS_ObjectAdd_S(csp, crp, &FrontierArticleActorHeadTbl[act_id]);
	CATS_ObjectUpdate(cap->act);
	return cap;
}

//--------------------------------------------------------------
/**
 * @brief   
 *
 * @param   cap		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
void FAct_ActorDel(CATS_ACT_PTR cap)
{
	CATS_ActorPointerDelete_S(cap);
}

