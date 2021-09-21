//==============================================================================
/**
 * @file	hakai_warp.c
 * @brief	破れた世界突入デモ
 * @author	matsuda
 * @date	2008.04.15(火)
 */
//==============================================================================
#include "common.h"
#include "system/palanm.h"
#include "system/lib_pack.h"
#include "system/snd_tool.h"
#include "system/brightness.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "system/wipe.h"
#include "system/pm_rtc.h"
#include "system/main.h"
#include "system/savedata.h"
#include "system/fontproc.h"
#include "system/procsys.h"
#include "system/pmfprint.h"

#include "../title/titledemo.naix"
#include "system/d3dobj.h"
#include "demo/hakai_warp.h"


//==============================================================================
//	定数定義
//==============================================================================
///HEAPID_TITLE_DEMOが確保するヒープサイズ
#define HAKAI_WARP_HEAP_SIZE		(0x50000)

//--------------------------------------------------------------
//	カメラ設定
//--------------------------------------------------------------
#define HAKAI_WARP_CAMERA_MODE			GF_CAMERA_PERSPECTIV	//(GF_CAMERA_ORTHO)

#define HAKAI_WARP_CAMERA_CLIP_NEAR		(0)
#define HAKAI_WARP_CAMERA_CLIP_FAR		(FX32_ONE*300)

///デモ用カメラ設定
#define DEMO_CAMERA_PERSPWAY		(FX_GET_ROTA_NUM(22))
#define DEMO_CAMERA_TX			( 0 )		/// target
#define DEMO_CAMERA_TY			( 0 )
#define DEMO_CAMERA_TZ			( 0 )
///デモ用カメラの注視点までの距離
#define DEMO_CAMERA_DISTANCE		(160 << FX32_SHIFT)

///デモ用カメラの移動速度
#define DEMO_CAMERA_MOVE_SPEED		(0xa00)
enum{
	DEMO_CAMERA_MOVE_FRAME = 60,			//移動しているフレーム
	DEMO_KAO_ANM_START_FRAME = 75,			//顔アニメ開始フレーム
	DEMO_CAMERA_ANGLE_START_FRAME = 90 + 160,		//角度変更開始フレーム
	DEMO_CAMERA_ANGLE_RETURN_FRAME = 95,	//角度変更折り返しフレーム
	DEMO_CAMERA_ANGLE_END_FRAME = 105,		//角度戻し終了フレーム
	DEMO_CAMERA_DISTANCE_MOVE_FRAME = 100,//115,	//距離詰める開始フレーム
};
///デモ用カメラのスタートオフセットZ(移動距離)
#define DEMO_CAMERA_MOVE_OFFSET_Z	(DEMO_CAMERA_MOVE_SPEED * DEMO_CAMERA_MOVE_FRAME)

///デモ用カメラの突っ込み前の角度加算値の初期値
#define DEMO_CAMERA_ADD_ANGLE_INIT		(0x10000 - 0x1c7d)	//(40)
#define DEMO_CAMERA_ADD_ANGLE_FRAME		(30)
#define DEMO_CAMERA_ADD_ANGLE_END		(0x10000 - 0x3fef)//(90 - DEMO_CAMERA_ADD_ANGLE_INIT-1)	//最終的な角度が90度(end+init)だと真っ暗になる

///デモ用カメラの突っ込み前の距離加算値の初期値
#define DEMO_CAMERA_ADD_DISTANCE_INIT	(FX32_ONE)
///デモ用カメラの突っ込み前のパース加算値の初期値
#define DEMO_CAMERA_ADD_PERSPWAY_INIT	(60 << 8)
///デモ用カメラの突っ込みのパース減算値の最低地
#define DEMO_CAMERA_ADD_PERSPWAY_KEEP	(16 << 8)
///デモ用カメラの突っ込み前のパース減算値の初期値
#define DEMO_CAMERA_ADD_PERSPWAY_SUB	(0x0080)


//--------------------------------------------------------------
//	モデル
//--------------------------------------------------------------
#define ANA_3D_X		(0)	//fx32
#define ANA_3D_Y		(0)	//fx32
#define ANA_3D_Z		(0)	//fx32
#define ANA_3D_SCALE	(FX32_ONE)



