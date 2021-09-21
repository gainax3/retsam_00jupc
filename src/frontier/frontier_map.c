//==============================================================================
/**
 * @file	frontier_map.c
 * @brief	フロンティアマップ
 * @author	matsuda
 * @date	2007.04.05(木)
 */
//==============================================================================
#include "common.h"
#include "system/pm_overlay.h"
#include "system/procsys.h"
#include "system/pmfprint.h"
#include "system/palanm.h"
#include "system/wipe.h"
#include "system/snd_tool.h"
#include "system/bmp_list.h"
#include "system/font_arc.h"
#include "system/window.h"
//#include "script.h"
#include "..\fielddata\script\connect_def.h"
#include "system/clact_tool.h"
#include "communication/wm_icon.h"

#include "system/msgdata.h"
#include "msgdata/msg.naix"
#include "msgdata/msg_debug_matsu.h"

#include "frontier_types.h"
#include "fss_scene.h"
#include "frontier_main.h"
#include "frontier_map.h"
#include "frontier_tcb_pri.h"
#include "frontier_particle.h"
#include "frontier_actor.h"
#include "frontier_tool.h"

#include "graphic/frontier_bg_def.h"




//==============================================================================
//	定数定義
//==============================================================================
///マップ面スクリーンベース
#define MAPBG_SCRBASE		(GX_BG_SCRBASE_0x2800)
///マップ面キャラクタベース
#define MAPBG_CHARBASE		(GX_BG_CHARBASE_0x30000)

///エフェクト面(多重面)スクリーンベース
#define EFFBG_SCRBASE		(GX_BG_SCRBASE_0x0800)
///エフェクト面(多重面)キャラクタベース
#define EFFBG_CHARBASE		(GX_BG_CHARBASE_0x20000)

///リクエストコマンドキュー数
#define FMAP_REQCMDQ_NUM		(128)
///アクションコマンドキュー数
#define FMAP_ACTCMDQ_NUM		(128)

//==============================================================================
//	構造体定義
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   ゲーム画面を構成するために必要なシステムデータ類を持つ構造体
 *
 * 通信時でも全てのマシンで個別に作成されます
 * 各スタッフが命令受信時、そのマシン個々で生成したこのワークバッファを渡す事になります
 */
//--------------------------------------------------------------
#if 0
typedef struct{
//	CATS_SYS_PTR		csp;
//	CATS_RES_PTR		crp;
	GF_BGL_INI *bgl;
//	GF_BGL_BMPWIN win[CONRES_BMPWIN_MAX];

//	MSGDATA_MANAGER *conres_msg;	///<ダンス部門メッセージ用メッセージマネージャのポインタ
//	WORDSET *wordset;
//	STRBUF *msg_buf;				///<Allocした文字列バッファへのポインタ

	PALETTE_FADE_PTR pfd;			///<パレットフェードシステムへのポインタ
	
	FMAIN_PTR fmain;
	
	//2Dマップシステムモジュール
	WF2DMAP_OBJSYS *objsys;		///<オブジェクト管理モジュール
	
}FRMAP_SYSTEM_PARAM;
#endif


//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static void FrontierMapVBlank(void *work);
static void FrontierMapHBlank(void *work);
static void VBlankTCB_IntrTask(TCB_PTR tcb, void *work);
static void FrontierMapUpdate(TCB_PTR tcb, void *work);
static void FrontierMapOBJSysUpdate(TCB_PTR tcb, void *work);
static void FrontierMapOBJCmdJudgeUpdate(TCB_PTR tcb, void *work);
static void FrontierMap_VramBankSet(GF_BGL_INI *bgl, int scene_id);
static void MapSystem2D_ModuleSet(FMAP_PTR fmap, int scene_id, const MYSTATUS *my_status);
static void MapSystem2D_ModuleFree(FMAP_PTR fmap);
static void SystemPaletteLoad(FMAP_PTR fmap);
static void ActorSysInit(FMAP_PTR fmap);
static void ActorSysFree(FMAP_PTR fmap);
FMAP_PTR FrontierMap_Init(FMAIN_PTR fmain);
void FrontierMap_End(FMAP_PTR fmap);
static void ObjCmdJudgeMove(FMAP_PTR fmap);
static void SubDisp_BGSet(FMAP_PTR fmap);
static GF_G3DMAN * Frontier_3D_Init(int heap_id);
static void Frontier_3D_SimpleSetUp(void);
static void Frontier_3D_Exit(GF_G3DMAN *g3Dman);
static void FMap_ArticleActPush(FMAP_PTR fmap);
static void FMap_ArticleActPop(FMAP_PTR fmap);
static void Frontier_ActWork_to_ActData(FSS_ACTOR_WORK *fss_actor, FSS_CHAR_ACTOR_DATA *act_data);
static void Frontier_ActWorkDataSet(FMAIN_PTR fmain, int act_no, WF2DMAP_OBJWK *objwk, WF2DMAP_OBJDRAWWK *drawwk, const FSS_CHAR_ACTOR_DATA *res);
static void FrontierMapScroll(FMAP_PTR fmap);
static void FrontierMapScroll_Easy(FMAP_PTR fmap);


//==============================================================================
//	データ
//==============================================================================
//==============================================================================
//	CLACT用データ
//==============================================================================
static	const TCATS_OAM_INIT MapTcats = {
	FMAP_OAM_START_MAIN, FMAP_OAM_END_MAIN,
	FMAP_OAM_AFFINE_START_MAIN, FMAP_OAM_AFFINE_END_MAIN,
	FMAP_OAM_START_SUB, FMAP_OAM_END_SUB,
	FMAP_OAM_AFFINE_START_SUB, FMAP_OAM_AFFINE_END_SUB,
};

static	const TCATS_CHAR_MANAGER_MAKE MapCcmm = {
	FMAP_CHAR_MAX,
	FMAP_CHAR_VRAMSIZE_MAIN,
	FMAP_CHAR_VRAMSIZE_SUB,
	GX_OBJVRAMMODE_CHAR_1D_128K,	//人物OBJのアニメが128kなので。VRAMは64kしか当てていないので人物アニメの64k版が用意出来るのならば、マッピングも64kにした方が隙間無くVRAMを使える
	GX_OBJVRAMMODE_CHAR_1D_32K
};

static const TCATS_RESOURCE_NUM_LIST MapResourceList = {
	FMAP_OAMRESOURCE_CHAR_MAX,
	FMAP_OAMRESOURCE_PLTT_MAX,
	FMAP_OAMRESOURCE_CELL_MAX,
	FMAP_OAMRESOURCE_CELLANM_MAX,
	FMAP_OAMRESOURCE_MCELL_MAX,
	FMAP_OAMRESOURCE_MCELLANM_MAX,
};

//--------------------------------------------------------------
//	ダミーマップデータ
//--------------------------------------------------------------
static const WF2DMAP_MAP DumyMap[MAP_GRID_MAX_X * MAP_GRID_MAX_Y] = {
	0,
};


//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   フロンティアマップシステム初期化
 *
 * @param   proc		プロセスデータ
 * @param   seq			シーケンス
 *
 * @retval  処理状況
 */
