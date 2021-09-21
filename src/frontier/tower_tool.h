//==============================================================================
/**
 * @file	tower_tool.h
 * @brief	バトルタワー関連ツール類
 * @author	nohara
 * @date	2007.05.28
 */
//==============================================================================
#ifndef __TOWER_TOOL_H__
#define __TOWER_TOOL_H__

//#include "common.h"
//#include "system/pms_data.h"
//#include "savedata/b_tower.h"
#include "frontier_types.h"
#include "frontier_main.h"


//==============================================================================
//	extern宣言
//==============================================================================
extern BOOL FSRomBattleTowerTrainerDataMake(BTOWER_SCRWORK* wk,B_TOWER_PARTNER_DATA *tr_data,u16 tr_no,int cnt,u16 *set_poke_no,u16 *set_item_no,B_TOWER_PAREPOKE_PARAM* poke,int heapID);
extern void btltower_BtlPartnerSelectWifi(SAVEDATA* sv,B_TOWER_PARTNER_DATA* wk,const u8 round);
extern BATTLE_PARAM* BtlTower_CreateBattleParam( BTOWER_SCRWORK* wk, FRONTIER_EX_PARAM* ex_param );
//extern void BTowerComm_SendPlayerData(BTOWER_SCRWORK* wk,SAVEDATA *sv);
//extern void BTowerComm_SendTrainerData(BTOWER_SCRWORK* wk);
extern void FSBTowerComm_SendRetireSelect(BTOWER_SCRWORK* wk,u16 retire);
//extern u16 BTowerComm_RecvPlayerData(BTOWER_SCRWORK* wk,const u16* recv_buf);
//extern u16 BTowerComm_RecvTrainerData(BTOWER_SCRWORK* wk,const u16* recv_buf);
extern u16 FSBTowerComm_RecvRetireSelect(BTOWER_SCRWORK* wk,const u16* recv_buf);
extern int BtlTower_GetTrMsgArc( u8 play_mode );


#endif	//__TOWER_TOOL_H__


