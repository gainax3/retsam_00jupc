//==============================================================================
/**
 * @file	footprint_comm.c
 * @brief	足跡ボードの通信送受信系
 * @author	matsuda
 * @date	2008.01.21(月)
 */
//==============================================================================
#include "common.h"
#include "system/arc_tool.h"
#include "application/wifi_lobby/wflby_system_def.h"
#include "footprint_common.h"
#include "footprint_stamp.h"
#include "include/wifi/dwc_lobbylib.h"
#include "footprint_comm.h"


//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static void COMMCMD_PlayerIn(s32 userid, const void *cp_data, u32 size, void *p_work);
static void COMMCMD_PlayerOut(s32 userid, const void *cp_data, u32 size, void *p_work);
static void COMMCMD_Stamp(s32 userid, const void *cp_data, u32 size, void *p_work);

static BOOL RecvDataCheck(s32 userid, FOOTPRINT_MY_COMM_STATUS *my_comm);



//==============================================================================
//	通信データテーブル
//==============================================================================

//--------------------------------------------------------------
/**
 *	通信受信テーブル		※COMM_FOOTPRINTと並びを同じにしておくこと！！
 */
//--------------------------------------------------------------
static const DWC_LOBBY_MSGCOMMAND MsgCommandTbl_Footprint[] = {
	{ COMMCMD_PlayerIn,						sizeof(FOOTPRINT_IN_PARAM) },
	{ COMMCMD_PlayerOut,					0 },
	{ COMMCMD_Stamp,						sizeof(STAMP_PARAM) },
	
	// 会話
//	{ WFLBY_SYSTEM_COMMCMD_TALK_Req,		sizeof(WFLBY_SYSTEM_TALK_DATA) },
//	{ WFLBY_SYSTEM_COMMCMD_TALK_Ans,		sizeof(WFLBY_SYSTEM_TALK_DATA) },
//	{ WFLBY_SYSTEM_COMMCMD_TALK_Data,		sizeof(WFLBY_SYSTEM_TALK_DATA) },
//	{ WFLBY_SYSTEM_COMMCMD_TALK_End,		sizeof(WFLBY_SYSTEM_TALK_DATA) },

	// トピック
//	{ WFLBY_SYSTEM_COMMCMD_TOPIC_Data,		sizeof(WFLBY_TOPIC) },
};

///足跡ボードの通信コマンド最大数
#define FOOTPRINT_COMM_MSGTBL_MAX		(NELEMS(MsgCommandTbl_Footprint))

///コマンド発行番号
typedef enum{
	COMM_FOOTPRINT_PLAYER_IN,	///<入室しました
	COMM_FOOTPRINT_PLAYER_OUT,	///<退室しました
	COMM_FOOTPRINT_STAMP,		///<スタンプ発行
}COMM_FOOTPRINT;



//==============================================================================
//
//	システム
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   通信システム初期設定
 *
 * @param   fps		
 */
//--------------------------------------------------------------
void Footprint_Comm_Init(FOOTPRINT_SYS_PTR fps)
{
	DWC_LOBBY_SUBCHAN_SetMsgCmd(MsgCommandTbl_Footprint, FOOTPRINT_COMM_MSGTBL_MAX, fps);
}

//--------------------------------------------------------------
/**
 * @brief   受信したデータが受け取れる状態かチェック
 *
 * @param   userid		ユーザーID
 * @param   my_comm		自分の通信ステータス
 *
 * @retval  TRUE:受信OK
 * @retval  FALSE:受信拒否
 */
//--------------------------------------------------------------
static BOOL RecvDataCheck(s32 userid, FOOTPRINT_MY_COMM_STATUS *my_comm)
{
	if(my_comm->user_id == userid){
		OS_TPrintf("自分のデータ受信\n");
		return FALSE;	//自分のデータなので無視
	}
	if(my_comm->ready == FALSE){
		OS_TPrintf("ready状態になっていないので受け取り拒否\n");
		return FALSE;
	}
	if(DWC_LOBBY_SUBCHAN_GetUserIDIdx(userid) == DWC_LOBBY_USERIDTBL_IDX_NONE){
		OS_TPrintf("まだサーバーのシステムに反映されていないユーザーなので拒否\n");
		return FALSE;
	}
	
	return TRUE;
}

//==============================================================================
//
//	送信命令
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   送信：入室しました
 *
 * @param   in_para		入室パラメータ
 *
 * @retval  TRUE:送信成功、　FALSE:失敗
 */
