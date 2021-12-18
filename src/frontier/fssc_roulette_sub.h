//============================================================================================
/**
 * @file	fssc_roulette_sub.h
 * @bfief	フロンティアシステムスクリプトコマンドサブ：ルーレット
 * @author	Satoshi Nohara
 * @date	07.09.06
 */
//============================================================================================
#ifndef	__FSSC_ROULETTE_SUB_H__
#define	__FSSC_ROULETTE_SUB_H__

#include "roulette_tool.h"


//============================================================================================
//
//	extern宣言
//
//============================================================================================
extern ROULETTE_SCRWORK* RouletteScr_WorkAlloc( SAVEDATA* savedata, u16 init, u8 type, u16 pos1, u16 pos2, u16 pos3, u16* work );
extern void RouletteScr_WorkInit( ROULETTE_SCRWORK* wk, u16 init );
extern void RouletteScr_WorkRelease( ROULETTE_SCRWORK* wk );
extern void RouletteScr_GetResult( ROULETTE_SCRWORK* wk, void* roulette_call );
extern u16 RouletteCall_GetRetWork( void* param, u8 pos );
extern void RouletteScr_BtlBeforePartySet( ROULETTE_SCRWORK* wk );
extern void RouletteScr_BtlAfterPartySet( ROULETTE_SCRWORK* wk );
extern void RouletteScr_ModeSet( ROULETTE_SCRWORK* wk, u8 mode );
extern int RouletteScr_GetBtlWinPoint( ROULETTE_SCRWORK* wk, POKEPARTY* party, POKEPARTY* pair_party, int turn );

extern void RouletteScr_SaveRestPlayData( ROULETTE_SCRWORK* wk, u8 mode );
//extern void RouletteScr_SetNG( ROULETTE_SCRWORK* wk );
extern u16 RouletteScr_IncRound( ROULETTE_SCRWORK* wk );
extern u16 RouletteScr_GetRound( ROULETTE_SCRWORK* wk );
extern u16 RouletteScr_GetEnemyObjCode( ROULETTE_SCRWORK* wk, u8 param );
extern u16 RouletteScr_GetTrIndex( ROULETTE_SCRWORK* wk, u8 param );

extern void RouletteScr_SetLose( ROULETTE_SCRWORK* wk );
extern void RouletteScr_SetClear( ROULETTE_SCRWORK* wk );
extern void RouletteScr_BGPanelChange( void* p_work, FMAP_PTR fmap );
extern void RouletteScr_MainBGChange( void* p_work, FMAP_PTR fmap );
extern void RouletteScr_AddPokeIcon( ROULETTE_SCRWORK* wk, FMAP_PTR fmap, u16 param1 );
extern void RouletteScr_DelPokeIcon( ROULETTE_SCRWORK* wk, FMAP_PTR fmap, u16 param1 );
extern void RouletteScr_MineChgPokeIconVanish( ROULETTE_SCRWORK* wk, FMAP_PTR fmap, u16 param1, u16 param2 );
extern void RouletteScr_EnemyChgPokeIconVanish( ROULETTE_SCRWORK* wk, FMAP_PTR fmap, u16 param1, u16 param2 );
extern void RouletteScr_AddItemKeepIcon( ROULETTE_SCRWORK* wk, FMAP_PTR fmap, u16 param1 );
extern void RouletteScr_DelItemKeepIcon( ROULETTE_SCRWORK* wk, FMAP_PTR fmap, u16 param1 );
extern void RouletteScr_ChgItemKeepIconVanish( ROULETTE_SCRWORK* wk, FMAP_PTR fmap, u16 param1, u16 param2, u16 param3 );

//名前＋CPウィンドウ
extern void FSSC_Sub_CPWinWrite( FSS_PTR fss, ROULETTE_SCRWORK* bc_scr_wk );
extern void FSSC_Sub_CPWinDel( FSS_PTR fss, ROULETTE_SCRWORK* bc_scr_wk );
extern void FSSC_Sub_CPWrite( FSS_PTR fss, ROULETTE_SCRWORK* bc_scr_wk );

//通信
extern BOOL RouletteScr_CommSetSendBuf( ROULETTE_SCRWORK* wk, u16 type, u16 param );
extern u16 RouletteScr_CommGetLap( ROULETTE_SCRWORK* wk );

//パネルイベント処理
extern void Roulette_EvSet( ROULETTE_SCRWORK* wk, u8 no );

extern u16	RouletteScr_GetAddBtlPoint( ROULETTE_SCRWORK* wk );

extern void RouletteScr_ResColorChange( u8 evy, u8 flag );
extern void RouletteScr_BattleAfterItemSet( POKEPARTY* btl_party, POKEPARTY* party, int btl_pos, int pos );


#endif	//__FSSC_ROULETTE_SUB_H__


