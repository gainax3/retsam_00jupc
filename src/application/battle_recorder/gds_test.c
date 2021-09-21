//==============================================================================
/**
 * @file	gds_test.c
 * @brief	GDSライブラリテスト用ソース
 * @author	matsuda
 * @date	2008.01.09(水)
 */
//==============================================================================
#include "common.h"

#include "savedata/savedata.h"

#include "system/pms_data.h"
#include "system/pm_str.h"

#include "gds_battle_rec.h"
#include "gds_ranking.h"
#include "gds_boxshot.h"
#include "gds_dressup.h"

#include "savedata/gds_profile.h"
#include "savedata/imageclip_data.h"
#include "poketool/boxdata.h"
#include "poketool/monsno.h"

#include "system/msgdata_util.h"
#include "system/wordset.h"
#include "msgdata/msg.naix"
#include "system/fontproc.h"
#include "gflib/strbuf_family.h"
#include "system/wipe.h"

#include "system/procsys.h"
#include "communication/wm_icon.h"
#include "communication\comm_system.h"
#include "communication\comm_state.h"
#include "communication\comm_def.h"

#include "savedata\system_data.h"

#include "gds_rap.h"
#include "gds_debug.h"
#include "gds_rap_response.h"

#include "savedata/battle_rec.h"

#include "br_define.h"

#include "msgdata/msg_debug_matsu.h"


#ifdef PM_DEBUG	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//==============================================================================
//	定数定義
//==============================================================================
///GDSテストで使用するヒープID
#define HEAPID_GDS_TEST		(HEAPID_CONTEST)


//--------------------------------------------------------------
//	BMPウィンドウ
//--------------------------------------------------------------
///BMPウィンドウCGXエリア開始位置(オフセット)
#define BMPWIN_CGX_START			((0x8000 - 0x4000) / 32)

#define BMPWIN_TALK_COLOR			(0xd)

#define BMPWIN_TITLE_CGX_OFFSET		(BMPWIN_CGX_START)
#define BMPWIN_TITLE_CGX_SIZE		(20 * 2)	//(0x900 / 32)

#define BMPWIN_LOG_CGX_OFFSET		(BMPWIN_TITLE_CGX_OFFSET + BMPWIN_TITLE_CGX_SIZE)
#define BMPWIN_LOG_CGX_SIZE			(20 * 12)	//(0x900 / 32)



enum{
	TEST_BMPWIN_TITLE,
	TEST_BMPWIN_LOG,
	
	TEST_BMPWIN_MAX,	//終端
};

//==============================================================================
//	構造体定義
//==============================================================================
typedef struct{
	IMC_TELEVISION_SAVEDATA* dummy_imc;
	POKEMON_PARAM *pp;
	int monsno;
	int count;
}DRESS_ALL_UP;

typedef struct _GDS_TEST_SYS{
	//システム系
	GF_BGL_INI *bgl;
	GF_BGL_BMPWIN win[TEST_BMPWIN_MAX];
	MSGDATA_MANAGER *msgman;		///<メッセージデータマネージャのポインタ
	WORDSET *wordset;				///<Allocしたメッセージ用単語バッファへのポインタ
	STRBUF *msg_buf;				///<Allocした文字列バッファへのポインタ
	TCB_PTR update_tcb;
	int heap_id;
	SAVEDATA *sv;
	int msg_index;
	
	//通信パラメータ
	GDS_PROFILE_PTR gds_profile_ptr;	
	
	//通信
	GDS_RAP_WORK gdsrap;	///<GDSライブラリ、NitroDWC関連のワーク構造体
	
	//ローカルワーク
	int seq;
	
	//受信データ
	int ranking_type[GT_RANKING_WEEK_NUM];
	u64 data_number;
	
	//ドレスアップ全アップ用ワーク
	DRESS_ALL_UP dau;
}GDS_TEST_SYS;