//--------------------------------------------------------------
FMAP_PTR FrontierMap_Init(FMAIN_PTR fmain)
{
	FMAP_PTR fmap;
	int i, scene_id;
	FRONTIER_EX_PARAM *ex_param = Frontier_ExParamGet(fmain);
	const MYSTATUS *my_status;
	
	my_status = SaveData_GetMyStatus(ex_param->savedata);
	scene_id = ex_param->scene_id;
	
//	Overlay_Load(FS_OVERLAY_ID(frontier_common), OVERLAY_LOAD_NOT_SYNCHRONIZE);

	sys_VBlankFuncChange(NULL, NULL);	// VBlankセット
	sys_HBlankIntrStop();	//HBlank割り込み停止

	GF_Disp_GX_VisibleControlInit();
	GF_Disp_GXS_VisibleControlInit();
	GX_SetVisiblePlane(0);
	GXS_SetVisiblePlane(0);
	GX_SetVisibleWnd(GX_WNDMASK_NONE);
	GXS_SetVisibleWnd(GX_WNDMASK_NONE);
	G2_BlendNone();
	G2S_BlendNone();

	sys_CreateHeap(HEAPID_BASE_APP, HEAPID_FRONTIERMAP, FRONTIERMAP_ALLOC_SIZE);

	fmap = sys_AllocMemory(HEAPID_FRONTIERMAP, sizeof(FRONTIERMAP_WORK));
	MI_CpuClear8(fmap, sizeof(FRONTIERMAP_WORK));
	fmap->fmain = fmain;
	
	//終了動作関数EndFuncが呼ばれる前にscene_idが変わる場合があるのでここで初期動作、
	//終了動作関数呼び出しようのシーンIDを記憶
	fmap->func_scene_id = scene_id;
	
	for(i = 0; i < ACTWORK_RESOURCE_MAX; i++){
		fmap->article.resource_id[i] = SCR_ACTWORK_RESOURCE_ID_NULL;
	}

	fmap->g3Dman = Frontier_3D_Init(HEAPID_FRONTIERMAP);
	
	//パレットフェードシステム作成
	fmap->pfd = PaletteFadeInit(HEAPID_FRONTIERMAP);
	PaletteTrans_AutoSet(fmap->pfd, TRUE);
	PaletteFadeWorkAllocSet(fmap->pfd, FADE_MAIN_BG, 0x200, HEAPID_FRONTIERMAP);
	PaletteFadeWorkAllocSet(fmap->pfd, FADE_SUB_BG, 0x200, HEAPID_FRONTIERMAP);
	PaletteFadeWorkAllocSet(fmap->pfd, FADE_MAIN_OBJ, 0x200 - 0x40, HEAPID_FRONTIERMAP);
	PaletteFadeWorkAllocSet(fmap->pfd, FADE_SUB_OBJ, 0x200, HEAPID_FRONTIERMAP);
	//ファクトリー対戦部屋の初期動作関数で、FADE_MAIN_BG_EX3の追加をしています(07.11.27)(未使用)

	fmap->bgl = GF_BGL_BglIniAlloc(HEAPID_FRONTIERMAP);

	initVramTransferManagerHeap(64, HEAPID_FRONTIERMAP);

	sys_KeyRepeatSpeedSet( SYS_KEYREPEAT_SPEED_DEF, SYS_KEYREPEAT_WAIT_DEF );

	//VRAM割り当て設定
	FrontierMap_VramBankSet(fmap->bgl, scene_id);

	//BG読み込み
	SystemPaletteLoad(fmap);
	SubDisp_BGSet(fmap);
	
	// タッチパネルシステム初期化
	InitTPSystem();
	InitTPNoBuff(4);

	//アクターシステム作成
	ActorSysInit(fmap);

	//パーティクルシステム初期化
	fmap->frp = FRParticle_Init(HEAPID_FRONTIERMAP);

	//2Dマップシステムモジュール設定
	MapSystem2D_ModuleSet(fmap, scene_id, my_status);

//	WIPE_SYS_Start(WIPE_PATTERN_FMAS, WIPE_TYPE_SCREWIN, WIPE_TYPE_SCREWIN, WIPE_FADE_BLACK, 
//		WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_FRONTIERMAP);

	fmap->objupdate_tcb = TCB_Add(FrontierMapOBJSysUpdate, fmap, TCBPRI_OBJSYS_UPDATE);
	fmap->cmdjudgeupdate_tcb = TCB_Add(FrontierMapOBJCmdJudgeUpdate, fmap, TCBPRI_CMDJUDGE_UPDATE);
	fmap->update_tcb = TCB_Add(FrontierMapUpdate, fmap, TCBPRI_MAP_UPDATE);

	GF_Disp_DispOn();
	GF_Disp_GX_VisibleControl(GX_PLANEMASK_OBJ, VISIBLE_ON);
	GF_Disp_GXS_VisibleControl(GX_PLANEMASK_OBJ, VISIBLE_ON);

	//サウンドデータロード(コンテスト)
	Snd_DataSetByScene( SND_SCENE_FIELD, 
						FSS_SceneParamGet(scene_id, FSS_SCENE_DATA_BGMID), 1 );

	MsgPrintAutoFlagSet(MSG_AUTO_ON);
	MsgPrintSkipFlagSet(MSG_SKIP_OFF);
	MsgPrintTouchPanelFlagSet(MSG_TP_OFF);
	
	sys_VBlankFuncChange(FrontierMapVBlank, fmap);
	//↓WipeでHブランクを使用するので、Wipeが終了したらセットするように変更
//	sys_HBlankIntrSet(FrontierMapHBlank, fmap);
	fmap->vintr_tcb = VIntrTCB_Add(VBlankTCB_IntrTask, fmap, 10);
	
	//シーンデータに設定されている初期動作関数呼び出し
	
	FSS_SceneInitFuncCall(fmap, &fmap->scene_func_work_ptr, fmap->func_scene_id);
	
	WirelessIconEasy();	//通信アイコン

	return fmap;
}

//--------------------------------------------------------------
/**
 * @brief   フロンティアマップシステム終了
 *
 * @param   proc		プロセスデータ
 * @param   seq			シーケンス
 *
 * @retval  処理状況
 */
//--------------------------------------------------------------
void FrontierMap_End(FMAP_PTR fmap)
{
	int i;

	//シーンデータに設定されている終了動作関数呼び出し
	{
		FRONTIER_EX_PARAM *ex_param = Frontier_ExParamGet(fmap->fmain);
		FSS_SceneEndFuncCall(fmap, &fmap->scene_func_work_ptr, fmap->func_scene_id);
	}

	//2Dマップシステムモジュール解放
	MapSystem2D_ModuleFree(fmap);

	//BMP開放
//	for(i = 0; i < CONRES_BMPWIN_MAX; i++){
//		GF_BGL_BmpWinDel(&fmap->win[i]);
//	}

	//メイン画面BG削除
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
	GF_BGL_BGControlExit(fmap->bgl, FRMAP_FRAME_WIN );
	GF_BGL_BGControlExit(fmap->bgl, FRMAP_FRAME_EFF );
	GF_BGL_BGControlExit(fmap->bgl, FRMAP_FRAME_MAP );
	//サブ画面BG削除
	GF_BGL_VisibleSet(FRMAP_FRAME_SUB_AUDIENCE, VISIBLE_OFF);
	GF_BGL_BGControlExit(fmap->bgl, FRMAP_FRAME_SUB_AUDIENCE);

	//アクターシステム削除
	ActorSysFree(fmap);

	//パーティクルシステム終了
	FRParticle_Exit(fmap->frp);

	//Vram転送マネージャー削除
	DellVramTransferManager();

	//パレットフェードシステム削除
	PaletteFadeWorkAllocFree(fmap->pfd, FADE_MAIN_BG);
	PaletteFadeWorkAllocFree(fmap->pfd, FADE_SUB_BG);
	PaletteFadeWorkAllocFree(fmap->pfd, FADE_MAIN_OBJ);
	PaletteFadeWorkAllocFree(fmap->pfd, FADE_SUB_OBJ);
	//ファクトリー対戦部屋の終了動作関数で、FADE_MAIN_BG_EX3の削除をしています(07.11.27)(未使用)

	PaletteFadeFree(fmap->pfd);

	//BGL開放
	sys_FreeMemoryEz(fmap->bgl);

	TCB_Delete(fmap->objupdate_tcb);
	TCB_Delete(fmap->cmdjudgeupdate_tcb);
	TCB_Delete(fmap->update_tcb);
	TCB_Delete(fmap->vintr_tcb);
	
	Frontier_3D_Exit(fmap->g3Dman);

	StopTP();		//タッチパネルの終了

	sys_FreeMemoryEz(fmap);
	
	GX_SetVisibleWnd(GX_WNDMASK_NONE);
	GXS_SetVisibleWnd(GX_WNDMASK_NONE);
	
	sys_VBlankFuncChange( NULL, NULL );		// VBlankセット
	sys_HBlankIntrStop();	//HBlank割り込み停止

	sys_DeleteHeap(HEAPID_FRONTIERMAP);

	MsgPrintSkipFlagSet(MSG_SKIP_OFF);
	MsgPrintAutoFlagSet(MSG_AUTO_OFF);
	MsgPrintTouchPanelFlagSet(MSG_TP_OFF);

	WirelessIconEasyEnd();	//通信アイコン削除
	
	MI_CpuFill16((void*)HW_BG_PLTT, 0x7fff, 0x200);
	MI_CpuFill16((void*)HW_OBJ_PLTT, 0x7fff, 0x200);
	MI_CpuFill16((void*)HW_DB_BG_PLTT, 0x7fff, 0x200);
	MI_CpuFill16((void*)HW_DB_OBJ_PLTT, 0x7fff, 0x200);

	G2_BlendNone();
	G2S_BlendNone();

//	Overlay_UnloadID(FS_OVERLAY_ID(frontier_common));
}

//--------------------------------------------------------------
/**
 * @brief   別画面移行時、復帰用に画面情報退避処理
 *
 * @param   fmap		
 */
//--------------------------------------------------------------
void FrontierMap_Push(FMAP_PTR fmap)
{
	int i;

	//リソース退避
	{
		//セット時から退避用ワークの値が変わる事はないので特にする事はなし
	}
	
	//アクター退避
	{
		FSS_ACTOR_WORK *fss_actor;
		
		for(i = 0; i < FIELD_OBJ_MAX; i++){
			fss_actor = Frontier_ActorWorkPtrGet(fmap->fmain, i);
			if(fss_actor->objwk != NULL){
				//変更の可能性のあるものだけ再退避
				fss_actor->param.way = WF2DMAP_OBJWkDataGet(fss_actor->objwk, WF2DMAP_OBJPM_WAY);
				fss_actor->param.status = WF2DMAP_OBJWkDataGet(fss_actor->objwk, WF2DMAP_OBJPM_ST);
				fss_actor->param.x = WF2DMAP_OBJWkDataGet(fss_actor->objwk, WF2DMAP_OBJPM_X);
				fss_actor->param.y = WF2DMAP_OBJWkDataGet(fss_actor->objwk, WF2DMAP_OBJPM_Y);
				
				fss_actor->param.visible = WF2DMAP_OBJDrawWkDrawFlagGet(fss_actor->drawwk);
			}
		}
	}

	//配置物アクター退避
	FMap_ArticleActPush(fmap);
}

