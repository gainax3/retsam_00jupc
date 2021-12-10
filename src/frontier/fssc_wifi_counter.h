//============================================================================================
/**
 * @file	fssc_wifi_counter.h
 * @bfief	フロンティアシステムスクリプトコマンド：WiFi受付
 * @author	Satoshi Nohara
 * @date	07.07.31
 */
//============================================================================================
#ifndef	__FSSC_WIFI_COUNTER_H__
#define	__FSSC_WIFICOUNTER_H__


//============================================================================================
//
//	extern宣言
//
//============================================================================================
extern BOOL FSSC_WiFiCounterWorkAlloc( FSS_TASK * core );
extern BOOL FSSC_WiFiCounterWorkFree( FSS_TASK * core );
extern BOOL FSSC_WiFiCounterSendBuf( FSS_TASK * core );
extern BOOL FSSC_WiFiCounterRecvBuf( FSS_TASK * core );
extern BOOL FSSC_WiFiCounterBFNoCheck( FSS_TASK * core );
extern BOOL FSSC_WiFiCounterPokeList( FSS_TASK * core );
extern BOOL FSSC_WiFiCounterPokeListGetResult(FSS_TASK * core);
extern BOOL FSSC_WiFiCounterPairYameruCheck( FSS_TASK * core );
extern BOOL FSSC_WiFiCounterScrWork( FSS_TASK * core );
extern BOOL FSSC_WiFiCounterTowerCallBefore( FSS_TASK * core );
extern BOOL FSSC_WiFiCounterTowerSendTrainerData( FSS_TASK * core );
extern BOOL FSSC_WiFiCounterTowerRecvTrainerData( FSS_TASK * core );
extern BOOL FSSC_WiFiCounterTowerCallAfter( FSS_TASK * core );


#endif	//__FSSC_WIFI_COUNTER_H__



