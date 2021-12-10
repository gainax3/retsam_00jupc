//==============================================================================
/**
 * @file	d_goto.c
 * @brief	後藤用デバッグ用ファイル
 * @author	goto
 * @date	2005.06.20(月)
 */
//==============================================================================
#ifdef PM_DEBUG

#include "common.h"
#include "system/pm_overlay.h"
#include "system/test_overlay.h"

#include "system/bmp_list.h"
#include "fieldmap.h"
#include "fieldsys.h"
#include "system/fontproc.h"
#include "contest/contest.h"

#include "script.h"

#include "system/fontproc.h"
#include "system/msgdata.h"
#include "msgdata/msg.naix"
#include "msgdata/msg_debug_goto.h"

#include "system/snd_tool.h"

#include "include/savedata/battle_rec.h"
#include "demo/demo_tengan.h"
#include "include/msgdata/msg_br_ranking.h"

extern void DebugGotoMenuInit(FIELDSYS_WORK* fsys);

extern void CustomBallEdit_Start(FIELDSYS_WORK *fsys);

//==============================================================================
//	オーバーレイIDのextern宣言
//==============================================================================
/* overlay id in *.lcf */
FS_EXTERN_OVERLAY(test_first);
FS_EXTERN_OVERLAY(test_second);
FS_EXTERN_OVERLAY(test_itcm);
FS_EXTERN_OVERLAY(test_second_abc);
FS_EXTERN_OVERLAY(demo_tenganzan);

#define DGOTO_HEAP	(HEAPID_BASE_DEBUG)

//==============================================================================
//	構造体宣言
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   後藤デバッグ用構造体
 */
//--------------------------------------------------------------
typedef struct{
	FIELDSYS_WORK *fsys;
	BMPLIST_WORK *lw;
	BMP_MENULIST_DATA *listmenu;
	GF_BGL_BMPWIN bmpwin;
	s16 wait;
	u8 main_seq;
		
}DGOTO_WORK;


//==============================================================================
//	型宣言
//==============================================================================
typedef void (*pDMFunc)(DGOTO_WORK *);


//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static void DG_MenuMain(TCB_PTR tcb, void *work);
static void DG_MenuExit(DGOTO_WORK *dm);
static void DG_CustomStart(DGOTO_WORK *dm);
static void DG_CustomStart2(DGOTO_WORK *dm);
static void DG_SealAdd(DGOTO_WORK *dm);
static void DG_EggDemoNormal(DGOTO_WORK *dm);
static void DG_EggDemoSpecial(DGOTO_WORK *dm);

static void DG_Prof1(DGOTO_WORK *dm);
static void DG_Prof2(DGOTO_WORK *dm);
static void DG_Prof3(DGOTO_WORK *dm);

static void DG_br_000( DGOTO_WORK *dm );
static void DG_br_001( DGOTO_WORK *dm );
static void DG_br_002( DGOTO_WORK *dm );
static void DG_br_003( DGOTO_WORK *dm );
static void DG_br_004( DGOTO_WORK *dm );
static void DG_br_005( DGOTO_WORK *dm );
static void DG_br_006( DGOTO_WORK *dm );

static void DG_Demo_TENGAN( DGOTO_WORK *dm );

static void DG_BattleRecoder_Score( DGOTO_WORK *dm );
static void DG_Honey( DGOTO_WORK *dm );
static void DG_CountryDelete( DGOTO_WORK *dm );


//==============================================================================
//	デバッグメニューリスト用データ等
//==============================================================================
///デバッグメニューの項目
static const struct {
	
	u32  str_id;
	u32  param;
	
} DebugMenuParamList[] = {	
	{ dg_004,			(u32)DG_Demo_TENGAN	},
	{ dmg_03,			(u32)DG_EggDemoNormal	},
	{ dmg_04,			(u32)DG_EggDemoSpecial	},
{ 	  dg_000,			(u32)BMPLIST_RABEL	},	
	{ dg_010,			(u32)DG_br_000	},
	{ dg_012,			(u32)DG_BattleRecoder_Score },
	{ debug_honey_000,	(u32)DG_Honey },
	{ dg_006,			(u32)DG_CountryDelete },
	{ dg_011,			(u32)DG_br_001	},
	{ dg_005,			(u32)DG_br_002	},
{ 	  dg_000,			(u32)BMPLIST_RABEL	},
	{ dmg_01,			(u32)DG_CustomStart		},
	{ dmg_01_01,		(u32)DG_CustomStart2	},
	{ dmg_02,			(u32)DG_SealAdd			},
{ 	  dg_000,			(u32)BMPLIST_RABEL	},	
	{ dg_001,			(u32)DG_Prof1			},
	{ dg_002,			(u32)DG_Prof2			},
	{ dg_003,			(u32)DG_Prof3			},	
};

