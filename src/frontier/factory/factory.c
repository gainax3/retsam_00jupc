//==============================================================================================
/**
 * @file	factory.c
 * @brief	「バトルファクトリー」メインソース
 * @author	Satoshi Nohara
 * @date	2007.03.14
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
#include "system/softsprite.h"
#include "gflib/strbuf_family.h"
#include "savedata/config.h"
#include "application/p_status.h"
#include "poketool/pokeparty.h"
#include "poketool/poke_number.h"
#include "savedata/b_tower.h"

#include "communication/comm_system.h"
#include "communication/comm_tool.h"
#include "communication/comm_def.h"
#include "communication/comm_info.h"
#include "communication/wm_icon.h"

#include "factory_sys.h"
#include "factory_clact.h"
#include "factory_bmp.h"
#include "factory_obj.h"
#include "factory_ball.h"
#include "factory_csr.h"
#include "factory_3d.h"
#include "application/factory.h"
#include "../frontier_tool.h"					//Frontier_PokeParaMake
#include "../factory_tool.h"					//FACTORY_POKE_RANGE
#include "../factory_def.h"
#include "../comm_command_frontier.h"

#include "../../field/comm_direct_counter_def.h"//

#include "msgdata\msg.naix"						//NARC_msg_??_dat
#include "msgdata\msg_factory_rental.h"			//msg_??

#include "../graphic/frontier_obj_def.h"
#include "../graphic/frontier_bg_def.h"


#include "system/pm_overlay.h"
FS_EXTERN_OVERLAY(frontier_common);


//==============================================================================================
//
//	定義
//
//==============================================================================================
//シーケンス定義
enum {
	SEQ_GAME_START_EFF,										//ゲーム開始演出

	SEQ_GAME_INIT,											//初期化
	SEQ_GAME_PSD,											//ポケモンステータス処理

	SEQ_GAME_RENTAL,										//レンタルポケモン選択中
	SEQ_GAME_FINAL_ANSWER,									//以上の3匹でよろしい？
	SEQ_GAME_FINAL_ANSWER_CANCEL,							//以上の3匹でよろしい？(キャンセル)

	SEQ_GAME_TRADE_TEBANASU,								//交換ポケモン選択中(てばなす)
	SEQ_GAME_TRADE_TEBANASU_TYUUSI,							//交換ポケモン選択中(てばなす→ちゅうし)
	SEQ_GAME_TRADE_UKETORU,									//交換ポケモン選択中(うけとる)
	SEQ_GAME_TRADE_UKETORU_TYUUSI,							//交換ポケモン選択中(うけとる→ちゅうし)

	SEQ_GAME_LEAVING_ENTRANCE,								//入場、退場

	SEQ_GAME_SEND_RECV,

	SEQ_GAME_TRADE_END_MULTI,								//通信交換終了
	SEQ_GAME_END,											//終了

	SEQ_GAME_PAIR_TRADE_END,								//パートナーが交換したので強制終了
};

//ポケモン表示
#define TEST_X			(120)
#define TEST_Y			(43)//(48)
#define TEST_Z			(0)
#define TEST_POLY_ID	(0)

//3匹がそろう画面の表示位置
#define TEST_X1			(44)//(48)
#define TEST_X2			(128)//(48)
#define TEST_X3			(212)//(208)

#define FACTORY_SOFT_SPRITE_MAX		(3)						//ソフトスプライトの数

//Hフリップ
#define FLIP_RENTAL			(0)
#define FLIP_TEBANASU		(1)
#define FLIP_UKETORU		(0)

//フォントカラー
#define	COL_BLUE	( GF_PRINTCOLOR_MAKE(FBMP_COL_BLUE,FBMP_COL_BLU_SDW,FBMP_COL_NULL) )//青
#define	COL_RED		( GF_PRINTCOLOR_MAKE(FBMP_COL_RED,FBMP_COL_RED_SDW,FBMP_COL_NULL) )	//赤

//パートナーが交換したことを知らせるメッセージウェイト
#define FACTORY_PAIR_TRADE_END_WAIT	(80)

//相手のポケモン名を描画リクエスト
enum{
	REQ_PAIR_MONSNO_NONE = 0,
	REQ_PAIR_MONSNO_DECIDE,
};

#define FACTORY_MARU_WAIT				(4)					//パレット切り替えウェイト
#define FACTORY_MARU_BASE_PAL			(6)					//基本パレットナンバー
#define FACTORY_MARU_PAL_NUM			(4)					//パレット数

//背景のウィンドウのパレット操作ID
enum{
	ID_PAL_SINGLE_OPEN = 0,									//シングル、ポケ3匹
	ID_PAL_SINGLE_FLASH,
	ID_PAL_SINGLE_NORMAL,
};

//背景のウィンドウの変更するパレットNO
#define FACTORY_SINGLE_WIN_OPEN_PAL		(2)					//開いている時(シングル、ポケ3匹)
#define FACTORY_SINGLE_WIN_FLASH_PAL	(1)					//光る時
#define FACTORY_SINGLE_WIN_NORMAL_PAL	(2)					//通常

//左上のポケモン表示ウィンドウの表示位置
enum{
	FACTORY_OBJ_SINGLE_WIN_X = 80,
	FACTORY_OBJ_SINGLE_WIN_Y = 40,
	FACTORY_OBJ_MULTI_WIN_X = 216,
	FACTORY_OBJ_MULTI_WIN_Y = 40,
	FACTORY_OBJ_3POKE_WIN_X = 128,//80,
	FACTORY_OBJ_3POKE_WIN_Y = 40,
	FACTORY_OBJ_2POKE_WIN_X_OPEN = 80,//64,
	FACTORY_OBJ_2POKE_WIN_X_CLOSE = 80,//64,
	FACTORY_OBJ_2POKE_WIN_Y = 40,
	FACTORY_OBJ_MULTI_2POKE_WIN_X = 128,//80,
	FACTORY_OBJ_MULTI_2POKE_WIN_Y = 40,
};

//アニメウェイトが変更されたら対応しないとダメ！
#define POKE_APPEAR_WAIT				(21)				//ポケモン表示するタイミング
#define POKE_FLASH_WAIT					(2)					//ポケモン光るウェイト

//レールの速度
#define RAIL_SPD	(8)
#define FACTORY_SCROLL_OFFSET_X			(33*RAIL_SPD)		//ループ回数


//==============================================================================================
//
//	メッセージ関連の定義
//
//==============================================================================================
#define FACTORY_MENU_BUF_MAX		(4)						//メニューバッファの最大数
#define BF_FONT						(FONT_SYSTEM)			//フォント種類
#define FACTORY_MSG_BUF_SIZE		(800)//(1024)			//会話のメッセージsize
#define FACTORY_MENU_BUF_SIZE		(64)					//メニューのメッセージsize
#define PLAYER_NAME_BUF_SIZE		(PERSON_NAME_SIZE + EOM_SIZE)	//プレイヤー名のメッセージsize
#define POKE_NAME_BUF_SIZE			(MONS_NAME_SIZE + EOM_SIZE)		//ポケモン名のメッセージsize

//配置データなど
#include "factory.dat"


//==============================================================================================
//
//	(ファクトリー)通常のステータス画面ページ
//
//==============================================================================================
static const u8 PST_PageTbl_Normal[] = {
	PST_PAGE_INFO,			// 「ポケモンじょうほう」
	PST_PAGE_MEMO,			// 「トレーナーメモ」
	PST_PAGE_PARAM,			// 「ポケモンのうりょく」
	//PST_PAGE_CONDITION,		// 「コンディション」
	PST_PAGE_B_SKILL,		// 「たたかうわざ」
	//PST_PAGE_C_SKILL,		// 「コンテストわざ」
	//PST_PAGE_RIBBON,		// 「きねんリボン」
	PST_PAGE_RET,			// 「もどる」
	PST_PAGE_MAX
};


//==============================================================================================
//
//	構造体宣言
//
//==============================================================================================
struct _FACTORY_WORK{
	PROC* proc;										//PROCへのポインタ
	PROC* child_proc;								//PROCへのポインタ

	u8	sub_seq;									//シーケンス
	u8	type;										//引数として渡されたバトルタイプ
	u8	level;										//LV50、オープン
	u8	mode;										//引数として渡された画面モード

	int scr_x;										//スクロールX

	u8	msg_index;									//メッセージindex
	u8	sel_count;									//選択している数
	u8	ball_max;									//表示するボールの最大数を格納
	u8	cancel:1;									//キャンセルしたフラグ
	u8	recover:1;									//復帰フラグ
	u8	tebanasu_uketoru:1;							//手放す=0,受け取る=1
	u8	trade_flag:1;								//交換したかフラグ
	u8	start_eff_flag:1;							//開始演出が終了したか
	u8	pair_data_off_flag:1;						//ペアのデータを表示しないフラグ
	u8	psd_flag:1;									//ポケモンステータス中フラグ
	u8	dummy67:1;									//

	u32	maru_wait;									//円のパレット切り替えウェイト

	u8	pair_sel_count;								//
	u8	wait_count;									//
	u8	tebanasu_max;								//
	u8	uketoru_max;								//

	MSGDATA_MANAGER* msgman;						//メッセージマネージャー
	WORDSET* wordset;								//単語セット
	STRBUF* msg_buf;								//メッセージバッファポインタ
	STRBUF* tmp_buf;								//テンポラリバッファポインタ

	STRBUF* menu_buf[FACTORY_MENU_BUF_MAX];			//メニューバッファポインタ
	STRCODE str[PERSON_NAME_SIZE + EOM_SIZE];		//メニューのメッセージ

	GF_BGL_INI*	bgl;								//BGLへのポインタ
	GF_BGL_BMPWIN bmpwin[FACTORY_BMPWIN_MAX];		//BMPウィンドウデータ

	//BMPメニュー(bmp_menu.h)
	BMPMENU_HEADER MenuH;							//BMPメニューヘッダー
	BMPMENU_WORK* mw;								//BMPメニューワーク
	BMPMENU_DATA Data[FACTORY_MENU_BUF_MAX];		//BMPメニューデータ

	PALETTE_FADE_PTR pfd;							//パレットフェード

	GF_G3DMAN* g3Dman;
	SOFT_SPRITE_MANAGER	*soft_sprite;				//表示用マネージャーへのポインタ
	SOFT_SPRITE *ss[FACTORY_SOFT_SPRITE_MAX];		//ポケモン表示用

	//const CONFIG* config;							//コンフィグポインタ
	CONFIG* config;									//コンフィグポインタ
	SAVEDATA* sv;									//セーブデータポインタ

	PSTATUS_DATA* psd;								//ポケモンステータス

	FACTORY_CLACT factory_clact;					//セルアクタデータ
	FACTORY_BALL* p_ball[BF_ENTRY_POKE_MAX];		//ボールOBJのポインタ格納テーブル
	FACTORY_CSR* p_csr;								//カーソルOBJのポインタ格納テーブル
	FACTORY_CSR* p_csr_menu;						//メニューカーソルOBJのポインタ格納テーブル
	FACTORY_OBJ* p_obj_win;							//ウィンドウOBJのポインタ格納テーブル
	FACTORY_OBJ* p_pair_obj_win;					//ペアのウィンドウOBJのポインタ格納テーブル

	POKEPARTY* p_m_party;							//渡された味方ポケモンデータ(6匹)
	POKEPARTY* p_e_party;							//渡された敵ポケモンデータ(6匹)
	u16 sel_work[ FACTORY_SEL_WORK_MAX ];			//選択したボール位置(0-5)キャンセル(0xff)
	u16* p_ret_work;								//FACTORY_CALL_WORKの戻り値ワークへのポインタ

	int psd_pos;									//ポケモンステータスの選択結果取得

	ARCHANDLE* hdl;

	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//FACTORY_COMM factory_comm;
#if 1
	//通信用：データバッファ
	u16	send_buf[FACTORY_COMM_BUF_LEN];

	//通信用
	u16 pair_trade_flag;							//交換したか

	//通信用：レンタルポケモンデータ
	u16	pair_poke_monsno[FACTORY_COMM_POKE_NUM];	//ポケモンナンバー
	u16 pair_poke_sex[FACTORY_COMM_POKE_NUM];		//ポケモン性別
#endif
	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	
	u8	trade_recieve_count;
	u8	pair_monsno_req;

	u32 dummy_work;
};


//==============================================================================================
//
//	プロトタイプ宣言
//
//==============================================================================================
//PROC
PROC_RESULT FactoryProc_Init( PROC * proc, int * seq );
PROC_RESULT FactoryProc_Main( PROC * proc, int * seq );
PROC_RESULT FactoryProc_End( PROC * proc, int * seq );

//シーケンス
static BOOL Seq_GameStartEff( FACTORY_WORK* wk );
static BOOL Seq_GameInit( FACTORY_WORK* wk );
static BOOL Seq_GamePsd( FACTORY_WORK* wk );
static void RentalInit( FACTORY_WORK* wk );
static void RentalBallInit( FACTORY_WORK* wk );
static void RentalPokeInit( FACTORY_WORK* wk );
static void TradeInit( FACTORY_WORK* wk );
static void TradeBallInit( FACTORY_WORK* wk );

static BOOL Seq_GameRental( FACTORY_WORK* wk );
static void SetBCancel( FACTORY_WORK* wk );
static void SetRentarusuru( FACTORY_WORK* wk );
static void SetHazusu( FACTORY_WORK* wk );

static BOOL Seq_GameFinalAnswer( FACTORY_WORK* wk );
static BOOL Seq_GameFinalAnswerCancel( FACTORY_WORK* wk );
static void SetFinalAnswerNo( FACTORY_WORK* wk );

static BOOL Seq_GameTradeTebanasu( FACTORY_WORK* wk );
static void SetTebanasu( FACTORY_WORK* wk );

static BOOL Seq_GameTradeTebanasuTyuusi( FACTORY_WORK* wk );
static void SetTebanasuTyuusiNo( FACTORY_WORK* wk );

static BOOL Seq_GameTradeUketoru( FACTORY_WORK* wk );
static void SetModoru( FACTORY_WORK* wk );
static void SetUketoru( FACTORY_WORK* wk );
static void SetUketoruNo( FACTORY_WORK* wk );

static BOOL Seq_GameTradeUketoruTyuusi( FACTORY_WORK* wk );
static void SetUketoruTyuusiNo( FACTORY_WORK* wk );

static BOOL Seq_GameLeavingEntrance( FACTORY_WORK* wk );

static BOOL Seq_GameSendRecv( FACTORY_WORK* wk );

static BOOL Seq_GameTradeEndMulti( FACTORY_WORK* wk );
static BOOL Seq_GameEnd( FACTORY_WORK* wk );

static BOOL Seq_GamePairTradeEnd( FACTORY_WORK* wk );

static void FactoryCommon_Delete( FACTORY_WORK* wk );
static void Factory_Recover( FACTORY_WORK* wk );
static void Factory_InitSub1( void );
static void Factory_InitSub2( FACTORY_WORK* wk );
static void FactoryCommon_ChangePoke( FACTORY_WORK* wk, u8 index, u8 poke_pos, int flip, const POKEPARTY* party );

//共通初期化、終了
static void Factory_ObjInit( FACTORY_WORK* wk );
static void Factory_BgInit( FACTORY_WORK* wk );
static void Factory_BgExit( GF_BGL_INI * ini );

//設定
static void VBlankFunc( void * work );
static void SetVramBank(void);
static void SetBgHeader( GF_BGL_INI * ini );

//ツール
static void NextSeq( FACTORY_WORK* wk, int* seq, int next );
static int KeyCheck( int key );
static void BgCheck( FACTORY_WORK* wk );
static BOOL RailMoveIn( FACTORY_WORK* wk );
static BOOL RailMoveOut( FACTORY_WORK* wk );
static void MaruMove( FACTORY_WORK* wk );
static void ScrPalChg( FACTORY_WORK* wk, u32 frm, u8 pltt_no, u8 sx, u8 sy );
static void Factory_ObjWinSet( FACTORY_WORK* wk, u32 anm_no );

//BGグラフィックデータ
static void Factory_SetMainBgGraphic( FACTORY_WORK * wk, u32 frm  );
static void Factory_SetMainBgGraphic2( FACTORY_WORK * wk, u32 frm  );
static void Factory_SetMainBgGraphic3( FACTORY_WORK * wk, u32 frm  );
static void Factory_SetMainBgGraphic_Rail( FACTORY_WORK * wk, u32 frm  );
static void Factory_SetMainBgGraphic_Back( FACTORY_WORK * wk, u32 frm  );
static void Factory_SetMainBgGraphic_Maru( FACTORY_WORK * wk, u32 frm  );
static void Factory_SetMainBgPalette( void );
static void Factory_SetSubBgGraphic( FACTORY_WORK * wk, u32 frm  );
static void Factory_SetMainBgScrn( FACTORY_WORK * wk, u32 frm  );
static void Factory_SetRentalBackBgScrn( FACTORY_WORK * wk, u32 frm );
static void Factory_Set3PokeBgScrn( FACTORY_WORK * wk, u32 frm );

//メッセージ
static u8 FactoryWriteMsg( FACTORY_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font );
static u8 FactoryWriteMsgSimple( FACTORY_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font );
static u8 Factory_EasyMsg( FACTORY_WORK* wk, int msg_id );

//メニュー
static void FactoryInitMenu( FACTORY_WORK* wk, GF_BGL_BMPWIN* win, u8 y_max );
static void FactorySetMenuData( FACTORY_WORK* wk, u8 no, u8 param, int msg_id );
static void Factory_SetMenu( FACTORY_WORK* wk );
static void Factory_SetMenu2( FACTORY_WORK* wk );
static void Factory_SetMenu3( FACTORY_WORK* wk );
static void Factory_SetMenu4( FACTORY_WORK* wk );

//文字列
static void Factory_SetNumber( FACTORY_WORK* wk, u32 bufID, s32 number );
static void Factory_SetPokeName( FACTORY_WORK* wk, u32 bufID, POKEMON_PASO_PARAM* ppp );
static void Factory_SetPlayerName( FACTORY_WORK* wk, u32 bufID );
static void PlayerNameWrite( FACTORY_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 font );
static void PairNameWrite( FACTORY_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 font );
static void PokeNameWrite( FACTORY_WORK* wk, GF_BGL_BMPWIN* win, u8 csr_pos, u32 x, u32 y, u8 f_col, u8 s_col, u8 b_col, u8 font, const POKEPARTY* party );
static void PokeNameWriteEx( FACTORY_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 f_col, u8 s_col, u8 b_col, u8 font, u16 monsno, u8 sex );

//ポケモンステータス
static void Factory_SetPSD( FACTORY_WORK* wk );

//ツール
static BOOL Factory_CheckType( FACTORY_WORK* wk, u8 type );
static BOOL Factory_CheckMode( FACTORY_WORK* wk, u8 mode );
static u8 Factory_GetRentalSelPokeNum( u8 type );
static void Factory_SelWorkClear( FACTORY_WORK* wk );
static BOOL Factory_CheckCancel( FACTORY_WORK* wk );
static void Factory_SetCancel( FACTORY_WORK* wk, u8 flag );
static u8 Factory_GetScrPalNo( FACTORY_WORK* wk, u8 id, u8 final_answer_flag  );
static u32 Factory_GetWinAnmNo( FACTORY_WORK* wk, u32 anm_no );

//ポケモン表示
static void Factory_PokeAdd(FACTORY_WORK* wk, u8 index, POKEMON_PARAM* poke, int x,int y, int flip);
static BOOL PokeFlipCheck(u32 monsno);

//通信
BOOL Factory_CommSetSendBuf( FACTORY_WORK* wk, u16 type, u16 param );
void Factory_CommSendBufBasicData( FACTORY_WORK* wk, u16 type );
void Factory_CommRecvBufBasicData(int id_no,int size,void *pData,void *work);
void Factory_CommSendBufRentalData( FACTORY_WORK* wk, u16 type, u16 param );
void Factory_CommRecvBufRentalData(int id_no,int size,void *pData,void *work);
void Factory_CommRecvBufCancelData(int id_no,int size,void *pData,void *work);
void Factory_CommSendBufTradeFlag( FACTORY_WORK* wk, u16 type, u16 sel );
void Factory_CommRecvBufTradeFlag(int id_no,int size,void *pData,void *work);
static void Factory_PairMonsNoRecvReq( FACTORY_WORK* wk );

//共通処理
static FACTORY_BALL* Factory_BallAdd( FACTORY_WORK* wk, int no );
static FACTORY_OBJ* Factory_ObjWinAdd( FACTORY_WORK* wk, u32 anm_no );
static FACTORY_OBJ* Factory_PairObjWinAdd( FACTORY_WORK* wk, u32 anm_no );
static void Factory_ObjWinDel( FACTORY_WORK* wk );
static void Factory_FrameWinBmpSet( FACTORY_WORK* wk );
static void BmpMenuWinClearSub( GF_BGL_BMPWIN* win );


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
PROC_RESULT FactoryProc_Init( PROC * proc, int * seq )
{
	int i;
	FACTORY_WORK* wk;
	FACTORY_CALL_WORK* factory_call;

	Overlay_Load( FS_OVERLAY_ID(frontier_common), OVERLAY_LOAD_NOT_SYNCHRONIZE );

	Factory_InitSub1();

	sys_CreateHeap( HEAPID_BASE_APP, HEAPID_FACTORY, 0x20000 );

	wk = PROC_AllocWork( proc, sizeof(FACTORY_WORK), HEAPID_FACTORY );
	memset( wk, 0, sizeof(FACTORY_WORK) );

	//3Dシステム設定
	wk->g3Dman = GF_G3DMAN_Init( HEAPID_FACTORY, GF_G3DMAN_LNK, GF_G3DTEX_256K, 
									GF_G3DMAN_LNK, GF_G3DPLT_32K, Factory_SimpleSetUp );

	wk->bgl				= GF_BGL_BglIniAlloc( HEAPID_FACTORY );
	wk->proc			= proc;
	factory_call		= (FACTORY_CALL_WORK*)PROC_GetParentWork( proc );
	wk->sv				= factory_call->sv;
	wk->type			= factory_call->type;
	wk->level			= factory_call->level;
	wk->mode			= factory_call->mode;
	wk->p_m_party		= factory_call->p_m_party;
	wk->p_e_party		= factory_call->p_e_party;
	wk->p_ret_work		= &factory_call->ret_work[0];
	wk->config			= SaveData_GetConfig( wk->sv );			//コンフィグポインタを取得

	//値が一番小さいパレットは6で、初期パレットが8になっている
	wk->maru_wait		= ( FACTORY_MARU_WAIT * 2 );

	//レンタルモード
	if( Factory_CheckMode(wk,FACTORY_MODE_RENTAL) == TRUE ){
		wk->ball_max = BF_ENTRY_POKE_MAX;

	//交換モード(ボールの数と配置場所などが違う)
	}else{

		//通信タイプの時
		if( Factory_CommCheck(wk->type) == TRUE ){
			wk->ball_max = BF_T_MULTI_BALL_TEBANASU_POS_MAX;
			wk->tebanasu_max = BF_HV_TYUUSI_MULTI_MAX;
			wk->uketoru_max = BF_HV_MODORU_MULTI_MAX;
		}else{
			wk->ball_max = BF_T_BALL_POS_MAX;
			wk->tebanasu_max = BF_HV_TYUUSI_MAX;
			wk->uketoru_max = BF_HV_MODORU_MAX;
		}
	}

	Factory_InitSub2( wk );

	if( Factory_CommCheck(wk->type) == TRUE ){
		CommCommandFrontierInitialize( wk );
	}

	(*seq) = SEQ_GAME_START_EFF;
	OS_Printf( "(*seq) = %d\n", (*seq) );

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
PROC_RESULT FactoryProc_Main( PROC * proc, int * seq )
{
	FACTORY_WORK* wk  = PROC_GetWork( proc );
	//OS_Printf( "(*seq) = %d\n", (*seq) );

	//ペアが交換した時
	if( wk->pair_trade_flag == 1 ){

		switch( *seq ){
		//このシーケンス内では強制終了させる！
		case SEQ_GAME_TRADE_TEBANASU:					//交換ポケモン選択中(てばなす)
		case SEQ_GAME_TRADE_TEBANASU_TYUUSI:			//交換ポケモン選択中(てばなす→ちゅうし)
		case SEQ_GAME_TRADE_UKETORU:					//交換ポケモン選択中(うけとる)
		case SEQ_GAME_TRADE_UKETORU_TYUUSI:				//交換ポケモン選択中(うけとる→ちゅうし)

			//ステータスに切り替える時以外
			if( wk->psd_flag == 0 ){
				NextSeq( wk, seq, SEQ_GAME_SEND_RECV );				//
			}
			break;

		//このシーケンス内では強制終了させない！
		//case SEQ_GAME_START_EFF:						//開始演出
		//case SEQ_GAME_INIT:							//初期化
		//case SEQ_GAME_PSD:							//ポケモンステータス処理
		//case SEQ_GAME_LEAVING_ENTRANCE:				//入場、退場
			//break;
		//特に何もしない！
		//case SEQ_GAME_TRADE_END_MULTI:				//通信交換終了
		//case SEQ_GAME_END:							//終了
		};
	}

	//]]]]]]]]]]]]]
	
	switch( *seq ){

	//-----------------------------------
	//開始演出
	case SEQ_GAME_START_EFF:
		if( Seq_GameStartEff(wk) == TRUE ){
			NextSeq( wk, seq, SEQ_GAME_INIT );
		}
		break;

#if 1
	//-----------------------------------
	//ポケモンステータス処理
	case SEQ_GAME_PSD:
		if( Seq_GamePsd(wk) == TRUE ){
			NextSeq( wk, seq, SEQ_GAME_INIT );
		}else{
			return PROC_RES_CONTINUE;	//注意！]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
		}
		//break;	//注意！]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#endif

	//-----------------------------------
	//初期化
	case SEQ_GAME_INIT:
		if( Seq_GameInit(wk) == TRUE ){
			if( Factory_CheckMode(wk,FACTORY_MODE_RENTAL) == TRUE ){	//レンタル
				NextSeq( wk, seq, SEQ_GAME_RENTAL );
			}else{														//交換
				NextSeq( wk, seq, SEQ_GAME_TRADE_TEBANASU );
			}
		}
		break;

	//-----------------------------------
	//レンタルポケモン選択中
	case SEQ_GAME_RENTAL:
		//パートナーのポケモン名
		Factory_PairMonsNoRecvReq( wk );

		if( Seq_GameRental(wk) == TRUE ){
			if( wk->recover == 1 ){
				NextSeq( wk, seq, SEQ_GAME_PSD );						//ポケモンステータス
			}else{
				if( wk->sel_count == Factory_GetRentalSelPokeNum(wk->type) ){
					NextSeq( wk, seq, SEQ_GAME_FINAL_ANSWER );			//3匹選択したので終了
				}else{
					//Bキャンセルをした時
					if( Factory_CheckCancel(wk) == TRUE ){
						//ここはキャンセルフラグそのまま
						NextSeq( wk, seq, SEQ_GAME_END );				//終了へ
					}else{
						NextSeq( wk, seq, SEQ_GAME_RENTAL );			//3匹選択するまで繰り返す
					}
				}
			}
		}
		break;

	//-----------------------------------
	//以上の3匹でよろしいですか？
	case SEQ_GAME_FINAL_ANSWER:

		//BgCheck( wk );

		//パートナーのポケモン名
		Factory_PairMonsNoRecvReq( wk );

		if( Seq_GameFinalAnswer(wk) == TRUE ){
			if( wk->sel_count == Factory_GetRentalSelPokeNum(wk->type) ){
				NextSeq( wk, seq, SEQ_GAME_END );						//終了へ
			}else{
				NextSeq( wk, seq, SEQ_GAME_FINAL_ANSWER_CANCEL );		//キャンセル処理→再度選択へ
			}
		}
		break;

	//-----------------------------------
	//以上の3匹でよろしいですか？(キャンセル)
	case SEQ_GAME_FINAL_ANSWER_CANCEL:
		if( Seq_GameFinalAnswerCancel(wk) == TRUE ){
			NextSeq( wk, seq, SEQ_GAME_RENTAL );						//再度選択へ
		}
		break;

	//-----------------------------------
	//手放すポケモン選択中
	case SEQ_GAME_TRADE_TEBANASU:
		if( Seq_GameTradeTebanasu(wk) == TRUE ){
			if( wk->recover == 1 ){
				NextSeq( wk, seq, SEQ_GAME_PSD );						//ポケモンステータス
			}else{
				//Bキャンセルをした時
				if( Factory_CheckCancel(wk) == TRUE ){
					Factory_SetCancel( wk, 0 );
					NextSeq( wk, seq, SEQ_GAME_TRADE_TEBANASU_TYUUSI );	//中止する？
				}else{
					NextSeq( wk, seq, SEQ_GAME_LEAVING_ENTRANCE );		//入場、退場
				}
			}
		}
		break;

	//-----------------------------------
	//手放すポケモン中止する？
	case SEQ_GAME_TRADE_TEBANASU_TYUUSI:
		if( Seq_GameTradeTebanasuTyuusi(wk) == TRUE ){

			//Bキャンセルをした時
			if( Factory_CheckCancel(wk) == TRUE ){
				Factory_SetCancel( wk, 0 );
				NextSeq( wk, seq, SEQ_GAME_TRADE_TEBANASU );			//手放すポケモンを選ぶ
			}else{
				//ここはキャンセルフラグそのまま

				//通信タイプの時
				if( Factory_CommCheck(wk->type) == TRUE ){
					wk->trade_flag = 0;									//交換したフラグOFF
					NextSeq( wk, seq, SEQ_GAME_SEND_RECV );				//
				}else{
					NextSeq( wk, seq, SEQ_GAME_END );					//終了へ
				}
			}
		}
		break;

	//-----------------------------------
	//受け取るポケモン選択中
	case SEQ_GAME_TRADE_UKETORU:
		if( Seq_GameTradeUketoru(wk) == TRUE ){

			//Bキャンセルをした時
			if( Factory_CheckCancel(wk) == TRUE ){
				Factory_SetCancel( wk, 0 );
				NextSeq( wk, seq, SEQ_GAME_TRADE_UKETORU_TYUUSI );		//中止する？
			}else{
				if( wk->sel_count == 0 ){
					NextSeq( wk, seq, SEQ_GAME_LEAVING_ENTRANCE );		//入場、退場
				}else{
					//通信タイプの時
					if( Factory_CommCheck(wk->type) == TRUE ){
						NextSeq( wk, seq, SEQ_GAME_SEND_RECV );			//
					}else{
						NextSeq( wk, seq, SEQ_GAME_END );				//終了へ
					}
				}
			}
		}
		break;

	//-----------------------------------
	//受け取るポケモン中止する？
	case SEQ_GAME_TRADE_UKETORU_TYUUSI:
		if( Seq_GameTradeUketoruTyuusi(wk) == TRUE ){

			//Bキャンセルをした時
			if( Factory_CheckCancel(wk) == TRUE ){
				Factory_SetCancel( wk, 0 );
				NextSeq( wk, seq, SEQ_GAME_TRADE_UKETORU );				//受け取るポケモンを選ぶ
			}else{

				//通信タイプの時
				if( Factory_CommCheck(wk->type) == TRUE ){
					wk->trade_flag = 0;									//交換したフラグOFF
					NextSeq( wk, seq, SEQ_GAME_SEND_RECV );				//
				}else{
					NextSeq( wk, seq, SEQ_GAME_END );					//終了へ
				}
			}
		}
		break;

	//-----------------------------------
	//入場、退場
	case SEQ_GAME_LEAVING_ENTRANCE:
		if( Seq_GameLeavingEntrance(wk) == TRUE ){
			if( wk->sel_count == 0 ){
				TradeInit( wk );
				NextSeq( wk, seq, SEQ_GAME_TRADE_TEBANASU );			//手放すポケモンを選ぶ
			}else{
				NextSeq( wk, seq, SEQ_GAME_TRADE_UKETORU );				//受け取るポケモンを選ぶ
			}
		}
		break;

	//-----------------------------------
	//送受信
	case SEQ_GAME_SEND_RECV:
		if( Seq_GameSendRecv(wk) == TRUE ){
			//相手の交換の方が有効かチェック
			if( wk->pair_trade_flag == 1 ){
				NextSeq( wk, seq, SEQ_GAME_PAIR_TRADE_END );
			}else{
				NextSeq( wk, seq, SEQ_GAME_TRADE_END_MULTI );			//通信交換終了へ
			}
		}
		break;

	//-----------------------------------
	//通信交換終了
	case SEQ_GAME_TRADE_END_MULTI:
		if( Seq_GameTradeEndMulti(wk) == TRUE ){
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

	//-----------------------------------
	//パートナーが交換したので強制終了
	case SEQ_GAME_PAIR_TRADE_END:
		if( Seq_GamePairTradeEnd(wk) == TRUE ){
			NextSeq( wk, seq, SEQ_GAME_TRADE_END_MULTI );				//通信交換終了へ
		}
		break;

	}

	CLACT_Draw( wk->factory_clact.ClactSet );		//セルアクター常駐関数
	Factory_3DMain( wk->soft_sprite );				//3Dメイン処理

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
PROC_RESULT FactoryProc_End( PROC * proc, int * seq )
{
	int i;
	FACTORY_WORK* wk = PROC_GetWork( proc );

	//レンタルモード
	if( Factory_CheckMode(wk,FACTORY_MODE_RENTAL) == TRUE ){
		for( i=0; i < Factory_GetRentalSelPokeNum(wk->type); i++ ){
			wk->p_ret_work[i] = wk->sel_work[i];		//戻り値格納ワークへ代入
		}

	//交換モード(ボールの数と配置場所が違う)
	}else{
		for( i=0; i < 2; i++ ){							//[0]手持ちのどれと、[1]敵のどれを交換
			wk->p_ret_work[i] = wk->sel_work[i];		//戻り値格納ワークへ代入
			//OS_Printf( "wk->sel_work[0] = %d\n", wk->sel_work[0] );
			//OS_Printf( "wk->sel_work[1] = %d\n", wk->sel_work[1] );
		}

		//交換していない時
		if( wk->trade_flag == 0 ){
			wk->p_ret_work[0] = FACTORY_RET_CANCEL;		//戻り値格納ワークへ代入
			wk->p_ret_work[1] = FACTORY_RET_CANCEL;		//戻り値格納ワークへ代入
		}
	}


	OS_Printf( "wk->p_ret_work[0] = %d\n", wk->p_ret_work[0] );
	OS_Printf( "wk->p_ret_work[1] = %d\n", wk->p_ret_work[1] );
	OS_Printf( "wk->p_ret_work[2] = %d\n", wk->p_ret_work[2] );

	FactoryCommon_Delete( wk );							//削除処理

	PROC_FreeWork( proc );								//ワーク開放

	sys_VBlankFuncChange( NULL, NULL );					//VBlankセット
	sys_DeleteHeap( HEAPID_FACTORY );

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
 * @brief	シーケンス：ゲーム開始演出
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameStartEff( FACTORY_WORK* wk )
{
	int i;
	const VecFx32* p_vec;

	enum{
		SEQ_START_EFF_TIMING = 0,
		SEQ_START_EFF_TIMING_SYNC,
		SEQ_START_EFF_START,
		SEQ_START_EFF_WIPE_END_CHECK,
		SEQ_START_EFF_RAIL_MOVE,
		SEQ_START_EFF_POKE_APPEAR_TIMING,
		SEQ_START_EFF_POKE_FADEIN,
	};

	switch( wk->sub_seq ){

	//コマンド設定後の同期
	case SEQ_START_EFF_TIMING:
		if( Factory_CommCheck(wk->type) == TRUE ){
			CommToolTempDataReset();
			CommTimingSyncStart( DBC_TIM_FACTORY_START_INIT );
		}
		wk->sub_seq++;
		break;

	//タイミングコマンドが届いたか確認
	case SEQ_START_EFF_TIMING_SYNC:
		if( Factory_CommCheck(wk->type) == TRUE ){
			if( CommIsTimingSync(DBC_TIM_FACTORY_START_INIT) == TRUE ){
				CommToolTempDataReset();
				wk->sub_seq++;
			}
		}else{
			wk->sub_seq++;
		}
		break;

	//ゲーム開始演出スタート
	case SEQ_START_EFF_START:
		OS_Printf( "*********\nゲーム開始演出\n************\n" );

		//ボールの初期位置を画面外にセット
		for( i=0; i < wk->ball_max ;i++ ){
			p_vec = FactoryBall_GetPos( wk->p_ball[i] );		//現在の座標取得
			FactoryBall_Entrance( wk->p_ball[i] );				//座標セット
		}

		//止まった時にスクロール位置があうようにする
		GF_BGL_ScrollSet( wk->bgl, BF_FRAME_SLIDE, GF_BGL_SCROLL_X_SET, FACTORY_SCROLL_OFFSET_X );

		SoftSpriteParaSet( wk->ss[0], SS_PARA_VANISH, 1 );		//非表示

		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
						WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC*3, HEAPID_FACTORY );

		wk->sub_seq++;
		break;

	//ワイプ終了待ち
	case SEQ_START_EFF_WIPE_END_CHECK:
		if( WIPE_SYS_EndCheck() == FALSE ){
			break;
		}
		Snd_SePlay( SEQ_SE_DP_ELEBETA2 );
		wk->wait_count = 0;
		wk->sub_seq++;
		break;

	//レール移動
	case SEQ_START_EFF_RAIL_MOVE:
		if( RailMoveIn(wk) == TRUE ){
			Snd_SeStopBySeqNo( SEQ_SE_DP_ELEBETA2, 0 );
			Snd_SePlay( SEQ_SE_DP_KASYA );

			//ボール揺れアニメ
			for( i=0; i < wk->ball_max; i++ ){
				FactoryBall_AnmChg( wk->p_ball[i], ANM_BALL_YURE );
			}

			//ウィンドウ処理開始
			Factory_ObjWinSet( wk, ANM_SINGLE_OPEN );	//開く

			//値が一番小さいパレットは6で、初期パレットが8になっている
			wk->maru_wait		= ( FACTORY_MARU_WAIT * 2 );

			wk->wait_count = 0;
			wk->sub_seq++;
		}
		break;

	//ポケモン表示タイミングを合わせる
	case SEQ_START_EFF_POKE_APPEAR_TIMING:
		//左上のOBJウィンドウのアニメショーン終了まで待つ
		if( FactoryObj_AnmActiveCheck(wk->p_obj_win) == TRUE ){
			break;
		}
		Factory_ObjWinDel( wk );											//OBJウィンドウ削除
		wk->wait_count = 0;													//クリア
		wk->sub_seq++;
		break;

	//ポケモンフェードイン
	case SEQ_START_EFF_POKE_FADEIN:
		if( wk->wait_count == 0 ){
			Factory_SetMainBgScrn( wk, BF_FRAME_BG );
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );		//BG非表示
			ScrPalChg( wk, BF_FRAME_BG, Factory_GetScrPalNo(wk, ID_PAL_SINGLE_FLASH, 0), 21, 11 );
			SoftSpriteParaSet( wk->ss[0], SS_PARA_VANISH, 0 );				//表示
			SoftSpritePalFadeSetAll( wk->soft_sprite, 16, 0, 1, 0xffff );	//白からフェードイン
		}

		if( SoftSpritePalFadeExist(wk->ss[0]) ){							//TRUE = フェード中
			//現状特になし
		}

		wk->wait_count++;
		if( wk->wait_count < POKE_FLASH_WAIT ){
			break;
		}
		ScrPalChg( wk, BF_FRAME_BG, Factory_GetScrPalNo(wk, ID_PAL_SINGLE_NORMAL, 0), 21, 11 );
		wk->wait_count = 0;

		//BMPとして使用するBG面をクリアしておく
		Factory_FrameWinBmpSet( wk );

		return TRUE;
		//break;
		
	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	シーケンス：ゲーム初期化
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameInit( FACTORY_WORK* wk )
{
	int i;

	switch( wk->sub_seq ){

	case 0:
		OS_Printf( "*********\nゲーム初期化\n************\n" );
		if( Factory_CheckMode(wk,FACTORY_MODE_RENTAL) == TRUE ){		//レンタル
			RentalInit( wk );
		}else{
			TradeInit( wk );
		}

		//通信タイプの時
		if( Factory_CommCheck(wk->type) == TRUE ){
			PairNameWrite(wk, &wk->bmpwin[BMPWIN_TR2], 0, 0, BF_FONT );	//パートナー名を表示
		}

		//開始演出が終了している時(強さを見るから画面復帰時など)はワイプを入れる
		if( wk->start_eff_flag == 1 ){
			WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
							WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC*3, HEAPID_FACTORY );
		}

		wk->start_eff_flag = 1;											//開始演出終了
		wk->sub_seq++;
		break;
		
	//フェード終了待ち
	case 1:
		//OS_Printf( "*********\nゲーム初期化 ワイプ待ち\n************\n" );
		if( WIPE_SYS_EndCheck() == TRUE ){
			return TRUE;
		}
		break;

	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	レンタル初期化
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static void RentalInit( FACTORY_WORK* wk )
{
	int i;
	u8 m_max;

	m_max = Factory_GetMinePokeNum( wk->type );

	Factory_SetCancel( wk, 0 );

	//カーソル生成
	wk->p_csr = FactoryCsr_Create(	&wk->factory_clact, 
									wk->ball_max, wk->ball_max,
									CSR_H_MODE, wk->psd_pos, bf_h_ball_csr_pos, NULL );

	//プレイヤー名を表示
	PlayerNameWrite(wk, &wk->bmpwin[BMPWIN_TR1], 0, 0, BF_FONT );

	//選択した状態にする
	for( i=0; i < wk->sel_count ;i++ ){
		FactoryBall_Decide( wk->p_ball[ wk->sel_work[i] ] );
		FactoryBall_PalChg( wk->p_ball[ wk->sel_work[i] ], PAL_BALL_MOVE );
		FactoryBall_AnmChg( wk->p_ball[ wk->sel_work[i] ], ANM_BALL_MOVE );

		PokeNameWrite(	wk, &wk->bmpwin[BMPWIN_POKE1+i], 
						wk->sel_work[i], 0, 0, 
						//FBMP_COL_BLACK, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, wk->p_m_party );
						FBMP_COL_WHITE, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, wk->p_m_party );
	}

	//現在選択しているポケモン名表示
	PokeNameWrite(	wk, &wk->bmpwin[BMPWIN_POKE1+wk->sel_count], 
					FactoryCsr_GetCsrPos(wk->p_csr), 0, 0, 
					//FBMP_COL_BLACK, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, wk->p_m_party );
					FBMP_COL_WHITE, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, wk->p_m_party );

	//会話ウィンドウ表示
	FactoryTalkWinPut( &wk->bmpwin[BMPWIN_TALK], CONFIG_GetWindowType(wk->config) );

	//「○ばんめの　ポケモンを　えらんで　ください」
	Factory_SetNumber( wk, 0, (wk->sel_count+1) );
	wk->msg_index = Factory_EasyMsg( wk, msg_rental_01 );

	if( wk->recover == 1 ){

		FactoryCsr_Pause( wk->p_csr, 1 );	//カーソルポーズ

		//メニュー生成
		Factory_SetMenu( wk );

		//メニューカーソル生成
		wk->p_csr_menu = FactoryCsr_Create( &wk->factory_clact, 
											POKE_SEL_MENU_MAX, POKE_SEL_MENU_MAX, 
											CSR_V_MODE, 0, bf_v_menu_csr_pos, NULL );

		//「つよさを見るを選んだ時に表示が消えるので対処する」
        // MatchComment: change condition from pair_poke_monsno[0] & [1] to pair_sel_count
		if ( (wk->pair_sel_count != 0) ) {

			//パートナーの選んだポケモンを表示
			for( i=0; i < m_max ;i++ ){
#if 0
				PokeNameWriteEx(wk, &wk->bmpwin[BMPWIN_POKE4+i],				//性別指定有り
								0, 0, 
								//FBMP_COL_BLACK, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, 
								FBMP_COL_WHITE, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, 
								wk->pair_poke_monsno[i], wk->pair_poke_sex[i] );

				GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_POKE4+i] );
#else
				GF_BGL_BmpWinDataFill( &wk->bmpwin[BMPWIN_POKE4+i], FBMP_COL_NULL );	//塗りつぶし

				//パートナーの選んだポケモンを表示
				if( i < wk->pair_sel_count ){
					PokeNameWriteEx(wk, &wk->bmpwin[BMPWIN_POKE4+i],				//性別指定有り
									0, 0, 
									//FBMP_COL_BLACK, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, 
									FBMP_COL_WHITE, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, 
									wk->pair_poke_monsno[i], wk->pair_poke_sex[i] );
				}

				GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_POKE4+i] );
#endif
			}

		}

	}

	return;
}

static void RentalBallInit( FACTORY_WORK* wk )
{
	int i;

	//ボール生成
	for( i=0; i < wk->ball_max ;i++ ){
		wk->p_ball[i] = Factory_BallAdd( wk, i );
		FactoryBall_Vanish( wk->p_ball[i], FACTORY_VANISH_OFF );	//表示
	}

	return;
}

static void RentalPokeInit( FACTORY_WORK* wk )
{
	Factory_PokeAdd(wk, 0, PokeParty_GetMemberPointer(wk->p_m_party,wk->psd_pos),
					TEST_X, TEST_Y, FLIP_RENTAL );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	交換初期化
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static void TradeInit( FACTORY_WORK* wk )
{
	int i;

	Factory_SetCancel( wk, 0 );

	//会話ウィンドウ表示
	FactoryTalkWinPut( &wk->bmpwin[BMPWIN_TALK], CONFIG_GetWindowType(wk->config) );

	//カーソル生成
	if( Factory_CommCheck(wk->type) == FALSE ){
		wk->p_csr = FactoryCsr_Create(	&wk->factory_clact, 
										wk->tebanasu_max, BF_HV_TYUUSI_H_MAX, 
										CSR_HV_MODE, wk->psd_pos, 
										bf_hv_tyuusi_csr_pos, bf_hv_tyuusi_anm_tbl );
	}else{
		wk->p_csr = FactoryCsr_Create(	&wk->factory_clact, 
										wk->tebanasu_max, BF_HV_TYUUSI_H_MULTI_MAX, 
										CSR_HV_MODE, wk->psd_pos, 
										bf_hv_tyuusi_multi_csr_pos, bf_hv_tyuusi_multi_anm_tbl );
	}

	//プレイヤー名を表示
	PlayerNameWrite(wk, &wk->bmpwin[BMPWIN_TR1], 0, 0, BF_FONT );

	//必ず1匹はポケモン名表示
	PokeNameWrite(	wk, &wk->bmpwin[BMPWIN_POKE1], 
					FactoryCsr_GetCsrPos(wk->p_csr), 0, 0, 
					//FBMP_COL_BLACK, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, wk->p_m_party );
					FBMP_COL_WHITE, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, wk->p_m_party );

	//右下メニュー表示「ちゅうし」
	FactoryWriteMenuWin( wk->bgl, &wk->bmpwin[BMPWIN_SEL] );
	wk->msg_index = FactoryWriteMsg(wk, &wk->bmpwin[BMPWIN_SEL], msg_f_change_choice_01, 
									1, 1, MSG_NO_PUT, 
									FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE, BF_FONT );
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_SEL] );

	//「てばなす　ポケモンを　えらんで　ください」
	wk->msg_index = Factory_EasyMsg( wk, msg_f_change_01 );

	if( wk->recover == 1 ){
		//メニュー生成
		Factory_SetMenu( wk );

		//メニューカーソル生成
		wk->p_csr_menu = FactoryCsr_Create( &wk->factory_clact, 
											POKE_SEL_MENU_MAX, POKE_SEL_MENU_MAX, 
											CSR_V_MODE, 0, bf_v_menu_csr_pos, NULL );
	}
	return;
}

static void TradeBallInit( FACTORY_WORK* wk )
{
	int i;

	SDK_ASSERTMSG( wk->p_ball[0] == NULL, "wk->p_ball[0]がNULLではありません！" );

	//ボール生成
	for( i=0; i < wk->ball_max ;i++ ){
		wk->p_ball[i] = Factory_BallAdd( wk, i );
		FactoryBall_Vanish( wk->p_ball[i], FACTORY_VANISH_OFF );

		//手放すの時
		if( wk->tebanasu_uketoru == 0 ){
			FactoryBall_Decide( wk->p_ball[i] );						//決定した状態へ
			FactoryBall_PalChg( wk->p_ball[i], PAL_BALL_MOVE );
			FactoryBall_AnmChg( wk->p_ball[i], ANM_BALL_STOP );
		}
	}

	return;
}

static void TradePokeInit( FACTORY_WORK* wk )
{
	Factory_PokeAdd(wk, 0, PokeParty_GetMemberPointer(wk->p_m_party,wk->psd_pos),
					TEST_X, TEST_Y, FLIP_TEBANASU );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	シーケンス：ポケモンステータス処理
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GamePsd( FACTORY_WORK* wk )
{
	switch( wk->sub_seq ){

	case 0:
		SDK_ASSERTMSG( wk->child_proc != NULL, "child_procがNULLです！" );

		if( ProcMain(wk->child_proc) == TRUE ){
			OS_Printf( "*********\nステータス終了\n************\n" );
			wk->psd_pos = wk->psd->pos;									//結果取得
			OS_Printf( "wk->psd_pos = %d\n", wk->psd_pos );
			sys_FreeMemoryEz( wk->psd );
			sys_FreeMemoryEz( wk->child_proc );
			wk->child_proc = NULL;
			Factory_Recover( wk );
			wk->psd_flag = 0;
			return TRUE;
		}
		break;
	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	ゲームレンタルポケモン選択中
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameRental( FACTORY_WORK* wk )
{
	int i;
	u32 ret;

	switch( wk->sub_seq ){

	case 0:
		OS_Printf( "*********\nゲームレンタル\n************\n" );

		//ポケモンステータスの復帰かによって飛び先を変更している
		if( wk->recover == 0 ){
			wk->sub_seq = 1;
		}else{
			wk->sub_seq = 2;
		}
		wk->recover = 0;
		break;

	//レンタルポケモンを選択中
	case 1:
		FactoryCsr_Move( wk->p_csr );			//カーソル移動処理

		//左右キーが押されたら表示しているポケモンを変更する
		if( sys.trg & (PAD_KEY_LEFT | PAD_KEY_RIGHT) ){
			Snd_SePlay( SEQ_SE_DP_SELECT );
			FactoryCommon_ChangePoke(	wk, wk->sel_count, 
										FactoryCsr_GetCsrPos(wk->p_csr), FLIP_RENTAL, 
										wk->p_m_party );
		}

		//決定ボタンを押したらメニュー作成
		if( sys.trg & PAD_BUTTON_A ){
			Snd_SePlay( SEQ_SE_DP_SELECT );
			FactoryCsr_Pause( wk->p_csr, 1 );	//カーソルポーズ
			Factory_SetMenu( wk );

			//カーソル生成
			wk->p_csr_menu = FactoryCsr_Create( &wk->factory_clact, 
												POKE_SEL_MENU_MAX, POKE_SEL_MENU_MAX,
												CSR_V_MODE, 0, bf_v_menu_csr_pos, NULL );
			wk->sub_seq++;

		}else if( sys.trg & PAD_BUTTON_B ){

			//キャンセルボタンを押したら1つ前に戻る
			if( wk->sel_count > 0 ){
				Snd_SePlay( SEQ_SE_DP_SELECT );
				SetBCancel( wk );

				//選んだポケモンナンバーを送信
				if( Factory_CommCheck(wk->type) == TRUE ){
					Factory_CommSetSendBuf( wk, FACTORY_COMM_PAIR_POKE, 0 );
				}

				return TRUE;					//注意！]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
			}
		}

		break;

	//メニュー処理
	case 2:
		ret = BmpMenuMain( wk->mw );
		FactoryCsr_Move( wk->p_csr_menu );		//カーソル移動処理

		switch( ret ){

		case BMPMENU_NULL:
			break;

		//「つよさをみる」
		case RET_PARAM_TUYOSA:
			BmpMenuExit( wk->mw, NULL );
			BmpMenuWinClearSub( wk->MenuH.win );
			FactoryCsr_Delete( wk->p_csr_menu );	//メニューカーソルOBJ削除
			wk->p_csr_menu = NULL;
			wk->psd_flag = 1;

			//フェードアウト
			WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
							WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_FACTORY );
			wk->sub_seq++;
			break;

		//「レンタルする」
		case RET_PARAM_RENTAL:
			BmpMenuExit( wk->mw, NULL );
			BmpMenuWinClearSub( wk->MenuH.win );
			SetRentarusuru( wk );

			//選んだポケモンナンバーを送信
			if( Factory_CommCheck(wk->type) == TRUE ){
				Factory_CommSetSendBuf( wk, FACTORY_COMM_PAIR_POKE, 0 );
			}

			return TRUE;						//注意！]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
			
		//「はずす」
		case RET_PARAM_HAZUSU:
			BmpMenuExit( wk->mw, NULL );
			BmpMenuWinClearSub( wk->MenuH.win );
			SetHazusu( wk );

			//選んだポケモンナンバーを送信
			if( Factory_CommCheck(wk->type) == TRUE ){
				Factory_CommSetSendBuf( wk, FACTORY_COMM_PAIR_POKE, 0 );
			}

			return TRUE;						//注意！]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

		//「べつのにする」・Bキャンセル
		case BMPMENU_CANCEL:
		case RET_PARAM_BETUNO:
		default:
			BmpMenuExit( wk->mw, NULL );
			BmpMenuWinClearSub( wk->MenuH.win );
			FactoryCsr_Pause( wk->p_csr, 0 );	//カーソルポーズ解除
			FactoryCsr_Delete( wk->p_csr_menu );//メニューカーソルOBJ削除
			wk->p_csr_menu = NULL;
			return TRUE;						//注意！]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
		};

		break;

	case 3:
		//フェード終了待ち
		if( WIPE_SYS_EndCheck() == TRUE ){
			Factory_SetPSD( wk );
			FactoryCommon_Delete( wk );			//削除処理
			wk->child_proc = PROC_Create( &PokeStatusProcData, wk->psd, HEAPID_FACTORY );
			wk->recover = 1;
			//wk->sub_seq++;
			return TRUE;
		}
		break;

	case 4:
		if( wk->child_proc == NULL ){
			return TRUE;
		}
		break;

	case 5:
		//フェード終了待ち
		if( WIPE_SYS_EndCheck() == TRUE ){
			wk->sub_seq = 2;					//注意！]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
			//return TRUE;
		}
		break;

	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	セット：Bキャンセルをした時の処理
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void SetBCancel( FACTORY_WORK* wk )
{
	//現在選択中のポケモン名を消す
	GF_BGL_BmpWinDataFill( &wk->bmpwin[BMPWIN_POKE1+wk->sel_count], FBMP_COL_NULL );	//塗りつぶし
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_POKE1+wk->sel_count] );

	//1つ前に選択したポケモン名を消して、現在選択中のポケモン名に書き換える
	wk->sel_count--;
	GF_BGL_BmpWinDataFill( &wk->bmpwin[BMPWIN_POKE1+wk->sel_count], FBMP_COL_NULL );	//塗りつぶし
	PokeNameWrite(	wk, &wk->bmpwin[BMPWIN_POKE1+wk->sel_count], 
					FactoryCsr_GetCsrPos(wk->p_csr), 0, 0, 
					//FBMP_COL_BLACK, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, wk->p_m_party );
					FBMP_COL_WHITE, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, wk->p_m_party );

	//「○ばんめの　ポケモンを　えらんで　ください」
	Factory_SetNumber( wk, 0, (wk->sel_count+1) );
	wk->msg_index = Factory_EasyMsg( wk, msg_rental_01 );

	FactoryBall_Cancel( wk->p_ball[ wk->sel_work[wk->sel_count] ] );	//キャンセル状態
	FactoryBall_PalChg( wk->p_ball[ wk->sel_work[wk->sel_count] ], PAL_BALL_STOP );
	FactoryBall_AnmChg( wk->p_ball[ wk->sel_work[wk->sel_count] ], ANM_BALL_STOP );

	wk->sel_work[wk->sel_count] = 0;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	セット：「レンタルする」を選んだ時の処理
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void SetRentarusuru( FACTORY_WORK* wk )
{
	FactoryBall_Decide( wk->p_ball[ FactoryCsr_GetCsrPos(wk->p_csr) ] );	//決定した状態へ
	FactoryBall_PalChg( wk->p_ball[ FactoryCsr_GetCsrPos(wk->p_csr) ], PAL_BALL_MOVE );
	FactoryBall_AnmChg( wk->p_ball[ FactoryCsr_GetCsrPos(wk->p_csr) ], ANM_BALL_MOVE );

	wk->sel_work[wk->sel_count] = FactoryCsr_GetCsrPos( wk->p_csr );

	wk->sel_count++;

	if( wk->sel_count == Factory_GetRentalSelPokeNum(wk->type) ){
		FactoryCsr_Delete( wk->p_csr_menu );		//メニューカーソルOBJ削除
		wk->p_csr_menu = NULL;
	}else{
		FactoryCsr_Pause( wk->p_csr, 0 );			//カーソルポーズ解除
		FactoryCsr_Delete( wk->p_csr_menu );		//メニューカーソルOBJ削除
		wk->p_csr_menu = NULL;

		PokeNameWrite(	wk, &wk->bmpwin[BMPWIN_POKE1+wk->sel_count], 
						FactoryCsr_GetCsrPos(wk->p_csr), 0, 0, 
						//FBMP_COL_BLACK, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, wk->p_m_party );
						FBMP_COL_WHITE, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, wk->p_m_party );

		//「○ばんめの　ポケモンを　えらんで　ください」
		Factory_SetNumber( wk, 0, (wk->sel_count+1) );
		wk->msg_index = Factory_EasyMsg( wk, msg_rental_01 );
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	セット：「はずす」を選んだ時の処理
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void SetHazusu( FACTORY_WORK* wk )
{
	u8 count;
	int i;

	FactoryBall_Cancel( wk->p_ball[ FactoryCsr_GetCsrPos(wk->p_csr) ] );	//キャンセル状態
	FactoryBall_PalChg( wk->p_ball[ FactoryCsr_GetCsrPos(wk->p_csr) ], PAL_BALL_STOP );
	FactoryBall_AnmChg( wk->p_ball[ FactoryCsr_GetCsrPos(wk->p_csr) ], ANM_BALL_STOP );

#if 1
	//-----------------------------------------------------
	//ポケモン選択が4匹以上になった時は修正しないとダメ！)
	//-----------------------------------------------------
	
	//すでに2匹選んでいる時
	if( wk->sel_count >= 2 ){

		//1匹目を外そうとしていたら
		if( wk->sel_work[0] == FactoryCsr_GetCsrPos(wk->p_csr) ){

			//2匹目を1匹目に代入する！
			wk->sel_work[0] = wk->sel_work[1];

			//ポケモンを消す
			SoftSpriteDel( wk->ss[0] );

			//ポケモン表示
			Factory_PokeAdd(wk, 0, 
						PokeParty_GetMemberPointer(wk->p_m_party,FactoryCsr_GetCsrPos(wk->p_csr)),
						TEST_X, TEST_Y, FLIP_RENTAL );
		}
	}
#endif

	wk->sel_count--;
	wk->sel_work[wk->sel_count] = 0;

	FactoryCsr_Pause( wk->p_csr, 0 );			//カーソルポーズ解除
	FactoryCsr_Delete( wk->p_csr_menu );		//メニューカーソルOBJ削除
	wk->p_csr_menu = NULL;

	//レンタル数を取得して、ポケモン名ウィンドウを塗りつぶし
	count = Factory_GetRentalSelPokeNum( wk->type );
	for( i=0; i < count ;i++ ){
		GF_BGL_BmpWinDataFill( &wk->bmpwin[BMPWIN_POKE1+i], FBMP_COL_NULL );
	}

	//選択している数、ポケモン名を表示
	for( i=0; i <  wk->sel_count ;i++ ){
		PokeNameWrite(	wk, &wk->bmpwin[BMPWIN_POKE1+i], 
						wk->sel_work[i], 0, 0, 
						//FBMP_COL_BLACK, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, wk->p_m_party );
						FBMP_COL_WHITE, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, wk->p_m_party );
	}

	//現在選択中のポケモン名を表示
	PokeNameWrite(	wk, &wk->bmpwin[BMPWIN_POKE1+wk->sel_count], 
					FactoryCsr_GetCsrPos(wk->p_csr), 0, 0,
					//FBMP_COL_BLACK, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, wk->p_m_party );
					FBMP_COL_WHITE, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, wk->p_m_party );

	//反映
	for( i=0; i < count ;i++ ){
		GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_POKE1+i] );
	}

	//「○ばんめの　ポケモンを　えらんで　ください」
	Factory_SetNumber( wk, 0, (wk->sel_count+1) );
	wk->msg_index = Factory_EasyMsg( wk, msg_rental_01 );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	シーケンス：以上の3匹でよろしいですか？
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameFinalAnswer( FACTORY_WORK* wk )
{
	u8 m_max;
	u32 ret;
	int i;
	int x_tbl[3];

	enum{
		SEQ_FINAL_ANSWER_START = 0,
		SEQ_FINAL_ANSWER_VANISH_TIMING,
		SEQ_FINAL_ANSWER_POKE_FADEOUT,
		SEQ_FINAL_ANSWER_POKE_APPEAR_TIMING,
		SEQ_FINAL_ANSWER_POKE_FADEIN,
		SEQ_FINAL_ANSWER_MENU,
		SEQ_FINAL_ANSWER_SEND,
		SEQ_FINAL_ANSWER_TIMING_SYNC,
	};

	m_max = Factory_GetMinePokeNum( wk->type );

	switch( wk->sub_seq ){

	//最終確認スタート
	case SEQ_FINAL_ANSWER_START:

		FactoryCsr_Vanish( wk->p_csr, FACTORY_VANISH_ON );		//バニッシュ

		//ペアのデータを表示しないフラグON
		wk->pair_data_off_flag = 1;
		GF_BGL_BmpWinOff( &wk->bmpwin[BMPWIN_TR2] );
		GF_BGL_BmpWinOff( &wk->bmpwin[BMPWIN_POKE4] );
		GF_BGL_BmpWinOff( &wk->bmpwin[BMPWIN_POKE5] );

#if 1
		//ポケモン名を消す
		for( i=0; i < wk->sel_count; i++ ){
			GF_BGL_BmpWinDataFill( &wk->bmpwin[BMPWIN_POKE1+i], FBMP_COL_NULL );	//抜け色
			GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_POKE1+i] );
		}

		//プレイヤー名を消す
		GF_BGL_BmpWinDataFill( &wk->bmpwin[BMPWIN_TR1], FBMP_COL_NULL );			//抜け色
		GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_TR1] );
#endif

		ScrPalChg( wk, BF_FRAME_BG, Factory_GetScrPalNo(wk, ID_PAL_SINGLE_FLASH, 0), 21, 11 );
		SoftSpritePalFadeSetAll( wk->soft_sprite, 0, 16, 0, 0xffff );		//フェードアウトして白

		wk->wait_count = 0;
		wk->sub_seq++;
		break;

	//BG光る、ポケモンホワイトアウト終わったら、背景を切り替える
	case SEQ_FINAL_ANSWER_VANISH_TIMING:
		wk->wait_count++;
		if( wk->wait_count < POKE_FLASH_WAIT ){
			break;
		}

		SoftSpriteParaSet( wk->ss[0], SS_PARA_VANISH, 1 );					//非表示
		Factory_ObjWinSet( wk, ANM_SINGLE_CLOSE );	//閉じる
		ScrPalChg( wk, BF_FRAME_BG, Factory_GetScrPalNo(wk, ID_PAL_SINGLE_OPEN, 0), 21, 11 );

		//スクリーン切り替え
		Factory_SetRentalBackBgScrn( wk, BF_FRAME_BG );

		wk->wait_count = 0;													//クリア
		wk->sub_seq++;
		break;

	//ウィンドウアニメ終了したら、3匹の表示準備
	case SEQ_FINAL_ANSWER_POKE_FADEOUT:
		if( SoftSpritePalFadeExist(wk->ss[0]) ){							//TRUE = フェード中
			//現状特になし
		}

		//左上のOBJウィンドウのアニメショーン終了まで待つ
		if( FactoryObj_AnmActiveCheck(wk->p_obj_win) == TRUE ){
			break;
		}
		Factory_ObjWinDel( wk );											//OBJウィンドウ削除

		//左から選んだ順番にポケモンを表示する
		SoftSpriteDel( wk->ss[0] );
		x_tbl[0] = TEST_X1;
		x_tbl[1] = TEST_X2;
		x_tbl[2] = TEST_X3;

		//全ての選択したポケモンを表示
		for( i=0; i < m_max ;i++ ){
			Factory_PokeAdd(wk, i, PokeParty_GetMemberPointer(wk->p_m_party,wk->sel_work[i]),
							x_tbl[i], TEST_Y, FLIP_RENTAL );
			SoftSpriteParaSet( wk->ss[i], SS_PARA_VANISH, 1 );				//非表示
		}

		Factory_ObjWinSet( wk, ANM_3POKE_OPEN );	//開く
		wk->sub_seq++;
		break;

	//ポケモン表示
	case SEQ_FINAL_ANSWER_POKE_APPEAR_TIMING:
		//左上のOBJウィンドウのアニメショーン終了まで待つ
		if( FactoryObj_AnmActiveCheck(wk->p_obj_win) == TRUE ){
			break;
		}
		Factory_ObjWinDel( wk );											//OBJウィンドウ削除
		wk->wait_count = 0;													//クリア
		wk->sub_seq++;
		break;

	//ポケモンフェードイン中
	case SEQ_FINAL_ANSWER_POKE_FADEIN:
		if( wk->wait_count == 0 ){

			//スクリーン切り替え
			Factory_Set3PokeBgScrn( wk, BF_FRAME_BG );

			//全ての選択したポケモンを表示
			for( i=0; i < m_max ;i++ ){
				SoftSpriteParaSet( wk->ss[i], SS_PARA_VANISH, 0 );			//表示
			}

			if( Factory_CommCheck(wk->type) == FALSE ){
				ScrPalChg( wk, BF_FRAME_BG, Factory_GetScrPalNo(wk,ID_PAL_SINGLE_FLASH,1), 32, 11 );
			}else{
				ScrPalChg(wk,BF_FRAME_BG, Factory_GetScrPalNo(wk, ID_PAL_SINGLE_FLASH, 1), 21, 11 );
			}
			SoftSpritePalFadeSetAll( wk->soft_sprite, 16, 0, 1, 0xffff );	//白からフェードイン
		}

		if( SoftSpritePalFadeExist(wk->ss[0]) ){							//TRUE = フェード中
			//現状特になし
		}

		wk->wait_count++;
		if( wk->wait_count < POKE_APPEAR_WAIT ){
			break;
		}

		if( Factory_CommCheck(wk->type) == FALSE ){
			ScrPalChg( wk, BF_FRAME_BG, Factory_GetScrPalNo(wk,ID_PAL_SINGLE_NORMAL,1), 32, 11 );
		}else{
			ScrPalChg( wk, BF_FRAME_BG, Factory_GetScrPalNo(wk, ID_PAL_SINGLE_NORMAL, 1), 21, 11);
		}

		//ペアのデータを表示しないフラグOFF
		wk->pair_data_off_flag = 0;

		//パートナーの選んだポケモンを表示リクエスト
		if( Factory_CommCheck(wk->type) == TRUE ){
			wk->pair_monsno_req = REQ_PAIR_MONSNO_DECIDE;
			Factory_PairMonsNoRecvReq( wk );
		}

		//「いじょうの　３ひきで　よろしいですか？」
		Factory_SetNumber( wk, 0, m_max );
		wk->msg_index = Factory_EasyMsg( wk, msg_rental_02 );
		Factory_SetMenu2( wk );							//「はい、いいえ」
		wk->p_csr_menu = FactoryCsr_Create( &wk->factory_clact, 
											YES_NO_MENU_MAX, YES_NO_MENU_MAX,
											CSR_V_MODE, 0, bf_v_yesno_csr_pos, NULL );
		wk->wait_count = 0;
		wk->sub_seq++;
		break;

	//メニュー処理
	case SEQ_FINAL_ANSWER_MENU:
		ret = BmpMenuMain( wk->mw );
		FactoryCsr_Move( wk->p_csr_menu );				//カーソル移動処理

		switch( ret ){

		case BMPMENU_NULL:
			break;

		//「はい」
		case RET_PARAM_YES:
			BmpMenuExit( wk->mw, NULL );
			BmpMenuWinClearSub( wk->MenuH.win );
			FactoryCsr_Delete( wk->p_csr_menu );		//メニューカーソルOBJ削除
			wk->p_csr_menu = NULL;
			wk->sub_seq++;
			break;

		//「いいえ」・Bキャンセル
		case BMPMENU_CANCEL:
		case RET_PARAM_NO:
		default:
			FactoryCsr_Vanish( wk->p_csr, FACTORY_VANISH_OFF );	
			BmpMenuExit( wk->mw, NULL );
			BmpMenuWinClearSub( wk->MenuH.win );
			FactoryCsr_Delete( wk->p_csr_menu );		//メニューカーソルOBJ削除
			wk->p_csr_menu = NULL;
			wk->sel_count--;
			wk->sel_work[wk->sel_count] = 0;

			//選んだポケモンナンバーを送信
			if( Factory_CommCheck(wk->type) == TRUE ){
				Factory_CommSetSendBuf( wk, FACTORY_COMM_PAIR_POKE, 0 );
			}

			return TRUE;						//注意！]]]]]]]]]]]]]]]]]]]]]]]]]]]
		};

		break;

	//
	case SEQ_FINAL_ANSWER_SEND:
		//通信タイプでない時は終了
		if( Factory_CommCheck(wk->type) == FALSE ){
			return TRUE;
		}

		//送信成功したら
		if( Factory_CommSetSendBuf(wk,FACTORY_COMM_PAIR_POKE,0) == TRUE ){

			//メッセージ表示
			wk->msg_index = Factory_EasyMsg( wk, msg_rental_03 );

			//同期開始
			//CommToolInitialize( HEAPID_FACTORY );	//timingSyncEnd=0xff
			CommToolTempDataReset();
			CommTimingSyncStart( DBC_TIM_FACTORY_RENTAL_END );
			wk->sub_seq++;
		}
		break;

	//同期待ち
	case SEQ_FINAL_ANSWER_TIMING_SYNC:
		//タイミングコマンドが届いたか確認
		if( CommIsTimingSync(DBC_TIM_FACTORY_RENTAL_END) == TRUE ){
			CommToolTempDataReset();
			return TRUE;
		}
		break;

	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	シーケンス：以上の3匹でよろしいですか？(キャンセル)
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameFinalAnswerCancel( FACTORY_WORK* wk )
{
	u8 m_max;
	int i;

	enum{
		SEQ_FINAL_ANSWER_CANCEL_START = 0,
		SEQ_FINAL_ANSWER_CANCEL_VANISH_TIMING,
		SEQ_FINAL_ANSWER_CANCEL_POKE_FADEOUT,
		SEQ_FINAL_ANSWER_CANCEL_POKE_APPEAR_TIMING,
		SEQ_FINAL_ANSWER_CANCEL_POKE_FADEIN,
	};

	m_max = Factory_GetMinePokeNum( wk->type );

	switch( wk->sub_seq ){

	//最終確認スタート
	case SEQ_FINAL_ANSWER_CANCEL_START:
		//ペアのデータを表示しないフラグON
		wk->pair_data_off_flag = 1;
		GF_BGL_BmpWinOff( &wk->bmpwin[BMPWIN_TR2] );
		GF_BGL_BmpWinOff( &wk->bmpwin[BMPWIN_POKE4] );
		GF_BGL_BmpWinOff( &wk->bmpwin[BMPWIN_POKE5] );

		if( Factory_CommCheck(wk->type) == FALSE ){
			ScrPalChg( wk, BF_FRAME_BG, Factory_GetScrPalNo(wk, ID_PAL_SINGLE_FLASH, 1), 32, 11 );
		}else{
			ScrPalChg( wk, BF_FRAME_BG, Factory_GetScrPalNo(wk, ID_PAL_SINGLE_FLASH, 1), 21, 11 );
		}
		SoftSpritePalFadeSetAll( wk->soft_sprite, 0, 16, 0, 0xffff );		//フェードアウトして白
		wk->wait_count = 0;
		wk->sub_seq++;
		break;

	//BG光る、ポケモンホワイトアウト終わったら、背景を切り替える
	case SEQ_FINAL_ANSWER_CANCEL_VANISH_TIMING:
		wk->wait_count++;
		if( wk->wait_count < POKE_FLASH_WAIT ){
			break;
		}

		//全ての選択したポケモンを削除
		for( i=0; i < m_max ;i++ ){
			SoftSpriteDel( wk->ss[i] );
		}

		Factory_ObjWinSet( wk, ANM_3POKE_CLOSE );	//閉じる
		if( Factory_CommCheck(wk->type) == FALSE ){
			ScrPalChg( wk, BF_FRAME_BG, Factory_GetScrPalNo(wk, ID_PAL_SINGLE_OPEN, 0), 32, 11 );
		}else{
			ScrPalChg( wk, BF_FRAME_BG, Factory_GetScrPalNo(wk, ID_PAL_SINGLE_OPEN, 0), 21, 11 );
		}

		//スクリーン切り替え
		Factory_SetRentalBackBgScrn( wk, BF_FRAME_BG );

		wk->wait_count = 0;													//クリア
		wk->sub_seq++;
		break;

	//ウィンドウアニメ終了したら、3匹の表示準備
	case SEQ_FINAL_ANSWER_CANCEL_POKE_FADEOUT:
		if( SoftSpritePalFadeExist(wk->ss[0]) ){							//TRUE = フェード中
			//現状特になし
		}

		//左上のOBJウィンドウのアニメショーン終了まで待つ
		if( FactoryObj_AnmActiveCheck(wk->p_obj_win) == TRUE ){
			break;
		}
		Factory_ObjWinDel( wk );											//OBJウィンドウ削除

		//現在選択しているポケモンを表示
		Factory_PokeAdd(wk, 0, 
						PokeParty_GetMemberPointer(wk->p_m_party,FactoryCsr_GetCsrPos(wk->p_csr)),
						TEST_X, TEST_Y, FLIP_RENTAL );
		SoftSpriteParaSet( wk->ss[0], SS_PARA_VANISH, 1 );					//非表示

		Factory_ObjWinSet( wk, ANM_SINGLE_OPEN );	//開く
		wk->sub_seq++;
		break;

	//ポケモン表示
	case SEQ_FINAL_ANSWER_CANCEL_POKE_APPEAR_TIMING:
		//左上のOBJウィンドウのアニメショーン終了まで待つ
		if( FactoryObj_AnmActiveCheck(wk->p_obj_win) == TRUE ){
			break;
		}
		Factory_ObjWinDel( wk );											//OBJウィンドウ削除
		wk->wait_count = 0;													//クリア
		wk->sub_seq++;
		break;

	//ポケモンフェードイン中
	case SEQ_FINAL_ANSWER_CANCEL_POKE_FADEIN:
		if( wk->wait_count == 0 ){
			Factory_SetMainBgScrn( wk, BF_FRAME_BG );
			SoftSpriteParaSet( wk->ss[0], SS_PARA_VANISH, 0 );				//表示
			ScrPalChg( wk, BF_FRAME_BG, Factory_GetScrPalNo(wk, ID_PAL_SINGLE_FLASH, 0), 21, 11 );
			SoftSpritePalFadeSetAll( wk->soft_sprite, 16, 0, 1, 0xffff );	//白からフェードイン
		}

		if( SoftSpritePalFadeExist(wk->ss[0]) ){							//TRUE = フェード中
			//現状特になし
		}

		wk->wait_count++;
		if( wk->wait_count < POKE_APPEAR_WAIT ){
			break;
		}
		ScrPalChg( wk, BF_FRAME_BG, Factory_GetScrPalNo(wk, ID_PAL_SINGLE_NORMAL, 0), 21, 11 );

		//ペアのデータを表示しないフラグOFF
		wk->pair_data_off_flag = 0;

		//パートナーの選んだポケモンを表示リクエスト
		if( Factory_CommCheck(wk->type) == TRUE ){
			wk->pair_monsno_req = REQ_PAIR_MONSNO_DECIDE;
			Factory_PairMonsNoRecvReq( wk );
		}

		SetFinalAnswerNo( wk );
		return TRUE;
		//break;

	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	セット：「以上の3匹でよろしいですか？」「いいえ」を選んだ時の処理
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void SetFinalAnswerNo( FACTORY_WORK* wk )
{
	int i;
	u8 m_max;

	m_max = Factory_GetMinePokeNum( wk->type );

	//ポケモン名を表示
	for( i=0; i < wk->sel_count; i++ ){
		PokeNameWrite(	wk, &wk->bmpwin[BMPWIN_POKE1+i], 
						wk->sel_work[i], 0, 0, 
						//FBMP_COL_BLACK, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, wk->p_m_party );
						FBMP_COL_WHITE, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, wk->p_m_party );
	}

	//選択中のポケモン名を表示
	PokeNameWrite(	wk, &wk->bmpwin[BMPWIN_POKE1+wk->sel_count], 
					FactoryCsr_GetCsrPos(wk->p_csr), 0, 0, 
					FBMP_COL_WHITE, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, wk->p_m_party );

	//プレイヤー名を表示
	PlayerNameWrite(wk, &wk->bmpwin[BMPWIN_TR1], 0, 0, BF_FONT );

	FactoryCsr_Pause( wk->p_csr, 0 );										//カーソルポーズ解除
	FactoryBall_Cancel( wk->p_ball[ FactoryCsr_GetCsrPos(wk->p_csr) ] );	//キャンセル状態へ
	FactoryBall_PalChg( wk->p_ball[ FactoryCsr_GetCsrPos(wk->p_csr) ], PAL_BALL_STOP );
	FactoryBall_AnmChg( wk->p_ball[ FactoryCsr_GetCsrPos(wk->p_csr) ], ANM_BALL_STOP );

	//「○ばんめの　ポケモンを　えらんで　ください」
	Factory_SetNumber( wk, 0, (wk->sel_count+1) );
	wk->msg_index = Factory_EasyMsg( wk, msg_rental_01 );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ゲーム交換手放すポケモン選択中
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameTradeTebanasu( FACTORY_WORK* wk )
{
	int i;
	u32 ret;

	switch( wk->sub_seq ){

	case 0:
		Factory_SetCancel( wk, 0 );

		//ポケモンステータスの復帰かによって飛び先を変更している
		if( wk->recover == 0 ){
			wk->sub_seq = 1;
		}else{
			wk->sub_seq = 2;
		}
		wk->recover = 0;
		break;

	//交換ポケモンを選択中
	case 1:
		FactoryCsr_Move( wk->p_csr );			//カーソル移動処理

		//上下左右キーが押されたら表示しているポケモンを変更する
		if( sys.trg & (PAD_KEY_LEFT | PAD_KEY_RIGHT | PAD_KEY_UP | PAD_KEY_DOWN) ){
			Snd_SePlay( SEQ_SE_DP_SELECT );
			FactoryCommon_ChangePoke(	wk, wk->sel_count, 
										FactoryCsr_GetCsrPos(wk->p_csr), FLIP_TEBANASU,
										wk->p_m_party );
		}

		//決定ボタンを押したらメニュー作成
		if( sys.trg & PAD_BUTTON_A ){

			Snd_SePlay( SEQ_SE_DP_SELECT );

			//「ちゅうし」を選んでいたら
			if( FactoryCsr_GetCsrPos(wk->p_csr) == (wk->tebanasu_max-1) ){
				Factory_SetCancel( wk, 1 );
				return TRUE;						//注意！]]]]]]]]]]]]]]]]]]]]]]]]]]]
			}else{
				FactoryCsr_Pause( wk->p_csr, 1 );	//カーソルポーズ
				Factory_SetMenu( wk );

				//カーソル生成
				wk->p_csr_menu = FactoryCsr_Create( &wk->factory_clact, 
													POKE_SEL_MENU_MAX, POKE_SEL_MENU_MAX, 
													CSR_V_MODE, 0, bf_v_menu_csr_pos, NULL );
				wk->sub_seq++;
			}

		}else if( sys.trg & PAD_BUTTON_B ){
			Snd_SePlay( SEQ_SE_DP_SELECT );
			Factory_SetCancel( wk, 1 );
			return TRUE;						//注意！]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
		}

		break;

	//メニュー処理
	case 2:
		ret = BmpMenuMain( wk->mw );
		FactoryCsr_Move( wk->p_csr_menu );		//カーソル移動処理

		switch( ret ){

		case BMPMENU_NULL:
			break;

		//「つよさをみる」
		case RET_PARAM_TUYOSA:
			BmpMenuExit( wk->mw, NULL );
			BmpMenuWinClearSub( wk->MenuH.win );
			FactoryCsr_Delete( wk->p_csr_menu );	//メニューカーソルOBJ削除
			wk->p_csr_menu = NULL;
			wk->psd_flag = 1;

			//フェードアウト
			WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
							WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_FACTORY );
			wk->sub_seq++;
			break;

		//「てばなす」
		case RET_PARAM_TEBANASU:
			BmpMenuExit( wk->mw, NULL );
			BmpMenuWinClearSub( wk->MenuH.win );
			SetTebanasu( wk );
			return TRUE;						//注意！]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
			
		//「べつのにする」・Bキャンセル
		case BMPMENU_CANCEL:
		case RET_PARAM_BETUNO:
		default:
			BmpMenuExit( wk->mw, NULL );
			BmpMenuWinClearSub( wk->MenuH.win );
			FactoryCsr_Pause( wk->p_csr, 0 );	//カーソルポーズ解除
			FactoryCsr_Delete( wk->p_csr_menu );//メニューカーソルOBJ削除
			wk->p_csr_menu = NULL;

			//右下メニュー表示「ちゅうし」
			FactoryWriteMenuWin( wk->bgl, &wk->bmpwin[BMPWIN_SEL] );
			wk->msg_index = FactoryWriteMsg(wk, &wk->bmpwin[BMPWIN_SEL], msg_f_change_choice_01, 
										1, 1, MSG_NO_PUT, 
										FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE, BF_FONT );
			GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_SEL] );

			wk->sub_seq = 1;					//注意！]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
			break;
			//return TRUE;						//注意！]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
		};

		break;

	case 3:
		//フェード終了待ち
		if( WIPE_SYS_EndCheck() == TRUE ){
			Factory_SetPSD( wk );
			FactoryCommon_Delete( wk );			//削除処理
			wk->child_proc = PROC_Create( &PokeStatusProcData, wk->psd, HEAPID_FACTORY );
			wk->recover = 1;
			//wk->sub_seq++;
			return TRUE;
		}
		break;

	case 4:
		if( wk->child_proc == NULL ){
			return TRUE;
		}
		break;

	case 5:
		//フェード終了待ち
		if( WIPE_SYS_EndCheck() == TRUE ){
			wk->sub_seq = 2;					//注意！]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
			//return TRUE;
		}
		break;
	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	「てばなす」を選んだ時の処理
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void SetTebanasu( FACTORY_WORK* wk )
{
	FactoryBall_Decide( wk->p_ball[ FactoryCsr_GetCsrPos(wk->p_csr) ] );	//決定した状態へ
	FactoryBall_PalChg( wk->p_ball[ FactoryCsr_GetCsrPos(wk->p_csr) ], PAL_BALL_MOVE );
	FactoryBall_AnmChg( wk->p_ball[ FactoryCsr_GetCsrPos(wk->p_csr) ], ANM_BALL_STOP );

	PokeNameWrite(	wk, &wk->bmpwin[BMPWIN_POKE1+wk->sel_count], 
					FactoryCsr_GetCsrPos(wk->p_csr), 0, 0, 
					//FBMP_COL_BLACK, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, wk->p_m_party );
					FBMP_COL_WHITE, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, wk->p_m_party );
	wk->sel_work[wk->sel_count] = FactoryCsr_GetCsrPos( wk->p_csr );
	wk->sel_count++;

	FactoryCsr_Delete( wk->p_csr_menu );								//メニューカーソルOBJ削除
	wk->p_csr_menu = NULL;
	FactoryCsr_Delete( wk->p_csr );										//カーソルOBJ削除
	wk->p_csr = NULL;

	//受け取るに切り替え
	wk->tebanasu_uketoru = 1;

	//ポケモン消す
	GF_BGL_BmpWinDataFill( &wk->bmpwin[BMPWIN_POKE1], FBMP_COL_NULL );	//塗りつぶし
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_POKE1] );
	//SoftSpriteDel( wk->ss[0] );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	シーケンス：手放す画面で交換中止する？
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameTradeTebanasuTyuusi( FACTORY_WORK* wk )
{
	int i,flag;
	u32 ret;

	switch( wk->sub_seq ){

	//準備
	case 0:
		//現在選択中のポケモン名を消す
		GF_BGL_BmpWinDataFill( &wk->bmpwin[BMPWIN_POKE1], FBMP_COL_NULL );	//塗りつぶし
		GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_POKE1+wk->sel_count] );
		SoftSpriteDel( wk->ss[0] );

		//「ポケモンの　こうかんを　ちゅうし　しますか？」
		wk->msg_index = Factory_EasyMsg( wk, msg_f_change_02 );

		FactoryCsr_Vanish( wk->p_csr, FACTORY_VANISH_ON );		//バニッシュ
		FactoryCsr_Pause( wk->p_csr, 1 );						//カーソルポーズ
		Factory_SetMenu2( wk );									//「はい、いいえ」
		wk->p_csr_menu = FactoryCsr_Create( &wk->factory_clact, 
											YES_NO_MENU_MAX, YES_NO_MENU_MAX,
											CSR_V_MODE, 0, bf_v_yesno_csr_pos, NULL );
		wk->sub_seq++;
		break;

	//メニュー処理
	case 1:
		ret = BmpMenuMain( wk->mw );
		FactoryCsr_Move( wk->p_csr_menu );				//カーソル移動処理

		switch( ret ){

		case BMPMENU_NULL:
			break;

		//「はい」
		case RET_PARAM_YES:
			BmpMenuExit( wk->mw, NULL );
			BmpMenuWinClearSub( wk->MenuH.win );
			FactoryCsr_Delete( wk->p_csr_menu );		//メニューカーソルOBJ削除
			wk->p_csr_menu = NULL;
			wk->trade_flag = 0;							//交換したフラグOFF

#if 1
			/*「しばらくおまちください」*/
			if( Factory_CommCheck(wk->type) == TRUE ){
				wk->msg_index = Factory_EasyMsg( wk, msg_rental_03 );
			}
