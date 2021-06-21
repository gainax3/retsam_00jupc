//==============================================================================
/**
 * @file	wifi_counter_tool.c
 * @brief	Wifiカウンター関連ツール類
 * @author	Satoshi Nohara
 * @date	2008.01.18
 */
//==============================================================================
#include "common.h"
#include "system/procsys.h"
#include "poketool/poke_tool.h"

#include "frontier_types.h"
#include "frontier_tcb_pri.h"
#include "frontier_tool.h"
#include "frontier_def.h"
#include "frontier_map.h"

#include "wifi_counter_tool.h"

#include "savedata/frontier_savedata.h"


//==============================================================================
//
//	定義	
//
//==============================================================================
#define WIFI_COUNTER_LAP_CHAR_XSIZE	(2)							//
#define WIFI_COUNTER_LAP_CHAR_YSIZE	(2)							//
#define WIFI_COUNTER_LAP_SCRN_SIZE	(WIFI_COUNTER_LAP_CHAR_XSIZE * WIFI_COUNTER_LAP_CHAR_YSIZE)
#define WIFI_COUNTER_CHAR_START		(12)						//書き換えるデータのX位置
#define WIFI_COUNTER_CHAR_START2	(14)						//書き換えるデータのY位置
#define WIFI_COUNTER_CHAR_START3	(0x10*6)					//書き換えるデータのあるライン
#define NUM_WRITE_X					(14)						//書き込みX
#define NUM_WRITE_Y					(2)							//書き込みY


//==============================================================================
//
//	構造体
//	
//==============================================================================
//BGアニメーション管理ワーク
typedef struct{
	TCB_PTR tcb;							///<制御タスクへのポインタ
	GF_BGL_INI* bgl;						///<
	u16 no;
	u16 wait;
}WIFI_COUNTER_MONITOR_BG_WORK;

struct _WIFI_COUNTER_MONITOR_SYS{
	GF_BGL_INI* bgl;						///<BGLへのポインタ
	WIFI_COUNTER_MONITOR_BG_WORK* work;
};

#define WIFI_COUNTER_MONITOR_WAIT	(2)		//モニターをアニメさせるウェイト


//==============================================================================
//
//	モニタースクリーンを切り替えてアニメさせる
//	
//==============================================================================
WIFI_COUNTER_MONITOR_SYS* WifiCounterMonitor_Init( GF_BGL_INI* bgl );
static WIFI_COUNTER_MONITOR_BG_WORK* WifiCounterMonitor_TCBAdd( GF_BGL_INI* bgl );
static void WifiCounterMonitor_AnimeControl( TCB_PTR tcb, void* work );
static void WifiCounter_WriteSub( u16* src, u8 flag );

//--------------------------------------------------------------
/**
 * @brief   初期化
 *
 * @param   bgl		BGLへのポインタ
 * @param   pfd		PFDへのポインタ
 *
 * @retval  "ワークのポインタ"
 */
//--------------------------------------------------------------
WIFI_COUNTER_MONITOR_SYS* WifiCounterMonitor_Init( GF_BGL_INI* bgl )
{
	WIFI_COUNTER_MONITOR_SYS* p_sys;
	
	p_sys = sys_AllocMemory( HEAPID_FRONTIERMAP, sizeof(WIFI_COUNTER_MONITOR_SYS) );
	MI_CpuClear8( p_sys, sizeof(WIFI_COUNTER_MONITOR_SYS) );
	
	p_sys->work = WifiCounterMonitor_TCBAdd( bgl );
	return p_sys;
}

//--------------------------------------------------------------
/**
 * @brief   TCB作成
 *
 * @param   bgl				BGLへのポインタ
 *
 * @retval  BGアニメ制御ワークのポインタ
 */
//--------------------------------------------------------------
static WIFI_COUNTER_MONITOR_BG_WORK* WifiCounterMonitor_TCBAdd( GF_BGL_INI* bgl )
{
	WIFI_COUNTER_MONITOR_BG_WORK* wk;
	
	wk = sys_AllocMemory( HEAPID_FRONTIERMAP, sizeof(WIFI_COUNTER_MONITOR_BG_WORK) );
	MI_CpuClear8( wk, sizeof(WIFI_COUNTER_MONITOR_BG_WORK) );
	wk->bgl = bgl;
	wk->no  = 0;
	
	wk->tcb = TCB_Add( WifiCounterMonitor_AnimeControl, wk, TCBPRI_STAGE_BGANIME_CONTROL );
	return wk;
}

//--------------------------------------------------------------
/**
 * @brief   TCBコントロール
 *
 * @param   tcb			TCBへのポインタ
 * @param   work		WIFI_COUNTER_MONITOR_BG_WORK構造体
 */