#define REC_NUM	( 87 )
#define RECID_DUMYY_DATA	( 0xFFFF )
static u32 RecordID_Table[ REC_NUM ] = {
	RECID_DENDOU_CNT,
	RECID_DUMYY_DATA,//RECID_WAZA_HANERU,	
	RECID_DUMYY_DATA,//RECID_WAZA_ZIBAKU,
	RECID_DUMYY_DATA,//RECID_WAZA_DAIBAKUHATU,
	RECID_DUMYY_DATA,
	RECID_DUMYY_DATA,
	RECID_DUMYY_DATA,
	RECID_DUMYY_DATA,
	RECID_DUMYY_DATA,
	RECID_DUMYY_DATA,
	RECID_DUMYY_DATA,
	RECID_DUMYY_DATA,
	RECID_DUMYY_DATA,
	RECID_DUMYY_DATA,
	RECID_CONTEST_ENTRY,
	RECID_CONTEST_COMM_ENTRY,
	RECID_CONTEST_GPX,
	RECID_CONTEST_COMM_GPX,
	RECID_DUMYY_DATA,//RECID_CONTEST_RIBBON_GET,
	RECID_DUMYY_DATA,//RECID_WAZA_MUKOU,
	RECID_DUMYY_DATA,//RECID_TEMOTI_KIZETU,
	RECID_DUMYY_DATA,//RECID_TEMOTI_MAKIZOE,
	RECID_DUMYY_DATA,//RECID_NIGERU_SIPPAI,
	RECID_NIGERARETA,
	RECID_DUMYY_DATA,//RECID_FISHING_FAILURE,
	RECID_NUTS_PLANT,
	RECID_WALK_COUNT,
	RECID_BTL_ENCOUNT,
	RECID_BTL_TRAINER,
	RECID_CAPTURE_POKE,
	RECID_FISHING_SUCCESS,
	RECID_TAMAGO_HATCHING,
	RECID_POKE_EVOLUTION,
	RECID_SLOT_FEAVER,
	RECID_DUMYY_DATA,//RECID_BTOWER_CHALLENGE,
	RECID_DUMYY_DATA,//RECID_COMM_TRADE,
	RECID_DUMYY_DATA,//RECID_COMM_BTL_WIN,
	RECID_DUMYY_DATA,//RECID_COMM_BTL_LOSE,
	RECID_DUMYY_DATA,//RECID_COMM_BTL_DRAW,
	RECID_WIFI_TRADE,
	RECID_WIFI_BTL_WIN,
	RECID_DUMYY_DATA,//RECID_WIFI_BTL_LOSE,
	RECID_WIFI_BTL_DRAW,
	RECID_BTOWER_WIN,
	RECID_SHOPPING_MONEY,
	RECID_SODATEYA_CNT,
	RECID_KILL_POKE,
	RECID_GTS_PUT,
	RECID_WIFICLUB_TRADE,
	RECID_MYSIGN_WRITE,
	RECID_FOSSIL_RESTORE,
	RECID_FOOTMARK_LOOK,
	RECID_MAIL_WRITE,
	RECID_MITSU_ENC,
	RECID_LOBBY_CHAT,
	RECID_UG_BALL,
	RECID_TV_COUNT,
	RECID_NICKNAME,
	RECID_PREMIUM_BALL,
	RECID_FUREAI_ITEM,
	RECID_COOKING_SINGLE,
	RECID_COOKING_FRIENDS,
	RECID_DRESS_COUNT,
	RECID_KAIRIKI_COUNT,
	RECID_MARSH_COUNT,
	RECID_LEADERHOUSE_BATTLE,
	RECID_FRONTIER_COUNT,
	RECID_FRONTIER_BRAIN,
	RECID_FACTORY_WIN,
	RECID_CASTLE_WIN,
	RECID_STAGE_WIN,
	RECID_ROULETTE,
	RECID_FACTORY_POKE_CHANGE,
	RECID_CASTLE_POINT,
	RECID_STAGE_RANK10_WIN,
	RECID_ROULETTE_BP,
	RECID_WIN_BP,
	RECID_USE_BP,
	RECID_LOBBY_GAME_COUNT,
	RECID_GURUGURU_COUNT,
	RECID_CHANGE_HOSHINOKAKERA,
	RECID_DUMYY_DATA,
	RECID_DUMYY_DATA,
	RECID_DUMYY_DATA,
	RECID_DUMYY_DATA,
	RECID_DUMYY_DATA,
	RECID_DUMYY_DATA
};

