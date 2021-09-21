//==============================================================================
/**
 * @file	d_matsu_res.c
 * @brief	松田デバッグ用ファイル(常駐物)
 * @author	matsuda
 * @date	2007.04.18(水)
 */
//==============================================================================

#ifdef PM_DEBUG	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
#include "common.h"
#include "system/pm_overlay.h"
#include "system/test_overlay.h"

#include "system/bmp_list.h"
#include "fieldmap.h"
#include "fieldsys.h"
#include "system/fontproc.h"
#include "contest/contest.h"

#include "script.h"
#include "..\fielddata\script\connect_def.h"

#include "system/particle.h"
#include "particledata/particledata.h"
#include "particledata/particledata.naix"

#include "system/fontproc.h"
#include "system/msgdata.h"
#include "msgdata/msg.naix"
#include "msgdata/msg_debug_matsu.h"

#include "field/sysflag.h"

#include "debug/d_matsu.h"

#include "application/balloon.h"




void DMRes_FssTest(DMATSU_WORK *dm);
static BOOL GMEVENT_DebugFssTestStart(GMEVENT_CONTROL * event);
static BOOL GMEVENT_Sub_FssTest(GMEVENT_CONTROL * event);

void DMRes_BalloonStart(FIELDSYS_WORK *fsys, int proc_flag, int mode);
static BOOL GMEVENT_DebugBalloonStart(GMEVENT_CONTROL * event);
static BOOL GMEVENT_Sub_DebugBalloon(GMEVENT_CONTROL * event);
static BOOL GMEVENT_Sub_DebugGds(GMEVENT_CONTROL * event);
static BOOL GMEVENT_Sub_DebugFootprint(GMEVENT_CONTROL * event);

#if DEBUG_MATSU_CONTEST
BOOL GMEVENT_DebugConStart(GMEVENT_CONTROL * event);
static void EventCmd_DebugConProc(GMEVENT_CONTROL * event, int debug_mode, int rank, int type);
static BOOL GMEVENT_Sub_DebugCon(GMEVENT_CONTROL * event);
static void ContestDebug_ImageClipDebugAccessorySet(CONTEST_SYSTEM *consys);
static void ContestDebug_ImageClipDebugAccessoryFree(CONTEST_SYSTEM *consys);
#endif

FS_EXTERN_OVERLAY(frontier_common);

FS_EXTERN_OVERLAY(balloon);
FS_EXTERN_OVERLAY(minigame_common);
FS_EXTERN_OVERLAY(gds_comm);
FS_EXTERN_OVERLAY(footprint_board);

#include "ev_mapchange.h"
#include "frontier/frontier_types.h"
#include "frontier/frontier_main.h"
#include "frontier/fss_scene.h"
#include "battle/attr_def.h"

#include "application/br_sys.h"
#include "application/gds_main.h"

#include "application/wifi_lobby/wflby_system_def.h"
#include "application/footprint_main.h"

#include "contest/contest.h"
#include "include/application/imageClip/imc_sys.h"

typedef struct{
	int seq;
	FRONTIER_EX_PARAM ex_param;
}DEBUG_FSS_WORK;


///風船割りミニゲーム呼び出し時に使用するワーク
typedef struct{
	int main_seq;
	int debug_mode;
	int proc_flag;		///<どのPROCを呼び出すか
	int mode;			///<PROCにオプション
}DMATSU_EV_BALLOON_WORK;

///風船割りミニゲーム呼び出し時に使用するワーク
typedef struct{
	int seq;
	int proc_flag;
	BALLOON_PROC_WORK *p_work;
	int mode;			///<PROCにオプション
}DEBUG_BALLOON_LOCAL;


//==============================================================================
//	グローバル変数宣言
//==============================================================================
///1:ローテーションなし。　0:ローテーションあり　2:ダンス回数制限なし
u8 DmatsuWork_DanceRotationOff = 0;