//--------------------------------------------------------------
/**
 * @brief   別画面から戻ってきた時の画面情報復帰処理
 *
 * @param   fmap		
 */
//--------------------------------------------------------------
void FrontierMap_Pop(FMAP_PTR fmap)
{
	int i;
	
	//リソース再登録
	{
		FSS_CHAR_RESOURCE_DATA *res_manage;
		
		res_manage = Frontier_CharResourceBufferGet(fmap->fmain);
		for(i = 0; i < FIELD_OBJ_RESOURCE_MAX; i++){
			if(res_manage[i].charid != OBJCODEMAX){
				//FMap_CharResourceSet(fmap, &res_manage[i]);
				WF2DMAP_OBJDrawSysResSet(fmap->drawsys, res_manage[i].charid,
					res_manage[i].movetype, HEAPID_FRONTIERMAP);
//					NNS_G2D_VRAM_TYPE_2DMAIN, res_manage[i].movetype, HEAPID_FRONTIERMAP);[tomoya変更]	
			}
		}
	}
	
	//アクター再登録
	{
		FSS_ACTOR_WORK *fss_actor;
		FSS_CHAR_ACTOR_DATA act_data;
		
		for(i = 0; i < FIELD_OBJ_MAX; i++){
			fss_actor = Frontier_ActorWorkPtrGet(fmap->fmain, i);
			if(fss_actor->param.playid != FIELD_PLAYID_NULL){
				OS_TPrintf("playid = %d, charid = %d\n", 
					fss_actor->param.playid, fss_actor->param.charid);
				Frontier_ActWork_to_ActData(fss_actor, &act_data);
				FMap_ActorSet(fmap, &act_data, i);
			}
		}
	}

	//配置物アクター再登録
	FMap_ArticleActPop(fmap);
}

//--------------------------------------------------------------
/**
 * @brief	VBLANK関数
 *
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void FrontierMapVBlank(void *work)
{
	FMAP_PTR fmap = work;

	DoVramTransferManager();	// Vram転送マネージャー実行
	CATS_RenderOamTrans();
	PaletteFadeTrans(fmap->pfd);
	
	GF_BGL_VBlankFunc(fmap->bgl);
	
	OS_SetIrqCheckFlag( OS_IE_V_BLANK);
}

//--------------------------------------------------------------
/**
 * @brief	HBLANK関数
 *
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void FrontierMapHBlank(void *work)
{
	FMAP_PTR fmap = work;

}

//--------------------------------------------------------------
/**
 * @brief   VBlankタスク関数(1/60で回ります)
 *
 * @param   tcb		TCBへのポインタ
 * @param   work	結果発表管理ワークへのポインタ
 */
//--------------------------------------------------------------
static void VBlankTCB_IntrTask(TCB_PTR tcb, void *work)
{
	FMAP_PTR fmap = work;
	
}

//--------------------------------------------------------------
/**
 * @brief   2Dモジュール：オブジェクト動作TCB
 *
 * @param   tcb			TCBへのポインタ
 * @param   work		fmap
 *
 * 各オブジェクトの動作関数の実行順序をWF2DMAP_OBJSysMainとコマンド判断の間に
 * TCBで実行出来るようにするため、独立したTCBにしています
 */
//--------------------------------------------------------------
static void FrontierMapOBJSysUpdate(TCB_PTR tcb, void *work)
{
	FMAP_PTR fmap = work;
	
	//オブジェクト動作メイン
	WF2DMAP_OBJSysMain(fmap->objsys);
}

//--------------------------------------------------------------
/**
 * @brief   2Dモジュール：コマンド判断＆実行TCB
 *
 * @param   tcb			TCBへのポインタ
 * @param   work		fmap
 *
 * 各オブジェクトの動作関数の実行順序をWF2DMAP_OBJSysMainとコマンド判断の間に
 * TCBで実行出来るようにするため、独立したTCBにしています
 */
//--------------------------------------------------------------
static void FrontierMapOBJCmdJudgeUpdate(TCB_PTR tcb, void *work)
{
	FMAP_PTR fmap = work;
	
	//コマンド判断＆実行
	ObjCmdJudgeMove(fmap);
}

//--------------------------------------------------------------
/**
 * @brief   メインループの最後に行うシステム関連の更新処理
 *
 * @param   tcb			TCBへのポインタ
 * @param   work		fmap
 */
//--------------------------------------------------------------
static void FrontierMapUpdate(TCB_PTR tcb, void *work)
{
	FMAP_PTR fmap = work;
	
	{//2Dモジュール：BGスクロール
		FSS_ACTOR_WORK *player_actor;
		
		player_actor = Frontier_ActorWorkPtrGet(fmap->fmain, FIELD_OBJ_PLAYER);
		if(player_actor->objwk != NULL){
			WF2DMAP_SCRContSysMain(&fmap->scrollsys, player_actor->objwk);
		}
		FrontierMapScroll(fmap);
	}
	
	{//2Dモジュール：オブジェクト描画
		WF2DMAP_OBJDrawSysUpdata(fmap->drawsys);
	}

	//配置物アクターアニメ
	{
		u32 anmbit = fmap->article.anmbit;
		int i;
		
		for(i = 0; i < ACTWORK_MAX; i++){
			if(fmap->article.act[i] != NULL && (anmbit & 1)){
				CATS_ObjectUpdateCap(fmap->article.act[i]);
			}
			anmbit >>= 1;
		}
	}

	//アクター
	CATS_Draw(fmap->clactsys.crp);
	CATS_UpdateTransfer();

	//パーティクル
	FRParticle_Main();
	GF_G3_RequestSwapBuffers(GX_SORTMODE_MANUAL, GX_BUFFERMODE_Z);
}

//--------------------------------------------------------------
/**
 * @brief   マップスクロール処理
 *
 * @param   fmap		
 */
//--------------------------------------------------------------
static void FrontierMapScroll(FMAP_PTR fmap)
{
	int scroll_mode;
	FRONTIER_EX_PARAM *ex_param = Frontier_ExParamGet(fmap->fmain);
	
	scroll_mode = FSS_SceneParamGet(ex_param->scene_id, FSS_SCENE_DATA_SCROLL_MODE);
	switch(scroll_mode){
	case SCROLL_MODE_NORMAL:
	default:
		//スクロール表示処理
		if(fmap->scrdrawsys != NULL){
			WF2DMAP_SCRDrawSysMain(fmap->scrdrawsys, &fmap->scrollsys);
		}
		if(fmap->multi_scrdrawsys != NULL
			 && FSS_SceneParamGet(ex_param->scene_id, FSS_SCENE_DATA_MULTI_SCROLL_MODE) == TRUE){
			WF2DMAP_SCRDrawSysMain(fmap->multi_scrdrawsys, &fmap->scrollsys);
		}
		break;
	case SCROLL_MODE_EASY:
		FrontierMapScroll_Easy(fmap);
		break;
	}
}

//--------------------------------------------------------------
/**
 * @brief   マップ簡易スクロール処理
 *
 * @param   fmap		
 */
//--------------------------------------------------------------
static void FrontierMapScroll_Easy(FMAP_PTR fmap)
{
	s16 scr_x;
	s16 scr_y;
	FRONTIER_EX_PARAM *ex_param = Frontier_ExParamGet(fmap->fmain);
	
	// スクロール座標からスクロール座標を各表示システムに設定
	scr_y = WF2DMAP_SCROLLSysTopGet(&fmap->scrollsys) + fmap->shake_work.shake_y;
	scr_x = WF2DMAP_SCROLLSysLeftGet(&fmap->scrollsys) + fmap->shake_work.shake_x;

	// サーフェース
	CLACT_U_SetMainSurfaceMatrix(CATS_EasyRenderGet(fmap->clactsys.csp), 
		FX32_CONST(scr_x), FX32_CONST(scr_y));

	//BGLスクロール座標設定
	GF_BGL_ScrollReq(fmap->bgl, FRMAP_FRAME_MAP, GF_BGL_SCROLL_X_SET, scr_x);
	GF_BGL_ScrollReq(fmap->bgl, FRMAP_FRAME_MAP, GF_BGL_SCROLL_Y_SET, scr_y);
	if(FSS_SceneParamGet(ex_param->scene_id, FSS_SCENE_DATA_MULTI_MAP_SCREENID) != MULTI_BG_NULL
			&& FSS_SceneParamGet(ex_param->scene_id, FSS_SCENE_DATA_MULTI_SCROLL_MODE) == TRUE){
		GF_BGL_ScrollReq(fmap->bgl, FRMAP_FRAME_EFF, GF_BGL_SCROLL_X_SET, scr_x);
		GF_BGL_ScrollReq(fmap->bgl, FRMAP_FRAME_EFF, GF_BGL_SCROLL_Y_SET, scr_y);
	}
}

