//=============================================================================
/**
 * @file	frontier_comm.c
 * @brief	comm_command_frontier.cから呼ばれる通信処理(非常中)
 * @author	nohara
 * @date    2007.12.05
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
#include "factory_tool.h"
#include "castle_tool.h"
#include "castle_def.h"

//受信関数が外にある
#include "factory/factory_sys.h"
#include "stage/stage_sys.h"
#include "castle/castle_sys.h"
#include "roulette/roulette_sys.h"
#include "../field/scr_stage_sub.h"
#include "../field/scr_castle_sub.h"
#include "../field/scr_roulette_sub.h"
#include "../field/fieldobj.h"
#include "../field/scr_tool.h"


//==============================================================================
//
//  テーブルに書く関数の定義
//
//==============================================================================
//ファクトリー
BOOL		CommFactorySendBasicData( FACTORY_SCRWORK* wk );
void CommFactoryRecvBasicData(int id_no,int size,void *pData,void *work);
BOOL		CommFactorySendTrData( FACTORY_SCRWORK* wk );
void CommFactoryRecvTrData(int id_no,int size,void *pData,void *work);
BOOL		CommFactorySendRentalData( FACTORY_SCRWORK* wk );
void CommFactoryRecvRentalData(int id_no,int size,void *pData,void *work);
BOOL		CommFactorySendEnemyPokeData( FACTORY_SCRWORK* wk );
void CommFactoryRecvEnemyPokeData(int id_no,int size,void *pData,void *work);
BOOL		CommFactorySendRetireFlag( FACTORY_SCRWORK* wk, u8 retire_flag );
void CommFactoryRecvRetireFlag(int id_no,int size,void *pData,void *work);
BOOL		CommFactorySendTradeYesNoFlag( FACTORY_SCRWORK* wk, u8 trade_yesno_flag );
void CommFactoryRecvTradeYesNoFlag(int id_no,int size,void *pData,void *work);
BOOL		CommFactorySendTemotiPokeData( FACTORY_SCRWORK* wk );
void CommFactoryRecvTemotiPokeData(int id_no,int size,void *pData,void *work);

//ステージ
BOOL		CommStageSendMonsNo( STAGE_SCRWORK* wk, const POKEMON_PARAM* pp );
void CommStageRecvMonsNo(int id_no,int size,void *pData,void *work);
BOOL		CommStageSendBasicData( STAGE_SCRWORK* wk );
void CommStageRecvBasicData(int id_no,int size,void *pData,void *work);
BOOL		CommStageSendTrData( STAGE_SCRWORK* wk );
void CommStageRecvTrData(int id_no,int size,void *pData,void *work);
BOOL		CommStageSendEnemyPokeData( STAGE_SCRWORK* wk );
void CommStageRecvEnemyPokeData(int id_no,int size,void *pData,void *work);
BOOL		CommStageSendRetireFlag( STAGE_SCRWORK* wk, u8 retire_flag );
void CommStageRecvRetireFlag(int id_no,int size,void *pData,void *work);
BOOL		CommStageSendBufTemotiPokeData( STAGE_SCRWORK* wk );
void CommStageRecvBufTemotiPokeData(int id_no,int size,void *pData,void *work);
u8* CommStageGetRecvDataBuff( int netID, void*pWork, int size );

//キャッスル
BOOL		CommCastleSendBufBasicData( CASTLE_SCRWORK* wk );
void CommCastleRecvBufBasicData(int id_no,int size,void *pData,void *work);
BOOL		CommCastleSendBufTrData( CASTLE_SCRWORK* wk );
void CommCastleRecvBufTrData(int id_no,int size,void *pData,void *work);
BOOL		CommCastleSendBufSelData( CASTLE_SCRWORK* wk );
void CommCastleRecvBufSelData(int id_no,int size,void *pData,void *work);
BOOL		CommCastleSendBufEnemyPokeData( CASTLE_SCRWORK* wk );
void CommCastleRecvBufEnemyPokeData(int id_no,int size,void *pData,void *work);
BOOL		CommCastleSendBufRetireFlag( CASTLE_SCRWORK* wk, u16 retire_flag );
void CommCastleRecvBufRetireFlag(int id_no,int size,void *pData,void *work);
BOOL		CommCastleSendBufTradeYesNoFlag( CASTLE_SCRWORK* wk, u16 trade_yesno_flag );
void CommCastleRecvBufTradeYesNoFlag(int id_no,int size,void *pData,void *work);
BOOL		CommCastleSendBufTemotiPokeData( CASTLE_SCRWORK* wk );
void CommCastleRecvBufTemotiPokeData(int id_no,int size,void *pData,void *work);
u8* CommCastleGetRecvDataBuff( int netID, void*pWork, int size );

//WiFi受付
#if 0
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

//ルーレット
BOOL		CommRouletteSendBufBasicData( ROULETTE_SCRWORK* wk );
void CommRouletteRecvBufBasicData(int id_no,int size,void *pData,void *work);
BOOL		CommRouletteSendBufTrData( ROULETTE_SCRWORK* wk );
void CommRouletteRecvBufTrData(int id_no,int size,void *pData,void *work);
BOOL		CommRouletteSendBufEnemyPokeData( ROULETTE_SCRWORK* wk );
void CommRouletteRecvBufEnemyPokeData(int id_no,int size,void *pData,void *work);
BOOL		CommRouletteSendBufRetireFlag( ROULETTE_SCRWORK* wk, u16 retire_flag );
void CommRouletteRecvBufRetireFlag(int id_no,int size,void *pData,void *work);
BOOL		CommRouletteSendBufTradeYesNoFlag( ROULETTE_SCRWORK* wk, u16 trade_yesno_flag );
void CommRouletteRecvBufTradeYesNoFlag(int id_no,int size,void *pData,void *work);
BOOL		CommRouletteSendBufTemotiPokeData( ROULETTE_SCRWORK* wk );
void CommRouletteRecvBufTemotiPokeData(int id_no,int size,void *pData,void *work);
u8* CommRouletteGetRecvDataBuff( int netID, void*pWork, int size );


//==============================================================================
//
//	ステージ
//
//	構造体をきって、そのサイズ、受信はその型で代入に変更予定。。。
//	
//==============================================================================

//--------------------------------------------------------------
/**
 * @brief   ポケモンナンバー送信命令(テスト用)
 *
 * @param   wk				ワークへのポインタ
 * @param   pp				ポケモンパラメータ
 *
 * @retval  TRUE:送信した。　FALSE:送信失敗
 */
