//==============================================================================
/**
 * @file	ending.c
 * @brief	エンディング メイン制御
 * @author	matsuda
 * @date	2008.04.07(月)
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
#include "system/wipe.h"
#include "system/wipe_def.h"
#include "system/procsys.h"

#include "msgdata\msg.naix"

#include "demo\ending.h"
#include "graphic/ending.naix"
#include "ending_common.h"
#include "ending_setup.h"
#include "ending_tool.h"
#include "ending_list.h"


//==============================================================================
//	定数定義
//==============================================================================
///エンディングで使用するヒープサイズ
#define HEAPSIZE_ENDING_DEMO	(0x80000)


//--------------------------------------------------------------
//	CL_ACT用の定数定義
//--------------------------------------------------------------
///メイン	OAM管理領域・開始
#define ENDING_OAM_START_MAIN			(0)
///メイン	OAM管理領域・終了
#define ENDING_OAM_END_MAIN				(128)
///メイン	アフィン管理領域・開始
#define ENDING_OAM_AFFINE_START_MAIN		(0)
///メイン	アフィン管理領域・終了
#define ENDING_OAM_AFFINE_END_MAIN		(32)
///サブ	OAM管理領域・開始
#define ENDING_OAM_START_SUB				(0)
///サブ	OAM管理領域・終了
#define ENDING_OAM_END_SUB				(128)
///サブ アフィン管理領域・開始
#define ENDING_OAM_AFFINE_START_SUB		(0)
///サブ	アフィン管理領域・終了
#define ENDING_OAM_AFFINE_END_SUB		(32)

///キャラマネージャ：キャラクタID管理数(上画面＋下画面)
#define ENDING_CHAR_MAX					(48 + 48)
///キャラマネージャ：メイン画面サイズ(byte単位)
#define ENDING_CHAR_VRAMSIZE_MAIN		(1024 * 0x40)	//64K
///キャラマネージャ：サブ画面サイズ(byte単位)
#define ENDING_CHAR_VRAMSIZE_SUB			(512 * 0x20)	//32K

///メイン画面＋サブ画面で使用するアクター総数
#define ENDING_ACTOR_MAX					(64 + 64)	//メイン画面 + サブ画面

///OBJで使用するパレット本数(上画面＋下画面)
#define ENDING_OAM_PLTT_MAX				(16 + 16)

///OAMリソース：キャラ登録最大数(メイン画面 + サブ画面)
#define ENDING_OAMRESOURCE_CHAR_MAX		(ENDING_CHAR_MAX)
///OAMリソース：パレット登録最大数(メイン画面 + サブ画面)
#define ENDING_OAMRESOURCE_PLTT_MAX		(ENDING_OAM_PLTT_MAX)
///OAMリソース：セル登録最大数
#define ENDING_OAMRESOURCE_CELL_MAX		(64)
///OAMリソース：セルアニメ登録最大数
#define ENDING_OAMRESOURCE_CELLANM_MAX	(64)
///OAMリソース：マルチセル登録最大数
#define ENDING_OAMRESOURCE_MCELL_MAX		(8)
///OAMリソース：マルチセルアニメ登録最大数
#define ENDING_OAMRESOURCE_MCELLANM_MAX	(8)



//--------------------------------------------------------------
//	カメラ設定
//--------------------------------------------------------------
#define FOOTPRINT_CAMERA_MODE			GF_CAMERA_PERSPECTIV	//(GF_CAMERA_ORTHO)

#define FOOTPRINT_CAMERA_PERSPWAY		(FX_GET_ROTA_NUM(22))
#define FOOTPRINT_CAMERA_NEAR			( FX32_ONE )
#define FOOTPRINT_CAMERA_FAR			( FX32_ONE * 900 )
#define FOOTPRINT_CAMERA_TX				( 0 )		/// target
#define FOOTPRINT_CAMERA_TY				( -FX32_ONE * 8 )
#define FOOTPRINT_CAMERA_TZ				( 0 )

///カメラの注視点までの距離
#define FOOTPRINT_CAMERA_DISTANCE		(0x7c000)	//(0x96 << FX32_SHIFT)

///カメラアングル
static const CAMERA_ANGLE EndingCameraAngle = {
	FX_GET_ROTA_NUM(0), FX_GET_ROTA_NUM(0), FX_GET_ROTA_NUM(0),
};

//==============================================================================
//	CLACT用データ
//==============================================================================
static	const TCATS_OAM_INIT EndingTcats = {
	ENDING_OAM_START_MAIN, ENDING_OAM_END_MAIN,
	ENDING_OAM_AFFINE_START_MAIN, ENDING_OAM_AFFINE_END_MAIN,
	ENDING_OAM_START_SUB, ENDING_OAM_END_SUB,
	ENDING_OAM_AFFINE_START_SUB, ENDING_OAM_AFFINE_END_SUB,
};

static	const TCATS_CHAR_MANAGER_MAKE EndingCcmm = {
	ENDING_CHAR_MAX,
	ENDING_CHAR_VRAMSIZE_MAIN,
	ENDING_CHAR_VRAMSIZE_SUB,
	GX_OBJVRAMMODE_CHAR_1D_128K,
	GX_OBJVRAMMODE_CHAR_1D_128K
};

static const TCATS_RESOURCE_NUM_LIST EndingResourceList = {
	ENDING_OAMRESOURCE_CHAR_MAX,
	ENDING_OAMRESOURCE_PLTT_MAX,
	ENDING_OAMRESOURCE_CELL_MAX,
	ENDING_OAMRESOURCE_CELLANM_MAX,
	ENDING_OAMRESOURCE_MCELL_MAX,
	ENDING_OAMRESOURCE_MCELLANM_MAX,
};


//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static void Ending_Update(TCB_PTR tcb, void *work);
static void VBlankFunc( void * work );
static void Ending_VramBankSet(GF_BGL_INI *bgl);
static void BgExit( GF_BGL_INI * ini );
static void Model3D_Update(ENDING_MAIN_WORK *emw);
static void Ending_CameraInit(ENDING_MAIN_WORK *emw);
static void Ending_CameraExit(ENDING_MAIN_WORK *emw);
static GF_G3DMAN * Ending_3D_Init(int heap_id);
static void EndingSimpleSetUp(void);
static void Ending_3D_Exit(GF_G3DMAN *g3Dman);
static void Debug_CameraMove(ENDING_MAIN_WORK *emw);
static void Ending_DefaultBGLoad(ENDING_MAIN_WORK *emw);
static void EndingSceneVramExit(ENDING_MAIN_WORK *emw);
static void FinalSceneVramExit(ENDING_MAIN_WORK *emw);
static void Ending_FinalVramBankSet(GF_BGL_INI *bgl);


//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   プロセス関数：初期化
 *
 * @param   proc		プロセスデータ
 * @param   seq			シーケンス
 *
 * @retval  処理状況
 */
