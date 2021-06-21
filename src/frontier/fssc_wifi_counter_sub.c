//============================================================================================
/**
 * @file	fss_wifi_counter_sub.c
 * @bfief	フロンティアシステムスクリプトコマンドサブ：WiFi受付
 * @author	Satoshi Nohara
 * @date	07.07.31
 */
//============================================================================================
#include "common.h"
#include "savedata/savedata_def.h"
#include "savedata/record.h"
#include "savedata/fnote_mem.h"
#include "system/buflen.h"
#include "savedata/tv_work.h"
#include "poketool/poke_tool.h"
#include "poketool/pokeparty.h"
#include "poketool/poke_number.h"
#include "system/procsys.h"
#include "system/gamedata.h"
#include "gflib/strbuf_family.h"
#include "application/pokelist.h"
#include "../field/fieldsys.h"
#include "savedata/frontier_savedata.h"
#include "savedata/sp_ribbon.h"
#include "field/eventflag.h"
#include "field/evwkdef.h"
#include "../field/sysflag.h"
#include "../field/syswork.h"

#include "frontier_types.h"
#include "frontier_main.h"
#include "frontier_scr.h"
#include "fss_task.h"
#include "fs_usescript.h"
#include "frontier_scrcmd.h"
#include "frontier_scrcmd_sub.h"

#include "frontier_wifi.h"
#include "frontier_def.h"
#include "fssc_wifi_counter_sub.h"

#include "frontier_tool.h"
#include "comm_command_frontier.h"
#include "itemtool/itemsym.h"

#include "communication/comm_system.h"
#include "communication/comm_tool.h"
#include "communication/comm_def.h"


//============================================================================================
//
//	プロトタイプ宣言
//
//============================================================================================
FRWIFI_SCRWORK* FrWiFiCounterScr_WorkAlloc( SAVEDATA* savedata );
void FrWiFiCounterScr_WorkFree( FRWIFI_SCRWORK* wk );
BOOL FrWiFiScr_CommSetSendBuf( FRWIFI_SCRWORK* wk, u16 type, u16 param, u16 param2 );
void FrWiFiCounter_PokeSelect( FMAIN_PTR fmain, FRWIFI_SCRWORK* wk, u16 seq );
static void FrWiFiCounter_PokeListCall( FMAIN_PTR fmain, FRWIFI_SCRWORK* wk, FIELDSYS_WORK* fsys, int heapID );
//static void FrWiFiCounter_PokeListWait( void* parent_work );
static void FrWiFiCounter_PokeListWait( FMAIN_PTR fmain, FRWIFI_SCRWORK* wk, FIELDSYS_WORK* fsys,int heapID);
static void FrWiFiCounter_PokeStatusCall( FMAIN_PTR fmain, FRWIFI_SCRWORK* wk, FIELDSYS_WORK* fsys, int heapID );
//static void FrWiFiCounter_PokeStatusWait( void* parent_work );
static void FrWiFiCounter_PokeStatusWait( FMAIN_PTR fmain, FRWIFI_SCRWORK* wk, FIELDSYS_WORK* fsys, int heapID );


//--------------------------------------------------------------
/**
 * @brief	ワークエリア確保
 *
 * @param	none
 *	
 * ＊かならず FrWiFiCounterScr_WorkRelease()で領域を開放すること
 */
//--------------------------------------------------------------
FRWIFI_SCRWORK* FrWiFiCounterScr_WorkAlloc( SAVEDATA* savedata )
{
	static FRWIFI_SCRWORK* wk;					//あとで確認]]]]]]]]]]]]]]]]]]]]]]]]]

	sys_PrintHeapFreeSize( HEAPID_WORLD );		//特定ヒープのメモリ空き容量合計を表示
	sys_PrintHeapFreeSize( HEAPID_WORLD );		//未開放メモリの情報をプリント

	wk = sys_AllocMemory( HEAPID_WORLD, sizeof(FRWIFI_SCRWORK) );
	MI_CpuClear8( wk, sizeof(FRWIFI_SCRWORK) );

	//セーブデータ取得
	wk->sv = savedata;
	OS_Printf( "savedata = %d\n", savedata );
	OS_Printf( "wk->sv = %d\n", wk->sv );

	return wk;
}

