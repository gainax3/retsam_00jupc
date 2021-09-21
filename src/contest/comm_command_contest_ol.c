//=============================================================================
/**
 * @file	comm_command_contest_ol.c
 * @brief	comm_command_contest.cからコンテスト領域にオーバーレイしても問題ないものを
 *			抜き出したファイルです。
 * @author	matsuda
 * @date    2007.12.04(火)
 */
//=============================================================================
#include "common.h"
#include "communication/communication.h"

#include "contest/contest.h"
#include "comm_command_contest.h"
#include "actin.h"
#include "visual.h"
#include "dance.h"
#include "con_result.h"
#include "contest_order.h"
#include "dance_tool.h"
#include "gflib/strbuf_family.h"
#include "savedata/perapvoice.h"

#include "comm_command_contest_ol.h"


//--------------------------------------------------------------
/**
 * @brief   ノーマルデータ通信受信処理
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		
 */
//--------------------------------------------------------------
void CommContestRecvNormal(int id_no,int size,void *pData,void *work)
{
	CONTEST_SYSTEM *consys = work;
	CONTEST_ORDER_WORK *cow;
	
	if(consys->class_flag == CON_CLASS_VISUAL){
		VISUAL_PROC_WORK *vpw;
		vpw = consys->class_proc;
		cow = &vpw->cow;
		OS_TPrintf("ビジュアル部門データ受信。id_no=%d, size=%d\n", id_no, size);
	}
	else if(consys->class_flag == CON_CLASS_DANCE){
		DANCE_PROC_WORK *dpw;
		dpw = consys->class_proc;
		cow = &dpw->cow;
		OS_TPrintf("ダンス部門データ受信。id_no=%d, size=%d\n", id_no, size);
	}
	else if(consys->class_flag == CON_CLASS_ACTIN){
		ACTIN_PROC_WORK *apw;
		apw = consys->class_proc;
		cow = &apw->cow;
		OS_TPrintf("演技力部門データ受信。id_no=%d, size=%d\n", id_no, size);
	}
	else if(consys->class_flag == CON_CLASS_RESULT){
		CONRES_PROC_WORK *rpw;
		rpw = consys->class_proc;
		cow = &rpw->cow;
		OS_TPrintf("結果発表データ受信。id_no=%d, size=%d\n", id_no, size);
	}
	else{
		GF_ASSERT(0 && "不明なクラスです");
		return;
	}

	GF_ASSERT(size <= CO_TRANSMIT_BUF_SIZE);
	GF_ASSERT(cow->receive_flag[id_no] == FALSE);
	MI_CpuCopy8(pData, cow->receive_buf[id_no], size);
	cow->receive_flag[id_no] = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ダンスデータ通信受信処理
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		
 */
//--------------------------------------------------------------
void CommContestRecvDance(int id_no,int size,void *pData,void *work)
{
	CONTEST_SYSTEM *consys = work;
	DANCE_PROC_WORK *dpw;
	DANCING_PARAM *dancing_param;
	
	dpw = consys->class_proc;
	dancing_param = pData;
	
	DT_DancingParamRecieve(dpw, dancing_param);
}
