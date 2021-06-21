//============================================================================================
/**
 * @file	wifi_p2pevent.c
 * @bfief	WIFIP2Pイベント制御
 * @author	k.ohno
 * @date	06.04.14
 */
//============================================================================================

#include "common.h"

#include "communication/communication.h"
#include "system/wipe.h"
#include "system/brightness.h"
#include "field/field_common.h"
#include "field/field_event.h"
#include "field/fieldsys.h"
#include "field/ev_mapchange.h"
#include "field/field_encount.h"
#include "field/sysflag.h"

#include "application/wifi_p2pmatch.h"
#include "field/fld_nmixer.h"

#include "application/wifi_battleroom.h"
#include "application/balance_ball.h"

#include "wifi/dwc_rap.h"
#include "savedata/wifilist.h"
#include "savedata/record.h"

#include "wifi_p2pmatchfour.h"
#include "application/bucket.h"
#include "system/pm_overlay.h"
#include "application/balloon.h"

#include "frontier/frontier_types.h"
#include "frontier/frontier_main.h"
#include "frontier/frontier_wifi.h"

extern void DwcUtilOverlayStart(void);



FS_EXTERN_OVERLAY(wifi_p2pmatch);
// プロセス定義データ
static const PROC_DATA WifiP2PMatchProcData = {
	WifiP2PMatchProc_Init,
	WifiP2PMatchProc_Main,
	WifiP2PMatchProc_End,
	FS_OVERLAY_ID(wifi_p2pmatch),
};

static const PROC_DATA WifiP2PMatchFourProcData = {
	WifiP2PMatchFourProc_Init,
	WifiP2PMatchFourProc_Main,
	WifiP2PMatchFourProc_End,
	FS_OVERLAY_ID(wifi_p2pmatch),
};

typedef struct{
    WIFIP2PMATCH_PROC_PARAM* pMatchParam;
	int seq;
    u16* ret;
    u8 lvLimit;
    u8 bSingle;
    void* work;		// wifiPofin用ワーク
	u32 vchat;
}EV_P2PEVENT_WORK;

    enum{
        P2P_INIT,
        P2P_MATCH_BOARD_INIT,
		P2P_MATCH_BOARD,
		P2P_SELECT,
		P2P_BATTLE,
        P2P_BATTLE_END,
		P2P_TRADE,
        P2P_TRADE_END,
        P2P_UTIL,
		P2P_EXIT,
        P2P_FREE,
        P2P_SETEND,
        P2P_POFIN_WAIT,
        P2P_POFIN_WAIT_END,
        P2P_POFIN,
        P2P_POFIN_END,
		P2P_FRONTIER,
		P2P_FRONTIER_END,
		
#ifdef WFP2P_DEBUG_EXON
        P2P_BATTLEROOM,
        P2P_BATTLEROOM_END,
#endif
        P2P_BUCKET_WAIT,
        P2P_BUCKET_WAIT_END,
        P2P_BUCKET,
        P2P_BUCKET_END,

        P2P_BALANCE_BALL_WAIT,
        P2P_BALANCE_BALL_WAIT_END,
        P2P_BALANCE_BALL,
        P2P_BALANCE_BALL_END,

        P2P_BALLOON_WAIT,
        P2P_BALLOON_WAIT_END,
        P2P_BALLOON,
        P2P_BALLOON_END,

        P2P_NOP
	};


// WiFiP2PMatrch ４人接続画面人数制限定義
static const u8 sc_P2P_FOUR_MATCH_MAX[ WFP2PMF_TYPE_NUM ] = {
	3, 4, 4, 4
};

static BOOL GMEVENT_Sub_P2PMatch(GMEVENT_CONTROL * event);



//-------------------------------------
///	バトルルームの実験のためプロセス変更関数を作成
static void P2P_FourWaitInit( EV_P2PEVENT_WORK* p_wk, FIELDSYS_WORK* fsys, u32 heapID, u32 type );
static u32 P2P_FourWaitEnd( EV_P2PEVENT_WORK* p_wk );