//==============================================================================
//	外部データ
//==============================================================================
extern const PROC_DATA ContestActinProcData;
extern const PROC_DATA ContestDanceProcData;
extern const PROC_DATA ContestVisualProcData;
extern const PROC_DATA ContestResultProcData;
extern const PROC_DATA IMC_SYS_Proc;

//==============================================================================
//	外部関数宣言
//==============================================================================
extern void Contest_ImageClipInitDataCreate(CONTEST_SYSTEM *consys);
extern void Contest_ImageClipInitDataFree(CONTEST_SYSTEM *consys);



//--------------------------------------------------------------
/**
 * @brief   FSS系スクリプトテスト実行
 *
 * @param   dm		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
void DMRes_FssTest(DMATSU_WORK *dm)
{
	u32 *al_work;
	
	al_work = sys_AllocMemoryLo(HEAPID_WORLD, sizeof(u32));
	MI_CpuClear8(al_work, sizeof(u32));
	
	FieldEvent_Set(dm->fsys, GMEVENT_DebugFssTestStart, al_work);
}

static BOOL GMEVENT_DebugFssTestStart(GMEVENT_CONTROL * event)
{
	FIELDSYS_WORK * fsys;
	u32 *al_work;
	
	fsys = FieldEvent_GetFieldSysWork(event);
	al_work = FieldEvent_GetSpecialWork(event);

	switch (*al_work) {
	case 0:
		EventCmd_FieldFadeOut(event);
		(*al_work)++;
		break;
	case 1:
		{
			DEBUG_FSS_WORK *sub_work = sys_AllocMemoryLo(HEAPID_WORLD, sizeof(DEBUG_FSS_WORK));
			MI_CpuClear8(sub_work, sizeof(DEBUG_FSS_WORK));
			FieldEvent_Call(event, GMEVENT_Sub_FssTest, sub_work);
		}
		(*al_work)++;
		break;
	case 2:
		EventCmd_FieldFadeIn(event);
		(*al_work)++;
		break;
	case 3:
		sys_FreeMemoryEz(al_work);
		return TRUE;
	}
	return FALSE;
}

static BOOL GMEVENT_Sub_FssTest(GMEVENT_CONTROL * event)
{
	FIELDSYS_WORK * fsys = FieldEvent_GetFieldSysWork(event);
	DEBUG_FSS_WORK *sub_work = FieldEvent_GetSpecialWork(event);
	
	switch (sub_work->seq) {
	case 0:
		EventCmd_FinishFieldMap(event);
		sub_work->seq++;
		break;
	case 1:
		//ex_param作成
		sub_work->ex_param.syswork = NULL;
		sub_work->ex_param.config = SaveData_GetConfig(fsys->savedata);
		sub_work->ex_param.savedata = GameSystem_GetSaveData(fsys);
		sub_work->ex_param.scene_id = FSS_SCENEID_TESTSCR;
		sub_work->ex_param.fnote_data = fsys->fnote;
		sub_work->ex_param.bag_cursor = fsys->bag_cursor;
		sub_work->ex_param.bg_id = BG_ID_ROOM_A;
		sub_work->ex_param.ground_id = GROUND_ID_FLOOR;
		sub_work->ex_param.zone_id = fsys->location->zone_id;
		sub_work->ex_param.battle_cursor = fsys->battle_cursor;
		
		EventCmd_CallSubProc(event, &FrontierMainProcData, &sub_work->ex_param);
		sub_work->seq++;
		break;
	case 2:
		EventCmd_StartFieldMap(event);
		sub_work->seq++;
		break;
	case 3:
		sys_FreeMemoryEz(sub_work);
		return TRUE;
	}
	return FALSE;
}




//--------------------------------------------------------------
/**
 * @brief   風船割り＆GDSテスト：呼び出し
 *
 * @param   dm		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
void DMRes_BalloonStart(FIELDSYS_WORK *fsys, int proc_flag, int mode)
{
	DMATSU_EV_BALLOON_WORK *eaw;
	
	eaw = sys_AllocMemoryLo(HEAPID_WORLD, sizeof(DMATSU_EV_BALLOON_WORK));
	MI_CpuClear8(eaw, sizeof(DMATSU_EV_BALLOON_WORK));
	eaw->proc_flag = proc_flag;
	eaw->mode = mode;
	FieldEvent_Set(fsys, GMEVENT_DebugBalloonStart, eaw);
}

//-----------------------------------------------------------------------------
/**
 * @brief	風船割り：イベントシーケンス
 * @param	event		イベント制御ワークへのポインタ
 * @retval	TRUE		イベント終了
 * @retval	FALSE		イベント継続中
 *
 * エンカウントエフェクト→→フィールド復帰を行う。
 * ゲームオーバー処理への分岐も入る（予定）
 */
