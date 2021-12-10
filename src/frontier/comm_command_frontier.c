//=============================================================================
/**
 * @file	comm_command_frontier.c
 * @brief	フロンティア　データを送るためのコマンドをテーブル化
 *          comm_command_frontier.h の enum と同じ並びである必要があります
 * @author	nohara
 * @date    2007.07.17
 */
//=============================================================================
#include "common.h"
#include "communication/communication.h"
#include "system/pms_data.h"
#include "savedata/b_tower.h"
#include "savedata/frontier_savedata.h"
#include "battle/battle_common.h"
#include "gflib/strbuf_family.h"
#include "poketool/pokeparty.h"
#include "poketool/poke_tool.h"

#include "comm_command_frontier.h"
#include "frontier_tool.h"
#include "wifi_counter_tool.h"
#include "factory_tool.h"
#include "castle_tool.h"
#include "castle_def.h"

//受信関数が外にある
#include "factory/factory_sys.h"
#include "stage/stage_sys.h"
#include "castle/castle_sys.h"
#include "roulette/roulette_sys.h"
//#include "../field/scr_stage_sub.h"
//#include "../field/scr_castle_sub.h"
//#include "../field/scr_roulette_sub.h"
#include "../field/fieldobj.h"
#include "../field/scr_tool.h"

#include "frontier_comm.h"


//==============================================================================
//
//  テーブルに書く関数の定義
//
//==============================================================================
//タワー(フィールド、WiFi)
static void CommFrWiFiCounterTowerRecvBufPlayerData(int id_no,int size,void *pData,void *work);
BOOL CommFrWiFiCounterTowerSendBufTrainerData( BTOWER_SCRWORK* wk );
static void CommFrWiFiCounterTowerRecvBufTrainerData(int id_no,int size,void *pData,void *work);
static void CommFrWiFiCounterTowerRecvBufRetireSelect(int id_no,int size,void *pData,void *work);

#if 1
BOOL CommFrWiFiCounterSendBufBFNo( FRWIFI_SCRWORK* wk );
void CommFrWiFiCounterRecvBufBFNo(int id_no,int size,void *pData,void *work);
BOOL CommFrWiFiCounterSendBufRetireFlag( FRWIFI_SCRWORK* wk, u16 retire_flag );
void CommFrWiFiCounterRecvBufRetireFlag(int id_no,int size,void *pData,void *work);
BOOL CommFrWiFiCounterSendBufMonsNoItemNo( FRWIFI_SCRWORK* wk, u16 pos1, u16 pos2 );
void CommFrWiFiCounterRecvBufMonsNoItemNo(int id_no,int size,void *pData,void *work);
BOOL CommFrWiFiCounterSendBufStageRecordDel( FRWIFI_SCRWORK* wk, u16 stage_del_flag );
void CommFrWiFiCounterRecvBufStageRecordDel(int id_no,int size,void *pData,void *work);
BOOL CommFrWiFiCounterSendBufGameContinue( FRWIFI_SCRWORK* wk, u16 flag );
void CommFrWiFiCounterRecvBufGameContinue(int id_no,int size,void *pData,void *work);
#endif


