//==============================================================================
/**
 * @file	d_contest.c
 * @brief	コンテストデバッグ呼び出し
 * @author	matsuda
 * @date	2008.01.31(木)
 */
//==============================================================================
#ifdef	PM_DEBUG

#include "common.h"
#include "system/fontproc.h"
#include "system/bmp_list.h"
#include "system/lib_pack.h"
#include "system/arc_util.h"
#include "system/wipe.h"
#include "system/snd_tool.h"
#include "demo/title.h"
#include "system/gamestart.h"
#include "field/game_init.h"

#include "system/main.h"

#include "msgdata/msg.naix"

#include "wifi/dwc_rap.h"
#include "wifi/dwc_test.h"

#include "common.h"
#include "system/pm_overlay.h"
#include "system/test_overlay.h"

#include "system/bmp_list.h"
#include "field/fieldmap.h"
#include "field/fieldsys.h"
#include "system/fontproc.h"
#include "contest/contest.h"

#include "field/script.h"
#include "../fielddata/script/connect_def.h"

#include "system/particle.h"
#include "particledata/particledata.h"
#include "particledata/particledata.naix"

#include "system/fontproc.h"
#include "system/msgdata.h"
#include "msgdata/msg.naix"
#include "msgdata/msg_debug_matsu.h"

#include "field/sysflag.h"

#include "debug/d_matsu.h"
#include "field/fld_bmp.h"
#include "application/app_tool.h"

#include "savedata/misc.h"

#include "application/balloon.h"
//#include "field/ev_mapchange.h"

#include "include/application/imageClip/imc_sys.h"

#include "wifi/dwc_rap.h"
#include "application/br_sys.h"



//============================================================================================
//	overlay定義
//============================================================================================

FS_EXTERN_OVERLAY( title );

//==============================================================================
//	グローバル変数宣言
//==============================================================================
///1:ローテーションなし。　0:ローテーションあり　2:ダンス回数制限なし
extern u8 DmatsuWork_DanceRotationOff;

//============================================================================================
//	定数定義
//============================================================================================
#define FCOLS_BLACK	(GF_PRINTCOLOR_MAKE(FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_NULL))
#define FCOLS_GREEN	(GF_PRINTCOLOR_MAKE(FBMP_COL_GREEN,FBMP_COL_GRN_SDW,FBMP_COL_WHITE))

#define	FONT_PAL_NO		( 0 )


#define	HEAPID_DEBUG_TITLE	HEAPID_DEBUG_APPLICATION

enum{
	DEBUG_GDS_START = 100,
	DEBUG_GDS_BV = DEBUG_GDS_START,		//BR_MODE_GDS_BV		
	DEBUG_GDS_BV_RANK,                  //BR_MODE_GDS_BV_RANK	
	DEBUG_GDS_RANK,                     //BR_MODE_GDS_RANK	
	DEBUG_GDS_DRESS,                    //BR_MODE_GDS_DRESS	
	DEBUG_GDS_BOX,                      //BR_MODE_GDS_BOX		
};

//==============================================================================
//	構造体定義
//==============================================================================
typedef struct {
	SAVEDATA *savedata;
	GF_BGL_INI * bgl;
	GF_BGL_BMPWIN	win;
	BMPLIST_WORK * lw;
	BMP_MENULIST_DATA *bmd;
	int select_id;
	int system_free;
	int main_seq;
	int sub_func_ok;
	
	int local_seq;
	TCB_PTR local_tcb;
	PROC *local_proc;
	
	u8 sub_seq;
	
	//-- コンテスト --//
	int debug_mode;
	int rank;
	int type;
	CONTEST_SYSTEM *consys;
}D_CONTEST_WORK;

///コンテストのランク、タイプ選択画面用ワーク
typedef struct{
	D_CONTEST_WORK *wk;

	GF_BGL_BMPWIN win;
	FIELDSYS_WORK *fsys;

	MSGDATA_MANAGER* msgman;
	STRBUF *rank_str;
	STRBUF *type_str;
	
	int first_wait;
	
	TCB_PTR tcb;
	s8 cursor_y;
	s8 rank;
	s8 type;
	s8 debug_mode;
}DEBUG_CONMODE_WORK;


//==============================================================================
//	型宣言
//==============================================================================
typedef void (*pDMFunc)(D_CONTEST_WORK *);

//============================================================================================
//	プロトタイプ宣言
//============================================================================================

