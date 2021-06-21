//============================================================================================
/**
 * @file	scr_castle.c
 * @bfief	スクリプトコマンド：バトルキャッスル(受付まわり)
 * @author	Satoshi Nohara
 * @date	07.07.03
 */
//============================================================================================
#include "common.h"

#include <nitro/code16.h> 
#include "system/pm_str.h"
#include "system\msgdata.h"			//MSGMAN_GetString
#include "system\wordset.h"			//WordSet_RegistPlayerName
#include "system/bmp_menu.h"
#include "system/bmp_list.h"
#include "system/get_text.h"
#include "system/lib_pack.h"
#include "poketool/poke_number.h"	//PMNumber_GetMode
#include "savedata/sp_ribbon.h"		//SaveData_GetSpRibbon
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
//バトルキャッスル
#include "savedata/frontier_savedata.h"
#include "scr_castle.h"
#include "scr_castle_sub.h"
#include "savedata/castle_savedata.h"
#include "../frontier/castle_def.h"
//#include "../frontier/comm_command_frontier.h"
#include "comm_command_field.h"
//通信
#include "communication/comm_def.h"
#include "communication/comm_tool.h"
#include "communication/comm_system.h"


//============================================================================================
//
//	構造体宣言
//
//============================================================================================
///ポケモン選択イベントワーク
typedef struct _CASTLE_POKESEL_EVENT{
	int	seq;
	u8	plist_type;							//ポケモンリストタイプ
	u8	pos;								//ポケモンリストの現在カーソル位置
	u8	sel[3];
	void** sp_wk;
}CASTLE_POKESEL_EVENT;

///ポケモン選択イベントシーケンスID
typedef enum{
	CASTLE_POKESEL_PLIST_CALL,				//ポケモンリスト呼び出し
	CASTLE_POKESEL_PLIST_WAIT,				//ポケモンリスト終了待ち
	CASTLE_POKESEL_PST_CALL,					//ポケモンステータス呼び出し
	CASTLE_POKESEL_PST_WAIT,					//ポケモンステータス終了待ち
	CASTLE_POKESEL_EXIT,						//終了
};


//============================================================================================
//
//	プロトタイプ宣言
//
//============================================================================================
BOOL EvCmdBattleCastleTools(VM_MACHINE* core);
BOOL EvCmdBattleCastleSetContinueNG( VM_MACHINE * core );
static void BattleCastleSetNewChallenge( SAVEDATA* sv, CASTLESCORE* wk, u8 type );

//ポケモンリスト＆ポケモンステータス
static void EventCmd_CastlePokeSelectCall( GMEVENT_CONTROL *event, void** buf, u8 plist_type );
static BOOL BtlCastleEv_PokeSelect( GMEVENT_CONTROL *ev );
static int BtlCastle_PokeListCall( CASTLE_POKESEL_EVENT* wk,FIELDSYS_WORK* fsys,int heapID );
static int BtlCastle_PokeListWait( CASTLE_POKESEL_EVENT* wk, FIELDSYS_WORK* fsys );
static int BtlCastle_PokeStatusCall(CASTLE_POKESEL_EVENT* wk,FIELDSYS_WORK* fsys,int heapID );
static int BtlCastle_PokeStatusWait( CASTLE_POKESEL_EVENT* wk, FIELDSYS_WORK* fsys );


//============================================================================================
//
//	コマンド
//
//============================================================================================

//--------------------------------------------------------------
/**
 *	@brief	バトルキャッスル用コマンド群呼び出しインターフェース
 *
 *	@param	com_id		u16:コマンドID
 *	@param	retwk_id	u16:返り値を格納するワークのID
 */