//==============================================================================
//	構造体定義
//==============================================================================
///破壊突入デモ制御構造体
typedef struct _WARP_SYS{
	GF_G3DMAN *g3Dman;
	GF_CAMERA_PTR camera;				///<カメラへのポインタ

	TCB_PTR update_tcb;					///<Update用TCBへのポインタ
	
	int frame_count;
	int se_counter;						//SE再生用のカウンター(08.04.17)
	
	D3DOBJ ana_obj;
	D3DOBJ_ANM ana_ica_anm;
	D3DOBJ_ANM ana_ita_anm;
	D3DOBJ_MDL ana_mdl;
	
	u32 demo_move_frame;
	int demo_camera_add_angle;
	int demo_camera_add_angle_calc;
	fx32 demo_camera_add_distance;
	int demo_camera_add_persp_way;

	NNSFndAllocator			allocater;
}WARP_SYS;


//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static void HakaiWarp_Update(TCB_PTR tcb, void *work);
static void VBlankFunc( void * work );
static void HakaiWarp_VramBankSet(void);
static void HakaiWarp_CameraInit(WARP_SYS *warp);
static void HakaiWarp_CameraExit(WARP_SYS *warp);
static void Model3DSet( WARP_SYS * warp);
static void Model3DDel(WARP_SYS *warp);
static void Model3D_Update(WARP_SYS *warp);
static GF_G3DMAN * HakaiWarp_3D_Init(int heap_id);
static void HakaiWarpSimpleSetUp(void);
static void HakaiWarp_3D_Exit(GF_G3DMAN *g3Dman);
static void Warp_Demo_CameraMove(WARP_SYS *warp);



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
PROC_RESULT HakaiWarpProc_Init( PROC * proc, int * seq )
{
	WARP_SYS *warp;
	
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

	sys_CreateHeap(HEAPID_BASE_APP, HEAPID_TITLE_DEMO, HAKAI_WARP_HEAP_SIZE);

	warp = PROC_AllocWork(proc, sizeof(WARP_SYS), HEAPID_TITLE_DEMO );
	MI_CpuClear8(warp, sizeof(WARP_SYS));
//	warp->parent_work = PROC_GetParentWork(proc);
	
//	simple_3DBGInit(HEAPID_TITLE_DEMO);
	warp->g3Dman = HakaiWarp_3D_Init(HEAPID_TITLE_DEMO);

	sys_KeyRepeatSpeedSet( SYS_KEYREPEAT_SPEED_DEF, SYS_KEYREPEAT_WAIT_DEF );

	//VRAM割り当て設定
	HakaiWarp_VramBankSet();

	// タッチパネルシステム初期化
	InitTPSystem();
	InitTPNoBuff(4);

	//3Dモデル転送
	Model3DSet(warp);

	//カメラ作成
	HakaiWarp_CameraInit(warp);

	// ワイプフェード開始
	WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK, 
		16, WIPE_DEF_SYNC, HEAPID_TITLE_DEMO );

	//メイン画面設定
	sys.disp3DSW = DISP_3D_TO_MAIN;
	GF_Disp_DispSelect();
	GF_Disp_DispOn();

	MsgPrintSkipFlagSet(MSG_SKIP_ON);
	MsgPrintAutoFlagSet(MSG_AUTO_OFF);
	MsgPrintTouchPanelFlagSet(MSG_TP_OFF);

	warp->update_tcb = TCB_Add(HakaiWarp_Update, warp, 60000);

	sys_VBlankFuncChange(VBlankFunc, warp);

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
PROC_RESULT HakaiWarpProc_Main( PROC * proc, int * seq )
{
	WARP_SYS * warp  = PROC_GetWork( proc );
	enum{
		SEQ_INIT,
		SEQ_MAIN,
		SEQ_OUT_INIT,
		SEQ_OUT_WAIT,
	};
	
	switch(*seq){
	case SEQ_INIT:
		if(WIPE_SYS_EndCheck() == TRUE){
			(*seq)++;
		}
		break;
	case SEQ_MAIN:
		if( warp->se_counter == 15 ){
			Snd_SePlay( SEQ_SE_PL_SYUWA2 );
		}
		warp->se_counter++;

		warp->frame_count++;
		if(warp->frame_count > 85){
			(*seq)++;
		}
		break;
	case SEQ_OUT_INIT:
		// ワイプフェード開始
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, 
			20, WIPE_DEF_SYNC, HEAPID_TITLE_DEMO );
		(*seq)++;
		break;
	case SEQ_OUT_WAIT:
		if(WIPE_SYS_EndCheck() == TRUE){
			return PROC_RES_FINISH;
		}
		break;
	}

	Warp_Demo_CameraMove(warp);

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
PROC_RESULT HakaiWarpProc_End( PROC * proc, int * seq )
{
	WARP_SYS * warp  = PROC_GetWork( proc );

	TCB_Delete(warp->update_tcb);

	//3Dモデル解放
	Model3DDel(warp);
	//カメラ削除
	HakaiWarp_CameraExit(warp);

	//simple_3DBGExit();
	HakaiWarp_3D_Exit(warp->g3Dman);

	sys_VBlankFuncChange( NULL, NULL );		// VBlankセット
	sys_HBlankIntrStop();	//HBlank割り込み停止

	StopTP();		//タッチパネルの終了

	MsgPrintSkipFlagSet(MSG_SKIP_OFF);
	MsgPrintAutoFlagSet(MSG_AUTO_OFF);
	MsgPrintTouchPanelFlagSet(MSG_TP_OFF);

	PROC_FreeWork( proc );				// PROCワーク開放
	sys_DeleteHeap( HEAPID_TITLE_DEMO );

	return PROC_RES_FINISH;
}


