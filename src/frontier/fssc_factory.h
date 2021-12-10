//============================================================================================
/**
 * @file	fssc_factory.h
 * @bfief	フロンティアシステムスクリプトコマンド：ファクトリー
 * @author	Satoshi Nohara
 * @date	07.04.10
 */
//============================================================================================
#ifndef	__FSSC_FACTORY_H__
#define	__FSSC_FACTORY_H__

#include "poketool/pokeparty.h"
#include "poketool/poke_tool.h"
#include "savedata/factory_savedata.h"		//FACTORYDATA
#include "factory_def.h"					//FACTORY_LAP_ENEMY_MAX


//============================================================================================
//
//	extern宣言
//
//============================================================================================
extern BOOL FSSC_FactoryWorkAlloc(FSS_TASK * core );
extern BOOL FSSC_FactoryWorkInit(FSS_TASK * core );
extern BOOL FSSC_FactoryWorkFree(FSS_TASK * core );
extern BOOL FSSC_FactoryRentalCall(FSS_TASK * core);
extern BOOL FSSC_FactoryCallGetResult(FSS_TASK * core);
extern BOOL FSSC_FactoryBattleCall(FSS_TASK * core);
extern BOOL FSSC_FactoryTradeCall(FSS_TASK * core);
extern BOOL FSSC_FactoryRentalPokePartySet(FSS_TASK * core);
extern BOOL FSSC_FactoryBtlAfterPokePartySet(FSS_TASK * core);
extern BOOL FSSC_FactoryTradePokeChange(FSS_TASK * core);
extern BOOL FSSC_FactoryTradeAfterPokePartySet(FSS_TASK * core);
extern BOOL FSSC_FactoryScrWork(FSS_TASK * core);
extern BOOL FSSC_FactoryLoseCheck( FSS_TASK * core );
extern BOOL FSSC_FactorySendBuf( FSS_TASK * core );
extern BOOL FSSC_FactoryRecvBuf( FSS_TASK * core );
extern BOOL FSSC_FactoryTalkMsgAppear(FSS_TASK* core);


#endif	//__FSSC_FACTORY_H__