//==============================================================================
//
//	テーブル宣言
//
//  comm_shar.h の enum と同じならびにしてください
//  CALLBACKを呼ばれたくない場合はNULLを書いてください
//  コマンドのサイズを返す関数を書いてもらえると通信が軽くなります
//  _getZeroはサイズなしを返します。_getVariableは可変データ使用時に使います
//
//==============================================================================
static const CommPacketTbl _CommPacketTbl[] = {
	//ファクトリー(フロンティア)
	{CommFactoryRecvBasicData,		_getVariable,	NULL},						//
	{CommFactoryRecvTrData,			_getVariable,	NULL},						// 
	{CommFactoryRecvRentalData,		_getVariable,	NULL},						// 
	{CommFactoryRecvEnemyPokeData,	_getVariable,	NULL},						//
	{CommFactoryRecvRetireFlag,		_getVariable,	NULL},						//
	{CommFactoryRecvTradeYesNoFlag,	_getVariable,	NULL},						//
	{CommFactoryRecvTemotiPokeData,	_getVariable,	NULL},						//
	
	//ファクトリー(レンタル、交換画面)
	{Factory_CommRecvBufBasicData,	_getVariable,	NULL},						//
	{Factory_CommRecvBufRentalData,	_getVariable,	NULL},						//
	{Factory_CommRecvBufCancelData,	_getVariable,	NULL},						//
	{Factory_CommRecvBufTradeFlag,	_getVariable,	NULL},						//

	//ステージ(フロンティア)
	{CommStageRecvMonsNo,			_getVariable,	NULL},						//
	{CommStageRecvBasicData,		_getVariable,	NULL},						//
	{CommStageRecvTrData,			_getVariable,	NULL},						//
	{CommStageRecvEnemyPokeData,	_getVariable,	NULL},						//
	{CommStageRecvRetireFlag,		_getVariable,	NULL},						//
	{CommStageRecvBufTemotiPokeData,_getVariable,	CommStageGetRecvDataBuff},	//巨大データ

	//ステージ(タイプ選択画面)
	{Stage_CommRecvBufBasicData,	_getVariable,	NULL},						//
	{Stage_CommRecvBufPokeTypeData,	_getVariable,	NULL},						//
	{Stage_CommRecvBufFinalAnswerData,_getVariable,	NULL},						//

	//キャッスル(フロンティア)
	{CommCastleRecvBufBasicData,	_getVariable,	NULL},						//
	{CommCastleRecvBufTrData,		_getVariable,	NULL},						// 
	{CommCastleRecvBufSelData,		_getVariable,	NULL},						// 
	{CommCastleRecvBufEnemyPokeData,_getVariable,	NULL},						//
	{CommCastleRecvBufRetireFlag,	_getVariable,	NULL},						//
	{CommCastleRecvBufTradeYesNoFlag,_getVariable,	NULL},						//
	//{CommCastleRecvBufTemotiPokeData,_getVariable,	NULL},						//
	{CommCastleRecvBufTemotiPokeData,_getVariable,	CommCastleGetRecvDataBuff},	//巨大データ

	//キャッスル(手持ちポケモン画面)
	{CastleMine_CommRecvBufBasicData,	_getVariable,	NULL},						//
	{CastleMine_CommRecvBufRankUpType,	_getVariable,	NULL},						//
	{CastleMine_CommRecvBufCsrPos,		_getVariable,	NULL},						//
	{CastleMine_CommRecvBufModoru,		_getVariable,	NULL},						//
	
	//キャッスル(敵トレーナー画面)
	{CastleEnemy_CommRecvBufBasicData,	_getVariable,	NULL},						//
	{CastleEnemy_CommRecvBufRankUpType,	_getVariable,	NULL},						//
	{CastleEnemy_CommRecvBufCsrPos,		_getVariable,	NULL},						//
	{CastleEnemy_CommRecvBufModoru,		_getVariable,	NULL},						//
	
	//フロンティアWiFi受付(フロンティア)
	{CommFrWiFiCounterRecvBufBFNo,		_getVariable,	NULL},						//
	{CommFrWiFiCounterRecvBufRetireFlag,_getVariable,	NULL},						//
	{CommFrWiFiCounterRecvBufMonsNoItemNo,_getVariable,	NULL},						//
	{CommFrWiFiCounterRecvBufStageRecordDel,_getVariable,	NULL},					//
	{CommFrWiFiCounterRecvBufGameContinue,_getVariable,	NULL},						//

	//タワー(フィールド、Wifi)
	{CommFrWiFiCounterTowerRecvBufPlayerData,_getVariable,	NULL},						//
	{CommFrWiFiCounterTowerRecvBufTrainerData,_getVariable,	NULL},						//
	{CommFrWiFiCounterTowerRecvBufRetireSelect,_getVariable,	NULL},					//

	//ルーレット(フロンティア)
	{CommRouletteRecvBufBasicData,	_getVariable,	NULL},						//
	{CommRouletteRecvBufTrData,		_getVariable,	NULL},						// 
	{CommRouletteRecvBufEnemyPokeData,_getVariable,	NULL},						//
	{CommRouletteRecvBufRetireFlag,	_getVariable,	NULL},						//
	{CommRouletteRecvBufTradeYesNoFlag,_getVariable,	NULL},						//
	//{CommRouletteRecvBufTemotiPokeData,_getVariable,	NULL},						//
	{CommRouletteRecvBufTemotiPokeData,_getVariable,	CommRouletteGetRecvDataBuff},	//巨大データ

	//ルーレット(ランクアップ画面)
	{Roulette_CommRecvBufBasicData,	_getVariable,	NULL},						//
	{Roulette_CommRecvBufRankUpType,	_getVariable,	NULL},						//
	{Roulette_CommRecvBufCsrPos,		_getVariable,	NULL},						//

};