static void VramBankSet(void);
static void BgInit( GF_BGL_INI * ini );
static void BmpListCB1( u32 param, u8 mode );
static void BmpListCB2( u8 index, u32 param, u8 y );
static void D_TitleMenuMemFree( GF_BGL_INI * ini );


static void SetFieldDebugStartProc(SAVEDATA * savedata);
static void SetTitleProc(SAVEDATA * savedata);

static void DebugMenuSystemFree(D_CONTEST_WORK *wk);
static void DM_ConModeSelectInit(D_CONTEST_WORK *wk, int debug_mode);
static void DM_ConModeSelectEnd(DEBUG_CONMODE_WORK *dcw);
static void DM_ConModeSelectMain(TCB_PTR tcb, void *work);
static void DM_ConModeSelectMsgWrite(DEBUG_CONMODE_WORK *dcw);
static void DM_ActingStart(D_CONTEST_WORK *wk);
static void DM_VisualStart(D_CONTEST_WORK *wk);
static void DM_ClipVisualStart(D_CONTEST_WORK *wk);
static void DM_DanceStart(D_CONTEST_WORK *wk);
static void DM_DanceNoneLimitStart(D_CONTEST_WORK *wk);
static void DM_DanceRotatinOn(D_CONTEST_WORK *wk);
static void DM_DanceRotationOff(D_CONTEST_WORK *wk);
static void DM_ResultStart(D_CONTEST_WORK *wk);
static void DebugContest_FieldConnectStart(D_CONTEST_WORK *wk, int debug_mode, int rank, int type);
static void GMEVENT_DebugConStart(TCB_PTR tcb, void *work);
static void ContestDebug_ImageClipDebugAccessorySet(CONTEST_SYSTEM *consys);
static void ContestDebug_ImageClipDebugAccessoryFree(CONTEST_SYSTEM *consys);

//============================================================================================
//	グローバル変数
//============================================================================================

//gmmファイルのIDを文字列指定に使う構造体宣言
typedef struct
{
	u32	strID;
	u32	param;
}LIST_DATA;

static const LIST_DATA ListData[] =
{
	{DMMSG_ACTIN, (u32)DM_ActingStart},
	{DMMSG_VISUAL, (u32)DM_VisualStart},
	{DMMSG_CLIP_VISUAL, (u32)DM_ClipVisualStart},
	{DMMSG_DANCE, (u32)DM_DanceStart},
	{DMMSG_ROTE_ON, (u32)DM_DanceRotatinOn},
	{DMMSG_ROTE_OFF, (u32)DM_DanceRotationOff},
	{DMMSG_DANCE_NONE_LIMIT, (u32)DM_DanceNoneLimitStart},
	{DMMSG_RESULT, (u32)DM_ResultStart},
};

static const BMPLIST_HEADER ListHeader = {
	NULL,
	NULL,					// カーソル移動ごとのコールバック関数
	NULL,					// 一列表示ごとのコールバック関数
	NULL,					// BMPウィンドウデータ
	NELEMS(ListData),		// リスト項目数
	6,						// 表示最大項目数
	0,						// ラベル表示Ｘ座標
	12,						// 項目表示Ｘ座標
	0,						// カーソル表示Ｘ座標
	0,						// 表示Ｙ座標
	FBMP_COL_WHITE,			//文字色
	FBMP_COL_BLACK,			//背景色
	FBMP_COL_BLK_SDW,		//文字影色
	0,						// 文字間隔Ｘ
	16,						// 文字間隔Ｙ
	BMPLIST_LRKEY_SKIP,		// ページスキップタイプ
	FONT_SYSTEM,			// 文字指定(本来は u8 だけど、そんなに作らないと思うので)
	0						// ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)
};


//==============================================================================
//	外部データ
//==============================================================================
extern const PROC_DATA ContestActinProcData;
extern const PROC_DATA ContestDanceProcData;
extern const PROC_DATA ContestVisualProcData;
extern const PROC_DATA ContestResultProcData;
extern const PROC_DATA IMC_SYS_Proc;

//==============================================================================
//	外部関数宣言
//==============================================================================
extern void Contest_ImageClipInitDataCreate(CONTEST_SYSTEM *consys);
extern void Contest_ImageClipInitDataFree(CONTEST_SYSTEM *consys);