//--------------------------------------------------------------
PROC_RESULT EndingProc_Init( PROC * proc, int * seq )
{
	ENDING_MAIN_WORK *emw;
	
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

	sys_CreateHeap(HEAPID_BASE_APP, HEAPID_ENDING_DEMO, HEAPSIZE_ENDING_DEMO);

	emw = PROC_AllocWork(proc, sizeof(ENDING_MAIN_WORK), HEAPID_ENDING_DEMO );
	MI_CpuClear8(emw, sizeof(ENDING_MAIN_WORK));
	emw->parent_work = PROC_GetParentWork( proc );
	
//	simple_3DBGInit(HEAPID_ENDING_DEMO);
	emw->g3Dman = Ending_3D_Init(HEAPID_ENDING_DEMO);

	//パレットフェードシステム作成
	emw->pfd = PaletteFadeInit(HEAPID_ENDING_DEMO);
	PaletteFadeWorkAllocSet(emw->pfd, FADE_MAIN_BG, 0x200, HEAPID_ENDING_DEMO);
	PaletteFadeWorkAllocSet(emw->pfd, FADE_SUB_BG, 0x200, HEAPID_ENDING_DEMO);
	PaletteFadeWorkAllocSet(emw->pfd, FADE_MAIN_OBJ, 0x200-0x40, HEAPID_ENDING_DEMO);	//通信アイコン-ローカライズ用
	PaletteFadeWorkAllocSet(emw->pfd, FADE_SUB_OBJ, 0x200, HEAPID_ENDING_DEMO);
	PaletteTrans_AutoSet(emw->pfd, TRUE);
	
	emw->bgl = GF_BGL_BglIniAlloc(HEAPID_ENDING_DEMO);

	initVramTransferManagerHeap(64, HEAPID_ENDING_DEMO);

	sys_KeyRepeatSpeedSet( SYS_KEYREPEAT_SPEED_DEF, SYS_KEYREPEAT_WAIT_DEF );

	//VRAM割り当て設定
	Ending_VramBankSet(emw->bgl);

	// タッチパネルシステム初期化
	InitTPSystem();
	InitTPNoBuff(4);

	//メッセージマネージャ作成
	emw->msgman = MSGMAN_Create( 
		MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_stafflist_dat, HEAPID_ENDING_DEMO );

	//ファイルハンドル開きっぱなしにする
	emw->ending_hdl = ArchiveDataHandleOpen(ARC_ENDING, HEAPID_ENDING_DEMO);

	//カメラ作成
	Ending_CameraInit(emw);

	//アクターシステム作成
	emw->csp=CATS_AllocMemory(HEAPID_ENDING_DEMO);
	CATS_SystemInit(emw->csp,&EndingTcats,&EndingCcmm,ENDING_OAM_PLTT_MAX);
	//通信アイコン用にキャラ＆パレット制限
	CLACT_U_WmIcon_SetReserveAreaCharManager(
		NNS_G2D_VRAM_TYPE_2DMAIN, GX_OBJVRAMMODE_CHAR_1D_128K);
	CLACT_U_WmIcon_SetReserveAreaPlttManager(NNS_G2D_VRAM_TYPE_2DMAIN);
	emw->crp=CATS_ResourceCreate(emw->csp);
	CATS_ClactSetInit(emw->csp, emw->crp, ENDING_ACTOR_MAX);
	CATS_ResourceManagerInit(emw->csp,emw->crp,&EndingResourceList);
	CLACT_U_SetSubSurfaceMatrix(CATS_EasyRenderGet(emw->csp), 0, ENDING_SUB_ACTOR_DISTANCE);

	//常駐グラフィックセット
	Ending_DefaultBGLoad(emw);

	//スタッフロール制御システム
	emw->list_work = PlEnding_ListWork_Create( 
		emw->bgl, -240, FRAME_M_STAFFROLL, SYSTEM_FONT_PAL, emw->msgman );
	emw->list_work_sub = PlEnding_ListWork_Create( 
		emw->bgl, -240-192-16, FRAME_S_STAFFROLL, SYSTEM_FONT_PAL, emw->msgman );

	// スタッフロールの可視範囲設定
//	G2_SetWnd0Position( 0, 94, 255, 192 );
	G2_SetWnd0Position( 0, 192-24, 255, 192 );
	G2_SetWnd0InsidePlane( GX_WND_PLANEMASK_ALL ^ GX_WND_PLANEMASK_BG1, TRUE );
	G2_SetWndOutsidePlane( GX_WND_PLANEMASK_ALL, TRUE );

	G2S_SetWnd0Position( 0, 0, 255, 24 );
	G2S_SetWnd0InsidePlane( GX_WND_PLANEMASK_ALL ^ GX_WND_PLANEMASK_BG1, TRUE );
	G2S_SetWndOutsidePlane( GX_WND_PLANEMASK_ALL, TRUE );

	GX_SetVisibleWnd(GX_WNDMASK_W0);
	GXS_SetVisibleWnd(GX_WNDMASK_W0);
	
	//メイン画面設定
	sys.disp3DSW = DISP_3D_TO_SUB;
	GF_Disp_DispSelect();
	GF_Disp_DispOn();
	GF_Disp_GX_VisibleControl(GX_PLANEMASK_OBJ, VISIBLE_ON);
	GF_Disp_GXS_VisibleControl(GX_PLANEMASK_OBJ, VISIBLE_ON);

	emw->update_tcb = TCB_Add(Ending_Update, emw, 60000);

	sys_VBlankFuncChange(VBlankFunc, emw);

	Snd_DataSetByScene( SND_SCENE_ENDING, SEQ_BLD_ENDING, 1 );	//エンディング曲再生

	//真っ暗にしておく
	SetBrightness(BRIGHTNESS_BLACK, ENDING_FADE_PLANE, MASK_DOUBLE_DISPLAY);
	WIPE_SetMstBrightness( WIPE_DISP_MAIN, 0 );
	WIPE_SetMstBrightness( WIPE_DISP_SUB, 0 );

	GF_BGL_VisibleSet( FRAME_M_STAFFROLL, VISIBLE_ON );
	GF_BGL_VisibleSet( FRAME_S_STAFFROLL, VISIBLE_ON );

#ifdef PM_DEBUG
//	emw->scene_no = 4;
//	emw->main_counter = ENDING_COUNTER_CAP_SCENE2_END;
#endif

	return PROC_RES_FINISH;
}