//--------------------------------------------------------------
/**
 * @brief   フロンティア用のコマンド体系に初期化します
 *
 * @param   none
 *
 * @retval  none
 */
//--------------------------------------------------------------
void CommCommandFrontierInitialize(void* pWork)
{
    int length = sizeof(_CommPacketTbl)/sizeof(CommPacketTbl);
    CommCommandInitialize(_CommPacketTbl, length, pWork);
}


//==============================================================================
//
//	タワー(フィールド,WiFi)
//
//==============================================================================
	
//--------------------------------------------------------------
/**
 * @brief	recv_bufのバトルタワー送られてきたプレイヤーデータを取得
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CommFrWiFiCounterTowerRecvBufPlayerData(int id_no,int size,void *pData,void *work)
{
	u16	ret;
	int num;
	BTOWER_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******WIFI受付****** バトルタワー　プレイヤーデータを受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	ret = 0;
	num = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	wk->pare_sex = (u8)recv_buf[0];
	wk->pare_poke[0] = recv_buf[1];
	wk->pare_poke[1] = recv_buf[2];
	wk->pare_stage = recv_buf[3];

	wk->partner = 5+wk->pare_sex;

	OS_Printf("sio multi mem = %d,%d:%d,%d\n",wk->mem_poke[0],wk->mem_poke[1],wk->pare_poke[0],wk->pare_poke[1]);
	if(	wk->mem_poke[0] == wk->pare_poke[0] ||
		wk->mem_poke[0] == wk->pare_poke[1]){
		ret += 1;
	}
	if(	wk->mem_poke[1] == wk->pare_poke[0] ||
		wk->mem_poke[1] == wk->pare_poke[1]){
		ret += 2;
	}

	wk->check_work = ret;
	return;
}
	
//--------------------------------------------------------------
/**
 * @brief	抽選したトレーナーNoを子機に送信
 *
 * @param	wk			BTOWER_SCRWORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
BOOL CommFrWiFiCounterTowerSendBufTrainerData( BTOWER_SCRWORK* wk )
{
	int ret,size;

	size = (BTOWER_STOCK_TRAINER_MAX * 2);

	OS_Printf( "******WIFI受付****** バトルタワー　トレーナーNoを送信した\n" );

	MI_CpuCopy8( wk->trainer, wk->send_buf, size );

	if( CommSendData(FC_TOWER_TR_DATA,wk->send_buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief	recv_bufのバトルタワー送られてきたトレーナーデータを取得
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CommFrWiFiCounterTowerRecvBufTrainerData(int id_no,int size,void *pData,void *work)
{
	int num;
	BTOWER_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******WIFI受付****** バトルタワー　トレーナーNoを受信した\n" );
	OS_Printf( "id_no = %d\n", id_no );
	OS_Printf( "自分id = %d\n", CommGetCurrentID() );

	num = 0;
	wk->recieve_count++;
	OS_Printf( "wk->recieve_count = %d\n", wk->recieve_count );

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	//親は送信するだけなので受け取らない
	if( CommGetCurrentID() == COMM_PARENT_ID ){
		return;
	}

	MI_CpuCopy8( recv_buf, wk->trainer, BTOWER_STOCK_TRAINER_MAX*2 );
	OS_Printf(	"sio multi trainer01 = %d,%d:%d,%d\n",
				wk->trainer[0], wk->trainer[1], wk->trainer[2], wk->trainer[3] );
	OS_Printf(	"sio multi trainer02 = %d,%d:%d,%d\n",
				wk->trainer[4], wk->trainer[5], wk->trainer[6], wk->trainer[7] );
	OS_Printf(	"sio multi trainer03 = %d,%d:%d,%d\n",
				wk->trainer[8], wk->trainer[9], wk->trainer[10], wk->trainer[11] );
	OS_Printf(	"sio multi trainer04 = %d,%d\n",
				wk->trainer[12], wk->trainer[13] );
	return;
}


//--------------------------------------------------------------
/**
 * @brief	recv_bufのバトルタワー送られてきたリタイアするかどうかの結果を取得
 *
 * @param	
 *
 * @retval	0	リタイアしない
 * @retval	1	リタイアする
 */
