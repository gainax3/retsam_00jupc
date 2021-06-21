//==============================================================================
/**
 * @file	frontier_objmove.c
 * @brief	OBJ動作コードの実動作
 * @author	matsuda
 * @date	2007.07.27(金)
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
#include "frontier_scrcmd_sub.h"
#include "frontier_objmove.h"


//==============================================================================
//	型定義
//==============================================================================
///OBJ動作コードの関数型
typedef BOOL (*FSS_OBJMOVE_FUNC)(FMAIN_PTR, FSS_ACTOR_WORK *);


//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static void FSS_ObjMoveFunc(FMAIN_PTR fmain, FSS_ACTOR_WORK *fss_actor);
void FSS_ObjMoveFuncIDSet(FMAIN_PTR fmain, u16 playid, u8 move_id, s16 work[], int work_num);
void FSS_ObjMoveFuncAll(FMAIN_PTR fmain);
static BOOL MoveFunc_Kyoro(FMAIN_PTR fmain, FSS_ACTOR_WORK *fss_actor);


//==============================================================================
//	関数テーブル
//==============================================================================
///OBJ動作コードの実行関数テーブル
///		※OBJMOVE_ID_???と並びを同じにしておくこと！
static const FSS_OBJMOVE_FUNC FssObjMoveFuncTbl[] = {
	NULL,
	MoveFunc_Kyoro,
};



//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   OBJ動作コード設定
 *
 * @param   fmain		
 * @param   playid		OBJ認識ID
 * @param   move_id		OBJ動作コードID(OBJMOVE_ID_???)
 * @param   work[]		引数として渡す値の入った配列
 * @param   work_num	work[]の要素数
 */
//--------------------------------------------------------------
void FSS_ObjMoveFuncIDSet(FMAIN_PTR fmain, u16 playid, u8 move_id, s16 work[], int work_num)
{
	FSS_ACTOR_WORK *fss_actor;
	int i;
	
	fss_actor = Frontier_ActorWorkSearch(fmain, playid);
	MI_CpuClear8(&fss_actor->move, sizeof(FSS_OBJMOVE_WORK));
	
	fss_actor->move.move_id = move_id;
	for(i = 0; i < work_num; i++){
		fss_actor->move.work[i] = work[i];
	}
}

//--------------------------------------------------------------
/**
 * @brief   OBJ動作コード実行
 *
 * @param   fmain		
 * @param   fss_actor	対象OBJのアクターポインタ
 */
//--------------------------------------------------------------
static void FSS_ObjMoveFunc(FMAIN_PTR fmain, FSS_ACTOR_WORK *fss_actor)
{
	int ret;
	
	if(FssObjMoveFuncTbl[fss_actor->move.move_id] == NULL){
		return;
	}
	
	ret = FssObjMoveFuncTbl[fss_actor->move.move_id](fmain, fss_actor);
	if(ret == TRUE){
		MI_CpuClear8(&fss_actor->move, sizeof(FSS_OBJMOVE_WORK));
	}
}

//--------------------------------------------------------------
/**
 * @brief   全てのOBJのOBJ動作コードを実行
 *
 * @param   fmain		
 */
//--------------------------------------------------------------
void FSS_ObjMoveFuncAll(FMAIN_PTR fmain)
{
	FSS_ACTOR_WORK *fss_actor;
	int i;
	
	fss_actor = Frontier_ActorBufferGet(fmain);
	for(i = 0; i < FIELD_OBJ_MAX; i++){
		if(fss_actor->objwk != NULL && fss_actor->anime_tcb == NULL){
			FSS_ObjMoveFunc(fmain, fss_actor);
		}
		fss_actor++;
	}
}




//==============================================================================
//
//	動作コード実行関数
//
//==============================================================================
///きょろきょろの向きを変えるウェイト値
#define KYORO_WAIT		(30)
///正面を向いている時のウェイト値
#define KYORO_FRONT_WAIT	(45)
//--------------------------------------------------------------
/**
 * @brief   OBJ動作：きょろきょろする
 *
 * @param   fmain		
 * @param   fss_actor		
 *
 * @retval  TRUE:動作終了
 *
 * work[0] = 正面の向き(FC_DIR_???)
 * work[1] = きょろきょろ動作で最初に向かせる方向を決定するフラグ(0 or 1)
 * work[2] = 初期ウェイト
 *
 * 正面の向きがDOWNの場合、work[1]が0だと、左＞右、1だと、右＞左、とアニメします
 */
//--------------------------------------------------------------
static BOOL MoveFunc_Kyoro(FMAIN_PTR fmain, FSS_ACTOR_WORK *fss_actor)
{
	FSS_OBJMOVE_WORK *move = &fss_actor->move;
	enum{
		WORK_FRONT_DIR,
		WORK_KYORO_FLG,
		WORK_WAIT,
		WORK_ANMCODE_1,
		WORK_ANMCODE_2,
		WORK_ANMCODE_3,
	};
	WF2DMAP_ACTCMD act;
	FMAP_PTR fmap = Frontier_FMapAdrsGet(fmain);
	
	if(move->work[WORK_WAIT] > 0){
		move->work[WORK_WAIT]--;
		return FALSE;
	}

	switch(move->seq){
	case 0:
		switch(move->work[WORK_FRONT_DIR]){
		case FC_DIR_U:
		case FC_DIR_D:
			if(move->work[WORK_KYORO_FLG] == 0){
				move->work[WORK_ANMCODE_1] = FC_DIR_L;
				move->work[WORK_ANMCODE_2] = FC_DIR_R;
			}
			else{
				move->work[WORK_ANMCODE_1] = FC_DIR_R;
				move->work[WORK_ANMCODE_2] = FC_DIR_L;
			}
			break;
		case FC_DIR_L:
		case FC_DIR_R:
			if(move->work[WORK_KYORO_FLG] == 0){
				move->work[WORK_ANMCODE_1] = FC_DIR_U;
				move->work[WORK_ANMCODE_2] = FC_DIR_D;
			}
			else{
				move->work[WORK_ANMCODE_1] = FC_DIR_D;
				move->work[WORK_ANMCODE_2] = FC_DIR_U;
			}
			break;
		default:
			GF_ASSERT(0);
			return TRUE;
		}
		move->work[WORK_ANMCODE_3] = move->work[WORK_FRONT_DIR];
		move->seq++;
		//break;
		
	case 1:
	case 2:
	case 3:
		FSSC_Sub_ActCmdCreate(&act, fss_actor->objwk, 
			fss_actor->param.playid, move->work[WORK_ANMCODE_1 + move->seq - 1]);
		WF2DMAP_ACTCMDQSysCmdPush(fmap->actcmd_q, &act);
		if(move->work[WORK_FRONT_DIR] == move->work[WORK_ANMCODE_1 + move->seq - 1]){
			move->work[WORK_WAIT] = KYORO_FRONT_WAIT;
			move->seq = 1;
		}
		else{
			move->work[WORK_WAIT] = KYORO_WAIT;
			move->seq++;
		}
		break;
	
	default:
		return TRUE;
	}
	
	return FALSE;
}