#endif

			wk->sub_seq++;
			break;

		//「いいえ」・Bキャンセル
		case BMPMENU_CANCEL:
		case RET_PARAM_NO:
		default:
			BmpMenuExit( wk->mw, NULL );
			BmpMenuWinClearSub( wk->MenuH.win );
			FactoryCsr_Delete( wk->p_csr_menu );		//メニューカーソルOBJ削除
			wk->p_csr_menu = NULL;
			SetTebanasuTyuusiNo( wk );
			wk->sub_seq++;
			break;
		};

		break;

	case 2:
		return TRUE;
	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	セット：手放す画面で「ちゅうし」「いいえ」を選んだ時の処理
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void SetTebanasuTyuusiNo( FACTORY_WORK* wk )
{
	FactoryCsr_SetCsrPos( wk->p_csr, 0 );

	PokeNameWrite(	wk, &wk->bmpwin[BMPWIN_POKE1], 
					FactoryCsr_GetCsrPos(wk->p_csr), 0, 0, 
					//FBMP_COL_BLACK, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, wk->p_m_party );
					FBMP_COL_WHITE, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, wk->p_m_party );

	Factory_PokeAdd(wk, 0, 
					PokeParty_GetMemberPointer(wk->p_m_party,FactoryCsr_GetCsrPos(wk->p_csr)),
					TEST_X, TEST_Y, FLIP_TEBANASU );

	FactoryCsr_Vanish( wk->p_csr, FACTORY_VANISH_OFF );	
	FactoryCsr_Pause( wk->p_csr, 0 );					//カーソルポーズ解除
	Factory_SetCancel( wk, 1 );

	//右下メニュー表示「ちゅうし」
	FactoryWriteMenuWin( wk->bgl, &wk->bmpwin[BMPWIN_SEL] );
	wk->msg_index = FactoryWriteMsg(wk, &wk->bmpwin[BMPWIN_SEL], msg_f_change_choice_01, 
									1, 1, MSG_NO_PUT, 
									FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE, BF_FONT );
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_SEL] );

	//「てばなす　ポケモンを　えらんで　ください」
	wk->msg_index = Factory_EasyMsg( wk, msg_f_change_01 );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	シーケンス：受け取る画面でポケモン選択中
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameTradeUketoru( FACTORY_WORK* wk )
{
	int i,flag;
	u32 ret;
	u8 poke_pos;

	switch( wk->sub_seq ){

	case 0:
		//右下メニュー表示「もどる」「ちゅうし」(塗りつぶしなしを使用)
		FactoryWriteMenuWin( wk->bgl, &wk->bmpwin[BMPWIN_SEL] );
		wk->msg_index = FactoryWriteMsg(wk, &wk->bmpwin[BMPWIN_SEL], msg_f_change_choice_05, 
									1, 1, MSG_NO_PUT, 
									FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE, BF_FONT );
		wk->msg_index = FactoryWriteMsgSimple(wk, &wk->bmpwin[BMPWIN_SEL], msg_f_change_choice_06, 
									1, 1+16, MSG_NO_PUT, 
									FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE, BF_FONT );
		GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_SEL] );

		//カーソル生成
		if( Factory_CommCheck(wk->type) == FALSE ){
			wk->p_csr = FactoryCsr_Create(	&wk->factory_clact, 
											wk->uketoru_max, BF_HV_MODORU_H_MAX, 
											CSR_HV_MODE, 0, 
											bf_hv_modoru_csr_pos, bf_hv_modoru_anm_tbl );
		}else{
			wk->p_csr = FactoryCsr_Create(	&wk->factory_clact, 
											wk->uketoru_max, BF_HV_MODORU_H_MULTI_MAX, 
											CSR_HV_MODE, 0, 
											bf_hv_modoru_multi_csr_pos, bf_hv_modoru_multi_anm_tbl);
		}

		//ポケモン表示
		poke_pos = FactoryCsr_GetCsrPos(wk->p_csr);
		PokeNameWrite(	wk, &wk->bmpwin[BMPWIN_POKE1], 
						poke_pos, 0, 0, 
						//FBMP_COL_BLACK, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, wk->p_e_party );
						FBMP_COL_WHITE, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, wk->p_e_party );

		//「うけとる　ポケモンを　えらんで　ください」
		FactoryTalkWinPut( &wk->bmpwin[BMPWIN_TALK], CONFIG_GetWindowType(wk->config) );
		wk->msg_index = Factory_EasyMsg( wk, msg_f_change_05 );
		wk->sub_seq++;
		break;

	//受け取るポケモンを選択中
	case 1:
		FactoryCsr_Move( wk->p_csr );			//カーソル移動処理

		//上下キー
		if( sys.trg & (PAD_KEY_UP | PAD_KEY_DOWN) ){
			Snd_SePlay( SEQ_SE_DP_SELECT );
            // MatchComment: add this call
			FactoryCommon_ChangePoke(	wk, 0, FactoryCsr_GetCsrPos(wk->p_csr), 
										FLIP_UKETORU, wk->p_e_party );
		}

		//左右キーが押されたら表示しているポケモンを変更する
		if( sys.trg & (PAD_KEY_LEFT | PAD_KEY_RIGHT) ){
			Snd_SePlay( SEQ_SE_DP_SELECT );
			FactoryCommon_ChangePoke(	wk, 0, FactoryCsr_GetCsrPos(wk->p_csr), 
										FLIP_UKETORU, wk->p_e_party );
		}

		//決定ボタンを押したらメニュー作成
		if( sys.trg & PAD_BUTTON_A ){

			Snd_SePlay( SEQ_SE_DP_SELECT );

			//「ちゅうし」を選んでいたら(もしくは、Bキャンセル)
			if( FactoryCsr_GetCsrPos(wk->p_csr) == (wk->uketoru_max-1) ){
				Factory_SetCancel( wk, 1 );
				return TRUE;						//注意！]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

			//「もどる」を選んでいたら
			}else if( FactoryCsr_GetCsrPos(wk->p_csr) == (wk->uketoru_max-2) ){
				SetModoru( wk );
				return TRUE;						//注意！]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
			}else{
				FactoryCsr_Pause( wk->p_csr, 1 );	//カーソルポーズ
				Factory_SetMenu2( wk );				//「はい、いいえ」
				wk->p_csr_menu = FactoryCsr_Create( &wk->factory_clact, 
													YES_NO_MENU_MAX, YES_NO_MENU_MAX, 
													CSR_V_MODE, 0, bf_v_yesno_csr_pos, NULL );

				//「この　ポケモンを　うけとりますか？」
				wk->msg_index = Factory_EasyMsg( wk, msg_f_change_06 );
				wk->sub_seq++;
			}

		}else if( sys.trg & PAD_BUTTON_B ){
			Snd_SePlay( SEQ_SE_DP_SELECT );
			Factory_SetCancel( wk, 1 );
			return TRUE;							//注意！]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
		}

		break;

	//メニュー処理
	case 2:
		ret = BmpMenuMain( wk->mw );
		FactoryCsr_Move( wk->p_csr_menu );			//カーソル移動処理

		switch( ret ){

		case BMPMENU_NULL:
			break;

		//「はい」
		case RET_PARAM_YES:
			BmpMenuExit( wk->mw, NULL );
			BmpMenuWinClearSub( wk->MenuH.win );
			SetUketoru( wk );

#if 1
			/*「しばらくおまちください」*/
			if( Factory_CommCheck(wk->type) == TRUE ){
				wk->msg_index = Factory_EasyMsg( wk, msg_rental_03 );
			}
#endif

			wk->sub_seq++;
			break;

		//「いいえ」・Bキャンセル
		case BMPMENU_CANCEL:
		case RET_PARAM_NO:
		default:
			BmpMenuExit( wk->mw, NULL );
			BmpMenuWinClearSub( wk->MenuH.win );
			SetUketoruNo( wk );
			wk->sub_seq = 1;						//注意！]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
			break;
		};

		break;

	case 3:
		return TRUE;
	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	セット：受け取る画面で「もどる」を選んだ時の処理
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void SetModoru( FACTORY_WORK* wk )
{
	FactoryCsr_Delete( wk->p_csr );								//カーソルOBJ削除
	wk->p_csr = NULL;

	//てばなすに切り替え
	wk->tebanasu_uketoru = 0;

	//ポケモン消す
	GF_BGL_BmpWinDataFill( &wk->bmpwin[BMPWIN_POKE1], FBMP_COL_NULL );	//塗りつぶし
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_POKE1] );
	SoftSpriteDel( wk->ss[0] );

	//右下メニューを消す
	GF_BGL_BmpWinDataFill( &wk->bmpwin[BMPWIN_SEL], FBMP_COL_WHITE );	//塗りつぶし
	BmpTalkWinClear( &wk->bmpwin[BMPWIN_SEL], WINDOW_TRANS_OFF );
	GF_BGL_BmpWinOffVReq( &wk->bmpwin[BMPWIN_SEL] );

	wk->sel_count--;
	wk->sel_work[0] = 0;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	セット：受け取る画面で「はい」を選んだ時の処理
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void SetUketoru( FACTORY_WORK* wk )
{
	wk->trade_flag = 1;													//交換したフラグON

	wk->sel_work[wk->sel_count] = FactoryCsr_GetCsrPos( wk->p_csr );
	wk->sel_count++;

	FactoryCsr_Delete( wk->p_csr_menu );								//メニューカーソルOBJ削除
	wk->p_csr_menu = NULL;
	FactoryCsr_Vanish( wk->p_csr, FACTORY_VANISH_ON );					//バニッシュ

	//ポケモン消す
	GF_BGL_BmpWinDataFill( &wk->bmpwin[BMPWIN_POKE1], FBMP_COL_NULL );	//塗りつぶし
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_POKE1] );
	SoftSpriteDel( wk->ss[0] );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	セット：受け取る画面で「いいえ」を選んだ時の処理
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void SetUketoruNo( FACTORY_WORK* wk )
{
	FactoryCsr_Delete( wk->p_csr_menu );	//メニューカーソルOBJ削除
	wk->p_csr_menu = NULL;
	FactoryCsr_Pause( wk->p_csr, 0 );		//カーソル解除

	//右下メニュー表示「もどる」「ちゅうし」(塗りつぶしなしを使用)
	FactoryWriteMenuWin( wk->bgl, &wk->bmpwin[BMPWIN_SEL] );
	wk->msg_index = FactoryWriteMsg(wk, &wk->bmpwin[BMPWIN_SEL], msg_f_change_choice_05, 
									1, 1, MSG_NO_PUT, 
									FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE, BF_FONT );
	wk->msg_index = FactoryWriteMsgSimple(wk,&wk->bmpwin[BMPWIN_SEL],msg_f_change_choice_06,
										1, 1+16, MSG_NO_PUT, 
										FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE, BF_FONT );
	GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_SEL] );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	シーケンス：受け取る画面で交換中止する？
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameTradeUketoruTyuusi( FACTORY_WORK* wk )
{
	int i,flag;
	u32 ret;

	switch( wk->sub_seq ){

	//準備
	case 0:
		//現在選択中のポケモンを消す
		GF_BGL_BmpWinDataFill( &wk->bmpwin[BMPWIN_POKE1], FBMP_COL_NULL );	//塗りつぶし
		GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_POKE1] );
		SoftSpriteDel( wk->ss[0] );

		//「ポケモンの　こうかんを　ちゅうし　しますか？」
		FactoryTalkWinPut( &wk->bmpwin[BMPWIN_TALK], CONFIG_GetWindowType(wk->config) );
		wk->msg_index = Factory_EasyMsg( wk, msg_f_change_02 );

		//次の選択肢「はい、いいえ」で消すか、生きてるか、決まるのでここはバニッシュ
		FactoryCsr_Vanish( wk->p_csr, FACTORY_VANISH_ON );		//バニッシュ

		Factory_SetMenu2( wk );									//「はい、いいえ」
		wk->p_csr_menu = FactoryCsr_Create( &wk->factory_clact, 
											YES_NO_MENU_MAX, YES_NO_MENU_MAX,
											CSR_V_MODE, 0, bf_v_yesno_csr_pos, NULL );
		wk->sub_seq++;
		break;

	//メニュー処理「はい、いいえ」
	case 1:
		ret = BmpMenuMain( wk->mw );
		FactoryCsr_Move( wk->p_csr_menu );		//カーソル移動処理

		switch( ret ){

		case BMPMENU_NULL:
			break;

		//「はい」
		case RET_PARAM_YES:
			BmpMenuExit( wk->mw, NULL );
			BmpMenuWinClearSub( wk->MenuH.win );
			FactoryCsr_Delete( wk->p_csr_menu );				//メニューカーソルOBJ削除
			wk->p_csr_menu = NULL;
			wk->trade_flag = 0;									//交換したフラグOFF

#if 1
			/*「しばらくおまちください」*/
			if( Factory_CommCheck(wk->type) == TRUE ){
				wk->msg_index = Factory_EasyMsg( wk, msg_rental_03 );
			}
#endif

			wk->sub_seq++;
			break;

		//「いいえ」・Bキャンセル
		case BMPMENU_CANCEL:
		case RET_PARAM_NO:
		default:
			BmpMenuExit( wk->mw, NULL );
			//BmpMenuWinClearSub( wk->MenuH.win );				//次を描画するまでに間があるので。
			FactoryCsr_Delete( wk->p_csr );						//カーソルOBJ削除
			wk->p_csr = NULL;
			FactoryCsr_Delete( wk->p_csr_menu );				//メニューカーソルOBJ削除
			wk->p_csr_menu = NULL;
			SetUketoruTyuusiNo( wk );
			wk->sub_seq++;
			break;
		};

		break;

	case 2:
		return TRUE;
	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	セット：受け取る画面で「ちゅうし」→「いいえ」を選んだ時の処理
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void SetUketoruTyuusiNo( FACTORY_WORK* wk )
{
	Factory_PokeAdd(wk, 0, 
					PokeParty_GetMemberPointer(wk->p_e_party,0),
					TEST_X, TEST_Y, FLIP_UKETORU );

	Factory_SetCancel( wk, 1 );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	シーケンス：入場、退場
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameLeavingEntrance( FACTORY_WORK* wk )
{
	u8 m_max;
	int i,flag,x,y;
	VecFx32 vec;
	const VecFx32* p_vec;

	enum{
		SEQ_LEAVING_ENTRANCE_START = 0,
		SEQ_LEAVING_ENTRANCE_VANISH_TIMING,
		SEQ_LEAVING_ENTRANCE_POKE_FADEOUT,

		SEQ_LEAVING_ENTRANCE_RAIL_MOVE_OUT,
		SEQ_LEAVING_ENTRANCE_RAIL_MOVE_IN,

		SEQ_LEAVING_ENTRANCE_POKE_APPEAR_TIMING,
		SEQ_LEAVING_ENTRANCE_POKE_FADEIN,
	};

	m_max = Factory_GetMinePokeNum( wk->type );

	switch( wk->sub_seq ){

	//スタート
	case SEQ_LEAVING_ENTRANCE_START:
		//ペアのデータを表示しないフラグON
		wk->pair_data_off_flag = 1;
		GF_BGL_BmpWinOff( &wk->bmpwin[BMPWIN_TR2] );
		GF_BGL_BmpWinOff( &wk->bmpwin[BMPWIN_POKE4] );
		GF_BGL_BmpWinOff( &wk->bmpwin[BMPWIN_POKE5] );

		//プレイヤー名を消す
		GF_BGL_BmpWinDataFill( &wk->bmpwin[BMPWIN_TR1], FBMP_COL_NULL );	//抜け色
		GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_TR1] );

		//会話ウィンドウ削除
		BmpTalkWinClear( &wk->bmpwin[BMPWIN_TALK], WINDOW_TRANS_OFF );
		GF_BGL_BmpWinOffVReq( &wk->bmpwin[BMPWIN_TALK] );

		FactoryExitBmpWin( wk->bmpwin );									//BMPウィンドウ開放

		ScrPalChg( wk, BF_FRAME_BG, Factory_GetScrPalNo(wk, ID_PAL_SINGLE_FLASH, 0), 21, 11 );
		SoftSpritePalFadeSetAll( wk->soft_sprite, 0, 16, 0, 0xffff );		//フェードアウトして白

		wk->wait_count = 0;
		wk->sub_seq++;
		break;

	//BG光る、ポケモンホワイトアウト終わったら、背景を切り替える
	case SEQ_LEAVING_ENTRANCE_VANISH_TIMING:
		wk->wait_count++;
		if( wk->wait_count < POKE_FLASH_WAIT ){
			break;
		}

		SoftSpriteDel( wk->ss[0] );

		Factory_ObjWinSet( wk, ANM_SINGLE_CLOSE );	//閉じる
		ScrPalChg( wk, BF_FRAME_BG, Factory_GetScrPalNo(wk, ID_PAL_SINGLE_OPEN, 0), 21, 11 );

		//スクリーン切り替え
		Factory_SetRentalBackBgScrn( wk, BF_FRAME_BG );

		wk->wait_count = 0;													//クリア
		wk->sub_seq++;
		break;

	//ウィンドウアニメ終了待ち
	case SEQ_LEAVING_ENTRANCE_POKE_FADEOUT:
		if( SoftSpritePalFadeExist(wk->ss[0]) ){							//TRUE = フェード中
			//現状特になし
		}

		//左上のOBJウィンドウのアニメショーン終了まで待つ
		if( FactoryObj_AnmActiveCheck(wk->p_obj_win) == TRUE ){
			break;
		}
		Factory_ObjWinDel( wk );											//OBJウィンドウ削除

		Factory_SetMainBgGraphic_Maru( wk, BF_FRAME_WIN );					//右上の円
		Factory_SetMainBgGraphic_Rail( wk, BF_FRAME_SLIDE );				//レール
		Factory_SetMainBgGraphic_Back( wk, BF_FRAME_BG );					//背景

		//スクロール位置の調整
		GF_BGL_ScrollSet( wk->bgl, BF_FRAME_SLIDE, GF_BGL_SCROLL_X_SET, wk->scr_x );

		Snd_SePlay( SEQ_SE_DP_ELEBETA2 );
		wk->wait_count = 0;
		wk->sub_seq++;
		break;

	//退場
	case SEQ_LEAVING_ENTRANCE_RAIL_MOVE_OUT:
		if( RailMoveOut(wk) == TRUE ){
			Snd_SeStopBySeqNo( SEQ_SE_DP_ELEBETA2, 0 );
			Snd_SePlay( SEQ_SE_DP_KASYA );

			//値が一番小さいパレットは6で、初期パレットが8になっている
			wk->maru_wait		= ( FACTORY_MARU_WAIT * 2 );

			//全てのボールが退場したら
			for( i=0; i < wk->ball_max; i++ ){
				wk->p_ball[i] = FactoryBall_Delete( wk->p_ball[i] );		//削除
				wk->p_ball[i] = NULL;
			}

			//通信の時は、てばなす、うけとるで表示するボールの数が変わるので対応
			if( Factory_CommCheck(wk->type) == TRUE ){
				if( wk->ball_max == BF_T_MULTI_BALL_TEBANASU_POS_MAX ){
					wk->ball_max = BF_T_MULTI_BALL_UKETORU_POS_MAX;
				}else{
					wk->ball_max = BF_T_MULTI_BALL_TEBANASU_POS_MAX;
				}
			}

			TradeBallInit( wk );											//ボール追加
			for( i=0; i < wk->ball_max; i++ ){
				FactoryBall_Entrance( wk->p_ball[i] );						//座標セット
			}

			Snd_SePlay( SEQ_SE_DP_ELEBETA2 );
			wk->wait_count = 0;
			wk->sub_seq++;
		}
		break;

	//入場
	case SEQ_LEAVING_ENTRANCE_RAIL_MOVE_IN:
		if( RailMoveIn(wk) == TRUE ){
			Snd_SeStopBySeqNo( SEQ_SE_DP_ELEBETA2, 0 );
			Snd_SePlay( SEQ_SE_DP_KASYA );

			for( i=0; i < wk->ball_max; i++ ){
				FactoryBall_AnmChg( wk->p_ball[i], ANM_BALL_YURE );
			}

			//値が一番小さいパレットは6で、初期パレットが8になっている
			wk->maru_wait		= ( FACTORY_MARU_WAIT * 2 );

			Factory_ObjWinSet( wk, ANM_SINGLE_OPEN );	//開く

			wk->wait_count = 0;
			wk->sub_seq++;
		}
		break;

	//ポケモン表示
	case SEQ_LEAVING_ENTRANCE_POKE_APPEAR_TIMING:
		//左上のOBJウィンドウのアニメショーン終了まで待つ
		if( FactoryObj_AnmActiveCheck(wk->p_obj_win) == TRUE ){
			break;
		}
		Factory_ObjWinDel( wk );											//OBJウィンドウ削除
		wk->wait_count = 0;													//クリア
		wk->sub_seq++;
		break;

	//ポケモンフェードイン中
	case SEQ_LEAVING_ENTRANCE_POKE_FADEIN:
		if( wk->wait_count == 0 ){
			Factory_SetMainBgScrn( wk, BF_FRAME_BG );
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );		//BG非表示

			//手持ち、敵どちらのポケモンか判別
			if( wk->tebanasu_uketoru == 0 ){
				TradePokeInit( wk );
			}else{
				Factory_PokeAdd(wk, 0, 
								PokeParty_GetMemberPointer(wk->p_e_party,0),
								TEST_X, TEST_Y, FLIP_UKETORU );
			}

			SoftSpriteParaSet( wk->ss[0], SS_PARA_VANISH, 0 );					//表示
			SoftSpritePalFadeSetAll( wk->soft_sprite, 16, 0, 1, 0xffff );		//白からフェードイン
			ScrPalChg( wk, BF_FRAME_BG, Factory_GetScrPalNo(wk, ID_PAL_SINGLE_FLASH, 1), 21, 11 );
		}

		if( SoftSpritePalFadeExist(wk->ss[0]) ){							//TRUE = フェード中
			//現状特になし
		}

		wk->wait_count++;
		if( wk->wait_count < POKE_APPEAR_WAIT ){
			break;
		}
		ScrPalChg( wk, BF_FRAME_BG, Factory_GetScrPalNo(wk, ID_PAL_SINGLE_NORMAL, 1), 21, 11 );

		//BMPとして使用するBG面をクリアしておく
		Factory_FrameWinBmpSet( wk );

		//ペアのデータを表示しないフラグOFF
		wk->pair_data_off_flag = 0;
		//パートナーの選んだポケモンを表示リクエスト
		if( Factory_CommCheck(wk->type) == TRUE ){
			wk->pair_monsno_req = REQ_PAIR_MONSNO_DECIDE;
			Factory_PairMonsNoRecvReq( wk );
		}

		wk->wait_count = 0;
		return TRUE;
	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	送受信
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameSendRecv( FACTORY_WORK* wk )
{
	switch( wk->sub_seq ){

	case 0:
		//交換したかフラグを送信
		if( Factory_CommSetSendBuf(wk,FACTORY_COMM_TRADE,wk->trade_flag) == TRUE ){
			wk->sub_seq++;
		}
		break;

	case 1:
		if( wk->trade_recieve_count < FACTORY_COMM_PLAYER_NUM ){
			break;
		}

		//この後抜けて、pair_trade_flagを見て、相手の交換が有効だったかで分岐
		
		wk->trade_recieve_count = 0;
		//break;
		return TRUE;
	};

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	シーケンス：交換マルチ終了へ
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameTradeEndMulti( FACTORY_WORK* wk )
{
	int i;

	switch( wk->sub_seq ){

	case 0:
		//交換したかフラグを送信
		//if( Factory_CommSetSendBuf(wk,FACTORY_COMM_TRADE,wk->trade_flag) == TRUE ){

			//メッセージ表示
			wk->msg_index = Factory_EasyMsg( wk, msg_f_change_07 );
	
			//同期開始
			CommToolTempDataReset();
			CommTimingSyncStart( DBC_TIM_FACTORY_TRADE_END );
		
			wk->sub_seq++;
		//}
		break;

	//同期待ち
	case 1:
		//タイミングコマンドが届いたか確認
		if( CommIsTimingSync(DBC_TIM_FACTORY_TRADE_END) == TRUE ){
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
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GameEnd( FACTORY_WORK* wk )
{
	int i;

	switch( wk->sub_seq ){

	//フェードアウト
	case 0:
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
					WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_FACTORY );

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

//--------------------------------------------------------------
/**
 * @brief	シーケンス：パートナーが交換したので強制終了へ
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	"TRUE = 次へ、FALSE = 継続"
 */
//--------------------------------------------------------------
static BOOL Seq_GamePairTradeEnd( FACTORY_WORK* wk )
{
	int i;
	POKEMON_PARAM* temp_poke;
	POKEMON_PASO_PARAM* paso_poke;

	switch( wk->sub_seq ){

	case 0:
		//メニューカーソルOBJが表示されていたら
		if( wk->p_csr_menu != NULL ){
			BmpMenuExit( wk->mw, NULL );
			BmpMenuWinClearSub( wk->MenuH.win );
			FactoryCsr_Delete( wk->p_csr_menu );
			wk->p_csr_menu = NULL;
		}

		//パートナー名をセット
		Frontier_PairNameWordSet( wk->wordset, 0 );

		//手放したポケモン(パートナーのポケモンは後半に格納されている)
		temp_poke = PokeParty_GetMemberPointer( wk->p_m_party, 
												(FACTORY_COMM_POKE_NUM + wk->sel_work[0]) );
		paso_poke = PPPPointerGet( temp_poke );
		Factory_SetPokeName( wk, 1, paso_poke );

		//受け取ったポケモン
		temp_poke = PokeParty_GetMemberPointer( wk->p_e_party, wk->sel_work[1] );
		paso_poke = PPPPointerGet( temp_poke );
		Factory_SetPokeName( wk, 2, paso_poke );

#if 0
		wk->msg_index = Factory_EasyMsg( wk, msg_f_change_08 );
#else
		FactoryTalkWinPut( &wk->bmpwin[BMPWIN_TALK], CONFIG_GetWindowType(wk->config) );
		wk->msg_index = FactoryWriteMsg(wk, &wk->bmpwin[BMPWIN_TALK], msg_f_change_08, 
										1, 1, 
										CONFIG_GetMsgPrintSpeed(SaveData_GetConfig(wk->sv)),
										//MSG_ALLPUT, 
										FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE, FONT_TALK );
		GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_TALK] );
#endif

		wk->wait_count = FACTORY_PAIR_TRADE_END_WAIT;
		wk->sub_seq++;
		break;

	case 1:
#if 1
		//3行メッセージなので会話終了待ち
		if( GF_MSG_PrintEndCheck(wk->msg_index) == 0 ){
			wk->wait_count = 0;
			return TRUE;
		}
#else
		//少しウェイトを入れる
		wk->wait_count--;
		if( wk->wait_count == 0 ){
			return TRUE;
		}
#endif
		break;
	};

	return FALSE;
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
	FACTORY_WORK* wk = work;

	//ポケモンステータス表示中
	if( wk->child_proc != NULL ){
		return;
	}
		
	//VBlank関数で呼び出し
	SoftSpriteTextureTrans( wk->soft_sprite );

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
		//GX_VRAM_OBJ_128_B,				//メイン2DエンジンのOBJ
		GX_VRAM_OBJEXTPLTT_NONE,		//メイン2DエンジンのOBJ拡張パレット

		GX_VRAM_SUB_OBJ_16_I,			//サブ2DエンジンのOBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	//サブ2DエンジンのOBJ拡張パレット

		GX_VRAM_TEX_01_AB,				//テクスチャイメージスロット
		//GX_VRAM_TEX_0_A,				//テクスチャイメージスロット
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
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_3D,
		};
		GF_BGL_InitBG( &BGsys_data );
	}

	{	//BG(フォント)
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01,
			1, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, BF_FRAME_WIN, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ClearCharSet( BF_FRAME_WIN, 32, 0, HEAPID_FACTORY );
		GF_BGL_ScrClear( ini, BF_FRAME_WIN );
	}

	{	//上画面(MULTI)
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			//GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_01,
			GX_BG_SCRBASE_0x1800, GX_BG_CHARBASE_0x0c000, GX_BG_EXTPLTT_01,
			2, 0, 0, FALSE
			//0, 0, 0, FALSE					//開始演出のみフォント面(丸)より上にくるようにする
		};
		GF_BGL_BGControlSet( ini, BF_FRAME_SLIDE, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( ini, BF_FRAME_SLIDE );
	}

	{	//上画面(SINGLE,DOUBLE)
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			//GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x0c000, GX_BG_EXTPLTT_01,
			GX_BG_SCRBASE_0x2800, GX_BG_CHARBASE_0x0c000, GX_BG_EXTPLTT_01,
			3, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, BF_FRAME_BG, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( ini, BF_FRAME_BG );
	}

	//--------------------------------------------------------------------------------
	{	//下画面(ボール)
		GF_BGL_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GF_BGL_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			//GX_BG_SCRBASE_0x3000, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
			GX_BG_SCRBASE_0x3800, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		};
		GF_BGL_BGControlSet( ini, BF_FRAME_SUB, &TextBgCntDat, GF_BGL_MODE_TEXT );
		GF_BGL_ScrClear( ini, BF_FRAME_SUB );
	}

	G2_SetBG0Priority( 0 );
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
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
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Factory_BgInit( FACTORY_WORK* wk )
{
	SetVramBank();
	SetBgHeader( wk->bgl );

	//パレットフェードシステムワーク作成
	wk->pfd = PaletteFadeInit( HEAPID_FACTORY );

	//リクエストデータをmallocしてセット
	PaletteFadeWorkAllocSet( wk->pfd, FADE_MAIN_OBJ, FADE_PAL_ALL_SIZE, HEAPID_FACTORY );
	PaletteFadeWorkAllocSet( wk->pfd, FADE_MAIN_BG, FADE_PAL_ALL_SIZE, HEAPID_FACTORY );

	Factory_SetMainBgPalette();

	//開始演出が終了していない時
	if( wk->start_eff_flag == 0 ){
		//上画面背景
//TEST
		Factory_SetMainBgGraphic_Maru( wk, BF_FRAME_WIN );				//右上の円
		Factory_SetMainBgGraphic_Rail( wk, BF_FRAME_SLIDE );			//レール
		Factory_SetMainBgGraphic_Back( wk, BF_FRAME_BG );				//背景

		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );		//BG表示
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );		//BG表示
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );		//BG表示
	}else{
		//上画面背景
		Factory_SetMainBgGraphic( wk, BF_FRAME_BG );					//全てそろった背景
		Factory_SetMainBgGraphic_Rail( wk, BF_FRAME_SLIDE );			//レール
		
		//スクロール位置の調整
		GF_BGL_ScrollSet( wk->bgl, BF_FRAME_SLIDE, GF_BGL_SCROLL_X_SET, wk->scr_x );

		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );		//BG表示
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );		//BG表示
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );		//BG表示
	}

	//下画面背景、パレットセット
	Factory_SetSubBgGraphic( wk, BF_FRAME_SUB );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	OBJ関連初期化
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Factory_ObjInit( FACTORY_WORK* wk )
{
	FactoryClact_InitCellActor( &wk->factory_clact );
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
static void Factory_BgExit( GF_BGL_INI * ini )
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
	GF_BGL_BGControlExit( ini, BF_FRAME_BG );
	GF_BGL_BGControlExit( ini, BF_FRAME_SLIDE );
	GF_BGL_BGControlExit( ini, BF_FRAME_WIN );
	GF_BGL_BGControlExit( ini, BF_FRAME_SUB );

	sys_FreeMemoryEz( ini );
	return;
}