//-----------------------------------------------------------------------------
static BOOL GMEVENT_DebugBalloonStart(GMEVENT_CONTROL * event)
{
	FIELDSYS_WORK * fsys;
	DMATSU_EV_BALLOON_WORK *eaw;
	
	fsys = FieldEvent_GetFieldSysWork(event);
	eaw = FieldEvent_GetSpecialWork(event);

	switch (eaw->main_seq) {
	case 0:
		EventCmd_FieldFadeOut(event);
		eaw->main_seq++;
		break;
	case 1:
		{
			DEBUG_BALLOON_LOCAL *sub_work = sys_AllocMemoryLo(HEAPID_WORLD, 
				sizeof(DEBUG_BALLOON_LOCAL));
			MI_CpuClear8(sub_work, sizeof(DEBUG_BALLOON_LOCAL));
			sub_work->proc_flag = eaw->proc_flag;
			sub_work->mode = eaw->mode;
			switch(eaw->proc_flag){
			case DMATSU_PROC_FLAG_BALLOON:
				FieldEvent_Call(event, GMEVENT_Sub_DebugBalloon, sub_work);
				break;
			case DMATSU_PROC_FLAG_GDS:
			case DMATSU_PROC_FLAG_GDS_RECORDER:
				FieldEvent_Call(event, GMEVENT_Sub_DebugGds, sub_work);
				break;
			case DMATSU_PROC_FLAG_FOOTPRINT:
				FieldEvent_Call(event, GMEVENT_Sub_DebugFootprint, sub_work);
				break;
			}
			eaw->main_seq++;
		}
		break;
	case 2:
		EventCmd_FieldFadeIn(event);
		eaw->main_seq++;
		break;
	case 3:
		sys_FreeMemoryEz(eaw);
		return TRUE;
	}
	return FALSE;
}

static BOOL GMEVENT_Sub_DebugBalloon(GMEVENT_CONTROL * event)
{
	FIELDSYS_WORK * fsys = FieldEvent_GetFieldSysWork(event);
	DEBUG_BALLOON_LOCAL *sub_work = FieldEvent_GetSpecialWork(event);
	
	switch (sub_work->seq) {
	case 0:
		EventCmd_FinishFieldMap(event);
		sub_work->seq ++;
		break;
	case 1:
		{
			// プロセス定義データ
			static const PROC_DATA BalloonProcData = {
				BalloonProc_Init,
				BalloonProc_Main,
				BalloonProc_End,
				FS_OVERLAY_ID(balloon),
			};
			sub_work->p_work = sys_AllocMemory( HEAPID_WORLD, sizeof(BALLOON_PROC_WORK) );
			MI_CpuClear8(sub_work->p_work, sizeof(BALLOON_PROC_WORK));
			sub_work->p_work->vchat		= 0;
			sub_work->p_work->wifi_lobby	= FALSE;
			sub_work->p_work->p_save		= NULL;
			sub_work->p_work->debug_offline = TRUE;
			
			// ミニゲーム共通オーバーレイを読み込む
			Overlay_Load( FS_OVERLAY_ID(minigame_common), OVERLAY_LOAD_NOT_SYNCHRONIZE);
			
			//GameSystem_StartSubProc(fsys, &BalloonProcData, sub_work->p_work);
			EventCmd_CallSubProc(event, &BalloonProcData, sub_work->p_work);
		}
		
		sub_work->seq ++;
		break;
	case 2:
		sys_FreeMemoryEz(sub_work->p_work);
		Overlay_UnloadID( FS_OVERLAY_ID(minigame_common) );
		
		EventCmd_StartFieldMap(event);
		sub_work->seq ++;
		break;
	case 3:
		sys_FreeMemoryEz(sub_work);
		return TRUE;
	}
	return FALSE;
}

