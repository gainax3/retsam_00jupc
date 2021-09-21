//============================================================================================
/**
 * @file	scr_stage_sub.c
 * @bfief	スクリプトコマンド：バトルステージ(受付まわり)
 * @author	Satoshi Nohara
 * @date	06.07.19
 *
 * スクリプト関連とは切り離したい処理を置く
 */
//============================================================================================
#include "common.h"

#include <nitro/code16.h> 
#include "system/pm_str.h"
#include "system\msgdata.h"			//MSGMAN_GetString
#include "system\wordset.h"			//WordSet_RegistPlayerName
#include "gflib/strbuf_family.h"	//許可制

//通信
#include "communication/comm_def.h"
#include "communication/comm_tool.h"
#include "communication/comm_system.h"
#include "../frontier/comm_command_frontier.h"

#include "savedata/frontier_savedata.h"
#include "scr_stage_sub.h"
#include "scr_tool.h"


//============================================================================================
//
//	プロトタイプ宣言
//
//============================================================================================
void CommFldStageRecvMonsNo(int id_no,int size,void *pData,void *work);
void BattleStageSetNewChallenge( SAVEDATA* sv, STAGESCORE* wk, u8 type );


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
void CommFldStageRecvMonsNo(int id_no,int size,void *pData,void *work)
{
	int i,num;
	FLDSCR_STAGE_COMM* wk = work;
	//const u16* recv_buf = pData;
	const FLDSCR_STAGE_COMM* recv_buf = pData;

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
	wk->pair_monsno = recv_buf->mine_monsno;
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		連勝中フラグクリア、連勝レコードクリア、タイプカウントクリア
 * @param		core
 */
//--------------------------------------------------------------------------------------------
void BattleStageSetNewChallenge( SAVEDATA* sv, STAGESCORE* wk, u8 type )
{
	int i;
	u8 buf8[4];
	u16 l_num,h_num;

	//"10連勝(クリア)したかフラグ"のクリアを書き出し
	buf8[0] = 0;
	STAGESCORE_PutScoreData( wk, STAGESCORE_ID_CLEAR_FLAG, type, 0, buf8 );

	//WIFIのみ特殊
	if( type == STAGE_TYPE_WIFI_MULTI ){
		FrontierRecord_Set(	SaveData_GetFrontier(sv), 
							FRID_STAGE_MULTI_WIFI_CLEAR_BIT,
							Frontier_GetFriendIndex(FRID_STAGE_MULTI_WIFI_CLEAR_BIT), 0 );
	}

	//"現在の連勝数"を0にする
	FrontierRecord_Set(	SaveData_GetFrontier(sv), 
						StageScr_GetWinRecordID(type),
						Frontier_GetFriendIndex(StageScr_GetWinRecordID(type)), 0 );

	//fssc_stage_sub.cに移動
	//現在のバトルタイプのタイプレベルをクリアする
	//for( i=0; i < STAGE_TR_TYPE_MAX ;i++ ){
	//	StageScr_TypeLevelRecordSet( sv, type, i, 0 );
	//}

	return;
}

