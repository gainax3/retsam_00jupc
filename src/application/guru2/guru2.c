//******************************************************************************
/**
 * 
 * @file	guru2.c
 * @brief	ぐるぐる交換
 * @author	kagaya
 * @data	05.07.13
 *
 */
//******************************************************************************
#include "guru2_local.h"

#include "../../field/fieldsys.h"
#include "../../field/field_subproc.h"

//==============================================================================
//	define
//==============================================================================
//--------------------------------------------------------------
///	管理シーケンス
//--------------------------------------------------------------
enum
{
	SEQNO_G2P_INIT = 0,
	SEQNO_G2P_RECEIPT,
	SEQNO_G2P_POKE_SELECT,
	SEQNO_G2P_POKE_STATUS,
	SEQNO_G2P_GURU2_GAME,
	SEQNO_G2P_END,
	
	SEQNO_G2P_MAX,
};

//==============================================================================
//	struct
//==============================================================================
//--------------------------------------------------------------
///	typedef GURU2_CALL_WORK
//--------------------------------------------------------------
typedef struct _GURU2_CALL_WORK GURU2_CALL_WORK;

//--------------------------------------------------------------
///	GURU2_CALL_WORK
//--------------------------------------------------------------
struct _GURU2_CALL_WORK
{
	int seq_no;
	int psel_pos;
	GURU2_PARAM param;
	GURU2PROC_WORK *g2p;
	FIELDSYS_WORK *fsys;
	PLIST_DATA *plist;
	PSTATUS_DATA *psd;
};

//==============================================================================
//	proto
//==============================================================================
static BOOL (* const DATA_Guru2ProcTbl[SEQNO_G2P_MAX])(GURU2_CALL_WORK*);
static const PROC_DATA Guru2Receipt_Proc;
static const PROC_DATA Guru2Main_Proc;

//==============================================================================
//	ぐるぐる交換
//==============================================================================
//--------------------------------------------------------------
/**
 * ぐるぐる交換　プロセス　初期化
 * @param	param	GURU2_PARAM
 * @param	heap_id	ヒープID
 * @retval	GURU2PROC_WORK	GURU2PROC_WORK
 */
//--------------------------------------------------------------
GURU2PROC_WORK * Guru2_ProcInit( GURU2_PARAM *param, u32 heap_id )
{
	GURU2PROC_WORK *g2p;
	
	g2p = sys_AllocMemory( heap_id, sizeof(GURU2PROC_WORK) );
	memset( g2p, 0, sizeof(GURU2PROC_WORK) );
	
	//外部データセット
	g2p->param = *param;
	
	//通信初期化
	g2p->g2c = Guru2Comm_WorkInit( g2p, heap_id );
	
	return( g2p );
}

//--------------------------------------------------------------
/**
 * ぐるぐる交換　プロセス　終了
 * @param	g2p		GURU2PROC_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
void Guru2_ProcDelete( GURU2PROC_WORK *g2p )
{
	//ワーク反映
	
	//ユニオンルーム通信へ
	Guru2Comm_CommUnionRoomChange( g2p->g2c );
	
	//通信ワーク削除
	Guru2Comm_WorkDelete( g2p->g2c );
	
	//リソース開放
	sys_FreeMemoryEz( g2p );
}

//==============================================================================
//	パーツ
//==============================================================================
//--------------------------------------------------------------
/**
 * 受付終了　戻り値チェック
 * @param	g2p	GURU2PROC_WORK *
 * @retval	BOOL	TRUE=継続 FALSE=終了
 */
//--------------------------------------------------------------
BOOL Guru2_ReceiptRetCheck( GURU2PROC_WORK *g2p )
{
	return( g2p->receipt_ret );
}

//==============================================================================
//	イベント
//==============================================================================
//--------------------------------------------------------------
/**
 * ぐるぐる交換処理用ワーク作成
 * @param	FIELDSYS_WORK *fsys
 * @retval	void*	
 */
//--------------------------------------------------------------
void * EventCmd_Guru2ProcWorkAlloc( FIELDSYS_WORK *fsys )
{
	GURU2_CALL_WORK *work;
	
	work = sys_AllocMemory( HEAPID_WORLD, sizeof(GURU2_CALL_WORK) );
	memset( work, 0, sizeof(GURU2_CALL_WORK) );
	
	work->fsys = fsys;
	work->param.sv = fsys->savedata;
	work->param.union_view = fsys->union_view;
	work->param.config = SaveData_GetConfig( fsys->savedata );
	work->param.record = SaveData_GetRecord( fsys->savedata );
	work->param.fnote = SaveData_GetFNote( fsys->savedata );
	work->param.win_type = CONFIG_GetWindowType( work->param.config );
	work->param.fsys = fsys;
	return( work );
}