//==============================================================================
//	
//==============================================================================
static void HakaiWarp_Update(TCB_PTR tcb, void *work)
{
	WARP_SYS *warp = work;
	
	Model3D_Update(warp);
	GF_G3_RequestSwapBuffers(GX_SORTMODE_MANUAL, GX_BUFFERMODE_W);
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
	WARP_SYS *warp = work;
	
	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}

//--------------------------------------------------------------
/**
 * @brief   Vramバンク設定を行う
 *
 */
//--------------------------------------------------------------
static void HakaiWarp_VramBankSet(void)
{
	GF_Disp_GX_VisibleControlInit();
	GF_Disp_GXS_VisibleControlInit();
	
	//VRAM設定
	{
		GF_BGL_DISPVRAM vramSetTable = {
			GX_VRAM_BG_128_C,				// メイン2DエンジンのBG
			GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット

			GX_VRAM_SUB_BG_32_H,			// サブ2DエンジンのBG
			GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット

			GX_VRAM_OBJ_64_E,				// メイン2DエンジンのOBJ
			GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット

			GX_VRAM_SUB_OBJ_16_I,			// サブ2DエンジンのOBJ
			GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット

			GX_VRAM_TEX_01_AB,				// テクスチャイメージスロット
			GX_VRAM_TEXPLTT_01_FG			// テクスチャパレットスロット
		};
		GF_Disp_SetBank( &vramSetTable );

		//VRAMクリア
		MI_CpuClear32((void*)HW_BG_VRAM, HW_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_BG_VRAM, HW_DB_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_OBJ_VRAM, HW_OBJ_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_OBJ_VRAM, HW_DB_OBJ_VRAM_SIZE);
	}
}

//--------------------------------------------------------------
/**
 * @brief   カメラ作成
 *
 * @param   warp		ゲームワークへのポインタ
 */
