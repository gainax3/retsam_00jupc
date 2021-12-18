//============================================================================================
/**
 * @file	fssc_tower.h
 * @bfief	フロンティアシステムスクリプトコマンド：タワー
 * @author	Satoshi Nohara
 * @date	07.05.28
 */
//============================================================================================
#ifndef	__FSSC_TOWER_H__
#define	__FSSC_TOWER_H__


//============================================================================================
//
//	extern宣言
//
//============================================================================================
extern BOOL FSSC_TowerScrWork( FSS_TASK * core );
extern BOOL FSSC_TowerTalkMsgAppear(FSS_TASK* core);
extern BOOL FSSC_TowerWorkRelease(FSS_TASK* core);
extern BOOL FSSC_TowerBattleCall( FSS_TASK * core);
extern BOOL FSSC_TowerCallGetResult( FSS_TASK * core);
extern BOOL FSSC_TowerSendBuf(FSS_TASK* core);
extern BOOL FSSC_TowerRecvBuf(FSS_TASK* core);


#endif	//__FSSC_TOWER_H__


