//==============================================================================
/**
 * @file	wifi_counter_tool.h
 * @brief	wifiカウンター関連ツール類
 * @author	Satoshi Nohara
 * @date	2008.01.18
 */
//==============================================================================
#ifndef __WIFI_COUNTER_TOOL_H__
#define __WIFI_COUNTER_TOOL_H__

#include "common.h"
#include "system/pms_data.h"
#include "savedata/b_tower.h"


//==============================================================================================
//
//	ワークへの不完全型ポインタ
//
//==============================================================================================
typedef	struct _WIFI_COUNTER_MONITOR_SYS WIFI_COUNTER_MONITOR_SYS;


//==============================================================================
//	extern宣言
//==============================================================================
extern WIFI_COUNTER_MONITOR_SYS* WifiCounterMonitor_Init( GF_BGL_INI* bgl );
extern void WifiCounterMonitor_End( WIFI_COUNTER_MONITOR_SYS* p_sys );
extern u16 WifiCounterRensyouRecordIDGet( u8 bf_no );
extern u16 WifiCounterClearRecordIDGet( u8 bf_no );


#endif	//__WIFI_COUNTER_TOOL_H__


