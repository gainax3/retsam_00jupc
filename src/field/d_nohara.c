//==============================================================================================
/**
 * @file	d_nohara.c
 * @brief	デバックソース
 * @author	Satoshi Nohara
 * @date	2005.07.26
 */
//==============================================================================================
#include "common.h"
#include "system/lib_pack.h"
#include "system/bmp_list.h"
#include "system/fontproc.h"
#include "system/pm_str.h"
#include "system/get_text.h"

#include "system/snd_tool.h"
#include "system/snd_perap.h"
#include "system/wipe.h"

#include "system/savedata.h"
#include "savedata/mystatus.h"
#include "savedata/zukanwork.h"
#include "savedata/battle_rec.h"
#include "savedata/frontier_savedata.h"
#include "savedata/frontier_ex.h"
#include "savedata/factory_savedata.h"
#include "savedata/stage_savedata.h"
#include "savedata/castle_savedata.h"
#include "savedata/roulette_savedata.h"

#include "fld_bgm.h"
#include "poketool/monsno.h"
#include "fieldmap.h"
#include "fieldsys.h"
#include "field_subproc.h"
#include "field_encount.h"
#include "field_battle.h"
#include "fld_bmp.h"
#include "mapdefine.h"						//ZONE_ID_C01
#include "script.h"							//EventSet_Script
#include "scr_tool.h"
#include "ev_mapchange.h"					//

#include "sysflag.h"
#include "syswork.h"

#include "btl_searcher.h"

#include "msgdata/msg.naix"
#include "msgdata/msg_debug_nohara.h"

#include "../fielddata/script/t01r0202_def.h"		//SCRID_TRAINER_MOVE_SCRIPT
#include "../fielddata/script/common_scr_def.h"		//SCRID_TRAINER_MOVE_SCRIPT
#include "../fielddata/script/connect_def.h"		//SCRID_CONNECT_COLOSSEUM
#include "../fielddata/script/trainer_def.h"		//SCRID_TANPAN_01
#include "../fielddata/script/r201_def.h"			//SCRID_R201_FLAG_CHANGE
#include "../fielddata/script/hiden_def.h"			//SCRID_R201_FLAG_CHANGE
#include "../fielddata/script/con_reception_def.h"	//
#include "../fielddata/script/pc_ug_def.h"	//
#include "../fielddata/script/perap_def.h"	//
#include "../fielddata/script/saisen_def.h"	//
#include "../fielddata/script/debug_scr_def.h"	//
#include "../fielddata/script/factory_lobby_def.h"	//
#include "../fielddata/script/seiseki_def.h"	//

#include "itemtool/myitem.h"
#include "itemtool/itemsym.h"

//#include "application/factory.h"
#include "../frontier/factory_def.h"
#include "../frontier/castle_def.h"
#include "../frontier/stage_def.h"
#include "../frontier/roulette_def.h"

#include "field/situation.h"
#include "situation_local.h"
#include "field_poketch.h"


//==============================================================================================
//
//	定義
//
//==============================================================================================
#define D_NOHARA_BMPWIN_MAX	(1)				//BMPウィンドウデータ最大数

typedef void (*voidFunc)(void* func);		//関数ポインタ型


//==============================================================================================
//
//	変数
//
//==============================================================================================
static NNSSndCaptureOutputEffectType stereo_mono = NNS_SND_CAPTURE_OUTPUT_EFFECT_NORMAL;

//static s16 d_buf[ 32*100 ] ATTRIBUTE_ALIGN(32);	//波形格納バッファ(何かの企画？)
static u8 debug_t07r0201_no;

//==============================================================================================
//
//	デバック構造体
//
//==============================================================================================
typedef struct{
	u8	seq;										//処理ナンバー
	u8	wave_buf_flag:1;							//波形格納バッファを確保したフラグ
	u8	waveout_flag:1;								//鳴き声再生中フラグ
	u8	play_flag:6;								//再生中フラグ
	s16 work;										//汎用ワーク

	int friend_work;								//何番目の友達か

	u8	fro_type;
	u8	btl_type;
	u8	fro_seq;
	u8	fro_add;

	u16 list_bak;									//リスト位置バックアップ
	u16 cursor_bak;									//カーソル位置バックアップ

	u32 sel;
	u32 count;										//カウンター
	u32 count2;										//カウンター

	FIELDSYS_WORK* fsys;							//

	GF_BGL_BMPWIN bmpwin[D_NOHARA_BMPWIN_MAX];		//BMPウィンドウデータ
	BMPLIST_WORK* lw;								//BMPリストデータ

	BMPLIST_DATA* menulist;							//

//	s16* wave_buf;									//波形格納バッファのポインタ

	//STRBUF* msg_buf[EV_WIN_MENU_MAX];				//メッセージデータのポインタ
	MSGDATA_MANAGER* msgman;						//メッセージマネージャー
	WORDSET* wordset;								//単語セット

	void* factory_call;								//ファクトリー呼び出し
}D_NOHARA_WORK;


//==============================================================================================
//
//	プロトタイプ宣言
//
//==============================================================================================
void DebugNoharaMenuInit( FIELDSYS_WORK* fsys );

static void DebugNoharaMenuMain( TCB_PTR tcb, void * work );
static void TcbBmpDel( TCB_PTR tcb, void * work );
static void BmpDel( TCB_PTR tcb );
static void NumMsgSet( D_NOHARA_WORK* wk, GF_BGL_BMPWIN* win, int num, u8 x, u8 y );

void DebugBgmMenuInit( FIELDSYS_WORK* fsys );
void DebugScriptMenuInit( FIELDSYS_WORK* fsys );

static void Debug_StageScr_TypeLevelRecordSet( SAVEDATA* sv, u8 type, u8 csr_pos );

//static void D_Nohara_01( TCB_PTR tcb, void * work );
//static void D_Nohara_02( TCB_PTR tcb, void * work );
static void D_Nohara_03( TCB_PTR tcb, void * work );
//static void D_Nohara_04( TCB_PTR tcb, void * work );
//static void D_Nohara_05( TCB_PTR tcb, void * work );
//static void D_Nohara_06( TCB_PTR tcb, void * work );
//static void D_Nohara_07( TCB_PTR tcb, void * work );
//static void D_Nohara_08( TCB_PTR tcb, void * work );
static void D_Nohara_09( TCB_PTR tcb, void * work );
//static void D_Nohara_10( TCB_PTR tcb, void * work );
static void D_Nohara_11( TCB_PTR tcb, void * work );
//static void D_Nohara_12( TCB_PTR tcb, void * work );
//static void D_Nohara_13( TCB_PTR tcb, void * work );
//static void D_Nohara_14( TCB_PTR tcb, void * work );
//static void D_Nohara_15( TCB_PTR tcb, void * work );
static void D_Nohara_16( TCB_PTR tcb, void * work );
static void D_Nohara_17( TCB_PTR tcb, void * work );
//static void D_Nohara_18( TCB_PTR tcb, void * work );
//static void D_Nohara_19( TCB_PTR tcb, void * work );
//static void D_Nohara_20( TCB_PTR tcb, void * work );
static void D_Nohara_21( TCB_PTR tcb, void * work );
//static void D_Nohara_22( TCB_PTR tcb, void * work );
static void D_Nohara_23( TCB_PTR tcb, void * work );
static void D_Nohara_24( TCB_PTR tcb, void * work );
static void D_Nohara_25( TCB_PTR tcb, void * work );
static void D_Nohara_26( TCB_PTR tcb, void * work );
static void D_Nohara_27( TCB_PTR tcb, void * work );
static void D_Nohara_28( TCB_PTR tcb, void * work );
static void D_Nohara_29( TCB_PTR tcb, void * work );
//static void D_Nohara_30( TCB_PTR tcb, void * work );
//static void D_Nohara_31( TCB_PTR tcb, void * work );
//static void D_Nohara_32( TCB_PTR tcb, void * work );
static void D_Nohara_33( TCB_PTR tcb, void * work );
static void D_Nohara_34( TCB_PTR tcb, void * work );
static void D_Nohara_35( TCB_PTR tcb, void * work );
static void D_Nohara_40( TCB_PTR tcb, void * work );
static void D_Nohara_41( TCB_PTR tcb, void * work );
static void D_Nohara_42( TCB_PTR tcb, void * work );


//==============================================================================================
//
//	リストデータ
//
//==============================================================================================
static const struct{
	u32  str_id;
	u32  param;
}DebugMenuList[] = {
	{ msg_debug_nohara_35, (u32)D_Nohara_35 },		//別荘チェック
	{ msg_debug_nohara_34, (u32)D_Nohara_34 },		//フロンティア連勝数操作
	//{ msg_debug_nohara_32, (u32)D_Nohara_32 },		//ＣＰセット
	{ msg_debug_nohara_21, (u32)D_Nohara_21 },		//デバックジャンプ
	//{ msg_debug_nohara_20, (u32)D_Nohara_31 },		//録画再生
	//{ msg_debug_nohara_30, (u32)D_Nohara_30 },		//バトルファクトリーモニター
	{ msg_debug_nohara_11, (u32)D_Nohara_11 },
	{ msg_debug_nohara_40, (u32)D_Nohara_40 },		//フロンティア到着フラグON
	{ msg_debug_nohara_33, (u32)D_Nohara_33 },		//HP5
	{ msg_debug_nohara_42, (u32)D_Nohara_42 },		//hp_rnd,spedef_rndを適当書き換え

	{ msg_debug_nohara_24, (u32)D_Nohara_24 },		//バトルサーチャー
	//{ msg_debug_nohara_01, (u32)D_Nohara_01 },
	//{ msg_debug_nohara_02, (u32)D_Nohara_02 },
	//{ msg_debug_nohara_03, (u32)D_Nohara_03 },
	//{ msg_debug_nohara_04, (u32)D_Nohara_04 },
	//{ msg_debug_nohara_05, (u32)D_Nohara_05 },
	//{ msg_debug_nohara_06, (u32)D_Nohara_06 },
	//{ msg_debug_nohara_07, (u32)D_Nohara_07 },
	//{ msg_debug_nohara_08, (u32)D_Nohara_08 },
	//{ msg_debug_nohara_09, (u32)D_Nohara_09 },
	//{ msg_debug_nohara_10, (u32)D_Nohara_10 },
	//{ msg_debug_nohara_12, (u32)D_Nohara_12 },
	//{ msg_debug_nohara_13, (u32)D_Nohara_13 },
	//{ msg_debug_nohara_14, (u32)D_Nohara_14 },
	//{ msg_debug_nohara_15, (u32)D_Nohara_15 },		//波形テスト
	//{ msg_debug_nohara_16, (u32)D_Nohara_16 },	//メインシナリオ進行
	{ msg_debug_nohara_17, (u32)D_Nohara_17 },		//バッジフラグセット
	//{ msg_debug_nohara_18, (u32)D_Nohara_18 },		//隠しアイテムリスト
	//{ msg_debug_nohara_19, (u32)D_Nohara_19 },		//キャプチャチェック
	//{ msg_debug_nohara_22, (u32)D_Nohara_22 },		//ポケッチ波形テスト
	//{ msg_debug_nohara_23, (u32)D_Nohara_23 },		//図鑑、バッグシステムフラグセット
	{ msg_debug_nohara_27, (u32)D_Nohara_27 },		//ポケモン鳴き声を2つ再生出来るフラグオン
	{ msg_debug_nohara_28, (u32)D_Nohara_28 },		//ポケモン鳴き声を2つ再生出来るフラグオフ
	{ msg_debug_nohara_29, (u32)D_Nohara_29 },		//ウェイト指定ありの鳴き声再生のテスト
	//{ msg_debug_nohara_20, (u32)D_Nohara_20 },		//ギネスウィンドウ
	{ msg_debug_nohara_41, (u32)D_Nohara_41 },		//ステージ連勝数操作(外部セーブが必要)
};

static const BMPLIST_HEADER DebugListH = {
	NULL,					//表示文字データポインタ

	NULL,					//カーソル移動ごとのコールバック関数
	NULL,					//一列表示ごとのコールバック関数

	NULL,					//GF_BGL_BMPWINのポインタ

	NELEMS(DebugMenuList),	//リスト項目数
	9,						//表示最大項目数

	0,						//ラベル表示Ｘ座標
	8,						//項目表示Ｘ座標
	0,						//カーソル表示Ｘ座標
	0,						//表示Ｙ座標
/*
	FBMP_COL_BLACK,			//文字色
	FBMP_COL_WHITE,			//背景色
	FBMP_COL_BLK_SDW,		//文字影色
*/
	FBMP_COL_WHITE,			//文字色
	FBMP_COL_BLACK,			//背景色
	FBMP_COL_BLK_SDW,		//文字影色
	0,						//文字間隔Ｘ
	16,						//文字間隔Ｙ
	BMPLIST_LRKEY_SKIP,		//ページスキップタイプ
	FONT_SYSTEM,			//文字指定(本来は u8 だけど、そんなに作らないと思うので)
	0						//ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)
};


