//==============================================================================
/**
 * @file	ファイル名
 * @brief	簡単な説明を書く
 * @author	matsuda
 * @date	2007.04.06(金)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
#include "common.h"

#include "system/arc_tool.h"
#include "system/bmp_menu.h"
#include "system/brightness.h"
#include "system/fontproc.h"
#include "system/fontoam.h"
#include "system/font_arc.h"
#include "system/snd_def.h"
#include "system/snd_perap.h"
#include "system/snd_tool.h"
#include "system/window.h"
#include "system/msg_ds_icon.h"
#include "system/wipe.h"
#include "system/pmfprint.h"

#include "gflib/msg_print.h"						//STRCODE
#include "system\msgdata.h"							//MSGMAN_TYPE_DIRECT
#include "system/bmp_list.h"
#include "system/pm_str.h"
#include "system/msgdata_util.h"
#include "system/wordset.h"							//WORDSET_Create
#include "system/gra_tool.h"

#include "frontier_types.h"
#include "frontier_main.h"
#include "frontier_scr.h"
#include "fss_task.h"
#include "fs_usescript.h"
#include "frontier_map.h"
#include "frontier_scrcmd.h"
#include "frontier_scrcmd_sub.h"
#include "frontier_id.h"
#include "frontier_act_pri.h"

#include "msgdata/msg.naix"
#include "poketool/pokeicon.h"
#include "application/pokelist.h"


//==============================================================================
//	定数定義
//==============================================================================
//カーソル幅
#define FSEV_WIN_CURSOR_WIDTH		(12)			//カーソル幅

#define FSEV_WIN_DEFAULT_WAIT		(3)				//キー操作がすぐに入らないように基本ウェイト

//BMPメニュー
#define FSEV_WIN_MENU_MAX			(28)			//メニュー項目の最大数

//BMPリスト
#define FSEV_WIN_LIST_MAX			(28)			//リスト項目の最大数

#define	FSEV_WIN_MSG_MAX			(28)			//MSGバッファの最大数

//BMPリストヘッダー定義
#define EV_LIST_LINE				(8)				//表示最大項目数
#define EV_LIST_RABEL_X				(1)				//ラベル表示Ｘ座標
#define EV_LIST_DATA_X				(12)			//項目表示Ｘ座標
#define EV_LIST_CURSOR_X			(2)				//カーソル表示Ｘ座標
#define EV_LIST_LINE_Y				(1)				//表示Ｙ座標

#define FSEV_FONT					(FONT_SYSTEM)

#define FSEVWIN_MSG_BUF_SIZE		(40*2)			//メッセージバッファサイズ


//==============================================================================
//	構造体定義
//==============================================================================
typedef struct _FSEV_WIN{
	FSS_PTR fss;
	TCB_PTR	tcb;

 	GF_BGL_BMPWIN bmpwin;						//BMPウィンドウデータ
 	GF_BGL_BMPWIN* talk_bmpwin;					//BMP会話ウィンドウデータ
	
	STRBUF* msg_buf[FSEV_WIN_MSG_MAX];			//メッセージデータのポインタ
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
 
	//BMPメニュー(bmp_menu.h)
	BMPMENU_HEADER MenuH;						//BMPメニューヘッダー
	BMPMENU_WORK * mw;							//BMPメニューワーク
	BMPMENU_DATA Data[FSEV_WIN_MENU_MAX];			//BMPメニューデータ

	//BMPリスト
	BMPLIST_HEADER ListH;						//BMPリストヘッダー
	BMPLIST_WORK* lw;							//BMPリストデータ
	u16 list_bak;								//リスト位置バックアップ
	u16 cursor_bak;								//カーソル位置バックアップ
	BMPLIST_DATA list_Data[FSEV_WIN_LIST_MAX];	//リストデータ
	u16 talk_msg_id[FSEV_WIN_LIST_MAX];			//リストデータに対になる会話メッセージID

	u16 pos_bak;

}FSEV_WIN_WORK;


//--------------------------------------------------------------
//	
//--------------------------------------------------------------
///アニメコマンドデータテーブル構造体
typedef struct{
	u16 cmd;		///<アニメーションコマンド
	u16 way;		///<方向
}ANM_CMD_TBL_DATA;


//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static void AddBitmapWin(FSS_PTR fss);
static void LoadPrintString(FSS_PTR fss, const MSGDATA_MANAGER* msgman, u32 msg_id );
static void ScrTalkMsgCore(FSS_PTR fss, int font, int speed, int skip_flag, int auto_flag );

static void EvWin_Init(FSS_PTR fss, FSEVWIN_PTR wk, u8 x, u8 y, u8 cursor, u8 cancel, 
	u16* work, WORDSET* wordset, MSGDATA_MANAGER* msgman );
FSEVWIN_PTR FSSC_Sub_BmpMenu_Init( FSS_PTR fss, u8 x, u8 y, u8 cursor, u8 cancel, 
	u16* work, WORDSET* wordset, MSGDATA_MANAGER* msgman );
void FSSC_Sub_BmpMenu_MakeList( FSEVWIN_PTR wk, u32 msg_id, u32 talk_msg_id, u32 param  );
void FSSC_Sub_BmpMenu_Start( FSEVWIN_PTR wk );
static void BmpMenu_list_make( FSEVWIN_PTR wk, u32 msg_id, u32 talk_msg_id, u32 param  );
static u32 BmpMenu_length_get( FSEVWIN_PTR wk );
static void BmpMenu_h_default_set(FSEVWIN_PTR wk);
static void EvBmpMenu_MainTCB( TCB_PTR tcb, void* wk );
static void EvBmpMenu_Del( FSEVWIN_PTR wk );
static void menu_talk_msg_update( FSEVWIN_PTR wk );
void FSSC_Sub_BmpMenu_Del( FSEVWIN_PTR wk );
static void talk_msg_print( FSEVWIN_PTR wk, u16 talk_msg_id, u32 wait );

//簡易会話
void FSTalkMsgPMSParam( FS_SYSTEM* fss, u8 spd, u16 stype, u16 sid, u16 word0, s16 word1, u8 skip );
static void CreatePrintStringByPmsParam( STRBUF* buf, u16 stype, u16 sid, PMS_WORD word0, PMS_WORD word1 );

//==============================================================================
//
//	会話メッセージ
//
//==============================================================================
//--------------------------------------------------------------
/**
 *	会話メッセージウィンドウ出力
 *
 * @param	core		FSS_TASK型のポインタ
 * @param	msg_id		メッセージID
 * @param	skip		メッセージスキップフラグ
 * @param   ex_param	拡張パラメータ(不要な場合はNULL)
 *
 * @retval	none
 */
//--------------------------------------------------------------
void FSSC_Sub_ScrTalkMsg(FSS_PTR fss, const MSGDATA_MANAGER* msgman, u16 msg_id, u8 skip,  FSCR_TALK_EX_PARAM *ex_param )
{
	u8 msg_speed;
	u8 auto_flag;
	u8 font;

	AddBitmapWin(fss);
	LoadPrintString(fss, msgman, msg_id );

	if(ex_param == NULL){
		FMAP_PTR fmap = FSS_GetFMapAdrs(fss);
		FRONTIER_EX_PARAM *ex_param;
		
		ex_param = Frontier_ExParamGet(fmap->fmain);

		msg_speed = CONFIG_GetMsgPrintSpeed(ex_param->config);
		auto_flag = MSG_AUTO_OFF;
		font = FONT_TALK;
	}
	else{
		msg_speed = ex_param->msg_speed;
		auto_flag = ex_param->auto_flag;
		font = ex_param->font;
	}

	ScrTalkMsgCore( fss, font, msg_speed, skip, auto_flag );
}

//------------------------------------------------------------------
/**
 * 会話ウィンドウ登録・表示
 *
 * @param   fsys		
 * @param   param		
 *
 */
//------------------------------------------------------------------
static void AddBitmapWin(FSS_PTR fss)
{
	FMAP_PTR fmap = FSS_GetFMapAdrs(fss);
	
	if(fss->win_open_flag == 0){
		GF_BGL_BmpWinAdd(
			fmap->bgl, &fss->bmpwin_talk, FRMAP_FRAME_WIN, FFD_MSG_WIN_PX, FFD_MSG_WIN_PY,
			FFD_MSG_WIN_SX, FFD_MSG_WIN_SY, FFD_MSG_WIN_PAL, FFD_MSG_WIN_CGX );
		GF_BGL_BmpWinDataFill( &fss->bmpwin_talk, FBMP_COL_WHITE );
		BmpTalkWinWrite(&fss->bmpwin_talk, WINDOW_TRANS_ON, FR_TALK_WIN_CGX_NUM, FR_TALK_WIN_PAL );

		fss->win_open_flag = 1;
	}
	else{
		GF_BGL_BmpWinDataFill( &fss->bmpwin_talk, FBMP_COL_WHITE );
	}
}