//  たまいれ設定
static BUCKET_PROC_WORK* BCT_ProcSet( FIELDSYS_WORK* fsys, u32 heapID, u32 vchat );
static void BCT_ProcEnd( BUCKET_PROC_WORK* p_wk );

// たまのり設定
static BB_PROC_WORK* BB_ProcSet( FIELDSYS_WORK* fsys, u32 heapID, u32 vchat );
static void BB_ProcEnd( BB_PROC_WORK* p_wk );

// ふうせんわり設定
static BALLOON_PROC_WORK* BL_ProcSet( FIELDSYS_WORK* fsys, u32 heapID, u32 vchat );
static void BL_ProcEnd( BUCKET_PROC_WORK* p_wk );

// レコード	ミニゲーム遊んだスコア設定
static void P2P_Record_Minigame( FIELDSYS_WORK* fsys );


#ifdef WFP2P_DEBUG
static WIFI_BATTLEROOM* WBR_ProcSet( FIELDSYS_WORK* fsys, u32 heapID );
static void WBR_ProcEnd( WIFI_BATTLEROOM* p_wk );

#endif

//==============================================================================
//	WIFI通信入り口
//==============================================================================
static BOOL GMEVENT_Sub_P2PMatch(GMEVENT_CONTROL * event)
{
	int len;
	FIELDSYS_WORK * fsys = FieldEvent_GetFieldSysWork(event);
	EV_P2PEVENT_WORK *ep2p = FieldEvent_GetSpecialWork(event);

		
    switch (ep2p->seq) {
      case P2P_INIT:
        ep2p->pMatchParam->pSaveData = fsys->savedata;
//		EventCmd_FinishFieldMap(event);
  //      ep2p->seq ++;
  //      break;
      case P2P_MATCH_BOARD_INIT:
        ep2p->seq++;
        if(ep2p->pMatchParam->seq == WIFI_P2PMATCH_DPW){
            if( mydwc_checkMyGSID(fsys->savedata) ){
                ep2p->seq = P2P_FREE;  //コード取得済みの場合なにもしない
                *(ep2p->ret) = 0;
            }
        }
		break;
      case P2P_MATCH_BOARD:
        EventCmd_CallSubProc(event, &WifiP2PMatchProcData, ep2p->pMatchParam);
        ep2p->seq ++;
		break;
      case P2P_SELECT:
        if( mydwc_checkMyGSID(fsys->savedata) ){  // コード取得に成功
            SysFlag_WifiUseSet(SaveData_GetEventWork(fsys->savedata));
        }
        switch(ep2p->pMatchParam->seq){
          case WIFI_P2PMATCH_SBATTLE_FREE:   // 通信対戦呼び出し
            ep2p->lvLimit = 0;
            ep2p->bSingle = WIFI_BATTLEFLAG_SINGLE;
            ep2p->seq = P2P_BATTLE;
            break;
          case WIFI_P2PMATCH_SBATTLE50:   // 通信対戦呼び出し
            ep2p->lvLimit = 50;
            ep2p->bSingle = WIFI_BATTLEFLAG_SINGLE;
            ep2p->seq = P2P_BATTLE;
            break;
          case WIFI_P2PMATCH_SBATTLE100:   // 通信対戦呼び出し
            ep2p->lvLimit = 100;
            ep2p->bSingle = WIFI_BATTLEFLAG_SINGLE;
            ep2p->seq = P2P_BATTLE;
            break;
          case WIFI_P2PMATCH_DBATTLE_FREE:   // 通信対戦呼び出し
            ep2p->lvLimit = 0;
            ep2p->bSingle = WIFI_BATTLEFLAG_DOUBLE;
            ep2p->seq = P2P_BATTLE;
            break;
          case WIFI_P2PMATCH_DBATTLE50:   // 通信対戦呼び出し
            ep2p->lvLimit = 50;
            ep2p->bSingle = WIFI_BATTLEFLAG_DOUBLE;
            ep2p->seq = P2P_BATTLE;
            break;
          case WIFI_P2PMATCH_DBATTLE100:   // 通信対戦呼び出し
            ep2p->lvLimit = 100;
            ep2p->bSingle = WIFI_BATTLEFLAG_DOUBLE;
            ep2p->seq = P2P_BATTLE;
            break;
          case WIFI_P2PMATCH_TRADE:   // ポケモントレード呼び出し
            ep2p->seq = P2P_TRADE;
            break;
          case WIFI_P2PMATCH_DPW_END:  //DPWへいく場合
            *(ep2p->ret) = 1;
            ep2p->seq = P2P_SETEND;
            break;
          case WIFI_P2PMATCH_END:  // 通信切断してます。終了します。
            ep2p->seq = P2P_EXIT;
            break;
          case WIFI_P2PMATCH_POFIN:   // ポフィン料理呼び出し
            ep2p->seq = P2P_POFIN_WAIT;
            break;
          case WIFI_P2PMATCH_FRONTIER:   // フロンティア呼び出し
            ep2p->seq = P2P_FRONTIER;
            break;
#ifdef WFP2P_DEBUG_EXON
          case WIFI_P2PMATCH_BATTLEROOM:   // ポフィン料理呼び出し
            ep2p->seq = P2P_BATTLEROOM;
            break;
		  case WIFI_P2PMATCH_MBATTLE_FREE:
            ep2p->lvLimit = 0;
            ep2p->bSingle = WIFI_BATTLEFLAG_MULTI;
            ep2p->seq = P2P_BATTLE;
			break;
#endif
          case WIFI_P2PMATCH_BUCKET:   // バケット呼び出し
            ep2p->seq = P2P_BUCKET_WAIT;
			break;
          case WIFI_P2PMATCH_BALANCEBALL:   // 玉乗り呼び出し
            ep2p->seq = P2P_BALANCE_BALL_WAIT;
			break;
          case WIFI_P2PMATCH_BALLOON:   // ふうせん呼び出し
            ep2p->seq = P2P_BALLOON_WAIT;
			break;
          case WIFI_P2PMATCH_UTIL:  //WIFIUTILへ飛びます
            ep2p->seq = P2P_UTIL;
            break;
        }
		break;
	case P2P_BATTLE:
        EventCmd_WifiBattle(event, ep2p->pMatchParam->targetID, ep2p->lvLimit, ep2p->bSingle);
        ep2p->seq++;
        break;
	case P2P_BATTLE_END:
        ep2p->seq = P2P_MATCH_BOARD;
		break;
      case P2P_TRADE:
        EventCmd_UnionTrade(event);
        ep2p->seq++;
        break;
      case P2P_TRADE_END:
        ep2p->seq = P2P_MATCH_BOARD;
		break;
      case P2P_UTIL:
        sys_CreateHeap( HEAPID_BASE_APP, HEAPID_WIFIP2PMATCH, DWC_UTILITY_WORK_SIZE+0x100 );
        DwcUtilOverlayStart();
        mydwc_callUtility(HEAPID_WIFIP2PMATCH);
        OS_ResetSystem(0); 
        break;
      case P2P_EXIT:
      case P2P_SETEND:
      case P2P_FREE:
        sys_FreeMemoryEz(ep2p->pMatchParam);
		sys_FreeMemoryEz(ep2p);
        ep2p->seq++;
		return TRUE;

	  case P2P_POFIN_WAIT:
		P2P_FourWaitInit( ep2p, fsys, HEAPID_WORLD, WFP2PMF_TYPE_POFIN );
        ep2p->seq++;
		break;
		
      case P2P_POFIN_WAIT_END:
        if( !FieldEvent_Cmd_WaitSubProcEnd(fsys) ){		//サブプロセス終了待ち
            ep2p->seq = P2P_FourWaitEnd( ep2p );
        }
		break;
		
      case P2P_POFIN:

        ep2p->work = (void*)FieldNutMixer_InitCallWiFi(fsys,HEAPID_WORLD,ep2p->vchat);
        ep2p->seq++;
        break;
      case P2P_POFIN_END:
        if( !FieldEvent_Cmd_WaitSubProcEnd(fsys) ){		//サブプロセス終了待ち
            sys_FreeMemoryEz(ep2p->work);
            ep2p->seq = P2P_MATCH_BOARD;
        }
		break;

      case P2P_FRONTIER:
        CommSetWifiBothNet(FALSE);
		ep2p->work = EvCmdFrontierSystemCall( fsys,  NULL );
        ep2p->seq++;
        break;
      case P2P_FRONTIER_END:
        if( !FieldEvent_Cmd_WaitSubProcEnd(fsys) ){		//サブプロセス終了待ち
            sys_FreeMemoryEz(ep2p->work);
            ep2p->seq = P2P_MATCH_BOARD;
        }
		break;

#ifdef WFP2P_DEBUG_EXON
      case P2P_BATTLEROOM:
        ep2p->work = WBR_ProcSet(fsys,HEAPID_WORLD);
        ep2p->seq++;
        break;

      case P2P_BATTLEROOM_END:
        if( WBRSys_EndCheck( ep2p->work ) == TRUE ){		//サブプロセス終了待ち
            ep2p->seq = P2P_MATCH_BOARD;
			WBR_ProcEnd( ep2p->work );
        }
		break;
#endif

	  case P2P_BUCKET_WAIT:
		P2P_FourWaitInit( ep2p, fsys, HEAPID_WORLD, WFP2PMF_TYPE_BUCKET );
        ep2p->seq++;
		break;

	  case P2P_BUCKET_WAIT_END:
        if( !FieldEvent_Cmd_WaitSubProcEnd(fsys) ){		//サブプロセス終了待ち
            ep2p->seq = P2P_FourWaitEnd( ep2p );
        }
		break;

      case P2P_BUCKET:
		// ミニゲームスコア設定
		P2P_Record_Minigame( fsys );
        ep2p->work = BCT_ProcSet(fsys,HEAPID_WORLD,ep2p->vchat);
        ep2p->seq++;
        break;

      case P2P_BUCKET_END:
        if( !FieldEvent_Cmd_WaitSubProcEnd(fsys) ){		//サブプロセス終了待ち
            ep2p->seq = P2P_MATCH_BOARD;
			BCT_ProcEnd( ep2p->work );
        }
		break;

	  case P2P_BALANCE_BALL_WAIT:
		P2P_FourWaitInit( ep2p, fsys, HEAPID_WORLD, WFP2PMF_TYPE_BALANCE_BALL );
        ep2p->seq++;
		break;

	  case P2P_BALANCE_BALL_WAIT_END:
        if( !FieldEvent_Cmd_WaitSubProcEnd(fsys) ){		//サブプロセス終了待ち
            ep2p->seq = P2P_FourWaitEnd( ep2p );
        }
		break;

      case P2P_BALANCE_BALL:
		// ミニゲームスコア設定
		P2P_Record_Minigame( fsys );
        ep2p->work = BB_ProcSet(fsys,HEAPID_WORLD,ep2p->vchat);
        ep2p->seq++;
        break;

      case P2P_BALANCE_BALL_END:
        if( !FieldEvent_Cmd_WaitSubProcEnd(fsys) ){		//サブプロセス終了待ち
            ep2p->seq = P2P_MATCH_BOARD;
			BB_ProcEnd( ep2p->work );
        }
		break;

	  case P2P_BALLOON_WAIT:
		P2P_FourWaitInit( ep2p, fsys, HEAPID_WORLD, WFP2PMF_TYPE_BALLOON );
        ep2p->seq++;
		break;

	  case P2P_BALLOON_WAIT_END:
        if( !FieldEvent_Cmd_WaitSubProcEnd(fsys) ){		//サブプロセス終了待ち
            ep2p->seq = P2P_FourWaitEnd( ep2p );
        }
		break;

      case P2P_BALLOON:
		// ミニゲームスコア設定
		P2P_Record_Minigame( fsys );
        ep2p->work = BL_ProcSet(fsys,HEAPID_WORLD,ep2p->vchat);
        ep2p->seq++;
        break;

      case P2P_BALLOON_END:
        if( !FieldEvent_Cmd_WaitSubProcEnd(fsys) ){		//サブプロセス終了待ち
            ep2p->seq = P2P_MATCH_BOARD;
			BL_ProcEnd( ep2p->work );
        }
		break;

      default:
		return TRUE;
	}
	return FALSE;
}


