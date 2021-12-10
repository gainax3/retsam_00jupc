//==============================================================================================
/**
 * @file	roulette.c
 * @brief	「バトルルーレット」メインソース
 * @author	Satoshi Nohara
 * @date	07.09.06
 */
//==============================================================================================
#include "system/main.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "system/clact_tool.h"
#include "system/brightness.h"
#include "system/wipe.h"
#include "system/snd_tool.h"
#include "system/pm_str.h"
#include "system/buflen.h"
#include "system/wordset.h"
#include "system/bmp_menu.h"
#include "system/window.h"
#include "system/fontproc.h"
#include "system/numfont.h"
#include "gflib/strbuf_family.h"
#include "gflib/touchpanel.h"
#include "savedata/config.h"
#include "application/p_status.h"
#include "poketool/pokeparty.h"
#include "poketool/poke_number.h"
#include "poketool/pokeicon.h"
#include "savedata/b_tower.h"
#include "savedata/frontier_savedata.h"
#include "battle/tokusyu_def.h"

#include "communication/comm_system.h"
#include "communication/comm_tool.h"
#include "communication/comm_info.h"
#include "communication/comm_def.h"
#include "communication/wm_icon.h"

#include "../roulette_def.h"
#include "roulette_sys.h"
#include "roulette_clact.h"
#include "roulette_bmp.h"
#include "roulette_obj.h"
#include "roulette.dat"
#include "application/roulette.h"
#include "../frontier_tool.h"					//Frontier_PokeParaMake
#include "../roulette_tool.h"						//ROULETTE_POKE_RANGE
#include "../comm_command_frontier.h"

#include "../../field/comm_direct_counter_def.h"//
#include "../../field/fieldobj.h"
#include "../../field/scr_tool.h"
#include "field/weather_no.h"

#include "msgdata/msg.naix"						//NARC_msg_??_dat
#include "msgdata/msg_roulette.h"				//msg_??

#include "../graphic/frontier_obj_def.h"
#include "../graphic/frontier_bg_def.h"


#include "system/pm_overlay.h"
FS_EXTERN_OVERLAY(frontier_common);


//==============================================================================================
//
//	デバック用
//
//==============================================================================================
//PROC_DATA* p_proc;

#ifdef PM_DEBUG
//#define DEBUG_DECIDE_EV_NO		//定義有効で、↓のパネルナンバーを強制セット
#define DEBUG_PANEL_NO			(ROULETTE_EV_MINE_DOKU)
//#define DEBUG_PANEL_NO			(ROULETTE_EV_ENEMY_ITEM_GET)
//#define DEBUG_PANEL_NO			(ROULETTE_EV_MINE_ITEM_GET)
//#define DEBUG_PANEL_NO			(ROULETTE_EV_EX_POKE_CHANGE)
//#define DEBUG_PANEL_NO			(ROULETTE_EV_ENEMY_NEMURI)
//#define DEBUG_PANEL_NO			(ROULETTE_EV_MINE_NEMURI)
#endif

static u8 debug_panel_no;
static u8 debug_panel_flag;


//==============================================================================================
//
//	定義
//
//==============================================================================================
//シーケンス定義
enum {
	SEQ_GAME_INIT,											//初期化

	SEQ_GAME_CSR_MOVE,										//カーソル移動中

	SEQ_GAME_SEND_RECV,										//送受信

	SEQ_GAME_END_MULTI,										//(通信)終了
	SEQ_GAME_END,											//終了
};

//フォントカラー
#define	COL_BLUE	( GF_PRINTCOLOR_MAKE(FBMP_COL_BLUE,FBMP_COL_BLU_SDW,FBMP_COL_NULL) )//青
#define	COL_RED		( GF_PRINTCOLOR_MAKE(FBMP_COL_RED,FBMP_COL_RED_SDW,FBMP_COL_NULL) )	//赤

//情報メッセージの表示位置
enum{
	INFO_SINGLE_OFFSET_X	= 8*8,
	INFO_MULTI_L_OFFSET_X	= 0,			//ペアのオフセット(Ｌ)
	INFO_MULTI_R_OFFSET_X	= 16*8,			//ペアのオフセット(Ｒ)

	INFO_PLAYER_X		= 1, 
	INFO_PLAYER_Y		= 1,
	INFO_PLAYER_CP_X	= 7*8, 
	INFO_PLAYER_CP_Y	= INFO_PLAYER_Y,

	INFO_KAIHUKU_X		= 1, 
	INFO_KAIHUKU_Y		= 1,
	INFO_KAIHUKU_RANK_X = 8*8, 
	INFO_KAIHUKU_RANK_Y = INFO_KAIHUKU_Y,

	INFO_ITEM_X			= 1,
	INFO_ITEM_Y			= 4*8,
	INFO_ITEM_RANK_X	= 8*8,
	INFO_ITEM_RANK_Y	= INFO_ITEM_Y,

	INFO_INFO_X			= 1,
	INFO_INFO_Y			= 8*8,
	INFO_INFO_RANK_X	= 8*8,
	INFO_INFO_RANK_Y	= INFO_INFO_Y,
};

//右側カーソルのオフセット
#define SEL_CSR_L_OFFSET	(8)
#define SEL_CSR_R_OFFSET	(120)

//「もどる」の位置
#define CSR_MODORU_X		(8*8)
#define CSR_MODORU_Y		(22*8+4)

#define ROULETTE_DECIDE_NONE	(0xff)				//決定無効

//パネル配置
enum{
	PANEL_START_X	= 68,//64,						//開始X
	PANEL_START_Y	= 36,//32,						//開始Y
	PANEL_WIDTH_X	= 40,//32,
	PANEL_WIDTH_Y	= 40,//32,
};

//ポケモンアイコン配置(少し見切れる形で行く)
enum{
	//ICON_MINE_START_X	= 24,						//開始X
	ICON_MINE_START_X	= 16,						//開始X
	//ICON_ENEMY_START_X	= 232,						//開始X
	ICON_ENEMY_START_X	= 238,						//開始X
	ICON_START_Y		= 46,						//開始Y
	ICON_START_MULTI_Y	= 22,						//開始Y
	ICON_WIDTH_Y		= 40,//32,
};

//アイテム持っているかアイコン配置
enum{
	ITEMKEEP_MINE_START_X	= (ICON_MINE_START_X + 8),	//開始X
	ITEMKEEP_ENEMY_START_X	= (ICON_ENEMY_START_X + 8),	//開始X
	ITEMKEEP_START_Y		= (ICON_START_Y + 4),		//開始Y
	ITEMKEEP_START_MULTI_Y	= (ICON_START_MULTI_Y + 4),	//開始Y
	ITEMKEEP_WIDTH_Y		= ICON_WIDTH_Y,
};

//ボタン配置
enum{
	BUTTON_START_X	= 128,//132,//64,					//開始X
	BUTTON_START_Y	= 96,//32,							//開始Y
	BUTTON_WIDTH_X	= 64,
	BUTTON_WIDTH_Y	= 64,
	TP_BUTTON_CX	= 128,								//ボタンの中心X
	TP_BUTTON_CY	= 96,								//ボタンの中心Y
	TP_BUTTON_R		= 32,								//半径
};

//ボタンのタッチ判定テーブル
static const TP_HIT_TBL ButtonHitTbl[] =
{
	{ TP_USE_CIRCLE, TP_BUTTON_CX, TP_BUTTON_CY, 32 },
	{ TP_HIT_END, 0, 0, 0 }
};

#define ROULETTE_COUNT_321_WAIT			( 24 )			//カウントダウンウェイト
#define ROULETTE_PANEL_WAIT				( 30 )			//パネル表示ウェイト

#define ROULETTE_GAME_TIME				( 30 * 30 )		//ゲーム時間(30秒)

#define ROULETTE_PANEL_EFF_WAIT			( 4 )			//パネルエフェクトスクリーンの切り替え

///半透明第1対象面
#define ROULETTE_BLD_PLANE_1			(GX_BLEND_PLANEMASK_BG2)

///半透明第2対象面
#define ROULETTE_BLD_PLANE_2			(GX_BLEND_BGALL | GX_BLEND_PLANEMASK_OBJ)

///第1対象面に対するαブレンディング係数
#define ROULETTE_BLD_ALPHA_1			(8)

///第2対象面に対するαブレンディング係数
#define ROULETTE_BLD_ALPHA_2			(14)

#define ROULETTE_START_WAIT				(10)			//3,2,1開始までのウェイト


//==============================================================================================
//
//	メッセージ関連の定義
//
//==============================================================================================
#define ROULETTE_MENU_BUF_MAX	(2)								//メニューバッファの最大数
#define BR_FONT					(FONT_SYSTEM)					//フォント種類
#define ROULETTE_MSG_BUF_SIZE	(600)//(800)//(1024)			//会話のメッセージsize
#define ROULETTE_MENU_BUF_SIZE	(32)							//メニューのメッセージsize
#define PLAYER_NAME_BUF_SIZE	(PERSON_NAME_SIZE + EOM_SIZE)	//プレイヤー名のメッセージsize
#define POKE_NAME_BUF_SIZE		(MONS_NAME_SIZE + EOM_SIZE)		//ポケモン名のメッセージsize


//==============================================================================================
//
//	構造体宣言
//
//==============================================================================================
struct _ROULETTE_WORK{

	PROC* proc;										//PROCへのポインタ
	FRONTIER_SAVEWORK* fro_sv;						//

	u8	sub_seq;									//シーケンス
	u8	type;										//引数として渡されたバトルタイプ
	u8	msg_index;									//メッセージindex
	u8	wait;

	u8	random_flag;								//カーソル移動ランダムonフラグ
	u8	csr_pos;									//現在のカーソル位置
	u8	parent_decide_pos;							//決定されたカーソル位置
	u8	recieve_count;								//受信カウント

	u8	csr_wait;									//カーソルが動くウェイト
	u8	point_rank;
	u8	point;										//戦闘結果ポイント
	u8	panel_scrn:1;
	u8	panel_wait:7;

	u8	suberi_csr_pos;
	u8  dummy16;
	u16	start_csr_pos;								//開始カーソル位置

	int game_timer;									//30秒で強制終了

	u8*	p_csr_speed_level;							//カーソルが動くスピードのレベル
	u8* p_decide_ev_no;								//決定したイベントナンバー
	u16* p_rand_pos;								//ポケパーティのどの位置からチェックするか

	u16	rensyou;									//何戦目か
	u16	lap;										//周回数

	u8	h_max;										//横の最大数
	u8	v_max;										//縦の最大数
	u8	hv_max;										//横*縦
	u8	menu_flag:1;								//メニュー表示中かフラグ
	u8	rankup_req:7;								//ランクアップしたいリクエストフラグ

	u8	panel_color_num[ROULETTE_PANEL_COLOR_MAX];	//パネルの色ごとの数を保存

	u8	panel_no[ROULETTE_PANEL_MAX];				//パネルナンバー

	u8	ev_tbl[ROULETTE_EV_MAX];					//選出リスト作成用
	u8	ev_tbl_max;									//選出リスト最大数

	MSGDATA_MANAGER* msgman;						//メッセージマネージャー
	WORDSET* wordset;								//単語セット
	STRBUF* msg_buf;								//メッセージバッファポインタ
	STRBUF* tmp_buf;								//テンポラリバッファポインタ

	STRBUF* menu_buf[ROULETTE_MENU_BUF_MAX];		//メニューバッファポインタ
	STRCODE str[PERSON_NAME_SIZE + EOM_SIZE];		//メニューのメッセージ

	GF_BGL_INI*	bgl;								//BGLへのポインタ
	GF_BGL_BMPWIN bmpwin[ROULETTE_BMPWIN_MAX];		//BMPウィンドウデータ

	//BMPメニュー(bmp_menu.h)
	BMPMENU_HEADER MenuH;							//BMPメニューヘッダー
	BMPMENU_WORK* mw;								//BMPメニューワーク
	BMPMENU_DATA Data[ROULETTE_MENU_BUF_MAX];		//BMPメニューデータ

	PALETTE_FADE_PTR pfd;							//パレットフェード

	//const CONFIG* config;							//コンフィグポインタ
	CONFIG* config;									//コンフィグポインタ
	SAVEDATA* sv;									//セーブデータポインタ
	ROULETTEDATA* roulette_sv;						//キャッスルセーブデータポインタ
	ROULETTESCORE* score_sv;						//キャッスルセーブデータポインタ