//==============================================================================================
//
//	BMPウィンドウ
//
//==============================================================================================
enum{
	D_NOHARA_BMPWIN_FRAME	= GF_BGL_FRAME3_M,
	D_NOHARA_BMPWIN_PX1		= 1,
	D_NOHARA_BMPWIN_PY1		= 1,
	D_NOHARA_BMPWIN_SX		= 16,
	D_NOHARA_BMPWIN_SY		= 20,
	D_NOHARA_BMPWIN_PL		= FLD_SYSFONT_PAL,
	D_NOHARA_BMPWIN_CH		= 1,
};

static const BMPWIN_DAT	DebugNoharaWinData = {
	D_NOHARA_BMPWIN_FRAME,					//ウインドウ使用フレーム
	D_NOHARA_BMPWIN_PX1,D_NOHARA_BMPWIN_PY1,//ウインドウ領域の左上のX,Y座標（キャラ単位で指定）
	D_NOHARA_BMPWIN_SX,	D_NOHARA_BMPWIN_SY,	//ウインドウ領域のX,Yサイズ（キャラ単位で指定）
	D_NOHARA_BMPWIN_PL,						//ウインドウ領域のパレットナンバー	
	D_NOHARA_BMPWIN_CH						//ウインドウキャラ領域の開始キャラクタナンバー
};


//==============================================================================================
//
//	プログラム
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	デバックメニュー呼び出し(fld_debug.c)
 *
 * @param	fsys	FIELDSYS_WORK型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
void DebugNoharaMenuInit( FIELDSYS_WORK* fsys )
{
	int i;
	BMPLIST_HEADER list_h;
	D_NOHARA_WORK* wk;

	wk = (D_NOHARA_WORK*)TCB_GetWork( PMDS_taskAdd(DebugNoharaMenuMain, sizeof(D_NOHARA_WORK), 0, HEAPID_BASE_DEBUG) );

	wk->seq				= 0;
	wk->sel				= 0;
	wk->work			= 0;
	wk->wave_buf_flag	= 0;								//波形格納バッファを確保したフラグOFF
	wk->waveout_flag	= 0;								//鳴き声再生中フラグOFF
	wk->fsys			= fsys;

	GF_BGL_BmpWinAddEx( wk->fsys->bgl, &wk->bmpwin[0], &DebugNoharaWinData );	//ビットマップ追加

	wk->menulist = BMP_MENULIST_Create( NELEMS(DebugMenuList), HEAPID_BASE_DEBUG );


	wk->wordset = WORDSET_Create( HEAPID_BASE_DEBUG );

	wk->msgman = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, 
								NARC_msg_debug_nohara_dat, HEAPID_BASE_DEBUG);

	for( i=0; i < NELEMS(DebugMenuList); i++ ){
		BMP_MENULIST_AddArchiveString( wk->menulist, wk->msgman, 
										DebugMenuList[i].str_id, DebugMenuList[i].param );
	}

	list_h			= DebugListH;
	list_h.list		= wk->menulist;
	list_h.win		= &wk->bmpwin[0];
	wk->lw			= BmpListSet( &list_h, 0, 0, HEAPID_BASE_DEBUG );

	GF_BGL_BmpWinOn( &wk->bmpwin[0] );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	デバックメニューメイン
 *
 * @param	tcb		TCB_PTR型
 * @param	work	ワーク
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void DebugNoharaMenuMain( TCB_PTR tcb, void * work )
{
	u32	ret;
	D_NOHARA_WORK* wk;
	wk = (D_NOHARA_WORK*)work;

	switch( wk->seq ){
	case 0:
		ret = BmpListMain( wk->lw );

		switch( ret ){
		case BMPLIST_NULL:
			break;
		case BMPLIST_CANCEL:
			TcbBmpDel( tcb, work );				//TCBBMP開放

			//波形再生用チャンネルを開放する
			Snd_DebugNormalChannelFree();

			break;
		default:
			wk->sel = ret;
			wk->seq++;
			break;
		};

		//BMPリストのリスト位置、カーソル位置を取得
		BmpListPosGet( wk->lw, &wk->list_bak, &wk->cursor_bak );
		break;

	case 1:
		{
			voidFunc func = (voidFunc)wk->sel;
			TCB_ChangeFunc( tcb, (void*)func );	//TCBの動作関数切り替え
		}
		break;

	};

	return;
}

//--------------------------------------------------------------
/**
 * @brief	TCB,BMP開放
 *
 * @param	tcb		TCB_PTR型
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void TcbBmpDel( TCB_PTR tcb, void * work )
{
	D_NOHARA_WORK* wk;
	wk = (D_NOHARA_WORK*)work;

	MSGMAN_Delete( wk->msgman );
	WORDSET_Delete( wk->wordset );

#if 0
	//波形格納バッファを確保したフラグONだったら
	if( wk->wave_buf_flag == 1 ){
		sys_FreeMemoryEz( wk->wave_buf );	//バッファ開放
	}
#endif

	BmpDel( tcb );						//BMP開放
	PMDS_taskDel( tcb );				//TCB開放
	FieldSystemProc_SeqHoldEnd();
	return;
}

//--------------------------------------------------------------
/**
 * @brief	BMP開放
 *
 * @param	tcb		TCB_PTR型
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void BmpDel( TCB_PTR tcb )
{
	D_NOHARA_WORK* wk;
	wk = (D_NOHARA_WORK*)TCB_GetWork(tcb);

	//BMP
	BMP_MENULIST_Delete( wk->menulist );
	BmpListExit( wk->lw, &wk->list_bak, &wk->cursor_bak );
	GF_BGL_BmpWinOff( &wk->bmpwin[0] );
	GF_BGL_BmpWinDel( &wk->bmpwin[0] );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	数字メッセージ表示
 *
 * @param	win_index	ビットマップINDEX
 * @param	num			数値
 * @param	x			表示位置X
 * @param	y			表示位置Y
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void NumMsgSet( D_NOHARA_WORK* wk, GF_BGL_BMPWIN* win, int num, u8 x, u8 y )
{
	STRBUF* tmp_buf	= STRBUF_Create( 12, HEAPID_BASE_DEBUG );
	STRBUF* tmp_buf2= STRBUF_Create( 12, HEAPID_BASE_DEBUG );

	MSGMAN_GetString( wk->msgman, msg_debug_nohara_num, tmp_buf );
	WORDSET_RegisterNumber(wk->wordset, 1, num, 4, NUMBER_DISPTYPE_ZERO, NUMBER_CODETYPE_DEFAULT);

	//登録された単語を使って文字列展開する
	WORDSET_ExpandStr( wk->wordset, tmp_buf2, tmp_buf );

	GF_STR_PrintSimple( win, FONT_SYSTEM, tmp_buf2, x, y, MSG_NO_PUT, NULL );

	STRBUF_Delete( tmp_buf );
	STRBUF_Delete( tmp_buf2 );
	return;
}


//==============================================================================================
//
//	01	逆再生関連
//
//==============================================================================================
//static void D_Nohara_01_Main( TCB_PTR tcb, void * work );

//extern BOOL Snd_ArcStrmStart( u16 no );

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_01( TCB_PTR tcb, void * work )

//--------------------------------------------------------------
/**
 * @brief	メイン
 *
 * @param	tcb		TCB_PTR型
 * @param	work	ワーク
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_01_Main( TCB_PTR tcb, void * work )


//==============================================================================================
//
//	02	リバーブ関連
//
//==============================================================================================
//static void D_Nohara_02_Main( TCB_PTR tcb, void * work );

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_02( TCB_PTR tcb, void * work )

//--------------------------------------------------------------
/**
 * @brief	メイン
 *
 * @param	tcb		TCB_PTR型
 * @param	work	ワーク
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_02_Main( TCB_PTR tcb, void * work )


//==============================================================================================
//
//	03	トラックフェード関連
//
//==============================================================================================
//static void D_Nohara_03_Main( TCB_PTR tcb, void * work );

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_03( TCB_PTR tcb, void * work )

//--------------------------------------------------------------
/**
 * @brief	メイン
 *
 * @param	tcb		TCB_PTR型
 * @param	work	ワーク
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_03_Main( TCB_PTR tcb, void * work )


//==============================================================================================
//
//	04	FIELD BGM 音量関連
//
//==============================================================================================
//static void D_Nohara_04_Main( TCB_PTR tcb, void * work );

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_04( TCB_PTR tcb, void * work )

//--------------------------------------------------------------
/**
 * @brief	メイン
 *
 * @param	tcb		TCB_PTR型
 * @param	work	ワーク
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_04_Main( TCB_PTR tcb, void * work )


//==============================================================================================
//
//	05	鳴き声パターン関連
//
//==============================================================================================
//static void D_Nohara_05_Main( TCB_PTR tcb, void * work );

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_05( TCB_PTR tcb, void * work )

//--------------------------------------------------------------
/**
 * @brief	メイン
 *
 * @param	tcb		TCB_PTR型
 * @param	work	ワーク
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_05_Main( TCB_PTR tcb, void * work )


//==============================================================================================
//
//	06	出力エフェクト関連
//
//==============================================================================================
//static void D_Nohara_06_Main( TCB_PTR tcb, void * work );

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_06( TCB_PTR tcb, void * work )

//--------------------------------------------------------------
/**
 * @brief	メイン
 *
 * @param	tcb		TCB_PTR型
 * @param	work	ワーク
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_06_Main( TCB_PTR tcb, void * work )


//==============================================================================================
//
//	07	マイク関連
//
//==============================================================================================
//static void D_Nohara_07_Main( TCB_PTR tcb, void * work );

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_07( TCB_PTR tcb, void * work )
 
//--------------------------------------------------------------
/**
 * @brief	メイン
 *
 * @param	tcb		TCB_PTR型
 * @param	work	ワーク
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_07_Main( TCB_PTR tcb, void * work )


//==============================================================================================
//
//	08	FIELD TEMPO 音量関連
//
//==============================================================================================
//static void D_Nohara_08_Main( TCB_PTR tcb, void * work );

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_08( TCB_PTR tcb, void * work )

//--------------------------------------------------------------
/**
 * @brief	メイン
 *
 * @param	tcb		TCB_PTR型
 * @param	work	ワーク
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_08_Main( TCB_PTR tcb, void * work )


//==============================================================================================
//
//	09	スクリプト関連
//
//==============================================================================================
static void D_Nohara_09_Main( TCB_PTR tcb, void * work );

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_09( TCB_PTR tcb, void * work )
{
	TcbBmpDel( tcb, work );		//TCBBMP開放
	return;
}


//==============================================================================================
//
//	10	トラックフェードフラグ操作
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_10( TCB_PTR tcb, void * work )


//==============================================================================================
//
//	11	スクリプト実行関連
//
//==============================================================================================
static void D_Nohara_11_Main( TCB_PTR tcb, void * work );

static const struct{
	u32  str_id;
	u32  param;
}DebugMenuList11[] = {
	{ msg_debug_nohara_11_01, (u32)D_Nohara_11_Main },
	{ msg_debug_nohara_11_02, (u32)D_Nohara_11_Main },
	{ msg_debug_nohara_11_03, (u32)D_Nohara_11_Main },
};

static const BMPLIST_HEADER ListH11 = {
	NULL,					//表示文字データポインタ
	NULL,					//カーソル移動ごとのコールバック関数
	NULL,					//一列表示ごとのコールバック関数

	NULL,					//GF_BGL_BMPWINのポインタ

	NELEMS(DebugMenuList11),//リスト項目数
	10,						//表示最大項目数

	0,						//ラベル表示Ｘ座標
	8,						//項目表示Ｘ座標
	0,						//カーソル表示Ｘ座標
	0,						//表示Ｙ座標
/*
	FBMP_COL_BLACK,			//文字色
	FBMP_COL_WHITE,			//背景色
	FBMP_COL_BLK_SDW,		//文字影色
*/
	FBMP_COL_WHITE,			//文字色
	FBMP_COL_BLACK,			//背景色
	FBMP_COL_BLK_SDW,		//文字影色
	0,						//文字間隔Ｘ
	16,						//文字間隔Ｙ
	BMPLIST_LRKEY_SKIP,		//ページスキップタイプ
	FONT_SYSTEM,			//文字指定(本来は u8 だけど、そんなに作らないと思うので)
	0						//ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)
};

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_11( TCB_PTR tcb, void * work )
{
	int i;
	BMPLIST_HEADER list_h;
	GF_BGL_BMPWIN* win;
	D_NOHARA_WORK* wk;
	wk = (D_NOHARA_WORK*)work;

	BmpDel( tcb );								//BMP開放

	wk->seq				= 0;
	wk->sel				= 0;
	wk->count			= 0;

	GF_BGL_BmpWinAddEx( wk->fsys->bgl, &wk->bmpwin[0], &DebugNoharaWinData );	//ビットマップ追加

	wk->menulist = BMP_MENULIST_Create( NELEMS(DebugMenuList11), HEAPID_BASE_DEBUG );

	for( i=0; i < NELEMS(DebugMenuList11); i++ ){
		BMP_MENULIST_AddArchiveString( wk->menulist, wk->msgman, 
										DebugMenuList11[i].str_id, DebugMenuList11[i].param );
	}

	list_h			= ListH11;
	list_h.list		= wk->menulist;
	list_h.win		= &wk->bmpwin[0];
	wk->lw			= BmpListSet( &list_h, 0, 0, HEAPID_BASE_DEBUG );

	GF_BGL_BmpWinOn( &wk->bmpwin[0] );

	//TCB_ChangeFunc( tcb, D_Nohara_11_Main );	//TCBの動作関数切り替え
	TCB_ChangeFunc( tcb, DebugNoharaMenuMain );	//TCBの動作関数切り替え
	return;
}
 