typedef struct {
	
	int				index;
	int				seq;
	int				cur;
	u32				param;
	GF_BGL_BMPWIN	win;
	FIELDSYS_WORK*	fsys;
	MSGDATA_MANAGER* man_ranking;
	
} SCORE_WORK;

static s64 Send_RECORD_Get( SCORE_WORK* wk )
{
	s64 data;
	int index;
	
	index = wk->index;

	if ( index >= REC_NUM ){ index = 0; }
	if ( RecordID_Table[ index ] == RECID_DUMYY_DATA ){ return -1; }

	data = RECORD_Get( SaveData_GetRecord( wk->fsys->savedata ), RecordID_Table[ index ] );
	
	OS_Printf( "wk->param1 = %d\n", data );

	wk->param = (u32)data;
	
	OS_Printf( "wk->param2 = %d\n", wk->param );

	return data;
}

static void Send_RECORD_Set( SCORE_WORK* wk )
{
	s64 data;
	int index;
	
	index = wk->index;

	RECORD_Set( SaveData_GetRecord( wk->fsys->savedata ), RecordID_Table[ index ], wk->param );
}

static BOOL Recode_Print( SCORE_WORK* wk )
{
	STRBUF*		str1;
	STRBUF*		str2;
	STRBUF*		str3;
	STRBUF*		temp;
	WORDSET*	wset;
	STRBUF*		str_num;	
	s64			data;
	
	
	data = Send_RECORD_Get( wk );
	
	if ( data < 0 ){
		return FALSE;
	}
	
	str1 = STRBUF_Create( 255, DGOTO_HEAP );
	str2 = MSGMAN_AllocString( wk->man_ranking, wk->index );
	
	//STRBUF_SetNumber( str1, data, 5, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
	STRBUF_SetNumber( str1, data, 9, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
	
	GF_BGL_BmpWinDataFill( &wk->win, 0xFF );

	GF_STR_PrintSimple( &wk->win, FONT_SYSTEM, str2, 4,  0, MSG_NO_PUT, NULL );
	GF_STR_PrintSimple( &wk->win, FONT_SYSTEM, str1, 4, 32, MSG_NO_PUT, NULL );

	GF_BGL_BmpWinOn( &wk->win );
	
	STRBUF_Delete( str1 );
	STRBUF_Delete( str2 );
	
	return TRUE;
}
static BOOL Recode_Print2( SCORE_WORK* wk )
{
	STRBUF*		str1;
	STRBUF*		str2;
	STRBUF*		str3;
	STRBUF*		temp;
	WORDSET*	wset;
	STRBUF*		str_num;	
	s64			data;
	
	
	data = wk->param;//Send_RECORD_Get( wk );
	
	if ( data < 0 ){
		return FALSE;
	}
	
	str1 = STRBUF_Create( 255, DGOTO_HEAP );
	str2 = MSGMAN_AllocString( wk->man_ranking, wk->index );
	
	STRBUF_SetNumber( str1, data, 5, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
	
	GF_BGL_BmpWinDataFill( &wk->win, 0xFF );

	GF_STR_PrintSimple( &wk->win, FONT_SYSTEM, str2, 4,  0, MSG_NO_PUT, NULL );
	GF_STR_PrintSimple( &wk->win, FONT_SYSTEM, str1, 4, 32, MSG_NO_PUT, NULL );

	GF_BGL_BmpWinOn( &wk->win );
	
	STRBUF_Delete( str1 );
	STRBUF_Delete( str2 );
	
	return TRUE;
}

static void DG_ScoreTCB( TCB_PTR tcb, void* work )
{
	SCORE_WORK* wk = work;
	
	switch ( wk->seq ){
	case 0:		
		GF_BGL_BmpWinInit( &wk->win );
		GF_BGL_BmpWinAdd( wk->fsys->bgl, &wk->win, FLD_MBGFRM_FONT, 2, 2, 24, 8, 13, 1);
		wk->index = 0;
		Recode_Print( wk );
		wk->seq++;
		break;
	
	case 1:
		if ( sys.repeat & PAD_KEY_UP ){
			do {
				if ( wk->index != 0 ){
					 wk->index--;
				}
				else {
					 wk->index = REC_NUM - 1;
				}
			} while ( Recode_Print( wk ) == FALSE );
		}		
		if ( sys.repeat & PAD_KEY_DOWN ){
			do {
				wk->index++;
				wk->index %= REC_NUM;
			} while ( Recode_Print( wk ) == FALSE );			
		}
		if ( sys.repeat & PAD_KEY_RIGHT ){
			if ( sys.cont & PAD_BUTTON_R ){
				wk->param += 10;
			}
			else {
				wk->param += 1;
			}
			wk->param %= 100000;
			Recode_Print2( wk );
		}
		if ( sys.repeat & PAD_KEY_LEFT ){
			if ( sys.cont & PAD_BUTTON_R ){
				if ( wk->param >= 10 ){
					wk->param -= 10;
				}
				else {
					wk->param = 99999;
				}
			}
			else {
				OS_Printf( "wk->param0 = %d\n", wk->param );
				if ( wk->param >= 1 ){
					wk->param -= 1;
				}
				else {
					wk->param = 99999;
				}
			}
			Recode_Print2( wk );
		}
		if ( sys.trg & PAD_BUTTON_A ){
			Send_RECORD_Set( wk );
		}
		if ( sys.trg & PAD_BUTTON_B ){
			wk->seq++;
		}
		break;
	
	default:
		GF_BGL_BmpWinOff( &wk->win );
		GF_BGL_BmpWinDel( &wk->win );
		MSGMAN_Delete( wk->man_ranking );
		sys_FreeMemoryEz( wk );
		TCB_Delete( tcb );
		FieldSystemProc_SeqHoldEnd();
		break;
	}
}

static void DG_BattleRecoder_Score( DGOTO_WORK *dm )
{	
	SCORE_WORK* wk = sys_AllocMemory( DGOTO_HEAP, sizeof( SCORE_WORK ) );
	
	wk->seq = 0;
	wk->cur = 0;
	wk->fsys = dm->fsys;	
	wk->man_ranking	= MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_br_ranking_dat, DGOTO_HEAP );

	TCB_Add( DG_ScoreTCB, wk, 100 );
}

#include "savedata/honeytree_data.h"
#include "savedata/encount.h"

typedef struct {
	
	int seq;
	FIELDSYS_WORK* fsys;
	GF_BGL_BMPWIN	win;
	
} HONEY_WORK;

#if 0
static const int HoneyTreeZoneList[HONEY_TREE_MAX] = {
	ZONE_ID_R205A,
	ZONE_ID_R205B,
	ZONE_ID_R206,
	ZONE_ID_R207,
	ZONE_ID_R208,
	ZONE_ID_R209,
	ZONE_ID_R210A,
	ZONE_ID_R210B,
	ZONE_ID_R211B,
	ZONE_ID_R212A,
	ZONE_ID_R212B,
	ZONE_ID_R213,
	ZONE_ID_R214,
	ZONE_ID_R215,
	ZONE_ID_R218,
	ZONE_ID_R221,
	ZONE_ID_R222,
	ZONE_ID_D02,
	ZONE_ID_D03,
	ZONE_ID_D04,
	ZONE_ID_D13R0101,
};
#endif

static void HoneyPrint( HONEY_WORK* wk )
{
	int i;
	int cnt = 0;
	STRBUF*		str1;
	STRBUF*		str2;
	HONEY_DATA* data;
	MSGDATA_MANAGER* man;
	HT_PTR ptr = EncDataSave_GetHoneyTree( EncDataSave_GetSaveDataPtr( wk->fsys->savedata ) );
	int mes[] = { debug_honey_001, debug_honey_002, debug_honey_003, debug_honey_004 };
	
	GF_BGL_BmpWinDataFill( &wk->win, 0xFF );
	
	man = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_debug_goto_dat, DGOTO_HEAP );
	
	for ( i = 0; i < HONEY_TREE_MAX / 2; i++ ){	
		data = HTSave_GetHoneyDataPtr( i, ptr );
		str1 = MSGMAN_AllocString( man, mes[ data->RareLv ] );			
		str2 = MSGMAN_AllocString( man, debug_honey_zid_000 + i );
		
		GF_STR_PrintSimple( &wk->win, FONT_SYSTEM, str2,  4, ( cnt * 16 ), MSG_NO_PUT, NULL );
		GF_STR_PrintSimple( &wk->win, FONT_SYSTEM, str1, 60, ( cnt * 16 ), MSG_NO_PUT, NULL );
		
		STRBUF_Delete( str1 );
		STRBUF_Delete( str2 );
		cnt++;
	}

	cnt = 0;
	for ( ; i < HONEY_TREE_MAX; i++ ){	
		data = HTSave_GetHoneyDataPtr( i, ptr );
		str1 = MSGMAN_AllocString( man, mes[ data->RareLv ] );			
		str2 = MSGMAN_AllocString( man, debug_honey_zid_000 + i );

		GF_STR_PrintSimple( &wk->win, FONT_SYSTEM, str2, 120, ( cnt * 16 ), MSG_NO_PUT, NULL );
		GF_STR_PrintSimple( &wk->win, FONT_SYSTEM, str1, 180, ( cnt * 16 ), MSG_NO_PUT, NULL );

		STRBUF_Delete( str1 );
		STRBUF_Delete( str2 );
		cnt++;
	}
	
	MSGMAN_Delete( man );
	
	GF_BGL_BmpWinOn( &wk->win );
}