//--------------------------------------------------------------
BOOL Footprint_Send_PlayerIn(const FOOTPRINT_IN_PARAM *in_para)
{
	DWC_LOBBY_SUBCHAN_SendMsg(COMM_FOOTPRINT_PLAYER_IN, in_para, sizeof(FOOTPRINT_IN_PARAM));
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   送信：退室しました
 *
 * @retval  TRUE:送信成功、　FALSE:失敗
 */
//--------------------------------------------------------------
BOOL Footprint_Send_PlayerOut(void)
{
	DWC_LOBBY_SUBCHAN_SendMsg(COMM_FOOTPRINT_PLAYER_OUT, NULL, 0);
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   送信：スタンプ発行
 *
 * @param   stamp		スタンプパラメータ
 *
 * @retval  TRUE:送信成功、　FALSE:失敗
 */
//--------------------------------------------------------------
BOOL Footprint_Send_Stamp(const STAMP_PARAM *stamp)
{
	if(WFLBY_ERR_CheckError() == TRUE){
		return FALSE;
	}
	DWC_LOBBY_SUBCHAN_SendMsg(COMM_FOOTPRINT_STAMP, stamp, sizeof(STAMP_PARAM));
	return TRUE;
}


//==============================================================================
//
//	受信コールバック
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   入室しました受信
 *
 * @param   userid		
 * @param   cp_data		
 * @param   size		
 * @param   p_work		
 */
//--------------------------------------------------------------
static void COMMCMD_PlayerIn(s32 userid, const void *cp_data, u32 size, void *p_work)
{
	FOOTPRINT_SYS_PTR fps = p_work;
	FOOTPRINT_MY_COMM_STATUS *my_comm = Footprint_MyCommStatusGet(fps);
	const FOOTPRINT_IN_PARAM *in_para = cp_data;
	
	OS_TPrintf("入室受信\n");
	if(RecvDataCheck(userid, my_comm) == FALSE){
		return;
	}
	
	Footprint_NameWrite(fps, userid);
}

//--------------------------------------------------------------
/**
 * @brief   退室しました受信
 *
 * @param   userid		
 * @param   cp_data		
 * @param   size		
 * @param   p_work		
 */
//--------------------------------------------------------------
static void COMMCMD_PlayerOut(s32 userid, const void *cp_data, u32 size, void *p_work)
{
	FOOTPRINT_SYS_PTR fps = p_work;
	FOOTPRINT_MY_COMM_STATUS *my_comm = Footprint_MyCommStatusGet(fps);
	
	OS_TPrintf("退室受信\n");
	if(RecvDataCheck(userid, my_comm) == FALSE){
		return;
	}
	
	Footprint_NameErase(fps, DWC_LOBBY_SUBCHAN_GetUserIDIdx(userid));
}

//--------------------------------------------------------------
/**
 * @brief   押したスタンプ受信
 *
 * @param   userid		
 * @param   cp_data		
 * @param   size		
 * @param   p_work		
 */
//--------------------------------------------------------------
static void COMMCMD_Stamp(s32 userid, const void *cp_data, u32 size, void *p_work)
{
	FOOTPRINT_SYS_PTR fps = p_work;
	FOOTPRINT_MY_COMM_STATUS *my_comm = Footprint_MyCommStatusGet(fps);
	const STAMP_PARAM *stamp = cp_data;
	
	OS_TPrintf("スタンプ受信\n");
	if(RecvDataCheck(userid, my_comm) == FALSE){
		return;
	}
	
	Footprint_StampAdd(fps, stamp, userid);
}

#if 0
// 通信コールバック
//----------------------------------------------------------------------------
/**
 *	@brief	会話リクエスト
 */
//-----------------------------------------------------------------------------
static void WFLBY_SYSTEM_COMMCMD_TALK_Req( s32 userid, const void* cp_data, u32 size, void* p_work )
{
	WFLBY_SYSTEM* p_wk = p_work;
	u32 idx;
	const WFLBY_SYSTEM_TALK_DATA* cp_talkdata;
	u32 status;

	// データ取得
	cp_talkdata = cp_data;

	// その人のIDXを取得
	idx = DWC_LOBBY_GetUserIDIdx( userid );

	
	status = WFLBY_SYSTEM_GetProfileStatus( &p_wk->myprofile );
	
	// 今会話できる状態かチェック
	// 状態が忙しいか、知らない人ならごめんなさい
	// その人から何度も話しかけられている場合
	if( (status != WFLBY_STATUS_LOGIN) ||
		(idx == DWC_LOBBY_USERIDTBL_IDX_NONE) ||
		(cp_talkdata->seq != WFLBY_TALK_SEQ_B_ANS) ||
		(WFLBY_SYSTEM_CheckTalkCount( p_wk, idx ) == FALSE)){

		// 会話できないことを送信
		OS_TPrintf( "talk don't  user=%d\n", userid );
		WFLBY_SYSTEM_TALK_SendTalkAns( p_wk, userid, FALSE );
		return ;
	}

	// 会話できるようなので、応答する
	WFLBY_SYSTEM_TALK_SendTalkAns( p_wk, userid, TRUE );

	// 初めてかチェック
	if( WFLBY_SYSTEM_TALK_CheckMsk( &p_wk->talk, idx ) == FALSE ){
		p_wk->talk.talk_first = TRUE;
	}else{
		p_wk->talk.talk_first = FALSE;
	}

	// 状態を会話にする
	WFLBY_SYSTEM_TALK_SetMsk( &p_wk->talk, idx );
	p_wk->talk.talk_idx		= idx;
	p_wk->talk.talk_seq		= WFLBY_TALK_SEQ_A_SEL;
	p_wk->talk.talk_type	= WFLBY_TALK_TYPE_B;
	p_wk->talk.talk_b_start	= FALSE;

	// 会話応答待ち開始
	WFLBY_SYSTEM_TALK_StartRecvWait( &p_wk->talk );

	OS_TPrintf( "talk match  user=%d\n", userid );
}

#endif