static EV_P2PEVENT_WORK* _createEvP2PWork(void)
{
    EV_P2PEVENT_WORK *ep2p = sys_AllocMemoryLo(HEAPID_WORLD, sizeof(EV_P2PEVENT_WORK));
    MI_CpuClear8(ep2p, sizeof(EV_P2PEVENT_WORK));
    ep2p->pMatchParam = sys_AllocMemoryLo(HEAPID_WORLD, sizeof(WIFIP2PMATCH_PROC_PARAM));
	MI_CpuClear8(ep2p->pMatchParam, sizeof(WIFIP2PMATCH_PROC_PARAM));
    return ep2p;
}

void EventCmd_P2PMatchProc(GMEVENT_CONTROL * event)
{
	EV_P2PEVENT_WORK *ep2p = _createEvP2PWork();
    ep2p->pMatchParam->seq = WIFI_P2PMATCH_P2P;
	FieldEvent_Call(event, GMEVENT_Sub_P2PMatch, ep2p);
}

void EventCmd_DPWInitProc(GMEVENT_CONTROL * event)
{
	EV_P2PEVENT_WORK *ep2p = _createEvP2PWork();
    ep2p->pMatchParam->seq = WIFI_P2PMATCH_DPW;
	FieldEvent_Call(event, GMEVENT_Sub_P2PMatch, ep2p);
}