//------------------------------------------------------------------
/**
 * 出力文字列をパラメータ内部バッファに読み込み
 *
 * @param   param		
 * @param   msgman		
 * @param   msg_id		
 *
 */
//------------------------------------------------------------------
static void LoadPrintString(FSS_PTR fss, const MSGDATA_MANAGER* msgman, u32 msg_id )
{
	MSGMAN_GetString( msgman, msg_id, fss->tmp_buf );
	WORDSET_ExpandStr( fss->wordset, fss->msg_buf, fss->tmp_buf );
}

//------------------------------------------------------------------
/**
 * 会話ウィンドウにメッセージを流す処理を開始
 *
 * @param   msg_param		
 * @param   font		
 * @param   speed		
 * @param   skip_flag		
 * @param   auto_flag		
 *
 */
//------------------------------------------------------------------
static void ScrTalkMsgCore(FSS_PTR fss, int font, int speed, int skip_flag, int auto_flag )
{
	MsgPrintSkipFlagSet( skip_flag );
	MsgPrintAutoFlagSet( auto_flag );
	MsgPrintTouchPanelFlagSet( MSG_TP_OFF );
	fss->msg_talk_index = GF_STR_PrintSimple( &fss->bmpwin_talk, font, 
		fss->msg_buf, 0, 0, speed, NULL );
}

//--------------------------------------------------------------
/**
 * @brief   会話ウィンドウを閉じる
 *
 * @param   fss		
 */
//--------------------------------------------------------------
void FSSC_Sub_ScrTalkClose(FSS_PTR fss)
{
	GF_ASSERT(fss->win_open_flag == 1);
	
	BmpTalkWinClear( &fss->bmpwin_talk, WINDOW_TRANS_ON );
	GF_BGL_BmpWinDel( &fss->bmpwin_talk );
	
	fss->win_open_flag = 0;
}

//==============================================================================
//	簡易会話
//==============================================================================

//--------------------------------------------------------------
/**
 * パラメータから簡易会話データを生成、メッセージ出力する
 *
 * @param	core		VM_MACHINE型のポインタ
 * @param	stype		
 * @param	sid
 * @param	word0
 * @param	word1
 * @param	skip
 *
 * @retval	none
 */
//--------------------------------------------------------------
void FSTalkMsgPMSParam( FS_SYSTEM* fss, u8 spd, u16 stype, u16 sid, u16 word0, s16 word1, u8 skip )
{
	PMS_DATA pms;

	AddBitmapWin( fss );

	//簡易会話データから文字列を生成し、パラメータ内部バッファに読み込み
	CreatePrintStringByPmsParam( fss->msg_buf, stype, sid, word0, word1 );

	if(skip != 0xFF){
		ScrTalkMsgCore( fss, FONT_TALK, spd, skip, MSG_AUTO_OFF );
	}else{
		// skip == 0xff だったら 一括表示する
		ScrTalkMsgCore( fss, FONT_TALK, MSG_ALLPUT, skip, MSG_AUTO_OFF );
	}
}

//------------------------------------------------------------------
/**
 * 簡易会話データから文字列を生成し、パラメータ内部バッファに読み込み
 *
 * @param   param		
 * @param   stype		
 * @param   sid		
 * @param   word0		
 * @param   word1		
 *
 */
//------------------------------------------------------------------
static void CreatePrintStringByPmsParam( STRBUF* buf, u16 stype, u16 sid, PMS_WORD word0, PMS_WORD word1 )
{
	PMS_DATA     pms;
	STRBUF* tmpBuf;

	// 簡易会話データ生成
	PMSDAT_Clear(&pms);
	PMSDAT_SetSentence( &pms, stype, sid );
	PMSDAT_SetWord( &pms, 0, word0 );
	PMSDAT_SetWord( &pms, 1, word1 );

	tmpBuf = PMSDAT_ToString( &pms, HEAPID_EVENT );
	STRBUF_Copy( buf, tmpBuf );
	STRBUF_Delete( tmpBuf );
}

//==============================================================================
//	BMPメニュー
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief	イベントウィンドウ　ワーク初期化
 *
 * @param	wk			FSEV_WIN_WORK型のポインタ
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
static void EvWin_Init(FSS_PTR fss, FSEVWIN_PTR wk, u8 x, u8 y, u8 cursor, u8 cancel, u16* work, WORDSET* wordset, MSGDATA_MANAGER* msgman )
{
	int i;

	if( msgman == NULL ){
		//メッセージデータマネージャー作成
		wk->msgman = MSGMAN_Create(MSGMAN_TYPE_DIRECT, ARC_MSG, NARC_msg_ev_win_dat, fss->heap_id);
		wk->msgman_del_flag = 1;
	}else{
		wk->msgman = msgman;	//引継ぎ
		wk->msgman_del_flag = 0;
	}

	//wk->wordset = WORDSET_Create( fss->heap_id );
	wk->wordset		= wordset;		//スクリプトを引き継ぐ

	wk->fss			= fss;
	wk->work		= work;
#if 1
	*wk->work		= 0;			//強制終了でワークの値を見るようになったので初期化を入れておく
#endif
	wk->cancel		= cancel;
	wk->cursor_pos	= cursor;
	wk->x			= x;
	wk->y			= y;
	wk->list_no		= 0;
	wk->talk_bmpwin	= &fss->bmpwin_talk;
	wk->wait		= FSEV_WIN_DEFAULT_WAIT;
	wk->pos_bak		= cursor;

	for( i=0; i < FSEV_WIN_MENU_MAX ;i++ ){
		wk->Data[i].str			= NULL;
		wk->Data[i].param		= 0;
	}

	for( i=0; i < FSEV_WIN_LIST_MAX ;i++ ){
		wk->list_Data[i].str	= NULL;
		wk->list_Data[i].param	= 0;
		wk->talk_msg_id[i] = FSEV_WIN_TALK_MSG_NONE;
	}

	//MSGMAN_Createの後に処理
	for( i=0; i < FSEV_WIN_MSG_MAX ;i++ ){
		wk->msg_buf[i] = STRBUF_Create( FSEVWIN_MSG_BUF_SIZE, fss->heap_id );
	}

	//選択した値を取得するワークを初期化
	*wk->work = FSEV_WIN_NOTHING;

	return;
}

//--------------------------------------------------------------
/**
 * @brief	BMPメニュー　初期化
 *
 * @param	x			ウィンドウ表示X座標
 * @param	y			ウィンドウ表示X座標
 * @param	cursor		カーソル位置
 * @param	cancel		Bキャンセルフラグ(TRUE=有効、FALSE=無効)
 * @param	work		結果を代入するワークのポインタ
 * @param	wordset		WORDSET型のポインタ
 *
 * @retval	"FSEV_WIN_WORK型のアドレス、NULLは失敗"
 */
//--------------------------------------------------------------
FSEVWIN_PTR FSSC_Sub_BmpMenu_Init( FSS_PTR fss, u8 x, u8 y, u8 cursor, u8 cancel, u16* work, WORDSET* wordset, MSGDATA_MANAGER* msgman )
{
	FSEVWIN_PTR wk;
	int i;

	wk = sys_AllocMemory( fss->heap_id, sizeof(FSEV_WIN_WORK) );
	if( wk == NULL ){
		OS_Printf( "ev_win.c Alloc ERROR!" );
		return NULL;
	}
	memset( wk, 0, sizeof(FSEV_WIN_WORK) );

	//ワーク初期化	
	EvWin_Init( fss, wk, x, y, cursor, cancel, work, wordset, msgman );

	return wk;
}

//--------------------------------------------------------------
/**
 * @brief	BMPメニュー　リスト作成
 *
 * @param	wk			FSEV_WIN_WORK型のポインタ
 * @param	msg_id		メッセージID
 * @param	param		BMPMENUパラメータ
 *
 * @retval	none
 */
