//==============================================================================
/**
 * @file	frontier_scr.c
 * @brief	フロンティア用スクリプト制御
 * @author	matsuda
 * @date	2007.03.28(水)
 */
//==============================================================================
#include "common.h"
#include "poketool/poke_tool.h"	// POKEMON_PASO_PARAM 参照のため
#include "battle/battle_common.h"
#include "system/clact_tool.h"
#include "system/arc_tool.h"

#include "frontier_types.h"
#include "fs_usescript.h"
#include "frontier_scr.h"
#include "fss_task.h"
#include "fss_scene.h"
#include "frontier_main.h"
#include "frontier_map.h"
#include "frontier_objmove.h"
#include "frontier_def.h"


//============================================================================================
//
//	定義
//
//============================================================================================
#define SCR_MSG_BUF_SIZE	(1024)					//メッセージバッファサイズ

//typedef void (*fsysFunc)(FIELDSYS_WORK* fsys);		//関数ポインタ型

enum{
	WORDSET_SCRIPT_SETNUM = 8,		//デフォルトバッファ数
	WORDSET_SCRIPT_BUFLEN = 64,		//デフォルトバッファ長（文字数）
};


//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static BOOL FSS_TaskPointerSet(FSS_PTR fss, FSS_TASK *fss_task);
static void FSS_ScrTaskDel(FSS_PTR fss, FSS_TASK *core);
static void FssScriptWork_Init(FSS_PTR fss, u16 scene_id, TARGET_OBJ_PTR obj, void* ret_wk );
static void FSS_SetScriptData(
	FSS_CODE **code, MSGDATA_MANAGER **msgman, int scene_id, int heap_id);
static void FSS_EventDataIDJump( FSS_TASK * core, int event_id );


//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   スクリプトシステム作成
 *
 * @param   heap_id		スクリプトシステム内部で使用するヒープID
 *
 * @retval  作成されたスクリプトシステムのポインタ
 */
//--------------------------------------------------------------
FSS_PTR FSS_SystemCreate(FMAIN_PTR fmain, int heap_id, int scene_id)
{
	FSS_PTR fss;
	
	fss = sys_AllocMemory(heap_id, sizeof(FS_SYSTEM));
	MI_CpuClear8(fss, sizeof(FS_SYSTEM));

	fss->fmain = fmain;
	fss->heap_id = heap_id;
	fss->def_scene_id = scene_id;

	//標準スクリプトデータ、標準メッセージデータ読み込み
	FSS_SetScriptData(&fss->def_script, &fss->def_msgman, scene_id, heap_id);

	fss->wordset = WORDSET_CreateEx(WORDSET_SCRIPT_SETNUM, WORDSET_SCRIPT_BUFLEN, heap_id);
	fss->msg_buf = STRBUF_Create(SCR_MSG_BUF_SIZE, heap_id);
	fss->tmp_buf = STRBUF_Create(SCR_MSG_BUF_SIZE, heap_id);

	//※check　まだ未作成
	FssScriptWork_Init(fss, scene_id, NULL, NULL);		//初期設定

	return fss;
}

#include "../field/scr_tool.h"
//--------------------------------------------------------------
/**
 * @brief	スクリプト制御メイン
 *
 * @param	event		GMEVENT_CONTROL型
 *
 * @retval	"FALSE = スクリプト実行中"
 * @retval	"TRUE = スクリプトは動いていない(仮想マシンの数０)"
 */