//--------------------------------------------------------------
/**
 * @brief   プロセス関数：メイン
 *
 * @param   proc		プロセスデータ
 * @param   seq			シーケンス
 *
 * @retval  処理状況
 */
//--------------------------------------------------------------
PROC_RESULT EndingProc_Main( PROC * proc, int * seq )
{
	ENDING_MAIN_WORK * emw  = PROC_GetWork( proc );
	
	enum{
		SKIPSEQ_NO_SKIP = 0,
		SKIPSEQ_START,
		SKIPSEQ_SKIP_END,
	};
	
	enum{
		SEQ_SCENE_LOAD = 0,
		SEQ_SCENE_MAIN,
		SEQ_OUT,
	};
	
	//スタッフロール制御
	if(emw->list_work != NULL){
		PlEnding_ListWork_Scroll( emw->list_work, 1 );
		emw->list_end_flag = PlEnding_ListWork_Scroll( emw->list_work_sub, 1 );
	}

	if(emw->parent_work->clearFlag && (sys.trg & PAD_BUTTON_START)){
		if(emw->skip_seq == SKIPSEQ_NO_SKIP && emw->scene_no < ENDING_SCENE_COPYRIGHT){
			WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
				WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_ENDING_DEMO );
			emw->skip_seq = SKIPSEQ_START;
		}
	}
	//スキップ制御
	switch(emw->skip_seq){
	case SKIPSEQ_NO_SKIP:
		break;
	case SKIPSEQ_START:
		if( WIPE_SYS_EndCheck() ){
			if(emw->scene_init_ok == TRUE){
				EndingSceneSetup_Unload(emw, emw->scene_no);
				emw->scene_init_ok = FALSE;
			}
			//輝度変更リクエストがかかっていた場合は強制終了
			if(IsFinishedBrightnessChg(MASK_DOUBLE_DISPLAY) == FALSE){
				BrightnessChgReset(MASK_DOUBLE_DISPLAY);
			}
			//Vram設定変更(既に変更済みの場合は中ではじかれる)
			FinalSceneVramSetup(emw);
			
			emw->scene_no = ENDING_SCENE_COPYRIGHT;
			GF_Disp_GX_VisibleControlDirect(0);
			GF_Disp_GXS_VisibleControlDirect(0);
//			WIPE_SetMstBrightness( WIPE_DISP_MAIN, 0 );
//			WIPE_SetMstBrightness( WIPE_DISP_SUB, 0 );
			MI_CpuClear8(&emw->scene_work, sizeof(ENDING_SCENE_WORK));
			
			emw->skip_seq = SKIPSEQ_SKIP_END;
			*seq = SEQ_SCENE_LOAD;
		}
		break;
	}
	
	
	//メイン
	switch(*seq){
	case SEQ_SCENE_LOAD:
		if(emw->skip_seq == SKIPSEQ_NO_SKIP || emw->skip_seq == SKIPSEQ_SKIP_END){
			EndingSceneSetup_Load(emw, emw->scene_no);
			emw->scene_init_ok = TRUE;
			(*seq)++;
		}
		break;
	case SEQ_SCENE_MAIN:
		if(EndingSceneSetup_MainFunc(emw, emw->scene_no) == TRUE){
			EndingSceneSetup_Unload(emw, emw->scene_no);
			emw->scene_init_ok = FALSE;
			emw->scene_no++;
			if(emw->scene_no < ENDING_SCENE_MAX){
				(*seq) = SEQ_SCENE_LOAD;
			}
			else{
				(*seq) = SEQ_OUT;
			}
		}
		break;
	case SEQ_OUT:
		return PROC_RES_FINISH;
	}
	
#ifdef PM_DEBUG
#ifdef DEBUG_ENDING_CAMERA
	if(emw->debug_mode == 0){
		//デバッグカメラ移動
		Debug_CameraMove(emw);
	}
	else{
		//デバッグ配置物の配置間隔制御
		EndingTool_Debug_ModelLineArrangeSetting(emw, MODEL_LINE_0);
	}
	if(sys.trg & PAD_BUTTON_SELECT){
		emw->debug_mode ^= 1;
	}
#endif
#endif
	
	emw->main_counter++;
	if(emw->main_counter % 30 == 0){
		OS_TPrintf("main_counter = %d\n", emw->main_counter / 30);
	}
	return PROC_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   プロセス関数：終了
 *
 * @param   proc		プロセスデータ
 * @param   seq			シーケンス
 *
 * @retval  処理状況
 */