//--------------------------------------------------------------
/**
 * @brief	メイン
 *
 * @param	tcb		TCB_PTR型
 * @param	work	ワーク
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_11_Main( TCB_PTR tcb, void * work )
{
	D_NOHARA_WORK* wk;
	wk = (D_NOHARA_WORK*)work;

	switch( wk->cursor_bak ){

	case 0:
		//シェイミイベントでマイを出現させる
		SpScriptStart( wk->fsys, SCRID_DEBUG_R224_SYEIMI );
		break;

	case 1:
		//３つの湖イベント開始
		SpScriptStart( wk->fsys, SCRID_DEBUG_L01_L02_L03_LAKE );
		break;

	case 2:
		//ポケセン地下ストッパー削除、ともだち手帳イベント無効
		SpScriptStart( wk->fsys, SCRID_DEBUG_PC_UG );
		break;

	default:
		//EventSet_Script( wk->fsys, SCRID_TANPAN_01, NULL );
		TcbBmpDel( tcb, work );		//TCBBMP開放
		return;
	};

	wk->seq = 0;
	TCB_ChangeFunc( tcb, DebugNoharaMenuMain );		//TCBの動作関数切り替え
	return;
}


//==============================================================================================
//
//	12	調律乱れ
//
//==============================================================================================
//static void D_Nohara_12_Main( TCB_PTR tcb, void * work );

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_12( TCB_PTR tcb, void * work )

//--------------------------------------------------------------
/**
 * @brief	メイン
 *
 * @param	tcb		TCB_PTR型
 * @param	work	ワーク
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_12_Main( TCB_PTR tcb, void * work )


//==============================================================================================
//
//	13	調律乱れ2
//
//==============================================================================================
//static void D_Nohara_13_Main( TCB_PTR tcb, void * work );

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_13( TCB_PTR tcb, void * work )

//--------------------------------------------------------------
/**
 * @brief	メイン
 *
 * @param	tcb		TCB_PTR型
 * @param	work	ワーク
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_13_Main( TCB_PTR tcb, void * work )


//==============================================================================================
//
//	14	サウンドエフェクト
//
//==============================================================================================
//static void D_Nohara_14_Main( TCB_PTR tcb, void * work );

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_14( TCB_PTR tcb, void * work )

//--------------------------------------------------------------
/**
 * @brief	メイン
 *
 * @param	tcb		TCB_PTR型
 * @param	work	ワーク
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_14_Main( TCB_PTR tcb, void * work )


//==============================================================================================
//
//	15	鳴き声波形関連
//
//==============================================================================================
//static void D_Nohara_15_Main( TCB_PTR tcb, void * work );

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_15( TCB_PTR tcb, void * work )

//--------------------------------------------------------------
/**
 * @brief	メイン
 *
 * @param	tcb		TCB_PTR型
 * @param	work	ワーク
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_15_Main( TCB_PTR tcb, void * work )


//==============================================================================================
//
//	16	メインシナリオ関連
//
//	現在のゾーンのスクリプトとメッセージを読み込むようにしているので、
//	デバックでスクリプトのIDを指定するだけでは出来ない！
//
//	まだスクリプトしか作成していないものは、common_scr.evに追加して試す。。。
//	とも思ったが、メッセージもないので、無理そう。。。
//
//==============================================================================================
static void D_Nohara_16_Main( TCB_PTR tcb, void * work );

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_16( TCB_PTR tcb, void * work )
{
	D_NOHARA_WORK* wk;
	wk = (D_NOHARA_WORK*)work;
	wk->seq		= 0;
	wk->count	= 0;
	wk->work	= 0;
	TCB_ChangeFunc( tcb, D_Nohara_16_Main );	//TCBの動作関数切り替え
	return;
}

//--------------------------------------------------------------
/**
 * @brief	メイン
 *
 * @param	tcb		TCB_PTR型
 * @param	work	ワーク
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_16_Main( TCB_PTR tcb, void * work )
{
	//終了
	if( sys.cont == PAD_BUTTON_B ){
		TcbBmpDel( tcb, work );		//TCBBMP開放
	}

	return;
}


//==============================================================================================
//
//	17	バッジフラグセット
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_17( TCB_PTR tcb, void * work )
{
	int i;
	D_NOHARA_WORK* wk = (D_NOHARA_WORK*)work;
	MYSTATUS* my = SaveData_GetMyStatus( wk->fsys->savedata );

	for( i=0; i < 8; i++ ){
		MyStatus_SetBadgeFlag( my, i );
	}

	TcbBmpDel( tcb, work );		//TCBBMP開放
	return;
}


//==============================================================================================
//
//	18	隠しアイテム検索
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_18( TCB_PTR tcb, void * work )


//==============================================================================================
//
//	19	キャプチャチェック
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_19( TCB_PTR tcb, void * work )


//==============================================================================================
//
//	20	ギネスウィンドウ
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_20( TCB_PTR tcb, void * work )


//==============================================================================================
//
//	21	デバックジャンプ
//
//==============================================================================================
static void D_Nohara_21_Main( TCB_PTR tcb, void * work );

typedef struct{
	u32 zone_id;
	u32 num;
	u32 x;
	u32 z;
}DEBUG_JUMP_WORK;

static const DEBUG_JUMP_WORK debug_jump_work[] = {

	{ ZONE_ID_D24R0106,	2406,	10,		10 },			//
	{ ZONE_ID_D05R0111,	5111,	10,		10 },			//
	{ ZONE_ID_R228,		228,	783,	339 },			//

	{ ZONE_ID_D35R0101,	3501,	10,		10 },			//
	{ ZONE_ID_D35R0102,	3502,	10,		10 },			//
	{ ZONE_ID_D24R0106,	2406,	10,		10 },			//

	{ ZONE_ID_T06R0101,	601,	7,		11 },			//再戦施設
	{ ZONE_ID_D05R0114,	514,	31,		33 },			//やりのはしら
	{ ZONE_ID_D05R0115,	515,	31,		33 },			//やりのはしら
	{ ZONE_ID_D17R0104,	174,	10,		10 },			//もどりのどうくつ
	//{ ZONE_ID_D32R0401,	3241,	16,		16 },			//ステージ受付
	//{ ZONE_ID_D32R0301,	3231,	16,		16 },			//ファクトリー受付
	{ ZONE_ID_T02,		2,		160,	846 },			//マサゴタウン
	{ ZONE_ID_T03,		3,		176,	624 },			//ソノオタウン
	{ ZONE_ID_R201,		201,	110,	856 },			//
	{ ZONE_ID_R205A,	2051,	208,	592 },			//
	{ ZONE_ID_R205B,	2052,	272,	528 },			//
	{ ZONE_ID_C05R1101,	511,	5,		5 },			//コンテスト受付
	{ ZONE_ID_D27R0101, 2701,	46,		53 },			//
};
#define DEBUG_JUMP_WORK_MAX	( NELEMS(debug_jump_work) )

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_21( TCB_PTR tcb, void * work )
{
	D_NOHARA_WORK* wk;
	wk = (D_NOHARA_WORK*)work;
	wk->seq		= 0;
	wk->count	= 0;
	wk->work	= 0;

	NumMsgSet( wk, &wk->bmpwin[0], debug_jump_work[wk->work].num, 8*7, 8*2 );
	GF_BGL_BmpWinOn( &wk->bmpwin[0] );

	TCB_ChangeFunc( tcb, D_Nohara_21_Main );	//TCBの動作関数切り替え
	return;
}

//--------------------------------------------------------------
/**
 * @brief	メイン
 *
 * @param	tcb		TCB_PTR型
 * @param	work	ワーク
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_21_Main( TCB_PTR tcb, void * work )
{
	int flag;
	D_NOHARA_WORK* wk;
	wk = (D_NOHARA_WORK*)work;

	flag = 0;	//書き換えフラグ

	if( (sys.repeat == PAD_KEY_UP) || (sys.cont == PAD_KEY_RIGHT) ){
		flag = 1;
		wk->work++;
		if( wk->work >= DEBUG_JUMP_WORK_MAX ){
			wk->work = 0;
		}
	}

	if( (sys.repeat == PAD_KEY_DOWN) || (sys.cont == PAD_KEY_LEFT) ){
		flag = 1;
		wk->work--;
		if( wk->work < 0 ){
			wk->work = (DEBUG_JUMP_WORK_MAX - 1);
		}
	}

	//書き換えフラグが立っていたら
	if( flag == 1 ){
		//指定範囲を塗りつぶし
		GF_BGL_BmpWinFill( &wk->bmpwin[0], FBMP_COL_BLACK, 8*7, 8*2, 
								8*8, 8*2 );
	
		//値表示
		NumMsgSet( wk, &wk->bmpwin[0], debug_jump_work[wk->work].num, 8*7, 8*2 );
		GF_BGL_BmpWinOn( &wk->bmpwin[0] );
	}

	//ジャンプ
	if( sys.trg == PAD_BUTTON_A ){

		EventSet_EasyMapChange( wk->fsys, debug_jump_work[wk->work].zone_id, DOOR_ID_JUMP_CODE, 
							debug_jump_work[wk->work].x , debug_jump_work[wk->work].z, DIR_DOWN );

		TcbBmpDel( tcb, work );		//TCBBMP開放
		return;
	}

	//終了
	if( sys.cont == PAD_BUTTON_B ){
		TcbBmpDel( tcb, work );		//TCBBMP開放
		return;
	}

	return;
}


//==============================================================================================
//
//	22	ポケッチ波形テスト
//
//==============================================================================================
//static void D_Nohara_22_Main( TCB_PTR tcb, void * work );

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_22( TCB_PTR tcb, void * work )

//--------------------------------------------------------------
/**
 * @brief	メイン
 *
 * @param	tcb		TCB_PTR型
 * @param	work	ワーク
 *
 * @retval	none
 */
//--------------------------------------------------------------
//static void D_Nohara_22_Main( TCB_PTR tcb, void * work )


//==============================================================================================
//
//	23	図鑑、バッグシステムフラグセット
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_23( TCB_PTR tcb, void * work )
{
	int type,ret;
	D_NOHARA_WORK* wk = (D_NOHARA_WORK*)work;

	{
		//メニュー「図鑑」
		ZUKAN_WORK* zw = SaveData_GetZukanWork( wk->fsys->savedata );
		ZukanWork_SetZukanGetFlag( zw );

		//メニュー「バッグ」
		SysFlag_BagSet( SaveData_GetEventWork(wk->fsys->savedata) );

		//メニュー「ポケモン」
		SysWork_FirstPokeNoSet( SaveData_GetEventWork(wk->fsys->savedata), MONSNO_HUSIGIDANE );
	}

	TcbBmpDel( tcb, work );		//TCBBMP開放
	return;
}


//==============================================================================================
//
//	24	バトルサーチャー関連
//
//==============================================================================================
static void D_Nohara_24_Main( TCB_PTR tcb, void * work );

static const struct{
	u32  str_id;
	u32  param;
}DebugMenuList24[] = {
	{ msg_debug_nohara_24_01, (u32)D_Nohara_24_Main },
	{ msg_debug_nohara_24_02, (u32)D_Nohara_24_Main },
	{ msg_debug_nohara_24_03, (u32)D_Nohara_24_Main },
	{ msg_debug_nohara_24_04, (u32)D_Nohara_24_Main },
	{ msg_debug_nohara_24_05, (u32)D_Nohara_24_Main },
	{ msg_debug_nohara_24_06, (u32)D_Nohara_24_Main },
	{ msg_debug_nohara_24_07, (u32)D_Nohara_24_Main },
	{ msg_debug_nohara_24_08, (u32)D_Nohara_24_Main },
	{ msg_debug_nohara_24_09, (u32)D_Nohara_24_Main },
	{ msg_debug_nohara_24_10, (u32)D_Nohara_24_Main },
};