void EventCmd_DPWInitProc2(GMEVENT_CONTROL * event, u16* ret)
{
	EV_P2PEVENT_WORK *ep2p = _createEvP2PWork();
    ep2p->pMatchParam->seq = WIFI_P2PMATCH_DPW;
    ep2p->ret = ret;
	FieldEvent_Call(event, GMEVENT_Sub_P2PMatch, ep2p);
}





//----------------------------------------------------------------------------
/**
 *	@brief	４にん待合室初期化
 *
 *	@param	p_wk		ワーク
 *	@param	fsys		フィールドシステム
 *	@param	heapID		ヒープ
 */
//-----------------------------------------------------------------------------
static void P2P_FourWaitInit( EV_P2PEVENT_WORK* p_wk, FIELDSYS_WORK* fsys, u32 heapID, u32 type )
{
	WFP2PMF_INIT* p_work;

	p_work = sys_AllocMemory( heapID, sizeof(WFP2PMF_INIT) );

	p_work->type = type;
	p_work->comm_min = 2;
	p_work->comm_max = sc_P2P_FOUR_MATCH_MAX[ type ];
	p_work->result = FALSE;
	p_work->vchat = FALSE;
	p_work->p_savedata = fsys->savedata;
	
	p_wk->work = p_work;

	GameSystem_StartSubProc(fsys, &WifiP2PMatchFourProcData, p_work);
}
//----------------------------------------------------------------------------
/**
 *	@brief	４にん待合室募集終了
 *
 *	@param	p_wk	ワーク
 *
 *	@retval	次進んでほしいシーケンス
 */