extern PROC_RESULT GdsMainProc_Init( PROC * proc, int * seq );
extern PROC_RESULT GdsMainProc_Main( PROC * proc, int * seq );
extern PROC_RESULT GdsMainProc_End( PROC * proc, int * seq );

static BOOL GMEVENT_Sub_DebugGds(GMEVENT_CONTROL * event)
{
	FIELDSYS_WORK * fsys = FieldEvent_GetFieldSysWork(event);
	DEBUG_BALLOON_LOCAL *sub_work = FieldEvent_GetSpecialWork(event);
	
	switch (sub_work->seq) {
	case 0:
		EventCmd_FinishFieldMap(event);
		sub_work->seq ++;
		break;
	case 1:
		{
			GDSPROC_PARAM *param;
			// プロセス定義データ
			static const PROC_DATA GdsProcData = {
				GdsMainProc_Init,
				GdsMainProc_Main,
				GdsMainProc_End,
				FS_OVERLAY_ID(gds_comm),
			};

			param = sys_AllocMemoryLo(HEAPID_WORLD, sizeof(GDSPROC_PARAM));
			MI_CpuClear8(param, sizeof(GDSPROC_PARAM));
			param->fsys = fsys;
			param->savedata = fsys->savedata;
			param->connect = 0;
			param->gds_mode = sub_work->mode;
	
			EventCmd_CallSubProc(event, &GdsProcData, param);
		}
		
		sub_work->seq ++;
		break;
	case 2:
		EventCmd_StartFieldMap(event);
		sub_work->seq ++;
		break;
	case 3:
		sys_FreeMemoryEz(sub_work);
		return TRUE;
	}
	return FALSE;
}

static BOOL GMEVENT_Sub_DebugFootprint(GMEVENT_CONTROL * event)
{
	FIELDSYS_WORK * fsys = FieldEvent_GetFieldSysWork(event);
	DEBUG_BALLOON_LOCAL *sub_work = FieldEvent_GetSpecialWork(event);
	
	switch (sub_work->seq) {
	case 0:
		EventCmd_FinishFieldMap(event);
		sub_work->seq ++;
		break;
	case 1:
		{
			// プロセス定義データ
			static const PROC_DATA FootprintProcData = {
				FootPrintProc_Init,
				FootPrintProc_Main,
				FootPrintProc_End,
				FS_OVERLAY_ID(footprint_board),
			};
			FOOTPRINT_PARAM *footparam = sys_AllocMemory(HEAPID_WORLD, sizeof(FOOTPRINT_PARAM));
			MI_CpuClear8(footparam, sizeof(FOOTPRINT_PARAM));
			footparam->debug_sv = fsys->savedata;
			EventCmd_CallSubProc(event, &FootprintProcData, footparam);
		}
		
		sub_work->seq ++;
		break;
	case 2:
		EventCmd_StartFieldMap(event);
		sub_work->seq ++;
		break;
	case 3:
		sys_FreeMemoryEz(sub_work);
		return TRUE;
	}
	return FALSE;
}








