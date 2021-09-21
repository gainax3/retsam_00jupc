//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wifi_p2pmatch_tool.h
 *	@brief		WiFiクラブ以外で任天堂認証サーバにアクセスするときに、WiFiクラブ上で表示されないステータス状態にするシステム
 *	@author		tomoya takahashi
 *	@data		2008.03.28
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WIFI_P2PMATCH_TOOL_H__
#define __WIFI_P2PMATCH_TOOL_H__


#include "savedata/mystatus.h"

// WiFiクラブ以外で任天堂認証サーバにアクセスするときに、
// WiFiクラブ上で表示されないステータス状態にするシステム
//
// Setでクリーンします。クリーンに使用したワークは任天堂サーバとの通信を
// 切断するまで保持してください。
typedef struct _WIFI_MACH_STATUS_tag _WIFI_MACH_STATUS;
extern _WIFI_MACH_STATUS* WifiP2PMatch_CleanStatus_Set( const MYSTATUS* cp_mystatus, u32 heapID );
extern void WifiP2PMatch_CleanStatus_Delete( _WIFI_MACH_STATUS* p_buff );
#endif		// __WIFI_P2PMATCH_TOOL_H__

