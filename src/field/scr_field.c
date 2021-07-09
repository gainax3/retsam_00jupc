//============================================================================================
/**
 * @file	scr_field.c
 * @bfief	スクリプトで使用するプログラム(非常駐フィールドプログラム)
 * @author	Satoshi Nohara
 * @date	07.12.03
 *
 * リスト処理の似たものがscr_wazaoshie.cにも存在している
 */
//============================================================================================
#include "common.h"
#include "gflib/msg_print.h"
#include "system/lib_pack.h"
#include "system/window.h"
#include "savedata/frontier_savedata.h"
#include "savedata/frontier_ex.h"
#include "savedata/savedata_def.h"
#include "savedata/misc.h"
#include "fieldsys.h"
#include "zonedata.h"
#include "mapdefine.h"
#include "poketool/monsno.h"
#include "field/eventflag.h"							//EventWork_CheckEventFlag
#include "field/evwkdef.h"								//FH_FLAG_START
#include "field/fieldobj.h"
#include "script.h"										//ID_HIDE_ITEM_SCR_OFFSET
#include "scr_tool.h"

#include "system/wipe.h"

#include "scrcmd_def.h"
#include "ev_win.h"
#include "scr_field.h"
#include "msgdata\msg.naix"								//NARC_msg_??_dat
#include "msgdata\msg_ev_win.h"	
#include "src/application/zukanlist/zkn_data/zukan_data.naix"

#include "system/msgdata.h"								//MSGMAN_TYPE_DIRECT
#include "system/pm_str.h"
#include "system/fontproc.h"
#include "system/snd_tool.h"
#include "system/bmp_list.h"
#include "fieldmap.h"
#include "fld_bmp.h"

#include "poketool/boxdata.h"
#include "savedata/sodateyadata.h"
#include "battle/battle_server.h"

#include "../frontier/frontier_def.h"
#include "syswork.h"

#include	"itemtool/item.h"
#include	"itemtool/itemequip.h"
#include	"itemtool/itemsym.h"

#include "communication/comm_wifihistory.h"

#include "../fielddata/eventdata/zone_c04evc.h"
#include "../../include/pl_bugfix.h"

//==============================================================================================
//
//	定義
//
//==============================================================================================
#define ZKN_SORTDATA_ONESIZE		(sizeof(u16))


//============================================================================================
//
//	プロトタイプ宣言	
//
//============================================================================================
static u16* ZKN_SORTDATA_Get( int heap, int idx, int* p_arry_num );
BOOL EvCmdSeisekiBmpListStart( VM_MACHINE * core );
static BOOL EvSeisekiSelWinWait(VM_MACHINE * core);
static void	GetMezameruPowerParam(POKEMON_PARAM *pp,int *power,int *type);
BOOL EvCmdSetFavoritePoke( VM_MACHINE * core );
BOOL EvCmdGetFavoritePoke( VM_MACHINE * core );
BOOL EvCmdVillaListCheck( VM_MACHINE * core );


//============================================================================================
//
//	データ
//
//============================================================================================
static u16 arc_tbl[] = {
	NARC_zukan_data_zkn_sort_a_dat,				//あ
	NARC_zukan_data_zkn_sort_ka_dat,			//か
	NARC_zukan_data_zkn_sort_sa_dat,			//さ
	NARC_zukan_data_zkn_sort_ta_dat,			//た
	NARC_zukan_data_zkn_sort_na_dat,			//な
	NARC_zukan_data_zkn_sort_ha_dat,			//は
	NARC_zukan_data_zkn_sort_ma_dat,			//ま
	NARC_zukan_data_zkn_sort_ra_dat,			//ら		//注意！　図鑑の並びにあわせた
	NARC_zukan_data_zkn_sort_yawa_dat,			//やわ		//注意！　図鑑の並びにあわせた
};
#define ARC_TBL_MAX		( NELEMS(arc_tbl) )


//==============================================================================================
//
//	ev_winのリスト処理を元にしている
//
//==============================================================================================
//有効で、表示数より項目数が多いリスト処理は、上下にスクロールカーソルを表示
//グラフィックデータがないので、その辺は仮です。
//#define EV_WIN_SCROLL_CURSOR

#ifdef EV_WIN_SCROLL_CURSOR	//070228
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "system/clact_tool.h"
#include "field_clact.h"
#include "field/ranking.naix"			//グラフィックデータがないので仮
#endif


//==============================================================================================
//
//	プロトタイプ宣言
//
//==============================================================================================
static void EvWin_MsgManSet( SEISEKI_WORK* wk, MSGDATA_MANAGER* msgman );
static void EvWin_Init( FIELDSYS_WORK* fsys, SEISEKI_WORK* wk, u8 x, u8 y, u8 cursor, u8 cancel, u16* work, WORDSET* wordset, GF_BGL_BMPWIN* talk_bmpwin, MSGDATA_MANAGER* msgman, u16* lp_work, u16* cp_work );

//BMPリスト
SEISEKI_WORK * CmdSeisekiBmpList_Init( FIELDSYS_WORK* fsys, u8 x, u8 y, u8 cursor, u8 cancel, u16* work, WORDSET* wordset, GF_BGL_BMPWIN* talk_bmpwin, MSGDATA_MANAGER* msgman, u16* lp_work, u16* cp_work);
void CmdSeisekiBmpList_MakeList( SEISEKI_WORK* wk, u32 msg_id, u32 talk_msg_id, u32 param  );
static void CmdSeisekiBmpList_Start( SEISEKI_WORK* wk );

static void BmpList_list_make( SEISEKI_WORK* wk, u32 msg_id, u32 talk_msg_id, u32 param  );
static u32 BmpList_length_get( SEISEKI_WORK* wk );
static void BmpList_h_default_set(SEISEKI_WORK* wk);
static void	BmpList_CallBack( BMPLIST_WORK* lw, u32 param, u8 y );
static void BmpList_CursorMoveCallBack( BMPLIST_WORK* wk, u32 param, u8 mode );
static void EvBmpList_MainTCB( TCB_PTR tcb, void* wk );
static void EvBmpList_Del( SEISEKI_WORK* wk );

#ifdef EV_WIN_SCROLL_CURSOR	//070228
static void EvWin_ActorInit( SEISEKI_WORK* wk );
static void EvWin_ActorRelease( SEISEKI_WORK* wk );
#endif


//--------------------------------------------------------------
/**
 * @brief	BMPリスト	初期化、リスト作成、開始
 *
 * @param	none
 *
 * @retval	1
 */
//--------------------------------------------------------------
BOOL EvCmdSeisekiBmpListStart( VM_MACHINE * core )
{
	LOAD_RESULT load_ret;
	FRONTIER_EX_SAVEDATA* fes;
	u16 renshou;
	u16* p_data;
	int i,sort_data_count;
	MSGDATA_MANAGER* man;
	MSGDATA_MANAGER* ev_win_man;
	FIELDSYS_WORK* fsys	= core->fsys;
	SEISEKI_WORK* seiseki_win;
	WORDSET** wordset			= GetEvScriptWorkMemberAdrs( fsys, ID_EVSCR_WORDSET );
	u16 type					= VMGetWorkValue( core );
	u16 param2					= VMGetWorkValue( core );
	u16 wk_id					= VMGetU16( core );
	u16 wk_id2					= VMGetU16( core );				//lp_work用
	u16 wk_id3					= VMGetU16( core );				//cp_work用

	//仮想マシンの汎用レジスタにワークのIDを格納
	core->reg[0] = wk_id;

	//ポケモン名
	man = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, NARC_msg_monsname_dat, HEAPID_EVENT );

	//evwin初期化
	seiseki_win	= CmdSeisekiBmpList_Init(	fsys, 20, 1, 0, 1, 
									GetEventWorkAdrs(fsys,wk_id), *wordset, 
									GetEvScriptWorkMemberAdrs(core->fsys,ID_EVSCR_MSGWINDAT), man, 
									GetEventWorkAdrs(fsys,wk_id2),
									GetEventWorkAdrs(fsys,wk_id3) );

	//フロンティア外部データのロード
	fes = FrontierEx_Load( fsys->savedata, HEAPID_WORLD, &load_ret );

	//データ正常読み込み
	if( load_ret == LOAD_RESULT_OK ){

		//リスト作成
		p_data = ZKN_SORTDATA_Get( HEAPID_EVENT, arc_tbl[param2], &sort_data_count );
		for( i=0; i < sort_data_count ;i++ ){

			OS_Printf( "p_data[%d] = %d\n", i, p_data[i] );
			OS_Printf( "renshou = %d\n", renshou );

			renshou = FrontierEx_StageRenshou_Get(	fsys->savedata, fes, 
													StageScr_GetExMaxWinRecordID(type), p_data[i] );
	
			//連勝記録が存在したらリスト作成
			if( renshou != 0 ){
				CmdSeisekiBmpList_MakeList(seiseki_win, p_data[i], EV_WIN_TALK_MSG_NONE, p_data[i]);
			}
		}

		sys_FreeMemoryEz( p_data );
	}

	if( fes != NULL ){
		sys_FreeMemoryEz( fes );
	}

#if 1
	//ev_win.gmmから「もどる」を使用している
	
	ev_win_man = MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, NARC_msg_ev_win_dat, HEAPID_EVENT );
	EvWin_MsgManSet( seiseki_win, ev_win_man );
	CmdSeisekiBmpList_MakeList(	seiseki_win, msg_ev_win_013, 
								EV_WIN_TALK_MSG_NONE, EV_WIN_B_CANCEL );
	MSGMAN_Delete( ev_win_man );

	//一応戻す
	EvWin_MsgManSet( seiseki_win, man );
#endif

	//リスト開始
	CmdSeisekiBmpList_Start( seiseki_win );
	
	VM_SetWait( core, EvSeisekiSelWinWait );

	MSGMAN_Delete( man );
	return 1;
}

//ウェイト関数
static BOOL EvSeisekiSelWinWait(VM_MACHINE * core)
{
	FIELDSYS_WORK* fsys = core->fsys;
	u16* ret_wk = GetEventWorkAdrs( fsys, core->reg[0] );	//注意！

	if( *ret_wk == EV_WIN_NOTHING ){
		return FALSE;	//継続
	}

	return TRUE;		//終了
}

