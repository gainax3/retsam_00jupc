//=============================================================================
/**
 * @file	comm_command_frontier.h
 * @brief	通信のコマンド一覧  フロンティア用
 * @author	nohara
 * @date    2007.07.18
 */
//=============================================================================
#ifndef __FRONTIER_COMM_H__
#define __FRONTIER_COMM_H__

#include "factory_tool.h"
#include "stage_tool.h"
#include "castle_tool.h"
#include "roulette_tool.h"
#include "frontier_wifi.h"


//==============================================================================
//
//  extern宣言
//
//==============================================================================
//ファクトリー
extern BOOL		CommFactorySendBasicData( FACTORY_SCRWORK* wk );
extern void CommFactoryRecvBasicData(int id_no,int size,void *pData,void *work);
extern BOOL		CommFactorySendTrData( FACTORY_SCRWORK* wk );
extern void CommFactoryRecvTrData(int id_no,int size,void *pData,void *work);
extern BOOL		CommFactorySendRentalData( FACTORY_SCRWORK* wk );
extern void CommFactoryRecvRentalData(int id_no,int size,void *pData,void *work);
extern BOOL		CommFactorySendEnemyPokeData( FACTORY_SCRWORK* wk );
extern void CommFactoryRecvEnemyPokeData(int id_no,int size,void *pData,void *work);
extern BOOL		CommFactorySendRetireFlag( FACTORY_SCRWORK* wk, u8 retire_flag );
extern void CommFactoryRecvRetireFlag(int id_no,int size,void *pData,void *work);
extern BOOL		CommFactorySendTradeYesNoFlag( FACTORY_SCRWORK* wk, u8 trade_yesno_flag );
extern void CommFactoryRecvTradeYesNoFlag(int id_no,int size,void *pData,void *work);
extern BOOL		CommFactorySendTemotiPokeData( FACTORY_SCRWORK* wk );
extern void CommFactoryRecvTemotiPokeData(int id_no,int size,void *pData,void *work);

//ステージ
extern BOOL		CommStageSendMonsNo( STAGE_SCRWORK* wk, const POKEMON_PARAM* pp );
extern void CommStageRecvMonsNo(int id_no,int size,void *pData,void *work);
extern BOOL		CommStageSendBasicData( STAGE_SCRWORK* wk );
extern void CommStageRecvBasicData(int id_no,int size,void *pData,void *work);
extern BOOL		CommStageSendTrData( STAGE_SCRWORK* wk );
extern void CommStageRecvTrData(int id_no,int size,void *pData,void *work);
extern BOOL		CommStageSendEnemyPokeData( STAGE_SCRWORK* wk );
extern void CommStageRecvEnemyPokeData(int id_no,int size,void *pData,void *work);
extern BOOL		CommStageSendRetireFlag( STAGE_SCRWORK* wk, u8 retire_flag );
extern void CommStageRecvRetireFlag(int id_no,int size,void *pData,void *work);
extern BOOL		CommStageSendBufTemotiPokeData( STAGE_SCRWORK* wk );
extern void CommStageRecvBufTemotiPokeData(int id_no,int size,void *pData,void *work);
extern u8* CommStageGetRecvDataBuff( int netID, void*pWork, int size );

//キャッスル
extern BOOL		CommCastleSendBufBasicData( CASTLE_SCRWORK* wk );
extern void CommCastleRecvBufBasicData(int id_no,int size,void *pData,void *work);
extern BOOL		CommCastleSendBufTrData( CASTLE_SCRWORK* wk );
extern void CommCastleRecvBufTrData(int id_no,int size,void *pData,void *work);
extern BOOL		CommCastleSendBufSelData( CASTLE_SCRWORK* wk );
extern void CommCastleRecvBufSelData(int id_no,int size,void *pData,void *work);
extern BOOL		CommCastleSendBufEnemyPokeData( CASTLE_SCRWORK* wk );
extern void CommCastleRecvBufEnemyPokeData(int id_no,int size,void *pData,void *work);
extern BOOL		CommCastleSendBufRetireFlag( CASTLE_SCRWORK* wk, u16 retire_flag );
extern void CommCastleRecvBufRetireFlag(int id_no,int size,void *pData,void *work);
extern BOOL		CommCastleSendBufTradeYesNoFlag( CASTLE_SCRWORK* wk, u16 trade_yesno_flag );
extern void CommCastleRecvBufTradeYesNoFlag(int id_no,int size,void *pData,void *work);
extern BOOL		CommCastleSendBufTemotiPokeData( CASTLE_SCRWORK* wk );
extern void CommCastleRecvBufTemotiPokeData(int id_no,int size,void *pData,void *work);
extern u8* CommCastleGetRecvDataBuff( int netID, void*pWork, int size );

//WiFi受付
#if 0
extern BOOL CommFrWiFiCounterSendBufBFNo( FRWIFI_SCRWORK* wk );
extern void CommFrWiFiCounterRecvBufBFNo(int id_no,int size,void *pData,void *work);
extern BOOL CommFrWiFiCounterSendBufRetireFlag( FRWIFI_SCRWORK* wk, u16 retire_flag );
extern void CommFrWiFiCounterRecvBufRetireFlag(int id_no,int size,void *pData,void *work);
extern BOOL CommFrWiFiCounterSendBufMonsNoItemNo( FRWIFI_SCRWORK* wk, u16 pos1, u16 pos2 );
extern void CommFrWiFiCounterRecvBufMonsNoItemNo(int id_no,int size,void *pData,void *work);
extern BOOL CommFrWiFiCounterSendBufStageRecordDel( FRWIFI_SCRWORK* wk, u16 stage_del_flag );
extern void CommFrWiFiCounterRecvBufStageRecordDel(int id_no,int size,void *pData,void *work);
extern BOOL CommFrWiFiCounterSendBufGameContinue( FRWIFI_SCRWORK* wk, u16 flag );
extern void CommFrWiFiCounterRecvBufGameContinue(int id_no,int size,void *pData,void *work);
#endif

//ルーレット
extern BOOL		CommRouletteSendBufBasicData( ROULETTE_SCRWORK* wk );
extern void CommRouletteRecvBufBasicData(int id_no,int size,void *pData,void *work);
extern BOOL		CommRouletteSendBufTrData( ROULETTE_SCRWORK* wk );
extern void CommRouletteRecvBufTrData(int id_no,int size,void *pData,void *work);
extern BOOL		CommRouletteSendBufEnemyPokeData( ROULETTE_SCRWORK* wk );
extern void CommRouletteRecvBufEnemyPokeData(int id_no,int size,void *pData,void *work);
extern BOOL		CommRouletteSendBufRetireFlag( ROULETTE_SCRWORK* wk, u16 retire_flag );
extern void CommRouletteRecvBufRetireFlag(int id_no,int size,void *pData,void *work);
extern BOOL		CommRouletteSendBufTradeYesNoFlag( ROULETTE_SCRWORK* wk, u16 trade_yesno_flag );
extern void CommRouletteRecvBufTradeYesNoFlag(int id_no,int size,void *pData,void *work);
extern BOOL		CommRouletteSendBufTemotiPokeData( ROULETTE_SCRWORK* wk );
extern void CommRouletteRecvBufTemotiPokeData(int id_no,int size,void *pData,void *work);
extern u8* CommRouletteGetRecvDataBuff( int netID, void*pWork, int size );


#endif// __FRONTIER_COMM_H__


