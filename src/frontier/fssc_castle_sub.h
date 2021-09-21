//============================================================================================
/**
 * @file	fssc_castle_sub.h
 * @bfief	フロンティアシステムスクリプトコマンドサブ：キャッスル
 * @author	Satoshi Nohara
 * @date	07.07.04
 */
//============================================================================================
#ifndef	__FSSC_CASTLE_SUB_H__
#define	__FSSC_CASTLE_SUB_H__

#include "castle_tool.h"


//============================================================================================
//
//	extern宣言
//
//============================================================================================
extern CASTLE_SCRWORK* CastleScr_WorkAlloc( SAVEDATA* savedata, u16 init, u8 type, u16 pos1, u16 pos2, u16 pos3, u16* work );
extern void CastleScr_WorkInit( CASTLE_SCRWORK* wk, u16 init );
extern void CastleScr_WorkRelease( CASTLE_SCRWORK* wk );
extern void CastleScr_GetResult( CASTLE_SCRWORK* wk, void* castle_call );
extern u16 CastleCall_GetRetWork( void* param, u8 pos );
extern void CastleScr_BtlBeforePartySet( CASTLE_SCRWORK* wk );
extern void CastleScr_BtlAfterPartySet( CASTLE_SCRWORK* wk );
extern void CastleScr_ModeSet( CASTLE_SCRWORK* wk, u8 mode );
extern void CastleScr_BtlWinPokeData( CASTLE_SCRWORK* wk );
extern int CastleScr_GetBtlWinCpPoint( CASTLE_SCRWORK* wk );

extern void CastleScr_SaveRestPlayData( CASTLE_SCRWORK* wk, u8 mode );
//extern void CastleScr_SetNG( CASTLE_SCRWORK* wk );
extern u16 CastleScr_IncRound( CASTLE_SCRWORK* wk );
extern u16 CastleScr_GetRound( CASTLE_SCRWORK* wk );
extern u16 CastleScr_GetEnemyObjCode( CASTLE_SCRWORK* wk, u8 param );
extern void CastleScr_SetLose( CASTLE_SCRWORK* wk );
extern void CastleScr_SetClear( CASTLE_SCRWORK* wk );

//名前＋CPウィンドウ
extern void FSSC_Sub_CPWinWrite( FSS_PTR fss, CASTLE_SCRWORK* bc_scr_wk );
extern void FSSC_Sub_CPWinDel( FSS_PTR fss, CASTLE_SCRWORK* bc_scr_wk );
extern void FSSC_Sub_CPWrite( FSS_PTR fss, CASTLE_SCRWORK* bc_scr_wk );

//通信
extern BOOL CastleScr_CommSetSendBuf( CASTLE_SCRWORK* wk, u16 type, u16 param );
extern u16 CastleScr_CommGetLap( CASTLE_SCRWORK* wk );

extern u16	CastleScr_GetAddBtlPoint( CASTLE_SCRWORK* wk );
extern void CastleScr_AddCP( SAVEDATA* sv, u8 type, int num );


#endif	//__FSSC_CASTLE_SUB_H__


