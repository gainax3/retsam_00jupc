//=============================================================================
/**
 * @file	comm_command_balloon.c
 * @brief	データを送るためのコマンドをテーブル化しています
 *          風船ミニゲーム用です
 *          comm_command_balloon.h の enum と同じ並びである必要があります
 * @author	matsuda
 * @date    2007.11.26(月)
 */
//=============================================================================
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
#include "communication/wm_icon.h"
#include "msgdata/msg.naix"
#include "system/wipe.h"
#include "communication/wm_icon.h"
#include "system/msgdata_util.h"
#include "system/procsys.h"
#include "system/d3dobj.h"
#include "system/fontoam.h"

#include "balloon_common.h"
#include "balloon_comm_types.h"
#include "balloon_game_types.h"
#include "application/balloon.h"
#include "balloon_game.h"
#include "balloon_tcb_pri.h"
#include "balloon_sonans.h"
#include "balloon_tool.h"
#include "comm_command_balloon.h"
#include "balloon_comm_types.h"
#include "balloon_send_recv.h"



//==============================================================================
//  テーブルに書く関数の定義
//==============================================================================
static	void Recv_CommTiming(int id_no,int size,void *pData,void *work);
static	void Recv_CommBalloonPlayData(int id_no,int size,void *pData,void *work);
static	void Recv_CommGameEnd(int id_no,int size,void *pData,void *work);
static	void Recv_CommServerVersion(int id_no,int size,void *pData,void *work);

//==============================================================================
//  static定義
//==============================================================================
static int _getGamePlaySize(void);
static int _getServerVersionSize(void);


//==============================================================================
//	テーブル宣言
//  comm_shar.h の enum と同じならびにしてください
//  CALLBACKを呼ばれたくない場合はNULLを書いてください
//  コマンドのサイズを返す関数を書いてもらえると通信が軽くなります
//  _getZeroはサイズなしを返します。_getVariableは可変データ使用時に使います
//==============================================================================
static const CommPacketTbl _CommPacketTbl[] = {
    {NULL,                      _getZero, 			NULL},	// CB_EXIT_BALLOON
	{Recv_CommServerVersion,	_getServerVersionSize,	NULL},	// CB_SERVER_VERSION
	{Recv_CommBalloonPlayData, 	_getGamePlaySize,	NULL},	// CB_PLAY_PARAM
	{Recv_CommGameEnd,		 	_getZero,			NULL},	// CB_GAME_END
};

//--------------------------------------------------------------
/**
 * @brief   コンテスト用のコマンド体系に初期化します
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------
void CommCommandBalloonInitialize(void* pWork)
{
    int length = sizeof(_CommPacketTbl)/sizeof(CommPacketTbl);
    CommCommandInitialize(_CommPacketTbl, length, pWork);
    
    GF_ASSERT(sizeof(BALLOON_SIO_PLAY_WORK) < 256);	//このサイズを超えたらHugeBuffにする必要がある
}

//--------------------------------------------------------------
/**
 * @brief   ３つともサイズを返します
 * @param   command         コマンド
 * @retval  サイズ   可変なら COMM_VARIABLE_SIZE Zeroは０を返す
 */
//--------------------------------------------------------------
static int _getGamePlaySize(void)
{
	return sizeof(BALLOON_SIO_PLAY_WORK);
}

//--------------------------------------------------------------
/**
 * @brief   サーバーバージョンサイズを返します
 * @param   command         コマンド
 * @retval  サイズ   可変なら COMM_VARIABLE_SIZE Zeroは０を返す
 */
//--------------------------------------------------------------
static int _getServerVersionSize(void)
{
	return sizeof(u32);
}





//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   同期取り通信受信処理
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		
 */
//--------------------------------------------------------------
static	void Recv_CommTiming(int id_no,int size,void *pData,void *work)
{
	;
}


//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   ゲームプレイ中データ通信受信処理
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		
 */
//--------------------------------------------------------------
static	void Recv_CommBalloonPlayData(int id_no,int size,void *pData,void *work)
{
	BALLOON_GAME_PTR game = work;
	
	BalloonSio_RecvBufferSet(game, id_no, pData);
}

//--------------------------------------------------------------
/**
 * @brief   ゲームプレイ中データ送信命令
 * @param   consys		コンテストシステムワークへのポインタ
 * @retval  TRUE:送信した。　FALSE:送信失敗
 */
//--------------------------------------------------------------
BOOL Send_CommBalloonPlayData(BALLOON_GAME_PTR game, BALLOON_SIO_PLAY_WORK *send_data)
{
	if(CommSendData(CB_PLAY_PARAM, send_data, sizeof(BALLOON_SIO_PLAY_WORK)) == TRUE){
		return TRUE;
	}
	return FALSE;
}


//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   ゲーム終了を伝える
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		
 */
//--------------------------------------------------------------
static	void Recv_CommGameEnd(int id_no,int size,void *pData,void *work)
{
	BALLOON_GAME_PTR game = work;
	
	game->game_end = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ゲーム終了メッセージ送信命令
 * @param   consys		コンテストシステムワークへのポインタ
 * @retval  TRUE:送信した。　FALSE:送信失敗
 */
//--------------------------------------------------------------
BOOL Send_CommGameEnd(BALLOON_GAME_PTR game)
{
	if(CommSendData(CB_GAME_END, NULL, 0) == TRUE){
		return TRUE;
	}
	return FALSE;
}

//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   サーバーバージョンを受信する
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		
 */
//--------------------------------------------------------------
static	void Recv_CommServerVersion(int id_no,int size,void *pData,void *work)
{
	BALLOON_GAME_PTR game = work;
	u32 *recv_data = pData;
	u32 server_version;
	int i;
	
	server_version = recv_data[0];
	for(i = 0; i < game->bsw->player_max; i++){
		if(game->bsw->player_netid[i] == id_no){
			game->recv_server_version[i] = server_version;
			break;
		}
	}
	GF_ASSERT(i < game->bsw->player_max);
	OS_TPrintf("受信：サーバーバージョン = %d(net_id=%d, 受信サイズ=%d)\n", server_version, id_no, size);
	game->recv_server_version_cnt++;
}

//--------------------------------------------------------------
/**
 * @brief   サーバーバージョン送信命令
 * @param   consys		コンテストシステムワークへのポインタ
 * @retval  TRUE:送信した。　FALSE:送信失敗
 */
//--------------------------------------------------------------
BOOL Send_CommServerVersion(BALLOON_GAME_PTR game)
{
	u32 server_version = BALLOON_SERVER_VERSION;
	
	if(CommSendData(CB_SERVER_VERSION, &server_version, sizeof(u32)) == TRUE){
		return TRUE;
	}
	return FALSE;
}
