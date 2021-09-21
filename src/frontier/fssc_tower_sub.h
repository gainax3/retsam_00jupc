//============================================================================================
/**
 * @file	fssc_tower_sub.h
 * @bfief	フロンティアシステムスクリプトコマンドサブ：タワー
 * @author	Satoshi Nohara
 * @date	07.05.28
 */
//============================================================================================
#ifndef	__FSSC_TOWER_SUB_H__
#define	__FSSC_TOWER_SUB_H__

//#include "factory_tool.h"


//============================================================================================
//
//	extern宣言
//
//============================================================================================
extern void FSTowerScr_ChoiceBtlPartner(BTOWER_SCRWORK* wk,SAVEDATA* sv);
extern u16 FSTowerScr_GetEnemyObj(BTOWER_SCRWORK* wk,u16 idx);
extern void FSTowerScr_SetLeaderClearFlag(BTOWER_SCRWORK* wk,u16 mode);
extern u16	FSTowerScr_IncRound(BTOWER_SCRWORK* wk);
extern u16 FSTowerScr_GetRenshouCount(BTOWER_SCRWORK* wk);


#endif	//__FSSC_TOWER_SUB_H__