//-----------------------------------------------------------------------------
static u32 P2P_FourWaitEnd( EV_P2PEVENT_WORK* p_wk )
{
	WFP2PMF_INIT* p_work = p_wk->work;

	if( p_work->result == TRUE ){
		switch( p_work->type ){
		// ポフィン
		case WFP2PMF_TYPE_POFIN:
			p_wk->seq = P2P_POFIN;
			break;
		// たまいれ
		case WFP2PMF_TYPE_BUCKET:
			p_wk->seq = P2P_BUCKET;
			break;
		// たまのり
		case WFP2PMF_TYPE_BALANCE_BALL:
			p_wk->seq = P2P_BALANCE_BALL;
			break;
		// ふうせんわり
		case WFP2PMF_TYPE_BALLOON:
			p_wk->seq = P2P_BALLOON;
			break;
		}
	}else{
		p_wk->seq = P2P_MATCH_BOARD_INIT;
	}
	p_wk->vchat = p_work->vchat;
	sys_FreeMemoryEz( p_wk->work );

	return p_wk->seq;
}

//----------------------------------------------------------------------------
/**
 *	@brief	バケットゲーム開始
 *
 *	@param	fsys		システムワーク
 *	@param	heapID		ヒープID
 */	
//-----------------------------------------------------------------------------
static BUCKET_PROC_WORK* BCT_ProcSet( FIELDSYS_WORK* fsys, u32 heapID, u32 vchat )
{
	BUCKET_PROC_WORK* p_work;

	FS_EXTERN_OVERLAY(bucket);
	FS_EXTERN_OVERLAY(minigame_common);
	{
		// プロセス定義データ
		static const PROC_DATA BCTProcData = {
			BucketProc_Init,
			BucketProc_Main,
			BucketProc_End,
			FS_OVERLAY_ID(bucket),
		};
		p_work = sys_AllocMemory( heapID, sizeof(BUCKET_PROC_WORK) );
		memset( p_work, 0, sizeof(BUCKET_PROC_WORK) );
		p_work->vchat		= vchat;
		p_work->wifi_lobby	= FALSE;
		p_work->p_save		= fsys->savedata;

		// ミニゲーム共通オーバーレイを読み込む
		Overlay_Load( FS_OVERLAY_ID(minigame_common), OVERLAY_LOAD_NOT_SYNCHRONIZE);
		
		GameSystem_StartSubProc(fsys, &BCTProcData, p_work);
	}
	return p_work;
}