//--------------------------------------------------------------
void FSSC_Sub_BmpMenu_MakeList( FSEVWIN_PTR wk, u32 msg_id, u32 talk_msg_id, u32 param  )
{
	BmpMenu_list_make( wk, msg_id, talk_msg_id, param  );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	BMPメニュー　開始
 *
 * @param	wk			FSEV_WIN_WORK型のポインタ
 * @param	msg_id		メッセージID
 * @param	param		BMPMENUパラメータ
 *
 * @retval	none
 */
//--------------------------------------------------------------
void FSSC_Sub_BmpMenu_Start( FSEVWIN_PTR wk )
{
	u32 len;
	FMAP_PTR fmap = FSS_GetFMapAdrs(wk->fss);

	//(最大文字数＋カーソル) * フォントサイズ
	//len = (BmpMenu_length_get(wk)+1) * FontHeaderGet(FSEV_FONT, FONT_HEADER_SIZE_X);
	len = BmpMenu_length_get(wk);

	if( (len % 8) == 0 ){
		len = (len / 8);
	}else{
		len = (len / 8)+1;
	}

	GF_BGL_BmpWinAdd( fmap->bgl, &wk->bmpwin, FRMAP_FRAME_WIN, wk->x, wk->y, 
									len, wk->list_no*2, FFD_SYSFONT_PAL, FFD_FREE_CGX );

	//メニューウィンドウのグラフィックをセット
//	MenuWinGraphicSet(
//		wk->fsys->bgl, FFD_MBGFRM_FONT, FR_MENU_WIN_CGX_NUM, FR_MENU_WIN_PAL, 0, fss->heap_id );

	//メニューウィンドウを描画
	BmpMenuWinWrite(&wk->bmpwin, WINDOW_TRANS_OFF, FR_MENU_WIN_CGX_NUM, FR_MENU_WIN_PAL);

	//メニューヘッダー初期設定
	BmpMenu_h_default_set(wk);
	wk->mw = BmpMenuAdd( &wk->MenuH, wk->cursor_pos, wk->fss->heap_id );

	//会話ウィンドウ更新
	menu_talk_msg_update( wk );

	//TCB追加
	wk->tcb	= TCB_Add( EvBmpMenu_MainTCB, wk, 0 );

	return;
}

//--------------------------------------------------------------
/**
 * @brief	BMPメニュー	リスト作成
 *
 * @param	wk			FSEV_WIN_WORK型のポインタ
 * @param	msg_id		メッセージID
 * @param	talk_msg_id	会話メッセージID
 * @param	param		BMPMENUパラメータ
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void BmpMenu_list_make( FSEVWIN_PTR wk, u32 msg_id, u32 talk_msg_id, u32 param  )
{
	int i;
	void* msg;

	SDK_ASSERTMSG( wk->list_no < FSEV_WIN_MENU_MAX, "メニュー項目数オーバー！" );

	{
		//展開込み
		
		STRBUF* tmp_buf2= STRBUF_Create( FSEVWIN_MSG_BUF_SIZE, wk->fss->heap_id );//コピー用バッファ

		MSGMAN_GetString( wk->msgman, msg_id, tmp_buf2 );
		WORDSET_ExpandStr( wk->wordset, wk->msg_buf[wk->list_no], tmp_buf2 );	//展開
		wk->Data[ wk->list_no ].str = (const void *)wk->msg_buf[wk->list_no];

		STRBUF_Delete( tmp_buf2 );
	}

	wk->talk_msg_id[ wk->list_no ] = talk_msg_id;
	wk->Data[ wk->list_no ].param = param;
	wk->list_no++;

	return;
}

//--------------------------------------------------------------
/**
 * @brief	BMPメニュー	項目の中から最大文字数を取得
 *
 * @param	wk			FSEV_WIN_WORK型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
static u32 BmpMenu_length_get( FSEVWIN_PTR wk )
{
	int i;
	u32 ret,tmp_ret;

	ret		= 0;
	tmp_ret = 0;

	for( i=0; i < wk->list_no ;i++ ){
		if( wk->Data[i].str == NULL ){
			break;
		}

		//ret = FontProc_GetPrintStrWidth( FSEV_FONT, wk->msg_buf[i], 0 );
		ret = FontProc_GetPrintStrWidth( FSEV_FONT, (STRBUF*)wk->Data[i].str, 0 );

		if( tmp_ret < ret ){
			tmp_ret = ret;
		}
	}

	return tmp_ret + FSEV_WIN_CURSOR_WIDTH;
}

//--------------------------------------------------------------
/**
 * @brief	BMPメニュ	ヘッダー初期設定
 *
 * @param	wk			FSEV_WIN_WORK型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void BmpMenu_h_default_set(FSEVWIN_PTR wk)
{
	wk->MenuH.menu		= wk->Data;

	wk->MenuH.win		= &wk->bmpwin;

	wk->MenuH.font		= FSEV_FONT;
	wk->MenuH.x_max		= 1;
	wk->MenuH.y_max		= wk->list_no;
	wk->MenuH.line_spc	= 0;
	wk->MenuH.c_disp_f	= 0;

	//メニュー項目が4種類以上だった時はカーソルのループ有りにする
	if( wk->list_no >= 4 ){
		wk->MenuH.loop_f	= 1;			//有
	}else{
		wk->MenuH.loop_f	= 0;			//無
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	BMPメニュー　メイン
 *
 * @param	tcb			TCB_PTR
 * @param	wk			ワークのアドレス
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void EvBmpMenu_MainTCB( TCB_PTR tcb, void* wk )
{
	u32 ret;
	FSEV_WIN_WORK *swk = wk;

	//ウェイト
	if( swk->wait != 0 ){
		swk->wait--;
		return;
	}

	//ワイプ中は処理しない
	if( WIPE_SYS_EndCheck() == FALSE ){
		return;
	}

	ret = BmpMenuMain( swk->mw );

	//会話ウィンドウのメッセージ更新
	if( (sys.repeat & PAD_KEY_UP) || (sys.repeat & PAD_KEY_DOWN) ||
		(sys.repeat & PAD_KEY_LEFT) || (sys.repeat & PAD_KEY_RIGHT) ){
		menu_talk_msg_update( swk );
	}

	//フロンティアの施設で、強制削除の値が入っていたら
	if( *swk->work == FSEV_WIN_COMPULSION_DEL ){

		EvBmpMenu_Del(wk);

	}else{

		switch( ret ){
		case BMPMENU_NULL:
			break;
		case BMPMENU_CANCEL:
			if( swk->cancel == TRUE ){			//TRUE = Bキャンセル有効
				*swk->work = FSEV_WIN_B_CANCEL;	//選択した値をワークに代入
				EvBmpMenu_Del(wk);
			}
			break;
		default:
			*swk->work = ret;					//選択した値をワークに代入
			EvBmpMenu_Del(wk);
			break;
		};

	}

	return;
};

//--------------------------------------------------------------
/**
 * @brief	BMPメニュー　終了
 *
 * @param	wk			FSEV_WIN_WORK型のポインタ
 *
 * @retval	none
 *
 * SEQ_SE_DP_SELECTを鳴らしている！
 */
//--------------------------------------------------------------
static void EvBmpMenu_Del( FSEVWIN_PTR wk )
{
	int i;

	Snd_SePlay(SEQ_SE_DP_SELECT);	//注意！

	BmpMenuExit( wk->mw, NULL );
	BmpMenuWinClear( wk->MenuH.win, WINDOW_TRANS_ON );
	GF_BGL_BmpWinDel( wk->MenuH.win );

	for( i=0; i < FSEV_WIN_MSG_MAX ;i++ ){
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

//--------------------------------------------------------------
/**
 * @brief	会話ウィンドウにメッセージを一括表示
 *
 * @param	wk			EV_WIN_WORK型のアドレス
 * @param	talk_msg_id	会話メッセージID
 * @param	wait		メッセージウェイト
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void menu_talk_msg_update( FSEVWIN_PTR wk )
{
	u8 csr_pos;

	csr_pos = BmpMenuCursorPosGet( wk->mw );			//カーソル座標取得
	//OS_Printf( "csr_pos = %d\n", csr_pos );

	if( wk->talk_msg_id[csr_pos] != FSEV_WIN_TALK_MSG_NONE ){
		talk_msg_print( wk, wk->talk_msg_id[csr_pos], MSG_ALLPUT );
	}
	return;
}

//--------------------------------------------------------------
/**
 * @brief	BMPメニュー	強制終了
 *
 * @param	wk			EV_WIN_WORK型のポインタ
 *
 * @retval	none
 *
 * バトルキャッスルのみで使用する
 */
//--------------------------------------------------------------
void FSSC_Sub_BmpMenu_Del( FSEVWIN_PTR wk )
{
	FSEV_WIN_WORK* swk;

	if( wk == NULL ){
		return;
	}

	swk = (FSEV_WIN_WORK*)wk;

	*swk->work = FSEV_WIN_B_CANCEL;	//選択した値をワークに代入

#ifdef EV_WIN_SCROLL_CURSOR	//070228
	EvWin_ActorRelease( wk );		//削除
#endif

	EvBmpMenu_Del(wk);
	return;
}


#if 1
//==============================================================================================
//
//	BMPリスト関連
//
//==============================================================================================
FSEVWIN_PTR FSSC_Sub_BmpList_Init( FSS_PTR fss, u8 x, u8 y, u8 cursor, u8 cancel, u16* work, WORDSET* wordset, MSGDATA_MANAGER* msgman );
void FSSC_Sub_BmpList_MakeList( FSEVWIN_PTR wk, u32 msg_id, u32 talk_msg_id, u32 param  );
void FSSC_Sub_BmpList_Start( FSEVWIN_PTR wk );
static void BmpList_list_make( FSEVWIN_PTR wk, u32 msg_id, u32 talk_msg_id, u32 param  );
static u32 BmpList_length_get( FSEVWIN_PTR wk );
static void BmpList_h_default_set(FSEVWIN_PTR wk);
static void	BmpList_CallBack(BMPLIST_WORK* lw,u32 param,u8 y);
static void BmpList_CursorMoveCallBack( BMPLIST_WORK* wk, u32 param, u8 mode );
static void EvBmpList_MainTCB( TCB_PTR tcb, void* wk );
static void EvBmpList_Del( FSEVWIN_PTR wk, u8 se_flag );
static void list_talk_msg_update( FSEVWIN_PTR wk );
void FSSC_Sub_BmpList_Del( FSEVWIN_PTR wk );

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
 * @retval	"EV_WIN_WORK型のアドレス、NULLは失敗"
 */
//--------------------------------------------------------------
FSEVWIN_PTR FSSC_Sub_BmpList_Init( FSS_PTR fss, u8 x, u8 y, u8 cursor, u8 cancel, u16* work, WORDSET* wordset, MSGDATA_MANAGER* msgman )
{
	return FSSC_Sub_BmpMenu_Init( fss, x, y, cursor, cancel, work, wordset, msgman );
}

//--------------------------------------------------------------
/**
 * @brief	BMPリスト　リスト作成
 *
 * @param	wk			EV_WIN_WORK型のポインタ
 * @param	msg_id		メッセージID
 * @param	talk_msg_id	会話メッセージID
 * @param	param		BMPLISTパラメータ
 *
 * @retval	none
 */
//--------------------------------------------------------------
void FSSC_Sub_BmpList_MakeList( FSEVWIN_PTR wk, u32 msg_id, u32 talk_msg_id, u32 param  )
{
	BmpList_list_make( wk, msg_id, talk_msg_id, param  );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	BMPリスト　開始
 *
 * @param	wk			EV_WIN_WORK型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
void FSSC_Sub_BmpList_Start( FSEVWIN_PTR wk )
{
	u32 len;
	FMAP_PTR fmap = FSS_GetFMapAdrs(wk->fss);

	//(最大文字数＋カーソル) * フォントサイズ
	//len = (BmpList_length_get(wk)+1) * FontHeaderGet(FSEV_FONT, FONT_HEADER_SIZE_X);
	len = BmpList_length_get(wk);

	if( (len % 8) == 0 ){
		len = (len / 8);
	}else{
		len = (len / 8)+1;
	}

	//表示最大項目数チェック
	if( wk->list_no > EV_LIST_LINE ){
		GF_BGL_BmpWinAdd( fmap->bgl, &wk->bmpwin, FRMAP_FRAME_WIN, wk->x, wk->y, 
									len, EV_LIST_LINE*2, FFD_SYSFONT_PAL, FFD_FREE_CGX );
	}else{
		GF_BGL_BmpWinAdd( fmap->bgl, &wk->bmpwin, FRMAP_FRAME_WIN, wk->x, wk->y, 
									len, wk->list_no*2, FFD_SYSFONT_PAL, FFD_FREE_CGX );
	}

	//メニューウィンドウのグラフィックをセット
//	MenuWinGraphicSet(
//		wk->fsys->bgl, FLD_MBGFRM_FONT, MENU_WIN_CGX_NUM, MENU_WIN_PAL, 0, HEAPID_FIELD );

	//メニューウィンドウを描画
	BmpMenuWinWrite(&wk->bmpwin, WINDOW_TRANS_OFF, FR_MENU_WIN_CGX_NUM, FR_MENU_WIN_PAL);

#ifdef EV_WIN_SCROLL_CURSOR	//070228
	EvWin_ActorInit( wk );
#endif

	//リストヘッダー初期設定
	BmpList_h_default_set(wk);

	wk->lw = BmpListSet( (const BMPLIST_HEADER*)&wk->ListH, 0, wk->cursor_pos, wk->fss->heap_id );

	//会話ウィンドウ更新
	list_talk_msg_update( wk );

	//TCB追加
	wk->tcb	= TCB_Add( EvBmpList_MainTCB, wk, 0 );

	return;
}

//--------------------------------------------------------------
/**
 * @brief	BMPリスト	リスト作成
 *
 * @param	wk			EV_WIN_WORK型のポインタ
 * @param	msg_id		メッセージID
 * @param	talk_msg_id	会話メッセージID
 * @param	param		BMPLISTパラメータ
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void BmpList_list_make( FSEVWIN_PTR wk, u32 msg_id, u32 talk_msg_id, u32 param  )
{
	int i;
	void* msg;

	SDK_ASSERTMSG( wk->list_no < EV_WIN_LIST_MAX, "リスト項目数オーバー！" );

#if 1
	{
		//展開込み
		
		STRBUF* tmp_buf2= STRBUF_Create( FSEVWIN_MSG_BUF_SIZE, wk->fss->heap_id );//コピー用バッファ

		MSGMAN_GetString( wk->msgman, msg_id, tmp_buf2 );
		WORDSET_ExpandStr( wk->wordset, wk->msg_buf[wk->list_no], tmp_buf2 );	//展開
		wk->list_Data[ wk->list_no ].str = (const void *)wk->msg_buf[wk->list_no];

		STRBUF_Delete( tmp_buf2 );
	}
#else

	MSGMAN_GetString( wk->msgman, msg_id, wk->msg_buf[wk->list_no] );
	wk->list_Data[ wk->list_no ].str = (const void *)wk->msg_buf[wk->list_no];
	//*st = (const void *)&wk->msg_buf[wk->list_no][0];
#endif
	if( param == FSEV_WIN_LIST_LABEL ){
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
 * @param	wk			EV_WIN_WORK型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
static u32 BmpList_length_get( FSEVWIN_PTR wk )
{
	int i;
	u32 ret,tmp_ret;

	ret		= 0;
	tmp_ret = 0;

	for( i=0; i < wk->list_no ;i++ ){
		if( wk->list_Data[i].str == NULL ){
			break;
		}

		//ret = FontProc_GetPrintStrWidth( FSEV_FONT, wk->msg_buf[i], 0 );
		ret = FontProc_GetPrintStrWidth( FSEV_FONT, (STRBUF*)wk->list_Data[i].str, 0 );
		
		if( tmp_ret < ret ){
			tmp_ret = ret;
		}
	}

	return tmp_ret + FSEV_WIN_CURSOR_WIDTH;
}

//--------------------------------------------------------------
/**
 * @brief	BMPリスト	ヘッダー初期設定
 *
 * @param	wk			EV_WIN_WORK型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void BmpList_h_default_set(FSEVWIN_PTR wk)
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
#if 0
	wk->ListH.page_skip	= BMPLIST_LRKEY_SKIP;
#else
	wk->ListH.page_skip	= BMPLIST_NO_SKIP;				//08.05.31変更
#endif
	wk->ListH.font		= FSEV_FONT;
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
	FSEVWIN_PTR evwin_wk = (FSEVWIN_PTR)BmpListParamGet(wk,BMPLIST_ID_WORK);

#ifdef EV_WIN_SCROLL_CURSOR	//070228

	//初期化時
	if( mode == 1 ){
		//
	}

	count = BmpListParamGet( wk, BMPLIST_ID_COUNT );	//リスト項目数
	line  = BmpListParamGet( wk, BMPLIST_ID_LINE );		//表示最大項目数

	//全ての項目を表示していない時
	if( count > line ){

		BmpListPosGet( wk, &list_bak, &cursor_bak );

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
	FSEV_WIN_WORK* swk;
	swk = (FSEV_WIN_WORK*)wk;

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

	//会話ウィンドウのメッセージ更新
	if( (sys.repeat & PAD_KEY_UP) || (sys.repeat & PAD_KEY_DOWN) ||
		(sys.repeat & PAD_KEY_LEFT) || (sys.repeat & PAD_KEY_RIGHT) ){
		list_talk_msg_update( swk );
	}

#ifdef EV_WIN_SCROLL_CURSOR	//070228
	CLACT_Draw( swk->fcat.cas );
#endif

	//フロンティアの施設で、強制削除の値が入っていたら
	if( *swk->work == FSEV_WIN_COMPULSION_DEL ){

#ifdef EV_WIN_SCROLL_CURSOR	//070228
		EvWin_ActorRelease( wk );				//削除
#endif
		EvBmpList_Del(wk,0);

	}else{

		switch( ret ){
		case BMPLIST_NULL:
			break;
		case BMPLIST_CANCEL:
			if( swk->cancel == TRUE ){			//TRUE = Bキャンセル有効
				Snd_SePlay( SEQ_SE_DP_SELECT );
				*swk->work = FSEV_WIN_B_CANCEL;	//選択した値をワークに代入
#ifdef EV_WIN_SCROLL_CURSOR	//070228
				EvWin_ActorRelease( wk );		//削除
#endif
				EvBmpList_Del(wk,1);
			}
			break;
		default:
			Snd_SePlay( SEQ_SE_DP_SELECT );
			*swk->work = ret;					//選択した値をワークに代入
#ifdef EV_WIN_SCROLL_CURSOR	//070228
			EvWin_ActorRelease( wk );			//削除
#endif
			EvBmpList_Del(wk,1);
			break;
		};

	}

	return;
};

//--------------------------------------------------------------
/**
 * @brief	BMPリスト　リスト終了
 *
 * @param	wk			EV_WIN_WORK型のポインタ
 *
 * @retval	none
 *
 * SEQ_SE_DP_SELECTを鳴らしている！
 */
//--------------------------------------------------------------
static void EvBmpList_Del( FSEVWIN_PTR wk, u8 se_flag )
{
	int i;

	if( se_flag == 1 ){
		Snd_SePlay(SEQ_SE_DP_SELECT);	//注意！
	}

	BmpListExit( wk->lw, NULL, NULL );
	BmpMenuWinClear( wk->ListH.win, WINDOW_TRANS_ON );
	GF_BGL_BmpWinDel( &wk->bmpwin );

	for( i=0; i < FSEV_WIN_MSG_MAX ;i++ ){
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

//--------------------------------------------------------------
/**
 * @brief	会話ウィンドウにメッセージを一括表示
 *
 * @param	wk			EV_WIN_WORK型のアドレス
 * @param	talk_msg_id	会話メッセージID
 * @param	wait		メッセージウェイト
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void talk_msg_print( FSEVWIN_PTR wk, u16 talk_msg_id, u32 wait )
{
	STRBUF* tmp_buf = STRBUF_Create( FSEVWIN_MSG_BUF_SIZE, wk->fss->heap_id );
	STRBUF* tmp_buf2= STRBUF_Create( FSEVWIN_MSG_BUF_SIZE, wk->fss->heap_id );

	//メッセージクリア
	GF_BGL_BmpWinDataFill( wk->talk_bmpwin, (FBMP_COL_WHITE) );

	MSGMAN_GetString( wk->msgman, talk_msg_id, tmp_buf );

	//登録された単語を使って文字列展開する
	WORDSET_ExpandStr( wk->wordset, tmp_buf2, tmp_buf );

	GF_STR_PrintSimple( wk->talk_bmpwin, FONT_TALK, tmp_buf2, 0, 0, wait, NULL );

	STRBUF_Delete( tmp_buf );
	STRBUF_Delete( tmp_buf2 );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	会話ウィンドウにメッセージを一括表示
 *
 * @param	wk			EV_WIN_WORK型のアドレス
 * @param	talk_msg_id	会話メッセージID
 * @param	wait		メッセージウェイト
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void list_talk_msg_update( FSEVWIN_PTR wk )
{
	BmpListDirectPosGet( wk->lw, &wk->cursor_bak );		//カーソル座標取得
	if( wk->talk_msg_id[wk->cursor_bak] != FSEV_WIN_TALK_MSG_NONE ){
		talk_msg_print( wk, wk->talk_msg_id[wk->cursor_bak], MSG_ALLPUT );
	}
	return;
}

//--------------------------------------------------------------
/**
 * @brief	BMPリスト	強制終了
 *
 * @param	wk			EV_WIN_WORK型のポインタ
 *
 * @retval	none
 *
 * バトルキャッスルのみで使用する
 */
//--------------------------------------------------------------
void FSSC_Sub_BmpList_Del( FSEVWIN_PTR wk )
{
	FSEV_WIN_WORK* swk;

	if( wk == NULL ){
		return;
	}

	swk = (FSEV_WIN_WORK*)wk;

	*swk->work = FSEV_WIN_B_CANCEL;	//選択した値をワークに代入

#ifdef EV_WIN_SCROLL_CURSOR	//070228
	EvWin_ActorRelease( wk );		//削除
#endif

	EvBmpList_Del(wk,0);
	return;
}

#endif


//==============================================================================
//
//	OBJアニメーション
//
//==============================================================================

//--------------------------------------------------------------
//	
//--------------------------------------------------------------
///アニメーションウェイトテーブル
ALIGN4 static const u8 AnmCmdWaitTbl[] = {
	1,		//WAIT_1F
	2,		//WAIT_2F
	4,		//WAIT_4F
	8,		//WAIT_8F
	15,		//WAIT_15F
	16,		//WAIT_16F
	32,		//WAIT_32F
};

///アニメーションコマンドデータテーブル
static ANM_CMD_TBL_DATA AnmCmdTbl[] = {
	{WF2DMAP_CMD_NONE,	WF2DMAP_WAY_UP},	//FC_DIR_U	
	{WF2DMAP_CMD_NONE,	WF2DMAP_WAY_DOWN},	//FC_DIR_D	
	{WF2DMAP_CMD_NONE,	WF2DMAP_WAY_LEFT},	//FC_DIR_L	
	{WF2DMAP_CMD_NONE,	WF2DMAP_WAY_RIGHT},	//FC_DIR_R	
	{WF2DMAP_CMD_WALK,	WF2DMAP_WAY_UP},	//FC_WALK_U_8F
	{WF2DMAP_CMD_WALK,	WF2DMAP_WAY_DOWN},	//FC_WALK_D_8F
	{WF2DMAP_CMD_WALK,	WF2DMAP_WAY_LEFT},	//FC_WALK_L_8F
	{WF2DMAP_CMD_WALK,	WF2DMAP_WAY_RIGHT},	//FC_WALK_R_8F
	{WF2DMAP_CMD_SLOWWALK,	WF2DMAP_WAY_UP},	//FC_WALK_U_16F
	{WF2DMAP_CMD_SLOWWALK,	WF2DMAP_WAY_DOWN},	//FC_WALK_D_16F
	{WF2DMAP_CMD_SLOWWALK,	WF2DMAP_WAY_LEFT},	//FC_WALK_L_16F
	{WF2DMAP_CMD_SLOWWALK,	WF2DMAP_WAY_RIGHT},	//FC_WALK_R_16F
	{WF2DMAP_CMD_WALK4,	WF2DMAP_WAY_UP},	//FC_WALK_U_4F
	{WF2DMAP_CMD_WALK4,	WF2DMAP_WAY_DOWN},	//FC_WALK_D_4F
	{WF2DMAP_CMD_WALK4,	WF2DMAP_WAY_LEFT},	//FC_WALK_L_4F
	{WF2DMAP_CMD_WALK4,	WF2DMAP_WAY_RIGHT},	//FC_WALK_R_4F
	{WF2DMAP_CMD_STAYWALK8,	WF2DMAP_WAY_UP},	//FC_STAYWALK_U_8F
	{WF2DMAP_CMD_STAYWALK8,	WF2DMAP_WAY_DOWN},	//FC_STAYWALK_D_8F
	{WF2DMAP_CMD_STAYWALK8,	WF2DMAP_WAY_LEFT},	//FC_STAYWALK_L_8F
	{WF2DMAP_CMD_STAYWALK8,	WF2DMAP_WAY_RIGHT},	//FC_STAYWALK_R_8F
	{WF2DMAP_CMD_STAYWALK16,WF2DMAP_WAY_UP},	//FC_STAYWALK_U_16F
	{WF2DMAP_CMD_STAYWALK16,WF2DMAP_WAY_DOWN},	//FC_STAYWALK_D_16F
	{WF2DMAP_CMD_STAYWALK16,WF2DMAP_WAY_LEFT},	//FC_STAYWALK_L_16F
	{WF2DMAP_CMD_STAYWALK16,WF2DMAP_WAY_RIGHT},	//FC_STAYWALK_R_16F
	{WF2DMAP_CMD_STAYWALK2,	WF2DMAP_WAY_UP},	//FC_STAYWALK_U_2F
	{WF2DMAP_CMD_STAYWALK2,	WF2DMAP_WAY_DOWN},	//FC_STAYWALK_D_2F
	{WF2DMAP_CMD_STAYWALK2,	WF2DMAP_WAY_LEFT},	//FC_STAYWALK_L_2F
	{WF2DMAP_CMD_STAYWALK2,	WF2DMAP_WAY_RIGHT},	//FC_STAYWALK_R_2F
	{WF2DMAP_CMD_STAYWALK4,	WF2DMAP_WAY_UP},	//FC_STAYWALK_U_4F
	{WF2DMAP_CMD_STAYWALK4,	WF2DMAP_WAY_DOWN},	//FC_STAYWALK_D_4F
	{WF2DMAP_CMD_STAYWALK4,	WF2DMAP_WAY_LEFT},	//FC_STAYWALK_L_4F
	{WF2DMAP_CMD_STAYWALK4,	WF2DMAP_WAY_RIGHT},	//FC_STAYWALK_R_4F
};

//--------------------------------------------------------------
/**
 * @brief   フィールドOBJアニメーション実行メイン
 *
 * @param   tcb			TCBへのポインタ
 * @param   work		FSSC_ANIME_MOVE_WORK構造体
 */
//--------------------------------------------------------------
void FSSC_Sub_AnimeListMain(TCB_PTR tcb, void *work)
{
	FSSC_ANIME_MOVE_WORK *move = work;
	WF2DMAP_ACTCMD act;
	u16 code, num;
	enum{
		SEQ_DATA_CHECK,
		SEQ_ANM_REQ,
		SEQ_END,
	};
	
	code = move->anm_list->code;
	num = move->anm_list->num;
	switch(move->seq){
	case SEQ_DATA_CHECK:
		if(code == FSS_DATA_END_CODE){
			move->seq = SEQ_END;
			break;
		}
		//break;
	case SEQ_ANM_REQ:
		if(WF2DMAP_OBJWkDataGet(move->fss_actor->objwk, WF2DMAP_OBJPM_ST) == WF2DMAP_OBJST_NONE){
			//waitチェック
			if(code >= FC_WAIT_1F && code <= FC_WAIT_32F){
				move->wait++;
				if(move->wait >= AnmCmdWaitTbl[code - FC_WAIT_1F]){
					move->wait = 0;
					move->anm_list++;
				}
				break;
			}
			else if(code >= FC_SYSCMD_START && code < FC_SYSCMD_END){
				switch(code){
				case FC_SYSCMD_VISIBLE_ON:
					WF2DMAP_OBJDrawWkDrawFlagSet(move->fss_actor->drawwk, ON);
					break;
				case FC_SYSCMD_VISIBLE_OFF:
					WF2DMAP_OBJDrawWkDrawFlagSet(move->fss_actor->drawwk, OFF);
					break;
				default:
					GF_ASSERT(0);	//存在しないコマンド
					break;
				}
				move->anm_list++;
				break;
			}
			
		#if 0	//スクリプトのOBJアニメはマップや人物との当たり判定を無視するため
				//アクションコマンドを使用するように変更
			req.cmd = AnmCmdTbl[code].cmd;
			req.way = AnmCmdTbl[code].way;
			req.playid = move->playid;
			WF2DMAP_REQCMDQSysCmdPush(move->reqcmd_q, &req);
		#else
			FSSC_Sub_ActCmdCreate(&act, move->fss_actor->objwk, move->playid, code);
			WF2DMAP_ACTCMDQSysCmdPush(move->actcmd_q, &act);
		#endif
		
			move->num++;
			if(move->num >= num){
				move->num = 0;
				move->anm_list++;
			}
			move->seq = SEQ_DATA_CHECK;
		}
		break;
	case SEQ_END:
		if(WF2DMAP_OBJWkDataGet(move->fss_actor->objwk, WF2DMAP_OBJPM_ST) == WF2DMAP_OBJST_NONE){
			(*(move->anm_count_ptr))--;
			move->fss_actor->anime_tcb = NULL;
			sys_FreeMemoryEz(move);
			TCB_Delete(tcb);
			return;
		}
		break;
	}
}

//--------------------------------------------------------------
/**
 * @brief   アクションコマンドを作成
 *
 * @param   act			代入先
 * @param   objwk		対象のオブジェクトワーク
 * @param   playid		認識ID
 * @param   code		アニメコード(FC_DIR_U、FC_WAIT_1F、等)
 */
//--------------------------------------------------------------
void FSSC_Sub_ActCmdCreate(WF2DMAP_ACTCMD *act, WF2DMAP_OBJWK *objwk, int playid, int code)
{
	//act->playid = WF2DMAP_OBJWkDataGet(objwk, WF2DMAP_OBJPM_PLID );
	act->playid = playid;
	act->way = AnmCmdTbl[code].way;
	//act->way = WF2DMAP_OBJWkDataGet(objwk, WF2DMAP_OBJPM_WAY );
	act->cmd = AnmCmdTbl[code].cmd;
	if( (act->cmd == WF2DMAP_CMD_WALK) || (act->cmd == WF2DMAP_CMD_RUN) 
			|| (act->cmd == WF2DMAP_CMD_SLOWWALK)){
		act->pos = WF2DMAP_OBJWkLastMatrixGet(objwk);
	}else{
		act->pos = WF2DMAP_OBJWkMatrixGet(objwk);
	}
}

//==============================================================================
//	ポケモン表示
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   ポケモンアクターを生成する
 *
 * @param   fmap			
 * @param   pp				表示するポケモンのデータ
 * @param   heap_id			ヒープID(関数内でテンポラリとして使用)
 * @param   manager_id		アクター＆リソース管理ID (POKE_DISP_MANAGER_ID_???)
 * @param   x				絶対座標X
 * @param   y				絶対座標Y
 * @param   soft_pri		ソフトプライオリティ
 * @param   bg_pri			BGプライオリティ
 * @param   evy				カラー加減算EVY値
 * @param   next_rgb		カラー加減算後の色
 */
//--------------------------------------------------------------
void FSSC_Sub_PokemonActorAdd(FMAP_PTR fmap, POKEMON_PARAM *pp, int heap_id, 
	int manager_id, int x, int y, int soft_pri, int bg_pri, int evy, u16 next_rgb)
{
	CATS_SYS_PTR csp = fmap->clactsys.csp;
	CATS_RES_PTR crp = fmap->clactsys.crp;
	PALETTE_FADE_PTR pfd = fmap->pfd;
	SOFT_SPRITE_ARC  ssa;
	void *decord_buf;
	CATS_ACT_PTR cap;
	
	//ダミーのリソースデータをマネージャに登録
	{
		ARCHANDLE* hdl;

		hdl  = ArchiveDataHandleOpen( ARC_BATT_OBJ,  heap_id ); 

		CATS_LoadResourceCharArcH(csp, crp, hdl, POKE_OAM128K_NCGR, 0, 
			NNS_G2D_VRAM_TYPE_2DMAIN, manager_id);
		CATS_LoadResourcePlttWorkArcH(pfd, FADE_MAIN_OBJ, csp, crp, hdl, POKE_OAM128K_NCLR, 0, 
			NNS_G2D_VRAM_TYPE_2DMAIN, 1, manager_id);
		CATS_LoadResourceCellArcH(csp, crp, hdl, POKE_OAM128K_NCER, 0, manager_id);
		CATS_LoadResourceCellAnmArcH(csp, crp, hdl, POKE_OAM128K_NANR, 0, manager_id);

		ArchiveDataHandleClose( hdl );
	}

	//アクター生成
	{
		int n;
		TCATS_OBJECT_ADD_PARAM_S coap;
		
		MI_CpuClear8(&coap, sizeof(TCATS_OBJECT_ADD_PARAM_S));
		coap.x		= x;
		coap.y		= y;
		coap.z		= 0;
		coap.anm	= 0;
		coap.pri	= soft_pri;
		coap.pal	= 0;
		coap.d_area = NNS_G2D_VRAM_TYPE_2DMAIN;
		coap.bg_pri = bg_pri;
		coap.vram_trans = 0;
		for (n = 0; n < CLACT_U_RES_MAX; n++){
			coap.id[ n ] = manager_id;
		}
		cap = CATS_ObjectAdd_S(csp, crp, &coap);
		CATS_ObjectUpdateCap(cap);
	}

	//ポケモングラフィックデータをワークに読み込み
	{
		u32 personal_rnd, monsno;

		decord_buf = sys_AllocMemory(heap_id, POKE_TEX_SIZE);
		personal_rnd = PokeParaGet(pp, ID_PARA_personal_rnd, NULL);
		monsno = PokeParaGet(pp, ID_PARA_monsno, NULL);
		
		PokeGraArcDataGetPP(&ssa, pp, PARA_FRONT);
		
		Ex_ChangesInto_OAM_from_PokeTex(ssa.arc_no, ssa.index_chr, heap_id, 
			POKE_TEX_X, POKE_TEX_Y, POKE_TEX_W, POKE_TEX_H, decord_buf, 
			personal_rnd, FALSE, PARA_FRONT, monsno);
	}

	//ダミーのリソースに正式データを転送しなおす
	{
		NNSG2dImageProxy * image;
		NNSG2dImagePaletteProxy* palette;
		int pal_no;
		
		//キャラクタ
		image = CLACT_ImageProxyGet(cap->act);
		DC_FlushRange(decord_buf, sizeof(POKE_TEX_SIZE));
		GX_LoadOBJ(decord_buf, 
			image->vramLocation.baseAddrOfVram[NNS_G2D_VRAM_TYPE_2DMAIN], POKE_TEX_SIZE);
		
		//パレット
		palette = CLACT_PaletteProxyGet(cap->act);
		pal_no = GetPlttProxyOffset(palette, NNS_G2D_VRAM_TYPE_2DMAIN);
		PaletteWorkSet_Arc(pfd, ssa.arc_no, ssa.index_pal, heap_id, 
			FADE_MAIN_OBJ, 0x20, pal_no*16);
		//カラー加減算
		if(evy > 0){
			SoftFadePfd(pfd, FADE_MAIN_OBJ, pal_no * 16, 16, evy, next_rgb);
		}
	}

	sys_FreeMemoryEz(decord_buf);
	GF_ASSERT(fmap->poke_cap[manager_id - POKE_DISP_MANAGER_ID_START] == NULL);
	fmap->poke_cap[manager_id - POKE_DISP_MANAGER_ID_START] = cap;
}

//--------------------------------------------------------------
/**
 * @brief   ポケモンアクターを削除する
 *
 * @param   fmap			
 * @param   manager_id		マネージャID(POKE_DISP_MANAGER_ID_???)
 */
//--------------------------------------------------------------
void FSSC_Sub_PokemonDispDelete(FMAP_PTR fmap, int manager_id)
{
	//アクター削除
	CATS_ActorPointerDelete_S(fmap->poke_cap[manager_id - POKE_DISP_MANAGER_ID_START]);
	fmap->poke_cap[manager_id - POKE_DISP_MANAGER_ID_START] = NULL;
	
	//リソース削除
	CATS_FreeResourceChar(fmap->clactsys.crp, manager_id);
	CATS_FreeResourcePltt(fmap->clactsys.crp, manager_id);
	CATS_FreeResourceCell(fmap->clactsys.crp, manager_id);
	CATS_FreeResourceCellAnm(fmap->clactsys.crp, manager_id);
}


//==============================================================================
//	ポケモンアイコン
//==============================================================================
///ポケモンアイコン：アクターヘッダ
static const TCATS_OBJECT_ADD_PARAM_S PokeIconObjParam = {
	0, 0, 0,		//x, y, z
	0, 100, 0,		//アニメ番号、優先順位、パレット番号
	NNS_G2D_VRAM_TYPE_2DMAIN,		//描画エリア
	{	//使用リソースIDテーブル
		FR_CHARID_POKEICON_0,				//キャラ
		FR_PLTTID_POKEICON,				//パレット
		FR_CELLID_POKEICON,				//セル
		FR_CELLANMID_POKEICON,				//セルアニメ
		CLACT_U_HEADER_DATA_NONE,		//マルチセル
		CLACT_U_HEADER_DATA_NONE,		//マルチセルアニメ
	},
	1,			//BGプライオリティ
	0,			//Vram転送フラグ
};

///アイテムアイコン：アクターヘッダ
static const TCATS_OBJECT_ADD_PARAM_S ItemIconObjParam = {
	0, 0, 0,		//x, y, z
	0, 99, 0,		//アニメ番号、優先順位、パレット番号
	NNS_G2D_VRAM_TYPE_2DMAIN,		//描画エリア
	{	//使用リソースIDテーブル
		FR_CHARID_ITEMICON,				//キャラ
		FR_PLTTID_ITEMICON,				//パレット
		FR_CELLID_ITEMICON,				//セル
		FR_CELLANMID_ITEMICON,				//セルアニメ
		CLACT_U_HEADER_DATA_NONE,		//マルチセル
		CLACT_U_HEADER_DATA_NONE,		//マルチセルアニメ
	},
	1,			//BGプライオリティ
	0,			//Vram転送フラグ
};

//--------------------------------------------------------------
/**
 * @brief   ポケモンアイコンの共通リソースを登録
 *
 * @param   fmap		
 */
//--------------------------------------------------------------
void FSSC_Sub_PokeIconCommonResourceSet(FMAP_PTR fmap)
{
	//パレット
	CATS_LoadResourcePlttWorkArc(fmap->pfd, FADE_MAIN_OBJ, fmap->clactsys.csp, fmap->clactsys.crp,
		ARC_POKEICON, PokeIconPalArcIndexGet(), 0, POKEICON_PAL_MAX, 
		NNS_G2D_VRAM_TYPE_2DMAIN, FR_PLTTID_POKEICON);
	//セル
	CATS_LoadResourceCellArc(fmap->clactsys.csp, fmap->clactsys.crp, ARC_POKEICON, 
		PokeIconAnmCellArcIndexGet(), 0, FR_CELLID_POKEICON);
	//セルアニメ
	CATS_LoadResourceCellAnmArc(fmap->clactsys.csp, fmap->clactsys.crp, ARC_POKEICON, 
		PokeIconAnmCellAnmArcIndexGet(), 0, FR_CELLANMID_POKEICON);
}

//--------------------------------------------------------------
/**
 * @brief   ポケモンアイコンの共通リソースを破棄する
 *
 * @param   fmap		
 */
//--------------------------------------------------------------
void FSSC_Sub_PokeIconCommonResourceFree(FMAP_PTR fmap)
{
	CATS_FreeResourceCell(fmap->clactsys.crp, FR_CELLID_POKEICON);
	CATS_FreeResourceCellAnm(fmap->clactsys.crp, FR_CELLANMID_POKEICON);
	CATS_FreeResourcePltt(fmap->clactsys.crp, FR_PLTTID_POKEICON);
}

//--------------------------------------------------------------
/**
 * @brief   ポケモンアイコンアクターを生成(キャラクタのリソース登録もする)
 *
 * @param   fmap		
 * @param   pp			生成するポケモンへのポインタ
 * @param   pos			管理番号
 * @param   x			表示座標X
 * @param   y			表示座標Y
 *
 * @retval  生成したアクターへのポインタ
 */
//--------------------------------------------------------------
CATS_ACT_PTR Frontier_PokeIconAdd(FMAP_PTR fmap, POKEMON_PARAM *pp, int pos, int x, int y)
{
	CATS_ACT_PTR cap;
	TCATS_OBJECT_ADD_PARAM_S obj_param;
	
	GF_ASSERT(pos < (FR_CHARID_POKEICON_MAX - FR_CHARID_POKEICON_0));
	
	OS_Printf( "pos = %d\n", (FR_CHARID_POKEICON_0 + pos) );

#if 1
	//キャラリソース	※キャラリソースとアクターはpos毎に個別登録
	CATS_LoadResourceCharArcModeAdjustAreaCont(
		fmap->clactsys.csp, fmap->clactsys.crp, ARC_POKEICON, 
		PokeIconCgxArcIndexGetByPP(pp), 0, NNS_G2D_VRAM_TYPE_2DMAIN, FR_CHARID_POKEICON_0 + pos);
#else
	//キャラリソース	※キャラリソースとアクターはpos毎に個別登録
	CATS_LoadResourceCharArc(fmap->clactsys.csp, fmap->clactsys.crp, ARC_POKEICON, 
		PokeIconCgxArcIndexGetByPP(pp), 0, NNS_G2D_VRAM_TYPE_2DMAIN, FR_CHARID_POKEICON_0 + pos);
#endif

	//アクター登録
	obj_param = PokeIconObjParam;
	obj_param.id[CLACT_U_CHAR_RES] += pos;
	obj_param.x		= x;
	obj_param.y		= y;
	obj_param.pri	= ACT_SOFTPRI_ROULETTE_POKE;
	cap = CATS_ObjectAdd_S(fmap->clactsys.csp, fmap->clactsys.crp, &obj_param);
	//パレット切り替え
	CLACT_PaletteOffsetChgAddTransPlttNo(cap->act, PokeIconPalNumGetByPP(pp));
	
	CATS_ObjectUpdateCap(cap);
	return cap;
}

//--------------------------------------------------------------
/**
 * @brief   ポケモンアイコンの削除を行う(キャラクタリソースの解放も行う)
 *
 * @param   fmap		
 * @param   cap			削除するポケモンアイコンアクターへのポインタ
 * @param   pos			管理番号
 */
//--------------------------------------------------------------
void FSSC_Sub_PokeIconDel(FMAP_PTR fmap, CATS_ACT_PTR cap, int pos)
{
	CATS_FreeResourceChar(fmap->clactsys.crp, FR_CHARID_POKEICON_0 + pos);
	CATS_ActorPointerDelete_S(cap);
}

//--------------------------------------------------------------
/**
 * @brief   アイテムアイコン(リスト用)のリソース登録
 *
 * @param   fmap		
 */
//--------------------------------------------------------------
void FSSC_Sub_ItemIconLoad(FMAP_PTR fmap)
{
	ARCHANDLE* hdl;
	
	hdl = ArchiveDataHandleOpen( ARC_PLIST_GRA, HEAPID_FRONTIERMAP); 
	
	// パレット
	CATS_LoadResourcePlttWorkArcH(
		fmap->pfd, FADE_MAIN_OBJ, fmap->clactsys.csp, fmap->clactsys.crp, hdl,
		Pokelist_ItemIconPalArcGet(), 0, 1, NNS_G2D_VRAM_TYPE_2DMAIN, FR_PLTTID_ITEMICON);
	// セル
	CATS_LoadResourceCellArcH(
		fmap->clactsys.csp, fmap->clactsys.crp, hdl, 
		Pokelist_ItemIconCellArcGet(), 0, FR_CELLID_ITEMICON );
	// セルアニメ
	CATS_LoadResourceCellAnmArcH(
		fmap->clactsys.csp, fmap->clactsys.crp, hdl, 
		Pokelist_ItemIconCAnmArcGet(), 0, FR_CELLANMID_ITEMICON );
	// キャラ
//	CATS_LoadResourceCharArcH(
//		fmap->clactsys.csp, fmap->clactsys.crp, hdl,
//		Pokelist_ItemIconCgxArcGet(), 0, NNS_G2D_VRAM_TYPE_2DMAIN, FR_CHARID_ITEMICON);
	CATS_LoadResourceCharArcModeAdjustAreaCont(
		fmap->clactsys.csp, fmap->clactsys.crp, ARC_PLIST_GRA,
		Pokelist_ItemIconCgxArcGet(), 0, NNS_G2D_VRAM_TYPE_2DMAIN, FR_CHARID_ITEMICON);
		
	ArchiveDataHandleClose( hdl );
}

//--------------------------------------------------------------
/**
 * @brief   アイテムアイコン(リスト用)のリソース解放
 *
 * @param   fmap		
 */
//--------------------------------------------------------------
void FSSC_Sub_ItemIconFree(FMAP_PTR fmap)
{
	CATS_FreeResourceChar(fmap->clactsys.crp, FR_CHARID_ITEMICON);
	CATS_FreeResourceCell(fmap->clactsys.crp, FR_CELLID_ITEMICON);
	CATS_FreeResourceCellAnm(fmap->clactsys.crp, FR_CELLANMID_ITEMICON);
	CATS_FreeResourcePltt(fmap->clactsys.crp, FR_PLTTID_ITEMICON);
}

//--------------------------------------------------------------
/**
 * @brief   ポケモンアイコンアクターを生成(キャラクタのリソース登録もする)
 *
 * @param   fmap		
 * @param   x			表示座標X
 * @param   y			表示座標Y
 *
 * @retval  生成したアクターへのポインタ
 */
//--------------------------------------------------------------
CATS_ACT_PTR Frontier_ItemIconAdd(FMAP_PTR fmap, int x, int y)
{
	CATS_ACT_PTR cap;
	TCATS_OBJECT_ADD_PARAM_S obj_param;
	
	//アクター登録
	obj_param = ItemIconObjParam;
	obj_param.x		= x;
	obj_param.y		= y;
	obj_param.pri	= ACT_SOFTPRI_ROULETTE_ITEM;
	cap = CATS_ObjectAdd_S(fmap->clactsys.csp, fmap->clactsys.crp, &obj_param);
	
	CATS_ObjectUpdateCap(cap);
	return cap;
}

//--------------------------------------------------------------
/**
 * @brief   アイテムアイコンの削除を行う
 *
 * @param   fmap		
 * @param   cap			削除するアイテムアイコンアクターへのポインタ
 */
//--------------------------------------------------------------
void FSSC_Sub_ItemIconDel(FMAP_PTR fmap, CATS_ACT_PTR cap)
{
	CATS_ActorPointerDelete_S(cap);
}

//--------------------------------------------------------------
/**
 * @brief   対戦前トレーナーデータセット
 *
 * @param   
 */
//--------------------------------------------------------------
static BOOL TowerTalkMsgWait(FSS_TASK * core);
void FrontierTalkMsgSub( FSS_TASK* core, u16* msg );
void FrontierTalkMsgSub2( FSS_TASK* core, u16* msg, u32 datID );

void FrontierTalkMsgSub( FSS_TASK* core, u16* msg )
{
	FrontierTalkMsgSub2( core, msg, NARC_msg_tower_trainerpl_dat );		//プラチナ
	return;
}

void FrontierTalkMsgSub2( FSS_TASK* core, u16* msg, u32 datID )
{
	u8 spd;
	MSGDATA_MANAGER * man;
	FRONTIER_EX_PARAM* ex_param = Frontier_ExParamGet( core->fss->fmain );

	if(msg[0] == 0xFFFF){	//ROMMSG----------------------------------------------

		man = MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, datID, HEAPID_EVENT );

		//ScrTalkMsg(core, man,msg[1], 1, NULL);
		FSSC_Sub_ScrTalkMsg(core->fss, man, msg[1], 1, NULL);
		MSGMAN_Delete(man);
	}else{					//簡易会話--------------------------------------------

		spd = CONFIG_GetMsgPrintSpeed( SaveData_GetConfig(ex_param->savedata) );
		FSTalkMsgPMSParam(core->fss,spd,msg[0],msg[1],msg[2],msg[3],1);
	}

	FSST_SetWait( core, TowerTalkMsgWait );
	return;
}

static BOOL TowerTalkMsgWait(FSS_TASK * core)
{
	if(GF_MSG_PrintEndCheck(core->fss->msg_talk_index) == 0){
		return 1;
	}
	return 0;
}


//--------------------------------------------------------------
/**
 * @brief   地震実行タスク
 *
 * @param   tcb			TCBへのポインタ
 * @param   work		FMAP_SHAK_WORK
 */
//--------------------------------------------------------------
void FSSC_Sub_ShakeMove(TCB_PTR tcb, void *work)
{
	FMAP_SHAKE_WORK *shake = work;
	
	shake->wait++;
	if(shake->wait > shake->wait_max){
		shake->wait = 0;
		shake->loop--;
		if(shake->loop < 0){
			TCB_Delete(tcb);
			MI_CpuClear8(shake, sizeof(FMAP_SHAKE_WORK));
			return;
		}
		shake->shake_x = -shake->shake_x;
		shake->shake_y = -shake->shake_y;
	}
}

//--------------------------------------------------------------
/**
 * @brief   Window実行タスク
 *
 * @param   tcb			TCBへのポインタ
 * @param   work		FMAP_WND_WORK
 */
//--------------------------------------------------------------
void FSSC_Sub_Window(TCB_PTR tcb, void *work)
{
	FMAP_WND_WORK *wnd = work;
	
	if(wnd->wait <= 0){
		if(wnd->on_off == TRUE){
			G2_SetWnd0InsidePlane(GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | 
				GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3, TRUE);

			G2_SetWndOutsidePlane(	GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | 
				GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 |
				GX_WND_PLANEMASK_OBJ, TRUE );

			G2_SetWnd0Position(wnd->x1, wnd->y1, wnd->x2, wnd->y2);
			GX_SetVisibleWnd(GX_WNDMASK_W0);
		}
		else{
			GX_SetVisibleWnd(GX_WNDMASK_NONE);
		}
		
		TCB_Delete(tcb);
		MI_CpuClear8(wnd, sizeof(FMAP_WND_WORK));
		return;
	}
	else{
		wnd->wait--;
	}
}