static void DG_HoneyTCB( TCB_PTR tcb, void* work )
{
	HONEY_WORK* wk = work;
	
	switch ( wk->seq ){
	case 0:
		GF_BGL_BmpWinInit( &wk->win );
		GF_BGL_BmpWinAdd( wk->fsys->bgl, &wk->win, FLD_MBGFRM_FONT, 1, 1, 30, 22, 13, 1 );
		HoneyPrint( wk );
		wk->seq++;

	case 1:
		if ( sys.trg & ( PAD_BUTTON_A | PAD_BUTTON_B ) ){
			wk->seq++;
		}
		break;
	
	default:
		GF_BGL_BmpWinOff( &wk->win );
		GF_BGL_BmpWinDel( &wk->win );
		sys_FreeMemoryEz( wk );
		TCB_Delete( tcb );
		FieldSystemProc_SeqHoldEnd();
		break;
	}
}

static void DG_Honey( DGOTO_WORK *dm )
{
	HONEY_WORK* wk = sys_AllocMemory( DGOTO_HEAP, sizeof( HONEY_WORK ) );
		
	wk->seq  = 0;
	wk->fsys = dm->fsys;
	
	TCB_Add( DG_HoneyTCB, wk, 100 );
}

#include "savedata/wifihistory.h"
static void DG_CountryDelete( DGOTO_WORK *dm )
{
	WIFI_HISTORY* wh = SaveData_GetWifiHistory( dm->fsys->savedata );
	
	WIFIHISTORY_SetMyNationArea( wh, 0, 0 );
	
	Snd_SePlay( SEQ_SE_DP_SELECT );
	
	FieldSystemProc_SeqHoldEnd();
}