//----------------------------------------------------------------------------
/**
 *	@brief	バケットゲーム終了
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static void BCT_ProcEnd( BUCKET_PROC_WORK* p_wk )
{
	FS_EXTERN_OVERLAY(minigame_common);

	sys_FreeMemoryEz( p_wk );

	Overlay_UnloadID( FS_OVERLAY_ID(minigame_common) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	バランスボールプロセス設定
 *
 *	@param	fsys		ワーク
 *	@param	heapID		ヒープ
 *	@param	vchat		VCHAT
 */
//-----------------------------------------------------------------------------
static BB_PROC_WORK* BB_ProcSet( FIELDSYS_WORK* fsys, u32 heapID, u32 vchat )
{
	BB_PROC_WORK* p_work;

	FS_EXTERN_OVERLAY(balance_ball);
	FS_EXTERN_OVERLAY(minigame_common);
	{
		// プロセス定義データ
		static const PROC_DATA BBProcData = {
			BalanceBallProc_Init,
			BalanceBallProc_Main,
			BalanceBallProc_Exit,
			FS_OVERLAY_ID(balance_ball),
		};
		p_work = sys_AllocMemory( heapID, sizeof(BB_PROC_WORK) );
		memset( p_work, 0, sizeof(BB_PROC_WORK) );
		p_work->vchat = vchat;
		p_work->wifi_lobby	= FALSE;
		p_work->p_save= fsys->savedata;

		// ミニゲーム共通オーバーレイを読み込む
		Overlay_Load( FS_OVERLAY_ID(minigame_common), OVERLAY_LOAD_NOT_SYNCHRONIZE);
		
		GameSystem_StartSubProc(fsys, &BBProcData, p_work);
	}
	return p_work;
}