	ROULETTE_CLACT roulette_clact;					//セルアクタデータ
	ROULETTE_OBJ* p_csr;							//カーソルOBJのポインタ格納テーブル
	ROULETTE_OBJ* p_panel[ROULETTE_PANEL_MAX];		//パネルOBJのポインタ格納テーブル
	ROULETTE_OBJ* p_m_icon[ROULETTE_COMM_POKE_TOTAL_NUM];//ポケモンアイコンOBJのポインタ格納テーブル
	ROULETTE_OBJ* p_e_icon[ROULETTE_COMM_POKE_TOTAL_NUM];//ポケモンアイコンOBJのポインタ格納テーブル
	ROULETTE_OBJ* p_m_itemkeep[ROULETTE_COMM_POKE_TOTAL_NUM];//アイテム持っているかアイコンOBJ
	ROULETTE_OBJ* p_e_itemkeep[ROULETTE_COMM_POKE_TOTAL_NUM];//アイテム持っているかアイコンOBJ
	ROULETTE_OBJ* p_button;							//ボタンOBJのポインタ格納テーブル

	int* p_weather;									//天候
	u16* p_ret_work;								//ROULETTE_CALL_WORKの戻り値ワークへのポインタ
	POKEPARTY* p_m_party;
	POKEPARTY* p_e_party;

	ARCHANDLE* hdl;

	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//ROULETTE_COMM roulette_comm;
#if 1
	//通信用：データバッファ
	u16	send_buf[ROULETTE_COMM_BUF_LEN];

	//通信用
	u8	pair_csr_pos;								//パートナーのカーソル位置
	u8	pair_sel_pos;								//パートナーのランクアップしたいカーソル位置

	u16 pair_rensyou;
	u16 dummy26;
#endif
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	
	u32 dummy_work;
};


//==============================================================================================
//
//	プロトタイプ宣言
//
//==============================================================================================
//PROC
PROC_RESULT RouletteProc_Init( PROC * proc, int * seq );
PROC_RESULT RouletteProc_Main( PROC * proc, int * seq );
PROC_RESULT RouletteProc_End( PROC * proc, int * seq );

//シーケンス
static BOOL Seq_GameInit( ROULETTE_WORK* wk );
static BOOL Seq_GameCsrMove( ROULETTE_WORK* wk );
static BOOL Seq_GameSendRecv( ROULETTE_WORK* wk );
static BOOL Seq_GameEndMulti( ROULETTE_WORK* wk );
static BOOL Seq_GameEnd( ROULETTE_WORK* wk );

//共通処理
static void RouletteCommon_Delete( ROULETTE_WORK* wk );
static void Roulette_InitSub1( void );
static void Roulette_InitSub2( ROULETTE_WORK* wk );

//共通初期化、終了
static void Roulette_ObjInit( ROULETTE_WORK* wk );
static void Roulette_BgInit( ROULETTE_WORK* wk );
static void Roulette_BgExit( GF_BGL_INI * ini );

//設定
static void VBlankFunc( void * work );
static void SetVramBank(void);
static void SetBgHeader( GF_BGL_INI * ini );

//BGグラフィックデータ
static void Roulette_SetMainBgGraphic( ROULETTE_WORK * wk, u32 frm );
static void Roulette_SetMain2BgGraphic( ROULETTE_WORK * wk, u32 frm );
//static void Roulette_SetPanelEffBgGraphic( ROULETTE_WORK * wk, u32 frm, u8 flag );
static void Roulette_SetMainBgPalette( void );
static void Roulette_SetSubBgGraphic( ROULETTE_WORK * wk, u32 frm  );

//メッセージ
static u8 RouletteWriteMsg( ROULETTE_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font );
static u8 RouletteWriteMsgSimple( ROULETTE_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font );
static u8 Roulette_EasyMsg( ROULETTE_WORK* wk, int msg_id );

//メニュー
static void RouletteInitMenu( ROULETTE_WORK* wk, GF_BGL_BMPWIN* win, u8 y_max );
static void RouletteSetMenuData( ROULETTE_WORK* wk, u8 no, u8 param, int msg_id );
static void Roulette_SetMenu2( ROULETTE_WORK* wk );

//文字列
static void Roulette_SetNumber( ROULETTE_WORK* wk, u32 bufID, s32 number, u32 keta, NUMBER_DISPTYPE disp );
static void Roulette_SetPokeName( ROULETTE_WORK* wk, u32 bufID, POKEMON_PASO_PARAM* ppp );
static void Roulette_SetPlayerName( ROULETTE_WORK* wk, u32 bufID );
static void PlayerNameWrite( ROULETTE_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 font );
static void PairNameWrite( ROULETTE_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 font );
static void TalkWinWrite( ROULETTE_WORK* wk );

//ツール
static BOOL Roulette_CheckType( ROULETTE_WORK* wk, u8 type );
static void NextSeq( ROULETTE_WORK* wk, int* seq, int next );
static int KeyCheck( int key );
static void BgCheck( ROULETTE_WORK* wk );
static void CsrMove( ROULETTE_WORK* wk, int key );
static void CsrPosSet( ROULETTE_WORK* wk, u8 csr_pos );
static u16 GetCsrX( ROULETTE_WORK* wk, u8 csr_pos, u8 lr );
static u16 GetCsrY( ROULETTE_WORK* wk, u8 csr_pos );
static void Roulette_GetOffset( ROULETTE_WORK* wk, u16* offset_x, u16* offset_y, u16* pair_offset_x, u16* pair_offset_y );
static void Roulette_SetCsrPosSuberi( ROULETTE_WORK* wk );
static u8 GetCommSelCsrPos( u8 type_offset, u8 decide_type );
static u16 Roulette_CommGetRensyou( ROULETTE_WORK* wk );

//通信
BOOL Roulette_CommSetSendBuf( ROULETTE_WORK* wk, u16 type, u16 param );
void Roulette_CommSendBufBasicData( ROULETTE_WORK* wk, u16 type );
void Roulette_CommRecvBufBasicData(int id_no,int size,void *pData,void *work);
void Roulette_CommSendBufRankUpType( ROULETTE_WORK* wk, u16 type, u16 param );
void Roulette_CommRecvBufRankUpType(int id_no,int size,void *pData,void *work);
void Roulette_CommSendBufCsrPos( ROULETTE_WORK* wk, u16 type );
void Roulette_CommRecvBufCsrPos(int id_no,int size,void *pData,void *work);

//サブシーケンス
static void Roulette_SeqSubPanelDecide( ROULETTE_WORK* wk, u8 decide_type );

//イベント
//static void Roulette_EvSet( ROULETTE_WORK* wk, u8 no );
static u8 Roulette_GetDecideColor( ROULETTE_WORK* wk, u8 point_rank );
static void Roulette_EvPanelChoice( ROULETTE_WORK* wk );
static void Roulette_EvPanelChoice2( ROULETTE_WORK* wk );
static void Roulette_EvPanelChoice3( ROULETTE_WORK* wk );
static void Roulette_ItemKeepVanishSet( ROULETTE_WORK* wk );

//タッチパネル
static BOOL Roulette_CheckButtonPush( ROULETTE_WORK* wk );
static void Roulette_SetButtonPush( ROULETTE_WORK* wk );

//共通のランダムを決めておく
static u16 Roulette_GetRandPos( ROULETTE_WORK* wk );


//==============================================================================================
//
//	PROC
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	プロセス関数：初期化
 *
 * @param	proc	プロセスデータ
 * @param	seq		シーケンス
 *
 * @return	"処理状況"
 */
//--------------------------------------------------------------
PROC_RESULT RouletteProc_Init( PROC * proc, int * seq )
{
	int i;
	ROULETTE_WORK* wk;
	ROULETTE_CALL_WORK* roulette_call;

	Overlay_Load( FS_OVERLAY_ID(frontier_common), OVERLAY_LOAD_NOT_SYNCHRONIZE );

	Roulette_InitSub1();

	sys_CreateHeap( HEAPID_BASE_APP, HEAPID_ROULETTE, 0x20000 );

	wk = PROC_AllocWork( proc, sizeof(ROULETTE_WORK), HEAPID_ROULETTE );
	memset( wk, 0, sizeof(ROULETTE_WORK) );

	wk->bgl				= GF_BGL_BglIniAlloc( HEAPID_ROULETTE );
	wk->proc			= proc;
	roulette_call		= (ROULETTE_CALL_WORK*)PROC_GetParentWork( proc );
	wk->sv				= roulette_call->sv;
	wk->roulette_sv		= SaveData_GetRouletteData( wk->sv );
	wk->score_sv		= SaveData_GetRouletteScore( wk->sv );
	wk->type			= roulette_call->type;
	wk->lap				= roulette_call->lap;
	wk->rensyou			= roulette_call->rensyou;
	wk->pair_rensyou	= roulette_call->pair_rensyou;
	wk->point			= roulette_call->point;
	wk->p_ret_work		= &roulette_call->ret_work;
	wk->config			= SaveData_GetConfig( wk->sv );			//コンフィグポインタを取得
	wk->p_m_party		= roulette_call->p_m_party;
	wk->p_e_party		= roulette_call->p_e_party;
	wk->parent_decide_pos = ROULETTE_DECIDE_NONE;
	wk->fro_sv			= SaveData_GetFrontier( wk->sv );
	wk->p_weather		= roulette_call->p_weather;
	wk->p_csr_speed_level = roulette_call->p_csr_speed_level;
	wk->p_decide_ev_no	= roulette_call->p_decide_ev_no;
	wk->p_rand_pos		= roulette_call->p_rand_pos;
	OS_Printf( "p_csr_speed_level = %d\n", *wk->p_csr_speed_level );
	wk->game_timer		= ROULETTE_GAME_TIME;
	wk->random_flag		= roulette_call->random_flag;

	for( i=0; i < ROULETTE_EV_MAX ;i++ ){
		wk->ev_tbl[i] = ROULETTE_EV_NONE;
	}

	wk->h_max = ROULETTE_PANEL_H_MAX;
	wk->v_max = ROULETTE_PANEL_V_MAX;
	wk->hv_max= ROULETTE_PANEL_MAX;

	//wk->pair_csr_pos	= (wk->h_max / 2);
	wk->pair_csr_pos	= 0;

	//共通のランダムを決めておく
	(*wk->p_rand_pos) = Roulette_GetRandPos( wk );

	//開始カーソル位置
	wk->start_csr_pos = ( gf_rand() % ROULETTE_PANEL_MAX );

	Roulette_InitSub2( wk );

	if( Roulette_CommCheck(wk->type) == TRUE ){
		CommCommandFrontierInitialize( wk );
	}

	(*seq) = SEQ_GAME_INIT;
	OS_Printf( "(*seq) = %d\n", (*seq) );

	debug_panel_no = ROULETTE_EV_ENEMY_HP_DOWN;			//デバックパネルナンバー
	debug_panel_flag = 0;

	return PROC_RES_FINISH;
}

//--------------------------------------------------------------
/**
 * @brief	プロセス関数：メイン
 *
 * @param	proc	プロセスデータ
 * @param	seq		シーケンス
 *
 * @return	"処理状況"
 */
