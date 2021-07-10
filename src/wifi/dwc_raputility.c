//=============================================================================
/**
 * @file	dwc_raputility.c
 * @bfief	DWCラッパー。バトルオーバーレイ２層に並列  06.03.25
 * @author	kazuki yoshihara  -> k.ohno
 * @date	06/03/25
 */
//=============================================================================

#include "common.h"
#include "wifi/dwc_rap.h"
#include "communication/communication.h"

// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ALL) imatake 2006/11/10
// ポケモンの言語ごとにWi-Fi設定画面を切り換え
#if (PM_LANG == LANG_JAPAN)
#define WIFI_SETTING_LANGUAGE	DWC_LANGUAGE_JAPANESE
#define WIFI_SETTING_UTILITY	DWC_UTILITY_TOP_MENU_FOR_JPN
#elif (PM_LANG == LANG_ENGLISH)
#define WIFI_SETTING_LANGUAGE	DWC_LANGUAGE_ENGLISH
//#define WIFI_SETTING_UTILITY	DWC_UTILITY_TOP_MENU_FOR_USA
#define WIFI_SETTING_UTILITY	DWC_UTILITY_TOP_MENU_COMMON
#elif (PM_LANG == LANG_FRANCE)
#define WIFI_SETTING_LANGUAGE	DWC_LANGUAGE_FRENCH
#define WIFI_SETTING_UTILITY	DWC_UTILITY_TOP_MENU_FOR_EUR
#elif (PM_LANG == LANG_GERMANY)
#define WIFI_SETTING_LANGUAGE	DWC_LANGUAGE_GERMAN
#define WIFI_SETTING_UTILITY	DWC_UTILITY_TOP_MENU_FOR_EUR
#elif (PM_LANG == LANG_ITALY)
#define WIFI_SETTING_LANGUAGE	DWC_LANGUAGE_ITALIAN
#define WIFI_SETTING_UTILITY	DWC_UTILITY_TOP_MENU_FOR_EUR
#elif (PM_LANG == LANG_SPAIN)
#define WIFI_SETTING_LANGUAGE	DWC_LANGUAGE_SPANISH
#define WIFI_SETTING_UTILITY	DWC_UTILITY_TOP_MENU_FOR_EUR
#endif
// ----------------------------------------------------------------------------

//==============================================================================
/**
 * WiFi接続ユーティリティを呼び出す
 * @param   none
 * @retval  none
 */
//==============================================================================
void mydwc_callUtility( int HeapID )
{
	OSIntrMode old;
	void *work;

	CommVRAMDInitialize(); // 
	// アラームシステムを初期化しておかないとDWC_StartUtility呼出し後にPanicが発生する
	OS_InitTick();
	OS_InitAlarm();


	// 割り込み処理を一旦禁止へ
	old = OS_DisableInterrupts();

    DWC_SetAuthServer( GF_DWC_CONNECTINET_AUTH_TYPE );

	// WiFi設定メニュー呼び出し（終わるまで帰ってこない)
	work = sys_AllocMemory( HeapID, DWC_UTILITY_WORK_SIZE );
	// ----------------------------------------------------------------------------
	// localize_spec_mark(LANG_ALL) imatake 2006/11/10
	// ポケモンの言語ごとにWi-Fi設定画面を切り換え
	(void)DWC_StartUtility( work, WIFI_SETTING_LANGUAGE, WIFI_SETTING_UTILITY );
	// ----------------------------------------------------------------------------
	sys_FreeMemoryEz( work );

	// 禁止した割り込み処理を復帰
	OS_RestoreInterrupts( old );

	//マスター割り込み処理を有効に
	OS_EnableIrq( ); 


}