//--------------------------------------------------------------
static void HakaiWarp_CameraInit(WARP_SYS *warp)
{
	{//デモ用カメラセット
		static const CAMERA_ANGLE DemoCameraAngle = {	//カメラアングル
			DEMO_CAMERA_ADD_ANGLE_INIT, FX_GET_ROTA_NUM(0), FX_GET_ROTA_NUM(0),
		};
		VecFx32	target = { DEMO_CAMERA_TX, DEMO_CAMERA_TY, DEMO_CAMERA_TZ };
		
		warp->camera = GFC_AllocCamera(HEAPID_TITLE_DEMO);	//カメラ作成

		GFC_InitCameraTDA(&target, DEMO_CAMERA_DISTANCE, &DemoCameraAngle,
			DEMO_CAMERA_PERSPWAY, GF_CAMERA_PERSPECTIV, FALSE, warp->camera);

		GFC_SetCameraClip( HAKAI_WARP_CAMERA_CLIP_NEAR, HAKAI_WARP_CAMERA_CLIP_FAR, 
			warp->camera);

		//スクロールINさせるのでちょっと離す
		{
//			VecFx32 move = {0,0,DEMO_CAMERA_MOVE_OFFSET_Z};
//			GFC_ShiftCamera(&move, warp->camera);
		}

		{
			CAMERA_ANGLE angle = {0,0,0,0};

			angle = GFC_GetCameraAngle(warp->camera);
			angle.x = DEMO_CAMERA_ADD_ANGLE_END;
			GFC_SetCameraAngleRev(&angle, warp->camera);
		}
		
		GFC_AttachCamera(warp->camera);
	}
}

//--------------------------------------------------------------
/**
 * @brief   カメラ解放
 *
 * @param   warp		ゲームワークへのポインタ
 */
//--------------------------------------------------------------
static void HakaiWarp_CameraExit(WARP_SYS *warp)
{
	GFC_FreeCamera(warp->camera);
}

//--------------------------------------------------------------
/**
 * @brief   3Dモデルセット
 *
 * @param   warp		
 */
//--------------------------------------------------------------
static void Model3DSet( WARP_SYS * warp)
{
	ARCHANDLE* hdl;

	sys_InitAllocator( &warp->allocater, HEAPID_TITLE_DEMO, 4 ); 

	hdl  = ArchiveDataHandleOpen(ARC_TITLE_PL, HEAPID_TITLE_DEMO); 

	{//穴
		//モデルデータ読み込み
		D3DOBJ_MdlLoadH(&warp->ana_mdl, hdl, NARC_titledemo_op_ana_nsbmd, HEAPID_TITLE_DEMO);
		NNS_G3dMdlUseMdlAlpha(warp->ana_mdl.pModel);
		NNS_G3dMdlUseMdlPolygonID(warp->ana_mdl.pModel);
		//icaアニメデータ読み込み
		D3DOBJ_AnmLoadH(&warp->ana_ica_anm, &warp->ana_mdl, hdl, 
			NARC_titledemo_op_ana_nsbca, HEAPID_TITLE_DEMO, &warp->allocater);
		D3DOBJ_AnmSet(&warp->ana_ica_anm, 0);
		//itaアニメデータ読み込み
		D3DOBJ_AnmLoadH(&warp->ana_ita_anm, &warp->ana_mdl, hdl, 
			NARC_titledemo_op_ana_nsbta, HEAPID_TITLE_DEMO, &warp->allocater);
		D3DOBJ_AnmSet(&warp->ana_ita_anm, 0);

		//レンダーオブジェクトに登録
		D3DOBJ_Init(&warp->ana_obj, &warp->ana_mdl);
		//座標設定
		D3DOBJ_SetMatrix( &warp->ana_obj, ANA_3D_X, ANA_3D_Y, ANA_3D_Z);
		D3DOBJ_SetScale(&warp->ana_obj, ANA_3D_SCALE, ANA_3D_SCALE, ANA_3D_SCALE);
		D3DOBJ_SetDraw( &warp->ana_obj, TRUE );
		//アニメ関連付け
		D3DOBJ_AddAnm(&warp->ana_obj, &warp->ana_ica_anm);
		D3DOBJ_AddAnm(&warp->ana_obj, &warp->ana_ita_anm);
	}

	ArchiveDataHandleClose( hdl );

	warp->demo_camera_add_angle = (DEMO_CAMERA_ADD_ANGLE_END - DEMO_CAMERA_ADD_ANGLE_INIT) / DEMO_CAMERA_ADD_ANGLE_FRAME;
	warp->demo_camera_add_angle_calc = DEMO_CAMERA_ADD_ANGLE_INIT;
	warp->demo_camera_add_distance = DEMO_CAMERA_ADD_DISTANCE_INIT;
	warp->demo_camera_add_persp_way = DEMO_CAMERA_ADD_PERSPWAY_INIT;
}