#if DEBUG_MATSU_CONTEST
//-----------------------------------------------------------------------------
/**
 * @brief	コンテスト：演技力部門イベントシーケンス
 * @param	event		イベント制御ワークへのポインタ
 * @retval	TRUE		イベント終了
 * @retval	FALSE		イベント継続中
 *
 * エンカウントエフェクト→→フィールド復帰を行う。
 * ゲームオーバー処理への分岐も入る（予定）
 */
//-----------------------------------------------------------------------------
BOOL GMEVENT_DebugConStart(GMEVENT_CONTROL * event)
{
	FIELDSYS_WORK * fsys;
	EV_ACTING_WORK *eaw;
	
	fsys = FieldEvent_GetFieldSysWork(event);
	eaw = FieldEvent_GetSpecialWork(event);

	switch (eaw->seq) {
	case 0:
		EventCmd_FieldFadeOut(event);
		eaw->seq++;
		break;
	case 1:
		EventCmd_DebugConProc(event, eaw->debug_mode, eaw->rank, eaw->type);
		eaw->seq++;
		break;
	case 2:
		EventCmd_FieldFadeIn(event);
		eaw->seq++;
		break;
	case 3:
		sys_FreeMemoryEz(eaw);
		return TRUE;
	}
	return FALSE;
}

static void EventCmd_DebugConProc(GMEVENT_CONTROL * event, int debug_mode, int rank, int type)
{
	EV_SUB_ACTING_WORK *esaw = sys_AllocMemoryLo(HEAPID_WORLD, sizeof(EV_SUB_ACTING_WORK));
	MI_CpuClear8(esaw, sizeof(EV_SUB_ACTING_WORK));
	esaw->debug_mode = debug_mode;
	esaw->rank = rank;
	esaw->type = type;
	FieldEvent_Call(event, GMEVENT_Sub_DebugCon, esaw);
}