//==============================================================================
//	プロトタイプ宣言
//==============================================================================
PROC_RESULT GdsTestProc_Init( PROC * proc, int * seq );
PROC_RESULT GdsTestProc_Main( PROC * proc, int * seq );
PROC_RESULT GdsTestProc_End( PROC * proc, int * seq );
PROC_RESULT GdsTestDressUpProc_Main( PROC * proc, int * seq );
static int GdsTest_Main(GDS_TEST_SYS *testsys);
static int GdsTest_DressUpload(GDS_TEST_SYS *testsys);
static void GdsTest_VramBankSet(GF_BGL_INI *bgl);
static void GdsTest_DefaultBmpWinAdd(GDS_TEST_SYS *testsys);
static void GdsTest_DefaultBmpWinDel(GDS_TEST_SYS *testsys);
static void GdsTestUpdate(TCB_PTR tcb, void *work);
static void GdsTestVBlank(void *work);
static void GdsTest_Callback_ErrorWideMsgPrint(void *work, STRBUF *error_msg);
static void Response_DressRegist(void *work, const GDS_RAP_ERROR_INFO *error_info);
static void Response_DressGet(void *work, const GDS_RAP_ERROR_INFO *error_info);
static void Response_BoxRegist(void *work, const GDS_RAP_ERROR_INFO *error_info);
static void Response_BoxGet(void *work, const GDS_RAP_ERROR_INFO *error_info);
static void Response_RankingType(void *work, const GDS_RAP_ERROR_INFO *error_info);
static void Response_RankingUpdate(void *work, const GDS_RAP_ERROR_INFO *error_info);
static void Response_BattleVideoRegist(void *work, const GDS_RAP_ERROR_INFO *error_info);
static void Response_BattleVideoSearch(void *work, const GDS_RAP_ERROR_INFO *error_info);
static void Response_BattleVideoDataGet(void *work, const GDS_RAP_ERROR_INFO *error_info);
static void Response_BattleVideoFavorite(void *work, const GDS_RAP_ERROR_INFO *error_info);


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
PROC_RESULT GdsTestProc_Init( PROC * proc, int * seq )
{
	GDS_TEST_SYS *testsys;
	
	sys_VBlankFuncChange(NULL, NULL);	// VBlankセット
//	sys_HBlankIntrStop();	//HBlank割り込み停止

	GF_Disp_GX_VisibleControlInit();
	GF_Disp_GXS_VisibleControlInit();
	GX_SetVisiblePlane(0);
	GXS_SetVisiblePlane(0);
	GX_SetVisibleWnd(GX_WNDMASK_NONE);
	GXS_SetVisibleWnd(GX_WNDMASK_NONE);
	G2_BlendNone();
	G2S_BlendNone();

	sys_CreateHeap(HEAPID_BASE_APP, HEAPID_GDS_TEST, 0x50000);

	testsys = PROC_AllocWork(proc, sizeof(GDS_TEST_SYS), HEAPID_GDS_TEST );
	MI_CpuClear8(testsys, sizeof(GDS_TEST_SYS));
	testsys->heap_id = HEAPID_GDS_TEST;
	testsys->sv = PROC_GetParentWork(proc);

//	testsys->bsw = PROC_GetParentWork(proc);
	
	testsys->bgl = GF_BGL_BglIniAlloc(HEAPID_GDS_TEST);

	initVramTransferManagerHeap(64, HEAPID_GDS_TEST);

	sys_KeyRepeatSpeedSet( SYS_KEYREPEAT_SPEED_DEF, SYS_KEYREPEAT_WAIT_DEF );

	//VRAM割り当て設定
	GdsTest_VramBankSet(testsys->bgl);

	// タッチパネルシステム初期化
	InitTPSystem();
	InitTPNoBuff(4);

	//通信アイコン用にキャラ＆パレット制限
#if 0	//キャラマネージャー作っていないので使わない
	CLACT_U_WmIcon_SetReserveAreaCharManager(NNS_G2D_VRAM_TYPE_2DMAIN, GX_OBJVRAMMODE_CHAR_1D_64K);
	CLACT_U_WmIcon_SetReserveAreaPlttManager(NNS_G2D_VRAM_TYPE_2DMAIN);
	WirelessIconEasy();	//通信アイコン
#endif

	//メッセージマネージャ作成
	testsys->msgman = MSGMAN_Create(MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_debug_matsu_dat, 
		HEAPID_GDS_TEST);

	testsys->wordset = WORDSET_Create(HEAPID_GDS_TEST);	//単語バッファ作成
	testsys->msg_buf = STRBUF_Create(2*160, HEAPID_GDS_TEST);//文字列バッファ作成 160文字分
	
	//BMPウィンドウ追加
	GdsTest_DefaultBmpWinAdd(testsys);

	//システムフォントパレット：メイン画面
	SystemFontPaletteLoad(PALTYPE_MAIN_BG, BMPWIN_TALK_COLOR*32, HEAPID_GDS_TEST);

	// 輝度変更セット
	WIPE_SYS_Start(WIPE_PATTERN_WMS, WIPE_TYPE_BLINDIN_H, WIPE_TYPE_BLINDIN_H, WIPE_FADE_BLACK, 
		WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_GDS_TEST);

	testsys->update_tcb = TCB_Add(GdsTestUpdate, testsys, 60000);

	//メイン画面設定
	sys.disp3DSW = DISP_3D_TO_MAIN;
	GF_Disp_DispSelect();

	GF_Disp_DispOn();
	GF_Disp_GX_VisibleControl(GX_PLANEMASK_OBJ, VISIBLE_ON);
	GF_Disp_GXS_VisibleControl(GX_PLANEMASK_OBJ, VISIBLE_ON);

	sys_VBlankFuncChange(GdsTestVBlank, testsys);
	
	//通信パラメータ作成
	testsys->gds_profile_ptr = GDS_Profile_AllocMemory(HEAPID_GDS_TEST);
	GDS_Profile_MyDataSet(testsys->gds_profile_ptr, testsys->sv);

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
PROC_RESULT GdsTestProc_Main( PROC * proc, int * seq )
{
	GDS_TEST_SYS * testsys  = PROC_GetWork( proc );
	int ret;
	enum{
		SEQ_IN_WAIT,
	
		SEQ_COMM_INIT,
		SEQ_COMM_MAIN,
		SEQ_COMM_END,
	
		SEQ_END,
	};
	
	switch( *seq ){
	case SEQ_IN_WAIT:
		if(WIPE_SYS_EndCheck() == TRUE){
			(*seq)++;
		}
		break;
	
	case SEQ_COMM_INIT:
		{
			GDSRAP_INIT_DATA init_data;
			
			init_data.gs_profile_id = SYSTEMDATA_GetDpwInfo(SaveData_GetSystemData(testsys->sv));
			init_data.heap_id = HEAPID_GDS_TEST;
			init_data.savedata = testsys->sv;
			
			init_data.response_callback.callback_work = testsys;
			init_data.response_callback.func_dressupshot_regist = Response_DressRegist;
			init_data.response_callback.func_dressupshot_get = Response_DressGet;
			init_data.response_callback.func_boxshot_regist = Response_BoxRegist;
			init_data.response_callback.func_boxshot_get = Response_BoxGet;
			init_data.response_callback.func_ranking_type_get = Response_RankingType;
			init_data.response_callback.func_ranking_update_get = Response_RankingUpdate;
			init_data.response_callback.func_battle_video_regist = Response_BattleVideoRegist;
			init_data.response_callback.func_battle_video_search_get = Response_BattleVideoSearch;
			init_data.response_callback.func_battle_video_data_get = Response_BattleVideoDataGet;
			init_data.response_callback.func_battle_video_favorite_regist = Response_BattleVideoFavorite;
			
			init_data.callback_work = testsys;
			init_data.callback_error_msg_wide = GdsTest_Callback_ErrorWideMsgPrint;

			GDSRAP_Init(&testsys->gdsrap, &init_data);	//通信ライブラリ初期化
		}
		(*seq)++;
		break;
	
	case SEQ_COMM_MAIN:
		if(GdsTest_Main(testsys) == FALSE){
			(*seq)++;
		}
		break;
	case SEQ_COMM_END:
		GDSRAP_Exit(&testsys->gdsrap);
		(*seq)++;
		break;
	
	case SEQ_END:
		return PROC_RES_FINISH;
	}

	return PROC_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   プロセス関数：メイン ※ドレスアップ全ポケモンアップロード
 *
 * @param   proc		プロセスデータ
 * @param   seq			シーケンス
 *
 * @retval  処理状況
 */
//--------------------------------------------------------------
PROC_RESULT GdsTestDressUpProc_Main( PROC * proc, int * seq )
{
	GDS_TEST_SYS * testsys  = PROC_GetWork( proc );
	int ret;
	enum{
		SEQ_IN_WAIT,
	
		SEQ_COMM_INIT,
		SEQ_COMM_MAIN,
		SEQ_COMM_END,
	
		SEQ_END,
	};
	
	switch( *seq ){
	case SEQ_IN_WAIT:
		if(WIPE_SYS_EndCheck() == TRUE){
			(*seq)++;
		}
		break;
	
	case SEQ_COMM_INIT:
		{
			GDSRAP_INIT_DATA init_data;
			
			init_data.gs_profile_id = SYSTEMDATA_GetDpwInfo(SaveData_GetSystemData(testsys->sv));
			init_data.heap_id = HEAPID_GDS_TEST;
			init_data.savedata = testsys->sv;
			
			init_data.response_callback.callback_work = testsys;
			init_data.response_callback.func_dressupshot_regist = Response_DressRegist;
			init_data.response_callback.func_dressupshot_get = Response_DressGet;
			init_data.response_callback.func_boxshot_regist = Response_BoxRegist;
			init_data.response_callback.func_boxshot_get = Response_BoxGet;
			init_data.response_callback.func_ranking_type_get = Response_RankingType;
			init_data.response_callback.func_ranking_update_get = Response_RankingUpdate;
			init_data.response_callback.func_battle_video_regist = Response_BattleVideoRegist;
			init_data.response_callback.func_battle_video_search_get = Response_BattleVideoSearch;
			init_data.response_callback.func_battle_video_data_get = Response_BattleVideoDataGet;
			init_data.response_callback.func_battle_video_favorite_regist = Response_BattleVideoFavorite;
			
			init_data.callback_work = testsys;
			init_data.callback_error_msg_wide = GdsTest_Callback_ErrorWideMsgPrint;

			GDSRAP_Init(&testsys->gdsrap, &init_data);	//通信ライブラリ初期化
		}
		(*seq)++;
		break;
	
	case SEQ_COMM_MAIN:
		if(GdsTest_DressUpload(testsys) == FALSE){
			(*seq)++;
		}
		break;
	case SEQ_COMM_END:
		GDSRAP_Exit(&testsys->gdsrap);
		(*seq)++;
		break;
	
	case SEQ_END:
		return PROC_RES_FINISH;
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
PROC_RESULT GdsTestProc_End( PROC * proc, int * seq )
{
	GDS_TEST_SYS * testsys = PROC_GetWork( proc );
	int i;

	GDS_Profile_FreeMemory(testsys->gds_profile_ptr);
	
	sys_VBlankFuncChange( NULL, NULL );		// VBlankセット
//	sys_HBlankIntrStop();	//HBlank割り込み停止

	//BMP開放
	GdsTest_DefaultBmpWinDel(testsys);
	
	//メイン画面BG削除
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
	GF_BGL_BGControlExit(testsys->bgl, GF_BGL_FRAME1_M );
	GF_BGL_BGControlExit(testsys->bgl, GF_BGL_FRAME2_M );

	//Vram転送マネージャー削除
	DellVramTransferManager();

	//メッセージマネージャの削除
	STRBUF_Delete(testsys->msg_buf);
	WORDSET_Delete(testsys->wordset);
	MSGMAN_Delete(testsys->msgman);

	//BGL開放
	sys_FreeMemoryEz(testsys->bgl);

	TCB_Delete(testsys->update_tcb);

	StopTP();		//タッチパネルの終了

	PROC_FreeWork(proc);				// ワーク開放

	WirelessIconEasyEnd();
	
	sys_DeleteHeap(HEAPID_GDS_TEST);

	return PROC_RES_FINISH;
}

//--------------------------------------------------------------
/**
 * @brief   Vramバンク設定を行う
 *
 * @param   bgl		BGLデータへのポインタ
 */
//--------------------------------------------------------------
static void GdsTest_VramBankSet(GF_BGL_INI *bgl)
{
	GF_Disp_GX_VisibleControlInit();

	//VRAM設定
	{
		GF_BGL_DISPVRAM vramSetTable = {
			GX_VRAM_BG_128_B,				// メイン2DエンジンのBG
			GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット
			GX_VRAM_SUB_BG_128_C,			// サブ2DエンジンのBG
			GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット
			GX_VRAM_OBJ_64_E,				// メイン2DエンジンのOBJ
			GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット
			GX_VRAM_SUB_OBJ_16_I,			// サブ2DエンジンのOBJ
			GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット
			GX_VRAM_TEX_0_A,				// テクスチャイメージスロット
			GX_VRAM_TEXPLTT_01_FG			// テクスチャパレットスロット
		};
		GF_Disp_SetBank( &vramSetTable );	//H32が余り。サブBG面の拡張パレットとして当てられる

		//VRAMクリア
		MI_CpuClear32((void*)HW_BG_VRAM, HW_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_BG_VRAM, HW_DB_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_OBJ_VRAM, HW_OBJ_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_OBJ_VRAM, HW_DB_OBJ_VRAM_SIZE);
	}

	// BG SYSTEM
	{
		GF_BGL_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
		};
		GF_BGL_InitBG( &BGsys_data );
	}

	//メイン画面フレーム設定
	{
		GF_BGL_BGCNT_HEADER TextBgCntDat[] = {
			///<FRAME1_M	ウィンドウ
			{
				0, 0, 0x1000, 0, GF_BGL_SCRSIZ_512x256, GX_BG_COLORMODE_16,
//				0, 0, 0x0800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01,
				2, 0, 0, FALSE
			},
			///<FRAME2_M	背景
			{
				0, 0, 0x2000, 0, GF_BGL_SCRSIZ_512x512, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000, GX_BG_EXTPLTT_01,
				3, 0, 0, FALSE
			},
		};
		GF_BGL_BGControlSet(bgl, GF_BGL_FRAME1_M, &TextBgCntDat[0], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, GF_BGL_FRAME1_M );
		GF_BGL_ScrollSet(bgl, GF_BGL_FRAME1_M, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet(bgl, GF_BGL_FRAME1_M, GF_BGL_SCROLL_Y_SET, 0);
		
		GF_BGL_BGControlSet(bgl, GF_BGL_FRAME2_M, &TextBgCntDat[1], GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear(bgl, GF_BGL_FRAME2_M );
		GF_BGL_ScrollSet(bgl, GF_BGL_FRAME2_M, GF_BGL_SCROLL_X_SET, 0);
		GF_BGL_ScrollSet(bgl, GF_BGL_FRAME2_M, GF_BGL_SCROLL_Y_SET, 0);
	}
}

//--------------------------------------------------------------
/**
 * @brief   初期BMPウィンドウを設定する
 *
 * @param   testsys		演技力管理ワークへのポインタ
 */
//--------------------------------------------------------------
static void GdsTest_DefaultBmpWinAdd(GDS_TEST_SYS *testsys)
{
	int i;
	
	GF_BGL_BmpWinAdd(testsys->bgl, &testsys->win[TEST_BMPWIN_TITLE], GF_BGL_FRAME1_M, 
		1, 1, 32, 2, BMPWIN_TALK_COLOR, BMPWIN_TITLE_CGX_OFFSET);
	GF_BGL_BmpWinAdd(testsys->bgl, &testsys->win[TEST_BMPWIN_LOG], GF_BGL_FRAME1_M, 
		1, 1, 20, 12, BMPWIN_TALK_COLOR, BMPWIN_LOG_CGX_OFFSET);

	for(i = 0; i < TEST_BMPWIN_MAX; i++){
		GF_BGL_BmpWinDataFill(&testsys->win[i], 0xff);
	}
}

//--------------------------------------------------------------
/**
 * @brief   初期BMPウィンドウを削除
 *
 * @param   testsys		
 */
//--------------------------------------------------------------
static void GdsTest_DefaultBmpWinDel(GDS_TEST_SYS *testsys)
{
	int i;
	
	//BMP開放
	for(i = 0; i < TEST_BMPWIN_MAX; i++){
		GF_BGL_BmpWinDel(&testsys->win[i]);
	}
}

//--------------------------------------------------------------
/**
 * @brief   メインループの最後に行うシステム関連の更新処理
 *
 * @param   tcb			TCBへのポインタ
 * @param   work		testsys
 */
//--------------------------------------------------------------
static void GdsTestUpdate(TCB_PTR tcb, void *work)
{
	GDS_TEST_SYS *testsys = work;
}

//--------------------------------------------------------------
/**
 * @brief	VBLANK関数
 *
 * @param	work	演技力部門管理ワークへのポインタ
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void GdsTestVBlank(void *work)
{
	GDS_TEST_SYS *testsys = work;
	
	DoVramTransferManager();	// Vram転送マネージャー実行
	
	GF_BGL_VBlankFunc(testsys->bgl);
	
	OS_SetIrqCheckFlag( OS_IE_V_BLANK);
}

//--------------------------------------------------------------
/**
 * @brief   GDSRAPからコールバックで呼ばれる巨大エラーメッセージ表示関数
 *
 * @param   work			Initで設定していたコールバック時に一緒に渡されるワークへのポインタ
 * @param   error_msg		メッセージデータへのポインタ(NULLの場合はウィンドウを閉じる)
 */
//--------------------------------------------------------------
static void GdsTest_Callback_ErrorWideMsgPrint(void *work, STRBUF *error_msg)
{
	GDS_TEST_SYS *testsys = work;
	
	if(error_msg != NULL){
		// ウインドウ枠描画＆メッセージ領域クリア
		GF_BGL_BmpWinDataFill(&testsys->win[TEST_BMPWIN_LOG], 15 );
		// 文字列描画開始
		testsys->msg_index = GF_STR_PrintSimple( &testsys->win[TEST_BMPWIN_LOG], FONT_TALK,
												error_msg, 0, 0, MSG_ALLPUT, NULL);
	}
	else{
		//ウィンドウを閉じる
		GF_BGL_BmpWinDataFill(&testsys->win[TEST_BMPWIN_LOG], 15 );
	}
}

//--------------------------------------------------------------
/**
 * @brief   メイン
 *
 * @param   testsys		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
static int GdsTest_Main(GDS_TEST_SYS *testsys)
{
	GDS_RAP_ERROR_INFO *error_info;
	enum{
		SEQ_WIFI_CONNECT,
		
		SEQ_DRESS_UPLOAD,
		SEQ_DRESS_UPLOAD_ERROR_CHECK,
		
		SEQ_DRESS_DOWNLOAD,
		SEQ_DRESS_DOWNLOAD_ERROR_CHECK,

		SEQ_BOX_UPLOAD,
		SEQ_BOX_UPLOAD_ERROR_CHECK,
		
		SEQ_BOX_DOWNLOAD,
		SEQ_BOX_DOWNLOAD_ERROR_CHECK,
		
		SEQ_RANKING_TYPE_DOWNLOAD,
		SEQ_RANKING_TYPE_DOWNLOAD_ERROR_CHECK,
		
		SEQ_RANKING_UPDATE,
		SEQ_RANKING_UPDATE_ERROR_CHECK,
		
		SEQ_VIDEO_UPLOAD,
		SEQ_VIDEO_UPLOAD_ERROR_CHECK,
		
		SEQ_VIDEO_SEARCH,
		SEQ_VIDEO_SEARCH_ERROR_CHECK,
		
		SEQ_VIDEO_DATA_GET,
		SEQ_VIDEO_DATA_GET_ERROR_CHECK,
		
		SEQ_VIDEO_FAVORITE,
		SEQ_VIDEO_FAVORITE_ERROR_CHECK,
		
		SEQ_WIFI_CLEANUP,
		
		SEQ_EXIT,
	};
	
	if(GDSRAP_MoveStatusAllCheck(&testsys->gdsrap) == TRUE){
		switch(testsys->seq){
		case SEQ_WIFI_CONNECT:	//WIFI接続
			//GDSRAP_ProccessReq(&testsys->gdsrap, GDSRAP_PROCESS_REQ_INTERNET_CONNECT);
			testsys->seq++;
			break;
		
		case SEQ_DRESS_UPLOAD:	//ドレスアップ送信
			if(GDSRAP_Tool_Send_DressupUpload(&testsys->gdsrap, testsys->gds_profile_ptr, 
					GDS_DEBUG_DRESSUP_GET_DummyDressData(testsys->sv)) == TRUE){
				testsys->seq++;
			}
			break;
		case SEQ_DRESS_UPLOAD_ERROR_CHECK:
			if(GDSRAP_ErrorInfoGet(&testsys->gdsrap, &error_info) == TRUE){
				//エラー発生時の処理
				//エラーメッセージの表示等はコールバックで行われるので、
				//ここでは表示後の処理。(アプリ終了とか特定のメニューに戻すとか)
				testsys->seq++;
			}
			else{
				testsys->seq++;
			}
			break;
			
		case SEQ_DRESS_DOWNLOAD:	//ドレスアップダウンロード
			if(GDSRAP_Tool_Send_DressupDownload(&testsys->gdsrap, 387) == TRUE){
				testsys->seq++;
			}
			break;
		case SEQ_DRESS_DOWNLOAD_ERROR_CHECK:
			if(GDSRAP_ErrorInfoGet(&testsys->gdsrap, &error_info) == TRUE){
				//エラー発生時の処理
				//エラーメッセージの表示等はコールバックで行われるので、
				//ここでは表示後の処理。(アプリ終了とか特定のメニューに戻すとか)
				testsys->seq++;
			}
			else{
				testsys->seq++;
			}
			break;

		case SEQ_BOX_UPLOAD:	//ボックスショット送信
			if(GDSRAP_Tool_Send_BoxshotUpload(&testsys->gdsrap, 3, testsys->gds_profile_ptr, 
					SaveData_GetBoxData(testsys->sv), 0) == TRUE){
				testsys->seq++;
			}
			break;
		case SEQ_BOX_UPLOAD_ERROR_CHECK:
			if(GDSRAP_ErrorInfoGet(&testsys->gdsrap, &error_info) == TRUE){
				//エラー発生時の処理
				//エラーメッセージの表示等はコールバックで行われるので、
				//ここでは表示後の処理。(アプリ終了とか特定のメニューに戻すとか)
				testsys->seq++;
			}
			else{
				testsys->seq++;
			}
			break;

		case SEQ_BOX_DOWNLOAD:	//ボックスショットダウンロード
			if(GDSRAP_Tool_Send_BoxshotDownload(&testsys->gdsrap, 3) == TRUE){
				testsys->seq++;
			}
			break;
		case SEQ_BOX_DOWNLOAD_ERROR_CHECK:
			if(GDSRAP_ErrorInfoGet(&testsys->gdsrap, &error_info) == TRUE){
				//エラー発生時の処理
				//エラーメッセージの表示等はコールバックで行われるので、
				//ここでは表示後の処理。(アプリ終了とか特定のメニューに戻すとか)
				testsys->seq++;
			}
			else{
				testsys->seq++;
			}
			break;

		case SEQ_RANKING_TYPE_DOWNLOAD:	//
			if(GDSRAP_Tool_Send_RankingTypeDownload(&testsys->gdsrap) == TRUE){
				testsys->seq++;
			}
			break;
		case SEQ_RANKING_TYPE_DOWNLOAD_ERROR_CHECK:
			if(GDSRAP_ErrorInfoGet(&testsys->gdsrap, &error_info) == TRUE){
				//エラー発生時の処理
				//エラーメッセージの表示等はコールバックで行われるので、
				//ここでは表示後の処理。(アプリ終了とか特定のメニューに戻すとか)
				testsys->seq++;
			}
			else{
				testsys->seq++;
			}
			break;

		case SEQ_RANKING_UPDATE:	//
			{
				GT_RANKING_MYDATA mydata[GT_RANKING_WEEK_NUM];
				int i;
				
				for(i = 0; i < GT_RANKING_WEEK_NUM; i++){	//自分のデータでっちあげ
					mydata[i].ranking_type = testsys->ranking_type[i];
					mydata[i].score = gf_rand();
				}
				
				if(GDSRAP_Tool_Send_RankingUpdate(
						&testsys->gdsrap, testsys->gds_profile_ptr, mydata) == TRUE){
					testsys->seq++;
				}
			}
			break;
		case SEQ_RANKING_UPDATE_ERROR_CHECK:
			if(GDSRAP_ErrorInfoGet(&testsys->gdsrap, &error_info) == TRUE){
				//エラー発生時の処理
				//エラーメッセージの表示等はコールバックで行われるので、
				//ここでは表示後の処理。(アプリ終了とか特定のメニューに戻すとか)
				testsys->seq++;
			}
			else{
				testsys->seq++;
			}
			break;

		case SEQ_VIDEO_UPLOAD:	//
			{
				LOAD_RESULT result;
				
				BattleRec_Load(testsys->sv, testsys->heap_id, &result, NULL, LOADDATA_MYREC);
				
				if(GDSRAP_Tool_Send_BattleVideoUpload(
						&testsys->gdsrap, testsys->gds_profile_ptr) == TRUE){
					testsys->seq++;
				}
			}
			break;
		case SEQ_VIDEO_UPLOAD_ERROR_CHECK:
			BattleRec_Exit();
			if(GDSRAP_ErrorInfoGet(&testsys->gdsrap, &error_info) == TRUE){
				//エラー発生時の処理
				//エラーメッセージの表示等はコールバックで行われるので、
				//ここでは表示後の処理。(アプリ終了とか特定のメニューに戻すとか)
				testsys->seq++;
			}
			else{
				testsys->seq++;
			}
			break;

		case SEQ_VIDEO_SEARCH:	//
			//最新30件
			if(GDSRAP_Tool_Send_BattleVideoNewDownload(&testsys->gdsrap) == TRUE){
				testsys->seq++;
			}
			break;
		case SEQ_VIDEO_SEARCH_ERROR_CHECK:
			if(GDSRAP_ErrorInfoGet(&testsys->gdsrap, &error_info) == TRUE){
				//エラー発生時の処理
				//エラーメッセージの表示等はコールバックで行われるので、
				//ここでは表示後の処理。(アプリ終了とか特定のメニューに戻すとか)
				testsys->seq++;
			}
			else{
				testsys->seq++;
			}
			break;

		case SEQ_VIDEO_DATA_GET:	//
			if(GDSRAP_Tool_Send_BattleVideo_DataDownload(
					&testsys->gdsrap, testsys->data_number) == TRUE){
				testsys->seq++;
			}
			break;
		case SEQ_VIDEO_DATA_GET_ERROR_CHECK:
			if(GDSRAP_ErrorInfoGet(&testsys->gdsrap, &error_info) == TRUE){
				//エラー発生時の処理
				//エラーメッセージの表示等はコールバックで行われるので、
				//ここでは表示後の処理。(アプリ終了とか特定のメニューに戻すとか)
				testsys->seq++;
			}
			else{
				testsys->seq++;
			}
			break;

		case SEQ_VIDEO_FAVORITE:	//
			if(GDSRAP_Tool_Send_BattleVideo_DataDownload(
					&testsys->gdsrap, testsys->data_number) == TRUE){
				testsys->seq++;
			}
			break;
		case SEQ_VIDEO_FAVORITE_ERROR_CHECK:
			if(GDSRAP_ErrorInfoGet(&testsys->gdsrap, &error_info) == TRUE){
				//エラー発生時の処理
				//エラーメッセージの表示等はコールバックで行われるので、
				//ここでは表示後の処理。(アプリ終了とか特定のメニューに戻すとか)
				testsys->seq++;
			}
			else{
				testsys->seq++;
			}
			break;

		case SEQ_WIFI_CLEANUP:	//WIFI切断
			//GDSRAP_ProccessReq(&testsys->gdsrap, GDSRAP_PROCESS_REQ_INTERNET_CLEANUP);
			testsys->seq++;
			break;
		
		case SEQ_EXIT:
			return FALSE;
		}
	}
	
	GDSRAP_Main(&testsys->gdsrap);
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   メイン(ドレスアップに全ポケモンアップロード
 *
 * @param   testsys		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
static int GdsTest_DressUpload(GDS_TEST_SYS *testsys)
{
	DRESS_ALL_UP *dau = &testsys->dau;
	GDS_RAP_ERROR_INFO *error_info;
	enum{
		SEQ_INIT,
		
		SEQ_POKE_CREATE,
		SEQ_DRESS_UPLOAD,
		SEQ_DRESS_UPLOAD_ERROR_CHECK,

		SEQ_EXIT,
		SEQ_TRG_WAIT,
	};
	// 文字列取得
	STRBUF *tempbuf, *destbuf;

	if(GDSRAP_MoveStatusAllCheck(&testsys->gdsrap) == TRUE){
		switch(testsys->seq){
		case SEQ_INIT:
			dau->monsno = 1;
			dau->pp = PokemonParam_AllocWork(testsys->heap_id);
			dau->dummy_imc = ImcSaveData_TelevisionAllocWork(testsys->heap_id);
			testsys->seq++;
			//break;
		
		case SEQ_POKE_CREATE:
			PokeParaSetPowRnd(dau->pp, dau->monsno, 20, 1234, gf_rand());//9876);
			PokeParaPut(dau->pp, ID_PARA_oyaname, MyStatus_GetMyName(SaveData_GetMyStatus(testsys->sv)));
			Debug_ImcSaveData_MakeTelevisionDummyData(dau->dummy_imc, dau->pp, 5);
			testsys->seq++;
			//break;
			
		case SEQ_DRESS_UPLOAD:	//ドレスアップ送信
			if(GDSRAP_Tool_Send_DressupUpload(&testsys->gdsrap, testsys->gds_profile_ptr, 
					dau->dummy_imc) == TRUE){
				DEBUG_GDSRAP_SaveFlagReset(&testsys->gdsrap);
				OS_TPrintf("monsno = %d番 送信中 %d件目\n", dau->monsno, dau->count+1);
				// ウインドウ枠描画＆メッセージ領域クリア
				GF_BGL_BmpWinDataFill(&testsys->win[TEST_BMPWIN_TITLE], 15 );
				// 文字列描画開始
				WORDSET_RegisterPokeMonsName(testsys->wordset, 0, PPPPointerGet(dau->pp));
				WORDSET_RegisterNumber(testsys->wordset, 1, dau->count+1, 1, 
					NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);
				destbuf = STRBUF_Create(256, testsys->heap_id);
				tempbuf = MSGMAN_AllocString(testsys->msgman, DMMSG_GDS_DRESS_UPLOAD);
				WORDSET_ExpandStr( testsys->wordset, destbuf, tempbuf);
				GF_STR_PrintSimple( &testsys->win[TEST_BMPWIN_TITLE], 
					FONT_TALK, destbuf, 0, 0, MSG_ALLPUT, NULL);
				STRBUF_Delete(destbuf);
				STRBUF_Delete(tempbuf);
				testsys->seq++;
			}
			break;
		case SEQ_DRESS_UPLOAD_ERROR_CHECK:
			if(GDSRAP_ErrorInfoGet(&testsys->gdsrap, &error_info) == TRUE){
				//エラー発生時の処理
				//エラーメッセージの表示等はコールバックで行われるので、
				//ここでは表示後の処理。(アプリ終了とか特定のメニューに戻すとか)
				OS_TPrintf("エラーが発生しました\n");
				tempbuf = MSGMAN_AllocString(testsys->msgman, DMMSG_GDS_DRESS_UPLOAD_ERROR);
				GF_STR_PrintSimple( &testsys->win[TEST_BMPWIN_TITLE], 
					FONT_TALK, tempbuf, 0, 0, MSG_ALLPUT, NULL);
				STRBUF_Delete(tempbuf);
				testsys->seq = SEQ_TRG_WAIT;
			}
			else{
				dau->count++;
				if(dau->count >= BR_DRESS_VIEW_MAX){
					dau->monsno++;
					dau->count = 0;
					if(dau->monsno > MONSNO_END-1){	//-1=アルセウスを除く(不正扱いされるので)
						testsys->seq++;
					}
					else{
						testsys->seq = SEQ_POKE_CREATE;
					}
				}
				else{
					testsys->seq = SEQ_POKE_CREATE;
				}
			}
			break;
			
		case SEQ_EXIT:
			sys_FreeMemoryEz(dau->dummy_imc);
			sys_FreeMemoryEz(dau->pp);
			return FALSE;
		
		case SEQ_TRG_WAIT:
			if(sys.trg & PAD_BUTTON_A){
				testsys->seq = SEQ_EXIT;
			}
			break;
		}
	}
	
	GDSRAP_Main(&testsys->gdsrap);
	return TRUE;
}

//==============================================================================
//
//	通信データレスポンスコールバック関数
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   ドレスアップアップロード時のレスポンスコールバック
 *
 * @param   work			
 * @param   error_info		エラー情報
 */
//--------------------------------------------------------------
static void Response_DressRegist(void *work, const GDS_RAP_ERROR_INFO *error_info)
{
	GDS_TEST_SYS *testsys = work;
	
	OS_TPrintf("ドレスアップショットのアップロードレスポンス取得\n");
	if(error_info->occ == TRUE){
		//TRUEならばエラー発生しているので、ここでメニューを戻すとかアプリ終了モードへ移行とかする
	}
	else{
		//正常時ならば受信バッファからデータ取得などを行う
		//アップロードの場合は特に必要なし
	}
}

//--------------------------------------------------------------
/**
 * @brief   ドレスアップダウンロード時のレスポンスコールバック
 *
 * @param   work			
 * @param   error_info		エラー情報
 */
//--------------------------------------------------------------
static void Response_DressGet(void *work, const GDS_RAP_ERROR_INFO *error_info)
{
	GDS_TEST_SYS *testsys = work;
	
	OS_TPrintf("ドレスアップショットのダウンロードレスポンス取得\n");
	if(error_info->occ == TRUE){
		//TRUEならばエラー発生しているので、ここでメニューを戻すとかアプリ終了モードへ移行とかする
	}
	else{
		//正常時ならば受信バッファからデータ取得などを行う
		//アップロードの場合は特に必要なし

	//	GDS_RAP_RESPONSE_DressupShot_Download_RecvPtr_Set(GDS_RAP_WORK *gdsrap, GT_GDS_DRESS_RECV **dress_array, int array_max);
	}
}

//--------------------------------------------------------------
/**
 * @brief   ボックスショットアップロード時のレスポンスコールバック
 *
 * @param   work			
 * @param   error_info		エラー情報
 */
//--------------------------------------------------------------
static void Response_BoxRegist(void *work, const GDS_RAP_ERROR_INFO *error_info)
{
	GDS_TEST_SYS *testsys = work;
	
	OS_TPrintf("ボックスショットのアップロードレスポンス取得\n");
	if(error_info->occ == TRUE){
		//TRUEならばエラー発生しているので、ここでメニューを戻すとかアプリ終了モードへ移行とかする
	}
	else{
		//正常時ならば受信バッファからデータ取得などを行う
		//アップロードの場合は特に必要なし
	}
}

//--------------------------------------------------------------
/**
 * @brief   ボックスショットダウンロード時のレスポンスコールバック
 *
 * @param   work			
 * @param   error_info		エラー情報
 */
//--------------------------------------------------------------
static void Response_BoxGet(void *work, const GDS_RAP_ERROR_INFO *error_info)
{
	GDS_TEST_SYS *testsys = work;
	
	OS_TPrintf("ボックスショットのダウンロードレスポンス取得\n");
	if(error_info->occ == TRUE){
		//TRUEならばエラー発生しているので、ここでメニューを戻すとかアプリ終了モードへ移行とかする
	}
	else{
		//正常時ならば受信バッファからデータ取得などを行う
		//アップロードの場合は特に必要なし

	//	int GDS_RAP_RESPONSE_Boxshot_Download_RecvPtr_Set(GDS_RAP_WORK *gdsrap, GT_BOX_SHOT_RECV **box_array, int array_max);
	}
}

//--------------------------------------------------------------
/**
 * @brief   開催中のランキングタイプダウンロード時のレスポンスコールバック
 *
 * @param   work			
 * @param   error_info		エラー情報
 */
//--------------------------------------------------------------
static void Response_RankingType(void *work, const GDS_RAP_ERROR_INFO *error_info)
{
	GDS_TEST_SYS *testsys = work;
	
	OS_TPrintf("開催中のランキングタイプのダウンロードレスポンス取得\n");
	if(error_info->occ == TRUE){
		//TRUEならばエラー発生しているので、ここでメニューを戻すとかアプリ終了モードへ移行とかする
	}
	else{
		//正常時ならば受信バッファからデータ取得などを行う
		GT_RANKING_TYPE_RECV *recv_type;
		int i;
		
		GDS_RAP_RESPONSE_RankingType_Download_RecvPtr_Set(&testsys->gdsrap, &recv_type);
		for(i = 0; i < GT_RANKING_WEEK_NUM; i++){
			OS_TPrintf("開催中のタイプ その%d = %d\n", i, recv_type->ranking_type[i]);
			testsys->ranking_type[i] = recv_type->ranking_type[i];
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   ランキング更新時のレスポンスコールバック
 *
 * @param   work			
 * @param   error_info		エラー情報
 */
//--------------------------------------------------------------
static void Response_RankingUpdate(void *work, const GDS_RAP_ERROR_INFO *error_info)
{
	GDS_TEST_SYS *testsys = work;
	
	OS_TPrintf("ランキング更新のレスポンス取得\n");
	if(error_info->occ == TRUE){
		//TRUEならばエラー発生しているので、ここでメニューを戻すとかアプリ終了モードへ移行とかする
	}
	else{
		//正常時ならば受信バッファからデータ取得などを行う
		GT_LAST_WEEK_RANKING_ALL_RECV *last_week;
		GT_THIS_WEEK_RANKING_DATA_ALL_RECV *this_week;
		int i;
		
		GDS_RAP_RESPONSE_RankingUpdate_Download_RecvPtr_Set(
			&testsys->gdsrap, &last_week, &this_week);
		for(i = 0; i < GT_RANKING_WEEK_NUM; i++){
			OS_TPrintf("先週のランキングタイプ その%d=%d", i, last_week->ranking_data[i].ranking_type);
		}
		for(i = 0; i < GT_RANKING_WEEK_NUM; i++){
			OS_TPrintf("今週のランキングタイプ その%d=%d", i, this_week->ranking_data[i].ranking_type);
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief   バトルビデオ登録時のレスポンスコールバック
 *
 * @param   work			
 * @param   error_info		エラー情報
 */
//--------------------------------------------------------------
static void Response_BattleVideoRegist(void *work, const GDS_RAP_ERROR_INFO *error_info)
{
	GDS_TEST_SYS *testsys = work;
	
	OS_TPrintf("バトルビデオ登録時のダウンロードレスポンス取得\n");
	if(error_info->occ == TRUE){
		//TRUEならばエラー発生しているので、ここでメニューを戻すとかアプリ終了モードへ移行とかする
		switch(error_info->result){
		case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_ERROR_AUTH:		//!< ユーザー認証エラー
			OS_TPrintf("バトルビデオデータ取得受信エラー！:ユーザー認証エラー\n");
			break;
		case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_ERROR_ILLEGALCODE:	//!< コードエラー
			OS_TPrintf("バトルビデオデータ取得受信エラー！:コードエラー\n");
			break;
		case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_ERROR_UNKNOWN:		//!< その他エラー
		default:
			OS_TPrintf("バトルビデオデータ取得受信エラー！:その他のエラー\n");
			break;
		}
	}
	else{
		//正常時ならば受信バッファからデータ取得などを行う
		//アップロードの場合は特に必要なし
		u64 data_number;
		
		data_number = GDS_RAP_RESPONSE_BattleVideo_Upload_DataGet(&testsys->gdsrap);
		testsys->data_number = data_number;
		OS_TPrintf("登録コード＝%d\n", data_number);
	}
}

//--------------------------------------------------------------
/**
 * @brief   バトルビデオ検索時のレスポンスコールバック
 *
 * @param   work			
 * @param   error_info		エラー情報
 */
//--------------------------------------------------------------
static void Response_BattleVideoSearch(void *work, const GDS_RAP_ERROR_INFO *error_info)
{
	GDS_TEST_SYS *testsys = work;
	
	OS_TPrintf("バトルビデオ検索のダウンロードレスポンス取得\n");
	if(error_info->occ == TRUE){
		//TRUEならばエラー発生しているので、ここでメニューを戻すとかアプリ終了モードへ移行とかする
	}
	else{
		//正常時ならば受信バッファからデータ取得などを行う
		//アップロードの場合は特に必要なし

	//	int GDS_RAP_RESPONSE_Boxshot_Download_RecvPtr_Set(GDS_RAP_WORK *gdsrap, GT_BOX_SHOT_RECV **box_array, int array_max);
	}
}

//--------------------------------------------------------------
/**
 * @brief   バトルビデオデータダウンロード時のレスポンスコールバック
 *
 * @param   work			
 * @param   error_info		エラー情報
 */
//--------------------------------------------------------------
static void Response_BattleVideoDataGet(void *work, const GDS_RAP_ERROR_INFO *error_info)
{
	GDS_TEST_SYS *testsys = work;
	
	OS_TPrintf("バトルビデオデータ取得のダウンロードレスポンス取得\n");
	if(error_info->occ == TRUE){
		//TRUEならばエラー発生しているので、ここでメニューを戻すとかアプリ終了モードへ移行とかする
	}
	else{
		//正常時ならば受信バッファからデータ取得などを行う
		//アップロードの場合は特に必要なし

	//	int GDS_RAP_RESPONSE_Boxshot_Download_RecvPtr_Set(GDS_RAP_WORK *gdsrap, GT_BOX_SHOT_RECV **box_array, int array_max);
	}
}

//--------------------------------------------------------------
/**
 * @brief   バトルビデオお気に入り登録のレスポンスコールバック
 *
 * @param   work			
 * @param   error_info		エラー情報
 */
//--------------------------------------------------------------
static void Response_BattleVideoFavorite(void *work, const GDS_RAP_ERROR_INFO *error_info)
{
	GDS_TEST_SYS *testsys = work;
	
	OS_TPrintf("バトルビデオお気に入り登録のダウンロードレスポンス取得\n");
	if(error_info->occ == TRUE){
		//TRUEならばエラー発生しているので、ここでメニューを戻すとかアプリ終了モードへ移行とかする
	}
	else{
		//正常時ならば受信バッファからデータ取得などを行う
		//アップロードの場合は特に必要なし

	//	int GDS_RAP_RESPONSE_Boxshot_Download_RecvPtr_Set(GDS_RAP_WORK *gdsrap, GT_BOX_SHOT_RECV **box_array, int array_max);
	}
}

#endif //PM_DEBUG +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
