//==============================================================================
/**
 * @file	gds_main.c
 * @brief	GDSモード：PROC制御
 * @author	matsuda
 * @date	2008.01.17(木)
 */
//==============================================================================
#include "common.h"
#include <dwc.h>
#include "wifi/dwc_rap.h"
#include "savedata\savedata.h"
#include "gds_battle_rec.h"
#include "gds_ranking.h"
#include "gds_boxshot.h"
#include "gds_dressup.h"

#include "savedata/imageclip_data.h"
#include "poketool/boxdata.h"
#include "poketool/monsno.h"
#include "gflib/strbuf_family.h"
#include "savedata/gds_profile.h"

#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "system/msgdata_util.h"
#include "system/wordset.h"
#include "msgdata/msg.naix"
#include "system/fontproc.h"
#include "gflib/strbuf_family.h"

#include "communication\comm_system.h"
#include "communication\comm_state.h"
#include "communication\comm_def.h"
#include "communication/wm_icon.h"
#include "communication\communication.h"

#include "gds_battle_rec.h"
#include "gds_ranking.h"
#include "gds_boxshot.h"
#include "gds_dressup.h"

#include "gds_rap.h"
#include "gds_rap_response.h"
#include "gds_data_conv.h"

#include "system/snd_tool.h"
#include "system/bmp_list.h"
#include "system/pmfprint.h"
#include "savedata/config.h"
#include "savedata\system_data.h"
#include "system/bmp_menu.h"
#include "system/procsys.h"
#include "system/wipe.h"
#include "system/window.h"

#include "application/gds_main.h"
#include "application/br_sys.h"


//==============================================================================
//	定数定義
//==============================================================================
#define MYDWC_HEAPSIZE		0x20000

///GDSプロック制御が使用するヒープサイズ
#define GDSPROC_HEAP_SIZE		(MYDWC_HEAPSIZE + 0x8000)

//==============================================================================
//	グローバル変数
//==============================================================================
static NNSFndHeapHandle _wtHeapHandle;

//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static void GdsMain_CommInitialize(GDSPROC_MAIN_WORK *gmw);
static void GdsMain_CommFree(GDSPROC_MAIN_WORK *gmw);
static void *AllocFunc( DWCAllocType name, u32   size, int align );
static void FreeFunc(DWCAllocType name, void* ptr,  u32 size);

//==============================================================================
//	データ
//==============================================================================
///WIFI接続画面プロセス定義データ
static const PROC_DATA GdsConnectProcData = {
	GdsConnectProc_Init,
	GdsConnectProc_Main,
	GdsConnectProc_End,
	NO_OVERLAY_ID,//FS_OVERLAY_ID(gds_comm),
};

