//==============================================================================
/**
 * @file	frontier_main.c
 * @brief	フロンティアメイン処理
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
//#include "script.h"
#include "..\fielddata\script\connect_def.h"
#include "system/pm_overlay.h"

#include "system/msgdata.h"
#include "msgdata/msg.naix"
#include "msgdata/msg_debug_matsu.h"

#include "frontier_scr.h"
#include "fss_scene.h"
#include "frontier_main.h"
#include "frontier_map.h"



//==============================================================================
//	
//==============================================================================
FS_EXTERN_OVERLAY(frontier_common);
FS_EXTERN_OVERLAY(factory);
FS_EXTERN_OVERLAY(wifi_2dmapsys);

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
typedef struct _FRONTIER_MAIN_WORK{
	FRONTIER_EX_PARAM *ex_param;		///<フロンティア呼び出し側から渡されるデータへのポインタ
	
	PROC *sub_proc;		///<サブプロックのポインタ
	void *sub_proc_parent_work;	///<サブプロック呼び出し時に渡す上位ポインタ
	FS_SUBPROC_END_FUNC sub_proc_end_func;
	u8 sub_proc_auto_free;	///<サブプロック終了時にワーク自動解放
	
	FSS_PTR fss;		///<スクリプトシステムへのポインタ
	FMAP_PTR fmap;		///<マップシステムへのポインタ
	u8 seq;
	u8 map_status;		///<TRUE:マップシステムが起動している
	u8 map_change;		///<TRUE:マップ切り替えリクエスト発生
	u16 map_change_event_id;	///<マップ切り替え後に起動するイベントID
	
	u8 finish_req;		///<TRUE:終了リクエスト
	
	//キャラ復帰用にリソースやアクターの登録情報をバックアップ
	//マップシステムが破棄されても情報は残るようにバッファはメインに、
	//常駐領域を圧迫しないようにマネージャ関数はfrontier_map.c側に持たせている。
	FSS_CHAR_RESOURCE_DATA char_res[FIELD_OBJ_RESOURCE_MAX];	///<登録リソース管理
	//FSS_CHAR_ACTOR_DATA char_actor[FIELD_OBJ_MAX];				///<登録アクター管理
	FSS_ACTOR_WORK fss_actor[FIELD_OBJ_MAX];					///<登録アクターワーク
	FSS_OBJMOVE_WORK objmove_work[FIELD_OBJ_MAX];				///<OBJ動作ワーク
	
	//配置物アクターのPush,Pop用退避バッファ
	PUSH_ARTICLE_WORK push_article;	///<退避用配置物アクターバッファ
}FRONTIER_MAIN_WORK;


//==============================================================================
//	プロトタイプ宣言
//==============================================================================
PROC_RESULT FrontierMainProc_Init( PROC * proc, int * seq );
PROC_RESULT FrontierMainProc_Main( PROC * proc, int * seq );
PROC_RESULT FrontierMainProc_End( PROC * proc, int * seq );
static void Frontier_MapCreate(FMAIN_PTR fmw);
static void Frontier_MapFree(FMAIN_PTR fmw);
static void Frontier_OverlayLoad(void);
static void Frontier_OverlayUnload(void);
static void Frontier_ManageWorkClear(FMAIN_PTR fmw);


//==============================================================================
//	PROC
//==============================================================================
//プロセス定義データ
const PROC_DATA FrontierMainProcData = {
	FrontierMainProc_Init,
	FrontierMainProc_Main,
	FrontierMainProc_End,
//	FS_OVERLAY_ID(frontier_common),
	NO_OVERLAY_ID,
};


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
PROC_RESULT FrontierMainProc_Init( PROC * proc, int * seq )
{
	FRONTIER_MAIN_WORK *fmw;
	int i;
	
	Frontier_OverlayLoad();
	
	fmw = PROC_AllocWork(proc, sizeof(FRONTIER_MAIN_WORK), HEAPID_WORLD );
	MI_CpuClear8(fmw, sizeof(FRONTIER_MAIN_WORK));
	Frontier_ManageWorkClear(fmw);
	Frontier_ArticlePushBufferInit(fmw);
	
	//外側から最初のシーンIDなどが入っているデータを受け取る
	fmw->ex_param = PROC_GetParentWork(proc);
	GF_ASSERT(fmw->ex_param != NULL);
	
	//※スクリプト設定
	fmw->fss = FSS_SystemCreate(fmw, HEAPID_WORLD, fmw->ex_param->scene_id);
	FSS_ScrTaskAdd(fmw->fss, fmw->ex_param->scene_id, 0);

	//マップ設定
	Frontier_MapCreate(fmw);
	
//	WIPE_SYS_Start(WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK, 
//		WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_WORLD);

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
PROC_RESULT FrontierMainProc_Main( PROC * proc, int * seq )
{
	FRONTIER_MAIN_WORK * fmw  = PROC_GetWork( proc );
	int ret;
	enum{
		SEQ_IN,
		SEQ_MAIN,
		SEQ_OUT,
		
		SEQ_SUB_PROC_INIT,
		SEQ_SUB_PROC_MAIN,
		
		SEQ_MAP_CHANGE,
		SEQ_MAP_CHANGE_LOAD,
	};
	
	switch(*seq){
	case SEQ_IN:
//		if(WIPE_SYS_EndCheck() == TRUE){
			*seq = SEQ_MAIN;
//		}
		break;
	case SEQ_MAIN:
		if(fmw->finish_req == TRUE){
			*seq = SEQ_OUT;
			break;
		}
		if(fmw->map_status == FALSE){
			break;
		}
		if(fmw->map_change == TRUE){
			*seq = SEQ_MAP_CHANGE;
			break;
		}
		
		//※スクリプト実行
		if(FSS_ScriptMain(fmw->fss) == TRUE){
			if(sys.trg & PAD_BUTTON_B){
				*seq = SEQ_OUT;
			}
			
			//キー入力チェック
			
			//敵AI動作
			
		}
		
		if(fmw->sub_proc != NULL){
			*seq = SEQ_SUB_PROC_INIT;
		}
		break;
	case SEQ_OUT:
		return PROC_RES_FINISH;
	
	case SEQ_SUB_PROC_INIT:
		FrontierMap_Push(fmw->fmap);
		Frontier_MapFree(fmw);
		Frontier_OverlayUnload();
		*seq = SEQ_SUB_PROC_MAIN;
		break;
	case SEQ_SUB_PROC_MAIN:
	    if(ProcMain(fmw->sub_proc) == TRUE) {
			PROC_Delete(fmw->sub_proc);
			Frontier_OverlayLoad();
			if(fmw->sub_proc_end_func != NULL){
				fmw->sub_proc_end_func(fmw->sub_proc_parent_work);
			}
			if(fmw->sub_proc_parent_work != NULL && fmw->sub_proc_auto_free == TRUE){
				sys_FreeMemoryEz(fmw->sub_proc_parent_work);
			}
			fmw->sub_proc = NULL;
			fmw->sub_proc_end_func = NULL;
			fmw->sub_proc_parent_work = NULL;
			
			Frontier_MapCreate(fmw);
			FrontierMap_Pop(fmw->fmap);
			*seq = SEQ_MAIN;
		}
		break;
	
	case SEQ_MAP_CHANGE:
		Frontier_MapFree(fmw);
		Frontier_ManageWorkClear(fmw);
		*seq = SEQ_MAP_CHANGE_LOAD;
		break;
	case SEQ_MAP_CHANGE_LOAD:
		Frontier_MapCreate(fmw);
		if(fmw->map_change_event_id == FS_MAPCHANGE_EVENTID_NULL){
			//通常のマップチェンジ(.sはそのまま継続して使用)
			
			FSS_MsgManageSceneChange(fmw->fss, fmw->ex_param->scene_id, HEAPID_WORLD);
		}
		else{	//スクリプトファイル(.s)まるごと切り替える
			FS_PARAM_WORK *pw;
			
			//param_workは継続させる為バックアップ
			pw = FSS_ParamWorkBackup(fmw->fss, HEAPID_WORLD);
			//現在読み込んでいるスクリプトを破棄
			FSS_SystemFree(fmw->fss);
			
			//新しくスクリプト読み込み
			fmw->fss = FSS_SystemCreate(fmw, HEAPID_WORLD, fmw->ex_param->scene_id);
			FSS_ScrTaskAdd(fmw->fss, fmw->ex_param->scene_id, fmw->map_change_event_id);
			//上でバックアップしたparam_workを新しいスクリプトシステムにセットする
			FSS_ParamWorkRecover(fmw->fss, pw);
		}
		fmw->map_change = FALSE;
		*seq = SEQ_MAIN;
		break;
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
PROC_RESULT FrontierMainProc_End( PROC * proc, int * seq )
{
	FRONTIER_MAIN_WORK * fmw = PROC_GetWork( proc );

	//※スクリプト解放
	FSS_SystemFree(fmw->fss);

	//マップ解放
	Frontier_MapFree(fmw);

	PROC_FreeWork(proc);				// ワーク開放
	
	Frontier_OverlayUnload();
	return PROC_RES_FINISH;
}

//--------------------------------------------------------------
/**
 * @brief   マップシステム作成
 *
 * @param   fmain		
 */
