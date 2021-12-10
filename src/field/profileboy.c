//=============================================================================
/**
 * @file	profileboy.c
 * @brief	プロフィール登録少年イベント処理
 * @author	nohara
 * @date	08.02.28
 *
 * ほとんどmsgboy.cと同じです
 */
//=============================================================================
#include "common.h"
#include "gflib/strbuf.h"
#include "gflib/bg_system.h"
#include "system/wipe.h"
#include "system/msgdata.h"
#include "system/wordset.h"
#include "system/window.h"
#include "system/fontproc.h"
#include "system/pms_data.h"
#include "savedata/misc.h"
#include "application/pms_input.h"
#include "msgdata/msg.naix"
#include "msgdata/msg_pmss_union.h"
#include "msgdata/msg_pms_word08.h"

#include "field/field.h"
#include "fieldsys.h"
#include "field_event.h"
#include "fieldmap.h"

#include "profileboy.h"


//----------------------------------------------
/**
 *	イベントワーク
 */
//----------------------------------------------
typedef struct {
	FIELDSYS_WORK*	fsys;
	WORDSET*		wordSet;
	PMS_DATA		pms;
	PMSI_PARAM*		pmsiParam;
	MISC*			misc;
	int             seq;
	int             menu_result;
	u16*			ret_wk;
}PROFILEBOY_EVENT_WORK;


//==============================================================
// Prototype
//==============================================================
static void delete_work( PROFILEBOY_EVENT_WORK* wk );
static BOOL GMEVENT_ProfileBoy( GMEVENT_CONTROL* event );


//------------------------------------------------------------------
/**
 * イベント開始
 *
 * @param   fsys		フィールドシステムワークポインタ
 *
 */
//------------------------------------------------------------------
void PROFILEBOY_EventStart( GMEVENT_CONTROL* event, u16* ret_wk )
{
	FIELDSYS_WORK* fsys = FieldEvent_GetFieldSysWork( event );
	PROFILEBOY_EVENT_WORK* wk = sys_AllocMemory( HEAPID_EVENT, sizeof(PROFILEBOY_EVENT_WORK) );

	wk->fsys		= fsys;
	wk->wordSet		= WORDSET_Create( HEAPID_EVENT );
	wk->pmsiParam	= PMSI_PARAM_Create(PMSI_MODE_SENTENCE, PMSI_GUIDANCE_DEFAULT,
										wk->fsys->savedata, HEAPID_EVENT );
	wk->misc		= SaveData_GetMisc( fsys->savedata );
	wk->ret_wk		= ret_wk;

#if 1
	//★
	PMSDAT_Init( &wk->pms, PMS_TYPE_UNION );
	//PMSDAT_Init( &misc->gds_self_introduction, PMS_TYPE_UNION );

	MISC_GetGdsSelfIntroduction( wk->misc, &wk->pms );
	//wk->pms.sentence_id = pmss_union_01;
	//wk->pms.word[0]		= PMSW_GetWordNumberByGmmID( NARC_msg_pms_word08_dat, pms_word08_100 );
	//wk->pms.word[1]		= PMS_WORD_NULL;
	//misc->gds_self_introduction.sentence_id = pmss_union_01;
	//misc->gds_self_introduction.word[0] = PMSW_GetWordNumberByGmmID(	NARC_msg_pms_word08_dat,
	//																	pms_word08_100 );
	//misc->gds_self_introduction.word[1] = PMS_WORD_NULL;
#endif

	//編集しなくても普通に終わるフラグをたてる
	PMSI_PARAM_SetNotEditEgnore( wk->pmsiParam );

	wk->seq = 0;
	FieldEvent_Call( event, GMEVENT_ProfileBoy, wk );
	return;
}

//------------------------------------------------------------------
/**
 * ワーク破棄
 *
 * @param   wk		
 *
 */
//------------------------------------------------------------------
static void delete_work( PROFILEBOY_EVENT_WORK* wk )
{
	PMSI_PARAM_Delete( wk->pmsiParam );
	WORDSET_Delete( wk->wordSet );
	sys_FreeMemoryEz( wk );
}

//------------------------------------------------------------------
/**
 * イベント関数本体
 *
 * @param   event		
 *
 * @retval  BOOL		
 */
//------------------------------------------------------------------
static BOOL GMEVENT_ProfileBoy( GMEVENT_CONTROL* event )
{
	enum {
		SEQ_SETUP_FIRST_MSG,
		SEQ_WAIT_PMS_INPUT,
		SEQ_WAIT_RETURN_FIELD,
		SEQ_WAIT_FADEIN_FIELD,
		SEQ_QUIT,
	};

	PROFILEBOY_EVENT_WORK* wk = FieldEvent_GetSpecialWork(event);

	switch( wk->seq ){
	case SEQ_SETUP_FIRST_MSG:
		//簡易会話入力画面の初期状態に使用するパラメータセット（文章モード用）
		PMSI_PARAM_SetInitializeDataSentence( wk->pmsiParam, &(wk->pms) );

		//一度結果を受け取った後、もう１度、入力画面用パラメータとして使えるようにする
		PMSI_PARAM_SetReuseState( wk->pmsiParam );

		FieldProc_SetPmsInput( wk->fsys, wk->pmsiParam );
		wk->seq = SEQ_WAIT_PMS_INPUT;
		break;

	case SEQ_WAIT_PMS_INPUT:
		if(FieldEvent_Cmd_WaitSubProcEnd(wk->fsys) == FALSE)
		{
			FieldEvent_Cmd_SetMapProc(wk->fsys);
			wk->seq = SEQ_WAIT_RETURN_FIELD;
		}
		break;

	case SEQ_WAIT_RETURN_FIELD:
		if(FieldEvent_Cmd_WaitMapProcStart(wk->fsys))
		{
			FieldFadeWipeSet( FLD_DISP_BRIGHT_BLACKIN );
			wk->seq = SEQ_WAIT_FADEIN_FIELD;
		}
		break;

	//フィールドにフェードイン
	case SEQ_WAIT_FADEIN_FIELD:
		if( WIPE_SYS_EndCheck() )
		{
			if( PMSI_PARAM_CheckCanceled(wk->pmsiParam) )
			{
				*wk->ret_wk = 0;
				OS_TPrintf("キャンセルされた = %d\n", *wk->ret_wk );
				wk->seq = SEQ_QUIT;
			}
			else
			{
				*wk->ret_wk = 1;
				OS_TPrintf("キャンセルされなかった = %d\n", *wk->ret_wk );
				PMSI_PARAM_GetInputDataSentence( wk->pmsiParam, &(wk->pms) );

				//{
					//簡易会話セット
					//PMS_WORD word = PMSDAT_GetWordNumber( &wk->pms, 0 );
					//if( word != PMS_WORD_NULL )
					//{
					//	WORDSET_RegisterPMSWord( wk->wordSet, 0, word );
						MISC_SetGdsSelfIntroduction( wk->misc, &wk->pms );
					//}
				//}

				wk->seq = SEQ_QUIT;
			}
		}
		break;

	//終了
	case SEQ_QUIT:
		delete_work( wk );
		return TRUE;

	}

	return FALSE;
}