static const BMPLIST_HEADER ListH24 = {
	NULL,					//表示文字データポインタ
	NULL,					//カーソル移動ごとのコールバック関数
	NULL,					//一列表示ごとのコールバック関数

	NULL,					//GF_BGL_BMPWINのポインタ

	NELEMS(DebugMenuList24),//リスト項目数
	10,						//表示最大項目数

	0,						//ラベル表示Ｘ座標
	8,						//項目表示Ｘ座標
	0,						//カーソル表示Ｘ座標
	0,						//表示Ｙ座標
/*
	FBMP_COL_BLACK,			//文字色
	FBMP_COL_WHITE,			//背景色
	FBMP_COL_BLK_SDW,		//文字影色
*/
	FBMP_COL_WHITE,			//文字色
	FBMP_COL_BLACK,			//背景色
	FBMP_COL_BLK_SDW,		//文字影色
	0,						//文字間隔Ｘ
	16,						//文字間隔Ｙ
	BMPLIST_LRKEY_SKIP,		//ページスキップタイプ
	FONT_SYSTEM,			//文字指定(本来は u8 だけど、そんなに作らないと思うので)
	0						//ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)
};

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_24( TCB_PTR tcb, void * work )
{
	int i;
	BMPLIST_HEADER list_h;
	GF_BGL_BMPWIN* win;
	D_NOHARA_WORK* wk;
	wk = (D_NOHARA_WORK*)work;

	BmpDel( tcb );								//BMP開放

	wk->seq				= 0;
	wk->sel				= 0;
	wk->count			= 0;

	GF_BGL_BmpWinAddEx( wk->fsys->bgl, &wk->bmpwin[0], &DebugNoharaWinData );	//ビットマップ追加

	wk->menulist = BMP_MENULIST_Create( NELEMS(DebugMenuList24), HEAPID_BASE_DEBUG );

	for( i=0; i < NELEMS(DebugMenuList24); i++ ){
		BMP_MENULIST_AddArchiveString( wk->menulist, wk->msgman, 
										DebugMenuList24[i].str_id, DebugMenuList24[i].param );
	}

	list_h			= ListH24;
	list_h.list		= wk->menulist;
	list_h.win		= &wk->bmpwin[0];
	wk->lw			= BmpListSet( &list_h, 0, 0, HEAPID_BASE_DEBUG );

	GF_BGL_BmpWinOn( &wk->bmpwin[0] );

	//TCB_ChangeFunc( tcb, D_Nohara_24_Main );	//TCBの動作関数切り替え
	TCB_ChangeFunc( tcb, DebugNoharaMenuMain );	//TCBの動作関数切り替え
	return;
}
 