//--------------------------------------------------------------
/**
 * @brief   3Dモデル削除
 *
 * @param   warp		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
static void Model3DDel(WARP_SYS *warp)
{
	D3DOBJ_MdlDelete( &warp->ana_mdl );
	D3DOBJ_AnmDelete(&warp->ana_ica_anm, &warp->allocater);
	D3DOBJ_AnmDelete(&warp->ana_ita_anm, &warp->allocater);
}

//--------------------------------------------------------------
/**
 * @brief   3Dモデル更新
 *
 * @param   warp		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
static void Model3D_Update(WARP_SYS *warp)
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
	
	GFC_AttachCamera(warp->camera);
	GFC_SetCameraView(HAKAI_WARP_CAMERA_MODE, warp->camera); //正射影設定
	GFC_CameraLookAt();

	// ライトとアンビエント
	NNS_G3dGlbLightVector( 0, 0, -FX32_ONE, 0 );
	NNS_G3dGlbLightColor( 0, GX_RGB( 28,28,28 ) );
	NNS_G3dGlbMaterialColorDiffAmb( GX_RGB( 31,31,31 ), GX_RGB( 31,31,31 ), FALSE );
	NNS_G3dGlbMaterialColorSpecEmi( GX_RGB( 31,31,31 ), GX_RGB( 31,31,31 ), FALSE );
	
	// 位置設定
	NNS_G3dGlbSetBaseTrans(&trans);
	// 角度設定
	NNS_G3dGlbSetBaseRot(&rot);
	// スケール設定
	NNS_G3dGlbSetBaseScale(&scale_vec);

//	NNS_G3dGlbFlush();
	
	
	//描画
	D3DOBJ_AnmLoop(&warp->ana_ica_anm, FX32_ONE);
	D3DOBJ_AnmLoop(&warp->ana_ita_anm, FX32_ONE);
	NNS_G3dGePushMtx();
	{
		D3DOBJ_Draw( &warp->ana_obj );
	}
	NNS_G3dGePopMtx(1);
}

//--------------------------------------------------------------
/**
 * @brief   破壊突入デモ用3DBG初期化関数
 * 
 * @param   ヒープID
 */
//--------------------------------------------------------------
static GF_G3DMAN * HakaiWarp_3D_Init(int heap_id)
{
	GF_G3DMAN *g3Dman;
	
	g3Dman = GF_G3DMAN_Init(heap_id, GF_G3DMAN_LNK, GF_G3DTEX_256K, 
		GF_G3DMAN_LNK, GF_G3DPLT_32K, HakaiWarpSimpleSetUp);
	return g3Dman;
}

static void HakaiWarpSimpleSetUp(void)
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
 * @brief   破壊突入デモ用3DBG終了処理
 *
 * @param   g3Dman		
 */
//--------------------------------------------------------------
static void HakaiWarp_3D_Exit(GF_G3DMAN *g3Dman)
{
	GF_G3D_Exit(g3Dman);
}

//--------------------------------------------------------------
/**
 * @brief   デモ用カメラの移動
 *
 * @param   warp		
 */
//--------------------------------------------------------------
static void Warp_Demo_CameraMove(WARP_SYS *warp)
{
	VecFx32 move = {0,0,0};
	CAMERA_ANGLE angle = {0,0,0,0};
	int set_angle;
	
	GFC_AddCameraPerspWay(-(warp->demo_camera_add_persp_way >> 8), warp->camera);
	warp->demo_camera_add_persp_way -= DEMO_CAMERA_ADD_PERSPWAY_SUB;
	if(warp->demo_camera_add_persp_way < DEMO_CAMERA_ADD_PERSPWAY_KEEP){
		warp->demo_camera_add_persp_way = DEMO_CAMERA_ADD_PERSPWAY_KEEP;
	}
	warp->demo_move_frame++;
}
