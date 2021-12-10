//============================================================================================
/**
 * @file	scr_factory.c
 * @bfief	スクリプトコマンド：バトルファクトリー(受付まわり)
 * @author	Satoshi Nohara
 * @date	07.09.18
 */
//============================================================================================
#include "common.h"

#include <nitro/code16.h> 
#include "system/pm_str.h"
#include "system/msgdata.h"			//MSGMAN_GetString
#include "system/wordset.h"			//WordSet_RegistPlayerName
#include "system/bmp_menu.h"
#include "system/bmp_list.h"
#include "system/get_text.h"
#include "system/lib_pack.h"
#include "gflib/strbuf_family.h"	//許可制
//フィールド
#include "fieldsys.h"
#include "field_subproc.h"
//スクリプト
#include "script.h"
#include "scrcmd.h"
#include "scrcmd_def.h"
#include "field/eventflag.h"
#include "field/evwkdef.h"
#include "sysflag.h"
#include "syswork.h"
#include "scr_tool.h"
//バトルファクトリー
#include "savedata/frontier_savedata.h"
#include "scr_factory.h"
//#include "scr_factory_sub.h"
#include "savedata/factory_savedata.h"
#include "../frontier/factory_def.h"
#include "application/factory.h"
#include "../frontier/comm_command_frontier.h"
//通信
#include "communication/comm_def.h"
#include "communication/comm_tool.h"
#include "communication/comm_system.h"


//============================================================================================
//
//	プロトタイプ宣言
//
//============================================================================================
BOOL EvCmdFactorySetContinueNG( VM_MACHINE * core );


//============================================================================================
//
//	コマンド
//
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		スクリプトコマンド：ファクトリー継続NGセット
 * @param		core
 */
//--------------------------------------------------------------------------------------------
BOOL EvCmdFactorySetContinueNG( VM_MACHINE * core )
{
	u32 clear_id;
	u8 buf8[4];
	FACTORYSCORE* score_wk;
	u16 type	= VMGetWorkValue(core);
	u16 level	= VMGetWorkValue(core);

	score_wk = SaveData_GetFactoryScore( core->fsys->savedata );

	//"7連勝(クリア)したかフラグ"のクリアを書き出し
	buf8[0] = 0;
	FACTORYSCORE_PutScoreData( score_wk, FACTORYSCORE_ID_CLEAR_FLAG, 
							(level*FACTORY_TYPE_MAX)+type, buf8 );

	//WIFIのみ特殊
	if( type == FACTORY_TYPE_WIFI_MULTI ){

		if( level == FACTORY_LEVEL_50 ){
			clear_id = FRID_FACTORY_MULTI_WIFI_CLEAR_BIT;
		}else{
			clear_id = FRID_FACTORY_MULTI_WIFI_CLEAR100_BIT;
		}

		FrontierRecord_Set(	SaveData_GetFrontier(core->fsys->savedata), 
							clear_id,
							Frontier_GetFriendIndex(clear_id), 0 );
	}

	//"現在の連勝数"を0にする
	FrontierRecord_Set(	SaveData_GetFrontier(core->fsys->savedata), 
						FactoryScr_GetWinRecordID(level,type),
						Frontier_GetFriendIndex(FactoryScr_GetWinRecordID(level,type)), 0 );

	//"現在の交換回数"を0にする
	FrontierRecord_Set(	SaveData_GetFrontier(core->fsys->savedata), 
						FactoryScr_GetTradeRecordID(level,type),
						Frontier_GetFriendIndex(FactoryScr_GetTradeRecordID(level,type)), 0 );
	return 0;
}