#define LIST_MENU_MAX		(NELEMS(DebugMenuParamList))

///デバッグメニューのリスト
static const BMPLIST_HEADER DebugMenuListHeader = {
	NULL,			// 表示文字データポインタ
	NULL,					// カーソル移動ごとのコールバック関数
	NULL,					// 一列表示ごとのコールバック関数
	NULL,
	LIST_MENU_MAX,	// リスト項目数
	11,						// 表示最大項目数
	0,						// ラベル表示Ｘ座標
	8,						// 項目表示Ｘ座標
	0,						// カーソル表示Ｘ座標
	0,						// 表示Ｙ座標
	FBMP_COL_WHITE,			// 文字色
	FBMP_COL_BLACK,			// 背景色
	FBMP_COL_BLK_SDW,		// 文字影色
	0,						// 文字間隔Ｘ
	16,						// 文字間隔Ｙ
	BMPLIST_LRKEY_SKIP,		// ページスキップタイプ
	FONT_SYSTEM,				// 文字指定
	0,						// ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)
};


void DebugGotoMenuInit(FIELDSYS_WORK* fsys)
{
	DGOTO_WORK *dm;
	BMPLIST_HEADER list_h;
	
	dm = sys_AllocMemory(DGOTO_HEAP, sizeof(DGOTO_WORK));
	memset(dm, 0, sizeof(DGOTO_WORK));
	
	//BMPウィンドウ生成
	GF_BGL_BmpWinAdd(fsys->bgl, &dm->bmpwin, 
		FLD_MBGFRM_FONT, 1, 1, 18, 24, 13, 1);
	dm->fsys = fsys;
	
	dm->listmenu = BMP_MENULIST_Create(LIST_MENU_MAX, DGOTO_HEAP);

	//文字列リスト作成
	{
		MSGDATA_MANAGER *man;
		STRBUF * str_buf;
		int i;
		
		man = MSGMAN_Create(MSGMAN_TYPE_NORMAL, ARC_MSG, 
			NARC_msg_debug_goto_dat, DGOTO_HEAP);
		
		for(i = 0; i < LIST_MENU_MAX; i++){
			BMP_MENULIST_AddArchiveString(dm->listmenu, man, DebugMenuParamList[i].str_id, 
				DebugMenuParamList[i].param);
		}
		
		MSGMAN_Delete(man);
	}
	
	list_h = DebugMenuListHeader;
	list_h.win = &dm->bmpwin;
    list_h.list = dm->listmenu;
	dm->lw = BmpListSet(&list_h, 0, 0, DGOTO_HEAP);
	
	GF_BGL_BmpWinOn(&dm->bmpwin);
	
	TCB_Add(DG_MenuMain, dm, 10);
//	FieldSystemProc_SeqHoldEnd();
}