//--------------------------------------------------------------
/**
 * @brief	メイン
 *
 * @param	tcb		TCB_PTR型
 * @param	work	ワーク
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_24_Main( TCB_PTR tcb, void * work )
{
	EVENTWORK* ev;
	D_NOHARA_WORK* wk;
	wk = (D_NOHARA_WORK*)work;

	ev = SaveData_GetEventWork( wk->fsys->savedata );

	switch( wk->cursor_bak ){
	case 0:
		//バッテリーMAX
		SysWork_BtlSearcherBatterySet( ev, BS_BATTERY_MAX );
		break;

	case 1:
		//バッテリーCLR
		SysWork_BtlSearcherBatterySet( ev, 0 );
		break;

	case 2:
		//クリアーMAX
		SysWork_BtlSearcherClearSet( ev, BS_CLEAR_MAX );
		break;

	case 3:
		//クリアーCLR
		SysWork_BtlSearcherClearSet( ev, 0 );
		break;

	case 4:
		//起動
		EventSet_Script( wk->fsys, SCRID_BTL_SEARCHER, NULL );
		TcbBmpDel( tcb, work );		//TCBBMP開放
		return;						//注意！

	case 5:
		//レベル１セット
		SysFlag_BsLvSet( ev, 1 );

		MyItem_AddItem( SaveData_GetMyItem(wk->fsys->savedata),
						ITEM_BATORUSAATYAA, 1, HEAPID_BASE_DEBUG);
		break;

	case 6:
		//レベル２セット
		SysFlag_BsLvSet( ev, 2 );
		break;

	case 7:
		//レベル３セット
		SysFlag_BsLvSet( ev, 3 );
		break;

	case 8:
		//レベル４セット
		SysFlag_BsLvSet( ev, 4 );
		break;

	case 9:
		//レベル５セット
		SysFlag_BsLvSet( ev, 5 );
		break;

	default:
		return;
	};

	wk->seq = 0;
	TCB_ChangeFunc( tcb, DebugNoharaMenuMain );		//TCBの動作関数切り替え
	return;
}


//==============================================================================================
//
//	25	BGMオフ
//
//==============================================================================================
extern void Snd_DebugBgmFlagSet( u8 sw );

//--------------------------------------------------------------
/**
 * @brief	BGMオフ
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_25( TCB_PTR tcb, void * work )
{
#ifdef PM_DEBUG
	Snd_DebugBgmFlagSet( 1 );
	Snd_Stop();
#endif

	TcbBmpDel( tcb, work );		//TCBBMP開放
	return;
}


//==============================================================================================
//
//	26	BGMオン
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	BGMオン
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_26( TCB_PTR tcb, void * work )
{
#ifdef PM_DEBUG
	u16 bgm_no;
	D_NOHARA_WORK* wk = (D_NOHARA_WORK*)work;

	Snd_DebugBgmFlagSet( 0 );

	//サウンドデータセット(シーンが変更されない時は何もしない)
	Snd_SceneSet( SND_SCENE_DUMMY );
	bgm_no = Snd_FieldBgmNoGet( wk->fsys, wk->fsys->location->zone_id );
	Snd_ZoneBgmSet(Snd_FieldBgmNoGetNonBasicBank(wk->fsys,wk->fsys->location->zone_id));//zone set

	Snd_DataSetByScene( SND_SCENE_FIELD, bgm_no, 1 );
#endif

	TcbBmpDel( tcb, work );		//TCBBMP開放
	return;
}


//==============================================================================================
//
//	27	ポケモン鳴き声を2つ再生出来るフラグ"オン"
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	ポケモン鳴き声を2つ再生出来るフラグ"オン"
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_27( TCB_PTR tcb, void * work )
{
#ifdef SND_PV_070213
	Snd_PMVoiceDoubleFlagSet( 1 );
#endif
	TcbBmpDel( tcb, work );		//TCBBMP開放
	return;
}


//==============================================================================================
//
//	28	ポケモン鳴き声を2つ再生出来るフラグ"オフ"
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	ポケモン鳴き声を2つ再生出来るフラグ"オフ"
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_28( TCB_PTR tcb, void * work )
{
#ifdef SND_PV_070213
	Snd_PMVoiceDoubleFlagSet( 0 );
#endif
	TcbBmpDel( tcb, work );		//TCBBMP開放
	return;
}


//==============================================================================================
//
//	29	ウェイト指定ありの鳴き声再生のテスト
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	ウェイト指定ありの鳴き声再生のテスト
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_29( TCB_PTR tcb, void * work )
{
#ifdef SND_PV_070213

#if 0
	Snd_PMVoiceWaitPlayEx( PV_NORMAL, MONSNO_HITOKAGE, 127, 127, HEAPID_WORLD, 60, 0 );

	Snd_PMVoiceWaitPlayEx( PV_NORMAL, MONSNO_WANINOKO, -128, 127, HEAPID_WORLD, 100, 0 );
#endif

#if 0
	Snd_PMVoicePlay( MONSNO_KAIOOGA, 0 );
	Snd_PMVoiceWaitPlayEx( PV_NORMAL, MONSNO_WANINOKO, -128, 127, HEAPID_WORLD, 10, 0 );
#endif

#if 0
	Snd_PMVoiceWaitPlayEx( PV_NORMAL, MONSNO_WANINOKO, -128, 127, HEAPID_WORLD, 10, 0 );
	Snd_PMVoicePlay( MONSNO_KAIOOGA, 0 );
#endif

	Snd_PMVoiceWaitPlayEx( PV_NORMAL, MONSNO_WANINOKO, -128, 127, HEAPID_WORLD, 100, 0 );

#endif

	TcbBmpDel( tcb, work );		//TCBBMP開放
	return;
}


//==============================================================================================
//
//	30	バトルファクトリー
//
//==============================================================================================
#if 0
static void D_Nohara_30_Main( TCB_PTR tcb, void * work );

//--------------------------------------------------------------
/**
 * @brief	バトルファクトリーモニター呼び出し
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_30( TCB_PTR tcb, void * work )
{
	D_NOHARA_WORK* wk = (D_NOHARA_WORK*)work;

	EventSet_Script( wk->fsys, SCRID_SEISEKI_STAGE, NULL );

	TcbBmpDel( tcb, work );		//TCBBMP開放
	return;
}

//--------------------------------------------------------------
/**
 * @brief	メイン
 *
 * @param	tcb		TCB_PTR型
 * @param	work	ワーク
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_30_Main( TCB_PTR tcb, void * work )
{
	D_NOHARA_WORK* wk;
	wk = (D_NOHARA_WORK*)work;

	switch( wk->seq ){

	case 0:
		//FieldSystemProc_SeqHoldEnd();		

		//wk->factory_call = Factory_SetProc( wk->fsys, HEAPID_BASE_DEBUG, 
		//									BF_TYPE_SINGLE, BF_MODE_RENTARU );

		//GameSystem_FinishFieldProc( wk->fsys );

		wk->seq++;
		break;

	case 1:
		//プロセス終了待ち
		if( FieldEvent_Cmd_WaitSubProcEnd(wk->fsys) == FALSE ){
			sys_FreeMemoryEz( wk->factory_call );
			wk->seq++;
		}
		break;

	case 2:
		if(GameSystem_CheckSubProcExists(wk->fsys) == FALSE){
			//フィールドマッププロセス復帰
			FieldEvent_Cmd_SetMapProc( wk->fsys );
		}
		break;

	case 3:
		//フィールドプロセス開始終了待ち
		if( !FieldEvent_Cmd_WaitMapProcStart(wk->fsys) ){
			wk->seq = 0;
			TcbBmpDel( tcb, work );				//TCBBMP開放
		}
		break;
	};

	return;
}
#endif


//==============================================================================================
//
//	31	録画再生
//
//==============================================================================================
#if 0
static void D_Nohara_31_Main( TCB_PTR tcb, void * work );

//--------------------------------------------------------------
/**
 * @brief	録画再生
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_31( TCB_PTR tcb, void * work )
{
	D_NOHARA_WORK* wk = (D_NOHARA_WORK*)work;
	wk->seq		= 0;
	TCB_ChangeFunc( tcb, D_Nohara_31_Main );	//TCBの動作関数切り替え
	return;
}

BATTLE_PARAM* bp;
//--------------------------------------------------------------
/**
 * @brief	メイン
 *
 * @param	tcb		TCB_PTR型
 * @param	work	ワーク
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_31_Main( TCB_PTR tcb, void * work )
{
	LOAD_RESULT load_ret;
	D_NOHARA_WORK* wk;
	wk = (D_NOHARA_WORK*)work;

	switch( wk->seq ){

	case 0:
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
						WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_WORLD );
		wk->seq++;
		break;

	case 1:
		//イベントコマンド：フィールドマッププロセス終了
		//EventCmd_FinishFieldMap( wk->fsys->event );
		
		if( WIPE_SYS_EndCheck() == TRUE ){
			MSGMAN_Delete( wk->msgman );
			WORDSET_Delete( wk->wordset );
			BmpDel( tcb );						//BMP開放
			wk->seq++;
		}
		break;

	case 2:
		//BATTLE_PARAM設定
		bp = BattleParam_Create( HEAPID_WORLD, FIGHT_TYPE_1vs1 );
		BattleParam_SetParamByGameDataCore( bp, wk->fsys, wk->fsys->savedata, 
											wk->fsys->location->zone_id, 
											wk->fsys->fnote, wk->fsys->bag_cursor, 
											wk->fsys->battle_cursor );

		//bp	ロードしたデータから生成するBATTLE_PARAM構造体へのポインタ
		//num	ロードするデータナンバー（LOADDATA_MYREC、LOADDATA_DOWNLOAD1、LOADDATA_DOWNLOAD2…）

		//対戦録画データのロード
		BattleRec_Load( wk->fsys->savedata, HEAPID_WORLD, &load_ret, bp, LOADDATA_MYREC );
		OS_Printf( "battle_load ret = %d\n", load_ret );
#if 0
	LOAD_RESULT_NULL = 0,		///<データなし
	LOAD_RESULT_OK,				///<データ正常読み込み
	LOAD_RESULT_NG,				///<データ異常
	LOAD_RESULT_BREAK,			///<破壊、復旧不能 
#endif
		wk->seq++;
		break;

	case 3:
		GameSystem_FinishFieldProc( wk->fsys );
		Snd_DataSetByScene( SND_SCENE_BATTLE, SEQ_BA_TRAIN, 1 );	//バトル曲再生
		FieldBattle_SetProc( wk->fsys, bp );
		OS_Printf( "録画戦闘呼び出しテスト\n" );
		wk->seq++;
		break;

	case 4:
		if( FieldEvent_Cmd_WaitSubProcEnd(wk->fsys) ){		//サブプロセス終了待ち
			break;
		}
		wk->seq++;
		break;

	case 5:
		if( GameSystem_CheckSubProcExists( wk->fsys ) == FALSE ){
			BattleParam_Delete( bp );						//BATTLE_PARAMの開放
			BattleRec_Exit();
			//FieldEvent_Cmd_SetMapProc( wk->fsys );
			GameSystem_CreateFieldProc( wk->fsys );			//フィールド復帰
			wk->seq++;
		}
		break;

	case 6:
		if( GameSystem_CheckFieldProcExists(wk->fsys) ){
		//if( FieldEvent_Cmd_WaitMapProcStart(wk->fsys) ){
			wk->seq++;
		}
		break;

	case 7:
		FieldFadeWipeSet( FLD_DISP_BRIGHT_BLACKIN );
#if 0

		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
						WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC*3, HEAPID_WORLD );
		wk->seq++;
		break;

	case 8:
		if( WIPE_SYS_EndCheck() == TRUE ){
#endif
			wk->seq = 0;
			PMDS_taskDel( tcb );				//TCB開放
			FieldSystemProc_SeqHoldEnd();
//		}
		return;
	};

	return;
}
#endif


//==============================================================================================
//
//	32	ＣＰセット
//
//==============================================================================================
#if 0
//--------------------------------------------------------------
/**
 * @brief	ＣＰセット
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_32( TCB_PTR tcb, void * work )
{
	D_NOHARA_WORK* wk = (D_NOHARA_WORK*)work;

	//シングル現在のCPレコードを9999
	FrontierRecord_Set( SaveData_GetFrontier(wk->fsys->savedata), 
						CastleScr_GetCPRecordID(CASTLE_TYPE_SINGLE),
						FRONTIER_RECORD_NOT_FRIEND, 9999 );

	//マルチ現在のCPレコードを9999
	FrontierRecord_Set( SaveData_GetFrontier(wk->fsys->savedata), 
						CastleScr_GetCPRecordID(CASTLE_TYPE_MULTI),
						FRONTIER_RECORD_NOT_FRIEND, 9999 );

	TcbBmpDel( tcb, work );		//TCBBMP開放
	return;
}
#endif


//==============================================================================================
//
//	33	HP操作
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	HP操作
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_33( TCB_PTR tcb, void * work )
{
	u32 hp;
	POKEMON_PARAM* poke;
	D_NOHARA_WORK* wk = (D_NOHARA_WORK*)work;

	//ポケモンへのポインタ取得
	poke = PokeParty_GetMemberPointer( SaveData_GetTemotiPokemon(wk->fsys->savedata), 0 );

	hp = 5;
	PokeParaPut( poke, ID_PARA_hp, &hp );
	//PokeParaCalc( poke );

	TcbBmpDel( tcb, work );		//TCBBMP開放
	return;
}


//==============================================================================================
//
//	42	hp_rnd,spedef_rnd操作
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	hp_rnd,spedef_rnd操作
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_42( TCB_PTR tcb, void * work )
{
	u32 hp;
	POKEMON_PARAM* poke;
	D_NOHARA_WORK* wk = (D_NOHARA_WORK*)work;

	//ポケモンへのポインタ取得
	poke = PokeParty_GetMemberPointer( SaveData_GetTemotiPokemon(wk->fsys->savedata), 0 );

	//努力値テスト
#if 0
	hp = 200;
	PokeParaPut( poke, ID_PARA_hp_rnd, &hp );
	hp = 200;
	PokeParaPut( poke, ID_PARA_spedef_rnd, &hp );
#else
	hp = 200;
	PokeParaPut( poke, ID_PARA_hp_exp, &hp );
	hp = 200;
	PokeParaPut( poke, ID_PARA_spedef_exp, &hp );
#endif

	TcbBmpDel( tcb, work );		//TCBBMP開放
	return;
}


//==============================================================================================
//
//	35	別荘イベント
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	別荘イベント
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_35( TCB_PTR tcb, void * work )
{
	u16 waza;
	POKEMON_PARAM* poke;
	D_NOHARA_WORK* wk = (D_NOHARA_WORK*)work;

	//レコード表示
	//OS_Printf( "dendou = %d\n", RECORD_Get(SaveData_GetRecord(wk->fsys->savedata),
	//								RECID_DENDOU_CNT) );

#if 0
	//先頭のポケモンのwaza3、waza4を忘れさせる

	//ポケモンへのポインタ取得
	poke = PokeParty_GetMemberPointer( SaveData_GetTemotiPokemon(wk->fsys->savedata), 0 );

	waza = 0;
	PokeParaPut( poke, ID_PARA_waza3, &waza );
	PokeParaPut( poke, ID_PARA_waza4, &waza );
	//PokeParaCalc( poke );
#endif

#if 0
	debug_t07r0201_no++;
	if( debug_t07r0201_no >= T07R0201_EVENT_EVENT_MAX ){
		debug_t07r0201_no = 0;
	}

	OS_Printf( "別荘のイベントナンバーを%dに変更しました\n", debug_t07r0201_no );
	SysFlag_T07ObjInReset( SaveData_GetEventWork(wk->fsys->savedata) );//別荘外のOBJが中に入ったoff
	SysWork_T07R0201Set( SaveData_GetEventWork(wk->fsys->savedata), debug_t07r0201_no );
#else
	SysWorkUpdateEventT07R0201( wk->fsys->savedata );
#endif

	TcbBmpDel( tcb, work );		//TCBBMP開放
	return;
}


//==============================================================================================
//
//	40	フロンティア到着
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	フロンティア到着
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_40( TCB_PTR tcb, void * work )
{
	EVENTWORK* ev;
	D_NOHARA_WORK* wk = (D_NOHARA_WORK*)work;

	ev = SaveData_GetEventWork( wk->fsys->savedata );

	if( sys.cont & PAD_BUTTON_L ){
		SysFlag_Arrive( ev, SYSFLAG_MODE_RESET, FLAG_ARRIVE_D32R0101 );
	}else{
		SysFlag_Arrive( ev, SYSFLAG_MODE_SET, FLAG_ARRIVE_D32R0101 );
	}

	TcbBmpDel( tcb, work );		//TCBBMP開放
	return;
}


//==============================================================================================
//
//	41	ステージ連勝数操作関連
//
//==============================================================================================
static void D_Nohara_41_Main( TCB_PTR tcb, void * work );
static void StageRenshouSet( D_NOHARA_WORK* wk, u16 start_no, u16 end_no );

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_41( TCB_PTR tcb, void * work )
{
	D_NOHARA_WORK* wk;
	wk = (D_NOHARA_WORK*)work;
	wk->seq		= 0;
	wk->count	= 0;
	wk->work	= 0;
	TCB_ChangeFunc( tcb, D_Nohara_41_Main );	//TCBの動作関数切り替え
	return;
}

//--------------------------------------------------------------
/**
 * @brief	メイン
 *
 * @param	tcb		TCB_PTR型
 * @param	work	ワーク
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_41_Main( TCB_PTR tcb, void * work )
{
	LOAD_RESULT load_result;
	SAVE_RESULT save_result;
	FRONTIER_EX_SAVEDATA* fes;
	int flag;
	D_NOHARA_WORK* wk;
	wk = (D_NOHARA_WORK*)work;

	flag = 0;	//書き換えフラグ

	if( (sys.repeat == PAD_KEY_UP) || (sys.cont == PAD_KEY_RIGHT) ){
		flag = 1;
		wk->work++;
		if( wk->work > MONSNO_END ){
			wk->work = 1;
		}
	}

	if( (sys.repeat == PAD_KEY_DOWN) || (sys.cont == PAD_KEY_LEFT) ){
		flag = 1;
		wk->work--;
		if( wk->work < 1 ){
			wk->work = MONSNO_END;
		}
	}

	//書き換えフラグが立っていたら
	if( flag == 1 ){
		//指定範囲を塗りつぶし
		GF_BGL_BmpWinFill( &wk->bmpwin[0], FBMP_COL_BLACK, 8*7, 8*2, 
								8*8, 8*2 );
	
		//モンスターナンバー表示
		NumMsgSet( wk, &wk->bmpwin[0], wk->work, 8*7, 8*2 );
		GF_BGL_BmpWinOn( &wk->bmpwin[0] );
	}

	//終了(1匹対象)
	if( sys.trg == PAD_BUTTON_A ){
		StageRenshouSet( wk, wk->work, (wk->work+1) );
		TcbBmpDel( tcb, work );		//TCBBMP開放
		return;
	}

	//終了(10匹対象)
	if( sys.trg == PAD_BUTTON_X ){
		//StageRenshouSet( wk, wk->work, (wk->work+10) );			//MONSNO_ENDチェックが必要
		StageRenshouSet( wk, 1, (10+1) );
		TcbBmpDel( tcb, work );		//TCBBMP開放
		return;
	}

	//終了(100匹対象)
	if( sys.trg == PAD_BUTTON_Y ){
		//StageRenshouSet( wk, wk->work, (wk->work+100) );			//MONSNO_ENDチェックが必要
		StageRenshouSet( wk, 1, (100+1) );
		TcbBmpDel( tcb, work );		//TCBBMP開放
		return;
	}

	//終了(全て対象)
	if( sys.trg == PAD_BUTTON_START ){
		//アルセウスは挑戦出来ないし、終端いきすぎは怖いので、ほぼ全てのポケモンにしている
		//StageRenshouSet( wk, 1, (MONSNO_END+1) );
		StageRenshouSet( wk, 1, MONSNO_END );
		TcbBmpDel( tcb, work );		//TCBBMP開放
		return;
	}

	//終了
	if( sys.cont == PAD_BUTTON_B ){
		TcbBmpDel( tcb, work );		//TCBBMP開放
	}

	return;
}




static void StageRenshouSet( D_NOHARA_WORK* wk, u16 start_no, u16 end_no )
{
	LOAD_RESULT load_result;
	SAVE_RESULT save_result;
	FRONTIER_EX_SAVEDATA* fes;
	int flag,i;

	fes = FrontierEx_Load( wk->fsys->savedata, HEAPID_WORLD, &load_result );
	if( load_result != LOAD_RESULT_OK ){
		//
	}else{

		for( i=start_no; i < end_no ;i++ ){

			//Debug_FrontierEx_StageRenshou_Set( fes, FREXID_STAGE_RENSHOU_SINGLE, wk->work, 9999 );
			Debug_FrontierEx_StageRenshou_Set( fes, FREXID_STAGE_RENSHOU_SINGLE, i, 500 );

			//OS_Printf( "num = %d\n", FrontierEx_StageRenshou_Get(	wk->fsys->savedata,fes, 
			//														FREXID_STAGE_RENSHOU_SINGLE, 
			//														wk->work) );
		}	

#if 1
		//フィールド動作モデルのセーブ
		Field_SaveFieldObj( wk->fsys );

		// ポケッチ関連データのセーブ
		Field_SendPoketchInfo( wk->fsys, POKETCH_SEND_SAVE, 0 );

		//位置を引っ張る
		wk->fsys->location->grid_x = Player_NowGPosXGet( wk->fsys->player );
		wk->fsys->location->grid_z = Player_NowGPosZGet( wk->fsys->player );
		wk->fsys->location->door_id = DOOR_ID_JUMP_CODE;
		wk->fsys->location->dir = Player_DirGet( wk->fsys->player );
#endif

		//外部セーブを呼ばれないと反映しないので、このデバック微妙かも。。。
		save_result = FrontierEx_Save(wk->fsys->savedata, fes);
	}

	if( fes != NULL ){
		sys_FreeMemoryEz(fes);
	}

	return;
}


//--------------------------------------------------------------
/**
 * @brief	バイナリメッセージファイル読み込み、表示テスト
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_89(void)
{
#if 1
	return;
#else
	static u16 * pMsg = NULL;

	//debug_msg.datはcvsに登録していないので注意！
	pMsg = (u16 *)sys_LoadFile( HEAPID_BASE_DEBUG, "/data/script/debug_msg.dat" );
	FieldTalkWinPut();
	msg_no_print( pMsg );
	FieldTalkMsgStart( pMsg, 1 );					//第2引数=skip
	sys_FreeMemoryEz( pMsg );

	return;
#endif
}

//--------------------------------------------------------------
/**
 * @brief	トラックミュートテスト
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_72(void)
{
	Snd_PlayerSetTrackMute( SND_HANDLE_FIELD, ( (1 << 0) | (2 << 0) ), TRUE );
	return;
}


//==============================================================================================
//
//	34	フロンティア関連(連勝数操作)
//
//==============================================================================================
static void D_Nohara_34_Main( TCB_PTR tcb, void * work );
static void D_Nohara_34_2_Main( TCB_PTR tcb, void * work );

enum{
	D34_PARAM_TOWER = 100,
	D34_PARAM_FACTORY,
	D34_PARAM_FACTORY_OPEN,
	D34_PARAM_STAGE,
	D34_PARAM_CASTLE,
	D34_PARAM_ROULETTE,
};

static const struct{
	u32  str_id;
	u32  param;
}DebugMenuList34[] = {
	//タワーのセットは色々違うので保留。。。
	{ msg_debug_nohara_34_15, D34_PARAM_TOWER },		//タワー
	{ msg_debug_nohara_34_02, D34_PARAM_FACTORY },		//ファクトリー
	{ msg_debug_nohara_36,	  D34_PARAM_FACTORY_OPEN },	//ファクトリーオープン
	{ msg_debug_nohara_34_03, D34_PARAM_STAGE },		//ステージ
	{ msg_debug_nohara_34_04, D34_PARAM_CASTLE },		//キャッスル
	{ msg_debug_nohara_34_05, D34_PARAM_ROULETTE },		//ルーレット
};

//マイナスにした時の最大連勝数セット値
static const u16 frontier_num_max[] = {
	9996,			//タワー
	9996,			//ファクトリー
	9996,			//ファクトリーオープン
	9990,			//ステージ
	9996,			//キャッスル
	9996,			//ルーレット
};

enum{
	D34_PARAM_SINGLE = 0,
	D34_PARAM_DOUBLE,
	D34_PARAM_MULTI,
	D34_PARAM_WIFI_MULTI,
};

static const struct{
	u32  str_id;
	u32  param;
}DebugMenuList34_type[] = {
	{ msg_debug_nohara_34_11, D34_PARAM_SINGLE },		//シングル
	{ msg_debug_nohara_34_12, D34_PARAM_DOUBLE },		//ダブル
	{ msg_debug_nohara_34_13, D34_PARAM_MULTI },		//マルチ
	{ msg_debug_nohara_34_14, D34_PARAM_WIFI_MULTI },	//WIFIマルチ
};

static const BMPLIST_HEADER ListH34 = {
	NULL,					//表示文字データポインタ
	NULL,					//カーソル移動ごとのコールバック関数
	NULL,					//一列表示ごとのコールバック関数

	NULL,					//GF_BGL_BMPWINのポインタ

	//NELEMS(DebugMenuList34_type),//リスト項目数
	1,						//リスト項目数
	10,						//表示最大項目数

	0,						//ラベル表示Ｘ座標
	8,						//項目表示Ｘ座標
	0,						//カーソル表示Ｘ座標
	0,						//表示Ｙ座標

	FBMP_COL_WHITE,			//文字色
	FBMP_COL_BLACK,			//背景色
	FBMP_COL_BLK_SDW,		//文字影色
	0,						//文字間隔Ｘ
	16,						//文字間隔Ｙ
	BMPLIST_LRKEY_SKIP,		//ページスキップタイプ
	FONT_SYSTEM,			//文字指定(本来は u8 だけど、そんなに作らないと思うので)
	0						//ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)
};

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_34( TCB_PTR tcb, void * work )
{
	int i;
	BMPLIST_HEADER list_h;
	GF_BGL_BMPWIN* win;
	D_NOHARA_WORK* wk;
	wk = (D_NOHARA_WORK*)work;

	switch( wk->fro_seq ){

	//施設選択
	case 0:
		BmpDel( tcb );								//BMP開放
		GF_BGL_BmpWinAddEx( wk->fsys->bgl, &wk->bmpwin[0], &DebugNoharaWinData );	//BMP追加
		wk->menulist = BMP_MENULIST_Create( NELEMS(DebugMenuList34), HEAPID_BASE_DEBUG );
		for( i=0; i < NELEMS(DebugMenuList34); i++ ){
			BMP_MENULIST_AddArchiveString( wk->menulist, wk->msgman, 
											DebugMenuList34[i].str_id, DebugMenuList34[i].param );
		}
		list_h			= ListH34;
		list_h.list		= wk->menulist;
		list_h.win		= &wk->bmpwin[0];
		list_h.count	= NELEMS(DebugMenuList34);
		wk->lw			= BmpListSet( &list_h, 0, 0, HEAPID_BASE_DEBUG );
		GF_BGL_BmpWinOn( &wk->bmpwin[0] );
		break;

	//バトルタイプ選択
	case 1:
		GF_BGL_BmpWinAddEx( wk->fsys->bgl, &wk->bmpwin[0], &DebugNoharaWinData );	//BMP追加
		wk->menulist = BMP_MENULIST_Create( NELEMS(DebugMenuList34_type), HEAPID_BASE_DEBUG );
		for( i=0; i < NELEMS(DebugMenuList34_type); i++ ){
			BMP_MENULIST_AddArchiveString( wk->menulist, wk->msgman, 
									DebugMenuList34_type[i].str_id, DebugMenuList34_type[i].param );
		}
		list_h			= ListH34;
		list_h.list		= wk->menulist;
		list_h.win		= &wk->bmpwin[0];
		list_h.count	= NELEMS(DebugMenuList34_type);
		wk->lw			= BmpListSet( &list_h, 0, 0, HEAPID_BASE_DEBUG );
		GF_BGL_BmpWinOn( &wk->bmpwin[0] );
		break;

	};

	wk->fro_seq++;
	TCB_ChangeFunc( tcb, D_Nohara_34_Main );	//TCBの動作関数切り替え
	return;
}
 
//--------------------------------------------------------------
/**
 * @brief	メイン
 *
 * @param	tcb		TCB_PTR型
 * @param	work	ワーク
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_34_Main( TCB_PTR tcb, void * work )
{
	u32	ret;
	D_NOHARA_WORK* wk;
	wk = (D_NOHARA_WORK*)work;

	ret = BmpListMain( wk->lw );

	switch( ret ){

	case BMPLIST_NULL:
		break;

	case BMPLIST_CANCEL:
		TcbBmpDel( tcb, work );				//TCBBMP開放
		break;

	////////////////////////////////////////////////////////////////////
	case D34_PARAM_TOWER:
		wk->fro_type = D34_PARAM_TOWER;
		wk->fro_add  = 7;
		BmpDel( tcb );						//BMP開放
		TCB_ChangeFunc( tcb, D_Nohara_34 );	//TCBの動作関数切り替え
		break;

	case D34_PARAM_FACTORY:
		wk->fro_type = D34_PARAM_FACTORY;
		wk->fro_add  = 7;
		BmpDel( tcb );						//BMP開放
		TCB_ChangeFunc( tcb, D_Nohara_34 );	//TCBの動作関数切り替え
		break;

	case D34_PARAM_FACTORY_OPEN:
		wk->fro_type = D34_PARAM_FACTORY_OPEN;
		wk->fro_add  = 7;
		BmpDel( tcb );						//BMP開放
		TCB_ChangeFunc( tcb, D_Nohara_34 );	//TCBの動作関数切り替え
		break;

	case D34_PARAM_STAGE:
		wk->fro_type = D34_PARAM_STAGE;
		wk->fro_add  = 10;
		BmpDel( tcb );						//BMP開放
		TCB_ChangeFunc( tcb, D_Nohara_34 );	//TCBの動作関数切り替え
		break;

	case D34_PARAM_CASTLE:
		wk->fro_type = D34_PARAM_CASTLE;
		wk->fro_add  = 7;
		BmpDel( tcb );						//BMP開放
		TCB_ChangeFunc( tcb, D_Nohara_34 );	//TCBの動作関数切り替え
		break;

	case D34_PARAM_ROULETTE:
		wk->fro_type = D34_PARAM_ROULETTE;
		wk->fro_add  = 7;
		BmpDel( tcb );						//BMP開放
		TCB_ChangeFunc( tcb, D_Nohara_34 );	//TCBの動作関数切り替え
		break;

	////////////////////////////////////////////////////////////////////
	case D34_PARAM_SINGLE:
		wk->btl_type = D34_PARAM_SINGLE;
		TCB_ChangeFunc( tcb, D_Nohara_34_2_Main );	//TCBの動作関数切り替え
		break;

	case D34_PARAM_DOUBLE:
		wk->btl_type = D34_PARAM_DOUBLE;
		TCB_ChangeFunc( tcb, D_Nohara_34_2_Main );	//TCBの動作関数切り替え
		break;

	case D34_PARAM_MULTI:
		wk->btl_type = D34_PARAM_MULTI;
		TCB_ChangeFunc( tcb, D_Nohara_34_2_Main );	//TCBの動作関数切り替え
		break;

	case D34_PARAM_WIFI_MULTI:
		wk->btl_type = D34_PARAM_WIFI_MULTI;
		TCB_ChangeFunc( tcb, D_Nohara_34_2_Main );	//TCBの動作関数切り替え
		break;

	};

	return;
}

//--------------------------------------------------------------
/**
 * @brief	メイン
 *
 * @param	tcb		TCB_PTR型
 * @param	work	ワーク
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void D_Nohara_34_2_Main( TCB_PTR tcb, void * work )
{
	int i;
	u8 buf8[4];
	u16 num,type,l_num,h_num;
	u32	ret,flag;
	D_NOHARA_WORK* wk;
	POKEMON_PARAM* poke;
	FACTORYSCORE* fa_score_sv;
	STAGESCORE* s_score_sv;
	CASTLESCORE* ca_score_sv;
	ROULETTESCORE* r_score_sv;
	FRONTIER_SAVEWORK * fr_sv;

	wk = (D_NOHARA_WORK*)work;
	fr_sv = SaveData_GetFrontier( wk->fsys->savedata );

	flag = 0;		//書き換えフラグ

	if( sys.trg == PAD_BUTTON_A ){

		switch( wk->fro_type ){
		////////////////////////////////////////////////////////////////////
		case D34_PARAM_TOWER:

			////////////////////
			//★WIFIマルチ限定！
			////////////////////

			//"7連勝(クリア)したかフラグ"書き出し
			FrontierRecord_Set(	fr_sv, 
				FRID_TOWER_MULTI_WIFI_CLEAR_BIT,
				wk->friend_work, 1 );

			//"連勝数"書き出し(「次は27人目です」というように使う)
			FrontierRecord_Set(	fr_sv, 
					FRID_TOWER_MULTI_WIFI_RENSHOU_CNT,
					wk->friend_work, wk->work );
			break;

		case D34_PARAM_FACTORY:
			fa_score_sv = SaveData_GetFactoryScore( wk->fsys->savedata );

			if( wk->btl_type == FACTORY_TYPE_WIFI_MULTI ){

				//"7連勝(クリア)したかフラグ"書き出し
				FrontierRecord_Set(	fr_sv, 
					FRID_FACTORY_MULTI_WIFI_CLEAR_BIT,
					wk->friend_work, 1 );

				//"連勝数"書き出し(「次は27人目です」というように使う)
				FrontierRecord_Set(	fr_sv, 
						FactoryScr_GetWinRecordID(0,wk->btl_type),
						wk->friend_work, wk->work );
			}else{

				//"7連勝(クリア)したかフラグ"書き出し
				buf8[0] = 1;
				FACTORYSCORE_PutScoreData(	fa_score_sv, FACTORYSCORE_ID_CLEAR_FLAG, 
											wk->btl_type, buf8 );

				//"連勝数"書き出し(「次は27人目です」というように使う)
				FrontierRecord_Set(	fr_sv, 
					FactoryScr_GetWinRecordID(0,wk->btl_type),	//LV50
					Frontier_GetFriendIndex(FactoryScr_GetWinRecordID(0,wk->btl_type)), wk->work );
			}
			break;

		case D34_PARAM_FACTORY_OPEN:
			fa_score_sv = SaveData_GetFactoryScore( wk->fsys->savedata );

			if( wk->btl_type == FACTORY_TYPE_WIFI_MULTI ){

				//"7連勝(クリア)したかフラグ"書き出し
				FrontierRecord_Set(	fr_sv, 
					FRID_FACTORY_MULTI_WIFI_CLEAR_BIT,
					wk->friend_work, 1 );

				//"連勝数"書き出し(「次は27人目です」というように使う)
				FrontierRecord_Set(	fr_sv, 
						FactoryScr_GetWinRecordID(1,wk->btl_type),
						wk->friend_work, wk->work );
			}else{

				//"7連勝(クリア)したかフラグ"書き出し
				buf8[0] = 1;
				FACTORYSCORE_PutScoreData(	fa_score_sv, FACTORYSCORE_ID_CLEAR_FLAG, 
											//wk->btl_type, buf8 );
											(1*FACTORY_TYPE_MAX)+wk->btl_type, buf8 );

				//"連勝数"書き出し(「次は27人目です」というように使う)
				FrontierRecord_Set(	fr_sv, 
					FactoryScr_GetWinRecordID(1,wk->btl_type),	//オープン
					Frontier_GetFriendIndex(FactoryScr_GetWinRecordID(1,wk->btl_type)), wk->work );
			}
			break;

		case D34_PARAM_STAGE:
			s_score_sv = SaveData_GetStageScore( wk->fsys->savedata );

			//先頭のポケモンにする
			poke = PokeParty_GetMemberPointer( SaveData_GetTemotiPokemon(wk->fsys->savedata), 0 );

			if( wk->btl_type == STAGE_TYPE_WIFI_MULTI ){

				//"7連勝(クリア)したかフラグ"書き出し
				FrontierRecord_Set(	fr_sv, 
					FRID_STAGE_MULTI_WIFI_CLEAR_BIT,
					wk->friend_work, 1 );

				//"連勝数"書き出し(「次は27人目です」というように使う)
				FrontierRecord_Set(	fr_sv, 
					StageScr_GetWinRecordID(wk->btl_type),
					wk->friend_work, wk->work );

				//"連勝中のポケモンナンバー"書き出し
				FrontierRecord_Set(	fr_sv, 
						StageScr_GetMonsNoRecordID(wk->btl_type),
						wk->friend_work, 
						PokeParaGet(poke,ID_PARA_monsno,NULL) );
			}else{
				//"7連勝(クリア)したかフラグ"書き出し
				buf8[0] = 1;
				STAGESCORE_PutScoreData( s_score_sv, STAGESCORE_ID_CLEAR_FLAG, wk->btl_type,0,buf8);

				//"連勝数"書き出し(「次は27人目です」というように使う)
				FrontierRecord_Set(	fr_sv, 
					StageScr_GetWinRecordID(wk->btl_type),
					Frontier_GetFriendIndex(StageScr_GetWinRecordID(wk->btl_type)), wk->work );

				//"連勝中のポケモンナンバー"書き出し
				FrontierRecord_Set(	fr_sv, 
						StageScr_GetMonsNoRecordID(wk->btl_type),
						Frontier_GetFriendIndex(StageScr_GetMonsNoRecordID(wk->btl_type)), 
						PokeParaGet(poke,ID_PARA_monsno,NULL) );

#if 1
				//「???」は抜かす
				//"タイプごとのレベル"を書き換え
				for( i=0; i < (STAGE_TR_TYPE_MAX-1) ;i++ ){
					//StageScr_TypeLevelRecordSet(wk->fsys->savedata, wk->btl_type, 
					Debug_StageScr_TypeLevelRecordSet(	wk->fsys->savedata, wk->btl_type, i );
				}
#endif
			}

#if 0 
			//"連勝中のポケモンナンバー"書き出し
			FrontierRecord_Set(	fr_sv, 
						StageScr_GetMonsNoRecordID(wk->btl_type),
						Frontier_GetFriendIndex(StageScr_GetMonsNoRecordID(wk->btl_type)), 
						0 );

			//ここで0をセットしても、d32r0401.evの流れで、
			//クリア中の時は、同じポケモンで挑戦しているかチェックが走ってしまい、
			//情報がクリアされてしまう。
#endif

			break;

		case D34_PARAM_CASTLE:
			ca_score_sv = SaveData_GetCastleScore( wk->fsys->savedata );
		
			if( wk->btl_type == CASTLE_TYPE_WIFI_MULTI ){

				//"7連勝(クリア)したかフラグ"書き出し
				FrontierRecord_Set(	fr_sv, 
					FRID_CASTLE_MULTI_WIFI_CLEAR_BIT,
					wk->friend_work, 1 );

				//"連勝数"書き出し(「次は27人目です」というように使う)
				FrontierRecord_Set(	fr_sv, 
						CastleScr_GetWinRecordID(wk->btl_type),
						wk->friend_work, wk->work );
			}else{

				//"7連勝(クリア)したかフラグ"書き出し
				buf8[0] = 1;
				CASTLESCORE_PutScoreData(	ca_score_sv, CASTLESCORE_ID_CLEAR_FLAG, 
											wk->btl_type, 0, buf8 );

				//"連勝数"書き出し(「次は27人目です」というように使う)
				FrontierRecord_Set(	fr_sv, 
						CastleScr_GetWinRecordID(wk->btl_type),
						Frontier_GetFriendIndex(CastleScr_GetWinRecordID(wk->btl_type)), wk->work );
			}
			break;

		case D34_PARAM_ROULETTE:
			r_score_sv = SaveData_GetRouletteScore( wk->fsys->savedata );

			if( wk->btl_type == ROULETTE_TYPE_WIFI_MULTI ){

				//"7連勝(クリア)したかフラグ"書き出し
				FrontierRecord_Set(	fr_sv, 
					FRID_ROULETTE_MULTI_WIFI_CLEAR_BIT,
					wk->friend_work, 1 );

				//"連勝数"書き出し(「次は27人目です」というように使う)
				FrontierRecord_Set(	fr_sv, 
					RouletteScr_GetWinRecordID(wk->btl_type),
					wk->friend_work, wk->work );
			}else{

				//"7連勝(クリア)したかフラグ"書き出し
				buf8[0] = 1;
				ROULETTESCORE_PutScoreData( r_score_sv, ROULETTESCORE_ID_CLEAR_FLAG, 
											wk->btl_type, 0, buf8 );

				//"連勝数"書き出し(「次は27人目です」というように使う)
				FrontierRecord_Set(	fr_sv, 
					RouletteScr_GetWinRecordID(wk->btl_type),
					Frontier_GetFriendIndex(RouletteScr_GetWinRecordID(wk->btl_type)), wk->work );
			}
			break;
		}

		return;
	};

	//Rボタンを押していると友達を選択
	if( sys.cont & PAD_BUTTON_R ){

		if( (sys.repeat & PAD_KEY_UP) || (sys.cont & PAD_KEY_RIGHT) ){
			flag = 1;

			wk->friend_work+=1;

			if( wk->friend_work >= 32 ){
				wk->friend_work = 0;
			}
		}

		if( (sys.repeat & PAD_KEY_DOWN) || (sys.cont & PAD_KEY_LEFT) ){
			flag = 1;
	
			wk->friend_work-=1;

			//マイナスにした時の最大連勝数セット値
			if( wk->friend_work < 0 ){
				wk->friend_work = 31;
			}
		}

	}else{

		if( (sys.repeat & PAD_KEY_UP) || (sys.cont & PAD_KEY_RIGHT) ){
			flag = 1;

			if( sys.cont & PAD_BUTTON_L ){
				wk->work+=1;
			}else{
				wk->work+=wk->fro_add;
			}

			if( wk->work >= 9999 ){
				wk->work = 0;
			}
		}

		if( (sys.repeat & PAD_KEY_DOWN) || (sys.cont & PAD_KEY_LEFT) ){
			flag = 1;
	
			if( sys.cont & PAD_BUTTON_L ){
				wk->work-=1;
			}else{
				wk->work-=wk->fro_add;
			}

			//マイナスにした時の最大連勝数セット値
			if( wk->work < 0 ){
				//wk->work = (9999 - 1);
				wk->work = frontier_num_max[ (wk->fro_type - D34_PARAM_TOWER) ];
			}
		}
	}

	//書き換えフラグが立っていたら
	if( flag == 1 ){
		//指定範囲を塗りつぶし
		//GF_BGL_BmpWinFill( &wk->bmpwin[0], FBMP_COL_BLACK, 8*11, 0, 8*6, 8*2 );
		GF_BGL_BmpWinFill( &wk->bmpwin[0], FBMP_COL_BLACK, 8*11, 0, 8*6, 8*4 );

		//数値表示
		NumMsgSet( wk, &wk->bmpwin[0], wk->work, 8*11, 2 );

		//数値表示(友達)
		NumMsgSet( wk, &wk->bmpwin[0], wk->friend_work, 8*11, 8*2+2 );

		GF_BGL_BmpWinOn( &wk->bmpwin[0] );
	}

	//終了
	if( sys.cont == PAD_BUTTON_B ){
		TcbBmpDel( tcb, work );				//TCBBMP開放
	}

	return;
}








//--------------------------------------------------------------------------------------------
/**
 * タイプレベルのレコードをセット(fssc_stage_sub.cにある関数で呼べないのでコピペでもってきた)
 *
 * @param	wk
 *
 * @return	"レコード"
 *
 */