//--------------------------------------------------------------
PROC_RESULT EndingProc_End( PROC * proc, int * seq )
{
	ENDING_MAIN_WORK * emw  = PROC_GetWork( proc );

	TCB_Delete(emw->update_tcb);

	if(emw->list_work != NULL){
		EndingSceneVramExit(emw);
	}
	else{
		FinalSceneVramExit(emw);
	}

	// メッセージマネージャー・ワードセットマネージャー解放
	MSGMAN_Delete( emw->msgman );
	

	//アクターシステム削除
	CATS_ResourceDestructor_S(emw->csp,emw->crp);
	CATS_FreeMemory(emw->csp);

	//パレットフェードシステム削除
	PaletteFadeWorkAllocFree(emw->pfd, FADE_MAIN_BG);
	PaletteFadeWorkAllocFree(emw->pfd, FADE_SUB_BG);
	PaletteFadeWorkAllocFree(emw->pfd, FADE_MAIN_OBJ);
	PaletteFadeWorkAllocFree(emw->pfd, FADE_SUB_OBJ);
	PaletteFadeFree(emw->pfd);

	//カメラ削除
	Ending_CameraExit(emw);

	//ハンドル閉じる
	ArchiveDataHandleClose( emw->ending_hdl );

	sys_VBlankFuncChange( NULL, NULL );		// VBlankセット
	sys_HBlankIntrStop();	//HBlank割り込み停止

	//Vram転送マネージャー削除
	DellVramTransferManager();

	StopTP();		//タッチパネルの終了

	PROC_FreeWork( proc );				// PROCワーク開放
	sys_DeleteHeap( HEAPID_ENDING_DEMO );

	return PROC_RES_FINISH;
}


//==============================================================================
//	
//==============================================================================
static void Ending_Update(TCB_PTR tcb, void *work)
{
	ENDING_MAIN_WORK *emw = work;
	
	if(emw->draw_3d == TRUE){
		Model3D_Update(emw);
		GF_G3_RequestSwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_Z);
	}

	CATS_Draw(emw->crp);
	CATS_UpdateTransfer();
}

//--------------------------------------------------------------
/**
 * @brief   
 *
 * @param   emw		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
static void EndingSceneVramExit(ENDING_MAIN_WORK *emw)
{
	GF_ASSERT(emw->list_work != NULL);
	
	PlEnding_ListWork_Delete( emw->list_work_sub );
	PlEnding_ListWork_Delete( emw->list_work );
	emw->list_work_sub = NULL;
	emw->list_work = NULL;
	GF_BGL_VisibleSet( FRAME_M_STAFFROLL, VISIBLE_OFF );
	GF_BGL_VisibleSet( FRAME_S_STAFFROLL, VISIBLE_OFF );

	// BG_SYSTEM解放
	GF_BGL_BGControlExit( emw->bgl, FRAME_S_BG );
	GF_BGL_BGControlExit( emw->bgl, FRAME_S_STAFFROLL );
	GF_BGL_BGControlExit( emw->bgl, FRAME_S_EFF );
	GF_BGL_BGControlExit( emw->bgl, FRAME_M_CAPTURE );
	GF_BGL_BGControlExit( emw->bgl, FRAME_M_BG );
	GF_BGL_BGControlExit( emw->bgl, FRAME_M_STAFFROLL );
	sys_FreeMemoryEz( emw->bgl );

	//simple_3DBGExit();
	Ending_3D_Exit(emw->g3Dman);
}

//--------------------------------------------------------------
/**
 * @brief   フィナーレ用にVRAMバンク設定を変更
 *
 * @param   emw		
 */
//--------------------------------------------------------------
void FinalSceneVramSetup(ENDING_MAIN_WORK *emw)
{
	if(emw->list_work == NULL){
		return;	//既に変更されている
	}
	EndingSceneVramExit(emw);
	
	emw->bgl = GF_BGL_BglIniAlloc(HEAPID_ENDING_DEMO);
	Ending_FinalVramBankSet(emw->bgl);
}

//--------------------------------------------------------------
/**
 * @brief   
 *
 * @param   emw		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
static void FinalSceneVramExit(ENDING_MAIN_WORK *emw)
{
	GF_ASSERT(emw->list_work == NULL);
	
	// BG_SYSTEM解放
	GF_BGL_BGControlExit( emw->bgl, FRAME_FIN_M_BG );
	GF_BGL_BGControlExit( emw->bgl, FRAME_FIN_M_CAP_A );
	GF_BGL_BGControlExit( emw->bgl, FRAME_FIN_M_CAP_B );
	GF_BGL_BGControlExit( emw->bgl, FRAME_FIN_S_BG );
	sys_FreeMemoryEz( emw->bgl );
}

//--------------------------------------------------------------------------------------------
/**
 * VBlank関数
 *
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void VBlankFunc( void * work )
{
	ENDING_MAIN_WORK *emw = work;
	
	// セルアクターVram転送マネージャー実行
	DoVramTransferManager();

	// レンダラ共有OAMマネージャVram転送
	CATS_RenderOamTrans();
	PaletteFadeTrans(emw->pfd);
	
	GF_BGL_VBlankFunc(emw->bgl);

	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}

//--------------------------------------------------------------
/**
 * @brief   Vramバンク設定を行う
 *
 * @param   bgl		BGLデータへのポインタ
 */
