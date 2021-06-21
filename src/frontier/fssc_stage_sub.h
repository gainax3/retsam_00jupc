//============================================================================================
/**
 * @file	fssc_stage_sub.h
 * @bfief	フロンティアシステムスクリプトコマンドサブ：ステージ
 * @author	Satoshi Nohara
 * @date	07.06.07
 */
//============================================================================================
#ifndef	__FSSC_STAGE_SUB_H__
#define	__FSSC_STAGE_SUB_H__

#include "stage_tool.h"


//============================================================================================
//
//	extern宣言
//
//============================================================================================
extern STAGE_SCRWORK* StageScr_WorkAlloc( SAVEDATA* savedata, u16 init, u8 type, u8 pos, u8 pos2 );
extern void StageScr_WorkEnemySet( STAGE_SCRWORK* wk, u16 init );
extern void StageScr_WorkRelease( STAGE_SCRWORK* wk );
extern void StageScr_GetResult( STAGE_SCRWORK* wk, void* stage_call );
extern u16 StageCall_GetRetWork( void* param, u8 pos );
extern void StageScr_RentalPartySet( STAGE_SCRWORK* wk );
extern void StageScr_BtlAfterPartySet( STAGE_SCRWORK* wk );
extern void StageScr_TradePokeChange( STAGE_SCRWORK* wk );
extern void StageScr_TradeAfterPartySet( STAGE_SCRWORK* wk );
extern void StageScr_StageRenshouCopyExtra( STAGE_SCRWORK* wk, u16* ret_wk );

//通信
extern BOOL StageScr_CommSetSendBuf( STAGE_SCRWORK* wk, u16 type, u16 param );

extern void StageScr_SaveRestPlayData( STAGE_SCRWORK* wk, u8 mode );
//extern void StageScr_SetNG( STAGE_SCRWORK* wk );
extern u16	StageScr_IncRound( STAGE_SCRWORK* wk );
extern u16	StageScr_GetRound( STAGE_SCRWORK* wk );
extern u16 StageScr_GetEnemyObjCode( STAGE_SCRWORK* wk, u8 param );
extern void StageScr_SetLose( STAGE_SCRWORK* wk );
extern void StageScr_SetClear( STAGE_SCRWORK* wk );

extern u16	StageScr_GetAddBtlPoint( STAGE_SCRWORK* wk );
extern void StageScr_TypeLevelMaxOff( STAGE_SCRWORK* wk );
extern void StageScr_Sqrt( STAGE_SCRWORK* wk );


#endif	//__FSSC_STAGE_SUB_H__