#ifdef PM_DEBUG
//※check テスト用
extern PROC_RESULT GdsTestProc_Init( PROC * proc, int * seq );
extern PROC_RESULT GdsTestProc_Main( PROC * proc, int * seq );
extern PROC_RESULT GdsTestProc_End( PROC * proc, int * seq );
extern PROC_RESULT GdsTestDressUpProc_Main( PROC * proc, int * seq );
#endif


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
PROC_RESULT GdsMainProc_Init( PROC * proc, int * seq )
{
	GDSPROC_MAIN_WORK *gmw;
	
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

	sys_CreateHeap(HEAPID_BASE_APP, HEAPID_GDS_MAIN, GDSPROC_HEAP_SIZE);

	gmw = PROC_AllocWork(proc, sizeof(GDSPROC_MAIN_WORK), HEAPID_GDS_MAIN );
	MI_CpuClear8(gmw, sizeof(GDSPROC_MAIN_WORK));
	gmw->proc_param = PROC_GetParentWork(proc);

	// サウンドデータロード(フィールド)
	Snd_DataSetByScene( SND_SCENE_P2P, SEQ_WIFILOBBY, 1 );

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
PROC_RESULT GdsMainProc_Main( PROC * proc, int * seq )
{
	GDSPROC_MAIN_WORK * gmw  = PROC_GetWork( proc );
	enum{
		SEQ_INIT_DPW,
		SEQ_INIT_DPW_WAIT,
		
		SEQ_WIFI_CONNECT,
		SEQ_WIFI_CONNECT_MAIN,
		
		SEQ_BATTLE_RECORDER,
		SEQ_BATTLE_RECORDER_MAIN,
		
		SEQ_WIFI_CLEANUP,
		SEQ_WIFI_CLEANUP_MAIN,
		
		SEQ_END,
	};
	
	switch(*seq){
	case SEQ_INIT_DPW:	//通信ライブラリ初期化
		GdsMain_CommInitialize(gmw);
		*seq = SEQ_INIT_DPW_WAIT;
		break;
	case SEQ_INIT_DPW_WAIT:
		if(CommIsVRAMDInitialize()){
			_wtHeapHandle = gmw->heapHandle;
	
			// wifiメモリ管理関数呼び出し
			DWC_SetMemFunc( AllocFunc, FreeFunc );
			
			gmw->comm_initialize_ok = TRUE;
			(*seq)++;
		}
		break;
		
	case SEQ_WIFI_CONNECT:	//WIFI接続
		gmw->sub_proc = PROC_Create(&GdsConnectProcData, gmw, HEAPID_GDS_MAIN);
		(*seq)++;
		break;
	case SEQ_WIFI_CONNECT_MAIN:
		if(ProcMain(gmw->sub_proc) == TRUE){
			PROC_Delete(gmw->sub_proc);
			if(gmw->ret_connect == TRUE){
				gmw->connect_success = TRUE;
				(*seq)++;
			}
			else{
				*seq = SEQ_END;
			}
		}
		break;
	
	case SEQ_BATTLE_RECORDER:	//バトルレコーダー(GDSモード)
	#ifdef PM_DEBUG
		{
			if(gmw->proc_param->gds_mode == 0xff){
				static const PROC_DATA GdsTestProcData = {
					GdsTestProc_Init,
					GdsTestDressUpProc_Main,	//GdsTestProc_Main,
					GdsTestProc_End,
					NO_OVERLAY_ID,//FS_OVERLAY_ID(gds_comm),
				};
				gmw->sub_proc = PROC_Create(&GdsTestProcData, gmw->proc_param->savedata, 
					HEAPID_GDS_MAIN);
				(*seq)++;
				break;
			}
		}
	#endif
		
		{
			const PROC_DATA *br_proc;
			
			br_proc = BattleRecoder_ProcDataGet(gmw->proc_param->gds_mode);
			gmw->sub_proc = PROC_Create(br_proc, gmw->proc_param->fsys, HEAPID_GDS_MAIN);
			(*seq)++;
		}
		break;
	case SEQ_BATTLE_RECORDER_MAIN:
		if(ProcMain(gmw->sub_proc) == TRUE){
			PROC_Delete(gmw->sub_proc);
			(*seq)++;
		}
		break;
	
	case SEQ_WIFI_CLEANUP:		//WIFI切断
		gmw->sub_proc = PROC_Create(&GdsConnectProcData, gmw, HEAPID_GDS_MAIN);
		(*seq)++;
		break;
	case SEQ_WIFI_CLEANUP_MAIN:
		if(ProcMain(gmw->sub_proc) == TRUE){
			PROC_Delete(gmw->sub_proc);
			gmw->connect_success = FALSE;
			(*seq)++;
		}
		break;

	case SEQ_END:
		return PROC_RES_FINISH;
	}

	if(gmw->comm_initialize_ok == TRUE && gmw->connect_success == TRUE && gmw->ret_connect == TRUE){
		// 受信強度リンクを反映させる
		DWC_UpdateConnection();

		// Dpw_Tr_Main() だけは例外的にいつでも呼べる
	//	Dpw_Tr_Main();

		// 通信状態を確認してアイコンの表示を変える
		WirelessIconEasy_SetLevel( WM_LINK_LEVEL_3 - DWC_GetLinkLevel() );
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
PROC_RESULT GdsMainProc_End( PROC * proc, int * seq )
{
	GDSPROC_MAIN_WORK * gmw  = PROC_GetWork( proc );

	GdsMain_CommFree(gmw);
	
	//GDSプロック呼び出しパラメータ解放
	sys_FreeMemoryEz(gmw->proc_param);
	
	PROC_FreeWork( proc );				// PROCワーク開放

	sys_DeleteHeap( HEAPID_GDS_MAIN );

	return PROC_RES_FINISH;
}


//--------------------------------------------------------------
/**
 * @brief   通信ライブラリ関連の初期化処理
 *
 * @param   gmw		
 */
//--------------------------------------------------------------
static void GdsMain_CommInitialize(GDSPROC_MAIN_WORK *gmw)
{
	if(gmw->comm_initialize_ok == FALSE){
		OS_TPrintf("Comm初期化開始\n");
		
		//世界交換のWifi通信命令を使用するためオーバーレイを読み出す(dpw_tr.c等)
//		Overlay_Load(FS_OVERLAY_ID(worldtrade), OVERLAY_LOAD_NOT_SYNCHRONIZE);

		// DWCライブラリ（Wifi）に渡すためのワーク領域を確保
		gmw->heapPtr    = sys_AllocMemory(HEAPID_GDS_MAIN, MYDWC_HEAPSIZE + 32);
		gmw->heapHandle = NNS_FndCreateExpHeap( (void *)( ((u32)gmw->heapPtr + 31) / 32 * 32 ), MYDWC_HEAPSIZE);

		//DWCオーバーレイ読み込み
		DwcOverlayStart();
		DpwCommonOverlayStart();
		// イクニューモン転送
		CommVRAMDInitialize();

		OS_TPrintf("Comm初期化終了\n");
	}
}

//--------------------------------------------------------------
/**
 * @brief   通信ライブラリ関連の解放処理
 *
 * @param   gmw		
 */
//--------------------------------------------------------------
static void GdsMain_CommFree(GDSPROC_MAIN_WORK *gmw)
{
	if(gmw->comm_initialize_ok == TRUE){
		OS_TPrintf("Comm解放開始\n");
		
		NNS_FndDestroyExpHeap(gmw->heapHandle);
		sys_FreeMemoryEz( gmw->heapPtr );
		DpwCommonOverlayEnd();
		DwcOverlayEnd();

		// イクニューモン解放
		CommVRAMDFinalize();

//		Overlay_UnloadID(FS_OVERLAY_ID(worldtrade));
		
		gmw->comm_initialize_ok = FALSE;

		OS_TPrintf("Comm解放完了\n");
	}
}

/*---------------------------------------------------------------------------*
  メモリ確保関数
 *---------------------------------------------------------------------------*/
static void *AllocFunc( DWCAllocType name, u32   size, int align )
{
#pragma unused( name )
    void * ptr;
    OSIntrMode old;
    old = OS_DisableInterrupts();
    ptr = NNS_FndAllocFromExpHeapEx( _wtHeapHandle, size, align );
    OS_RestoreInterrupts( old );
    if(ptr == NULL){
	}
	
    return ptr;
}

/*---------------------------------------------------------------------------*
  メモリ開放関数
 *---------------------------------------------------------------------------*/
static void FreeFunc(DWCAllocType name, void* ptr,  u32 size)
{
#pragma unused( name, size )
    OSIntrMode old;

    if ( !ptr ) return;
    old = OS_DisableInterrupts();
    NNS_FndFreeToExpHeap( _wtHeapHandle, ptr );
    OS_RestoreInterrupts( old );
}
