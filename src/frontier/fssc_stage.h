//============================================================================================
/**
 * @file	fssc_stage.h
 * @bfief	フロンティアシステムスクリプトコマンド：ステージ
 * @author	Satoshi Nohara
 * @date	07.06.07
 */
//============================================================================================
#ifndef	__FSSC_STAGE_H__
#define	__FSSC_STAGE_H__

#include "poketool/pokeparty.h"
#include "poketool/poke_tool.h"
#include "savedata/stage_savedata.h"		//STAGEDATA
#include "stage_def.h"


//============================================================================================
//
//	extern宣言
//
//============================================================================================
extern BOOL FSSC_StageWorkAlloc(FSS_TASK * core );
extern BOOL FSSC_StageWorkEnemySet(FSS_TASK * core );
extern BOOL FSSC_StageWorkFree(FSS_TASK * core );
extern BOOL FSSC_StagePokeTypeSelCall(FSS_TASK * core);
extern BOOL FSSC_StageCallGetResult(FSS_TASK * core);
extern BOOL FSSC_StageBattleCall(FSS_TASK * core);
extern BOOL FSSC_StageScrWork(FSS_TASK * core);
extern BOOL FSSC_StageLoseCheck( FSS_TASK * core );
extern BOOL FSSC_StageSendBuf( FSS_TASK * core );
extern BOOL FSSC_StageRecvBuf( FSS_TASK * core );
extern BOOL FSSC_StageTalkMsgAppear(FSS_TASK* core);
extern BOOL FSSC_StageRenshouCopyExtra( FSS_TASK * core );
extern BOOL FSSC_TVTempStageSet( FSS_TASK * core );


#endif	//__FSSC_STAGE_H__


