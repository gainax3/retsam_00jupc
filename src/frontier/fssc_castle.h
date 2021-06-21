//============================================================================================
/**
 * @file	fssc_castle.h
 * @bfief	フロンティアシステムスクリプトコマンド：キャッスル
 * @author	Satoshi Nohara
 * @date	07.04.10
 */
//============================================================================================
#ifndef	__FSSC_CASTLE_H__
#define	__FSSC_CASTLE_H__

#include "poketool/pokeparty.h"
#include "poketool/poke_tool.h"
#include "savedata/castle_savedata.h"		//CASTLEDATA
#include "castle_def.h"						//CASTLE_LAP_ENEMY_MAX


//============================================================================================
//
//	extern宣言
//
//============================================================================================
extern BOOL FSSC_CastleWorkAlloc(FSS_TASK * core );
extern BOOL FSSC_CastleWorkInit(FSS_TASK * core );
extern BOOL FSSC_CastleWorkFree(FSS_TASK * core );
extern BOOL FSSC_CastleMineCall(FSS_TASK * core);
extern BOOL FSSC_CastleRankUpCall(FSS_TASK * core);
extern BOOL FSSC_CastleCallGetResult(FSS_TASK * core);
extern BOOL FSSC_CastleBattleCall(FSS_TASK * core);
extern BOOL FSSC_CastleEnemyCall(FSS_TASK * core);
extern BOOL FSSC_CastleBtlBeforePokePartySet(FSS_TASK * core);
extern BOOL FSSC_CastleBtlAfterPokePartySet(FSS_TASK * core);
extern BOOL FSSC_CastleScrWork(FSS_TASK * core);
extern BOOL FSSC_CastleLoseCheck( FSS_TASK * core );
extern BOOL FSSC_CastleSendBuf( FSS_TASK * core );
extern BOOL FSSC_CastleRecvBuf( FSS_TASK * core );
extern BOOL FSSC_CastleParentCheckWait( FSS_TASK * core );
extern BOOL FSSC_CPWinWrite( FSS_TASK * core );
extern BOOL FSSC_CPWinDel( FSS_TASK * core );
extern BOOL FSSC_CPWrite( FSS_TASK * core );
extern BOOL FSSC_CastleGetCP( FSS_TASK * core );
extern BOOL FSSC_CastleSubCP( FSS_TASK * core );
extern BOOL FSSC_CastleAddCP( FSS_TASK * core );
extern BOOL FSSC_CastleTalkMsgAppear(FSS_TASK* core);


#endif	//__FSSC_CASTLE_H__