//--------------------------------------------------------------
/**
 * @brief	ワークエリア開放
 *
 * @param	none
 */
//--------------------------------------------------------------
void FrWiFiCounterScr_WorkFree( FRWIFI_SCRWORK* wk )
{
	if( wk == NULL ){
		return;
	}

	sys_FreeMemoryEz( wk );
	wk = NULL;

	sys_PrintHeapFreeSize( HEAPID_WORLD );		//特定ヒープのメモリ空き容量合計を表示
	sys_PrintHeapFreeSize( HEAPID_WORLD );		//未開放メモリの情報をプリント
	return;
}


//==============================================================================================
//
//	通信(CommStart)
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	送信ウェイト　
 *
 * @param	wk			CASTLE_SCRWORK型のポインタ
 * @param	type		送信タイプ
 *
 * @retval	none
 */
//--------------------------------------------------------------
BOOL FrWiFiScr_CommSetSendBuf( FRWIFI_SCRWORK* wk, u16 type, u16 param, u16 param2 )
{
	int ret;

	switch( type ){

	//施設ナンバー
	case FRONTIER_COMM_SEL:
		ret = CommFrWiFiCounterSendBufBFNo( wk );
		break;

	//やめる
	case FRONTIER_COMM_YAMERU:
		ret = CommFrWiFiCounterSendBufRetireFlag( wk, param );
		break;

	//選んだモンスターナンバーを送信
	case FRONTIER_COMM_MONSNO_ITEMNO:
		//posのtemoti_partyからpokeを取得でよいかも
		//monsno1,monsno2,itemno1,itemno2
		ret = CommFrWiFiCounterSendBufMonsNoItemNo( wk, param, param2 );
		break;

	//ステージの記録を消してよいか
	case FRONTIER_COMM_STAGE_RECORD_DEL:
		ret = CommFrWiFiCounterSendBufStageRecordDel( wk, param );
		break;

	//引き続き遊ぶか
	case FRONTIER_COMM_GAME_CONTINUE:
		ret = CommFrWiFiCounterSendBufGameContinue( wk, param );
		break;
	};

	return ret;
}


//============================================================================================
//
//	ポケモンリスト＆ポケモンステータス
//
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief	ポケモンリスト＆ステータス
 *
 * @param	tcb
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
void FrWiFiCounter_PokeSelect( FMAIN_PTR fmain, FRWIFI_SCRWORK* wk, u16 seq )
{
	FIELDSYS_WORK* fsys	= wk->fsys;

	switch( seq ){

	//ポケモンリスト呼び出し
	case FR_WIFI_POKESEL_PLIST_CALL:
		FrWiFiCounter_PokeListCall( fmain, wk, fsys, HEAPID_WORLD );
		break;

	//ポケモンリスト終了チェック
	case FR_WIFI_POKESEL_PLIST_WAIT:
		FrWiFiCounter_PokeListWait( fmain, wk, fsys, HEAPID_WORLD );
		break;

	//ポケモンステータス呼び出し
	case FR_WIFI_POKESEL_PST_CALL:
		FrWiFiCounter_PokeStatusCall( fmain, wk, fsys, HEAPID_WORLD );
		break;

	//ポケモンステータス終了チェック
	case FR_WIFI_POKESEL_PST_WAIT:
		FrWiFiCounter_PokeStatusWait( fmain, wk, fsys, HEAPID_WORLD );
		break;

	//終了
	case FR_WIFI_POKESEL_EXIT:
		return;
	}

	return;
}

//--------------------------------------------------------------------------------------------
/**
 *	@brief	バトルステージ　ポケモンリスト呼び出し
 */