//--------------------------------------------------------------
static void CommFrWiFiCounterTowerRecvBufRetireSelect(int id_no,int size,void *pData,void *work)
{
	int num;
	BTOWER_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******WIFI受付****** バトルタワー　トレーナーNoを受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	num = 0;
	wk->check_work = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	OS_Printf("sio multi retire = %d,%d\n",wk->retire_f,recv_buf[0]);
	if( wk->retire_f || recv_buf[0] ){
		wk->check_work = 1;
	}

	return;
}


//==============================================================================
//
//	フロンティアWiFi受付
//
//==============================================================================
#if 1
//--------------------------------------------------------------
/**
 * @brief	send_bufに選択した施設Noをセット
 *
 * @param	wk		FRWIFI_SCRWORK型のポインタ
 *
 * @retval  TRUE:送信した。　FALSE:送信失敗
 */
//--------------------------------------------------------------
BOOL CommFrWiFiCounterSendBufBFNo( FRWIFI_SCRWORK* wk )
{
	int ret,size,id;

	OS_Printf( "******WIFI受付****** 選択した施設No送信\n" );

	size = FRWIFI_COMM_BUF_LEN;									//実際はu16データなのでこの半分

	wk->send_buf[0]	= wk->bf_no;								//施設No

#if 1
	//連勝数を取得
	id = WifiCounterRensyouRecordIDGet( wk->bf_no );
	wk->send_buf[1] = FrontierRecord_Get(	SaveData_GetFrontier(wk->sv), id, 
											Frontier_GetFriendIndex(id) );
#endif

	if( CommSendData(FC_WIFI_COUNTER_NO,wk->send_buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief	recv_bufの選択した施設Noを取得
 *
 * @param	wk			FRWIFI_SCRWORK型のポインタ
 * @param	recv_buf	
 *
 * @return	none
 */
//--------------------------------------------------------------
void CommFrWiFiCounterRecvBufBFNo(int id_no,int size,void *pData,void *work)
{
	u16 id;
	FRWIFI_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******WIFI受付****** 選択した施設No受信\n" );

	wk->recieve_count++;
	//OS_Printf( "wk->recieve_count = %d\n", wk->recieve_count );

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	wk->pair_bf_no = (u8)recv_buf[0];							//施設No
	OS_Printf( "受信：wk->pair_bf_no = %d\n", wk->pair_bf_no );

	//連勝数を取得
	wk->pair_rensyou = recv_buf[1];								//連勝数
	OS_Printf( "受信：wk->pair_rensyou = %d\n", wk->pair_rensyou );
	return;
}


//--------------------------------------------------------------
/**
 * @brief	send_bufに選択肢で何を選んだかをセット
 *
 * @param	wk		FRWIFI_SCRWORK型のポインタ
 * @param	sel		0=つづける、0以外=リタイヤ(きろくするはいらない)
 *
 * @return	none
 *
 * 両方、どちらかが「リタイヤ」の時は中止
 */
//--------------------------------------------------------------
BOOL CommFrWiFiCounterSendBufRetireFlag( FRWIFI_SCRWORK* wk, u16 retire_flag )
{
	int ret,size;

	size = FRWIFI_COMM_BUF_LEN;								//実際はu16データなのでこの半分

	OS_Printf( "******WIFI受付****** リタイア送信\n" );

	wk->send_buf[0] = retire_flag;
	OS_Printf( "送信：retire_flag = %d\n", wk->send_buf[0] );

	if( CommSendData(FC_WIFI_COUNTER_RETIRE_FLAG,wk->send_buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief	recv_bufの選択肢で何を選んだかを取得
 *
 * @param	wk			FRWIFI_SCRWORK型のポインタ
 * @param	recv_buf	
 *
 * @return	none
 *
 * 両方、どちらかが「リタイヤ」の時は中止
 */
//--------------------------------------------------------------
void CommFrWiFiCounterRecvBufRetireFlag(int id_no,int size,void *pData,void *work)
{
	int num;
	FRWIFI_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******WIFI受付****** リタイア受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	num = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	wk->pair_retire_flag = (u8)recv_buf[0];
	OS_Printf( "受信：wk->pair_retire_flag = %d\n", wk->pair_retire_flag );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	send_bufにモンスターナンバーをセット
 *
 * @param	wk		FRWIFI_SCRWORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
BOOL CommFrWiFiCounterSendBufMonsNoItemNo( FRWIFI_SCRWORK* wk, u16 pos1, u16 pos2 )
{
	int ret,size;
	POKEMON_PARAM* pp;
	POKEPARTY* party;

	size = FRWIFI_COMM_BUF_LEN;								//実際はu16データなのでこの半分

	OS_Printf( "******WIFI受付****** モンスターナンバー送信\n" );

	party = SaveData_GetTemotiPokemon( wk->sv );

	OS_Printf( "pos1 = %d\n", pos1 );
	OS_Printf( "pos2 = %d\n", pos2 );
	wk->mine_poke_pos[0] = pos1;
	wk->mine_poke_pos[1] = pos2;
	
	//ポケモンリストをキャンセルしていたら
	if( pos1 == 0xff ){
		wk->monsno[0] = 0;
		wk->itemno[0] = 0;
		wk->monsno[1] = 0;
		wk->itemno[1] = 0;
	}else{
		pp = PokeParty_GetMemberPointer( party, pos1 );
		wk->monsno[0] = PokeParaGet( pp, ID_PARA_monsno, NULL );
		wk->itemno[0] = PokeParaGet( pp, ID_PARA_item, NULL );

		pp = PokeParty_GetMemberPointer( party, pos2 );
		wk->monsno[1] = PokeParaGet( pp, ID_PARA_monsno, NULL );
		wk->itemno[1] = PokeParaGet( pp, ID_PARA_item, NULL );
	}

	wk->send_buf[0] = wk->monsno[0];
	wk->send_buf[1] = wk->itemno[0];
	OS_Printf( "送信：monsno1 = %d\n", wk->send_buf[0] );
	OS_Printf( "送信：itemno1 = %d\n", wk->send_buf[1] );

	////////////////////////////////////////////////////////////
	wk->send_buf[2] = wk->monsno[1];
	wk->send_buf[3] = wk->itemno[1];
	OS_Printf( "送信：monsno2 = %d\n", wk->send_buf[2] );
	OS_Printf( "送信：itemno2 = %d\n", wk->send_buf[3] );

	if( CommSendData(FC_WIFI_COUNTER_MONSNO_ITEMNO,wk->send_buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief	recv_bufのモンスターナンバーを取得
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		FRONTIER_SYSTEMへのポインタ
 */
//--------------------------------------------------------------
void CommFrWiFiCounterRecvBufMonsNoItemNo(int id_no,int size,void *pData,void *work)
{
	FRWIFI_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******WIFI受付****** モンスターナンバー受信\n" );

	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	//相手の手持ちのポケモンナンバーを取得(ペアワークに代入)
	wk->pair_monsno[0] = recv_buf[0];
	wk->pair_itemno[0] = recv_buf[1];
	wk->pair_monsno[1] = recv_buf[2];
	wk->pair_itemno[1] = recv_buf[3];
	OS_Printf( "受信：wk->pair_monsno[0] = %d\n", wk->pair_monsno[0] );
	OS_Printf( "受信：wk->pair_itemno[0] = %d\n", wk->pair_itemno[0] );
	OS_Printf( "受信：wk->pair_monsno[1] = %d\n", wk->pair_monsno[1] );
	OS_Printf( "受信：wk->pair_itemno[1] = %d\n", wk->pair_itemno[1] );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	send_bufにステージの記録を消してよいかをセット
 *
 * @param	wk		FRWIFI_SCRWORK型のポインタ
 * @param	sel		0=つづける、0以外=リタイヤ(きろくするはいらない)
 *
 * @return	none
 *
 * 両方、どちらかが「いいえ」の時は中止
 */
//--------------------------------------------------------------
BOOL CommFrWiFiCounterSendBufStageRecordDel( FRWIFI_SCRWORK* wk, u16 stage_del_flag )
{
	int ret,size;

	size = FRWIFI_COMM_BUF_LEN;								//実際はu16データなのでこの半分

	OS_Printf( "******WIFI受付****** ステージ記録消してよいか送信\n" );

	wk->send_buf[0] = stage_del_flag;
	OS_Printf( "送信：stage_del_flag = %d\n", wk->send_buf[0] );

	if( CommSendData(FC_WIFI_COUNTER_STAGE_RECORD_DEL,wk->send_buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief	recv_bufのステージの記録を消してよいかを取得
 *
 * @param	wk			FRWIFI_SCRWORK型のポインタ
 * @param	recv_buf	
 *
 * @return	none
 *
 * 両方、どちらかが「いいえ」の時は中止
 */
//--------------------------------------------------------------
void CommFrWiFiCounterRecvBufStageRecordDel(int id_no,int size,void *pData,void *work)
{
	int num;
	FRWIFI_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******WIFI受付****** ステージ記録消してよいか受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	num = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	wk->pair_stage_del = recv_buf[0];
	OS_Printf( "受信：wk->pair_stage_del = %d\n", wk->pair_stage_del );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	send_bufに引き続き遊ぶかをセット
 *
 * @param	wk		FRWIFI_SCRWORK型のポインタ
 * @param	sel		0=はい、1=いいえ
 *
 * @return	none
 *
 * 両方、どちらかが「いいえ」の時は中止
 */
//--------------------------------------------------------------
BOOL CommFrWiFiCounterSendBufGameContinue( FRWIFI_SCRWORK* wk, u16 flag )
{
	int ret,size;

	size = FRWIFI_COMM_BUF_LEN;								//実際はu16データなのでこの半分

	OS_Printf( "******WIFI受付****** 引き続き遊ぶかを送信\n" );

	wk->send_buf[0] = flag;
	OS_Printf( "送信：game_continue = %d\n", wk->send_buf[0] );

	if( CommSendData(FC_WIFI_COUNTER_GAME_CONTINUE,wk->send_buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief	recv_bufの引き続き遊ぶかを取得
 *
 * @param	wk			FRWIFI_SCRWORK型のポインタ
 * @param	recv_buf	
 *
 * @return	none
 *
 * 両方、どちらかが「いいえ」の時は中止
 */
//--------------------------------------------------------------
void CommFrWiFiCounterRecvBufGameContinue(int id_no,int size,void *pData,void *work)
{
	int num;
	FRWIFI_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******WIFI受付****** 引き続き遊ぶかを受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	num = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	wk->pair_game_continue = (u8)recv_buf[0];
	OS_Printf( "受信：wk->pair_game_continue = %d\n", wk->pair_game_continue );
	return;
}
#endif