//--------------------------------------------------------------
BOOL EvCmdBattleCastleTools(VM_MACHINE* core)
{
	u8 plist_type;
	u8 buf8[4];
	u16 buf16[4];
	u16	com_id,param,retwk_id;
	u16* ret_wk,*prm_wk;
	void** buf;
	BTOWER_SCRWORK* wk;
	CASTLEDATA* castle_sv;
	CASTLESCORE* score_sv;

	com_id	= VMGetU16(core);
	param	= VMGetWorkValue(core);
	retwk_id= VMGetU16(core);
	ret_wk	= GetEventWorkAdrs( core->fsys, retwk_id );

	castle_sv= SaveData_GetCastleData( core->fsys->savedata );
	score_sv= SaveData_GetCastleScore( core->fsys->savedata );
	buf		= GetEvScriptWorkMemberAdrs( core->fsys, ID_EVSCR_SUBPROC_WORK );

	switch( com_id ){

	//0:参加可能なポケモン数のチェック(アイテムチェックなし)
	case CASTLE_TOOL_CHK_ENTRY_POKE_NUM:
		//*ret_wk = FrontierScrTools_CheckEntryPokeNum( param, core->fsys->savedata );
		*ret_wk = TowerScrTools_CheckEntryPokeNum( param, core->fsys->savedata, 0 );
		break;

	//1:連勝中か取得
	case CASTLE_TOOL_GET_CLEAR_FLAG:
		//WIFIのみ特殊(友達ごとに"クリアしたかフラグ"をもっている)
		if( param == CASTLE_TYPE_WIFI_MULTI ){
			*ret_wk = FrontierRecord_Get(SaveData_GetFrontier(core->fsys->savedata), 
										FRID_CASTLE_MULTI_WIFI_CLEAR_BIT,
										Frontier_GetFriendIndex(FRID_CASTLE_MULTI_WIFI_CLEAR_BIT) );
		}else{
			*ret_wk = (u16)CASTLESCORE_GetScoreData(	score_sv, CASTLESCORE_ID_CLEAR_FLAG, 
														param, 0, NULL);
		}
		break;

	//3:連勝中フラグクリア、連勝レコードクリア、タイプカウントクリア、ランククリア
	case CASTLE_TOOL_SET_NEW_CHALLENGE:
		BattleCastleSetNewChallenge( core->fsys->savedata, score_sv, param );
		break;

	//4:ポケモンリスト画面呼び出し
	case CASTLE_TOOL_SELECT_POKE:

		//バトルタイプによって分岐
		if( param == CASTLE_TYPE_SINGLE ){
			plist_type = PL_TYPE_SINGLE;
		}else if( param == CASTLE_TYPE_DOUBLE ){
			plist_type = PL_TYPE_DOUBLE;
		}else if( param == CASTLE_TYPE_MULTI ){
			plist_type = PL_TYPE_MULTI;
		}else{
			plist_type = PL_TYPE_MULTI;
		}

		OS_Printf( "castle_type = %d\n", param );
		EventCmd_CastlePokeSelectCall( core->event_work, buf, plist_type );
		return 1;

	default:
		OS_Printf( "渡されたcom_id = %d\n", com_id );
		GF_ASSERT( (0) && "com_idが未対応です！" );
		*ret_wk = 0;
		break;
	}

	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		スクリプトコマンド：バトルキャッスル継続NGセット
 * @param		core
 */
//--------------------------------------------------------------------------------------------
BOOL EvCmdBattleCastleSetContinueNG( VM_MACHINE * core )
{
	CASTLESCORE* score_wk;
	u16 type = VMGetWorkValue(core);

	score_wk = SaveData_GetCastleScore( core->fsys->savedata );

	BattleCastleSetNewChallenge( core->fsys->savedata, score_wk, type );
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		連勝中フラグクリア、連勝レコードクリア、タイプカウントクリア、ランククリア
 * @param		core
 */
//--------------------------------------------------------------------------------------------
static void BattleCastleSetNewChallenge( SAVEDATA* sv, CASTLESCORE* wk, u8 type )
{
	int i;
	u16 buf16[4];
	u8 buf8[4];

	//"7連勝(クリア)したかフラグ"のクリアを書き出し
	buf8[0] = 0;
	CASTLESCORE_PutScoreData( wk, CASTLESCORE_ID_CLEAR_FLAG, type, 0, buf8 );

	//WIFIのみ特殊(友達ごとに"クリアしたかフラグ"をもっている)
	if( type == CASTLE_TYPE_WIFI_MULTI ){
		FrontierRecord_Set(	SaveData_GetFrontier(sv), 
							FRID_CASTLE_MULTI_WIFI_CLEAR_BIT,
							Frontier_GetFriendIndex(FRID_CASTLE_MULTI_WIFI_CLEAR_BIT), 0 );
	}

	//"現在の連勝数"を0にする
	FrontierRecord_Set(	SaveData_GetFrontier(sv), 
						CastleScr_GetWinRecordID(type),
						Frontier_GetFriendIndex(CastleScr_GetWinRecordID(type)), 0 );

	//"CP"を0にする
	FrontierRecord_Set(	SaveData_GetFrontier(sv), 
						CastleScr_GetCPRecordID(type),
						Frontier_GetFriendIndex(CastleScr_GetCPRecordID(type)), 0 );
	
	//負けた時のみランクを元に戻す(7連勝の時はランクそのまま引き継ぐ)
	buf16[0] = 1;
	for( i=0; i < CASTLE_RANK_TYPE_MAX ;i++ ){
		FrontierRecord_Set(	SaveData_GetFrontier(sv), 
					CastleScr_GetRankRecordID(type,i),
					Frontier_GetFriendIndex(CastleScr_GetRankRecordID(type,i)), 1 );
	}

	return;
}


//============================================================================================
//
//	通信
//
//============================================================================================
BOOL EvCmdBattleCastleCommMonsNo(VM_MACHINE* core);
static void EventCall_CastleComm( GMEVENT_CONTROL* event, u16 monsno, u16 monsno2, u16* ret_wk );
static BOOL GMEVENT_CastleComm( GMEVENT_CONTROL* event );

//--------------------------------------------------------------------------------------------
/**
 *	@brief	通信マルチデータ送信
 */
//--------------------------------------------------------------------------------------------
BOOL EvCmdBattleCastleCommMonsNo(VM_MACHINE* core)
{
	u16 monsno	= VMGetWorkValue( core );
	u16 monsno2	= VMGetWorkValue( core );
	u16* ret_wk	= VMGetWork( core );

	OS_Printf( "バトルキャッスル通信マルチデータ送信\n" );
	OS_Printf( "ポケモンナンバー monsno = %d\n", monsno );
	OS_Printf( "ポケモンナンバー monsno2= %d\n", monsno2 );
	
	EventCall_CastleComm( core->event_work, monsno, monsno2, ret_wk );
	return 1;
}
	
//--------------------------------------------------------------------------------------------
/**
 * @brief	イベント擬似コマンド：送受信
 *
 * @param	event		イベント制御ワークへのポインタ
 * @param	monsno		ポケモンナンバー
 */
//--------------------------------------------------------------------------------------------
static void EventCall_CastleComm( GMEVENT_CONTROL* event, u16 monsno, u16 monsno2, u16* ret_wk )
{
	FLDSCR_CASTLE_COMM* comm_wk;

	comm_wk = sys_AllocMemory( HEAPID_WORLD, sizeof(FLDSCR_CASTLE_COMM) );
	memset( comm_wk, 0, sizeof(FLDSCR_CASTLE_COMM) );

	comm_wk->mine_monsno[0]	= monsno;
	comm_wk->mine_monsno[1]	= monsno2;
	comm_wk->ret_wk			= ret_wk;
	//CommCommandFrontierInitialize( comm_wk );
    CommCommandFieldInitialize( comm_wk );

	FieldEvent_Call( event, GMEVENT_CastleComm, comm_wk );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	送受信
 *
 * @param	event		GMEVENT_CONTROL型
 *
 * @retval	"FALSE = 実行中"
 * @retval	"TRUE = 実行終了"
 */
//--------------------------------------------------------------------------------------------
static BOOL GMEVENT_CastleComm( GMEVENT_CONTROL* event )
{
	FLDSCR_CASTLE_COMM* comm_wk = FieldEvent_GetSpecialWork( event );

	switch( comm_wk->seq ){

	case 0:
		if( CommSendData(CF_CASTLE_MONSNO,comm_wk,sizeof(FLDSCR_CASTLE_COMM)) == TRUE ){
			comm_wk->seq++;
		}
		break;
		
	case 1:
		if( comm_wk->recieve_count >= CASTLE_COMM_PLAYER_NUM ){

			*comm_wk->ret_wk = 0;

			//出場ポケモンがかぶっているかチェック
			if( (comm_wk->mine_monsno[0] == comm_wk->pair_monsno[0]) ||
				(comm_wk->mine_monsno[0] == comm_wk->pair_monsno[1]) ){
				*comm_wk->ret_wk+=1;
			}

			if( (comm_wk->mine_monsno[1] == comm_wk->pair_monsno[0]) ||
				(comm_wk->mine_monsno[1] == comm_wk->pair_monsno[1]) ){
				*comm_wk->ret_wk+=2;
			}

			OS_Printf( "comm_wk->mine_monsno[0] = %d\n", comm_wk->mine_monsno[0] );
			OS_Printf( "comm_wk->mine_monsno[1] = %d\n", comm_wk->mine_monsno[1] );
			OS_Printf( "comm_wk->pair_monsno[0] = %d\n", comm_wk->pair_monsno[0] );
			OS_Printf( "comm_wk->pair_monsno[1] = %d\n", comm_wk->pair_monsno[1] );
			OS_Printf( "*comm_wk->ret_wk = %d\n", *comm_wk->ret_wk );
			comm_wk->seq++;
		}
		break;

	case 2:
		sys_FreeMemoryEz( comm_wk );
		return TRUE;
	};

	return FALSE;
}


//============================================================================================
//
//	ポケモンリスト＆ポケモンステータス
//
//============================================================================================

//--------------------------------------------------------------
/**
 * @brief	ポケモン選択　サブイベント呼び出し
 *
 * @param	event	GMEVENT_CONTROL*
 *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EventCmd_CastlePokeSelectCall( GMEVENT_CONTROL *event, void** buf, u8 plist_type )
{
	FIELDSYS_WORK* fsys		= FieldEvent_GetFieldSysWork( event );
	CASTLE_POKESEL_EVENT* wk = sys_AllocMemory( HEAPID_WORLD, sizeof(CASTLE_POKESEL_EVENT) );
	MI_CpuClear8( wk, sizeof(CASTLE_POKESEL_EVENT) );

	wk->plist_type	= plist_type;
	wk->sp_wk		= buf;

	FieldEvent_Call( fsys->event, BtlCastleEv_PokeSelect, wk );
	return;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	ゲームイベントコントローラ　ポケモンリスト＆ステータス
 *
 * @param	ev	GMEVENT_CONTROL *
 *
 * @retval	BOOL	TRUE=イベント終了
 */
//--------------------------------------------------------------------------------------------
static BOOL BtlCastleEv_PokeSelect( GMEVENT_CONTROL *ev )
{
	FIELDSYS_WORK* fsys		= FieldEvent_GetFieldSysWork( ev );
	CASTLE_POKESEL_EVENT* wk	= FieldEvent_GetSpecialWork( ev );

	switch( wk->seq ){

	//ポケモンリスト呼び出し
	case CASTLE_POKESEL_PLIST_CALL:
		wk->seq = BtlCastle_PokeListCall( wk, fsys, HEAPID_WORLD );
		break;

	//ポケモンリスト終了待ち
	case CASTLE_POKESEL_PLIST_WAIT:
		wk->seq = BtlCastle_PokeListWait( wk, fsys );
		break;

	//ポケモンステータス呼び出し
	case CASTLE_POKESEL_PST_CALL:
		wk->seq = BtlCastle_PokeStatusCall( wk, fsys, HEAPID_WORLD );
		break;

	//ポケモンステータス終了待ち
	case CASTLE_POKESEL_PST_WAIT:
		wk->seq = BtlCastle_PokeStatusWait( wk, fsys );
		break;

	//終了
	case CASTLE_POKESEL_EXIT:
		sys_FreeMemoryEz( wk );		//CASTLE_POKESEL_EVENTを開放
		return TRUE;
	}

	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 *	@brief	バトルキャッスル　ポケモンリスト呼び出し
 */
//--------------------------------------------------------------------------------------------
static int BtlCastle_PokeListCall( CASTLE_POKESEL_EVENT* wk,FIELDSYS_WORK* fsys,int heapID )
{
	u8 i;

	//PLIST_DATA* pld = PLISTDATA_Create(HEAPID_WORLD, fsys, wk->plist_type, PL_MODE_BATTLE_TOWER);
	PLIST_DATA* pld = sys_AllocMemory( HEAPID_WORLD, sizeof(PLIST_DATA) );
	MI_CpuClearFast( pld, sizeof(PLIST_DATA) );

	//PILSTDATA_Createの中身と同じ
	pld->pp			= SaveData_GetTemotiPokemon( fsys->savedata );
	pld->myitem		= SaveData_GetMyItem( fsys->savedata );
	pld->mailblock	= SaveData_GetMailBlock( fsys->savedata );
	pld->cfg		= SaveData_GetConfig( fsys->savedata );

	//タイプはシングル固定でよさそう(確認する)
	//pld->type		= wk->plist_type;
	pld->type		= PL_TYPE_SINGLE;

	pld->mode		= PL_MODE_BATTLE_CASTLE;
	pld->fsys		= fsys;

	//カーソル位置
	pld->ret_sel	= wk->pos;

	//選択している位置(リスト→ステータス→リストで状態を復帰させる)
	for( i=0; i < 3 ;i++ ){
		pld->in_num[i] = wk->sel[i];
	}

	pld->in_lv		= 100;			//参加レベル
	pld->in_min		= 3;			//参加最小数
	pld->in_max		= 3;			//参加最大数

	if( wk->plist_type == PL_TYPE_MULTI ){
		pld->in_min = 2;			//参加最小数
		pld->in_max = 2;			//参加最大数
	}

	GameSystem_StartSubProc( fsys, &PokeListProcData, pld );

	*(wk->sp_wk) = pld;

	return CASTLE_POKESEL_PLIST_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 *	@brief	ポケモンリスト終了待ち
 */
//--------------------------------------------------------------------------------------------
static int BtlCastle_PokeListWait( CASTLE_POKESEL_EVENT* wk, FIELDSYS_WORK* fsys )
{
	int	ret;
	PLIST_DATA* pld;

	// サブプロセス終了待ち
	if( FieldEvent_Cmd_WaitSubProcEnd(fsys) ) {
		return CASTLE_POKESEL_PLIST_WAIT;
	}

	pld = *(wk->sp_wk);

	//データ取得
	switch( pld->ret_sel ){

	case PL_SEL_POS_EXIT:					//やめる(pldは開放していない)
		return CASTLE_POKESEL_EXIT;

	case PL_SEL_POS_ENTER:					//決定(pldは開放していない)
		return CASTLE_POKESEL_EXIT;

	default:								//つよさをみる
		break;
	}

	//選択している状態を、ポケモンステータスを呼んだあとに復帰させるために必要
	MI_CpuCopy8( pld->in_num, wk->sel, 3 );	//現在選ばれているポケモンを保存

	//ポジションを保存
	wk->pos = pld->ret_sel;

	sys_FreeMemoryEz(pld);
	*(wk->sp_wk) = NULL;
	return	CASTLE_POKESEL_PST_CALL;
}

//--------------------------------------------------------------------------------------------
/**
 *	@brief	バトルキャッスル　ポケモンステータス呼び出し
 */
//--------------------------------------------------------------------------------------------
static int BtlCastle_PokeStatusCall(CASTLE_POKESEL_EVENT* wk,FIELDSYS_WORK* fsys,int heapID )
{
	PSTATUS_DATA* psd;
	SAVEDATA* sv;

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
	
	sv = fsys->savedata;

	//ポケモンステータスを呼び出す
	psd = sys_AllocMemoryLo( heapID, sizeof(PSTATUS_DATA) );
	MI_CpuClear8( psd,sizeof(PSTATUS_DATA) );

	psd->cfg		= SaveData_GetConfig( sv );
	psd->ppd		= SaveData_GetTemotiPokemon( sv );
	psd->zukan_mode	= PMNumber_GetMode( sv );
	psd->ev_contest	= PokeStatus_ContestFlagGet( sv );
	psd->ppt		= PST_PP_TYPE_POKEPARTY;
	psd->pos		= wk->pos;
	psd->max		= (u8)PokeParty_GetPokeCount( psd->ppd );
	psd->waza		= 0;
	psd->mode		= PST_MODE_NORMAL;
	psd->ribbon		= SaveData_GetSpRibbon( sv );

	PokeStatus_PageSet( psd, PST_PageTbl );
	PokeStatus_PlayerSet( psd, SaveData_GetMyStatus(sv) );
	
	GameSystem_StartSubProc( fsys, &PokeStatusProcData, psd );
	
	*(wk->sp_wk) = psd;
	return CASTLE_POKESEL_PST_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 *	@brief	バトルキャッスル　ポケモンステータス終了待ち
 */
//--------------------------------------------------------------------------------------------
static int BtlCastle_PokeStatusWait( CASTLE_POKESEL_EVENT* wk, FIELDSYS_WORK* fsys )
{
	PSTATUS_DATA* psd;

	//サブプロセス終了待ち
	if( FieldEvent_Cmd_WaitSubProcEnd(fsys) ) {
		return CASTLE_POKESEL_PST_WAIT;
	}

	psd = *(wk->sp_wk);
	
	//切り替えられたカレントを保存する
	wk->pos = psd->pos;

	sys_FreeMemoryEz( psd );
	*(wk->sp_wk) = NULL;

	return CASTLE_POKESEL_PLIST_CALL;
}