//--------------------------------------------------------------------------------------------
static void FrWiFiCounter_PokeListCall( FMAIN_PTR fmain, FRWIFI_SCRWORK* wk, FIELDSYS_WORK* fsys,int heapID)
{
	u8 i;

	wk->pld = sys_AllocMemory( HEAPID_WORLD, sizeof(PLIST_DATA) );
	MI_CpuClearFast( wk->pld, sizeof(PLIST_DATA) );

	//PLISTDATA_Createの中身と同じ
	wk->pld->pp			= SaveData_GetTemotiPokemon( wk->sv );
	wk->pld->myitem		= SaveData_GetMyItem( wk->sv );
	wk->pld->mailblock	= SaveData_GetMailBlock( wk->sv );

	OS_Printf( "wk->sv = %d\n", wk->sv );
	wk->pld->cfg		= SaveData_GetConfig( wk->sv );
	OS_Printf( "wk->pld->cfg = %d\n", wk->pld->cfg );

	//タイプはシングル固定でよさそう(確認する)
	wk->pld->type		= PL_TYPE_SINGLE;

	OS_Printf( "wk->bf_no = %d\n", wk->bf_no );

	//タワー
	if( wk->bf_no == FRONTIER_NO_TOWER ){
		wk->pld->mode = PL_MODE_BATTLE_TOWER;
		OS_Printf( "wk->pld->mode = PL_MODE_BATTLE_TOWER\n" );

	//ステージ
	}else if( wk->bf_no == FRONTIER_NO_STAGE ){
		wk->pld->mode = PL_MODE_BATTLE_STAGE;
		OS_Printf( "wk->pld->mode = PL_MODE_BATTLE_STAGE\n" );

	//キャッスル
	}else if( wk->bf_no == FRONTIER_NO_CASTLE ){
		wk->pld->mode = PL_MODE_BATTLE_CASTLE;
		OS_Printf( "wk->pld->mode = PL_MODE_BATTLE_CASTLE\n" );

	//ルーレット
	}else if( wk->bf_no == FRONTIER_NO_ROULETTE ){
		wk->pld->mode = PL_MODE_BATTLE_ROULETTE;
		OS_Printf( "wk->pld->mode = PL_MODE_BATTLE_ROULETTE\n" );

	}else{
		GF_ASSERT(0);
	}

	//wk->pld->mode		= wk->mode;
	wk->pld->fsys		= fsys;

	//カーソル位置
	wk->pld->ret_sel	= wk->pos;

	//選択している位置(リスト→ステータス→リストで状態を復帰させる)
	for( i=0; i < WIFI_COUNTER_ENTRY_POKE_MAX ;i++ ){
		wk->pld->in_num[i] = wk->sel[i];
	}

	if( wk->bf_no == FRONTIER_NO_TOWER ){
		wk->pld->in_min = 2;					//参加最小数
		wk->pld->in_max = 2;					//参加最大数
		wk->pld->in_lv	= 100;					//(あとでLV50に制限される)
	}else if( wk->bf_no == FRONTIER_NO_STAGE ){
		wk->pld->in_min = 1;					//参加最小数
		wk->pld->in_max = 1;					//参加最大数
		wk->pld->in_lv	= 30;					//参加レベル30-100
	}else if( wk->bf_no == FRONTIER_NO_CASTLE ){
		wk->pld->in_min = 2;					//参加最小数
		wk->pld->in_max = 2;					//参加最大数
		wk->pld->in_lv	= 100;					//あとでLV50に制限される)
	}else if( wk->bf_no == FRONTIER_NO_ROULETTE ){
		wk->pld->in_min = 2;					//参加最小数
		wk->pld->in_max = 2;					//参加最大数
		wk->pld->in_lv	= 100;					//あとでLV50に制限される)
	}else{
		GF_ASSERT(0);
	}

	//Frontier_SubProcSet(fmain, &PokeListProcData, wk, 
	Frontier_SubProcSet(fmain, &PokeListProcData, wk->pld, 
						//FALSE, FrWiFiCounter_PokeListWait );
						FALSE, NULL );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 *	@brief	ポケモンリスト終了待ち
 */
//--------------------------------------------------------------------------------------------
//static void FrWiFiCounter_PokeListWait( void* parent_work )
static void FrWiFiCounter_PokeListWait( FMAIN_PTR fmain, FRWIFI_SCRWORK* wk, FIELDSYS_WORK* fsys,int heapID)
{
	int	ret;
	//FRWIFI_SCRWORK* wk = parent_work;
	//PLIST_DATA* wk = parent_work;

	//データ取得
	switch( wk->pld->ret_sel ){

	case PL_SEL_POS_EXIT:					//やめる(pldは開放していない)
		*wk->ret_wk = FR_WIFI_POKESEL_EXIT;
		wk->next_seq = FR_WIFI_POKESEL_EXIT;
		return;

	case PL_SEL_POS_ENTER:					//決定(pldは開放していない)
		*wk->ret_wk = FR_WIFI_POKESEL_EXIT;
		wk->next_seq = FR_WIFI_POKESEL_EXIT;
		return;

	default:								//つよさをみる
		break;
	}

	//選択している状態を、ポケモンステータスを呼んだあとに復帰させるために必要
	MI_CpuCopy8( wk->pld->in_num, wk->sel, WIFI_COUNTER_ENTRY_POKE_MAX );//現在選ばれているpoke保存

	//ポジションを保存
	wk->pos = wk->pld->ret_sel;

	sys_FreeMemoryEz(wk->pld);
	wk->pld = NULL;
	*wk->ret_wk = FR_WIFI_POKESEL_PST_CALL;
	wk->next_seq = FR_WIFI_POKESEL_PST_CALL;
	return;
}

//--------------------------------------------------------------------------------------------
/**
 *	@brief	バトルステージ　ポケモンステータス呼び出し
 */
//--------------------------------------------------------------------------------------------
static void FrWiFiCounter_PokeStatusCall( FMAIN_PTR fmain, FRWIFI_SCRWORK* wk, FIELDSYS_WORK* fsys, int heapID )
{
	static const u8 PST_PageTbl[] = {
		PST_PAGE_INFO,			//「ポケモンじょうほう」
		PST_PAGE_MEMO,			//「トレーナーメモ」
		PST_PAGE_PARAM,			//「ポケモンのうりょく」
		PST_PAGE_CONDITION,		//「コンディション」
		PST_PAGE_B_SKILL,		//「たたかうわざ」
		PST_PAGE_C_SKILL,		//「コンテストわざ」
		PST_PAGE_RIBBON,		//「きねんリボン」
		PST_PAGE_RET,			//「もどる」
		PST_PAGE_MAX
	};
	
	//ポケモンステータスを呼び出す
	wk->psd = sys_AllocMemoryLo( heapID, sizeof(PSTATUS_DATA) );
	MI_CpuClear8( wk->psd,sizeof(PSTATUS_DATA) );

	wk->psd->cfg		= SaveData_GetConfig( wk->sv );
	wk->psd->ppd		= SaveData_GetTemotiPokemon( wk->sv );
	wk->psd->zukan_mode	= PMNumber_GetMode( wk->sv );
	wk->psd->ev_contest	= PokeStatus_ContestFlagGet( wk->sv );
	wk->psd->ppt		= PST_PP_TYPE_POKEPARTY;
	wk->psd->pos		= wk->pos;
	wk->psd->max		= (u8)PokeParty_GetPokeCount( wk->psd->ppd );
	wk->psd->waza		= 0;
	wk->psd->mode		= PST_MODE_NORMAL;
	wk->psd->ribbon		= SaveData_GetSpRibbon( wk->sv );

	PokeStatus_PageSet( wk->psd, PST_PageTbl );
	PokeStatus_PlayerSet( wk->psd, SaveData_GetMyStatus(wk->sv) );
	
	//Frontier_SubProcSet(fmain, &PokeStatusProcData, wk, 
	Frontier_SubProcSet(fmain, &PokeStatusProcData, wk->psd, 
						//FALSE, FrWiFiCounter_PokeStatusWait );
						FALSE, NULL );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 *	@brief	バトルステージ　ポケモンステータス終了待ち
 */
//--------------------------------------------------------------------------------------------
//static void FrWiFiCounter_PokeStatusWait( void* parent_work )
static void FrWiFiCounter_PokeStatusWait( FMAIN_PTR fmain, FRWIFI_SCRWORK* wk, FIELDSYS_WORK* fsys, int heapID )
{
	//FRWIFI_SCRWORK* wk = parent_work;

	//切り替えられたカレントを保存する
	wk->pos = wk->psd->pos;

	sys_FreeMemoryEz( wk->psd );
	wk->psd = NULL;

	*wk->ret_wk = FR_WIFI_POKESEL_PLIST_CALL;
	wk->next_seq = FR_WIFI_POKESEL_PLIST_CALL;
	return;
}