//--------------------------------------------------------------------------------------------
static void Debug_StageScr_TypeLevelRecordSet( SAVEDATA* sv, u8 type, u8 csr_pos )
{
	u8 offset;
	u8 param;
	u8 set_num,total;
	u16 l_num,h_num;

	//total = 0xaa;		//10101010(10挑戦済み)
	//total = 0x99;		//10011001(10挑戦前)
	//total = 0x88;		//10001000(9)
	total = 0x00;		//00000000(0)

#if 0
	switch( csr_pos ){
	case 0:
		total = 0x01;
		break;

	case 1:
		total = 0x01;
		break;

	case 2:
		total = 0x23;
		break;

	case 3:
		total = 0x23;
		break;

	case 4:
		total = 0x45;
		break;

	case 5:
		total = 0x45;
		break;

	case 6:
		total = 0x67;
		break;

	case 7:
		total = 0x67;
		break;

	case 8:
		total = 0x89;
		break;

	case 9:
		total = 0x89;
		break;

	};
#endif

	FrontierRecord_Set(	SaveData_GetFrontier(sv), 
					StageScr_GetTypeLevelRecordID(type,csr_pos),
					Frontier_GetFriendIndex(StageScr_GetTypeLevelRecordID(type,csr_pos)), total );
	return;
}