//--------------------------------------------------------------
/**
 * @brief   後藤用デバッグメニューリスト選択
 *
 * @param   tcb			TCBへのポインタ
 * @param   work		デバッグ用後藤ワークへのポインタ
 */
//--------------------------------------------------------------
static void DG_MenuMain(TCB_PTR tcb, void *work)
{
	DGOTO_WORK *dm = work;
	u32	ret;

	ret = BmpListMain(dm->lw);
	switch(dm->main_seq){
	case 0:
		dm->main_seq++;
		break;
	case 1:
		switch(ret){
		case BMPLIST_NULL:
			break;
		case BMPLIST_CANCEL:
			DG_MenuExit(dm);
			TCB_Delete(tcb);
			sys_FreeMemoryEz(dm);
			FieldSystemProc_SeqHoldEnd();
			return;
		default:
			{
				pDMFunc func = (pDMFunc)ret;

				DG_MenuExit(dm);
				TCB_Delete(tcb);
				func(dm);
				sys_FreeMemoryEz(dm);
				return;
			}
			break;
		}
		break;
	}
}

//--------------------------------------------------------------
/**
 * @brief   後藤用デバッグメニューリストを閉じる
 * @param   dm		デバッグ用後藤ワークへのポインタ
 */
//--------------------------------------------------------------
static void DG_MenuExit(DGOTO_WORK *dm)
{
	BMP_MENULIST_Delete(dm->listmenu);
	BmpListExit(dm->lw, NULL, NULL);

	GF_BGL_BmpWinOff(&dm->bmpwin);
	GF_BGL_BmpWinDel(&dm->bmpwin);
}


