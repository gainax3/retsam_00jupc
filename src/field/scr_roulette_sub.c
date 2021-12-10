//============================================================================================
/**
 * @file	scr_roulette_sub.c
 * @bfief	スクリプトコマンド：バトルルーレット(受付まわり)
 * @author	Satoshi Nohara
 * @date	07.09.06
 *
 * スクリプト関連とは切り離したい処理を置く
 */
//============================================================================================
#include "common.h"

#include <nitro/code16.h> 
#include "system/pm_str.h"
#include "system/msgdata.h"			//MSGMAN_GetString
#include "system/wordset.h"			//WordSet_RegistPlayerName
#include "gflib/strbuf_family.h"	//許可制

//通信
#include "communication/comm_def.h"
#include "communication/comm_tool.h"
#include "communication/comm_system.h"
#include "../frontier/comm_command_frontier.h"

#include "scr_roulette_sub.h"


//============================================================================================
//
//	プロトタイプ宣言
//
//============================================================================================
void CommFldRouletteRecvMonsNo(int id_no,int size,void *pData,void *work);


//============================================================================================
//
//	通信
//
//============================================================================================

//--------------------------------------------------------------
/**
 * @brief   基本情報 通信受信処理
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		FRONTIER_SYSTEMへのポインタ
 *
 * バトルステージワークを確保後、最初に受け取るデータ
 */
//--------------------------------------------------------------
void CommFldRouletteRecvMonsNo(int id_no,int size,void *pData,void *work)
{
	int i,num;
	FLDSCR_ROULETTE_COMM* wk = work;
	//const u16* recv_buf = pData;
	const FLDSCR_ROULETTE_COMM* recv_buf = pData;

	OS_Printf( "******フィールドステージ****** モンスターナンバー受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	num = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	OS_Printf( "size = %d\n", size );
	OS_Printf( "recv_buf[0] = %d\n", recv_buf[0] );

	//相手の手持ちのポケモンナンバーを取得(ペアワークに代入)
	wk->pair_monsno[0] = recv_buf->mine_monsno[0];
	wk->pair_monsno[1] = recv_buf->mine_monsno[1];
	return;
}