//--------------------------------------------------------------
/**
 * @brief   コマンドモジュール判断＆コマンド実行処理
 *
 * @param   fmap
 */
//--------------------------------------------------------------
static void ObjCmdJudgeMove(FMAP_PTR fmap)
{
	WF2DMAP_REQCMD req;
	WF2DMAP_ACTCMD act;
	BOOL result;

	//アクションコマンドキュー
	while(WF2DMAP_ACTCMDQSysCmdPop(fmap->actcmd_q, &act) == TRUE){
		//アクションコマンド設定
		WF2DMAP_OBJSysCmdSet(fmap->objsys, &act);
	}

	//リクエストコマンドキュー
	while(WF2DMAP_REQCMDQSysCmdPop(fmap->reqcmd_q, &req) == TRUE){
		result = WF2DMAP_JUDGESysCmdJudge(fmap->mapsys, fmap->objsys, &req, &act);
		if(result == TRUE){
			//アクションコマンド設定
			WF2DMAP_OBJSysCmdSet(fmap->objsys, &act);
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   Vramバンク設定を行う
 *
 * @param   bgl		BGLデータへのポインタ
 */
//--------------------------------------------------------------
static void FrontierMap_VramBankSet(GF_BGL_INI *bgl, int scene_id)
{
	int text_mode;
	
	text_mode = FSS_SceneParamGet(scene_id, FSS_SCENE_DATA_BG_MODE);
	
	GF_Disp_GX_VisibleControlInit();

	//VRAM設定
	{
		GF_BGL_DISPVRAM vramSetTable = {
			GX_VRAM_BG_256_BC,				// メイン2DエンジンのBG
			GX_VRAM_BGEXTPLTT_23_G,			// メイン2DエンジンのBG拡張パレット
			GX_VRAM_SUB_BG_32_H,			// サブ2DエンジンのBG
			GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット
			GX_VRAM_OBJ_64_E,				// メイン2DエンジンのOBJ
			GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット
			GX_VRAM_SUB_OBJ_16_I,			// サブ2DエンジンのOBJ
			GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット
			GX_VRAM_TEX_0_A,				// テクスチャイメージスロット
			GX_VRAM_TEXPLTT_0_F				// テクスチャパレットスロット
		};
		GF_Disp_SetBank( &vramSetTable );

		//VRAMクリア
		MI_CpuClear32((void*)HW_BG_VRAM, HW_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_BG_VRAM, HW_DB_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_OBJ_VRAM, HW_OBJ_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_OBJ_VRAM, HW_DB_OBJ_VRAM_SIZE);
	}

	// BG SYSTEM
	{
		GF_BGL_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_5, GX_BGMODE_0, GX_BG0_AS_3D,
		};
		BGsys_data.bgMode = text_mode;
		GF_BGL_InitBG( &BGsys_data );
	}

	//メイン画面フレーム設定
	{
		GF_BGL_BGCNT_HEADER TextBgCntDat[] = {
			///<FRAME1_M	ウィンドウ
			{
				0, 0, 0x0800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_01,
				FRMAP_BGPRI_WIN, 0, 0, FALSE
			},
			///<FRAME2_M	エフェクト
			{
				0, 0, 0x2000, 0, GF_BGL_SCRSIZ_512x512, GX_BG_COLORMODE_256,
				EFFBG_SCRBASE, EFFBG_CHARBASE, GX_BG_EXTPLTT_23,
				FRMAP_BGPRI_EFF, 0, 0, FALSE
			},
			///<FRAME3_M	マップ
			{
				0, 0, 0x2000, 0, GF_BGL_SCRSIZ_512x512, GX_BG_COLORMODE_256,
				MAPBG_SCRBASE, MAPBG_CHARBASE, GX_BG_EXTPLTT_23,
				FRMAP_BGPRI_MAP, 0, 0, FALSE
			},
		};
		u16 screen_size;
		
		if(text_mode == GX_BGMODE_0){
			TextBgCntDat[1].colorMode = GX_BG_COLORMODE_16;
			TextBgCntDat[2].colorMode = GX_BG_COLORMODE_16;
			TextBgCntDat[1].bgExtPltt = GX_BG_EXTPLTT_01;
			TextBgCntDat[2].bgExtPltt = GX_BG_EXTPLTT_01;
		}
		screen_size = FSS_SceneParamGet(scene_id, FSS_SCENE_DATA_SCREEN_SIZE);
		TextBgCntDat[2].screenSize = screen_size;
		if(FSS_SceneParamGet(scene_id, FSS_SCENE_DATA_MULTI_MAP_SCREENID) != MULTI_BG_NULL){
			//多重面使用の場合はマップとスクリーンサイズを合わせる
			TextBgCntDat[1].screenSize = screen_size;
		}
		
		if(text_mode == GX_BGMODE_0){
			GF_BGL_BGControlSet(bgl, FRMAP_FRAME_WIN, &TextBgCntDat[0], GF_BGL_MODE_TEXT );
			GF_BGL_ScrClear(bgl, FRMAP_FRAME_WIN );
			GF_BGL_ScrollSet(bgl, FRMAP_FRAME_WIN, GF_BGL_SCROLL_X_SET, 0);
			GF_BGL_ScrollSet(bgl, FRMAP_FRAME_WIN, GF_BGL_SCROLL_Y_SET, 0);
			GF_BGL_BGControlSet(bgl, FRMAP_FRAME_EFF, &TextBgCntDat[1], GF_BGL_MODE_TEXT );
			GF_BGL_ScrClear(bgl, FRMAP_FRAME_EFF );
			GF_BGL_ScrollSet(bgl, FRMAP_FRAME_EFF, GF_BGL_SCROLL_X_SET, 0);
			GF_BGL_ScrollSet(bgl, FRMAP_FRAME_EFF, GF_BGL_SCROLL_Y_SET, 0);
			GF_BGL_BGControlSet(bgl, FRMAP_FRAME_MAP, &TextBgCntDat[2], GF_BGL_MODE_TEXT );
			GF_BGL_ScrClear(bgl, FRMAP_FRAME_MAP );
			GF_BGL_ScrollSet(bgl, FRMAP_FRAME_MAP, GF_BGL_SCROLL_X_SET, 0);
			GF_BGL_ScrollSet(bgl, FRMAP_FRAME_MAP, GF_BGL_SCROLL_Y_SET, 0);
		}
		else{
			GF_BGL_BGControlSet(bgl, FRMAP_FRAME_WIN, &TextBgCntDat[0], GF_BGL_MODE_TEXT );
			GF_BGL_ScrClear(bgl, FRMAP_FRAME_WIN );
			GF_BGL_ScrollSet(bgl, FRMAP_FRAME_WIN, GF_BGL_SCROLL_X_SET, 0);
			GF_BGL_ScrollSet(bgl, FRMAP_FRAME_WIN, GF_BGL_SCROLL_Y_SET, 0);
			GF_BGL_BGControlSet(bgl, FRMAP_FRAME_EFF, &TextBgCntDat[1], GF_BGL_MODE_256X16 );
			GF_BGL_ScrClear(bgl, FRMAP_FRAME_EFF );
			GF_BGL_ScrollSet(bgl, FRMAP_FRAME_EFF, GF_BGL_SCROLL_X_SET, 0);
			GF_BGL_ScrollSet(bgl, FRMAP_FRAME_EFF, GF_BGL_SCROLL_Y_SET, 0);
			GF_BGL_BGControlSet(bgl, FRMAP_FRAME_MAP, &TextBgCntDat[2], GF_BGL_MODE_256X16 );
			GF_BGL_ScrClear(bgl, FRMAP_FRAME_MAP );
			GF_BGL_ScrollSet(bgl, FRMAP_FRAME_MAP, GF_BGL_SCROLL_X_SET, 0);
			GF_BGL_ScrollSet(bgl, FRMAP_FRAME_MAP, GF_BGL_SCROLL_Y_SET, 0);
		}
	
		G2_SetBG0Priority(FRMAP_3DBG_PRIORITY);
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
	}
	//サブ画面フレーム設定
	{
		GF_BGL_BGCNT_HEADER SubBgCntDat[] = {
			///<FRAME0_S	観客
			{
				0, 0, 0x0800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
				FRMAP_BGPRI_SUB_AUDIENCE, 0, 0, FALSE
			},
		};
		GF_BGL_BGControlSet(bgl, FRMAP_FRAME_SUB_AUDIENCE, &SubBgCntDat[0], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, FRMAP_FRAME_SUB_AUDIENCE );
		GF_BGL_ScrollSet(bgl, FRMAP_FRAME_SUB_AUDIENCE, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet(bgl, FRMAP_FRAME_SUB_AUDIENCE, GF_BGL_SCROLL_Y_SET, 0);
	}
}

//--------------------------------------------------------------
/**
 * @brief   システム系パレットの読み込み
 *
 * @param   fmap		
 */
//--------------------------------------------------------------
static void SystemPaletteLoad(FMAP_PTR fmap)
{
	//システムフォント
	PaletteWorkSet_Arc(fmap->pfd, ARC_FONT, NARC_font_system_ncrl, 
		HEAPID_FRONTIERMAP, FADE_MAIN_BG, 0x20, FFD_MENU_WIN_PAL * 16);
	//メッセージフォント
	PaletteWorkSet_Arc(fmap->pfd, ARC_FONT, NARC_font_talk_ncrl, 
		HEAPID_FRONTIERMAP, FADE_MAIN_BG, 0x20, FFD_MSG_WIN_PAL * 16);
	
	//会話ウィンドウ　キャラ＆パレット
	{
		FRONTIER_EX_PARAM *ex_param;
		
		ex_param = Frontier_ExParamGet(fmap->fmain);
		TalkWinGraphicSet(fmap->bgl, FRMAP_FRAME_WIN, FR_TALK_WIN_CGX_NUM, FR_TALK_WIN_PAL, 
			CONFIG_GetWindowType(ex_param->config), HEAPID_FRONTIERMAP);
		PaletteWorkSet_VramCopy(fmap->pfd, FADE_MAIN_BG, FR_TALK_WIN_PAL * 16, 0x20);
	}
	
	//システムウィンドウ　キャラ＆パレット
	MenuWinGraphicSet(fmap->bgl, FRMAP_FRAME_WIN, FR_MENU_WIN_CGX_NUM, FR_MENU_WIN_PAL,
		MENU_TYPE_SYSTEM, HEAPID_FRONTIERMAP);
	PaletteWorkSet_VramCopy(fmap->pfd, FADE_MAIN_BG, FR_MENU_WIN_PAL * 16, 0x20);
}

//--------------------------------------------------------------
/**
 * @brief   サブ画面のBGセット
 *
 * @param   fmap		
 */
//--------------------------------------------------------------
static void SubDisp_BGSet(FMAP_PTR fmap)
{
	ARCHANDLE* hdl;
		
	//とりあえずポケッチにある下画面を使いまわす
	hdl = ArchiveDataHandleOpen( ARC_FRONTIER_BG, HEAPID_FRONTIERMAP );

	ArcUtil_HDL_BgCharSet(	hdl, BATT_FRONTIER_NCGR_BIN, fmap->bgl, GF_BGL_FRAME0_S,
							0, 0, TRUE, HEAPID_FRONTIERMAP );

	ArcUtil_HDL_ScrnSet(hdl, BATT_FRONTIER_NSCR_BIN, fmap->bgl, GF_BGL_FRAME0_S,
						0, 0, TRUE, HEAPID_FRONTIERMAP );

//	ArcUtil_HDL_PalSet(	hdl, NARC_poketch_before_nclr, PALTYPE_SUB_BG,
//						0, 0x20, HEAPID_FRONTIERMAP );
	PaletteWorkSet_Arc(fmap->pfd, ARC_FRONTIER_BG, BATT_FRONTIER_NCLR, 
		HEAPID_FRONTIERMAP, FADE_SUB_BG, 0x20, 0 * 16);
		
	ArchiveDataHandleClose( hdl );
}

//--------------------------------------------------------------
/**
 * @brief   2Dマップシステムモジュール設定
 *
 * @param   fmap		
 * @param   scene_id	
 * @param   my_status	
 */
//--------------------------------------------------------------
static void MapSystem2D_ModuleSet(FMAP_PTR fmap, int scene_id, const MYSTATUS *my_status)
{
	//オブジェクト管理モジュール
	fmap->objsys = WF2DMAP_OBJSysInit(FIELD_OBJ_MAX, HEAPID_FRONTIERMAP);
	
	//マップデータ管理モジュール
	fmap->mapsys = WF2DMAP_MAPSysInit(MAP_GRID_MAX_X, MAP_GRID_MAX_Y, HEAPID_FRONTIERMAP);
	
	//スクロールデータ管理モジュール
	WF2DMAP_SCROLLSysDataInit(&fmap->scrollsys);
	
	//オブジェクトデータ描画モジュール
	fmap->drawsys = WF2DMAP_OBJDrawSysInit_Shadow(CATS_GetClactSetPtr(fmap->clactsys.crp) ,
		fmap->pfd, FIELD_OBJ_MAX, FrontierTool_MyObjCodeGet(my_status), WF2DC_C_MOVERUN, 
		NNS_G2D_VRAM_TYPE_2DMAIN, HEAPID_FRONTIERMAP);
//		fmap->pfd, FIELD_OBJ_MAX, HEAPID_FRONTIERMAP);	[tomoya変更]

	//スクロールデータ描画モジュール
	{
		WF2DMAP_SCRDRAWINIT init = {
			CLACT_U_EASYRENDER_SURFACE_MAIN,
			FRMAP_FRAME_MAP,
			GX_BG_COLORMODE_16,
			MAPBG_SCRBASE,
			MAPBG_CHARBASE,
			GX_BG_EXTPLTT_01,
			FRMAP_BGPRI_MAP,
			FALSE,
			0,
			0,
			TRUE,
		};
		int scroll_mode;
		
		init.arcid_scrn = FSS_SceneParamGet(scene_id, FSS_SCENE_DATA_MAP_ARCID);
		init.dataid_scrn = FSS_SceneParamGet(scene_id, FSS_SCENE_DATA_MAP_SCREENID);
		scroll_mode = FSS_SceneParamGet(scene_id, FSS_SCENE_DATA_SCROLL_MODE);
		if(scroll_mode == SCROLL_MODE_NORMAL){
			fmap->scrdrawsys = WF2DMAP_SCRDrawSysInit(CATS_EasyRenderGet(fmap->clactsys.csp), 
				fmap->bgl, &init, HEAPID_FRONTIERMAP);
		}

		//多重面スクロールデータ描画モジュール
		if(FSS_SceneParamGet(scene_id, FSS_SCENE_DATA_MULTI_MAP_SCREENID) != MULTI_BG_NULL){
			init.dataid_scrn = FSS_SceneParamGet(scene_id, FSS_SCENE_DATA_MULTI_MAP_SCREENID);
			init.bg_frame = FRMAP_FRAME_EFF;
			init.screenBase = EFFBG_SCRBASE;
			init.charbase = EFFBG_CHARBASE;
			init.priority = FRMAP_BGPRI_EFF;
			if(scroll_mode == SCROLL_MODE_NORMAL){
				fmap->multi_scrdrawsys = WF2DMAP_SCRDrawSysInit(
					CATS_EasyRenderGet(fmap->clactsys.csp), fmap->bgl, &init, HEAPID_FRONTIERMAP);
			}
		}
	}
	
	//キューモジュール
	fmap->reqcmd_q = WF2DMAP_REQCMDQSysInit(FMAP_REQCMDQ_NUM, HEAPID_FRONTIERMAP);
	fmap->actcmd_q = WF2DMAP_ACTCMDQSysInit(FMAP_ACTCMDQ_NUM, HEAPID_FRONTIERMAP);


	//-- データセット --//
	
	//マップデータ設定
	WF2DMAP_MAPSysDataSet(fmap->mapsys, DumyMap);
	
	//マップグラフィック読み込み(キャラ＆パレット)
	//キャラ
	{
		u32 arc_id;
		ARCHANDLE *hdl_bg;
		int text_mode = FSS_SceneParamGet(scene_id, FSS_SCENE_DATA_BG_MODE);
		
		arc_id = FSS_SceneParamGet(scene_id, FSS_SCENE_DATA_MAP_ARCID);
		hdl_bg = ArchiveDataHandleOpen(arc_id, HEAPID_FRONTIERMAP);
		
		ArcUtil_HDL_BgCharSet(hdl_bg, FSS_SceneParamGet(scene_id, FSS_SCENE_DATA_MAP_CHARID), 
			fmap->bgl, FRMAP_FRAME_MAP, 0, 0, TRUE, HEAPID_FRONTIERMAP);
		if(text_mode == GX_BGMODE_0){
			PaletteWorkSet_Arc(fmap->pfd, arc_id, 
				FSS_SceneParamGet(scene_id, FSS_SCENE_DATA_MAP_PLTTID), HEAPID_FRONTIERMAP, 
				FADE_MAIN_BG, FFD_FIELD_PAL_SIZE, FFD_FIELD_PAL_START * 16);
		}
		else{
			// パレットデータを拡張パレットVRAMに転送
			NNSG2dPaletteData *palData;
			void *p_work;
			
			p_work = ArcUtil_HDL_PalDataGet(hdl_bg, 
				FSS_SceneParamGet(scene_id, FSS_SCENE_DATA_MAP_PLTTID), 
				&palData, HEAPID_FRONTIERMAP);
			DC_FlushRange(palData->pRawData, palData->szByte);
			GX_BeginLoadBGExtPltt();	   // パレットデータの転送準備
			GX_LoadBGExtPltt(palData->pRawData, 0x6000, 0x2000);
		    GX_EndLoadBGExtPltt();         // パレットデータの転送完了
		    sys_FreeMemoryEz(p_work);
		}
		
		//画面外を真っ黒にする為、先頭のパレットに強制で黒をセット
		PaletteWork_Clear(fmap->pfd, FADE_MAIN_BG, FADEBUF_ALL, 0x0000, 0, 1);

		ArcUtil_HDL_ScrnSet(hdl_bg, FSS_SceneParamGet(scene_id, FSS_SCENE_DATA_MAP_SCREENID),
			fmap->bgl, FRMAP_FRAME_MAP, 0, 0, 1, HEAPID_FRONTIERMAP);

		//多重面
		if(FSS_SceneParamGet(scene_id, FSS_SCENE_DATA_MULTI_MAP_SCREENID) != MULTI_BG_NULL){
			ArcUtil_HDL_BgCharSet(hdl_bg, 
				FSS_SceneParamGet(scene_id, FSS_SCENE_DATA_MULTI_MAP_CHARID), 
				fmap->bgl, FRMAP_FRAME_EFF, 0, 0, TRUE, HEAPID_FRONTIERMAP);
			ArcUtil_HDL_ScrnSet(hdl_bg, 
				FSS_SceneParamGet(scene_id, FSS_SCENE_DATA_MULTI_MAP_SCREENID),
				fmap->bgl, FRMAP_FRAME_EFF, 0, 0, 1, HEAPID_FRONTIERMAP);
			if(text_mode == GX_BGMODE_0){
				;
			}
			else{
				// パレットデータを拡張パレットVRAMに転送
				NNSG2dPaletteData *palData;
				void *p_work;
				
				p_work = ArcUtil_HDL_PalDataGet(hdl_bg, 
					FSS_SceneParamGet(scene_id, FSS_SCENE_DATA_MULTI_MAP_PLTTID), 
					&palData, HEAPID_FRONTIERMAP);
				DC_FlushRange(palData->pRawData, palData->szByte);
				GX_BeginLoadBGExtPltt();	   // パレットデータの転送準備
				GX_LoadBGExtPltt(palData->pRawData, 0x4000, 0x2000);
			    GX_EndLoadBGExtPltt();         // パレットデータの転送完了
			    sys_FreeMemoryEz(p_work);
			}
		}

		GF_BGL_LoadScreenV_Req(fmap->bgl, FRMAP_FRAME_MAP);
		
		ArchiveDataHandleClose(hdl_bg);
	}
}

//--------------------------------------------------------------
/**
 * @brief   2Dマップシステムモジュール解放
 *
 * @param   fmap		
 */
//--------------------------------------------------------------
static void MapSystem2D_ModuleFree(FMAP_PTR fmap)
{
	int i;
	
	//オブジェクト管理モジュール
	{
		FSS_ACTOR_WORK *fss_actor;
		
		fss_actor = Frontier_ActorBufferGet(fmap->fmain);
		for(i = 0; i < FIELD_OBJ_MAX; i++){
			if(fss_actor[i].objwk != NULL){
				WF2DMAP_OBJWkDel(fss_actor[i].objwk);
				GF_ASSERT(fss_actor[i].anime_tcb == NULL);
			}
		}
	}
	WF2DMAP_OBJSysExit(fmap->objsys);
	
	//マップデータ管理モジュール
	WF2DMAP_MAPSysExit(fmap->mapsys);
	
	//スクロールデータ管理モジュール
	//これだけ実体なので解放の必要なし
	
	//オブジェクトデータ描画モジュール
//	for(i = 0; i < FIELD_OBJ_MAX; i++){
//		if(fmap->drawwk[i] != NULL){
//			WF2DMAP_OBJDrawWkDel(fmap->drawwk[i]);
//		}
//	}
//	WF2DMAP_OBJDrawSysResDel
	WF2DMAP_OBJDrawSysExit(fmap->drawsys);	//全部中で自動破棄
	
	//スクロールデータ描画モジュール
	if(fmap->scrdrawsys != NULL){
		WF2DMAP_SCRDrawSysExit(fmap->scrdrawsys);
	}
	if(fmap->multi_scrdrawsys != NULL){
		WF2DMAP_SCRDrawSysExit(fmap->multi_scrdrawsys);
	}

	//キューモジュール
	WF2DMAP_REQCMDQSysExit(fmap->reqcmd_q);
	WF2DMAP_ACTCMDQSysExit(fmap->actcmd_q);
}


//--------------------------------------------------------------
/**
 * @brief   フロンティア用3DBG初期化関数
 * 
 * @param   ヒープID
 */
//--------------------------------------------------------------
static GF_G3DMAN * Frontier_3D_Init(int heap_id)
{
	GF_G3DMAN *g3Dman;
	
	//simple_3DBGInit(HEAPID_FRONTIERMAP);
	
	//拡張BGとパレットVRAMを共用するため、パレット16K指定
	g3Dman = GF_G3DMAN_Init(heap_id, GF_G3DMAN_LNK, GF_G3DTEX_128K, 
		GF_G3DMAN_LNK, GF_G3DPLT_16K, Frontier_3D_SimpleSetUp);
	return g3Dman;
}

static void Frontier_3D_SimpleSetUp(void)
{
	// ３Ｄ使用面の設定(表示＆プライオリティー)
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
    G2_SetBG0Priority(1);

	// 各種描画モードの設定(シェード＆アンチエイリアス＆半透明)
    G3X_SetShading( GX_SHADING_TOON );
    G3X_AntiAlias( TRUE );
	G3X_AlphaTest( FALSE, 0 );	// アルファテスト　　オフ
	G3X_AlphaBlend( TRUE );		// アルファブレンド　オン
	G3X_EdgeMarking( FALSE );
	G3X_SetFog( FALSE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x8000, 0 );

	// クリアカラーの設定
    G3X_SetClearColor(GX_RGB(0,0,0),0,0x7fff,63,FALSE);	//color,alpha,depth,polygonID,fog

	// ビューポートの設定
    G3_ViewPort(0, 0, 255, 191);
}

//--------------------------------------------------------------
/**
 * @brief   フロンティア用3DBG終了処理
 *
 * @param   g3Dman		
 */
//--------------------------------------------------------------
static void Frontier_3D_Exit(GF_G3DMAN *g3Dman)
{
	//simple_3DBGExit();
	
	GF_G3D_Exit(g3Dman);
}

//----------------------------------------------------------------------------
/**
 *	@brief	セルアクターシステム初期化
 *
 *	@param	p_clact		セルアクターワーク
 *	@param	heapID		ヒープ
 *	@param	p_handle	アーカイブハンドル
 */
//-----------------------------------------------------------------------------
static void ActorSysInit(FMAP_PTR fmap)
{
	fmap->clactsys.csp=CATS_AllocMemory(HEAPID_FRONTIERMAP);
	CATS_SystemInit(fmap->clactsys.csp, &MapTcats,&MapCcmm, FMAP_OAM_PLTT_MAX);
	
	//通信アイコン用にキャラ＆パレット制限
	CLACT_U_WmIcon_SetReserveAreaCharManager(
		NNS_G2D_VRAM_TYPE_2DMAIN, GX_OBJVRAMMODE_CHAR_1D_128K);
	CLACT_U_WmIcon_SetReserveAreaPlttManager(NNS_G2D_VRAM_TYPE_2DMAIN);
	
	fmap->clactsys.crp=CATS_ResourceCreate(fmap->clactsys.csp);
	CATS_ClactSetInit(fmap->clactsys.csp, fmap->clactsys.crp, FMAP_ACTOR_MAX);
	CATS_ResourceManagerInit(fmap->clactsys.csp,fmap->clactsys.crp,&MapResourceList);
	CLACT_U_SetSubSurfaceMatrix(
		CATS_EasyRenderGet(fmap->clactsys.csp), 0, FMAP_SUB_ACTOR_DISTANCE);
}

//----------------------------------------------------------------------------
/**
 *	@brief	セルアクターシステム破棄
 *	
 *	@param	p_clact		セルアクターワーク
 */
//-----------------------------------------------------------------------------
static void ActorSysFree(FMAP_PTR fmap)
{
	//配置物アクター＆リソース削除
	{
		int i;
		
		//アクター
		for(i = 0; i < ACTWORK_MAX; i++){
			if(fmap->article.act[i] != NULL){
				FAct_ActorDel(fmap->article.act[i]);
			}
		}
		//リソース
		for(i = 0; i < ACTWORK_RESOURCE_MAX; i++){
			if(fmap->article.resource_id[i] != SCR_ACTWORK_RESOURCE_ID_NULL){
				FAct_ResourceFree(fmap->clactsys.crp, fmap->article.resource_id[i]);
			}
		}
	}
	
	//ポケモンアクター＆リソース削除
	{
		int i;
		
		for(i = 0; i < POKE_DISP_MANAGER_ID_NUM; i++){
			if(fmap->poke_cap[i] != NULL){
				//アクター削除
				CATS_ActorPointerDelete_S(fmap->poke_cap[i]);
				//リソース削除
				CATS_FreeResourceChar(fmap->clactsys.crp, POKE_DISP_MANAGER_ID_START + i);
				CATS_FreeResourcePltt(fmap->clactsys.crp, POKE_DISP_MANAGER_ID_START + i);
				CATS_FreeResourceCell(fmap->clactsys.crp, POKE_DISP_MANAGER_ID_START + i);
				CATS_FreeResourceCellAnm(fmap->clactsys.crp, POKE_DISP_MANAGER_ID_START + i);
			}
		}
	}

	CATS_ResourceDestructor_S(fmap->clactsys.csp,fmap->clactsys.crp);
	CATS_FreeMemory(fmap->clactsys.csp);
}

//--------------------------------------------------------------
/**
 * @brief   キャラクタリソース登録
 *
 * @param   fmap		
 * @param   res			登録リソースデータへのポインタ
 */
//--------------------------------------------------------------
void FMap_CharResourceSet(FMAP_PTR fmap, const FSS_CHAR_RESOURCE_DATA *res)
{
	FSS_CHAR_RESOURCE_DATA *manage;
	int i, setno;
	
	manage = Frontier_CharResourceBufferGet(fmap->fmain);
	
	//2重登録チェック
	for(i = 0; i < FIELD_OBJ_RESOURCE_MAX; i++){
		if(manage[i].charid == res->charid){
			OS_TPrintf("リソース登録済み\n");
			return;		//既に登録済み
		}
	}
	//空きチェック
	for(i = 0; i < FIELD_OBJ_RESOURCE_MAX; i++){
		if(manage[i].charid == OBJCODEMAX){
			break;
		}
	}
	GF_ASSERT(i != FIELD_OBJ_RESOURCE_MAX);	//空きなし

	setno = i;
	manage[setno] = *res;
	WF2DMAP_OBJDrawSysResSet(fmap->drawsys, res->charid, 
		res->movetype, HEAPID_FRONTIERMAP);
//		NNS_G2D_VRAM_TYPE_2DMAIN, res->movetype, HEAPID_FRONTIERMAP);	[tomoya変更]
}

//--------------------------------------------------------------
/**
 * @brief   キャラクタリソース解放
 *
 * @param   fmap		
 * @param   charid		キャラクタID
 */
//--------------------------------------------------------------
void FMap_CharResourceFree(FMAP_PTR fmap, int charid)
{
	int i;
	FSS_CHAR_RESOURCE_DATA *manage;

	manage = Frontier_CharResourceBufferGet(fmap->fmain);
	for(i = 0; i < FIELD_OBJ_RESOURCE_MAX; i++){
		if(manage[i].charid == charid){
			WF2DMAP_OBJDrawSysResDel(fmap->drawsys, charid);
			manage[i].charid = OBJCODEMAX;
			return;
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   アクター登録
 *
 * @param   fmap		
 * @param   res		 登録アクターデータへのポインタ
 * @param   pos_auto FMAP_ACT_POS_AUTO:空き領域を検索して登録。
 * 					 FMAP_ACT_POS_AUTO以外:pos_autoで渡されている配列番号に登録(自機登録時とPOP用)
 *
 * @retval  生成されたオブジェクトワークへのポインタ
 */
//--------------------------------------------------------------
WF2DMAP_OBJWK * FMap_ActorSet(FMAP_PTR fmap, const FSS_CHAR_ACTOR_DATA *res, int pos_auto)
{
	FSS_ACTOR_WORK *fss_actor;
	int i, setno;
	WF2DMAP_OBJDATA objdata;
	WF2DMAP_OBJWK *objwk;
	WF2DMAP_OBJDRAWWK *drawwk;
	
	fss_actor = Frontier_ActorBufferGet(fmap->fmain);
	
	if(pos_auto == FMAP_ACT_POS_AUTO){
		//空きチェック
		for(i = 0; i < FIELD_OBJ_MAX; i++){
			if(fss_actor[i].objwk == NULL){
				break;
			}
		}
		GF_ASSERT(i != FIELD_OBJ_MAX);	//空きなし
		setno = i;
	}
	else{
		setno = pos_auto;
	}
	
	objdata.x = res->x;
	objdata.y = res->y;
	objdata.playid = res->playid;
	objdata.status = res->status;
	objdata.way = res->way;
	objdata.charaid = res->charid;
	
	objwk = WF2DMAP_OBJWkNew(fmap->objsys, &objdata);
	drawwk = WF2DMAP_OBJDrawWkNew(fmap->drawsys, objwk, FALSE, HEAPID_FRONTIERMAP);
	WF2DMAP_OBJDrawWkDrawFlagSet(drawwk, res->visible);
	
	Frontier_ActWorkDataSet(fmap->fmain, setno, objwk, drawwk, res);
	
	return objwk;
}

//--------------------------------------------------------------
/**
 * @brief   アクター削除
 *
 * @param   fmap		
 * @param   objwk		オブジェクトワークのポインタ
 */
//--------------------------------------------------------------
void FMap_ActorFree(FMAP_PTR fmap, WF2DMAP_OBJWK *objwk)
{
	int i;
	FSS_ACTOR_WORK *fss_actor;

	fss_actor = Frontier_ActorBufferGet(fmap->fmain);
	for(i = 0; i < FIELD_OBJ_MAX; i++){
		if(fss_actor[i].objwk == objwk){
			WF2DMAP_OBJWkDel(fss_actor[i].objwk);
			WF2DMAP_OBJDrawWkDel(fss_actor[i].drawwk);
			
			GF_ASSERT(fss_actor[i].anime_tcb == NULL);
			MI_CpuClear8(&fss_actor[i], sizeof(FSS_ACTOR_WORK));
			fss_actor[i].param.playid = FIELD_PLAYID_NULL;
			return;
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   playidからOBJWK、DRAWWKのポインタを取得する
 *
 * @param   fmap		
 * @param   playid		プレイヤー識別ID
 * @param   objwk		OBJWK代入先(不要な場合はNULL)
 * @param   drawwk		DRAWWK代入先(不要な場合はNULL)
 */
//--------------------------------------------------------------
void FMap_OBJWkGet(FMAP_PTR fmap, u16 playid, WF2DMAP_OBJWK **objwk, WF2DMAP_OBJDRAWWK **drawwk)
{
	int i;
	FSS_ACTOR_WORK *fss_actor;

	fss_actor = Frontier_ActorBufferGet(fmap->fmain);
	for(i = 0; i < FIELD_OBJ_MAX; i++){
		if(fss_actor[i].param.playid == playid){
			if(objwk != NULL){
				*objwk = fss_actor[i].objwk;
			}
			if(drawwk != NULL){
				*drawwk = fss_actor[i].drawwk;
			}
			return;
		}
	}
	
	OS_TPrintf("見つからないplayid = %d\n", playid);
	GF_ASSERT(0);
}

//==============================================================================
//	配置物アクター
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   スクリプトで生成したアクターのリソースIDを管理領域に記憶する
 *
 * @param   fmap			
 * @param   act_id		アクターID
 *
 * IDの管理だけでリソースのセットは行わないことに注意
 */
//--------------------------------------------------------------
void FMap_ArticleActResourceIDSet(FMAP_PTR fmap, u16 act_id)
{
	int i;
	
	for(i = 0; i < ACTWORK_RESOURCE_MAX; i++){
		if(fmap->article.resource_id[i] == SCR_ACTWORK_RESOURCE_ID_NULL){
			fmap->article.resource_id[i] = act_id;
			return;
		}
	}
	
	GF_ASSERT(0);	//ID管理領域の空きがない
}

//--------------------------------------------------------------
/**
 * @brief   スクリプトで生成したアクターのリソースIDを管理領域から削除する
 *
 * @param   fmap		
 * @param   act_id		アクターID
 *
 * IDの削除だけでリソース自体は削除しないことに注意
 */
//--------------------------------------------------------------
void FMap_ArticleActResourceIDDel(FMAP_PTR fmap, u16 act_id)
{
	int i;
	
	for(i = 0; i < ACTWORK_RESOURCE_MAX; i++){
		if(fmap->article.resource_id[i] == act_id){
			fmap->article.resource_id[i] = SCR_ACTWORK_RESOURCE_ID_NULL;
			return;
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   スクリプトシステムで管理するアクターを生成する
 *
 * @param   fmap			
 * @param   work_no		ワーク番号(ACTWORK_?)
 * @param   cap			アクターポインタ
 */
//--------------------------------------------------------------
CATS_ACT_PTR FMap_ArticleActCreate(FMAP_PTR fmap, u16 work_no, u16 act_id)
{
	CATS_ACT_PTR cap;
	
	GF_ASSERT(work_no < ACTWORK_MAX);
	GF_ASSERT(fmap->article.act[work_no] == NULL);
	
	cap = FAct_ActorSet(fmap->clactsys.csp, fmap->clactsys.crp, act_id);
	fmap->article.act[work_no] = cap;
	fmap->article.act_id[work_no] = act_id;
	
	FMap_ArticleActAnmBitSet(fmap, work_no, OFF);
	return cap;
}

//--------------------------------------------------------------
/**
 * @brief   スクリプトシステムで管理するアクターを削除する
 *
 * @param   fmap		
 * @param   work_no		ワーク番号(ACTWORK_?)
 */
//--------------------------------------------------------------
void FMap_ArticleActDel(FMAP_PTR fmap, u16 work_no)
{
	GF_ASSERT(work_no < ACTWORK_MAX);
	GF_ASSERT(fmap->article.act[work_no] != NULL);
	
	FAct_ActorDel(fmap->article.act[work_no]);
	fmap->article.act[work_no] = NULL;
}

//--------------------------------------------------------------
/**
 * @brief   スクリプトシステムで管理しているアクターポインタを取得する
 *
 * @param   fmap		
 * @param   work_no		ワーク番号(ACTWORK_?)
 *
 * @retval  アクターポインタ
 */
//--------------------------------------------------------------
CATS_ACT_PTR FMap_ArticleActGet(FMAP_PTR fmap, u16 work_no)
{
	return fmap->article.act[work_no];
}

//--------------------------------------------------------------
/**
 * @brief   スクリプトシステムで管理するアクターのアニメ動作設定
 *
 * @param   work_no			ワーク番号(ACTWORK_?)
 * @param   anm_start		ON:アニメさせる。　OFF：させない
 */
//--------------------------------------------------------------
void FMap_ArticleActAnmBitSet(FMAP_PTR fmap, u16 work_no, int anm_start)
{
	if(anm_start == ON){
		fmap->article.anmbit |= 1 << work_no;
	}
	else{
		fmap->article.anmbit &= 0xffffffff ^ (1 << work_no);
	}
}

//--------------------------------------------------------------
/**
 * @brief   スクリプトシステムで管理するアクターのアニメ動作設定を取得する
 *
 * @param   work_no			ワーク番号(ACTWORK_?)
 * 
 * @retval  1=アニメ動作有効。　0=アニメ動作設定がされていない
 */
//--------------------------------------------------------------
u32 FMap_ArticleActAnmBitGet(FMAP_PTR fmap, u16 work_no)
{
	return (fmap->article.anmbit >> work_no) & 1;
}

//--------------------------------------------------------------
/**
 * @brief   スクリプトで生成したアクターを退避
 *
 * @param   fmap		
 */
//--------------------------------------------------------------
static void FMap_ArticleActPush(FMAP_PTR fmap)
{
	int i;
	PUSH_ARTICLE_WORK *push_buf;
	FMAP_ARTICLE_ACT *article;
	
	push_buf = Frontier_ArticlePushBufferGet(fmap->fmain);
	article = &fmap->article;
	
	//リソースID退避
	for(i = 0; i < ACTWORK_RESOURCE_MAX; i++){
		if(article->resource_id[i] != SCR_ACTWORK_RESOURCE_ID_NULL){
			push_buf->resource_id[i] = article->resource_id[i];
			i++;
		}
	}
	
	//アクターパラメータ退避
	i = 0;
	for(i = 0; i < ACTWORK_MAX; i++){
		if(article->act[i] != NULL){
			push_buf->act_param[i].anm_seq = CATS_ObjectAnimeSeqGetCap(article->act[i]);
			push_buf->act_param[i].anm_frame = CATS_ObjectAnimeFrameGetCap(article->act[i]);
			push_buf->act_param[i].anm_stop = FMap_ArticleActAnmBitGet(fmap, i);
			push_buf->act_param[i].visible = CATS_ObjectEnableGetCap(article->act[i]);
			push_buf->act_param[i].act_id = article->act_id[i];
			CATS_ObjectPosGetCap(article->act[i], 
				&push_buf->act_param[i].x, &push_buf->act_param[i].y);
			
			push_buf->act_param[i].occ = TRUE;
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   スクリプトで生成したアクターを復帰
 *
 * @param   fmap		
 */
//--------------------------------------------------------------
static void FMap_ArticleActPop(FMAP_PTR fmap)
{
	int i;
	ARCHANDLE *hdl_obj;
	PUSH_ARTICLE_WORK *push_buf;
	CATS_ACT_PTR cap;
	
	push_buf = Frontier_ArticlePushBufferGet(fmap->fmain);

	hdl_obj = ArchiveDataHandleOpen(ARC_FRONTIER_OBJ, HEAPID_FRONTIERMAP);
	
	//リソース復帰
	for(i = 0; i < ACTWORK_RESOURCE_MAX; i++){
		if(push_buf->resource_id[i] != SCR_ACTWORK_RESOURCE_ID_NULL){
			FAct_ResourceLoad(fmap->clactsys.csp, fmap->clactsys.crp, 
				hdl_obj, fmap->pfd, push_buf->resource_id[i]);
			FMap_ArticleActResourceIDSet(fmap, push_buf->resource_id[i]);
		}
	}
	
	//アクター復帰
	for(i = 0; i < ACTWORK_MAX; i++){
		if(push_buf->act_param[i].occ == TRUE){
			cap = FMap_ArticleActCreate(fmap, i, push_buf->act_param[i].act_id);
			CATS_ObjectPosSetCap(cap, push_buf->act_param[i].x, push_buf->act_param[i].y);
			CATS_ObjectEnableCap(cap, push_buf->act_param[i].visible);
			FMap_ArticleActAnmBitSet(fmap, i, push_buf->act_param[i].anm_stop);
			
			CATS_ObjectAnimeSeqSetCap(cap, push_buf->act_param[i].anm_seq);
			CATS_ObjectAnimeFrameSetCap(cap, push_buf->act_param[i].anm_frame);
		}
	}
	
	ArchiveDataHandleClose(hdl_obj);

	//復帰が完了したので退避バッファを初期化
	Frontier_ArticlePushBufferInit(fmap->fmain);
}

//--------------------------------------------------------------
/**
 * @brief   スクロール座標取得
 *
 * @param   fmap		
 * @param   ret_x		X座標代入先
 * @param   ret_y		Y座標代入先
 */
//--------------------------------------------------------------
void FMap_ScrollPosGet(FMAP_PTR fmap, s16 *ret_x, s16 *ret_y)
{
	// スクロール座標からスクロール座標を各表示システムに設定
	*ret_y = WF2DMAP_SCROLLSysTopGet(&fmap->scrollsys);
	*ret_x = WF2DMAP_SCROLLSysLeftGet(&fmap->scrollsys);
	OS_TPrintf("surface scroll x = %d, y = %d\n", *ret_x, *ret_y);
}

//--------------------------------------------------------------
/**
 * @brief   FSS_ACTOR_WORKのデータを元にしてFSS_CHAR_ACTOR_DATAを作り出す
 *
 * @param   fss_actor		作成元データになるFSS_ACTOR_WORK
 * @param   act_data		データ出力先
 */
//--------------------------------------------------------------
static void Frontier_ActWork_to_ActData(FSS_ACTOR_WORK *fss_actor, FSS_CHAR_ACTOR_DATA *act_data)
{
	*act_data = fss_actor->param;
}

//--------------------------------------------------------------
/**
 * @brief   フロンティアアクターワークにデータをセットする
 *
 * @param   act_no		アクター番号
 * @param   objwk		OBJWK
 * @param   drawwk		DRAWWK
 * @param   res			
 */
//--------------------------------------------------------------
static void Frontier_ActWorkDataSet(FMAIN_PTR fmain, int act_no, WF2DMAP_OBJWK *objwk, WF2DMAP_OBJDRAWWK *drawwk, const FSS_CHAR_ACTOR_DATA *res)
{
	FSS_ACTOR_WORK *fss_actor;
	
	fss_actor = Frontier_ActorWorkPtrGet(fmain, act_no);

//POP時のmoveを消さないようにする為、全体クリアしない。
//ActorFree時はClearしているので、ここでクリアしない事は問題ない。
//	MI_CpuClear8(fss_actor, sizeof(FSS_ACTOR_WORK));
	
	fss_actor->objwk = objwk;
	fss_actor->drawwk = drawwk;
	fss_actor->param = *res;
}

//--------------------------------------------------------------
/**
 * @brief   playidからフロンティアアクターワークポインタを取得する
 *
 * @param   fmain		
 * @param   playid		
 *
 * @retval  フロンティアアクターワーク
 */
//--------------------------------------------------------------
FSS_ACTOR_WORK * Frontier_ActorWorkSearch(FMAIN_PTR fmain, int playid)
{
	int i;
	FSS_ACTOR_WORK *fss_actor;
	
	fss_actor = Frontier_ActorBufferGet(fmain);
	for(i = 0; i < FIELD_OBJ_MAX; i++){
		if(fss_actor->objwk != NULL && fss_actor->param.playid == playid){
			return fss_actor;
		}
		fss_actor++;
	}
	GF_ASSERT(0);
	return NULL;
}