#include "savedata/custom_ball.h"
#include "src/demo/egg/egg.h"
#include "d_taya2.h"



static void Debug_ChangeScheneReq( FIELDSYS_WORK* fsys, ScheneChangeCallback start_func, ScheneChangeCallback end_func, void* gen_wk )
{
	DEBUG_CHANGESCHENE_WORK* wk = sys_AllocMemory( HEAPID_BASE_DEBUG, sizeof(DEBUG_CHANGESCHENE_WORK) );
	if( wk != NULL )
	{
		wk->seq = 0;
		wk->start_func = start_func;
		wk->end_func = end_func;
		wk->gen_wk = gen_wk;
		FieldEvent_Set(fsys, GMEVENT_ChangeScheneDebug, wk);
	}
}

static void DG_Demo_TenganStart( FIELDSYS_WORK* fsys, void* wk )
{
	static const PROC_DATA proc_data = {
		DemoTengan_Proc_Init,
		DemoTengan_Proc_Main,
		DemoTengan_Proc_Exit,
		FS_OVERLAY_ID( demo_tenganzan ),
	};
	
	GameSystem_StartSubProc( fsys,  &proc_data, wk );
}
#if 0
PROC_RESULT Test_Proc_Init( PROC * proc, int * seq )
{
	return PROC_RES_FINISH;
}
PROC_RESULT Test_Proc_Main( PROC * proc, int * seq )
{
	if ( sys.trg & PAD_BUTTON_A ){
		return PROC_RES_FINISH;
	}
	return PROC_RES_CONTINUE;
}
PROC_RESULT Test_Proc_End( PROC * proc, int * seq )
{
	return PROC_RES_FINISH;
}
static void Debug_Start( FIELDSYS_WORK* fsys, void* wk )
{
	static const PROC_DATA proc_data = {
		Test_Proc_InitInit,
		Test_Proc_InitMain,
		Test_Proc_InitExit,
		NO_OVERLAY_ID,
	};	
	GameSystem_StartSubProc( fsys,  &proc_data, wk );
}
#endif
static void DG_Demo_TenganEnd( FIELDSYS_WORK* fsys, void* wk )
{
	sys_FreeMemoryEz( wk );
}

static void DG_Demo_TENGAN( DGOTO_WORK *dm )
{
	DEMO_TENGAN_PARAM* wk;
	
	wk = sys_AllocMemory( HEAPID_BASE_DEBUG, sizeof( DEMO_TENGAN_PARAM ) );
	
	wk->player_sex	= 0;
	wk->cfg			= SaveData_GetConfig( dm->fsys->savedata );
	wk->my_status	= SaveData_GetMyStatus( dm->fsys->savedata );
	
	Debug_ChangeScheneReq( dm->fsys, DG_Demo_TenganStart, DG_Demo_TenganEnd, wk );
}

static void DG_CustomStart( DGOTO_WORK *dm )
{

//	GameSystem_StartSubProc( dm->fsys,  &proc_data, wk );

	CustomBallEdit_Start(dm->fsys);
	
}

static void DG_CustomStart2(DGOTO_WORK *dm)
{
	Debug_CB_SaveData_Sample(CB_SaveData_AllDataGet(dm->fsys->savedata), 1);
	
	CustomBallEdit_Start(dm->fsys);
	
}

static void DG_SealAdd(DGOTO_WORK *dm)
{
	Debug_CB_SaveData_Sample(CB_SaveData_AllDataGet(dm->fsys->savedata), DEBUG_CB_MODE_ITEM_RANDOM);
	
	CustomBallEdit_Start(dm->fsys);
}


static void DG_EggDemoNormal(DGOTO_WORK *dm)
{
	DebugEGG_DemoStart(dm->fsys);
}


static void DG_EggDemoSpecial(DGOTO_WORK *dm)
{
	DebugEGG_DemoStartMana(dm->fsys);
}