//----------------------------------------------------------------------------
/**
 *	@brief	バランスボールプロセス破棄
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void BB_ProcEnd( BB_PROC_WORK* p_wk )
{
	FS_EXTERN_OVERLAY(minigame_common);

	sys_FreeMemoryEz( p_wk );

	Overlay_UnloadID( FS_OVERLAY_ID(minigame_common) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ふうせんわりプロセス設定
 *
 *	@param	fsys		ワーク
 *	@param	heapID		ヒープ
 *	@param	vchat		VCHAT
 */
//-----------------------------------------------------------------------------
static BALLOON_PROC_WORK* BL_ProcSet( FIELDSYS_WORK* fsys, u32 heapID, u32 vchat )
{
	BALLOON_PROC_WORK* p_work;

	FS_EXTERN_OVERLAY(balloon);
	FS_EXTERN_OVERLAY(minigame_common);
	{
		// プロセス定義データ
		static const PROC_DATA BCTProcData = {
			BalloonProc_Init,
			BalloonProc_Main,
			BalloonProc_End,
			FS_OVERLAY_ID(balloon),
		};
		p_work = sys_AllocMemory( heapID, sizeof(BALLOON_PROC_WORK) );
		MI_CpuClear8(p_work, sizeof(BALLOON_PROC_WORK));
		p_work->vchat		= vchat;
		p_work->wifi_lobby	= FALSE;
		p_work->p_save		= fsys->savedata;

		// ミニゲーム共通オーバーレイを読み込む
		Overlay_Load( FS_OVERLAY_ID(minigame_common), OVERLAY_LOAD_NOT_SYNCHRONIZE);
		
		GameSystem_StartSubProc(fsys, &BCTProcData, p_work);
	}
	return p_work;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ふうせんわりプロセス破棄
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void BL_ProcEnd( BUCKET_PROC_WORK* p_wk )
{
	FS_EXTERN_OVERLAY(minigame_common);

	sys_FreeMemoryEz( p_wk );

	Overlay_UnloadID( FS_OVERLAY_ID(minigame_common) );
}


//----------------------------------------------------------------------------
/**
 *	@brief	ミニゲームで遊んだ	スコア計算
 *
 *	@param	fsys	
 */
//-----------------------------------------------------------------------------
static void P2P_Record_Minigame( FIELDSYS_WORK* fsys )
{
	RECORD* p_rec;
	
	p_rec = SaveData_GetRecord( fsys->savedata );
	RECORD_Score_Add( p_rec, SCORE_ID_CLUB_MINIGAME );
}


#ifdef WFP2P_DEBUG
// オーバーレイID宣言
FS_EXTERN_OVERLAY(wifi_battleroom);
//----------------------------------------------------------------------------
/**
 *	@brief	バトルルームプロセス設定
 *
 *	@param	fsys		フィールドシステム
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static WIFI_BATTLEROOM* WBR_ProcSet( FIELDSYS_WORK* fsys, u32 heapID )
{
	WBR_INIT init;

	// オーバーレイロード
	Overlay_Load( FS_OVERLAY_ID(wifi_battleroom), OVERLAY_LOAD_NOT_SYNCHRONIZE);
	
	init.p_fsys = fsys;
	
	return WBRSys_Init( &init, heapID );
}
static void WBR_ProcEnd( WIFI_BATTLEROOM* p_wk )
{
	WBRSys_Exit( p_wk );
	Overlay_UnloadID( FS_OVERLAY_ID(wifi_battleroom) );
}


#endif