//==============================================================================================
//
//
//
//==============================================================================================

static const struct{
	u32  str_id;
	u32  param;
}DebugMenuList2[] = {
	{ msg_debug_nohara_25, (u32)D_Nohara_25 },
	{ msg_debug_nohara_26, (u32)D_Nohara_26 },
};

static const BMPLIST_HEADER DebugListH2 = {
	NULL,					//表示文字データポインタ

	NULL,					//カーソル移動ごとのコールバック関数
	NULL,					//一列表示ごとのコールバック関数

	NULL,					//GF_BGL_BMPWINのポインタ

	NELEMS(DebugMenuList2),	//リスト項目数
	9,						//表示最大項目数

	0,						//ラベル表示Ｘ座標
	8,						//項目表示Ｘ座標
	0,						//カーソル表示Ｘ座標
	0,						//表示Ｙ座標
/*
	FBMP_COL_BLACK,			//文字色
	FBMP_COL_WHITE,			//背景色
	FBMP_COL_BLK_SDW,		//文字影色
*/
	FBMP_COL_WHITE,			//文字色
	FBMP_COL_BLACK,			//背景色
	FBMP_COL_BLK_SDW,		//文字影色
	0,						//文字間隔Ｘ
	16,						//文字間隔Ｙ
	BMPLIST_LRKEY_SKIP,		//ページスキップタイプ
	FONT_SYSTEM,			//文字指定(本来は u8 だけど、そんなに作らないと思うので)
	0						//ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)
};

