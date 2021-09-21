//============================================================================================
/**
 * @file	fssc_roulette.h
 * @bfief	フロンティアシステムスクリプトコマンド：ルーレット
 * @author	Satoshi Nohara
 * @date	07.09.06
 */
//============================================================================================
#ifndef	__FSSC_ROULETTE_H__
#define	__FSSC_ROULETTE_H__

#include "poketool/pokeparty.h"
#include "poketool/poke_tool.h"
#include "savedata/roulette_savedata.h"		//ROULETTEDATA
#include "roulette_def.h"						//ROULETTE_LAP_ENEMY_MAX


//============================================================================================
//
//	extern宣言
//
//============================================================================================
extern BOOL FSSC_RouletteWorkAlloc(FSS_TASK * core );
extern BOOL FSSC_RouletteWorkInit(FSS_TASK * core );
extern BOOL FSSC_RouletteWorkFree(FSS_TASK * core );
extern BOOL FSSC_RouletteMineCall(FSS_TASK * core);
extern BOOL FSSC_RouletteRankUpCall(FSS_TASK * core);
extern BOOL FSSC_RouletteCallGetResult(FSS_TASK * core);
extern BOOL FSSC_RouletteBattleCall(FSS_TASK * core);
extern BOOL FSSC_RouletteEnemyCall(FSS_TASK * core);
extern BOOL FSSC_RouletteBtlBeforePokePartySet(FSS_TASK * core);
extern BOOL FSSC_RouletteBtlAfterPokePartySet(FSS_TASK * core);
extern BOOL FSSC_RouletteScrWork(FSS_TASK * core);
extern BOOL FSSC_RouletteLoseCheck( FSS_TASK * core );
extern BOOL FSSC_RouletteSendBuf( FSS_TASK * core );
extern BOOL FSSC_RouletteRecvBuf( FSS_TASK * core );
extern BOOL FSSC_RouletteParentCheckWait( FSS_TASK * core );
extern BOOL FSSC_CPWinWrite( FSS_TASK * core );
extern BOOL FSSC_CPWinDel( FSS_TASK * core );
extern BOOL FSSC_CPWrite( FSS_TASK * core );
extern BOOL FSSC_RouletteGetCP( FSS_TASK * core );
extern BOOL FSSC_RouletteSubCP( FSS_TASK * core );
extern BOOL FSSC_RouletteAddCP( FSS_TASK * core );
extern BOOL FSSC_RouletteTalkMsgAppear(FSS_TASK* core);
extern BOOL FSSC_RouletteDecideEvNoFunc(FSS_TASK* core);
extern BOOL FSSC_RouletteChgItemKeepVanish(FSS_TASK* core);


#endif	//__FSSC_ROULETTE_H__