//----------------------------------------------------------------------------
/**
 *	@brief	ソートﾃﾞｰﾀ取得
 *
 *	@param	heap		ヒープ
 *	@param	idx			ﾃﾞｰﾀｲﾝﾃﾞｯｸｽ
 *	@param	p_arry_num	配列要素数取得先
 *
 *	@return	ﾃﾞｰﾀバッファ		要素数はp_arry_numに格納
 */
//-----------------------------------------------------------------------------
static u16* ZKN_SORTDATA_Get( int heap, int idx, int* p_arry_num )
{
	u32 size;
	u16* p_buf;
	
	//読み込み
	p_buf = ArcUtil_LoadEx( ARC_ZUKAN_DATA, idx, FALSE, 
							heap, ALLOC_TOP, &size );

	*p_arry_num = size / ZKN_SORTDATA_ONESIZE;
	return p_buf;
}


//==============================================================================================
//
//	定義
//
//==============================================================================================
#define EV_WIN_FONT				(FONT_SYSTEM)	//フォント指定

#define EVWIN_MSG_BUF_SIZE		(40*2)			//メッセージバッファサイズ

#define EV_WIN_LIST_MAX			(120)			//BMPリスト項目の最大数
#define	EV_WIN_MSG_MAX			(120)			//MSGバッファの最大数

//BMPリストヘッダー定義
#define EV_LIST_LINE			(8)				//表示最大項目数
#define EV_LIST_RABEL_X			(1)				//ラベル表示Ｘ座標
#define EV_LIST_DATA_X			(12)			//項目表示Ｘ座標
#define EV_LIST_CURSOR_X		(2)				//カーソル表示Ｘ座標
#define EV_LIST_LINE_Y			(1)				//表示Ｙ座標

//カーソル幅
#define EV_WIN_CURSOR_WIDTH		(12)			//カーソル幅

#define EV_WIN_DEFAULT_WAIT		(3)				//キー操作がすぐに入らないように基本ウェイト

// ↓ここから使って下さい！！
#define	EVWIN_FREE_CGX			( 1 )			//フリーのキャラ位置

#ifdef EV_WIN_SCROLL_CURSOR	//070228
#define CURSOR_ANMNO				(0)
#define CURSOR_SPRI					(0)
#define CURSOR_PALT					(1)

#define EVWIN_CHR_H_ID_CURSOR		(13528)
#define EVWIN_PAL_H_ID				(13528)
#define EVWIN_CEL_H_ID_CURSOR		(13528)
#define EVWIN_ANM_H_ID_CURSOR		(13528)

#define EVWIN_ACTMAX				(2)			//アクター数(上向きと下向きの２つ)

#define ACT_RES_PAL_NUM				(3)

//グラフィックデータがないので置き換え
enum{
	ARC_EVWIN_GRA	= ARC_RANKING_GRA,
	NARC_evwin_nclr = NARC_ranking_ranking_nclr,
	NARC_evwin_ncer = NARC_ranking_ranking_ncer,
	NARC_evwin_nanr = NARC_ranking_ranking_nanr,
	NARC_evwin_ncgr = NARC_ranking_ranking_ncgr,
};
#endif


//==============================================================================================
//
//	構造体
//
//==============================================================================================
struct _SEISEKI_WORK{
	FIELDSYS_WORK * fsys;						//FIELDSYS_WORKのポインタ
	TCB_PTR	tcb;

 	GF_BGL_BMPWIN bmpwin;						//BMPウィンドウデータ
 	GF_BGL_BMPWIN* talk_bmpwin;					//BMP会話ウィンドウデータ
	
	STRBUF* msg_buf[EV_WIN_MSG_MAX];			//メッセージデータのポインタ
	MSGDATA_MANAGER* msgman;					//メッセージマネージャー
	WORDSET* wordset;							//単語セット

	u8  wait;									//ウェイト
	u8  menu_id;								//BMPメニューID
	u8  cursor_pos;								//カーソル位置
	u8  cancel:1;								//キャンセル
	u8  msgman_del_flag:1;						//メッセージマネージャー削除フラグ
	u8  dmy:6;									//

	u8  x;										//ウィンドウ位置X
	u8  y;										//ウィンドウ位置Y
	u8  dmyy;									//メニューナンバー
	u8  list_no;								//メニュー項目の何番目か

	u16* pMsg;									//メッセージデータ
	u16* work;									//結果取得ワーク
	u16* lp_work;								//list_bak取得ワーク
	u16* cp_work;								//cursor_bak取得ワーク
 
	//BMPリスト
	BMPLIST_HEADER ListH;						//BMPリストヘッダー
	BMPLIST_WORK* lw;							//BMPリストデータ
	u16 list_bak;								//リスト位置バックアップ
	u16 cursor_bak;								//カーソル位置バックアップ
	BMPLIST_DATA list_Data[EV_WIN_LIST_MAX];	//リストデータ
	u16 talk_msg_id[EV_WIN_LIST_MAX];			//リストデータに対になる会話メッセージID

	u16 pos_bak;

#ifdef EV_WIN_SCROLL_CURSOR	//070228
	FIELD_CLACT		fcat;						//フィールド用セルアクター設定
	CATS_ACT_PTR	act[EVWIN_ACTMAX];			//CATSを使う時の専用アクター構造体
#endif
};