//--------------------------------------------------------------
//static BOOL FSEVENT_ControlScript(GMEVENT_CONTROL * event)
BOOL FSS_ScriptMain(FSS_PTR fss)
{
	int i;
	FSS_TASK *core;

	if(fss->fss_task_count == 0){
		return TRUE;
	}
	
#ifdef PM_DEBUG
#ifdef DEBUG_FRONTIER_LOOP
	if( sys.cont & PAD_BUTTON_START ){
		debug_frontier_key_set = 1;
	}
	
	if( debug_frontier_key_set ){
		sys.cont |= PAD_BUTTON_A;
		sys.trg |= PAD_BUTTON_A;
	}
#endif
#endif

	switch(fss->seq){
	case 0:
		//仮想マシンコントロール
		for(i = 0; i < FSS_TASK_MAX; i++){
			core = fss->fss_task[i];
			if(core != NULL){
				if( FSST_Control(core) == FALSE ){				//制御メイン
					FSS_ScrTaskDel(fss, core);					//スクリプト削除
					fss->fss_task[i] = NULL;
					fss->fss_task_count--;
				}
			}
		}

		//OBJ動作コード実行
		FSS_ObjMoveFuncAll(fss->fmain);
		
		break;
	
	default:
		break;
	};

	if(fss->fss_task_count == 0){
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   スクリプトシステム解放
 * @param   fss		スクリプトシステムワークへのポインタ
 */
//--------------------------------------------------------------
void FSS_SystemFree(FSS_PTR fss)
{
	GF_ASSERT(fss->fss_task_count == 0);	//仮想マシンが動いている状態では終了出来ない
	
	//デバック処理
	//debug_script_flag = 0;
//	fss->magic_no = 0;

	WORDSET_Delete( fss->wordset );
	STRBUF_Delete( fss->msg_buf );
	STRBUF_Delete( fss->tmp_buf );

	sys_FreeMemoryEz(fss->def_script);
	MSGMAN_Delete(fss->def_msgman);
	
	sys_FreeMemoryEz(fss);								//スクリプトワーク
}

//--------------------------------------------------------------
/**
 * @brief   スクリプトタスク作成
 *
 * @param   fss			スクリプトシステムへのポインタ
 * @param   scr_id		スクリプトID(FSS_SCENEID_DEFAULTの場合は標準スクリプト使用)
 * @param   event_id	イベントID
 *
 * @retval  
 */
//--------------------------------------------------------------
void FSS_ScrTaskAdd(FSS_PTR fss, int scene_id, int event_id)
{
	FSS_TASK* core;

	core = sys_AllocMemory( fss->heap_id, sizeof(FSS_TASK) );
	MI_CpuClear8(core, sizeof(FSS_TASK));
	FSST_Init( core, FSSCmdTable, FSSCmdTableMax);

	core->fss = fss;
	
	if(scene_id == FSS_SCENEID_DEFAULT || fss->def_scene_id == scene_id){
		core->pScript = fss->def_script;
		core->msgman = fss->def_msgman;
	}
	else{
		//スクリプトデータ、メッセージデータ読み込み
		FSS_SetScriptData(&core->pScript, &core->msgman, scene_id, fss->heap_id);
	}
	
	FSST_Start( core, core->pScript );				//仮想マシンにコード設定
	FSS_EventDataIDJump( core, event_id );
//	VM_SetWork( core, (void *)fsys->event );		//コマンドなどで参照するワークセット

	FSS_TaskPointerSet(fss, core);
}

//--------------------------------------------------------------
/**
 * @brief   スクリプトシステムワークに新しく仮想マシンのポインタを追加する
 *
 * @param   fss				スクリプトシステムワークへのポインタ
 * @param   fss_task		追加する仮想マシンのポインタ
 *
 * @retval  TRUE:成功
 * @retval  FALSE:失敗
 */
//--------------------------------------------------------------
static BOOL FSS_TaskPointerSet(FSS_PTR fss, FSS_TASK *fss_task)
{
	int i;

	for(i = 0; i < FSS_TASK_MAX; i++){
		if(fss->fss_task[i] == NULL){
			fss->fss_task[i] = fss_task;
			fss->fss_task_count++;
			return TRUE;
		}
	}
	
	GF_ASSERT(0 && "タスクが既にいっぱい");
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   スクリプト共通削除処理
 * @param   core		
 */
//--------------------------------------------------------------
static void FSS_ScrTaskDel(FSS_PTR fss, FSS_TASK *core)
{
	if(core->msgman != fss->def_msgman){
		MSGMAN_Delete( core->msgman );
	}
	if(core->pScript != fss->def_script){
		sys_FreeMemoryEz( core->pScript );			//スクリプトデータ
	}
	
	sys_FreeMemoryEz( core );
}

//--------------------------------------------------------------
/**
 * @brief	スクリプト制御ワーク初期設定
 *
 * @param	fsys		FIELDSYS_WORK型のポインタ
 * @param	fss			スクリプトシステムワークへのポインタ
 * @param	scr_id		スクリプトID
 * @param	obj			話しかけ対象OBJのポインタ
 * @param	ret_wk		スクリプト結果を代入するワークのポインタ
 *
 * @retval	"スクリプト制御ワークのアドレス"
 */
//--------------------------------------------------------------
static void FssScriptWork_Init(FSS_PTR fss, u16 scene_id, TARGET_OBJ_PTR obj, void* ret_wk )
{
//	u16* objid = GetEvScriptWorkMemberAdrs_Sub( fss, ID_EVSCR_WK_TARGET_OBJID );

	//DIR_NOT = -1
//	fss->player_dir = Player_DirGet(fsys->player);	//イベント起動時の主人公の向きセット
//	fss->target_obj = obj;							//話しかけ対象OBJのポインタセット
//	fss->script_id  = scr_id;						//メインのスクリプトID
//	fss->ret_script_wk = ret_wk;						//スクリプト結果を代入するワーク

	if( obj != NULL ){
//		*objid = FieldOBJ_OBJIDGet(obj);			//話しかけ対象OBJIDのセット
	}
}

//--------------------------------------------------------------
/**
 * @brief   ゾーンスクリプトデータセット
 *
 * @param   fss			
 * @param   core		
 * @param   scr_id		スクリプトID
 */
//--------------------------------------------------------------
static void FSS_SetScriptData(FSS_CODE **code, MSGDATA_MANAGER **msgman, int scene_id, int heap_id)
{
	int scr_id, msg_id;
	
	scr_id = FSS_SceneParamGet(scene_id, FSS_SCENE_DATA_SCRID);
	msg_id = FSS_SceneParamGet(scene_id, FSS_SCENE_DATA_MSGID);

	//スクリプトファイルをロード
	OS_TPrintf("scene = %d, scr = %d, msg = %d\n", scene_id, scr_id, msg_id);
	*code = ArchiveDataLoadMalloc(ARC_FRONTIER_SCRIPT, scr_id, heap_id);

	//メッセージデータマネージャー作成
	*msgman = MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, msg_id, heap_id );
}

//--------------------------------------------------------------
/**
 * @brief   スクリプトシステムで使用している標準メッセージマネージャを新しいシーンの
 * 			メッセージデータでマネージャを作り直して、使用している仮想マシンの
 *			メッセージマネージャのポインタを新しいほうで設定しなおす
 *
 * @param   fss					スクリプトシステムワークへのポインタ
 * @param   new_scene_id		新しいシーンのID
 * @param   heap_id				ヒープID
 *
 * 標準メッセージマネージャを使用していない仮想マシンについては何も行われません
 *
 * この関数はスクリプトからMAP_CHANGEでシーンIDを変更した場合、そのシーンデータのメッセージを
 * 使うようにする為に用意されたものです。
 */
//--------------------------------------------------------------
void FSS_MsgManageSceneChange(FSS_PTR fss, int new_scene_id, int heap_id)
{
	int i;
	FSS_TASK *core;
	int now_msg_id, new_msg_id;
	MSGDATA_MANAGER *new_msgman;
	
	now_msg_id = FSS_SceneParamGet(fss->def_scene_id, FSS_SCENE_DATA_MSGID);
	new_msg_id = FSS_SceneParamGet(new_scene_id, FSS_SCENE_DATA_MSGID);
	if(now_msg_id == new_msg_id){
		return;
	}
	
	new_msgman = MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, new_msg_id, heap_id );
	
	for(i = 0; i < FSS_TASK_MAX; i++){
		core = fss->fss_task[i];
		if(core != NULL){
			if(core->msgman == fss->def_msgman){
				//デフォルトのメッセージマネージャを使っているなら新しいシーンの
				//メッセージマネージャに設定しなおす
				core->msgman = new_msgman;
			}
		}
	}
	
	MSGMAN_Delete(fss->def_msgman);
	fss->def_msgman = new_msgman;
}

//--------------------------------------------------------------
/**
 * @brief   param_workの値をメモリを確保して、そこにバックアップします
 *
 * @param   fss			
 * @param   heap_id		ヒープID
 *
 * @retval  バックアップデータが入っているバッファのポインタ
 */
//--------------------------------------------------------------
FS_PARAM_WORK *FSS_ParamWorkBackup(FSS_PTR fss, int heap_id)
{
	FS_PARAM_WORK *pw;
	
	pw = sys_AllocMemory(heap_id, sizeof(FS_PARAM_WORK));
	*pw = fss->param_work;
	return pw;
}

//--------------------------------------------------------------
/**
 * @brief   FSS_ParamWorkBackup関数でバックアップしたparam_workを復帰させます
 *
 * @param   fss		
 * @param   pw		param_workへのポインタ
 *
 * FSS_ParamWorkBackup関数とセットで使用することが前提になっています。
 * 関数内でpwはメモリ解放されます
 */
//--------------------------------------------------------------
void FSS_ParamWorkRecover(FSS_PTR fss, FS_PARAM_WORK *pw)
{
	fss->param_work = *pw;
	sys_FreeMemoryEz(pw);
}

//--------------------------------------------------------------
/**
 * @brief   イベントIDジャンプ
 *
 * @param   core		
 * @param   event_id	イベントID
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
static void FSS_EventDataIDJump( FSS_TASK * core, int event_id )
{
	core->PC += (event_id * 4);			//ID分進める(adrsがlongなので*4)
	core->PC += FSSTGetU32( core );		//ラベルオフセット分進める
}

//--------------------------------------------------------------
/**
 * @brief   param_workのアドレスを取得する
 *
 * @param   fss			
 * @param   work_no		ワーク番号
 *
 * @retval  ワークのアドレス
 */
//--------------------------------------------------------------
u16 * FSS_ParamWorkAdrsGet(FSS_PTR fss, int work_no)
{
	return &fss->param_work.work[work_no];
}

//--------------------------------------------------------------
/**
 * @brief   fmapのポインタを取得する
 *
 * @param   fss		
 *
 * @retval  fmapポインタ
 */
//--------------------------------------------------------------
FMAP_PTR FSS_GetFMapAdrs(FSS_PTR fss)
{
	return Frontier_FMapAdrsGet(fss->fmain);
}