//--------------------------------------------------------------------------------------------
/**
 * VRAM初期化
 *
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void VramBankSet(void)
{
	GF_BGL_DISPVRAM vramSetTable = {
		GX_VRAM_BG_128_C,				// メイン2DエンジンのBG
		GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット
		GX_VRAM_SUB_BG_32_H,			// サブ2DエンジンのBG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット
		GX_VRAM_OBJ_16_F,				// メイン2DエンジンのOBJ
		GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット
		GX_VRAM_SUB_OBJ_16_I,			// サブ2DエンジンのOBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット
		GX_VRAM_TEX_01_AB,				// テクスチャイメージスロット
		GX_VRAM_TEXPLTT_0123_E			// テクスチャパレットスロット
	};
	GF_Disp_SetBank( &vramSetTable );
}

//--------------------------------------------------------------------------------------------
/**
 * BG初期化
 *
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BgInit( GF_BGL_INI * ini )
{
	{	// BG SYSTEM
		GF_BGL_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
		};
		GF_BGL_InitBG( &BGsys_data );
	}

	{	// MAIN DISP BG0
		GF_BGL_BGCNT_HEADER MBg0_Data = {
			0, 0, 0x800, 0,
			GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xd000, GX_BG_CHARBASE_0x00000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, GF_BGL_FRAME0_M, &MBg0_Data, GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( ini, GF_BGL_FRAME0_M );
	}

	{	// MAIN DISP BG1
		GF_BGL_BGCNT_HEADER MBg1_Data = {
			0, 0, 0x800, 0,
			GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xd800, GX_BG_CHARBASE_0x00000,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, GF_BGL_FRAME1_M, &MBg1_Data, GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( ini, GF_BGL_FRAME1_M );
	}

	{	// MAIN DISP BG2
		GF_BGL_BGCNT_HEADER MBg2_Data = {
			0, 0, 0x800, 0,
			GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x00000,
			GX_BG_EXTPLTT_23, 2, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, GF_BGL_FRAME2_M, &MBg2_Data, GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( ini, GF_BGL_FRAME2_M );
	}

	{	// MAIN DISP BG3
		GF_BGL_BGCNT_HEADER MBg3_Data = {
			0, 0, 0x800, 0,
			GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000,
			GX_BG_EXTPLTT_23, 3, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, GF_BGL_FRAME3_M, &MBg3_Data, GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( ini, GF_BGL_FRAME3_M );
	}
}


//--------------------------------------------------------------------------------------------
/**
 * メモリ開放
 *
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void D_TitleMenuMemFree( GF_BGL_INI * ini )
{
	// BGL開放
	GF_BGL_BGControlExit( ini, GF_BGL_FRAME0_M );
	GF_BGL_BGControlExit( ini, GF_BGL_FRAME1_M );
	GF_BGL_BGControlExit( ini, GF_BGL_FRAME2_M );
	GF_BGL_BGControlExit( ini, GF_BGL_FRAME3_M );
/*
	GF_BGL_BGControlExit( ini, GF_BGL_FRAME0_S );
	GF_BGL_BGControlExit( ini, GF_BGL_FRAME1_S );
*/
}


//--------------------------------------------------------------------------------------------
/**
 * @brief	人名選択プロセス：BMPMENULISTDATAの生成
 * @param	bmd		BMP_MENULIST_DATA構造体へのポインタ
 */