//--------------------------------------------------------------
/**
 * ぐるぐる交換イベント処理
 * @param	wk		EventCmd_Guru2ProcWorkAlloc()の戻り値　終了時開放
 * @retval	BOOL	TRUE=終了
 */
//--------------------------------------------------------------
BOOL EventCmd_Guru2Proc( void *wk )
{
	GURU2_CALL_WORK *g2call = wk;
	
	if( DATA_Guru2ProcTbl[g2call->seq_no](g2call) == TRUE ){
		sys_FreeMemoryEz( g2call );
		return( TRUE );
	}
	
	return( FALSE );
}

//==============================================================================
//	ぐるぐる交換　プロセス
//==============================================================================
//--------------------------------------------------------------
/**
 * ぐるぐる交換　初期化
 * @param	g2call	GURU2_CALL_WORK
 * @retval	BOOL	TRUE=終了
 */
//--------------------------------------------------------------
static BOOL Guru2Proc_Init( GURU2_CALL_WORK *g2call )
{
	g2call->g2p = Guru2_ProcInit( &g2call->param, HEAPID_WORLD );
	g2call->seq_no = SEQNO_G2P_RECEIPT;
	GameSystem_StartSubProc( g2call->fsys, &Guru2Receipt_Proc, g2call->g2p );
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * ぐるぐる交換　参加者受付
 * @param	g2c	GURU2_CALL_WORK
 * @retval	BOOL	TRUE=終了
 */
//--------------------------------------------------------------
static BOOL Guru2Proc_Receipt( GURU2_CALL_WORK *g2call )
{
	if( FieldEvent_Cmd_WaitSubProcEnd(g2call->fsys) == 0 ){
		if( Guru2_ReceiptRetCheck(g2call->g2p) == FALSE ){
			g2call->seq_no = SEQNO_G2P_END;
		}else{
			CommStateSetErrorCheck( TRUE, TRUE ); //切断不可
			
			g2call->plist =
				Guru2ListEvent_SetProc( g2call->fsys, g2call->psel_pos );
			g2call->g2p->guru2_mode = GURU2MODE_POKESEL;
			g2call->seq_no = SEQNO_G2P_POKE_SELECT;
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * ぐるぐる交換　ポケモン選択
 * @param	g2call	GURU2_CALL_WORK
 * @retval	BOOL	TRUE=終了
 */
//--------------------------------------------------------------
static BOOL Guru2Proc_PokeSelect( GURU2_CALL_WORK *g2call )
{
	if( FieldEvent_Cmd_WaitSubProcEnd(g2call->fsys) == 0 ){
		int ret = g2call->plist->ret_sel;
		sys_FreeMemoryEz( g2call->plist );
		
		if( g2call->plist->ret_mode == PL_RET_STATUS ){ //ステータス閲覧
			g2call->psd = PSTATUS_Temoti_Create(
				g2call->fsys, HEAPID_BASE_APP, PST_MODE_NORMAL );
			g2call->psel_pos = ret;
			g2call->psd->pos = ret;
			FieldPokeStatus_SetProc( g2call->fsys, g2call->psd );	
			g2call->seq_no = SEQNO_G2P_POKE_STATUS;
		}else{											//ゲームへ
			g2call->g2p->trade_no = ret;
			GameSystem_StartSubProc(
				g2call->fsys, &Guru2Main_Proc, g2call->g2p );
			g2call->g2p->guru2_mode = GURU2MODE_GAME_MAIN;
			g2call->seq_no = SEQNO_G2P_GURU2_GAME;
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * ぐるぐる交換　ポケモンステータス閲覧
 * @param	g2call	GURU2_CALL_WORK
 * @retval	BOOL	TRUE=終了
 */
//--------------------------------------------------------------
static BOOL Guru2Proc_PokeStatus( GURU2_CALL_WORK *g2call )
{
	if( FieldEvent_Cmd_WaitSubProcEnd(g2call->fsys) == 0 ){
		sys_FreeMemoryEz( g2call->psd );
		g2call->plist =
			Guru2ListEvent_SetProc( g2call->fsys, g2call->psel_pos );
		g2call->seq_no = SEQNO_G2P_POKE_SELECT;
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * ぐるぐる交換　ぐるぐる交換ゲーム
 * @param	g2call	GURU2_CALL_WORK
 * @retval	BOOL	TRUE=終了
 */
//--------------------------------------------------------------
static BOOL Guru2Proc_Guru2Game( GURU2_CALL_WORK *g2call )
{
	if( FieldEvent_Cmd_WaitSubProcEnd(g2call->fsys) == 0 ){
		g2call->seq_no = SEQNO_G2P_END;
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * ぐるぐる交換　終了
 * @param	g2call	GURU2_CALL_WORK
 * @retval	BOOL	TRUE=終了
 */
//--------------------------------------------------------------
static BOOL Guru2Proc_End( GURU2_CALL_WORK *g2call )
{
	Guru2_ProcDelete( g2call->g2p );
	return( TRUE );
}

//--------------------------------------------------------------
/// ぐるぐる交換　プロセスまとめ
//--------------------------------------------------------------
static BOOL (* const DATA_Guru2ProcTbl[SEQNO_G2P_MAX])(GURU2_CALL_WORK*) =
{
	Guru2Proc_Init,
	Guru2Proc_Receipt,
	Guru2Proc_PokeSelect,
	Guru2Proc_PokeStatus,
	Guru2Proc_Guru2Game,
	Guru2Proc_End,
};

//==============================================================================
//	data
//==============================================================================
// オーバーレイID宣言
FS_EXTERN_OVERLAY(guru2);

//--------------------------------------------------------------
///	ぐるぐる受付プロセスデータ
//--------------------------------------------------------------
static const PROC_DATA Guru2Receipt_Proc = {
	Guru2Receipt_Init,
	Guru2Receipt_Main,
	Guru2Receipt_End,
	FS_OVERLAY_ID(guru2),
};

//--------------------------------------------------------------
///	ぐるぐるメインプロセスデータ
//--------------------------------------------------------------
static const PROC_DATA Guru2Main_Proc = {
	Guru2Main_Init,
	Guru2Main_Main,
	Guru2Main_End,
	FS_OVERLAY_ID(guru2),
};

//==============================================================================
//	old
//==============================================================================
#if 0	//old
BOOL EventCmd_Guru2Proc( void *wk )
{
	GURU2_CALL_WORK *work = wk;
	FIELDSYS_WORK *fsys = work->fsys;
	
	switch( work->seq_no ){
	case SEQNO_G2P_INIT:	//初期化
		work->g2p = Guru2_ProcInit( &work->param, HEAPID_WORLD );
		work->seq_no = SEQNO_G2P_RECEIPT;
		
		GameSystem_StartSubProc( fsys, &Guru2Receipt_Proc, work->g2p );
		break;
	case SEQNO_G2P_RECEIPT:	//受付
		if( FieldEvent_Cmd_WaitSubProcEnd(fsys) ){
			break;
		}
		
		if( Guru2_ReceiptRetCheck(work->g2p) == FALSE ){
			work->seq_no = SEQNO_G2P_END;
			break;
		}
		
		work->plist = Guru2ListEvent_SetProc( fsys );
		work->g2p->guru2_mode = GURU2MODE_POKESEL;
		work->seq_no = SEQNO_G2P_POKE_SELECT;
		break;
	case SEQNO_G2P_POKE_SELECT: //ポケモン選択
		if( FieldEvent_Cmd_WaitSubProcEnd(fsys) ){
			break;
		}
		
		{
			int ret = work->plist->ret_sel;
			
			sys_FreeMemoryEz( work->plist );		//リストワーク開放
			
			if( ret == PL_SEL_POS_EXIT ){			//キャンセル
				work->seq_no = SEQNO_G2P_CANCEL;
				break;
			}
			
			if( work->plist->ret_mode == PL_RET_STATUS ){	//ステータス閲覧
				work->psd = PSTATUS_Temoti_Create(
					fsys, HEAPID_BASE_APP, PST_MODE_NORMAL );
				work->psd->pos = ret;
				FieldPokeStatus_SetProc( fsys, work->psd );	
				work->seq_no = SEQNO_G2P_STATUS;
				break;
			}
			
			work->g2p->trade_no = ret;
		}
		
		GameSystem_StartSubProc( fsys, &Guru2Main_Proc, work->g2p );
		work->g2p->guru2_mode = GURU2MODE_GAME_MAIN;
		work->seq_no = SEQNO_G2P_MAIN;
		break;
	case SEQNO_G2P_STATUS:	//ポケモンステータス
		if( FieldEvent_Cmd_WaitSubProcEnd(fsys) ){
			break;
		}
		
		sys_FreeMemoryEz( work->psd );
		work->plist = Guru2ListEvent_SetProc( fsys );
		work->seq_no = SEQNO_G2P_POKE_SELECT;
		break;
	case SEQNO_G2P_MAIN:	//ぐるぐるメイン
		if( FieldEvent_Cmd_WaitSubProcEnd(fsys) ){
			break;
		}
		
		work->seq_no = SEQNO_G2P_END;
		break;
	case SEQNO_G2P_CANCEL:	//キャンセル
		if( CommGetCurrentID() == 0 ){	//親
			if( Guru2Comm_PokeSelCancelCheck(work->g2p->g2c) ){ //全キャンセル
				work->seq_no = SEQNO_G2P_END;
			}else{
				u16 bit = G2COMM_GMSBIT_CANCEL;
				Guru2Comm_SendData(
					work->g2p->g2c, G2COMM_GM_SIGNAL, &bit, 2 );
				work->g2p->guru2_mode = GURU2MODE_POKESEL_CANCEL;
				work->seq_no = SEQNO_G2P_OYA_CANCEL_START;
			}
		}else{
			int oya_end = work->g2p->g2c->comm_psel_oya_end_flag;
			
			if( oya_end == G2C_OYA_END_FLAG_NON ){
				Guru2Comm_SendData(
					work->g2p->g2c, G2COMM_GM_CANCEL, NULL, 0 );
				work->g2p->guru2_mode = GURU2MODE_POKESEL_CANCEL;
				work->seq_no = SEQNO_G2P_END;
				break;
			}
			
			if( oya_end == G2C_OYA_END_FLAG_EXE ){	//親切断実行中
				break;
			}
			
			work->seq_no = SEQNO_G2P_END;	//親切断済み
		}
		
		break;
	case SEQNO_G2P_OYA_CANCEL_START:		//親キャンセル相手受け取り待ち
		CommTimingSyncStart( COMM_GURU2_TIMINGSYNC_NO );
		work->seq_no = SEQNO_G2P_OYA_CANCEL_TIMING_WAIT;
		break;
	case SEQNO_G2P_OYA_CANCEL_TIMING_WAIT:	//親キャンセル通信終了待ち
		if( CommIsTimingSync(COMM_GURU2_TIMINGSYNC_NO) ){
			CommStateSetErrorCheck( FALSE, FALSE ); //切断可能に
			CommStateSetLimitNum(1);
			CommStateUnionBconCollectionRestart();
			work->seq_no = SEQNO_G2P_CANCEL_CONNECT_CHECK;
		}
		break;
	case SEQNO_G2P_CANCEL_CONNECT_CHECK:
		if( CommGetConnectNum() > 1 ){
			break;
		}
	case SEQNO_G2P_END:		//終了
		Guru2_ProcDelete( work->g2p );
		sys_FreeMemoryEz( wk );
		return( TRUE );
	}
	
	return( FALSE );
}
#endif

#if 0
//--------------------------------------------------------------
///	GURU2_CONNECT_WORK
//--------------------------------------------------------------
#if 0
typedef struct
{
	u16 proc_no;
	u16 seq_no;
	u8 connect_num;
	u8 state;
	TCB_PTR tcb_p;
	GURU2_CALL_WORK *g2call;
}GURU2_CONNECT_WORK;
#endif

#if 0
static void Guru2_ConnectProcInit( GURU2_CALL_WORK *g2call );
static void Guru2_ConnectProcDelete( GURU2_CALL_WORK *g2call );
static void Guru2_ConnectProcTCB( TCB_PTR tcb, void *wk );
static void Guru2_ConnectProc( GURU2_CALL_WORK *g2call );
static void (* const DATA_Guru2ConnectProcTbl[GURU2CONNECTPROC_MAX])(
		GURU2_CONNECT_WORK * );
#endif

//	ぐるぐる交換　ポケモン選びから発生する通信処理
//--------------------------------------------------------------
/**
 * ぐるぐる通信処理　ステータス取得
 * @param	g2c	GURU2_CALL_WORK
 * @retval
 */
//--------------------------------------------------------------
int Guru2_ConnectStateGet( GURU2_CALL_WORK *g2call )
{
	return( g2call->state );
}

//--------------------------------------------------------------
/**
 * ぐるぐる通信処理用ワーク初期化
 * @param	g2call	GURU2_CALL_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Guru2_ConnectProcInit( GURU2_CALL_WORK *g2call )
{
	GURU2_CONNECT_WORK *work = &g2call->g2con;
	memset( work, 0, sizeof(GURU2_CONNECT_WORK) );
	work->g2call = g2call;
	work->tcb_p = TCB_Add( Guru2_ConnectProcTCB, g2call, 0 );
}

//--------------------------------------------------------------
/**
 * ぐるぐる通信処理用ワーク削除
 * @param	g2c	GURU2_CALL_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Guru2_ConnectProcDelete( GURU2_CALL_WORK *g2call )
{
	GURU2_CONNECT_WORK *work = &g2call->g2con;
	if( work->tcb_p != NULL ){ TCB_Delete( work->tcb_p ); }
}

//--------------------------------------------------------------
/**
 * ぐるぐる通信処理　TCB
 * @param	tcb	TCB_PTR
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Guru2_ConnectProcTCB( TCB_PTR tcb, void *wk )
{
	GURU2_CALL_WORK *g2call = wk;
	Guru2_ConnectProc( g2call );
}

//--------------------------------------------------------------
/**
 * ぐるぐる通信処理
 * @param	g2call GURU2_CALL_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Guru2_ConnectProc( GURU2_CALL_WORK *g2call )
{
	GURU2_CONNECT_WORK *work = &g2call->g2con;
	DATA_Guru2ConnectProcTbl[work->proc_no]( work );
}

//--------------------------------------------------------------
/**
 * ぐるぐる通信　親子共通　
 * @param	work	GURU2_CONNECT_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Guru2ConnectProc_BeConnect( GURU2_CONNECT_WORK *work )
{
}

//--------------------------------------------------------------
/**
 * ぐるぐる通信　親子共通　同期シーケンス
 * @param	work	GURU2_CONNECT_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Guru2ConnectProc_Timing( GURU2_CONNECT_WORK *work )
{
	switch( work->seq_no ){
	case 0:	//同期開始
		CommTimingSyncStart( COMM_GURU2_TIMINGSYNC_NO );
		work->state = GURU2CONNECT_TIMING;
		work->seq_no++;
		break;
	case 1:	//同期待ち
		if( CommIsTimingSync(COMM_GURU2_TIMINGSYNC_NO) ){
			work->seq_no++;
		}
		break;
	case 2:	//指定接続数になるまで待ち
		if( work->connect_num >= CommGetConnectNum() ){
			work->state = GURU2CONNECT_ON;
			work->proc_no = GURU2CONNECTPROC_NON;
		}
		break;
	}
}

//--------------------------------------------------------------
/**
 * ぐるぐる通信　切断シーケンス
 * @param	work	GURU2_CONNECT_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
static void Guru2ConnectProc_Cut( GURU2_CONNECT_WORK *work )
{
	switch( work->seq_no ){
	case 0:	//同期開始
		CommTimingSyncStart( COMM_GURU2_TIMINGSYNC_NO );
		work->state = GURU2CONNECT_CUT_EXE;
		work->seq_no++;
		break;
	case 1:	//同期待ち
		if( CommIsTimingSync(COMM_GURU2_TIMINGSYNC_NO) ){
			CommStateSetErrorCheck( FALSE, FALSE );		//切断可能に
			CommStateSetLimitNum(1);
			CommStateUnionBconCollectionRestart();
			work->seq_no++;
		}
		break;
	case 2:	//自身のみになるまで待ち
		if( CommGetConnectNum() <= 1 ){
			work->state = GURU2CONNECT_CUT;
			work->proc_no = GURU2CONNECTPROC_NON;
		}
		break;
	}
}

//--------------------------------------------------------------
///	ぐるぐる通信プロセステーブル
//--------------------------------------------------------------
static void (* const DATA_Guru2ConnectProcTbl[GURU2CONNECTPROC_MAX])(
		GURU2_CONNECT_WORK * ) =
{
	Guru2ConnectProc_Non,
	Guru2ConnectProc_Timing,
	Guru2ConnectProc_Cut,
};
#endif	//old if 0 