//--------------------------------------------------------------
static void Frontier_MapCreate(FMAIN_PTR fmw)
{
	fmw->fmap = FrontierMap_Init(fmw);
	fmw->map_status = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   マップシステム解放
 *
 * @param   fmw		
 */
//--------------------------------------------------------------
static void Frontier_MapFree(FMAIN_PTR fmw)
{
	FrontierMap_End(fmw->fmap);
	fmw->map_status = FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   Push,Pop用の管理用ワークを初期化する
 *
 * @param   fmw		
 */
//--------------------------------------------------------------
static void Frontier_ManageWorkClear(FMAIN_PTR fmw)
{
	int i;
	
	for(i = 0; i < FIELD_OBJ_RESOURCE_MAX; i++){
		fmw->char_res[i].charid = OBJCODEMAX;
	}
	
	MI_CpuClear8(fmw->fss_actor, sizeof(FSS_ACTOR_WORK) * FIELD_OBJ_MAX);
	for(i = 0; i < FIELD_OBJ_MAX; i++){
		fmw->fss_actor[i].param.playid = FIELD_PLAYID_NULL;
	}
}

//--------------------------------------------------------------
/**
 * @brief   オーバーレイロード
 *
 * 結局戦闘画面への移行、復帰時にオーバーレイのロード、アンロードを自前でやらなければいけないので、
 * PROC_DATAでのオーバーレイIDは使用していないで、自力でやる方式を取っています
 */
//--------------------------------------------------------------
static void Frontier_OverlayLoad(void)
{
	Overlay_Load(FS_OVERLAY_ID(frontier_common), OVERLAY_LOAD_NOT_SYNCHRONIZE);
	Overlay_Load(FS_OVERLAY_ID(factory), OVERLAY_LOAD_NOT_SYNCHRONIZE);
	Overlay_Load(FS_OVERLAY_ID(wifi_2dmapsys), OVERLAY_LOAD_NOT_SYNCHRONIZE);
}

//--------------------------------------------------------------
/**
 * @brief   オーバーレイアンロード
 */
//--------------------------------------------------------------
static void Frontier_OverlayUnload(void)
{
	Overlay_UnloadID(FS_OVERLAY_ID(frontier_common));
	Overlay_UnloadID(FS_OVERLAY_ID(factory));
	Overlay_UnloadID(FS_OVERLAY_ID(wifi_2dmapsys));
}

//--------------------------------------------------------------
/**
 * @brief   ExParamを取得する
 *
 * @param   fmain		
 *
 * @retval  ポインタ
 */
//--------------------------------------------------------------
FRONTIER_EX_PARAM * Frontier_ExParamGet(FMAIN_PTR fmain)
{
	return fmain->ex_param;
}

//--------------------------------------------------------------
/**
 * @brief   FMAPアドレスを取得する
 * @param   fmain		
 * @retval  
 */
//--------------------------------------------------------------
FMAP_PTR Frontier_FMapAdrsGet(FMAIN_PTR fmain)
{
	return fmain->fmap;
}

//--------------------------------------------------------------
/**
 * @brief   FSSアドレスを取得する
 * @param   fmain		
 * @retval  
 */
//--------------------------------------------------------------
FSS_PTR Frontier_FSSAdrsGet(FMAIN_PTR fmain)
{
	return fmain->fss;
}

//--------------------------------------------------------------
/**
 * @brief   メイン側が保持しているアプリ用システムワークを取得する
 *
 * @param   fmain		
 *
 * @retval  アプリ用システムワークへのポインタ
 */
//--------------------------------------------------------------
void * Frontier_SysWorkGet(FMAIN_PTR fmain)
{
	return fmain->ex_param->syswork;
}

//--------------------------------------------------------------
/**
 * @brief   アプリ用システムワークをセットする
 *
 * @param   fmain		
 * @param   syswork		アプリ用システムワークへのポインタ
 */
//--------------------------------------------------------------
void Frontier_SysWorkSet(FMAIN_PTR fmain, void *syswork)
{
	fmain->ex_param->syswork = syswork;
}

//--------------------------------------------------------------
/**
 * @brief   サブPROC登録
 *
 * @param   fmain			
 * @param   data			プロセス動作関数
 * @param   parent_work		上位から引き渡すワークへのポインタ
 * @param   auto_free		TRUE:サブPROC終了時に、parent_workで渡されたワークをFreeMemoryする
 * @param   end_func		サブPROC終了時に呼び出す関数(NULL可。 parent_workがただのFreeMemoryで
 * 							解放出来ない時などの、個別解放処理なんかに使ってください
 */
//--------------------------------------------------------------
void Frontier_SubProcSet(FMAIN_PTR fmain, const PROC_DATA * proc_data, void * parent_work, int auto_free, FS_SUBPROC_END_FUNC end_func)
{
	GF_ASSERT(fmain->sub_proc == NULL);
	fmain->sub_proc = PROC_Create(proc_data, parent_work, HEAPID_WORLD);
	fmain->sub_proc_parent_work = parent_work;
	fmain->sub_proc_auto_free = auto_free;
	fmain->sub_proc_end_func = end_func;
}

//--------------------------------------------------------------
/**
 * @brief   終了リクエスト設定
 *
 * @param   fmain		
 */
//--------------------------------------------------------------
void Frontier_FinishReq(FMAIN_PTR fmain)
{
	fmain->finish_req = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   マップ切り替えリクエスト設定
 *
 * @param   fmain		
 * @param   scene_id		切り替え後のシーンID
 * @param   event_id		イベントID指定(指定しない場合はFS_MAPCHANGE_EVENTID_NULL)
 *
 * event_idをFS_MAPCHANGE_EVENTID_NULL以外にした場合は、マップ切り替えで今読み込んでいるスクリプト
 * を一度破棄して、scene_idで指定しているSceneDataを参照してスクリプトファイルを読み込みなおします
 */
//--------------------------------------------------------------
void Frontier_MapChangeReq(FMAIN_PTR fmain, u16 scene_id, u16 event_id)
{
	fmain->ex_param->scene_id = scene_id;
	fmain->map_change = TRUE;
	fmain->map_change_event_id = event_id;
}

//--------------------------------------------------------------
/**
 * @brief   リソース管理バッファのポインタを取得する
 * @param   fmain		
 * @retval  リソース管理バッファのポインタ
 */
//--------------------------------------------------------------
FSS_CHAR_RESOURCE_DATA * Frontier_CharResourceBufferGet(FMAIN_PTR fmain)
{
	return fmain->char_res;
}

//--------------------------------------------------------------
/**
 * @brief   アクター管理バッファのポインタを取得する
 * @param   fmain		
 * @retval  アクター管理バッファのポインタ
 */
//--------------------------------------------------------------
FSS_ACTOR_WORK * Frontier_ActorBufferGet(FMAIN_PTR fmain)
{
	return fmain->fss_actor;
}

//--------------------------------------------------------------
/**
 * @brief   アクター番号を指定してアクター管理バッファのポインタを取得する
 *
 * @param   fmain			
 * @param   actor_no		アクター番号
 *
 * @retval  アクター管理バッファのポインタ
 *
 *
 */
//--------------------------------------------------------------
FSS_ACTOR_WORK * Frontier_ActorWorkPtrGet(FMAIN_PTR fmain, int actor_no)
{
	return &fmain->fss_actor[actor_no];
}

//--------------------------------------------------------------
/**
 * @brief   配置物アクター退避用バッファのポインタを取得する
 * @param   fmain		
 * @retval  配置物アクター退避用バッファのポインタ
 */
//--------------------------------------------------------------
PUSH_ARTICLE_WORK * Frontier_ArticlePushBufferGet(FMAIN_PTR fmain)
{
	return &fmain->push_article;
}

//--------------------------------------------------------------
/**
 * @brief   配置物アクター退避用バッファを初期化する
 * @param   fmain		
 */
//--------------------------------------------------------------
void Frontier_ArticlePushBufferInit(FMAIN_PTR fmain)
{
	int i;
	
	MI_CpuClear8(&fmain->push_article, sizeof(PUSH_ARTICLE_ACT_PARAM));
	for(i = 0; i < ACTWORK_RESOURCE_MAX; i++){
		fmain->push_article.resource_id[i] = SCR_ACTWORK_RESOURCE_ID_NULL;
	}
}