//--------------------------------------------------------------------------------------------
static	void	BMP_MENULIST_Make(BMP_MENULIST_DATA *bmd)
{
	int				i;
	MSGDATA_MANAGER	*man;

	man=MSGMAN_Create(MSGMAN_TYPE_NORMAL,ARC_MSG,NARC_msg_debug_matsu_dat,HEAPID_DEBUG_TITLE);

	for(i=0;i<NELEMS(ListData);i++){
		BMP_MENULIST_AddArchiveString(bmd,man,ListData[i].strID,ListData[i].param);
	}

	MSGMAN_Delete(man);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	人名選択プロセス：初期化
 * @param	proc	プロセスへのポインタ
 * @param	seq		シーケンス用ワークへのポインタ
 * @return	PROC_RES_CONTINUE	動作継続中
 * @return	PROC_RES_FINISH		動作終了
 */
//--------------------------------------------------------------------------------------------
static PROC_RESULT DebugContestMenuProc_Init(PROC * proc, int * seq)
{
	sys_CreateHeap(HEAPID_BASE_APP,HEAPID_WORLD,0x1c000);
	sys_CreateHeap( HEAPID_BASE_APP, HEAPID_DEBUG_TITLE, 0x10000);
	{
		D_CONTEST_WORK * wk;
		//int id = Debug_GetMyID();

		wk = PROC_AllocWork(proc, sizeof(D_CONTEST_WORK), HEAPID_DEBUG_TITLE);
		MI_CpuClear8(wk, sizeof(D_CONTEST_WORK));
		wk->savedata = ((MAINWORK*)PROC_GetParentWork(proc))->savedata;

		wk->bgl = GF_BGL_BglIniAlloc( HEAPID_DEBUG_TITLE );

		GF_Disp_GX_VisibleControlInit();
		VramBankSet();
		BgInit( wk->bgl );

		MSG_PrintInit();

		SystemFontPaletteLoad( PALTYPE_MAIN_BG, FONT_PAL_NO * 32, HEAPID_DEBUG_TITLE );

		{
			BMPLIST_HEADER	list_h;

			wk->bmd=BMP_MENULIST_Create(NELEMS(ListData),HEAPID_DEBUG_TITLE);
			BMP_MENULIST_Make(wk->bmd);

			GF_BGL_BmpWinAdd(
				wk->bgl, &wk->win, GF_BGL_FRAME0_M, 1, 1, 18, 18, FONT_PAL_NO, 1 );

			list_h = ListHeader;
			list_h.list = wk->bmd;
			list_h.win = &wk->win;
			wk->lw = BmpListSet( &list_h, 0, 0, HEAPID_DEBUG_TITLE );
		}
	}

		WIPE_ResetBrightness( WIPE_DISP_MAIN );
		WIPE_ResetBrightness( WIPE_DISP_SUB );

		GF_Disp_DispOn();

	Snd_BgmChannelSetAndReverbSet( 0 );	//使用可能チャンネル操作、リバーブ設定(クリア)

	return PROC_RES_FINISH;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	人名選択プロセス：メイン
 * @param	proc	プロセスへのポインタ
 * @param	seq		シーケンス用ワークへのポインタ
 * @return	PROC_RES_CONTINUE	動作継続中
 * @return	PROC_RES_FINISH		動作終了
 */
//--------------------------------------------------------------------------------------------
static PROC_RESULT DebugContestMenuProc_Main(PROC * proc, int * seq)
{
	D_CONTEST_WORK * wk = PROC_GetWork(proc);
	u32	ret;

	//サブPROCが有る場合は呼び出し
	if(wk->local_proc != NULL){
		if(ProcMain(wk->local_proc) == TRUE){
			PROC_Delete(wk->local_proc);
			wk->local_proc = NULL;
		}
	}
	
	switch(wk->main_seq){
	case 0:
		ret = BmpListMain( wk->lw );

		switch( ret ){
		case BMPLIST_NULL:
			break;
		case BMPLIST_CANCEL:
			wk->main_seq = 0xff;
			break;
		default:
			{
				pDMFunc func = (pDMFunc)ret;
				func(wk);
				wk->main_seq++;
			}
			break;
		}
		break;
	case 1:
		if(wk->sub_func_ok == TRUE){
			wk->main_seq++;
		}
		break;
	default:
		wk->select_id = -1;
		return PROC_RES_FINISH;
	}
	return PROC_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	人名選択プロセス：終了
 * @param	proc	プロセスへのポインタ
 * @param	seq		シーケンス用ワークへのポインタ
 * @return	PROC_RES_CONTINUE	動作継続中
 * @return	PROC_RES_FINISH		動作終了
 */
//--------------------------------------------------------------------------------------------
static PROC_RESULT DebugContestMenuProc_End(PROC * proc, int * seq)
{
	D_CONTEST_WORK * wk = PROC_GetWork(proc);

	DebugMenuSystemFree(wk);

	((MAINWORK*)PROC_GetParentWork(proc))->select_id = wk->select_id;
	if (wk->select_id != -1) {
		//デバッグゲーム開始
		Main_SetNextProc(OVERLAY_ID_GAMESTART, &DebugNewGameStartProcData);
	} else {
		//タイトルに戻る
		Main_SetNextProc(FS_OVERLAY_ID(title), &TitleProcData);
	}
	PROC_FreeWork(proc);

	sys_DeleteHeap(HEAPID_DEBUG_TITLE);
	sys_DeleteHeap(HEAPID_WORLD);
	return PROC_RES_FINISH;
}

static void DebugMenuSystemFree(D_CONTEST_WORK *wk)
{
	if(wk->system_free == TRUE){
		return;
	}
	wk->system_free = TRUE;
	
	BMP_MENULIST_Delete(wk->bmd);
	BmpListExit( wk->lw, NULL, NULL );
	GF_BGL_BmpWinOff( &wk->win );
	GF_BGL_BmpWinDel( &wk->win );
	D_TitleMenuMemFree( wk->bgl );
	sys_FreeMemoryEz( wk->bgl );
}

//==============================================================================
//==============================================================================
//==============================================================================

//--------------------------------------------------------------
/**
 * @brief   コンテストのランク、タイプ選択画面初期化
 *
 * @param   wk		
 * @param   debug_mode		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
static void DM_ConModeSelectInit(D_CONTEST_WORK *wk, int debug_mode)
{
	DEBUG_CONMODE_WORK *dcw;
	
	dcw = sys_AllocMemory(HEAPID_DEBUG_TITLE, sizeof(DEBUG_CONMODE_WORK));
	MI_CpuClear8(dcw, sizeof(DEBUG_CONMODE_WORK));
	dcw->debug_mode = debug_mode;
	dcw->wk = wk;
	
	//BMPウィンドウを追加
	GF_BGL_BmpWinAdd(wk->bgl, &dcw->win, GF_BGL_FRAME0_M, 2, 2, 12, 4,
			FONT_PAL_NO, 0x4000/0x20);
	GF_BGL_BmpWinDataFill(&dcw->win,APP_WINCLR_COL(FBMP_COL_WHITE));
	GF_BGL_BmpWinOn(&dcw->win);

	//文字セット追加
	dcw->msgman = MSGMAN_Create(MSGMAN_TYPE_NORMAL, ARC_MSG,
			NARC_msg_debug_matsu_dat, HEAPID_DEBUG_TITLE);
	dcw->rank_str = STRBUF_Create(30, HEAPID_DEBUG_TITLE);
	dcw->type_str = STRBUF_Create(30, HEAPID_DEBUG_TITLE);
	
	DM_ConModeSelectMsgWrite(dcw);
	
	dcw->tcb = TCB_Add(DM_ConModeSelectMain, dcw, 100);
}

///コンテストのランク、タイプ選択終了処理
static void DM_ConModeSelectEnd(DEBUG_CONMODE_WORK *dcw)
{
	GF_BGL_BmpWinOff(&dcw->win);
	GF_BGL_BmpWinDel(&dcw->win);

	STRBUF_Delete(dcw->rank_str);
	STRBUF_Delete(dcw->type_str);
	MSGMAN_Delete(dcw->msgman);
	
	TCB_Delete(dcw->tcb);
	sys_FreeMemoryEz(dcw);
}

///コンテストのランク、タイプ選択メイン
static void DM_ConModeSelectMain(TCB_PTR tcb, void *work)
{
	DEBUG_CONMODE_WORK *dcw = work;
	D_CONTEST_WORK *wk = dcw->wk;
	int add_num;
	int jyoudan_max, gedan_max;
	
	if(dcw->first_wait == 0){	//トリガーがタスク生成同一フレームで取れてしまうのでウェイト入れる
		dcw->first_wait++;
		return;
	}
	
	if(dcw->debug_mode < DEBUG_GDS_START){
		jyoudan_max = CONRANK_MAX;
	}
	else{
		jyoudan_max = BR_MODE_GDS_BOX - BR_MODE_GDS_BV + 1;
	}
	gedan_max = CONTYPE_MAX;
	
	if((dcw->debug_mode < DEBUG_GDS_START) && (sys.trg & (PAD_KEY_DOWN | PAD_KEY_UP))){
		dcw->cursor_y ^= 1;
		DM_ConModeSelectMsgWrite(dcw);
	}
	
	if(sys.trg & (PAD_KEY_LEFT|PAD_KEY_RIGHT)){
		add_num = (sys.trg & PAD_KEY_LEFT) ? -1 : 1;
		if(dcw->cursor_y == 0){
			dcw->rank += add_num;
			if(dcw->rank<0){
				dcw->rank = jyoudan_max-1;
			}
			else if(dcw->rank >= jyoudan_max){
				dcw->rank = 0;
			}
		}
		else{
			dcw->type += add_num;
			if(dcw->type<0){
				dcw->type = gedan_max-1;
			}
			else if(dcw->type >= gedan_max){
				dcw->type = 0;
			}
		}
		DM_ConModeSelectMsgWrite(dcw);
	}
	
	if(sys.trg & (PAD_BUTTON_A|PAD_BUTTON_B|PAD_BUTTON_START)){
		FIELDSYS_WORK *fsys;
		int rank,type,debug_mode;
		
		fsys = dcw->fsys;
		rank = dcw->rank;
		type = dcw->type;
		debug_mode = dcw->debug_mode;
		
		DM_ConModeSelectEnd(dcw);
		
		if(debug_mode >= DEBUG_GDS_START){	//GDS
	//		DMRes_BalloonStart(dcw->fsys, 
	//			DMATSU_PROC_FLAG_GDS_RECORDER, dcw->rank + BR_MODE_GDS_BV);
		}
		else{	//コンテスト
			DebugContest_FieldConnectStart(wk, debug_mode, rank, type);
		}
		return;
	}
}

///ランク、モードのメッセージ再描画
static void DM_ConModeSelectMsgWrite(DEBUG_CONMODE_WORK *dcw)
{
	GF_PRINTCOLOR rank_col, type_col;
	
	if(dcw->cursor_y == 0){
		rank_col = FCOLS_GREEN;
		type_col = FCOLS_BLACK;
	}
	else{
		rank_col = FCOLS_BLACK;
		type_col = FCOLS_GREEN;
	}

	if(dcw->debug_mode < DEBUG_GDS_START){
		MSGMAN_GetString(dcw->msgman, DMMSG_RANK0001 + dcw->rank, dcw->rank_str);
		MSGMAN_GetString(dcw->msgman, DMMSG_TYPE0001 + dcw->type, dcw->type_str);
		
		GF_STR_PrintColor(&dcw->win, FONT_SYSTEM,
			dcw->rank_str, 0,0, MSG_NO_PUT, rank_col, NULL);
		GF_STR_PrintColor(&dcw->win, FONT_SYSTEM,
			dcw->type_str, 0,16, MSG_NO_PUT, type_col, NULL);
	}
	else{
		MSGMAN_GetString(dcw->msgman, DMMSG_GDS_MODE_0000 + dcw->rank, dcw->rank_str);
		GF_STR_PrintColor(&dcw->win, FONT_SYSTEM,
			dcw->rank_str, 0,0, MSG_NO_PUT, rank_col, NULL);
	}

	GF_BGL_BmpWinOn(&dcw->win);
}

//--------------------------------------------------------------
/**
 * @brief   コンテスト：演技力部門開始
 *
 * @param   wk		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
static void DM_ActingStart(D_CONTEST_WORK *wk)
{
//	DebugContest_FieldConnectStart(wk->fsys, DEBUG_CON_ACTIN, 
//		CONRANK_NORMAL, gf_rand() % CONTYPE_MAX);
	DM_ConModeSelectInit(wk, DEBUG_CON_ACTIN);
}

//--------------------------------------------------------------
/**
 * @brief   コンテスト：ビジュアル部門開始
 *
 * @param   wk		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
static void DM_VisualStart(D_CONTEST_WORK *wk)
{
//	DebugContest_FieldConnectStart(wk->fsys, DEBUG_CON_VISUAL, 
//		CONRANK_NORMAL, gf_rand() % CONTYPE_MAX);
	DM_ConModeSelectInit(wk, DEBUG_CON_VISUAL);
}

//--------------------------------------------------------------
/**
 * @brief   コンテスト：イメージクリップ＞ビジュアル部門開始
 *
 * @param   wk		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
static void DM_ClipVisualStart(D_CONTEST_WORK *wk)
{
//	DebugContest_FieldConnectStart(wk->fsys, DEBUG_CON_CLIP_VISUAL, 
//		CONRANK_NORMAL, gf_rand() % CONTYPE_MAX);
	DM_ConModeSelectInit(wk, DEBUG_CON_CLIP_VISUAL);
}

//--------------------------------------------------------------
/**
 * @brief   コンテスト：ダンス部門開始
 *
 * @param   wk		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
static void DM_DanceStart(D_CONTEST_WORK *wk)
{
	DmatsuWork_DanceRotationOff = 0;
#if 0
	DebugContest_FieldConnectStart(wk->fsys, DEBUG_CON_DANCE, 
		CONRANK_NORMAL, gf_rand() % CONTYPE_MAX);
#endif

	DM_ConModeSelectInit(wk, DEBUG_CON_DANCE);
}

//--------------------------------------------------------------
/**
 * @brief   コンテスト：ダンス部門開始(全ての四分音符でダンス)
 *
 * @param   wk		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
static void DM_DanceNoneLimitStart(D_CONTEST_WORK *wk)
{
	DmatsuWork_DanceRotationOff = 2;
	DebugContest_FieldConnectStart(wk, DEBUG_CON_DANCE, 
		CONRANK_NORMAL, gf_rand() % CONTYPE_MAX);
}

//--------------------------------------------------------------
/**
 * @brief   コンテスト：ダンス部門開始(ローテーションあり)
 *
 * @param   wk		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
static void DM_DanceRotatinOn(D_CONTEST_WORK *wk)
{
	DebugContest_FieldConnectStart(wk, DEBUG_CON_DANCE, 
		CONRANK_NORMAL, gf_rand() % CONTYPE_MAX);
}

//--------------------------------------------------------------
/**
 * @brief   コンテスト：ダンス部門開始(ローテーションなし)
 *
 * @param   wk		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
static void DM_DanceRotationOff(D_CONTEST_WORK *wk)
{
	DmatsuWork_DanceRotationOff = 1;
	DebugContest_FieldConnectStart(wk, DEBUG_CON_DANCE, 
		CONRANK_NORMAL, gf_rand() % CONTYPE_MAX);
}

//--------------------------------------------------------------
/**
 * @brief   コンテスト：結果発表開始
 *
 * @param   wk		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
static void DM_ResultStart(D_CONTEST_WORK *wk)
{
	DebugContest_FieldConnectStart(wk, DEBUG_CON_RESULT, 
		CONRANK_NORMAL, gf_rand() % CONTYPE_MAX);
}

//--------------------------------------------------------------
/**
 * @brief   デバッグコンテスト呼び出し
 *
 * @param   fsys		
 * @param   debug_mode		DEBUG_CON_???
 */
//--------------------------------------------------------------
static void DebugContest_FieldConnectStart(D_CONTEST_WORK *wk, int debug_mode, int rank, int type)
{
	DebugMenuSystemFree(wk);

	wk->debug_mode = debug_mode;
	wk->rank = rank;
	wk->type = type;
	wk->local_seq = 0;
	wk->local_tcb = TCB_Add(GMEVENT_DebugConStart, wk, 10);
}

//-----------------------------------------------------------------------------
/**
 * @brief	コンテスト：演技力部門イベントシーケンス
 * @param	event		イベント制御ワークへのポインタ
 * @retval	TRUE		イベント終了
 * @retval	FALSE		イベント継続中
 *
 * エンカウントエフェクト→→フィールド復帰を行う。
 * ゲームオーバー処理への分岐も入る（予定）
 */
//-----------------------------------------------------------------------------
static void GMEVENT_DebugConStart(TCB_PTR tcb, void *work)
{
	D_CONTEST_WORK *wk = work;
	
	if(wk->local_proc != NULL){
		return;
	}

	switch (wk->local_seq) {
	case 0:
		//※check	入り口がここしかないので、とりあえずここでコンテスト全体ヒープ作成
		{
			POKEMON_PARAM * poke;
			STRBUF *myname;
			const MYSTATUS *my_status;
			CONTEST_INIT_DATA cid;
			
			poke = PokeParty_GetMemberPointer( SaveData_GetTemotiPokemon(wk->savedata), 0);
			my_status = SaveData_GetMyStatus(wk->savedata);
			myname = MyStatus_CreateNameString(my_status, HEAPID_WORLD);
			
			cid.type = wk->type;//gf_rand() % CONTYPE_MAX;//CONTYPE_STYLE;
			cid.rank = wk->rank;//CONRANK_NORMAL;
			cid.mode = CONMODE_GRAND;
			cid.my_pp = poke;
			cid.player_name_str = myname;
			cid.my_status = my_status;
			cid.imc_save = SaveData_GetImcSaveData(wk->savedata);
			cid.config = SaveData_GetConfig(wk->savedata);
			cid.sv = wk->savedata;
			wk->consys = Contest_SystemCreate(&cid);
			
			STRBUF_Delete(myname);
		}
		
		wk->local_seq ++;
		break;
	case 1:		//それぞれの部門を呼び出し
		switch(wk->debug_mode){
		case DEBUG_CON_ACTIN:
			wk->local_proc = PROC_Create(&ContestActinProcData, wk->consys, HEAPID_DEBUG_TITLE);
			wk->local_seq++;
			break;
		case DEBUG_CON_VISUAL:
			//デバッグモードでは自分のクリップデータだけないので、ここでセット
			ImcSaveData_SetContestPokeData_Easy(wk->consys->c_game.imc_data[0], 
				wk->consys->c_game.pp[0], -1);
			ImcSaveData_SetContestBgId(wk->consys->c_game.imc_data[0], 0);
			ImcSaveData_SetContestRank(wk->consys->c_game.imc_data[0], 
				wk->consys->c_game.rank);

			wk->local_proc = PROC_Create(&ContestVisualProcData, wk->consys, HEAPID_DEBUG_TITLE);
			wk->local_seq++;
			break;
		case DEBUG_CON_CLIP_VISUAL:
			switch(wk->sub_seq){
			case 0:
				Contest_ImageClipInitDataCreate(wk->consys);
				ContestDebug_ImageClipDebugAccessorySet(wk->consys);	//デバッグデータセット
				wk->local_proc = PROC_Create(&IMC_SYS_Proc, wk->consys->icpw, HEAPID_DEBUG_TITLE);
				wk->sub_seq++;
				break;
			case 1:
				ContestDebug_ImageClipDebugAccessoryFree(wk->consys);	//デバッグデータ開放
				Contest_ImageClipInitDataFree(wk->consys);
				wk->sub_seq++;
				break;
			default:
				wk->local_proc = PROC_Create(&ContestVisualProcData, 
					wk->consys, HEAPID_DEBUG_TITLE);
				wk->sub_seq = 0;
				wk->local_seq++;
				break;
			}
			break;
		case DEBUG_CON_DANCE:
			//デバッグモードでは自分のクリップデータだけないので、ここでセット
			ImcSaveData_SetContestPokeData_Easy(wk->consys->c_game.imc_data[0], 
				wk->consys->c_game.pp[0], -1);
			ImcSaveData_SetContestBgId(wk->consys->c_game.imc_data[0], 0);
			ImcSaveData_SetContestRank(wk->consys->c_game.imc_data[0], 
				wk->consys->c_game.rank);

			wk->local_proc = PROC_Create(&ContestDanceProcData, wk->consys, HEAPID_DEBUG_TITLE);
			wk->local_seq++;
			break;
		case DEBUG_CON_RESULT:
			wk->local_proc = PROC_Create(&ContestResultProcData, wk->consys, HEAPID_DEBUG_TITLE);
			wk->local_seq++;
			break;
		}
		break;
	case 2:
		//※check　出口がここしかないので、とりあえずここでコンテスト全体ヒープ解放
		Contest_SystemExit(wk->consys);
		
		wk->local_seq ++;
		break;
	default:
		wk->sub_func_ok = TRUE;
		wk->local_tcb = NULL;
		TCB_Delete(tcb);
		return;
	}
}

//--------------------------------------------------------------
/**
 * @brief   イメージクリップデバッグ用にアクセサリーを全てセット
 *
 * @param   consys		コンテストシステムワークへのポインタ
 *
 * Contest_ImageClipInitDataCreateの関数後に呼び出してください
 */
//--------------------------------------------------------------
static void ContestDebug_ImageClipDebugAccessorySet(CONTEST_SYSTEM *consys)
{
	IMC_CONTEST_PROC_WORK *icpw;
	IMC_ITEM_SAVEDATA *imcsave;
	
	GF_ASSERT(consys->icpw != NULL);
	icpw = consys->icpw;
	
	imcsave = ImcSaveData_ItemAllocWork(HEAPID_WORLD);
	Debug_ImcSaveData_SetItemData(imcsave);

	icpw->cp_imc_item = imcsave;
}

//--------------------------------------------------------------
/**
 * @brief   デバッグ用にセットしたアクセサリ領域を開放
 *
 * @param   consys		コンテストシステムワークへのポインタ
 *
 * Contest_ImageClipInitDataFree関数を呼ぶよりも先にこれでデバッグ領域の解放をしてください
 */
//--------------------------------------------------------------
static void ContestDebug_ImageClipDebugAccessoryFree(CONTEST_SYSTEM *consys)
{
	IMC_CONTEST_PROC_WORK *icpw;
	IMC_ITEM_SAVEDATA *imcsave;
	icpw = consys->icpw;
	imcsave = (void*)icpw->cp_imc_item;
	sys_FreeMemoryEz(imcsave);
}


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
const PROC_DATA DebugContestMenuProcData = {
	DebugContestMenuProc_Init,
	DebugContestMenuProc_Main,
	DebugContestMenuProc_End,
	NO_OVERLAY_ID
};

#endif	//PM_DEBUG