//==============================================================================================
//
//	関数
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	イベントウィンドウ　メッセージマネージャーセット
 *
 * @param	wk			WAZA_OSHIE_WORK型のポインタ
 * @param	msgman		MSGDATA_MANAGER型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void EvWin_MsgManSet( SEISEKI_WORK* wk, MSGDATA_MANAGER* msgman )
{
	wk->msgman = msgman;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	イベントウィンドウ　ワーク初期化
 *
 * @param	wk			SEISEKI_WORK型のポインタ
 * @param	x			ウィンドウ表示X座標
 * @param	y			ウィンドウ表示X座標
 * @param	cursor		カーソル位置
 * @param	cancel		Bキャンセルフラグ(TRUE=有効、FALSE=無効)
 * @param	work		結果を代入するワークのポインタ
 * @param	wordset		WORDSET型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void EvWin_Init( FIELDSYS_WORK* fsys, SEISEKI_WORK* wk, u8 x, u8 y, u8 cursor, u8 cancel, u16* work, WORDSET* wordset, GF_BGL_BMPWIN* talk_bmpwin, MSGDATA_MANAGER* msgman, u16* lp_work, u16* cp_work )
{
	int i;

	wk->msgman		= msgman;		//引継ぎ
	wk->msgman_del_flag = 0;
	wk->wordset		= wordset;		//スクリプトを引き継ぐ

	wk->fsys		= fsys;
	wk->work		= work;
#if 1
	*wk->work		= 0;			//初期化
#endif
	wk->lp_work		= lp_work;
	wk->cp_work		= cp_work;
	wk->cancel		= cancel;
	wk->cursor_pos	= cursor;
	wk->x			= x;
	wk->y			= y;
	wk->list_no		= 0;
	wk->talk_bmpwin	= talk_bmpwin;
	wk->wait		= EV_WIN_DEFAULT_WAIT;
	wk->pos_bak		= cursor;

	for( i=0; i < EV_WIN_LIST_MAX ;i++ ){
		wk->list_Data[i].str	= NULL;
		wk->list_Data[i].param	= 0;
		wk->talk_msg_id[i] = EV_WIN_TALK_MSG_NONE;
	}

	//MSGMAN_Createの後に処理
	for( i=0; i < EV_WIN_MSG_MAX ;i++ ){
		wk->msg_buf[i] = STRBUF_Create( EVWIN_MSG_BUF_SIZE, HEAPID_FIELD );
	}

	//選択した値を取得するワークを初期化
	*wk->work = EV_WIN_NOTHING;

	return;
}


//==============================================================================================
//
//	BMPリスト関連
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	BMPリスト　初期化
 *
 * @param	x			ウィンドウ表示X座標
 * @param	y			ウィンドウ表示X座標
 * @param	cursor		カーソル位置
 * @param	cancel		Bキャンセルフラグ(TRUE=有効、FALSE=無効)
 * @param	work		結果を代入するワークのポインタ
 * @param	wordset		WORDSET型のポインタ
 *
 * @retval	"SEISEKI_WORK型のアドレス、NULLは失敗"
 */
//--------------------------------------------------------------
SEISEKI_WORK * CmdSeisekiBmpList_Init( FIELDSYS_WORK* fsys, u8 x, u8 y, u8 cursor, u8 cancel, u16* work, WORDSET* wordset, GF_BGL_BMPWIN* talk_bmpwin, MSGDATA_MANAGER* msgman, u16* lp_work, u16* cp_work )
{
	SEISEKI_WORK* wk;
	int i;

	wk = sys_AllocMemory( HEAPID_FIELD, sizeof(SEISEKI_WORK) );
	if( wk == NULL ){
		OS_Printf( "ev_win.c Alloc ERROR!" );
		return NULL;
	}
	memset( wk, 0, sizeof(SEISEKI_WORK) );

	//ワーク初期化	
	EvWin_Init( fsys, wk, x, y, cursor, cancel, work, wordset, talk_bmpwin, msgman, lp_work, cp_work );

	return wk;
}

//--------------------------------------------------------------
/**
 * @brief	BMPリスト　リスト作成
 *
 * @param	wk			SEISEKI_WORK型のポインタ
 * @param	msg_id		メッセージID
 * @param	talk_msg_id	会話メッセージID
 * @param	param		BMPLISTパラメータ
 *
 * @retval	none
 */
//--------------------------------------------------------------
void CmdSeisekiBmpList_MakeList( SEISEKI_WORK* wk, u32 msg_id, u32 talk_msg_id, u32 param  )
{
	BmpList_list_make( wk, msg_id, talk_msg_id, param  );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	BMPリスト　開始
 *
 * @param	wk			SEISEKI_WORK型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void CmdSeisekiBmpList_Start( SEISEKI_WORK* wk )
{
	//表示最大項目数チェック
	if( wk->list_no > EV_LIST_LINE ){
		GF_BGL_BmpWinAdd( wk->fsys->bgl, &wk->bmpwin, FLD_MBGFRM_FONT, wk->x, wk->y, 
									11, EV_LIST_LINE*2, FLD_SYSFONT_PAL, EVWIN_FREE_CGX );
	}else{
		GF_BGL_BmpWinAdd( wk->fsys->bgl, &wk->bmpwin, FLD_MBGFRM_FONT, wk->x, wk->y, 
									11, wk->list_no*2, FLD_SYSFONT_PAL, EVWIN_FREE_CGX );
	}

	//メニューウィンドウのグラフィックをセット
	MenuWinGraphicSet(
		wk->fsys->bgl, FLD_MBGFRM_FONT, MENU_WIN_CGX_NUM, MENU_WIN_PAL, 0, HEAPID_FIELD );

	//メニューウィンドウを描画
	BmpMenuWinWrite(&wk->bmpwin, WINDOW_TRANS_OFF, MENU_WIN_CGX_NUM, MENU_WIN_PAL);

#ifdef EV_WIN_SCROLL_CURSOR	//070228
	EvWin_ActorInit( wk );
#endif

	//リストヘッダー初期設定
	BmpList_h_default_set(wk);

	//OS_Printf( "*wk->lp_work = %d\n", *wk->lp_work );
	//OS_Printf( "*wk->cp_work = %d\n", *wk->cp_work );
	wk->lw = BmpListSet(	(const BMPLIST_HEADER*)&wk->ListH, *wk->lp_work, 
							*wk->cp_work, HEAPID_FIELD );

	//TCB追加
	wk->tcb	= TCB_Add( EvBmpList_MainTCB, wk, 0 );

	return;
}

//--------------------------------------------------------------
/**
 * @brief	BMPリスト	リスト作成
 *
 * @param	wk			SEISEKI_WORK型のポインタ
 * @param	msg_id		メッセージID
 * @param	talk_msg_id	会話メッセージID
 * @param	param		BMPLISTパラメータ
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void BmpList_list_make( SEISEKI_WORK* wk, u32 msg_id, u32 talk_msg_id, u32 param  )
{
	int i;
	void* msg;

	SDK_ASSERTMSG( wk->list_no < EV_WIN_LIST_MAX, "リスト項目数オーバー！" );

	{
		//展開込み
		
		STRBUF* tmp_buf2= STRBUF_Create( EVWIN_MSG_BUF_SIZE, HEAPID_FIELD );	//コピー用バッファ

		MSGMAN_GetString( wk->msgman, msg_id, tmp_buf2 );
		WORDSET_ExpandStr( wk->wordset, wk->msg_buf[wk->list_no], tmp_buf2 );	//展開
		wk->list_Data[ wk->list_no ].str = (const void *)wk->msg_buf[wk->list_no];

		STRBUF_Delete( tmp_buf2 );
	}

	if( param == EV_WIN_LIST_LABEL ){
		wk->list_Data[ wk->list_no ].param = BMPLIST_RABEL;
		//*pa = BMPLIST_RABEL;
	}else{
		wk->list_Data[ wk->list_no ].param = param;
		//*pa = param;
	}

	wk->talk_msg_id[ wk->list_no ] = talk_msg_id;
	wk->list_no++;

	return;
}

//--------------------------------------------------------------
/**
 * @brief	BMPリスト	項目の中から最大文字数を取得
 *
 * @param	wk			SEISEKI_WORK型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
static u32 BmpList_length_get( SEISEKI_WORK* wk )
{
	int i;
	u32 ret,tmp_ret;

	ret		= 0;
	tmp_ret = 0;

	for( i=0; i < wk->list_no ;i++ ){
		if( wk->list_Data[i].str == NULL ){
			break;
		}

		ret = FontProc_GetPrintStrWidth( EV_WIN_FONT, (STRBUF*)wk->list_Data[i].str, 0 );
		
		if( tmp_ret < ret ){
			tmp_ret = ret;
		}
	}

	return tmp_ret + EV_WIN_CURSOR_WIDTH;
}

//--------------------------------------------------------------
/**
 * @brief	BMPリスト	ヘッダー初期設定
 *
 * @param	wk			SEISEKI_WORK型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void BmpList_h_default_set(SEISEKI_WORK* wk)
{
	wk->ListH.list		= wk->list_Data;
	wk->ListH.call_back = BmpList_CursorMoveCallBack;
	wk->ListH.icon		= BmpList_CallBack;
	wk->ListH.win		= &wk->bmpwin;

	wk->ListH.count		= wk->list_no;
	wk->ListH.line		= EV_LIST_LINE;

	wk->ListH.rabel_x	= EV_LIST_RABEL_X;
	wk->ListH.data_x	= EV_LIST_DATA_X;
	wk->ListH.cursor_x	= EV_LIST_CURSOR_X;
	wk->ListH.line_y	= EV_LIST_LINE_Y;

	wk->ListH.f_col		= FBMP_COL_BLACK;
	wk->ListH.b_col		= FBMP_COL_WHITE;		
	wk->ListH.s_col		= FBMP_COL_BLK_SDW;

	wk->ListH.msg_spc	= 0;
	wk->ListH.line_spc	= 16;
	wk->ListH.page_skip	= BMPLIST_LRKEY_SKIP;
	wk->ListH.font		= EV_WIN_FONT;
	wk->ListH.c_disp_f	= 0;

	wk->ListH.work		= (void*)wk;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	リストシステムからのコールバック※一行表示ごと
 *
 * @param	lw			BMPLIST_WORK型のポインタ
 * @param	param		選択時リターンパラメータ
 * @param	y			
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void	BmpList_CallBack(BMPLIST_WORK* lw,u32 param,u8 y)
{
	if( param == BMPLIST_RABEL ){
		BmpListTmpColorChange( lw, FBMP_COL_RED, FBMP_COL_WHITE, FBMP_COL_RED_SDW );
	}else{
		BmpListTmpColorChange( lw, FBMP_COL_BLACK, FBMP_COL_WHITE, FBMP_COL_BLK_SDW );
	}
}

//--------------------------------------------------------------
/**
 * @brief	BMPリスト	カーソル移動ごとのコールバック
 *
 * @param	
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void BmpList_CursorMoveCallBack( BMPLIST_WORK* wk, u32 param, u8 mode )
{
	u32 count,line;
	u16 list_bak = 0;
	u16 cursor_bak = 0;
	SEISEKI_WORK* evwin_wk = (SEISEKI_WORK*)BmpListParamGet(wk,BMPLIST_ID_WORK);

	BmpListPosGet( wk, &list_bak, &cursor_bak );

	if( (evwin_wk->lp_work != NULL) && (evwin_wk->cp_work != NULL) ){
		*evwin_wk->lp_work = list_bak;
		*evwin_wk->cp_work = cursor_bak;
		OS_Printf( "*wk->lp_work = %d\n", *evwin_wk->lp_work );
		OS_Printf( "*wk->cp_work = %d\n", *evwin_wk->cp_work );
	}

#ifdef EV_WIN_SCROLL_CURSOR	//070228

	//初期化時
	if( mode == 1 ){
		//
	}

	count = BmpListParamGet( wk, BMPLIST_ID_COUNT );			//リスト項目数
	line  = BmpListParamGet( wk, BMPLIST_ID_LINE );				//表示最大項目数

	//全ての項目を表示していない時
	if( count > line ){

		if( list_bak == 0 ){
			//OS_Printf( "ウィンドウの上のスクロールカーソル非表示\n" );
			//OS_Printf( "ウィンドウの下のスクロールカーソル表示\n" );
			CATS_ObjectEnableCap( evwin_wk->act[0], FALSE );	//OBJの非表示設定
			CATS_ObjectEnableCap( evwin_wk->act[1], TRUE );		//OBJの表示設定
		}else if( list_bak == (count-line) ){
			//OS_Printf( "ウィンドウの上のスクロールカーソル表示\n" );
			//OS_Printf( "ウィンドウの下のスクロールカーソル非表示\n" );
			CATS_ObjectEnableCap( evwin_wk->act[0], TRUE );		//OBJの表示設定
			CATS_ObjectEnableCap( evwin_wk->act[1], FALSE );	//OBJの非表示設定
		}else{
			//OS_Printf( "ウィンドウの上のスクロールカーソル表示\n" );
			//OS_Printf( "ウィンドウの下のスクロールカーソル表示\n" );
			CATS_ObjectEnableCap( evwin_wk->act[0], TRUE );		//OBJの表示設定
			CATS_ObjectEnableCap( evwin_wk->act[1], TRUE );		//OBJの表示設定
		}
	}

#endif

	return;
}

//--------------------------------------------------------------
/**
 * @brief	BMPリスト　リストメイン
 *
 * @param	tcb			TCB_PTR
 * @param	wk			ワークのアドレス
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void EvBmpList_MainTCB( TCB_PTR tcb, void* wk )
{
	u16 tmp_pos_bak;
	u32	ret;
	SEISEKI_WORK* swk;
	swk = (SEISEKI_WORK*)wk;

	//ウェイト
	if( swk->wait != 0 ){
		swk->wait--;
		return;
	}

	//ワイプ中は処理しない
	if( WIPE_SYS_EndCheck() == FALSE ){
		return;
	}

	ret = BmpListMain( swk->lw );

	//BMPリストの全体位置を取得
	tmp_pos_bak = swk->pos_bak;		//退避
	BmpListDirectPosGet( swk->lw, &swk->pos_bak );

	//カーソルが動いたかチェック
	if( tmp_pos_bak != swk->pos_bak ){
		Snd_SePlay( SEQ_SE_DP_SELECT );
	}

#ifdef EV_WIN_SCROLL_CURSOR	//070228
	CLACT_Draw( swk->fcat.cas );
#endif

	switch( ret ){
	case BMPLIST_NULL:
		break;
	case BMPLIST_CANCEL:
		if( swk->cancel == TRUE ){			//TRUE = Bキャンセル有効
			Snd_SePlay( SEQ_SE_DP_SELECT );
			*swk->work = EV_WIN_B_CANCEL;	//選択した値をワークに代入
#ifdef EV_WIN_SCROLL_CURSOR	//070228
			EvWin_ActorRelease( wk );		//削除
#endif
			EvBmpList_Del(wk);
		}
		break;
	default:
		Snd_SePlay( SEQ_SE_DP_SELECT );
		*swk->work = ret;					//選択した値をワークに代入
#ifdef EV_WIN_SCROLL_CURSOR	//070228
		EvWin_ActorRelease( wk );			//削除
#endif
		EvBmpList_Del(wk);
		break;
	};

	return;
};

//--------------------------------------------------------------
/**
 * @brief	BMPリスト　リスト終了
 *
 * @param	wk			SEISEKI_WORK型のポインタ
 *
 * @retval	none
 *
 * SEQ_SE_DP_SELECTを鳴らしている！
 */
//--------------------------------------------------------------
static void EvBmpList_Del( SEISEKI_WORK* wk )
{
	int i;

	Snd_SePlay(SEQ_SE_DP_SELECT);	//注意！

	BmpListExit( wk->lw, NULL, NULL );
	BmpMenuWinClear( wk->ListH.win, WINDOW_TRANS_ON );
	GF_BGL_BmpWinDel( &wk->bmpwin );

	for( i=0; i < EV_WIN_MSG_MAX ;i++ ){
		STRBUF_Delete( wk->msg_buf[i] );
	}

	if( wk->msgman_del_flag == 1 ){
		//WORDSET_Delete( wk->wordset );
		MSGMAN_Delete( wk->msgman );
	}

	TCB_Delete( wk->tcb );
	sys_FreeMemoryEz( wk );
	return;
}


//==============================================================================================
//
//	スクロールカーソル
//
//==============================================================================================
#ifdef EV_WIN_SCROLL_CURSOR	//070228

//アクター初期化
static void EvWin_ActorInit( SEISEKI_WORK* wk )
{
	int i;
	s16 x = (wk->x * 8) + (BmpList_length_get(wk) / 2);

	//リソース登録最大数定義構造体
	TCATS_RESOURCE_NUM_LIST	crnl = { 1, 1, 1, 1 };

	//登録用構造体 単体登録用(座標は後で設定し直す)
	static const TCATS_OBJECT_ADD_PARAM_S ActAddParam_S[] =
	{
		//上向き
		{
			0, 0, 0,
			CURSOR_ANMNO, CURSOR_SPRI, CURSOR_PALT, NNS_G2D_VRAM_TYPE_2DMAIN,
			{
				EVWIN_CHR_H_ID_CURSOR, EVWIN_PAL_H_ID,
				EVWIN_CEL_H_ID_CURSOR, EVWIN_ANM_H_ID_CURSOR, 0, 0,
			},
			0, 0
		},

		//下向き
		{
			0, 0, 0,
			CURSOR_ANMNO, CURSOR_SPRI, CURSOR_PALT, NNS_G2D_VRAM_TYPE_2DMAIN,
			{
				EVWIN_CHR_H_ID_CURSOR, EVWIN_PAL_H_ID,
				EVWIN_CEL_H_ID_CURSOR, EVWIN_ANM_H_ID_CURSOR, 0, 0,
			},
			0, 0
		},
	};
	
	//フィールドセルアクター初期化
	FieldCellActSet_S( &wk->fcat, &crnl, EVWIN_ACTMAX, HEAPID_FIELD );
	
	{
		ARCHANDLE* hdl;
		
		hdl = ArchiveDataHandleOpen( ARC_EVWIN_GRA, HEAPID_FIELD );

		//リソース設定
		FldClact_LoadResPlttArcH(	&wk->fcat, hdl, NARC_evwin_nclr,
									0, ACT_RES_PAL_NUM, NNS_G2D_VRAM_TYPE_2DMAIN, 
									EVWIN_PAL_H_ID );

		FldClact_LoadResCellArcH(	&wk->fcat, hdl, NARC_evwin_ncer,
									0, EVWIN_CEL_H_ID_CURSOR );

		FldClact_LoadResCellAnmArcH(&wk->fcat, hdl, NARC_evwin_nanr,
									0, EVWIN_ANM_H_ID_CURSOR );

		FldClact_LoadResourceCharArcH(	&wk->fcat, hdl, NARC_evwin_ncgr,
										0, NNS_G2D_VRAM_TYPE_2DMAIN, EVWIN_CHR_H_ID_CURSOR );
		
		ArchiveDataHandleClose( hdl );
	}

	//セルアクター追加(単発用)
	for( i=0; i < EVWIN_ACTMAX; i++ ){
		wk->act[i] = FieldCellActAdd_S( &wk->fcat, &ActAddParam_S[i] );
		CLACT_SetAnmFlag( wk->act[i]->act, 1 );				//オートアニメ
		CATS_ObjectEnableCap( wk->act[i], FALSE );			//OBJの非表示設定
	}

	//座標の設定
	//OS_Printf( "x = %d\n", x );
	//OS_Printf( "wk->y = %d\n", (wk->y*8) );
	//OS_Printf( "y = %d\n", (wk->list_no*8) );
	CATS_ObjectPosSetCap( wk->act[0], x, (wk->y*8) );
	CATS_ObjectPosSetCap( wk->act[1], x, (wk->list_no*8) );

	//オートアニメで対応じゃなくて、座標を動かしてアニメでもいいのかも。。保留
	
	//選択カーソルの色変更
	//CATS_ObjectPaletteSetCap(wk->act[ACT_CURSOR],CURSOR_PALF);
	
	return;
}

//アクター削除
static void EvWin_ActorRelease( SEISEKI_WORK* wk )
{
	int i;

	//アクターポインタの削除
	for( i=0; i < EVWIN_ACTMAX; i++ ){
		if( wk->act[i] != NULL ){
			CATS_ActorPointerDelete_S( wk->act[i] );
		}
	}

	//セルアクター削除（単発用）
	FieldCellActDelete_S( &wk->fcat );
	return;
}
#endif


//============================================================================================
//
//	トップブリーダー
//
//============================================================================================
BOOL EvCmdTopBreederPowRndGet( VM_MACHINE * core );

//--------------------------------------------------------------
/**
 * @brief	ポケモンのパワー乱数値チェック
 *
 * @param	none
 *
 * @retval	0
 */
//--------------------------------------------------------------
//status.gmmでは「HP」ではなく「たいりょく」となっているのでダメ(08.05.15)
#if 0

#include "msgdata/msg_status.h"
static const u16 msg_id_tbl[] = {
	STATUS_000,	//たいりょく
	STATUS_001,	//こうげき
	STATUS_002, //ぼうぎょ
	STATUS_003,	//すばやさ
	STATUS_004,	//とくこう
	STATUS_005,	//とくぼう
};

#else

#include "msgdata/msg_d31r0201.h"
static const u16 msg_id_tbl[] = {
	msg_tower_judge_08,		//HP
	msg_tower_judge_08_01,	//こうげき
	msg_tower_judge_08_02,	//ぼうぎょ
	msg_tower_judge_08_05,	//すばやさ
	msg_tower_judge_08_03,	//とくこう
	msg_tower_judge_08_04,	//とくぼう
};

#endif

BOOL EvCmdTopBreederPowRndGet( VM_MACHINE * core )
{
	u8 i;
	u32 temp[6];
	POKEMON_PARAM* poke;
	FIELDSYS_WORK* fsys	= core->fsys;
	u16 pos				= VMGetWorkValue( core );	//手持ちの何番目かを代入しておく
	u16* ret_wk			= VMGetWork( core );		//パワー乱数の合計
	u16* ret_wk2		= VMGetWork( core );		//パワー乱数が最も高いパラメータ
	u16* ret_wk3		= VMGetWork( core );		//パワー乱数が最も高いパラメータの値

	//ポケモンへのポインタ取得
	poke = PokeParty_GetMemberPointer( SaveData_GetTemotiPokemon(fsys->savedata), pos );

	//パワー乱数取得
	temp[0] = PokeParaGet( poke, ID_PARA_hp_rnd, NULL );
	temp[1] = PokeParaGet( poke, ID_PARA_pow_rnd, NULL );
	temp[2] = PokeParaGet( poke, ID_PARA_def_rnd, NULL );
	temp[3] = PokeParaGet( poke, ID_PARA_agi_rnd, NULL );
	temp[4] = PokeParaGet( poke, ID_PARA_spepow_rnd, NULL );
	temp[5] = PokeParaGet( poke, ID_PARA_spedef_rnd, NULL );

	//パワー乱数の合計
	*ret_wk = 0;
	for( i=0; i < 6 ;i++ ){
		*ret_wk += temp[i];
	}

	//パワー乱数が最も高いパラメータ
	*ret_wk2 = 0;
	*ret_wk3 = temp[0];

	for( i=1; i < 6 ;i++ ){

		if( temp[*ret_wk2] < temp[i] ){

			*ret_wk2 = i;
			*ret_wk3 = temp[i];

		}else if( temp[*ret_wk2] == temp[i] ){

			//同じ値が存在したらランダムにする
			if( (gf_rand() % 2) == 0 ){
				*ret_wk2 = i;
				*ret_wk3 = temp[i];
			}
		}
	}

	//メッセージのIDを返すようにする
	*ret_wk2 = msg_id_tbl[*ret_wk2];
	return 0;
}


//==============================================================================
//
//	ユニオンルームに入る前にフォルムを戻す
//
//==============================================================================
BOOL EvCmdUnionPokeFormReturn( VM_MACHINE * core )
{
	POKEMON_PARAM* poke;
	POKEPARTY *party;
	int party_max, pos, ret;
	int monsno, form_no;
	u32 set_item;
	int item_array[TEMOTI_POKEMAX];
	int hakkin_num		= 0;
	FIELDSYS_WORK* fsys	= core->fsys;
	u16* ret_wk			= VMGetWork( core );
	
	party		= SaveData_GetTemotiPokemon(fsys->savedata);
	party_max	= PokeParty_GetPokeCount(party);
	*ret_wk		= 0;
	
	//手持ちのハッキンダマの数をカウント
	for(pos = 0; pos < party_max; pos++){
		poke = PokeParty_GetMemberPointer(party, pos);
		item_array[pos] = PokeParaGet(poke, ID_PARA_item, NULL);
		if(item_array[pos] == ITEM_HAKKINDAMA){
			hakkin_num++;
		}
	}

	//ハッキンダマをバッグへ戻す
	if(hakkin_num > 0){

		ret = MyItem_AddItem(	SaveData_GetMyItem(fsys->savedata), 
								ITEM_HAKKINDAMA, hakkin_num, HEAPID_FIELD);

		if(ret == FALSE){
			*ret_wk = 0xff;	//ハッキンダマを手持ちに戻せなかった
			return 0;
		}

		//バッグに戻したのでハッキンダマを所持しているポケモンのアイテムを無しにする
		set_item = 0;
		for(pos = 0; pos < party_max; pos++){
			if(item_array[pos] == ITEM_HAKKINDAMA){
				poke = PokeParty_GetMemberPointer(party, pos);
				PokeParaPut(poke, ID_PARA_item, &set_item);
			}
		}
	}
	
	//ギラティナ、シェイミ、ロトムのフォルムを戻す
	for(pos = 0; pos < party_max; pos++){

		poke	= PokeParty_GetMemberPointer(party, pos);
		form_no = PokeParaGet(poke, ID_PARA_form_no, NULL);

		if(form_no > 0){

			monsno = PokeParaGet(poke, ID_PARA_monsno, NULL);

			switch(monsno){
			case MONSNO_KIMAIRAN:	//ギラティナ
				//上でハッキンダマは取り上げているので再計算だけする
				PokeParaGirathinaFormChange(poke);
				break;
			case MONSNO_PURAZUMA:	//ロトム
				PokeParaRotomuFormChange(poke, FORMNO_ROTOMU_NORMAL, 0);
				break;
			case MONSNO_EURISU:		//シェイミ
				PokeParaSheimiFormChange(poke, FORMNO_SHEIMI_NORMAL);
				break;
			}
		}
	}
	
	return 0;
}

//==============================================================================
//
//	育て屋に預ける前にフォルムを戻す(EvCmdUnionPokeFormReturnの単品バージョン)
//
//==============================================================================
BOOL EvCmdSodateyaPokeFormReturn( VM_MACHINE * core )
{
	POKEMON_PARAM* poke;
	POKEPARTY *party;
	int monsno, form_no;
	u32 set_item;
	int item;
	int ret;
	FIELDSYS_WORK* fsys	= core->fsys;
	u16 poke_pos		= VMGetWorkValue( core );				//手持ち位置
	u16* ret_wk			= VMGetWork( core );
	
	party	= SaveData_GetTemotiPokemon(fsys->savedata);
	poke	= PokeParty_GetMemberPointer(party, poke_pos);
	*ret_wk = 0;
	
	//リストをキャンセルした値が入っている時は何もしない
	if( poke_pos == 0xff ){
		return 0;
	}

	//ハッキンダマ確認
	item = PokeParaGet(poke, ID_PARA_item, NULL);

	if(item == ITEM_HAKKINDAMA){

		ret = MyItem_AddItem(	SaveData_GetMyItem(fsys->savedata), 
								ITEM_HAKKINDAMA, 1, HEAPID_FIELD);

		if(ret == FALSE){
			*ret_wk = 0xff;	//ハッキンダマを手持ちに戻せなかった
			return 0;
		}

		//バッグに戻したのでハッキンダマを所持しているポケモンのアイテムを無しにする
		set_item = 0;
		PokeParaPut(poke, ID_PARA_item, &set_item);
	}
	
	//ギラティナ、シェイミ、ロトムのフォルムを戻す
	form_no = PokeParaGet(poke, ID_PARA_form_no, NULL);

	if(form_no > 0){

		monsno = PokeParaGet(poke, ID_PARA_monsno, NULL);

		switch(monsno){
		case MONSNO_KIMAIRAN:	//ギラティナ
			//上でハッキンダマは取り上げているので再計算だけする
			PokeParaGirathinaFormChange(poke);
			break;
		case MONSNO_PURAZUMA:	//ロトム
			PokeParaRotomuFormChange(poke, FORMNO_ROTOMU_NORMAL, 0);
			break;
		case MONSNO_EURISU:		//シェイミ
			PokeParaSheimiFormChange(poke, FORMNO_SHEIMI_NORMAL);
			break;
		}
	}

	return 0;
}

//============================================================================================
//
//	フォルムチェンジ
//
//============================================================================================
BOOL EvCmdPokeFormChange( VM_MACHINE * core );

//フォルムが増えたら対応しないとダメ！
#include "battle/battle_server.h"
#include "battle/wazano_def.h"
#include "ev_pokemon.h"
static const u8 pura_form_tbl[FORMNO_ROTOMU_MAX] = {
	//FORMNO_ROTOMU_NORMAL,				//ノーマルフォルム
	FORMNO_ROTOMU_HOT,					//電子レンジフォルム
	FORMNO_ROTOMU_WASH,					//洗濯機フォルム
	FORMNO_ROTOMU_COLD,					//冷蔵庫フォルム
	FORMNO_ROTOMU_FAN,					//扇風機フォルム
	FORMNO_ROTOMU_CUT,					//草刈り機フォルム
};

static const u16 pura_waza_tbl[FORMNO_ROTOMU_MAX] = {
	//WAZANO_DENKISYOKKU,
	WAZANO_OOBAAHIITO,
	WAZANO_HAIDOROPONPU,
	WAZANO_HUBUKI,
	WAZANO_EASURASSYU,
	WAZANO_RIIHUSUTOOMU,
};

//--------------------------------------------------------------
/**
 * @brief	ポケモンのフォルムチェンジ
 *
 * @param	none
 *
 * @retval	0
 */
//--------------------------------------------------------------
BOOL EvCmdPokeFormChange( VM_MACHINE * core )
{
	POKEMON_PARAM* poke;
	FIELDSYS_WORK* fsys	= core->fsys;
	u16 pos				= VMGetWorkValue( core );	//手持ちの何番目かを代入しておく
	u16 no				= VMGetWorkValue( core );	//formどれか

	//ポケモンへのポインタ取得
	poke = PokeParty_GetMemberPointer( SaveData_GetTemotiPokemon(fsys->savedata), pos );
	PokeParaPut( poke, ID_PARA_form_no, &no );
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	ロトムのフォルムチェンジが手持ちに何匹いるか取得、最初に見つけた手持ち位置も取得
 *
 * @param	none
 *
 * @retval	0
 */
//--------------------------------------------------------------
BOOL EvCmdTemotiRotomuFormChangeGet( VM_MACHINE * core )
{
	u32 monsno,form_no,tamago_flag;
	int temoti_max,i,count;
	POKEMON_PARAM* pp;
	POKEPARTY* party;
	FIELDSYS_WORK* fsys	= core->fsys;
	u16* ret_wk			= VMGetWork( core );
	u16* ret_wk2		= VMGetWork( core );

	count = 0;
	*ret_wk2 = 0xff;

	party = SaveData_GetTemotiPokemon( fsys->savedata );
	temoti_max = PokeParty_GetPokeCount( party );

	//タマゴでない、ノーマルフォルムでない、ロトムの数を取得
	for( i=0; i < temoti_max; i++ ){

		pp = PokeParty_GetMemberPointer( party, i );

		monsno = PokeParaGet( pp, ID_PARA_monsno, NULL );
		form_no= PokeParaGet( pp, ID_PARA_form_no, NULL );
		tamago_flag = PokeParaGet( pp, ID_PARA_tamago_flag, NULL );
		OS_Printf( "monsno = %d\n", monsno );
		OS_Printf( "form_no = %d\n", form_no );
		OS_Printf( "tamago_flag = %d\n", tamago_flag );

		if( (monsno == MONSNO_PURAZUMA) && (form_no != FORMNO_ROTOMU_NORMAL) && (tamago_flag == 0)){

			//最初に見つけた手持ち位置を保存
			if( *ret_wk2 == 0xff ){
				*ret_wk2 = i;
			}

			count++;
		}
	}

	*ret_wk = count;
	OS_Printf( "数 *ret_wk = %d\n", *ret_wk );
	OS_Printf( "位置 *ret_wk2 = %d\n", *ret_wk2 );
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	ロトムのフォルムを変えた時の技書き換え
 *
 * @param	none
 *
 * @retval	0
 */
//--------------------------------------------------------------
BOOL EvCmdRotomuFormWazaChange( VM_MACHINE * core )
{
	u32 now_form_no,i;
	u16 del_waza_no,waza_count;
	POKEMON_PARAM* poke;
	POKEPARTY* party;
	FIELDSYS_WORK* fsys	= core->fsys;
	u16 poke_pos	= VMGetWorkValue( core );				//手持ち位置
	u16 waza_pos	= VMGetWorkValue( core );				//技の位置
	u16 waza		= VMGetWorkValue( core );				//覚える技
	u16 next_form	= VMGetWorkValue( core );				//次のフォルム

	party = SaveData_GetTemotiPokemon( fsys->savedata );
	poke = PokeParty_GetMemberPointer( party, poke_pos );

#if 1	//2008.03.27(木)　matsuda
	PokeParaRotomuFormChange(poke, next_form, waza_pos);
	ZukanWork_SetPokeGet(SaveData_GetZukanWork(fsys->savedata), poke);//図鑑登録
#else

	now_form_no = PokeParaGet( poke, ID_PARA_form_no, NULL );

	///////////////////////////////////////
	//ノーマルから違うフォルムに変更する時
	///////////////////////////////////////
	if( now_form_no == FORMNO_ROTOMU_NORMAL ){

			EvPoke_ChangeWaza(	SaveData_GetTemotiPokemon(core->fsys->savedata), poke_pos, 
								waza_pos, waza );

	}else{
		for( i=0; i < FORMNO_ROTOMU_MAX ;i++ ){
			if( now_form_no == pura_form_tbl[i] ){
				del_waza_no = pura_waza_tbl[i];				//今のフォルムの専用技を取得
				break;
			}
		}

		/////////////////////////////////
		//ノーマルに戻す時
		/////////////////////////////////
		if( next_form == FORMNO_ROTOMU_NORMAL ){

			//指定された技(専用技)をもっていたら忘れさせる
			for( i=0; i < 4 ;i++ ){
				if( PokeParaGet(poke,ID_PARA_waza1+i,NULL) == del_waza_no ){
					PokeParaWazaDelPos( poke, i );
					break;
				}
			}

			//技の数が0になっていたら「でんきショック」を覚えさせておく
			waza_count = 0;
			for( i=0; i < 4 ;i++ ){
				if( PokeParaGet(poke,ID_PARA_waza1+i,NULL) != 0 ){
					waza_count++;
				}
			}
	
			if( waza_count == 0 ){
				EvPoke_ChangeWaza(	SaveData_GetTemotiPokemon(core->fsys->savedata), poke_pos, 
									0, WAZANO_DENKISYOKKU );
			}

		/////////////////////////////////
		//さらに違うフォルムに変更する時
		/////////////////////////////////
		}else{

			//指定された技(専用技)をもっていたら、次のフォルムの技に置き換える
			for( i=0; i < 4 ;i++ ){
				if( PokeParaGet(poke,ID_PARA_waza1+i,NULL) == del_waza_no ){
					EvPoke_ChangeWaza(	SaveData_GetTemotiPokemon(core->fsys->savedata), poke_pos, 
										i, waza );
					break;
				}
			}

			//指定された技をもっていなかったら、プレイヤーが選択した技位置を書き換える
			if( i == 4 ){
				EvPoke_ChangeWaza(	SaveData_GetTemotiPokemon(core->fsys->savedata), poke_pos, 
									waza_pos, waza );
			}
		}
	}
#endif

	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	めざめるパワーのタイプを取得
 *
 * @param	none
 *
 * @retval	0
 *
 * ポケモンナンバーを見て、覚えられないポケモン判別をしているので注意！
 */
//--------------------------------------------------------------
BOOL EvCmdMezameruPawaaTypeGet( VM_MACHINE * core )
{
	u16 monsno;
	int power,type;
	POKEMON_PARAM* poke;
	FIELDSYS_WORK* fsys	= core->fsys;
	u16 pos				= VMGetWorkValue( core );	//手持ちの何番目かを代入しておく
	u16*ret_wk			= VMGetWork( core );	//

	//ポケモンへのポインタ取得
	poke = PokeParty_GetMemberPointer( SaveData_GetTemotiPokemon(fsys->savedata), pos );

	monsno = PokeParaGet( poke, ID_PARA_monsno, NULL );

	if( PokeParaGet(poke,ID_PARA_tamago_flag,NULL) == 0 ){
		switch( monsno ){
		case MONSNO_KYATAPII:
		case MONSNO_TORANSERU:
		case MONSNO_BIIDORU:
		case MONSNO_KOKUUN:
		case MONSNO_KOIKINGU:
		case MONSNO_METAMON:
		case MONSNO_SOONANSU:
		case MONSNO_DOOBURU:
		case MONSNO_KEMUSSO:
		case MONSNO_KARASARISU:
		case MONSNO_MAYURUDO:
		case MONSNO_SOONANO:
		case MONSNO_DANBARU:
		case MONSNO_MINOMUTTI:
		case MONSNO_HEKISAGON:		//ミツハニー
		case MONSNO_KOBOSI:			//コロボーシ
			*ret_wk = 0xffff;		//なしコードをいれておく
			return 0;
		};
	}

	//めざめるパワーのタイプを取得
	GetMezameruPowerParam( poke, &power, &type );
	*ret_wk = type;
	//*ret_wk2 = power;
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	めざめるパワーの威力とタイプを取得
 *
 * @param[in]	pp		取得するPokemonParam構造体へのポインタ
 * @param[out]	power	威力を格納するワークへのポインタ（NULL指定で取得なし）
 * @param[out]	type	威力を格納するワークへのポインタ（NULL指定で取得なし）
 */
//--------------------------------------------------------------
static void	GetMezameruPowerParam(POKEMON_PARAM *pp,int *power,int *type)
{
	int	hp_rnd;
	int	pow_rnd;
	int	def_rnd;
	int	agi_rnd;
	int	spepow_rnd;
	int	spedef_rnd;

	hp_rnd		=PokeParaGet(pp,ID_PARA_hp_rnd,		NULL);
	pow_rnd		=PokeParaGet(pp,ID_PARA_pow_rnd,	NULL);
	def_rnd		=PokeParaGet(pp,ID_PARA_def_rnd,	NULL);
	agi_rnd		=PokeParaGet(pp,ID_PARA_agi_rnd,	NULL);
	spepow_rnd	=PokeParaGet(pp,ID_PARA_spepow_rnd,	NULL);
	spedef_rnd	=PokeParaGet(pp,ID_PARA_spedef_rnd,	NULL);

	if(power!=NULL){
		*power=	((hp_rnd&2)		>>1)|
				((pow_rnd&2)	>>0)|
				((def_rnd&2)	<<1)|
				((agi_rnd&2)	<<2)|
				((spepow_rnd&2)	<<3)|
				((spedef_rnd&2)	<<4);
		*power=((*power)*40/63)+30;
	}
	if(type!=NULL){
		*type=	((hp_rnd&1)		>>0)|
				((pow_rnd&1)	<<1)|
				((def_rnd&1)	<<2)|
				((agi_rnd&1)	<<3)|
				((spepow_rnd&1)	<<4)|
				((spedef_rnd&1)	<<5);
		*type=((*type)*15/63)+1;

		if(*type>=HATE_TYPE){
			//*type++;
			*type=(*type)+1;
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief	フォルムチェンジしたロトムがいるかチェック(全てチェック)
 *
 * @param	none
 *
 * @retval	0
 */
//--------------------------------------------------------------
#if 0
static u32 MyRotomuFormCheck(SAVEDATA *sv);

#include "battle/battle_server.h"
#if 0
// ロトムのフォルム定義(BIT指定)
#define FORMNO_ROTOMU_BIT_NORMAL		(1 << FORMNO_ROTOMU_NORMAL)
#define FORMNO_ROTOMU_BIT_HOT			(1 << FORMNO_ROTOMU_HOT)
#define FORMNO_ROTOMU_BIT_WASH			(1 << FORMNO_ROTOMU_WASH)
#define FORMNO_ROTOMU_BIT_COLD			(1 << FORMNO_ROTOMU_COLD)
#define FORMNO_ROTOMU_BIT_FAN			(1 << FORMNO_ROTOMU_FAN)
#define FORMNO_ROTOMU_BIT_CUT			(1 << FORMNO_ROTOMU_CUT)
#endif
BOOL EvCmdRotomuFormCheck( VM_MACHINE * core )
{
	u32 rotom_bit;
	FIELDSYS_WORK* fsys	= core->fsys;
	u16*hot_wk			= VMGetWork( core );	//
	u16*wash_wk			= VMGetWork( core );	//
	u16*cold_wk			= VMGetWork( core );	//
	u16*fan_wk			= VMGetWork( core );	//
	u16*cut_wk			= VMGetWork( core );	//

	*hot_wk	= 0;		//
	*wash_wk= 0;		//
	*cold_wk= 0;		//
	*fan_wk	= 0;		//
	*cut_wk	= 0;		//

	rotom_bit = MyRotomuFormCheck( fsys->savedata );

	if( ((rotom_bit >> FORMNO_ROTOMU_BIT_HOT) & 0x01) == 1 ){
		*hot_wk = 1;
	}

	if( ((rotom_bit >> FORMNO_ROTOMU_BIT_WASH) & 0x01) == 1 ){
		*wash_wk = 1;
	}

	if( ((rotom_bit >> FORMNO_ROTOMU_BIT_COLD) & 0x01) == 1 ){
		*cold_wk = 1;
	}

	if( ((rotom_bit >> FORMNO_ROTOMU_BIT_FAN) & 0x01) == 1 ){
		*fan_wk = 1;
	}

	if( ((rotom_bit >> FORMNO_ROTOMU_BIT_CUT) & 0x01) == 1 ){
		*cut_wk = 1;
	}

	OS_Printf( "*hot_wk = %d\n", *hot_wk );
	OS_Printf( "*wash_wk = %d\n", *wash_wk );
	OS_Printf( "*cold_wk = %d\n", *cold_wk );
	OS_Printf( "*fan_wk = %d\n", *fan_wk );
	OS_Printf( "*cut_wk = %d\n", *cut_wk );
	return 1;
}

//--------------------------------------------------------------
/**
 * @brief   自分の手持ち、ボックス、育て屋、からフォルムチェンジしたロトムがいるか取得
 *
 * @param   sv		セーブデータへのポインタ
 *
 * @retval  FORMNO_ROTOMU_BIT_??? が ORの総和で返ってきます
 * 			※ノーマルフォルムでもビットが立つことに注意
 */
//--------------------------------------------------------------
static u32 MyRotomuFormCheck(SAVEDATA *sv)
{
	int i;
	POKEMON_PARAM *pp;
	POKEMON_PASO_PARAM *ppp;
	u32 form_bit;
	
	form_bit = 0;
	
	//手持ちチェック
	{
		POKEPARTY *party;
		int temoti_max;
		
		party = SaveData_GetTemotiPokemon(sv);
		temoti_max = PokeParty_GetPokeCount(party);
		for(i = 0; i < temoti_max; i++){
			pp = PokeParty_GetMemberPointer(party, i);
			if(PokeParaGet(pp, ID_PARA_monsno, NULL) == MONSNO_PURAZUMA
					&& PokeParaGet(pp, ID_PARA_tamago_flag, NULL) == 0){
				form_bit |= 1 << PokeParaGet(pp, ID_PARA_form_no, NULL);
			}
		}
	}
	
	//育て屋チェック
	{
		SODATEYA_WORK *sodateya;
		SODATEYA_POKE *s_poke;
		
		sodateya = SaveData_GetSodateyaWork(sv);
		for(i = 0; i < SODATEYA_POKE_MAX; i++){
			s_poke = SodateyaWork_GetSodateyaPokePointer(sodateya, i);
			ppp = SodateyaPoke_GetPokePasoPointer(s_poke);
			if(PokePasoParaGet(ppp,ID_PARA_monsno,NULL) == MONSNO_PURAZUMA
					&& PokePasoParaGet(ppp, ID_PARA_tamago_flag, NULL) == 0){
				form_bit |= 1 << PokePasoParaGet(ppp, ID_PARA_form_no, NULL);
			}
		}
	}
	
	//ボックスチェック
	{
		BOX_DATA *boxdata;
		u32 tray;
		
		boxdata = SaveData_GetBoxData(sv);
		for(tray = 0; tray < BOX_MAX_TRAY; tray++){
			for(i = 0; i < BOX_MAX_POS; i++){
				ppp = BOXDAT_GetPokeDataAddress(boxdata, tray, i);
				if(PokePasoParaGet(ppp, ID_PARA_monsno, NULL) == MONSNO_PURAZUMA
						&& PokePasoParaGet(ppp, ID_PARA_tamago_flag, NULL) == 0){
					form_bit |= 1 << PokePasoParaGet(ppp, ID_PARA_form_no, NULL);
				}
			}
		}
	}
	
	return form_bit;
}
#endif

//============================================================================================
//
//	お気に入りポケモン
//
//============================================================================================

//--------------------------------------------------------------
/**
 * @brief	お気に入りポケモンのセット
 *
 * @param	none
 *
 * @retval	0
 */
//--------------------------------------------------------------
BOOL EvCmdSetFavoritePoke( VM_MACHINE * core )
{
	MISC* misc;
	POKEMON_PARAM* poke;
	FIELDSYS_WORK* fsys	= core->fsys;

	//先頭ポケモンへのポインタ取得
	poke = PokeParty_GetMemberPointer( SaveData_GetTemotiPokemon(fsys->savedata), 0 );

	misc = SaveData_GetMisc( fsys->savedata );
	MISC_SetFavoriteMonsno( misc, 
							PokeParaGet(poke,ID_PARA_monsno,NULL),
							PokeParaGet(poke,ID_PARA_form_no,NULL),
							PokeParaGet(poke,ID_PARA_tamago_flag,NULL) );
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	お気に入りポケモンの取得
 *
 * @param	none
 *
 * @retval	0
 */
//--------------------------------------------------------------
BOOL EvCmdGetFavoritePoke( VM_MACHINE * core )
{
	int mons,form,egg;
	MISC* misc;
	POKEMON_PARAM* poke;
	FIELDSYS_WORK* fsys	= core->fsys;
	u16* mons_wk		= VMGetWork( core );		//
	u16* form_wk		= VMGetWork( core );		//
	u16* egg_wk			= VMGetWork( core );		//

	misc = SaveData_GetMisc( fsys->savedata );
	MISC_GetFavoriteMonsno( misc, &mons, &form, &egg );

	*mons_wk = mons;
	*form_wk = form;
	*egg_wk	 = egg;
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	指定した位置のフォルムナンバーをかえす(ロトム用)
 *
 * @param	none
 *
 * @retval	0
 */
//--------------------------------------------------------------
BOOL EvCmdTemotiRotomuFormNoGet( VM_MACHINE * core )
{
	POKEMON_PARAM* pp;
	POKEPARTY* party;
	FIELDSYS_WORK* fsys	= core->fsys;
	u16 pos				= VMGetWorkValue( core );
	u16* ret_wk			= VMGetWork( core );

	party = SaveData_GetTemotiPokemon( fsys->savedata );
	pp = PokeParty_GetMemberPointer( party, pos );

	*ret_wk = PokeParaGet( pp, ID_PARA_form_no, NULL );
	OS_Printf( "pos = %d\n", pos );
	OS_Printf( "*ret_wk = %d\n", *ret_wk );
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief	別荘のリスト表示可能かチェック
 *
 * @param	none
 *
 * @retval	0
 */
//--------------------------------------------------------------
BOOL EvCmdVillaListCheck( VM_MACHINE * core )
{
	EVENTWORK* ev;
	RECORD* rec;
	FIELDSYS_WORK* fsys	= core->fsys;
	u16 no				= VMGetWorkValue( core );
	u16* ret_wk			= VMGetWork( core );

	ev		= SaveData_GetEventWork( fsys->savedata );
	rec		= SaveData_GetRecord( fsys->savedata );
	*ret_wk = 1;		//OK

	switch( no ){

	//ポケモンぞう1
	case 13:
		//フロンティア全施設1勝以上している？
		if( RECORD_Get(rec,RECID_BTOWER_WIN) < 1 ){
			*ret_wk = 0;
		}

		if( RECORD_Get(rec,RECID_FACTORY_WIN) < 1 ){
			*ret_wk = 0;
		}

		if( RECORD_Get(rec,RECID_CASTLE_WIN) < 1 ){
			*ret_wk = 0;
		}

		if( RECORD_Get(rec,RECID_STAGE_WIN) < 1 ){
			*ret_wk = 0;
		}

		if( RECORD_Get(rec,RECID_ROULETTE) < 1 ){
			*ret_wk = 0;
		}
		break;

	//ポケモンぞう2
	case 14:
		*ret_wk = 0;		//NG

		//銀シンボルをどれか1コ取得している？
		if( SysWork_MemoryPrintFactory(ev) >= MEMORY_PRINT_PUT_OK_1ST ){
			*ret_wk = 1;
		}

		if( SysWork_MemoryPrintStage(ev) >= MEMORY_PRINT_PUT_OK_1ST ){
			*ret_wk = 1;
		}

		if( SysWork_MemoryPrintCastle(ev) >= MEMORY_PRINT_PUT_OK_1ST ){
			*ret_wk = 1;
		}

		if( SysWork_MemoryPrintRoulette(ev) >= MEMORY_PRINT_PUT_OK_1ST ){
			*ret_wk = 1;
		}

		if( SysWork_MemoryPrintTower(ev) >= MEMORY_PRINT_PUT_OK_1ST ){
			*ret_wk = 1;
		}
		break;

	//ピアノ
	case 15:
		//殿堂入りカウント10以上？
		if( RECORD_Get(rec,RECID_DENDOU_CNT) < 10 ){
			*ret_wk = 0;
		}
		break;

	//おうせつセット
	case 16:
		//再戦施設で戦闘した回数50以上？
		if( RECORD_Get(rec,RECID_LEADERHOUSE_BATTLE) < 50 ){
			*ret_wk = 0;
		}
		break;

	//かべどけい
	case 17:
		//木の実を植えた回数50以上？
		if( RECORD_Get(rec,RECID_NUTS_PLANT) < 50 ){
			*ret_wk = 0;
		}
		break;

	//すばらしいめいが
	case 18:
		//タマゴ孵化した回数30以上？
		if( RECORD_Get(rec,RECID_TAMAGO_HATCHING) < 30 ){
			*ret_wk = 0;
		}
		break;

	//シャンデリア
	case 20:
		//歩数カウンター300000以上？
		if( RECORD_Get(rec,RECID_WALK_COUNT) < 300000 ){
			*ret_wk = 0;
		}
		break;

	};

	return 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ドレスアップ送信済みフラグをリセットする
 *
 *	@param	core		仮想マシン制御構造体へのポインタ
 *
 *	@return "1"
 */
//-----------------------------------------------------------------------------
BOOL EvCmdImageClipTvSendFlagReset( VM_MACHINE * core )
{
#if 0	//フラグ自体無くなった2008.06.21(土) matsuda
	FIELDSYS_WORK* fsys	= core->fsys;
	MISC * misc;
	
	misc = SaveData_GetMisc(fsys->savedata);
	MISC_SetGdsSend_Dress(misc, 0);
#endif
	return 1;
}

typedef struct{
	s16 x;
	s16 z;
}POS;

//踏む位置
static const POS rezi_d35r0102_pos[] = {
	{ 4,	7 },	//
	{ 5,	5 },	//
	{ 5,	9 },	//
	{ 7,	7 },	//
	{ 9,	5 },	//
	{ 9,	9 },	//
	{ 10,	7 },	//
};
#define REZI_D35R0102_POS_MAX	( NELEMS(rezi_d35r0102_pos) )

static const POS rezi_d35r0104_pos[] = {
	{ 3,	7 },
	{ 5,	7 },
	{ 7,	5 },
	{ 7,	7 },
	{ 7,	9 },
	{ 9,	7 },
	{ 11,	7 },
};
#define REZI_D35R0104_POS_MAX	( NELEMS(rezi_d35r0104_pos) )

static const POS rezi_d35r0106_pos[] = {
	{ 5,	5 },
	{ 5,	7 },
	{ 5,	9 },
	{ 7,	7 },
	{ 9,	5 },
	{ 9,	7 },
	{ 9,	9 },
};
#define REZI_D35R0106_POS_MAX	( NELEMS(rezi_d35r0106_pos) )

//----------------------------------------------------------------------------
/**
 *	@brief	レジマップの床チェック
 *
 *	@param	core		仮想マシン制御構造体へのポインタ
 *
 *	@return "0"
 */
//-----------------------------------------------------------------------------
BOOL EvCmdD35FloorSet( VM_MACHINE * core )
{
	int i,all_num;
	u16* wk		= VMGetWork( core );
	u16 zone_id = VMGetWorkValue( core );
	u16 x		= VMGetWorkValue( core );
	u16 z		= VMGetWorkValue( core );
	
	switch( zone_id ){

	case ZONE_ID_D35R0102:
		all_num = REZI_WORK_2_ALL_NUM;
		for( i=0; i < REZI_D35R0102_POS_MAX ;i++ ){
			if( (x == rezi_d35r0102_pos[i].x) && (z == rezi_d35r0102_pos[i].z) ){
				Snd_SePlay( SEQ_SE_PL_JUMP2 );
				*wk |= (1 << i);
				break;
			}
		}
		break;

	case ZONE_ID_D35R0104:
		all_num = REZI_WORK_4_ALL_NUM;
		for( i=0; i < REZI_D35R0104_POS_MAX ;i++ ){
			if( (x == rezi_d35r0104_pos[i].x) && (z == rezi_d35r0104_pos[i].z) ){
				Snd_SePlay( SEQ_SE_PL_JUMP2 );
				*wk |= (1 << i);
				break;
			}
		}
		break;

	case ZONE_ID_D35R0106:
		all_num = REZI_WORK_6_ALL_NUM;
		for( i=0; i < REZI_D35R0106_POS_MAX ;i++ ){
			if( (x == rezi_d35r0106_pos[i].x) && (z == rezi_d35r0106_pos[i].z) ){
				Snd_SePlay( SEQ_SE_PL_JUMP2 );
				*wk |= (1 << i);
				break;
			}
		}
		break;
	};

	//全ての床を踏んだ
	if( *wk == all_num ){
		*wk = REZI_WORK_POS_OK;
	}

	OS_Printf( "i = %d\n", i );
	OS_Printf( "*wk = %d\n", *wk );
	return 0;
}


//============================================================================================
//	フィールドOBJ
//============================================================================================
//--------------------------------------------------------------
/**
 *  @brief フィールドOBJ　OBJに高さ取得失敗でも表示する様にする。
 *  ついでに高さを自機に合わせる。自機だけはそのまま。
 *	@param	core		仮想マシン制御構造体へのポインタ
 *	@return "0"
 */
//--------------------------------------------------------------
BOOL EvCmdFldOBJAllHeightVanishOFF( VM_MACHINE * core )
{
	fx32 y;
	VecFx32 pos;
	BLACT_WORK_PTR blact;
	
	int objno = 0;
	FIELDSYS_WORK *fsys = core->fsys;
	FIELD_OBJ_SYS_PTR fos = fsys->fldobjsys;
	FIELD_OBJ_PTR jiki = Player_FieldOBJGet( fsys->player );
	FIELD_OBJ_PTR fldobj;
	
	FieldOBJ_VecPosGet( jiki, &pos );
	y = pos.y;
	
	while( FieldOBJSys_FieldOBJSearch(
		fos,&fldobj,&objno,FLDOBJ_STA_BIT_USE) == TRUE ){
		if( fldobj != jiki ){
			FieldOBJ_StatusBit_ON(
				fldobj, FLDOBJ_STA_BIT_HEIGHT_VANISH_OFF );
			
			if( FieldOBJ_StatusBit_CheckEasy(
				fldobj,FLDOBJ_STA_BIT_HEIGHT_GET_ERROR) == TRUE ){
				FieldOBJ_VecPosGet( fldobj, &pos );
				pos.y = y;
				FieldOBJ_VecPosSet( fldobj, &pos );
				FieldOBJ_NowPosGY_Set( fldobj, SIZE_H_GRID_FX32(y) );
			}
			
			blact = FieldOBJ_DrawBlAct00_BlActPtrGet( fldobj );
			
			if( blact == NULL &&
				FieldOBJ_OBJCodeSeedCheck(FieldOBJ_OBJCodeGet(fldobj)) ){
				if( FieldOBJ_StatusBitCheck_DrawProcInitComp(fldobj) ){
					FieldOBJ_DrawProcCall( fldobj );
					blact = FieldOBJ_SeedBlActPtrGet( fldobj );
				}
			}
			
			if( blact != NULL ){
				FieldOBJ_BlActPosSet( fldobj, blact );
				BLACT_ObjDrawFlagSet( blact, TRUE );
			}
		}
	}
	
	return 0;
}

//--------------------------------------------------------------
/**
 *  @brief フィールドOBJ
 *  EvCmdFldOBJAllHeightVanishOFF()でセットしたフラグを戻す。
 *	@param	core		仮想マシン制御構造体へのポインタ
 *	@return "0"
 */
//--------------------------------------------------------------
BOOL EvCmdFldOBJAllHeightVanishON( VM_MACHINE * core )
{
	int objno = 0;
	FIELDSYS_WORK *fsys = core->fsys;
	FIELD_OBJ_SYS_PTR fos = fsys->fldobjsys;
	FIELD_OBJ_PTR jiki = Player_FieldOBJGet( fsys->player );
	FIELD_OBJ_PTR fldobj;
	
	while( FieldOBJSys_FieldOBJSearch(
		fos,&fldobj,&objno,FLDOBJ_STA_BIT_USE) == TRUE ){
		if( fldobj != jiki ){
			FieldOBJ_StatusBit_OFF(
				fldobj, FLDOBJ_STA_BIT_HEIGHT_VANISH_OFF );
		}
	}
	
	return 0;
}

//--------------------------------------------------------------
/**
 *  @brief EvCmdFldOBJForceDraw_C04EventOnly()コア
 */
//--------------------------------------------------------------
static void evcmd_FldOBJForceDrawC04OnlyCore( FIELD_OBJ_PTR fldobj, int grid )
{
	VecFx32 pos;
	BLACT_WORK_PTR blact;
	
	FieldOBJ_StatusBit_ON( fldobj, FLDOBJ_STA_BIT_HEIGHT_VANISH_OFF );
	
	FieldOBJ_VecPosGet( fldobj, &pos );
	pos.y = GRID_SIZE_FX32( grid );
	FieldOBJ_VecPosSet( fldobj, &pos );
	FieldOBJ_NowPosGY_Set( fldobj, G_GRID_H_GRID(grid) );
	
	blact = FieldOBJ_DrawBlAct00_BlActPtrGet( fldobj );
	
	if( blact != NULL ){
		FieldOBJ_BlActPosSet( fldobj, blact );
		BLACT_ObjDrawFlagSet( blact, TRUE );
	}
}

//--------------------------------------------------------------
/**
 *  @brief C04限定 フィールドOBJ、GINGABOSS,BABYBOY1を強制表示
 *	@param	core		仮想マシン制御構造体へのポインタ
 *	@return "0"
 */
//--------------------------------------------------------------
BOOL EvCmdFldOBJForceDraw_C04EventOnly( VM_MACHINE * core )
{
	FIELD_OBJ_PTR fldobj;
	FIELDSYS_WORK *fsys = core->fsys;
	FIELD_OBJ_SYS_PTR fos = fsys->fldobjsys;
	
	//アカギ
	fldobj = FieldOBJSys_OBJIDSearch( fos, C04_AKAGI );
	
	if( fldobj != NULL ){
		evcmd_FldOBJForceDrawC04OnlyCore( fldobj, 9 );
	}
	
	//子供
	fldobj = FieldOBJSys_OBJIDSearch( fos, C04_BABYBOY1 );
	
	if( fldobj != NULL ){
		evcmd_FldOBJForceDrawC04OnlyCore( fldobj, 9 );
	}
	
	return 0;
}

//--------------------------------------------------------------
/**
 *  @brief C04限定 EvCmdFldOBJForceDraw_C04EventOnly()の後始末
 *	@param	core		仮想マシン制御構造体へのポインタ
 *	@return "0"
 */
//--------------------------------------------------------------
BOOL EvCmdFldOBJForceDraw_C04EventAfterOnly( VM_MACHINE * core )
{
	FIELD_OBJ_PTR fldobj;
	FIELD_OBJ_SYS_PTR fos = core->fsys->fldobjsys;
	
	//アカギ
	fldobj = FieldOBJSys_OBJIDSearch( fos, C04_AKAGI );
	
	if( fldobj != NULL ){
		FieldOBJ_StatusBit_OFF( fldobj, FLDOBJ_STA_BIT_HEIGHT_VANISH_OFF );
	}
	
	//子供
	fldobj = FieldOBJSys_OBJIDSearch( fos, C04_BABYBOY1 );
	
	if( fldobj != NULL ){
		FieldOBJ_StatusBit_OFF( fldobj, FLDOBJ_STA_BIT_HEIGHT_VANISH_OFF );
	}
	
	return 0;
}

void ov05_21F7704(void);
void ov05_21F7754(void);

asm void ov05_21F7704(void)
{
	push {r4, lr}
	add r0, #0x80
	ldr r0, [r0, #0]
	mov r1, #0xf
	ldr r0, [r0, #0x38]
	bl FieldOBJSys_OBJIDSearch
	add r4, r0, #0
	beq _021F774E
	bl FieldOBJ_NowPosGX_Get
	cmp r0, #0x28
	bgt _021F7730
	cmp r0, #0x26
	blt _021F772A
	beq _021F773A
	cmp r0, #0x28
	beq _021F773E
	b _021F7746
_021F772A:
	cmp r0, #0x1c
	beq _021F7736
	b _021F7746
_021F7730:
	cmp r0, #0x30
	beq _021F7742
	b _021F7746
_021F7736:
	mov r1, #6
	b _021F7748
_021F773A:
	mov r1, #5
	b _021F7748
_021F773E:
	mov r1, #3
	b _021F7748
_021F7742:
	mov r1, #2
	b _021F7748
_021F7746:
	mov r1, #4
_021F7748:
	add r0, r4, #0
	bl evcmd_FldOBJForceDrawC04OnlyCore
_021F774E:
	mov r0, #0
	pop {r4, pc}
}

asm void ov05_21F7754(void)
{
    push {r3, lr}
	add r0, #0x80
	ldr r0, [r0]
	mov r1, #0xf
	ldr r0, [r0, #0x38]
	bl FieldOBJSys_OBJIDSearch
	cmp r0, #0
	beq _021F776E
	mov r1, #2
	lsl r1, r1, #0xc
	bl FieldOBJ_StatusBit_OFF
_021F776E:
	mov r0, #0
	pop {r3, pc}
}

//--------------------------------------------------------------
/**
 * @brief   地球儀登録
 *
 * @param   SAVEDATA	savedata
 *
 * @return "1"
 */
//--------------------------------------------------------------
BOOL EvCmdWifiHistory( VM_MACHINE * core )
{
	WIFI_HISTORY* pHistory = SaveData_GetWifiHistory( core->fsys->savedata );
	Comm_WifiHistoryCheck( pHistory );
	return 1;
}

//--------------------------------------------------------------
/**
 * @brief   プレイヤーの音量を操作
 *
 * @param   SAVEDATA	savedata
 *
 * @return "0"
 */
//--------------------------------------------------------------
BOOL EvCmdSndPlayerVolume( VM_MACHINE * core )
{
	u16 volume = VMGetWorkValue( core );

	Snd_PlayerSetPlayerVolume( PLAYER_FIELD, volume );
	return 0;
}

#if PL_S0802_080716_FIX
//--------------------------------------------------------------------------------------------
/**
 * @brief		スクリプトコマンド：図鑑見たことあるか取得
 * @param		core
 * @return		"0"
 */
//--------------------------------------------------------------------------------------------
BOOL EvCmdGetPokeSeeFlag( VM_MACHINE * core )
{
	u16 monsno	= VMGetWorkValue(core);
	u16* ret_wk	= VMGetWork( core );
	const ZUKAN_WORK* zw = SaveData_GetZukanWork( core->fsys->savedata );
	
	*ret_wk = ZukanWork_GetPokeSeeFlag( zw, monsno );
	return 0;
}
#endif

