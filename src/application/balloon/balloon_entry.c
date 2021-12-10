//==============================================================================
/**
 * @file	balloon_entry.c
 * @brief	風船割り：エントリー画面＆結果画面
 * @author	matsuda
 * @date	2007.11.06(火)
 */
//==============================================================================
#include "common.h"
#include "system/clact_tool.h"
#include "system/palanm.h"
#include "system/pmfprint.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "system/fontproc.h"
#include "system/msgdata.h"
#include "system/particle.h"
#include "system/brightness.h"
#include "system/snd_tool.h"
#include "communication/communication.h"
#include "msgdata/msg.naix"
#include "system/wipe.h"
#include "communication/wm_icon.h"
#include "system/msgdata_util.h"
#include <dwc.h>
#include "libdpw/dpw_tr.h"
#include "system/procsys.h"

#include "balloon_common.h"
#include "application/balloon.h"
#include "balloon_game.h"
#include "balloon_entry.h"


//==============================================================================
//	定数定義
//==============================================================================
///メインシーケンス
enum{
	MAINSEQ_INIT,
	MAINSEQ_ENTRY_INIT,
	MAINSEQ_ENTRY_WAIT,
	MAINSEQ_RESULT_INIT,
	MAINSEQ_RESULT_WAIT,
	MAINSEQ_END,
};


//==============================================================================
//	構造体定義
//==============================================================================
///風船割りエントリー画面ROCワーク構造体定義
typedef struct{
	BALLOON_SYSTEM_WORK *bsw;		///<風船割りシステムワークへのポインタ
	MNGM_ENTRYWK *mngm_entry;		///<エントリー画面アプリのポインタ
	MNGM_RESULTWK *mngm_result;		///<結果発表画面アプリのポインタ
}BALLOON_ENTRY_PROC_WORK;


//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   風船割りエントリー画面：初期化
 *
 * @param   proc		PROCへのポインタ
 * @param   seq			シーケンスワーク
 *
 * @retval  
 */
//--------------------------------------------------------------
PROC_RESULT BalloonEntryProc_Init( PROC * proc, int * seq )
{
	BALLOON_ENTRY_PROC_WORK *entry;

	entry = PROC_AllocWork(proc, sizeof(BALLOON_ENTRY_PROC_WORK), HEAPID_BALLOON );
	MI_CpuClear8(entry, sizeof(BALLOON_ENTRY_PROC_WORK));
	
	entry->bsw = PROC_GetParentWork(proc);
	
	return PROC_RES_FINISH;
}

//--------------------------------------------------------------
/**
 * @brief   風船割りエントリー画面：メイン
 *
 * @param   proc		PROCへのポインタ
 * @param   seq			シーケンスワーク
 *
 * @retval  
 */
//--------------------------------------------------------------
PROC_RESULT BalloonEntryProc_Main( PROC * proc, int * seq )
{
	BALLOON_ENTRY_PROC_WORK * entry  = PROC_GetWork( proc );

	// 通信エラーチェック
	if( entry->bsw->dis_error == TRUE ){

		switch( entry->bsw->dis_error_seq ){
		case 0:
			// WIPE中なら中断して真っ暗にする、
			if( WIPE_SYS_EndCheck() == TRUE ){
				WIPE_SYS_ExeEnd();
			}
			WIPE_SetBrightnessFadeOut(WIPE_FADE_BLACK);
			entry->bsw->dis_error_seq ++;
			break;

		case 1:
			// まづは通信切断
			if( MNGM_ERROR_DisconnectWait( &entry->bsw->entry_param ) == TRUE ){
				entry->bsw->dis_error_seq ++;
			}
			break;

		default:
		case 2:
			// さらにワークを全部破棄する
			if( entry->mngm_entry != NULL ){
				MNGM_ENTRY_Exit(entry->mngm_entry);
				entry->mngm_entry = NULL;
			} 
			if( entry->mngm_result != NULL ){
				MNGM_RESULT_Exit(entry->mngm_result);
				entry->mngm_result = NULL;
			} 
			return PROC_RES_FINISH;
		}
		
		return PROC_RES_CONTINUE;
	}
	
	// メインシーケンス
	switch(*seq){
	case MAINSEQ_INIT:
		if(entry->bsw->mode == BALLOON_MODE_ENTRY){
			*seq = MAINSEQ_ENTRY_INIT;
		}
		else{
			*seq = MAINSEQ_RESULT_INIT;
		}
		break;
	
	//エントリー画面
	case MAINSEQ_ENTRY_INIT:
		entry->mngm_entry = MNGM_ENTRY_InitBalloon(&entry->bsw->entry_param, HEAPID_BALLOON );
		(*seq)++;
		break;
	case MAINSEQ_ENTRY_WAIT:
		if(MNGM_ENTRY_Wait(entry->mngm_entry) == TRUE){
			entry->bsw->raregame_type = MNGM_ENTRY_GetRareGame(entry->mngm_entry);
			MNGM_ENTRY_Exit(entry->mngm_entry);
			entry->mngm_entry = NULL;
			(*seq) = MAINSEQ_END;
		}
		break;

	//結果発表画面
	case MAINSEQ_RESULT_INIT:
		entry->mngm_result = MNGM_RESULT_InitBalloon(&entry->bsw->entry_param, 
			&entry->bsw->result_param, HEAPID_BALLOON );
		(*seq)++;
		break;
	case MAINSEQ_RESULT_WAIT:
		if(MNGM_RESULT_Wait(entry->mngm_result) == TRUE){
			entry->bsw->replay = MNGM_RESULT_GetReplay(entry->mngm_result);
			MNGM_RESULT_Exit(entry->mngm_result);
			entry->mngm_result = NULL;
			(*seq) = MAINSEQ_END;
		}
		break;
	
	//終了
	case MAINSEQ_END:
	default:
		return PROC_RES_FINISH;
	}
	
	return PROC_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   風船割りエントリー画面：終了処理
 *
 * @param   proc		PROCへのポインタ
 * @param   seq			シーケンスワーク
 *
 * @retval  
 */
//--------------------------------------------------------------
PROC_RESULT BalloonEntryProc_End(PROC *proc, int *seq)
{
	BALLOON_ENTRY_PROC_WORK * entry  = PROC_GetWork( proc );

	PROC_FreeWork( proc );				// PROCワーク開放

	return PROC_RES_FINISH;
}