//==============================================================================================
//
//	ツール
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	シーケンス変更
 *
 * @param	wk		FACTORY_WORK型のポインタ
 * @param	seq		シーケンスのポインタ
 * @param	next	次のシーケンス定義
 *
 * @return	none
 */
//--------------------------------------------------------------
static void NextSeq( FACTORY_WORK* wk, int* seq, int next )
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
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void BgCheck( FACTORY_WORK* wk )
{
	if( sys.cont & PAD_KEY_UP ){
		if( sys.cont & PAD_BUTTON_SELECT ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
		}else if( sys.cont & PAD_BUTTON_START ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );
		}
	}else if( sys.cont & PAD_KEY_DOWN ){
		if( sys.cont & PAD_BUTTON_A ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
		}else if( sys.cont & PAD_BUTTON_B ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
		}else if( sys.cont & PAD_BUTTON_L ){
			SoftSpriteParaSet( wk->ss[0], SS_PARA_VANISH, 0 );		//表示
		}else if( sys.cont & PAD_BUTTON_R ){
			SoftSpriteParaSet( wk->ss[0], SS_PARA_VANISH, 1 );		//非表示
		}
	}else if( sys.cont & PAD_KEY_LEFT ){
		if( sys.cont & PAD_BUTTON_A ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
		}else if( sys.cont & PAD_BUTTON_B ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
		}
	}else if( sys.cont & PAD_KEY_RIGHT ){
		if( sys.cont & PAD_BUTTON_A ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
		}else if( sys.cont & PAD_BUTTON_B ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
		}
	}else if( sys.cont & PAD_BUTTON_SELECT ){
		if( sys.cont & PAD_BUTTON_A ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
		}else if( sys.cont & PAD_BUTTON_B ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_OFF );
		}
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	レール移動(入場)
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static BOOL RailMoveIn( FACTORY_WORK* wk )
{
	int i,x,y,end_flag;
	const VecFx32* p_vec;

	end_flag = 0;

	//レールスクロール
	GF_BGL_ScrollSet( wk->bgl, BF_FRAME_SLIDE, GF_BGL_SCROLL_X_INC, RAIL_SPD );
	wk->scr_x = GF_BGL_ScrollGetX( wk->bgl, BF_FRAME_SLIDE );
	//OS_Printf( "scroll_x = %d\n", GF_BGL_ScrollGetX(wk->bgl,BF_FRAME_SLIDE) );

	for( i=0; i < wk->ball_max; i++ ){
		p_vec = FactoryBall_GetPos( wk->p_ball[i] );					//現在の座標取得

		//元の位置より進みすぎないようにする
		if( ((p_vec->x / FX32_ONE)-RAIL_SPD) <= FactoryBall_GetInitX(wk->p_ball[i]) ){

			x = FactoryBall_GetInitX( wk->p_ball[i] );
			y = FactoryBall_GetInitY( wk->p_ball[i] );
			FactoryBall_SetPos( wk->p_ball[i], x, y );
			end_flag = 1;

		}else{
			FactoryBall_Move( wk->p_ball[i], -RAIL_SPD, 0 );		//画面右からインしてくる
		}
	}

	MaruMove( wk );													//円の移動

	if( end_flag == 1 ){
		return TRUE;
	}

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	レール移動(退場)
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static BOOL RailMoveOut( FACTORY_WORK* wk )
{
	int i,x,y,end_flag;
	const VecFx32* p_vec;

	end_flag = 0;

	//レールスクロール
	GF_BGL_ScrollSet( wk->bgl, BF_FRAME_SLIDE, GF_BGL_SCROLL_X_INC, RAIL_SPD );
	wk->scr_x = GF_BGL_ScrollGetX( wk->bgl, BF_FRAME_SLIDE );
	//OS_Printf( "scroll_x = %d\n", GF_BGL_ScrollGetX(wk->bgl,BF_FRAME_SLIDE) );

	for( i=0; i < wk->ball_max; i++ ){
		p_vec = FactoryBall_GetPos( wk->p_ball[i] );					//現在の座標取得

		//画面外に出ているかチェック
		if( ((p_vec->x / FX32_ONE)-RAIL_SPD) < -24 ){
			FactoryBall_Vanish( wk->p_ball[i], FACTORY_VANISH_ON );		//非表示
			end_flag++;
		}else{
			FactoryBall_Move( wk->p_ball[i], -RAIL_SPD, 0 );			//画面左へアウトしていく
		}
	}

	MaruMove( wk );														//円の移動

	//全てのボールが退場したかチェック
	if( end_flag == wk->ball_max ){
		return TRUE;
	}

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	円の移動
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void MaruMove( FACTORY_WORK* wk )
{
	if( (wk->maru_wait % FACTORY_MARU_WAIT) == 0 ){
		//パレット本数をチェックして、カウンターをクリアする
		if( (wk->maru_wait / FACTORY_MARU_WAIT) >= FACTORY_MARU_PAL_NUM ){
			wk->maru_wait = 0;
		}

		ScrPalChg(	wk, BF_FRAME_WIN, 
					(wk->maru_wait / FACTORY_MARU_WAIT) + FACTORY_MARU_BASE_PAL, 32, 32 );
	}

	wk->maru_wait++;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	パレット切り替え
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void ScrPalChg( FACTORY_WORK* wk, u32 frm, u8 pltt_no, u8 sx, u8 sy )
{
	OS_Printf( "切り替えるパレットナンバー = %d\n", pltt_no );
	GF_BGL_ScrPalChange( wk->bgl, frm, 0, 0, sx, sy, pltt_no );
	GF_BGL_LoadScreenV_Req( wk->bgl, frm );							//スクリーンデータ転送
	return;
}

//--------------------------------------------------------------
/**
 * @brief	OBJウィンドウのセット
 *
 * @param	wk			FACTORY_WORK型のポインタ
 * @param	anm_no		アニメナンバー
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void Factory_ObjWinSet( FACTORY_WORK* wk, u32 anm_no )
{
	u32 comm_anm_no;

	switch( anm_no ){

	case ANM_SINGLE_OPEN:	//開く
		Snd_SePlay( SEQ_SE_DP_OPEN2 );
		comm_anm_no = ANM_MULTI_OPEN;
		break;

	case ANM_SINGLE_CLOSE:	//閉じる
		Snd_SePlay( SEQ_SE_DP_CLOSE2 );
		comm_anm_no = ANM_MULTI_CLOSE;
		break;

	case ANM_3POKE_OPEN:	//開く
		Snd_SePlay( SEQ_SE_DP_OPEN2 );
		comm_anm_no = ANM_MULTI_OPEN;
		break;

	case ANM_3POKE_CLOSE:	//閉じる
		Snd_SePlay( SEQ_SE_DP_CLOSE2 );
		comm_anm_no = ANM_MULTI_CLOSE;
		break;
	};

	wk->p_obj_win = Factory_ObjWinAdd( wk, Factory_GetWinAnmNo(wk,anm_no) );
	if( Factory_CommCheck(wk->type) == TRUE ){
		wk->p_pair_obj_win = Factory_PairObjWinAdd( wk, comm_anm_no );
	}
	return;
}


//==============================================================================================
//
//	BGグラフィックデータ
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	グラフィックデータセット：上画面背景(シングル、ダブル)
 *
 * @param	wk		FACTORY_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Factory_SetMainBgGraphic( FACTORY_WORK * wk, u32 frm  )
{
	ArcUtil_HDL_BgCharSet(	wk->hdl, BF_RENTAL_NCGR_BIN, 
							wk->bgl, frm, 0, 0, 1, HEAPID_FACTORY );

	if( Factory_CommCheck(wk->type) == FALSE ){
		ArcUtil_HDL_ScrnSet(wk->hdl, BF_RENTAL_NSCR_BIN, 
							wk->bgl, frm, 0, 0, 1, HEAPID_FACTORY );
	}else{
		ArcUtil_HDL_ScrnSet(wk->hdl, BF_RENTAL_MULTI_NSCR_BIN, 
							wk->bgl, frm, 0, 0, 1, HEAPID_FACTORY );
	}
	return;
}

//--------------------------------------------------------------
/**
 * @brief	グラフィックデータセット：上画面背景(開始演出時)
 *
 * @param	wk		FACTORY_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Factory_SetMainBgGraphic_Back( FACTORY_WORK * wk, u32 frm  )
{
	ArcUtil_HDL_BgCharSet(	wk->hdl, BF_RENTAL_NCGR_BIN, 
							wk->bgl, frm, 0, 0, 1, HEAPID_FACTORY );

	ArcUtil_HDL_ScrnSet(wk->hdl, BF_RENTAL_BACK_NSCR_BIN, 
						wk->bgl, frm, 0, 0, 1, HEAPID_FACTORY );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	グラフィックデータセット：上画面(マルチ)
 *
 * @param	wk		FACTORY_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Factory_SetMainBgGraphic2( FACTORY_WORK * wk, u32 frm  )
{
	ArcUtil_HDL_BgCharSet(	wk->hdl, BF_RENTAL_NCGR_BIN, 
							wk->bgl, frm, 0, 0, 1, HEAPID_FACTORY );

	ArcUtil_HDL_ScrnSet(wk->hdl, BF_RENTAL_WIFIMODE_NSCR_BIN, 
						wk->bgl, frm, 0, 0, 1, HEAPID_FACTORY );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	グラフィックデータセット：上画面(レンタルポケモン決定時)
 *
 * @param	wk		FACTORY_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Factory_SetMainBgGraphic3( FACTORY_WORK * wk, u32 frm  )
{
	ArcUtil_HDL_BgCharSet(	wk->hdl, BF_RENTAL_NCGR_BIN, 
							wk->bgl, frm, 0, 0, 1, HEAPID_FACTORY );

	ArcUtil_HDL_ScrnSet(wk->hdl, BF_RENTAL2_NSCR_BIN, 
						wk->bgl, frm, 0, 0, 1, HEAPID_FACTORY );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	グラフィックデータセット：上画面(レール)
 *
 * @param	wk		FACTORY_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Factory_SetMainBgGraphic_Rail( FACTORY_WORK * wk, u32 frm  )
{
	ArcUtil_HDL_BgCharSet(	wk->hdl, BF_RENTAL_NCGR_BIN, 
							wk->bgl, frm, 0, 0, 1, HEAPID_FACTORY );

	ArcUtil_HDL_ScrnSet(wk->hdl, BF_RENTAL_RAIL_NSCR_BIN, 
						wk->bgl, frm, 0, 0, 1, HEAPID_FACTORY );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	グラフィックデータセット：上画面(右上の円)
 *
 * @param	wk		FACTORY_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Factory_SetMainBgGraphic_Maru( FACTORY_WORK * wk, u32 frm  )
{
	ArcUtil_HDL_BgCharSet(	wk->hdl, BF_RENTAL_NCGR_BIN, 
							wk->bgl, frm, 0, 0, 1, HEAPID_FACTORY );

	ArcUtil_HDL_ScrnSet(wk->hdl, BF_RENTAL_MARU_NSCR_BIN, 
						wk->bgl, frm, 0, 0, 1, HEAPID_FACTORY );

	GF_BGL_PrioritySet( BF_FRAME_WIN, 2 );	//右上の円のプライオリティを下げる
	return;
}

//--------------------------------------------------------------
/**
 * @brief	上画面背景パレット設定
 *
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Factory_SetMainBgPalette( void )
{
	void *buf;
	NNSG2dPaletteData *dat;

	buf = ArcUtil_PalDataGet( ARC_FRONTIER_BG, BF_RENTAL_NCLR, &dat, HEAPID_FACTORY );
		
	DC_FlushRange( dat->pRawData, (sizeof(u16)*16*11) );
	GX_LoadBGPltt( dat->pRawData, 0, (sizeof(u16)*16*11) );		//メイン

	sys_FreeMemoryEz(buf);
	return;
}

//--------------------------------------------------------------
/**
 * @brief	グラフィックデータセット：下画面背景、パレットセット
 *
 * @param	wk		FACTORY_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Factory_SetSubBgGraphic( FACTORY_WORK * wk, u32 frm  )
{
	ArcUtil_HDL_BgCharSet(	wk->hdl, BATT_FRONTIER_NCGR_BIN, wk->bgl, frm,
							0, 0, TRUE, HEAPID_FACTORY );

	ArcUtil_HDL_ScrnSet(wk->hdl, BATT_FRONTIER_NSCR_BIN, wk->bgl, frm,
						0, 0, TRUE, HEAPID_FACTORY );

	ArcUtil_HDL_PalSet(	wk->hdl, BATT_FRONTIER_NCLR, PALTYPE_SUB_BG,
						0, 0x20, HEAPID_FACTORY );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	スクリーンデータセット：上画面背景
 *
 * @param	wk		FACTORY_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Factory_SetMainBgScrn( FACTORY_WORK * wk, u32 frm  )
{
	if( Factory_CommCheck(wk->type) == FALSE ){
		ArcUtil_HDL_ScrnSet(wk->hdl, BF_RENTAL_NSCR_BIN, 
							wk->bgl, BF_FRAME_BG, 0, 0, 1, HEAPID_FACTORY );
	}else{
		ArcUtil_HDL_ScrnSet(wk->hdl, BF_RENTAL_MULTI_NSCR_BIN, 
							wk->bgl, BF_FRAME_BG, 0, 0, 1, HEAPID_FACTORY );
	}
	return;
}

//--------------------------------------------------------------
/**
 * @brief	スクリーンデータセット：開始演出時
 *
 * @param	wk		FACTORY_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Factory_SetRentalBackBgScrn( FACTORY_WORK * wk, u32 frm )
{
	ArcUtil_HDL_ScrnSet(wk->hdl, BF_RENTAL_BACK_NSCR_BIN, 
						wk->bgl, BF_FRAME_BG, 0, 0, 1, HEAPID_FACTORY );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	スクリーンデータセット：2,3匹ポケモン決定スクリーン
 *
 * @param	wk		FACTORY_WORK型のポインタ
 * @param	frm		フレームナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Factory_Set3PokeBgScrn( FACTORY_WORK * wk, u32 frm )
{
	if( Factory_CommCheck(wk->type) == FALSE ){
		ArcUtil_HDL_ScrnSet(wk->hdl, BF_RENTAL_3POKE_NSCR_BIN, 
							wk->bgl, BF_FRAME_BG, 0, 0, 1, HEAPID_FACTORY );
	}else{
		ArcUtil_HDL_ScrnSet(wk->hdl, BF_RENTAL_MULTI_2POKE_NSCR_BIN, 
							wk->bgl, BF_FRAME_BG, 0, 0, 1, HEAPID_FACTORY );
	}
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
 * @param	wk		FACTORY_WORK型のポインタ
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
static u8 FactoryWriteMsg( FACTORY_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font )
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
 * @param	wk		FACTORY_WORK型のポインタ
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
static u8 FactoryWriteMsgSimple( FACTORY_WORK* wk, GF_BGL_BMPWIN* win, int msg_id, u32 x, u32 y, u32 wait, u8 f_col, u8 s_col, u8 b_col, u8 font )
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
 * @param	wk		FACTORY_WORK型のポインタ
 * @param	msg_id	メッセージID
 *
 * @return	"msg_index"
 */
//--------------------------------------------------------------
static u8 Factory_EasyMsg( FACTORY_WORK* wk, int msg_id )
{
	u8 msg_index;

	msg_index = FactoryWriteMsg(wk, &wk->bmpwin[BMPWIN_TALK], msg_id, 
								1, 1, MSG_NO_PUT, 
								FBMP_COL_BLACK,FBMP_COL_BLK_SDW,FBMP_COL_WHITE, FONT_TALK );


	GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_TALK] );
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
 * @param	wk		FACTORY_WORK型のポインタ
 * @param	win		GF_BGL_BMPWIN型のポインタ
 * @param	y_max	項目最大数
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void FactoryInitMenu( FACTORY_WORK* wk, GF_BGL_BMPWIN* win, u8 y_max )
{
	int i;

	for( i=0; i < FACTORY_MENU_BUF_MAX ;i++ ){
		wk->Data[i].str	  = NULL;
		wk->Data[i].param = 0;
	}

	wk->MenuH.menu		= wk->Data;
	wk->MenuH.win		= win;
	wk->MenuH.font		= BF_FONT;
	wk->MenuH.x_max		= 1;
	wk->MenuH.y_max		= y_max;
	wk->MenuH.line_spc	= 0;
	wk->MenuH.c_disp_f	= 1;			//カーソルなし
	wk->MenuH.loop_f	= 1;			//ループ有り
	return;
}

//--------------------------------------------------------------
/**
 * @brief	BMPメニュー	データセット
 *
 * @param	wk		FACTORY_WORK型のポインタ
 * @param	no		セットする場所(戻り値)
 * @param	param	戻り値
 * @param	msg_id	メッセージID
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void FactorySetMenuData( FACTORY_WORK* wk, u8 no, u8 param, int msg_id )
{
	int i;
	void* msg;

	SDK_ASSERTMSG( no < FACTORY_MENU_BUF_MAX, "メニュー項目数オーバー！" );

	MSGMAN_GetString( wk->msgman, msg_id, wk->menu_buf[no] );

	wk->Data[ no ].str = (const void *)wk->menu_buf[no];
	wk->Data[ no ].param = param;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	共通 メニューセット
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Factory_SetMenu( FACTORY_WORK* wk )
{
	FactoryWriteMenuWin( wk->bgl, &wk->bmpwin[BMPWIN_SEL] );
	FactoryInitMenu( wk, &wk->bmpwin[BMPWIN_SEL], POKE_SEL_MENU_MAX );
	FactorySetMenuData( wk, 0, RET_PARAM_TUYOSA, msg_rental_choice_01 );			//ようすをみる

	//レンタルモード
	if( Factory_CheckMode(wk,FACTORY_MODE_RENTAL) == TRUE ){

		if( FactoryBall_GetDecideFlag(wk->p_ball[ FactoryCsr_GetCsrPos(wk->p_csr) ]) == 0 ){
			FactorySetMenuData( wk, 1, RET_PARAM_RENTAL, msg_rental_choice_02 );	//レンタルする
		}else{
			FactorySetMenuData( wk, 1, RET_PARAM_HAZUSU, msg_rental_choice_04 );	//はずす
		}

	//交換モード(ボールの数と配置場所などが違う)
	}else{
		FactorySetMenuData( wk, 1, RET_PARAM_TEBANASU, msg_f_change_choice_03 );	//てばなす
	}

	FactorySetMenuData( wk, 2, RET_PARAM_BETUNO, msg_rental_choice_03 );			//べつのにする
	wk->mw = BmpMenuAddEx( &wk->MenuH, 0, 0, 0, HEAPID_FACTORY, PAD_BUTTON_B );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	共通 メニューセット2「はい、いいえ」
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Factory_SetMenu2( FACTORY_WORK* wk )
{
	FactoryWriteMenuWin( wk->bgl, &wk->bmpwin[BMPWIN_SEL] );
	FactoryInitMenu( wk, &wk->bmpwin[BMPWIN_SEL], YES_NO_MENU_MAX );
	FactorySetMenuData( wk, 0, RET_PARAM_YES, msg_rental_04 );						//はい
	FactorySetMenuData( wk, 1, RET_PARAM_NO, msg_rental_05 );						//いいえ
	wk->mw = BmpMenuAddEx( &wk->MenuH, 0, 0, 0, HEAPID_FACTORY, PAD_BUTTON_B );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	共通 メニューセット3「もどる、ちゅうし」
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Factory_SetMenu3( FACTORY_WORK* wk )
{
	FactoryWriteMenuWin( wk->bgl, &wk->bmpwin[BMPWIN_SEL] );
	FactoryInitMenu( wk, &wk->bmpwin[BMPWIN_SEL], TRADE_FINAL_MENU_MAX );
	FactorySetMenuData( wk, 0, RET_PARAM_MODORU, msg_rental_04 );					//もどる
	FactorySetMenuData( wk, 1, RET_PARAM_TYUUSI, msg_rental_05 );					//ちゅうし
	wk->mw = BmpMenuAddEx( &wk->MenuH, 0, 0, 0, HEAPID_FACTORY, PAD_BUTTON_B );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	共通 メニューセット4「ちゅうし」
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Factory_SetMenu4( FACTORY_WORK* wk )
{
	FactoryWriteMenuWin( wk->bgl, &wk->bmpwin[BMPWIN_SEL] );
	FactoryInitMenu( wk, &wk->bmpwin[BMPWIN_SEL], TRADE_FINAL_MENU_MAX );
	FactorySetMenuData( wk, 0, RET_PARAM_MODORU, msg_f_change_choice_01 );			//ちゅうし
	wk->mw = BmpMenuAddEx( &wk->MenuH, 0, 0, 0, HEAPID_FACTORY, PAD_BUTTON_B );
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
 * @param	wk		FACTORY_WORK型のポインタ
 * @param	bufID	バッファID
 * @param	number	セットする数値
 *
 * @retval	none
 *
 * 桁を固定にしている
 */
//--------------------------------------------------------------
static void Factory_SetNumber( FACTORY_WORK* wk, u32 bufID, s32 number )
{
	WORDSET_RegisterNumber( wk->wordset, bufID, number, 1, 
							NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ポケモン名をセット
 *
 * @param	wk		FACTORY_WORK型のポインタ
 * @param	bufID	バッファID
 * @param	ppp		POKEMON_PASO_PARAM
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void Factory_SetPokeName( FACTORY_WORK* wk, u32 bufID, POKEMON_PASO_PARAM* ppp )
{
	WORDSET_RegisterPokeMonsName( wk->wordset, bufID, ppp );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	プレイヤー名をセット
 *
 * @param	wk		FACTORY_WORK型のポインタ
 * @param	bufID	バッファID
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void Factory_SetPlayerName( FACTORY_WORK* wk, u32 bufID )
{
	WORDSET_RegisterPlayerName( wk->wordset, bufID, SaveData_GetMyStatus(wk->sv) );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	プレイヤー名を表示
 *
 * @param	wk		FACTORY_WORK型のポインタ
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
static void PlayerNameWrite( FACTORY_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 font )
{
	u32 col;
	const MYSTATUS* my;
	STRBUF* player_buf;								//プレイヤー名バッファポインタ

	my = SaveData_GetMyStatus( wk->sv );
	player_buf = STRBUF_Create( PLAYER_NAME_BUF_SIZE, HEAPID_FACTORY );
	
	GF_BGL_BmpWinDataFill( win, FBMP_COL_NULL );			//塗りつぶし

	STRBUF_SetStringCode( player_buf, MyStatus_GetMyName(my) );

	if( MyStatus_GetMySex(my) == PM_MALE ){
		col = COL_BLUE;
	}else{
		col = COL_RED;
	}

	GF_STR_PrintColor( win, font, player_buf, x, y, MSG_NO_PUT, col, NULL );

	STRBUF_Delete( player_buf );					//メッセージバッファ開放

	GF_BGL_BmpWinOnVReq( win );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	パートナー名を表示
 *
 * @param	wk		FACTORY_WORK型のポインタ
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
static void PairNameWrite( FACTORY_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 font )
{
	u32 col;
	STRBUF* player_buf;								//プレイヤー名バッファポインタ
	MYSTATUS* my;									//パートナーのMyStatus

	//パートナーのMyStatusを取得
	my = CommInfoGetMyStatus( 1 - CommGetCurrentID() );

	player_buf = STRBUF_Create( PLAYER_NAME_BUF_SIZE, HEAPID_FACTORY );
	
	GF_BGL_BmpWinDataFill( win, FBMP_COL_NULL );			//塗りつぶし

	//名前取得(STRBUFにコピー)
	MyStatus_CopyNameString( my, player_buf );

	if( MyStatus_GetMySex(my) == PM_MALE ){
		col = COL_BLUE;
	}else{
		col = COL_RED;
	}

	GF_STR_PrintColor( win, font, player_buf, x, y, MSG_NO_PUT, col, NULL );

	STRBUF_Delete( player_buf );					//メッセージバッファ開放

	GF_BGL_BmpWinOnVReq( win );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ポケモン名を表示
 *
 * @param	wk		FACTORY_WORK型のポインタ
 * @param	win		ビットマップウィンドウ
 * @param	csr_pos	POKEPARTYの位置
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
static void PokeNameWrite( FACTORY_WORK* wk, GF_BGL_BMPWIN* win, u8 csr_pos, u32 x, u32 y, u8 f_col, u8 s_col, u8 b_col, u8 font, const POKEPARTY* party )
{
	u8 x_pos;
	u32 sex,msg_id,col;
	STRBUF* buf;
	POKEMON_PARAM* poke;
	STRCODE sel_poke_buf[POKE_NAME_BUF_SIZE];					//ポケモン名バッファポインタ

	poke = PokeParty_GetMemberPointer( party, csr_pos );
	PokeParaGet( poke, ID_PARA_default_name, sel_poke_buf );	//ポケモンのデフォルト名を取得

	GF_BGL_BmpWinDataFill( win, b_col );						//塗りつぶし

	buf = STRBUF_Create( POKE_NAME_BUF_SIZE, HEAPID_FACTORY );	//STRBUF生成
	STRBUF_SetStringCode( buf, sel_poke_buf );					//STRCODE→STRBUF

	GF_STR_PrintColor( win, font, buf, x, y, MSG_NO_PUT, 
								GF_PRINTCOLOR_MAKE(f_col,s_col,b_col), NULL );

	//右端に性別コードを表示
	x_pos = GF_BGL_BmpWinGet_SizeX(win) - 1;
	sex = PokeParaGet( poke, ID_PARA_sex, NULL );

	msg_id = (sex == PARA_MALE) ? msg_rental_male : msg_rental_female;
	col = (sex == PARA_MALE) ? COL_BLUE : COL_RED;

	STRBUF_Clear( buf );

	//性別がある時
	if( sex != PARA_UNK ){
		MSGMAN_GetString( wk->msgman, msg_id, buf );
		GF_STR_PrintColor( win, font, buf, x_pos*8, y, MSG_NO_PUT, col, NULL );
	}

	STRBUF_Delete( buf );
	GF_BGL_BmpWinOnVReq( win );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	ポケモン名を表示(性別指定)
 *
 * @param	wk		FACTORY_WORK型のポインタ
 * @param	win		ビットマップウィンドウ
 * @param	x		X座標(ドット単位)
 * @param	y		Y座標(ドット単位)
 * @param	f_col	文字色ナンバー(塗りつぶしカラーコード)
 * @param	s_col	影色ナンバー
 * @param	b_col	背景色ナンバー
 * @param	font	フォント種類
 * @param	monsno	モンスターナンバー
 * @param	sex		性別
 *
 * @return	none
 */
//--------------------------------------------------------------
static void PokeNameWriteEx( FACTORY_WORK* wk, GF_BGL_BMPWIN* win, u32 x, u32 y, u8 f_col, u8 s_col, u8 b_col, u8 font, u16 monsno, u8 sex )
{
	u8 x_pos;
	u32 msg_id,col;
	MSGDATA_MANAGER* man;
	STRBUF* buf;
	POKEMON_PARAM* poke;
	STRCODE sel_poke_buf[POKE_NAME_BUF_SIZE];					//ポケモン名バッファポインタ

	GF_BGL_BmpWinDataFill( win, b_col );						//塗りつぶし

	//ポケモン名を取得
	man = MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, NARC_msg_monsname_dat, HEAPID_FACTORY );
	buf = MSGMAN_AllocString( man, monsno );
	MSGMAN_Delete( man );

	GF_STR_PrintColor( win, font, buf, x, y, MSG_NO_PUT, 
								GF_PRINTCOLOR_MAKE(f_col,s_col,b_col), NULL );

	//右端に性別コードを表示
	x_pos = GF_BGL_BmpWinGet_SizeX(win) - 1;

	msg_id = (sex == PARA_MALE) ? msg_rental_male : msg_rental_female;
	col = (sex == PARA_MALE) ? COL_BLUE : COL_RED;

	STRBUF_Clear( buf );

	//性別がある時
	if( sex != PARA_UNK ){
		MSGMAN_GetString( wk->msgman, msg_id, buf );
		GF_STR_PrintColor( win, font, buf, x_pos*8, y, MSG_NO_PUT, col, NULL );
	}

	STRBUF_Delete( buf );

	//GF_BGL_BmpWinOnVReq( win );
	return;
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
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void FactoryCommon_Delete( FACTORY_WORK* wk )
{
	int i;

	//ボールOBJ削除
	for( i=0; i < wk->ball_max ;i++ ){
		if( wk->p_ball[i] != NULL ){
			wk->p_ball[i] = FactoryBall_Delete( wk->p_ball[i] );
		}
	}

	//カーソルOBJ削除
	if( wk->p_csr != NULL ){
		FactoryCsr_Delete( wk->p_csr );
		wk->p_csr = NULL;
	}

	//通信アイコン削除
	WirelessIconEasyEnd();

	//パレットフェード開放
	PaletteFadeWorkAllocFree( wk->pfd, FADE_MAIN_OBJ );
	PaletteFadeWorkAllocFree( wk->pfd, FADE_MAIN_BG );

	//パレットフェードシステム開放
	PaletteFadeFree( wk->pfd );
	wk->pfd = NULL;

	FactoryClact_DeleteCellObject(&wk->factory_clact);	//2Dオブジェクト関連領域開放

	//ソフトウェアスプライト削除
	for( i=0; i < FACTORY_SOFT_SPRITE_MAX; i++ ){
		if( wk->ss[i] != NULL ){
			SoftSpriteDel( wk->ss[i] );
		}
	}

	//システム終了
	SoftSpriteEnd( wk->soft_sprite );

	MSGMAN_Delete( wk->msgman );						//メッセージマネージャ開放
	WORDSET_Delete( wk->wordset );
	STRBUF_Delete( wk->msg_buf );						//メッセージバッファ開放
	STRBUF_Delete( wk->tmp_buf );						//メッセージバッファ開放

	for( i=0; i < FACTORY_MENU_BUF_MAX ;i++ ){
		STRBUF_Delete( wk->menu_buf[i] );				//メニューバッファ開放
	}

	FactoryExitBmpWin( wk->bmpwin );					//BMPウィンドウ開放
	Factory_BgExit( wk->bgl );							//BGL削除
	ArchiveDataHandleClose( wk->hdl );

	GF_G3D_Exit( wk->g3Dman );
	return;
}

//--------------------------------------------------------------
/**
 * @brief	復帰
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Factory_Recover( FACTORY_WORK* wk )
{
	int i;

	Factory_InitSub1();

	//3Dシステム設定
	wk->g3Dman = GF_G3DMAN_Init(HEAPID_FACTORY, GF_G3DMAN_LNK, GF_G3DTEX_256K, 
								GF_G3DMAN_LNK, GF_G3DPLT_32K, Factory_SimpleSetUp );

	wk->bgl	= GF_BGL_BglIniAlloc( HEAPID_FACTORY );

	Factory_InitSub2( wk );
	FactoryAddBmpWin( wk->bgl, wk->bmpwin );		//ビットマップ追加
	return;
}

//--------------------------------------------------------------
/**
 * @brief	共通 初期化1
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Factory_InitSub1( void )
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
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Factory_InitSub2( FACTORY_WORK* wk )
{
	int i,flip;

	wk->hdl = ArchiveDataHandleOpen( ARC_FRONTIER_BG, HEAPID_FACTORY );
	Factory_BgInit( wk );								//BG初期化
	Factory_ObjInit( wk );								//OBJ初期化

	//メッセージデータマネージャー作成
	wk->msgman = MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, 
								NARC_msg_factory_rental_dat, HEAPID_FACTORY );

	wk->wordset = WORDSET_Create( HEAPID_FACTORY );
	wk->msg_buf = STRBUF_Create( FACTORY_MSG_BUF_SIZE, HEAPID_FACTORY );
	wk->tmp_buf = STRBUF_Create( FACTORY_MSG_BUF_SIZE, HEAPID_FACTORY );

	//文字列バッファ作成
	for( i=0; i < FACTORY_MENU_BUF_MAX ;i++ ){
		wk->menu_buf[i] = STRBUF_Create( FACTORY_MENU_BUF_SIZE, HEAPID_FACTORY );
	}

	//フォントパレット
	SystemFontPaletteLoad( PALTYPE_MAIN_BG, BF_FONT_PAL * 32, HEAPID_FACTORY );
	TalkFontPaletteLoad( PALTYPE_MAIN_BG, BF_MSGFONT_PAL * 32, HEAPID_FACTORY );

	//ソフトウェアスプライト
	wk->soft_sprite = SoftSpriteInit( HEAPID_FACTORY );

	//通信アイコンセット
	if( CommIsInitialize() ){
//CLACT_U_WmIcon_SetReserveAreaCharManager( NNS_G2D_VRAM_TYPE_2DMAIN,GX_OBJVRAMMODE_CHAR_1D_64K );
		CLACT_U_WmIcon_SetReserveAreaCharManager( NNS_G2D_VRAM_TYPE_2DMAIN,
												  GX_OBJVRAMMODE_CHAR_1D_32K );
		CLACT_U_WmIcon_SetReserveAreaPlttManager( NNS_G2D_VRAM_TYPE_2DMAIN );
		WirelessIconEasy();
	}

	//ボール、ポケモン追加
	if( Factory_CheckMode(wk,FACTORY_MODE_RENTAL) == TRUE ){	//レンタル
		RentalBallInit( wk );
		RentalPokeInit( wk );
	}else{
		TradeBallInit( wk );
		TradePokeInit( wk );
	}

	GF_Disp_DispOn();

	sys_VBlankFuncChange( VBlankFunc, (void*)wk );		//VBlankセット
	return;
}

//--------------------------------------------------------------
/**
 * @brief	共通 ポケモン切り替え
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void FactoryCommon_ChangePoke( FACTORY_WORK* wk, u8 index, u8 poke_pos, int flip, const POKEPARTY* party )
{
	int pos = poke_pos;

	//ボールの表示数より小さい時の処理
	if( FactoryCsr_GetCsrPos(wk->p_csr) < wk->ball_max ){
		SoftSpriteDel( wk->ss[0] );
		Factory_PokeAdd(wk, 0, 
						PokeParty_GetMemberPointer(party,pos),
						TEST_X, TEST_Y, flip );
		PokeNameWrite(	wk, &wk->bmpwin[BMPWIN_POKE1+index], 
						pos, 0, 0, 
						//FBMP_COL_BLACK, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, party );
						FBMP_COL_WHITE, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, party );
	}

	return;
}


//==============================================================================================
//
//	ポケモンステータス呼び出し関連
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	psdのセット
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Factory_SetPSD( FACTORY_WORK* wk )
{
	wk->psd = sys_AllocMemory( HEAPID_FACTORY, sizeof(PSTATUS_DATA) );
	memset( wk->psd, 0, sizeof(PSTATUS_DATA) );

	wk->psd->ppd	= wk->p_m_party;
	wk->psd->cfg	= wk->config;
	wk->psd->ppt	= PST_PP_TYPE_POKEPARTY;		//パラメータタイプ

	//あとで確認
	//wk->psd->mode = PST_MODE_NORMAL;
	wk->psd->mode = PST_MODE_NO_WAZACHG;

	wk->psd->max	= wk->ball_max;
	wk->psd->pos	= FactoryCsr_GetCsrPos( wk->p_csr );

	//あとで確認
	wk->psd->waza	= 0;
	wk->psd->zukan_mode = PMNumber_GetMode( wk->sv );
	//wk->psd->ev_contest = PokeStatus_ContestFlagGet( wk->sv );
	wk->psd->ev_contest = 0;	//下画面のタッチボタンでコンテスト項目が表示されないようにする

	//[[[[[[ステータスのページはこれでよいのか？]]]]]]]
	PokeStatus_PageSet( wk->psd, PST_PageTbl_Normal );
	PokeStatus_PlayerSet( wk->psd, SaveData_GetMyStatus(wk->sv) );
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
 * @param	wk		FACTORY_WORK型のポインタ
 * @param	type	チェックするタイプ
 *
 * @return	"TRUE = typeが同じ、FALSE = typeが違う"
 */
//--------------------------------------------------------------
static BOOL Factory_CheckType( FACTORY_WORK* wk, u8 type )
{
	if( wk->type == type ){
		return TRUE;
	}

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	現在のmodeチェック関数
 *
 * @param	wk		FACTORY_WORK型のポインタ
 * @param	mode	チェックするMODE
 *
 * @return	"TRUE = typeが同じ、FALSE = typeが違う"
 */
//--------------------------------------------------------------
static BOOL Factory_CheckMode( FACTORY_WORK* wk, u8 mode )
{
	if( wk->mode == mode ){
		return TRUE;
	}

	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	typeごとのポケモンを選ぶ人数を取得
 *
 * @param	type	タイプ
 *
 * @return	"シングル、ダブル		3"
 * @return	"マルチ、wifiマルチ		2"
 */
//--------------------------------------------------------------
static u8 Factory_GetRentalSelPokeNum( u8 type )
{
	switch( type ){
	case FACTORY_TYPE_SINGLE:
	case FACTORY_TYPE_DOUBLE:
		return 3;
	};

	//case FACTORY_TYPE_MULTI:
	//case FACTORY_TYPE_WIFI_MULTI:
	return FACTORY_COMM_POKE_NUM;	//2
}

//--------------------------------------------------------------
/**
 * @brief	sel_workのクリア
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Factory_SelWorkClear( FACTORY_WORK* wk )
{
	int i;

	for( i=0; i < FACTORY_SEL_WORK_MAX ;i++ ){
		wk->sel_work[i] = 0;
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	キャンセルしたかチェック
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	"TRUE = キャンセルした、FALSE = キャンセルしていない"
 */
//--------------------------------------------------------------
static BOOL Factory_CheckCancel( FACTORY_WORK* wk )
{
	if( wk->cancel == 0 ){
		return 0;
	}

	return 1;
}

//--------------------------------------------------------------
/**
 * @brief	キャンセルのセット
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	"TRUE = キャンセルした、FALSE = キャンセルしていない"
 */
//--------------------------------------------------------------
static void Factory_SetCancel( FACTORY_WORK* wk, u8 flag )
{
	wk->cancel = flag;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	IDから、通信タイプかをチェックして、背景の変更するパレットNO取得
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	"パレットナンバー"
 */
//--------------------------------------------------------------
static u8 Factory_GetScrPalNo( FACTORY_WORK* wk, u8 id, u8 final_answer_flag  )
{
	u8 pltt_no;
	pltt_no = 0;

	switch( id ){

	//開いてる時
	case ID_PAL_SINGLE_OPEN:
		if( final_answer_flag == 1 ){
			pltt_no = FACTORY_SINGLE_WIN_OPEN_PAL;				//ポケ3匹
		}else if( Factory_CommCheck(wk->type) == FALSE ){
			pltt_no = FACTORY_SINGLE_WIN_OPEN_PAL;				//シングル
		}else{
			pltt_no = FACTORY_SINGLE_WIN_OPEN_PAL;				//シングル
		}
		break;

	//光る時
	case ID_PAL_SINGLE_FLASH:
		if( final_answer_flag == 1 ){
			pltt_no = FACTORY_SINGLE_WIN_FLASH_PAL;				//ポケ3匹
		}else if( Factory_CommCheck(wk->type) == FALSE ){
			pltt_no = FACTORY_SINGLE_WIN_FLASH_PAL;				//シングル
		}else{
			pltt_no = FACTORY_SINGLE_WIN_FLASH_PAL;				//シングル
		}
		break;

	//通常
	case ID_PAL_SINGLE_NORMAL:
		if( final_answer_flag == 1 ){
			pltt_no = FACTORY_SINGLE_WIN_NORMAL_PAL;			//ポケ3匹
		}else if( Factory_CommCheck(wk->type) == FALSE ){
			pltt_no = FACTORY_SINGLE_WIN_NORMAL_PAL;			//シングル
		}else{
			pltt_no = FACTORY_SINGLE_WIN_NORMAL_PAL;			//シングル
		}
		break;

	};

	return pltt_no;
};

//--------------------------------------------------------------
/**
 * @brief	通信タイプかをチェックして、アニメナンバーを取得
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	"アニメナンバー"
 */
//--------------------------------------------------------------
static u32 Factory_GetWinAnmNo( FACTORY_WORK* wk, u32 anm_no )
{
	switch( anm_no ){

	case ANM_SINGLE_OPEN:
		break;

	case ANM_SINGLE_CLOSE:
		break;

	case ANM_3POKE_OPEN:
		if( Factory_CommCheck(wk->type) == TRUE ){
			return ANM_MULTI_2POKE_OPEN;
		}
		break;

	case ANM_3POKE_CLOSE:
		if( Factory_CommCheck(wk->type) == TRUE ){
			return ANM_MULTI_2POKE_CLOSE;
		}
		break;

	};

	return anm_no;
};


//==============================================================================================
//
//	ソフトウェアスプライト
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	ポケモン表示
 *
 * @param	wk		FACTORY_WORK型のポインタ
 * @param	index	ss[index]	
 * @param	poke	POKEMON_PARAMポインタ
 * @param	x		X座標
 * @param	y		Y座標
 * @param	flip	左右反転するか
 *
 * @return	none
 */
//--------------------------------------------------------------
static void Factory_PokeAdd(FACTORY_WORK* wk, u8 index, POKEMON_PARAM* poke, int x, int y, int flip)
{
	u32 monsno,formno;

	wk->ss[index] = Factory_SoftSpritePokeAdd(	wk->soft_sprite, TEST_POLY_ID, 
												poke,
												x, y, TEST_Z );

	SoftSpriteParaSet( wk->ss[index], SS_PARA_VANISH, 0 );	//表示

	//コンテストと同じ反転できるかチェックを使用する
	monsno = PokeParaGet( poke, ID_PARA_monsno, NULL );
	formno = PokeParaGet( poke, ID_PARA_form_no, NULL );

	if( PokeFormNoPersonalParaGet(monsno,formno,ID_PER_reverse) == 0 ){		//反転OK
	//if( PokeFlipCheck(monsno) == TRUE ){
		Factory_PokeHFlip( wk->ss[index], flip );			//左右反転
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief   ファクトリーで反転させないポケモンかチェック
 *
 * @param   monsno		ポケモン番号
 *
 * @retval  TRUE：反転する
 * @retval  FALSE：反転しない
 *
 * src/contest/con_battle.cの中身をコピーしています
 */
//--------------------------------------------------------------
static BOOL PokeFlipCheck(u32 monsno)
{
	switch(monsno){
	case MONSNO_HITODEMAN:
	case MONSNO_EREBUU:
	case MONSNO_ARIGEITU:
	case MONSNO_ANNOON:
	case MONSNO_NYUURA:
	case MONSNO_ROZERIA:
	case MONSNO_ZANGUUSU:
	case MONSNO_ABUSORU:
	case MONSNO_SUBOMII:
	case MONSNO_ROZUREIDO:
	case MONSNO_BUUBAAN:
	case MONSNO_MORIGAME:
	case MONSNO_TOGEON:
	case MONSNO_KINGURAA:
		return FALSE;
	}
	return TRUE;
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
 * @param	wk			FACTORY_WORK型のポインタ
 * @param	type		送信タイプ
 *
 * @retval	none
 */
//--------------------------------------------------------------
BOOL Factory_CommSetSendBuf( FACTORY_WORK* wk, u16 type, u16 param )
{
	int ret,command;

	//通信タイプでない時
	if( Factory_CommCheck(wk->type) == FALSE ){
		return FALSE;
	}

	switch( type ){

	//名前
	case FACTORY_COMM_PAIR:
		command = FC_FACTORY_PAIR;
		Factory_CommSendBufBasicData( wk, type );
		break;

	//レンタル画面で決定したポケモンナンバーと性別(2匹決定したら送る)
	case FACTORY_COMM_PAIR_POKE:
		command = FC_FACTORY_PAIR_POKE;
		Factory_CommSendBufRentalData( wk, type, param );
		break;

	//交換画面で交換した(決定したら送る)
	case FACTORY_COMM_TRADE:
		command = FC_FACTORY_TRADE;
		Factory_CommSendBufTradeFlag( wk, type, param );
		break;
	};

	if( CommSendData(command,wk->send_buf,FACTORY_COMM_BUF_LEN) == TRUE ){
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
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 *
 * ファクトリーワークを確保後、最初に送るデータ
 */
//--------------------------------------------------------------
void Factory_CommSendBufBasicData( FACTORY_WORK* wk, u16 type )
{
	MYSTATUS* my;
	
	my	= SaveData_GetMyStatus( wk->sv );

	wk->send_buf[0] = type;
	OS_Printf( "送信：type = %d\n", wk->send_buf[0] );
	//wk->send_buf[1]	= ;
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
 *
 * ファクトリーワークを確保後、最初に受け取るデータ
 */
//--------------------------------------------------------------
void Factory_CommRecvBufBasicData(int id_no,int size,void *pData,void *work)
{
	int i,num;
	FACTORY_WORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******ステージ****** 基本情報受信\n" );

	num = 0;
	//wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	OS_Printf( "id_no = %d\n", id_no );
	OS_Printf( "size = %d\n", size );
	//OS_Printf( "pData[0] = %d\n", ((u8*)pData)[0] );
	OS_Printf( "recv_buf[0] = %d\n", recv_buf[0] );

	//type = recv_buf[0];
	
	//wk-> = ;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	send_bufにレンタルで選んだポケモンナンバーと性別をセット
 *
 * @param	wk		FACTORY_WORK型のポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------
void Factory_CommSendBufRentalData( FACTORY_WORK* wk, u16 type, u16 param )
{
	int i,num;
	POKEMON_PARAM* poke;
	
	num = 0;

	//クリアしておく
	for( i=0; i < FACTORY_COMM_BUF_LEN ;i++ ){
		wk->send_buf[i] = 0;
	}

	wk->send_buf[0] = wk->sel_count;
	OS_Printf( "送信：sel_count = %d\n", wk->send_buf[0] );
	num += 1;															//1

	//ポケモンナンバー
	for( i=0; i < wk->sel_count ;i++ ){
		poke = PokeParty_GetMemberPointer( wk->p_m_party, wk->sel_work[i] );
		wk->send_buf[i+num] = PokeParaGet( poke, ID_PARA_monsno, NULL );
		OS_Printf( "送信：monsno[%d] = %d\n", i, wk->send_buf[i+num] );
	}
	num += wk->sel_count;												//3

	//ポケモン性別
	for( i=0; i < wk->sel_count ;i++ ){
		poke = PokeParty_GetMemberPointer( wk->p_m_party, wk->sel_work[i] );
		wk->send_buf[i+num] = PokeParaGet( poke, ID_PARA_sex, NULL );
		OS_Printf( "送信：sex[%d] = %d\n", i, wk->send_buf[i+num] );
	}
	num += wk->sel_count;												//5
	
	return;
}

//--------------------------------------------------------------
/**
 * @brief   レンタルで選んだポケモンナンバーと性別 通信受信処理
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		FRONTIER_SYSTEMへのポインタ
 *
 * 子機のレンタルポケモン
 * トレーナーデータ(7人分)
 *
 * レンタル画面呼び出し前に、受け取るデータ
 */
//--------------------------------------------------------------
void Factory_CommRecvBufRentalData(int id_no,int size,void *pData,void *work)
{
	int i,num;
	u8 m_max;
	FACTORY_WORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******ステージ****** レンタル情報受信\n" );

	num = 0;
	m_max = Factory_GetMinePokeNum( wk->type );
	//wk->recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	OS_Printf( "id_no = %d\n", id_no );
	OS_Printf( "size = %d\n", size );
	//OS_Printf( "pData[0] = %d\n", ((u8*)pData)[0] );
	OS_Printf( "recv_buf[0] = %d\n", recv_buf[0] );

	wk->pair_sel_count = recv_buf[0];
	OS_Printf( "受信：wk->pair_sel_count = %d\n", wk->pair_sel_count );
	num += 1;															//1
	
	//ポケモンナンバー
	for( i=0; i < wk->pair_sel_count ;i++ ){
		wk->pair_poke_monsno[i] = recv_buf[i+num];
		OS_Printf( "受信：wk->pair_poke_monsno[%d] = %d\n", i, wk->pair_poke_monsno[i] );
	}
	num += wk->pair_sel_count;											//3

	//ポケモンのパワー乱数
	for( i=0; i < wk->pair_sel_count ;i++ ){
		wk->pair_poke_sex[i] = recv_buf[i+num];
		OS_Printf( "受信：wk->pair_poke_sex[%d] = %d\n", i, wk->pair_poke_sex[i] );
	}
	num += wk->pair_sel_count;											//5
	
	//パートナーの選んだポケモンを表示リクエスト
	wk->pair_monsno_req = REQ_PAIR_MONSNO_DECIDE;

	return;
}

//--------------------------------------------------------------
/**
 * @brief   レンタルキャンセル 通信受信処理
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		FRONTIER_SYSTEMへのポインタ
 */
//--------------------------------------------------------------
void Factory_CommRecvBufCancelData(int id_no,int size,void *pData,void *work)
{
	OS_Printf( "未使用\n" );
	GF_ASSERT(0);
	return;
}

//受信したタイミングが、
//ポケモンステータスから画面復帰している時だと、
//BMPがまだ生成されていないためまずい！
static void Factory_PairMonsNoRecvReq( FACTORY_WORK* wk )
{
	int i;
	u8 m_max;

	m_max = Factory_GetMinePokeNum( wk->type );

	//ペアのデータを表示しないフラグ
	if( wk->pair_data_off_flag == 1 ){
		GF_BGL_BmpWinOff( &wk->bmpwin[BMPWIN_TR2] );
		GF_BGL_BmpWinOff( &wk->bmpwin[BMPWIN_POKE4] );
		GF_BGL_BmpWinOff( &wk->bmpwin[BMPWIN_POKE5] );
		return;
	}

	if( wk->pair_monsno_req == REQ_PAIR_MONSNO_DECIDE ){

		//パートナーの選んだポケモンを消す
		for( i=0; i < m_max ;i++ ){
			GF_BGL_BmpWinDataFill( &wk->bmpwin[BMPWIN_POKE4+i], FBMP_COL_NULL );	//塗りつぶし

			//「つよさを見るを選んだ時に表示が消えるので対処する」
			//パートナーの選んだポケモンを表示
			if( i < wk->pair_sel_count ){
				PokeNameWriteEx(wk, &wk->bmpwin[BMPWIN_POKE4+i],				//性別指定有り
								0, 0, 
								//FBMP_COL_BLACK, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, 
								FBMP_COL_WHITE, FBMP_COL_BLK_SDW, FBMP_COL_NULL, BF_FONT, 
								wk->pair_poke_monsno[i], wk->pair_poke_sex[i] );
			}

			GF_BGL_BmpWinOnVReq( &wk->bmpwin[BMPWIN_POKE4+i] );
		}

		PairNameWrite(wk, &wk->bmpwin[BMPWIN_TR2], 0, 0, BF_FONT );	//パートナー名を表示
	}

	wk->pair_monsno_req = REQ_PAIR_MONSNO_NONE;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	send_bufに交換したかをセット
 *
 * @param	wk		FACTORY_WORK型のポインタ
 * @param	sel		0=何もしていない(交換しなかった)、1=交換した
 *
 * @return	none
 */
//--------------------------------------------------------------
void Factory_CommSendBufTradeFlag( FACTORY_WORK* wk, u16 type, u16 trade_flag )
{
	wk->send_buf[0] = type;
	OS_Printf( "送信：type = %d\n", wk->send_buf[0] );

	wk->send_buf[1] = trade_flag;
	OS_Printf( "送信：trade_flag = %d\n", wk->send_buf[1] );

	wk->send_buf[2] = wk->sel_work[0];
	OS_Printf( "送信：sel_work[0] = %d\n", wk->send_buf[2] );

	wk->send_buf[3] = wk->sel_work[1];
	OS_Printf( "送信：sel_work[1] = %d\n", wk->send_buf[3] );
	return;
}

//--------------------------------------------------------------
/**
 * @brief   交換したか 通信受信処理
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		FRONTIER_SYSTEMへのポインタ
 *
 * wk->pair_trade_flag
 * 0=交換していない
 * 1=交換した
 */
//--------------------------------------------------------------
void Factory_CommRecvBufTradeFlag(int id_no,int size,void *pData,void *work)
{
	int i,num;
	FACTORY_WORK* wk = work;
	const u16* recv_buf = pData;

	OS_Printf( "******ステージ****** 交換したか情報受信\n" );

	num = 0;
	wk->trade_recieve_count++;

	//自分のデータは受け取らない
	if( CommGetCurrentID() == id_no ){
		return;
	}

	OS_Printf( "id_no = %d\n", id_no );

	//type = recv_buf[0];
	
	wk->pair_trade_flag = (u8)recv_buf[1];
	OS_Printf( "受信：wk->pair_trade_flag = %d\n", wk->pair_trade_flag );

	////////////////////////////////////////////////////////////////////////
	//親
	if( CommGetCurrentID() == COMM_PARENT_ID ){

		//親の決定がすでに決まっていたら、子の選択は無効
		if( wk->trade_flag == 1 ){
			wk->pair_trade_flag = 0;
		}else{

#if PL_T0868_080717_FIX
			//子機が選択していたら(交換中止の時は上書きしない)
			if( wk->pair_trade_flag == 1 ){
#endif
			wk->sel_work[0] = (u8)recv_buf[2];
			OS_Printf( "受信：wk->sel_work[0] = %d\n", wk->sel_work[0] );

			wk->sel_work[1] = (u8)recv_buf[3];
			OS_Printf( "受信：wk->sel_work[1] = %d\n", wk->sel_work[1] );
#if PL_T0868_080717_FIX
			}
#endif

			//親の決定が決まっていない時は、
			//親が送信する時に「子にそれでいいよ」と送信する
			
			//子の選択が採用されたことがわかるようにオフセットを加える
			//wk->parent_decide_pos	= wk->pair_sel_pos + wk->hv_max;
		}
	////////////////////////////////////////////////////////////////////////
	//子
	}else{
		//親の決定タイプ
		//wk->parent_decide_pos = recv_buf[2];
		
		//親が交換した情報を優先して、子の交換した情報はクリアする
		if( wk->pair_trade_flag == 1 ){
			wk->trade_flag = 0;

			wk->sel_work[0] = (u8)recv_buf[2];
			OS_Printf( "受信：wk->sel_work[0] = %d\n", wk->sel_work[0] );

			wk->sel_work[1] = (u8)recv_buf[3];
			OS_Printf( "受信：wk->sel_work[1] = %d\n", wk->sel_work[1] );
		}
	}

	return;
}


//==============================================================================================
//
//	共通処理
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	ボール追加
 *
 * @param	wk			FACTORY_WORK型のポインタ
 * @param	no			テーブルの何番目か
 *
 * @retval	none
 */
//--------------------------------------------------------------
static FACTORY_BALL* Factory_BallAdd( FACTORY_WORK* wk, int no )
{
	int x,y;

	//レンタル(通信は関係ない)
	if( Factory_CheckMode(wk,FACTORY_MODE_RENTAL) == TRUE ){
		x = bf_ball_pos[no][0];										//レンタルのボール
		y = bf_ball_pos[no][1]; 
	}else{

		//交換

		//通信でない時
		if( Factory_CommCheck(wk->type) == FALSE ){
			x = bf_t_ball_pos[no][0];								//てばなす、うけとるのボール
			y = bf_t_ball_pos[no][1]; 
		}else{

			//通信の時
			if( wk->ball_max == BF_T_MULTI_BALL_TEBANASU_POS_MAX ){
				x = bf_t_multi_ball_tebanasu_pos[no][0];			//てばなす時のボール
				y = bf_t_multi_ball_tebanasu_pos[no][1];
			}else{
				x = bf_t_multi_ball_uketoru_pos[no][0];				//うけとる時のボール
				y = bf_t_multi_ball_uketoru_pos[no][1];
			}
		}
	}

	return FactoryBall_Create( &wk->factory_clact, x, y, HEAPID_FACTORY );
}

//--------------------------------------------------------------
/**
 * @brief	左上のウィンドウ表示
 *
 * @param	wk			FACTORY_WORK型のポインタ
 * @param	no			テーブルの何番目か
 *
 * @retval	none
 */
//--------------------------------------------------------------
static FACTORY_OBJ* Factory_ObjWinAdd( FACTORY_WORK* wk, u32 anm_no )
{
	int x, y;

	switch( anm_no ){

	//シングル：開く、閉じる
	case ANM_SINGLE_OPEN:
	case ANM_SINGLE_CLOSE:
		x = FACTORY_OBJ_SINGLE_WIN_X;
		y = FACTORY_OBJ_SINGLE_WIN_Y;
		break;

	//シングル：(決定ポケモン3匹)開く、閉じる
	case ANM_3POKE_OPEN:
	case ANM_3POKE_CLOSE:
		x = FACTORY_OBJ_3POKE_WIN_X;
		y = FACTORY_OBJ_3POKE_WIN_Y;
		break;

	//マルチ：(決定ポケモン3匹)開く、閉じる
	case ANM_MULTI_2POKE_OPEN:
		x = FACTORY_OBJ_2POKE_WIN_X_OPEN;
		y = FACTORY_OBJ_2POKE_WIN_Y;
		break;

	case ANM_MULTI_2POKE_CLOSE:
		x = FACTORY_OBJ_2POKE_WIN_X_CLOSE;
		y = FACTORY_OBJ_2POKE_WIN_Y;
		break;
	};

	return FactoryObj_Create( &wk->factory_clact, anm_no, x, y, HEAPID_FACTORY );
}

//--------------------------------------------------------------
/**
 * @brief	通信専用：右上のウィンドウ表示
 *
 * @param	wk			FACTORY_WORK型のポインタ
 * @param	no			テーブルの何番目か
 *
 * @retval	none
 */
//--------------------------------------------------------------
static FACTORY_OBJ* Factory_PairObjWinAdd( FACTORY_WORK* wk, u32 anm_no )
{
	int x, y;

	switch( anm_no ){

	//マルチ：開く、閉じる
	case ANM_MULTI_OPEN:
	case ANM_MULTI_CLOSE:
		x = FACTORY_OBJ_MULTI_WIN_X;
		y = FACTORY_OBJ_MULTI_WIN_Y;
		break;

	default:
		OS_Printf( "不正なアニメナンバーが渡されました！\n" );
		GF_ASSERT(0);
		x = 0;
		y = 0;
		break;
	};

	return FactoryObj_Create( &wk->factory_clact, anm_no, x, y, HEAPID_FACTORY );
}

//--------------------------------------------------------------
/**
 * @brief	OBJウィンドウの削除
 *
 * @param	wk			FACTORY_WORK型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void Factory_ObjWinDel( FACTORY_WORK* wk )
{
	if( wk->p_obj_win != NULL ){
		FactoryObj_Delete( wk->p_obj_win );								//OBJウィンドウ削除
		wk->p_obj_win = NULL;
	}

	if( wk->p_pair_obj_win != NULL ){
		FactoryObj_Delete( wk->p_pair_obj_win );						//OBJウィンドウ削除
		wk->p_pair_obj_win = NULL;
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	BF_FRAME_WINをBMPに設定
 *
 * @param	wk			FACTORY_WORK型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void Factory_FrameWinBmpSet( FACTORY_WORK* wk )
{
	GF_BGL_PrioritySet( BF_FRAME_WIN, 1 );	//右上の円のプライオリティを上げる
	GF_BGL_ClearCharSet( BF_FRAME_WIN, 32, 0, HEAPID_FACTORY );
	GF_BGL_ScrClear( wk->bgl, BF_FRAME_WIN );
	FactoryAddBmpWin( wk->bgl, wk->bmpwin );							//ビットマップ追加
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );			//BG表示
	return;
}

//--------------------------------------------------------------
/**
 * @brief	BF_FRAME_WINをBMPに設定
 *
 * @param	wk			FACTORY_WORK型のポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void BmpMenuWinClearSub( GF_BGL_BMPWIN* win )
{
	BmpMenuWinClear( win, WINDOW_TRANS_OFF );
	GF_BGL_BmpWinOffVReq( win );
	return;
}