//--------------------------------------------------------------
static void Ending_VramBankSet(GF_BGL_INI *bgl)
{
	GF_Disp_GX_VisibleControlInit();
	GF_Disp_GXS_VisibleControlInit();
	
	//VRAM設定
	{
		GF_BGL_DISPVRAM vramSetTable = {
			GX_VRAM_BG_128_B,				// メイン2DエンジンのBG
			GX_VRAM_BGEXTPLTT_23_G,			// メイン2DエンジンのBG拡張パレット

			GX_VRAM_BG_128_C,			// サブ2DエンジンのBG
			GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット

			GX_VRAM_OBJ_64_E,				// メイン2DエンジンのOBJ
			GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット

			GX_VRAM_SUB_OBJ_16_I,			// サブ2DエンジンのOBJ
			GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット

			GX_VRAM_TEX_0_A,				// テクスチャイメージスロット
			GX_VRAM_TEXPLTT_0_F			// テクスチャパレットスロット
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
			GX_DISPMODE_GRAPHICS, GX_BGMODE_3, GX_BGMODE_0, GX_BG0_AS_3D,
		};
		GF_BGL_InitBG( &BGsys_data );
	}

	//メイン画面フレーム設定
	{
		GF_BGL_BGCNT_HEADER TextBgCntDat[] = {
			///<FRAME_M_STAFFROLL	スタッフロール
			{
				0, 0, 0x0800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01,
				FRAME_BGPRI_M_STAFFROLL, 0, 0, FALSE
			},
			///<FRAME_M_BG			背景
			{
				0, 0, 0x2000, 0, GF_BGL_SCRSIZ_512x512, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000, GX_BG_EXTPLTT_01,
				FRAME_BGPRI_M_BG, 0, 0, FALSE
			},
			///<FRAME_M_CAPTURE		キャプチャ画像(拡張BG)
			{
				0, 0, 0, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_256,
				GX_BG_SCRBASE_0x3000, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_23,
				FRAME_BGPRI_M_CAPTURE, 0, 0, FALSE
			},
		};
		GF_BGL_BGControlSet(bgl, FRAME_M_STAFFROLL, &TextBgCntDat[0], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, FRAME_M_STAFFROLL );
		GF_BGL_ScrollSet(bgl, FRAME_M_STAFFROLL, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet(bgl, FRAME_M_STAFFROLL, GF_BGL_SCROLL_Y_SET, 0);

		GF_BGL_BGControlSet(bgl, FRAME_M_BG, &TextBgCntDat[1], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, FRAME_M_BG );
		GF_BGL_ScrollSet(bgl, FRAME_M_BG, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet(bgl, FRAME_M_BG, GF_BGL_SCROLL_Y_SET, 0);
		
		GF_BGL_BGControlSet(bgl, FRAME_M_CAPTURE, &TextBgCntDat[2], GF_BGL_MODE_256X16 );
		//GF_BGL_ScrClear(bgl, FRAME_M_CAPTURE );
		GF_BGL_ScrollSet(bgl, FRAME_M_CAPTURE, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet(bgl, FRAME_M_CAPTURE, GF_BGL_SCROLL_Y_SET, 0);

		//3D面
		G2_SetBG0Priority(FRAME_BGPRI_3D);
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
	}
	//サブ画面フレーム設定
	{
		GF_BGL_BGCNT_HEADER TextBgCntDat[] = {
			///<FRAME_S_STAFFROLL	スタッフロール
			{
				0, 0, 0x0800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01,
				FRAME_BGPRI_S_STAFFROLL, 0, 0, FALSE
			},
			///<FRAME_S_EFF			エフェクト(現状未使用)
			{
				0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000, GX_BG_EXTPLTT_01,
				FRAME_BGPRI_S_EFF, 0, 0, FALSE
			},
			///<FRAME_S_BG			背景
			{
				0, 0, 0x2000, 0, GF_BGL_SCRSIZ_512x512, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x14000, GX_BG_EXTPLTT_01,
				FRAME_BGPRI_S_BG, 0, 0, FALSE
			},
		};
		GF_BGL_BGControlSet(bgl, FRAME_S_STAFFROLL, &TextBgCntDat[0], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, FRAME_S_STAFFROLL );
		GF_BGL_ScrollSet(bgl, FRAME_S_STAFFROLL, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet(bgl, FRAME_S_STAFFROLL, GF_BGL_SCROLL_Y_SET, 0);
		
		GF_BGL_BGControlSet(bgl, FRAME_S_EFF, &TextBgCntDat[1], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, FRAME_S_EFF );
		GF_BGL_ScrollSet(bgl, FRAME_S_EFF, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet(bgl, FRAME_S_EFF, GF_BGL_SCROLL_Y_SET, 0);

		GF_BGL_BGControlSet(bgl, FRAME_S_BG, &TextBgCntDat[2], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, FRAME_S_BG );
		GF_BGL_ScrollSet(bgl, FRAME_S_BG, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet(bgl, FRAME_S_BG, GF_BGL_SCROLL_Y_SET, 0);
	}

	GF_BGL_ClearCharSet( FRAME_M_STAFFROLL, 32, 0, HEAPID_ENDING_DEMO );
	GF_BGL_ClearCharSet( FRAME_S_STAFFROLL, 32, 0, HEAPID_ENDING_DEMO );
	GF_BGL_ClearCharSet( FRAME_S_EFF, 32, 0, HEAPID_ENDING_DEMO );
	GF_BGL_ClearCharSet( FRAME_S_BG, 32, 0, HEAPID_ENDING_DEMO );
	
	GF_BGL_VisibleSet( FRAME_M_CAPTURE, VISIBLE_OFF );
	GF_BGL_VisibleSet( FRAME_S_EFF, VISIBLE_OFF );
}

//--------------------------------------------------------------
/**
 * @brief   Vramバンク設定を行う(フィナーレ用)
 *
 * @param   bgl		BGLデータへのポインタ
 */
//--------------------------------------------------------------
static void Ending_FinalVramBankSet(GF_BGL_INI *bgl)
{
	GF_Disp_GX_VisibleControlInit();
	GF_Disp_GXS_VisibleControlInit();
	
	//VRAM設定
	{
		GF_BGL_DISPVRAM vramSetTable = {
			GX_VRAM_BG_256_AB,				// メイン2DエンジンのBG
			GX_VRAM_BGEXTPLTT_23_G,			// メイン2DエンジンのBG拡張パレット

			GX_VRAM_BG_128_C,			// サブ2DエンジンのBG
			GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット

			GX_VRAM_OBJ_64_E,				// メイン2DエンジンのOBJ
			GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット

			GX_VRAM_SUB_OBJ_16_I,			// サブ2DエンジンのOBJ
			GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット

			GX_VRAM_TEX_NONE,				// テクスチャイメージスロット
			GX_VRAM_TEXPLTT_NONE			// テクスチャパレットスロット
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
			GX_DISPMODE_GRAPHICS, GX_BGMODE_5, GX_BGMODE_0, GX_BG0_AS_2D,
		};
		GF_BGL_InitBG( &BGsys_data );
	}

	//メイン画面フレーム設定
	{
		GF_BGL_BGCNT_HEADER TextBgCntDat[] = {
			///<FRAME_FIN_M_BG			背景
			{
				0, 0, 0x0800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x0c000, GX_BG_EXTPLTT_01,
				FRAME_BGPRI_M_BG, 0, 0, FALSE
			},
			///<FRAME_FIN_M_CAP_A		キャプチャ画像(拡張BG)
			{
				0, 0, 0, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_256,
				GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x20000, GX_BG_EXTPLTT_23,
				FRAME_BGPRI_FIN_M_CAPTURE_TOP, 0, 0, FALSE
			},
			///<FRAME_FIN_M_CAP_B		キャプチャ画像(拡張BG)
			{
				0, 0, 0, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_256,
				GX_BG_SCRBASE_0x2800, GX_BG_CHARBASE_0x30000, GX_BG_EXTPLTT_23,
				FRAME_BGPRI_FIN_M_CAPTURE_BOTTOM, 0, 0, FALSE
			},
		};
		GF_BGL_BGControlSet(bgl, FRAME_FIN_M_BG, &TextBgCntDat[0], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, FRAME_FIN_M_BG );
		GF_BGL_ScrollSet(bgl, FRAME_FIN_M_BG, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet(bgl, FRAME_FIN_M_BG, GF_BGL_SCROLL_Y_SET, 0);

		GF_BGL_BGControlSet(bgl, FRAME_FIN_M_CAP_A, &TextBgCntDat[1], GF_BGL_MODE_256X16 );
		GF_BGL_ScrClear(bgl, FRAME_FIN_M_CAP_A );
		GF_BGL_ScrollSet(bgl, FRAME_FIN_M_CAP_A, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet(bgl, FRAME_FIN_M_CAP_A, GF_BGL_SCROLL_Y_SET, 0);
		
		GF_BGL_BGControlSet(bgl, FRAME_FIN_M_CAP_B, &TextBgCntDat[2], GF_BGL_MODE_256X16 );
		GF_BGL_ScrClear(bgl, FRAME_FIN_M_CAP_B );
		GF_BGL_ScrollSet(bgl, FRAME_FIN_M_CAP_B, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet(bgl, FRAME_FIN_M_CAP_B, GF_BGL_SCROLL_Y_SET, 0);
	}
	//サブ画面フレーム設定
	{
		GF_BGL_BGCNT_HEADER TextBgCntDat[] = {
			///<FRAME_FIN_S_BG			背景
			{
				0, 0, 0x2000, 0, GF_BGL_SCRSIZ_512x512, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x14000, GX_BG_EXTPLTT_01,
				FRAME_BGPRI_S_BG, 0, 0, FALSE
			},
		};
		GF_BGL_BGControlSet(bgl, FRAME_FIN_S_BG, &TextBgCntDat[0], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, FRAME_FIN_S_BG );
		GF_BGL_ScrollSet(bgl, FRAME_FIN_S_BG, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet(bgl, FRAME_FIN_S_BG, GF_BGL_SCROLL_Y_SET, 0);
	}

	GF_BGL_ClearCharSet( FRAME_FIN_S_BG, 32, 0, HEAPID_ENDING_DEMO );
}

//--------------------------------------------------------------------------------------------
/**
 * BG解放
 *
 * @param	ini		BGLデータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BgExit( GF_BGL_INI * ini )
{
	GF_BGL_BGControlExit( ini, FRAME_S_BG );
	GF_BGL_BGControlExit( ini, FRAME_S_STAFFROLL );
	GF_BGL_BGControlExit( ini, FRAME_S_EFF );
	GF_BGL_BGControlExit( ini, FRAME_M_CAPTURE );
	GF_BGL_BGControlExit( ini, FRAME_M_BG );
	GF_BGL_BGControlExit( ini, FRAME_M_STAFFROLL );

}

//--------------------------------------------------------------
/**
 * @brief   全シーン共通で使用するグラフィックデータを設定
 *
 * @param   emw		
 */
//--------------------------------------------------------------
static void Ending_DefaultBGLoad(ENDING_MAIN_WORK *emw)
{
	//システムフォントパレット転送
	PaletteWorkSet_Arc(emw->pfd, ARC_ENDING, NARC_ending_font_NCLR, HEAPID_ENDING_DEMO, 
		FADE_MAIN_BG, 0x20, SYSTEM_FONT_PAL * 16);
	PaletteWorkSet_Arc(emw->pfd, ARC_ENDING, NARC_ending_font_NCLR, HEAPID_ENDING_DEMO, 
		FADE_SUB_BG, 0x20, SYSTEM_FONT_PAL * 16);
}

//--------------------------------------------------------------
/**
 * @brief   3Dモデル更新
 *
 * @param   emw		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
static void Model3D_Update(ENDING_MAIN_WORK *emw)
{
	VecFx32 scale_vec, trans;
	MtxFx33 rot;
	
	scale_vec.x = FX32_ONE;
	scale_vec.y = FX32_ONE;
	scale_vec.z = FX32_ONE;
	
	trans.x = 0;
	trans.y = 0;
	trans.z = 0;
	
	MTX_Identity33(&rot);

	//３Ｄ描画開始
	GF_G3X_Reset();
	
	GFC_AttachCamera(emw->camera);
	GFC_SetCameraView(FOOTPRINT_CAMERA_MODE, emw->camera); //射影設定
	GFC_CameraLookAt();

	// ライトとアンビエント
	switch(emw->scene_no){
	case ENDING_SCENE_MAP1:
		NNS_G3dGlbLightVector( 0, 1897, -3600, -466 );
		NNS_G3dGlbLightVector( 2, 0, 0, 4096 );
		NNS_G3dGlbLightColor( 0, GX_RGB( 28, 12, 6 ) );	//GX_RGB( 19,16,12 ) );
		NNS_G3dGlbLightColor( 2, GX_RGB( 0,0,0 ) );
		NNS_G3dGlbMaterialColorDiffAmb( GX_RGB( 15,15,15 ), GX_RGB( 11,12,12 ), FALSE );
		NNS_G3dGlbMaterialColorSpecEmi( GX_RGB( 17,17,17 ), GX_RGB( 8,8,7 ), FALSE );
		break;
	case ENDING_SCENE_MAP2:
		NNS_G3dGlbLightVector( 0, -2043, -3548, 110 );
		NNS_G3dGlbLightVector( 2, 0, 0, 4096 );
		NNS_G3dGlbLightColor( 0, GX_RGB( 22,22,20 ) );
		NNS_G3dGlbLightColor( 2, GX_RGB( 0,0,0 ) );
		NNS_G3dGlbMaterialColorDiffAmb( GX_RGB( 15,15,15 ), GX_RGB( 9,11,11 ), FALSE );
		NNS_G3dGlbMaterialColorSpecEmi( GX_RGB( 16,16,16 ), GX_RGB( 14,14,14 ), FALSE );
		break;
	case ENDING_SCENE_MAP3:
		NNS_G3dGlbLightVector( 0, 1897, -3600, -466 );
		NNS_G3dGlbLightVector( 2, 0, 0, 4096 );
		NNS_G3dGlbLightColor( 0, GX_RGB( 11,11,16 ) );
		NNS_G3dGlbLightColor( 2, GX_RGB( 0,0,0 ) );
		NNS_G3dGlbMaterialColorDiffAmb( GX_RGB( 14,14,16 ), GX_RGB( 10,10,10 ), FALSE );
		NNS_G3dGlbMaterialColorSpecEmi( GX_RGB( 14,14,16 ), GX_RGB( 8,8,11 ), FALSE );
		break;
	default:
		NNS_G3dGlbLightVector( 0, 0, -FX32_ONE, 0 );
		NNS_G3dGlbLightColor( 0, GX_RGB( 28,28,28 ) );
		NNS_G3dGlbMaterialColorDiffAmb( GX_RGB( 31,31,31 ), GX_RGB( 31,31,31 ), FALSE );
		NNS_G3dGlbMaterialColorSpecEmi( GX_RGB( 31,31,31 ), GX_RGB( 31,31,31 ), FALSE );
		break;
	}
	//常時ONのライト
	NNS_G3dGlbLightVector( 3, 0, -FX32_ONE, 0 );
	NNS_G3dGlbLightColor( 3, GX_RGB( 28,28,28 ) );
	
	// 位置設定
	NNS_G3dGlbSetBaseTrans(&trans);
	// 角度設定
	NNS_G3dGlbSetBaseRot(&rot);
	// スケール設定
	NNS_G3dGlbSetBaseScale(&scale_vec);

//	NNS_G3dGlbFlush();
	
	// 描画
	NNS_G3dGePushMtx();
	{
		int line, i;
		
		for(line = 0; line < MODEL_LINE_MAX; line++){
			for(i = 0; i < ENDING_SCENE_LINE_3DOBJ_MAX; i++){
				D3DOBJ_Draw( &emw->obj[line][i] );
			}
		}
	}
	NNS_G3dGePopMtx(1);
}

//--------------------------------------------------------------
/**
 * @brief   カメラ作成
 *
 * @param   emw		ゲームワークへのポインタ
 */
//--------------------------------------------------------------
static void Ending_CameraInit(ENDING_MAIN_WORK *emw)
{
	VecFx32	target = { FOOTPRINT_CAMERA_TX, FOOTPRINT_CAMERA_TY, FOOTPRINT_CAMERA_TZ };
	VecFx32 cam_pos = {-31712, -142304, 496744};
	VecFx32 t_pos = {-31712, -67780, -5704};
	
	emw->camera = GFC_AllocCamera( HEAPID_ENDING_DEMO );

	GFC_InitCameraTDA(&target, FOOTPRINT_CAMERA_DISTANCE, &EndingCameraAngle,
		FOOTPRINT_CAMERA_PERSPWAY, FOOTPRINT_CAMERA_MODE, FALSE, emw->camera);

//	GFC_SetCameraAngleRot(&cam_rotate, emw->camera);	//少し上向きのカメラにする
//	GFC_SetCameraAngleRev(&cam_rev_ang, emw->camera);
	GFC_SetCameraPos(&cam_pos, emw->camera);
	GFC_SetLookTarget(&t_pos, emw->camera);
	//GFC_SetCamUp(&cam_up, emw->camera);

	GFC_SetCameraClip( FOOTPRINT_CAMERA_NEAR, FOOTPRINT_CAMERA_FAR, emw->camera);
	
	GFC_AttachCamera(emw->camera);
}

//--------------------------------------------------------------
/**
 * @brief   カメラ解放
 *
 * @param   emw		ゲームワークへのポインタ
 */
//--------------------------------------------------------------
static void Ending_CameraExit(ENDING_MAIN_WORK *emw)
{
	GFC_FreeCamera(emw->camera);
}

//--------------------------------------------------------------
/**
 * @brief   足跡ボード用3DBG初期化関数
 * 
 * @param   ヒープID
 */
//--------------------------------------------------------------
static GF_G3DMAN * Ending_3D_Init(int heap_id)
{
	GF_G3DMAN *g3Dman;
	
	g3Dman = GF_G3DMAN_Init(heap_id, GF_G3DMAN_LNK, GF_G3DTEX_128K, 
		GF_G3DMAN_LNK, GF_G3DPLT_16K, EndingSimpleSetUp);
	return g3Dman;
}

static void EndingSimpleSetUp(void)
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
 * @brief   足跡ボード用3DBG終了処理
 *
 * @param   g3Dman		
 */
//--------------------------------------------------------------
static void Ending_3D_Exit(GF_G3DMAN *g3Dman)
{
	GF_G3D_Exit(g3Dman);
}

//--------------------------------------------------------------
/**
 * @brief   デバッグ機能：カメラ移動
 *
 * @param   emw		
 */
//--------------------------------------------------------------
static void Debug_CameraMove(ENDING_MAIN_WORK *emw)
{
#ifdef PM_DEBUG
	VecFx32 move = {0,0,0};
	fx32 value = FX32_ONE;
	int add_angle = 64;
	CAMERA_ANGLE angle = {0,0,0,0};
	int mode = 0;
	VecFx32 print_vec;
	enum{
		MODE_ROTATE, 		//自転
		MODE_SHIFT,			//平行移動
		MODE_DISTANCE,		//距離
		MODE_ANGLE_REV,		//公転
	};
	static CAMERA_ANGLE cam_up = {0, 0, 0};
	
	if((sys.cont & PAD_BUTTON_L) && (sys.cont & PAD_BUTTON_R)){
		mode = MODE_DISTANCE;
	}
	else if(sys.cont & PAD_BUTTON_L){
		mode = MODE_SHIFT;
	}
	else if(sys.cont & PAD_BUTTON_R){
		mode = MODE_ANGLE_REV;
	}
	else{
		//return;
		mode = MODE_ROTATE;
	}
	
	switch(mode){
	case MODE_ROTATE:
		if(sys.cont & PAD_KEY_LEFT){
			cam_up.y += 0x100;
		}
		else if(sys.cont & PAD_KEY_RIGHT){
			cam_up.y -= 0x100;
		}
		else if(sys.cont & PAD_KEY_UP){
			cam_up.x -= 0x100;
		}
		else if(sys.cont & PAD_KEY_DOWN){
			cam_up.x += 0x100;
		}
		else if(sys.cont & PAD_BUTTON_X){
			cam_up.z -= 0x100;
		}
		else if(sys.cont & PAD_BUTTON_B){
			cam_up.z += 0x100;
		}
		else{
			return;
		}
		GFC_SetCameraAngleRot(&cam_up,emw->camera);
	//	GFC_SetCamUp(&cam_up, emw->camera);	//少し上向きのカメラにする
		OS_TPrintf("cam_up.x = %d, y = %d, z = %d\n", cam_up.x, cam_up.y, cam_up.z);
		print_vec = GFC_GetLookTarget(emw->camera);
		OS_TPrintf("target.x = %d, y = %d, z = %d\n", print_vec.x, print_vec.y, print_vec.z);
		print_vec = GFC_GetCameraPos(emw->camera);
		OS_TPrintf("pos.x = %d, y = %d, z = %d\n", print_vec.x, print_vec.y, print_vec.z);
		break;
	case MODE_SHIFT:
		if(sys.cont & PAD_KEY_LEFT){
			move.x -= value;
		}
		if(sys.cont & PAD_KEY_RIGHT){
			move.x += value;
		}
		if(sys.cont & PAD_KEY_UP){
			move.y += value;
		}
		if(sys.cont & PAD_KEY_DOWN){
			move.y -= value;
		}
		if(sys.cont & PAD_BUTTON_X){
			move.z -= value;
		}
		if(sys.cont & PAD_BUTTON_B){
			move.z += value;
		}
		GFC_ShiftCamera(&move, emw->camera);
		move = GFC_GetCameraPos(emw->camera);
		OS_TPrintf("カメラ位置 x=%d(16進:%x), y=%d(16進:%x), z=%d(16進:%x)\n", move.x, move.x, move.y, move.y, move.z, move.z);
		print_vec = GFC_GetLookTarget(emw->camera);
		OS_TPrintf("target.x = %d, y = %d, z = %d\n", print_vec.x, print_vec.y, print_vec.z);
		print_vec = GFC_GetCameraPos(emw->camera);
		OS_TPrintf("pos.x = %d, y = %d, z = %d\n", print_vec.x, print_vec.y, print_vec.z);
		break;
	
	case MODE_ANGLE_REV:
		if(sys.cont & PAD_KEY_LEFT){
			angle.y -= add_angle;
		}
		if(sys.cont & PAD_KEY_RIGHT){
			angle.y += add_angle;
		}
		if(sys.cont & PAD_KEY_UP){
			angle.x += add_angle;
		}
		if(sys.cont & PAD_KEY_DOWN){
			angle.x -= add_angle;
		}
		if(sys.cont & PAD_BUTTON_X){
			angle.z -= add_angle;
		}
		if(sys.cont & PAD_BUTTON_B){
			angle.z += add_angle;
		}
		GFC_AddCameraAngleRev(&angle, emw->camera);
		angle = GFC_GetCameraAngle(emw->camera);
		OS_TPrintf("カメラアングル　x=%d, y=%d, z=%d\n", angle.x, angle.y, angle.z);
		print_vec = GFC_GetLookTarget(emw->camera);
		OS_TPrintf("target.x = %d, y = %d, z = %d\n", print_vec.x, print_vec.y, print_vec.z);
		print_vec = GFC_GetCameraPos(emw->camera);
		OS_TPrintf("pos.x = %d, y = %d, z = %d\n", print_vec.x, print_vec.y, print_vec.z);
		break;
	case MODE_DISTANCE:
		if(sys.cont & PAD_KEY_UP){
			GFC_AddCameraDistance(FX32_ONE, emw->camera);
		}
		if(sys.cont & PAD_KEY_DOWN){
			GFC_AddCameraDistance(-FX32_ONE, emw->camera);
		}
		OS_TPrintf("カメラ距離＝%d(16進:%x)\n", GFC_GetCameraDistance(emw->camera), GFC_GetCameraDistance(emw->camera));
		print_vec = GFC_GetLookTarget(emw->camera);
		OS_TPrintf("target.x = %d, y = %d, z = %d\n", print_vec.x, print_vec.y, print_vec.z);
		print_vec = GFC_GetCameraPos(emw->camera);
		OS_TPrintf("pos.x = %d, y = %d, z = %d\n", print_vec.x, print_vec.y, print_vec.z);
		break;
	}
#endif
}