//--------------------------------------------------------------
static void WifiCounterMonitor_AnimeControl( TCB_PTR tcb, void* work )
{
	u32 i;
	u16 src[WIFI_COUNTER_LAP_SCRN_SIZE];
	WIFI_COUNTER_MONITOR_BG_WORK* wk = work;
	
	//wifi広場だと、2,3ウェイトを交互にしているらしいが、微妙な違いなので、2固定にしてます
	
	if( wk->wait < WIFI_COUNTER_MONITOR_WAIT){
		wk->wait++;
		return;
	}
	wk->wait = 0;
	wk->no ^= 1;

	WifiCounter_WriteSub( src, wk->no );

	GF_BGL_ScrWrite(wk->bgl, FRMAP_FRAME_MAP, src, NUM_WRITE_X, NUM_WRITE_Y, 
					WIFI_COUNTER_LAP_CHAR_XSIZE, WIFI_COUNTER_LAP_CHAR_YSIZE );	//scrn書き込み

	GF_BGL_LoadScreenV_Req( wk->bgl, FRMAP_FRAME_MAP );				//スクリーンデータ転送
	return;
}

//--------------------------------------------------------------
/**
 * @brief	バッファに書き込み
 *
 * @param	src		書き込みバッファ
 * @param	flag	0,1を渡してアニメ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void WifiCounter_WriteSub( u16* src, u8 flag )
{
	u32 i,j,no,start;

	if( flag == 0 ){
		start = WIFI_COUNTER_CHAR_START;
	}else{
		start = WIFI_COUNTER_CHAR_START2;
	}

	for( i=0; i < WIFI_COUNTER_LAP_CHAR_YSIZE ;i++ ){
		for( j=0; j < WIFI_COUNTER_LAP_CHAR_XSIZE ;j++ ){

			//0x10=CHARファイルのYオフセット
			no = (WIFI_COUNTER_CHAR_START3 + i * 0x10) + (start + j);
			//OS_Printf( "no = %d\n", no );
			src[ (i * WIFI_COUNTER_LAP_CHAR_XSIZE) + j ] = no;
		}
	}

	return;
}


//==============================================================================
//
//	モニタースクリーンのアニメ終了
//	
//==============================================================================
void WifiCounterMonitor_End( WIFI_COUNTER_MONITOR_SYS* p_sys );
static void WifiCounterMonitorBGAnime_End( WIFI_COUNTER_MONITOR_BG_WORK* wk );

//--------------------------------------------------------------
/**
 * @brief   終了
 *
 * @param   anmsys		ステージアニメーション管理ワークのポインタ
 */
//--------------------------------------------------------------
void WifiCounterMonitor_End( WIFI_COUNTER_MONITOR_SYS* p_sys )
{
	WifiCounterMonitorBGAnime_End( p_sys->work );
	sys_FreeMemoryEz( p_sys );
	return;
}

//--------------------------------------------------------------
/**
 * @brief   WifiカウンターBGアニメシステム終了
 *
 * @param   sbw		BGアニメ制御ワークのポインタ
 */
//--------------------------------------------------------------
static void WifiCounterMonitorBGAnime_End( WIFI_COUNTER_MONITOR_BG_WORK* wk )
{
	TCB_Delete( wk->tcb );
	sys_FreeMemoryEz( wk );
	return;
}

//--------------------------------------------------------------
/**
 * @brief   WIFI連勝レコードID取得
 *
 * @param   "レコードID"
 */
//--------------------------------------------------------------
u16 WifiCounterRensyouRecordIDGet( u8 bf_no )
{
	u16 id;

	switch( bf_no ){

	case FRONTIER_NO_FACTORY_LV50:
		id = FRID_FACTORY_MULTI_WIFI_RENSHOU_CNT;
		break;

	case FRONTIER_NO_FACTORY_LV100:
		id = FRID_FACTORY_MULTI_WIFI_RENSHOU100_CNT;
		break;

	case FRONTIER_NO_CASTLE:
		id = FRID_CASTLE_MULTI_WIFI_RENSHOU_CNT;
		break;

	case FRONTIER_NO_STAGE:
		id = FRID_STAGE_MULTI_WIFI_RENSHOU_CNT;
		break;

	case FRONTIER_NO_ROULETTE:
		id = FRID_ROULETTE_MULTI_WIFI_RENSHOU_CNT;
		break;

	case FRONTIER_NO_TOWER:
		id = FRID_TOWER_MULTI_WIFI_RENSHOU_CNT;
		break;
	};

	return id;
}

//--------------------------------------------------------------
/**
 * @brief   WIFIクリアID取得
 *
 * @param   "レコードID"
 */
//--------------------------------------------------------------
u16 WifiCounterClearRecordIDGet( u8 bf_no )
{
	u16 id;

	switch( bf_no ){

	case FRONTIER_NO_FACTORY_LV50:
		id = FRID_FACTORY_MULTI_WIFI_CLEAR_BIT;
		break;

	case FRONTIER_NO_FACTORY_LV100:
		id = FRID_FACTORY_MULTI_WIFI_CLEAR100_BIT;
		break;

	case FRONTIER_NO_CASTLE:
		id = FRID_CASTLE_MULTI_WIFI_CLEAR_BIT;
		break;

	case FRONTIER_NO_STAGE:
		id = FRID_STAGE_MULTI_WIFI_CLEAR_BIT;
		break;

	case FRONTIER_NO_ROULETTE:
		id = FRID_ROULETTE_MULTI_WIFI_CLEAR_BIT;
		break;

	case FRONTIER_NO_TOWER:
		id = FRID_TOWER_MULTI_WIFI_CLEAR_BIT;
		break;
	};

	return id;
}