//--------------------------------------------------------------
BOOL CommStageSendMonsNo( STAGE_SCRWORK* wk, const POKEMON_PARAM* pp )
{
	//u8 *buf;
	u16 *buf;
	int size;
	int ret;

	size = 2;
	//buf = sys_AllocMemory( HEAPID_WORLD, size );
	buf = wk->buf;

	OS_Printf( "CommGetCurrentID() = %d\n", CommGetCurrentID() );

	if( CommGetCurrentID() == 0 ){
		buf[0] = 100;
	}else{
		buf[0] = 200;
	}

	if( CommSendData(FC_STAGE_MONSNO,buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	//sys_FreeMemoryEz( buf );
	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   ポケモンナンバー通信受信処理(テスト用)
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		FRONTIER_SYSTEMへのポインタ
 */
//--------------------------------------------------------------
void CommStageRecvMonsNo(int id_no,int size,void *pData,void *work)
{
	STAGE_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	OS_Printf( "id_no = %d\n", id_no );
	OS_Printf( "size = %d\n", size );
	//OS_Printf( "pData[0] = %d\n", ((u8*)pData)[0] );
	OS_Printf( "recv_buf[0] = %d\n", recv_buf[0] );
	//MI_CpuCopy8(pData, consys->c_game.pp[breeder_no], pp_size);
	return;
}

//--------------------------------------------------------------
/**
 * @brief   基本情報送信命令
 *
 * @param   wk				ワークへのポインタ
 * @param   pp				ポケモンパラメータ
 *
 * @retval  TRUE:送信した。　FALSE:送信失敗
 */
//--------------------------------------------------------------
BOOL CommStageSendBasicData( STAGE_SCRWORK* wk )
{
	int i,num;
	MYSTATUS* my;
	//u8 *buf;
	u16 *buf;
	int ret,size;

	OS_Printf( "******ステージ****** 基本情報送信\n" );
	//OS_Printf( "CommGetCurrentID() = %d\n", CommGetCurrentID() );

	size = STAGE_COMM_BUF_LEN;							//実際はu16データなのでこの半分
	buf = wk->buf;
	num = 0;
	my	= SaveData_GetMyStatus( wk->sv );

	//buf[0] = ;
	num += 1;												//1

	//for( i=0; i < (PERSON_NAME_SIZE + EOM_SIZE) ;i++ ){
	num += (PERSON_NAME_SIZE + EOM_SIZE);					//9

	if( CommSendData(FC_STAGE_BASIC_DATA,buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	return ret;
}

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
void CommStageRecvBasicData(int id_no,int size,void *pData,void *work)
{
	int i,num;
	STAGE_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******ステージ****** 基本情報受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	num = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	OS_Printf( "size = %d\n", size );
	//OS_Printf( "pData[0] = %d\n", ((u8*)pData)[0] );
	OS_Printf( "recv_buf[0] = %d\n", recv_buf[0] );

	//wk-> = ;
	num += 1;														//1

	//名前
	//for( i=0; i < (PERSON_NAME_SIZE + EOM_SIZE) ;i++ ){
	num += (PERSON_NAME_SIZE + EOM_SIZE);							//9

	return;
}

//--------------------------------------------------------------
/**
 * @brief   トレーナー情報送信命令
 *
 * @param   wk				ワークへのポインタ
 * @param   pp				ポケモンパラメータ
 *
 * @retval  TRUE:送信した。　FALSE:送信失敗
 */
//--------------------------------------------------------------
BOOL CommStageSendTrData( STAGE_SCRWORK* wk )
{
	POKEPARTY* party;
	POKEMON_PARAM* poke;
	int i,num;
	u16 *buf;
	int ret,size;

	OS_Printf( "******ステージ****** トレーナー情報送信\n" );
	//OS_Printf( "CommGetCurrentID() = %d\n", CommGetCurrentID() );
	
	size = STAGE_COMM_BUF_LEN;								//実際はu16データなのでこの半分
	buf = wk->buf;
	num = 0;

	//敵トレーナーデータ
	for( i=0; i < STAGE_LAP_MULTI_ENEMY_MAX ;i++ ){
		buf[i+num] = wk->tr_index[i];
		OS_Printf( "送信：tr_index[%d] = %d\n", i, buf[i+num] );
	}
	num += (STAGE_LAP_MULTI_ENEMY_MAX);						//10*2

	if( CommSendData(FC_STAGE_TR_DATA,buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	OS_Printf( "ret = %d\n", ret );
	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   トレーナー情報 通信受信処理
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		FRONTIER_SYSTEMへのポインタ
 */
//--------------------------------------------------------------
void CommStageRecvTrData(int id_no,int size,void *pData,void *work)
{
	int i,num;
	STAGE_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******ステージ****** トレーナー情報受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	num = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	//親は送信するだけなので受け取らない
	if( CommGetCurrentID() == COMM_PARENT_ID ){
		return;
	}

	OS_Printf( "size = %d\n", size );
	//OS_Printf( "pData[0] = %d\n", ((u8*)pData)[0] );
	OS_Printf( "recv_buf[0] = %d\n", recv_buf[0] );

	//敵トレーナーデータ
	for( i=0; i < STAGE_LAP_MULTI_ENEMY_MAX ;i++ ){
		//wk->tr_index[i] = ((u8*)pData)[i+num];
		wk->tr_index[i] = recv_buf[i+num];
		OS_Printf( "受信：wk->tr_index[%d] = %d\n", i, wk->tr_index[i] );
	}
	num += (STAGE_LAP_MULTI_ENEMY_MAX);								//10*2

	return;
}

//--------------------------------------------------------------
/**
 * @brief   敵ポケモンデータ送信命令
 *
 * @param   wk				ワークへのポインタ
 * @param   pp				ポケモンパラメータ
 *
 * @retval  TRUE:送信した。　FALSE:送信失敗
 */
//--------------------------------------------------------------
BOOL CommStageSendEnemyPokeData( STAGE_SCRWORK* wk )
{
	int i,num;
	u16 *buf;
	int ret,size;
	
	OS_Printf( "******ステージ****** 敵ポケモンデータ送信\n" );
	//OS_Printf( "CommGetCurrentID() = %d\n", CommGetCurrentID() );
	
	size = STAGE_COMM_BUF_LEN;								//実際はu16データなのでこの半分
	buf = wk->buf;
	num = 0;

	//ポケモンindexテーブル
	for( i=0; i < STAGE_LAP_MULTI_ENEMY_MAX ;i++ ){
		buf[i] = wk->enemy_poke_index[i];
		OS_Printf( "送信：enemy_index[%d] = %d\n", i, buf[i] );
	}
	num += STAGE_LAP_MULTI_ENEMY_MAX;									//20

	if( CommSendData(FC_STAGE_ENEMY_POKE_DATA,buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   敵ポケモンデータ 通信受信処理
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		FRONTIER_SYSTEMへのポインタ
 */
//--------------------------------------------------------------
void CommStageRecvEnemyPokeData(int id_no,int size,void *pData,void *work)
{
	int i,num;
	STAGE_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******ステージ****** 敵ポケモンデータ受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	num = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	//親は送信するだけなので受け取らない
	if( CommGetCurrentID() == COMM_PARENT_ID ){
		return;
	}

	OS_Printf( "size = %d\n", size );
	OS_Printf( "recv_buf[0] = %d\n", recv_buf[0] );

	//ポケモンindexテーブル
	for( i=0; i < STAGE_LAP_MULTI_ENEMY_MAX ;i++ ){
		wk->enemy_poke_index[i] = recv_buf[i];
		OS_Printf( "受信：wk->enemy_index[%d] = %d\n", i, wk->enemy_poke_index[i] );
	}
	num += STAGE_LAP_MULTI_ENEMY_MAX;									//20

	return;
}

//--------------------------------------------------------------
/**
 * @brief   戦闘後のメニューで何を選んだか送信命令
 *
 * @param   wk				ワークへのポインタ
 * @param	sel				0=つづける、0以外=リタイヤ(きろくするはいらない)
 *
 * @retval  TRUE:送信した。　FALSE:送信失敗
 *
 * お互いが「つづける」の時は継続、
 * 両方、どちらかが「リタイヤ」の時は中止
 */
//--------------------------------------------------------------
BOOL CommStageSendRetireFlag( STAGE_SCRWORK* wk, u8 retire_flag )
{
	//u8 *buf;
	u16 *buf;
	int ret,size;
	
	OS_Printf( "******ステージ****** 戦闘後のメニューで何を選んだか送信\n" );
	//OS_Printf( "CommGetCurrentID() = %d\n", CommGetCurrentID() );
	
	size = STAGE_COMM_BUF_LEN;								//実際はu16データなのでこの半分
	buf = wk->buf;

	buf[0] = retire_flag;
	OS_Printf( "送信：retire_flag = %d\n", buf[0] );

	if( CommSendData(FC_STAGE_RETIRE_FLAG,buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   戦闘後のメニューで何を選んだか 通信受信処理
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		FRONTIER_SYSTEMへのポインタ
 *
 * お互いが「つづける」の時は継続、
 * 両方、どちらかが「リタイヤ」の時は中止
 */
//--------------------------------------------------------------
void CommStageRecvRetireFlag(int id_no,int size,void *pData,void *work)
{
	STAGE_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******ステージ****** 戦闘後のメニューで何を選んだか受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	OS_Printf( "size = %d\n", size );
	//OS_Printf( "pData[0] = %d\n", ((u8*)pData)[0] );
	OS_Printf( "recv_buf[0] = %d\n", recv_buf[0] );

	//wk->pair_retire_flag = ((u8*)pData)[0];
	wk->pair_retire_flag = (u8)recv_buf[0];
	OS_Printf( "受信：wk->pair_retire_flag = %d\n", wk->pair_retire_flag );

	return;
}

//--------------------------------------------------------------
/**
 * @brief	send_bufに手持ちポケモンデータをセット
 *
 * @param	wk		STAGE_SCRWORK型のポインタ
 *
 * @return	none
 *
 * 親機、子機の両方とも選んだ手持ちを渡す必要がある
 */
//--------------------------------------------------------------
BOOL CommStageSendBufTemotiPokeData( STAGE_SCRWORK* wk )
{
	int i,num,ret,size,pp_size;
	POKEMON_PARAM* temp_poke;
	POKEPARTY* party;
	
	num = 0;
	size = STAGE_HUGE_BUF_LEN;

	//POKEMON_PARAMのサイズを取得
	pp_size = PokemonParam_GetWorkSize();
	OS_Printf( "pokemon_param_size = %d\n", pp_size );

	party = SaveData_GetTemotiPokemon( wk->sv );					//手持ちパーティ取得
	temp_poke = PokeParty_GetMemberPointer( party, wk->mine_poke_pos[0] );

	MI_CpuCopy8( temp_poke, &wk->huge_buf[0], pp_size );

	OS_Printf( "送信：pokemon_param\n" );

	if( CommSendHugeData(FC_STAGE_TEMOTI_POKE_DATA,wk->huge_buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	OS_Printf( "送信結果：CommSendHugeData = %d\n", ret );
	return ret;
}

//--------------------------------------------------------------
/**
 * @brief	recv_bufの手持ちポケモンデータを取得
 *
 * @param	wk			STAGE_SCRWORK型のポインタ
 * @param	recv_buf	
 *
 * @return	none
 *
 * 親機、子機の両方とも選んだ手持ちを渡す必要がある
 *
 * pair_rental_...を使いまわしている
 */
//--------------------------------------------------------------
void CommStageRecvBufTemotiPokeData(int id_no,int size,void *pData,void *work)
{
	int i,num,pp_size;
	STAGE_SCRWORK* wk = work;
	const u8* recv_buf = pData;

	OS_Printf( "******ステージ****** 手持ちポケモン情報受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	num = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	OS_Printf( "size = %d\n", size );
	//OS_Printf( "pData[0] = %d\n", ((u8*)pData)[0] );
	OS_Printf( "recv_buf[0] = %d\n", recv_buf[0] );

	pp_size = PokemonParam_GetWorkSize();
	MI_CpuCopy8( &recv_buf[0], wk->pair_poke, pp_size );
	return;
}

//--------------------------------------------------------------
/**
 * @brief   受信領域取得関数(巨大データ：CommSendHugeDataの受信用)
 *
 * @param   netID		
 * @param   pWork		
 * @param   size		
 *
 * @retval  
 */
//--------------------------------------------------------------
u8* CommStageGetRecvDataBuff( int netID, void*pWork, int size )
{
	STAGE_SCRWORK* wk = pWork;
	OS_Printf( "size = %d\n", size );
	GF_ASSERT(size <= STAGE_HUGE_BUF_LEN);
    return wk->recieve_huge_buf[netID];
}


//==============================================================================
//
//	ファクトリー
//
//==============================================================================

//--------------------------------------------------------------
/**
 * @brief   基本情報送信命令
 *
 * @param   wk				ワークへのポインタ
 * @param   pp				ポケモンパラメータ
 *
 * @retval  TRUE:送信した。　FALSE:送信失敗
 */
//--------------------------------------------------------------
BOOL CommFactorySendBasicData( FACTORY_SCRWORK* wk )
{
	int i,num;
	MYSTATUS* my;
	u16 *buf;
	int ret,size;

	OS_Printf( "******ファクトリー****** 基本情報送信\n" );
	//OS_Printf( "CommGetCurrentID() = %d\n", CommGetCurrentID() );
	
	size = FACTORY_COMM_BUF_LEN;						//実際はu16データなのでこの半分
	buf = wk->buf;
	num = 0;
	my	= SaveData_GetMyStatus( wk->sv );

	//buf[0] = 

	buf[1] = wk->trade_count;							//交換回数
	buf[2] = wk->rensyou;								//現在の連勝数
	buf[3] = wk->lap;									//周回数
	num += 4;											//4

	OS_Printf( "____send trade_count = %d\n", buf[1] );
	OS_Printf( "____send rensyou = %d\n", buf[2] );
	OS_Printf( "____send lap = %d\n", buf[3] );

	//名前
	//for( i=0; i < (PERSON_NAME_SIZE + EOM_SIZE) ;i++ ){
	num += (PERSON_NAME_SIZE + EOM_SIZE);					//12

	if( CommSendData(FC_FACTORY_BASIC_DATA,buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   基本情報 通信受信処理
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		FRONTIER_SYSTEMへのポインタ
 */
//--------------------------------------------------------------
void CommFactoryRecvBasicData(int id_no,int size,void *pData,void *work)
{
	int i,num;
	FACTORY_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******ファクトリー****** 基本情報受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	num = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	OS_Printf( "size = %d\n", size );
	//OS_Printf( "pData[0] = %d\n", ((u8*)pData)[0] );
	OS_Printf( "recv_buf[0] = %d\n", recv_buf[0] );

	//wk->	= ;
	//
	wk->pair_trade_count	= recv_buf[1];							//交換回数
	OS_Printf( "受信：wk->pair_trade_count = %d\n", wk->pair_trade_count );
	wk->pair_rensyou		= recv_buf[2];							//現在の連勝数
	OS_Printf( "受信：wk->pair_rensyou = %d\n", wk->pair_rensyou );
	wk->pair_lap			= recv_buf[3];							//周回数
	OS_Printf( "受信：wk->pair_lap = %d\n", wk->pair_lap );
	num += 4;														//4

	OS_Printf( "____recv trade_count = %d\n", recv_buf[1] );
	OS_Printf( "____recv rensyou = %d\n", recv_buf[2] );
	OS_Printf( "____recv lap = %d\n", recv_buf[3] );

	//名前
	//for( i=0; i < (PERSON_NAME_SIZE + EOM_SIZE) ;i++ ){
	num += (PERSON_NAME_SIZE + EOM_SIZE);							//12

	return;
}

//--------------------------------------------------------------
/**
 * @brief   トレーナー情報送信命令
 *
 * @param   wk				ワークへのポインタ
 * @param   pp				ポケモンパラメータ
 *
 * @retval  TRUE:送信した。　FALSE:送信失敗
 */
//--------------------------------------------------------------
BOOL CommFactorySendTrData( FACTORY_SCRWORK* wk )
{
	int i,num;
	u16 *buf;
	int ret,size;

	OS_Printf( "******ファクトリー****** トレーナー情報送信\n" );
	//OS_Printf( "CommGetCurrentID() = %d\n", CommGetCurrentID() );
	
	size = FACTORY_COMM_BUF_LEN;						//実際はu16データなのでこの半分
	buf = wk->buf;
	num = 0;

	//敵トレーナーデータ
	for( i=0; i < FACTORY_LAP_ENEMY_MAX*2 ;i++ ){
		buf[i+num] = wk->tr_index[i];
		OS_Printf( "送信：tr_index[%d] = %d\n", i, buf[i+num] );
	}
	num += (FACTORY_LAP_ENEMY_MAX*2);						//14

	if( CommSendData(FC_FACTORY_TR_DATA,buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	OS_Printf( "ret = %d\n", ret );
	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   トレーナー情報 通信受信処理
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		FRONTIER_SYSTEMへのポインタ
 */
//--------------------------------------------------------------
void CommFactoryRecvTrData(int id_no,int size,void *pData,void *work)
{
	int i,num;
	FACTORY_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******ファクトリー****** トレーナー情報受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	num = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	//親は送信するだけなので受け取らない
	if( CommGetCurrentID() == COMM_PARENT_ID ){
		return;
	}

	OS_Printf( "size = %d\n", size );
	//OS_Printf( "pData[0] = %d\n", ((u8*)pData)[0] );
	OS_Printf( "recv_buf[0] = %d\n", recv_buf[0] );

	//敵トレーナーデータ
	for( i=0; i < FACTORY_LAP_ENEMY_MAX*2 ;i++ ){
		wk->tr_index[i] = recv_buf[i+num];
		OS_Printf( "受信：wk->tr_index[%d] = %d\n", i, wk->tr_index[i] );
	}
	num += (FACTORY_LAP_ENEMY_MAX*2);								//14

	return;
}

//--------------------------------------------------------------
/**
 * @brief   レンタル準備データ 情報送信命令
 *
 * @param   wk				ワークへのポインタ
 * @param   pp				ポケモンパラメータ
 *
 * @retval  TRUE:送信した。　FALSE:送信失敗
 */
//--------------------------------------------------------------
BOOL CommFactorySendRentalData( FACTORY_SCRWORK* wk )
{
	int i,num;
	u16 *buf;
	int ret,size;

	OS_Printf( "******ファクトリー****** レンタル準備データ送信\n" );
	OS_Printf( "CommGetCurrentID() = %d\n", CommGetCurrentID() );
	
	size = FACTORY_COMM_BUF_LEN;						//実際はu16データなのでこの半分
	buf = wk->buf;
	num = 0;

	//ポケモンindexテーブル
	for( i=0; i < FACTORY_RENTAL_POKE_MAX ;i++ ){
		buf[i] = wk->pair_rental_poke_index[i];
		OS_Printf( "送信：rental_index[%d] = %d\n", i, buf[i] );
	}
	num += FACTORY_RENTAL_POKE_MAX;										//6

	//ポケモンのパワー乱数
	for( i=0; i < FACTORY_RENTAL_POKE_MAX ;i++ ){
		buf[i+num] = wk->pair_rental_pow_rnd[i];
		OS_Printf( "送信：rental_pow_rnd[%d] = %d\n", i, buf[i+num] );
	}
	num += FACTORY_RENTAL_POKE_MAX;										//12
	
	//ポケモンの個性乱数(u32なので、u16ずつセットしているので注意！)
	for( i=0; i < FACTORY_RENTAL_POKE_MAX ;i++ ){
		//u32 rental_personal_rnd[FACTORY_RENTAL_POKE_MAX];
		buf[i+num] = (wk->pair_rental_personal_rnd[i] & 0xffff);
		buf[i+num+FACTORY_RENTAL_POKE_MAX] = 
								((wk->pair_rental_personal_rnd[i] >> 16) & 0xffff);
		OS_Printf( "元データ rental_personal_rnd[%d] = %d\n", i, wk->pair_rental_personal_rnd[i] );
		OS_Printf( "送信：rental_personal_rnd[%d] = %d\n", i, buf[i+num] );
		OS_Printf( "送信：rental_personal_rnd[%d] = %d\n", i+FACTORY_RENTAL_POKE_MAX, 
													buf[i+num+FACTORY_RENTAL_POKE_MAX] );
	}
	num += (FACTORY_RENTAL_POKE_MAX * 2);								//24

#if 0
	for( i=0; i < FACTORY_COMM_BUF_LEN ;i++ ){
		OS_Printf( "buf[%d] = %d\n", i, buf[i] );
	}
#endif

	if( CommSendData(FC_FACTORY_RENTAL_DATA,buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	OS_Printf( "ret = %d\n", ret );
	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   レンタル準備データ 通信受信処理
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		FRONTIER_SYSTEMへのポインタ
 */
//--------------------------------------------------------------
void CommFactoryRecvRentalData(int id_no,int size,void *pData,void *work)
{
	int i,num;
	POKEMON_PARAM* temp_poke;
	FACTORY_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******ファクトリー****** レンタル準備データ受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	num = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		OS_Printf( "id_no = %d 自分のデータは受けとらない\n", id_no );
		return;
	}

	//親は送信するだけなので受け取らない
	if( CommGetCurrentID() == COMM_PARENT_ID ){
		OS_Printf( "id_no = %d 親は送信するだけで受け取らない\n", id_no );
		return;
	}

	OS_Printf( "size = %d\n", size );
	//OS_Printf( "pData[0] = %d\n", ((u8*)pData)[0] );
	OS_Printf( "recv_buf[0] = %d\n", recv_buf[0] );

	//ポケモンindexテーブル
	for( i=0; i < FACTORY_RENTAL_POKE_MAX ;i++ ){
		//wk->pair_rental_poke_index[i] = recv_buf[i];
		//OS_Printf( "受信：wk->pair_rental_index[%d] = %d\n", i, wk->pair_rental_poke_index[i] );
		wk->rental_poke_index[i] = recv_buf[i];
		OS_Printf( "受信：wk->rental_index[%d] = %d\n", i, wk->rental_poke_index[i] );
	}
	num += FACTORY_RENTAL_POKE_MAX;										//6

	//ポケモンのパワー乱数
	for( i=0; i < FACTORY_RENTAL_POKE_MAX ;i++ ){
		//wk->pair_rental_pow_rnd[i] = recv_buf[i+num];
		//OS_Printf( "受信：wk->pair_rental_pow_rnd[%d] = %d\n", i, wk->pair_rental_pow_rnd[i] );
		wk->rental_pow_rnd[i] = recv_buf[i+num];
		OS_Printf( "受信：wk->rental_pow_rnd[%d] = %d\n", i, wk->rental_pow_rnd[i] );
	}
	num += FACTORY_RENTAL_POKE_MAX;										//12
	
	//ポケモンの個性乱数(u32なので、u16ずつセットしているので注意！)
	for( i=0; i < FACTORY_RENTAL_POKE_MAX ;i++ ){
		//u32 rental_personal_rnd[FACTORY_RENTAL_POKE_MAX];
		//wk->pair_rental_personal_rnd[i] = recv_buf[i+num];
		//wk->pair_rental_personal_rnd[i] |= (recv_buf[i+num+FACTORY_RENTAL_POKE_MAX] << 16);
		//OS_Printf( "受信：wk->pair_rental_personal_rnd[%d] = %d\n", i, 
		//										wk->pair_rental_personal_rnd[i] );
		wk->rental_personal_rnd[i] = recv_buf[i+num];
		wk->rental_personal_rnd[i] |= (recv_buf[i+num+FACTORY_RENTAL_POKE_MAX] << 16);

		OS_Printf( "受信：wk->rental_personal_rnd[%d] = %d\n", i, 
												wk->rental_personal_rnd[i] );
	}
	num += (FACTORY_RENTAL_POKE_MAX * 2);								//24

#if 0
	for( i=0; i < FACTORY_COMM_BUF_LEN ;i++ ){
		OS_Printf( "recv_buf[%d] = %d\n", i, recv_buf[i] );
	}
#endif

#if 0
	Factory_RentalPokeMake2( wk );
	//------------------------------------------------------------------
#endif

	return;
}

//--------------------------------------------------------------
/**
 * @brief   敵ポケモンデータ　送信命令
 *
 * @param   wk				ワークへのポインタ
 * @param   pp				ポケモンパラメータ
 *
 * @retval  TRUE:送信した。　FALSE:送信失敗
 */
//--------------------------------------------------------------
BOOL CommFactorySendEnemyPokeData( FACTORY_SCRWORK* wk )
{
	int i,num;
	u16 *buf;
	int ret,size;

	OS_Printf( "******ファクトリー****** 敵ポケモンデータ送信\n" );
	//OS_Printf( "CommGetCurrentID() = %d\n", CommGetCurrentID() );
	
	size = FACTORY_COMM_BUF_LEN;						//実際はu16データなのでこの半分
	buf = wk->buf;
	num = 0;

	//ポケモンindexテーブル
	for( i=0; i < FACTORY_ENEMY_POKE_MAX ;i++ ){
		buf[i] = wk->enemy_poke_index[i];
		OS_Printf( "送信：enemy_index[%d] = %d\n", i, buf[i] );
	}
	num += FACTORY_ENEMY_POKE_MAX;										//4

	//ポケモンのパワー乱数
	for( i=0; i < FACTORY_ENEMY_POKE_MAX ;i++ ){
		buf[i+num] = wk->enemy_pow_rnd[i];
		OS_Printf( "送信：enemy_pow_rnd[%d] = %d\n", i, buf[i+num] );
	}
	num += FACTORY_ENEMY_POKE_MAX;										//8
	
	//ポケモンの個性乱数(u32なので、u16ずつセットしているので注意！)
	for( i=0; i < FACTORY_ENEMY_POKE_MAX ;i++ ){
		//u32 enemy_personal_rnd[FACTORY_ENEMY_POKE_MAX];
		buf[i+num] = (wk->enemy_personal_rnd[i] & 0xffff);
		buf[i+num+FACTORY_ENEMY_POKE_MAX] = 
								((wk->enemy_personal_rnd[i] >> 16) & 0xffff);
		OS_Printf( "元データ enemy_personal_rnd[%d] = %d\n", i, wk->enemy_personal_rnd[i] );
		OS_Printf( "送信：enemy_personal_rnd[%d] = %d\n", i, buf[i+num] );
		OS_Printf( "送信：enemy_personal_rnd[%d] = %d\n", i+FACTORY_ENEMY_POKE_MAX, 
													buf[i+num+FACTORY_ENEMY_POKE_MAX] );
	}
	num += (FACTORY_ENEMY_POKE_MAX * 2);								//16

	if( CommSendData(FC_FACTORY_ENEMY_POKE_DATA,buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	OS_Printf( "ret = %d\n", ret );
	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   敵ポケモンデータ 通信受信処理
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		FRONTIER_SYSTEMへのポインタ
 */
//--------------------------------------------------------------
void CommFactoryRecvEnemyPokeData(int id_no,int size,void *pData,void *work)
{
	int i,num;
	FACTORY_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******ファクトリー****** 敵ポケモンデータ受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	num = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	//親は送信するだけなので受け取らない
	if( CommGetCurrentID() == COMM_PARENT_ID ){
		return;
	}

	OS_Printf( "size = %d\n", size );
	//OS_Printf( "pData[0] = %d\n", ((u8*)pData)[0] );
	OS_Printf( "recv_buf[0] = %d\n", recv_buf[0] );

	//ポケモンindexテーブル
	for( i=0; i < FACTORY_ENEMY_POKE_MAX ;i++ ){
		wk->enemy_poke_index[i] = recv_buf[i];
		OS_Printf( "受信：wk->enemy_index[%d] = %d\n", i, wk->enemy_poke_index[i] );
	}
	num += FACTORY_ENEMY_POKE_MAX;										//4

	//ポケモンのパワー乱数
	for( i=0; i < FACTORY_ENEMY_POKE_MAX ;i++ ){
		wk->enemy_pow_rnd[i] = recv_buf[i+num];
		OS_Printf( "受信：wk->enemy_pow_rnd[%d] = %d\n", i, wk->enemy_pow_rnd[i] );
	}
	num += FACTORY_ENEMY_POKE_MAX;										//8
	
	//ポケモンの個性乱数(u32なので、u16ずつセットしているので注意！)
	for( i=0; i < FACTORY_ENEMY_POKE_MAX ;i++ ){
		//u32 enemy_personal_rnd[FACTORY_ENEMY_POKE_MAX];
		wk->enemy_personal_rnd[i] = recv_buf[i+num];
		wk->enemy_personal_rnd[i] |= (recv_buf[i+num+FACTORY_ENEMY_POKE_MAX] << 16);
		OS_Printf( "受信：wk->enemy_personal_rnd[%d] = %d\n", i, wk->enemy_personal_rnd[i] );
	}
	num += (FACTORY_ENEMY_POKE_MAX * 2);								//16

#if 0
	//B_TOWER_POKEMON enemy_poke[FACTORY_ENEMY_POKE_MAX];

	//敵ポケモンを一度に生成
	Frontier_PokemonParamCreateAll(	wk->enemy_poke, wk->enemy_poke_index, 
									wk->enemy_pow_rnd, wk->enemy_personal_rnd, NULL, 
									FACTORY_ENEMY_POKE_MAX, HEAPID_WORLD, ARC_PL_BTD_PM );
#endif

	return;
}

//--------------------------------------------------------------
/**
 * @brief   戦闘後のメニューで何を選んだか送信命令
 *
 * @param   wk				ワークへのポインタ
 * @param	sel				0=つづける、0以外=リタイヤ(きろくするはいらない)
 *
 * @retval  TRUE:送信した。　FALSE:送信失敗
 *
 * お互いが「つづける」の時は継続、
 * 両方、どちらかが「リタイヤ」の時は中止
 */
//--------------------------------------------------------------
BOOL CommFactorySendRetireFlag( FACTORY_SCRWORK* wk, u8 retire_flag )
{
	u16 *buf;
	int ret,size;
	
	OS_Printf( "******ファクトリー****** 戦闘後のメニューで何を選んだかを送信\n" );
	//OS_Printf( "CommGetCurrentID() = %d\n", CommGetCurrentID() );
	
	size = FACTORY_COMM_BUF_LEN;						//実際はu16データなのでこの半分
	buf = wk->buf;

	buf[0] = retire_flag;
	OS_Printf( "送信：retire_flag = %d\n", buf[0] );

	if( CommSendData(FC_FACTORY_RETIRE_FLAG,buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   戦闘後のメニューで何を選んだか 通信受信処理
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		FRONTIER_SYSTEMへのポインタ
 *
 * お互いが「つづける」の時は継続、
 * 両方、どちらかが「リタイヤ」の時は中止
 */
//--------------------------------------------------------------
void CommFactoryRecvRetireFlag(int id_no,int size,void *pData,void *work)
{
	FACTORY_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******ファクトリー****** 戦闘後のメニューで何を選んだか受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	OS_Printf( "size = %d\n", size );
	//OS_Printf( "pData[0] = %d\n", ((u8*)pData)[0] );
	OS_Printf( "recv_buf[0] = %d\n", recv_buf[0] );

	//wk->pair_retire_flag = ((u8*)pData)[0];
	wk->pair_retire_flag = (u8)recv_buf[0];
	OS_Printf( "受信：wk->pair_retire_flag = %d\n", wk->pair_retire_flag );

	return;
}

//--------------------------------------------------------------
/**
 * @brief   交換したいかを送信命令
 *
 * @param   wk				ワークへのポインタ
 * @param	sel				0=つづける、0以外=リタイヤ(きろくするはいらない)
 *
 * @retval  TRUE:送信した。　FALSE:送信失敗
 *
 * お互いが「つづける」の時は継続、
 * 両方、どちらかが「リタイヤ」の時は中止
 */
//--------------------------------------------------------------
BOOL CommFactorySendTradeYesNoFlag( FACTORY_SCRWORK* wk, u8 trade_yesno_flag )
{
	u16 *buf;
	int ret,size;
	
	OS_Printf( "******ファクトリー****** 交換したいかを送信\n" );
	//OS_Printf( "CommGetCurrentID() = %d\n", CommGetCurrentID() );
	
	size = FACTORY_COMM_BUF_LEN;						//実際はu16データなのでこの半分
	buf = wk->buf;

	buf[0] = trade_yesno_flag;
	OS_Printf( "送信：trade_yesno_flag = %d\n", buf[0] );

	if( CommSendData(FC_FACTORY_TRADE_YESNO_FLAG,buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   交換したいかを 通信受信処理
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		FRONTIER_SYSTEMへのポインタ
 *
 * お互いが「つづける」の時は継続、
 * 両方、どちらかが「リタイヤ」の時は中止
 */
//--------------------------------------------------------------
void CommFactoryRecvTradeYesNoFlag(int id_no,int size,void *pData,void *work)
{
	FACTORY_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******ファクトリー****** 交換したいかを受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	OS_Printf( "size = %d\n", size );
	//OS_Printf( "pData[0] = %d\n", ((u8*)pData)[0] );
	OS_Printf( "recv_buf[0] = %d\n", recv_buf[0] );

	wk->pair_trade_yesno_flag = (u8)recv_buf[0];
	OS_Printf( "受信：wk->pair_trade_yesno_flag = %d\n", wk->pair_trade_yesno_flag );
	return;
}

//--------------------------------------------------------------
/**
 * @brief   手持ちポケモンデータ　送信命令
 *
 * @param   wk				ワークへのポインタ
 * @param   pp				ポケモンパラメータ
 *
 * @retval  TRUE:送信した。　FALSE:送信失敗
 */
//--------------------------------------------------------------
BOOL CommFactorySendTemotiPokeData( FACTORY_SCRWORK* wk )
{
	u8 m_max;
	int i,num;
	u16 *buf;
	int ret,size;
	u32 personal_rnd[2];
	u8 pow_rnd[2];
	POKEMON_PARAM* temp_poke;

	OS_Printf( "******ファクトリー****** 手持ちポケモンデータ送信\n" );
	//OS_Printf( "CommGetCurrentID() = %d\n", CommGetCurrentID() );
	
	size = FACTORY_COMM_BUF_LEN;						//実際はu16データなのでこの半分
	buf = wk->buf;
	num = 0;

	//タイプによってポケモンの数を取得
	m_max = Factory_GetMinePokeNum( wk->type );

	//手持ちポケパーティからpow_rnd,personal_rndを取得
	//temp_poke = PokemonParam_AllocWork( HEAPID_WORLD );
	for( i=0; i < m_max ;i++ ){
		temp_poke = PokeParty_GetMemberPointer( wk->p_m_party, i );
		pow_rnd[i] = PokeParaGet( temp_poke, ID_PARA_pow_rnd, NULL );
		personal_rnd[i] = PokeParaGet( temp_poke, ID_PARA_personal_rnd, NULL );
		OS_Printf( "factory monsno[%d] = %d\n", i, PokeParaGet(temp_poke,ID_PARA_monsno,NULL) );
	}
	//sys_FreeMemoryEz( temp_poke );

	//ポケモンindexテーブル
	for( i=0; i < m_max ;i++ ){
		buf[i] = wk->temoti_poke_index[i];
		OS_Printf( "送信：temoti_index[%d] = %d\n", i, buf[i] );
	}
	num += m_max;														//2

	//ポケモンのパワー乱数
	for( i=0; i < m_max ;i++ ){
		buf[i+num] = pow_rnd[i];
		OS_Printf( "送信：temoti_pow_rnd[%d] = %d\n", i, buf[i+num] );
	}
	num += m_max;														//4
	
	//ポケモンの個性乱数(u32なので、u16ずつセットしているので注意！)
	for( i=0; i < m_max ;i++ ){
		buf[i+num] = (personal_rnd[i] & 0xffff);
		buf[i+num+m_max] = ((personal_rnd[i] >> 16) & 0xffff);
		OS_Printf( "送信：temoti_personal_rnd[%d] = %d\n", i, buf[i+num] );
		OS_Printf( "送信：temoti_personal_rnd[%d] = %d\n", i+m_max, buf[i+num+m_max] );
	}
	num += (m_max * 2);													//8

	if( CommSendData(FC_FACTORY_TEMOTI_POKE_DATA,buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	OS_Printf( "ret = %d\n", ret );
	return ret;
}

//--------------------------------------------------------------
/**
 * @brief   手持ちポケモンデータ 通信受信処理
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		FRONTIER_SYSTEMへのポインタ
 */
//--------------------------------------------------------------
void CommFactoryRecvTemotiPokeData(int id_no,int size,void *pData,void *work)
{
	int i,num;
	u8 m_max;
	u32 personal_rnd[2];
	u8 pow_rnd[2];
	POKEMON_PARAM* temp_poke;
	FACTORY_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******ファクトリー****** 手持ちポケモンデータ受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	num = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	OS_Printf( "size = %d\n", size );
	//OS_Printf( "pData[0] = %d\n", ((u8*)pData)[0] );
	OS_Printf( "recv_buf[0] = %d\n", recv_buf[0] );

	//タイプによってポケモンの数を取得
	m_max = Factory_GetMinePokeNum( wk->type );

	//ポケモンindexテーブル
	for( i=0; i < m_max ;i++ ){
		wk->pair_rental_poke_index[i] = recv_buf[i];
		OS_Printf( "受信：wk->rental_index[%d] = %d\n", i, wk->pair_rental_poke_index[i] );
	}
	num += m_max;														//2

	//ポケモンのパワー乱数
	for( i=0; i < m_max ;i++ ){
		wk->pair_rental_pow_rnd[i] = recv_buf[i+num];
		OS_Printf( "受信：wk->rental_pow_rnd[%d] = %d\n", i, wk->pair_rental_pow_rnd[i] );
	}
	num += m_max;														//4
	
	//ポケモンの個性乱数(u32なので、u16ずつセットしているので注意！)
	for( i=0; i < m_max ;i++ ){
		wk->pair_rental_personal_rnd[i] = recv_buf[i+num];
		wk->pair_rental_personal_rnd[i] |= (recv_buf[i+num+m_max] << 16);
		OS_Printf( "受信：wk->rental_personal_rnd[%d] = %d\n", i, wk->pair_rental_personal_rnd[i] );
	}
	num += (m_max * 2);													//8

#if 0
	//----------------------------------------------------------------------------------------
	Factory_PairPokeMake( wk );
#endif

	return;
}


//==============================================================================
//
//	キャッスル
//
//==============================================================================

//--------------------------------------------------------------
/**
 * @brief	send_bufに基本情報をセット
 *
 * @param	wk		CASTLE_SCRWORK型のポインタ
 *
 * @retval  TRUE:送信した。　FALSE:送信失敗
 *
 * キャッスルワークを確保後、最初に送るデータ
 */
//--------------------------------------------------------------
BOOL CommCastleSendBufBasicData( CASTLE_SCRWORK* wk )
{
	int i,num,ret,size;
	CASTLESCORE* score_sv;

	OS_Printf( "******キャッスル****** 基本情報送信\n" );

	score_sv = SaveData_GetCastleScore( wk->sv );
	size = CASTLE_COMM_BUF_LEN;						//実際はu16データなのでこの半分
	num = 0;

	//wk->send_buf[0]		= ;
	wk->send_buf[1] = wk->rensyou;							//現在の連勝数
	wk->send_buf[2] = wk->lap;								//周回数
	num += 3;												//3

	//for( i=0; i < (PERSON_NAME_SIZE + EOM_SIZE) ;i++ ){
	num += (PERSON_NAME_SIZE + EOM_SIZE);					//11

	//CP
	wk->send_buf[num] = FrontierRecord_Get( SaveData_GetFrontier(wk->sv), 
									CastleScr_GetCPRecordID(wk->type),
									Frontier_GetFriendIndex(CastleScr_GetCPRecordID(wk->type)) );

	OS_Printf( "送信：cp = %d\n", wk->send_buf[num] );
	num+=1;													//12

	if( CommSendData(FC_CASTLE_BASIC_DATA,wk->send_buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief	recv_bufの基本情報を取得
 *
 * @param	wk			CASTLE_SCRWORK型のポインタ
 * @param	recv_buf	
 *
 * @return	none
 *
 * キャッスルワークを確保後、最初に受け取るデータ
 */
//--------------------------------------------------------------
void CommCastleRecvBufBasicData(int id_no,int size,void *pData,void *work)
{
	int i,num;
	CASTLE_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******キャッスル****** 基本情報受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	num = 0;
	wk->recieve_count++;
	//OS_Printf( "wk->recieve_count = %d\n", wk->recieve_count );

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	//wk->					= recv_buf[0];
	//
	wk->pair_rensyou		= recv_buf[1];							//現在の連勝数
	OS_Printf( "受信：wk->pair_rensyou = %d\n", wk->pair_rensyou );
	wk->pair_lap			= recv_buf[2];							//周回数
	OS_Printf( "受信：wk->pair_lap = %d\n", wk->pair_lap );
	num += 3;														//3

	//for( i=0; i < (PERSON_NAME_SIZE + EOM_SIZE) ;i++ ){
	num += (PERSON_NAME_SIZE + EOM_SIZE);							//11

	//CP
	wk->pair_cp = recv_buf[num];									//CP
	OS_Printf( "受信：wk->pair_cp = %d\n", wk->pair_cp );
	num+=1;															//12

	return;
}

//--------------------------------------------------------------
/**
 * @brief	send_bufにトレーナー情報をセット
 *
 * @param	wk		CASTLE_SCRWORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
BOOL CommCastleSendBufTrData( CASTLE_SCRWORK* wk )
{
	int i,num,ret,size;
	
	OS_Printf( "******キャッスル****** トレーナー情報送信\n" );

	num = 0;
	size = CASTLE_COMM_BUF_LEN;						//実際はu16データなのでこの半分

	//敵トレーナーデータ
	for( i=0; i < CASTLE_LAP_MULTI_ENEMY_MAX ;i++ ){
		wk->send_buf[i+num] = wk->tr_index[i];
		OS_Printf( "送信：tr_index[%d] = %d\n", i, wk->send_buf[i+num] );
	}
	num += CASTLE_LAP_MULTI_ENEMY_MAX;						//14

	if( CommSendData(FC_CASTLE_TR_DATA,wk->send_buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief	recv_bufのトレーナー情報を取得
 *
 * @param	wk			CASTLE_SCRWORK型のポインタ
 * @param	recv_buf	
 *
 * @return	none
 *
 * キャッスルワークを確保後、最初に受け取るデータ
 */
//--------------------------------------------------------------
void CommCastleRecvBufTrData(int id_no,int size,void *pData,void *work)
{
	int i,num;
	CASTLE_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******キャッスル****** トレーナー情報受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	num = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	//親は送信するだけなので受け取らない
	if( CommGetCurrentID() == COMM_PARENT_ID ){
		return;
	}

	//敵トレーナーデータ
	for( i=0; i < CASTLE_LAP_MULTI_ENEMY_MAX ;i++ ){
		wk->tr_index[i] = recv_buf[i+num];
		OS_Printf( "受信：wk->tr_index[%d] = %d\n", i, wk->tr_index[i] );
	}
	num += CASTLE_LAP_MULTI_ENEMY_MAX;								//14

	return;
}

//--------------------------------------------------------------
/**
 * @brief	send_bufにランク、抜け道などのリクエストをセット
 *
 * @param	wk		CASTLE_SCRWORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
BOOL CommCastleSendBufSelData( CASTLE_SCRWORK* wk )
{
	int i,num,ret,size;
	MYSTATUS* my;
	
	OS_Printf( "******キャッスル****** ランク、抜け道などのリクエスト情報送信\n" );

	OS_Printf( "初期値\n" );
	OS_Printf( "wk->sel_type = %d\n", wk->sel_type );
	OS_Printf( "wk->pair_sel_type = %d\n\n", wk->pair_sel_type );
	OS_Printf( "wk->parent_check_flag = %d\n", wk->parent_check_flag );

	num = 0;
	size = CASTLE_COMM_BUF_LEN;						//実際はu16データなのでこの半分

	wk->send_buf[0] = wk->sel_type;
	OS_Printf( "送信：type = %d\n", wk->send_buf[0] );

	//親
	if( CommGetCurrentID() == COMM_PARENT_ID ){

		//先に子の選択がきていなくて値が入っていない時は、親の決定はセットしてしまう
		if( wk->parent_check_flag == CASTLE_SCR_MENU_NONE ){
			wk->parent_check_flag = wk->sel_type;

			//CP減らす
			//CASTLESCORE_SubCP( SaveData_GetCastleScore(wk->sv), CP_USE_NUKEMITI );
			
		//子の選択がきていてそれでいいよ、と送る前に「たいせん」チェック
		}else{

			//子がたいせんを選んでいても、親がたいせんを選んでいなかったら、親採用
			if( (wk->parent_check_flag-CASTLE_SCR_MENU_MAX) == CASTLE_SCR_MENU_TAISEN ){
				if( wk->sel_type != CASTLE_SCR_MENU_TAISEN ){
					wk->parent_check_flag = wk->sel_type;
				}
			}
		}

	//子
	}else{

		//すでに親の選択がきていて、たいせんの時
		if( wk->parent_check_flag == CASTLE_SCR_MENU_TAISEN ){

			//子の選択がたいせんでない時、子が採用
			if( wk->sel_type != CASTLE_SCR_MENU_TAISEN ){
				wk->parent_check_flag = wk->sel_type;
			}
		}
	}

	wk->send_buf[1] = wk->parent_check_flag;
	OS_Printf( "送信：parent_check_flag = %d\n", wk->send_buf[1] );
	OS_Printf( "wk->sel_type = %d\n", wk->sel_type );
	OS_Printf( "wk->pair_sel_type = %d\n\n", wk->pair_sel_type );
	OS_Printf( "wk->parent_check_flag = %d\n", wk->parent_check_flag );

	if( CommSendData(FC_CASTLE_SEL_DATA,wk->send_buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief	recv_bufのランク、抜け道などのリクエストを取得
 *
 * @param	wk			CASTLE_SCRWORK型のポインタ
 * @param	recv_buf	
 *
 * @return	none
 */
//--------------------------------------------------------------
void CommCastleRecvBufSelData(int id_no,int size,void *pData,void *work)
{
	POKEMON_PARAM* temp_poke;
	int i,num;
	CASTLE_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******キャッスル****** ランク、抜け道などのリクエスト情報受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	OS_Printf( "初期値\n" );
	OS_Printf( "wk->sel_type = %d\n", wk->sel_type );
	OS_Printf( "wk->pair_sel_type = %d\n\n", wk->pair_sel_type );
	OS_Printf( "wk->parent_check_flag = %d\n", wk->parent_check_flag );

	num = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	wk->pair_sel_type = recv_buf[0];
	OS_Printf( "受信：wk->pair_sel_type = %d\n", wk->pair_sel_type );

	//wk->parent_check_flag = recv_buf[1];
	//OS_Printf( "受信：wk->parent_check_flag = %d\n", wk->parent_check_flag );

	//親
	if( CommGetCurrentID() == COMM_PARENT_ID ){

		//親の決定がすでに決まっていたら、子の選択は無効
		if( wk->parent_check_flag != CASTLE_SCR_MENU_NONE ){

			//親が「たいせん」を選んで、子が「たいせん」ではない時は、子を採用
			if( wk->parent_check_flag == CASTLE_SCR_MENU_TAISEN ){
				if( wk->pair_sel_type != CASTLE_SCR_MENU_TAISEN ){

					//子の選択が採用されたことがわかるようにオフセットを加える
					wk->sel_type			= wk->pair_sel_type + CASTLE_SCR_MENU_MAX;
					wk->parent_check_flag	= wk->pair_sel_type + CASTLE_SCR_MENU_MAX;
				}
			}
		}else{

			//親の決定が決まっていない時は、子にそれでいいよと送信する
			//BOOL CommCastleSendBufSelData( CASTLE_SCRWORK* wk )

			//これは、親がリストを選んでいる時に、それを強制解除して、
			//先に進ませる処理が必要ということ。
			//リストのポインタから、強制解除関数を呼び出す感じ

#if 0
			wk->sel_type = wk->pair_sel_type;
			wk->parent_check_flag = wk->pair_sel_type;
#else
			//子の選択が採用されたことがわかるようにオフセットを加える
			//wk->sel_type = wk->pair_sel_type + CASTLE_SCR_MENU_MAX;
			//
			//受信、送信の流れだと「たいせん」を選んでいないのに「たいせんに」になってしまう対処

			wk->parent_check_flag = wk->pair_sel_type + CASTLE_SCR_MENU_MAX;
#endif

			//「たいせん」以外を選んでいたら、リストを削除
			if( wk->pair_sel_type != CASTLE_SCR_MENU_TAISEN ){
				*wk->list_del_work = FSEV_WIN_COMPULSION_DEL;
			}
		}

	//子
	}else{

		wk->parent_check_flag = recv_buf[1];
		OS_Printf( "受信：wk->parent_check_flag = %d\n", wk->parent_check_flag );
		
		//子
		//wk->parent_check_flag = wk->pair_sel_type;
		
		//「たいせん」以外を選んでいたら、リストを削除
		if( wk->parent_check_flag != CASTLE_SCR_MENU_TAISEN ){
			*wk->list_del_work = FSEV_WIN_COMPULSION_DEL;
		}

#if 1
		//08.07.05
		//親が「たいせん」を選んでいて、recv_buf[1]に「たいせん」が入っている
		//ペアが「たいせん」以外を選んだ時は、メニューを強制終了させる
		//
		//親が「たいせん」を選んでいて、子がワンテンポ遅れて「たいせん」以外を選んでいると、
		//parent_check_flagは「たいせん」になってしまっているので、
		//子の自分の選択が「たいせん」以外なら、それで上書きする

		//親が「たいせん」を選んでいて
		//★「じぶん」「あいて」の時は先勝ちになるので「たいせん」かのチェック必要！
		if( wk->pair_sel_type == CASTLE_SCR_MENU_TAISEN ){

			OS_Printf( "\n**********\nwk->sel_type = %d\n", wk->sel_type );

			//子がすでに選択している
			if( wk->sel_type != CASTLE_SCR_MENU_NONE ){

				//子が「たいせん」以外を選んでいたら
				if( wk->sel_type != CASTLE_SCR_MENU_TAISEN ){
				//if( (wk->sel_type - CASTLE_SCR_MENU_MAX) != CASTLE_SCR_MENU_TAISEN ){

					//子の選択が採用されたことがわかるようにオフセットを加える(注意)
					//wk->sel_type			= wk->sel_type + CASTLE_SCR_MENU_MAX;
					wk->parent_check_flag	= wk->sel_type + CASTLE_SCR_MENU_MAX;
				
					//wk->parent_check_flag	= wk->sel_type;
					OS_Printf( "書換(子)：wk->sel_type = %d\n", wk->sel_type );
					OS_Printf( "書換(子)：wk->parent_check_flag = %d\n", wk->parent_check_flag );
				}
			}
		}
#endif

	}

	OS_Printf( "受信：wk->pair_sel_type = %d\n", wk->pair_sel_type );
	OS_Printf( "wk->sel_type = %d\n", wk->sel_type );
	OS_Printf( "wk->pair_sel_type = %d\n\n", wk->pair_sel_type );
	OS_Printf( "wk->parent_check_flag = %d\n", wk->parent_check_flag );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	send_bufに敵ポケモンデータをセット
 *
 * @param	wk		CASTLE_SCRWORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
BOOL CommCastleSendBufEnemyPokeData( CASTLE_SCRWORK* wk )
{
	int i,num,ret,size;
	
	OS_Printf( "******キャッスル****** 敵ポケモン送信\n" );

	num = 0;
	size = CASTLE_COMM_BUF_LEN;						//実際はu16データなのでこの半分

	//ポケモンindexテーブル
	for( i=0; i < CASTLE_ENEMY_POKE_MAX ;i++ ){
		wk->send_buf[i] = wk->enemy_poke_index[i];
		OS_Printf( "送信：enemy_index[%d] = %d\n", i, wk->send_buf[i] );
	}
	num += CASTLE_ENEMY_POKE_MAX;										//4

	//ポケモンのパワー乱数
	for( i=0; i < CASTLE_ENEMY_POKE_MAX ;i++ ){
		wk->send_buf[i+num] = wk->enemy_pow_rnd[i];
		OS_Printf( "送信：enemy_pow_rnd[%d] = %d\n", i, wk->send_buf[i+num] );
	}
	num += CASTLE_ENEMY_POKE_MAX;										//8
	
	//ポケモンの個性乱数(u32なので、u16ずつセットしているので注意！)
	for( i=0; i < CASTLE_ENEMY_POKE_MAX ;i++ ){
		//u32 enemy_personal_rnd[CASTLE_ENEMY_POKE_MAX];
		wk->send_buf[i+num] = (wk->enemy_personal_rnd[i] & 0xffff);
		wk->send_buf[i+num+CASTLE_ENEMY_POKE_MAX] = 
								((wk->enemy_personal_rnd[i] >> 16) & 0xffff);
		OS_Printf( "元データ enemy_personal_rnd[%d] = %d\n", i, wk->enemy_personal_rnd[i] );
		OS_Printf( "送信：enemy_personal_rnd[%d] = %d\n", i, wk->send_buf[i+num] );
		OS_Printf( "送信：enemy_personal_rnd[%d] = %d\n", i+CASTLE_ENEMY_POKE_MAX, 
													wk->send_buf[i+num+CASTLE_ENEMY_POKE_MAX] );
	}
	num += (CASTLE_ENEMY_POKE_MAX * 2);								//16

	if( CommSendData(FC_CASTLE_ENEMY_POKE_DATA,wk->send_buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief	recv_bufの敵ポケモンデータを取得
 *
 * @param	wk			CASTLE_SCRWORK型のポインタ
 * @param	recv_buf	
 *
 * @return	none
 */
//--------------------------------------------------------------
void CommCastleRecvBufEnemyPokeData(int id_no,int size,void *pData,void *work)
{
	int i,num;
	CASTLE_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******キャッスル****** 敵ポケモン情報受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	num = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	//親は送信するだけなので受け取らない
	if( CommGetCurrentID() == COMM_PARENT_ID ){
		return;
	}

	OS_Printf( "size = %d\n", size );
	OS_Printf( "recv_buf[0] = %d\n", recv_buf[0] );

	//ポケモンindexテーブル
	for( i=0; i < CASTLE_ENEMY_POKE_MAX ;i++ ){
		wk->enemy_poke_index[i] = recv_buf[i];
		OS_Printf( "受信：wk->enemy_index[%d] = %d\n", i, wk->enemy_poke_index[i] );
	}
	num += CASTLE_ENEMY_POKE_MAX;										//4

	//ポケモンのパワー乱数
	for( i=0; i < CASTLE_ENEMY_POKE_MAX ;i++ ){
		wk->enemy_pow_rnd[i] = recv_buf[i+num];
		OS_Printf( "受信：wk->enemy_pow_rnd[%d] = %d\n", i, wk->enemy_pow_rnd[i] );
	}
	num += CASTLE_ENEMY_POKE_MAX;										//8
	
	//ポケモンの個性乱数(u32なので、u16ずつセットしているので注意！)
	for( i=0; i < CASTLE_ENEMY_POKE_MAX ;i++ ){
		//u32 enemy_personal_rnd[CASTLE_ENEMY_POKE_MAX];
		wk->enemy_personal_rnd[i] = recv_buf[i+num];
		wk->enemy_personal_rnd[i] |= (recv_buf[i+num+CASTLE_ENEMY_POKE_MAX] << 16);
		OS_Printf( "受信：wk->enemy_personal_rnd[%d] = %d\n", i, wk->enemy_personal_rnd[i] );
	}
	num += (CASTLE_ENEMY_POKE_MAX * 2);								//16

	return;
}

//--------------------------------------------------------------
/**
 * @brief	send_bufに戦闘後のメニューで何を選んだかをセット
 *
 * @param	wk		CASTLE_SCRWORK型のポインタ
 * @param	sel		0=つづける、0以外=リタイヤ(きろくするはいらない)
 *
 * @return	none
 *
 * お互いが「つづける」の時は継続、
 * 両方、どちらかが「リタイヤ」の時は中止
 */
//--------------------------------------------------------------
BOOL CommCastleSendBufRetireFlag( CASTLE_SCRWORK* wk, u16 retire_flag )
{
	int ret,size;

	size = CASTLE_COMM_BUF_LEN;						//実際はu16データなのでこの半分

	wk->send_buf[0] = retire_flag;
	OS_Printf( "送信：retire_flag = %d\n", wk->send_buf[0] );

	if( CommSendData(FC_CASTLE_RETIRE_FLAG,wk->send_buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief	recv_bufの戦闘後のメニューで何を選んだかを取得
 *
 * @param	wk			CASTLE_SCRWORK型のポインタ
 * @param	recv_buf	
 *
 * @return	none
 *
 * お互いが「つづける」の時は継続、
 * 両方、どちらかが「リタイヤ」の時は中止
 */
//--------------------------------------------------------------
void CommCastleRecvBufRetireFlag(int id_no,int size,void *pData,void *work)
{
	int num;
	CASTLE_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******キャッスル****** リタイア情報受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	num = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	OS_Printf( "size = %d\n", size );
	//OS_Printf( "pData[0] = %d\n", ((u8*)pData)[0] );
	OS_Printf( "recv_buf[0] = %d\n", recv_buf[0] );

	wk->pair_retire_flag = (u8)recv_buf[0];
	OS_Printf( "受信：wk->pair_retire_flag = %d\n", wk->pair_retire_flag );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	send_bufに交換したいかををセット
 *
 * @param	wk		CASTLE_SCRWORK型のポインタ
 * @param	sel		0=交換したい、1=交換したくない
 *
 * @return	none
 */
//--------------------------------------------------------------
BOOL CommCastleSendBufTradeYesNoFlag( CASTLE_SCRWORK* wk, u16 trade_yesno_flag )
{
	int ret,size;

	size = CASTLE_COMM_BUF_LEN;						//実際はu16データなのでこの半分

	wk->send_buf[0] = trade_yesno_flag;
	OS_Printf( "送信：trade_yesno_flag = %d\n", wk->send_buf[0] );

	if( CommSendData(FC_CASTLE_TRADE_YESNO_FLAG,wk->send_buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief	recv_bufの交換したいかを取得
 *
 * @param	wk			CASTLE_SCRWORK型のポインタ
 * @param	recv_buf	
 *
 * @return	none
 */
//--------------------------------------------------------------
void CommCastleRecvBufTradeYesNoFlag(int id_no,int size,void *pData,void *work)
{
	int num;
	CASTLE_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******キャッスル****** 交換はい いいえ情報受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	num = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	OS_Printf( "size = %d\n", size );
	//OS_Printf( "pData[0] = %d\n", ((u8*)pData)[0] );
	OS_Printf( "recv_buf[0] = %d\n", recv_buf[0] );

	wk->pair_trade_yesno_flag = (u8)recv_buf[0];
	OS_Printf( "受信：wk->pair_trade_yesno_flag = %d\n", wk->pair_trade_yesno_flag );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	send_bufに手持ちポケモンデータをセット
 *
 * @param	wk		CASTLE_SCRWORK型のポインタ
 *
 * @return	none
 *
 * 親機、子機の両方とも選んだ手持ちを渡す必要がある
 */
//--------------------------------------------------------------
BOOL CommCastleSendBufTemotiPokeData( CASTLE_SCRWORK* wk )
{
	int i,num,ret,size,pp_size;
	u8 m_max;
	POKEMON_PARAM* temp_poke;
	
	num = 0;
	size = CASTLE_HUGE_BUF_LEN;

	//タイプによってポケモンの数を取得
	m_max = Castle_GetMinePokeNum( wk->type, CASTLE_FLAG_SOLO );

	//POKEMON_PARAMのサイズを取得
	pp_size = PokemonParam_GetWorkSize();
	OS_Printf( "pokemon_param_size = %d\n", pp_size );

	for( i=0; i < m_max ;i++ ){
		temp_poke = PokeParty_GetMemberPointer( wk->p_m_party, i );
		MI_CpuCopy8( temp_poke, &wk->huge_buf[i*pp_size], pp_size );
	}

	OS_Printf( "送信：pokemon_param\n" );

	if( CommSendHugeData(FC_CASTLE_TEMOTI_POKE_DATA,wk->huge_buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	OS_Printf( "送信結果：CommSendHugeData = %d\n", ret );
	return ret;
}

//--------------------------------------------------------------
/**
 * @brief	recv_bufの手持ちポケモンデータを取得
 *
 * @param	wk			CASTLE_SCRWORK型のポインタ
 * @param	recv_buf	
 *
 * @return	none
 *
 * 親機、子機の両方とも選んだ手持ちを渡す必要がある
 *
 * pair_rental_...を使いまわしている
 */
//--------------------------------------------------------------
void CommCastleRecvBufTemotiPokeData(int id_no,int size,void *pData,void *work)
{
	u8 m_max;
	int i,num,pp_size;
	POKEMON_PARAM* temp_poke;
	CASTLE_SCRWORK* wk = work;
	const u8* recv_buf = pData;

	OS_Printf( "******キャッスル****** 手持ちポケモン情報受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	num = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	OS_Printf( "size = %d\n", size );
	OS_Printf( "recv_buf[0] = %d\n", recv_buf[0] );

	//タイプによってポケモンの数を取得
	m_max = Castle_GetMinePokeNum( wk->type, CASTLE_FLAG_SOLO );

	pp_size = PokemonParam_GetWorkSize();

	temp_poke = PokemonParam_AllocWork( HEAPID_WORLD );
	for( i=0; i < m_max ;i++ ){
		MI_CpuCopy8( &recv_buf[pp_size*i], temp_poke, pp_size );
		PokeParty_Add( wk->p_m_party, temp_poke );
	}
	sys_FreeMemoryEz( temp_poke );

	//////////////////////////////////////////////////////
	//子は手持ちポケモンを後ろにする
	if( CommGetCurrentID() != COMM_PARENT_ID ){
		PokeParty_ExchangePosition( wk->p_m_party, 0, 2 );
		PokeParty_ExchangePosition( wk->p_m_party, 1, 3 );
	}
	//////////////////////////////////////////////////////

	OS_Printf( "ポケモンパーティ数 = %d\n",	PokeParty_GetPokeCount(wk->p_m_party) );
	return;
}

//--------------------------------------------------------------
/**
 * @brief   受信領域取得関数(巨大データ：CommSendHugeDataの受信用)
 *
 * @param   netID		
 * @param   pWork		
 * @param   size		
 *
 * @retval  
 */
//--------------------------------------------------------------
u8* CommCastleGetRecvDataBuff( int netID, void*pWork, int size )
{
	CASTLE_SCRWORK* wk = pWork;
	OS_Printf( "size = %d\n", size );
	GF_ASSERT(size <= CASTLE_HUGE_BUF_LEN);
    return wk->recieve_huge_buf[netID];
}


//==============================================================================
//
//	フロンティアWiFi受付
//
//==============================================================================
#if 0
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
	int ret,size;

	OS_Printf( "******WIFI受付****** 選択した施設No送信\n" );

	size = FRWIFI_COMM_BUF_LEN;

	wk->send_buf[0]	= wk->bf_no;								//施設No

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

	size = FRWIFI_COMM_BUF_LEN;

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

	size = FRWIFI_COMM_BUF_LEN;

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
	}else{
		pp = PokeParty_GetMemberPointer( party, pos1 );
		wk->monsno[0] = PokeParaGet( pp, ID_PARA_monsno, NULL );
		wk->itemno[0] = PokeParaGet( pp, ID_PARA_item, NULL );
	}

	wk->send_buf[0] = wk->monsno[0];
	wk->send_buf[1] = wk->itemno[0];
	OS_Printf( "送信：monsno1 = %d\n", wk->send_buf[0] );
	OS_Printf( "送信：itemno1 = %d\n", wk->send_buf[1] );

	////////////////////////////////////////////////////////////
	party = SaveData_GetTemotiPokemon( wk->sv );
	pp = PokeParty_GetMemberPointer( party, pos2 );

	wk->monsno[1] = PokeParaGet( pp, ID_PARA_monsno, NULL );
	wk->itemno[1] = PokeParaGet( pp, ID_PARA_item, NULL );

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

	size = FRWIFI_COMM_BUF_LEN;

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

	wk->pair_stage_del = (u8)recv_buf[0];
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

	size = FRWIFI_COMM_BUF_LEN;

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


//==============================================================================
//
//	ルーレット
//
//	構造体をきって、そのサイズ、受信はその型で代入に変更予定。。。
//	
//==============================================================================

//--------------------------------------------------------------
/**
 * @brief	send_bufに基本情報をセット
 *
 * @param	wk		ROULETTE_SCRWORK型のポインタ
 *
 * @retval  TRUE:送信した。　FALSE:送信失敗
 *
 * ルーレットワークを確保後、最初に送るデータ
 */
//--------------------------------------------------------------
BOOL CommRouletteSendBufBasicData( ROULETTE_SCRWORK* wk )
{
	int i,num,ret,size;
	ROULETTESCORE* score_sv;

	OS_Printf( "******ルーレット****** 基本情報送信\n" );

	score_sv = SaveData_GetRouletteScore( wk->sv );
	size = ROULETTE_COMM_BUF_LEN;						//実際はu16データなのでこの半分
	num = 0;

	//wk->send_buf[0]	= 
	wk->send_buf[1] = wk->rensyou;						//現在の連勝数
	wk->send_buf[2] = wk->lap;							//周回数
	num += 3;											//3

	if( CommSendData(FC_ROULETTE_BASIC_DATA,wk->send_buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief	recv_bufの基本情報を取得
 *
 * @param	wk			ROULETTE_SCRWORK型のポインタ
 * @param	recv_buf	
 *
 * @return	none
 *
 * ルーレットワークを確保後、最初に受け取るデータ
 */
//--------------------------------------------------------------
void CommRouletteRecvBufBasicData(int id_no,int size,void *pData,void *work)
{
	int i,num;
	ROULETTE_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******ルーレット****** 基本情報受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	num = 0;
	wk->recieve_count++;
	//OS_Printf( "wk->recieve_count = %d\n", wk->recieve_count );

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	//wk->	= 
	//
	wk->pair_rensyou		= recv_buf[1];							//現在の連勝数
	OS_Printf( "受信：wk->pair_rensyou = %d\n", wk->pair_rensyou );
	wk->pair_lap			= recv_buf[2];							//周回数
	OS_Printf( "受信：wk->pair_lap = %d\n", wk->pair_lap );
	num += 3;														//3

	return;
}

//--------------------------------------------------------------
/**
 * @brief	send_bufにトレーナー情報をセット
 *
 * @param	wk		ROULETTE_SCRWORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
BOOL CommRouletteSendBufTrData( ROULETTE_SCRWORK* wk )
{
	int i,num,ret,size;
	
	OS_Printf( "******ルーレット****** トレーナー情報送信\n" );

	num = 0;
	size = ROULETTE_COMM_BUF_LEN;						//実際はu16データなのでこの半分

	//敵トレーナーデータ
	for( i=0; i < ROULETTE_LAP_MULTI_ENEMY_MAX ;i++ ){
		wk->send_buf[i+num] = wk->tr_index[i];
		OS_Printf( "送信：tr_index[%d] = %d\n", i, wk->send_buf[i+num] );
	}
	num += ROULETTE_LAP_MULTI_ENEMY_MAX;						//14

	if( CommSendData(FC_ROULETTE_TR_DATA,wk->send_buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief	recv_bufのトレーナー情報を取得
 *
 * @param	wk			ROULETTE_SCRWORK型のポインタ
 * @param	recv_buf	
 *
 * @return	none
 *
 * ルーレットワークを確保後、最初に受け取るデータ
 */
//--------------------------------------------------------------
void CommRouletteRecvBufTrData(int id_no,int size,void *pData,void *work)
{
	int i,num;
	ROULETTE_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******ルーレット****** トレーナー情報受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	num = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	//親は送信するだけなので受け取らない
	if( CommGetCurrentID() == COMM_PARENT_ID ){
		return;
	}

	//敵トレーナーデータ
	for( i=0; i < ROULETTE_LAP_MULTI_ENEMY_MAX ;i++ ){
		wk->tr_index[i] = recv_buf[i+num];
		OS_Printf( "受信：wk->tr_index[%d] = %d\n", i, wk->tr_index[i] );
	}
	num += ROULETTE_LAP_MULTI_ENEMY_MAX;								//14

	return;
}

//--------------------------------------------------------------
/**
 * @brief	send_bufに敵ポケモンデータをセット
 *
 * @param	wk		ROULETTE_SCRWORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
BOOL CommRouletteSendBufEnemyPokeData( ROULETTE_SCRWORK* wk )
{
	int i,num,ret,size;
	
	OS_Printf( "******ルーレット****** 敵ポケモン送信\n" );

	num = 0;
	size = ROULETTE_COMM_BUF_LEN;						//実際はu16データなのでこの半分

	//ポケモンindexテーブル
	for( i=0; i < ROULETTE_ENEMY_POKE_MAX ;i++ ){
		wk->send_buf[i] = wk->enemy_poke_index[i];
		OS_Printf( "送信：enemy_index[%d] = %d\n", i, wk->send_buf[i] );
	}
	num += ROULETTE_ENEMY_POKE_MAX;										//4

	//ポケモンのパワー乱数
	for( i=0; i < ROULETTE_ENEMY_POKE_MAX ;i++ ){
		wk->send_buf[i+num] = wk->enemy_pow_rnd[i];
		OS_Printf( "送信：enemy_pow_rnd[%d] = %d\n", i, wk->send_buf[i+num] );
	}
	num += ROULETTE_ENEMY_POKE_MAX;										//8
	
	//ポケモンの個性乱数(u32なので、u16ずつセットしているので注意！)
	for( i=0; i < ROULETTE_ENEMY_POKE_MAX ;i++ ){
		//u32 enemy_personal_rnd[ROULETTE_ENEMY_POKE_MAX];
		wk->send_buf[i+num] = (wk->enemy_personal_rnd[i] & 0xffff);
		wk->send_buf[i+num+ROULETTE_ENEMY_POKE_MAX] = 
								((wk->enemy_personal_rnd[i] >> 16) & 0xffff);
		OS_Printf( "元データ enemy_personal_rnd[%d] = %d\n", i, wk->enemy_personal_rnd[i] );
		OS_Printf( "送信：enemy_personal_rnd[%d] = %d\n", i, wk->send_buf[i+num] );
		OS_Printf( "送信：enemy_personal_rnd[%d] = %d\n", i+ROULETTE_ENEMY_POKE_MAX, 
													wk->send_buf[i+num+ROULETTE_ENEMY_POKE_MAX] );
	}
	num += (ROULETTE_ENEMY_POKE_MAX * 2);								//16

	//B_TOWER_POKEMON enemy_poke[ROULETTE_ENEMY_POKE_MAX];

	if( CommSendData(FC_ROULETTE_ENEMY_POKE_DATA,wk->send_buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief	recv_bufの敵ポケモンデータを取得
 *
 * @param	wk			ROULETTE_SCRWORK型のポインタ
 * @param	recv_buf	
 *
 * @return	none
 */
//--------------------------------------------------------------
void CommRouletteRecvBufEnemyPokeData(int id_no,int size,void *pData,void *work)
{
	int i,num;
	ROULETTE_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******ルーレット****** 敵ポケモン情報受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	num = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	//親は送信するだけなので受け取らない
	if( CommGetCurrentID() == COMM_PARENT_ID ){
		return;
	}

	OS_Printf( "size = %d\n", size );
	//OS_Printf( "pData[0] = %d\n", ((u8*)pData)[0] );
	OS_Printf( "recv_buf[0] = %d\n", recv_buf[0] );

	//ポケモンindexテーブル
	for( i=0; i < ROULETTE_ENEMY_POKE_MAX ;i++ ){
		wk->enemy_poke_index[i] = recv_buf[i];
		OS_Printf( "受信：wk->enemy_index[%d] = %d\n", i, wk->enemy_poke_index[i] );
	}
	num += ROULETTE_ENEMY_POKE_MAX;										//4

	//ポケモンのパワー乱数
	for( i=0; i < ROULETTE_ENEMY_POKE_MAX ;i++ ){
		wk->enemy_pow_rnd[i] = recv_buf[i+num];
		OS_Printf( "受信：wk->enemy_pow_rnd[%d] = %d\n", i, wk->enemy_pow_rnd[i] );
	}
	num += ROULETTE_ENEMY_POKE_MAX;										//8
	
	//ポケモンの個性乱数(u32なので、u16ずつセットしているので注意！)
	for( i=0; i < ROULETTE_ENEMY_POKE_MAX ;i++ ){
		//u32 enemy_personal_rnd[ROULETTE_ENEMY_POKE_MAX];
		wk->enemy_personal_rnd[i] = recv_buf[i+num];
		wk->enemy_personal_rnd[i] |= (recv_buf[i+num+ROULETTE_ENEMY_POKE_MAX] << 16);
		OS_Printf( "受信：wk->enemy_personal_rnd[%d] = %d\n", i, wk->enemy_personal_rnd[i] );
	}
	num += (ROULETTE_ENEMY_POKE_MAX * 2);								//16

#if 0
	//B_TOWER_POKEMON enemy_poke[ROULETTE_ENEMY_POKE_MAX];

	//敵ポケモンを一度に生成
	Frontier_PokemonParamCreateAll(	wk->enemy_poke, wk->enemy_poke_index, 
									wk->enemy_pow_rnd, wk->enemy_personal_rnd, NULL, 
									ROULETTE_ENEMY_POKE_MAX, HEAPID_WORLD, ARC_PL_BTD_PM );

	//敵パーティのセット
	Roulette_EnemyPartySet( wk );
#endif
	return;
}

//--------------------------------------------------------------
/**
 * @brief	send_bufに戦闘後のメニューで何を選んだかをセット
 *
 * @param	wk		ROULETTE_SCRWORK型のポインタ
 * @param	sel		0=つづける、0以外=リタイヤ(きろくするはいらない)
 *
 * @return	none
 *
 * お互いが「つづける」の時は継続、
 * 両方、どちらかが「リタイヤ」の時は中止
 */
//--------------------------------------------------------------
BOOL CommRouletteSendBufRetireFlag( ROULETTE_SCRWORK* wk, u16 retire_flag )
{
	int ret,size;

	size = ROULETTE_COMM_BUF_LEN;						//実際はu16データなのでこの半分

	wk->send_buf[0] = retire_flag;
	OS_Printf( "送信：retire_flag = %d\n", wk->send_buf[0] );

	if( CommSendData(FC_ROULETTE_RETIRE_FLAG,wk->send_buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief	recv_bufの戦闘後のメニューで何を選んだかを取得
 *
 * @param	wk			ROULETTE_SCRWORK型のポインタ
 * @param	recv_buf	
 *
 * @return	none
 *
 * お互いが「つづける」の時は継続、
 * 両方、どちらかが「リタイヤ」の時は中止
 */
//--------------------------------------------------------------
void CommRouletteRecvBufRetireFlag(int id_no,int size,void *pData,void *work)
{
	int num;
	ROULETTE_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******ルーレット****** リタイア情報受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	num = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	OS_Printf( "size = %d\n", size );
	//OS_Printf( "pData[0] = %d\n", ((u8*)pData)[0] );
	OS_Printf( "recv_buf[0] = %d\n", recv_buf[0] );

	wk->pair_retire_flag = (u8)recv_buf[0];
	OS_Printf( "受信：wk->pair_retire_flag = %d\n", wk->pair_retire_flag );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	send_bufに交換したいかををセット
 *
 * @param	wk		ROULETTE_SCRWORK型のポインタ
 * @param	sel		0=交換したい、1=交換したくない
 *
 * @return	none
 */
//--------------------------------------------------------------
BOOL CommRouletteSendBufTradeYesNoFlag( ROULETTE_SCRWORK* wk, u16 trade_yesno_flag )
{
	int ret,size;

	size = ROULETTE_COMM_BUF_LEN;						//実際はu16データなのでこの半分

	wk->send_buf[0] = trade_yesno_flag;
	OS_Printf( "送信：trade_yesno_flag = %d\n", wk->send_buf[0] );

	if( CommSendData(FC_ROULETTE_TRADE_YESNO_FLAG,wk->send_buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief	recv_bufの交換したいかを取得
 *
 * @param	wk			ROULETTE_SCRWORK型のポインタ
 * @param	recv_buf	
 *
 * @return	none
 */
//--------------------------------------------------------------
void CommRouletteRecvBufTradeYesNoFlag(int id_no,int size,void *pData,void *work)
{
	int num;
	ROULETTE_SCRWORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******ルーレット****** 交換はい いいえ情報受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	num = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	OS_Printf( "size = %d\n", size );
	//OS_Printf( "pData[0] = %d\n", ((u8*)pData)[0] );
	OS_Printf( "recv_buf[0] = %d\n", recv_buf[0] );

	wk->pair_trade_yesno_flag = (u8)recv_buf[0];
	OS_Printf( "受信：wk->pair_trade_yesno_flag = %d\n", wk->pair_trade_yesno_flag );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	send_bufに手持ちポケモンデータをセット
 *
 * @param	wk		ROULETTE_SCRWORK型のポインタ
 *
 * @return	none
 *
 * 親機、子機の両方とも選んだ手持ちを渡す必要がある
 */
//--------------------------------------------------------------
BOOL CommRouletteSendBufTemotiPokeData( ROULETTE_SCRWORK* wk )
{
	int i,num,ret,size,pp_size;
	u8 m_max;
	POKEMON_PARAM* temp_poke;
	
	num = 0;
	size = ROULETTE_HUGE_BUF_LEN;

	//タイプによってポケモンの数を取得
	m_max = Roulette_GetMinePokeNum( wk->type, ROULETTE_FLAG_SOLO );

	//POKEMON_PARAMのサイズを取得
	pp_size = PokemonParam_GetWorkSize();
	OS_Printf( "pokemon_param_size = %d\n", pp_size );

	for( i=0; i < m_max ;i++ ){
		temp_poke = PokeParty_GetMemberPointer( wk->p_m_party, i );
		MI_CpuCopy8( temp_poke, &wk->huge_buf[i*pp_size], pp_size );
	}

	OS_Printf( "送信：pokemon_param\n" );

	if( CommSendHugeData(FC_ROULETTE_TEMOTI_POKE_DATA,wk->huge_buf,size) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	OS_Printf( "送信結果：CommSendHugeData = %d\n", ret );
	return ret;
}

//--------------------------------------------------------------
/**
 * @brief	recv_bufの手持ちポケモンデータを取得
 *
 * @param	wk			ROULETTE_SCRWORK型のポインタ
 * @param	recv_buf	
 *
 * @return	none
 *
 * 親機、子機の両方とも選んだ手持ちを渡す必要がある
 *
 * pair_rental_...を使いまわしている
 */
//--------------------------------------------------------------
void CommRouletteRecvBufTemotiPokeData(int id_no,int size,void *pData,void *work)
{
	u8 m_max;
	int i,num,pp_size;
	POKEMON_PARAM* temp_poke;
	ROULETTE_SCRWORK* wk = work;
	const u8* recv_buf = pData;

	OS_Printf( "******ルーレット****** 手持ちポケモン情報受信\n" );
	OS_Printf( "id_no = %d\n", id_no );

	num = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	OS_Printf( "size = %d\n", size );
	//OS_Printf( "pData[0] = %d\n", ((u8*)pData)[0] );
	OS_Printf( "recv_buf[0] = %d\n", recv_buf[0] );

	//タイプによってポケモンの数を取得
	m_max = Roulette_GetMinePokeNum( wk->type, ROULETTE_FLAG_SOLO );

	pp_size = PokemonParam_GetWorkSize();

	temp_poke = PokemonParam_AllocWork( HEAPID_WORLD );
	for( i=0; i < m_max ;i++ ){
		MI_CpuCopy8( &recv_buf[pp_size*i], temp_poke, pp_size );
		PokeParty_Add( wk->p_m_party, temp_poke );
	}
	sys_FreeMemoryEz( temp_poke );

	//////////////////////////////////////////////////////
	//子は手持ちポケモンを後ろにする
	if( CommGetCurrentID() != COMM_PARENT_ID ){
		PokeParty_ExchangePosition( wk->p_m_party, 0, 2 );
		PokeParty_ExchangePosition( wk->p_m_party, 1, 3 );
	}
	//////////////////////////////////////////////////////

	OS_Printf( "ポケモンパーティ数 = %d\n",	PokeParty_GetPokeCount(wk->p_m_party) );
	return;
}

//--------------------------------------------------------------
/**
 * @brief   受信領域取得関数(巨大データ：CommSendHugeDataの受信用)
 *
 * @param   netID		
 * @param   pWork		
 * @param   size		
 *
 * @retval  
 */
//--------------------------------------------------------------
u8* CommRouletteGetRecvDataBuff( int netID, void*pWork, int size )
{
	ROULETTE_SCRWORK* wk = pWork;
	OS_Printf( "size = %d\n", size );
	GF_ASSERT(size <= ROULETTE_HUGE_BUF_LEN);
    return wk->recieve_huge_buf[netID];
}