//--------------------------------------------------------------
PROC_RESULT RouletteProc_Main( PROC * proc, int * seq )
{
	ROULETTE_WORK* wk  = PROC_GetWork( proc );
	//OS_Printf( "(*seq) = %d\n", (*seq) );

	//BgCheck( wk );

	if( wk->parent_decide_pos != ROULETTE_DECIDE_NONE ){
		switch( *seq ){

		//送受信へ強制移動
		case SEQ_GAME_CSR_MOVE:
			OS_Printf( "ボタン押された！\n" );
			Roulette_SetButtonPush( wk );

			//受信したカーソル位置をセット
			wk->suberi_csr_pos = GetCommSelCsrPos( wk->hv_max, wk->parent_decide_pos );
			//RouletteObj_Vanish( wk->p_csr, ROULETTE_VANISH_ON );					//非表示

			NextSeq( wk, seq, SEQ_GAME_SEND_RECV );
			break;

		//何もしない
		//case SEQ_GAME_INIT:
		//case SEQ_GAME_SEND_RECV:
		//case SEQ_GAME_END_MULTI:
		//case SEQ_GAME_END:
		//	break;
		};
	}

	switch( *seq ){

	//-----------------------------------
	//初期化
	case SEQ_GAME_INIT:
		if( Seq_GameInit(wk) == TRUE ){
			NextSeq( wk, seq, SEQ_GAME_CSR_MOVE );					//カーソル移動中へ
		}
		break;

	//-----------------------------------
	//カーソル移動中
	case SEQ_GAME_CSR_MOVE:

		if( Seq_GameCsrMove(wk) == TRUE ){

			if( wk->rankup_req == 1 ){
				NextSeq( wk, seq, SEQ_GAME_SEND_RECV );					//送受信へ
			}else{

				//通信タイプの時
				if( Roulette_CommCheck(wk->type) == TRUE ){
					NextSeq( wk, seq, SEQ_GAME_END_MULTI );				//(通信)終了へ
				}else{
					NextSeq( wk, seq, SEQ_GAME_END );					//終了へ
				}
			}

		}
		break;

	//-----------------------------------
	//送受信
	case SEQ_GAME_SEND_RECV:
		if( Seq_GameSendRecv(wk) == TRUE ){
			NextSeq( wk, seq, SEQ_GAME_END_MULTI );						//(通信)終了へ
		}
		break;

	//-----------------------------------
	//通信交換終了
	case SEQ_GAME_END_MULTI:
		if( Seq_GameEndMulti(wk) == TRUE ){
			NextSeq( wk, seq, SEQ_GAME_END );							//終了へ
		}
		break;

	//-----------------------------------
	//終了
	case SEQ_GAME_END:
		if( Seq_GameEnd(wk) == TRUE ){
			return PROC_RES_FINISH;
		}
		break;

	}

#if 1
	//パネル演出BGのスクリーン切り替え
	wk->panel_wait++;
	if( wk->panel_wait >= ROULETTE_PANEL_EFF_WAIT ){
		wk->panel_wait = 0;
		wk->panel_scrn ^= 1;
		//Roulette_SetPanelEffBgGraphic( wk, BR_FRAME_EFF, (wk->panel_scrn + 1) );	//1か2を渡す
	}
#endif

	CLACT_Draw( wk->roulette_clact.ClactSet );		//セルアクター常駐関数

	return PROC_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief	プロセス関数：終了
 *
 * @param	proc	プロセスデータ
 * @param	seq		シーケンス
 *
 * @return	"処理状況"
 */
//--------------------------------------------------------------
PROC_RESULT RouletteProc_End( PROC * proc, int * seq )
{
	int i;
	ROULETTE_WORK* wk = PROC_GetWork( proc );

	*(wk->p_ret_work) = wk->csr_pos;					//戻り値格納ワークへ代入(カーソル位置)

	OS_Printf( "*(wk->p_ret_work) = %d\n", *(wk->p_ret_work) );

	DellVramTransferManager();

	RouletteCommon_Delete( wk );						//削除処理

	PROC_FreeWork( proc );								//ワーク開放

	sys_VBlankFuncChange( NULL, NULL );					//VBlankセット
	sys_DeleteHeap( HEAPID_ROULETTE );

	Overlay_UnloadID( FS_OVERLAY_ID(frontier_common) );

	return PROC_RES_FINISH;
}


//==============================================================================================
//
//	シーケンス
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	シーケンス：ゲーム初期化
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameInit( ROULETTE_WORK* wk )
{
	switch( wk->sub_seq ){

	//同期開始
	case 0:
		if( Roulette_CommCheck(wk->type) == TRUE ){
			CommToolTempDataReset();
			CommTimingSyncStart( DBC_TIM_ROULETTE_INIT_5 );
			wk->sub_seq++;
		}else{
			wk->sub_seq++;
		}
		break;

	//同期待ち
	case 1:
		if( Roulette_CommCheck(wk->type) == TRUE ){
			//タイミングコマンドが届いたか確認
			if( CommIsTimingSync(DBC_TIM_ROULETTE_INIT_5) == TRUE ){
				CommToolTempDataReset();
				wk->sub_seq++;
			}
		}else{
			wk->sub_seq++;
		}
		break;

	//パネル選出中1
	case 2:
		Roulette_EvPanelChoice( wk );
		wk->sub_seq++;
		break;

	//パネル選出中2
	case 3:
		Roulette_EvPanelChoice2( wk );
		wk->sub_seq++;
		break;

	//パネル選出中3
	case 4:
		Roulette_EvPanelChoice3( wk );
		wk->sub_seq++;
		break;

	//同期開始
	case 5:
		if( Roulette_CommCheck(wk->type) == TRUE ){
			CommToolTempDataReset();
			CommTimingSyncStart( DBC_TIM_ROULETTE_PANEL );
			wk->sub_seq++;
		}else{
			wk->sub_seq++;
		}
		break;

	//同期待ち
	case 6:
		if( Roulette_CommCheck(wk->type) == TRUE ){
			//タイミングコマンドが届いたか確認
			if( CommIsTimingSync(DBC_TIM_ROULETTE_PANEL) == TRUE ){
				CommToolTempDataReset();
				wk->sub_seq++;
			}
		}else{
			wk->sub_seq++;
		}
		break;

	//基本情報送信
	case 7:
		if( Roulette_CommCheck(wk->type) == TRUE ){
			if( Roulette_CommSetSendBuf(wk,ROULETTE_COMM_RANK_PAIR,0) == TRUE ){
				wk->sub_seq++;
			}
		}else{
			WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
							WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC*3, HEAPID_ROULETTE );
			wk->sub_seq++;
		}
		break;

	//受信待ち、ブラックイン
	case 8:
		if( Roulette_CommCheck(wk->type) == TRUE ){
			if( wk->recieve_count >= ROULETTE_COMM_PLAYER_NUM ){
				wk->recieve_count = 0;

				WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
								WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC*3, HEAPID_ROULETTE );
				wk->sub_seq++;
			}
		}else{
			wk->sub_seq++;
		}
		break;
		
	//フェード終了待ち
	case 9:
		if( WIPE_SYS_EndCheck() == TRUE ){
			return TRUE;
		}
		break;
	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	カーソル移動中
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameCsrMove( ROULETTE_WORK* wk )
{
	u16 x,y;
	u16 now_cp;
	int i;
	u32 ret,color;
	u16 buf16[4];

	enum{
		SEQ_SUB_START = 0,
		SEQ_SUB_START2,
		SEQ_SUB_START_SUB,
		SEQ_SUB_COUNT_3,
		SEQ_SUB_COUNT_2,
		SEQ_SUB_COUNT_1,
		SEQ_SUB_RANKUP_SEL,
		SEQ_SUB_CSR_STOP,
		SEQ_SUB_PANEL_WAIT,
	};

	switch( wk->sub_seq ){

	case SEQ_SUB_START:

#if 0
		PaletteFadeReq( wk->pfd, PF_BIT_MAIN_BG, 0xffff, 1, 
						0,									//開始濃度
						16,									//最終濃度 
						0x0000 );							//変更後の色
		SoftFadePfd( wk->pfd, FADE_MAIN_BG, 0, 16*13, 16, 0x0000 );
#endif

		wk->wait = 0;
		wk->csr_pos = wk->start_csr_pos;
		CsrPosSet( wk, wk->csr_pos );
		wk->sub_seq = SEQ_SUB_START2;
		break;

	case SEQ_SUB_START2:
		wk->wait++;
		if( wk->wait >= ROULETTE_START_WAIT ){
			wk->wait = 0;
			wk->sub_seq = SEQ_SUB_START_SUB;
		}
		break;

	case SEQ_SUB_START_SUB:
		//カウント3
		for( i=0; i < ROULETTE_PANEL_MAX ;i++ ){
			if( wk->p_panel[i] != NULL ){
				RouletteObj_AnmChg( wk->p_panel[i], ROULETTE_ANM_COUNT_3 );
				RouletteObj_Vanish( wk->p_panel[i], ROULETTE_VANISH_OFF );			//表示
			}
		}

		Roulette_SetMain2BgGraphic( wk, BR_FRAME_BG );	//上画面パネルを表示する時の背景
		Snd_SePlay( SEQ_SE_DP_WIN_OPEN2 );
		wk->wait = ROULETTE_COUNT_321_WAIT;
		wk->sub_seq = SEQ_SUB_COUNT_3;
		break;

	case SEQ_SUB_COUNT_3:
		wk->wait--;
		if( wk->wait > 0 ){
			break;
		}

		//カウント2
		for( i=0; i < ROULETTE_PANEL_MAX ;i++ ){
			if( wk->p_panel[i] != NULL ){
				RouletteObj_AnmChg( wk->p_panel[i], ROULETTE_ANM_COUNT_2 );
			}
		}

		Snd_SePlay( SEQ_SE_DP_WIN_OPEN2 );
		wk->wait = ROULETTE_COUNT_321_WAIT;
		wk->sub_seq = SEQ_SUB_COUNT_2;
		break;

	case SEQ_SUB_COUNT_2:
		wk->wait--;
		if( wk->wait > 0 ){
			break;
		}

		//カウント1
		for( i=0; i < ROULETTE_PANEL_MAX ;i++ ){
			if( wk->p_panel[i] != NULL ){
				RouletteObj_AnmChg( wk->p_panel[i], ROULETTE_ANM_COUNT_1 );
			}
		}

		Snd_SePlay( SEQ_SE_DP_WIN_OPEN2 );
		wk->wait = ROULETTE_COUNT_321_WAIT;
		wk->sub_seq = SEQ_SUB_COUNT_1;
		break;

	case SEQ_SUB_COUNT_1:
		wk->wait--;
		if( wk->wait > 0 ){
			break;
		}

		//パネルセット
		for( i=0; i < ROULETTE_PANEL_MAX ;i++ ){
			if( wk->p_panel[i] != NULL ){
				RouletteObj_AnmChg( wk->p_panel[i], wk->panel_no[i] );
			}
		}

		RouletteObj_Vanish( wk->p_csr, ROULETTE_VANISH_OFF );					//表示
		Snd_SePlay( SEQ_SE_DP_UG_020 );
		RouletteObj_AnmChg( wk->p_button, ROULETTE_ANM_BUTTON_STOP );
		wk->sub_seq = SEQ_SUB_RANKUP_SEL;
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//ランクアップを選択中
	case SEQ_SUB_RANKUP_SEL:
#ifdef PM_DEBUG
		if( sys.trg & PAD_BUTTON_L ){
			debug_panel_no++;
			if( debug_panel_no >= ROULETTE_EV_MAX ){
				debug_panel_no = ROULETTE_EV_ENEMY_HP_DOWN;
			}
			debug_panel_flag = 1;
			OS_Printf( "DEBUG panel_no = %d\n", debug_panel_no );
		}else if( sys.trg & PAD_BUTTON_R ){
			debug_panel_no+=10;
			if( debug_panel_no >= ROULETTE_EV_MAX ){
				debug_panel_no = ROULETTE_EV_ENEMY_HP_DOWN;
			}
			debug_panel_flag = 1;
			OS_Printf( "DEBUG panel_no = %d\n", debug_panel_no );
		}
#endif

		CsrMove( wk, sys.trg );

		//親のみタイマー減らす
		if( CommGetCurrentID() == COMM_PARENT_ID ){
			if( wk->game_timer > 0 ){
				wk->game_timer--;
			}

			if( wk->game_timer == 0 ){
				//Snd_SePlay( SEQ_SE_DP_PINPON );	//パネル決定音で上書きされるのでいらない！
				OS_Printf( "wk->game_timer = 0になりました！\n" );
			}
		}
		
		//下画面のボタンを押した時、タイマーが0になった時、Aボタンを押した時
		//if( Roulette_CheckButtonPush(wk) == TRUE ){
		if( (Roulette_CheckButtonPush(wk) == TRUE) || (wk->game_timer == 0) ){

			//すべりを足した値を決定する
			Roulette_SetCsrPosSuberi( wk );
			//RouletteObj_Vanish( wk->p_csr, ROULETTE_VANISH_ON );					//非表示

			if( Roulette_CommCheck(wk->type) == FALSE ){
				wk->sub_seq = SEQ_SUB_CSR_STOP;
				break;
			}else{
				wk->rankup_req		= 1;
				return TRUE;
			}
		}
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//カーソル停止処理
	case SEQ_SUB_CSR_STOP:
		Roulette_SeqSubPanelDecide( wk, wk->csr_pos );
		wk->wait = ROULETTE_PANEL_WAIT;
		wk->sub_seq = SEQ_SUB_PANEL_WAIT;
		break;

	//////////////////////////////////////////////////////////////////////////////////////
	//パネルウェイト処理
	case SEQ_SUB_PANEL_WAIT:
		wk->wait--;
		if( wk->wait == 0 ){
			return TRUE;
		}
		break;

	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	送受信
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameSendRecv( ROULETTE_WORK* wk )
{
	switch( wk->sub_seq ){

	case 0:
		//選んだリクエスト(やりたいこと)を送信
		if( Roulette_CommSetSendBuf(wk,ROULETTE_COMM_RANK_UP_TYPE, wk->suberi_csr_pos) == TRUE ){
			//Snd_SeStopBySeqNo( SEQ_SE_DP_SELECT, 0 );
			//Snd_SePlay( SEQ_SE_DP_BUTTON9 );
			//RouletteObj_AnmChg( wk->p_button, ROULETTE_ANM_BUTTON_PUSH );
			wk->rankup_req = 0;
			wk->sub_seq++;
		}
		break;

	case 1:
		wk->sub_seq++;
		break;

	case 2:
		if( wk->parent_decide_pos == ROULETTE_DECIDE_NONE ){
			break;
		}

		//親が決めたランクとLRをランクアップさせる
		wk->recieve_count = 0;
		Roulette_SeqSubPanelDecide( wk, wk->parent_decide_pos );
		wk->sub_seq++;
		break;

	case 3:
		//if( sys.trg & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL) ){
			//同期開始
			CommToolTempDataReset();
			CommTimingSyncStart( DBC_TIM_ROULETTE_UP );
			wk->sub_seq++;
			break;
		//}
		break;

	//同期
	case 4:
		//タイミングコマンドが届いたか確認
		if( CommIsTimingSync(DBC_TIM_ROULETTE_UP) == TRUE ){
			CommToolTempDataReset();
			CommToolInitialize( HEAPID_ROULETTE );	//timingSyncEnd=0xff
			wk->parent_decide_pos = ROULETTE_DECIDE_NONE;
			return TRUE;
		}
		break;

	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	シーケンス：マルチ終了へ
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameEndMulti( ROULETTE_WORK* wk )
{
	switch( wk->sub_seq ){

	//「戻る」送信
	case 0:
		wk->wait = ROULETTE_MODORU_WAIT;
		wk->sub_seq++;
		break;

	//同期開始
	case 1:
		if( wk->wait > 0 ){
			wk->wait--;
		}

		if( wk->wait == 0 ){
			CommToolTempDataReset();
			CommTimingSyncStart( DBC_TIM_ROULETTE_END );
			wk->sub_seq++;
		}
		break;

	//同期待ち
	case 2:
		//タイミングコマンドが届いたか確認
		if( CommIsTimingSync(DBC_TIM_ROULETTE_END) == TRUE ){
			CommToolTempDataReset();
			return TRUE;
		}
		break;
	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	シーケンス：終了へ
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameEnd( ROULETTE_WORK* wk )
{
	int i;

	switch( wk->sub_seq ){

	//フェードアウト
	case 0:
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
					WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_ROULETTE );

		wk->sub_seq++;
		break;

	//フェード終了待ち
	case 1:
		if( WIPE_SYS_EndCheck() == TRUE ){
			return TRUE;
		}
		break;
	};

	return FALSE;
}


//==============================================================================================
//
//	共通処理
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	共通 削除
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void RouletteCommon_Delete( ROULETTE_WORK* wk )
{
	u8 m_max;
	int i;

	if( wk->p_csr != NULL ){
		RouletteObj_Delete( wk->p_csr );
	}

	if( wk->p_button != NULL ){
		RouletteObj_Delete( wk->p_button );
	}

	for( i=0; i < ROULETTE_PANEL_MAX ;i++ ){
		if( wk->p_panel[i] != NULL ){
			RouletteObj_Delete( wk->p_panel[i] );
		}
	}

	m_max = Roulette_GetMinePokeNum( wk->type, ROULETTE_FLAG_TOTAL );
	for( i=0; i < m_max ;i++ ){	
		if( wk->p_m_icon[i] != NULL ){
			RouletteObj_Delete( wk->p_m_icon[i] );
		}
		if( wk->p_e_icon[i] != NULL ){
			RouletteObj_Delete( wk->p_e_icon[i] );
		}

		if( wk->p_m_itemkeep[i] != NULL ){
			RouletteObj_Delete( wk->p_m_itemkeep[i] );
		}
		if( wk->p_e_itemkeep[i] != NULL ){
			RouletteObj_Delete( wk->p_e_itemkeep[i] );
		}
	}

	//通信アイコン削除
	WirelessIconEasyEnd();

	//パレットフェード開放
	PaletteFadeWorkAllocFree( wk->pfd, FADE_MAIN_OBJ );
	PaletteFadeWorkAllocFree( wk->pfd, FADE_MAIN_BG );

	//パレットフェードシステム開放
	PaletteFadeFree( wk->pfd );
	wk->pfd = NULL;

	RouletteClact_DeleteCellObject(&wk->roulette_clact);		//2Dオブジェクト関連領域開放

	MSGMAN_Delete( wk->msgman );						//メッセージマネージャ開放
	WORDSET_Delete( wk->wordset );
	STRBUF_Delete( wk->msg_buf );						//メッセージバッファ開放
	STRBUF_Delete( wk->tmp_buf );						//メッセージバッファ開放

	for( i=0; i < ROULETTE_MENU_BUF_MAX ;i++ ){
		STRBUF_Delete( wk->menu_buf[i] );				//メニューバッファ開放
	}

	RouletteExitBmpWin( wk->bmpwin );					//BMPウィンドウ開放
	Roulette_BgExit( wk->bgl );							//BGL削除
	ArchiveDataHandleClose( wk->hdl );

	return;
}

//--------------------------------------------------------------
/**
 * @brief	共通 初期化1
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_InitSub1( void )
{
	sys_VBlankFuncChange( NULL, NULL );					//VBlankセット
	sys_HBlankIntrSet( NULL,NULL );						//HBlankセット
	GF_Disp_GX_VisibleControlInit();
	GF_Disp_GXS_VisibleControlInit();
	GX_SetVisiblePlane( 0 );
	GXS_SetVisiblePlane( 0 );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	共通 初期化2
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_InitSub2( ROULETTE_WORK* wk )
{
	u8 m_max;
	u16 x,y,offset_y,offset_y2;
	int i,flip;
	GF_BGL_BMPWIN* win;
	POKEMON_PARAM* poke;

	wk->hdl = ArchiveDataHandleOpen( ARC_FRONTIER_BG, HEAPID_ROULETTE );
	Roulette_BgInit( wk );								//BG初期化
	Roulette_ObjInit( wk );								//OBJ初期化

	//メッセージデータマネージャー作成
	wk->msgman = MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, 
								NARC_msg_roulette_dat, HEAPID_ROULETTE );

	wk->wordset = WORDSET_Create( HEAPID_ROULETTE );
	wk->msg_buf = STRBUF_Create( ROULETTE_MSG_BUF_SIZE, HEAPID_ROULETTE );
	wk->tmp_buf = STRBUF_Create( ROULETTE_MSG_BUF_SIZE, HEAPID_ROULETTE );

	//文字列バッファ作成
	for( i=0; i < ROULETTE_MENU_BUF_MAX ;i++ ){
		wk->menu_buf[i] = STRBUF_Create( ROULETTE_MENU_BUF_SIZE, HEAPID_ROULETTE );
	}

	//フォントパレット
	SystemFontPaletteLoad( PALTYPE_MAIN_BG, BR_FONT_PAL * 32, HEAPID_ROULETTE );
	TalkFontPaletteLoad( PALTYPE_MAIN_BG, BR_MSGFONT_PAL * 32, HEAPID_ROULETTE );

	//ビットマップ追加
	RouletteAddBmpWin( wk->bgl, wk->bmpwin );

	//カーソルOBJ追加
	wk->p_csr = RouletteObj_Create(	&wk->roulette_clact, 
									ROULETTE_ID_CHAR_CSR, ROULETTE_ID_PLTT_CSR,
									ROULETTE_ID_CELL_CSR, 
									ROULETTE_ANM_CSR, 
									PANEL_START_X,
									PANEL_START_Y,
									0, ROULETTE_BG_PRI_LOW, DISP_MAIN );
	RouletteObj_Vanish( wk->p_csr, ROULETTE_VANISH_ON );					//非表示

	//パネルOBJ追加
	for( i=0; i < ROULETTE_PANEL_MAX ;i++ ){	
		wk->p_panel[i] = RouletteObj_Create(&wk->roulette_clact, 
										ROULETTE_ID_CHAR_CSR, ROULETTE_ID_PLTT_CSR,
										ROULETTE_ID_CELL_CSR, 
										ROULETTE_ANM_COUNT_3,
										PANEL_START_X + (PANEL_WIDTH_X*(i%ROULETTE_PANEL_H_MAX)),
										PANEL_START_Y + (PANEL_WIDTH_Y*(i/ROULETTE_PANEL_H_MAX)),
										1, ROULETTE_BG_PRI_LOW, DISP_MAIN );
		RouletteObj_Vanish( wk->p_panel[i], ROULETTE_VANISH_ON );			//非表示
	}

	//アイコンOBJ追加
	m_max = Roulette_GetMinePokeNum( wk->type, ROULETTE_FLAG_TOTAL );

	if( Roulette_CommCheck(wk->type) == FALSE ){
		offset_y  = ICON_START_Y;
		offset_y2 = ITEMKEEP_START_Y;
	}else{
		offset_y  = ICON_START_MULTI_Y;
		offset_y2 = ITEMKEEP_START_MULTI_Y;
	}

	for( i=0; i < m_max ;i++ ){	

		//アイテム持っているかアイコン
		wk->p_m_itemkeep[i] = RouletteObj_Create(	&wk->roulette_clact, 
												ROULETTE_ID_CHAR_ITEMKEEP, 
												ROULETTE_ID_PLTT_ITEMKEEP,
												ROULETTE_ID_CELL_ITEMKEEP, 
												0,
												ITEMKEEP_MINE_START_X,
												offset_y2 + (ITEMKEEP_WIDTH_Y*i),
												0, ROULETTE_BG_PRI_LOW, DISP_MAIN );
		//RouletteObj_SetScaleAffine( wk->p_m_itemkeep[i], 1 );	//1.5倍

		wk->p_e_itemkeep[i] = RouletteObj_Create(	&wk->roulette_clact, 
												ROULETTE_ID_CHAR_ITEMKEEP, 
												ROULETTE_ID_PLTT_ITEMKEEP,
												ROULETTE_ID_CELL_ITEMKEEP, 
												0,
												ITEMKEEP_ENEMY_START_X,
												offset_y2 + (ITEMKEEP_WIDTH_Y*i),
												0, ROULETTE_BG_PRI_LOW, DISP_MAIN );
		//RouletteObj_SetScaleAffine( wk->p_e_itemkeep[i], 1 );	//1.5倍

		//ポケモンアイコン
		wk->p_m_icon[i] = RouletteObj_Create(	&wk->roulette_clact, 
												ROULETTE_ID_CHAR_ICON1+i, ROULETTE_ID_PLTT_ICON,
												ROULETTE_ID_CELL_ICON, 
												POKEICON_ANM_HPMAX,
												ICON_MINE_START_X,
												offset_y + (ICON_WIDTH_Y*i),
												1, ROULETTE_BG_PRI_LOW, DISP_MAIN );
		//RouletteObj_SetScaleAffine( wk->p_m_icon[i], 1 );	//1.5倍

		wk->p_e_icon[i] = RouletteObj_Create(	&wk->roulette_clact, 
												ROULETTE_ID_CHAR_ICON5+i, ROULETTE_ID_PLTT_ICON,
												ROULETTE_ID_CELL_ICON, 
												POKEICON_ANM_HPMAX,
												ICON_ENEMY_START_X,
												offset_y + (ICON_WIDTH_Y*i),
												1, ROULETTE_BG_PRI_LOW, DISP_MAIN );
		//RouletteObj_SetScaleAffine( wk->p_e_icon[i], 1 );	//1.5倍

		//パレット切り替え
		RouletteObj_IconPalChg( wk->p_m_icon[i], PokeParty_GetMemberPointer(wk->p_m_party,i) );
		RouletteObj_IconPalChg( wk->p_e_icon[i], PokeParty_GetMemberPointer(wk->p_e_party,i) );

		//オートアニメなし
		RouletteObj_SetAnmFlag( wk->p_m_icon[i], 0 );
		RouletteObj_SetAnmFlag( wk->p_e_icon[i], 0 );
	}

	//アイテム持っているかアイコンのバニッシュセット
	Roulette_ItemKeepVanishSet( wk );

	//ボタン
	wk->p_button = RouletteObj_Create(	&wk->roulette_clact, 
										ROULETTE_ID_CHAR_BUTTON, ROULETTE_ID_PLTT_BUTTON,
										ROULETTE_ID_CELL_BUTTON, 
										ROULETTE_ANM_BUTTON_START, 
										BUTTON_START_X,
										BUTTON_START_Y,
										0, ROULETTE_BG_PRI_HIGH, DISP_SUB );

	//通信アイコンセット
	if( CommIsInitialize() ){
//CLACT_U_WmIcon_SetReserveAreaCharManager( NNS_G2D_VRAM_TYPE_2DMAIN,GX_OBJVRAMMODE_CHAR_1D_64K );
		CLACT_U_WmIcon_SetReserveAreaCharManager( NNS_G2D_VRAM_TYPE_2DMAIN,
												  GX_OBJVRAMMODE_CHAR_1D_32K );
		CLACT_U_WmIcon_SetReserveAreaPlttManager( NNS_G2D_VRAM_TYPE_2DMAIN );
		WirelessIconEasy();
	}

	sys_VBlankFuncChange( VBlankFunc, (void*)wk );		//VBlankセット
	return;
}


//==============================================================================================
//
//	共通初期化、終了
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	BG関連初期化
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_BgInit( ROULETTE_WORK* wk )
{
	SetVramBank();
	SetBgHeader( wk->bgl );

	//パレットフェードシステムワーク作成
	wk->pfd = PaletteFadeInit( HEAPID_ROULETTE );

	//リクエストデータをmallocしてセット
	PaletteFadeWorkAllocSet( wk->pfd, FADE_MAIN_OBJ, FADE_PAL_ALL_SIZE, HEAPID_ROULETTE );
	PaletteFadeWorkAllocSet( wk->pfd, FADE_MAIN_BG, FADE_PAL_ALL_SIZE, HEAPID_ROULETTE );

	//上画面
	Roulette_SetMainBgGraphic( wk, BR_FRAME_BG );					//背景
	//Roulette_SetPanelEffBgGraphic( wk, BR_FRAME_EFF, 0 );			//パネルエフェクト
	Roulette_SetMainBgPalette();
	//GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );		//BG非表示

	//下画面背景、パレットセット
	Roulette_SetSubBgGraphic( wk, BR_FRAME_SUB );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	OBJ関連初期化
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_ObjInit( ROULETTE_WORK* wk )
{
	//RouletteClact_InitCellActor( &wk->roulette_clact );
	RouletteClact_InitCellActor(&wk->roulette_clact, 
								wk->p_m_party, wk->p_e_party, 
								Roulette_CommCheck(wk->type) );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	BG解放
 *
 * @param	ini		BGLデータ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_BgExit( GF_BGL_INI * ini )
{
	//メイン画面の各面の表示コントロール(表示OFF)
	GF_Disp_GX_VisibleControl(	GX_PLANEMASK_BG0 | 
								GX_PLANEMASK_BG1 | 
								GX_PLANEMASK_BG2 |
								GX_PLANEMASK_BG3 | 
								GX_PLANEMASK_OBJ, 
								VISIBLE_OFF );

	//サブ画面の各面の表示コントロール(表示OFF)
	GF_Disp_GXS_VisibleControl(	GX_PLANEMASK_BG0 | 
								GX_PLANEMASK_BG1 | 
								GX_PLANEMASK_BG2 | 
								GX_PLANEMASK_BG3 | 
								GX_PLANEMASK_OBJ, 
								VISIBLE_OFF );

	//GF_BGL_BGControlSetで取得したメモリを開放
	GF_BGL_BGControlExit( ini, BR_FRAME_BG );			//3
	//GF_BGL_BGControlExit( ini, BR_FRAME_EFF );
	//GF_BGL_BGControlExit( ini, BR_FRAME_TYPE );
	GF_BGL_BGControlExit( ini, BR_FRAME_WIN );
	GF_BGL_BGControlExit( ini, BR_FRAME_SUB );

	sys_FreeMemoryEz( ini );
	return;
}


//==============================================================================================
//
//	設定
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	VBlank関数
 *
 * @param	work	ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------
static void VBlankFunc( void * work )
{
	ROULETTE_WORK* wk = work;

	//パレット転送
	if( wk->pfd != NULL ){
		PaletteFadeTrans( wk->pfd );
	}

	GF_BGL_VBlankFunc( wk->bgl );

	//セルアクター
	//Vram転送マネージャー実行
	DoVramTransferManager();

	//レンダラ共有OAMマネージャVram転送
	REND_OAMTrans();	

	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}

//--------------------------------------------------------------
/**
 * @brief	VRAM設定
 *
 * @param	none
 *
 * @return	none
 *
 * 細かく設定していないので注意！
 */
//--------------------------------------------------------------
static void SetVramBank(void)
{
	GF_BGL_DISPVRAM tbl = {
		GX_VRAM_BG_128_C,				//メイン2DエンジンのBG
		GX_VRAM_BGEXTPLTT_NONE,			//メイン2DエンジンのBG拡張パレット

		GX_VRAM_SUB_BG_32_H,			//サブ2DエンジンのBG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		//サブ2DエンジンのBG拡張パレット

		GX_VRAM_OBJ_64_E,				//メイン2DエンジンのOBJ
		GX_VRAM_OBJEXTPLTT_NONE,		//メイン2DエンジンのOBJ拡張パレット

		GX_VRAM_SUB_OBJ_16_I,			//サブ2DエンジンのOBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	//サブ2DエンジンのOBJ拡張パレット

		GX_VRAM_TEX_01_AB,				//テクスチャイメージスロット
		GX_VRAM_TEXPLTT_01_FG			//テクスチャパレットスロット
	};

	GF_Disp_SetBank( &tbl );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	BG設定
 *
 * @param	init	BGLデータ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void SetBgHeader( GF_BGL_INI * ini )
{
	{	//BG SYSTEM
		GF_BGL_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
		};
		GF_BGL_InitBG( &BGsys_data );
	}

	{	//BG(フォント)(会話、メニュー)
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, BR_FRAME_WIN, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ClearCharSet( BR_FRAME_WIN, 32, 0, HEAPID_ROULETTE );
		GF_BGL_ScrClear( ini, BR_FRAME_WIN );
	}

#if 0
	{	//上画面(パネルエフェクト)
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			//GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000, GX_BG_EXTPLTT_01,
			//GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x0c000, GX_BG_EXTPLTT_01,
			//GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_01,
			GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x0c000, GX_BG_EXTPLTT_01,
			1, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, BR_FRAME_EFF, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( ini, BR_FRAME_EFF );
	}
#endif

	{	//上画面(SINGLE,DOUBLE)
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			//GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_01,
			GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x0c000, GX_BG_EXTPLTT_01,
			2, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, BR_FRAME_BG, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( ini, BR_FRAME_BG );
	}

	//--------------------------------------------------------------------------------
	{	//下画面(ボール)
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x3000, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, BR_FRAME_SUB, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( ini, BR_FRAME_SUB );
	}

	G2_SetBG0Priority( 0 );
	//GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );		//非表示
	return;
}


//==============================================================================================
//
//	BGグラフィックデータ
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	グラフィックデータセット：上画面背景
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_SetMainBgGraphic( ROULETTE_WORK * wk, u32 frm )
{
	u32 scrn;

	ArcUtil_HDL_BgCharSet(	wk->hdl, BR_PANEL_NCGR_BIN, 
							wk->bgl, frm, 0, 0, 1, HEAPID_ROULETTE );

	if( Roulette_CommCheck(wk->type) == FALSE ){
		scrn = BR_PANEL_NSCR_BIN;
	}else{
		scrn = BR_PANEL_MUL_NSCR_BIN;
	}
	ArcUtil_HDL_ScrnSet(wk->hdl, scrn, wk->bgl, frm, 0, 0, 1, HEAPID_ROULETTE );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	グラフィックデータセット：上画面パネルOBJを表示する時の背景
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_SetMain2BgGraphic( ROULETTE_WORK * wk, u32 frm )
{
	u32 scrn;

	//ArcUtil_HDL_BgCharSet(	wk->hdl, BR_PANEL_NCGR_BIN, 
	//						wk->bgl, frm, 0, 0, 1, HEAPID_ROULETTE );

	if( Roulette_CommCheck(wk->type) == FALSE ){
		scrn = BR_PANEL2_NSCR_BIN;
	}else{
		scrn = BR_PANEL_MUL2_NSCR_BIN;
	}
	ArcUtil_HDL_ScrnSet(wk->hdl, scrn, wk->bgl, frm, 0, 0, 1, HEAPID_ROULETTE );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	グラフィックデータセット：上画面パネルエフェクト
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 * @param	frm		フレームナンバー
 * @param	flag	0=char,scrnセット、1=scrnセット、2=scrn2セット
 *
 * @return	none
 */
//--------------------------------------------------------------
#if 0
static void Roulette_SetPanelEffBgGraphic( ROULETTE_WORK * wk, u32 frm, u8 flag )
{
	u32 scrn;

	if( Roulette_CommCheck(wk->type) == FALSE ){
		if( flag == 2 ){
			scrn = BR_PANEL_ALPHA2_NSCR_BIN;
		}else{
			scrn = BR_PANEL_ALPHA_NSCR_BIN;
		}
	}else{
		if( flag == 2 ){
			scrn = BR_PANEL_ALPHA2_MUL_NSCR_BIN;
		}else{
			scrn = BR_PANEL_ALPHA_MUL_NSCR_BIN;
		}
	}

	if( flag == 0 ){
		ArcUtil_HDL_BgCharSet( wk->hdl, BR_PANEL_NCGR_BIN, wk->bgl, frm, 0, 0, 1, HEAPID_ROULETTE );
	}

	ArcUtil_HDL_ScrnSet( wk->hdl, scrn, wk->bgl, frm, 0, 0, 1, HEAPID_ROULETTE );

#if 1
	//ブレンド設定
	G2_SetBlendAlpha(	ROULETTE_BLD_PLANE_1, ROULETTE_BLD_PLANE_2, 
						ROULETTE_BLD_ALPHA_1, ROULETTE_BLD_ALPHA_2 );
#endif

	return;
}
#endif

//--------------------------------------------------------------
/**
 * @brief	上画面背景パレット設定
 *
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_SetMainBgPalette( void )
{
	void *buf;
	NNSG2dPaletteData *dat;

	//buf = ArcUtil_PalDataGet( ARC_FRONTIER_BG, BR_RANK_NCLR, &dat, HEAPID_ROULETTE );
	buf = ArcUtil_PalDataGet( ARC_FRONTIER_BG, BR_PANEL_NCLR, &dat, HEAPID_ROULETTE );

	DC_FlushRange( dat->pRawData, (sizeof(u16)*16*7) );
	GX_LoadBGPltt( dat->pRawData, 0, (sizeof(u16)*16*7) );		//メイン

	sys_FreeMemoryEz(buf);
	return;
}

//--------------------------------------------------------------
/**
 * @brief	グラフィックデータセット：下画面背景、パレットセット
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_SetSubBgGraphic( ROULETTE_WORK * wk, u32 frm  )
{
	ArcUtil_HDL_BgCharSet(	wk->hdl, BR_BUTTON_BG_NCGR_BIN, wk->bgl, frm,
							0, 0, TRUE, HEAPID_ROULETTE );

	ArcUtil_HDL_PalSet(	wk->hdl, BR_BUTTON_BG_NCLR, PALTYPE_SUB_BG,
						0, (sizeof(u16)*16*2), HEAPID_ROULETTE );
	
	ArcUtil_HDL_ScrnSet(wk->hdl, BR_BUTTON_BG_NSCR_BIN, wk->bgl, frm,
						0, 0, TRUE, HEAPID_ROULETTE );
	return;
}


//==============================================================================================
//
//	メッセージ関連
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	メッセージ表示
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 * @param	win		ビットマップウィンドウ
 * @param	msg_id	メッセージID
 * @param	x		X座標(ドット単位)
 * @param	y		Y座標(ドット単位)
 * @param	wait	文字表示ウェイト
 * @param	f_col	文字色ナンバー(塗りつぶしカラーコード)
 * @param	s_col	影色ナンバー
 * @param	b_col	背景色ナンバー
 * @param	font	フォント種類
 *
 * @return	"文字描画ルーチンのインデックス"
 */
//--------------------------------------------------------------
static u8 RouletteWriteMsg( ROULETTE_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font )
{
	GF_BGL_BmpWinDataFill( win, b_col );			//塗りつぶし
	MSGMAN_GetString( wk->msgman, msg_id, wk->tmp_buf );

	//登録された単語を使って文字列展開する
	WORDSET_ExpandStr( wk->wordset, wk->msg_buf, wk->tmp_buf );

	return GF_STR_PrintColor( win, font, wk->msg_buf, x, y, wait, 
								GF_PRINTCOLOR_MAKE(f_col,s_col,b_col), NULL );
}

//--------------------------------------------------------------
/**
 * @brief	メッセージ表示(塗りつぶしなし)
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 * @param	win		ビットマップウィンドウ
 * @param	msg_id	メッセージID
 * @param	x		X座標(ドット単位)
 * @param	y		Y座標(ドット単位)
 * @param	wait	文字表示ウェイト
 * @param	f_col	文字色ナンバー(塗りつぶしカラーコード)
 * @param	s_col	影色ナンバー
 * @param	b_col	背景色ナンバー
 * @param	font	フォント種類
 *
 * @return	"文字描画ルーチンのインデックス"
 *
 * 塗りつぶしなし
 */
//--------------------------------------------------------------
static u8 RouletteWriteMsgSimple( ROULETTE_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font )
{
	MSGMAN_GetString( wk->msgman, msg_id, wk->tmp_buf );

	//登録された単語を使って文字列展開する
	WORDSET_ExpandStr( wk->wordset, wk->msg_buf, wk->tmp_buf );

	return GF_STR_PrintColor( win, font, wk->msg_buf, x, y, wait, 
								GF_PRINTCOLOR_MAKE(f_col,s_col,b_col), NULL );
}

//--------------------------------------------------------------
/**
 * @brief	簡単メッセージ表示
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 * @param	msg_id	メッセージID
 *
 * @return	"msg_index"
 */
//--------------------------------------------------------------
static u8 Roulette_EasyMsg( ROULETTE_WORK* wk, int msg_id )
{
	u8 msg_index;

	msg_index = RouletteWriteMsg(	wk, &wk->bmpwin[RANK_BMPWIN_TALK], msg_id, 
								1, 1, MSG_ALLPUT, 
								//1, 1, CONFIG_GetMsgSpeed(wk->config), 
								FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE, BR_FONT );

	GF_BGL_BmpWinOn( &wk->bmpwin[RANK_BMPWIN_TALK] );
	return msg_index;
}


//==============================================================================================
//
//	メニュー関連
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	BMPメニュー	ヘッダー初期設定
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 * @param	win		GF_BGL_BMPWIN型のポインタ
 * @param	y_max	項目最大数
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void RouletteInitMenu( ROULETTE_WORK* wk, GF_BGL_BMPWIN* win, u8 y_max )
{
	int i;

	for( i=0; i < ROULETTE_MENU_BUF_MAX ;i++ ){
		wk->Data[i].str	  = NULL;
		wk->Data[i].param = 0;
	}

	wk->MenuH.menu		= wk->Data;
	wk->MenuH.win		= win;
	wk->MenuH.font		= BR_FONT;
	wk->MenuH.x_max		= 1;
	wk->MenuH.y_max		= y_max;
	wk->MenuH.line_spc	= 0;
	wk->MenuH.c_disp_f	= 0;
	//wk->MenuH.c_disp_f	= 1;			//カーソルなし
	//wk->MenuH.loop_f	= 0;			//ループ無し
	wk->MenuH.loop_f	= 1;			//ループ有り
	return;
}

//--------------------------------------------------------------
/**
 * @brief	BMPメニュー	データセット
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 * @param	no		セットする場所(戻り値)
 * @param	param	戻り値
 * @param	msg_id	メッセージID
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void RouletteSetMenuData( ROULETTE_WORK* wk, u8 no, u8 param, int msg_id )
{
	int i;
	void* msg;

	SDK_ASSERTMSG( no < ROULETTE_MENU_BUF_MAX, "メニュー項目数オーバー！" );

	MSGMAN_GetString( wk->msgman, msg_id, wk->menu_buf[no] );

	wk->Data[ no ].str = (const void *)wk->menu_buf[no];
	wk->Data[ no ].param = param;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	共通 メニューセット2「はい、いいえ」
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_SetMenu2( ROULETTE_WORK* wk )
{
	RouletteWriteMenuWin( wk->bgl, &wk->bmpwin[RANK_BMPWIN_YESNO] );
	RouletteInitMenu( wk, &wk->bmpwin[RANK_BMPWIN_YESNO], 2 );
	RouletteSetMenuData( wk, 0, 0, msg_roulette_02_01 );				//はい
	RouletteSetMenuData( wk, 1, 1, msg_roulette_02_02 );				//いいえ
	wk->mw = BmpMenuAddEx( &wk->MenuH, 8, 0, 0, HEAPID_ROULETTE, PAD_BUTTON_CANCEL );
	return;
}


//==============================================================================================
//
//	文字列セット
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	数値をセット
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 * @param	bufID	バッファID
 * @param	number	セットする数値
 *
 * @retval	none
 *
 * 桁を固定にしている
 */
//--------------------------------------------------------------
static void Roulette_SetNumber( ROULETTE_WORK* wk, u32 bufID, s32 number, u32 keta, NUMBER_DISPTYPE disp )
{
	WORDSET_RegisterNumber( wk->wordset, bufID, number, keta, disp, NUMBER_CODETYPE_DEFAULT );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ポケモン名をセット
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 * @param	bufID	バッファID
 * @param	ppp		POKEMON_PASO_PARAM
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void Roulette_SetPokeName( ROULETTE_WORK* wk, u32 bufID, POKEMON_PASO_PARAM* ppp )
{
	WORDSET_RegisterPokeMonsName( wk->wordset, bufID, ppp );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	プレイヤー名をセット
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 * @param	bufID	バッファID
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void Roulette_SetPlayerName( ROULETTE_WORK* wk, u32 bufID )
{
	WORDSET_RegisterPlayerName( wk->wordset, bufID, SaveData_GetMyStatus(wk->sv) );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	プレイヤー名を表示
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 * @param	win		ビットマップウィンドウ
 * @param	x		X座標(ドット単位)
 * @param	y		Y座標(ドット単位)
 * @param	f_col	文字色ナンバー(塗りつぶしカラーコード)
 * @param	s_col	影色ナンバー
 * @param	b_col	背景色ナンバー
 * @param	font	フォント種類
 *
 * @return	none
 */
//--------------------------------------------------------------
static void PlayerNameWrite( ROULETTE_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 font )
{
	u32 col;
	const MYSTATUS* my;
	STRBUF* player_buf;								//プレイヤー名バッファポインタ

	my = SaveData_GetMyStatus( wk->sv );
	player_buf = STRBUF_Create( PLAYER_NAME_BUF_SIZE, HEAPID_ROULETTE );
	
	//GF_BGL_BmpWinDataFill( win, FBMP_COL_NULL );			//塗りつぶし

	STRBUF_SetStringCode( player_buf, MyStatus_GetMyName(my) );

	if( MyStatus_GetMySex(my) == PM_MALE ){
		col = COL_BLUE;
	}else{
		col = COL_RED;
	}

	GF_STR_PrintColor( win, font, player_buf, x, y, MSG_ALLPUT, col, NULL );

	STRBUF_Delete( player_buf );					//メッセージバッファ開放

	//GF_BGL_BmpWinOn( win );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	パートナー名を表示
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 * @param	win		ビットマップウィンドウ
 * @param	x		X座標(ドット単位)
 * @param	y		Y座標(ドット単位)
 * @param	f_col	文字色ナンバー(塗りつぶしカラーコード)
 * @param	s_col	影色ナンバー
 * @param	b_col	背景色ナンバー
 * @param	font	フォント種類
 *
 * @return	none
 */
//--------------------------------------------------------------
static void PairNameWrite( ROULETTE_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 font )
{
	MYSTATUS* my;									//パートナーのMyStatus
	u32 col;

	//パートナーのMyStatusを取得
	my = CommInfoGetMyStatus( 1 - CommGetCurrentID() );

	if( MyStatus_GetMySex(my) == PM_MALE ){
		col = COL_BLUE;
	}else{
		col = COL_RED;
	}

	WORDSET_RegisterPlayerName( wk->wordset, 0, my );

	RouletteWriteMsgSimple(	wk, win, msg_roulette_name_02, 
							x, y, MSG_ALLPUT, 
							GF_PRINTCOLOR_GET_LETTER(col),
							GF_PRINTCOLOR_GET_SHADOW(col),
							GF_PRINTCOLOR_GET_GROUND(col),
							//FBMP_COL_NULL,
							BR_FONT );
								
	//GF_BGL_BmpWinOn( win );
	return;
}


//==============================================================================================
//
//	ツール
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	現在のtypeチェック関数
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 * @param	type	チェックするタイプ
 *
 * @return	"TRUE = typeが同じ、FALSE = typeが違う"
 */
//--------------------------------------------------------------
static BOOL Roulette_CheckType( ROULETTE_WORK* wk, u8 type )
{
	if( wk->type == type ){
		return TRUE;
	}

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	シーケンス変更
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 * @param	seq		シーケンスのポインタ
 * @param	next	次のシーケンス定義
 *
 * @return	none
 */
//--------------------------------------------------------------
static void NextSeq( ROULETTE_WORK* wk, int* seq, int next )
{
	wk->sub_seq = 0;
	*seq		= next;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	キーチェック
 *
 * @param	key		チェックするキー
 *
 * @return	"結果"
 */
//--------------------------------------------------------------
static int KeyCheck( int key )
{
	return (sys.cont & key);
}

//--------------------------------------------------------------
/**
 * @brief	BGチェック
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void BgCheck( ROULETTE_WORK* wk )
{
	if( sys.cont & PAD_KEY_UP ){
		if( sys.cont & PAD_BUTTON_SELECT ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
		}else if( sys.cont & PAD_BUTTON_START ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );
		}
	}else if( sys.cont & PAD_KEY_DOWN ){
		if( sys.cont & PAD_BUTTON_SELECT ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
		}else if( sys.cont & PAD_BUTTON_START ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
		}
	}else if( sys.cont & PAD_KEY_LEFT ){
		if( sys.cont & PAD_BUTTON_SELECT ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
		}else if( sys.cont & PAD_BUTTON_START ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
		}
	}else if( sys.cont & PAD_KEY_RIGHT ){
		if( sys.cont & PAD_BUTTON_SELECT ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
			//GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
		}else if( sys.cont & PAD_BUTTON_START ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
			//GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
		}
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	十字キー操作
 *
 * @param	key		チェックするキー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void CsrMove( ROULETTE_WORK* wk, int key )
{
	u8 level;

	level = *wk->p_csr_speed_level;

	//滑りなし
	wk->csr_wait++;

	if( wk->csr_wait >= csr_wait_tbl[level][0] ){
		wk->csr_wait = 0;

		if( wk->random_flag == 1 ){		//カーソルがランダムの時
			wk->csr_pos = ( gf_rand() % ROULETTE_PANEL_MAX );
		}else{
			wk->csr_pos++;
		}

		//Snd_SePlay( SEQ_SE_DP_SELECT );
		Snd_SePlay( SEQ_SE_DP_BUTTON3 );

		if( wk->csr_pos >= wk->hv_max ){
			wk->csr_pos = 0;
		}

		CsrPosSet( wk, wk->csr_pos );
	}

	return;
}

static void CsrPosSet( ROULETTE_WORK* wk, u8 csr_pos )
{
	RouletteObj_SetObjPos(	wk->p_csr,
							PANEL_START_X + (PANEL_WIDTH_X*(csr_pos%ROULETTE_PANEL_H_MAX)),
							PANEL_START_Y + (PANEL_WIDTH_Y*(csr_pos/ROULETTE_PANEL_H_MAX)));
	return;
}

//--------------------------------------------------------------
/**
 * @brief	カーソルＸ取得
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 *
 * @return	"X"
 */
//--------------------------------------------------------------
static u16 GetCsrX( ROULETTE_WORK* wk, u8 csr_pos, u8 lr )
{
	u16 x,y,offset_x,offset_y,pair_offset_x,pair_offset_y,lr_offset_x;

	if( lr == 0 ){						//左
		lr_offset_x = SEL_CSR_L_OFFSET;
	}else{								//右
		lr_offset_x = SEL_CSR_R_OFFSET;
	}

	if( csr_pos >= (wk->hv_max-1) ){
		return (CSR_MODORU_X + lr_offset_x);
	}

	Roulette_GetOffset( wk, &offset_x, &offset_y, &pair_offset_x, &pair_offset_y );

	if( Roulette_CommCheck(wk->type) == TRUE ){
		if( (csr_pos % wk->h_max) == 0 ){
			//OS_Printf( "GetCsrX = %d\n", (lr_offset_x + offset_x) );
			return ( lr_offset_x + offset_x );
		}else{
			//OS_Printf( "GetCsrX = %d\n", (lr_offset_x + pair_offset_x) );
			return ( lr_offset_x + pair_offset_x );
		}
	}

	//OS_Printf( "GetCsrX = %d\n", (lr_offset_x + offset_x) );
	return ( lr_offset_x + offset_x );
}

//--------------------------------------------------------------
/**
 * @brief	カーソルＹ取得
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 *
 * @return	"Y"
 */
//--------------------------------------------------------------
//カーソルＹ座標(ランクアップ項目ごと)
static const u16 csr_y_tbl[] = {
	72, 104, 136, 180,
};

static u16 GetCsrY( ROULETTE_WORK* wk, u8 csr_pos )
{
	if( csr_pos >= (wk->hv_max-1) ){
		return CSR_MODORU_Y;
	}

	//OS_Printf( "GetCsrY = %d\n", csr_y_tbl[csr_pos / wk->h_max] );
	return csr_y_tbl[csr_pos / wk->h_max];
}

//--------------------------------------------------------------
/**
 * @brief	オフセット取得
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_GetOffset( ROULETTE_WORK* wk, u16* offset_x, u16* offset_y, u16* pair_offset_x, u16* pair_offset_y )
{
	//通信しているかで表示のオフセットが変わる
	if( Roulette_CommCheck(wk->type) == FALSE ){
		*offset_x		= INFO_SINGLE_OFFSET_X;
		*offset_y		= 0;
		*pair_offset_x	= 0;
		*pair_offset_y	= 0;
	}else{
		*offset_x		= INFO_MULTI_L_OFFSET_X;
		*offset_y		= 0;
		*pair_offset_x	= INFO_MULTI_R_OFFSET_X;
		*pair_offset_y	= 0;
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	すべりを足した値を決定する(無効)
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_SetCsrPosSuberi( ROULETTE_WORK* wk )
{
	wk->suberi_csr_pos = wk->csr_pos;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	通信の選んだカーソル位置を取得
 *
 * @param	hv_max	縦横の数
 * @param	csr_pos	カーソル位置
 *
 * @return	"カーソル位置"
 */
//--------------------------------------------------------------
static u8 GetCommSelCsrPos( u8 type_offset, u8 decide_type )
{
	//オフセットを外す
	if( decide_type < type_offset ){
		return decide_type;
	}

	return (decide_type - type_offset);
}

//--------------------------------------------------------------
/**
 * @brief	"親子"の連勝数を比較して、使用する連勝数を取得
 *
 * @param	wk		ROULETTE_SCRWORK型のポインタ
 *
 * @return	"連勝数"
 */
//--------------------------------------------------------------
static u16 Roulette_CommGetRensyou( ROULETTE_WORK* wk )
{
	u16	rensyou;
	
	rensyou = wk->rensyou;

	if( Roulette_CommCheck(wk->type) == TRUE ){

		//通信時には周回数の多いほうで抽選
		if( wk->pair_rensyou > wk->rensyou ){
			rensyou = wk->pair_rensyou;
		}
	}

	OS_Printf( "wk->rensyou = %d\n", wk->rensyou );
	OS_Printf( "wk->pair_rensyou = %d\n", wk->pair_rensyou );
	OS_Printf( "rensyou = %d\n", rensyou );
	return rensyou;
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
 * @param	wk			ROULETTE_WORK型のポインタ
 * @param	type		送信タイプ
 *
 * @retval	none
 */
//--------------------------------------------------------------
BOOL Roulette_CommSetSendBuf( ROULETTE_WORK* wk, u16 type, u16 param )
{
	int ret,command;

	switch( type ){

	//名前
	case ROULETTE_COMM_RANK_PAIR:
		command = FC_ROULETTE_PAIR;
		Roulette_CommSendBufBasicData( wk, type );
		break;

	//ランクアップしたいリクエスト
	case ROULETTE_COMM_RANK_UP_TYPE:
		command = FC_ROULETTE_UP_TYPE;
		Roulette_CommSendBufRankUpType( wk, type, param );
		break;

	//カーソル位置
	case ROULETTE_COMM_RANK_CSR_POS:
		command = FC_ROULETTE_CSR_POS;
		Roulette_CommSendBufCsrPos( wk, type );
		break;

	};

	if( CommSendData(command,wk->send_buf,ROULETTE_COMM_BUF_LEN) == TRUE ){
		ret = TRUE;
	}else{
		ret = FALSE;
	}

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief	send_bufに基本情報をセット
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void Roulette_CommSendBufBasicData( ROULETTE_WORK* wk, u16 type )
{
	int i,num;
	MYSTATUS* my;
	
	num = 0;
	my	= SaveData_GetMyStatus( wk->sv );

	wk->send_buf[num] = type;
	OS_Printf( "送信：type = %d\n", wk->send_buf[0] );
	num+=1;														//1

	//wk->send_buf[num] = wk->rensyou;
	//OS_Printf( "送信：rensyou = %d\n", wk->send_buf[1] );
	num+=1;														//2

	//パネル
	for( i=0; i < ROULETTE_PANEL_MAX ;i++ ){
		wk->send_buf[num+i] = wk->panel_no[i];
		OS_Printf( "送信：panel_no = %d\n", wk->send_buf[num+i] );
	}
	num+=ROULETTE_PANEL_MAX;									//18

	//開始カーソル位置
	wk->send_buf[num] = wk->start_csr_pos;						//19
	num+=1;

	return;
}

//--------------------------------------------------------------
/**
 * @brief   基本情報 通信受信処理
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		FRONTIER_SYSTEMへのポインタ
 */
//--------------------------------------------------------------
void Roulette_CommRecvBufBasicData(int id_no,int size,void *pData,void *work)
{
	int i,num;
	ROULETTE_WORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******キャッスル****** 基本情報受信\n" );

	num = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	//type = recv_buf[0];
	num+=1;													//1
	
	//wk->pair_rensyou = (u16)recv_buf[num];
	//OS_Printf( "受信：wk->pair_rensyou = %d\n", wk->pair_rensyou );
	num+=1;													//2

	//子のみ受け取る(ルーレット画面に渡されたlapは、すでに周回数多い方の値になっている)
	if( CommGetCurrentID() != COMM_PARENT_ID ){

		//パネル
		for( i=0; i < ROULETTE_PANEL_MAX ;i++ ){
			wk->panel_no[i] = (u8)recv_buf[num+i];
			OS_Printf( "受信：wk->panel_no[%d] = %d\n", i, wk->panel_no[i] );
		}
		num+=ROULETTE_PANEL_MAX;							//18

		//開始カーソル位置
		wk->start_csr_pos = (u16)recv_buf[num];				//19
		num+=1;
	}

	//デバック****************************************************/
	OS_Printf( "***************\n最終決定されたイベント\n" );
	for( i=0; i < ROULETTE_PANEL_MAX; i++ ){
		OS_Printf( "wk->panel_no[%d] = %d\n", i, wk->panel_no[i] );
	}
	/*************************************************************/

	return;
}

//--------------------------------------------------------------
/**
 * @brief	send_bufにリクエスト(どれをランクアップしたい)をセット
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void Roulette_CommSendBufRankUpType( ROULETTE_WORK* wk, u16 type, u16 param )
{
	u8 m_max;

	//コマンド
	wk->send_buf[0] = type;
	OS_Printf( "送信：type = %d\n", wk->send_buf[0] );

	//カーソル位置
	wk->send_buf[1] = param;
	OS_Printf( "送信：csr_pos = %d\n", wk->send_buf[1] );

	//先に子の選択がきていなくて、まだ値が入っていない時は、親の決定はセットしてしまう
	if( CommGetCurrentID() == COMM_PARENT_ID ){
		if( wk->parent_decide_pos == ROULETTE_DECIDE_NONE ){
			wk->parent_decide_pos = param;
		}
	}

	//親の決定タイプ
	wk->send_buf[2] = wk->parent_decide_pos;
	OS_Printf( "送信：parent_decide_pos = %d\n", wk->send_buf[2] );

#if 1
	//共通のランダム(というかポケモンの位置をどこから見ていくか)を決めておく
	wk->send_buf[3] = *wk->p_rand_pos;
	OS_Printf( "送信：rand_pos = %d\n", wk->send_buf[3] );
#endif

	return;
}

//共通のランダムを決めておく
static u16 Roulette_GetRandPos( ROULETTE_WORK* wk )
{
	return gf_rand();
}

//--------------------------------------------------------------
/**
 * @brief   ランクアップしたいリクエスト 通信受信処理
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		FRONTIER_SYSTEMへのポインタ
 */
//--------------------------------------------------------------
void Roulette_CommRecvBufRankUpType(int id_no,int size,void *pData,void *work)
{
	int i,num;
	ROULETTE_WORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******キャッスル****** ランクアップしたいリクエスト情報受信\n" );

	num = 0;
	wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	//コマンド
	//recv_buf[0]
	
	wk->pair_sel_pos = recv_buf[1];
	OS_Printf( "受信：wk->pair_sel_pos = %d\n", wk->pair_sel_pos );

	////////////////////////////////////////////////////////////////////////
	//親
	if( CommGetCurrentID() == COMM_PARENT_ID ){

		//親の決定がすでに決まっていたら、子の選択は無効
		if( wk->parent_decide_pos != ROULETTE_DECIDE_NONE ){
			wk->pair_sel_pos = 0;
		}else{

			//親の決定が決まっていない時は、
			//親が送信する時に「子にそれでいいよ」と送信する
			//wk->parent_decide_pos	= wk->pair_sel_pos;
			
			//子の選択が採用されたことがわかるようにオフセットを加える
			wk->parent_decide_pos	= wk->pair_sel_pos + wk->hv_max;
		}
	////////////////////////////////////////////////////////////////////////
	//子
	}else{
		//親の決定タイプ
		wk->parent_decide_pos = recv_buf[2];

#if 1
		//親に合わせる
		//共通のランダム(というかポケモンの位置をどこから見ていくか)を決めておく
		(*wk->p_rand_pos) = recv_buf[3];
		OS_Printf( "受信：wk->p_rand_pos = %d\n", *wk->p_rand_pos );
#endif

	}

	OS_Printf( "受信：wk->parent_decide_pos = %d\n", wk->parent_decide_pos );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	send_bufにカーソル位置をセット
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void Roulette_CommSendBufCsrPos( ROULETTE_WORK* wk, u16 type )
{
	wk->send_buf[0] = type;
	OS_Printf( "送信：type = %d\n", wk->send_buf[0] );

	//wk->send_buf[1]	= wk->csr_pos;
	if( wk->csr_pos == (wk->hv_max - 1) ){		//「もどる」
		wk->send_buf[1]	= wk->csr_pos;
	}else{
		//wk->send_buf[1]	= (wk->csr_pos ^ 1);	//位置が左右逆なので反転
		wk->send_buf[1]	= wk->csr_pos;
	}
	OS_Printf( "送信：csr_pos = %d\n", wk->send_buf[1] );

	return;
}

//--------------------------------------------------------------
/**
 * @brief   カーソル位置 通信受信処理
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		FRONTIER_SYSTEMへのポインタ
 */
//--------------------------------------------------------------
void Roulette_CommRecvBufCsrPos(int id_no,int size,void *pData,void *work)
{
	ROULETTE_WORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******キャッスル****** カーソル位置情報受信\n" );

	//wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	//type = recv_buf[0];
	
	wk->pair_csr_pos = (u8)recv_buf[1];						//カーソル位置
	OS_Printf( "受信：wk->pair_csr_pos = %d\n", wk->pair_csr_pos );

	return;
}


//==============================================================================================
//
//	サブシーケンス
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	パネル決定
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_SeqSubPanelDecide( ROULETTE_WORK* wk, u8 decide_type )
{
	int i;
	u8	type,type_offset,pos,panel_no;
	u16 buf16[4];

	//オフセット
	type_offset = wk->hv_max;	

	//選んだカーソル位置を取得
	pos = GetCommSelCsrPos( type_offset, decide_type );

	//決定したイベントナンバーをセット
	panel_no = wk->panel_no[pos];

#ifdef PM_DEBUG
#ifdef DEBUG_DECIDE_EV_NO
	OS_Printf( "\n■デバックでパネルナンバーを強制セットしています！■\n" );
	(panel_no) = DEBUG_PANEL_NO;
#endif

	//通信では、セットするパネルと、どこで止めたかの位置しか送っていないので、
	//配置されていないパネルは、反映しないので、このデバックは無効となるはず。
	if( debug_panel_flag == 1 ){
		(panel_no) = debug_panel_no;
	}
#endif

	(*wk->p_decide_ev_no) = panel_no;

	//カーソル埋め尽くし
	for( i=0; i < ROULETTE_PANEL_MAX ;i++ ){
		if( wk->p_panel[i] != NULL ){
			RouletteObj_AnmChg( wk->p_panel[i], panel_no );
		}
	}

	//決定したカーソル位置にカーソルOBJを移動
	CsrPosSet( wk, pos );

	Snd_SePlay( SEQ_SE_DP_PIRORIRO2 );
	OS_Printf( "カーソル位置 = %d\n", pos );
	OS_Printf( "イベントの種類 = %d\n", panel_no );
	//Roulette_EvSet( wk, panel_no );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	イベントパネルの決定
 *
 * @param	
 *
 * @return	"イベントナンバー"
 */
//--------------------------------------------------------------
static void Roulette_EvPanelChoice( ROULETTE_WORK* wk )
{
	int ev_i,i,j;
	u16 rand,num,rensyou;
	u8 challenge,flag;

	////////////////////////////////////////////////////////////////////////////
	//イベント出現周回数テーブルのどこを参照するかを現在の周回数で決める
	OS_Printf( "現在の周回数 = %d\n", wk->lap );

	//出現可能かテーブルからフラグを取得
	for( ev_i=0; ev_i < ROULETTE_EV_APPEAR_MAX ;ev_i++ ){

		//ルーレット専用画面に渡す周回数は、数が多い方がすでに渡されている
		switch( wk->lap ){
		case 0:
			flag = roulette_ev_appear_tbl[ev_i].flag1;
			break;
		case 1:
			flag = roulette_ev_appear_tbl[ev_i].flag2;
			break;
		case 2:
			flag = roulette_ev_appear_tbl[ev_i].flag3;
			break;
		case 3:
			flag = roulette_ev_appear_tbl[ev_i].flag4;
			break;
		case 4:
			flag = roulette_ev_appear_tbl[ev_i].flag5;
			break;
		case 5:
			flag = roulette_ev_appear_tbl[ev_i].flag6;
			break;
		//case 6:
		//case 7:
		default:
			flag = roulette_ev_appear_tbl[ev_i].flag7;
			break;
		};
		OS_Printf( "roulette_ev_appear_tbl[%d] = %d\n", ev_i, flag );

		//出現可能だったら
		if( flag == 1 ){

			challenge = 0xff;

			//何戦目から出現可能イベントかチェック
			switch( ev_i ){
			case ROULETTE_EV_EX_POKE_CHANGE:
				challenge = ROULETTE_EV_CHALLENGE_POKE_CHANGE;
				break;
			case ROULETTE_EV_PLACE_SPEED_UP:
				challenge = ROULETTE_EV_CHALLENGE_SPEED_UP;
				break;
			case ROULETTE_EV_PLACE_SPEED_DOWN:
				challenge = ROULETTE_EV_CHALLENGE_SPEED_DOWN;
				break;
			case ROULETTE_EV_PLACE_RANDOM:
				challenge = ROULETTE_EV_CHALLENGE_RANDOM;
				break;
			case ROULETTE_EV_EX_BP_GET:
				challenge = ROULETTE_EV_CHALLENGE_BP_GET;
				break;
			case ROULETTE_EV_EX_BTL_WIN:
				challenge = ROULETTE_EV_CHALLENGE_BTL_WIN;
				break;
			case ROULETTE_EV_EX_BP_GET_BIG:
				challenge = ROULETTE_EV_CHALLENGE_BP_GET_BIG;
				break;
			};

			//親子を考慮した連勝数を取得
			rensyou = Roulette_CommGetRensyou( wk );

			num = ( rensyou % ROULETTE_LAP_ENEMY_MAX );

			//カンストしていたら必ず7人目にする
			if( rensyou >= ROULETTE_RENSYOU_MAX ){
				num = 6;			//0-6
			}
			OS_Printf( "現在何人目か = %d\n", num );

			if( challenge != 0xff ){
				switch( num ){
				case 0:
					flag = roulette_ev_challenge_tbl[challenge].flag1;
					break;
				case 1:
					flag = roulette_ev_challenge_tbl[challenge].flag2;
					break;
				case 2:
					flag = roulette_ev_challenge_tbl[challenge].flag3;
					break;
				case 3:
					flag = roulette_ev_challenge_tbl[challenge].flag4;
					break;
				case 4:
					flag = roulette_ev_challenge_tbl[challenge].flag5;
					break;
				case 5:
					flag = roulette_ev_challenge_tbl[challenge].flag6;
					break;
				case 6:
				case 7:
					flag = roulette_ev_challenge_tbl[challenge].flag7;
					break;
				default:
					OS_Printf( "ここには処理がこないはず！\n" );
					GF_ASSERT( 0 );
					flag = 1;
					break;
				};
			}else{
				flag = 1;		//何戦目かのチェックがないので出現可能 
			}

			if( flag == 1 ){
				wk->ev_tbl[wk->ev_tbl_max] = ev_i;		//イベントナンバー代入
				wk->ev_tbl_max++;
				OS_Printf( "選出イベント = %d\n", ev_i );
				OS_Printf( "選出イベント数 = %d\n", wk->ev_tbl_max );
			}
		}
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	イベントパネルの決定2(通信なので処理を分けないとダメ！)
 *
 * @param	
 *
 * @return	"イベントナンバー"
 */
//--------------------------------------------------------------
static void Roulette_EvPanelChoice2( ROULETTE_WORK* wk )
{
	int i,j;

	////////////////////////////////////////////////////////////////////////////
	//ポイント数によってランクを決定
	for( i=0; i < ROULETTE_POINT_TBL_MAX ;i++ ){
		if( wk->point >= roulette_point_tbl[i] ){
			break;
		}
	}

	//roulette_category_tbl[ここ]を取得(0に近いほど自分が有利な確立になる)
	wk->point_rank = i;		//0-3でヒットしなかったら4になる(この値でroulette_category_tbl参照)
	OS_Printf( "**********************\npoint_rank = %d\n", wk->point_rank );

	////////////////////////////////////////////////////////////////////////////

	//デバック*********************************/
	OS_Printf( "***************\n選出されたイベント\n" );
	for( j=0; j < wk->ev_tbl_max; j++ ){
		OS_Printf( "wk->ev_tbl[%d] = %d\n", j, wk->ev_tbl[j] );
	}
	OS_Printf( "***************\n\n" );
	/******************************************/

	//色ごとの枚数を求める
	for( i=0; i < wk->ev_tbl_max; i++ ){

		//赤
		if( Roulette_GetEvPanelColor(wk->ev_tbl[i]) == ROULETTE_PANEL_COLOR_RED ){
			wk->panel_color_num[ROULETTE_PANEL_COLOR_RED]++;

		//黒
		}else if( Roulette_GetEvPanelColor(wk->ev_tbl[i]) == ROULETTE_PANEL_COLOR_BLACK ){
			wk->panel_color_num[ROULETTE_PANEL_COLOR_BLACK]++;

		//白
		}else if( Roulette_GetEvPanelColor(wk->ev_tbl[i]) == ROULETTE_PANEL_COLOR_WHITE ){
			wk->panel_color_num[ROULETTE_PANEL_COLOR_WHITE]++;

		//黄色
		}else if( Roulette_GetEvPanelColor(wk->ev_tbl[i]) == ROULETTE_PANEL_COLOR_YELLOW ){
			wk->panel_color_num[ROULETTE_PANEL_COLOR_YELLOW]++;
		}
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	イベントパネルの決定3(通信なので処理を分けないとダメ！)
 *
 * @param	
 *
 * @return	"イベントナンバー"
 */
//--------------------------------------------------------------
static void Roulette_EvPanelChoice3( ROULETTE_WORK* wk )
{
	int loop,j,offset,range;
	u16 rand,temp_rand;
	u8 num,color;

	//パネル16個セットする
	num = 0;
	for( j=0; j < ROULETTE_PANEL_MAX; j++ ){

		//確立テーブル0-4を渡す
		color = Roulette_GetDecideColor( wk, wk->point_rank );	//赤0、黒1、白2、黄3を取得

#if 1
		offset	= 0;
		range	= 0;

		//ランダムを求める幅をセット(色ごとの個数をを取得)
		range = wk->panel_color_num[color];

		//赤
		if( color == ROULETTE_PANEL_COLOR_RED ){
			offset	= 0;

		//黒
		}else if( color == ROULETTE_PANEL_COLOR_BLACK ){
			offset += wk->panel_color_num[ROULETTE_PANEL_COLOR_RED];

		//白
		}else if( color == ROULETTE_PANEL_COLOR_WHITE ){
			offset += wk->panel_color_num[ROULETTE_PANEL_COLOR_RED];
			offset += wk->panel_color_num[ROULETTE_PANEL_COLOR_BLACK];

		//黄色
		}else if( color == ROULETTE_PANEL_COLOR_YELLOW ){
			offset += wk->panel_color_num[ROULETTE_PANEL_COLOR_RED];
			offset += wk->panel_color_num[ROULETTE_PANEL_COLOR_BLACK];
			offset += wk->panel_color_num[ROULETTE_PANEL_COLOR_WHITE];
		}

		rand = ( gf_rand() % range );

		//出現OKだった各色の個数を足して、決まった色の開始オフセットとする
		rand += offset;

		OS_Printf( "イベントサーチ開始位置 = %d\n", rand );
#else
		rand = ( gf_rand() % wk->ev_tbl_max );	//開始位置
		OS_Printf( "イベントサーチ開始位置 = %d\n", rand );
#endif

		//1周チェック用にコピー
		num = rand;
		loop = 0;

		//選ぶことの出来るリストから選出
		while( 1 ){

			//無限ループ回避
			if( loop >= 50 ){
				GF_ASSERT_MSG( 0, "パネルが決まらずループしています！\n" );
				wk->panel_no[j] = ROULETTE_EV_ENEMY_HP_DOWN;
				loop = 0;
				break;
			}

			///////////////////////////////////////////////////////////////////////////////
			//ここは、色の範囲内から、ランダムで選出にしているので、色は必ず同じになるはず
			//なので、同じ色かをチェックする必要はないが、一応。(X)
			//
			//例えば、白が選ばれることがあるが、
			//白が一つも選出されていない時があるので、
			//同じ色かを見る必要はある！
			///////////////////////////////////////////////////////////////////////////////

			//選出されたリストから、同じ色のイベントを探す
			if( Roulette_GetEvPanelColor(wk->ev_tbl[num]) == color ){
				OS_Printf( "選ばれたイベント = %d\n", wk->ev_tbl[num] );
				wk->panel_no[j] = wk->ev_tbl[num];
				loop = 0;
				break;
			}

			num++;
			loop++;
			if( num >= wk->ev_tbl_max ){
				num = 0;
			}

			//開始位置に戻ってきたら、リストから選ぶことが出来なかったので、色を変更する！
			if( num == rand ){

#if 0
				//違う色を取得
				while( 1 ){
					temp_rand = ( gf_rand() % ROULETTE_PANEL_COLOR_MAX );
					if( temp_rand != color ){
						break;
					}
				}

				//次のカラーもないと、また前のカラーに戻ってということがあるかも
				//白ない、赤ない、白ない、赤ない、、、
#endif

				color++;
				if( color >= ROULETTE_PANEL_COLOR_MAX ){
					color = 0;
				}
				OS_Printf( "開始位置に戻ってきたのでカラーを変更 = %d\n", color );
			}
		}
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	確立からパネルの色を決める
 *
 * @param	point_rank(0-4)
 *
 * @return	"どの色のイベントにするか"
 */
//--------------------------------------------------------------
static u8 Roulette_GetDecideColor( ROULETTE_WORK* wk, u8 point_rank )
{
	u8 num,i;
	u16 rand;

	num = 0;
	rand = ( gf_rand() % 100 );
	OS_Printf( "カラーを決定するrand = %d\n", rand );

	//赤、黒、白、黄の4つ
	for( i=0; i < ROULETTE_PANEL_COLOR_MAX ;i++ ){
		num+=roulette_category_tbl[point_rank][i];
		if( rand < num ){
			break;
		}
	}

	//エラー回避
	if( i >= ROULETTE_PANEL_COLOR_MAX ){
		OS_Printf( "カラー取得がおかしい！\n" );
		GF_ASSERT( 0 );
		i = 0;
	}

	OS_Printf( "color = %d\n", i );
	return i;				//どの色のイベントか
}

//--------------------------------------------------------------
/**
 * @brief	アイテム持っているかアイコン表示、非表示
 *
 * @param	
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Roulette_ItemKeepVanishSet( ROULETTE_WORK* wk )
{
	u8 m_max,e_max,i;
	POKEMON_PARAM* poke;

	m_max = Roulette_GetMinePokeNum( wk->type, ROULETTE_FLAG_TOTAL );
	e_max = Roulette_GetEnemyPokeNum( wk->type, ROULETTE_FLAG_TOTAL );

	//味方
	for( i=0; i < m_max ;i++ ){	
		poke =  PokeParty_GetMemberPointer( wk->p_m_party, i );
		if( PokeParaGet(poke,ID_PARA_item,NULL) == 0 ){
			RouletteObj_Vanish( wk->p_m_itemkeep[i], ROULETTE_VANISH_ON );			//非表示
		}else{
			RouletteObj_Vanish( wk->p_m_itemkeep[i], ROULETTE_VANISH_OFF );			//表示
		}
	}

	//敵
	for( i=0; i < e_max ;i++ ){	
		poke =  PokeParty_GetMemberPointer( wk->p_e_party, i );
		//OS_Printf( "enemy item = %d\n", PokeParaGet(poke,ID_PARA_item,NULL) );
		if( PokeParaGet(poke,ID_PARA_item,NULL) == 0 ){
			RouletteObj_Vanish( wk->p_e_itemkeep[i], ROULETTE_VANISH_ON );			//非表示
		}else{
			RouletteObj_Vanish( wk->p_e_itemkeep[i], ROULETTE_VANISH_OFF );			//表示
		}
	}

	return;
}


//==============================================================================================
//
//	タッチパネル関連
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	ボタン押したか判定
 *
 * @param	wk		ROULETTE_WORK型のポインタ
 *
 * @return	"TRUE = ボタン押した、FALSE = ボタン押していない"
 */
//--------------------------------------------------------------
static BOOL Roulette_CheckButtonPush( ROULETTE_WORK* wk )
{
	int	ret = GF_TP_HitTrg( ButtonHitTbl );

	if( ret == 0 ){
	//if( (ret == 0) || (sys.trg & PAD_BUTTON_A) ){
		//Snd_SePlay( SEQ_SE_DP_BUTTON9 );
		OS_Printf( "ボタン押した！\n" );
		Roulette_SetButtonPush( wk );
		return TRUE;
	}

	return FALSE;
}

static void Roulette_SetButtonPush( ROULETTE_WORK* wk )
{
	Snd_SeStopBySeqNo( SEQ_SE_DP_SELECT, 0 );
	Snd_SePlay( SEQ_SE_DP_BUTTON9 );
	RouletteObj_AnmChg( wk->p_button, ROULETTE_ANM_BUTTON_PUSH );
	return;
}