//--------------------------------------------------------------
/**
 * @brief	BGMデバックメニュー呼び出し(fld_debug.c)
 *
 * @param	fsys	FIELDSYS_WORK型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
void DebugBgmMenuInit( FIELDSYS_WORK* fsys )
{
	int i;
	BMPLIST_HEADER list_h;
	D_NOHARA_WORK* wk;

	wk = (D_NOHARA_WORK*)TCB_GetWork( PMDS_taskAdd(DebugNoharaMenuMain, sizeof(D_NOHARA_WORK), 0, HEAPID_BASE_DEBUG) );

	wk->seq				= 0;
	wk->sel				= 0;
	wk->work			= 0;
	wk->wave_buf_flag	= 0;								//波形格納バッファを確保したフラグOFF
	wk->waveout_flag	= 0;								//鳴き声再生中フラグOFF
	wk->fsys			= fsys;

	GF_BGL_BmpWinAddEx( wk->fsys->bgl, &wk->bmpwin[0], &DebugNoharaWinData );	//ビットマップ追加

	wk->menulist = BMP_MENULIST_Create( NELEMS(DebugMenuList2), HEAPID_BASE_DEBUG );

	wk->wordset = WORDSET_Create( HEAPID_BASE_DEBUG );

	wk->msgman = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, 
								NARC_msg_debug_nohara_dat, HEAPID_BASE_DEBUG);

	for( i=0; i < NELEMS(DebugMenuList2); i++ ){
		BMP_MENULIST_AddArchiveString( wk->menulist, wk->msgman, 
										DebugMenuList2[i].str_id, DebugMenuList2[i].param );
	}

	list_h			= DebugListH2;
	list_h.list		= wk->menulist;
	list_h.win		= &wk->bmpwin[0];
	wk->lw			= BmpListSet( &list_h, 0, 0, HEAPID_BASE_DEBUG );

	GF_BGL_BmpWinOn( &wk->bmpwin[0] );
	return;
}


//==============================================================================================
//
//	デバック「のはら」ではない所からの「デバックスクリプト」呼び出し
//
//==============================================================================================

static const struct{
	u32  str_id;
	u32  param;
}DebugMenuList3[] = {
	{ msg_debug_nohara_11, (u32)D_Nohara_11 },
};

static const BMPLIST_HEADER DebugListH3 = {
	NULL,					//表示文字データポインタ

	NULL,					//カーソル移動ごとのコールバック関数
	NULL,					//一列表示ごとのコールバック関数

	NULL,					//GF_BGL_BMPWINのポインタ

	NELEMS(DebugMenuList3),	//リスト項目数
	9,						//表示最大項目数

	0,						//ラベル表示Ｘ座標
	8,						//項目表示Ｘ座標
	0,						//カーソル表示Ｘ座標
	0,						//表示Ｙ座標
/*
	FBMP_COL_BLACK,			//文字色
	FBMP_COL_WHITE,			//背景色
	FBMP_COL_BLK_SDW,		//文字影色
*/
	FBMP_COL_WHITE,			//文字色
	FBMP_COL_BLACK,			//背景色
	FBMP_COL_BLK_SDW,		//文字影色
	0,						//文字間隔Ｘ
	16,						//文字間隔Ｙ
	BMPLIST_LRKEY_SKIP,		//ページスキップタイプ
	FONT_SYSTEM,			//文字指定(本来は u8 だけど、そんなに作らないと思うので)
	0						//ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)
};

//--------------------------------------------------------------
/**
 * @brief	BGMデバックメニュー呼び出し(fld_debug.c)
 *
 * @param	fsys	FIELDSYS_WORK型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
void DebugScriptMenuInit( FIELDSYS_WORK* fsys )
{
	int i;
	BMPLIST_HEADER list_h;
	D_NOHARA_WORK* wk;

	wk = (D_NOHARA_WORK*)TCB_GetWork( PMDS_taskAdd(DebugNoharaMenuMain, sizeof(D_NOHARA_WORK), 0, HEAPID_BASE_DEBUG) );

	wk->seq				= 0;
	wk->sel				= 0;
	wk->work			= 0;
	wk->wave_buf_flag	= 0;								//波形格納バッファを確保したフラグOFF
	wk->waveout_flag	= 0;								//鳴き声再生中フラグOFF
	wk->fsys			= fsys;

	GF_BGL_BmpWinAddEx( wk->fsys->bgl, &wk->bmpwin[0], &DebugNoharaWinData );	//ビットマップ追加

	wk->menulist = BMP_MENULIST_Create( NELEMS(DebugMenuList3), HEAPID_BASE_DEBUG );

	wk->wordset = WORDSET_Create( HEAPID_BASE_DEBUG );

	wk->msgman = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG, 
								NARC_msg_debug_nohara_dat, HEAPID_BASE_DEBUG);

	for( i=0; i < NELEMS(DebugMenuList3); i++ ){
		BMP_MENULIST_AddArchiveString( wk->menulist, wk->msgman, 
										DebugMenuList3[i].str_id, DebugMenuList3[i].param );
	}

	list_h			= DebugListH3;
	list_h.list		= wk->menulist;
	list_h.win		= &wk->bmpwin[0];
	wk->lw			= BmpListSet( &list_h, 0, 0, HEAPID_BASE_DEBUG );

	GF_BGL_BmpWinOn( &wk->bmpwin[0] );
	return;
}

#include "savedata/wifilist.h"
//--------------------------------------------------------------
/**
 * @brief	友達手帳のカンストチェック用処理
 *
 * @param	savedata	SAVEDATA型のポインタ
 *
 * @retval	none
 *
 * メモ：この関数が呼ばれる処理で、
 *       友達をMAXにする処理は、1人友達がいないと機能しない
 */
//--------------------------------------------------------------
void Debug_FrontierWifiMultiRecordMaxSet( FIELDSYS_WORK* fsys );
void Debug_FrontierWifiMultiRecordMaxSet( FIELDSYS_WORK* fsys )
{
#ifdef PM_DEBUG
	int i;
	//LOAD_RESULT load_result;
	//SAVE_RESULT save_result;
	POKEMON_PARAM* poke;
	FACTORYSCORE* fa_score_sv;
	STAGESCORE* s_score_sv;
	CASTLESCORE* ca_score_sv;
	ROULETTESCORE* r_score_sv;
	FRONTIER_SAVEWORK * fr_sv;
	SAVEDATA* savedata;
	//FRONTIER_EX_SAVEDATA* fes;

	savedata    = fsys->savedata;
	fr_sv		= SaveData_GetFrontier( savedata );
	fa_score_sv = SaveData_GetFactoryScore( savedata );
	fa_score_sv = SaveData_GetFactoryScore( savedata );
	s_score_sv	= SaveData_GetStageScore( savedata );
	ca_score_sv = SaveData_GetCastleScore( savedata );
	r_score_sv	= SaveData_GetRouletteScore( savedata );

	for( i=0; i < WIFILIST_FRIEND_MAX; i++ ){

		////////////////////
		//タワー
		////////////////////

		//"7連勝(クリア)したかフラグ"書き出し
		FrontierRecord_Set(	fr_sv, FRID_TOWER_MULTI_WIFI_CLEAR_BIT, i, 1 );

		//"連勝数"書き出し
		FrontierRecord_Set(	fr_sv, FRID_TOWER_MULTI_WIFI_RENSHOU_CNT, i, 9999 );

		//"最高連勝数"書き出し
		FrontierRecord_Set(	fr_sv, FRID_TOWER_MULTI_WIFI_RENSHOU, i, 9999 );

		////////////////////
		//ファクトリー(LV50)
		////////////////////

		//"7連勝(クリア)したかフラグ"書き出し
		FrontierRecord_Set(	fr_sv, FRID_FACTORY_MULTI_WIFI_CLEAR_BIT, i, 1 );

		//"連勝数"書き出し
		FrontierRecord_Set(	fr_sv, FactoryScr_GetWinRecordID(0,FACTORY_TYPE_WIFI_MULTI), 
							i, FACTORY_RENSYOU_MAX );

		//"最高連勝数"書き出し
		FrontierRecord_Set(	fr_sv, FactoryScr_GetMaxWinRecordID(0,FACTORY_TYPE_WIFI_MULTI), 
							i, FACTORY_RENSYOU_MAX );

		//"交換数"書き出し
		FrontierRecord_Set(	fr_sv, FactoryScr_GetTradeRecordID(0,FACTORY_TYPE_WIFI_MULTI),
							i, FACTORY_TRADE_MAX );

		//"最大交換数"書き出し
		FrontierRecord_Set(	fr_sv, FactoryScr_GetMaxTradeRecordID(0,FACTORY_TYPE_WIFI_MULTI),
							i, FACTORY_TRADE_MAX );

		////////////////////
		//ファクトリー(LV100)
		////////////////////

		//"7連勝(クリア)したかフラグ"書き出し
		FrontierRecord_Set(	fr_sv, FRID_FACTORY_MULTI_WIFI_CLEAR100_BIT, i, 1 );

		//"連勝数"書き出し
		FrontierRecord_Set(	fr_sv, FactoryScr_GetWinRecordID(1,FACTORY_TYPE_WIFI_MULTI),
							i, FACTORY_RENSYOU_MAX );

		//"最高連勝数"書き出し
		FrontierRecord_Set(	fr_sv, FactoryScr_GetMaxWinRecordID(1,FACTORY_TYPE_WIFI_MULTI),
							i, FACTORY_RENSYOU_MAX );

		//"交換数"書き出し
		FrontierRecord_Set(	fr_sv, FactoryScr_GetTradeRecordID(1,FACTORY_TYPE_WIFI_MULTI),
							i, FACTORY_TRADE_MAX );

		//"最大交換数"書き出し
		FrontierRecord_Set(	fr_sv, FactoryScr_GetMaxTradeRecordID(1,FACTORY_TYPE_WIFI_MULTI),
							i, FACTORY_TRADE_MAX );

		////////////////////
		//ステージ
		////////////////////

		//先頭のポケモンにする
		poke = PokeParty_GetMemberPointer( SaveData_GetTemotiPokemon(savedata), 0 );

		//"7連勝(クリア)したかフラグ"書き出し
		FrontierRecord_Set(	fr_sv, FRID_STAGE_MULTI_WIFI_CLEAR_BIT, i, 1 );

		//"連勝数"書き出し
		FrontierRecord_Set(	fr_sv, StageScr_GetWinRecordID(STAGE_TYPE_WIFI_MULTI),
							i, STAGE_RENSYOU_MAX );

		//"最高連勝数"書き出し
		FrontierRecord_Set(	fr_sv, StageScr_GetMaxWinRecordID(STAGE_TYPE_WIFI_MULTI),
							i, STAGE_RENSYOU_MAX );

		//"連勝中のポケモンナンバー"書き出し
		FrontierRecord_Set(	fr_sv, StageScr_GetMonsNoRecordID(STAGE_TYPE_WIFI_MULTI),
							i, PokeParaGet(poke,ID_PARA_monsno,NULL) );

		////////////////////
		//キャッスル
		////////////////////
		
		//"7連勝(クリア)したかフラグ"書き出し
		FrontierRecord_Set(	fr_sv, FRID_CASTLE_MULTI_WIFI_CLEAR_BIT, i, 1 );

		//"連勝数"書き出し
		FrontierRecord_Set(	fr_sv, CastleScr_GetWinRecordID(CASTLE_TYPE_WIFI_MULTI),
							i, CASTLE_RENSYOU_MAX );

		//"最高連勝数"書き出し
		FrontierRecord_Set(	fr_sv, CastleScr_GetMaxWinRecordID(CASTLE_TYPE_WIFI_MULTI),
							i, CASTLE_RENSYOU_MAX );

		//"CP"書き出し
		FrontierRecord_Set(	fr_sv, CastleScr_GetCPRecordID(CASTLE_TYPE_WIFI_MULTI),
							i, CASTLE_CP_MAX );

		//"最高CP"書き出し
		FrontierRecord_Set(	fr_sv, CastleScr_GetRemainderCPRecordID(CASTLE_TYPE_WIFI_MULTI),
							i, CASTLE_CP_MAX );

		////////////////////
		//ルーレット
		////////////////////

		//"7連勝(クリア)したかフラグ"書き出し
		FrontierRecord_Set(	fr_sv, FRID_ROULETTE_MULTI_WIFI_CLEAR_BIT, i, 1 );

		//"連勝数"書き出し
		FrontierRecord_Set(	fr_sv, RouletteScr_GetWinRecordID(ROULETTE_TYPE_WIFI_MULTI),
							i, ROULETTE_RENSYOU_MAX );

		//"最高連勝数"書き出し
		FrontierRecord_Set(	fr_sv, RouletteScr_GetMaxWinRecordID(ROULETTE_TYPE_WIFI_MULTI),
							i, ROULETTE_RENSYOU_MAX );
	}
#endif

#if 0
	//ステージ"最高連勝中のポケモンナンバー"書き出し

	fes = FrontierEx_Load( savedata, HEAPID_WORLD, &load_result );
	if( load_result != LOAD_RESULT_OK ){
		//
	}else{

		Debug_FrontierEx_StageRenshou_Set( fes, FREXID_STAGE_RENSHOU_SINGLE, i, 500 );

#if 1
		//フィールド動作モデルのセーブ
		Field_SaveFieldObj( fsys );

		// ポケッチ関連データのセーブ
		Field_SendPoketchInfo( fsys, POKETCH_SEND_SAVE, 0 );

		//位置を引っ張る
		fsys->location->grid_x	= Player_NowGPosXGet( fsys->player );
		fsys->location->grid_z	= Player_NowGPosZGet( fsys->player );
		fsys->location->door_id = DOOR_ID_JUMP_CODE;
		fsys->location->dir		= Player_DirGet( fsys->player );
#endif

		//外部セーブを呼ばれないと反映しない
		save_result = FrontierEx_Save( savedata, fes );
	}

	if( fes != NULL ){
		sys_FreeMemoryEz(fes);
	}
#endif

	return;
}