static BOOL GMEVENT_Sub_DebugCon(GMEVENT_CONTROL * event)
{
	FIELDSYS_WORK * fsys = FieldEvent_GetFieldSysWork(event);
	EV_SUB_ACTING_WORK *esaw = FieldEvent_GetSpecialWork(event);
	
	switch (esaw->seq) {
	case 0:
		EventCmd_FinishFieldMap(event);
		esaw->seq ++;
		break;
	case 1:
		//※check	入り口がここしかないので、とりあえずここでコンテスト全体ヒープ作成
		{
			POKEMON_PARAM * poke;
			STRBUF *myname;
			const MYSTATUS *my_status;
			CONTEST_INIT_DATA cid;
			
			poke = PokeParty_GetMemberPointer( SaveData_GetTemotiPokemon(fsys->savedata), 0);
			my_status = SaveData_GetMyStatus(GameSystem_GetSaveData(fsys));
			myname = MyStatus_CreateNameString(my_status, HEAPID_WORLD);
			
			cid.type = esaw->type;//gf_rand() % CONTYPE_MAX;//CONTYPE_STYLE;
			cid.rank = esaw->rank;//CONRANK_NORMAL;
			cid.mode = CONMODE_GRAND;
			cid.my_pp = poke;
			cid.player_name_str = myname;
			cid.my_status = my_status;
			cid.imc_save = SaveData_GetImcSaveData(fsys->savedata);
			cid.config = SaveData_GetConfig(fsys->savedata);
			cid.sv = fsys->savedata;
			esaw->consys = Contest_SystemCreate(&cid);
			
			STRBUF_Delete(myname);
		}
		
		esaw->seq ++;
		break;
	case 2:		//それぞれの部門を呼び出し
		switch(esaw->debug_mode){
		case DEBUG_CON_ACTIN:
			EventCmd_CallSubProc(event, &ContestActinProcData, esaw->consys);
			esaw->seq++;
			break;
		case DEBUG_CON_VISUAL:
			//デバッグモードでは自分のクリップデータだけないので、ここでセット
			ImcSaveData_SetContestPokeData_Easy(esaw->consys->c_game.imc_data[0], 
				esaw->consys->c_game.pp[0], -1);
			ImcSaveData_SetContestBgId(esaw->consys->c_game.imc_data[0], 0);
			ImcSaveData_SetContestRank(esaw->consys->c_game.imc_data[0], 
				esaw->consys->c_game.rank);

			EventCmd_CallSubProc(event, &ContestVisualProcData, esaw->consys);
			esaw->seq++;
			break;
		case DEBUG_CON_CLIP_VISUAL:
			switch(esaw->local_seq){
			case 0:
				Contest_ImageClipInitDataCreate(esaw->consys);
				ContestDebug_ImageClipDebugAccessorySet(esaw->consys);	//デバッグデータセット
				EventCmd_CallSubProc(event, &IMC_SYS_Proc, esaw->consys->icpw);
				esaw->local_seq++;
				break;
			case 1:
				ContestDebug_ImageClipDebugAccessoryFree(esaw->consys);	//デバッグデータ開放
				Contest_ImageClipInitDataFree(esaw->consys);
				esaw->local_seq++;
				break;
			default:
				EventCmd_CallSubProc(event, &ContestVisualProcData, esaw->consys);
				esaw->local_seq = 0;
				esaw->seq++;
				break;
			}
			break;
		case DEBUG_CON_DANCE:
			//デバッグモードでは自分のクリップデータだけないので、ここでセット
			ImcSaveData_SetContestPokeData_Easy(esaw->consys->c_game.imc_data[0], 
				esaw->consys->c_game.pp[0], -1);
			ImcSaveData_SetContestBgId(esaw->consys->c_game.imc_data[0], 0);
			ImcSaveData_SetContestRank(esaw->consys->c_game.imc_data[0], 
				esaw->consys->c_game.rank);

			EventCmd_CallSubProc(event, &ContestDanceProcData, esaw->consys);
			esaw->seq++;
			break;
		case DEBUG_CON_RESULT:
			EventCmd_CallSubProc(event, &ContestResultProcData, esaw->consys);
			esaw->seq++;
			break;
		}
		break;
	case 3:
		//※check　出口がここしかないので、とりあえずここでコンテスト全体ヒープ解放
		Contest_SystemExit(esaw->consys);
		
		EventCmd_StartFieldMap(event);
		esaw->seq ++;
		break;
	case 4:
		sys_FreeMemoryEz(esaw);
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   イメージクリップデバッグ用にアクセサリーを全てセット
 *
 * @param   consys		コンテストシステムワークへのポインタ
 *
 * Contest_ImageClipInitDataCreateの関数後に呼び出してください
 */
//--------------------------------------------------------------
static void ContestDebug_ImageClipDebugAccessorySet(CONTEST_SYSTEM *consys)
{
	IMC_CONTEST_PROC_WORK *icpw;
	IMC_ITEM_SAVEDATA *imcsave;
	
	GF_ASSERT(consys->icpw != NULL);
	icpw = consys->icpw;
	
	imcsave = ImcSaveData_ItemAllocWork(HEAPID_CONTEST);
	Debug_ImcSaveData_SetItemData(imcsave);

	icpw->cp_imc_item = imcsave;
}

//--------------------------------------------------------------
/**
 * @brief   デバッグ用にセットしたアクセサリ領域を開放
 *
 * @param   consys		コンテストシステムワークへのポインタ
 *
 * Contest_ImageClipInitDataFree関数を呼ぶよりも先にこれでデバッグ領域の解放をしてください
 */
//--------------------------------------------------------------
static void ContestDebug_ImageClipDebugAccessoryFree(CONTEST_SYSTEM *consys)
{
	IMC_CONTEST_PROC_WORK *icpw;
	IMC_ITEM_SAVEDATA *imcsave;
	icpw = consys->icpw;
	imcsave = (void*)icpw->cp_imc_item;
	sys_FreeMemoryEz(imcsave);
}
#endif	//DEBUG_MATSU_CONTEST

#endif	// PM_DEBUG +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
