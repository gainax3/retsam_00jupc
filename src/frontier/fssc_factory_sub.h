//============================================================================================
/**
 * @file	fssc_factory_sub.h
 * @bfief	フロンティアシステムスクリプトコマンドサブ：ファクトリー
 * @author	Satoshi Nohara
 * @date	07.03.29
 */
//============================================================================================
#ifndef	__FSSC_FACTORY_SUB_H__
#define	__FSSC_FACTORY_SUB_H__

#include "factory_tool.h"


//============================================================================================
//
//	extern宣言
//
//============================================================================================
extern FACTORY_SCRWORK* FactoryScr_WorkAlloc( SAVEDATA* savedata, u16 init, u8 type, u8 level );
extern void FactoryScr_WorkInit( FACTORY_SCRWORK* wk, u16 init );
extern void FactoryScr_WorkRelease( FACTORY_SCRWORK* wk );
extern void FactoryScr_GetResult( FACTORY_SCRWORK* wk, void* factory_call );
extern u16 FactoryCall_GetRetWork( void* param, u8 pos );
extern void FactoryScr_RentalPartySet( FACTORY_SCRWORK* wk );
extern void FactoryScr_BtlAfterPartySet( FACTORY_SCRWORK* wk );
extern void FactoryScr_TradePokeChange( FACTORY_SCRWORK* wk );
extern void FactoryScr_TradeAfterPartySet( FACTORY_SCRWORK* wk );
extern void FactoryScr_ModeSet( FACTORY_SCRWORK* wk, u8 mode );

extern void FactoryScr_SaveRestPlayData( FACTORY_SCRWORK* wk, u8 mode );
//extern void FactoryScr_SetNG( FACTORY_SCRWORK* wk );
extern u16 FactoryScr_IncRound( FACTORY_SCRWORK* wk );
extern u16 FactoryScr_GetRound( FACTORY_SCRWORK* wk );
extern u16 FactoryScr_GetEnemyObjCode( FACTORY_SCRWORK* wk, u8 param );
extern void FactoryScr_SetLose( FACTORY_SCRWORK* wk );
extern void FactoryScr_SetClear( FACTORY_SCRWORK* wk );

//通信
extern BOOL FactoryScr_CommSetSendBuf( FACTORY_SCRWORK* wk, u16 type, u16 param );
extern u16 FactoryScr_CommGetLap( FACTORY_SCRWORK* wk );

extern u16	FactoryScr_GetAddBtlPoint( FACTORY_SCRWORK* wk );

extern void FactoryScr_TradeCountInc( FACTORY_SCRWORK* wk );


#endif	//__FSSC_FACTORY_SUB_H__