/// profile
#include "src/savedata/gds_profile_types.h"
#include "gflib/strbuf_family.h"

static const GDS_PROFILE gds_table[] = {
	{
		{ HI_,TO_,MI_,sa_,n_,EOM_ },
		55555,
		PM_FEMALE,18,10,
		6,2,
		1,1,
		0,0,316,		
		0,0,
		{ 1,1,1,1 },
		{ 0},
		{0},		
	},
	{
		{ hi_,to_,mi_,sa_,n_,EOM_ },
		55555,
		PM_MALE,99,10,
		3,1,
		1,1,
		0,0,316,		
		0,0,
		{ 5,5,5,5 },
		{ 0,},
		{0},
	},
	{
		{ HI_,to_,MI_,sa_,n_,EOM_ },
		55555,
		PM_FEMALE,22,10,
		9,2,
		1,1,
		0,0,316,		
		0,0,
		{ 3,2,3,2 },
		{ 0,},
		{0},
	},
};

static void DG_Prof_Set( DGOTO_WORK *dm, int no, int sample )
{
	GDS_PROFILE_PTR gpp;
	BATTLE_REC_SAVEDATA *work;
	SAVE_RESULT result;
	LOAD_RESULT load_result;
	
	work = SaveData_Extra_LoadBattleRecData( dm->fsys->savedata, DGOTO_HEAP, &load_result, LOADDATA_MYREC );
//	BattleRec_WorkInit( work );
	
	MI_CpuCopy32( (void*)(&gds_table[ sample ]), (void*)work, sizeof( GDS_PROFILE ) );
	result = SaveData_Extra_SaveBattleRecData( dm->fsys->savedata, work, no );
	sys_FreeMemoryEz( work );
	
	Snd_SePlay( SEQ_SE_DP_SELECT );
}

static void DG_Prof1( DGOTO_WORK *dm )
{	
	DG_Prof_Set( dm, LOADDATA_DOWNLOAD0, 0 );

	FieldSystemProc_SeqHoldEnd();
}

static void DG_Prof2(DGOTO_WORK *dm)
{
	DG_Prof_Set( dm, LOADDATA_DOWNLOAD1, 1 );
	
	FieldSystemProc_SeqHoldEnd();
}

static void DG_Prof3(DGOTO_WORK *dm)
{
	DG_Prof_Set( dm, LOADDATA_DOWNLOAD2, 2 );
	
	FieldSystemProc_SeqHoldEnd();
}

#include "application/br_sys.h"
#include "savedata/misc.h"

static int BR_MODE = 0;
static void DG_BRS( FIELDSYS_WORK* fsys, void* wk )
{
	const PROC_DATA* pData = BattleRecoder_ProcDataGet( BR_MODE );
	
	GameSystem_StartSubProc( fsys, pData, fsys ); 
}

static void Debug_BR_Start( DGOTO_WORK *dm, int mode )
{
	BR_MODE = mode;
	Debug_ChangeScheneReq( dm->fsys, DG_BRS, NULL, NULL );
}

static void DG_br_000( DGOTO_WORK *dm )
{
	Debug_BR_Start( dm, 0 );	
}

static void DG_br_001( DGOTO_WORK *dm )
{
	SaveData_Save( dm->fsys->savedata );
	FieldSystemProc_SeqHoldEnd();
//	Debug_BR_Start( dm, 1 );
}

static void DG_br_002( DGOTO_WORK *dm )
{
	SaveData_Save( dm->fsys->savedata );
	FieldSystemProc_SeqHoldEnd();
//	Debug_BR_Start( dm, 2 );
}

static void DG_br_003( DGOTO_WORK *dm )
{
	Debug_BR_Start( dm, 3 );
}

static void DG_br_004( DGOTO_WORK *dm )
{
	Debug_BR_Start( dm, 4 );
}

static void DG_br_005( DGOTO_WORK *dm )
{
	Debug_BR_Start( dm, 5 );
}

static void DG_br_006( DGOTO_WORK *dm )
{
	Debug_BR_Start( dm, 6 );
}

#endif
