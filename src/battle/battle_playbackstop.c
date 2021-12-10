//==============================================================================
/**
 * @file	battle_playbackstop.c
 * @brief	戦闘録画再生中の停止ボタン制御
 * @author	matsuda
 * @date	2007.07.25(水)
 */
//==============================================================================
#include "common.h"
#include "battle/battle_common.h"
#include "battle_input.h"
#include "battle/battle_tcb_pri.h"
#include "battle/fight_tool.h"
#include "battle_playbackstop.h"
#include "system/snd_tool.h"
#include "battle_snd_def.h"


//==============================================================================
//	定数定義
//==============================================================================
///サーバーに停止フラグをセットする前に少しウェイトを入れる(押した時のSEを聞かせるため)
#define RPS_END_WAIT		(8)


//==============================================================================
//	構造体定義
//==============================================================================
///録画再生の停止ボタン制御システム構造体
typedef struct{
	BATTLE_WORK *bw;	///<バトルワークへのポインタ
	s16 seq;			///<現在のシーケンス番号
	s16 wait;			///<ウェイトカウンタ
}PLAYBACK_STOP_WORK;


//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static void PlayBackStop_Main(TCB_PTR tcb, void *work);



//--------------------------------------------------------------
/**
 * @brief   戦闘録画再生の停止ボタン制御システム作成
 * @param   bw		
 * @retval  制御システムのTCBポインタ
 */
//--------------------------------------------------------------
TCB_PTR PlayBackStopButton_Create(BATTLE_WORK *bw)
{
	PLAYBACK_STOP_WORK *psw;
	TCB_PTR tcb;
	
	psw = sys_AllocMemory(HEAPID_BATTLE, sizeof(PLAYBACK_STOP_WORK));
	MI_CpuClear8(psw, sizeof(PLAYBACK_STOP_WORK));
	
	psw->bw = bw;
	tcb = TCB_Add(PlayBackStop_Main, psw, TCBPRI_PLAYBACK_STOP);
	
	return tcb;
}

//--------------------------------------------------------------
/**
 * @brief   戦闘録画再生の停止ボタン制御システム解放
 * @param   tcb		制御システムのTCBポインタ
 */
//--------------------------------------------------------------
void PlayBackStopButton_Free(TCB_PTR tcb)
{
	PLAYBACK_STOP_WORK *psw;
	
	psw = TCB_GetWork(tcb);
	sys_FreeMemoryEz(psw);
	TCB_Delete(tcb);
}


//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   戦闘録画再生の停止ボタン制御システム：メイン処理
 *
 * @param   tcb			TCBへのポインタ
 * @param   work		PLAYBACK_STOP_WORK構造体
 */
//--------------------------------------------------------------
static void PlayBackStop_Main(TCB_PTR tcb, void *work)
{
	PLAYBACK_STOP_WORK *psw = work;
	enum{
		SEQ_INIT,
		SEQ_IN_EFF_WAIT,
		SEQ_MAIN,
		SEQ_WAIT,
		SEQ_END,
	};
	BI_PARAM_PTR bip;
	
	bip = BattleWorkGF_BGL_BIPGet(psw->bw);
	
	switch(psw->seq){
	case SEQ_INIT:
		GF_ASSERT(bip != NULL);
		
		{
			ARCHANDLE* hdl_bg, *hdl_obj;

			hdl_bg  = ArchiveDataHandleOpen( ARC_BATT_BG,  HEAPID_BATTLE ); 
			hdl_obj = ArchiveDataHandleOpen( ARC_BATT_OBJ, HEAPID_BATTLE );
			BINPUT_CreateBG(hdl_bg, hdl_obj, bip, BINPUT_TYPE_PLAYBACK_STOP, FALSE, NULL);
			ArchiveDataHandleClose( hdl_bg );
			ArchiveDataHandleClose( hdl_obj );
		}
		psw->seq++;
		break;
	case SEQ_IN_EFF_WAIT:
		if(BINPUT_EffectEndCheck(bip) == TRUE){
			psw->seq++;
		}
		break;
	case SEQ_MAIN:
		if(BattleWorkBattleRecStopFlagCheck(psw->bw) == TRUE){
			if(BINPUT_TouchCheck(bip) == TRUE){
				Snd_SePlay(BSE_TP_DECIDE);
				psw->seq++;
			}
		}
		break;
	case SEQ_WAIT:
		psw->wait++;
		if(psw->wait > RPS_END_WAIT){
			BattleWorkBattleRecStopFlagSet(psw->bw,BI_SYSMSG_STOP);
			psw->seq++;
		}
		break;

	case SEQ_END:	//全ての処理終了。Freeを呼ばれるのを待つ
		break;
	}
}

